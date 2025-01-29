#include "main.h"
#include "psx_mem.h"
#include "int_math.h"
#include "decompilation.h"

#include <stdint.h>

uint32_t psx_has_2mb_vram(void)
{
  uint32_t version = lbu(0x80074A64);
  return version == 1 || version == 2;
}

uint32_t spyro_set_drawing_area_top_left_command(int16_t x, int16_t y)
{
  x = clamp_int(x, 0, lh(VRAM_SIZE_X)-1);
  y = clamp_int(y, 0, lh(VRAM_SIZE_Y)-1);
  if (psx_has_2mb_vram()) {
    v1 = (y & 0xFFF) << 12;
    v0 = x & 0xFFF;
  } else {
    v1 = (y & 0x3FF) << 10;
    v0 = x & 0x3FF;
  }
  return 0xE3000000 | v0 | v1;
}

// size: 0x000000CC
void function_80060BC8(void)
{
  BREAKPOINT;
  v0 = spyro_set_drawing_area_top_left_command(a0, a1);
}

uint32_t spyro_set_drawing_area_bottom_right_command(int16_t x, int16_t y)
{
  x = clamp_int(x, 0, lh(VRAM_SIZE_X)-1);
  y = clamp_int(y, 0, lh(VRAM_SIZE_Y)-1);
  if (psx_has_2mb_vram()) {
    v1 = (y & 0xFFF) << 12;
    v0 = x & 0xFFF;
  } else {
    v1 = (y & 0x3FF) << 10;
    v0 = x & 0x3FF;
  }
  return 0xE4000000 | v1 | v0;
}

// size: 0x000000CC
void function_80060C94(void)
{
  BREAKPOINT;
  v0 = spyro_set_drawing_area_bottom_right_command(a0, a1);
}

uint32_t spyro_set_drawing_offset_command(uint32_t x, uint32_t y)
{
  if (psx_has_2mb_vram()) {
    v1 = (y & 0xFFF) << 12;
    v0 = x & 0xFFF;
  } else {
    v1 = (y & 0x7FF) << 11;
    v0 = x & 0x7FF;
  }
  return 0xE5000000 | v1 | v0;
}

// size: 0x00000044
void function_80060D60(void)
{
  BREAKPOINT;
  v0 = spyro_set_drawing_offset_command(a0, a1);
}

uint32_t spyro_draw_mode_setting_command(uint32_t allow_drawing_to_display, uint32_t dithering_enabled, uint32_t tex_page)
{
  if (psx_has_2mb_vram()) {
    v0 = tex_page & 0x27FF;
    if (allow_drawing_to_display)
      v0 = v0 | 0x1000;

    if (dithering_enabled)
      v0 = v0 | 0x800;
  } else {
    v0 = tex_page & 0x9FF;
    if (allow_drawing_to_display)
      v0 = v0 | 0x400;

    if (dithering_enabled)
      v0 = v0 | 0x200;
  }
  return 0xE1000000 | v0;
}

// size: 0x00000058
// get gpu info, and calculate a draw mode setting command
void function_80060B70(void)
{
  BREAKPOINT;
  v0 = spyro_draw_mode_setting_command(a0, a1, a2);
}

uint32_t spyro_set_texture_window_setting_command(uint32_t texture_window)
{
  if (texture_window == 0)
    return 0;
  
  a1 = lbu(texture_window + 0) >> 3;
  a2 = ((-lh(texture_window + 4)) >> 3) & 0x1F;
  v0 = lbu(texture_window + 2) >> 3;
  v1 = ((-lh(texture_window + 6)) >> 3) & 0x1F;
  return 0xE2000000 | (v0 << 15) | (a1 << 10) | (v1 << 5) | (a2 << 0);
}

// size: 0x00000084
void function_80060DA4(void)
{
  BREAKPOINT;
  v0 = spyro_set_texture_window_setting_command(a0);
}

// size: 0x00000290
void spyro_clear_screen(uint32_t cmd_ptr, uint32_t screen_info)
{
  int16_t x = lh(screen_info + 0x00);
  int16_t y = lh(screen_info + 0x02);
  int16_t width = lh(screen_info + 0x04);
  int16_t height = lh(screen_info + 0x06);
  int16_t off_x = lh(screen_info + 0x08);
  int16_t off_y = lh(screen_info + 0x0A);
  uint32_t allow_drawing_to_display = lbu(screen_info + 0x17);
  uint32_t dithering_enabled = lbu(screen_info + 0x16);
  uint32_t tex_page = lhu(screen_info + 0x14);
  
  sw(cmd_ptr + 0x04, spyro_set_drawing_area_top_left_command(x, y));
  sw(cmd_ptr + 0x08, spyro_set_drawing_area_bottom_right_command(x + width - 1, y + height - 1));
  sw(cmd_ptr + 0x0C, spyro_set_drawing_offset_command(off_x, off_y));
  sw(cmd_ptr + 0x10, spyro_draw_mode_setting_command(allow_drawing_to_display, dithering_enabled, tex_page));
  sw(cmd_ptr + 0x14, spyro_set_texture_window_setting_command(screen_info + 0x0C));
  sw(cmd_ptr + 0x18, 0xE6000000);
  t0 = 7;
  if (lbu(screen_info + 0x18)) {
    if ((x & 0x3F) || (width & 0x3F)) {
      sw(cmd_ptr + 4*t0++, 0x60000000
        | (lbu(screen_info + 0x1B) << 16)
        | (lbu(screen_info + 0x1A) <<  8)
        | (lbu(screen_info + 0x19) <<  0));
      sw(cmd_ptr + 4*t0++, (x - off_x) | ((y - off_y) << 16));
      sw(cmd_ptr + 4*t0++, width | (height << 16));
    } else {
      sw(cmd_ptr + 4*t0++, 0x02000000
        | (lbu(screen_info + 0x1B) << 16)
        | (lbu(screen_info + 0x1A) <<  8)
        | (lbu(screen_info + 0x19) <<  0));
      sw(cmd_ptr + 4*t0++, x | (y << 16));
      sw(cmd_ptr + 4*t0++, width | (height << 16));
    }
  }
  sb(cmd_ptr + 0x03, t0 - 1);
}

void function_800608E0(void)
{
  BREAKPOINT;
  spyro_clear_screen(a0, a1);
}