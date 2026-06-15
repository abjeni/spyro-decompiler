#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

void init_game_window(void);
void update_vram(uint8_t *mem, uint32_t X, uint32_t Y, uint8_t bit24);
void set_resolution(int x, int y);
int handle_input(void);
void wait_frame(void);
uint16_t get_input(void);

void *platform_aquire_rom(void);
void *platform_allocate(size_t memory_size);
int platform_init(void);

int memcard_open(char *file_name, uint32_t modev, uint32_t *size);
int memcard_lseek(int fd, uint32_t offset, int seektype);
ssize_t memcard_write(int fd, void *buf, int size);
ssize_t memcard_read(int fd, void *buf, int size);
void memcard_close(int fd);