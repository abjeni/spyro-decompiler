#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#define BREAKPOINT \
do { \
  fflush(stdout); \
  assert(0); \
} while(0);

typedef struct {
  char *file;
  uint32_t line;
} file_loc;

void print_trace (void);
void print_gte_regs();
void print_all_regs();