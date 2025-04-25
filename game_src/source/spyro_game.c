#include "spyro_game.h"
#include "debug.h"
#include "main.h"
#include "decompilation.h"
#include "psx_mem.h"
#include "spyro_graphics.h"
#include "spyro_cdrom.h"
#include "spyro_string.h"
#include "spyro_spu.h"
#include "spyro_psy.h"
#include "spyro_math.h"
#include "spyro_vsync.h"
#include "psx_ops.h"
#include "int_math.h"
#include "function_chooser.h"
#include "not_renamed.h"

#include <string.h>

void redirect_func_800758CC()
{
  switch (lw(0x800758CC))
  {
  case 0x800857CC:
    function_800857CC();
    break;
  case 0x8008772C:
    function_8008772C();
    break;
  case 0x80082960:
    function_80082960();
    break;
  case 0x800872A4:
    function_800872A4();
    break;
  case 0x80081DA8:
    function_80081DA8();
    break;
  case 0x8008249C:
    function_8008249C();
    break;
  case 0x8008A258:
    function_8008A258();
    break;
  case 0x8008A4D0:
    function_8008A4D0();
    break;
  case 0x80088B88:
    function_80088B88();
    break;
  case 0x80083608:
    function_80083608();
    break;
  case 0x800845F0:
    function_800845F0();
    break;
  case 0x800819BC:
    function_800819BC();
    break;
  case 0x8008B2C0:
    function_8008B2C0();
    break;
  case 0x8008A36C:
    function_8008A36C();
    break;
  case 0x8008883C:
    function_8008883C();
    break;
  case 0x80086DD8:
    function_80086DD8();
    break;
  case 0x80083AB4:
    function_80083AB4();
    break;
  case 0x80081F0C:
    function_80081F0C();
    break;
  case 0x80087EF0:
    function_80087EF0();
    break;
  case 0x8008465C:
    function_8008465C();
    break;
  case 0x80084718:
    function_80084718();
    break;
  case 0x80086B38:
    function_80086B38();
    break;
  case 0x800874FC:
    function_800874FC();
    break;
  case 0x8008223C:
    function_8008223C();
    break;
  case 0x80088E24:
    function_80088E24();
    break;
  case 0x80084B94:
    function_80084B94();
    break;
  case 0x80084620:
    function_80084620();
    break;
  case 0x8008590C:
    function_8008590C();
    break;
  case 0x800836A8:
    function_800836A8();
    break;
  case 0x80082028:
    function_80082028();
    break;
  case 0x80083568:
    function_80083568();
    break;
  case 0x80085664:
    function_80085664();
    break;
  case 0x80083108:
    function_80083108();
    break;
  case 0x80082F24:
    function_80082F24();
    break;
  case 0x80083690:
    function_80083690();
    break;
  default:
    BREAKPOINT;
  }
}

// size: 0x000001D4
void function_80051FEC(void)
{
  memset(addr_to_pointer(0x80077868), 0, 32);
  vec3_32 pos = *(vec3_32*)addr_to_pointer(spyro_position);
  t6 = 0x8006FCF4 + 0x400;
  for (struct game_object *object = addr_to_pointer(lw(0x80075828)); object->unknown48 != -1; object++) {
    if ((int32_t)object->unknown48 < 0) continue;
    if (object->unknown51) goto label8005210C;
    v1 = object->unknown52;
    if (v1 == 0) continue;
    if ((int32_t)v1 < 0) goto label8005210C;
    v1 = v1 << 10;
    vec3_32 p = vec3_32_sub(object->p, pos);
    if (abs_int(p.x) >= v1
     || abs_int(p.y) >= v1
     || abs_int(p.z) >= v1) continue;
    if (v1/8*v1/8 <= p.x/8*p.x/8 + p.y/8*p.y/8) continue;
  label8005210C:
    if ((int32_t)object->unknown43 >= 0) {
      sb(0x80077868 + object->unknown43, 1);
    } else {
      sw(t6, pointer_to_addr(object));
      t6 += 4;
    }
  }
  struct game_object *game_objects = addr_to_pointer(lw(0x80075828));
  at = lw(0x80075828);
  v0 = lw(0x800757F8);
  for (int i = 0; i < 0x20; i++) {
    if (lb(0x80077868+i) == 0) continue;

    a2 = lw(v0 + i*4);
    do {
      a3 = lh(a2);
      a2 += 2;
      struct game_object *object = &game_objects[a3 & 0x7FFF];
      if (object->unknown48 >= 0) {
        sw(t6, pointer_to_addr(object));
        t6 += 4;
      }
    } while ((int32_t)a3 >= 0);
  }
  sw(t6, 0);
}

// size: 0x0000038C
void function_8003C358(void)
{
  sp -= 128; // 0xFFFFFF80
  sw(sp + 0x68, s4);
  sw(sp + 0x64, s3);
  sw(sp + 0x7C, ra);
  sw(sp + 0x78, fp);
  sw(sp + 0x74, s7);
  sw(sp + 0x70, s6);
  sw(sp + 0x6C, s5);
  sw(sp + 0x60, s2);
  sw(sp + 0x5C, s1);
  sw(sp + 0x58, s0);

  vec3_32 pos = *(vec3_32*)addr_to_pointer(spyro_position);

  s4 = a0;
  s3 = a1;

  if (s3) {
    s7 = lbu(s4 + 0x46);
    s2 = lw(0x8006F7F0 + lw(lw(s4))*4); // &0x80075578
    s0 = s7 & 0xFF;
  } else {
    s2 = lw(0x8006F880); // &0x80010078
    s7 = spyro_atan2(pos.x - lw(s4 + 0x0C), pos.y - lw(s4 + 0x10), 0);
    s0 = s7 & 0xFF;
  }

  sw(sp + 0x10,  cos_lut[s0]*3/16);
  sw(sp + 0x14,  sin_lut[s0]*3/16);
  sw(sp + 0x18,  0);
  sw(sp + 0x20, -sin_lut[s0]/32);
  sw(sp + 0x24,  cos_lut[s0]/32);
  sw(sp + 0x28, 0);
  if (s3) {
    if (lbu(s4 + 0x49)) {
      s7 = lbu(s4 + 0x46);
      s0 = sp + 0x30;
      spyro_vec3_clear(s0);
      spyro_vec3_sub(sp + 0x10, s0, sp + 0x10);
      spyro_vec3_sub(sp + 0x20, s0, sp + 0x20);
    } else {
      s7 = lbu(s4 + 0x46) - 0x80;
    }
  }
  s0 = sp + 0x20;
  fp = spyro_strlen(s2);
  s1 = fp - 1;
  spyro_vec3_mul(sp + 0x30, s0, s1);
  spyro_vec3_shift_left(s0, 1);
  s5 = s1*2;
  s0 = (s5 & 0xFF) << 4;
  sw(sp + 0x38, lw(sp + 0x38) + spyro_cos(s0)*3/8);
  sw(sp + 0x40, lw(sp + 0x10)*spyro_cos(s0));
  sw(sp + 0x48, 0);
  sw(sp + 0x44, lw(sp + 0x14)*spyro_cos(s0));
  s3 = 0;
  s6 = s2;
  for (int i = 0; i < fp; i++) {
    char ch = lbu(s6);
    uint32_t modelID;
    if (ch != ' ') {
      if (ch >= 'A' && ch <= 'Z')
        modelID = 0x1AA + ch - 'A';
      else
        modelID = 0x4C;
      a0 = modelID;
      a1 = s4;
      ra = 0x8003C584;
      redirect_func_800758CC();
      s2 = v0;
      s1 = (s5 & 0xFF) << 4;
      sw(lw(s2 + 0x00) + 0, s4);
      sh(lw(s2 + 0x00) + 4, fp);
      sh(lw(s2 + 0x00) + 6, s3);
      s0 = s2 + 0x0C;
      sw(s0 + 0, lw(sp + 0x10)*spyro_cos(s1));
      sw(s0 + 4, lw(sp + 0x14)*spyro_cos(s1));
      sw(s0 + 8, 0);
      spyro_vec3_sub(s0, s0, sp + 0x40);
      spyro_vec3_shift_right(s0, 10);
      spyro_vec3_add(s0, s0, sp + 0x10);
      spyro_vec3_add(s0, s0, s4 + 0x0C);
      spyro_vec3_sub(s0, s0, sp + 0x30);
      a0 = lw(s0 + 8) + spyro_cos(s1)*3/8;
      v1 = lh(s2 + 0x36);
      if (v1 == 0x4C) // ','
        sw(s0 + 8, a0 + 0x100);
      else
        sw(s0 + 8, a0);
      sb(s2 + 0x46, s7);
      sh(s2 + 0x38, s7 & 0xFF);
      v1 = lbu(s4 + 0x49);
      sb(s2 + 0x49, s3*8);
      sb(s2 + 0x4F, 2);
      sb(s2 + 0x48, v1);
    }
    spyro_vec3_sub(sp + 0x30, sp + 0x30, sp + 0x20);
    s5 -= 4;
    s3++;
    s6++;
  }
  ra = lw(sp + 0x7C);
  fp = lw(sp + 0x78);
  s7 = lw(sp + 0x74);
  s6 = lw(sp + 0x70);
  s5 = lw(sp + 0x6C);
  s4 = lw(sp + 0x68);
  s3 = lw(sp + 0x64);
  s2 = lw(sp + 0x60);
  s1 = lw(sp + 0x5C);
  s0 = lw(sp + 0x58);
  sp += 0x80;
}

// size: 0x000001C8
struct game_object *create_3d_text2(char *str, vec3_32 *pos, uint32_t spacing, uint32_t unknown_metadata)
{
  struct game_object *game_object = addr_to_pointer(lw(gameobject_stack_ptr));

  for (uint8_t ch = *str++; ch; ch = *str++) {
    if (ch == ' ') {
      pos->x += spacing;
      continue;
    }

    game_object--;
    *game_object = (struct game_object){0};
    game_object->p = *pos;
    if (ch >= '0' && ch <= '9') {
      game_object->modelID = 0x104 + ch - '0';
    } else if (ch >= 'A' && ch <= 'Z') {
      game_object->modelID = 0x1AA + ch - 'A';
    } else if (ch == '/') {
      game_object->modelID = 0x115;
    } else if (ch == '?') {
      game_object->modelID = 0x116;
    } else if (ch == '%') {
      game_object->modelID = 0x110;
    } else if (ch == '^') {
      game_object->modelID = 0x141;
    } else if (ch == '+') {
      game_object->modelID = 0x13D;
    } else {
      game_object->modelID = 0x147;
    }
    game_object->unknown47 = 0x7F;
    game_object->unknown4F = unknown_metadata;
    game_object->render_distance = 0xFF;
    pos->x += spacing;
  }
  sw(gameobject_stack_ptr, pointer_to_addr(game_object));
  return game_object;
}

// size: 0x000001C8
void function_80017FE4(void)
{
  v0 = pointer_to_addr(create_3d_text2(addr_to_pointer(a0), addr_to_pointer(a1), a2, a3));
}

// size: 0x000002A0
struct game_object *create_3d_text1(char *str, vec3_32 *pos, vec3_32 size, uint32_t unknown_spacing, uint32_t unknown_metadata)
{
  uint32_t s2 = 1;
  struct game_object *game_object = addr_to_pointer(lw(gameobject_stack_ptr));
  for (char ch = *str++; ch; ch = *str++) {
    if (ch == ' ') {
      s2 = 1;
      pos->x += size.x*3/4;
      continue;
    }
    game_object--;
    *game_object = (struct game_object){0};
    game_object->p = *pos;
    
    if (ch == '!' || ch == '?')
      s2 = 1;

    if (s2 == 0) {
      game_object->p.y += size.y;
      game_object->p.z = size.z;
    }

    if (ch >= '0' && ch <= '9') {
      game_object->modelID = 0x104 + ch - '0';
    } else if (ch >= 'A' && ch <= 'Z') {
      game_object->modelID = 0x1AA + ch - 'A';
    } else if (ch == '!') {
      game_object->modelID = 0x4B;
    } else if (ch == ',') {
      game_object->modelID = 0x4C;
    } else if (ch == '?') {
      game_object->modelID = 0x116;
    } else if (ch == '.') {
      game_object->modelID = 0x147;
    } else {
      game_object->modelID = 0x4C;
      game_object->p.y -= size.x*2/3;
    }
    game_object->unknown47 = 0x7F;
    game_object->unknown4F = unknown_metadata;
    game_object->render_distance = 0xFF;

    if (s2)
      pos->x += unknown_spacing;
    else
      pos->x += size.x;

    s2 = ch >= '0' && ch <= '9';
  }
  sw(gameobject_stack_ptr, pointer_to_addr(game_object));
  return game_object;
}

// size: 0x000002A0
void function_800181AC(void)
{
  BREAKPOINT;
  v0 = pointer_to_addr(create_3d_text1(addr_to_pointer(a0), addr_to_pointer(a1), *(vec3_32 *)addr_to_pointer(a2), a3, lw(sp + 0x10)));
}

// size: 0x00000068
void function_80012CF0(void)
{
  sw(0x800761D4, a0);
  sw(0x800761D0, a0 + 0x104);
  if (a1 == 0) return;
  int32_t num = lw(a0 + 0x100);
  for (int i = 0; i < num; i++) {
    v1 = a0 + 0x104 + i*0x14;
    sw(v1, lw(v1) + 0x1010);
  };
}

// size: 0x000004D8
void function_80012D58(void)
{

  uint32_t struct_base = a0;

  const uint32_t struct1 = struct_base + 0x04;
  const int32_t struct1_len = lw(struct1);
  const uint32_t struct1_elements = struct1 + 0x04;
  sw(0x800785C0, struct1_elements);
  sw(0x800785C4, struct1_elements + struct1_len*16);
  sw(0x800785C8, struct1_len);

  struct_base = struct_base + lw(struct_base);

  const uint32_t struct2 = struct_base + 0x04;
  const int32_t struct2_len = lw(struct2);
  const uint32_t struct2_elements = struct2 + 0x04;
  sw(0x800785A8, struct2_elements);
  sw(0x800785AC, struct2_len);
  
  for (int i = 0; i < struct2_len; i++)
    sw(struct2_elements + i*4, struct2 + lw(struct2_elements + i*4));

  if (a1 == 0) {
    struct_base = struct_base + lw(struct_base);

    if (lw(struct_base) > 4) {
      const uint32_t struct3 = struct_base + 0x04;

      const uint32_t struct3_1 = struct3 + 0x04;
      const int32_t struct3_1_len = lw(struct3_1);
      const uint32_t struct3_1_elements = struct3_1 + 0x04;
      sw(0x800785B0, struct3_1_elements);
      sw(0x800785B4, struct3_1_len);

      for (int i = 0; i < struct3_1_len; i++)
        sw(struct3_1_elements + i*4, struct3 + lw(struct3_1_elements + i*4));

      const uint32_t struct3_2 = struct_base + lw(struct3) + 0x04;
      const int32_t struct3_2_len = lw(struct3_2);
      const uint32_t struct3_2_elements = struct3_2 + 0x04;

      sw(0x80077788, struct3_2_len);
      sw(0x8007778C, struct3_2_elements);

      for (int i = 0; i < struct3_2_len; i++)
        sw(struct3_2_elements + i*4, struct3_2 + lw(struct3_2_elements + i*4));

    } else {
      sw(0x800785B0, 0);
      sw(0x800785B4, 0);
      sw(0x8007778C, 0);
    }

    struct_base = struct_base + lw(struct_base);

    const uint32_t struct4 = struct_base + 0x04;
    const uint32_t struct4_len = lw(struct4);
    const uint32_t struct4_elements = struct4 + 0x04;
    sw(0x800785B8, struct4_elements);
    sw(0x800785BC, struct4_len);
    for (int i = 0; i < struct4_len; i++)
      sw(struct4_elements + i*4, lw(struct4_elements + i*4) + struct4);
    
    struct_base = struct_base + lw(struct_base);

    const uint32_t struct5 = struct_base + 0x04;
    const uint32_t struct5_elements = struct5 + 0x08;

    sw(0x800785D4, struct5);

    for (int i = 0; i < 5; i++)
      sw(struct5_elements + i*4, lw(struct5_elements + i*4) + struct5);
  }

  struct_base = struct_base + lw(struct_base);

  const uint32_t struct6 = struct_base + 4;
  const uint32_t struct6_len = lw(struct6 + 4);
  const uint32_t struct6_elements = struct6 + 8;

  sb(0x80077790, lbu(struct6 + 0));
  sb(0x80077791, lbu(struct6 + 1));
  sb(0x80077792, lbu(struct6 + 2));
  sw(0x80077780, struct6_len);
  sw(0x80077784, struct6_elements);

  for (int i = 0; i < struct6_len; i++)
    sw(struct6_elements + i*4, lw(struct6_elements + i*4) + struct_base + 4);
  
  struct_base = struct_base + lw(struct_base);
  if (a1) {
    v0 = struct_base;
    return;
  }
  
  const uint32_t struct7 = struct_base;
  const int32_t struct7_len = lw(struct7);
  const uint32_t struct7_elements = struct7 + 0x04;
  sw(0x800758BC, struct7_len);
  uint32_t element = struct7_elements;
  for (int i = 0; i < struct7_len; i++) {
    const uint32_t element_struct1 = element + 0x04;
    (void)element_struct1;
    const uint32_t element_struct1_len = lw(element + 0x04);
    const uint32_t element_struct1_elements = element + 0x08;

    sw(0x80078640 + i*4, element);
    int32_t element_unknown2 = element_struct1_elements + element_struct1_len*0x0C + 0x18;
    sw(element, element_unknown2);
    struct_base = element_unknown2 + 0x14;

    const uint32_t element_struct2 = struct_base + 0x04;
    const uint32_t element_struct2_unknown1 = lw(element_struct2);
    const uint32_t element_struct2_len = lw(element_struct2 + 0x04);
    const uint32_t element_struct2_elements = element_struct2 + 0x08;

    sw(element_unknown2 + 0x10, element_struct2_unknown1);
    sw(element_unknown2 + 0x00, element_struct2_len);
    sw(element_unknown2 + 0x04, element_struct2_elements);
    for (int i = 0; i < element_struct2_len; i++)
      sw(element_struct2_elements + i*4, lw(element_struct2_elements + i*4) + element_struct2);

    element = struct_base + lw(struct_base);
  }
  
  struct_base = element;

  const uint32_t struct8 = struct_base + 0x04;
  const uint32_t struct8_len = lw(struct8);
  const uint32_t struct8_elements = struct8 + 0x04;

  element = struct8_elements;
  for (int i = 0; i < struct8_len; i++) {
    const int16_t element_index = lh(element + 0);
    const int16_t element_len = lh(element + 2);
    sw(0x80076278 + element_index*4, element);
    element = element + element_len + 4;
  }

  struct_base = struct_base + lw(struct_base);
  sw(0x80076234, lw(struct_base));

  a0 = struct_base + 4;
  a1 = 1;
  function_80012CF0();

  v0 = lw(struct_base) + struct_base;
  return;
}

// contains bug
// size: 0x0000026C
uint32_t func_800133E0(uint32_t param1)
{
  const int32_t len = lw(param1 + 0x00);
  if (len < 0) {
    sw(param1 + 0x04, lw(param1 + 0x04) + param1);
    sw(param1 + 0x08, lw(param1 + 0x08) + param1);
    sw(param1 + 0x0C, lw(param1 + 0x0C) + param1);
    return param1 & 0x7FFFFFFF;
  }

  for (int i = 0; i < 8; i++) {
    v0 = lw(param1 + i*4 + 0x14);
    if (v0) sw(param1 + i*4 + 0x14, v0 + (param1 & 0x7FFFFFFF));
  }

  const uint32_t ptr_list = param1 + 0x38;
  
  const uint32_t num1 = lw(param1 + 0x34) + param1;
  sw(param1 + 0x34, num1);
  for (int i = 0; i < len; i++) {
    const uint32_t struct_ptr = ptr_list + i*4;
    if (lw(struct_ptr) == -1) continue;
    const uint32_t struct_addr = lw(struct_ptr) + param1;
    sw(struct_ptr, struct_addr);

    int32_t struct_len = lh(struct_addr);

    sw(struct_addr + 0x14, lw(struct_addr + 0x14) + num1);
    sw(struct_addr + 0x18, lw(struct_addr + 0x18) + num1);
    if (lw(struct_addr + 0x1C)) {
      sw(struct_addr + 0x1C, lw(struct_addr + 0x1C) + num1);
      sw(struct_addr + 0x20, lw(struct_addr + 0x20) + num1);
    }
    if (lbu(struct_addr + 0x04)) {
      sw(struct_addr + 0x10, lw(struct_addr + 0x10) + num1);
      for (int i = 0; i < struct_len; i++) {
        v1 = lw(struct_addr + 0x24 + i*4);
        sw(struct_addr + 0x24 + i*4, (v1 & 0xFFE00000) + (((int32_t)(num1 + (v1 & 0x1FFFFF)) >> 1) & 0x1FFFFF));
      }
    } else {
      for (int i = 0; i < struct_len; i++) {
        sw(struct_addr + 0x24 + i*8, lw(struct_addr + 0x24 + i*8) + (num1 & 0x001FFFFF));
        v1 = lw(struct_addr + 0x28 + i*8);
        if (v1 & 0xFFFF) {
          v0 = (num1 - (struct_addr + 0x24 + i*8)) >> 2;
          sw(struct_addr + 0x28 + i*8, (v0 & 0xFFFF) + v1);
        }
      }
    }
  }
  return param1;
}
/*
// size: 0x0000026C
void function_800133E0(void)
{
  v0 = func_800133E0(a0);
}*/

// size: 0x0000007C
void new_game_object(struct game_object *game_object)
{
  game_object->unknown18 = 0;
  game_object->unknown1C = 0;

  game_object->unknown38 = 0;

  game_object->unknown3A = 0;
  game_object->unknown3D = 0;
  game_object->unknown3E = 0;
  game_object->unknown3F = 1;
  game_object->unknown40 = 0;
  game_object->unknown41 = 0x20;

  game_object->unknown43 = 0xFF;
  game_object->rotx = 0;
  game_object->roty = 0;
  game_object->rotz = 0;
  game_object->unknown47 = 4;
  game_object->unknown48 = 0;
  game_object->unknown49 = 0;
  game_object->unknown4A = 0xFF;
  game_object->unknown4B = 0x20;
  
  game_object->unknown4C = 0;
  game_object->unknown4D = 0;
  game_object->unknown4E = 0;
  game_object->unknown4F = 0;

  game_object->render_distance = 0x10;

  game_object->unknown52 = 0xFF;
  game_object->unknown53 = 0xFF;
  game_object->unknown54 = 0x7F;

  game_object->unknown57 = 0;
}

// size: 0x0000007C
void function_8003A720(void)
{
  BREAKPOINT;
  new_game_object(addr_to_pointer(a0));
}

// size: 0x0000060C
void function_80014564(void)
{
  ra = 0x8001457C;
  function_800163E4();

  if (lw(0x80076BB8)) return;

  a0 = 1;
  a1 = 0;
  ra = 0x8001459C;

  // CdReady -> 2
  function_80063BD8();
  
  if (v0 != 2 || (lw(0x800774B4) & 0x40) == 0) return;
  
  v1 = lw(0x80075864);
  if (v1 == 0) {
    v1 = lw(0x8007566C);
    read_disk2(
      lw(WAD_sector), 
      lw(lib_end_ptr), 
      0x800, 
      lw(WAD_header + 0x18 + v1*8), 
      0x258
    );
    sw(0x80075864, 1);
  } else if (v1 == 1) {
    spyro_memcpy32(WAD_nested_header, lw(lib_end_ptr), 0x1D0);
    v1 = lw(0x8007566C);
    read_disk2(
      lw(WAD_sector), 
      lw(lib_end_ptr), 
      lw(0x8006EE5C + v1*4), 
      lw(WAD_nested_header) + lw(WAD_header + 0x18 + v1*8), 
      0x258
    );
    sw(0x80075864, 2);
  } else if (v1 == 2) {
    RECT box = {0x200, 0, 0x200, (int32_t)lw(0x8006EE5C + lw(0x8007566C)*4) >> 10};
    LoadImage(&box, addr_to_pointer(lw(lib_end_ptr)));
    sw(0x80075864, 3);
  } else if (v1 == 3) {
    read_disk2(
      lw(WAD_sector), 
      lw(lib_end_ptr),
      lw(WAD_nested_header + 0x04) - 0x80000,
      lw(WAD_nested_header + 0x04) + lw(WAD_header + 0x18 + lw(0x8007566C)*8) + 0x80000,
      0x258
    );
    sw(0x80075864, 4);
  } else if (v1 == 4) {
    a0 = 0x1010;
    ra = 0x80014764;
    function_8005CB24();
    a0 = lw(lib_end_ptr);
    a1 = 0x0007EFF0;
    ra = 0x80014778;
    function_8005CAC4();
    while (test_spu_event(0) == 0);
    v1 = lw(0x8007566C);
    read_disk2(
      lw(WAD_sector), 
      lw(0x800785DC), 
      lw(WAD_nested_header + 0x0C), 
      lw(WAD_header + 0x18 + v1*8) + lw(WAD_nested_header + 0x08), 
      0x258
    );
    sw(0x80075864, 5);
  } else if (v1 == 5) {
    a0 = lw(0x800785DC);
    a1 = 1;
    ra = 0x800147F0;
    function_80012D58();
    sw(0x800785E0, v0);
    
    spyro_memcpy32(SKYBOX_DATA, 0x80077780, 4*5);

    if (lw(0x8007566C) == 1) {
      sw(0x80075864, 6);
      a2 = lw(WAD_nested_header + 0x14) - 0x60000;
    } else {
      sw(0x80075864, 8);
      a2 = lw(WAD_nested_header + 0x14);
    }
    read_disk2(
      lw(WAD_sector), 
      lw(0x800785E0), 
      a2, 
      lw(WAD_header + 0x18 + lw(0x8007566C)*8) + lw(WAD_nested_header + 0x10), 
      0x258
    );
  } else if (v1 == 6) {
    sw(0x80075864, 7);
  } else if (v1 == 7) {
    v1 = lw(0x8007566C);
    t1 = lw(WAD_nested_header + 0x14);
    read_disk2(
      lw(WAD_sector), 
      t1 + lw(0x800785E0) - 0x60000, 
      0x60000, 
      t1 + lw(WAD_header + 0x18 + v1*8) + lw(WAD_nested_header + 0x10) - 0x60000, 
      0x258
    );
    sw(0x80075864, 8);
  } else if (v1 == 8) {
    for (int i = 0; (int32_t)lw(WAD_nested_header + 0x50 + i*4) > 0; i++)
      sw(0x80076378 + 4 + i*4, func_800133E0(lw(WAD_nested_header + 0x50 + i*4) + lw(0x800785E0) - lw(WAD_nested_header + 0x10)));
    v1 = lw(0x8007566C);
    sw(0x800785E4, lw(WAD_nested_header + 0x14) + lw(0x800785E0));
    sw(0x80078604, lw(WAD_nested_header + 0x1C));
    sw(0x80078600, lw(WAD_nested_header + 0x18));
    read_disk2(
      lw(WAD_sector),
      lw(0x800785E4),
      lw(0x80078604),
      lw(WAD_header + 0x18 + v1*8) + lw(WAD_nested_header + 0x18),
      0x258
    );
    sw(0x80075864, 9);
  } else if (v1 == 9) {
    a0 = lw(0x800785E4);
    sw(0x80075680, a0);
    int32_t num2 = lw(a0 + 0x0C);
    sw(a0 + 0x10, lw(a0 + 0x10) + a0);
    for (int i = 0; i < num2; i++)
      sw(a0 + 0x14 + i*4, lw(a0 + 0x14 + i*4) + a0);
      
    struct game_object *game_objects = addr_to_pointer(a0 + lw(0x80078604));
    int32_t num1 = lw(lw(0x80075680) + 0x0C);
    sw(0x80075828, pointer_to_addr(game_objects));
    for (int32_t i = 0; i < num1; i++) {
      struct game_object *game_object = game_objects + i;
      new_game_object(game_object);
      game_object->render_distance = 0x20;
      game_object->modelID = i+1;
    }
    (game_objects + num1)->unknown48 = 0xFF;
    sw(0x80075864, 10);
  }
}

// size: 0x00000078
void function_8003FDC8(void)
{
  function_8003EA68();
  a0 = lw(0x80078AD0);
  v1 = lbu(0x8006C470 + a0);
  sb(0x80078A76, 0);
  sb(0x80078A70, v1);
  sb(0x80078A77, 1);
  sb(0x80078A7C, 0);
  sw(0x80078AAC, 0);
  sw(0x80078AB4, a0);
  sb(0x80078A71, v1);
}

// size: 0x0000015C
void function_80056B28(void)
{
  a0 = a0 ^ 0x00FFFFFF;
  for (int i = 0; i < 24; i++) {
    if (lhu(0x80075F3E + i*0x1C) & 0x83) {
      if ((a0 >> i) & 1) {
        v1 = 1 << i;
        sw(0x8007623C, lw(0x8007623C) | v1);
        v1 = lhu(0x80075F3E + i*0x1C) & 0x1C;
        if (v1 != 8) {
          if (v1 == 4) {
            v0 = lw(0x80075F30 + i*0x1C);
            if (v0) sb(v0 + 0x02A0, 0x7F);
          }
        } else {
          v0 = lw(0x80075F30 + i*0x1C);
          if (v0) sb(v0 + 0x54, 0x7F);
        }
        v0 = i*0x1C;
        sh(0x80075F3E + i*0x1C, 0x40);
        sw(0x80075F30 + i*0x1C, 0);
        sw(0x80075F48 + i*0x1C, 0);
        sb(0x80075F3D + i*0x1C, 0xFF);
      }
    }
  }
  sw(0x80076238, 0);
  a0 = 0;
  a1 = 4;
  function_800567F4();
}

// size: 0x0000009C
void function_800144C8(void)
{
  a0 = 0;
  function_80056B28();
  read_disk1(
    lw(WAD_sector),
    lw(0x800785E4),
    lw(0x80078604),
    lw(0x80078600) + lw(WAD_header + 0x50 + lw(CONTINUOUS_LEVEL_ID)*16),
    0x258
  );
  a0 = 1;
  function_8001364C();
  a0 = 0;
  if (lw(0x80075690))
    a0 = 0x20;
  function_8003FDC8();
  sw(0x80078BBC, 3);
}

// size: 0x00001074
void function_80015370(void)
{
  uint32_t temp, return_address = ra;
  sp -= 0x80;
  sw(sp + 0x78, ra);
  sw(sp + 0x74, s5);
  sw(sp + 0x70, s4);
  sw(sp + 0x6C, s3);
  sw(sp + 0x68, s2);
  sw(sp + 0x64, s1);
  sw(sp + 0x60, s0);

  s0 = a0;
  if ((int32_t)lw(0x80075864) >= 2) {
    ra = 0x800153AC;
    function_800163E4();
    if (lw(0x80076BB8)) goto label800163BC;

    a1 = 0;
    a0 = 1;
    ra = 0x800153CC;
    function_80063BD8();

    if (v0 != 2) goto label800163BC; // CdReady
    if ((lw(0x800774B4) & 0x40) == 0) goto label800163BC;
  }
  
  v1 = lw(0x80075864);
  v0 = v1 < 14;
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label800163BC;
  v0 = lw(0x80010A88 + v0);
  temp = v0;
  if (temp == return_address) return;
  switch (temp)
  {
  case 0x80015420: // 0
    goto label80015420;
    break;
  case 0x800155B4: // 1
    goto label800155B4;
    break;
  case 0x800155E4: // 2
    goto label800155E4;
    break;
  case 0x800156B4: // 3
    goto label800156B4;
    break;
  case 0x800156FC: // 4
    goto label800156FC;
    break;
  case 0x80015770: // 5
    goto label80015770;
    break;
  case 0x8001583C: // 6
    goto label8001583C;
    break;
  case 0x80015868: // 7
    goto label80015868;
    break;
  case 0x80015888: // 8
    goto label80015888;
    break;
  case 0x800158E0: // 9
    goto label800158E0;
    break;
  case 0x80015A54: // 10
    goto label80015A54;
    break;
  case 0x80015B80: // 11
    goto label80015B80;
    break;
  case 0x80015BD8: // 12
    goto label80015BD8;
    break;
  case 0x80015CCC: // 13
    goto label80015CCC;
    break;
  default:
    JR(temp, 0x80015418);
    return;
  }
label80015420:
  a0 = 0;
  ra = 0x80015428;
  function_80056B28();
  v0 = lw(0x8007576C);
  if ((int32_t)v0 < 0) goto label800154E8;
  a1 = lw(lw(0x80078640 + v0*4));
  v1 = lw(0x800785E8);
  v0 = lw(a1 + 0x14);
  s2 = a1 + 0x14;
  s1 = v0 + 0x400;
  s0 = v1 - s1;
  a0 = s0;
  a2 = s1;
  spyro_memcpy32(a0, a1, a2);
  s0 += 20; // 0x0014
  a0 = s0;
  s0 += 4; // 0x0004
  v0 = lw(s0 + 0x0000);
  s0 += 4; // 0x0004
  a1 = 0x80078A50;
  sw(a1 + 0x0000, v0);
  v0 = lw(s0 + 0x0000);
  s0 += 4; // 0x0004
  sw(SKYBOX_DATA + 4, s0);
  sw(SKYBOX_DATA, v0);
  temp = (int32_t)v0 <= 0;
  v1 = 0;
  if (temp) goto label8001556C;
  a1 -= 16; // 0xFFFFFFF0
label800154B8:
  v0 = lw(s0 + 0x0000);
  v1++;
  v0 += a0;
  v0 -= s2;
  sw(s0 + 0x0000, v0);
  v0 = lw(a1 + 0x0000);
  v0 = (int32_t)v1 < (int32_t)v0;
  temp = v0 != 0;
  s0 += 4; // 0x0004
  if (temp) goto label800154B8;
  goto label8001556C;
label800154E8:
  s3 = SKYBOX_DATA + 4;
  v0 = lw(s3 + 0x0000);
  v1 = lw(0x800785E8);
  s2 = v0 - 12; // 0xFFFFFFF4
  v0 = lw(v0 - 0x000C); // 0xFFFFFFF4
  a1 = s2;
  s1 = v0 + 1024; // 0x0400
  s0 = v1 - s1;
  a0 = s0;
  a2 = s1;
  ra = 0x8001551C;
  spyro_memcpy32(a0, a1, a2);
  a0 = s0;
  s0 += 8; // 0x0008
  v0 = lw(s0 + 0x0000);
  s0 += 4; // 0x0004
  sw(s3 + 0x0000, s0);
  sw(SKYBOX_DATA, v0);
  temp = (int32_t)v0 <= 0;
  v1 = 0;
  if (temp) goto label8001556C;
  a1 = s3 - 4; // 0xFFFFFFFC
label80015544:
  v0 = lw(s0 + 0x0000);
  v1++;
  v0 += a0;
  v0 -= s2;
  sw(s0 + 0x0000, v0);
  v0 = lw(a1 + 0x0000);
  v0 = (int32_t)v1 < (int32_t)v0;
  temp = v0 != 0;
  s0 += 4; // 0x0004
  if (temp) goto label80015544;
label8001556C:
  a1 = lw(0x800761D4);
  v0 = lw(0x800785E8);
  a2 = lw(0x80076234);
  v0 -= s1;
  s0 = v0 - a2;
  a0 = s0;
  ra = 0x80015594;
  spyro_memcpy32(a0, a1, a2);
  a0 = s0;
  a1 = 0;
  ra = 0x800155A0;
  function_80012CF0();
  v0 = -1; // 0xFFFFFFFF
  sw(0x80076E24, v0);
  v0 = 2; // 0x0002
  goto label800163B4;
label800155B4:
  a0 = 0;
  ra = 0x800155BC;
  function_80056B28();
  sw(SKYBOX_DATA, 0);
  sw(0x80076E24, -1);
  sw(0x80075864, lw(0x80075864)+1);
label800155E4:
  v0 = lw(0x800758B4);
  sw(LEVEL_ID, v0);
  ra = 0x800155FC;
  function_8005A470();
  a1 = lw(LEVEL_ID);
  a0 = (int32_t)a1 / 10;
  a1 -= a0*10;
  sw(0x80075688, lw(CONTINUOUS_LEVEL_ID));
  v0 = a0 - 1;
  v1 = v0*6;
  sw(WORLD_ID, v0);
  v0 = v1 + a1;
  sw(CONTINUOUS_LEVEL_ID, v0);
  if (s0) { // load level executable
    read_disk2(lw(WAD_sector), lw(exe_end_ptr), lw(WAD_header + 0x48 + 4 + v0*0x10), lw(WAD_header + 0x48 + 0 + v0*0x10), 0x258);
  }
  v0 = lw(0x80075864)+1;
  goto label800163B4;
label800156B4:
  a2 = 2048; // 0x0800
  a0 = lw(WAD_sector);
  v0 = lw(CONTINUOUS_LEVEL_ID);
  a1 = lw(0x800785DC);
  v0 = v0 << 4;
  a3 = lw(WAD_header + 0x0050 + v0);
  v0 = 600; // 0x0258
  sw(sp + 0x0010, v0);
  ra = 0x800156EC;
  read_disk2(a0, a1, a2, a3, lw(sp+0x10));
  v0 = lw(0x80075864);
  v0++;
  goto label800163B4;
label800156FC:
  s1 = WAD_nested_header + 0x00;
  spyro_memcpy32(WAD_nested_header, lw(0x800785DC), 0x1D0);
  v0 = lw(CONTINUOUS_LEVEL_ID);
  read_disk2(
    lw(WAD_sector),
    lw(0x800785DC), 0x80000,
    lw(WAD_header + 0x50 + v0*0x10) + lw(WAD_nested_header + 0x00),
    0x258
  );
  sw(0x8007576C, 0);
  v0 = lw(0x80075864)+1;
  goto label800163B4;
label80015770:
  v0 = lw(0x8007576C);
  v1 = lw(0x800785DC);
  a1 = v0 << 18;
  a1 += v1;
  v0 = v0*0x100;
  RECT rect1 = {0x200, v0, 0x200, 0x100};
  LoadImage(&rect1, addr_to_pointer(a1));
  v0 = lw(0x8007576C);
  v1 = 2;
  v0++;
  sw(0x8007576C, v0);
  temp = v0 != v1;
  a2 = 0xFFF80000;
  if (temp) goto label800163BC;
  a0 = lw(WAD_sector);
  a1 = lw(0x800785DC);
  v0 = lw(WAD_nested_header + 0x04);
  v1 = lw(CONTINUOUS_LEVEL_ID);
  t0 = lw(WAD_nested_header + 0x00);
  a2 += v0;
  v1 = v1 << 4;
  v0 = lw(0x80075864);
  v1 = lw(WAD_header + 0x0050 + v1);
  v0++;
  sw(0x80075864, v0);
  t0 += v1;
  a3 = 0x00080000 + t0;
  ra = 0x80015834;
  read_disk2(a0, a1, a2, a3, 0x0258);
  goto label800163BC;
label8001583C:
  a0 = 0x1010;
  ra = 0x80015844;
  function_8005CB24();
  a0 = lw(0x800785DC);
  a1 = 0x0007EFF0;
  ra = 0x80015858;
  function_8005CAC4();
  v0 = lw(0x80075864);
  v0++;
  goto label800163B4;
label80015868:
  if (test_spu_event(0) == 0) goto label800163BC;
  v0 = lw(0x80075864)+1;
  goto label800163B4;
label80015888:
  v0 = lw(CONTINUOUS_LEVEL_ID);
  read_disk2(
    lw(WAD_sector),
    lw(0x800785DC),
    lw(WAD_nested_header + 0x0C),
    lw(WAD_nested_header + 0x08) + lw(WAD_header + 0x50 + v0*0x10),
    0x258
  );
  v0 = lw(0x80075864) + 1;
  goto label800163B4;
label800158E0:
  a0 = lw(0x800785DC);
  a1 = 0;
  ra = 0x800158F0;
  function_80012D58();
  a2 = SKYBOX_DATA;
  v1 = lw(a2 + 0x0000);
  sw(0x800785E0, v0);
  temp = v1 == 0;
  if (temp) goto label800159FC;
  v1 = lw(0x800757D8);
  v0 = 5; // 0x0005
  temp = v1 == v0;
  if (temp) goto label800159FC;
  v0 = 12; // 0x000C
  temp = v1 == v0;
  if (temp) goto label800159FC;
  v1 = lw(0x800758AC);
  temp = v1 == 0;
  if (temp) goto label800159CC;
  v0 = lw(0x800758BC);
  temp = (int32_t)v0 <= 0;
  s0 = 0;
  if (temp) goto label800159FC;
  a2 = v1;
  a1 = v0;
  a0 = 0x80078640;
label80015964:
  v1 = lw(a0 + 0x0000);
  v0 = lw(v1 + 0x001C);
  temp = v0 == a2;
  s0++;
  if (temp) goto label80015990;
  v0 = (int32_t)s0 < (int32_t)a1;
  temp = v0 != 0;
  a0 += 4; // 0x0004
  if (temp) goto label80015964;
  goto label800159FC;
label80015990:
  v0 = lw(v1 + 0x0000);
  a2 = SKYBOX_DATA;
  v1 = lw(v0 + 0x0000);
  a0 = lw(v0 + 0x0004);
  a1 = lw(v0 + 0x0008);
  sw(a2 + 0x0000, v1);
  sw(a2 + 0x0004, a0);
  sw(a2 + 0x0008, a1);
  v1 = lw(v0 + 0x000C);
  a0 = lw(v0 + 0x0010);
  sw(a2 + 0x000C, v1);
  sw(a2 + 0x0010, a0);
  goto label800159FC;
label800159CC:
  a1 = 0x80077780;
  v0 = lw(a1 + 0x0000);
  v1 = lw(a1 + 0x0004);
  a0 = lw(a1 + 0x0008);
  sw(a2 + 0x0000, v0);
  sw(a2 + 0x0004, v1);
  sw(a2 + 0x0008, a0);
  v0 = lw(a1 + 0x000C);
  v1 = lw(a1 + 0x0010);
  sw(a2 + 0x000C, v0);
  sw(a2 + 0x0010, v1);
label800159FC:
  a0 = lw(WAD_sector);
  a1 = lw(0x800785E0);
  v0 = lw(CONTINUOUS_LEVEL_ID);
  a2 = lw(WAD_nested_header + 0x0014);
  v1 = lw(WAD_nested_header + 0x0010);
  v0 = v0 << 4;
  a3 = lw(WAD_header + 0x0050 + v0);
  v0 = 600; // 0x0258
  sw(sp + 0x0010, v0);
  a3 += v1;
  ra = 0x80015A44;
  read_disk2(a0, a1, a2, a3, lw(sp+0x10));
  v0 = lw(0x80075864);
  v0++;
  goto label800163B4;
label80015A54:
  temp = s0 == 0;
  s0 = 0;
  if (temp) goto label80015A84;
  a0 = lw(WAD_nested_header + 0x0050);
  v0 = lw(WAD_nested_header + 0x0010);
  v1 = lw(0x800785E0);
  a0 -= v0;
  a0 += v1;
  ra = 0x80015A80;
  function_80013230();
  s0 = 0;
label80015A84:
  a1 = 0x00FFFFFF;
  a2 = 0x800785FC;
  a0 = 0x80076378;
label80015A9C:
  v1 = lw(a0 + 0x0000);
  v0 = lw(a2 + 0x0000);
  v1 = v1 & a1;
  v0 = v0 & a1;
  v1 = (int32_t)v1 < (int32_t)v0;
  temp = v1 == 0;
  if (temp) goto label80015ABC;
  sw(a0 + 0x0000, 0);
label80015ABC:
  s0++;
  v0 = (int32_t)s0 < 512;
  temp = v0 != 0;
  a0 += 4; // 0x0004
  if (temp) goto label80015A9C;
  s0 = 1; // 0x0001
  s4 = 0x80076378;
  s3 = WAD_nested_header + 0x0050;
  s2 = s3 + 258; // 0x0102
  s1 = s3 + 4; // 0x0004
label80015AE8:
  v1 = lw(s1 + 0x0000);
  temp = (int32_t)v1 <= 0;
  s1 += 4; // 0x0004
  if (temp) goto label80015B34;
  s0++;
  a0 = lw(s3 - 0x0040); // 0xFFFFFFC0
  v0 = lw(0x800785E0);
  a0 = v1 - a0;
  a0 += v0;
  ra = 0x80015B14;
  function_800133E0();
  v1 = lh(s2 + 0x0000);
  v1 = v1 << 2;
  v1 += s4;
  sw(v1 + 0x0000, v0);
  v0 = (int32_t)s0 < 64;
  temp = v0 != 0;
  s2 += 2; // 0x0002
  if (temp) goto label80015AE8;
label80015B34:
  v0 = lw(0x800785E0);
  a0 = lw(s3 - 0x003C); // 0xFFFFFFC4
  a1 = lw(s3 - 0x0034); // 0xFFFFFFCC
  a2 = lw(s3 - 0x0038); // 0xFFFFFFC8
  v1 = lw(0x80075864);
  v0 += a0;
  v1++;
  sw(0x800785E4, v0);
  sw(0x80078604, a1);
  sw(0x80078600, a2);
  sw(0x80075864, v1);
  goto label800163BC;
label80015B80:
  a0 = lw(WAD_sector);
  a1 = lw(0x800785E4);
  v0 = lw(CONTINUOUS_LEVEL_ID);
  a2 = lw(0x80078604);
  v1 = lw(0x80078600);
  v0 = v0 << 4;
  a3 = lw(WAD_header + 0x0050 + v0);
  v0 = 600; // 0x0258
  sw(sp + 0x0010, v0);
  a3 += v1;
  ra = 0x80015BC8;
  read_disk2(a0, a1, a2, a3, lw(sp+0x10));
  v0 = lw(0x80075864);
  v0++;
  goto label800163B4;
label80015BD8:
  v0 = lw(0x800756D0);
  temp = v0 != 0;
  if (temp) goto label80015BFC;
  v0 = lw(0x80075864);
  v0++;
  goto label800163B4;
label80015BFC:
  v0 = lw(0x80075910);
  temp = v0 != 0;
  if (temp) goto label800163BC;
  v0 = lw(0x800758AC);
  temp = v0 == 0;
  if (temp) goto label80015C78;
  v0 = lw(0x80078B74);
  v1 = lw(0x80076E60);
  v0 += v1;
  v1 = v0 & 0xFFF;
  v0 = (int32_t)v1 < 2049;
  temp = v0 != 0;
  if (temp) goto label80015C50;
  v1 -= 4096; // 0xFFFFF000
label80015C50:
  temp = (int32_t)v1 >= 0;
  v0 = v1;
  if (temp) goto label80015C5C;
  v0 = -v0;
label80015C5C:
  v0 = (int32_t)v0 < 128;
  temp = v0 == 0;
  if (temp) goto label800163BC;
  v0 = lw(0x80075864);
  v0++;
  goto label800163B4;
label80015C78:
  v0 = lbu(0x80078A66);
  v1 = lbu(0x80078C5E);
  v0 -= v1;
  v1 = v0 & 0xFF;
  v0 = (int32_t)v1 < 129;
  temp = v0 != 0;
  if (temp) goto label80015CA4;
  v1 -= 256; // 0xFFFFFF00
label80015CA4:
  temp = (int32_t)v1 >= 0;
  v0 = v1;
  if (temp) goto label80015CB0;
  v0 = -v0;
label80015CB0:
  v0 = (int32_t)v0 < 16;
  temp = v0 == 0;
  if (temp) goto label800163BC;
  v0 = lw(0x80075864);
  v0++;
  goto label800163B4;
label80015CCC:
  v0 = lw(CONTINUOUS_LEVEL_ID);
  v0 = lbu(0x80078E78 + v0);
  temp = v0 != 0;
  v0 = 28800; // 0x7080
  if (temp) goto label80015CF0;
  v0 = 0 | 0xA8C0;
label80015CF0:
  sw(0x800776CC, v0);
  s3 = 0x80077888;
  a0 = s3;
  v1 = lw(CONTINUOUS_LEVEL_ID);
  a1 = 0;
  v0 = v1 << 2;
  v0 = lw(0x8006EF9C + v0);
  sw(0x800774B0, v0);
  v0 = 1; // 0x0001
  sb(0x80078E78 + v1, v0);
  a2 = 104; // 0x0068
  ra = 0x80015D40;
  spyro_memset32(a0, a1, a2);
  v0 = lw(CONTINUOUS_LEVEL_ID);
  v0 = v0 << 2;
  v0 = lw(0x80077420 + v0);
  sw(0x80075830, 0);
  sw(0x800756C8, 0);
  sw(0x8007587C, v0);
  a0 = s0;
  ra = 0x80015D7C;
  function_8001364C();
  v0 = lw(0x800756D0);
  temp = v0 == 0;
  if (temp) goto label800162D4;
  v0 = lw(0x80075690);
  temp = v0 == 0;
  v0 = 10; // 0x000A
  if (temp) goto label80015EB4;
  s1 = 0x80076DF8;
  a0 = s1;
  s0 = 0x80078AD4;
  sw(s0 + 0x0000, v0);
  ra = 0x80015DC0;
  function_80033F08();
  s0 -= 124; // 0xFFFFFF84
  a0 = s0;
  v0 = lw(0x80076E60);
  v1 = lw(0x80078B74);
  v0 += v1;
  sw(0x80076E60, v0);
  a1 = s3 + 80; // 0x0050
  ra = 0x80015DF0;
  spyro_vec3_copy(a0, a1);
  a1 = 0x80076E60;
  a0 = 0x80076E48;
  v0 = lw(a1 + 0x0000);
  v1 = lw(a1 + 0x0004);
  sw(a0 + 0x0000, v0);
  sw(a0 + 0x0004, v1);
  v0 = lw(a1 + 0x0008);
  v1 = lw(a1 + 0x000C);
  sw(a0 + 0x0008, v0);
  sw(a0 + 0x000C, v1);
  v0 = lw(a1 + 0x0010);
  v1 = lw(a1 + 0x0014);
  sw(a0 + 0x0010, v0);
  sw(a0 + 0x0014, v1);
  v0 = lw(0x800778E4);
  sb(0x80078A66, v0);
  v0 = v0 & 0xFF;
  v0 = v0 << 4;
  sw(0x80078B74, v0);
  v0 = lbu(0x80078A66);
  v1 = lw(0x80076E48);
  v0 = v0 << 4;
  v1 -= v0;
  sw(0x80076E48, v1);
  a0 = s1;
  ra = 0x80015E78;
  function_80034204();
  a0 = s1;
  a1 = s1;
  a2 = s0;
  ra = 0x80015E88;
  spyro_vec3_add(a0, a1, a2);
  ra = 0x80015E90;
  function_800342F8();
  s0 = 0x8006EBE4;
  a0 = s0;
  a1 = s1;
  ra = 0x80015EA4;
  spyro_vec3_copy(a0, a1);
  a0 = s0 - 24; // 0xFFFFFFE8
  ra = 0x80015EAC;
  function_80037714();
  v0 = 9; // 0x0009
  goto label800162C4;
label80015EB4:
  v0 = lw(0x800758AC);
  temp = v0 == 0;
  if (temp) goto label800160C8;
  v0 = lw(0x800758BC);
  temp = (int32_t)v0 <= 0;
  s0 = 0;
  if (temp) goto label80016314;
  s1 = 0x80078A66;
  s2 = s1 + 270; // 0x010E
  s4 = 0x80076DF8;
  s3 = s4 + 80; // 0x0050
  v0 = s0 << 2;
label80015EF8:
  a0 = lw(0x80078640 + v0);
  v0 = lw(0x800758AC);
  v1 = lw(a0 + 0x001C);
  temp = v1 != v0;
  s0++;
  if (temp) goto label800160A8;
  v1 = lw(a0 + 0x0018);
  s5 = lbu(s1 + 0x0000);
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 2;
  v0 -= v1;
  v1 = lw(0x80075828);
  v0 = v0 << 3;
  v0 += v1;
  s0 = lw(v0 + 0x0000);
  a0 = 15; // 0x000F
  ra = 0x80015F50;
  function_8003EA68();
  a0 = s4;
  v0 = 9; // 0x0009
  sw(s1 + 0x006E, v0);
  ra = 0x80015F60;
  function_80033F08();
  v0 = lw(0x80076E60);
  v1 = lw(s1 + 0x010E);
  v0 += v1;
  sw(0x80076E60, v0);
  v0 = lw(s0 + 0x0010);
  temp = v0 == 0;
  a0 = s1 - 14; // 0xFFFFFFF2
  if (temp) goto label80015FC8;
  a1 = lw(s0 + 0x0000);
  a1 += 8; // 0x0008
  ra = 0x80015F98;
  spyro_vec3_copy(a0, a1);
  a1 = lw(s0 + 0x0000);
  a0 = s1 + 490; // 0x01EA
  a1 += 24; // 0x0018
  ra = 0x80015FA8;
  spyro_vec3_copy(a0, a1);
  v0 = lw(s0 + 0x0000);
  a3 = lw(v0 + 0x0018);
  a0 = lw(v0 + 0x0008);
  v1 = lw(v0 + 0x001C);
  a1 = lw(v0 + 0x000C);
  a2 = 0;
  goto label80015FFC;
label80015FC8:
  a1 = lw(s0 + 0x0000);
  a1 += 24; // 0x0018
  ra = 0x80015FD4;
  spyro_vec3_copy(a0, a1);
  a1 = lw(s0 + 0x0000);
  a0 = s1 + 490; // 0x01EA
  a1 += 8; // 0x0008
  ra = 0x80015FE4;
  spyro_vec3_copy(a0, a1);
  v0 = lw(s0 + 0x0000);
  a2 = 0;
  a3 = lw(v0 + 0x0008);
  a0 = lw(v0 + 0x0018);
  v1 = lw(v0 + 0x000C);
  a1 = lw(v0 + 0x001C);
label80015FFC:
  a0 = a3 - a0;
  a1 = v1 - a1;
  ra = 0x80016008;
  v0 = spyro_atan2(a0, a1, a2);
  sb(s1 + 0x0000, v0);
  v0 = lw(s3 + 0x0018);
  v1 = lw(s3 + 0x001C);
  a0 = lw(s3 + 0x0020);
  a1 = lw(s3 + 0x0024);
  sw(s3 + 0x0000, v0);
  sw(s3 + 0x0004, v1);
  sw(s3 + 0x0008, a0);
  sw(s3 + 0x000C, a1);
  v0 = lw(s3 + 0x0028);
  v1 = lw(s3 + 0x002C);
  sw(s3 + 0x0010, v0);
  sw(s3 + 0x0014, v1);
  v0 = lbu(s2 - 0x010E); // 0xFFFFFEF2
  a0 = s4;
  v0 = v0 << 4;
  sw(s2 + 0x0000, v0);
  v0 = lbu(s2 - 0x010E); // 0xFFFFFEF2
  v1 = lw(s3 + 0x0000);
  v0 = v0 << 4;
  v1 -= v0;
  sw(s3 + 0x0000, v1);
  ra = 0x80016064;
  function_80034204();
  a0 = s4;
  a1 = a0;
  a2 = s2 - 284; // 0xFFFFFEE4
  ra = 0x80016074;
  spyro_vec3_add(a0, a1, a2);
  ra = 0x8001607C;
  function_800342F8();
  v1 = lbu(s2 - 0x010E); // 0xFFFFFEF2
  v0 = 127; // 0x007F
  sb(s2 - 0x00F5, v0); // 0xFFFFFF0B
  sw(0x800758FC, 0);
  v1 = s5 - v1;
  v1 = v1 << 4;
  sw(0x80075858, v1);
  goto label80016314;
label800160A8:
  v0 = lw(0x800758BC);
  v0 = (int32_t)s0 < (int32_t)v0;
  temp = v0 != 0;
  v0 = s0 << 2;
  if (temp) goto label80015EF8;
  goto label80016314;
label800160C8:
  a0 = 15; // 0x000F
  ra = 0x800160D0;
  function_8003EA68();
  s1 = 0x80076DF8;
  a0 = s1;
  ra = 0x800160E0;
  function_80033F08();
  s0 = 0x80078B74;
  s2 = s0 - 284; // 0xFFFFFEE4
  a0 = s2;
  v0 = lw(0x80076E60);
  v1 = lw(s0 + 0x0000);
  v0 += v1;
  sw(0x80076E60, v0);
  v0 = lw(0x800778E4);
  t0 = 0x80076E60;
  a3 = 0x80076E48;
  v1 = lw(t0 + 0x0000);
  a2 = lw(t0 + 0x0004);
  sw(a3 + 0x0000, v1);
  sw(a3 + 0x0004, a2);
  v1 = lw(t0 + 0x0008);
  a2 = lw(t0 + 0x000C);
  sw(a3 + 0x0008, v1);
  sw(a3 + 0x000C, a2);
  v1 = lw(t0 + 0x0010);
  a2 = lw(t0 + 0x0014);
  sw(a3 + 0x0010, v1);
  sw(a3 + 0x0014, a2);
  sb(0x80078A66, v0);
  v0 = v0 & 0xFF;
  v0 = v0 << 4;
  sw(s0 + 0x0000, v0);
  v0 = lbu(0x80078A66);
  v1 = lw(0x80076E48);
  v0 = v0 << 4;
  v1 -= v0;
  sw(0x80076E48, v1);
  a1 = s3 + 80; // 0x0050
  ra = 0x80016190;
  spyro_vec3_copy(a0, a1);
  a0 = s1;
  ra = 0x80016198;
  function_80034204();
  a0 = s0 - 16; // 0xFFFFFFF0
  ra = 0x800161A0;
  spyro_vec3_clear(a0);
  v1 = lw(0x80076EA8);
  v0 = 0x8006CA84;
  temp = v1 != v0;
  a0 = s1;
  if (temp) goto label80016284;
  v0 = lw(spyro_position + 8);
  a2 = lbu(0x80078A66);
  v0 -= 5632; // 0xFFFFEA00
  a2 = a2 << 1;
  sw(spyro_position + 8, v0);
  v1 = lh(spyro_cos_lut + a2);
  a1 = a0;
  v0 = v1 << 2;
  v0 += v1;
  v1 = lw(s0 - 0x011C); // 0xFFFFFEE4
  v0 = (int32_t)v0 >> 1;
  v1 -= v0;
  sw(s0 - 0x011C, v1); // 0xFFFFFEE4
  v1 = lh(spyro_sin_lut + a2);
  v0 = v1 << 2;
  v0 += v1;
  v1 = lw(spyro_position + 4);
  v0 = (int32_t)v0 >> 1;
  v1 -= v0;
  sw(spyro_position + 4, v1);
  a2 = s2;
  ra = 0x80016238;
  spyro_vec3_add(a0, a1, a2);
  ra = 0x80016240;
  function_800342F8();
  v0 = lw(0x80078AD0);
  v1 = lbu(0x8006C588 + v0);
  v0 = 11; // 0x000B
  sw(0x80078AD4, v0);
  v0 = 4; // 0x0004
  sw(0x800757D8, 0);
  sb(0x80078A7F, v0);
  sw(0x80076E90, v1);
  goto label80016314;
label80016284:
  a1 = s1;
  a2 = s2;
  ra = 0x80016290;
  spyro_vec3_add(a0, a1, a2);
  ra = 0x80016298;
  function_800342F8();
  s0 = 0x8006EBE4;
  a0 = s0;
  a1 = s1;
  ra = 0x800162AC;
  spyro_vec3_copy(a0, a1);
  a0 = s0 - 24; // 0xFFFFFFE8
  ra = 0x800162B4;
  function_80037714();
  v0 = 10; // 0x000A
  sw(0x80078AD4, v0);
  v0 = 9; // 0x0009
label800162C4:
  sw(0x800757D8, v0);
  goto label80016314;
label800162D4:
  v0 = lw(0x80075690);
  temp = v0 == 0;
  v0 = 12; // 0x000C
  if (temp) goto label800162F8;
  sw(0x80078AD4, 0);
  goto label80016314;
label800162F8:
  v1 = lw(0x800757D8);
  temp = v1 == v0;
  if (temp) goto label80016314;
  a0 = 0;
  ra = 0x80016314;
  function_8003EA68();
label80016314:
  spyro_memcpy32(SKYBOX_DATA, 0x80077780, 5*4);
  sw(0x800756D0, 0);
  sw(0x800758AC, 0);
  sw(0x80075870, 0);
  sw(0x80075874, 0);
  sw(0x80075838, 0);
  sw(0x8007583C, 0);
  sw(0x80075670, 0);
  sw(0x80075674, 0);
  sw(0x8007577C, 0);
  sw(0x80075854, 0);
  sw(0x80075850, 0);
  sw(0x80075818, -1);
  v0 = -1;
label800163B4:
  sw(0x80075864, v0);
label800163BC:
  ra = lw(sp + 0x78);
  s5 = lw(sp + 0x74);
  s4 = lw(sp + 0x70);
  s3 = lw(sp + 0x6C);
  s2 = lw(sp + 0x68);
  s1 = lw(sp + 0x64);
  s0 = lw(sp + 0x60);
  sp += 0x80;\
}

// size: 0x000000C0
uint32_t completion_percentage(void)
{
  uint32_t gems = lw(total_found_gems);
  uint32_t dragons = lw(total_found_dragons);
  uint32_t eggs = lw(total_found_eggs);
  uint32_t score = (gems*50 + dragons*6000 + eggs*10000)/12000;
  if (gems > 12000)
    score = 100 + (gems - 12000) / 100;
  return score;
}

// size: 0x000000C0
void function_8002BB20(void)
{
  BREAKPOINT;
  v0 = completion_percentage();
}

// size: 0x00000088
void game_loop(void)
{
  function_8005B988();
  
  initial_loading_screen();
  
  while (1) {
    sb(gp + 0x0604, 0);

    ra = 0x80012238;
    function_8003385C();

    sb(gp + 0x0604, 1);
    sw(gp + 0x0468, lw(gp + 0x04FC));

    if (lw(gp + 0x0468) < 2)
      sw(gp + 0x0468, 2);

    if (lw(gp + 0x0468) > 4)
      sw(gp + 0x0468, 4);
    
    sw(gp + 0x04FC, 0);
    if (lw(gp + 0x0538) == 0) {
      ra = 0x80012284;
      function_8001ED5C();
    }
  }
}

// size: 0x00000088
void function_80012204(void)
{
  BREAKPOINT;
  game_loop();
}

// size: 0x00000178
void function_80012604(void)
{
  for (int i = 0; i < 36; i++) {
    sw(0x80076FE8 + i*4, 0);
    sw(0x800772D8 + i*4, 0);
    if ((int32_t)i < 18)
      sw(0x80077420 + i*4, 0);
    
    sb(0x80078E78 + i, 0);
    sb(0x8007A6A8 + i, 0);
  }

  for (int i = 0; i < 6; i++) // loop over worlds?
    sb(0x800758D5 + i, 0);

  sb(0x800758D0, 2);
  for (int i = 0; i < 6; i++) { // loop over worlds?
    sw(0x80078618+i*4, 0);
    for (int j = 0; j < 5; j++) // loop over levels?
      sb(0x80078680+i*5+j, 0);
  }

  sw(0x80078BBC, 3);
  sw(0x8007582C, 4);
  sw(total_found_gems, 0);
  sw(total_found_dragons, 0);
  sw(total_found_eggs, 0);
  sw(0x8007587C, 0);
  sw(0x800756C8, 0);
  sw(0x80075830, 0);
  sw(0x800758E8, 0);
  sw(0x8007580C, -1);
  sw(0x80075838, 0);
  sw(0x8007583C, 0);
  spyro_memset32(0x80077888, 0, 0x68);
  spyro_memset32(0x80077908, 0, 0x480);
}

// size: 0x00000044
void function_8001277C(void)
{
  sw(LEVEL_ID, 10);
  sw(0x800757A4, 1);
  sw(CAMERA_MODE, 0x52);
  function_80012604();
}