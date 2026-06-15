
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <wasm.h>
#include <stdio.h>

extern void *get_fb(void);
extern void *display_fb(void);
extern void *get_rom(void);
extern void *wasm_allocate(size_t memory_size);

int main(int argc, char *argv[]);

WASM_PUBLIC void wasm_main(void) {
    main(0, NULL);
}

void __assert_fail(const char *expr, const char *file, int line, const char *func) {
	printf("%s:%s:%d assertion failed assert(%s);.\n", func, file, line, expr);
	__asm__(
		"unreachable"
	);
    while(1);
}

void *fb;
uint32_t resx;
uint32_t resy;

void update_vram(uint8_t *mem, uint32_t X, uint32_t Y, uint8_t bit16)
{
  uint32_t *pixels = (uint32_t *)fb;

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

        uint32_t color = (b<<16) | (g<<8) | (r<<0) | (0xFF << 24);

         pixels[y*resx+x] = color;
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
        uint32_t r = ((rgb >>  0) & 0x1F) << 3;
        uint32_t g = ((rgb >>  5) & 0x1F) << 3;
        uint32_t b = ((rgb >> 10) & 0x1F) << 3;

        uint32_t color = (b<<16) | (g<<8) | (r<<0) | (0xFF<<24);

         pixels[y*resx+x] = color;
      }
    }
  }

  display_fb();
}

int handle_input()
{
    return 0;
}

extern uint16_t wasm_get_keys(void);

uint16_t get_input()
{
    uint16_t keys = wasm_get_keys();

	return keys;
}

void wait_frame(void)
{
  
}

void set_resolution(int x, int y)
{
  resx = 512;
  resy = 240;
  /*if (resx != x || resy != y) {
    resx = x;
    resy = y;
  }*/
}

void init_game_window()
{
	fb = get_fb();
}


void *platform_aquire_rom(void)
{
  return get_rom();
}


void *platform_allocate(size_t memory_size)
{
  return wasm_allocate(memory_size);
}

int platform_init(void)
{
  set_resolution(512, 240);
  init_game_window();

  return 0;
}