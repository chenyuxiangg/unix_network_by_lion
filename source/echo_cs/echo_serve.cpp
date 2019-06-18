#include "unp.h"
#include "cyx_error.h"
#include "cyx_util.h"

/*
*
*description:该程序实现了简单的回射服务器，并解决了以下三个问题：
*1、当fork子进程时，必须捕获SIGCHLD信号
*2、当捕获信号时，必须处理被中断的系统调用
*3、使用waitpid避免遗留僵死进程
*/

void sig_chld(int signo){
	pid_t pid;
	int stat;

	//使用wait的版本不足以处理并发服务器的需求，因此改用waitpid
	//pid = wait(&stat);
	while((pid = waitpid(-1,&stat,WNOHANG)) > 0)
		printf("child %d terminated\n",pid);
	return;
	}

int main(int argc,char* argv[]){
	int listenfd,clientfd;
	socklen_t clientlen;
	pid_t childpid;
	struct sockaddr_in servaddr,clitaddr;

	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(ECHO_PORT);
	
	Bind(listenfd,(SA*)&servaddr,sizeof(servaddr));
	Listen(listenfd,LISTENQ);

	//当使用fork子进程时，必须捕捉SIGCHLD信号
	sigconnect(SIGCHLD,sig_chld);

	while(true){
		clientlen = sizeof(clitaddr);
		//clientfd = Accept(listenfd,(SA*)&clitaddr,&clientlen);
		//为了兼容本身不支持重启慢系统调用的系统，改为下：
		clientfd = accept(listenfd,(SA*)&clitaddr,&clientlen);
		if(clientfd < 0){
			if(errno == EINTR)
				continue;
			else
				err_sys("accept error");
			}
		if((childpid = fork()) == 0){
			//子进程
			Close(listenfd);
			str_echo(clientfd);
			exit(0);
			}
		//父进程
		Close(clientfd);
		}
	}
