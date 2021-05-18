#include <iostream>
#include <unistd.h>
#include <sys/types.h>

int Global = 0xdeadbeef;

int main() {
    pid_t child = fork();

    if (child > 0) {
        std::cout << "==========================" << std::endl;
        std::cout << "This is father process" << std::endl;
        std::cout << "Origin Global Is 0x" << std::hex << Global << std::endl;
		std::cout << "Origin Global Address Is 0x" << std::hex << &Global << std::endl;
        Global = 0xaeadbeef;
        std::cout << "New Global Is 0x" << std::hex << Global << std::endl;
        std::cout << "==========================" << std::endl;
    } else if (child == 0) {
        std::cout << "This is child process" << std::endl;
        std::cout << "Origin Global Is 0x" << std::hex << Global << std::endl;
		std::cout << "Origin Global Address Is 0x" << std::hex << &Global << std::endl;
        std::cout << "==========================" << std::endl;
    }
}