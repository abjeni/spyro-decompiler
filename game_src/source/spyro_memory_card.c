#include "main.h"
#include "psx_bios.h"
#include "psx_mem.h"
#include "decompilation.h"
#include "spyro_string.h"
#include "psx_ops.h"
#include "not_renamed.h"
#include "level_loading.h"
#include "spyro_math.h"

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
  UNREACHABLE;
  v0 = init_memory_card();
}



// size: 0x0000005C
void function_80067D74(void)
{
  uint32_t temp;
  a2 = a0;
  a3 = a1;
  a1 = 0x800121C0; // "bu00:"
  v0 = lw(a1 + 0x00);
  v1 = lb(a1 + 0x04);
  a0 = lb(a1 + 0x05);
  sw(a3 + 0x00, v0);
  sb(a3 + 0x04, v1);
  sb(a3 + 0x05, a0);
  temp = (int32_t)a2 >= 0;
  v0 = a2;
  if (temp) goto label80067DB0;
  v0 = a2 + 15; // 0x000F
label80067DB0:
  v0 = (int32_t)v0 >> 4;
  v1 = v0 + 48; // 0x0030
  v0 = v0 << 4;
  v0 = a2 - v0;
  v0 += 48; // 0x0030
  sb(a3 + 0x0002, v1);
  sb(a3 + 0x0003, v0);
}

// size: 0x00000108
void function_8006815C(void)
{
  TestEvent(lw(memcard_events + 0x00));
  TestEvent(lw(memcard_events + 0x04));
  TestEvent(lw(memcard_events + 0x08));
  TestEvent(lw(memcard_events + 0x0C));
  TestEvent(lw(memcard_events + 0x10));
  TestEvent(lw(memcard_events + 0x14));
  TestEvent(lw(memcard_events + 0x18));
  TestEvent(lw(memcard_events + 0x1C));
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

  TestEvent(lw(memcard_events + 0x00));
  TestEvent(lw(memcard_events + 0x04));
  TestEvent(lw(memcard_events + 0x08));
  TestEvent(lw(memcard_events + 0x0C));

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
  sw(memcard_events + 0x00, OpenEvent(0xF4000001, 0x0004, 0x1000, 0x80067DD0));
  sw(memcard_events + 0x04, OpenEvent(0xF4000001, 0x8000, 0x1000, 0x80067DE4));
  sw(memcard_events + 0x08, OpenEvent(0xF4000001, 0x0100, 0x1000, 0x80067DF8));
  sw(memcard_events + 0x0C, OpenEvent(0xF4000001, 0x2000, 0x1000, 0x80067E0C));
  sw(memcard_events + 0x10, OpenEvent(0xF0000011, 0x0004, 0x1000, 0x80067E20));
  sw(memcard_events + 0x14, OpenEvent(0xF0000011, 0x8000, 0x1000, 0x80067E34));
  sw(memcard_events + 0x18, OpenEvent(0xF0000011, 0x0100, 0x1000, 0x80067E48));
  sw(memcard_events + 0x1C, OpenEvent(0xF0000011, 0x2000, 0x1000, 0x80067E5C));
  EnableEvent(lw(memcard_events + 0x00));
  EnableEvent(lw(memcard_events + 0x04));
  EnableEvent(lw(memcard_events + 0x08));
  EnableEvent(lw(memcard_events + 0x0C));
  EnableEvent(lw(memcard_events + 0x10));
  EnableEvent(lw(memcard_events + 0x14));
  EnableEvent(lw(memcard_events + 0x18));
  EnableEvent(lw(memcard_events + 0x1C));

  function_8006815C();
  
  if (crit == 1) ExitCriticalSection();
}

// size: 0x000001E4
void function_80067EA0(void)
{
  UNREACHABLE;
  MemCardStart();
}

// size: 0x000000B8
void MemCardStop(void)
{
  uint32_t crit = EnterCriticalSection();
  CloseEvent(lw(memcard_events));
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
  UNREACHABLE;
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
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, ra);
  function_80068F30();
  v0 = 0x80075B50;
  sw(v0 + 0x0000, 0);
  v0 = -1; // 0xFFFFFFFF
  sw(0x80075B54, 0);
  sw(0x80075B58, 0);
  sw(0x80075B60, v0);
  MemCardStart();
  a1 = 0x80067CD4; // &0x27BDFFE8
  a0 = 7; // 0x0007
  function_8005DE8C();
  ra = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x00000040
void function_8006631C(void)
{
  uint32_t temp;
  sp -= 24; // 0xFFFFFFE8
  v1 = 0x80075B50;
  sw(sp + 0x0010, ra);
label8006632C:
  v0 = lw(v1 + 0x0000);
  temp = v0 != 0;
  a0 = 7; // 0x0007
  if (temp) goto label8006632C;
  a1 = 0;
  function_8005DE8C();
  MemCardStop();
  ra = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
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
  sw(v1 + 0x0000, 0);
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
  v1 = lw(s0 + 0x0000);
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
  sw(s0 + 0x0000, v0);
label8006643C:
  a0 = lw(0x80075B5C);
  v0 = _card_info(a0);
  v0 = lw(s0 + 0x0000);
  v0++;
  sw(s0 + 0x0000, v0);
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
  sw(s0 + 0x0000, s1);
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
  v0 = lw(v1 + 0x0000);
  temp = v0 != 0;
  a1 = a0;
  if (temp) goto label80066610;
  a0 = 0x80066634; // &0x27BDFFE8
  v0 = 2; // 0x0002
  sw(v1 + 0x0000, v0);
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
  v1 = lw(s0 + 0x0000);
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
  sw(s0 + 0x0000, v0);
  goto label80066828;
label800666A4:
  v0 = 0x80075B54;
  v1 = lw(v0 + 0x0000);
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
  sw(s0 + 0x0000, v0);
  goto label80066828;
label800666F0:
  v0 = 30; // 0x001E
  sw(s0 + 0x0000, v0);
  goto label80066828;
label800666FC:
  function_80068458();
  temp = v0 == 0;
  v0 = 0;
  if (temp) goto label8006682C;
  function_80068340();
  v0 = 30; // 0x001E
  sw(s0 + 0x0000, v0);
label8006671C:
  function_8006815C();
  a0 = lw(0x80075B5C);
  v0 = _card_load(a0);
  v0 = lw(s0 + 0x0000);
  v0++;
  sw(s0 + 0x0000, v0);
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
  sw(s0 + 0x0000, 0);
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

// size: 0x000000A8
void function_800687D4(void)
{
  uint32_t temp;
  sp -= 32; // 0xFFFFFFE0
  sw(sp + 0x0014, s1);
  s1 = a0;
  sw(sp + 0x0018, s2);
  s2 = a1;
  sw(sp + 0x001C, ra);
  temp = s1 == 0;
  sw(sp + 0x0010, s0);
  if (temp) goto label8006885C;
  temp = s2 == 0;
  v0 = 0;
  if (temp) goto label80068860;
  a0 = s1;
  v0 = spyro_strlen(a0);
  a0 = s2;
  s0 = s1 + v0;
  v0 = spyro_strlen(a0);
  v0 += s2;
  temp = s0 == v0;
  v1 = s1;
  if (temp) goto label8006885C;
  v0 = lbu(v1 + 0x0000);
  temp = v0 == 0;
  s1 = v1 + 1; // 0x0001
  if (temp) goto label8006883C;
label8006882C:
  v0 = lbu(s1 + 0x0000);
  temp = v0 != 0;
  s1++;
  if (temp) goto label8006882C;
label8006883C:
  s1--;
label80068840:
  v0 = lbu(s2 + 0x0000);
  s2++;
  sb(s1 + 0x0000, v0);
  temp = v0 != 0;
  s1++;
  if (temp) goto label80068840;
  v0 = v1;
  goto label80068860;
label8006885C:
  v0 = 0;
label80068860:
  ra = lw(sp + 0x001C);
  s2 = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 32; // 0x0020
  return;
}

// size: 0x0000010C
void function_80066E28(void)
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
  v0 = lw(s1 + 0x0000);
  s6 = lw(sp + 0x0040);
  temp = v0 == 0;
  v0 = s6 & 0x7F;
  if (temp) goto label80066E84;
  printf("Access Denied. : system busy\n");
  goto label80066EFC;
label80066E84:
  temp = v0 != 0;
  a0 = s3;
  if (temp) goto label80066EF4;
  s0 = s1 + 32; // 0x0020
  a1 = s0;
  function_80067D74();
  a0 = s0;
  a1 = s2;
  function_800687D4();
  a0 = 0x80066F34; // &0x27BDFFE0
  v0 = 3; // 0x0003
  sw(s1 + 0x0000, v0);
  sw(0x80075B54, 0);
  sw(0x80075B58, 0);
  sw(0x80075B64, s4);
  sw(0x80075B6C, s5);
  sw(0x80075B68, s6);
  sw(0x80075B5C, s3);
  function_80068F44();
  v0 = 1; // 0x0001
  goto label80066F08;
label80066EF4:
  printf("Access Denied. : invalid data size\n");
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
  uint32_t temp;
  sp -= 32; // 0xFFFFFFE0
  sw(sp + 0x0014, s1);
  s1 = a0;
  sw(sp + 0x0018, ra);
  sw(sp + 0x0010, s0);
  v1 = lw(s1 + 0x0000);
  v0 = v1 < 31;
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label800670C8;
  v0 = lw(0x800120C4 + v0); // &0x80066F74
  temp = v0;
  switch (temp)
  {
  case 0x80066F74: // 0
    goto label80066F74;
    break;
  case 0x800670C8: // 1 2 3 4 5 6 7 8 9 12 13 14 15 16 17 18 19 21 22 23 24 25 26 27 28 29
    goto label800670C8;
    break;
  case 0x80066F98: // 10
    goto label80066F98;
    break;
  case 0x80066FD8: // 11
    goto label80066FD8;
    break;
  case 0x80067008: // 20
    goto label80067008;
    break;
  case 0x8006703C: // 30
    goto label8006703C;
    break;
  default:
    JR(temp, 0x80066F6C);
    return;
  }
label80066F74:
  a0 = 0x800663D8; // &0x27BDFFE0
  sw(0x80075190, 0); // &0x00000000
  function_80068F44();
  v0 = 10; // 0x000A
  sw(s1 + 0x0000, v0);
  goto label800670C8;
label80066F98:
  s0 = 0x80075B54;
  v0 = lw(s0 + 0x0000);
  temp = v0 != 0;
  v0 = 1; // 0x0001
  if (temp) goto label800670CC;
  a0 = s0 + 28; // 0x001C
  a1 = 0 | 0x8001;
  v0 = psx_open(addr_to_pointer(a0), a1);
  sw(0x80075B60, v0);
  temp = (int32_t)v0 >= 0;
  v0 = 5; // 0x0005
  if (temp) goto label80066FD8;
  sw(s0 + 0x0000, v0);
  v0 = 1; // 0x0001
  goto label800670CC;
label80066FD8:
  s0 = 0x80075B60;
label80066FE0:
  a0 = lw(s0 + 0x0000);
  a1 = lw(s0 + 0x0004);
  a2 = 0;
  v0 = psx_lseek(a0, a1, a2);
  v1 = lw(s0 + 0x0004);
  temp = v0 != v1;
  v0 = 20; // 0x0014
  if (temp) goto label80066FE0;
  sw(s1 + 0x0000, v0);
  goto label800670C8;
label80067008:
  function_8006815C();
  s0 = 0x80075B60;
label80067018:
  a0 = lw(s0 + 0x0000);
  a1 = lw(s0 + 0x000C);
  a2 = lw(s0 + 0x0008);
  v0 = psx_read(a0, addr_to_pointer(a1), a2);
  temp = v0 != 0;
  v0 = 30; // 0x001E
  if (temp) goto label80067018;
  sw(s1 + 0x0000, v0);
  goto label800670C8;
label8006703C:
  function_8006841C();
  temp = v0 == 0;
  v0 = 0;
  if (temp) goto label800670CC;
  function_80068264();
  a0 = v0;
  v0 = 1; // 0x0001
  temp = a0 != v0;
  if (temp) goto label80067094;
  v0 = lw(0x80075190); // &0x00000000
  v0++;
  sw(0x80075190, v0); // &0x00000000
  v0 = (int32_t)v0 < 16;
  temp = v0 == 0;
  if (temp) goto label80067094;
  v0 = 11; // 0x000B
  sw(s1 + 0x0000, v0);
  goto label800670C8;
label80067094:
  function_80067C7C();
  a0 = lw(0x80075B60);
  sw(0x80075B54, v0);
  v0 = psx_close(a0);
  v1 = -1; // 0xFFFFFFFF
  sw(0x80075B60, v1);
  v0 = 1; // 0x0001
  goto label800670CC;
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
  v0 = lw(s1 + 0x0000);
  s6 = lw(sp + 0x0040);
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
  function_80067D74();
  a0 = s0;
  a1 = s2;
  function_800687D4();
  a0 = 0x800671F0; // &0x27BDFFE0
  v0 = 4; // 0x0004
  sw(s1 + 0x0000, v0);
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
  v1 = lw(s1 + 0x0000);
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
  sw(0x80075194, 0); // &0x00000000
  function_80068F44();
  v0 = 10; // 0x000A
  sw(s1 + 0x0000, v0);
  goto label80067384;
label80067254:
  s0 = 0x80075B54;
  v0 = lw(s0 + 0x0000);
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
  sw(s0 + 0x0000, v0);
  v0 = 1; // 0x0001
  goto label80067388;
label80067294:
  s0 = 0x80075B60;
label8006729C:
  a0 = lw(s0 + 0x0000);
  a1 = lw(s0 + 0x0004);
  a2 = 0;
  v0 = psx_lseek(a0, a1, a2);
  v1 = lw(s0 + 0x0004);
  temp = v0 != v1;
  v0 = 20; // 0x0014
  if (temp) goto label8006729C;
  sw(s1 + 0x0000, v0);
  goto label80067384;
label800672C4:
  function_8006815C();
  s0 = 0x80075B60;
label800672D4:
  a0 = lw(s0 + 0x0000);
  a1 = lw(s0 + 0x000C);
  a2 = lw(s0 + 0x0008);
  v0 = psx_write(a0, addr_to_pointer(a1), a2);
  temp = v0 != 0;
  v0 = 30; // 0x001E
  if (temp) goto label800672D4;
  sw(s1 + 0x0000, v0);
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
  v0 = lw(0x80075194); // &0x00000000
  v0++;
  sw(0x80075194, v0); // &0x00000000
  v0 = (int32_t)v0 < 16;
  temp = v0 == 0;
  if (temp) goto label80067350;
  v0 = 11; // 0x000B
  sw(s1 + 0x0000, v0);
  goto label80067384;
label80067350:
  function_80067C7C();
  a0 = lw(0x80075B60);
  sw(0x80075B54, v0);
  v0 = psx_close(a0);
  v1 = -1; // 0xFFFFFFFF
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
  sw(sp + 0x0040, s2);
  s2 = a0;
  sw(sp + 0x0038, s0);
  s0 = a1;
  sw(sp + 0x0048, s4);
  s4 = a2;
  sw(sp + 0x0044, s3);
  s3 = 0x80075B50;
  sw(sp + 0x004C, ra);
  sw(sp + 0x003C, s1);
  v0 = lw(s3 + 0x0000);
  temp = v0 == 0;
  s1 = 0;
  if (temp) goto label80067770;
  printf("Access Denied. : system busy\n");
  v0 = -1; // 0xFFFFFFFF
  goto label80067924;
label80067770:
  a0 = s2;
  a1 = sp + 16; // 0x0010
  function_80067D74();
  a0 = sp + 16; // 0x0010
  a1 = s0;
  function_800687D4();
  a0 = sp + 16; // 0x0010
  a1 = 1; // 0x0001
  v0 = 1; // 0x0001
  a2 = lw(0x80075B5C);
  v1 = lw(0x80075B4C);
  v0 = v0 << a2;
  v1 = v1 | v0;
  sw(0x80075B4C, v1);
  v0 = psx_open(addr_to_pointer(a0), a1);
  temp = (int32_t)v0 < 0;
  s0 = s3;
  if (temp) goto label800677DC;
  a0 = v0;
  v0 = psx_close(a0);
  v0 = 6; // 0x0006
  goto label80067924;
label800677D4:
  v0 = 7; // 0x0007
  goto label80067924;
label800677DC:
  s3 = 2; // 0x0002
  a0 = sp + 16; // 0x0010
label800677E4:
  a1 = s4 << 16;
label800677E8:
  a1 = a1 | 0x200;
  v0 = psx_open(addr_to_pointer(a0), a1);
  temp = (int32_t)v0 >= 0;
  if (temp) goto label80067918;
  a0 = 0;
  function_80067614();
  v1 = lw(s0 + 0x0000);
  sw(0x80075BA0, v0);
  temp = v1 == 0;
  if (temp) goto label8006782C;
  printf("Access Denied. : event multipul open\n");
  goto label80067858;
label8006782C:
  a0 = 0x80066634; // &0x27BDFFE8
  sw(s0 + 0x0000, s3);
  sw(0x80075B54, 0);
  sw(0x80075B58, 0);
  sw(0x80075B5C, s2);
  function_80068F44();
label80067858:
  v0 = lw(s0 + 0x0000);
  temp = v0 != 0;
  if (temp) goto label80067878;
  v0 = lw(s0 + 0x0008);
  temp = v0 == 0;
  if (temp) goto label800678C0;
label80067878:
  v0 = lw(s0 + 0x0000);
  v0 = lw(s0 + 0x0004);
  v0 = lw(s0 + 0x0008);
  temp = v0 != 0;
  if (temp) goto label800678A8;
  v1 = 0x80075B58;
label80067898:
  v0 = lw(v1 + 0x0000);
  temp = v0 == 0;
  if (temp) goto label80067898;
label800678A8:
  v0 = 0x80075B98;
  v0 = lw(v0 + 0x0000);
  sw(0x80075B58, 0);
  sw(sp + 0x0030, v0);
label800678C0:
  a0 = lw(0x80075BA0);
  function_80067614();
  v1 = lw(sp + 0x0030);
  temp = v1 == 0;
  v0 = 3; // 0x0003
  if (temp) goto label800677D4;
  temp = v1 == v0;
  a0 = sp + 16; // 0x0010
  if (temp) goto label800677E4;
  temp = v1 != s3;
  if (temp) goto label80067900;
  s1++;
  v0 = (int32_t)s1 < 16;
  temp = v0 != 0;
  a1 = s4 << 16;
  if (temp) goto label800677E8;
label80067900:
  temp = v1 != 0;
  v0 = 5; // 0x0005
  if (temp) goto label8006790C;
  sw(sp + 0x0030, v0);
label8006790C:
  v0 = lw(sp + 0x0030);
  goto label80067924;
label80067918:
  a0 = v0;
  v0 = psx_close(a0);
  v0 = 0;
label80067924:
  ra = lw(sp + 0x004C);
  s4 = lw(sp + 0x0048);
  s3 = lw(sp + 0x0044);
  s2 = lw(sp + 0x0040);
  s1 = lw(sp + 0x003C);
  s0 = lw(sp + 0x0038);
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
  v0 = lw(v0 + 0x0000);
  temp = v0 != 0;
  v0 = 1; // 0x0001
  if (temp) goto label80067B9C;
  a1 = lw(0x80075B5C);
  v1 = lw(0x80075B4C);
  v0 = v0 << a1;
  v1 = v1 | v0;
  a1 = sp + 16; // 0x0010
  sw(0x80075B4C, v1);
  function_80067D74();
  function_8006815C();
  a0 = sp + 16; // 0x0010
  v0 = format(a0);
  function_80068340();
  a0 = v0;
  function_80067C7C();
  goto label80067BB0;
label80067B9C:
  printf("Access Denied. : system busy\n");
  v0 = -1; // 0xFFFFFFFF
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
  v0 = lw(v1 + 0x0000);
  a0 = 0x80075B94;
  sw(a0 + 0x0000, v0);
  v0 = lw(0x80075B54);
  a2 = lw(0x80075B90);
  sw(0x80075B98, v0);
  sw(v1 + 0x0000, 0);
  sw(0x80075B54, 0);
  temp = a2 == 0;
  if (temp) goto label80067D64;
  a0 = lw(a0 + 0x0000);
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



uint32_t savegame_checksum(uint8_t *savegame)
{
  uint32_t sum = 0;
  for (int i = 0; i < 0x58C; i++)
    sum += savegame[i];
  return sum;
}

// size: 0x00000028
void function_8005956C(void)
{
  UNREACHABLE;
  v0 = savegame_checksum(addr_to_pointer(a0));
}

// size: 0x000001E4
void function_80059864(void)
{
  uint32_t temp;
  sp -= 0x20; // 0xFFFFFFE0
  sw(sp + 0x10, s0);
  sw(sp + 0x18, ra);
  sw(sp + 0x14, s1);

  s0 = a0;
  s1 = s0;
  spyro_memset32(s0, 0, 0x600);
  sb(s0 + 0x00, lw(LEVEL_ID));
  sb(s0 + 0x01, lw(0x80075754));
  sb(s0 + 0x02, lw(0x80075748));
  sb(s0 + 0x03, lw(0x800757A4));
  sw(s0 + 0x04, lw(CAMERA_MODE));
  sb(s0 + 0x08, lw(0x80076240));
  sb(s0 + 0x09, lw(0x80075838));
  sb(s0 + 0x0A, lw(0x8007583C));
  sb(s0 + 0x000B, lw(0x8007582C));
  spyro_memcpy32(s0 + 0x10, 0x80078618, 0x14);
  t0 = 0;
  a3 = 0x80078680;
  a1 = s0;
label8005992C:
  v1 = a1 + 36; // 0x0024
  a0 = a3;
  a2 = a1 + 41; // 0x0029
label80059938:
  v0 = lbu(a0 + 0x0000);
  sb(v1 + 0x0000, v0);
  v1++;
  v0 = (int32_t)v1 < (int32_t)a2;
  temp = v0 != 0;
  a0++;
  if (temp) goto label80059938;
  a3 += 5; // 0x0005
  t0++;
  v0 = (int32_t)t0 < 5;
  temp = v0 != 0;
  a1 += 5; // 0x0005
  if (temp) goto label8005992C;
  a0 = s0 + 64; // 0x0040
  a1 = 0x80078E78;
  a2 = 36; // 0x0024
  spyro_memcpy32(a0, a1, a2);
  a0 = s0 + 100; // 0x0064
  a1 = 0x8007A6A8;
  a2 = 36; // 0x0024
  spyro_memcpy32(a0, a1, a2);
  a0 = 0;
  t0 = 0x80076FE8;
  a3 = 0x80077420;
  a2 = s0;
  a1 = 0x800772D8;
label800599B0:
  v0 = lw(a1 + 0x0000);
  a1 += 4; // 0x0004
  v1 = s0 + a0;
  a0++;
  sb(v1 + 0x0088, v0);
  v0 = lw(a3 + 0x0000);
  a3 += 4; // 0x0004
  sh(a2 + 0x00AC, v0);
  v0 = lw(t0 + 0x0000);
  t0 += 4; // 0x0004
  sb(v1 + 0x00F4, v0);
  v0 = (int32_t)a0 < 36;
  temp = v0 != 0;
  a2 += 2; // 0x0002
  if (temp) goto label800599B0;
  a0 = 0;
  v1 = s0 + a0;
label800599F0:
  v0 = lbu(0x800758D0 + a0);
  a0++;
  sb(v1 + 0x0106, v0);
  v0 = (int32_t)a0 < 6;
  temp = v0 != 0;
  v1 = s0 + a0;
  if (temp) goto label800599F0;
  a0 = s0 + 268; // 0x010C
  a1 = 0x80077908;
  a2 = 1152; // 0x0480
  spyro_memcpy32(a0, a1, a2);
  a0 = s1;
  sw(s0 + 0x058C, savegame_checksum(addr_to_pointer(s1)));
  ra = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 32; // 0x0020
  return;
}

// size: 0x000002D0
void function_80059594(void)
{
  uint32_t temp;
  sp -= 32; // 0xFFFFFFE0
  sw(sp + 0x0010, s0);
  s0 = a0;
  sw(sp + 0x0018, ra);
  sw(sp + 0x0014, s1);
  v0 = lbu(s0 + 0x0000);
  v1 = 0x66660000;
  sw(LEVEL_ID, v0);
  a3 = lbu(s0 + 0x0001);
  v1 = v1 | 0x6667;
  v0 = a3 << 14;
  v0 -= a3;
  mult(v0, v1);
  a0 = 0x80020009; // &0x20304207
  v0 = (int32_t)v0 >> 31;
  t4=hi;
  a1 = (int32_t)t4 >> 2;
  a1 -= v0;
  v1 = a1 << 12;
  mult(v1, a0);
  a2 = 0x80076228;
  sw(0x80075754, a3); // &0x00000000
  sw(a2 + 0x0000, a1);
  t4=hi;
  v0 = t4 + v1;
  v0 = (int32_t)v0 >> 13;
  v1 = (int32_t)v1 >> 31;
  v0 -= v1;
  sw(0x8007622C, v0);
  v0 = lbu(s0 + 0x0002);
  v1 = 192; // 0x00C0
  sw(a2 - 0x0320, v1); // 0xFFFFFCE0
  sw(0x80075748, v0); // &0x00000000
  v0 = v0 << 11;
  sw(0x80076224, v0);
  sh(0x80075F18, v0);
  sh(0x80075F1A, v0);
  a0 = a2 - 800; // 0xFFFFFCE0
  function_8005CC58();
  v0 = lbu(s0 + 0x0003);
  v1 = lw(s0 + 0x0004);
  sw(0x800757A4, v0); // &0x00000000
  sw(CAMERA_MODE, v1);
  v0 = lbu(s0 + 0x0008);
  sw(0x80076240, v0);
  v0 = lbu(s0 + 0x0009);
  sw(0x80075838, v0);
  v0 = lbu(s0 + 0x000A);
  sw(0x8007583C, v0);
  v0 = lbu(s0 + 0x000B);
  sw(0x8007582C, v0);
  v0 = (int32_t)v0 < 4;
  temp = v0 == 0;
  s1 = s0;
  if (temp) goto label800596B4;
  v0 = 4; // 0x0004
  sw(0x8007582C, v0);
label800596B4:
  a0 = 0x80078618;
  a1 = s0 + 16; // 0x0010
  a2 = 20; // 0x0014
  spyro_memcpy32(a0, a1, a2);
  t0 = 0;
  a3 = s0;
  a1 = 0x80078680;
label800596D8:
  v1 = a1;
  a0 = a3 + 36; // 0x0024
  a2 = a1 + 5; // 0x0005
label800596E4:
  v0 = lbu(a0 + 0x0000);
  sb(v1 + 0x0000, v0);
  v1++;
  v0 = (int32_t)v1 < (int32_t)a2;
  temp = v0 != 0;
  a0++;
  if (temp) goto label800596E4;
  a3 += 5; // 0x0005
  t0++;
  v0 = (int32_t)t0 < 5;
  temp = v0 != 0;
  a1 += 5; // 0x0005
  if (temp) goto label800596D8;
  a0 = 0x80078E78;
  a1 = s0 + 64; // 0x0040
  a2 = 36; // 0x0024
  spyro_memcpy32(a0, a1, a2);
  a0 = 0x8007A6A8;
  a1 = s0 + 100; // 0x0064
  a2 = 36; // 0x0024
  spyro_memcpy32(a0, a1, a2);
  a2 = 0;
  t3 = 0x80076FE8;
  t2 = 0x80077420;
  t1 = s0;
  t0 = 0x800772D8;
  sw(total_found_dragons, 0);
  sw(total_found_gems, 0);
  sw(total_found_eggs, 0);
label80059774:
  a3 = s0 + a2;
  a0 = lbu(a3 + 0x0088);
  v0 = lw(total_found_dragons);
  v1 = lw(total_found_gems);
  sw(t0 + 0x0000, a0);
  a1 = lh(t1 + 0x00AC);
  v0 += a0;
  sw(total_found_dragons, v0);
  v0 = (int32_t)a2 < 18;
  v1 += a1;
  sw(t2 + 0x0000, a1);
  sw(total_found_gems, v1);
  temp = v0 == 0;
  if (temp) goto label800597DC;
  v1 = lbu(a3 + 0x00F4);
  v0 = lw(total_found_eggs);
  v0 += v1;
  sw(t3 + 0x0000, v1);
  sw(total_found_eggs, v0);
label800597DC:
  t3 += 4; // 0x0004
  t2 += 4; // 0x0004
  t1 += 2; // 0x0002
  a2++;
  v0 = (int32_t)a2 < 36;
  temp = v0 != 0;
  t0 += 4; // 0x0004
  if (temp) goto label80059774;
  a2 = 0;
  v0 = s0 + a2;
label80059800:
  v0 = lbu(v0 + 0x0106);
  sb(0x800758D0 + a2, v0);
  a2++;
  v0 = (int32_t)a2 < 6;
  temp = v0 != 0;
  v0 = s0 + a2;
  if (temp) goto label80059800;
  spyro_memcpy32(0x80077908, s0 + 0x10C, 0x480);
  v0 = lw(s0 + 0x58C) == savegame_checksum(addr_to_pointer(s1));
  ra = lw(sp + 0x0018);
  s1 = lw(sp + 0x0014);
  s0 = lw(sp + 0x0010);
  sp += 32; // 0x0020
  return;
}


// size: 0x00001024
void function_800314B4(void)
{
  uint32_t temp;
  sp -= 72; // 0xFFFFFFB8
  sw(sp + 0x0034, s1);
  s1 = 0x80078D04;
  sw(sp + 0x0040, ra);
  sw(sp + 0x003C, s3);
  sw(sp + 0x0038, s2);
  sw(sp + 0x0030, s0);
  v0 = lw(s1 + 0x0000);
  v1 = lw(0x800756CC); // &0x00000000
  a0 = 3; // 0x0003
  v0 += v1;
  sw(s1 + 0x0000, v0);
  function_80058CC0();
  a0 = 0x8006FCF4 + 0x0400;
  v0 = lw(0x80078D74);
  a1 = 0;
  sw(0x8006FCF4 + 0x0404, 0);
  sw(a0 + 0x0000, v0);
  function_800522C0();
  v1 = lw(s1 - 0x0004); // 0xFFFFFFFC
  temp = v1 == 0;
  v0 = 1; // 0x0001
  if (temp) goto label80031534;
  temp = v1 == v0;
  if (temp) goto label80031DBC;
  goto label800324B8;
label80031534:
  a0 = lw(s1 + 0x0000);
  v0 = (int32_t)a0 < 64;
  temp = v0 == 0;
  a0 = a0 << 5;
  if (temp) goto label80031BA8;
  a0 -= 1024; // 0xFFFFFC00
  v0 = spyro_sin(a0);
  v1 = lw(0x80078D74);
  v1 = lw(v1 + 0x0000);
  v1 = lw(v1 + 0x0004);
  s3 = v0 + 4096; // 0x1000
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 2;
  v0 -= v1;
  v1 = lw(0x80075828);
  v0 = v0 << 3;
  a0 = v1 + v0;
  v1 = lbu(a0 + 0x0048);
  v0 = 2; // 0x0002
  temp = v1 != v0;
  if (temp) goto label800315CC;
  v1 = lw(0x800756CC); // &0x00000000
  v0 = lbu(a0 + 0x0049);
  v0 += v1;
  sb(a0 + 0x0049, v0);
  v0 = v0 & 0xFF;
  v0 = v0 < 48;
  temp = v0 != 0;
  if (temp) goto label800315CC;
  sb(a0 + 0x0048, 0);
  sb(a0 + 0x003C, 0);
label800315CC:
  s0 = 0x80078D38;
  a0 = lw(s0 + 0x0000);
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
  v0 = lw(player_position);
  v1 = lw(0x80078D74);
  t0=lo;
  a1 = (int32_t)t0 >> 12;
  v0 += a1;
  sw(v1 + 0x000C, v0);
  v0 = spyro_sin(a0);
  mult(v0, s2);
  v0 = lw(player_position + 0x0004);
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
  v0 = lw(player_position);
  v1 = lw(0x80075898);
  t0=lo;
  a1 = (int32_t)t0 >> 12;
  v0 += a1;
  sw(v1 + 0x000C, v0);
  v0 = spyro_sin(a0);
  mult(v0, s2);
  v0 = lw(player_position + 0x0004);
  v1 = lw(0x80075898);
  t0=lo;
  a0 = (int32_t)t0 >> 12;
  v0 += a0;
  sw(v1 + 0x0010, v0);
label80031820:
  s0 = 0x80078D60;
  a0 = lw(s0 + 0x0000);
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
  s0 = player_position;
  v0 = lw(s0 + 0x0000);
  v1=lo;
  v1 = (int32_t)v1 >> 12;
  v0 += v1;
  sw(s1 + 0x0000, v0);
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
  v0 = lw(player_position + 0x0004);
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
  a0 = lw(s0 + 0x0000);
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
  sb(0x80078A66, v0);
  a0 = 0x80078A64;
  a1 = a0 + 40; // 0x0028
  a2 = 0;
  spyro_mat3_rotation(a0, a1, a2);
  v0 = s3 >> 9;
  sb(0x80078A69, v0);
  a0 = 16; // 0x0010
  function_8003CB24();
  function_80049660();
  function_80049E8C();
  a0 = lw(0x800756CC); // &0x00000000
  function_80048D10();
  a0 = lw(0x800756CC); // &0x00000000
  function_loaded_800756BC();
  goto label800324B8;
label80031BA8:
  a0 = lw(0x80078D38);
  v0 = spyro_cos(a0);
  v1 = lw(0x80078D3C);
  mult(v0, v1);
  s0 = player_position;
  v0 = lw(s0 + 0x0000);
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
  a3 = lw(s0 + 0x0000);
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
  a0 = lw(s1 + 0x0048);
  v0 = spyro_cos(a0);
  v1 = lw(s1 + 0x004C);
  mult(v0, v1);
  v0 = lw(s0 + 0x0000);
  v1 = lw(0x80075898);
  t0=lo;
  a0 = (int32_t)t0 >> 12;
  v0 += a0;
  sw(v1 + 0x000C, v0);
  a0 = lw(s1 + 0x0048);
  v0 = spyro_sin(a0);
  v1 = lw(s1 + 0x004C);
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
  v0 = lw(s0 + 0x0000);
  t0=lo;
  v1 = (int32_t)t0 >> 12;
  v0 += v1;
  sw(0x80076DF8, v0);
  v0 = spyro_sin(a0);
  a0 = lw(0x80078D64);
  v1 = 4; // 0x0004
  sb(0x80078A7C, v1);
  v1 = lw(0x80078D70);
  sb(0x80078A71, 0);
  mult(v0, a0);
  sb(0x80078A77, 0);
  v0 = 4; // 0x0004
  sw(0x80078AB0, v0);
  v0 = 1; // 0x0001
  a0 = lw(0x80078D68);
  v1 = (int32_t)v1 >> 4;
  sw(s1 - 0x0004, v0); // 0xFFFFFFFC
  sw(s1 + 0x0000, 0);
  sb(0x80078A66, v1);
  sw(0x80076E00, a0);
  v0 = lw(s0 + 0x0004);
  t0=lo;
  v1 = (int32_t)t0 >> 12;
  v0 += v1;
  sw(0x80076DFC, v0);
  goto label800324B8;
label80031DBC:
  a0 = lw(0x80078AB0);
  function_8003CB24();
  function_80049660();
  function_80049E8C();
  a0 = lw(0x800756CC); // &0x00000000
  v0 = lw(level_frame_counter);
  v1 = lw(0x800756BC); // &0x00000000
  v0++;
  sw(level_frame_counter, v0);
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
  v1 = lw(buttons_press);
  v0 = v1 & 0x4000;
  temp = v0 == 0;
  a1 = 0;
  if (temp) goto label80031E9C;
  v0 = lw(0x800761D4);
  a2 = 16; // 0x0010
  a0 = lbu(v0 + 0x002D);
  a3 = 0;
  function_80055A78();
  v0 = lw(0x80078D08);
  sw(0x80078D04, 0);
  v0++;
  sw(0x80078D08, v0);
  v0 = (int32_t)v0 < 3;
  temp = v0 != 0;
  if (temp) goto label80031EEC;
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
  v0 = lw(0x80078D08);
  sw(0x80078D04, 0);
  v0--;
  sw(0x80078D08, v0);
  temp = (int32_t)v0 >= 0;
  v0 = 2; // 0x0002
  if (temp) goto label80031EEC;
  sw(0x80078D08, v0);
label80031EEC:
  s0 = 0x80078D04;
  v0 = lw(s0 + 0x0000);
  v0 = (int32_t)v0 < 8;
  temp = v0 != 0;
  if (temp) goto label800324B8;
  v0 = lw(buttons_press);
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
  sw(s0 + 0x0000, 0);
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
  v0 = lw(v0 + 0x0000);
  v1 = lw(v0 + 0x0000);
  v0 = v1 << 1;
  v0 += v1;
  v0 = v0 << 2;
  v0 -= v1;
  v1 = lw(0x80075828);
  v0 = v0 << 3;
  s0 = v1 + v0;
  v1 = lw(s0 + 0x0000);
  a0 = lw(v1 + 0x0024);
  v0 = -1; // 0xFFFFFFFF
  temp = a0 != v0;
  if (temp) goto label800324B8;
  v0 = lw(v1 + 0x0018);
  temp = v0 == a0;
  if (temp) goto label800324B8;
  function_8002D02C();
  a0 = s0;
  v0 = 2; // 0x0002
  sb(a0 + 0x0048, v0);
  v0 = -1; // 0xFFFFFFFF
  sw(a0 + 0x0008, 0);
  sh(a0 + 0x0034, v0);
  function_8002C924();
  a0 = lw(CONTINUOUS_LEVEL_ID);
  v0 = 0x800772D8;
  a0 = a0 << 2;
  a0 += v0;
  v0 = lw(total_found_dragons);
  v1 = lw(a0 + 0x0000);
  v0--;
  v1--;
  sw(total_found_dragons, v0);
  sw(a0 + 0x0000, v1);
  goto label800324B8;
label80032054:
  s0 = 0x80078D04;
  v0 = lw(s0 + 0x0000);
  v0 = (int32_t)v0 < 60;
  temp = v0 != 0;
  if (temp) goto label800324B8;
  v0 = lw(buttons_press);
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
  sw(s0 + 0x0000, 0);
  sw(0x80078D0C, 0);
  sw(0x80078D08, v0);
  goto label800324B8;
label800320C0:
  s1 = 0x80078D08;
  v1 = lw(s1 + 0x0000);
  temp = v1 != 0;
  v0 = 1; // 0x0001
  if (temp) goto label800320FC;
  function_800662BC();
  a0 = lw(0x80078D18);
  function_800665B8();
  v0 = 1; // 0x0001
  sw(s1 + 0x0000, v0);
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
  a2 -= 1536; // 0xFFFFFA00
  function_80066E28();
  v0 = 2; // 0x0002
  sw(s1 + 0x0000, v0);
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
  v0 = lw(0x800785F0);
  v1 = lhu(v0 - 0x0582); // 0xFFFFFA7E
  v0 = lw(0x80078D20);
  temp = v0 != v1;
  v0 = 4; // 0x0004
  if (temp) goto label80032320;
  s0 = 0x800785F0;
  a0 = lw(s0 + 0x0000);
  a0 -= 1536; // 0xFFFFFA00
  function_80059864();
  a1 = 0x80010DB8; // "BASCUS-94228SPYRO"
  a0 = lw(0x80078D18);
  v0 = 1536; // 0x0600
  sw(sp + 0x0010, v0);
  a2 = lw(s0 + 0x0000);
  v0 = lw(0x80078D1C);
  a2 -= 1536; // 0xFFFFFA00
  a3 = v0 << 1;
  a3 += v0;
  a3 = a3 << 9;
  a3 += 512; // 0x0200
  function_800670E4();
  v0 = 3; // 0x0003
  sw(s1 + 0x0000, v0);
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
  v0 = 1536; // 0x0600
  sw(sp + 0x0010, v0);
  v0 = lw(0x80078D1C);
  a2 -= 1536; // 0xFFFFFA00
  a3 = v0 << 1;
  a3 += v0;
  a3 = a3 << 9;
  a3 += 512; // 0x0200
  function_80066E28();
  v0 = 4; // 0x0004
  sw(s1 + 0x0000, v0);
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
  v0 = lw(s0 + 0x0000);
  v0 = (int32_t)v0 < 60;
  temp = v0 != 0;
  if (temp) goto label800324B8;
  v0 = lw(buttons_press);
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
  s1 = buttons_press;
  v0 = lw(s1 + 0x0000);
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
  v1 = lw(a0 + 0x0000);
  v0 = 1; // 0x0001
  v0 -= v1;
  sw(a0 + 0x0000, v0);
label800323D8:
  s0 = 0x80078D04;
  v0 = lw(s0 + 0x0000);
  v0 = (int32_t)v0 < 32;
  temp = v0 != 0;
  if (temp) goto label800324B8;
  v0 = lw(s1 + 0x0000);
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
  sw(s0 + 0x0000, 0);
  sw(0x80078D0C, v0);
  sw(0x80078D08, 0);
  goto label800324B8;
label8003245C:
  v0 = lw(0x80078D04);
  v0 = (int32_t)v0 < 8;
  temp = v0 != 0;
  if (temp) goto label800324B8;
  v0 = lw(buttons_press);
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
  ra = lw(sp + 0x0040);
  s3 = lw(sp + 0x003C);
  s2 = lw(sp + 0x0038);
  s1 = lw(sp + 0x0034);
  s0 = lw(sp + 0x0030);
  sp += 72; // 0x0048
  return;
}