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

    /*
     *
     * 此处为初始化线程池
     *
     */
    ThreadPool<HTTPHandler> * pool = new ThreadPool<HTTPHandler>(8);

    while(true) {
        if ((connfd = accept(listenfd, (sockaddr *)&clientinfo, &clientlen)) < 0) {
            std::perror("Accept Connect Failed");
            exit(EXIT_FAILURE);
        }

        HTTPHandler * handler = new HTTPHandler(connfd);

        std::clog << "=================================== New Connection ===================================" << std::endl;
        std::clog << "[*] Connect From ";
        std::clog << inet_ntoa(clientinfo.sin_addr) << " : ";
        std::clog << ntohs(clientinfo.sin_port) << std::endl;

        handler->bufSize = read(connfd, handler->buf_, MAXLINE);

        pool->Append(handler);

        // handler->MainStateMachine();

        // delete handler;
    }
}