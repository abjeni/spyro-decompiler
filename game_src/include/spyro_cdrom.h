#pragma once

#include <stdint.h>

char *write_cdrom_header(uint32_t sector_num, char header[]);
uint32_t parse_cdrom_header(char header[]);

uint32_t dma_cdrom_callback(uint32_t callback);

uint32_t CdInit(void);
void init_cdrom(void);

void read_disk(uint32_t sector, uint32_t dst, uint32_t len, uint32_t offset, uint32_t num);
void read_disk1(uint32_t sector, uint32_t dst, uint32_t len, uint32_t offset, uint32_t num);
void read_disk2(uint32_t sector, uint32_t dst, uint32_t len, uint32_t offset, uint32_t num);