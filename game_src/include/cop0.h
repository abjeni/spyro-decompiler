#pragma once

#include <stdint.h>

struct cop0 {
  uint32_t BPC;
  uint32_t BDA;
  uint32_t JUMPDEST;
  uint32_t DCIC;
  uint32_t BadVaddr;
  uint32_t BDAM;
  uint32_t BPCM;
  uint32_t SR;
  uint32_t CAUSE;
  uint32_t EPC;
  uint32_t PRID;
};

extern struct cop0 cop0;