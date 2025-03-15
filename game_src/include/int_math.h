#pragma once

#include <stdint.h>

typedef struct {
  int16_t m[3][3];
} mat3;

typedef struct {
  int16_t x, y, z;
} vec3;

typedef struct {
  int16_t x, y, z, w;
} vec4;

typedef struct {
  int32_t x, y, z;
} vec3_32;

int32_t fixed_multi(int32_t a, int32_t b);
uint32_t fixed_multu(uint32_t a, uint32_t b);

int32_t abs_int(int32_t x);
int32_t max_int(int32_t x, int32_t y);
int32_t min_int(int32_t x, int32_t y);
int32_t clamp_int(int32_t x, int32_t min, int32_t max);

uint32_t to_rgb(uint32_t r, uint32_t g, uint32_t b);

mat3 mat3_c(int16_t x00, int16_t x01, int16_t x02,
            int16_t x10, int16_t x11, int16_t x12, 
            int16_t x20, int16_t x21, int16_t x22);

mat3 list_to_mat3(uint32_t list[5]);
void mat3_to_list(mat3 m, uint32_t list[5]);

mat3 mat3_mul(mat3 a, mat3 b);

vec3_32 vec_mat_mul_gte(vec3 v, mat3 m);
vec3_32 vec3_32_shift_right(vec3_32 v, int32_t shift);
vec3_32 vec3_32_shift_left(vec3_32 v, int32_t shift);
vec3_32 vec3_32_mat_mul(vec3_32 v, mat3 m);
vec3 vec3_shift_right(vec3 v, int32_t shift);
vec3 vec3_shift_left(vec3 v, int32_t shift);
vec3 vec3_mat_mul(vec3 v, mat3 m);
vec3 unpack_uint32_vec3_888(uint32_t val);
vec3 unpack_uint32_vec3(uint32_t val);
uint32_t pack_vec3_uint32(vec3 v);
mat3 mat3rotX(uint32_t a);
mat3 mat3rotY(uint32_t a);
mat3 mat3rotZ(uint32_t a);
mat3 mat3_identity(void);
mat3 mat3_transpose(mat3 m);