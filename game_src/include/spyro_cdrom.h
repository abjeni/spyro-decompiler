#pragma once

#include <stdint.h>

void write_cdrom_header(uint32_t sector_num, char header[]);
uint32_t parse_cdrom_header(char header[]);

uint32_t dma_cdrom_callback(uint32_t callback);

int CdSync(int mode, uint8_t *result);

uint32_t CdInit(void);
void init_cdrom(void);

void read_disk(uint32_t sector, uint32_t dst, uint32_t len, uint32_t offset, uint32_t num);
void read_disk1(uint32_t sector, uint32_t dst, uint32_t len, uint32_t offset, uint32_t num);
void read_disk2(uint32_t sector, uint32_t dst, uint32_t len, uint32_t offset, uint32_t num);

// interrupt types
#define CdlNoIntr 0
#define CdlDataReady 1
#define CdlComplete 2
#define CdlAcknowledge 3
#define CdlDataEnd 4
#define CdlDiskError 5

// command types
#define CdlSync 0
#define CdlNop 1
#define CdlSetloc 2
#define CdlPlay 3
#define CdlForward 4
#define CdlBackward 5
#define CdlReadN 6
#define CdlStandby 7
#define CdlStop 8
#define CdlPause 9
#define CdlReset 10
#define CdlMute 11
#define CdlDemute 12
#define CdlSetfilter 13
#define CdlSetmode 14
#define CdlGetlocL 16
#define CdlGetlocP 17
#define CdlGetTN 19
#define CdlGetTD 20
#define CdlSeekL 21
#define CdlSeekP 22
#define CdlReadS 27