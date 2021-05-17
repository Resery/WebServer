#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

std::string filename = "Test";

int fd = 0;

void Seek_Set() {
    if ((fd = open(filename.c_str(), O_RDWR)) == -1) {
        perror("Open Failed");
    }

    char rbuf[20];

    lseek(fd, 7, SEEK_SET);
    read(fd, rbuf, 20);
    *(rbuf + 20) = '\0';
    std::cout << rbuf << std::endl;

    close(fd);
}

void Seek_Cur() {
    if ((fd = open(filename.c_str(), O_RDWR)) == -1) {
        perror("Open Failed");
    }

    char rbuf[23];

    lseek(fd, 4, SEEK_CUR);
    read(fd, rbuf, 23);
    *(rbuf + 23) = '\0';
    std::cout << rbuf << std::endl;

    close(fd);
}

void Seek_End() {
    if ((fd = open(filename.c_str(), O_RDWR)) == -1) {
        perror("Open Failed");
    }

    char rbuf[10];

    lseek(fd, -8, SEEK_END);
    read(fd, rbuf, 8);
    *(rbuf + 8) = '\0';
    std::cout << rbuf;
    
    close(fd);
}

void Seek_Data() {
    if ((fd = open(filename.c_str(), O_RDWR)) == -1) {
        perror("Open Failed");
    }

    char rbuf[20];

    lseek(fd, 7, SEEK_DATA);
    read(fd, rbuf, 20);
    *(rbuf + 20) = '\0';
    std::cout << rbuf << std::endl;
    
    close(fd);
}

void Seek_Hole() {
    if ((fd = open(filename.c_str(), O_RDWR)) == -1) {
        perror("Open Failed");
    }

    char rbuf[13];

    lseek(fd, 14, SEEK_HOLE);
    read(fd, rbuf, 13);
    *(rbuf + 13) = '\0';
    std::cout << rbuf << std::endl;

    close(fd);
}

int main() {
    Seek_Set();
    Seek_Cur();
    Seek_End();
    Seek_Data();
    Seek_Hole();
}