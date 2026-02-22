#include "main.h"
#include "psx_mem.h"
#include "psx_ops.h"
#include "decompilation.h"
#include "gte.h"
#include "extra_gte.h"

// different return instruction (not JR RA)
void function_800530C0(void)
{
  uint32_t temp;
  t7 = a0;
  cop2.RBK = a1;
  load_RTM(t7 + 0x20);
  cop2.TRX = lw(t7 + 0x0C) >> 2;
  cop2.TRY = lw(t7 + 0x10) >> 2;
  cop2.TRZ = lw(t7 + 0x14) >> 2;
  at = lhu(t7 + 0x36);
  v1 = lw(t7 + 0x3C);
  at = lw(0x80076378 + at*4);
  v0 = lw(at + (v1 & 0xFF)*4 + 0x38);
  v1 = v0 + ((v1 >> 16) & 0xFF)*8 + 0x24;
  at = lw(v1 + 0x00);
  v1 = lw(v1 + 0x04);
  a0 = lbu(v0 + 0x08);
  a1 = lbu(v0 + 0x05);
  a2 = lbu(v0 + 0x06);
  t8 = lw(v0 + 0x14);
  t9 = lw(v0 + 0x18);
  at = at & 0x1FFFFF;
  v0 = at + (v1 >> 24)*4;
  v1 = 0x1F800000;
  t3 = 1;
  a0 = a0 >> 1;
  a0 = a0 << 3;
  a0 += 0x1F800000;
  t2 = 0;
  a2 += a1 + 1;
label800531A0:
  temp = t2 != 0;
  if (temp) goto label800531D8;
  t1 = lw(at);
  at += 4;
  t2 = t1 & 1;
  a3 = (int32_t)(t1 <<  0) >> 21 << a1;
  t0 = (int32_t)(t1 << 10) >> 21 << a1;
  t1 = (int32_t)(t1 << 20) >> 19 << a1;
  goto label80053210;
label800531D8:
  t4 = lh(v0 + 0x0000);
  v0 += 2; // 0x0002
  t2 = t4 & 1;
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
  function_8006272C();
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


// different return instruction (not JR RA)
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
  function_8006272C();
  mult((v0 & 0xFF)+1, 0x20);
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

// patched out spinlock
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