#include "HTTPHandler.h"

#define MAXLINE 8192

HTTPHandler::HTTPHandler(int Fd) {
    ClientFd = Fd;
    Static = true;
}

HTTPHandler::HTTPHandler(const HTTPHandler & httphandler) : 
ClientFd(httphandler.ClientFd),
Method(httphandler.Method),
Path(httphandler.Path),
Version(httphandler.Version),
CgiArgs(httphandler.CgiArgs),
FileName(httphandler.FileName),
FileType(httphandler.FileType)
{}

HTTPHandler::~HTTPHandler() {
}

int HTTPHandler::GetClientFd() {
    return ClientFd;
}

std::string & HTTPHandler::GetMethod() {
    return Method;
}

std::string & HTTPHandler::GetPath() {
    return Path; 
}

std::string & HTTPHandler::GetVersion() {
    return Version;
}

std::string & HTTPHandler::GetResponseBody() {
    return ResponseBody;
}

std::string & HTTPHandler::GetFileType() {
    return FileType;
}

int HTTPHandler::HttpRead(int CFd, void * Buf, size_t Count) {
    char * TmpBuf = (char *)Buf;
    size_t LeftNum = Count; 
    size_t ReadNum = 0;

    while (LeftNum > 0) {
        ssize_t TmpRead = 0;

        TmpRead = recv(CFd, TmpBuf, LeftNum, MSG_DONTWAIT);
        // TmpRead = read(CFd, TmpBuf, LeftNum);

        if (TmpRead < 0) {
            if (errno == EINTR) TmpRead = 0;
            else if (errno == EAGAIN) return ReadNum;
            else return -1;
        }

        if (TmpRead == 0) break;

        ReadNum += TmpRead;
        TmpBuf += TmpRead;
        LeftNum -= TmpRead;
    }

    return ReadNum;
}

int HTTPHandler::HttpWrite(int CFd, const void * Buf, size_t Count) {
    char * TmpBuf = (char *)Buf;
    size_t LeftNum = Count; 
    size_t WriteNum = 0;

    while (LeftNum > 0) {
        ssize_t TmpWrite = 0;

        TmpWrite = send(CFd, TmpBuf, LeftNum, 0);
        // TmpWrite = write(CFd, TmpBuf, LeftNum);

        if (TmpWrite < 0) {
            if (errno == EINTR) TmpWrite = 0;
            else if (errno == EAGAIN) return WriteNum;
            else return -1;
        }

        if (TmpWrite == 0) break;

        WriteNum += TmpWrite;
        TmpBuf += TmpWrite;
        LeftNum -= TmpWrite;
    }

    return WriteNum;
}

void HTTPHandler::SendResponse(int CFd, const std::string & ResponseCode, const std::string & ResponseMsg,
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

    write(CFd, Response.c_str(), Response.size());

    std::clog << "================================== Response Packet ===================================" << std::endl;
    std::clog << Response.c_str() << std::endl;
}

void HTTPHandler::SendErrorResponse(int CFd, const std::string & ErrorCode, const std::string & ErrorMsg) {
    std::string ErrorString = ErrorCode + " " + ErrorMsg;
    std::string ResponseBody = 
                "<html>"
                "<title>" + ErrorString + "</title>"
                "<body>" + ErrorString + 
                    "<hr><em> Resery's Web HTTPHandler</em>"
                "</body>"
                "</html>";
    SendResponse(CFd, ErrorCode, ErrorMsg, "text/html", ResponseBody);
}

void HTTPHandler::HandleError(int CFd, HTTPHandler::ErrorType ErrorCode) {
    std::string ErrorMsg;
    switch (ErrorCode) {
    case ErrSuccess:
        break;
    case ErrForBid:
        SendErrorResponse(CFd, "403", "ForBid");
        break;
    case ErrSendResponseFail:
        std::clog << "Send Response Failed" << std::endl;
        break;
    case ErrNotFound:
        SendErrorResponse(CFd, "404", "Not Found");
        break;
    case ErrImplemented:
        SendErrorResponse(CFd, "501", "Not Implemented");
        break;
    case ErrVersionNotSupported:
        SendErrorResponse(CFd, "505", "Version Not Supported");
        break;
    }
}

void HTTPHandler::ParseFileType(const std::string & FN) {
    if (FN.find(".html") != 0)
        FileType = "text/html";
    else if (FN.find(".gif") != 0)
        FileType = "image/gif";
    else if (FN.find(".jpg") != 0)
        FileType = "image/jpg";
    else if (FN.find(".png") != 0)
        FileType = "image/png";
    else
        FileType = "text/plain";
}

void HTTPHandler::ReadRequest(int CFd, std::string & Request) {
    char Buffer[MAXLINE];

    while (1) {
        ssize_t Len = HttpRead(CFd, Buffer, MAXLINE);

        if (Len < 0) return;
        else if (Len == 0) {
            if (errno == EAGAIN) return;
            assert(errno == 0 || errno == ENOENT);
            return;
        }

        std::string RequestBuf(Buffer, Buffer + Len);
        Request += RequestBuf;
    }
    
}

void HTTPHandler::ParseRequest(int CFd) {
    char Buf[MAXLINE];

    int Len = read(CFd, Buf, MAXLINE);

    std::string Request(Buf, Buf + Len);
    size_t RequestLineEnd, MethodEnd, PathEnd, HeaderLineEnd;

    RequestLineEnd = Request.find("\r\n");

    std::string && RequestLine = Request.substr(0, RequestLineEnd);

    MethodEnd = RequestLine.find(' ');

    Method = RequestLine.substr(0, MethodEnd);

    MethodEnd++;
    PathEnd = RequestLine.find(' ', MethodEnd);

    Path = RequestLine.substr(MethodEnd, PathEnd - MethodEnd);

    PathEnd++;
    Version = RequestLine.substr(PathEnd, RequestLine.size() - PathEnd);

    std::clog << "[*] Method: " << Method << std::endl;
    std::clog << "[*] Path: " << Path << std::endl;
    std::clog << "[*] HTTP Version: " << Version << std::endl;

    HeaderLineEnd = RequestLineEnd;

    while (HeaderLineEnd < Request.size()) {
        size_t LineEnd = HeaderLineEnd + 2;
        HeaderLineEnd = Request.find("\r\n", LineEnd);
        std::string HeaderTmp = Request.substr(LineEnd, HeaderLineEnd - LineEnd);
        if (HeaderTmp.compare("") == 0) break;
        std::clog << "[*] [" << HeaderTmp << "]" << std::endl;
    }
}

void HTTPHandler::ParseUri(int CFd, const std::string & Path) {
    if (Path.find("cgi-bin") != 0) {
        CgiArgs = "";
        FileName = "." + Path;
        if (Path[Path.size() - 1] == '/')
            FileName += "home.html";
    } else {
        Static = false;
        size_t BinEnd = Path.find('?');
        if (BinEnd) {
            CgiArgs = Path.substr(BinEnd + 1, Path.size() - BinEnd);
        } else {
            CgiArgs = "";
        }
    }
}

HTTPHandler::ErrorType HTTPHandler::Check(const std::string & method,
                                            const std::string & path,
                                            const std::string & version) {
    HTTPHandler::ErrorType Errno = ErrSuccess;
    if ((Errno = CheckMethod(method)) != ErrSuccess) return Errno;
    else if ((Errno = CheckPath(path)) != ErrSuccess) return Errno;
    else if ((Errno = CheckVersion(version)) != ErrSuccess) return Errno;
    else return Errno;
}

HTTPHandler::ErrorType HTTPHandler::CheckMethod(const std::string & method) {
    if (method.compare("GET") != 0 &&
        method.compare("POST") != 0 &&
        method.compare("HEAD") != 0)
        return ErrImplemented;
    
    return ErrSuccess;
}

HTTPHandler::ErrorType HTTPHandler::CheckPath(const std::string & path) {
    struct stat SBuf;

    if (stat(FileName.c_str(), &SBuf) < 0)
        return ErrNotFound;
    
    if (Static) {
        if (!(S_ISREG(SBuf.st_mode)) || !(S_IRUSR & SBuf.st_mode))
            return ErrForBid;
        ParseFileType(FileName);
        int SrcFd = open(FileName.c_str(), O_RDONLY);
        void * Addr = mmap(nullptr, SBuf.st_size, PROT_READ, MAP_PRIVATE, SrcFd, 0);
        char * FileDataPtr = static_cast<char *>(Addr);
        std::string Tmp(FileDataPtr, FileDataPtr + SBuf.st_size); 
        ResponseBody = Tmp;
        close(SrcFd);
        munmap(Addr, SBuf.st_size);
    } else {
        if (!(S_ISREG(SBuf.st_mode)) || !(S_IXUSR & SBuf.st_mode))
            return ErrForBid;
    }

    return ErrSuccess;
}

HTTPHandler::ErrorType HTTPHandler::CheckVersion(const std::string & version) {
    if (version.compare("HTTP/1.0") != 0 &&
        version.compare("HTTP/1.1") != 0)
        return ErrVersionNotSupported;

    return ErrSuccess;
}