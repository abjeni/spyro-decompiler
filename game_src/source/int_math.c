#include <stdint.h>

#include "int_math.h"

int16_t cos_lut[] = {
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

uint32_t spyro_cos(uint32_t angle)
{
  angle = angle & 0xFFF;
  uint32_t remainder = angle & 0xF;
  uint32_t index = angle >> 4;

  uint32_t cos = cos_lut[index];
  if (remainder) {
    uint32_t next = cos_lut[(index+1)&0xFF];
    cos += (int32_t)((int32_t)remainder*(next-cos)) >> 4;
  }
  return cos;
}

uint32_t spyro_sin(uint32_t angle)
{
  return spyro_cos(angle-1024);
}

int32_t fixed_multi(int32_t a, int32_t b)
{
  return (a*b) >> 12;
}

uint32_t fixed_multu(uint32_t a, uint32_t b)
{
  return (a*b) >> 12;
}

int32_t absint(int32_t x)
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
    4096, 0, 0,
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
    0, 4096, 0,
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
    0, 0, 4096
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