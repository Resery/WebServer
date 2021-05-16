#include <iostream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

void Append() {
    int fd = 0;
    if ((fd = open("/home/WebServer/Preview/File/APPEND", O_RDWR | O_APPEND)) < 0) {
        perror("Can't open file APPEND");
    }
}

int main() {
    Append();
}