#include "decompilation.h"
#include "psx_mem.h"
#include "main.h"

#include <string.h>

void spyro_memset32(uint32_t dst, uint32_t set, uint32_t len)
{
  assert((dst%4) == 0);
  assert((len%4) == 0);
  memset(addr_to_pointer(dst), set, len);
}

// size: 0x0000001C
void function_80016914()
{
  BREAKPOINT;
  spyro_memset32(a0, a1, a2);
}

// size: 0x00000028
void function_80016930()
{
  BREAKPOINT;
  assert((a2%16) == 0);spyro_memset32(a0, a1, a2);
}

void spyro_memcpy32(uint32_t dst, uint32_t src, uint32_t len)
{
  assert((dst%4) == 0);
  assert((src%4) == 0);
  assert((len%4) == 0);
  memcpy(addr_to_pointer(dst), addr_to_pointer(src), len);
}

// size: 0x00000054
void function_80016958(void)
{
  BREAKPOINT;
  spyro_memcpy32(a0, a1, a2);
}

uint32_t spyro_memcpy8(uint32_t dst, uint32_t src, uint32_t len)
{
  if (dst == 0) return 0;
  memcpy(addr_to_pointer(dst), addr_to_pointer(src), len);
  return dst;
}

void spyro_memset8(uint32_t dst, uint8_t set, uint32_t len)
{
  memset(addr_to_pointer(dst), set, len);
}

// size: 0x0000002C
void function_8006230C(void)
{
  BREAKPOINT;
  spyro_memset8(a0, a1, a2);
}

// size: 0x00000034
void function_800626F8()
{
  BREAKPOINT;
  v0 = spyro_memcpy8(a0, a1, a2);
}

uint32_t spyro_strlen(uint32_t str)
{
  if (str == 0) return 0;
  return strlen(addr_to_pointer(str));
}

// size: 0x00000030
void function_8006276C()
{
  BREAKPOINT;
  v0 = spyro_strlen(a0);
}

uint32_t spyro_strchr(uint32_t str, uint32_t chr, uint32_t len)
{
  if (str == 0) return 0;
  chr = chr & 0xFF;
  for (int i = 0; i < len; i++)
    if (lbu(a0+i) == a1) return a0+i;
  return 0;
}

// size: 0x00000050
void function_80062EC0()
{
  BREAKPOINT;
  v0 = spyro_strchr(a0, a1, a2);
}

void spyro_memmove(uint32_t dst, uint32_t src, uint32_t len)
{
  memmove(addr_to_pointer(dst), addr_to_pointer(src), len);
}

// size: 0x0000006C
void function_80063830(void)
{
  BREAKPOINT;
  spyro_memmove(a0, a1, a2);
}

void spyro_memclr32(uint32_t dst, uint32_t len)
{
  for (int i = 0; i < len; i++)
    sw(dst + i*4, 0);
}

uint32_t spyro_memclr8(uint32_t dst, uint32_t len)
{
  if (dst == 0 || (int32_t)len <= 0)
    return 0;
  for (int i = 0; i < len; i++)
    sb(dst + i, 0);
  return dst;
}

// size: 0x00000030
void function_8006B670(void)
{
  BREAKPOINT;
  v0 = spyro_memclr8(a0, a1);
}

// clear_memory(a0: addr, a1: len)
void function_8005E4AC()
{
  BREAKPOINT;
  spyro_memclr32(a0, a1);
}

// another mem zero lmao
void function_8005E8AC()
{
  BREAKPOINT;
  spyro_memclr32(a0, a1);
}

// another mem zero lmao
void function_8005E604()
{
  BREAKPOINT;
  spyro_memclr32(a0, a1);
}