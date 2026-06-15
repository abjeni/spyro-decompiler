#include "psx_mem.h"
#include "spyro_string.h"
#include "spyro_system.h"
#include "not_renamed.h"
#include "psx_bios.h"
#include "main.h"
#include "decompilation.h"
#include "psx_ops.h"

#include <stdint.h>

// size: 0x00000034
void function_8005375C(void)
{
  sw(a0 + 0x20, 0x7F7F7F7F);
  sw(a0 + 0x24, 0);
  sw(a0 + 0x28, 0x7F7F7F7F);
}

// size: 0x00000020
void function_8006B64C(void)
{
  v0 = 0x80075D18;
  if (a0 & 0xF0)
    v0 += 0xF0;
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
  UNREACHABLE;
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

    spyro_memset8(a0 + 0x5D, 0xFF, 6);
    
    sb(a0 + 0xE3, 0);
    sb(a0 + 0xE4, 0);
    sh(a0 + 0xE6, 0);
    sb(a0 + 0xE9, 0);
    sb(a0 + 0xEA, 0);
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
  UNREACHABLE;
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
  UNREACHABLE;
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

  if (lw(0x800751C0) != 0x8006B170) UNREACHABLE;

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
  UNREACHABLE;
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
  UNREACHABLE;
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
  UNREACHABLE;
  controller_wait_for_data();
}

// size: 0x00000098
void function_800123C8(void)
{
  UNREACHABLE;
  init_controller();
}

// size: 0x0000002C
void function_8006BAE8(void)
{
  if (lhu(a0 + 0xE6)) {
    if (lbu(a0 + 0x46) == 0xFF) {
      v0 = 1;
    } else {
      v0 = 0;
    }
  } else {
    v0 = 1;
  }
}

// size: 0x00000010
void function_8006B2CC(void)
{
  v0 = lbu(a0 + 0x36);
  sb(a0 + 0x36, 0);
  sb(a0 + 0x37, v0);
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

// size: 0x00000038
void function_8006A2BC(void)
{
  v0 = ((lbu(a0 + 0xE3)+1) & 0xFE)*2;
  v0 += ((lbu(a0 + 0xE9)*5+3) & 0xFFC)+4;
  v0 += lw(a0 + 0xEC);
}

// size: 0x0000000C
void function_8006A0D0(void)
{
  sw(a0 + 0x28, a1);
  sb(a0 + 0x34, a2);
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

// size: 0x000000D4
void function_8006A2F4(void)
{
  uint32_t temp;
  sp -= 32; // 0xFFFFFFE0
  sw(sp + 0x0010, s0);
  s0 = a0;
  sw(sp + 0x0014, s1);
  s1 = a1;
  temp = s1 == 0;
  sw(sp + 0x0018, ra);
  if (temp) goto label8006A33C;
  v0 = lw(s0 + 0x0004);
  temp = v0 != 0;
  v0 = 0;
  if (temp) goto label8006A3B4;
  v0 = lw(0x800751D8); // &0x00000000
  temp = v0;
  switch (temp)
  {
  default:
    JALR(temp, 0x8006A32C);
  }
  temp = v0 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label8006A344;
label8006A33C:
  v0 = 0;
  goto label8006A3B4;
label8006A344:
  v1 = 4; // 0x0004
  a1 = s1 + 3; // 0x0003
  a1 = (int32_t)a1 >> 2;
  sb(s0 + 0x0049, v1);
  v1 = 1; // 0x0001
  sb(s0 + 0x0046, v1);
  v1 = 0x8006A3C8; // &0x27BDFFE8
  sw(s0 + 0x0014, v1);
  v1 = lbu(s0 + 0x00E3);
  a0 = 0x8006A470; // &0x00802821
  sw(s0 + 0x0018, a0);
  a0 = lbu(s0 + 0x00E9);
  a1 = a1 << 2;
  sw(s0 + 0x0000, a1);
  sb(s0 + 0x0047, 0);
  v1++;
  v1 = (int32_t)v1 >> 1;
  v1 = v1 << 2;
  a1 += v1;
  v1 = a0 << 2;
  v1 += a0;
  v1 += 3; // 0x0003
  v1 = v1 & 0xFFC;
  sw(s0 + 0x0004, a1);
  a1 += v1;
  sw(s0 + 0x0008, a1);
label8006A3B4:
  ra = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 32; // 0x0020
  return;
}

// size: 0x0000014C
void function_8006A170(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, s0);
  s0 = a0;
  sw(sp + 0x0014, ra);
  v1 = lbu(s0 + 0x0046);
  v0 = 3; // 0x0003
  temp = v1 == v0;
  v0 = (int32_t)v1 < 4;
  if (temp) goto label8006A1FC;
  temp = v0 == 0;
  v0 = 2; // 0x0002
  if (temp) goto label8006A1A8;
  temp = v1 == v0;
  v0 = 1; // 0x0001
  if (temp) goto label8006A1BC;
  goto label8006A2AC;
label8006A1A8:
  v0 = 4; // 0x0004
  temp = v1 == v0;
  v0 = 1; // 0x0001
  if (temp) goto label8006A220;
  goto label8006A2AC;
label8006A1BC:
  v0 = lw(s0 + 0x003C);
  v1 = lw(s0 + 0x003C);
  v0 = lbu(v0 + 0x0003);
  sb(s0 + 0x00E3, v0);
  v0 = lbu(v1 + 0x0004);
  v1 = lw(s0 + 0x003C);
  sh(s0 + 0x00E6, 0);
  sb(s0 + 0x00E4, v0);
  v0 = lbu(v1 + 0x0005);
  v1 = lw(s0 + 0x003C);
  sb(s0 + 0x00E9, v0);
  v0 = lbu(v1 + 0x0006);
  sw(s0 + 0x00EC, 0);
  sb(s0 + 0x00EA, v0);
  goto label8006A2A8;
label8006A1FC:
  v0 = lw(s0 + 0x003C);
  v1 = lbu(v0 + 0x0004);
  v0 = lbu(v0 + 0x0005);
  sb(s0 + 0x0047, 0);
  v1 = v1 << 8;
  v0 += v1;
  sh(s0 + 0x00E6, v0);
  goto label8006A2A8;
label8006A220:
  a0 = lw(s0 + 0x00EC);
  v0 = lbu(s0 + 0x0047);
  v1 = lw(s0 + 0x003C);
  v0++;
  v1 = lbu(v1 + 0x0004);
  a0 += 8; // 0x0008
  sb(s0 + 0x0047, v0);
  v1 += 3; // 0x0003
  v1 = v1 & 0x1FC;
  a0 += v1;
  v1 = lbu(s0 + 0x00EA);
  v0 = v0 & 0xFF;
  v0 = v0 < v1;
  temp = v0 == 0;
  sw(s0 + 0x00EC, a0);
  if (temp) goto label8006A264;
label8006A25C:
  v0 = 0;
  goto label8006A2AC;
label8006A264:
  a0 = s0;
  function_8006A2BC();
  v0 = (int32_t)v0 < 129;
  temp = v0 != 0;
  a0 = s0;
  if (temp) goto label8006A28C;
  v0 = 254; // 0x00FE
  sb(s0 + 0x0046, v0);
  v0 = 2; // 0x0002
  sb(s0 + 0x0049, v0);
  goto label8006A25C;
label8006A28C:
  v0 = 255; // 0x00FF
  a1 = s0 + 99; // 0x0063
  sb(s0 + 0x0046, v0);
  function_8006A2F4();
  v0 = 2; // 0x0002
  sb(s0 + 0x0046, v0);
  goto label8006A25C;
label8006A2A8:
  v0 = 1; // 0x0001
label8006A2AC:
  ra = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x00000264
void function_8006B7A8(void)
{
  uint32_t temp;
  sp -= 32; // 0xFFFFFFE0
  sw(sp + 0x0010, s0);
  s0 = a0;
  sw(sp + 0x0018, ra);
  sw(sp + 0x0014, s1);
  v0 = lw(s0 + 0x003C);
  v1 = 15; // 0x000F
  v0 = lbu(v0 + 0x0000);
  s1 = lbu(s0 + 0x00E8);
  v0 = v0 >> 4;
  temp = v0 != v1;
  sb(s0 + 0x00E8, v0);
  if (temp) goto label8006B7E0;
  sb(s0 + 0x00E8, s1);
  goto label8006B840;
label8006B7E0:
  v0 = lw(s0 + 0x0030);
  sb(v0 + 0x0000, 0);
  v0 = lw(s0 + 0x003C);
  v1 = lw(s0 + 0x0030);
  v0 = lbu(v0 + 0x0000);
  sb(v1 + 0x0001, v0);
  v0 = lbu(s0 + 0x0044);
  a0 = 2; // 0x0002
  v0 = (int32_t)a0 < (int32_t)v0;
  temp = v0 == 0;
  if (temp) goto label8006B840;
label8006B814:
  v0 = lw(s0 + 0x003C);
  v1 = lw(s0 + 0x0030);
  v0 += a0;
  v0 = lbu(v0 + 0x0000);
  v1 += a0;
  sb(v1 + 0x0000, v0);
  v0 = lbu(s0 + 0x0044);
  a0++;
  v0 = (int32_t)a0 < (int32_t)v0;
  temp = v0 != 0;
  if (temp) goto label8006B814;
label8006B840:
  v0 = lw(s0 + 0x003C);
  v0 = lbu(v0 + 0x0001);
  temp = v0 != 0;
  v0 = 1; // 0x0001
  if (temp) goto label8006B888;
  v1 = lbu(s0 + 0x0046);
  temp = v1 != v0;
  if (temp) goto label8006B878;
  v0 = lw(s0 + 0x0014);
  temp = v0 == 0;
  if (temp) goto label8006B888;
label8006B878:
  v0 = lbu(s0 + 0x0050);
  temp = v0 == 0;
  if (temp) goto label8006B8C8;
label8006B888:
  a0 = s0;
  function_8006BAE8();
  temp = v0 != 0;
  if (temp) goto label8006B8DC;
  v0 = lbu(s0 + 0x0037);
  temp = v0 != 0;
  if (temp) goto label8006B8DC;
  v0 = lbu(s0 + 0x004A);
  temp = v0 != 0;
  if (temp) goto label8006B8DC;
  v0 = lbu(s0 + 0x00E8);
  temp = v0 == s1;
  if (temp) goto label8006B8DC;
label8006B8C8:
  v0 = lw(0x800751C0); // &0x00000000
  temp = v0;
  a0 = s0;
  switch (temp)
  {
  default:
    JALR(temp, 0x8006B8D4);
  }
label8006B8DC:
  v0 = lbu(s0 + 0x0046);
  v0 -= 2; // 0xFFFFFFFE
  v0 = v0 & 0xFF;
  v0 = v0 < 252;
  temp = v0 == 0;
  sb(s0 + 0x004A, 0);
  if (temp) goto label8006B924;
  v0 = lw(s0 + 0x003C);
  v1 = lbu(v0 + 0x0000);
  v0 = 243; // 0x00F3
  temp = v1 == v0;
  if (temp) goto label8006B924;
  v0 = lw(0x800751C0); // &0x00000000
  temp = v0;
  a0 = s0;
  switch (temp)
  {
  default:
    JALR(temp, 0x8006B91C);
  }
label8006B924:
  v1 = lbu(s0 + 0x0046);
  temp = v1 == 0;
  v0 = 255; // 0x00FF
  if (temp) goto label8006B94C;
  temp = v1 == v0;
  if (temp) goto label8006B94C;
  v0 = lbu(s0 + 0x0036);
  temp = v0 == 0;
  if (temp) goto label8006B9F8;
label8006B94C:
  a0 = 1; // 0x0001
  temp = v1 == a0;
  v0 = (int32_t)v1 < 2;
  if (temp) goto label8006B9A4;
  temp = v0 == 0;
  v0 = 254; // 0x00FE
  if (temp) goto label8006B970;
  temp = v1 == 0;
  if (temp) goto label8006B988;
  goto label8006B9C0;
label8006B970:
  temp = v1 == v0;
  v0 = 255; // 0x00FF
  if (temp) goto label8006B9B8;
  temp = v1 == v0;
  if (temp) goto label8006B9F8;
  goto label8006B9C0;
label8006B988:
  v0 = lbu(s0 + 0x00E8);
  temp = v0 == 0;
  if (temp) goto label8006B9F8;
  v0 = lbu(s0 + 0x0046);
  sb(s0 + 0x0049, a0);
  goto label8006B9AC;
label8006B9A4:
  v0 = lbu(s0 + 0x0046);
  sb(s0 + 0x0047, 0);
label8006B9AC:
  v0++;
  sb(s0 + 0x0046, v0);
  goto label8006B9F8;
label8006B9B8:
  sb(s0 + 0x0046, v0);
  goto label8006B9F8;
label8006B9C0:
  v0 = lw(s0 + 0x0018);
  temp = v0 == 0;
  if (temp) goto label8006B9E0;
  temp = v0;
  a0 = s0;
  switch (temp)
  {
  default:
    JALR(temp, 0x8006B9D0);
  }
  goto label8006B9E8;
label8006B9E0:
  a0 = s0;
  function_8006A170();
label8006B9E8:
  v1 = lbu(s0 + 0x0046);
  v1 += v0;
  sb(s0 + 0x0046, v1);
label8006B9F8:
  ra = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 32; // 0x0020
  return;
}

// size: 0x00000214
void function_8006992C(void)
{
  uint32_t temp;
  sp -= 32; // 0xFFFFFFE0
  sw(sp + 0x0010, s0);
  s0 = a0;
  v1 = lw(JOY_BASE_ptr);
  v0 = 64; // 0x0040
  sw(sp + 0x0018, ra);
  sw(sp + 0x0014, s1);
  sh(v1 + 0x000A, v0);
  v0 = 13; // 0x000D
  sh(v1 + 0x000A, 0);
  sh(v1 + 0x0008, v0);
  v0 = 136; // 0x0088
  sh(v1 + 0x000E, v0);
  v1 = lbu(s0 + 0x00E8);
  v0 = 8; // 0x0008
  temp = v1 != v0;
  a0 = 145; // 0x0091
  if (temp) goto label80069978;
  a0 = 80; // 0x0050
label80069978:
  set_timer(a0);
  v0 = lw(0x800751FC); // &0x00000000
  a0 = lw(JOY_BASE_ptr);
  temp = v0 == 0;
  v1 = 4099; // 0x1003
  if (temp) goto label8006999C;
  v1 = 12291; // 0x3003
label8006999C:
  v0 = v0 << 2;
  sh(a0 + 0x000A, v1);
  v0 = lw(0x80075214 + v0); // &0xFFFFFFFF
  v1 = 0x8006FCF4 + 0x030C;
  temp = (int32_t)v0 < 0;
  v1 += 21012; // 0x5214
  if (temp) goto label80069A88;
  temp = (int32_t)v0 <= 0;
  if (temp) goto label80069A34;
  s1 = v1;
label800699C8:
  v1 = lw(0x800751FC); // &0x00000000
  v1 = v1 << 2;
  v1 += s1;
  v0 = lw(v1 + 0x0000);
  v0--;
  a0 = v0 << 4;
  a0 -= v0;
  a0 = a0 << 4;
  sw(v1 + 0x0000, v0);
  v0 = lw(s0 + 0x000C);
  v1 = lw(0x800751DC); // &0x00000000
  temp = v1;
  a0 += v0;
  switch (temp)
  {
  case 0x8006B7A8:
    function_8006B7A8();
    break;
  default:
    JALR(temp, 0x80069A08);
  }
  v0 = lw(0x800751FC); // &0x00000000
  v0 = v0 << 2;
  v0 += s1;
  v0 = lw(v0 + 0x0000);
  temp = (int32_t)v0 > 0;
  if (temp) goto label800699C8;
label80069A34:
  v0 = lw(0x800751FC); // &0x00000000
  v1 = 0x80075214; // &0xFFFFFFFF
  v0 = v0 << 2;
  a1 = v0 + v1;
  v0 = lw(a1 + 0x0000);
  temp = v0 != 0;
  v1 = -1; // 0xFFFFFFFF
  if (temp) goto label80069A88;
  a0 = s0;
  v0 = lw(0x800751DC); // &0x00000000
  temp = v0;
  sw(a1 + 0x0000, v1);
  switch (temp)
  {
  case 0x8006B7A8:
    function_8006B7A8();
    break;
  default:
    JALR(temp, 0x80069A6C);
  }
  v0 = lw(0x800751E0); // &0x00000000
  temp = v0;
  a0 = s0;
  switch (temp)
  {
  case 0x8006B2CC:
    function_8006B2CC();
    break;
  default:
    JALR(temp, 0x80069A80);
  }
label80069A88:
  v1 = lw(JOY_BASE_ptr);
  v0 = lhu(v1 + 0x0004);
  v0 = v0 & 0x200;
  temp = v0 == 0;
  if (temp) goto label80069B00;
  v0 = lhu(v1 + 0x000A);
  v0 = v0 | 0x10;
  sh(v1 + 0x000A, v0);
  v0 = lhu(v1 + 0x0004);
  v0 = v0 & 0x200;
  temp = v0 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label80069AF0;
  sb(v1 + 0x0000, v0);
  controller_wait_for_data();
  v1 = lw(JOY_BASE_ptr);
  v0 = 0;
  v1 = lbu(v1 + 0x0000);
  goto label80069B2C;
label80069AF0:
  v1 = lw(I_STAT2_ptr);
  v0 = -129; // 0xFFFFFF7F
  sw(v1 + 0x0000, v0);
label80069B00:
  v0 = lbu(s0 + 0x0050);
  temp = v0 == 0;
  if (temp) goto label80069B20;
  v0 = lbu(s0 + 0x0036);
  temp = v0 != 0;
  v0 = 0;
  if (temp) goto label80069B2C;
label80069B20:
  v1 = lw(s0 + 0x003C);
  v0 = 1; // 0x0001
  sb(v1 + 0x0000, 0);
label80069B2C:
  ra = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 32; // 0x0020
  return;
}



// size: 0x000000DC
void function_8006BA0C(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, s0);
  s0 = a0;
  sw(sp + 0x0014, ra);
  v0 = lw(s0 + 0x004C);
  v1 = lbu(s0 + 0x0046);
  v0++;
  temp = v1 == 0;
  sw(s0 + 0x004C, v0);
  if (temp) goto label8006BAA4;
  v0 = 1; // 0x0001
  temp = v1 != v0;
  if (temp) goto label8006BA64;
  v1 = lbu(s0 + 0x004A);
  v0 = v1 < 2;
  temp = v0 != 0;
  v0 = v1 + 1; // 0x0001
  if (temp) goto label8006BA78;
  v0 = 2; // 0x0002
  sb(s0 + 0x0049, v0);
  v0 = 255; // 0x00FF
  sb(s0 + 0x0046, v0);
  goto label8006BAD8;
label8006BA64:
  v1 = lbu(s0 + 0x004A);
  v0 = v1 < 4;
  temp = v0 == 0;
  v0 = v1 + 1; // 0x0001
  if (temp) goto label8006BA80;
label8006BA78:
  sb(s0 + 0x004A, v0);
  goto label8006BAD8;
label8006BA80:
  v0 = lbu(s0 + 0x0049);
  temp = v0 == 0;
  if (temp) goto label8006BAA4;
  v0 = lw(0x800751C0); // &0x00000000
  temp = v0;
  a0 = s0;
  switch (temp)
  {
  default:
    JALR(temp, 0x8006BA9C);
  }
label8006BAA4:
  v0 = lw(s0 + 0x003C);
  v1 = lbu(v0 + 0x0000);
  v0 = 243; // 0x00F3
  temp = v1 == v0;
  v1 = 255; // 0x00FF
  if (temp) goto label8006BAD8;
  v0 = lw(s0 + 0x0030);
  sb(v0 + 0x0000, v1);
  v0 = lw(s0 + 0x0030);
  sb(v0 + 0x0001, 0);
  sb(s0 + 0x00E8, 0);
label8006BAD8:
  ra = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x000000F4
void function_8006B1D8(void)
{
  uint32_t temp;
  sp -= 40; // 0xFFFFFFD8
  a1 = a0;
  sw(sp + 0x0014, s1);
  s1 = 0x80075D18;
  sw(sp + 0x001C, s3);
  s3 = -9; // 0xFFFFFFF7
  sw(sp + 0x0018, s2);
  s2 = 0x80075214; // &0xFFFFFFFF
  sw(sp + 0x0020, ra);
  sw(sp + 0x0010, s0);
label8006B208:
  v1 = lw(0x800751FC); // &0x00000000
  v0 = v1 << 4;
  v0 -= v1;
  v0 = v0 << 4;
  temp = a1 == s3;
  s0 = v0 + s1;
  if (temp) goto label8006B24C;
  temp = a1 != 0;
  v0 = v1 << 2;
  if (temp) goto label8006B23C;
  v0 += s2;
  sw(v0 + 0x0000, 0);
  goto label8006B24C;
label8006B23C:
  a0 = s0;
  function_8006BA0C();
  a0 = s0;
  function_8006B2CC();
label8006B24C:
  v0 = lw(JOY_BASE2_ptr);
  v1 = lw(0x800751FC); // &0x00000000
  sw(0x80075200, 0); // &0x00000000
  sh(v0 + 0x000A, 0);
  v0 = lw(0x80075210); // &0x00000001
  v1++;
  sw(0x800751FC, v1); // &0x00000000
  v0 = (int32_t)v0 < (int32_t)v1;
  temp = v0 != 0;
  v0 = 1; // 0x0001
  if (temp) goto label8006B2A4;
  a0 = v1 << 4;
  a0 -= v1;
  a0 = a0 << 4;
  a0 += s1;
  function_8006992C();
  a1 = 0 | 0xFFFF;
  goto label8006B2A8;
label8006B2A4:
  a1 = 0 | 0xFFFF;
label8006B2A8:
  temp = v0 == 0;
  if (temp) goto label8006B208;
  ra = lw(sp + 0x0020);
  s3 = lw(sp + 0x001C);
  s2 = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 40; // 0x0028
  return;
}

// size: 0x00000084
void function_8006A0EC(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, ra);
  v1 = lbu(a0 + 0x0046);
  v0 = 3; // 0x0003
  temp = v1 == v0;
  v0 = (int32_t)v1 < 4;
  if (temp) goto label8006A140;
  temp = v0 == 0;
  v0 = 2; // 0x0002
  if (temp) goto label8006A11C;
  temp = v1 == v0;
  if (temp) goto label8006A130;
  goto label8006A160;
label8006A11C:
  v0 = 4; // 0x0004
  temp = v1 == v0;
  if (temp) goto label8006A154;
  goto label8006A160;
label8006A130:
  function_8006A99C();
  goto label8006A160;
label8006A140:
  a1 = lbu(a0 + 0x00E4);
  function_8006A9B0();
  goto label8006A160;
label8006A154:
  a1 = lbu(a0 + 0x0047);
  function_8006A9F0();
label8006A160:
  ra = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x000000D4
void function_8006B6D4(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, ra);
  v0 = lw(a0 + 0x003C);
  v1 = lbu(v0 + 0x0000);
  v0 = 243; // 0x00F3
  temp = v1 != v0;
  if (temp) goto label8006B704;
  v0 = lbu(a0 + 0x00E8);
  temp = v0 == 0;
  if (temp) goto label8006B760;
label8006B704:
  v1 = lbu(a0 + 0x0046);
  v0 = 1; // 0x0001
  temp = v1 == v0;
  if (temp) goto label8006B750;
  v0 = (int32_t)v1 < 2;
  temp = v0 == 0;
  if (temp) goto label8006B730;
  temp = v1 == 0;
  if (temp) goto label8006B798;
  goto label8006B770;
label8006B730:
  v0 = 254; // 0x00FE
  temp = v1 == v0;
  if (temp) goto label8006B760;
  v0 = 255; // 0x00FF
  temp = v1 == v0;
  if (temp) goto label8006B798;
  goto label8006B770;
label8006B750:
  a1 = 1; // 0x0001
  function_8006A97C();
  goto label8006B798;
label8006B760:
  a1 = 0;
  function_8006A97C();
  goto label8006B798;
label8006B770:
  v0 = lw(a0 + 0x0014);
  temp = v0 == 0;
  if (temp) goto label8006B790;
  temp = v0;
  switch (temp)
  {
  default:
    JALR(temp, 0x8006B780);
  }
  goto label8006B798;
label8006B790:
  function_8006A0EC();
label8006B798:
  ra = lw(sp + 0x0010);
  v0 = 0;
  sp += 24; // 0x0018
  return;
}

// size: 0x000001D8
void function_80069C08(void)
{
  uint32_t temp;
  sp -= 32; // 0xFFFFFFE0
  sw(sp + 0x0010, s0);
  s0 = a0;
  sw(sp + 0x0018, s2);
  s2 = a1;
  sw(sp + 0x001C, ra);
  temp = (int32_t)s2 >= 0;
  sw(sp + 0x0014, s1);
  if (temp) goto label80069CAC;
  v0 = lw(JOY_BASE_ptr);
  v1 = lw(s0 + 0x0040);
  a0 = lbu(v0 + 0x0000);
  v0 = 255; // 0x00FF
  sb(s0 + 0x0044, v0);
  v0 = 1; // 0x0001
  sb(s0 + 0x0045, v0);
  v0 = ~s2;
  sb(v1 + 0x0000, v0);
  v1 = lw(JOY_BASE_ptr);
  v0 = lhu(v1 + 0x0004);
  v0 = v0 & 0x1;
  temp = v0 != 0;
  s1 = a0 & 0xFF;
  if (temp) goto label80069C84;
label80069C70:
  v0 = lhu(v1 + 0x0004);
  v0 = v0 & 0x1;
  temp = v0 == 0;
  if (temp) goto label80069C70;
label80069C84:
  v0 = get_timer();
  temp = v0 == 0;
  v0 = ~s2;
  if (temp) goto label80069C84;
  v1 = lw(JOY_BASE_ptr);
  sb(v1 + 0x0000, v0);
  v0 = s1;
  goto label80069DC8;
label80069CAC:
  v0 = lw(s0 + 0x003C);
  v0 = lbu(v0 + 0x0000);
  v1 = 8; // 0x0008
  v0 = (int32_t)v0 >> 4;
  temp = v0 != v1;
  a2 = 136; // 0x0088
  if (temp) goto label80069CE0;
  v0 = lbu(s0 + 0x0044);
  v0 = v0 < 9;
  temp = v0 != 0;
  v0 = 0x1F800000;
  if (temp) goto label80069CE4;
  a2 = 34; // 0x0022
label80069CE0:
  v0 = 0x1F800000;
label80069CE4:
  v0 = v0 | 0x1120;
  a1 = lw(JOY_BASE_ptr);
  a0 = lhu(v0 + 0x0000);
  v1 = lhu(a1 + 0x0004);
  v0 = 430; // 0x01AE
  sw(timeout1_duration, v0);
  sw(timeout1_start, a0);
  v1 = v1 & 0x2;
  temp = v1 != 0;
  v1 = a1;
  if (temp) goto label80069D2C;
label80069D18:
  v0 = lhu(v1 + 0x0004);
  v0 = v0 & 0x2;
  temp = v0 == 0;
  if (temp) goto label80069D18;
label80069D2C:
  v0 = lw(JOY_BASE_ptr);
  v1 = lw(I_STAT2_ptr);
  a0 = lbu(v0 + 0x0000);
  sh(v0 + 0x000E, a2);
  v0 = lw(v1 + 0x0000);
  v0 = v0 & 0x80;
  temp = v0 != 0;
  s1 = a0 & 0xFF;
  if (temp) goto label80069D88;
label80069D58:
  v0 = get_timer();
  temp = v0 != 0;
  v0 = -20; // 0xFFFFFFEC
  if (temp) goto label80069DC8;
  v0 = lw(I_STAT2_ptr);
  v0 = lw(v0 + 0x0000);
  v0 = v0 & 0x80;
  temp = v0 == 0;
  if (temp) goto label80069D58;
label80069D88:
  v0 = lw(JOY_BASE_ptr);
  sb(v0 + 0x0000, s2);
  v0 = lbu(s0 + 0x0045);
  a0 = lbu(s0 + 0x0044);
  v1 = lw(s0 + 0x003C);
  v0++;
  v1 += a0;
  sb(s0 + 0x0045, v0);
  sb(v1 + 0x0000, s1);
  v0 = lbu(s0 + 0x0044);
  v0++;
  sb(s0 + 0x0044, v0);
  v0 = s1;
label80069DC8:
  ra = lw(sp + 0x001C);
  s2 = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 32; // 0x0020
  return;
}

// size: 0x00000040
void function_8006AA30(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, s0);
  v0 = lw(0x800751D4); // &0x00000000
  sw(sp + 0x0014, ra);
  temp = v0;
  s0 = a0;
  switch (temp)
  {
  case 0x8006B6D4:
    function_8006B6D4();
    break;
  default:
    JALR(temp, 0x8006AA44);
  }
  a0 = s0;
  sw(0x8007523C, v0); // &0x00000000
  a1 = -2; // 0xFFFFFFFE
  function_80069C08();
  ra = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x00000234
void function_80069DE0(void)
{
  uint32_t temp;
  sp -= 40; // 0xFFFFFFD8
  sw(sp + 0x0010, s0);
  s0 = a0;
  sw(sp + 0x001C, s3);
  s3 = a1;
  sw(sp + 0x0020, ra);
  sw(sp + 0x0018, s2);
  sw(sp + 0x0014, s1);
  v0 = lw(s0 + 0x003C);
  v0 = lbu(v0 + 0x0000);
  v1 = 8; // 0x0008
  v0 = (int32_t)v0 >> 4;
  temp = v0 != v1;
  s1 = 136; // 0x0088
  if (temp) goto label80069E34;
  v0 = lbu(s0 + 0x0044);
  v0 = v0 < 9;
  temp = v0 != 0;
  if (temp) goto label80069E34;
  s1 = 34; // 0x0022
label80069E34:
  v1 = lw(JOY_BASE_ptr);
label80069E40:
  v0 = lhu(v1 + 0x0004);
  v0 = v0 & 0x2;
  temp = v0 == 0;
  if (temp) goto label80069E40;
  a0 = 400; // 0x0190
  set_timer(a0);
  a0 = lw(JOY_BASE_ptr);
  v0 = lbu(a0 + 0x0000);
  v1 = lbu(s0 + 0x0044);
  temp = v1 != 0;
  s2 = v0 & 0xFF;
  if (temp) goto label80069E8C;
  v1 = (int32_t)s2 >> 4;
  v0 = 8; // 0x0008
  temp = v1 == v0;
  v0 = 34; // 0x0022
  if (temp) goto label80069E98;
label80069E8C:
  sh(a0 + 0x000E, s1);
  goto label80069E9C;
label80069E98:
  sh(a0 + 0x000E, v0);
label80069E9C:
  v1 = lw(I_STAT2_ptr);
  v0 = lw(v1 + 0x0000);
  v0 = v0 & 0x80;
  temp = v0 != 0;
  if (temp) goto label80069F6C;
  t1 = TIMER2;
  a2 = TIMER2_TARGET;
  t2 = 0x00010000;
  t0 = TIMER2_MODE;
  a3 = v1;
  a0 = lw(timeout1_start);
  a1 = lw(timeout1_duration);
label80069EEC:
  v0 = lhu(t1 + 0x0000);
  v1 = v0 & 0xFFFF;
  v0 = v1 < a0;
  temp = v0 == 0;
  if (temp) goto label80069F24;
  v0 = lhu(a2 + 0x0000);
  temp = v0 == 0;
  if (temp) goto label80069F20;
  v0 = lhu(a2 + 0x0000);
  v1 += v0;
  goto label80069F24;
label80069F20:
  v1 += t2;
label80069F24:
  v0 = lhu(t0 + 0x0000);
  v0 = v0 & 0x200;
  temp = v0 == 0;
  v0 = v1 - a0;
  if (temp) goto label80069F44;
  v0 = v0 < a1;
  temp = v0 == 0;
  v0 = -2; // 0xFFFFFFFE
  if (temp) goto label80069FF8;
label80069F44:
  v0 = v1 - a0;
  v0 = v0 >> 3;
  v0 = v0 < a1;
  temp = v0 == 0;
  v0 = -2; // 0xFFFFFFFE
  if (temp) goto label80069FF8;
  v0 = lw(a3 + 0x0000);
  v0 = v0 & 0x80;
  temp = v0 == 0;
  if (temp) goto label80069EEC;
label80069F6C:
  v1 = lbu(s0 + 0x00E8);
  v0 = 8; // 0x0008
  temp = v1 == v0;
  v0 = 2; // 0x0002
  if (temp) goto label80069FA8;
  v1 = lw(0x80075200); // &0x00000000
  temp = v1 != v0;
  if (temp) goto label80069FA8;
  a0 = 60; // 0x003C
  set_timer(a0);
label80069F98:
  v0 = get_timer();
  temp = v0 == 0;
  if (temp) goto label80069F98;
label80069FA8:
  v0 = lw(JOY_BASE_ptr);
  sb(v0 + 0x0000, s3);
  v0 = lbu(s0 + 0x0045);
  v1 = lbu(s0 + 0x0044);
  v0++;
  sb(s0 + 0x0045, v0);
  v0 = 255; // 0x00FF
  temp = v1 == v0;
  if (temp) goto label80069FE8;
  v1 = lbu(s0 + 0x0044);
  v0 = lw(s0 + 0x003C);
  v0 += v1;
  sb(v0 + 0x0000, s2);
label80069FE8:
  v1 = lbu(s0 + 0x0044);
  v0 = s2;
  v1++;
  sb(s0 + 0x0044, v1);
label80069FF8:
  ra = lw(sp + 0x0020);
  s3 = lw(sp + 0x001C);
  s2 = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 40; // 0x0028
  return;
}

// size: 0x000000D8
void function_8006AA70(void)
{
  uint32_t temp;
  v1 = lw(0x800751FC); // &0x00000000
  v0 = lw(0x8007520C); // &0x00000000
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, s0);
  s0 = a0;
  temp = v1 != v0;
  sw(sp + 0x0014, ra);
  if (temp) goto label8006AAD0;
  v0 = lw(0x800751F8); // &0x00000000
  temp = v0 == 0;
  if (temp) goto label8006AAD0;
  v0 = lw(0x800751EC); // &0x00000000
  temp = v0;
  switch (temp)
  {
  default:
    JALR(temp, 0x8006AAB4);
  }
  v0 = lw(0x800751E8); // &0x00000000
  temp = v0;
  switch (temp)
  {
  default:
    JALR(temp, 0x8006AAC8);
  }
label8006AAD0:
  v0 = lw(0x8007523C); // &0x00000000
  temp = v0 == 0;
  if (temp) goto label8006AB14;
  a0 = lw(s0 + 0x000C);
  v0 = lw(0x800751D4); // &0x00000000
  temp = v0;
  switch (temp)
  {
  case 0x8006B6D4:
    function_8006B6D4();
    break;
  default:
    JALR(temp, 0x8006AAF4);
  }
  a0 = lw(s0 + 0x000C);
  v0 = lw(0x800751D4); // &0x00000000
  temp = v0;
  a0 += 240; // 0x00F0
  switch (temp)
  {
  case 0x8006B6D4:
    function_8006B6D4();
    break;
  default:
    JALR(temp, 0x8006AB0C);
  }
label8006AB14:
  v0 = lbu(s0 + 0x0036);
  temp = v0 != 0;
  a0 = s0;
  if (temp) goto label8006AB2C;
  a1 = 66; // 0x0042
  goto label8006AB30;
label8006AB2C:
  a1 = lbu(s0 + 0x0036);
label8006AB30:
  function_80069DE0();
  ra = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x000000C0
void function_8006AB48(void)
{
  uint32_t temp;
  v0 = lw(0x8007523C); // &0x00000000
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, s0);
  s0 = a0;
  temp = v0 == 0;
  sw(sp + 0x0014, ra);
  if (temp) goto label8006AB94;
  a0 = lw(s0 + 0x000C);
  v0 = lw(0x800751D4); // &0x00000000
  temp = v0;
  a0 += 480; // 0x01E0
  switch (temp)
  {
  case 0x8006B6D4:
    function_8006B6D4();
    break;
  default:
    JALR(temp, 0x8006AB74);
  }
  a0 = lw(s0 + 0x000C);
  v0 = lw(0x800751D4); // &0x00000000
  temp = v0;
  a0 += 720; // 0x02D0
  switch (temp)
  {
  case 0x8006B6D4:
    function_8006B6D4();
    break;
  default:
    JALR(temp, 0x8006AB8C);
  }
label8006AB94:
  v0 = lbu(s0 + 0x0036);
  temp = v0 != 0;
  a1 = 0;
  if (temp) goto label8006ABAC;
  a1 = lw(0x80075208); // &0x00000000
label8006ABAC:
  a0 = s0;
  function_80069DE0();
  v1 = v0;
  temp = (int32_t)v1 < 0;
  if (temp) goto label8006ABF8;
  v0 = v1 & 0xF0;
  temp = v0 != 0;
  v0 = v1 & 0xF;
  if (temp) goto label8006ABD4;
  v0 = -9; // 0xFFFFFFF7
  goto label8006ABF8;
label8006ABD4:
  v0 = v0 << 1;
  sw(0x80075234, v0); // &0x00000000
  temp = v0 != 0;
  v0 = 0;
  if (temp) goto label8006ABF8;
  v0 = 32; // 0x0020
  sw(0x80075234, v0); // &0x00000000
  v0 = 0;
label8006ABF8:
  ra = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x000000BC
void function_8006B2DC(void)
{
  uint32_t temp;
  v0 = lbu(a0 + 0x0045);
  a1 = lbu(a0 + 0x0036);
  temp = a1 == 0;
  v1 = v0 - 3; // 0xFFFFFFFD
  if (temp) goto label8006B304;
  v0 = 77; // 0x004D
  temp = a1 == v0;
  if (temp) goto label8006B34C;
  goto label8006B36C;
label8006B304:
  v0 = (int32_t)v1 < 6;
  temp = v0 == 0;
  v0 = a0 + v1;
  if (temp) goto label8006B320;
  v0 = lbu(v0 + 0x0057);
  temp = v0 == 0;
  v0 = 0;
  if (temp) goto label8006B390;
label8006B320:
  v0 = lbu(a0 + 0x0034);
  v0 = (int32_t)v1 < (int32_t)v0;
  temp = v0 == 0;
  v0 = 0;
  if (temp) goto label8006B390;
  v0 = lw(a0 + 0x0028);
  v0 += v1;
label8006B340:
  v0 = lbu(v0 + 0x0000);
  goto label8006B390;
label8006B34C:
  v0 = lbu(a0 + 0x0035);
  v0 = (int32_t)v1 < (int32_t)v0;
  temp = v0 == 0;
  v0 = 255; // 0x00FF
  if (temp) goto label8006B390;
  v0 = lw(a0 + 0x002C);
  v0 += v1;
  goto label8006B340;
label8006B36C:
  v0 = lbu(a0 + 0x0035);
  v0 = (int32_t)v1 < (int32_t)v0;
  temp = v0 == 0;
  v0 = 0;
  if (temp) goto label8006B390;
  v0 = lw(a0 + 0x002C);
  v0 += v1;
  v0 = lbu(v0 + 0x0000);
label8006B390:
  return;
}

// size: 0x0000008C
void function_8006AC08(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, s0);
  s0 = a0;
  sw(sp + 0x0014, ra);
  v0 = lw(s0 + 0x003C);
  v0 = lbu(v0 + 0x0000);
  v1 = 8; // 0x0008
  v0 = v0 >> 4;
  temp = v0 != v1;
  a1 = 0;
  if (temp) goto label8006AC40;
  v0 = lbu(s0 + 0x0036);
  a1 = v0 < 1;
label8006AC40:
  v0 = lw(0x800751C4); // &0x00000000
  temp = v0;
  a0 = s0;
  switch (temp)
  {
  case 0x8006B2DC:
    function_8006B2DC();
    break;
  default:
    JALR(temp, 0x8006AC4C);
  }
  a0 = s0;
  a1 = v0 & 0xFF;
  function_80069DE0();
  v1 = v0;
  v0 = 90; // 0x005A
  temp = v1 == v0;
  v0 = v1;
  if (temp) goto label8006AC84;
  temp = v1 == 0;
  if (temp) goto label8006AC84;
  temp = (int32_t)v1 >= 0;
  v0 = -9; // 0xFFFFFFF7
  if (temp) goto label8006AC84;
  v0 = v1;
label8006AC84:
  ra = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x0000027C
void function_8006B398(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, s0);
  s0 = a0;
  a0 = s0 + 87; // 0x0057
  sw(sp + 0x0014, ra);
  a1 = 6; // 0x0006
  v0 = spyro_memclr8(a0, a1);
  v0 = lhu(s0 + 0x00E6);
  temp = v0 == 0;
  if (temp) goto label8006B510;
  v0 = lw(s0 + 0x0028);
  temp = v0 == 0;
  if (temp) goto label8006B510;
  v0 = lbu(s0 + 0x0034);
  v0 = v0 < 7;
  temp = v0 == 0;
  t1 = 6; // 0x0006
  if (temp) goto label8006B3EC;
  t1 = lbu(s0 + 0x0034);
label8006B3EC:
  v0 = lbu(s0 + 0x00E9);
  temp = v0 == 0;
  t0 = 0;
  if (temp) goto label8006B604;
  t3 = 1; // 0x0001
  t2 = 0;
label8006B404:
  v0 = lw(s0 + 0x0004);
  a2 = 0;
  v0 += t2;
  v0 = lbu(v0 + 0x0002);
  temp = v0 == 0;
  a3 = 1; // 0x0001
  if (temp) goto label8006B424;
  a3 = 255; // 0x00FF
label8006B424:
  a1 = s0 + 93; // 0x005D
  a0 = lw(s0 + 0x0028);
  temp = t1 == 0;
  v1 = 0;
  if (temp) goto label8006B46C;
label8006B434:
  v0 = lbu(a1 + 0x0000);
  temp = v0 != t0;
  if (temp) goto label8006B458;
  v0 = lbu(a0 + 0x0000);
  v0 = v0 & a3;
  temp = v0 != 0;
  if (temp) goto label8006B4AC;
label8006B458:
  a1++;
  v1++;
  v0 = (int32_t)v1 < (int32_t)t1;
  temp = v0 != 0;
  a0++;
  if (temp) goto label8006B434;
label8006B46C:
  temp = a2 == 0;
  if (temp) goto label8006B4F4;
  v0 = lw(s0 + 0x0004);
  v0 += t2;
  v1 = lbu(v0 + 0x0003);
  v0 = lw(0x80075204); // &0x00000000
  v1 += v0;
  v0 = (int32_t)v1 < 61;
  temp = v0 == 0;
  if (temp) goto label8006B4B4;
  at = 0x8006FCF4 + 0x030C;
  sw(at + 0x5204, v1);
  goto label8006B4B8;
label8006B4AC:
  a2 = 1; // 0x0001
  goto label8006B46C;
label8006B4B4:
  a2 = 0;
label8006B4B8:
  temp = a2 == 0;
  if (temp) goto label8006B4F4;
  a1 = s0 + 93; // 0x005D
  a0 = s0 + 87; // 0x0057
  temp = t1 == 0;
  v1 = 0;
  if (temp) goto label8006B4F4;
label8006B4D0:
  v0 = lbu(a1 + 0x0000);
  temp = v0 != t0;
  a1++;
  if (temp) goto label8006B4E4;
  sb(a0 + 0x0000, t3);
label8006B4E4:
  v1++;
  v0 = (int32_t)v1 < (int32_t)t1;
  temp = v0 != 0;
  a0++;
  if (temp) goto label8006B4D0;
label8006B4F4:
  v0 = lbu(s0 + 0x00E9);
  t0++;
  v0 = (int32_t)t0 < (int32_t)v0;
  temp = v0 != 0;
  t2 += 5; // 0x0005
  if (temp) goto label8006B404;
  goto label8006B604;
label8006B510:
  v1 = lbu(s0 + 0x00E8);
  v0 = v1 - 4; // 0xFFFFFFFC
  v0 = v0 < 2;
  temp = v0 != 0;
  v0 = 7; // 0x0007
  if (temp) goto label8006B530;
  temp = v1 != v0;
  if (temp) goto label8006B5C4;
label8006B530:
  v0 = lhu(s0 + 0x00E6);
  temp = v0 != 0;
  if (temp) goto label8006B5C4;
  v0 = lbu(s0 + 0x0034);
  v0 = v0 < 2;
  temp = v0 != 0;
  v1 = 64; // 0x0040
  if (temp) goto label8006B5C4;
  a0 = lw(s0 + 0x0028);
  v0 = lbu(a0 + 0x0000);
  v0 = v0 & 0xC0;
  temp = v0 != v1;
  if (temp) goto label8006B604;
  v0 = lbu(a0 + 0x0001);
  v0 = v0 & 0x1;
  temp = v0 == 0;
  if (temp) goto label8006B604;
  v0 = lw(0x80075204); // &0x00000000
  v0 += 10; // 0x000A
  v0 = (int32_t)v0 < 61;
  temp = v0 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label8006B604;
  sb(s0 + 0x0058, v0);
  sb(s0 + 0x0057, v0);
  v0 = lw(0x80075204); // &0x00000000
  v0 += 10; // 0x000A
  at = 0x8006FCF4 + 0x030C;
  sw(at + 0x5204, v0);
  goto label8006B604;
label8006B5C4:
  v1 = lbu(s0 + 0x00E8);
  v0 = 3; // 0x0003
  temp = v1 != v0;
  v0 = 1; // 0x0001
  if (temp) goto label8006B5DC;
  sb(s0 + 0x0057, v0);
  goto label8006B604;
label8006B5DC:
  v0 = lhu(s0 + 0x00E6);
  temp = v0 != 0;
  v0 = 1; // 0x0001
  if (temp) goto label8006B604;
  v1 = 5; // 0x0005
  a0 = s0 + 5; // 0x0005
label8006B5F4:
  sb(a0 + 0x0057, v0);
  v1--;
  temp = (int32_t)v1 >= 0;
  a0--;
  if (temp) goto label8006B5F4;
label8006B604:
  ra = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x0000037C
void function_8006AC94(void)
{
  uint32_t temp;
  sp -= 48; // 0xFFFFFFD0
  sw(sp + 0x0014, s1);
  s1 = a0;
  v0 = lw(0x800751C8); // &0x00000000
  sw(sp + 0x0028, ra);
  sw(sp + 0x0024, s5);
  sw(sp + 0x0020, s4);
  sw(sp + 0x001C, s3);
  sw(sp + 0x0018, s2);
  temp = v0;
  sw(sp + 0x0010, s0);
  switch (temp)
  {
  case 0x8006B398:
    function_8006B398();
    break;
  default:
    JALR(temp, 0x8006ACBC);
  }
  v0 = lw(0x80075208); // &0x00000000
  temp = v0 == 0;
  s4 = 0;
  if (temp) goto label8006AD00;
  v0 = lw(s1 + 0x003C);
  v0 = lbu(v0 + 0x0000);
  v1 = 8; // 0x0008
  v0 = (int32_t)v0 >> 4;
  temp = v0 != v1;
  if (temp) goto label8006AD00;
  v0 = lbu(s1 + 0x0036);
  s4 = v0 < 1;
label8006AD00:
  temp = s4 == 0;
  if (temp) goto label8006AD9C;
  s0 = -1; // 0xFFFFFFFF
  s2 = -240; // 0xFFFFFF10
label8006AD10:
  v0 = lw(0x80075234); // &0x00000000
  v0--;
  at = 0x8006FCF4 + 0x030C;
  temp = (int32_t)v0 <= 0;
  sw(at + 0x5234, v0);
  if (temp) goto label8006AD9C;
  temp = (int32_t)s0 < 0;
  if (temp) goto label8006AD4C;
  a0 = lw(s1 + 0x000C);
  v0 = lw(0x800751C8); // &0x00000000
  temp = v0;
  a0 += s2;
  switch (temp)
  {
  case 0x8006B398:
    function_8006B398();
    break;
  default:
    JALR(temp, 0x8006AD44);
  }
label8006AD4C:
  a0 = s1;
  v0 = lw(0x800751C4); // &0x00000000
  temp = v0;
  a1 = 1; // 0x0001
  switch (temp)
  {
  case 0x8006B2DC:
    function_8006B2DC();
    break;
  default:
    JALR(temp, 0x8006AD5C);
  }
  a0 = s1;
  a1 = v0 & 0xFF;
  function_80069DE0();
  temp = (int32_t)v0 < 0;
  if (temp) goto label8006AFEC;
  a0 = 60; // 0x003C
  set_timer(a0);
  function_8006A014();
  temp = v0 == 0;
  s0++;
  if (temp) goto label8006AF10;
  v0 = (int32_t)s0 < 4;
  temp = v0 != 0;
  s2 += 240; // 0x00F0
  if (temp) goto label8006AD10;
label8006AD9C:
  v1 = lw(0x800751FC); // &0x00000000
  v0 = lw(0x80075234); // &0x00000000
  v0 = (int32_t)v0 < 2;
  temp = v0 != 0;
  a0 = v1 < 1;
  if (temp) goto label8006AF88;
  v1 = 0x80075214; // &0xFFFFFFFF
  v0 = a0 << 2;
  s0 = v0 + v1;
  v0 = a0 << 4;
  v0 -= a0;
  s2 = v0 << 4;
  s5 = 3; // 0x0003
label8006ADDC:
  a0 = lw(s0 + 0x0000);
  temp = (int32_t)a0 < 0;
  if (temp) goto label8006AF88;
  temp = (int32_t)a0 <= 0;
  v0 = a0 << 4;
  if (temp) goto label8006AE28;
  v1 = lw(0x800751F0); // &0x00000000
  v0 -= a0;
  v1 += s2;
  v1 = lw(v1 + 0x000C);
  v0 = v0 << 4;
  v1 += v0;
  s3 = v1 - 240; // 0xFFFFFF10
  v0 = lw(0x800751DC); // &0x00000000
  temp = v0;
  a0 = s3;
  switch (temp)
  {
  case 0x8006B7A8:
    function_8006B7A8();
    break;
  default:
    JALR(temp, 0x8006AE20);
  }
label8006AE28:
  v1 = lw(s0 + 0x0000);
  temp = v1 == s5;
  if (temp) goto label8006AE70;
  v0 = (int32_t)v1 < 4;
  temp = v0 == 0;
  v0 = (int32_t)v1 < 2;
  if (temp) goto label8006AE5C;
  temp = v0 == 0;
  a0 = s1;
  if (temp) goto label8006AED0;
  temp = (int32_t)v1 < 0;
  if (temp) goto label8006AED0;
  goto label8006AE8C;
label8006AE5C:
  v0 = 4; // 0x0004
  temp = v1 != v0;
  a0 = s1;
  if (temp) goto label8006AED0;
  sw(s0 + 0x0000, s5);
  goto label8006AED0;
label8006AE70:
  v0 = lw(0x800751DC); // &0x00000000
  temp = v0;
  a0 = s3 - 240; // 0xFFFFFF10
  switch (temp)
  {
  case 0x8006B7A8:
    function_8006B7A8();
    break;
  default:
    JALR(temp, 0x8006AE7C);
  }
  v0 = 1; // 0x0001
  goto label8006AEC8;
label8006AE8C:
  v0 = lw(0x800751F0); // &0x00000000
  s3 = v0 + s2;
  v0 = lw(0x800751DC); // &0x00000000
  temp = v0;
  a0 = s3;
  switch (temp)
  {
  case 0x8006B7A8:
    function_8006B7A8();
    break;
  default:
    JALR(temp, 0x8006AEA8);
  }
  v0 = lw(0x800751E0); // &0x00000000
  temp = v0;
  a0 = s3;
  switch (temp)
  {
  case 0x8006B2CC:
    function_8006B2CC();
    break;
  default:
    JALR(temp, 0x8006AEBC);
  }
  v0 = -1; // 0xFFFFFFFF
label8006AEC8:
  sw(s0 + 0x0000, v0);
  a0 = s1;
label8006AED0:
  v0 = lw(0x800751C4); // &0x00000000
  temp = v0;
  a1 = s4;
  switch (temp)
  {
  case 0x8006B2DC:
    function_8006B2DC();
    break;
  default:
    JALR(temp, 0x8006AEDC);
  }
  a0 = s1;
  a1 = v0 & 0xFF;
  function_80069C08();
  temp = (int32_t)v0 < 0;
  if (temp) goto label8006AFEC;
  a0 = 60; // 0x003C
  set_timer(a0);
  function_8006A014();
  temp = v0 != 0;
  if (temp) goto label8006AF18;
label8006AF10:
  v0 = -3; // 0xFFFFFFFD
  goto label8006AFEC;
label8006AF18:
  v0 = lw(0x80075234); // &0x00000000
  v0--;
  sw(0x80075234, v0); // &0x00000000
  v0 = (int32_t)v0 < 2;
  temp = v0 == 0;
  if (temp) goto label8006ADDC;
  goto label8006AF88;
label8006AF44:
  a0 = s1;
  v0 = lw(0x800751C4); // &0x00000000
  temp = v0;
  a1 = s4;
  switch (temp)
  {
  case 0x8006B2DC:
    function_8006B2DC();
    break;
  default:
    JALR(temp, 0x8006AF54);
  }
  a0 = s1;
  a1 = v0 & 0xFF;
  function_80069C08();
  temp = (int32_t)v0 < 0;
  if (temp) goto label8006AFEC;
  a0 = 60; // 0x003C
  set_timer(a0);
  function_8006A014();
  temp = v0 == 0;
  v0 = -3; // 0xFFFFFFFD
  if (temp) goto label8006AFEC;
label8006AF88:
  v0 = lw(0x80075234); // &0x00000000
  v0--;
  at = 0x8006FCF4 + 0x030C;
  temp = (int32_t)v0 > 0;
  sw(at + 0x5234, v0);
  if (temp) goto label8006AF44;
  controller_wait_for_data();
  v1 = lbu(s1 + 0x0044);
  v0 = v1 + 1; // 0x0001
  sb(s1 + 0x0044, v0);
  v0 = lw(JOY_BASE3_ptr);
  a0 = lw(s1 + 0x003C);
  v0 = lbu(v0 + 0x0000);
  v1 += a0;
  sb(v1 + 0x0000, v0);
  v0 = lw(0x800751BC); // &0x00000000
  temp = v0;
  a0 = 0;
  switch (temp)
  {
  case 0x8006B1D8:
    function_8006B1D8();
    break;
  default:
    JALR(temp, 0x8006AFE0);
  }
  v0 = 0;
label8006AFEC:
  ra = lw(sp + 0x0028);
  s5 = lw(sp + 0x0024);
  s4 = lw(sp + 0x0020);
  s3 = lw(sp + 0x001C);
  s2 = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 48; // 0x0030
  return;
}

// size: 0x000000C8
void function_80069B40(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  a1 = 0x80075200; // &0x00000000
  sw(sp + 0x0010, ra);
  v1 = lw(a1 + 0x0000);
  v0 = v1 << 2;
  v0 = lw(0x80075240 + v0); // &0x8006AA30
  v1++;
  temp = v0;
  sw(a1 + 0x0000, v1);
  switch (temp)
  {
  case 0x8006AA30:
    function_8006AA30();
    break;
  case 0x8006AA70:
    function_8006AA70();
    break;
  case 0x8006AB48:
    function_8006AB48();
    break;
  case 0x8006AC08:
    function_8006AC08();
    break;
  case 0x8006AC94:
    function_8006AC94();
    break;
  default:
    JALR(temp, 0x80069B6C);
  }
  a0 = v0;
  temp = (int32_t)a0 < 0;
  if (temp) goto label80069BE4;
  v0 = lw(0x80075200); // &0x00000000
  temp = v0 == 0;
  if (temp) goto label80069BC0;
  a0 = 60; // 0x003C
  set_timer(a0);
  function_8006A014();
  temp = v0 != 0;
  if (temp) goto label80069BC0;
  v0 = lw(0x800751BC); // &0x00000000
  temp = v0;
  a0 = -3; // 0xFFFFFFFD
  switch (temp)
  {
  case 0x8006B1D8:
    function_8006B1D8();
    break;
  default:
    JALR(temp, 0x80069BB8);
  }
label80069BC0:
  v1 = lw(0x80075200); // &0x00000000
  v0 = (int32_t)v1 < 5;
  temp = v0 != 0;
  v0 = v1 - 1; // 0xFFFFFFFF
  if (temp) goto label80069BF8;
  at = 0x8006FCF4 + 0x030C;
  sw(at + 0x5200, v0);
  goto label80069BF8;
label80069BE4:
  v0 = lw(0x800751BC); // &0x00000000
  temp = v0;
  switch (temp)
  {
  case 0x8006B1D8:
    function_8006B1D8();
    break;
  default:
    JALR(temp, 0x80069BF0);
  }
label80069BF8:
  ra = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x0000016C
void function_8006969C(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  v1 = lw(0x8007520C); // &0x00000000
  v0 = 1; // 0x0001
  sw(sp + 0x0010, ra);
  at = 0x8006FCF4 + 0x030C;
  temp = v1 == 0;
  sw(at + 0x5224, v0);
  if (temp) goto label800696DC;
  a0 = 0x80075C68;
  v1 = lw(a0 + 0x0000);
  v0 = (int32_t)v1 < 150;
  temp = v0 == 0;
  v0 = v1 + 1; // 0x0001
  if (temp) goto label800696DC;
  sw(a0 + 0x0000, v0);
label800696DC:
  v0 = lw(0x80075210); // &0x00000001
  temp = v0 != 0;
  if (temp) goto label80069710;
  a0 = 0x80075C6C;
  v1 = lw(a0 + 0x0000);
  v0 = (int32_t)v1 < 150;
  temp = v0 == 0;
  v0 = v1 + 1; // 0x0001
  if (temp) goto label80069710;
  sw(a0 + 0x0000, v0);
label80069710:
  v0 = lw(0x800751F4); // &0x00000000
  temp = v0 == 0;
  if (temp) goto label800697F8;
  v1 = lw(0x8007520C); // &0x00000000
  v0 = lw(0x80075210); // &0x00000001
  v0 = (int32_t)v0 < (int32_t)v1;
  temp = v0 != 0;
  a0 = v1 << 4;
  if (temp) goto label800697F8;
  a0 -= v1;
  v0 = lw(0x800751F0); // &0x00000000
  a0 = a0 << 4;
  sw(0x80075200, 0); // &0x00000000
  sw(0x800751FC, v1); // &0x00000000
  a0 += v0;
  function_8006992C();
  temp = v0 != 0;
  if (temp) goto label80069788;
  v0 = lw(0x800751BC); // &0x00000000
  temp = v0;
  a0 = 0 | 0xFFFF;
  switch (temp)
  {
  case 0x8006B1D8:
    function_8006B1D8();
    break;
  default:
    JALR(temp, 0x80069780);
  }
label80069788:
  v1 = lw(0x800751FC); // &0x00000000
  v0 = lw(0x80075210); // &0x00000001
  sw(0x80075204, 0); // &0x00000000
  v0 = (int32_t)v0 < (int32_t)v1;
  temp = v0 != 0;
  if (temp) goto label800697E8;
label800697AC:
  a0 = v1 << 4;
  a0 -= v1;
  v0 = lw(0x800751F0); // &0x00000000
  a0 = a0 << 4;
  a0 += v0;
  function_80069B40();
  v1 = lw(0x800751FC); // &0x00000000
  v0 = lw(0x80075210); // &0x00000001
  v0 = (int32_t)v0 < (int32_t)v1;
  temp = v0 == 0;
  if (temp) goto label800697AC;
label800697E8:
  v1 = lw(JOY_BASE_ptr);
  v0 = 136; // 0x0088
  sh(v1 + 0x000E, v0);
label800697F8:
  ra = lw(sp + 0x0010);
  v0 = 0;
  sp += 24; // 0x0018
  return;
}

// size: 0x00000068
void function_8006A6E4(void)
{
  uint32_t temp;
  sp -= 32; // 0xFFFFFFE0
  sw(sp + 0x0010, s0);
  s0 = a0;
  sw(sp + 0x0014, s1);
  v0 = lw(0x800751D8); // &0x00000000
  sw(sp + 0x0018, ra);
  temp = v0;
  s1 = a1;
  switch (temp)
  {
  default:
    JALR(temp, 0x8006A700);
  }
  temp = v0 != 0;
  v0 = 0;
  if (temp) goto label8006A738;
  v0 = 1; // 0x0001
  v1 = 1; // 0x0001
  sb(s0 + 0x0046, v1);
  v1 = 0x8006A74C; // &0x8C830020
  sw(s0 + 0x0014, v1);
  v1 = 0x8006A768; // &0x908200E9
  sw(s0 + 0x0020, s1);
  sw(s0 + 0x0018, v1);
label8006A738:
  ra = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 32; // 0x0020
  return;
}

// size: 0x00000098
void function_8006A830(void)
{
  uint32_t temp;
  sp -= 40; // 0xFFFFFFD8
  sw(sp + 0x0010, s0);
  s0 = a0;
  sw(sp + 0x0014, s1);
  s1 = a1;
  sw(sp + 0x0018, s2);
  s2 = a2;
  sw(sp + 0x001C, s3);
  v0 = lw(0x800751D8); // &0x00000000
  sw(sp + 0x0020, ra);
  temp = v0;
  s3 = s1;
  switch (temp)
  {
  default:
    JALR(temp, 0x8006A85C);
  }
  temp = v0 != 0;
  v0 = 0;
  if (temp) goto label8006A8AC;
  v0 = 1; // 0x0001
  a0 = lbu(s0 + 0x00E4);
  v1 = 1; // 0x0001
  sb(s0 + 0x0046, v1);
  v1 = 0x8006A8C8; // &0x90830046
  sw(s0 + 0x0014, v1);
  v1 = 0x8006A91C; // &0x27BDFFE8
  sw(s0 + 0x0018, v1);
  v1 = s3 & 0xFF;
  sb(s0 + 0x0051, s1);
  sb(s0 + 0x0052, s2);
  v1 = v1 ^ a0;
  v1 = v1 < 1;
  sb(s0 + 0x0053, v1);
label8006A8AC:
  ra = lw(sp + 0x0020);
  s3 = lw(sp + 0x001C);
  s2 = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 40; // 0x0028
  return;
}

// size: 0x000000C0
void function_800690EC(void)
{
  uint32_t temp;
  v0 = lw(0x800751D0); // &0x00000000
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, ra);
  temp = v0;
  switch (temp)
  {
  case 0x8006B64C:
    function_8006B64C();
    break;
  default:
    JALR(temp, 0x800690FC);
  }
  a0 = v0;
  v0 = lw(a0 + 0x0034);
  v1 = 0xFFFF0000;
  v0 = v0 & v1;
  temp = v0 != 0;
  if (temp) goto label80069154;
  v0 = lw(a0 + 0x0010);
  temp = a0 == v0;
  if (temp) goto label8006913C;
  v0 = lbu(a0 + 0x0038);
  temp = v0 != 0;
  if (temp) goto label80069154;
label8006913C:
  v0 = lw(a0 + 0x0030);
  v0 = lbu(v0 + 0x0000);
  temp = v0 == 0;
  if (temp) goto label80069198;
label80069154:
  v1 = lbu(a0 + 0x0049);
  v0 = 3; // 0x0003
  temp = v1 == v0;
  v0 = (int32_t)v1 < 4;
  if (temp) goto label80069190;
  temp = v0 == 0;
  v0 = 2; // 0x0002
  if (temp) goto label8006917C;
  temp = v1 == v0;
  v0 = 1; // 0x0001
  if (temp) goto label8006919C;
  goto label80069198;
label8006917C:
  v0 = 6; // 0x0006
  temp = v1 == v0;
  v0 = 4; // 0x0004
  if (temp) goto label8006919C;
  goto label80069198;
label80069190:
  v0 = 1; // 0x0001
  goto label8006919C;
label80069198:
  v0 = lbu(a0 + 0x0049);
label8006919C:
  ra = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x000000F8
void function_800691AC(void)
{
  uint32_t temp;
  v0 = lw(0x800751D0); // &0x00000000
  sp -= 32; // 0xFFFFFFE0
  sw(sp + 0x0010, s0);
  s0 = a1;
  sw(sp + 0x0014, s1);
  sw(sp + 0x0018, ra);
  temp = v0;
  s1 = a2;
  switch (temp)
  {
  case 0x8006B64C:
    function_8006B64C();
    break;
  default:
    JALR(temp, 0x800691C8);
  }
  v1 = v0;
  v0 = 3; // 0x0003
  temp = s0 == v0;
  v0 = (int32_t)s0 < 4;
  if (temp) goto label80069234;
  temp = v0 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label80069200;
  temp = s0 == v0;
  v0 = 2; // 0x0002
  if (temp) goto label8006921C;
  temp = s0 == v0;
  v0 = 0;
  if (temp) goto label80069228;
  goto label80069290;
label80069200:
  v0 = 4; // 0x0004
  temp = s0 == v0;
  v0 = 100; // 0x0064
  if (temp) goto label80069240;
  temp = s0 == v0;
  v0 = 0;
  if (temp) goto label80069280;
  goto label80069290;
label8006921C:
  v0 = lbu(v1 + 0x00E8);
  goto label80069290;
label80069228:
  v0 = lhu(v1 + 0x00E6);
  goto label80069290;
label80069234:
  v0 = lbu(v1 + 0x00E4);
  goto label80069290;
label80069240:
  temp = (int32_t)s1 >= 0;
  if (temp) goto label80069254;
  v0 = lbu(v1 + 0x00E3);
  goto label80069290;
label80069254:
  v0 = lbu(v1 + 0x00E3);
  v0 = (int32_t)s1 < (int32_t)v0;
  temp = v0 == 0;
  v0 = s1 << 1;
  if (temp) goto label8006928C;
  v1 = lw(v1 + 0x0000);
  v0 += v1;
  v0 = lhu(v0 + 0x0000);
  goto label80069290;
label80069280:
  v0 = lw(v1 + 0x004C);
  goto label80069290;
label8006928C:
  v0 = 0;
label80069290:
  ra = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 32; // 0x0020
  return;
}

// size: 0x00000038
void function_80069420(void)
{
  uint32_t temp;
  v0 = lw(0x800751D0); // &0x00000000
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, s0);
  sw(sp + 0x0014, ra);
  temp = v0;
  s0 = a1;
  switch (temp)
  {
  case 0x8006B64C:
    function_8006B64C();
    break;
  default:
    JALR(temp, 0x80069434);
  }
  a0 = v0;
  a1 = s0;
  function_8006A6E4();
  ra = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x00000048
void function_80069458(void)
{
  uint32_t temp;
  v0 = lw(0x800751D0); // &0x00000000
  sp -= 32; // 0xFFFFFFE0
  sw(sp + 0x0010, s0);
  s0 = a1;
  sw(sp + 0x0014, s1);
  sw(sp + 0x0018, ra);
  temp = v0;
  s1 = a2;
  switch (temp)
  {
  case 0x8006B64C:
    function_8006B64C();
    break;
  default:
    JALR(temp, 0x80069474);
  }
  a0 = v0;
  a1 = s0 & 0xFF;
  a2 = s1 & 0xFF;
  function_8006A830();
  ra = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 32; // 0x0020
  return;
}

// size: 0x00000048
void function_800694A0(void)
{
  uint32_t temp;
  v0 = lw(0x800751D0); // &0x00000000
  sp -= 32; // 0xFFFFFFE0
  sw(sp + 0x0010, s0);
  s0 = a1;
  sw(sp + 0x0014, s1);
  sw(sp + 0x0018, ra);
  temp = v0;
  s1 = a2;
  switch (temp)
  {
  case 0x8006B64C:
    function_8006B64C();
    break;
  default:
    JALR(temp, 0x800694BC);
  }
  a0 = v0;
  a1 = s0;
  a2 = s1;
  function_8006A0D0();
  ra = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 32; // 0x0020
  return;
}

