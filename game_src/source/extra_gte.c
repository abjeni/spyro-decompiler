#include <stdio.h>
#include <stdint.h>

#include "psx_mem.h"
#include "gte.h"
#include "int_math.h"

vec3 load_vec3(uint32_t addr)
{
  return (vec3){
    lh(addr + 0),
    lh(addr + 2),
    lh(addr + 4)
  };
}

vec3_32 load_vec3_32(uint32_t addr)
{
  return (vec3_32){
    lw(addr + 0),
    lw(addr + 4),
    lw(addr + 8)
  };
}

void load_RTM(uint32_t addr)
{
  cop2.RTM0 = lw(addr + 0x00);
  cop2.RTM1 = lw(addr + 0x04);
  cop2.RTM2 = lw(addr + 0x08);
  cop2.RTM3 = lw(addr + 0x0C);
  cop2.RTM4 = lw(addr + 0x10);
}

mat3 get_RTM(void)
{
  return list_to_mat3((uint32_t[]){cop2.RTM0,cop2.RTM1,cop2.RTM2,cop2.RTM3,cop2.RTM4});
}

mat3 get_LLM(void)
{
  return list_to_mat3((uint32_t[]){cop2.LLM0,cop2.LLM1,cop2.LLM2,cop2.LLM3,cop2.LLM4});
}

void set_RTM1(uint32_t list[5])
{
  cop2.RTM0 = list[0];
  cop2.RTM1 = list[1];
  cop2.RTM2 = list[2];
  cop2.RTM3 = list[3];
  cop2.RTM4 = list[4];
}

void set_RTM(mat3 m)
{
  uint32_t list[5];
  mat3_to_list(m, list);
  set_RTM1(list);
}

void load_TR(uint32_t addr)
{
  cop2.TRX = lw(addr + 0);
  cop2.TRY = lw(addr + 4);
  cop2.TRZ = lw(addr + 8);
}

void set_TR(uint32_t trx, uint32_t try, uint32_t trz)
{
  cop2.TRX = trx;
  cop2.TRY = try;
  cop2.TRZ = trz;
}

void set_V0(uint32_t v0x, uint32_t v0y, uint32_t v0z)
{
  cop2.VXY0 = v0x + (v0y << 16);
  cop2.VZ0 = v0z;
}

void set_V0_vec3(vec3 v0)
{
  cop2.VXY0 = ((uint32_t)v0.x) + ((uint32_t)v0.y << 16);
  cop2.VZ0 = v0.z;
}

vec3 get_S2(void)
{
  return xy_z_to_vec(cop2.SXY2, cop2.SZ2);
}

void load_IR(uint32_t addr)
{
  cop2.IR1 = lw(addr + 0);
  cop2.IR2 = lw(addr + 4);
  cop2.IR3 = lw(addr + 8);
}

void set_IR(uint32_t irx, uint32_t iry, uint32_t irz)
{
  cop2.IR1 = irx;
  cop2.IR2 = iry;
  cop2.IR3 = irz;
}

void set_IR_vec3(vec3 ir)
{
  cop2.IR1 = ir.x;
  cop2.IR2 = ir.y;
  cop2.IR3 = ir.z;
}

void get_IR(uint32_t *irx, uint32_t *iry, uint32_t *irz)
{
  *irx = cop2.IR1;
  *iry = cop2.IR2;
  *irz = cop2.IR3;
}

vec3 get_IR_vec3(void)
{
  return (vec3) {
    cop2.IR1,
    cop2.IR2,
    cop2.IR3
  };
}

void set_FC_vec3(vec3 fc)
{
  cop2.RFC = fc.x;
  cop2.GFC = fc.y;
  cop2.BFC = fc.z;
}

void set_BK_vec3(vec3 bk)
{
  cop2.RBK = bk.x;
  cop2.GBK = bk.y;
  cop2.BBK = bk.z;
}

vec3 get_MAC_vec3(void)
{
  return (vec3) {
    cop2.MAC1,
    cop2.MAC2,
    cop2.MAC3
  };
}

void save_mat3(uint32_t addr, mat3 m)
{
  sh(addr+ 0, m.m[0][0]);
  sh(addr+ 2, m.m[0][1]);
  sh(addr+ 4, m.m[0][2]);
  sh(addr+ 6, m.m[1][0]);
  sh(addr+ 8, m.m[1][1]);
  sh(addr+10, m.m[1][2]);
  sh(addr+12, m.m[2][0]);
  sh(addr+14, m.m[2][1]);
  sh(addr+16, m.m[2][2]);
}

mat3 load_mat3(uint32_t addr)
{
  return mat3_c(
    lh(addr+ 0),
    lh(addr+ 2),
    lh(addr+ 4),
    lh(addr+ 6),
    lh(addr+ 8),
    lh(addr+10),
    lh(addr+12),
    lh(addr+14),
    lh(addr+16)
  );
}

void print_mat3(mat3 m)
{
  for (int y = 0; y < 3; y++)
  {
    printf("  ");
    for (int x = 0; x < 3; x++)
      printf("%4.4hX ", m.m[y][x]);
    printf("\n");
  }
}