#include "spyro_game.h"
#include "debug.h"
#include "main.h"
#include "decompilation.h"
#include "psx_mem.h"
#include "spyro_graphics.h"
#include "spyro_string.h"
#include "spyro_math.h"
#include "spyro_vsync.h"
#include "psx_ops.h"
#include "int_math.h"
#include "not_renamed.h"
#include "level_loading.h"
#include "function_chooser.h"

#include "title_screen.h"
#include "credits.h"

#include <string.h>

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
      function_loaded_800758CC();
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



// size: 0x000003F4
void function_8003385C(void)
{
  uint32_t temp;
  sw(0x8007579C, 0); // &0x00000000
  function_8005637C();
  function_8002BBE0();
  switch (lw(0x800757D8)) {
  case 0: 
    sw(level_frame_counter, lw(level_frame_counter)+1);
    if (lw(IS_DEMO_MODE)) {
      function_800334D4();
    }
    a0 = lw(0x800756CC); // &0x00000000
    function_8002A6FC();
    function_loaded_80075734();
    v1 = lw(0x800757D8); // &0x00000000
    if (v1 == 1 || v1 == 7 || v1 == 11 || v1 == 12) return;
    function_8004A200();
    v1 = lw(0x800757D8);
    if (v1 == 1 || v1 == 4 || v1 == 5 || v1 == 7) return;
    a0 = lw(0x800756CC);
    function_loaded_800756BC();
    
    if (lw(0x80075690) == 0)
      function_80054988();
    
    function_80058BD8();
    function_80037BD4();

    if (lw(0x80075918))
      sw(0x80075918, max_int(lw(0x80075918) - lw(0x800756CC), 0));
    
    if (lw(0x800757D8)) break;
    if (lw(0x8007570C)) break;
    if ((int32_t)lw(0x80078BBC) < 0) break;
    if (lw(0x80076E28) == 0x8000000E) break;

    if (lw(0x80077384) < 2 || lw(buttons_press) & 0x800) {
      BREAKPOINT;
      a0 = 1;
      function_8002C420();
    } else if (lw(buttons_press) & 0x100) {
      a0 = 1;
      function_8002C714();
    }
    break;
  case 1:
    function_8002DF9C();
    break;
  case 2:
    function_8002E12C();
    break;
  case 3:
    function_8002EB2C();
  case 4:
  case 5:
    function_8002EDF0();
    break;
  case 6:
    function_8002F3C4(); // this does nothing
    break;
  case 7:
    v0 = lw(0x800757C0);
    temp = v0;
    switch (temp)
    {
    case 0x8007B1FC:
      function_8007B1FC();
      break;
    default:
      JALR(temp, 0x8003392C);
    }
    break;
  case 8:
    function_8002F3E4();
    break;
  case 9:
    function_8002E000();
    break;
  case 10:
    function_8002E084();
    break;
  case 11:
    function_800314B4();
    break;
  case 12:
    function_800324D8();
    break;
  case 13:
    if (lw(0x80078D78) != 3)
      function_8007ABAC_title_screen();
    else
      function_80032B08();
    break;
  case 14:
    function_800331AC();
    break;
  case 15:
    if ((int32_t)lw(0x80075704) >= 99)
      function_800333DC();
    else
      function_8007AA50_credits();
    break;
  }
  function_80056ED4();
}

// size: 0x00000088
void game_loop(void)
{
  function_8005B988();
  
  initial_loading_screen();
  
  while (1) {

    if (start_frame()) return;
    
    sb(gp + 0x0604, 0);

    function_8003385C();

    sb(gp + 0x0604, 1);
    sw(gp + 0x0468, lw(gp + 0x04FC));

    if (lw(gp + 0x0468) < 2)
      sw(gp + 0x0468, 2);

    if (lw(gp + 0x0468) > 4)
      sw(gp + 0x0468, 4);
    
    sw(gp + 0x04FC, 0);
    if (lw(gp + 0x0538) == 0) {
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