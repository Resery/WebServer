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
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>

/**
 * @brief: The class of HTTPHandler is for deal with any connect from client,
 *        read request from client and return a response to client
 * @todo:
 * @fixme:
 */
class HTTPHandler {
private:
    int clientfd_;

    int mainstate_;

    int followstate_;

    bool static_;

    int epollfd_;

    std::string method_;
    std::string path_;
    std::string version_;

    std::string cgiargs_;
    std::string filename_;
    std::string filetype_;

    std::string responsebody_;

    std::string response_;

    // unsigned int requestmethod_;

    // Support Error Type
    enum ErrorType {
        ErrSuccess = 0,

        ErrSendResponseFail,

        ErrForBid,
        ErrNotFound,

        ErrImplemented,
        ErrVersionNotSupported,
        ErrInternalError,

        ErrNoRequest,
        ErrGetRequest
    };

    enum RequesetMethod {
        Get,
        Post,
        Head
    };

    enum MainState {
        CheckRequestLine,
        CheckHeader,
        CheckBody
    };

    enum FollowState {
        LineOk,
        LineBad,
        LineOpen
    };

    // Make sure nobody can invoke copy and copy assignment
    HTTPHandler& operator=(const HTTPHandler&);

public:
    char buf_[4096];

    unsigned int prevPos;
    unsigned int currentPos;

    unsigned int bufSize;

    unsigned int contentLength;
    bool longConnect;

    /**
     * @brief: Init the object with the Client File Description
     * @param: Fd       The Client File Description
     */
    HTTPHandler();
    HTTPHandler(int Fd);
    HTTPHandler(const HTTPHandler&);

    /**
     * @brief: Free the resource but destructor will do nothing at this version
     * @todo:
     *      1. After support multithread the destructor need to recycle resource
     */
    ~HTTPHandler();

    /**
     * @brief: Get the ClientFd
     */
    int GetClientFd();

    /**
     * @brief: Get Method
     */
    std::string & GetMethod();

    /**
     * @brief: Get Path
     */
    std::string & GetPath();

    /**
     * @brief: Get Version
     */
    std::string & GetVersion();

    /**
     * @brief: Get Response Body
     */
    std::string & GetResponseBody();

    /**
     * @brief:
     */
    std::string & GetFileType();

    /**
     * @brief: Get the request file type
     */
    void ParseFileType(const std::string & FN);

    /**
     * @brief: HttpRead is a read wrapper that can support more effictive I/O
     */
    int HttpRead(int ClientFd, void * Buf, size_t Count);

    /**
     * @brief: HttpWrite is a write wrapper that can support more effictive I/O
     */
    int HttpWrite(int ClientFd, const void * Buf, size_t Count);

    /**
     * @brief: Read the request from client
     */
    void ReadRequest(int ClientFd, std::string & Request);

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
    void ParseUri(int ClientFd, const std::string & Path);

    /**
     * @brief: Send response packet to the client
     * @param: ClientFd            The Client File Description
     * @param: ResponseCode        The status of the response
     * @param: ResponseMsg         The message that want to show client
     * @param: ResponseBodyType    The type of reponse body
     * @param: ResponseBody        The content of reponse body
     */
    bool SendResponse(int ClientFd, const std::string & ResponseCode, const std::string & ResponseMsg,
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
    bool HandleError(int ClientFd, ErrorType ErrorCode);

    /**
     * @brief: Check the request whether or not vaild
     * @param: Method      The client request method
     * @param: Path        The client request path
     * @param: Version     The client request HTTP Version
     */
    HTTPHandler::ErrorType Check(const std::string & Method, 
                                    const std::string & Path,
                                    const std::string &Version);

    /**
     * @brief: Check if this method is supported
     * @param: Method       The clinet request method
     */
    HTTPHandler::ErrorType CheckMethod(const std::string & Method);

    /**
     * @brief: Check the path whether or not supported
     * @param: Path         The client request path
     */
    HTTPHandler::ErrorType CheckPath(const std::string & Path);
    
    /**
     * @brief: Check the version whether or nor supported
     * @param: Version      The client request HTTP Version
     */
    HTTPHandler::ErrorType CheckVersion(const std::string & Version);

    HTTPHandler::FollowState ParseLine();

    HTTPHandler::ErrorType ParseRequestLine(std::string & requestline);

    HTTPHandler::ErrorType ParseHeader(std::string & requestheader);

    HTTPHandler::ErrorType ParseBody(std::string & requestbody);

    // char * GetLine() { return buf_ + prevPos; }
    std::string GetLine() { return std::string(buf_ + prevPos); };

    HTTPHandler::ErrorType MainStateMachine();

    HTTPHandler::ErrorType DoRequest();

    void Process();

    void SetClientFd(int fd);
    void SetEpollFd(int fd);

    std::string & GetResponse();
};

#endif