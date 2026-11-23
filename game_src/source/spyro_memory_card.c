#include "main.h"
#include "psx_bios.h"
#include "psx_mem.h"
#include "decompilation.h"
#include "spyro_string.h"
#include "spyro_memory_card.h"
#include "psx_ops.h"
#include "not_renamed.h"
#include "level_loading.h"
#include "spyro_math.h"
#include "string.h"

// size: 0x00000010
void function_80068A1C(void)
{
  v0 = lw(0x800751A0);
}

// size: 0x00000014
void function_80067614(void)
{
  v1 = 0x80075B90;
  v0 = lw(v1);
  sw(v1, a0);
}

// size: 0x00000010
void function_80069030(void)
{
  v0 = lw(0x800751B0) >> 31;
}

// size: 0x00000058
void function_80068958(void)
{
  sp -= 0x18;
  sw(sp + 0x10, s0);
  sw(sp + 0x14, ra);
  
  s0 = a0;

  ChangeClearPAD(0);
  EnterCriticalSection();
  function_80068A1C(); // v0 = lw(0x800751A0);
  if (v0 == 0)
    s0 = 0;
  InitCARD2(s0);
  ExitCriticalSection();

  ra = lw(sp + 0x14);
  s0 = lw(sp + 0x10);
  sp += 0x18;
}

// size: 0x00000038
void function_800689B0(void)
{
  EnterCriticalSection();
  StartCARD2();
  ChangeClearPAD(0);
  ExitCriticalSection();
}

// size: 0x00000030
void function_80067E70(void)
{
  function_80068958();
  function_800689B0();
  _bu_init();
  v0 = 0;
}

// size: 0x00000030
void function_8006626C(void)
{
  sw(0x80075B4C, 0);
  sw(0x80075B90, 0);
  function_80067E70();
}

// size: 0x00000020
uint32_t init_memory_card(void)
{
  a0 = 0;
  function_8006626C();
  return v0;
}

// size: 0x00000020
void function_80012460(void)
{
  DEPRECATED;
  v0 = init_memory_card();
}

void setup_bu_prefix(uint32_t num, uint32_t dst)
{
  memcpy(addr_to_pointer(dst), "bu00:", sizeof("bu00:"));

  uint32_t num1 = ((num >> 4) & 0xF);
  uint32_t num2 = ((num >> 0) & 0xF);

  assert(num1 < 10);
  assert(num2 < 10);

  sb(dst + 2, num1 + '0');
  sb(dst + 3, num2 + '0');
}

// size: 0x0000005C
void function_80067D74(void)
{
  DEPRECATED;
  setup_bu_prefix(a0, a1);
}

// size: 0x00000108
void function_8006815C(void)
{
  TestEvent(lw(MEMCARD_EVENTS + 0x00));
  TestEvent(lw(MEMCARD_EVENTS + 0x04));
  TestEvent(lw(MEMCARD_EVENTS + 0x08));
  TestEvent(lw(MEMCARD_EVENTS + 0x0C));
  TestEvent(lw(MEMCARD_EVENTS + 0x10));
  TestEvent(lw(MEMCARD_EVENTS + 0x14));
  TestEvent(lw(MEMCARD_EVENTS + 0x18));
  TestEvent(lw(MEMCARD_EVENTS + 0x1C));
  sw(0x80075B38, 0);
  sw(0x80075B34, lw(0x80075B38));
  sw(0x80075B30, lw(0x80075B34));
  sw(0x80075B2C, lw(0x80075B30));
  sw(0x80075B48, 0);
  sw(0x80075B44, lw(0x80075B48));
  sw(0x80075B40, lw(0x80075B44));
  sw(0x80075B3C, lw(0x80075B40));
}

// size: 0x000000DC
void function_80068264(void)
{
  uint32_t event = 0;

  while (event == 0) {
    event = lw(0x80075B2C)*1
          + lw(0x80075B30)*2
          + lw(0x80075B34)*4
          + lw(0x80075B38)*8;
  }

  TestEvent(lw(0x80075B1C));
  TestEvent(lw(0x80075B20));
  TestEvent(lw(0x80075B24));
  TestEvent(lw(0x80075B28));
  sw(0x80075B38, 0);
  sw(0x80075B34, lw(0x80075B38));
  sw(0x80075B30, lw(0x80075B34));
  sw(0x80075B2C, lw(0x80075B30));

  v0 = event >> 1;
}

// size: 0x000000DC
void function_80068340(void)
{
  uint32_t event = 0;

  while (event == 0) {
    event = lw(0x80075B3C)*1
          + lw(0x80075B40)*2
          + lw(0x80075B44)*4
          + lw(0x80075B48)*8;
  }

  TestEvent(lw(MEMCARD_EVENTS + 0x00));
  TestEvent(lw(MEMCARD_EVENTS + 0x04));
  TestEvent(lw(MEMCARD_EVENTS + 0x08));
  TestEvent(lw(MEMCARD_EVENTS + 0x0C));

  sw(0x80075B48, 0);
  sw(0x80075B44, lw(0x80075B48));
  sw(0x80075B40, lw(0x80075B44));
  sw(0x80075B3C, lw(0x80075B40));

  v0 = event >> 1;
}

// size: 0x0000003C
void function_8006841C(void)
{
  v0 = lw(0x80075B2C)*1
     + lw(0x80075B30)*2
     + lw(0x80075B34)*4
     + lw(0x80075B38)*8;
}

// size: 0x0000003C
void function_80068458(void)
{
  v0 = lw(0x80075B3C)*1
     + lw(0x80075B40)*2
     + lw(0x80075B44)*4
     + lw(0x80075B48)*8;
}

// size: 0x00000014
void function_80067DD0(void)
{
  sw(0x80075B2C, 1);
  v0 = 0;
}

// size: 0x00000014
void function_80067DE4(void)
{
  sw(0x80075B30, 1);
  v0 = 0;
}

// size: 0x00000014
void function_80067DF8(void)
{
  sw(0x80075B34, 1);
  v0 = 0;
}

// size: 0x00000014
void function_80067E0C(void)
{
  sw(0x80075B38, 1);
  v0 = 0;
}

// size: 0x00000014
void function_80067E20(void)
{
  sw(0x80075B3C, 1);
  v0 = 0;
}

// size: 0x00000014
void function_80067E34(void)
{
  sw(0x80075B40, 1);
  v0 = 0;
}

// size: 0x00000014
void function_80067E48(void)
{
  sw(0x80075B44, 1);
  v0 = 0;
}

// size: 0x00000014
void function_80067E5C(void)
{
  sw(0x80075B48, 1);
  v0 = 0;
}

// size: 0x000001E4
void MemCardStart(void)
{
  uint32_t crit = EnterCriticalSection();
  sw(MEMCARD_EVENTS + 0x00, OpenEvent(0xF4000001, 0x0004, 0x1000, 0x80067DD0));
  sw(MEMCARD_EVENTS + 0x04, OpenEvent(0xF4000001, 0x8000, 0x1000, 0x80067DE4));
  sw(MEMCARD_EVENTS + 0x08, OpenEvent(0xF4000001, 0x0100, 0x1000, 0x80067DF8));
  sw(MEMCARD_EVENTS + 0x0C, OpenEvent(0xF4000001, 0x2000, 0x1000, 0x80067E0C));
  sw(MEMCARD_EVENTS + 0x10, OpenEvent(0xF0000011, 0x0004, 0x1000, 0x80067E20));
  sw(MEMCARD_EVENTS + 0x14, OpenEvent(0xF0000011, 0x8000, 0x1000, 0x80067E34));
  sw(MEMCARD_EVENTS + 0x18, OpenEvent(0xF0000011, 0x0100, 0x1000, 0x80067E48));
  sw(MEMCARD_EVENTS + 0x1C, OpenEvent(0xF0000011, 0x2000, 0x1000, 0x80067E5C));
  EnableEvent(lw(MEMCARD_EVENTS + 0x00));
  EnableEvent(lw(MEMCARD_EVENTS + 0x04));
  EnableEvent(lw(MEMCARD_EVENTS + 0x08));
  EnableEvent(lw(MEMCARD_EVENTS + 0x0C));
  EnableEvent(lw(MEMCARD_EVENTS + 0x10));
  EnableEvent(lw(MEMCARD_EVENTS + 0x14));
  EnableEvent(lw(MEMCARD_EVENTS + 0x18));
  EnableEvent(lw(MEMCARD_EVENTS + 0x1C));

  function_8006815C();
  
  if (crit == 1) ExitCriticalSection();
}

// size: 0x000001E4
void function_80067EA0(void)
{
  DEPRECATED;
  MemCardStart();
}

// size: 0x000000B8
void MemCardStop(void)
{
  uint32_t crit = EnterCriticalSection();
  CloseEvent(lw(MEMCARD_EVENTS));
  CloseEvent(lw(0x80075B10));
  CloseEvent(lw(0x80075B14));
  CloseEvent(lw(0x80075B18));
  CloseEvent(lw(0x80075B1C));
  CloseEvent(lw(0x80075B20));
  CloseEvent(lw(0x80075B24));
  CloseEvent(lw(0x80075B28));
  if (crit == 1) ExitCriticalSection(); 
}

// size: 0x000000B8
void function_800680A4(void)
{
  DEPRECATED;
  MemCardStop();
}

// size: 0x00000014
void function_80068F30(void)
{
  sw(0x800751B0, -1); // &0xFFFFFFFF
}

// size: 0x00000060
void function_800662BC(void)
{
  function_80068F30();
  sw(0x80075B50, 0);
  sw(0x80075B54, 0);
  sw(0x80075B58, 0);
  sw(0x80075B60, -1);
  MemCardStart();
  a1 = 0x80067CD4;
  a0 = 7;
  function_8005DE8C();
}

// size: 0x00000040
void function_8006631C(void)
{
  while (lw(0x80075B50));
  a0 = 7;
  a1 = 0;
  function_8005DE8C();
  MemCardStop();
}

// size: 0x00000080
void function_80068F44(void)
{
  uint32_t temp;
  v0 = lw(0x800751B0); // &0xFFFFFFFF
  sp -= 24; // 0xFFFFFFE8
  a2 = v0 + 1; // 0x0001
  v0 = (int32_t)a2 < 4;
  temp = v0 != 0;
  sw(sp + 0x0010, ra);
  if (temp) goto label80068F78;
  printf("libmcrd: event overflow\n");
  goto label80068FB4;
label80068F78:
  a1 = 3; // 0x0003
  v0 = a2 << 4;
  v1 = 0x80075C14;
  v1 += v0;
  v0 = a2 << 2;
  sw(0x800751B0, a2); // &0xFFFFFFFF
  sw(0x80075C48 + v0, a0);
label80068FA4:
  sw(v1, 0);
  a1--;
  temp = (int32_t)a1 >= 0;
  v1 -= 4; // 0xFFFFFFFC
  if (temp) goto label80068FA4;
label80068FB4:
  ra = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x0000007C
void function_8006635C(void)
{
  v1 = 0x80075B50;
  v0 = lw(v1);
  if (lw(v1)) {
    printf("Access Denied. : event multipul open\n");
    v0 = 0;
    return;
  }
  a1 = a0;
  a0 = 0x800663D8; // &0x27BDFFE0
  sw(v1, 1);
  sw(0x80075B54, 0);
  sw(0x80075B58, 0);
  sw(0x80075B5C, a1);
  function_80068F44();
  v0 = 1;
  return;
}

// size: 0x00000058
void function_80067C7C(void)
{
  uint32_t temp;
  v0 = 1; // 0x0001
  temp = a0 == v0;
  v1 = 0;
  if (temp) goto label80067CC8;
  v0 = (int32_t)a0 < 2;
  temp = v0 == 0;
  if (temp) goto label80067CA4;
  temp = a0 == 0;
  if (temp) goto label80067CCC;
  v1 = a0 | 0x8000;
  goto label80067CCC;
label80067CA4:
  v0 = 2; // 0x0002
  temp = a0 == v0;
  v0 = 4; // 0x0004
  if (temp) goto label80067CC0;
  temp = a0 == v0;
  v1 = 3; // 0x0003
  if (temp) goto label80067CCC;
  v1 = a0 | 0x8000;
  goto label80067CCC;
label80067CC0:
  v1 = 1; // 0x0001
  goto label80067CCC;
label80067CC8:
  v1 = 2; // 0x0002
label80067CCC:
  v0 = v1;
  return;
}

// size: 0x000001E0
void function_800663D8(void)
{
  uint32_t temp;
  sp -= 32; // 0xFFFFFFE0
  sw(sp + 0x0010, s0);
  s0 = a0;
  sw(sp + 0x0018, ra);
  sw(sp + 0x0014, s1);
  v1 = lw(s0);
  temp = v1 == 0;
  if (temp) goto label8006641C;
  temp = (int32_t)v1 < 0;
  s1 = 10; // 0x000A
  if (temp) goto label8006659C;
  temp = v1 == s1;
  v0 = 11; // 0x000B
  if (temp) goto label8006643C;
  temp = v1 == v0;
  v0 = 0;
  if (temp) goto label80066460;
  goto label800665A0;
label8006641C:
  sw(0x80075AFC, 0);
  sw(0x80075AF8, 0);
  function_8006815C();
  v0 = 10; // 0x000A
  sw(s0, v0);
label8006643C:
  a0 = lw(0x80075B5C);
  v0 = _card_info(a0);
  v0 = lw(s0);
  v0++;
  sw(s0, v0);
  goto label8006659C;
label80066460:
  function_8006841C();
  temp = v0 == 0;
  v0 = 0;
  if (temp) goto label800665A0;
  function_80068264();
  v1 = v0;
  sw(0x80075AFC, v0);
  a1 = 1; // 0x0001
  temp = v1 == a1;
  v0 = (int32_t)v1 < 2;
  if (temp) goto label80066524;
  temp = v0 == 0;
  if (temp) goto label800664A8;
  temp = v1 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label800664D4;
  goto label80066550;
label800664A8:
  v0 = 4; // 0x0004
  temp = v1 != v0;
  v0 = 1; // 0x0001
  if (temp) goto label80066550;
  a0 = 4; // 0x0004
  v0 = lw(0x80075B5C);
  v1 = lw(0x80075B4C);
  v0 = a1 << v0;
  v1 = v1 | v0;
  goto label80066574;
label800664D4:
  v1 = lw(0x80075B5C);
  v0 = lw(0x80075B4C);
  a0 = a1 << v1;
  v0 = v0 & a0;
  temp = v0 != 0;
  if (temp) goto label80066514;
  v0 = lw(0x80075B4C);
  v1 = 4; // 0x0004
  sw(0x80075AFC, v1);
  v0 = v0 | a0;
  sw(0x80075B4C, v0);
label80066514:
  a0 = lw(0x80075AFC);
  goto label8006657C;
label80066524:
  v0 = lw(0x80075AF8);
  v0++;
  sw(0x80075AF8, v0);
  v0 = (int32_t)v0 < 16;
  temp = v0 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label80066550;
  sw(s0, s1);
  goto label8006659C;
label80066550:
  v1 = lw(0x80075B5C);
  a0 = lw(0x80075AFC);
  v0 = v0 << v1;
  v1 = lw(0x80075B4C);
  v0 = ~v0;
  v1 = v1 & v0;
label80066574:
  sw(0x80075B4C, v1);
label8006657C:
  function_80067C7C();
  v1 = v0;
  v0 = 1; // 0x0001
  sw(0x80075B54, v1);
  goto label800665A0;
label8006659C:
  v0 = 0;
label800665A0:
  ra = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 32; // 0x0020
  return;
}

// size: 0x0000007C
void function_800665B8(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  v1 = 0x80075B50;
  sw(sp + 0x0010, ra);
  v0 = lw(v1);
  temp = v0 != 0;
  a1 = a0;
  if (temp) goto label80066610;
  a0 = 0x80066634; // &0x27BDFFE8
  v0 = 2; // 0x0002
  sw(v1, v0);
  sw(0x80075B54, 0);
  sw(0x80075B58, 0);
  sw(0x80075B5C, a1);
  function_80068F44();
  v0 = 1; // 0x0001
  goto label80066624;
label80066610:
  printf("Access Denied. : event multipul open\n");
  v0 = 0;
label80066624:
  ra = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x00000038
void function_80068920(void)
{
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, s0);
  sw(sp + 0x0014, ra);
  s0 = a0;
  _new_card();
  _card_write(s0, 0x3F, 0);
  ra = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x0000020C
void function_80066634(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, s0);
  s0 = a0;
  sw(sp + 0x0014, ra);
  v1 = lw(s0);
  v0 = v1 < 32;
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label80066828;
  v0 = lw(0x80011F7C + v0); // &0x80066670
  temp = v0;
  switch (temp)
  {
  case 0x80066670: // 0
    goto label80066670;
    break;
  case 0x80066828: // 1 2 3 4 5 6 7 8 9 11 12 13 14 15 16 17 18 19 20 22 23 24 25 26 27 28 29
    goto label80066828;
    break;
  case 0x800666A4: // 10
    goto label800666A4;
    break;
  case 0x800666FC: // 21
    goto label800666FC;
    break;
  case 0x8006671C: // 30
    goto label8006671C;
    break;
  case 0x80066748: // 31
    goto label80066748;
    break;
  default:
    JR(temp, 0x80066668);
    return;
  }
label80066670:
  a0 = 0x800663D8; // &0x27BDFFE0
  sw(0x80075B08, 0);
  sw(0x80075B04, 0);
  sw(0x80075B00, 0);
  function_80068F44();
  v0 = 10; // 0x000A
  sw(s0, v0);
  goto label80066828;
label800666A4:
  v0 = 0x80075B54;
  v1 = lw(v0);
  temp = v1 == 0;
  v0 = 3; // 0x0003
  if (temp) goto label800666F0;
  temp = v1 != v0;
  v0 = 1; // 0x0001
  if (temp) goto label8006682C;
  sw(0x80075B08, v0);
  function_8006815C();
  a0 = lw(0x80075B5C);
  function_80068920();
  v0 = 21; // 0x0015
  sw(s0, v0);
  goto label80066828;
label800666F0:
  v0 = 30; // 0x001E
  sw(s0, v0);
  goto label80066828;
label800666FC:
  function_80068458();
  temp = v0 == 0;
  v0 = 0;
  if (temp) goto label8006682C;
  function_80068340();
  v0 = 30; // 0x001E
  sw(s0, v0);
label8006671C:
  function_8006815C();
  a0 = lw(0x80075B5C);
  v0 = _card_load(a0);
  v0 = lw(s0);
  v0++;
  sw(s0, v0);
  goto label80066828;
label80066748:
  function_8006841C();
  temp = v0 == 0;
  v0 = 0;
  if (temp) goto label8006682C;
  function_80068264();
  v1 = v0;
  sw(0x80075B04, v0);
  v0 = 1; // 0x0001
  temp = v1 == v0;
  v0 = (int32_t)v1 < 2;
  if (temp) goto label800667D4;
  temp = v0 == 0;
  if (temp) goto label80066790;
  temp = v1 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label800667AC;
  goto label80066800;
label80066790:
  v0 = 4; // 0x0004
  temp = v1 != v0;
  if (temp) goto label80066800;
  sw(0x80075B54, v1);
  v0 = 1; // 0x0001
  goto label8006682C;
label800667AC:
  v1 = lw(0x80075B08);
  v1 = 0 < v1;
  v1 = -v1;
  v1 = v1 & 0x3;
  sw(0x80075B54, v1);
  goto label8006682C;
label800667D4:
  v0 = lw(0x80075B00);
  v0++;
  sw(0x80075B00, v0);
  v0 = (int32_t)v0 < 16;
  temp = v0 == 0;
  if (temp) goto label80066800;
  sw(s0, 0);
  goto label80066828;
label80066800:
  a0 = lw(0x80075B04);
  function_80067C7C();
  v1 = v0;
  v0 = 1; // 0x0001
  sw(0x80075B54, v1);
  goto label8006682C;
label80066828:
  v0 = 0;
label8006682C:
  ra = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x000000F0
uint32_t func_80067628(uint32_t async, uint32_t a1, uint32_t a2)
{
  assert(a1 == 0x80078D78 + 0x40);
  assert(a2 == 0x80078D78 + 0x44);

  if (lw(0x80075B50) == 0 && lw(0x80075B58) == 0) return -1;
  
  if (!async) while (lw(0x80075B58) == 0) inter(0);
  
  if (lw(0x80075B58) || !async) {
    if (a2) sw(a2, lw(0x80075B98));
    if (a1) sw(a1, lw(0x80075B94));
    sw(0x80075B58, 0);
    return 1;
  } else {
    if (a2) sw(a2, lw(0x80075B54));
    if (a1) sw(a1, lw(0x80075B50));
    return 0;
  }
}

// size: 0x000000F0
void function_80067628(void)
{
  v0 = func_80067628(a0, a1, a2);
}

// size: 0x0000010C
void function_80066E28(void)
{
  sp -= 48; // 0xFFFFFFD0
  sw(sp + 0x1C, s3);
  sw(sp + 0x18, s2);
  sw(sp + 0x24, s5);
  sw(sp + 0x20, s4);
  sw(sp + 0x14, s1);

  s3 = a0;
  s2 = a1;
  s5 = a2;
  s4 = a3;

  s1 = 0x80075B50;
  sw(sp + 0x2C, ra);
  sw(sp + 0x28, s6);
  sw(sp + 0x10, s0);
  s6 = lw(sp + 0x40);
  if (lw(0x80075B50)) {
    printf("Access Denied. : system busy\n");
    goto label80066EFC;
  }
  if (s6 & 0x7F) {
    printf("Access Denied. : invalid data size\n");
    goto label80066EFC;
  }
  s0 = 0x80075B50 + 0x20;
  setup_bu_prefix(s3, s0);
  spyro_strcat(s0, s2);
  a0 = 0x80066F34; // &0x27BDFFE0
  sw(0x80075B50, 3);
  sw(0x80075B54, 0);
  sw(0x80075B58, 0);
  sw(0x80075B64, s4);
  sw(0x80075B6C, s5);
  sw(0x80075B68, s6);
  sw(0x80075B5C, s3);
  function_80068F44();
  v0 = 1;
  goto label80066F08;
label80066EFC:
  v0 = 0;
label80066F08:
  ra = lw(sp + 0x002C);
  s6 = lw(sp + 0x0028);
  s5 = lw(sp + 0x0024);
  s4 = lw(sp + 0x0020);
  s3 = lw(sp + 0x001C);
  s2 = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 48; // 0x0030
  return;
}

// size: 0x000001B0
void function_80066F34(void)
{
  static int counter1 = 0;

  uint32_t temp;
  sp -= 32; // 0xFFFFFFE0
  sw(sp + 0x14, s1);
  sw(sp + 0x18, ra);
  sw(sp + 0x10, s0);

  s1 = a0;
  
  switch (lw(s1))
  {
  case 0:
    a0 = 0x800663D8; // &0x27BDFFE0
    counter1 = 0;
    function_80068F44();
    v0 = 10; // 0x000A
    sw(s1, v0);
    goto label800670C8;
  case 10:
    s0 = 0x80075B54;
    v0 = 1;
    if (lw(s0)) goto label800670CC;
    v0 = psx_open(addr_to_pointer(s0 + 0x1C), 0x8001);
    sw(0x80075B60, v0);
    if ((int32_t)v0 < 0) {
      sw(s0, 5);
      v0 = 1;
      goto label800670CC;
    }
#ifdef __GNUC__
    [[fallthrough]];
#endif
  case 11:
    s0 = 0x80075B60;
    while (psx_lseek(lw(s0 + 0x00), lw(s0 + 0x04), 0) != lw(s0 + 0x04));
    sw(s1, 20);
    goto label800670C8;
  case 20:
    function_8006815C();
    s0 = 0x80075B60;
    while (psx_read(lw(s0 + 0x00), addr_to_pointer(lw(s0 + 0x0C)), lw(s0 + 0x08)));
    sw(s1, 30);
    goto label800670C8;
  case 30:
    function_8006841C();
    temp = v0 == 0;
    v0 = 0;
    if (temp) goto label800670CC;
    function_80068264();
    a0 = v0;
    temp = a0 != 1;
    if (temp) goto label80067094;
    counter1++;
    temp = v0 == 0;
    if (counter1 < 16) {
      sw(s1, 11);
      goto label800670C8;
    }
  label80067094:
    function_80067C7C();
    a0 = lw(0x80075B60);
    sw(0x80075B54, v0);
    v0 = psx_close(a0);
    v1 = -1;
    sw(0x80075B60, v1);
    v0 = 1; // 0x0001
    goto label800670CC;
  default:
    goto label800670C8;
  }
label800670C8:
  v0 = 0;
label800670CC:
  ra = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 32; // 0x0020
  return;
}

// size: 0x0000010C
void function_800670E4(void)
{
  uint32_t temp;
  sp -= 48; // 0xFFFFFFD0
  sw(sp + 0x001C, s3);
  s3 = a0;
  sw(sp + 0x0018, s2);
  s2 = a1;
  sw(sp + 0x0024, s5);
  s5 = a2;
  sw(sp + 0x0020, s4);
  s4 = a3;
  sw(sp + 0x0014, s1);
  s1 = 0x80075B50;
  sw(sp + 0x002C, ra);
  sw(sp + 0x0028, s6);
  sw(sp + 0x0010, s0);
  v0 = lw(s1);
  s6 = lw(sp + 0x40);
  temp = v0 == 0;
  v0 = s6 & 0x7F;
  if (temp) goto label80067140;
  printf("Access Denied. : system busy\n");
  goto label800671B8;
label80067140:
  temp = v0 != 0;
  a0 = s3;
  if (temp) goto label800671B0;
  s0 = s1 + 32; // 0x0020
  a1 = s0;
  setup_bu_prefix(a0, a1);
  spyro_strcat(s0, s2);
  a0 = 0x800671F0; // &0x27BDFFE0
  sw(s1, 4);
  sw(0x80075B54, 0);
  sw(0x80075B58, 0);
  sw(0x80075B64, s4);
  sw(0x80075B6C, s5);
  sw(0x80075B68, s6);
  sw(0x80075B5C, s3);
  function_80068F44();
  v0 = 1; // 0x0001
  goto label800671C4;
label800671B0:
  printf("Access Denied. : invalid data size\n");
label800671B8:
  v0 = 0;
label800671C4:
  ra = lw(sp + 0x002C);
  s6 = lw(sp + 0x0028);
  s5 = lw(sp + 0x0024);
  s4 = lw(sp + 0x0020);
  s3 = lw(sp + 0x001C);
  s2 = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 48; // 0x0030
  return;
}

// size: 0x000001B0
void function_800671F0(void)
{
  uint32_t temp;
  sp -= 32; // 0xFFFFFFE0
  sw(sp + 0x0014, s1);
  s1 = a0;
  sw(sp + 0x0018, ra);
  sw(sp + 0x0010, s0);
  v1 = lw(s1);
  v0 = v1 < 31;
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label80067384;
  v0 = lw(0x80012144 + v0); // &0x80067230
  temp = v0;
  switch (temp)
  {
  case 0x80067230: // 0
    goto label80067230;
    break;
  case 0x80067384: // 1 2 3 4 5 6 7 8 9 12 13 14 15 16 17 18 19 21 22 23 24 25 26 27 28 29
    goto label80067384;
    break;
  case 0x80067254: // 10
    goto label80067254;
    break;
  case 0x80067294: // 11
    goto label80067294;
    break;
  case 0x800672C4: // 20
    goto label800672C4;
    break;
  case 0x800672F8: // 30
    goto label800672F8;
    break;
  default:
    JR(temp, 0x80067228);
    return;
  }
label80067230:
  a0 = 0x800663D8; // &0x27BDFFE0
  sw(0x80075194, 0);
  function_80068F44();
  v0 = 10; // 0x000A
  sw(s1, v0);
  goto label80067384;
label80067254:
  s0 = 0x80075B54;
  v0 = lw(s0);
  temp = v0 != 0;
  v0 = 1; // 0x0001
  if (temp) goto label80067388;
  a0 = s0 + 28; // 0x001C
  a1 = 0 | 0x8001;
  v0 = psx_open(addr_to_pointer(a0), a1);
  sw(0x80075B60, v0);
  temp = (int32_t)v0 >= 0;
  v0 = 5; // 0x0005
  if (temp) goto label80067294;
  sw(s0, v0);
  v0 = 1; // 0x0001
  goto label80067388;
label80067294:
  s0 = 0x80075B60;
label8006729C:
  a0 = lw(s0);
  a1 = lw(s0 + 0x0004);
  a2 = 0;
  v0 = psx_lseek(a0, a1, a2);
  v1 = lw(s0 + 0x0004);
  temp = v0 != v1;
  v0 = 20; // 0x0014
  if (temp) goto label8006729C;
  sw(s1, v0);
  goto label80067384;
label800672C4:
  function_8006815C();
  s0 = 0x80075B60;
label800672D4:
  a0 = lw(s0);
  a1 = lw(s0 + 0x000C);
  a2 = lw(s0 + 0x0008);
  v0 = psx_write(a0, addr_to_pointer(a1), a2);
  temp = v0 != 0;
  v0 = 30; // 0x001E
  if (temp) goto label800672D4;
  sw(s1, v0);
  goto label80067384;
label800672F8:
  function_8006841C();
  temp = v0 == 0;
  v0 = 0;
  if (temp) goto label80067388;
  function_80068264();
  a0 = v0;
  v0 = 1; // 0x0001
  temp = a0 != v0;
  if (temp) goto label80067350;
  v0 = lw(0x80075194);
  v0++;
  sw(0x80075194, v0);
  v0 = (int32_t)v0 < 16;
  temp = v0 == 0;
  if (temp) goto label80067350;
  v0 = 11; // 0x000B
  sw(s1, v0);
  goto label80067384;
label80067350:
  function_80067C7C();
  a0 = lw(0x80075B60);
  sw(0x80075B54, v0);
  v0 = psx_close(a0);
  v1 = -1;
  sw(0x80075B60, v1);
  v0 = 1; // 0x0001
  goto label80067388;
label80067384:
  v0 = 0;
label80067388:
  ra = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 32; // 0x0020
  return;
}

// size: 0x00000230
void function_80067718(void)
{
  uint32_t temp;
  sp -= 80; // 0xFFFFFFB0
  sw(sp + 0x40, s2);
  sw(sp + 0x38, s0);
  sw(sp + 0x48, s4);
  sw(sp + 0x44, s3);
  sw(sp + 0x4C, ra);
  sw(sp + 0x3C, s1);

  s2 = a0;
  s0 = a1;
  s4 = a2;
  s3 = 0x80075B50;
  s1 = 0;
  if (lw(s3)) {
    printf("Access Denied. : system busy\n");
    v0 = -1;
    goto label80067924;
  }
  a0 = s2;
  a1 = sp + 0x10;
  setup_bu_prefix(a0, a1);
  spyro_strcat(sp + 0x10, s0);
  sw(0x80075B4C, lw(0x80075B4C) | (1 << lw(0x80075B5C)));
  v0 = psx_open(addr_to_pointer(sp + 0x10), 1);
  s0 = s3;
  if ((int32_t)v0 < 0) goto label800677DC;
  v0 = psx_close(v0);
  v0 = 6;
  goto label80067924;
label800677DC:
  s3 = 2;
label800677E8:
  a1 = s4 << 16;
  a1 = a1 | 0x200;
  v0 = psx_open(addr_to_pointer(sp + 0x10), a1 | 0x200);
  temp = (int32_t)v0 >= 0;
  if (temp) {
    psx_close(v0);
    v0 = 0;
    goto label80067924;
  }
  a0 = 0;
  function_80067614();
  v1 = lw(s0);
  sw(0x80075BA0, v0);
  temp = v1 == 0;
  if (temp) goto label8006782C;
  printf("Access Denied. : event multipul open\n");
  goto label80067858;
label8006782C:
  a0 = 0x80066634; // &0x27BDFFE8
  sw(s0, s3);
  sw(0x80075B54, 0);
  sw(0x80075B58, 0);
  sw(0x80075B5C, s2);
  function_80068F44();
label80067858:
  v0 = lw(s0);
  temp = v0 != 0;
  if (temp) goto label80067878;
  v0 = lw(s0 + 8);
  temp = v0 == 0;
  if (temp) goto label800678C0;
label80067878:
  v0 = lw(s0 + 0);
  v0 = lw(s0 + 4);
  v0 = lw(s0 + 8);
  temp = v0 != 0;
  if (temp) goto label800678A8;
  v1 = 0x80075B58;
label80067898:
  v0 = lw(v1);
  temp = v0 == 0;
  if (temp) goto label80067898;
label800678A8:
  v0 = 0x80075B98;
  v0 = lw(v0);
  sw(0x80075B58, 0);
  sw(sp + 0x0030, v0);
label800678C0:
  a0 = lw(0x80075BA0);
  function_80067614();
  v1 = lw(sp + 0x0030);
  temp = v1 == 0;
  v0 = 3; // 0x0003
  if (temp) {
    v0 = 7; // 0x0007
    goto label80067924;
  }
  temp = v1 == v0;
  if (temp) goto label800677E8;
  temp = v1 != s3;
  if (temp) goto label80067900;
  s1++;
  v0 = (int32_t)s1 < 16;
  temp = v0 != 0;
  if (temp) goto label800677E8;
label80067900:
  temp = v1 != 0;
  v0 = 5; // 0x0005
  if (temp) goto label8006790C;
  sw(sp + 0x0030, v0);
label8006790C:
  v0 = lw(sp + 0x0030);
  goto label80067924;
label80067924:
  ra = lw(sp + 0x4C);
  s4 = lw(sp + 0x48);
  s3 = lw(sp + 0x44);
  s2 = lw(sp + 0x40);
  s1 = lw(sp + 0x3C);
  s0 = lw(sp + 0x38);
  sp += 80; // 0x0050
  return;
}

// size: 0x00000098
void function_80067B28(void)
{
  uint32_t temp;
  sp -= 88; // 0xFFFFFFA8
  v0 = 0x80075B50;
  sw(sp + 0x0050, ra);
  v0 = lw(v0);
  temp = v0 != 0;
  v0 = 1; // 0x0001
  if (temp) goto label80067B9C;
  a1 = lw(0x80075B5C);
  v1 = lw(0x80075B4C);
  v0 = v0 << a1;
  v1 = v1 | v0;
  a1 = sp + 16; // 0x0010
  sw(0x80075B4C, v1);
  setup_bu_prefix(a0, a1);
  function_8006815C();
  a0 = sp + 16; // 0x0010
  v0 = format(a0);
  function_80068340();
  a0 = v0;
  function_80067C7C();
  goto label80067BB0;
label80067B9C:
  printf("Access Denied. : system busy\n");
  v0 = -1;
label80067BB0:
  ra = lw(sp + 0x0050);
  sp += 88; // 0x0058
  return;
}

// size: 0x0000006C
void function_80068FC4(void)
{
  uint32_t temp;
  v1 = lw(0x800751B0); // &0xFFFFFFFF
  sp -= 24; // 0xFFFFFFE8
  temp = (int32_t)v1 < 0;
  sw(sp + 0x0010, ra);
  if (temp) goto label80069020;
  v0 = v1 << 2;
  v1 = v1 << 4;
  a0 = 0x80075C08;
  v0 = lw(0x80075C48 + v0);
  temp = v0;
  a0 += v1;
  switch (temp)
  {
  case 0x80066634:
    function_80066634();
    break;
  case 0x800663D8:
    function_800663D8();
    break;
  case 0x80066F34:
    function_80066F34();
    break;
  case 0x800671F0:
    function_800671F0();
    break;
  default:
    JALR(temp, 0x80068FF8);
  }
  temp = v0 == 0;
  if (temp) goto label80069020;
  v0 = lw(0x800751B0); // &0xFFFFFFFF
  v0--;
  sw(0x800751B0, v0); // &0xFFFFFFFF
label80069020:
  ra = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x000000A0
void function_80067CD4(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, ra);
  function_80069030();
  temp = v0 != 0;
  if (temp) goto label80067D64;
  function_80068FC4();
  function_80069030();
  temp = v0 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label80067D64;
  v1 = 0x80075B50;
  sw(0x80075B58, v0);
  v0 = lw(v1);
  a0 = 0x80075B94;
  sw(a0, v0);
  v0 = lw(0x80075B54);
  a2 = lw(0x80075B90);
  sw(0x80075B98, v0);
  sw(v1, 0);
  sw(0x80075B54, 0);
  temp = a2 == 0;
  if (temp) goto label80067D64;
  a0 = lw(a0);
  a1 = lw(0x80075B98);
  temp = a2;
  switch (temp)
  {
  default:
    JALR(temp, 0x80067D5C);
  }
label80067D64:
  ra = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

uint32_t savegame_checksum(struct savegame *savegame)
{
  uint8_t *ptr = (void*)savegame;
  uint32_t sum = 0;
  for (int i = 0; i < 0x58C; i++)
    sum += ptr[i];
  return sum;
}

// size: 0x00000028
void function_8005956C(void)
{
  DEPRECATED;
  v0 = savegame_checksum(addr_to_pointer(a0));
}

void create_savedata(struct savegame *save)
{
  memset(save, 0, sizeof(*save));

  save->level_id = lw(LEVEL_ID);
  save->unknown1 = lw(0x80075754);
  save->unknown2 = lw(0x80075748);
  save->unknown3 = lw(0x800757A4);
  save->camera_mode = lw(CAMERA_MODE);
  save->unknown8 = lw(0x80076240);
  save->unknown9 = lw(0x80075838);
  save->unknownA = lw(0x8007583C);
  save->lives = lw(LIVES);

  memcpy(&save->unknown10, addr_to_pointer(0x80078618), 0x14);
  memcpy(&save->unknown24, addr_to_pointer(0x80078680), 5*5);

  memcpy(&save->unknown40, addr_to_pointer(0x80078E78), 0x24);
  memcpy(&save->unknown64, addr_to_pointer(0x8007A6A8), 0x24);

  for (int i = 0; i < 36; i++) {
    save->level_dragons[i] = lw(LEVEL_DRAGONS + i*4);
    save->level_gems[i] = lw(LEVEL_GEMS + i*4);
  }

  for (int i = 0; i < 18; i++) {
    save->level_eggs[i] = lw(LEVEL_EGGS + i*4);
  }

  memcpy(&save->unknown106, addr_to_pointer(0x800758D0), 6);
  memcpy(&save->unknown10C, addr_to_pointer(0x80077908), 0x480);
  save->checksum = savegame_checksum(save);
}

// size: 0x000001E4
void function_80059864(void)
{
  DEPRECATED;
  create_savedata(addr_to_pointer(a0));
}

// size: 0x000002D0
uint32_t load_savedata(struct savegame *save)
{
  sw(LEVEL_ID, save->level_id);
  a3 = save->unknown1;
  v0 = a3 << 14;
  v0 -= a3;
  mult(v0, 0x66666667);
  v0 = (int32_t)v0 >> 31;
  t4=hi;
  a1 = (int32_t)t4 >> 2;
  a1 -= v0;
  v1 = a1 << 12;
  mult(v1, 0x80020009);
  sw(0x80075754, a3);
  sw(0x80076228, a1);
  t4=hi;
  v0 = t4 + v1;
  v0 = (int32_t)v0 >> 13;
  v1 = (int32_t)v1 >> 31;
  v0 -= v1;
  sw(0x8007622C, v0);
  v0 = save->unknown2;
  sw(0x80075F08, 0xC0);
  sw(0x80075748, v0);
  v0 = v0 << 11;
  sw(0x80076224, v0);
  sh(0x80075F18, v0);
  sh(0x80075F1A, v0);
  a0 = 0x80075F08;
  function_8005CC58();

  sw(0x800757A4, save->unknown3);
  sw(CAMERA_MODE, save->camera_mode);
  sw(0x80076240, save->unknown8);
  sw(0x80075838, save->unknown9);
  sw(0x8007583C, save->unknownA);
  sw(LIVES, max_int(save->lives, 4));

  memcpy(addr_to_pointer(0x80078618), &save->unknown10, 0x14);
  memcpy(addr_to_pointer(0x80078680), &save->unknown24, 5*5);

  memcpy(addr_to_pointer(0x80078E78), &save->unknown40, 0x24);
  memcpy(addr_to_pointer(0x8007A6A8), &save->unknown64, 0x24);

  uint32_t total_found_dragons = 0;
  uint32_t total_found_gems = 0;

  for (int i = 0; i < 36; i++)
  {
    uint32_t level_dragons = save->level_dragons[i];
    uint32_t level_gems = save->level_gems[i];

    total_found_dragons += level_dragons;
    total_found_gems += level_gems;

    sw(LEVEL_DRAGONS + i*4, level_dragons);
    sw(LEVEL_GEMS + i*4, level_gems);
  }

  sw(TOTAL_FOUND_DRAGONS, total_found_dragons);
  sw(TOTAL_FOUND_GEMS, total_found_gems);

  uint32_t total_found_eggs = 0;

  for (int i = 0; i < 18; i++)
  {
    uint32_t level_eggs = save->level_eggs[i];
    total_found_eggs += level_eggs;
    sw(LEVEL_EGGS + i*4, level_eggs);
  }

  sw(TOTAL_FOUND_EGGS, total_found_eggs);

  memcpy(addr_to_pointer(0x800758D0), &save->unknown106, 6);
  memcpy(addr_to_pointer(0x80077908), &save->unknown10C, 0x480);

  v0 = save->checksum == savegame_checksum(save);

  assert(v0);

  return v0;
}

// size: 0x000002D0
void function_80059594(void)
{
  v0 = load_savedata(addr_to_pointer(a0));
}