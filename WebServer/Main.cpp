#include <iostream>
#include "Server.h"
#include "HTTPHandler.h"
#include "threadpool.h"

int OpenListenFd(Server serverinfo) {
    int listenfd = 0;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    sockaddr_in server = serverinfo.GetServerAddr();

    if (bind(listenfd, (const sockaddr *)&server, sizeof(serverinfo.GetServerAddr())) < 0)
        return -1;

    listen(listenfd, 4);

    return listenfd;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        exit(EXIT_FAILURE);
    }

    int listenfd = 0;
    int connfd = 0;
    sockaddr_in clientinfo;
    socklen_t clientlen = sizeof(clientinfo);
    Server serverinfo = Server(argv[1]);

    if ((listenfd = OpenListenFd(serverinfo)) < 0) {
        std::perror("Open Listenfd Failed");
        exit(EXIT_FAILURE);
    }

    serverinfo.PrintServerInfo();

    ThreadPool<HTTPHandler> * pool = new ThreadPool<HTTPHandler>(8);

    Epoll event;

    event.Add(listenfd, EPOLLIN);

    HTTPHandler * users = new HTTPHandler[1024];

    while(true) {
        int ready = event.Wait(100);
        if (ready < 0) perror("Wait Failed");

        for (int i = 0; i < ready; i++) {
            struct epoll_event ev = event.GetEvent(i);
            if (ev.data.fd == listenfd) {
                if ((connfd = accept(listenfd, (sockaddr *)&clientinfo, &clientlen)) < 0) {
                    std::perror("Accept Connect Failed");
                    exit(EXIT_FAILURE);
                }

                std::clog << "=================================== New Connection ===================================" << std::endl;
                std::clog << "[*] Connect From ";
                std::clog << inet_ntoa(clientinfo.sin_addr) << " : ";
                std::clog << ntohs(clientinfo.sin_port) << std::endl;

                users[connfd].SetClientFd(connfd);
                users[connfd].SetEpollFd(event.GetEpollFd());
                // epoll_ctl(event.GetEpollFd(), EPOLL_CTL_ADD, connfd, &ev);
                event.Add(connfd, EPOLLIN);
            } else if (ev.data.fd != listenfd && ev.data.fd != 0) {
                HTTPHandler * handler = new HTTPHandler(ev.data.fd);
                handler->bufSize = read(ev.data.fd, handler->buf_, MAXLINE);
                if (handler->bufSize <= 0) {
                    event.Del(ev.data.fd, EPOLLIN);
                    shutdown(ev.data.fd, SHUT_RDWR);
                    close(ev.data.fd);
                } else {
                    // pool->Append(&users[ev.data.fd]);
                    pool->Append(handler);
                }
                
                /*
                if (ev.events & EPOLLIN) {
                    users[ev.data.fd].bufSize = read(ev.data.fd, users[ev.data.fd].buf_, MAXLINE);
                    if (users[ev.data.fd].bufSize <= 0) {
                        event.Del(ev.data.fd, EPOLLIN);
                        shutdown(ev.data.fd, SHUT_RDWR);
                        close(ev.data.fd);
                        delete &users[ev.data.fd];
                    } else {
                        pool->Append(&users[ev.data.fd]);
                    }
                } else if (ev.events & EPOLLOUT) {
                    std::string response = users[ev.data.fd].GetResponse();
                    send(ev.data.fd, response.c_str(), response.size(), MSG_SYN);
                    event.Mod(ev.data.fd, EPOLLIN);
                }
                */
            }
        }
    }
}