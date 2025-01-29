#pragma once

#include <stdint.h>
#include "debug.h"

void start_frame();
void init_psx_mem();

void enable_timer();
void disable_timer();

void set_lock();
void release_lock();

void psx_read_sectors(uint32_t dst, uint32_t sector, uint32_t sector_len);

file_loc get_access(uint32_t addr);
void print_access(uint32_t addr);

void sw_unaligned(uint32_t addr, uint32_t value, file_loc loc);
void sw(uint32_t addr, uint32_t value, file_loc loc);
void sh(uint32_t addr, uint16_t value, file_loc loc);
void sb(uint32_t addr, uint8_t  value, file_loc loc);
uint32_t lw_unaligned(uint32_t addr, file_loc loc);
uint32_t lw(uint32_t addr, file_loc loc);
uint32_t lh(uint32_t addr, file_loc loc);
uint32_t lb(uint32_t addr, file_loc loc);

uint32_t lhu(uint32_t addr, file_loc loc);
uint32_t lbu(uint32_t addr, file_loc loc);

void psx_test_render(uint32_t cmd_ptr, uint32_t depth, file_loc loc);

#define LOC (file_loc){__FILE__, __LINE__}

#define psx_test_render(cmd_ptr, depth) psx_test_render(cmd_ptr, depth, LOC)

#define sw_unaligned(addr, value) sw_unaligned(addr, value, LOC)
#define sw(addr, value) sw(addr, value, LOC)
#define sh(addr, value) sh(addr, value, LOC)
#define sb(addr, value) sb(addr, value, LOC)
#define lw_unaligned(addr) lw_unaligned(addr, LOC)
#define lw(addr) lw(addr, LOC)
#define lh(addr) lh(addr, LOC)
#define lb(addr) lb(addr, LOC)

#define lhu(addr) lhu(addr, LOC)
#define lbu(addr) lbu(addr, LOC)

void *addr_to_pointer(uint32_t addr);

void inter(int type);