#include <string.h>

#include "debug.h"
#include "main.h"
#include "psx_mem.h"
#include "spyro_string.h"
#include "psx_bios.h"
#include "spyro_vsync.h"
#include "debug.h"
#include "decompilation.h"
#include "spyro_print.h"
#include "spyro_graphics.h"
#include "decompilation.h"

// size: 0x00000184
uint32_t ResetGraph(uint32_t mode)
{
  sp -= 0x20;
  sw(sp + 0x10, s0);
  sw(sp + 0x18, ra);
  
  printf("resetgraph mode 0x%.2X\n", mode);

  switch (mode & 7) {
  case 0: // reset
  case 3:
    spyro_printf(0x800117A8, 0x80074A1C, 0x80074A64, 0); // "ResetGraph:jtb=%08x,env=%08x\n"
  case 5:
    s0 = 0x80074A64;
    spyro_memset8(s0, 0, 0x80);
    function_8005DDC8();
    GPU_cw(lw(0x80074A5C) & 0x00FFFFFF);
    a0 = mode;
    function_80061DEC();
    sb(0x80074A65, 1);
    sb(s0, v0);
    v0 = v0 & 0xFF;
    sh(0x80074A68, lw(0x80074AE4 + v0*4));
    sh(0x80074A6A, lw(0x80074AF8 + v0*4));
    spyro_memset8(s0 + 0x10, -1, 0x5C);
    spyro_memset8(s0 + 0x6C, -1, 0x14);
    v0 = lbu(s0);
    break;
  default: // flush
    if (lbu(0x80074A66) >= 2) {
      if (lw(0x80074A60) != 0x8006279C) BREAKPOINT;
      spyro_printf(0x800117C8, mode, 0, 0); // "ResetGraph(%d)...\n"
    }
    a0 = 1;
    v0 = lw(lw(0x80074A5C) + 0x34);
    if (v0 != 0x80061DEC) BREAKPOINT;
    function_80061DEC();
    break;
  }
  

  ra = lw(sp + 0x18);
  s0 = lw(sp + 0x10);
  sp += 0x20;
  return v0;
}

void function_8005F2A4(void)
{
  BREAKPOINT;
  v0 = ResetGraph(a0);
}

// size: 0x0000009C
void SetDispMask(uint32_t mask)
{
  sp -= 0x20;
  sw(sp + 0x14, s1);
  sw(sp + 0x18, ra);
  sw(sp + 0x10, s0);

  s1 = 0x80074A66;
  if (lbu(s1) >= 2) {
    if (lw(0x80074A60) != 0x8006279C) BREAKPOINT;
    spyro_printf(0x80011850, mask, 0, 0); // SetDispMask(%d)
  }
  if (mask == 0)
    memset(addr_to_pointer(s1 + 0x6A), -1, 0x14);
  
  if (mask)
    a0 = 0x03000000;
  else
    a0 = 0x03000001;

  if (lw(lw(0x80074A5C) + 0x10) != 0x800616F4) BREAKPOINT;
  function_800616F4();

  ra = lw(sp + 0x18);
  s1 = lw(sp + 0x14);
  s0 = lw(sp + 0x10);
  sp += 0x20;
  return;
}

void function_8005F6C8(void)
{
  BREAKPOINT;
  SetDispMask(a0);
}

// size: 0x0000006C
uint32_t DrawSync(uint32_t mode)
{
  if (lbu(0x80074A66) >= 2) {
    v0 = lw(0x80074A60);
    if (v0 != 0x8006279C) BREAKPOINT;
    spyro_printf(0x80011864, mode, 0, 0); // "DrawSync(%d)"
  }
  
  if (lw(lw(0x80074A5C) + 0x3C) != 0x80061F48) BREAKPOINT;
  a0 = mode;
  function_80061F48();

  return v0;
}

void function_8005F764(void)
{
  BREAKPOINT;
  v0 = DrawSync(a0);
}

// size: 0x00000064
uint32_t SetGraphDebug(uint32_t level)
{
  // level 0: nothing
  // level 1: warnings/errors
  // level 2: function calls
  //printf("SetGraphDebug level set to %d i am setting it to level 2 instead\n", level);
  //level = 2;

  uint32_t previous_debug_level = lbu(psy_debug_level_ptr);
  sb(psy_debug_level_ptr, level);

  if (level & 0xFF) {
    if (lw(0x80074A60) != 0x8006279C) BREAKPOINT;
    spyro_printf(0x800117F4, level, lbu(0x80074A64), lbu(0x80074A67)); // "SetGraphDebug:level:%d,type:%d reverse:%d\n"
  }

  return previous_debug_level;
}

void function_8005F53C(void)
{
  BREAKPOINT;
  v0 = SetGraphDebug(a0);
}

// size: 0x00000128
void psyq_check_box(uint32_t str, uint32_t box_ptr)
{
  t0 = str;
  v1 = lbu(0x80074A66);
  if (v1 == 1) goto label8005F808;
  if (v1 == 2) goto label8005F8A0;
  goto label8005F8E4;
label8005F808: {}
  BREAKPOINT;
  int32_t val1 = (int32_t)lh(0x80074A68);
  int32_t val2 = (int32_t)lh(0x80074A6A);
  a3 = lh(box_ptr + 0);
  v1 = lh(box_ptr + 2);
  a1 = lh(box_ptr + 4);
  a2 = lh(box_ptr + 6);
  if (val1 < (int32_t)a1) goto label8005F890;
  if (val1 < (int32_t)(a1 + a3)) goto label8005F890;
  if (val2 < (int32_t)v1) goto label8005F890;
  if (val2 < (int32_t)(v1 + a2)) goto label8005F890;
  if ((int32_t)a3 < 0) goto label8005F890;
  if ((int32_t)v1 < 0) goto label8005F890;
  if ((int32_t)a1 <= 0) goto label8005F890;
  if ((int32_t)a2 <= 0) goto label8005F890;
  goto label8005F8E4;
label8005F890:
  a0 = 0x80011878; // "%s:bad RECT"
  goto label8005F8A8;
label8005F8A0:
  a0 = 0x80011898; // "%s:"
label8005F8A8:
  if (lw(0x80074A60) != 0x8006279C) BREAKPOINT;
  spyro_printf(a0, str, a2, a3);
  if (lw(0x80074A60) != 0x8006279C) BREAKPOINT;
  sw(sp + 0x10, lh(box_ptr + 6));
  spyro_printf(0x80011884, lh(box_ptr + 0), lh(box_ptr + 2), lh(box_ptr + 4)); // "(%d,%d)-(%d,%d)\n"
label8005F8E4:
  return;
}

void function_8005F7D0(void)
{
  BREAKPOINT;
  psyq_check_box(a0, a1);
}

// size: 0x00000094
uint32_t ClearImage(uint32_t box_ptr, uint32_t r, uint32_t g, uint32_t b)
{
  psyq_check_box(0x8001189C, box_ptr); // "ClearImage"
  
  a0 = lw(lw(0x80074A5C) + 0x0C);
  a1 = box_ptr;
  a2 = 8;
  a3 = ((b & 0xFF) << 16)
     | ((g & 0xFF) <<  8)
     | ((r & 0xFF) <<  0);
  
  if (lw(lw(0x80074A5C) + 0x08) != 0x80061820) BREAKPOINT;
  function_80061820();

  return v0;
}

void function_8005F8F8(void)
{
  BREAKPOINT;
  v0 = ClearImage(a0, a1, a2, a3);
}

// size: 0x00000064
uint32_t LoadImage(uint32_t box_ptr, uint32_t img_ptr)
{
  psyq_check_box(0x800118A8, box_ptr); // "LoadImage"

  a1 = box_ptr;
  v0 = lw(0x80074A5C);
  a2 = 8;
  a0 = lw(lw(0x80074A5C) + 0x20);
  a3 = img_ptr;
  if (lw(lw(0x80074A5C) + 0x08) != 0x80061820) BREAKPOINT;
  function_80061820();

  return v0;
}

void function_8005FA28(void)
{
  BREAKPOINT;
  v0 = LoadImage(a0, a1);
}

// size: 0x00000064
uint32_t StoreImage(uint32_t box_ptr, uint32_t img_ptr)
{
  psyq_check_box(0x800118B4, box_ptr); // "StoreImage"

  a1 = box_ptr;
  a2 = 8;
  a0 = lw(lw(0x80074A5C) + 0x1C);
  a3 = img_ptr;
  if (lw(lw(0x80074A5C) + 0x08) != 0x80061820) BREAKPOINT;
  function_80061820();

  return v0;
}

void function_8005FA8C(void)
{
  BREAKPOINT;
  v0 = StoreImage(a0, a1);
}

// size: 0x000000C4
uint32_t MoveImage(uint32_t box_ptr, uint32_t x, uint32_t y)
{
  psyq_check_box(0x800118C0, box_ptr); // MoveImage

  if (lh(box_ptr + 0x04) == 0 || lh(box_ptr + 0x06) == 0)
    return -1;

  sw(0x80074B18, (y << 16) | (x & 0xFFFF));
  sw(0x80074B14, lw(box_ptr + 0x00));
  sw(0x80074B1C, lw(box_ptr + 0x04));
  if (lw(lw(0x80074A5C) + 0x08) != 0x80061820) BREAKPOINT;
  a0 = lw(lw(0x80074A5C) + 0x18);
  a1 = 0x80074B0C;
  a2 = 0x14;
  a3 = 0;
  function_80061820();

  return v0;
}

void function_8005FAF0(void)
{
  BREAKPOINT;
  v0 = MoveImage(a0, a1, a2);
}

// size: 0x00000074
void DrawOTag(uint32_t of)
{
  if (lbu(0x80074A66) >= 2) {
    uint32_t func = lw(0x80074A60); // 0x8006279C
    if (func != 0x8006279C) BREAKPOINT;
    spyro_printf(0x800118FC, of, 0, 0); // "DrawOTag(%08x)...\n"
  }
  
  a0 = lw(lw(0x80074A5C) + 0x18);
  a1 = of;
  a2 = 0;
  a3 = 0;
  if (lw(lw(0x80074A5C) + 0x08) != 0x80061820) BREAKPOINT;
  function_80061820();
}

void function_8005FD64(void)
{
  BREAKPOINT;
  DrawOTag(a0);
}

// size: 0x00000104
uint32_t PutDrawEnv(uint32_t env_ptr)
{

  sp -= 0x20;
  sw(sp + 0x18, s2);
  sw(sp + 0x1C, ra);
  sw(sp + 0x10, s0);
  s2 = 0x80074A66;
  if (lbu(s2) >= 2) {
    uint32_t func = lw(0x80074A60);
    if (func != 0x8006279C) BREAKPOINT;
    spyro_printf(0x80011910, env_ptr, 0, 0); // "PutDrawEnv(%08x)...\n"
  }
  s0 = env_ptr + 0x1C;
  spyro_clear_screen(s0, env_ptr);
  a0 = 0x00FFFFFF;
  a1 = s0;
  a2 = 0x40;
  v0 = lw(env_ptr + 0x1C);
  v1 = lw(0x80074A5C);
  v0 = v0 | a0;
  sw(env_ptr + 0x1C, v0);
  a0 = lw(v1 + 0x18);
  v0 = lw(v1 + 0x08);
  a3 = 0;
  if (v0 != 0x80061820) BREAKPOINT;
  function_80061820();
  a3 = s2 + 0x0E;
  a2 = env_ptr;
  for (int i = 0; i < 23; i++) {
    sw(a3, lw(a2));
    a2 += 4;
    a3 += 4;
  }
  v0 = env_ptr;
  ra = lw(sp + 0x1C);
  s2 = lw(sp + 0x18);
  s0 = lw(sp + 0x10);
  sp += 0x20;
  return v0;
}

void function_8005FDD8(void)
{
  BREAKPOINT;
  v0 = PutDrawEnv(a0);
}

// size: 0x000000B0
void function_80060E28(void)
{
  v1 = lbu(0x80074A64);
  if (v1 == 1) {
    if (lbu(0x80074A67))
      v0 = 1024 - lh(a0 + 4) - lh(a0 + 0);
    else
      v0 = lh(a0 + 0);
  } else if (v1 != 2) {
    v0 = lh(a0 + 0);
  } else if (lbu(0x80074A67)) {
    v1 = lh(a0 + 4);
    if ((int32_t)v1 < 0) v1++;
    v1 = (int32_t)v1 >> 1;
    v0 = 1024 - v1 - lh(a0 + 0);
  } else {
    v1 = lh(a0 + 0);
    if ((int32_t)v1 < 0) v1++;
    v0 = (int32_t)v1 >> 1;
  }
  return;
}

// size: 0x000004A8
uint32_t PutDispEnv(uint32_t env_ptr)
{
  sp -= 40; // 0xFFFFFFD8
  sw(sp + 0x20, ra);
  sw(sp + 0x1C, s3);
  sw(sp + 0x18, s2);
  sw(sp + 0x14, s1);
  sw(sp + 0x10, s0);
  s0 = env_ptr;
  s3 = 0x08000000;
  v0 = lbu(0x80074A66);
  if (v0 > 2) {
    v0 = lw(0x80074A60);
    if (v0 != 0x8006279C) BREAKPOINT;
    spyro_printf(0x80011944, s0, a2, a3); // "PutDispEnv(%08x)...\n"
  }
  v0 = lbu(0x80074A64);
  if (v0 == 1 || v0 == 2) {
    a0 = s0;
    function_80060E28();
    v1 = lhu(s0 + 2);
    v0 = v0 & 0xFFF;
    v1 = v1 & 0xFFF;
    v1 = (v1 << 12) | v0;
    v0 = 0x05000000;
  } else {
    v1 = lhu(s0 + 0);
    v1 = v1 & 0x3FF;
    v0 = lhu(s0 + 2);
    v0 = v0 & 0x3FF;
    v0 = (v0 << 10) | v1;
    v1 = 0x05000000;
  }
  a0 = v0 | v1;
  v0 = lw(lw(0x80074A5C) + 0x10);
  if (v0 != 0x800616F4) BREAKPOINT;
  function_800616F4();
  v0 = lh(0x80074AD0 + 0x08);
  v1 = lh(s0 + 0x08);
  if (v0 != v1) goto label8006016C;
  v0 = lh(0x80074AD0 + 0x0A);
  v1 = lh(s0 + 0x0A);
  if (v0 != v1) goto label8006016C;
  v0 = lh(0x80074AD0 + 0x0C);
  v1 = lh(s0 + 0x0C);
  if (v0 != v1) goto label8006016C;
  v0 = lh(0x80074AD0 + 0x0E);
  v1 = lh(s0 + 0x0E);
  if (v0 == v1) goto label80060334;
label8006016C:
  function_8005EBA0();
  a0 = lh(s0 + 0x08);
  sb(s0 + 0x12, v0);
  v0 = v0 & 0xFF;
  v1 = a0 << 2;
  v1 += a0;
  v1 = v1 << 1;
  a0 = lh(s0 + 0x0A);
  if (v0 == 0) {
    v1 += 608; // 0x0260
    goto label800601A0;
  }
  v1 += 608; // 0x0260
  s1 = a0 + 19; // 0x0013
  goto label800601A4;
label800601A0:
  s1 = a0 + 16; // 0x0010
label800601A4:
  a1 = lh(s0 + 0x000C);
  if (a1 == 0) {
    v0 = a1 << 2;
    goto label800601C4;
  }
  v0 = a1 << 2;
  v0 += a1;
  v0 = v0 << 1;
  a2 = v1 + v0;
  goto label800601C8;
label800601C4:
  a2 = v1 + 2560; // 0x0A00
label800601C8:
  v0 = lh(s0 + 0x000E);
  if (v0 != 0) {
    s2 = s1 + v0;
    goto label800601DC;
  }
  s2 = s1 + v0;
  s2 = s1 + 240; // 0x00F0
label800601DC:
  v0 = (int32_t)v1 < 500;
  if (v0 != 0) {
    v0 = (int32_t)v1 < 3291;
    goto label800601F8;
  }
  v0 = (int32_t)v1 < 3291;
  if (v0 == 0) {
    a1 = 3290; // 0x0CDA
    goto label800601FC;
  }
  a1 = 3290; // 0x0CDA
  a1 = v1;
  goto label800601FC;
label800601F8:
  a1 = 500; // 0x01F4
label800601FC:
  v1 = a1;
  a1 = v1 + 80; // 0x0050
  v0 = (int32_t)a2 < (int32_t)a1;
  if (v0 != 0) {
    v0 = (int32_t)s1 < 16;
    goto label80060224;
  }
  v0 = (int32_t)s1 < 16;
  v0 = (int32_t)a2 < 3291;
  if (v0 == 0) {
    a1 = 3290; // 0x0CDA
    goto label80060220;
  }
  a1 = 3290; // 0x0CDA
  a1 = a2;
label80060220:
  v0 = (int32_t)s1 < 16;
label80060224:
  if (v0 != 0) {
    a2 = a1;
    goto label80060278;
  }
  a2 = a1;
  v0 = lbu(s0 + 0x0012);
  if (v0 == 0) {
    v0 = (int32_t)s1 < 311;
    goto label8006024C;
  }
  v0 = (int32_t)s1 < 311;
  if (v0 == 0) goto label80060258;
  a0 = s1;
  goto label8006027C;
label8006024C:
  v0 = (int32_t)s1 < 257;
  if (v0 != 0) goto label80060270;
label80060258:
  v0 = lbu(s0 + 0x0012);
  if (v0 == 0) {
    a0 = 256; // 0x0100
    goto label8006027C;
  }
  a0 = 256; // 0x0100
  a0 = 310; // 0x0136
  goto label8006027C;
label80060270:
  a0 = s1;
  goto label8006027C;
label80060278:
  a0 = 16; // 0x0010
label8006027C:
  s1 = a0;
  a1 = s1 + 2; // 0x0002
  v0 = (int32_t)s2 < (int32_t)a1;
  if (v0 != 0) goto label800602D8;
  v0 = lbu(s0 + 0x0012);
  if (v0 == 0) {
    v0 = (int32_t)s2 < 313;
    goto label800602B0;
  }
  v0 = (int32_t)s2 < 313;
  if (v0 == 0) goto label800602BC;
  a1 = s2;
  goto label800602D8;
label800602B0:
  if ((int32_t)s2 < 259) goto label800602D4;
label800602BC:
  if (lbu(s0 + 0x12))
    a1 = 0x138;
  else
    a1 = 0x102;
  goto label800602D8;
label800602D4:
  a1 = s2;
label800602D8:
  s2 = a1;
  v0 = a2 & 0xFFF;
  v0 = v0 << 12;
  a0 = v1 & 0xFFF;
  v1 = 0x06000000;
  a1 = lw(0x80074A5C);
  a0 = a0 | v1;
  a0 = v0 | a0;
  v0 = lw(lw(0x80074A5C) + 0x10);
  if (v0 != 0x800616F4) BREAKPOINT;
  function_800616F4();

  v0 = s2 & 0x3FF;
  v0 = v0 << 10;
  a0 = s1 & 0x3FF;
  v1 = 0x07000000;
  a1 = lw(0x80074A5C);
  a0 = a0 | v1;
  a0 = v0 | a0;
  v0 = lw(lw(0x80074A5C) + 0x10);
  if (v0 != 0x800616F4) BREAKPOINT;
  function_800616F4();
label80060334:
  v1 = lw(0x80074AE0);
  v0 = lw(s0 + 0x10);
  if (v1 != v0) goto label800603BC;
  v0 = lh(0x80074AD0);
  v1 = lh(s0 + 0x00);
  if (v0 != v1) goto label800603BC;
  v0 = lh(0x80074AD2);
  v1 = lh(s0 + 0x02);
  if (v0 != v1) goto label800603BC;
  v0 = lh(0x80074AD4);
  v1 = lh(s0 + 0x04);
  if (v0 != v1) goto label800603BC;
  v0 = lh(0x80074AD6);
  v1 = lh(s0 + 0x06);
  if (v0 == v1) goto label800604A0;
label800603BC:
  function_8005EBA0();
  sb(s0 + 0x0012, v0);
  v0 = v0 & 0xFF;
  if (v0 == 1)
    s3 = s3 | 0x08;
  v0 = lbu(s0 + 0x11);
  if (v0) s3 = s3 | 0x10;
  v0 = lbu(s0 + 0x10);
  if (v0) s3 = s3 | 0x20;
  v0 = lbu(0x80074A67);
  if (v0) s3 = s3 | 0x80;
  v1 = lh(s0 + 0x04);
  v0 = (int32_t)v1 < 281;
  if (v0 != 0) goto label80060464;
  v0 = (int32_t)v1 < 353;
  if (v0) {
    s3 = s3 | 0x01;
    goto label80060464;
  }
  v0 = (int32_t)v1 < 401;
  if (v0 == 0) goto label80060450;
  s3 = s3 | 0x40;
  goto label80060464;
label80060450:
  v0 = (int32_t)v1 < 561;
  if (v0 == 0) goto label80060460;
  s3 = s3 | 0x2;
  goto label80060464;
label80060460:
  s3 = s3 | 0x3;
label80060464:
  v0 = lbu(s0 + 0x12);
  v1 = lh(s0 + 0x06);
  if (v0) {
    v0 = (int32_t)v1 < 289;
  } else {
    v0 = (int32_t)v1 < 257;
  }
  if (v0 == 0) s3 = s3 | 0x24;
  v0 = lw(lw(0x80074A5C) + 0x10);
  a0 = s3;
  if (v0 != 0x800616F4) BREAKPOINT;
  function_800616F4();
label800604A0:
  spyro_memcpy8(0x80074AD0, s0, 0x14);
  v0 = s0;
  ra = lw(sp + 0x20);
  s3 = lw(sp + 0x1C);
  s2 = lw(sp + 0x18);
  s1 = lw(sp + 0x14);
  s0 = lw(sp + 0x10);
  sp += 0x28;
  return v0;
}

void function_80060030(void)
{
  BREAKPOINT;
  v0 = PutDispEnv(a0);
}