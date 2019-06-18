#include "unp.h"
#include "cyx_wrapper.h"
#include <time.h>

int main(int argc,char* argv[]){
	int listenfd,connfd;

	char buf[MAXLINE];
	struct sockaddr_in servaddr;
	time_t ticks;

	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(8050);

	Bind(listenfd,(SA*)&servaddr,sizeof(servaddr));

	Listen(listenfd,LISTENQ);

	while(true){
		connfd = Accept(listenfd,(SA*)NULL,NULL);
		printf("client has accepted\n");
		ticks = time(NULL);
		snprintf(buf,sizeof(buf),"%.24s\r\n",ctime(&ticks));
		Write(connfd,buf,strlen(buf));
		Close(connfd);
		}
	return 0;
	}
