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

void download(const char* filename, int sockfd) {
	FILE *fp;
	ssize_t n;
	const char error[6] = "error";
	char buf[MAXLINE];
	int childsockfd;

	if ((fp = fopen(filename, "r")) == NULL) {
		printf("cannot open file!\n");
		write(sockfd, error, sizeof(error));
		//exit(0);
		return;
	}
again:
	while ((n = fread(buf, 1, MAXLINE, fp)) > 0) {
		write(sockfd, buf, n);
		//printf("the fread count:%d\n", n);
	}
	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		printf("str_echo:read error");
	//printf("done!\n"); 
	write(sockfd, buf, 1);
	fclose(fp);
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
again:
	while ((n = read(sockfd, recvline, MAXLINE)) == MAXLINE)
	{
		fwrite(recvline, 1, n, fp);
	}
	if (n>1)
	{
		if (strcmp(recvline, "error") == 0)
		{
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



void ls(char* path, int connfd) {
	struct dirent* ent = NULL;
	DIR *pDir;
	char sendline[100] = { '\0' };
	if ((pDir = opendir(path)) == NULL)
	{
		printf("cannot open direactory.");
		write(connfd, sendline, 2);
		return;
	}
	while ((ent = readdir(pDir)) != NULL)
	{
		strcpy(sendline, ent->d_name);
		if (write(connfd, sendline, sizeof(sendline)) < 0)
		{
			printf("write error: %s (errno:%d)", strerror(errno), errno);
			exit(0);
		}
	}
	write(connfd, sendline, 1);
	closedir(pDir);
	return;
}

int Iscmd(char cmd[10])
{
	if (!strcmp(cmd, "cd") || !strcmp(cmd, "mkdir") || !strcmp(cmd, "download") || !strcmp(cmd, "upload"))
		return 1;
	else
		return 0;
}

void cmd_Up(int connfd, char str[10], char strname[20], char* path) {
	if (strcmp(str, "cd") == 0) {
		char path_tmp[PATH_LENGTH];
		strcpy(path_tmp, changedir(strname));
		printf("%s\n", path_tmp);
		if (strcmp(path_tmp, "error") == 0) {
			write(connfd, path_tmp, 1);
		}
		else {
			strcpy(path, path_tmp);
			printf("the new path is %s %lu\n", path, strlen(path));
			int n = write(connfd, path, strlen(path));
			printf("the writing length is :%d\n", n);
		}
		return;
	}
	else if (strcmp(str, "download") == 0)
	{
		printf("%s %s\n", str, strname);
		download(strname, connfd);
		return;

	}
	else if (strcmp(str, "upload") == 0)
	{
		upload(strname, connfd);
		return;
	}
	else if (strcmp(str, "mkdir") == 0)
	{
		mkdir(strname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
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
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child:%d terminated\n", pid);
	return;
}


char* changedir(const char* strname) {
	int n;
	char path[PATH_LENGTH];
	n = chdir(strname);
	if (n != 0)
	{
		printf("change dir error: %s (errno:%d)\n", strerror(errno), errno);
		return "error";
	}
	getcwd(path, PATH_LENGTH);
	return path;
}