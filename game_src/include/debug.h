#pragma once

#include <assert.h>
#include <stdio.h>
#include <signal.h>
#include <stdint.h>

#define BREAKPOINT \
do { \
  print_trace(); \
  fflush(stdout); \
  raise(SIGABRT); \
} while(0);

#define UNREACHABLE \
do { \
  print_trace(); \
  fflush(stdout); \
  assert(0); \
} while(0);

#define DEPRECATED \
do { \
  print_trace(); \
  fflush(stdout); \
  assert(0); \
} while(0);

typedef struct {
  const char *file;
  uint32_t line;
} file_loc;

void print_trace(void);
void print_gte_regs(void);
void print_all_regs(void);