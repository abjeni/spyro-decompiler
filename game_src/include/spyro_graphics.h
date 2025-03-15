#pragma once

#include <stdint.h>
#include "spyro_psy.h"

uint32_t spyro_set_drawing_area_top_left_command(int16_t x, int16_t y);
uint32_t spyro_set_drawing_area_bottom_right_command(int16_t x, int16_t y);
uint32_t spyro_set_drawing_offset_command(int16_t x, int16_t y);
uint32_t spyro_draw_mode_setting_command(uint32_t dfe, uint32_t dtd, uint32_t tpage);
uint32_t spyro_set_texture_window_setting_command(RECT *tw);
void spyro_clear_screen(DR_ENV *cmd_ptr, DRAWENV screen_info);

uint32_t psx_has_2mb_vram(void);

uint32_t vram_to_ram(RECT *rect, uint32_t *data);
uint32_t ram_to_vram(RECT *rect, uint32_t *data);

void execute_gpu_linked_list(void *node);

uint32_t fill_color(RECT *rect, uint32_t color);

uint32_t *spyro_combine_all_command_buffers(uint32_t depth_command_buffers);

uint32_t command_queue_append(uint32_t func, uint32_t data, uint32_t data_size, uint32_t data_or_color);

uint32_t gpu_internal_register(uint32_t reg);
void enable_dma_linked_list(uint32_t *node);

uint32_t gpu_check_timeout(void);
void gpu_start_timeout(void);

void GP1_cmd(uint32_t cmd);

void SetGeomOffset(uint32_t ofx, uint32_t ofy);
void SetGeomScreen(uint32_t h);

void initial_loading_screen(void);