#include "main.h"
#include "psx_bios.h"
#include "psx_mem.h"

// size: 0x00000010
void function_80068A1C(void)
{
  v0 = lw(0x800751A0);
}

// size: 0x00000058
void function_80068958(void)
{
  sp -= 0x18;
  sw(sp + 0x10, s0);
  sw(sp + 0x14, ra);
  
  s0 = a0;

  ChangeClearPAD(0);
  EnterCriticalSection();
  function_80068A1C(); // v0 = lw(0x800751A0);
  if (v0 == 0)
    s0 = 0;
  InitCARD2(s0);
  ExitCriticalSection();

  ra = lw(sp + 0x14);
  s0 = lw(sp + 0x10);
  sp += 0x18;
}

// size: 0x00000038
void function_800689B0(void)
{
  EnterCriticalSection();
  StartCARD2();
  ChangeClearPAD(0);
  ExitCriticalSection();
}

// size: 0x00000030
void function_80067E70(void)
{
  function_80068958();
  function_800689B0();
  _bu_init();
  v0 = 0;
}

// size: 0x00000030
void function_8006626C(void)
{
  sw(0x80075B4C, 0);
  sw(0x80075B90, 0);
  function_80067E70();
}

// size: 0x00000020
uint32_t init_memory_card(void)
{
  a0 = 0;
  function_8006626C();
  return v0;
}

// size: 0x00000020
void function_80012460(void)
{
  v0 = init_memory_card();
}