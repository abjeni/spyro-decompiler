#pragma once

#include <stdint.h>

void set_timer(uint32_t duration);
uint32_t get_timer(void);

void do_literally_nothing(void);

uint32_t dma_callback(uint32_t dma_num, uint32_t callback);
uint32_t dma_callback2(uint32_t dma_num, uint32_t callback);

void spyro_patch_bios(void);