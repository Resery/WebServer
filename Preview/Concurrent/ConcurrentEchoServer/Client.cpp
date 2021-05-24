#include <iostream>
#include <vector>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 128 

int main(int argc, char ** argv) {
    int serverfd;
    char SendMsg[MAXLINE] = {0};
    char RecvMsg[MAXLINE] = {0};
    struct sockaddr_in serveraddr;
    struct hostent * htp;

    if (argc != 3) {
        std::cerr << "Usage " << argv[0] << " <hostname / ip> <port>" << std::endl;
        exit(0);
    }

    bzero(&serveraddr, sizeof(serveraddr));

    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        std::perror("Create Socket Failed");
        exit(0);
    }

    if (inet_pton(AF_INET, argv[1], &serveraddr.sin_addr) != 0)
        htp = gethostbyaddr(&serveraddr.sin_addr, sizeof(serveraddr), AF_INET);
    else {
        htp = gethostbyname(argv[1]);
        serveraddr.sin_addr.s_addr = *(unsigned int *)htp->h_addr_list[0];
    }
    
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(argv[2]));

    if (connect(serverfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        std::perror("Connect Server Failed");
        exit(0);
    }

    std::clog << "Send Msg: ";
    while (fgets(SendMsg, MAXLINE, stdin)) {
        int ret = 0;
        if ((ret = strncmp(SendMsg, "exit", 4)) == 0) exit(0);
        write(serverfd, SendMsg, sizeof(SendMsg));
        read(serverfd, RecvMsg, MAXLINE);
        std::cout << "Recv Msg: " << RecvMsg << std::endl;
        std::cout << "Send Msg: ";
    }
}