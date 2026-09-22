
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>

int abs(int j) {
    return j >= 0 ? j : -j;
}

void exit(int status) {
    (void)status;
    print("exit");
    __asm__(
        "unreachable"
    );
    while (1);
}

int creat(const char *path, mode_t mode) {
    (void)path;
    (void)mode;
    return -1;
}

int open(const char *path, int flags) {
    (void)path;
    (void)flags;
    return -1;
}

int raise(int sig) {
    (void)sig;
    print("raising");
    __asm__(
        "unreachable"
    );
    while (1);
    return 0;
}

int fstat(int fildes, struct stat *buf) {
    (void)fildes;
    (void)buf;
    return -1;
}