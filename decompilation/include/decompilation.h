#pragma once

#include "functions.h"
#include <stdio.h>
#include <stdint.h>

#define TYPE_CUSTOM_LIST 0
#define TYPE_PSXMEM_LIST 1

#define CUSTOM_LIST(addrs2, jumps2) \
  (struct jump_list){ \
    .type = TYPE_CUSTOM_LIST, \
    .num_addrs = sizeof(addrs2)/sizeof(addrs2[0]), \
    .addrs = addrs2, \
    .num_jumps = sizeof(jumps2)/sizeof(jumps2[0]), \
    .list = jumps2 \
  }

#define PSXMEM_LIST(addrs2, num_jumps2, psx_addr2) \
  (struct jump_list){ \
    .type = TYPE_PSXMEM_LIST, \
    .num_addrs = sizeof(addrs2)/sizeof(addrs2[0]), \
    .addrs = addrs2, \
    .num_jumps = num_jumps2, \
    .psx_addr = psx_addr2 \
  }

#define JUMP_LIST_LIST(jump_lists) \
  (struct jump_list_list){ \
    .n = sizeof(jump_lists)/sizeof(jump_lists[0]), \
    .jumps = jump_lists \
  }

#define ENTRIES(entries2) \
  (struct entries){ \
    .n = sizeof(entries2)/sizeof(entries2[0]), \
    .entries = entries2 \
  }

#define SKIPS(skips2) \
  (struct skips){ \
    .n = sizeof(skips2)/sizeof(skips2[0]), \
    .funcs = skips2 \
  }

#define ARR_JUMP_LIST(...) (struct jump_list[]){__VA_ARGS__}
#define ARR_U32(...) (uint32_t[]){__VA_ARGS__}

struct jump_list {
  uint32_t type;
  uint32_t num_addrs;
  uint32_t *addrs;
  uint32_t num_jumps;
  union {
    const uint32_t *list;
    uint32_t psx_addr;
  };
};

struct jump_list_list {
  uint32_t n;
  struct jump_list *jumps;
};

struct skips {
  uint32_t n;
  uint32_t *funcs;
};

struct entries {
  uint32_t n;
  uint32_t *entries;
};

struct mem_range {
  uint32_t base;
  uint32_t size;
};

struct program {
  char *psx_mem;
  struct mem_range range;
  struct entries entries;
  uint32_t entries2n;
  uint32_t *entries2;
  struct jump_list_list jumpss;
  struct skips skips;
  addr_list *external_calls;
  char *id;
  FILE *output;
  FILE *header;
};

struct function_name {
  uint32_t func;
  char *func_call;
  char *declaration;
  char *id;
};


struct address_name {
  uint32_t addr;
  char *name;
  char *id;
};

void print_func_name(struct program prog, uint32_t addr);
char *get_address_name(struct program prog, uint32_t addr);
int addr_in_range(struct program prog, uint32_t addr);
int read_instructions(struct program prog);