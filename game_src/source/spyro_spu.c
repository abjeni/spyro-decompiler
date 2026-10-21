#include "psx_mem.h"
#include "main.h"
#include "decompilation.h"
#include "psx_bios.h"
#include "spyro_system.h"
#include "spyro_string.h"
#include "not_renamed.h"
#include "int_math.h"
#include "psx_ops.h"
#include "spyro_math.h"
#include "spyro_psy.h"

const uint32_t spu_registers_base = 0x1F801C00;

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
  if (a0 == (uint32_t)-2) {
    v0 = a1;
  } else if (a0 == (uint32_t)-1) {
    v0 = a3 & 0xFFFF;
  } else {
    sh(spu_registers_base + a0*2, a3);
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
  UNREACHABLE;
  v0 = test_spu_event(a0);
}

// size: 0x000001CC
void sound_ram_data_transfer(uint16_t *data, size_t len)
{
  sh(spu_registers_base + 0x1A6, lhu(0x8007356C)); // 1F801DA6h 2  Sound RAM Data Transfer Address
  const uint32_t original_status = lhu(spu_registers_base + 0x1AE) & 0x7FF;

  do_literally_nothing();

  while (len) {
    size_t chunk_size = min_int(len, 0x40);
    len -= chunk_size;

    v1 = 0;
    for (size_t i = 0; i < chunk_size; i++) {
      sh(spu_registers_base + 0x1A8, *data++);
    }

    sh(spu_registers_base + 0x1AA, (lhu(spu_registers_base + 0x1AA) & 0xFFCF) | 0x10);
    
    do_literally_nothing();

    v1 = 0;
    while (lhu(spu_registers_base + 0x1AE) & 0x400) {
      v1++;
      if (v1 > 0xF00) {
        printf("SPU:T/O [%s]\n", "wait (wrdy H -> L)");
        break;
      }
    }

    do_literally_nothing();
    do_literally_nothing();
  }

  sh(spu_registers_base + 0x01AA, lhu(spu_registers_base + 0x01AA) & 0xFFCF);
  
  v1 = 0;
  while ((lhu(spu_registers_base + 0x01AE) & 0x7FF) != original_status)  {
    v1++;
    if (v1 > 0xF00) {
      printf("SPU:T/O [%s]\n", "wait (dmaf clear/W)");
      break;
    }
  }
}

// size: 0x000001CC
void function_8005BE88(void)
{
  UNREACHABLE;
  sound_ram_data_transfer(addr_to_pointer(a0), a1);
}

// size: 0x00000294
void function_8005BBF4(void)
{
  sp -= 0x20;
  sw(sp + 0x10, s0);
  sw(sp + 0x18, ra);
  sw(sp + 0x14, s1);
  
  s0 = a0;

  a0 = lw(DMA_control_register_copy_4_ptr);
  sw(a0, lw(a0) | 0x000B0000);
  sw(0x80073570, 0);
  sw(0x80073574, 0);
  sh(0x8007356C, 0);

  sh(spu_registers_base + 0x180, 0);
  sh(spu_registers_base + 0x182, 0);
  sh(spu_registers_base + 0x1AA, 0);

  do_literally_nothing();

  sh(spu_registers_base + 0x180, 0);
  sh(spu_registers_base + 0x182, 0);
  v1 = 0;
  // spu status register
  while (lhu(spu_registers_base + 0x01AE) & 0x7FF) {
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
  
  sh(spu_registers_base + 0x1AC, 4);
  sh(spu_registers_base + 0x184, 0);
  sh(spu_registers_base + 0x186, 0);
  sh(spu_registers_base + 0x18C, 0xFFFF);
  sh(spu_registers_base + 0x18E, 0xFFFF);
  sh(spu_registers_base + 0x198, 0);
  sh(spu_registers_base + 0x19A, 0);
  
  for (int i = 0; i < 10; i++)
    sh(0x800777A8 + i*2, 0);
  
  if (s0 == 0) {
    sh(0x8007356C, 0x200);
    sh(spu_registers_base + 0x190, 0);
    sh(spu_registers_base + 0x192, 0);
    sh(spu_registers_base + 0x194, 0);
    sh(spu_registers_base + 0x196, 0);
    sh(spu_registers_base + 0x1B0, 0);
    sh(spu_registers_base + 0x1B2, 0);
    sh(spu_registers_base + 0x1B4, 0);
    sh(spu_registers_base + 0x1B6, 0);

    sound_ram_data_transfer(addr_to_pointer(0x80073594), 0x10);

    for (int i = 0; i < 24; i++)
    {
      sh(spu_registers_base + 0x10*i + 0x00, 0);
      sh(spu_registers_base + 0x10*i + 0x02, 0);
      sh(spu_registers_base + 0x10*i + 0x04, 0x3FFF);
      sh(spu_registers_base + 0x10*i + 0x06, 0x200);
      sh(spu_registers_base + 0x10*i + 0x08, 0);
      sh(spu_registers_base + 0x10*i + 0x0A, 0);
    }

    sh(spu_registers_base + 0x188, 0xFFFF);
    sh(spu_registers_base + 0x18A, 0xFF);

    do_literally_nothing();
    do_literally_nothing();
    do_literally_nothing();
    do_literally_nothing();

    sh(spu_registers_base + 0x18C, 0xFFFF);
    sh(spu_registers_base + 0x18E, 0xFF);

    do_literally_nothing();
    do_literally_nothing();
    do_literally_nothing();
    do_literally_nothing();
  }
  sw(0x80073588, 1);
  sh(spu_registers_base + 0x1AA, 0xC000);
  sw(0x8007358C, 0);
  sw(0x80073590, 0);

  v0 = 0;

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
  UNREACHABLE;
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
  UNREACHABLE;
  spu_enable_irq();
}

// size: 0x00000048
void function_8005C540(void)
{
  if (a2)
    sh(spu_registers_base + a0*2, a1 >> lw(0x8007357C));
  else
    sh(spu_registers_base + a0*2, a1);
}

// size: 0x000000F8
void function_8005BA80(void)
{
  sp -= 0x18;
  sw(sp + 0x10, s0);
  sw(sp + 0x14, ra);
  s0 = a0;

  init_hook_entry_int2();

  a0 = s0;
  function_8005BBF4();

  if (s0 == 0)
    for (int i = 0; i < 24; i++)
      sh(0x8007314A - i*2, 0xC000);
  
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
  function_8005C7AC();
  a0 = 0;
  a1 = 0x00FFFFFF;
  function_8005C7D4();
  a0 = 0;
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
  UNREACHABLE;
  init_spu();
}

// size: 0x00000290
void function_8005C1C0(void)
{
  uint32_t temp;
  sw(sp + 0x0000, a0);
  sw(sp + 0x0004, a1);
  sw(sp + 0x0008, a2);
  sw(sp + 0x000C, a3);
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, s0);
  s0 = sp + 28; // 0x001C
  a2 = 1; // 0x0001
  sw(sp + 0x0014, ra);
  temp = a0 == a2;
  sw(sp + 0x0018, a0);
  if (temp) goto label8005C250;
  v0 = (int32_t)a0 < 2;
  temp = v0 == 0;
  if (temp) goto label8005C208;
  temp = a0 == 0;
  v0 = 0;
  if (temp) goto label8005C2C0;
  goto label8005C43C;
label8005C208:
  v0 = 2; // 0x0002
  temp = a0 == v0;
  v0 = 3; // 0x0003
  if (temp) goto label8005C224;
  temp = a0 == v0;
  v0 = 0;
  if (temp) goto label8005C32C;
  goto label8005C43C;
label8005C224:
  a0 = lw(sp + 0x001C);
  v0 = lw(0x8007357C); // &0x00000003
  v0 = a0 >> v0;
  sh(0x8007356C, v0); // &0x00000000
  sh(spu_registers_base + 0x01A6, v0);
  v0 = 0;
  goto label8005C43C;
label8005C250:
  a0 = lhu(0x8007356C); // &0x00000000
  v0 = lhu(spu_registers_base + 0x01A6);
  sw(0x800735A4, 0); // &0x00000000
  v0 = v0 & 0xFFFF;
  temp = v0 == a0;
  v1 = 0;
  if (temp) goto label8005C298;
  v1++;
label8005C27C:
  v0 = v1 < 3841;
  temp = v0 == 0;
  v0 = -2; // 0xFFFFFFFE
  if (temp) goto label8005C43C;
  v0 = lhu(spu_registers_base + 0x01A6);
  temp = v0 != a0;
  v1++;
  if (temp) goto label8005C27C;
label8005C298:
  a0 = lhu(spu_registers_base + 0x01AA);
  v0 = a0 & 0xFFCF;
  a0 = v0 | 0x20;
  sh(spu_registers_base + 0x01AA, a0);
  v0 = 0;
  goto label8005C43C;
label8005C2C0:
  a0 = lhu(0x8007356C); // &0x00000000
  v0 = lhu(spu_registers_base + 0x01A6);
  sw(0x800735A4, a2); // &0x00000000
  v0 = v0 & 0xFFFF;
  temp = v0 == a0;
  v1 = 0;
  if (temp) goto label8005C308;
  v1++;
label8005C2EC:
  v0 = v1 < 3841;
  temp = v0 == 0;
  v0 = -2; // 0xFFFFFFFE
  if (temp) goto label8005C43C;
  v0 = lhu(spu_registers_base + 0x01A6);
  temp = v0 != a0;
  v1++;
  if (temp) goto label8005C2EC;
label8005C308:
  sh(spu_registers_base + 0x01AA, lhu(spu_registers_base + 0x01AA) | 0x30);
  v0 = 0;
  goto label8005C43C;
label8005C32C:
  v0 = lw(0x800735A4); // &0x00000000
  temp = v0 != a2;
  a0 = 32; // 0x0020
  if (temp) goto label8005C344;
  a0 = 48; // 0x0030
label8005C344:
  v1 = 0;
  v0 = lhu(spu_registers_base + 0x01AA);
  a0 = a0 & 0xFFFF;
  v0 = v0 & 0x30;
  temp = v0 == a0;
  v1++;
  if (temp) goto label8005C384;
label8005C364:
  v0 = v1 < 3841;
  temp = v0 == 0;
  v0 = -2; // 0xFFFFFFFE
  if (temp) goto label8005C43C;
  v0 = lhu(spu_registers_base + 0x01AA);
  v0 = v0 & 0x30;
  temp = v0 != a0;
  v1++;
  if (temp) goto label8005C364;
label8005C384:
  v1 = lw(0x800735A4); // &0x00000000
  v0 = 1; // 0x0001
  temp = v1 != v0;
  if (temp) goto label8005C3A8;
  s0 += 4; // 0x0004
  function_8005C6F4();
  a2 = 0x01000000;
  goto label8005C3B4;
label8005C3A8:
  s0 += 4; // 0x0004
  function_8005C6C8();
  a2 = 0x01000000;
label8005C3B4:
  a0 = lw(s0 - 0x0004); // 0xFFFFFFFC
  sw(0x800735A8, a0); // &0x00000000
  a0 = lw(s0 + 0x0000);
  a1 = lw(DMA_SPU_address_ptr);
  v1 = a0 >> 6;
  v0 = a0 & 0x3F;
  v0 = 0 < v0;
  a0 = lw(0x800735A8); // &0x00000000
  v1 += v0;
  sw(0x800735AC, v1); // &0x00000000
  sw(a1 + 0x0000, a0);
  v0 = lw(0x800735AC); // &0x00000000
  v1 = lw(DMA_SPU_block_control_ptr);
  v0 = v0 << 16;
  v0 = v0 | 0x10;
  sw(v1 + 0x0000, v0);
  v1 = lw(0x800735A4); // &0x00000000
  v0 = 1; // 0x0001
  temp = v1 != v0;
  a2 = a2 | 0x201;
  if (temp) goto label8005C428;
  a2 = 0x01000200;
label8005C428:
  v0 = lw(DMA_SPU_channel_control_ptr);
  sw(v0 + 0x0000, a2);
  v0 = 0;
label8005C43C:
  ra = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x00000088
void function_8005C450(void)
{
  uint32_t temp;
  sp -= 32; // 0xFFFFFFE0
  sw(sp + 0x14, s1);
  sw(sp + 0x10, s0);
  sw(sp + 0x18, ra);

  v0 = lw(0x80073570);
  s1 = a0;
  s0 = a1;
  temp = v0 != 0;
  if (temp) goto label8005C4B0;
  a0 = 2;
  a1 = lhu(0x8007356C) << lw(0x8007357C);
  function_8005C1C0();
  a0 = 1; // 0x0001
  function_8005C1C0();
  a0 = 3; // 0x0003
  a1 = s1;
  a2 = s0;
  function_8005C1C0();
  v0 = s0;
  goto label8005C4C0;
label8005C4B0:
  sound_ram_data_transfer(addr_to_pointer(s1), s0);
  v0 = s0;
label8005C4C0:
  ra = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 32; // 0x0020
  return;
}

// size: 0x00000068
void function_8005C4D8(void)
{
  sp -= 32; // 0xFFFFFFE0
  sw(sp + 0x0014, s1);
  s1 = a0;
  sw(sp + 0x0010, s0);
  s0 = a1;
  v0 = lhu(0x8007356C); // &0x00000000
  a1 = lw(0x8007357C); // &0x00000003
  a0 = 2; // 0x0002
  sw(sp + 0x0018, ra);
  a1 = v0 << a1;
  function_8005C1C0();
  a0 = 0;
  function_8005C1C0();
  a0 = 3; // 0x0003
  a1 = s1;
  a2 = s0;
  function_8005C1C0();
  v0 = s0;
  ra = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 32; // 0x0020
  return;
}

// size: 0x000001FC
void function_8005C7D4(void)
{
  uint32_t temp;
  v0 = 0x00FFFFFF;
  a1 = a1 & v0;
  a2 = a1;
  t0 = a1 >> 16;
  temp = a0 == 0;
  a3 = t0;
  if (temp) goto label8005C8E4;
  v0 = 1; // 0x0001
  temp = a0 != v0;
  if (temp) goto label8005C9C8;
  v0 = lw(0x8007354C); // &0x00000000
  v0 = v0 & 0x1;
  temp = v0 == 0;
  if (temp) goto label8005C8C0;
  v0 = 0x800777A8;
  sh(v0 + 0x0000, a2);
  sh(0x800777AA, a3);
  v0 = lw(0x80073118); // &0x00000000
  v0 = v0 | 0x1;
  sw(0x80073118, v0); // &0x00000000
  v0 = lw(0x80073114); // &0x00000000
  v0 = v0 | a1;
  sw(0x80073114, v0); // &0x00000000
  v0 = lhu(0x800777AC);
  v0 = v0 & a1;
  temp = v0 == 0;
  if (temp) goto label8005C888;
  v0 = lhu(0x800777AC);
  v1 = ~a1;
  v0 = v0 & v1;
  sh(0x800777AC, v0);
label8005C888:
  v0 = lhu(0x800777AE);
  v0 = v0 & t0;
  temp = v0 == 0;
  if (temp) goto label8005C9C8;
  v0 = lhu(0x800777AE);
  v1 = ~t0;
  v0 = v0 & v1;
  sh(0x800777AE, v0);
  goto label8005C9C8;
label8005C8C0:
  sw(0x800730EC, lw(0x800730EC) | a1);
  sh(spu_registers_base + 0x188, a2); // Key ON 24 bitfield
  sh(spu_registers_base + 0x18A, a3); // ????
  goto label8005C9C0;
label8005C8E4:
  v0 = lw(0x8007354C) & 1;
  temp = v0 == 0;
  if (temp) goto label8005C99C;
  sh(0x800777AC, a2);
  sh(0x800777AE, a3);
  v0 = lw(0x80073118); // &0x00000000
  a0 = ~a1;
  v0 = v0 | 0x1;
  sw(0x80073118, v0); // &0x00000000
  v0 = lw(0x80073114); // &0x00000000
  v1 = 0x800777A8;
  v0 = v0 & a0;
  sw(0x80073114, v0); // &0x00000000
  v0 = lhu(v1 + 0x0000);
  v0 = v0 & a1;
  temp = v0 == 0;
  if (temp) goto label8005C964;
  v0 = lhu(v1 + 0x0000);
  v0 = v0 & a0;
  sh(v1 + 0x0000, v0);
label8005C964:
  v0 = lhu(0x800777AA);
  v0 = v0 & t0;
  temp = v0 == 0;
  if (temp) goto label8005C9C8;
  v0 = lhu(0x800777AA);
  v1 = ~t0;
  v0 = v0 & v1;
  sh(0x800777AA, v0);
  goto label8005C9C8;
label8005C99C:
  sh(spu_registers_base + 0x18C, a2);
  sh(spu_registers_base + 0x18E, a3);
  sw(0x800730EC, lw(0x800730EC) & (~a1)); // &0x00000000
label8005C9C0:
label8005C9C8:
  return;
}

uint32_t psu_voice_status(uint32_t bitfield)
{
  a1 = -1;
  for (int i = 0; i < 24; i++) {
    if (bitfield & (1 << i)) {
      a1 = i;
      break;
    }
  }
  if (a1 == (uint32_t)-1) {
    return -1;
  }
  v1 = lw(0x800730EC) & (1 << a1);
  a0 = lhu(spu_registers_base + a1*0x10 + 0x0C);
  if (v1) {
    if (a0) return 3;
    else    return 1;
  } else {
    if (a0) return 2;
    else    return 0;
  }
}

// size: 0x00000094
void function_8005C9D0(void)
{
  v0 = psu_voice_status(a0);
}

// size: 0x00000060
void function_8005CA64(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, s0);
  s0 = a1;
  v0 = 0x0007EFF0;
  v0 = v0 < s0;
  temp = v0 == 0;
  sw(sp + 0x0014, ra);
  if (temp) goto label8005CA8C;
  s0 = 0x0007EFF0;
label8005CA8C:
  a1 = s0;
  function_8005C4D8();
  v0 = lw(0x8007358C); // &0x00000000
  temp = v0 != 0;
  v0 = s0;
  if (temp) goto label8005CAB0;
  sw(0x80073588, 0); // &0x00000001
label8005CAB0:
  ra = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x00000060
void function_8005CAC4(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, s0);
  s0 = a1;
  v0 = 0x0007EFF0;
  v0 = v0 < s0;
  temp = v0 == 0;
  sw(sp + 0x0014, ra);
  if (temp) goto label8005CAEC;
  s0 = 0x0007EFF0;
label8005CAEC:
  a1 = s0;
  function_8005C450();
  v0 = lw(0x8007358C); // &0x00000000
  temp = v0 != 0;
  v0 = s0;
  if (temp) goto label8005CB10;
  sw(0x80073588, 0); // &0x00000001
label8005CB10:
  ra = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x00000058
void function_8005CB24(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  a1 = a0;
  v0 = 0x0007EFE8;
  v1 = a1 - 4112; // 0xFFFFEFF0
  v0 = v0 < v1;
  temp = v0 != 0;
  sw(sp + 0x0010, ra);
  if (temp) goto label8005CB68;
  a0 = -1; // 0xFFFFFFFF
  function_8005C588();
  v1 = lw(0x8007357C); // &0x00000003
  sh(0x8007356C, v0); // &0x00000000
  v0 = v0 & 0xFFFF;
  v0 = v0 << v1;
  goto label8005CB6C;
label8005CB68:
  v0 = 0;
label8005CB6C:
  ra = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x00000394
void function_8005CC58(void)
{
  uint32_t temp;
  sp -= 16; // 0xFFFFFFF0
  a2 = 0;
  t1 = lw(a0 + 0x0000);
  t2 = t1 < 1;
  temp = t2 != 0;
  t0 = 0;
  if (temp) goto label8005CC88;
  v0 = t1 & 0x1;
  temp = v0 == 0;
  v0 = t1 & 0x4;
  if (temp) goto label8005CD38;
  temp = v0 == 0;
  if (temp) goto label8005CCEC;
label8005CC88:
  v1 = lh(a0 + 0x0008);
  v0 = v1 < 8;
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label8005CCEC;
  v0 = lw(0x8001157C + v0); // &0x8005CCEC
  temp = v0;
  switch (temp)
  {
  case 0x8005CCEC: // 0
    goto label8005CCEC;
    break;
  case 0x8005CCB4: // 1
    goto label8005CCB4;
    break;
  case 0x8005CCBC: // 2
    goto label8005CCBC;
    break;
  case 0x8005CCC4: // 3
    goto label8005CCC4;
    break;
  case 0x8005CCCC: // 4
    goto label8005CCCC;
    break;
  case 0x8005CCD4: // 5
    goto label8005CCD4;
    break;
  case 0x8005CCDC: // 6
    goto label8005CCDC;
    break;
  case 0x8005CCE4: // 7
    goto label8005CCE4;
    break;
  default:
    JR(temp, 0x8005CCAC);
    return;
  }
label8005CCB4:
  a1 = 0 | 0x8000;
  goto label8005CCF4;
label8005CCBC:
  a1 = 0 | 0x9000;
  goto label8005CCF4;
label8005CCC4:
  a1 = 0 | 0xA000;
  goto label8005CCF4;
label8005CCCC:
  a1 = 0 | 0xB000;
  goto label8005CCF4;
label8005CCD4:
  a1 = 0 | 0xC000;
  goto label8005CCF4;
label8005CCDC:
  a1 = 0 | 0xD000;
  goto label8005CCF4;
label8005CCE4:
  a1 = 0 | 0xE000;
  goto label8005CCF4;
label8005CCEC:
  a2 = lhu(a0 + 0x0004);
  a1 = 0;
label8005CCF4:
  temp = a1 == 0;
  v0 = a2 & 0x7FFF;
  if (temp) goto label8005CD28;
  a3 = lh(a0 + 0x0004);
  v0 = (int32_t)a3 < 128;
  temp = v0 != 0;
  v1 = a3;
  if (temp) goto label8005CD18;
  a2 = 127; // 0x007F
  goto label8005CD24;
label8005CD18:
  temp = (int32_t)a3 >= 0;
  a2 = v1;
  if (temp) goto label8005CD24;
  a2 = 0;
label8005CD24:
  v0 = a2 & 0x7FFF;
label8005CD28:
  v0 = v0 | a1;
  sh(spu_registers_base + 0x180, v0);
label8005CD38:
  if (t2 || t1 & 2) {
    if (t2 == 0 && (t1 & 8) == 0) {
      a1 = 0x0000;
      t0 = lhu(a0 + 0x06);
      goto label8005CDBC;
    }
    switch (lh(a0 + 0x0A))
    {
    case 1:
      a1 = 0x8000;
      break;
    case 2:
      a1 = 0x9000;
      break;
    case 3:
      a1 = 0xA000;
      break;
    case 4:
      a1 = 0xB000;
      break;
    case 5:
      a1 = 0xC000;
      break;
    case 6:
      a1 = 0xD000;
      break;
    case 7:
      a1 = 0xE000;
      break;
    default:
      a1 = 0x0000;
      t0 = lhu(a0 + 0x06);
      goto label8005CDBC;
      break;
    }
  label8005CDBC:
    if (a1) {
      a2 = lh(a0 + 0x0006);
      t0 = clamp_int(a2, 0, 0x7F);
    }
    sh(spu_registers_base + 0x182, (t0 & 0x7FFF) | a1);
  }
  temp = t2 != 0;
  v0 = t1 & 0x40;
  if (temp) goto label8005CE10;
  temp = v0 == 0;
  if (temp) goto label8005CE24;
label8005CE10:
  v0 = lhu(a0 + 0x10);
  sh(spu_registers_base + 0x1B0, v0);
label8005CE24:
  temp = t2 != 0;
  v0 = t1 & 0x80;
  if (temp) goto label8005CE34;
  temp = v0 == 0;
  if (temp) goto label8005CE48;
label8005CE34:
  v0 = lhu(a0 + 0x0012);
  sh(spu_registers_base + 0x1B2, v0);
label8005CE48:
  temp = t2 != 0;
  v0 = t1 & 0x400;
  if (temp) goto label8005CE58;
  temp = v0 == 0;
  if (temp) goto label8005CE6C;
label8005CE58:
  v0 = lhu(a0 + 0x001C);
  sh(spu_registers_base + 0x1B4, v0);
label8005CE6C:
  temp = t2 != 0;
  v0 = t1 & 0x800;
  if (temp) goto label8005CE7C;
  temp = v0 == 0;
  if (temp) goto label8005CE90;
label8005CE7C:
  v0 = lhu(a0 + 0x001E);
  sh(spu_registers_base + 0x1B6, v0);
label8005CE90:
  temp = t2 != 0;
  v0 = t1 & 0x100;
  if (temp) goto label8005CEA0;
  temp = v0 == 0;
  if (temp) goto label8005CEE4;
label8005CEA0:
  v0 = lw(a0 + 0x0014);
  temp = v0 != 0;
  if (temp) goto label8005CEC8;
  sh(spu_registers_base + 0x1AA, lhu(v0 + 0x1AA) & (~4));
  goto label8005CEE0;
label8005CEC8:
  sh(spu_registers_base + 0x1AA, lhu(spu_registers_base + 0x1AA) | 4);
label8005CEE0:
label8005CEE4:
  temp = t2 != 0;
  v0 = t1 & 0x200;
  if (temp) goto label8005CEF4;
  temp = v0 == 0;
  if (temp) goto label8005CF38;
label8005CEF4:
  v0 = lw(a0 + 0x0018);
  temp = v0 != 0;
  if (temp) goto label8005CF1C;
  sh(spu_registers_base + 0x1AA, lhu(spu_registers_base + 0x1AA) & (~1));
  goto label8005CF34;
label8005CF1C:
  sh(spu_registers_base + 0x1AA, lhu(spu_registers_base + 0x1AA) | 1);
label8005CF34:
label8005CF38:
  temp = t2 != 0;
  v0 = t1 & 0x1000;
  if (temp) goto label8005CF48;
  temp = v0 == 0;
  if (temp) goto label8005CF8C;
label8005CF48:
  v0 = lw(a0 + 0x0020);
  temp = v0 != 0;
  if (temp) goto label8005CF70;
  sh(spu_registers_base + 0x1AA, lhu(spu_registers_base + 0x1AA) & (~8));
  goto label8005CF88;
label8005CF70:
  sh(spu_registers_base + 0x1AA, lhu(spu_registers_base + 0x1AA) | 8);
label8005CF88:
label8005CF8C:
  temp = t2 != 0;
  v0 = t1 & 0x2000;
  if (temp) goto label8005CF9C;
  temp = v0 == 0;
  if (temp) goto label8005CFE0;
label8005CF9C:
  v0 = lw(a0 + 0x0024);
  temp = v0 != 0;
  if (temp) goto label8005CFC4;
  sh(spu_registers_base + 0x1AA, lhu(spu_registers_base + 0x1AA) & (~2));
  goto label8005CFDC;
label8005CFC4:
  sh(spu_registers_base + 0x1AA, lhu(spu_registers_base + 0x1AA) | 2);
label8005CFDC:
label8005CFE0:
  sp += 16; // 0x0010
  return;
}

// size: 0x0000013C
void function_8005D6B8(void)
{
  uint32_t temp;
  sp -= 16; // 0xFFFFFFF0
  a0 = a0 & 0xFFFF;
  a0 = a0 << 7;
  a1 = a1 & 0xFFFF;
  a0 += a1;
  a2 = a2 & 0xFFFF;
  a2 = a2 << 7;
  a3 = a3 & 0xFFFF;
  a2 += a3;
  a2 -= a0;
  temp = (int32_t)a2 >= 0;
  a1 = a2;
  if (temp) goto label8005D6EC;
  a1 = -a2;
label8005D6EC:
  v0 = 0x2AAAAAAB;
  mult(a1, v0);
  v0 = (int32_t)a1 >> 31;
  t1=hi;
  v1 = (int32_t)t1 >> 8;
  v1 -= v0;
  a0 = v1;
  v0 = a0 << 1;
  v0 += a0;
  v0 = v0 << 9;
  temp = (int32_t)a2 < 0;
  v1 = a1 - v0;
  if (temp) goto label8005D72C;
  v0 = 4096; // 0x1000
  v0 = v0 << a0;
  goto label8005D744;
label8005D72C:
  temp = v1 == 0;
  v0 = 1536; // 0x0600
  if (temp) goto label8005D73C;
  a0++;
  v1 = v0 - v1;
label8005D73C:
  v0 = 4096; // 0x1000
  v0 = (int32_t)v0 >> a0;
label8005D744:
  a2 = v0 & 0xFFFF;
  a0 = 4155; // 0x103B
  mult(a2, a0);
  a3 = a2 << 12;
  a1 = 0;
  temp = (int32_t)v1 >= 0;
  v0 = v1;
  if (temp) goto label8005D764;
  v0 = -v0;
label8005D764:
  v1 = v0 >> 5;
  t0 = v0 & 0x1F;
  t1=lo;
  temp = v1 == 0;
  sw(sp + 0x0008, t1);
  if (temp) goto label8005D7B4;
  mult(a2, a0);
label8005D77C:
  v0 = a0 << 6;
  v0 += a0;
  v0 = v0 << 4;
  v0 -= a0;
  v0 = v0 << 2;
  a3=lo;
  a0 = v0 - a0;
  a0 = a0 >> 12;
  mult(a2, a0);
  a1++;
  v0 = (int32_t)a1 < (int32_t)v1;
  t1=lo;
  temp = v0 != 0;
  sw(sp + 0x0008, t1);
  if (temp) goto label8005D77C;
label8005D7B4:
  t1 = lw(sp + 0x0008);
  v0 = t1 - a3;
  v0 = v0 >> 5;
  mult(v0, t0);
  t1=lo;
  v0 = a3 + t1;
  v1 = v0 >> 12;
  v0 = v1 < 0x4000;
  temp = v0 != 0;
  v0 = v1 & 0xFFFF;
  if (temp) goto label8005D7E8;
  v1 = 16383; // 0x3FFF
  v0 = v1 & 0xFFFF;
label8005D7E8:
  sp += 16; // 0x0010
  return;
}

// size: 0x0000063C
void function_8005CFEC(void)
{
  uint32_t temp;
  sp -= 80; // 0xFFFFFFB0
  sw(sp + 0x0028, s0);
  s0 = a0;
  sw(sp + 0x0044, s7);
  s7 = a2;
  sw(sp + 0x0040, s6);
  s6 = 0;
  sw(sp + 0x003C, s5);
  s5 = 0;
  v0 = a1 ^ s7;
  sw(sp + 0x004C, ra);
  sw(sp + 0x0048, fp);
  sw(sp + 0x0038, s4);
  sw(sp + 0x0034, s3);
  sw(sp + 0x0030, s2);
  sw(sp + 0x002C, s1);
  sw(sp + 0x0010, a3);
  s1 = lw(s0 + 0x0004);
  fp = v0 < 1;
  temp = (int32_t)a1 >= 0;
  s2 = s1 < 1;
  if (temp) goto label8005D044;
  a1 = 0;
label8005D044:
  v0 = (int32_t)a1 < 24;
  temp = v0 == 0;
  v0 = (int32_t)s7 < 24;
  if (temp) goto label8005D06C;
  temp = v0 != 0;
  if (temp) goto label8005D05C;
  s7 = 23; // 0x0017
label8005D05C:
  temp = (int32_t)s7 < 0;
  v0 = (int32_t)s7 < (int32_t)a1;
  if (temp) goto label8005D06C;
  temp = v0 == 0;
  s7++;
  if (temp) goto label8005D074;
label8005D06C:
  v0 = -3; // 0xFFFFFFFD
  goto label8005D5F4;
label8005D074:
  s4 = a1;
  v0 = (int32_t)s4 < (int32_t)s7;
  temp = v0 == 0;
  if (temp) goto label8005D5D8;
label8005D084:
  temp = fp != 0;
  v0 = 1; // 0x0001
  if (temp) goto label8005D0A0;
  v1 = lw(s0 + 0x0000);
  v0 = v0 << s4;
  v1 = v1 & v0;
  temp = v1 == 0;
  if (temp) goto label8005D5C8;
label8005D0A0:
  temp = s2 != 0;
  s3 = s4 << 3;
  if (temp) goto label8005D0B4;
  v0 = s1 & 0x10;
  temp = v0 == 0;
  if (temp) goto label8005D0CC;
label8005D0B4:
  v0 = s4 << 4;
  v1 = lw(spu_registers_base_ptr);
  a0 = lhu(s0 + 0x0014);
  v0 += v1;
  sh(v0 + 0x0004, a0);
label8005D0CC:
  temp = s2 != 0;
  v0 = s1 & 0x40;
  if (temp) goto label8005D0DC;
  temp = v0 == 0;
  if (temp) goto label8005D0F0;
label8005D0DC:
  v1 = lhu(s0 + 0x0018);
  v0 = s4 << 1;
  sh(0x8007311C + v0, v1); // &0xC000C000
label8005D0F0:
  temp = s2 != 0;
  v0 = s4 << 1;
  if (temp) goto label8005D104;
  v0 = s1 & 0x20;
  temp = v0 == 0;
  v0 = s4 << 1;
  if (temp) goto label8005D13C;
label8005D104:
  a1 = lhu(0x8007311C + v0); // &0xC000C000
  a3 = lhu(s0 + 0x0016);
  a0 = a1 >> 8;
  a1 = a1 & 0xFF;
  a2 = a3 >> 8;
  a3 = a3 & 0xFF;
  function_8005D6B8();
  a0 = lw(spu_registers_base_ptr);
  v1 = s3 << 1;
  v1 += a0;
  sh(v1 + 0x0004, v0);
label8005D13C:
  temp = s2 != 0;
  v0 = s3 << 1;
  if (temp) goto label8005D154;
  v0 = 0x00020000;
  v0 = s1 & v0;
  temp = v0 == 0;
  v0 = s3 << 1;
  if (temp) goto label8005D168;
label8005D154:
  v1 = lw(spu_registers_base_ptr);
  a0 = lhu(s0 + 0x003A);
  v0 += v1;
  sh(v0 + 0x0008, a0);
label8005D168:
  temp = s2 != 0;
  v0 = s3 << 1;
  if (temp) goto label8005D180;
  v0 = 0x00040000;
  v0 = s1 & v0;
  temp = v0 == 0;
  v0 = s3 << 1;
  if (temp) goto label8005D194;
label8005D180:
  v1 = lw(spu_registers_base_ptr);
  a0 = lhu(s0 + 0x003C);
  v0 += v1;
  sh(v0 + 0x000A, a0);
label8005D194:
  temp = s2 != 0;
  v0 = s1 & 0x1;
  if (temp) goto label8005D1AC;
  temp = v0 == 0;
  v0 = s1 & 0x4;
  if (temp) goto label8005D264;
  temp = v0 == 0;
  if (temp) goto label8005D210;
label8005D1AC:
  v1 = lh(s0 + 0x000C);
  v0 = v1 < 8;
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label8005D210;
  v0 = lw(0x800115BC + v0); // &0x8005D210
  temp = v0;
  switch (temp)
  {
  case 0x8005D210: // 0
    goto label8005D210;
    break;
  case 0x8005D1D8: // 1
    goto label8005D1D8;
    break;
  case 0x8005D1E0: // 2
    goto label8005D1E0;
    break;
  case 0x8005D1E8: // 3
    goto label8005D1E8;
    break;
  case 0x8005D1F0: // 4
    goto label8005D1F0;
    break;
  case 0x8005D1F8: // 5
    goto label8005D1F8;
    break;
  case 0x8005D200: // 6
    goto label8005D200;
    break;
  case 0x8005D208: // 7
    goto label8005D208;
    break;
  default:
    JR(temp, 0x8005D1D0);
    return;
  }
label8005D1D8:
  a1 = 0 | 0x8000;
  goto label8005D218;
label8005D1E0:
  a1 = 0 | 0x9000;
  goto label8005D218;
label8005D1E8:
  a1 = 0 | 0xA000;
  goto label8005D218;
label8005D1F0:
  a1 = 0 | 0xB000;
  goto label8005D218;
label8005D1F8:
  a1 = 0 | 0xC000;
  goto label8005D218;
label8005D200:
  a1 = 0 | 0xD000;
  goto label8005D218;
label8005D208:
  a1 = 0 | 0xE000;
  goto label8005D218;
label8005D210:
  s6 = lhu(s0 + 0x0008);
  a1 = 0;
label8005D218:
  temp = a1 == 0;
  v0 = s6 & 0x7FFF;
  if (temp) goto label8005D24C;
  a0 = lh(s0 + 0x0008);
  v0 = (int32_t)a0 < 128;
  temp = v0 != 0;
  v1 = a0;
  if (temp) goto label8005D23C;
  s6 = 127; // 0x007F
  goto label8005D248;
label8005D23C:
  temp = (int32_t)a0 >= 0;
  s6 = v1;
  if (temp) goto label8005D248;
  s6 = 0;
label8005D248:
  v0 = s6 & 0x7FFF;
label8005D24C:
  v1 = s3 << 1;
  a0 = lw(spu_registers_base_ptr);
  v0 = v0 | a1;
  v1 += a0;
  sh(v1 + 0x0000, v0);
label8005D264:
  temp = s2 != 0;
  v0 = s1 & 0x2;
  if (temp) goto label8005D27C;
  temp = v0 == 0;
  v0 = s1 & 0x8;
  if (temp) goto label8005D334;
  temp = v0 == 0;
  if (temp) goto label8005D2E0;
label8005D27C:
  v1 = lh(s0 + 0x000E);
  v0 = v1 < 8;
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label8005D2E0;
  v0 = lw(0x800115DC + v0); // &0x8005D2E0
  temp = v0;
  switch (temp)
  {
  case 0x8005D2E0: // 0
    goto label8005D2E0;
    break;
  case 0x8005D2A8: // 1
    goto label8005D2A8;
    break;
  case 0x8005D2B0: // 2
    goto label8005D2B0;
    break;
  case 0x8005D2B8: // 3
    goto label8005D2B8;
    break;
  case 0x8005D2C0: // 4
    goto label8005D2C0;
    break;
  case 0x8005D2C8: // 5
    goto label8005D2C8;
    break;
  case 0x8005D2D0: // 6
    goto label8005D2D0;
    break;
  case 0x8005D2D8: // 7
    goto label8005D2D8;
    break;
  default:
    JR(temp, 0x8005D2A0);
    return;
  }
label8005D2A8:
  a1 = 0 | 0x8000;
  goto label8005D2E8;
label8005D2B0:
  a1 = 0 | 0x9000;
  goto label8005D2E8;
label8005D2B8:
  a1 = 0 | 0xA000;
  goto label8005D2E8;
label8005D2C0:
  a1 = 0 | 0xB000;
  goto label8005D2E8;
label8005D2C8:
  a1 = 0 | 0xC000;
  goto label8005D2E8;
label8005D2D0:
  a1 = 0 | 0xD000;
  goto label8005D2E8;
label8005D2D8:
  a1 = 0 | 0xE000;
  goto label8005D2E8;
label8005D2E0:
  s5 = lhu(s0 + 0x000A);
  a1 = 0;
label8005D2E8:
  temp = a1 == 0;
  v0 = s5 & 0x7FFF;
  if (temp) goto label8005D31C;
  a0 = lh(s0 + 0x000A);
  v0 = (int32_t)a0 < 128;
  temp = v0 != 0;
  v1 = a0;
  if (temp) goto label8005D30C;
  s5 = 127; // 0x007F
  goto label8005D318;
label8005D30C:
  temp = (int32_t)a0 >= 0;
  s5 = v1;
  if (temp) goto label8005D318;
  s5 = 0;
label8005D318:
  v0 = s5 & 0x7FFF;
label8005D31C:
  v1 = s3 << 1;
  a0 = lw(spu_registers_base_ptr);
  v0 = v0 | a1;
  v1 += a0;
  sh(v1 + 0x0002, v0);
label8005D334:
  temp = s2 != 0;
  v0 = 0x00010000;
  if (temp) goto label8005D348;
  v0 = s1 & v0;
  temp = v0 == 0;
  if (temp) goto label8005D354;
label8005D348:
  a1 = lw(s0 + 0x0020);
  a0 = s3 | 0x7;
  function_8005C588();
label8005D354:
  temp = s2 != 0;
  v0 = s1 & 0x80;
  if (temp) goto label8005D364;
  temp = v0 == 0;
  if (temp) goto label8005D370;
label8005D364:
  a1 = lw(s0 + 0x001C);
  a0 = s3 | 0x3;
  function_8005C588();
label8005D370:
  temp = s2 != 0;
  v0 = s1 & 0x800;
  if (temp) goto label8005D388;
  temp = v0 == 0;
  v0 = s1 & 0x100;
  if (temp) goto label8005D3F4;
  temp = v0 == 0;
  a2 = 0;
  if (temp) goto label8005D3AC;
label8005D388:
  v1 = lw(s0 + 0x0024);
  v0 = 1; // 0x0001
  temp = v1 == v0;
  v0 = 5; // 0x0005
  if (temp) goto label8005D3A8;
  temp = v1 != v0;
  a2 = 0;
  if (temp) goto label8005D3AC;
  a2 = 128; // 0x0080
  goto label8005D3AC;
label8005D3A8:
  a2 = 0;
label8005D3AC:
  v0 = lhu(s0 + 0x0030);
  a1 = v0;
  v0 = a1 < 128;
  temp = v0 != 0;
  if (temp) goto label8005D3C8;
  a1 = 127; // 0x007F
label8005D3C8:
  v0 = lw(spu_registers_base_ptr);
  v1 = s3 << 1;
  v1 += v0;
  v0 = lhu(v1 + 0x0008);
  a0 = v0 & 0xFF;
  v0 = a1 | a2;
  v0 = v0 << 8;
  v0 = a0 | v0;
  sh(v1 + 0x0008, v0);
label8005D3F4:
  temp = s2 != 0;
  v0 = s1 & 0x1000;
  if (temp) goto label8005D404;
  temp = v0 == 0;
  if (temp) goto label8005D448;
label8005D404:
  v0 = lhu(s0 + 0x0032);
  a1 = v0;
  v0 = a1 < 16;
  temp = v0 != 0;
  if (temp) goto label8005D420;
  a1 = 15; // 0x000F
label8005D420:
  v0 = lw(spu_registers_base_ptr);
  v1 = s3 << 1;
  v1 += v0;
  v0 = lhu(v1 + 0x0008);
  a0 = v0 & 0xFF0F;
  v0 = a1 << 4;
  v0 = a0 | v0;
  sh(v1 + 0x0008, v0);
label8005D448:
  temp = s2 != 0;
  v0 = s1 & 0x8000;
  if (temp) goto label8005D458;
  temp = v0 == 0;
  if (temp) goto label8005D498;
label8005D458:
  v0 = lhu(s0 + 0x0038);
  a1 = v0;
  v0 = a1 < 16;
  temp = v0 != 0;
  if (temp) goto label8005D474;
  a1 = 15; // 0x000F
label8005D474:
  v0 = lw(spu_registers_base_ptr);
  v1 = s3 << 1;
  v1 += v0;
  v0 = lhu(v1 + 0x0008);
  v0 = v0 & 0xFFF0;
  v0 = a1 | v0;
  sh(v1 + 0x0008, v0);
label8005D498:
  temp = s2 != 0;
  v0 = s1 & 0x2000;
  if (temp) goto label8005D4B0;
  temp = v0 == 0;
  v0 = s1 & 0x200;
  if (temp) goto label8005D548;
  temp = v0 == 0;
  a2 = 0;
  if (temp) goto label8005D500;
label8005D4B0:
  v1 = lw(s0 + 0x0028);
  v0 = 3; // 0x0003
  temp = v1 == v0;
  v0 = (int32_t)v1 < 4;
  if (temp) goto label8005D4FC;
  temp = v0 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label8005D4D8;
  temp = v1 == v0;
  a2 = 0;
  if (temp) goto label8005D500;
  a2 = 256; // 0x0100
  goto label8005D500;
label8005D4D8:
  v0 = 5; // 0x0005
  temp = v1 == v0;
  v0 = 7; // 0x0007
  if (temp) goto label8005D4F4;
  temp = v1 == v0;
  a2 = 768; // 0x0300
  if (temp) goto label8005D500;
  a2 = 256; // 0x0100
  goto label8005D500;
label8005D4F4:
  a2 = 512; // 0x0200
  goto label8005D500;
label8005D4FC:
  a2 = 256; // 0x0100
label8005D500:
  v0 = lhu(s0 + 0x0034);
  a1 = v0;
  v0 = a1 < 128;
  temp = v0 != 0;
  if (temp) goto label8005D51C;
  a1 = 127; // 0x007F
label8005D51C:
  v0 = lw(spu_registers_base_ptr);
  v1 = s3 << 1;
  v1 += v0;
  v0 = lhu(v1 + 0x000A);
  a0 = v0 & 0x3F;
  v0 = a1 | a2;
  v0 = v0 << 6;
  v0 = a0 | v0;
  sh(v1 + 0x000A, v0);
label8005D548:
  temp = s2 != 0;
  v0 = s1 & 0x4000;
  if (temp) goto label8005D560;
  temp = v0 == 0;
  v0 = s1 & 0x400;
  if (temp) goto label8005D5C8;
  temp = v0 == 0;
  a2 = 0;
  if (temp) goto label8005D584;
label8005D560:
  v1 = lw(s0 + 0x002C);
  v0 = 3; // 0x0003
  temp = v1 == v0;
  v0 = 7; // 0x0007
  if (temp) goto label8005D580;
  temp = v1 != v0;
  a2 = 0;
  if (temp) goto label8005D584;
  a2 = 32; // 0x0020
  goto label8005D584;
label8005D580:
  a2 = 0;
label8005D584:
  v0 = lhu(s0 + 0x0036);
  a1 = v0;
  v0 = a1 < 32;
  temp = v0 != 0;
  if (temp) goto label8005D5A0;
  a1 = 31; // 0x001F
label8005D5A0:
  v0 = lw(spu_registers_base_ptr);
  v1 = s3 << 1;
  v1 += v0;
  v0 = lhu(v1 + 0x000A);
  a0 = v0 & 0xFFC0;
  v0 = a1 | a2;
  v0 = a0 | v0;
  sh(v1 + 0x000A, v0);
label8005D5C8:
  s4++;
  v0 = (int32_t)s4 < (int32_t)s7;
  temp = v0 != 0;
  if (temp) goto label8005D084;
label8005D5D8:
  t0 = lw(sp + 0x0010);
  temp = t0 != 0;
  v0 = 0;
  if (temp) goto label8005D5F4;
  do_literally_nothing();
  v0 = 0;
label8005D5F4:
  ra = lw(sp + 0x004C);
  fp = lw(sp + 0x0048);
  s7 = lw(sp + 0x0044);
  s6 = lw(sp + 0x0040);
  s5 = lw(sp + 0x003C);
  s4 = lw(sp + 0x0038);
  s3 = lw(sp + 0x0034);
  s2 = lw(sp + 0x0030);
  s1 = lw(sp + 0x002C);
  s0 = lw(sp + 0x0028);
  sp += 80; // 0x0050
  return;
}

// size: 0x00000028
void function_8005C7AC(void)
{
  a1 = 0;
  a2 = 0x17;
  a3 = 0;
  function_8005CFEC();
}

// size: 0x00000088
void function_8005D9EC(void)
{
  uint32_t temp;
  t2 = 24; // 0x0018
  a2 = 0;
  a3 = 1; // 0x0001
  t1 = 3; // 0x0003
  t0 = 2; // 0x0002
  a1 = a0;
label8005DA04:
  a0 = a2 << 4;
  v0 = lw(spu_registers_base_ptr);
  v1 = lw(0x800730EC); // &0x00000000
  a0 += v0;
  v0 = a3 << a2;
  v1 = v1 & v0;
  v0 = lhu(a0 + 0x000C);
  temp = v1 == 0;
  if (temp) goto label8005DA48;
  temp = v0 == 0;
  if (temp) goto label8005DA40;
  sb(a1 + 0x0000, a3);
  goto label8005DA5C;
label8005DA40:
  sb(a1 + 0x0000, t1);
  goto label8005DA5C;
label8005DA48:
  temp = v0 == 0;
  if (temp) goto label8005DA58;
  sb(a1 + 0x0000, t0);
  goto label8005DA5C;
label8005DA58:
  sb(a1 + 0x0000, 0);
label8005DA5C:
  a2++;
  v0 = (int32_t)a2 < (int32_t)t2;
  temp = v0 != 0;
  a1++;
  if (temp) goto label8005DA04;
  return;
}

// size: 0x00000040
void function_8005DA74(void)
{
  sp -= 24; // 0xFFFFFFE8
  a1 = a1 & 0x7FFF;
  a2 = a2 & 0x7FFF;
  v0 = lw(spu_registers_base_ptr);
  a0 = a0 << 4;
  sw(sp + 0x0010, ra);
  a0 += v0;
  sh(a0 + 0x0000, a1);
  sh(a0 + 0x0002, a2);
  do_literally_nothing();
  ra = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x00000060
void function_8005DAB4(void)
{
  uint32_t temp;
  v1 = lhu(spu_registers_base + a0*0x10 + 0x0000);
  a0 = lhu(spu_registers_base + a0*0x10 + 0x0002);
  a3 = v1 & 0xFFFF;
  v0 = a3 < 0x4000;
  temp = v0 != 0;
  v0 = 0 | 0x8000;
  if (temp) goto label8005DAE8;
  v0 = a3 - v0;
  sh(a1 + 0x0000, v0);
  goto label8005DAEC;
label8005DAE8:
  sh(a1 + 0x0000, v1);
label8005DAEC:
  v1 = a0 & 0xFFFF;
  v0 = v1 < 0x4000;
  temp = v0 != 0;
  v0 = 0 | 0x8000;
  if (temp) goto label8005DB08;
  v0 = v1 - v0;
  sh(a2 + 0x0000, v0);
  goto label8005DB0C;
label8005DB08:
  sh(a2 + 0x0000, a0);
label8005DB0C:
  return;
}

// size: 0x00000478
void function_8005637C(void)
{
  uint32_t temp;
  sp -= 0x98; // 0xFFFFFF68
  sw(sp + 0x84, s5);
  sw(sp + 0x7C, s3);
  sw(sp + 0x90, fp);
  sw(sp + 0x80, s4);
  sw(sp + 0x88, s6);
  sw(sp + 0x94, ra);
  sw(sp + 0x78, s2);
  sw(sp + 0x70, s0);

  s5 = 0;
  fp = 0x80075F30 + 0x04;
  s4 = fp + 0x04;
  s6 = fp + 0x10;
  v0 = lhu(0x80076E20) >> 4;
  sb(sp + 0x68, v0);
  for (int i = 0; i < 24; i++) {
    v1 = lhu(0x80075F30 + i*0x1C + 0x0E);
    if (v1 & 0x40) {
      sh(0x80075F30 + i*0x1C + 0x0E, 0);
      continue;
    }
    s0 = 1 << i;
    if ((v1 & 1) == 0) continue;
    a0 = s0;
    function_8005C9D0();
    a0 = lhu(0x80075F30 + i*0x1C + 0x0E);
    v1 = v0;
    v0 = a0 & 0x2;
    temp = v0 == 0;
    v0 = v1 & 0xFF;
    if (temp) goto label80056458;
    temp = v0 != 3;
    if (temp) goto label8005647C;
    v1 = lw(0x80075F30 + i*0x1C + 0x18);
    s5 = s5 | s0;
    goto label800565B4;
  label80056458:
    if (v0 == 1 || v0 == 3)
      sh(0x80075F30 + i*0x1C + 0x0E, a0 | 0x02);
  label8005647C:
      
    v1 = lhu(0x80075F30 + i*0x1C + 0x0E);
    v1 = v1 & 0x1C;
    if ((v1 & 1) == 0) continue;

    if (v1 == 0x08) goto label800564D8;
    if ((int32_t)v1 > 0x08) {
      if (v1 == 0x10) goto label800566E8;
      goto label80056710;
    }
    v0 = 0x2000;
    if (v1 == 0x04) {
      sh(sp + 0x22, 0x2000);
      goto label80056708;
    }
    goto label80056710;
  label800564D8:
    a0 = i;
    a1 = i*0x1C + fp;
    a2 = a1 + 2;
    function_8005DAB4();
    a1 = lw(0x80075F30 + i*0x1C + 0x00);
    s2 = lbu(a1 + 0x55) << 10;
    s0 = sp + 88; // 0x0058
    if (s2 == 0)
      s2 = 0x4000;
      
    spyro_vec3_sub(s0, a1 + 0x0C, 0x80076DF8);
    a0 = s0;
    a1 = 1; // 0x0001
    v0 = spyro_vec_length(a0, a1);
    s0 = v0;
    v0 = (int32_t)s0 < (int32_t)s2;
    temp = v0 != 0;
    a2 = 0;
    if (temp) goto label80056684;
    a0 = fp - 44; // 0xFFFFFFD4
    v0 = lhu(0x80075F30 + i*0x1C + 0x06);
    a0 += s1;
    v0 = v0 << 16;
    v0 = (int32_t)v0 >> 17;
    sh(a0 + 0x002E, v0);
    sh(sp + 0x0022, v0);
    v0 = lhu(0x80075F30 + i*0x1C + 0x04);
    v0 = v0 << 16;
    a1 = (int32_t)v0 >> 17;
    v1 = a1;
    sh(a0 + 0x002C, v1);
    v0 = lh(sp + 0x0022);
    v0 = (int32_t)v0 < 64;
    temp = v0 == 0;
    sh(sp + 0x0020, v1);
    if (temp) goto label80056600;
    v0 = (int32_t)a1 < 64;
    temp = v0 == 0;
    v0 = 1 << i;
    if (temp) goto label80056600;
    v1 = lw(0x80075F30 + i*0x1C + 0x18);
    s5 = s5 | v0;
  label800565B4:
    if (v1)
      sb(v1, 0x7F);
      
    sh(0x80075F30 + i*0x1C + 0x0E, 0x40);
    sw(0x80075F30 + i*0x1C + 0x18, 0);
    sw(0x80075F30 + i*0x1C + 0x00, 0);
    sb(0x80075F30 + i*0x1C + 0x0D, 0xFF);
    continue;
  label80056600:
    a0 = 0x8007622C;
    v1 = lh(0x80075F30 + i*0x1C + 0x04);
    v0 = lw(a0);
    mult(v1, v0);
    v1 = lh(0x80075F30 + i*0x1C + 0x06);
    t0=lo;
    v0 = (int32_t)t0 >> 12;
    sh(0x80075F30 + i*0x1C + 0x04, v0);
    v0 = lw(a0);
    mult(v1, v0);
    a0 = i;
    a1 = lh(0x80075F30 + i*0x1C + 0x04);
    t0=lo;
    v0 = (int32_t)t0 >> 12;
    a2 = v0 << 16;
    sh(0x80075F30 + i*0x1C + 0x06, v0);
    a2 = (int32_t)a2 >> 16;
    function_8005DA74();
    continue;
  label80056684:
    v0 = lw(0x80075F30 + i*0x1C + 0x00);
    a0 = lw(0x80076DF8);
    a1 = lw(0x80076DFC);
    v1 = lw(v0 + 0x0C);
    v0 = lw(v0 + 0x10);
    a0 = v1 - a0;
    a1 = v0 - a1;
    v0 = spyro_atan2(a0, a1, a2);
    a0 = sp + 0x20;
    a1 = s0;
    a3 = s2;
    v0 -= lbu(sp + 0x68);
    a2 = v0 & 0xFF;
    v0 = 0x80075F30 + i*0x1C + 0x10;
    sw(sp + 0x10, v0);
    function_80056C84();
    goto label80056710;
  label800566E8:
    v0 = lhu(0x80075F30 + i*0x1C + 0x06);
    sh(sp + 0x22, v0);
    v0 = lhu(0x80075F30 + i*0x1C + 0x04);
  label80056708:
    sh(sp + 0x20, v0);
  label80056710:
    v0 = lh(sp + 0x20);
    v1 = lw(0x8007622C);
    mult(v0, v1);
    t1=lo;
    v0 = lh(sp + 0x22);
    mult(v0, v1);
    sw(sp + 0x1C, 3);
    v0 = 1 << i;
    sw(sp + 0x18, v0);
    v0 = (int32_t)t1 >> 12;
    sh(sp + 0x20, v0);
    v1=lo;
    v0 = (int32_t)v1 >> 12;
    sh(sp + 0x22, v0);
    v0 = lw(s4 + i*0x1C);
    v1 = lw(s6 + i*0x1C);
    v0 += v1;
    sw(s4 + i*0x1C, v0);
    sw(s6 + i*0x1C, 0);
    v0 = lw(sp + 0x001C);
    v0 = v0 | 0x10;
    sw(sp + 0x1C, v0);
    v0 = lw(s4 + i*0x1C);
    a0 = sp + 0x18;
    sh(sp + 0x2C, v0);
    function_8005C7AC();
  }
  a0 = 0x8007623C;
  v1 = lw(a0) | s5;
  sw(a0, v1);
  v0 = 0;
  ra = lw(sp + 0x94);
  fp = lw(sp + 0x90);
  s6 = lw(sp + 0x88);
  s5 = lw(sp + 0x84);
  s4 = lw(sp + 0x80);
  s2 = lw(sp + 0x78);
  s1 = lw(sp + 0x74);
  s0 = lw(sp + 0x70);
  sp += 152; // 0x0098
  return;
}