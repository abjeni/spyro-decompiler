#include "psx_mem.h"
#include "main.h"
#include "decompilation.h"
#include "psx_bios.h"
#include "spyro_system.h"
#include "spyro_string.h"
#include "not_renamed.h"

// size: 0x0000002C
void function_8005C6C8(void)
{
  a0 = lw(SPU_DELAY);
  sw(a0, (lw(a0) & 0xF0FFFFFF) | 0x20000000);
}

// size: 0x0000002C
void function_8005C6F4(void)
{
  a0 = lw(SPU_DELAY);
  sw(a0, (lw(a0) & 0xF0FF0FFFF) | 0x22000000);
}

// size: 0x000000A4
void function_8005C588(void)
{
  a2 = lw(0x80073580);
  if (lw(0x80073578) && a1 % a2)
    a1 = (a1 + a2) & ~lw(0x80073584);
    
  a3 = a1 >> lw(0x8007357C);
  if (a0 == -2) {
    v0 = a1;
  } else if (a0 == -1) {
    v0 = a3 & 0xFFFF;
  } else {
    sh(lw(spu_registers_base_ptr) + a0*2, a3);
    v0 = a1;
  }
}

// size: 0x000000A8
uint32_t test_spu_event(uint32_t sync)
{
  if (lw(0x800730F0) == 1 || lw(0x80073588) == 1)
    return 1;
  
  if (sync == 1) {
    while (TestEvent(lw(spu_event)) == 0);
    sw(0x80073588, 1);
    return 1;
  }

  if (TestEvent(lw(spu_event)) == 1) {
    sw(0x80073588, 1);
    return 1;
  }

  return 0;
}

// size: 0x000000A8
void function_8005CBB0(void)
{
  BREAKPOINT;
  v0 = test_spu_event(a0);
}

// size: 0x00000294
void function_8005BBF4(void)
{
  sp -= 0x20;
  sw(sp + 0x10, s0);
  
  s0 = a0;

  a0 = lw(DMA_control_register_copy_4_ptr);
  sw(sp + 0x18, ra);
  sw(sp + 0x14, s1);
  sw(a0, lw(a0) | 0x000B0000);
  sw(0x80073570, 0);
  sw(0x80073574, 0);
  sh(0x8007356C, 0);

  v0 = lw(spu_registers_base_ptr);
  sh(v0 + 0x0180, 0);
  sh(v0 + 0x0182, 0);
  sh(v0 + 0x01AA, 0);

  ra = 0x8005BC54;
  do_literally_nothing();

  v0 = lw(spu_registers_base_ptr);
  sh(v0 + 0x0180, 0);
  sh(v0 + 0x0182, 0);
  v1 = 0;
  // spu status register
  while (lhu(lw(spu_registers_base_ptr) + 0x01AE) & 0x7FF) {
    v1++;
    if (v1 > 3840) {
      printf("SPU:T/O [%s]\n", "wait (reset)");
      break;
    }
  }
  sw(0x80073578, 2);
  sw(0x8007357C, 3);
  sw(0x80073580, 8);
  sw(0x80073584, 7);

  v0 = lw(spu_registers_base_ptr);
  sh(v0 + 0x01AC, 4);
  sh(v0 + 0x0184, 0);
  sh(v0 + 0x0186, 0);
  sh(v0 + 0x018C, 0xFFFF);
  sh(v0 + 0x018E, 0xFFFF);
  sh(v0 + 0x0198, 0);
  sh(v0 + 0x019A, 0);
  
  for (int i = 0; i < 10; i++)
    sh(0x800777A8 + 2*i, 0);
  
  if (s0 == 0) {
    v0 = lw(spu_registers_base_ptr);
    sh(0x8007356C, 0x200);
    sh(v0 + 0x0190, 0);
    sh(v0 + 0x0192, 0);
    sh(v0 + 0x0194, 0);
    sh(v0 + 0x0196, 0);
    sh(v0 + 0x01B0, 0);
    sh(v0 + 0x01B2, 0);
    sh(v0 + 0x01B4, 0);
    sh(v0 + 0x01B6, 0);

    a0 = 0x80073594;
    a1 = 0x10;
    ra = 0x8005BD94;
    function_8005BE88();

    v1 = lw(spu_registers_base_ptr);
    for (int i = 0; i < 24; i++)
    {
      sh(v1 + 0x10*i + 0x00, 0);
      sh(v1 + 0x10*i + 0x02, 0);
      sh(v1 + 0x10*i + 0x04, 0x3FFF);
      sh(v1 + 0x10*i + 0x06, 0x200);
      sh(v1 + 0x10*i + 0x08, 0);
      sh(v1 + 0x10*i + 0x0A, 0);
    }
    sh(lw(spu_registers_base_ptr) + 0x0188, 0xFFFF);
    sh(lw(spu_registers_base_ptr) + 0x018A, 0xFF);
    do_literally_nothing();
    do_literally_nothing();
    do_literally_nothing();
    do_literally_nothing();
    sh(lw(spu_registers_base_ptr) + 0x018C, 0xFFFF);
    sh(lw(spu_registers_base_ptr) + 0x018E, 0xFF);
    do_literally_nothing();
    do_literally_nothing();
    do_literally_nothing();
    do_literally_nothing();
  }
  v0 = 0;
  a0 = lw(spu_registers_base_ptr);
  sw(0x80073588, 1);
  sh(a0 + 0x01AA, 0xC000);
  sw(0x8007358C, 0);
  sw(0x80073590, 0);

  ra = lw(sp + 0x18);
  s1 = lw(sp + 0x14);
  s0 = lw(sp + 0x10);
  sp += 0x20;
  return;
}

// size: 0x00000024
void dma_spu_callback(uint32_t callback)
{
  dma_callback(4, callback);
}

// size: 0x00000024
void function_8005C788(void)
{
  BREAKPOINT;
  dma_spu_callback(a0);
}

// size: 0x0000007C
void spu_enable_irq(void)
{
  if (lw(0x80073550)) return;
  
  sw(0x80073550, 1);
  EnterCriticalSection();
    dma_spu_callback(0x8005C054);
    
    uint32_t event = OpenEvent(0xF0000009, 0x20, 0x2000, 0);
    sw(spu_event, event);
    EnableEvent(event);
  ExitCriticalSection();
}

// size: 0x0000007C
void function_8005BB78(void)
{
  BREAKPOINT;
  spu_enable_irq();
}

// size: 0x00000048
void function_8005C540(void)
{
  if (a2)
    sh(lw(spu_registers_base_ptr) + a0*2, a1 >> lw(0x8007357C));
  else
    sh(lw(spu_registers_base_ptr) + a0*2, a1);
}

// size: 0x000000F8
void function_8005BA80(void)
{
  sp -= 0x18;
  sw(sp + 0x10, s0);
  sw(sp + 0x14, ra);
  s0 = a0;

  ra = 0x8005BA94;
  function_8005DDC8();
  a0 = s0;
  ra = 0x8005BA9C;
  function_8005BBF4();
  if (s0 == 0) {
    v0 = 0x8007314A;
    for (int i = 0; i < 24; i++)
      sh(v0-2*i, 0xC000);
  }
  spu_enable_irq();
  sw(0x800730F4, 0);
  sw(0x800730F8, 0);
  sw(0x80073104, 0);
  sh(0x80073108, 0);
  sh(0x8007310A, 0);
  sw(0x8007310C, 0);
  sw(0x80073110, 0);
  sw(0x800730FC, a1);
  a0 = 209;
  a1 = lw(0x800735BC);
  a2 = 0;
  ra = 0x8005BB1C;
  function_8005C540();
  sw(0x800735B0, 0);
  sw(0x800735B4, 0);
  sw(0x800735B8, 0);
  sw(0x800730F0, 0);
  sw(0x80073570, 0);
  sw(0x800730EC, 0);
  sw(0x80073118, 0);
  sw(0x80073114, 0);
  sw(0x8007354C, 0);
  ra = lw(sp + 0x14);
  s0 = lw(sp + 0x10);
  sp += 0x18;
  return;
}

// size: 0x00000020
void function_8005BA60(void)
{
  a0 = 0;
  function_8005BA80();
}

// size: 0x00000034
void function_8005CB7C(void)
{
  sw(0x800730F0, a0);
  sw(0x80073570, a0 == 1);
}

// size: 0x0000011C
void init_spu(void)
{
  sp -= 0x60;
  sw(sp + 0x5C, ra);
  sw(sp + 0x58, s0);

  ra = 0x8005596C;
  function_8005BA60();
  s0 = 0x80075F08;
  sw(s0, 195);
  sw(0x80076228, 0x3FFF);
  sw(0x8007622C, 0x1000);
  sh(0x80075F0C, 0x3CCC);
  sh(0x80075F0E, 0x3CCC);
  sw(0x80076224, 0x5000);
  sh(0x80075F18, 0x5000);
  sh(0x80075F1A, 0x5000);
  sw(0x800761D8, -1);
  a0 = s0;
  ra = 0x800559D8;
  function_8005CC58();
  a0 = sp + 0x18;
  sw(sp + 0x1C, 0xFF13);
  sh(sp + 0x20, 0x2FFF);
  sh(sp + 0x22, 0x2FFF);
  sh(sp + 0x2C, 0x400);
  sw(sp + 0x3C, 1);
  sw(sp + 0x40, 1);
  sw(sp + 0x44, 3);
  sw(sp + 0x18, 0xFFFFFF);
  sh(sp + 0x48, 0);
  sh(sp + 0x4A, 0);
  sh(sp + 0x4C, 0);
  sh(sp + 0x4E, 0);
  sh(sp + 0x50, 15);
  ra = 0x80055A34;
  function_8005C7AC();
  a0 = 0;
  a1 = 0x00FFFFFF;
  ra = 0x80055A44;
  function_8005C7D4();
  a0 = 0;
  ra = 0x80055A4C;
  function_8005CB7C();
  spyro_memset32(0x80075F30, 0, 0x2A0);
  sw(0x80076240, 0);

  ra = lw(sp + 0x5C);
  s0 = lw(sp + 0x58);
  sp += 0x60;
  return;
}

void function_8005595C(void)
{
  BREAKPOINT;
  init_spu();
}