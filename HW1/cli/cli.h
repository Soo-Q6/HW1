#ifndef CLIENT_H
#define CLIENT_H

#define MAXLINE 2048
/**
@client
select a file and send it to the socket
*/
void upload(const char*fp, int sockfd);
/**
@client
download a file from server
*/
void download(const char* filename, int sockfd);

/**
@client
get the content of the server's current path
*/
void ls(int sockfd);
/**
@client
to find whether cmd is a right commander or not
include cd, download, upload, mkdir
*/
int Iscmd(char cmd[10]);
/**
@client
if a commander need a object to handle
include cd, download, upload, mkdir
*/
void cmd_Up(int sockfd, char str[10], char strname[20], char *path);

#endif // !CLIENT_H

