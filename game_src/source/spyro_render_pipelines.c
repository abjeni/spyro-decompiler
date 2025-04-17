#include "decompilation.h"
#include "title_screen.h"
#include "int_math.h"
#include "spyro_string.h"
#include "spyro_print.h"
#include "main.h"
#include "psx_mem.h"
#include "psx_ops.h"
#include "spyro_math.h"
#include "spyro_string.h"
#include "spyro_psy.h"
#include "spyro_game.h"
#include "spyro_vsync.h"
#include "spyro_graphics.h"
#include "function_chooser.h"
#include "spyro_constants.h"
#include "level_loading.h"
#include "triangle.h"

#include <stdint.h>
#include <string.h>

DISP *backbuffer_disp = NULL;

void wait_two_vsyncs()
{
  DrawSync(0);
  if (lw(0x80075784))
    VSync(0);

  sw(current_frame, VSync(-1));
  while ((int32_t)(lw(current_frame) - lw(drawn_frame)) < 2) {
    VSync(0);
    sw(current_frame, VSync(-1));
  }

  sw(drawn_frame, VSync(-1));
}

void text_wave_effect1(struct game_object *game_object, uint32_t offset, uint32_t freq, uint32_t mult, uint32_t div)
{
  struct game_object *game_object_end = addr_to_pointer(lw(gameobject_stack_ptr));

  uint32_t rot = 0;
  while (game_object > game_object_end) {
    game_object--;
    game_object->rotz = cos_lut[(offset + rot) & 0xFF] * mult / div;
    rot += freq;
  }
}

void text_wave_effect2(uint32_t num, uint32_t offset, uint32_t freq, uint32_t mult, uint32_t div)
{
  struct game_object *game_object = addr_to_pointer(lw(gameobject_stack_ptr));

  uint32_t rot = 0;
  for (int i = 0; i < num; i++) {
    game_object->rotz = cos_lut[(offset + rot) & 0xFF] * mult / div;
    rot += freq;
    game_object++;
  }
}

void text_wave_effect3(struct game_object *game_object, uint32_t num, uint32_t offset, uint32_t freq, uint32_t mult, uint32_t div)
{
  uint32_t rot = 0;
  for (int i = 0; i < num; i++) {
    game_object->rotz = cos_lut[(offset + rot) & 0xFF] * mult / div;
    rot += freq;
    game_object++;
  }
}

// size: 0x000000E8
void gui_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  uint32_t cmd_ptr = lw(allocator1_ptr);
  sw(cmd_ptr + 0x00, 0x04000000);
  sb(cmd_ptr + 0x07, 0x50);
  sh(cmd_ptr + 0x08, x1);
  sh(cmd_ptr + 0x0A, y1);
  sh(cmd_ptr + 0x10, x2);
  sh(cmd_ptr + 0x12, y2);
  v0 = 0x80 - spyro_two_angle_diff_8bit(spyro_atan(x1 - 0x100, y1 - 0x78), lw(0x800770F4));
  sb(cmd_ptr + 0x04, v0 + 0x60);
  sb(cmd_ptr + 0x05, v0 + 0x60);
  sb(cmd_ptr + 0x06, v0);
  v0 = 0x80 - spyro_two_angle_diff_8bit(spyro_atan(x2 - 0x100, y2 - 0x78), lw(0x800770F4));
  sb(cmd_ptr + 0x0C, v0 + 0x60);
  sb(cmd_ptr + 0x0D, v0 + 0x60);
  sb(cmd_ptr + 0x0E, v0);
  append_gpu_command_block(addr_to_pointer(cmd_ptr));
  cmd_ptr += 0x14;
  sw(allocator1_ptr, cmd_ptr);
}

void function_8001844C(void)
{
  gui_line(a0, a1, a2, a3);
}

// size: 0x000000D8
void blinking_arrow(vec3_32 p, uint32_t frame, int32_t direction)
{
  if ((frame & 0xF) < 16) {
    sw(gameobject_stack_ptr, lw(gameobject_stack_ptr) - 88);
    struct game_object *object = addr_to_pointer(lw(gameobject_stack_ptr));
    *object = (struct game_object){0};
    object->modelID = 0x105;
    object->p = p;
    if (direction < 2) {
      object->rotx = 0x40;
      object->rotz = direction*0x80;
    }
    object->unknown47 = 0x7F;
    object->unknown4F = 11;
    object->render_distance = 0xFF;
  }
}

// size: 0x000000D8
void function_80018534(void)
{
  blinking_arrow(*(vec3_32*)addr_to_pointer(a0), a1, a2);
}

// size: 0x0000011C
void gui_box_balloonist(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  uint32_t cmd_ptr = lw(allocator1_ptr);
  SetDrawMode(addr_to_pointer(cmd_ptr), 1, 0, 0x40, NULL);
  append_gpu_command_block(addr_to_pointer(cmd_ptr));

  sw(cmd_ptr + 0x0C, 0x05000000);
  sb(cmd_ptr + 0x13, 0x2A);
  sb(cmd_ptr + 0x10, 0x70);
  sb(cmd_ptr + 0x11, 0x70);
  sb(cmd_ptr + 0x12, 0x70);

  sh(cmd_ptr + 0x14, x);
  sh(cmd_ptr + 0x16, w);

  sh(cmd_ptr + 0x18, y);
  sh(cmd_ptr + 0x1A, w);

  sh(cmd_ptr + 0x1C, x);
  sh(cmd_ptr + 0x1E, h);

  sh(cmd_ptr + 0x20, y);
  sh(cmd_ptr + 0x22, h);

  append_gpu_command_block(addr_to_pointer(cmd_ptr + 0x0C));
  cmd_ptr += 0x24;
  sw(allocator1_ptr, cmd_ptr);

  gui_line(x, w, y, w);
  gui_line(y, w, y, h);
  gui_line(y, h, x, h);
  gui_line(x, h, x, w);
}

// size: 0x0000011C
void function_8001860C(void)
{
  BREAKPOINT;
  gui_box_balloonist(a0, a1, a2, a3);
}

// size: 0x00000150
void function_80018728(void)
{
  char *dragon_name = dragon_names[lw(lw(lw(0x800770C0)) + 0x38)];
  uint32_t len = strlen(dragon_name);
  uint32_t centering = (len-1)*13;
  struct game_object *object = addr_to_pointer(lw(gameobject_stack_ptr));
  create_3d_text2("RESCUED", &(vec3_32){0xB0 - centering, 0xC8, 0x1000}, 20, 2);
  create_3d_text2(dragon_name, &(vec3_32){0x150 - centering, 0xC8, 0xC00}, 26, 2);
  text_wave_effect1(object, lw(0x80077080)*2, 12, 1, 128);
}

// size: 0x00000088
void function_80018880(void)
{
  a0 = 0x8006FCF4 + 0x2400;

  while (lw(a0))
    a0 += 4;

  a1 = lw(gameobject_stack_ptr);

  while (a1 < lw(0x800756FC)) {
    sw(a0, a1);
    a0 += 4;
    a1 += 88;
  }
  sw(gameobject_stack_ptr, a1);
  sw(a0, 0);
}

// size: 0x000000E8
void function_80018908(void)
{
  vec3_32 pos = {0xC7, 0xC8, 0x1100};
  vec3_32 size = {0x10, 1, 0x1400};
  
  struct game_object *object = addr_to_pointer(lw(gameobject_stack_ptr));
  create_3d_text1("DEMO MODE", &pos, size, 18, 2);
  text_wave_effect1(object, lw(0x800758C8)*4, 12, 1, 128);
  function_80018880();
}

// size: 0x00000540
void function_800189F0(void)
{
  uint32_t temp;
  sp -= 0xF0;
  sw(sp + 0xEC, ra);
  sw(sp + 0xE8, fp);
  sw(sp + 0xE4, s7);
  sw(sp + 0xE0, s6);
  sw(sp + 0xDC, s5);
  sw(sp + 0xD8, s4);
  sw(sp + 0xD4, s3);
  sw(sp + 0xD0, s2);
  sw(sp + 0xCC, s1);
  sw(sp + 0xC8, s0);

  sw(sp + 0x28, 0);
  sw(sp + 0x70, 0);

  v0 = lw(0x80075684);
  temp = (int32_t)v0 <= 0;
  if (temp) goto label80018EF4;
  sw(sp + 0xB0, 0);
  t1 = 0x800772C8;
  sw(sp + 0xB8, t1);
label80018A40:
  t1 = lw(0x80078658 + lw(sp + 0xB0));
  s6 = 0;
  sw(sp + 0x78, t1);
  if ((int32_t)lw(lw(sp + 0xB8)) <= 0) goto label80018A9C;
  s0 = lw(sp + 0x78);
  s1 = 0x800772C8 + lw(sp + 0xB0);
label80018A78:
  spyro_world_to_screen_projection_with_right_shift(s0 + 0x0C, s0, 1);
  s6++;
  s0 += 0x1C;
  if ((int32_t)s6 < (int32_t)lw(s1)) goto label80018A78;
label80018A9C:
  t1 = lw(sp + 0xB8);
  v1 = 0x800772C8;
  v0 = lw(t1)-1;
  s6 = 0;
  if ((int32_t)v0 <= 0) goto label80018EC0;
  s5 = lw(sp + 0x78);
  sw(sp + 0xA0, 0);
  sw(sp + 0xA8, 0);
label80018AC8:
  v0 = lw(0x800772C8 + lw(sp + 0xB0))-2;
  s0 = 0x00011170;
  v0 = s6 ^ v0;
  v0 = 0 < v0;
  v0 = -v0;
  s0 &= v0;
  if ((int32_t)s6 < 3) {
    t1 = lw(sp + 0xA8);
    s0 = t1 << 5;
  }
  if (lw(LEVEL_ID) == LIB_GNASTY_GNORC)
    s0 = (int32_t)s0 >> 2;
  v0 = lw(s5 + 0x18);
  v0 = (int32_t)v0 < 81;
  temp = v0 == 0;
  a0 = sp + 0x18;
  if (temp) goto label80018E78;
  t2 = lw(sp + 0xA0);
  t1 = lw(sp + 0x78);
  spyro_vec3_sub(sp + 0x18, s5 + 0x0C, t2 + t1 + 0x28);
  v1 = lw(sp + 0x1C);
  v0 = -lw(sp + 0x18);
  sw(sp + 0x70, lw(sp + 0x70)+1);
  sw(sp + 0x18, v1);
  sw(sp + 0x1C, v0);
  a0 = spyro_sqrt(v1*v1 + v0*v0);
  temp = a0 == 0;
  if (temp) goto label80018BAC;
  a1 = lw(s5 + 0x14);
  temp = a1 != 0;
  if (temp) goto label80018BB8;
label80018BAC:
  sw(sp + 0x18, 0);
  sw(sp + 0x1C, 0);
  goto label80018C6C;
label80018BB8:
  v0 = lw(sp + 0x18);
  mult(v0, s0);
  v1=lo;
  mult(a1, a0);
  v0=lo;
  div_psx(v1,v0);
  v1=lo;
  v0 = lw(sp + 0x1C);
  mult(v0, s0);
  a1=lo;
  sw(sp + 0x18, v1);
  v0 = lw(s5 + 0x14);
  mult(v0, a0);
  v0=lo;
  div_psx(a1,v0);
  v0=lo;
  sw(sp + 0x1C, v0);
label80018C6C:
  a1 = lw(s5 + 0x0C);
  v1 = lw(sp + 0x18);
  v0 = lw(sp + 0x1C);
  t1 = a1 + v1;
  sw(sp + 0x30, t1);
  a0 = lw(s5 + 0x10);
  t1 = lw(sp + 0x70);
  a1 -= v1;
  sw(sp + 0x38, a1);
  t2 = a0 + v0;
  a0 -= v0;
  sw(sp + 0x40, t2);
  temp = v0 != 0;
  sw(sp + 0x48, a0);
  t0 = (int32_t)lw(s5 + 0x14) >> 7;
  if ((int32_t)t1 >= 2 && t0 < 2000) {
    v1 = lw(s5 + 0x18);
    color_int col1 = {
      0x96 - v1*2,
      0x96 - v1*4,
      0x96 - v1*10
    };
    
    v1 = lw(s5 - 0x04);
    color_int col2 = {
      0x96 - v1*2,
      0x96 - v1*4,
      0x96 - v1*10
    };

    if ((int32_t)col1.r < 0) col1.r = 0;
    if ((int32_t)col1.g < 0) col1.g = 0;
    if ((int32_t)col1.b < 0) col1.b = 0;
    if ((int32_t)col2.r < 0) col2.r = 0;
    if ((int32_t)col2.g < 0) col2.g = 0;
    if ((int32_t)col2.b < 0) col2.b = 0;

    if (lw(LEVEL_ID) == LIB_GNASTY_GNORC) {
      SWAP(col1.r, col1.g);
      SWAP(col2.r, col2.g);
    }

    s0 = lw(allocator1_ptr);
    sw(sp + 0x10, 0);
    sw(sp + 0xC0, t0);
    SetDrawMode(addr_to_pointer(s0), 1, 0, 0x20, addr_to_pointer(lw(sp + 0x10)));
    append_gpu_command_block_depth_slot(addr_to_pointer(s0), lw(sp + 0xC0));

    sw(s0 + 0x0C, 0x08000000);

    sb(s0 + 0x10, col1.r);
    sb(s0 + 0x11, col1.g);
    sb(s0 + 0x12, col1.b);

    sb(s0 + 0x13, 0x3A);

    sh(s0 + 0x14, lhu(sp + 0x30));
    sh(s0 + 0x16, lhu(sp + 0x40));

    sb(s0 + 0x18, col1.r);
    sb(s0 + 0x19, col1.g);
    sb(s0 + 0x1A, col1.b);

    sh(s0 + 0x1C, lhu(sp + 0x38));
    sh(s0 + 0x1E, lhu(sp + 0x48));

    sb(s0 + 0x20, col2.r);
    sb(s0 + 0x21, col2.g);
    sb(s0 + 0x22, col2.b);

    sh(s0 + 0x24, lhu(sp + 0x50));
    sh(s0 + 0x26, lhu(sp + 0x60));

    sb(s0 + 0x28, col2.r);
    sb(s0 + 0x29, col2.g);
    sb(s0 + 0x2A, col2.b);

    sh(s0 + 0x2C, lhu(sp + 0x58));
    sh(s0 + 0x2E, lhu(sp + 0x68));

    append_gpu_command_block_depth_slot(addr_to_pointer(s0 + 0x0C), lw(sp + 0xC0));
    s0 += 0x30;
    sw(allocator1_ptr, s0);
  }
  sw(sp + 0x50, lw(sp + 0x30));
  sw(sp + 0x58, lw(sp + 0x38));
  sw(sp + 0x60, lw(sp + 0x40));
  sw(sp + 0x68, lw(sp + 0x48));
label80018E78:
  s6++;
  t1 = lw(sp + 0xA0);
  t2 = lw(sp + 0xA8);
  t1 += 28; // 0x001C
  sw(sp + 0xA0, t1);
  t1 = lw(sp + 0x28);
  t2 += 625; // 0x0271
  sw(sp + 0xA8, t2);
  v0 = t1 << 2;
  v0 = lw(0x800772C8 + v0)-1;
  v1 = 0x800772C8;
  v0--;
  s5 += 0x1C;
  if ((int32_t)s6 < (int32_t)v0) goto label80018AC8;
label80018EC0:
  t2 = lw(sp + 0xB0);
  t1 = lw(sp + 0xB8);
  v0 = lw(0x80075684);
  t2 += 4;
  sw(sp + 0xB0, t2);
  t2 = lw(sp + 0x28);
  t1 += 4;
  sw(sp + 0xB8, t1);
  t2++;
  v0 = (int32_t)t2 < (int32_t)v0;
  temp = v0 != 0;
  sw(sp + 0x28, t2);
  if (temp) goto label80018A40;
label80018EF4:
  sw(0x80075684, 0);
  ra = lw(sp + 0xEC);
  fp = lw(sp + 0xE8);
  s7 = lw(sp + 0xE4);
  s6 = lw(sp + 0xE0);
  s5 = lw(sp + 0xDC);
  s4 = lw(sp + 0xD8);
  s3 = lw(sp + 0xD4);
  s2 = lw(sp + 0xD0);
  s1 = lw(sp + 0xCC);
  s0 = lw(sp + 0xC8);
  sp += 0xF0;
}

// size: 0x000001A4
void function_80018F30(void)
{
  uint32_t temp;
  v0 = lw(0x8007570C); // &0x00000000
  sp -= 40; // 0xFFFFFFD8
  sw(sp + 0x20, ra);
  sw(sp + 0x1C, s3);
  sw(sp + 0x18, s2);
  sw(sp + 0x14, s1);
  temp = v0 == 0;
  sw(sp + 0x10, s0);
  if (temp) goto label80018FB0;
  v1 = lw(0x800756C0); // &0x00000000
  v0 = (int32_t)v1 < 22;
  temp = v0 == 0;
  if (temp) goto label80018F84;
  v0 = lw(0x800756CC); // &0x00000000
  v0 += v1;
  sw(0x800756C0, v0); // &0x00000000
label80018F84:
  v0 = lw(0x800756C0); // &0x00000000
  v0 = (int32_t)v0 < 23;
  temp = v0 != 0;
  s3 = 0x05000000;
  if (temp) goto label80018FF8;
  v0 = 22; // 0x0016
  sw(0x800756C0, v0); // &0x00000000
  s2 = 40; // 0x0028
  goto label80018FFC;
label80018FB0:
  v1 = lw(0x800756C0); // &0x00000000
  temp = (int32_t)v1 <= 0;
  if (temp) goto label80018FDC;
  v0 = lw(0x800756CC); // &0x00000000
  v0 = v1 - v0;
  sw(0x800756C0, v0); // &0x00000000
label80018FDC:
  v0 = lw(0x800756C0); // &0x00000000
  temp = (int32_t)v0 >= 0;
  s3 = 0x05000000;
  if (temp) goto label80018FF8;
  sw(0x800756C0, 0); // &0x00000000
label80018FF8:
  s2 = 40; // 0x0028
label80018FFC:
  s0 = lw(allocator1_ptr);
  s1 = 512; // 0x0200
  sb(s0 + 0x07, s2);
  v0 = lw(0x800756C0); // &0x00000000
  a0 = s0;
  sw(s0 + 0x00, s3);
  sh(s0 + 0x08, 0);
  sh(s0 + 0x0C, s1);
  sh(s0 + 0x10, 0);
  sh(s0 + 0x14, s1);
  sh(s0 + 0x0A, 0);
  sh(s0 + 0x0E, 0);
  sb(s0 + 0x04, 0);
  sb(s0 + 0x05, 0);
  sb(s0 + 0x06, 0);
  sh(s0 + 0x12, v0);
  sh(s0 + 0x16, v0);
  ra = 0x8001904C;
  append_gpu_command_block(addr_to_pointer(a0));
  s0 += 24; // 0x0018
  a0 = s0;
  sw(allocator1_ptr, s0);
  sb(s0 + 0x07, s2);
  v1 = lw(0x800756C0); // &0x00000000
  v0 = 240; // 0x00F0
  sh(s0 + 0x12, v0);
  sh(s0 + 0x16, v0);
  v0 = 240; // 0x00F0
  sw(s0 + 0x00, s3);
  sh(s0 + 0x08, 0);
  sh(s0 + 0x0C, s1);
  sh(s0 + 0x10, 0);
  sh(s0 + 0x14, s1);
  sb(s0 + 0x04, 0);
  sb(s0 + 0x05, 0);
  sb(s0 + 0x06, 0);
  v0 -= v1;
  sh(s0 + 0x0A, v0);
  sh(s0 + 0x0E, v0);
  ra = 0x800190A8;
  append_gpu_command_block(addr_to_pointer(a0));
  s0 += 24; // 0x0018
  sw(allocator1_ptr, s0);
  ra = lw(sp + 0x20);
  s3 = lw(sp + 0x1C);
  s2 = lw(sp + 0x18);
  s1 = lw(sp + 0x14);
  s0 = lw(sp + 0x10);
  sp += 40; // 0x0028
}

// size: 0x000000C8
void function_800190D4(void)
{
  sp -= 48; // 0xFFFFFFD0
  sw(sp + 0x1C, s1);
  s1 = a1;
  sw(sp + 0x20, s2);
  s2 = a2;
  sw(sp + 0x24, s3);
  s3 = a3;
  a3 = a0 << 5;
  a1 = 1; // 0x0001
  sw(sp + 0x18, s0);
  s0 = lw(allocator1_ptr);
  a2 = 0;
  sw(sp + 0x28, ra);
  sw(sp + 0x10, 0);
  a0 = s0;
  SetDrawMode(addr_to_pointer(s0), a1, a2, a3, addr_to_pointer(lw(sp + 0x10)));
  append_gpu_command_block(addr_to_pointer(s0));
  a0 = s0 + 12; // 0x000C
  v0 = 0x05000000;
  sw(s0 + 0x0C, v0);
  v0 = 42; // 0x002A
  sb(s0 + 0x13, v0);
  v0 = 8; // 0x0008
  v1 = 512; // 0x0200
  sh(s0 + 0x16, v0);
  sh(s0 + 0x1A, v0);
  v0 = 232; // 0x00E8
  sh(s0 + 0x14, 0);
  sh(s0 + 0x18, v1);
  sh(s0 + 0x1C, 0);
  sh(s0 + 0x1E, v0);
  sh(s0 + 0x20, v1);
  sh(s0 + 0x22, v0);
  sb(s0 + 0x10, s1);
  sb(s0 + 0x11, s2);
  sb(s0 + 0x12, s3);
  ra = 0x80019170;
  append_gpu_command_block(addr_to_pointer(a0));
  s0 += 36; // 0x0024
  sw(allocator1_ptr, s0);
  ra = lw(sp + 0x28);
  s3 = lw(sp + 0x24);
  s2 = lw(sp + 0x20);
  s1 = lw(sp + 0x1C);
  s0 = lw(sp + 0x18);
  sp += 48; // 0x0030
}

// size: 0x00000164
void function_8001919C(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x10, s0);
  s0 = lw(allocator1_ptr);
  v0 = 0x09000000;
  sw(sp + 0x14, ra);
  sw(s0 + 0x00, v0);
  v0 = 44; // 0x002C
  temp = a2 != 0;
  sb(s0 + 0x07, v0);
  if (temp) goto label800191D4;
  v0 = 128; // 0x0080
  sb(s0 + 0x04, v0);
  sb(s0 + 0x05, v0);
  goto label800191F0;
label800191D4:
  v0 = lw(a2 + 0x00);
  sb(s0 + 0x04, v0);
  v0 = lw(a2 + 0x04);
  sb(s0 + 0x05, v0);
  v0 = lw(a2 + 0x08);
label800191F0:
  sb(s0 + 0x06, v0);
  v0 = lhu(a0 + 0x00);
  sh(s0 + 0x08, v0);
  v0 = lhu(a0 + 0x02);
  sh(s0 + 0x0A, v0);
  v0 = lhu(a0 + 0x00);
  v1 = lhu(a0 + 0x04);
  v0 += v1;
  sh(s0 + 0x10, v0);
  v0 = lhu(a0 + 0x02);
  sh(s0 + 0x12, v0);
  v0 = lhu(a0 + 0x00);
  sh(s0 + 0x18, v0);
  v0 = lhu(a0 + 0x02);
  v1 = lhu(a0 + 0x06);
  v0 += v1;
  sh(s0 + 0x1A, v0);
  v0 = lhu(a0 + 0x00);
  v1 = lhu(a0 + 0x04);
  v0 += v1;
  sh(s0 + 0x20, v0);
  v0 = lhu(a0 + 0x02);
  v1 = lhu(a0 + 0x06);
  v0 += v1;
  sh(s0 + 0x22, v0);
  v0 = lw(a1 + 0x00);
  sw(s0 + 0x0C, v0);
  v0 = lw(a1 + 0x04);
  sw(s0 + 0x14, v0);
  v0 = lbu(s0 + 0x0C);
  v1 = lbu(a0 + 0x04);
  v0 += v1;
  sb(s0 + 0x14, v0);
  v0 = lbu(s0 + 0x0C);
  v1 = lbu(s0 + 0x14);
  sb(s0 + 0x1C, v0);
  v0 = lbu(s0 + 0x0D);
  sb(s0 + 0x24, v1);
  v1 = lbu(a0 + 0x06);
  v0 += v1;
  sb(s0 + 0x1D, v0);
  v0 = lbu(s0 + 0x0D);
  v1 = lbu(a0 + 0x06);
  a0 = s0;
  v0 += v1;
  sb(s0 + 0x25, v0);
  ra = 0x800192E0;
  append_gpu_command_block(addr_to_pointer(a0));
  v0 = s0 + 40; // 0x0028
  sw(allocator1_ptr, v0);
  ra = lw(sp + 0x14);
  s0 = lw(sp + 0x10);
  sp += 24; // 0x0018
}

// size: 0x00000398
void function_80019300(void)
{
  uint32_t temp;
  v0 = lw(0x80075690); // &0x00000000
  sp -= 88; // 0xFFFFFFA8
  sw(sp + 0x50, ra);
  sw(sp + 0x4C, s3);
  sw(sp + 0x48, s2);
  sw(sp + 0x44, s1);
  temp = v0 != 0;
  sw(sp + 0x40, s0);
  if (temp) goto label80019528;
  s1 = 0x8006FCF4 + 0x2400; // "H^^^oooooofffOOO((("
  v0 = lw(s1 + 0x00);
  temp = v0 == 0;
  if (temp) goto label80019354;
  s1 += 4; // 0x0004
label80019340:
  v0 = lw(s1 + 0x00);
  temp = v0 != 0;
  s1 += 4; // 0x0004
  if (temp) goto label80019340;
  s1 -= 4; // 0xFFFFFFFC
label80019354:
  v1 = 0x80077FA8;
  v0 = lbu(v1 + 0x00);
  temp = v0 == 0;
  v0 = v0 ^ 0x4;
  if (temp) goto label80019484;
  v0 = v0 < 1;
  s0 = v0 << 2;
  v0 = (int32_t)s0 < 5;
  temp = v0 == 0;
  v1 += 68; // 0x0044
  if (temp) goto label800193B0;
  v0 = s0 << 1;
  v0 += s0;
  v0 = v0 << 2;
  v0 -= s0;
  v0 = v0 << 3;
  v1 += v0;
label80019398:
  sw(s1 + 0x00, v1);
  s1 += 4; // 0x0004
  s0++;
  v0 = (int32_t)s0 < 5;
  temp = v0 != 0;
  v1 += 88; // 0x0058
  if (temp) goto label80019398;
label800193B0:
  s2 = 0x80077FA8;
  v1 = lbu(s2 + 0x00);
  v0 = 4; // 0x0004
  temp = v1 != v0;
  if (temp) goto label80019484;
  char buf[0x20];
  sprintf(buf, "%d/%d", lw(0x80077FC8), lw(0x80077FC8));
  struct game_object *object = addr_to_pointer(lw(gameobject_stack_ptr));
  create_3d_text2(buf, &(vec3_32){0x5A, 0x24, 0xB40}, 0x1C, 11);
  text_wave_effect1(object, lw(s2 + 0x0C)*4, 12, 1, 128);
  struct game_object *game_object_end = addr_to_pointer(lw(gameobject_stack_ptr));
  while (object > game_object_end) {
    object--;
    sw(s1, pointer_to_addr(object));
    s1 += 4;
  }

label80019484:
  v1 = 0x80077FA9;
  v0 = lbu(v1 + 0x00);
  temp = v0 == 0;
  v1 += 507; // 0x01FB
  if (temp) goto label800194B8;
  s0 = 0;
label800194A0:
  sw(s1 + 0x00, v1);
  s1 += 4; // 0x0004
  s0++;
  v0 = (int32_t)s0 < 3;
  temp = v0 != 0;
  v1 += 88; // 0x0058
  if (temp) goto label800194A0;
label800194B8:
  v1 = 0x80077FAA;
  v0 = lbu(v1 + 0x00);
  temp = v0 == 0;
  if (temp) goto label800194F0;
  s0 = 0;
  v1 += 770; // 0x0302
label800194D8:
  sw(s1 + 0x00, v1);
  s1 += 4; // 0x0004
  s0++;
  v0 = (int32_t)s0 < 3;
  temp = v0 != 0;
  v1 += 88; // 0x0058
  if (temp) goto label800194D8;
label800194F0:
  a0 = 0x80077FAC;
  v0 = lbu(a0 + 0x00);
  temp = v0 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label80019524;
  v1 = lw(0x80077FD8);
  temp = v1 != v0;
  v0 = a0 + 1032; // 0x0408
  if (temp) goto label80019524;
  sw(s1 + 0x00, v0);
  s1 += 4; // 0x0004
label80019524:
  sw(s1 + 0x00, 0);
label80019528:
  a1 = 0x80077FAA;
  v0 = lbu(a1 + 0x00);
  temp = v0 == 0;
  if (temp) goto label800195E0;
  v0 = lw(0x80077FE0);
  temp = (int32_t)v0 <= 0;
  s0 = 0;
  if (temp) goto label800195E0;
  s3 = spyro_cos_lut;
  s2 = a1 + 0x4C2;
  s1 = s2;
label80019568:
  v0 = 0x66666667;
  a3 = s0 << 8;
  mult(a3, v0);
  a0 = s1;
  a1 = s2 + 160; // 0x00A0
  a2 = sp + 16; // 0x0010
  a3 = (int32_t)a3 >> 31;
  v0 = lw(0x800770F4);
  t0=hi;
  v1 = (int32_t)t0 >> 3;
  v1 -= a3;
  v0 -= v1;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += s3;
  v0 = lhu(v0 + 0x00);
  s1 += 8; // 0x0008
  v0 = v0 << 16;
  v0 = (int32_t)v0 >> 23;
  v0 += 128; // 0x0080
  sw(sp + 0x10, v0);
  sw(sp + 0x14, v0);
  sw(sp + 0x18, v0);
  ra = 0x800195CC;
  function_8001919C();
  v0 = lw(s2 - 0x048C); // 0xFFFFFB74
  s0++;
  v0 = (int32_t)s0 < (int32_t)v0;
  temp = v0 != 0;
  if (temp) goto label80019568;
label800195E0:
  a1 = 0x80077FAB;
  v0 = lbu(a1 + 0x00);
  temp = v0 == 0;
  if (temp) goto label80019678;
  v0 = lw(0x80077FD4);
  temp = (int32_t)v0 <= 0;
  s0 = 0;
  if (temp) goto label80019678;
  s3 = 0x38E38E39;
  s2 = a1 + 1121; // 0x0461
  s1 = s2;
label8001961C:
  a3 = lw(s2 - 0x0424); // 0xFFFFFBDC
  a3 += s0;
  mult(a3, s3);
  a0 = s1;
  a2 = 0;
  s1 += 8; // 0x0008
  a1 = s2 + 264; // 0x0108
  v0 = (int32_t)a3 >> 31;
  t0=hi;
  v1 = (int32_t)t0 >> 1;
  v1 -= v0;
  v0 = v1 << 3;
  v0 += v1;
  a3 -= v0;
  a3 = a3 << 3;
  a1 += a3;
  ra = 0x80019664;
  function_8001919C();
  v0 = lw(s2 - 0x0438); // 0xFFFFFBC8
  s0++;
  v0 = (int32_t)s0 < (int32_t)v0;
  temp = v0 != 0;
  if (temp) goto label8001961C;
label80019678:
  ra = lw(sp + 0x50);
  s3 = lw(sp + 0x4C);
  s2 = lw(sp + 0x48);
  s1 = lw(sp + 0x44);
  s0 = lw(sp + 0x40);
  sp += 88; // 0x0058
}

// size: 0x000000A4
void function_80019698(void)
{
  ra = 0x800196A8;
  function_8001F158();
  spyro_memset32(0x8006FCF4, 0, 0x900);
  ra = 0x800196C4;
  function_8001F798();
  ra = 0x800196CC;
  function_800208FC();
  ra = 0x800196D4;
  function_80020F34();
  ra = 0x800196DC;
  function_80022A2C();
  ra = 0x800196E4;
  function_80059F8C();
  if (lw(0x80075814) == 0) {
    ra = 0x80019700;
    function_80023AC4();
    ra = 0x80019708;
    function_80059A48();
  }
  if (lbu(0x80078760)) {
    ra = 0x80019724;
    function_80058D64();
  }
  ra = 0x8001972C;
  function_80058BA8();
}

// size: 0x00000914
void function_8001973C(void)
{
  sp -= 104; // 0xFFFFFF98
  sw(sp + 0x64, ra);
  sw(sp + 0x60, s2);
  sw(sp + 0x5C, s1);
  sw(sp + 0x58, s0);

  char buf[0x20] = ""; // sp + 56

  a1 = lw(0x800758B4);
  a0 = ((int32_t)a1)/10; // world id
  v1 = (a0 - 1)*6;
  a0 = a1 - a0*10;
  v1 += a0;
  if (a0 == 0) {
    sprintf(buf, "RETURNING HOME...");
  } else {
    if ((a1 < 60 && a0 == 4) || a1 == LIB_GNASTY_GNORC) {
      sprintf(buf, "CONFRONTING %s...", level_names[v1]);
    } else {
      sprintf(buf, "ENTERING %s...", level_names[v1]);
    }
  }
  v1 = 0x100-(strlen(buf)-1)*8;
  a0 = lw(0x800756AC);
  v1 -= v0;
  sw(sp + 0x18, v1);
  if ((int32_t)a0 < 32)
    v0 = (int32_t)lh(spyro_sin_lut + a0*4)/128;
  else if ((int32_t)a0 >= 385)
    v0 = (int32_t)lh(spyro_sin_lut + (0x1A0 - a0)*4)/128;
  else
    v0 = 0x20;
  sw(sp + 0x1C, v0);
  struct game_object *object = addr_to_pointer(lw(gameobject_stack_ptr));

  sw(sp + 0x20, 0x1100);

  create_3d_text1(
    buf,
    addr_to_pointer(sp + 0x18),
    (vec3_32){0x10, 1, 0x1400}, 18, 2
  );

  text_wave_effect1(object, lw(0x800756AC)*2, 12, 1, 128);

  v0 = lw(0x800756AC);
  v0 = (int32_t)v0 < 64;
  if (v0) goto label8001A01C;
  s0 = lw(total_found_gems);
  v0 = (int32_t)s0 < 10;
  s2 = 40;
  if (v0) goto label8001999C;
label8001997C:
  mult(s0, 0x66666667);
  v0 = (int32_t)s0 >> 31;
  t0=hi;
  v1 = (int32_t)t0 >> 2;
  s0 = v1 - v0;
  v0 = (int32_t)s0 < 10;
  s2 -= 10; // 0xFFFFFFF6
  if (v0 == 0) goto label8001997C;
label8001999C:
  v1 = lw(0x800756AC);
  sw(sp + 0x18, s2 + 40);
  sw(sp + 0x20, 0x1100);
  v0 = (int32_t)v1 < 96;
  if (v0 == 0) goto label800199E4;
  v0 = v1 - 64; // 0xFFFFFFC0
  v0 = v0 << 2;
  v1 = lhu(spyro_sin_lut + v0);
  v0 = 272; // 0x0110
  v1 = v1 << 16;
  v1 = (int32_t)v1 >> 22;
  v0 -= v1;
  goto label80019AB4;
label800199E4:
  a0 = lw(0x80075740);
  v0 = 208; // 0x00D0
  if ((int32_t)v1 < (int32_t)a0) goto label80019AB4;
  if ((int32_t)v1 >= (int32_t)(a0 + 32)) goto label80019A34;
  v0 = v1 - 32; // 0xFFFFFFE0
  v0 = a0 - v0;
  v0 = v0 << 2;
  v1 = lhu(spyro_sin_lut + v0);
  v0 = 272; // 0x0110
  v1 = v1 << 16;
  v1 = (int32_t)v1 >> 22;
  v0 -= v1;
  goto label80019AB4;
label80019A34:
  v0 = 272; // 0x0110
  if ((int32_t)v1 < 224) goto label80019AB4;
  v0 = v1 - 224; // 0xFFFFFF20
  if ((int32_t)v1 >= 256) goto label80019A70;
  v0 = v0 << 2;
  v1 = lhu(spyro_sin_lut + v0);
  v0 = 272; // 0x0110
  v1 = v1 << 16;
  v1 = (int32_t)v1 >> 22;
  v0 -= v1;
  goto label80019AB4;
label80019A70:
  v0 = 208; // 0x00D0
  if ((int32_t)v1 < 384) goto label80019AB4;
  v0 = 272; // 0x0110
  if ((int32_t)v1 >= 416) goto label80019AB4;
  v0 = 416; // 0x01A0
  v0 -= v1;
  v0 = v0 << 2;
  v1 = lhu(spyro_sin_lut + v0);
  v0 = 272; // 0x0110
  v1 = v1 << 16;
  v1 = (int32_t)v1 >> 22;
  v0 -= v1;
label80019AB4:
  sw(sp + 0x1C, v0);
  v0 = lw(0x800756AC); // &0x00000000
  v0 = (int32_t)v0 < 224;
  if (v0 == 0) goto label80019AE8;
  sprintf(buf, "TREASURE FOUND");
  goto label80019B0C;
label80019AE8:
  sprintf(buf, "TOTAL TREASURE");
  v0 = lw(sp + 0x18) - 0x10;
  sw(sp + 0x18, v0);
label80019B0C:
  a1 = sp + 24; // 0x0018
  a2 = sp + 40; // 0x0028
  a3 = 18; // 0x0012
  object = addr_to_pointer(lw(gameobject_stack_ptr));
  v0 = 2; // 0x0002
  sw(sp + 0x10, v0);

  sw(sp + 0x28, 0x10);
  sw(sp + 0x2C, 1);
  sw(sp + 0x30, 0x1400);
  create_3d_text1(buf, addr_to_pointer(a1), *(vec3_32 *)addr_to_pointer(a2), a3, lw(sp + 0x10));
  
  text_wave_effect1(object, lw(0x800756AC)*2, 12, 1, 128);
  
  v1 = lw(0x800756AC); // &0x00000000
  v0 = (int32_t)v1 < 128;
  if (v0 == 0) goto label80019BD8;
  v0 = lw(0x80075688); // &0x00000000
  v0 = v0 << 2;
  v1 = lw(0x80077420 + v0);
  v0 = lw(0x8007587C);
  a2 = v1 - v0;
  goto label80019D84;
label80019BD8:
  v0 = (int32_t)v1 < 224;
  if (v0 == 0) goto label80019C90;
  v0 = lw(0x800756C8); // &0x00000000
  v0++;
  a0 = v0 << 1;
  if ((int32_t)a0 < 65) goto label80019C04;
  a0 = 64; // 0x0040
label80019C04:
  v0 = v1 - 128; // 0xFFFFFF80
  a1 = a0 - v0;
  if ((int32_t)a1 >= 0) goto label80019C18;
  a1 = 0;
label80019C18:
  v0 = (int32_t)a0 < (int32_t)a1;
  if (v0 == 0) goto label80019C24;
  a1 = a0;
label80019C24:
  v0 = lw(0x80075688);
  v1 = lw(0x8007587C);
  v0 = v0 << 2;
  v0 = lw(0x80077420 + v0);
  v0 -= v1;
  mult(v0, a1);
  v0=lo;
  div_psx(v0,a0);
  a2=lo;
  goto label80019D84;
label80019C90:
  v0 = (int32_t)v1 < 272;
  if (v0 == 0) goto label80019CCC;
  v0 = lw(0x80075688);
  v1 = lw(0x8007587C);
  v0 = v0 << 2;
  v0 = lw(0x80077420 + v0);
  a0 = lw(total_found_gems);
  v0 -= v1;
  a2 = a0 - v0;
  goto label80019D84;
label80019CCC:
  v0 = lw(0x800756C8);
  v0++;
  a0 = v0 << 1;
  if ((int32_t)a0 < 65) goto label80019CF0;
  a0 = 64; // 0x0040
label80019CF0:
  v0 = v1 - 272; // 0xFFFFFEF0
  a1 = a0 - v0;
  v0 = (int32_t)a0 < (int32_t)a1;
  if ((int32_t)a1 >= 0) goto label80019D04;
  a1 = 0;
  v0 = (int32_t)a0 < (int32_t)a1;
label80019D04:
  if (v0 == 0) goto label80019D10;
  a1 = a0;
label80019D10:
  v0 = lw(0x80075688); // &0x00000000
  v1 = lw(0x8007587C);
  v0 = v0 << 2;
  v0 = lw(0x80077420 + v0);
  v0 -= v1;
  mult(v0, a1);
  v0=lo;
  div_psx(v0,a0);
  v1=lo;
  v0 = lw(total_found_gems);
  a2 = v0 - v1;
label80019D84:
  sprintf(buf, "%d", a2);

  sw(sp + 0x18, s2 + 0x178);
  sw(sp + 0x20, 0xF80);
  object = addr_to_pointer(lw(gameobject_stack_ptr));
  create_3d_text2(buf, addr_to_pointer(sp + 0x18), 20, 2);
  text_wave_effect1(object, lw(0x800756AC)*2, 12, 1, 128);

  object = addr_to_pointer(lw(gameobject_stack_ptr));
  
  for (int i = 0; i < 32; i++) {
    s1 = i*16;
    v1 = lw(0x8007571C) + s1;
    if (lbu(v1 + 0x08)) {
      object--;
      memset(object, 0, 88);
      object->p = (vec3_32){
        s2 + lh(v1 + 0x00) + 0x140,
        0xC6 - lh(v1 + 0x02),
        0x1000
      };
      object->modelID = lbu(0x80077DB8 + i);

      object->rotx = lbu(v1 + 0x0A);
      object->roty = lbu(v1 + 0x0B);
      object->rotz = lbu(v1 + 0x0C);

      object->unknown47 = 0x7F;
      object->unknown4F = object->modelID - 0x52;
      object->render_distance = 0xFF;
    }
  }
  object--;
  memset(object, 0, 88);
  object->modelID = 0x1D9; // chest model
  if ((int32_t)lw(0x800756AC) < 224) {
    object->p = (vec3_32){
      s2 + 0x140,
      lw(sp + 0x1C) + 8,
      0x800
    };
  } else {
    object->p = (vec3_32){
      s2 + 0x140,
      lw(sp + 0x1C) + 12,
      0x600
    };
  }
  object->rotx = 6;
  object->rotz = 0xB0;
  object->unknown47 = 0x7F;
  object->unknown4F = 11;
  object->render_distance = 0xFF;
  sw(gameobject_stack_ptr, pointer_to_addr(object));
label8001A01C:
  sw(0x8006FCF4 + 0x2400, 0);
  ra = 0x8001A02C;
  function_80018880();
  ra = 0x8001A034;
  function_80022A2C();
  ra = lw(sp + 0x64);
  s2 = lw(sp + 0x60);
  s1 = lw(sp + 0x5C);
  s0 = lw(sp + 0x58);
  sp += 0x68;
}

// size: 0x000003BC
void function_8001A050(void)
{
  uint32_t temp;
  sp -= 96; // 0xFFFFFFA0
  v0 = lbu(SKYBOX_DATA + 0x10);
  v1 = lbu(SKYBOX_DATA + 0x11);
  a3 = lbu(SKYBOX_DATA + 0x12);
  sw(sp + 0x58, ra);
  sw(sp + 0x54, s1);
  sw(sp + 0x50, s0);
  sb(0x80076EF9, v0);
  sb(0x80076EFA, v1);
  sb(0x80076EFB, a3);
  sb(0x80076F7D, v0);
  sb(0x80076F7E, v1);
  sb(0x80076F7F, a3);
  spyro_memset32(0x8006FCF4, 0, 0x900);
  v0 = lw(0x800756B0); // &0x00000000
  temp = v0 == 0;
  if (temp) goto label8001A0D8;
  ra = 0x8001A0D8;
  function_8001973C();
label8001A0D8:
  ra = 0x8001A0E0;
  function_80023AC4();
  v0 = lw(SKYBOX_DATA + 0x00);
  temp = v0 == 0;
  if (temp) goto label8001A334;
  v0 = lw(0x80075910);
  v0 -= 2; // 0xFFFFFFFE
  sw(0x80075910, v0);
  temp = (int32_t)v0 >= 0;
  if (temp) goto label8001A11C;
  sw(0x80075910, 0);
label8001A11C:
  v0 = lw(0x80075910);
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label8001A31C;
  a0 = sp + 16; // 0x0010
  a2 = 32; // 0x0020
  ra = 0x8001A13C;
  spyro_memset32(a0, a1, a2);
  a0 = sp + 16; // 0x0010
  ra = 0x8001A144;
  function_800625F8();
  s0 = 0x80076E1E;
  v0 = lh(s0 + 0x00);
  a0 = lw(0x80075910);
  s1 = 4096; // 0x1000
  sh(sp + 0x10, s1);
  a0 = v0 - a0;
  ra = 0x8001A168;
  v0 = spyro_cos(a0);
  v1 = lh(s0 + 0x00);
  a0 = lw(0x80075910);
  sh(sp + 0x18, v0);
  a0 = v1 - a0;
  ra = 0x8001A180;
  v0 = spyro_sin(a0);
  v1 = lh(s0 + 0x00);
  a0 = lw(0x80075910);
  sh(sp + 0x1E, v0);
  a0 = v1 - a0;
  ra = 0x8001A198;
  v0 = spyro_sin(a0);
  v1 = lh(s0 + 0x00);
  a0 = lw(0x80075910);
  v0 = -v0;
  sh(sp + 0x1A, v0);
  a0 = v1 - a0;
  ra = 0x8001A1B4;
  v0 = spyro_cos(a0);
  s0 = sp + 48; // 0x0030
  a0 = s0;
  a1 = 0;
  a2 = 32; // 0x0020
  sh(sp + 0x20, v0);
  ra = 0x8001A1CC;
  spyro_memset32(a0, a1, a2);
  a0 = lh(0x80076E20);
  ra = 0x8001A1DC;
  v0 = spyro_cos(a0);
  a0 = lh(0x80076E20);
  sh(sp + 0x30, v0);
  ra = 0x8001A1EC;
  v0 = spyro_sin(a0);
  a0 = lh(0x80076E20);
  v0 = -v0;
  sh(sp + 0x3C, v0);
  sh(sp + 0x38, s1);
  ra = 0x8001A204;
  v0 = spyro_sin(a0);
  a0 = lh(0x80076E20);
  sh(sp + 0x34, v0);
  ra = 0x8001A214;
  v0 = spyro_cos(a0);
  a0 = sp + 16; // 0x0010
  a1 = s0;
  sh(sp + 0x40, v0);
  ra = 0x8001A224;
  function_800624E8();
  a0 = s0;
  a1 = 0;
  a2 = 32; // 0x0020
  ra = 0x8001A234;
  spyro_memset32(a0, a1, a2);
  a0 = lh(0x80076E1C);
  ra = 0x8001A244;
  v0 = spyro_cos(a0);
  a0 = lh(0x80076E1C);
  sh(sp + 0x30, v0);
  ra = 0x8001A254;
  v0 = spyro_sin(a0);
  a0 = lh(0x80076E1C);
  v0 = -v0;
  sh(sp + 0x36, v0);
  ra = 0x8001A268;
  v0 = spyro_sin(a0);
  a0 = lh(0x80076E1C);
  sh(sp + 0x32, v0);
  ra = 0x8001A278;
  v0 = spyro_cos(a0);
  a0 = sp + 16; // 0x0010
  a1 = s0;
  sh(sp + 0x38, v0);
  sh(sp + 0x40, s1);
  ra = 0x8001A28C;
  function_800624E8();
  a0 = s0;
  a1 = sp + 16; // 0x0010
  a2 = 20; // 0x0014
  ra = 0x8001A29C;
  spyro_memcpy32(a0, a1, a2);
  v1 = lh(sp + 0x16);
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 6;
  temp = (int32_t)v0 >= 0;
  if (temp) goto label8001A2BC;
  v0 += 511; // 0x01FF
label8001A2BC:
  v1 = lh(sp + 0x18);
  v0 = (int32_t)v0 >> 9;
  sh(sp + 0x36, v0);
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 6;
  temp = (int32_t)v0 >= 0;
  if (temp) goto label8001A2E0;
  v0 += 511; // 0x01FF
label8001A2E0:
  v1 = lh(sp + 0x1A);
  v0 = (int32_t)v0 >> 9;
  sh(sp + 0x38, v0);
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 6;
  temp = (int32_t)v0 >= 0;
  if (temp) goto label8001A304;
  v0 += 511; // 0x01FF
label8001A304:
  v0 = (int32_t)v0 >> 9;
  sh(sp + 0x3A, v0);
  a0 = -1; // 0xFFFFFFFF
  a1 = sp + 16; // 0x0010
  a2 = s0;
  goto label8001A32C;
label8001A31C:
  a0 = -1; // 0xFFFFFFFF
  a1 = 0x80076DE4;
  a2 = a1 - 20; // 0xFFFFFFEC
label8001A32C:
  ra = 0x8001A334;
  draw_skybox(a0, a1, a2);
label8001A334:
  wait_two_vsyncs();

  PutDispEnv(&backbuffer_disp->disp);
  PutDrawEnv(&backbuffer_disp->draw);
  DrawOTag(spyro_combine_all_command_buffers(0x800));
  ra = lw(sp + 0x58);
  s1 = lw(sp + 0x54);
  s0 = lw(sp + 0x50);
  sp += 96; // 0x0060
}

union color15 {
  uint16_t val;
  struct {
    uint16_t r : 5;
    uint16_t g : 5;
    uint16_t b : 5;
  };
};

// size: 0x0000008C
// rgb to single channel
// a0: src / dst
// a1: dst size in bytes, src size in halfwords (2 bytes) (pixels)
void rgb_to_grey(uint32_t *buf, uint32_t len)
{
  uint8_t *dst = (uint8_t *)buf;
  uint16_t *src = (uint16_t *)buf;
  for (int i = 0; i < len; i++) {
    union color15 col;
    col.val = src[i];
    dst[i] = (col.r*4 + col.g*3 + col.b)/8;
  }
}

void function_80017E98(void)
{
  rgb_to_grey(addr_to_pointer(a0), a1);
}

// size: 0x00002288
void function_8001A40C(void)
{
  uint32_t temp;
  v0 = lw(0x800758B8);
  sp -= 336; // 0xFFFFFEB0
  sw(sp + 0x014C, ra);
  sw(sp + 0x0148, fp);
  sw(sp + 0x0144, s7);
  sw(sp + 0x0140, s6);
  sw(sp + 0x013C, s5);
  sw(sp + 0x0138, s4);
  sw(sp + 0x0134, s3);
  sw(sp + 0x0130, s2);
  sw(sp + 0x012C, s1);
  temp = v0 != 0;
  sw(sp + 0x0128, s0);
  if (temp) goto label8001A5E0;
  s3 = 0;
  ra = 0x8001A44C;
  function_800521C0();
  s4 = 224; // 0xE0
  ra = 0x8001A454;
  function_80019698();
  ra = 0x8001A45C;
  function_800573C8();
  ra = 0x8001A464;
  function_80050BD0();
  ra = 0x8001A46C;
  function_8002B9CC();
  DrawSync(0);
  VSync(0);
  s5 = DISP1;
  
  PutDispEnv(&backbuffer_disp->disp);
  PutDrawEnv(&backbuffer_disp->draw);
  DrawOTag(spyro_combine_all_command_buffers(0x800));

  s0 = 0x800785F0;
  s2 = 0x800785E8;
  DrawSync(0);
  VSync(0);
  PutDispEnv(&backbuffer_disp->disp);

  StoreImage((RECT[]){{0x200, 0, 0x100, 0xE1}}, addr_to_pointer(lw(s0) - 0x1C200));
label8001A514:
  if (backbuffer_disp != addr_to_pointer(DISP1))
    a2 = 0xF8;
  else
    a2 = 0x08;
  a0 = sp + 0x18;
  a1 = lw(s2);
  sh(sp + 0x18, s3*0x80);
  sh(sp + 0x1A, a2);
  sh(sp + 0x1C, 0x80);
  sh(sp + 0x1E, 0xE0);
  StoreImage(addr_to_pointer(a0), addr_to_pointer(a1));
  DrawSync(0);
  rgb_to_grey(addr_to_pointer(lw(s2)), 0x7000);
  a0 = sp + 0x18;
  sh(sp + 0x18, 0x200 + s3*0x40);
  sh(sp + 0x1A, 0);
  sh(sp + 0x1C, 0x40);
  sh(sp + 0x1E, 0xE0);
  LoadImage(addr_to_pointer(a0), addr_to_pointer(lw(s2)));
  s3++;
  v0 = (int32_t)s3 < 4;
  temp = v0 != 0;
  a2 = 8; // 0x0008
  if (temp) goto label8001A514;
  a0 = sp + 24; // 0x0018
  a1 = 0x8006F310; // &0x04210020
  v0 = 512; // 0x0200
  sh(sp + 0x18, v0);
  v0 = 224; // 0x00E0
  sh(sp + 0x1A, v0);
  v0 = 32; // 0x0020
  sh(sp + 0x1C, v0);
  v0 = 1; // 0x0001
  sh(sp + 0x1E, v0);

  LoadImage(addr_to_pointer(a0), addr_to_pointer(a1)); // color pallette
  a0 = 0;
  ra = 0x8001A5C8;
  v0 = DrawSync(a0);
  a0 = -1; // 0xFFFFFFFF
  ra = 0x8001A5D0;
  v0 = VSync(a0);
  sw(drawn_frame, v0);
  goto label8001C648;
label8001A5E0:
  if (backbuffer_disp == addr_to_pointer(DISP1))
    a0 = DISP2;
  else
    a0 = DISP1;
  s3 = 0;
  PutDrawEnv(addr_to_pointer(a0));
  a0 =  0x1C000;
  a1 = -0x1C200;
  v1 = lw(0x800785E8);
  v0 = lw(0x800785F0);
  sw(0x800758B0, 0);
  sw(allocator1_ptr, v1);
  v1 += 0x1C000;
  v0 -= 0x1C200;
  sw(allocator1_end, v1);
  sw(0x800756FC, v0); // &0x00000000
  sw(gameobject_stack_ptr, v0);
label8001A658:
  v1 = s3 << 7;
  a3 = s3 + 136; // 0x0088
  s0 = lw(allocator1_ptr);
  sw(s0 + 0x00, 0x09000000);
  sb(s0 + 0x04, 0x4C); // r
  sb(s0 + 0x05, 0x80); // g
  sb(s0 + 0x06, 0x40); // b
  sb(s0 + 0x07, 0x2C);
  sh(s0 + 0x08, v1);
  sh(s0 + 0x0A, 0x08);
  a1 = lhu(s0 + 0x0A);
  v1 = lhu(s0 + 0x0A);
  sh(s0 + 0x10, lhu(s0 + 0x08) + 0x80);
  v1 += 223; // 0xDF
  sh(s0 + 0x1A, v1);
  v1 = lhu(s0 + 0x0A);
  s3++;
  sb(s0 + 0x0D, 0);
  sh(s0 + 0x12, a1);
  a1 = lbu(s0 + 0x0D);
  s4 = 64; // 0x40
  sb(s0 + 0x0C, 0);
  sh(s0 + 0x18, lhu(s0 + 0x08));
  a2 = lbu(s0 + 0x0C);
  sh(s0 + 0x20, lhu(s0 + 0x08) + 0x80);
  v0 = lbu(s0 + 0x0C);
  v1 += 223; // 0x00DF
  sh(s0 + 0x22, v1);
  v1 = lbu(s0 + 0x0D);
  a0 = s0;
  sb(s0 + 0x15, a1);
  sb(s0 + 0x1C, a2);
  v0 += 128; // 0x0080
  sb(s0 + 0x14, v0);
  v0 = lbu(s0 + 0x0C);
  v1 -= 33; // 0xFFFFFFDF
  sb(s0 + 0x1D, v1);
  v1 = lbu(s0 + 0x0D);
  v0 += 128; // 0x0080
  v1 -= 33; // 0xFFFFFFDF
  sb(s0 + 0x24, v0);
  v0 = 14368; // 0x3820
  sb(s0 + 0x25, v1);
  sh(s0 + 0x0E, v0);
  sh(s0 + 0x16, a3);
  append_gpu_command_block(addr_to_pointer(a0));
  a0 = s0 + 40; // 0x0028
  v0 = (int32_t)s3 < 4;
  sw(allocator1_ptr, a0);
  temp = v0 != 0;
  if (temp) goto label8001A658;
  v0 = 0x03000000;
  v1 = 231; // 0x00E7
  sw(s0 + 0x28, v0);
  v0 = 512; // 0x0200
  sb(s0 + 0x2F, s4);
  sh(s0 + 0x30, 0);
  sh(s0 + 0x32, v1);
  sh(s0 + 0x34, v0);
  sh(s0 + 0x36, v1);
  sb(s0 + 0x2C, 0);
  sb(s0 + 0x2D, 0);
  sb(s0 + 0x2E, 0);
  append_gpu_command_block(addr_to_pointer(a0));
  a0 = s0 + 56; // 0x0038
  v1 = lw(0x800757D8); // &0x00000000
  v0 = 2; // 0x0002
  sw(allocator1_ptr, a0);
  temp = v1 != v0;
  v0 = 3; // 0x0003
  if (temp) goto label8001B64C;
  s1 = a0;
  a1 = 1; // 0x0001
  a2 = 0;
  a3 = 64; // 0x0040
  sw(sp + 0x10, 0);
  SetDrawMode(addr_to_pointer(a0), a1, a2, a3, addr_to_pointer(lw(sp + 0x10)));
  append_gpu_command_block(addr_to_pointer(s1));
  v0 = 0x05000000;
  sw(s0 + 0x44, v0);
  v0 = 42; // 0x002A
  sb(s0 + 0x4B, v0);
  v0 = 67; // 0x0043
  sb(s0 + 0x48, s4);
  sb(s0 + 0x49, s4);
  sb(s0 + 0x4A, s4);
  v1 = lw(0x800757C8); // &0x00000000
  sh(s0 + 0x4E, v0);
  sh(s0 + 0x52, v0);
  v0 = 1; // 0x0001
  temp = v1 != v0;
  s1 = s0 + 68; // 0x0044
  if (temp) goto label8001A82C;
  v0 = 84; // 0x0054
  sh(s0 + 0x4C, v0);
  v0 = lw(0x800756D8); // &0x00000000
  v1 = 428; // 0x01AC
  sh(s0 + 0x50, v1);
  v0 = 0 < v0;
  sb(sp + 0xA0, v0);
  v0 = v0 << 4;
  v0 += 182; // 0x00B6
  goto label8001A848;
label8001A82C:
  t1 = 1; // 0x0001
  v0 = 140; // 0x008C
  sb(sp + 0xA0, t1);
  sh(s0 + 0x4C, v0);
  v0 = 372; // 0x0174
  sh(s0 + 0x50, v0);
  v0 = 176; // 0x00B0
label8001A848:
  sh(s0 + 0x56, v0);
  sh(s0 + 0x5A, v0);
  v0 = lhu(s1 + 0x08);
  v1 = lhu(s1 + 0x0C);
  a0 = s1;
  sh(s1 + 0x10, v0);
  sh(s1 + 0x14, v1);
  append_gpu_command_block(addr_to_pointer(a0));
  sw(allocator1_ptr, s1 + 0x18);
  gui_line(0xE0, 0x61, 0x120, 0x61);
  v1 = lw(0x800757C8);
  if (v1 == 1) {
    s0 = 0xB6 + lbu(sp + 0xA0)*0x10;

    gui_line(0x54, 0x43, 0x1AC, 0x43);
    gui_line(0x1AC, 0x43, 0x1AC, s0);
    gui_line(0x1AC, s0, 0x54, s0);
    gui_line(0x54, s0, 0x54, 0x43);
  } else {
    s0 = 0x9E + lbu(sp + 0xA0)*18;

    gui_line(0x8C, 0x43, 0x174, 0x43);
    gui_line(0x174, 0x43, 0x174, s0);
    gui_line(0x174, s0, 0x8C, s0);
    gui_line(0x8C, s0, 0x8C, 0x43);
  }
  s0 = sp + 0x40;
  sw(sp + 0x40, 0xBA);
  sw(sp + 0x44, 0x52);
  sw(sp + 0x48, 0xC00);
  create_3d_text2("PAUSED", addr_to_pointer(s0), 28, 11);
  v0 = lw(0x800757C8); // &0x00000000
  fp = 2; // 0x0002
  temp = v0 != fp;
  s6 = 1; // 0x0001
  if (temp) goto label8001AAF0;
  s1 = sp + 80; // 0x0050
  a3 = 18; // 0x0012
  v0 = 16; // 0x0010
  s7 = 1; // 0x0001
  sw(sp + 0x50, v0);
  v0 = 5120; // 0x1400
  sw(sp + 0x58, v0);
  v0 = 183; // 0x00B7
  sw(sp + 0x40, v0);
  v0 = 119; // 0x0077
  sw(sp + 0x54, s7);
  sw(sp + 0x44, v0);
  sw(sp + 0x48, 0x1100);
  create_3d_text1("QUIT GAME?", addr_to_pointer(s0), *(vec3_32 *)addr_to_pointer(s1), 18, 11);
  v0 = 192; // 0xC0
  s6 = 148; // 0x94
  sw(sp + 0x40, 0xC0);
  sw(sp + 0x44, 0x94);
  sw(sp + 0x48, 0x1100);
  create_3d_text1("YES", addr_to_pointer(s0), *(vec3_32 *)addr_to_pointer(s1), 18, 11);
  v0 = lw(0x80075720); // &0x00000000
  temp = v0 != 0;
  if (temp) goto label8001AA40;
  s2 = lw(gameobject_stack_ptr);
  s5 = 3; // 0x0003
label8001AA40:
  sw(sp + 0x40, 0x128);
  sw(sp + 0x44, s6);
  sw(sp + 0x48, 0x1100);
  create_3d_text1("NO", addr_to_pointer(s0), *(vec3_32 *)addr_to_pointer(s1), 18, 11);
  v0 = lw(0x80075720); // &0x00000000
  temp = v0 != s7;
  a1 = s5 & 0xFF;
  if (temp) goto label8001AA90;
  s2 = lw(gameobject_stack_ptr);
  s5 = 2; // 0x0002
  a1 = s5 & 0xFF;
label8001AA90:
  temp = (int32_t)a1 <= 0;
  s3 = 0;
  if (temp) goto label8001C53C;
  a2 = spyro_cos_lut;
  a0 = 0;
label8001AAA4:
  v0 = lw(0x800758B8);
  s3++;
  v0 = v0 << 3;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a2;
  v1 = lh(v0 + 0x00);
  a0 += 12; // 0x000C
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 9;
  sb(s2 + 0x46, v0);
  v0 = (int32_t)s3 < (int32_t)a1;
  temp = v0 != 0;
  s2 += 88; // 0x0058
  if (temp) goto label8001AAA4;
  goto label8001C53C;
label8001AAF0:
  temp = v0 != s6;
  if (temp) goto label8001B3AC;
  s1 = sp + 0x60;
  sw(sp + 0x60, 0x0F);
  s7 = 107; // 0x006B
  sw(sp + 0x64, s6);
  sw(sp + 0x68, 0x1600);

  sw(sp + 0x40, s7);
  sw(sp + 0x44, 0x6C);
  sw(sp + 0x48, 0x1400);
  create_3d_text1("SOUND EFFECTS", addr_to_pointer(s0), *(vec3_32 *)addr_to_pointer(s1), 16, 11);
  v0 = lw(0x80075720); // &0x00000000
  temp = v0 != 0;
  if (temp) goto label8001AB64;
  s2 = lw(gameobject_stack_ptr);
  s5 = 11; // 0x000B
label8001AB64:
  sw(sp + 0x40, 0x79);
  sw(sp + 0x44, 0x7C);
  sw(sp + 0x48, 0x1400);
  create_3d_text1("MUSIC VOLUME", addr_to_pointer(s0), *(vec3_32 *)addr_to_pointer(s1), 16, 11);
  v0 = lw(0x80075720); // &0x00000000
  temp = v0 != s6;
  if (temp) goto label8001ABB4;
  s2 = lw(gameobject_stack_ptr);
  s5 = 11; // 0x000B
label8001ABB4:
  sw(sp + 0x40, s7);
  sw(sp + 0x44, 0x8C);
  sw(sp + 0x48, 0x1400);
  create_3d_text1("SPEAKER SETUP", addr_to_pointer(s0), *(vec3_32 *)addr_to_pointer(s1), 16, 11);
  v0 = lw(0x80075720); // &0x00000000
  temp = v0 != fp;
  if (temp) goto label8001AC00;
  s2 = lw(gameobject_stack_ptr);
  s5 = 12; // 0x000C
label8001AC00:
  v0 = lw(0x800756D8); // &0x00000000
  temp = v0 == 0;
  a1 = s0;
  if (temp) goto label8001AC60;
  sw(sp + 0x40, 0xA3);
  sw(sp + 0x44, 0x9C);
  sw(sp + 0x48, 0x1400);
  create_3d_text1("VIBRATION", addr_to_pointer(s0), *(vec3_32 *)addr_to_pointer(s1), 16, 11);
  v1 = lw(0x80075720); // &0x00000000
  v0 = 3; // 0x0003
  temp = v1 != v0;
  if (temp) goto label8001AC60;
  s2 = lw(gameobject_stack_ptr);
  s5 = 9; // 0x0009
label8001AC60:
  s3 = sp + 0x40;
  s1 = sp + 0x60;
  s6 = 0x1100;
  s0 = lbu(sp + 0xA0)*16;
  sw(sp + 0x40, 0xD0);
  sw(sp + 0x44, s0 + 0x9C);
  sw(sp + 0x48, 0x1100);
  create_3d_text1("CAMERA", addr_to_pointer(s3), *(vec3_32 *)addr_to_pointer(s1), 16, 11);
  v1 = lw(0x80075720); // &0x00000000
  v0 = 4; // 0x0004
  temp = v1 != v0;
  if (temp) goto label8001ACC8;
  s2 = lw(gameobject_stack_ptr);
  s5 = 6;
label8001ACC8:
  sw(sp + 0x40, 0xEE);
  sw(sp + 0x44, s0 + 0xAC);
  sw(sp + 0x48, 0x1100);
  create_3d_text1("DONE", addr_to_pointer(s3), *(vec3_32 *)addr_to_pointer(s1), 16, 11);
  v1 = lw(0x80075720); // &0x00000000
  v0 = 5; // 0x0005
  temp = v1 != v0;
  a1 = s5 & 0xFF;
  if (temp) goto label8001AD1C;
  s2 = lw(gameobject_stack_ptr);
  s5 = 4; // 0x0004
  a1 = s5 & 0xFF;
label8001AD1C:
  temp = (int32_t)a1 <= 0;
  s3 = 0;
  if (temp) goto label8001AD74;
  a2 = spyro_cos_lut;
  a0 = 0;
label8001AD30:
  v0 = lw(0x800758B8);
  s3++;
  v0 = v0 << 3;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a2;
  v1 = lh(v0 + 0x00);
  a0 += 12; // 0x000C
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 9;
  sb(s2 + 0x46, v0);
  v0 = (int32_t)s3 < (int32_t)a1;
  temp = v0 != 0;
  s2 += 88; // 0x0058
  if (temp) goto label8001AD30;
label8001AD74:
  v0 = lw(0x80075754); // &0x00000000
  temp = v0 != 0;
  a1 = sp + 0x40;
  if (temp) goto label8001AE30;
  sw(sp + 0x40, 0x142);
  sw(sp + 0x44, 0x6C);
  sw(sp + 0x48, 0x1400);
  create_3d_text1("OFF", addr_to_pointer(sp + 0x40), *(vec3_32 *)addr_to_pointer(sp + 0x60), 16, 11);
  v0 = lw(0x80075720); // &0x00000000
  temp = v0 != 0;
  a0 = 0;
  if (temp) goto label8001AF28;
  s2 = lw(gameobject_stack_ptr);
  s3 = 0;
  a1 = spyro_cos_lut;
label8001ADE4:
  v0 = lw(0x800758B8);
  s3++;
  v0 = v0 << 3;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a1;
  v1 = lh(v0 + 0x00);
  a0 += 12; // 0x000C
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 9;
  sb(s2 + 0x46, v0);
  v0 = (int32_t)s3 < 3;
  temp = v0 != 0;
  s2 += 88; // 0x0058
  if (temp) goto label8001ADE4;
  goto label8001AF28;
label8001AE30:
  temp = (int32_t)v0 <= 0;
  s3 = 0;
  if (temp) goto label8001AF28;
  s2 = spyro_cos_lut;
  s1 = 0;
  s0 = 320; // 0x140
label8001AE48:
  sw(gameobject_stack_ptr, lw(gameobject_stack_ptr) - 88);
  spyro_memset32(lw(gameobject_stack_ptr), 0, 88);
  a0 = lw(gameobject_stack_ptr);
  sh(a0 + 0x36, 0x1B2); // 'I'

  sw(a0 + 0x0C, s0);
  sw(a0 + 0x10, 0x6D);
  sw(a0 + 0x14, 0x1400);
  
  if (lw(0x80075720)) goto label8001AED8;
  v0 = lw(0x800758B8);
  v0 = v0 << 3;
  v0 += s1;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += s2;
  v1 = lh(v0 + 0x00);
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 9;
  sb(a0 + 0x46, v0);
  goto label8001AEDC;
label8001AED8:
  sb(a0 + 0x46, 0);
label8001AEDC:
  s1 += 12; // 0x000C
  v1 = lw(gameobject_stack_ptr);
  v0 = 127; // 0x007F
  sb(v1 + 0x47, v0);
  v1 = lw(gameobject_stack_ptr);
  v0 = 11; // 0x000B
  sb(v1 + 0x4F, v0);
  v1 = lw(gameobject_stack_ptr);
  v0 = 255; // 0x00FF
  sb(v1 + 0x50, v0);
  v0 = lw(0x80075754); // &0x00000000
  s3++;
  v0 = (int32_t)s3 < (int32_t)v0;
  temp = v0 != 0;
  s0 += 10; // 0x000A
  if (temp) goto label8001AE48;
label8001AF28:
  v0 = lw(0x80075748); // &0x00000000
  temp = v0 != 0;
  a1 = sp + 0x40;
  if (temp) goto label8001AFE4;
  sw(sp + 0x40, 0x142);
  sw(sp + 0x44, 0x7C);
  sw(sp + 0x48, 0x1400);
  create_3d_text1("OFF", addr_to_pointer(sp + 0x40), *(vec3_32 *)addr_to_pointer(sp + 0x60), 16, 11);
  v1 = lw(0x80075720); // &0x00000000
  v0 = 1; // 0x0001
  temp = v1 != v0;
  a0 = 0;
  if (temp) goto label8001B0E0;
  s2 = lw(gameobject_stack_ptr);
  s3 = 0;
  a1 = spyro_cos_lut;
label8001AF98:
  v0 = lw(0x800758B8);
  s3++;
  v0 = v0 << 3;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a1;
  v1 = lh(v0 + 0x00);
  a0 += 12; // 0x000C
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 9;
  sb(s2 + 0x46, v0);
  v0 = (int32_t)s3 < 3;
  temp = v0 != 0;
  s2 += 88; // 0x0058
  if (temp) goto label8001AF98;
  goto label8001B0E0;
label8001AFE4:
  temp = (int32_t)v0 <= 0;
  s3 = 0;
  if (temp) goto label8001B0E0;
  s2 = spyro_cos_lut;
  s1 = 0;
  s0 = 320; // 0x0140
label8001AFFC:
  sw(gameobject_stack_ptr, lw(gameobject_stack_ptr) - 88);
  spyro_memset32(lw(gameobject_stack_ptr), 0, 88);
  a0 = lw(gameobject_stack_ptr);
  v1 = lw(0x80075720);
  sh(a0 + 0x36, 0x1B2); // 'I'
  v0 = 125; // 0x007D
  sw(a0 + 0x10, v0);
  v0 = 5120; // 0x1400
  sw(a0 + 0x14, v0);
  v0 = 1; // 0x0001
  temp = v1 != v0;
  sw(a0 + 0x0C, s0);
  if (temp) goto label8001B090;
  v0 = lw(0x800758B8);
  v0 = v0 << 3;
  v0 += s1;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += s2;
  v1 = lh(v0 + 0x00);
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 9;
  sb(a0 + 0x46, v0);
  goto label8001B094;
label8001B090:
  sb(a0 + 0x46, 0);
label8001B094:
  s1 += 12; // 0x000C
  v1 = lw(gameobject_stack_ptr);
  v0 = 127; // 0x007F
  sb(v1 + 0x47, v0);
  v1 = lw(gameobject_stack_ptr);
  v0 = 11; // 0x000B
  sb(v1 + 0x4F, v0);
  v1 = lw(gameobject_stack_ptr);
  v0 = 255; // 0x00FF
  sb(v1 + 0x50, v0);
  v0 = lw(0x80075748); // &0x00000000
  s3++;
  v0 = (int32_t)s3 < (int32_t)v0;
  temp = v0 != 0;
  s0 += 10; // 0x000A
  if (temp) goto label8001AFFC;
label8001B0E0:
  sw(sp + 0x40, 0x142);
  sw(sp + 0x44, 0x8C);
  sw(sp + 0x48, 0x1400);
  if (lw(0x80076240)) {
    create_3d_text1("MONO", addr_to_pointer(sp + 0x40), *(vec3_32 *)addr_to_pointer(sp + 0x60), 16, 11);
    a1 = 4; // 0x0004
  } else {
    create_3d_text1("STEREO", addr_to_pointer(sp + 0x40), *(vec3_32 *)addr_to_pointer(sp + 0x60), 16, 11);
    a1 = 6; // 0x0006
  }
  v1 = lw(0x80075720); // &0x00000000
  v0 = 2; // 0x0002
  temp = v1 != v0;
  if (temp) goto label8001B1C4;
  s2 = lw(gameobject_stack_ptr);
  temp = a1 == 0;
  s3 = 0;
  if (temp) goto label8001B1C4;
  a2 = spyro_cos_lut;
  a0 = 0;
label8001B180:
  v0 = lw(0x800758B8);
  s3++;
  v0 = v0 << 3;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a2;
  v1 = lh(v0 + 0x00);
  a0 += 12; // 0x000C
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 9;
  sb(s2 + 0x46, v0);
  v0 = (int32_t)s3 < (int32_t)a1;
  temp = v0 != 0;
  s2 += 88; // 0x0058
  if (temp) goto label8001B180;
label8001B1C4:
  v0 = lw(0x800756D8); // &0x00000000
  temp = v0 == 0;
  if (temp) goto label8001B2B8;
  v1 = lw(0x800757A4); // &0x00000000
  sw(sp + 0x40, 0x142);
  sw(sp + 0x44, 0x9C);
  sw(sp + 0x48, 0x1400);
  temp = v1 == 0;
  if (temp) goto label8001B220;
  create_3d_text1("ON", addr_to_pointer(sp + 0x40), *(vec3_32 *)addr_to_pointer(sp + 0x60), 16, 11);
  a1 = 2; // 0x0002
  goto label8001B244;
label8001B220:
  create_3d_text1("OFF", addr_to_pointer(sp + 0x40), *(vec3_32 *)addr_to_pointer(sp + 0x60), 16, 11);
  a1 = 3; // 0x0003
label8001B244:
  v1 = lw(0x80075720); // &0x00000000
  v0 = 3; // 0x0003
  temp = v1 != v0;
  if (temp) goto label8001B2B8;
  s2 = lw(gameobject_stack_ptr);
  temp = a1 == 0;
  s3 = 0;
  if (temp) goto label8001B2B8;
  a2 = spyro_cos_lut;
  a0 = 0;
label8001B274:
  v0 = lw(0x800758B8);
  s3++;
  v0 = v0 << 3;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a2;
  v1 = lh(v0 + 0x00);
  a0 += 12; // 0x000C
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 9;
  sb(s2 + 0x46, v0);
  v0 = (int32_t)s3 < (int32_t)a1;
  temp = v0 != 0;
  s2 += 88; // 0x0058
  if (temp) goto label8001B274;
label8001B2B8:
  t1 = lbu(sp + 0xA0);
  sw(sp + 0x40, 0x142);
  sw(sp + 0x44, t1*16 + 0x9C);
  sw(sp + 0x48, 0x1400);
  temp = lw(CAMERA_MODE) != 2;
  if (temp) goto label8001B310;
  create_3d_text1("ACTIVE", addr_to_pointer(sp + 0x40), *(vec3_32 *)addr_to_pointer(sp + 0x60), 16, 11);
  a1 = 6; // strlen("ACTIVE");
  goto label8001B330;
label8001B310:
  create_3d_text1("PASSIVE", addr_to_pointer(sp + 0x40), *(vec3_32 *)addr_to_pointer(sp + 0x60), 16, 11);
  a1 = 7; // strlen("PASSIVE")
label8001B330:
  v1 = lw(0x80075720); // &0x00000000
  v0 = 4; // 0x0004
  temp = v1 != v0;
  if (temp) goto label8001C53C;
  s2 = lw(gameobject_stack_ptr);
  temp = a1 == 0;
  s3 = 0;
  if (temp) goto label8001C53C;
  a2 = spyro_cos_lut;
  a0 = 0;
label8001B360:
  v0 = lw(0x800758B8);
  s3++;
  v0 = v0 << 3;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a2;
  v1 = lh(v0 + 0x00);
  a0 += 12; // 0x000C
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 9;
  sb(s2 + 0x46, v0);
  v0 = (int32_t)s3 < (int32_t)a1;
  temp = v0 != 0;
  s2 += 88; // 0x0058
  if (temp) goto label8001B360;
  goto label8001C53C;
label8001B3AC:
  s1 = sp + 0x70;
  sw(sp + 0x70, 0x10);
  sw(sp + 0x78, 0x1400);
  sw(sp + 0x40, 0xC7);
  sw(sp + 0x74, s6);
  sw(sp + 0x44, 0x6E);
  sw(sp + 0x48, 0x1100);
  create_3d_text1("CONTINUE", addr_to_pointer(s0), *(vec3_32 *)addr_to_pointer(s1), 18, 11);
  v0 = lw(0x80075720); // &0x00000000
  temp = v0 != 0;
  if (temp) goto label8001B418;
  s2 = lw(gameobject_stack_ptr);
  s5 = 8; // 0x0008
label8001B418:
  sw(sp + 0x40, 0xCF);
  sw(sp + 0x44, 0x80);
  sw(sp + 0x48, 0x1100);
  create_3d_text1("OPTIONS", addr_to_pointer(s0), *(vec3_32 *)addr_to_pointer(s1), 18, 11);
  v0 = lw(0x80075720); // &0x00000000
  temp = v0 != s6;
  if (temp) goto label8001B468;
  s2 = lw(gameobject_stack_ptr);
  s5 = 7; // 0x0007
label8001B468:
  a3 = 18; // 0x12
  s6 = 191; // 0xBF
  sw(sp + 0x40, 0xBF);
  sw(sp + 0x44, 0x92);
  sw(sp + 0x48, 0x1100);
  create_3d_text1("INVENTORY", addr_to_pointer(s0), *(vec3_32 *)addr_to_pointer(s1), 18, 11);
  v0 = lw(0x80075720); // &0x00000000
  temp = v0 != fp;
  if (temp) goto label8001B4B8;
  s2 = lw(gameobject_stack_ptr);
  s5 = 9; // 0x0009
label8001B4B8:
  v0 = lw(0x80075690); // &0x00000000
  temp = v0 == 0;
  if (temp) goto label8001B51C;
  sw(sp + 0x40, 0xE7);
  sw(sp + 0x44, 0xA4);
  sw(sp + 0x48, 0x1100);
  create_3d_text1("QUIT", addr_to_pointer(s0), *(vec3_32 *)addr_to_pointer(s1), 18, 11);
  v1 = lw(0x80075720); // &0x00000000
  v0 = 3; // 0x0003
  temp = v1 != v0;
  a1 = s5 & 0xFF;
  if (temp) goto label8001B5EC;
  s2 = lw(gameobject_stack_ptr);
  s5 = 4; // 0x0004
  goto label8001B5E8;
label8001B51C:
  v0 = 0x66660000;
  a0 = lw(LEVEL_ID);
  v0 = v0 | 0x6667;
  mult(a0, v0);
  v0 = (int32_t)a0 >> 31;
  t1=hi;
  v1 = (int32_t)t1 >> 2;
  v1 -= v0;
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 1;
  temp = a0 == v0;
  if (temp) goto label8001B5A0;
  sw(sp + 0x40, 0xB7);
  sw(sp + 0x44, 0xA4);
  sw(sp + 0x48, 0x1100);
  create_3d_text1("EXIT LEVEL", addr_to_pointer(s0), *(vec3_32 *)addr_to_pointer(s1), 18, 11);
  v1 = lw(0x80075720); // &0x00000000
  v0 = 3; // 0x0003
  temp = v1 != v0;
  a1 = s5 & 0xFF;
  if (temp) goto label8001B5EC;
  s2 = lw(gameobject_stack_ptr);
  s5 = 9; // 0x0009
  goto label8001B5E8;
label8001B5A0:
  sw(sp + 0x40, s6);
  sw(sp + 0x44, 0xA4);
  sw(sp + 0x48, 0x1100);
  create_3d_text1("QUIT GAME", addr_to_pointer(s0), *(vec3_32 *)addr_to_pointer(s1), 18, 11);
  v1 = lw(0x80075720); // &0x00000000
  v0 = 3; // 0x0003
  temp = v1 != v0;
  a1 = s5 & 0xFF;
  if (temp) goto label8001B5EC;
  s2 = lw(gameobject_stack_ptr);
  s5 = 8; // 0x0008
label8001B5E8:
  a1 = s5 & 0xFF;
label8001B5EC:
  temp = (int32_t)a1 <= 0;
  s3 = 0;
  if (temp) goto label8001C53C;
  a2 = spyro_cos_lut;
  a0 = 0;
label8001B600:
  v0 = lw(0x800758B8);
  s3++;
  v0 = v0 << 3;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a2;
  v1 = lh(v0 + 0x00);
  a0 += 12; // 0x000C
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 9;
  sb(s2 + 0x46, v0);
  v0 = (int32_t)s3 < (int32_t)a1;
  temp = v0 != 0;
  s2 += 88; // 0x0058
  if (temp) goto label8001B600;
  goto label8001C53C;
label8001B64C:
  temp = v1 != v0;
  if (temp) goto label8001C53C;
  ra = 0x8001B65C;
  v0 = completion_percentage();
  v1 = 440; // 0x01B8
  sw(sp + 0x40, v1);
  v1 = 44; // 0x002C
  sw(sp + 0x44, v1);
  v1 = 2304; // 0x0900
  a2 = v0;
  v0 = (int32_t)a2 < 10;
  temp = v0 != 0;
  sw(sp + 0x48, v1);
  if (temp) goto label8001B688;
  v0 = 406; // 0x0196
  sw(sp + 0x40, v0);
label8001B688:
  v0 = (int32_t)a2 < 100;
  temp = v0 != 0;
  s2 = sp + 32; // 0x0020
  if (temp) goto label8001B6A4;
  v0 = lw(sp + 0x40);
  v0 -= 34; // 0xFFFFFFDE
  sw(sp + 0x40, v0);
label8001B6A4:
  a0 = s2;
  s3 = 0x800755BC; // "%d"
  a1 = s3;
  spyro_sprintf();
  s4 = sp + 0x40;
  create_3d_text2(addr_to_pointer(s2), addr_to_pointer(s4), 34, 2);
  sw(sp + 0x40, 0x1DE);
  sb(sp + 0x20, 0x25);
  sb(sp + 0x21, 0);
  create_3d_text2(addr_to_pointer(s2), addr_to_pointer(s4), 34, 2);
  a0 = s2;
  a1 = s3;
  v0 = 90; // 0x005A
  s1 = 36; // 0x0024
  a2 = lw(total_found_gems);
  s0 = 2880; // 0x0B40
  sw(sp + 0x40, v0);
  sw(sp + 0x44, s1);
  sw(sp + 0x48, s0);
  ra = 0x8001B724;
  spyro_sprintf();
  create_3d_text2(addr_to_pointer(s2), addr_to_pointer(s4), 27, 2);
  a2 = lw(total_found_dragons);
  sw(sp + 0x40, 0x14D);
  sw(sp + 0x44, s1);
  v0 = (int32_t)a2 < 10;
  temp = v0 != 0;
  sw(sp + 0x48, s0);
  if (temp) goto label8001B760;
  v0 = 306; // 0x0132
  sw(sp + 0x40, v0);
label8001B760:
  a0 = s2;
  a1 = s3;
  ra = 0x8001B76C;
  spyro_sprintf();
  create_3d_text2(addr_to_pointer(s2), addr_to_pointer(s4), 27, 2);
  a1 = 0;
  a0 = lw(gameobject_stack_ptr);
  a0 -= 88; // 0xFFFFFFA8
  sw(gameobject_stack_ptr, a0);
  a2 = 88; // 0x0058
  ra = 0x8001B7A4;
  spyro_memset32(a0, a1, a2);
  v0 = 471; // 0x01D7
  v1 = lw(gameobject_stack_ptr);
  s1 = 46; // 0x002E
  sh(v1 + 0x36, v0);
  v0 = 3072; // 0x0C00
  sw(v1 + 0x0C, s1);
  sw(v1 + 0x10, s1);
  sw(v1 + 0x14, v0);
  v0 = lw(0x80077FE4);
  v0 = v0 << 1;
  v0 = lhu(spyro_cos_lut + v0);
  s5 = 0x80077FE4;
  v0 = v0 << 16;
  v0 = (int32_t)v0 >> 25;
  sb(v1 + 0x45, v0);
  v1 = lw(gameobject_stack_ptr);
  v0 = lw(s5 + 0x00);
  a1 = 0;
  sb(v1 + 0x46, v0);
  v0 = lw(gameobject_stack_ptr);
  s6 = 127; // 0x007F
  sb(v0 + 0x47, s6);
  v0 = lw(gameobject_stack_ptr);
  s3 = 11; // 0x000B
  sb(v0 + 0x4F, s3);
  v0 = lw(gameobject_stack_ptr);
  s2 = 255; // 0x00FF
  sb(v0 + 0x50, s2);
  a0 = lw(gameobject_stack_ptr);
  a0 -= 88; // 0xFFFFFFA8
  sw(gameobject_stack_ptr, a0);
  a2 = 88; // 0x0058
  ra = 0x8001B85C;
  spyro_memset32(a0, a1, a2);
  v1 = lw(gameobject_stack_ptr);
  v0 = 506; // 0x01FA
  sh(v1 + 0x36, v0);
  v0 = 300; // 0x012C
  sw(v1 + 0x0C, v0);
  v0 = lw(total_found_dragons);
  s0 = spyro_cos_lut;
  v0 = (int32_t)v0 < 10;
  temp = v0 != 0;
  v0 = 273; // 0x0111
  if (temp) goto label8001B894;
  sw(v1 + 0x0C, v0);
label8001B894:
  v1 = 1920; // 0x0780
  v0 = lw(0x800758B8);
  a0 = lw(gameobject_stack_ptr);
  v0 = v0 << 4;
  v0 = v0 & 0x1F0;
  v0 += s0;
  sw(a0 + 0x10, s1);
  sw(a0 + 0x14, v1);
  v0 = lhu(v0 + 0x00);
  v0 = v0 >> 8;
  sb(a0 + 0x46, v0);
  v0 = lw(gameobject_stack_ptr);
  sb(v0 + 0x47, s6);
  v0 = lw(gameobject_stack_ptr);
  sb(v0 + 0x4F, s3);
  v0 = lw(gameobject_stack_ptr);
  sb(v0 + 0x50, s2);
  v0 = lw(total_found_eggs);
  temp = (int32_t)v0 <= 0;
  s1 = 36; // 0x0024
  if (temp) goto label8001B9F8;
  s3 = 0;
  s2 = s4;
  s4 = 32; // 0x0020
  s6 = s5 + 4; // 0x0004
  s5 += 1328; // 0x0530
  s0 = 24; // 0x0018
label8001B928:
  v1 = lw(total_found_eggs);
  v0 = 50; // 0x0032
  sh(sp + 0x82, v0);
  v0 = 24; // 0x0018
  sh(sp + 0x84, v0);
  v0 = 16; // 0x0010
  sh(sp + 0x80, s0);
  v1 = (int32_t)s3 < (int32_t)v1;
  temp = v1 != 0;
  sh(sp + 0x86, v0);
  if (temp) goto label8001B9A0;
  sw(sp + 0x44, 0x3A);
  sw(sp + 0x40, s1);
  sw(sp + 0x48, 0x1800);
  create_3d_text2("X", addr_to_pointer(s2), 0, 2);
  a0 = sp + 128; // 0x0080
  a1 = 0x80078514;
  a2 = s2;
  sw(sp + 0x40, s4);
  sw(sp + 0x44, s4);
  sw(sp + 0x48, s4);
  goto label8001B9E0;
label8001B9A0:
  v0 = 0x38E30000;
  a1 = lw(s6 + 0x00);
  v0 = v0 | 0x8E39;
  a1 += s3;
  mult(a1, v0);
  a0 = sp + 128; // 0x0080
  a2 = 0;
  v0 = (int32_t)a1 >> 31;
  t1=hi;
  v1 = (int32_t)t1 >> 1;
  v1 -= v0;
  v0 = v1 << 3;
  v0 += v1;
  a1 -= v0;
  a1 = a1 << 3;
  a1 += s5;
label8001B9E0:
  s1 += 27; // 0x001B
  ra = 0x8001B9E8;
  function_8001919C();
  s3++;
  v0 = (int32_t)s3 < 12;
  temp = v0 != 0;
  s0 += 27; // 0x001B
  if (temp) goto label8001B928;
label8001B9F8:
  a0 = 64; // 0x0040
  a1 = 69; // 0x0045
  a2 = 448; // 0x01C0
  a3 = 69; // 0x0045
  ra = 0x8001BA0C;
  gui_line(a0, a1, a2, a3);
  a1 = 1; // 0x0001
  a2 = 0;
  s1 = lw(allocator1_ptr);
  a3 = 64; // 0x0040
  sw(sp + 0x10, 0);
  a0 = s1;
  ra = 0x8001BA2C;
  SetDrawMode(addr_to_pointer(a0), a1, a2, a3, addr_to_pointer(lw(sp + 0x10)));
  a0 = s1;
  ra = 0x8001BA34;
  append_gpu_command_block(addr_to_pointer(a0));
  a0 = s1 + 12; // 0x000C
  v0 = 0x05000000;
  sw(s1 + 0x0C, v0);
  v0 = 42; // 0x002A
  sb(s1 + 0x13, v0);
  v0 = 96; // 0x0060
  a1 = 64; // 0x0040
  sb(s1 + 0x10, v0);
  sb(s1 + 0x11, v0);
  sb(s1 + 0x12, v0);
  v0 = 70; // 0x0046
  v1 = 448; // 0x01C0
  sh(s1 + 0x16, v0);
  sh(s1 + 0x1A, v0);
  v0 = 71; // 0x0047
  sh(s1 + 0x14, a1);
  sh(s1 + 0x18, v1);
  sh(s1 + 0x1C, a1);
  sh(s1 + 0x1E, v0);
  sh(s1 + 0x20, v1);
  sh(s1 + 0x22, v0);
  ra = 0x8001BA8C;
  append_gpu_command_block(addr_to_pointer(a0));
  a0 = 64; // 0x0040
  a1 = 71; // 0x0047
  a2 = 448; // 0x01C0
  v0 = s1 + 36; // 0x0024
  sw(allocator1_ptr, v0);
  a3 = 71; // 0x0047
  ra = 0x8001BAAC;
  gui_line(a0, a1, a2, a3);
  v1 = lw(0x80075744);
  if ((int32_t)v1 > 0) {
    if (lbu(0x80078E72 + v1*6)) {
      blinking_arrow((vec3_32){0x28, 0x48, 0x1100}, lw(0x800758B8)*2, 1);
      v1 = lw(0x80075744);
    }
  }

  if ((int32_t)v1 < 5)
    if (lbu(0x80078E7E + v1*6))
      blinking_arrow((vec3_32){0x1D7, 0x48, 0x1100}, lw(0x800758B8)*2, 0);

  v0 = lw(0x80075744); // &0x00000000
  s3 = 0;
  v1 = v0 << 2;
  v0 = (int32_t)v0 < 3;
  a0 = lw(0x8006F7D4 + v1); // &0x8001006C
  s7 = v0 << 3;
  ra = 0x8001BB98;
  v0 = spyro_strlen(a0);
  a1 = sp + 64; // 0x0040
  a2 = sp + 144; // 0x0090
  v1 = 92; // 0x005C
  sw(sp + 0x44, v1);
  v1 = 3584; // 0x0E00
  sw(sp + 0x48, v1);
  v1 = 20; // 0x0014
  a3 = 2; // 0x0002
  sw(sp + 0x90, v1);
  v1 = 4096; // 0x1000
  v0--;
  sw(sp + 0x98, v1);
  v1 = v0 << 2;
  v1 += v0;
  v1 = v1 << 1;
  v0 = lw(0x800757CC); // &0x00000000
  v1 -= 158; // 0xFFFFFF62
  sw(sp + 0x94, a3);
  v0 -= v1;
  sw(sp + 0x40, v0);
  sw(sp + 0x10, a3);

  a0 = lw(0x8006F7D4 + lw(0x80075744)*4); // &0x8001006C
  create_3d_text1(addr_to_pointer(a0), addr_to_pointer(a1), *(vec3_32 *)addr_to_pointer(a2), 23, 2);
  v0 = lw(0x80075744); // &0x00000000
  a1 = 0x8007555C; // &0x0B0D0C08
  v0 = lbu(0x8007555C + v0); // &0x0B0D0C08
  s0 = lw(gameobject_stack_ptr);
  temp = (int32_t)v0 <= 0;
  a0 = 0;
  if (temp) goto label8001BCA0;
  a2 = spyro_cos_lut;
label8001BC44:
  v0 = lw(0x800758B8);
  v0 = v0 << 3;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a2;
  v1 = lh(v0 + 0x00);
  s3++;
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 9;
  sb(s0 + 0x46, v0);
  v0 = lw(0x80075744); // &0x00000000
  a0 += 12; // 0x000C
  v0 += a1;
  v0 = lbu(v0 + 0x00);
  v0 = (int32_t)s3 < (int32_t)v0;
  temp = v0 != 0;
  s0 += 88; // 0x0058
  if (temp) goto label8001BC44;
label8001BCA0:
  a1 = 0;
  a0 = lw(gameobject_stack_ptr);
  a0 -= 88; // 0xFFFFFFA8
  sw(gameobject_stack_ptr, a0);
  a2 = 88; // 0x0058
  ra = 0x8001BCC4;
  spyro_memset32(a0, a1, a2);
  a0 = lw(gameobject_stack_ptr);
  v0 = 471; // 0x01D7
  sh(a0 + 0x36, v0);
  v0 = lw(0x800757CC); // &0x00000000
  v1 = s7 + 346; // 0x015A
  v0 += v1;
  sw(a0 + 0x0C, v0);
  v0 = 108; // 0x006C
  sw(a0 + 0x10, v0);
  v0 = 3072; // 0x0C00
  sw(a0 + 0x14, v0);
  v0 = lw(0x80077FE4);
  s4 = sp + 64; // 0x0040
  v0 = v0 << 1;
  v0 = lhu(spyro_cos_lut + v0);
  s6 = sp + 32; // 0x0020
  v0 = v0 << 16;
  v0 = (int32_t)v0 >> 25;
  sb(a0 + 0x45, v0);
  v1 = lw(gameobject_stack_ptr);
  v0 = lw(0x80077FE4);
  a1 = 0;
  sb(v1 + 0x46, v0);
  v0 = lw(gameobject_stack_ptr);
  s0 = 127; // 0x007F
  sb(v0 + 0x47, s0);
  v0 = lw(gameobject_stack_ptr);
  s1 = 11; // 0x000B
  sb(v0 + 0x4F, s1);
  v0 = lw(gameobject_stack_ptr);
  s2 = 255; // 0x00FF
  sb(v0 + 0x50, s2);
  a0 = lw(gameobject_stack_ptr);
  a0 -= 88; // 0xFFFFFFA8
  sw(gameobject_stack_ptr, a0);
  a2 = 88; // 0x0058
  ra = 0x8001BD8C;
  spyro_memset32(a0, a1, a2);
  v0 = lw(0x800757CC); // &0x00000000
  a0 = lw(gameobject_stack_ptr);
  v0 += 465; // 0x01D1
  sw(a0 + 0x0C, v0);
  v0 = 110; // 0x006E
  sw(a0 + 0x10, v0);
  v0 = lw(0x800758B8);
  v1 = 506; // 0x01FA
  sh(a0 + 0x36, v1);
  v1 = 1920; // 0x0780
  sw(a0 + 0x14, v1);
  v0 = v0 << 4;
  v0 = v0 & 0x1F0;
  v0 = lhu(spyro_cos_lut + v0);
  v0 = v0 >> 8;
  sb(a0 + 0x46, v0);
  v0 = lw(gameobject_stack_ptr);
  sb(v0 + 0x47, s0);
  v0 = lw(gameobject_stack_ptr);
  s3 = 0;
  sb(v0 + 0x4F, s1);
  v0 = lw(gameobject_stack_ptr);
  s5 = 123; // 0x007B
  sb(v0 + 0x50, s2);
label8001BE14:
  v0 = lw(0x80075744); // &0x00000000
  v1 = v0 << 1;
  v1 += v0;
  v1 = v1 << 1;
  v1 += s3;
  v0 = lbu(0x80078E78 + v1);
  temp = v0 == 0;
  a2 = 0;
  if (temp) goto label8001C4D8;
  a1 = 1; // 0x0001
  s1 = lw(allocator1_ptr);
  a3 = 64; // 0x0040
  sw(sp + 0x10, 0);
  a0 = s1;
  ra = 0x8001BE64;
  SetDrawMode(addr_to_pointer(a0), a1, a2, a3, addr_to_pointer(lw(sp + 0x10)));
  a0 = s1;
  ra = 0x8001BE6C;
  append_gpu_command_block(addr_to_pointer(a0));
  a0 = s1 + 12; // 0x000C
  v0 = 0x05000000;
  sw(s1 + 0x0C, v0);
  v0 = 42; // 0x002A
  sb(s1 + 0x13, v0);
  v0 = 40; // 0x0028
  sb(s1 + 0x10, v0);
  sb(s1 + 0x11, v0);
  sb(s1 + 0x12, v0);
  v1 = lw(0x800757CC); // &0x00000000
  v0 = s5 - 10; // 0xFFFFFFF6
  sh(s1 + 0x16, v0);
  sh(s1 + 0x1A, v0);
  v0 = s5 + 9; // 0x0009
  sh(s1 + 0x1E, v0);
  sh(s1 + 0x22, v0);
  v0 = v1 + 511; // 0x01FF
  sh(s1 + 0x14, v1);
  sh(s1 + 0x18, v0);
  sh(s1 + 0x1C, v1);
  sh(s1 + 0x20, v0);
  ra = 0x8001BEC8;
  append_gpu_command_block(addr_to_pointer(a0));
  s0 = s5 - 11; // 0xFFFFFFF5
  a1 = s0;
  a3 = s0;
  a2 = lw(0x800757CC); // &0x00000000
  v0 = s1 + 36; // 0x0024
  sw(allocator1_ptr, v0);
  a0 = a2 + 112; // 0x0070
  a2 += 208; // 0x00D0
  ra = 0x8001BEF4;
  gui_line(a0, a1, a2, a3);
  v0 = lw(0x800757CC);
  gui_line(s7 + 0x13A + v0, s0, s7 + 0x13A + 0x40, s0);
  a1 = s0;
  a2 = lw(0x800757CC); // &0x00000000
  a3 = a1;
  a0 = a2 + 430; // 0x01AE
  a2 += 494; // 0x01EE
  gui_line(a0, a1, a2, a3);
  a1 = s4;
  a2 = sp + 144; // 0x0090
  t1 = 4352; // 0x1100
  v1 = lw(0x800757CC); // &0x00000000
  v0 = 16; // 0x0010
  sw(sp + 0x90, v0);
  v0 = 1; // 0x0001
  sw(sp + 0x94, v0);
  v0 = 5120; // 0x1400
  sw(sp + 0x98, v0);
  v1 += 36; // 0x0024
  sw(sp + 0x40, v1);
  v1 = lw(0x80075744); // &0x00000000
  v0 = 2; // 0x0002
  sw(sp + 0x44, s5);
  sw(sp + 0x48, t1);
  sw(sp + 0x10, v0);
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 1;
  v0 += s3;
  v0 = v0 << 2;
  a0 = lw(0x8006F7F0 + v0); // &0x80075578
  s0 = lw(gameobject_stack_ptr);
  a3 = 18; // 0x0012
  create_3d_text1(addr_to_pointer(a0), addr_to_pointer(s4), *(vec3_32 *)addr_to_pointer(sp + 0x90), 18, 2);
  a0 = s7 + 275; // 0x0113
  v0 = lw(0x800757CC); // &0x00000000
  v1 = lw(0x80075744); // &0x00000000
  a0 += v0;
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 1;
  v0 += s3;
  v1 = 0x80077420;
  v0 = v0 << 2;
  a2 = v0 + v1;
  sw(sp + 0x40, a0);
  v0 = lw(a2 + 0x00);
  v0 = (int32_t)v0 < 1000;
  temp = v0 == 0;
  v0 = a0 + 18; // 0x0012
  if (temp) goto label8001C004;
  sw(sp + 0x40, v0);
label8001C004:
  v0 = lw(a2 + 0x00);
  v0 = (int32_t)v0 < 100;
  temp = v0 == 0;
  if (temp) goto label8001C028;
  v0 = lw(sp + 0x40);
  v0 += 18; // 0x0012
  sw(sp + 0x40, v0);
label8001C028:
  v0 = lw(a2 + 0x00);
  v0 = (int32_t)v0 < 10;
  temp = v0 == 0;
  if (temp) goto label8001C04C;
  v0 = lw(sp + 0x40);
  v0 += 18; // 0x0012
  sw(sp + 0x40, v0);
label8001C04C:
  a2 = lw(a2 + 0x00);
  a1 = 0x800755BC; // "%d"
  a0 = s6;
  spyro_sprintf();
  create_3d_text2(addr_to_pointer(s6), addr_to_pointer(s4), 18, 2);
  v0 = lw(0x800757CC); // &0x00000000
  v1 = s7 + 346; // 0x015A
  sw(sp + 0x48, 0x1800);
  v0 += v1;
  sw(sp + 0x40, v0);
  create_3d_text2("/", addr_to_pointer(s4), 18, 2);
  t1 = 4352; // 0x1100
  v0 = lw(0x800757CC); // &0x00000000
  v1 = lw(0x80075744); // &0x00000000
  a1 = s7 + 363; // 0x016B
  sw(sp + 0x48, t1);
  v0 += a1;
  sw(sp + 0x40, v0);
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 1;
  v0 += s3;
  v0 = v0 << 1;
  a2 = lh(0x8006F438 + v0); // &0x00C80064
  a1 = 0x800755BC; // "%d"
  a0 = s6;
  spyro_sprintf();
  create_3d_text2(addr_to_pointer(s6), addr_to_pointer(s4), 18, 2);
  v1 = lw(0x80075744); // &0x00000000
  t1 = 0x8006F414; // &0x04030404
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 1;
  v1 = v0 + s3;
  v0 = v1 + t1;
  v0 = lbu(v0 + 0x00);
  temp = v0 == 0;
  a1 = s4;
  if (temp) goto label8001C228;
  v0 = lw(0x800757CC); // &0x00000000
  v0 += 445; // 0x01BD
  sw(sp + 0x40, v0);
  v0 = v1 << 2;
  a2 = lw(0x800772D8 + v0);
  a1 = 0x800755BC; // "%d"
  a0 = s6;
  spyro_sprintf();
  create_3d_text2(addr_to_pointer(s6), addr_to_pointer(s4), 18, 2);
  v0 = lw(0x800757CC);
  sw(sp + 0x48, 0x1800);
  v0 += 462; // 0x01CE
  sw(sp + 0x40, v0);
  create_3d_text2("/", addr_to_pointer(s4), 18, 2);
  t1 = 4352; // 0x1100
  sw(sp + 0x48, t1);
  t1 = 0x8006F414; // &0x04030404
  v0 = lw(0x800757CC);
  v1 = lw(0x80075744);
  v0 += 479; // 0x01DF
  sw(sp + 0x40, v0);
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 1;
  v0 += s3;
  v0 += t1;
  a2 = lbu(v0 + 0x00);
  a1 = 0x800755BC; // "%d"
  a0 = s6;
  spyro_sprintf();
  create_3d_text2(addr_to_pointer(s6), addr_to_pointer(s4), 18, 2);
  goto label8001C264;
label8001C228:
  a0 = 0x8007561C; // &0x0000002F
  a2 = 18; // 0x0012
  a3 = 2; // 0x0002
  v0 = lw(0x800757CC); // &0x00000000
  t1 = 6144; // 0x1800
  sw(sp + 0x48, t1);
  v0 += 462; // 0x01CE
  sw(sp + 0x40, v0);
  create_3d_text2(addr_to_pointer(a0), addr_to_pointer(a1), a2, a3);
  v1 = lw(gameobject_stack_ptr);
  v0 = 41; // 0x0029
  sb(v1 + 0x44, v0);
label8001C264:
  v0 = lw(0x80075744); // &0x00000000
  v1 = v0 << 1;
  v1 += v0;
  v1 = v1 << 1;
  v0 = lw(CONTINUOUS_LEVEL_ID);
  v1 += s3;
  temp = v1 != v0;
  if (temp) goto label8001C304;
  v0 = lw(gameobject_stack_ptr);
  s0 -= 88; // 0xFFFFFFA8
  v0 = (int32_t)s0 < (int32_t)v0;
  temp = v0 != 0;
  if (temp) goto label8001C304;
  a1 = spyro_cos_lut;
  a0 = 0;
label8001C2B4:
  v0 = lw(0x800758B8);
  v0 = v0 << 3;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a1;
  v1 = lh(v0 + 0x00);
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 9;
  sb(s0 + 0x46, v0);
  v0 = lw(gameobject_stack_ptr);
  s0 -= 88; // 0xFFFFFFA8
  v0 = (int32_t)s0 < (int32_t)v0;
  temp = v0 == 0;
  a0 += 12; // 0x000C
  if (temp) goto label8001C2B4;
label8001C304:
  v0 = lw(total_found_eggs);
  temp = (int32_t)v0 <= 0;
  if (temp) goto label8001C4D4;
  v1 = lw(0x80075744); // &0x00000000
  v0 = (int32_t)v1 < 3;
  temp = v0 == 0;
  v0 = v1 << 1;
  if (temp) goto label8001C4D4;
  a1 = 0x8006F480; // &0x01000100
  v0 += v1;
  v0 = v0 << 1;
  v0 += s3;
  v0 = lbu(0x8006F480 + v0); // &0x01000100
  temp = (int32_t)v0 <= 0;
  s0 = 0;
  if (temp) goto label8001C4D4;
  s2 = 0x80077FE8;
  fp = s2 + 1324; // 0x052C
  s1 = 0;
label8001C36C:
  v0 = lw(0x800757CC); // &0x00000000
  v0 += 232; // 0x00E8
  a0 = v0 + s1;
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 1;
  v0 += s3;
  v0 += a1;
  v1 = lbu(v0 + 0x00);
  v0 = 1; // 0x0001
  temp = v1 != v0;
  t0 = a0;
  if (temp) goto label8001C3A8;
  t0 = a0 + 24; // 0x0018
label8001C3A8:
  v1 = lw(0x80075744); // &0x00000000
  v0 = s5 - 8; // 0xFFFFFFF8
  sh(sp + 0x8A, v0);
  v0 = 24; // 0x0018
  sh(sp + 0x8C, v0);
  v0 = 16; // 0x0010
  sh(sp + 0x88, t0);
  sh(sp + 0x8E, v0);
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 1;
  v0 += s3;
  v0 = v0 << 2;
  v0 = lw(0x80076FE8 + v0);
  v0 = (int32_t)s0 < (int32_t)v0;
  temp = v0 != 0;
  v0 = 0x38E30000;
  if (temp) goto label8001C454;
  v0 = t0 << 16;
  v0 = (int32_t)v0 >> 16;
  v0 += 12; // 0x000C
  t1 = 6144; // 0x1800
  sw(sp + 0x40, v0);
  sw(sp + 0x44, s5);
  sw(sp + 0x48, t1);
  create_3d_text2("X", addr_to_pointer(s4), 0, 2);
  a0 = sp + 136; // 0x0088
  a1 = 0x80078514;
  a2 = s4;
  v0 = 32; // 0x0020
  sw(sp + 0x40, v0);
  sw(sp + 0x44, v0);
  sw(sp + 0x48, v0);
  goto label8001C490;
label8001C454:
  a1 = lw(s2 + 0x00);
  v0 = v0 | 0x8E39;
  a1 += s3;
  mult(a1, v0);
  a0 = sp + 136; // 0x0088
  a2 = 0;
  v0 = (int32_t)a1 >> 31;
  t1=hi;
  v1 = (int32_t)t1 >> 1;
  v1 -= v0;
  v0 = v1 << 3;
  v0 += v1;
  a1 -= v0;
  a1 = a1 << 3;
  a1 += fp;
label8001C490:
  s1 += 24; // 0x0018
  ra = 0x8001C498;
  function_8001919C();
  v1 = lw(0x80075744); // &0x00000000
  s0++;
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 1;
  v0 += s3;
  v0 = lbu(0x8006F480 + v0); // &0x01000100
  a1 = 0x8006F480; // &0x01000100
  v0 = (int32_t)s0 < (int32_t)v0;
  temp = v0 != 0;
  if (temp) goto label8001C36C;
label8001C4D4:
  s5 += 19; // 0x0013
label8001C4D8:
  s3++;
  v0 = (int32_t)s3 < 6;
  temp = v0 != 0;
  s0 = s5 - 11; // 0xFFFFFFF5
  if (temp) goto label8001BE14;
  a1 = s0;
  a2 = lw(0x800757CC); // &0x00000000
  a3 = s0;
  a0 = a2 + 112; // 0x0070
  a2 += 208; // 0x00D0
  ra = 0x8001C504;
  gui_line(a0, a1, a2, a3);
  a1 = s0;
  a2 = lw(0x800757CC); // &0x00000000
  a3 = s0;
  a0 = a2 + 320; // 0x0140
  a2 += 384; // 0x0180
  ra = 0x8001C520;
  gui_line(a0, a1, a2, a3);
  a1 = s0;
  a2 = lw(0x800757CC); // &0x00000000
  a3 = a1;
  a0 = a2 + 430; // 0x01AE
  a2 += 494; // 0x01EE
  ra = 0x8001C53C;
  gui_line(a0, a1, a2, a3);
label8001C53C:
  v1 = lw(0x800757D8); // &0x00000000
  v0 = 2; // 0x0002
  sw(0x8006FCF4 + 0x2400, 0); // "H^^^oooooofffOOO((("
  temp = v1 != v0;
  if (temp) goto label8001C574;
  v0 = lw(0x80075690); // &0x00000000
  temp = v0 != 0;
  if (temp) goto label8001C574;
  ra = 0x8001C574;
  function_80019300();
label8001C574:
  ra = 0x8001C57C;
  function_80018880();
  spyro_memset32(0x8006FCF4, 0, 0x900);
  ra = 0x8001C598;
  function_80022A2C();
  DrawOTag(spyro_combine_all_command_buffers(0));

  wait_two_vsyncs();
  
  PutDispEnv(&backbuffer_disp->disp);
label8001C648:
  v0 = lw(0x800758B8);
  v0++;
  sw(0x800758B8, v0);
  ra = lw(sp + 0x014C);
  fp = lw(sp + 0x0148);
  s7 = lw(sp + 0x0144);
  s6 = lw(sp + 0x0140);
  s5 = lw(sp + 0x013C);
  s4 = lw(sp + 0x0138);
  s3 = lw(sp + 0x0134);
  s2 = lw(sp + 0x0130);
  s1 = lw(sp + 0x012C);
  s0 = lw(sp + 0x0128);
  sp += 336; // 0x0150
}

// size: 0x000003A4
void function_8001C694(void)
{
  uint32_t temp;
  sp -= 48; // 0xFFFFFFD0
  v1 = 0x00010000;
  v0 = lw(0x800785E8);
  a0 = lw(0x800758B8);
  v1 = v1 | 0xC000;
  sw(sp + 0x2C, ra);
  sw(sp + 0x28, s2);
  sw(sp + 0x24, s1);
  sw(sp + 0x20, s0);
  sw(0x800758B0, 0);
  sw(allocator1_ptr, v0);
  v0 += v1;
  sw(allocator1_end, v0);
  temp = a0 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label8001C6F4;
  temp = a0 == v0;
  a2 = 32; // 0x0020
  if (temp) goto label8001C8A4;
  goto label8001C950;
label8001C6F4:
  if (backbuffer_disp == addr_to_pointer(DISP1))
    a0 = DISP2;
  else
    a0 = DISP1;
  s1 = 0;
  PutDrawEnv(addr_to_pointer(a0));
  v1 = lw(0x8007568C);
  LoadImage(&(RECT){v1 + 0x200, 0xE0, 0x20 - v1, 1}, addr_to_pointer(0x8006F310));
  DrawSync(0);
label8001C760:
  v1 = s1 << 7;
  t0 = s1 + 136; // 0x0088
  s0 = lw(allocator1_ptr);
  v0 = 0x09000000;
  sw(s0 + 0x00, v0);
  v0 = 44; // 0x002C
  sb(s0 + 0x07, v0);
  v0 = 76; // 0x004C
  sb(s0 + 0x04, v0);
  v0 = 128; // 0x0080
  sb(s0 + 0x05, v0);
  v0 = 8; // 0x0008
  sh(s0 + 0x08, v1);
  sh(s0 + 0x0A, v0);
  v0 = lhu(s0 + 0x08);
  a2 = lhu(s0 + 0x0A);
  a3 = lhu(s0 + 0x08);
  v1 = lhu(s0 + 0x0A);
  v0 += 128; // 0x0080
  sh(s0 + 0x10, v0);
  v0 = lhu(s0 + 0x08);
  v1 += 223; // 0x00DF
  sh(s0 + 0x1A, v1);
  v1 = lhu(s0 + 0x0A);
  s1++;
  sb(s0 + 0x0D, 0);
  sh(s0 + 0x12, a2);
  a2 = lbu(s0 + 0x0D);
  s2 = 64; // 0x0040
  sb(s0 + 0x0C, 0);
  sh(s0 + 0x18, a3);
  a3 = lbu(s0 + 0x0C);
  v0 += 128; // 0x0080
  sh(s0 + 0x20, v0);
  v0 = lbu(s0 + 0x0C);
  v1 += 223; // 0x00DF
  sh(s0 + 0x22, v1);
  v1 = lbu(s0 + 0x0D);
  a0 = s0;
  sb(s0 + 0x06, s2);
  sb(s0 + 0x15, a2);
  sb(s0 + 0x1C, a3);
  v0 += 128; // 0x0080
  sb(s0 + 0x14, v0);
  v0 = lbu(s0 + 0x0C);
  v1 -= 33; // 0xFFFFFFDF
  sb(s0 + 0x1D, v1);
  v1 = lbu(s0 + 0x0D);
  v0 += 128; // 0x0080
  v1 -= 33; // 0xFFFFFFDF
  sb(s0 + 0x24, v0);
  v0 = 14368; // 0x3820
  sb(s0 + 0x25, v1);
  sh(s0 + 0x0E, v0);
  sh(s0 + 0x16, t0);
  ra = 0x8001C844;
  append_gpu_command_block_depth_slot(addr_to_pointer(a0), 0x3FF);
  a0 = s0 + 40; // 0x0028
  v0 = (int32_t)s1 < 4;
  sw(allocator1_ptr, a0);
  temp = v0 != 0;
  if (temp) goto label8001C760;
  v0 = 0x03000000;
  v1 = 231; // 0x00E7
  sw(s0 + 0x28, v0);
  v0 = 512; // 0x0200
  sb(s0 + 0x2F, s2);
  sh(s0 + 0x30, 0);
  sh(s0 + 0x32, v1);
  sh(s0 + 0x34, v0);
  sh(s0 + 0x36, v1);
  sb(s0 + 0x2C, 0);
  sb(s0 + 0x2D, 0);
  sb(s0 + 0x2E, 0);
  ra = 0x8001C890;
  append_gpu_command_block_depth_slot(addr_to_pointer(a0), 0x3FF);
  v0 = s0 + 56; // 0x0038
  sw(allocator1_ptr, v0);
  goto label8001C950;
label8001C8A4:
  a0 = lw(SKYBOX_DATA + 0x10);
  v0 = lw(0x8007568C); // &0x00000000
  sw(0x800757D4, 0); // &0x00000000
  a2 -= v0;
  a2 = a2 << 7;
  sw(0x8007575C, a2); // &0x00000000
  a1 = 0;
  ra = 0x8001C8D4;
  v0 = interpolate_color(a0, a1, a2);
  sw(sp + 0x18, v0);
  if (backbuffer_disp == addr_to_pointer(DISP1))
  {
    a0 = DISP2;
    v0 = lbu(sp + 0x18);
    v1 = lbu(sp + 0x19);
    a1 = lbu(sp + 0x1A);
    sb(0x80076F7D, v0);
    sb(0x80076F7E, v1);
    sb(0x80076F7F, a1);
  } else {
    a0 = DISP1;
    v0 = lbu(sp + 0x18);
    v1 = lbu(sp + 0x19);
    a1 = lbu(sp + 0x1A);
    sb(0x80076EF9, v0);
    sb(0x80076EFA, v1);
    sb(0x80076EFB, a1);
  }
  PutDrawEnv(addr_to_pointer(a0));
  ra = 0x8001C950;
  function_8004F000();
label8001C950:
  spyro_memset32(0x8006FCF4, 0, 0x900);
  ra = 0x8001C96C;
  function_80023AC4();
  DrawOTag(spyro_combine_all_command_buffers(0x800));

  wait_two_vsyncs();

  PutDispEnv(&backbuffer_disp->disp);
  ra = lw(sp + 0x2C);
  s2 = lw(sp + 0x28);
  s1 = lw(sp + 0x24);
  s0 = lw(sp + 0x20);
  sp += 48; // 0x0030
}

// size: 0x000005A4
void function_8001CA38(void)
{
  uint32_t temp;
  v0 = lw(0x80075940);
  sp -= 88; // 0xFFFFFFA8
  sw(sp + 0x50, ra);
  sw(sp + 0x4C, s3);
  sw(sp + 0x48, s2);
  sw(sp + 0x44, s1);
  temp = v0 != 0;
  sw(sp + 0x40, s0);
  if (temp) goto label8001CB10;
  v1 = lw(0x8007593C);
  temp = v1 != 0;
  v0 = (int32_t)v1 < 16;
  if (temp) goto label8001CAF0;
  ra = 0x8001CA78;
  function_800521C0();
  ra = 0x8001CA80;
  function_80019698();
  ra = 0x8001CA88;
  function_800573C8();
  ra = 0x8001CA90;
  function_80050BD0();
  ra = 0x8001CA98;
  function_8002B9CC();
  DrawSync(0);
  VSync(0);
  
  PutDispEnv(&backbuffer_disp->disp);
  PutDrawEnv(&backbuffer_disp->draw);
  DrawOTag(spyro_combine_all_command_buffers(0x800));

  DrawSync(0);
  VSync(0);
  goto label8001CE58;
label8001CAF0:
  temp = v0 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label8001CFBC;
  temp = v1 != v0;
  a1 = 16; // 0x0010
  if (temp) goto label8001CEDC;
  a0 = 2; // 0x0002
  a2 = 16; // 0x0010
  a3 = 16; // 0x0010
  goto label8001CEEC;
label8001CB10:
  a0 = 1; // 0x0001
  temp = v0 != a0;
  if (temp) goto label8001CE40;
  v0 = lw(0x8007593C);
  v0 = (int32_t)v0 < 60;
  temp = v0 != 0;
  if (temp) goto label8001CD74;
  s1 = 0;
  s3 = 0x8006F2D4; // &0x0B800C00
  s2 = 0;
label8001CB44:
  v0 = lbu(0x800752E0 + s1); // &0x18100800
  v1 = lw(0x8007593C);
  v0 += 60; // 0x003C
  s0 = v1 - v0;
  temp = (int32_t)s0 < 0;
  a1 = 0;
  if (temp) goto label8001CC44;
  a0 = lw(gameobject_stack_ptr);
  a0 -= 88; // 0xFFFFFFA8
  sw(gameobject_stack_ptr, a0);
  a2 = 88; // 0x0058
  ra = 0x8001CB88;
  spyro_memset32(a0, a1, a2);
  v0 = (int32_t)s0 < 24;
  temp = v0 == 0;
  v0 = s0 << 1;
  if (temp) goto label8001CBAC;
  v0 += s3;
  v1 = lw(gameobject_stack_ptr);
  v0 = lh(v0 + 0x00);
  sw(v1 + 0x14, v0);
  goto label8001CBBC;
label8001CBAC:
  v1 = lw(gameobject_stack_ptr);
  v0 = 1024; // 0x0400
  sw(v1 + 0x14, v0);
label8001CBBC:
  v0 = lbu(0x8006F304 + s1); // "GAMEOVER"
  v1 = lw(gameobject_stack_ptr);
  v0 += 361; // 0x0169
  sh(v1 + 0x36, v0);
  v0 = lh(0x8006F2B4 + s2); // &0x090022CE
  sw(v1 + 0x0C, v0);
  v0 = lh(0x8006F2B6 + s2); // &0x23C20900
  sw(v1 + 0x10, v0);
  v0 = lbu(0x800752D0 + s1); // &0x4E55606B
  sb(v1 + 0x46, v0);
  v1 = lw(gameobject_stack_ptr);
  v0 = 4; // 0x0004
  sb(v1 + 0x47, v0);
  v1 = lw(gameobject_stack_ptr);
  v0 = 11; // 0x000B
  sb(v1 + 0x4F, v0);
  v1 = lw(gameobject_stack_ptr);
  v0 = 32; // 0x0020
  sb(v1 + 0x50, v0);
label8001CC44:
  s1++;
  v0 = (int32_t)s1 < 8;
  temp = v0 != 0;
  s2 += 4; // 0x0004
  if (temp) goto label8001CB44;
  v0 = lw(0x80075864);
  v0 = (int32_t)v0 < 11;
  temp = v0 != 0;
  if (temp) goto label8001CD2C;
  v0 = lw(0x8007593C);
  v0 = (int32_t)v0 < 421;
  temp = v0 != 0;
  if (temp) goto label8001CD2C;
  create_3d_text1("PRESS START", &(vec3_32){0xAE, 0xD0, 0x1100}, (vec3_32){0x10, 1, 0x1400}, 18, 11);
  s1 = 0;
  a2 = spyro_cos_lut;
  a1 = 0;
  a0 = lw(gameobject_stack_ptr);
label8001CCE8:
  s1++;
  sb(a0 + 0x46, cos_lut[(lw(0x8007593C)*4 + a1) & 0xFF]*3/512);
  a1 += 12;
  a0 += 88; // 0x0058
  if ((int32_t)s1 < 10) goto label8001CCE8;
label8001CD2C:
  s0 = 0x8006FCF4 + 0x2400; // "H^^^oooooofffOOO((("
  sw(s0 + 0x00, 0);
  ra = 0x8001CD3C;
  function_80018880();
  a0 = s0 - 9216; // 0xFFFFDC00
  a1 = 0;
  a2 = 2304; // 0x0900
  ra = 0x8001CD4C;
  spyro_memset32(a0, a1, a2);
  ra = 0x8001CD54;
  function_80022A2C();
  v0 = lw(0x8007593C);
  v0 = (int32_t)v0 < 181;
  temp = v0 != 0;
  if (temp) goto label8001CD74;
  ra = 0x8001CD74;
  function_80023AC4();
label8001CD74:
  v0 = lhu(0x800752DC); // &0x08840F50
  v1 = lhu(0x800752DE); // &0x08000884
  s0 = 0x80076E1E;
  sh(s0 + 0x00, v0);
  sh(0x80076E20, v1);
  ra = 0x8001CDA0;
  function_80033C50();
  draw_skybox(-1, s0 - 58, s0 - 78);
  v0 = lhu(0x800752D8); // &0x040000B0
  v1 = lhu(0x800752DA); // &0x0F500400
  a2 = lw(0x8007593C);
  sh(s0 + 0x00, v0);
  v0 = (int32_t)a2 < 32;
  sh(0x80076E20, v1);
  temp = v0 == 0;
  a0 = 2; // 0x0002
  if (temp) goto label8001CDF8;
  a1 = 31; // 0x001F
  a1 -= a2;
  a1 = a1 << 3;
  a2 = a1;
  a3 = a1;
  ra = 0x8001CDF8;
  function_800190D4();
label8001CDF8:
  a0 = 0;
  ra = 0x8001CE00;
  v0 = DrawSync(a0);
  a0 = 0;
  ra = 0x8001CE08;
  v0 = VSync(a0);
  
  PutDispEnv(&backbuffer_disp->disp);
  PutDrawEnv(&backbuffer_disp->draw);
  DrawOTag(spyro_combine_all_command_buffers(0x800));

  goto label8001CFBC;
label8001CE40:
  v1 = lw(0x8007593C);
  temp = v1 != 0;
  v0 = (int32_t)v1 < 16;
  if (temp) goto label8001CEBC;
label8001CE58:
  if (backbuffer_disp != addr_to_pointer(DISP1))
    a3 = 0xF8;
  else
    a3 = 0x08;
  a0 = sp + 24; // 0x0018
  a1 = 0;
  a2 = 256; // 0x0100
  a2 -= a3;
  sh(sp + 0x18, 0);
  sh(sp + 0x1C, 0x200);
  sh(sp + 0x1A, a3);
  sh(sp + 0x1E, 0xE0);
  ra = 0x8001CE9C;
  v0 = MoveImage(addr_to_pointer(a0), a1, a2);
  a0 = 0;
  ra = 0x8001CEA4;
  v0 = DrawSync(a0);
  sb(0x80076EF8, 0);
  sb(0x80076F7C, 0);
  goto label8001CFBC;
label8001CEBC:
  temp = v0 == 0;
  if (temp) goto label8001CFBC;
  temp = v1 != a0;
  a1 = 16; // 0x0010
  if (temp) goto label8001CEDC;
  a0 = 2; // 0x0002
  a2 = 16; // 0x0010
  a3 = 16; // 0x0010
  goto label8001CEEC;
label8001CEDC:
  a0 = 2; // 0x0002
  a1 = 32; // 0x0020
  a2 = 32; // 0x0020
  a3 = 32; // 0x0020
label8001CEEC:
  ra = 0x8001CEF4;
  function_800190D4();
  DrawSync(0);
  VSync(0);
  
  PutDispEnv(&backbuffer_disp->disp);
  PutDrawEnv(&backbuffer_disp->draw);
  DrawOTag(spyro_combine_all_command_buffers(0x800));

  v1 = lw(0x8007593C);
  v0 = 15; // 0x000F
  temp = v1 != v0;
  a0 = 0;
  if (temp) goto label8001CFBC;
  v1 = 0x80076EF8;
  sb(v1, 1);
  sb(0x80076F7C, 1);
  sb(0x80076EF9, 0);
  sb(0x80076EFA, 0);
  sb(0x80076EFB, 0);
  sb(v1 + 0x85, 0);
  sb(v1 + 0x86, 0);
  sb(v1 + 0x87, 0);
  DrawSync(0);
  VSync(0);
  a0 = sp + 24; // 0x0018
  a1 = 0;
  a2 = 0;
  a3 = 0;
  v0 = 512; // 0x0200
  sh(sp + 0x1C, v0);
  v0 = 480; // 0x01E0
  sh(sp + 0x18, 0);
  sh(sp + 0x1A, 0);
  sh(sp + 0x1E, v0);
  ra = 0x8001CFBC;
  v0 = ClearImage(addr_to_pointer(a0), a1, a2, a3);
label8001CFBC:
  ra = lw(sp + 0x50);
  s3 = lw(sp + 0x4C);
  s2 = lw(sp + 0x48);
  s1 = lw(sp + 0x44);
  s0 = lw(sp + 0x40);
  sp += 88; // 0x0058
}

// size: 0x0000073C
void function_8001CFDC(void)
{
  uint32_t temp;
  sp -= 40; // 0xFFFFFFD8
  a1 = 0x80076248;
  sw(sp + 0x24, ra);
  sw(sp + 0x20, s0);
  v0 = lw(a1 + 0x00);
  temp = v0 == 0;
  a0 = a1 + 40; // 0x0028
  if (temp) goto label8001D018;
  a2 = 0x80076DD0;
  a1 += 16; // 0x0010
  ra = 0x8001D010;
  spyro_mat3_rotation(a0, a1, a2);
  ra = 0x8001D018;
  function_80058864();
label8001D018:
  a0 = lw(0x80077058);
  temp = a0 != 0;
  v0 = (int32_t)a0 < 4;
  if (temp) goto label8001D07C;
  ra = 0x8001D034;
  function_800521C0();
  ra = 0x8001D03C;
  function_80019698();
  ra = 0x8001D044;
  function_8002B9CC();
  ra = 0x8001D04C;
  function_80050BD0();
  ra = 0x8001D054;
  function_800573C8();
  a1 = lw(0x80077074);
  temp = a1 == 0;
  a0 = 1; // 0x0001
  if (temp) goto label8001D4D8;
  a2 = a1;
  a3 = a1;
  ra = 0x8001D074;
  function_800190D4();
  goto label8001D4D8;
label8001D07C:
  temp = v0 == 0;
  v0 = 4; // 0x0004
  if (temp) goto label8001D1C8;
  a1 = lw(0x800770C0);
  v0 = lbu(a1 + 0x48);
  v1 = 0x8006FCF4; // &0x000EA69B
  v0 = v0 < 128;
  temp = v0 == 0;
  v0 = 3; // 0x0003
  if (temp) goto label8001D0B0;
  sw(v1 + 0x00, a1);
  v1 += 4; // 0x0004
label8001D0B0:
  temp = a0 != v0;
  if (temp) goto label8001D0CC;
  v0 = lw(0x800770B8);
  sw(v1 + 0x00, v0);
  v1 += 4; // 0x0004
label8001D0CC:
  sw(v1 + 0x00, 0);
  a1 = lw(0x80075828);
  a2 = 0x8006FCF4 + 0x2400; // "H^^^oooooofffOOO((("
  a0 = lbu(a1 + 0x48);
  v0 = 255; // 0x00FF
  v1 = a0 & 0xFF;
  temp = v1 == v0;
  t0 = 251; // 0x00FB
  if (temp) goto label8001D134;
  a3 = 255; // 0x00FF
  v1 = a1 + 72; // 0x0048
label8001D0FC:
  v0 = lh(v1 - 0x0012); // 0xFFFFFFEE
  temp = v0 != t0;
  v0 = a0 & 0xFF;
  if (temp) goto label8001D120;
  v0 = v0 < 128;
  temp = v0 == 0;
  if (temp) goto label8001D120;
  sw(a2 + 0x00, a1);
  a2 += 4; // 0x0004
label8001D120:
  v1 += 88; // 0x0058
  a0 = lbu(v1 + 0x00);
  temp = a0 != a3;
  a1 += 88; // 0x0058
  if (temp) goto label8001D0FC;
label8001D134:
  sw(a2 + 0x00, 0);
  ra = 0x8001D13C;
  function_80018728();
  ra = 0x8001D144;
  function_80018880();
  ra = 0x8001D14C;
  function_8001F158();
  a0 = 0x8006FCF4; // &0x000EA69B
  a1 = 0;
  a2 = 2304; // 0x0900
  ra = 0x8001D160;
  spyro_memset32(a0, a1, a2);
  ra = 0x8001D168;
  function_8001F798();
  ra = 0x8001D170;
  function_80022A2C();
  ra = 0x8001D178;
  function_80059F8C();
  ra = 0x8001D180;
  function_80023AC4();
  ra = 0x8001D188;
  function_80059A48();
  ra = 0x8001D190;
  function_8002B9CC();
  ra = 0x8001D198;
  function_80050BD0();
  ra = 0x8001D1A0;
  function_800573C8();
  a1 = lw(0x80077074);
  temp = a1 == 0;
  a0 = 1; // 0x0001
  if (temp) goto label8001D4D8;
  a2 = a1;
  a3 = a1;
  ra = 0x8001D1C0;
  function_800190D4();
  goto label8001D4D8;
label8001D1C8:
  temp = a0 != v0;
  v0 = 5; // 0x0005
  if (temp) goto label8001D2C8;
  v1 = lw(0x800770B8);
  v0 = 0x8006FCF4; // &0x000EA69B
  sw(v0 + 0x00, v1);
  v1 = lw(0x800770BC);
  v0 += 4; // 0x0004
  sw(v0 + 0x00, v1);
  v0 += 4; // 0x0004
  sw(v0 + 0x00, 0);
  a1 = lw(0x80075828);
  a2 = v0 + 9208; // 0x23F8
  a0 = lbu(a1 + 0x48);
  v0 = 255; // 0x00FF
  v1 = a0 & 0xFF;
  temp = v1 == v0;
  if (temp) goto label8001D260;
  t0 = 251; // 0x00FB
  a3 = 255; // 0x00FF
  v1 = a1 + 72; // 0x0048
label8001D228:
  v0 = lh(v1 - 0x0012); // 0xFFFFFFEE
  temp = v0 != t0;
  v0 = a0 & 0xFF;
  if (temp) goto label8001D24C;
  v0 = v0 < 128;
  temp = v0 == 0;
  if (temp) goto label8001D24C;
  sw(a2 + 0x00, a1);
  a2 += 4; // 0x0004
label8001D24C:
  v1 += 88; // 0x0058
  a0 = lbu(v1 + 0x00);
  temp = a0 != a3;
  a1 += 88; // 0x0058
  if (temp) goto label8001D228;
label8001D260:
  sw(a2 + 0x00, 0);
  v0 = lw(0x8007706C);
  v0 = (int32_t)v0 < 60;
  temp = v0 == 0;
  if (temp) goto label8001D28C;
  ra = 0x8001D284;
  function_80018728();
  ra = 0x8001D28C;
  function_80018880();
label8001D28C:
  ra = 0x8001D294;
  function_8001F158();
  a0 = 0x8006FCF4; // &0x000EA69B
  a1 = 0;
  a2 = 2304; // 0x0900
  ra = 0x8001D2A8;
  spyro_memset32(a0, a1, a2);
  ra = 0x8001D2B0;
  function_8001F798();
  ra = 0x8001D2B8;
  function_80022A2C();
  ra = 0x8001D2C0;
  function_80059F8C();
  goto label8001D4B8;
label8001D2C8:
  temp = a0 != v0;
  v0 = 6; // 0x0006
  if (temp) goto label8001D45C;
  v0 = 0x8006FCF4; // &0x000EA69B
  a1 = 0;
  v1 = lw(0x800770B8);
  a0 = 0x800781A4;
  sw(v0 + 0x00, v1);
  v0 += 4; // 0x0004
  v1 = v0 + 9212; // 0x23FC
  sw(v0 + 0x00, 0);
label8001D2FC:
  sw(v1 + 0x00, a0);
  v1 += 4; // 0x0004
  a1++;
  v0 = (int32_t)a1 < 3;
  temp = v0 != 0;
  a0 += 88; // 0x0058
  if (temp) goto label8001D2FC;
  sw(v1 + 0x00, 0);
  ra = 0x8001D31C;
  function_8001F158();
  v0 = lw(0x800712F4); // &0xEBEBEBEB
  temp = v0 == 0;
  if (temp) goto label8001D438;
  a0 = lh(0x8006F3C0 + lw(0x8007706C)*2); // &0x0DE90EF6
  v0 = lh(0x80071310); // &0xC3C3C3C3
  mult(v0, a0);
  t0=lo;
  v0 = lh(0x80071312); // &0xC3C3C3C3
  mult(v0, a0);
  v1=lo;
  v0 = lh(0x80071314); // &0xC3C3C3C3
  mult(v0, a0);
  v0 = (int32_t)t0 >> 12;
  sh(0x80071310, v0); // &0xC3C3C3C3
  v0 = (int32_t)v1 >> 12;
  v1 = lw(0x8007706C);
  sh(0x80071312, v0); // &0xC3C3C3C3
  a0=lo;
  v0 = (int32_t)a0 >> 12;
  sh(0x80071314, v0); // &0xC3C3C3C3
  v0 = (int32_t)v1 < 9;
  temp = v0 != 0;
  v0 = v1 - 8; // 0xFFFFFFF8
  if (temp) goto label8001D438;
  a0 = v0 << 2;
  a0 += v0;
  a0 = a0 << 2;
  a0 += v0;
  a0 = a0 << 1;
  ra = 0x8001D3D8;
  v0 = spyro_cos(a0);
  v1 = lh(0x80071316); // &0xC3C3C3C3
  a0 = v0;
  mult(v1, a0);
  t0=lo;
  v0 = lh(0x80071318); // &0xC3C3C3C3
  mult(v0, a0);
  v1=lo;
  v0 = lh(0x8007131A); // &0xD4D4C3C3
  mult(v0, a0);
  v0 = (int32_t)t0 >> 12;
  sh(0x80071316, v0); // &0xC3C3C3C3
  v0 = (int32_t)v1 >> 12;
  sh(0x80071318, v0); // &0xC3C3C3C3
  a0=lo;
  v0 = (int32_t)a0 >> 12;
  sh(0x8007131A, v0); // &0xD4D4C3C3
label8001D438:
  a0 = 0x8006FCF4; // &0x000EA69B
  a1 = 0;
  a2 = 2304; // 0x0900
  ra = 0x8001D44C;
  spyro_memset32(a0, a1, a2);
  ra = 0x8001D454;
  function_8001F798();
  goto label8001D4A8;
label8001D45C:
  temp = a0 != v0;
  v0 = 7; // 0x0007
  if (temp) goto label8001D510;
  a3 = 0x8006FCF4 + 0x2400; // "H^^^oooooofffOOO((("
  a0 = 0;
  v1 = 0x800781A4;
label8001D478:
  sw(a3 + 0x00, v1);
  a3 += 4; // 0x0004
  a0++;
  v0 = (int32_t)a0 < 3;
  temp = v0 != 0;
  v1 += 88; // 0x0058
  if (temp) goto label8001D478;
  a0 = 0x8006FCF4; // &0x000EA69B
  a1 = 0;
  a2 = 2304; // 0x0900
  sw(a3 + 0x00, 0);
  ra = 0x8001D4A8;
  spyro_memset32(a0, a1, a2);
label8001D4A8:
  ra = 0x8001D4B0;
  function_80022A2C();
  ra = 0x8001D4B8;
  function_80023AC4();
label8001D4B8:
  ra = 0x8001D4C0;
  function_80059A48();
  ra = 0x8001D4C8;
  function_8002B9CC();
  ra = 0x8001D4D0;
  function_80050BD0();
  ra = 0x8001D4D8;
  function_800573C8();
label8001D4D8:
  v0 = lw(0x8007570C); // &0x00000000
  temp = v0 != 0;
  if (temp) goto label8001D500;
  v0 = lw(0x800756C0); // &0x00000000
  temp = v0 == 0;
  if (temp) goto label8001D644;
label8001D500:
  ra = 0x8001D508;
  function_80018F30();
  goto label8001D644;
label8001D510:
  temp = a0 != v0;
  if (temp) goto label8001D644;
  v0 = lw(0x8007706C);
  v0 = (int32_t)v0 < 16;
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label8001D580;
  v1 = 0x8006FCF4 + 0x2400; // "H^^^oooooofffOOO((("
  a0 = 0x800781A4;
label8001D540:
  sw(v1 + 0x00, a0);
  v1 += 4; // 0x0004
  a1++;
  v0 = (int32_t)a1 < 3;
  temp = v0 != 0;
  a0 += 88; // 0x0058
  if (temp) goto label8001D540;
  sw(v1 + 0x00, 0);
  a0 = 0x8006FCF4; // &0x000EA69B
  a1 = 0;
  a2 = 2304; // 0x0900
  ra = 0x8001D570;
  spyro_memset32(a0, a1, a2);
  ra = 0x8001D578;
  function_80022A2C();
  goto label8001D5CC;
label8001D580:
  ra = 0x8001D588;
  function_800521C0();
  ra = 0x8001D590;
  function_8001F158();
  a0 = 0x8006FCF4; // &0x000EA69B
  a1 = 0;
  a2 = 2304; // 0x0900
  ra = 0x8001D5A4;
  spyro_memset32(a0, a1, a2);
  ra = 0x8001D5AC;
  function_8001F798();
  ra = 0x8001D5B4;
  function_800208FC();
  ra = 0x8001D5BC;
  function_80020F34();
  ra = 0x8001D5C4;
  function_80022A2C();
  ra = 0x8001D5CC;
  function_80059F8C();
label8001D5CC:
  ra = 0x8001D5D4;
  function_80023AC4();
  ra = 0x8001D5DC;
  function_80059A48();
  ra = 0x8001D5E4;
  function_8002B9CC();
  ra = 0x8001D5EC;
  function_80050BD0();
  ra = 0x8001D5F4;
  function_800573C8();
  v0 = lw(0x8007570C); // &0x00000000
  temp = v0 != 0;
  if (temp) goto label8001D61C;
  v0 = lw(0x800756C0); // &0x00000000
  temp = v0 == 0;
  if (temp) goto label8001D624;
label8001D61C:
  ra = 0x8001D624;
  function_80018F30();
label8001D624:
  a1 = lw(0x80077074);
  temp = a1 == 0;
  a0 = 1; // 0x0001
  if (temp) goto label8001D644;
  a2 = a1;
  a3 = a1;
  ra = 0x8001D644;
  function_800190D4();
label8001D644:

  wait_two_vsyncs();

  PutDispEnv(&backbuffer_disp->disp);
  PutDrawEnv(&backbuffer_disp->draw);
  DrawOTag(spyro_combine_all_command_buffers(0x800));

  ra = lw(sp + 0x24);
  s0 = lw(sp + 0x20);
  sp += 40; // 0x0028
}

// size: 0x00000B34
void function_8001D718(void)
{
  struct game_object *object;

  sp -= 0x68;
  sw(sp + 0x64, ra);
  sw(sp + 0x60, s4);
  sw(sp + 0x5C, s3);
  sw(sp + 0x58, s2);
  sw(sp + 0x54, s1);
  sw(sp + 0x50, s0);
  
  if (lw(0x800756FC) == lw(0x800785F0)) {
    v0 = lw(allocator1_ptr) + 0x1BA00;
    sw(0x800756FC, v0); // &0x00000000
    sw(gameobject_stack_ptr, v0);
  }
  s0 = lw(0x80078D00);
  if (s0 == 1) {
    v0 = lw(0x80078D0C);
    v1 = lw(0x80078D10);
    a0 = lh(0x8006F350 + v0*8) + v1;
    a1 = lh(0x8006F352 + v0*8) + v1;
    a2 = lh(0x8006F354 + v0*8);
    a3 = lh(0x8006F356 + v0*8);
    gui_box_balloonist(a0, a1, a2, a3);

    sw(sp + 0x20, 0x1100);

    sw(sp + 0x28, 0x10);
    sw(sp + 0x2C, 1);
    sw(sp + 0x30, 0x1400);

    switch (lw(0x80078D0C))
    {
    case 0:

      sw(sp + 0x18, lw(0x80078D10) + 0x40);
      sw(sp + 0x1C, 0x32);

      create_3d_text1("HI SPYRO,", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 18, 11);
      
      sw(sp + 0x18, lw(0x80078D10) + 0x4C);
      sw(sp + 0x1C, lw(sp + 0x1C) + 0x18);
      
      create_3d_text1("SAVE GAME", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 18, 11);
      
      if (lw(0x80078D08) == 0) {
        s1 = lw(gameobject_stack_ptr);
        s3 = 8;
      }
      sw(sp + 0x18, lw(0x80078D10) + 0x4C);
      sw(sp + 0x1C, lw(sp + 0x1C) + 0x13);
      
      create_3d_text1("REPLAY DRAGON", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 18, 11);
      
      if (lw(0x80078D08) == 1) {
        s1 = lw(gameobject_stack_ptr);
        s3 = 12;
      }
      sw(sp + 0x18, lw(0x80078D10) + 0x4C);
      sw(sp + 0x1C, lw(sp + 0x1C) + 0x13);
      
      create_3d_text1("CONTINUE", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 18, 11);
      
      if (lw(0x80078D08) == 2) {
        s1 = lw(gameobject_stack_ptr);
        s3 = 8;
      }
      object = addr_to_pointer(s1);
      text_wave_effect3(object, s3, lw(0x80078D04)*4, 12, 3, 0x200);
      break;
    case 1:
      sw(sp + 0x18, lw(0x80078D10) + 0x52);
      sw(sp + 0x1C, 0x32);
      create_3d_text1("NO SAVE FILE", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 18, 11);

      text_wave_effect2(10, lw(0x80078D04)*4, 12, 3, 0x200);

      sw(sp + 0x18, lw(0x80078D10) + 0x2D);
      sw(sp + 0x1C, lw(sp + 0x1C) + 0x1A);
      sw(sp + 0x20, 0x1400);

      sw(sp + 0x28, 0x0E);
      sw(sp + 0x2C, 0x01);
      sw(sp + 0x30, 0x1600);

      create_3d_text1("PLEASE RESTART WITH", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 16, 11);

      sw(sp + 0x18, lw(0x80078D10) + 0x43);
      sw(sp + 0x1C, lw(sp + 0x1C) + 0x11);

      create_3d_text1("A MEMORY CARD TO", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 16, 11);
      
      sw(sp + 0x1C, lw(sp + 0x1C) + 0x11);
      sw(sp + 0x18, lw(0x80078D10) + 0x38);

      create_3d_text1("ENABLE GAME SAVES.", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 16, 11);
      break;
    case 2:
      sw(sp + 0x18, lw(0x80078D10) + 0x6A);
      sw(sp + 0x1C, 0x52);

      create_3d_text1("SAVING...", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 18, 11);
      
      text_wave_effect2(9, lw(0x80078D04)*4, 12, 3, 0x200);
      break;
    case 3:
    case 4:
      sw(sp + 0x1C, 0x32);
      char *str;
      if (lw(0x80078D0C) == 3) {
        str = "NO MEMORY CARD";
        s3 = 12;
        sw(sp + 0x18, lw(0x80078D10) + 0x42);
      } else {
        str = "NO SAVE FILE";
        s3 = 10;
        sw(sp + 0x18, lw(0x80078D10) + 0x54);
      }
      create_3d_text1(str, addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), a3, 11);
      
      text_wave_effect2(s3, lw(0x80078D04)*4, 12, 3, 0x200);
      
      sw(sp + 0x28, 0x0E);
      sw(sp + 0x2C, 1);
      sw(sp + 0x30, 0x1600);

      sw(sp + 0x18, lw(0x80078D10) + 0x3B);
      sw(sp + 0x1C, lw(sp + 0x1C) + 0x1A);
      sw(sp + 0x20, 0x1400);

      create_3d_text1("PLEASE INSERT THE", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 16, 11);

      sw(sp + 0x18, lw(0x80078D10) + 0x43);
      sw(sp + 0x1C, lw(sp + 0x1C) + 0x11);

      create_3d_text1("MEMORY CARD WITH", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 16, 11);

      sw(sp + 0x18, lw(0x80078D10) + 0x2D);
      sw(sp + 0x1C, lw(sp + 0x1C) + 0x11);
      
      create_3d_text1("THIS GAME SAVE FILE", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 16, 11);
      break;
    case 5:
      sw(sp + 0x18, lw(0x80078D10) + 0x5E);
      sw(sp + 0x1C, 0x32);

      create_3d_text1("SAVE ERROR", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 18, 11);

      text_wave_effect2(9, lw(0x80078D04)*4, 12, 3, 0x200);

      sw(sp + 0x28, 0x0E);
      sw(sp + 0x2C, 0x01);
      sw(sp + 0x30, 0x1600);
      
      sw(sp + 0x18, lw(0x80078D10) + 0x3B);
      sw(sp + 0x1C, lw(sp + 0x1C) + 0x1A);
      sw(sp + 0x20, 0x1400);

      create_3d_text1("PLEASE CHECK THAT", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 16, 11);

      sw(sp + 0x1C, lw(sp + 0x1C) + 0x11);
      sw(sp + 0x18, lw(0x80078D10) + 0x49);

      create_3d_text1("THE MEMORY CARD", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 16, 11);

      sw(sp + 0x1C, lw(sp + 0x1C) + 0x11);
      sw(sp + 0x18, lw(0x80078D10) + 0x36);

      create_3d_text1("IS STILL IN PLACE.", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 16, 11);
      break;
    case 6:
      sw(sp + 0x18, lw(0x80078D10) + 0x56);
      sw(sp + 0x1C, 0x35);

      create_3d_text1("SAVE FAILED", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 18, 11);
      
      sw(sp + 0x18, lw(0x80078D10) + 0x87);
      sw(sp + 0x1C, lw(sp + 0x1C) + 0x28);

      create_3d_text1("RETRY", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 18, 11);
      
      if (lw(0x80078D08) == 0) {
        s1 = lw(gameobject_stack_ptr);
        s3 = 5;
      }
      sw(sp + 0x18, lw(0x80078D10) + 0x87);
      sw(sp + 0x1C, lw(sp + 0x1C) + 0x13);

      create_3d_text1("ABORT", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 18, 11);
      
      if (lw(0x80078D08) == 1) {
        s1 = lw(gameobject_stack_ptr);
        s3 = 5;
      }
      object = addr_to_pointer(s1);
      text_wave_effect3(object, s3, lw(0x80078D04)*4, 12, 3, 0x200);
      break;
    case 7:
      sw(sp + 0x18, lw(0x80078D10) + 0x5E);
      sw(sp + 0x1C, 0x52);
      create_3d_text1("GAME SAVED", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 18, 11);
      
      text_wave_effect2(9, lw(0x80078D04)*4, 12, 3, 0x200);
      break;
    }
    if ((int32_t)lw(0x80078D0C) >= 2) {

      char *str;
      if (lw(0x80078D18) == 0) {
        str = "SLOT 1";
        v0 = lw(0x80078D10) + 0x30;
      } else {
        str = "SLOT 2";
        v0 = 0xE6;
      }

      sw(sp + 0x18, v0);
      sw(sp + 0x1C, 0x1D);
      sw(sp + 0x20, 0x1600);

      sw(sp + 0x28, 0x0D);
      sw(sp + 0x2C, 0x01);
      sw(sp + 0x30, 0x1A00);

      create_3d_text1(str, addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 14, 11);
    }
  }
  ra = 0x8001E114;
  function_800521C0();
  ra = 0x8001E11C;
  function_80019698();
  if (lw(0x80078D00) == 1) {
    sw(0x8006FCF4 + 0x2400, 0);
    ra = 0x8001E140;
    function_80018880();
    ra = 0x8001E148;
    function_80022A2C();
  }
  ra = 0x8001E150;
  function_8002B9CC();
  ra = 0x8001E158;
  function_80050BD0();
  ra = 0x8001E160;
  function_800573C8();
  ra = 0x8001E168;
  function_80018F30();

  wait_two_vsyncs();

  PutDispEnv(&backbuffer_disp->disp);
  PutDrawEnv(&backbuffer_disp->draw);
  DrawOTag(spyro_combine_all_command_buffers(0x800));

  ra = lw(sp + 0x64);
  s4 = lw(sp + 0x60);
  s3 = lw(sp + 0x5C);
  s2 = lw(sp + 0x58);
  s1 = lw(sp + 0x54);
  s0 = lw(sp + 0x50);
  sp += 0x68;
}

// size: 0x00000A08
void function_ptr_800758D8(void)
{
  char buf[0x20];
  uint32_t temp, return_address = ra;
  sp -= 248; // 0xFFFFFF08
  sw(sp + 0xE8, s2);
  sw(sp + 0xF4, ra);
  sw(sp + 0xF0, s4);
  sw(sp + 0xEC, s3);
  sw(sp + 0xE4, s1);
  sw(sp + 0xE0, s0);

  s2 = 0x800777E8;
  v1 = lw(s2 + 0x00);
  v0 = (int32_t)v1 < 4;
  temp = v0 == 0;
  v0 = (int32_t)v1 < 2;
  if (temp) goto label8007D940;
  temp = v0 == 0;
  if (temp) goto label8007D114;
  s0 = sp + 56; // 0x0038
  s1 = lw(gameobject_stack_ptr);
  if (lw(LEVEL_ID)/10 != lw(WORLD_ID)+1) {
    printf("LEVEL_ID: %d\n", lw(LEVEL_ID));
    printf("WORLD_ID: %d\n", lw(WORLD_ID));
    BREAKPOINT;
  }
  sprintf(buf, "%s THE BALLOONIST", balloonist_names[lw(LEVEL_ID)/10]);

  sw(sp + 0x1C, 0xCC);
  sw(sp + 0x18, 0x108 - strlen(buf)*8);
  sw(sp + 0x20, 0x1100);

  sw(sp + 0x28, 0x10);
  sw(sp + 0x2C, 0x01);
  sw(sp + 0x30, 0x1400);
  
  create_3d_text1(buf, addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 18, 11);
  v0 = lw(gameobject_stack_ptr);
  s1 -= 88; // 0xFFFFFFA8
  v0 = (int32_t)s1 < (int32_t)v0;
  temp = v0 != 0;
  if (temp) goto label8007D114;
  a1 = s2 + 4; // 0x0004
  a0 = spyro_cos_lut;
  v1 = 0;
label8007D0D0:
  v0 = lw(a1 + 0x00);
  v0 = v0 << 2;
  v0 += v1;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a0;
  v0 = lhu(v0 + 0x00);
  v0 = v0 >> 8;
  sb(s1 + 0x46, v0);
  v0 = lw(gameobject_stack_ptr);
  s1 -= 88; // 0xFFFFFFA8
  v0 = (int32_t)s1 < (int32_t)v0;
  temp = v0 == 0;
  v1 += 12; // 0x000C
  if (temp) goto label8007D0D0;
label8007D114:
  s3 = lw(0x800777E8);
  v0 = 1; // 0x0001
  temp = s3 != v0;
  if (temp) goto label8007D8F8;
  v1 = lw(0x800777F4);
  v0 = v1 - 30; // 0xFFFFFFE2
  v0 = v0 < 3;
  temp = v0 != 0;
  v0 = 30; // 0x001E
  if (temp) goto label8007D30C;
  a0 = 72; // 0x0048
  a1 = 440; // 0x01B8
  a2 = 28; // 0x001C
  a3 = 104; // 0x0068
  ra = 0x8007D158;
  gui_box_balloonist(a0, a1, a2, a3);
  v1 = lw(0x800777F4);
  v0 = 44; // 0x002C
  sw(sp + 0x5C, v0);
  v0 = 4352; // 0x1100
  sw(sp + 0x60, v0);
  v0 = 16; // 0x0010
  sw(sp + 0x68, v0);
  v0 = 5120; // 0x1400
  sw(sp + 0x6C, 1);
  sw(sp + 0x70, v0);
  s0 = lbu(0x8006F89C + v1);
  v0 = lbu(0x8006F89D + v1);
  s2 = lw(gameobject_stack_ptr);
  v0 = (int32_t)s0 < (int32_t)v0;
  temp = v0 == 0;
  v0 = s0 << 2;
  if (temp) goto label8007D214;
  s3 = 94; // 0x005E
  v1 = 0x8006F8C4;
  s1 = v0 + v1;
  a1 = sp + 88; // 0x0058
label8007D1C4:
  a2 = sp + 104; // 0x0068
  a3 = 18; // 0x0012
  sw(sp + 0x58, s3);
  a0 = lw(s1 + 0x00);
  s1 += 4; // 0x0004
  create_3d_text1(addr_to_pointer(a0), addr_to_pointer(a1), *(vec3_32 *)addr_to_pointer(a2), a3, 11);
  v0 = lw(sp + 0x5C);
  v1 = lw(0x800777F4);
  v0 += 18; // 0x0012
  sw(sp + 0x5C, v0);
  v0 = lbu(0x8006F89D + v1);
  s0++;
  v0 = (int32_t)s0 < (int32_t)v0;
  temp = v0 != 0;
  a1 = sp + 88; // 0x0058
  if (temp) goto label8007D1C4;
label8007D214:
  v1 = lw(0x800777EC);
  v0 = (int32_t)v1 < 80;
  temp = v0 == 0;
  v0 = v1 << 1;
  if (temp) goto label8007D25C;
  v0 += v1;
  v0 = v0 << 2;
  v0 -= v1;
  v0 = v0 << 3;
  v1 = lw(gameobject_stack_ptr);
  v0 = s2 - v0;
  v1 = (int32_t)v0 < (int32_t)v1;
  temp = v1 != 0;
  if (temp) goto label8007D25C;
  sw(gameobject_stack_ptr, v0);
label8007D25C:
  v0 = lw(gameobject_stack_ptr);
  s2 -= 88; // 0xFFFFFFA8
  v0 = (int32_t)s2 < (int32_t)v0;
  temp = v0 != 0;
  if (temp) goto label8007D2CC;
  a1 = 0x800777EC;
  a0 = spyro_cos_lut;
  v1 = 0;
label8007D288:
  v0 = lw(a1 + 0x00);
  v0 = v0 << 2;
  v0 += v1;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a0;
  v0 = lhu(v0 + 0x00);
  v0 = v0 >> 8;
  sb(s2 + 0x46, v0);
  v0 = lw(gameobject_stack_ptr);
  s2 -= 88; // 0xFFFFFFA8
  v0 = (int32_t)s2 < (int32_t)v0;
  temp = v0 == 0;
  v1 += 12; // 0x000C
  if (temp) goto label8007D288;
label8007D2CC:
  a1 = lw(0x800777EC);
  v0 = (int32_t)a1 < 80;
  temp = v0 != 0;
  a0 = sp + 88; // 0x0058
  if (temp) goto label8007D8F8;
  a2 = 0;
  v0 = 412; // 0x019C
  sw(sp + 0x58, v0);
  v0 = 96; // 0x0060
  sw(sp + 0x5C, v0);
  v0 = 4352; // 0x1100
  sw(sp + 0x60, v0);
  ra = 0x8007D304;
  blinking_arrow(*(vec3_32*)addr_to_pointer(a0), a1, 0);
  goto label8007D8F8;
label8007D30C:
  temp = v1 != v0;
  if (temp) goto label8007D5F4;
  v1 = lw(0x800777F8);
  sw(sp + 0x88, 0x0E);
  sw(sp + 0x90, 0x1600);
  sw(sp + 0x80, 0x1400);
  sw(sp + 0x78, 0xF0);
  sw(sp + 0x8C, 1);
  sw(sp + 0x7C, 0x24);
  if (v1 != 0) goto label8007D360;
  a0 = 0x8007AA70; // "HOP ON, SPYRO!"
  goto label8007D384;
label8007D360:
  if (v1 != 1) goto label8007D378;
  a0 = 0x8007AA80; // "NEED A LIFT?"
  goto label8007D384;
label8007D378:
  a0 = 0x8007AA90; // "WHERE TO, SPYRO?"
label8007D384:
  create_3d_text1(addr_to_pointer(a0), addr_to_pointer(sp + 0x78), *(vec3_32 *)addr_to_pointer(sp + 0x88), 16, 11);
  v1 = lw(gameobject_stack_ptr);
  v0 = 272; // 0x0110
  sw(sp + 0x78, v0);
  v0 = lw(sp + 0x7C);
  s0 = v1 - 88; // 0xFFFFFFA8
  v0 += 20; // 0x0014
  sw(sp + 0x7C, v0);
  create_3d_text1("STAY HERE", addr_to_pointer(sp + 0x78), *(vec3_32 *)addr_to_pointer(sp + 0x88), 16, 11);
  a0 = 0x800777F0;
  v0 = lw(sp + 0x7C);
  v1 = lw(a0 + 0x00);
  v0 += 15; // 0x000F
  temp = v1 != 0;
  sw(sp + 0x7C, v0);
  if (temp) goto label8007D45C;
  v0 = lw(gameobject_stack_ptr);
  v0 = s0 < v0;
  temp = v0 != 0;
  s2 = 1; // 0x0001
  if (temp) goto label8007D460;
  v1 = a0 - 4; // 0xFFFFFFFC
  a1 = spyro_cos_lut;
  a0 = 0;
label8007D418:
  v0 = lw(v1 + 0x00);
  v0 = v0 << 2;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a1;
  v0 = lhu(v0 + 0x00);
  v0 = v0 >> 8;
  sb(s0 + 0x46, v0);
  v0 = lw(gameobject_stack_ptr);
  s0 -= 88; // 0xFFFFFFA8
  v0 = s0 < v0;
  temp = v0 == 0;
  a0 += 12; // 0x000C
  if (temp) goto label8007D418;
label8007D45C:
  s2 = 1; // 0x0001
label8007D460:
  s1 = 0;
  s4 = 0x800777EC;
  s3 = spyro_cos_lut;
  v0 = 0x66660000;
label8007D478:
  v1 = lw(LEVEL_ID);
  v0 = v0 | 0x6667;
  mult(v1, v0);
  v1 = (int32_t)v1 >> 31;
  t0=hi;
  v0 = (int32_t)t0 >> 2;
  v0 -= v1;
  v0--;
  temp = v0 == s1;
  v0 = 2; // 0x0002
  if (temp) goto label8007D570;
  v1 = lbu(0x800758D0 + s1);
  if (v1 != v0) goto label8007D570;
  v0 = 272; // 0x0110
  sw(sp + 0x78, v0);
  v0 = s1 << 2;
  v1 = lw(gameobject_stack_ptr);
  s0 = v1 - 88; // 0xFFFFFFA8
  
  create_3d_text1(homeworld_names[s1], addr_to_pointer(sp + 0x78), *(vec3_32 *)addr_to_pointer(sp + 0x88), 16, 11);
  v0 = lw(sp + 0x7C);
  v1 = lw(0x800777F0);
  v0 += 15; // 0x000F
  temp = v1 != s2;
  sw(sp + 0x7C, v0);
  if (temp) goto label8007D56C;
  v0 = lw(gameobject_stack_ptr);
  v0 = s0 < v0;
  temp = v0 != 0;
  if (temp) goto label8007D56C;
  v1 = 0;
label8007D528:
  v0 = lw(s4 + 0x00);
  v0 = v0 << 2;
  v0 += v1;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += s3;
  v0 = lhu(v0 + 0x00);
  v0 = v0 >> 8;
  sb(s0 + 0x46, v0);
  v0 = lw(gameobject_stack_ptr);
  s0 -= 88; // 0xFFFFFFA8
  v0 = s0 < v0;
  temp = v0 == 0;
  v1 += 12; // 0x000C
  if (temp) goto label8007D528;
label8007D56C:
  s2++;
label8007D570:
  s1++;
  v0 = (int32_t)s1 < 6;
  temp = v0 != 0;
  v0 = 0x66660000;
  if (temp) goto label8007D478;
  a0 = 224; // 0x00E0
  a1 = 472; // 0x01D8
  a2 = 25; // 0x0019
  a3 = s2 << 4;
  a3 -= s2;
  a3 += 51; // 0x0033
  ra = 0x8007D59C;
  gui_box_balloonist(a0, a1, a2, a3);
  a0 = 240; // 0x00F0
  a1 = 46; // 0x002E
  a2 = 416; // 0x01A0
  a3 = 46; // 0x002E
  ra = 0x8007D5B0;
  function_8001844C();
  a0 = sp + 120; // 0x0078
  a2 = 0;
  a1 = lw(0x800777EC);
  v1 = lw(0x800777F0);
  v0 = 244; // 0x00F4
  sw(sp + 0x78, v0);
  v0 = 4352; // 0x1100
  sw(sp + 0x80, v0);
  v0 = v1 << 4;
  v0 -= v1;
  v0 += 54; // 0x0036
  sw(sp + 0x7C, v0);
  ra = 0x8007D5EC;
  blinking_arrow(*(vec3_32*)addr_to_pointer(a0), a1, 0);
  goto label8007D8F8;
label8007D5F4:
  v0 = v1 - 31; // 0xFFFFFFE1
  v0 = v0 < 2;
  temp = v0 == 0;
  a0 = 72; // 0x0048
  if (temp) goto label8007D8F8;
  a1 = 440; // 0x01B8
  a2 = 28; // 0x001C
  a3 = 102; // 0x0066
  ra = 0x8007D610;
  gui_box_balloonist(a0, a1, a2, a3);
  a0 = 100; // 0x0064
  a1 = 55; // 0x0037
  a2 = 388; // 0x0184
  a3 = 55; // 0x0037
  ra = 0x8007D624;
  function_8001844C();
  s2 = sp + 152; // 0x0098
  a0 = s2;
  a2 = 0;
  s1 = 96; // 0x0060
  a1 = lw(0x800777EC);
  v1 = lw(0x800777F0);
  s0 = 4352; // 0x1100
  sw(sp + 0x98, s1);
  sw(sp + 0xA0, s0);
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 2;
  v0 -= v1;
  v0 += 67; // 0x0043
  sw(sp + 0x9C, v0);
  ra = 0x8007D66C;
  blinking_arrow(*(vec3_32*)addr_to_pointer(a0), a1, 0);
  v1 = lw(0x800777F4);
  v0 = 16; // 0x0010
  sw(sp + 0xA8, v0);
  v0 = 5120; // 0x1400
  sw(sp + 0xB0, v0);
  v0 = 44; // 0x002C
  sw(sp + 0x9C, v0);
  v0 = 31; // 0x001F
  sw(sp + 0xAC, s3);
  sw(sp + 0xA0, s0);
  temp = v1 != v0;
  sw(sp + 0x98, s1);
  if (temp) goto label8007D6B4;
  a0 = 0x8007AAB0; // "ARE YOU READY TO GO?"
  goto label8007D6C4;
label8007D6B4:
  a0 = 0x8007AAC8; // "WOULD YOU LIKE TO GO?"
label8007D6C4:
  create_3d_text1(addr_to_pointer(a0), addr_to_pointer(s2), *(vec3_32 *)addr_to_pointer(sp + 0xA8), 18, 11);
  v1 = lw(gameobject_stack_ptr);
  v0 = 128; // 0x0080
  sw(sp + 0x98, v0);
  v0 = lw(sp + 0x9C);
  s0 = v1 - 88; // 0xFFFFFFA8
  v0 += 25; // 0x0019
  sw(sp + 0x9C, v0);
  v0 = 11; // 0x000B
  sw(sp + 0x10, v0);
  create_3d_text1("STAY HERE", addr_to_pointer(sp + 0x98), *(vec3_32 *)addr_to_pointer(sp + 0xA8), 18, 11);
  a0 = 0x800777F0;
  v0 = lw(sp + 0x9C);
  v1 = lw(a0 + 0x00);
  v0 += 19; // 0x0013
  temp = v1 != 0;
  sw(sp + 0x9C, v0);
  if (temp) goto label8007D798;
  v0 = lw(gameobject_stack_ptr);
  v0 = s0 < v0;
  temp = v0 != 0;
  v1 = a0 - 4; // 0xFFFFFFFC
  if (temp) goto label8007D798;
  a1 = spyro_cos_lut;
  a0 = 0;
label8007D754:
  v0 = lw(v1 + 0x00);
  v0 = v0 << 2;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a1;
  v0 = lhu(v0 + 0x00);
  v0 = v0 >> 8;
  sb(s0 + 0x46, v0);
  v0 = lw(gameobject_stack_ptr);
  s0 -= 88; // 0xFFFFFFA8
  v0 = s0 < v0;
  temp = v0 == 0;
  a0 += 12; // 0x000C
  if (temp) goto label8007D754;
label8007D798:
  v1 = lw(gameobject_stack_ptr);
  v0 = 128; // 0x0080
  sw(sp + 0x98, v0);
  s0 = v1 - 88; // 0xFFFFFFA8
  v1 = lw(0x800777F4);
  v0 = 31; // 0x001F
  temp = v1 != v0;
  v0 = 0x66660000;
  if (temp) goto label8007D7F8;
  v1 = lw(LEVEL_ID);
  v0 = v0 | 0x6667;
  mult(v1, v0);
  v1 = (int32_t)v1 >> 31;
  t0=hi;
  v0 = (int32_t)t0 >> 2;
  v0 -= v1;
  v0 = v0 << 2;
  a2 = lw(HOMEWORLD_NAMES + 0x00 + v0);
  a0 = sp + 184; // 0x00B8
  goto label8007D84C;
label8007D7F8:
  a1 = lw(0x800758B4);
  v0 = v0 | 0x6667;
  mult(a1, v0);
  a0 = sp + 184; // 0x00B8
  v0 = (int32_t)a1 >> 31;
  t0=hi;
  v1 = (int32_t)t0 >> 2;
  v1 -= v0;
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 1;
  a1 -= v0;
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 1;
  v0 += a1;
  v0 = v0 << 2;
  a2 = lw(HOMEWORLD_NAMES + 0x04 + v0);
label8007D84C:
  a1 = 0x8007AAE0; // "GO TO %s"
  spyro_sprintf();
  create_3d_text1(addr_to_pointer(sp + 0xB8), addr_to_pointer(sp + 0x98), *(vec3_32 *)addr_to_pointer(sp + 0xA8), 18, 11);
  a0 = 0x800777F0;
  v1 = lw(a0 + 0x00);
  v0 = 1; // 0x0001
  temp = v1 != v0;
  if (temp) goto label8007D8F8;
  v0 = lw(gameobject_stack_ptr);
  v0 = s0 < v0;
  temp = v0 != 0;
  v1 = a0 - 4; // 0xFFFFFFFC
  if (temp) goto label8007D8F8;
  a1 = spyro_cos_lut;
  a0 = 0;
label8007D8B4:
  v0 = lw(v1 + 0x00);
  v0 = v0 << 2;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a1;
  v0 = lhu(v0 + 0x00);
  v0 = v0 >> 8;
  sb(s0 + 0x46, v0);
  v0 = lw(gameobject_stack_ptr);
  s0 -= 88; // 0xFFFFFFA8
  v0 = s0 < v0;
  temp = v0 == 0;
  a0 += 12; // 0x000C
  if (temp) goto label8007D8B4;
label8007D8F8:
  ra = 0x8007D900;
  function_800521C0();
  ra = 0x8007D908;
  function_80019698();
  v0 = lw(0x800777E8);
  v0 = (int32_t)v0 < 2;
  temp = v0 == 0;
  if (temp) goto label8007D98C;
  sw(0x8006FCF4 + 0x2400, 0);
  ra = 0x8007D930;
  function_80018880();
  ra = 0x8007D938;
  function_80022A2C();
  goto label8007D98C;
label8007D940:
  ra = 0x8007D948;
  function_800521C0();
  v1 = 0x8006FCF4;
  v0 = lw(v1 + 0x00);
  temp = v0 == 0;
  if (temp) goto label8007D978;
  v1 += 4; // 0x0004
label8007D964:
  v0 = lw(v1 + 0x00);
  temp = v0 != 0;
  v1 += 4; // 0x0004
  if (temp) goto label8007D964;
  v1 -= 4; // 0xFFFFFFFC
label8007D978:
  v0 = lw(0x80077850);
  sw(v1 + 0x04, 0);
  sw(v1 + 0x00, v0);
  ra = 0x8007D98C;
  function_80019698();
label8007D98C:
  ra = 0x8007D994;
  function_8002B9CC();
  ra = 0x8007D99C;
  function_80050BD0();
  ra = 0x8007D9A4;
  function_800573C8();

  ra = lw(sp + 0xF4);
  s4 = lw(sp + 0xF0);
  s3 = lw(sp + 0xEC);
  s2 = lw(sp + 0xE8);
  s1 = lw(sp + 0xE4);
  s0 = lw(sp + 0xE0);
  sp += 248; // 0x00F8
}

// size: 0x0000046C
void function_8001E24C(void)
{
  uint32_t temp;
  v1 = lw(0x800777E8);
  sp -= 136; // 0xFFFFFF78
  sw(sp + 0x84, ra);
  v0 = (int32_t)v1 < 4;
  temp = v0 != 0;
  sw(sp + 0x80, s0);
  if (temp) goto label8001E5C8;
  v0 = (int32_t)v1 < 6;
  temp = v0 == 0;
  if (temp) goto label8001E5C8;
  v0 = lw(0x80077850);
  v1 = 0x8006FCF4; // &0x000EA69B
  sw(v1 + 0x04, 0);
  sw(v1 + 0x00, v0);
  v1 = lw(0x800777FC);
  v0 = (int32_t)v1 < 5;
  temp = v0 == 0;
  if (temp) goto label8001E2E4;
  a2 = lw(0x8006F7D0 + (int32_t)lw(0x800758B4)/10*4);
  // problem here
  a1 = 0x80010CD0; // "ENTERING %s WORLD"
  a0 = sp + 56; // 0x0038
  goto label8001E384;
label8001E2E4:
  temp = v1 != 5;
  if (temp) goto label8001E328;
  a2 = lw(0x8006F7D0 + (int32_t)lw(0x800758B4)/10*4);
  a0 = sp + 56; // 0x0038
  goto label8001E37C;
label8001E328:
  a1 = lw(0x800758B4);
  v1 = (int32_t)a1/10;
  a1 = a1 % 10;
  v0 = v1*6 + a1;
  a2 = lw(0x8006F7D8 + v0*4); // &0x8001005C
label8001E37C:
  a1 = 0x80010CE4; // "ENTERING %s"
label8001E384:
  ra = 0x8001E38C;
  spyro_sprintf();
  s0 = sp + 56; // 0x0038
  a0 = s0;
  v0 = spyro_strlen(a0);
  v0--;
  v1 = v0*7;
  sw(sp + 0x18, 0x100 - v1);
  sw(sp + 0x1C, 0xC8);
  sw(sp + 0x20, 0x1400);
  sw(sp + 0x28, 14);
  sw(sp + 0x2C, 1);
  sw(sp + 0x30, 0x1600);
  s0 = lw(gameobject_stack_ptr);
  create_3d_text1(addr_to_pointer(a0), addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(sp + 0x28), 16, 2);
  v0 = lw(gameobject_stack_ptr);
  s0 -= 88; // 0xFFFFFFA8
  v0 = (int32_t)s0 < (int32_t)v0;
  temp = v0 != 0;
  v1 = 0;
  if (temp) goto label8001E468;
  a1 = 0x800777EC;
  a0 = spyro_cos_lut;
label8001E424:
  v0 = lw(a1 + 0x00);
  v0 = v0 << 1;
  v0 += v1;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a0;
  v0 = lhu(v0 + 0x00);
  v0 = v0 >> 7;
  sb(s0 + 0x46, v0);
  v0 = lw(gameobject_stack_ptr);
  s0 -= 88; // 0xFFFFFFA8
  v0 = (int32_t)s0 < (int32_t)v0;
  temp = v0 == 0;
  v1 += 12; // 0x000C
  if (temp) goto label8001E424;
label8001E468:
  ra = 0x8001E470;
  function_8001F158();
  a0 = 0x8006FCF4; // &0x000EA69B
  a1 = 0;
  a2 = 2304; // 0x0900
  ra = 0x8001E484;
  spyro_memset32(a0, a1, a2);
  ra = 0x8001E48C;
  function_8001F798();
  sw(0x8006FCF4 + 0x2400, 0); // "H^^^oooooofffOOO((("
  ra = 0x8001E49C;
  function_80018880();
  ra = 0x8001E4A4;
  function_80022A2C();
  ra = 0x8001E4AC;
  function_80023AC4();
  v1 = lw(0x800777E8);
  v0 = (int32_t)v1 < 6;
  temp = v0 == 0;
  v0 = 0x00400000;
  if (temp) goto label8001E5DC;
  v0 = v0 | 0x1010;
  sw(0x800757D4, v0); // &0x00000000
  v0 = 4; // 0x0004
  temp = v1 != v0;
  v0 = 256; // 0x0100
  if (temp) goto label8001E4EC;
  v0 = lw(0x800777EC);
  v0 = v0 << 4;
  goto label8001E500;
label8001E4EC:
  v1 = lw(0x800777EC);
  v0 -= v1;
  v0 = v0 << 4;
label8001E500:
  sw(0x8007575C, v0); // &0x00000000
  v0 = lw(0x8007575C); // &0x00000000
  temp = (int32_t)v0 >= 0;
  v0 = (int32_t)v0 < 4097;
  if (temp) goto label8001E534;
  sw(0x8007575C, 0); // &0x00000000
  v0 = lw(0x8007575C); // &0x00000000
  v0 = (int32_t)v0 < 4097;
label8001E534:
  temp = v0 != 0;
  v0 = 4096; // 0x1000
  if (temp) goto label8001E544;
  sw(0x8007575C, v0); // &0x00000000
label8001E544:
  a0 = lw(SKYBOX_DATA + 0x10);
  a1 = lw(0x800757D4); // &0x00000000
  a2 = lw(0x8007575C); // &0x00000000
  ra = 0x8001E564;
  v0 = interpolate_color(a0, a1, a2);
  sw(sp + 0x78, v0);
  v1 = lbu(sp + 0x78);
  a0 = lbu(sp + 0x79);
  v0 = lw(0x8007575C); // &0x00000000
  a1 = lbu(sp + 0x7A);
  v0 = (int32_t)v0 < 4096;
  sb(0x80076EF9, v1);
  sb(0x80076EFA, a0);
  sb(0x80076EFB, a1);
  sb(0x80076F7D, v1);
  sb(0x80076F7E, a0);
  sb(0x80076F7F, a1);
  temp = v0 == 0;
  if (temp) goto label8001E5DC;
  ra = 0x8001E5C0;
  function_8004F000();
  goto label8001E5DC;
label8001E5C8:
  function_ptr_800758D8();
label8001E5DC:
  ra = 0x8001E5E4;
  function_80018F30();

  wait_two_vsyncs();

  PutDispEnv(&backbuffer_disp->disp);
  PutDrawEnv(&backbuffer_disp->draw);
  DrawOTag(spyro_combine_all_command_buffers(0x800));

  ra = lw(sp + 0x84);
  s0 = lw(sp + 0x80);
  sp += 136; // 0x0088
}

// size: 0x00000310
void function_8001E6B8(void)
{
  uint32_t temp;
  sp -= 64; // 0xFFFFFFC0
  v1 = lw(0x80078D7C);
  v0 = 2; // 0x0002
  sw(sp + 0x3C, ra);
  temp = v1 != v0;
  sw(sp + 0x38, s0);
  if (temp) goto label8001E8C4;
  v0 = lw(0x80078D80);
  v0 = (int32_t)v0 < 140;
  temp = v0 != 0;
  v0 = 120; // 0x0078
  if (temp) goto label8001E8C4;
  sw(sp + 0x1C, v0);
  v0 = 5120; // 0x1400
  sw(sp + 0x20, v0);
  v0 = 14; // 0x000E
  s0 = lw(gameobject_stack_ptr);
  v1 = lw(0x80078D94);
  a0 = 1; // 0x0001
  sw(sp + 0x28, v0);
  v0 = 5632; // 0x1600
  sw(sp + 0x2C, a0);
  temp = v1 != 0;
  sw(sp + 0x30, v0);
  if (temp) goto label8001E754;
  a1 = sp + 24; // 0x0018
  a2 = sp + 40; // 0x0028
  a3 = 16; // 0x0010
  v0 = 92; // 0x005C
  sw(sp + 0x18, v0);
  create_3d_text1("IN THE WORLD OF DRAGONS...", addr_to_pointer(a1), *(vec3_32 *)addr_to_pointer(a2), a3, 11);
  v1 = 184; // 0x00B8
  goto label8001E7EC;
label8001E754:
  temp = v1 != a0;
  a1 = sp + 24; // 0x0018
  if (temp) goto label8001E7C4;
  v0 = lbu(0x80078E78);
  temp = v0 == 0;
  a2 = sp + 40; // 0x0028
  if (temp) goto label8001E798;
  a3 = 16; // 0x0010
  v0 = 80; // 0x0050
  sw(sp + 0x18, v0);
  v0 = 11; // 0x000B
  sw(sp + 0x10, v0);
  create_3d_text1("THE ADVENTURE CONTINUES...", addr_to_pointer(sp + 0x18), *(vec3_32 *)addr_to_pointer(a2), 16, 11);
  v1 = 188; // 0x00BC
  goto label8001E7EC;
label8001E798:
  a1 = sp + 24; // 0x0018
  a3 = 16; // 0x0010
  v0 = 100; // 0x0064
  sw(sp + 0x18, v0);
  create_3d_text1("THE ADVENTURE BEGINS...", addr_to_pointer(a1), *(vec3_32 *)addr_to_pointer(a2), 16, 11);
  v1 = 182; // 0x00B6
  goto label8001E7EC;
label8001E7C4:
  a2 = sp + 40; // 0x0028
  a3 = 16; // 0x0010
  v0 = 104; // 0x0068
  sw(sp + 0x18, v0);
  create_3d_text1("ENTERING DEMO MODE...", addr_to_pointer(a1), *(vec3_32 *)addr_to_pointer(a2), 16, 11);
  v1 = 178; // 0x00B2
label8001E7EC:
  a1 = 0x80078D80;
  a0 = lw(a1 + 0x00);
  v0 = (int32_t)a0 < (int32_t)v1;
  temp = v0 == 0;
  v0 = v1 - a0;
  if (temp) goto label8001E834;
  v0 = (int32_t)v0 >> 1;
  v1 = v0 << 1;
  v1 += v0;
  v1 = v1 << 2;
  v1 -= v0;
  v0 = lw(gameobject_stack_ptr);
  v1 = v1 << 3;
  v0 += v1;
  sw(gameobject_stack_ptr, v0);
label8001E834:
  v0 = lw(gameobject_stack_ptr);
  s0 -= 88; // 0xFFFFFFA8
  v0 = (int32_t)s0 < (int32_t)v0;
  temp = v0 != 0;
  a3 = 0;
  if (temp) goto label8001E8C4;
  t1 = a1;
  t0 = spyro_cos_lut;
  a1 = s0 + 70; // 0x0046
  a2 = 0;
label8001E860:
  a0 = lw(t1 + 0x00);
  v0 = a3 + 140; // 0x008C
  v1 = a0 - v0;
  v0 = (int32_t)v1 < 56;
  temp = v0 == 0;
  v0 = v1 << 3;
  if (temp) goto label8001E880;
  v0 += 64; // 0x0040
  goto label8001E8A0;
label8001E880:
  v0 = a0 << 2;
  v0 += a2;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += t0;
  v0 = lhu(v0 + 0x00);
  v0 = v0 >> 7;
label8001E8A0:
  sb(a1 + 0x00, v0);
  a2 += 12; // 0x000C
  a3++;
  v0 = lw(gameobject_stack_ptr);
  s0 -= 88; // 0xFFFFFFA8
  v0 = (int32_t)s0 < (int32_t)v0;
  temp = v0 == 0;
  a1 -= 88; // 0xFFFFFFA8
  if (temp) goto label8001E860;
label8001E8C4:
  s0 = 0x8006FCF4 + 0x2400; // "H^^^oooooofffOOO((("
  sb(0x80076EF9, 0);
  sb(0x80076EFA, 0);
  sb(0x80076EFB, 0);
  sb(0x80076F7D, 0);
  sb(0x80076F7E, 0);
  sb(0x80076F7F, 0);
  sw(s0 + 0x00, 0);
  ra = 0x8001E904;
  function_80018880();
  a0 = s0 - 9216; // 0xFFFFDC00
  a1 = 0;
  a2 = 2304; // 0x0900
  ra = 0x8001E914;
  spyro_memset32(a0, a1, a2);
  ra = 0x8001E91C;
  function_80022A2C();
  v1 = lw(0x80078D7C);
  v0 = 2; // 0x0002
  temp = v1 != v0;
  if (temp) goto label8001E974;
  ra = 0x8001E938;
  function_80023AC4();
  v1 = lw(linked_list1);
  a0 = lw(ordered_linked_list);
  v0 = lw(v1 + 0x00);
  sw(a0 + 0x0100, v0);
  v0 = lw(v1 + 0x04);
  sw(a0 + 0x0104, v0);
  sw(v1 + 0x00, 0);
  v0 = lw(linked_list1);
  sw(v0 + 0x04, 0);
label8001E974:
  a0 = 0;
  ra = 0x8001E97C;
  v0 = DrawSync(a0);
  a0 = 0;
  ra = 0x8001E984;
  v0 = VSync(a0);
  
  PutDispEnv(&backbuffer_disp->disp);
  PutDrawEnv(&backbuffer_disp->draw);
  DrawOTag(spyro_combine_all_command_buffers(0x800));

  ra = lw(sp + 0x3C);
  s0 = lw(sp + 0x38);
  sp += 64; // 0x0040
}

// size: 0x000001B8
void function_8001E9C8(void)
{
  uint32_t temp;
  v0 = lbu(SKYBOX_DATA + 0x10);
  v1 = lbu(SKYBOX_DATA + 0x11);
  a0 = lbu(SKYBOX_DATA + 0x12);
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x14, ra);
  sw(sp + 0x10, s0);
  sb(0x80076EF9, v0);
  sb(0x80076EFA, v1);
  sb(0x80076EFB, a0);
  sb(0x80076F7D, v0);
  sb(0x80076F7E, v1);
  sb(0x80076F7F, a0);
  ra = 0x8001EA24;
  function_800521C0();
  ra = 0x8001EA2C;
  function_8001F158();
  s0 = 0x8006FCF4; // &0x000EA69B
  a0 = s0;
  a1 = 0;
  a2 = 2304; // 0x0900
  ra = 0x8001EA44;
  spyro_memset32(a0, a1, a2);
  ra = 0x8001EA4C;
  function_8001F798();
  a0 = s0;
  a1 = 0;
  a2 = 7168; // 0x1C00
  ra = 0x8001EA5C;
  spyro_memset32(a0, a1, a2);
  v0 = 0x00014000;
  sw(0x800785D0, v0);
  a0 = -1; // 0xFFFFFFFF
  ra = 0x8001EA74;
  function_800258F0();
  a0 = -1; // 0xFFFFFFFF
  a1 = 0x80076DE4;
  a2 = a1 - 20; // 0xFFFFFFEC
  ra = 0x8001EA88;
  draw_skybox(a0, a1, a2);
  a1 = lw(0x80075918);
  temp = a1 == 0;
  a0 = 2; // 0x0002
  if (temp) goto label8001EAAC;
  a1 = a1 << 4;
  a2 = a1;
  a3 = a1;
  ra = 0x8001EAAC;
  function_800190D4();
label8001EAAC:

  wait_two_vsyncs();

  PutDispEnv(&backbuffer_disp->disp);
  PutDrawEnv(&backbuffer_disp->draw);
  DrawOTag(spyro_combine_all_command_buffers(0x800));

  ra = lw(sp + 0x14);
  s0 = lw(sp + 0x10);
  sp += 24; // 0x0018
}

// size: 0x000001DC
void function_8001EB80(void)
{
  struct game_object *game_object = addr_to_pointer(lw(gameobject_stack_ptr));
  sb(0x80076EF9, 0);
  sb(0x80076EFA, 0);
  sb(0x80076EFB, 0);
  sb(0x80076F7D, 0);
  sb(0x80076F7E, 0);
  sb(0x80076F7F, 0);

  create_3d_text1("RETURNING HOME...", &(vec3_32){0x76, 0x6E, 0x1100}, (vec3_32){0x10, 1, 0x1400}, 18, 11);

  text_wave_effect1(game_object, lw(0x800756F8)*2, 12, 1, 128);

  sw(0x8006FCF4 + 0x2400, 0);
  function_80018880();
  spyro_memset32(0x8006FCF4, 0, 0x900);
  function_80022A2C();

  wait_two_vsyncs();

  PutDispEnv(&backbuffer_disp->disp);
  PutDrawEnv(&backbuffer_disp->draw);
  DrawOTag(spyro_combine_all_command_buffers(0x800));
}

// size: 0x000003FC
void function_8001ED5C(void)
{
  uint32_t temp;
  sp -= 24;
  sw(sp + 0x14, ra);
  sw(sp + 0x10, s0);
  if (backbuffer_disp != addr_to_pointer(DISP1))
    backbuffer_disp = addr_to_pointer(DISP1);
  else
    backbuffer_disp = addr_to_pointer(DISP2);

  a1 = 0x0001C000;
  v0 = backbuffer_disp->unknown1;
  v1 = backbuffer_disp->unknown2;
  a0 = backbuffer_disp->unknown3;
  sw(0x800758B0, 0);
  sw(allocator1_ptr, v0);
  v0 += a1;
  sw(ordered_linked_list, v1);
  sw(linked_list1, a0);
  sw(allocator1_end, v0);
  sw(0x800756FC, v0); // &0x00000000
  sw(gameobject_stack_ptr, v0);
  ra = 0x8001EDE0;
  function_80033C50();
  v1 = lw(0x800757D8); // &0x00000000
  temp = v1 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label8001EF80;
  temp = v1 == v0;
  v0 = 2; // 0x0002
  if (temp) goto label8001EE88;
  temp = v1 == v0;
  v0 = 3; // 0x0003
  if (temp) goto label8001EE34;
  temp = v1 == v0;
  v0 = 4; // 0x0004
  if (temp) goto label8001EE34;
  temp = v1 == v0;
  v0 = 5; // 0x0005
  if (temp) goto label8001EE1C;
  temp = v1 != v0;
  v0 = 6; // 0x0006
  if (temp) goto label8001EE2C;
label8001EE1C:
  ra = 0x8001EE24;
  function_8001CA38();
  goto label8001F144;
label8001EE2C:
  temp = v1 != v0;
  v0 = 7; // 0x0007
  if (temp) goto label8001EE44;
label8001EE34:
  ra = 0x8001EE3C;
  function_8001A40C();
  goto label8001F144;
label8001EE44:
  temp = v1 != v0;
  v0 = 8; // 0x0008
  if (temp) goto label8001EE68;
  v0 = lw(0x8007567C); // &0x00000000
  temp = v0;
  ra = 0x8001EE60;
  switch (temp)
  {
  case 0x8007B68C:
    function_8007B68C();
    break;
  default:
    JALR(temp, 0x8001EE58);
  }
  goto label8001F144;
label8001EE68:
  temp = v1 != v0;
  v0 = 9; // 0x0009
  if (temp) goto label8001EE80;
  ra = 0x8001EE78;
  function_8001CFDC();
  goto label8001F144;
label8001EE80:
  temp = v1 != v0;
  v0 = 10; // 0x000A
  if (temp) goto label8001EE98;
label8001EE88:
  ra = 0x8001EE90;
  function_8001A050();
  goto label8001F144;
label8001EE98:
  v1 = lw(0x800757D8); // &0x00000000
  temp = v1 != v0;
  v0 = 11; // 0x000B
  if (temp) goto label8001EEBC;
  ra = 0x8001EEB4;
  function_8001C694();
  goto label8001F144;
label8001EEBC:
  temp = v1 != v0;
  v0 = 12; // 0x000C
  if (temp) goto label8001EED4;
  ra = 0x8001EECC;
  function_8001D718();
  goto label8001F144;
label8001EED4:
  temp = v1 != v0;
  v0 = 13; // 0x000D
  if (temp) goto label8001EEEC;
  ra = 0x8001EEE4;
  function_8001E24C();
  goto label8001F144;
label8001EEEC:
  temp = v1 != v0;
  v0 = 14; // 0x000E
  if (temp) goto label8001EF28;
  v1 = lw(0x80078D78);
  v0 = 3; // 0x0003
  temp = v1 != v0;
  if (temp) goto label8001EF18;
  ra = 0x8001EF10;
  function_8001E6B8();
  goto label8001F144;
label8001EF18:
  ra = 0x8001EF20;
  function_8007CEE4();
  goto label8001F144;
label8001EF28:
  temp = v1 != v0;
  v0 = 15; // 0x000F
  if (temp) goto label8001EF40;
  ra = 0x8001EF38;
  function_8001E9C8();
  goto label8001F144;
label8001EF40:
  temp = v1 != v0;
  if (temp) goto label8001F144;
  v0 = lw(0x80075704); // &0x00000000
  v0 = (int32_t)v0 < 99;
  temp = v0 == 0;
  if (temp) goto label8001EF70;
  ra = 0x8001EF68;
  function_8007BFD0();
  goto label8001F144;
label8001EF70:
  ra = 0x8001EF78;
  function_8001EB80();
  goto label8001F144;
label8001EF80:
  v0 = lbu(SKYBOX_DATA + 0x10);
  v1 = lbu(SKYBOX_DATA + 0x11);
  a0 = lbu(SKYBOX_DATA + 0x12);
  sb(0x80076EF9, v0);
  sb(0x80076EFA, v1);
  sb(0x80076EFB, a0);
  sb(0x80076F7D, v0);
  sb(0x80076F7E, v1);
  sb(0x80076F7F, a0);
  ra = 0x8001EFD0;
  function_800521C0();
  v0 = lw(0x80075690); // &0x00000000
  temp = v0 != 0;
  if (temp) goto label8001EFEC;
  ra = 0x8001EFEC;
  function_80019300();
label8001EFEC:
  v0 = lw(IS_DEMO_MODE); // &0x00000000
  temp = v0 == 0;
  if (temp) goto label8001F008;
  ra = 0x8001F008;
  function_80018908();
label8001F008:
  ra = 0x8001F010;
  function_80019698();
  ra = 0x8001F018;
  function_8002B9CC();
  ra = 0x8001F020;
  function_80050BD0();
  ra = 0x8001F028;
  function_800573C8();
  a1 = lw(0x80075918);
  temp = a1 == 0;
  a0 = 2; // 0x0002
  if (temp) goto label8001F04C;
  a1 = a1 << 3;
  a2 = a1;
  a3 = a1;
  ra = 0x8001F04C;
  function_800190D4();
label8001F04C:
  v0 = lw(0x8007570C); // &0x00000000
  temp = v0 != 0;
  if (temp) goto label8001F074;
  v0 = lw(0x800756C0); // &0x00000000
  temp = v0 == 0;
  if (temp) goto label8001F07C;
label8001F074:
  ra = 0x8001F07C;
  function_80018F30();
label8001F07C:
  ra = 0x8001F084;
  function_800189F0();

  wait_two_vsyncs();
  
  PutDispEnv(&backbuffer_disp->disp);
  PutDrawEnv(&backbuffer_disp->draw);
  DrawOTag(spyro_combine_all_command_buffers(0x800));

label8001F144:
  ra = lw(sp + 0x14);
  s0 = lw(sp + 0x10);
  sp += 24; // 0x0018
}

// size: 0x000001DC
void function_8007B4B0(void)
{
  uint32_t temp;
  sp -= 64; // 0xFFFFFFC0
  sw(sp + 0x2C, s1);
  s1 = a1;
  sw(sp + 0x34, s3);
  s3 = a2;
  sw(sp + 0x38, ra);
  sw(sp + 0x30, s2);
  temp = (int32_t)a0 >= 0;
  sw(sp + 0x28, s0);
  if (temp) goto label8007B508;
  v0 = lw(a1 + 0x00);
  v1 = lw(a1 + 0x04);
  v0 -= 54; // 0xFFFFFFCA
  v1 -= 5; // 0xFFFFFFFB
  sw(a1 + 0x00, v0);
  sw(a1 + 0x04, v1);
  create_3d_text2("......", addr_to_pointer(a1), 18, s3);
  v0 = 6; // 0x0006
  goto label8007B66C;
label8007B508:
  v0 = 0x1B4E81B5;
  mult(a0, v0);
  t1=hi;
  v0 = 0x2AAAAAAB;
  mult(a0, v0);
  a1=hi;
  v0 = 0x7482296B;
  mult(a0, v0);
  v1 = (int32_t)a0 >> 31;
  a0 = (int32_t)t1 >> 6;
  a0 -= v1;
  a3 = a0;
  a1 -= v1;
  v0 = a3 << 1;
  v0 += a3;
  v0 = v0 << 3;
  v0 += a3;
  v0 = v0 << 2;
  a3 = a1 - v0;
  a2=hi;
  v0 = (int32_t)a2 >> 14;
  s0 = v0 - v1;
  v0 = s0 << 4;
  v0 -= s0;
  v0 = v0 << 2;
  temp = s0 == 0;
  v0 = a0 - v0;
  if (temp) goto label8007B5A4;
  sw(sp + 0x10, a3);
  a0 = sp + 24; // 0x0018
  a1 = 0x8007AA44; // "%d %02d.%02d"
  a2 = s0;
  a3 = v0;
  ra = 0x8007B59C;
  spyro_sprintf();
  goto label8007B5B8;
label8007B5A4:
  a0 = sp + 24; // 0x0018
  a1 = 0x8007AA54; // "%d.%02d"
  a2 = v0;
  ra = 0x8007B5B8;
  spyro_sprintf();
label8007B5B8:
  a0 = sp + 24; // 0x0018
  v0 = spyro_strlen(a0);
  s2 = v0;
  a0 = sp + 24; // 0x0018
  a1 = s1;
  a2 = 18; // 0x0012
  a3 = s3;
  v0 = s2 << 3;
  v1 = lw(s1 + 0x00);
  v0 += s2;
  v1 -= v0;
  sw(s1 + 0x00, v1);
  create_3d_text2(addr_to_pointer(a0), addr_to_pointer(a1), a2, a3);
  temp = s0 == 0;
  a1 = s1;
  if (temp) goto label8007B668;
  s0 = 0x8007AA5C; // &0x0000002E
  a0 = s0;
  a2 = 18; // 0x0012
  a3 = s3;
  v0 = lw(s1 + 0x00);
  s2++;
  v0 -= 108; // 0xFFFFFF94
  sw(s1 + 0x00, v0);
  v0 = lw(s1 + 0x04);
  v1 = lw(s1 + 0x08);
  v0++;
  sw(s1 + 0x04, v0);
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 1;
  sw(s1 + 0x08, v0);
  create_3d_text2(addr_to_pointer(a0), addr_to_pointer(a1), a2, a3);
  a0 = s0;
  a1 = s1;
  a2 = 18; // 0x0012
  a3 = s3;
  v0 = lw(a1 + 0x00);
  v1 = lw(a1 + 0x04);
  v0 -= 18; // 0xFFFFFFEE
  v1 -= 8; // 0xFFFFFFF8
  sw(a1 + 0x00, v0);
  sw(a1 + 0x04, v1);
  create_3d_text2(addr_to_pointer(a0), addr_to_pointer(a1), a2, a3);
label8007B668:
  v0 = s2;
label8007B66C:
  ra = lw(sp + 0x38);
  s3 = lw(sp + 0x34);
  s2 = lw(sp + 0x30);
  s1 = lw(sp + 0x2C);
  s0 = lw(sp + 0x28);
  sp += 64; // 0x0040
}

void function_8007B68C(void)
{
  uint32_t temp;
  v0 = lw(0x800758B8);
  sp -= 144; // 0xFFFFFF70
  sw(sp + 0x88, ra);
  sw(sp + 0x84, s5);
  sw(sp + 0x80, s4);
  sw(sp + 0x7C, s3);
  sw(sp + 0x78, s2);
  sw(sp + 0x74, s1);
  temp = v0 != 0;
  sw(sp + 0x70, s0);
  if (temp) goto label8007B854;
  s1 = 0;
  ra = 0x8007B6C0;
  function_800521C0();
  s4 = 224; // 0x00E0
  ra = 0x8007B6C8;
  function_80019698();
  s2 = 512; // 0x0200
  ra = 0x8007B6D0;
  function_800573C8();
  ra = 0x8007B6D8;
  function_80050BD0();
  ra = 0x8007B6E0;
  function_8002B9CC();
  a0 = 0;
  ra = 0x8007B6E8;
  v0 = DrawSync(a0);
  a0 = 0;
  ra = 0x8007B6F0;
  v0 = VSync(a0);
  
  PutDispEnv(&backbuffer_disp->disp);
  PutDrawEnv(&backbuffer_disp->draw);
  DrawOTag(spyro_combine_all_command_buffers(0x800));

  s5 = DISP1;
  s0 = 0x800785F0;
  s3 = 0x800785E8;
  DrawSync(0);
  VSync(0);
  PutDispEnv(&backbuffer_disp->disp);
  v1 = -0x1C200;
  a0 = sp + 16; // 0x0010
  v0 = 512; // 0x0200
  sh(sp + 0x10, v0);
  v0 = 256; // 0x0100
  sh(sp + 0x12, 0);
  sh(sp + 0x14, v0);
  a1 = lw(s0 + 0x00);
  v0 = 225; // 0x00E1
  sh(sp + 0x16, v0);
  a1 += v1;
  ra = 0x8007B784;
  v0 = StoreImage(addr_to_pointer(a0), addr_to_pointer(a1));
  a2 = 8; // 0x0008
label8007B788:
  v0 = s1 << 7;
  sh(sp + 0x10, v0);

  if (backbuffer_disp != addr_to_pointer(DISP1))
    a2 = 0xF8;
  else
    a2 = 0x08;

  a0 = sp + 16; // 0x0010
  s1++;
  a1 = lw(s3 + 0x00);
  v0 = 128; // 0x0080
  sh(sp + 0x12, a2);
  sh(sp + 0x14, v0);
  sh(sp + 0x16, s4);
  ra = 0x8007B7C0;
  v0 = StoreImage(addr_to_pointer(a0), addr_to_pointer(a1));
  a0 = 0;
  ra = 0x8007B7C8;
  v0 = DrawSync(a0);
  a0 = lw(s3 + 0x00);
  a1 = 28672; // 0x7000
  ra = 0x8007B7D4;
  rgb_to_grey(addr_to_pointer(a0), a1);
  a1 = lw(s3 + 0x00);
  a0 = sp + 16; // 0x0010
  sh(sp + 0x10, s2);
  s2 += 64; // 0x0040
  v0 = 64; // 0x0040
  sh(sp + 0x12, 0);
  sh(sp + 0x14, v0);
  sh(sp + 0x16, s4);
  ra = 0x8007B7F8;
  v0 = LoadImage(addr_to_pointer(a0), addr_to_pointer(a1));
  v0 = (int32_t)s1 < 4;
  temp = v0 != 0;
  a2 = 8; // 0x0008
  if (temp) goto label8007B788;
  a0 = sp + 16; // 0x0010
  a1 = 0x8006F310;
  v0 = 512; // 0x0200
  sh(sp + 0x10, v0);
  v0 = 224; // 0x00E0
  sh(sp + 0x12, v0);
  v0 = 32; // 0x0020
  sh(sp + 0x14, v0);
  v0 = 1; // 0x0001
  sh(sp + 0x16, v0);
  ra = 0x8007B834;
  v0 = LoadImage(addr_to_pointer(a0), addr_to_pointer(a1));
  a0 = 0;
  ra = 0x8007B83C;
  v0 = DrawSync(a0);
  a0 = -1; // 0xFFFFFFFF
  ra = 0x8007B844;
  v0 = VSync(a0);
  sw(drawn_frame, v0);
  goto label8007CE90;
label8007B854:
  s5 = 0;
  if (backbuffer_disp == addr_to_pointer(DISP1))
    a0 = DISP2;
  else
    a0 = DISP1;
  
  s1 = 0;
  PutDrawEnv(addr_to_pointer(a0));
  a0 =  0x1C000;
  a1 = -0x1C200;
  v1 = lw(0x800785E8);
  v0 = lw(0x800785F0);
  sw(0x800758B0, 0);
  sw(allocator1_ptr, v1);
  v1 += a0;
  v0 += a1;
  sw(allocator1_end, v1);
  sw(0x800756FC, v0);
  sw(gameobject_stack_ptr, v0);
  v1 = s1 << 7;
label8007B8CC:
  a3 = s1 + 136; // 0x0088
  s0 = lw(allocator1_ptr);
  v0 = 0x09000000;
  sw(s0 + 0x00, v0);
  v0 = 44; // 0x002C
  sb(s0 + 0x07, v0);
  v0 = 76; // 0x004C
  sb(s0 + 0x04, v0);
  v0 = 128; // 0x0080
  sb(s0 + 0x05, v0);
  v0 = 8; // 0x0008
  sh(s0 + 0x08, v1);
  sh(s0 + 0x0A, v0);
  v0 = lhu(s0 + 0x08);
  a1 = lhu(s0 + 0x0A);
  a2 = lhu(s0 + 0x08);
  v1 = lhu(s0 + 0x0A);
  v0 += 128; // 0x0080
  sh(s0 + 0x10, v0);
  v0 = lhu(s0 + 0x08);
  v1 += 223; // 0x00DF
  sh(s0 + 0x1A, v1);
  v1 = lhu(s0 + 0x0A);
  s1++;
  sb(s0 + 0x0D, 0);
  sh(s0 + 0x12, a1);
  a1 = lbu(s0 + 0x0D);
  s2 = 64; // 0x0040
  sb(s0 + 0x0C, 0);
  sh(s0 + 0x18, a2);
  a2 = lbu(s0 + 0x0C);
  v0 += 128; // 0x0080
  sh(s0 + 0x20, v0);
  v0 = lbu(s0 + 0x0C);
  v1 += 223; // 0x00DF
  sh(s0 + 0x22, v1);
  v1 = lbu(s0 + 0x0D);
  a0 = s0;
  sb(s0 + 0x06, s2);
  sb(s0 + 0x15, a1);
  sb(s0 + 0x1C, a2);
  v0 += 128; // 0x0080
  sb(s0 + 0x14, v0);
  v0 = lbu(s0 + 0x0C);
  v1 -= 33; // 0xFFFFFFDF
  sb(s0 + 0x1D, v1);
  v1 = lbu(s0 + 0x0D);
  v0 += 128; // 0x0080
  v1 -= 33; // 0xFFFFFFDF
  sb(s0 + 0x24, v0);
  v0 = 14368; // 0x3820
  sb(s0 + 0x25, v1);
  sh(s0 + 0x0E, v0);
  sh(s0 + 0x16, a3);
  append_gpu_command_block(addr_to_pointer(a0));
  a0 = s0 + 40; // 0x0028
  v0 = (int32_t)s1 < 4;
  sw(allocator1_ptr, a0);
  temp = v0 != 0;
  v1 = s1 << 7;
  if (temp) goto label8007B8CC;
  v0 = 0x03000000;
  v1 = 231; // 0x00E7
  sw(s0 + 0x28, v0);
  v0 = 512; // 0x0200
  sb(s0 + 0x2F, s2);
  sh(s0 + 0x30, 0);
  sh(s0 + 0x32, v1);
  sh(s0 + 0x34, v0);
  sh(s0 + 0x36, v1);
  sb(s0 + 0x2C, 0);
  sb(s0 + 0x2D, 0);
  sb(s0 + 0x2E, 0);
  append_gpu_command_block(addr_to_pointer(a0));
  v0 = lw(0x80075744);
  v1 = s0 + 56; // 0x0038
  sw(allocator1_ptr, v1);
  v0 = (int32_t)v0 < 100;
  temp = v0 == 0;
  s3 = sp + 56; // 0x0038
  if (temp) goto label8007C8D8;
  v0 = lw(0x80078630);
  v1 = lw(0x80078634);
  v0 += v1;
  v1 = lw(0x80078638);
  a0 = lw(0x8007863C);
  v0 += v1;
  v0 += a0;
  v1 = 32; // 0x0020
  temp = v0 != v1;
  s4 = 0;
  if (temp) goto label8007BA7C;
  a1 = 0x8007AA60; // "COMPLETED"
  v0 = lw(a1 + 0x00);
  v1 = lw(a1 + 0x04);
  a0 = lh(a1 + 0x08);
  sw(sp + 0x18, v0);
  sw(sp + 0x1C, v1);
  sh(sp + 0x20, a0);
  s5 = 1; // 0x0001
  goto label8007BB10;
label8007BA7C:
  v0 = lw(0x80075900);
  temp = v0 == 0;
  s1 = sp + 24; // 0x0018
  if (temp) goto label8007BAB0;
  v0 = lw(0x8007AA6C); // "QUIT"
  v1 = lb(0x8007AA70); // &0x00000000
  sw(sp + 0x18, v0);
  sb(sp + 0x1C, v1);
  goto label8007BB14;
label8007BAB0:
  v0 = lw(0x80075908);
  temp = (int32_t)v0 >= 0;
  if (temp) goto label8007BAF8;
  a1 = 0x8007AA74; // "TIME IS UP"
  v0 = lw(a1 + 0x00);
  v1 = lw(a1 + 0x04);
  a0 = lh(a1 + 0x08);
  sw(sp + 0x18, v0);
  sw(sp + 0x1C, v1);
  sh(sp + 0x20, a0);
  v0 = lb(a1 + 0x0A);
  sb(sp + 0x22, v0);
  goto label8007BB14;
label8007BAF8:
  v0 = lw(0x8007AA80); // "CRASHED"
  v1 = lw(0x8007AA84); // "HED"
  sw(sp + 0x18, v0);
  sw(sp + 0x1C, v1);
label8007BB10:
  s1 = sp + 24; // 0x0018
label8007BB14:
  a0 = s1;
  ra = 0x8007BB1C;
  v0 = spyro_strlen(a0);
  a0 = s1;
  s2 = sp + 56; // 0x0038
  a1 = s2;
  a2 = 24; // 0x0018
  a3 = 11; // 0x000B
  v1 = v0 << 1;
  v1 += v0;
  v1 = v1 << 2;
  v0 = 268; // 0x010C
  v0 -= v1;
  sw(sp + 0x38, v0);
  v0 = 30; // 0x001E
  s0 = 3072; // 0x0C00
  sw(sp + 0x3C, v0);
  sw(sp + 0x40, s0);
  create_3d_text2(addr_to_pointer(a0), addr_to_pointer(a1), a2, a3);
  a1 = 0;
  a0 = lw(gameobject_stack_ptr);
  a0 -= 88; // 0xFFFFFFA8
  sw(gameobject_stack_ptr, a0);
  a2 = 88; // 0x0058
  ra = 0x8007BB80;
  spyro_memset32(a0, a1, a2);
  v1 = lw(gameobject_stack_ptr);
  v0 = 471; // 0x1D7
  sh(v1 + 0x36, 0x1D7);
  v0 = 460; // 0x01CC
  sw(v1 + 0x0C, v0);
  v0 = 40; // 0x0028
  sw(v1 + 0x10, v0);
  sw(v1 + 0x14, s0);
  a2 = 452; // 0x1C4
  sb(v1 + 0x45, lh(spyro_cos_lut + lw(0x80077FE4)*2));
  sb(lw(gameobject_stack_ptr) + 0x46, lw(0x80077FE4));
  sb(lw(gameobject_stack_ptr) + 0x47, 0x7F);
  sb(lw(gameobject_stack_ptr) + 0x4F, 0x0B);
  sb(lw(gameobject_stack_ptr) + 0x50, 0xFF);
  gui_line(0x3C, 0x2E, 0x1C4, 0x2E);
  s3 = 60; // 0x003C
  sw(sp + 0x38, 0x64);
  s0 = 4352; // 0x1100
  sw(sp + 0x3C, s3);
  sw(sp + 0x40, 0x1100);
  a0 = lw(0x8006E8C0 + lw(WORLD_ID)*16);
  create_3d_text2(addr_to_pointer(a0), addr_to_pointer(s2), 18, 11);
  v0 = lw(0x80075744);
  temp = (int32_t)v0 <= 0;
  v0 = (int32_t)v0 < 2;
  if (temp) goto label8007BCC0;
  a2 = lw(0x80078630);
  a1 = 0x8007AA88; // "%d/8"
  a0 = s1;
  ra = 0x8007BC8C;
  spyro_sprintf();
  a0 = s1;
  a1 = s2;
  a2 = 18; // 0x0012
  a3 = 11; // 0x000B
  v0 = 320; // 0x0140
  sw(sp + 0x38, v0);
  sw(sp + 0x3C, s3);
  sw(sp + 0x40, 0x1100);
  create_3d_text2(addr_to_pointer(a0), addr_to_pointer(a1), 18, 11);
  v0 = lw(0x80075744);
  v0 = (int32_t)v0 < 2;
label8007BCC0:
  temp = v0 != 0;
  v0 = 8; // 0x0008
  if (temp) goto label8007BE14;
  v1 = lw(0x80078630);
  temp = v1 != v0;
  if (temp) goto label8007BD70;
  v0 = lw(WORLD_ID);
  v1 = v0 << 2;
  v1 += v0;
  v0 = lbu(0x80078680 + v1);
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label8007BD20;
  a1 = 0x8007AA90; // &0x0000005E
  a0 = s1;
  ra = 0x8007BD18;
  spyro_sprintf();
  s1 = 0;
  goto label8007BD84;
label8007BD20:
  a2 = lw(0x8006E920 + v0);
  a1 = 0x8007AA94; // "%d"
  a0 = s1;
  ra = 0x8007BD3C;
  spyro_sprintf();
  v1 = lw(WORLD_ID);
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 2;
  v0 = lw(0x8006E920 + v0);
  s4 += v0;
  s1 = 0;
  goto label8007BD84;
label8007BD70:
  a1 = 0x8007AA98; // &0x00000030
  a0 = s1;
  ra = 0x8007BD80;
  spyro_sprintf();
  s1 = 33; // 0x0021
label8007BD84:
  s0 = sp + 24; // 0x0018
  a0 = s0;
  ra = 0x8007BD90;
  v0 = spyro_strlen(a0);
  a0 = s0;
  a1 = sp + 56; // 0x0038
  a2 = 18; // 0x0012
  a3 = 11; // 0x000B
  v1 = v0 << 3;
  v1 += v0;
  v1 = v1 << 1;
  v0 = 480; // 0x01E0
  v0 -= v1;
  sw(sp + 0x38, v0);
  v0 = 60; // 0x003C
  sw(sp + 0x3C, v0);
  v0 = 4352; // 0x1100
  sw(sp + 0x40, v0);
  create_3d_text2(addr_to_pointer(a0), addr_to_pointer(a1), 18, 11);
  v0 = lw(0x8007569C);
  v0 = (int32_t)v0 < 2;
  temp = v0 == 0;
  a2 = 16; // 0x0010
  if (temp) goto label8007BE14;
  a1 = spyro_position_x;
  v0 = lw(0x800761D4);
  v0 += s1;
  a0 = lbu(v0 + 0x00);
  a3 = 0;
  ra = 0x8007BE08;
  function_80055A78();
  v0 = 2; // 0x0002
  sw(0x8007569C, v0);
label8007BE14:
  s1 = sp + 56; // 0x0038
  v0 = 100; // 0x0064
  s3 = 80; // 0x0050
  sw(sp + 0x38, v0);
  v0 = lw(WORLD_ID);
  s2 = 4352; // 0x1100
  sw(sp + 0x3C, s3);
  sw(sp + 0x40, s2);
  v0 = v0 << 4;
  a0 = lw(0x8006E8C4 + v0);
  create_3d_text2(addr_to_pointer(a0), addr_to_pointer(s1), 18, 11);
  v0 = lw(0x80075744);
  v0 = (int32_t)v0 < 3;
  temp = v0 != 0;
  s0 = sp + 24; // 0x0018
  if (temp) goto label8007BEAC;
  a2 = lw(0x80078634);
  a1 = 0x8007AA88; // "%d/8"
  a0 = s0;
  ra = 0x8007BE88;
  spyro_sprintf();
  v0 = 320; // 0x0140
  sw(sp + 0x38, v0);
  sw(sp + 0x3C, s3);
  sw(sp + 0x40, s2);
  create_3d_text2(addr_to_pointer(s0), addr_to_pointer(s1), 18, 11);
label8007BEAC:
  v0 = lw(0x80075744);
  v0 = (int32_t)v0 < 4;
  temp = v0 != 0;
  v0 = 8; // 0x0008
  if (temp) goto label8007C010;
  v1 = lw(0x80078634);
  temp = v1 != v0;
  if (temp) goto label8007BF6C;
  v0 = lw(WORLD_ID);
  v1 = v0 << 2;
  v1 += v0;
  v0 = lbu(0x80078681 + v1);
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label8007BF1C;
  a1 = 0x8007AA90; // &0x0000005E
  a0 = sp + 24; // 0x0018
  ra = 0x8007BF14;
  spyro_sprintf();
  s1 = 0;
  goto label8007BF80;
label8007BF1C:
  a2 = lw(0x8006E924 + v0);
  a1 = 0x8007AA94; // "%d"
  a0 = sp + 24; // 0x0018
  ra = 0x8007BF38;
  spyro_sprintf();
  v1 = lw(WORLD_ID);
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 2;
  v0 = lw(0x8006E924 + v0);
  s4 += v0;
  s1 = 0;
  goto label8007BF80;
label8007BF6C:
  a1 = 0x8007AA98; // &0x00000030
  a0 = sp + 24; // 0x0018
  ra = 0x8007BF7C;
  spyro_sprintf();
  s1 = 33; // 0x0021
label8007BF80:
  s0 = sp + 24; // 0x0018
  a0 = s0;
  ra = 0x8007BF8C;
  v0 = spyro_strlen(a0);
  a0 = s0;
  a1 = sp + 56; // 0x0038
  a2 = 18; // 0x0012
  a3 = 11; // 0x000B
  v1 = v0 << 3;
  v1 += v0;
  v1 = v1 << 1;
  v0 = 480; // 0x01E0
  v0 -= v1;
  sw(sp + 0x38, v0);
  v0 = 80; // 0x0050
  sw(sp + 0x3C, v0);
  v0 = 4352; // 0x1100
  sw(sp + 0x40, v0);
  create_3d_text2(addr_to_pointer(a0), addr_to_pointer(a1), a2, a3);
  v0 = lw(0x8007569C);
  v0 = (int32_t)v0 < 4;
  temp = v0 == 0;
  a2 = 16; // 0x0010
  if (temp) goto label8007C010;
  a1 = spyro_position_x;
  v0 = lw(0x800761D4);
  v0 += s1;
  a0 = lbu(v0 + 0x00);
  a3 = 0;
  ra = 0x8007C004;
  function_80055A78();
  v0 = 4; // 0x0004
  sw(0x8007569C, v0);
label8007C010:
  s2 = sp + 56; // 0x0038
  a1 = s2;
  a2 = 18; // 0x0012
  s1 = 100; // 0x0064
  v0 = lw(WORLD_ID);
  s3 = 4352; // 0x1100
  sw(sp + 0x38, s1);
  sw(sp + 0x3C, s1);
  sw(sp + 0x40, s3);
  v0 = v0 << 4;
  a0 = lw(0x8006E8C8 + v0);
  a3 = 11; // 0x000B
  create_3d_text2(addr_to_pointer(a0), addr_to_pointer(a1), a2, a3);
  v0 = lw(0x80075744);
  v0 = (int32_t)v0 < 5;
  temp = v0 != 0;
  s0 = sp + 24; // 0x0018
  if (temp) goto label8007C0A4;
  a2 = lw(0x80078638);
  a1 = 0x8007AA88; // "%d/8"
  a0 = s0;
  ra = 0x8007C080;
  spyro_sprintf();
  a0 = s0;
  a1 = s2;
  a2 = 18; // 0x0012
  a3 = 11; // 0x000B
  v0 = 320; // 0x0140
  sw(sp + 0x38, v0);
  sw(sp + 0x3C, s1);
  sw(sp + 0x40, s3);
  create_3d_text2(addr_to_pointer(a0), addr_to_pointer(a1), a2, a3);
label8007C0A4:
  v0 = lw(0x80075744);
  v0 = (int32_t)v0 < 6;
  temp = v0 != 0;
  s1 = sp + 56; // 0x0038
  if (temp) goto label8007C20C;
  v1 = lw(0x80078638);
  v0 = 8; // 0x0008
  temp = v1 != v0;
  if (temp) goto label8007C164;
  v0 = lw(WORLD_ID);
  v1 = v0 << 2;
  v1 += v0;
  v0 = lbu(0x80078682 + v1);
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label8007C114;
  a1 = 0x8007AA90; // &0x0000005E
  a0 = sp + 24; // 0x0018
  ra = 0x8007C10C;
  spyro_sprintf();
  s1 = 0;
  goto label8007C178;
label8007C114:
  a2 = lw(0x8006E928 + v0);
  a1 = 0x8007AA94; // "%d"
  a0 = sp + 24; // 0x0018
  ra = 0x8007C130;
  spyro_sprintf();
  v1 = lw(WORLD_ID);
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 2;
  v0 = lw(0x8006E928 + v0);
  s4 += v0;
  s1 = 0;
  goto label8007C178;
label8007C164:
  a1 = 0x8007AA98; // &0x00000030
  a0 = sp + 24; // 0x0018
  ra = 0x8007C174;
  spyro_sprintf();
  s1 = 33; // 0x0021
label8007C178:
  s0 = sp + 24; // 0x0018
  a0 = s0;
  ra = 0x8007C184;
  v0 = spyro_strlen(a0);
  a0 = s0;
  a1 = sp + 56; // 0x0038
  a2 = 18; // 0x0012
  a3 = 11; // 0x000B
  v1 = v0 << 3;
  v1 += v0;
  v1 = v1 << 1;
  v0 = 480; // 0x01E0
  v0 -= v1;
  sw(sp + 0x38, v0);
  v0 = 100; // 0x0064
  sw(sp + 0x3C, v0);
  v0 = 4352; // 0x1100
  sw(sp + 0x40, v0);
  create_3d_text2(addr_to_pointer(a0), addr_to_pointer(a1), a2, a3);
  v0 = lw(0x8007569C);
  v0 = (int32_t)v0 < 6;
  temp = v0 == 0;
  a2 = 16; // 0x0010
  if (temp) goto label8007C208;
  a1 = spyro_position_x;
  v0 = lw(0x800761D4);
  v0 += s1;
  a0 = lbu(v0 + 0x00);
  a3 = 0;
  ra = 0x8007C1FC;
  function_80055A78();
  v0 = 6; // 0x0006
  sw(0x8007569C, v0);
label8007C208:
  s1 = sp + 56; // 0x0038
label8007C20C:
  a1 = s1;
  a2 = 18; // 0x0012
  v0 = 100; // 0x0064
  s3 = 120; // 0x0078
  sw(sp + 0x38, v0);
  v0 = lw(WORLD_ID);
  s2 = 4352; // 0x1100
  sw(sp + 0x3C, s3);
  sw(sp + 0x40, s2);
  v0 = v0 << 4;
  a0 = lw(0x8006E8CC + v0);
  a3 = 11; // 0x000B
  create_3d_text2(addr_to_pointer(a0), addr_to_pointer(a1), 18, 11);
  v0 = lw(0x80075744);
  v0 = (int32_t)v0 < 7;
  temp = v0 != 0;
  s0 = sp + 24; // 0x0018
  if (temp) goto label8007C2A0;
  a2 = lw(0x8007863C);
  a1 = 0x8007AA88; // "%d/8"
  a0 = s0;
  ra = 0x8007C27C;
  spyro_sprintf();
  a0 = s0;
  a1 = s1;
  a2 = 18; // 0x0012
  a3 = 11; // 0x000B
  v0 = 320; // 0x0140
  sw(sp + 0x38, v0);
  sw(sp + 0x3C, s3);
  sw(sp + 0x40, s2);
  create_3d_text2(addr_to_pointer(s0), addr_to_pointer(s1), 18, 11);
label8007C2A0:
  v0 = lw(0x80075744);
  v0 = (int32_t)v0 < 8;
  temp = v0 != 0;
  v0 = 8; // 0x0008
  if (temp) goto label8007C404;
  v1 = lw(0x8007863C);
  temp = v1 != v0;
  if (temp) goto label8007C360;
  v0 = lw(WORLD_ID);
  v1 = v0 << 2;
  v1 += v0;
  v0 = lbu(0x80078683 + v1);
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label8007C310;
  a1 = 0x8007AA90; // &0x0000005E
  a0 = sp + 24; // 0x0018
  ra = 0x8007C308;
  spyro_sprintf();
  s1 = 0;
  goto label8007C374;
label8007C310:
  a2 = lw(0x8006E92C + v0);
  a1 = 0x8007AA94; // "%d"
  a0 = sp + 24; // 0x0018
  ra = 0x8007C32C;
  spyro_sprintf();
  v1 = lw(WORLD_ID);
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 2;
  v0 = lw(0x8006E92C + v0);
  s4 += v0;
  s1 = 0;
  goto label8007C374;
label8007C360:
  a1 = 0x8007AA98; // &0x00000030
  a0 = sp + 24; // 0x0018
  ra = 0x8007C370;
  spyro_sprintf();
  s1 = 33; // 0x0021
label8007C374:
  s0 = sp + 24; // 0x0018
  a0 = s0;
  ra = 0x8007C380;
  v0 = spyro_strlen(a0);
  a0 = s0;
  a1 = sp + 56; // 0x0038
  a2 = 18; // 0x0012
  a3 = 11; // 0x000B
  v1 = v0 << 3;
  v1 += v0;
  v1 = v1 << 1;
  v0 = 480; // 0x01E0
  v0 -= v1;
  sw(sp + 0x38, v0);
  v0 = 120; // 0x0078
  sw(sp + 0x3C, v0);
  v0 = 4352; // 0x1100
  sw(sp + 0x40, v0);
  create_3d_text2(addr_to_pointer(s0), addr_to_pointer(a1), 18, 11);
  v0 = lw(0x8007569C);
  v0 = (int32_t)v0 < 8;
  temp = v0 == 0;
  a2 = 16; // 0x0010
  if (temp) goto label8007C404;
  a1 = spyro_position_x;
  v0 = lw(0x800761D4);
  v0 += s1;
  a0 = lbu(v0 + 0x00);
  a3 = 0;
  ra = 0x8007C3F8;
  function_80055A78();
  v0 = 8; // 0x0008
  sw(0x8007569C, v0);
label8007C404:
  a1 = sp + 56; // 0x0038
  a2 = 18; // 0x0012
  a3 = 11; // 0x000B
  v0 = 100; // 0x0064
  sw(sp + 0x38, v0);
  v0 = 140; // 0x008C
  sw(sp + 0x3C, v0);
  v0 = 4352; // 0x1100
  sw(sp + 0x40, v0);
  create_3d_text2("ALL IN ONE", addr_to_pointer(a1), 18, 11);
  v0 = lw(0x80075744);
  v0 = (int32_t)v0 < 9;
  temp = v0 != 0;
  a0 = 416; // 0x01A0
  if (temp) goto label8007C5C4;
  v0 = lw(0x80078630);
  v1 = lw(0x80078634);
  v0 += v1;
  v1 = lw(0x80078638);
  a0 = lw(0x8007863C);
  v0 += v1;
  v0 += a0;
  v1 = 32; // 0x0020
  temp = v0 != v1;
  if (temp) goto label8007C51C;
  v0 = lw(WORLD_ID);
  v1 = v0 << 2;
  v1 += v0;
  v0 = lbu(0x80078684 + v1);
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label8007C4CC;
  a1 = 0x8007AA90; // &0x0000005E
  a0 = sp + 24; // 0x0018
  ra = 0x8007C4C4;
  spyro_sprintf();
  s1 = 0;
  goto label8007C530;
label8007C4CC:
  a2 = lw(0x8006E930 + v0);
  a1 = 0x8007AA94; // "%d"
  a0 = sp + 24; // 0x0018
  ra = 0x8007C4E8;
  spyro_sprintf();
  v1 = lw(WORLD_ID);
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 2;
  v0 = lw(0x8006E930 + v0);
  s4 += v0;
  s1 = 0;
  goto label8007C530;
label8007C51C:
  a1 = 0x8007AA98; // &0x00000030
  a0 = sp + 24; // 0x0018
  ra = 0x8007C52C;
  spyro_sprintf();
  s1 = 33; // 0x0021
label8007C530:
  s0 = sp + 24; // 0x0018
  a0 = s0;
  ra = 0x8007C53C;
  v0 = spyro_strlen(a0);
  a0 = s0;
  a1 = sp + 56; // 0x0038
  v1 = v0 << 3;
  v1 += v0;
  v1 = v1 << 1;
  v0 = 480; // 0x01E0
  v0 -= v1;
  sw(sp + 0x38, v0);
  v0 = 140; // 0x008C
  sw(sp + 0x3C, v0);
  v0 = 4352; // 0x1100
  sw(sp + 0x40, v0);
  create_3d_text2(addr_to_pointer(a0), addr_to_pointer(a1), 18, 11);
  v0 = lw(0x8007569C);
  v0 = (int32_t)v0 < 9;
  temp = v0 == 0;
  if (temp) goto label8007C5C0;
  a1 = spyro_position_x;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  v0 += s1;
  a0 = lbu(v0 + 0x00);
  a3 = 0;
  ra = 0x8007C5B4;
  function_80055A78();
  v0 = 9; // 0x0009
  sw(0x8007569C, v0);
label8007C5C0:
  a0 = 416; // 0x01A0
label8007C5C4:
  a1 = 150; // 0x0096
  a2 = 470; // 0x01D6
  a3 = 150; // 0x0096
  ra = 0x8007C5D4;
  gui_line(a0, a1, a2, a3);
  a0 = 0x8007AAA8; // "TOTAL"
  s2 = sp + 56; // 0x0038
  a1 = s2;
  a2 = 18; // 0x0012
  a3 = 11; // 0x000B
  v0 = 100; // 0x0064
  s1 = 160; // 0x00A0
  s3 = 4352; // 0x1100
  sw(sp + 0x38, v0);
  sw(sp + 0x3C, s1);
  sw(sp + 0x40, s3);
  create_3d_text2(addr_to_pointer(a0), addr_to_pointer(a1), 18, 11);
  s0 = sp + 24; // 0x0018
  a0 = s0;
  a1 = 0x8007AA94; // "%d"
  a2 = s4;
  ra = 0x8007C620;
  spyro_sprintf();
  a0 = s0;
  ra = 0x8007C628;
  v0 = spyro_strlen(a0);
  a0 = s0;
  a1 = s2;
  v1 = v0 << 3;
  v1 += v0;
  v1 = v1 << 1;
  v0 = 480; // 0x01E0
  v0 -= v1;
  sw(sp + 0x38, v0);
  sw(sp + 0x3C, s1);
  sw(sp + 0x40, s3);
  create_3d_text2(addr_to_pointer(a0), addr_to_pointer(a1), 18, 11);
  a0 = 60; // 0x003C
  a1 = 176; // 0x00B0
  a2 = 452; // 0x01C4
  a3 = 176; // 0x00B0
  ra = 0x8007C670;
  gui_line(a0, a1, a2, a3);
  v0 = lw(0x80075744);
  v0 = (int32_t)v0 < 9;
  temp = v0 != 0;
  if (temp) goto label8007C8A4;
  temp = s5 != 0;
  if (temp) goto label8007C6B8;
  v0 = lw(0x80078618 + lw(WORLD_ID)*4);
  temp = v0 == 0;
  if (temp) goto label8007C70C;
label8007C6B8:
  sw(sp + 0x38, 0x50);
  sw(sp + 0x3C, 0xD2);
  sw(sp + 0x40, s3);
  create_3d_text2("PRESS   TO CONTINUE", addr_to_pointer(s2), 18, 11);
  sw(sp + 0x38, 0xBC);
  create_3d_text2("X", addr_to_pointer(s2), 18, 10);
  goto label8007C8A4;
label8007C70C:
  s0 = 190; // 0x00BE
  sw(sp + 0x38, 0x8C);
  sw(sp + 0x3C, 0xBE);
  sw(sp + 0x40, s3);
  create_3d_text2("TRY AGAIN?", addr_to_pointer(s2), 18, 11);
  a3 = 0;
  sw(sp + 0x38, 0x15C);
  v0 = lw(0x80075720);
  a1 = s2;
  sw(sp + 0x3C, 0xBE);
  temp = v0 != 0;
  sw(sp + 0x40, s3);
  if (temp) goto label8007C78C;
  v1 = lw(0x8007568C);
  temp = (int32_t)v1 >= 0;
  v0 = v1;
  if (temp) goto label8007C770;
  v0 = v1 + 15; // 0x000F
label8007C770:
  v0 = (int32_t)v0 >> 4;
  v0 = v0 << 4;
  v0 = v1 - v0;
  v0 = (int32_t)v0 < 6;
  temp = v0 == 0;
  if (temp) goto label8007C78C;
  a3 = 1; // 0x0001
label8007C78C:
  a3 = a3 << 1;
  a3 += 10; // 0x000A
  create_3d_text2("YES", addr_to_pointer(a1), 18, a3);
  s2 = 3; // 0x0003
  a3 = 0;
  v0 = 348; // 0x015C
  sw(sp + 0x38, v0);
  v0 = 210; // 0x00D2
  sw(sp + 0x3C, v0);
  v0 = 4352; // 0x1100
  sw(sp + 0x40, v0);
  v0 = 1; // 0x0001
  v1 = lw(0x80075720);
  s0 = lw(gameobject_stack_ptr);
  temp = v1 != v0;
  a1 = sp + 56; // 0x0038
  if (temp) goto label8007C814;
  v1 = lw(0x8007568C);
  temp = (int32_t)v1 >= 0;
  v0 = v1;
  if (temp) goto label8007C7F8;
  v0 = v1 + 15; // 0x000F
label8007C7F8:
  v0 = (int32_t)v0 >> 4;
  v0 = v0 << 4;
  v0 = v1 - v0;
  v0 = (int32_t)v0 < 6;
  temp = v0 == 0;
  if (temp) goto label8007C814;
  a3 = 1; // 0x0001
label8007C814:
  a3 = a3 << 1;
  a3 += 10; // 0x000A
  create_3d_text2("NO", addr_to_pointer(a1), 18, a3);
  v0 = lw(0x80075720);
  temp = v0 == 0;
  if (temp) goto label8007C84C;
  s0 = lw(gameobject_stack_ptr);
  s2 = 2; // 0x0002
label8007C84C:
  temp = (int32_t)s2 <= 0;
  s1 = 0;
  if (temp) goto label8007C8A4;
  a1 = spyro_cos_lut;
  a0 = 0;
label8007C860:
  v0 = lw(0x800758B8);
  s1++;
  v0 = v0 << 3;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a1;
  v1 = lh(v0 + 0x00);
  a0 += 12; // 0x000C
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 9;
  sb(s0 + 0x46, v0);
  v0 = (int32_t)s1 < (int32_t)s2;
  temp = v0 != 0;
  s0 += 88; // 0x0058
  if (temp) goto label8007C860;
label8007C8A4:
  v1 = 0x8006FCF4 + 0x2400;
  s1 = 0;
  a0 = 0x80077FEC;
label8007C8B8:
  sw(v1 + 0x00, a0);
  v1 += 4; // 0x0004
  s1++;
  v0 = (int32_t)s1 < 4;
  temp = v0 != 0;
  a0 += 88; // 0x0058
  if (temp) goto label8007C8B8;
  sw(v1 + 0x00, 0);
  goto label8007CE90;
label8007C8D8:
  a1 = s3;
  v0 = 136; // 0x0088
  sw(sp + 0x38, v0);
  v0 = 40; // 0x0028
  sw(sp + 0x3C, v0);
  v0 = 3072; // 0x0C00
  sw(sp + 0x40, v0);
  create_3d_text2("TIME ATTACK", addr_to_pointer(a1), 24, 11);
  a0 = 60; // 0x003C
  a1 = 61; // 0x003D
  a2 = 452; // 0x01C4
  a3 = 61; // 0x003D
  ra = 0x8007C91C;
  gui_line(a0, a1, a2, a3);
  v0 = 320; // 0x0140
  s2 = 80; // 0x0050
  s0 = 4352; // 0x1100
  sw(sp + 0x38, v0);
  sw(sp + 0x3C, s2);
  sw(sp + 0x40, s0);
  create_3d_text2("BEST TIME", addr_to_pointer(s3), 18, 11);
  a1 = s3;
  v0 = 401; // 0x0191
  sw(sp + 0x38, v0);
  v0 = lw(WORLD_ID);
  s1 = 100; // 0x0064
  sw(sp + 0x3C, s1);
  sw(sp + 0x40, s0);
  v0 = v0 << 2;
  a0 = lw(0x80078618 + v0);
  v0 = lw(0x800758C4);
  temp = v0;
  a2 = 11; // 0x000B
  ra = 0x8007C990;
  switch (temp)
  {
  case 0x8007B4B0:
    function_8007B4B0();
    break;
  default:
    JALR(temp, 0x8007C988);
  }
  v0 = 56; // 0x0038
  sw(sp + 0x38, v0);
  sw(sp + 0x3C, s2);
  sw(sp + 0x40, s0);
  create_3d_text2("YOUR TIME", addr_to_pointer(s3), 18, 11);
  v1 = lw(WORLD_ID);
  a1 = lw(0x800758F4);
  v0 = 137; // 0x0089
  sw(sp + 0x38, v0);
  sw(sp + 0x3C, s1);
  sw(sp + 0x40, s0);
  v1 = v1 << 2;
  v0 = lw(0x80078618 + v1);
  v0 = (int32_t)v0 < (int32_t)a1;
  temp = v0 != 0;
  s1 = 11; // 0x000B
  if (temp) goto label8007C9FC;
  s1 = 8; // 0x0008
label8007C9FC:
  v0 = lw(0x80078630);
  v1 = lw(0x80078634);
  v0 += v1;
  v1 = lw(0x80078638);
  a0 = lw(0x8007863C);
  v0 += v1;
  v0 += a0;
  v1 = 32; // 0x0020
  temp = v0 != v1;
  a0 = -1; // 0xFFFFFFFF
  if (temp) goto label8007CAC0;
  a0 = a1;
  a1 = s3;
  a2 = s1;
  v0 = lw(0x800758C4);
  temp = v0;
  s1 = 0;
  ra = 0x8007CA58;
  switch (temp)
  {
  case 0x8007B4B0:
    function_8007B4B0();
    break;
  default:
    JALR(temp, 0x8007CA50);
  }
  s2 = v0;
  s0 = lw(gameobject_stack_ptr);
  temp = (int32_t)s2 <= 0;
  a0 = 0;
  if (temp) goto label8007CAD8;
  a1 = spyro_cos_lut;
label8007CA74:
  v0 = lw(0x800758B8);
  s1++;
  v0 = v0 << 3;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a1;
  v1 = lh(v0 + 0x00);
  a0 += 12; // 0x000C
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 9;
  sb(s0 + 0x46, v0);
  v0 = (int32_t)s1 < (int32_t)s2;
  temp = v0 != 0;
  s0 += 88; // 0x0058
  if (temp) goto label8007CA74;
  goto label8007CAD8;
label8007CAC0:
  a1 = s3;
  v0 = lw(0x800758C4);
  temp = v0;
  a2 = 11; // 0x000B
  ra = 0x8007CAD8;
  switch (temp)
  {
  case 0x8007B4B0:
    function_8007B4B0();
    break;
  default:
    JALR(temp, 0x8007CAD0);
  }
label8007CAD8:
  v0 = lw(0x80078630);
  v1 = lw(0x80078634);
  v0 += v1;
  v1 = lw(0x80078638);
  a0 = lw(0x8007863C);
  v0 += v1;
  v0 += a0;
  v0 = (int32_t)v0 < 32;
  temp = v0 == 0;
  if (temp) goto label8007CC10;
  v0 = lw(0x80075900);
  temp = v0 == 0;
  s0 = sp + 0x18;
  if (temp) goto label8007CB48;
  sw(s0 + 0x00, lw(0x8007AA6C)); // "QUIT"
  sb(s0 + 0x04, lb(0x8007AA70)); // "\0"
  goto label8007CB60;
label8007CB48:
  sw(s0 + 0x00, lw(0x8007AA80)); // "CRAS"
  sw(s0 + 0x04, lw(0x8007AA84)); // "HED"
label8007CB60:
  s2 = spyro_strlen(s0);
  sw(sp + 0x38, 0x10C - s2*12);
  sw(sp + 0x3C, 0x96);
  sw(sp + 0x40, 0xC00);
  create_3d_text2(addr_to_pointer(s0), addr_to_pointer(sp + 0x38), 24, 11);
  s0 = lw(gameobject_stack_ptr);
  temp = (int32_t)s2 <= 0;
  s1 = 0;
  if (temp) goto label8007CCD0;
  a1 = spyro_cos_lut;
  a0 = 0;
label8007CBC4:
  v0 = lw(0x800758B8);
  s1++;
  v0 = v0 << 3;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a1;
  v1 = lh(v0 + 0x00);
  a0 += 12; // 0x000C
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 9;
  sb(s0 + 0x46, v0);
  v0 = (int32_t)s1 < (int32_t)s2;
  temp = v0 != 0;
  s0 += 88; // 0x0058
  if (temp) goto label8007CBC4;
  a0 = 60; // 0x003C
  goto label8007CCD4;
label8007CC10:
  v0 = lw(WORLD_ID);
  v1 = lw(0x800758F4);
  v0 = v0 << 2;
  v0 = lw(0x80078618 + v0);
  v0 = (int32_t)v0 < (int32_t)v1;
  temp = v0 != 0;
  a0 = 60; // 0x003C
  if (temp) goto label8007CCD4;
  a1 = sp + 56; // 0x0038
  v0 = 148; // 0x0094
  sw(sp + 0x38, v0);
  v0 = 150; // 0x0096
  sw(sp + 0x3C, v0);
  v0 = 3072; // 0x0C00
  sw(sp + 0x40, v0);
  create_3d_text2("NEW RECORD", addr_to_pointer(a1), 24, 8);
  s2 = 10; // 0x000A
  s1 = 0;
  a1 = spyro_cos_lut;
  a0 = 0;
  s0 = lw(gameobject_stack_ptr);
label8007CC8C:
  v0 = lw(0x800758B8);
  s1++;
  v0 = v0 << 3;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a1;
  v1 = lh(v0 + 0x00);
  a0 += 12; // 0x000C
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 9;
  sb(s0 + 0x46, v0);
  v0 = (int32_t)s1 < (int32_t)s2;
  temp = v0 != 0;
  s0 += 88; // 0x0058
  if (temp) goto label8007CC8C;
label8007CCD0:
  a0 = 60; // 0x003C
label8007CCD4:
  a1 = 171; // 0x00AB
  a2 = 452; // 0x01C4
  a3 = 171; // 0x00AB
  ra = 0x8007CCE4;
  gui_line(a0, a1, a2, a3);
  s2 = sp + 56; // 0x0038
  v0 = 140; // 0x008C
  s1 = 190; // 0x00BE
  s0 = 4352; // 0x1100
  sw(sp + 0x38, v0);
  sw(sp + 0x3C, s1);
  sw(sp + 0x40, s0);
  create_3d_text2("TRY AGAIN", addr_to_pointer(s2), 18, 11);
  a3 = 0;
  v0 = 340; // 0x0154
  sw(sp + 0x38, v0);
  v0 = lw(0x80075720);
  a1 = s2;
  sw(sp + 0x3C, s1);
  temp = v0 != 0;
  sw(sp + 0x40, s0);
  if (temp) goto label8007CD70;
  v1 = lw(0x8007568C);
  temp = (int32_t)v1 >= 0;
  v0 = v1;
  if (temp) goto label8007CD54;
  v0 = v1 + 15; // 0x000F
label8007CD54:
  v0 = (int32_t)v0 >> 4;
  v0 = v0 << 4;
  v0 = v1 - v0;
  v0 = (int32_t)v0 < 6;
  temp = v0 == 0;
  if (temp) goto label8007CD70;
  a3 = 1; // 0x0001
label8007CD70:
  a3 = a3 << 1;
  a3 += 10; // 0x000A
  create_3d_text2("YES", addr_to_pointer(a1), 18, a3);
  s2 = 3; // 0x0003
  a3 = 0;
  v0 = 340; // 0x0154
  sw(sp + 0x38, v0);
  v0 = 210; // 0x00D2
  sw(sp + 0x3C, v0);
  v0 = 4352; // 0x1100
  sw(sp + 0x40, v0);
  v0 = 1; // 0x0001
  v1 = lw(0x80075720);
  s0 = lw(gameobject_stack_ptr);
  temp = v1 != v0;
  a1 = sp + 56; // 0x0038
  if (temp) goto label8007CDF8;
  v1 = lw(0x8007568C);
  temp = (int32_t)v1 >= 0;
  v0 = v1;
  if (temp) goto label8007CDDC;
  v0 = v1 + 15; // 0x000F
label8007CDDC:
  v0 = (int32_t)v0 >> 4;
  v0 = v0 << 4;
  v0 = v1 - v0;
  v0 = (int32_t)v0 < 6;
  temp = v0 == 0;
  if (temp) goto label8007CDF8;
  a3 = 1; // 0x0001
label8007CDF8:
  a3 = a3 << 1;
  a3 += 10; // 0x000A
  create_3d_text2("NO", addr_to_pointer(a1), 18, a3);
  v0 = lw(0x80075720);
  temp = v0 == 0;
  if (temp) goto label8007CE30;
  s0 = lw(gameobject_stack_ptr);
  s2 = 2; // 0x0002
label8007CE30:
  temp = (int32_t)s2 <= 0;
  s1 = 0;
  if (temp) goto label8007CE88;
  a1 = spyro_cos_lut;
  a0 = 0;
label8007CE44:
  v0 = lw(0x800758B8);
  s1++;
  v0 = v0 << 3;
  v0 += a0;
  v0 = v0 & 0xFF;
  v0 = v0 << 1;
  v0 += a1;
  v1 = lh(v0 + 0x00);
  a0 += 12; // 0x000C
  v0 = v1 << 1;
  v0 += v1;
  v0 = (int32_t)v0 >> 9;
  sb(s0 + 0x46, v0);
  v0 = (int32_t)s1 < (int32_t)s2;
  temp = v0 != 0;
  s0 += 88; // 0x0058
  if (temp) goto label8007CE44;
label8007CE88:
  sw(0x8006FCF4 + 0x2400, 0);
label8007CE90:
  v0 = lw(0x800758B8);
  temp = (int32_t)v0 <= 0;
  if (temp) goto label8007CEC8;
  ra = 0x8007CEAC;
  function_80018880();
  a0 = 0x8006FCF4;
  a1 = 0;
  a2 = 2304; // 0x0900
  ra = 0x8007CEC0;
  spyro_memset32(a0, a1, a2);
  ra = 0x8007CEC8;
  function_80022A2C();
label8007CEC8:
  DrawOTag(spyro_combine_all_command_buffers(0));
  
  wait_two_vsyncs();

  PutDispEnv(&backbuffer_disp->disp);
  v0 = lw(0x800758B8);
  v0++;
  sw(0x800758B8, v0);
  ra = lw(sp + 0x88);
  s5 = lw(sp + 0x84);
  s4 = lw(sp + 0x80);
  s3 = lw(sp + 0x7C);
  s2 = lw(sp + 0x78);
  s1 = lw(sp + 0x74);
  s0 = lw(sp + 0x70);
  sp += 144; // 0x0090
}

// size: 0x00000F04
void function_8007CEE4_title_screen(void)
{
  uint32_t temp;
  sp -= 80; // 0xFFFFFFB0
  sw(sp + 0x28, s0);
  s0 = 0x80078D78;
  sw(sp + 0x4C, ra);
  sw(sp + 0x48, fp);
  sw(sp + 0x44, s7);
  sw(sp + 0x40, s6);
  sw(sp + 0x3C, s5);
  sw(sp + 0x38, s4);
  sw(sp + 0x34, s3);
  sw(sp + 0x30, s2);
  sw(sp + 0x2C, s1);
  v1 = lw(s0 + 0x00);
  v0 = 1; // 0x0001
  temp = v1 == v0;
  v0 = (int32_t)v1 < 2;
  if (temp) goto label8007D06C;
  temp = v0 == 0;
  if (temp) goto label8007CF40;
  temp = v1 == 0;
  v0 = 2; // 0x0002
  if (temp) goto label8007CF54;
  goto label8007DC34;
label8007CF40:
  v0 = 2; // 0x0002
  temp = v1 == v0;
  if (temp) goto label8007D768;
  goto label8007DC34;
label8007CF54:
  v1 = lw(0x80078D88);
  temp = v1 != v0;
  v0 = 3; // 0x0003
  if (temp) goto label8007CFAC;
  v0 = lw(0x80075680);
  v0 = lw(v0 + 0x00);
  v0 = (int32_t)v0 < 1170;
  temp = v0 != 0;
  a2 = 0;
  if (temp) goto label8007DC34;
  a0 = 128; // 0x0080
  v0 = lw(0x80078D84);
  a1 = lbu(0x8006FA74 + v0);
  a3 = 0;
  a1 -= 128; // 0xFFFFFF80
  goto label8007DC2C;
label8007CFAC:
  temp = v1 != v0;
  v0 = 4; // 0x0004
  if (temp) goto label8007CFEC;
  a0 = 128; // 0x0080
  a1 = 0;
  a2 = 0;
  a3 = 0;
  ra = 0x8007CFC8;
  function_8007CD38_title_screen();
  a0 = 192; // 0x00C0
  a1 = 210; // 0x00D2
  a3 = lw(0x80078D84);
  a2 = 11; // 0x000B
  a3 = a3 & 0xF;
  a3 = (int32_t)a3 < 8;
  a3 = a3 << 1;
  goto label8007DC2C;
label8007CFEC:
  temp = v1 != v0;
  if (temp) goto label8007DC34;
  v1 = lw(0x80078D84);
  v0 = (int32_t)v1 < 16;
  temp = v0 == 0;
  a3 = 0;
  if (temp) goto label8007D028;
  a0 = 128; // 0x0080
  a2 = 0;
  a1 = lbu(0x8006FA84 + v1);
  a1 -= 128; // 0xFFFFFF80
  goto label8007DC2C;
label8007D028:
  a0 = 108; // 0x006C
  a2 = 1; // 0x0001
  a1 = lbu(0x8006FA64 + v1);
  a1 -= 119; // 0xFFFFFF89
  ra = 0x8007D044;
  function_8007CD38_title_screen();
  a0 = 255; // 0x00FF
  v0 = lw(0x80078D84);
  a2 = -1; // 0xFFFFFFFF
  a1 = lbu(0x8006FA64 + v0);
  a3 = 0;
  a1 -= 119; // 0xFFFFFF89
  goto label8007DC2C;
label8007D06C:
  a0 = 108; // 0x006C
  a1 = 9; // 0x0009
  a2 = 1; // 0x0001
  a3 = 0;
  ra = 0x8007D080;
  function_8007CD38_title_screen();
  a0 = 255; // 0x00FF
  a1 = 9; // 0x0009
  a2 = -1; // 0xFFFFFFFF
  a3 = 0;
  ra = 0x8007D094;
  function_8007CD38_title_screen();
  v1 = lw(0x80078D88);
  v0 = v1 < 16;
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label8007DC34;
  v0 = lw(0x8007AA94 + v0); // &0x8007D0C4
  temp = v0;
  switch (temp)
  {
  case 0x8007D0C4: // 0
    goto label8007D0C4;
    break;
  case 0x8007D0D8: // 1
    goto label8007D0D8;
    break;
  case 0x8007D108: // 2
    goto label8007D108;
    break;
  case 0x8007D198: // 3
    goto label8007D198;
    break;
  case 0x8007D228: // 4
    goto label8007D228;
    break;
  case 0x8007D2F4: // 5 6
    goto label8007D2F4;
    break;
  case 0x8007D3F8: // 7
    goto label8007D3F8;
    break;
  case 0x8007D40C: // 8
    goto label8007D40C;
    break;
  case 0x8007D44C: // 9
    goto label8007D44C;
    break;
  case 0x8007D4DC: // 10
    goto label8007D4DC;
    break;
  case 0x8007D59C: // 11
    goto label8007D59C;
    break;
  case 0x8007D5B0: // 12
    goto label8007D5B0;
    break;
  case 0x8007D648: // 13
    goto label8007D648;
    break;
  case 0x8007DC34: // 14
    goto label8007DC34;
    break;
  case 0x8007D6D8: // 15
    goto label8007D6D8;
    break;
  default:
    JR(temp, 0x8007D0BC);
    return;
  }
label8007D0C4:
  a0 = 128; // 0x0080
  a1 = 46; // 0x002E
  a2 = 24; // 0x0018
  a3 = 0;
  goto label8007DC2C;
label8007D0D8:
  a0 = 128; // 0x0080
  a1 = 46; // 0x002E
  a2 = 24; // 0x0018
  a3 = 0;
  ra = 0x8007D0EC;
  function_8007CD38_title_screen();
  v0 = lw(0x80078D8C);
  temp = v0 == 0;
  if (temp) goto label8007DC34;
  a0 = 128; // 0x0080
  goto label8007DC18;
label8007D108:
  a0 = 128; // 0x0080
  a1 = 46; // 0x002E
  a2 = 25; // 0x0019
  a3 = 0;
  ra = 0x8007D11C;
  function_8007CD38_title_screen();
  s0 = 0x80078D84;
  v1 = lw(s0 + 0x00);
  v0 = (int32_t)v1 < 8;
  temp = v0 != 0;
  a0 = 128; // 0x0080
  if (temp) goto label8007DC18;
  v0 = lw(0x80078D8C);
  temp = v0 != 0;
  a3 = 0;
  if (temp) goto label8007D158;
  v0 = v1 & 0xF;
  a3 = (int32_t)v0 < 8;
  a0 = 128; // 0x0080
label8007D158:
  a1 = 88; // 0x0058
  a2 = 52; // 0x0034
  ra = 0x8007D164;
  function_8007CD38_title_screen();
  v1 = lw(0x80078D8C);
  v0 = 1; // 0x0001
  temp = v1 != v0;
  a3 = 0;
  if (temp) goto label8007D188;
  v0 = lw(s0 + 0x00);
  v0 = v0 & 0xF;
  a3 = (int32_t)v0 < 8;
label8007D188:
  a0 = 256; // 0x0100
  a1 = 88; // 0x0058
  a2 = 53; // 0x0035
  goto label8007DC0C;
label8007D198:
  a0 = 128; // 0x0080
  a1 = 46; // 0x002E
  a2 = 26; // 0x001A
  a3 = 0;
  ra = 0x8007D1AC;
  function_8007CD38_title_screen();
  s0 = 0x80078D84;
  v1 = lw(s0 + 0x00);
  v0 = (int32_t)v1 < 8;
  temp = v0 != 0;
  a0 = 128; // 0x0080
  if (temp) goto label8007DC18;
  v0 = lw(0x80078D8C);
  temp = v0 != 0;
  a3 = 0;
  if (temp) goto label8007D1E8;
  v0 = v1 & 0xF;
  a3 = (int32_t)v0 < 8;
  a0 = 128; // 0x0080
label8007D1E8:
  a1 = 88; // 0x0058
  a2 = 52; // 0x0034
  ra = 0x8007D1F4;
  function_8007CD38_title_screen();
  v1 = lw(0x80078D8C);
  v0 = 1; // 0x0001
  temp = v1 != v0;
  a3 = 0;
  if (temp) goto label8007D218;
  v0 = lw(s0 + 0x00);
  v0 = v0 & 0xF;
  a3 = (int32_t)v0 < 8;
label8007D218:
  a0 = 256; // 0x0100
  a1 = 88; // 0x0058
  a2 = 53; // 0x0035
  goto label8007DC0C;
label8007D228:
  a0 = 128; // 0x0080
  a1 = 22; // 0x0016
  a2 = 27; // 0x001B
  a3 = 0;
  ra = 0x8007D23C;
  function_8007CD38_title_screen();
  a0 = 128; // 0x0080
  a1 = 38; // 0x0026
  a2 = 28; // 0x001C
  a3 = 0;
  ra = 0x8007D250;
  function_8007CD38_title_screen();
  a0 = 128; // 0x0080
  a1 = 54; // 0x0036
  a2 = 29; // 0x001D
  a3 = 0;
  ra = 0x8007D264;
  function_8007CD38_title_screen();
  a0 = 128; // 0x0080
  a1 = 70; // 0x0046
  a2 = 30; // 0x001E
  a3 = 0;
  ra = 0x8007D278;
  function_8007CD38_title_screen();
  s0 = 0x80078D84;
  v1 = lw(s0 + 0x00);
  v0 = (int32_t)v1 < 8;
  temp = v0 != 0;
  a0 = 128; // 0x0080
  if (temp) goto label8007DC18;
  v0 = lw(0x80078D8C);
  temp = v0 != 0;
  a3 = 0;
  if (temp) goto label8007D2B4;
  v0 = v1 & 0xF;
  a3 = (int32_t)v0 < 8;
  a0 = 128; // 0x0080
label8007D2B4:
  a1 = 88; // 0x0058
  a2 = 52; // 0x0034
  ra = 0x8007D2C0;
  function_8007CD38_title_screen();
  v1 = lw(0x80078D8C);
  v0 = 1; // 0x0001
  temp = v1 != v0;
  a3 = 0;
  if (temp) goto label8007D2E4;
  v0 = lw(s0 + 0x00);
  v0 = v0 & 0xF;
  a3 = (int32_t)v0 < 8;
label8007D2E4:
  a0 = 256; // 0x0100
  a1 = 88; // 0x0058
  a2 = 54; // 0x0036
  goto label8007DC0C;
label8007D2F4:
  v1 = lw(0x80078D88);
  v0 = 5; // 0x0005
  temp = v1 != v0;
  a0 = 128; // 0x0080
  if (temp) goto label8007D344;
  a1 = 30; // 0x001E
  a2 = lw(0x80078DA0);
  a3 = 0;
  a2 += 31; // 0x001F
  ra = 0x8007D320;
  function_8007CD38_title_screen();
  a0 = 128; // 0x0080
  a1 = 46; // 0x002E
  a2 = 33; // 0x0021
  a3 = 0;
  ra = 0x8007D334;
  function_8007CD38_title_screen();
  a0 = 128; // 0x0080
  a1 = 62; // 0x003E
  a2 = 34; // 0x0022
  goto label8007D374;
label8007D344:
  a1 = 30; // 0x001E
  a2 = 27; // 0x001B
  a3 = 0;
  ra = 0x8007D354;
  function_8007CD38_title_screen();
  a0 = 128; // 0x0080
  a1 = 46; // 0x002E
  a2 = 35; // 0x0023
  a3 = 0;
  ra = 0x8007D368;
  function_8007CD38_title_screen();
  a0 = 128; // 0x0080
  a1 = 62; // 0x003E
  a2 = 36; // 0x0024
label8007D374:
  a3 = 0;
  ra = 0x8007D37C;
  function_8007CD38_title_screen();
  s0 = 0x80078D84;
  v1 = lw(s0 + 0x00);
  v0 = (int32_t)v1 < 8;
  temp = v0 != 0;
  a0 = 128; // 0x0080
  if (temp) goto label8007DC18;
  v0 = lw(0x80078D8C);
  temp = v0 != 0;
  a3 = 0;
  if (temp) goto label8007D3B8;
  v0 = v1 & 0xF;
  a3 = (int32_t)v0 < 8;
  a0 = 128; // 0x0080
label8007D3B8:
  a1 = 88; // 0x0058
  a2 = 55; // 0x0037
  ra = 0x8007D3C4;
  function_8007CD38_title_screen();
  v1 = lw(0x80078D8C);
  v0 = 1; // 0x0001
  temp = v1 != v0;
  a3 = 0;
  if (temp) goto label8007D3E8;
  v0 = lw(s0 + 0x00);
  v0 = v0 & 0xF;
  a3 = (int32_t)v0 < 8;
label8007D3E8:
  a0 = 256; // 0x0100
  a1 = 88; // 0x0058
  a2 = 54; // 0x0036
  goto label8007DC0C;
label8007D3F8:
  a0 = 128; // 0x0080
  a1 = 46; // 0x002E
  a2 = 37; // 0x0025
  a3 = 0;
  goto label8007DC2C;
label8007D40C:
  a0 = 128; // 0x0080
  a1 = 46; // 0x002E
  a2 = 38; // 0x0026
  a3 = 0;
  ra = 0x8007D420;
  function_8007CD38_title_screen();
  a3 = lw(0x80078D84);
  v0 = (int32_t)a3 < 8;
  temp = v0 != 0;
  a1 = 88; // 0x0058
  if (temp) goto label8007DC14;
  a0 = 128; // 0x0080
  a2 = 57; // 0x0039
  a3 = a3 & 0xF;
  a3 = (int32_t)a3 < 8;
  goto label8007DC0C;
label8007D44C:
  a0 = 128; // 0x0080
  a1 = 46; // 0x002E
  a2 = 39; // 0x0027
  a3 = 0;
  ra = 0x8007D460;
  function_8007CD38_title_screen();
  s0 = 0x80078D84;
  v1 = lw(s0 + 0x00);
  v0 = (int32_t)v1 < 8;
  temp = v0 != 0;
  a0 = 128; // 0x0080
  if (temp) goto label8007DC18;
  v0 = lw(0x80078D8C);
  temp = v0 != 0;
  a3 = 0;
  if (temp) goto label8007D49C;
  v0 = v1 & 0xF;
  a3 = (int32_t)v0 < 8;
  a0 = 128; // 0x0080
label8007D49C:
  a1 = 88; // 0x0058
  a2 = 53; // 0x0035
  ra = 0x8007D4A8;
  function_8007CD38_title_screen();
  v1 = lw(0x80078D8C);
  v0 = 1; // 0x0001
  temp = v1 != v0;
  a3 = 0;
  if (temp) goto label8007D4CC;
  v0 = lw(s0 + 0x00);
  v0 = v0 & 0xF;
  a3 = (int32_t)v0 < 8;
label8007D4CC:
  a0 = 256; // 0x0100
  a1 = 88; // 0x0058
  a2 = 54; // 0x0036
  goto label8007DC0C;
label8007D4DC:
  a0 = 128; // 0x0080
  a1 = 30; // 0x001E
  a2 = lw(0x80078DA0);
  a3 = 0;
  a2 += 31; // 0x001F
  ra = 0x8007D4F8;
  function_8007CD38_title_screen();
  a0 = 128; // 0x0080
  a1 = 46; // 0x002E
  a2 = 40; // 0x0028
  a3 = 0;
  ra = 0x8007D50C;
  function_8007CD38_title_screen();
  a0 = 128; // 0x0080
  a1 = 62; // 0x003E
  a2 = 41; // 0x0029
  a3 = 0;
  ra = 0x8007D520;
  function_8007CD38_title_screen();
  v1 = lw(0x80078D84);
  v0 = (int32_t)v1 < 8;
  temp = v0 != 0;
  a0 = 128; // 0x0080
  if (temp) goto label8007DC18;
  v0 = lw(0x80078D8C);
  temp = v0 != 0;
  a3 = 0;
  if (temp) goto label8007D558;
  v0 = v1 & 0xF;
  a3 = (int32_t)v0 < 8;
  a0 = 128; // 0x0080
label8007D558:
  a1 = 88; // 0x0058
  a2 = 56; // 0x0038
  ra = 0x8007D564;
  function_8007CD38_title_screen();
  v1 = lw(0x80078D8C);
  v0 = 1; // 0x0001
  temp = v1 != v0;
  a3 = 0;
  if (temp) goto label8007D58C;
  v0 = lw(0x80078D84);
  v0 = v0 & 0xF;
  a3 = (int32_t)v0 < 8;
label8007D58C:
  a0 = 256; // 0x0100
  a1 = 88; // 0x0058
  a2 = 54; // 0x0036
  goto label8007DC0C;
label8007D59C:
  a0 = 128; // 0x0080
  a1 = 46; // 0x002E
  a2 = 42; // 0x002A
  a3 = 0;
  goto label8007DC0C;
label8007D5B0:
  a0 = 128; // 0x0080
  a1 = 22; // 0x0016
  s0 = 0x80078DA0;
  a2 = lw(s0 + 0x00);
  a3 = 0;
  a2 += 31; // 0x001F
  ra = 0x8007D5D0;
  function_8007CD38_title_screen();
  a0 = 128; // 0x0080
  a1 = 38; // 0x0026
  a2 = 43; // 0x002B
  a3 = 0;
  ra = 0x8007D5E4;
  function_8007CD38_title_screen();
  a0 = 128; // 0x0080
  a1 = 54; // 0x0036
  a2 = 44; // 0x002C
  a3 = 0;
  ra = 0x8007D5F8;
  function_8007CD38_title_screen();
  a0 = 128; // 0x0080
  a1 = 70; // 0x0046
  a2 = 63; // 0x003F
  a3 = 0;
  ra = 0x8007D60C;
  function_8007CD38_title_screen();
  a3 = lw(0x80078D84);
  v0 = (int32_t)a3 < 8;
  temp = v0 != 0;
  a1 = 88; // 0x0058
  if (temp) goto label8007D638;
  a0 = 128; // 0x0080
  a2 = 57; // 0x0039
  a3 = a3 & 0xF;
  a3 = (int32_t)a3 < 8;
  ra = 0x8007D638;
  function_8007CD38_title_screen();
label8007D638:
  a0 = 128; // 0x0080
  a2 = lw(s0 + 0x00);
  a1 = 106; // 0x006A
  goto label8007DC24;
label8007D648:
  a0 = 128; // 0x0080
  a1 = 46; // 0x002E
  a2 = 45; // 0x002D
  a3 = 0;
  ra = 0x8007D65C;
  function_8007CD38_title_screen();
  s0 = 0x80078D84;
  v1 = lw(s0 + 0x00);
  v0 = (int32_t)v1 < 8;
  temp = v0 != 0;
  a0 = 128; // 0x0080
  if (temp) goto label8007DC18;
  v0 = lw(0x80078D8C);
  temp = v0 != 0;
  a3 = 0;
  if (temp) goto label8007D698;
  v0 = v1 & 0xF;
  a3 = (int32_t)v0 < 8;
  a0 = 128; // 0x0080
label8007D698:
  a1 = 88; // 0x0058
  a2 = 53; // 0x0035
  ra = 0x8007D6A4;
  function_8007CD38_title_screen();
  v1 = lw(0x80078D8C);
  v0 = 1; // 0x0001
  temp = v1 != v0;
  a3 = 0;
  if (temp) goto label8007D6C8;
  v0 = lw(s0 + 0x00);
  v0 = v0 & 0xF;
  a3 = (int32_t)v0 < 8;
label8007D6C8:
  a0 = 256; // 0x0100
  a1 = 88; // 0x0058
  a2 = 54; // 0x0036
  goto label8007DC0C;
label8007D6D8:
  a0 = 128; // 0x0080
  a1 = 46; // 0x002E
  a2 = 51; // 0x0033
  a3 = 0;
  ra = 0x8007D6EC;
  function_8007CD38_title_screen();
  s0 = 0x80078D84;
  v1 = lw(s0 + 0x00);
  v0 = (int32_t)v1 < 8;
  temp = v0 != 0;
  if (temp) goto label8007DC34;
  v0 = lw(0x80078D8C);
  temp = v0 != 0;
  a3 = 0;
  if (temp) goto label8007D724;
  v0 = v1 & 0xF;
  a3 = (int32_t)v0 < 8;
label8007D724:
  a0 = 128; // 0x0080
  a1 = 88; // 0x0058
  a2 = 60; // 0x003C
  ra = 0x8007D734;
  function_8007CD38_title_screen();
  v1 = lw(0x80078D8C);
  v0 = 1; // 0x0001
  temp = v1 != v0;
  a3 = 0;
  if (temp) goto label8007D758;
  v0 = lw(s0 + 0x00);
  v0 = v0 & 0xF;
  a3 = (int32_t)v0 < 8;
label8007D758:
  a0 = 256; // 0x0100
  a1 = 88; // 0x0058
  a2 = 61; // 0x003D
  goto label8007DC2C;
label8007D768:
  v0 = lw(0x80078D7C);
  v0 = (int32_t)v0 < 5;
  temp = v0 == 0;
  a0 = 108; // 0x006C
  if (temp) goto label8007D970;
  a1 = 9; // 0x0009
  a2 = 1; // 0x0001
  a3 = 0;
  ra = 0x8007D790;
  function_8007CD38_title_screen();
  a0 = 255; // 0x00FF
  a1 = 9; // 0x0009
  a2 = -1; // 0xFFFFFFFF
  a3 = 0;
  ra = 0x8007D7A4;
  function_8007CD38_title_screen();
  v0 = lw(0x80078D7C);
  temp = (int32_t)v0 <= 0;
  s4 = 0;
  if (temp) goto label8007D9BC;
  t0 = 168; // 0x00A8
  sw(sp + 0x20, t0);
  fp = 0;
  s6 = s0 + 80; // 0x0050
  s3 = s6;
  s7 = 184; // 0x00B8
  s5 = 140; // 0x008C
label8007D7D4:
  v0 = lw(s3 + 0x00);
  v0 = lbu(v0 + 0x40);
  temp = v0 != 0;
  if (temp) goto label8007D834;
  v1 = lw(s6 - 0x004C); // 0xFFFFFFB4
  v0 = (int32_t)v1 < 3;
  temp = v0 == 0;
  a3 = 0;
  if (temp) goto label8007D810;
  v0 = lw(s6 - 0x003C); // 0xFFFFFFC4
  temp = v0 != s4;
  if (temp) goto label8007D820;
label8007D810:
  v0 = v1 - 3; // 0xFFFFFFFD
  v0 = v0 < 2;
  temp = v0 == 0;
  a0 = s5;
  if (temp) goto label8007D828;
label8007D820:
  a3 = 3; // 0x0003
  a0 = s5;
label8007D828:
  a1 = 38; // 0x0026
  a2 = 8; // 0x0008
  goto label8007D938;
label8007D834:
  v1 = lw(s6 - 0x004C); // 0xFFFFFFB4
  v0 = (int32_t)v1 < 4;
  temp = v0 == 0;
  s2 = 0;
  if (temp) goto label8007D858;
  v0 = lw(s6 - 0x003C); // 0xFFFFFFC4
  temp = v0 != s4;
  if (temp) goto label8007D864;
label8007D858:
  v0 = 4; // 0x0004
  temp = v1 != v0;
  if (temp) goto label8007D868;
label8007D864:
  s2 = 3; // 0x0003
label8007D868:
  v0 = lw(s3 + 0x00);
  v1 = lbu(v0 + 0x00);
  v0 = 0xCCCCCCCD;
  multu(v1, v0);
  a0 = s5;
  a1 = 38; // 0x0026
  a3 = s2;
  s1 = 0;
  t0=hi;
  a2 = t0 >> 3;
  a2 = a2 & 0xFF;
  a2++;
  ra = 0x8007D8A4;
  function_8007CD38_title_screen();
  a0 = s7;
  a1 = 22; // 0x0016
  a2 = 23; // 0x0017
  a3 = s2;
  ra = 0x8007D8B8;
  function_8007CD38_title_screen();
  v1 = 0;
  a0 = lw(s3 + 0x00);
  v0 = a0 + v1;
label8007D8C8:
  v0 = lbu(v0 + 0x88);
  v1++;
  s1 += v0;
  v0 = (int32_t)v1 < 36;
  temp = v0 != 0;
  v0 = a0 + v1;
  if (temp) goto label8007D8C8;
  v0 = 0x66666667;
  mult(s1, v0);
  a0 = lw(sp + 0x20);
  a1 = 22; // 0x0016
  a3 = s2;
  v0 = (int32_t)s1 >> 31;
  t0=hi;
  v1 = (int32_t)t0 >> 2;
  s0 = v1 - v0;
  a2 = s0 << 2;
  a2 += s0;
  a2 = a2 << 1;
  a2 = s1 - a2;
  a2 += 13; // 0x000D
  ra = 0x8007D920;
  function_8007CD38_title_screen();
  v0 = (int32_t)s1 < 10;
  temp = v0 != 0;
  a0 = fp + 152; // 0x0098
  if (temp) goto label8007D940;
  a1 = 22; // 0x0016
  a2 = s0 + 13; // 0x000D
  a3 = s2;
label8007D938:
  ra = 0x8007D940;
  function_8007CD38_title_screen();
label8007D940:
  fp += 80; // 0x0050
  s3 += 4; // 0x0004
  s7 += 80; // 0x0050
  s5 += 80; // 0x0050
  s4++;
  t0 = lw(sp + 0x20);
  v0 = (int32_t)s4 < 3;
  t0 += 80; // 0x0050
  temp = v0 != 0;
  sw(sp + 0x20, t0);
  if (temp) goto label8007D7D4;
  goto label8007D9BC;
label8007D970:
  v0 = lw(0x80078D84);
  a2 = 1; // 0x0001
  a1 = lbu(0x8006FA84 + v0);
  a3 = 0;
  a1 -= 119; // 0xFFFFFF89
  ra = 0x8007D994;
  function_8007CD38_title_screen();
  a0 = 255; // 0x00FF
  v0 = lw(0x80078D84);
  a2 = -1; // 0xFFFFFFFF
  a1 = lbu(0x8006FA84 + v0);
  a3 = 0;
  a1 -= 119; // 0xFFFFFF89
  ra = 0x8007D9BC;
  function_8007CD38_title_screen();
label8007D9BC:
  v1 = lw(0x80078D7C);
  s0 = 1; // 0x0001
  temp = v1 != s0;
  v0 = 2; // 0x0002
  if (temp) goto label8007DA4C;
  v0 = lw(0x80078D84);
  v0 = v0 & 0xF;
  v0 = (int32_t)v0 < 8;
  temp = v0 == 0;
  a1 = 38; // 0x0026
  if (temp) goto label8007DA10;
  a2 = 9; // 0x0009
  v0 = lw(0x80078D8C);
  a3 = 0;
  a0 = v0 << 2;
  a0 += v0;
  a0 = a0 << 4;
  a0 += 140; // 0x008C
  ra = 0x8007DA10;
  function_8007CD38_title_screen();
label8007DA10:
  a0 = 128; // 0x0080
  a1 = 88; // 0x0058
  a2 = 59; // 0x003B
  a3 = 1; // 0x0001
  ra = 0x8007DA24;
  function_8007CD38_title_screen();
  a0 = 256; // 0x0100
  a1 = 88; // 0x0058
  a2 = 58; // 0x003A
  a3 = 0;
  ra = 0x8007DA38;
  function_8007CD38_title_screen();
  a0 = 128; // 0x0080
  a1 = 106; // 0x006A
  a2 = 46; // 0x002E
  a3 = 0;
  goto label8007DC2C;
label8007DA4C:
  temp = v1 != v0;
  v0 = 3; // 0x0003
  if (temp) goto label8007DB14;
  a1 = 38; // 0x0026
  a2 = 10; // 0x000A
  v0 = lw(0x80078D8C);
  a3 = 0;
  a0 = v0 << 2;
  a0 += v0;
  a0 = a0 << 4;
  a0 += 140; // 0x008C
  ra = 0x8007DA7C;
  function_8007CD38_title_screen();
  a1 = 38; // 0x0026
  a2 = 9; // 0x0009
  v0 = lw(0x80078D8C);
  a3 = 0;
  a0 = v0 << 2;
  a0 += v0;
  a0 = a0 << 4;
  a0 += 140; // 0x008C
  ra = 0x8007DAA4;
  function_8007CD38_title_screen();
  v0 = lw(0x80078D90);
  temp = v0 != 0;
  a3 = 0;
  if (temp) goto label8007DACC;
  v0 = lw(0x80078D84);
  v0 = v0 & 0xF;
  a3 = (int32_t)v0 < 8;
label8007DACC:
  a0 = 128; // 0x0080
  a1 = 88; // 0x0058
  a2 = 62; // 0x003E
  ra = 0x8007DADC;
  function_8007CD38_title_screen();
  v0 = lw(0x80078D90);
  temp = v0 != s0;
  a3 = 0;
  if (temp) goto label8007DB04;
  v0 = lw(0x80078D84);
  v0 = v0 & 0xF;
  a3 = (int32_t)v0 < 8;
label8007DB04:
  a0 = 256; // 0x0100
  a1 = 88; // 0x0058
  a2 = 54; // 0x0036
  goto label8007DC2C;
label8007DB14:
  temp = v1 != v0;
  v0 = 4; // 0x0004
  if (temp) goto label8007DB98;
  v0 = lw(0x80078D84);
  v0 = v0 & 0xF;
  v0 = (int32_t)v0 < 8;
  temp = v0 == 0;
  a1 = 38; // 0x0026
  if (temp) goto label8007DB5C;
  a2 = 9; // 0x0009
  v0 = lw(0x80078D8C);
  a3 = 0;
  a0 = v0 << 2;
  a0 += v0;
  a0 = a0 << 4;
  a0 += 140; // 0x008C
  ra = 0x8007DB5C;
  function_8007CD38_title_screen();
label8007DB5C:
  a0 = 128; // 0x0080
  a1 = 88; // 0x0058
  a2 = 59; // 0x003B
  a3 = 0;
  ra = 0x8007DB70;
  function_8007CD38_title_screen();
  a0 = 256; // 0x0100
  a1 = 88; // 0x0058
  a2 = 58; // 0x003A
  a3 = 1; // 0x0001
  ra = 0x8007DB84;
  function_8007CD38_title_screen();
  a0 = 128; // 0x0080
  a1 = 106; // 0x006A
  a2 = 47; // 0x002F
  a3 = 0;
  goto label8007DC2C;
label8007DB98:
  temp = v1 != v0;
  if (temp) goto label8007DC34;
  v0 = lw(0x80078D8C);
  temp = v0 != 0;
  a3 = 0;
  if (temp) goto label8007DBC8;
  v0 = lw(0x80078D84);
  v0 = v0 & 0xF;
  a3 = (int32_t)v0 < 8;
label8007DBC8:
  a0 = 128; // 0x0080
  a1 = 88; // 0x0058
  a2 = 59; // 0x003B
  ra = 0x8007DBD8;
  function_8007CD38_title_screen();
  v0 = lw(0x80078D8C);
  temp = v0 != s0;
  a3 = 0;
  if (temp) goto label8007DC00;
  v0 = lw(0x80078D84);
  v0 = v0 & 0xF;
  a3 = (int32_t)v0 < 8;
label8007DC00:
  a0 = 256; // 0x0100
  a1 = 88; // 0x0058
  a2 = 58; // 0x003A
label8007DC0C:
  ra = 0x8007DC14;
  function_8007CD38_title_screen();
label8007DC14:
  a0 = 128; // 0x0080
label8007DC18:
  a1 = 106; // 0x006A
  a2 = lw(0x80078DA0);
label8007DC24:
  a3 = 0;
  a2 += 49; // 0x0031
label8007DC2C:
  ra = 0x8007DC34;
  function_8007CD38_title_screen();
label8007DC34:
  v0 = lbu(SKYBOX_DATA + 0x10);
  v1 = lbu(SKYBOX_DATA + 0x11);
  a0 = lbu(SKYBOX_DATA + 0x12);
  sb(DISP1 + 0x19, v0);
  sb(DISP1 + 0x1A, v1);
  sb(DISP1 + 0x1B, a0);
  sb(DISP2 + 0x19, v0);
  sb(DISP2 + 0x1A, v1);
  sb(DISP2 + 0x1B, a0);
  ra = 0x8007DC84;
  function_800521C0();
  ra = 0x8007DC8C;
  function_8001F158();
  s0 = 0x8006FCF4;
  a0 = s0;
  a1 = 0;
  a2 = 2304; // 0x0900
  ra = 0x8007DCA4;
  spyro_memset32(a0, a1, a2);
  ra = 0x8007DCAC;
  function_8001F798();
  a0 = s0;
  a1 = 0;
  a2 = 7168; // 0x1C00
  ra = 0x8007DCBC;
  spyro_memset32(a0, a1, a2);
  v0 = 0x0001C000;
  sw(0x800785D0, v0);
  a0 = -1; // 0xFFFFFFFF
  ra = 0x8007DCD4;
  function_800258F0();
  a0 = -1; // 0xFFFFFFFF
  a1 = 0x80076DE4;
  a2 = a1 - 20; // 0xFFFFFFEC
  ra = 0x8007DCE8;
  draw_skybox(a0, a1, a2);
  a1 = lw(0x80075918);
  if (a1) {
    a0 = 2;
    a1 = a1 << 4;
    a2 = a1;
    a3 = a1;
    ra = 0x8007DD0C;
    function_800190D4();
  }

  wait_two_vsyncs();
  
  PutDispEnv(&backbuffer_disp->disp);
  PutDrawEnv(&backbuffer_disp->draw);
  DrawOTag(spyro_combine_all_command_buffers(0x800));

  ra = lw(sp + 0x4C);
  fp = lw(sp + 0x48);
  s7 = lw(sp + 0x44);
  s6 = lw(sp + 0x40);
  s5 = lw(sp + 0x3C);
  s4 = lw(sp + 0x38);
  s3 = lw(sp + 0x34);
  s2 = lw(sp + 0x30);
  s1 = lw(sp + 0x2C);
  s0 = lw(sp + 0x28);
  sp += 80; // 0x0050
}

// size: 0x00000368
void function_8007BFD0_credits(void)
{
  uint32_t temp;
  sp -= 48; // 0xFFFFFFD0
  v1 = lw(0x80075704);
  v0 = -1; // 0xFFFFFFFF
  sw(sp + 0x2C, ra);
  sw(sp + 0x28, s2);
  sw(sp + 0x24, s1);
  temp = v1 != v0;
  sw(sp + 0x20, s0);
  if (temp) goto label8007C05C;
  a0 = 0;
  ra = 0x8007BFFC;
  v0 = DrawSync(a0);
  a0 = 0;
  ra = 0x8007C004;
  v0 = VSync(a0);
  a3 = 8; // 0x0008
  sh(sp + 0x10, 0);
  if (backbuffer_disp != addr_to_pointer(DISP1))
    a3 = 0xF8;
  else
    a3 = 0x08;
  
  a0 = sp + 16; // 0x0010
  a1 = 0;
  a2 = 256; // 0x0100
  a2 -= a3;
  v0 = 512; // 0x0200
  sh(sp + 0x14, v0);
  v0 = 224; // 0x00E0
  sh(sp + 0x12, a3);
  sh(sp + 0x16, v0);
  ra = 0x8007C04C;
  v0 = MoveImage(addr_to_pointer(a0), a1, a2);
  a0 = 0;
  ra = 0x8007C054;
  v0 = DrawSync(a0);
  goto label8007C31C;
label8007C05C:
  v0 = lbu(SKYBOX_DATA + 0x10);
  v1 = lbu(SKYBOX_DATA + 0x11);
  a0 = lbu(SKYBOX_DATA + 0x12);
  a1 = lw(0x800756E4);
  sb(DISP1 + 0x19, v0);
  sb(DISP1 + 0x1A, v1);
  sb(DISP1 + 0x1B, a0);
  sb(DISP2 + 0x19, v0);
  sb(DISP2 + 0x1A, v1);
  sb(DISP2 + 0x1B, a0);
  temp = (int32_t)a1 <= 0;
  s1 = 0;
  if (temp) goto label8007C1D4;
  s2 = 0;
label8007C0B8:
  a1 = 0;
  a2 = 88; // 0x0058
  a0 = lw(gameobject_stack_ptr);
  v0 = lw(0x8007589C);
  a0 -= 88; // 0xFFFFFFA8
  sw(gameobject_stack_ptr, a0);
  s0 = v0 + s2;
  ra = 0x8007C0E4;
  spyro_memset32(a0, a1, a2);
  v1 = lw(gameobject_stack_ptr);
  v0 = lh(s0 + 0x12);
  sw(v1 + 0x0C, v0);
  v0 = lh(s0 + 0x14);
  sw(v1 + 0x10, v0);
  v0 = lh(s0 + 0x16);
  sw(v1 + 0x14, v0);
  v0 = lbu(s0 + 0x05);
  sb(v1 + 0x44, v0);
  v1 = lh(s0 + 0x1A);
  v0 = 277; // 0x0115
  temp = v1 != v0;
  if (temp) goto label8007C158;
  v1 = lw(gameobject_stack_ptr);
  v0 = lbu(v1 + 0x44);
  v0 += 41; // 0x0029
  sb(v1 + 0x44, v0);
  v1 = lw(gameobject_stack_ptr);
  v0 = 7168; // 0x1C00
  sw(v1 + 0x14, v0);
label8007C158:
  v1 = lw(gameobject_stack_ptr);
  v0 = lbu(s0 + 0x06);
  sb(v1 + 0x45, v0);
  v1 = lw(gameobject_stack_ptr);
  v0 = lbu(s0 + 0x07);
  sb(v1 + 0x46, v0);
  v1 = lw(gameobject_stack_ptr);
  v0 = lhu(s0 + 0x1A);
  sh(v1 + 0x36, v0);
  v0 = 11; // 0x000B
  sb(v1 + 0x4F, v0);
  v1 = lw(gameobject_stack_ptr);
  v0 = 255; // 0x00FF
  sb(v1 + 0x50, v0);
  v1 = lw(gameobject_stack_ptr);
  v0 = 127; // 0x007F
  sb(v1 + 0x47, v0);
  v0 = lw(0x800756E4);
  s1++;
  v0 = (int32_t)s1 < (int32_t)v0;
  temp = v0 != 0;
  s2 += 28; // 0x001C
  if (temp) goto label8007C0B8;
label8007C1D4:
  a1 = lw(0x80075918);
  temp = a1 == 0;
  a1 = a1 << 4;
  if (temp) goto label8007C1F8;
  a0 = 2; // 0x0002
  a2 = a1;
  a3 = a1;
  ra = 0x8007C1F8;
  function_800190D4();
label8007C1F8:
  sw(0x8006FCF4 + 0x2400, 0);
  function_80018880();
  spyro_memset32(0x8006FCF4, 0, 0x900);
  ra = 0x8007C224;
  function_80022A2C();
  spyro_memset32(0x8006FCF4, 0, 0x1C00);
  v0 = 0x00028000;
  sw(0x800785D0, v0);
  a0 = -1; // 0xFFFFFFFF
  ra = 0x8007C24C;
  function_800258F0();
  draw_skybox(-1, 0x80076DE4, 0x80076DD0);

  wait_two_vsyncs();
  
  PutDispEnv(&backbuffer_disp->disp);
  PutDrawEnv(&backbuffer_disp->draw);
  DrawOTag(spyro_combine_all_command_buffers(0x800));

label8007C31C:
  ra = lw(sp + 0x2C);
  s2 = lw(sp + 0x28);
  s1 = lw(sp + 0x24);
  s0 = lw(sp + 0x20);
  sp += 48; // 0x0030
}