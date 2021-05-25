#include "Server.h"

Server::Server(char * Port) {
    this->ServerAddr.sin_family = AF_INET;
    this->ServerAddr.sin_port = htons(atoi(Port));
    this->ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
}

Server::~Server() {
}

sockaddr_in Server::GetServerAddr() {
    return ServerAddr;
}

void Server::PrintServerInfo() {
    std::clog << "================================== Starting Server ===================================" << std::endl;
    std::clog << "[*] Welcome to use Resery WebServer" << std::endl;
    std::clog << "[*] The server version is 1.0" << std::endl;
    std::clog << "[*] The server is running at ";
    std::clog << inet_ntoa(ServerAddr.sin_addr) << " : ";
    std::clog << ntohs(ServerAddr.sin_port) << std::endl;
}