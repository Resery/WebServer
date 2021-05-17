#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

std::string prefix = "/home/WebServer/Preview/File/";

void RdOnly() {
    std::cout << "==========RdOnly==========" << std::endl;

    int fd = 0;
    // char * filename = strcat(, "RDONLY");
    std::string filename =  prefix + "RDONLY";

    if ((fd = open(filename.c_str(), O_RDONLY)) == -1) {
        perror("Open RDONLY Failed");
    }

    char buf[] = "Resery";
    if(write(fd, buf, sizeof(buf)) == -1) {
        std::cout << "[*] Beacuse Open Flags Is RDONLY So Write Failed" << std::endl;
    }

    close(fd);

    std::cout << "==========================" << std::endl;
}

void WrOnly() {
    std::cout << "==========WrOnly==========" << std::endl;

    int fd = 0;
    std::string filename = prefix + "WRONLY";

    if((fd = open(filename.c_str(), O_WRONLY)) == -1) {
        perror("Open WRONLY Failed");
    }

    char * buf = new char;
    if (read(fd, buf, 8) == -1) {
        std::cout << "[*] Beacuse Open Flags Is WRONLY So Read Failed" << std::endl;
    }

    delete buf;

    close(fd);

    std::cout << "==========================" << std::endl;
}

void Rdwr() {
    std::cout << "==========RDWR==========" << std::endl;

    int fd = 0;
    std::string filename = prefix + "RDWR";

    if ((fd = open(filename.c_str(), O_RDWR)) == -1) {
        perror("Open RDWR Failed");
    }

    char rbuf[6];
    char wbuf[] = "RESERY";

    std::cout << "[*] Starting Read" << std::endl;

    read(fd, rbuf, 6);
    std::cout << rbuf << std::endl;

    std::cout << "[*] Before Write" << std::endl;

    write(fd, wbuf, sizeof(wbuf));

    std::cout << "[*] After Write" << std::endl;

    read(fd, rbuf, 6);
    std::cout << wbuf << std::endl;

    std::cout << "========================" << std::endl;
}

void Append() {
    std::cout << "==========Append=========" << std::endl;

    int fd = 0;
    std::string filename = prefix + "APPEND";

    if ((fd = open(filename.c_str(), O_RDWR | O_APPEND)) < 0) {
        perror("Open APPEND Failed");
    }

    std::cout << "[*] Before write" << std::endl;

    char * rbuf = new char;
    read(fd, rbuf, sizeof(rbuf));
    std::cout << rbuf; 

    int rlen = sizeof(rbuf);

    delete rbuf;

    char wbuf[] = "Append Resery\n";
    write(fd, wbuf, sizeof(wbuf) - 1);

    lseek(fd, 0, SEEK_SET);

    std::cout << "[*] After write" << std::endl;

    char * afterwbuf = new char;
    read(fd, afterwbuf, rlen + sizeof(wbuf));
    *(afterwbuf + rlen + sizeof(wbuf) - 2) = '\0';
    std::cout << afterwbuf;

    delete afterwbuf;
    close(fd);

    std::cout << "==========================" << std::endl;
}

void Creat() {
    std::cout << "===========Creat==========" << std::endl;

    std::cout << "[*] Before Creat" << std::endl;

    system("ls -al CREAT");

    int fd = 0;
    std::string filename = prefix + "CREAT";

    if ((fd = open(filename.c_str(), O_RDWR | O_CREAT, 
                    S_IRWXU | S_IRWXG | S_IRWXO)) == 0) {
        perror("Open RDWR Failed");
    }

    std::cout << "[*] After Creat" << std::endl;

    system("ls -al CREAT");

    close(fd);

    std::cout << "==========================" << std::endl;
}

void Excl() {
    std::cout << "==========Excl==========" << std::endl;

    int fd = 0;
    std::string filename = prefix + "EXCL";

    if ((fd = open(filename.c_str(), O_RDWR | O_EXCL | O_CREAT)) < 0) {
        std::cout << "[*] Beacuse EXCL Is Exist So Failed" << std::endl;
    }

    close(fd);

    std::cout << "========================" << std::endl;
}

void Trunc() {
    std::cout << "===========Trunc=========" << std::endl;

    int fd = 0;
    std::string filename = prefix + "TRUNC";

    std::cout << "[*] Before Open Trunc Run Cat" << std::endl;

    system("cat TRUNC");

    if ((fd = open(filename.c_str(), O_RDWR | O_TRUNC)) == -1) {
        perror("Open TRUNC Failed");
    }

    std::cout << "[*] After Open Trunc Run Cat" << std::endl;

    system("cat TRUNC");

    close(fd);

    std::cout << "=========================" << std::endl;
}

int main() {
    // RdOnly();
    // WrOnly();
    // Rdwr();
    // Append();
    // Creat();
    // Excl();
    // Trunc();
}