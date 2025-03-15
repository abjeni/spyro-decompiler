#pragma once

#include <stdint.h>

void init_game_window();
void update_vram(uint8_t *mem, uint32_t X, uint32_t Y, uint8_t bit24);
void set_resolution(int x, int y);
void handle_input();
void wait_frame(void);
uint16_t get_input();
