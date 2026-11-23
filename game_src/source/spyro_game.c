#include "spyro_game.h"
#include "debug.h"
#include "main.h"
#include "decompilation.h"
#include "psx_mem.h"
#include "spyro_graphics.h"
#include "spyro_string.h"
#include "spyro_math.h"
#include "spyro_vsync.h"
#include "spyro_spu.h"
#include "psx_ops.h"
#include "int_math.h"
#include "not_renamed.h"
#include "level_loading.h"
#include "function_chooser.h"
#include "extra_gte.h"
#include "gte.h"
#include "spyro_memory_card.h"
#include "spyro_cdrom.h"
#include "spyro_controller.h"

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
    v1 = (lw(v0 + 0x24) >> 21) & 7;
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
    v1 = lw(a1);
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
  t6 = lw(0x80076378);
  at = lw(t6 + 0x38 + at*4);
  v0 = lw(at + 0x24 + v0*4);
  a3 = (v0 >> 21) & 1;
  v0 = v0 << 11;
  v0 = v0 >> 10;
  a0 = lw(at + 0x10);
  a1 = lw(v0 + 0x08);
  a2 = v0 + 0x18;
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
  t0 = lbu(a1);
  t1 = lw(a0);
  a1++;
  a0 += 4; // 0x0004
  a3 = t0 & 1;
  t0 = t0 << 1;
  t0 = t0 & 0x1FC;
  t0 += t6;
  t0 = lw(t0);
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
  t0 = lh(a2);
  a2 += 2; // 0x0002
  temp = (int32_t)t0 < 0;
  a3 = t0 & 0x4000;
  if (temp) goto label80053078;
  t1 = lw(a0);
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
  t1 = lhu(a2);
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
  sw(t9, at);
  sw(t9 + 0x0004, v0);
  sw(t9 + 0x0008, v1);
  return;
}

// size: 0x000001D4
void function_80051FEC(void)
{
  memset(addr_to_pointer(0x80077868), 0, 32);
  vec3_32 pos = *(vec3_32*)addr_to_pointer(PLAYER_POSITION + 0);
  t6 = 0x8006FCF4 + 0x400;
  for (struct game_object *object = addr_to_pointer(lw(0x80075828)); object->unknown48 != -1; object++) {
    if ((int32_t)object->unknown48 < 0) continue;
    if (object->unknown51 == 0) {
      v1 = object->unknown52;
      if (v1 == 0) continue;
      if ((int32_t)v1 > 0) {
        v1 = v1 << 10;
        vec3_32 p = vec3_32_sub(object->p, pos);
        if (abs_int(p.x) >= (int32_t)v1
        || abs_int(p.y) >= (int32_t)v1
        || abs_int(p.z) >= (int32_t)v1) continue;
        if ((int)(v1/8*v1/8) <= p.x/8*p.x/8 + p.y/8*p.y/8) continue;
      }
    }
    if ((int32_t)object->unknown43 >= 0) {
      sb(0x80077868 + object->unknown43, 1);
    } else {
      sw(t6, pointer_to_addr(object));
      t6 += 4;
    }
  }

  struct game_object *game_objects = addr_to_pointer(lw(0x80075828));
  v0 = lw(0x800757F8);
  for (int i = 0; i < 0x20; i++) {
    if (lb(0x80077868+i) == 0) continue;

    a2 = lw(v0 + i*4);
    while (1) {
      a3 = lh(a2);
      a2 += 2;
      struct game_object *object = &game_objects[a3 & 0x7FFF];
      if (object->unknown48 >= 0) {
        sw(t6, pointer_to_addr(object));
        t6 += 4;
      }
      if ((int32_t)a3 < 0) break;
    }
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

  vec3_32 pos = *(vec3_32*)addr_to_pointer(PLAYER_POSITION + 0);

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
  struct game_object *game_object = addr_to_pointer(lw(GAMEOBJECT_STACK_PTR));

  for (uint8_t ch = *str++; ch; ch = *str++) {
    if (ch == ' ') {
      pos->x += spacing;
      continue;
    }

    game_object--;
    *game_object = (struct game_object){0};
    game_object->p = *pos;
    if (ch >= '0' && ch <= '9') {
      game_object->modelID = MODEL_ID_0 + ch - '0';
    } else if (ch >= 'A' && ch <= 'Z') {
      game_object->modelID = MODEL_ID_A + ch - 'A';
    } else if (ch == '/') {
      game_object->modelID = MODEL_ID_FORWARDSLASH;
    } else if (ch == '?') {
      game_object->modelID = MODEL_ID_QUESTION_MARK;
    } else if (ch == '%') {
      game_object->modelID = MODEL_ID_PERCENT;
    } else if (ch == '^') {
      game_object->modelID = MODEL_ID_CARET;
    } else if (ch == '+') {
      game_object->modelID = MODEL_ID_PLUS;
    } else {
      game_object->modelID = MODEL_ID_DOT;
    }
    game_object->unknown47 = 0x7F;
    game_object->unknown4F = unknown_metadata;
    game_object->render_distance = 0xFF;
    pos->x += spacing;
  }
  sw(GAMEOBJECT_STACK_PTR, pointer_to_addr(game_object));
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
  struct game_object *game_object = addr_to_pointer(lw(GAMEOBJECT_STACK_PTR));
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
      game_object->modelID = MODEL_ID_0 + ch - '0';
    } else if (ch >= 'A' && ch <= 'Z') {
      game_object->modelID = MODEL_ID_A + ch - 'A';
    } else if (ch == '!') {
      game_object->modelID = MODEL_ID_EXCLAMATIONMARK;
    } else if (ch == ',') {
      game_object->modelID = MODEL_ID_COMMA;
    } else if (ch == '?') {
      game_object->modelID = MODEL_ID_QUESTION_MARK;
    } else if (ch == '.') {
      game_object->modelID = MODEL_ID_DOT;
    } else {
      game_object->modelID = MODEL_ID_COMMA;
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
  sw(GAMEOBJECT_STACK_PTR, pointer_to_addr(game_object));
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
  s2 = -1;
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
  sb(s7, v0);
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
  v0 = s3 | 1;
  a2 = 0x800761DC;
  sh(0x80075F30 + 0x000E + a0, v0);
  sw(0x80075F30 + a0, s1);
  sb(0x80075F30 + 0x000D + a0, s4);
  a1 = lw(a2);
  v0 = a1 & 2;
  temp = v0 == 0;
  v0 = -3; // 0xFFFFFFFD
  if (temp) goto label80055C68;
  v1 = lhu(0x800761F4);
  v0 = a1 & v0;
  sw(a2, v0);
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
  v0 = lw(a0);
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
  v1 = lw(a0);
  v0 = v1 & 1;
  temp = v0 == 0;
  v0 = -2; // 0xFFFFFFFE
  if (temp) goto label80055F8C;
  v0 = v1 & v0;
  sw(a0, v0);
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
  v1 = lw(a0);
  v0 = v1 & 1;
  temp = v0 == 0;
  v0 = -2; // 0xFFFFFFFE
  if (temp) goto label80056058;
  v0 = v1 & v0;
  sw(a0, v0);
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
  sb(s7, s2);
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
        sw(0x80075F30 + i*0x1C + 0x00, 0);
        sb(0x80075F30 + i*0x1C + 0x0D, 0xFF);
        sh(0x80075F30 + i*0x1C + 0x0E, 0x40);
        sw(0x80075F30 + i*0x1C + 0x18, 0);
      }
    }
  }
  sw(0x80076238, 0);
  a0 = 0;
  a1 = 4;
  function_800567F4();
}

// size: 0x000000A4
void function_80056200(void)
{
  a2 = a0 & 0x7F;
  if (a2 >= 24) return;
  sw(0x8007623C, lw(0x8007623C) | (1 << a2));
  if (a1 == 2 || a1 == 4) {
    v1 = a2*0x1C;
    a0 = lw(0x80075F30 + v1 + 0x18);
    if (a0)
      sb(a0, 0x7F);
    sw(0x80075F30 + v1 + 0x00, 0);
    sb(0x80075F30 + v1 + 0x0D, 0xFF);
    sh(0x80075F30 + v1 + 0x0E, 0x40);
    sw(0x80075F30 + v1 + 0x18, 0);
  }
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
  a0=lo;
  v0 = lh(t2);
  mult(a0, v0);
  t3=lo;
  v1 = t3 >> 8;
  v0 -= v1;
  v0 = v0 << 16;
  v0 = (int32_t)v0 >> 16;
  mult(t1, v0);
  t3=lo;
  v0 = (int32_t)t3 >> 7;
  sh(t0, v0);
  v0 = lh(t2 + 0x0002);
  mult(a0, v0);
  t3=lo;
  v1 = t3 >> 8;
  v0 -= v1;
  v0 = v0 << 16;
  v0 = (int32_t)v0 >> 16;
  mult(a2, v0);
  v1 = lh(t0);
  t3=lo;
  v0 = (int32_t)t3 >> 7;
  temp = (int32_t)v1 >= 0;
  sh(t0 + 0x0002, v0);
  if (temp) goto label80056D54;
  sh(t0, 0);
  goto label80056D64;
label80056D54:
  v0 = (int32_t)v1 < 0x4000;
  temp = v0 != 0;
  v0 = 16383; // 0x3FFF
  if (temp) goto label80056D64;
  sh(t0, v0);
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
  v1 = lh(t0);
  v0 += v1;
  v0 = v0 >> 1;
  sh(t0, v0);
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
  v1 = lbu(a2);
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

// size: 0x0000002C
void function_8002D554(void)
{
  memset(addr_to_pointer(0x80076BC0), 0, 0x40);
  sw(0x80075928, 0);
}

// size: 0x000000A8
void function_8002C714(void)
{
  uint32_t s0 = a0;
  if (s0) {
    a0 = 0;
    function_80056B28();
  }
  a0 = lbu(lw(0x800761D4) + 0x2C);
  a1 = 0;
  a2 = 0x10;
  a3 = 0;
  function_80055A78();
  sw(0x800757CC, 0);
  sw(0x800756D4, 0);
  sw(GAMESTATE, 3);

  sw(HOMEWORLD_TO_LOAD, lw(CONTINUOUS_LEVEL_ID)/6);
  if (s0)
    sw(0x800758B8, 0);
    
  function_8002D554();
}

// size: 0x000000C0
uint32_t completion_percentage(void)
{
  uint32_t gems = lw(TOTAL_FOUND_GEMS);
  uint32_t dragons = lw(TOTAL_FOUND_DRAGONS);
  uint32_t eggs = lw(TOTAL_FOUND_EGGS);
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

// size: 0x000020D0
void function_8002F3E4(void)
{
  uint32_t temp;
  sp -= 112; // 0xFFFFFF90
  v1 = 0x8007706C;
  sw(sp + 0x6C, ra);
  sw(sp + 0x68, s4);
  sw(sp + 0x64, s3);
  sw(sp + 0x60, s2);
  sw(sp + 0x5C, s1);
  sw(sp + 0x58, s0);
  a0 = lw(0x800756CC);
  sw(v1, lw(v1) + a0);
  v1 = lw(0x80077058);
  sw(0x80077080, lw(0x80077080) + a0);
  v0 = v1 < 8;
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label800313F8;
  v0 = lw(0x80010DE8 + v0); // &0x8002F460
  temp = v0;
  switch (temp)
  {
  case 0x8002F460: // 0
    goto label8002F460;
    break;
  case 0x8002F7E4: // 1
    goto label8002F7E4;
    break;
  case 0x800303D0: // 2
    goto label800303D0;
    break;
  case 0x800306D4: // 3
    goto label800306D4;
    break;
  case 0x80030804: // 4
    goto label80030804;
    break;
  case 0x80030F30: // 5
    goto label80030F30;
    break;
  case 0x8003115C: // 6
    goto label8003115C;
    break;
  case 0x800312B8: // 7
    goto label800312B8;
    break;
  default:
    JR(temp, 0x8002F458);
    return;
  }
label8002F460:
  sw(LEVEL_FRAME_COUNTER, lw(LEVEL_FRAME_COUNTER)+1);
  function_loaded_80075734();
  function_8004A200();
  function_80037BD4();
  a0 = lw(0x8007706C);
  v1 = lw(0x800756CC); // &0x00000000
  v0 = (int32_t)a0 >> 4;
  v0 += 2; // 0x0002
  mult(v0, v1);
  v0 = 1; // 0x0001
  sw(0x80076248, v0);
  v0 = a0 << 2;
  v0 += 64; // 0x0040
  sb(0x80076273, v0);
  v0 = lbu(0x80076272);
  v1 = a0 << 4;
  sw(0x8007626C, v1);
  t0=lo;
  v0 += t0;
  sb(0x80076272, v0);
  v0 = (int32_t)a0 < 33;
  temp = v0 != 0;
  v0 = a0 - 32; // 0xFFFFFFE0
  if (temp) goto label8002F510;
  v0 = v0 << 4;
  sw(0x80077074, v0);
  goto label8002F518;
label8002F510:
  sw(0x80077074, 0);
label8002F518:
  v0 = lw(0x800757D0); // &0x00000000
  v0 = (int32_t)v0 < 8;
  temp = v0 == 0;
  v0 = 8; // 0x0008
  if (temp) goto label8002F538;
  sw(0x800757D0, v0); // &0x00000000
label8002F538:
  s1 = 0x8007706C;
  v0 = lw(s1);
  v1 = 70; // 0x0046
  sw(0x8007584C, v1);
  v0 = (int32_t)v0 < 48;
  temp = v0 != 0;
  v0 = 255; // 0x00FF
  if (temp) goto label800313F8;
  s2 = 1; // 0x0001
  sw(0x80077058, s2);
  sw(s1, 0);
  v1 = lw(0x800770C0);
  a2 = 1; // 0x0001
  sw(0x80077074, v0);
  v0 = lw(v1 + 0x0014);
  s0 = PLAYER_POSITION;
  v0 -= 120; // 0xFFFFFF88
  sw(v1 + 0x0014, v0);
  v0 = lw(0x800770C0);
  a3 = lw(s0);
  v1 = lw(PLAYER_POSITION + 4);
  a0 = lw(v0 + 0x000C);
  a1 = lw(v0 + 0x0010);
  a0 = a3 - a0;
  a1 = v1 - a1;
  v0 = spyro_atan2(a0, a1, a2);
  a0 = sp + 16; // 0x0010
  a2 = lw(0x800770C0);
  a1 = s0;
  sw(0x80077090, v0);
  a2 += 12; // 0x000C
  spyro_vec3_sub(a0, a1, a2);
  a0 = sp + 16; // 0x0010
  a1 = 0;
  v0 = spyro_vec_length(a0, a1);
  v1 = lw(0x800770C0);
  sw(0x80077094, v0);
  v0 = lw(v1);
  v0 = lw(v0 + 0x0028);
  sw(0x80077098, v0);
  v0 = lw(v1);
  a2 = 1; // 0x0001
  v0 = lw(v0 + 0x002C);
  s0 = 0x80076DF8;
  sw(0x8007709C, v0);
  a3 = lw(s0);
  a0 = lw(v1 + 0x000C);
  v0 = lw(0x80076DFC);
  a1 = lw(v1 + 0x0010);
  a0 = a3 - a0;
  a1 = v0 - a1;
  v0 = spyro_atan2(a0, a1, a2);
  a0 = sp + 16; // 0x0010
  a2 = lw(0x800770C0);
  a1 = s0;
  sw(0x800770A0, v0);
  a2 += 12; // 0x000C
  spyro_vec3_sub(a0, a1, a2);
  a0 = sp + 16; // 0x0010
  a1 = 0;
  v0 = spyro_vec_length(a0, a1);
  a0 = lw(0x80076E00);
  v1 = lw(0x800770C0);
  a2 = 1; // 0x0001
  sw(0x800770A4, v0);
  sw(0x800770A8, a0);
  s0 = lw(v1);
  a0 = lw(v1 + 0x000C);
  a1 = lw(v1 + 0x0010);
  a3 = lw(s0);
  v0 = lw(s0 + 0x0004);
  a0 = a3 - a0;
  a1 = v0 - a1;
  v0 = spyro_atan2(a0, a1, a2);
  a0 = sp + 16; // 0x0010
  a2 = lw(0x800770C0);
  a1 = s0;
  sw(0x800770AC, v0);
  a2 += 12; // 0x000C
  spyro_vec3_sub(a0, a1, a2);
  a0 = sp + 16; // 0x0010
  a1 = 0;
  v0 = spyro_vec_length(a0, a1);
  sw(0x800770B0, v0);
  v0 = lw(s0 + 0x0008);
  a0 = lw(0x80077090);
  a1 = lw(0x80077098);
  sw(0x800770B4, v0);
  v0 = spyro_two_angle_diff_12bit(a0, a1);
  v0 = (int32_t)v0 >> 4;
  v0 += 40; // 0x0028
  sw(0x80077070, v0);
  v0 = (int32_t)v0 < 64;
  temp = v0 == 0;
  v0 = 64; // 0x0040
  if (temp) goto label8002F734;
  sw(0x80077070, v0);
label8002F734:
  v0 = lw(0x80077070);
  v0 = (int32_t)v0 < 129;
  temp = v0 != 0;
  v0 = 128; // 0x0080
  if (temp) goto label8002F754;
  sw(0x80077070, v0);
label8002F754:
  v0 = lw(0x800786C8 + 0x00A0);
  sw(0x800758A4, v0);
  a0 = 1; // 0x0001
  function_8004AC24();
  v1 = lw(0x80075824);
  v0 = 1; // 0x0001
  sb(PLAYER_DATA + 0x18, v0);
  sb(PLAYER_DATA + 0x19, v0);
  sb(PLAYER_DATA + 0x1F, v0);
  v0 = 255; // 0x00FF
  sb(PLAYER_DATA + 0x1E, 0);
  sb(PLAYER_DATA + 0x24, 0);
  sw(0x80075738, v1); // &0x00000000
  sb(v1 + 0x0001, v0);
  v0 = lw(0x800770C0);
  a0 = lbu(v0 + 0x0054);
  a0 = s2 << a0;
  function_80056B28();
  a1 = 0;
  v0 = lw(0x800761D4);
  a0 = lbu(v0 + 0x0032);
  a2 = 16; // 0x0010
  goto label800312A0;
label8002F7E4:
  a0 = lw(0x8007706C);
  v0 = (int32_t)a0 < 32;
  temp = v0 == 0;
  v1 = 32; // 0x0020
  if (temp) goto label8002F85C;
  v1 -= a0;
  v0 = (int32_t)v1 >> 4;
  a0 = lw(0x800756CC); // &0x00000000
  v0 += 2; // 0x0002
  mult(v0, a0);
  v0 = 1; // 0x0001
  sw(0x80076248, v0);
  v0 = v1 << 2;
  v0 += 64; // 0x0040
  sb(0x80076273, v0);
  v0 = lbu(0x80076272);
  v1 = v1 << 4;
  sw(0x8007626C, v1);
  t0=lo;
  v0 += t0;
  sb(0x80076272, v0);
  goto label8002F864;
label8002F85C:
  sw(0x80076248, 0);
label8002F864:
  v1 = lw(0x8007706C);
  v0 = (int32_t)v1 < 8;
  temp = v0 == 0;
  v0 = 8; // 0x0008
  if (temp) goto label8002F894;
  v0 -= v1;
  v0 = v0 << 5;
  sw(0x80077074, v0);
  goto label8002F89C;
label8002F894:
  sw(0x80077074, 0);
label8002F89C:
  v1 = lw(0x800770C0);
  v0 = lbu(v1 + 0x0049);
  v0++;
  v0 = v0 & 3;
  sb(v1 + 0x0049, v0);
  v1 = lw(0x800770C0);
  v0 = lbu(v1 + 0x0049);
  v0 = v0 << 1;
  v0 = lbu(0x8006F390 + v0); // &0x00FF0001
  sb(v1 + 0x0044, v0);
  v1 = lw(0x800770C0);
  v0 = lbu(v1 + 0x0049);
  v0 = v0 << 1;
  v0 = lbu(0x8006F391 + v0); // &0x0000FF00
  sb(v1 + 0x0045, v0);
  v0 = lw(0x800757D0); // &0x00000000
  v0 = (int32_t)v0 < 8;
  temp = v0 == 0;
  v0 = 8; // 0x0008
  if (temp) goto label8002F934;
  sw(0x800757D0, v0); // &0x00000000
label8002F934:
  a1 = lw(0x800770C0);
  sw(0x8007584C, 0x46);
  a0 = 0xFB;
  function_loaded_800758CC();
  a0 = lw(0x8007706C);
  v1 = lw(0x80077070);
  v0 = (int32_t)a0 < (int32_t)v1;
  temp = v0 == 0;
  v0 = v1 + 32; // 0x0020
  if (temp) goto label8002FD6C;
  a0 = a0 << 11;
  div_psx(a0,v1);
  a0=lo;
  a0 -= 0x400;
  v0 = spyro_sin(a0);
  a0 = lw(0x80077098);
  a1 = lw(0x80077090);
  s3 = v0 + 0x1000;
  v0 = a0 - a1;
  v0 = v0 & 0xFFF;
  v0 = (int32_t)v0 < 2049;
  temp = v0 == 0;
  if (temp) goto label8002F9FC;
  v0 = spyro_two_angle_diff_12bit(a0, a1);
  mult(v0, s3);
  v1 = lw(0x80077090);
  t0=lo;
  v0 = (int32_t)t0 >> 13;
  s1 = v1 + v0;
  goto label8002FA1C;
label8002F9FC:
  v0 = spyro_two_angle_diff_12bit(a0, a1);
  mult(v0, s3);
  v1 = lw(0x80077090);
  t0=lo;
  v0 = (int32_t)t0 >> 13;
  s1 = v1 - v0;
label8002FA1C:
  a0 = lw(0x8007706C);
  v0 = lw(0x80077070);
  a0 = a0 << 10;
  div_psx(a0,v0);
  a0=lo;
  a0 -= 0x400;
  v0 = spyro_sin(a0);
  v1 = lw(0x8007709C);
  a2 = lw(0x80077094);
  v0 += 0x1000;
  v1 -= a2;
  mult(v1, v0);
  a0 = sp + 0x10;
  s0 = PLAYER_POSITION;
  a1 = s0;
  t0=lo;
  v0 = (int32_t)t0 >> 12;
  s2 = a2 + v0;
  spyro_vec3_copy(a0, a1);
  a0 = s1;
  v0 = spyro_cos(a0);
  mult(v0, s2);
  v0 = lw(0x800770C0);
  a0 = s1;
  v0 = lw(v0 + 0x000C);
  t0=lo;
  v1 = (int32_t)t0 >> 12;
  v0 += v1;
  sw(s0, v0);
  v0 = spyro_sin(a0);
  mult(v0, s2);
  a0 = sp + 16; // 0x0010
  a1 = s0;
  v0 = lw(0x800770C0);
  v0 = lw(v0 + 0x0010);
  t0=lo;
  v1 = (int32_t)t0 >> 12;
  v0 += v1;
  sw(PLAYER_POSITION + 4, v0);
  a2 = a0;
  spyro_vec3_sub(a0, a1, a2);
  a0 = lw(sp + 0x0010);
  a1 = lw(sp + 0x0014);
  a2 = 0;
  v0 = spyro_atan2(a0, a1, a2);
  s0 = v0;
  a1 = lbu(PLAYER_DATA + 0x0E);
  a0 = s0;
  v0 = spyro_two_angle_diff_8bit(a0, a1);
  v0 = (int32_t)v0 < 4;
  temp = v0 != 0;
  if (temp) goto label8002FB94;
  a1 = lbu(PLAYER_DATA + 0x0E);
  v0 = s0 - a1;
  v0 = v0 & 0xFF;
  v0 = (int32_t)v0 < 129;
  temp = v0 == 0;
  if (temp) goto label8002FB74;
  a0 = s0;
  v0 = spyro_two_angle_diff_8bit(a0, a1);
  v1 = lbu(PLAYER_DATA + 0x0E);
  v0 = (int32_t)v0 >> 2;
  v1 += v0;
  goto label8002FB8C;
label8002FB74:
  a0 = s0;
  v0 = spyro_two_angle_diff_8bit(a0, a1);
  v1 = lbu(PLAYER_DATA + 0x0E);
  v0 = (int32_t)v0 >> 2;
  v1 -= v0;
label8002FB8C:
  sb(PLAYER_DATA + 0x0E, v1);
label8002FB94:
  a0 = sp + 16; // 0x0010
  a1 = 0;
  v0 = spyro_vec_length(a0, a1);
  v1 = PLAYER_DATA + 0x58;
  v0 = (int32_t)v0 >> 1;
  sw(v1, v0);
  v0 = (int32_t)v0 < 4;
  temp = v0 == 0;
  v0 = 4; // 0x0004
  if (temp) goto label8002FBC0;
  sw(v1, v0);
label8002FBC0:
  v0 = lw(v1);
  v0 = (int32_t)v0 < 17;
  temp = v0 != 0;
  v0 = 16; // 0x0010
  if (temp) goto label8002FBD8;
  sw(v1, v0);
label8002FBD8:
  s0 = 0x800770AC;
  a0 = lw(s0);
  a1 = lw(s0 - 0x000C); // 0xFFFFFFF4
  v0 = a0 - a1;
  v0 = v0 & 0xFFF;
  v0 = (int32_t)v0 < 2049;
  temp = v0 == 0;
  if (temp) goto label8002FC20;
  v0 = spyro_two_angle_diff_12bit(a0, a1);
  mult(v0, s3);
  v1 = lw(s0 - 0x000C); // 0xFFFFFFF4
  t0=lo;
  v0 = (int32_t)t0 >> 13;
  s1 = v1 + v0;
  goto label8002FC3C;
label8002FC20:
  v0 = spyro_two_angle_diff_12bit(a0, a1);
  mult(v0, s3);
  v1 = lw(s0 - 0x000C); // 0xFFFFFFF4
  t0=lo;
  v0 = (int32_t)t0 >> 13;
  s1 = v1 - v0;
label8002FC3C:
  v0 = lw(0x800770B0);
  v1 = lw(0x800770A4);
  v0 -= v1;
  mult(v0, s3);
  a0 = s1;
  t1=lo;
  v0 = (int32_t)t1 >> 13;
  s2 = v1 + v0;
  v0 = spyro_cos(a0);
  mult(v0, s2);
  v0 = lw(0x800770C0);
  v0 = lw(v0 + 0x000C);
  v1=lo;
  v1 = (int32_t)v1 >> 12;
  v0 += v1;
  sw(0x80076DF8, v0);
  a0 = s1;
  v0 = spyro_sin(a0);
  mult(v0, s2);
  v0 = lw(0x800770B4);
  a0 = lw(0x800770A8);
  v1=lo;
  v0 -= a0;
  mult(v0, s3);
  v0 = lw(0x800770C0);
  v0 = lw(v0 + 0x0010);
  v1 = (int32_t)v1 >> 12;
  v0 += v1;
  sw(0x80076DFC, v0);
  t1=lo;
  v0 = (int32_t)t1 >> 13;
  a0 += v0;
  v0 = lw(0x80077070);
  sw(0x80076E00, a0);
  a0 = lw(0x8007706C);
  v1 = v0 - 16; // 0xFFFFFFF0
  v0 = (int32_t)a0 < (int32_t)v1;
  temp = v0 != 0;
  if (temp) goto label8002FF78;
  v0 = lw(0x800756CC); // &0x00000000
  v0 = a0 - v0;
  v0 = (int32_t)v0 < (int32_t)v1;
  temp = v0 == 0;
  v1 = 3; // 0x0003
  if (temp) goto label8002FF78;
  v0 = lbu(PLAYER_DATA + 0x1F);
  sb(PLAYER_DATA + 0x19, v1);
  v1 = 12; // 0x000C
  sb(PLAYER_DATA + 0x1F, 0);
  sb(PLAYER_DATA + 0x24, 0);
  sw(PLAYER_DATA + 0x58, v1);
  sb(PLAYER_DATA + 0x1E, v0);
  goto label8002FF78;
label8002FD6C:
  v0 = (int32_t)a0 < (int32_t)v0;
  temp = v0 == 0;
  v0 = 2; // 0x0002
  if (temp) goto label8002FF2C;
  v0 = lw(0x800756CC); // &0x00000000
  v0 = a0 - v0;
  v0 = (int32_t)v0 < (int32_t)v1;
  temp = v0 == 0;
  if (temp) goto label8002FE58;
  a0 = lw(0x80077098);
  v0 = spyro_cos(a0);
  v1 = lw(0x8007709C);
  mult(v0, v1);
  v0 = lw(0x800770C0);
  a0 = lw(0x80077098);
  v0 = lw(v0 + 0x000C);
  t0=lo;
  v1 = (int32_t)t0 >> 12;
  v0 += v1;
  sw(PLAYER_POSITION + 0, v0);
  v0 = spyro_sin(a0);
  v1 = lw(0x8007709C);
  mult(v0, v1);
  a1 = lw(0x800770C0);
  a0 = lw(a1 + 0x10);
  v0 = lbu(PLAYER_DATA + 0x0E)*16;
  v1 = (lw(0x80077098) + 0x800) & 0xFFF;
  sw(0x80077090, v0);
  sw(0x80077098, v1);
  t0=lo;
  v0 = (int32_t)t0 >> 12;
  a0 += v0;
  sw(PLAYER_POSITION + 4, a0);
  spyro_vec3_copy(0x80076DF8, lw(a1 + 0x00));
label8002FE58:
  a0 = lw(0x80077098);
  a1 = lw(0x80077090);
  v0 = a0 - a1;
  v0 = v0 & 0xFFF;
  v0 = (int32_t)v0 < 0x801;
  temp = v0 == 0;
  if (temp) goto label8002FEC8;
  v0 = spyro_two_angle_diff_12bit(a0, a1);
  v1 = lw(0x8007706C);
  a0 = lw(0x80077070);
  v1 -= a0;
  mult(v0, v1);
  v0=lo;
  temp = (int32_t)v0 >= 0;
  if (temp) goto label8002FEB4;
  v0 += 31; // 0x001F
label8002FEB4:
  v1 = lw(0x80077090);
  v0 = (int32_t)v0 >> 5;
  s1 = v1 + v0;
  goto label8002FF0C;
label8002FEC8:
  v0 = spyro_two_angle_diff_12bit(a0, a1);
  v1 = lw(0x8007706C);
  a0 = lw(0x80077070);
  v1 -= a0;
  mult(v0, v1);
  v0=lo;
  temp = (int32_t)v0 >= 0;
  if (temp) goto label8002FEFC;
  v0 += 31; // 0x001F
label8002FEFC:
  v1 = lw(0x80077090);
  v0 = (int32_t)v0 >> 5;
  s1 = v1 - v0;
label8002FF0C:
  v0 = (int32_t)s1 >> 4;
  sb(PLAYER_DATA + 0x0E, v0);
  v0 = 8; // 0x0008
  sw(PLAYER_DATA + 0x58, v0);
  goto label8002FF78;
label8002FF2C:
  sw(0x80077058, v0);
  v0 = 4; // 0x0004
  sb(PLAYER_DATA + 0x24, v0);
  v0 = lw(0x80077098);
  v1 = 4; // 0x0004
  sw(0x8007706C, 0);
  sb(PLAYER_DATA + 0x19, 0);
  sb(PLAYER_DATA + 0x1F, 0);
  sw(PLAYER_DATA + 0x58, v1);
  v0 = (int32_t)v0 >> 4;
  sb(PLAYER_DATA + 0x0E, v0);
label8002FF78:
  v1 = lw(0x80077058);
  v0 = 1; // 0x0001
  temp = v1 != v0;
  a0 = sp + 16; // 0x0010
  if (temp) goto label80030258;
  a2 = lw(0x800770C0);
  a1 = PLAYER_POSITION;
  a2 += 12; // 0x000C
  spyro_vec3_add(a0, a1, a2);
  a0 = sp + 16; // 0x0010
  a1 = 1; // 0x0001
  spyro_vec3_shift_right(a0, a1);
  a0 = sp + 16; // 0x0010
  a2 = 0x80076DF8;
  a1 = a0;
  spyro_vec3_sub(a0, a1, a2);
  v1 = lw(0x8007706C);
  v0 = (int32_t)v1 < 16;
  temp = v0 == 0;
  a0 = sp + 16; // 0x0010
  if (temp) goto label800300D0;
  a1 = 0;
  sh(sp + 0x0020, 0);
  v0 = spyro_vec_length(a0, a1);
  a0 = v0;
  a1 = lw(sp + 0x0018);
  a2 = 1; // 0x0001
  a1 = -a1;
  v0 = spyro_atan2(a0, a1, a2);
  a0 = lw(sp + 0x0010);
  a1 = lw(sp + 0x0014);
  a2 = 1; // 0x0001
  sh(sp + 0x0022, v0);
  v0 = spyro_atan2(a0, a1, a2);
  a0 = lh(sp + 0x0020);
  a1 = lh(0x80076E1C);
  sh(sp + 0x0024, v0);
  v0 = spyro_two_angle_signed_diff_12bit(a0, a1);
  v1 = lw(0x8007706C);
  mult(v0, v1);
  a0 = lh(sp + 0x0022);
  a1 = lh(0x80076E1E);
  v0 = lhu(0x80076E1C);
  t0=lo;
  v1 = (int32_t)t0 >> 4;
  v0 += v1;
  v0 = v0 & 0xFFF;
  sh(0x80076E1C, v0);
  v0 = spyro_two_angle_signed_diff_12bit(a0, a1);
  v1 = lw(0x8007706C);
  mult(v0, v1);
  a0 = lh(sp + 0x0024);
  a1 = lh(0x80076E20);
  v0 = lhu(0x80076E1E);
  t0=lo;
  v1 = (int32_t)t0 >> 4;
  v0 += v1;
  v0 = v0 & 0xFFF;
  sh(0x80076E1E, v0);
  v0 = spyro_two_angle_signed_diff_12bit(a0, a1);
  v1 = lw(0x8007706C);
  mult(v0, v1);
  v0 = lhu(0x80076E20);
  t0=lo;
  v1 = (int32_t)t0 >> 4;
  goto label80030248;
label800300D0:
  v0 = lw(0x80077070);
  v0 -= 32; // 0xFFFFFFE0
  v0 = (int32_t)v1 < (int32_t)v0;
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label80030130;
  sh(0x80076E1C, 0);
  a1 = 0;
  v0 = spyro_vec_length(a0, a1);
  a0 = v0;
  a1 = lw(sp + 0x0018);
  a2 = 1; // 0x0001
  a1 = -a1;
  v0 = spyro_atan2(a0, a1, a2);
  a0 = lw(sp + 0x0010);
  a1 = lw(sp + 0x0014);
  sh(0x80076E1E, v0);
  a2 = 1; // 0x0001
  v0 = spyro_atan2(a0, a1, a2);
  goto label80030250;
label80030130:
  v0 = lw(0x800770C0);
  a0 = sp + 16; // 0x0010
  s0 = lw(v0);
  sh(sp + 0x0020, 0);
  v0 = spyro_vec_length(a0, a1);
  a0 = v0;
  a1 = lw(sp + 0x0018);
  a2 = 1; // 0x0001
  a1 = -a1;
  v0 = spyro_atan2(a0, a1, a2);
  a0 = lw(sp + 0x0010);
  a1 = lw(sp + 0x0014);
  a2 = 1; // 0x0001
  sh(sp + 0x0022, v0);
  v0 = spyro_atan2(a0, a1, a2);
  sh(sp + 0x0024, v0);
  a0 = lw(s0 + 0x000C);
  a1 = lh(sp + 0x0020);
  v0 = spyro_two_angle_signed_diff_12bit(a0, a1);
  v1 = lw(0x8007706C);
  a0 = lw(0x80077070);
  v1 -= a0;
  v1 += 32; // 0x0020
  mult(v0, v1);
  a1 = lh(sp + 0x0022);
  v0 = lhu(sp + 0x0020);
  t0=lo;
  v1 = (int32_t)t0 >> 6;
  v0 += v1;
  v0 = v0 & 0xFFF;
  sh(0x80076E1C, v0);
  a0 = lw(s0 + 0x0010);
  v0 = spyro_two_angle_signed_diff_12bit(a0, a1);
  v1 = lw(0x8007706C);
  a0 = lw(0x80077070);
  v1 -= a0;
  v1 += 32; // 0x0020
  mult(v0, v1);
  a1 = lh(sp + 0x0024);
  v0 = lhu(sp + 0x0022);
  t0=lo;
  v1 = (int32_t)t0 >> 6;
  v0 += v1;
  v0 = v0 & 0xFFF;
  sh(0x80076E1E, v0);
  a0 = lw(s0 + 0x0014);
  v0 = spyro_two_angle_signed_diff_12bit(a0, a1);
  v1 = lw(0x8007706C);
  a0 = lw(0x80077070);
  v1 -= a0;
  v1 += 32; // 0x0020
  mult(v0, v1);
  v0 = lhu(sp + 0x0024);
  t0=lo;
  v1 = (int32_t)t0 >> 6;
label80030248:
  v0 += v1;
  v0 = v0 & 0xFFF;
label80030250:
  sh(0x80076E20, v0);
label80030258:
  s1 = PLAYER_DATA + 0x0E;
  a0 = lbu(s1);
  a0 = a0 << 4;
  v0 = spyro_cos(a0);
  a0 = lbu(s1);
  s0 = v0;
  a0 = a0 << 4;
  v0 = spyro_sin(a0);
  v1 = lw(PLAYER_DATA + 0xA4);
  mult(v1, s0);
  a1=lo;
  v1 = lw(PLAYER_DATA + 0xA8);
  mult(v1, v0);
  a0 = lbu(s1);
  a0 = a0 << 4;
  v1=lo;
  v0 = a1 + v1;
  v0 = (int32_t)v0 >> 12;
  sw(sp + 0x0010, v0);
  s0 = spyro_cos(a0);
  v1 = lw(PLAYER_DATA + 0xA8);
  mult(v1, s0);
  a3=lo;
  v1 = lw(sp + 0x10);
  mult(v1, v1);
  a2=lo;
  v1 = lw(PLAYER_DATA + 0xAC);
  mult(v1, v1);
  a1=lo;
  mult(lw(PLAYER_DATA + 0xA4), spyro_sin(lbu(s1)*16));
  sw(sp + 0x18, v1);
  a0 = a2 + a1;
  v0 = a3 - lo;
  v0 = (int32_t)v0 >> 12;
  sw(sp + 0x14, v0);
  sb(PLAYER_DATA + 0x0C, -spyro_atan2(spyro_sqrt(a0), lw(sp + 0x14), 1));
  sb(PLAYER_DATA + 0x0D, -spyro_atan2(lw(sp + 0x18), lw(sp + 0x10), 1));
  a0 = lw(PLAYER_DATA + 0x58);
  function_8003CB24();
  function_80049660();
  function_80049E8C();
  v0 = lw(PLAYER_POSITION + 8);
  v0 += 1024; // 0x0400
  sw(PLAYER_POSITION + 8, v0);
  a0 = s1 - 14; // 0xFFFFFFF2
  function_8004DF24();
  sw(PLAYER_DATA + 0xA0, v0);
  v0 += 356; // 0x0164
  sw(PLAYER_POSITION + 8, v0);
  a0 = 0;
  function_80049FAC();
  goto label800313F8;
label800303D0:
  v1 = lw(0x800770C0);
  v0 = lbu(v1 + 0x0049);
  v0++;
  v0 = v0 & 3;
  sb(v1 + 0x0049, v0);
  v1 = lw(0x800770C0);
  v0 = lbu(v1 + 0x0049);
  v0 = v0 << 1;
  v0 = lbu(0x8006F398 + v0); // &0xFFFF0101
  sb(v1 + 0x0044, v0);
  v1 = lw(0x800770C0);
  v0 = lbu(v1 + 0x0049);
  v0 = v0 << 1;
  v0 = lbu(0x8006F399 + v0); // &0x01FFFF01
  sb(v1 + 0x0045, v0);
  v0 = lw(0x800757D0); // &0x00000000
  v0 = (int32_t)v0 < 8;
  temp = v0 == 0;
  v0 = 8; // 0x0008
  if (temp) goto label80030468;
  sw(0x800757D0, v0); // &0x00000000
label80030468:
  a1 = lw(0x800770C0);
  v0 = 70; // 0x0046
  sw(0x8007584C, v0);
  a0 = 251; // 0x00FB
  function_loaded_800758CC();
  v0 = lw(0x8007706C);
  v0 = (int32_t)v0 < 16;
  temp = v0 != 0;
  if (temp) goto label800304BC;
  v0 = lw(0x8007705C);
  v0 = (int32_t)v0 < 3;
  temp = v0 == 0;
  s0 = 0;
  if (temp) goto label800304E4;
label800304BC:
  a0 = lw(PLAYER_DATA + 0x58);
  function_8003CB24();
  function_80049660();
  function_80049E8C();
  goto label800313F8;
label800304E4:
  sw(0x80077058, 3);
  sw(0x8007706C, 0);
  a0 = 251; // 0x00FB
label800304FC:
  a1 = lw(0x800770C0);
  s0++;
  function_loaded_800758CC();
  v0 = (int32_t)s0 < 32;
  temp = v0 != 0;
  a0 = 251; // 0x00FB
  if (temp) goto label800304FC;
  s1 = 5; // 0x0005
  function_800524C4();
  a0 = v0;
  s0 = 0x800770B8;
  sw(s0, a0);
  new_game_object(addr_to_pointer(a0));
  v1 = lw(0x800770B8);
  v0 = 510; // 0x01FE
  sh(v1 + 0x0036, v0);
  a0 = lw(s0);
  a1 = lw(0x800770C0);
  a0 += 12; // 0x000C
  a1 += 12; // 0x000C
  spyro_vec3_copy(a0, a1);
  v0 = lw(0x800770C0);
  v1 = lw(0x800770B8);
  v0 = lbu(v0 + 0x0046);
  sb(v1 + 0x0046, v0);
  v1 = lw(0x800770B8);
  v0 = 96; // 0x0060
  sb(v1 + 0x0057, v0);
  v0 = lw(0x800770B8);
  sb(v0 + 0x0047, s1);
  v1 = lw(0x800770B8);
  v0 = -1;
  sw(v1 + 0x001C, v0);
  v1 = lw(0x800770B8);
  v0 = lw(v1 + 0x0014);
  v0 += 1024; // 0x0400
  sw(v1 + 0x0014, v0);
  a0 = lw(s0);
  a0 += 12; // 0x000C
  function_8004DF24();
  v1 = lw(0x800770B8);
  v0 = lw(v1 + 0x0014);
  v0 -= 1024; // 0xFFFFFC00
  sw(v1 + 0x0014, v0);
  a0 = lw(s0);
  function_800533D0();
  a0 = lw(s0);
  function_800526A8();
  function_800524C4();
  a0 = v0;
  sw(0x800770BC, a0);
  new_game_object(addr_to_pointer(a0));
  v1 = lw(0x800770BC);
  v0 = 511; // 0x01FF
  sh(v1 + 0x0036, v0);
  a0 = lw(0x800770BC);
  a1 = PLAYER_POSITION;
  a0 += 12; // 0x000C
  spyro_vec3_copy(a0, a1);
  v0 = lw(0x800770BC);
  sb(v0 + 0x0047, s1);
  a0 = lw(0x800770BC);
  function_800526A8();
  a0 = lw(s0 - 0x0034); // 0xFFFFFFCC
  a1 = 4; // 0x0004
  function_80056200();
  a1 = 0;
  a2 = 16; // 0x0010
  v0 = lw(0x800761D4);
  s0 -= 52; // 0xFFFFFFCC
  a0 = lbu(v0 + 0x0031);
  a3 = s0;
  function_80055A78();
  v1 = lw(0x800770C0);
  v0 = lw(v1 + 0x0014);
  v0 += 120; // 0x0078
  sw(v1 + 0x0014, v0);
  a0 = lw(0x800770C0);
  function_80052568();
  goto label800313F8;
label800306D4:
  s0 = 0x8007706C;
  a0 = lw(s0);
  v0 = (int32_t)a0 < 24;
  temp = v0 == 0;
  v1 = a0 << 1;
  if (temp) goto label80030778;
  v1 += a0;
  v0 = 96; // 0x0060
  a0 = lw(0x800770B8);
  v0 -= v1;
  sb(a0 + 0x0057, v0);
  v1 = lw(0x800770C0);
  v0 = lw(v1);
  a0 = lw(v0 + 0x0030);
  a1 = lbu(v1 + 0x0046);
  a0 = (int32_t)a0 >> 4;
  v0 = spyro_two_angle_signed_diff_8bit(a0, a1);
  v1 = lw(s0);
  mult(v0, v1);
  v0=lo;
  v1 = 0x2AAAAAAB;
  mult(v0, v1);
  a0 = lw(0x800770C0);
  v0 = (int32_t)v0 >> 31;
  t0=hi;
  v1 = (int32_t)t0 >> 2;
  v1 -= v0;
  v0 = lbu(a0 + 0x0046);
  a0 = lw(0x800770B8);
  v0 += v1;
  sb(a0 + 0x0046, v0);
  goto label800313F8;
label80030778:
  v1 = lw(0x800770C0);
  v0 = 4; // 0x0004
  sw(0x80077058, v0);
  sw(s0, 0);
  v0 = lw(v1);
  v0 = lw(v0 + 0x0030);
  v1 = lw(0x800770B8);
  v0 = (int32_t)v0 >> 4;
  sb(v1 + 0x0046, v0);
  v0 = lw(0x800770B8);
  sb(v0 + 0x0057, 0);
  v1 = lw(0x800770BC);
  v0 = lbu(PLAYER_DATA + 0x0C);
  sb(v1 + 0x0044, v0);
  v1 = lw(0x800770BC);
  v0 = lbu(PLAYER_DATA + 0x0D);
  sb(v1 + 0x0045, v0);
  v1 = lw(0x800770BC);
  v0 = lbu(PLAYER_DATA + 0x0E);
  sb(v1 + 0x0046, v0);
  goto label800313F8;
label80030804:
  s3 = 0x8007706C;
  v0 = lw(0x8007707C);
  v1 = lw(s3);
  v0 = v0 << 1;
  v0 = (int32_t)v1 < (int32_t)v0;
  temp = v0 == 0;
  s4 = 0;
  if (temp) goto label80030D2C;
  v0 = lw(0x80077088);
  temp = v0 != 0;
  if (temp) goto label8003097C;
  v0 = lw(0x800770C0);
  v0 = lw(v0);
  v0 = lw(v0 + 0x0040);
  v0 = (int32_t)v1 < (int32_t)v0;
  temp = v0 != 0;
  s2 = sp + 40; // 0x0028
  if (temp) goto label8003097C;
  a0 = s2;
  s1 = 0x800761D0;
  a1 = lw(s1);
  a2 = 20; // 0x0014
  spyro_memcpy32(a0, a1, a2);
  v0 = 0x00080000;
  v1 = lw(0x80077038);
  a0 = lw(0x800761D0);
  v0 -= v1;
  sw(a0, v0);
  v1 = lw(0x800761D0);
  v0 = -1;
  sw(v1 + 0x0004, v0);
  v1 = lw(0x800761D0);
  v0 = 50; // 0x0032
  sh(v1 + 0x0008, v0);
  v0 = lw(0x800770C0);
  v0 = lw(v0);
  v1 = 0x2F8D0000;
  v0 = lw(v0 + 0x003C);
  v1 = v1 | 0xF18F;
  v0 = v0 << 10;
  mult(v0, v1);
  a0 = lw(0x800761D0);
  v0 = (int32_t)v0 >> 31;
  t0=hi;
  v1 = (int32_t)t0 >> 11;
  v1 -= v0;
  sh(a0 + 0x0A, v1);
  v0 = lw(0x800761D0);
  sh(v0 + 0x0C, 0);
  v0 = lw(0x800761D0);
  sh(v0 + 0x0E, 0);
  v0 = lw(0x800761D0);
  s0 = 1; // 0x0001
  sw(v0 + 0x0010, 0);
  a0 = lw(s3 + 0x18);
  sw(0x800761DC, s0);
  sh(0x800761E8, 0x3FFF);
  sh(0x800761EA, 0x3FFF);
  a1 = 4; // 0x0004
  function_80056200();
  a0 = 0;
  a1 = 0;
  a2 = 16; // 0x0010
  a3 = s3 + 24; // 0x0018
  function_80055A78();
  a1 = s2;
  a0 = lw(s1);
  a2 = 20; // 0x0014
  spyro_memcpy32(a0, a1, a2);
  sw(0x80077088, s0);
label8003097C:
  v0 = lw(0x8007706C);
  v0 = (int32_t)v0 >> 1;
  v1 = v0 << 1;
  v1 += v0;
  v0 = lw(0x80077078);
  v1 = v1 << 3;
  v1 += v0;
  v0 = lw(v1);
  sw(0x80076DF8, v0);
  v0 = lw(v1 + 0x0004);
  sw(0x80076DFC, v0);
  v0 = lw(v1 + 0x0008);
  sw(0x80076E00, v0);
  v0 = lw(v1 + 0x000C);
  sh(0x80076E1C, v0);
  v0 = lw(v1 + 0x0010);
  sh(0x80076E1E, v0);
  v0 = lw(v1 + 0x0014);
  v1 = lw(0x80076B70);
  sh(0x80076E20, v0);
  v0 = lw(v1 + 0x0038);
  v1 = lbu(v0 + 0x000C);
  v0 = 16; // 0x0010
  temp = v1 != v0;
  v0 = 21; // 0x0015
  if (temp) goto label80030A14;
  a1 = 8; // 0x0008
  goto label80030A30;
label80030A14:
  temp = v1 != v0;
  v0 = 32; // 0x0020
  if (temp) goto label80030A24;
  a1 = 6; // 0x0006
  goto label80030A30;
label80030A24:
  temp = v1 != v0;
  a1 = 2; // 0x0002
  if (temp) goto label80030A30;
  a1 = 4; // 0x0004
label80030A30:
  a0 = 0x8007706C;
  v0 = lw(a0);
  div_psx(v0,a1);
  v0=lo;
  v1 = lw(0x800770B8);
  sb(v1 + 0x003E, v0);
  v1 = lw(0x800770B8);
  v0 = lbu(v1 + 0x003E);
  v0++;
  sb(v1 + 0x003F, v0);
  v0 = lw(a0);
  div_psx(v0,a1);
  v1=hi;
  v1 = v1 << 6;
  div_psx(v1,a1);
  v1=lo;
  v0 = lw(0x800770B8);
  sb(v0 + 0x0040, v1);
  v0 = lw(0x80076B70);
  v0 = lw(v0 + 0x0038);
  a1 = lw(0x800770B8);
  v1 = lh(v0);
  v0 = lbu(a1 + 0x003F);
  v0 = (int32_t)v0 < (int32_t)v1;
  temp = v0 != 0;
  a0 = v1;
  if (temp) goto label80030B58;
  v0 = a0 - 1;
  sb(a1 + 0x003E, v0);
  v0 = lw(0x800770B8);
  sb(v0 + 0x0040, 0);
label80030B58:
  v0 = lw(0x80076B74);
  v0 = lw(v0 + 0x0038);
  v1 = lbu(v0 + 0x000C);
  v0 = 16; // 0x0010
  temp = v1 != v0;
  v0 = 21; // 0x0015
  if (temp) goto label80030B84;
  a1 = 8; // 0x0008
  goto label80030BA0;
label80030B84:
  temp = v1 != v0;
  v0 = 32; // 0x0020
  if (temp) goto label80030B94;
  a1 = 6; // 0x0006
  goto label80030BA0;
label80030B94:
  temp = v1 != v0;
  a1 = 2; // 0x0002
  if (temp) goto label80030BA0;
  a1 = 4; // 0x0004
label80030BA0:
  a0 = 0x8007706C;
  v0 = lw(a0);
  div_psx(v0,a1);
  v0=lo;
  v1 = lw(0x800770BC);
  sb(v1 + 0x003E, v0);
  v1 = lw(0x800770BC);
  v0 = lbu(v1 + 0x003E);
  v0++;
  sb(v1 + 0x003F, v0);
  v0 = lw(a0);
  div_psx(v0,a1);
  v1 = hi << 6;
  div_psx(v1,a1);
  v1=lo;
  v0 = lw(0x800770BC);
  sb(v0 + 0x0040, v1);
  v0 = lw(0x80076B74);
  v0 = lw(v0 + 0x0038);
  a1 = lw(0x800770BC);
  v1 = lh(v0);
  v0 = lbu(a1 + 0x003F);
  v0 = (int32_t)v0 < (int32_t)v1;
  temp = v0 != 0;
  a0 = v1;
  if (temp) goto label80030CC8;
  v0 = a0 - 1;
  sb(a1 + 0x003E, v0);
  v0 = lw(0x800770BC);
  sb(v0 + 0x0040, 0);
label80030CC8:
  v0 = lw(BUTTONS);
  v0 = v0 & 0x40;
  temp = v0 == 0;
  if (temp) goto label80030D2C;
  v0 = lw(0x800770C0);
  v0 = lw(v0);
  v0 = lw(v0 + 0x0034);
  temp = v0 == 0;
  if (temp) goto label80030D28;
  v0 = lw(0x8007708C);
  temp = v0 != 0;
  v0 = 1; // 0x0001
  if (temp) goto label80030D2C;
  sw(0x8007708C, v0);
  goto label80030D2C;
label80030D28:
  s4 = 1; // 0x0001
label80030D2C:
  temp = s4 != 0;
  if (temp) goto label80030D54;
  v0 = lw(0x8007707C);
  v1 = lw(0x8007706C);
  v0 = v0 << 1;
  v1 = (int32_t)v1 < (int32_t)v0;
  temp = v1 != 0;
  if (temp) goto label80030EF0;
label80030D54:
  v1 = lw(0x8007707C);
  v0 = 5; // 0x0005
  sw(0x80077058, v0);
  sw(0x8007706C, 0);
  v0 = v1 << 1;
  v0 += v1;
  v1 = lw(0x80077078);
  v0 = v0 << 3;
  v0 += v1;
  v1 = lw(v0 - 0x0018); // 0xFFFFFFE8
  sw(0x80076DF8, v1);
  v1 = lw(v0 - 0x0014); // 0xFFFFFFEC
  sw(0x80076DFC, v1);
  v1 = lw(v0 - 0x0010); // 0xFFFFFFF0
  sw(0x80076E00, v1);
  v1 = lw(v0 - 0x000C); // 0xFFFFFFF4
  sh(0x80076E1C, v1);
  v1 = lw(v0 - 0x0008); // 0xFFFFFFF8
  sh(0x80076E1E, v1);
  v0 = lw(v0 - 0x0004);
  v1 = lw(0x80076B70);
  sh(0x80076E20, v0);
  v0 = lw(v1 + 0x0038);
  v0 = lbu(v0);
  v1 = lw(0x800770B8);
  v0--;
  sb(v1 + 0x003E, v0);
  v0 = lw(0x800770B8);
  sb(v0 + 0x0040, 0);
  v0 = lw(0x80076B74);
  v0 = lw(v0 + 0x0038);
  v0 = lbu(v0);
  v1 = lw(0x800770BC);
  v0--;
  sb(v1 + 0x003E, v0);
  v0 = lw(0x800770BC);
  sb(v0 + 0x0040, 0);
  v0 = 1; // 0x0001
  sb(PLAYER_DATA + 0x18, 0);
  sb(PLAYER_DATA + 0x19, 0);
  sb(PLAYER_DATA + 0x1E, 0);
  sb(PLAYER_DATA + 0x1F, v0);
  v0 = 4; // 0x0004
  sb(PLAYER_DATA + 0x24, 0);
  sw(PLAYER_DATA + 0x58, v0);
  function_80049660();
  function_80049E8C();
  a0 = lw(0x80077084);
  a1 = 2; // 0x0002
  function_80056200();
  function_80056ED4();
  v0 = lw(0x80077064);
  temp = v0 != 0;
  a0 = 0x00080000;
  if (temp) goto label80030EF0;
  v0 = lw(0x80077038);
  a0 -= v0;
  function_8005CB24();
  v0 = lw(0x800785E0);
  a0 = lw(0x80077034);
  a1 = lw(0x80077038);
  a0 += v0;
  function_8005CAC4();
label80030EE0:
  a0 = 0;
  v0 = test_spu_event(a0);
  temp = v0 == 0;
  if (temp) goto label80030EE0;
label80030EF0:
  a0 = 0x8007708C;
  v1 = lw(a0);
  temp = v1 == 0;
  if (temp) goto label800313F8;
  v0 = lw(0x800756CC); // &0x00000000
  v0 += v1;
  sw(a0, v0);
  v0 = (int32_t)v0 < 120;
  temp = v0 != 0;
  if (temp) goto label800313F8;
  sw(a0, 0);
  goto label800313F8;
label80030F30:
  v1 = lw(0x80076B70);
  v0 = lw(v1 + 0x0038);
  v0 = lh(v0 + 0x0002);
  temp = (int32_t)v0 <= 0;
  s1 = 0;
  if (temp) goto label80030FC4;
label80030F54:
  a1 = 0x00FF0000;
  v0 = lw(v1 + 0x0038);
  a1 = a1 | 0xFFFF;
  v0 = lw(v0 + 0x0018);
  s0 = s1 << 2;
  v0 += s0;
  a0 = lw(v0);
  a2 = 512; // 0x0200
  v0 = interpolate_color(a0, a1, a2);
  v1 = lw(0x80076B70);
  v1 = lw(v1 + 0x0038);
  v1 = lw(v1 + 0x0018);
  s0 += v1;
  sw(s0, v0);
  v1 = lw(0x80076B70);
  v0 = lw(v1 + 0x0038);
  v0 = lh(v0 + 0x0002);
  s1++;
  v0 = (int32_t)s1 < (int32_t)v0;
  temp = v0 != 0;
  if (temp) goto label80030F54;
label80030FC4:
  a0 = lw(PLAYER_DATA + 0x58);
  s1 = 0;
  s4 = 0x8006E77C; // &0x00050002
  s0 = 0x80077FEC + 0x01C8;
  s3 = 0;
  s2 = 0;
  function_8003CB24();
  function_80049660();
  function_80049E8C();
label80030FFC:
  a0 = 0x80077FEC + 0x01C4;
  a0 += s3;
  a1 = 0x8006F3F0; // &0x00000100
  a1 += s2;
  spyro_vec3_copy(a0, a1);
  v1 = lw(0x8007706C);
  v0 = (int32_t)v1 < 25;
  temp = v0 == 0;
  v0 = (int32_t)v1 >> 1;
  if (temp) goto label8003105C;
  v0 = v0 << 1;
  v0 += s4;
  v1 = lh(v0);
  v0 = v1 << 1;
  v0 += v1;
  v1 = lw(s0);
  v0 = (int32_t)v0 >> 1;
  v1 += v0;
  sw(s0, v1);
  goto label80031080;
label8003105C:
  v0 = lh(0x8006E794); // &0x00000024
  v1 = v0 << 1;
  v1 += v0;
  v0 = lw(s0);
  v1 = (int32_t)v1 >> 1;
  v0 += v1;
  sw(s0, v0);
label80031080:
  s0 += 88; // 0x0058
  s3 += 88; // 0x0058
  s1++;
  v0 = (int32_t)s1 < 3;
  temp = v0 != 0;
  s2 += 12; // 0x000C
  if (temp) goto label80030FFC;
  s1 = 0x8007706C;
  v0 = lw(s1);
  v0 = (int32_t)v0 < 25;
  temp = v0 != 0;
  if (temp) goto label800313F8;
  s0 = sp + 32; // 0x0020
  a0 = s0;
  sw(0x80077058, 6);
  sw(s1, 0);
  sb(0x80077FAE, 0);
  
  spyro_vec3_copy(a0, lw(0x800770B8) + 0x0C);
  a0 = 0x20;
  a1 = 0x18;
  a2 = s0;
  a3 = 0;
  sw(sp + 0x28, lw(sp + 0x28) + 0x40);
  function_loaded_800758E4();
  a0 = 0x8007624C;
  a1 = s0;
  spyro_vec3_copy(a0, a1);
  a0 = lw(0x800770B8);
  function_80052568();
  a0 = lw(0x800770BC);
  function_80052568();
  a1 = 0;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x0034);
  a3 = s1 + 24; // 0x0018
  function_80055A78();
  goto label800313F8;
label8003115C:
  a0 = lw(PLAYER_DATA + 0x58);
  function_8003CB24();
  function_80049660();
  function_80049E8C();
  s0 = 0x80077FCC;
  a2 = lw(s0);
  v0 = lw(TOTAL_FOUND_DRAGONS);
  temp = a2 != v0;
  a0 = 5; // 0x0005
  if (temp) goto label800311B0;
  v0 = lbu(0x80077FAE);
  temp = v0 == 0;
  if (temp) goto label80031238;
label800311B0:
  a1 = 2; // 0x0002
  a2++;
  v0 = lbu(0x80077FAE);
  v0 += 16; // 0x0010
  sb(0x80077FAE, v0);
  a3 = 0;
  function_800542E4();
  a0 = 5; // 0x0005
  a2 = lbu(0x80077FAE);
  a1 = 2; // 0x0002
  function_800544A8();
  v1 = lbu(0x80077FAE);
  v0 = 192; // 0x00C0
  temp = v1 != v0;
  if (temp) goto label80031210;
  v0 = lw(s0);
  v0++;
  sw(s0, v0);
label80031210:
  temp = v1 != 0;
  a1 = 0;
  if (temp) goto label80031238;
  v0 = lw(0x800761D4);
  a0 = lbu(v0 + 0x0035);
  a3 = 0x80077084;
  a2 = 16; // 0x0010
  function_80055A78();
label80031238:
  a3 = 0x8007706C;
  v0 = lw(a3);
  v0 = (int32_t)v0 < 96;
  temp = v0 != 0;
  v0 = 6; // 0x0006
  if (temp) goto label800313F8;
  v1 = lw(0x8007705C);
  temp = v1 != v0;
  a1 = 0;
  if (temp) goto label800313F8;
  v1 = lw(0x80075824);
  v0 = 7; // 0x0007
  sw(0x80077058, v0);
  v0 = 255; // 0x00FF
  sw(a3, 0);
  sw(0x80075738, v1); // &0x00000000
  sb(v1 + 0x0001, v0);
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x0033);
label800312A0:
  a3 = 0x80077084;
  function_80055A78();
  goto label800313F8;
label800312B8:
  a0 = lw(PLAYER_DATA + 0x58);
  function_8003CB24();
  function_80049660();
  function_80049E8C();
  v0 = lw(0x8007706C);
  v0 = (int32_t)v0 < 16;
  temp = v0 == 0;
  if (temp) goto label80031374;
  s3 = 0;
  s0 = 0x80077FEC + 0x01C8;
  s2 = 0;
  s1 = 0;
label80031304:
  a0 = 0x80077FEC + 0x01C4;
  a0 += s2;
  a1 = 0x8006F3F0; // &0x00000100
  a1 += s1;
  spyro_vec3_copy(a0, a1);
  s2 += 88; // 0x0058
  s1 += 12; // 0x000C
  s3++;
  a0 = 17; // 0x0011
  v0 = lw(0x8007706C);
  v1 = lw(s0);
  a0 -= v0;
  v0 = a0 << 1;
  v0 += a0;
  v1 += v0;
  sw(s0, v1);
  v0 = (int32_t)s3 < 3;
  temp = v0 != 0;
  s0 += 88; // 0x0058
  if (temp) goto label80031304;
  v1 = lw(0x8007706C);
  v0 = (int32_t)v1 < 16;
  temp = v0 != 0;
  v0 = v1 << 4;
  if (temp) goto label80031388;
label80031374:
  v1 = lw(0x8007706C);
  v0 = 32; // 0x0020
  v1 = v0 - v1;
  v0 = v1 << 4;
label80031388:
  v0--;
  a0 = 0x80076248;
  sw(0x80077074, v0);
  v0 = 1; // 0x0001
  sw(a0, v0);
  v0 = v1 << 3;
  v0 += 64; // 0x0040
  sb(0x80076273, v0);
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 4;
  sw(0x8007626C, v0);
  v0 = lbu(0x80076272);
  v1 = lw(0x8007706C);
  v0 += 8; // 0x0008
  v1 = (int32_t)v1 < 32;
  sb(0x80076272, v0);
  temp = v1 != 0;
  if (temp) goto label800313F8;
  sw(a0, 0);
  function_8002CB6C();
label800313F8:
  a0 = lw(0x800756CC); // &0x00000000
  function_loaded_800756BC();
  v0 = lw(0x80077058);
  temp = (int32_t)v0 <= 0;
  if (temp) goto label80031430;
  function_80014B70();
label80031430:
  s1 = lw(0x80075828);
  a0 = lbu(s1 + 0x48);
  v1 = a0 & 0xFF;
  temp = v1 == 0xFF;
  s3 = 251; // 0xFB
  if (temp) goto label80031490;
  s2 = 255; // 0x00FF
  s0 = s1 + 0x48;
label80031458:
  if (lh(s1 + 0x36) == 0xFB && (a0 & 0xFF) < 128) {
    a0 = s1;
    function_8003C6E4();
  }
  s0 += 88;
  s1 += 88;
  a0 = lbu(s1 + 0x48);
  temp = a0 != s2;
  if (temp) goto label80031458;
label80031490:
  ra = lw(sp + 0x6C);
  s4 = lw(sp + 0x68);
  s3 = lw(sp + 0x64);
  s2 = lw(sp + 0x60);
  s1 = lw(sp + 0x5C);
  s0 = lw(sp + 0x58);
  sp += 112; // 0x0070
  return;
}

// size: 0x000006A4
void function_80032B08(void)
{
  uint32_t temp;
  sp -= 0x38;
  sw(sp + 0x34, ra);
  sw(sp + 0x30, s2);
  sw(sp + 0x2C, s1);
  sw(sp + 0x28, s0);

  v1 = lw(0x80078D78);

  temp = v1 != 3;
  if (temp) goto label80033170;
  v1 = lw(0x80078D78 + 0x04);
  temp = v1 != 0;
  v0 = 0x1;
  if (temp) goto label80032CD0;
  a0 = 0;
  function_80056B28();
  function_80056ED4();
  v1 = lw(0x80078D78 + 0x1C);
  sw(0x80078D78 + 0x04, 1);
  
  if (v1 == 0) {
    sw(0x8007566C, 1);
    sw(0x80075864, 0);
    sw(0x800785A8 + 0x30, lw(EXE_END_PTR));
    sw(LIB_END_PTR2, lw(EXE_END_PTR));
  } else if (v1 == 1) {
    sw(0x80075690, 0);
    sw(0x80075864, 1);
    sw(0x800758AC, 0);
    sw(0x800756D0, 0);
    sw(0x800758B4, lw(LEVEL_ID));
  } else if (v1 == 2) {
    function_8001277C();
    sw(0x80075690, 0);
    sw(0x80075864, 1);
    sw(0x800758AC, 0);
    sw(0x800756D0, 0);
    sw(LEVEL_ID, lw(0x8006EE7C + lw(0x800757B8)*4));
    sw(0x800758B4, lw(LEVEL_ID));
  }
  sh(0x80076E20, 0x400);
  sh(0x80076E1C, 0);
  sh(0x80076E1E, 0);
  sw(0x80076DF8, 0x2000);
  sw(0x80076DFC, 0);
  sw(0x80076E00, 0x2000);
  a0 = 1;
  function_8004AC24();
  sw(PLAYER_POSITION + 0, 0x2780);
  sw(PLAYER_POSITION + 4, 0xF00);
  sw(PLAYER_POSITION + 8, 0x29A0);
  sb(PLAYER_DATA + 0x18, 0x10);
  sb(PLAYER_DATA + 0x19, 0x10);
  sb(PLAYER_DATA + 0x0C, 0);
  sb(PLAYER_DATA + 0x0D, 0);
  sb(PLAYER_DATA + 0x0E, 0);
  sb(PLAYER_DATA + 0x1F, 1);
  goto label80033170;
label80032CD0:
  temp = v1 != 1;
  v0 = 2;
  if (temp) goto label80032D74;
  v0 = lw(0x80078D78 + 0x1C);
  temp = v0 != 0;
  if (temp) goto label80032D44;
  v0 = lw(0x80075864);
  v0 = (int32_t)v0 < 0x3;
  temp = v0 == 0;
  v0 = 0x2;
  if (temp) goto label80032D5C;
label80032D04:
  function_80014564();
  function_8002BBE0();
  v0 = lw(0x80075864);
  v0 = (int32_t)v0 < 0x3;
  temp = v0 != 0;
  v0 = 0x2;
  if (temp) goto label80032D04;
  goto label80032D5C;
label80032D34:
  a0 = 0x1;
  function_80015370();
  function_8002BBE0();
label80032D44:
  v0 = lw(0x80075864);
  v0 = (int32_t)v0 < 0x6;
  temp = v0 != 0;
  v0 = 0x2;
  if (temp) goto label80032D34;
label80032D5C:
  sw(0x80078D78 + 0x04, v0);
  sw(0x80078D78 + 0x8, 0);
  goto label80033170;
label80032D74:
  temp = v1 != v0;
  a0 = sp + 0x10;
  if (temp) goto label80033170;
  s2 = PLAYER_POSITION;
  spyro_vec3_copy(a0, PLAYER_POSITION);
  a0 = lw(0x80078D78 + 0x8);
  v0 = (int32_t)a0 < 0x50;
  temp = v0 == 0;
  if (temp) goto label80032E44;
  a0 = -a0;
  a0 = a0 << 10;
  mult(a0, 0x66666667);
  a0 = (int32_t)a0 >> 31;
  t0=hi;
  v0 = (int32_t)t0 >> 5;
  a0 = v0 - a0;
  v0 = spyro_cos(a0);
  a0 = lw(0x80078D78 + 0x8);
  a0 = -a0;
  a0 = a0 << 10;
  mult(a0, 0x66666667);
  v0 += 0x1780;
  sw(s2, v0);
  a0 = (int32_t)a0 >> 31;
  t0=hi;
  v0 = (int32_t)t0 >> 5;
  a0 = v0 - a0;
  v0 = spyro_sin(a0);
  a0 = lw(0x80078D78 + 0x8);
  a0 = -a0;
  a0 = a0 << 10;
  mult(a0, 0x66666667);
  v0 = (int32_t)v0 >> 1;
  v0 += 0xE80;
  sw(PLAYER_POSITION + 0x4, v0);
  a0 = (int32_t)a0 >> 31;
  t0=hi;
  v0 = (int32_t)t0 >> 5;
  a0 = v0 - a0;
  v0 = spyro_sin(a0);
  v0 = (int32_t)v0 >> 1;
  v0 += 0x2980;
  goto label80032F7C;
label80032E44:
  v0 = (int32_t)a0 < 0x78;
  temp = v0 == 0;
  v0 = 0x1400;
  if (temp) goto label80032E68;
  sw(s2, v0);
  v0 = 0xC00;
  sw(PLAYER_POSITION + 4, v0);
  v0 = 0x20C0;
  goto label80032F7C;
label80032E68:
  v0 = (int32_t)a0 < 0xC8;
  temp = v0 == 0;
  v0 = (int32_t)a0 < 0xF0;
  if (temp) goto label80032E88;
  v0 = lw(s2);
  v0 += 0x4C;
  sw(s2, v0);
  goto label80032F84;
label80032E88:
  temp = v0 == 0;
  v0 = 0x2880;
  if (temp) goto label80032EA8;
  sw(s2, v0);
  v0 = 0x680;
  sw(PLAYER_POSITION + 0x4, v0);
  v0 = 0x2180;
  goto label80032F7C;
label80032EA8:
  v0 = (int32_t)a0 < 0x140;
  temp = v0 == 0;
  if (temp) goto label80032F64;
  v0 = a0 - 0xF0;
  v0 = v0 << 10;
  mult(v0, 0x66666667);
  v0 = (int32_t)v0 >> 31;
  s1 = 0xC00;
  t0=hi;
  a0 = (int32_t)t0 >> 5;
  a0 -= v0;
  a0 = s1 - a0;
  v0 = spyro_cos(a0);
  v1 = lw(0x80078D78 + 0x8);
  v1 -= 0xF0;
  v1 = v1 << 10;
  mult(v1, 0x66666667);
  v0 += 0x2880;
  sw(s2, v0);
  v1 = (int32_t)v1 >> 31;
  t0=hi;
  a0 = (int32_t)t0 >> 5;
  a0 -= v1;
  a0 = s1 - a0;
  v0 = spyro_sin(a0);
  v1 = lw(0x80078D78 + 0x8);
  v1 -= 0xF0;
  v1 = v1 << 10;
  mult(v1, 0x66666667);
  v0 = (int32_t)v0 >> 1;
  v0 += 0xE80;
  sw(PLAYER_POSITION + 0x4, v0);
  v1 = (int32_t)v1 >> 31;
  t0=hi;
  a0 = (int32_t)t0 >> 5;
  a0 -= v1;
  a0 += 0xC00;
  v0 = spyro_sin(a0);
  v0 = (int32_t)v0 >> 1;
  v0 += 0x2980;
  goto label80032F7C;
label80032F64:
  sw(s2, 0x2000);
  sw(PLAYER_POSITION + 0x4, 0x1000);
  v0 = 0x4000;
label80032F7C:
  sw(PLAYER_POSITION + 0x8, v0);
label80032F84:
  spyro_vec3_sub(sp + 0x10, PLAYER_POSITION, sp + 0x10);
  sb(PLAYER_DATA + 0x0D, spyro_atan2(spyro_vec_length(sp + 0x10, 0), lw(sp + 0x18), 0));
  sb(PLAYER_DATA + 0x0E, spyro_atan2(lw(sp + 0x10), lw(sp + 0x14), 0));
  a0 = 3;
  function_8003CB24();
  function_80049660();
  function_80049E8C();
  v0 = lw(0x80078D78 + 0x1C);
  temp = v0 != 0;
  if (temp) goto label800330EC;
  
  if ((int32_t)lw(0x80075864) < 7)
    function_80014564();
  
  if ((int32_t)lw(0x80078D78 + 0x8) < 0x180) goto label80033170;
  if (lw(0x80075864) != 7) goto label80033170;
  sh(sp + 0x20, 0);
  sh(sp + 0x22, 0);
  sh(sp + 0x24, 0x200);
  sh(sp + 0x26, 0x1E0);
  ClearImage(addr_to_pointer(sp + 0x20), 0, 0, 0);
  DrawSync(0);
  v0 = 0x80200000 - lw(0x800755A4);
  sw(0x800785FC, v0);
  a0 = 0x1;
  function_8005B6F8();
  while ((int32_t)lw(0x80075864) < 10) {
    function_80014564();
    function_8002BBE0();
  }
  function_8002D338();
  sw(0x8007579C, 1);
  goto label80033190;
label800330EC:
  if ((int32_t)lw(0x80075864) < 0xD) {
    a0 = 1;
    function_80015370();
  }
  v0 = lw(0x80078D78 + 0x8);
  v0 = (int32_t)v0 < 0x180;
  temp = v0 != 0;
  v0 = 0xD;
  if (temp) goto label80033170;
  v1 = lw(0x80075864);
  temp = v1 != v0;
  v0 = 0x2;
  if (temp) goto label80033170;
  v1 = lw(0x80078D78 + 0x1C);
  temp = v1 != v0;
  v0 = 0x1;
  if (temp) goto label80033158;
  sw(IS_DEMO_MODE, v0);
  sw(0x80075884, 0);
label80033158:
  a0 = 0x1;
  function_8004AC24();
  a0 = 0x1;
  function_80015370();
  goto label80033190;
label80033170:
  a0 = 3;
  function_80058CC0();
  v1 = 0x80078D78 + 0x8;
  sw(v1, lw(v1)+1);
label80033190:
  ra = lw(sp + 0x34);
  s2 = lw(sp + 0x30);
  s1 = lw(sp + 0x2C);
  s0 = lw(sp + 0x28);
  sp += 0x38;
  return;
}

// only used in function function_8007ABAC_title_screen
// size: 0x00000058
// press triangle to go back
void function_80032AB0(void)
{
  v0 = 0;
  if (lw(BUTTONS) & BUTTONS_MASK_TRIANGLE) {
    sw(0x80078D78 + 0x00, 1);
    sw(0x80078D78 + 0x04, 0);
    sw(0x80078D78 + 0x10, 0);

    a0 = 0;
    a1 = 0x80078D78 + 0x40;
    a2 = 0x80078D78 + 0x44;
    function_80067628();
  }
}

// size: 0x0000218C
void function_8007ABAC_title_screen(void)
{
  uint32_t temp;
  sp -= 104;
  sw(sp + 0x50, s0);
  sw(sp + 0x54, s1);
  sw(sp + 0x60, s4);
  sw(sp + 0x64, ra);
  if ((int32_t)lw(0x80078D78) < 2)
    s4 = lw(0x80078D78 + 0x10);
  else
    s4 = lw(0x80078D78 + 0x04);

  a0 = lw(0x80075680);
  sw(a0, lw(a0) + 2);
  if ((int32_t)lw(a0) < 1100) {
    v1 = lw(0x80078D78 + 0x10);
    sw(0x80078D78 + 0x08, 0);
    if (v1 == 2 && (int32_t)lw(a0) >= 300 && (lw(BUTTONS + 0x08) & (BUTTONS_MASK_CROSS | BUTTONS_MASK_START)))  {
      a0 = lw(0x80077084);
      a1 = 4;
      function_80056200();
      sw(lw(0x80075680), 1170);
    }
  } else if (lw(a0) == 1344 && spyro_rand() & 3) {
    sw(lw(0x80075680), 1172);
  } else {
    v1 = lw(0x80075680);
    if ((int32_t)lw(v1) >= 1530)
      sw(v1, 1172);
  }
  
  sw(0x80075918, max_int(lw(0x80075918)-1, 0));

  a0 = lw(lw(0x80075680));
  s1 = 1;
  function_8002BFE0();
  s0 = 0x80078D78;
  sh(0x80076E1C, 0);
  sh(0x80076E1E, lhu(0x80076E1E) + 0x10);
  v1 = lw(0x80078D78 + 0x00);
  if (v1 == 0) goto label8007AD64;
  if (v1 == 1) goto label8007B0B8;
  if (v1 == 2) goto label8007C454;
  goto label8007CC48;
label8007AD64:
  switch (lw(0x80078D78 + 0x10)) {
  case 0:
  case 1:
    function_800163E4();
    if (lw(0x80076BB8) || CdSync(1, NULL) != CdlComplete || (lw(0x800774B4) & 0x40) == 0) break;
    if (lw(0x80078D78 + 0x10) == 0) {
      read_disk2(
        lw(WAD_SECTOR),
        lw(0x800785E8) - 0x40000,
        0x40000,
        lw(WAD_HEADER + 8),
        0x258
      );
      sw(0x80078D78 + 0x10, 1);
    } else if (lw(0x80078D78 + 0x10) == 1) {
      sw(0x800774B0, 0x14);
      sw(0x800776CC, 0x09450C00);
      a0 = 0x14;
      a1 = 1;
      function_800567F4();

      sh(sp + 0x18, 0x200);
      sh(sp + 0x1A, 0x100);
      sh(sp + 0x1C, 0x200);
      sh(sp + 0x1E, 0x100);
      LoadImage(addr_to_pointer(sp + 0x18), addr_to_pointer(lw(0x800785E8) - 0x40000));
      sw(0x80078D78 + 0x10, 2);
    }
    break;
  case 2:
    if ((int32_t)lw(lw(0x80075680)) < 1169) {
      sw(0x80078D78 + 0x0C, 0);
    } else {
      if (lw(0x80078D78 + 0x0C) == 1) {
        a0 = 6;
        function_8007AAD4_title_screen();
      }
      if ((int32_t)lw(0x80078D78 + 0x0C) >= 0x10)
        sw(0x80078D78 + 0x10, 3);
    }
    break;
  case 3:
    if ((lw(BUTTONS + 8) & (BUTTONS_MASK_TRIANGLE | BUTTONS_MASK_L2)) == (BUTTONS_MASK_TRIANGLE | BUTTONS_MASK_L2)) {
      a0 = 0;
      function_80056B28();
      a0 = 9;
      function_8007AAD4_title_screen();
      s0 = 2;
      function_80056ED4();
      s1 = 0x80076BB8;
      while (lw(0x80076BB8) || CdSync(1, NULL) != CdlComplete || (lw(0x800774B4) & 0x40) == 0) {
        function_800163E4();
        function_8005637C();
        function_8002BBE0();
      }
      function_8002BAB8();
    } else if (lw(BUTTONS) & (BUTTONS_MASK_START | BUTTONS_MASK_CROSS)) {
      sw(0x80078D78 + 0x10, 4);
      a0 = 7;
      function_8007AAD4_title_screen();
    } else if ((int32_t)lw(0x80078D78 + 0x08) >= 990) {
      sw(0x80078D78 + 0x10, 5);
    }
    break;
  case 4:
    if (lw(0x80078D78 + 0x0C) == 0x10) {
      a0 = 8;
      function_8007AAD4_title_screen();
    }
    if ((int32_t)lw(0x80078D78 + 0x0C) >= 0x20) {
      sw(0x80078D78, s1);
      sw(0x80078D78 + 0x10, 0);
      sw(0x80078D78 + 0x04, 0);
      function_800662BC();
    }
    break;
  case 5:
    if (lw(0x80078D78 + 0x0C) == 0) {
      a0 = 5;
      function_8007AAD4_title_screen();
    }
    
    v1 = (int32_t)lw(0x80078D78 + 0x0C) < 0x10;
    sw(0x80075918, v0);
    if (v1) break;
    sw(0x80075918, 15);
    if (v1) break;
    sw(0x80078D78 + 0x00, 3);
    sw(0x80078D78 + 0x04, 0);
    sw(0x80078D78 + 0x1C, 2);
    sh(sp + 0x18, 0);
    sh(sp + 0x1A, 0);
    sh(sp + 0x1C, 0x200);
    sh(sp + 0x1E, 0x1E0);
    ClearImage(addr_to_pointer(sp + 0x18), 0, 0, 0);
    DrawSync(0);
    break;
  default:
    break;
  }
  goto label8007CC48;
label8007B0B8:
  switch (lw(0x80078D78 + 0x10))
  {
  case 0:
    a0 = 1;
    s0 = 0x80078D78 + 0x40;
    a1 = s0;
    a2 = s0 + 4;
    function_80067628();
    temp = v0 == 0;
    if (temp) break;
    v0 = lw(0x80078D78 + 0x04);
    temp = v0 != 0;
    a0 = 1;
    if (temp) goto label8007B134;
    a0 = 2;
    function_8007AAD4_title_screen();
    a0 = 0;
    function_800665B8();
    v0 = 1;
    sw(0x80078D78 + 0x04, 1);
    break;
  label8007B134:
    temp = v0 != a0;
    v0 = 3;
    if (temp) goto label8007B19C;
    v1 = lw(s0 + 4);
    sw(0x80078D78 + 0x30, v1);
    temp = v1 != v0;
    if (temp) goto label8007B158;
    sw(0x80078D78 + 0x30, 0);
  label8007B158:
    v1 = lw(0x80078D78 + 0x30);
    v0 = 4;
    temp = v1 != v0;
    if (temp) goto label8007B184;
    sw(0x80078D78 + 0x30, 0);
    sw(0x80078D78 + 0x38, a0);
    goto label8007B18C;
  label8007B184:
    sw(0x80078D78 + 0x38, 0);
  label8007B18C:
    a0 = 0x10;
    function_800665B8();
    sw(0x80078D78 + 0x04, 2);
    break;
  label8007B19C:
    v1 = lw(s0 + 4);
    sw(0x80078D78 + 0x34, v1);
    temp = v1 != v0;
    if (temp) goto label8007B1B8;
    sw(0x80078D78 + 0x34, 0);
  label8007B1B8:
    v1 = lw(0x80078D78 + 0x34);
    v0 = 4;
    temp = v1 != v0;
    if (temp) goto label8007B1E4;
    sw(0x80078D78 + 0x34, 0);
    sw(0x80078D78 + 0x3C, a0);
    goto label8007B1EC;
  label8007B1E4:
    sw(0x80078D78 + 0x3C, 0);
  label8007B1EC:
    v0 = lw(0x80078D78 + 0x30);
    temp = v0 != 0;
    if (temp) goto label8007B238;
    v0 = lw(0x80078D78 + 0x34);
    temp = v0 == 0;
    v0 = 0xF;
    if (temp) {
      sw(0x80078D78 + 0x10, v0);
      sw(0x80078D78 + 0x14, 0);
      break;
    }
    v0 = lw(0x80078D78 + 0x38);
    temp = v0 != 0;
    v0 = 1;
    if (temp) goto label8007B238;
    sw(0x80078D78 + 0x28, 0);
    goto label8007B268;
  label8007B238:
    v0 = lw(0x80078D78 + 0x34);
    temp = v0 != 0;
    if (temp) goto label8007B290;
    v0 = lw(0x80078D78 + 0x3C);
    temp = v0 != 0;
    v0 = 1;
    if (temp) goto label8007B290;
    sw(0x80078D78 + 0x28, v0);
  label8007B268:
    sw(0x80078D78 + 0x10, v0);
    sw(0x80078D78 + 0x04, 0);
    sw(0x80078D78 + 0x24, 0);
    sw(0x80078D78 + 0x14, 0);
    break;
  label8007B290:
    v0 = lw(0x80078D78 + 0x30);
    temp = v0 != 0;
    if (temp) goto label8007B2C8;
    v0 = lw(0x80078D78 + 0x38);
    temp = v0 == 0;
    v0 = 5;
    if (temp) goto label8007B2C8;
    sw(0x80078D78 + 0x28, 0);
    sw(0x80078D78 + 0x10, v0);
    sw(0x80078D78 + 0x14, 0);
    break;
  label8007B2C8:
    v0 = lw(0x80078D78 + 0x34);
    temp = v0 != 0;
    v0 = -1;
    if (temp) goto label8007B304;
    v0 = lw(0x80078D78 + 0x3C);
    temp = v0 == 0;
    v0 = -1;
    if (temp) goto label8007B304;
    v0 = 1;
    sw(0x80078D78 + 0x28, v0);
    v0 = 5;
    sw(0x80078D78 + 0x10, v0);
    sw(0x80078D78 + 0x14, 0);
    break;
  label8007B304:
    sw(0x80078D78 + 0x28, v0);
    v0 = 2;
    sw(0x80078D78 + 0x10, v0);
    sw(0x80078D78 + 0x14, 0);
    break;
  case 1:
    a0 = 1;
    s0 = 0x80078D78 + 0x40;
    a1 = s0;
    a2 = s0 + 4;
    function_80067628();
    temp = v0 == 0;
    if (temp) break;
    s1 = lw(0x80078D78 + 0x04);
    temp = s1 != 0;
    v0 = 1;
    if (temp) goto label8007B388;
    a1 = 0x8007AA3C; // "BASCUS-94228SPYRO"
    a3 = 0;
    a2 = lw(0x80078D78 + 0x4C);
    a0 = lw(0x80078D78 + 0x28);
    v0 = 0x1400;
    sw(sp + 0x10, v0);
    a0 = a0 << 4;
    function_80066E28();
    sw(0x80078D78 + 0x04, 1);
    break;
  label8007B388:
    temp = s1 != v0;
    if (temp) break;
    a1 = lw(s0 + 4);
    temp = a1 != 0;
    v0 = 5;
    if (temp) goto label8007B548;

    {
      struct savegame *save = addr_to_pointer(lw(0x80078D78 + 0x4C) + 0x200);
      if (savegame_checksum(save + 0) == save->checksum
       && savegame_checksum(save + 1) == save->checksum
       && savegame_checksum(save + 2) == save->checksum)
      {
        sw(0x80078D78 + 0x00, 2);
        sw(0x80078D78 + 0x04, 4);
        sw(0x80078D78 + 0x14, 1);
        sw(0x8007579C, 1);
        a0 = 9;
        function_8007AAD4_title_screen();
        break;
      }
    }
    
    a0 = 0x80078D78 + 0x24;
    if ((int32_t)lw(a0) < 2) {
      sw(a0, lw(a0) + 1);
      sw(0x80078D78 + 0x04, 0);
      break;
    }
    function_8001277C();

    {
      struct savegame *save = addr_to_pointer(lw(0x80078D78 + 0x4C) + 0x200);
      if (savegame_checksum(save) != save->checksum) create_savedata(save);
      save++;
      if (savegame_checksum(save) != save->checksum) create_savedata(save);
      save++;
      if (savegame_checksum(save) != save->checksum) create_savedata(save);
    }

    sw(0x80078D78 + 0x00, 2);
    sw(0x80078D78 + 0x04, 4);
    sw(0x80078D78 + 0x14, 1);
    sw(0x8007579C, 1);
    a0 = 9;
    function_8007AAD4_title_screen();
    break;
  label8007B548:
    if (a1 == v0)
      v0 = 0xA;
    else
      v0 = 0x3;
    sw(0x80078D78 + 0x10, v0);
    sw(0x80078D78 + 0x14, 0);
    break;
  case 2:
    function_80032A20();
    s0 = 0x80078D78 + 0x30;
    v0 = lw(s0);
    temp = v0 == 0;
    if (temp) goto label8007C348;
    a0 = 0;
    if (v0 == 3) goto label8007C34C;
    v0 = lw(0x80078D78 + 0x34);
    if (v0 == 0) goto label8007C34C;
    if (v0 == 3) goto label8007C34C;
    if (lw(0x80078D78 + 0x0C) == 0) {
      a0 = 4;
      function_8007AAD4_title_screen();
    }
    if ((int32_t)lw(0x80078D78 + 0x0C) < 8) break;
    function_80032AB0();
    if (v0) break;
    a0 = 0;
    if (lw(BUTTONS) & (BUTTONS_MASK_LEFT | BUTTONS_MASK_RIGHT)) {
      sw(0x80078D78 + 0x0C, 0x10);
      sw(0x80078D78 + 0x14, 1 - lw(0x80078D78 + 0x14));
      function_8007AAD4_title_screen();
      break;
    }
    v0 = v1 & (BUTTONS_MASK_START | BUTTONS_MASK_CROSS);
    temp = v0 == 0;
    if (temp) break;
    a0 = 0;
    if (lw(0x80078D78 + 0x14) == 0) {
      v0 = 4;
      sw(0x80078D78 + 0x10, v0);
      a0 = 3;
      function_8007AAD4_title_screen();
      break;
    }
    a1 = s0 + 0x10;
    sw(0x80078D78 + 0x10, 0);
    a2 = s0 + 0x14;
    sw(0x80078D78 + 0x04, 0);
    function_80067628();
    break;
    break;
  case 3:
    function_80032A20();
    a2 = 0x80078D78 + 0x28;
    v1 = lw(a2);
    temp = (int32_t)v1 < 0;
    a0 = a2 + 8;
    if (temp) goto label8007B6C4;
    v0 = lw(0x80078D78 + 0x28);
    v0 = v0 << 2;
    v0 += a0;
    v0 = lw(v0);
    temp = v0 != 0;
    v0 = 1;
    if (temp) goto label8007B6AC;
    v0 -= v1;
    v0 = v0 << 2;
    v0 += a0;
    v1 = lw(v0);
    v0 = 3;
    temp = v1 != v0;
    if (temp) goto label8007B6FC;
  label8007B6AC:
    a0 = 0;
    a1 = a2 + 0x18;
    sw(0x80078D78 + 0x10, 0);
    a2 += 0x1C;
    sw(0x80078D78 + 0x04, 0);
    function_80067628();
    break;
  label8007B6C4:
    v0 = lw(0x80078D78 + 0x30);
    temp = v0 == 0;
    v1 = 3;
    if (temp) goto label8007C348;
    temp = v0 == v1;
    a0 = 0;
    if (temp) goto label8007C34C;
    v0 = lw(0x80078D78 + 0x34);
    temp = v0 == 0;
    if (temp) goto label8007C34C;
    temp = v0 == v1;
    if (temp) goto label8007C34C;
  label8007B6FC:
    s0 = 0x80078D78 + 0x0C;
    if ((int32_t)lw(0x80078D78 + 0x0C) < 8) break;
    function_80032AB0();
    if (v0) break;
    if (lw(BUTTONS) & (BUTTONS_MASK_LEFT | BUTTONS_MASK_RIGHT)) {
      sw(0x80078D78 + 0x0C, 0x10);
      sw(0x80078D78 + 0x14, 1 - lw(0x80078D78 + 0x14));
      a0 = 0;
      function_8007AAD4_title_screen();
    } else if (lw(BUTTONS) & (BUTTONS_MASK_START | BUTTONS_MASK_CROSS)) {
      if (lw(0x80078D78 + 0x14)) {
        sw(0x80078D78 + 0x10, 0);
        sw(0x80078D78 + 0x04, 0);
        a0 = 0;
        a1 = 0x80078D78 + 0x40;
        a2 = 0x80078D78 + 0x44;
        function_80067628();
      } else {
        sw(0x80078D78 + 0x10, 4);
        a0 = 3;
        function_8007AAD4_title_screen();
      }
    }
    break;
  case 4:
    function_80032A20();
    a2 = 0x80078D78 + 0x28;
    v1 = lw(a2);
    temp = (int32_t)v1 < 0;
    a0 = a2 + 8;
    if (temp) goto label8007B824;
    v0 = lw(0x80078D78 + 0x28);
    v0 = v0 << 2;
    v0 += a0;
    v0 = lw(v0);
    temp = v0 != 0;
    v0 = 1;
    if (temp) goto label8007B80C;
    v0 -= v1;
    v0 = v0 << 2;
    v0 += a0;
    v1 = lw(v0);
    v0 = 3;
    temp = v1 != v0;
    if (temp) goto label8007B85C;
  label8007B80C:
    a0 = 0;
    a1 = a2 + 0x18;
    sw(0x80078D78 + 0x10, 0);
    a2 += 0x1C;
    sw(0x80078D78 + 0x04, 0);
    function_80067628();
    break;
  label8007B824:
    v0 = lw(0x80078D78 + 0x30);
    temp = v0 == 0;
    v1 = 3;
    if (temp) goto label8007C348;
    temp = v0 == v1;
    a0 = 0;
    if (temp) goto label8007C34C;
    v0 = lw(0x80078D78 + 0x34);
    temp = v0 == 0;
    if (temp) goto label8007C34C;
    temp = v0 == v1;
    if (temp) goto label8007C34C;
  label8007B85C:
    s0 = 0x80078D78 + 0x0C;
    if ((int32_t)lw(s0) < 8) break;
    function_80032AB0();
    if (v0) break;
    v1 = lw(BUTTONS);
    v0 = lw(BUTTONS) & (BUTTONS_MASK_LEFT | BUTTONS_MASK_RIGHT);
    temp = v0 == 0;
    a0 = 0;
    if (temp) goto label8007B8BC;
    sw(s0, 0x10);
    sw(0x80078D78 + 0x14, 1 - lw(0x80078D78 + 0x14));
    function_8007AAD4_title_screen();
    break;
  label8007B8BC:
    v0 = lw(BUTTONS) & (BUTTONS_MASK_START | BUTTONS_MASK_CROSS);
    temp = v0 == 0;
    if (temp) break;
    a0 = 0;
    if (lw(0x80078D78 + 0x14) == 0) {
      function_8001277C();
      sw(0x80078D78, 2);
      sw(0x80078D78 + 0x04, 5);
      sw(0x80078D78 + 0x1C, 0);
      sw(0x80078D14, 0);
      break;
    }
    sw(0x80078D78 + 0x10, 0);
    a1 = s0 + 0x34;
    a2 = s0 + 0x38;
    sw(0x80078D78 + 0x04, 0);
    function_80067628();
    break;
  case 5: case 6:
    function_80032A20();
    v1 = lw(0x80078D78 + 0x28);
    v0 = v1 << 2;
    v0 = lw(0x80078D78 + 0x30 + v0);
    s0 = 0x80078D78 + 0x30;
    temp = v0 != 0;
    a0 = 0;
    if (temp) goto label8007B97C;
    s1 = 1;
    v0 = s1 - v1;
    v0 = v0 << 2;
    v0 += s0;
    v0 = lw(v0);
    temp = v0 != 3;
    if (temp) goto label8007B990;
  label8007B97C:
    a1 = s0 + 0x10;
    sw(0x80078D78 + 0x10, 0);
    a2 = s0 + 0x14;
    sw(0x80078D78 + 0x04, 0);
    function_80067628();
    break;
  label8007B990:
    v0 = lw(0x80078D78 + 0x0C);
    temp = v0 != 0;
    v0 = (int32_t)v0 < 8;
    if (temp) goto label8007B9BC;
    a0 = 3;
    function_8007AAD4_title_screen();
    v0 = lw(0x80078D78 + 0x0C);
    v0 = (int32_t)v0 < 8;
  label8007B9BC:
    temp = v0 != 0;
    if (temp) break;
    function_80032AB0();
    temp = v0 != 0;
    if (temp) break;
    v1 = lw(BUTTONS);
    v0 = v1 & (BUTTONS_MASK_LEFT | BUTTONS_MASK_RIGHT);
    temp = v0 == 0;
    a0 = 0;
    if (temp) goto label8007BA08;
    v0 = lw(0x80078D78 + 0x14);
    v1 = 0x10;
    sw(0x80078D78 + 0x0C, v1);
    v0 = s1 - v0;
    sw(0x80078D78 + 0x14, v0);
    function_8007AAD4_title_screen();
    break;
  label8007BA08:
    v0 = v1 & (BUTTONS_MASK_START | BUTTONS_MASK_CROSS);
    temp = v0 == 0;
    if (temp) break;
    v0 = lw(0x80078D78 + 0x14);
    temp = v0 != 0;
    v0 = 5;
    if (temp) goto label8007BA70;
    v1 = lw(0x80078D78 + 0x10);
    temp = v1 != v0;
    v0 = 7;
    if (temp) goto label8007BA58;
    v0 = 6;
    sw(0x80078D78 + 0x10, v0);
    sw(0x80078D78 + 0x14, s1);
    break;
  label8007BA58:
    sw(0x80078D78 + 0x10, v0);
    a0 = 0;
    a1 = s0 + 0x10;
    a2 = s0 + 0x14;
    function_80067628();
    a0 = 2;
    function_8007AAD4_title_screen();
    break;
  label8007BA70:
    sw(0x80078D78 + 0x10, 3);
    sw(0x80078D78 + 0x14, 0);
    a0 = 3;
    function_8007AAD4_title_screen();
    break;
  case 7:
    s0 = 0x80078D78 + 0x0C;
    v0 = lw(s0);
    v0 = (int32_t)v0 < 2;
    temp = v0 != 0;
    if (temp) goto label8007CC48;
    a0 = lw(0x80078D78 + 0x28);
    a0 = a0 << 4;
    function_80067B28();
    sw(0x80078D78 + 0x44, v0);
    temp = v0 != 0;
    a0 = 4;
    if (temp) goto label8007BAE8;
    v0 = lw(0x80078D78 + 0x28);
    v1 = s0 + 0x24;
    v0 = v0 << 2;
    v0 += v1;
    sw(v0, 0);
    sw(0x80078D78 + 0x10, 8);
    sw(0x80078D78 + 0x14, 0);
    a0 = 9;
    function_8007AAD4_title_screen();
    break;
  label8007BAE8:
    sw(0x80078D78 + 0x10, 9);
    sw(0x80078D78 + 0x14, 1);
    function_8007AAD4_title_screen();
    break;
  case 8:
    function_80032A20();
    v1 = lw(0x80078D78 + 0x28);
    v0 = v1 << 2;
    v0 = lw(0x80078D78 + 0x30 + v0);
    a2 = 0x80078D78 + 0x30;
    temp = v0 != 0;
    a0 = 0;
    if (temp) goto label8007BB50;
    s0 = 1;
    v0 = s0 - v1;
    v0 = v0 << 2;
    v0 += a2;
    v1 = lw(v0);
    v0 = 3;
    temp = v1 != v0;
    if (temp) goto label8007BB64;
  label8007BB50:
    a1 = a2 + 0x10;
    sw(0x80078D78 + 0x10, 0);
    a2 += 0x14;
    sw(0x80078D78 + 0x04, 0);
    function_80067628();
    break;
  label8007BB64:
    v0 = lw(0x80078D78 + 0x0C);
    v0 = (int32_t)v0 < 8;
    temp = v0 != 0;
    if (temp) break;
    function_80032AB0();
    temp = v0 != 0;
    if (temp) break;
    v0 = lw(BUTTONS);
    v0 = v0 & (BUTTONS_MASK_START | BUTTONS_MASK_CROSS);
    temp = v0 == 0;
    if (temp) break;
    a0 = 1;
    function_8007AAD4_title_screen();
    sw(0x80078D78 + 0x10, s0);
    sw(0x80078D78 + 0x04, 0);
    sw(0x80078D78 + 0x24, 0);
    sw(0x80078D78 + 0x14, s0);
    break;
  case 9:
    function_80032A20();
    v1 = lw(0x80078D78 + 0x28);
    v0 = v1 << 2;
    v0 = lw(0x80078D78 + 0x30 + v0);
    s0 = 0x80078D78 + 0x30;
    temp = v0 != 0;
    a0 = 0;
    if (temp) goto label8007BC28;
    v0 = 1 - v1;
    v0 = v0 << 2;
    v0 += s0;
    v0 = lw(v0);
    s1 = 3;
    temp = v0 != s1;
    if (temp) goto label8007BC3C;
  label8007BC28:
    a1 = s0 + 0x10;
    sw(0x80078D78 + 0x10, 0);
    a2 = s0 + 0x14;
    sw(0x80078D78 + 0x04, 0);
    function_80067628();
    break;
  label8007BC3C:
    v0 = lw(0x80078D78 + 0x0C);
    v0 = (int32_t)v0 < 8;
    temp = v0 != 0;
    if (temp) break;
    function_80032AB0();
    temp = v0 != 0;
    if (temp) break;
    v1 = lw(BUTTONS);
    v0 = v1 & (BUTTONS_MASK_LEFT | BUTTONS_MASK_RIGHT);
    temp = v0 != 0;
    a0 = 0;
    if (temp) {
      sw(0x80078D78 + 0x0C, 0x10);
      sw(0x80078D78 + 0x14, 1 - lw(0x80078D78 + 0x14));
      function_8007AAD4_title_screen();
      break;
    }
    v0 = v1 & (BUTTONS_MASK_START | BUTTONS_MASK_CROSS);
    temp = v0 == 0;
    if (temp) break;
    v0 = lw(0x80078D78 + 0x14);
    temp = v0 != 0;
    v0 = 7;
    if (temp) {
      a0 = 3;
      sw(0x80078D78 + 0x10, s1);
      sw(0x80078D78 + 0x14, 0);
      function_8007AAD4_title_screen();
      break;
    }
    sw(0x80078D78 + 0x10, v0);
    a0 = 0;
    a1 = s0 + 0x10;
    a2 = s0 + 0x14;
    function_80067628();
    a0 = 2;
    function_8007AAD4_title_screen();
    break;
  case 10:
    function_80032A20();
    v1 = lw(0x80078D78 + 0x28);
    v0 = v1 << 2;
    v0 = lw(0x80078D78 + 0x30 + v0);
    s0 = 0x80078D78 + 0x30;
    temp = v0 != 0;
    a0 = 0;
    if (temp) goto label8007BD08;
    v0 = 1 - v1;
    v0 = v0 << 2;
    v0 += s0;
    v0 = lw(v0);
    s1 = 3;
    temp = v0 != s1;
    if (temp) goto label8007BD1C;
  label8007BD08:
    a1 = s0 + 0x10;
    sw(0x80078D78 + 0x10, 0);
    a2 = s0 + 0x14;
    sw(0x80078D78 + 0x04, 0);
    function_80067628();
    break;
  label8007BD1C:
    v0 = lw(0x80078D78 + 0x0C);
    temp = v0 != 0;
    v0 = (int32_t)v0 < 8;
    if (temp) goto label8007BD48;
    a0 = 3;
    function_8007AAD4_title_screen();
    v0 = lw(0x80078D78 + 0x0C);
    v0 = (int32_t)v0 < 8;
  label8007BD48:
    temp = v0 != 0;
    if (temp) break;
    function_80032AB0();
    temp = v0 != 0;
    if (temp) break;
    v1 = lw(BUTTONS);
    v0 = v1 & (BUTTONS_MASK_LEFT | BUTTONS_MASK_RIGHT);
    temp = v0 != 0;
    a0 = 0;
    if (temp) {
      sw(0x80078D78 + 0x0C, 0x10);
      sw(0x80078D78 + 0x14, 1 - lw(0x80078D78 + 0x14));
      function_8007AAD4_title_screen();
      break;
    }
    v0 = v1 & (BUTTONS_MASK_START | BUTTONS_MASK_CROSS);
    temp = v0 == 0;
    if (temp) break;
    v0 = lw(0x80078D78 + 0x14);
    temp = v0 == 0;
    a1 = s0 + 0x10;
    if (temp) goto label8007C2A8;
    a0 = 3;
    sw(0x80078D78 + 0x10, s1);
    sw(0x80078D78 + 0x14, 0);
    function_8007AAD4_title_screen();
    break;
  case 11:
    s0 = 0x80078D78 + 0x04;
    v0 = lw(s0);
    temp = v0 != 0;
    if (temp) goto label8007BDC4;
    v0 = 1;
    sw(s0, v0);
    break;
  label8007BDC4:
    temp = v0 != 1;
    s1 = 2;
    if (temp) goto label8007BEB0;
    v0 = lw(0x80078D78 + 0x24);
    temp = v0 != 0;
    a2 = 1;
    if (temp) goto label8007BE00;
    a1 = 0x8007AA3C; // "BASCUS-94228SPYRO"
    a0 = lw(0x80078D78 + 0x28);
    a0 = a0 << 4;
    function_80067718();
    sw(0x80078D78 + 0x44, v0);
  label8007BE00:
    v1 = lw(0x80078D78 + 0x44);
    temp = v1 != 0;
    v0 = 7;
    if (temp) goto label8007BEA0;
    a1 = 0x8006F494;
    a2 = 0x200;
    s0 = lw(0x80078D78 + 0x4C);
    s1 = 0;
    a0 = s0;
    spyro_memcpy32(a0, a1, a2);
    s0 += 0x200;
    function_80017FD4();
    v1 = lw(0x80078D78 + 0x4C);
    sw(0x80078D20, v0);
    sh(v1 + 0x7E, v0);
    function_8001277C();
  label8007BE54:
    a0 = s0;
    create_savedata(addr_to_pointer(a0));
    s1++;
    v0 = (int32_t)s1 < 3;
    temp = v0 != 0;
    s0 += 0x600;
    if (temp) goto label8007BE54;
    a1 = 0x8007AA3C; // "BASCUS-94228SPYRO"
    a3 = 0;
    a2 = lw(0x80078D78 + 0x4C);
    a0 = lw(0x80078D78 + 0x28);
    v0 = 0x1400;
    sw(sp + 0x10, v0);
    a0 = a0 << 4;
    function_800670E4();
    v0 = 2;
    sw(0x80078D78 + 0x04, v0);
    break;
  label8007BEA0:
    temp = v1 != v0;
    if (v1 == v0)
      v0 = 0xC;
    else
      v0 = 0xD;
    sw(0x80078D78 + 0x10, v0);
    sw(0x80078D78 + 0x14, 0);
    break;
  label8007BEB0:
    temp = v0 != s1;
    a0 = 1;
    if (temp) goto label8007BF14;
    a1 = 0x80078D78 + 0x40;
    a2 = 0x80078D78 + 0x44;
    function_80067628();
    temp = v0 == 0;
    if (temp) break;
    v0 = lw(0x80078D78 + 0x44);
    temp = v0 != 0;
    v0 = 0xD;
    if (temp) {
      sw(0x80078D78 + 0x10, v0);
      sw(0x80078D78 + 0x14, 0);
      break;
    }
    a1 = 0x8007AA3C; // "BASCUS-94228SPYRO"
    a3 = 0;
    a2 = lw(0x80078D78 + 0x4C);
    a0 = lw(0x80078D78 + 0x28);
    v0 = 0x1400;
    sw(sp + 0x10, v0);
    a0 = a0 << 4;
    function_80066E28();
    v0 = 3;
    sw(s0, v0);
    break;
  label8007BF14:
    v0 = lw(0x80078D78 + 0x44);
    temp = v0 != 0;
    v0 = 0xD;
    if (temp) {
      sw(0x80078D78 + 0x10, v0);
      sw(0x80078D78 + 0x14, 0);
      break;
    }

    {
      struct savegame *save = addr_to_pointer(lw(0x80078D78 + 0x4C) + 0x200);
      if (savegame_checksum(save + 0) == save->checksum
       && savegame_checksum(save + 1) == save->checksum
       && savegame_checksum(save + 2) == save->checksum)
      {
        sw(0x80078D78 + 0x00, 2);
        sw(0x80078D78 + 0x04, 4);
        sw(0x80078D78 + 0x14, 0);
        sw(0x8007579C, 1);
        a0 = 9;
        function_8007AAD4_title_screen();
        break;
      }
    }

    a0 = 0x80078D78 + 0x24;
    if ((int32_t)lw(a0) < 2) {
      sw(a0, lw(a0) + 1);
      sw(0x80078D78 + 0x04, 2);
      break;
    }
    function_8001277C();

    {
      struct savegame *save = addr_to_pointer(lw(0x80078D78 + 0x4C) + 0x200);
      if (savegame_checksum(save) != save->checksum) create_savedata(save);
      save++;
      if (savegame_checksum(save) != save->checksum) create_savedata(save);
      save++;
      if (savegame_checksum(save) != save->checksum) create_savedata(save);
    }
    
    sw(0x80078D78 + 0x00, 2);
    sw(0x80078D78 + 0x04, 4);
    sw(0x80078D78 + 0x14, 0);
    sw(0x8007579C, 1);
    a0 = 9;
    function_8007AAD4_title_screen();
    break;
  case 12:
    function_80032A20();
    v1 = lw(0x80078D78 + 0x28);
    v0 = v1 << 2;
    v0 = lw(0x80078D78 + 0x30 + v0);
    a2 = 0x80078D78 + 0x30;
    temp = v0 != 0;
    a0 = 0;
    if (temp) goto label8007C134;
    v0 = 1;
    v0 -= v1;
    v0 = v0 << 2;
    v0 += a2;
    v0 = lw(v0);
    s0 = 3;
    temp = v0 != s0;
    if (temp) goto label8007C148;
  label8007C134:
    a1 = a2 + 0x10;
    sw(0x80078D78 + 0x10, 0);
    a2 += 0x14;
    sw(0x80078D78 + 0x04, 0);
    function_80067628();
    break;
  label8007C148:
    v0 = lw(0x80078D78 + 0x0C);
    temp = v0 != 0;
    v0 = (int32_t)v0 < 8;
    if (temp) goto label8007C174;
    a0 = 4;
    function_8007AAD4_title_screen();
    v0 = lw(0x80078D78 + 0x0C);
    v0 = (int32_t)v0 < 8;
  label8007C174:
    temp = v0 != 0;
    if (temp) break;
    function_80032AB0();
    temp = v0 != 0;
    if (temp) break;
    v0 = lw(BUTTONS);
    v0 = v0 & (BUTTONS_MASK_START | BUTTONS_MASK_CROSS);
    temp = v0 == 0;
    if (temp) break;
    a0 = 3;
    function_8007AAD4_title_screen();
    sw(0x80078D78 + 0x10, s0);
    sw(0x80078D78 + 0x14, 0);
    break;
  case 13:
    function_80032A20();
    v1 = lw(0x80078D78 + 0x28);
    v0 = v1 << 2;
    v0 = lw(0x80078D78 + 0x30 + v0);
    s0 = 0x80078D78 + 0x30;
    temp = v0 != 0;
    a0 = 0;
    if (temp) goto label8007C210;
    v0 = 1 - v1;
    v0 = v0 << 2;
    v0 += s0;
    v0 = lw(v0);
    s1 = 3;
    temp = v0 != s1;
    if (temp) goto label8007C224;
  label8007C210:
    a1 = s0 + 0x10;
    a2 = s0 + 0x14;
    sw(0x80078D78 + 0x10, 0);
    sw(0x80078D78 + 0x04, 0);
    function_80067628();
    break;
  label8007C224:
    v0 = lw(0x80078D78 + 0x0C);
    temp = v0 != 0;
    v0 = (int32_t)v0 < 8;
    if (temp) goto label8007C250;
    a0 = 4;
    function_8007AAD4_title_screen();
    v0 = lw(0x80078D78 + 0x0C);
    v0 = (int32_t)v0 < 8;
  label8007C250:
    temp = v0 != 0;
    if (temp) break;
    function_80032AB0();
    temp = v0 != 0;
    if (temp) break;
    v1 = lw(BUTTONS);
    v0 = v1 & (BUTTONS_MASK_LEFT | BUTTONS_MASK_RIGHT);
    temp = v0 != 0;
    a0 = 0;
    if (temp) {
      sw(0x80078D78 + 0x0C, 0x10);
      sw(0x80078D78 + 0x14, 1 - lw(0x80078D78 + 0x14));
      function_8007AAD4_title_screen();
      break;
    }
    v0 = v1 & (BUTTONS_MASK_START | BUTTONS_MASK_CROSS);
    temp = v0 == 0;
    if (temp) break;
    
    if (lw(0x80078D78 + 0x14)) {
      sw(0x80078D78 + 0x10, s1);
      sw(0x80078D78 + 0x14, 0);
      a0 = 3;
      function_8007AAD4_title_screen();
      break;
    }
    a0 = 0;
    a1 = s0 + 0x10;
  label8007C2A8:
    a2 = s0 + 0x14;
    v0 = 0xB;
    sw(0x80078D78 + 0x10, v0);
    sw(0x80078D78 + 0x04, 0);
    sw(0x80078D78 + 0x24, 0);
    function_80067628();
    a0 = 2;
    function_8007AAD4_title_screen();
    break;
  case 15:
    s0 = 1;
    function_80032A20();
    s1 = 0x80078D78 + 0x30;
    v0 = lw(0x80078D78 + 0x30);
    temp = v0 == s0;
    v1 = 3;
    if (temp) goto label8007C348;
    temp = v0 == v1;
    a0 = 0;
    if (temp) goto label8007C34C;
    v0 = lw(0x80078D78 + 0x34);
    temp = v0 == s0;
    if (temp) goto label8007C34C;
    temp = v0 != v1;
    if (temp) goto label8007C378;
  label8007C348:
    a0 = 0;
  label8007C34C:
    v0 = 0x80078D78 + 0x10;
    a1 = v0 + 0x30;
    a2 = v0 + 0x34;
    sw(v0, 0);
    sw(0x80078D78 + 0x04, 0);
    function_80067628();
    break;
  label8007C378:
    v0 = lw(0x80078D78 + 0x0C);
    temp = v0 != 0;
    v0 = (int32_t)v0 < 8;
    if (temp) goto label8007C3A4;
    a0 = 3;
    function_8007AAD4_title_screen();
    v0 = lw(0x80078D78 + 0x0C);
    v0 = (int32_t)v0 < 8;
  label8007C3A4:
    temp = v0 != 0;
    if (temp) break;
    v0 = lw(BUTTONS) & (BUTTONS_MASK_LEFT | BUTTONS_MASK_RIGHT);
    temp = v0 == 0;
    a0 = 0;
    if (temp) goto label8007C3E0;
    v0 = lw(0x80078D78 + 0x14);
    v1 = 0x10;
    sw(0x80078D78 + 0x0C, v1);
    v0 = s0 - v0;
    sw(0x80078D78 + 0x14, v0);
    function_8007AAD4_title_screen();
    break;
  label8007C3E0:
    v0 = lw(BUTTONS) & (BUTTONS_MASK_START | BUTTONS_MASK_CROSS);
    temp = v0 == 0;
    if (temp) break;
    v0 = lw(0x80078D78 + 0x14);
    sw(0x80078D78 + 0x28, v0);
    v0 = lw(0x80078D78 + 0x38 + v0*4);
    temp = v0 == 0;
    v0 = 5;
    if (temp) goto label8007C424;
    sw(0x80078D78 + 0x10, v0);
    sw(0x80078D78 + 0x14, 0);
    break;
  label8007C424:
    sw(0x80078D78 + 0x10, s0);
    sw(0x80078D78 + 0x04, 0);
    sw(0x80078D78 + 0x24, 0);
    sw(0x80078D78 + 0x14, s0);
    a0 = 1;
    function_8007AAD4_title_screen();
    break;
  default:
    break;
  }
  goto label8007CC48;
label8007C454:
  if ((int32_t)lw(s0 + 0x04) < 5) {
    function_80032A20();
    v1 = lw(0x80078D78 + 0x28);
    if (lw(s0 + 0x30 + v1*4) && lw(s0 + 0x30 + (1 - v1)*4) == 3) {
      a0 = 0;
      a1 = s0 + 0x40;
      a2 = s0 + 0x44;
      sw(s0, 1);
      sw(0x80078D78 + 0x10, 0);
      sw(s0 + 4, 0);
      function_80067628();
      goto label8007CC48;
    }
  }
  {
    struct savegame *saves = addr_to_pointer(lw(0x80078D78 + 0x4C) + 0x200);

    sw(0x80078D78 + 0x50, pointer_to_addr(&saves[0]));
    sw(0x80078D78 + 0x54, pointer_to_addr(&saves[1]));
    sw(0x80078D78 + 0x58, pointer_to_addr(&saves[2]));
    
    sw(0x80078D78 + 0x48, saves[0].unknown40[0] || saves[1].unknown40[0] || saves[2].unknown40[0]);
  }
  switch (lw(0x80078D78 + 0x04)) {
  case 0:
    if (lw(0x80078D78 + 0x20) == 0) {
      sw(0x80078D78 + 0x04, 1);
      if (       lbu(lw(0x80078D78 + 0x50) + 0x40) == 0) {
        sw(0x80078D78 + 0x14, 0);
      } else if (lbu(lw(0x80078D78 + 0x54) + 0x40) == 0) {
        sw(0x80078D78 + 0x14, 1);
      } else if (lbu(lw(0x80078D78 + 0x58) + 0x40) == 0) {
        sw(0x80078D78 + 0x14, 2);
      } else {
        sw(0x80078D78 + 0x14, 0);
      }
    } else if (lw(0x80078D78 + 0x48)) {
      sw(0x80078D78 + 0x04, 3);
      if (       lbu(lw(0x80078D78 + 0x50) + 0x40)) {
        sw(0x80078D78 + 0x14, 0);
      } else if (lbu(lw(0x80078D78 + 0x54) + 0x40)) {
        sw(0x80078D78 + 0x14, 1);
      } else {/* lbu(lw(0x80078D78 + 0x54) + 0x40) */
        sw(0x80078D78 + 0x14, 2);
      }
    } else {
      sw(0x80078D78 + 0x14, 1);
      sw(0x80078D78 + 0x04, 4);
    }
    break;
  case 1:
    if ((int32_t)lw(0x80078D78 + 0x0C) < 8) break;
    if (lw(BUTTONS) & (BUTTONS_MASK_DOWN | BUTTONS_MASK_TRIANGLE)) {
      a0 = 9;
      function_8007AAD4_title_screen();
      sw(0x80078D78 + 0x04, 4);
      sw(0x80078D78 + 0x14, 0);
    } else if (lw(BUTTONS) & BUTTONS_MASK_LEFT) {
      sw(0x80078D78 + 0x0C, 0x10);
      sw(0x80078D78 + 0x14, (lw(0x80078D78 + 0x14) - 1) % 3);

      a0 = 0;
      function_8007AAD4_title_screen();
    } else if (lw(BUTTONS) & BUTTONS_MASK_RIGHT) {
      sw(0x80078D78 + 0x0C, 0x10);
      sw(0x80078D78 + 0x14, (lw(0x80078D78 + 0x14) + 1) % 3);
      
      a0 = 0;
      function_8007AAD4_title_screen();
    } else if (lw(BUTTONS) & (BUTTONS_MASK_START | BUTTONS_MASK_CROSS)) {
      if (lbu(lw(0x80078D78 + 0x50 + lw(0x80078D78 + 0x14)*4) + 0x40)) {
        a0 = 3;
        function_8007AAD4_title_screen();
        sw(0x80078D78 + 0x04, 2);
        sw(0x80078D78 + 0x18, 1);
      } else {
        function_8001277C();

        a0 = 0;
        a1 = 0x80078D78 + 0x40;
        a2 = 0x80078D78 + 0x44;
        a3 = lhu(lw(0x80078D78 + 0x4C) + 0x7E);
        
        sw(0x80078D78 + 0x04, 5);
        sw(0x80078D78 + 0x1C, 0);
        sw(0x80078D14, 1);
        sw(0x80078D18, lw(0x80078D78 + 0x28)*16);
        sw(0x80078D1C, lw(0x80078D78 + 0x14));
        sw(0x80078D20, a3);
        function_80067628();
      }
    }
    break;
  case 2:
    if ((int32_t)lw(0x80078D78 + 0x0C) < 8) break;
    if (lw(BUTTONS) & (BUTTONS_MASK_LEFT | BUTTONS_MASK_RIGHT)) {
      sw(0x80078D78 + 0x0C, 0x10);
      sw(0x80078D78 + 0x18, 1 - lw(0x80078D78 + 0x18));
      a0 = 0;
      function_8007AAD4_title_screen();
    } else if (lw(BUTTONS) & (BUTTONS_MASK_START | BUTTONS_MASK_CROSS | BUTTONS_MASK_TRIANGLE)) {
      if (lw(0x80078D78 + 0x18) == 1 || lw(BUTTONS) & BUTTONS_MASK_TRIANGLE) {
        sw(0x80078D78 + 0x04, 1);
        a0 = 4;
        function_8007AAD4_title_screen();
      } else {
        function_8001277C();

        a0 = 0;
        a1 = 0x80078D78 + 0x40;
        a2 = 0x80078D78 + 0x44;
        a3 = lhu(lw(0x80078D78 + 0x4C) + 0x7E);

        sw(0x80078D78 + 0x04, 5);
        sw(0x80078D78 + 0x1C, 0);
        sw(0x80078D14, 1);
        sw(0x80078D18, lw(0x80078D78 + 0x28)*16);
        sw(0x80078D1C, lw(0x80078D78 + 0x14));
        sw(0x80078D20, a3);
        function_80067628();
      }
    }
    break;
  case 3:
    // save game select
    if ((int32_t)lw(0x80078D78 + 0x0C) < 8) break;

    if (lw(BUTTONS) & (BUTTONS_MASK_DOWN | BUTTONS_MASK_TRIANGLE)) {
      a0 = 9;
      function_8007AAD4_title_screen();
      sw(0x80078D78 + 0x04, 4);
      sw(0x80078D78 + 0x14, 1);
    } else if (lw(BUTTONS) & BUTTONS_MASK_LEFT) {
      a0 = lw(0x80078D78 + 0x14) - 1;
      v1 = 0x80078D78 + 0x50 + a0*4;
      while ((int32_t)a0 >= 0) {
        if (lbu(lw(v1) + 0x40)) {
          sw(0x80078D78 + 0x0C, 0x10);
          if ((int32_t)a0 < 3) {
            sw(0x80078D78 + 0x14, a0);

            a0 = 0;
            function_8007AAD4_title_screen();
          }
          break;
        }
        a0--;
        v1 -= 4;
      }
    } else if (lw(BUTTONS) & BUTTONS_MASK_RIGHT) {
      a0 = lw(0x80078D78 + 0x14) + 1;
      v1 = 0x80078D78 + 0x50 + a0*4;
      while ((int32_t)a0 < 3) {
        if (lbu(lw(v1) + 0x40)) {
          sw(0x80078D78 + 0x0C, 0x10);
          if ((int32_t)a0 < 3) {
            sw(0x80078D78 + 0x14, a0);

            a0 = 0;
            function_8007AAD4_title_screen();
          }
          break;
        }
        a0++;
        v1 += 4;
      }
    } else if (lw(BUTTONS) & (BUTTONS_MASK_START | BUTTONS_MASK_CROSS)) {
      sw(PLAYER_DATA + 0x164, 3);
      sw(0x800758E8, 0);
      sw(0x8007580C, -1);
      a0 = lw(0x80078D78 + 0x50 + lw(0x80078D78 + 0x14)*4);
      function_80059594();

      a0 = 0;
      a1 = 0x80078D78 + 0x40;
      a2 = 0x80078D78 + 0x44;
      a3 = lhu(lw(0x80078D78 + 0x4C) + 0x7E);

      sw(0x80078D78 + 0x04, 5);
      sw(0x80078D78 + 0x1C, 1);
      sw(0x80078D14, 1);
      sw(0x80078D18, lw(0x80078D78 + 0x28)*16);
      sw(0x80078D1C, lw(0x80078D78 + 0x14));
      sw(0x80078D20, a3);
      function_80067628();
    }
    break;
  case 4:
    if ((int32_t)lw(0x80078D78 + 0x0C) < 8) break;
    function_80032AB0();
    if (v0) break;
    if (lw(BUTTONS) & (BUTTONS_MASK_LEFT | BUTTONS_MASK_RIGHT)) {
      sw(0x80078D78 + 0x0C, 0x10);
      sw(0x80078D78 + 0x14, 1 - lw(0x80078D78 + 0x14));
      a0 = 0;
      function_8007AAD4_title_screen();
    } else if (lw(BUTTONS) & (BUTTONS_MASK_UP | BUTTONS_MASK_START | BUTTONS_MASK_CROSS)) {
      if (lw(0x80078D78 + 0x14) != 1 || lw(0x80078D78 + 0x48)) {
        sw(0x80078D78 + 0x04, 0);
        sw(0x8007579C, 1);
        sw(0x80078D78 + 0x20, lw(0x80078D78 + 0x14));
        a0 = 1;
        function_8007AAD4_title_screen();
      } else {
        sw(0x80078D78 + 0x0C, 0x10);
        a0 = 4;
        function_8007AAD4_title_screen();
      }
    }
    break;
  case 5:
    if (lw(0x80078D78 + 0x0C) == 0) {
      a0 = 5;
      function_8007AAD4_title_screen();
    }
    v1 = lw(0x80078D78 + 0x0C);
    v0 = v1 + 1;
    sw(0x80075918, v0);
    if ((int32_t)v0 >= 0x10) {
      sw(0x80075918, 0xF);
    }
    if ((int32_t)v1 < 0x10) {
      function_8006631C();
      sw(0x80078D78, 3);
      sw(0x80078D78 + 0x04, 0);
      sh(sp + 0x18, 0);
      sh(sp + 0x1A, 0);
      sh(sp + 0x1C, 0x200);
      sh(sp + 0x1E, 0x1E0);
      ClearImage(addr_to_pointer(sp + 0x18), 0, 0, 0);
      DrawSync(0);
    }
  }
  label8007CC48:
  a0 = 3;
  function_80058CC0();
  if ((int32_t)lw(BUTTONS + 0xC) > 1 && lhu(BUTTONS + 8)) {
    sw(0x80078D78 + 0x08, 0);
  } else {
    sw(0x80078D78 + 0x08, lw(0x80078D78 + 0x08) + 1);
  }

  if ((int32_t)lw(0x80078D78) < 2) {
    if (lw(0x80078D78 + 0x10) == s4) {
      sw(0x80078D78 + 0x0C, lw(0x80078D78 + 0x0C) + 1);
    } else {
      sw(0x80078D78 + 0x0C, 0);
    }
  } else {
    if (lw(0x80078D78 + 0x04) == s4) {
      sw(0x80078D78 + 0x0C, lw(0x80078D78 + 0x0C) + 1);
    } else {
      sw(0x80078D78 + 0x0C, 0);
    }
  }

  ra = lw(sp + 0x64);
  s4 = lw(sp + 0x60);
  s1 = lw(sp + 0x54);
  s0 = lw(sp + 0x50);
  sp += 0x68;
  return;
}

// size: 0x00001024
void function_800314B4(void)
{
  uint32_t temp;
  sp -= 72; // 0xFFFFFFB8
  sw(sp + 0x0034, s1);
  s1 = 0x80078D04;
  sw(sp + 0x40, ra);
  sw(sp + 0x3C, s3);
  sw(sp + 0x38, s2);
  sw(sp + 0x0030, s0);
  v0 = lw(s1);
  v1 = lw(0x800756CC);
  a0 = 3; // 0x0003
  v0 += v1;
  sw(s1, v0);
  function_80058CC0();
  a0 = 0x8006FCF4 + 0x0400;
  v0 = lw(0x80078D74);
  a1 = 0;
  sw(0x8006FCF4 + 0x0404, 0);
  sw(a0, v0);
  function_800522C0();
  v1 = lw(s1 - 0x0004); // 0xFFFFFFFC
  temp = v1 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label80031534;
  temp = v1 == v0;
  if (temp) goto label80031DBC;
  goto label800324B8;
label80031534:
  a0 = lw(s1);
  v0 = (int32_t)a0 < 64;
  temp = v0 == 0;
  a0 = a0 << 5;
  if (temp) goto label80031BA8;
  a0 -= 1024; // 0xFFFFFC00
  v0 = spyro_sin(a0);
  v1 = lw(0x80078D74);
  v1 = lw(v1);
  v1 = lw(v1 + 0x0004);
  s3 = v0 + 4096; // 0x1000
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 2;
  v0 -= v1;
  v1 = lw(0x80075828);
  v0 = v0 << 3;
  a0 = v1 + v0;
  v1 = lbu(a0 + 0x48);
  v0 = 2; // 0x0002
  temp = v1 != v0;
  if (temp) goto label800315CC;
  v1 = lw(0x800756CC);
  v0 = lbu(a0 + 0x0049);
  v0 += v1;
  sb(a0 + 0x0049, v0);
  v0 = v0 & 0xFF;
  v0 = v0 < 48;
  temp = v0 != 0;
  if (temp) goto label800315CC;
  sb(a0 + 0x48, 0);
  sb(a0 + 0x3C, 0);
label800315CC:
  s0 = 0x80078D38;
  a0 = lw(s0);
  a1 = lw(s0 - 0x000C); // 0xFFFFFFF4
  v0 = a0 - a1;
  v0 = v0 & 0xFFF;
  v0 = (int32_t)v0 < 2049;
  temp = v0 == 0;
  if (temp) goto label80031614;
  v0 = spyro_two_angle_diff_12bit(a0, a1);
  mult(v0, s3);
  v1 = lw(s0 - 0x000C); // 0xFFFFFFF4
  t0=lo;
  v0 = (int32_t)t0 >> 13;
  s0 = v1 + v0;
  goto label80031630;
label80031614:
  v0 = spyro_two_angle_diff_12bit(a0, a1);
  mult(v0, s3);
  v1 = lw(s0 - 0x000C); // 0xFFFFFFF4
  t0=lo;
  v0 = (int32_t)t0 >> 13;
  s0 = v1 - v0;
label80031630:
  v0 = lw(0x80078D3C);
  v1 = lw(0x80078D30);
  v0 -= v1;
  mult(v0, s3);
  a0 = s0;
  t0=lo;
  v0 = (int32_t)t0 >> 13;
  s2 = v1 + v0;
  v0 = spyro_cos(a0);
  mult(v0, s2);
  a0 = s0;
  v0 = lw(PLAYER_POSITION);
  v1 = lw(0x80078D74);
  t0=lo;
  a1 = (int32_t)t0 >> 12;
  v0 += a1;
  sw(v1 + 0x000C, v0);
  v0 = spyro_sin(a0);
  mult(v0, s2);
  v0 = lw(PLAYER_POSITION + 0x0004);
  v1 = lw(0x80078D74);
  t0=lo;
  a0 = (int32_t)t0 >> 12;
  v0 += a0;
  sw(v1 + 0x0010, v0);
  v0 = lw(0x80078D40);
  v1 = lw(0x80078D34);
  v0 -= v1;
  mult(v0, s3);
  a1 = 3; // 0x0003
  a2 = 0;
  a0 = lw(0x80078D74);
  t0=lo;
  v0 = (int32_t)t0 >> 13;
  v1 += v0;
  sw(a0 + 0x0014, v1);
  a0 = lw(0x80078D74);
  a3 = 0;
  function_80038DC0();
  a0 = lw(0x80078D74);
  a1 = 4096; // 0x1000
  a0 += 12; // 0x000C
  function_8004D5EC();
  a0 = lw(0x80078D74);
  function_800533D0();
  v0 = lw(0x80075898);
  temp = v0 == 0;
  if (temp) goto label80031820;
  a0 = lw(0x80078D4C);
  a1 = lw(0x80078D44);
  v0 = a0 - a1;
  v0 = v0 & 0xFFF;
  v0 = (int32_t)v0 < 2049;
  temp = v0 == 0;
  if (temp) goto label80031780;
  v0 = spyro_two_angle_diff_12bit(a0, a1);
  mult(v0, s3);
  v1 = lw(0x80078D44);
  t0=lo;
  v0 = (int32_t)t0 >> 13;
  s0 = v1 + v0;
  goto label800317A0;
label80031780:
  v0 = spyro_two_angle_diff_12bit(a0, a1);
  mult(v0, s3);
  v1 = lw(0x80078D44);
  t0=lo;
  v0 = (int32_t)t0 >> 13;
  s0 = v1 - v0;
label800317A0:
  v0 = lw(0x80078D50);
  v1 = lw(0x80078D48);
  v0 -= v1;
  mult(v0, s3);
  a0 = s0;
  t0=lo;
  v0 = (int32_t)t0 >> 13;
  s2 = v1 + v0;
  v0 = spyro_cos(a0);
  mult(v0, s2);
  a0 = s0;
  v0 = lw(PLAYER_POSITION);
  v1 = lw(0x80075898);
  t0=lo;
  a1 = (int32_t)t0 >> 12;
  v0 += a1;
  sw(v1 + 0x000C, v0);
  v0 = spyro_sin(a0);
  mult(v0, s2);
  v0 = lw(PLAYER_POSITION + 0x0004);
  v1 = lw(0x80075898);
  t0=lo;
  a0 = (int32_t)t0 >> 12;
  v0 += a0;
  sw(v1 + 0x0010, v0);
label80031820:
  s0 = 0x80078D60;
  a0 = lw(s0);
  a1 = lw(s0 - 0x000C); // 0xFFFFFFF4
  v0 = a0 - a1;
  v0 = v0 & 0xFFF;
  v0 = (int32_t)v0 < 2049;
  temp = v0 == 0;
  if (temp) goto label80031868;
  v0 = spyro_two_angle_diff_12bit(a0, a1);
  mult(v0, s3);
  v1 = lw(s0 - 0x000C); // 0xFFFFFFF4
  t0=lo;
  v0 = (int32_t)t0 >> 13;
  s0 = v1 + v0;
  goto label80031884;
label80031868:
  v0 = spyro_two_angle_diff_12bit(a0, a1);
  mult(v0, s3);
  v1 = lw(s0 - 0x000C); // 0xFFFFFFF4
  t0=lo;
  v0 = (int32_t)t0 >> 13;
  s0 = v1 - v0;
label80031884:
  v0 = lw(0x80078D64);
  v1 = lw(0x80078D58);
  v0 -= v1;
  mult(v0, s3);
  a0 = s0;
  t1=lo;
  v0 = (int32_t)t1 >> 13;
  s2 = v1 + v0;
  v0 = spyro_cos(a0);
  mult(v0, s2);
  a0 = s0;
  s1 = 0x80076DF8;
  s0 = PLAYER_POSITION;
  v0 = lw(s0);
  v1=lo;
  v1 = (int32_t)v1 >> 12;
  v0 += v1;
  sw(s1, v0);
  v0 = spyro_sin(a0);
  mult(v0, s2);
  v0 = lw(0x80078D68);
  a3 = lw(0x80078D5C);
  v1=lo;
  v0 -= a3;
  mult(v0, s3);
  a0 = sp + 24; // 0x0018
  a2 = lw(0x80078D74);
  a1 = s0;
  v0 = lw(PLAYER_POSITION + 0x0004);
  v1 = (int32_t)v1 >> 12;
  v0 += v1;
  sw(0x80076DFC, v0);
  t1=lo;
  v0 = (int32_t)t1 >> 13;
  a3 += v0;
  sw(0x80076E00, a3);
  a2 += 12; // 0x000C
  spyro_vec3_add(a0, a1, a2);
  a0 = sp + 24; // 0x0018
  v0 = lw(sp + 0x0020);
  a1 = 1; // 0x0001
  v0 += 576; // 0x0240
  sw(sp + 0x0020, v0);
  spyro_vec3_shift_right(a0, a1);
  a0 = sp + 24; // 0x0018
  a1 = a0;
  a2 = s1;
  spyro_vec3_sub(a0, a1, a2);
  v0 = lw(0x80078D04);
  v0 = (int32_t)v0 < 32;
  temp = v0 == 0;
  a0 = sp + 24; // 0x0018
  if (temp) goto label80031A88;
  a1 = 0;
  sh(sp + 0x0028, 0);
  v0 = spyro_vec_length(a0, a1);
  a0 = v0;
  a1 = lw(sp + 0x0020);
  a2 = 1; // 0x0001
  a1 = -a1;
  v0 = spyro_atan2(a0, a1, a2);
  a0 = lw(sp + 0x0018);
  a1 = lw(sp + 0x001C);
  a2 = 1; // 0x0001
  sh(sp + 0x002A, v0);
  v0 = spyro_atan2(a0, a1, a2);
  a0 = lh(sp + 0x0028);
  a1 = lh(0x80076E1C);
  sh(sp + 0x002C, v0);
  v0 = spyro_two_angle_signed_diff_12bit(a0, a1);
  v1 = lw(0x80078D04);
  mult(v0, v1);
  a0 = lh(sp + 0x002A);
  a1 = lh(0x80076E1E);
  v0 = lhu(0x80076E1C);
  t0=lo;
  v1 = (int32_t)t0 >> 5;
  v0 += v1;
  v0 = v0 & 0xFFF;
  sh(0x80076E1C, v0);
  v0 = spyro_two_angle_signed_diff_12bit(a0, a1);
  v1 = lw(0x80078D04);
  mult(v0, v1);
  a0 = lh(sp + 0x002C);
  a1 = lh(0x80076E20);
  v0 = lhu(0x80076E1E);
  t0=lo;
  v1 = (int32_t)t0 >> 5;
  v0 += v1;
  v0 = v0 & 0xFFF;
  sh(0x80076E1E, v0);
  v0 = spyro_two_angle_signed_diff_12bit(a0, a1);
  v1 = lw(0x80078D04);
  mult(v0, v1);
  v0 = lhu(0x80076E20);
  t0=lo;
  v1 = (int32_t)t0 >> 5;
  v0 += v1;
  v0 = v0 & 0xFFF;
  goto label80031AC4;
label80031A88:
  sh(0x80076E1C, 0);
  a1 = 0;
  v0 = spyro_vec_length(a0, a1);
  a0 = v0;
  a1 = lw(sp + 0x0020);
  a2 = 1; // 0x0001
  a1 = -a1;
  v0 = spyro_atan2(a0, a1, a2);
  a0 = lw(sp + 0x0018);
  a1 = lw(sp + 0x001C);
  sh(0x80076E1E, v0);
  a2 = 1; // 0x0001
  v0 = spyro_atan2(a0, a1, a2);
label80031AC4:
  sh(0x80076E20, v0);
  s0 = 0x80078D70;
  a0 = lw(s0);
  a1 = lw(s0 - 0x0004); // 0xFFFFFFFC
  v0 = a0 - a1;
  v0 = v0 & 0xFFF;
  v0 = (int32_t)v0 < 2049;
  temp = v0 == 0;
  if (temp) goto label80031B14;
  v0 = spyro_two_angle_diff_12bit(a0, a1);
  mult(v0, s3);
  v0 = lw(s0 - 0x0004); // 0xFFFFFFFC
  t0=lo;
  v1 = (int32_t)t0 >> 13;
  v0 += v1;
  goto label80031B30;
label80031B14:
  v0 = spyro_two_angle_diff_12bit(a0, a1);
  mult(v0, s3);
  v0 = lw(s0 - 0x0004); // 0xFFFFFFFC
  t0=lo;
  v1 = (int32_t)t0 >> 13;
  v0 -= v1;
label80031B30:
  v0 = (int32_t)v0 >> 4;
  sb(PLAYER_DATA + 0x0E, v0);
  a0 = PLAYER_DATA + 0x0C;
  a1 = a0 + 40; // 0x0028
  a2 = 0;
  spyro_mat3_rotation(a0, a1, a2);
  v0 = s3 >> 9;
  sb(PLAYER_DATA + 0x11, v0);
  a0 = 16; // 0x0010
  function_8003CB24();
  function_80049660();
  function_80049E8C();
  a0 = lw(0x800756CC);
  function_80048D10();
  a0 = lw(0x800756CC);
  function_loaded_800756BC();
  goto label800324B8;
label80031BA8:
  a0 = lw(0x80078D38);
  v0 = spyro_cos(a0);
  v1 = lw(0x80078D3C);
  mult(v0, v1);
  s0 = PLAYER_POSITION;
  v0 = lw(s0);
  v1 = lw(0x80078D74);
  t0=lo;
  a0 = (int32_t)t0 >> 12;
  v0 += a0;
  sw(v1 + 0x000C, v0);
  a0 = lw(0x80078D38);
  v0 = spyro_sin(a0);
  v1 = lw(0x80078D3C);
  mult(v0, v1);
  v0 = lw(s0 + 0x0004);
  v1 = lw(0x80078D74);
  t0=lo;
  a0 = (int32_t)t0 >> 12;
  v0 += a0;
  sw(v1 + 0x0010, v0);
  v1 = lw(0x80078D74);
  v0 = lw(0x80078D40);
  a2 = 0;
  sw(v1 + 0x0014, v0);
  v0 = lw(0x80078D74);
  a3 = lw(s0);
  v1 = lw(s0 + 0x0004);
  a0 = lw(v0 + 0x000C);
  a1 = lw(v0 + 0x0010);
  a0 = a3 - a0;
  a1 = v1 - a1;
  v0 = spyro_atan2(a0, a1, a2);
  v1 = lw(0x80078D74);
  sb(v1 + 0x0046, v0);
  v0 = lw(0x80075898);
  temp = v0 == 0;
  if (temp) goto label80031CF0;
  a0 = lw(s1 + 0x48);
  v0 = spyro_cos(a0);
  v1 = lw(s1 + 0x4C);
  mult(v0, v1);
  v0 = lw(s0);
  v1 = lw(0x80075898);
  t0=lo;
  a0 = (int32_t)t0 >> 12;
  v0 += a0;
  sw(v1 + 0x000C, v0);
  a0 = lw(s1 + 0x48);
  v0 = spyro_sin(a0);
  v1 = lw(s1 + 0x4C);
  mult(v0, v1);
  v0 = lw(s0 + 0x0004);
  v1 = lw(0x80075898);
  t0=lo;
  a0 = (int32_t)t0 >> 12;
  v0 += a0;
  sw(v1 + 0x0010, v0);
label80031CF0:
  a0 = lw(0x80078D60);
  v0 = spyro_cos(a0);
  v1 = lw(0x80078D64);
  mult(v0, v1);
  a0 = lw(0x80078D60);
  v0 = lw(s0);
  t0=lo;
  v1 = (int32_t)t0 >> 12;
  v0 += v1;
  sw(0x80076DF8, v0);
  v0 = spyro_sin(a0);
  a0 = lw(0x80078D64);
  v1 = 4; // 0x0004
  sb(PLAYER_DATA + 0x24, v1);
  v1 = lw(0x80078D70);
  sb(PLAYER_DATA + 0x19, 0);
  mult(v0, a0);
  sb(PLAYER_DATA + 0x1F, 0);
  v0 = 4; // 0x0004
  sw(PLAYER_DATA + 0x58, v0);
  v0 = 1; // 0x0001
  a0 = lw(0x80078D68);
  v1 = (int32_t)v1 >> 4;
  sw(s1 - 0x0004, v0); // 0xFFFFFFFC
  sw(s1, 0);
  sb(PLAYER_DATA + 0x0E, v1);
  sw(0x80076E00, a0);
  v0 = lw(s0 + 0x0004);
  t0=lo;
  v1 = (int32_t)t0 >> 12;
  v0 += v1;
  sw(0x80076DFC, v0);
  goto label800324B8;
label80031DBC:
  a0 = lw(PLAYER_DATA + 0x58);
  function_8003CB24();
  function_80049660();
  function_80049E8C();
  a0 = lw(0x800756CC);
  v0 = lw(LEVEL_FRAME_COUNTER);
  v1 = lw(0x800756BC);
  v0++;
  sw(LEVEL_FRAME_COUNTER, v0);
  function_loaded_800756BC();
  v1 = lw(s1 + 0x0008);
  v0 = v1 < 8;
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label800324B8;
  v0 = lw(0x80010E08 + v0); // &0x80031E34
  temp = v0;
  switch (temp)
  {
  case 0x80031E34: // 0
    goto label80031E34;
    break;
  case 0x80032054: // 1
    goto label80032054;
    break;
  case 0x800320C0: // 2
    goto label800320C0;
    break;
  case 0x80032338: // 3 4 5
    goto label80032338;
    break;
  case 0x8003238C: // 6
    goto label8003238C;
    break;
  case 0x8003245C: // 7
    goto label8003245C;
    break;
  default:
    JR(temp, 0x80031E2C);
    return;
  }
label80031E34:
  v1 = lw(BUTTONS);
  v0 = v1 & 0x4000;
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label80031E9C;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002D);
  a3 = 0;
  function_80055A78();
  sw(0x80078D04, 0);
  sw(0x80078D08, lw(0x80078D08)+1);
  if ((int32_t)lw(0x80078D08) >= 3)
    sw(0x80078D08, 0);
  goto label80031EEC;
label80031E9C:
  v0 = v1 & 0x1000;
  temp = v0 == 0;
  a2 = 16; // 0x0010
  if (temp) goto label80031EEC;
  v0 = lw(0x800761D4);
  a0 = lbu(v0 + 0x002D);
  a3 = 0;
  function_80055A78();
  sw(0x80078D04, 0);
  sw(0x80078D08, lw(0x80078D08)-1);
  if ((int32_t)lw(0x80078D08) < 0)
    sw(0x80078D08, 2);
label80031EEC:
  s0 = 0x80078D04;
  v0 = lw(s0);
  v0 = (int32_t)v0 < 8;
  temp = v0 != 0;
  if (temp) goto label800324B8;
  v0 = lw(BUTTONS);
  v0 = v0 & 0x40;
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label800324B8;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002E);
  a3 = 0;
  function_80055A78();
  v1 = lw(0x80078D08);
  temp = v1 != 0;
  v0 = 1; // 0x0001
  if (temp) goto label80031F88;
  v0 = lw(0x80078D14);
  temp = v0 == 0;
  sw(s0, 0);
  if (temp) goto label80031F74;
  v0 = 2; // 0x0002
  sw(0x80078D0C, v0);
  goto label800324B8;
label80031F74:
  v0 = 1; // 0x0001
  sw(0x80078D0C, v0);
  goto label800324B8;
label80031F88:
  temp = v1 != v0;
  if (temp) goto label80032494;
  v0 = lw(0x80078D74);
  v0 = lw(v0);
  v1 = lw(v0);
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 2;
  v0 -= v1;
  v1 = lw(0x80075828);
  v0 = v0 << 3;
  s0 = v1 + v0;
  v1 = lw(s0);
  a0 = lw(v1 + 0x0024);
  v0 = -1;
  temp = a0 != v0;
  if (temp) goto label800324B8;
  v0 = lw(v1 + 0x0018);
  temp = v0 == a0;
  if (temp) goto label800324B8;
  function_8002D02C();
  a0 = s0;
  v0 = 2; // 0x0002
  sb(a0 + 0x48, v0);
  v0 = -1;
  sw(a0 + 0x0008, 0);
  sh(a0 + 0x0034, v0);
  function_8002C924();
  a0 = lw(CONTINUOUS_LEVEL_ID);
  v0 = LEVEL_DRAGONS;
  a0 = a0 << 2;
  a0 += v0;
  v0 = lw(TOTAL_FOUND_DRAGONS);
  v1 = lw(a0);
  v0--;
  v1--;
  sw(TOTAL_FOUND_DRAGONS, v0);
  sw(a0, v1);
  goto label800324B8;
label80032054:
  s0 = 0x80078D04;
  v0 = lw(s0);
  v0 = (int32_t)v0 < 60;
  temp = v0 != 0;
  if (temp) goto label800324B8;
  v0 = lw(BUTTONS);
  v0 = v0 & 0x40;
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label800324B8;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002E);
  a3 = 0;
  function_80055A78();
  v0 = 2; // 0x0002
  sw(s0, 0);
  sw(0x80078D0C, 0);
  sw(0x80078D08, v0);
  goto label800324B8;
label800320C0:
  s1 = 0x80078D08;
  v1 = lw(s1);
  temp = v1 != 0;
  v0 = 1; // 0x0001
  if (temp) goto label800320FC;
  function_800662BC();
  a0 = lw(0x80078D18);
  function_800665B8();
  v0 = 1; // 0x0001
  sw(s1, v0);
  goto label800324B8;
label800320FC:
  temp = v1 != v0;
  v0 = 2; // 0x0002
  if (temp) goto label80032194;
  a0 = 1; // 0x0001
  a1 = s1 + 28; // 0x001C
  a2 = s1 + 32; // 0x0020
  function_80067628();
  temp = v0 == 0;
  if (temp) goto label800324B8;
  v1 = lw(s1 + 0x0020);
  temp = v1 == 0;
  a0 = 3; // 0x0003
  if (temp) goto label80032134;
  temp = v1 != a0;
  v0 = 4; // 0x0004
  if (temp) goto label8003216C;
label80032134:
  a1 = 0x80010DB8; // "BASCUS-94228SPYRO"
  a3 = 0;
  a0 = lw(0x80078D18);
  a2 = lw(0x800785F0);
  v0 = 128; // 0x0080
  sw(sp + 0x0010, v0);
  a2 -= 0x600;
  function_80066E28();
  v0 = 2; // 0x0002
  sw(s1, v0);
  goto label800324B8;
label8003216C:
  temp = v1 != v0;
  if (temp) goto label80032184;
label80032174:
  sw(0x80078D0C, v1);
  goto label80032328;
label80032184:
  sw(0x80078D0C, a0);
  goto label80032328;
label80032194:
  temp = v1 != v0;
  v0 = 3; // 0x0003
  if (temp) goto label80032254;
  a0 = 1; // 0x0001
  a1 = s1 + 28; // 0x001C
  a2 = s1 + 32; // 0x0020
  function_80067628();
  temp = v0 == 0;
  if (temp) goto label800324B8;
  v0 = lw(s1 + 0x0020);
  temp = v0 != 0;
  v1 = 5; // 0x0005
  if (temp) goto label80032244;
  v1 = lhu(lw(0x800785F0) - 0x582); // 0xFFFFFA7E
  v0 = lw(0x80078D20);
  temp = v0 != v1;
  v0 = 4; // 0x0004
  if (temp) goto label80032320;
  s0 = 0x800785F0;
  a0 = lw(s0) - 0x600;
  create_savedata(addr_to_pointer(a0));
  a1 = 0x80010DB8; // "BASCUS-94228SPYRO"
  a0 = lw(0x80078D18);
  v0 = 0x600;
  sw(sp + 0x0010, v0);
  a2 = lw(s0);
  v0 = lw(0x80078D1C);
  a2 -= 0x600;
  a3 = v0 << 1;
  a3 += v0;
  a3 = a3 << 9;
  a3 += 512; // 0x0200
  function_800670E4();
  v0 = 3; // 0x0003
  sw(s1, v0);
  goto label800324B8;
label80032244:
  temp = v0 != v1;
  v0 = 4; // 0x0004
  if (temp) goto label80032174;
  goto label80032320;
label80032254:
  temp = v1 != v0;
  a0 = 1; // 0x0001
  if (temp) goto label800322CC;
  a1 = s1 + 28; // 0x001C
  a2 = s1 + 32; // 0x0020
  function_80067628();
  temp = v0 == 0;
  if (temp) goto label800324B8;
  v0 = lw(s1 + 0x0020);
  temp = v0 != 0;
  v0 = 5; // 0x0005
  if (temp) goto label80032320;
  a1 = 0x80010DB8; // "BASCUS-94228SPYRO"
  a0 = lw(0x80078D18);
  a2 = lw(0x800785F0);
  v0 = 0x600;
  sw(sp + 0x0010, v0);
  v0 = lw(0x80078D1C);
  a2 -= 0x600;
  a3 = v0 << 1;
  a3 += v0;
  a3 = a3 << 9;
  a3 += 512; // 0x0200
  function_80066E28();
  v0 = 4; // 0x0004
  sw(s1, v0);
  goto label800324B8;
label800322CC:
  a1 = s1 + 28; // 0x001C
  a2 = s1 + 32; // 0x0020
  function_80067628();
  temp = v0 == 0;
  if (temp) goto label800324B8;
  v0 = lw(s1 + 0x0020);
  temp = v0 != 0;
  v0 = 5; // 0x0005
  if (temp) goto label80032320;
  v0 = savegame_checksum(addr_to_pointer(lw(0x800785F0) - 0x600));
  v1 = lw(lw(0x800785F0) - 0x74);
  temp = v0 != v1;
  v0 = 5; // 0x0005
  if (temp) goto label80032320;
  v0 = 7; // 0x0007
label80032320:
  sw(0x80078D0C, v0);
label80032328:
  sw(0x80078D04, 0);
  goto label800324B8;
label80032338:
  s0 = 0x80078D04;
  v0 = lw(s0);
  v0 = (int32_t)v0 < 60;
  temp = v0 != 0;
  if (temp) goto label800324B8;
  v0 = lw(BUTTONS);
  v0 = v0 & 0x40;
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label800324B8;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002E);
  a3 = 0;
  function_80055A78();
  v0 = 6; // 0x0006
  goto label80032440;
label8003238C:
  s1 = BUTTONS;
  v0 = lw(s1);
  v0 = v0 & 0x5000;
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label800323D8;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002D);
  a3 = 0;
  function_80055A78();
  a0 = 0x80078D08;
  v1 = lw(a0);
  v0 = 1; // 0x0001
  v0 -= v1;
  sw(a0, v0);
label800323D8:
  s0 = 0x80078D04;
  v0 = lw(s0);
  v0 = (int32_t)v0 < 32;
  temp = v0 != 0;
  if (temp) goto label800324B8;
  v0 = lw(s1);
  v0 = v0 & 0x40;
  temp = v0 == 0;
  if (temp) goto label800324B8;
  function_8006631C();
  v0 = lw(0x80078D08);
  temp = v0 != 0;
  a1 = 0;
  if (temp) goto label80032494;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002E);
  a3 = 0;
  function_80055A78();
  v0 = 2; // 0x0002
label80032440:
  sw(s0, 0);
  sw(0x80078D0C, v0);
  sw(0x80078D08, 0);
  goto label800324B8;
label8003245C:
  v0 = lw(0x80078D04);
  v0 = (int32_t)v0 < 8;
  temp = v0 != 0;
  if (temp) goto label800324B8;
  v0 = lw(BUTTONS);
  v0 = v0 & 0x40;
  temp = v0 == 0;
  if (temp) goto label800324B8;
  function_8006631C();
label80032494:
  function_8002D02C();
  a1 = 0;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002E);
  a3 = 0;
  function_80055A78();
label800324B8:
  ra = lw(sp + 0x40);
  s3 = lw(sp + 0x3C);
  s2 = lw(sp + 0x38);
  s1 = lw(sp + 0x0034);
  s0 = lw(sp + 0x0030);
  sp += 72; // 0x48
  return;
}

// size: 0x000003F4
void function_8003385C(void)
{
  sw(0x8007579C, 0);
  function_8005637C();
  function_8002BBE0();
  switch (lw(GAMESTATE)) {
  case GAMESTATE_INGAME: // in game
    sw(LEVEL_FRAME_COUNTER, lw(LEVEL_FRAME_COUNTER)+1);
    if (lw(IS_DEMO_MODE)) {
      function_800334D4();
      if (v0) return;
    }
    a0 = lw(0x800756CC);
    function_8002A6FC();
    function_loaded_80075734();

    v1 = lw(GAMESTATE);
    if (v1 == GAMESTATE_LOADING || v1 == GAMESTATE_FLIGHT_LEVEL_MENU || v1 == GAMESTATE_FAIRY || v1 == GAMESTATE_BALLOONIST) return;
    
    function_8004A200();

    v1 = lw(GAMESTATE);
    if (v1 == GAMESTATE_LOADING || v1 == GAMESTATE_DEAD || v1 == GAMESTATE_GAMEOVER || v1 == GAMESTATE_FLIGHT_LEVEL_MENU) return;

    a0 = lw(0x800756CC);
    function_loaded_800756BC();
    
    if (lw(0x80075690) == 0)
      function_80054988();
    
    function_80058BD8();
    function_80037BD4();

    if (lw(0x80075918))
      sw(0x80075918, max_int(lw(0x80075918) - lw(0x800756CC), 0));
    
    if (lw(GAMESTATE)) break;
    if (lw(0x8007570C)) break;
    if ((int32_t)lw(PLAYER_DATA + 0x164) < 0) break;
    if (lw(0x80076E28) == 0x8000000E) break;

    if (lw(BUTTONS + 0x0C) < 2 || lw(BUTTONS) & BUTTONS_MASK_START) {
      a0 = 1;
      function_8002C420();
    } else if (lw(BUTTONS) & BUTTONS_MASK_SELECT) {
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
    sw(LEVEL_EGGS + i*4, 0);
    sw(LEVEL_DRAGONS + i*4, 0);
    if ((int32_t)i < 18)
      sw(LEVEL_GEMS + i*4, 0);
    
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

  sw(PLAYER_DATA + 0x164, 3);
  sw(LIVES, 4);
  sw(TOTAL_FOUND_GEMS, 0);
  sw(TOTAL_FOUND_DRAGONS, 0);
  sw(TOTAL_FOUND_EGGS, 0);
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

// size: 0x0000006C
void function_8003E1AC(void)
{
  sp -= 0x28;
  sw(sp + 0x20, s0);
  sw(sp + 0x24, ra);

  s0 = PLAYER_DATA + 0xDC;
  spyro_vec3_copy(s0, PLAYER_DATA + 0xA4);
  a1 = abs_int(lw(PLAYER_DATA + 0x14C));
  spyro_set_vec3_length(s0, abs_int(lw(PLAYER_DATA + 0x14C)));
  spyro_vec3_clear(sp + 0x10);
  spyro_vec3_sub(s0, sp + 0x10, s0);

  ra = lw(sp + 0x24);
  s0 = lw(sp + 0x20);
  sp += 0x28;
  return;
}

// size: 0x00001360
void spyro_change_movestate(uint32_t movestate)
{
  uint32_t old_movestate = lw(PLAYER_MOVESTATE);

  sp -= 80;
  sw(sp + 0x48, ra);
  sw(sp + 0x44, s5);
  sw(sp + 0x40, s4);
  sw(sp + 0x3C, s3);
  sw(sp + 0x38, s2);
  sw(sp + 0x34, s1);
  sw(sp + 0x30, s0);
  switch (old_movestate)
  {
  case MOVESTATE_CHARGE:
  case MOVESTATE_GLIDE:
  case MOVESTATE_FLYING:
  case 44:
    a0 = PLAYER_POSITION;
    a1 = 2;
    function_800562A4();
    break;
  case 7:
    sb(PLAYER_DATA + 0x2B, 0);
#ifdef __GNUC__
    [[fallthrough]];
#endif
  case 14:case 22:case 27:case 28:
    a0 = lbu(lw(0x800761D4) + 0x1B);
    a1 = PLAYER_POSITION;
    a2 = 4;
    a3 = PLAYER_DATA + 0x2A0;
    function_80055A78();
    break;
  case 25:
    a0 = lbu(lw(0x800761D4) + 0x20);
    a1 = PLAYER_POSITION;
    a2 = 4;
    a3 = PLAYER_DATA + 0x2A0;
    function_80055A78();
    break;
  default:
    break;
  }
  
  switch (movestate)
  {
  case MOVESTATE_STAND:
    sw(PLAYER_DATA + 0x7C, 0);
    sw(PLAYER_DATA + 0x14C, -0x80);
    sw(PLAYER_DATA + 0x15C, 1);
    sh(0x80075788, lbu(0x80075274 + (spyro_rand() & 7))*lbu(0x8006C4A2) - 2);
    break;
  case MOVESTATE_STARTING:
  case MOVESTATE_RUN:
  case MOVESTATE_STOP:
  case MOVESTATE_WALK:
    v0 = PLAYER_DATA + 0x7C;
    s0 = v0 + 0x90;
    sw(v0, 0);
    s1 = (spyro_atan2(lw(s0), lw(PLAYER_DATA + 0x110), 1) - lw(PLAYER_DATA + 0x11C)) & 0xFFF;
    if (s1 > 0x400 && s1 < 0xC00) {
      sw(PLAYER_DATA + 0x118, 0);
    } else {
      sw(PLAYER_DATA + 0x118, spyro_vec_length(s0, 0));
    }
    if (movestate == MOVESTATE_STOP) {
      v1 = PLAYER_DATA + 0x118;
      sw(v1, min_int(lw(v1), 0x1400));
    }
    sw(PLAYER_DATA + 0x14C, -0xC00);
    function_8003E1AC();
    if (lw(PLAYER_DATA + 0xB0)) {
      s1 = (lw(PLAYER_DATA + 0x11C) - spyro_atan2(lw(PLAYER_DATA + 0xA4), lw(PLAYER_DATA + 0xA8), 1)) & 0xFFF;

      if ((int32_t)s1 > 0x800)
        s1 -= 0x1000;

      s1 = abs_int(s1);

      v0 = lw(PLAYER_DATA + 0xB0) << 12;
      a0 = (int32_t)v0 / 22;
      if ((int32_t)s1 > 0x400) {
        mult(a0, lw(0x8006C5CC) - lw(0x8006C5C0));
        s1 = 0x800 - s1;
      } else {
        mult(a0, lw(0x8006C5C4) - lw(0x8006C5B8));
      }
      v0 = (int32_t)lo >> 12;
      s2 = v1 + v0;
      v0 = lw(0x8006C5C8);
      v1 = lw(0x8006C5BC);
      v0 -= v1;
      mult(a0, v0);
      v0 = (int32_t)lo >> 12;
      s0 = v1 + v0;
      mult(s2, spyro_cos(s1));
      s2 = (int32_t)lo >> 12;
      mult(s0, spyro_sin(s1));
      s0 = (int32_t)lo >> 12;
      mult(s2, s2);
      v0=lo;
      mult(s0, s0);
      v0 = spyro_sqrt(v0 + lo);
    } else {
      v0 = lw(0x8006C5B8);
    }
    sw(PLAYER_DATA + 0x144, v0);
    v1 = lw(PLAYER_DATA + 0x194);
    sw(PLAYER_DATA + 0x15C, 1);
    if (v1) {
      sw(PLAYER_DATA + 0xC8, 0);
      sw(PLAYER_DATA + 0xCC, lbu(PLAYER_DATA + 0x206)*16);
    }
    break;
  case 4:
    sw(PLAYER_DATA + 0x7C, 0);
    sw(PLAYER_DATA + 0x14C, -0xC00);
    function_8003E1AC();
    sw(PLAYER_DATA + 0x15C, 1);
    break;
  case MOVESTATE_JUMP:
    a0 = PLAYER_DATA + 0xF4;
    spyro_vec3_copy(a0, a0 + 0x18);
    sw(PLAYER_DATA + 0xF0, 0xDC0);
    sw(PLAYER_DATA + 0x14C, -0xC0);
    sw(PLAYER_DATA + 0x7C, 0);
    sw(PLAYER_DATA + 0x154, 0);
    sw(PLAYER_DATA + 0x15C, 0);
    sw(PLAYER_DATA + 0x158, lw(PLAYER_POSITION + 0x08) + ((int32_t)lw(PLAYER_DATA + 0xFC) >> 6));

    v0 = 1;
    if (old_movestate == 29)
      sw(gp + 0x450, 1);
    else
      sw(gp + 0x450, 0);
    break;
  case MOVESTATE_FALL:
    sw(PLAYER_DATA + 0x7C, 0);
    spyro_vec3_copy(PLAYER_DATA + 0xF4, PLAYER_DATA + 0x10C);
    if (lw(PLAYER_DATA + 0x98) == 0 && (int32_t)lw(PLAYER_DATA + 0xB0) < 32)
      function_8003E90C();
      
    sw(PLAYER_DATA + 0x14C, -0xC0);
    sw(PLAYER_DATA + 0x154, 0);
    sw(PLAYER_DATA + 0x15C, 0);
    break;
  case 7:
    sw(PLAYER_DATA + 0x7C, 0);
    s1 = PLAYER_DATA + 0xF4;
    spyro_vec3_clear(s1);
    spyro_vec3_sub(s1, s1, PLAYER_DATA + 0x10C);
    a1 = spyro_vec_length(s1, 1);
    if ((int32_t)a1 > 0x1E00)
      vec3_mul_div(s1, a1, 0x1E00);
      
    sw(PLAYER_DATA + 0x14C, -0x300);
    if (lw(PLAYER_DATA + 0x9C) == 0)
      function_8003E1AC();
    sw(PLAYER_DATA + 0x15C, 1);
    a0 = 5;
    a1 = 10;
    a2 = 0;
    a3 = 0;
    function_loaded_800758E4();
    sw(0x80075764, max_int(lw(0x80075764), 45));
    break;
  case 8:
    sw(PLAYER_DATA + 0x7C, 0);
    sw(PLAYER_DATA + 0x15C, 1);
    break;
  case 9: case 10:
    v0 = PLAYER_DATA + 0x7C;
    sw(v0, 0);
    spyro_vec3_copy(v0 + 0x6C, v0 + 0x90);
    sw(PLAYER_DATA + 0x118, 0);
    sw(PLAYER_DATA + 0x14C, -0xC00);
    function_8003E1AC();
    sw(PLAYER_DATA + 0x15C, 1);
    break;
  case MOVESTATE_CHARGE:
    spyro_vec3_copy(PLAYER_DATA + 0xE8, PLAYER_DATA + 0x10C);
    if (lw(PLAYER_DATA + 0x250)) {
      if ((lw(PLAYER_DATA + 0x7C) & 0x80) == 0) {
        sw(PLAYER_DATA + 0x158, lw(PLAYER_POSITION + 8));
        sw(PLAYER_DATA + 0xC8, max_int(spyro_vec_length(PLAYER_DATA + 0xE8, 1), 0x1F80));
      }
      sw(PLAYER_DATA + 0x7C, 0x81);
    } else {
      sw(PLAYER_DATA + 0x7C, 1);
    }
    s0 = PLAYER_DATA + 0x148;
    sw(s0, 0);
    sw(PLAYER_DATA + 0x14C, -0x240);
    function_8003E1AC();
    function_8003E0B4();
    sw(PLAYER_DATA + 0x15C, 1);

    a0 = lbu(lw(0x800761D4) + 4);
    a1 = PLAYER_POSITION;
    a2 = 4;
    a3 = PLAYER_DATA + 0x2A1;
    function_80055A78();

    v0 = lw(PLAYER_DATA + 0x11C) - lh(0x80076E20);
    sw(gp + 0x052C, 0);
    v1 = v0 & 0xFFF;
    if ((int32_t)v1 > 0x800)
      v1 -= 0x1000;

    if ((int32_t)abs_int(v1) <= 0x200) break;

    struct game_object *object = addr_to_pointer(lw(0x80075828));
    s3 = 30000;
    while ((void *)object < addr_to_pointer(lw(0x80075890))) {
      if ((uint8_t)object->unknown48 < 0x7F) {
        if (object->unknown08) {
          vec3_32 p = vec3_32_sub(object->p, load_vec3_32(PLAYER_POSITION));
          
          v0 = spyro_atan2(p.x, p.y, 1) - lw(PLAYER_DATA + 0x11C);
          v1 = v0 & 0xFFF;
          if ((int32_t)v1 > 0x800)
            v1 -= 0x1000;
          
          s2 = abs_int(v1);
          if ((int32_t)s2 < 0x200) {
            if (abs_int(p.x) + abs_int(p.y) + abs_int(p.z) < 0x4000) {
              a0 = vec3_32_length(p);
              if ((int32_t)a0 < 0x1800) {
                a0 += s2*4;
                if ((int32_t)a0 < (int32_t)s3) {
                  s3 = a0;
                  sw(gp + 0x52C, pointer_to_addr(object));
                }
              }
            }
          }
        }
      }
      object++;
    }
    break;
  case MOVESTATE_CRASH:


    s2 = spyro_rand() & 0x3F;

    for (int i = 0; i < 4; i++) {
      sw(sp + 0x20, 0);
      sw(sp + 0x24, (int32_t)cos_lut[(s2 + 0x40*i) & 0xFF] >> 7);
      sw(sp + 0x28, (int32_t)sin_lut[(s2 + 0x40*i) & 0xFF] >> 7);
      spyro_set_mat_mirrored_vec_multiply(PLAYER_DATA + 0x34, sp + 0x20, sp + 0x20);

      a0 = 1;
      a1 = 33;
      a2 = sp + 0x20;
      a3 = 1;
      function_loaded_800758E4();
    }

    sw(PLAYER_DATA + 0x7C, 0);
    spyro_vec3_clear(PLAYER_DATA + 0xE8);
    spyro_vec3_sub(PLAYER_DATA + 0xF4, PLAYER_DATA + 0xE8, PLAYER_DATA + 0xF4);
    spyro_vec3_shift_right(PLAYER_DATA + 0xF4, 2);
    sw(PLAYER_DATA + 0x14C, -0x300);
    sw(PLAYER_DATA + 0x15C, 1);

    sw(0x8007584C, max_int(lw(0x8007584C), 120));
    sw(0x800757D0, max_int(lw(0x800757D0), 15));
    break;
  case MOVESTATE_EDGE:
    sw(PLAYER_DATA + 0x14C, -0xC00);
    sw(PLAYER_DATA + 0x7C, 0);
    sw(PLAYER_DATA + 0x15C, 1);
    break;
  case 14:
    sw(PLAYER_DATA + 0x7C, 0);
    spyro_vec3_clear(PLAYER_DATA + 0xF4);
    spyro_vec3_sub(PLAYER_DATA + 0xF4, PLAYER_DATA + 0xF4, PLAYER_DATA + 0x10C);
    a1 = spyro_vec_length(PLAYER_DATA + 0xF4, 1);
    if ((int32_t)a1 > 0x1E00)
      vec3_mul_div(PLAYER_DATA + 0xF4, a1, 0x1E00);
    
    sw(PLAYER_DATA + 0x14C, -0x300);
    if (lw(PLAYER_DATA + 0x9C) == 0)
      function_8003E1AC();

    sw(PLAYER_DATA + 0x15C, 1);

    a0 = 5;
    a1 = 10;
    a2 = 0;
    a3 = 0;
    function_loaded_800758E4();

    sw(0x80075904, max_int(lw(0x80075904), 15));
    
    a0 = lbu(lw(0x800761D4) + 0x3B);
    a1 = PLAYER_POSITION;
    a2 = 4;
    a3 = PLAYER_DATA + 0x2A0;
    function_80055A78();

    break;
  case MOVESTATE_GLIDE:
  case 23:
  case MOVESTATE_FLYING:
  case 33: case 34:
    if (old_movestate == MOVESTATE_GLIDE || old_movestate == 23 || old_movestate == MOVESTATE_FLYING || old_movestate == 33 || old_movestate == 34) {
      sw(PLAYER_DATA + 0x15C, 0);
      break;
    }
    sw(PLAYER_DATA + 0x7C, 0);
    sw(PLAYER_DATA + 0x150, 1);
    sw(PLAYER_DATA + 0x148, 0);
    sw(0x80075960, 0);
    sw(0x800758A0, 0);
    sw(0x80075700, 0);

    if ((int32_t)lw(PLAYER_DATA + 0x114) > 0)
      sw(PLAYER_DATA + 0x114, 0);
    sw(PLAYER_DATA + 0x118, clamp_int(spyro_vec_length(PLAYER_DATA + 0xF4, 0), 0x780, 0x1900));

    if (old_movestate != MOVESTATE_FALL || lw(0x80075690))
      sw(PLAYER_DATA + 0x158, max_int(lw(PLAYER_DATA + 0x158), lw(PLAYER_POSITION + 8)));

    if ((int32_t)lw(PLAYER_DATA + 0x120) > 0x400)
      sw(PLAYER_DATA + 0x254, 1);
    else
      sw(PLAYER_DATA + 0x254, 0);

    sw(PLAYER_DATA + 0x14C, -0x80);
    sw(PLAYER_DATA + 0x15C, 0);
    break;
  case 16:
    sw(PLAYER_DATA + 0x7C, 0);
    sw(PLAYER_DATA + 0xF4, 0);
    sw(PLAYER_DATA + 0xF8, 0);
    sw(PLAYER_DATA + 0xFC, lw(PLAYER_DATA + 0x114));
    
    if (lw(PLAYER_DATA + 0x98) == 0 && (int32_t)lw(PLAYER_DATA + 0xB0) < 32)
      function_8003E90C();

    sw(PLAYER_DATA + 0x14C, -0xC0);
    sw(PLAYER_DATA + 0x154, 0);
    sw(PLAYER_DATA + 0x15C, 0);
    break;
  case MOVESTATE_VORTEX:
    if (lw(PLAYER_DATA + 0x224)) {
      spyro_vec3_clear(PLAYER_DATA + 0xF4);
      v1 = lw(PLAYER_DATA + 0x200);
      a0 = lw(PLAYER_DATA + 0x11C);
      sw(PLAYER_DATA + 0x158, v1);
      v1 -= lw(PLAYER_POSITION + 8) + 0x800;
      sw(gp + 0x04C0, v1);
      v1 = ((int32_t)(v1 << 10) >> 10) + 0x400;
      sw(gp + 0x0404, v0);
      v0 = lw(PLAYER_DATA + 0x224);
      sw(gp + 0x528, a0);
      sw(gp + 0x58C, v1);
      v0 = lbu(v0 + 0x46)*16 - a0 - v1;
      v1 = v0 & 0xFFF;
      if ((int32_t)v1 > 0x800)
        v1 -= 0x1000;
      sw(gp + 0x58C, v1);
    } else {
      spyro_vec3_copy(PLAYER_DATA + 0xF4, PLAYER_DATA + 0x10C);
    }
    sw(PLAYER_DATA + 0x14C, 0x80);
    sw(PLAYER_DATA + 0x15C, 0);
    break;
  case MOVESTATE_IDLE_LOOKAROUND:
  case 36: case 37: case 38:
  case MOVESTATE_IDLE_HAIR:
  case 40: case 41:
  case 42: case 43:
    sw(PLAYER_DATA + 0x7C, 0);
    sw(PLAYER_DATA + 0x14C, -0xC00);
    sh(0x80075788, lbu(0x8006C4A2 + movestate*4)*16);
    break;
  case 19:
    sw(PLAYER_DATA + 0x7C, 0);
    spyro_vec3_copy(PLAYER_DATA + 0xE8, PLAYER_DATA + 0x10C);
    sw(PLAYER_DATA + 0xF0, min_int(lw(PLAYER_DATA + 0xF0), 0));
    sw(PLAYER_DATA + 0x14C, -0x200);
    function_8003E1AC();
    function_8003E0B4();
    sw(PLAYER_DATA + 0x15C, 0);
    break;
  case MOVESTATE_CHARGE_JUMP:
    spyro_vec3_copy(PLAYER_DATA + 0xF4, PLAYER_DATA + 0x10C);
    spyro_vec3_copy(PLAYER_DATA + 0xE8, PLAYER_DATA + 0x10C);
    
    sw(PLAYER_DATA + 0x148, 0);
    if (lw(PLAYER_DATA + 0x250)) {
      if ((lw(PLAYER_DATA + 0x7C) & 0x80) == 0) {
        sw(PLAYER_DATA + 0x158, lw(PLAYER_POSITION + 8));
        sw(PLAYER_DATA + 0xC8, max_int(spyro_vec_length(PLAYER_DATA + 0xE8, 1), 0x1F80));
      }
      v1 = PLAYER_DATA + 0x7C;
      sw(v1, lw(v1) | 0x80);
    } else {
      sw(PLAYER_DATA + 0x7C, 0);
      sw(PLAYER_DATA + 0xFC, 0);
      sw(PLAYER_DATA + 0xF0, 0);
    }
    sw(PLAYER_DATA + 0x14C, 0x240);
    sw(PLAYER_DATA + 0x154, 0);
    sw(PLAYER_DATA + 0x15C, 0);
    break;
  case 22: case 28:
    sw(PLAYER_DATA + 0xF4, lw(PLAYER_DATA + 0x208) << 6);
    sw(PLAYER_DATA + 0x7C, 0);
    sw(PLAYER_DATA + 0xF8, lw(PLAYER_DATA + 0x20C) << 6);
    v1 = lw(PLAYER_DATA + 0x210);
    v0 = v1 << 6;
    if (v1 == 0)
      v0 = lw(PLAYER_DATA + 0x114);
    sw(PLAYER_DATA + 0xFC, v0);
    v0 = spyro_atan2(lw(PLAYER_DATA + 0xF4), lw(PLAYER_DATA + 0xF8), 2);
    v1 = lw(PLAYER_DATA + 0x9C);
    sw(PLAYER_DATA + 0x14C, -0x300);
    if (v1 == 0)
      function_8003E1AC();

    sw(PLAYER_DATA + 0x15C, 1);

    a0 = 5;
    a1 = 10;
    a2 = 0;
    a3 = 0;
    function_loaded_800758E4();

    a0 = lbu(lw(0x800761D4) + 0x3B);
    a1 = PLAYER_POSITION;
    a2 = 4;
    a3 = PLAYER_DATA + 0x2A0;
    function_80055A78();
    sw(0x80075904, max_int(lw(0x80075904), 15));
    break;
  case 24:
    spyro_vec3_copy(PLAYER_DATA + 0xF4, PLAYER_DATA + 0x10C);
    spyro_vec3_copy(PLAYER_DATA + 0xE8, s1);
    sw(PLAYER_DATA + 0x148, 0);
    sw(PLAYER_DATA + 0x07C, 0xC0);
    sw(PLAYER_DATA + 0x14C, 0);
    sw(PLAYER_DATA + 0x154, 0);
    break;
  case 25:
    sw(PLAYER_DATA + 0x7C, 0);
    spyro_vec3_clear(PLAYER_DATA + 0xF4);

    sw(PLAYER_DATA + 0xFC, -0xC00);
    sw(PLAYER_DATA + 0x15C, 1);

    a0 = 5;
    a1 = 10;
    a2 = 0;
    a3 = 0;
    function_loaded_800758E4();

    sw(0x80075904, max_int(lw(0x80075904), 15));

    a0 = lbu(lw(0x800761D4) + 0x3A);
    a1 = PLAYER_POSITION;
    a2 = 4;
    a3 = PLAYER_DATA + 0x2A0;
    function_80055A78();
    break;
  case 26:
    sw(PLAYER_DATA + 0x7C, 0);
    v0 = (spyro_atan2(lw(PLAYER_DATA + 0x10C), lw(PLAYER_DATA + 0x110), 1) - lw(PLAYER_DATA + 0x11C)) & 0xFFF;
    if (v0 > 0x400 && v0 < 0xC00)
      sw(PLAYER_DATA + 0x118, 0);
    else
      sw(PLAYER_DATA + 0x118, spyro_vec_length(PLAYER_DATA + 0x10C, 0));
    
    sw(PLAYER_DATA + 0x14C, -0xC00);
    function_8003E1AC();
    sw(PLAYER_DATA + 0x144, 0x280);
    sw(0x800758A0, 0);
    sw(PLAYER_DATA + 0x15C, 1);
    break;
  case 27:
    sw(PLAYER_DATA + 0x7C, 0);
    spyro_vec3_clear(PLAYER_DATA + 0xF4);

    sw(PLAYER_DATA + 0x14C, -0x300);
    sw(PLAYER_DATA + 0x15C, 1);

    a0 = 5;
    a1 = 10;
    a2 = 0;
    a3 = 0;
    function_loaded_800758E4();

    sw(0x80075904, max_int(lw(0x80075904), 15));
    break;
  case 29:
    sw(PLAYER_DATA + 0x7C, 0);
    sw(PLAYER_DATA + 0x14C, -0x80);
    sw(PLAYER_DATA + 0x15C, 1);
    if (lw(PLAYER_DATA + 0x160) == 0) {
      a0 = 5;
      a1 = 10;
      a2 = 0;
      a3 = 0;
      function_loaded_800758E4();
    }
    a0 = lw(0x8006EA34 + lw(0x80075728)*4);
    a1 = 0;
    function_loaded_800758CC();

    a0 = lbu(lw(0x800761D4) + 0x1D);
    a1 = PLAYER_POSITION;
    a2 = 4;
    a3 = PLAYER_DATA + 0x2A0;
    function_80055A78();

    a0 = lbu(lw(0x800761D4) + 0x1B);
    a1 = PLAYER_POSITION;
    a2 = 4;
    a3 = PLAYER_DATA + 0x2A0;
    function_80055A78();
    sw(0x80075904, max_int(lw(0x80075904), 15));
    break;
  case 30:
    sw(PLAYER_DATA + 0x7C, 0);
    spyro_vec3_copy(PLAYER_DATA + 0xE8, PLAYER_DATA + 0x10C);
    sw(PLAYER_DATA + 0x14C, -0x300);
    if (lw(PLAYER_DATA + 0x9C) == 0)
      function_8003E1AC();
    sw(PLAYER_DATA + 0x15C, 1);
    break;
  case 31:
    sw(PLAYER_DATA + 0x7C, 0);
    spyro_vec3_copy(PLAYER_DATA + 0xE8, PLAYER_DATA + 0x10C);
    sw(PLAYER_DATA + 0x14C, -0x300);
    if (lw(PLAYER_DATA + 0x9C) == 0)
      function_8003E1AC();

    sw(PLAYER_DATA + 0x15C, 1);

    a0 = 5;
    a1 = 10;
    a2 = 0;
    a3 = 0;
    function_loaded_800758E4();
    break;
  case 35:
    sw(PLAYER_DATA + 0x14C, -0x80);
    sw(PLAYER_DATA + 0x7C, 0);
    sw(PLAYER_DATA + 0x15C, 1);
    break;
  case 44:
    spyro_vec3_copy(PLAYER_DATA + 0xE8, PLAYER_DATA + 0x10C);
    if ((lw(PLAYER_DATA + 0x7C) & 0x80) == 0) {
      sw(PLAYER_DATA + 0x158, lw(PLAYER_POSITION + 8));
      sw(PLAYER_DATA + 0xC8, max_int(spyro_vec_length(PLAYER_DATA + 0xE8, 1), 0x1F80));
    }
    sw(PLAYER_DATA + 0x7C, 0xC0);
    sw(PLAYER_DATA + 0x148, 0);
    sw(PLAYER_DATA + 0x14C, -0x240);
    function_8003E1AC();
    function_8003E0B4();
    sw(PLAYER_DATA + 0x15C, 1);
    sw(0x80075700, 0);
    a0 = lbu(lw(0x800761D4) + 5);
    a1 = PLAYER_POSITION;
    a2 = 4;
    a3 = PLAYER_DATA + 0x2A1;
    function_80055A78();
    break;
  default:
    break;
  }
  
  sw(PLAYER_MOVESTATE, movestate);
  sw(PLAYER_DATA + 0x84, lw(PLAYER_DATA + 0x80));
  sw(PLAYER_DATA + 0x80, 0);
  sw(PLAYER_DATA + 0x58, lbu(0x8006C4A3 + lbu(0x8006C470 + movestate)*4));

  ra = lw(sp + 0x48);
  s5 = lw(sp + 0x44);
  s4 = lw(sp + 0x40);
  s3 = lw(sp + 0x3C);
  s2 = lw(sp + 0x38);
  s1 = lw(sp + 0x34);
  s0 = lw(sp + 0x30);
  sp += 80; // 0x0050
  return;
}

void function_8003EA68(void)
{
  spyro_change_movestate(a0);
}

// size: 0x00000078
void function_8003FDC8(void)
{
  uint32_t movestate = a0;
  spyro_change_movestate(movestate);

  v1 = lbu(0x8006C470 + movestate);
  sb(PLAYER_DATA + 0x1E, 0);
  sb(PLAYER_DATA + 0x18, v1);
  sb(PLAYER_DATA + 0x1F, 1);
  sb(PLAYER_DATA + 0x24, 0);
  sw(PLAYER_DATA + 0x54, 0);
  sw(PLAYER_DATA + 0x5C, movestate);
  sb(PLAYER_DATA + 0x19, v1);
  
}

// size: 0x0000004C
void function_8002C618(void)
{
  a0 = 0;
  function_80056B28();
  v0 = 10; // 0x000A
  sw(GAMESTATE, v0); // &0x00000000
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
  sw(GAMESTATE, 1);
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
  sw(GAMESTATE, 0);
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
  v0 = lw(LIVES);
  if (v0) {
    sw(LIVES, v0-1);
    v0 = GAMESTATE_DEAD;
  } else
    v0 = GAMESTATE_GAMEOVER;
  
  sw(GAMESTATE, v0);
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
  sw(0x80077888 + 0x04, lw(LEVEL_DRAGONS + v1));
  sw(0x80077888 + 0x0C, 0);
  sw(0x80077888 + 0x08, lw(LEVEL_GEMS + v1));
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
    
    sb(gp + 0x604, 0);

    function_8003385C();

    sb(gp + 0x604, 1);
    sw(gp + 0x468, lw(gp + 0x4FC));

    if (lw(gp + 0x468) < 2)
      sw(gp + 0x468, 2);

    if (lw(gp + 0x468) > 4)
      sw(gp + 0x468, 4);
    
    sw(gp + 0x4FC, 0);
    if (lw(gp + 0x538) == 0) {
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