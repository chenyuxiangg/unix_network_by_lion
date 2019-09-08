#ifndef UNP_H
#define UNP_H

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/select.h>
#include <math.h>

//macro
//--const value
#define MAXLINE 4096
#define LISTENQ 1024
#define ECHO_PORT 8977
#define SA struct sockaddr

#endif
