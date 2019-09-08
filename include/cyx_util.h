#include "unp.h"
#include "cyx_error.h"

int Socket(int domain,int type,int protocol){
	int n;
	if((n = socket(domain,type,protocol)) < 0)
		err_sys("socket error");
	return n;
	}

int Bind(int sockfd,const struct sockaddr* addr,socklen_t addrlen){
	int n;
	if((n = bind(sockfd,addr,addrlen)) < 0)
		err_sys("bind error");
	return n;
	}

int Close(int fd){
	int n;
	if((n = close(fd)) < 0)
		err_sys("close error");
	return n;
	}

int Listen(int sockfd,int backlog){
	int n;
	if((n = listen(sockfd,backlog)) < 0)
		err_sys("listen error");
	return n;
	}

int Connect(int sockfd,const struct sockaddr* addr,socklen_t addrlen){
	int n;
	if((n = connect(sockfd,addr,addrlen)) < 0)
		err_sys("connect error");
	return n;
	}

int Accept(int sockfd,struct sockaddr* addr,socklen_t* addrlen){
	int n;
	if((n = accept(sockfd,addr,addrlen)) < 0)
		err_sys("accept error");
	return n;
	}

ssize_t Read(int fd,void* buf,size_t count){
	ssize_t n;
	if((n = read(fd,buf,count)) < 0)
		err_sys("read error");
	return n;
	}

ssize_t Write(int fd,const void* buf,size_t count){
	ssize_t n;
	if((n = write(fd,buf,count)) < 0)
		err_sys("write error");
	return n;
	}

int max(const int& a,const int& b){
	return (a >= b ? a : b);
	}

/*
*
*================writen================
*
*description:用于字节流套接字上的write函数，能够满足一次性读入所需字符的数量，该函数解决了普通write函数输出字节数可能少于请求的字节数的问题。
*
*return:写入成功返回写入字符数，否则返回-1
*
*arg:
*fd(int):文件描述符，指向待写入的文件
*vprt(const void*):指向常量的指针，所指即为写入文件的字符的来源
*n(size_t):准备写入字符的个数
*
*
*/

ssize_t writen(int fd,const void* vptr,size_t n){
	size_t nleft;
	ssize_t nwritten;
	const char* ptr;

	ptr = (const char*)vptr;
	nleft = n;

	while(nleft > 0){
		if((nwritten = write(fd,ptr,nleft)) <= 0){
			if(nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return -1;
			}

		nleft -= nwritten;
		ptr += nwritten;
		}
	return n;
	}
/*
*
*===================readline=================
*
*desctiption:按字符读取一行，当读到maxlen长度的内容或者读到'\n'时返回
*
*return:返回读取到的字符个数，如果出错则返回-1
*
*arg:
*fd(int):文件描述符，待读取的文件
*vptr(void*):指向存取读取字符串的指针，读取成功后，该指针所指的内容即为读取的内容
*maxlen(size_t):最大读取长度，若文件中无换行符，则读取到指定maxlen数量的字符时返回
*/
ssize_t readline(int fd,void* vptr,size_t maxlen){
	ssize_t n,rc;
	char c,*ptr;
	ptr = (char*)vptr;

	for(n = 1;n < maxlen;n++){
		if((rc = read(fd,&c,1)) == 1){
			*ptr++ = c;
			if(c == '\n')
				break;
			}
		else if(rc == 0){
			*ptr = 0;
			return (n-1);
			}
		else{
			if(errno == EINTR)
				continue;
			return -1;
			}
		}
	*ptr = 0;
	return n;
	}

/*
*
*===================Readline=====================
*
*description:具有提示功能的readline
*
*返回值及参数同readline
*/
ssize_t Readline(int fd,void* vptr,size_t maxlen){
	ssize_t n;
	if((n = readline(fd,vptr,maxlen)) < 0)
		err_sys("readline error");
	return n;
	}

/*
*
*=================Writen=================
*
*description:具有错误提示的writen
*
*返回值及参数同writen
*
*/

ssize_t Writen(int fd,const void* vptr,size_t n){
	ssize_t len;
	if((len = writen(fd,vptr,n)) < 0)
		err_sys("writen error");
	return n;
	}

/*
*=================Select===================
*
*description:select的带错误提示版
*
*参数、返回值与select函数相同
*
*/
int Select(int nfds,fd_set* readfds,fd_set* writefds,fd_set* exceptfds,struct timeval* timeout){
	int n;
	if((n = select(nfds,readfds,writefds,exceptfds,timeout)) < 0)
		err_sys("select error");
	}

/*
*==================str_echo=================
*
*description:用于回射服务器，获取到从客户机发送的消息并原样返回给客户机
*
*return:void
*
*arg:
*fd(int):文件描述符，与客户端连接的套接字
*/

void str_echo(int fd){
	ssize_t n;
	char buf[MAXLINE];

	while(true){
		if((n = read(fd,buf,MAXLINE)) > 0){
			Writen(fd,buf,n);
			}
		else if(n < 0 && errno == EINTR)
			continue;
		else if(n == 0)
			break;
		else{
			err_sys("str_echo:read error");
			}
		}
		return;
	}

/*
*==================str_cli=================
*
*description:用于回射客户机，接收服务器返回的消息并显示在标准输出中
*
*return:void
*
*arg:
*fp(FILE*):字符串源，可能为标准输入
*sockfd(int):文件描述符，与服务端连接的套接字
*/

void str_cli(FILE* fp,int sockfd){
	fd_set rset;
	char recvline[MAXLINE],sendline[MAXLINE];
	FD_ZERO(&rset);

	int maxfdl;
	while(true){
		FD_SET(fileno(fp),&rset);
		FD_SET(sockfd,&rset);
		maxfdl = max(fileno(fp),sockfd) + 1;
		Select(maxfdl,&rset,NULL,NULL,NULL);
		
		if(FD_ISSET(fileno(fp),&rset)){
			if(fgets(sendline,MAXLINE,fp) == NULL)
				return;
			Writen(sockfd,sendline,strlen(sendline));
			}
		if(FD_ISSET(sockfd,&rset)){
			if(Readline(sockfd,recvline,MAXLINE) == 0)
				err_quit("str_cli:server terminated prematurely");
			}
			fputs(recvline,stdout);
		}
	}

/*
*
*==============sigconnect=============
*
*description:可移植的信号与槽连接器,被中断的系统调用在设置了SA_RESTART标志后会自动重启
*
*return:返回一个函数对象void (*func)(int),是当前被修改信号的旧行为
*
*arg:
*signo(int):信号
*func(Sigfunc*):指定信号的处理函数
*/
typedef void Sigfunc(int);
Sigfunc* sigconnect(int signo,Sigfunc* func){
	struct sigaction act,oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if(signo == SIGALRM){
		#ifdef SA_INTERRUPT
			act.sa_flags |= SA_INTERRUPT;
		#endif
		}
	else{
		#ifdef SA_RESTART
			act.sa_flags |= SA_RESTART;
		#endif
		}
	if(sigaction(signo,&act,&oact) < 0)
		return(SIG_ERR);
	return(oact.sa_handler);
	}
