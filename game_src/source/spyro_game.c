#include "spyro_game.h"
#include "debug.h"
#include "main.h"
#include "decompilation.h"
#include "psx_mem.h"
#include "temporary.h"
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
  v1 = object->unknown3E*8;
  v0 += v1;
  v1 = lw(v0 + 0x24);
  v1 = (v1 >> 19) & 0x1C;
  v0 = at + v1;
  v0 = lw(v0 + 0x14);
  at = lw(at + 0x14);
  object->unknown08 = v0;
  object->unknown34 = -1;
  if (at) {
    v0 = object->p.x >> 13;
    v1 = object->p.y >> 13;
    v0 = v0 + (v1 << 5);
    if ((int32_t)at >= 0)
      v0 += 1 << 10;
    object->unknown34 = v0;
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

    a2 = (int32_t)object->unknown34;
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
      object->unknown34 = at;
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

/*
// size: 0x00000380
// struct game_object *a0
void function_800529E4(void)
{
  struct game_object *object = addr_to_pointer(a0);

  if (a1 & 1) {
    at = lw(0x80076378 + object->modelID*4);
    v1 = object->unknown3C;
    v0 = v1;
    if (object->unknown40) {
      v0 = (v1 >> 8) & 0xFF;
      v1 = v1 >> 24;
    } else {
      v1 = (v1 >> 16) & 0xFF;
    }
    v0 = lw(at + 0x38 + v0*4) + v1*8;
    v1 = (lw(v0 + 0x24) >> 19) & 0x1C;
    v0 = lw(at + v1 + 0x14);
    object->unknown08 = v0;
  }

  if (a1 & 2) {
    at = object->p.x >> 13;
    v0 = object->p.y >> 13;
    at = at + (v0 << 5);
    v1 = lw(0x80075778);

    a2 = (int32_t)object->unknown34;
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
      object->unknown34 = at;
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
}

*/

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
  sw(0x800785A8 + 0x18, struct1_elements);
  sw(0x800785A8 + 0x1C, struct1_elements + struct1_len*16);
  sw(0x800785A8 + 0x20, struct1_len);

  struct_base = struct_base + lw(struct_base);

  const uint32_t struct2 = struct_base + 0x04;
  const int32_t struct2_len = lw(struct2);
  const uint32_t struct2_elements = struct2 + 0x04;
  sw(0x800785A8 + 0x00, struct2_elements);
  sw(0x800785A8 + 0x04, struct2_len);
  
  for (int i = 0; i < struct2_len; i++)
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
      sw(0x800785A8 + 0x08, 0);
      sw(0x800785A8 + 0x0C, 0);
      sw(0x8007778C, 0);
    }

    struct_base = struct_base + lw(struct_base);

    const uint32_t struct4 = struct_base + 0x04;
    const uint32_t struct4_len = lw(struct4);
    const uint32_t struct4_elements = struct4 + 0x04;
    sw(0x800785A8 + 0x10, struct4_elements);
    sw(0x800785A8 + 0x14, struct4_len);
    for (int i = 0; i < struct4_len; i++)
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
    if (lhu(0x80075F3E + i*0x1C) & 0x83) {
      if ((a0 >> i) & 1) {
        v1 = 1 << i;
        sw(0x8007623C, lw(0x8007623C) | v1);
        v1 = lhu(0x80075F3E + i*0x1C) & 0x1C;
        if (v1 != 8) {
          if (v1 == 4) {
            v0 = lw(0x80075F30 + i*0x1C);
            if (v0) sb(v0 + 0x2A0, 0x7F);
          }
        } else {
          v0 = lw(0x80075F30 + i*0x1C);
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
  sw(0x8007579C, 0); // &0x00000000
  function_8005637C();
  function_8002BBE0();
  switch (lw(0x800757D8)) {
  case 0: // in game
    sw(level_frame_counter, lw(level_frame_counter)+1);
    if (lw(IS_DEMO_MODE)) {
      function_800334D4();
      if (v0) return;
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
      a0 = 1;
      function_8002C420();
    } else if (lw(buttons_press) & 0x100) {
      a0 = 1;
      function_8002C714();
    }
    break;
  case 1: // loading level
    function_8002DF9C();
    break;
  case 2: // pause menu
    function_8002E12C();
    break;
  case 3: // inventory
    function_8002EB2C();
  case 4: // dead
  case 5:
    function_8002EDF0();
    break;
  case 6:
    // function_8002F3C4(); // this does nothing
    UNREACHABLE;
    break;
  case 7: // flight level crashed
    if(lw(0x800757C0) != 0x8007B1FC) BREAKPOINT;
    function_8007B1FC();
    break;
  case 8: // freed dragon
    function_8002F3E4();
    break;
  case 9: // landing after loading
    function_8002E000();
    break;
  case 10: // before loading ?
    function_8002E084();
    break;
  case 11: // fairy menu
    function_800314B4();
    break;
  case 12: // balloonist
    function_800324D8();
    break;
  case 13: // title screen / game intro
    if (lw(0x80078D78) != 3)
      function_8007ABAC_title_screen();
    else
      function_80032B08();
    break;
  case 14: // credits
    function_800331AC();
    break;
  case 15: // more credits
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
    sw(0x80076FE8 + i*4, 0);
    sw(0x800772D8 + i*4, 0);
    if ((int32_t)i < 18)
      sw(0x80077420 + i*4, 0);
    
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

// size: 0x00001360
void function_8003EA68(void)
{
  uint32_t temp;
  v0 = lw(player_movestate);
  sp -= 80; // 0xFFFFFFB0
  sw(sp + 0x0044, s5);
  s5 = a0;
  sw(sp + 0x0048, ra);
  sw(sp + 0x0040, s4);
  sw(sp + 0x003C, s3);
  sw(sp + 0x0038, s2);
  sw(sp + 0x0034, s1);
  v1 = v0 - 7; // 0xFFFFFFF9
  v0 = v1 < 38;
  temp = v0 == 0;
  sw(sp + 0x0030, s0);
  if (temp) goto label8003EB1C;
  v0 = v1 << 2;
  v0 = lw(0x80010E88 + v0); // &0x8003EAD4
  temp = v0;
  switch (temp)
  {
  case 0x8003EAD4: // 0
    goto label8003EAD4;
    break;
  case 0x8003EB1C: // 1 2 3 5 6 9 10 11 12 13 14 16 17 19 22 23 24 26 27 28 29 30 31 32 33 34 35 36
    goto label8003EB1C;
    break;
  case 0x8003EABC: // 4 8 25 37
    goto label8003EABC;
    break;
  case 0x8003EADC: // 7 15 20 21
    goto label8003EADC;
    break;
  case 0x8003EAFC: // 18
    goto label8003EAFC;
    break;
  default:
    JR(temp, 0x8003EAB4);
    return;
  }
label8003EABC:
  a0 = player_position;
  a1 = 2; // 0x0002
  function_800562A4();
  v0 = s5 < 45;
  goto label8003EB20;
label8003EAD4:
  sb(0x80078A83, 0);
label8003EADC:
  a1 = player_position;
  v0 = lw(0x800761D4);
  a0 = lbu(v0 + 0x001B);
  a2 = 4; // 0x0004
  goto label8003EB14;
label8003EAFC:
  a1 = player_position;
  v0 = lw(0x800761D4);
  a2 = 4; // 0x0004
  a0 = lbu(v0 + 0x0020);
label8003EB14:
  a3 = a1 + 672; // 0x02A0
  function_80055A78();
label8003EB1C:
  v0 = s5 < 45;
label8003EB20:
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
  function_8006272C();
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
  function_8006272C();
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

// size: 0x00000088
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