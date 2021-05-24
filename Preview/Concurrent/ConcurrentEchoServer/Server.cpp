#include <iostream>
#include <vector>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 128

char RecvMsg[MAXLINE] = {0};

void echo(int connfd) {
    while (read(connfd, RecvMsg, MAXLINE)) {
        std::clog << "Recv Msg: ";
        std::clog << RecvMsg << std::endl;
        write(connfd, RecvMsg, MAXLINE);
    }
    return;
}

void command(void) {
    char buf[MAXLINE];

    if (!fgets(buf, MAXLINE, stdin))
        exit(0);
    std::clog << "Select " << buf << std::endl;
}

int main(int argc, char ** argv) {
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    char Ip[MAXLINE] = {0};
    fd_set read_set, ready_set;

    if (argc != 2) {
        std::cerr << "Usage " << argv[0] << " port" << std::endl;
        exit(0);
    }

    bzero(&clientaddr, sizeof(clientaddr));
    bzero(&serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(atoi(argv[1]));

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::perror("Create Socket Failed");
        exit(0);
    }

    if (bind(listenfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr))) {
        std::perror("Bind Socket Failed");
        exit(0);
    }

    std::clog << "Start Server at ";
    std::clog << inet_ntop(AF_INET, &serveraddr.sin_addr, Ip, MAXLINE) << " : ";
    std::clog << ntohs(serveraddr.sin_port) << std::endl;

    listen(listenfd, 4);

    // Init Read Set
    FD_ZERO(&read_set);
    // Insert Stdin To Read Set
    FD_SET(STDIN_FILENO, &read_set);
    // Insert Listenfd To Read Set
    FD_SET(listenfd, &read_set);

    clientlen = sizeof(clientaddr);
    while (1) {
        ready_set = read_set;
        select(listenfd + 1, &ready_set, NULL, NULL, NULL);

        if (FD_ISSET(STDIN_FILENO, &ready_set))
            command();

        if (FD_ISSET(listenfd, &ready_set)) {
            if ((connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen)) < 0) {
                std::perror("Accept Socket Failed");
                exit(0);
            }
    
            std::clog << "Connect From: ";
            std::clog << inet_ntop(AF_INET, &clientaddr.sin_addr, Ip, MAXLINE) << " : ";
            std::clog << ntohs(clientaddr.sin_port) << std::endl;
    
            if (fork() == 0) {
                close(listenfd);
                echo(connfd);
                close(connfd);
                exit(0);
            }
    
            close(connfd);
    
        }
    }
}