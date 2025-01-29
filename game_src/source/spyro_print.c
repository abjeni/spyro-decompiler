#include "debug.h"
#include "int_math.h"
#include "main.h"
#include "spyro_string.h"
#include "psx_mem.h"
#include "psx_bios.h"
#include "int_math.h"
#include "decompilation.h"

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
  BREAKPOINT;
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
  BREAKPOINT;
  spyro_puts(a0);
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
  BREAKPOINT;
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

  ra = 0x800627C8;
  function_800627D8();

  ra = lw(sp + 0x10);
  sp += 0x18;
}

void function_8006279C(void)
{
  BREAKPOINT;
  spyro_printf(a0, a1, a2, a3);
}