#include "main.h"
#include "psx_mem.h"
#include "psx_bios.h"
#include "decompilation.h"
#include "not_renamed.h"
#include "spyro_string.h"
#include "spyro_psy.h"
#include "spyro_game.h"
#include "psx_ops.h"
#include "spyro_print.h"

#include <string.h>

// size: 0x00000020
void set_timer(uint32_t duration)
{
  sw(timeout1_duration, duration);
  sw(timeout1_start, lhu(TIMER2));
}

void function_8006BB20(void)
{
  UNREACHABLE;
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
  UNREACHABLE;
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
  UNREACHABLE;
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
  v1 = DMA_callbacks + dma_num*4;
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
  UNREACHABLE;
  v0 = dma_callback(a0, a1);
}

// size: 0x00000030
uint32_t dma_callback2(uint32_t dma_num, uint32_t callback)
{
  if (lw(v0 + 0x04) != 0x8005E804) UNREACHABLE;
  return dma_callback(a0, a1);
}

// size: 0x00000030
void function_8005DE28(void)
{
  UNREACHABLE;
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
  UNREACHABLE;
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
void spyro_start(void)
{
  memset(addr_to_pointer(0x80075640), 0, 0x53F8);  // [0x80075640 - 0x8007AA38]

  uint32_t sp_num = (0x200000-8);

  sp = sp_num | 0x80000000;
  fp = sp;
  gp = 0x80075264;
  v1 = lw(0x800755A4);
  sw(heap_base, lib_begin);
  sw(heap_size, sp_num - 0x800 - 0x7AA38); // 0x184DC0
  InitHeap(0x8007AA3C, sp_num - 0x800 - 0x7AA38); // do not use this heap!!!
  game_loop();
}

// size: 0x00000030
void function_8005DDF8(void)
{
  if (lw(lw(0x800749AC) + 0x08) != 0x8005E224) UNREACHABLE;
  function_8005E224();
}

// size: 0x00000058
void function_8005E508(void)
{
  a0 = 0x800749C0; // &0x00000000
  v1 = lw(TIMER_1_horizontal_retrace_counter_mode_ptr);
  v0 = 263; // 0x0107
  sw(lw(TIMER_1_horizontal_retrace_counter_mode_ptr), 0x0107);
  sw(frame_counter, 0);
  a1 = 8; // 0x0008
  spyro_memclr32(a0, a1);
  a1 = 0x8005E560; // &0x3C028007
  a0 = 0;
  function_8005DDF8();
  v0 = 0x8005E5D8; // &0x3C028007
}

// size: 0x00000050
void function_8005E630(void)
{
  spyro_memclr32(DMA_callbacks, 8);
  sw(lw(DMA_interrupt_register_ptr), 0);
  a0 = 3;
  a1 = 0x8005E680; // &0x3C028007
  function_8005DDF8();
  v0 = 0x8005E804; // &0x00803021
}

// size: 0x000000DC
uint32_t init_hook_entry_int(void)
{
  if (lhu(0x80073924)) return 0;
  sh(lw(I_MASK_ptr), 0);
  sh(lw(I_STAT_ptr), 0);
  sw(lw(DMA_control_register_copy_1_ptr), 0x33333333);
  spyro_memclr32(0x80073924, 0x41A);
  a0 = 0x80073924 + 0x38;
  function_800638EC();
  if (v0)
    function_8005E03C();
  
  sw(a0 + 4, 0x8007493C);
  HookEntryInt(0x80073924 + 0x38);
  sh(0x80073924, 1);
  function_8005E508();
  sw(lw(0x800749AC) + 0x14, v0); // &0x8007498C
  function_8005E630();
  sw(lw(0x800749AC) + 0x04, v0); // &0x8007498C
  _96_remove();
  ExitCriticalSection();
  return 0x80073924;
}

void function_8005DF60(void)
{
  UNREACHABLE;
  v0 = init_hook_entry_int();
}

// size: 0x00000030
uint32_t init_hook_entry_int2(void)
{
  if (lw(lw(0x800749AC) + 0x0C) != 0x8005DF60) UNREACHABLE;
  return init_hook_entry_int();
}

// size: 0x00000030
void function_8005DDC8(void)
{
  UNREACHABLE;
  v0 = init_hook_entry_int2();
}

// size: 0x000001E8
void function_8005E03C(void)
{
  sp -= 40; // 0xFFFFFFD8
  sw(sp + 0x0014, s1);
  sw(sp + 0x0024, ra);
  sw(sp + 0x0020, s4);
  sw(sp + 0x001C, s3);
  sw(sp + 0x0018, s2);
  sw(sp + 0x0010, s0);
  if (lhu(0x80073924) == 0) {
    printf("unexpected interrupt(%04x)\n", lhu(lw(I_STAT_ptr)));
    //ReturnFromException();
    goto end;
  }
  sh(0x80073926, 1);
  while ((s0 = lhu(lw(I_MASK_ptr)) & lhu(0x80073954) & lhu(lw(I_STAT_ptr)))) {
    for (int i = 0; i < 12; i++) {
      if (!(s0 & 0xFFF)) break;
      if (s0 & 1) {
        sh(lw(I_STAT_ptr), ~(1 << i)); // acknowledge interrupt
        uint32_t callback = lw(0x80073928 + i*4);
        switch (callback)
        {
        case 0:
          break;
        case 0x8006590C:
          function_8006590C();
          break;
        case 0x8005E560:
          function_8005E560();
          break;
        default:
          JALR(callback, 0x8005E11C);
        }
      }
      s0 = s0 >> 1;
    }
  }
  if (!(lhu(lw(I_MASK_ptr)) & lhu(lw(I_STAT_ptr))))
  {
    sw(0x800749BC, 0);
  } else {
    sw(0x800749BC, lw(0x800749BC) + 1);
    if ((int32_t)v0 > 2048) {
      printf("intr timeout(%04x:%04x)\n", lhu(lw(I_STAT_ptr)), lhu(lw(I_MASK_ptr)));
      sw(0x800749BC, 0);
      sh(lw(I_STAT_ptr), 0);
    }
  }
  sh(0x80073926, 0);
  //ReturnFromException();
end:
  ra = lw(sp + 0x0024);
  s4 = lw(sp + 0x0020);
  s3 = lw(sp + 0x001C);
  s2 = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 40; // 0x0028
  return;
}

void function_80067CD4(void);

// size: 0x00000078
void function_8005E560(void)
{
  uint32_t temp;
  v0 = lw(frame_counter);
  sp -= 32; // 0xFFFFFFE0
  sw(sp + 0x0014, s1);
  s1 = 0;
  sw(sp + 0x0010, s0);
  s0 = 0x800749C0; // &0x00000000
  sw(sp + 0x0018, ra);
  v0++;
  sw(frame_counter, v0);
  v0 = lw(frame_counter);
label8005E598:
  v0 = lw(s0 + 0x0000);
  temp = v0 == 0;
  if (temp) goto label8005E5B0;
  temp = v0;
  switch (temp)
  {
  case 0x80053C68:
    function_80053C68();
    break;
  case 0x80067CD4:
    function_80067CD4();
    break;
  default:
    JALR(temp, 0x8005E5A8);
  }
label8005E5B0:
  s1++;
  v0 = (int32_t)s1 < 8;
  temp = v0 != 0;
  s0 += 4; // 0x0004
  if (temp) goto label8005E598;
  ra = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 32; // 0x0020
  return;
}