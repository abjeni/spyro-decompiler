#include "main.h"
#include "psx_mem.h"
#include "decompilation.h"
#include "gte.h"
#include "spyro_psy.h"
#include "psx_ops.h"

// size: 0x000000D8
void function_800562A4(void)
{
  t3 = 0;
  for (int i = 0; i < 24; i++)
  {
    if (lw(0x80075F30 + i*0x1C) == a0
      && (a1 == 1 || (a1 == 2
      && lhu(0x80075F30 + i*0x1C + 0x0E) & 0x100))) 
    {
      t3 |= 1 << i;

      v1 = lw(0x80075F30 + i*0x1C + 0x18);
      if (v1)
        sb(v1, 0x7F);
      
      sw(0x80075F30 + i*0x1C + 0x00, 0);
      sb(0x80075F30 + i*0x1C + 0x0D, 0xFF);
      sh(0x80075F30 + i*0x1C + 0x0E, 0x40);
      sw(0x80075F30 + i*0x1C + 0x14, 0);
      sw(0x80075F30 + i*0x1C + 0x18, 0);
    }
  }
  sw(0x8007623C, lw(0x8007623C) | t3);
}

// size: 0x00000310
void function_800530C0(void)
{
  uint32_t temp;
  t7 = a0;
  cop2.RBK = a1;
  at = lw(t7 + 0x0020);
  v0 = lw(t7 + 0x0024);
  v1 = lw(t7 + 0x0028);
  a0 = lw(t7 + 0x002C);
  a1 = lw(t7 + 0x0030);
  cop2.RTM0 = at;
  cop2.RTM1 = v0;
  cop2.RTM2 = v1;
  cop2.RTM3 = a0;
  cop2.RTM4 = a1;
  at = lw(t7 + 0x000C);
  v0 = lw(t7 + 0x0010);
  v1 = lw(t7 + 0x0014);
  at = at >> 2;
  v0 = v0 >> 2;
  v1 = v1 >> 2;
  cop2.TRX = at;
  cop2.TRY = v0;
  cop2.TRZ = v1;
  at = lhu(t7 + 0x0036);
  v1 = lw(t7 + 0x003C);
  v0 = 0x80076378;
  at = at << 2;
  at += v0;
  at = lw(at + 0x0000);
  v0 = v1 << 2;
  v0 = v0 & 0x3FC;
  v0 += at;
  v0 = lw(v0 + 0x0038);
  v1 = v1 >> 13;
  v1 = v1 & 0x7F8;
  v1 += 36; // 0x0024
  v1 += v0;
  at = lw(v1 + 0x0000);
  v1 = lw(v1 + 0x0004);
  a0 = lbu(v0 + 0x0008);
  a1 = lbu(v0 + 0x0005);
  a2 = lbu(v0 + 0x0006);
  t8 = lw(v0 + 0x0014);
  t9 = lw(v0 + 0x0018);
  at = at << 11;
  at = at >> 11;
  v1 = v1 >> 24;
  v1 = v1 << 2;
  v0 = at + v1;
  v1 = 0x1F800000;
  t3 = 1; // 0x0001
  a0 = a0 >> 1;
  a0 = a0 << 3;
  a0 += v1;
  t2 = 0;
  a2++;
  a2 += a1;
label800531A0:
  temp = t2 != 0;
  if (temp) goto label800531D8;
  t1 = lw(at + 0x0000);
  at += 4; // 0x0004
  t2 = t1 & 0x1;
  a3 = (int32_t)t1 >> 21;
  t0 = t1 << 10;
  t0 = (int32_t)t0 >> 21;
  t1 = t1 << 20;
  t1 = (int32_t)t1 >> 19;
  a3 = a3 << a1;
  t0 = t0 << a1;
  t1 = t1 << a1;
  goto label80053210;
label800531D8:
  t4 = lh(v0 + 0x0000);
  v0 += 2; // 0x0002
  t5 = (int32_t)t4 >> 11;
  t5 = t5 << a2;
  a3 += t5;
  t5 = t4 << 21;
  t5 = (int32_t)t5 >> 27;
  t5 = t5 << a2;
  t0 -= t5;
  t5 = t4 << 26;
  t5 = (int32_t)t5 >> 27;
  t5 = t5 << a2;
  t1 -= t5;
  t2 = t4 & 0x1;
label80053210:
  temp = (int32_t)t3 < 0;
  t3 = -t3;
  if (temp) goto label800531A0;
  cop2.IR3 = a3;
  cop2.IR1 = t0;
  cop2.IR2 = t1;
  v1 += 8; // 0x0008
  MVMVA(SF_ON, MX_RT, V_IR, CV_NONE, LM_OFF);
  t4 = cop2.MAC3;
  t5 = cop2.MAC1;
  t6 = cop2.MAC2;
  t5 = -t5;
  t6 = -t6;
  sh(v1 - 0x0008, t4); // 0xFFFFFFF8
  sh(v1 - 0x0006, t5); // 0xFFFFFFFA
  temp = v1 != a0;
  sh(v1 - 0x0004, t6); // 0xFFFFFFFC
  if (temp) goto label800531A0;
  t8 = 0x1F800000;
  t9 = a0 - 8; // 0xFFFFFFF8
label80053260:
  v0 = spyro_rand();
  t6 = v0;
  a0 = 2; // 0x0002
  function_80053570();
  v1 = lh(t8 + 0x0000);
  a0 = lh(t8 + 0x0002);
  a1 = lh(t8 + 0x0004);
  v1 = (int32_t)v1 >> 2;
  a0 = (int32_t)a0 >> 2;
  a1 = (int32_t)a1 >> 2;
  a2 = cop2.TRX;
  a3 = cop2.TRY;
  t0 = cop2.TRZ;
  v1 += a2;
  a0 += a3;
  a1 += t0;
  sh(v0 + 0x0004, v1);
  sh(v0 + 0x0006, a0);
  at = t6 & 0x3;
  temp = at == 0;
  sh(v0 + 0x0008, a1);
  if (temp) goto label80053350;
  v1 = cop2.RBK;
  at = t6 & 0x3;
  at += v1;
  sb(v0 + 0x000A, at);
  at = t6 & 0xFF;
  sb(v0 + 0x000B, at);
  at = t6 & 0x7;
  at = at << 3;
  at += 48; // 0x0030
  sb(v0 + 0x000C, at);
  sb(v0 + 0x000D, at);
  sb(v0 + 0x000E, at);
  at = 46; // 0x002E
  sb(v0 + 0x000F, at);
  at = 0;
  sb(v0 + 0x0010, at);
  at = 4; // 0x0004
  sb(v0 + 0x0011, at);
  at = t6 << 17;
  at = (int32_t)at >> 29;
  sh(v0 + 0x0012, at);
  at = t6 << 14;
  at = (int32_t)at >> 29;
  sh(v0 + 0x0014, at);
  at = t6 << 11;
  at = (int32_t)at >> 29;
  at += 2; // 0x0002
  sh(v0 + 0x0016, at);
  at = 11; // 0x000B
  sb(v0 + 0x0000, at);
  at = 0;
  sb(v0 + 0x0002, at);
  at = 1; // 0x0001
  sb(v0 + 0x0003, at);
  temp = t8 != t9;
  t8 += 8; // 0x0008
  if (temp) goto label80053260;
  return;
label80053350:
  at = t6 << 17;
  at = (int32_t)at >> 27;
  v1 += at;
  sh(v0 + 0x000A, v1);
  at = t6 << 20;
  at = (int32_t)at >> 27;
  a0 += at;
  sh(v0 + 0x000C, a0);
  at = t6 << 23;
  at = at >> 29;
  at += 6; // 0x0006
  a1 -= at;
  sh(v0 + 0x000E, a1);
  at = 72; // 0x0048
  sb(v0 + 0x0000, at);
  at = 1; // 0x0001
  sb(v0 + 0x0001, at);
  at = t6 >> 10;
  at = at & 0x1F;
  sb(v0 + 0x0002, at);
  at = 1; // 0x0001
  sb(v0 + 0x0003, at);
  at = 0x5080F0F0;
  sw(v0 + 0x0010, at);
  at = 0x024080C0;
  sw(v0 + 0x0014, at);
  temp = t8 != t9;
  t8 += 8; // 0x0008
  if (temp) goto label80053260;
  return;
}

// size: 0x000000A4
void function_800524C4(void)
{
  at = 0x800756A4;
  sw(at, lw(at)+1);
  
  at = 0x8007573C;
  v0 = lw(at);
  a0 = lb(v0 + 0x48);
  sb(v0 + 0x48, 0);
  v1 = v0 + 88;

  if (a0 == -1)
    sb(v1 + 0x48, a0);
  else
    while ((int32_t)lb(v1 + 0x48) >= 0)
      v1 += 88;

  sw(at, v1);

  at = 0x80075930;
  v1 = lw(at);
  a1 = lb(v1 - 1);
  sb(v1 - 1, 0);
  a0 = v1 - 24;
  if (a1 == -1)
    sb(a0 - 1, a1);
  else
    while ((int32_t)lb(a0 - 1) >= 0)
      a0 -= 24;
    
  v1 -= 24;
  sw(v0, v1);
  sw(at, a0);
  return;
}



// size: 0x00000098
void function_80053570(void)
{
  uint32_t temp;
  a1 = 0x80075824;
  a1 = lw(a1 + 0x0000);
  at = 0x80075738; // &0x00000000
  v0 = lw(at + 0x0000);
  a1 += 8192; // 0x2000
  temp = v0 != a1;
  if (temp) goto label800535C0;
  a3 = ra;
  a2 = a0;
  mult((spyro_rand() & 0xFF)+1, 0x20);
  v0=lo;
  v0 = a1 - v0;
  temp = a3;
  sb(v0 + 0x0001, a2);
  return;
label800535C0:
  v1 = lb(v0 + 0x0001);
  sb(v0 + 0x0001, a0);
  a0 = -1; // 0xFFFFFFFF
  temp = v1 != a0;
  v1 = v0 + 32; // 0x0020
  if (temp) goto label800535E8;
  temp = v1 == a1;
  if (temp) goto label800535E0;
  sb(v1 + 0x0001, a0);
label800535E0:
  sw(at + 0x0000, v1);
  return;
label800535E8:
  a0 = lb(v1 + 0x0001);
  temp = v1 == a1;
  if (temp) goto label80053600;
  temp = (int32_t)a0 >= 0;
  v1 += 32; // 0x0020
  if (temp) goto label800535E8;
  v1 -= 32; // 0xFFFFFFE0
label80053600:
  sw(at + 0x0000, v1);
  return;
}

// size: 0x000000F8
void function_800333DC(void)
{
  a0 = 3;
  function_80058CC0();
  v1 = lw(0x80075704);
  if (v1 == 99) {
    sw(0x80075864, 1);
    v0 = lw(0x80075818);
    if (v0 != -1) {
      sw(0x800758B4, v0);
      sw(0x80075818, -1);
    } else
      if (lw(total_found_gems) == 14000)
        sw(0x800758B4, 10);
      else
        sw(0x800758B4, 60);
    
    sw(0x800758AC, 0);
    sw(0x800756D0, 0);
    sw(0x800756F8, 0);
    sw(0x80075704, lw(0x80075704)+1);
  } else if (v1 == 100) {
    sw(0x800756F8, lw(0x800756F8)+lw(0x800756CC));
    a0 = 1;
    function_80015370();
  }
}

uint32_t func_80056DC4(uint32_t a, uint32_t b)
{
  for (int i = 0; i < 24; i++)
  {
    if (lw(0x80075F30 + i*0x1C + 0x00) == a
     && lbu(0x80075F30 + i*0x1C + 0x0D) == b)
    {
      if (lhu(0x80075F30 + i*0x1C + 0x0E) & 0x100) 
        return 2;
      else
        return 1;
    }
  }
  return 0;
}

// size: 0x00000078
void function_80056DC4(void)
{
  v0 = func_80056DC4(a0, a1);
}