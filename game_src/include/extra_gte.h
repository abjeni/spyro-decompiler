#include <stdint.h>
#include <stddef.h>
#include "int_math.h"

void spyro_unpack1(uint32_t pack, uint32_t *reg1, uint32_t *reg2, uint32_t *reg3);
void spyro_unpack3(uint32_t pack, uint32_t *reg1, uint32_t *reg2, uint32_t *reg3);
void spyro_unpack6(uint32_t pack, uint32_t *reg1, uint32_t *reg2, uint32_t *reg3);
vec3 spyro_unpack6_vec3(uint32_t pack);
vec3 spyro_unpack2_vec3(uint32_t pack, vec3 v);
uint32_t spyro_pack1_vec3(vec3 v);
vec3 spyro_unpack4_vec3(uint32_t pack, uint32_t x, uint32_t y, uint32_t z);
vec3 spyro_unpack5_vec3(uint32_t pack, uint32_t x, uint32_t y, uint32_t z);
void spyro_interpolate_packed_vecs1(uint32_t *a, uint32_t *b, uint32_t *c, int32_t i, size_t iter);
vec3 load_vec3(uint32_t addr);
void save_vec3(uint32_t addr, vec3 v);
vec3_32 load_vec3_32(uint32_t addr);
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
void set_FC(uint32_t r, uint32_t g, uint32_t b);
void set_BK_vec3(vec3 bk);
void set_BK_vec3_32(vec3_32 bk);
void set_BK(uint32_t r, uint32_t g, uint32_t b);
vec3 get_MAC_vec3(void);
void save_mat3(uint32_t addr, mat3 m);
mat3 load_mat3(uint32_t addr);
void print_mat3(mat3 m);