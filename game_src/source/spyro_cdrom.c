#include <stdint.h>
#include <stdbool.h>

#include "debug.h"
#include "psx_mem.h"
#include "psx_ops.h"
#include "psx_bios.h"
#include "main.h"
#include "spyro_vsync.h"
#include "spyro_string.h"
#include "decompilation.h"
#include "not_renamed.h"
#include "spyro_print.h"
#include "spyro_constants.h"
#include "spyro_cdrom.h"
#include "temporary.h"

int CdSync(int mode, uint8_t *result)
{
  //if (result) UNREACHABLE;

  return CdlComplete;
}

// size: 0x00000020
void function_80063BD8(void)
{
  v0 = CdSync(a0, addr_to_pointer(a1));
  UNREACHABLE;
}

void function_800653B4(void)
{
  v0 = 0;
  return;
}

// size: 0x00000138
void function_80063C48(void)
{
  return;
}

// size: 0x0000012C
void function_80063D80(void)
{
  return;
}

// size: 0x00000144
void function_80063EAC(void)
{
  return;
}

// size: 0x00000020
void function_80063FF0(void)
{
  v0 = 1;
  return;
}

void read_disk(uint32_t sector, uint32_t dst, uint32_t len, uint32_t offset, uint32_t num)
{
  if (len & 0x7FF || offset & 0x7FF || num != 0x258) {
    printf("len: %X offset: %X num: %X\n", len, offset, num);
    UNREACHABLE;
  }

  sector += offset >> 11;
  uint32_t sector_len = len >> 11;
  psx_read_sectors(dst, sector, sector_len);
}

void read_disk1(uint32_t sector, uint32_t dst, uint32_t len, uint32_t offset, uint32_t num)
{
  read_disk(sector, dst, len, offset, num);
}

void read_disk2(uint32_t sector, uint32_t dst, uint32_t len, uint32_t offset, uint32_t num)
{
  read_disk(sector, dst, len, offset, num);
}

void function_80016500(void)
{
  UNREACHABLE;
  read_disk1(a0, a1, a2, a3, lw(sp+0x10));
}

void function_80016698(void)
{
  UNREACHABLE;
  read_disk2(a0, a1, a2, a3, lw(sp+0x10));
}

// size: 0x00000010
void function_80063A8C(void)
{
  v0 = lbu(cd_result1);
}

// size: 0x00000010
void function_80063A9C(void)
{
  v0 = lbu(0x80074E54);
}

// size: 0x00000010
void function_80063AAC(void)
{
  v0 = lbu(cd_current_command);
}

// size: 0x00000010
void function_80063ABC(void)
{
  v0 = 0x80074E50;
}

// size: 0x00000020
void function_80064010(void)
{
  function_8006570C();
  v0 = v0 < 1;
}

// size: 0x00000020
void function_80063B38(void)
{
  function_80065190();
}

// size: 0x00000018
void function_80063C18(void)
{
  v0 = lw(0x80074E34);
  sw(0x80074E34, a0);
}

// size: 0x00000018
void function_80063C30(void)
{
  v0 = lw(0x80074E38);
  sw(0x80074E38, a0);
}

// size: 0x00000020
void function_80064030(void)
{
  function_8006580C();
  v0 = v0 < 1;
}

// size: 0x00000020
void function_80064074(void)
{
  function_800655A0();
}

// size: 0x00000018
void function_8006623C(void)
{
  v0 = lw(0x80075144);
  sw(0x80075144, a0);
}

// size: 0x00000050
void function_8002BA68(void)
{
  SetDispMask(0);
  CdInit();
  a0 = 0;
  function_8006623C();
  a0 = 0;
  function_8005DE58();
  function_80069080();
  ResetGraph(3);
  function_8005DEBC();
}

// size: 0x00000068
void function_8002BAB8(void)
{
  printf("Shutting Down\n");
  function_8002BA68();
  _96_remove();
  _96_init();
  printf("Launching Crash Demo\n");
  LoadExec("cdrom:\\S0\\CRASH.EXE;1", 0x801FFF00, 0);
}

uint8_t to_hexnum(uint8_t num)
{
  return ((num/10) << 4) + (num % 10);
}

uint8_t from_hexnum(uint8_t num)
{
  return ((num >> 4)*10) + (num % 10);
}

// size: 0x00000104
void write_cdrom_header(uint32_t sector_num, char header[])
{
  sector_num += 150;

  const uint32_t sector = sector_num % 75;
  const uint32_t second = (sector_num / 75) % 60;
  const uint32_t minute = (sector_num / 75) / 60;

  header[0] = to_hexnum(minute);
  header[1] = to_hexnum(second);
  header[2] = to_hexnum(sector);
}

// size: 0x00000104
void function_80064094(void)
{
  UNREACHABLE;
  write_cdrom_header(a0, addr_to_pointer(a1));
}

// size: 0x00000080
uint32_t parse_cdrom_header(char header[])
{
  const uint32_t sector = from_hexnum(header[0]);
  const uint32_t second = from_hexnum(header[1]);
  const uint32_t minute = from_hexnum(header[2]);

  const uint32_t sector_num = sector + second*75 + minute*60*75;

  return sector_num;
}

// size: 0x00000080
void function_80064198(void)
{
  UNREACHABLE;
  v0 = parse_cdrom_header(addr_to_pointer(a0));
}

// size: 0x00000024
uint32_t dma_cdrom_callback(uint32_t callback)
{
  return dma_callback(3, callback);
}

// size: 0x00000024
void function_80064050(void)
{
  UNREACHABLE;
  v0 = dma_cdrom_callback(a0);
}

// size: 0x00000050
void function_80065364(void)
{
  sw(0x80074E38, 0);
  sw(0x80074E34, 0);
  sw(cd_result2, 0);
  sw(cd_result1, 0);
  init_hook_entry_int2();
  a1 = 0x8006590C;
  a0 = 2; // 0x0002
  function_8005DDF8();
}

// size: 0x000000F4
void function_80065270(void)
{
  v1 = lw(SPU_voice_base_ptr);
  if (lhu(v1 + 0x01B8) == 0 && lhu(v1 + 0x01BA) == 0) {
    sh(v1 + 0x0180, 0x3FFF);
    sh(v1 + 0x0182, 0x3FFF);
  }
  sh(v1 + 0x01B0, 0x3FFF);
  sh(v1 + 0x01B2, 0x3FFF);
  sh(v1 + 0x01AA, 0xC001);
  
  sb(lw(CDROM_INDEX_ptr), 2);
  sb(lw(CDROM_REG_2_ptr), 0x80);
  sb(lw(CDROM_REG_3_ptr), 0);

  sb(lw(CDROM_INDEX_ptr), 3);
  sb(lw(CDROM_REG_1_ptr), 0x80);
  sb(lw(CDROM_REG_2_ptr), 0);
  sb(lw(CDROM_REG_3_ptr), 0x20);
  v0 = 0;
}

// size: 0x0000006C
void function_80063ACC(void)
{
  uint32_t s0 = a0;
  if (s0 == 2) {
    function_80065364();
    v0 = 1;
    return;
  }
  function_800653B4();
  if (v0) {
    v0 = 0;
    return;
  }
  if (s0 == 1) {
    function_80065270();
    if (v0) {
      v0 = 0;
      return;
    }
  }
  v0 = 1;
}

// size: 0x00000018
void function_80066254(void)
{
  v1 = 0x80075178;
  v0 = lw(v1);
  sw(v1, a0);
}

// size: 0x00000098
uint32_t CdInit(void)
{
  uint32_t s0 = 4;
  while (1) {
    a0 = 1;
    function_80063ACC();
    s0--;
    if (v0 == 1) {
      a0 = 0x80063A14;
      function_80063C18();
      a0 = 0x80063A3C;
      function_80063C30();
      a0 = 0x80063A64;
      function_8006623C();
      a0 = 0;
      function_80066254();
      return 1;
    }
    if (s0 == -1) {
      printf("CdInit: Init failed\n");
      return 0;
    }
  }
}

// size: 0x00000098
void function_8006397C(void)
{
  UNREACHABLE
  v0 = CdInit();
}

// size: 0x0000008C
void init_cdrom(void)
{
  sp -= 0x20;
  sb(sp + 0x10, 0x80);

  CdInit();

  a0 = 14;
  a1 = sp + 0x10;
  a2 = 0;
  function_80063C48();

  a0 = 0x80016490;
  function_8006623C();
  
  sw(0x800774B4, 0x40);

  sw(0x800776C4, 0);
  sw(0x800776C8, 0);
  sb(0x800776D2, 0x7F);
  sb(0x800776D0, 0x7F);
  sb(0x800776D3, 0);
  sb(0x800776D1, 0);

  a0 = 0x800776D0;
  function_80063FF0();

  sp += 0x20;
  return;
}

// size: 0x0000008C
void function_80012480(void)
{
  UNREACHABLE;
  init_cdrom();
}

// size: 0x00000588
void function_80064218(void)
{
  sp -= 0x30;
  sw(sp + 0x28, ra);
  sw(sp + 0x24, s1);
  sw(sp + 0x20, s0);

  sb(lw(CDROM_INDEX_ptr), 1);
  int intsts = lbu(lw(CDROM_REG_3_ptr)) & 7;
  sb(sp + 0x10, lbu(lw(CDROM_REG_3_ptr)) & 7);
  s1 = 0;
  if (intsts == CdlNoIntr) {
    v0 = 0;
    goto end;
  }

  int i;
  for (i = 0; i < 8; i++) {
    if ((lbu(lw(CDROM_INDEX_ptr)) & 0x20) == 0) break;
    sb(sp + 0x18 + i, lbu(lw(CDROM_REG_1_ptr)));
  }

  for (; i < 8; i++) {
    sb(sp + 0x18 + i, 0);
  }

  sb(lw(CDROM_INDEX_ptr), 1);
  sb(lw(CDROM_REG_3_ptr), 7);
  sb(lw(CDROM_REG_2_ptr), 7);
  if (intsts != CdlAcknowledge || lw(0x80074FFC + lbu(cd_current_command)*4)) {
    if ((lw(cd_result1) & 0x10) == 0 && lbu(sp + 0x18) & 0x10)
      sw(0x80074E4C, lw(0x80074E4C) + 1);

    v0 = lbu(sp + 0x18);
    s1 = v0 & 0x1D;
    sw(cd_result1, v0);
    sw(cd_result2, lbu(sp + 0x19));
  }

  if (intsts == CdlDiskError && (int32_t)lw(0x80074E40) > 0) {
    printf("DiskError: com=%s,code=(%02x:%02x)\n",
  	  cd_cmd_str[lbu(cd_current_command)],
  	  lw(cd_result1),
  	  lw(cd_result2)
  	);
  }

  switch (intsts)
  {
    case CdlDataReady:
      if (s1) {
        if (s0 == 1)
          s1 = 0;
        if (s1)
          v0 = 5;
        else
          v0 = 1;
      } else {
        v0 = 1;
      }
      sb(cd_ready_status, v0);
      spyro_memcpy8(0x80075AD8, sp + 0x18, 8);
      sb(lw(CDROM_INDEX_ptr), 0);
      sb(lw(CDROM_REG_3_ptr), 0);
      v0 = 4;
      break;
    case CdlComplete:
      if (s1)
        v0 = 5;
      else
        v0 = 2;
      sb(cd_sync_status, v0); // &0x00000000
      spyro_memcpy8(0x80075AD0, sp + 0x18, 8);
      v0 = 2;
      break;
    case CdlAcknowledge:
      if (s1) {
        sb(cd_sync_status, 5);
        v0 = 2;
      } else if (lw(0x80074EFC + lbu(cd_current_command)*4)) {
        sb(cd_sync_status, 3);
        v0 = 1;
      } else {
        sb(cd_sync_status, 2);
        v0 = 2;
      }
      spyro_memcpy8(0x80075AD0, sp + 0x18, 8);
      break;
    case CdlDataEnd:
      sb(0x80075116, 4);
      sb(cd_ready_status, lbu(0x80075116));
      spyro_memcpy8(0x80075AE0, sp + 0x18, 8);
      spyro_memcpy8(0x80075AD8, sp + 0x18, 8);
      v0 = 4;
      break;
    case CdlDiskError:
      sb(cd_ready_status, 5);
      sb(cd_sync_status, lbu(cd_ready_status));
      spyro_memcpy8(0x80075AD0, sp + 0x18, 8);
      spyro_memcpy8(0x80075AD8, sp + 0x18, 8);
      v0 = 6;
      break;
    default:
      puts("CDROM: unknown intr");
      printf("(%d)\n", lbu(sp + 0x10));
      v0 = 0;
      break;
  }
end:
  ra = lw(sp + 0x28);
  s1 = lw(sp + 0x24);
  s0 = lw(sp + 0x20);
  sp += 0x30;
  return;
}

// size: 0x000000E0
void function_80065190(void)
{
  sb(lw(CDROM_INDEX_ptr), 1);
  while (lbu(lw(CDROM_REG_3_ptr)) & 7) {
    sb(lw(CDROM_INDEX_ptr), 1);
    sb(lw(CDROM_REG_3_ptr), 7);
    sb(lw(CDROM_REG_2_ptr), 7);
  }
  sb(0x80075116, 0);
  sb(cd_ready_status, 0);
  sb(cd_sync_status, 2);
  sb(lw(CDROM_INDEX_ptr), 0);
  sb(lw(CDROM_REG_3_ptr), 0);
  sw(lw(MEM_COMMON_DELAY_ptr), 0x1325);
}

// size: 0x0000016C
void function_800655A0(void)
{
  bool nonblock = a0;
  int32_t frame_limit = VSync(-1) + 0x3C0;
  int32_t counter = 0;
  do {
    if (VSync(-1) > frame_limit || counter++ > 0x3C0000) {
      puts("CD timeout: ");
      printf("%s:(%s) Sync=%s, Ready=%s\n",
        "CD_datasync",
        cd_cmd_str[lbu(cd_current_command)],
        cd_status_str[lbu(cd_sync_status)],
        cd_status_str[lbu(cd_ready_status)]
      );
      function_80065190();
      v0 = -1;
      return;
    }
    if ((lw(lw(DMA_CDROM_channel_control_ptr)) & 0x01000000) == 0) {
      v0 = 0;
      return;
    }
  } while (!nonblock);

  v0 = 1;
  return;
}

// size: 0x00000100
void function_8006570C(void)
{
  sb(lw(CDROM_INDEX_ptr), 0);
  sb(lw(CDROM_REG_3_ptr), 0x80);
  sw(lw(MEM_CDROM_DELAY_ptr), 0x20943);
  sw(lw(MEM_COMMON_DELAY_ptr), 0x1323);
  v1 = lw(DMA_control_register_copy_3_ptr);
  sw(v1, lw(v1) | 0x8000);
  sw(lw(DMA_CDROM_address_ptr), a0);
  sw(lw(DMA_CDROM_block_control_ptr), a1 | 0x00010000);

  while ((lbu(lw(CDROM_INDEX_ptr)) & 0x40) == 0);

  sw(lw(DMA_CDROM_channel_control_ptr), 0x11000000);
  
  while (lw(lw(DMA_CDROM_channel_control_ptr)) & 0x01000000);
  
  sw(lw(MEM_COMMON_DELAY_ptr), 0x1325);
  v0 = 0;
  return;
}

// size: 0x000000F0
void function_8006580C(void)
{
  sb(lw(CDROM_INDEX_ptr), 0);
  sb(lw(CDROM_REG_3_ptr), 0x80);
  sw(lw(MEM_CDROM_DELAY_ptr), 0x21020843);
  sw(lw(MEM_COMMON_DELAY_ptr), 0x1325);
  v1 = lw(DMA_control_register_copy_3_ptr);
  sw(v1, lw(v1) | 0x8000);
  sw(lw(DMA_CDROM_address_ptr), a0);
  sw(lw(DMA_CDROM_block_control_ptr), a1 | 0x10000);

  while ((lbu(lw(CDROM_INDEX_ptr)) & 0x40) == 0);

  sw(lw(DMA_CDROM_channel_control_ptr), 0x11400100);
  lw(lw(DMA_CDROM_channel_control_ptr));

  v0 = 0;
  return;
}

// size: 0x00000028
void function_80063A14(void)
{
  DeliverEvent(0xF0000003, 0x20);
}

// size: 0x00000028
void function_80063A3C(void)
{
  DeliverEvent(0xF0000003, 0x40);
}

// size: 0x000000E4
void function_8006590C(void)
{
  sp -= 40; // 0xFFFFFFD8
  sw(sp + 0x20, ra);
  sw(sp + 0x10, s0);
  uint32_t saved_cdrom_index = lbu(lw(CDROM_INDEX_ptr)) & 3;
  while (1) {
    function_80064218();
    s0 = v0;
    if (s0 == 0) break;

    v0 = lw(0x80074E38);
    if ((s0 & 4) && v0) {
      a0 = lbu(cd_ready_status);
      a1 = 0x80075AD8;
      switch (v0)
      {
        case 0x80063A3C:
          function_80063A3C();
          break;
        case 0x800659F0:
          function_800659F0();
          break;
        default:
          JALR(v0, 0x8006597C);
      }
    }
    v0 = lw(0x80074E34);
    if ((s0 & 2) && v0) {
      a0 = lbu(cd_ready_status);
      a1 = 0x80075AD0;
      switch (v0)
      {
        case 0x80063A14:
          function_80063A14();
          break;
        default:
          JALR(v0, 0x800659B0);
      }
    }
  }
  sb(lw(CDROM_INDEX_ptr), saved_cdrom_index);
  ra = lw(sp + 0x20);
  s0 = lw(sp + 0x10);
  sp += 40; // 0x0028
  return;
}

// size: 0x00000214
void function_80065DBC(void)
{
  uint32_t temp;
  sp -= 40; // 0xFFFFFFD8
  sw(sp + 0x001C, s1);
  s1 = a0;
  a0 = 0;
  sw(sp + 0x0020, ra);
  sw(sp + 0x0018, s0);
  function_80063C18();
  a0 = 0;
  function_80063C30();
  v0 = 0x80075178; // &0x00000000
  v0 = lw(v0 + 0x0000);
  v0 = v0 & 0x1;
  temp = v0 == 0;
  if (temp) goto label80065E04;
  a0 = 0;
  v0 = dma_cdrom_callback(a0);
label80065E04:
  function_80063A8C();
  if (v0 & 0x10) goto label80065E6C;
  if ((VSync(-1) & 0x3F) == 0)
    puts("CdRead: Shell open...\n");
  a1 = 0;
  function_80063D80();
  a0 = -1; // 0xFFFFFFFF
  v0 = VSync(a0);
  sw(0x80075164, v0); // &0x00000000
  v0 = -1; // 0xFFFFFFFF
  sw(0x8007515C, v0); // &0x00000000
  goto label80065FB0;
label80065E6C:
  temp = s1 == 0;
  if (temp) goto label80065EC4;
  puts("CdRead: retry...\n");
  a0 = 9;
  a1 = 0;
  a2 = 0;
  function_80063C48();
  function_80063ABC();
  a0 = 2;
  a1 = v0;
  a2 = 0;
  function_80063C48();
  if (v0) goto label80065EC4;
  sw(0x8007515C, -1);
  goto label80065FB0;
label80065EC4:
  function_80063B38();
  s0 = lw(0x80075154);
  sb(sp + 0x10, s0);
  s0 = s0 & 0xFF;
  function_80063A9C();
  temp = s0 != v0;
  a0 = 14; // 0x000E
  if (temp) goto label80065EF8;
  temp = s1 == 0;
  if (temp) goto label80065F1C;
label80065EF8:
  a1 = sp + 16; // 0x0010
  a2 = 0;
  function_80063C48();
  temp = v0 != 0;
  v0 = -1; // 0xFFFFFFFF
  if (temp) goto label80065F1C;
  sw(0x8007515C, v0); // &0x00000000
  goto label80065FB0;
label80065F1C:
  function_80063ABC();
  sw(0x80075168, parse_cdrom_header(addr_to_pointer(v0)));
  a0 = 0x800659F0; // &0x27BDFFD8
  function_80063C30();
  v0 = lw(0x80075178); // &0x00000000
  v0 = v0 & 0x1;
  temp = v0 == 0;
  a0 = 6; // 0x0006
  if (temp) goto label80065F70;
  a0 = 0x80065CC0; // &0x3C028007
  v0 = dma_cdrom_callback(a0);
  a0 = 6; // 0x0006
label80065F70:
  a1 = 0;
  sw(0x80075150, lw(0x8007514C));
  function_80063D80();
  sw(0x8007515C, lw(0x80075148));
  sw(0x80075160, VSync(-1));
label80065FB0:
  v0 = lw(0x8007515C);
  ra = lw(sp + 0x20);
  s1 = lw(sp + 0x1C);
  s0 = lw(sp + 0x18);
  sp += 0x28;
  return;
}

// size: 0x00000104
void function_8006606C(void)
{
  sw(0x80075154, a2);
  a3 = a0;
  v1 = lw(0x80075154) & 0x30;
  if (v1 == 0) goto label800660A4;
  v0 = 0x249;
  if (v1 == 0x20) goto label800660A8;
  v0 = 0x246;
  goto label800660A8;
label800660A4:
  v0 = 0x200;
label800660A8:
  sw(0x80075158, v0);
  a0 = 0;
  sw(0x80075154, lw(0x80075154) | 0x20);
  sw(0x8007514C, a1);
  sw(0x80075148, a3);
  function_80063C18();
  sw(0x8007516C, v0); // &0x00000000

  a0 = 0;
  function_80063C30();
  sw(0x80075170, v0); // &0x00000000

  if (lw(0x80075178) & 1)
    sw(0x80075174, dma_cdrom_callback(0));

  sw(0x80075164, VSync(-1));
  function_80063A8C();

  if (v0 & 0xE0)
  {
    a0 = 9;
    a1 = 0;
    a2 = 0;
    function_80063EAC();
  }

  a0 = 0;
  function_80065DBC();
  v0 = (int32_t)0 < (int32_t)v0;
  return;
}

// size: 0x00000070
void function_80016490(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  a0 = a0 & 0xFF;
  v0 = 2; // 0x0002
  temp = a0 != v0;
  sw(sp + 0x0010, ra);
  if (temp) goto label800164C4;
  sw(0x80076BB8, 0);
  sw(0x8007588C, 0);
  sw(0x800756E0, 0); // &0x00000000
  goto label800164F0;
label800164C4:
  a0 = 2; // 0x0002
  a1 = 0x80076B98;
  a2 = 0;
  function_80063C48();
  a0 = lw(0x80076B94);
  a1 = lw(0x80076B9C);
  a2 = 128; // 0x0080
  function_8006606C();
label800164F0:
  ra = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x000002D0
uint32_t CdFunc1(uint8_t unknown, uint8_t *result)
{
  uint32_t temp;
  sp -= 40;
  sw(0x8007517C, pointer_to_addr(result));
  if (unknown != 1)
    sw(0x8007515C, -1);

  if ((int32_t)lw(0x8007515C) <= 0) goto label80065B94;
  if (lw(0x80075158) == 0x200) {
    if (lw(0x80075178) & 1) {
      dma_cdrom_callback(0);
      a0 = sp + 0x10;
      a1 = 3;
      function_80064030();
      a0 = 0;
      function_80064074();
      dma_cdrom_callback(0x80065CC0); // &0x3C028007
    } else {
      a0 = sp + 0x10;
      a1 = 3;
      function_80064010();
    }
    
    if (parse_cdrom_header(addr_to_pointer(sp + 0x10)) != lw(0x80075168)) {
      puts("CdRead: sector error\n");
      sw(0x8007515C, -1);
    }
  }
  if (lw(0x80075178) & 1) {
    a0 = lw(0x80075150);
    a1 = lw(0x80075158);
    function_80064030();
  } else {
    a0 = lw(0x80075150);
    a1 = lw(0x80075158);
    function_80064010();
    v0 = lw(0x80075158);
    v1 = lw(0x80075150);
    v0 = v0 << 2;
    v1 += v0;
    sw(0x80075150, v1);
    sw(0x8007515C, lw(0x8007515C)-1);
    sw(0x80075168, lw(0x80075168)+1);
  }
label80065B94:
  sw(0x80075160, VSync(-1));
  v0 = lw(0x8007515C);
  temp = (int32_t)v0 >= 0;
  if (temp) goto label80065BC0;
  a0 = 1; // 0x0001
  function_80065DBC();
label80065BC0:
  v1 = (int32_t)VSync(-1) > (int32_t)lw(0x80075164) + 1200;
  temp = v1 == 0;
  v0 = -1; // 0xFFFFFFFF
  if (temp) goto label80065BEC;
  sw(0x8007515C, v0); // &0x00000000
label80065BEC:
  v0 = lw(0x8007515C); // &0x00000000
  temp = v0 == 0;
  if (temp) goto label80065C24;
  v1 = lw(0x80075164); // &0x00000000
  v1 += 1200; // 0x04B0
  v1 = (int32_t)v1 < (int32_t)VSync(-1);
  temp = v1 == 0;
  if (temp) goto label80065CAC;
label80065C24:
  a0 = lw(0x8007516C); // &0x00000000
  function_80063C18();
  a0 = lw(0x80075170); // &0x00000000
  function_80063C30();
  if (lw(0x80075178) & 1)
    dma_cdrom_callback(lw(0x80075174));
  a0 = 9;
  a1 = 0;
  function_80063D80();
  v1 = lw(0x80075144); // &0x00000000
  temp = v1 == 0;
  if (temp) goto label80065CAC;
  v0 = lw(0x8007515C); // &0x00000000
  temp = v0 != 0;
  a0 = 5; // 0x0005
  if (temp) goto label80065CA4;
  a0 = 2; // 0x0002
label80065CA4:
  temp = v1;
  a1 = pointer_to_addr(result);
  switch (temp)
  {
  case 0x80016490:
    function_80016490();
    break;
  default:
    JALR(temp, 0x80065CA4);
  }
label80065CAC:
  sp += 40; // 0x0028
  return v0;
}

// size: 0x000002D0
void function_800659F0(void) {
  UNREACHABLE;
  v0 = CdFunc1(a0, addr_to_pointer(a1));
}

// size: 0x000000AC
void function_800163E4(void)
{
  v1 = lw(0x800756E0); // &0x00000000
  if (v1 == 0)
    return;

  if ((int32_t)v1 >= (int32_t)lw(0x8007588C))
    return;

  CdInit();
  a0 = 0x80016490;
  function_8006623C();
  sw(0x800774B4, 0x40);
  sw(0x800776C4, 0);
  sw(0x800776C8, 0);
  while (CdSync(1, NULL) != CdlComplete);
  a0 = lw(0x80076B94);
  a1 = lw(0x80076B9C);
  sw(0x8007588C, 0);
  a2 = 128; // 0x0080
  function_8006606C();
  return;
}

// size: 0x00000400
void function_8002BBE0(void)
{
  uint32_t temp;
  sp -= 48; // 0xFFFFFFD0
  sw(sp + 0x0028, ra);
  sw(sp + 0x0024, s1);
  sw(sp + 0x0020, s0);
  v0 = CdSync(1, addr_to_pointer(0x800776BC));
  v1 = lw(0x800776C4);
  temp = v1 == 0;
  s0 = v0;
  if (temp) goto label8002BC70;
  v0 = v1 < 8;
  temp = v0 != 0;
  v0 = v1 < 10;
  if (temp) goto label8002BC9C;
  temp = v0 != 0;
  v0 = 27; // 0x001B
  if (temp) goto label8002BC38;
  temp = v1 == v0;
  v0 = 16; // 0x0010
  if (temp) goto label8002BC58;
  goto label8002BC9C;
label8002BC38:
  v0 = lw(0x800774B4);
  v0 = v0 & 0x200;
  temp = v0 != 0;
  v0 = 2; // 0x0002
  if (temp) goto label8002BC9C;
  temp = s0 != v0;
  v0 = 64; // 0x0040
  if (temp) goto label8002BC9C;
label8002BC58:
  sw(0x800774B4, v0);
  sw(0x800776C4, 0);
  goto label8002BC9C;
label8002BC70:
  a1 = lw(0x800776C8);
  temp = a1 == 0;
  if (temp) goto label8002BC9C;
  a0 = lw(0x800774B0);
  function_800567F4();
  sw(0x800776C8, 0);
label8002BC9C:
  s1 = 0x800774B4;
  v0 = lw(s1 + 0x0000);
  v0 = v0 & 0x10;
  temp = v0 == 0;
  v0 = 2; // 0x0002
  if (temp) goto label8002BEA4;
  temp = s0 == v0;
  v0 = 5; // 0x0005
  if (temp) goto label8002BD30;
  temp = s0 != v0;
  if (temp) goto label8002BEA4;
  v0 = lw(0x800774B0);
  v0 = v0 << 3;
  a0 = lw(0x800774B8 + v0);
  a1 = sp + 16; // 0x0010
  write_cdrom_header(a0, addr_to_pointer(a1));
  a0 = 27; // 0x001B
  a1 = sp + 16; // 0x0010
  a2 = 0;
  function_80063C48();
  v0 = lw(0x800774B0);
  v1 = 27; // 0x001B
  sw(0x800776C4, v1);
  v0 = v0 << 3;
  v0 = lw(0x800774B8 + v0);
  sw(0x800776B8, v0);
  goto label8002BEA4;
label8002BD30:
  v0 = lw(0x800774B0);
  v1 = lw(0x800776B8);
  v0 = v0 << 3;
  v0 = lw(0x800774BC + v0);
  v0 = v0 < v1;
  temp = v0 == 0;
  if (temp) goto label8002BE70;
  v1 = lw(0x800758C8);
  v0 = lw(0x800776CC);
  v0 = v0 < v1;
  temp = v0 == 0;
  if (temp) goto label8002BE04;
  v0 = spyro_rand();
  v1 = 0x55555556;
  mult(v0, v1);
  v1 = lw(CONTINUOUS_LEVEL_ID);
  a0 = 0x8006F05C; // &0x00000000
  a1 = v1 << 1;
  a1 += v1;
  a1 = a1 << 2;
  a1 += a0;
  a0 = (int32_t)v0 >> 31;
  a3=hi;
  a0 = a3 - a0;
  v1 = a0 << 1;
  v1 += a0;
  v0 -= v1;
  v0 = v0 << 2;
  v0 += a1;
  v0 = lw(v0 + 0x0000);
  v1 = lw(0x800758C8);
  v0 = v0 << 2;
  v0 = lw(0x8006EF9C + v0); // &0x00000000
  v1 += 28800; // 0x7080
  sw(0x800776CC, v1);
  sw(0x800774B0, v0);
label8002BE04:
  v0 = lw(0x800774B0);
  v0 = v0 << 3;
  a0 = lw(0x800774B8 + v0);
  sw(0x800776B8, a0);
  a1 = sp + 16; // 0x0010
  write_cdrom_header(a0, addr_to_pointer(a1));
  a0 = 13; // 0x000D
  a1 = sp + 24; // 0x0018
  a2 = 0;
  v0 = lw(0x800774B0);
  v1 = 1; // 0x0001
  sb(sp + 0x0018, v1);
  v0 = v0 & 0x7;
  sb(sp + 0x0019, v0);
  function_80063EAC();
  a0 = 27; // 0x001B
  a1 = sp + 16; // 0x0010
  a2 = 0;
  function_80063C48();
  goto label8002BEA4;
label8002BE70:
  function_80063AAC();
  temp = v0 != CdlGetlocP;
  a0 = 17;
  if (temp) goto label8002BE9C;
  a0 = s1 + 525; // 0x020D
  v0 = parse_cdrom_header(addr_to_pointer(a0));
  temp = (int32_t)v0 <= 0;
  a0 = 17; // 0x0011
  if (temp) goto label8002BE9C;
  sw(0x800776B8, v0);
label8002BE9C:
  a1 = 0;
  function_80063D80();
label8002BEA4:
  a1 = 0x800761D8;
  a0 = lw(a1 + 0x0000);
  temp = (int32_t)a0 < 0;
  if (temp) goto label8002BFA8;
  v1 = lh(a1 - 0x02C0); // 0xFFFFFD40
  v0 = lw(a1 + 0x0058);
  temp = (int32_t)v0 >= 0;
  v1 += v0;
  if (temp) goto label8002BF5C;
  v0 = (int32_t)a0 < (int32_t)v1;
  temp = v0 != 0;
  if (temp) goto label8002BF80;
  temp = a0 != 0;
  if (temp) goto label8002BF30;
  a0 = lw(0x800776C4);
  v0 = a0 < 8;
  temp = v0 != 0;
  v0 = a0 < 10;
  if (temp) goto label8002BF30;
  temp = v0 != 0;
  v0 = 27; // 0x001B
  if (temp) goto label8002BF14;
  temp = a0 == v0;
  v0 = 16; // 0x0010
  if (temp) goto label8002BF28;
  goto label8002BF30;
label8002BF14:
  a0 = a0 & 0xFF;
  a1 = 0;
  a2 = 0;
  function_80063C48();
  v0 = 256; // 0x0100
label8002BF28:
  sw(0x800774B4, v0);
label8002BF30:
  v1 = lw(0x800761D8);
  v0 = -1; // 0xFFFFFFFF
  sw(0x80076230, 0);
  sw(0x800761D8, v0);
  sh(0x80075F18, v1);
  goto label8002BF84;
label8002BF5C:
  v0 = (int32_t)v1 < (int32_t)a0;
  temp = v0 != 0;
  v0 = -1; // 0xFFFFFFFF
  if (temp) goto label8002BF80;
  sw(a1 + 0x0058, 0);
  sh(a1 - 0x02C0, a0); // 0xFFFFFD40
  sh(0x80075F1A, a0);
  sw(a1 + 0x0000, v0);
  goto label8002BF8C;
label8002BF80:
  sh(a1 - 0x02C0, v1); // 0xFFFFFD40
label8002BF84:
  sh(0x80075F1A, v1);
label8002BF8C:
  a0 = 0x80075F08;
  v0 = 192; // 0x00C0
  sw(a0 + 0x0000, v0);
  function_8005CC58();
  goto label8002BFC8;
label8002BFA8:
  v1 = 0x800774B4;
  v0 = lw(v1 + 0x0000);
  v0 = v0 & 0x200;
  temp = v0 == 0;
  v0 = 256; // 0x0100
  if (temp) goto label8002BFC8;
  sw(v1 + 0x0000, v0);
label8002BFC8:
  ra = lw(sp + 0x0028);
  s1 = lw(sp + 0x0024);
  s0 = lw(sp + 0x0020);
  sp += 48; // 0x0030
  return;
}

// size: 0x00000334
void function_800567F4(void)
{
  uint32_t temp;
  sp -= 56; // 0xFFFFFFC8
  sw(sp + 0x0028, s0);
  s0 = a0;
  sw(sp + 0x0030, s2);
  s2 = 0x800774B4;
  sw(sp + 0x0034, ra);
  sw(sp + 0x002C, s1);
  v1 = lw(s2 + 0x0000);
  v0 = v1 & 0x80;
  temp = v0 != 0;
  s1 = a1;
  if (temp) goto label80056B0C;
  v0 = 2; // 0x0002
  temp = s1 == v0;
  v0 = (int32_t)s1 < 3;
  if (temp) goto label8005693C;
  temp = v0 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label8005684C;
  temp = s1 == v0;
  v0 = v1 & 0x40;
  if (temp) goto label80056868;
  goto label80056B0C;
label8005684C:
  v0 = 4; // 0x0004
  temp = s1 == v0;
  v0 = 8; // 0x0008
  if (temp) goto label8005693C;
  temp = s1 == v0;
  v0 = v1 & 0x40;
  if (temp) goto label800569EC;
  goto label80056B0C;
label80056868:
  temp = v0 == 0;
  if (temp) goto label80056908;
  v0 = lw(0x80075748); // &0x00000000
  temp = v0 == 0;
  a0 = 14; // 0x000E
  if (temp) goto label80056908;
  a1 = sp + 16; // 0x0010
  a2 = 0;
  v0 = 200; // 0x00C8
  sb(sp + 0x0010, v0);
  function_80063EAC();
  v1 = s0;
  v0 = 1; // 0x0001
  temp = (int32_t)s0 >= 0;
  sb(sp + 0x0020, v0);
  if (temp) goto label800568AC;
  v1 = s0 + 7; // 0x0007
label800568AC:
  a0 = 13; // 0x000D
  a1 = sp + 32; // 0x0020
  a2 = 0;
  v0 = (int32_t)v1 >> 3;
  v0 = v0 << 3;
  v0 = s0 - v0;
  sb(sp + 0x0021, v0);
  function_80063EAC();
  v0 = s0 << 3;
  s0 = sp + 24; // 0x0018
  a0 = lw(0x800774B8 + v0);
  a1 = s0;
  write_cdrom_header(a0, addr_to_pointer(a1));
  a0 = 27; // 0x001B
  a1 = s0;
  a2 = 0;
  function_80063EAC();
  temp = v0 == 0;
  if (temp) goto label80056AC4;
  goto label80056A7C;
label80056908:
  a0 = 0x800776C8;
  v1 = lw(a0 + 0x0000);
  temp = v1 != 0;
  v0 = 1; // 0x0001
  if (temp) goto label80056B00;
  v0 = lw(0x800776C4);
  temp = v0 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label80056B0C;
  goto label80056B00;
label8005693C:
  v1 = 0x800774B4;
  v0 = lw(v1 + 0x0000);
  v0 = v0 & 0x10;
  temp = v0 == 0;
  if (temp) goto label800569A4;
  v0 = lh(0x80075F18);
  temp = v0 == 0;
  v0 = -v0;
  if (temp) goto label80056998;
  v0 = (int32_t)v0 >> 3;
  sw(0x80076230, v0);
  v0 = 9; // 0x0009
  sw(0x800776C4, v0);
  v0 = 512; // 0x0200
  sw(0x800761D8, 0);
  sw(v1 + 0x0000, v0);
  goto label80056B0C;
label80056998:
  v0 = 64; // 0x0040
  sw(v1 + 0x0000, v0);
  goto label80056B0C;
label800569A4:
  v1 = lw(0x800776C8);
  temp = v1 != 0;
  v0 = 4; // 0x0004
  if (temp) goto label800569CC;
  v0 = lw(0x800776C4);
  temp = v0 == 0;
  v0 = 4; // 0x0004
  if (temp) goto label80056B0C;
label800569CC:
  temp = v1 == v0;
  v0 = 2; // 0x0002
  if (temp) goto label80056B0C;
  temp = v1 == v0;
  if (temp) goto label80056B0C;
  sw(0x800776C8, v0);
  goto label80056B0C;
label800569EC:
  temp = v0 == 0;
  if (temp) goto label80056AD4;
  v0 = lw(0x80075748); // &0x00000000
  temp = v0 == 0;
  a0 = 14; // 0x000E
  if (temp) goto label80056AD4;
  a1 = sp + 16; // 0x0010
  a2 = 0;
  v0 = 200; // 0x00C8
  sb(sp + 0x0010, v0);
  function_80063EAC();
  v1 = s0;
  v0 = 1; // 0x0001
  temp = (int32_t)s0 >= 0;
  sb(sp + 0x0020, v0);
  if (temp) goto label80056A30;
  v1 = s0 + 7; // 0x0007
label80056A30:
  a0 = 13; // 0x000D
  a1 = sp + 32; // 0x0020
  a2 = 0;
  v0 = (int32_t)v1 >> 3;
  v0 = v0 << 3;
  v0 = s0 - v0;
  sb(sp + 0x0021, v0);
  function_80063EAC();
  s0 = sp + 24; // 0x0018
  a0 = lw(0x800776B8);
  a1 = s0;
  write_cdrom_header(a0, addr_to_pointer(a1));
  a0 = 27; // 0x001B
  a1 = s0;
  a2 = 0;
  function_80063EAC();
  temp = v0 == 0;
  if (temp) goto label80056AC4;
label80056A7C:
  v0 = lw(0x80076224);
  a0 = lh(0x80075F18);
  v1 = 16; // 0x0010
  sw(0x800776C4, 0);
  sw(0x800776C8, 0);
  sw(s2 + 0x0000, v1);
  sw(0x800761D8, v0);
  v0 -= a0;
  v0 = (int32_t)v0 >> 3;
  sw(0x80076230, v0);
  goto label80056B0C;
label80056AC4:
  sw(0x800776C8, s1);
  goto label80056B0C;
label80056AD4:
  a0 = 0x800776C8;
  v1 = lw(a0 + 0x0000);
  temp = v1 != 0;
  v0 = 8; // 0x0008
  if (temp) goto label80056B00;
  v0 = lw(0x800776C4);
  temp = v0 == 0;
  v0 = 8; // 0x0008
  if (temp) goto label80056B0C;
label80056B00:
  temp = v1 == v0;
  if (temp) goto label80056B0C;
  sw(a0 + 0x0000, v0);
label80056B0C:
  ra = lw(sp + 0x0034);
  s2 = lw(sp + 0x0030);
  s1 = lw(sp + 0x002C);
  s0 = lw(sp + 0x0028);
  sp += 56; // 0x0038
  return;
}