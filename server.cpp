/** server.cpp
 **/

#include <bits/stdc++.h>

#include "echo-server.hpp"
#include "epoll_func.hpp"

using namespace std;
int listenfd;
int epollfd;
int socket_bind();
int main()
{
    cout << "Hello From Debian Server" << endl;
    // Create Socket
    listenfd = socket_bind();
    // Create epoll
    epollfd = epoll_create(16);
    // Add event
    add_event(epollfd, listenfd, EPOLLIN);
    struct epoll_event events[128];
    char buffer[1024] = {0};
    // Start Process
    while (true)
    {
        int ret = epoll_wait(epollfd, events, 128, -1);
        handle_events(epollfd, events, ret, listenfd, buffer);
    }
    return 0;
}
int socket_bind()
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        cerr << "create socket error!" << endl;
        exit(-1);
    }

    struct sockaddr_in saddr; // Generate Addr
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = INADDR_ANY;
    int on = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
    setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(int));
    if (bind(sfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr)) == -1) // Bind Addr
    {
        cerr << "bind socket error!, errno=" << strerror(errno) << endl;
        exit(-1);
    }

    if (listen(sfd, 8) == -1) // Listen
    {
        cerr << "listen error!" << endl;
        exit(-1);
    }
    return sfd;
}
void do_exit()
{
    close(listenfd);
    close(epollfd);
    exit(0);
}
