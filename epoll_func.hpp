/** epoll_func.cpp
*/
#include <bits/stdc++.h>
//#include "echo-server.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/fcntl.h>

using namespace std;

void add_event(int, int, int);
void handle_events(int, epoll_event *, int, int, char *);
void handle_accpet(int, int);
void do_read(int, int, char *);
void do_write(int, int, char *);
void delete_event(int, int, int);
void modify_event(int, int, int);
void handle_events(int epollfd, epoll_event *events, int num, int listenfd, char *buf)
{

    //进行遍历;这里只要遍历已经准备好的io事件。num并不是当初epoll_create时的FDSIZE。
    for (int i = 0; i < num; i++)
    {
        int fd = events[i].data.fd;
        //根据描述符的类型和事件类型进行处理
        if ((fd == listenfd) && (events[i].events & EPOLLIN))
            handle_accpet(epollfd, listenfd);
        else if (events[i].events & EPOLLIN)
            do_read(epollfd, fd, buf);
        else if (events[i].events & EPOLLOUT)
            do_write(epollfd, fd, buf);
    }
}
void add_event(int epfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
}

//处理接收到的连接
void handle_accpet(int epollfd, int listenfd)
{

    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen;
    int clifd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddrlen);
    if (clifd == -1)
        perror("accpet error:");
    else
    {
        //cout << "accept a new client: " << inet_ntoa(cliaddr.sin_addr) << ':' << cliaddr.sin_port << endl; //添加一个客户描述符和事件
        printf("accept a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port); //添加一个客户描述符和事件
        add_event(epollfd, clifd, EPOLLIN);
    }
}

//读处理
void do_read(int epollfd, int fd, char *buf)
{
    int nread = read(fd, buf, MAXSIZE);
    if (nread == -1)
    {
        perror("read error:");
        close(fd);                          //记住close fd
        delete_event(epollfd, fd, EPOLLIN); //删除监听
    }
    else if (nread == 0)
    {
        cerr << "Client Close.\n";
        close(fd);                          //记住close fd
        delete_event(epollfd, fd, EPOLLIN); //删除监听
    }
    else
    {
        if (strcmp(buf, "q") == 0)
        {
            cout << "Server Close!" << endl;
        }
        else
        {
            cout << "read: " << buf << endl;
        }

        //修改描述符对应的事件，由读改为写
        modify_event(epollfd, fd, EPOLLOUT);
    }
}

//写处理
void do_write(int epollfd, int fd, char *buf)
{
    if (strcmp(buf, "q") == 0)
    {
        write(fd, "Server Close!", strlen("Server Close!"));
        close(fd);
        delete_event(epollfd, fd, EPOLLOUT);
        do_exit();
    }
    int nwrite = write(fd, buf, strlen(buf));
    cout << "write: " << buf << endl;
    if (nwrite == -1)
    {
        perror("write error:");
        close(fd);                           //记住close fd
        delete_event(epollfd, fd, EPOLLOUT); //删除监听
    }
    else
    {
        modify_event(epollfd, fd, EPOLLIN);
    }
    memset(buf, 0, sizeof(buf));
}

//删除事件
void delete_event(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

//修改事件
void modify_event(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}