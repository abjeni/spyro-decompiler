#include "debug.h"
#include "int_math.h"
#include "main.h"
#include "spyro_string.h"
#include "psx_mem.h"
#include "psx_bios.h"
#include "int_math.h"
#include "psx_ops.h"

#include <string.h>
#include <stdint.h>

void spyro_putchar(uint32_t chr)
{
  chr = chr & 0xFF;
  if (chr == '\t') {
    uint32_t spaces = ((lw(0x80075AC8)+7)%8)+1;
    for (uint32_t i = 0; i < spaces; i++)
      spyro_putchar(' ');
    return;
  }
  if (chr == '\n') {
    spyro_putchar('\r');
    sw(0x80075AC8, 0);
  } else {
    if (lbu(0x80074D15 + chr) & 0x97)
      sw(0x80075AC8, lw(0x80075AC8) + 1);
  }
  sb(sp - 1, chr);
  psx_write(1, addr_to_pointer(sp - 1), 1);
}

// size: 0x000000B4
// print single character
// meant to be used by other printing functions
void function_80062F10(void)
{
  UNREACHABLE;
  spyro_putchar(a0);
}

void spyro_puts(uint32_t str)
{
  if (str == 0)
    str = 0x80011C98; // (null)
  while (1) {
    uint32_t chr = lbu(str++);
    if (chr == 0) break;
    spyro_putchar(chr);
  }
}

// size: 0x00000050
// print zero terminated string
// a0: string address
void function_8006389C(void)
{
  UNREACHABLE;
  spyro_puts(a0);
}



// size: 0x00000688
void function_800627D8(void)
{
  uint32_t temp;
  sp -= 136; // 0xFFFFFF78
  sw(sp + 0x0064, s1);
  s1 = a2;
  sw(sp + 0x0084, ra);
  sw(sp + 0x0080, fp);
  sw(sp + 0x007C, s7);
  sw(sp + 0x0078, s6);
  sw(sp + 0x0074, s5);
  sw(sp + 0x0070, s4);
  sw(sp + 0x006C, s3);
  sw(sp + 0x0068, s2);
  temp = a1 != 0;
  sw(sp + 0x0060, s0);
  if (temp) goto label80062814;
  v0 = 0;
  goto label80062E2C;
label80062814:
  s7 = a1;
  a3 = 0x800119A8; // "0123456789abcdef"
  sw(sp + 0x0058, a3);
  sw(sp + 0x0038, 0);
label80062828:
  a0 = lbu(s7 + 0x0000);
  temp = a0 != 0;
  v0 = 37; // 0x0025
  if (temp) goto label80062844;
label80062838:
  v0 = lw(sp + 0x0038);
  goto label80062E2C;
label80062844:
  temp = a0 != v0;
  s3 = 0;
  if (temp) goto label80062E1C;
  s4 = -1; // 0xFFFFFFFF
  fp = 0;
  sw(sp + 0x0040, 0);
  sw(sp + 0x0048, 0);
  sw(sp + 0x0050, 0);
label80062860:
  s7++;
  v1 = lbu(s7 + 0x0000);
  v0 = v1 < 121;
  temp = v0 == 0;
  v0 = v1 << 2;
  if (temp) goto label80062E0C;
  v0 = lw(0x800119D8 + v0); // &0x80062838
  temp = v0;
  switch (temp)
  {
  case 0x80062838: // 0
    goto label80062838;
    break;
  case 0x80062E0C: // 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 33 34 36 37 38 39 40 41 44 47 58 59 60 61 62 63 64 65 66 67 69 70 71 72 73 74 75 77 78 80 81 82 83 84 86 87 89 90 91 92 93 94 95 96 97 98 101 102 103 106 107 109 113 114 116 118 119
    goto label80062E0C;
    break;
  case 0x80062890: // 32
    goto label80062890;
    break;
  case 0x800628A0: // 35
    goto label800628A0;
    break;
  case 0x800628A8: // 42
    goto label800628A8;
    break;
  case 0x800628CC: // 43
    goto label800628CC;
    break;
  case 0x800628C4: // 45
    goto label800628C4;
    break;
  case 0x800628D4: // 46
    goto label800628D4;
    break;
  case 0x80062960: // 48
    goto label80062960;
    break;
  case 0x80062968: // 49 50 51 52 53 54 55 56 57
    goto label80062968;
    break;
  case 0x800629F4: // 68
    goto label800629F4;
    break;
  case 0x800629C0: // 76
    goto label800629C0;
    break;
  case 0x80062A78: // 79
    goto label80062A78;
    break;
  case 0x80062B20: // 85
    goto label80062B20;
    break;
  case 0x80062B58: // 88
    goto label80062B58;
    break;
  case 0x800629D8: // 99
    goto label800629D8;
    break;
  case 0x800629F8: // 100 105
    goto label800629F8;
    break;
  case 0x800629C8: // 104
    goto label800629C8;
    break;
  case 0x800629D0: // 108
    goto label800629D0;
    break;
  case 0x80062A3C: // 110
    goto label80062A3C;
    break;
  case 0x80062A7C: // 111
    goto label80062A7C;
    break;
  case 0x80062AB0: // 112
    goto label80062AB0;
    break;
  case 0x80062AC0: // 115
    goto label80062AC0;
    break;
  case 0x80062B24: // 117
    goto label80062B24;
    break;
  case 0x80062B64: // 120
    goto label80062B64;
    break;
  default:
    JR(temp, 0x80062888);
    return;
  }
label80062890:
  temp = fp != 0;
  if (temp) goto label80062860;
  fp = 32; // 0x0020
  goto label80062860;
label800628A0:
  s3 = s3 | 0x8;
  goto label80062860;
label800628A8:
  s1 += 4; // 0x0004
  a3 = lw(s1 - 0x0004); // 0xFFFFFFFC
  temp = (int32_t)a3 >= 0;
  sw(sp + 0x0050, a3);
  if (temp) goto label80062860;
  a3 = -a3;
  sw(sp + 0x0050, a3);
label800628C4:
  s3 = s3 | 0x10;
  goto label80062860;
label800628CC:
  fp = 43; // 0x002B
  goto label80062860;
label800628D4:
  s7++;
  v1 = lbu(s7 + 0x0000);
  v0 = 42; // 0x002A
  temp = v1 != v0;
  if (temp) goto label800628F8;
  s1 += 4; // 0x0004
  s0 = lw(s1 - 0x0004); // 0xFFFFFFFC
  goto label80062950;
label800628F8:
  v0 = v1 < 128;
  temp = v0 == 0;
  s0 = 0;
  if (temp) goto label8006294C;
label80062904:
  a0 = lbu(s7 + 0x0000);
  v0 = lbu(0x80074D15 + a0); // &0x20202020
  v0 = v0 & 0x4;
  temp = v0 == 0;
  if (temp) goto label8006294C;
  s7++;
  v0 = s0 << 2;
  v0 += s0;
  v0 = v0 << 1;
  v0 -= 48; // 0xFFFFFFD0
  v1 = lbu(s7 + 0x0000);
  v1 = v1 < 128;
  temp = v1 != 0;
  s0 = v0 + a0;
  if (temp) goto label80062904;
label8006294C:
  s7--;
label80062950:
  temp = (int32_t)s0 >= 0;
  s4 = s0;
  if (temp) goto label80062860;
  s4 = -1; // 0xFFFFFFFF
  goto label80062860;
label80062960:
  s3 = s3 | 0x20;
  goto label80062860;
label80062968:
  s0 = 0;
label8006296C:
  v0 = lbu(s7 + 0x0000);
  s7++;
  v1 = s0 << 2;
  v1 += s0;
  v1 = v1 << 1;
  v1 -= 48; // 0xFFFFFFD0
  a0 = lbu(s7 + 0x0000);
  s0 = v1 + v0;
  v0 = a0 < 128;
  temp = v0 == 0;
  if (temp) goto label800629B4;
  v0 = lbu(0x80074D15 + a0); // &0x20202020
  v0 = v0 & 0x4;
  temp = v0 != 0;
  if (temp) goto label8006296C;
label800629B4:
  sw(sp + 0x0050, s0);
  s7--;
  goto label80062860;
label800629C0:
  s3 = s3 | 0x2;
  goto label80062860;
label800629C8:
  s3 = s3 | 0x4;
  goto label80062860;
label800629D0:
  s3 = s3 | 0x1;
  goto label80062860;
label800629D8:
  s2 = sp + 16; // 0x0010
  s1 += 4; // 0x0004
  s6 = 1; // 0x0001
  v0 = lw(s1 - 0x0004); // 0xFFFFFFFC
  fp = 0;
  sb(sp + 0x0010, v0);
  goto label80062C48;
label800629F4:
  s3 = s3 | 0x1;
label800629F8:
  v0 = s3 & 0x1;
  temp = v0 != 0;
  v0 = s3 & 0x4;
  if (temp) goto label80062A1C;
  temp = v0 == 0;
  if (temp) goto label80062A1C;
  s1 += 4; // 0x0004
  v1 = lh(s1 - 0x0004); // 0xFFFFFFFC
  goto label80062A24;
label80062A1C:
  s1 += 4; // 0x0004
  v1 = lw(s1 - 0x0004); // 0xFFFFFFFC
label80062A24:
  temp = (int32_t)v1 >= 0;
  a1 = 10; // 0x000A
  if (temp) goto label80062BAC;
  v1 = -v1;
  fp = 45; // 0x002D
  goto label80062BAC;
label80062A3C:
  v0 = s3 & 0x1;
  temp = v0 != 0;
  v0 = s3 & 0x4;
  if (temp) goto label80062A64;
  temp = v0 == 0;
  if (temp) goto label80062A64;
  s1 += 4; // 0x0004
  v0 = lw(s1 - 0x0004); // 0xFFFFFFFC
  a3 = lhu(sp + 0x0038);
  sh(v0 + 0x0000, a3);
  goto label80062E24;
label80062A64:
  s1 += 4; // 0x0004
  v0 = lw(s1 - 0x0004); // 0xFFFFFFFC
  a3 = lw(sp + 0x0038);
  sw(v0 + 0x0000, a3);
  goto label80062E24;
label80062A78:
  s3 = s3 | 0x1;
label80062A7C:
  v0 = s3 & 0x1;
  temp = v0 != 0;
  v0 = s3 & 0x4;
  if (temp) goto label80062AA0;
  temp = v0 == 0;
  a1 = 8; // 0x0008
  if (temp) goto label80062AA0;
  s1 += 4; // 0x0004
  v1 = lh(s1 - 0x0004); // 0xFFFFFFFC
  goto label80062BA8;
label80062AA0:
  s1 += 4; // 0x0004
  v1 = lw(s1 - 0x0004); // 0xFFFFFFFC
  a1 = 8; // 0x0008
  goto label80062BA8;
label80062AB0:
  s1 += 4; // 0x0004
  v1 = lw(s1 - 0x0004); // 0xFFFFFFFC
  a1 = 16; // 0x0010
  goto label80062BA8;
label80062AC0:
  s1 += 4; // 0x0004
  s2 = lw(s1 - 0x0004); // 0xFFFFFFFC
  temp = s2 != 0;
  if (temp) goto label80062ADC;
  s2 = 0x800119BC; // "(null)"
label80062ADC:
  temp = (int32_t)s4 < 0;
  a0 = s2;
  if (temp) goto label80062B0C;
  a1 = 0;
  a2 = s4;
  v0 = spyro_strchr(a0, a1, a2);
  temp = v0 == 0;
  s6 = v0 - s2;
  if (temp) goto label80062B04;
  v0 = (int32_t)s4 < (int32_t)s6;
  temp = v0 == 0;
  fp = 0;
  if (temp) goto label80062C48;
label80062B04:
  s6 = s4;
  goto label80062B18;
label80062B0C:
  a0 = s2;
  v0 = spyro_strlen(a0);
  s6 = v0;
label80062B18:
  fp = 0;
  goto label80062C48;
label80062B20:
  s3 = s3 | 0x1;
label80062B24:
  v0 = s3 & 0x1;
  temp = v0 != 0;
  v0 = s3 & 0x4;
  if (temp) goto label80062B48;
  temp = v0 == 0;
  a1 = 10; // 0x000A
  if (temp) goto label80062B48;
  s1 += 4; // 0x0004
  v1 = lh(s1 - 0x0004); // 0xFFFFFFFC
  goto label80062BA8;
label80062B48:
  s1 += 4; // 0x0004
  v1 = lw(s1 - 0x0004); // 0xFFFFFFFC
  a1 = 10; // 0x000A
  goto label80062BA8;
label80062B58:
  a3 = 0x800119C4; // "0123456789ABCDEF"
  sw(sp + 0x0058, a3);
label80062B64:
  v0 = s3 & 0x1;
  temp = v0 != 0;
  v0 = s3 & 0x4;
  if (temp) goto label80062B88;
  temp = v0 == 0;
  v0 = s3 & 0x8;
  if (temp) goto label80062B88;
  s1 += 4; // 0x0004
  v1 = lh(s1 - 0x0004); // 0xFFFFFFFC
  goto label80062B94;
label80062B88:
  s1 += 4; // 0x0004
  v1 = lw(s1 - 0x0004); // 0xFFFFFFFC
  v0 = s3 & 0x8;
label80062B94:
  temp = v0 == 0;
  a1 = 16; // 0x0010
  if (temp) goto label80062BA8;
  temp = v1 == 0;
  fp = 0;
  if (temp) goto label80062BAC;
  s3 = s3 | 0x40;
label80062BA8:
  fp = 0;
label80062BAC:
  temp = (int32_t)s4 < 0;
  sw(sp + 0x0040, s4);
  if (temp) goto label80062BBC;
  v0 = -33; // 0xFFFFFFDF
  s3 = s3 & v0;
label80062BBC:
  temp = v1 != 0;
  s2 = sp + 56; // 0x0038
  if (temp) goto label80062BD4;
  a3 = lw(sp + 0x0040);
  temp = a3 == 0;
  v0 = sp - s2;
  if (temp) goto label80062C44;
label80062BD4:
  divu_psx(v1, a1);
  temp = a1 != 0;
  if (temp) goto label80062BE4;
  UNREACHABLE; // BREAK 0x01C00
label80062BE4:
  v1=lo;
  v0=hi;
  a3 = lw(sp + 0x0058);
  v0 += a3;
  a0 = lbu(v0 + 0x0000);
  s2--;
  temp = v1 != 0;
  sb(s2 + 0x0000, a0);
  if (temp) goto label80062BD4;
  a3 = 0x800119A8; // "0123456789abcdef"
  v0 = s3 & 0x8;
  temp = v0 == 0;
  sw(sp + 0x0058, a3);
  if (temp) goto label80062C40;
  v0 = 8; // 0x0008
  temp = a1 != v0;
  v0 = sp - s2;
  if (temp) goto label80062C44;
  v1 = a0 & 0xFF;
  v0 = 48; // 0x0030
  temp = v1 == v0;
  v0 = 48; // 0x0030
  if (temp) goto label80062C40;
  s2--;
  sb(s2 + 0x0000, v0);
label80062C40:
  v0 = sp - s2;
label80062C44:
  s6 = v0 + 56; // 0x0038
label80062C48:
  a3 = lw(sp + 0x0048);
  temp = fp == 0;
  s4 = s6 + a3;
  if (temp) goto label80062C58;
  s4++;
label80062C58:
  v0 = s3 & 0x40;
  temp = v0 == 0;
  if (temp) goto label80062C68;
  s4 += 2; // 0x0002
label80062C68:
  s5 = lw(sp + 0x0040);
  v0 = (int32_t)s5 < (int32_t)s4;
  temp = v0 == 0;
  v0 = s3 & 0x30;
  if (temp) goto label80062C80;
  s5 = s4;
label80062C80:
  temp = v0 != 0;
  a0 = fp & 0xFF;
  if (temp) goto label80062CBC;
  a3 = lw(sp + 0x0050);
  temp = a3 == 0;
  v0 = (int32_t)s5 < (int32_t)a3;
  if (temp) goto label80062CBC;
  temp = v0 == 0;
  s0 = s5;
  if (temp) goto label80062CBC;
label80062CA0:
  a0 = 32; // 0x0020
  spyro_putchar(a0);
  a3 = lw(sp + 0x0050);
  s0++;
  v0 = (int32_t)s0 < (int32_t)a3;
  temp = v0 != 0;
  a0 = fp & 0xFF;
  if (temp) goto label80062CA0;
label80062CBC:
  temp = a0 == 0;
  v0 = s3 & 0x40;
  if (temp) goto label80062CD0;
  spyro_putchar(a0);
  v0 = s3 & 0x40;
label80062CD0:
  temp = v0 == 0;
  v1 = s3 & 0x30;
  if (temp) goto label80062CF0;
  a0 = 48; // 0x0030
  spyro_putchar(a0);
  a0 = lbu(s7 + 0x0000);
  spyro_putchar(a0);
  v1 = s3 & 0x30;
label80062CF0:
  v0 = 32; // 0x0020
  temp = v1 != v0;
  if (temp) goto label80062D2C;
  a3 = lw(sp + 0x0050);
  v0 = (int32_t)s5 < (int32_t)a3;
  temp = v0 == 0;
  s0 = s5;
  if (temp) goto label80062D2C;
label80062D10:
  a0 = 48; // 0x0030
  spyro_putchar(a0);
  a3 = lw(sp + 0x0050);
  s0++;
  v0 = (int32_t)s0 < (int32_t)a3;
  temp = v0 != 0;
  if (temp) goto label80062D10;
label80062D2C:
  a3 = lw(sp + 0x0040);
  s0 = s4;
  v0 = (int32_t)s0 < (int32_t)a3;
  temp = v0 == 0;
  if (temp) goto label80062D5C;
label80062D40:
  a0 = 48; // 0x0030
  spyro_putchar(a0);
  a3 = lw(sp + 0x0040);
  s0++;
  v0 = (int32_t)s0 < (int32_t)a3;
  temp = v0 != 0;
  if (temp) goto label80062D40;
label80062D5C:
  s0 = s6 - 1; // 0xFFFFFFFF
  temp = (int32_t)s0 < 0;
  if (temp) goto label80062D90;
label80062D68:
  a0 = lbu(s2 + 0x0000);
  s2++;
  s0--;
  spyro_putchar(a0);
  temp = (int32_t)s0 < 0;
  if (temp) goto label80062D90;
  goto label80062D68;
label80062D88:
  a0 = 48; // 0x0030
  spyro_putchar(a0);
label80062D90:
  a3 = lw(sp + 0x0048);
  a3--;
  temp = (int32_t)a3 >= 0;
  sw(sp + 0x0048, a3);
  if (temp) goto label80062D88;
  v0 = s3 & 0x10;
  temp = v0 == 0;
  if (temp) goto label80062DE0;
  a3 = lw(sp + 0x0050);
  v0 = (int32_t)s5 < (int32_t)a3;
  temp = v0 == 0;
  s0 = s5;
  if (temp) goto label80062DE0;
label80062DC4:
  a0 = 32; // 0x0020
  spyro_putchar(a0);
  a3 = lw(sp + 0x0050);
  s0++;
  v0 = (int32_t)s0 < (int32_t)a3;
  temp = v0 != 0;
  if (temp) goto label80062DC4;
label80062DE0:
  v1 = lw(sp + 0x0050);
  v0 = (int32_t)v1 < (int32_t)s5;
  temp = v0 == 0;
  if (temp) goto label80062DF8;
  v1 = s5;
label80062DF8:
  a3 = lw(sp + 0x0038);
  a3 += v1;
  sw(sp + 0x0038, a3);
  goto label80062E24;
label80062E0C:
  a3 = lw(sp + 0x0038);
  a0 = lbu(s7 + 0x0000);
  a3++;
  sw(sp + 0x0038, a3);
label80062E1C:
  spyro_putchar(a0);
label80062E24:
  s7++;
  goto label80062828;
label80062E2C:
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

// size: 0x0000085C
// sprintf
// a0: dst (char *)
// a1: fmt (char *)
// sp + 0x08: vaarg pointer
void spyro_sprintf(void)
{
  sw(sp + 0x4, a1);
  sw(sp + 0x8, a2);
  sw(sp + 0xC, a3);

  sp -= 0x248;
  sw(sp + 0x0244, ra);
  sw(sp + 0x0234, s3);
  sw(sp + 0x0230, s2);
  sw(sp + 0x022C, s1);
  sw(sp + 0x0228, s0);

  s3 = a0;
  uint32_t va_addr = sp + 0x0250;
  uint32_t fmt_ptr = a1;

  s2 = 0;
  a1 = lbu(fmt_ptr);
  while (a1) {
    if (a1 != '%') goto label80063738;
    a1 = 0x80074D98;
    sw(sp + 0x0210, lw(a1 + 0));
    sw(sp + 0x0214, lw(a1 + 4));
    sw(sp + 0x0218, lw(a1 + 8));
    while (1) {
      a1 = lbu(++fmt_ptr);
      if (a1 == '-')
        sb(sp + 0x0210, lb(sp + 0x0210) | 0x01);
      else if (a1 == '+')
        sb(sp + 0x0210, lb(sp + 0x0210) | 0x02);
      else if (a1 == '#')
        sb(sp + 0x0210, lb(sp + 0x0210) | 0x04);
      else if (a1 == '0')
        sb(sp + 0x0210, lb(sp + 0x0210) | 0x08);
      else if (a1 == ' ')
        sb(sp + 0x0210 + 1, 0x20);
      else break;
    }
    v0 = a1 - '0';
    if (a1 != '*') goto label80063188;
    v0 = lw(va_addr);
    va_addr += 4;
    if ((int32_t)v0 < 0) {
      v0 = -v0;
      sb(sp + 0x0210, lb(sp + 0x0210) | 0x01);
    }
    sw(sp + 0x0214, v0);
    a1 = lbu(++fmt_ptr);
    v0 = '.';
    goto label80063194;
  label8006314C:
    v1 = lw(sp + 0x0214);
    v0 = v1*10 + a1 - '0';
    sw(sp + 0x0214, v0);
    a1 = lbu(fmt_ptr++ + 1);
    v0 = a1 - '0';
  label80063188:
    v0 = v0 < 10;
    if (v0 != 0) goto label8006314C;
  label80063194:
    if (a1 != '.') goto label80063254;
    a1 = lbu(++fmt_ptr);
    if (a1 != '*') goto label80063228;
    sw(sp + 0x0218, lw(va_addr));
    va_addr += 4;
    a1 = lbu(++fmt_ptr);
    goto label80063234;
  label800631EC:
    sw(sp + 0x0218, lw(sp + 0x0218)*10 + a1 - '0');
    a1 = lbu(++fmt_ptr);
  label80063228:
    if (a1 >= '0' && a1 <= '9') goto label800631EC;
  label80063234:
    v0 = lw(sp + 0x0218);
    if ((int32_t)v0 >= 0)
      sb(sp + 0x0210, lb(sp + 0x0210) | 0x10);
    
  label80063254:

    v1 = lw(sp + 0x0210);
    s1 = sp + 0x0210;
    if (v1 & 0x01)
      sw(sp + 0x0210, v1 & ~0x08);
  label80063274:
    switch (a1)
    {
    case 'L':
      sw(sp + 0x0210, lw(sp + 0x0210) | 0x80);
      a1 = lbu(++fmt_ptr);
      goto label80063274;
    case 'X':
      a3 = 0x80011BBC;
      goto label80063564;
    case 'c':
      s1--;
      sb(s1, lw(va_addr));
      va_addr += 4;
      s0 = 1;
      goto label80063744;
    case 'd':
    case 'i':
      a0 = lw(va_addr);
      va_addr += 4;
      v1 = lw(sp + 0x0210);
      v0 = a0 << 16;
      if (v1 & 0x20)
        a0 = (int32_t)v0 >> 16;
      
      v0 = v1 & 0x02;
      if ((int32_t)a0 < 0) {
        a0 = -a0;
        sb(sp + 0x0211, '-');
      } else if (v1 & 0x02)
        sb(sp + 0x0211, '+');
      goto label80063360;
    case 'h':
      sw(sp + 0x0210, lw(sp + 0x0210) | 0x20);
      a1 = lbu(++fmt_ptr);
      goto label80063274;
    case 'l':
      sw(sp + 0x0210, lw(sp + 0x0210) | 0x40);
      a1 = lbu(++fmt_ptr);
      goto label80063274;
    case 'n':
      if (lw(sp + 0x0210) & 0x20)
        sh(va_addr, s2);
      else
        sw(va_addr, s2);
      va_addr += 4;
      goto label800637D8;
    case 'o':
      a0 = lw(va_addr);
      va_addr += 4;
      v1 = lw(sp + 0x0210);
      if (v1 & 0x20)
        a0 = a0 & 0xFFFF;
      
      if ((v1 & 0x10) == 0) {
        if (v1 & 0x8)
          sw(sp + 0x0218, lw(sp + 0x0214));
        v0 = 1;
        if ((int32_t)lw(sp + 0x0218) <= 0)
          sw(sp + 0x0218, v0);
      }

      s0 = 0;
      while (a0) {
        s1--;
        v0 = (a0 & 7) + '0'; // 0x0030
        sb(s1, v0);
        a0 = a0 >> 3;
        s0++;
      }
      if (lw(sp + 0x0210) & 0x04 && s0 && lbu(s1) != '0') {
        s1--;
        sb(s1, '0');
        s0++;
      }
      v1 = '0';
      while ((int32_t)s0 < (int32_t)lw(sp + 0x0218)) {
        s1--;
        sb(s1, '0');
        s0++;
      }
      goto label80063744;
    case 'p':
      sw(sp + 0x0218, 8);
      sw(sp + 0x0210, lw(sp + 0x0210) | 0x50);
      a3 = 0x80011BBC;
      goto label80063564;
    case 's':
      s1 = lw(va_addr);
      va_addr += 4;
      v1 = lw(sp + 0x0210);
      if (v1 & 0x04) {
        s0 = lbu(s1);
        s1++;
        if (v1 & 0x10)
          s0 = min_int(s0, lw(sp + 0x0218));
      } else {
        if ((v1 & 0x10) == 0)
          s0 = spyro_strlen(s1);
        else {
          v0 = spyro_strchr(s1, 0, lw(sp + 0x0218));
          if (v0)
            s0 = v0 - s1;
          else
            s0 = lw(sp + 0x0218);
        }
      }
      goto label80063744;
    case 'u':
      a0 = lw(va_addr);
      va_addr += 4;
      if (lw(sp + 0x0210) & 0x20)
        a0 = a0 & 0xFFFF;
      sb(sp + 0x0211, 0);
      goto label80063360;
    case 'x':
      a3 = 0x80011BD0;
      goto label80063564;
    }
    if (a1 != '%') goto label800637F8;
  label80063738:
    sb(s3 + s2, a1);
    s2++;
    goto label800637D8;
  label80063360:
    v1 = lw(sp + 0x0210);
    if ((v1 & 0x10) == 0) {
      if (v1 & 0x08) {
        v1 = lw(sp + 0x0214);
        if (lbu(sp + 0x0211))
          sw(sp + 0x0218, v1 - 1);
        else
          sw(sp + 0x0218, v1);
      }
      if ((int32_t)lw(sp + 0x0218) <= 0)
        sw(sp + 0x0218, 1);
    }

    s0 = 0;
    while (a0) {
      s0++;
      v1 = a0 / 10;
      sb(--s1, a0 - v1*10 + '0');
      a0 = v1;
    }
    while ((int32_t)s0 < (int32_t)lw(sp + 0x0218)) {
      s1--;
      sb(s1, '0');
      s0++;
    }
    v0 = lbu(sp + 0x0211);
    if (v0) {
      s1--;
      sb(s1, v0);
      s0++;
    }
    goto label80063744;
  label80063564:
    a0 = lw(va_addr);
    va_addr += 4;
    v1 = lw(sp + 0x0210);
    if (v1 & 0x20)
      a0 = a0 & 0xFFFF;
    
    if ((v1 & 0x10) == 0) {
      if (v1 & 0x08) {
        a2 = lw(sp + 0x0214);
        if (v1 & 0x04)
          sw(sp + 0x0218, a2 - 2);
        else
          sw(sp + 0x0218, a2);
      }
      v0 = lw(sp + 0x0218);
      if ((int32_t)v0 <= 0)
        sw(sp + 0x0218, 1);
    }
    s0 = 0;
    while (a0) {
      s1--;
      s0++;
      sb(s1, lbu((a0 & 0xF) + a3));
      a0 = a0 >> 4;
    }
    while ((int32_t)s0 < (int32_t)lw(sp + 0x0218)) {
      s1--;
      sb(s1, '0');
      s0++;
    }
    if (lw(sp + 0x0210) & 0x04) {
      s1--;
      sb(s1, a1);
      s1--;
      s0 += 2;
      sb(s1, '0');
    }
    goto label80063744;
  label80063744:
    if (!(lw(sp + 0x0210) & 0x01)) {
      v1 = s2 + s3;
      while ((int32_t)lw(sp + 0x0214) > (int32_t)s0) {
        sb(v1, ' ');
        v1++;
        sw(sp + 0x0214, lw(sp + 0x0214)-1);
        s2++;
      }
    }
    memmove(addr_to_pointer(s3 + s2), addr_to_pointer(s1), s0);
    s2 += s0;
    v1 = s2 + s3;
    while ((int32_t)s0 < (int32_t)lw(sp + 0x0214)) {
      sb(v1, ' ');
      v1++;
      s0++;
      s2++;
    }
  label800637D8:
    a1 = lbu(++fmt_ptr);
  }
label800637F8:
  sb(s3 + s2, 0);
  v0 = s2;
  ra = lw(sp + 0x0244);
  s3 = lw(sp + 0x0234);
  s2 = lw(sp + 0x0230);
  s1 = lw(sp + 0x022C);
  s0 = lw(sp + 0x0228);
  sp += 0x248;
  return;
}

void function_80062FD4(void)
{
  UNREACHABLE;
  spyro_sprintf();
}

// size: 0x0000003C
void spyro_printf(uint32_t fmt, uint32_t param1, uint32_t param2, uint32_t param3)
{
  sp -= 0x18;
  sw(sp + 0x10, ra);

  sw(sp + 0x18, fmt);
  sw(sp + 0x1C, param1);
  sw(sp + 0x20, param2);
  sw(sp + 0x24, param3);
  
  a0 = 1;
  a1 = fmt;
  a2 = sp + 0x1C;

  function_800627D8();

  ra = lw(sp + 0x10);
  sp += 0x18;
}

void function_8006279C(void)
{
  UNREACHABLE;
  spyro_printf(a0, a1, a2, a3);
}