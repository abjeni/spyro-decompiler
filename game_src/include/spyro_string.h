#include <stdint.h>

void spyro_memset32(uint32_t dst, uint32_t set, uint32_t len);
void spyro_memcpy32(uint32_t dst, uint32_t src, uint32_t len);
uint32_t spyro_memcpy8(uint32_t dst, uint32_t src, uint32_t len);
void spyro_memset8(uint32_t dst, uint8_t set, uint32_t len);
uint32_t spyro_strlen(uint32_t str);
uint32_t spyro_strchr(uint32_t str, uint32_t chr, uint32_t len);
void spyro_memmove(uint32_t dst, uint32_t src, uint32_t len);
void spyro_memclr32(uint32_t dst, uint32_t len);
uint32_t spyro_memclr8(uint32_t dst, uint32_t len);