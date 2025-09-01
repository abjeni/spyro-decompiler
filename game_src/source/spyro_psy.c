#include "spyro_psy.h"
#include <string.h>

#include "debug.h"
#include "int_math.h"
#include "main.h"
#include "psx_mem.h"
#include "psx_bios.h"
#include "spyro_string.h"
#include "psx_bios.h"
#include "spyro_vsync.h"
#include "debug.h"
#include "decompilation.h"
#include "spyro_print.h"
#include "spyro_graphics.h"
#include "decompilation.h"
#include "not_renamed.h"

// size: 0x00000010
void function_8005EBA0(void)
{
  v0 = lw(0x80074A10);
  return;
}

int32_t random_seed = 0;

// size: 0x00000010
void spyro_srand(int32_t seed)
{
  random_seed = seed;
}

// size: 0x00000010
void function_8006275C(void)
{
  BREAKPOINT;
  spyro_srand(a0);
}

// size: 0x00000030
int spyro_rand(void)
{
  random_seed = random_seed*0x41C64E6D + 12345;
  return (random_seed >> 16) & 0x7FFF;
}

// size: 0x00000030
void function_8006272C(void)
{
  BREAKPOINT;
  v0 = spyro_rand();
}

// size: 0x00000184
int32_t ResetGraph(int32_t mode)
{
  sp -= 0x20;
  sw(sp + 0x10, s0);
  sw(sp + 0x18, ra);
  
  printf("resetgraph mode 0x%.2X\n", mode);

  switch (mode & 7) {
  case 0: // reset
  case 3:
    printf("ResetGraph:jtb=%08x,env=%08x\n", 0x80074A1C, 0x80074A64);
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
    sh(VRAM_SIZE_X, lw(0x80074AE4 + v0*4));
    sh(VRAM_SIZE_Y, lw(0x80074AF8 + v0*4));
    spyro_memset8(s0 + 0x10, -1, 0x5C);
    spyro_memset8(s0 + 0x6C, -1, 0x14);
    v0 = lbu(s0);
    break;
  default: // flush
    if (lbu(psy_debug_level_ptr) >= 2) {
      if (lw(0x80074A60) != 0x8006279C) BREAKPOINT;
      printf("ResetGraph(%d)...\n", mode);
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
void SetDispMask(int32_t mask)
{
  sp -= 0x20;
  sw(sp + 0x14, s1);
  sw(sp + 0x18, ra);
  sw(sp + 0x10, s0);

  s1 = psy_debug_level_ptr;
  if (lbu(psy_debug_level_ptr) >= 2) {
    if (lw(0x80074A60) != 0x8006279C) BREAKPOINT;
    printf("SetDispMask(%d)...\n", mask);
  }
  if (mask == 0)
    memset(addr_to_pointer(psy_debug_level_ptr + 0x6A), -1, 0x14);
  
  if (mask)
    a0 = 0x03000000;
  else
    a0 = 0x03000001;

  if (lw(lw(0x80074A5C) + 0x10) != 0x800616F4) BREAKPOINT;
  GP1_command(a0);

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
  if (lbu(psy_debug_level_ptr) >= 2) {
    v0 = lw(0x80074A60);
    if (v0 != 0x8006279C) BREAKPOINT;
    printf("DrawSync(%d)...\n", mode);
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
int32_t SetGraphDebug(int32_t level)
{
  // level 0: nothing
  // level 1: warnings/errors
  // level 2: function calls
  //printf("SetGraphDebug level set to %d i am setting it to level 2 instead\n", level);
  //level = 2;
  level = 1;

  uint32_t previous_debug_level = lbu(psy_debug_level_ptr);
  sb(psy_debug_level_ptr, level);

  if (level & 0xFF) {
    if (lw(0x80074A60) != 0x8006279C) BREAKPOINT;
    printf("SetGraphDebug:level:%d,type:%d reverse:%d\n", level, lbu(0x80074A64), lbu(0x80074A67));
  }

  return previous_debug_level;
}

void function_8005F53C(void)
{
  BREAKPOINT;
  v0 = SetGraphDebug(a0);
}

// size: 0x00000128
void psyq_check_box(char *str, RECT box)
{
  uint32_t debug_level = lbu(psy_debug_level_ptr);
  if (debug_level == 1) {
    int32_t size_x = (int32_t)lh(VRAM_SIZE_X);
    int32_t size_y = (int32_t)lh(VRAM_SIZE_Y);
    if ((size_x < box.w)
     || (size_x < (box.w + box.x))
     || (size_y < box.y)
     || (size_y < (box.y + box.h))
     || (box.x < 0)
     || (box.y < 0)
     || (box.w <= 0)
     || (box.h <= 0)) {
      if (lw(0x80074A60) != 0x8006279C) BREAKPOINT;
      printf("%s:bad RECT", str);
      printf("(%d,%d)-(%d,%d)\n", box.x, box.y, box.w, box.h);
      return;
     }
    return;
  }
  if (debug_level == 2) {
    if (lw(0x80074A60) != 0x8006279C) BREAKPOINT;
    printf("%s:", str);
    printf("(%d,%d)-(%d,%d)\n", box.x, box.y, box.w, box.h);
    return;
  }
  return;
}

void function_8005F7D0(void)
{
  BREAKPOINT;
  psyq_check_box(addr_to_pointer(a0), *(RECT*)addr_to_pointer(a1));
}

// size: 0x00000094
int32_t ClearImage(RECT *rect, uint8_t r, uint8_t g, uint8_t b)
{
  psyq_check_box("ClearImage", *rect);
  
  if (lw(lw(0x80074A5C) + 0x08) != 0x80061820) BREAKPOINT;
  //v0 = command_queue_append(lw(lw(0x80074A5C) + 0x0C), pointer_to_addr(rect), 8, 
  //   ((b & 0xFF) << 16)
  // | ((g & 0xFF) <<  8)
  // | ((r & 0xFF) <<  0));

  fill_color(rect, 
      ((b & 0xFF) << 16)
    | ((g & 0xFF) <<  8)
    | ((r & 0xFF) <<  0));
  v0 = 0;

  return v0;
}

void function_8005F8F8(void)
{
  BREAKPOINT;
  v0 = ClearImage(addr_to_pointer(a0), a1, a2, a3);
}

// size: 0x00000064
int32_t LoadImage(RECT *recp, void *p)
{
  psyq_check_box("LoadImage", *recp);

  if (lw(lw(0x80074A5C) + 0x08) != 0x80061820) BREAKPOINT;
  //v0 = command_queue_append(lw(lw(0x80074A5C) + 0x20), pointer_to_addr(recp), 8, pointer_to_addr(p));

  ram_to_vram(recp, p);
  v0 = 0;

  return v0;
}

void function_8005FA28(void)
{
  BREAKPOINT;
  v0 = LoadImage(addr_to_pointer(a0), addr_to_pointer(a1));
}

// size: 0x00000064
int32_t StoreImage(RECT *recp, void *p)
{
  psyq_check_box("StoreImage", *recp);

  if (lw(lw(0x80074A5C) + 0x08) != 0x80061820) BREAKPOINT;
  vram_to_ram(recp, p);
  v0 = 0;
  //v0 = command_queue_append(lw(lw(0x80074A5C) + 0x1C), pointer_to_addr(recp), 8, pointer_to_addr(p));

  return v0;
}

void function_8005FA8C(void)
{
  BREAKPOINT;
  v0 = StoreImage(addr_to_pointer(a0), addr_to_pointer(a1));
}

// size: 0x000000C4
int32_t MoveImage(RECT *recp, int32_t x, int32_t y)
{
  psyq_check_box("MoveImage", *recp);

  if (recp->w == 0 || recp->h == 0)
    return -1;

  sw(0x80074B14, (recp->y << 16) | (recp->x & 0xFFFF));
  sw(0x80074B18, (y << 16) | (x & 0xFFFF));
  sw(0x80074B1C, (recp->h << 16) | (recp->w & 0xFFFF));
  if (lw(lw(0x80074A5C) + 0x08) != 0x80061820) BREAKPOINT;
  v0 = command_queue_append(lw(lw(0x80074A5C) + 0x18), 0x80074B0C, 0x14, 0);

  return v0;
}

void function_8005FAF0(void)
{
  BREAKPOINT;
  v0 = MoveImage(addr_to_pointer(a0), a1, a2);
}

// size: 0x00000074
void DrawOTag(uint32_t *ot)
{
  if (lbu(psy_debug_level_ptr) >= 2) {
    if (lw(0x80074A60) != 0x8006279C) BREAKPOINT;
    printf("DrawOTag(%08x)...\n", pointer_to_addr_maybe(ot));
  }

  if (lw(lw(0x80074A5C) + 0x08) != 0x80061820) BREAKPOINT;
  //v0 = command_queue_append(lw(lw(0x80074A5C) + 0x18), pointer_to_addr(ot), 0, 0);

  execute_gpu_linked_list(ot);
}

void function_8005FD64(void)
{
  BREAKPOINT;
  DrawOTag(addr_to_pointer(a0));
}

// size: 0x00000104
DRAWENV *PutDrawEnv(DRAWENV *env)
{
  if (lbu(psy_debug_level_ptr) >= 2) {
    uint32_t func = lw(0x80074A60);
    if (func != 0x8006279C) BREAKPOINT;
    spyro_printf(0x80011910, pointer_to_addr_maybe(env), 0, 0); // "PutDrawEnv(%08x)...\n"
  }

  spyro_clear_screen(&env->dr_env, *env);
  env->dr_env.tag = env->dr_env.tag | 0x00FFFFFF;
  
  if (lw(lw(0x80074A5C) + 0x08) != 0x80061820) BREAKPOINT;
  //v0 = command_queue_append(lw(lw(0x80074A5C) + 0x18), pointer_to_addr(&env->dr_env), 0x40, 0);

  execute_gpu_linked_list(&env->dr_env);

  *(DRAWENV*)addr_to_pointer(0x80074A74) = *env;

  return env;
}

void function_8005FDD8(void)
{
  BREAKPOINT;
  v0 = pointer_to_addr(PutDrawEnv(addr_to_pointer(a0)));
}

// size: 0x000000B0
uint32_t psy_lib_func1(RECT box)
{
  v1 = lbu(0x80074A64);
  if (v1 == 1) {
    if (lbu(0x80074A67))
      v0 = 1024 - box.w - box.x;
    else
      v0 = box.x;
  } else if (v1 != 2) {
    v0 = box.x;
  } else if (lbu(0x80074A67)) {
    v1 = box.w;
    if ((int32_t)v1 < 0) v1++;
    v1 = (int32_t)v1 >> 1;
    v0 = 1024 - v1 - box.x;
  } else {
    v1 = box.x;
    if ((int32_t)v1 < 0) v1++;
    v0 = (int32_t)v1 >> 1;
  }
  return v0;
}

void function_80060E28(void)
{
  v0 = psy_lib_func1(*(RECT*)addr_to_pointer(a0));
}

// size: 0x000004A8
DISPENV *PutDispEnv(DISPENV *env)
{
  sp -= 40; // 0xFFFFFFD8
  sw(sp + 0x1C, s3);
  sw(sp + 0x18, s2);
  sw(sp + 0x14, s1);

  DISPENV *current_env = addr_to_pointer(0x80074AD0);

  s3 = 0x08000000;
  v0 = lbu(psy_debug_level_ptr);
  if (v0 > 2) {
    v0 = lw(0x80074A60);
    if (v0 != 0x8006279C) BREAKPOINT;
    printf("PutDispEnv(%08x)...\n", pointer_to_addr_maybe(env));
  }
  if (psx_has_2mb_vram()) {
    a0 = 0x05000000 | ((env->disp.y & 0xFFF) << 12) | (psy_lib_func1(env->disp) & 0xFFF);
  } else {
    a0 = 0x05000000 | ((env->disp.y & 0x3FF) << 10) | (env->disp.x & 0x3FF);
  }
  v0 = lw(lw(0x80074A5C) + 0x10);
  if (v0 != 0x800616F4) BREAKPOINT;
  GP1_command(a0);

  if (current_env->screen.x != env->screen.x
   || current_env->screen.y != env->screen.y
   || current_env->screen.w != env->screen.w
   || current_env->screen.h != env->screen.h)
  {
    function_8005EBA0();
    env->pad0 = v0;
    v0 = v0 & 0xFF;
    v1 = env->screen.x*10 + 608;
    a0 = env->screen.y;
    if (v0 == 0) goto label800601A0;
    s1 = a0 + 0x13;
    goto label800601A4;
  label800601A0:
    s1 = a0 + 0x10;
  label800601A4:
    a1 = env->screen.w;
    v0 = a1 << 2;
    if (a1 == 0) goto label800601C4;
    v0 += a1;
    v0 = v0 << 1;
    a2 = v1 + v0;
    goto label800601C8;
  label800601C4:
    a2 = v1 + 2560; // 0x0A00
  label800601C8:
    v0 = env->screen.h;
    s2 = s1 + v0;
    if (v0 != 0) goto label800601DC;
    s2 = s1 + 240; // 0x00F0
  label800601DC:
    v1 = clamp_int(v1, 500, 3290);
    a1 = v1 + 80;
    
    if ((int32_t)a2 >= (int32_t)a1)
      a1 = min_int(a2, 3290);

    v0 = (int32_t)s1 < 16;
    if (v0 != 0) {
      a2 = a1;
      goto label80060278;
    }
    a2 = a1;
    v0 = env->pad0;
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
    v0 = env->pad0;
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
    v0 = env->pad0;
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
    if (env->pad0)
      a1 = 0x138;
    else
      a1 = 0x102;
    goto label800602D8;
  label800602D4:
    a1 = s2;
  label800602D8:
    s2 = a1;
    a0 = 0x06000000 | ((a2 & 0xFFF) << 12) | (v1 & 0xFFF);
    a1 = lw(0x80074A5C);
    v0 = lw(lw(0x80074A5C) + 0x10);
    if (v0 != 0x800616F4) BREAKPOINT;
    GP1_command(a0);

    a0 = 0x07000000 | ((s2 & 0x3FF) << 10) | (s1 & 0x3FF);
    a1 = lw(0x80074A5C);
    v0 = lw(lw(0x80074A5C) + 0x10);
    if (v0 != 0x800616F4) BREAKPOINT;
    GP1_command(a0);
  }
  
  if (current_env->isinter != env->isinter
   || current_env->disp.x != env->disp.x
   || current_env->disp.y != env->disp.y
   || current_env->disp.w != env->disp.w
   || current_env->disp.h != env->disp.h) 
  {
    function_8005EBA0();
    env->pad0 = v0;
    v0 = v0 & 0xFF;
    if (v0 == 1)
      s3 = s3 | 0x08;
    v0 = env->isrgb24;
    if (v0) s3 = s3 | 0x10;
    v0 = env->isinter;
    if (v0) s3 = s3 | 0x20;
    v0 = lbu(0x80074A67);
    if (v0) s3 = s3 | 0x80;
    v1 = env->disp.w;
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
    v0 = env->pad0;
    v1 = env->disp.h;
    if (v0) {
      v0 = (int32_t)v1 < 289;
    } else {
      v0 = (int32_t)v1 < 257;
    }
    if (v0 == 0) s3 = s3 | 0x24;
    v0 = lw(lw(0x80074A5C) + 0x10);
    a0 = s3;
    if (v0 != 0x800616F4) BREAKPOINT;
    GP1_command(a0);
  }
  *current_env = *env;
  s3 = lw(sp + 0x1C);
  s2 = lw(sp + 0x18);
  s1 = lw(sp + 0x14);
  sp += 0x28;
  return env;
}

void function_80060030(void)
{
  BREAKPOINT;
  v0 = pointer_to_addr(PutDispEnv(addr_to_pointer(a0)));
}

// size: 0x00000058
void SetDrawMode(DR_MODE *p, int32_t dfe, int32_t dfd, int32_t tpage, RECT *tw)
{
  p->tag = (p->tag & 0x00FFFFFF) | (2 << 24);
  p->code[0] = spyro_draw_mode_setting_command(dfe, dfd, tpage);
  p->code[1] = spyro_set_texture_window_setting_command(tw);
  return;
}

// size: 0x00000058
void function_80060670(void)
{
  BREAKPOINT;
  SetDrawMode(addr_to_pointer(a0), a1, a2, a3, addr_to_pointer(lw(sp + 0x10)));
}