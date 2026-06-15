
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>

int abs(int j) {
    return j >= 0 ? j : -j;
}

void exit(int status) {
    print("exit");
    __asm__(
        "unreachable"
    );
    while (1);
}

int creat(const char *path, mode_t mode) {
    return -1;
}

int open(const char *path, int flags) {
    return -1;
}

int raise(int sig) {
    print("raising");
    __asm__(
        "unreachable"
    );
    while (1);
    return 0;
}

int fstat(int fildes, struct stat *buf) {
    return -1;
}