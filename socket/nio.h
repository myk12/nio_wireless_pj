#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#define SERVER_PORT (7777)
#define LOCAL_TEST_SERVER_IP   ("127.0.0.1")
#define SERVER_IP   ("192.168.0.114")

#define MAXLINE 1024
#define READ_BUFF_LEN (1024*1024)
#define SEND_BUFF_LEN  (128*1024)