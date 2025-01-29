#include "main.h"
#include "psx_mem.h"
#include "psx_bios.h"
#include "decompilation.h"
#include <stdio.h>

// size: 0x0000009C
void function_8005DD0C(void)
{
  uint32_t timeout_counter = a1 << 15;
  while ((int32_t)lw(frame_counter) < (int32_t)a0) {
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