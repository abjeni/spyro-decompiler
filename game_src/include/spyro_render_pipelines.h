#pragma once

#include "int_math.h"
#include "spyro_psy.h"
#include <stdint.h>

void gui_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void blinking_arrow(vec3_32 p, uint32_t frame, int32_t direction);
void gui_box_balloonist(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void rescued_dragon_name(void);
void fade_in(uint32_t semi_transparency, uint8_t r, uint8_t g, uint8_t b);
void draw_sprite(RECT *box, uint32_t sprite[], vec3_32 *col);
void rgb_to_grey(void *buf, uint32_t len);