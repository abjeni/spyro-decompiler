#include "main.h"
#include "psx_mem.h"
#include "psx_bios.h"
#include "decompilation.h"
#include "int_math.h"
#include <stdio.h>

// size: 0x00000030
void function_8005DEBC(void)
{
  if (lw(lw(0x800749AC) + 0x10) != 0x8005E378) BREAKPOINT;
  ra = 0x8005DEDC;
  // function_8005E378();
  BREAKPOINT;
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
  v0 = set_I_MASK(a0);
}

// size: 0x0000002C
void function_8005E5D8(void)
{
  if (a1 != lw(0x800749C0 + a0*4))
    sw(0x800749C0 + a0*4, a1);
}

// size: 0x00000034
void function_8005DE58(void)
{
  a1 = a0;
  a0 = 4;
  if (lw(lw(0x800749AC) + 0x14) != 0x8005E5D8) BREAKPOINT;
  ra = 0x8005DE7C;
  function_8005E5D8();
}

// size: 0x00000030
void function_8005DE8C(void)
{
  if (lw(lw(0x800749AC) + 0x14) != 0x8005E5D8) BREAKPOINT;
  ra = 0x8005DEAC;
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
  wait_until_frame(a0, a1);
}

// size: 0x00000148
int VSync(int mode)
{
  union gpustat GPUSTAT;
  uint32_t time_delta;

  GPUSTAT = psx_gpustat();
  time_delta = (lw(lw(TIMER_1_horizontal_retrace_ptr)) - lw(VSync_last_vsync_time)) & 0xFFFF;
  if ((int32_t)mode < 0) return lw(frame_counter);
  if (mode == 1) return time_delta;
  int32_t skip_frames = max_int(0, mode-1);
  wait_until_frame(lw(VSync_last_frame_num) + skip_frames, skip_frames);
  GPUSTAT = psx_gpustat();
  wait_until_frame(lw(frame_counter)+1, 1);
  if (GPUSTAT.vertical_interlace) while (GPUSTAT.even_odd_interlace == psx_gpustat().even_odd_interlace);
  sw(VSync_last_frame_num, lw(frame_counter));
  sw(VSync_last_vsync_time, lw(lw(TIMER_1_horizontal_retrace_ptr)));
  return time_delta;
}

// size: 0x00000148
void function_8005DBC4(void)
{
  v0 = VSync(a0);
}