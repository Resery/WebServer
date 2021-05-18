#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() {
    int fd = open("/home/WebServer/Preview/Process/ThirdTest", O_RDWR);
    lseek(fd, -6, SEEK_END);
    
    pid_t child = fork();

    if (child == 0) {
        char buf[6];
        read(fd, buf, 6);

        std::cout << buf << std::endl;
    }
}