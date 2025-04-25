#include "psx_bios.h"
#include "psx_mem.h"
#include "main.h"
#include "spyro_psy.h"
#include "decompilation.h"
#include "not_renamed.h"

#include <stdio.h>
#include <string.h>

// size: 0x00000010
void function_80017FD4(void)
{
  v0 = lhu(TIMER2);
}

// size: 0x00000050
void function_8002BA68(void)
{
  SetDispMask(0);
  ra = 0x8002BA80;
  function_8006397C();
  a0 = 0;
  ra = 0x8002BA88;
  function_8006623C();
  a0 = 0;
  ra = 0x8002BA90;
  function_8005DE58();
  ra = 0x8002BA98;
  function_80069080();
  ResetGraph(3);
  function_8005DEBC();
}

// size: 0x00000068
void function_8002BAB8(void)
{
  printf("Shutting Down\n");
  function_8002BA68();
  _96_remove();
  _96_init();
  printf("Launching Crash Demo\n");
  LoadExec("cdrom:\\S0\\CRASH.EXE;1", 0x801FFF00, 0);
}

// size: 0x00000008
void function_8002C914(void)
{
}

// size: 0x00000008
void function_8002C91C(void)
{
}

// size: 0x0000002C
void function_8002D554(void)
{
  memset(addr_to_pointer(0x80076BC0), 0, 0x40);
  sw(0x80075928, 0);
}

// size: 0x00000020
void function_8002F3C4(void)
{
  function_8002C91C();
}

// size: 0x00000008
void function_80037E98(void)
{
}

// size: 0x00000018
void function_800529CC(void)
{
  sw(a0 + 0x08, 0);
  sh(a0 + 0x34, -1);
  sb(a0 + 0x41, 0);
  sb(a0 + 0x4B, 0);
}

// size: 0x00000008
void function_80058B60(void)
{
  sw(a0, 0);
}

// size: 0x00000028
void function_8005956C(void)
{
  v0 = 0;
  for (int i = 0; i < 0x58C; i++)
    v0 += lbu(a0 + i);
}

// size: 0x00000028
void function_8005C7AC(void)
{
  a1 = 0;
  a2 = 0x17;
  a3 = 0;
  function_8005CFEC();
}

// size: 0x0000003C
void function_800638EC(void)
{
  sw(a0 + 0x00, ra);
  sw(a0 + 0x04, sp);
  sw(a0 + 0x08, fp);
  sw(a0 + 0x0C, s0);
  sw(a0 + 0x10, s1);
  sw(a0 + 0x14, s2);
  sw(a0 + 0x18, s3);
  sw(a0 + 0x1C, s4);
  sw(a0 + 0x20, s5);
  sw(a0 + 0x24, s6);
  sw(a0 + 0x28, s7);
  sw(a0 + 0x2C, gp);
  v0 = 0;
}

// size: 0x00000028
void function_80063A14(void)
{
  DeliverEvent(0xF0000003, 0x20);
}

// size: 0x00000028
void function_80063A3C(void)
{
  DeliverEvent(0xF0000003, 0x40);
}

// size: 0x00000010
void function_80063A8C(void)
{
  v0 = lbu(0x80074E44);
}

// size: 0x00000010
void function_80063A9C(void)
{
  v0 = lbu(0x80074E54);
}

// size: 0x00000010
void function_80063AAC(void)
{
  v0 = lbu(0x80074E55);
}

// size: 0x00000010
void function_80063ABC(void)
{
  v0 = 0x80074E50;
}

// size: 0x00000020
void function_80063B38(void)
{
  function_80065190();
}

// size: 0x00000018
void function_80063C18(void)
{
  v0 = lw(0x80074E34);
  sw(0x80074E34, a0);
}

// size: 0x00000018
void function_80063C30(void)
{
  v0 = lw(0x80074E38);
  sw(0x80074E38, a0);
}

// size: 0x00000020
void function_80064010(void)
{
  function_8006570C();
  v0 = v0 < 1;
}

// size: 0x00000020
void function_80064030(void)
{
  function_8006580C();
  v0 = v0 < 1;
}

// size: 0x00000020
void function_80064074(void)
{
  function_800655A0();
}

// size: 0x00000018
void function_8006623C(void)
{
  v0 = lw(0x80075144);
  sw(0x80075144, a0);
}

// size: 0x00000018
void function_80066254(void)
{
  v1 = 0x80075178;
  v0 = lw(v1);
  sw(v1, a0);
}

// size: 0x00000014
void function_80067614(void)
{
  v1 = 0x80075B90;
  v0 = lw(v1);
  sw(v1, a0);
}

// size: 0x00000014
void function_80068F30(void)
{
  sw(0x800751B0, -1); // &0xFFFFFFFF
}

// size: 0x00000010
void function_80069030(void)
{
  v0 = lw(0x800751B0) >> 31;
}

// size: 0x0000000C
void function_8006A0D0(void)
{
  sw(a0 + 0x28, a1);
  sb(a0 + 0x34, a2);
}

// size: 0x00000038
void function_8006A2BC(void)
{
  v0 = ((lbu(a0 + 0xE3)+1) & 0xFE)*2;
  v0 += ((lbu(a0 + 0xE9)*5+3) & 0xFFC)+4;
  v0 += lw(a0 + 0xEC);
}

// size: 0x00000020
void function_8006A97C(void)
{
  sb(a0 + 0x36, 0x43);
  sw(a0 + 0x2C, a0 + 0x24);
  sb(a0 + 0x24, a1);
  sb(a0 + 0x35, 1);
}

// size: 0x00000014
void function_8006A99C(void)
{
  sb(a0 + 0x36, 0x45);
  sw(a0 + 0x2C, 0);
  sb(a0 + 0x35, 0);
}

// size: 0x00000020
void function_8006A9B0(void)
{
  sb(a0 + 0x36, 0x4C);
  sw(a0 + 0x2C, a0 + 0x24);
  sb(a0 + 0x24, a1);
  sb(a0 + 0x35, 1);
}

// size: 0x00000020
void function_8006A9F0(void)
{
  sb(a0 + 0x36, 0x47);
  sw(a0 + 0x2C, a0 + 0x24);
  sb(a0 + 0x24, a1);
  sb(a0 + 0x35, 1);
}

// size: 0x00000010
void function_8006B2CC(void)
{
  v0 = lbu(a0 + 0x36);
  sb(a0 + 0x36, 0);
  sb(a0 + 0x37, v0);
}

// size: 0x00000020
void function_8006B64C(void)
{
  v0 = 0x80075D18;
  if (a0 & 0xF0)
    v0 += 0xF0;
}