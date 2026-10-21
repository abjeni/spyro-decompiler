#include "int_math.h"
#include "main.h"
#include "psx_mem.h"
#include "decompilation.h"
#include "gte.h"
#include "spyro_psy.h"
#include "psx_ops.h"
#include "spyro_math.h"
#include "extra_gte.h"

// size: 0x000000D8
void function_800562A4(void)
{
  uint32_t bitfield = 0;
  for (int i = 0; i < 24; i++)
  {
    if (lw(0x80075F30 + i*0x1C) == a0
      && (a1 == 1
      || (a1 == 2
      && lhu(0x80075F30 + i*0x1C + 0x0E) & 0x100))) 
    {
      bitfield |= 1 << i;

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
  sw(0x8007623C, lw(0x8007623C) | bitfield);
}

// size: 0x000000A4
void function_800524C4(void)
{
  sw(0x800756A4, lw(0x800756A4)+1);
  
  v0 = lw(0x8007573C);
  a0 = lb(v0 + 0x48);
  sb(v0 + 0x48, 0);
  v1 = v0 + 88;

  if (a0 == (uint32_t)-1)
    sb(v1 + 0x48, a0);
  else
    while ((int32_t)lb(v1 + 0x48) >= 0)
      v1 += 88;

  sw(0x8007573C, v1);

  v1 = lw(0x80075930);
  a1 = lb(v1 - 1);
  sb(v1 - 1, 0);
  a0 = v1 - 24;
  if (a1 == (uint32_t)-1)
    sb(a0 - 1, a1);
  else
    while ((int32_t)lb(a0 - 1) >= 0)
      a0 -= 24;
    
  v1 -= 24;
  sw(v0, v1);
  sw(0x80075930, a0);
  return;
}

// size: 0x00000098
void function_80053570(void)
{
  uint32_t temp;
  a1 = lw(0x80075824) + 0x2000;
  v0 = lw(0x80075738);
  temp = v0 != a1;
  if (temp) goto label800535C0;
  a3 = ra;
  a2 = a0;
  mult((spyro_rand() & 0xFF)+1, 0x20);
  v0 = a1 - lo;
  temp = a3;
  sb(v0 + 1, a2);
  return;
label800535C0:
  v1 = lb(v0 + 1);
  sb(v0 + 1, a0);
  temp = v1 != (uint32_t)-1;
  v1 = v0 + 0x20;
  if (temp) goto label800535E8;
  if (v1 == a1) goto label800535E0;
  sb(v1 + 1, -1);
label800535E0:
  sw(0x80075738, v1);
  return;
label800535E8:
  a0 = lb(v1 + 0x0001);
  temp = v1 == a1;
  if (temp) goto label80053600;
  temp = (int32_t)a0 >= 0;
  v1 += 0x20;
  if (temp) goto label800535E8;
  v1 -= 0x20;
label80053600:
  sw(0x80075738, v1);
  return;
}

// size: 0x00000310
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
  t6 = spyro_rand();
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

// size: 0x000000F8
void function_800333DC(void)
{
  a0 = 3;
  function_80058CC0();
  v1 = lw(0x80075704);
  if (v1 == 99) {
    sw(0x80075864, 1);
    v0 = lw(0x80075818);
    if (v0 != (uint32_t)-1) {
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

// size: 0x0000006C
void function_8003E1AC(void)
{
  sp -= 0x28;
  sw(sp + 0x20, s0);
  sw(sp + 0x24, ra);

  s0 = 0x80078B34;
  spyro_vec3_copy(s0, 0x80078AFC);
  a1 = abs_int(lw(0x80078BA4));
  spyro_set_vec3_length(s0, abs_int(lw(0x80078BA4)));
  spyro_vec3_clear(sp + 0x10);
  spyro_vec3_sub(s0, sp + 0x10, s0);

  ra = lw(sp + 0x24);
  s0 = lw(sp + 0x20);
  sp += 0x28;
  return;
}

// size: 0x00000038
void function_8005882C(void)
{
  for (uint32_t i = 0; i < 8; i++) {
    if (lbu(0x80077108 + i*0x18 + 0x0C) == 0) {
      v0 = i;
      return;
    }
  }
  v0 = -1;
  return;
}

// size: 0x000000D4
void function_8003AAEC(void)
{
  uint32_t temp;
  sp -= 40; // 0xFFFFFFD8
  sw(sp + 0x0014, s1);
  s1 = a0;
  sw(sp + 0x001C, s3);
  s3 = a1;
  sw(sp + 0x0020, ra);
  sw(sp + 0x0018, s2);
  sw(sp + 0x0010, s0);
  function_8005882C();
  s2 = v0;
  temp = (int32_t)s2 < 0;
  a0 = s1;
  if (temp) goto label8003AB9C;
  a1 = 4; // 0x0004
  s0 = s2 << 1;
  s0 += s2;
  s0 = s0 << 3;
  v0 = 0x80077108;
  s0 += v0;
  v0 = 255; // 0x00FF
  sb(s0 + 0x0010, v0);
  sb(s0 + 0x0011, v0);
  sb(s0 + 0x0012, v0);
  function_800529E4();
  a0 = s1 + 32; // 0x0020
  a1 = s3;
  a2 = s0;
  spyro_set_mat_mirrored_vec_multiply(a0, a1, a2);
  a0 = s0;
  a1 = s0;
  a2 = s1 + 12; // 0x000C
  spyro_vec3_add(a0, a1, a2);
  v0 = spyro_rand();
  sb(s0 + 0x000E, v0);
  v0 = 3; // 0x0003
  v1 = 32; // 0x0020
  sb(s0 + 0x000F, v0);
  v0 = 16; // 0x0010
  sb(s0 + 0x000D, v0);
  v0 = 64; // 0x0040
  sb(s0 + 0x000C, v1);
  sb(s0 + 0x0014, v1);
  sb(s0 + 0x0015, v0);
label8003AB9C:
  v0 = s2;
  ra = lw(sp + 0x0020);
  s3 = lw(sp + 0x001C);
  s2 = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 40; // 0x0028
  return;
}


/*
// size: 0x00000E7C
void function_8001364C(void)
{
  uint32_t temp;
  sp -= 128; // 0xFFFFFF80
  sw(sp + 0x0074, s5);
  s5 = a0;
  a0 = 0;
  sw(sp + 0x0078, ra);
  sw(sp + 0x0070, s4);
  sw(sp + 0x006C, s3);
  sw(sp + 0x0068, s2);
  sw(sp + 0x0064, s1);
  sw(sp + 0x0060, s0);
  function_80056B28();
  function_80058C7C();
  s0 = lw(0x800785E4);
  v1 = lw(LEVEL_ID);
  sw(0x800758C8, 0);
  sw(level_frame_counter, 0);
  sw(0x80075814, 0);
  sw(0x80075684, 0);
  s2 = s0;
  if (v1 == 0x40)
    sw(0x80075830, 0);
  
  sw(0x80075698, 0);
  if (v1 == 0x28 || v1 == 0x2A)
    sw(0x8007591C, 1);
  else
    sw(0x8007591C, 0);
  
  a0 = buttons;
  function_800536A4();
  v0 = lw(0x80075690); // &0x00000000
  temp = v0 != 0;
  if (temp) goto label80013730;
  v0 = lw(0x80078BBC);
  sw(0x8007580C, v0);
label80013730:
  a1 = 0x80077888;
  v0 = lw(a1 + 0x0000);
  temp = v0 == 0;
  if (temp) goto label80013768;
  a0 = player_position;
  a1 += 80; // 0x0050
  spyro_vec3_copy(a0, a1);
  v0 = lw(0x80077888 + 0x005C);
  v0 = (int32_t)v0 >> 4;
  goto label80013884;
label80013768:
  v0 = lw(0x800758AC);
  temp = v0 != 0;
  if (temp) goto label8001388C;
  v0 = lw(s0 + 0x0000);
  v1 = lw(0x800756D0); // &0x00000000
  sw(0x80077888 + 0x0050, v0);
  v0 = lw(s0 + 0x0004);
  sw(0x80077888 + 0x0054, v0);
  v0 = lw(s0 + 0x0008);
  sw(0x80077888 + 0x0058, v0);
  v0 = lbu(s0 + 0x000E);
  sw(0x80077888 + 0x005C, v0);
  temp = v1 != 0;
  v0 = 12; // 0x000C
  if (temp) goto label8001388C;
  v1 = lw(gamestate); // &0x00000000
  temp = v1 == v0;
  v0 = 5; // 0x0005
  if (temp) goto label8001388C;
  temp = v1 != v0;
  if (temp) goto label80013864;
  v0 = lw(LEVEL_ID);
  v0 -= 40; // 0xFFFFFFD8
  v0 = v0 < 20;
  temp = v0 == 0;
  if (temp) goto label80013864;
  a2 = lw(0x800757E8); // &0x00000000
  v0 = a2 - 18; // 0xFFFFFFEE
  v1 = v0 << 1;
  v1 += v0;
  v1 = v1 << 2;
  v0 = lw(0x8006F218 + v1); // &0x00000000
  temp = v0 == 0;
  a1 = a2 << 1;
  if (temp) goto label80013864;
  a0 = player_position;
  a1 += a2;
  a1 = a1 << 2;
  v0 = 0x8006F140; // &0x00000027
  a1 += v0;
  spyro_vec3_copy(a0, a1);
  v0 = lw(0x800757E8); // &0x00000000
  v0 = lbu(0x8006F296 + v0); // &0x3D630000
  goto label80013884;
label80013864:
  a0 = player_position;
  a1 = 0x80077888 + 0x0050;
  spyro_vec3_copy(a0, a1);
  v0 = lw(0x80077888 + 0x005C);
label80013884:
  sb(0x80078A66, v0);
label8001388C:
  v0 = lw(0x8007580C);
  sw(0x80078BBC, v0);
  v0 = lw(gamestate); // &0x00000000
  s1 = 12; // 0x000C
  temp = v0 == s1;
  v0 = 0x66660000;
  if (temp) goto label800138C4;
  a0 = 0;
  function_8004AC24();
  sw(0x80078C48, s1);
  v0 = 0x66660000;
label800138C4:
  a0 = lw(LEVEL_ID);
  v0 = v0 | 0x6667;
  mult(a0, v0);
  v0 = (int32_t)a0 >> 31;
  a0 -= 5; // 0xFFFFFFFB
  t0=hi;
  v1 = (int32_t)t0 >> 2;
  v1 -= v0;
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 1;
  temp = v0 != a0;
  v0 = 1; // 0x0001
  if (temp) goto label8001399C;
  v1 = lw(0x80077888 + 0x0058);
  sw(0x80075690, v0); // &0x00000000
  sw(0x80078CA4, v0);
  sw(0x800758C0, v1);
  sw(0x80078BB0, v1);
  spyro_change_movestate(32);
  v0 = lw(0x800756D0); // &0x00000000
  temp = v0 != 0;
  if (temp) goto label80013A44;
  v0 = lw(player_position + 0x0008);
  sw(0x80078AD4, 0);
  sb(0x80078A64, 0);
  sb(0x80078A65, 0);
  sw(0x80078B78, 0);
  sw(0x80078B7C, 0);
  sw(0x80075960, 0);
  sw(0x800758A0, 0);
  sw(0x80075700, 0); // &0x00000000
  v0 -= 1024; // 0xFFFFFC00
  sw(player_position + 0x0008, v0);
  goto label80013A44;
label8001399C:
  v0 = lw(0x800756D0); // &0x00000000
  sw(0x80075690, 0); // &0x00000000
  sw(0x80078CA4, 0);
  temp = v0 == 0;
  if (temp) goto label80013A44;
  a1 = 0x80077888 + 0x0058;
  v0 = lw(CONTINUOUS_LEVEL_ID);
  v1 = lw(a1 + 0x0000);
  a0 = lw(0x80077888 + 0x005C);
  v0 = v0 << 2;
  v0 = lw(0x8006EDB4 + v0); // &0x00002000
  a0 = a0 << 1;
  v1 += v0;
  sw(a1 + 0x0000, v1);
  v0 = lhu(spyro_cos_lut + a0);
  v1 = lw(0x80077888 + 0x0050);
  v0 = v0 << 16;
  v0 = (int32_t)v0 >> 18;
  v1 -= v0;
  sw(0x80077888 + 0x0050, v1);
  v0 = lhu(spyro_sin_lut + a0);
  v1 = lw(0x80077888 + 0x0054);
  v0 = v0 << 16;
  v0 = (int32_t)v0 >> 18;
  v1 -= v0;
  sw(0x80077888 + 0x0054, v1);
label80013A44:
  v1 = lw(LEVEL_ID);
  v0 = 64; // 0x0040
  temp = v1 != v0;
  s0 += 16; // 0x0010
  if (temp) goto label80013A70;
  v0 = 16000; // 0x3E80
  sw(0x800758C0, v0);
  v0 = 17976; // 0x4638
  sw(0x80075678, v0); // &0x00000000
label80013A70:
  v0 = lw(0x800756D0); // &0x00000000
  sw(0x80075918, 0);
  temp = v0 != 0;
  v0 = 12; // 0x000C
  if (temp) goto label80013B34;
  v1 = lw(gamestate); // &0x00000000
  temp = v1 == v0;
  if (temp) goto label80013B68;
  a0 = 0x80078668;
  a2 = 0x8006C934; // &0x00000800
  v0 = lw(a2 + 0x0000);
  v1 = lw(a2 + 0x0004);
  a1 = lw(a2 + 0x0008);
  sw(a0 + 0x0000, v0);
  sw(a0 + 0x0004, v1);
  sw(a0 + 0x0008, a1);
  v0 = lw(a2 + 0x000C);
  v1 = lw(a2 + 0x0010);
  a1 = lw(a2 + 0x0014);
  sw(a0 + 0x000C, v0);
  sw(a0 + 0x0010, v1);
  sw(a0 + 0x0014, a1);
  v1 = lw(0x80078B74);
  v0 = player_position;
  sw(0x80076EA0, v0);
  v0 = 1536; // 0x0600
  sw(0x80076E28, 0);
  sw(0x80076E90, 0);
  sw(0x80078670, v0);
  sw(0x80076EA8, a0);
  sw(0x80076EA4, v1);
  function_80034358();
  v0 = 32; // 0x0020
  sw(0x80075918, v0);
label80013B34:
  v1 = lw(gamestate); // &0x00000000
  v0 = 12; // 0x000C
  temp = v1 == v0;
  v0 = 1; // 0x0001
  if (temp) goto label80013B68;
  sw(gamestate, 0); // &0x00000000
  sw(0x8007579C, v0); // &0x00000000
  sw(0x8007570C, 0); // &0x00000000
  sw(0x800756C0, 0); // &0x00000000
label80013B68:
  a0 = 0x800786C8;
  a1 = 0;
  a2 = 312; // 0x0138
  spyro_memset32(a0, a1, a2);
  a0 = 0x8007AA10;
  a1 = 0;
  v0 = lw(s0 + 0x0000);
  a2 = 40; // 0x0028
  sw(0x800786C8 + 0x0010, v0);
  v0 = lw(s0 + 0x0004);
  sw(0x800786C8 + 0x0014, v0);
  s0 += 8; // 0x0008
  spyro_memset32(a0, a1, a2);
  v0 = lw(s0 + 0x0000);
  sw(0x80075EF8, v0);
  v0 = lw(s0 + 0x0004);
  s0 += 8; // 0x0008
  sw(0x80075EFC, v0);
  v0 = lw(s0 + 0x0008);
  sw(0x8007850C, v0);
  v0 = lw(s0 + 0x000C);
  s1 = 0;
  sw(0x80078510, v0);
  a1 = s1 + 1; // 0x0001
label80013BE8:
  v0 = s1 << 3;
  v0 += s0;
  v1 = lw(v0 + 0x0010);
  a0 = a1 << 3;
  sw(0x8007850C + a0, v1);
  v0 = lw(v0 + 0x0014);
  s1 = a1;
  sw(0x80078510 + a0, v0);
  v0 = (int32_t)s1 < 9;
  temp = v0 != 0;
  a1 = s1 + 1; // 0x0001
  if (temp) goto label80013BE8;
  s0 += 88; // 0x0058
  v0 = lw(s0 + 0x0000);
  s1 = 0;
  sw(0x800786C8 + 0x0018, v0);
  v0 = lw(s0 + 0x0004);
  s0 += 8; // 0x0008
  sw(0x800786C8 + 0x001C, v0);
  v0 = lw(s0 + 0x0000);
  v1 = 0x80078560;
  sw(0x800756F0, v0); // &0x00000000
  v0 = lw(s0 + 0x0004);
  s0 += 8; // 0x0008
  a1 = s0;
  s0 += 4; // 0x0004
  sw(0x800756F4, v0); // &0x00000000
  v0 = lw(s0 + 0x0000);
  s0 += 4; // 0x0004
  sw(0x80078560 + 0x0004, s0);
  temp = (int32_t)v0 <= 0;
  sw(v1 + 0x0000, v0);
  if (temp) goto label80013CB8;
label80013C8C:
  v0 = lw(s0 + 0x0000);
  s1++;
  v0 += 4; // 0x0004
  v0 += a1;
  sw(s0 + 0x0000, v0);
  v0 = lw(v1 + 0x0000);
  v0 = (int32_t)s1 < (int32_t)v0;
  temp = v0 != 0;
  s0 += 4; // 0x0004
  if (temp) goto label80013C8C;
  s1 = 0;
label80013CB8:
  v0 = lw(a1 + 0x0000);
  v1 = 0x80078560 + 0x0008;
  s0 = a1 + v0;
  a1 = s0;
  s0 += 4; // 0x0004
  v0 = lw(s0 + 0x0000);
  s0 += 4; // 0x0004
  sw(0x80078560 + 0x000C, s0);
  temp = (int32_t)v0 <= 0;
  sw(v1 + 0x0000, v0);
  if (temp) goto label80013D10;
label80013CE8:
  v0 = lw(s0 + 0x0000);
  s1++;
  v0 += 4; // 0x0004
  v0 += a1;
  sw(s0 + 0x0000, v0);
  v0 = lw(v1 + 0x0000);
  v0 = (int32_t)s1 < (int32_t)v0;
  temp = v0 != 0;
  s0 += 4; // 0x0004
  if (temp) goto label80013CE8;
label80013D10:
  s1 = 0;
  v0 = lw(a1 + 0x0000);
  v1 = 0x80078560 + 0x0020;
  s0 = a1 + v0;
  a1 = s0;
  s0 += 4; // 0x0004
  v0 = lw(s0 + 0x0000);
  s0 += 4; // 0x0004
  sw(0x80078560 + 0x0024, s0);
  temp = (int32_t)v0 <= 0;
  sw(v1 + 0x0000, v0);
  if (temp) goto label80013D70;
label80013D44:
  v0 = lw(s0 + 0x0000);
  s1++;
  v0 += 4; // 0x0004
  v0 += a1;
  sw(s0 + 0x0000, v0);
  v0 = lw(v1 + 0x0000);
  v0 = (int32_t)s1 < (int32_t)v0;
  temp = v0 != 0;
  s0 += 4; // 0x0004
  if (temp) goto label80013D44;
  s1 = 0;
label80013D70:
  v0 = lw(a1 + 0x0000);
  v1 = 0x80078560 + 0x0010;
  s0 = a1 + v0;
  a1 = s0;
  s0 += 4; // 0x0004
  v0 = lw(s0 + 0x0000);
  s0 += 4; // 0x0004
  sw(0x80078560 + 0x0014, s0);
  temp = (int32_t)v0 <= 0;
  sw(v1 + 0x0000, v0);
  if (temp) goto label80013DC8;
label80013DA0:
  v0 = lw(s0 + 0x0000);
  s1++;
  v0 += 4; // 0x0004
  v0 += a1;
  sw(s0 + 0x0000, v0);
  v0 = lw(v1 + 0x0000);
  v0 = (int32_t)s1 < (int32_t)v0;
  temp = v0 != 0;
  s0 += 4; // 0x0004
  if (temp) goto label80013DA0;
label80013DC8:
  s1 = 0;
  v0 = lw(a1 + 0x0000);
  v1 = 0x80078560 + 0x0040;
  s0 = a1 + v0;
  a1 = s0;
  s0 += 4; // 0x0004
  v0 = lw(s0 + 0x0000);
  s0 += 4; // 0x0004
  sw(0x80078560 + 0x0044, s0);
  temp = (int32_t)v0 <= 0;
  sw(v1 + 0x0000, v0);
  if (temp) goto label80013E28;
label80013DFC:
  v0 = lw(s0 + 0x0000);
  s1++;
  v0 += 4; // 0x0004
  v0 += a1;
  sw(s0 + 0x0000, v0);
  v0 = lw(v1 + 0x0000);
  v0 = (int32_t)s1 < (int32_t)v0;
  temp = v0 != 0;
  s0 += 4; // 0x0004
  if (temp) goto label80013DFC;
  s1 = 0;
label80013E28:
  v0 = lw(a1 + 0x0000);
  v1 = 0x80078560 + 0x0028;
  s0 = a1 + v0;
  a1 = s0;
  s0 += 4; // 0x0004
  v0 = lw(s0 + 0x0000);
  s0 += 4; // 0x0004
  sw(0x80078560 + 0x002C, s0);
  temp = (int32_t)v0 <= 0;
  sw(v1 + 0x0000, v0);
  if (temp) goto label80013E80;
label80013E58:
  v0 = lw(s0 + 0x0000);
  s1++;
  v0 += 4; // 0x0004
  v0 += a1;
  sw(s0 + 0x0000, v0);
  v0 = lw(v1 + 0x0000);
  v0 = (int32_t)s1 < (int32_t)v0;
  temp = v0 != 0;
  s0 += 4; // 0x0004
  if (temp) goto label80013E58;
label80013E80:
  s1 = 0;
  v0 = lw(a1 + 0x0000);
  v1 = 0x80078560 + 0x0018;
  s0 = a1 + v0;
  a1 = s0;
  s0 += 4; // 0x0004
  v0 = lw(s0 + 0x0000);
  s0 += 4; // 0x0004
  sw(0x80078560 + 0x001C, s0);
  temp = (int32_t)v0 <= 0;
  sw(v1 + 0x0000, v0);
  if (temp) goto label80013EDC;
label80013EB4:
  v0 = lw(s0 + 0x0000);
  s1++;
  v0 += 4; // 0x0004
  v0 += a1;
  sw(s0 + 0x0000, v0);
  v0 = lw(v1 + 0x0000);
  v0 = (int32_t)s1 < (int32_t)v0;
  temp = v0 != 0;
  s0 += 4; // 0x0004
  if (temp) goto label80013EB4;
label80013EDC:
  v0 = lw(a1 + 0x0000);
  temp = s5 == 0;
  s0 = a1 + v0;
  if (temp) goto label80013EF8;
  function_8002B4AC();
  a1 = s0;
  goto label80013F24;
label80013EF8:
  sw(0x80078560, 0);
  sw(0x80078560 + 0x08, 0);
  sw(0x80078560 + 0x40, 0);
  sw(0x80078560 + 0x28, 0);
  sw(0x80078560 + 0x18, 0);
  a1 = s0;
label80013F24:
  s0 += 4; // 0x0004
  s1 = lw(s0 + 0x0000);
  s0 += 4; // 0x0004
  a0 = 0x24924926;
  sw(0x80075828, s0);
  v0 = lw(a1 + 0x0000);
  v1 = lw(0x80075828);
  s0 = a1 + v0;
  v0 = s1 << 1;
  v0 += s1;
  v0 = v0 << 2;
  v0 -= s1;
  v0 = v0 << 3;
  v1 += v0;
  v0 = 255; // 0x00FF
  sw(0x80075890, v1);
  sw(0x8007573C, v1); // &0x00000000
  sb(v1 + 0x0048, v0);
  sw(0x800756A4, 0); // &0x00000000
  sw(0x800756E8, s0); // &0x00000000
  sw(0x80075930, s0);
  sb(s0 - 0x0001, v0); // 0xFFFFFFFF
  v0 = lw(0x800756E8); // &0x00000000
  v1 = lw(0x80075890);
  v0 -= v1;
  v0 = v0 >> 4;
  multu(v0, a0);
  t0=hi;
  sw(0x800756A8, t0); // &0x00000000
  v0 = lw(s0 + 0x0000);
  s0 += v0;
  a1 = s0;
  s0 += 4; // 0x0004
  v0 = lw(s0 + 0x0000);
  s0 += 4; // 0x0004
  sw(0x800757F8, s0); // &0x00000000
  s1 = v0 - 1; // 0xFFFFFFFF
  sw(0x800757B4, v0); // &0x00000000
  temp = (int32_t)s1 < 0;
  v0 = s1 << 2;
  if (temp) goto label80014018;
  v1 = v0 + s0;
label80014000:
  v0 = lw(v1 + 0x0000);
  s1--;
  v0 += a1;
  sw(v1 + 0x0000, v0);
  temp = (int32_t)s1 >= 0;
  v1 -= 4; // 0xFFFFFFFC
  if (temp) goto label80014000;
label80014018:
  v0 = lw(a1 + 0x0000);
  s0 = a1 + v0;
  a1 = s0;
  s0 += 4; // 0x0004
  sw(0x80075778, s0); // &0x00000000
  v0 = lw(a1 + 0x0000);
  s1 = 0;
  s0 = a1 + v0;
  a1 = s0;
  v0 = lw(a1 + 0x0000);
  temp = (int32_t)v0 <= 0;
  s0 += 4; // 0x0004
  if (temp) goto label80014084;
label80014054:
  v0 = lw(s0 + 0x0000);
  v0 += s2;
  v1 = lw(v0 + 0x0000);
  s1++;
  v1 += s2;
  sw(v0 + 0x0000, v1);
  v0 = lw(a1 + 0x0000);
  v0 = (int32_t)s1 < (int32_t)v0;
  temp = v0 != 0;
  s0 += 4; // 0x0004
  if (temp) goto label80014054;
label80014084:
  v0 = lw(IS_DEMO_MODE);
  temp = v0 == 0;
  if (temp) goto label80014154;
  a0 = 1234; // 0x04D2
  spyro_srand(a0);
  v1 = lw(0x800757B8); // &0x00000000
  v1 = v1 << 4;
  v0 = lw(0x8006EE9C + v1); // &0x00029EBE
  sw(player_position, v0);
  v0 = lw(0x8006EEA0 + v1); // &0x0002254F
  sw(player_position + 0x0004, v0);
  v0 = lw(0x8006EEA4 + v1); // "jQ"
  sw(player_position + 0x0008, v0);
  v0 = lw(0x8006EEA8 + v1); // &0x00000020
  v1 = lw(IS_DEMO_MODE);
  a0 = 1; // 0x0001
  sw(0x80075784, a0); // &0x00000000
  sb(0x80078A66, v0);
  v0 = v0 & 0xFF;
  v0 = v0 << 4;
  sw(0x80078B74, v0);
  temp = v1 != a0;
  v0 = 0x80600000;
  if (temp) goto label8001413C;
  sw(0x8007585C, s0);
  s2 = 0;
  goto label80014158;
label8001413C:
  sw(0x8007585C, v0);
  a0 = 0x80600000;
  a1 = 0;
  a2 = 16384; // 0x4000
  spyro_memset32(a0, a1, a2);
label80014154:
  s2 = 0;
label80014158:
  a0 = lw(0x80075828);
  v1 = lbu(a0 + 0x0048);
  v0 = 255; // 0x00FF
  temp = v1 == v0;
  s1 = 0;
  if (temp) goto label800142B4;
  s3 = 1; // 0x0001
  s0 = 0;
label8001417C:
  a0 += s0;
  v0 = lh(a0 + 0x0036);
  v0 = v0 << 2;
  v0 = lw(0x80076378 + v0);
  temp = (int32_t)v0 >= 0;
  if (temp) goto label800141B4;
  function_800526A8();
  goto label800141BC;
label800141B4:
  function_800529CC();
label800141BC:
  a1 = lw(0x80075828);
  a0 = sp + 16; // 0x0010
  a1 += s0;
  a1 += 12; // 0x000C
  spyro_vec3_copy(a0, a1);
  a0 = sp + 16; // 0x0010
  v0 = lw(sp + 0x0018);
  a1 = 0x00010000;
  v0 += 1024; // 0x0400
  sw(sp + 0x0018, v0);
  function_8004D5EC();
  a0 = lw(0x80075828);
  a0 += s0;
  function_800533D0();
  v0 = lw(0x80075828);
  v0 += s0;
  a3 = lbu(v0 + 0x003A);
  v0 = a3 & 0x7F;
  v0 = v0 < 32;
  temp = v0 == 0;
  a2 = (int32_t)s1 >> 5;
  if (temp) goto label8001427C;
  a1 = s1 & 0x1F;
  a0 = a2 << 2;
  v0 = lw(0x80077888 + 0x0010 + a0);
  a1 = s3 << a1;
  v0 = v0 & a1;
  temp = v0 == 0;
  v0 = s3 << a3;
  if (temp) goto label80014278;
  v0 = lw(CONTINUOUS_LEVEL_ID);
  v1 = 0x80077908;
  v0 = v0 << 5;
  v0 += v1;
  v0 += a0;
  v0 = lw(v0 + 0x0000);
  v0 = v0 & a1;
  temp = v0 != 0;
  v0 = s3 << a3;
  if (temp) goto label8001427C;
label80014278:
  s2 = s2 | v0;
label8001427C:
  s0 += 88; // 0x0058
  a0 = lw(0x80075828);
  v0 = s0 + a0;
  v1 = lbu(v0 + 0x0048);
  v0 = 255; // 0x00FF
  temp = v1 != v0;
  s1++;
  if (temp) goto label8001417C;
  a0 = lw(0x80075828);
  v1 = lbu(a0 + 0x0048);
  v0 = 255; // 0x00FF
label800142B4:
  temp = v1 == v0;
  s1 = 0;
  if (temp) goto label800143D0;
  s4 = 1; // 0x0001
  s3 = 255; // 0x00FF
  s0 = 0;
label800142C8:
  a2 = (int32_t)s1 >> 5;
  a1 = s1 & 0x1F;
  v1 = 0x80077908;
  v0 = lw(CONTINUOUS_LEVEL_ID);
  a2 = a2 << 2;
  v0 = v0 << 5;
  v0 += v1;
  v0 += a2;
  v0 = lw(v0 + 0x0000);
  v1 = s4 << a1;
  v0 = v0 & v1;
  temp = v0 == 0;
  if (temp) goto label800143B0;
  v0 = lw(0x80077888 + 0x0010 + a2);
  v0 = v0 & v1;
  temp = v0 != 0;
  a0 += s0;
  if (temp) goto label8001433C;
  v0 = lbu(a0 + 0x003A);
  v1 = 125; // 0x007D
  v0 = v0 & 0x7F;
  temp = v0 == v1;
  if (temp) goto label800143A8;
  sb(a0 + 0x0053, s3);
  goto label800143B0;
label8001433C:
  v0 = lw(0x80075828);
  v0 += s0;
  v0 = lbu(v0 + 0x003A);
  a1 = v0 & 0x7F;
  v0 = (int32_t)a1 < 32;
  temp = v0 == 0;
  v0 = s4 << a1;
  if (temp) goto label80014370;
  v0 = s2 & v0;
  v0 = 0 < v0;
  goto label80014378;
label80014370:
  v0 = a1 ^ 0x7E;
  v0 = v0 < 1;
label80014378:
  temp = v0 == 0;
  v1 = 255; // 0x00FF
  if (temp) goto label80014398;
  v0 = lw(0x80075828);
  v0 += s0;
  sb(v0 + 0x0053, v1);
  goto label800143B0;
label80014398:
  a0 = lw(0x80075828);
  a0 += s0;
label800143A8:
  function_80052568();
label800143B0:
  a0 = lw(0x80075828);
  s0 += 88; // 0x0058
  v0 = s0 + a0;
  v0 = lbu(v0 + 0x0048);
  temp = v0 != s3;
  s1++;
  if (temp) goto label800142C8;
label800143D0:
  s1 = 0;
  v0 = 0x80077888 + 0x0030;
  v1 = v0;
  a0 = v1 - 32; // 0xFFFFFFE0
label800143E4:
  v0 = lw(a0 + 0x0000);
  a0 += 4; // 0x0004
  s1++;
  sw(v1 + 0x0000, v0);
  v0 = (int32_t)s1 < 8;
  temp = v0 != 0;
  v1 += 4; // 0x0004
  if (temp) goto label800143E4;
  s1 = 9; // 0x0009
  v0 = 0x800777E4;
label8001440C:
  sw(v0 + 0x0000, 0);
  s1--;
  temp = (int32_t)s1 >= 0;
  v0 -= 4; // 0xFFFFFFFC
  if (temp) goto label8001440C;
  v0 = lw(0x800785F8);
  v1 = 255; // 0x00FF
  sw(0x80075824, v0);
  sw(0x80075738, v0); // &0x00000000
  sb(v0 + 0x0001, v1);
  v1 = lw(0x80075824);
  v0 = -1; // 0xFFFFFFFF
  sw(v1 + 0x2000, v0);
  function_80058B68();
  temp = s5 == 0;
  if (temp) goto label80014490;
  v0 = lw(0x80075690); // &0x00000000
  temp = v0 != 0;
  a0 = 120; // 0x0078
  if (temp) goto label80014490;
  v0 = lw(0x800758CC);
  temp = v0;
  a1 = 0;
  switch (temp)
  {
  case 0x800857CC:
    function_800857CC();
    break;
  case 0x8008772C:
    function_8008772C();
    break;
  case 0x80082960:
    function_80082960();
    break;
  case 0x800872A4:
    function_800872A4();
    break;
  case 0x80081DA8:
    function_80081DA8();
    break;
  case 0x8008249C:
    function_8008249C();
    break;
  case 0x8008A258:
    function_8008A258();
    break;
  case 0x8008A4D0:
    function_8008A4D0();
    break;
  case 0x80088B88:
    function_80088B88();
    break;
  case 0x80083608:
    function_80083608();
    break;
  case 0x800845F0:
    function_800845F0();
    break;
  case 0x800819BC:
    function_800819BC();
    break;
  case 0x8008B2C0:
    function_8008B2C0();
    break;
  case 0x8008A36C:
    function_8008A36C();
    break;
  case 0x8008883C:
    function_8008883C();
    break;
  case 0x80086DD8:
    function_80086DD8();
    break;
  case 0x80083AB4:
    function_80083AB4();
    break;
  case 0x80081F0C:
    function_80081F0C();
    break;
  case 0x80087EF0:
    function_80087EF0();
    break;
  case 0x8008465C:
    function_8008465C();
    break;
  case 0x80084718:
    function_80084718();
    break;
  case 0x80086B38:
    function_80086B38();
    break;
  case 0x800874FC:
    function_800874FC();
    break;
  case 0x8008223C:
    function_8008223C();
    break;
  case 0x80088E24:
    function_80088E24();
    break;
  case 0x80084B94:
    function_80084B94();
    break;
  case 0x80084620:
    function_80084620();
    break;
  case 0x8008590C:
    function_8008590C();
    break;
  case 0x800836A8:
    function_800836A8();
    break;
  case 0x80082028:
    function_80082028();
    break;
  case 0x80083568:
    function_80083568();
    break;
  case 0x80085664:
    function_80085664();
    break;
  case 0x80083108:
    function_80083108();
    break;
  case 0x80082F24:
    function_80082F24();
    break;
  case 0x80083690:
    function_80083690();
    break;
  default:
    JALR(temp, 0x80014478);
  }
  sw(0x80075898, v0);
  a0 = 1; // 0x0001
  function_80054600();
label80014490:
  a0 = lw(0x800774B0);
  a1 = 1; // 0x0001
  function_800567F4();
  ra = lw(sp + 0x0078);
  s5 = lw(sp + 0x0074);
  s4 = lw(sp + 0x0070);
  s3 = lw(sp + 0x006C);
  s2 = lw(sp + 0x0068);
  s1 = lw(sp + 0x0064);
  s0 = lw(sp + 0x0060);
  sp += 128; // 0x0080
  return;
}
*/