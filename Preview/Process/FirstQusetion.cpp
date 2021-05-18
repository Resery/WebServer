/*
 * 问题：如何循环生成 n 个子进程,并且这些子进程均为兄弟关系，这里设 n 为 5
 */

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

    getchar();
}