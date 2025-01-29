#include "psx_bios.h"
#include "debug.h"
#include "main.h"
#include "psx_mem.h"

// size: 0x0000000C
void function_8005DB14(void)
{
  BREAKPOINT;
  InitHeap(a0, a1);
}

// size: 0x0000000C
void function_8005DB24(void)
{
  BREAKPOINT;
  LoadExec((char *)addr_to_pointer(a0), a1, a2);
}

// size: 0x0000000C
void function_8005DB34(void)
{
  BREAKPOINT;
  _96_init();
}

// size: 0x0000000C
void function_8005DB4C(void)
{
  BREAKPOINT;
  _96_remove();
}

// size: 0x0000000C
void function_8005DB64(void)
{
  BREAKPOINT;
  DeliverEvent(a0, a1);
}

// size: 0x0000000C
void function_8005DB74(void)
{
  BREAKPOINT;
  v0 = OpenEvent(a0, a1, a2, a3);
}

// size: 0x0000000C
void function_8005DB84(void)
{
  BREAKPOINT;
  v0 = TestEvent(a0);
}

// size: 0x0000000C
void function_8005DB94(void)
{
  BREAKPOINT;
  v0 = EnableEvent(a0);
}

// size: 0x00000010
void function_8005DBA4(void)
{
  BREAKPOINT;
  v0 = EnterCriticalSection();
}

// size: 0x00000010
void function_8005DBB4(void)
{
  BREAKPOINT;
  ExitCriticalSection();
}

// size: 0x0000000C
void function_8005DDA8(void)
{
  BREAKPOINT;
  v0 = ChangeClearPAD(a0);
}

// size: 0x0000000C
void function_8005DDB8(void)
{
  BREAKPOINT;
  v0 = ChangeClearRCnt(a0, a1);
}

// size: 0x0000000C
void function_8005E4D8(void)
{
  BREAKPOINT;
  ReturnFromException();
}

// size: 0x0000000C
void function_8005E4E8(void)
{
  BREAKPOINT;
  ResetEntryInt();
}

// size: 0x0000000C
void function_8005E4F8(void)
{
  BREAKPOINT;
  HookEntryInt(a0);
}

// size: 0x0000000C
void function_80062338(void)
{
  BREAKPOINT;
  GPU_cw(a0);
}

// size: 0x0000000C
void function_800626E8(void)
{
  BREAKPOINT;
  FlushCache();
}

// size: 0x0000000C
void function_8006396C(void)
{
  BREAKPOINT;
  psx_exit(a0);
}

// size: 0x0000000C
void function_80068494(void)
{
  BREAKPOINT;
  _bu_init();
}

// size: 0x0000000C
void function_800684A4(void)
{
  BREAKPOINT;
  v0 = CloseEvent(a0);
}

// size: 0x0000000C
void function_800684B4(void)
{
  BREAKPOINT;
  v0 = psx_open(addr_to_pointer(a0), a1);
}

// size: 0x0000000C
void function_800684C4(void)
{
  BREAKPOINT;
  v0 = psx_lseek(a0, a1, a2);
}

// size: 0x0000000C
void function_800684D4(void)
{
  BREAKPOINT;
  v0 = psx_read(a0, addr_to_pointer(a1), a2);
}

// size: 0x0000000C
void function_800684E4(void)
{
  BREAKPOINT;
  v0 = psx_close(a0);
}

// size: 0x0000000C
void function_800684F4(void)
{
  BREAKPOINT;
  v0 = format(a0);
}

// size: 0x0000000C
void function_800688E0(void)
{
  BREAKPOINT;
  v0 = _card_info(a0);
}

// size: 0x0000000C
void function_800688F0(void)
{
  BREAKPOINT;
  v0 = _card_load(a0);
}

// size: 0x0000000C
void function_80068900(void)
{
  BREAKPOINT;
  _card_write(a0, a1, a2);
}

// size: 0x0000000C
void function_80068910(void)
{
  BREAKPOINT;
  _new_card();
}

// size: 0x0000000C
void function_80068D50(void)
{
  BREAKPOINT;
  SysEnqIntRP(a0, a1);
}

// size: 0x0000000C
void function_80068D60(void)
{
  BREAKPOINT;
  SysDeqIntRP(a0, a1);
}

// size: 0x0000000C
void function_80068E80(void)
{
  BREAKPOINT;
  InitCARD2(a0);
}

// size: 0x0000000C
void function_80068E90(void)
{
  BREAKPOINT;
  StartCARD2();
}