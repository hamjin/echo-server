#include <bits/stdc++.h>
#include "echo-server.hpp"

using namespace std;

int main()
{
    cout << "Hello From Debian Client" << endl;
    // Create Socket
    struct sockaddr_in servaddr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr);
    // Connect Socket
    bool connected = false;
    int ret = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret != 0)
    {
        cerr << "connect error: " << strerror(errno) << endl;
        // return -1;
    }
    else
        connected = true;
    // Communicate
    while (true)
    {
        if (!connected)
        {
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            memset(&servaddr, 0, sizeof(servaddr));
            servaddr.sin_family = AF_INET;
            servaddr.sin_port = htons(SERV_PORT);
            inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr);
            ret = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
            if (ret != 0)
            {
                cerr << "connect error: " << strerror(errno) << endl;
                cerr << "Wait 5s " << endl;
                sleep(5);
                // return -1;
                continue;
            }
            else
            {
                connected = true;
                cout << "Connected" << endl;
            }
        }
        string buf, bufin;
        char bufinc[MAXSIZE] = {0};
        getline(cin, buf);
        cout << "sent msg:" << buf << endl;
        // printf("%s\n", buf.c_str());
        write(sockfd, buf.c_str(), buf.length());
        memset(bufinc, 0, sizeof(bufinc));
        if (read(sockfd, bufinc, MAXSIZE) <= 0)
        {
            cout << "Falied Read From Server!" << endl;
            perror("errno");
            // break;
            close(sockfd);
            connected = false;
        }
        bufin = bufinc;
        cout << "read msg:" << bufin << endl;
        // printf("read msg:%s\n", bufin);
        if (buf == "q")
        {
            cout << "close client!" << endl;
            // printf("close client!\n");
            close(sockfd);
            break;
        }
    }
}