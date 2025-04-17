#include "extra_gte.h"
#include "int_math.h"
#include "psx_ops.h"
#include "decompilation.h"
#include "gte.h"
#include "main.h"
#include "int_math.h"
#include "spyro_math.h"

// size: 0x0000010C
void spyro_mat_mul(uint32_t m1, uint32_t m2, uint32_t dst)
{
  save_mat3(dst, mat3_mul(load_mat3(m1), load_mat3(m2)));
}

void function_800623D8(void)
{
  BREAKPOINT;
  spyro_mat_mul(a0, a1, a2);
}

int16_t spyro_atan(int32_t x, int32_t y)
{
  uint32_t at = abs_int(x);
  uint32_t v0 = abs_int(y);
  if (at > v0)
  {
    uint32_t tmp = at;
    at = v0;
    v0 = tmp;
  }
  at = at*64;
  if (v0 == 0)
    v0 = 1;
  
  uint32_t lo = at/v0;
  if (x >= 0)
    if (y >= 0)
      if (x >= y) {
        at = 0;
        v0 = 0;
      } else {
        at = 1;
        v0 = 0x40;
      }
    else
      if (x >= -y) {
        at = 1;
        v0 = 0x100;
      } else {
        at = 0;
        v0 = 0xC0;
      }
  else
    if (y >= 0)
      if (-x >= y) {
        at = 1;
        v0 = 0x80;
      } else {
        at = 0;
        v0 = 0x40;
      }
    else
      if (-x >= -y) {
        at = 0;
        v0 = 0x80;
      } else {
        at = 1;
        v0 = 0xC0;
      }

  const int arr[64] = {
    0x00, 0x01, 0x01, 0x02,
    0x03, 0x03, 0x04, 0x04,
    0x05, 0x06, 0x06, 0x07,
    0x08, 0x08, 0x09, 0x09,
    0x0a, 0x0b, 0x0b, 0x0c,
    0x0c, 0x0d, 0x0d, 0x0e,
    0x0f, 0x0f, 0x10, 0x10,
    0x11, 0x11, 0x12, 0x12,
    0x13, 0x13, 0x14, 0x14,
    0x15, 0x15, 0x16, 0x16,
    0x17, 0x17, 0x18, 0x18,
    0x19, 0x19, 0x19, 0x1a,
    0x1a, 0x1b, 0x1b, 0x1b,
    0x1c, 0x1c, 0x1d, 0x1d,
    0x1d, 0x1e, 0x1e, 0x1e,
    0x1f, 0x1f, 0x1f, 0x20
  };

  uint32_t tmp = arr[lo];
  if (at == 0) {
    at = v0 + tmp;
  } else {
    at = v0 - tmp;
  }
  return at;
}

// size: 0x00000108
void function_800169AC(void)
{
  BREAKPOINT;
  v0 = spyro_atan(a0, a1);
}

uint32_t math_lut[] = {
  0xFFFFFFFF,
  0x00000324,
  0x0000096E,
  0x00000FBA,
  0x0000160C,
  0x00001C64,
  0x000022C5,
  0x00002931,
  0x00002FAA,
  0x00003632,
  0x00003CCC,
  0x00004379,
  0x00004A3D,
  0x00005119,
  0x00005811,
  0x00005F28,
  0x00006660,
  0x00006DBD,
  0x00007542,
  0x00007CF2,
  0x000084D7,
  0x00008CE7,
  0x00009534,
  0x00009DBE,
  0x0000A68C,
  0x0000AFA3,
  0x0000B909,
  0x0000C2C7,
  0x0000CCE3,
  0x0000D768,
  0x0000E25E,
  0x0000EDD0,
  0x0000F9CB,
  0x00010001
};

// size: 0x000001A4
int16_t spyro_atan2(int32_t x, int32_t y, uint32_t a3)
{
  int32_t at = abs_int(x);
  int32_t v0 = abs_int(y);
  cop2.LZCS = at | v0;
  int32_t v1 = 17 - LZCR();
  if (v1 > 0) {
    at = at >> v1;
    v0 = v0 >> v1;
  }
  if (at > v0) {
    v1 = at;
    at = v0;
    v0 = v1;
  }
  if (v0 == 0)
    return 0;

  at = at << 16;
  div_psx(at,v0);
  uint32_t negative;
  if (x >= 0)
    if (y >= 0)
      if (x >= y) {
        negative = 0;
        v0 = 0;
      } else {
        negative = 1;
        v0 = 0x40;
      }
    else
      if (x >= -y) {
        negative = 1;
        v0 = 0x100;
      } else {
        negative = 0;
        v0 = 0xC0;
      }
  else
    if (y >= 0)
      if (-x >= y) {
        negative = 1;
        v0 = 0x80;
      } else {
        negative = 0;
        v0 = 0x40;
      }
    else
      if (-x >= -y) {
        negative = 0;
        v0 = 0x80;
      } else {
        negative = 1;
        v0 = 0xC0;
      }
  
  v1 = 0x8006CE7C;
  a0=lo;
  a1 = a0 >> 11;
  do {
    a1++;
    a2 = lw(0x8006CE7C + a1*4) - a0;
  } while (a2 < 0);
  a1--;
  v1 = a1;

  // extra precision maybe
  if (a3) {
    a1 = a1*2;
    v1 = lhu(0x8006CF04 + a1);
    a2 = a2 << 16;
    if (a1 != 64) 
      a1 += 1;
    v1 = a1*8 - ((int32_t)a2/v1 >> 12);

    v0 = v0*16;
  }

  if (negative)
    v1 = -v1;
  v0 += v1;
  return v0;
}

// size: 0x000001A4
void function_80016AB4(void)
{
  BREAKPOINT;
  v0 = spyro_atan2(a0, a1, a2);
}

int16_t cos_lut[256] = {
  0x1000, 0x0FFF, 0x0FFB, 0x0FF5, 0x0FEC, 0x0FE1, 0x0FD4, 0x0FC4,
  0x0FB1, 0x0F9C, 0x0F85, 0x0F6C, 0x0F50, 0x0F31, 0x0F11, 0x0EEE,
  0x0EC8, 0x0EA1, 0x0E77, 0x0E4B, 0x0E1C, 0x0DEC, 0x0DB9, 0x0D85, 
  0x0D4E, 0x0D15, 0x0CDA, 0x0C9D, 0x0C5E, 0x0C1E, 0x0BDB, 0x0B97, 
  0x0B50, 0x0B08, 0x0ABF, 0x0A73, 0x0A26, 0x09D8, 0x0988, 0x0937, 
  0x08E4, 0x088F, 0x083A, 0x07E3, 0x078B, 0x0732, 0x06D7, 0x067C, 
  0x061F, 0x05C2, 0x0564, 0x0505, 0x04A5, 0x0444, 0x03E3, 0x0381, 
  0x031F, 0x02BC, 0x0259, 0x01F5, 0x0191, 0x012D, 0x00C9, 0x0065, 
  0x0000, 0xFF9B, 0xFF37, 0xFED3, 0xFE6F, 0xFE0B, 0xFDA7, 0xFD44, 
  0xFCE1, 0xFC7F, 0xFC1D, 0xFBBC, 0xFB5B, 0xFAFB, 0xFA9C, 0xFA3E, 
  0xF9E1, 0xF984, 0xF929, 0xF8CE, 0xF875, 0xF81D, 0xF7C6, 0xF771, 
  0xF71C, 0xF6C9, 0xF678, 0xF628, 0xF5DA, 0xF58D, 0xF541, 0xF4F8, 
  0xF4B0, 0xF469, 0xF425, 0xF3E2, 0xF3A2, 0xF363, 0xF326, 0xF2EB, 
  0xF2B2, 0xF27B, 0xF247, 0xF214, 0xF1E4, 0xF1B5, 0xF189, 0xF15F, 
  0xF138, 0xF112, 0xF0EF, 0xF0CF, 0xF0B0, 0xF094, 0xF07B, 0xF064, 
  0xF04F, 0xF03C, 0xF02C, 0xF01F, 0xF014, 0xF00B, 0xF005, 0xF001, 
  0xF000, 0xF001, 0xF005, 0xF00B, 0xF014, 0xF01F, 0xF02C, 0xF03C,
  0xF04F, 0xF064, 0xF07B, 0xF094, 0xF0B0, 0xF0CF, 0xF0EF, 0xF112,
  0xF138, 0xF15F, 0xF189, 0xF1B5, 0xF1E4, 0xF214, 0xF247, 0xF27B,
  0xF2B2, 0xF2EB, 0xF326, 0xF363, 0xF3A2, 0xF3E2, 0xF425, 0xF469,
  0xF4B0, 0xF4F8, 0xF541, 0xF58D, 0xF5DA, 0xF628, 0xF678, 0xF6C9,
  0xF71C, 0xF771, 0xF7C6, 0xF81D, 0xF875, 0xF8CE, 0xF929, 0xF984,
  0xF9E1, 0xFA3E, 0xFA9C, 0xFAFB, 0xFB5B, 0xFBBC, 0xFC1D, 0xFC7F,
  0xFCE1, 0xFD44, 0xFDA7, 0xFE0B, 0xFE6F, 0xFED3, 0xFF37, 0xFF9B,
  0x0000, 0x0065, 0x00C9, 0x012D, 0x0191, 0x01F5, 0x0259, 0x02BC,
  0x031F, 0x0381, 0x03E3, 0x0444, 0x04A5, 0x0505, 0x0564, 0x05C2,
  0x061F, 0x067C, 0x06D7, 0x0732, 0x078B, 0x07E3, 0x083A, 0x088F,
  0x08E4, 0x0937, 0x0988, 0x09D8, 0x0A26, 0x0A73, 0x0ABF, 0x0B08,
  0x0B50, 0x0B97, 0x0BDB, 0x0C1E, 0x0C5E, 0x0C9D, 0x0CDA, 0x0D15,
  0x0D4E, 0x0D85, 0x0DB9, 0x0DEC, 0x0E1C, 0x0E4B, 0x0E77, 0x0EA1,
  0x0EC8, 0x0EEE, 0x0F11, 0x0F31, 0x0F50, 0x0F6C, 0x0F85, 0x0F9C,
  0x0FB1, 0x0FC4, 0x0FD4, 0x0FE1, 0x0FEC, 0x0FF5, 0x0FFB, 0x0FFF,
};

int16_t sin_lut[256] = {
  0x0000, 0x0065, 0x00C9, 0x012D, 0x0191, 0x01F5, 0x0259, 0x02BC,
  0x031F, 0x0381, 0x03E3, 0x0444, 0x04A5, 0x0505, 0x0564, 0x05C2,
  0x061F, 0x067C, 0x06D7, 0x0732, 0x078B, 0x07E3, 0x083A, 0x088F,
  0x08E4, 0x0937, 0x0988, 0x09D8, 0x0A26, 0x0A73, 0x0ABF, 0x0B08,
  0x0B50, 0x0B97, 0x0BDB, 0x0C1E, 0x0C5E, 0x0C9D, 0x0CDA, 0x0D15,
  0x0D4E, 0x0D85, 0x0DB9, 0x0DEC, 0x0E1C, 0x0E4B, 0x0E77, 0x0EA1,
  0x0EC8, 0x0EEE, 0x0F11, 0x0F31, 0x0F50, 0x0F6C, 0x0F85, 0x0F9C,
  0x0FB1, 0x0FC4, 0x0FD4, 0x0FE1, 0x0FEC, 0x0FF5, 0x0FFB, 0x0FFF,
  0x1000, 0x0FFF, 0x0FFB, 0x0FF5, 0x0FEC, 0x0FE1, 0x0FD4, 0x0FC4,
  0x0FB1, 0x0F9C, 0x0F85, 0x0F6C, 0x0F50, 0x0F31, 0x0F11, 0x0EEE,
  0x0EC8, 0x0EA1, 0x0E77, 0x0E4B, 0x0E1C, 0x0DEC, 0x0DB9, 0x0D85, 
  0x0D4E, 0x0D15, 0x0CDA, 0x0C9D, 0x0C5E, 0x0C1E, 0x0BDB, 0x0B97, 
  0x0B50, 0x0B08, 0x0ABF, 0x0A73, 0x0A26, 0x09D8, 0x0988, 0x0937, 
  0x08E4, 0x088F, 0x083A, 0x07E3, 0x078B, 0x0732, 0x06D7, 0x067C, 
  0x061F, 0x05C2, 0x0564, 0x0505, 0x04A5, 0x0444, 0x03E3, 0x0381, 
  0x031F, 0x02BC, 0x0259, 0x01F5, 0x0191, 0x012D, 0x00C9, 0x0065, 
  0x0000, 0xFF9B, 0xFF37, 0xFED3, 0xFE6F, 0xFE0B, 0xFDA7, 0xFD44, 
  0xFCE1, 0xFC7F, 0xFC1D, 0xFBBC, 0xFB5B, 0xFAFB, 0xFA9C, 0xFA3E, 
  0xF9E1, 0xF984, 0xF929, 0xF8CE, 0xF875, 0xF81D, 0xF7C6, 0xF771, 
  0xF71C, 0xF6C9, 0xF678, 0xF628, 0xF5DA, 0xF58D, 0xF541, 0xF4F8, 
  0xF4B0, 0xF469, 0xF425, 0xF3E2, 0xF3A2, 0xF363, 0xF326, 0xF2EB, 
  0xF2B2, 0xF27B, 0xF247, 0xF214, 0xF1E4, 0xF1B5, 0xF189, 0xF15F, 
  0xF138, 0xF112, 0xF0EF, 0xF0CF, 0xF0B0, 0xF094, 0xF07B, 0xF064, 
  0xF04F, 0xF03C, 0xF02C, 0xF01F, 0xF014, 0xF00B, 0xF005, 0xF001, 
  0xF000, 0xF001, 0xF005, 0xF00B, 0xF014, 0xF01F, 0xF02C, 0xF03C,
  0xF04F, 0xF064, 0xF07B, 0xF094, 0xF0B0, 0xF0CF, 0xF0EF, 0xF112,
  0xF138, 0xF15F, 0xF189, 0xF1B5, 0xF1E4, 0xF214, 0xF247, 0xF27B,
  0xF2B2, 0xF2EB, 0xF326, 0xF363, 0xF3A2, 0xF3E2, 0xF425, 0xF469,
  0xF4B0, 0xF4F8, 0xF541, 0xF58D, 0xF5DA, 0xF628, 0xF678, 0xF6C9,
  0xF71C, 0xF771, 0xF7C6, 0xF81D, 0xF875, 0xF8CE, 0xF929, 0xF984,
  0xF9E1, 0xFA3E, 0xFA9C, 0xFAFB, 0xFB5B, 0xFBBC, 0xFC1D, 0xFC7F,
  0xFCE1, 0xFD44, 0xFDA7, 0xFE0B, 0xFE6F, 0xFED3, 0xFF37, 0xFF9B,
};

int16_t spyro_cos(int32_t angle)
{
  angle = angle & 0xFFF;
  uint32_t remainder = angle & 0xF;
  uint32_t index = angle >> 4;

  int32_t cos = cos_lut[index];
  if (remainder) {
    uint32_t next = cos_lut[(index+1)&0xFF];
    cos += (int32_t)((int32_t)remainder*(next-cos)) >> 4;
  }
  return cos;
}

int16_t spyro_sin(int32_t angle)
{
  return spyro_cos(angle-1024);
}

// size: 0x00000058
void function_80016C58(void)
{
  BREAKPOINT;
  v0 = spyro_sin(a0);
}

// size: 0x00000058
void function_80016CB0(void)
{
  BREAKPOINT;
  v0 = spyro_cos(a0);
}

uint32_t spyro_log2_uint(uint32_t a)
{
  if (a == 0) return 0;
  cop2.LZCS = a;
  return 31 - LZCR();
}

// size: 0x00000024
void function_80016D08(void)
{
  BREAKPOINT;
  v0 = spyro_log2_uint(a0);
}

void spyro_mat3_rotation(uint32_t rot_addr, uint32_t dst, uint32_t src)
{
  uint32_t rot = lw(rot_addr);
  mat3 m;
  if (src) {
    m = load_mat3(src);
  } else {
    m = mat3_identity();
  }

  uint32_t rotY = (rot >> 16) & 0xFF;
  if (rotY) m = mat3_mul(m, mat3rotY(-rotY*16));

  uint32_t rotX = (rot >> 8) & 0xFF;
  if (rotX) m = mat3_mul(m, mat3rotX(rotX*16));
  
  uint32_t rotZ = (rot << 0) & 0xFF;
  if (rotZ) m = mat3_mul(m, mat3rotZ(-rotZ*16));

  save_mat3(dst, m);
}

// size: 0x000002A4
// create rotation matrix
// or rotate matrix with
// given rotation vector
void function_80016D2C(void)
{
  BREAKPOINT;
  spyro_mat3_rotation(a0, a1, a2);
}

void spyro_mat3_transpose(uint32_t dst, uint32_t src)
{
  mat3 m = load_mat3(src);
  m = mat3_transpose(m);
  save_mat3(dst, m);
}

// size: 0x00000078
void function_80016FD0(void)
{
  BREAKPOINT;
  spyro_mat3_transpose(a0, a1);
}

void spyro_set_mat_mirrored_vec_multiply(uint32_t mat, uint32_t vec_src, uint32_t vec_dst)
{
  load_RTM(mat);
  mat3 m = get_RTM();

  vec3_32 v = {
    .z =  lw(vec_src + 0),
    .x = -lw(vec_src + 4),
    .y = -lw(vec_src + 8)
  };

  v = vec3_32_mat_mul(v, m);
  sw(vec_dst + 0,  v.z);
  sw(vec_dst + 4, -v.x);
  sw(vec_dst + 8, -v.y);
}

// size: 0x00000078
void function_80017048(void)
{
  BREAKPOINT;
  spyro_set_mat_mirrored_vec_multiply(a0, a1, a2);
}

void spyro_mat_mirrored_vec_multiply(uint32_t vec_src, uint32_t vec_dst)
{
  mat3 m = get_RTM();

  vec3_32 v = {
    .z =  lw(vec_src + 0),
    .x = -lw(vec_src + 4),
    .y = -lw(vec_src + 8)
  };

  v = vec3_32_mat_mul(v, m);
  sw(vec_dst + 0,  v.z);
  sw(vec_dst + 4, -v.x);
  sw(vec_dst + 8, -v.y);
}

// size: 0x00000050
void function_800170C0(void)
{
  BREAKPOINT;
  spyro_mat_mirrored_vec_multiply(a0, a1);
}

void spyro_camera_mat_vec_multiply(uint32_t vec_src, uint32_t vec_dst)
{
  load_RTM(0x80076DD0 + 0x14);
  mat3 m = get_RTM();

  vec3_32 v = {
    .z =  lw(vec_src + 0),
    .x = -lw(vec_src + 4),
    .y = -lw(vec_src + 8)
  };

  v = vec3_32_mat_mul(v, m);
  sw(vec_dst + 0, v.x);
  sw(vec_dst + 4, v.y);
  sw(vec_dst + 8, v.z);
}

// size: 0x00000078
void function_80017110(void)
{
  BREAKPOINT;
  spyro_camera_mat_vec_multiply(a0, a1);
}

// size: 0x000000A0
// vector length
uint32_t spyro_vec_length(uint32_t vec, uint32_t is_vec3)
{
  int32_t x = lw(vec + 0);
  int32_t y = lw(vec + 4);
  int32_t z = 0;
  if (is_vec3)
    z = lw(vec + 8);

  at = x*x + y*y + z*z;
  if (at == 0)
    return 0;
  return spyro_sqrt(at);
}

void function_800171FC(void)
{
  BREAKPOINT;
  v0 = spyro_vec_length(a0, a1);
}

// size: 0x00000094
void function_8001729C(void)
{
  uint32_t temp, return_address = ra;
  cop2.IR1 = lw(a0 + 0);
  cop2.IR3 = 0;
  cop2.IR2 = lw(a0 + 4);
  if (a2)
    cop2.IR3 = lw(a0 + 8);
  mult(a1, a1);
  SQR(SF_OFF);
  at = cop2.MAC1;
  v0 = cop2.MAC2;
  v1 = cop2.MAC3;
  at += v0;
  at += v1;
  v0=lo;
  cop2.LZCS = v0;
  v1 = LZCR();
  a0 = 32; // 0x0020
  v1 = a0 - v1;
  v1 = v1 >> 1;
  a0 = v1 - 6; // 0xFFFFFFFA
  temp = (int32_t)a0 <= 0;
  v0 = v0 >> v1;
  if (temp) goto label80017328;
  div_psx(at,v0);
  a0 = 1; // 0x0001
  a0 = a0 << v1;
  at=lo;
  at -= a0;
  at = (int32_t)at >> 1;
  mult(at, a1);
  at=lo;
  at = (int32_t)at >> v1;
  temp = ra;
  v0 = a1 + at;
  if (temp == return_address) return;
  switch (temp)
  {
  default:
    JR(temp, 0x80017320);
    return;
  }
label80017328:
  temp = ra;
  v0 = a1;
  if (temp == return_address) return;
  switch (temp)
  {
  default:
    JR(temp, 0x80017328);
    return;
  }
}

// size: 0x000000F8
void spyro_set_vec3_length(uint32_t vec, int32_t len)
{
  int32_t x = lw(vec + 0);
  int32_t y = lw(vec + 4);
  int32_t z = lw(vec + 8);
  int32_t dist = x*x + y*y + z*z;
  if (dist == 0) {
    sw(vec + 0, 0);
    sw(vec + 4, 0);
    sw(vec + 8, 0);
    return;
  }
  dist = spyro_sqrt(dist);
  len = len << 12;
  int32_t scale = len/dist;
  sw(vec + 0, x*scale >> 12);
  sw(vec + 4, y*scale >> 12);
  sw(vec + 8, z*scale >> 12);
}

void function_80017330(void)
{
  BREAKPOINT;
  spyro_set_vec3_length(a0, a1);
}

// size: 0x00000120
void function_80017428(void)
{
  uint32_t temp, return_address = ra;
  at = (int32_t)lw(a1 + 0) >> 4;
  v0 = (int32_t)lw(a1 + 4) >> 4;
  v1 = (int32_t)lw(a1 + 8) >> 4;
  a3 = -lw(a0 + 0);
  t0 = -lw(a0 + 4);
  t1 = -lw(a0 + 8);
  cop2.RTM0 = (a3 & 0xFFFF) + (t0 << 16);
  cop2.RTM1 = t1;
  cop2.IR1 = at;
  cop2.IR2 = v0;
  cop2.IR3 = v1;
  MVMVA(SF_OFF, MX_RT, V_IR, CV_NONE, LM_OFF);
  t2 = cop2.MAC1;
  cop2.IR1 = at;
  cop2.IR2 = v0;
  cop2.IR3 = v1;
  v0 = 0;
  if ((int32_t)t2 < 0) {
    sw(a2 + 0, -a3);
    sw(a2 + 4, -t0);
    sw(a2 + 8, -t1);
    return;
  }
  SQR(SF_OFF);
  t3 = cop2.MAC1;
  t4 = cop2.MAC2;
  t5 = cop2.MAC3;
  t3 += t4;
  t3 += t5;
  t2 = t2 << 6;
  div_psx(t2,t3);
  cop2.IR1 = at;
  cop2.IR2 = v0;
  cop2.IR3 = v1;
  cop2.MAC1 = 0;
  cop2.MAC2 = 0;
  cop2.MAC3 = 0;
  t2=lo;
  cop2.IR0 = t2;
  GPL(SF_OFF, LM_OFF);
  at = cop2.MAC1;
  v0 = cop2.MAC2;
  v1 = cop2.MAC3;
  at = (int32_t)at >> 6;
  v0 = (int32_t)v0 >> 6;
  v1 = (int32_t)v1 >> 6;
  a3 = at - a3;
  t0 = v0 - t0;
  t1 = v1 - t1;
  at += a3;
  v0 += t0;
  v1 += t1;
  sw(a2 + 0x0000, at);
  sw(a2 + 0x0004, v0);
  sw(a2 + 0x0008, v1);
  temp = ra;
  v0 = 1; // 0x0001
  if (temp == return_address) return;
  switch (temp)
  {
  default:
    JR(temp, 0x80017540);
    return;
  }
}

void vec3_mul_div(uint32_t vec, int32_t div, int32_t mul)
{
  if (div == 0) {
    sw(vec + 0, 0);
    sw(vec + 4, 0);
    sw(vec + 8, 0);
    return;
  }
  sw(vec + 0, (int32_t)lw(vec + 0)*mul/div);
  sw(vec + 4, (int32_t)lw(vec + 4)*mul/div);
  sw(vec + 8, (int32_t)lw(vec + 8)*mul/div);
}

// size: 0x0000005C
void function_800175B8(void)
{
  BREAKPOINT;
  vec3_mul_div(a0, a1, a2);
}

// size: 0x0000008C
void spyro_vec3_multiply_fancy_shift_right(uint32_t vec, uint32_t mult, uint32_t shift)
{
  at = (int32_t)lw(vec + 0)*(int32_t)mult;
  v0 = (int32_t)lw(vec + 4)*(int32_t)mult;
  v1 = (int32_t)lw(vec + 8)*(int32_t)mult;
  if ((int32_t)at >= 0)
    at = at >> shift;
  else
    at = -((-at) >> shift);

  if ((int32_t)v0 >= 0) 
    v0 = v0 >> shift;
  else
    v0 = -((-v0) >> shift);

  if ((int32_t)v1 >= 0) 
    v1 = v1 >> shift;
  else
    v1 = -((-v1) >> shift);

  sw(a0 + 0, at);
  sw(a0 + 4, v0);
  sw(a0 + 8, v1);
  return;
}

void function_80017614(void)
{
  BREAKPOINT;
  spyro_vec3_multiply_fancy_shift_right(a0, a1, a2);
}

void spyro_vec3_shift_left(uint32_t vec, uint32_t shift)
{
  sw(vec + 0, lw(vec + 0) << shift);
  sw(vec + 4, lw(vec + 4) << shift);
  sw(vec + 8, lw(vec + 8) << shift);
}

// size: 0x00000028
void function_800176A0(void)
{
  BREAKPOINT;
  spyro_vec3_shift_left(a0, a1);
}

void spyro_vec3_shift_right(uint32_t vec, uint32_t shift)
{
  sw(vec + 0, (int32_t)lw(vec + 0) >> shift);
  sw(vec + 4, (int32_t)lw(vec + 4) >> shift);
  sw(vec + 8, (int32_t)lw(vec + 8) >> shift);
}

// size: 0x00000028
void function_800176C8(void)
{
  BREAKPOINT;
  spyro_vec3_shift_right(a0, a1);
}

void spyro_vec3_clear(uint32_t vec)
{
  sw(vec + 0, 0);
  sw(vec + 4, 0);
  sw(vec + 8, 0);
}

// size: 0x00000010
void function_800176F0(void)
{
  BREAKPOINT;
  spyro_vec3_clear(a0);
}

void spyro_vec3_copy(uint32_t dst, uint32_t src)
{
  sw(dst + 0, lw(src + 0));
  sw(dst + 4, lw(src + 4));
  sw(dst + 8, lw(src + 8));
}

// size: 0x0000001C
void function_80017700(void)
{
  BREAKPOINT;
  spyro_vec3_copy(a0, a1);
}

void spyro_vec3_add(uint32_t dst, uint32_t a, uint32_t b)
{
  sw(dst + 0, lw(a + 0) + lw(b + 0));
  sw(dst + 4, lw(a + 4) + lw(b + 4));
  sw(dst + 8, lw(a + 8) + lw(b + 8));
}

// size: 0x00000034
void function_80017758(void)
{
  BREAKPOINT;
  spyro_vec3_add(a0, a1, a2);
}

void spyro_vec3_sub(uint32_t dst, uint32_t a, uint32_t b)
{
  sw(dst + 0, lw(a + 0) - lw(b + 0));
  sw(dst + 4, lw(a + 4) - lw(b + 4));
  sw(dst + 8, lw(a + 8) - lw(b + 8));
}

// size: 0x00000034
void function_8001778C(void)
{
  BREAKPOINT;
  spyro_vec3_sub(a0, a1, a2);
}

void spyro_vec3_mul(uint32_t dst, uint32_t src, int32_t mul)
{
  sw(dst + 0, (int32_t)lw(src + 0)*mul);
  sw(dst + 4, (int32_t)lw(src + 4)*mul);
  sw(dst + 8, (int32_t)lw(src + 8)*mul);
}

// size: 0x00000038
void function_800177C0(void)
{
  BREAKPOINT;
  spyro_vec3_mul(a0, a1, a2);
}

void spyro_vec3_div(uint32_t dst, uint32_t src, int32_t div)
{
  sw(dst + 0, (int32_t)lw(src + 0)/div);
  sw(dst + 4, (int32_t)lw(src + 4)/div);
  sw(dst + 8, (int32_t)lw(src + 8)/div);
}

// size: 0x0000005C
void function_800177F8(void)
{
  BREAKPOINT;
  spyro_vec3_div(a0, a1, a2);
}

void spyro_vec_interpolation(uint32_t dst, uint32_t vec1, uint32_t vec2, int32_t ipol)
{
  sw(dst + 0, ((int32_t)lw(vec2 + 0)*ipol + (int32_t)lw(vec1 + 0)*(0x1000-ipol)) >> 12);
  sw(dst + 4, ((int32_t)lw(vec2 + 4)*ipol + (int32_t)lw(vec1 + 4)*(0x1000-ipol)) >> 12);
  sw(dst + 8, ((int32_t)lw(vec2 + 8)*ipol + (int32_t)lw(vec1 + 8)*(0x1000-ipol)) >> 12);
}

// size: 0x00000074
void function_80017894(void)
{
  BREAKPOINT;
  spyro_vec_interpolation(a0, a1, a2, a3);
}

uint32_t spyro_two_angle_diff_8bit(uint32_t a, uint32_t b)
{
  uint32_t diff = a0 - a1;
  diff = diff & 0xFF;
  if (diff >= 0x80)
    diff = 0x100 - diff;
  return diff;
}

// size: 0x00000020
// zigzag function 0-128-0-128
void function_80017908(void)
{
  BREAKPOINT;
  v0 = spyro_two_angle_diff_8bit(a0, a1);
}

uint32_t spyro_two_angle_diff_12bit(uint32_t a, uint32_t b)
{
  uint32_t diff = a0 - a1;
  diff = diff & 0xFFF;
  if (diff >= 0x800)
    diff = 0x1000 - diff;
  return diff;
}

// size: 0x00000020
void function_80017928(void)
{
  BREAKPOINT;
  v0 = spyro_two_angle_diff_12bit(a0, a1);
}

// size: 0x00000024
uint32_t spyro_two_angle_signed_diff_8bit(int32_t a, int32_t b)
{

  uint32_t diff = (a - b) & 0xFF;
  if (diff > 0x80)
    return diff - 0x100;
  return diff;
}

// size: 0x00000024
void function_80017948(void)
{
  BREAKPOINT;
  v0 = spyro_two_angle_signed_diff_8bit(a0, a1);
}

// size: 0x00000024
uint32_t spyro_two_angle_signed_diff_12bit(int32_t a, int32_t b)
{
  uint32_t diff = (a - b) & 0xFFF;
  if (diff > 0x800)
    return diff - 0x1000;
  return diff;
}

// size: 0x00000024
void function_8001796C(void)
{
  BREAKPOINT;
  v0 = spyro_two_angle_signed_diff_12bit(a0, a1);
}

uint32_t spyro_octagon_distance(uint32_t a, uint32_t b)
{
  at = lw(a + 0);
  v0 = lw(a + 4);
  a2 = lw(b + 0);
  a3 = lw(b + 4);
  a2 = abs_int(a2 - at);
  a3 = abs_int(a3 - v0);
  if ((int32_t)a2 < (int32_t)a3) {
    v0 = a2*3/8 + a3;
  } else {
    v0 = a2 + a3*3/8;
  }
  return v0;
}

// size: 0x00000060
// octagon distance between two points
void function_80017990(void)
{
  BREAKPOINT;
  v0 = spyro_octagon_distance(a0, a1);
}

// size: 0x00000048
uint32_t spyro_attract_angle_in_range(uint32_t a, uint32_t b, int32_t attraction, int32_t range)
{
  int32_t diff = (a - b) & 0xFF;
  if (diff > 0x80) {
    if (0x100 - diff < range)
      return b & 0xFF;
    return (b - attraction) & 0xFF;
  }

  if (diff < range)
    return b & 0xFF;
  return (b + attraction) & 0xFF;
}

void function_800179F0(void)
{
  BREAKPOINT;
  v0 = spyro_attract_angle_in_range(a0, a1, a2, a3);
}

// size: 0x0000006C
uint32_t spyro_sqrt(uint32_t a)
{
  if (a == 0)
    return 0;
  cop2.LZCS = a;
  a1 = LZCR() & ~1;
  at = (int32_t)(31 - a1)/2;
  if ((int32_t)a1 >= 24)
    a3 = a << (a1 - 24);
  else
    a3 = (int32_t)a >> (24 - a1);
  a3 -= 64;
  t0 = lh(0x80074B84 + a3*2) << at;
  v0 = t0 >> 12;
  return v0;
}

void function_80017A38(void)
{
  BREAKPOINT;
  v0 = spyro_sqrt(a0);
}

// size: 0x000000A4
void spyro_world_to_screen_projection(uint32_t dst, uint32_t vec)
{
  load_RTM(0x80076DD0);
  set_TR(0,0,0);
  a2 = lw(0x80076DD0 + 0x28);
  t0 = lw(0x80076DD0 + 0x2C);
  a3 = lw(0x80076DD0 + 0x30);
  at = lw(vec + 0);
  v0 = lw(vec + 4);
  v1 = lw(vec + 8);
  at =   at - a2;
  v0 = -(v0 - t0);
  v1 = -(v1 - a3);
  cop2.VXY0 = (v0 & 0xFFFF) | (v1 << 16);
  cop2.VZ0 = at;
  RTPS();
  v1 = cop2.MAC3;
  v0 = (int32_t)(cop2.SXY2 << 16) >> 16;
  at = (int32_t)cop2.SXY2 >> 16;
  sw(dst + 0, v0);
  sw(dst + 4, at);
  sw(dst + 8, v1);
}

void function_80017AA4(void)
{
  BREAKPOINT;
  spyro_world_to_screen_projection(a0, a1);
}

// size: 0x000000B4
void spyro_world_to_screen_projection_with_right_shift(uint32_t dst, uint32_t vec, uint32_t shift)
{
  a3 = 0x80076DD0;
  load_RTM(a3);
  set_TR(0,0,0);
  t1 = lw(a3 + 0x28);
  t0 = lw(a3 + 0x30);
  a3 = lw(a3 + 0x2C);
  at = lw(vec + 0);
  v0 = lw(vec + 4);
  v1 = lw(vec + 8);
  at =   at - t1;
  v0 = -(v0 - a3);
  v1 = -(v1 - t0);
  at = (int32_t)at >> shift;
  v0 = (int32_t)v0 >> shift;
  v1 = (int32_t)v1 >> shift;
  cop2.VXY0 = (v0 & 0xFFFF) | (v1 << 16);
  cop2.VZ0 = at;
  RTPS();
  v0 = (int32_t)(cop2.SXY2 << 16) >> 16;
  at = (int32_t)cop2.SXY2 >> 16;
  v1 = cop2.MAC3 << shift;
  sw(dst + 0, v0);
  sw(dst + 4, at);
  sw(dst + 8, v1);
}

void function_80017B48(void)
{
  BREAKPOINT;
  spyro_world_to_screen_projection_with_right_shift(a0, a1, a2);
}

void spyro_vec_32_to_16_div_4(uint32_t dst, uint32_t src)
{
  sh(dst + 0, (int32_t)lw(src + 0) >> 2);
  sh(dst + 2, (int32_t)lw(src + 4) >> 2);
  sh(dst + 4, (int32_t)lw(src + 8) >> 2);
}

// size: 0x00000028
void function_80017BFC(void)
{
  BREAKPOINT;
  spyro_vec_32_to_16_div_4(a0, a1);
}

void spyro_vec_16_to_32_mul_4(uint32_t dst, uint32_t src)
{
  sw(dst + 0, lhu(src + 0) << 2);
  sw(dst + 4, lhu(src + 2) << 2);
  sw(dst + 8, lhu(src + 4) << 2);
}

// size: 0x00000028
void function_80017C24(void)
{
  BREAKPOINT;
  spyro_vec_16_to_32_mul_4(a0, a1);
}

void spyro_vec_16_to_32(uint32_t dst, uint32_t src)
{
  sw(dst + 0, lh(src + 0));
  sw(dst + 4, lh(src + 2));
  sw(dst + 8, lh(src + 4));
}

// size: 0x0000001C
void function_80017C4C(void)
{
  BREAKPOINT;
  spyro_vec_16_to_32(a0, a1);
}

void spyro_vec_32_to_16(uint32_t dst, uint32_t src)
{
  sh(dst + 0, lw(src + 0));
  sh(dst + 2, lw(src + 4));
  sh(dst + 4, lw(src + 8));
}

// size: 0x0000001C
void function_80017C68(void)
{
  BREAKPOINT;
  spyro_vec_32_to_16(a0, a1);
}

void spyro_vec_16_add(uint32_t dst, uint32_t a, uint32_t b)
{
  sh(dst + 0, lh(a + 0) + lh(b + 0));
  sh(dst + 2, lh(a + 2) + lh(b + 2));
  sh(dst + 4, lh(a + 4) + lh(b + 4));
}

// size: 0x00000034
void function_80017C84(void)
{
  BREAKPOINT;
  spyro_vec_16_add(a0, a1, a2);
}

// size: 0x000000C4
void spyro_unpack_96bit_triangle(uint32_t index, uint32_t dst)
{
  at = lw(lw(0x800785A8 + 0x2C) + 0x10) + index*12;
  v0 = lw(at + 0);
  v1 = lw(at + 4);
  a0 = lw(at + 8);
  t0 = (int32_t)(v0 << 0) >> 23; // 23:31
  a3 = (int32_t)(v0 << 9) >> 23; // 14:22
  a2 = v0 & 0x3FFF; // 0:13
  a3 += a2;
  t0 += a2;
  sw(dst + 0x00, a2 << 4);
  sw(dst + 0x0C, a3 << 4);
  sw(dst + 0x18, t0 << 4);
  t0 = (int32_t)(v1 << 0) >> 23; // 23:31
  a3 = (int32_t)(v1 << 9) >> 23; // 14:22
  a2 = v1 & 0x3FFF; // 0:13
  a3 += a2;
  t0 += a2;
  sw(dst + 0x04, a2 << 4);
  sw(dst + 0x10, a3 << 4);
  sw(dst + 0x1C, t0 << 4);
  t0 = (a0 << 0) >> 24; // 24:31
  a3 = (a0 << 8) >> 24; // 16:23
  a2 = a0 & 0x3FFF; // 0:13
  a3 += a2;
  t0 += a2;
  sw(dst + 0x08, a2 << 4);
  sw(dst + 0x14, a3 << 4);
  sw(dst + 0x20, t0 << 4);
}

void function_80017CB8(void)
{
  BREAKPOINT;
  spyro_unpack_96bit_triangle(a0, a1);
}

// size: 0x000000D8
void function_80017D7C(void)
{
  uint32_t temp, return_address = ra;
  t0 = lw(a0 + 0x0000);
  t1 = lw(a1 + 0x0000);
  t2 = lw(a0 + 0x0004);
  t3 = lw(a1 + 0x0004);
  a3 = a3 >> 1;
  cop2.LZCS = a3;
  t4 = lw(a0 + 0x0008);
  t5 = lw(a1 + 0x0008);
  v0 = LZCR();
  at = 32; // 0x0020
  temp = v0 == at;
  t7 = at - v0;
  if (temp) goto label80017E4C;
  at = t1 - t0;
  temp = (int32_t)at >= 0;
  v0 = t3 - t2;
  if (temp) goto label80017DBC;
  at = -at;
label80017DBC:
  temp = (int32_t)v0 >= 0;
  v1 = t5 - t4;
  if (temp) goto label80017DC8;
  v0 = -v0;
label80017DC8:
  temp = (int32_t)v1 >= 0;
  a3 = at - v0;
  if (temp) goto label80017DD4;
  v1 = -v1;
label80017DD4:
  temp = (int32_t)a3 >= 0;
  if (temp) goto label80017DE0;
  at = v0;
label80017DE0:
  a3 = at - v1;
  temp = (int32_t)a3 >= 0;

  if (temp) goto label80017DF0;
  at = v1;
label80017DF0:
  t7 = at >> t7;
  t6 = 1; // 0x0001
  t6 = t6 << 12;
  div_psx(t6,t7);
  at = t1 - t0;
  cop2.IR1 = at;
  v0 = t3 - t2;
  cop2.IR2 = v0;
  v1 = t5 - t4;
  cop2.IR3 = v1;
  t7=lo;
  cop2.IR0 = t7;
  v0 = t7;
  GPF(SF_OFF, LM_OFF);
  t0 = cop2.MAC1;
  t1 = cop2.MAC2;
  t0 = (int32_t)t0 >> 12;
  t1 = (int32_t)t1 >> 12;
  t2 = cop2.MAC3;
  sw(a2 + 0x0000, t0);
  t2 = (int32_t)t2 >> 12;
  sw(a2 + 0x0004, t1);
label80017E4C:
  temp = ra;
  sw(a2 + 0x0008, t2);
  if (temp == return_address) return;
  switch (temp)
  {
  default:
    JR(temp, 0x80017E4C);
    return;
  }
}

uint32_t interpolate_color(uint32_t c1, uint32_t c2, int32_t ipol)
{
  int32_t r1 = (c1 >>  0) & 0xFF;
  int32_t g1 = (c1 >>  8) & 0xFF;
  int32_t b1 = (c1 >> 16) & 0xFF;

  int32_t r2 = (c2 >>  0) & 0xFF;
  int32_t g2 = (c2 >>  8) & 0xFF;
  int32_t b2 = (c2 >> 16) & 0xFF;

  int32_t dr = r2-r1;
  int32_t dg = g2-g1;
  int32_t db = b2-b1;

  int32_t r = (r1 + dr*ipol/4096);
  int32_t g = (g1 + dg*ipol/4096);
  int32_t b = (b1 + db*ipol/4096);

  if (r > 255) r = 255;
  if (g > 255) g = 255;
  if (b > 255) b = 255;
  if (r < 0) r = 0;
  if (g < 0) g = 0;
  if (b < 0) b = 0;

  uint32_t c3 = r | (g << 8) | (b << 16);
  return c3;
}

// size: 0x00000044
void function_80017E54(void)
{
  BREAKPOINT;
  v0 = interpolate_color(a0, a1, a2);
}