#include "debug.h"
#include "psx_mem.h"
#include "main.h"
#include "decompilation.h"
#include "not_renamed.h"

#include <string.h>

// small functions

// size: 0x00000010
void function_80017FD4(void)
{
  v0 = lhu(TIMER2);
}

// NOP
// size: 0x00000008
void function_8002C914(void)
{
  UNREACHABLE;
}

// NOP
// size: 0x00000008
void function_8002C91C(void)
{
  UNREACHABLE;
}

// size: 0x0000002C
void function_8002D554(void)
{
  memset(addr_to_pointer(0x80076BC0), 0, 0x40);
  sw(0x80075928, 0);
}

// NOP
// size: 0x00000020
void function_8002F3C4(void)
{
  function_8002C91C();
}

// NOP
// size: 0x00000008
void function_80037E98(void)
{
  UNREACHABLE;
}

// size: 0x00000018
void function_800529CC(void)
{
  sw(a0 + 0x08, 0);
  sh(a0 + 0x34, -1);
  sb(a0 + 0x41, 0);
  sb(a0 + 0x4B, 0);
}

// size: 0x00000008
void function_80058B60(void)
{
  sw(a0, 0);
}

// size: 0x0000003C
void function_800638EC(void)
{
  sw(a0 + 0x00, ra);
  sw(a0 + 0x04, sp);
  sw(a0 + 0x08, fp);
  sw(a0 + 0x0C, s0);
  sw(a0 + 0x10, s1);
  sw(a0 + 0x14, s2);
  sw(a0 + 0x18, s3);
  sw(a0 + 0x1C, s4);
  sw(a0 + 0x20, s5);
  sw(a0 + 0x24, s6);
  sw(a0 + 0x28, s7);
  sw(a0 + 0x2C, gp);
  v0 = 0;
}