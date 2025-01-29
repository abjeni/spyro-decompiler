#pragma once

#include <stdint.h>

void init_vram_view();
void update_vram(uint8_t *mem, uint32_t X, uint32_t Y);
void handle_input();
void wait_frame(void);
uint16_t get_input();
