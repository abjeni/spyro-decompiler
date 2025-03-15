#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "gte.h"
#include "int_math.h"
#include "debug.h"

struct cop2 cop2;

mat3 get_mat(uint32_t i)
{
  uint32_t list[5];

  switch (i) {
  case 0:
    list[0] = cop2.RTM0;
    list[1] = cop2.RTM1;
    list[2] = cop2.RTM2;
    list[3] = cop2.RTM3;
    list[4] = cop2.RTM4;
    break;
  case 1:
    list[0] = cop2.LLM0;
    list[1] = cop2.LLM1;
    list[2] = cop2.LLM2;
    list[3] = cop2.LLM3;
    list[4] = cop2.LLM4;
    break;
  case 2:
    list[0] = cop2.LCM0;
    list[1] = cop2.LCM1;
    list[2] = cop2.LCM2;
    list[3] = cop2.LCM3;
    list[4] = cop2.LCM4;
    break;
  default:
    BREAKPOINT;
  }

  return list_to_mat3(list);
}

vec3 xy_z_to_vec(uint32_t xy, uint32_t z)
{
  return (vec3){
    .x = xy & 0xFFFF,
    .y = xy >> 16,
    .z = z & 0xFFFF
  };
}

vec3 get_mul_vec(uint32_t i)
{
  uint32_t xy, z;

  switch (i) {
    case 0:
      xy = cop2.VXY0;
      z = cop2.VZ0;
      break;
    case 1:
      xy = cop2.VXY1;
      z = cop2.VZ1;
      break;
    case 2:
      xy = cop2.VXY2;
      z = cop2.VZ2;
      break;
    case 3:
      return (vec3){
        .x = cop2.IR1,
        .y = cop2.IR2,
        .z = cop2.IR3
      };
    default:
      BREAKPOINT;
  }

  return xy_z_to_vec(xy, z);
}

vec3 get_trans_vec(uint32_t i)
{
  vec3 vec;

  switch(i) {
  case 0:
    vec = (vec3){
      .x = cop2.TRX & 0xFFFF,
      .y = cop2.TRY & 0xFFFF,
      .z = cop2.TRZ & 0xFFFF,
    };
    break;
  case 1:
    vec = (vec3){
      .x = cop2.RBK & 0xFFFF,
      .y = cop2.GBK & 0xFFFF,
      .z = cop2.BBK & 0xFFFF,
    };
    break;
  case 2:
    vec = (vec3){
      .x = cop2.RFC & 0xFFFF,
      .y = cop2.GFC & 0xFFFF,
      .z = cop2.BFC & 0xFFFF,
    };
    break;
  case 3:
    vec = (vec3){0};
    break;
  default:
    BREAKPOINT;
  }

  return vec;
}

uint32_t clamp(int32_t *x, int32_t min, int32_t max)
{
  if (*x < min) {
    *x = min;
    return 1;
  }
  if (*x > max) {
    *x = max;
    return 1;
  }
  return 0;
}

void clamp_REGS(int min)
{
  cop2.IR0sat = clamp((int32_t*)&cop2.IR0, 0x0, 0x7FFF);
  cop2.IR1sat = clamp((int32_t*)&cop2.IR1, min, 0x7FFF);
  cop2.IR2sat = clamp((int32_t*)&cop2.IR2, min, 0x7FFF);
  cop2.IR3sat = clamp((int32_t*)&cop2.IR3, min, 0x7FFF);
  cop2.SZ3OTZsat = clamp((int32_t*)&cop2.SZ3, 0x0, 0x0000FFFF);
  cop2.SZ3OTZsat = clamp((int32_t*)&cop2.SZ2, 0x0, 0x0000FFFF) || cop2.SZ3OTZsat;
  cop2.SZ3OTZsat = clamp((int32_t*)&cop2.SZ1, 0x0, 0x0000FFFF) || cop2.SZ3OTZsat;
  cop2.SZ3OTZsat = clamp((int32_t*)&cop2.OTZ, 0x0, 0x0000FFFF) || cop2.SZ3OTZsat;
}

void start_gte(void)
{
  cop2.FLAG = 0;
}

void end_gte(void)
{
  cop2.err = (cop2.FLAG & 0x7F87E000) != 0;
}

void MVMVA(uint32_t sf, uint32_t mx, uint32_t v, uint32_t cv, uint32_t lm)
{
  mat3 m = get_mat(mx);
  vec3 x = get_mul_vec(v);
  vec3 t = get_trans_vec(cv);
  vec3_32 r = vec_mat_mul_gte(x, m);

  cop2.MAC1 = (t.x >> (12-12*sf)) + (r.x >> (sf*12));
  cop2.MAC2 = (t.y >> (12-12*sf)) + (r.y >> (sf*12));
  cop2.MAC3 = (t.z >> (12-12*sf)) + (r.z >> (sf*12));
  cop2.IR1 = cop2.MAC1;
  cop2.IR2 = cop2.MAC2;
  cop2.IR3 = cop2.MAC3;

  int min = -0x8000;
  if (lm) min = 0x0;

  clamp_REGS(min);
}

void project(void)
{
  int64_t num = 0x20000;
  if (((int32_t)cop2.SZ3)*2 > (int32_t)cop2.H)
    num = ((((int64_t)cop2.H)*((int64_t)0x20000)/((int64_t)cop2.SZ3))+1)/2;
//    num = ((((int32_t)cop2.H)*0x10000+((int32_t)cop2.SZ3)/2)/((int32_t)cop2.SZ3));
  
  if (num > 0x1FFFF || num < 0)
  {
    num = 0x1FFFF;
    cop2.div_overflow = 1;
  }

  int64_t a;

  a = num*((int32_t)cop2.IR1)+(int32_t)cop2.OFX;
  if (a > 0x7FFFFFFF) {cop2.MAC0overflow = 1;}
  if (a < -0x80000000) {cop2.MAC0underflow = 1;}
  int32_t x = (int64_t)a/0x10000;

  a = num*((int32_t)cop2.IR2)+(int32_t)cop2.OFY;
  if (a > 0x7FFFFFFF) {cop2.MAC0overflow = 1;}
  if (a < -0x80000000) {cop2.MAC0underflow = 1;}
  int32_t y = (int64_t)a/0x10000;

  cop2.SY2sat = clamp(&y, -0x400, 0x3FF);
  cop2.SX2sat = clamp(&x, -0x400, 0x3FF);

  cop2.SXY2 = ((uint32_t)y << 16) | ((uint32_t)x & 0xFFFF);

  a = num*((int32_t)cop2.DQA)+(int32_t)cop2.DQB;
  if (a > 0x7FFFFFFF) cop2.MAC0overflow = 1;
  if (a < -0x80000000) cop2.MAC0underflow = 1;
  cop2.MAC0=a;
  cop2.IR0=a/0x1000;

  clamp_REGS(-0x8000);
}

void RTPS(void)
{
  start_gte();

  mat3 mm = get_mat(0);
  vec3 vx = get_mul_vec(0);
  vec3 vt = get_trans_vec(0);
  vec3_32 r = vec_mat_mul_gte(vx, mm);

  cop2.MAC1 = vt.x + (((int32_t)r.x) >> 12);
  cop2.MAC2 = vt.y + (((int32_t)r.y) >> 12);
  cop2.MAC3 = vt.z + (((int32_t)r.z) >> 12);
  cop2.IR1 = cop2.MAC1;
  cop2.IR2 = cop2.MAC2;
  cop2.IR3 = cop2.MAC3;

  cop2.SZ0 = cop2.SZ1;
  cop2.SZ1 = cop2.SZ2;
  cop2.SZ2 = cop2.SZ3;
  cop2.SZ3 = cop2.MAC3;

  cop2.SXY0=cop2.SXY1;
  cop2.SXY1=cop2.SXY2;

  project();

  end_gte();
}

void push_S(void)
{
  cop2.SZ0 = cop2.SZ1;
  cop2.SZ1 = cop2.SZ2;
  cop2.SZ2 = cop2.SZ3;
  cop2.SZ3 = cop2.MAC3;

  cop2.SXY0=cop2.SXY1;
  cop2.SXY1=cop2.SXY2;
}

void RTPT(void)
{
  mat3 mm = get_mat(0);
  vec3 vx0 = get_mul_vec(0);
  vec3 vx1 = get_mul_vec(1);
  vec3 vx2 = get_mul_vec(2);
  vec3 vt = get_trans_vec(0);
  vec3_32 r0 = vec_mat_mul_gte(vx0, mm);
  vec3_32 r1 = vec_mat_mul_gte(vx1, mm);
  vec3_32 r2 = vec_mat_mul_gte(vx2, mm);

  cop2.MAC1 = vt.x + ((int32_t)r0.x >> 12);
  cop2.MAC2 = vt.y + ((int32_t)r0.y >> 12);
  cop2.MAC3 = vt.z + ((int32_t)r0.z >> 12);
  cop2.IR1 = cop2.MAC1;
  cop2.IR2 = cop2.MAC2;
  cop2.IR3 = cop2.MAC3;

  push_S();

  project();
  
  cop2.MAC1 = vt.x + (((int32_t)r1.x) >> 12);
  cop2.MAC2 = vt.y + (((int32_t)r1.y) >> 12);
  cop2.MAC3 = vt.z + (((int32_t)r1.z) >> 12);
  cop2.IR1 = cop2.MAC1;
  cop2.IR2 = cop2.MAC2;
  cop2.IR3 = cop2.MAC3;

  push_S();

  project();

  cop2.MAC1 = vt.x + (((int32_t)r2.x) >> 12);
  cop2.MAC2 = vt.y + (((int32_t)r2.y) >> 12);
  cop2.MAC3 = vt.z + (((int32_t)r2.z) >> 12);
  cop2.IR1 = cop2.MAC1;
  cop2.IR2 = cop2.MAC2;
  cop2.IR3 = cop2.MAC3;

  push_S();

  project();
}

void push_rgb(uint32_t rgb)
{
  cop2.RGB0 = cop2.RGB1;
  cop2.RGB1 = cop2.RGB2;
  cop2.RGB2 = rgb;
}

uint32_t get_crgb(int32_t r, int32_t g, int32_t b, int32_t c)
{
  r /= 16;
  g /= 16;
  b /= 16;
  c /= 16;
  cop2.Rsat = clamp((int32_t*)&r, 0, 255);
  cop2.Gsat = clamp((int32_t*)&g, 0, 255);
  cop2.Bsat = clamp((int32_t*)&b, 0, 255);

  return (((uint32_t)c) << 24) | (((uint32_t)b) << 16) | (((uint32_t)g) << 8) | r;
}

uint32_t crgb_code(void)
{
  return (cop2.RGBC >> 24) << 4;
}

void INTPL(void)
{
  cop2.MAC1 = cop2.IR1+((((int32_t)(cop2.RFC-cop2.IR1))*((int32_t)cop2.IR0))>>12);
  cop2.MAC2 = cop2.IR2+((((int32_t)(cop2.GFC-cop2.IR2))*((int32_t)cop2.IR0))>>12);
  cop2.MAC3 = cop2.IR3+((((int32_t)(cop2.BFC-cop2.IR3))*((int32_t)cop2.IR0))>>12);

  cop2.IR1 = cop2.MAC1;
  cop2.IR2 = cop2.MAC2;
  cop2.IR3 = cop2.MAC3;

  clamp_REGS(-0x8000);
  push_rgb(get_crgb(cop2.MAC1, cop2.MAC2, cop2.MAC3, crgb_code()));
}

void DPCS(void)
{
  uint32_t r = (cop2.RGBC >>  0) & 0xFF;
  uint32_t g = (cop2.RGBC >>  8) & 0xFF;
  uint32_t b = (cop2.RGBC >> 16) & 0xFF;

  cop2.MAC1 = r<<4;
  cop2.MAC2 = g<<4;
  cop2.MAC3 = b<<4;

  int32_t v1 = cop2.RFC-cop2.MAC1;
  int32_t v2 = cop2.GFC-cop2.MAC2;
  int32_t v3 = cop2.BFC-cop2.MAC3;

  cop2.MAC1 = cop2.MAC1+((v1*((int32_t)cop2.IR0))>>12);
  cop2.MAC2 = cop2.MAC2+((v2*((int32_t)cop2.IR0))>>12);
  cop2.MAC3 = cop2.MAC3+((v3*((int32_t)cop2.IR0))>>12);

  cop2.IR1 = cop2.MAC1;
  cop2.IR2 = cop2.MAC2;
  cop2.IR3 = cop2.MAC3;

  clamp_REGS(-0x8000);

  push_rgb(get_crgb(cop2.IR1, cop2.IR2, cop2.IR3, crgb_code()));
}

void NCLIP(void)
{
  int32_t SY0 = (int16_t)(cop2.SXY0 >> 16);
  int32_t SX0 = (int16_t)(cop2.SXY0 & 0xFFFF);
  
  int32_t SY1 = (int16_t)(cop2.SXY1 >> 16);
  int32_t SX1 = (int16_t)(cop2.SXY1 & 0xFFFF);
  
  int32_t SY2 = (int16_t)(cop2.SXY2 >> 16);
  int32_t SX2 = (int16_t)(cop2.SXY2 & 0xFFFF);
  
  cop2.MAC0 = (SX0*SY1 + SX1*SY2 + SX2*SY0 - SX0*SY2 - SX1*SY0 - SX2*SY1);
}

void GPF(uint32_t sf, uint32_t lm)
{
  cop2.MAC1 = (((int32_t)cop2.IR1) * ((int32_t)cop2.IR0)) >> (sf*12);
  cop2.MAC2 = (((int32_t)cop2.IR2) * ((int32_t)cop2.IR0)) >> (sf*12);
  cop2.MAC3 = (((int32_t)cop2.IR3) * ((int32_t)cop2.IR0)) >> (sf*12);

  cop2.IR1 = cop2.MAC1;
  cop2.IR2 = cop2.MAC2;
  cop2.IR3 = cop2.MAC3;

  int min = -0x8000;
  if (lm) min = 0x0;

  clamp_REGS(min);
  
  push_rgb(get_crgb(cop2.MAC1, cop2.MAC2, cop2.MAC3,crgb_code()));
}

void GPL(uint32_t sf, uint32_t lm)
{
  cop2.MAC1 = ((((int32_t)cop2.IR1) * ((int32_t)cop2.IR0)) + (((int32_t)cop2.MAC1) << (sf*12))) >> (sf*12);
  cop2.MAC2 = ((((int32_t)cop2.IR2) * ((int32_t)cop2.IR0)) + (((int32_t)cop2.MAC2) << (sf*12))) >> (sf*12);
  cop2.MAC3 = ((((int32_t)cop2.IR3) * ((int32_t)cop2.IR0)) + (((int32_t)cop2.MAC3) << (sf*12))) >> (sf*12);

  cop2.IR1 = cop2.MAC1;
  cop2.IR2 = cop2.MAC2;
  cop2.IR3 = cop2.MAC3;

  int min = -0x8000;
  if (lm) min = 0x0;

  clamp_REGS(min);
  
  push_rgb(get_crgb(cop2.MAC1, cop2.MAC2, cop2.MAC3,crgb_code()));
}

void SQR(uint32_t sf)
{
  clamp_REGS(-0x8000);

  cop2.MAC1 = (((int32_t)cop2.IR1) * ((int32_t)cop2.IR1)) >> (sf*12);
  cop2.MAC2 = (((int32_t)cop2.IR2) * ((int32_t)cop2.IR2)) >> (sf*12);
  cop2.MAC3 = (((int32_t)cop2.IR3) * ((int32_t)cop2.IR3)) >> (sf*12);

  cop2.IR1 = cop2.MAC1;
  cop2.IR2 = cop2.MAC2;
  cop2.IR3 = cop2.MAC3;

  clamp_REGS(0);
}


void CC(void)
{
  mat3 lcm = get_mat(2);
  vec3 x = get_mul_vec(3);
  vec3 t = get_trans_vec(1);
  vec3_32 res = vec_mat_mul_gte(x, lcm);
  cop2.MAC1 = t.x + (res.x >> 12);
  cop2.MAC2 = t.y + (res.y >> 12);
  cop2.MAC3 = t.z + (res.z >> 12);
  cop2.IR1 = cop2.MAC1;
  cop2.IR2 = cop2.MAC2;
  cop2.IR3 = cop2.MAC3;
  uint32_t r = ((cop2.RGBC >>  0) & 0xFF) << 4;
  uint32_t g = ((cop2.RGBC >>  8) & 0xFF) << 4;
  uint32_t b = ((cop2.RGBC >> 16) & 0xFF) << 4;
  cop2.MAC1 = (((int32_t)cop2.IR1)*((int32_t)r)) >> 12;
  cop2.MAC2 = (((int32_t)cop2.IR2)*((int32_t)g)) >> 12;
  cop2.MAC3 = (((int32_t)cop2.IR3)*((int32_t)b)) >> 12;
  cop2.IR1 = cop2.MAC1;
  cop2.IR2 = cop2.MAC2;
  cop2.IR3 = cop2.MAC3;

  clamp_REGS(-0x80000);

  push_rgb(get_crgb(cop2.MAC1, cop2.MAC2, cop2.MAC3,crgb_code()));
}

void OP(uint32_t sf, uint32_t lm)
{
  mat3 m = get_mat(0);
  
  cop2.MAC1 = (((int32_t)cop2.IR3)*((int32_t)m.m[1][1]) - ((int32_t)cop2.IR2)*((int32_t)m.m[2][2])) >> (sf*12);
  cop2.MAC2 = (((int32_t)cop2.IR1)*((int32_t)m.m[2][2]) - ((int32_t)cop2.IR3)*((int32_t)m.m[0][0])) >> (sf*12);
  cop2.MAC3 = (((int32_t)cop2.IR2)*((int32_t)m.m[0][0]) - ((int32_t)cop2.IR1)*((int32_t)m.m[1][1])) >> (sf*12);
  
  cop2.IR1 = cop2.MAC1;
  cop2.IR2 = cop2.MAC2;
  cop2.IR3 = cop2.MAC3;

  int min = -0x8000;
  if (lm) min = 0x0;

  clamp_REGS(min);
}

uint32_t LZCR(void)
{
  uint32_t num = cop2.LZCS;

  if (num&0x80000000) num = ~num;

  if (num == 0) return 32;

  for (int i = 0; i < 32; i++)
  {
    if (num &0x80000000) return i;
    num = num << 1;
  }

  BREAKPOINT;
  return -1;
}


void print_gte_regs(void)
{
  printf("cop2.VXY0: %.8X\n", cop2.VXY0);
  printf("cop2.VZ0: %.8X\n", cop2.VZ0);
  printf("cop2.VXY1: %.8X\n", cop2.VXY1);
  printf("cop2.VZ1: %.8X\n", cop2.VZ1);
  printf("cop2.VXY2: %.8X\n", cop2.VXY2);
  printf("cop2.VZ2: %.8X\n", cop2.VZ2);
  printf("cop2.RGBC: %.8X\n", cop2.RGBC);
  printf("cop2.OTZ: %.8X\n", cop2.OTZ);
  printf("cop2.IR0: %.8X\n", cop2.IR0);
  printf("cop2.IR1: %.8X\n", cop2.IR1);
  printf("cop2.IR2: %.8X\n", cop2.IR2);
  printf("cop2.IR3: %.8X\n", cop2.IR3);
  printf("cop2.SXY0: %.8X\n", cop2.SXY0);
  printf("cop2.SXY1: %.8X\n", cop2.SXY1);
  printf("cop2.SXY2: %.8X\n", cop2.SXY2);
  printf("cop2.SXYP: %.8X\n", cop2.SXYP);
  printf("cop2.SZ0: %.8X\n", cop2.SZ0);
  printf("cop2.SZ1: %.8X\n", cop2.SZ1);
  printf("cop2.SZ2: %.8X\n", cop2.SZ2);
  printf("cop2.SZ3: %.8X\n", cop2.SZ3);
  printf("cop2.RGB0: %.8X\n", cop2.RGB0);
  printf("cop2.RGB1: %.8X\n", cop2.RGB1);
  printf("cop2.RGB2: %.8X\n", cop2.RGB2);
  printf("cop2.RES1: %.8X\n", cop2.RES1);
  printf("cop2.MAC0: %.8X\n", cop2.MAC0);
  printf("cop2.MAC1: %.8X\n", cop2.MAC1);
  printf("cop2.MAC2: %.8X\n", cop2.MAC2);
  printf("cop2.MAC3: %.8X\n", cop2.MAC3);
  printf("cop2.IRGB: %.8X\n", cop2.IRGB);
  printf("cop2.ORGB: %.8X\n", cop2.ORGB);
  printf("cop2.LZCS: %.8X\n", cop2.LZCS);
  printf("cop2.LZCR: %.8X\n", cop2.LZCR);
  printf("cop2.RTM0: %.8X\n", cop2.RTM0);
  printf("cop2.RTM1: %.8X\n", cop2.RTM1);
  printf("cop2.RTM2: %.8X\n", cop2.RTM2);
  printf("cop2.RTM3: %.8X\n", cop2.RTM3);
  printf("cop2.RTM4: %.8X\n", cop2.RTM4);
  printf("cop2.TRX: %.8X\n", cop2.TRX);
  printf("cop2.TRY: %.8X\n", cop2.TRY);
  printf("cop2.TRZ: %.8X\n", cop2.TRZ);
  printf("cop2.LLM0: %.8X\n", cop2.LLM0);
  printf("cop2.LLM1: %.8X\n", cop2.LLM1);
  printf("cop2.LLM2: %.8X\n", cop2.LLM2);
  printf("cop2.LLM3: %.8X\n", cop2.LLM3);
  printf("cop2.LLM4: %.8X\n", cop2.LLM4);
  printf("cop2.RBK: %.8X\n", cop2.RBK);
  printf("cop2.GBK: %.8X\n", cop2.GBK);
  printf("cop2.BBK: %.8X\n", cop2.BBK);
  printf("cop2.LCM0: %.8X\n", cop2.LCM0);
  printf("cop2.LCM1: %.8X\n", cop2.LCM1);
  printf("cop2.LCM2: %.8X\n", cop2.LCM2);
  printf("cop2.LCM3: %.8X\n", cop2.LCM3);
  printf("cop2.LCM4: %.8X\n", cop2.LCM4);
  printf("cop2.RFC: %.8X\n", cop2.RFC);
  printf("cop2.GFC: %.8X\n", cop2.GFC);
  printf("cop2.BFC: %.8X\n", cop2.BFC);
  printf("cop2.OFX: %.8X\n", cop2.OFX);
  printf("cop2.OFY: %.8X\n", cop2.OFY);
  printf("cop2.H: %.8X\n", cop2.H);
  printf("cop2.DQA: %.8X\n", cop2.DQA);
  printf("cop2.DQB: %.8X\n", cop2.DQB);
  printf("cop2.ZSF3: %.8X\n", cop2.ZSF3);
  printf("cop2.ZSF4: %.8X\n", cop2.ZSF4);
}