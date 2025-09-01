#include "psx_mem.h"
#include "spyro_string.h"
#include "spyro_system.h"
#include "not_renamed.h"
#include "psx_bios.h"
#include "main.h"
#include "decompilation.h"

#include <stdint.h>

// size: 0x00000034
void function_8005375C(void)
{
  sw(a0 + 0x20, 0x7F7F7F7F);
  sw(a0 + 0x24, 0);
  sw(a0 + 0x28, 0x7F7F7F7F);
}


// size: 0x000001B0
void function_80053790(void)
{
  sp -= 0x18;
  sw(sp + 0x10, s0);
  sw(sp + 0x14, ra);

  s0 = a0;
  
  t1 = 1;
  for (int i = 0; i < 4; i++)
  {
    a2 = lbu(s0 + i + 0x20);
    if (a2 + 0x30 >= lbu(s0 + i + 0x28) || lbu(s0 + i + 0x24) >= (int32_t)(a2 - 0x30)) {
      a0 = s0;
      function_8005375C();
      t1 = 0;
      break;
    }
  }
  
  for (int i = 0; i < 4; i++)
  {
    a0 = s0 + i;
    sb(s0 + i + 0x2C, lbu(s0 + i + 0x20) + 0x30);
    sb(s0 + i + 0x30, lbu(s0 + i + 0x20) - 0x30);
    
    sh(s0 + 2*i + 0x34, 0x4000 / (lbu(s0 + i + 0x28) - lbu(s0 + i + 0x2C)));
    sh(s0 + 2*i + 0x3C, 0x3F80 / (lbu(s0 + i + 0x30) - lbu(s0 + i + 0x24)));
  }

  sw(s0 + 0x00, 0);
  sw(s0 + 0x04, 0);
  sw(s0 + 0x08, 0);
  sw(s0 + 0x14, 0x7F7F7F7F);
  sw(s0 + 0x18, 1);
  sw(s0 + 0x1C, 1);

  for (int i = 0; i < 4; i++)
  {
    sw(s0 + 0x48 + 0x18*i + 0x00, 0);
    sw(s0 + 0x48 + 0x18*i + 0x04, 0);
    sw(s0 + 0x48 + 0x18*i + 0x08, 0);
    sw(s0 + 0x48 + 0x18*i + 0x10, 0x7F7F7F7F);
  }
  v0 = t1;
  ra = lw(sp + 0x14);
  s0 = lw(sp + 0x10);
  sp += 0x18;
}

// size: 0x00000034
void function_8006B6A0(void)
{
  sw(0x800751D4, 0x8006B6D4);
  sw(0x800751D8, 0x8006BAE8);
  sw(0x800751DC, 0x8006B7A8);
}

// size: 0x0000002C
void set_controller_interrupt_chain_node(void)
{
  sw(0x80075C58 + 0x00, 0);
  sw(0x80075C58 + 0x04, 0x8006969C);
  sw(0x80075C58 + 0x08, 0x80069634);
  sw(0x80075C58 + 0x0C, 0);
}

void function_80069608(void)
{
  BREAKPOINT;
  set_controller_interrupt_chain_node();
}

// size: 0x00000160
void function_8006B010(void)
{
  sp -= 0x20;
  sw(sp + 0x14, s1);
  sw(sp + 0x18, s2);
  sw(sp + 0x1C, ra);
  sw(sp + 0x10, s0);

  s1 = a0;
  s2 = a1;

  sw(0x800751F4, 0);
  sw(0x80075208, 0);
  
  function_8006B6A0();

  s0 = 0x80075D18;
  sw(0x800751BC, 0x8006B1D8);
  sw(0x800751C0, 0x8006B170);
  sw(0x800751C4, 0x8006B2DC);
  sw(0x800751C8, 0x8006B398);
  sw(0x800751CC, 0x8006B614);
  sw(0x800751D0, 0x8006B64C);
  sw(0x800751F0, s0);
  sw(0x800751E0, 0x8006B2CC);
  
  spyro_memclr8(s0, 0x1E0);

  t0 = 0;
  t1 = 0xFF;
  sw(s0 + 0xF0*0 + 0x30, s1);
  sw(s0 + 0xF0*1 + 0x30, s2);
  for (int j = 0; j < 2; j++) {
    sw(s0 + 0xF0*j + 0x0C, 0);
    sw(s0 + 0xF0*j + 0x10, s0);
    sb(lw(s0 + 0xF0*j + 0x30) + 0, 0xFF);
    sb(lw(s0 + 0xF0*j + 0x30) + 1, 0x00);
    sw(s0 + 0xF0*j + 0x3C, 0x80075C88 + 35*j);
    sw(s0 + 0xF0*j + 0x40, 0x80075CD0 + 35*j);

    for (int i = 0; i < 5; i++)
      sb(s0 + 0xF0*j + 0x5D + i, t1);
  }
  
  set_controller_interrupt_chain_node();
  
  sw(0x800751F4, 1);
  ra = lw(sp + 0x1C);
  s2 = lw(sp + 0x18);
  s1 = lw(sp + 0x14);
  s0 = lw(sp + 0x10);
  sp += 0x20;
}

// size: 0x00000068
void function_8006B170(void)
{
  if (lbu(a0 + 0x49)) {
    sw(a0 + 0x00, 0);
    sw(a0 + 0x04, 0);
    sw(a0 + 0x08, 0);
    sw(a0 + 0x14, 0);
    sw(a0 + 0x18, 0);
    sb(a0 + 0x46, 0);
    sb(a0 + 0x49, 0);
    sb(a0 + 0xE3, 0);
    sb(a0 + 0xE4, 0);
    sh(a0 + 0xE6, 0);
    sb(a0 + 0xE9, 0);
    sb(a0 + 0xEA, 0);
    for (int i = 0; i < 6; i++)
      sb(a0 + 0x5D + i, 0xFF);
  }
}

// size: 0x00000044
void stop_registering_gamepads_internal(void)
{
  EnterCriticalSection();
  ChangeClearRCnt(3, 1);
  SysDeqIntRP(2, 0x80075C58);
  ExitCriticalSection();
}

void function_800698E8(void)
{
  BREAKPOINT;
  stop_registering_gamepads_internal();
}

// size: 0x00000020
void stop_registering_gamepads(void)
{
  stop_registering_gamepads_internal();
}

// size: 0x00000020
void function_80069080(void)
{
  BREAKPOINT;
  stop_registering_gamepads();
}

// size: 0x000000CC
void start_registering_gamepads_internal(void)
{
  sp -= 0x18;
  sw(sp + 0x14, ra);
  sw(sp + 0x10, s0);

  sw(0x800751F4, 0);

  EnterCriticalSection();
  {
    SysDeqIntRP(2, 0x80075C58);
    SysEnqIntRP(2, 0x80075C58);
    sw(lw(I_STAT_ptr), ~1);
    sw(lw(I_MASK_ptr), lw(lw(I_MASK_ptr)) | 1);
    ChangeClearRCnt(3, 0);
  }
  ExitCriticalSection();

  if (lw(0x800751C0) != 0x8006B170) BREAKPOINT;

  a0 = lw(0x800751F0);
  function_8006B170();
  
  a0 = lw(0x800751F0) + 0xF0;
  function_8006B170();

  sw(0x80075C68 + 4, 0);
  sw(0x80075C68 + 0, 0);
  sw(0x800751F4, 1);

  ra = lw(sp + 0x14);
  s0 = lw(sp + 0x10);
  sp += 0x18;
}

void function_8006981C(void)
{
  BREAKPOINT;
  start_registering_gamepads_internal();
}

// size: 0x00000020
void start_registering_gamepads(void)
{
  start_registering_gamepads_internal();
}

// size: 0x00000020
void function_80069060(void)
{
  BREAKPOINT;
  start_registering_gamepads();
}

// size: 0x00000098
void init_controller(void)
{
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0014, ra);
  sw(sp + 0x0010, s0);

  a0 = 0x800786A0;
  a1 = 0x80078E50;
  function_8006B010();

  s0 = 0x800776D8;
  a0 = s0;
  function_8005375C();

  a0 = s0;
  function_80053790();
  
  sw(0x80075904, 0);
  sw(0x80075764, 0);
  sw(0x800757D0, 0);
  sw(0x8007584C, 0);
  sw(0x80075730, 1);
  
  start_registering_gamepads();
  
  function_80053C68();
  a0 = 0x80053C68;
  function_8005DE58();

  ra = lw(sp + 0x14);
  s0 = lw(sp + 0x10);
  sp += 0x18;
}

// size: 0x00000028
void controller_wait_for_data(void)
{
  v1 = lw(JOY_BASE_ptr);
  while ((lhu(v1 + 4) & 2) == 0);
}

// size: 0x00000028
void function_8006A0A4(void)
{
  BREAKPOINT;
  controller_wait_for_data();
}

// size: 0x00000098
void function_800123C8(void)
{
  BREAKPOINT;
  init_controller();
}

// size: 0x00000090
void function_8006A014(void)
{
  uint32_t joy_base = lw(JOY_BASE_ptr);
  sw(lw(I_STAT2_ptr), ~0x80);
  while (lhu(joy_base + 0x04) & 0x80) {
    v0 = 0;
    if (get_timer()) return;
  }
  sh(joy_base + 0x0A, lhu(joy_base + 0x0A) | 0x10);
  v0 = 1;
  return;
}

// size: 0x00000054
void function_80053708(void)
{
  sp -= 0x18;
  sw(sp + 0x10, s0);
  sw(sp + 0x14, ra);

  s0 = a0;

  sb(0x80075944, 1-lbu(0x80075944));
  spyro_memcpy32(a1, s0, 0xA4);

  a0 = s0;
  function_800536A4();
  
  ra = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 0x18;
}

// size: 0x00000064
void function_800536A4(void)
{
  sw(a0 + 0x00, 0);
  sw(a0 + 0x04, 0);
  sw(a0 + 0x08, 0);
  sw(a0 + 0x14, 0x7F7F7F7F);
  sw(a0 + 0x18, 1);
  sw(a0 + 0x1C, 1);
  for (int i = 0; i < 4; i++) {
    sw(a0 + 0x44 + i*0x18 + 0x00, lw(a0 + 0x0C));
    sw(a0 + 0x44 + i*0x18 + 0x04, 0);
    sw(a0 + 0x44 + i*0x18 + 0x08, 0);
    sw(a0 + 0x44 + i*0x18 + 0x0C, 0);
    sw(a0 + 0x44 + i*0x18 + 0x14, 0x7F7F7F7F);
  }
}