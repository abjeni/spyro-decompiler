#include <stdint.h>

#include "int_math.h"
#include "spyro_math.h"

int32_t fixed_multi(int32_t a, int32_t b)
{
  return (a*b) >> 12;
}

uint32_t fixed_multu(uint32_t a, uint32_t b)
{
  return (a*b) >> 12;
}

int32_t abs_int(int32_t x)
{
  if (x < 0) return -x;
  return x;
}

int32_t max_int(int32_t x, int32_t y)
{
  if (x < y) return y;
  return x;
}

int32_t min_int(int32_t x, int32_t y)
{
  if (x > y) return y;
  return x;
}

int32_t clamp_int(int32_t x, int32_t min, int32_t max)
{
  if (x > max) return max;
  if (x < min) return min;
  return x;
}

uint32_t to_rgb(uint32_t r, uint32_t g, uint32_t b)
{
  return (r << 0) + (g << 8) + (b << 16);
}

mat3 mat3_c(int16_t x00, int16_t x01, int16_t x02,
            int16_t x10, int16_t x11, int16_t x12, 
            int16_t x20, int16_t x21, int16_t x22)
{
  return (mat3){{
    {x00, x01, x02},
    {x10, x11, x12},
    {x20, x21, x22},
  }};
}

vec3_32 vec3_32_shift_right(vec3_32 v, int32_t shift)
{
  return (vec3_32){
    v.x >> shift,
    v.y >> shift,
    v.z >> shift
  };
}

vec3_32 vec3_32_shift_left(vec3_32 v, int32_t shift)
{
  return (vec3_32){
    v.x << shift,
    v.y << shift,
    v.z << shift
  };
}

vec3_32 vec_mat_mul_gte(vec3 v, mat3 m)
{
  return (vec3_32){
    (int32_t)m.m[0][0]*v.x + (int32_t)m.m[0][1]*v.y + (int32_t)m.m[0][2]*v.z,
    (int32_t)m.m[1][0]*v.x + (int32_t)m.m[1][1]*v.y + (int32_t)m.m[1][2]*v.z,
    (int32_t)m.m[2][0]*v.x + (int32_t)m.m[2][1]*v.y + (int32_t)m.m[2][2]*v.z
  };
}

vec3_32 vec3_32_mat_mul(vec3_32 v, mat3 m)
{
  return vec3_32_shift_right((vec3_32){
    (int32_t)m.m[0][0]*v.x + (int32_t)m.m[0][1]*v.y + (int32_t)m.m[0][2]*v.z,
    (int32_t)m.m[1][0]*v.x + (int32_t)m.m[1][1]*v.y + (int32_t)m.m[1][2]*v.z,
    (int32_t)m.m[2][0]*v.x + (int32_t)m.m[2][1]*v.y + (int32_t)m.m[2][2]*v.z
  }, 12);
}

vec3_32 vec3_32_sub(vec3_32 a, vec3_32 b)
{
  return (vec3_32){
    a.x - b.x,
    a.y - b.y,
    a.z - b.z
  };
}

vec3 vec3_shift_right(vec3 v, int32_t shift)
{
  return (vec3){
    v.x >> shift,
    v.y >> shift,
    v.z >> shift
  };
}

vec3 vec3_shift_left(vec3 v, int32_t shift)
{
  return (vec3){
    v.x << shift,
    v.y << shift,
    v.z << shift
  };
}

vec3 vec3_mat_mul(vec3 v, mat3 m)
{
  vec3_32 v32 = vec3_32_shift_right((vec3_32){
    (int32_t)m.m[0][0]*v.x + (int32_t)m.m[0][1]*v.y + (int32_t)m.m[0][2]*v.z,
    (int32_t)m.m[1][0]*v.x + (int32_t)m.m[1][1]*v.y + (int32_t)m.m[1][2]*v.z,
    (int32_t)m.m[2][0]*v.x + (int32_t)m.m[2][1]*v.y + (int32_t)m.m[2][2]*v.z
  }, 12);
  return (vec3){v32.x, v32.y, v32.z};
}

vec3 unpack_uint32_vec3(uint32_t val)
{
  return (vec3){
    (val >> 21) & 0x7FF,
    (val >> 10) & 0x7FF,
    (val >>  0) & 0x3FF
  };
}

vec3 unpack_uint32_vec3_888(uint32_t val)
{
  return (vec3){
    (val >>  0) & 0xFF,
    (val >>  8) & 0xFF,
    (val >> 16) & 0xFF
  };
}

uint32_t pack_vec3_uint32(vec3 v)
{
  return ((v.x & 0x7FF) << 21) + ((v.y & 0x7FF) << 10) + ((v.z & 0x3FF) << 0);
}

mat3 mat3_mul(mat3 a, mat3 b)
{
  mat3 m = {0};

  for (int y = 0; y < 3; y++)
    for (int x = 0; x < 3; x++)
    {
      int32_t sum = 0;
      for (int i = 0; i < 3; i++)
        sum += (int32_t)(a.m[y][i] * b.m[i][x]);
      m.m[y][x] = sum >> 12;
    }

  return m;
}

mat3 mat3rotX(uint32_t a)
{
  uint32_t c = spyro_cos(a);
  uint32_t s = spyro_sin(a);
  return mat3_c(
    0x1000, 0, 0,
    0, c, -s,
    0, s, c
  );
}

mat3 mat3rotY(uint32_t a)
{
  uint32_t c = spyro_cos(a);
  uint32_t s = spyro_sin(a);
  return mat3_c(
    c, 0, s,
    0, 0x1000, 0,
    -s, 0, c
  );
}

mat3 mat3rotZ(uint32_t a)
{
  uint32_t c = spyro_cos(a);
  uint32_t s = spyro_sin(a);
  return mat3_c(
    c, s, 0,
    -s, c, 0,
    0, 0, 0x1000
  );
}

mat3 list_to_mat3(uint32_t list[5])
{
  return mat3_c(
    list[0] & 0xFFFF,
    list[0] >> 16,
    list[1] & 0xFFFF,
    list[1] >> 16,
    list[2] & 0xFFFF,
    list[2] >> 16,
    list[3] & 0xFFFF,
    list[3] >> 16,
    list[4] & 0xFFFF
  );
}

void mat3_to_list(mat3 m, uint32_t list[5])
{
  list[0] = (uint32_t)((uint16_t)m.m[0][0]) | (m.m[0][1] << 16);
  list[1] = (uint32_t)((uint16_t)m.m[0][2]) | (m.m[1][0] << 16);
  list[2] = (uint32_t)((uint16_t)m.m[1][1]) | (m.m[1][2] << 16);
  list[3] = (uint32_t)((uint16_t)m.m[2][0]) | (m.m[2][1] << 16);
  list[4] = (uint32_t)((uint16_t)m.m[2][2]);
}

mat3 mat3_identity(void)
{
  return mat3_c(
    4096, 0, 0,
    0, 4096, 0,
    0, 0, 4096
  );
}

mat3 mat3_transpose(mat3 m)
{
  int16_t tmp;

  tmp = m.m[1][0];
  m.m[1][0] = m.m[0][1];
  m.m[0][1] = tmp;

  tmp = m.m[2][0];
  m.m[2][0] = m.m[0][2];
  m.m[0][2] = tmp;

  tmp = m.m[2][1];
  m.m[2][1] = m.m[1][2];
  m.m[1][2] = tmp;
  
  return m;
}