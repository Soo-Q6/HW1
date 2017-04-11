#include<sys/types.h>
#include<sys/socket.h>
#include<time.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<arpa/inet.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#define SERV_PORT 8888
#define MAXLINE 2048
void str_cli(FILE*fp, int sockfd);
void upload(const char*fp, int sockfd);
void download(const char* filename,int sockfd);
void ls(int sockfd);
int Iscmd(char cmd[10]);
void cmd_Up(int sockfd,char str[10], char strname[20],char *path);
int main(int argc, char **argv) {
	int sockfd;                        //should be socket[5]
	struct sockaddr_in servaddr;
	/*if(argc!=2){
	printf("usefjds");
	exit(0);
	}*/
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd<0)
		{
			printf("secket error :%s (errno:%d)\n", strerror(errno), errno);
			exit(0);
		}
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = PF_INET;                   //change from AF_INET to PF_INET
		servaddr.sin_port = htons(SERV_PORT);
		inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

		if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
			printf("connect error :%s (errno:%d)\n", strerror(errno), errno);
			exit(0);
		}
		
		char path[100];
		int n=recv(sockfd, path, 100,0);   //接受服务器当前地址 done.
		
		while (1)
		{
			printf("%s>", path);
			char str[10];
			char strname[20];
			scanf("%s", &str);
			//write(sockfd, str, 10);
			//printf("the str is %s\n", str);
			send(sockfd, str, 10, 0);           //传指令 done.
			if (strcmp(str, "ls") == 0)
			{
				//ls(sockfd);
				//printf("ls  %s\n", str);
				ls(sockfd);
				printf("ls done??\n");
			}
			else if (Iscmd(str))
			{
				scanf("%s", &strname);
				cmd_Up(sockfd,str, strname,path);
			}
			else if (strcmp(str,"exit")==0)
			{
				//send(sockfd, str, 10, 0);
				//return 0;
				exit(0);
			}
			else
			{
				printf("commander wrong!\n");
			}
		}



		//ls(sockfd);
		//download("text_download", sockfd);
		//exit(0);
		return 0;
}



void download(const char* filename, int sockfd) {
	
	char recvline[MAXLINE];
	//while (fgets(sendline, MAXLINE, fp) != NULL) {
	//	write(sockfd, sendline, strlen(sendline));
	//	if (read(sockfd, recvline, strlen(sendline)) == 0) {
	//		printf("sre_cli:error");
	//		exit(0);

	//	}
	//	fputs(recvline, stdout);
	//}
	FILE* fp = fopen(filename, "w");
	if (fp==NULL)
	{
		printf("open file error\n");
		exit(0);
	}
	ssize_t n;
	again:
	while ((n=read(sockfd,recvline,MAXLINE))!=0)
	{
		//fputs(recvline, stdout);
		//printf("%d", /*strlen(recvline)*/n);
		//printf("%s\n", recvline);
		fwrite(recvline, 1, /*strlen(recvline)*/n, fp);
	}
	printf("%d\n",n);
	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		printf("read error");
	else
		printf("Download Complete!\n");
	fclose(fp);
	//exit(0);
	return;
}



void upload(const char* filename, int sockfd) {
	FILE *fp;
	ssize_t n;
	char buf[MAXLINE];
	if ((fp = fopen(filename, "r")) == NULL)
	{
		printf("cannot open file!");
		exit(0);
	}
again:
	while ((n = fread(buf, 1, MAXLINE, fp))>0) {
		write(sockfd, buf, n);
	}
	fclose(fp);
	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		printf("read error");
	else
		printf("Upload Complete!\n");
	//exit(0);
	return;
}

/**
get the content of the server's current path
*/
void ls(int sockfd) {
	char recvline[100];
	int n;
	//printf("comeing\n");
	while ((n=recv(sockfd, recvline, /*strlen(recvline)+1)*/100,0)) > 0) {
		printf("%s", recvline);
	}
	printf("%d\n", n);
	if (n<0)
	{
		printf("read error: %s (errno:%d)\n", strerror(errno), errno);
	}
	printf("\n");
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

void cmd_Up(int sockfd,char str[10], char strname[20],char* path) {
	if (strcmp(str, "cd") == 0) {
		int n;
		send(sockfd, strname, 20, 0);
		//printf("%s %s\n", str, strname);
		n=read(sockfd, path, strlen(path));
		if (n<0)
		{
			printf("new path read error!\n");
		}
		else
		{
			printf("the new path is %s\n", path);
		}
		return;
	}
	else if (strcmp(str,"download")==0)
	{
		printf("%s %s\n", str, strname);
		send(sockfd, strname, 20, 0);
		download(strname, sockfd);
		return;

	}
	else if (strcmp(str,"upload")==0)
	{
		printf("%s %s\n", str, strname);
		send(sockfd, strname, 20, 0);
		upload(strname, sockfd);
		return;
	}/*
	else
	{
		printf("error:");
		return;
	}*/

}

