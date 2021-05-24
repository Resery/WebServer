#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <netdb.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "sbuf.h"

#define MAXLINE 128
#define NTHREADS 4
#define SBUFSIZE 16
#define P sem_wait
#define V sem_post

void echo_cnt(int connfd);
void * thread(void * vargp);
void echo_cnt(int connfd);
static void int_echo_cng(void);

sbuf_t sbuf;
char RecvMsg[MAXLINE];

static int byte_cnt;
static sem_t mutex;

int main(int argc, char ** argv) {
    int listenfd, connfd;
    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    socklen_t clientlen = sizeof(clientaddr);
    char Ip[MAXLINE] = {0};

    pthread_t tid;

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        exit(0);
    }

    sbuf_init(&sbuf, SBUFSIZE);

    memset(&serveraddr, 0, sizeof(serveraddr));
    memset(&clientaddr, 0, sizeof(clientaddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(argv[1]));
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Create Socket Failed" << std::endl;
        exit(0);
    }

    if (bind(listenfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
        std::cerr << "Bind Socket Failed" << std::endl;
        exit(0);
    }

    listen(listenfd, 4);

    std::clog << "Server Start At ";
    std::clog << inet_ntop(AF_INET, &serveraddr.sin_addr, Ip, MAXLINE) << " : ";
    std::clog << ntohs(serveraddr.sin_port) << std::endl;;

    for (int i = 0; i < NTHREADS; i++)
        pthread_create(&tid, NULL, thread, NULL);

    while (1) {
        connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);

        std::clog << "Connect From ";
        std::clog << inet_ntop(AF_INET, &clientaddr.sin_addr, Ip, MAXLINE) << " : ";
        std::clog << ntohs(clientaddr.sin_port) << std::endl;

        sbuf_insert(&sbuf, connfd);
    }
}

void * thread(void *vargp) {
    pthread_detach(pthread_self());
    while (1) {
        int connfd = sbuf_remove(&sbuf);
        echo_cnt(connfd);
        close(connfd);
    }
}

static void init_echo_cnt(void) {
    sem_init(&mutex, 0, 1);
    byte_cnt = 0;
}

void echo_cnt(int connfd) {
    int n;
    char buf[MAXLINE];
    static pthread_once_t once = PTHREAD_ONCE_INIT;

    pthread_once(&once, init_echo_cnt);
    while ((n = read(connfd, buf, MAXLINE)) > 0) {
        P(&mutex);
        byte_cnt += n;
        std::clog << "Thread " << (unsigned int)pthread_self() << " received " << n;
        std::clog << " (" << byte_cnt <<  " total) bytes on fd " << connfd << std::endl;
        V(&mutex);
        write(connfd, buf, n);
    }
}