#include<sys/socket.h>
#include<wait.h>
#include<string.h>
#include<unistd.h>   
#include<signal.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<errno.h>
#include"ser.h"

int main(int argc,char **argv)
{
	int listenid, connfd;
	pid_t childpid;
	socklen_t clilen;
	int serv_port;
	//char* path;
	struct sockaddr_in cliaddr, servaddr;
	if (argc != 2) {
		printf("usefjds");
		exit(0);
	}
	serv_port = atoi(argv[1]);

	if ((listenid = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("socket errro");
		exit(0);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = PF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(serv_port);

	if (bind(listenid, (const struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
		printf("bind:%s(errno:%d)\n", strerror(errno), errno);
		exit(0);
	}

	if (listen(listenid, LISTENQ) == -1) {
		printf("listen:%s(errno:%d)\n", strerror(errno), errno);
		exit(0);
	}else 
	{
		printf("waiting for new connection\n");
	}
	

	if (signal(SIGCHLD, sig_chid) < 0) {
		printf("%s,%d", strerror(errno), errno);
	}

	while (1) {
		clilen = sizeof(cliaddr);
		connfd = accept(listenid, (struct sockaddr*)&cliaddr, &clilen);
		if (connfd < 0) {
			if (errno == EINTR)
				continue;
			else {
				printf("accept error:%s (errno:%d)\n", strerror(errno), errno);
				exit(0);
			}
		}

		getpeername(connfd, (struct sockaddr*)&cliaddr, &clilen);
		printf("connection form:%s  port:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
		if ((childpid = fork()) == 0) {
			close(listenid);
			char path[PATH_LENGTH];
			getcwd(path, sizeof(path));
			send(connfd, path, strlen(path), 0);      //发送当前地址 done.

			while (1)
			{
				char str[10] = { '\0' };
				char strname[20] = { '\0' };
				recv(connfd, str, 10, 0);         //获取指令 done.
				if (strcmp(str, "ls") == 0)
				{
					ls(path, connfd);
				}
				else if (strcmp(str, "exit") == 0)
				{
					printf("disconnection form:%s  port:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
					close(connfd);
					exit(0);
					break;
				}
				else if (Iscmd(str))
				{
					recv(connfd, strname, 20, 0);
					cmd_Up(connfd, str, strname, path);
				}
				else
				{
					printf("commander wrong!\n");
				}
			}
		}

	}
	return 0;
}






