#include "HTTPHandler.h"

#define MAXLINE 8192

HTTPHandler::HTTPHandler(int Fd) {
    ConnFd = Fd;
    FileName = (char *)malloc(MAXLINE);
    FileType = (char *)malloc(MAXLINE);
    CgiArgs = (char *)malloc(MAXLINE);
}

HTTPHandler::~HTTPHandler() {
    free(FileName);
    free(FileType);
    free(CgiArgs);
}

int HTTPHandler::GetConnFd() {
    return ConnFd;
}

void HTTPHandler::SendResponse(int ClientFd, const std::string & ResponseCode, const std::string & ResponseMsg,
                        const std::string & ResponseBodyType,const std::string & ResponseBody) {
    std::stringstream Stream;
    Stream << "HTTP/1.1" << " " << ResponseCode << " " << ResponseMsg << "\r\n";
    Stream << "Connection: " << "Close" << "\r\n";
    Stream << "HTTPHandler: WebHTTPHandler/1.1" << "\r\n";
    Stream << "Content-length: " << ResponseBody.size() << "\r\n";
    Stream << "Content-type: " << ResponseBodyType << "\r\n";
    Stream << "\r\n";
    Stream << ResponseBody;

    std::string && Response = Stream.str();

    write(ClientFd, Response.c_str(), Response.size());

    std::clog << "================================== Response Packet ===================================" << std::endl;
    std::clog << Response.c_str() << std::endl;
}

void HTTPHandler::SendErrorResponse(int ClientFd, const std::string & ErrorCode, const std::string & ErrorMsg) {
    std::string ErrorString = ErrorCode + " " + ErrorMsg;
    std::string ResponseBody = 
                "<html>"
                "<title>" + ErrorString + "</title>"
                "<body>" + ErrorString + 
                    "<hr><em> Resery's Web HTTPHandler</em>"
                "</body>"
                "</html>";
    SendResponse(ClientFd, ErrorCode, ErrorMsg, "text/html", ResponseBody);
}

void HTTPHandler::ClientError(int ClientFd, HTTPHandler::ErrorType ErrorCode) {
    std::string ErrorMsg;
    switch (ErrorCode) {
    case ErrorNotFound:
        SendErrorResponse(ClientFd, "404", "Not Found");
        break;
    case ErrorImplemented:
        SendErrorResponse(ClientFd, "501", "Not Implemented");
    }
}

void HTTPHandler::GetFileType(char * FileName) {
    if (strstr(FileName, ".html"))
        strcpy(FileType, "text/html");
    else if (strstr(FileName, ".gif"))
        strcpy(FileType, "image/gif");
    else if (strstr(FileName, ".jpg"))
        strcpy(FileType, "image/jpg");
    else if (strstr(FileName, ".png"))
        strcpy(FileType, "image/png");
    else
        strcpy(FileType, "text/plain");
}

void HTTPHandler::ParseUri(int ConnFd, char * Path) {
    char * ptr;
    struct stat SBuf;
    bool IsStaic = true;

    if (!strstr(Path, "cgi-bin")) {
        strcpy(CgiArgs, "");
        strcpy(FileName, ".");
        strcat(FileName, Path);
        
        if (Path[strlen(Path) - 1] == '/')
            strcat(FileName, "home.html");
    } else {
        IsStaic = false;
        ptr = index(Path, '?');
        if (ptr) {
            strcpy(CgiArgs, ptr + 1);
            *ptr = '\0';
        } else
            strcpy(CgiArgs, "");
        strcpy(FileName, ".");
        strcpy(FileName, Path);
    }

    if (stat(FileName, &SBuf) < 0)
        SendErrorResponse(ConnFd, "404", "Not Found");
    
    if (IsStaic) {
        if (!(S_ISREG(SBuf.st_mode)) || !(S_IRUSR & SBuf.st_mode))
            SendErrorResponse(ConnFd, "403", "Forbidden");
        GetFileType(FileName);
        int SrcFd = open(FileName, O_RDONLY);
        void * Addr = mmap(nullptr, SBuf.st_size, PROT_READ, MAP_PRIVATE, SrcFd, 0);
        char * FileDataPtr = static_cast<char *>(Addr);
        std::string ResponseBody(FileDataPtr, FileDataPtr + SBuf.st_size);
        SendResponse(ConnFd, "200", "OK", FileType, ResponseBody);
        close(SrcFd);
        munmap(Addr, SBuf.st_size);
    } else {
        if (!(S_ISREG(SBuf.st_mode)) || !(S_IXUSR & SBuf.st_mode))
            SendErrorResponse(ConnFd, "403", "Forbidden");
    }
}

void HTTPHandler::ParseRequest(int ConnFd) {
    char Buf[MAXLINE];
    char Method[MAXLINE], Path[MAXLINE], Version[MAXLINE];
    char Header[MAXLINE];

    read(ConnFd, Buf, MAXLINE);

    sscanf(Buf, "%s %s %s\r\n", Method, Path, Version);

    if (strcmp(Method, "GET") != 0) {
        ClientError(ConnFd, ErrorImplemented);
        exit(0);
    }

    std::clog << "[*] Method: " << Method << std::endl;
    std::clog << "[*] Path: " << Path << std::endl;
    std::clog << "[*] HTTP Version: " << Version << std::endl;

    for (int i = 0; i < MAXLINE; i++) {
        if (Buf[i] == '\r' && Buf[i + 1] == '\n') {
            memcpy(Header, Buf + i + 2, MAXLINE);
            break;
        }
    }

    char * tmp = strtok(Header, "\r\n");
    while (tmp != NULL) {
        std::clog << "[*] [" << tmp << "]" << std::endl;
        tmp = strtok(NULL, "\r\n");
    }

    ParseUri(ConnFd, Path);
}
