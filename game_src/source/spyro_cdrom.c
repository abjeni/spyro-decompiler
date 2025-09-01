#include <stdint.h>
#include "psx_mem.h"
#include "psx_ops.h"
#include "main.h"
#include "spyro_vsync.h"
#include "spyro_system.h"
#include "decompilation.h"
#include "not_renamed.h"
#include "spyro_print.h"
#include "spyro_constants.h"
#include "spyro_cdrom.h"

uint8_t to_hexnum(uint8_t num)
{
  return ((num/10) << 4) + (num % 10);
}

uint8_t from_hexnum(uint8_t num)
{
  return ((num >> 4)*10) + (num % 10);
}

// size: 0x00000104
char *write_cdrom_header(uint32_t sector_num, char header[])
{
  sector_num += 150;

  const uint32_t sector = sector_num % 75;
  const uint32_t second = (sector_num / 75) % 60;
  const uint32_t minute = (sector_num / 75) / 60;

  header[0] = to_hexnum(minute);
  header[1] = to_hexnum(second);
  header[2] = to_hexnum(sector);

  return header;
}

// size: 0x00000104
void function_80064094(void)
{
  BREAKPOINT;
  v0 = pointer_to_addr(write_cdrom_header(a0, addr_to_pointer(a1)));
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
  BREAKPOINT;
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
  BREAKPOINT;
  v0 = dma_cdrom_callback(a0);
}

// size: 0x00000030
void function_8005DDC8(void)
{
  if (lw(lw(0x800749AC) + 0x0C) != 0x8005DF60) BREAKPOINT;
  function_8005DF60();
}

// size: 0x00000050
void function_80065364(void)
{
  sw(0x80074E38, 0);
  sw(0x80074E34, 0);
  sw(0x80074E48, 0);
  sw(0x80074E44, 0);
  function_8005DDC8();
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

// size: 0x000001EC
void function_800653B4(void)
{
  v0 = 0;
  return;
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

int CdSync(int mode, uint8_t *result)
{
  //if (result) BREAKPOINT;

  return CdlComplete;
}

// size: 0x00000020
void function_80063BD8(void)
{
  v0 = CdSync(a0, addr_to_pointer(a1));
  BREAKPOINT;
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
  BREAKPOINT
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
  BREAKPOINT;
  init_cdrom();
}

void read_disk(uint32_t sector, uint32_t dst, uint32_t len, uint32_t offset, uint32_t num)
{
  if (len & 0x7FF || offset & 0x7FF || num != 0x258) {
    printf("len: %X offset: %X num: %X\n", len, offset, num);
    BREAKPOINT;
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
  BREAKPOINT;
  read_disk1(a0, a1, a2, a3, lw(sp+0x10));
}

void function_80016698(void)
{
  BREAKPOINT;
  read_disk2(a0, a1, a2, a3, lw(sp+0x10));
}

// size: 0x00000588
void function_80064218(void)
{
  uint32_t temp;
  sp -= 48; // 0xFFFFFFD0
  v1 = lw(CDROM_INDEX_ptr);
  v0 = 1; // 0x0001
  sw(sp + 0x0028, ra);
  sw(sp + 0x0024, s1);
  sw(sp + 0x0020, s0);
  sb(v1 + 0x0000, v0);
  a0 = lw(CDROM_REG_3_ptr);
  v0 = lbu(a0 + 0x0000);
  v0 = v0 & 0x7;
  sb(sp + 0x0010, v0);
  v0 = lbu(sp + 0x0010);
  temp = v0 == 0;
  s1 = 0;
  if (temp) goto label80064784;
  goto label8006427C;
label8006426C:
  v0 = lbu(a0 + 0x0000);
  v0 = v0 & 0x7;
  sb(sp + 0x0010, v0);
label8006427C:
  v0 = lbu(a0 + 0x0000);
  v1 = lbu(sp + 0x0010);
  v0 = v0 & 0x7;
  temp = v1 != v0;
  s0 = 0;
  if (temp) goto label8006426C;
  v1 = sp + 24; // 0x0018
label80064294:
  v0 = lw(CDROM_INDEX_ptr);
  v0 = lbu(v0 + 0x0000);
  v0 = v0 & 0x20;
  temp = v0 == 0;
  v0 = (int32_t)s0 < 8;
  if (temp) goto label800642DC;
  v0 = lw(CDROM_REG_1_ptr);
  v0 = lbu(v0 + 0x0000);
  s0++;
  sb(v1 + 0x0000, v0);
  v0 = (int32_t)s0 < 8;
  temp = v0 != 0;
  v1++;
  if (temp) goto label80064294;
  v0 = (int32_t)s0 < 8;
label800642DC:
  temp = v0 == 0;
  v0 = sp + 24; // 0x0018
  if (temp) goto label80064300;
  v1 = s0 + v0;
  a0 = sp + 32; // 0x0020
label800642EC:
  sb(v1 + 0x0000, 0);
  v1++;
  v0 = (int32_t)v1 < (int32_t)a0;
  temp = v0 != 0;
  if (temp) goto label800642EC;
label80064300:
  v1 = lw(CDROM_INDEX_ptr);
  v0 = 1; // 0x0001
  sb(v1 + 0x0000, v0);
  v0 = lw(CDROM_REG_3_ptr);
  v1 = 7; // 0x0007
  sb(v0 + 0x0000, v1);
  v0 = lw(CDROM_REG_2_ptr);
  sb(v0 + 0x0000, v1);
  v0 = lbu(sp + 0x0010);
  v1 = 3; // 0x0003
  v0 = v0 & 0xFF;
  temp = v0 != v1;
  if (temp) goto label8006436C;
  v0 = lbu(0x80074E55); // &0x00000000
  v0 = v0 << 2;
  v0 = lw(0x80074FFC + v0); // &0x00000001
  temp = v0 == 0;
  if (temp) goto label800643D0;
label8006436C:
  v0 = lw(0x80074E44); // &0x00000000
  v0 = v0 & 0x10;
  temp = v0 != 0;
  if (temp) goto label800643B0;
  v0 = lbu(sp + 0x0018);
  v0 = v0 & 0x10;
  temp = v0 == 0;
  if (temp) goto label800643B0;
  v0 = lw(0x80074E4C); // &0x00000000
  v0++;
  sw(0x80074E4C, v0); // &0x00000000
label800643B0:
  v0 = lbu(sp + 0x0018);
  v1 = lbu(sp + 0x0019);
  v0 = v0 & 0xFF;
  s1 = v0 & 0x1D;
  sw(0x80074E44, v0);
  sw(0x80074E48, v1);
label800643D0:
  v0 = lbu(sp + 0x10);
  v1 = 5;
  v0 = v0 & 0xFF;
  if (v0 != 5) goto label80064454;
  if ((int32_t)lw(0x80074E40) <= 0) goto label80064454;
  printf("DiskError: ");
  if ((int32_t)lw(0x80074E40) <= 0) goto label80064454;
  printf("com=%s,code=(%02x:%02x)\n",
    (char *)addr_to_pointer(lw(0x80074E5C + lbu(0x80074E55)*4)),
    lw(0x80074E44),
    lw(0x80074E48)
  );
label80064454:
  v0 = lbu(sp + 0x0010);
  v1 = v0 - 1; // 0xFFFFFFFF
  v0 = v1 < 5;
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label80064760;
  v0 = lw(0x80011E70 + v0); // &0x800645D0
  temp = v0;
  switch (temp)
  {
  case 0x800645D0: // 0
    goto label800645D0;
    break;
  case 0x80064584: // 1
    goto label80064584;
    break;
  case 0x80064484: // 2
    goto label80064484;
    break;
  case 0x80064654: // 3
    goto label80064654;
    break;
  case 0x800646D8: // 4
    goto label800646D8;
    break;
  default:
    JR(temp, 0x8006447C);
    return;
  }
label80064484:
  temp = s1 == 0;
  v0 = 5; // 0x0005
  if (temp) goto label800644D0;
  v1 = 0x80075114; // &0x00000000
  sb(v1 + 0x0000, v0);
  v1 = 0x80075AD0;
  temp = v1 == 0;
  a1 = sp + 24; // 0x0018
  if (temp) goto label800645C8;
  a0 = 7; // 0x0007
  a2 = -1; // 0xFFFFFFFF
label800644B0:
  v0 = lbu(a1 + 0x0000);
  a1++;
  a0--;
  sb(v1 + 0x0000, v0);
  temp = a0 != -1;
  v1++;
  if (temp) goto label800644B0;
  v0 = 2; // 0x0002
  goto label80064788;
label800644D0:
  v0 = lbu(0x80074E55); // &0x00000000
  v0 = v0 << 2;
  v0 = lw(0x80074EFC + v0); // &0x00000000
  temp = v0 == 0;
  v0 = 3; // 0x0003
  if (temp) goto label8006453C;
  v1 = 0x80075114; // &0x00000000
  sb(v1 + 0x0000, v0);
  v1 = 0x80075AD0;
  temp = v1 == 0;
  a1 = sp + 24; // 0x0018
  if (temp) goto label80064534;
  a0 = 7; // 0x0007
  a2 = -1; // 0xFFFFFFFF
label8006451C:
  v0 = lbu(a1 + 0x0000);
  a1++;
  a0--;
  sb(v1 + 0x0000, v0);
  temp = a0 != -1;
  v1++;
  if (temp) goto label8006451C;
label80064534:
  v0 = 1; // 0x0001
  goto label80064788;
label8006453C:
  v1 = 0x80075114; // &0x00000000
  v0 = 2; // 0x0002
  sb(v1 + 0x0000, v0);
  v1 = 0x80075AD0;
  temp = v1 == 0;
  a1 = sp + 24; // 0x0018
  if (temp) goto label800645C8;
  a0 = 7; // 0x0007
  a2 = -1; // 0xFFFFFFFF
label80064564:
  v0 = lbu(a1 + 0x0000);
  a1++;
  a0--;
  sb(v1 + 0x0000, v0);
  temp = a0 != -1;
  v1++;
  if (temp) goto label80064564;
  v0 = 2; // 0x0002
  goto label80064788;
label80064584:
  temp = s1 == 0;
  v0 = 2; // 0x0002
  if (temp) goto label80064590;
  v0 = 5; // 0x0005
label80064590:
  sb(0x80075114, v0); // &0x00000000
  v1 = 0x80075AD0;
  temp = v1 == 0;
  a1 = sp + 24; // 0x0018
  if (temp) goto label800645C8;
  a0 = 7; // 0x0007
  a2 = -1; // 0xFFFFFFFF
label800645B0:
  v0 = lbu(a1 + 0x0000);
  a1++;
  a0--;
  sb(v1 + 0x0000, v0);
  temp = a0 != -1;
  v1++;
  if (temp) goto label800645B0;
label800645C8:
  v0 = 2; // 0x0002
  goto label80064788;
label800645D0:
  temp = s1 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label800645F4;
  v0 = 1; // 0x0001
  temp = s0 != v0;
  if (temp) goto label800645E8;
  s1 = 0;
label800645E8:
  temp = s1 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label800645F4;
  v0 = 5; // 0x0005
label800645F4:
  sb(0x80075115, v0); // &0x14000000
  v1 = 0x80075AD8;
  temp = v1 == 0;
  a1 = sp + 24; // 0x0018
  if (temp) goto label8006462C;
  a0 = 7; // 0x0007
  a2 = -1; // 0xFFFFFFFF
label80064614:
  v0 = lbu(a1 + 0x0000);
  a1++;
  a0--;
  sb(v1 + 0x0000, v0);
  temp = a0 != -1;
  v1++;
  if (temp) goto label80064614;
label8006462C:
  v0 = lw(CDROM_INDEX_ptr);
  sb(v0 + 0x0000, 0);
  v1 = lw(CDROM_REG_3_ptr);
  v0 = 4; // 0x0004
  sb(v1 + 0x0000, 0);
  goto label80064788;
label80064654:
  a0 = 0x80075AE0;
  v0 = 4; // 0x0004
  sb(0x80075116, v0); // &0x51140000
  v0 = lbu(0x80075116); // &0x51140000
  a1 = sp + 24; // 0x0018
  sb(0x80075115, v0); // &0x14000000
  temp = a0 == 0;
  v1 = 7; // 0x0007
  if (temp) goto label800646A0;
  a2 = -1; // 0xFFFFFFFF
label80064688:
  v0 = lbu(a1 + 0x0000);
  a1++;
  v1--;
  sb(a0 + 0x0000, v0);
  temp = v1 != -1;
  a0++;
  if (temp) goto label80064688;
label800646A0:
  v1 = 0x80075AD8;
  temp = v1 == 0;
  a1 = sp + 24; // 0x0018
  if (temp) goto label800646D0;
  a0 = 7; // 0x0007
  a2 = -1; // 0xFFFFFFFF
label800646B8:
  v0 = lbu(a1 + 0x0000);
  a1++;
  a0--;
  sb(v1 + 0x0000, v0);
  temp = a0 != -1;
  v1++;
  if (temp) goto label800646B8;
label800646D0:
  v0 = 4; // 0x0004
  goto label80064788;
label800646D8:
  a0 = 0x80075AD0;
  a1 = sp + 24; // 0x0018
  v0 = 5; // 0x0005
  sb(0x80075115, 5);
  v1 = lbu(0x80075115); // &0x14000000
  v0 = 0x80075114; // &0x00000000
  sb(v0 + 0x0000, v1);
  v1 = 7; // 0x0007
  a2 = -1; // 0xFFFFFFFF
label80064710:
  v0 = lbu(a1);
  a1++;
  v1--;
  sb(a0, v0);
  a0++;
  if (v1 != -1) goto label80064710;
  v1 = 0x80075AD8;
  a1 = sp + 24; // 0x0018
  a0 = 7; // 0x0007
  a2 = -1; // 0xFFFFFFFF
label80064740:
  v0 = lbu(a1 + 0x0000);
  a1++;
  a0--;
  sb(v1 + 0x0000, v0);
  v1++;
  if (a0 != -1) goto label80064740;
  v0 = 6; // 0x0006
  goto label80064788;
label80064760:
  puts("CDROM: unknown intr");
  printf("(%d)\n", lbu(sp + 0x10));
label80064784:
  v0 = 0;
label80064788:
  ra = lw(sp + 0x0028);
  s1 = lw(sp + 0x0024);
  s0 = lw(sp + 0x0020);
  sp += 48; // 0x0030
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
  sb(0x80075115, 0);
  sb(0x80075114, 2);
  sb(lw(CDROM_INDEX_ptr), 0);
  sb(lw(CDROM_REG_3_ptr), 0);
  sw(lw(MEM_COMMON_DELAY_ptr), 0x1325);
}

// size: 0x0000016C
void function_800655A0(void)
{
  uint32_t temp;
  sp -= 48; // 0xFFFFFFD0
  sw(sp + 0x0020, s2);
  s2 = a0;
  a0 = -1; // 0xFFFFFFFF
  sw(sp + 0x0028, ra);
  sw(sp + 0x0024, s3);
  sw(sp + 0x001C, s1);
  v0 = VSync(a0);
  s3 = 0x80074E5C; // &0x80011DB4
  s1 = 0x80075114; // &0x00000000
  v0 += 960; // 0x03C0
  sw(0x80075AE8, v0);
  v0 = 0x80011F04; // "CD_datasync"
  sw(0x80075AEC, 0);
  sw(0x80075AF0, v0);
label80065600:
  a0 = -1; // 0xFFFFFFFF
  v0 = VSync(a0);
  v1 = lw(0x80075AE8);
  v1 = (int32_t)v1 < (int32_t)v0;
  temp = v1 != 0;
  if (temp) goto label80065648;
  v1 = lw(0x80075AEC);
  v0 = v1 + 1; // 0x0001
  sw(0x80075AEC, v0);
  v0 = 0x003C0000;
  v0 = (int32_t)v0 < (int32_t)v1;
  temp = v0 == 0;
  if (temp) goto label800656B8;
label80065648:
  puts("CD timeout: ");
  printf("%s:(%s) Sync=%s, Ready=%s\n",
    (char *)addr_to_pointer(lw(0x80075AF0)),
    cd_cmd_str[lbu(0x80074E55)],
    cd_status_str[lbu(0x80075114)],
    cd_status_str[lbu(0x80075115)]
  );
  function_80065190();
  v0 = -1;
  goto label800656BC;
label800656B8:
  v0 = 0;
label800656BC:
  temp = v0 != 0;
  v0 = -1; // 0xFFFFFFFF
  if (temp) goto label800656EC;
  v0 = lw(DMA_CDROM_channel_control_ptr);
  v0 = lw(v0 + 0x0000);
  v1 = 0x01000000;
  v0 = v0 & v1;
  temp = v0 == 0;
  v0 = 0;
  if (temp) goto label800656EC;
  temp = s2 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label80065600;
label800656EC:
  ra = lw(sp + 0x0028);
  s3 = lw(sp + 0x0024);
  s2 = lw(sp + 0x0020);
  s1 = lw(sp + 0x001C);
  sp += 48; // 0x0030
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

// size: 0x000000E4
void function_8006590C(void)
{
  uint32_t temp;
  v0 = lw(CDROM_INDEX_ptr);
  sp -= 40; // 0xFFFFFFD8
  sw(sp + 0x0014, s1);
  s1 = 0x80075115; // &0x14000000
  sw(sp + 0x0020, ra);
  sw(sp + 0x001C, s3);
  sw(sp + 0x0018, s2);
  sw(sp + 0x0010, s0);
  v0 = lbu(v0 + 0x0000);
  s3 = s1 - 1; // 0xFFFFFFFF
  s2 = v0 & 0x3;
label80065940:
  function_80064218();
  s0 = v0;
  temp = s0 == 0;
  v0 = s0 & 0x4;
  if (temp) goto label800659C0;
  temp = v0 == 0;
  v0 = s0 & 0x2;
  if (temp) goto label80065988;
  v0 = lw(0x80074E38); // &0x00000000
  temp = v0 == 0;
  if (temp) goto label80065984;
  a0 = lbu(s1 + 0x0000);
  a1 = 0x80075AD8;
  temp = v0;
  switch (temp)
  {
  case 0x80063A3C:
    function_80063A3C();
    break;
  case 0x800659F0:
    function_800659F0();
    break;
  default:
    JALR(temp, 0x8006597C);
  }
label80065984:
  v0 = s0 & 0x2;
label80065988:
  temp = v0 == 0;
  if (temp) goto label80065940;
  v0 = lw(0x80074E34); // &0x00000000
  temp = v0 == 0;
  if (temp) goto label80065940;
  a0 = lbu(s3 + 0x0000);
  a1 = 0x80075AD0;
  temp = v0;
  switch (temp)
  {
  case 0x80063A14:
    function_80063A14();
    break;
  default:
    JALR(temp, 0x800659B0);
  }
  goto label80065940;
label800659C0:
  sb(lw(CDROM_INDEX_ptr), s2);
  ra = lw(sp + 0x0020);
  s3 = lw(sp + 0x001C);
  s2 = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
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
  temp = v0 != 0;
  v0 = -1; // 0xFFFFFFFF
  if (temp) goto label80065EC4;
  sw(0x8007515C, v0); // &0x00000000
  goto label80065FB0;
label80065EC4:
  function_80063B38();
  v0 = 0x80075154; // &0x00000000
  s0 = lw(v0 + 0x0000);
  sb(sp + 0x0010, s0);
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
  a0 = v0;
  v0 = parse_cdrom_header(addr_to_pointer(a0));
  a0 = 0x800659F0; // &0x27BDFFD8
  sw(0x80075168, v0); // &0x00000000
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
  sp -= 0x18;
  sw(sp + 0x10, ra);

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
  ra = lw(sp + 0x10);
  sp += 0x18;
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
void function_800659F0(void)
{
  uint32_t temp;
  sp -= 40; // 0xFFFFFFD8
  sw(sp + 0x0020, s0);
  s0 = a1;
  a0 = a0 & 0xFF;
  v0 = 1; // 0x0001
  sw(sp + 0x0024, ra);
  sw(0x8007517C, s0); // &0x00000000
  temp = a0 != v0;
  v0 = -1; // 0xFFFFFFFF
  if (temp) goto label80065B8C;
  v0 = lw(0x8007515C); // &0x00000000
  temp = (int32_t)v0 <= 0;
  if (temp) goto label80065B94;
  v1 = lw(0x80075158); // &0x00000000
  v0 = 512; // 0x0200
  temp = v1 != v0;
  if (temp) goto label80065AD0;
  v0 = lw(0x80075178); // &0x00000000
  v0 = v0 & 0x1;
  temp = v0 == 0;
  a0 = sp + 16; // 0x0010
  if (temp) goto label80065A8C;
  a0 = 0;
  v0 = dma_cdrom_callback(a0);
  a0 = sp + 16; // 0x0010
  a1 = 3; // 0x0003
  function_80064030();
  a0 = 0;
  function_80064074();
  a0 = 0x80065CC0; // &0x3C028007
  v0 = dma_cdrom_callback(a0);
  goto label80065A94;
label80065A8C:
  a1 = 3; // 0x0003
  function_80064010();
label80065A94:
  a0 = sp + 16; // 0x0010
  v0 = parse_cdrom_header(addr_to_pointer(a0));
  v1 = 0x80075168; // &0x00000000
  v1 = lw(v1 + 0x0000);
  temp = v0 == v1;
  if (temp) goto label80065AD0;
  a0 = 0x80011F10; // "CdRead: sector error\n"
  spyro_puts(a0);
  v0 = -1; // 0xFFFFFFFF
  sw(0x8007515C, v0); // &0x00000000
label80065AD0:
  v0 = 0x80075178; // &0x00000000
  v0 = lw(v0 + 0x0000);
  v0 = v0 & 0x1;
  temp = v0 == 0;
  if (temp) goto label80065B0C;
  a0 = lw(0x80075150); // &0x00000000
  a1 = lw(0x80075158); // &0x00000000
  function_80064030();
  goto label80065B94;
label80065B0C:
  a0 = lw(0x80075150); // &0x00000000
  a1 = lw(0x80075158); // &0x00000000
  function_80064010();
  v0 = lw(0x80075158); // &0x00000000
  v1 = lw(0x80075150); // &0x00000000
  v0 = v0 << 2;
  v1 += v0;
  sw(0x80075150, v1); // &0x00000000
  v0 = lw(0x8007515C); // &0x00000000
  v0--;
  sw(0x8007515C, v0); // &0x00000000
  v0 = lw(0x8007515C); // &0x00000000
  v0 = lw(0x80075168); // &0x00000000
  v0++;
  sw(0x80075168, v0); // &0x00000000
  v0 = lw(0x80075168); // &0x00000000
  goto label80065B94;
label80065B8C:
  sw(0x8007515C, v0); // &0x00000000
label80065B94:
  a0 = -1; // 0xFFFFFFFF
  v0 = VSync(a0);
  sw(0x80075160, v0); // &0x00000000
  v0 = lw(0x8007515C); // &0x00000000
  temp = (int32_t)v0 >= 0;
  if (temp) goto label80065BC0;
  a0 = 1; // 0x0001
  function_80065DBC();
label80065BC0:
  a0 = -1; // 0xFFFFFFFF
  v0 = VSync(a0);
  v1 = lw(0x80075164); // &0x00000000
  v1 += 1200; // 0x04B0
  v1 = (int32_t)v1 < (int32_t)v0;
  temp = v1 == 0;
  v0 = -1; // 0xFFFFFFFF
  if (temp) goto label80065BEC;
  sw(0x8007515C, v0); // &0x00000000
label80065BEC:
  v0 = lw(0x8007515C); // &0x00000000
  temp = v0 == 0;
  if (temp) goto label80065C24;
  a0 = -1; // 0xFFFFFFFF
  v0 = VSync(a0);
  v1 = lw(0x80075164); // &0x00000000
  v1 += 1200; // 0x04B0
  v1 = (int32_t)v1 < (int32_t)v0;
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
  a1 = s0;
  switch (temp)
  {
  case 0x80016490:
    function_80016490();
    break;
  default:
    JALR(temp, 0x80065CA4);
  }
label80065CAC:
  ra = lw(sp + 0x0024);
  s0 = lw(sp + 0x0020);
  sp += 40; // 0x0028
  return;
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
  while (CdSync(a0, addr_to_pointer(a1)) != CdlComplete);
  a0 = lw(0x80076B94);
  a1 = lw(0x80076B9C);
  sw(0x8007588C, 0);
  a2 = 128; // 0x0080
  function_8006606C();
  return;
}