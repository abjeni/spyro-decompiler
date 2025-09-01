#pragma once

#include <stdint.h>

typedef struct __attribute__((packed)) {
  int16_t x, y, w, h;
} RECT;

typedef struct __attribute__((packed)) {
  uint32_t tag;
  uint32_t code[15];
} DR_ENV;

typedef struct {
  uint32_t tag;
  uint32_t code[2];
} DR_MODE;

typedef struct __attribute__((packed)) {
  RECT clip; // 0
  int16_t ofs[2]; // 8
  RECT tw; // C
  uint16_t tpage; // 14
  uint8_t dtd; // 16
  uint8_t dfe; // 17
  uint8_t isbg; // 18

  uint8_t r0, g0, b0; // 19, 1A, 1B
  DR_ENV dr_env; // 1C
} DRAWENV;

typedef struct __attribute__((packed)) {
  RECT disp; // 0
  RECT screen; // 8
  uint8_t isinter; // 10
  uint8_t isrgb24; // 11
  uint8_t pad0, pad1; // 12, 13
} DISPENV;

// i made this one up
typedef struct __attribute__((packed)) {
  DRAWENV draw;
  DISPENV disp;
  uint32_t memory_arena;
  uint32_t memory_ordered_link_list;
  uint32_t memory_link_list;
  uint32_t unknown4;
  uint32_t unknown5;
} DISP;

// size: 0x00000010
void spyro_srand(int32_t seed);
int spyro_rand(void);

int32_t ResetGraph(int32_t mode);
void SetDispMask(int32_t mask);
uint32_t DrawSync(uint32_t mode);
int32_t SetGraphDebug(int32_t level);
int32_t ClearImage(RECT *rect, uint8_t r, uint8_t g, uint8_t b);
int32_t LoadImage(RECT *recp, void *p);
int32_t StoreImage(RECT *recp, void *p);
int32_t MoveImage(RECT *recp, int32_t x, int32_t y);
void DrawOTag(uint32_t *of);
DRAWENV *PutDrawEnv(DRAWENV *env);
DISPENV *PutDispEnv(DISPENV *env);
void SetDrawMode(DR_MODE *p, int32_t dfe, int32_t dfd, int32_t tpage, RECT *tw);