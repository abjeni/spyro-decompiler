#pragma once

#include <sys/types.h>

#define	S_IRUSR 0400
#define	S_IWUSR 0200

struct stat {
    off_t     st_size;
};

int fstat(int fildes, struct stat *buf);