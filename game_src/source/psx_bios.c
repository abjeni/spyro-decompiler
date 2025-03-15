#include <stdint.h>
#include <setjmp.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "main.h"
#include "psx_mem.h"
#include "spyro_vsync.h"
#include "debug.h"

// i decided to use pointers as parameters for some of these, which was a mistake

extern struct psx_mem psx_mem;

void InitHeap(uint32_t addr, uint32_t size)
{
  //printf("heap address and size [%8X, %d]\n", addr, size);
  //printf("range [%8X, %8X]\n", addr, addr+size);
}

void GPU_cw(uint32_t gp0cmd)
{
  //printf("sending command %.8X\n", gp0cmd);
  sw(0x1F801810, gp0cmd);
}

int ChangeClearPAD(int a0)
{
  //printf("ChangeClearPAD(%d)\n", a0);
  return 0;
}

void _96_init(void)
{
  //printf("call to _96_init()\n");
}

void _96_remove(void)
{
  //printf("call to _96_remove()\n");
}

uint32_t EnterCriticalSection(void)
{
  return 1;
  //printf("entering critical section\n");
}

void ExitCriticalSection(void)
{
  //printf("exiting critical section\n");
}

uint32_t timerflags[4] = {1, 1, 1, 1};

uint32_t ChangeClearRCnt(uint32_t t, uint32_t flag)
{
  //printf("ChangeClearRCnt(timer: %d,flag: %d)\n", t, flag);

  uint32_t prev = timerflags[t];
  timerflags[t] = flag;

  if (flag == 1) BREAKPOINT;

  return prev;
}

void _bu_init(void)
{
  //printf("_bu_init called\n");
}

void DelDrv(uint32_t device_name_lowercase)
{
  //char *name = addr_to_pointer(device_name_lowercase);

  //printf("DelDrv(%X)\n", device_name_lowercase);
}

void InitCARD2(uint32_t pad_enable)
{
  //printf("start card2\n");
}

void StartCARD2(void)
{
  //printf("start card2\n");
}

static uint32_t SysIntRP[8] = {};

void SysEnqIntRP(uint32_t priority, uint32_t struc) //bugged, use with care
{
  //printf("SysEnqIntRP(%.8X, %.8X) info:\n", priority, struc);

  //printf("  second function %.8X\n", lw(struc+4));
  //printf("  first function %.8X\n", lw(struc+8));

  assert(priority < 8);
  assert(SysIntRP[priority] == 0);

  SysIntRP[priority] = struc;
}

uint32_t hook;

uint32_t ResetEntryInt(void)
{
  BREAKPOINT;
  return 0;
}

void HookEntryInt(uint32_t addr)
{
  //printf("HookEntryInt(0x%.8X) %X\n", addr, lw(addr));
  //printf("- ra: %.8X\n", lw(addr + 0x0));
  //printf("- sp: %.8X\n", lw(addr + 0x4));
  //printf("- fp: %.8X\n", lw(addr + 0x8));

  //for (int i = 0; i < 8; i++)
    //printf("- s%d: %.8X\n", i, lw(addr + 0xC+i*4));

  //printf("- gp: %.8X\n", lw(addr + 0x2C));

  hook = addr;
}

uint32_t int_ret_regs[34];
uint32_t temp_save;
void int_save_regs(void)
{
  int_ret_regs[ 0] = at;
  int_ret_regs[ 1] = v0;
  int_ret_regs[ 2] = v1;
  int_ret_regs[ 3] = a0;
  int_ret_regs[ 4] = a1;
  int_ret_regs[ 5] = a2;
  int_ret_regs[ 6] = a3;
  int_ret_regs[ 7] = t0;
  int_ret_regs[ 8] = t1;
  int_ret_regs[ 9] = t2;
  int_ret_regs[10] = t3;
  int_ret_regs[11] = t4;
  int_ret_regs[12] = t5;
  int_ret_regs[13] = t6;
  int_ret_regs[14] = t7;
  int_ret_regs[15] = s0;
  int_ret_regs[16] = s1;
  int_ret_regs[17] = s2;
  int_ret_regs[18] = s3;
  int_ret_regs[19] = s4;
  int_ret_regs[20] = s5;
  int_ret_regs[21] = s6;
  int_ret_regs[22] = s7;
  int_ret_regs[23] = t8;
  int_ret_regs[24] = t9;
  int_ret_regs[26] = k0;
  int_ret_regs[27] = k1;
  int_ret_regs[28] = gp;
  int_ret_regs[29] = sp;
  int_ret_regs[30] = fp;
  int_ret_regs[31] = ra;
  int_ret_regs[32] = hi;
  int_ret_regs[33] = lo;
}
void int_load_regs(void)
{
  at = int_ret_regs[ 0];
  v0 = int_ret_regs[ 1];
  v1 = int_ret_regs[ 2];
  a0 = int_ret_regs[ 3];
  a1 = int_ret_regs[ 4];
  a2 = int_ret_regs[ 5];
  a3 = int_ret_regs[ 6];
  t0 = int_ret_regs[ 7];
  t1 = int_ret_regs[ 8];
  t2 = int_ret_regs[ 9];
  t3 = int_ret_regs[10];
  t4 = int_ret_regs[11];
  t5 = int_ret_regs[12];
  t6 = int_ret_regs[13];
  t7 = int_ret_regs[14];
  s0 = int_ret_regs[15];
  s1 = int_ret_regs[16];
  s2 = int_ret_regs[17];
  s3 = int_ret_regs[18];
  s4 = int_ret_regs[19];
  s5 = int_ret_regs[20];
  s6 = int_ret_regs[21];
  s7 = int_ret_regs[22];
  t8 = int_ret_regs[23];
  t9 = int_ret_regs[24];
  k1 = int_ret_regs[26];
  k1 = int_ret_regs[27];
  gp = int_ret_regs[28];
  sp = int_ret_regs[29];
  fp = int_ret_regs[30];
  ra = int_ret_regs[31];
  hi = int_ret_regs[32];
  lo = int_ret_regs[33];
}

void function_8005E03C(void);
void function_80069634(void);
void function_8006969C(void);

void interrupt2(uint32_t type)
{
  //uint32_t mask = 1<<type;

  sp = 0x80010000;

  for (int i = 0; i < 8; i++) {
    if(SysIntRP[i])
    {
      uint32_t addr = lw(SysIntRP[i]+8);
      switch(addr)
      {
      case 0x80069634:
        function_80069634();
        break;
      default:
        printf("unknown interrupt address %.8X\n", addr);
        BREAKPOINT;
      }

      if (v0)
      {
        uint32_t addr = lw(SysIntRP[i]+4);
        switch(addr)
        {
        case 0x8006969C:
          function_8006969C();
          break;
        default:
          printf("unknown interrupt address %.8X\n", addr);
          BREAKPOINT;
        }
      }
    }
  }

  ra = lw(hook+0x00);
  sp = lw(hook+0x04);
  fp = lw(hook+0x08);
  s0 = lw(hook+0x0C);
  s1 = lw(hook+0x10);
  s2 = lw(hook+0x14);
  s3 = lw(hook+0x18);
  s4 = lw(hook+0x1C);
  s5 = lw(hook+0x20);
  s6 = lw(hook+0x24);
  s7 = lw(hook+0x28);
  gp = lw(hook+0x2C);

  a1 = 1;
  v0 = 1;

  switch (ra) {
  case 0x8005DFC8:
    function_8005E03C();
    return;
  default:
    printf("%.8X\n", ra);
    BREAKPOINT;
  }

  //interrupt_entry_hook();
}

int interrupt_depth = 0;
struct __jmp_buf_tag env;

void interrupt(uint32_t type)
{
  interrupt_depth++;

#ifdef DEBUG_INTERRUPT
  printf("interrupt %d begin\n", type);
#endif

  if(interrupt_depth != 1) {
    printf("interrupt on top of interrupt!\n");
    BREAKPOINT;
  }

  int_save_regs();

  if (setjmp(&env) == 0)
    interrupt2(type);

  int_load_regs();

#ifdef DEBUG_INTERRUPT
  printf("interrupt %d end\n", type);
#endif

  interrupt_depth--;
}

void ReturnFromException(void)
{
  longjmp(&env, 1);
  BREAKPOINT;
}

void SysDeqIntRP(uint32_t priority, uint32_t struc) //bugged, use with care
{
  //printf("SysDeqIntRP(%.8X, %.8X)\n", priority, struc);

  assert(priority < 8);
  //assert(SysIntRP[priority]);

  SysIntRP[priority] = 0;

}

#define EVENT_STATUS_NONE 0
#define EVENT_STATUS_DISABLED 1
#define EVENT_STATUS_ENABLED 1

struct event {
  uint32_t status;
  uint32_t class;
  uint32_t spec;
  uint32_t mode;
  uint32_t func;
  uint32_t ready;
};

struct event events[16] = {0};

uint32_t OpenEvent(uint32_t class, uint32_t spec, uint32_t mode, uint32_t func)
{
  //printf("OpenEvent(class: %X, spec: %X, mode: %X, func: %X)\n", class, spec, mode, func);
  

  for (uint32_t i = 0; i < 16; i++)
  {
    if (events[i].status == EVENT_STATUS_NONE)
    {
      events[i].status = EVENT_STATUS_DISABLED;
      events[i].class = class;
      events[i].spec = spec;
      events[i].mode = mode;
      events[i].func = func;
      events[i].ready = 0;

      return 0xF1000000 | i;
    }
  }

  BREAKPOINT;
  return -1;
}

uint32_t CloseEvent(uint32_t event_id)
{
  //printf("CloseEvent %X\n", event_id);
  events[event_id&15].status = EVENT_STATUS_NONE;

  return 1;
}

uint32_t EnableEvent(uint32_t event)
{
  //printf("EnableEvent(event: %X)\n", event);

  uint32_t i = event&15;

  if (events[i].status == EVENT_STATUS_DISABLED)
  {
    events[i].status = EVENT_STATUS_ENABLED;
    events[i].ready = 0;
  }

  return 1;
}

void function_80067DD0(void);
void function_80067DE4(void);
void function_80067DF8(void);
void function_80067E0C(void);
void function_80067E20(void);
void function_80067E34(void);
void function_80067E48(void);
void function_80067E5C(void);

void DeliverEvent(uint32_t class, uint32_t spec)
{
  //if (class != 0xF0000003)
    //printf("DeliverEvent(class: %X, spec: %X)\n", class, spec);

  uint32_t delivered = 0;
  for (uint32_t i = 0; i < 16; i++) {
    struct event event = events[i];
    if (event.status == EVENT_STATUS_ENABLED) {
      if (event.class == class && event.spec == spec) {
        delivered = 1;
        if (event.mode == 0x2000) {
          events[i].ready = 1;
        }
        else {
          switch(events[i].func) {
          case 0x80067DD0:
            function_80067DD0();
            break;
          case 0x80067DE4:
            function_80067DE4();
            break;
          case 0x80067DF8:
            function_80067DF8();
            break;
          case 0x80067E0C:
            function_80067E0C();
            break;
          case 0x80067E20:
            function_80067E20();
            break;
          case 0x80067E34:
            function_80067E34();
            break;
          case 0x80067E48:
            function_80067E48();
            break;
          case 0x80067E5C:
            function_80067E5C();
            break;
          default:
            printf("class: %X, spec: %X\n", events[i].class, events[i].spec);
            printf("line %d: calling function %X\n", __LINE__, events[i].func);
            BREAKPOINT;
          }
        }
      }
    }
  }
  if (!delivered) printf("not delivered!\n");
}

uint32_t TestEvent(uint32_t event)
{

  uint32_t i = event&15;

  uint32_t rdy = events[i].ready;
  events[i].ready = 0;

  if (events[i].class == 0xF0000009) rdy = 1;

  return rdy;
}

void _new_card(void)
{
  //printf("_new_card()\n");
  return;
}

void _card_write(uint32_t port, uint32_t sector, uint32_t src)
{
  //printf("_card_write(port: %.8X, sector: %.8X, src: %.8X)\n", port, sector, src);
  
  DeliverEvent(0xF4000001, 4);
  DeliverEvent(0xF0000011, 4);
}

uint32_t _card_info(uint32_t port)
{
  //printf("_card_info(port: %d)\n", port);

  DeliverEvent(0xF4000001, 4);
  DeliverEvent(0xF0000011, 4);

  return 0;
}

uint32_t _card_load(uint32_t port)
{
  //printf("_card_load(port: %d)\n", port);

  DeliverEvent(0xF4000001, 4);
  DeliverEvent(0xF0000011, 4);

  return 0;
}

/*
bit0     1=Read  ;\These bits aren't actually used by the BIOS, however, at
bit1     1=Write ;/least 1 should be set; won't work when all 32bits are zero
bit2     1=Exit without waiting for incoming data (when TTY buffer empty)
bit9     0=Open Existing File, 1=Create New file (memory card only)
bit15    1=Asynchronous mode (memory card only; don't wait for completion)
bit16-31 Number of memory card blocks for a new file on the memory card
*/

union mode {
  struct {
    uint32_t read : 1;
    uint32_t write : 1;
    uint32_t exit : 1;
    uint32_t unknown1 : 6;
    uint32_t create : 1;
    uint32_t unknown2 : 5;
    uint32_t async : 1;
    uint32_t blocks : 16;
  };
  uint32_t val;
};

// buffer overflow warning
char memcard_path[256];

char *get_memcard_path(char *file_name)
{
  char *prefix = "mem_card/";
  char *buf = memcard_path;

  uint32_t available = 256-9;

  while (*prefix)
  {
    *buf = *prefix;
    prefix++;buf++;
  }

  while (*file_name)
  {
    *buf = *file_name;
    file_name++;buf++;
    if (available-- <= 0) BREAKPOINT;
  }

  *buf = 0;

  return memcard_path;
}

struct file {
  int fd;
  int size;
};

struct file files[16] = {
  {1}, {1}, {0}, {0},
  {0}, {0}, {0}, {0},
  {0}, {0}, {0}, {0},
  {0}, {0}, {0}, {0}
};

uint32_t psx_open(char *file_name, uint32_t modev)
{
  union mode mode = {
    .val = modev
  };

  char *file_path = get_memcard_path(file_name);

  struct file file;

  if (mode.create)
  {
    printf("creating %s\n", file_path);
    file.fd = creat(file_path, S_IRUSR | S_IWUSR);

    printf("error %m\n");
    if (file.fd == -1) BREAKPOINT;

    file.size = mode.blocks*0x2000;
    char *buf = calloc(file.size, 1);
    write(file.fd, buf, file.size);

    lseek(file.fd, 0, SEEK_SET);

    DeliverEvent(0xF4000001, 4);
    DeliverEvent(0xF0000011, 4);
  }
  else
  {
    file.fd = open(file_path, O_RDWR);
    if (file.fd == -1) {
      DeliverEvent(0xF4000001, 0x8000);
      DeliverEvent(0xF0000011, 0x8000);
      return -1;
    }

    struct stat stats;

    int err = fstat(file.fd, &stats);

    if (err == -1) BREAKPOINT;

    file.size = stats.st_size;

    DeliverEvent(0xF4000001, 4);
    DeliverEvent(0xF0000011, 4);
  }

  //printf("fd: %d\n", file.fd);

  int fd2 = -1;

  for (int i = 0; i < 16; i++)
  {
    if (files[i].fd == 0)
    {
      files[i] = file;
      fd2 = i;
      break;
    }
    if (i == 15) BREAKPOINT;
  }

  return fd2;
}

uint32_t psx_lseek(uint32_t fd, uint32_t offset, uint32_t seektype)
{

  struct file file = files[fd];

  //printf("lseek(%d, %d, %d)\n", file.fd, offset, seektype);
  int cursor = lseek(file.fd, offset, seektype);

  DeliverEvent(0xF4000001, 4);
  DeliverEvent(0xF0000011, 4);

  if (cursor == -1) BREAKPOINT;

  return cursor;
}

uint32_t psx_write(int fd, char *str, uint32_t len)
{
  if (fd == 1) {
    printf("%.*s", len, str);
  } else {

    struct file file = files[fd];
    if (file.fd == 0) BREAKPOINT;

    int cursor = lseek(file.fd, 0, SEEK_CUR);

    if (cursor == -1) BREAKPOINT;

    int len2 = file.size - cursor;
    //printf("filesize: %d cursor: %d len2: %d\n", file.size, cursor, len2);
    if (len2 > len) len2 = len; 

    ssize_t written_bytes = write(file.fd, str, len2);
    //printf("write(%d, %p, %d)\n", file.fd, str, len2);

    if (written_bytes == -1) {
      printf("err: %s\n", strerror(errno));
      BREAKPOINT;
    } else if (written_bytes == 0) {
      DeliverEvent(0xF4000001, 0x8000);
      DeliverEvent(0xF0000011, 0x8000);
      return 0;
    }

    DeliverEvent(0xF4000001, 4);
    DeliverEvent(0xF0000011, 4);

    return 0;
  }

  return len;
}

uint32_t psx_read(uint32_t fd, char *dst, uint32_t len)
{

  struct file file = files[fd];
  if (file.fd == 0) BREAKPOINT;

  ssize_t read_bytes = read(file.fd, dst, len);

  if (read_bytes == -1) BREAKPOINT;

  //printf("read(%d, %p, %d)\n", file.fd, dst, len);
  DeliverEvent(0xF4000001, 4);
  DeliverEvent(0xF0000011, 4);

  return 0;
}

uint32_t psx_close(uint32_t fd)
{

  //printf("close(%d)\n", files[fd].fd);
  DeliverEvent(0xF4000001, 4);
  DeliverEvent(0xF0000011, 4);

  close(files[fd].fd);

  files[fd] = (struct file){0};

  return fd;
}

uint32_t GetC0Table(void)
{
  BREAKPOINT;
  //printf("getting C0 jump table\n");
  return -1;
}

void FlushCache(void)
{
  //printf("flushing the cache\n");
}

void LoadExec(char *filename, uint32_t stackbase, uint32_t stack_offset)
{
  BREAKPOINT;
}

void psx_exit(uint32_t code)
{
  exit(code);
}

uint32_t format(uint32_t devicename)
{
  printf("format(%.8X)\n", devicename);
  BREAKPOINT;
  return 0;
}