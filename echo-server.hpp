#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/fcntl.h>
#define LISTENQ 20

const int PORT = 55555;
const int MAXSIZE = 1024;

//定义连接服务器IP，根据服务端IP地址修改
const char IPADDRESS[] = "127.0.0.1";
//定义连接服务器端口，根据服务端监听端口修改
const int SERV_PORT = 55555;

void do_exit();