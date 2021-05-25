/**
 * @brief:      Class HTTPHandler Head File
 * @copyright:  Copyright 2021 Resery
 * @license:    GPL
 * @birth:      Create by Resery on 2021-05-25
 * @version:    1.0
 * @revision:   Last revised by Resery on 2021-05-25
 */
#ifndef HTTPHandler_Header
#define HTTPHandler_Header

#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/**
 * @brief: The class of HTTPHandler is for deal with any connect from client,
 *        read request from client and return a response to client
 * @todo:
 * @fixme:
 */
class HTTPHandler {
private:
    // The connect client's fd
    int ConnFd;
    char * FileName;
    char * FileType;
    char * CgiArgs;

    // Support Error Type
    enum ErrorType {
        ErrorNotFound = 404,
        ErrorImplemented = 501
    };

    // Support Request Method

public:
    /**
     * @brief: Init the object with the Client File Description
     * @param: Fd       The Client File Description
     */
    HTTPHandler(int Fd);

    /**
     * @brief: Free the resource but destructor will do nothing at this version
     * @todo:
     *      1. After support multithread the destructor need to recycle resource
     */
    ~HTTPHandler();

    /**
     * @brief: Get the ConnFd
     */
    int GetConnFd();

    /**
     * @brief: Get the request file type
     */
    void GetFileType(char * FileName);

    /**
     * @brief: Parse the request to get the message such as method, uri, version, header, etc
     * @param: Clientfd    The Client File Description
     */
    void ParseRequest(int ClientFd);

    /**
     * @brief: Parse the uri to make sure is the static access or dynamic access
     * @param: Clientfd    The Client File Description
     * @param: Path        The path of request resource
     */
    void ParseUri(int ClientFd, char * Path);

    /**
     * @brief: To check the request method whether or not supported
     * @param: Method     The request method
     */
    /*
    void CheckMethod(int ClientFd, char * Method);
    */

    /**
     * @brief: Send response packet to the client
     * @param: ClientFd            The Client File Description
     * @param: ResponseCode        The status of the response
     * @param: ResponseMsg         The message that want to show client
     * @param: ResponseBodyType    The type of reponse body
     * @param: ResponseBody        The content of reponse body
     */
    void SendResponse(int ClientFd, const std::string & ResponseCode, const std::string & ResponseMsg,
                        const std::string & ResponseBodyType, const std::string & ResponseBody);

    /**
     * @brief: Tell client what mistake happend
     * @param: ClientFd     The Client File Description
     * @param: ErrorCode    The error code of this mistake
     * @param: ErrorMsg     The error message of this mistake
     */
    void SendErrorResponse(int ClientFd, const std::string & ErrorCode, const std::string & ErrorMsg);

    /**
     * @brief: Retrun a mistake message to client
     * @param: ClientFd     The Client File Description 
     * @param: ErrorCode    The error code of this mistake
     */
    void ClientError(int ClientFd, ErrorType ErrorCode);
};

#endif