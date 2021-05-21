#include <iostream>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAXLINE 128

int main(int argc, char **argv) {
    int listenfd = 0;
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
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Create Socket Failed");
        exit(-1);
    }

    if (bind(listenfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind Socket Failed");
        exit(-1);
    }

    std::cout << "Start Server at ";
    std::cout << inet_ntop(AF_INET, &servaddr.sin_addr, ip, MAXLINE) << " : ";
    std::cout << ntohs(servaddr.sin_port) << std::endl;
    listen(listenfd, 4);

    clilen = sizeof(cliaddr);
    if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0) {
        perror("Accept Failed");
        exit(-1);
    }

    std::cout << "Connect From ";
    std::cout << inet_ntop(AF_INET, &cliaddr.sin_addr, ip, MAXLINE) << " : ";
    std::cout << ntohs(cliaddr.sin_port) << "\n" << std::endl;

    while (read(connfd, RecvMsg, MAXLINE)) {
        std::cout << "Recv Msg : " << RecvMsg << std::endl;
        write(connfd, RecvMsg, sizeof(RecvMsg));
    }

    close(connfd);
    close(listenfd);
    exit(0);
}