#pragma once

#include <stdint.h>

struct savegame {
  uint8_t level_id; // 0x00
  uint8_t unknown0; // 0x01
  uint8_t unknown1; // 0x02
  uint8_t unknown2; // 0x03
  uint32_t camera_mode; // 0x04
  uint8_t unknown3; // 0x08
  uint8_t unknown4; // 0x09
  uint8_t unknown5; // 0x0A
  uint8_t lives; // 0x0B
  uint32_t unknown6; // 0x0C
  uint8_t unknown7[0x14]; // 0x10
  uint8_t unknown8[5*5]; // 0x24
  uint8_t unknown9[3]; // 0x3D
  uint8_t unknown10[36]; // 0x40
  uint8_t unknown11[36]; // 0x64
  uint8_t level_dragons[36]; // 0x88
  uint16_t level_gems[36]; // 0xAC
  uint8_t level_eggs[18]; // 0xF4
  uint8_t unknown12[6]; // 0x106
  uint8_t unknown13[0x480]; // 0x10C
  uint32_t checksum; // 0x58C
  uint8_t unknown14[0x70]; // 0x590
};

uint32_t init_memory_card(void);
void MemCardStart(void);
void MemCardStop(void);

uint32_t savegame_checksum(struct savegame *savegame);
void create_savedata(struct savegame *save);