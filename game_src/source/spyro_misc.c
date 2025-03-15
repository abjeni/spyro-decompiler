#include "psx_bios.h"
#include "main.h"
#include "spyro_psy.h"
#include "decompilation.h"

#include <stdio.h>

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