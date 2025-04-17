#pragma once

#include <stdint.h>

void spyro_mat_mul(uint32_t m1, uint32_t m2, uint32_t dst);
int16_t spyro_atan(int32_t x, int32_t y);
int16_t spyro_atan2(int32_t x, int32_t y, uint32_t a3);
int16_t spyro_cos(int32_t angle);
int16_t spyro_sin(int32_t angle);
uint32_t spyro_log2_uint(uint32_t a);
void spyro_mat3_rotation(uint32_t rot_addr, uint32_t dst, uint32_t src);
void spyro_mat3_transpose(uint32_t dst, uint32_t src);
void spyro_set_mat_mirrored_vec_multiply(uint32_t mat, uint32_t vec_src, uint32_t vec_dst);
void spyro_mat_mirrored_vec_multiply(uint32_t vec_src, uint32_t vec_dst);
void spyro_camera_mat_vec_multiply(uint32_t vec_src, uint32_t vec_dst);
uint32_t spyro_vec_length(uint32_t vec, uint32_t is_vec3);
void spyro_set_vec3_length(uint32_t vec, int32_t len);
void vec3_mul_div(uint32_t vec, int32_t div, int32_t mul);
void spyro_vec3_multiply_fancy_shift_right(uint32_t vec, uint32_t mult, uint32_t shift);
void spyro_vec3_shift_left(uint32_t vec, uint32_t shift);
void spyro_vec3_shift_right(uint32_t vec, uint32_t shift);
void spyro_vec3_clear(uint32_t vec);
void spyro_vec3_copy(uint32_t dst, uint32_t src);
void spyro_vec3_add(uint32_t dst, uint32_t a, uint32_t b);
void spyro_vec3_sub(uint32_t dst, uint32_t a, uint32_t b);
void spyro_vec3_mul(uint32_t dst, uint32_t src, int32_t mul);
void spyro_vec3_div(uint32_t dst, uint32_t src, int32_t div);
void spyro_vec_interpolation(uint32_t dst, uint32_t vec1, uint32_t vec2, int32_t ipol);
uint32_t spyro_two_angle_diff_8bit(uint32_t a, uint32_t b);
uint32_t spyro_two_angle_diff_12bit(uint32_t a, uint32_t b);
uint32_t spyro_two_angle_signed_diff_8bit(int32_t a, int32_t b);
uint32_t spyro_two_angle_signed_diff_12bit(int32_t a, int32_t b);
uint32_t spyro_octagon_distance(uint32_t a, uint32_t b);
uint32_t spyro_attract_angle_in_range(uint32_t a, uint32_t b, int32_t attraction, int32_t range);
uint32_t spyro_sqrt(uint32_t a);
void spyro_world_to_screen_projection(uint32_t dst, uint32_t vec);
void spyro_world_to_screen_projection_with_right_shift(uint32_t dst, uint32_t vec, uint32_t shift);
void spyro_vec_32_to_16_div_4(uint32_t dst, uint32_t src);
void spyro_vec_16_to_32_mul_4(uint32_t dst, uint32_t src);
void spyro_vec_16_to_32(uint32_t dst, uint32_t src);
void spyro_vec_32_to_16(uint32_t dst, uint32_t src);
void spyro_vec_16_add(uint32_t dst, uint32_t a, uint32_t b);
void spyro_unpack_96bit_triangle(uint32_t index, uint32_t dst);
uint32_t interpolate_color(uint32_t c1, uint32_t c2, int32_t ipol);


extern int16_t cos_lut[256];
extern int16_t sin_lut[256];