#include "HTTPHandler.h"
#include "epoll.h"

#define MAXLINE 8192
HTTPHandler::HTTPHandler() {
    clientfd_ = 0;
    mainstate_ = CheckRequestLine;
    followstate_ = LineOk;
    static_ = true;
    epollfd_ = 0;
    method_ = "";    
    path_ = "";
    version_ = "";
    cgiargs_ = "";
    filename_ = "";
    filetype_ = "";
    responsebody_ = "";
    response_ = "";

    prevPos = 0;
    currentPos = 0;
    longConnect = false;
    contentLength = 0;

    memset(buf_, 0, sizeof(buf_));

    prevPos = 0;
    currentPos = 0;

    bufSize = 0;

    contentLength = 0;
    longConnect = true;
}

HTTPHandler::HTTPHandler(int fd) {
    clientfd_ = fd;
    mainstate_ = CheckRequestLine;
    followstate_ = LineOk;
    static_ = true;
    epollfd_ = 0;
    method_ = "";    
    path_ = "";
    version_ = "";
    cgiargs_ = "";
    filename_ = "";
    filetype_ = "";
    responsebody_ = "";
    response_ = "";

    prevPos = 0;
    currentPos = 0;
    longConnect = false;
    contentLength = 0;

    memset(buf_, 0, sizeof(buf_));

    prevPos = 0;
    currentPos = 0;

    bufSize = 0;

    contentLength = 0;
    longConnect = true;
}

HTTPHandler::HTTPHandler(const HTTPHandler & httphandler) : 
clientfd_(httphandler.clientfd_),
mainstate_(httphandler.mainstate_),
followstate_(httphandler.followstate_),
epollfd_(httphandler.epollfd_),
method_(httphandler.method_),
path_(httphandler.path_),
version_(httphandler.version_),
cgiargs_(httphandler.cgiargs_),
filename_(httphandler.filename_),
filetype_(httphandler.filetype_),
prevPos(httphandler.prevPos),
currentPos(httphandler.currentPos),
bufSize(httphandler.bufSize),
longConnect(httphandler.longConnect)
{}

HTTPHandler::~HTTPHandler() {
    clientfd_ = 0;
    mainstate_ = 0;
    followstate_ = 0;
    static_ = 0;
    epollfd_ = 0;
    method_ = "";    
    path_ = "";
    version_ = "";
    cgiargs_ = "";
    filename_ = "";
    filetype_ = "";
    responsebody_ = "";
    response_ = "";

    prevPos = 0;
    currentPos = 0;
    longConnect = 0;
    contentLength = 0;

    memset(buf_, 0, sizeof(buf_));

    prevPos = 0;
    currentPos = 0;

    bufSize = 0;

    contentLength = 0;
    longConnect = 0;

}

int HTTPHandler::GetClientFd() {
    return clientfd_;
}

std::string & HTTPHandler::GetMethod() {
    return method_;
}

std::string & HTTPHandler::GetPath() {
    return path_; 
}

std::string & HTTPHandler::GetVersion() {
    return version_;
}

std::string & HTTPHandler::GetResponseBody() {
    return responsebody_;
}

std::string & HTTPHandler::GetFileType() {
    return filetype_;
}

/*
int HTTPHandler::HttpRead(int CFd, void * Buf, size_t Count) {
    char * TmpBuf = (char *)Buf;
    size_t LeftNum = Count; 
    size_t ReadNum = 0;

    while (LeftNum > 0) {
        ssize_t TmpRead = 0;

        // TmpRead = recv(CFd, TmpBuf, LeftNum, 0);
        TmpRead = recv(CFd, TmpBuf, LeftNum, 0);
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
*/

bool HTTPHandler::SendResponse(int CFd, const std::string & ResponseCode, const std::string & ResponseMsg,
                        const std::string & ResponseBodyType,const std::string & ResponseBody) {
    std::stringstream Stream;
    Stream << "HTTP/1.1" << " " << ResponseCode << " " << ResponseMsg << "\r\n";
    if (longConnect)
        Stream << "Connection: keep-alive" << "\r\n";
    else
        Stream << "Connection: close" << "\r\n";
    Stream << "HTTPHandler: WebHTTPHandler/1.1" << "\r\n";
    Stream << "Content-length: " << ResponseBody.size() << "\r\n";
    Stream << "Content-type: " << ResponseBodyType << "\r\n";
    Stream << "\r\n";
    Stream << ResponseBody;

    response_ = Stream.str();
    // std::string && Response = Stream.str();

    send(CFd, response_.c_str(), response_.size(), MSG_SYN);
    // write(CFd, Response.c_str(), Response.size());

    std::clog << "================================== Response Packet ===================================" << std::endl;
    std::clog << response_.c_str() << std::endl;

    return ResponseBody.size() != 0;
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

bool HTTPHandler::HandleError(int CFd, HTTPHandler::ErrorType ErrorCode) {
    std::string ErrorMsg;
    switch (ErrorCode) {
    case ErrSuccess:
        break;
    case ErrForBid:
        SendErrorResponse(CFd, "403", "ForBid");
        return true;
        break;
    case ErrSendResponseFail:
        std::clog << "Send Response Failed" << std::endl;
        return true;
        break;
    case ErrNotFound:
        SendErrorResponse(CFd, "404", "Not Found");
        return true;
        break;
    case ErrInternalError:
        SendErrorResponse(CFd, "500", "Internal Error");
        return true;
        break;
    case ErrImplemented:
        SendErrorResponse(CFd, "501", "Not Implemented");
        return true;
        break;
    case ErrVersionNotSupported:
        SendErrorResponse(CFd, "505", "version_ Not Supported");
        return true;
        break;
    case ErrNoRequest:
    case ErrGetRequest:
        break;
    }

    return false;
}

void HTTPHandler::ParseFileType(const std::string & FN) {
    if ((int)FN.find(".html") > 0)
        filetype_ = "text/html";
    else if ((int)FN.find(".gif") > 0)
        filetype_ = "image/gif";
    else if ((int)FN.find(".jpg") > 0)
        filetype_ = "image/jpg";
    else if ((int)FN.find(".png") > 0)
        filetype_ = "image/png";
    else if ((int)FN.find(".ico") > 0)
        filetype_ = "image/ico";
    else if ((int)FN.find(".mp4") > 0)
        filetype_ = "video/mp4";
    else
        filetype_ = "text/plain";
}

/*
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
*/

void HTTPHandler::ParseRequest(int CFd) {
    char Buf[MAXLINE];

    int Len = read(CFd, Buf, MAXLINE);

    std::string Request(Buf, Buf + Len);
    size_t RequestLineEnd, MethodEnd, path_End, HeaderLineEnd;

    RequestLineEnd = Request.find("\r\n");

    std::string && RequestLine = Request.substr(0, RequestLineEnd);

    MethodEnd = RequestLine.find(' ');

    method_ = RequestLine.substr(0, MethodEnd);

    MethodEnd++;
    path_End = RequestLine.find(' ', MethodEnd);

    path_ = RequestLine.substr(MethodEnd, path_End - MethodEnd);

    path_End++;
    version_ = RequestLine.substr(path_End, RequestLine.size() - path_End);

    std::clog << "[*] Method: " << method_ << std::endl;
    std::clog << "[*] path_: " << path_ << std::endl;
    std::clog << "[*] HTTP version_: " << version_ << std::endl;

    HeaderLineEnd = RequestLineEnd;

    while (HeaderLineEnd < Request.size()) {
        size_t LineEnd = HeaderLineEnd + 2;
        HeaderLineEnd = Request.find("\r\n", LineEnd);
        std::string HeaderTmp = Request.substr(LineEnd, HeaderLineEnd - LineEnd);
        if (HeaderTmp.compare("") == 0) break;
        std::clog << "[*] [" << HeaderTmp << "]" << std::endl;
    }
}

void HTTPHandler::ParseUri(int CFd, const std::string & path_) {
    if (path_.find("cgi-bin") != 0) {
        cgiargs_ = "";
        filename_ = "." + path_;
        if (path_[path_.size() - 1] == '/')
            filename_ += "home.html";
    } else {
        static_ = false;
        size_t BinEnd = path_.find('?');
        if (BinEnd) {
            cgiargs_ = path_.substr(BinEnd + 1, path_.size() - BinEnd);
        } else {
            cgiargs_ = "";
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
    
    if (path_.find("cgi-bin") != 0) {
        cgiargs_ = "";
        filename_ = "." + path_;
        if (path_[path_.size() - 1] == '/')
            filename_ += "home.html";
    } else {
        static_ = false;
        size_t BinEnd = path_.find('?');
        if (BinEnd) {
            cgiargs_ = path_.substr(BinEnd + 1, path_.size() - BinEnd);
        } else {
            cgiargs_ = "";
        }
    }

    if (stat(filename_.c_str(), &SBuf) < 0)
        return ErrNotFound;
    
    if (static_) {
        if (!(S_ISREG(SBuf.st_mode)) || !(S_IRUSR & SBuf.st_mode))
            return ErrForBid;
        ParseFileType(filename_);
        int SrcFd = open(filename_.c_str(), O_RDONLY);
        void * Addr = mmap(nullptr, SBuf.st_size, PROT_READ, MAP_PRIVATE, SrcFd, 0);
        char * FileDataPtr = static_cast<char *>(Addr);
        std::string Tmp(FileDataPtr, FileDataPtr + SBuf.st_size); 
        responsebody_ = Tmp;
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

HTTPHandler::FollowState HTTPHandler::ParseLine() {
    char tmp;

    // currentPos 记录现在遍历到的位置，bufSize 记录读取到的内容的大小
    for (; currentPos < bufSize; currentPos++) {
        tmp = buf_[currentPos];
        if (tmp == '\r') {
            // 检测 \r 之后是否还有数据如果没有数据说明数据不完整，则返回 LineOpen
            if (currentPos + 1 == bufSize) return LineOpen;
            else if (buf_[currentPos + 1] == '\n') {
                buf_[currentPos++] = '\0';
                buf_[currentPos++] = '\0';
                return LineOk;
            }
            return LineBad;
        } else if (tmp == '\n') {
            if (currentPos > 1 && buf_[currentPos - 1] == '\r') {
                buf_[currentPos - 1] = '\0';
                buf_[currentPos++] = '\0';
                return LineOk;
            }
            return LineBad;
        } else if (currentPos == bufSize - 1 && tmp != '\r') {
            currentPos++;
            return LineOk;
        }
    }

    return LineOpen;
}

HTTPHandler::ErrorType HTTPHandler::ParseRequestLine(std::string & requestline) {
    int methodPos = requestline.find(' ');
    method_ = requestline.substr(0, methodPos);

    int pathPos = requestline.find(' ', methodPos + 1);
    path_ = requestline.substr(methodPos + 1, pathPos - methodPos - 1);

    int versionPos = requestline.find('\r', pathPos + 1);
    version_ = requestline.substr(pathPos + 1, versionPos - pathPos - 1);

    mainstate_ = CheckHeader;

    return ErrSuccess;
}

HTTPHandler::ErrorType HTTPHandler::ParseHeader(std::string & requestheader) {
    int pos = requestheader.find(' ');
    std::string field = requestheader.substr(0, pos);

    if (requestheader == "") {
        if (contentLength != 0) {
            mainstate_ = CheckBody;
            return ErrNoRequest;
        }
        return ErrGetRequest;
    } else if ((field.compare("Connection:") == 0)) {
        longConnect = requestheader.substr(pos + 1, requestheader.size() - pos).compare("keep-alive") == 0 ?
                        true : false;
    } else if ((field.compare("Content-Length:") == 0)) {
        int pos = requestheader.find(' ');
        contentLength = atoi(requestheader.substr(pos + 1, requestheader.size() - pos).c_str());
    }

    return ErrNoRequest;
}

HTTPHandler::ErrorType HTTPHandler::ParseBody(std::string & requestbody) {
    if (currentPos != bufSize || buf_[currentPos] != '\0') {
        return ErrGetRequest;
    }

    return ErrNoRequest;
}

HTTPHandler::ErrorType HTTPHandler::MainStateMachine() {
    HTTPHandler::ErrorType ret = ErrNoRequest;
    std::string line;
    // bool flag = false;

    while ((mainstate_ == CheckRequestLine && (followstate_ = ParseLine()) == LineOk) || ((followstate_ = ParseLine() == LineOk))) {
        std::string line = GetLine();
        prevPos = currentPos;
        std::clog << "[*] [" << line << "]" << std::endl;
        switch (mainstate_) {
        case CheckRequestLine:
            if ((ret = ParseRequestLine(line)) != ErrSuccess)
                return ret;
            HandleError(clientfd_, Check(method_, path_, version_));
            break;
        case CheckHeader:
            if ((ret = ParseHeader(line)) == ErrGetRequest)
                // return ErrSuccess;
                SendResponse(clientfd_, "200", "OK", GetFileType(), GetResponseBody());
            break;
        case CheckBody:
            if ((ret = ParseBody(line)) != ErrGetRequest)
                // return ErrSuccess;
                SendResponse(clientfd_, "200", "OK", GetFileType(), GetResponseBody());
            break;
        default:
            return ErrInternalError;
            break;
        }
    }

    close(clientfd_);

    return ret;
}

/*
HTTPHandler::ErrorType HTTPHandler::DoRequest() {

    if (contentLength != 0) {

    } else {
        switch (requestmethod_) {
        case Get:
            break;
        case Post:

            break;
        case Head:

            break;
        default:
            break;
        }
    }

}
*/

void HTTPHandler::SetClientFd(int fd) {
    clientfd_ = fd;
}

void HTTPHandler::SetEpollFd(int fd) {
    epollfd_ = fd;
}

std::string & HTTPHandler::GetResponse() {
    return response_;
}

void HTTPHandler::Process() {
    // Epoll event(epollfd_);
    epoll_event ev;

    HTTPHandler::ErrorType readRet = MainStateMachine();
    if (readRet == ErrNoRequest) {
        ev.data.fd = clientfd_;
        ev.events = EPOLLIN;
        epoll_ctl(epollfd_, EPOLL_CTL_MOD, ev.data.fd, &ev);
        // event.Mod(clientfd_, EPOLLIN);
        return;
    }

    bool writeRet = SendResponse(clientfd_, "200", "OK", GetFileType(), GetResponseBody());
    if (writeRet) {
        ev.data.fd = clientfd_;
        ev.events = EPOLLOUT;
        epoll_ctl(epollfd_, EPOLL_CTL_MOD, ev.data.fd, &ev);
        // event.Mod(clientfd_, EPOLLOUT);
        return;
    }

    ev.data.fd = clientfd_;
    ev.events = EPOLLIN | EPOLLOUT;
    epoll_ctl(epollfd_, EPOLL_CTL_DEL, clientfd_, &ev);
}