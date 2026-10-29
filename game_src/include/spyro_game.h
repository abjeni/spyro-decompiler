#pragma once

#include "int_math.h"

#include <stddef.h>
#include <stdint.h>

struct game_object {
  uint32_t unknown00; // 0x00 - 0x04 // pointer
  uint32_t unknown04; // 0x04 - 0x08
  uint32_t unknown08;
  vec3_32 p; // 0x0C - 0x18
  uint32_t unknown18; // 0x18 - 0x1C
  uint32_t unknown1C; // 0x1C - 0x20
  mat3 m; // 0x20 - 0x32
  uint8_t padding1[0x02];
  int16_t cellID; // 0x34 - 0x36
  uint16_t modelID; // 0x36 - 0x38
  uint16_t unknown38; // 0x38 - 0x3A
  uint16_t unknown3A; // 0x3A - 0x3C
  uint8_t unknown3C; // 0x3C - 0x3D
  uint8_t unknown3D; // 0x3D - 0x3E
  uint8_t unknown3E; // 0x3E - 0x3F
  uint8_t unknown3F; // 0x3F - 0x40
  int8_t unknown40; // 0x40 - 0x41
  uint8_t unknown41; // 0x41 - 0x42
  uint8_t unknown42; // 0x42 - 0x43
  int8_t unknown43; // 0x43 - 0x44
  uint8_t rotx; // 0x44 - 0x45
  uint8_t roty; // 0x45 - 0x46
  uint8_t rotz; // 0x46 - 0x47
  uint8_t unknown47; // 0x47 - 0x48
  int8_t unknown48; // 0x48 - 0x49
  uint8_t unknown49; // 0x49 - 0x4A
  uint8_t unknown4A; // 0x4A - 0x4B
  uint8_t unknown4B; // 0x4B - 0x4C
  uint8_t unknown4C; // 0x4C - 0x4D
  uint8_t unknown4D; // 0x4D - 0x4E
  uint8_t unknown4E; // 0x4E - 0x4F
  uint8_t unknown4F; // 0x4F - 0x50 material id?
  uint8_t render_distance; // 0x50 - 0x51
  int8_t unknown51; // 0x51 - 0x52
  int8_t unknown52; // 0x52 - 0x53
  uint8_t unknown53; // 0x53 - 0x54
  uint8_t unknown54; // 0x54 - 0x55
  uint8_t unknown55; // 0x55 - 0x56
  uint8_t unknown56; // 0x56 - 0x57
  uint8_t unknown57; // 0x57 - 0x58
};

#define MODEL_ID_CHEST 0x1D9
#define MODEL_ID_0 0x104 // rest of the numbers following
#define MODEL_ID_A 0x1AA // rest of the uppercase alphabet following
#define MODEL_ID_FORWARDSLASH 0x115
#define MODEL_ID_QUESTION_MARK 0x116
#define MODEL_ID_PERCENT 0x110
#define MODEL_ID_CARET 0x141
#define MODEL_ID_PLUS 0x13D
#define MODEL_ID_DOT 0x147
#define MODEL_ID_EXCLAMATIONMARK 0x4B
#define MODEL_ID_COMMA 0x4C

uint32_t completion_percentage(void);

struct game_object *create_3d_text2(const char *str, vec3_32 *pos, uint32_t spacing, uint32_t unknown_metadata);

struct game_object *create_3d_text1(const char *str, vec3_32 *pos, vec3_32 size, uint32_t unknown_spacing, uint32_t unknown_metadata);

void new_game_object(struct game_object *game_object);

void game_loop(void);

// these are animations
#define MOVESTATE_STAND 0
#define MOVESTATE_STARTING 1
#define MOVESTATE_RUN 2
#define MOVESTATE_STOP 3
#define MOVESTATE_JUMP 5
#define MOVESTATE_FALL 6
#define MOVESTATE_CHARGE 11
#define MOVESTATE_CRASH 12
#define MOVESTATE_GLIDE 15
#define MOVESTATE_VORTEX 17
#define MOVESTATE_IDLE_LOOKAROUND 18
#define MOVESTATE_CHARGE_JUMP 20
#define MOVESTATE_WALK 21
#define MOVESTATE_IDLE_HAIR 39

#define GAMESTATE_INGAME 0
#define GAMESTATE_LOADING 1
#define GAMESTATE_PAUSE 2
#define GAMESTATE_INVENTORY 3
#define GAMESTATE_DEAD 4
#define GAMESTATE_GAMEOVER 5
#define GAMESTATE_UNKNOWN 6
#define GAMESTATE_FLIGHT_LEVEL_MENU 7
#define GAMESTATE_DRAGON_FREED 8
#define GAMESTATE_LOADING_TRANSITION 9
#define GAMESTATE_PREPARE_LOADING 10
#define GAMESTATE_FAIRY 11
#define GAMESTATE_BALLOONIST 12
#define GAMESTATE_TITLE_SCREEN 13
#define GAMESTATE_CUTSCENE 14
#define GAMESTATE_CREDITS 15

void spyro_change_movestate(uint32_t movestate);

/*

known model ids:
0x4B: '!'
0x4C: ','
0x53: red gem
0x54: green gem
0x55: blue gem
0x56: yellow gem
0x57: purple gem
0x104-0x10E: numbers '0' to '9'
0x110: '%'
0x115: '/'
0x116: '?'
0x13D: '+'
0x141: '^'
0x147: '.'
0x1AA-0x1D4: letters 'A' to 'Z'
0x1D9: chest model
*/