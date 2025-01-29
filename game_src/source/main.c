#include <execinfo.h>
#include <assert.h>
#include <stdint.h>
#include <unistd.h>

#include "psx_mem.h"

uint32_t v0, v1, k0, k1, a0, a1, a2, a3;
uint32_t t0, t1, t2, t3, t4, t5, t6, t7, t8, t9;
uint32_t s0, s1, s2, s3, s4, s5, s6, s7;
uint32_t at, gp, sp, fp, ra, pc, hi, lo;

void test(void);

void function_8005B8E0(void);

int main(int argc, char *argv[])
{
  int err = chdir("..");
  if (err) return err;
  
  init_psx_mem();

  sw(0x80074E40, 2); // PSY-Q debug level

  function_8005B8E0();
  return 0;
}