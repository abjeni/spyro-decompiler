#include "extra_gte.h"
#include "int_math.h"
#include "psx_mem.h"
#include "psx_ops.h"
#include "decompilation.h"
#include "gte.h"
#include "main.h"
#include "int_math.h"
#include "spyro_math.h"
#include "debug.h"

#undef DEPRECATED
#define DEPRECATED

int16_t sqrt_lookup[] = { // 0x80074B84
  0x1000, 0x101F, 0x103F, 0x105E, 0x107E, 0x109C, 0x10BB, 0x10DA, 
  0x10F8, 0x1116, 0x1134, 0x1152, 0x116F, 0x118C, 0x11A9, 0x11C6, 
  0x11E3, 0x1200, 0x121C, 0x1238, 0x1254, 0x1270, 0x128C, 0x12A7, 
  0x12C2, 0x12DE, 0x12F9, 0x1314, 0x132E, 0x1349, 0x1364, 0x137E, 
  0x1398, 0x13B2, 0x13CC, 0x13E6, 0x1400, 0x1419, 0x1432, 0x144C, 
  0x1465, 0x147E, 0x1497, 0x14B0, 0x14C8, 0x14E1, 0x14F9, 0x1512, 
  0x152A, 0x1542, 0x155A, 0x1572, 0x158A, 0x15A2, 0x15B9, 0x15D1, 
  0x15E8, 0x1600, 0x1617, 0x162E, 0x1645, 0x165C, 0x1673, 0x1689, 
  0x16A0, 0x16B7, 0x16CD, 0x16E4, 0x16FA, 0x1710, 0x1726, 0x173C, 
  0x1752, 0x1768, 0x177E, 0x1794, 0x17AA, 0x17BF, 0x17D5, 0x17EA, 
  0x1800, 0x1815, 0x182A, 0x183F, 0x1854, 0x1869, 0x187E, 0x1893, 
  0x18A8, 0x18BD, 0x18D1, 0x18E6, 0x18FA, 0x190F, 0x1923, 0x1938, 
  0x194C, 0x1960, 0x1974, 0x1988, 0x199C, 0x19B0, 0x19C4, 0x19D8, 
  0x19EC, 0x1A00, 0x1A13, 0x1A27, 0x1A3A, 0x1A4E, 0x1A61, 0x1A75, 
  0x1A88, 0x1A9B, 0x1AAE, 0x1AC2, 0x1AD5, 0x1AE8, 0x1AFB, 0x1B0E, 
  0x1B21, 0x1B33, 0x1B46, 0x1B59, 0x1B6C, 0x1B7E, 0x1B91, 0x1BA3, 
  0x1BB6, 0x1BC8, 0x1BDB, 0x1BED, 0x1C00, 0x1C12, 0x1C24, 0x1C36, 
  0x1C48, 0x1C5A, 0x1C6C, 0x1C7E, 0x1C90, 0x1CA2, 0x1CB4, 0x1CC6, 
  0x1CD8, 0x1CE9, 0x1CFB, 0x1D0D, 0x1D1E, 0x1D30, 0x1D41, 0x1D53, 
  0x1D64, 0x1D76, 0x1D87, 0x1D98, 0x1DAA, 0x1DBB, 0x1DCC, 0x1DDD, 
  0x1DEE, 0x1E00, 0x1E11, 0x1E22, 0x1E33, 0x1E43, 0x1E54, 0x1E65, 
  0x1E76, 0x1E87, 0x1E98, 0x1EA8, 0x1EB9, 0x1ECA, 0x1EDA, 0x1EEB, 
  0x1EFB, 0x1F0C, 0x1F1C, 0x1F2D, 0x1F3D, 0x1F4E, 0x1F5E, 0x1F6E, 
  0x1F7E, 0x1F8F, 0x1F9F, 0x1FAF, 0x1FBF, 0x1FCF, 0x1FDF, 0x1FEF, 
};

const uint8_t atan_lut[64] = { // 0x8006D908
  0x00, 0x01, 0x01, 0x02,
  0x03, 0x03, 0x04, 0x04,
  0x05, 0x06, 0x06, 0x07,
  0x08, 0x08, 0x09, 0x09,
  0x0a, 0x0b, 0x0b, 0x0c,
  0x0c, 0x0d, 0x0d, 0x0e,
  0x0f, 0x0f, 0x10, 0x10,
  0x11, 0x11, 0x12, 0x12,
  0x13, 0x13, 0x14, 0x14,
  0x15, 0x15, 0x16, 0x16,
  0x17, 0x17, 0x18, 0x18,
  0x19, 0x19, 0x19, 0x1a,
  0x1a, 0x1b, 0x1b, 0x1b,
  0x1c, 0x1c, 0x1d, 0x1d,
  0x1d, 0x1e, 0x1e, 0x1e,
  0x1f, 0x1f, 0x1f, 0x20
};

const uint32_t math_lut[] = { // 0x8006CE7C
  0xFFFFFFFF,
  0x00000324,
  0x0000096E,
  0x00000FBA,
  0x0000160C,
  0x00001C64,
  0x000022C5,
  0x00002931,
  0x00002FAA,
  0x00003632,
  0x00003CCC,
  0x00004379,
  0x00004A3D,
  0x00005119,
  0x00005811,
  0x00005F28,
  0x00006660,
  0x00006DBD,
  0x00007542,
  0x00007CF2,
  0x000084D7,
  0x00008CE7,
  0x00009534,
  0x00009DBE,
  0x0000A68C,
  0x0000AFA3,
  0x0000B909,
  0x0000C2C7,
  0x0000CCE3,
  0x0000D768,
  0x0000E25E,
  0x0000EDD0,
  0x0000F9CB,
  0x00010001
};

const uint16_t math_lut2[] = { // 0x8006CF04
  0x0648, 0x064A, 0x064C, 0x0652,
  0x0658, 0x0661, 0x066C, 0x0679,
  0x0688, 0x069A, 0x06AD, 0x06C4,
  0x06DC, 0x06F8, 0x0717, 0x0738,
  0x075D, 0x0785, 0x07B0, 0x07E5,
  0x0810, 0x084D, 0x088A, 0x08CE,
  0x0917, 0x0966, 0x09BF, 0x0A1C,
  0x0A85, 0x0AF6, 0x0B72, 0x0BFB,
  0x0C6A, 0x0000,
};

struct game_object {
  uint32_t unknown00; // 0x00 - 0x04 // pointer
  uint8_t unknown04[0x04]; // 0x04 - 0x08
  uint32_t unknown08;
  vec3_32 p; // 0x0C - 0x18
  uint32_t unknown18; // 0x18 - 0x1C
  uint32_t unknown1C; // 0x1C - 0x20
  uint8_t unknown20[0x14]; // 0x20 - 0x34
  uint16_t unknown34; // 0x34 - 0x36
  uint16_t modelID; // 0x36 - 0x38
  uint16_t unknown38; // 0x38 - 0x3A
  uint16_t unknown3A; // 0x3A - 0x3C
  uint8_t unknown3C; // 0x3C - 0x3D
  uint8_t unknown3D; // 0x3D - 0x3E
  uint8_t unknown3E; // 0x3E - 0x3F
  uint8_t unknown3F; // 0x3F - 0x40
  uint8_t unknown40; // 0x40 - 0x41
  uint8_t unknown41; // 0x41 - 0x42
  uint8_t unknown42; // 0x42 - 0x43
  int8_t unknown43; // 0x43 - 0x44
  uint8_t rotx; // 0x44 - 0x45
  uint8_t roty; // 0x45 - 0x46
  uint8_t rotz; // 0x46 - 0x47
  uint8_t unknown47; // 0x47 - 0x48
  int8_t unknown48; // 0x48 - 0x49
  uint8_t unknown49; // 0x49 - 0x4A
  uint8_t unknown4A; // 0x4A - 0x4B
  uint8_t unknown4B; // 0x4B - 0x4C
  uint8_t unknown4C; // 0x4C - 0x4D
  uint8_t unknown4D; // 0x4D - 0x4E
  uint8_t unknown4E; // 0x4E - 0x4F
  uint8_t unknown4F; // 0x4F - 0x50 material id?
  uint8_t render_distance; // 0x50 - 0x51
  int8_t unknown51; // 0x51 - 0x52
  int8_t unknown52; // 0x52 - 0x53
  uint8_t unknown53; // 0x53 - 0x54
  uint8_t unknown54; // 0x54 - 0x55
  uint8_t unknown55; // 0x55 - 0x56
  uint8_t unknown56; // 0x56 - 0x57
  uint8_t unknown57; // 0x57 - 0x58
};

// size: 0x0000010C
uint32_t spyro_mat_mul(uint32_t m1, uint32_t m2, uint32_t dst)
{
  save_mat3(dst, mat3_mul(load_mat3(m1), load_mat3(m2)));
  return dst;
}

void function_800623D8(void)
{
  DEPRECATED;
  v0 = spyro_mat_mul(a0, a1, a2);
}

uint32_t spyro_mat_mul_2(uint32_t m1, uint32_t m2)
{
  return spyro_mat_mul(m1, m2, m1);
}

// size: 0x0000010C
void function_800624E8(void)
{
  DEPRECATED;
  v0 = spyro_mat_mul_2(a0, a1);
}

// size: 0x00000108
int16_t spyro_atan(int16_t x, int16_t y)
{
  int16_t xabs = abs_int(a0);
  int16_t yabs = abs_int(a1);
  int x_positive = x >= 0;
  int y_positive = y >= 0;
  int xgty = xabs >= yabs;

  int desc;
  int16_t atan;

  if (x_positive) {
    if (y_positive) {
      if (xgty) {
        desc = 0;
        atan = 0;
      } else {
        desc = 1;
        atan = 64;
      }
    } else {
      if (xgty) {
        desc = 1;
        atan = 256;
      } else {
        desc = 0;
        atan = 192;
      }
    }
  } else {
    if (y_positive) {
      if (xgty) {
        desc = 1;
        atan = 128;
      } else {
        desc = 0;
        atan = 64;
      }
    } else {
      if (xgty) {
        desc = 0;
        atan = 128;
      } else {
        desc = 1;
        atan = 192;
      }
    }
  }

  int16_t x2 = xabs;
  int16_t y2 = yabs;
  if (xgty) {
    int16_t tmp = x2;
    x2 = y2;
    y2 = tmp;
  }
  if (y2 == 0) y2 = 1;
  a0=x2*64/y2;
  assert(a0 < 64);
  v1 = atan_lut[a0];
  
  if (desc)
    atan -= v1;
  else
    atan += v1;

  return atan;
}

// size: 0x00000108
void function_800169AC(void)
{
  DEPRECATED;
  v0 = spyro_atan(a0, a1);
}

// size: 0x000001A4
int32_t spyro_atan2(int32_t x, int32_t y, uint32_t a3)
{
  int32_t xabs = abs_int(a0);
  int32_t yabs = abs_int(a1);
  int x_positive = (int32_t)a0 >= 0;
  int y_positive = (int32_t)a1 >= 0;
  int xgty = xabs >= yabs;

  int32_t x2 = xabs;
  int32_t y2 = yabs;
  
  v1 = 17 - lzcr(x2 | y2);
  if ((int32_t)v1 > 0) {
    x2 >>= v1;
    y2 >>= v1;
  }

  if (x2 >= y2) {
    int32_t tmp = x2;
    x2 = y2;
    y2 = tmp;
  }
  
  if (y2 == 0) return 0;

  div_psx(x2 << 16, y2);

  int desc;
  int16_t atan;

  if (x_positive) {
    if (y_positive) {
      if (xgty) {
        desc = 0;
        atan = 0;
      } else {
        desc = 1;
        atan = 64;
      }
    } else {
      if (xgty) {
        desc = 1;
        atan = 256;
      } else {
        desc = 0;
        atan = 192;
      }
    }
  } else {
    if (y_positive) {
      if (xgty) {
        desc = 1;
        atan = 128;
      } else {
        desc = 0;
        atan = 64;
      }
    } else {
      if (xgty) {
        desc = 0;
        atan = 128;
      } else {
        desc = 1;
        atan = 192;
      }
    }
  }


  a0=lo;
  size_t i = (a0 >> 11);

  do {
    i++;
    assert(i < 34);
    a2 = math_lut[i] - a0;
  } while ((int32_t)a2 < 0);
  i--;

  int32_t offset;
  if (a3) {
    div_psx(a2 << 16, math_lut2[i]);
    a1 = i*2;
    if (a1 != 64) a1++;
    atan *= 16;
    offset = a1*8 - (lo >> 12);
  } else {
    offset = i;
  }
  if (desc) offset = -offset;
  atan += offset;
  return atan;
}

// size: 0x000001A4
void function_80016AB4(void)
{
  DEPRECATED;
  v0 = spyro_atan2(a0, a1, a2);
}

// cos_lut and sin_lut overlap
int16_t cos_lut[256] = {
  0x1000, 0x0FFF, 0x0FFB, 0x0FF5, 0x0FEC, 0x0FE1, 0x0FD4, 0x0FC4,
  0x0FB1, 0x0F9C, 0x0F85, 0x0F6C, 0x0F50, 0x0F31, 0x0F11, 0x0EEE,
  0x0EC8, 0x0EA1, 0x0E77, 0x0E4B, 0x0E1C, 0x0DEC, 0x0DB9, 0x0D85, 
  0x0D4E, 0x0D15, 0x0CDA, 0x0C9D, 0x0C5E, 0x0C1E, 0x0BDB, 0x0B97, 
  0x0B50, 0x0B08, 0x0ABF, 0x0A73, 0x0A26, 0x09D8, 0x0988, 0x0937, 
  0x08E4, 0x088F, 0x083A, 0x07E3, 0x078B, 0x0732, 0x06D7, 0x067C, 
  0x061F, 0x05C2, 0x0564, 0x0505, 0x04A5, 0x0444, 0x03E3, 0x0381, 
  0x031F, 0x02BC, 0x0259, 0x01F5, 0x0191, 0x012D, 0x00C9, 0x0065, 
  0x0000, 0xFF9B, 0xFF37, 0xFED3, 0xFE6F, 0xFE0B, 0xFDA7, 0xFD44, 
  0xFCE1, 0xFC7F, 0xFC1D, 0xFBBC, 0xFB5B, 0xFAFB, 0xFA9C, 0xFA3E, 
  0xF9E1, 0xF984, 0xF929, 0xF8CE, 0xF875, 0xF81D, 0xF7C6, 0xF771, 
  0xF71C, 0xF6C9, 0xF678, 0xF628, 0xF5DA, 0xF58D, 0xF541, 0xF4F8, 
  0xF4B0, 0xF469, 0xF425, 0xF3E2, 0xF3A2, 0xF363, 0xF326, 0xF2EB, 
  0xF2B2, 0xF27B, 0xF247, 0xF214, 0xF1E4, 0xF1B5, 0xF189, 0xF15F, 
  0xF138, 0xF112, 0xF0EF, 0xF0CF, 0xF0B0, 0xF094, 0xF07B, 0xF064, 
  0xF04F, 0xF03C, 0xF02C, 0xF01F, 0xF014, 0xF00B, 0xF005, 0xF001, 
  0xF000, 0xF001, 0xF005, 0xF00B, 0xF014, 0xF01F, 0xF02C, 0xF03C,
  0xF04F, 0xF064, 0xF07B, 0xF094, 0xF0B0, 0xF0CF, 0xF0EF, 0xF112,
  0xF138, 0xF15F, 0xF189, 0xF1B5, 0xF1E4, 0xF214, 0xF247, 0xF27B,
  0xF2B2, 0xF2EB, 0xF326, 0xF363, 0xF3A2, 0xF3E2, 0xF425, 0xF469,
  0xF4B0, 0xF4F8, 0xF541, 0xF58D, 0xF5DA, 0xF628, 0xF678, 0xF6C9,
  0xF71C, 0xF771, 0xF7C6, 0xF81D, 0xF875, 0xF8CE, 0xF929, 0xF984,
  0xF9E1, 0xFA3E, 0xFA9C, 0xFAFB, 0xFB5B, 0xFBBC, 0xFC1D, 0xFC7F,
  0xFCE1, 0xFD44, 0xFDA7, 0xFE0B, 0xFE6F, 0xFED3, 0xFF37, 0xFF9B,
  0x0000, 0x0065, 0x00C9, 0x012D, 0x0191, 0x01F5, 0x0259, 0x02BC,
  0x031F, 0x0381, 0x03E3, 0x0444, 0x04A5, 0x0505, 0x0564, 0x05C2,
  0x061F, 0x067C, 0x06D7, 0x0732, 0x078B, 0x07E3, 0x083A, 0x088F,
  0x08E4, 0x0937, 0x0988, 0x09D8, 0x0A26, 0x0A73, 0x0ABF, 0x0B08,
  0x0B50, 0x0B97, 0x0BDB, 0x0C1E, 0x0C5E, 0x0C9D, 0x0CDA, 0x0D15,
  0x0D4E, 0x0D85, 0x0DB9, 0x0DEC, 0x0E1C, 0x0E4B, 0x0E77, 0x0EA1,
  0x0EC8, 0x0EEE, 0x0F11, 0x0F31, 0x0F50, 0x0F6C, 0x0F85, 0x0F9C,
  0x0FB1, 0x0FC4, 0x0FD4, 0x0FE1, 0x0FEC, 0x0FF5, 0x0FFB, 0x0FFF,
};

int16_t sin_lut[256] = {
  0x0000, 0x0065, 0x00C9, 0x012D, 0x0191, 0x01F5, 0x0259, 0x02BC,
  0x031F, 0x0381, 0x03E3, 0x0444, 0x04A5, 0x0505, 0x0564, 0x05C2,
  0x061F, 0x067C, 0x06D7, 0x0732, 0x078B, 0x07E3, 0x083A, 0x088F,
  0x08E4, 0x0937, 0x0988, 0x09D8, 0x0A26, 0x0A73, 0x0ABF, 0x0B08,
  0x0B50, 0x0B97, 0x0BDB, 0x0C1E, 0x0C5E, 0x0C9D, 0x0CDA, 0x0D15,
  0x0D4E, 0x0D85, 0x0DB9, 0x0DEC, 0x0E1C, 0x0E4B, 0x0E77, 0x0EA1,
  0x0EC8, 0x0EEE, 0x0F11, 0x0F31, 0x0F50, 0x0F6C, 0x0F85, 0x0F9C,
  0x0FB1, 0x0FC4, 0x0FD4, 0x0FE1, 0x0FEC, 0x0FF5, 0x0FFB, 0x0FFF,
  0x1000, 0x0FFF, 0x0FFB, 0x0FF5, 0x0FEC, 0x0FE1, 0x0FD4, 0x0FC4,
  0x0FB1, 0x0F9C, 0x0F85, 0x0F6C, 0x0F50, 0x0F31, 0x0F11, 0x0EEE,
  0x0EC8, 0x0EA1, 0x0E77, 0x0E4B, 0x0E1C, 0x0DEC, 0x0DB9, 0x0D85, 
  0x0D4E, 0x0D15, 0x0CDA, 0x0C9D, 0x0C5E, 0x0C1E, 0x0BDB, 0x0B97, 
  0x0B50, 0x0B08, 0x0ABF, 0x0A73, 0x0A26, 0x09D8, 0x0988, 0x0937, 
  0x08E4, 0x088F, 0x083A, 0x07E3, 0x078B, 0x0732, 0x06D7, 0x067C, 
  0x061F, 0x05C2, 0x0564, 0x0505, 0x04A5, 0x0444, 0x03E3, 0x0381, 
  0x031F, 0x02BC, 0x0259, 0x01F5, 0x0191, 0x012D, 0x00C9, 0x0065, 
  0x0000, 0xFF9B, 0xFF37, 0xFED3, 0xFE6F, 0xFE0B, 0xFDA7, 0xFD44, 
  0xFCE1, 0xFC7F, 0xFC1D, 0xFBBC, 0xFB5B, 0xFAFB, 0xFA9C, 0xFA3E, 
  0xF9E1, 0xF984, 0xF929, 0xF8CE, 0xF875, 0xF81D, 0xF7C6, 0xF771, 
  0xF71C, 0xF6C9, 0xF678, 0xF628, 0xF5DA, 0xF58D, 0xF541, 0xF4F8, 
  0xF4B0, 0xF469, 0xF425, 0xF3E2, 0xF3A2, 0xF363, 0xF326, 0xF2EB, 
  0xF2B2, 0xF27B, 0xF247, 0xF214, 0xF1E4, 0xF1B5, 0xF189, 0xF15F, 
  0xF138, 0xF112, 0xF0EF, 0xF0CF, 0xF0B0, 0xF094, 0xF07B, 0xF064, 
  0xF04F, 0xF03C, 0xF02C, 0xF01F, 0xF014, 0xF00B, 0xF005, 0xF001, 
  0xF000, 0xF001, 0xF005, 0xF00B, 0xF014, 0xF01F, 0xF02C, 0xF03C,
  0xF04F, 0xF064, 0xF07B, 0xF094, 0xF0B0, 0xF0CF, 0xF0EF, 0xF112,
  0xF138, 0xF15F, 0xF189, 0xF1B5, 0xF1E4, 0xF214, 0xF247, 0xF27B,
  0xF2B2, 0xF2EB, 0xF326, 0xF363, 0xF3A2, 0xF3E2, 0xF425, 0xF469,
  0xF4B0, 0xF4F8, 0xF541, 0xF58D, 0xF5DA, 0xF628, 0xF678, 0xF6C9,
  0xF71C, 0xF771, 0xF7C6, 0xF81D, 0xF875, 0xF8CE, 0xF929, 0xF984,
  0xF9E1, 0xFA3E, 0xFA9C, 0xFAFB, 0xFB5B, 0xFBBC, 0xFC1D, 0xFC7F,
  0xFCE1, 0xFD44, 0xFDA7, 0xFE0B, 0xFE6F, 0xFED3, 0xFF37, 0xFF9B,
};

int16_t spyro_cos(int32_t angle)
{
  angle = angle & 0xFFF;
  uint32_t remainder = angle & 0xF;
  uint32_t index = angle >> 4;

  int32_t cos = cos_lut[index];
  if (remainder) {
    uint32_t next = cos_lut[(index+1)&0xFF];
    cos += (int32_t)((int32_t)remainder*(next-cos)) >> 4;
  }
  return cos;
}

int16_t spyro_sin(int32_t angle)
{
  return spyro_cos(angle-1024);
}

// size: 0x00000058
void function_80016C58(void)
{
  DEPRECATED;
  v0 = spyro_sin(a0);
}

// size: 0x00000058
void function_80016CB0(void)
{
  DEPRECATED;
  v0 = spyro_cos(a0);
}

uint32_t spyro_log2_uint(uint32_t a)
{
  if (a == 0) return 0;
  cop2.LZCS = a;
  return 31 - LZCR();
}

// size: 0x00000024
void function_80016D08(void)
{
  DEPRECATED;
  v0 = spyro_log2_uint(a0);
}

void spyro_mat3_rotation(uint32_t rot_addr, uint32_t dst, uint32_t src)
{
  uint32_t rot = lw(rot_addr);
  mat3 m;
  if (src) {
    m = load_mat3(src);
  } else {
    m = mat3_identity();
  }

  uint32_t rotY = (rot >> 16) & 0xFF;
  if (rotY) m = mat3_mul(m, mat3rotY(-rotY*16));

  uint32_t rotX = (rot >> 8) & 0xFF;
  if (rotX) m = mat3_mul(m, mat3rotX(rotX*16));
  
  uint32_t rotZ = (rot << 0) & 0xFF;
  if (rotZ) m = mat3_mul(m, mat3rotZ(-rotZ*16));

  save_mat3(dst, m);
}

// size: 0x000002A4
// create rotation matrix
// or rotate matrix with
// given rotation vector
void function_80016D2C(void)
{
  DEPRECATED;
  spyro_mat3_rotation(a0, a1, a2);
}

void spyro_mat3_transpose(uint32_t dst, uint32_t src)
{
  mat3 m = load_mat3(src);
  m = mat3_transpose(m);
  save_mat3(dst, m);
}

// size: 0x00000078
void function_80016FD0(void)
{
  DEPRECATED;
  spyro_mat3_transpose(a0, a1);
}

void spyro_set_mat_mirrored_vec_multiply(uint32_t mat, uint32_t vec_src, uint32_t vec_dst)
{
  load_RTM(mat);
  mat3 m = get_RTM();

  vec3_32 v = {
    .z =  lw(vec_src + 0),
    .x = -lw(vec_src + 4),
    .y = -lw(vec_src + 8)
  };

  v = vec3_32_mat_mul(v, m);
  sw(vec_dst + 0,  v.z);
  sw(vec_dst + 4, -v.x);
  sw(vec_dst + 8, -v.y);
}

// size: 0x00000078
void function_80017048(void)
{
  DEPRECATED;
  spyro_set_mat_mirrored_vec_multiply(a0, a1, a2);
}

void spyro_mat_mirrored_vec_multiply(uint32_t vec_src, uint32_t vec_dst)
{
  mat3 m = get_RTM();

  vec3_32 v = {
    .z =  lw(vec_src + 0),
    .x = -lw(vec_src + 4),
    .y = -lw(vec_src + 8)
  };

  v = vec3_32_mat_mul(v, m);
  sw(vec_dst + 0,  v.z);
  sw(vec_dst + 4, -v.x);
  sw(vec_dst + 8, -v.y);
}

// size: 0x00000050
void function_800170C0(void)
{
  DEPRECATED;
  spyro_mat_mirrored_vec_multiply(a0, a1);
}

void spyro_camera_mat_vec_multiply(vec3_32 *src, vec3_32 *dst)
{
  load_RTM(0x80076DD0 + 0x14);
  mat3 m = get_RTM();

  vec3_32 v = {
    .z =  src->x,
    .x = -src->y,
    .y = -src->z
  };

  v = vec3_32_mat_mul(v, m);
  dst->x = v.x;
  dst->y = v.y;
  dst->z = v.z;
}

// size: 0x00000078
void function_80017110(void)
{
  DEPRECATED;
  spyro_camera_mat_vec_multiply(addr_to_pointer(a0), addr_to_pointer(a1));
}

// size: 0x000000A0
// vector length
uint32_t spyro_vec_length(uint32_t vec, uint32_t is_vec3)
{
  int32_t x = lh(vec + 0);
  int32_t y = lh(vec + 4);
  int32_t z = 0;
  if (is_vec3)
    z = lh(vec + 8);

  uint32_t sqr = x*x + y*y + z*z;
  return spyro_sqrt(sqr);
}

void function_800171FC(void)
{
  DEPRECATED;
  v0 = spyro_vec_length(a0, a1);
}

// size: 0x00000094
uint32_t math_func1(vec3_32 *v_src, int32_t n, uint32_t v3)
{
  vec3_32 v;
  v.x = v_src->x;
  v.y = v_src->y;
  if (v3) v.z = v_src->z;
  else    v.z = 0;

  uint32_t n2 = n*n;

  uint32_t d2 = v.x*v.x + v.y*v.y + v.z*v.z;

  uint32_t a = (32 - lzcr(n2))/2;

  uint32_t b = n2 >> a;

  if (a > 6) {
    int32_t c = d2/b - (1 << a);
    c /= 2;
    int32_t d = c*n >> a;
    return n + d;
  } else {
    return b;
  }
}

// size: 0x00000094
void function_8001729C(void)
{
  DEPRECATED;
  v0 = math_func1(addr_to_pointer(a0), a1, a2);
}

// size: 0x000000F8
void spyro_set_vec3_length(uint32_t vec, int32_t len)
{
  int32_t x = lw(vec + 0);
  int32_t y = lw(vec + 4);
  int32_t z = lw(vec + 8);
  int32_t dist = x*x + y*y + z*z;
  if (dist == 0) {
    sw(vec + 0, 0);
    sw(vec + 4, 0);
    sw(vec + 8, 0);
    return;
  }
  dist = spyro_sqrt(dist);
  len = len << 12;
  int32_t scale = len/dist;
  sw(vec + 0, x*scale >> 12);
  sw(vec + 4, y*scale >> 12);
  sw(vec + 8, z*scale >> 12);
}

void function_80017330(void)
{
  DEPRECATED;
  spyro_set_vec3_length(a0, a1);
}

// size: 0x00000120
void function_80017428(void)
{
  at = (int32_t)lw(a1 + 0) >> 4;
  v0 = (int32_t)lw(a1 + 4) >> 4;
  v1 = (int32_t)lw(a1 + 8) >> 4;
  a3 = -lw(a0 + 0);
  t0 = -lw(a0 + 4);
  t1 = -lw(a0 + 8);
  cop2.RTM0 = (a3 & 0xFFFF) + (t0 << 16);
  cop2.RTM1 = t1;
  cop2.IR1 = at;
  cop2.IR2 = v0;
  cop2.IR3 = v1;
  MVMVA(SF_OFF, MX_RT, V_IR, CV_NONE, LM_OFF);
  t2 = cop2.MAC1;
  if ((int32_t)t2 < 0) {
    sw(a2 + 0, -a3);
    sw(a2 + 4, -t0);
    sw(a2 + 8, -t1);
    v0 = 0;
    return;
  }
  div_psx(t2 << 6,at*at+v0*v0+v1*v1);
  at *= lo;
  v0 *= lo;
  v1 *= lo;
  at = (int32_t)at >> 6;
  v0 = (int32_t)v0 >> 6;
  v1 = (int32_t)v1 >> 6;
  sw(a2 + 0, at*2 - a3);
  sw(a2 + 4, v0*2 - t0);
  sw(a2 + 8, v1*2 - t1);
  v0 = 1;
}

void vec3_mul_div(uint32_t vec, int32_t div, int32_t mul)
{
  if (div == 0) {
    sw(vec + 0, 0);
    sw(vec + 4, 0);
    sw(vec + 8, 0);
    return;
  }
  int32_t q = (mul << 12) / div;
  sw(vec + 0, (int32_t)lh(vec + 0)*q >> 12);
  sw(vec + 4, (int32_t)lh(vec + 4)*q >> 12);
  sw(vec + 8, (int32_t)lh(vec + 8)*q >> 12);
  return;
}

// size: 0x0000005C
void function_800175B8(void)
{
  DEPRECATED;
  vec3_mul_div(a0, a1, a2);
}

// size: 0x0000008C
void spyro_vec3_multiply_fancy_shift_right(uint32_t vec, uint32_t mult, uint32_t shift)
{
  at = (int32_t)lw(vec + 0)*(int32_t)mult;
  v0 = (int32_t)lw(vec + 4)*(int32_t)mult;
  v1 = (int32_t)lw(vec + 8)*(int32_t)mult;
  if ((int32_t)at >= 0)
    at = at >> shift;
  else
    at = -((-at) >> shift);

  if ((int32_t)v0 >= 0) 
    v0 = v0 >> shift;
  else
    v0 = -((-v0) >> shift);

  if ((int32_t)v1 >= 0) 
    v1 = v1 >> shift;
  else
    v1 = -((-v1) >> shift);

  sw(a0 + 0, at);
  sw(a0 + 4, v0);
  sw(a0 + 8, v1);
  return;
}

void function_80017614(void)
{
  DEPRECATED;
  spyro_vec3_multiply_fancy_shift_right(a0, a1, a2);
}

void spyro_vec3_shift_left(uint32_t vec, uint32_t shift)
{
  sw(vec + 0, lw(vec + 0) << shift);
  sw(vec + 4, lw(vec + 4) << shift);
  sw(vec + 8, lw(vec + 8) << shift);
}

// size: 0x00000028
void function_800176A0(void)
{
  DEPRECATED;
  spyro_vec3_shift_left(a0, a1);
}

void spyro_vec3_shift_right(uint32_t vec, uint32_t shift)
{
  sw(vec + 0, (int32_t)lw(vec + 0) >> shift);
  sw(vec + 4, (int32_t)lw(vec + 4) >> shift);
  sw(vec + 8, (int32_t)lw(vec + 8) >> shift);
}

// size: 0x00000028
void function_800176C8(void)
{
  DEPRECATED;
  spyro_vec3_shift_right(a0, a1);
}

void spyro_vec3_clear(uint32_t vec)
{
  sw(vec + 0, 0);
  sw(vec + 4, 0);
  sw(vec + 8, 0);
}

// size: 0x00000010
void function_800176F0(void)
{
  DEPRECATED;
  spyro_vec3_clear(a0);
}

void spyro_vec3_copy(uint32_t dst, uint32_t src)
{
  sw(dst + 0, lw(src + 0));
  sw(dst + 4, lw(src + 4));
  sw(dst + 8, lw(src + 8));
}

// size: 0x0000001C
void function_80017700(void)
{
  DEPRECATED;
  spyro_vec3_copy(a0, a1);
}

void spyro_vec3_add(uint32_t dst, uint32_t a, uint32_t b)
{
  sw(dst + 0, lw(a + 0) + lw(b + 0));
  sw(dst + 4, lw(a + 4) + lw(b + 4));
  sw(dst + 8, lw(a + 8) + lw(b + 8));
}

// size: 0x00000034
void function_80017758(void)
{
  DEPRECATED;
  spyro_vec3_add(a0, a1, a2);
}

void spyro_vec3_sub(uint32_t dst, uint32_t a, uint32_t b)
{
  sw(dst + 0, lw(a + 0) - lw(b + 0));
  sw(dst + 4, lw(a + 4) - lw(b + 4));
  sw(dst + 8, lw(a + 8) - lw(b + 8));
}

// size: 0x00000034
void function_8001778C(void)
{
  DEPRECATED;
  spyro_vec3_sub(a0, a1, a2);
}

void spyro_vec3_mul(uint32_t dst, uint32_t src, int32_t mul)
{
  sw(dst + 0, (int32_t)lw(src + 0)*mul);
  sw(dst + 4, (int32_t)lw(src + 4)*mul);
  sw(dst + 8, (int32_t)lw(src + 8)*mul);
}

// size: 0x00000038
void function_800177C0(void)
{
  DEPRECATED;
  spyro_vec3_mul(a0, a1, a2);
}

void spyro_vec3_div(uint32_t dst, uint32_t src, int32_t div)
{
  sw(dst + 0, (int32_t)lw(src + 0)/div);
  sw(dst + 4, (int32_t)lw(src + 4)/div);
  sw(dst + 8, (int32_t)lw(src + 8)/div);
}

// size: 0x0000005C
void function_800177F8(void)
{
  DEPRECATED;
  spyro_vec3_div(a0, a1, a2);
}

void spyro_vec_interpolation(uint32_t dst, uint32_t vec1, uint32_t vec2, int32_t ipol)
{
  sw(dst + 0, ((int32_t)lw(vec2 + 0)*ipol + (int32_t)lw(vec1 + 0)*(0x1000-ipol)) >> 12);
  sw(dst + 4, ((int32_t)lw(vec2 + 4)*ipol + (int32_t)lw(vec1 + 4)*(0x1000-ipol)) >> 12);
  sw(dst + 8, ((int32_t)lw(vec2 + 8)*ipol + (int32_t)lw(vec1 + 8)*(0x1000-ipol)) >> 12);
}

// size: 0x00000074
void function_80017894(void)
{
  DEPRECATED;
  spyro_vec_interpolation(a0, a1, a2, a3);
}

uint32_t spyro_two_angle_diff_8bit(uint32_t a, uint32_t b)
{
  uint32_t diff = a - b;
  diff = diff & 0xFF;
  if (diff >= 0x80)
    diff = 0x100 - diff;
  return diff;
}

// size: 0x00000020
// zigzag function 0-128-0-128
void function_80017908(void)
{
  DEPRECATED;
  v0 = spyro_two_angle_diff_8bit(a0, a1);
}

uint32_t spyro_two_angle_diff_12bit(uint32_t a, uint32_t b)
{
  uint32_t diff = a0 - a1;
  diff = diff & 0xFFF;
  if (diff >= 0x800)
    diff = 0x1000 - diff;
  return diff;
}

// size: 0x00000020
void function_80017928(void)
{
  DEPRECATED;
  v0 = spyro_two_angle_diff_12bit(a0, a1);
}

// size: 0x00000024
uint32_t spyro_two_angle_signed_diff_8bit(int32_t a, int32_t b)
{

  uint32_t diff = (a - b) & 0xFF;
  if (diff > 0x80)
    return diff - 0x100;
  return diff;
}

// size: 0x00000024
void function_80017948(void)
{
  DEPRECATED;
  v0 = spyro_two_angle_signed_diff_8bit(a0, a1);
}

// size: 0x00000024
uint32_t spyro_two_angle_signed_diff_12bit(int32_t a, int32_t b)
{
  uint32_t diff = (a - b) & 0xFFF;
  if (diff > 0x800)
    return diff - 0x1000;
  return diff;
}

// size: 0x00000024
void function_8001796C(void)
{
  DEPRECATED;
  v0 = spyro_two_angle_signed_diff_12bit(a0, a1);
}

uint32_t spyro_octagon_distance(uint32_t a, uint32_t b)
{
  at = lw(a + 0);
  v0 = lw(a + 4);
  a2 = lw(b + 0);
  a3 = lw(b + 4);
  a2 = abs_int(a2 - at);
  a3 = abs_int(a3 - v0);
  if ((int32_t)a2 < (int32_t)a3) {
    v0 = a2*3/8 + a3;
  } else {
    v0 = a2 + a3*3/8;
  }
  return v0;
}

// size: 0x00000060
// octagon distance between two points
void function_80017990(void)
{
  DEPRECATED;
  v0 = spyro_octagon_distance(a0, a1);
}

// size: 0x00000048
uint32_t spyro_attract_angle_in_range(uint32_t a, uint32_t b, int32_t attraction, int32_t range)
{
  int32_t diff = (a - b) & 0xFF;
  if (diff > 0x80) {
    if (0x100 - diff < range)
      return b & 0xFF;
    return (b - attraction) & 0xFF;
  }

  if (diff < range)
    return b & 0xFF;
  return (b + attraction) & 0xFF;
}

void function_800179F0(void)
{
  DEPRECATED;
  v0 = spyro_attract_angle_in_range(a0, a1, a2, a3);
}

// size: 0x0000006C
uint32_t spyro_sqrt(uint32_t a)
{
  uint32_t a1, at, a3;
  if (a == 0)
    return 0;
  a1 = ulzcr(a) & ~1;
  at = (31 - a1)/2;
  if (a1 >= 24) {
    a3 = a << (a1 - 24);
  } else {
    a3 = a >> (24 - a1);
  }


  uint32_t i = a3-64;
  if (i > 192) {
    BREAKPOINT;
  }

  return (sqrt_lookup[i] << at) >> 12;
}

void function_80017A38(void)
{
  DEPRECATED;
  v0 = spyro_sqrt(a0);
}

// size: 0x000000A4
void spyro_world_to_screen_projection(uint32_t dst, uint32_t vec)
{
  load_RTM(0x80076DD0);
  set_TR(0,0,0);
  a2 = lw(0x80076DD0 + 0x28);
  t0 = lw(0x80076DD0 + 0x2C);
  a3 = lw(0x80076DD0 + 0x30);
  at = lw(vec + 0);
  v0 = lw(vec + 4);
  v1 = lw(vec + 8);
  at =   at - a2;
  v0 = -(v0 - t0);
  v1 = -(v1 - a3);
  cop2.VXY0 = (v0 & 0xFFFF) | (v1 << 16);
  cop2.VZ0 = at;
  RTPS();
  v1 = cop2.MAC3;
  v0 = (int32_t)(cop2.SXY2 << 16) >> 16;
  at = (int32_t)cop2.SXY2 >> 16;
  sw(dst + 0, v0);
  sw(dst + 4, at);
  sw(dst + 8, v1);
}

void function_80017AA4(void)
{
  DEPRECATED;
  spyro_world_to_screen_projection(a0, a1);
}

// size: 0x000000B4
void spyro_world_to_screen_projection_with_right_shift(uint32_t dst, uint32_t vec, uint32_t shift)
{
  a3 = 0x80076DD0;
  load_RTM(a3);
  set_TR(0,0,0);
  t1 = lw(a3 + 0x28);
  t0 = lw(a3 + 0x30);
  a3 = lw(a3 + 0x2C);
  at = lw(vec + 0);
  v0 = lw(vec + 4);
  v1 = lw(vec + 8);
  at =   at - t1;
  v0 = -(v0 - a3);
  v1 = -(v1 - t0);
  at = (int32_t)at >> shift;
  v0 = (int32_t)v0 >> shift;
  v1 = (int32_t)v1 >> shift;
  cop2.VXY0 = (v0 & 0xFFFF) | (v1 << 16);
  cop2.VZ0 = at;
  RTPS();
  v0 = (int32_t)(cop2.SXY2 << 16) >> 16;
  at = (int32_t)cop2.SXY2 >> 16;
  v1 = cop2.MAC3 << shift;
  sw(dst + 0, v0);
  sw(dst + 4, at);
  sw(dst + 8, v1);
}

void function_80017B48(void)
{
  DEPRECATED;
  spyro_world_to_screen_projection_with_right_shift(a0, a1, a2);
}

void spyro_vec_32_to_16_div_4(uint32_t dst, uint32_t src)
{
  sh(dst + 0, (int32_t)lw(src + 0) >> 2);
  sh(dst + 2, (int32_t)lw(src + 4) >> 2);
  sh(dst + 4, (int32_t)lw(src + 8) >> 2);
}

// size: 0x00000028
void function_80017BFC(void)
{
  DEPRECATED;
  spyro_vec_32_to_16_div_4(a0, a1);
}

void spyro_vec_16_to_32_mul_4(uint32_t dst, uint32_t src)
{
  sw(dst + 0, lhu(src + 0) << 2);
  sw(dst + 4, lhu(src + 2) << 2);
  sw(dst + 8, lhu(src + 4) << 2);
}

// size: 0x00000028
void function_80017C24(void)
{
  DEPRECATED;
  spyro_vec_16_to_32_mul_4(a0, a1);
}

void spyro_vec_16_to_32(uint32_t dst, uint32_t src)
{
  sw(dst + 0, lh(src + 0));
  sw(dst + 4, lh(src + 2));
  sw(dst + 8, lh(src + 4));
}

// size: 0x0000001C
void function_80017C4C(void)
{
  DEPRECATED;
  spyro_vec_16_to_32(a0, a1);
}

void spyro_vec_32_to_16(vec3 *dst, vec3_32 *src)
{
  dst->x = src->x;
  dst->y = src->y;
  dst->z = src->z;
}

// size: 0x0000001C
void function_80017C68(void)
{
  DEPRECATED;
  spyro_vec_32_to_16(addr_to_pointer(a0), addr_to_pointer(a1));
}

void spyro_vec_16_add(uint32_t dst, uint32_t a, uint32_t b)
{
  sh(dst + 0, lh(a + 0) + lh(b + 0));
  sh(dst + 2, lh(a + 2) + lh(b + 2));
  sh(dst + 4, lh(a + 4) + lh(b + 4));
}

// size: 0x00000034
void function_80017C84(void)
{
  DEPRECATED;
  spyro_vec_16_add(a0, a1, a2);
}

// size: 0x000000C4
void spyro_unpack_96bit_triangle(uint32_t index, uint32_t dst)
{
  at = lw(lw(0x800785A8 + 0x2C) + 0x10) + index * 0x0C;
  v0 = lw(at + 0);
  v1 = lw(at + 4);
  a0 = lw(at + 8);

  t0 = (int32_t)(v0 << 0) >> 23; // 23:31
  a3 = (int32_t)(v0 << 9) >> 23; // 14:22
  a2 = v0 & 0x3FFF; // 0:13
  a3 += a2;
  t0 += a2;
  sw(dst + 0x00, a2 << 4);
  sw(dst + 0x0C, a3 << 4);
  sw(dst + 0x18, t0 << 4);

  t0 = (int32_t)(v1 << 0) >> 23; // 23:31
  a3 = (int32_t)(v1 << 9) >> 23; // 14:22
  a2 = v1 & 0x3FFF; // 0:13
  a3 += a2;
  t0 += a2;
  sw(dst + 0x04, a2 << 4);
  sw(dst + 0x10, a3 << 4);
  sw(dst + 0x1C, t0 << 4);

  t0 = (a0 << 0) >> 24; // 24:31
  a3 = (a0 << 8) >> 24; // 16:23
  a2 = a0 & 0x3FFF; // 0:13
  a3 += a2;
  t0 += a2;
  sw(dst + 0x08, a2 << 4);
  sw(dst + 0x14, a3 << 4);
  sw(dst + 0x20, t0 << 4);
}

void function_80017CB8(void)
{
  DEPRECATED;
  spyro_unpack_96bit_triangle(a0, a1);
}

// size: 0x000000D8
void function_80017D7C(void)
{
  uint32_t temp;
  t0 = lw(a0 + 0x0000);
  t1 = lw(a1 + 0x0000);
  t2 = lw(a0 + 0x0004);
  t3 = lw(a1 + 0x0004);
  a3 = a3 >> 1;
  cop2.LZCS = a3;
  t4 = lw(a0 + 0x0008);
  t5 = lw(a1 + 0x0008);
  v0 = LZCR();
  at = 32; // 0x0020
  temp = v0 == at;
  t7 = at - v0;
  if (temp) goto label80017E4C;
  at = t1 - t0;
  temp = (int32_t)at >= 0;
  v0 = t3 - t2;
  if (temp) goto label80017DBC;
  at = -at;
label80017DBC:
  temp = (int32_t)v0 >= 0;
  v1 = t5 - t4;
  if (temp) goto label80017DC8;
  v0 = -v0;
label80017DC8:
  temp = (int32_t)v1 >= 0;
  a3 = at - v0;
  if (temp) goto label80017DD4;
  v1 = -v1;
label80017DD4:
  temp = (int32_t)a3 >= 0;
  if (temp) goto label80017DE0;
  at = v0;
label80017DE0:
  a3 = at - v1;
  temp = (int32_t)a3 >= 0;

  if (temp) goto label80017DF0;
  at = v1;
label80017DF0:
  t7 = at >> t7;
  t6 = 1; // 0x0001
  t6 = t6 << 12;
  div_psx(t6,t7);
  at = t1 - t0;
  cop2.IR1 = at;
  v0 = t3 - t2;
  cop2.IR2 = v0;
  v1 = t5 - t4;
  cop2.IR3 = v1;
  t7=lo;
  cop2.IR0 = t7;
  v0 = t7;
  GPF(SF_OFF, LM_OFF);
  t0 = cop2.MAC1;
  t1 = cop2.MAC2;
  t0 = (int32_t)t0 >> 12;
  t1 = (int32_t)t1 >> 12;
  t2 = cop2.MAC3;
  sw(a2 + 0x0000, t0);
  t2 = (int32_t)t2 >> 12;
  sw(a2 + 0x0004, t1);
label80017E4C:
  sw(a2 + 0x0008, t2);
}

uint32_t interpolate_color(uint32_t c1, uint32_t c2, int32_t ipol)
{
  int32_t r1 = (c1 >>  0) & 0xFF;
  int32_t g1 = (c1 >>  8) & 0xFF;
  int32_t b1 = (c1 >> 16) & 0xFF;

  int32_t r2 = (c2 >>  0) & 0xFF;
  int32_t g2 = (c2 >>  8) & 0xFF;
  int32_t b2 = (c2 >> 16) & 0xFF;

  int32_t dr = r2-r1;
  int32_t dg = g2-g1;
  int32_t db = b2-b1;

  int32_t r = (r1 + dr*ipol/4096);
  int32_t g = (g1 + dg*ipol/4096);
  int32_t b = (b1 + db*ipol/4096);

  if (r > 255) r = 255;
  if (g > 255) g = 255;
  if (b > 255) b = 255;
  if (r < 0) r = 0;
  if (g < 0) g = 0;
  if (b < 0) b = 0;

  uint32_t c3 = r | (g << 8) | (b << 16);
  return c3;
}

// size: 0x00000044
void function_80017E54(void)
{
  DEPRECATED;
  v0 = interpolate_color(a0, a1, a2);
}

int32_t spyro_two_angle_add(int32_t angle1, int32_t angle2)
{
  return (a0 + a1) % 0x100;
}

// size: 0x00000024
void function_80038074(void)
{
  DEPRECATED;
  v0 = spyro_two_angle_add(a0, a1);
}

int32_t spyro_two_angle_diff_8bit2(int32_t angle1, int32_t angle2)
{
  int32_t angle = angle1 - angle2;
  if ((int32_t)angle >= 0x80)
    angle -= 0x100;
  if ((int32_t)angle < -0x80)
    angle += 0x100;
  return angle;
}

// size: 0x0000002C
void function_800381BC(void)
{
  DEPRECATED;
  v0 = spyro_two_angle_diff_8bit2(a0, a1);
}

int32_t spyro_two_angle_idk(uint32_t a, uint32_t b, uint32_t c)
{
  return spyro_two_angle_add(b, spyro_two_angle_diff_8bit2(b, a)*(c & 0xFF) >> 8);
}

// size: 0x00000058
void function_80038120(void)
{
  v0 = spyro_two_angle_idk(a0, a1, a2);
}

// size: 0x00000324
// struct game_object *a0
void function_800526A8(void)
{
  struct game_object *object = addr_to_pointer(a0);

  at = lw(0x80076378 + object->modelID*4);
  v0 = lw(at + 0x38 + object->unknown3C*4);
  v1 = lw(v0 + 0x24 + object->unknown3E*8);
  object->unknown41 = lbu(v0 + 0x0C);
  v1 = (v1 >> 19) & 0x1C;
  v0 = at + v1;
  v0 = lw(v0 + 0x0014);
  at = lw(at + 0x0014);

  object->unknown08 = v0;
  object->unknown34 = -1;
  if (at) {
    
    a1 = lw(0x80075778);
    v0 = (object->p.x >> 13) + (object->p.y >> 13)*32;
    a2 = 0;
    if ((int32_t)at >= 0)
      a2 = 0x400;
  
    v0 += a2;
    sh(a0 + 0x34, v0);
    v0 = v0 << 2;
    v0 += a1;
    v1 = lw(v0 + 0x0000);
    sw(v0 + 0x0000, a0);
    sw(a0 + 0x0004, v1);
  }
  t7 = a0;
  at = lw(t7 + 0x0044);
  v0 = spyro_sin_lut;
  v1 = spyro_cos_lut;
  set_RTM(mat3_identity());
  set_TR(0, 0, 0);
  a0 = (at >> 16) & 0xFF;
  if (a0) {
    a1 = cos_lut[a0];
    a0 = sin_lut[a0];
    cop2.VXY0 = a1 & 0xFFFF;
    cop2.VZ0 = a0;
    MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
    a0 = (-a0) & 0xFFFF;
    a1 = a1 & 0xFFFF;
    a3 = cop2.IR1;
    t0 = cop2.IR2;
    t1 = cop2.IR3;
    cop2.VXY0 = a0;
    cop2.VZ0 = a1;
    a2 = 0xFFFF0000;
    MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
    t2 = t2 & a2;
    a3 = a3 & 0xFFFF;
    t2 += a3;
    t5 = t5 & a2;
    t1 = t1 & 0xFFFF;
    t5 += t1;
    t0 = t0 << 16;
    t4 = t4 & 0xFFFF;
    a0 = cop2.IR1;
    a1 = cop2.IR2;
    a2 = cop2.IR3;
    a0 = a0 & 0xFFFF;
    t3 = a0 + t0;
    a1 = a1 << 16;
    t4 += a1;
    t6 = a2 & 0xFFFF;
    cop2.RTM0 = t2;
    cop2.RTM1 = t3;
    cop2.RTM2 = t4;
    cop2.RTM3 = t5;
    cop2.RTM4 = t6;
  }

  a0 = (a0 >> 8) & 0xFF;
  if (a0) {
    a1 = cos_lut[a0];
    a0 = sin_lut[a0];
    a2 = a1 << 16;
    cop2.VXY0 = a2;
    cop2.VZ0 = a0;
    MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
    a2 = a0 << 16;
    a2 = -a2;
    a3 = cop2.IR1;
    t0 = cop2.IR2;
    t1 = cop2.IR3;
    cop2.VXY0 = a2;
    cop2.VZ0 = a1;
    a2 = 0xFFFF0000;
    MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
    t2 = t2 & 0xFFFF;
    a3 = a3 << 16;
    t2 += a3;
    t5 = t5 & 0xFFFF;
    t1 = t1 << 16;
    t5 += t1;
    t0 = t0 & 0xFFFF;
    t3 = t3 & a2;
    a0 = cop2.IR1;
    a1 = cop2.IR2;
    a2 = cop2.IR3;
    a0 = a0 & 0xFFFF;
    t3 += a0;
    a1 = a1 << 16;
    t4 = a1 + t0;
    t6 = a2 & 0xFFFF;
    cop2.RTM0 = t2;
    cop2.RTM1 = t3;
    cop2.RTM2 = t4;
    cop2.RTM3 = t5;
    cop2.RTM4 = t6;
  }

  a0 = (at >> 0) & 0xFF;
  if (a0) {
    a0 = a0 << 1;
    a1 = a0 + v1;
    a0 += v0;
    a1 = lhu(a1 + 0x0000);
    a0 = lhu(a0 + 0x0000);
    cop2.VZ0 = 0;
    a2 = a0 << 16;
    a2 += a1;
    cop2.VXY0 = a2;
    MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
    a0 = -a0;
    a0 = a0 & 0xFFFF;
    a1 = a1 << 16;
    a0 += a1;
    a3 = cop2.IR1;
    t0 = cop2.IR2;
    t1 = cop2.IR3;
    cop2.VXY0 = a0;
    cop2.VZ0 = 0;
    MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
    t0 = t0 << 16;
    t3 = t3 & 0xFFFF;
    t3 += t0;
    a3 = a3 & 0xFFFF;
    t1 = t1 & 0xFFFF;
    t4 = t4 & 0xFFFF0000;
    a0 = cop2.IR1 << 16;
    t2 = a0 + a3;
    a2 = cop2.IR3 << 16;
    t5 = a2 + t1;
    a1 = cop2.IR2 & 0xFFFF;
    t4 += a1;
  }

  sw(t7 + 0x20, t2);
  sw(t7 + 0x24, t3);
  sw(t7 + 0x28, t4);
  sw(t7 + 0x2C, t5);
  sw(t7 + 0x30, t6);
  return;
}

// size: 0x00000380
// struct game_object *a0
void function_800529E4(void)
{
  uint32_t temp;
  at = a1 & 1;
  temp = at == 0;
  if (temp) goto label80052A64;
  at = lhu(a0 + 0x36);
  v0 = 0x80076378;
  at = at << 2;
  at += v0;
  v0 = lbu(a0 + 0x40);
  v1 = lw(a0 + 0x3C);
  at = lw(at);
  temp = (int32_t)v0 > 0;
  v0 = v1 & 0xFF;
  if (temp) goto label80052A24;
  v1 = v1 >> 16;
  v1 = v1 & 0xFF;
  goto label80052A30;
label80052A24:
  v0 = v1 >> 8;
  v0 = v0 & 0xFF;
  v1 = v1 >> 24;
label80052A30:
  v0 = lw(at + 0x38 + v0*4) + v1*8;
  v1 = (lw(v0 + 0x24) >> 19) & 0x1C;
  v0 = at + v1;
  v0 = lw(v0 + 0x0014);
  sw(a0 + 0x0008, v0);
label80052A64:
  at = a1 & 0x2;
  temp = at == 0;
  if (temp) goto label80052AEC;
  at = lw(a0 + 0x0C);
  v0 = lw(a0 + 0x10);
  v1 = lw(0x80075778);
  at = at >> 13;
  v0 = v0 >> 13;
  v0 = v0 << 5;
  a2 = lh(a0 + 0x34);
  at += v0;
  a3 = a2 & 0x400;
  at = at | a3;
  temp = at == a2;
  a2 = a2 << 2;
  if (temp) goto label80052AEC;
  temp = (int32_t)a2 < 0;
  a2 += v1;
  if (temp) goto label80052AEC;
  v0 = a2 - 4; // 0xFFFFFFFC
label80052AB4:
  a2 = v0 + 4; // 0x0004
  v0 = lw(a2 + 0x0000);
  temp = v0 != a0;
  if (temp) goto label80052AB4;
  v0 = lw(a0 + 0x0004);
  sw(a2 + 0x0000, v0);
  sh(a0 + 0x0034, at);
  at = v1 + at*4;
  v0 = lw(at + 0x0000);
  sw(at + 0x0000, a0);
  sw(a0 + 0x0004, v0);
label80052AEC:
  at = a1 & 0x4;
  temp = at == 0;
  t7 = a0;
  if (temp) goto label80052D5C;
  t4 = lw(t7 + 0x0044);
  set_RTM(mat3_identity());
  a3 = cop2.RTM0;
  t0 = cop2.RTM1;
  t1 = cop2.RTM2;
  t2 = cop2.RTM3;
  t3 = cop2.RTM4;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  at = (t4 >> 16) & 0xFF;
  if (at) {
    v0 = cos_lut[at];
    at = sin_lut[at];
    v0 = v0 & 0xFFFF;
    cop2.VXY0 = v0;
    cop2.VZ0 = at;
    MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
    at = (-at) & 0xFFFF;
    a0 = cop2.IR1;
    a1 = cop2.IR2;
    a2 = cop2.IR3;
    cop2.VXY0 = at;
    cop2.VZ0 = v0;
    v1 = 0xFFFF0000;
    MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
    a3 = a3 & v1;
    a0 = a0 & 0xFFFF;
    a3 += a0;
    t2 = t2 & v1;
    a2 = a2 & 0xFFFF;
    t2 += a2;
    a1 = a1 << 16;
    t1 = t1 & 0xFFFF;
    at = cop2.IR1;
    v0 = cop2.IR2;
    v1 = cop2.IR3;
    at = at & 0xFFFF;
    t0 = at + a1;
    v0 = v0 << 16;
    t1 += v0;
    t3 = v1 & 0xFFFF;
    cop2.RTM0 = a3;
    cop2.RTM1 = t0;
    cop2.RTM2 = t1;
    cop2.RTM3 = t2;
    cop2.RTM4 = t3;
  }
  at = t4 & 0xFF00;
  temp = at == 0;
  at = at >> 8;
  if (temp) goto label80052CA4;
  at = at << 1;
  v0 = at + spyro_cos_lut;
  at = at + spyro_sin_lut;
  v0 = lhu(v0 + 0x0000);
  at = lhu(at + 0x0000);
  v1 = v0 << 16;
  cop2.VXY0 = v1;
  cop2.VZ0 = at;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  a0 = cop2.IR1;
  a1 = cop2.IR2;
  a2 = cop2.IR3;
  cop2.VXY0 = -(at << 16);
  cop2.VZ0 = v0;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  a3 = a3 & 0xFFFF;
  a0 = a0 << 16;
  a3 += a0;
  t2 = t2 & 0xFFFF;
  a2 = a2 << 16;
  t2 += a2;
  a1 = a1 & 0xFFFF;
  t0 = t0 & 0xFFFF0000;
  at = cop2.IR1;
  v0 = cop2.IR2;
  at = at & 0xFFFF;
  t0 += at;
  v0 = v0 << 16;
  t1 = v0 + a1;
  t3 = cop2.IR3 & 0xFFFF;
  cop2.RTM0 = a3;
  cop2.RTM1 = t0;
  cop2.RTM2 = t1;
  cop2.RTM3 = t2;
  cop2.RTM4 = t3;
label80052CA4:
  at = t4 & 0xFF;
  temp = at == 0;
  at = at << 1;
  if (temp) goto label80052D48;
  v0 = at + spyro_cos_lut;
  at = at + spyro_sin_lut;
  v0 = lhu(v0 + 0x0000);
  at = lhu(at + 0x0000);
  cop2.VZ0 = 0;
  cop2.VXY0 = (at << 16) + v0;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  at = -at;
  at = at & 0xFFFF;
  v0 = v0 << 16;
  at += v0;
  a0 = cop2.IR1;
  a1 = cop2.IR2;
  a2 = cop2.IR3;
  cop2.VXY0 = at;
  cop2.VZ0 = 0;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  a1 = a1 << 16;
  t0 = t0 & 0xFFFF;
  t0 += a1;
  a0 = a0 & 0xFFFF;
  a2 = a2 & 0xFFFF;
  t1 = t1 & 0xFFFF0000;
  at = cop2.IR1;
  v0 = cop2.IR2;
  v1 = cop2.IR3;
  at = at << 16;
  a3 = at + a0;
  v1 = v1 << 16;
  t2 = v1 + a2;
  v0 = v0 & 0xFFFF;
  t1 += v0;
label80052D48:
  sw(t7 + 0x0020, a3);
  sw(t7 + 0x0024, t0);
  sw(t7 + 0x0028, t1);
  sw(t7 + 0x002C, t2);
  sw(t7 + 0x0030, t3);
label80052D5C:
  return;
}

// size: 0x000001D4
void function_80052D64(void)
{
  uint32_t temp;
  t8 = a0;
  t7 = a1;
  t9 = a2;
  at = lw(t8 + 0x0020);
  v0 = lw(t8 + 0x0024);
  v1 = lw(t8 + 0x0028);
  a0 = lw(t8 + 0x002C);
  a1 = lw(t8 + 0x0030);
  cop2.RTM0 = at;
  cop2.RTM1 = v0;
  cop2.RTM2 = v1;
  cop2.RTM3 = a0;
  cop2.RTM4 = a1;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  at = lhu(t8 + 0x0036);
  a1 = lw(t8 + 0x003C);
  v0 = 0x80076378;
  at = at << 2;
  at += v0;
  at = lw(at + 0x0000);
  a2 = lbu(t8 + 0x0040);
  v0 = a1 << 2;
  v0 = v0 & 0x3FC;
  v0 += at;
  v1 = a1 >> 6;
  v1 = v1 & 0x3FC;
  v1 += at;
  v0 = lw(v0 + 0x0038);
  a0 = a1 >> 13;
  a0 = a0 & 0x7F8;
  a1 = a1 >> 21;
  a1 = a1 & 0x7F8;
  a0 += v0;
  a0 = lw(a0 + 0x0024);
  at = lb(v0 + 0x0005);
  a0 = a0 << 11;
  a0 = a0 >> 11;
  t7 = t7 << 2;
  a0 += t7;
  temp = (int32_t)a2 > 0;
  v0 = lw(a0 + 0x0000);
  if (temp) goto label80052E3C;
  a1 = v0 << 20;
  a1 = (int32_t)a1 >> 19;
  a0 = v0 << 10;
  a0 = (int32_t)a0 >> 21;
  v1 = (int32_t)v0 >> 21;
  v1 = v1 << at;
  a0 = a0 << at;
  a1 = a1 << at;
  goto label80052EE8;
label80052E3C:
  v1 = lw(v1 + 0x0038);
  t1 = v0 << 20;
  t1 = (int32_t)t1 >> 19;
  t0 = v0 << 10;
  t0 = (int32_t)t0 >> 21;
  a3 = (int32_t)v0 >> 21;
  a1 += v1;
  a1 = lw(a1 + 0x0024);
  a3 = a3 << at;
  t0 = t0 << at;
  t1 = t1 << at;
  v1 = 64; // 0x0040
  v1 -= a2;
  a1 = a1 << 11;
  a1 = a1 >> 11;
  cop2.IR0 = v1;
  cop2.IR1 = a3;
  cop2.IR2 = t0;
  cop2.IR3 = t1;
  a1 += t7;
  v1 = lw(a1 + 0x0000);
  GPF(SF_OFF, LM_OFF);
  a1 = v1 << 20;
  a1 = (int32_t)a1 >> 19;
  a0 = v1 << 10;
  a0 = (int32_t)a0 >> 21;
  v1 = (int32_t)v1 >> 21;
  v1 = v1 << at;
  a0 = a0 << at;
  a1 = a1 << at;
  cop2.IR0 = a2;
  cop2.IR1 = v1;
  cop2.IR2 = a0;
  cop2.IR3 = a1;
  GPL(SF_OFF, LM_OFF);
  v1 = cop2.MAC1;
  a0 = cop2.MAC2;
  a1 = cop2.MAC3;
  v1 = (int32_t)v1 >> 6;
  a0 = (int32_t)a0 >> 6;
  a1 = (int32_t)a1 >> 6;
label80052EE8:
  a1 = a1 << 16;
  a0 += a1;
  cop2.VZ0 = v1;
  cop2.VXY0 = a0;
  RTPS();
  at = lw(t8 + 0x000C);
  v0 = lw(t8 + 0x0010);
  v1 = lw(t8 + 0x0014);
  a0 = cop2.IR3;
  a1 = cop2.IR1;
  a2 = cop2.IR2;
  a0 += at;
  a1 = v0 - a1;
  a2 = v1 - a2;
  sw(t9 + 0x0000, a0);
  sw(t9 + 0x0004, a1);
  sw(t9 + 0x0008, a2);
  return;
}

// size: 0x00000188
void function_80052F38(void)
{
  uint32_t temp;
  t8 = a0;
  t9 = a1;
  at = a2;
  v0 = a3;
  t7 = player_position;
  t6 = 0x80076378;
  t6 = lw(t6 + 0x0000);
  at = at << 2;
  at += t6;
  at = lw(at + 0x0038);
  v0 = v0 << 2;
  v0 += at;
  v0 = lw(v0 + 0x0024);
  a3 = v0 >> 21;
  a3 = a3 & 0x1;
  v0 = v0 << 11;
  v0 = v0 >> 10;
  a0 = lw(at + 0x0010);
  a1 = lw(v0 + 0x0008);
  a2 = v0 + 24; // 0x0018
  a1 = a1 >> 20;
  a1 += a2;
  at = 0;
  v0 = 0;
  v1 = 0;
  t6 = 0x8006D614; // &0xFE9FD3FA
label80052FB0:
  temp = (int32_t)a3 > 0;
  t8--;
  if (temp) goto label80053010;
  t0 = lbu(a1 + 0x0000);
  t1 = lw(a0 + 0x0000);
  a1++;
  a0 += 4; // 0x0004
  a3 = t0 & 0x1;
  t0 = t0 << 1;
  t0 = t0 & 0x1FC;
  t0 += t6;
  t0 = lw(t0 + 0x0000);
  t0 += t1;
  t1 = (int32_t)t0 >> 21;
  at += t1;
  t1 = t0 << 11;
  t1 = (int32_t)t1 >> 21;
  v0 += t1;
  t1 = t0 << 22;
  t1 = (int32_t)t1 >> 21;
  temp = t8 != 0;
  v1 += t1;
  if (temp) goto label80052FB0;
  goto label800530B0;
label80053010:
  t0 = lh(a2 + 0x0000);
  a2 += 2; // 0x0002
  temp = (int32_t)t0 < 0;
  a3 = t0 & 0x4000;
  if (temp) goto label80053078;
  t1 = lw(a0 + 0x0000);
  a0 += 4; // 0x0004
  t2 = t0 << 18;
  t2 = (int32_t)t2 >> 25;
  at += t2;
  t2 = t0 << 23;
  t2 = (int32_t)t2 >> 25;
  v0 += t2;
  t2 = t0 << 28;
  t2 = (int32_t)t2 >> 25;
  v1 += t2;
  t0 = (int32_t)t1 >> 21;
  at += t0;
  t0 = t1 << 11;
  t0 = (int32_t)t0 >> 21;
  v0 += t0;
  t0 = t1 << 22;
  t0 = (int32_t)t0 >> 21;
  temp = t8 != 0;
  v1 += t0;
  if (temp) goto label80052FB0;
  goto label800530B0;
label80053078:
  t1 = lhu(a2 + 0x0000);
  a2 += 2; // 0x0002
  a0 += 4; // 0x0004
  t2 = t0 << 18;
  at = (int32_t)t2 >> 21;
  t2 = t0 << 28;
  t0 = t1 << 12;
  t0 = t0 | t2;
  v0 = (int32_t)t0 >> 21;
  t2 = t1 << 22;
  temp = t8 != 0;
  v1 = (int32_t)t2 >> 21;
  if (temp) goto label80052FB0;
  goto label800530B0;
label800530B0:
  sw(t9 + 0x0000, at);
  sw(t9 + 0x0004, v0);
  sw(t9 + 0x0008, v1);
  return;
}

// size: 0x000001A0
void function_800533D0(void)
{
  a3 = lw(a0 + 0x1C);
  if ((int32_t)a3 >= 0) {
    sw(a0 + 0x1C, a3);
    return;
  }
  v1 = 0x80077368;
  at = lw(v1 + 0);
  v0 = lw(v1 + 4);
  v1 = lw(v1 + 8);
  a1 = spyro_sqrt(at*at + v1*v1);

  if ((int32_t)(a1 - v0) < 0) {
    if ((int32_t)(a1 + v0) < 0) {
      div_psx(v0 << 5, a1);
      a3 = 0x80;
    } else {
      div_psx(a1 << 5, -v0);
      a3 = 0x40;
    }
  } else {
    if ((int32_t)(a1 + v0) < 0) {
      div_psx(a1 << 5, -v0);
      a3 = 0xC0;
    } else {
      div_psx(v0 << 5, a1);
      a3 = 0x00;
    }
  }
  v0=lo;
  v0 += a3;

  if ((int32_t)(v1 - at) < 0) {
    if ((int32_t)(v1 + at) < 0) {
      div_psx(at << 5, v1);
      a3 = 0x80;
    } else {
      div_psx(v1 << 5, -at);
      a3 = 0x40;
    }
  } else {
    if ((int32_t)(v1 + at) < 0) {
      div_psx(v1 << 5, -at);
      a3 = 0xC0;
    } else {
      div_psx(at << 5, v1);
      a3 = 0;
    }
  }

  a1 = lw(0x80076B80 + 8);
  at = lo + a3;

  a3 = 0x80000000
    | ((((-at) >> 2) & 0x3F) << 16)
    | ((((-v0) >> 2) & 0x3F) << 22)
    | (a1 >> 2);
  sw(a0 + 0x1C, a3);
  return;
}