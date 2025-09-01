#include "main.h"
#include "psx_mem.h"
#include "psx_bios.h"
#include "decompilation.h"
#include "spyro_string.h"
#include "spyro_psy.h"
#include "spyro_game.h"

#include <string.h>

// size: 0x0000002C
void function_8006BAE8(void)
{
  if (lhu(a0 + 0xE6)) {
    if (lbu(a0 + 0x46) == 0xFF) {
      v0 = 1;
    } else {
      v0 = 0;
    }
  } else {
    v0 = 1;
  }
}

// size: 0x00000020
void set_timer(uint32_t duration)
{
  sw(timeout1_duration, duration);
  sw(timeout1_start, lhu(TIMER2));
}

void function_8006BB20(void)
{
  BREAKPOINT;
  set_timer(a0);
}

// size: 0x000000A0
uint32_t get_timer(void)
{
  a0 = lhu(TIMER2);
  if (a0 < lw(timeout1_start)) {
    if (lhu(TIMER2_TARGET)) {
      a0 += lhu(TIMER2_TARGET);
    } else {
      a0 += 0x10000;
    }
  }
  v0 = a0 - lw(timeout1_start);
  if ((lhu(TIMER2_MODE) & 0x200) == 0) {
    v0 /= 8;
  }
  return v0 >= lw(timeout1_duration);
}


void function_8006BB40(void)
{
  BREAKPOINT;
  v0 = get_timer();
}

// size: 0x000000B8
void function_8002D170(void)
{
  sw(0x800757D8, 13);
  spyro_memset32(0x80078D78, 0, 0x5C);
  sw(0x80076228, 0x3FFF);
  sw(0x80076224, 0x5000);
  sh(0x80075F18, 0x5000);
  sh(0x80075F1A, 0x5000);
  sw(0x80075754, 10);
  sw(0x80075748, 10);
  sw(0x80078DC4, lw(0x800785E8) - 0x2000);
  a0 = 0x80075F08;
  sw(a0, 0xC0);
  function_8005CC58();
  sw(0x8007591C, 0);
  sw(lw(0x80075680), 0);
  spyro_srand(345);
}

// flush the write queue (?)
void do_literally_nothing(void)
{
  sw(sp + 4, 13);
  sw(sp + 0, 0);
  while (lw(sp + 0) < 60) {
    sw(sp + 4, lw(sp + 4)*13);
    sw(sp + 0, lw(sp + 0) + 1);
  }
}

void function_8005C720(void)
{
  BREAKPOINT;
  do_literally_nothing();
}

// size: 0x00000154
void function_8005E224(void)
{
  uint32_t s1 = a0;
  uint32_t s2 = a1;

  if (a1)
    printf("adding: function_%.8X type: %d\n", a1, a0);
  else
    printf("adding: function_%.8X type: %d\n", lw(0x80073928 + a0*4), a0);

  a0 = 0x80073928 + s1*4;
  uint32_t s4 = lw(a0);
  if (s2 == s4 || lhu(0x80073924) == 0) {
    v0 = s4;
    return;
  }
  v0 = lw(I_MASK_ptr);
  uint32_t s3 = lhu(v0);
  sh(v0, 0);
  if (s2) {
    sw(a0, s2);
    s3 = s3 | (1 << s1);
    sh(0x80073954, lhu(0x80073954) | (1 << s1));
  } else {
    sw(a0, 0);
    s3 = s3 & ~(1 << s1);
    sh(0x80073954, lhu(0x80073954) & ~(1 << s1));
  }
  if (s1 == 0) {
    ChangeClearPAD(s2 == 0);
    ChangeClearRCnt(3, s2 == 0);
  }
  if (s1 == 4) ChangeClearRCnt(0, s2 == 0);
  if (s1 == 5) ChangeClearRCnt(1, s2 == 0);
  if (s1 == 6) ChangeClearRCnt(2, s2 == 0);
  sh(lw(I_MASK_ptr), s3);
  v0 = s4;
}

// size: 0x000000A8
uint32_t dma_callback(uint32_t dma_num, uint32_t callback)
{
  uint32_t DMA_interrupt_register = lw(DMA_interrupt_register_ptr);
  v1 = 0x800749EC + dma_num*4;
  a3 = lw(v1);
  if (callback != a3) {
    if (callback) {
      sw(v1, callback);
      sw(DMA_interrupt_register, (lw(DMA_interrupt_register) & 0x00FFFFFF) | 0x00800000 | (1 << (dma_num + 16)));
    } else {
      sw(v1, 0);
      sw(DMA_interrupt_register, ((lw(DMA_interrupt_register) & 0x00FFFFFF) | 0x00800000) & ~(1 << (dma_num + 16)));
    }
  }
  return a3;
}

// size: 0x000000A8
void function_8005E804(void)
{
  BREAKPOINT;
  v0 = dma_callback(a0, a1);
}

// size: 0x00000030
uint32_t dma_callback2(uint32_t dma_num, uint32_t callback)
{
  if (lw(v0 + 0x04) != 0x8005E804) BREAKPOINT;
  return dma_callback(a0, a1);
}

// size: 0x00000030
void function_8005DE28(void)
{
  BREAKPOINT;
  v0 = dma_callback2(a0, a1);
}

// size: 0x00000068
void spyro_patch_bios(void)
{
  /*
  EnterCriticalSection();
    v0 = lw(GetC0Table() + 0x18);
    t2 = 0x800626B0;
    t1 = 0x800626E8;
    for (int i = 0; i < 0x800626E8 - 0x800626B0; i += 4)
    {
      v1 = lw(0x800626B0 + i);
      sw(v0 + i, v1);
    }
    FlushCache();
  ExitCriticalSection();
  */
}

// size: 0x00000068
void function_80062648(void)
{
  BREAKPOINT;
  spyro_patch_bios();
}

// size: 0x000000F0
uint32_t func_80067628(uint32_t async, uint32_t a1, uint32_t a2)
{
  if (lw(0x80075B50) == 0 && lw(0x80075B58) == 0) return -1;
  
  if (!async) while (lw(0x80075B58) == 0) inter(0);
  
  if (lw(0x80075B58) || !async) {
    if (a2) sw(a2, lw(0x80075B98));
    if (a1) sw(a1, lw(0x80075B94));
    sw(0x80075B58, 0);
    return 1;
  } else {
    if (a2) sw(a2, lw(0x80075B54));
    if (a1) sw(a1, lw(0x80075B50));
    return 0;
  }
}

// size: 0x000000F0
void function_80067628(void)
{
  v0 = func_80067628(a0, a1, a2);
}



// size: 0x000000AC
void function_8005B8E0(void)
{
  memset(addr_to_pointer(0x80075640), 0, 0x8007AA38-0x80075640);

  sp = (lw(0x800755A8)-8) | 0x80000000;
  a0 = 0x8007AA38 & 0x1FFFFFFF;
  v1 = lw(0x800755A4);
  a1 = v0 - v1 - a0;
  sw(0x800730C4, a1);
  a0 = a0 | 0x80000000;
  sw(0x800730C0, a0);
  gp = 0x80075264;
  fp = sp;
  a0 += 4;
  InitHeap(a0, a1);
  game_loop();
}