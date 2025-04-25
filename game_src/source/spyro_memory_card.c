#include "main.h"
#include "psx_bios.h"
#include "psx_mem.h"
#include "decompilation.h"

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
  BREAKPOINT;
  v0 = init_memory_card();
}



// size: 0x0000005C
void function_80067D74(void)
{
  uint32_t temp;
  a2 = a0;
  a3 = a1;
  a1 = 0x800121C0; // "bu00:"
  v0 = lw(a1 + 0x00);
  v1 = lb(a1 + 0x04);
  a0 = lb(a1 + 0x05);
  sw(a3 + 0x00, v0);
  sb(a3 + 0x04, v1);
  sb(a3 + 0x05, a0);
  temp = (int32_t)a2 >= 0;
  v0 = a2;
  if (temp) goto label80067DB0;
  v0 = a2 + 15; // 0x000F
label80067DB0:
  v0 = (int32_t)v0 >> 4;
  v1 = v0 + 48; // 0x0030
  v0 = v0 << 4;
  v0 = a2 - v0;
  v0 += 48; // 0x0030
  sb(a3 + 0x0002, v1);
  sb(a3 + 0x0003, v0);
}

// size: 0x00000108
void function_8006815C(void)
{
  TestEvent(lw(memcard_events + 0x00));
  TestEvent(lw(memcard_events + 0x04));
  TestEvent(lw(memcard_events + 0x08));
  TestEvent(lw(memcard_events + 0x0C));
  TestEvent(lw(memcard_events + 0x10));
  TestEvent(lw(memcard_events + 0x14));
  TestEvent(lw(memcard_events + 0x18));
  TestEvent(lw(memcard_events + 0x1C));
  sw(0x80075B38, 0);
  sw(0x80075B34, lw(0x80075B38));
  sw(0x80075B30, lw(0x80075B34));
  sw(0x80075B2C, lw(0x80075B30));
  sw(0x80075B48, 0);
  sw(0x80075B44, lw(0x80075B48));
  sw(0x80075B40, lw(0x80075B44));
  sw(0x80075B3C, lw(0x80075B40));
}

// size: 0x000000DC
void function_80068264(void)
{
  uint32_t event = 0;

  while (event == 0) {
    event = lw(0x80075B2C)*1
          + lw(0x80075B30)*2
          + lw(0x80075B34)*4
          + lw(0x80075B38)*8;
  }

  TestEvent(lw(0x80075B1C));
  TestEvent(lw(0x80075B20));
  TestEvent(lw(0x80075B24));
  TestEvent(lw(0x80075B28));
  sw(0x80075B38, 0);
  sw(0x80075B34, lw(0x80075B38));
  sw(0x80075B30, lw(0x80075B34));
  sw(0x80075B2C, lw(0x80075B30));

  v0 = event >> 1;
}

// size: 0x000000DC
void function_80068340(void)
{
  uint32_t event = 0;

  while (event == 0) {
    event = lw(0x80075B3C)*1
          + lw(0x80075B40)*2
          + lw(0x80075B44)*4
          + lw(0x80075B48)*8;
  }

  TestEvent(lw(memcard_events + 0x00));
  TestEvent(lw(memcard_events + 0x04));
  TestEvent(lw(memcard_events + 0x08));
  TestEvent(lw(memcard_events + 0x0C));

  sw(0x80075B48, 0);
  sw(0x80075B44, lw(0x80075B48));
  sw(0x80075B40, lw(0x80075B44));
  sw(0x80075B3C, lw(0x80075B40));

  v0 = event >> 1;
}

// size: 0x0000003C
void function_8006841C(void)
{
  v0 = lw(0x80075B2C)*1
     + lw(0x80075B30)*2
     + lw(0x80075B34)*4
     + lw(0x80075B38)*8;
}

// size: 0x0000003C
void function_80068458(void)
{
  v0 = lw(0x80075B3C)*1
     + lw(0x80075B40)*2
     + lw(0x80075B44)*4
     + lw(0x80075B48)*8;
}

// size: 0x00000014
void function_80067DD0(void)
{
  sw(0x80075B2C, 1);
  v0 = 0;
}

// size: 0x00000014
void function_80067DE4(void)
{
  sw(0x80075B30, 1);
  v0 = 0;
}

// size: 0x00000014
void function_80067DF8(void)
{
  sw(0x80075B34, 1);
  v0 = 0;
}

// size: 0x00000014
void function_80067E0C(void)
{
  sw(0x80075B38, 1);
  v0 = 0;
}

// size: 0x00000014
void function_80067E20(void)
{
  sw(0x80075B3C, 1);
  v0 = 0;
}

// size: 0x00000014
void function_80067E34(void)
{
  sw(0x80075B40, 1);
  v0 = 0;
}

// size: 0x00000014
void function_80067E48(void)
{
  sw(0x80075B44, 1);
  v0 = 0;
}

// size: 0x00000014
void function_80067E5C(void)
{
  sw(0x80075B48, 1);
  v0 = 0;
}

// size: 0x000001E4
void MemCardStart(void)
{
  uint32_t crit = EnterCriticalSection();
  sw(memcard_events + 0x00, OpenEvent(0xF4000001, 0x0004, 0x1000, 0x80067DD0));
  sw(memcard_events + 0x04, OpenEvent(0xF4000001, 0x8000, 0x1000, 0x80067DE4));
  sw(memcard_events + 0x08, OpenEvent(0xF4000001, 0x0100, 0x1000, 0x80067DF8));
  sw(memcard_events + 0x0C, OpenEvent(0xF4000001, 0x2000, 0x1000, 0x80067E0C));
  sw(memcard_events + 0x10, OpenEvent(0xF0000011, 0x0004, 0x1000, 0x80067E20));
  sw(memcard_events + 0x14, OpenEvent(0xF0000011, 0x8000, 0x1000, 0x80067E34));
  sw(memcard_events + 0x18, OpenEvent(0xF0000011, 0x0100, 0x1000, 0x80067E48));
  sw(memcard_events + 0x1C, OpenEvent(0xF0000011, 0x2000, 0x1000, 0x80067E5C));
  EnableEvent(lw(memcard_events + 0x00));
  EnableEvent(lw(memcard_events + 0x04));
  EnableEvent(lw(memcard_events + 0x08));
  EnableEvent(lw(memcard_events + 0x0C));
  EnableEvent(lw(memcard_events + 0x10));
  EnableEvent(lw(memcard_events + 0x14));
  EnableEvent(lw(memcard_events + 0x18));
  EnableEvent(lw(memcard_events + 0x1C));

  function_8006815C();
  
  if (crit == 1) ExitCriticalSection();
}

// size: 0x000001E4
void function_80067EA0(void)
{
  BREAKPOINT;
  MemCardStart();
}

// size: 0x000000B8
void MemCardStop(void)
{
  uint32_t crit = EnterCriticalSection();
  CloseEvent(lw(memcard_events));
  CloseEvent(lw(0x80075B10));
  CloseEvent(lw(0x80075B14));
  CloseEvent(lw(0x80075B18));
  CloseEvent(lw(0x80075B1C));
  CloseEvent(lw(0x80075B20));
  CloseEvent(lw(0x80075B24));
  CloseEvent(lw(0x80075B28));
  if (crit == 1) ExitCriticalSection(); 
}

// size: 0x000000B8
void function_800680A4(void)
{
  BREAKPOINT;
  MemCardStop();
}