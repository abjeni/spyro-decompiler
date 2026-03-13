/*

#include <stdint.h>

#include "psx_mem.h"
#include "psx_ops.h"
#include "main.h"
#include "decompilation.h"
#include "spyro_string.h"
#include "spyro_vsync.h"
#include "spyro_math.h"

//#define WIDESCREEN

static void adjust_mat(uint32_t mat)
{
#ifdef WIDESCREEN
  for (int i = 0; i < 3; i++) {
    uint32_t addr = sp + 0x10 + 3*2*0 + i*2;
    sh(addr, ((int32_t)lh(addr))*320/512);
  }
  for (int i = 0; i < 3; i++) {
    uint32_t addr = sp + 0x10 + 3*2*1 + i*2;
    sh(addr, ((int32_t)lh(addr))*320/512);
  }
#else // WIDESCREEN
  for (int i = 0; i < 3; i++) {
    uint32_t addr = sp + 0x10 + 3*2*1 + i*2;
    sh(addr, ((int32_t)lh(addr))*320/512);
  }
#endif // WIDESCREEN
}

// size: 0x000003BC
void function_8001A050(void)
{
  uint32_t temp;
  sp -= 96; // 0xFFFFFFA0
  sw(sp + 0x0058, ra);
  sw(sp + 0x0054, s1);
  sw(sp + 0x0050, s0);

  v0 = lbu(SKYBOX_DATA + 0x10);
  v1 = lbu(SKYBOX_DATA + 0x11);
  a3 = lbu(SKYBOX_DATA + 0x12);

  sb(DISP1 + 0x19, v0);
  sb(DISP1 + 0x1A, v1);
  sb(DISP1 + 0x1B, a3);
  sb(DISP2 + 0x19, v0);
  sb(DISP2 + 0x1A, v1);
  sb(DISP2 + 0x1B, a3);

  spyro_memset32(0x8006FCF4, 0, 0x900);
  v0 = lw(0x800756B0);
  temp = v0 == 0;
  if (temp) goto label8001A0D8;
  function_8001973C();
label8001A0D8:
  function_80023AC4();
  v0 = lw(SKYBOX_DATA);
  temp = v0 == 0;
  if (temp) goto label8001A334;
  v0 = lw(0x80075910);
  v0 -= 2; // 0xFFFFFFFE
  sw(0x80075910, v0);
  temp = (int32_t)v0 >= 0;
  if (temp) goto label8001A11C;
  sw(0x80075910, 0);
label8001A11C:
  v0 = lw(0x80075910);
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label8001A31C;
  a0 = sp + 16; // 0x0010
  a2 = 32; // 0x0020
  spyro_memset32(a0, a1, a2);
  a0 = sp + 16; // 0x0010
  function_800625F8();
  s0 = 0x80076E1E;
  v0 = lh(s0 + 0x0000);
  a0 = lw(0x80075910);
  s1 = 4096; // 0x1000
  sh(sp + 0x0010, s1);
  a0 = v0 - a0;
  v0 = spyro_cos(a0);
  v1 = lh(s0 + 0x0000);
  a0 = lw(0x80075910);
  sh(sp + 0x0018, v0);
  a0 = v1 - a0;
  v0 = spyro_sin(a0);
  v1 = lh(s0 + 0x0000);
  a0 = lw(0x80075910);
  sh(sp + 0x001E, v0);
  a0 = v1 - a0;
  v0 = spyro_sin(a0);
  v1 = lh(s0 + 0x0000);
  a0 = lw(0x80075910);
  v0 = -v0;
  sh(sp + 0x001A, v0);
  a0 = v1 - a0;
  v0 = spyro_cos(a0);
  s0 = sp + 48; // 0x0030
  a0 = s0;
  a1 = 0;
  a2 = 32; // 0x0020
  sh(sp + 0x0020, v0);
  spyro_memset32(a0, a1, a2);
  a0 = lh(0x80076E20);
  v0 = spyro_cos(a0);
  a0 = lh(0x80076E20);
  sh(sp + 0x0030, v0);
  v0 = spyro_sin(a0);
  a0 = lh(0x80076E20);
  v0 = -v0;
  sh(sp + 0x003C, v0);
  sh(sp + 0x0038, s1);
  v0 = spyro_sin(a0);
  a0 = lh(0x80076E20);
  sh(sp + 0x0034, v0);
  v0 = spyro_cos(a0);
  a0 = sp + 16; // 0x0010
  a1 = s0;
  sh(sp + 0x0040, v0);
  function_800624E8();
  a0 = s0;
  a1 = 0;
  a2 = 32; // 0x0020
  spyro_memset32(a0, a1, a2);
  a0 = lh(0x80076E1C);
  v0 = spyro_cos(a0);
  a0 = lh(0x80076E1C);
  sh(sp + 0x0030, v0);
  v0 = spyro_sin(a0);
  a0 = lh(0x80076E1C);
  v0 = -v0;
  sh(sp + 0x0036, v0);
  v0 = spyro_sin(a0);
  a0 = lh(0x80076E1C);
  sh(sp + 0x0032, v0);
  v0 = spyro_cos(a0);
  a0 = sp + 16; // 0x0010
  a1 = s0;
  sh(sp + 0x0038, v0);
  sh(sp + 0x0040, s1);
  function_800624E8();;
  a0 = s0;
  a1 = sp + 16; // 0x0010
  a2 = 20; // 0x0014
  spyro_memcpy32(a0, a1, a2);

  adjust_mat(sp + 0x10);

  spyro_memcpy8(sp + 0x36, sp + 0x16, 6);
  
  a0 = -1; // 0xFFFFFFFF
  a1 = sp + 16; // 0x0010
  a2 = s0;
  goto label8001A32C;
label8001A31C:
  a0 = -1; // 0xFFFFFFFF
  a1 = 0x80076DE4;
  a2 = a1 - 20; // 0xFFFFFFEC
label8001A32C:
  function_8004EBA8();
label8001A334:
  a0 = 0;
  function_8005F764();
  v0 = lw(0x80075784); // &0x00000000
  temp = v0 == 0;
  if (temp) goto label8001A358;
  a0 = 0;
  v0 = VSync(a0);
label8001A358:
  a0 = -1; // 0xFFFFFFFF
  v0 = VSync(a0);
  v1 = lw(drawn_frame);
  sw(current_frame, v0);
  v0 -= v1;
  v0 = (int32_t)v0 < 2;
  temp = v0 == 0;
  if (temp) goto label8001A3B4;
  s0 = current_frame;
label8001A388:
  a0 = 0;
  v0 = VSync(a0);
  a0 = -1; // 0xFFFFFFFF
  v0 = VSync(a0);
  v1 = lw(s0 - 0x0004); // 0xFFFFFFFC
  sw(current_frame, v0);
  v0 -= v1;
  v0 = (int32_t)v0 < 2;
  temp = v0 != 0;
  if (temp) goto label8001A388;
label8001A3B4:
  a0 = -1; // 0xFFFFFFFF
  v0 = VSync(a0);
  a0 = lw(BACKBUFFER_DISP);
  sw(drawn_frame, v0);
  a0 += 92; // 0x005C
  function_80060030();
  a0 = lw(BACKBUFFER_DISP);
  function_8005FDD8();
  a0 = 2048; // 0x0800
  function_80016784();
  a0 = v0;
  function_8005FD64();
  ra = lw(sp + 0x0058);
  s1 = lw(sp + 0x0054);
  s0 = lw(sp + 0x0050);
  sp += 96; // 0x0060
  return;
}

// size: 0x000001F0
void function_80033C50(void)
{
  sp -= 96; // 0xFFFFFFA0
  a0 = sp + 16; // 0x0010
  a1 = 0;
  a2 = 32; // 0x0020
  sw(sp + 0x005C, ra);
  sw(sp + 0x0058, s2);
  sw(sp + 0x0054, s1);
  sw(sp + 0x0050, s0);
  spyro_memset32(a0, a1, a2);
  a0 = sp + 16; // 0x0010
  function_800625F8();
  s2 = 0x80076E1E;
  a0 = lh(s2 + 0x0000);
  s1 = 4096; // 0x1000
  sh(sp + 0x0010, s1);
  v0 = spyro_cos(a0);
  a0 = lh(s2 + 0x0000);
  sh(sp + 0x0018, v0);
  v0 = spyro_sin(a0);
  a0 = lh(s2 + 0x0000);
  sh(sp + 0x001E, v0);
  v0 = spyro_sin(a0);
  a0 = lh(s2 + 0x0000);
  v0 = -v0;
  sh(sp + 0x001A, v0);
  v0 = spyro_cos(a0);
  s0 = sp + 48; // 0x0030
  a0 = s0;
  a1 = 0;
  a2 = 32; // 0x0020
  sh(sp + 0x0020, v0);
  spyro_memset32(a0, a1, a2);
  a0 = lh(0x80076E20);
  v0 = spyro_cos(a0);
  a0 = lh(0x80076E20);
  sh(sp + 0x0030, v0);
  v0 = spyro_sin(a0);
  a0 = lh(0x80076E20);
  v0 = -v0;
  sh(sp + 0x003C, v0);
  sh(sp + 0x0038, s1);
  v0 = spyro_sin(a0);
  a0 = lh(0x80076E20);
  sh(sp + 0x0034, v0);
  v0 = spyro_cos(a0);
  a0 = sp + 16; // 0x0010
  a1 = s0;
  sh(sp + 0x0040, v0);
  function_800624E8();;
  a0 = s0;
  a1 = 0;
  a2 = 32; // 0x0020
  spyro_memset32(a0, a1, a2);
  a0 = lh(0x80076E1C);
  v0 = spyro_cos(a0);
  a0 = lh(0x80076E1C);
  sh(sp + 0x0030, v0);
  v0 = spyro_sin(a0);
  a0 = lh(0x80076E1C);
  v0 = -v0;
  sh(sp + 0x0036, v0);
  v0 = spyro_sin(a0);
  a0 = lh(0x80076E1C);
  sh(sp + 0x0032, v0);
  v0 = spyro_cos(a0);
  a0 = sp + 16; // 0x0010
  a1 = s0;
  sh(sp + 0x0038, v0);
  sh(sp + 0x0040, s1);
  function_800624E8();;
  a0 = s2 - 58; // 0xFFFFFFC6
  a1 = sp + 16; // 0x0010
  a2 = 20; // 0x0014
  spyro_memcpy32(a0, a1, a2);

  adjust_mat(sp + 0x10);

  a0 = s2 - 78; // 0xFFFFFFB2
  a1 = sp + 16; // 0x0010
  a2 = 20; // 0x0014
  spyro_memcpy32(a0, a1, a2);
  ra = lw(sp + 0x005C);
  s2 = lw(sp + 0x0058);
  s1 = lw(sp + 0x0054);
  s0 = lw(sp + 0x0050);
  sp += 96; // 0x0060
  return;
}

// size: 0x0000141C
void function_80050BD0(void)
{
  uint32_t temp;
  v0 = lw(0x80075858);
  v1 = lw(0x800758FC);
  sp -= 416; // 0xFFFFFE60
  sw(sp + 0x019C, ra);
  sw(sp + 0x0198, fp);
  sw(sp + 0x0194, s7);
  sw(sp + 0x0190, s6);
  sw(sp + 0x018C, s5);
  sw(sp + 0x0188, s4);
  sw(sp + 0x0184, s3);
  sw(sp + 0x0180, s2);
  sw(sp + 0x017C, s1);
  sw(sp + 0x0178, s0);
  v0 += 2; // 0x0002
  v0 = v0 & 0xFFF;
  v1 += 2; // 0x0002
  v1 = v1 & 0xFFF;
  sw(0x80075858, v0);
  v0 = (int32_t)v1 < 2049;
  sw(0x800758FC, v1);
  temp = v0 != 0;
  v0 = v1 - 4096; // 0xFFFFF000
  if (temp) goto label80050C40;
  sw(0x800758FC, v0);
label80050C40:
  v0 = lw(0x800758FC);
  v0 = (int32_t)v0 < 129;
  temp = v0 != 0;
  v0 = 128; // 0x0080
  if (temp) goto label80050C60;
  sw(0x800758FC, v0);
label80050C60:
  a0 = sp + 16; // 0x0010
  a1 = 0;
  a2 = 32; // 0x0020
  spyro_memset32(a0, a1, a2);
  a0 = sp + 16; // 0x0010
  function_800625F8();
  s0 = 0x80076E1E;
  v0 = lh(s0 + 0x0000);
  a0 = lw(0x800758FC);
  s1 = 4096; // 0x1000
  sh(sp + 0x0010, s1);
  a0 = v0 - a0;
  v0 = spyro_cos(a0);
  v1 = lh(s0 + 0x0000);
  a0 = lw(0x800758FC);
  sh(sp + 0x0018, v0);
  a0 = v1 - a0;
  v0 = spyro_sin(a0);
  v1 = lh(s0 + 0x0000);
  a0 = lw(0x800758FC);
  sh(sp + 0x001E, v0);
  a0 = v1 - a0;
  v0 = spyro_sin(a0);
  v1 = lh(s0 + 0x0000);
  a0 = lw(0x800758FC);
  v0 = -v0;
  sh(sp + 0x001A, v0);
  a0 = v1 - a0;
  v0 = spyro_cos(a0);
  s0 = sp + 48; // 0x0030
  a0 = s0;
  a1 = 0;
  a2 = 32; // 0x0020
  sh(sp + 0x0020, v0);
  spyro_memset32(a0, a1, a2);
  v0 = lh(0x80076E20);
  a0 = lw(0x80075858);
  a0 += v0;
  v0 = spyro_cos(a0);
  v1 = lh(0x80076E20);
  a0 = lw(0x80075858);
  sh(sp + 0x0030, v0);
  a0 += v1;
  v0 = spyro_sin(a0);
  v1 = lh(0x80076E20);
  a0 = lw(0x80075858);
  v0 = -v0;
  sh(sp + 0x003C, v0);
  sh(sp + 0x0038, s1);
  a0 += v1;
  v0 = spyro_sin(a0);
  v1 = lh(0x80076E20);
  a0 = lw(0x80075858);
  sh(sp + 0x0034, v0);
  a0 += v1;
  v0 = spyro_cos(a0);
  a0 = sp + 16; // 0x0010
  a1 = s0;
  sh(sp + 0x0040, v0);
  function_800624E8();;
  a0 = s0;
  a1 = 0;
  a2 = 32; // 0x0020
  spyro_memset32(a0, a1, a2);
  a0 = lh(0x80076E1C);
  v0 = spyro_cos(a0);
  a0 = lh(0x80076E1C);
  sh(sp + 0x0030, v0);
  v0 = spyro_sin(a0);
  a0 = lh(0x80076E1C);
  v0 = -v0;
  sh(sp + 0x0036, v0);
  v0 = spyro_sin(a0);
  a0 = lh(0x80076E1C);
  sh(sp + 0x0032, v0);
  v0 = spyro_cos(a0);
  a0 = sp + 16; // 0x0010
  a1 = s0;
  sh(sp + 0x0038, v0);
  sh(sp + 0x0040, s1);
  function_800624E8();;
  a0 = s0;
  a1 = sp + 16; // 0x0010
  a2 = 20; // 0x0014
  spyro_memcpy32(a0, a1, a2);

  adjust_mat(sp + 0x10);

  spyro_memcpy8(sp + 0x36, sp + 0x16, 6);

  v1 = lw(0x800758BC);
  sw(sp + 0x0108, 0);
  temp = (int32_t)v1 <= 0;
  if (temp) goto label80051F88;
  t4 = sp + 80; // 0x0050
  s5 = 0x80078640;
  sw(sp + 0x0140, t4);
  sw(sp + 0x0170, 0);
label80050E90:
  v0 = lw(s5 + 0x0000);
  v0 = lw(v0 + 0x0014);
  temp = (int32_t)v0 < 0;
  if (temp) goto label80050EC0;
  v0 = lbu(0x800771C8 + v0);
  temp = v0 == 0;
  if (temp) goto label80051F5C;
label80050EC0:
  v0 = lw(0x800758C8);
  t4 = 1; // 0x0001
  sw(sp + 0x0118, t4);
  sw(sp + 0x0110, t4);
  t4 = lw(sp + 0x0108);
  v0 = v0 << 4;
  a0 = t4 << 9;
  a0 += v0;
  v0 = spyro_cos(a0);
  a1 = 0x00FF0000;
  v1 = lw(s5 + 0x0000);
  temp = (int32_t)v0 >= 0;
  a2 = v0;
  if (temp) goto label80050EFC;
  a2 = -a2;
label80050EFC:
  v0 = lw(v1 + 0x0000);
  a2 = (int32_t)a2 >> 1;
  sw(sp + 0x00B8, a2);
  a0 = lw(v0 + 0x0010);
  a1 = a1 | 0xFFFF;
  function_80017E54();;
  a0 = lw(sp + 0x0140);
  a1 = lw(s5 + 0x0000);
  a2 = 0x80076DF8;
  sw(sp + 0x00B8, v0);
  a1 += 32; // 0x0020
  spyro_vec3_sub(a0, a1, a2);
  v1 = lw(sp + 0x0050);
  v0 = lw(sp + 0x0054);
  a0 = lw(sp + 0x0058);
  temp = (int32_t)v1 >= 0;
  if (temp) goto label80050F48;
  v1 = -v1;
label80050F48:
  temp = (int32_t)v0 >= 0;
  if (temp) goto label80050F54;
  v0 = -v0;
label80050F54:
  v1 += v0;
  temp = (int32_t)a0 >= 0;
  if (temp) goto label80050F64;
  a0 = -a0;
label80050F64:
  a0 += v1;
  v0 = spyro_log2_uint(a0);
  s4 = v0;
  v0 = (int32_t)s4 < 15;
  temp = v0 != 0;
  s4 = (int32_t)s4 >> 1;
  if (temp) goto label80050F9C;
  a0 = lw(sp + 0x0140);
  a1 = s4;
  spyro_vec3_shift_right(a0, a1);
  a0 = lw(sp + 0x0140);
  a1 = 1; // 0x0001
  v0 = spyro_vec_length(a0, a1);
  v0 = v0 << s4;
  goto label80050FA8;
label80050F9C:
  a0 = lw(sp + 0x0140);
  a1 = 1; // 0x0001
  v0 = spyro_vec_length(a0, a1);
label80050FA8:
  sw(sp + 0x0138, v0);
  a0 = lw(sp + 0x0138);
  v0 = spyro_log2_uint(a0);
  s4 = v0 - 13; // 0xFFFFFFF3
  temp = (int32_t)s4 >= 0;
  if (temp) goto label80050FC8;
  s4 = 0;
label80050FC8:
  v0 = lw(0x8007591C);
  temp = v0 == 0;
  if (temp) goto label8005102C;
  t4 = lw(sp + 0x0138);
  v0 = (int32_t)t4 < 16385;
  temp = v0 != 0;
  v0 = 0 | 0x8000;
  if (temp) goto label8005102C;
  v0 -= t4;
  s2 = (int32_t)v0 >> 2;
  temp = (int32_t)s2 >= 0;
  v0 = (int32_t)s2 < 4097;
  if (temp) goto label80051008;
  s2 = 0;
  v0 = (int32_t)s2 < 4097;
label80051008:
  temp = v0 != 0;
  if (temp) goto label80051014;
  s2 = 4096; // 0x1000
label80051014:
  a0 = lw(SKYBOX_DATA + 0x0010);
  a1 = lw(sp + 0x00B8);
  a2 = s2;
  function_80017E54();;
  sw(sp + 0x00B8, v0);
label8005102C:
  a0 = sp + 96; // 0x0060
  spyro_vec3_clear(a0);
  v0 = lw(s5 + 0x0000);
  v0 = lw(v0 + 0x0004);
  temp = (int32_t)v0 <= 0;
  s2 = 0;
  if (temp) goto label800510A4;
  s3 = sp + 96; // 0x0060
  s0 = 32; // 0x0020
  s1 = 0x80078DD8;
label8005105C:
  a0 = s1;
  a2 = s4;
  a1 = lw(s5 + 0x0000);
  s1 += 12; // 0x000C
  a1 += s0;
  function_80017B48();
  a0 = s3;
  a2 = lw(s5 + 0x0000);
  a1 = s3;
  a2 += s0;
  spyro_vec3_add(a0, a1, a2);
  v0 = lw(s5 + 0x0000);
  s2++;
  v0 = lw(v0 + 0x0004);
  v0 = (int32_t)s2 < (int32_t)v0;
  temp = v0 != 0;
  s0 += 12; // 0x000C
  if (temp) goto label8005105C;
label800510A4:
  v0 = lw(s5 + 0x0000);
  v1 = lw(sp + 0x0060);
  v0 = lw(v0 + 0x0004);
  div_psx(v1,v0);
  temp = v0 != 0;
  if (temp) goto label800510C4;
  UNREACHABLE; // BREAK 0x01C00
label800510C4:
  at = -1; // 0xFFFFFFFF
  temp = v0 != at;
  at = 0x80000000;
  if (temp) goto label800510DC;
  temp = v1 != at;
  if (temp) goto label800510DC;
  UNREACHABLE; // BREAK 0x01800
label800510DC:
  v1=lo;
  sw(sp + 0x0060, v1);
  v0 = lw(s5 + 0x0000);
  v1 = lw(sp + 0x0064);
  v0 = lw(v0 + 0x0004);
  div_psx(v1,v0);
  temp = v0 != 0;
  if (temp) goto label80051108;
  UNREACHABLE; // BREAK 0x01C00
label80051108:
  at = -1; // 0xFFFFFFFF
  temp = v0 != at;
  at = 0x80000000;
  if (temp) goto label80051120;
  temp = v1 != at;
  if (temp) goto label80051120;
  UNREACHABLE; // BREAK 0x01800
label80051120:
  v1=lo;
  sw(sp + 0x0064, v1);
  v0 = lw(s5 + 0x0000);
  v1 = lw(sp + 0x0068);
  v0 = lw(v0 + 0x0004);
  div_psx(v1,v0);
  temp = v0 != 0;
  if (temp) goto label8005114C;
  UNREACHABLE; // BREAK 0x01C00
label8005114C:
  at = -1; // 0xFFFFFFFF
  temp = v0 != at;
  at = 0x80000000;
  if (temp) goto label80051164;
  temp = v1 != at;
  if (temp) goto label80051164;
  UNREACHABLE; // BREAK 0x01800
label80051164:
  v1=lo;
  a1 = 0x80076DF8;
  sw(sp + 0x0068, v1);
  a2 = lw(s5 + 0x0000);
  a0 = lw(sp + 0x0140);
  a2 += 44; // 0x002C
  spyro_vec3_sub(a0, a1, a2);
  a0 = lw(sp + 0x0140);
  a1 = s4;
  spyro_vec3_shift_right(a0, a1);
  a0 = lw(s5 + 0x0000);
  v0 = lw(sp + 0x0050);
  v1 = lw(a0 + 0x0008);
  v0 = -v0;
  mult(v0, v1);
  v1 = lw(sp + 0x0054);
  a2=lo;
  v0 = lw(a0 + 0x000C);
  mult(v1, v0);
  fp = 5112; // 0x13F8
  s6 = 240; // 0x00F0
  s7 = 0;
  v1 = lw(sp + 0x0058);
  a1=lo;
  v0 = lw(a0 + 0x0010);
  s1 = 0;
  mult(v1, v0);
  s2 = 0;
  s3 = 0;
  s4 = 0;
  sw(0x80075934, 0);
  sw(sp + 0x0128, 0);
  sw(sp + 0x0130, 0);
  v0 = 0x8006FCF4 + 0x0800;
  sw(0x80075798, v0); // &0x00000000
  v0 = lw(s5 + 0x0000);
  v1 = a2 - a1;
  v0 = lw(v0 + 0x0004);
  a3=lo;
  v1 -= a3;
  temp = (int32_t)v0 <= 0;
  sw(sp + 0x0120, v1);
  if (temp) goto label8005155C;
  t3 = s5;
  t0 = 0;
  t2 = 0x80077EAC;
  t1 = 0;
label80051234:
  t4 = lw(sp + 0x0120);
  temp = (int32_t)t4 <= 0;
  a1 = s2 + 1; // 0x0001
  if (temp) goto label80051264;
  v0 = lw(t3 + 0x0000);
  v0 = lw(v0 + 0x0004);
  temp = a1 != v0;
  v0 = a1 << 1;
  if (temp) goto label80051288;
  a1 = 0;
  goto label80051284;
label80051264:
  a1 = s2 - 1; // 0xFFFFFFFF
  temp = (int32_t)a1 >= 0;
  v0 = a1 << 1;
  if (temp) goto label80051288;
  v0 = lw(t3 + 0x0000);
  v0 = lw(v0 + 0x0004);
  a1 = v0 - 1; // 0xFFFFFFFF
label80051284:
  v0 = a1 << 1;
label80051288:
  v0 += a1;
  a2 = v0 << 2;
  v0 = lw(0x80078DD8 + a2);
  t4 = lw(sp + 0x0128);
  v1 = lw(0x80078DDC + a2);
  t4 += v0;
  sw(sp + 0x0128, t4);
  t4 = lw(sp + 0x0130);
  t4 += v1;
  v1 = lw(0x80075934);
  sw(sp + 0x0130, t4);
  a0 = lw(0x80078DE0 + a2);
  v0 = lw(0x80078DD8 + a2);
  v1 += a0;
  v0--;
  v0 = v0 < 511;
  sw(0x80075934, v1);
  temp = v0 == 0;
  v0 = a1 << 1;
  if (temp) goto label8005133C;
  v0 = lw(0x80078DDC + a2);
  v0--;
  v0 = v0 < 239;
  temp = v0 == 0;
  v0 = a1 << 1;
  if (temp) goto label8005133C;
  v0 = lw(0x80078DE0 + a2);
  temp = (int32_t)v0 <= 0;
  v0 = a1 << 1;
  if (temp) goto label8005133C;
  s1 = 1; // 0x0001
label8005133C:
  v0 += a1;
  a3 = v0 << 2;
  a0 = lw(0x80078DD8 + a3);
  v0 = (int32_t)s4 < (int32_t)a0;
  temp = v0 == 0;
  v0 = (int32_t)a0 < (int32_t)fp;
  if (temp) goto label80051364;
  s4 = a0;
label80051364:
  temp = v0 == 0;
  if (temp) goto label80051370;
  fp = a0;
label80051370:
  v1 = lw(0x80078DDC + a3);
  v0 = (int32_t)s7 < (int32_t)v1;
  temp = v0 == 0;
  v0 = (int32_t)v1 < (int32_t)s6;
  if (temp) goto label80051390;
  s7 = v1;
label80051390:
  temp = v0 == 0;
  if (temp) goto label8005139C;
  s6 = v1;
label8005139C:
  a2 = lw(0x80078DD8 + t0);
  v0 = (int32_t)a2 < 512;
  temp = v0 != 0;
  v0 = (int32_t)a0 < 512;
  if (temp) goto label800513C0;
  temp = v0 == 0;
  if (temp) goto label80051540;
label800513C0:
  temp = (int32_t)a2 > 0;
  if (temp) goto label800513D0;
  temp = (int32_t)a0 <= 0;
  if (temp) goto label80051540;
label800513D0:
  a1 = lw(0x80078DDC + t0);
  v0 = (int32_t)a1 < 240;
  temp = v0 != 0;
  v0 = (int32_t)v1 < 240;
  if (temp) goto label800513F4;
  temp = v0 == 0;
  if (temp) goto label80051540;
label800513F4:
  temp = (int32_t)a1 > 0;
  v0 = a2 - a0;
  if (temp) goto label80051404;
  temp = (int32_t)v1 <= 0;
  if (temp) goto label80051540;
label80051404:
  temp = (int32_t)v0 >= 0;
  if (temp) goto label80051410;
  v0 = -v0;
label80051410:
  v0 = (int32_t)v0 < 3;
  temp = v0 != 0;
  v0 = a1 - v1;
  if (temp) goto label80051420;
  sw(sp + 0x0110, 0);
label80051420:
  temp = (int32_t)v0 >= 0;
  if (temp) goto label8005142C;
  v0 = -v0;
label8005142C:
  v0 = (int32_t)v0 < 3;
  temp = v0 != 0;
  v0 = 1; // 0x0001
  if (temp) goto label8005143C;
  sw(sp + 0x0118, 0);
label8005143C:
  sw(0x80077EA0 + t1, v0);
  v0 = lw(0x80078DDC + t0);
  v1 = lhu(0x80078DD8 + t0);
  v0 = v0 << 16;
  v0 = v0 | v1;
  sw(0x80077EA4 + t1, v0);
  v0 = lw(0x80078DDC + a3);
  v1 = lhu(0x80078DD8 + a3);
  v0 = v0 << 16;
  v0 = v0 | v1;
  sw(0x80077EA8 + t1, v0);
  v0 = lw(0x80078DDC + t0);
  v1 = lw(0x80078DDC + a3);
  v0 -= v1;
  sw(t2 + 0x0000, v0);
  a0 = lw(0x80078DD8 + a3);
  v0 = lw(0x80078DD8 + t0);
  a0 -= v0;
  sw(0x80077EB0 + t1, a0);
  v0 = lw(t2 + 0x0000);
  v1 = lw(0x80078DD8 + t0);
  v0 = -v0;
  mult(v0, v1);
  v1=lo;
  v0 = lw(0x80078DDC + t0);
  mult(a0, v0);
  s3++;
  t2 += 24; // 0x0018
  a3=lo;
  v0 = v1 - a3;
  sw(0x80077EB4 + t1, v0);
  t1 += 24; // 0x0018
label80051540:
  v0 = lw(t3 + 0x0000);
  v0 = lw(v0 + 0x0004);
  s2++;
  v0 = (int32_t)s2 < (int32_t)v0;
  temp = v0 != 0;
  t0 += 12; // 0x000C
  if (temp) goto label80051234;
label8005155C:
  t4 = lw(sp + 0x0110);
  temp = t4 != 0;
  if (temp) goto label8005157C;
  t4 = lw(sp + 0x0118);
  temp = t4 == 0;
  if (temp) goto label80051584;
label8005157C:
  temp = s3 != 0;
  if (temp) goto label80051F5C;
label80051584:
  v0 = lw(s5 + 0x0000);
  t4 = lw(sp + 0x0128);
  v0 = lw(v0 + 0x0004);
  div_psx(t4,v0);
  temp = v0 != 0;
  if (temp) goto label800515A4;
  UNREACHABLE; // BREAK 0x01C00
label800515A4:
  at = -1; // 0xFFFFFFFF
  temp = v0 != at;
  at = 0x80000000;
  if (temp) goto label800515BC;
  temp = t4 != at;
  if (temp) goto label800515BC;
  UNREACHABLE; // BREAK 0x01800
label800515BC:
  t4=lo;
  v1 = lw(0x80075934);
  div_psx(v1,v0);
  temp = v0 != 0;
  if (temp) goto label800515DC;
  UNREACHABLE; // BREAK 0x01C00
label800515DC:
  at = -1; // 0xFFFFFFFF
  temp = v0 != at;
  at = 0x80000000;
  if (temp) goto label800515F4;
  temp = v1 != at;
  if (temp) goto label800515F4;
  UNREACHABLE; // BREAK 0x01800
label800515F4:
  v1=lo;
  sw(sp + 0x0128, t4);
  t4 = lw(sp + 0x0130);
  div_psx(t4,v0);
  temp = v0 != 0;
  if (temp) goto label80051614;
  UNREACHABLE; // BREAK 0x01C00
label80051614:
  at = -1; // 0xFFFFFFFF
  temp = v0 != at;
  at = 0x80000000;
  if (temp) goto label8005162C;
  temp = t4 != at;
  if (temp) goto label8005162C;
  UNREACHABLE; // BREAK 0x01800
label8005162C:
  t4=lo;
  v1 = (int32_t)v1 >> 7;
  v0 = (int32_t)v1 < 256;
  sw(0x80075934, v1);
  temp = v0 != 0;
  sw(sp + 0x0130, t4);
  if (temp) goto label80051654;
  v0 = v1 + 64; // 0x0040
  sw(0x80075934, v0);
label80051654:
  v0 = lw(0x80075934);
  v0 = (int32_t)v0 < 2048;
  temp = v0 != 0;
  v0 = (int32_t)fp < 512;
  if (temp) goto label8005167C;
  v0 = 2047; // 0x07FF
  sw(0x80075934, v0);
  v0 = (int32_t)fp < 512;
label8005167C:
  temp = v0 == 0;
  v0 = (int32_t)s6 < 240;
  if (temp) goto label800516B8;
  temp = v0 == 0;
  if (temp) goto label800516B8;
  temp = (int32_t)s4 <= 0;
  if (temp) goto label800516B8;
  temp = (int32_t)s7 <= 0;
  if (temp) goto label800516B8;
  v0 = lw(0x80075934);
  temp = (int32_t)v0 <= 0;
  if (temp) goto label800516B8;
  temp = s3 != 0;
  if (temp) goto label800516EC;
label800516B8:
  temp = (int32_t)fp > 0;
  if (temp) goto label800516F4;
  temp = (int32_t)s6 > 0;
  v0 = (int32_t)s4 < 512;
  if (temp) goto label800516F4;
  temp = v0 != 0;
  v0 = (int32_t)s7 < 240;
  if (temp) goto label800516F4;
  temp = v0 != 0;
  if (temp) goto label800516F4;
  v0 = lw(0x80075934);
  temp = (int32_t)v0 <= 0;
  if (temp) goto label800516F4;
label800516EC:
  s1 = s1 | 0x2;
  goto label800516F8;
label800516F4:
  s1 = 0;
label800516F8:
  v0 = 2; // 0x0002
  temp = s1 != v0;
  if (temp) goto label800517DC;
  t4 = lw(sp + 0x0138);
  v0 = (int32_t)t4 < 4096;
  temp = v0 == 0;
  a0 = sp + 128; // 0x0080
  if (temp) goto label800517DC;
  a2 = 0x80076DF8;
  a1 = sp + 96; // 0x0060
  spyro_vec3_sub(a0, a1, a2);
  a0 = sp + 144; // 0x0090
  s0 = sp + 152; // 0x0098
  a1 = s0;
  a3 = lhu(0x80076E1C);
  v0 = 4096; // 0x1000
  sw(sp + 0x0070, v0);
  v0 = lhu(0x80076E1E);
  v1 = lhu(0x80076E20);
  a2 = 0;
  sw(sp + 0x0074, 0);
  sw(sp + 0x0078, 0);
  a3 = a3 >> 4;
  v0 = v0 >> 4;
  v1 = v1 >> 4;
  sb(sp + 0x0090, a3);
  sb(sp + 0x0091, v0);
  sb(sp + 0x0092, v1);
  spyro_mat3_rotation(a0, a1, a2);
  a0 = s0;
  a1 = sp + 112; // 0x0070
  a2 = a1;
  spyro_set_mat_mirrored_vec_multiply(a0, a1, a2);
  v1 = lw(sp + 0x0070);
  v0 = lw(sp + 0x0080);
  mult(v1, v0);
  v1 = lw(sp + 0x0074);
  a1=lo;
  v0 = lw(sp + 0x0084);
  mult(v1, v0);
  v1 = lw(sp + 0x0078);
  a0=lo;
  v0 = lw(sp + 0x0088);
  mult(v1, v0);
  v0 = a1 + a0;
  v1=lo;
  v0 += v1;
  temp = (int32_t)v0 >= 0;
  if (temp) goto label800517DC;
  s1 = 0;
label800517DC:
  temp = s1 == 0;
  if (temp) goto label800518CC;
  temp = (int32_t)fp >= 0;
  if (temp) goto label800517F0;
  fp = 0;
label800517F0:
  temp = (int32_t)s6 >= 0;
  v0 = (int32_t)s4 < 513;
  if (temp) goto label800517FC;
  s6 = 0;
label800517FC:
  temp = v0 != 0;
  v0 = (int32_t)s7 < 241;
  if (temp) goto label80051808;
  s4 = 512; // 0x0200
label80051808:
  temp = v0 != 0;
  v0 = s6 << 16;
  if (temp) goto label80051814;
  s7 = 240; // 0x00F0
label80051814:
  sw(0x8007AA00, v0);
  v0 = s7 << 16;
  sw(0x8007AA04, v0);
  v0 = s4 << 16;
  sw(0x8007AA08, v0);
  v0 = fp << 16;
  sw(0x8007AA0C, v0);
  v0 = s3 << 1;
  v0 += s3;
  v0 = v0 << 3;
  sw(0x80077EA0 + v0, 0);
  t4 = lw(sp + 0x0138);
  v0 = (int32_t)t4 < 16384;
  temp = v0 == 0;
  v0 = (int32_t)t4 < 12289;
  if (temp) goto label800518C0;
  temp = v0 != 0;
  a2 = t4 - 12288; // 0xFFFFD000
  if (temp) goto label800518AC;
  v0 = lw(s5 + 0x0000);
  a1 = lw(sp + 0x00B8);
  v0 = lw(v0 + 0x0000);
  sw(0x8007575C, a2); // &0x00000000
  a0 = lw(v0 + 0x0010);
  function_80017E54();;
  a0 = 0x800757D4; // &0x00000000
  sw(0x800757D4, v0); // &0x00000000
  goto label800518C4;
label800518AC:
  v0 = lw(s5 + 0x0000);
  a0 = lw(v0 + 0x0000);
  a0 += 16; // 0x0010
  goto label800518C4;
label800518C0:
  a0 = sp + 184; // 0x00B8
label800518C4:
  function_8004FEA0();
label800518CC:
  t4 = lw(sp + 0x0138);
  v0 = (int32_t)t4 < 16384;
  temp = v0 == 0;
  fp = 512; // 0x0200
  if (temp) goto label80051F5C;
  s4 = 0;
  s6 = 240; // 0x00F0
  s7 = 0;
  s1 = 0;
  s2 = 0;
  v0 = lw(s5 + 0x0000);
  v1 = lw(v0 + 0x0004);
  v0 = 0x8006FCF4 + 0x0800;
  sw(0x80075798, v0); // &0x00000000
  temp = (int32_t)v1 <= 0;
  s3 = 0;
  if (temp) goto label80051A04;
  v0 = 0x80078640;
  t4 = lw(sp + 0x0170);
  a1 = 0x80078DD8;
  a0 = 0;
  a2 = t4 + v0;
label80051934:
  v1 = lw(a1 + 0x0000);
  t4 = lw(sp + 0x0128);
  v0 = (int32_t)t4 < (int32_t)v1;
  temp = v0 != 0;
  v0 = v1 + 2; // 0x0002
  if (temp) goto label80051958;
  v0 = (int32_t)v1 < (int32_t)t4;
  temp = v0 == 0;
  v0 = v1 - 2; // 0xFFFFFFFE
  if (temp) goto label8005195C;
label80051958:
  sw(a1 + 0x0000, v0);
label8005195C:
  v1 = lw(0x80078DDC + a0);
  t4 = lw(sp + 0x0130);
  v0 = (int32_t)t4 < (int32_t)v1;
  temp = v0 != 0;
  v0 = v1 + 2; // 0x0002
  if (temp) goto label80051988;
  v0 = (int32_t)v1 < (int32_t)t4;
  temp = v0 == 0;
  v0 = v1 - 2; // 0xFFFFFFFE
  if (temp) goto label80051994;
label80051988:
  sw(0x80078DDC + a0, v0);
label80051994:
  v1 = lw(a1 + 0x0000);
  v0 = (int32_t)s4 < (int32_t)v1;
  temp = v0 == 0;
  v0 = (int32_t)v1 < (int32_t)fp;
  if (temp) goto label800519AC;
  s4 = v1;
label800519AC:
  temp = v0 == 0;
  if (temp) goto label800519B8;
  fp = v1;
label800519B8:
  v1 = lw(0x80078DDC + a0);
  v0 = (int32_t)s7 < (int32_t)v1;
  temp = v0 == 0;
  v0 = (int32_t)v1 < (int32_t)s6;
  if (temp) goto label800519D8;
  s7 = v1;
label800519D8:
  temp = v0 == 0;
  if (temp) goto label800519E4;
  s6 = v1;
label800519E4:
  a1 += 12; // 0x000C
  v0 = lw(a2 + 0x0000);
  v0 = lw(v0 + 0x0004);
  s2++;
  v0 = (int32_t)s2 < (int32_t)v0;
  temp = v0 != 0;
  a0 += 12; // 0x000C
  if (temp) goto label80051934;
label80051A04:
  v0 = lw(s5 + 0x0000);
  v0 = lw(v0 + 0x0004);
  temp = (int32_t)v0 <= 0;
  s2 = 0;
  if (temp) goto label80051D0C;
  t1 = s5;
  a2 = 0;
  v0 = s3 << 1;
  v0 += s3;
  v0 = v0 << 3;
  v1 = 0x80077EAC;
  t0 = v0 + v1;
  a3 = v0;
label80051A40:
  t4 = lw(sp + 0x0120);
  temp = (int32_t)t4 <= 0;
  a1 = s2 + 1; // 0x0001
  if (temp) goto label80051A70;
  v0 = lw(s5 + 0x0000);
  v0 = lw(v0 + 0x0004);
  temp = a1 != v0;
  v0 = a1 << 1;
  if (temp) goto label80051A94;
  a1 = 0;
  goto label80051A90;
label80051A70:
  a1 = s2 - 1; // 0xFFFFFFFF
  temp = (int32_t)a1 >= 0;
  v0 = a1 << 1;
  if (temp) goto label80051A94;
  v0 = lw(t1 + 0x0000);
  v0 = lw(v0 + 0x0004);
  a1 = v0 - 1; // 0xFFFFFFFF
label80051A90:
  v0 = a1 << 1;
label80051A94:
  v0 += a1;
  v1 = v0 << 2;
  v0 = lw(0x80078DD8 + v1);
  v0--;
  v0 = v0 < 511;
  temp = v0 == 0;
  if (temp) goto label80051AF8;
  v0 = lw(0x80078DDC + v1);
  v0--;
  v0 = v0 < 239;
  temp = v0 == 0;
  if (temp) goto label80051AF8;
  v0 = lw(0x80078DE0 + v1);
  temp = (int32_t)v0 <= 0;
  if (temp) goto label80051AF8;
  s1 = 1; // 0x0001
label80051AF8:
  v1 = lw(0x80078DD8 + a2);
  v0 = (int32_t)v1 < 512;
  temp = v0 != 0;
  v0 = a1 << 1;
  if (temp) goto label80051B38;
  v0 += a1;
  v0 = v0 << 2;
  v0 = lw(0x80078DD8 + v0);
  v0 = (int32_t)v0 < 512;
  temp = v0 == 0;
  if (temp) goto label80051CDC;
label80051B38:
  temp = (int32_t)v1 > 0;
  v0 = a1 << 1;
  if (temp) goto label80051B60;
  v0 += a1;
  v0 = v0 << 2;
  v0 = lw(0x80078DD8 + v0);
  temp = (int32_t)v0 <= 0;
  if (temp) goto label80051CDC;
label80051B60:
  v1 = lw(0x80078DDC + a2);
  v0 = (int32_t)v1 < 240;
  temp = v0 != 0;
  v0 = a1 << 1;
  if (temp) goto label80051BA0;
  v0 += a1;
  v0 = v0 << 2;
  v0 = lw(0x80078DDC + v0);
  v0 = (int32_t)v0 < 240;
  temp = v0 == 0;
  if (temp) goto label80051CDC;
label80051BA0:
  temp = (int32_t)v1 > 0;
  v0 = 1; // 0x0001
  if (temp) goto label80051BCC;
  v0 = a1 << 1;
  v0 += a1;
  v0 = v0 << 2;
  v0 = lw(0x80078DDC + v0);
  temp = (int32_t)v0 <= 0;
  v0 = 1; // 0x0001
  if (temp) goto label80051CDC;
label80051BCC:
  a0 = a1 << 1;
  a0 += a1;
  a0 = a0 << 2;
  sw(0x80077EA0 + a3, v0);
  v0 = lw(0x80078DDC + a2);
  v1 = lhu(0x80078DD8 + a2);
  v0 = v0 << 16;
  v0 = v0 | v1;
  sw(0x80077EA4 + a3, v0);
  v0 = lw(0x80078DDC + a0);
  v1 = lhu(0x80078DD8 + a0);
  v0 = v0 << 16;
  v0 = v0 | v1;
  sw(0x80077EA8 + a3, v0);
  v0 = lw(0x80078DDC + a2);
  v1 = lw(0x80078DDC + a0);
  v0 -= v1;
  sw(t0 + 0x0000, v0);
  a0 = lw(0x80078DD8 + a0);
  v0 = lw(0x80078DD8 + a2);
  a0 -= v0;
  sw(0x80077EB0 + a3, a0);
  v0 = lw(t0 + 0x0000);
  v1 = lw(0x80078DD8 + a2);
  v0 = -v0;
  mult(v0, v1);
  v1=lo;
  v0 = lw(0x80078DDC + a2);
  mult(a0, v0);
  s3++;
  t0 += 24; // 0x0018
  t2=lo;
  v0 = v1 - t2;
  sw(0x80077EB4 + a3, v0);
  a3 += 24; // 0x0018
label80051CDC:
  t4 = lw(sp + 0x0108);
  v0 = t4 << 2;
  v0 = lw(0x80078640 + v0);
  v0 = lw(v0 + 0x0004);
  s2++;
  v0 = (int32_t)s2 < (int32_t)v0;
  temp = v0 != 0;
  a2 += 12; // 0x000C
  if (temp) goto label80051A40;
label80051D0C:
  v0 = (int32_t)fp < 512;
  temp = v0 == 0;
  v0 = (int32_t)s6 < 240;
  if (temp) goto label80051D4C;
  temp = v0 == 0;
  if (temp) goto label80051D4C;
  temp = (int32_t)s4 <= 0;
  if (temp) goto label80051D4C;
  temp = (int32_t)s7 <= 0;
  if (temp) goto label80051D4C;
  v0 = lw(0x80075934);
  temp = (int32_t)v0 <= 0;
  if (temp) goto label80051D4C;
  temp = s3 != 0;
  if (temp) goto label80051D80;
label80051D4C:
  temp = (int32_t)fp > 0;
  if (temp) goto label80051D88;
  temp = (int32_t)s6 > 0;
  v0 = (int32_t)s4 < 512;
  if (temp) goto label80051D88;
  temp = v0 != 0;
  v0 = (int32_t)s7 < 240;
  if (temp) goto label80051D88;
  temp = v0 != 0;
  if (temp) goto label80051D88;
  v0 = lw(0x80075934);
  temp = (int32_t)v0 <= 0;
  if (temp) goto label80051D88;
label80051D80:
  s1 = s1 | 0x2;
  goto label80051D8C;
label80051D88:
  s1 = 0;
label80051D8C:
  v0 = 2; // 0x0002
  temp = s1 != v0;
  if (temp) goto label80051E70;
  t4 = lw(sp + 0x0138);
  v0 = (int32_t)t4 < 4096;
  temp = v0 == 0;
  a0 = sp + 208; // 0x00D0
  if (temp) goto label80051E70;
  a2 = 0x80076DF8;
  a1 = sp + 96; // 0x0060
  spyro_vec3_sub(a0, a1, a2);
  a0 = sp + 224; // 0x00E0
  s0 = sp + 232; // 0x00E8
  a1 = s0;
  a3 = lhu(0x80076E1C);
  v0 = 4096; // 0x1000
  sw(sp + 0x00C0, v0);
  v0 = lhu(0x80076E1E);
  v1 = lhu(0x80076E20);
  a2 = 0;
  sw(sp + 0x00C4, 0);
  sw(sp + 0x00C8, 0);
  a3 = a3 >> 4;
  v0 = v0 >> 4;
  v1 = v1 >> 4;
  sb(sp + 0x00E0, a3);
  sb(sp + 0x00E1, v0);
  sb(sp + 0x00E2, v1);
  spyro_mat3_rotation(a0, a1, a2);
  a0 = s0;
  a1 = sp + 192; // 0x00C0
  a2 = a1;
  spyro_set_mat_mirrored_vec_multiply(a0, a1, a2);
  v1 = lw(sp + 0x00C0);
  v0 = lw(sp + 0x00D0);
  mult(v1, v0);
  v1 = lw(sp + 0x00C4);
  a1=lo;
  v0 = lw(sp + 0x00D4);
  mult(v1, v0);
  v1 = lw(sp + 0x00C8);
  a0=lo;
  v0 = lw(sp + 0x00D8);
  mult(v1, v0);
  v0 = a1 + a0;
  v1=lo;
  v0 += v1;
  temp = (int32_t)v0 >= 0;
  if (temp) goto label80051E70;
  s1 = 0;
label80051E70:
  temp = s1 == 0;
  if (temp) goto label80051F5C;
  temp = (int32_t)fp >= 0;
  if (temp) goto label80051E84;
  fp = 0;
label80051E84:
  temp = (int32_t)s6 >= 0;
  v0 = (int32_t)s4 < 513;
  if (temp) goto label80051E90;
  s6 = 0;
label80051E90:
  temp = v0 != 0;
  v0 = (int32_t)s7 < 241;
  if (temp) goto label80051E9C;
  s4 = 512; // 0x0200
label80051E9C:
  temp = v0 != 0;
  v0 = s6 << 16;
  if (temp) goto label80051EA8;
  s7 = 240; // 0x00F0
label80051EA8:
  sw(0x8007AA00, v0);
  v0 = s7 << 16;
  sw(0x8007AA04, v0);
  v0 = s4 << 16;
  sw(0x8007AA08, v0);
  v0 = fp << 16;
  sw(0x8007AA0C, v0);
  v0 = s3 << 1;
  v0 += s3;
  v0 = v0 << 3;
  sw(0x80077EA0 + v0, 0);
  t4 = lw(sp + 0x0138);
  v0 = (int32_t)t4 < 12289;
  temp = v0 != 0;
  a1 = sp + 16; // 0x0010
  if (temp) goto label80051F48;
  v0 = lw(s5 + 0x0000);
  a1 = lw(sp + 0x00B8);
  v0 = lw(v0 + 0x0000);
  a2 = t4 - 12288; // 0xFFFFD000
  sw(0x8007575C, a2); // &0x00000000
  a0 = lw(v0 + 0x0010);
  function_80017E54();;
  v1 = lw(s5 + 0x0000);
  a1 = sp + 16; // 0x0010
  a0 = lw(v1 + 0x0000);
  sw(0x800757D4, v0); // &0x00000000
  a2 = sp + 48; // 0x0030
  function_80050240();
  goto label80051F5C;
label80051F48:
  v0 = lw(s5 + 0x0000);
  a0 = lw(v0 + 0x0000);
  a2 = sp + 48; // 0x0030
  function_8004F4BC();
label80051F5C:
  t4 = lw(sp + 0x0170);
  s5 += 4; // 0x0004
  t4 += 4; // 0x0004
  sw(sp + 0x0170, t4);
  t4 = lw(sp + 0x0108);
  v0 = lw(0x800758BC);
  t4++;
  v0 = (int32_t)t4 < (int32_t)v0;
  temp = v0 != 0;
  sw(sp + 0x0108, t4);
  if (temp) goto label80050E90;
label80051F88:
  a2 = 0x80076E24;
  a0 = lw(a2 + 0x0000);
  v0 = lw(0x800785A8 + 0x000C);
  v0 = (int32_t)a0 < (int32_t)v0;
  temp = v0 != 0;
  a1 = a2 - 64; // 0xFFFFFFC0
  if (temp) goto label80051FB0;
  a0 = -1; // 0xFFFFFFFF
label80051FB0:
  a2 -= 84; // 0xFFFFFFAC
  function_8004EBA8();
  ra = lw(sp + 0x019C);
  fp = lw(sp + 0x0198);
  s7 = lw(sp + 0x0194);
  s6 = lw(sp + 0x0190);
  s5 = lw(sp + 0x018C);
  s4 = lw(sp + 0x0188);
  s3 = lw(sp + 0x0184);
  s2 = lw(sp + 0x0180);
  s1 = lw(sp + 0x017C);
  s0 = lw(sp + 0x0178);
  sp += 416; // 0x01A0
  return;
}

*/