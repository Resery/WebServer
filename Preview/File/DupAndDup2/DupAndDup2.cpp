#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

std::string filename = "/home/WebServer/Preview/File/DupAndDup2/Test";

void Dup() {
    std::cout << "===========Dup============" << std::endl;
    int fd = 0;

    if ((fd = open(filename.c_str(), O_RDWR)) == -1) {
        perror("Open Test Failed");
    }

    std::cout << "Origin Fd Is: " << fd << std::endl;

    int CopyFd = dup(fd);

    std::cout << "Copy Fd Is: " << CopyFd << std::endl;

    char buf[6];
    read(CopyFd, buf, 6);

    std::cout << buf << std::endl;
    std::cout << "==========================" << std::endl;
}

void Dup2() {
    std::cout << "===========Dup2===========" << std::endl;

    int fd = 0;

    if ((fd = open(filename.c_str(), O_RDWR)) == -1) {
        perror("Open Test Failed");
    }

    std::cout << "Origin Fd Is: " << fd << std::endl;

    int CopyFd = dup2(fd, fd + 4);

    std::cout << "Copy Fd Is: " << CopyFd << std::endl;

    char buf[6];
    read(CopyFd, buf, 6);

    std::cout << buf << std::endl;
    std::cout << "==========================" << std::endl;
}

int main() {
    Dup();
    Dup2();
}