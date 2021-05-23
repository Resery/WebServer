#include "csapp.h"

void doit(int fd);
void read_requsethdrs(rio_t *rp);
int parse_uri(char * uri, char * filenmae, char * cgiargs);
void serve_static(int fd, char * filename, int filesize);
void get_filetype(char * filename, char * filetype);
void serve_dynamic(int fd, char * filename, char *cgiargs);
void clienterror(int fd, char * cause, char * errnum, char *shormsg, char *longmsg);

int main(int argc, char ** argv) {
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(0);
    }

    listenfd = Open_listenfd(argv[1]);
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *)&clientlen);
        char ip[MAXLINE] = {0};
        printf("Connect From ");
        printf("%s : ", inet_ntop(AF_INET, &clientaddr.sin_addr, ip, MAXLINE));
        printf("%u\n", ntohs(clientaddr.sin_port));
        doit(connfd);
        Close(connfd);
    }
}
/**
 * Doit:
 * 
 * fd -> Socket File Description
 * 
 * 1. Read Request Line And Parse The Method , Uri And Protocol Version
 * 2. Parse Uri To Get Request File Or Cgi.Bin
 * 3. Need Check The Resource Whether Or Not Exist
 */
void doit(int fd) {
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    rio_t rio;

    // Read Request Line And Headers
    Rio_readinitb(&rio, fd);
    // Read Request Line
    Rio_readlineb(&rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, uri, version);
    // Echo Request Method
    printf("The Request Method: %s\n", method);
    // Echo Request URI
    printf("The Qequest Uri: %s\n", uri);
    // Check Browser Version
    printf("The Brower HTPP Version: %s\n\n", version);

    // Only Suport Get Method If Use another Method To Access Will Be Failed
    if (strcasecmp(method, "GET")) {
        clienterror(fd, method, "501", "Not Implemented",
                    "Tiny does not implement this method");
        return;
    }
    // Read Request Headers
    read_requesthdrs(&rio);

    // Parse URI from GET request
    is_static = parse_uri(uri, filename, cgiargs);
    // Check Request Resource Whether Or Not Exist
    if (stat(filename, &sbuf) < 0) {
        clienterror(fd, filename, "404", "Not Found",
                    "Tiny couldn't find this file");
        return;
    }

    // Don't Have CgiArgs So Is A Static Request
    if (is_static) {
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
            clienterror(fd, filename, "403", "Forbidden",
                        "Tiny couldn't read the file");
            return;
        }
        serve_static(fd, filename, sbuf.st_size);
    } else { // Invoking The BIn To Get The Response Value And Return
        // Check The Cgi.Bin Whether Or Nor Exist
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
            clienterror(fd, filename, "403", "Forbidden",
                        "Tiny couldn't run the CGI program");
            return;
        }
        serve_dynamic(fd, filename, cgiargs);
    }
}

/**
 * clienterrot
 * 
 * fd -> The Socket Description
 * cause -> The Reason Of This Mistake
 * errnum -> The Error Code
 * shortmsg -> The Short Error Message
 * longmsg -> The Long Error Message
 * 
 * This Function Will Explain The Reason Of This Mistake
 */ 
void clienterror(int fd, char * cause, char * errnum, char * shortmsg, char * longmsg) {
    char buf[MAXLINE], body[MAXBUF];

    // Build The Http Response Body
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}

/**
 * read_requesthdrs
 * 
 * rp -> The File Description Of The Request Buffer
 * 
 * We Don't Deal With The Request Body So Just Read It And Ignore It
 */
void read_requesthdrs(rio_t *rp) {
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    while (strcmp(buf, "\r\n"))
        Rio_readlineb(rp, buf, MAXLINE);
    return;
}

/**
 * parse_uri
 * 
 * uri -> The URI
 * filename -> Request Resource
 * cgiargs -> Wang To Invoked Bin
 * 
 * This Function Is parse The Uri To Get The Request Resource
 * Or Want To Invoked Bin
 */
int parse_uri(char * uri, char * filename, char * cgiargs) {
    char *ptr;
    
    if (!strstr(uri, "cgi-bin")) { // Static Access
        strcpy(cgiargs, "");
        // Strcat The Filename From Uri
        strcpy(filename, ".");
        strcat(filename, uri);
        // If The Path Is '/' Then To Access Home.html
        if (uri[strlen(uri) - 1] == '/')
            strcat(filename, "home.html");
        return 1;
    } else { // Dynamic Access
        // Find Which Bin That Want To Invoke
        ptr = index(uri, '?');
        if (ptr) {
            // Get The Cgi Args
            strcpy(cgiargs, ptr + 1);
            *ptr = '\0';
        } else // No Cgi Args
            strcpy(cgiargs, "");
        // Beacuse The Uri Is Be Truncated By '\0' So Now Uri Noly Point To The Cgi Path
        strcpy(filename, ".");
        strcpy(filename, uri);
        return 0;
    }
}

/**
 * get_filetype
 * 
 * filename -> The Request File Name
 * filetype -> Store Request File Type
 * 
 * This Function Is For Get The File Type
 */
void get_filetype(char * filename, char * filetype) {
    if (strstr(filename, ".html"))
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif"))
        strcpy(filetype, "image/gif");
    else if (strstr(filename, ".jpg"))
        strcpy(filetype, "image/jpg");
    else
        strcpy(filetype, "text/plain");
}

/**
 * serve_static
 * 
 * fd -> The Request File Description
 * filename -> The Request File Name
 * filesize -> The Request File Size
 * 
 * This Function Is For Return Static Response To Client
 */
void serve_static(int fd, char *filename, int filesize) {
    int srcfd;
    char * srcp, filetype[MAXLINE], buf[MAXBUF];

    // Send Response Headers To Client
    get_filetype(filename, filetype);
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
    sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
    sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
    Rio_writen(fd, buf, strlen(buf));

    // Send Response Body To Client
    srcfd = Open(filename, O_RDONLY, 0);
    srcp = (char *)Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    Close(srcfd);
    Rio_writen(fd, srcp, filesize);
    Munmap(srcp, filesize);
}

/**
 * serve_dynamic
 * 
 * fd -> The CGI Bin Description
 * filename -> The CGI BIn Filename
 * cgiargs -> The CGIArgs
 */
void serve_dynamic(int fd, char * filename, char * cgiargs) {
    char buf[MAXLINE], *emptylist[] = { NULL };
    
    // Return First Part Of HTTP Response
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: Tiny Web Server\r\n");
    Rio_writen(fd, buf, strlen(buf));

    if (Fork == 0) {
        // Run The File Name And Echo Output To The Buf
        setenv("QUERY_STRING", cgiargs, 1);
        Dup2(fd, STDOUT_FILENO);
        Execve(filename, emptylist, environ);
    }
    Wait(NULL);
}