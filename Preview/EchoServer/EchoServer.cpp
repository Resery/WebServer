#include <iostream>
#include <set>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAXLINE 128

int Fd[MAXLINE];

unsigned int maxSocketId(int ClientFd, std::set<int>& ConnFd) {
    return ConnFd.empty() ? ClientFd : std::max(ClientFd, *ConnFd.rbegin());
}

static inline int set_nonblock(int fd) {
    int flags;
#if defined(O_NONBLOCK)
    if (-1 == (flags = fcntl(fd, F_GETFL, 0))) {
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    flags = 1;
    return ioctl(fd, FIONBIO, &flags);
#endif
}

int main(int argc, char **argv) {
    int ListenFd = 0;
    int connfd = 0;
    socklen_t clilen;
    char RecvMsg[MAXLINE] = {0};

    struct sockaddr_in cliaddr, servaddr;
    char ip[MAXLINE] = {0};

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>", argv[0]);
        exit(-1);
    }

    // Init Server Info Struct
    bzero(&servaddr, sizeof(servaddr));
    // Set Server Procotol
    servaddr.sin_family = AF_INET;
    // Set Server Ip Address
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // Set Server Port
    servaddr.sin_port = htons(atoi(argv[1]));

    //
    if ((ListenFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Create Socket Failed");
        exit(-1);
    }

    if (bind(ListenFd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind Socket Failed");
        exit(-1);
    }

    std::cout << "Start Server at ";
    std::cout << inet_ntop(AF_INET, &servaddr.sin_addr, ip, MAXLINE) << " : ";
    std::cout << ntohs(servaddr.sin_port) << std::endl;
    listen(ListenFd, 4);

    int EpollFd = epoll_create(0x1);
    struct epoll_event Event;
    struct epoll_event * Events = (epoll_event *)malloc(sizeof(Events));

    Event.data.fd = ListenFd;
    Event.events = EPOLLIN | EPOLLET;

    epoll_ctl(EpollFd, EPOLL_CTL_ADD, Event.data.fd, &Event);

    clilen = sizeof(cliaddr);
    while (true) {
        memset(&Events[0].data, 0, sizeof(Events[0].data));
        Events->events = 0;
        int Nfds = epoll_wait(EpollFd, Events, 5, 100);
        if (Nfds < 0) {
            perror("Epoll Wait Failed");
        }

        for (int i = 0; i < Nfds; i++) {
            if (Events[i].data.fd == ListenFd) {
                int ConnFd = accept(ListenFd, (struct sockaddr *)&cliaddr, &clilen);
                if (ConnFd < 0) {
                    perror("Accepte Socket Failed");
                    exit(EXIT_FAILURE);
                }

                std::cout << "Connect From ";
                std::cout << inet_ntop(AF_INET, &cliaddr.sin_addr, ip, MAXLINE) << " : ";
                std::cout << ntohs(cliaddr.sin_port) << "\n" << std::endl;

                Event.data.fd = ConnFd;
                Event.events = EPOLLIN | EPOLLET;
                epoll_ctl(EpollFd, EPOLL_CTL_ADD, Event.data.fd, &Event);
            } 
            else if (Events[i].data.fd != ListenFd) {
                char RecvMsg[MAXLINE];
                int Len = recv(Events[i].data.fd, RecvMsg, MAXLINE, MSG_NOSIGNAL);
                if (Len <= 0) {
                    std::clog << "The Client ";
                    std::cout << inet_ntop(AF_INET, &cliaddr.sin_addr, ip, MAXLINE) << " : ";
                    std::cout << ntohs(cliaddr.sin_port) << " Is Disconnect" << std::endl;
                    epoll_ctl(EpollFd, EPOLL_CTL_DEL, Events[i].data.fd, &Events[i]);
                    shutdown(Events[i].data.fd, SHUT_RDWR);
                    close(Events[i].data.fd);
                } else {
                    std::clog << "Recv Msg : " << RecvMsg << std::endl;
                    send(Events[i].data.fd, RecvMsg, MAXLINE, MSG_NOSIGNAL);
                }
            }
            /*
            else {
                char RecvMsg[MAXLINE];
                int Len = recv(Events[i].data.fd, RecvMsg, MAXLINE, MSG_NOSIGNAL);
                if (Len <= 0) {
                    std::clog << "The Client ";
                    std::cout << inet_ntop(AF_INET, &cliaddr.sin_addr, ip, MAXLINE) << " : ";
                    std::cout << ntohs(cliaddr.sin_port) << " Is Disconnect" << std::endl;
                    epoll_ctl(EpollFd, EPOLL_CTL_DEL, Events[i].data.fd, &Events[i]);
                    shutdown(Events[i].data.fd, SHUT_RDWR);
                    close(Events[i].data.fd);
                } else {
                    std::clog << "Recv Msg : " << RecvMsg << std::endl;
                    send(Events[i].data.fd, RecvMsg, MAXLINE, MSG_NOSIGNAL);
                }
            }
            */
        }
    }

    /* Poll
    pollfd Fds[4096];
    int FdsSize = 1;
    int TimeOut = 0;

    Fds[0].fd = ListenFd;
    Fds[0].events = POLLRDNORM;
    Fds[0].revents = 0;

    while (true) {
        int Ready = poll(Fds, FdsSize, TimeOut);

        if (Fds[0].revents & POLLRDNORM) {
            int ConnFd = 0;
            if ((ConnFd = accept(ListenFd, (struct sockaddr *)&cliaddr, &clilen)) < 0) {
                perror("Accept Socket Failed");
                exit(EXIT_FAILURE);
            }
            std::cout << "Connect From ";
            std::cout << inet_ntop(AF_INET, &cliaddr.sin_addr, ip, MAXLINE) << " : ";
            std::cout << ntohs(cliaddr.sin_port) << "\n" << std::endl;
            Fds[FdsSize].fd = ConnFd;
            Fds[FdsSize].events = POLLRDNORM;
            Fds[FdsSize].revents = 0;
            FdsSize++;
        }

        for (int i = 1; i < FdsSize; i++) {
            if (Fds[i].revents & POLLRDNORM) {
                char RecvMsg[MAXLINE];
                int Len = recv(Fds[i].fd, RecvMsg, MAXLINE, MSG_NOSIGNAL);
                if (Len <= 0) {
                    std::clog << "The Client ";
                    std::cout << inet_ntop(AF_INET, &cliaddr.sin_addr, ip, MAXLINE) << " : ";
                    std::cout << ntohs(cliaddr.sin_port) << " Is Disconnect" << std::endl;
                    shutdown(Fds[i].fd, SHUT_RDWR);
                    close(Fds[i].fd);
                    if (i == FdsSize - 1) {
                        FdsSize--;
                    }
                    Fds[i].fd = -1;
                } else {
                    std::clog << "Recv Msg : " << RecvMsg << std::endl;
                    send(Fds[i].fd, RecvMsg, MAXLINE, MSG_NOSIGNAL);
                }
            }
        }
    }
    */

    /* Select
    std::set<int> ConnFd;

    fd_set ReadFds;

    struct timeval Tmv;

    clilen = sizeof(cliaddr);
    while(true) {
        // 初始化监听集合
        FD_ZERO(&ReadFds);
        // 将 listen 函数返回的 ListenFd添加到 ReadFds 集合中
        FD_SET(ListenFd, &ReadFds);

        // 初始化 Tmv
        Tmv.tv_sec = 30;
        Tmv.tv_usec = 0;

        // 遍历 ConnFd 集合将其中的元素全部添加到监听集合中
        for (auto Fd : ConnFd) {
            FD_SET(Fd, &ReadFds);
        }

        // 设置最大可连接的文件描述符为 MaxFd
        unsigned int MaxFd = maxSocketId(ListenFd, ConnFd) + 1;

        // 监听 ReadFds 集合中处于就绪状态的文件描述符
        select(MaxFd, &ReadFds, NULL, NULL, &Tmv);

        // 遍历 ConnFd 集合，检查集合中处于就绪状态的文件描述符并且读取对应的内容
        auto It = ConnFd.begin();
        while (It != ConnFd.end()) {
            int Fd = *It;
            if (FD_ISSET(Fd, &ReadFds)) {
                char RecvMsg[MAXLINE];
                // 接收准备就绪的描述符发送的内容
                int Len = recv(Fd, &RecvMsg, MAXLINE, MSG_NOSIGNAL);

                std::cout << "Recv Msg : " << RecvMsg << std::endl;

                // 检测连接是否已经断开，如果连接已经断开则从 ConnFd 中移除这个描述符
                if (Len <= 0 && errno != EAGAIN) {
                    std::clog << "The Client ";
                    std::cout << inet_ntop(AF_INET, &cliaddr.sin_addr, ip, MAXLINE) << " : ";
                    std::cout << ntohs(cliaddr.sin_port) << " Is Disconnect" << std::endl;

                    shutdown(Fd, SHUT_RDWR);
                    close(Fd);
                    It = ConnFd.erase(It);
                } else if (Len > 0) {
                    send(Fd, &RecvMsg, MAXLINE, MSG_NOSIGNAL);
                    It++;
                } else It++;
            } else It++;
        }

        // 检测 listen 返回的文件描述符是否处于就绪状态，如果其处于就绪状态说明它现在可以通过 accept 函数接受一个请求
        if (FD_ISSET(ListenFd, &ReadFds)) {
            int Fd = accept(ListenFd, (struct sockaddr *)&cliaddr, &clilen);

            std::cout << "Connect From ";
            std::cout << inet_ntop(AF_INET, &cliaddr.sin_addr, ip, MAXLINE) << " : ";
            std::cout << ntohs(cliaddr.sin_port) << "\n" << std::endl;

            if (Fd == -1) {
                perror("Accept Socket Failed");
                exit(EXIT_FAILURE);
            }

            // 关闭阻塞
            set_nonblock(Fd);
            ConnFd.insert(Fd);
        }
    }
    */
}