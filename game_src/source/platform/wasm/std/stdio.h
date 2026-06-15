#pragma once

typedef struct {

} FILE;

extern FILE *stdout;

void fflush(FILE *stream);
void print(const char *msg);
void printf(char *fmt, ...);
void puts(char *s);