/**
 * @brief:      Class Server Head File
 * @copyright:  Copyright 2021 Resery
 * @license:    GPL
 * @birth:      Create by Resery on 2021-05-25
 * @version:    1.0
 * @revision:   Last revised by Resery on 2021-05-25
 */
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAXLINE 8192

/**
 * @brief: The class of Server is contains Server's Info
 * @todo:
 */
class Server {
private:
    sockaddr_in ServerAddr;

public:
    /**
     * @brief: Init the object with the server port, defualt protocol is TCP(AF_INET),
     *         Ip addr Is any Ip addr
     * @todo: Support specified the www path
     */
    Server(char * Port);
    /**
     * @brief: Free the resource but destructor will do nothing in this version
     * @todo:
     *      1. After support multithread the destructor need to recycle resource
     */
    ~Server();

    /**
     * @brief: To get the server information
     */
    sockaddr_in GetServerAddr();

    /**
     * @brief: Print the server information
     */
    void PrintServerInfo();
};