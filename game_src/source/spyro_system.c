#include "main.h"
#include "psx_mem.h"
#include "psx_bios.h"

// size: 0x00000068
void spyro_patch_bios(void)
{
  return;
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
}

// size: 0x00000068
void function_80062648(void)
{
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