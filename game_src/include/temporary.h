#pragma once
#include <stdint.h>
#include "spyro_psy.h"

////////////////////////
// spyro_controller.h //
////////////////////////

void init_controller(void);
void controller_wait_for_data(void);
void stop_registering_gamepads(void);

////////////////////////
// spyro_controller.h //
////////////////////////

//////////////////////
// spyro_graphics.h //
//////////////////////

void initial_loading_screen(void);
void GP1_command(uint32_t cmd);
uint32_t fill_color(RECT *rect, uint32_t color);
uint32_t ram_to_vram(RECT *rect, uint16_t *data);
uint32_t vram_to_ram(RECT *rect, uint16_t *data);
uint32_t command_queue_append(uint32_t func, uint32_t data, uint32_t data_size, uint32_t data_or_color);
void execute_gpu_linked_list(void *node);
void spyro_clear_screen(DR_ENV *dr_env, DRAWENV env);
uint32_t psx_has_2mb_vram();
uint32_t spyro_draw_mode_setting_command(uint8_t dfe, uint8_t dtd, uint16_t tpage);
uint32_t spyro_set_texture_window_setting_command(RECT *tw);

//////////////////////
// spyro_graphics.h //
//////////////////////