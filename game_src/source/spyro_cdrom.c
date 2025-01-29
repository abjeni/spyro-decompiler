#include <stdint.h>
#include "psx_mem.h"
#include "main.h"
#include "spyro_vsync.h"

// size: 0x00000020
void function_80063BD8(void)
{
  v0 = 2;
  return;
}

// size: 0x00000138
void function_80063C48(void)
{
  return;
}

// size: 0x0000012C
void function_80063D80(void)
{
  return;
}

// size: 0x00000144
void function_80063EAC(void)
{
  return;
}

// size: 0x00000020
void function_80063FF0(void)
{
  v0 = 1;
  return;
}

// size: 0x000001EC
void function_800653B4(void)
{
  return;
}

void read_disk(uint32_t sector, uint32_t dst, uint32_t len, uint32_t offset, uint32_t num)
{
  if (len & 0x7FF || offset & 0x7FF || num != 0x258) {
    printf("len: %X offset: %X num: %X\n", len, offset, num);
    BREAKPOINT;
  }

  sector += offset >> 11;
  uint32_t sector_len = len >> 11;
  psx_read_sectors(dst, sector, sector_len);
}

void read_disk1(uint32_t sector, uint32_t dst, uint32_t len, uint32_t offset, uint32_t num)
{
  read_disk(sector, dst, len, offset, num);
}

void read_disk2(uint32_t sector, uint32_t dst, uint32_t len, uint32_t offset, uint32_t num)
{
  read_disk(sector, dst, len, offset, num);
}

void function_80016500()
{
  BREAKPOINT;
  read_disk1(a0, a1, a2, a3, lw(sp+0x10));
}

void function_80016698()
{
  BREAKPOINT;
  read_disk2(a0, a1, a2, a3, lw(sp+0x10));
}