

#include "main.h"
#include "psx_mem.h"
#include "psx_ops.h"
#include "psx_bios.h"
#include "spyro_print.h"
#include "spyro_string.h"
#include "spyro_math.h"
#include "decompilation.h"
#include "spyro_cdrom.h"
#include "spyro_spu.h"
#include "spyro_psy.h"
#include "function_chooser.h"
#include "not_renamed.h"
#include "spyro_game.h"
#include "level_loading.h"

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

// size: 0x0000060C
void function_80014564(void)
{
  function_800163E4();

  if (lw(0x80076BB8)) return;
  
  if (CdSync(1, NULL) != CdlComplete || (lw(0x800774B4) & 0x40) == 0) return;
  
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
    function_8005CB24();
    a0 = lw(lib_end_ptr);
    a1 = 0x0007EFF0;
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

// size: 0x00000800
void function_80014B70(void)
{
  uint32_t temp;
  sp -= 88; // 0xFFFFFFA8
  sw(sp + 0x0054, ra);
  sw(sp + 0x0050, s4);
  sw(sp + 0x004C, s3);
  sw(sp + 0x0048, s2);
  sw(sp + 0x0044, s1);
  sw(sp + 0x0040, s0);

  function_800163E4();

  if (lw(0x80076BB8) || CdSync(1, NULL) != CdlComplete || (lw(0x800774B4) & 0x40) == 0) goto label8001534C;

  v1 = lw(0x8007705C);
  switch (v1)
  {
  case 0:
    v0 = lw(0x80077054);
    a0 = lw(0x80076760);
    v1 = lw(0x800785E0);
    v0 = v0 << 3;
    a1 = lw(WAD_nested_header + 0x0024 + v0);
    a0 -= v1;
    v0 = (int32_t)a0 < (int32_t)a1;
    temp = v0 != 0;
    v0 = a0;
    if (temp) goto label80014C54;
    sw(0x80077068, a1);
    sw(0x80077064, 0);
    goto label80014C7C;
  label80014C54:
    temp = (int32_t)v0 >= 0;
    if (temp) goto label80014C60;
    v0 += 2047; // 0x07FF
  label80014C60:
    v0 = (int32_t)v0 >> 11;
    v0 = v0 << 11;
    sw(0x80077068, v0);
    v0 = 1; // 0x0001
    sw(0x80077064, v0);
  label80014C7C:
    a0 = lw(WAD_sector);
    a1 = lw(0x800785E0);
    a2 = lw(0x80077068);
    v0 = lw(0x80077054);
    v1 = lw(CONTINUOUS_LEVEL_ID);
    v0 = v0 << 3;
    v1 = v1 << 4;
    t0 = lw(WAD_nested_header + 0x0020 + v0);
    a3 = lw(WAD_header + 0x0050 + v1);
    v0 = 600; // 0x0258
    sw(sp + 0x0010, v0);
    a3 += t0;
    read_disk2(a0, a1, a2, a3, lw(sp+0x10));
    goto label80015008;
    break;
  case 1:
    goto label80014CDC;
    break;
  case 2:
    goto label80014E7C;
    break;
  case 3:
    goto label800150D4;
    break;
  case 4:
    goto label800151A8;
    break;
  case 5:
    goto label80015258;
    break;
  default:
    goto label8001534C;
    break;
  }
label80014CDC:
  a0 = 0x80077030;
  s0 = 0x800785E0;
  a1 = lw(s0 + 0x0000);
  a2 = 36; // 0x0024
  spyro_memcpy32(a0, a1, a2);
  v0 = lw(0x80077064);
  temp = v0 != 0;
  v0 = 0x2AAA0000;
  if (temp) goto label80014D94;
  a1 = lw(0x80077050);
  v0 = v0 | 0xAAAB;
  mult(a1, v0);
  a0 = lw(0x8007703C);
  v1 = lw(0x8007704C);
  v0 = lw(s0 + 0x0000);
  a1 = (int32_t)a1 >> 31;
  a0 += v0;
  v0 += v1;
  sw(0x80077078, v0);
  t1=hi;
  v0 = (int32_t)t1 >> 2;
  v0 -= a1;
  sw(0x8007707C, v0);
  v0 = func_800133E0(a0);
  v1 = lw(s0 + 0x0000);
  a0 = lw(0x80077044);
  sw(0x80076B70, v0);
  a0 += v1;
  v0 = func_800133E0(a0);
  sw(0x80076B74, v0);
  sw(0x80077060, 0);
  goto label80015334;
label80014D94:
  v0 = lw(0x80077038);
  a0 = 0x00080000;
  a0 -= v0;
  function_8005CB24();
  v0 = lw(s0 + 0x0000);
  a0 = lw(0x80077034);
  a1 = lw(0x80077038);
  a0 += v0;
  function_8005CAC4();
label80014DC4:
  a0 = 0;
  v0 = test_spu_event(a0);
  temp = v0 == 0;
  if (temp) goto label80014DC4;
  s1 = 0x800785E0;
  s0 = 0x8007703C;
  a0 = lw(s1 + 0x0000);
  v0 = lw(s0 + 0x0000);
  a2 = lw(0x80077068);
  a1 = a0 + v0;
  a2 -= v0;
  spyro_memcpy32(a0, a1, a2);
  a0 = lw(WAD_sector);
  a1 = lw(s1 + 0x0000);
  t0 = lw(0x80077068);
  v0 = lw(s0 + 0x0000);
  v1 = lw(CONTINUOUS_LEVEL_ID);
  a1 += t0;
  a1 -= v0;
  v1 = v1 << 4;
  v0 = lw(0x80077054);
  v1 = lw(WAD_header + 0x0050 + v1);
  v0 = v0 << 3;
  a2 = lw(WAD_nested_header + 0x0024 + v0);
  a3 = lw(WAD_nested_header + 0x0020 + v0);
  v0 = 600; // 0x0258
  sw(sp + 0x0010, v0);
  a2 -= t0;
  a3 += v1;
  a3 += t0;
  read_disk2(a0, a1, a2, a3, lw(sp+0x10));
  goto label80015334;
label80014E7C:
  v0 = lw(0x80077058);
  v0 = (int32_t)v0 < 2;
  temp = v0 != 0;
  if (temp) goto label8001534C;
  v0 = lw(0x80077064);
  temp = v0 != 0;
  v0 = 0x2AAA0000;
  if (temp) goto label80015028;
  v0 = 0x00080000;
  a0 = lw(0x80077060);
  v1 = lw(0x80077038);
  a0 = a0 << 15;
  a0 += v0;
  a0 -= v1;
  function_8005CB24();
  v1 = lw(BACKBUFFER_DISP);
  v0 = DISP1;
  temp = v1 != v0;
  v1 = 0 | 0x8000;
  if (temp) goto label80014EF4;
  s2 = lw(DISP2 + 0x0070);
  goto label80014EFC;
label80014EF4:
  s2 = lw(DISP1 + 0x0070);
label80014EFC:
  v0 = lw(0x80077060);
  a0 = lw(0x80077038);
  v0 = v0 << 15;
  v0 += v1;
  v0 = (int32_t)a0 < (int32_t)v0;
  temp = v0 == 0;
  s1 = 0 | 0x8000;
  if (temp) goto label80014F38;
  temp = (int32_t)a0 >= 0;
  v0 = a0;
  if (temp) goto label80014F2C;
  v0 = a0 + 32767; // 0x7FFF
label80014F2C:
  s1 = (int32_t)v0 >> 15;
  v0 = s1 << 15;
  s1 = a0 - v0;
label80014F38:
  a0 = s2;
  a1 = s1;
  function_8005CA64();
label80014F44:
  a0 = 0;
  v0 = test_spu_event(a0);
  temp = v0 == 0;
  v0 = 0x00080000;
  if (temp) goto label80014F44;
  s0 = 0x80077060;
  a0 = lw(s0 + 0x0000);
  v1 = lw(0x80077038);
  a0 = a0 << 15;
  a0 += v0;
  a0 -= v1;
  function_8005CB24();
  a1 = s1;
  v0 = lw(0x800785E0);
  v1 = lw(0x80077034);
  a0 = lw(s0 + 0x0000);
  v0 += v1;
  a0 = a0 << 15;
  a0 += v0;
  function_8005CAC4();
label80014FA0:
  a0 = 0;
  v0 = test_spu_event(a0);
  temp = v0 == 0;
  a1 = s2;
  if (temp) goto label80014FA0;
  a2 = s1;
  v0 = lw(0x800785E0);
  v1 = lw(0x80077034);
  a0 = lw(0x80077060);
  v0 += v1;
  a0 = a0 << 15;
  a0 += v0;
  spyro_memcpy32(a0, a1, a2);
  v0 = lw(0x80077060);
  v1 = lw(0x80077038);
  v0++;
  sw(0x80077060, v0);
  v0 = v0 << 15;
  v0 = (int32_t)v0 < (int32_t)v1;
  temp = v0 != 0;
  if (temp) goto label8001534C;
label80015008:
  v0 = lw(0x8007705C);
  v0++;
  sw(0x8007705C, v0);
  goto label8001534C;
label80015028:
  a1 = lw(0x80077050);
  v0 = v0 | 0xAAAB;
  mult(a1, v0);
  s0 = 0x800785E0;
  a0 = lw(s0 + 0x0000);
  v1 = lw(0x8007704C);
  v1 += a0;
  v0 = lw(0x8007703C);
  a1 = (int32_t)a1 >> 31;
  v1 -= v0;
  sw(0x80077078, v1);
  t1=hi;
  v0 = (int32_t)t1 >> 2;
  v0 -= a1;
  sw(0x8007707C, v0);
  v0 = func_800133E0(a0);
  sw(0x80076B70, v0);
  a0 = lw(s0 + 0x0000);
  v0 = lw(0x80077044);
  v1 = lw(0x8007703C);
  a0 += v0;
  a0 -= v1;
  v0 = func_800133E0(a0);
  v1 = lw(0x8007705C);
  sw(0x80076B74, v0);
  v1++;
  sw(0x8007705C, v1);
  goto label8001534C;
label800150D4:
  v0 = lw(0x80077058);
  v0 = (int32_t)v0 < 6;
  temp = v0 != 0;
  v0 = 1; // 0x0001
  if (temp) goto label8001534C;
  v1 = lw(0x80077064);
  temp = v1 != v0;
  if (temp) goto label80015334;
  v1 = lw(0x80077038);
  v0 = v1 + 6159; // 0x180F
  temp = (int32_t)v0 >= 0;
  v0 = (int32_t)v0 >> 11;
  if (temp) goto label80015120;
  v0 = v1 + 8206; // 0x200E
  v0 = (int32_t)v0 >> 11;
label80015120:
  t0 = v0 << 11;
  a2 = lw(WAD_nested_header + 0x0004);
  v1 = 0x00100000;
  sw(0x80077068, t0);
  v0 = v1 - a2;
  v0 = (int32_t)v0 < (int32_t)t0;
  temp = v0 == 0;
  t0 -= v1;
  if (temp) goto label80015198;
  a2 += t0;
  a0 = lw(WAD_sector);
  v0 = lw(CONTINUOUS_LEVEL_ID);
  a1 = lw(0x800785E0);
  a3 = lw(WAD_nested_header);
  v0 = v0 << 4;
  v1 = lw(WAD_header + 0x0050 + v0);
  v0 = 600; // 0x0258
  sw(sp + 0x0010, v0);
  a3 += v1;
  a3 -= t0;
  read_disk2(a0, a1, a2, a3, lw(sp+0x10));
  goto label80015334;
label80015198:
  sw(0x80077064, 0);
  goto label80015334;
label800151A8:
  v1 = lw(0x80077064);
  v0 = 1; // 0x0001
  temp = v1 != v0;
  v0 = 0x00080000;
  if (temp) goto label80015208;
  a0 = lw(0x80077068);
  v0 = v0 | 0x1010;
  a0 = v0 - a0;
  function_8005CB24();
  v1 = 0xFFF00000;
  a0 = lw(0x800785E0);
  a1 = lw(0x80077068);
  v0 = lw(WAD_nested_header + 0x0004);
  a1 += v1;
  a1 += v0;
  function_8005CAC4();
label800151F8:
  a0 = 0;
  v0 = test_spu_event(a0);
  temp = v0 == 0;
  if (temp) goto label800151F8;
label80015208:
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
  read_disk2(a0, a1, a2, a3, lw(sp+0x10));
  goto label80015334;
label80015258:
  s1 = 0;
  s0 = 0x800785E0;
  a0 = lw(WAD_nested_header + 0x0050);
  v0 = lw(WAD_nested_header + 0x0010);
  v1 = lw(s0 + 0x0000);
  a0 -= v0;
  a0 += v1;
  function_80013230();
  a1 = 0x00FFFFFF;
  s0 += 28; // 0x001C
  a0 = 0x80076378;
label80015298:
  v1 = lw(a0 + 0x0000);
  v0 = lw(s0 + 0x0000);
  v1 = v1 & a1;
  v0 = v0 & a1;
  v1 = (int32_t)v1 < (int32_t)v0;
  temp = v1 == 0;
  if (temp) goto label800152B8;
  sw(a0 + 0x0000, 0);
label800152B8:
  s1++;
  v0 = (int32_t)s1 < 512;
  temp = v0 != 0;
  a0 += 4; // 0x0004
  if (temp) goto label80015298;
  s1 = 1; // 0x0001
  s4 = 0x80076378;
  s3 = WAD_nested_header + 0x0050;
  s2 = s3 + 258; // 0x0102
  s0 = s3 + 4; // 0x0004
label800152E4:
  v1 = lw(s0 + 0x0000);
  temp = (int32_t)v1 <= 0;
  if (temp) goto label80015334;
  s0 += 4; // 0x0004
  s1++;
  a0 = lw(s3 - 0x0040); // 0xFFFFFFC0
  v0 = lw(0x800785E0);
  a0 = v1 - a0;
  a0 += v0;
  v0 = func_800133E0(a0);
  v1 = lh(s2 + 0x0000);
  v1 = v1 << 2;
  v1 += s4;
  sw(v1 + 0x0000, v0);
  v0 = (int32_t)s1 < 64;
  temp = v0 != 0;
  s2 += 2;
  if (temp) goto label800152E4;
label80015334:
  v0 = lw(0x8007705C)+1;
  sw(0x8007705C, v0);
label8001534C:
  ra = lw(sp + 0x0054);
  s4 = lw(sp + 0x0050);
  s3 = lw(sp + 0x004C);
  s2 = lw(sp + 0x0048);
  s1 = lw(sp + 0x0044);
  s0 = lw(sp + 0x0040);
  sp += 88; // 0x0058
  return;
}

// size: 0x00001074
void function_80015370(void)
{
  uint32_t temp;
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
    function_800163E4();
    if (lw(0x80076BB8)) goto label800163BC;

    if (CdSync(1, NULL) != CdlComplete) goto label800163BC;
    if ((lw(0x800774B4) & 0x40) == 0) goto label800163BC;
  }
  
  switch (lw(0x80075864))
  {
  case 0:
    goto label80015420;
    break;
  case 1:
    goto label800155B4;
    break;
  case 2:
    goto label800155E4;
    break;
  case 3:
    goto label800156B4;
    break;
  case 4:
    goto label800156FC;
    break;
  case 5:
    goto label80015770;
    break;
  case 6:
    goto label8001583C;
    break;
  case 7:
    goto label80015868;
    break;
  case 8:
    goto label80015888;
    break;
  case 9:
    goto label800158E0;
    break;
  case 10:
    goto label80015A54;
    break;
  case 11:
    goto label80015B80;
    break;
  case 12:
    goto label80015BD8;
    break;
  case 13:
    goto label80015CCC;
    break;
  default:
    goto label800163BC;
    break;
  }
label80015420:
  a0 = 0;
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
  spyro_memcpy32(a0, a1, a2);
  a0 = s0;
  a1 = 0;
  function_80012CF0();
  v0 = -1; // 0xFFFFFFFF
  sw(0x80076E24, v0);
  v0 = 2; // 0x0002
  goto label800163B4;
label800155B4:
  a0 = 0;
  function_80056B28();
  sw(SKYBOX_DATA, 0);
  sw(0x80076E24, -1);
  sw(0x80075864, lw(0x80075864)+1);
label800155E4:
  v0 = lw(0x800758B4);
  sw(LEVEL_ID, v0);
  update_level_functions();
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
  a1 = v1 + v0 * 0x40000;

  LoadImage((RECT[]){{0x200, v0*0x100, 0x200, 0x100}}, addr_to_pointer(a1));
  sw(0x8007576C, lw(0x8007576C)+1);
  a2 = 0xFFF80000;
  if (lw(0x8007576C) == 2) {
    t0 = lw(WAD_nested_header + 0x00);
    a2 = lw(WAD_nested_header + 0x04) - 0x80000;
    v1 = lw(WAD_header + 0x50 + lw(CONTINUOUS_LEVEL_ID)*0x10);
    sw(0x80075864, lw(0x80075864)+1);
    t0 += v1;
    a3 = 0x00080000 + t0;
    read_disk2(lw(WAD_sector), lw(0x800785DC), a2, a3, 0x0258);
  }
  goto label800163BC;
label8001583C:
  a0 = 0x1010;
  function_8005CB24();
  a0 = lw(0x800785DC);
  a1 = 0x0007EFF0;
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
  v0 = func_800133E0(a0);
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
  spyro_memset32(a0, a1, a2);
  v0 = lw(CONTINUOUS_LEVEL_ID);
  v0 = v0 << 2;
  v0 = lw(0x80077420 + v0);
  sw(0x80075830, 0);
  sw(0x800756C8, 0);
  sw(0x8007587C, v0);
  a0 = s0;
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
  function_80033F08();
  s0 -= 124; // 0xFFFFFF84
  a0 = s0;
  v0 = lw(0x80076E60);
  v1 = lw(0x80078B74);
  v0 += v1;
  sw(0x80076E60, v0);
  a1 = s3 + 80; // 0x0050
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
  function_80034204();
  a0 = s1;
  a1 = s1;
  a2 = s0;
  spyro_vec3_add(a0, a1, a2);
  function_800342F8();
  s0 = 0x8006EBE4;
  a0 = s0;
  a1 = s1;
  spyro_vec3_copy(a0, a1);
  a0 = s0 - 24; // 0xFFFFFFE8
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
  function_8003EA68();
  a0 = s4;
  v0 = 9; // 0x0009
  sw(s1 + 0x006E, v0);
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
  spyro_vec3_copy(a0, a1);
  a1 = lw(s0 + 0x0000);
  a0 = s1 + 490; // 0x01EA
  a1 += 24; // 0x0018
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
  spyro_vec3_copy(a0, a1);
  a1 = lw(s0 + 0x0000);
  a0 = s1 + 490; // 0x01EA
  a1 += 8; // 0x0008
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
  function_80034204();
  a0 = s4;
  a1 = a0;
  a2 = s2 - 284; // 0xFFFFFEE4
  spyro_vec3_add(a0, a1, a2);
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
  function_8003EA68();
  s1 = 0x80076DF8;
  a0 = s1;
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
  spyro_vec3_copy(a0, a1);
  a0 = s1;
  function_80034204();
  a0 = s0 - 16; // 0xFFFFFFF0
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
  spyro_vec3_add(a0, a1, a2);
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
  spyro_vec3_add(a0, a1, a2);
  function_800342F8();
  s0 = 0x8006EBE4;
  a0 = s0;
  a1 = s1;
  spyro_vec3_copy(a0, a1);
  a0 = s0 - 24; // 0xFFFFFFE8
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
  sp += 0x80;
}

// size: 0x00000110
void function_8002D228(void)
{

  uint32_t s0 = a0;

  ClearImage((RECT[]){{0, 0, 0x200, 0x1E0}}, 0, 0, 0);
  
  DrawSync(0);

  a0 = 0;
  function_80056B28();

  function_80056ED4();
  
  if (s0)
    function_8005B7D8();

  a0 = 1;
  function_8004AC24();

  sw(0x800757D8, 0x0F);
  sw(0x80075704, -1);
  sw(0x80075968, 0);
  sw(0x80075878, 0);
  sw(0x800757DC, 0);
  sw(0x800757BC, 0);
  sw(0x80075768, 0);
  sw(0x800756F8, 0);

  if (lw(0x800757AC) != 10)
    sw(0x800757AC, 0);

  read_disk1(lw(WAD_sector), lw(exe_end_ptr), lw(WAD_header + 0x28C), lw(WAD_header + 0x288), 0x258);
}

// size: 0x00000A00
void function_8002E12C(void)
{
  uint32_t temp;
  v0 = lw(0x80075690); // &0x00000000
  sp -= 48; // 0xFFFFFFD0
  sw(sp + 0x0028, ra);
  sw(sp + 0x0024, s1);
  temp = v0 != 0;
  sw(sp + 0x0020, s0);
  if (temp) goto label8002E150;
  function_80054988();
label8002E150:
  v0 = lw(0x8007568C); // &0x00000000
  sw(0x80077FB4, 0);
  sw(0x80077FB8, 0);
  sw(0x80077FBC, 0);
  sw(0x80077FC0, 0);
  sw(0x80077FC4, 0);
  v0++;
  sw(0x8007568C, v0); // &0x00000000
  a0 = 3; // 0x0003
  function_80058CC0();
  v0 = lw(0x800758B8);
  v0 = (int32_t)v0 < 5;
  temp = v0 != 0;
  if (temp) goto label8002EB14;
  a0 = lw(buttons_press);
  v0 = a0 & 0x100;
  temp = v0 != 0;
  v0 = 2; // 0x0002
  if (temp) goto label8002EA34;
  v1 = lw(0x800757C8); // &0x00000000
  temp = v1 != v0;
  v0 = 1; // 0x0001
  if (temp) goto label8002E384;
  v0 = a0 & 0xA000;
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label8002E21C;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002E);
  a3 = 0;
  function_80055A78();
  v1 = lw(0x80075720); // &0x00000000
  v0 = 1; // 0x0001
  v0 -= v1;
  sw(0x80075720, v0); // &0x00000000
  goto label8002EB14;
label8002E21C:
  v0 = a0 & 0x50;
  temp = v0 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label8002EB14;
  v1 = lw(0x80075720); // &0x00000000
  temp = v1 == v0;
  v0 = a0 & 0x10;
  if (temp) goto label8002E244;
  temp = v0 == 0;
  if (temp) goto label8002E27C;
label8002E244:
  a1 = 0;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002D);
  a3 = 0;
  function_80055A78();
  v0 = 3; // 0x0003
  sw(0x800757C8, 0); // &0x00000000
  sw(0x80075720, v0); // &0x00000000
  goto label8002EB14;
label8002E27C:
  a0 = 0;
  function_80056B28();
  a1 = 0;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002D);
  a3 = 0;
  function_80055A78();
  function_80056ED4();
  a0 = sp + 24; // 0x0018
  a1 = 0;
  a2 = 0;
  a3 = 0;
  v0 = 512; // 0x0200
  sh(sp + 0x001C, v0);
  v0 = 480; // 0x01E0
  sh(sp + 0x0018, 0);
  sh(sp + 0x001A, 0);
  sh(sp + 0x001E, v0);
  v0 = ClearImage(addr_to_pointer(a0), a1, a2, a3);
  a0 = 0;
  v0 = DrawSync(a0);
  a0 = lw(WAD_sector);
  a2 = lw(WAD_header + 0x0014);
  a3 = lw(WAD_header + 0x0010);
  a1 = lw(exe_end_ptr);
  v0 = 0x8007DDE8;
  sw(lib_end_ptr, v0);
  sw(0x800785DC, v0);
  v0 = 600; // 0x0258
  sw(0x80075864, 0);
  sw(0x8007566C, 0); // &0x00000000
  sw(sp + 0x0010, v0);
  read_disk1(a0, a1, a2, a3, lw(sp+0x10));
  goto label8002E348;
label8002E338:
  function_80014564();
  function_8002BBE0();
label8002E348:
  v0 = lw(0x80075864);
  v0 = (int32_t)v0 < 10;
  temp = v0 != 0;
  if (temp) goto label8002E338;
  function_8002D338();
  function_8002D170();
  v0 = 1; // 0x0001
  sw(0x8007579C, v0); // &0x00000000
  goto label8002EB14;
label8002E384:
  temp = v1 != v0;
  v0 = a0 & 0x4000;
  if (temp) goto label8002E8D0;
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label8002E408;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002D);
  a3 = 0;
  function_80055A78();
  a0 = lw(0x80075720); // &0x00000000
  sw(0x8007568C, 0); // &0x00000000
  v1 = a0 + 1; // 0x0001
  v0 = (int32_t)v1 < 6;
  sw(0x80075720, v1); // &0x00000000
  temp = v0 != 0;
  v0 = 3; // 0x0003
  if (temp) goto label8002E3E4;
  sw(0x80075720, 0); // &0x00000000
  goto label8002E47C;
label8002E3E4:
  temp = v1 != v0;
  if (temp) goto label8002E47C;
  v0 = lw(0x800756D8); // &0x00000000
  temp = v0 != 0;
  v0 = a0 + 2; // 0x0002
  if (temp) goto label8002E47C;
  goto label8002E474;
label8002E408:
  v0 = a0 & 0x1000;
  temp = v0 == 0;
  a2 = 16; // 0x0010
  if (temp) goto label8002E47C;
  v0 = lw(0x800761D4);
  a0 = lbu(v0 + 0x002D);
  a3 = 0;
  function_80055A78();
  a0 = lw(0x80075720); // &0x00000000
  sw(0x8007568C, 0); // &0x00000000
  v1 = a0 - 1; // 0xFFFFFFFF
  sw(0x80075720, v1); // &0x00000000
  temp = (int32_t)v1 >= 0;
  v0 = 3; // 0x0003
  if (temp) goto label8002E458;
  v0 = 5; // 0x0005
  goto label8002E474;
label8002E458:
  temp = v1 != v0;
  if (temp) goto label8002E47C;
  v0 = lw(0x800756D8); // &0x00000000
  temp = v0 != 0;
  v0 = a0 - 2; // 0xFFFFFFFE
  if (temp) goto label8002E47C;
label8002E474:
  sw(0x80075720, v0); // &0x00000000
label8002E47C:
  v0 = lw(buttons_press);
  v0 = v0 & 0x10;
  temp = v0 != 0;
  a1 = 0;
  if (temp) goto label8002E8A0;
  v1 = lw(0x80075720); // &0x00000000
  v0 = v1 < 6;
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label8002EB14;
  v0 = lw(0x80010DD0 + v0); // &0x8002E4C4
  temp = v0;
  switch (temp)
  {
  case 0x8002E4C4: // 0
    goto label8002E4C4;
    break;
  case 0x8002E5E0: // 1
    goto label8002E5E0;
    break;
  case 0x8002E6D4: // 2
    goto label8002E6D4;
    break;
  case 0x8002E788: // 3
    goto label8002E788;
    break;
  case 0x8002E824: // 4
    goto label8002E824;
    break;
  case 0x8002E888: // 5
    goto label8002E888;
    break;
  default:
    JR(temp, 0x8002E4BC);
    return;
  }
label8002E4C4:
  v0 = lw(buttons_press);
  v0 = v0 & 0x8000;
  temp = v0 == 0;
  if (temp) goto label8002E518;
  v0 = lw(0x80075754); // &0x00000000
  temp = (int32_t)v0 <= 0;
  a1 = 0;
  if (temp) goto label8002E518;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002E);
  a3 = 0;
  function_80055A78();
  v0 = lw(0x80075754); // &0x00000000
  v0--;
  goto label8002E570;
label8002E518:
  v0 = lw(buttons_press);
  v0 = v0 & 0x2000;
  temp = v0 == 0;
  a0 = 0x66660000;
  if (temp) goto label8002E57C;
  v0 = lw(0x80075754); // &0x00000000
  v0 = (int32_t)v0 < 10;
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label8002E57C;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002E);
  a3 = 0;
  function_80055A78();
  v0 = lw(0x80075754); // &0x00000000
  v0++;
label8002E570:
  sw(0x80075754, v0); // &0x00000000
  a0 = 0x66660000;
label8002E57C:
  v1 = lw(0x80075754); // &0x00000000
  a0 = a0 | 0x6667;
  v0 = v1 << 14;
  v0 -= v1;
  mult(v0, a0);
  a1 = 0x80020009; // &0x20304207
  v0 = (int32_t)v0 >> 31;
  t0=hi;
  v1 = (int32_t)t0 >> 2;
  v1 -= v0;
  a0 = v1 << 12;
  mult(a0, a1);
  sw(0x80076228, v1);
  t0=hi;
  v0 = t0 + a0;
  v0 = (int32_t)v0 >> 13;
  a0 = (int32_t)a0 >> 31;
  v0 -= a0;
  sw(0x8007622C, v0);
  goto label8002EB14;
label8002E5E0:
  v0 = lw(buttons_press);
  v0 = v0 & 0x8000;
  temp = v0 == 0;
  if (temp) goto label8002E634;
  v0 = lw(0x80075748); // &0x00000000
  temp = (int32_t)v0 <= 0;
  a1 = 0;
  if (temp) goto label8002E634;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002E);
  a3 = 0;
  function_80055A78();
  v0 = lw(0x80075748); // &0x00000000
  v0--;
  goto label8002E68C;
label8002E634:
  v0 = lw(buttons_press);
  v0 = v0 & 0x2000;
  temp = v0 == 0;
  if (temp) goto label8002E694;
  v0 = lw(0x80075748); // &0x00000000
  v0 = (int32_t)v0 < 10;
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label8002E694;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002E);
  a3 = 0;
  function_80055A78();
  v0 = lw(0x80075748); // &0x00000000
  v0++;
label8002E68C:
  sw(0x80075748, v0); // &0x00000000
label8002E694:
  v0 = 0x80076224;
  v1 = lw(0x80075748); // &0x00000000
  a1 = 192; // 0x00C0
  sw(v0 - 0x031C, a1); // 0xFFFFFCE4
  v1 = v1 << 11;
  sw(v0 + 0x0000, v1);
  sh(0x80075F18, v1);
  sh(0x80075F1A, v1);
  a0 = v0 - 796; // 0xFFFFFCE4
  function_8005CC58();
  goto label8002EB14;
label8002E6D4:
  v0 = lw(buttons_press);
  v0 = v0 & 0xA040;
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label8002EB14;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002E);
  a3 = 0;
  function_80055A78();
  a0 = 0x80076240;
  v1 = lw(a0 + 0x0000);
  v0 = 1; // 0x0001
  v0 -= v1;
  temp = v0 == 0;
  sw(a0 + 0x0000, v0);
  if (temp) goto label8002E74C;
  v0 = 63; // 0x003F
  sb(0x800776D3, v0);
  sb(0x800776D2, v0);
  sb(0x800776D1, v0);
  sb(0x800776D0, v0);
  goto label8002E770;
label8002E74C:
  v0 = 127; // 0x007F
  sb(0x800776D2, v0);
  sb(0x800776D0, v0);
  sb(0x800776D3, 0);
  sb(0x800776D1, 0);
label8002E770:
  a0 = 0x800776D0;
  function_80063FF0();
  goto label8002EB14;
label8002E788:
  v0 = lw(0x800756D8); // &0x00000000
  temp = v0 == 0;
  v0 = 4; // 0x0004
  if (temp) goto label8002E814;
  v0 = lw(buttons_press);
  v0 = v0 & 0xA040;
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label8002EB14;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002E);
  a3 = 0;
  function_80055A78();
  v1 = lw(0x800757A4); // &0x00000000
  v0 = 1; // 0x0001
  v0 -= v1;
  sw(0x800757A4, v0); // &0x00000000
  temp = v0 == 0;
  if (temp) goto label8002EB14;
  v0 = lw(0x80075764); // &0x00000000
  v0 = (int32_t)v0 < 10;
  temp = v0 == 0;
  v0 = 10; // 0x000A
  if (temp) goto label8002EB14;
  sw(0x80075764, v0); // &0x00000000
  goto label8002EB14;
label8002E814:
  sw(0x80075720, v0); // &0x00000000
  goto label8002EB14;
label8002E824:
  v0 = lw(buttons_press);
  v0 = v0 & 0xA040;
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label8002EB14;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002E);
  a3 = 0;
  function_80055A78();
  v0 = lw(CAMERA_MODE);
  v1 = 2; // 0x0002
  temp = v0 != v1;
  v0 = 82; // 0x0052
  if (temp) goto label8002E878;
  sw(CAMERA_MODE, v0);
  goto label8002EB14;
label8002E878:
  sw(CAMERA_MODE, v1);
  goto label8002EB14;
label8002E888:
  v0 = lw(buttons_press);
  v0 = v0 & 0x40;
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label8002EB14;
label8002E8A0:
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002E);
  a3 = 0;
  function_80055A78();
  sw(0x800757C8, 0); // &0x00000000
  sw(0x80075720, 0); // &0x00000000
  goto label8002EB14;
label8002E8D0:
  temp = v0 == 0;
  s0 = 3; // 0x0003
  if (temp) goto label8002E92C;
  a1 = 0;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002D);
  a3 = 0;
  function_80055A78();
  v0 = lw(0x80075720); // &0x00000000
  sw(0x8007568C, 0); // &0x00000000
  v0++;
  sw(0x80075720, v0); // &0x00000000
  v0 = (int32_t)s0 < (int32_t)v0;
  temp = v0 == 0;
  if (temp) goto label8002E97C;
  sw(0x80075720, 0); // &0x00000000
  goto label8002E97C;
label8002E92C:
  v0 = a0 & 0x1000;
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label8002E97C;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002D);
  a3 = 0;
  function_80055A78();
  v0 = lw(0x80075720); // &0x00000000
  sw(0x8007568C, 0); // &0x00000000
  v0--;
  sw(0x80075720, v0); // &0x00000000
  temp = (int32_t)v0 >= 0;
  if (temp) goto label8002E97C;
  sw(0x80075720, s0); // &0x00000000
label8002E97C:
  v0 = lw(buttons_press);
  v0 = v0 & 0x840;
  temp = v0 == 0;
  s0 = 1; // 0x0001
  if (temp) goto label8002EB14;
  v1 = lw(0x80075720); // &0x00000000
  temp = v1 == s0;
  v0 = (int32_t)v1 < 2;
  if (temp) goto label8002EA00;
  temp = v0 == 0;
  s1 = 2; // 0x0002
  if (temp) goto label8002E9C0;
  temp = v1 == 0;
  a1 = 0;
  if (temp) goto label8002E9D8;
  goto label8002EB14;
label8002E9C0:
  temp = v1 == s1;
  v0 = 3; // 0x0003
  if (temp) goto label8002EA34;
  temp = v1 == v0;
  a1 = 0;
  if (temp) goto label8002EA44;
  goto label8002EB14;
label8002E9D8:
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002E);
  a3 = 0;
  function_80055A78();
  a0 = 1; // 0x0001
  function_8002C534();
  goto label8002EB14;
label8002EA00:
  a1 = 0;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002E);
  a3 = 0;
  function_80055A78();
  sw(0x800757C8, s0); // &0x00000000
  sw(0x80075720, 0); // &0x00000000
  goto label8002EB14;
label8002EA34:
  a0 = 0;
  function_8002C714();
  goto label8002EB14;
label8002EA44:
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002E);
  a3 = 0;
  function_80055A78();
  v0 = 0x66660000;
  a0 = lw(LEVEL_ID);
  v0 = v0 | 0x6667;
  mult(a0, v0);
  v0 = (int32_t)a0 >> 31;
  t0=hi;
  v1 = (int32_t)t0 >> 2;
  v1 -= v0;
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 1;
  temp = a0 != v0;
  if (temp) goto label8002EAAC;
  sw(0x80075720, s0); // &0x00000000
  sw(0x800757C8, s1); // &0x00000000
  goto label8002EB14;
label8002EAAC:
  a0 = 0x80078BFC;
  spyro_vec3_clear(a0);
  v0 = lw(0x80075690); // &0x00000000
  sw(0x80078BEC, 0);
  temp = v0 == 0;
  v0 = 7; // 0x0007
  if (temp) goto label8002EB0C;
  v1 = lw(0x80075734); // &0x00000000
  sw(0x800757D8, v0); // &0x00000000
  sw(0x80075720, 0); // &0x00000000
  sw(0x8007568C, 0); // &0x00000000
  sw(0x80075900, s0);
  temp = v1;
  switch (temp)
  {
  case 0x8007D9C8:
    function_8007D9C8();
    break;
  case 0x8007DA78:
    function_8007DA78();
    break;
  case 0x8007AE40:
    function_8007AE40();
    break;
  case 0x8007DA54:
    function_8007DA54();
    break;
  case 0x8007AF94:
    function_8007AF94();
    break;
  case 0x8007CFB4:
    function_8007CFB4();
    break;
  case 0x8007E3A0:
    function_8007E3A0();
    break;
  case 0x8007E240:
    function_8007E240();
    break;
  case 0x8007B4C8:
    function_8007B4C8();
    break;
  case 0x8007AEB8:
    function_8007AEB8();
    break;
  case 0x8007E398:
    function_8007E398();
    break;
  case 0x8007BB00:
    function_8007BB00();
    break;
  case 0x8007B64C:
    function_8007B64C();
    break;
  case 0x8007B7A8:
    function_8007B7A8();
    break;
  case 0x8007AF28:
    function_8007AF28();
    break;
  case 0x8007E18C:
    function_8007E18C();
    break;
  case 0x8007B5DC:
    function_8007B5DC();
    break;
  case 0x8007AFBC:
    function_8007AFBC();
    break;
  case 0x8007B698:
    function_8007B698();
    break;
  case 0x8007B770:
    function_8007B770();
    break;
  case 0x8007E3C0:
    function_8007E3C0();
    break;
  case 0x8007B4F8:
    function_8007B4F8();
    break;
  case 0x8007B4DC:
    function_8007B4DC();
    break;
  case 0x8007B510:
    function_8007B510();
    break;
  case 0x8007AF50:
    function_8007AF50();
    break;
  case 0x8007D938:
    function_8007D938();
    break;
  case 0x8007B528:
    function_8007B528();
    break;
  case 0x8007AE5C:
    function_8007AE5C();
    break;
  case 0x8007AD64:
    function_8007AD64();
    break;
  case 0x8007AD4C:
    function_8007AD4C();
    break;
  default:
    JALR(temp, 0x8002EAFC);
  }
  goto label8002EB14;
label8002EB0C:
  function_8002C618();
label8002EB14:
  ra = lw(sp + 0x0028);
  s1 = lw(sp + 0x0024);
  s0 = lw(sp + 0x0020);
  sp += 48; // 0x0030
  return;
}

// function called on death/game over
// size: 0x000005D4
void function_8002EDF0(void)
{
  uint32_t temp;
  v0 = lw(0x8007593C);
  sp -= 80; // 0xFFFFFFB0
  sw(sp + 0x0048, ra);
  sw(sp + 0x0044, s3);
  sw(sp + 0x0040, s2);
  sw(sp + 0x003C, s1);
  sw(sp + 0x0038, s0);

  if (lw(0x8007593C) == 0)
  {
    a0 = 0;
    function_80056B28();
  }

  sw(0x8007593C, lw(0x8007593C)+1);

  a0 = 3;
  function_80058CC0();
  s2 = lw(0x80075940);
  if (s2 != 0) goto label8002F0C4;
  if ((int32_t)lw(0x8007593C) < 16) goto label8002F3A4;
  if (lw(0x800757D8) != 4) goto label8002EE94;
  function_8002C8A4();
  function_800144C8();
  goto label8002F3A4;
label8002EE94:
  s0 = WAD_header + 0x3C;
  a0 = lw(WAD_sector);
  a2 = lw(WAD_header + 0x3C);
  s1 = 0x800785E8;
  v0 = lw(s1);
  a1 = lw(WAD_header + 0x3C);
  a3 = lw(WAD_header + 0x0038);
  a1 = v0 - a1;
  read_disk1(a0, a1, a2, a3, 0x258);
  v1 = lw(s1);
  v0 = lw(WAD_header + 0x3C);
  t0 = SKYBOX_DATA + 0x10;
  a1 = v1 - v0;
  a0 = lbu(a1 + 0);
  sb(t0, a0);
  v1 = lbu(a1 + 1);
  a3 = a1;
  sb(SKYBOX_DATA + 0x11, v1);
  v0 = lbu(a1 + 2);
  a1 += 4;
  sb(DISP1 + 0x19, a0);
  sb(DISP1 + 0x1A, v1);
  sb(DISP2 + 0x19, a0);
  sb(DISP2 + 0x1A, v1);
  sb(SKYBOX_DATA + 0x12, v0);
  sb(DISP1 + 0x1B, v0);
  sb(DISP2 + 0x1B, v0);
  v0 = lw(a1 + 0);
  a1 += 4;
  sw(SKYBOX_DATA + 0x04, a1);
  sw(SKYBOX_DATA + 0x00, v0);
  v0 = (int32_t)v0 > 0;
  temp = v0 == 0;
  for (int i = 0; i < lw(SKYBOX_DATA + 0x00); i++)
  {
    v0 = lw(a1 + 0x0000);
    v0 += a3;
    sw(a1 + 0x0000, v0);
    a1 += 4; // 0x0004
  }
  v1 = lhu(0x800752D8);
  a1 = lhu(0x800752DA);
  sw(0x80076DF8, 0x2800);
  sw(0x80076DFC, 0x80);
  sh(0x80076E1C, 0);
  sw(0x80076E00, 0x800);
  sh(0x80076E1E, v1);
  sh(0x80076E20, a1);

  a0 = 1;
  function_8004AC24();

  sb(0x80078A70, 0x10);
  sb(0x80078A71, 0x10);
  sb(0x80078A77, 1);
  sw(0x8007582C, 4);
  sw(0x800758E8, 0);
  sw(0x80078BBC, 3);
  sw(0x800756D0, 0);
  sw(0x800758AC, 0);
  sw(0x800757E8, lw(CONTINUOUS_LEVEL_ID));
  spyro_memset32(0x80077888, 0, 0x68);
  a0 = lw(LEVEL_ID);
  v0 = a0/10*10;
  if (a0 == v0) goto label8002F0B0;
  sw(0x800758B4, v0);
  sw(0x80075864, 2);
  sw(0x8007576C, -1);
  sw(0x80075940, 1);
  sw(0x8007593C, 0);
  goto label8002F3A4;
label8002F0B0:
  sw(0x80075864, 11);
  sw(0x80075940, 1);
  sw(0x8007593C, 0);
  goto label8002F3A4;
label8002F0C4:
  temp = s2 != 1;
  if (temp) goto label8002F368;
  v0 = lw(0x8007593C);
  s3 = v0 - 180; // 0xFFFFFF4C
  v0 = s3 < 192;
  temp = v0 == 0;
  s0 = s3 << 4;
  if (temp) goto label8002F240;
  s1 = s0 + 0xE00;
  s1 = s1 & 0xFFF;
  a0 = s1;
  v0 = spyro_cos(a0);
  s2 = 0x2AAAAAAB;
  s0 += s3;
  s0 = s0 << 5;
  s0 -= s3;
  s0 = s0 << 1;
  mult(s0, 0x2AAAAAAB);
  s0 = (int32_t)s0 >> 31;
  t1=hi;
  v1 = (int32_t)t1 >> 5;
  v1 -= s0;
  s0 = 0x87C - v1;
  mult(v0, s0);
  v1=lo;
  v0 = (int32_t)v1 >> 12;
  v0 += 10752; // 0x2A00
  sw(spyro_position + 0, v0);
  a0 = s1;
  v0 = spyro_sin(a0);
  mult(v0, s0);
  v1 = s3 << 1;
  v1 += s3;
  a0=lo;
  v1 = v1 << 1;
  v0 = 2884; // 0x0B44
  mult(s3, 0x2AAAAAAB);
  v0 -= v1;
  s1 += 1024; // 0x0400
  s1 = (int32_t)s1 >> 4;
  v1 = (int32_t)s3 >> 31;
  sw(spyro_position + 8, v0);
  sb(0x80078A66, s1);
  sb(0x80078A65, 0);
  v0 = (int32_t)a0 >> 12;
  a0 = lw(0x8007593C);
  v0 += 3072; // 0x0C00
  sw(spyro_position + 4, v0);
  t1=hi;
  v0 = (int32_t)t1 >> 1;
  v0 -= v1;
  v0 -= 32; // 0xFFFFFFE0
  sb(0x80078A64, v0);
  temp = v0 == 0;
  if ((int32_t)a0 < 356) {
    a0 = 3;
    function_8003CB24();
  }
  else
  {
    v1 = a0 - 356;
    v1 = v1*10;
    sb(0x80078A71, 0x0E);
    sb(0x80078A77, 0);
    sb(0x80078A7C, a0 - 100);
    v0 = (int32_t)v1 >> 4;
    if ((int32_t)v1 < 0) {
      v1 += 0x0F;
      v0 = (int32_t)v1 >> 4;
    }
    sb(0x80078A65, -(((int32_t)v0)/16));
    sb(0x80078A64, a0 - 116);
  }
  function_80049660();
  function_80049E8C();
label8002F240:
  v1 = lw(0x8007593C);
  
  if ((int32_t)v1 >= 372) {
    if (v1 == 372) {
      sb(0x80078A70, 0x0E);
      sb(0x80078A71, 0x0E);
      sb(0x80078A76, 0);
      sb(0x80078A77, 1);
      sb(0x80078A7C, 0);
    }
    sw(spyro_position + 0, 0x26F8);
    sw(spyro_position + 4, 0x900);
    sw(spyro_position + 8, 0x6C4);
    sb(0x80078A65, 0xF6);
    sb(0x80078A64, 0);
    sb(0x80078A66, 0xE0);
    a0 = 3;
    function_8003CB24();
    function_80049660();
    function_80049E8C();
  }

  if ((int32_t)lw(0x80075864) < 11) {
    a0 = 1;
    function_80015370();
    goto label8002F3A4;
  }
  v0 = lw(0x8007593C);
  if ((int32_t)v0 > 360 && lw(buttons) & 0x800) {
    a0 = 0;
    function_8003FDC8();
    sw(0x80075940, 2);
    sw(0x8007593C, 0);
  }
  goto label8002F3A4;
label8002F368:
  v0 = (int32_t)lw(0x8007593C) < 17;
  temp = v0 != 0;
  if (temp) goto label8002F3A4;
  a0 = 1; // 0x0001
  function_80015370();
  v0 = lw(0x80075864);
  temp = (int32_t)v0 >= 0;
  if (temp) goto label8002F3A4;
  function_8002C8A4();
label8002F3A4:
  ra = lw(sp + 0x48);
  s3 = lw(sp + 0x44);
  s2 = lw(sp + 0x40);
  s1 = lw(sp + 0x3C);
  s0 = lw(sp + 0x38);
  sp += 0x50;
  return;
}

// size: 0x0000026C
void function_800539FC(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  v1 = lw(IS_DEMO_MODE);
  v0 = 2; // 0x0002
  sw(sp + 0x0014, ra);
  temp = v1 != v0;
  sw(sp + 0x0010, s0);
  if (temp) goto label80053A8C;
  a0 = 0x8007738C;
  v0 = lbu(0x800786A2);
  v1 = lbu(0x800786A3);
  v0 = v0 << 8;
  s0 = ~(v1 | v0);
  v0 = lw(0x800786A4);
  a1 = a0 - 20; // 0xFFFFFFEC
  sw(a0 + 0x0000, v0);
  function_80053940();
  s0 = s0 & 0xFFFF;
  v0 = lbu(0x8007738E);
  v1 = lbu(0x8007738F);
  a0 = lw(0x8007585C);
  v0 = v0 << 16;
  s0 = s0 | v0;
  v1 = v1 << 24;
  s0 = s0 | v1;
  v0 = a0 + 4; // 0x0004
  sw(0x8007585C, v0);
  sw(a0 + 0x0000, s0);
  goto label80053AD4;
label80053A8C:
  v1 = lw(0x8007585C);
  v0 = v1 + 4; // 0x0004
  sw(0x8007585C, v0);
  s0 = lw(v1 + 0x0000);
  v0 = 127; // 0x007F
  sb(0x8007738C, v0);
  sb(0x8007738D, v0);
  v0 = (int32_t)s0 >> 16;
  sb(0x8007738E, v0);
  v0 = (int32_t)s0 >> 24;
  sb(0x8007738F, v0);
label80053AD4:
  s0 = s0 & 0xFFFF;
  v1 = lw(buttons);
  v0 = 3; // 0x0003
  sw(0x80077384, v0);
  v0 = ~v1;
  v0 = v0 & s0;
  sw(buttons_press, v0);
  v0 = ~s0;
  v1 = v1 & v0;
  v0 = lw(0x8007738C);
  sw(buttons, s0);
  sw(buttons_unpress, v1);
  v1 = 0xFFFF0000;
  v0 = v0 & v1;
  v1 = 0x7F7F0000;
  temp = v0 == v1;
  v0 = 1; // 0x0001
  if (temp) goto label80053B40;
  sw(0x80077388, v0);
  a2 = 0;
  goto label80053B4C;
label80053B40:
  sw(0x80077388, 0);
  a2 = 0;
label80053B4C:
  a0 = buttons;
  a1 = a0 + 80; // 0x0050
  v1 = 0;
label80053B5C:
  v0 = lw(a0 + 0x0000);
  sw(0x800773C0 + v1, v0);
  v0 = lw(a0 - 0x0008); // 0xFFFFFFF8
  sw(0x800773C4 + v1, v0);
  v0 = lw(a0 - 0x0004); // 0xFFFFFFFC
  sw(0x800773C8 + v1, v0);
  v0 = lw(a0 + 0x0004);
  sw(0x800773BC + v1, v0);
  v0 = lw(a0 + 0x000C);
  a2++;
  sw(a1 + 0x0000, v0);
  v0 = lw(a0 + 0x0008);
  a1 += 24; // 0x0018
  sw(0x800773CC + v1, v0);
  v0 = (int32_t)a2 < 4;
  temp = v0 != 0;
  v1 += 24; // 0x0018
  if (temp) goto label80053B5C;
  v0 = lw(buttons);
  v1 = 1; // 0x0001
  sw(0x80077390, v1);
  sw(0x80077394, v1);
  v0 = v0 & 0xF000;
  temp = v0 != 0;
  v1 = 0xFFFF0000;
  if (temp) goto label80053C0C;
  v0 = lw(0x8007738C);
  v0 = v0 & v1;
  v1 = 0x7F7F0000;
  temp = v0 == v1;
  if (temp) goto label80053C14;
label80053C0C:
  sw(0x80077394, 0);
label80053C14:
  v0 = lw(0x80077394);
  temp = v0 == 0;
  if (temp) goto label80053C40;
  v0 = lw(buttons);
  v0 = v0 & 0xF0FF;
  temp = v0 == 0;
  v0 = 2; // 0x0002
  if (temp) goto label80053C4C;
label80053C40:
  sw(0x80077390, 0);
  v0 = 2; // 0x0002
label80053C4C:
  sw(0x80075760, v0); // &0x00000000
  ra = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x00000388
void function_800334D4(void)
{
  uint32_t temp;
  sp -= 48; // 0xFFFFFFD0
  sw(sp + 0x002C, ra);
  sw(sp + 0x0028, s0);
  function_800539FC();
  v1 = lw(IS_DEMO_MODE);
  v0 = 2; // 0x0002
  temp = v1 == v0;
  v0 = 0;
  if (temp) goto label80033848;
  v0 = lw(0x80075884);
  temp = v0 != 0;
  v0++;
  if (temp) goto label80033598;
  v0 = lw(0x800757B8); // &0x00000000
  v0 = v0 << 2;
  v0 = lw(0x8006EE8C + v0); // &0x0000035C
  v1 = lw(level_frame_counter);
  v0 -= 8; // 0xFFFFFFF8
  v0 = (int32_t)v1 < (int32_t)v0;
  temp = v0 == 0;
  v0 = (int32_t)v1 < 16;
  if (temp) goto label80033584;
  temp = v0 != 0;
  if (temp) goto label800335AC;
  v0 = lbu(0x800786A0);
  temp = v0 != 0;
  v0 = 255; // 0x00FF
  if (temp) goto label800335AC;
  v1 = lbu(0x800786A2);
  temp = v1 != v0;
  v0 = 1; // 0x0001
  if (temp) goto label80033588;
  v0 = lbu(0x800786A3);
  temp = v0 == v1;
  if (temp) goto label800335AC;
label80033584:
  v0 = 1; // 0x0001
label80033588:
  sw(0x80075884, v0);
  goto label800335AC;
label80033598:
  sw(0x80075884, v0);
  v0 = v0 << 1;
  sw(0x80075918, v0);
label800335AC:
  v1 = lw(0x80075884);
  v0 = 16; // 0x0010
  temp = v1 != v0;
  v0 = 0;
  if (temp) goto label80033848;
  a0 = 0;
  function_80056B28();
  function_80056ED4();
  a0 = sp + 24; // 0x0018
  a1 = 0;
  a2 = 0;
  a3 = 0;
  v0 = 512; // 0x0200
  sh(sp + 0x001C, v0);
  v0 = 480; // 0x01E0
  sh(sp + 0x0018, 0);
  sh(sp + 0x001A, 0);
  sh(sp + 0x001E, v0);
  v0 = ClearImage(addr_to_pointer(a0), a1, a2, a3);
  a0 = 0;
  v0 = DrawSync(a0);
  a0 = lw(WAD_sector);
  a2 = lw(WAD_header + 0x0014);
  a3 = lw(WAD_header + 0x0010);
  a1 = lw(exe_end_ptr);
  v0 = 0x8007DDE8;
  sw(lib_end_ptr, v0);
  sw(0x800785DC, v0);
  v0 = 600; // 0x0258
  sw(0x80075904, 0);
  sw(0x80075764, 0); // &0x00000000
  sw(0x800757D0, 0); // &0x00000000
  sw(0x8007584C, 0);
  sw(IS_DEMO_MODE, 0);
  sw(0x80075864, 0);
  sw(0x8007566C, 0); // &0x00000000
  sw(0x80075690, 0); // &0x00000000
  sw(0x80075784, 0); // &0x00000000
  sw(sp + 0x0010, v0);
  read_disk1(a0, a1, a2, a3, lw(sp+0x10));
  goto label800336A8;
label80033698:
  function_80014564();
  function_8002BBE0();
label800336A8:
  v0 = lw(0x80075864);
  v0 = (int32_t)v0 < 10;
  temp = v0 != 0;
  v0 = 14; // 0x000E
  if (temp) goto label80033698;
  sw(0x800757D8, v0); // &0x00000000
  v0 = 15; // 0x000F
  sw(0x80075918, v0);
  function_8002D170();
  v0 = lw(0x800757B8); // &0x00000000
  v1 = lw(level_frame_counter);
  v0 = v0 << 2;
  v0 = lw(0x8006EE8C + v0); // &0x0000035C
  v1 = (int32_t)v1 < (int32_t)v0;
  temp = v1 != 0;
  a0 = 0x00020000;
  if (temp) goto label800337D8;
  v1 = lw(0x80075680); // &0x00000000
  v0 = 720; // 0x02D0
  sw(v1 + 0x0000, v0);
  v0 = 0x800778F0;
  sw(0x800761D0, v0);
  v0 = lw(0x800761D0);
  a0 = a0 | 0x62D0;
  sw(v0 + 0x0000, a0);
  v1 = lw(0x800761D0);
  v0 = -1; // 0xFFFFFFFF
  sw(v1 + 0x0004, v0);
  v1 = lw(0x800761D0);
  v0 = 80; // 0x0050
  sh(v1 + 0x0008, v0);
  v1 = lw(0x800761D0);
  v0 = lw(0x8006EE6C); // &0x00000800
  a1 = 0;
  sh(v1 + 0x000A, v0);
  v0 = lw(0x800761D0);
  a2 = 16; // 0x0010
  sh(v0 + 0x000C, 0);
  v0 = lw(0x800761D0);
  a3 = 0x80077084;
  sh(v0 + 0x000E, 0);
  v0 = lw(0x800761D0);
  sw(v0 + 0x0010, 0);
  v0 = 1; // 0x0001
  sw(0x800761DC, v0);
  v0 = 16383; // 0x3FFF
  sh(0x800761E8, v0);
  sh(0x800761EA, v0);
  a0 = 0;
  function_80055A78();
  a2 = 0x00040000;
  goto label800337EC;
label800337D8:
  v1 = lw(0x80075680); // &0x00000000
  v0 = 1168; // 0x0490
  sw(v1 + 0x0000, v0);
  a2 = 0x00040000;
label800337EC:
  s0 = 1; // 0x0001
  a0 = lw(WAD_sector);
  a3 = lw(WAD_header + 0x0008);
  v0 = 600; // 0x0258
  sw(sp + 0x0010, v0);
  v0 = lw(0x800785E8);
  a1 = 0xFFFC0000;
  sw(0x80078D78 + 0x0010, s0);
  a1 += v0;
  read_disk2(a0, a1, a2, a3, lw(sp+0x10));
  v1 = lw(0x800757B8); // &0x00000000
  sw(0x8007579C, s0); // &0x00000000
  v1++;
  v1 = v1 & 0x3;
  sw(0x800757B8, v1); // &0x00000000
  v0 = 1; // 0x0001
label80033848:
  ra = lw(sp + 0x002C);
  s0 = lw(sp + 0x0028);
  sp += 48; // 0x0030
  return;
}

// size: 0x00000108
void function_8005B7D8(void)
{
  uint32_t temp;
  sp -= 48; // 0xFFFFFFD0
  sw(sp + 0x0020, s2);
  s2 = 0;
  sw(sp + 0x0024, s3);
  s3 = 0x80076378;
  v1 = WAD_header + 0x0044;
  a0 = lw(WAD_sector);
  v0 = 600; // 0x0258
  sw(sp + 0x0028, ra);
  sw(sp + 0x001C, s1);
  sw(sp + 0x0018, s0);
  a2 = lw(v1 + 0x0000);
  s1 = 0x80200000;
  sw(sp + 0x0010, v0);
  v1 = lw(v1 + 0x0000);
  v0 = lw(0x800755A4); // &0x00000800
  a3 = lw(WAD_header + 0x0040);
  s0 = s1 - v1;
  s0 -= v1;
  s0 -= v0;
  a1 = s0;
  read_disk1(a0, a1, a2, a3, lw(sp+0x10));
  a1 = s0 + 2048; // 0x0800
  v0 = lw(0x800755A4); // &0x00000800
  v1 = lw(s0 + 0x0000);
  s1 -= v0;
  s1 -= v1;
  sw(0x800785FC, s1);
  a2 = lw(s0 + 0x0000);
  a0 = s1;
  spyro_memcpy32(a0, a1, a2);
label8005B870:
  v0 = lw(s0 + 0x0004);
  temp = v0 == 0;
  if (temp) goto label8005B8B8;
  a0 = lw(0x800785FC);
  s2++;
  a0 += v0;
  a0 -= 2048; // 0xFFFFF800
  v0 = func_800133E0(a0);
  v1 = lw(s0 + 0x0008);
  v1 = v1 << 2;
  v1 += s3;
  sw(v1 + 0x0000, v0);
  v0 = (int32_t)s2 < 64;
  temp = v0 != 0;
  s0 += 8; // 0x0008
  if (temp) goto label8005B870;
label8005B8B8:
  a0 = 0;
  function_8005B6F8();
  ra = lw(sp + 0x0028);
  s3 = lw(sp + 0x0024);
  s2 = lw(sp + 0x0020);
  s1 = lw(sp + 0x001C);
  s0 = lw(sp + 0x0018);
  sp += 48; // 0x0030
  return;
}



// size: 0x00001580
void function_8007AA50_credits(void)
{
  uint32_t temp;
  sp -= 136; // 0xFFFFFF78
  v1 = lw(0x80075704);
  v0 = -1; // 0xFFFFFFFF
  sw(sp + 0x84, ra);
  sw(sp + 0x80, fp);
  sw(sp + 0x7C, s7);
  sw(sp + 0x78, s6);
  sw(sp + 0x74, s5);
  sw(sp + 0x70, s4);
  sw(sp + 0x6C, s3);
  sw(sp + 0x68, s2);
  sw(sp + 0x64, s1);
  sw(sp + 0x60, s0);

  if (v1 == -1) 
  {
    v0 = lw(0x80075768);
    a1 = 0x8007C338; // &0x03E00008
    temp = v0 != 0;
    a2 = 2048; // 0x0800
    if (temp) goto label8007AB08;
    a0 = 0;
    v1 = 0;
    sw(0x8007589C, a1);
    sw(0x800756E4, 0);
  label8007AABC:
    v0 = lw(0x8007589C);
    a0++;
    v0 += v1;
    sb(v0 + 0x0004, 0);
    v0 = (int32_t)a0 < 100;
    temp = v0 != 0;
    v1 += 28; // 0x001C
    if (temp) goto label8007AABC;
    v0 = a1 + 2800; // 0x0AF0
    sw(0x80075708, v0);
    v0 = a1 + 10240; // 0x2800
    sw(0x80075768, v0);
    sw(lib_end_ptr, v0);
    sw(0x800785DC, v0);
    a2 = 2048; // 0x800
  label8007AB08:
    read_disk1(
      lw(WAD_sector), lw(0x800785DC), 0x800,
      lw(WAD_header + 0x0290 + lw(0x800757AC)*8),
      0x258
    );

    spyro_memcpy32(WAD_nested_header, lw(0x800785DC), 0x1D0);

    read_disk1(
      lw(WAD_sector), lw(0x800785DC), 0x60000,
      lw(WAD_nested_header) + lw(WAD_header + 0x0290 + lw(0x800757AC)*8),
      0x258
    );

    LoadImage((RECT[]){{0x200, 0, 0x200, 0x180}}, addr_to_pointer(lw(0x800785DC)));

    read_disk1(
      lw(WAD_sector), lw(0x800785DC), lw(WAD_nested_header + 0x04) - 0x60000,
      lw(WAD_nested_header) + lw(WAD_header + 0x0290 + lw(0x800757AC)*8) + 0x60000,
      0x258
    );

    a0 = 0x1010;
    function_8005CB24();

    a1 = 0x0007EFF0;
    a0 = lw(0x800785DC);
    function_8005CAC4();

    while (test_spu_event(0) == 0);

    read_disk1(
      lw(WAD_sector), lw(0x800785DC), lw(WAD_nested_header + 0x0C),
      lw(WAD_nested_header + 0x08) + lw(WAD_header + 0x0290 + lw(0x800757AC)*8),
      0x258
    );

    a0 = lw(0x800785DC);
    a1 = 1;
    function_80012D58();

    sw(0x800785E0, v0);
    
    v1 = lw(WAD_nested_header + 0x18);
    sw(0x800785E4, lw(0x800785E0));
    spyro_memcpy32(SKYBOX_DATA, 0x80077780, 0x14);
    sw(0x80078600, lw(WAD_nested_header + 0x18));
    sw(0x80078604, lw(WAD_nested_header + 0x1C));
    sw(0x800758BC, 0);
    
    read_disk1(
      lw(WAD_sector), lw(0x800785E4), lw(0x80078604),
      lw(WAD_nested_header + 0x18) + lw(WAD_header + 0x0290 + lw(0x800757AC)*8),
      0x258
    );

    t0 = lw(0x800785E4);

    sw(0x800761D0, 0x800778F0);

    s0 = 0x800778F0;
    sw(s0 + 0x00, 0x1010);
    sw(s0 + 0x04, -1);
    sh(s0 + 0x08, 0x50);
    sh(s0 + 0x0A, 0x659);
    sh(s0 + 0x0C, 0);
    sh(s0 + 0x0E, 0);
    sw(s0 + 0x10, 0);

    sw(0x800757BC, lw(0x800785E4));
    sw(0x800761DC, 1);
    sh(0x800761E8, 0x3FFF);
    sh(0x800761EA, 0x3FFF);

    a0 = 0;
    a1 = 0;
    a2 = 0x10;
    a3 = 0;
    function_80055A78();

    v0 = lw(0x800785E4) + lw(lw(0x800757BC) + 4);
    a0 = lw(0x80075708);
    sw(0x800757EC, v0);
    sw(0x800757DC, lw(v0 + 0x08));
    s0 = a0 + lw(v0 + 0x04) - 0x0C;

    spyro_memcpy32(a0, v0 + 0x0C, lw(v0 + 0x00) - 0x0C);

    a1 = lw(0x800757DC);
    v1 = lw(0x80075708);
    for (int i = 0; i < a1; i++)
      sw(v1 + i*0x0C, lw(v1 + i*0x0C) + s0);
    
    sw(0x80075704, 0);
    sw(0x800757AC, lw(0x800757AC)+1);
  }

  a1 = lw(0x800757BC);
  temp = a1 == 0;
  if (temp) goto label8007B068;
  a2 = lw(0x80075704);
  temp = (int32_t)a2 < 0;
  if (temp) goto label8007B068;
  v1 = lw(a1 + 0x0000);
  v0 = lw(0x800756CC);
  a0 = lw(a1 + 0x0008);
  v1 += v0;
  a0 = a0 << 1;
  v0 = (int32_t)v1 < (int32_t)a0;
  temp = v0 == 0;
  sw(a1 + 0x0000, v1);
  if (temp) goto label8007B05C;
  v0 = (int32_t)v1 < 32;
  temp = v0 != 0;
  if (temp) goto label8007AEF8;
  v1 = a0 - v1;
  v0 = (int32_t)v1 < 32;
  temp = v0 == 0;
  if (temp) goto label8007AF14;
label8007AEF8:
  v1 = (int32_t)v1 >> 1;
  v0 = 16; // 0x0010
  v0 -= v1;
  sw(0x80075918, v0);
  goto label8007AF1C;
label8007AF14:
  sw(0x80075918, 0);
label8007AF1C:
  v0 = lw(0x80075918);
  v0 = (int32_t)v0 < 16;
  temp = v0 != 0;
  v0 = 15; // 0x000F
  if (temp) goto label8007AF3C;
  sw(0x80075918, v0);
label8007AF3C:
  v0 = lw(0x80075918);
  temp = (int32_t)v0 >= 0;
  if (temp) goto label8007AF58;
  sw(0x80075918, 0);
label8007AF58:
  a0 = lw(0x800757BC);
  v1 = lw(a0 + 0x0000);
  v1 = (int32_t)v1 >> 1;
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 3;
  v0 += a0;
  v0 = lw(v0 + 0x000C);
  sw(0x80076DF8, v0);
  v1 = lw(a0 + 0x0000);
  v1 = (int32_t)v1 >> 1;
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 3;
  v0 += a0;
  v0 = lw(v0 + 0x0010);
  sw(0x80076DFC, v0);
  v1 = lw(a0 + 0x0000);
  v1 = (int32_t)v1 >> 1;
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 3;
  v0 += a0;
  v0 = lw(v0 + 0x0014);
  sw(0x80076E00, v0);
  v1 = lw(a0 + 0x0000);
  v1 = (int32_t)v1 >> 1;
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 3;
  v0 += a0;
  v0 = lw(v0 + 0x0018);
  sh(0x80076E1C, v0);
  v1 = lw(a0 + 0x0000);
  v1 = (int32_t)v1 >> 1;
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 3;
  v0 += a0;
  v0 = lw(v0 + 0x001C);
  sh(0x80076E1E, v0);
  v1 = lw(a0 + 0x0000);
  v1 = (int32_t)v1 >> 1;
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 3;
  a0 += v0;
  v0 = lw(a0 + 0x0020);
  sh(0x80076E20, v0);
  goto label8007B068;
label8007B05C:
  v0 = a2 + 1; // 0x0001
  sw(0x80075704, v0);
label8007B068:
  v0 = lw(0x800756E4);
  temp = v0 != 0;
  if (temp) goto label8007B134;
  v0 = lw(0x80075968);
  v1 = lw(0x800757DC);
  v0 = (int32_t)v0 < (int32_t)v1;
  temp = v0 != 0;
  if (temp) goto label8007B12C;
  v1 = lw(0x800761D0);
  v0 = lhu(v1 + 0x0008);
  v0 -= 10; // 0xFFFFFFF6
  sh(v1 + 0x0008, v0);
  v0 = lw(0x800756F8);
  a1 = lw(0x800761D0);
  v1 = lw(0x800756CC);
  a0 = lh(a1 + 0x0008);
  v0 += v1;
  sw(0x800756F8, v0);
  temp = (int32_t)a0 >= 0;
  if (temp) goto label8007B0EC;
  sh(a1 + 0x0008, 0);
label8007B0EC:
  v0 = lw(0x800756F8);
  sw(0x80075918, v0);
  v0 = (int32_t)v0 < 16;
  temp = v0 != 0;
  v0 = 15; // 0x000F
  if (temp) goto label8007B110;
  sw(0x80075918, v0);
label8007B110:
  v1 = lw(0x80075918);
  v0 = 15; // 0x000F
  temp = v1 != v0;
  v0 = 99; // 0x0063
  if (temp) goto label8007B12C;
  sw(0x80075704, v0);
label8007B12C:
  v0 = lw(0x800756E4);
label8007B134:
  temp = (int32_t)v0 > 0;
  if (temp) goto label8007B16C;
  v1 = lw(0x800757DC);
  temp = (int32_t)v1 <= 0;
  if (temp) goto label8007BBE4;
  v0 = lw(0x80075968);
  v0 = (int32_t)v0 < (int32_t)v1;
  temp = v0 == 0;
  if (temp) goto label8007BBE4;
label8007B16C:
  fp = lw(0x80075878);
  a0 = 3; // 0x0003
  function_80058CC0();
  s5 = lw(0x80075968);
  v0 = (int32_t)fp < (int32_t)s5;
  temp = v0 != 0;
  s4 = 1; // 0x0001
  if (temp) goto label8007B828;
  v0 = s5 << 1;
  v0 += s5;
  s7 = v0 << 2;
label8007B1A0:
  s3 = -1; // 0xFFFFFFFF
  a0 = lw(0x80075708);
  a1 = 2; // 0x0002
  a0 += s7;
  a0 += 6; // 0x0006
  function_80037F90();
  temp = v0 == 0;
  if (temp) goto label8007B200;
  v0 = lw(0x80075708);
  v0 += s7;
  a0 = lh(v0 + 0x0008);
  v1 = 2; // 0x0002
  temp = a0 != 0;
  sh(v0 + 0x000A, v1);
  if (temp) goto label8007B200;
  v0 = lw(0x80075968);
  temp = s5 != v0;
  v0 = s5 + 1; // 0x0001
  if (temp) goto label8007B200;
  sw(0x80075968, v0);
label8007B200:
  a0 = lw(0x80075708);
  v0 = lh(a0 + 0x000A);
  temp = v0 != 0;
  a0 += s7;
  if (temp) goto label8007B224;
  s3 = 0;
  goto label8007B278;
label8007B224:
  a0 += 8; // 0x0008
  a1 = 2; // 0x0002
  function_80037F90();
  temp = v0 == 0;
  v0 = -1; // 0xFFFFFFFF
  if (temp) goto label8007B27C;
  v0 = lw(0x80075708);
  v0 += s7;
  v0 = lh(v0 + 0x0016);
  temp = v0 != 0;
  v0 = -1; // 0xFFFFFFFF
  if (temp) goto label8007B27C;
  v0 = lw(0x800757DC);
  v0--;
  v0 = (int32_t)s5 < (int32_t)v0;
  temp = v0 == 0;
  v0 = -1; // 0xFFFFFFFF
  if (temp) goto label8007B27C;
  s3 = s5 + 1; // 0x0001
label8007B278:
  v0 = -1; // 0xFFFFFFFF
label8007B27C:
  temp = s3 == v0;
  a3 = 0;
  if (temp) goto label8007B818;
  t0 = 0;
  v0 = s3 << 1;
  v0 += s3;
  v1 = lw(0x80075708);
  v0 = v0 << 2;
  a0 = v0 + v1;
  v0 = 1; // 0x0001
  sh(a0 + 0x000A, v0);
  v0 = lw(0x80075878);
  v0 = (int32_t)v0 < (int32_t)s3;
  temp = v0 == 0;
  t1 = 16; // 0x0010
  if (temp) goto label8007B2C8;
  sw(0x80075878, s3);
label8007B2C8:
  v0 = lw(0x80075878);
  v1 = lw(0x800757DC);
  v0 = (int32_t)v1 < (int32_t)v0;
  temp = v0 == 0;
  v0 = 256; // 0x0100
  if (temp) goto label8007B2F0;
  sw(0x80075878, v1);
label8007B2F0:
  sw(sp + 0x0020, v0);
  v1 = lh(a0 + 0x0004);
  v0 = 4787; // 0x12B3
  sw(sp + 0x0028, v0);
  v0 = 14; // 0x000E
  sw(sp + 0x0030, v0);
  v0 = 1; // 0x0001
  sw(sp + 0x0034, v0);
  v0 = 5632; // 0x1600
  sw(sp + 0x0038, v0);
  sw(sp + 0x0024, v1);
  a2 = lw(a0 + 0x0000);
  v1 = lbu(a2 + 0x0000);
  temp = v1 == 0;
  t2 = 256; // 0x0100
  if (temp) goto label8007B60C;
label8007B334:
  v1 = v1 & 0xFF;
  v0 = 32; // 0x0020
  temp = v1 == v0;
  if (temp) goto label8007B5CC;
label8007B344:
  a1 = lw(0x800756E4);
  s1 = a3;
  v0 = (int32_t)s1 < (int32_t)a1;
  temp = v0 == 0;
  v0 = s1 << 3;
  if (temp) goto label8007B3BC;
  a0 = lw(0x8007589C);
  v0 -= s1;
  v0 = v0 << 2;
  v1 = v0 + a0;
label8007B370:
  v0 = lbu(v1 + 0x0004);
  temp = v0 == 0;
  if (temp) goto label8007B390;
  s1++;
  v0 = (int32_t)s1 < (int32_t)a1;
  temp = v0 != 0;
  v1 += 28; // 0x001C
  if (temp) goto label8007B370;
label8007B390:
  v0 = lw(0x800756E4);
  v0 = (int32_t)s1 < (int32_t)v0;
  temp = v0 == 0;
  v0 = s1 << 3;
  if (temp) goto label8007B3BC;
  a3 = s1;
  v0 -= a3;
  v0 = v0 << 2;
  a0 += v0;
  goto label8007B3E8;
label8007B3BC:
  v0 = lw(0x800756E4);
  a0 = lw(0x8007589C);
  v1 = v0 << 3;
  v1 -= v0;
  v1 = v1 << 2;
  a0 += v1;
  v0++;
  sw(0x800756E4, v0);
label8007B3E8:
  v1 = lbu(a2 + 0x0000);
  v0 = v1 - 65; // 0xFFFFFFBF
  v0 = v0 < 26;
  temp = v0 == 0;
  v0 = v1 - 97; // 0xFFFFFF9F
  if (temp) goto label8007B404;
  s4 = 1; // 0x0001
label8007B404:
  v0 = v0 < 26;
  temp = v0 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label8007B414;
  s4 = 0;
label8007B414:
  sb(a0 + 0x0004, v0);
  v0 = lw(0x800756E4);
  v0 = (int32_t)v0 < 101;
  temp = v0 != 0;
  sh(a0 + 0x0018, s3);
  if (temp) goto label8007B438;
  a0 = 0;
  psx_exit(a0);
label8007B438:
  v0 = lw(sp + 0x0020);
  sh(a0 + 0x0012, v0);
  v0 = lw(sp + 0x0024);
  sh(a0 + 0x0014, v0);
  v0 = lw(sp + 0x0028);
  temp = s4 != 0;
  sh(a0 + 0x0016, v0);
  if (temp) goto label8007B47C;
  v0 = lw(sp + 0x0034);
  v1 = lhu(a0 + 0x0014);
  v1 += v0;
  sh(a0 + 0x0014, v1);
  v0 = lw(sp + 0x0038);
  sh(a0 + 0x0016, v0);
label8007B47C:
  v1 = lbu(a2 + 0x0000);
  v0 = v1 - 48; // 0xFFFFFFD0
  v0 = v0 < 10;
  temp = v0 == 0;
  v0 = v1 + 212; // 0x00D4
  if (temp) goto label8007B49C;
  sh(a0 + 0x001A, v0);
  goto label8007B590;
label8007B49C:
  v0 = v1 - 65; // 0xFFFFFFBF
  v0 = v0 < 26;
  temp = v0 == 0;
  v0 = v1 + 361; // 0x0169
  if (temp) goto label8007B4B4;
  sh(a0 + 0x001A, v0);
  goto label8007B590;
label8007B4B4:
  v0 = v1 - 97; // 0xFFFFFF9F
  v0 = v0 < 26;
  temp = v0 == 0;
  v0 = v1 + 329; // 0x0149
  if (temp) goto label8007B4CC;
  sh(a0 + 0x001A, v0);
  goto label8007B590;
label8007B4CC:
  v1 = v1 & 0xFF;
  v0 = 33; // 0x0021
  temp = v1 != v0;
  v0 = 44; // 0x002C
  if (temp) goto label8007B4E8;
  v0 = 75; // 0x004B
  sh(a0 + 0x001A, v0);
  goto label8007B590;
label8007B4E8:
  temp = v1 != v0;
  v0 = 46; // 0x002E
  if (temp) goto label8007B4FC;
  v0 = 76; // 0x004C
  sh(a0 + 0x001A, v0);
  goto label8007B590;
label8007B4FC:
  temp = v1 != v0;
  v0 = 45; // 0x002D
  if (temp) goto label8007B510;
  v0 = 327; // 0x0147
  sh(a0 + 0x001A, v0);
  goto label8007B590;
label8007B510:
  temp = v1 != v0;
  v0 = 58; // 0x003A
  if (temp) goto label8007B524;
  v0 = 277; // 0x0115
  sh(a0 + 0x001A, v0);
  goto label8007B590;
label8007B524:
  temp = v1 != v0;
  v1 = 0x55550000;
  if (temp) goto label8007B560;
  v1 = lhu(a0 + 0x0016);
  v0 = 327; // 0x0147
  sh(a0 + 0x001A, v0);
  v1 += 1400; // 0x0578
  temp = t0 != 0;
  sh(a0 + 0x0016, v1);
  if (temp) goto label8007B54C;
  t0 = 1; // 0x0001
  goto label8007B344;
label8007B54C:
  v0 = lhu(a0 + 0x0014);
  t0 = 0;
  v0 -= 6; // 0xFFFFFFFA
  sh(a0 + 0x0014, v0);
  goto label8007B590;
label8007B560:
  v0 = 76; // 0x004C
  sh(a0 + 0x001A, v0);
  v0 = lw(sp + 0x0030);
  v1 = v1 | 0x5556;
  v0 = v0 << 1;
  mult(v0, v1);
  v0 = (int32_t)v0 >> 31;
  v1 = lhu(a0 + 0x0014);
  t3=hi;
  v0 = t3 - v0;
  v1 -= v0;
  sh(a0 + 0x0014, v1);
label8007B590:
  temp = s4 == 0;
  if (temp) goto label8007B5A4;
  v0 = lw(sp + 0x0020);
  v0 += t1;
  goto label8007B5B4;
label8007B5A4:
  v0 = lw(sp + 0x0020);
  v1 = lw(sp + 0x0030);
  v0 += v1;
label8007B5B4:
  sw(sp + 0x0020, v0);
  v0 = lbu(a2 + 0x0000);
  v0 -= 48; // 0xFFFFFFD0
  s4 = v0 < 10;
  goto label8007B5F8;
label8007B5CC:
  v0 = lw(sp + 0x0030);
  v1 = v0 << 1;
  v1 += v0;
  temp = (int32_t)v1 >= 0;
  s4 = 1; // 0x0001
  if (temp) goto label8007B5E8;
  v1 += 3; // 0x0003
label8007B5E8:
  v0 = lw(sp + 0x0020);
  v1 = (int32_t)v1 >> 2;
  v0 += v1;
  sw(sp + 0x0020, v0);
label8007B5F8:
  a2++;
  v1 = lbu(a2 + 0x0000);
  temp = v1 != 0;
  if (temp) goto label8007B334;
label8007B60C:
  v0 = lw(sp + 0x0020);
  s1 = 0;
  v0 -= t2;
  s6 = (int32_t)v0 >> 1;
  v0 = lw(0x800756E4);
  temp = (int32_t)v0 <= 0;
  s2 = 30; // 0x001E
  if (temp) goto label8007B818;
  s0 = 0;
label8007B634:
  v0 = lw(0x8007589C);
  a3 = s0 + v0;
  v0 = lh(a3 + 0x0018);
  temp = v0 != s3;
  a1 = 15; // 0x000F
  if (temp) goto label8007B800;
  a2 = lhu(a3 + 0x0012);
  a2 -= s6;
  v0 = a2 << 16;
  v0 = (int32_t)v0 >> 16;
  v0 -= 256; // 0xFFFFFF00
  v1 = v0 << 1;
  v1 += v0;
  v1 = -v1;
  div_psx(v1,s2);
  temp = s2 != 0;
  if (temp) goto label8007B688;
  BREAKPOINT; // BREAK 0x01C00
label8007B688:
  at = -1; // 0xFFFFFFFF
  temp = s2 != at;
  at = 0x80000000;
  if (temp) goto label8007B6A0;
  temp = v1 != at;
  if (temp) goto label8007B6A0;
  BREAKPOINT; // BREAK 0x01800
label8007B6A0:
  v1=lo;
  v0 = lh(a3 + 0x0016);
  v0 = v0 << 1;
  v0 = -v0;
  div_psx(v0,s2);
  temp = s2 != 0;
  if (temp) goto label8007B6C4;
  BREAKPOINT; // BREAK 0x01C00
label8007B6C4:
  at = -1; // 0xFFFFFFFF
  temp = s2 != at;
  at = 0x80000000;
  if (temp) goto label8007B6DC;
  temp = v0 != at;
  if (temp) goto label8007B6DC;
  BREAKPOINT; // BREAK 0x01800
label8007B6DC:
  v0=lo;
  mult(v1, s2);
  t1=lo;
  mult(v0, s2);
  sh(a3 + 0x0012, a2);
  a0 = 4; // 0x0004
  sh(a3 + 0x000C, v1);
  sh(a3 + 0x0010, v0);
  v0 = lhu(a3 + 0x0016);
  a2 -= t1;
  sh(a3 + 0x0012, a2);
  t0=lo;
  v0 -= t0;
  sh(a3 + 0x0016, v0);
  function_80037F10();
  a0 = 4; // 0x0004
  v1 = lw(0x8007589C);
  a1 = 15; // 0x000F
  v1 += s0;
  sb(v1 + 0x0008, v0);
  function_80037F10();
  a0 = 4; // 0x0004
  v1 = lw(0x8007589C);
  a1 = 15; // 0x000F
  v1 += s0;
  sb(v1 + 0x0009, v0);
  function_80037F10();
  v1 = lw(0x8007589C);
  v1 += s0;
  sb(v1 + 0x000A, v0);
  v1 = lw(0x8007589C);
  v1 += s0;
  v0 = lbu(v1 + 0x0008);
  v0 = -v0;
  mult(v0, s2);
  t0=lo;
  sb(v1 + 0x0005, t0);
  v1 = lw(0x8007589C);
  v1 += s0;
  v0 = lbu(v1 + 0x0009);
  v0 = -v0;
  mult(v0, s2);
  t0=lo;
  sb(v1 + 0x0006, t0);
  v1 = lw(0x8007589C);
  v1 += s0;
  v0 = lbu(v1 + 0x000A);
  v0 = -v0;
  mult(v0, s2);
  t0=lo;
  sb(v1 + 0x0007, t0);
  v0 = lw(0x8007589C);
  v0 += s0;
  sb(v0 + 0x000B, s2);
label8007B800:
  v0 = lw(0x800756E4);
  s1++;
  v0 = (int32_t)s1 < (int32_t)v0;
  temp = v0 != 0;
  s0 += 28; // 0x001C
  if (temp) goto label8007B634;
label8007B818:
  s5++;
  v0 = (int32_t)fp < (int32_t)s5;
  temp = v0 == 0;
  s7 += 12; // 0x000C
  if (temp) goto label8007B1A0;
label8007B828:
  v0 = lw(0x800756E4);
  temp = (int32_t)v0 <= 0;
  s1 = 0;
  if (temp) goto label8007B8D0;
  t0 = 2; // 0x0002
  a3 = 3; // 0x0003
  a2 = 4; // 0x0004
  a1 = 0;
label8007B84C:
  v0 = lw(0x8007589C);
  a0 = a1 + v0;
  v1 = lh(a0 + 0x0018);
  v0 = v1 << 1;
  v0 += v1;
  v1 = lw(0x80075708);
  v0 = v0 << 2;
  v0 += v1;
  v1 = lh(v0 + 0x000A);
  temp = v1 != t0;
  if (temp) goto label8007B8B0;
  v0 = lbu(a0 + 0x0004);
  temp = v0 != v1;
  if (temp) goto label8007B8A4;
  sb(a0 + 0x0004, a3);
  goto label8007B8B0;
label8007B8A4:
  temp = v0 != a2;
  if (temp) goto label8007B8B0;
  sb(a0 + 0x0004, 0);
label8007B8B0:
  v0 = lw(0x800756E4);
  s1++;
  v0 = (int32_t)s1 < (int32_t)v0;
  temp = v0 != 0;
  a1 += 28; // 0x001C
  if (temp) goto label8007B84C;
  v0 = lw(0x800756E4);
label8007B8D0:
  temp = (int32_t)v0 <= 0;
  s1 = 0;
  if (temp) goto label8007BB6C;
  s0 = 0;
label8007B8E0:
  v0 = lw(0x8007589C);
  a2 = s0 + v0;
  v1 = lbu(a2 + 0x0004);
  v0 = 1; // 0x0001
  temp = v1 == v0;
  v0 = 3; // 0x0003
  if (temp) goto label8007B910;
  temp = v1 == v0;
  if (temp) goto label8007BA74;
  goto label8007BB54;
label8007B910:
  v0 = lhu(a2 + 0x0012);
  a0 = lhu(a2 + 0x000C);
  v1 = lbu(a2 + 0x0005);
  a1 = lbu(a2 + 0x0008);
  v0 += a0;
  sh(a2 + 0x0012, v0);
  v0 = lhu(a2 + 0x0016);
  v1 += a1;
  sb(a2 + 0x0005, v1);
  v1 = lhu(a2 + 0x0010);
  a0 = lw(0x8007589C);
  v0 += v1;
  a0 += s0;
  sh(a2 + 0x0016, v0);
  v0 = lbu(a0 + 0x0006);
  v1 = lbu(a0 + 0x0009);
  v0 += v1;
  sb(a0 + 0x0006, v0);
  a0 = lw(0x8007589C);
  a0 += s0;
  v0 = lbu(a0 + 0x0007);
  v1 = lbu(a0 + 0x000A);
  v0 += v1;
  sb(a0 + 0x0007, v0);
  v1 = lw(0x8007589C);
  v1 += s0;
  v0 = lbu(v1 + 0x000B);
  v0--;
  sb(v1 + 0x000B, v0);
  v0 = v0 & 0xFF;
  temp = v0 != 0;
  a0 = 3; // 0x0003
  if (temp) goto label8007BB54;
  a1 = 11; // 0x000B
  v1 = lw(0x8007589C);
  v0 = 2; // 0x0002
  v1 += s0;
  sb(v1 + 0x0004, v0);
  function_80037F10();
  a0 = 6; // 0x0006
  v1 = lw(0x8007589C);
  a1 = 10; // 0x000A
  v1 += s0;
  sh(v1 + 0x000C, v0);
  function_80037EA0();
  a0 = -35; // 0xFFFFFFDD
  a1 = 35; // 0x0023
  v1 = lw(0x8007589C);
  v0 = -v0;
  v1 += s0;
  sh(v1 + 0x000E, v0);
  function_80037EA0();
  a0 = 4; // 0x0004
  v1 = lw(0x8007589C);
  a1 = 9; // 0x0009
  v1 += s0;
  sh(v1 + 0x0010, v0);
  function_80037F10();
  a0 = 4; // 0x0004
  v1 = lw(0x8007589C);
  a1 = 9; // 0x0009
  v1 += s0;
  sb(v1 + 0x0008, v0);
  function_80037F10();
  a0 = 4; // 0x0004
  v1 = lw(0x8007589C);
  a1 = 9; // 0x0009
  v1 += s0;
  sb(v1 + 0x0009, v0);
  function_80037F10();
  v1 = lw(0x8007589C);
  v1 += s0;
  sb(v1 + 0x000A, v0);
  goto label8007BB54;
label8007BA74:
  v0 = lhu(a2 + 0x0012);
  a0 = lhu(a2 + 0x000C);
  v1 = lhu(a2 + 0x0014);
  a1 = lhu(a2 + 0x000E);
  v0 += a0;
  sh(a2 + 0x0012, v0);
  v0 = lhu(a2 + 0x0016);
  a0 = lhu(a2 + 0x0010);
  v1 += a1;
  sh(a2 + 0x0014, v1);
  v1 = lbu(a2 + 0x0005);
  a1 = lbu(a2 + 0x0008);
  v0 += a0;
  sh(a2 + 0x0016, v0);
  v0 = lhu(a2 + 0x000E);
  v1 += a1;
  sb(a2 + 0x0005, v1);
  a0 = lw(0x8007589C);
  v0++;
  a0 += s0;
  sh(a2 + 0x000E, v0);
  v0 = lbu(a0 + 0x0006);
  v1 = lbu(a0 + 0x0009);
  v0 += v1;
  sb(a0 + 0x0006, v0);
  a0 = lw(0x8007589C);
  a0 += s0;
  v0 = lbu(a0 + 0x0007);
  v1 = lbu(a0 + 0x000A);
  v0 += v1;
  sb(a0 + 0x0007, v0);
  v0 = lw(0x8007589C);
  v1 = s0 + v0;
  v0 = lh(v1 + 0x000E);
  v0 = (int32_t)v0 < 21;
  temp = v0 != 0;
  v0 = 20; // 0x0014
  if (temp) goto label8007BB2C;
  sh(v1 + 0x000E, v0);
label8007BB2C:
  v0 = lw(0x8007589C);
  v1 = s0 + v0;
  v0 = lh(v1 + 0x0014);
  v0 = (int32_t)v0 < 281;
  temp = v0 != 0;
  v0 = 4; // 0x0004
  if (temp) goto label8007BB54;
  sb(v1 + 0x0004, v0);
label8007BB54:
  v0 = lw(0x800756E4);
  s1++;
  v0 = (int32_t)s1 < (int32_t)v0;
  temp = v0 != 0;
  s0 += 28; // 0x001C
  if (temp) goto label8007B8E0;
label8007BB6C:
  v1 = lw(0x800756E4);
  temp = (int32_t)v1 <= 0;
  v0 = v1 << 3;
  if (temp) goto label8007BBE4;
  v0 -= v1;
  v1 = lw(0x8007589C);
  v0 = v0 << 2;
  v0 += v1;
  v0 = lbu(v0 - 0x0018); // 0xFFFFFFE8
  temp = v0 != 0;
  if (temp) goto label8007BBE4;
  a0 = v1;
label8007BBA8:
  v0 = lw(0x800756E4);
  v1 = v0 - 1; // 0xFFFFFFFF
  sw(0x800756E4, v1);
  temp = (int32_t)v1 <= 0;
  v0 = v1 << 3;
  if (temp) goto label8007BBE4;
  v0 -= v1;
  v0 = v0 << 2;
  v0 += a0;
  v0 = lbu(v0 - 0x0018); // 0xFFFFFFE8
  temp = v0 == 0;
  if (temp) goto label8007BBA8;
label8007BBE4:
  v1 = lw(0x80075704);
  if (v1 == -1) goto label8007BF9C;
  if ((int32_t)v1 <= 0) goto label8007BC44;
  function_800163E4();
  if (lw(0x80076BB8)) goto label8007BF9C;
  if (CdSync(1, NULL) != CdlComplete) goto label8007BF9C;
  v1 = lw(0x80075704);
label8007BC44:
  if (v1 > 4) goto label8007BF9C;
  switch (lw(0x8007AA3C + v1*4))
  {
  case 0x8007BC64: // 0
    goto label8007BC64;
    break;
  case 0x8007BC9C: // 1
    goto label8007BC9C;
    break;
  case 0x8007BCF4: // 2
    goto label8007BCF4;
    break;
  case 0x8007BDC0: // 3
    goto label8007BDC0;
    break;
  case 0x8007BDD0: // 4
    goto label8007BDD0;
    break;
  default:
    JR(temp, 0x8007BC5C);
    return;
  }
label8007BC64:
  read_disk2(lw(WAD_sector), lw(0x800757EC), 0x800, lw(WAD_header + 0x290 + lw(0x800757AC)*8), 0x258);
  goto label8007BDA0;
label8007BC9C:
  spyro_memcpy32(WAD_nested_header, lw(0x800757EC), 0x1D0);
  a2 = 0x00020000;
  a0 = lw(WAD_sector);
  a1 = lw(0x800757EC);
  v0 = lw(0x800757AC);
  v1 = lw(WAD_nested_header);
  v0 = v0 << 3;
  a3 = lw(WAD_header + 0x0290 + v0);
  v0 = 600; // 0x0258
  sw(sp + 0x0010, v0);
  goto label8007BD94;
label8007BCF4:
  a0 = 0;
  v1 = 512; // 0x0200
  v0 = 384; // 0x0180
  sh(sp + 0x0022, v0);
  v0 = 128; // 0x0080
  sh(sp + 0x0020, v1);
  sh(sp + 0x0024, v1);
  sh(sp + 0x0026, v0);
  v0 = DrawSync(a0);
  a1 = lw(0x800757EC);
  a0 = sp + 32; // 0x0020
  v0 = LoadImage(addr_to_pointer(a0), addr_to_pointer(a1));
  a0 = 0;
  v0 = DrawSync(a0);
  v1 = 0x00040000;
  a1 = lw(0x800757EC);
  a0 = lw(WAD_sector);
  v0 = lw(WAD_nested_header + 0x001C);
  a2 = lw(WAD_nested_header + 0x000C);
  v0 += v1;
  a2 += v0;
  v0 = lw(0x800757AC);
  v1 = lw(WAD_nested_header);
  sw(0x80075958, a2);
  v0 = v0 << 3;
  a3 = lw(WAD_header + 0x0290 + v0);
  v0 = 600; // 0x0258
  sw(sp + 0x0010, v0);
  v1 += a3;
  a3 = 0x00020000;
label8007BD94:
  a3 += v1;
  read_disk2(a0, a1, a2, a3, lw(sp+0x10));
label8007BDA0:
  v0 = lw(0x80075704);
  v0++;
  sw(0x80075704, v0);
  goto label8007BF9C;
label8007BDC0:
  sw(0x8007576C, 0);
  goto label8007BF9C;
label8007BDD0:
  s1 = 512; // 0x0200
  s2 = 128; // 0x0080
  v1 = lw(0x80075768);
  v0 = 256; // 0x0100
  sh(sp + 0x0028, s1);
  sh(sp + 0x002A, s2);
  sh(sp + 0x002C, s1);
  sh(sp + 0x002E, v0);
  sw(lib_end_ptr, v1);
  sw(0x800785DC, v1);
  DrawSync(0);
  s0 = sp + 0x28;
  a1 = lw(0x800757EC);
  LoadImage(addr_to_pointer(sp + 0x28), addr_to_pointer(a1));
  a0 = sp + 0x28;
  a1 = 512; // 0x0200
  a2 = 0;
  v0 = 384; // 0x0180
  sh(sp + 0x0028, s1);
  sh(sp + 0x002A, v0);
  sh(sp + 0x002C, s1);
  sh(sp + 0x002E, s2);
  MoveImage(addr_to_pointer(sp + 0x28), a1, a2);
  DrawSync(0);
  s0 = 0x40000;
  a0 = lw(0x80075768);
  a1 = lw(0x800757EC);
  a2 = lw(0x80075958);
  a1 += 0x40000;
  a2 -= 0x40000;
  spyro_memcpy32(a0, a1, a2);
  v0 = lw(0x80075768);
  v1 = lw(0x80075958);
  a0 = lw(0x800785DC);
  v0 += v1;
  v0 -= 0x40000;
  sw(0x800757EC, v0);
  a1 = 1; // 0x0001
  function_80012D58();
  v1 = 0x66660000;
  a0 = lw(0x800757AC);
  v1 = v1 | 0x6667;
  sw(0x800785E0, v0);
  mult(a0, v1);
  v0 = lw(0x800785DC);
  a3 = 0x80077780;
  a2 = SKYBOX_DATA;
  v1 = lw(a3 + 0x0000);
  a1 = lw(a3 + 0x0004);
  sw(a2 + 0x0000, v1);
  sw(a2 + 0x0004, a1);
  v1 = lw(a3 + 0x0008);
  a1 = lw(a3 + 0x000C);
  sw(a2 + 0x0008, v1);
  sw(a2 + 0x000C, a1);
  v1 = lw(a3 + 0x0010);
  sw(a2 + 0x0010, v1);
  v1 = lw(WAD_nested_header + 0x000C);
  a1 = lw(WAD_nested_header + 0x001C);
  a2 = lw(WAD_nested_header + 0x0018);
  v0 += v1;
  sw(0x800785E4, v0);
  sw(0x800757BC, v0);
  v0 = (int32_t)a0 >> 31;
  sw(0x80078604, a1);
  sw(0x80078600, a2);
  t3=hi;
  v1 = (int32_t)t3 >> 2;
  v1 -= v0;
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 1;
  a0 -= v0;
  a0 = (int32_t)a0 < 9;
  temp = a0 == 0;
  v0 = 3; // 0x0003
  if (temp) goto label8007BF7C;
  sw(0x80075704, 0);
  goto label8007BF84;
label8007BF7C:
  sw(0x80075704, v0);
label8007BF84:
  v0 = lw(0x800757AC);
  v0++;
  sw(0x800757AC, v0);
label8007BF9C:
  ra = lw(sp + 0x0084);
  fp = lw(sp + 0x0080);
  s7 = lw(sp + 0x007C);
  s6 = lw(sp + 0x0078);
  s5 = lw(sp + 0x0074);
  s4 = lw(sp + 0x0070);
  s3 = lw(sp + 0x006C);
  s2 = lw(sp + 0x0068);
  s1 = lw(sp + 0x0064);
  s0 = lw(sp + 0x0060);
  sp += 136; // 0x0088
  return;
}