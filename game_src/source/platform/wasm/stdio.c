
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_SMALL_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_ALT_FORM_FLAG 1
#define NANOPRINTF_IMPLEMENTATION
#include <nanoprintf.h>

char temp_buf[0x100];

extern void console_log(const char *msg, int size);

size_t strlen(const char *s) {
    size_t len = 0;
    while (*s++) len++;
    return len;
}

void print(const char *msg)
{
    console_log(msg, strlen(msg));
}

void fflush(FILE *stream) {

}

void puts(char *s) {
    print(s);
}

int chdir(const char *path) {
    return 0;
}

void *memset(void *s, int c, size_t n) {

    for (int i = 0; i < n; i++)
        ((char *)s)[i] = (char)c;

    return s;
}

void *memcpy(void *dest, const void *src, size_t n) {
    for (int i = 0; i < n; i++) {
        ((char *)dest)[i] = ((char *)src)[i];
    }
    return dest;
}

void *memmove(void *dest, const void *src, size_t n) {

    if (dest < src) {
        return memcpy(dest, src, n);
    }

    for (int i = n-1; i >= 0; i--) {
        ((char *)dest)[i] = ((char *)src)[i];
    }
    return dest;
}

int vsprintf(char * buf, const char * fmt, va_list va)
{
    return npf_vsnprintf(buf, 1024*1024, fmt, va);
}

int sprintf(char * buf, const char * fmt, ...)
{
    va_list va;
    va_start(va,fmt);
    int ret = vsprintf(buf, fmt, va);
    va_end(va);
    return ret;
}

void printf(char *fmt, ...) {
    va_list va;
    va_start(va,fmt);
    vsprintf(temp_buf, fmt, va);
    va_end(va);
    print(temp_buf);
}

ssize_t read(int fd, void *buf, size_t count) {
    return -1;
}

ssize_t write(int fd, const void *buf, size_t count) {
    return -1;
}

int close(int fd) {
    return -1;
}

off_t lseek(int fd, off_t offset, int whence) {
    return -1;
}