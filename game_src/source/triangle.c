#include <unistd.h>
#include <stdlib.h>
#include "triangle.h"

inline static color mix_colors(color c1, color c2, int a, int b)
{
  return (color){
    c1.r+(c2.r-c1.r)*a/b,
    c1.g+(c2.g-c1.g)*a/b,
    c1.b+(c2.b-c1.b)*a/b
  };
}

inline static color_int mix_colors2(color c1, color c2, int a, int b, int b2)
{
  return (color_int){
    ((int)c1.r*b+((int)c2.r-(int)c1.r)*a)*b2,
    ((int)c1.g*b+((int)c2.g-(int)c1.g)*a)*b2,
    ((int)c1.b*b+((int)c2.b-(int)c1.b)*a)*b2
  };
}

inline static color mix_colors3(color_int c1, color_int c2, int a, int b, int b2)
{
  return (color){
    (c1.r*b+(c2.r-c1.r)*a)/(b*b2),
    (c1.g*b+(c2.g-c1.g)*a)/(b*b2),
    (c1.b*b+(c2.b-c1.b)*a)/(b*b2)
  };
}

inline static vec2 mix_vec2(vec2 uv1, vec2 uv2, int a, int b)
{
  return (vec2){
    uv1.x+(uv2.x-uv1.x)*a/b,
    uv1.y+(uv2.y-uv1.y)*a/b
  };
}

inline static vec2_64 mix_vec22(vec2 uv1, vec2 uv2, int a, int b, int b2)
{
  return (vec2_64){
    (uv1.x*b+(uv2.x-uv1.x)*a)*b2,
    (uv1.y*b+(uv2.y-uv1.y)*a)*b2
  };
}

inline static vec2 mix_vec23(vec2_64 uv1, vec2_64 uv2, int a, int b, int b2)
{
  return (vec2){
    (uv1.x*b+(uv2.x-uv1.x)*a)/(b*b2),
    (uv1.y*b+(uv2.y-uv1.y)*a)/(b*b2)
  };
}

inline static vertex mix_vertices(vertex v1, vertex v2, int a, int b)
{
  return (vertex){
    .v.x = v1.v.x+(v2.v.x-v1.v.x)*a/b,
    .c = mix_colors(v1.c, v2.c, a, b),
    .uv = mix_vec2(v1.uv, v2.uv, a, b)
  };
}

inline static vertex mix_vertices2(vertex v1, vertex v2, int a, int b)
{
  return (vertex){
    .c = mix_colors(v1.c, v2.c, a, b),
    .uv = mix_vec2(v1.uv, v2.uv, a, b)
  };
}

inline static vertex_int mix_vertices3(vertex v1, vertex v2, int a, int b, int b2)
{
  return (vertex_int){
    .v.x = v1.v.x+(v2.v.x-v1.v.x)*a/b,
    .c = mix_colors2(v1.c, v2.c, a, b, b2),
    .uv = mix_vec22(v1.uv, v2.uv, a, b, b2)
  };
}

inline static vertex mix_vertices4(vertex_int v1, vertex_int v2, int a, int b, int b2)
{
  return (vertex){
    .c = mix_colors3(v1.c, v2.c, a, b, b2),
    .uv = mix_vec23(v1.uv, v2.uv, a, b, b2)
  };
}

#define SWAP(x, y) do {typeof(x) SWAP = x; x = y; y = SWAP;} while(0)
inline static void sort_tri(vertex *v)
{
  if (v[0].v.y > v[1].v.y)
    SWAP(v[0], v[1]);

  if (v[0].v.y > v[2].v.y)
    SWAP(v[0], v[2]);

  if (v[1].v.y > v[2].v.y)
    SWAP(v[1], v[2]);
}

inline static void draw_triangle_half(int which_half, int x1, int x2, int y1, int y2, vertex v[static 3], void (*set_pixel)(vertex v))
{
  int d[] = {
    v[1].v.y - v[0].v.y,
    v[2].v.y - v[0].v.y,
    v[2].v.y - v[1].v.y
  };
  int top = v[0+which_half].v.y;
  int bottom = v[1+which_half].v.y;
  if (top < y1) top = y1;
  if (bottom > y2) bottom = y2;

  for (int i = top; i < bottom; i++)
  {
    // Wmaybe-uninitialized complaining :(
    vertex_int v1 = {};
    vertex_int v2 = {};
    int b = 0;
    if (which_half == 0) {
      v1 = mix_vertices3(v[0], v[1], i-v[0].v.y, d[0], d[1]);
      v2 = mix_vertices3(v[0], v[2], i-v[0].v.y, d[1], d[0]);
      b = d[0]*d[1];
    } else {
      v1 = mix_vertices3(v[1], v[2], i-v[1].v.y, d[2], d[1]);
      v2 = mix_vertices3(v[0], v[2], i-v[1].v.y+d[0], d[1], d[2]);
      b = d[2]*d[1];
    }
    v1.v.y = i;
    v2.v.y = i;

    if (v1.v.x > v2.v.x)
      SWAP(v1, v2);

    int left = x1;
    int right = x2;
    if (left <  v1.v.x) left = v1.v.x;
    if (right >= v2.v.x) right = v2.v.x;
    
    for (int j = left; j < right; j++)
    {
      vertex v = mix_vertices4(v1, v2, j-v1.v.x, v2.v.x-v1.v.x, b);
      v.v = (vec2){j, i};
      set_pixel(v);
    }
  }
}

void draw_triangle(vertex v[static 3], int x1, int x2, int y1, int y2, void (*set_pixel)(vertex v))
{
  sort_tri(v);
  draw_triangle_half(0, x1, x2, y1, y2, v, set_pixel);
  draw_triangle_half(1, x1, x2, y1, y2, v, set_pixel);
}

void draw_line(vertex v[static 2], int x1, int x2, int y1, int y2, void (*set_pixel)(vertex v))
{

  if (v[0].v.y > v[1].v.y)
  {
    vertex tmp = v[1];
    v[1] = v[0];
    v[0] = tmp;
  }

  int32_t dx = v[1].v.x - v[0].v.x;
  int32_t dy = v[1].v.y - v[0].v.y;

  if (dy > dx)
  {
    for (int i = v[0].v.y; i < v[1].v.y; i++)
    {
      vertex vert = mix_vertices(v[0], v[1], i - v[0].v.y, dy);
      vert.v = (vec2){v[0].v.x+dx*(i - v[0].v.y)/dy, i};
      set_pixel(vert);
    }
  }
  else
  {
    for (int i = v[0].v.x; i < v[1].v.x; i++)
    {
      vertex vert = mix_vertices(v[0], v[1], i - v[0].v.x, dx);
      vert.v = (vec2){i, v[0].v.y+dy*(i - v[0].v.x)/dx};
      set_pixel(vert);
    }
  }

  set_pixel(v[0]);
}