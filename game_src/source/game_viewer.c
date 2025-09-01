
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

#define SDL_DISABLE_IMMINTRIN_H
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>

#include "debug.h"

#define WIDTH (512)
#define HEIGHT (240)
#define SCALE 2

int resx = WIDTH;
int resy = HEIGHT;

int create_window(SDL_Window** window, SDL_Surface **screen_surface, int x, int y)
{
  //Create window
  *window = SDL_CreateWindow("spyro", WIDTH*SCALE, HEIGHT*SCALE, SDL_WINDOW_OPENGL);
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

void update_vram(uint8_t *mem, uint32_t X, uint32_t Y, uint8_t bit16)
{
  SDL_Window *window = g_window;
  SDL_Surface *screen_surface = g_screen_surface;

  SDL_LockSurface(screen_surface);

  uint32_t *pixels = (uint32_t *)screen_surface->pixels;

  uint32_t sx = screen_surface->w/resx;
  uint32_t sy = screen_surface->h/resy;

  uint32_t scale = sx < sy ? sx : sy;

  if (bit16)
  {
    uint8_t *ptr = (uint8_t *)mem;
    for (int y = 0; y < resy; y++)
    {
      for (int x = 0; x < resx; x++)
      {
        uint32_t i = (y+Y)*2048+(x+X)*3;
        uint32_t r = ptr[i+0];
        uint32_t g = ptr[i+1];
        uint32_t b = ptr[i+2];

        uint32_t color = (b<<0) | (g<<8) | (r<<16);

        for (int x2 = 0; x2 < scale; x2++)
        for (int y2 = 0; y2 < scale; y2++)
        {
          int x3 = x*scale+x2;
          int y3 = y*scale+y2;
          pixels[y3*screen_surface->pitch/4+x3] = color;
        }
      }
    }
  }
  else
  {
    uint16_t *ptr = (uint16_t *)mem;
    for (int y = 0; y < resy; y++)
    {
      for (int x = 0; x < resx; x++)
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
  }

  SDL_UnlockSurface(screen_surface);

  SDL_UpdateWindowSurface(window);
}

uint16_t keys = 0xFFFF;

uint32_t get_key_mask(SDL_Keycode key)
{
  if (key == SDLK_RETURN) // 0x0800
    return 1 << 3;

  if (key == SDLK_W) // 0x1000
    return 1 << 4;

  if (key == SDLK_D) // 0x2000
    return 1 << 5;

  if (key == SDLK_S) // 0x4000
    return 1 << 6;

  if (key == SDLK_A) // 0x8000
    return 1 << 7;



  if (key == SDLK_Y) // 0x0001
    return 1 << 8;

  if (key == SDLK_U) // 0x0002
    return 1 << 9;

  if (key == SDLK_H) // 0x0004
    return 1 << 10;

  if (key == SDLK_J) // 0x0008
    return 1 << 11;



  if (key == SDLK_Q) // 0x0010
    return 1 << 12;

  if (key == SDLK_E) // 0x0020
    return 1 << 13;

  if (key == SDLK_SPACE) // 0x0040
    return 1 << 14;

  if (key == SDLK_LSHIFT) // 0x0080
    return 1 << 15;

  return 0;
}

uint32_t fast_forward = 0;

int handle_input()
{
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_EVENT_QUIT:
      return 1;
      break;
    case SDL_EVENT_KEY_DOWN:
      if (e.key.key == SDLK_T) fast_forward = 1;
      if (e.key.key == SDLK_P) assert(0);
      keys &= ~get_key_mask(e.key.key);
      break;
    case SDL_EVENT_KEY_UP:
      if (e.key.key == SDLK_T) fast_forward = 0;
      keys |= get_key_mask(e.key.key);
      break;
    }
  }
  return 0;
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

void set_resolution(int x, int y)
{
  if (resx != x || resy != y) {
    resx = x;
    resy = y;
  }
}

void init_game_window()
{
  SDL_Window *window = NULL;
  SDL_Surface *screen_surface = NULL;


  //struct sigaction action;
  //sigaction(SIGINT, NULL, &action);
  assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS));
  //sigaction(SIGINT, &action, NULL);

  create_window(&window, &screen_surface, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED);

  if (window == NULL) BREAKPOINT;
  if (screen_surface == NULL) BREAKPOINT;

  g_window = window;
  g_screen_surface = screen_surface;
}
