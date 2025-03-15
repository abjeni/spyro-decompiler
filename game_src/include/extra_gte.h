#include <stdint.h>
#include "int_math.h"

vec3 load_vec3(uint32_t addr);
void load_RTM(uint32_t addr);
mat3 get_RTM(void);
mat3 get_LLM(void);
void set_RTM1(uint32_t list[5]);
void set_RTM(mat3 m);
void load_TR(uint32_t addr);
void set_TR(uint32_t trx, uint32_t try, uint32_t trz);
void set_V0(uint32_t v0x, uint32_t v0y, uint32_t v0z);
void set_V0_vec3(vec3 v0);
vec3 get_S2(void);
void load_IR(uint32_t addr);
void set_IR(uint32_t irx, uint32_t iry, uint32_t irz);
void set_IR_vec3(vec3 ir);
void get_IR(uint32_t *irx, uint32_t *iry, uint32_t *irz);
vec3 get_IR_vec3(void);
void set_FC_vec3(vec3 fc);
void set_BK_vec3(vec3 bk);
vec3 get_MAC_vec3(void);
void save_mat3(uint32_t addr, mat3 m);
mat3 load_mat3(uint32_t addr);
void print_mat3(mat3 m);