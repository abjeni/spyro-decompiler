#pragma once

#include "stdint.h"

void InitHeap(uint32_t addr, uint32_t size);
void InitCARD2(uint32_t pad_enable);
void GPU_cw(uint32_t gp0cmd);
int ChangeClearPAD(int a0);
void _96_init(void);
void _96_remove(void);
uint32_t EnterCriticalSection(void);
void ExitCriticalSection(void);
uint32_t ChangeClearRCnt(uint32_t t, uint32_t flag);
void _bu_init(void);
void DelDrv(uint32_t device_name_lowercase);
void StartCARD2(void);
void SysEnqIntRP(uint32_t priority, uint32_t struc);
uint32_t ResetEntryInt(void);
void HookEntryInt(uint32_t addr);
void int_save_regs(void);
void interrupt2(uint32_t type);
void interrupt(uint32_t type);
void ReturnFromException(void);
void SysDeqIntRP(uint32_t priority, uint32_t struc);
uint32_t OpenEvent(uint32_t class, uint32_t spec, uint32_t mode, uint32_t func);
uint32_t CloseEvent(uint32_t event_id);
uint32_t EnableEvent(uint32_t event);
void DeliverEvent(uint32_t class, uint32_t spec);
uint32_t TestEvent(uint32_t event);
void _new_card(void);
void _card_write(uint32_t port, uint32_t sector, uint32_t src);
uint32_t _card_info(uint32_t port);
uint32_t _card_load(uint32_t port);

uint32_t psx_open(char *file_name, uint32_t mode);
uint32_t psx_lseek(uint32_t fd, uint32_t offset, uint32_t seektype);
uint32_t psx_read(uint32_t fd, char *dst, uint32_t len);
uint32_t psx_write(int fd, char *src, uint32_t len);
uint32_t psx_close(uint32_t fd);

uint32_t GetC0Table(void);
void FlushCache(void);

void LoadExec(char *filename, uint32_t stackbase, uint32_t stack_offset);
void psx_exit(uint32_t code);
uint32_t format(uint32_t devicename);