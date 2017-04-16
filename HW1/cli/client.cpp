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
#include"cli.h"

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





