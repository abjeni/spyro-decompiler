#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "debug.h"
#include "main.h"
#include "psx_mem.h"
#include "psx_bios.h"
#include <unistd.h>

extern uint32_t hi, lo;

void divu_psx(uint32_t a, uint32_t b)
{
  if (b == 0)
    BREAKPOINT;

  lo = a/b;
  hi = a%b;
}

void div_psx(int32_t a, int32_t b)
{
  if (b == 0)
  {
    lo = 0;
    hi = 0;
    return;
  }
  
  if (b == -1 && a == 0x80000000)
    BREAKPOINT;

  lo = a/b;
  hi = a%b;
}

void multu(uint32_t a, uint32_t b)
{
  uint64_t result = (uint64_t)a*(uint64_t)b;
  hi = result>>32;
  lo = result&0xFFFFFFFF;
}

void mult(int32_t a, int32_t b)
{
  int64_t result = (int64_t)a*(int64_t)b;
  hi = result>>32;
  lo = result&0xFFFFFFFF;
}

void A(file_loc loc)
{
  switch (t1) {
  case 0x39:
    InitHeap(a0, a1);
    break;
  case 0x44:
    FlushCache();
    break;
  case 0x49:
    GPU_cw(a0);
    break;
  case 0x70:
    _bu_init();
    break;
  case 0x72:
    _96_remove();
    break;
  case 0xAB:
    v0 = _card_info(a0);
    break;
  case 0xAC:
    v0 = _card_load(a0);
    break;
  default:
    printf("ERROR: %s:%u: A(0x%.2X)\n", loc.file, loc.line, t1);
    BREAKPOINT;
  }
}

void B(file_loc loc)
{
  switch (t1) {
  case 0x07:
    DeliverEvent(a0, a1);
    break;
  case 0x08:
    v0 = OpenEvent(a0, a1, a2, a3);
    break;
  case 0x09:
    v0 = CloseEvent(a0);
    break;
  case 0x0B:
    v0 = TestEvent(a0);
    break;
  case 0x0C:
    v0 = EnableEvent(a0);
    break;
  case 0x17:
    ReturnFromException();
    break;
  case 0x19:
    HookEntryInt(a0);
    break;
  case 0x32:
    v0 = psx_open(addr_to_pointer(a0), a1);
    break;
  case 0x33:
    v0 = psx_lseek(a0, a1, a2);
    break;
  case 0x34:
    v0 = psx_read(a0, addr_to_pointer(a1), a2);
    break;
  case 0x35:
    v0 = psx_write(a0, addr_to_pointer(a1), a2);
    break;
  case 0x36:
    v0 = psx_close(a0);
    break;
  case 0x56:
    v0 = GetC0Table();
    break;
  case 0x4A:
    InitCARD2(a0);
    break;
  case 0x4B:
    StartCARD2();
    break;
  case 0x4E:
    _card_write(a0, a1, a2);
    break;
  case 0x50:
    _new_card();
    break;
  case 0x5B:
    v0 = ChangeClearPAD(a0);
    break;
  default:
    printf("ERROR: %s:%u: B(0x%.2X)\n", loc.file, loc.line, t1);
    BREAKPOINT;
  }
}

void C(file_loc loc)
{
  switch (t1) {
  case 0x02:
    SysEnqIntRP(a0, a1);
    break;
  case 0x03:
    SysDeqIntRP(a0, a1);
    break;
  case 0x0A:
    v0 = ChangeClearRCnt(a0, a1);
    break;
  default:
    printf("ERROR: %s:%u: C(0x%.2X)\n", loc.file, loc.line, t1);
    BREAKPOINT;
  }
}

/*
  SYS(00h) NoFunction()
  SYS(01h) EnterCriticalSection()
  SYS(02h) ExitCriticalSection()
  SYS(03h) ChangeThreadSubFunction(addr) ;syscall with r4=03h, r5=addr
  SYS(04h..FFFFFFFFh) calls DeliverEvent(F0000010h,4000h)
*/

void system_call(file_loc loc)
{
  switch (a0) {
  case 1:
    v0 = EnterCriticalSection();
    break;
  case 2:
    ExitCriticalSection();
    break;
  default:
    printf("ERROR: %s:%u: system_call(%d)\n", loc.file, loc.line, a0);
    BREAKPOINT;
  }
}

void JR(uint32_t addr, uint32_t location, file_loc loc)
{
  switch (addr) {
  case 0xA0:
    A(loc);
    break;
  case 0xB0:
    B(loc);
    break;
  case 0xC0:
    C(loc);
    break;
  default:
    printf("ERROR: %s:%u: %.8X: goto label%.8X;\n", loc.file, loc.line, location, addr);
    BREAKPOINT;
  }
}


void JALR(uint32_t addr, uint32_t location, file_loc loc)
{
  switch (addr) {
  case 0xA0:
    A(loc);
    break;
  case 0xB0:
    B(loc);
    break;
  case 0xC0:
    C(loc);
    break;
  default:
    printf("ERROR: %s:%u: %.8X: function_%.8X();\n", loc.file, loc.line, location, addr);
    BREAKPOINT;
  }
}