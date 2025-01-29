#pragma once

#include <stdint.h>
#include "psx_mem.h"

void divu_psx(uint32_t a, uint32_t b);
void div_psx(int32_t a, int32_t b);
void multu(uint32_t a, uint32_t b);
void mult(int32_t a, int32_t b);
void system_call(file_loc loc);
void JR(uint32_t addr, uint32_t location, file_loc loc);
void JALR(uint32_t addr, uint32_t location, file_loc loc);

#define system_call() system_call(LOC)

#define JR(addr, location) JR(addr, location, LOC)
#define JALR(addr, location) JALR(addr, location, LOC)