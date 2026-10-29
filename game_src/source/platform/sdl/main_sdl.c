
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#include "../../../../config.h"

#define SDL_DISABLE_IMMINTRIN_H
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>

#define WIDTH (512)
#define HEIGHT (240)
#define SCALE 2

int resx = WIDTH;
int resy = HEIGHT;

uint32_t g_debug_line_num;
SDL_Window* g_debug_window;
SDL_Renderer *g_debug_renderer;

SDL_Window* g_window;
SDL_Surface *g_screen_surface;

void update_vram(uint8_t *mem, uint32_t X, uint32_t Y, uint8_t bit16)
{
  SDL_Window *window = g_window;
  SDL_Surface *screen_surface = g_screen_surface;

  SDL_LockSurface(screen_surface);

  uint32_t *pixels = (uint32_t *)screen_surface->pixels;

  int sx = screen_surface->w/resx;
  int sy = screen_surface->h/resy;

  int scale = sx < sy ? sx : sy;

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

void *platform_aquire_rom(void)
{
  FILE *file = fopen(ROM_NAME, "r");
  assert(file != NULL);

  assert(fseek(file, 0, SEEK_END) != -1);
  int filesize = ftell(file);
  assert(filesize != -1);
  assert(fseek(file, 0, SEEK_SET) != -1);

  uint8_t *ptr = malloc(filesize);
  int things_read = fread(ptr, filesize, 1, file);
  assert(things_read == 1);
  assert(fclose(file) == 0);

  return ptr;
}

void *platform_allocate(size_t memory_size)
{
  return malloc(memory_size);
}

int create_window(SDL_Window** window, SDL_Surface **screen_surface, const char *name)
{
  *window = SDL_CreateWindow(name, WIDTH*SCALE, HEIGHT*SCALE, SDL_WINDOW_OPENGL);
  if (*window == NULL)
  {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }

  *screen_surface = SDL_GetWindowSurface(*window);

  if (window == NULL) assert(0);
  if (screen_surface == NULL) assert(0);

  return 0;
}

void init_game_window(void)
{
  create_window(&g_window, &g_screen_surface, "spyro");
}

void init_debug_window(void)
{
  assert(SDL_CreateWindowAndRenderer("examples/renderer/debug-text", 1024, 512, SDL_WINDOW_RESIZABLE, &g_debug_window, &g_debug_renderer));
  SDL_SetRenderLogicalPresentation(g_debug_renderer, 1024, 512, SDL_LOGICAL_PRESENTATION_LETTERBOX);
  SDL_SetRenderScale(g_debug_renderer, 2.0f, 2.0f);
}

int platform_init(void)
{
  int err = chdir("..");
  if (err) return err;

  assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS));

  init_debug_window();

  init_game_window();

  return 0;
}

void debug_begin()
{
  g_debug_line_num = 0;

  SDL_SetRenderDrawColor(g_debug_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

  SDL_RenderClear(g_debug_renderer);

  SDL_SetRenderDrawColor(g_debug_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
}

void debug_text(const char *text)
{
  SDL_RenderDebugText(g_debug_renderer, 2, 2 + g_debug_line_num*10, text);
  g_debug_line_num++;
}

void debug_end()
{
  SDL_RenderPresent(g_debug_renderer);
}