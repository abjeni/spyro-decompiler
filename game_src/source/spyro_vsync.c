#include "main.h"
#include "psx_mem.h"
#include "psx_bios.h"
#include "decompilation.h"
#include "int_math.h"
#include <stdio.h>

// size: 0x00000030
void function_8005DEBC(void)
{
  if (lw(lw(0x800749AC) + 0x10) != 0x8005E378) UNREACHABLE;
  // function_8005E378();
  UNREACHABLE;
}

// size: 0x0000001C
uint32_t set_I_MASK(uint32_t i_mask)
{
  uint32_t i_mask_old = lw(lw(I_MASK_ptr));
  sh(lw(I_MASK_ptr), i_mask);
  return i_mask_old;
}

// size: 0x0000001C
void function_8005DF44(void)
{
  UNREACHABLE;
  v0 = set_I_MASK(a0);
}

// size: 0x0000002C
void function_8005E5D8(void)
{
  if (a1 != lw(0x800749C0 + a0*4))
    sw(0x800749C0 + a0*4, a1);
}

// size: 0x00000030
void function_8005DE8C(void)
{
  if (lw(lw(0x800749AC) + 0x14) != 0x8005E5D8) UNREACHABLE;
  function_8005E5D8();
}

// size: 0x00000034
void function_8005DE58(void)
{
  a1 = a0;
  a0 = 4;
  if (lw(lw(0x800749AC) + 0x14) != 0x8005E5D8) UNREACHABLE;
  function_8005E5D8();
}

// size: 0x0000009C
void wait_until_frame(int32_t frame, uint32_t timeout_time)
{
  uint32_t timeout_counter = timeout_time << 15;
  while ((int32_t)lw(frame_counter) < frame) {
    inter(0);
    timeout_counter--;
    if (timeout_counter == -1) {
      printf("VSync: timeout\n");
      ChangeClearPAD(0);
      ChangeClearRCnt(3, 0);
      break;
    }
  }
}

void function_8005DD0C(void)
{
  UNREACHABLE;
  wait_until_frame(a0, a1);
}

// size: 0x00000148
int VSync(int mode)
{
  static uint32_t last_frame_num = 0;
  static uint32_t last_vsync_time = 0;

  uint32_t time_delta;

  time_delta = (lw(lw(TIMER_1_horizontal_retrace_ptr)) - last_vsync_time) & 0xFFFF;

  if ((int32_t)mode < 0) return lw(frame_counter);

  if (mode == 1) return time_delta;

  int32_t skip_frames = max_int(0, mode-1);
  wait_until_frame(last_frame_num + skip_frames, skip_frames);

  union gpustat GPUSTAT = psx_gpustat();

  wait_until_frame(lw(frame_counter)+1, 1);

  if (GPUSTAT.vertical_interlace) while (GPUSTAT.even_odd_interlace == psx_gpustat().even_odd_interlace);
  last_frame_num = lw(frame_counter);
  last_vsync_time = lw(lw(TIMER_1_horizontal_retrace_ptr));
  return time_delta;
}

// size: 0x00000148
void function_8005DBC4(void)
{
  UNREACHABLE;
  v0 = VSync(a0);
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

void function_80062648(void)
{
  // not doing all that
}