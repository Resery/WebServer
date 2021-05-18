# Process

这里主要列出两个关于进程的问题，关于 fork 出来的进程的相关信息可以阅读 man 手册，在写代码的时候可以参考

1. 如何循环生成n个子进程,并且这些子进程均为兄弟关系

```CPP
#include <iostream>
#include <unistd.h>
#include <sys/types.h>

int main() {
    for (int i = 0; i < 5; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            continue;
        } else if (pid > 0) {
            break;
        }
    }

    // 为了暂停程序以便于调用 pstree
    getchar();
}

Output:

root@971f1b1c5e99:/home/WebServer# pstree 2880
FirstQusetion---FirstQusetion---FirstQusetion---FirstQusetion---FirstQusetion---FirstQusetion
```

2. 父子进程是否共享全局变量，利用代码来验证，根据下面的代码可以判断出父子进程，是共享全局变量的，读时共享，写时复制

```CPP
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

Output:

==========================
This is father process
Origin Global Is 0xdeadbeef
Origin Global Address Is 0x0xaaaae7ca2010
New Global Is 0xaeadbeef
==========================
This is child process
Origin Global Is 0xdeadbeef
Origin Global Address Is 0x0xaaaae7ca2010
==========================
```

3. 父子进程是否共享文件描述符和文件偏移量，代码验证，根据输出可知如果是在创建子进程之前打开的文件描述符会共享并且也会共享文件偏移量，如果是在创建子进程之后打开的文件描述符那么不会共享

```CPP
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

Output:

esery

```