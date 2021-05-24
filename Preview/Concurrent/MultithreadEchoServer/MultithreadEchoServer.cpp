#include <iostream>
#include <stdlib.h>
#include <strings.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define MAXLINE 128

char RecvMsg[MAXLINE] = {0};

void echo(int connfd) {
    while (read(connfd, RecvMsg, MAXLINE) > 0) {
        std::clog << "Recv Msg:";
        std::clog << RecvMsg << std::endl;
        write(connfd, RecvMsg, sizeof(RecvMsg));
    }
    /*
    while (1) {
        if (read(connfd, RecvMsg, MAXLINE) > 0) {
        }
    }
    */
}

void * thread(void *vargp) {
    int connfd = *((int *)vargp);
    pthread_detach(pthread_self());
    free(vargp);
    echo(connfd);
    close(connfd);
    return NULL;
}

int main(int argc, char **argv) {
    int listenfd, * connfd;
    socklen_t clientlen;
    pthread_t tid;

    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;

    char Ip[MAXLINE] = {0};

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        exit(0);
    }

    bzero(&clientaddr, sizeof(clientaddr));
    bzero(&serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(atoi(argv[1]));

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std:perror("Create Socket Failed");
        exit(0);
    }

    if (bind(listenfd, (const sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
        std::perror("Bind Socket Failed");
        exit(0);
    }

    std::clog << "Start Server At ";
    std::clog << inet_ntop(AF_INET, &serveraddr.sin_addr, Ip, MAXLINE) << " : ";
    std::clog << ntohs(serveraddr.sin_port) << std::endl;;

    listen(listenfd, 4);

    clientlen = sizeof(clientaddr);
    while(1) {
        connfd = (int *)malloc(sizeof(int));
        *connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);

        std::clog << "Connect From ";
        std::clog << inet_ntop(AF_INET, &clientaddr.sin_addr, Ip, MAXLINE) << " : ";
        std::clog << ntohs(clientaddr.sin_port) << std::endl;
        pthread_create(&tid, NULL, thread, connfd);
    }
}