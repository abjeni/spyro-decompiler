#pragma once

#include <stdint.h>

typedef struct {
  uint8_t r, g, b;
} color;

typedef struct {
  int64_t r, g, b;
} color_big;

typedef struct {
  int x, y;
} vec2t;

typedef struct {
  int64_t x, y;
} vec2t_64;

typedef struct {
  vec2t v;
  vec2t uv;
  color c;
} vertex;

typedef struct {
  vec2t v;
  vec2t_64 uv;
  color_big c;
} vertex_int;

void draw_triangle(vertex v[static 3], int x1, int x2, int y1, int y2, void (*set_pixel)(vertex v));
void draw_line(vertex v[static 2], int x1, int x2, int y1, int y2, void (*set_pixel)(vertex v));
void draw_triangle_solid_color(vec2t v[static 3], color c, int x1, int x2, int y1, int y2, void (*set_pixel)(vertex v));
void draw_triangle_color(vertex v[static 3], int x1, int x2, int y1, int y2, void (*set_pixel)(vertex v));