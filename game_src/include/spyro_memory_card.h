#pragma once

#include <stdint.h>

uint32_t init_memory_card(void);
void MemCardStart(void);
void MemCardStop(void);

uint32_t savegame_checksum(uint8_t *savegame);