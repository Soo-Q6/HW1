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
//#define SERV_PORT 8888
#define MAXLINE 2048
void upload(const char*fp, int sockfd);
void download(const char* filename,int sockfd);
void ls(int sockfd);
int Iscmd(char cmd[10]);
void cmd_Up(int sockfd,char str[10], char strname[20],char *path);
int main(int argc, char **argv) {
	int sockfd;                        //should be socket[5]
	struct sockaddr_in servaddr;
	int SERV_PORT;
	if(argc!=3){
	printf("usefjds");
	exit(0);
	}
	SERV_PORT = atoi(argv[2]);
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd<0)
		{
			printf("secket error :%s (errno:%d)\n", strerror(errno), errno);
			exit(0);
		}
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = PF_INET;                   //change from AF_INET to PF_INET
		servaddr.sin_port = htons(SERV_PORT);
		//inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
		//inet_pton(AF_INET, "192.168.50.93", &servaddr.sin_addr);
		inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

		if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
			printf("connect error :%s (errno:%d)\n", strerror(errno), errno);
			exit(0);
		}
		
		char path[100]={'\0'};
		int n=recv(sockfd, path, 100,0);   //接受服务器当前地址 done.
		
		while (1)
		{
			printf("%s/>", path);
			char str[10];
			char strname[20];
			scanf("%s", str);
			send(sockfd, str, 10, 0);           //传指令 done.
			if (strcmp(str, "ls") == 0)
			{
				ls(sockfd);
			}
			else if (Iscmd(str))
			{
				scanf("%s", strname);
				cmd_Up(sockfd,str, strname,path);
			}
			else if (strcmp(str,"exit")==0)
			{
				exit(0);
			}
			else
			{
				printf("commander wrong!\n");
			}
		}
		return 0;
}



void download(const char* filename, int sockfd) {
	
	char recvline[MAXLINE];
	int n;
	FILE* fp = fopen(filename, "w");
	if (fp==NULL)
	{
		printf("open file error\n");
		exit(0);
	}
	again:
	while ((n=read(sockfd,recvline,MAXLINE))==MAXLINE)    
	{
		//fputs(recvline, stdout);
		fwrite(recvline, 1, n, fp);
	}
	if (n>1)
	{
		//fputs(recvline, stdout);
		if (strcmp(recvline,"error")==0)
		{
			printf("no such file!\n");
			remove(filename);
			return;
		}
		else
			fwrite(recvline, 1, n, fp);
	}
	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		printf("read error");
	else
		printf("Download Complete!\n");
	fclose(fp);
	return;
}



void upload(const char* filename, int sockfd) {
	FILE *fp;
	ssize_t n;
	char buf[MAXLINE];
	const char error[6] = "error";
	if ((fp = fopen(filename, "r")) == NULL){
		printf("cannot open file!\n");
		write(sockfd, error, sizeof(error));
		return;
		//exit(0);
	}
	again:
	while ((n = fread(buf, 1, MAXLINE, fp))>0) {
		write(sockfd, buf, n);
	}
	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		printf("read error");
	else
		printf("Upload Complete!\n");
	write(sockfd, buf, 1);
	fclose(fp);
	return;
}

/**
get the content of the server's current path
*/
void ls(int sockfd) {
	char recvline[100] = { '\0' };
	int n=100;
	for(;n==100;)
	{
		n=read(sockfd, recvline, 100);
		if (n==100)
		{
			printf("%s\t", recvline);
		}
		else if(n==2)
		{
			printf("cannot open this direactory!\n");
		}
		else
		{
			printf("\n");
		}
	}
	return;
}

int Iscmd(char cmd[10]){
	if (!strcmp(cmd, "cd") || !strcmp(cmd, "mkdir") || !strcmp(cmd, "download") || !strcmp(cmd, "upload"))
		return 1;
	else
		return 0;
}

void cmd_Up(int sockfd,char str[10], char strname[20],char* path) {
	if (strcmp(str, "cd") == 0) {
		int n;
		char recvline[100];
		send(sockfd, strname, 20, 0);
		n=read(sockfd, recvline, 100);
		if (n==1)
		{
			printf("new path read error!\n");
		}
		else
		{
			strcpy(path, recvline);
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
	}
	else if(strcmp(str,"mkdir")==0)
	{
		printf("%s %s\n", str, strname);
		send(sockfd, strname, 20, 0);
		printf("create dir %s successfuly\n", strname);
		return;
	}
	else
	{
		printf("error\n");
	}
}

