#include<sys/socket.h>
#include<wait.h>
#include<string.h>
#include<unistd.h>   
#include<signal.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<errno.h>
#define serv_port 8888
#define LISTENQ 1024
#define MAXLINE 2048
#define PATH_LENGTH 100
//void str_echo(int sockfd);
void download(const char* filename, int sockfd);
void upload(const char* filename, int sockdf);
void sig_chid(int signo);
void ls(char* path,int connfd);
char* changedir(const char* path);
int Iscmd(char cmd[10]);
void cmd_Up(int connfd, char str[10], char strname[20],char* path);
int main()
{
	int listenid, connfd;
	pid_t childpid;
	socklen_t clilen;
	//char* path;
	struct sockaddr_in cliaddr, servaddr;

	if ((listenid = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("socket errro");
		exit(0);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(serv_port);

	if (bind(listenid, (const struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
		printf("bind:%s(errno:%d)\n", strerror(errno), errno);
		exit(0);
	}

	if (listen(listenid, LISTENQ) == -1) {
		printf("listen:%s(errno:%d)\n", strerror(errno), errno);
		exit(0);
	}
	else {
		printf("waiting for new connection\n");
	}

	if (signal(SIGCHLD, sig_chid)<0) {
		printf("%s,%d", strerror(errno), errno);
	}

	while (1) {
		clilen = sizeof(cliaddr);
		connfd = accept(listenid, (struct sockaddr*)&cliaddr, &clilen);
		//printf("%d\n", connfd);
		if (connfd <0) {
			if (errno == EINTR)
				continue;
			else {
				printf("accept error:%s (errno:%d)\n", strerror(errno), errno);
				exit(0);
			}
		}

		getpeername(connfd,(struct sockaddr*)&cliaddr,&clilen);
		printf("connection form:%s  port:%d\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));
		if ((childpid = fork()) == 0) {
			close(listenid);

			// //char *path = getcwd(NULL,NULL);
			 char path[PATH_LENGTH];
			 getcwd(path,sizeof(path));
			send(connfd, path, strlen(path),0);      //发送当前地址 done.
			
			while (1)
			{
				printf("waiting for new commander!\n");
				char str[10] = { '\0' };
				char strname[20] = { '\0' };
				recv(connfd, str, 10, 0);         //获取指令 done.
				printf("cmd is %s\n", str);
				if (strcmp(str, "ls") == 0)
				{
					ls(path,connfd);
					//printf(" done1\n");
				}
				else if (strcmp(str, "exit") == 0)
				{
					printf("disconnection form:%s  port:%d\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));
					close(connfd);
					exit(0);
					break;
					//return 0;
				}
				else if (Iscmd(str))
				{
					recv(connfd, strname, 20, 0);
					printf("strname is %s\n", strname);
					cmd_Up(connfd,str, strname,path);
					printf("after cme_up, the new path is:%s\n",path);
				}
				else
				{
					printf("commander wrong!\n");
				}
			}
			//str_echo(connfd);
			//path=changedir("Debug");
			//ls(path,connfd);
			//download("text", connfd);
			//exit(0);
		}
		//printf("childpid:%d\n", childpid);
		//close(connfd);

	}
	return 0;
}


void download(const char* filename,int sockfd) {
	FILE *fp;
	ssize_t n;
	char buf[MAXLINE];
	//struct sockaddr_in clientaddr;
	//socklen_t clientLen = sizeof(clientaddr);
	if ((fp=fopen(filename,"r"))==NULL){
		printf("cannot open file!");
		exit(0);
	}
	again:
	while ((n = fread(buf,1,MAXLINE,fp))>0) {
		//printf("%d %s", n, buf);
		//buf[n]='\0';
		write(sockfd, buf, n);
		//getpeername(sockfd,(struct sockaddr*)&clientaddr,&clientLen);
		//printf("connection form:%s  port:%d\n",inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));

	}
	fclose(fp);
	if (n<0 && errno == EINTR)
		goto again;
	else if (n<0)
		printf("str_echo:read error");
	//exit(0);
	return;
}


void upload(const char* filename, int sockfd) {
	char recvline[MAXLINE];
	FILE* fp = fopen(filename, "w");
	ssize_t n;
	if (fp == NULL)
	{
		printf("open file error\n");
		exit(0);
	}
	while ((n = read(sockfd, recvline, MAXLINE)) != 0)
	{
		//fputs(recvline, stdout);
		//printf("%d", /*strlen(recvline)*/n);
		fwrite(recvline, 1, /*strlen(recvline)*/n, fp);
	}
	fclose(fp);
	//exit(0);
	return;
}


/**
display the files of the current path from server, 
send the contents to client.
*/
void ls( char* path,int connfd) {
	struct dirent* ent = NULL;
	DIR *pDir;
	//printf("the ls wait starting\n");
  
	if ((pDir = opendir(path)) == NULL)
	{
		printf("cannot open direactory.");
		exit(0);
		return ;
	} 
	else
	{
		printf("open %s successfully\n", path);
	}


	int n;
	//printf("the ls starting\n");
	while ((ent = readdir(pDir)) != NULL)
	{ 
		printf("%s  %lu", ent->d_name, strlen(ent->d_name));
		//n=write(connfd, ent->d_name , strlen(ent->d_name));
		//n=write(connfd,)
		//printf("%d\n", n);
		if (write(connfd, ent->d_name , strlen(ent->d_name))<0)
		{
			printf("write error: %s (errno:%d)", strerror(errno), errno);
			exit(0);
		}
	}
	//printf("done\n");  
	closedir(pDir);
	//exit(0);
	return;
}

int Iscmd(char cmd[10])
{
	//	int i;
	if (!strcmp(cmd, "cd") || !strcmp(cmd, "cdir") || !strcmp(cmd, "download") || !strcmp(cmd, "upload"))
		return 1;
	else
		return 0;
}

void cmd_Up(int connfd, char str[10], char strname[20],char* path) {
	if (strcmp(str, "cd") == 0) {
		char path_tmp[PATH_LENGTH];
		strcpy(path_tmp,changedir(strname));
		//path_tmp=changedir(strname);
		if(path_tmp==NULL){
			printf("error:%lu\n",strlen(path_tmp));
			write(connfd,path_tmp,strlen(path_tmp));
		}
		else{
			strcpy(path,changedir(strname));
			//printf("the new path is %s %lu\n", path,strlen(path));
			int n=write(connfd, path, strlen(path));
			//printf("the writing length is :%d\n",n);
		}
		return;
	}
	else if (strcmp(str, "download") == 0)
	{
		//printf("%s %s\n", str, strname);
		//recv(connfd, str, 10, 0);
		download(strname, connfd);
		return;

	}
	else if (strcmp(str, "upload") == 0)
	{
		//printf("%s %s\n", str, strname);
		//recv(connfd, str, 10, 0);
		upload(strname, connfd);
		return;
	}
	else
	{
		printf("error:");
		return;
	}

}

void sig_chid(int signo) {
	pid_t pid;
	int stat;
	while ((pid = waitpid(-1, &stat, WNOHANG))>0)
		//pid=waitpid(&stat);
		printf("child:%d terminated", pid);
	return;
}


char* changedir(const char* strname) {
	int n;
	char path[PATH_LENGTH];
	n=chdir(strname);
	if (n!=0)
	{
		printf("change dir error: %s (errno:%d)\n", strerror(errno), errno);
		return NULL;
		//exit(0);
	}
	getcwd(path, PATH_LENGTH);
	return path;
}



