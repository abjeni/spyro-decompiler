#pragma once

#include <stdint.h>

typedef struct {
  uint8_t r, g, b;
} color;

typedef struct {
  int64_t r, g, b;
} color_int;

typedef struct {
  int x, y;
} vec2;

typedef struct {
  int64_t x, y;
} vec2_64;

typedef struct {
  vec2 v;
  vec2 uv;
  color c;
} vertex;

typedef struct {
  vec2 v;
  vec2_64 uv;
  color_int c;
} vertex_int;

void draw_triangle(vertex v[static 3], int x1, int x2, int y1, int y2, void (*set_pixel)(vertex v));
void draw_line(vertex v[static 2], int x1, int x2, int y1, int y2, void (*set_pixel)(vertex v));
void draw_triangle_solid_color(vec2 v[static 3], color c, int x1, int x2, int y1, int y2, void (*set_pixel)(vertex v));
void draw_triangle_color(vertex v[static 3], int x1, int x2, int y1, int y2, void (*set_pixel)(vertex v));