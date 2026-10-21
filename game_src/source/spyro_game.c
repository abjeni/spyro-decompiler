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
#include "extra_gte.h"
#include "gte.h"

#include "title_screen.h"

#include <string.h>

// size: 0x00000324
void function_800526A8(void)
{
  struct game_object *object = addr_to_pointer(a0);

  at = lw(0x80076378 + object->modelID*4);
  v0 = lw(at + 0x38 + object->unknown3C*4);
  a1 = lbu(v0 + 0x0C);
  object->unknown41 = a1;
  v1 = (lw(v0 + 0x24 + object->unknown3E*8) >> 21) & 0x07;
  v0 = lw(at + 0x14 + v1*4);
  at = lw(at + 0x14);
  object->unknown08 = v0;
  object->cellID = -1;
  if (at) {
    v0 = object->p.x >> 13;
    v1 = object->p.y >> 13;
    v0 = v0 + (v1 << 5);
    if ((int32_t)at > 0)
      v0 += 1 << 10;
    object->cellID = v0;
    v0 = lw(0x80075778) + v0*4;
    object->unknown04 = lw(v0);
    sw(v0, pointer_to_addr(object));
  }

  mat3 m = mat3_identity();

  uint32_t rotY = object->rotz;
  if (rotY) m = mat3_mul(m, mat3rotY(-rotY*16));

  uint32_t rotX = object->roty;
  if (rotX) m = mat3_mul(m, mat3rotX(rotX*16));
  
  uint32_t rotZ = object->rotx;
  if (rotZ) m = mat3_mul(m, mat3rotZ(-rotZ*16));

  object->m = m;
}

// size: 0x00000380
void function_800529E4(void)
{
  struct game_object *object = addr_to_pointer(a0);
  if (a1 & 1) {
    at = lw(0x80076378 + object->modelID*4);
    v1 = lw(a0 + 0x3C);
    if (object->unknown40 <= 0) {
      v0 = object->unknown3C;
      v1 = object->unknown3E;
    } else {
      v0 = object->unknown3D;
      v1 = object->unknown3F;
    }
    v0 = lw(at + 0x38 + v0*4) + v1*8;
    v1 = (lw(v0 + 0x24) >> 21) & 0x07;
    object->unknown08 = lw(at + 0x14 + v1*4);
  }

  if (a1 & 2) {
    at = object->p.x >> 13;
    v0 = object->p.y >> 13;
    at = at + (v0 << 5);
    v1 = lw(0x80075778);

    a2 = (int32_t)object->cellID;
    a3 = a2 & 0x400;
    at = at | a3;
    if (at != a2 && (int32_t)a2 >= 0) {
      a2 = v1 + a2*4;
      v0 = a2 - 4;
      do {
        a2 = v0 + 4;
        v0 = lw(a2);
      } while (addr_to_pointer(v0) != object);
      sw(a2, object->unknown04);
      object->cellID = at;
      at = v1 + at*4;
      object->unknown04 = lw(at);
      sw(at, a0);
    }
  }

  if (a1 & 4) {
    mat3 m = mat3_identity();

    uint32_t rotY = object->rotz;
    if (rotY) m = mat3_mul(m, mat3rotY(-rotY*16));

    uint32_t rotX = object->roty;
    if (rotX) m = mat3_mul(m, mat3rotX(rotX*16));
    
    uint32_t rotZ = object->rotx;
    if (rotZ) m = mat3_mul(m, mat3rotZ(-rotZ*16));

    object->m = m;
  }
  return;
}

// size: 0x000001D4
// struct game_object *a0
void function_80052D64(void)
{
  struct game_object *object = addr_to_pointer(a0);
  t7 = a1;
  t9 = a2;
  set_TR(0, 0, 0);
  at = lw(0x80076378 + object->modelID*4);
  a2 = object->unknown40;
  v0 = lw(at + 0x38 + object->unknown3C*4);
  v1 = lw(at + 0x38 + object->unknown3D*4);
  a0 = lw(v0 + 0x24 + object->unknown3E*8) & 0x1FFFFF;
  at = lb(v0 + 0x05);
  v0 = lw(a0 + t7*4);
  if (a2 == 0) {
    a1 = ((int32_t)(v0 << 20) >> 19) << at;
    a0 = ((int32_t)(v0 << 10) >> 21) << at;
    v1 = ((int32_t)(v0 <<  0) >> 21) << at;
  } else {
    t1 = ((int32_t)(v0 << 20) >> 19) << at;
    t0 = ((int32_t)(v0 << 10) >> 21) << at;
    a3 = ((int32_t)(v0 <<  0) >> 21) << at;
    a1 = lw(v1 + 0x24 + object->unknown3F*8) & 0x1FFFFF;
    v1 = 64 - a2;
    cop2.IR0 = v1;
    cop2.IR1 = a3;
    cop2.IR2 = t0;
    cop2.IR3 = t1;
    a1 += t7*4;
    v1 = lw(a1 + 0x0000);
    GPF(SF_OFF, LM_OFF);
    a1 = ((int32_t)(v1 << 20) >> 19) << at;
    a0 = ((int32_t)(v1 << 10) >> 21) << at;
    v1 = ((int32_t)(v1 <<  0) >> 21) << at;
    cop2.IR0 = a2;
    cop2.IR1 = v1;
    cop2.IR2 = a0;
    cop2.IR3 = a1;
    GPL(SF_OFF, LM_OFF);
    v1 = (int32_t)cop2.MAC1 >> 6;
    a0 = (int32_t)cop2.MAC2 >> 6;
    a1 = (int32_t)cop2.MAC3 >> 6;
  }
  set_RTM(object->m);

  cop2.VZ0 = v1;
  cop2.VXY0 = (a1 << 16) + a0;
  RTPS();
  a0 = object->p.x + cop2.IR3;
  a1 = object->p.y - cop2.IR1;
  a2 = object->p.z - cop2.IR2;
  sw(t9 + 0x00, a0);
  sw(t9 + 0x04, a1);
  sw(t9 + 0x08, a2);
  return;
}

// size: 0x00000188
void function_80052F38(void)
{
  uint32_t temp;
  t8 = a0;
  t9 = a1;
  at = a2;
  v0 = a3;
  t7 = player_position;
  t6 = 0x80076378;
  t6 = lw(t6 + 0x0000);
  at = at << 2;
  at += t6;
  at = lw(at + 0x0038);
  v0 = v0 << 2;
  v0 += at;
  v0 = lw(v0 + 0x0024);
  a3 = v0 >> 21;
  a3 = a3 & 0x1;
  v0 = v0 << 11;
  v0 = v0 >> 10;
  a0 = lw(at + 0x0010);
  a1 = lw(v0 + 0x0008);
  a2 = v0 + 24; // 0x0018
  a1 = a1 >> 20;
  a1 += a2;
  at = 0;
  v0 = 0;
  v1 = 0;
  t6 = 0x8006D614; // &0xFE9FD3FA
label80052FB0:
  temp = (int32_t)a3 > 0;
  t8--;
  if (temp) goto label80053010;
  t0 = lbu(a1 + 0x0000);
  t1 = lw(a0 + 0x0000);
  a1++;
  a0 += 4; // 0x0004
  a3 = t0 & 0x1;
  t0 = t0 << 1;
  t0 = t0 & 0x1FC;
  t0 += t6;
  t0 = lw(t0 + 0x0000);
  t0 += t1;
  t1 = (int32_t)t0 >> 21;
  at += t1;
  t1 = t0 << 11;
  t1 = (int32_t)t1 >> 21;
  v0 += t1;
  t1 = t0 << 22;
  t1 = (int32_t)t1 >> 21;
  temp = t8 != 0;
  v1 += t1;
  if (temp) goto label80052FB0;
  goto label800530B0;
label80053010:
  t0 = lh(a2 + 0x0000);
  a2 += 2; // 0x0002
  temp = (int32_t)t0 < 0;
  a3 = t0 & 0x4000;
  if (temp) goto label80053078;
  t1 = lw(a0 + 0x0000);
  a0 += 4; // 0x0004
  t2 = t0 << 18;
  t2 = (int32_t)t2 >> 25;
  at += t2;
  t2 = t0 << 23;
  t2 = (int32_t)t2 >> 25;
  v0 += t2;
  t2 = t0 << 28;
  t2 = (int32_t)t2 >> 25;
  v1 += t2;
  t0 = (int32_t)t1 >> 21;
  at += t0;
  t0 = t1 << 11;
  t0 = (int32_t)t0 >> 21;
  v0 += t0;
  t0 = t1 << 22;
  t0 = (int32_t)t0 >> 21;
  temp = t8 != 0;
  v1 += t0;
  if (temp) goto label80052FB0;
  goto label800530B0;
label80053078:
  t1 = lhu(a2 + 0x0000);
  a2 += 2; // 0x0002
  a0 += 4; // 0x0004
  t2 = t0 << 18;
  at = (int32_t)t2 >> 21;
  t2 = t0 << 28;
  t0 = t1 << 12;
  t0 = t0 | t2;
  v0 = (int32_t)t0 >> 21;
  t2 = t1 << 22;
  temp = t8 != 0;
  v1 = (int32_t)t2 >> 21;
  if (temp) goto label80052FB0;
  goto label800530B0;
label800530B0:
  sw(t9 + 0x0000, at);
  sw(t9 + 0x0004, v0);
  sw(t9 + 0x0008, v1);
  return;
}

// size: 0x000001D4
void function_80051FEC(void)
{
  memset(addr_to_pointer(0x80077868), 0, 32);
  vec3_32 pos = *(vec3_32*)addr_to_pointer(player_position);
  t6 = 0x8006FCF4 + 0x400;
  for (struct game_object *object = addr_to_pointer(lw(0x80075828)); object->unknown48 != -1; object++) {
    if ((int32_t)object->unknown48 < 0) continue;
    if (object->unknown51) goto label8005210C;
    v1 = object->unknown52;
    if (v1 == 0) continue;
    if ((int32_t)v1 < 0) goto label8005210C;
    v1 = v1 << 10;
    vec3_32 p = vec3_32_sub(object->p, pos);
    if (abs_int(p.x) >= (int32_t)v1
     || abs_int(p.y) >= (int32_t)v1
     || abs_int(p.z) >= (int32_t)v1) continue;
    if ((int)(v1/8*v1/8) <= p.x/8*p.x/8 + p.y/8*p.y/8) continue;
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

  vec3_32 pos = *(vec3_32*)addr_to_pointer(player_position);

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
  for (uint32_t i = 0; i < fp; i++) {
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
struct game_object *create_3d_text2(const char *str, vec3_32 *pos, uint32_t spacing, uint32_t unknown_metadata)
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
struct game_object *create_3d_text1(const char *str, vec3_32 *pos, vec3_32 size, uint32_t unknown_spacing, uint32_t unknown_metadata)
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
  DEPRECATED;
  v0 = pointer_to_addr(create_3d_text1(addr_to_pointer(a0), addr_to_pointer(a1), *(vec3_32 *)addr_to_pointer(a2), a3, lw(sp + 0x10)));
}

// size: 0x00000068
void function_80012CF0(void)
{
  sw(0x800761D4, a0);
  sw(0x800761D0, a0 + 0x104);
  if (a1 == 0) return;
  int32_t num = lw(a0 + 0x100);
  for (int32_t i = 0; i < num; i++) {
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
  sw(0x800785A8 + 0x18, struct1_elements);
  sw(0x800785A8 + 0x1C, struct1_elements + struct1_len*16);
  sw(0x800785A8 + 0x20, struct1_len);

  struct_base = struct_base + lw(struct_base);

  const uint32_t struct2 = struct_base + 0x04;
  const int32_t struct2_len = lw(struct2);
  const uint32_t struct2_elements = struct2 + 0x04;
  sw(0x800785A8 + 0x00, struct2_elements);
  sw(0x800785A8 + 0x04, struct2_len);
  
  for (int32_t i = 0; i < struct2_len; i++)
    sw(struct2_elements + i*4, struct2 + lw(struct2_elements + i*4));

  if (a1 == 0) {
    struct_base = struct_base + lw(struct_base);

    if (lw(struct_base) > 4) {
      const uint32_t struct3 = struct_base + 0x04;

      const uint32_t struct3_1 = struct3 + 0x04;
      const int32_t struct3_1_len = lw(struct3_1);
      const uint32_t struct3_1_elements = struct3_1 + 0x04;
      sw(0x800785A8 + 0x08, struct3_1_elements);
      sw(0x800785A8 + 0x0C, struct3_1_len);

      for (int32_t i = 0; i < struct3_1_len; i++)
        sw(struct3_1_elements + i*4, struct3 + lw(struct3_1_elements + i*4));

      const uint32_t struct3_2 = struct_base + lw(struct3) + 0x04;
      const int32_t struct3_2_len = lw(struct3_2);
      const uint32_t struct3_2_elements = struct3_2 + 0x04;

      sw(0x80077788, struct3_2_len);
      sw(0x8007778C, struct3_2_elements);

      for (int32_t i = 0; i < struct3_2_len; i++)
        sw(struct3_2_elements + i*4, struct3_2 + lw(struct3_2_elements + i*4));

    } else {
      sw(0x800785A8 + 0x08, 0);
      sw(0x800785A8 + 0x0C, 0);
      sw(0x8007778C, 0);
    }

    struct_base = struct_base + lw(struct_base);

    const uint32_t struct4 = struct_base + 0x04;
    const int32_t struct4_len = lw(struct4);
    const uint32_t struct4_elements = struct4 + 0x04;
    sw(0x800785A8 + 0x10, struct4_elements);
    sw(0x800785A8 + 0x14, struct4_len);
    for (int32_t i = 0; i < struct4_len; i++)
      sw(struct4_elements + i*4, lw(struct4_elements + i*4) + struct4);
    
    struct_base = struct_base + lw(struct_base);

    const uint32_t struct5 = struct_base + 0x04;
    const uint32_t struct5_elements = struct5 + 0x08;

    sw(0x800785A8 + 0x2C, struct5);

    for (int i = 0; i < 5; i++)
      sw(struct5_elements + i*4, lw(struct5_elements + i*4) + struct5);
  }

  struct_base = struct_base + lw(struct_base);

  const uint32_t struct6 = struct_base + 4;
  const int32_t struct6_len = lw(struct6 + 4);
  const uint32_t struct6_elements = struct6 + 8;

  sb(0x80077790, lbu(struct6 + 0));
  sb(0x80077791, lbu(struct6 + 1));
  sb(0x80077792, lbu(struct6 + 2));
  sw(0x80077780, struct6_len);
  sw(0x80077784, struct6_elements);

  for (int32_t i = 0; i < struct6_len; i++)
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
  for (int32_t i = 0; i < struct7_len; i++) {
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
    for (uint32_t i = 0; i < element_struct2_len; i++)
      sw(element_struct2_elements + i*4, lw(element_struct2_elements + i*4) + element_struct2);

    element = struct_base + lw(struct_base);
  }
  
  struct_base = element;

  const uint32_t struct8 = struct_base + 0x04;
  const uint32_t struct8_len = lw(struct8);
  const uint32_t struct8_elements = struct8 + 0x04;

  element = struct8_elements;
  for (uint32_t i = 0; i < struct8_len; i++) {
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

  game_object->unknown3C = 0;
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
  DEPRECATED;
  new_game_object(addr_to_pointer(a0));
}

// size: 0x0000015C
void function_80056B28(void)
{
  a0 = a0 ^ 0x00FFFFFF;
  for (int i = 0; i < 24; i++) {
    if (lhu(0x80075F30 + i*0x1C + 0x0E) & 0x83) {
      if ((a0 >> i) & 1) {
        v1 = 1 << i;
        sw(0x8007623C, lw(0x8007623C) | v1);
        v1 = lhu(0x80075F30 + i*0x1C + 0x0E) & 0x1C;
        if (v1 != 8) {
          if (v1 == 4) {
            v0 = lw(0x80075F30 + i*0x1C + 0x00);
            if (v0) sb(v0 + 0x2A0, 0x7F);
          }
        } else {
          v0 = lw(0x80075F30 + i*0x1C + 0x00);
          if (v0) sb(v0 + 0x54, 0x7F);
        }
        v0 = i*0x1C;
        sh(0x80075F30 + i*0x1C + 0x0E, 0x40);
        sw(0x80075F30 + i*0x1C + 0x00, 0);
        sw(0x80075F30 + i*0x1C + 0x18, 0);
        sb(0x80075F30 + i*0x1C + 0x0D, 0xFF);
      }
    }
  }
  sw(0x80076238, 0);
  a0 = 0;
  a1 = 4;
  function_800567F4();
}


// size: 0x00000788
void function_80055A78(void)
{
  uint32_t temp;
  sp -= 144; // 0xFFFFFF70
  sw(sp + 0x0078, s4);
  s4 = a0;
  sw(sp + 0x006C, s1);
  s1 = a1;
  sw(sp + 0x0074, s3);
  s3 = a2;
  sw(sp + 0x0084, s7);
  s7 = a3;
  sw(sp + 0x0070, s2);
  s2 = -1; // 0xFFFFFFFF
  sw(sp + 0x007C, s5);
  s5 = 0x80076E20;
  v0 = 8; // 0x0008
  sw(sp + 0x008C, ra);
  sw(sp + 0x0088, fp);
  sw(sp + 0x0080, s6);
  temp = s3 == v0;
  sw(sp + 0x0068, s0);
  if (temp) goto label80055AFC;
  v0 = s3 < 9;
  temp = v0 == 0;
  v0 = 4; // 0x0004
  if (temp) goto label80055B7C;
  temp = s3 != v0;
  a1 = 256; // 0x0100
  if (temp) goto label80055B80;
  a0 = s1;
  a1 = s4;
  function_80056DC4();
  v1 = 2; // 0x0002
  temp = v0 != v1;
  a1 = 256; // 0x0100
  if (temp) goto label80055B80;
  v0 = 0;
  goto label800561CC;
label80055AFC:
  a0 = s1;
  a1 = s4;
  function_80056DC4();
  v1 = 2; // 0x0002
  temp = v0 == v1;
  a1 = s1 + 12; // 0x000C
  if (temp) goto label800561C8;
  s0 = sp + 88; // 0x0058
  a0 = s0;
  a2 = s5 - 40; // 0xFFFFFFD8
  spyro_vec3_sub(a0, a1, a2);
  a0 = s0;
  a1 = 4; // 0x0004
  spyro_vec3_shift_right(a0, a1);
  a0 = s0;
  a1 = 1; // 0x0001
  v0 = spyro_vec_length(a0, a1);
  v1 = lbu(s1 + 0x0055);
  s6 = v1 << 10;
  temp = s6 != 0;
  fp = v0 << 4;
  if (temp) goto label80055B54;
  s6 = 0x4000; // 0x4000
label80055B54:
  v0 = (int32_t)s6 < (int32_t)fp;
  temp = v0 == 0;
  if (temp) goto label80055B7C;
  temp = s7 == 0;
  v0 = 127; // 0x007F
  if (temp) goto label80055B6C;
  sb(s7 + 0x0000, v0);
label80055B6C:
  v0 = 127; // 0x007F
  goto label800561CC;
label80055B74:
  s2 = s0;
  goto label80055BE8;
label80055B7C:
  a1 = 256; // 0x0100
label80055B80:
  s0 = 0;
  a0 = 0;
label80055B88:
  v0 = lhu(0x80075F30 + 0x000E + a0);
  v0 = v0 & 0xC1;
  temp = v0 == 0;
  v0 = a1 << 3;
  if (temp) goto label80055B74;
  v0 -= a1;
  v0 = v0 << 2;
  v1 = lbu(0x80075F30 + 0x000C + a0);
  v0 = lbu(0x80075F30 + 0x000C + v0);
  v1 = v1 < v0;
  temp = v1 == 0;
  if (temp) goto label80055BD8;
  a1 = s0;
label80055BD8:
  s0++;
  v0 = (int32_t)s0 < 24;
  temp = v0 != 0;
  a0 += 28; // 0x001C
  if (temp) goto label80055B88;
label80055BE8:
  temp = (int32_t)s2 < 0;
  v0 = 24; // 0x0018
  if (temp) goto label800561C8;
  temp = s2 == v0;
  v0 = s2 << 3;
  if (temp) goto label80056160;
  v0 -= s2;
  a0 = v0 << 2;
  v0 = s3 | 0x1;
  a2 = 0x800761DC;
  sh(0x80075F30 + 0x000E + a0, v0);
  sw(0x80075F30 + a0, s1);
  sb(0x80075F30 + 0x000D + a0, s4);
  a1 = lw(a2 + 0x0000);
  v0 = a1 & 0x2;
  temp = v0 == 0;
  v0 = -3; // 0xFFFFFFFD
  if (temp) goto label80055C68;
  v1 = lhu(0x800761F4);
  v0 = a1 & v0;
  sw(a2 + 0x0000, v0);
  sw(0x80075F30 + 0x0008 + a0, v1);
  v0 = 0x00010000;
  goto label80055C94;
label80055C68:
  v0 = s4 << 2;
  v0 += s4;
  v1 = lw(0x800761D0);
  v0 = v0 << 2;
  v0 += v1;
  v0 = lhu(v0 + 0x000A);
  sw(0x80075F30 + 0x0008 + a0, v0);
  v0 = 0x00010000;
label80055C94:
  v0 = v0 | 0x93;
  a3 = 1; // 0x0001
  sw(sp + 0x001C, v0);
  v0 = a3 << s2;
  sw(sp + 0x0018, v0);
  v0 = s4 << 2;
  v0 += s4;
  v1 = lw(0x800761D0);
  s1 = v0 << 2;
  a0 = s1 + v1;
  v0 = lw(a0 + 0x0000);
  sw(sp + 0x0034, v0);
  a2 = lhu(a0 + 0x000A);
  sh(sp + 0x002C, a2);
  a1 = lhu(a0 + 0x000C);
  temp = a1 == 0;
  if (temp) goto label80055E74;
  v1 = lw(a0 + 0x0010);
  temp = v1 == a3;
  if (temp) goto label80055DA4;
  v0 = (int32_t)v1 < 2;
  temp = v0 == 0;
  if (temp) goto label80055D14;
  temp = v1 == 0;
  v0 = a1 >> 1;
  if (temp) goto label80055D28;
  v0 = s4 << 2;
  goto label80055E78;
label80055D14:
  v0 = 2; // 0x0002
  temp = v1 == v0;
  v0 = s4 << 2;
  if (temp) goto label80055E0C;
  goto label80055E78;
label80055D28:
  v1 = lhu(a0 + 0x000E);
  mult(v1, v0);
  t0=lo;
  v0 = a2 - t0;
  sh(sp + 0x002C, v0);
  v0 = spyro_rand();
  a0 = lw(0x800761D0);
  a0 += s1;
  v1 = lhu(a0 + 0x000C);
  div_psx(v0,v1);
  temp = v1 != 0;
  if (temp) goto label80055D6C;
  UNREACHABLE; // BREAK 0x01C00
label80055D6C:
  at = -1; // 0xFFFFFFFF
  temp = v1 != at;
  at = 0x80000000;
  if (temp) goto label80055D84;
  temp = v0 != at;
  if (temp) goto label80055D84;
  UNREACHABLE; // BREAK 0x01800
label80055D84:
  v1=hi;
  v0 = lhu(a0 + 0x000E);
  mult(v1, v0);
  v0 = lhu(sp + 0x002C);
  t0=lo;
  v0 += t0;
  goto label80055E70;
label80055DA4:
  v0 = spyro_rand();
  a0 = lw(0x800761D0);
  a0 += s1;
  v1 = lhu(a0 + 0x000C);
  div_psx(v0,v1);
  temp = v1 != 0;
  if (temp) goto label80055DD4;
  UNREACHABLE; // BREAK 0x01C00
label80055DD4:
  at = -1; // 0xFFFFFFFF
  temp = v1 != at;
  at = 0x80000000;
  if (temp) goto label80055DEC;
  temp = v0 != at;
  if (temp) goto label80055DEC;
  UNREACHABLE; // BREAK 0x01800
label80055DEC:
  v1=hi;
  v0 = lhu(a0 + 0x000E);
  mult(v1, v0);
  v0 = lhu(sp + 0x002C);
  t0=lo;
  v0 += t0;
  goto label80055E70;
label80055E0C:
  v0 = spyro_rand();
  a0 = lw(0x800761D0);
  a0 += s1;
  v1 = lhu(a0 + 0x000C);
  div_psx(v0,v1);
  temp = v1 != 0;
  if (temp) goto label80055E3C;
  UNREACHABLE; // BREAK 0x01C00
label80055E3C:
  at = -1; // 0xFFFFFFFF
  temp = v1 != at;
  at = 0x80000000;
  if (temp) goto label80055E54;
  temp = v0 != at;
  if (temp) goto label80055E54;
  UNREACHABLE; // BREAK 0x01800
label80055E54:
  v1=hi;
  v0 = lhu(a0 + 0x000E);
  mult(v1, v0);
  v0 = lhu(sp + 0x002C);
  t0=lo;
  v0 -= t0;
label80055E70:
  sh(sp + 0x002C, v0);
label80055E74:
  v0 = s4 << 2;
label80055E78:
  v0 += s4;
  v1 = lw(0x800761D0);
  v0 = v0 << 2;
  v0 += v1;
  v0 = lw(v0 + 0x0004);
  sw(sp + 0x0038, v0);
  v0 = s0 << 3;
  v0 -= s0;
  v0 = v0 << 2;
  v0 = lhu(0x80075F30 + 0x000E + v0);
  v1 = v0 & 0x1C;
  v0 = 8; // 0x0008
  temp = v1 == v0;
  v0 = (int32_t)v1 < 9;
  if (temp) goto label80055FE8;
  temp = v0 == 0;
  v0 = 4; // 0x0004
  if (temp) goto label80055EDC;
  temp = v1 == v0;
  if (temp) goto label80055EE8;
  goto label80056104;
label80055EDC:
  v0 = 16; // 0x0010
  temp = v1 != v0;
  if (temp) goto label80056104;
label80055EE8:
  a0 = 0x800761DC;
  v1 = lw(a0 + 0x0000);
  v0 = v1 & 0x1;
  temp = v0 == 0;
  v0 = -2; // 0xFFFFFFFE
  if (temp) goto label80055F8C;
  v0 = v1 & v0;
  sw(a0 + 0x0000, v0);
  v0 = s2 << 3;
  v0 -= s2;
  v1 = lhu(0x800761E8);
  v0 = v0 << 2;
  sh(0x80075F30 + 0x0010 + v0, v1);
  v1 = lhu(0x800761E8);
  sh(0x80075F30 + 0x0012 + v0, v1);
  v0 = lhu(0x800761E8);
  v1 = lhu(0x800761EA);
  sh(sp + 0x0020, v0);
  v0 = s0 << 3;
  v0 -= s0;
  v0 = v0 << 2;
  sh(sp + 0x0022, v1);
  sh(0x80075F30 + 0x0006 + v0, v1);
  v1 = lhu(0x800761E8);
  sh(0x80075F30 + 0x0004 + v0, v1);
  goto label80056104;
label80055F8C:
  v0 = s2 << 3;
  v0 -= s2;
  v0 = v0 << 2;
  v1 = 8192; // 0x2000
  sh(0x80075F30 + 0x0010 + v0, v1);
  sh(0x80075F30 + 0x0012 + v0, v1);
  v0 = s0 << 3;
  v0 -= s0;
  v0 = v0 << 2;
  sh(sp + 0x0022, v1);
  sh(sp + 0x0020, v1);
  sh(0x80075F30 + 0x0006 + v0, v1);
  sh(0x80075F30 + 0x0004 + v0, v1);
  goto label80056104;
label80055FE8:
  a0 = 0x800761DC;
  v1 = lw(a0 + 0x0000);
  v0 = v1 & 0x1;
  temp = v0 == 0;
  v0 = -2; // 0xFFFFFFFE
  if (temp) goto label80056058;
  v0 = v1 & v0;
  sw(a0 + 0x0000, v0);
  v0 = s2 << 3;
  v0 -= s2;
  v1 = lhu(0x800761E8);
  v0 = v0 << 2;
  sh(0x80075F30 + 0x0010 + v0, v1);
  v1 = lhu(0x800761EA);
  sh(0x80075F30 + 0x0012 + v0, v1);
  v0 = lhu(0x800761EA);
  v1 = lhu(0x800761E8);
  sh(sp + 0x0022, v0);
  goto label80056084;
label80056058:
  v0 = s2 << 3;
  v0 -= s2;
  v0 = v0 << 2;
  v1 = 15564; // 0x3CCC
  sh(0x80075F30 + 0x0010 + v0, v1);
  sh(0x80075F30 + 0x0012 + v0, v1);
  sh(sp + 0x0022, v1);
label80056084:
  sh(sp + 0x0020, v1);
  a2 = 0;
  s0 = s2 << 3;
  s0 -= s2;
  s0 = s0 << 2;
  v0 = lw(0x80075F30 + s0);
  a3 = lw(0x80076DF8);
  v1 = lw(0x80076DFC);
  a0 = lw(v0 + 0x000C);
  a1 = lw(v0 + 0x0010);
  a0 = a3 - a0;
  a1 = v1 - a1;
  v0 = spyro_atan2(a0, a1, a2);
  a0 = lhu(0x80076E20);
  a1 = v0 & 0xFF;
  a0 = a0 << 16;
  a0 = (int32_t)a0 >> 20;
  v0 = spyro_two_angle_diff_8bit(a0, a1);
  a0 = sp + 32; // 0x0020
  a1 = fp;
  a2 = v0 & 0xFF;
  v0 = 0x80075F30 + 0x0010;
  s0 += v0;
  a3 = s6;
  sw(sp + 0x0010, s0);
  function_80056C84();
label80056104:
  v0 = lh(sp + 0x0020);
  v1 = lw(0x8007622C);
  mult(v0, v1);
  t1=lo;
  v0 = lh(sp + 0x0022);
  mult(v0, v1);
  a0 = sp + 24; // 0x0018
  v0 = (int32_t)t1 >> 12;
  sh(sp + 0x0020, v0);
  v1=lo;
  v0 = (int32_t)v1 >> 12;
  sh(sp + 0x0022, v0);
  function_8005C7AC();
  v0 = 1; // 0x0001
  v1 = lw(0x80076238);
  v0 = v0 << s2;
  v1 = v1 | v0;
  sw(0x80076238, v1);
label80056160:
  v0 = s2 << 3;
  v0 -= s2;
  a0 = v0 << 2;
  sw(0x80075F30 + 0x0018 + a0, s7);
  temp = s7 == 0;
  v0 = s4 << 2;
  if (temp) goto label800561C8;
  v0 += s4;
  sb(s7 + 0x0000, s2);
  v1 = lw(0x800761D0);
  v0 = v0 << 2;
  v0 += v1;
  v0 = lw(v0 + 0x0004);
  temp = v0 == 0;
  v0 = 0;
  if (temp) goto label800561CC;
  v0 = lhu(0x80075F30 + 0x000E + a0);
  v0 = v0 | 0x100;
  sh(0x80075F30 + 0x000E + a0, v0);
label800561C8:
  v0 = 0;
label800561CC:
  ra = lw(sp + 0x008C);
  fp = lw(sp + 0x0088);
  s7 = lw(sp + 0x0084);
  s6 = lw(sp + 0x0080);
  s5 = lw(sp + 0x007C);
  s4 = lw(sp + 0x0078);
  s3 = lw(sp + 0x0074);
  s2 = lw(sp + 0x0070);
  s1 = lw(sp + 0x006C);
  s0 = lw(sp + 0x0068);
  sp += 144; // 0x0090
  return;
}

// size: 0x000000A4
void function_80056200(void)
{
  uint32_t temp;
  a2 = a0 & 0x7F;
  v0 = a2 < 24;
  temp = v0 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label8005629C;
  a0 = 0x8007623C;
  v1 = lw(a0 + 0x0000);
  v0 = v0 << a2;
  v1 = v1 | v0;
  v0 = 2; // 0x0002
  temp = a1 == v0;
  sw(a0 + 0x0000, v1);
  if (temp) goto label8005623C;
  v0 = 4; // 0x0004
  temp = a1 != v0;
  if (temp) goto label8005629C;
label8005623C:
  v0 = a2 << 3;
  v0 -= a2;
  v1 = v0 << 2;
  a0 = lw(0x80075F30 + 0x0018 + v1);
  temp = a0 == 0;
  v0 = 127; // 0x007F
  if (temp) goto label80056264;
  sb(a0 + 0x0000, v0);
label80056264:
  v0 = 64; // 0x0040
  sh(0x80075F30 + 0x000E + v1, v0);
  v0 = 255; // 0x00FF
  sw(0x80075F30 + 0x0018 + v1, 0);
  sw(0x80075F30 + v1, 0);
  sb(0x80075F30 + 0x000D + v1, v0);
label8005629C:
  return;
}

// size: 0x00000140
void function_80056C84(void)
{
  uint32_t temp;
  sp -= 32; // 0xFFFFFFE0
  t0 = a0;
  t2 = lw(sp + 0x0030);
  temp = a3 == 0;
  v1 = a2;
  if (temp) goto label80056DB8;
  v0 = a2 - 64; // 0xFFFFFFC0
  v0 = v0 & 0xFF;
  v0 = v0 < 128;
  temp = v0 == 0;
  v0 = -a2;
  if (temp) goto label80056CB8;
  t1 = v0 & 0xFF;
  a2 = v1 & 0xFF;
  goto label80056CCC;
label80056CB8:
  v0 = a2 + 128; // 0x0080
  t1 = v0 & 0xFF;
  v0 = 128; // 0x0080
  v0 -= a2;
  a2 = v0 & 0xFF;
label80056CCC:
  a0 = a1 << 8;
  divu_psx(a0, a3);
  temp = a3 != 0;
  if (temp) goto label80056CE0;
  UNREACHABLE; // BREAK 0x01C00
label80056CE0:
  a0=lo;
  v0 = lh(t2 + 0x0000);
  mult(a0, v0);
  t3=lo;
  v1 = t3 >> 8;
  v0 -= v1;
  v0 = v0 << 16;
  v0 = (int32_t)v0 >> 16;
  mult(t1, v0);
  t3=lo;
  v0 = (int32_t)t3 >> 7;
  sh(t0 + 0x0000, v0);
  v0 = lh(t2 + 0x0002);
  mult(a0, v0);
  t3=lo;
  v1 = t3 >> 8;
  v0 -= v1;
  v0 = v0 << 16;
  v0 = (int32_t)v0 >> 16;
  mult(a2, v0);
  v1 = lh(t0 + 0x0000);
  t3=lo;
  v0 = (int32_t)t3 >> 7;
  temp = (int32_t)v1 >= 0;
  sh(t0 + 0x0002, v0);
  if (temp) goto label80056D54;
  sh(t0 + 0x0000, 0);
  goto label80056D64;
label80056D54:
  v0 = (int32_t)v1 < 0x4000;
  temp = v0 != 0;
  v0 = 16383; // 0x3FFF
  if (temp) goto label80056D64;
  sh(t0 + 0x0000, v0);
label80056D64:
  v0 = lh(t0 + 0x0002);
  temp = (int32_t)v0 >= 0;
  v0 = (int32_t)v0 < 0x4000;
  if (temp) goto label80056D7C;
  sh(t0 + 0x0002, 0);
  goto label80056D88;
label80056D7C:
  temp = v0 != 0;
  v0 = 16383; // 0x3FFF
  if (temp) goto label80056D88;
  sh(t0 + 0x0002, v0);
label80056D88:
  v0 = lw(0x80076240);
  temp = v0 == 0;
  if (temp) goto label80056DB8;
  v0 = lh(t0 + 0x0002);
  v1 = lh(t0 + 0x0000);
  v0 += v1;
  v0 = v0 >> 1;
  sh(t0 + 0x0000, v0);
  sh(t0 + 0x0002, v0);
label80056DB8:
  sp += 32; // 0x0020
  return;
}

void function_8005D9EC(void);

// size: 0x00000098
void function_80056E3C(void)
{
  uint32_t temp;
  sp -= 48; // 0xFFFFFFD0
  sw(sp + 0x0028, ra);
  a0 = sp + 16; // 0x0010
  function_8005D9EC();
  a1 = 0;
  t1 = 1; // 0x0001
  t0 = 64; // 0x0040
  a3 = 0x8007623C;
  a0 = 0;
  a2 = sp + 16; // 0x0010
label80056E68:
  v1 = lbu(a2 + 0x0000);
  temp = v1 != t1;
  if (temp) goto label80056EB0;
  v0 = lhu(0x80075F30 + 0x0E + a0) & 0xC1;
  temp = v0 != 0;
  v1 = v1 << a1;
  if (temp) goto label80056EB0;
  sh(0x80075F30 + 0x0E + a0, t0);
  sw(a3, lw(a3) | v1);
label80056EB0:
  a0 += 28; // 0x001C
  a1++;
  v0 = (int32_t)a1 < 24;
  temp = v0 != 0;
  a2++;
  if (temp) goto label80056E68;
  ra = lw(sp + 0x0028);
  sp += 48; // 0x0030
  return;
}

// size: 0x000000A8
void function_8002C714(void)
{
  sp -= 24;
  sw(sp + 0x10, s0);
  sw(sp + 0x14, ra);
  s0 = a0;
  if (s0) {
    a0 = 0;
    function_80056B28();
  }
  a0 = lbu(lw(0x800761D4) + 0x2C);
  a1 = 0;
  a2 = 16; // 0x0010
  a3 = 0;
  function_80055A78();
  v1 = lw(CONTINUOUS_LEVEL_ID);
  mult(v1, 0x2AAAAAAB);
  sw(0x800757CC, 0);
  sw(0x800756D4, 0);
  v1 = (int32_t)v1 >> 31;
  sw(gamestate, 3);
  t0=hi;
  v1 = t0 - v1;
  sw(0x80075744, v1);
  if (s0)
    sw(0x800758B8, 0);
    
  function_8002D554();
  ra = lw(sp + 0x14);
  s0 = lw(sp + 0x10);
  sp += 24;
  return;
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
  DEPRECATED;
  v0 = completion_percentage();
}

// size: 0x000003F4
void function_8003385C(void)
{
  sw(0x8007579C, 0);
  function_8005637C();
  function_8002BBE0();
  switch (lw(gamestate)) {
  case GAMESTATE_INGAME: // in game
    sw(level_frame_counter, lw(level_frame_counter)+1);
    if (lw(IS_DEMO_MODE)) {
      function_800334D4();
      if (v0) return;
    }
    a0 = lw(0x800756CC);
    function_8002A6FC();
    function_loaded_80075734();
    v1 = lw(gamestate);
    if (v1 == 1 || v1 == 7 || v1 == 11 || v1 == 12) return;
    function_8004A200();
    v1 = lw(gamestate);
    if (v1 == 1 || v1 == 4 || v1 == 5 || v1 == 7) return;
    a0 = lw(0x800756CC);
    function_loaded_800756BC();
    
    if (lw(0x80075690) == 0)
      function_80054988();
    
    function_80058BD8();
    function_80037BD4();

    if (lw(0x80075918))
      sw(0x80075918, max_int(lw(0x80075918) - lw(0x800756CC), 0));
    
    if (lw(gamestate)) break;
    if (lw(0x8007570C)) break;
    if ((int32_t)lw(0x80078BBC) < 0) break;
    if (lw(0x80076E28) == 0x8000000E) break;

    if (lw(buttons + 0x0C) < 2 || lw(buttons) & 0x800) {
      a0 = 1;
      function_8002C420();
    } else if (lw(buttons) & 0x100) {
      a0 = 1;
      function_8002C714();
    }
    break;
  case GAMESTATE_LOADING: // loading level
    function_8002DF9C();
    break;
  case GAMESTATE_PAUSE: // pause menu
    function_8002E12C();
    break;
  case GAMESTATE_INVENTORY: // inventory
    function_8002EB2C();
    break;
  case GAMESTATE_DEAD: // dead
  case GAMESTATE_GAMEOVER:
    function_8002EDF0();
    break;
  case GAMESTATE_UNKNOWN:
    // function_8002F3C4(); // this does nothing
    UNREACHABLE;
    break;
  case GAMESTATE_FLIGHT_LEVEL_MENU: // flight level crashed
    if(lw(0x800757C0) != 0x8007B1FC) BREAKPOINT;
    function_8007B1FC();
    break;
  case GAMESTATE_DRAGON_FREED: // freed dragon
    function_8002F3E4();
    break;
  case GAMESTATE_LOADING_TRANSITION: // landing after loading
    function_8002E000();
    break;
  case GAMESTATE_PREPARE_LOADING: // before loading ?
    function_8002E084();
    break;
  case GAMESTATE_FAIRY: // fairy menu
    function_800314B4();
    break;
  case GAMESTATE_BALLOONIST: // balloonist
    function_800324D8();
    break;
  case GAMESTATE_TITLE_SCREEN: // title screen / game intro
    if (lw(0x80078D78) != 3)
      function_8007ABAC_title_screen();
    else
      function_80032B08();
    break;
  case GAMESTATE_CUTSCENE: // credits
    function_800331AC();
    break;
  case GAMESTATE_CREDITS: // more credits
    if ((int32_t)lw(0x80075704) >= 99)
      function_800333DC();
    else
      function_8007AA50_credits();
    break;
  }
  function_80056ED4();
}

// size: 0x00000178
// guessing: create a new game save
void function_80012604(void)
{
  for (int i = 0; i < 36; i++) {
    sw(level_eggs + i*4, 0);
    sw(level_dragons + i*4, 0);
    if ((int32_t)i < 18)
      sw(level_gems + i*4, 0);
    
    sb(0x80078E78 + i, 0);
    sb(0x8007A6A8 + i, 0);
  }

  for (int i = 0; i < 6; i++)
    sb(0x800758D5 + i, 0);

  sb(0x800758D0, 2);
  for (int i = 0; i < 6; i++) {
    sw(0x80078618+i*4, 0);
    for (int j = 0; j < 5; j++)
      sb(0x80078680+i*5+j, 0);
  }

  sw(0x80078BBC, 3);
  sw(lives, 4);
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

// size: 0x00001360
void function_8003EA68(void)
{
  uint32_t temp;
  v0 = lw(player_movestate);
  sp -= 80;
  sw(sp + 0x48, ra);
  sw(sp + 0x44, s5);
  sw(sp + 0x40, s4);
  sw(sp + 0x3C, s3);
  sw(sp + 0x38, s2);
  sw(sp + 0x34, s1);
  sw(sp + 0x30, s0);
  s5 = a0;
  switch (v0)
  {
  case 11:case 15:case 32:case 44:
    a0 = player_position;
    a1 = 2;
    function_800562A4();
    break;
  case 7:
    sb(0x80078A83, 0);
#ifdef __GNUC__
    [[fallthrough]];
#endif
  case 14:case 22:case 27:case 28:
    a0 = lbu(lw(0x800761D4) + 0x1B);
    a1 = player_position;
    v0 = lw(0x800761D4);
    a2 = 4;
    a3 = a1 + 0x2A0;
    function_80055A78();
    break;
  case 25:
    a0 = lbu(lw(0x800761D4) + 0x20);
    a1 = player_position;
    a2 = 4;
    a3 = a1 + 0x2A0;
    function_80055A78();
    break;
  default:
    break;
  }
  v0 = s5 < 45;
  temp = v0 == 0;
  v0 = s5 << 2;
  if (temp) goto label8003FD50;
  v0 = lw(0x80010F20 + v0); // &0x8003EB40
  temp = v0;
  switch (temp)
  {
  case 0x8003EB40: // 0
    goto label8003EB40;
    break;
  case 0x8003EBA0: // 1 2 3 21
    goto label8003EBA0;
    break;
  case 0x8003EDF4: // 4
    goto label8003EDF4;
    break;
  case 0x8003EE10: // 5
    goto label8003EE10;
    break;
  case 0x8003EE9C: // 6
    goto label8003EE9C;
    break;
  case 0x8003F308: // 7
    goto label8003F308;
    break;
  case 0x8003FC6C: // 8
    goto label8003FC6C;
    break;
  case 0x8003EF50: // 9 10
    goto label8003EF50;
    break;
  case 0x8003EF84: // 11
    goto label8003EF84;
    break;
  case 0x8003F1D4: // 12
    goto label8003F1D4;
    break;
  case 0x8003F300: // 13
    goto label8003F300;
    break;
  case 0x8003F3C8: // 14
    goto label8003F3C8;
    break;
  case 0x8003F570: // 15 23 32 33 34
    goto label8003F570;
    break;
  case 0x8003EEF0: // 16
    goto label8003EEF0;
    break;
  case 0x8003F6BC: // 17
    goto label8003F6BC;
    break;
  case 0x8003F784: // 18 36 37 38 39 40 41 42 43
    goto label8003F784;
    break;
  case 0x8003F7BC: // 19
    goto label8003F7BC;
    break;
  case 0x8003F810: // 20
    goto label8003F810;
    break;
  case 0x8003F9E0: // 22 28
    goto label8003F9E0;
    break;
  case 0x8003F8EC: // 24
    goto label8003F8EC;
    break;
  case 0x8003F49C: // 25
    goto label8003F49C;
    break;
  case 0x8003F93C: // 26
    goto label8003F93C;
    break;
  case 0x8003F520: // 27
    goto label8003F520;
    break;
  case 0x8003FAB4: // 29
    goto label8003FAB4;
    break;
  case 0x8003FBA4: // 30
    goto label8003FBA4;
    break;
  case 0x8003FBF4: // 31
    goto label8003FBF4;
    break;
  case 0x8003FC60: // 35
    goto label8003FC60;
    break;
  case 0x8003FC88: // 44
    goto label8003FC88;
    break;
  default:
    JR(temp, 0x8003EB38);
    return;
  }
label8003EB40:
  sw(0x80078AD4, 0);
  spyro_rand();
  v0 = v0 & 0x7;
  v1 = lbu(0x80075274 + v0); // &0x08181008
  v0 = lbu(0x8006C4A2); // &0x14000216
  mult(v1, v0);
  v0 = -128; // 0xFFFFFF80
  sw(0x80078BA4, v0);
  v0 = 1; // 0x0001
  sw(0x80078BB4, v0);
  t0=lo;
  v0 = t0 - 2; // 0xFFFFFFFE
  sh(0x80075788, v0); // &0x00000000
  goto label8003FD50;
label8003EBA0:
  v0 = 0x80078AD4;
  a2 = 1; // 0x0001
  a0 = lw(v0 + 0x0090);
  a1 = lw(0x80078B68);
  s0 = v0 + 144; // 0x0090
  sw(v0 + 0x0000, 0);
  v0 = spyro_atan2(a0, a1, a2);
  v1 = lw(0x80078B74);
  v0 -= v1;
  s1 = v0 & 0xFFF;
  v0 = s1 - 1025; // 0xFFFFFBFF
  v0 = v0 < 2047;
  temp = v0 == 0;
  a0 = s0;
  if (temp) goto label8003EBF8;
  sw(0x80078B70, 0);
  v0 = 3; // 0x0003
  goto label8003EC0C;
label8003EBF8:
  a1 = 0;
  v0 = spyro_vec_length(a0, a1);
  sw(0x80078B70, v0);
  v0 = 3; // 0x0003
label8003EC0C:
  temp = s5 != v0;
  v0 = -3072; // 0xFFFFF400
  if (temp) goto label8003EC3C;
  v1 = 0x80078B70;
  v0 = lw(v1 + 0x0000);
  v0 = (int32_t)v0 < 5121;
  temp = v0 != 0;
  v0 = -3072; // 0xFFFFF400
  if (temp) goto label8003EC3C;
  v0 = 5120; // 0x1400
  sw(v1 + 0x0000, v0);
  v0 = -3072; // 0xFFFFF400
label8003EC3C:
  sw(0x80078BA4, v0);
  function_8003E1AC();
  v0 = lw(0x80078B08);
  temp = v0 == 0;
  if (temp) goto label8003EDA4;
  a0 = lw(0x80078AFC);
  a1 = lw(0x80078B00);
  a2 = 1; // 0x0001
  v0 = spyro_atan2(a0, a1, a2);
  v1 = lw(0x80078B74);
  v1 -= v0;
  s1 = v1 & 0xFFF;
  v0 = (int32_t)s1 < 2049;
  temp = v0 != 0;
  v1 = 0x2E8B0000;
  if (temp) goto label8003EC9C;
  s1 -= 4096; // 0xFFFFF000
label8003EC9C:
  v0 = lw(0x80078B08);
  v1 = v1 | 0xA2E9;
  v0 = v0 << 12;
  mult(v0, v1);
  temp = (int32_t)s1 >= 0;
  if (temp) goto label8003ECBC;
  s1 = -s1;
label8003ECBC:
  v0 = (int32_t)v0 >> 31;
  t0=hi;
  v1 = (int32_t)t0 >> 2;
  a0 = v1 - v0;
  v0 = (int32_t)s1 < 1025;
  temp = v0 != 0;
  if (temp) goto label8003ED00;
  v0 = lw(0x8006C5CC); // &0x000003C0
  v1 = lw(0x8006C5C0); // &0x00001200
  v0 -= v1;
  mult(a0, v0);
  v0 = 2048; // 0x0800
  s1 = v0 - s1;
  goto label8003ED1C;
label8003ED00:
  v0 = lw(0x8006C5C4); // &0x00001440
  v1 = lw(0x8006C5B8); // &0x00001200
  v0 -= v1;
  mult(a0, v0);
label8003ED1C:
  t0=lo;
  v0 = (int32_t)t0 >> 12;
  s2 = v1 + v0;
  v0 = lw(0x8006C5C8); // &0x00001200
  v1 = lw(0x8006C5BC); // &0x00001200
  v0 -= v1;
  mult(a0, v0);
  a0 = s1;
  t1=lo;
  v0 = (int32_t)t1 >> 12;
  s0 = v1 + v0;
  v0 = spyro_cos(a0);
  mult(s2, v0);
  a0 = s1;
  v1=lo;
  s2 = (int32_t)v1 >> 12;
  v0 = spyro_sin(a0);
  mult(s0, v0);
  v1=lo;
  mult(s2, s2);
  v0=lo;
  s0 = (int32_t)v1 >> 12;
  mult(s0, s0);
  v1=lo;
  a0 = v0 + v1;
  v0 = spyro_sqrt(a0);
  goto label8003EDAC;
label8003EDA4:
  v0 = lw(0x8006C5B8); // &0x00001200
label8003EDAC:
  sw(0x80078B9C, v0);
  v1 = lw(0x80078BEC);
  v0 = 1; // 0x0001
  sw(0x80078BB4, v0);
  temp = v1 == 0;
  if (temp) goto label8003FD50;
  v0 = lbu(0x80078C5E);
  sw(0x80078B20, 0);
  v0 = v0 << 4;
  sw(0x80078B24, v0);
  goto label8003FD50;
label8003EDF4:
  v0 = -3072; // 0xFFFFF400
  sw(0x80078AD4, 0);
  sw(0x80078BA4, v0);
  goto label8003FBD8;
label8003EE10:
  a0 = 0x80078B4C;
  a1 = a0 + 24; // 0x0018
  spyro_vec3_copy(a0, a1);
  v0 = 3520; // 0x0DC0
  sw(0x80078B48, v0);
  v0 = -192; // 0xFFFFFF40
  sw(0x80078BA4, v0);
  v0 = lw(0x80078B54);
  v1 = lw(player_position + 0x0008);
  sw(0x80078AD4, 0);
  sw(0x80078BAC, 0);
  sw(0x80078BB4, 0);
  v0 = (int32_t)v0 >> 6;
  v1 += v0;
  sw(0x80078BB0, v1);
  v1 = lw(player_movestate);
  v0 = 29; // 0x001D
  temp = v1 != v0;
  v0 = 1; // 0x0001
  if (temp) goto label8003EE90;
  sw(gp + 0x0450, v0);
  goto label8003FD50;
label8003EE90:
  sw(gp + 0x0450, 0);
  goto label8003FD50;
label8003EE9C:
  v0 = 0x80078AD4;
  a0 = v0 + 120; // 0x0078
  a1 = v0 + 144; // 0x0090
  sw(v0 + 0x0000, 0);
  spyro_vec3_copy(a0, a1);
  v0 = lw(0x80078AF0);
  temp = v0 != 0;
  v0 = -192; // 0xFFFFFF40
  if (temp) goto label8003F8D4;
  v0 = lw(0x80078B08);
  v0 = (int32_t)v0 < 32;
  temp = v0 == 0;
  v0 = -192; // 0xFFFFFF40
  if (temp) goto label8003F8D4;
  function_8003E90C();
  v0 = -192; // 0xFFFFFF40
  goto label8003F8D4;
label8003EEF0:
  v0 = lw(0x80078B6C);
  v1 = lw(0x80078AF0);
  sw(0x80078AD4, 0);
  sw(0x80078B4C, 0);
  sw(0x80078B50, 0);
  sw(0x80078B54, v0);
  temp = v1 != 0;
  v0 = -192; // 0xFFFFFF40
  if (temp) goto label8003F8D4;
  v0 = lw(0x80078B08);
  v0 = (int32_t)v0 < 32;
  temp = v0 == 0;
  v0 = -192; // 0xFFFFFF40
  if (temp) goto label8003F8D4;
  function_8003E90C();
  v0 = -192; // 0xFFFFFF40
  goto label8003F8D4;
label8003EF50:
  v0 = 0x80078AD4;
  a0 = v0 + 108; // 0x006C
  a1 = v0 + 144; // 0x0090
  sw(v0 + 0x0000, 0);
  spyro_vec3_copy(a0, a1);
  v0 = -3072; // 0xFFFFF400
  sw(0x80078B70, 0);
  sw(0x80078BA4, v0);
  goto label8003FBD8;
label8003EF84:
  s0 = 0x80078B40;
  a0 = s0;
  a1 = s0 + 36; // 0x0024
  spyro_vec3_copy(a0, a1);
  v0 = lw(0x80078CA8);
  temp = v0 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label8003F000;
  v0 = lw(0x80078AD4);
  v0 = v0 & 0x80;
  temp = v0 != 0;
  v0 = 129; // 0x0081
  if (temp) goto label8003F000;
  a0 = s0;
  v0 = lw(player_position + 0x0008);
  sw(0x80078BB0, v0);
  a1 = 1; // 0x0001
  v0 = spyro_vec_length(a0, a1);
  v1 = v0;
  v0 = (int32_t)v1 < 8064;
  temp = v0 == 0;
  if (temp) goto label8003EFF4;
  v1 = 8064; // 0x1F80
label8003EFF4:
  sw(0x80078B20, v1);
  v0 = 129; // 0x0081
label8003F000:
  sw(0x80078AD4, v0);
  s0 = 0x80078BA0;
  v0 = -576; // 0xFFFFFDC0
  sw(s0 + 0x0000, 0);
  sw(0x80078BA4, v0);
  s1 = s0 - 328; // 0xFFFFFEB8
  function_8003E1AC();
  function_8003E0B4();
  a1 = s1;
  a2 = 4; // 0x0004
  v1 = lw(0x800761D4);
  v0 = 1; // 0x0001
  sw(0x80078BB4, v0);
  a0 = lbu(v1 + 0x0004);
  a3 = s0 + 345; // 0x0159
  function_80055A78();
  v1 = lh(0x80076E20);
  v0 = lw(0x80078B74);
  sw(gp + 0x052C, 0);
  v0 -= v1;
  v1 = v0 & 0xFFF;
  v0 = (int32_t)v1 < 2049;
  temp = v0 != 0;
  if (temp) goto label8003F084;
  v1 -= 4096; // 0xFFFFF000
label8003F084:
  temp = (int32_t)v1 >= 0;
  v0 = v1;
  if (temp) goto label8003F090;
  v0 = -v0;
label8003F090:
  v0 = (int32_t)v0 < 513;
  temp = v0 != 0;
  if (temp) goto label8003FD50;
  s0 = lw(0x80075828);
  v0 = lw(0x80075890);
  v0 = s0 < v0;
  temp = v0 == 0;
  s3 = 30000; // 0x7530
  if (temp) goto label8003FD50;
  s4 = s1;
  s1 = s0 + 12; // 0x000C
label8003F0C4:
  v0 = lbu(s1 + 0x003C);
  v0 = v0 < 127;
  temp = v0 == 0;
  if (temp) goto label8003F1B4;
  v0 = lw(s1 - 0x0004); // 0xFFFFFFFC
  temp = v0 == 0;
  a0 = sp + 16; // 0x0010
  if (temp) goto label8003F1B4;
  a1 = s1;
  a2 = s4;
  spyro_vec3_sub(a0, a1, a2);
  a0 = lw(sp + 0x0010);
  a1 = lw(sp + 0x0014);
  a2 = 1; // 0x0001
  v0 = spyro_atan2(a0, a1, a2);
  v1 = lw(0x80078B74);
  v0 -= v1;
  v1 = v0 & 0xFFF;
  v0 = (int32_t)v1 < 2049;
  temp = v0 != 0;
  if (temp) goto label8003F128;
  v1 -= 4096; // 0xFFFFF000
label8003F128:
  temp = (int32_t)v1 >= 0;
  s2 = v1;
  if (temp) goto label8003F134;
  s2 = -s2;
label8003F134:
  v0 = (int32_t)s2 < 512;
  temp = v0 == 0;
  if (temp) goto label8003F1B4;
  a0 = lw(sp + 0x0010);
  v1 = lw(sp + 0x0014);
  v0 = lw(sp + 0x0018);
  temp = (int32_t)a0 >= 0;
  if (temp) goto label8003F158;
  a0 = -a0;
label8003F158:
  temp = (int32_t)v1 >= 0;
  if (temp) goto label8003F164;
  v1 = -v1;
label8003F164:
  a0 += v1;
  temp = (int32_t)v0 >= 0;
  if (temp) goto label8003F174;
  v0 = -v0;
label8003F174:
  a0 += v0;
  v0 = (int32_t)a0 < 16384;
  temp = v0 == 0;
  a0 = sp + 16; // 0x0010
  if (temp) goto label8003F1B4;
  a1 = 1; // 0x0001
  v0 = spyro_vec_length(a0, a1);
  a0 = v0;
  v0 = (int32_t)a0 < 6144;
  temp = v0 == 0;
  v0 = s2 << 2;
  if (temp) goto label8003F1B4;
  a0 += v0;
  v0 = (int32_t)a0 < (int32_t)s3;
  temp = v0 == 0;
  if (temp) goto label8003F1B4;
  s3 = a0;
  sw(gp + 0x052C, s0);
label8003F1B4:
  v0 = lw(0x80075890);
  s0 += 88; // 0x0058
  v0 = s0 < v0;
  temp = v0 != 0;
  s1 += 88; // 0x0058
  if (temp) goto label8003F0C4;
  goto label8003FD50;
label8003F1D4:
  s1 = 0;
  spyro_rand();
  s2 = v0 & 0x3F;
  s3 = spyro_cos_lut;
  s0 = sp + 32; // 0x0020
label8003F1EC:
  a0 = 0x80078A8C;
  a1 = s0;
  v1 = s2 << 1;
  v0 = v1 + s3;
  sw(sp + 0x0020, 0);
  v0 = lhu(v0 + 0x0000);
  a2 = s0;
  v0 = v0 << 16;
  v0 = (int32_t)v0 >> 23;
  sw(sp + 0x0024, v0);
  v0 = lhu(spyro_sin_lut + v1);
  s1++;
  v0 = v0 << 16;
  v0 = (int32_t)v0 >> 23;
  sw(sp + 0x0028, v0);
  spyro_set_mat_mirrored_vec_multiply(a0, a1, a2);
  a0 = 1; // 0x0001
  a1 = 33; // 0x0021
  a2 = s0;
  a3 = 1; // 0x0001
  function_loaded_800758E4();
  v0 = s2 + 64; // 0x0040
  s2 = v0 & 0xFF;
  v0 = (int32_t)s1 < 4;
  temp = v0 != 0;
  if (temp) goto label8003F1EC;
  s0 = 0x80078AD4;
  s1 = s0 + 108; // 0x006C
  a0 = s1;
  sw(s0 + 0x0000, 0);
  spyro_vec3_clear(a0);
  s0 += 120; // 0x0078
  a0 = s0;
  a1 = s1;
  a2 = s0;
  spyro_vec3_sub(a0, a1, a2);
  a0 = s0;
  a1 = 2; // 0x0002
  spyro_vec3_shift_right(a0, a1);
  v0 = -768; // 0xFFFFFD00
  sw(0x80078BA4, v0);
  v0 = lw(0x8007584C);
  v1 = 1; // 0x0001
  sw(0x80078BB4, v1);
  v0 = (int32_t)v0 < 120;
  temp = v0 == 0;
  v0 = 120; // 0x0078
  if (temp) goto label8003F2D8;
  sw(0x8007584C, v0);
label8003F2D8:
  v0 = lw(0x800757D0); // &0x00000000
  v0 = (int32_t)v0 < 15;
  temp = v0 == 0;
  v0 = 15; // 0x000F
  if (temp) goto label8003FD50;
  sw(0x800757D0, v0); // &0x00000000
  goto label8003FD50;
label8003F300:
  v0 = -3072; // 0xFFFFF400
  goto label8003FC64;
label8003F308:
  s0 = 0x80078AD4;
  sw(s0 + 0x0000, 0);
  s1 = s0 + 120; // 0x0078
  a0 = s1;
  spyro_vec3_clear(a0);
  a0 = s1;
  a1 = s1;
  a2 = s0 + 144; // 0x0090
  spyro_vec3_sub(a0, a1, a2);
  a0 = s1;
  a1 = 1; // 0x0001
  v0 = spyro_vec_length(a0, a1);
  a1 = v0;
  v0 = (int32_t)a1 < 7681;
  temp = v0 != 0;
  a0 = s1;
  if (temp) goto label8003F354;
  a2 = 7680; // 0x1E00
  vec3_mul_div(a0, a1, a2);
label8003F354:
  v1 = lw(0x80078AF4);
  v0 = -768; // 0xFFFFFD00
  sw(0x80078BA4, v0);
  temp = v1 != 0;
  a0 = 5; // 0x0005
  if (temp) goto label8003F37C;
  function_8003E1AC();
  a0 = 5; // 0x0005
label8003F37C:
  a1 = 10; // 0x000A
  a2 = 0;
  v0 = 1; // 0x0001
  sw(0x80078BB4, v0);
  a3 = 0;
  function_loaded_800758E4();
  v0 = lw(0x80075764); // &0x00000000
  v0 = (int32_t)v0 < 45;
  temp = v0 == 0;
  v0 = 45; // 0x002D
  if (temp) goto label8003FD50;
  sw(0x80075764, v0); // &0x00000000
  goto label8003FD50;
label8003F3C8:
  s1 = 0x80078AD4;
  sw(s1 + 0x0000, 0);
  s0 = s1 + 120; // 0x0078
  a0 = s0;
  spyro_vec3_clear(a0);
  a0 = s0;
  a1 = s0;
  a2 = s1 + 144; // 0x0090
  spyro_vec3_sub(a0, a1, a2);
  a0 = s0;
  a1 = 1; // 0x0001
  v0 = spyro_vec_length(a0, a1);
  a1 = v0;
  v0 = (int32_t)a1 < 7681;
  temp = v0 != 0;
  a0 = s0;
  if (temp) goto label8003F414;
  a2 = 7680; // 0x1E00
  vec3_mul_div(a0, a1, a2);
label8003F414:
  v1 = lw(0x80078AF4);
  v0 = -768; // 0xFFFFFD00
  sw(0x80078BA4, v0);
  temp = v1 != 0;
  a0 = 5; // 0x0005
  if (temp) goto label8003F43C;
  function_8003E1AC();
  a0 = 5; // 0x0005
label8003F43C:
  a1 = 10; // 0x000A
  a2 = 0;
  v0 = 1; // 0x0001
  sw(0x80078BB4, v0);
  a3 = 0;
  function_loaded_800758E4();
  v0 = lw(0x80075904);
  v0 = (int32_t)v0 < 15;
  temp = v0 == 0;
  v0 = 15; // 0x000F
  if (temp) goto label8003F480;
  sw(0x80075904, v0);
label8003F480:
  a1 = s1 - 124; // 0xFFFFFF84
  v0 = lw(0x800761D4);
  a2 = 4; // 0x0004
  a0 = lbu(v0 + 0x003B);
  a3 = s1 + 548; // 0x0224
  goto label8003FD48;
label8003F49C:
  s0 = 0x80078AD4;
  a0 = s0 + 120; // 0x0078
  sw(s0 + 0x0000, 0);
  spyro_vec3_clear(a0);
  a0 = 5; // 0x0005
  a1 = 10; // 0x000A
  a2 = 0;
  v0 = -3072; // 0xFFFFF400
  sw(0x80078B54, v0);
  v0 = 1; // 0x0001
  sw(0x80078BB4, v0);
  a3 = 0;
  function_loaded_800758E4();
  v0 = lw(0x80075904);
  v0 = (int32_t)v0 < 15;
  temp = v0 == 0;
  v0 = 15; // 0x000F
  if (temp) goto label8003F504;
  sw(0x80075904, v0);
label8003F504:
  a1 = s0 - 124; // 0xFFFFFF84
  v0 = lw(0x800761D4);
  a2 = 4; // 0x0004
  a0 = lbu(v0 + 0x003A);
  a3 = s0 + 548; // 0x0224
  goto label8003FD48;
label8003F520:
  v0 = 0x80078AD4;
  a0 = v0 + 120; // 0x0078
  sw(v0 + 0x0000, 0);
  spyro_vec3_clear(a0);
  a0 = 5; // 0x0005
  a1 = 10; // 0x000A
  a2 = 0;
  v0 = -768; // 0xFFFFFD00
  sw(0x80078BA4, v0);
  v0 = 1; // 0x0001
  sw(0x80078BB4, v0);
  a3 = 0;
  function_loaded_800758E4();
  goto label8003FB7C;
label8003F570:
  s0 = player_movestate;
  v1 = lw(s0 + 0x0000);
  v0 = 15; // 0x000F
  temp = v1 == v0;
  v0 = 23; // 0x0017
  if (temp) goto label8003F774;
  temp = v1 == v0;
  v0 = v1 - 32; // 0xFFFFFFE0
  if (temp) goto label8003F774;
  v0 = v0 < 2;
  temp = v0 != 0;
  v0 = 34; // 0x0022
  if (temp) goto label8003F774;
  temp = v1 == v0;
  v0 = 1; // 0x0001
  if (temp) goto label8003F774;
  v1 = lw(0x80078B6C);
  sw(0x80078AD4, 0);
  sw(0x80078BA8, v0);
  sw(0x80078BA0, 0);
  sw(0x80075960, 0);
  sw(0x800758A0, 0);
  sw(0x80075700, 0); // &0x00000000
  temp = (int32_t)v1 <= 0;
  a0 = s0 + 124; // 0x007C
  if (temp) goto label8003F5EC;
  sw(0x80078B6C, 0);
label8003F5EC:
  a1 = 0;
  v0 = spyro_vec_length(a0, a1);
  sw(0x80078B70, v0);
  v0 = (int32_t)v0 < 6401;
  temp = v0 != 0;
  v0 = 6400; // 0x1900
  if (temp) goto label8003F610;
  sw(0x80078B70, v0);
label8003F610:
  v0 = lw(0x80078B70);
  v0 = (int32_t)v0 < 1920;
  temp = v0 == 0;
  v0 = 1920; // 0x0780
  if (temp) goto label8003F630;
  sw(0x80078B70, v0);
label8003F630:
  v1 = lw(s0 + 0x0000);
  v0 = 6; // 0x0006
  temp = v1 != v0;
  if (temp) goto label8003F674;
  v0 = lw(0x80075690); // &0x00000000
  temp = v0 != 0;
  if (temp) goto label8003F674;
  v0 = lw(0x80078BB0);
  v1 = lw(player_position + 0x0008);
  v0 = (int32_t)v0 < (int32_t)v1;
  temp = v0 == 0;
  if (temp) goto label8003F684;
label8003F674:
  v0 = lw(player_position + 0x0008);
  sw(0x80078BB0, v0);
label8003F684:
  v0 = lw(0x80078B78);
  v0 = (int32_t)v0 < 1025;
  temp = v0 != 0;
  v0 = 1; // 0x0001
  if (temp) goto label8003F6AC;
  sw(0x80078CAC, v0);
  v0 = -128; // 0xFFFFFF80
  goto label8003F76C;
label8003F6AC:
  sw(0x80078CAC, 0);
  v0 = -128; // 0xFFFFFF80
  goto label8003F76C;
label8003F6BC:
  a1 = 0x80078C7C;
  v0 = lw(a1 + 0x0000);
  temp = v0 == 0;
  a0 = a1 - 304; // 0xFFFFFED0
  if (temp) goto label8003F760;
  a0 = a1 - 304; // 0xFFFFFED0
  spyro_vec3_clear(a0);
  v1 = lw(0x80078C58);
  a0 = lw(0x80078B74);
  v0 = lw(player_position + 0x0008);
  sw(0x80078BB0, v1);
  v1 -= v0;
  v1 -= 2048; // 0xFFFFF800
  sw(gp + 0x04C0, v1);
  v1 = v1 << 10;
  v1 = (int32_t)v1 >> 10;
  sw(gp + 0x0404, v0);
  v0 = lw(0x80078C7C);
  v1 += 1024; // 0x0400
  sw(gp + 0x0528, a0);
  sw(gp + 0x058C, v1);
  v0 = lbu(v0 + 0x0046);
  v0 = v0 << 4;
  v0 -= a0;
  v0 -= v1;
  v1 = v0 & 0xFFF;
  v0 = (int32_t)v1 < 2049;
  sw(gp + 0x058C, v1);
  temp = v0 != 0;
  v0 = 128; // 0x0080
  if (temp) goto label8003F76C;
  v0 = v1 - 4096; // 0xFFFFF000
  sw(gp + 0x058C, v0);
  v0 = 128; // 0x0080
  goto label8003F76C;
label8003F760:
  a1 -= 280; // 0xFFFFFEE8
  spyro_vec3_copy(a0, a1);
  v0 = 128; // 0x0080
label8003F76C:
  sw(0x80078BA4, v0);
label8003F774:
  sw(0x80078BB4, 0);
  goto label8003FD50;
label8003F784:
  v0 = s5 << 2;
  sw(0x80078AD4, 0);
  v0 = lbu(0x8006C4A2 + v0); // &0x14000216
  v1 = -3072; // 0xFFFFF400
  sw(0x80078BA4, v1);
  v0 = v0 << 4;
  sh(0x80075788, v0); // &0x00000000
  goto label8003FD50;
label8003F7BC:
  v0 = 0x80078AD4;
  a0 = v0 + 108; // 0x006C
  a1 = v0 + 144; // 0x0090
  sw(v0 + 0x0000, 0);
  spyro_vec3_copy(a0, a1);
  v0 = lw(0x80078B48);
  temp = (int32_t)v0 <= 0;
  v0 = -512; // 0xFFFFFE00
  if (temp) goto label8003F7F0;
  sw(0x80078B48, 0);
label8003F7F0:
  sw(0x80078BA4, v0);
  function_8003E1AC();
  function_8003E0B4();
  goto label8003F774;
label8003F810:
  s0 = 0x80078B4C;
  a0 = s0;
  s1 = s0 + 24; // 0x0018
  a1 = s1;
  spyro_vec3_copy(a0, a1);
  s0 -= 12; // 0xFFFFFFF4
  a0 = s0;
  a1 = s1;
  spyro_vec3_copy(a0, a1);
  v0 = lw(0x80078CA8);
  sw(0x80078BA0, 0);
  temp = v0 == 0;
  if (temp) goto label8003F8B8;
  v0 = lw(0x80078AD4);
  v0 = v0 & 0x80;
  temp = v0 != 0;
  a0 = s0;
  if (temp) goto label8003F89C;
  v0 = lw(player_position + 0x0008);
  sw(0x80078BB0, v0);
  a1 = 1; // 0x0001
  v0 = spyro_vec_length(a0, a1);
  v1 = v0;
  v0 = (int32_t)v1 < 8064;
  temp = v0 == 0;
  if (temp) goto label8003F894;
  v1 = 8064; // 0x1F80
label8003F894:
  sw(0x80078B20, v1);
label8003F89C:
  v1 = 0x80078AD4;
  v0 = lw(v1 + 0x0000);
  v0 = v0 | 0x80;
  sw(v1 + 0x0000, v0);
  goto label8003F8D0;
label8003F8B8:
  sw(0x80078AD4, 0);
  sw(0x80078B54, 0);
  sw(0x80078B48, 0);
label8003F8D0:
  v0 = -576; // 0xFFFFFDC0
label8003F8D4:
  sw(0x80078BA4, v0);
  sw(0x80078BAC, 0);
  goto label8003F774;
label8003F8EC:
  s0 = 0x80078B4C;
  a0 = s0;
  s1 = s0 + 24; // 0x0018
  a1 = s1;
  spyro_vec3_copy(a0, a1);
  a0 = s0 - 12; // 0xFFFFFFF4
  a1 = s1;
  spyro_vec3_copy(a0, a1);
  v0 = 192; // 0x00C0
  sw(0x80078BA0, 0);
  sw(0x80078AD4, v0);
  sw(0x80078BA4, 0);
  sw(0x80078BAC, 0);
  goto label8003FD50;
label8003F93C:
  v0 = 0x80078AD4;
  a2 = 1; // 0x0001
  a0 = lw(v0 + 0x0090);
  a1 = lw(0x80078B68);
  s0 = v0 + 144; // 0x0090
  sw(v0 + 0x0000, 0);
  v0 = spyro_atan2(a0, a1, a2);
  v1 = lw(0x80078B74);
  v0 -= v1;
  v0 = v0 & 0xFFF;
  v0 -= 1025; // 0xFFFFFBFF
  v0 = v0 < 2047;
  temp = v0 == 0;
  a0 = s0;
  if (temp) goto label8003F994;
  sw(0x80078B70, 0);
  v0 = -3072; // 0xFFFFF400
  goto label8003F9A8;
label8003F994:
  a1 = 0;
  v0 = spyro_vec_length(a0, a1);
  sw(0x80078B70, v0);
  v0 = -3072; // 0xFFFFF400
label8003F9A8:
  sw(0x80078BA4, v0);
  function_8003E1AC();
  v0 = 640; // 0x0280
  sw(0x80078B9C, v0);
  v0 = 1; // 0x0001
  sw(0x800758A0, 0);
  sw(0x80078BB4, v0);
  goto label8003FD50;
label8003F9E0:
  v0 = lw(0x80078C60);
  v1 = lw(0x80078C68);
  v0 = v0 << 6;
  sw(0x80078B4C, v0);
  v0 = lw(0x80078C64);
  sw(0x80078AD4, 0);
  v0 = v0 << 6;
  sw(0x80078B50, v0);
  temp = v1 != 0;
  v0 = v1 << 6;
  if (temp) goto label8003FA28;
  v0 = lw(0x80078B6C);
label8003FA28:
  sw(0x80078B54, v0);
  s0 = 0x80078B4C;
  a0 = lw(s0 + 0x0000);
  a1 = lw(0x80078B50);
  a2 = 1; // 0x0001
  v0 = spyro_atan2(a0, a1, a2);
  v1 = lw(0x80078AF4);
  v0 = -768; // 0xFFFFFD00
  sw(0x80078BA4, v0);
  temp = v1 != 0;
  a0 = 5; // 0x0005
  if (temp) goto label8003FA74;
  function_8003E1AC();
  a0 = 5; // 0x0005
label8003FA74:
  a1 = 10; // 0x000A
  a2 = 0;
  v0 = 1; // 0x0001
  sw(0x80078BB4, v0);
  a3 = 0;
  function_loaded_800758E4();
  a1 = s0 - 244; // 0xFFFFFF0C
  v0 = lw(0x800761D4);
  a0 = lbu(v0 + 0x003B);
  a2 = 4; // 0x0004
  goto label8003FB6C;
label8003FAB4:
  s0 = 0x80078AD4;
  sw(s0 + 0x0000, 0);
  v1 = lw(0x80078BB8);
  v0 = -128; // 0xFFFFFF80
  sw(0x80078BA4, v0);
  v0 = 1; // 0x0001
  sw(0x80078BB4, v0);
  temp = v1 != 0;
  a0 = 5; // 0x0005
  if (temp) goto label8003FB04;
  a1 = 10; // 0x000A
  a2 = 0;
  a3 = 0;
  function_loaded_800758E4();
label8003FB04:
  a0 = lw(0x8006EA34 + lw(0x80075728)*4);
  a1 = 0;
  function_loaded_800758CC();
  s1 = s0 - 124; // 0xFFFFFF84
  a1 = s1;
  a2 = 4; // 0x0004
  v0 = lw(0x800761D4);
  s0 += 548; // 0x0224
  a0 = lbu(v0 + 0x001D);
  a3 = s0;
  function_80055A78();
  a1 = s1;
  v0 = lw(0x800761D4);
  a2 = 4; // 0x0004
  a0 = lbu(v0 + 0x001B);
label8003FB6C:
  a3 = 0x80078CF8;
  function_80055A78();
label8003FB7C:
  v0 = lw(0x80075904);
  v0 = (int32_t)v0 < 15;
  temp = v0 == 0;
  v0 = 15; // 0x000F
  if (temp) goto label8003FD50;
  sw(0x80075904, v0);
  goto label8003FD50;
label8003FBA4:
  v0 = 0x80078AD4;
  a0 = v0 + 108; // 0x006C
  a1 = v0 + 144; // 0x0090
  sw(v0 + 0x0000, 0);
  spyro_vec3_copy(a0, a1);
  v1 = lw(0x80078AF4);
  v0 = -768; // 0xFFFFFD00
  sw(0x80078BA4, v0);
  temp = v1 != 0;
  v0 = 1; // 0x0001
  if (temp) goto label8003FBE4;
label8003FBD8:
  function_8003E1AC();
  v0 = 1; // 0x0001
label8003FBE4:
  sw(0x80078BB4, v0);
  goto label8003FD50;
label8003FBF4:
  v0 = 0x80078AD4;
  a0 = v0 + 108; // 0x006C
  a1 = v0 + 144; // 0x0090
  sw(v0 + 0x0000, 0);
  spyro_vec3_copy(a0, a1);
  v1 = lw(0x80078AF4);
  v0 = -768; // 0xFFFFFD00
  sw(0x80078BA4, v0);
  temp = v1 != 0;
  a0 = 5; // 0x0005
  if (temp) goto label8003FC34;
  function_8003E1AC();
  a0 = 5; // 0x0005
label8003FC34:
  a1 = 10; // 0x000A
  a2 = 0;
  v0 = 1; // 0x0001
  sw(0x80078BB4, v0);
  a3 = 0;
  function_loaded_800758E4();
  goto label8003FD50;
label8003FC60:
  v0 = -128; // 0xFFFFFF80
label8003FC64:
  sw(0x80078BA4, v0);
label8003FC6C:
  v0 = 1; // 0x0001
  sw(0x80078AD4, 0);
  sw(0x80078BB4, v0);
  goto label8003FD50;
label8003FC88:
  s0 = 0x80078B40;
  a0 = s0;
  a1 = s0 + 36; // 0x0024
  spyro_vec3_copy(a0, a1);
  v0 = lw(0x80078AD4);
  v0 = v0 & 0x80;
  temp = v0 != 0;
  a0 = s0;
  if (temp) goto label8003FCE8;
  v0 = lw(player_position + 0x0008);
  sw(0x80078BB0, v0);
  a1 = 1; // 0x0001
  v0 = spyro_vec_length(a0, a1);
  v1 = v0;
  v0 = (int32_t)v1 < 8064;
  temp = v0 == 0;
  if (temp) goto label8003FCE0;
  v1 = 8064; // 0x1F80
label8003FCE0:
  sw(0x80078B20, v1);
label8003FCE8:
  s0 = 0x80078AD4;
  v0 = 192; // 0x00C0
  sw(s0 + 0x0000, v0);
  v0 = -576; // 0xFFFFFDC0
  sw(0x80078BA0, 0);
  sw(0x80078BA4, v0);
  function_8003E1AC();
  function_8003E0B4();
  a1 = s0 - 124; // 0xFFFFFF84
  a2 = 4; // 0x0004
  v1 = lw(0x800761D4);
  v0 = 1; // 0x0001
  sw(0x80078BB4, v0);
  sw(0x80075700, 0); // &0x00000000
  a0 = lbu(v1 + 0x0005);
  a3 = s0 + 549; // 0x0225
label8003FD48:
  function_80055A78();
label8003FD50:
  v0 = lw(0x80078AD8);
  sw(player_movestate, s5);
  v1 = lw(player_movestate);
  sw(0x80078AD8, 0);
  sw(0x80078ADC, v0);
  v0 = lbu(0x8006C470 + v1); // &0x03020100
  v0 = v0 << 2;
  v0 = lbu(0x8006C4A3 + v0); // &0x14140002
  sw(0x80078AB0, v0);
  ra = lw(sp + 0x0048);
  s5 = lw(sp + 0x0044);
  s4 = lw(sp + 0x0040);
  s3 = lw(sp + 0x003C);
  s2 = lw(sp + 0x0038);
  s1 = lw(sp + 0x0034);
  s0 = lw(sp + 0x0030);
  sp += 80; // 0x0050
  return;
}

void spyro_change_movestate(uint32_t movestate)
{
  a0 = movestate;
  function_8003EA68();
}

// size: 0x00000078
void function_8003FDC8(void)
{
  spyro_change_movestate(a0);

  a0 = lw(player_movestate);
  v1 = lbu(0x8006C470 + a0);
  sb(0x80078A76, 0);
  sb(0x80078A70, v1);
  sb(0x80078A77, 1);
  sb(0x80078A7C, 0);
  sw(0x80078AAC, 0);
  sw(0x80078AB4, a0);
  sb(0x80078A71, v1);
}

// size: 0x0000004C
void function_8002C618(void)
{
  a0 = 0;
  function_80056B28();
  v0 = 10; // 0x000A
  sw(gamestate, v0); // &0x00000000
  sw(0x8007568C, 0); // &0x00000000
  sw(0x800758B8, 0);
  a0 = 0;
  function_8004AC24();
  spyro_change_movestate(MOVESTATE_STAND);
}

// size: 0x000000B0
void function_8002C664(void)
{
  function_80058C7C();
  uint32_t level_id = lw(LEVEL_ID);
  sw(0x80075864, 0);
  mult(level_id, 0x66666667);
  sw(0x800756AC, 0);
  sw(0x80076E90, 0x80000012);
  sw(0x8007576C, -1);
  v0 = 1; // 0x0001
  sw(0x800758AC, level_id);
  a0 = (int32_t)level_id >> 31;
  sw(gamestate, 1);
  sw(0x800756D0, 1);
  sw(0x800756B0, 1);
  sw(0x8007579C, 1);
  sw(0x800758B4, level_id/10*10);
}

// size: 0x000000A0
void function_8002C7BC(void)
{
  RECT rect = {0x200, 0, 0x100, 0xE1};
  LoadImage(&rect, addr_to_pointer(lw(0x800785F0) - 0x1C200));
  DrawSync(0);
  sw(gamestate, 0); // &0x00000000
  function_80058C7C();
  a0 = lw(0x800774B0);
  sb(0x80077FA8, 0);
  sb(0x80077FA9, 0);
  sb(0x80077FAA, 0);
  sb(0x80077FAB, 0);
  sb(0x80077FAC, 0);
  a1 = 8; // 0x0008
  function_800567F4();
}

// size: 0x00000048
void function_8002C85C(void)
{
  v0 = lw(lives);
  if (v0) {
    sw(lives, v0-1);
    v0 = GAMESTATE_DEAD;
  } else
    v0 = GAMESTATE_GAMEOVER;
  
  sw(gamestate, v0);
  sw(0x80075940, 0);
  sw(0x8007593C, 0);
  return;
}

// size: 0x00000040
void function_80058B68(void)
{
  for (int i = 0; i < 16; i++) sw(0x80078800 + i*0x24, 0);
  for (int i = 0; i < 8; i++) sb(0x80077114 + i*0x18, 0);
}

// size: 0x000000F8
void function_80059474(void)
{
  sw(0x80077888 + 0x5C, (a1 & 0xFF)*16);
  spyro_vec3_copy(0x80077888 + 0x50, a0 + 0x0C);
  sw(0x80077888, 1);
  v1 = lw(CONTINUOUS_LEVEL_ID)*4;
  sw(0x80077888 + 0x58, lw(0x80077888 + 0x58) + 0x164);
  sw(0x80077888 + 0x04, lw(level_dragons + v1));
  sw(0x80077888 + 0x0C, 0);
  sw(0x80077888 + 0x08, lw(level_gems + v1));
  spyro_memcpy32(0x80077888 + 0x10, 0x80077888 + 0x30, 0x20);
  
  sw(0x80077888 + 0x64, lw(0x80077888 + 0x60));
}



#ifdef __wasm__

#include <wasm.h>

WASM_PUBLIC void actual_game_loop(void)
{
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

void request_animation_frame(void);

void game_loop(void)
{
  initial_loading_screen();

  request_animation_frame();
}

#else // __wasm__

// probably main()
void game_loop(void)
{
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
  DEPRECATED;
  game_loop();
}

#endif // __wasm__