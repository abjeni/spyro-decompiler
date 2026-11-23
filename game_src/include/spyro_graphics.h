#pragma once

#include <stdint.h>
#include "spyro_psy.h"
#include "int_math.h"

typedef struct {
  uint32_t end; // node*
  uint32_t base; // node*
} link_list;

typedef struct {
  uint32_t ptr;
  vec3 v;
} ptr_vec;

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} color;

typedef struct {
  vec3 v;
  color c;
  uint32_t cmd;
} vertex;

typedef struct {
  uint32_t ptr;
  vertex v;
} ptr_vertex;

void store_ptr_vertex(ptr_vertex a);
color avg_color(color a, color b);
ptr_vertex avg_ptr_vertex(ptr_vertex a, ptr_vertex b);
ptr_vertex unpack_ptr_vertex(uint32_t ptr, uint32_t vec, vec3 v, uint32_t c);
void store_ptr_vec(ptr_vec a);
ptr_vec avg_ptr_vec(ptr_vec a, ptr_vec b);
void graphics_create_slope1(ptr_vertex a, ptr_vertex b);

uint32_t spyro_set_drawing_area_top_left_command(int16_t x, int16_t y);
uint32_t spyro_set_drawing_area_bottom_right_command(int16_t x, int16_t y);
uint32_t spyro_set_drawing_offset_command(int16_t x, int16_t y);
uint32_t spyro_draw_mode_setting_command(uint8_t dfe, uint8_t dtd, uint16_t tpage);
uint32_t spyro_set_texture_window_setting_command(RECT *tw);
void spyro_clear_screen(DR_ENV *cmd_ptr, DRAWENV screen_info);

uint32_t psx_has_2mb_vram(void);

uint32_t vram_to_ram(RECT *rect, uint16_t *data);
uint32_t ram_to_vram(RECT *rect, uint16_t *data);

void execute_gpu_linked_list(void *node);

void append_gpu_command_block(void *node);
void append_gpu_command_block_depth_slot(void *node, uint32_t depth_slot);

void GP1_command(uint32_t cmd);

uint32_t command_queue_advance(void);

uint32_t fill_color(RECT *rect, uint32_t color);

uint32_t *spyro_combine_all_command_buffers(uint32_t depth_command_buffers);

uint32_t command_queue_append(uint32_t func, uint32_t data, uint32_t data_size, uint32_t data_or_color);

uint32_t gpu_internal_register(uint32_t reg);
void enable_dma_linked_list(uint32_t *node);

uint32_t gpu_check_timeout(void);
void gpu_start_timeout(void);

void GP1_cmd(uint32_t cmd);

void mat3x4setTR(uint32_t mat3x4);

void SetGeomOffset(uint32_t ofx, uint32_t ofy);
void SetGeomScreen(uint32_t h);

void initial_loading_screen(void);

void draw_skybox(int32_t index, uint32_t matrix1, uint32_t matrix2);

void spyro_image_unpack(uint32_t *src, uint32_t *dst, uint32_t fade);