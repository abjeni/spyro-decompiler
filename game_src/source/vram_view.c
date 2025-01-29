#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#define SDL_DISABLE_IMMINTRIN_H
#include <SDL2/SDL.h>
#include <assert.h>
#include "debug.h"

#define WIDTH (512)
#define HEIGHT (240)
#define SCALE 4

int create_window(SDL_Window** window, SDL_Surface **screen_surface, int x, int y)
{
  //Create window
  *window = SDL_CreateWindow("spyro", x, y, WIDTH*SCALE, HEIGHT*SCALE, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
  if (*window == NULL)
  {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }

  *screen_surface = SDL_GetWindowSurface(*window);

  return 0;
}

SDL_Window* g_window;
SDL_Surface *g_screen_surface;

void update_vram(uint8_t *mem, uint32_t X, uint32_t Y)
{
  SDL_Window *window = g_window;
  SDL_Surface *screen_surface = g_screen_surface;

  SDL_LockSurface(screen_surface);

  uint16_t *ptr = (uint16_t *)mem;  
  uint32_t *pixels = (uint32_t *)screen_surface->pixels;

  uint32_t sx = screen_surface->w/WIDTH;
  uint32_t sy = screen_surface->h/HEIGHT;

  uint32_t scale = sx < sy ? sx : sy;

  for (int y = 0; y < HEIGHT; y++)
  {
    for (int x = 0; x < WIDTH; x++)
    {
      uint32_t i = (y+Y)*1024+x+X;
      uint32_t rgb = ptr[i];
      uint32_t r = (rgb >>  0) & 0x1F;
      uint32_t g = (rgb >>  5) & 0x1F;
      uint32_t b = (rgb >> 10) & 0x1F;

      uint32_t color = (b<<3) | (g<<11) | (r<<19);

      for (int x2 = 0; x2 < scale; x2++)
      for (int y2 = 0; y2 < scale; y2++)
      {
        int x3 = x*scale+x2;
        int y3 = y*scale+y2;
        pixels[y3*screen_surface->pitch/4+x3] = color;
      }
    }
  }

  SDL_UnlockSurface(screen_surface);

  SDL_UpdateWindowSurface(window);
}

uint16_t keys = 0xFFFF;

uint32_t get_key_mask(SDL_Keycode key)
{
  /*if (key == SDLK_g)
    return 1 << 0;
  if (key == SDLK_h)
    return 1 << 1;
  if (key == SDLK_b)
    return 1 << 2;
  if (key == SDLK_n)
    return 1 << 3;
  if (key == SDLK_q)
    return 1 << 4;
  if (key == SDLK_e)
    return 1 << 5;
  if (key == SDLK_SPACE)
    return 1 << 6;
  if (key == SDLK_LSHIFT)
    return 1 << 7;
  if (key == SDLK_z)
    return 1 << 8;
  if (key == SDLK_x)
    return 1 << 9;
  if (key == SDLK_c)
    return 1 << 10;
  if (key == SDLK_RETURN)
    return 1 << 11;
  if (key == SDLK_w)
    return 1 << 12;
  if (key == SDLK_a)
    return 1 << 13;
  if (key == SDLK_s)
    return 1 << 14;
  if (key == SDLK_d)
    return 1 << 15;*/

  if (key == SDLK_RETURN)
    return 1 << 3;

  if (key == SDLK_w)
    return 1 << 4;

  if (key == SDLK_d)
    return 1 << 5;

  if (key == SDLK_s)
    return 1 << 6;

  if (key == SDLK_a)
    return 1 << 7;



  if (key == SDLK_y)
    return 1 << 8;

  if (key == SDLK_u)
    return 1 << 9;

  if (key == SDLK_h)
    return 1 << 10;

  if (key == SDLK_j)
    return 1 << 11;



  if (key == SDLK_q)
    return 1 << 12;

  if (key == SDLK_e)
    return 1 << 13;

  if (key == SDLK_SPACE)
    return 1 << 14;

  if (key == SDLK_LSHIFT)
    return 1 << 15;

  return 0;
}

uint32_t fast_forward = 0;

void handle_input()
{
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT:
      exit(0);
      break;
    case SDL_KEYDOWN:
      if (e.key.keysym.sym == SDLK_t) fast_forward = 1;
      if (e.key.keysym.sym == SDLK_p) exit(0);
      keys &= ~get_key_mask(e.key.keysym.sym);
      break;
    case SDL_KEYUP:
      if (e.key.keysym.sym == SDLK_t) fast_forward = 0;
      keys |= get_key_mask(e.key.keysym.sym);
      break;
    }
  }
}

uint16_t get_input()
{
  return keys;
}

void wait_frame(void)
{
  static uint32_t last_ticks = 0;
  uint32_t current_ticks = SDL_GetTicks();
  uint32_t diff;

  if (!fast_forward) {
    do {
      current_ticks = SDL_GetTicks();
      diff = current_ticks-last_ticks;
    } while (diff < 1000/30);
  }
  last_ticks = current_ticks;
}

void init_vram_view()
{
  SDL_Window *window = NULL;
  SDL_Surface *screen_surface = NULL;


  struct sigaction action;
  sigaction(SIGINT, NULL, &action);
  assert(SDL_Init(SDL_INIT_EVERYTHING) >= 0);
  sigaction(SIGINT, &action, NULL);

  create_window(&window, &screen_surface, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED);

  if (window == NULL) BREAKPOINT;
  if (screen_surface == NULL) BREAKPOINT;

  g_window = window;
  g_screen_surface = screen_surface;
}
