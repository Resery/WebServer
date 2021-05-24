#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

#define HOSTNAME_MAX_LEN 63
#define PORT_MAX_LEN 10
#define HEADER_NAME_MAX_LEN 32
#define HEADER_VALUE_MAX_LEN 64

void doit(int connfd);
void * thread(void * vargp);
int send_to_server(char * method, char * uri, char * version, char * host, char * port);

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

int main(int argc, char **argv) {
    int listenfd;
    pthread_t tid;

    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    socklen_t clientlen = sizeof(clientaddr);

    char Ip[MAXLINE];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(0);
    }

    memset(&clientaddr, 0, sizeof(clientaddr));
    memset(&serveraddr, 0, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(argv[1]));
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Create Socket Failed\n");
        exit(0);
    }

    if (bind(listenfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
        fprintf(stderr, "Bind Socket Failed\n");
        exit(0);
    }

    printf("Start Proxy At %s : %u\n",
            inet_ntop(AF_INET, &serveraddr.sin_addr, Ip, MAXLINE),
            ntohs(serveraddr.sin_port));

    listen(listenfd, 4);

    while(1) {
        int * connfd = (int *)malloc(sizeof(int));
        *connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);

        printf("Connect From %s : %u\n",
                inet_ntop(AF_INET, &clientaddr.sin_addr, Ip, MAXLINE),
                ntohs(clientaddr.sin_port));

        pthread_create(&tid, NULL, thread, connfd);
    }
}

void * thread(void * vargp) {
    int connfd = *((int *)vargp);
    pthread_detach(pthread_self());
    free(vargp);
    doit(connfd);
    close(connfd);
    return NULL;
}

void doit(int connfd) {
    int serverfd;
    char buf[MAXLINE];
    char request_line[MAXLINE];
    // char request_header[MAXLINE];
    char server_info[MAXLINE];
    char * host;
    char * port;
    char * uri;
    char method[MAXLINE], uri_buf[MAXLINE], version[MAXLINE];

    read(connfd, request_line, MAXLINE);
    sscanf(request_line, "%s %s %s\nHost: %s", method, uri_buf, version, server_info);

    char * tmp1 = strchr(server_info, ':');
    *tmp1 = '\0';
    host = server_info;
    port = tmp1 + 1;

    char * tmp2 = strrchr(uri_buf, ':');
    *tmp2 = '\0';
    uri = tmp2 + strlen(port) + 1;

    printf("The Request Method Is %s\n", method);
    printf("The Request URI Is %s\n", uri);
    printf("The Request Version Is %s\n", version);
    printf("The Request Host Is %s\n", host);
    printf("The Request Host Port Is %s\n\n", port);

    serverfd = send_to_server(method, uri, version, host, port);
    
    int n = 0;
    while((n = read(serverfd, buf, MAXLINE))) {
        write(connfd, buf, n);
    }
}

int send_to_server(char * method, char * uri, char * version, char * host, char * port) {
    int serverfd;
    struct sockaddr_in serveraddr;
    struct hostent * htp;
    char * request;
    // char buf[MAXLINE], * buf_head = buf;

    memset(&serveraddr, 0, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(port));

    if (inet_pton(AF_INET, host, &serveraddr.sin_addr) != 0)
        htp = gethostbyaddr(&serveraddr.sin_addr, sizeof(serveraddr), AF_INET);
    else {
        htp = gethostbyname(host);
        serveraddr.sin_addr.s_addr = *(unsigned int *)htp->h_addr_list[0];
    } 

    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Create Socket Failed\n");
        exit(0);
    }

    if (connect(serverfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
        fprintf(stderr, "Connect Socket Failed\n");
        exit(0);
    }

    sprintf(request, "%s %s %s\r\n", method, uri, version);
    sprintf(request, "%sHost: %s:%s\r\n", request, host, port);
    sprintf(request, "%sConnection: keep-alive\r\n", request);
    sprintf(request, "%s%s\r\n", request, user_agent_hdr);

    write(serverfd, request, MAXLINE);

    return serverfd;
}