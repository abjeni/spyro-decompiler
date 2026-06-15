#pragma once

#include <stdint.h>
#include <stddef.h>

void init_game_window();
void update_vram(uint8_t *mem, uint32_t X, uint32_t Y, uint8_t bit24);
void set_resolution(int x, int y);
int handle_input();
void wait_frame(void);
uint16_t get_input();


void *platform_aquire_rom(void);
void *platform_allocate(size_t memory_size);
int platform_init(void);

extern void console_log(char *msg, int size);