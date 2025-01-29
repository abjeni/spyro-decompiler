#include <main.h>
#include <stdlib.h>
#include <stdio.h>
#include <execinfo.h>

#include "debug.h"

void print_trace(void)
{
  void *array[10];
  char **strings;
  int size, i;

  size = backtrace (array, 10);
  strings = backtrace_symbols (array, size);
  if (strings != NULL)
  {

    printf ("Obtained %d stack frames.\n", size);
    for (i = 0; i < size; i++)
      printf ("%s\n", strings[i]);
  }

  free (strings);
}

void print_all_regs()
{
  printf("regs:\n"
  "pc: %.8x  s0: %.8X\n"
  "at: %.8X  s1: %.8X\n"
  "v0: %.8X  s2: %.8X\n"
  "v1: %.8X  s3: %.8X\n"
  "a0: %.8X  s4: %.8X\n"
  "a1: %.8X  s5: %.8X\n"
  "a2: %.8X  s6: %.8X\n"
  "a3: %.8X  s7: %.8X\n"
  "t0: %.8X  t8: %.8X\n"
  "t1: %.8X  t9: %.8X\n"
  "t2: %.8X  k0: %.8X\n"
  "t3: %.8X  k1: %.8X\n"
  "t4: %.8X  gp: %.8X\n"
  "t5: %.8X  sp: %.8X\n"
  "t6: %.8X  fp: %.8X\n"
  "t7: %.8X  ra: %.8X\n"
  "lo: %.8X  hi: %.8X\n",
  pc, s0, at, s1, v0, s2, v1, s3, a0, s4, a1, s5, a2, s6, a3, s7, t0,
  t8, t1, t9, t2, k0, t3, k1, t4, gp, t5, sp, t6, fp, t7, ra, lo, hi
  );
}