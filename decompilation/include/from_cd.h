#pragma once

#include <stdio.h>
#include <stdint.h>

int load_sectors(FILE *file, char *psx_mem, uint32_t base_addr, uint32_t sector, uint32_t sectors);
int decompile_everything(void);