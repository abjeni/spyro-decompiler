#include <stdint.h>
#include "psx_mem.h"
#include "main.h"
#include "spyro_vsync.h"
#include "spyro_system.h"
#include "decompilation.h"
#include "not_renamed.h"

uint8_t to_hexnum(uint8_t num)
{
  return ((num/10) << 4) + (num % 10);
}

uint8_t from_hexnum(uint8_t num)
{
  return ((num >> 4)*10) + (num % 10);
}

// size: 0x00000104
char *write_cdrom_header(uint32_t sector_num, char header[])
{
  sector_num += 150;

  const uint32_t sector = sector_num % 75;
  const uint32_t second = (sector_num / 75) % 60;
  const uint32_t minute = (sector_num / 75) / 60;

  header[0] = to_hexnum(minute);
  header[1] = to_hexnum(second);
  header[2] = to_hexnum(sector);

  return header;
}

// size: 0x00000104
void function_80064094(void)
{
  BREAKPOINT;
  v0 = pointer_to_addr(write_cdrom_header(a0, addr_to_pointer(a1)));
}

// size: 0x00000080
uint32_t parse_cdrom_header(char header[])
{
  const uint32_t sector = from_hexnum(header[0]);
  const uint32_t second = from_hexnum(header[1]);
  const uint32_t minute = from_hexnum(header[2]);

  const uint32_t sector_num = sector + second*75 + minute*60*75;

  return sector_num;
}

// size: 0x00000080
void function_80064198(void)
{
  BREAKPOINT;
  v0 = parse_cdrom_header(addr_to_pointer(a0));
}

// size: 0x00000024
uint32_t dma_cdrom_callback(uint32_t callback)
{
  return dma_callback(3, callback);
}

// size: 0x00000024
void function_80064050(void)
{
  BREAKPOINT;
  v0 = dma_cdrom_callback(a0);
}

// size: 0x00000030
void function_8005DDC8(void)
{
  if (lw(lw(0x800749AC) + 0x0C) != 0x8005DF60) BREAKPOINT;
  function_8005DF60();
}

// size: 0x00000050
void function_80065364(void)
{
  sw(0x80074E38, 0);
  sw(0x80074E34, 0);
  sw(0x80074E48, 0);
  sw(0x80074E44, 0);
  ra = 0x80065394;
  function_8005DDC8();
  a1 = 0x8006590C;
  a0 = 2; // 0x0002
  ra = 0x800653A4;
  function_8005DDF8();
}

// size: 0x000000F4
void function_80065270(void)
{
  v1 = lw(SPU_voice_base_ptr);
  if (lhu(v1 + 0x01B8) == 0 && lhu(v1 + 0x01BA) == 0) {
    sh(v1 + 0x0180, 0x3FFF);
    sh(v1 + 0x0182, 0x3FFF);
  }
  sh(v1 + 0x01B0, 0x3FFF);
  sh(v1 + 0x01B2, 0x3FFF);
  sh(v1 + 0x01AA, 0xC001);
  
  sb(lw(CDROM_INDEX_ptr), 2);
  sb(lw(CDROM_REG_2_ptr), 0x80);
  sb(lw(CDROM_REG_3_ptr), 0);

  sb(lw(CDROM_INDEX_ptr), 3);
  sb(lw(CDROM_REG_1_ptr), 0x80);
  sb(lw(CDROM_REG_2_ptr), 0);
  sb(lw(CDROM_REG_3_ptr), 0x20);
  v0 = 0;
}

// size: 0x000001EC
void function_800653B4(void)
{
  v0 = 0;
  return;
}

// size: 0x0000006C
void function_80063ACC(void)
{
  uint32_t s0 = a0;
  if (s0 == 2) {
    ra = 0x80063AEC;
    function_80065364();
    v0 = 1;
    return;
  }
  ra = 0x80063AFC;
  function_800653B4();
  if (v0) {
    v0 = 0;
    return;
  }
  if (s0 == 1) {
    ra = 0x80063B18;
    function_80065270();
    if (v0) {
      v0 = 0;
      return;
    }
  }
  v0 = 1;
}

// size: 0x00000098
uint32_t CdInit(void)
{
  uint32_t s0 = 4;
  while (1) {
    a0 = 1;
    ra = 0x80063994;
    function_80063ACC();
    s0--;
    if (v0 == 1) {
      a0 = 0x80063A14;
      ra = 0x800639B0;
      function_80063C18();
      a0 = 0x80063A3C;
      ra = 0x800639C0;
      function_80063C30();
      a0 = 0x80063A64;
      ra = 0x800639D0;
      function_8006623C();
      a0 = 0;
      ra = 0x800639D8;
      function_80066254();
      return 1;
    }
    if (s0 == -1) {
      printf("CdInit: Init failed\n");
      return 0;
    }
  }
}

// size: 0x00000098
void function_8006397C(void)
{
  BREAKPOINT
  v0 = CdInit();
}

// size: 0x0000008C
void init_cdrom(void)
{
  sp -= 0x20;
  sb(sp + 0x10, 0x80);

  CdInit();

  a0 = 14;
  a1 = sp + 0x10;
  a2 = 0;
  ra = 0x800124A4;
  function_80063C48();

  a0 = 0x80016490;
  ra = 0x800124B4;
  function_8006623C();
  
  sw(0x800774B4, 0x40);

  sw(0x800776C4, 0);
  sw(0x800776C8, 0);
  sb(0x800776D2, 0x7F);
  sb(0x800776D0, 0x7F);
  sb(0x800776D3, 0);
  sb(0x800776D1, 0);

  a0 = 0x800776D0;
  ra = 0x800124FC;
  function_80063FF0();

  sp += 0x20;
  return;
}

// size: 0x0000008C
void function_80012480(void)
{
  BREAKPOINT;
  init_cdrom();
}

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

void function_80016500(void)
{
  BREAKPOINT;
  read_disk1(a0, a1, a2, a3, lw(sp+0x10));
}

void function_80016698(void)
{
  BREAKPOINT;
  read_disk2(a0, a1, a2, a3, lw(sp+0x10));
}