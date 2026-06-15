#pragma once

#include <sys/stat.h>

#define O_RDONLY	     00
#define O_WRONLY	     01
#define O_RDWR		     02

int creat(const char *path, mode_t mode);

int open(const char *path, int flags);