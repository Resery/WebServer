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

int byte_cnt = 0;

typedef struct {
    int maxfd;
    fd_set read_set;
    fd_set ready_set;
    int nready;
    int maxi;
    int clientfd[FD_SETSIZE];
} pool;

void echo(int connfd) {
    while (read(connfd, RecvMsg, MAXLINE)) {
        std::clog << "Recv Msg: ";
        std::clog << RecvMsg << std::endl;
        write(connfd, RecvMsg, MAXLINE);
    }
    return;
}

void init_pool(int listenfd, pool * p) {
    p->maxi = -1;
    for (int i = 0; i < FD_SETSIZE; i++)
        p->clientfd[i] = -1;

    p->maxfd = listenfd;
    FD_ZERO(&p->read_set);
    FD_SET(listenfd, &p->read_set);
}

void add_client(int connfd, pool *p) {
    int i;
    p->nready--;
    for (i = 0; i < FD_SETSIZE; i++) {
        if (p->clientfd[i] < 0)
            p->clientfd[i] = connfd;
        FD_SET(connfd, &p->read_set);

        if (connfd > p->maxfd)
            p->maxfd = connfd;
        if (i > p->maxi)
            p->maxi = i;
        break;
    }

    if (i == FD_SETSIZE)
        std::cerr << "Add_Client Error: Too Many Clients" << std::endl;
    return;
}

void check_clients(pool *p) {
    int i, connfd, n;
    char buf[MAXLINE];

    for (i = 0; (i <= p->maxi) && (p->nready > 0); i++) {
        connfd = p->clientfd[i];
        
        if ((connfd > 0) && (FD_ISSET(connfd, &p->ready_set))) {
            p->nready--;
            if (n = read(connfd, buf, MAXLINE) != 0) {
                byte_cnt += n;
                std::clog << "Server received " << n << " (" << byte_cnt;
                std::clog << " total) bytes on fd " << connfd << std::endl;
                write(connfd, buf, n);
            }
        } else {
            close(connfd);
            FD_CLR(connfd, &p->read_set);
            p->clientfd[i] = -1;
        }
    }

}

int main(int argc, char ** argv) {
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    char Ip[MAXLINE] = {0};
    fd_set read_set, ready_set;
    static pool pool;

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

    clientlen = sizeof(clientaddr);

    init_pool(listenfd, &pool);
    while (1) {
        pool.ready_set = pool.read_set;
        pool.nready = select(pool.maxfd + 1, &pool.read_set, NULL, NULL, NULL);

        if (FD_ISSET(listenfd, &ready_set)) {
            if ((connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen)) < 0) {
                std::perror("Accept Socket Failed");
                exit(0);
            }

            std::clog << "Connect From: ";
            std::clog << inet_ntop(AF_INET, &clientaddr.sin_addr, Ip, MAXLINE) << " : ";
            std::clog << ntohs(clientaddr.sin_port) << std::endl;
    
            add_client(connfd, &pool);
        }

        check_clients(&pool);
    }
}