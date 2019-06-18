#include "unp.h"
#include "cyx_error.h"
#include "cyx_util.h"
/*
*
*description:并发客户端
*
*
*/

int main(int argc,char* argv[]){
	if(argc != 2)
		err_quit("usage:tcpcli <IPaddress>");
	int connfd[5];
	struct sockaddr_in servaddr;
	for(int i = 0;i < 5;++i){
		connfd[i] = Socket(AF_INET,SOCK_STREAM,0);
		bzero(&servaddr,sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(ECHO_PORT);
		inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
		Connect(connfd[i],(SA*)&servaddr,sizeof(servaddr));
		}
	str_cli(stdin,connfd[0]);
	exit(0);
	}
