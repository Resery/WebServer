#include <iostream>
#include "Server.h"
#include "HTTPHandler.h"

int OpenListenFd(Server ServerInfo) {
    int listenfd = 0;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    sockaddr_in Server = ServerInfo.GetServerAddr();

    if (bind(listenfd, (const sockaddr *)&Server, sizeof(ServerInfo.GetServerAddr())) < 0)
        return -1;

    listen(listenfd, 4);

    return listenfd;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        exit(EXIT_FAILURE);
    }

    int ListenFd = 0;
    int ConnFd = 0;
    sockaddr_in ClientInfo;
    socklen_t ClientLen = sizeof(ClientInfo);
    Server ServerInfo = Server(argv[1]);

    if ((ListenFd = OpenListenFd(ServerInfo)) < 0) {
        std::perror("Open Listenfd Failed");
        exit(EXIT_FAILURE);
    }

    ServerInfo.PrintServerInfo();

    while(1) {
        if ((ConnFd = accept(ListenFd, (sockaddr *)&ClientInfo, &ClientLen)) < 0) {
            std::perror("Accept Connect Failed");
            exit(EXIT_FAILURE);
        }

        HTTPHandler Handler = HTTPHandler(ConnFd);

        std::clog << "=================================== New Connection ===================================" << std::endl;
        std::clog << "[*] Connect From ";
        std::clog << inet_ntoa(ClientInfo.sin_addr) << " : ";
        std::clog << ntohs(ClientInfo.sin_port) << std::endl;

        int ClientFd = Handler.GetClientFd();

        Handler.ParseRequest(ClientFd);
        Handler.ParseUri(ClientFd, Handler.GetPath());
        Handler.HandleError(ClientFd, Handler.Check(Handler.GetMethod(), Handler.GetPath(), Handler.GetVersion()));
        Handler.SendResponse(ClientFd, "200", "OK", Handler.GetFileType(), Handler.GetResponseBody());
    }
}