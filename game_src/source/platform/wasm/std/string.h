#pragma once

#include <stddef.h>
#include <stdarg.h>

char *strerror(int no);

void *memset(void *s, int c, size_t n);

void *memcpy(void *dest, const void *src, size_t n);

void *memmove(void *dest, const void *src, size_t n);

size_t strlen(const char *s);

int vsprintf(char * buf, const char * fmt, va_list va);

int sprintf(char * buf, const char * fmt, ...);