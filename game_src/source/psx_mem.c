#include "psx_mem.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "psx_bios.h"
#include "game_viewer.h"
#include "debug.h"
#include "triangle.h"
#include "debug.h"
#include "../../config.h"
#include "decompilation.h"

//#define LOG_DISK_READ

#define DEBUG_MASK_CDROM        (1<<0)
#define DEBUG_MASK_INTERRUPT    (1<<1)
#define DEBUG_MASK_DMA          (1<<2)
#define DEBUG_MASK_DMA_TRANSFER (1<<3)
#define DEBUG_MASK_GPU          (1<<4)
#define DEBUG_MASK_GPU_GEOMETRY (1<<5)
#define DEBUG_MASK_SPU          (1<<6)
#define DEBUG_MASK_MEMCTR       (1<<7)
#define DEBUG_MASK_TIMER        (1<<8)
#define DEBUG_MASK_CONTROLLER   (1<<9)

#define DEBUG (0)

#define debug_printf(mask, ...) \
  do { \
    if (mask&DEBUG) { \
      printf(__VA_ARGS__); \
    } \
  } while(0)

#define LOC (file_loc){__FILE__, __LINE__}

#define assert2(cond) \
{ \
  if (!(cond)) BREAKPOINT; \
}

#define STATE_NONE 0
#define STATE_JOY_1 1
#define STATE_JOY_2 2
#define STATE_JOY_3 3
#define STATE_JOY_4 4

uint32_t joy_state[] = {STATE_NONE, STATE_NONE};

char *dmas[] = {
  "MDECin",
  "MDECout",
  "GPU",
  "CDROM",
  "SPU",
  "PIO",
  "OTC"
};


#define SYNC_TRANSFER 0
#define SYNC_BLOCKS 1
#define SYNC_LINK_LIST 2

char *filename = "root/SCUS_942.28;1";

struct psx_mem psx_mem;

void *addr_to_pointer(uint32_t addr)
{
  if (addr == 0) return NULL;

  if (addr >= 0x80000000 && addr < 0x80200000)
  {
    addr -= 0x80000000;
    return psx_mem.mem+addr;
  }
  else if (addr >= 0x00000000 && addr < 0x00200000)
  {
    return psx_mem.mem+addr;
  }
  else if (addr >= 0x1F800000 && addr < 0x1F800400)
  {
    addr -= 0x1F800000;
    return psx_mem.scratchpad+addr;
  }
  printf("addr 0x%.8X is not mapped to physical memory\n", addr);
  BREAKPOINT;
  return NULL;
}

uint32_t pointer_to_addr(void *ptr)
{
  uintptr_t addr = (uintptr_t)ptr - (uintptr_t)psx_mem.mem;

  if (addr < 0x200000) return ((uint32_t)addr) + 0x80000000;

  uintptr_t addr_scratchpad = (uintptr_t)ptr - (uintptr_t)psx_mem.scratchpad;

  if (addr_scratchpad < 0x400) return ((uint32_t)addr_scratchpad) + 0x1F800000;

  printf("pointer_to_addr: addr %.16lX or %.16lX incompatible\n", addr, addr_scratchpad);
  BREAKPOINT;
  return 0;
}

uint32_t pointer_to_addr_maybe(void *ptr)
{
  uintptr_t addr = (uintptr_t)ptr - (uintptr_t)psx_mem.mem;

  if (addr < 0x200000) return ((uint32_t)addr) + 0x80000000;

  uintptr_t addr_scratchpad = (uintptr_t)ptr - (uintptr_t)psx_mem.scratchpad;

  if (addr_scratchpad < 0x400) return ((uint32_t)addr_scratchpad) + 0x1F800000;

  //printf("pointer_to_addr: addr %.16lX or %.16lX incompatible\n", addr, addr_scratchpad);
  return 0xDEADBEEF;
}

void psx_read_sectors_direct(uint32_t dst, uint32_t sector, uint32_t sector_len)
{
  for (int i = 0; i < sector_len; i++) {
    uint32_t s = sector+i;
    uint32_t *ptr = (uint32_t *)(psx_mem.cdrom.disc + s*0x930 + 24);
    uint32_t *dst_ptr = addr_to_pointer(dst + i*0x800);
    for (int j = 0; j < 0x200; j++)
      *dst_ptr++ = *ptr++;
  }
}

void load_psx_exe()
{
  psx_read_sectors_direct(0x80010000, 53875+1, 204-1);
}

uint32_t stopped;

void enable_timer(void)
{
  if (stopped == 0) BREAKPOINT;
  stopped = 0;
}

void disable_timer(void)
{
  if (stopped == 1) BREAKPOINT;
  stopped = 1;
}

void inter(int type)
{
  uint32_t mask = 1<<type;

  psx_mem.I_STAT.val |= mask;
  if (psx_mem.I_MASK.val & mask) {
    disable_timer();
    debug_printf(DEBUG_MASK_INTERRUPT, "interrupt begin %d\n", type);
    interrupt(type);
    debug_printf(DEBUG_MASK_INTERRUPT, "interrupt end %d\n", type);
    enable_timer();

    if (type == 0) {
      start_frame();
    }
  }
}

int frame_delays[16] = {0};
void (*delayed_functions[16])(void) = {NULL};

void add_delayed_function(int frame_delay, void (*delayed_function)(void))
{
  for (int i = 0; i < 16; i++)
  if (frame_delays[i] == 0)
  {
    frame_delays[i] = frame_delay;
    delayed_functions[i] = delayed_function;
    return;
  }
  BREAKPOINT;
}

void psx_init_cdrom()
{
  psx_mem.cdrom.interrupt_enable = 0x1F;
  psx_mem.cdrom.status.PRMEMPT = 1;
  psx_mem.cdrom.status.PRMWRDY = 1;
  psx_mem.cdrom.stat.spindle_motor = 1;

  FILE *file = fopen(ROM_NAME, "r");
  assert2(file != NULL);

  assert2(fseek(file, 0, SEEK_END) != -1);
  int filesize = ftell(file);
  assert2(filesize != -1);
  assert2(fseek(file, 0, SEEK_SET) != -1);

  uint8_t *ptr = malloc(filesize);
  int things_read = fread(ptr, filesize, 1, file);
  assert2(things_read == 1);
  assert(fclose(file) == 0);

  psx_mem.cdrom.disc = ptr;
}

void init_psx_mem()
{
  int memory_size = 2*1024*1024 + 1024*1024 + 512*1024 + 1024 + sizeof(file_loc)*2*1024*1024;
  uint8_t *ptr = malloc(memory_size);
  
  psx_mem.mem = ptr;
  ptr += 2*1024*1024;
  psx_mem.scratchpad = ptr;
  ptr += 1024;
  psx_mem.gpu.mem = ptr;
  ptr += 1*1024*1024;
  psx_mem.spu.mem = ptr;
  ptr += 512*1024;
  psx_mem.mem_access = (void *)ptr;

  psx_mem.gpu.GPUSTAT.val = 0x94802000;

  psx_mem.memctl.spu_delay = 0x200931E1;

  psx_mem.timer.timer[2].ctrl.val = 0x1C00;

  psx_init_cdrom();

  init_game_window();

  load_psx_exe();
}

void cdrom_set_sector(uint32_t sector)
{
  psx_mem.cdrom.sector_num = sector;
  psx_mem.cdrom.disc_byte = sector * 0x930;

  psx_mem.cdrom.disc_ptr = (uint32_t *)(psx_mem.cdrom.disc + psx_mem.cdrom.disc_byte + 12);
}

void cdrom_next_sector()
{
  cdrom_set_sector(psx_mem.cdrom.sector_num+1);
}

uint32_t cdrom_set_sector_from_address(uint32_t amm, uint32_t ass, uint32_t asect)
{
  amm = (amm&15) + (amm>>4)*10;
  ass = (ass&15) + (ass>>4)*10;
  asect = (asect&15) + (asect>>4)*10;

  uint32_t sector = (amm*60+ass)*75+asect;
  sector -= 150;

  cdrom_set_sector(sector);

  return sector;
}

void interrupt_cdrom(uint32_t type)
{
  if (psx_mem.cdrom.interrupt_enable & (1<<type))
  {
    psx_mem.cdrom.interrupt_flag.response_received = type;
    inter(2);
  }
  else {
    debug_printf(DEBUG_MASK_CDROM, "interrupt enable: %.8X type %d\n", psx_mem.cdrom.interrupt_enable, type);
    BREAKPOINT;
  }
}

void cdrom_init_response()
{
  psx_mem.cdrom.status.RSLRRDY = 0;
  psx_mem.cdrom.response_fifo.length = 0;
}

void cdrom_push_response(uint8_t value)
{
  psx_mem.cdrom.status.RSLRRDY = 1;
  assert2(psx_mem.cdrom.response_fifo.length < 16);
  psx_mem.cdrom.response_fifo.parameters[psx_mem.cdrom.response_fifo.length++] = value;
}

void cdrom_readN2()
{

  if (psx_mem.cdrom.stat.read)
  {
    cdrom_init_response();
    psx_mem.cdrom.status.DRQSTS = 1;
    cdrom_push_response(psx_mem.cdrom.stat.val);

    interrupt_cdrom(1);

    if (psx_mem.cdrom.stat.read && psx_mem.I_MASK.bits.CDROM && psx_mem.cdrom.interrupt_enable & (1<<1)) {
      cdrom_next_sector();
    }
    
    if (psx_mem.cdrom.stat.read)
      add_delayed_function(1, cdrom_readN2);
  }
}

void cdrom_readN1()
{
  cdrom_init_response();
  cdrom_push_response(psx_mem.cdrom.stat.val);
  psx_mem.cdrom.stat.read = 1;
  interrupt_cdrom(3);
  psx_mem.cdrom.status.BUSYSTS = 0;

  add_delayed_function(1, cdrom_readN2);
}

void cdrom_readN()
{
  add_delayed_function(1, cdrom_readN1);
}

void cdrom_pause2()
{
  cdrom_init_response();
  cdrom_push_response(psx_mem.cdrom.stat.val);
  interrupt_cdrom(2);
  psx_mem.cdrom.status.BUSYSTS = 0;
}

void cdrom_pause1()
{
  cdrom_init_response();
  cdrom_push_response(psx_mem.cdrom.stat.val);
  interrupt_cdrom(3);
  
  add_delayed_function(1, cdrom_pause2);
}

void cdrom_pause()
{
  psx_mem.cdrom.stat.play = 0;
  psx_mem.cdrom.stat.read = 0;
  
  add_delayed_function(1, cdrom_pause1);
}

void cdrom_int3()
{
  interrupt_cdrom(3);
}

void cdrom_int1()
{
  interrupt_cdrom(3);
}

void cdrom_getstat()
{
  psx_mem.cdrom.status.BUSYSTS = 0;

  interrupt_cdrom(3);
}

void cdrom_push_getstat()
{
  add_delayed_function(1, cdrom_getstat);
}

void cdrom_cmd(uint8_t cmd, file_loc loc)
{
  debug_printf(DEBUG_MASK_CDROM, "cdrom command %.2X ", cmd);
  psx_mem.cdrom.status.BUSYSTS = 1;
  switch (cmd)
  {
  case 0x01:
    debug_printf(DEBUG_MASK_CDROM, "cdrom getstat command %.8X\n", psx_mem.I_STAT.val);
    cdrom_init_response();
    cdrom_push_response(psx_mem.cdrom.stat.val);
    cdrom_getstat();
    return;
  case 0x02: {
    debug_printf(DEBUG_MASK_CDROM, "cdrom setloc command\n");
    assert2(psx_mem.cdrom.parameter_fifo.length == 3);
    psx_mem.cdrom.parameter_fifo.length = 0;

    uint32_t amm   = psx_mem.cdrom.parameter_fifo.parameters[0];
    uint32_t ass   = psx_mem.cdrom.parameter_fifo.parameters[1];
    uint32_t asect = psx_mem.cdrom.parameter_fifo.parameters[2];

    uint32_t sector = cdrom_set_sector_from_address(amm, ass, asect);


    debug_printf(DEBUG_MASK_CDROM, "  amm  : %.2X\n", amm);
    debug_printf(DEBUG_MASK_CDROM, "  ass  : %.2X\n", ass);
    debug_printf(DEBUG_MASK_CDROM, "  asect: %.2X\n", asect);
    debug_printf(DEBUG_MASK_CDROM, "  sector: %d\n", sector);

    cdrom_init_response();
    cdrom_push_response(psx_mem.cdrom.stat.val);
    cdrom_getstat();
    return;
  }
  case 0x06: // readN
    debug_printf(DEBUG_MASK_CDROM, "cdrom readN command\n");
    cdrom_readN();
    return;
  case 0x1B: // readS
    debug_printf(DEBUG_MASK_CDROM, "cdrom readS command\n");
    cdrom_readN();
    return;
  case 0x09:
    debug_printf(DEBUG_MASK_CDROM, "cdrom pause command\n");
    cdrom_pause();
    return;
  case 0x0A:
    debug_printf(DEBUG_MASK_CDROM, "init cdrom command\n");
    cdrom_init_response();
    cdrom_push_response(psx_mem.cdrom.stat.val);
    interrupt_cdrom(3);
    cdrom_init_response();
    cdrom_push_response(psx_mem.cdrom.stat.val);
    interrupt_cdrom(2);
    return;
  case 0x0C:
    debug_printf(DEBUG_MASK_CDROM, "demute spu cdrom command\n");
    cdrom_init_response();
    cdrom_push_response(psx_mem.cdrom.stat.val);
    cdrom_getstat();
    return;
  case 0x0D:
    debug_printf(DEBUG_MASK_CDROM, "setfilter spu cdrom command\n");
    assert2(psx_mem.cdrom.parameter_fifo.length == 2);
    psx_mem.cdrom.parameter_fifo.length = 0;
    uint8_t file = psx_mem.cdrom.parameter_fifo.parameters[0];
    uint8_t channel = psx_mem.cdrom.parameter_fifo.parameters[1];
    psx_mem.cdrom.filter.file = file;
    psx_mem.cdrom.filter.channel = channel;
    debug_printf(DEBUG_MASK_CDROM, "  file: %.2X  channel: %.2X\n", file, channel);
    cdrom_init_response();
    cdrom_push_response(psx_mem.cdrom.stat.val);
    cdrom_getstat();
    return;
  case 0x0E:
    debug_printf(DEBUG_MASK_CDROM, "setmode spu cdrom command\n");
    if(psx_mem.cdrom.parameter_fifo.length != 1) {BREAKPOINT;}
    psx_mem.cdrom.parameter_fifo.length = 0;
    psx_mem.cdrom.mode.val = psx_mem.cdrom.parameter_fifo.parameters[0];
    debug_printf(DEBUG_MASK_CDROM, "  mode: %.2X\n", psx_mem.cdrom.parameter_fifo.parameters[0]);
    cdrom_init_response();
    cdrom_push_response(psx_mem.cdrom.stat.val);
    cdrom_getstat();
    return;
  case 0x11: {
    debug_printf(DEBUG_MASK_CDROM, "GetlocP spu cdrom command\n");
    
    uint32_t sector = psx_mem.cdrom.sector_num;

    uint32_t sect = sector%150;
    uint32_t ss = sector/150;
    uint32_t mm = ss/60;
    ss = ss % 60;

    cdrom_init_response();
    cdrom_push_response(1);
    cdrom_push_response(1);
    cdrom_push_response(mm);
    cdrom_push_response(ss);
    cdrom_push_response(sect);
    cdrom_push_response(mm);
    cdrom_push_response(ss+2);
    cdrom_push_response(sect);
    cdrom_getstat();
    return;
  }
  default:
    printf("file %s:%u: unknown cdrom command %.2X\n", loc.file, loc.line, cmd);
    BREAKPOINT;
    return;
  }
}

#define STREAM_STATE_NONE 0
#define STREAM_STATE_CPU_TO_VRAM 1
#define STREAM_STATE_VRAM_TO_CPU 2

uint32_t stream_state = STREAM_STATE_NONE;
uint32_t stream_params = 0;

void GP0_stream(gp0_cmd param_list[static 16], uint32_t len, uint32_t value)
{
  static uint32_t i = 0;
  switch (stream_state)
  {
  case STREAM_STATE_CPU_TO_VRAM: {
    gp0_cmd p1 = param_list[1];
    gp0_cmd p2 = param_list[2];
    
    uint32_t x = (i*2)%p2.res.x+p1.res.x;
    uint32_t y = (i*2)/p2.res.x+p1.res.y;

    uint32_t *ptr = (uint32_t *)psx_mem.gpu.mem;

    ptr[(x+y*1024)/2] = value;

    i++;
    if (i == (p2.res.x*p2.res.y+1)/2)
    {
      debug_printf(DEBUG_MASK_DMA, "CPU-to-VRAM done\n");
      i = 0;

      stream_state = STREAM_STATE_NONE;

      psx_mem.gpu.GPUSTAT.cmd_ready = 1;
    }
  }
    break;
  default:
    BREAKPOINT;
  }
}

uint32_t stream_state2;

uint32_t GP0_read_stream(gp0_cmd param_list[static 16], uint32_t len)
{
  static uint32_t i = 0;
  switch (stream_state2)
  {
  case STREAM_STATE_VRAM_TO_CPU: {
    gp0_cmd p1 = param_list[1];
    gp0_cmd p2 = param_list[2];
    
    uint32_t x = (i*2)%p2.res.x+p1.res.x;
    uint32_t y = (i*2)/p2.res.x+p1.res.y;

    uint32_t *ptr = (uint32_t *)psx_mem.gpu.mem;
    uint32_t value = ptr[(x+y*1024)/2];

    i++;
    if (i*2 == p2.res.x*p2.res.y)
    {
      debug_printf(DEBUG_MASK_DMA, "VRAM-to-CPU done\n");
      i = 0;
      stream_state2 = STREAM_STATE_NONE;

      psx_mem.gpu.GPUSTAT.vram_ready = 0;
      psx_mem.gpu.GPUSTAT.cmd_ready = 1;
    }

    return value;
  }
    break;
  default:
    BREAKPOINT;
  }
  return -1;
}

void fill_rect(uint16_t col, uint16_t x, uint16_t y, uint16_t sx, uint16_t sy)
{
  uint16_t *mem = (uint16_t *)psx_mem.gpu.mem;

  assert(y+sy < 512);
  assert(x+sx < 1024);

  for (int Y = y; Y < y+sy; Y++)
    for (int X = x; X < x+sx; X++)
      mem[X+Y*1024] = col;
}

color blend_col(color old, color new, uint32_t transparency)
{
  int32_t r;
  int32_t g;
  int32_t b;
  int32_t oldR = old.r;
  int32_t oldG = old.g;
  int32_t oldB = old.b;
  int32_t newR = new.r;
  int32_t newG = new.g;
  int32_t newB = new.b;
  switch (transparency)
  {
  case 0:
    return (color){
      .r = (old.r+new.r)/2,
      .g = (old.g+new.g)/2,
      .b = (old.b+new.b)/2,
    };
  case 1:
    r = oldR+newR;
    g = oldG+newG;
    b = oldB+newB;

    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;

    return (color){
      .r = r,
      .g = g,
      .b = b,
    };
  case 2:
    r = oldR-newR;
    g = oldG-newG;
    b = oldB-newB;

    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;

    return (color){
      .r = r,
      .g = g,
      .b = b,
    };
  case 3:
    r = oldR+newR/4;
    g = oldG+newG/4;
    b = oldB+newB/4;

    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;

    return (color){
      .r = r,
      .g = g,
      .b = b,
    };
  default:
    printf("BLENDING: %d\n", transparency);
    BREAKPOINT;
  }
  return (color){0,0,0};
}

uint32_t blend;

struct {
  poly poly;
  union {
    uint32_t val;
    struct draw_mode draw_mode;
  } draw_mode;
  uint16_t clut;
} tri_info;

int dither_map[4][4] = {
  {-4, +0, -3, +1},
  {+2, -2, +3, -1},
  {-3, +1, -4, +0},
  {+3, -1, +2, -2},
};

void set_pixel(vertex v)
{
  int32_t x = v.v.x+psx_mem.gpu.area.x;
  int32_t y = v.v.y+psx_mem.gpu.area.y;

  uint32_t x1 = psx_mem.gpu.area.x1;
  uint32_t y1 = psx_mem.gpu.area.y1;
  uint32_t x2 = psx_mem.gpu.area.x2;
  uint32_t y2 = psx_mem.gpu.area.y2;

  if (x < x1 || x >= x2) return;
  if (y < y1 || y >= y2) return;

  int tex_trans = 0;

  uint16_t *ptr = (uint16_t *)psx_mem.gpu.mem;
  color c = v.c;
  if (tri_info.poly.textured)
  {
    uint32_t pagex = tri_info.draw_mode.draw_mode.tex_x_base;
    uint32_t pagey = tri_info.draw_mode.draw_mode.tex_y_base_1;
    uint32_t uvx, uvy;

    uint16_t *texels = (uint16_t *)psx_mem.gpu.mem;

    uint16_t colcol;

    color c2;

    uint32_t clut = tri_info.clut;

    int cx = (clut & 0x3F) * 16;
    int cy = (clut >> 6) & 0x1FF;

    switch (tri_info.draw_mode.draw_mode.texture_bits) {
      case 0: { //4 bits
        uvx = v.uv.x + pagex * 256;
        uvy = v.uv.y + pagey * 256;
        uint16_t texel_pack = texels[uvx/4 + uvy * 1024];

        uint16_t texel = (texel_pack >> ((uvx & 3) * 4)) & 0xF;

        colcol = texels[(texel + cx + cy * 1024)];

        tex_trans = (colcol >> 15);
        break;
      }
      case 1: { //8 bits
        uvx = v.uv.x + pagex * 128;
        uvy = v.uv.y + pagey * 256;
        uint16_t texel_pack = texels[uvx/2 + uvy * 1024];

        uint16_t texel = (texel_pack >> ((uvx & 1) * 8)) & 0xFF;

        colcol = texels[texel + cx + cy * 1024];

        tex_trans = (colcol >> 15);
        break;
      }
      case 2: //16 bits
        printf("16 bit textures\n");
        BREAKPOINT;
      case 3: BREAKPOINT;
    }

    if (colcol == 0) return;

    c2 = (color){
      ((colcol >>  0) & 0x1F) << 3,
      ((colcol >>  5) & 0x1F) << 3,
      ((colcol >> 10) & 0x1F) << 3
    };

    if (tri_info.poly.raw_texture)
      c = c2;
    else {
      int r = (c.r * c2.r) / 128;
      int g = (c.g * c2.g) / 128;
      int b = (c.b * c2.b) / 128;
      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;
      c = (color){
        .r = r,
        .g = g,
        .b = b
      };
    }
  }

  if (tri_info.poly.transparent || tex_trans)
  {
    uint16_t old = ptr[x+y*1024];
    color old_col = (color){
      ((old >>  0) & 0x1F) << 3,
      ((old >>  5) & 0x1F) << 3,
      ((old >> 10) & 0x1F) << 3
    };

    c = blend_col(old_col, c, tri_info.draw_mode.draw_mode.semi_transparency);
  }

  int r = c.r;
  int g = c.g;
  int b = c.b;

  if (tri_info.draw_mode.draw_mode.dither_24_to_15) {
    int dither = dither_map[y&3][x&3];
    r += dither;
    g += dither;
    b += dither;
    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
  }

  uint16_t col16 = (r>>3) | ((g>>3)<<5) | ((b>>3)<<10);

  ptr[x+y*1024] = col16;
}

void set_pixel_line(vertex v)
{
  int32_t x = v.v.x+psx_mem.gpu.area.x;
  int32_t y = v.v.y+psx_mem.gpu.area.y;

  uint32_t x1 = psx_mem.gpu.area.x1;
  uint32_t y1 = psx_mem.gpu.area.y1;
  uint32_t x2 = psx_mem.gpu.area.x2;
  uint32_t y2 = psx_mem.gpu.area.y2;

  if (x < x1 || x >= x2) return;
  if (y < y1 || y >= y2) return;

  uint16_t *ptr = (uint16_t *)psx_mem.gpu.mem;
  color c = v.c;

  int r = c.r;
  int g = c.g;
  int b = c.b;

  if (tri_info.draw_mode.draw_mode.dither_24_to_15) {
    int dither = dither_map[y&3][x&3];
    r += dither;
    g += dither;
    b += dither;
    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
  }

  uint16_t col16 = (r>>3) | ((g>>3)<<5) | ((b>>3)<<10);

  ptr[x+y*1024] = col16;
}

void gpu_triangle(gp0_cmd *params)
{
  gp0_cmd type = params[0];

  tri_info.poly = type.poly;
  tri_info.draw_mode.draw_mode = psx_mem.gpu.draw_mode;

  uint32_t col1 = params[0].val & 0xFFFFFF;
  uint32_t col2, col3, col4;

  if (type.poly.gouraud)
  {
    if (type.poly.textured)
    {
      col2 = params[3].val & 0xFFFFFF;
      col3 = params[6].val & 0xFFFFFF;
      col4 = params[9].val & 0xFFFFFF;
    }
    else
    {
      col2 = params[2].val;
      col3 = params[4].val;
      col4 = params[6].val;
    }
  }
  else
  {
    col2 = col1;
    col3 = col1;
    col4 = col1;
  }

  uint32_t v1, v2, v3, v4;
  int16_t x1, x2, x3, x4;
  int16_t y1, y2, y3, y4;

  v1 = params[1].val;

  if (type.poly.gouraud)
  {
    if (type.poly.textured)
    {
      v2 = params[4].val;
      v3 = params[7].val;
      v4 = params[10].val;
    }
    else
    {
      v2 = params[3].val;
      v3 = params[5].val;
      v4 = params[7].val;
    }
  }
  else
  {
    if (type.poly.textured)
    {
      v2 = params[3].val;
      v3 = params[5].val;
      v4 = params[7].val;
    }
    else
    {
      v2 = params[2].val;
      v3 = params[3].val;
      v4 = params[4].val;
    }
  }

  x1 = v1 & 0xFFFF;
  y1 = v1 >> 16;
  x2 = v2 & 0xFFFF;
  y2 = v2 >> 16;
  x3 = v3 & 0xFFFF;
  y3 = v3 >> 16;
  x4 = v4 & 0xFFFF;
  y4 = v4 >> 16;

  uint32_t t1, t2, t3, t4;

  t1 = params[2].val;
  if (type.poly.gouraud)
  {
    t2 = params[5].val;
    t3 = params[8].val;
    t4 = params[11].val;
  }
  else
  {
    t2 = params[4].val;
    t3 = params[6].val;
    t4 = params[8].val;
  }


  if (type.poly.textured || type.poly.raw_texture) {
    uint32_t clut, page;
    clut = t1 >> 16;
    page = t2 >> 16;

    tri_info.clut = clut;
    tri_info.draw_mode.val = (tri_info.draw_mode.val & 0xFFFFF600) | (page & 0x000009FF);
  }

  t1 = t1 & 0xFFFF;
  t2 = t2 & 0xFFFF;
  t3 = t3 & 0xFFFF;
  t4 = t4 & 0xFFFF;

  uint32_t u1, u2, u3, u4;
  uint32_t w1, w2, w3, w4;

  u1 = t1&0xFF; w1 = t1>>8;
  u2 = t2&0xFF; w2 = t2>>8;
  u3 = t3&0xFF; w3 = t3>>8;
  u4 = t4&0xFF; w4 = t4>>8;

  color c1 = {col1&0xFF, (col1>>8)&0xFF, (col1>>16)&0xFF};
  color c2 = {col2&0xFF, (col2>>8)&0xFF, (col2>>16)&0xFF};
  color c3 = {col3&0xFF, (col3>>8)&0xFF, (col3>>16)&0xFF};
  color c4 = {col4&0xFF, (col4>>8)&0xFF, (col4>>16)&0xFF};

  int x = psx_mem.gpu.area.x;
  int y = psx_mem.gpu.area.y;

  draw_triangle(
    (vertex[]){
      {{x1, y1}, {u1, w1}, c1},
      {{x2, y2}, {u2, w2}, c2},
      {{x3, y3}, {u3, w3}, c3}
    },
    psx_mem.gpu.area.x1-x,
    psx_mem.gpu.area.x2-x,
    psx_mem.gpu.area.y1-y,
    psx_mem.gpu.area.y2-y,
    set_pixel
  );

  if (type.poly.quad)
    draw_triangle(
      (vertex[]){
        {{x2, y2}, {u2, w2}, c2},
        {{x3, y3}, {u3, w3}, c3},
        {{x4, y4}, {u4, w4}, c4}
      },
      psx_mem.gpu.area.x1-x,
      psx_mem.gpu.area.x2-x,
      psx_mem.gpu.area.y1-y,
      psx_mem.gpu.area.y2-y,
      set_pixel
    );
}

void gpu_line(gp0_cmd *params)
{
  gp0_cmd type = params[0];

  uint32_t col1 = params[0].val & 0xFFFFFF;
  uint32_t col2;

  if (type.poly.gouraud)
  {
    col2 = params[2].val;
  } else {
    col2 = col1;
  }

  color c1 = {col1&0xFF, (col1>>8)&0xFF, (col1>>16)&0xFF};
  color c2 = {col2&0xFF, (col2>>8)&0xFF, (col2>>16)&0xFF};

  uint32_t xy1 = params[1].val;
  uint32_t xy2;
  
  if (type.poly.gouraud)
  {
    xy2 = params[3].val;
  } else {
    xy2 = params[2].val;
  }

  int16_t x1 = xy1;
  int16_t y1 = xy1 >> 16;
  int16_t x2 = xy2;
  int16_t y2 = xy2 >> 16;

  int x = psx_mem.gpu.area.x;
  int y = psx_mem.gpu.area.y;

  draw_line(
      (vertex[]){
        {.v = {x1, y1}, .c = c1},
        {.v = {x2, y2}, .c = c2}
      },
      psx_mem.gpu.area.x1-x,
      psx_mem.gpu.area.x2-x,
      psx_mem.gpu.area.y1-y,
      psx_mem.gpu.area.y2-y,
      set_pixel_line
    );
}

void gpu_rect(gp0_cmd *params)
{
  BREAKPOINT;
  gp0_cmd type = params[0];
  gp0_cmd xy = params[1];
  uint32_t x = xy.res.x;
  uint32_t y = xy.res.y;
  uint32_t sx;
  uint32_t sy;

  uint32_t i = 2;

  if (type.rect.textured)
  {
    i++;
    BREAKPOINT;
  }

  switch (type.rect.size)
  {
  case 0: {
    gp0_cmd size = params[i];
    i++;
    sx = size.res.x;
    sy = size.res.y;
    break;
  }
  case 1:
    sx = 1; sy = 1;
    break;
  case 2:
    sx = 8; sy = 8;
    break;
  case 3:
    sx = 16; sy = 16;
    break;
  }

  uint16_t *ptr = (uint16_t *)psx_mem.gpu.mem;

  for (int j = 0; j < sy+10; j++)
  for (int i = 0; i < sx+10; i++)
  {
    uint32_t x2 = x+i;
    uint32_t y2 = y+i;

    int32_t r = type.rect.r;
    int32_t g = type.rect.g;
    int32_t b = type.rect.b;
    uint16_t col16 = (r>>3) | ((g>>3)<<5) | ((b>>3)<<10);
    ptr[x2+y2*1024] = col16;

  }
}

void gpu_blit(gp0_cmd *params)
{
  uint32_t src = params[1].val;
  uint32_t dst = params[2].val;
  uint32_t size = params[3].val;

  uint16_t srcX = src & 0xFFFF;
  uint16_t srcY = src >> 16;
  
  uint16_t dstX = dst & 0xFFFF;
  uint16_t dstY = dst >> 16;
  
  uint16_t sizeX = size & 0xFFFF;
  uint16_t sizeY = size >> 16;

  uint16_t *mem = (uint16_t *)psx_mem.gpu.mem;
  uint16_t *mem2 = (uint16_t *)psx_mem.gpu.mem;

  assert(srcX+sizeX <= 1024);
  assert(srcY+sizeY <= 512);

  for (uint16_t y = 0; y < sizeY; y++)
  for (uint16_t x = 0; x < sizeX; x++)
  {
    mem2[x+dstX+(y+dstY)*1024] = mem[x+srcX+(y+srcY)*1024];
  }
}

uint32_t params = 0;
gp0_cmd param_list[16];
uint32_t param_list_len = 0;
void GP0_cmd(uint32_t value, file_loc loc)
{
  if (stream_state != STREAM_STATE_NONE)
  {
    GP0_stream(param_list, param_list_len, value);
    return;
  }

  if (params > 0)
  {
    debug_printf(DEBUG_MASK_GPU, "  file %s:%u: param %d: %.8X\n", loc.file, loc.line, params, value);
    param_list[param_list_len].val = value;
    param_list_len++;
    params--;

    if (params == 0)
    {
      gp0_cmd param = param_list[0];

      switch (param.cmd) {
      case 0: {
        uint32_t cmd = param_list[0].val >> 24;
        switch (cmd) {
        case 2: {
          uint32_t color = param_list[0].val & 0x00FFFFFF;
          uint32_t topleft = param_list[1].val;
          uint32_t size = param_list[2].val;
          uint16_t x = topleft&0xFFFF;
          uint16_t y = topleft>>16;
          uint16_t sx = size&0xFFFF;
          uint16_t sy = size>>16;

          uint16_t col = ((color>>19)&0x1F)<<10;
          col |= ((color>>11)&0x1F)<<5;
          col |= (color>>3)&0x1F;

          fill_rect(col, x, y, sx, sy);
          psx_mem.gpu.GPUSTAT.cmd_ready = 1;
        }
          break;
        default:
          BREAKPOINT;
        }
      }
        break;
      case 1:
        gpu_triangle(param_list);
        psx_mem.gpu.GPUSTAT.cmd_ready = 1;
        break;
      case 2:
        gpu_line(param_list);
        psx_mem.gpu.GPUSTAT.cmd_ready = 1;
        break;
      case 3:
        gpu_rect(param_list);
        psx_mem.gpu.GPUSTAT.cmd_ready = 1;
        break;
      case 4:
        gpu_blit(param_list);
        psx_mem.gpu.GPUSTAT.cmd_ready = 1;
        break;
        BREAKPOINT;
      case 5:
        stream_state = STREAM_STATE_CPU_TO_VRAM;
        break;
      case 6:
        stream_state2 = STREAM_STATE_VRAM_TO_CPU;
        psx_mem.gpu.GPUSTAT.vram_ready = 1;
        break;
      default:
        BREAKPOINT;
      }
      param_list_len = 0;
    }

    return;
  }

  gp0_cmd param;
  param.val = value;
  uint32_t cmd;
  switch (param.cmd) {
  case 0:
    debug_printf(DEBUG_MASK_GPU, "file %s:%u: GP0 Misc commands %.8X\n", loc.file, loc.line, value);

    psx_mem.gpu.GPUSTAT.cmd_ready = 1;
    cmd = value>>24;
    switch (cmd) {
    case 0:
      // nop
      break;
    case 1:
      // flush cache
      break;
    case 2:
      param_list[0].val = value;
      param_list_len++;
      params = 2;
      psx_mem.gpu.GPUSTAT.cmd_ready = 0;
      break;
    default:
      printf("file %s:%u: GP0 cmd %d not supported\n", loc.file, loc.line, value);
      BREAKPOINT;
    }
    break;
  case 1:
    debug_printf(DEBUG_MASK_GPU_GEOMETRY, "file %s:%u: GP0 Polygon primitive %.8X: ", loc.file, loc.line, value);

    psx_mem.gpu.GPUSTAT.cmd_ready = 0;
    if (param.poly.gouraud)     debug_printf(DEBUG_MASK_GPU_GEOMETRY, "g");
    else                        debug_printf(DEBUG_MASK_GPU_GEOMETRY, "-");
    if (param.poly.raw_texture) debug_printf(DEBUG_MASK_GPU_GEOMETRY, "r");
    else                        debug_printf(DEBUG_MASK_GPU_GEOMETRY, "-");
    if (param.poly.transparent) debug_printf(DEBUG_MASK_GPU_GEOMETRY, "l");
    else                        debug_printf(DEBUG_MASK_GPU_GEOMETRY, "-");
    if (param.poly.textured)    debug_printf(DEBUG_MASK_GPU_GEOMETRY, "t");
    else                        debug_printf(DEBUG_MASK_GPU_GEOMETRY, "-");
    if (param.poly.quad)        debug_printf(DEBUG_MASK_GPU_GEOMETRY, "q");
    else                        debug_printf(DEBUG_MASK_GPU_GEOMETRY, "-");

    debug_printf(DEBUG_MASK_GPU_GEOMETRY, " %.6X\n", param.val & 0xFFFFFF);

    if (param.poly.gouraud == 1 && param.poly.quad == 0 && param.poly.textured == 0)
    {
      param_list[0].val = value;
      param_list_len++;
      params = 5;
    }
    else if (param.poly.gouraud == 0 && param.poly.quad == 0 && param.poly.textured == 0)
    {
      param_list[0].val = value;
      param_list_len++;
      params = 3;
    }
    else if (param.poly.gouraud == 1 && param.poly.quad == 1 && param.poly.textured == 0)
    {
      param_list[0].val = value;
      param_list_len++;
      params = 7;
    }
    else if (param.poly.gouraud == 1 && param.poly.quad == 1 && param.poly.textured == 1)
    {
      param_list[0].val = value;
      param_list_len++;
      params = 11;
    }
    else if (param.poly.gouraud == 1 && param.poly.quad == 0 && param.poly.textured == 1)
    {
      param_list[0].val = value;
      param_list_len++;
      params = 8;
    }
    else if (param.poly.gouraud == 0 && param.poly.quad == 1 && param.poly.textured == 0)
    {
      param_list[0].val = value;
      param_list_len++;
      params = 4;
    }
    else if (param.poly.gouraud == 0 && param.poly.quad == 1 && param.poly.textured == 1)
    {
      param_list[0].val = value;
      param_list_len++;
      params = 8;
    }
    else if (param.poly.gouraud == 0 && param.poly.quad == 0 && param.poly.textured == 1)
    {
      param_list[0].val = value;
      param_list_len++;
      params = 6;
    }
    else
    {
      printf("file %s:%u: GP0 Polygon primitive %.8X: ", loc.file, loc.line, value);

      if (param.poly.gouraud)     printf("g");
      else                        printf("-");
      if (param.poly.raw_texture) printf("r");
      else                        printf("-");
      if (param.poly.transparent) printf("l");
      else                        printf("-");
      if (param.poly.textured)    printf("t");
      else                        printf("-");
      if (param.poly.quad)        printf("q");
      else                        printf("-");

      printf("\n");

      BREAKPOINT;
    }

    break;
  case 2:
    debug_printf(DEBUG_MASK_GPU_GEOMETRY, "file %s:%u: GP0 Line primitive %.8X: ", loc.file, loc.line, value);

    psx_mem.gpu.GPUSTAT.cmd_ready = 0;
    if (param.poly.gouraud)     debug_printf(DEBUG_MASK_GPU_GEOMETRY, "g");
    else                        debug_printf(DEBUG_MASK_GPU_GEOMETRY, "-");
    if (param.poly.transparent) debug_printf(DEBUG_MASK_GPU_GEOMETRY, "l");
    else                        debug_printf(DEBUG_MASK_GPU_GEOMETRY, "-");
    if (param.poly.quad)        debug_printf(DEBUG_MASK_GPU_GEOMETRY, "p");
    else                        debug_printf(DEBUG_MASK_GPU_GEOMETRY, "-");

    debug_printf(DEBUG_MASK_GPU_GEOMETRY, "\n");

    if (param.poly.quad == 0 && param.poly.gouraud == 1)
    {
      param_list[0].val = value;
      param_list_len++;
      params = 3;
    }
    else if (param.poly.quad == 0 && param.poly.gouraud == 0)
    {
      param_list[0].val = value;
      param_list_len++;
      params = 2;
    } else {
      printf("ERROR: file %s:%u: GP0 Line primitive %.8X: ", loc.file, loc.line, value);
      if (param.poly.gouraud)     printf("g");
      else                        printf("-");
      if (param.poly.transparent) printf("l");
      else                        printf("-");
      if (param.poly.quad)        printf("p");
      else                        printf("-");
      printf("\n");
      BREAKPOINT;
    }

    break;
  case 3:
    BREAKPOINT;
    debug_printf(DEBUG_MASK_GPU_GEOMETRY, "file %s:%u: GP0 Rectangle primitive %.8X\n", loc.file, loc.line, value);

    psx_mem.gpu.GPUSTAT.cmd_ready = 0;

    if (param.rect.transparent == 0 && param.rect.raw_texture == 0 && param.rect.textured == 0 && param.rect.size != 0) {
      param_list[0].val = value;
      param_list_len++;
      params = 1;
    } else if (param.rect.transparent == 1 && param.rect.raw_texture == 1 && param.rect.textured == 0 && param.rect.size != 0) {
      param_list[0].val = value;
      param_list_len++;
      params = 1;
    } else if (param.rect.transparent == 1 && param.rect.raw_texture == 1 && param.rect.textured == 1 && param.rect.size != 0) {
      param_list[0].val = value;
      param_list_len++;
      params = 2;
    } else {
      printf("file %s:%u: GP0 Rectangle primitive %.8X: size: %d ", loc.file, loc.line, value, param.rect.size);

      if (param.rect.raw_texture) printf("r");
      else                        printf("-");
      if (param.rect.transparent) printf("l");
      else                        printf("-");
      if (param.rect.textured)    printf("t");
      else                        printf("-");

      printf("\n");
      printf("\n");
      BREAKPOINT;
    }
    break;
  case 4:
    debug_printf(DEBUG_MASK_GPU, "file %s:%u: GP0 VRAM-to-VRAM blit %.8X\n", loc.file, loc.line, value);
    param_list[0].val = value;
    param_list_len++;
    params = 3;
    break;
  case 5:
    debug_printf(DEBUG_MASK_GPU, "file %s:%u: GP0 CPU-to-VRAM blit %.8X\n", loc.file, loc.line, value);
    param_list[0].val = value;
    param_list_len++;
    params = 2;
    psx_mem.gpu.GPUSTAT.cmd_ready = 0;
    break;
  case 6:
    debug_printf(DEBUG_MASK_GPU, "file %s:%u: GP0 VRAM-to-CPU blit %.8X\n", loc.file, loc.line, value);
    param_list[0].val = value;
    param_list_len++;
    params = 2;
    psx_mem.gpu.GPUSTAT.cmd_ready = 0;
    break;
  case 7:
    debug_printf(DEBUG_MASK_GPU, "file %s:%u: GP0 Environment command %.8X:", loc.file, loc.line, value);

    cmd = value>>24;

    switch (cmd) {
      case 0xE1:
        debug_printf(DEBUG_MASK_GPU, " draw mode setting");

        psx_mem.gpu.draw_mode = param.draw_mode;
        break;
      case 0xE2: {
        uint32_t maskx = param.tex_win.maskx * 8;
        uint32_t masky = param.tex_win.masky * 8;
        uint32_t offsx = param.tex_win.offsx * 8;
        uint32_t offsy = param.tex_win.offsy * 8;

        debug_printf(DEBUG_MASK_GPU, " texture window setting %d %d %d %d", maskx, masky, offsx, offsy);
      }
        break;
      case 0xE3: {
        uint32_t x = param.area.x;
        uint32_t y = param.area.y;
        psx_mem.gpu.area.x1 = x;
        psx_mem.gpu.area.y1 = y;
        debug_printf(DEBUG_MASK_GPU, " set drawing area top left: %d %d", x, y);
      }
        break;
      case 0xE4: {
        uint32_t x = param.area.x;
        uint32_t y = param.area.y;
        psx_mem.gpu.area.x2 = x;
        psx_mem.gpu.area.y2 = y;
        debug_printf(DEBUG_MASK_GPU, " set drawing area bottom right: %d %d", x, y);
      }
        break;
      case 0xE5: {
        int32_t x = param.offs.x;
        int32_t y = param.offs.y;

        if (x & 0x800) x -= 0x800;
        if (y & 0x800) y -= 0x800;
        psx_mem.gpu.area.x = x;
        psx_mem.gpu.area.y = y;

        debug_printf(DEBUG_MASK_GPU, " set drawing offset: %d %d", x, y);
      }
        break;
      case 0xE6: {
        uint32_t props = value&3;
        psx_mem.gpu.GPUSTAT.draw_mask_set = props&1;
        psx_mem.gpu.GPUSTAT.use_draw_mask = (props>>1)&1;

        if (psx_mem.gpu.GPUSTAT.draw_mask_set)
          debug_printf(DEBUG_MASK_GPU, " draw to mask");

        if (psx_mem.gpu.GPUSTAT.draw_mask_set && psx_mem.gpu.GPUSTAT.use_draw_mask)
          debug_printf(DEBUG_MASK_GPU, " and");

        if (psx_mem.gpu.GPUSTAT.use_draw_mask)
          debug_printf(DEBUG_MASK_GPU, " use draw mask");
      }
        break;
      default:
        printf("file %s:%u: cmd: %X, %X\n", loc.file, loc.line, cmd, value);
        BREAKPOINT;
    }
    debug_printf(DEBUG_MASK_GPU, "\n");
    break;
  default:
    printf("file %s:%u: unknown GP0 command %2.2X %.8X\n", loc.file, loc.line, value, value);
    BREAKPOINT;
    break;
  }
}

void gpu_info(uint32_t info_type)
{
  switch (info_type) {
  case 7:
    // emulating psx gpu type 2
    psx_mem.gpu.GPUREAD = 2;
    break;
  default:
    printf("gpu info type %d not implemented\n", info_type);
    BREAKPOINT;
  }
}

char *dma_directions[] = {
  "Off", "FIFO", "CPUtoGP0", "GPUREADtoCPU"
};

void GP1_cmd(uint32_t value, file_loc loc)
{
  psx_mem.gpu.GP1.val = value;
  switch (psx_mem.gpu.GP1.cmd) {
  case 0x00:
    debug_printf(DEBUG_MASK_GPU, "file %s:%u: GP1 reset GPU\n", loc.file, loc.line);
    break;
  case 0x01:
    debug_printf(DEBUG_MASK_GPU, "file %s:%u: GP1 reset command buffer\n", loc.file, loc.line);
    break;
  case 0x03:
  {
    char *onoff[] =  {
      "on", "off"
    };
    debug_printf(DEBUG_MASK_GPU, "file %s:%u: GP1 display enable %s\n", loc.file, loc.line, onoff[value&1]);
    break;
  }
  case 0x04:
    debug_printf(DEBUG_MASK_GPU, "file %s:%u: GP1 set DMA direction: %s\n", loc.file, loc.line, dma_directions[value&3]);
    psx_mem.gpu.GPUSTAT.dma_dir = value&3;
    break;
  case 0x05:
    debug_printf(DEBUG_MASK_GPU, "file %s:%u: GP1 start of display area, X: %d, Y: %d | %.8X\n", loc.file, loc.line, psx_mem.gpu.GP1.coord.Y1, psx_mem.gpu.GP1.coord.Y2, psx_mem.gpu.GP1.val);
    psx_mem.gpu.display_area_start = value;
    break;
  case 0x06:
    debug_printf(DEBUG_MASK_GPU, "file %s:%u: GP1 horizontal display range, X1: %d, X2: %d | %.8X\n", loc.file, loc.line, psx_mem.gpu.GP1.coord2.X1, psx_mem.gpu.GP1.coord2.X2, psx_mem.gpu.GP1.val);
    break;
  case 0x07:
    debug_printf(DEBUG_MASK_GPU, "file %s:%u: GP1 vertical display range, Y1: %d, Y2: %d | %.8X\n", loc.file, loc.line, psx_mem.gpu.GP1.coord.Y1, psx_mem.gpu.GP1.coord.Y2, psx_mem.gpu.GP1.val);
    break;
  case 0x08:
    psx_mem.gpu.display_mode.val = value;
    debug_printf(DEBUG_MASK_GPU, "file %s:%u: GP1 display mode | %.8X\n", loc.file, loc.line, psx_mem.gpu.GP1.val);
    break;
  case 0x10:
    debug_printf(DEBUG_MASK_GPU, "file %s:%u: GP1 get GPU info %d\n", loc.file, loc.line, psx_mem.gpu.GP1.param);
    gpu_info(psx_mem.gpu.GP1.param);
    break;
  default:
    debug_printf(DEBUG_MASK_GPU, "file %s:%u: GP1 cmd nr. %X -----------\n", loc.file, loc.line, psx_mem.gpu.GP1.cmd);
    break;
  }
}

#undef sw
#undef sh
#undef sb
#undef lw
#undef lh
#undef lb
#undef lhu
#undef lbu
#undef sw_unaligned
#undef lw_unaligned

void sw(uint32_t addr, uint32_t value, file_loc loc);
void sh(uint32_t addr, uint16_t value, file_loc loc);
void sb(uint32_t addr, uint8_t value, file_loc loc);
uint32_t lw(uint32_t addr, file_loc loc);
uint32_t lh(uint32_t addr, file_loc loc);
uint32_t lb(uint32_t addr, file_loc loc);
uint32_t lhu(uint32_t addr, file_loc loc);
uint32_t lbu(uint32_t addr, file_loc loc);

#define MASK_READ   (1 << 0)
#define MASK_WRITE  (1 << 1)
#define MASK_ACCESS (MASK_READ | MASK_WRITE)
#define MASK_PANIC  (1 << 2)
#define MASK_READONLY (MASK_PANIC | MASK_WRITE)
#define MASK_FORBIDDEN (MASK_PANIC | MASK_ACCESS)

struct segment {
  uint32_t base;
  uint32_t size;
  uint32_t operation_mask;
};

struct segment segments[] = {
  {LEVEL_ID, 4, MASK_WRITE},
  {0x80074A10, 4, MASK_READONLY},
  {0x80075888, 4, MASK_ACCESS}, // BACKBUFFER_DISP
  {0x80010000, 0x5BBDC, MASK_READONLY}
};

void report_addr(uint32_t addr, uint32_t size, file_loc loc, char *func, uint32_t value)
{
  for (int i = 0; i < sizeof(segments)/sizeof(struct segment); i++)
  {
    struct segment segment = segments[i];

    if (addr >= segment.base && addr < segment.base+segment.size) {
      if (!(segment.operation_mask & MASK_READ) && func[0] == 'l')
        continue;

      if (!(segment.operation_mask & MASK_WRITE) && func[0] == 's')
        continue;

      if (func[0] == 's')
        printf("---------- file %s:%u: %s(0x%.8X, 0x%.8X)\n", loc.file, loc.line, func, addr, value);

      if (func[0] == 'l')
        printf("---------- file %s:%u: %s(0x%.8X) -> 0x%.8X\n", loc.file, loc.line, func, addr, *(uint32_t*)addr_to_pointer(addr));

      if (segment.operation_mask & MASK_PANIC) {
        BREAKPOINT;
      }
    }
  }
}

void check_addr(uint32_t addr, uint32_t size, file_loc loc, char *func, uint32_t value)
{
  report_addr(addr, size, loc, func, value);
  
  if (addr >= 0x80000000 && addr <= 0x80200000-size)
    return;
  
  if (addr >= 0x1F800000 && addr <= 0x1F800400-size)
    return;

  printf("%s: file %s:%u: address %X outside range\n", func, loc.file, loc.line, addr);
  BREAKPOINT;
}

file_loc get_access(uint32_t addr)
{
  return psx_mem.mem_access[addr&0x001FFFFF];
}

void print_access(uint32_t addr)
{
  file_loc loc = get_access(addr);

  printf("0x%.8X (0x%.2X) written by: %s:%u\n", addr, lbu(addr, LOC), loc.file, loc.line);
}

uint32_t translate_addr(uint32_t addr)
{
  if (addr < 0x00200000) 
    return addr | 0x80000000;

  if (addr >= 0xA0000000 && addr < 0xA0200000)
    return 0x80000000 | (addr&0x001FFFFF);

  return addr;
}

void interrupt_dma(uint32_t dma_num)
{
  uint32_t mask = 1<<dma_num;
  if (psx_mem.dma.DICR.irq_master_enable)
  {
    if (psx_mem.dma.DICR.irq_enable & mask)
    {
      psx_mem.dma.DICR.irq_flags |= mask;
      inter(3);
    }
  }
}

void enable_dma(uint32_t dma_num)
{
  struct dma dma = psx_mem.dma.DMA[dma_num];

  if (dma.chcr.chopping) BREAKPOINT;

  debug_printf(DEBUG_MASK_DMA, "dma running %d\n", dma.chcr.sync_mode);

  if (dma.chcr.sync_mode == SYNC_TRANSFER && dma.chcr.dir == 0 && dma_num == 3)
  {
    uint32_t blocks = dma.bcr & 0xFFFF;
    
    for (int i = 0; i < blocks; i++)
    {
      uint32_t value = *psx_mem.cdrom.disc_ptr++;
      sw(dma.madr | 0x80000000, value, LOC);
      dma.madr += 4;
    }

    dma.chcr.enable = 0;
    dma.chcr.start = 0;
    psx_mem.dma.DMA[dma_num] = dma;

    interrupt_dma(dma_num);
    return;
  }

  if (dma.chcr.sync_mode == SYNC_LINK_LIST && dma_num == 2 && dma.chcr.dir == 1)
  {
    while(!(dma.madr & 0x800000))
    {

      debug_printf(DEBUG_MASK_DMA_TRANSFER, "- address: 0x%.8X\n", dma.madr);

      if (dma.madr == 0) BREAKPOINT;

      uint32_t addr = dma.madr | 0x80000000;

      uint32_t header = lw(addr, LOC);
      uint32_t words = header>>24;
      dma.madr = header&0x00FFFFFF;

      debug_printf(DEBUG_MASK_DMA_TRANSFER, "- %.8X %d words file %s:%u\n", header, words, get_access(addr&0x1FFFFF).file, get_access(addr&0x1FFFFF).line);

      addr += 4;
      for (int i = 0; i < words; i++)
      {
        uint32_t address = addr+i*4;
        uint32_t word = lw(address, LOC);
        debug_printf(DEBUG_MASK_DMA_TRANSFER, "- 0x%.8X -> 0x%.8X file %s:%u\n", address, word, get_access(address&0x1FFFFF).file, get_access(address&0x1FFFFF).line);
        sw(0x1F801810, word, LOC);
      }
    }

    dma.chcr.enable = 0;
    dma.chcr.start = 0;

    psx_mem.dma.DMA[dma_num] = dma;

    interrupt_dma(dma_num);

    return;
  }

  if (dma.chcr.sync_mode == SYNC_BLOCKS && dma.chcr.dir == 1)
  {
    debug_printf(DEBUG_MASK_DMA_TRANSFER, "- address: 0x%.8X\n", dma.madr);


    uint32_t blocks    = (dma.bcr >> 16) & 0xFFFF;
    uint32_t blocksize = (dma.bcr >>  0) & 0xFFFF;

    debug_printf(DEBUG_MASK_DMA_TRANSFER, " - amount of blocks: %d\n", blocks);
    debug_printf(DEBUG_MASK_DMA_TRANSFER, " - block size: %d\n", blocksize);

    uint32_t addr = dma.madr | 0x80000000;

    for (int i = 0; i < blocks; i++)
    {
      for (int j = 0; j < blocksize; j++)
      {
        if (dma_num == 2)
        {
          uint32_t val = lw(addr+(j+i*16)*4, LOC);
          sw(0x1F801810, val, LOC);
        }
        else if (dma_num == 4)
        {
          uint32_t *ptr = (uint32_t *)(psx_mem.spu.mem + psx_mem.spu.current_addr);
          *ptr = lw(addr+(j+i*16)*4, LOC);
          psx_mem.spu.current_addr += 4;
        }
        else
          BREAKPOINT;
      }

      dma.madr += blocksize;
    }

    dma.chcr.enable = 0;
    dma.chcr.start = 0;

    psx_mem.dma.DMA[dma_num] = dma;

    interrupt_dma(dma_num);
    return;
  }

  if (dma.chcr.sync_mode == SYNC_BLOCKS && dma.chcr.dir == 0)
  {
    debug_printf(DEBUG_MASK_DMA_TRANSFER, "- address: 0x%.8X\n", dma.madr);

    uint32_t blocks    = (dma.bcr >> 16) & 0xFFFF;
    uint32_t blocksize = (dma.bcr >>  0) & 0xFFFF;

    debug_printf(DEBUG_MASK_DMA_TRANSFER, " - amount of blocks: %d\n", blocks);
    debug_printf(DEBUG_MASK_DMA_TRANSFER, " - block size: %d\n", blocksize);

    uint32_t addr = dma.madr | 0x80000000;

    for (int i = 0; i < blocks; i++)
    {
      for (int j = 0; j < blocksize; j++)
      {
        if (dma_num == 2)
        {
          uint32_t val = val = lw(0x1F801810, LOC);
          sw(addr+(j+i*16)*4, val, LOC);
        }
        else if (dma_num == 4)
        {
          uint32_t *ptr = (uint32_t *)(psx_mem.spu.mem + psx_mem.spu.current_addr);
          sw(addr+(j+i*16)*4, *ptr, LOC);
          psx_mem.spu.current_addr += 4;
        }
        else
          BREAKPOINT;
      }

      dma.madr += blocksize;
    }

    dma.chcr.enable = 0;
    dma.chcr.start = 0;

    psx_mem.dma.DMA[dma_num] = dma;

    interrupt_dma(dma_num);
    return;
  }

  printf("sync: %d, dir: %d, num: %d\n", dma.chcr.sync_mode, dma.chcr.dir, dma_num);
  BREAKPOINT;
}

void sw_dma(uint32_t addr, uint32_t value, file_loc loc)
{
  debug_printf(DEBUG_MASK_DMA, "file %s:%u: ", loc.file, loc.line);

  if (addr == 0x1F8010F0)
  {
    psx_mem.dma.DPCR.val = value;
    debug_printf(DEBUG_MASK_DMA, "writing 0x%.8X to DPCR\n", value);
    return;
  }

  if (addr == 0x1F8010F4)
  {
    union dicr dicr = {.val = value};

    psx_mem.dma.DICR.unknown = dicr.unknown;
    psx_mem.dma.DICR.irq_enable = dicr.irq_enable;
    psx_mem.dma.DICR.irq_flags &= ~dicr.irq_flags;
    psx_mem.dma.DICR.force_irq = dicr.force_irq;
    psx_mem.dma.DICR.irq_master_enable = dicr.irq_master_enable;

    debug_printf(DEBUG_MASK_DMA, "writing 0x%.8X to DICR\n", value);
    return;
  }

  if (addr >= 0x1F801080 && addr < 0x1F8010F0)
  {
    uint32_t a = addr -0x1F801080;
    uint32_t dma_num = a/16;
    uint32_t reg = a%16;

    debug_printf(DEBUG_MASK_DMA, "writing 0x%.8X to DMA%d %s ", value, dma_num, dmas[dma_num]);

    switch (reg) {
    case 0:
      psx_mem.dma.DMA[dma_num].madr = value&0x00FFFFFF;
      debug_printf(DEBUG_MASK_DMA, "memory address: 0x%.8X\n", value&0x00FFFFFF);
      break;
    case 4:
      psx_mem.dma.DMA[dma_num].bcr = value;
      debug_printf(DEBUG_MASK_DMA, "block control: 0x%.8X\n", value);
      break;
    case 8:
      psx_mem.dma.DMA[dma_num].chcr.val = value;
      debug_printf(DEBUG_MASK_DMA, "channel control: 0x%.8X\n", value);

      if (psx_mem.dma.DMA[dma_num].chcr.enable)
      {
        enable_dma(dma_num);
      }

      break;
    default:
      printf("unknown %d\n", reg);
      BREAKPOINT;
    }
    return;
  }

  printf("unknown address %X\n", addr);
  BREAKPOINT;
}

void sw_timer(uint32_t addr, uint32_t value, file_loc loc)
{
  uint32_t addr2 = addr - 0x1F801100;
  uint32_t timer_num = addr2/0x10;
  addr2 = addr2 % 0x10;

  switch (addr2) {
  case 0:
    debug_printf(DEBUG_MASK_TIMER, "file %s:%u: writing %.8X to timer%d counter\n", loc.file, loc.line, value, timer_num);
    psx_mem.timer.timer[timer_num].counter.val = value;
    psx_mem.timer.timer[timer_num].counter.counter++;
    break;
  case 4:
    debug_printf(DEBUG_MASK_TIMER, "file %s:%u: writing %.8X to timer%d control\n", loc.file, loc.line, value, timer_num);
    psx_mem.timer.timer[timer_num].ctrl.val = value;
    break;
  case 8:
    debug_printf(DEBUG_MASK_TIMER, "file %s:%u: writing %.8X to timer%d target\n", loc.file, loc.line, value, timer_num);
    psx_mem.timer.timer[timer_num].target.val = value;
    break;
  default:
    BREAKPOINT;
    break;
  } 
}

void sh_timer(uint32_t addr, uint32_t value, file_loc loc)
{
  uint32_t addr2 = addr - 0x1F801100;
  uint32_t timer_num = addr2/0x10;
  addr2 = addr2 % 0x10;

  switch (addr2) {
  case 0:
    debug_printf(DEBUG_MASK_TIMER, "file %s:%u: writing %.4hX to timer%d counter\n", loc.file, loc.line, value, timer_num);
    psx_mem.timer.timer[timer_num].counter.val = value;
    psx_mem.timer.timer[timer_num].counter.counter++;
    break;
  case 4:
    debug_printf(DEBUG_MASK_TIMER, "file %s:%u: writing %.4hX to timer%d control\n", loc.file, loc.line, value, timer_num);
    psx_mem.timer.timer[timer_num].ctrl.val = value;
    break;
  case 8:
    debug_printf(DEBUG_MASK_TIMER, "file %s:%u: writing %.4hX to timer%d target\n", loc.file, loc.line, value, timer_num);
    psx_mem.timer.timer[timer_num].target.val = value;
    break;
  default:
    BREAKPOINT;
    break;
  } 
}

void sw_unaligned(uint32_t addr, uint32_t value, file_loc loc)
{
  #ifdef DEBUG_ALL_MEM
    printf("file %s:%u: sw(%.8X, %.8X)\n", loc.file, loc.line, addr, value);
  #endif

  addr = translate_addr(addr);
  
  if (addr == 0x1F801014)
  {
    psx_mem.memctl.spu_delay = value;
    debug_printf(DEBUG_MASK_MEMCTR, "file %s:%u: writing 0x%.8X spu delay: \n", loc.file, loc.line, value);
    return;
  }

  if (addr == 0x1F801070)
  {
    psx_mem.I_STAT.val = psx_mem.I_STAT.val&value;
    debug_printf(DEBUG_MASK_INTERRUPT, "file %s:%u: writing %.8X to I_STAT\n", loc.file, loc.line, value);
    return;
  }

  if (addr == 0x1F801074)
  {
    debug_printf(DEBUG_MASK_INTERRUPT, "file %s:%u: writing %.8X to I_MASK\n", loc.file, loc.line, value);

    psx_mem.I_MASK.val = value;
    return;
  }

  if (addr >= 0x1F801100 && addr < 0x1F801130)
  {
    sw_timer(addr, value, loc);
    return;
  }

  if (addr >= 0x1F801080 && addr < 0x1F801100)
  {
    sw_dma(addr, value, loc);
    return;
  }

  if (addr == 0x1F801810)
  {
    GP0_cmd(value, loc);
    return;
  }

  if (addr == 0x1F801814)
  {
    GP1_cmd(value, loc);
    return;
  }

  if (addr == 0x1F801020)
  {
    debug_printf(DEBUG_MASK_MEMCTR, "file %s:%u: writing %.8X to memory COMMON DELAY\n", loc.file, loc.line, value);
    return;
  }

  if (addr == 0x1F801018)
  {
    debug_printf(DEBUG_MASK_MEMCTR, "file %s:%u: writing %.8X to memory CDROM DELAY\n", loc.file, loc.line, value);
    return;
  }

  check_addr(addr, 4, loc, "sw", value);
  uint32_t *ptr = addr_to_pointer(addr);
  *ptr = value;
}

void sw(uint32_t addr, uint32_t value, file_loc loc)
{
  if(addr & 3) {
    printf("file %s:%u: sw, address %.8X unaligned\n", loc.file, loc.line, addr);
    BREAKPOINT;
  }

  sw_unaligned(addr, value, loc);
}

void spu_fifo(uint16_t value)
{
  (void)value;
}

void sh_spu_voice(uint32_t addr, uint16_t value, file_loc loc)
{
  uint32_t reladdr = addr-0x1F801C00;
  uint32_t voice = reladdr/16;
  uint32_t reg = (reladdr/2)%8;

  psx_mem.spu.voices[voice].regs[reg] = value;

  debug_printf(DEBUG_MASK_SPU, "file %s:%u: setting voice %d register %d: %.4X\n", loc.file, loc.line, voice, reg, value);
}

void sh_spu(uint32_t addr, uint16_t value, file_loc loc)
{
  if (addr >= 0x1F801C00 && addr < 0x1F801D80)
  {
    sh_spu_voice(addr, value, loc);
    return;
  }

  debug_printf(DEBUG_MASK_SPU, "file %s:%u: SPU ", loc.file, loc.line);
  switch (addr) {
  case 0x1F801D80:
    debug_printf(DEBUG_MASK_SPU, "setting left main volume: %.4X\n", value);
    psx_mem.spu.volume.left = value;
    break;
  case 0x1F801D82:
    debug_printf(DEBUG_MASK_SPU, "setting right main volune: %.4X\n", value);
    psx_mem.spu.volume.right = value;
    break;
  case 0x1F801D84:
    debug_printf(DEBUG_MASK_SPU, "setting left reverb volume: %.4X\n", value);
    psx_mem.spu.reverb_volume.left = value;
    break;
  case 0x1F801D86:
    debug_printf(DEBUG_MASK_SPU, "setting right reverb volune: %.4X\n", value);
    psx_mem.spu.reverb_volume.right = value;
    break;
  case 0x1F801D88:
    debug_printf(DEBUG_MASK_SPU, "setting voice key on 0-15: %.4X\n", value);
    psx_mem.spu.voice_key_on = (psx_mem.spu.voice_key_on&0xFFFF0000) | value;
    break;
  case 0x1F801D8A:
    debug_printf(DEBUG_MASK_SPU, "setting voice key on 16-31: %.4X\n", value);
    psx_mem.spu.voice_key_on = (psx_mem.spu.voice_key_on&0x0000FFFF) | (value<<16);
    break;
  case 0x1F801D8C:
    debug_printf(DEBUG_MASK_SPU, "setting voice key off 0-15: %.4X\n", value);
    psx_mem.spu.voice_key_off = (psx_mem.spu.voice_key_off&0xFFFF0000) | value;
    break;
  case 0x1F801D8E:
    debug_printf(DEBUG_MASK_SPU, "setting voice key off 16-31: %.4X\n", value);
    psx_mem.spu.voice_key_off = (psx_mem.spu.voice_key_off&0x0000FFFF) | (value<<16);
    break;
  case 0x1F801D90:
    debug_printf(DEBUG_MASK_SPU, "setting voice channel FM 0-15: %.4X\n", value);
    psx_mem.spu.voice_channel_fm = (psx_mem.spu.voice_channel_fm&0xFFFF0000) | value;
    break;
  case 0x1F801D92:
    debug_printf(DEBUG_MASK_SPU, "setting voice channel FM 16-31: %.4X\n", value);
    psx_mem.spu.voice_channel_fm = (psx_mem.spu.voice_channel_fm&0x0000FFFF) | (value<<16);
    break;
  case 0x1F801D94:
    debug_printf(DEBUG_MASK_SPU, "setting voice channel noise mode 0-15: %.4X\n", value);
    psx_mem.spu.voice_channel_noise_mode = (psx_mem.spu.voice_channel_noise_mode&0xFFFF0000) | value;
    break;
  case 0x1F801D96:
    debug_printf(DEBUG_MASK_SPU, "setting voice channel noise mode 16-31: %.4X\n", value);
    psx_mem.spu.voice_channel_noise_mode = (psx_mem.spu.voice_channel_noise_mode&0x0000FFFF) | (value<<16);
    break;
  case 0x1F801D98:
    debug_printf(DEBUG_MASK_SPU, "setting voice channel reverb mode 0-15: %.4X\n", value);
    psx_mem.spu.voice_channel_reverb_mode = (psx_mem.spu.voice_channel_reverb_mode&0xFFFF0000) | value;
    break;
  case 0x1F801D9A:
    debug_printf(DEBUG_MASK_SPU, "setting voice channel reverb mode 16-31: %.4X\n", value);
    psx_mem.spu.voice_channel_reverb_mode = (psx_mem.spu.voice_channel_reverb_mode&0x0000FFFF) | (value<<16);
    break;
  case 0x1F801DA2:
    debug_printf(DEBUG_MASK_SPU, "setting sound RAM reverb work area start address: %.4X\n", value);
    psx_mem.spu.sound_ram_reverb_start_address = value;
    break;
  case 0x1F801DA6:
    debug_printf(DEBUG_MASK_SPU, "setting sound RAM data transfer address: %.4X\n", value);
    psx_mem.spu.sound_ram_transfer_addr = value;
    psx_mem.spu.current_addr = psx_mem.spu.sound_ram_transfer_addr*8;
    break;
  case 0x1F801DA8:
    debug_printf(DEBUG_MASK_SPU, " RAM data transfer to fifo: %.4X\n", value);
    spu_fifo(value);
    break;
  case 0x1F801DAA:
    debug_printf(DEBUG_MASK_SPU, "setting spu control register: %.4X\n", value);
    psx_mem.spu.spu_control_register = value;
    break;
  case 0x1F801DAC:
    debug_printf(DEBUG_MASK_SPU, "setting sound RAM data transfer control: %.4X\n", value);
    psx_mem.spu.sound_ram_transfer_control = value;
    break;
  case 0x1F801DB0:
    debug_printf(DEBUG_MASK_SPU, "setting left CD volume: %.4X\n", value);
    psx_mem.spu.cd_volume.left = value;
    break;
  case 0x1F801DB2:
    debug_printf(DEBUG_MASK_SPU, "setting right CD volune: %.4X\n", value);
    psx_mem.spu.cd_volume.right = value;
    break;
  case 0x1F801DB4:
    debug_printf(DEBUG_MASK_SPU, "setting left extern volume: %.4X\n", value);
    psx_mem.spu.extern_volume.left = value;
    break;
  case 0x1F801DB6:
    debug_printf(DEBUG_MASK_SPU, "setting right extern volune: %.4X\n", value);
    psx_mem.spu.extern_volume.right = value;
    break;
  default:
    printf("writing %.4X to unknown spu address %.8X\n", value, addr);
    BREAKPOINT;
    break;
  }
}

uint32_t command_success = 0;

void sh_unaligned(uint32_t addr, uint16_t value, file_loc loc)
{

  #ifdef DEBUG_ALL_MEM
    printf("file %s:%u: sh(%.8X, %4.4X)\n", loc.file, loc.line, addr, value);
  #endif

  addr = translate_addr(addr);

  if (addr >= 0x1F801100 && addr < 0x1F801130)
  {
    sh_timer(addr, value, loc);
    return;
  }

  if (addr >= 0x1F801C00 && addr < 0x1F801DC0)
  {
    sh_spu(addr, value, loc);
    return;
  }

  if (addr == 0x1F801048)
  {
    psx_mem.controller.joy_mode.val = value;
    debug_printf(DEBUG_MASK_CONTROLLER, "file %s:%u: writing %.8X to JOY_MODE\n", loc.file, loc.line, value);
    return;
  }

  if (addr == 0x1F80104A)
  {
    psx_mem.controller.joy_ctrl.val = value;

    if (!(value & 2))
    {
      joy_state[0] = STATE_NONE;
      joy_state[1] = STATE_NONE;
    }

    debug_printf(DEBUG_MASK_CONTROLLER, "file %s:%u: writing %.8X to JOY_CTRL\n", loc.file, loc.line, value);
    return;
  }

  if (addr == 0x1F80104E)
  {
    psx_mem.controller.joy_baud = value;
    if (command_success)
    {
      psx_mem.I_STAT.bits.CONTROLLER_AND_MEM_CARD = 1;
      command_success = 0;
    }
    debug_printf(DEBUG_MASK_CONTROLLER, "file %s:%u: writing %.8X to JOY_BAUD\n", loc.file, loc.line, value);
    return;
  }

  if (addr == 0x1F801070)
  {
    psx_mem.I_STAT.val = psx_mem.I_STAT.val&value;
    debug_printf(DEBUG_MASK_INTERRUPT, "file %s:%u: writing %.8X to I_STAT\n", loc.file, loc.line, value);
    return;
  }

  if (addr == 0x1F801074)
  {
    debug_printf(DEBUG_MASK_INTERRUPT, "file %s:%u: writing %.8X to I_MASK\n", loc.file, loc.line, value);
    psx_mem.I_MASK.val = value;
    return;
  }

  check_addr(addr, 2, loc, "sh", value);
  uint16_t *ptr = addr_to_pointer(addr);
  *ptr = value;
}

void sh(uint32_t addr, uint16_t value, file_loc loc)
{
  if(addr & 1) {
    printf("file %s:%u: sh, address %.8X unaligned\n", loc.file, loc.line, addr);
    BREAKPOINT;
  }

  sh_unaligned(addr, value, loc);
}

uint8_t cdrom_pull_response(void)
{
  assert2(psx_mem.cdrom.response_fifo.length > 0);
  psx_mem.cdrom.response_fifo.length--;
  uint8_t value = psx_mem.cdrom.response_fifo.parameters[0];

  for (int i = 0; i < 15; i++)
    psx_mem.cdrom.response_fifo.parameters[i] = psx_mem.cdrom.response_fifo.parameters[i+1];
  psx_mem.cdrom.response_fifo.parameters[15] = 0;

  if (psx_mem.cdrom.response_fifo.length == 0)
    psx_mem.cdrom.status.RSLRRDY = 0;

  return value;
}

void sb_cdrom(uint32_t addr, uint8_t value, file_loc loc)
{
  if (addr == 0x1F801800) {
    debug_printf(DEBUG_MASK_CDROM, "file %s:%u: writing 0x%.2X to CDROM status register\n", loc.file, loc.line, value);
    psx_mem.cdrom.status.index = value&3;
    return;
  }

  uint8_t index = psx_mem.cdrom.status.index;

  if (addr == 0x1F801803 && index == 0)
  {
    debug_printf(DEBUG_MASK_CDROM, "file %s:%u: writing 0x%.2X to READONLY CDROM interrupt enable register\n", loc.file, loc.line, value);
    return;
  }

  if (addr == 0x1F801802 && index == 0)
  {
    debug_printf(DEBUG_MASK_CDROM, "file %s:%u: writing 0x%.2X to CDROM parameter FIFO\n", loc.file, loc.line, value);
    assert2(psx_mem.cdrom.parameter_fifo.length < 16);
    psx_mem.cdrom.parameter_fifo.parameters[psx_mem.cdrom.parameter_fifo.length] = value;
    psx_mem.cdrom.parameter_fifo.length++;
    return;
  }

  if (addr == 0x1F801802 && index == 1)
  {
    debug_printf(DEBUG_MASK_CDROM, "file %s:%u: writing 0x%.2X to CDROM interrupt enable register\n", loc.file, loc.line, value);
    psx_mem.cdrom.interrupt_enable = value;
    return;
  }

  if (addr == 0x1F801802 && index == 2)
  {
    debug_printf(DEBUG_MASK_CDROM, "file %s:%u: writing 0x%.2X to CDROM Audio Volume for Left-CD-Out to Left-SPU-Input\n", loc.file, loc.line, value);
    psx_mem.cdrom.volume_Left_CD_Left_SPU = value;
    return;
  }

  if (addr == 0x1F801803 && index == 2)
  {
    debug_printf(DEBUG_MASK_CDROM, "file %s:%u: writing 0x%.2X to CDROM Audio Volume for Left-CD-Out to Right-SPU-Input\n", loc.file, loc.line, value);
    psx_mem.cdrom.volume_Left_CD_Right_SPU = value;
    return;
  }

  if (addr == 0x1F801801 && index == 3)
  {
    debug_printf(DEBUG_MASK_CDROM, "file %s:%u: writing 0x%.2X to CDROM Audio Volume for Right-CD-Out to Right-SPU-Input\n", loc.file, loc.line, value);
    psx_mem.cdrom.volume_Right_CD_Right_SPU = value;
    return;
  }

  if (addr == 0x1F801802 && index == 3)
  {
    debug_printf(DEBUG_MASK_CDROM, "file %s:%u: writing 0x%.2X to CDROM Audio Volume for Right-CD-Out to Left-SPU-Input\n", loc.file, loc.line, value);
    psx_mem.cdrom.volume_Right_CD_Left_SPU = value;
    return;
  }

  if (addr == 0x1F801803 && index == 3)
  {
    assert2(value == 0x20);


    debug_printf(DEBUG_MASK_CDROM, "file %s:%u: writing 0x%.2X to CDROM apply changes\n", loc.file, loc.line, value);
    return;
  }

  if (addr == 0x1F801803 && index == 1)
  {

    debug_printf(DEBUG_MASK_CDROM, "file %s:%u: writing 0x%.2X to CDROM interrupt flag register %.8X\n", loc.file, loc.line, value, addr);
    if ((value & 7) == 7)
    {
      psx_mem.cdrom.interrupt_flag.response_received = 0;
    }

    if (value & ~7) BREAKPOINT;
    return;
  }


  if (addr == 0x1F801801 && index == 0)
  {
    debug_printf(DEBUG_MASK_CDROM, "file %s:%u: writing 0x%.2X to CDROM command register\n", loc.file, loc.line, value);
    cdrom_cmd(value, loc);
    psx_mem.cdrom.cmd = value;
    return;
  }

  printf("file %s:%u: sb: writing 0x%.2X to unknown CDROM address %.8X index: %d\n", loc.file, loc.line, value, addr, index);
  BREAKPOINT;
}

struct {
  uint8_t data[8];
  uint32_t size;
} joy_fifo;

void push_joy_fifo(uint8_t value)
{
  if (joy_fifo.size >= 8) BREAKPOINT;

  for (int i = 0; i < 7; i++)
    joy_fifo.data[i+1] = joy_fifo.data[i];

  joy_fifo.data[0] = value;

  joy_fifo.size++;
}

uint8_t pop_joy_fifo()
{
  if (joy_fifo.size == 0) return 0x00;

  joy_fifo.size--;

  return joy_fifo.data[joy_fifo.size];
}

uint32_t joy_entering_config = 0;

void sb_joy_tx(uint8_t value, file_loc loc)
{
  if (psx_mem.controller.joy_ctrl.desired_slot == 1) {
    push_joy_fifo(0xFF);
    command_success = 0;
    return;
  }

  uint16_t input;
  // first byte selects 0x01 controller or 0x81 memory card
  switch (joy_state[psx_mem.controller.joy_ctrl.desired_slot]) {
  case STATE_NONE:
    if (value != 1) BREAKPOINT;
    push_joy_fifo(0xFF);
    command_success = 1;
    joy_state[psx_mem.controller.joy_ctrl.desired_slot] = STATE_JOY_1;
    break;
  case STATE_JOY_1:
    push_joy_fifo(0x41);
    if (value == 0x42) {
      command_success = 1;
      joy_state[psx_mem.controller.joy_ctrl.desired_slot] = STATE_JOY_2;
    } else if (value == 0x43) {
      command_success = 0;
      joy_state[psx_mem.controller.joy_ctrl.desired_slot] = STATE_NONE;
    } else {
      printf("unknown joy command %.2X\n", value);
      BREAKPOINT;
    }
    break;
  case STATE_JOY_2:
    if (value != 0) BREAKPOINT;
    push_joy_fifo(0x5A);
    command_success = 1;
    joy_state[psx_mem.controller.joy_ctrl.desired_slot] = STATE_JOY_3;
    break;
  case STATE_JOY_3:
    if (value != 0) BREAKPOINT;
    input = get_input();
    command_success = 1;
    push_joy_fifo((input >> 0) & 0xFF);
    joy_state[psx_mem.controller.joy_ctrl.desired_slot] = STATE_JOY_4;
    break;
  case STATE_JOY_4:
    if (value != 0) BREAKPOINT;
    input = get_input();
    command_success = 1;
    push_joy_fifo((input >> 8) & 0xFF);
    joy_state[psx_mem.controller.joy_ctrl.desired_slot] = STATE_NONE;
    break;
  default:
    printf("unknown joy state %d\n", joy_state[psx_mem.controller.joy_ctrl.desired_slot]);
    BREAKPOINT;
  }
}

uint8_t lb_joy_rx()
{
  return pop_joy_fifo();
}

void sb(uint32_t addr, uint8_t value, file_loc loc)
{
  #ifdef DEBUG_ALL_MEM
    printf("file %s:%u: sb(%.8X, %2.2X)\n", loc.file, loc.line, addr, value);
  #endif

  addr = translate_addr(addr);

  if (addr == 0x1F801040)
  {
    debug_printf(DEBUG_MASK_CONTROLLER, "file %s:%u: writing %.2X to   TX FIFO %d\n", loc.file, loc.line, value, psx_mem.controller.joy_ctrl.desired_slot);
    sb_joy_tx(value, loc);
    return;
  }

  if (addr >= 0x1F801800 && addr < 0x1F801804) {
    sb_cdrom(addr, value, loc);
    return;
  }

  switch (addr) {
  default:
    check_addr(addr, 1, loc, "sb", value);
    uint8_t *ptr = addr_to_pointer(addr);
    *ptr = value;
  }
}

uint32_t lw_timer(uint32_t addr, file_loc loc)
{
  uint32_t addr2 = addr - 0x1F801100;
  uint32_t timer_num = addr2/0x10;
  addr2 = addr2 % 0x10;

  uint32_t value;

  switch (addr2) {
  case 0: {}
    value = psx_mem.timer.timer[timer_num].counter.val;
    psx_mem.timer.timer[timer_num].counter.counter++;
    debug_printf(DEBUG_MASK_TIMER, "file %s:%u: reading %.8X from timer%d counter\n", loc.file, loc.line, value, timer_num);
    return value;
    break;
  case 4:
    value = psx_mem.timer.timer[timer_num].ctrl.val;
    debug_printf(DEBUG_MASK_TIMER, "file %s:%u: reading %.8X from timer%d control\n", loc.file, loc.line, value, timer_num);
    return value;
    break;
  case 8:
    value = psx_mem.timer.timer[timer_num].target.val;
    debug_printf(DEBUG_MASK_TIMER, "file %s:%u: reading %.8X from timer%d target\n", loc.file, loc.line, value, timer_num);
    return value;
    break;
  default:
    BREAKPOINT;
    break;
  }
  return 0;
}

uint16_t lh_timer(uint32_t addr, file_loc loc)
{
  uint32_t addr2 = addr - 0x1F801100;
  uint32_t timer_num = addr2/0x10;
  addr2 = addr2 % 0x10;

  uint32_t value;

  switch (addr2) {
  case 0: {}
    value = psx_mem.timer.timer[timer_num].counter.val;
    psx_mem.timer.timer[timer_num].counter.counter++;
    debug_printf(DEBUG_MASK_TIMER, "file %s:%u: reading %.8X from timer%d counter\n", loc.file, loc.line, value, timer_num);
    return value;
    break;
  case 4:
    value = psx_mem.timer.timer[timer_num].ctrl.val;
    debug_printf(DEBUG_MASK_TIMER, "file %s:%u: reading %.8X from timer%d control\n", loc.file, loc.line, value, timer_num);
    return value;
    break;
  case 8:
    value = psx_mem.timer.timer[timer_num].target.val;
    debug_printf(DEBUG_MASK_TIMER, "file %s:%u: reading %.8X from timer%d target\n", loc.file, loc.line, value, timer_num);
    return value;
    break;
  default:
    BREAKPOINT;
    break;
  }
  return 0;
}

uint32_t lw_dma(uint32_t addr, file_loc loc)
{
  debug_printf(DEBUG_MASK_DMA, "file %s:%u: ", loc.file, loc.line);

  if (addr == 0x1F8010F0)
  {
    uint32_t value = psx_mem.dma.DPCR.val;
    debug_printf(DEBUG_MASK_DMA, "reading %.8X from DPCR\n", value);
    return value;
  }

  if (addr == 0x1F8010F4)
  {
    union dicr dicr = {.val = psx_mem.dma.DICR.val};
    dicr.irq_master_flag = dicr.force_irq || (dicr.irq_master_enable && ((dicr.irq_enable & dicr.irq_flags) > 0));
    uint32_t value = dicr.val;
    debug_printf(DEBUG_MASK_DMA, "reading 0x%.8X from DICR\n", value);
    return value;
  }

  if (addr >= 0x1F801080 && addr < 0x1F8010F0)
  {
    uint32_t a = addr -0x1F801080;
    uint32_t dma_num = a/16;
    uint32_t reg = a%16;

    debug_printf(DEBUG_MASK_DMA, "reading DMA%d %s ", dma_num, dmas[dma_num]);

    uint32_t value;
    switch (reg) {
    case 0:
      value = psx_mem.dma.DMA[dma_num].madr;
      debug_printf(DEBUG_MASK_DMA, "memory address: %.8X\n", value);
      break;
    case 8:
      value = psx_mem.dma.DMA[dma_num].chcr.val;
      debug_printf(DEBUG_MASK_DMA, "channel control: %.8X\n", value);
      break;
    default:
      printf("unknown %d\n", reg);
      BREAKPOINT;
    }

    return value;
  }
  printf("unknown DMA address %X\n", addr);
  BREAKPOINT;
  return 0;
}

union gpustat psx_gpustat()
{
  return psx_mem.gpu.GPUSTAT;
}

uint32_t lw_unaligned(uint32_t addr, file_loc loc)
{

  #ifdef DEBUG_ALL_MEM
    printf("file %s:%u: lw(%.8X)\n", loc.file, loc.line, addr);
  #endif

  addr = translate_addr(addr);

  uint32_t value;
  if (addr >= 0x1F801100 && addr < 0x1F801130)
  {
    value = lw_timer(addr, loc);
    return value;
  }

  if (addr >= 0x1F801080 && addr < 0x1F801100)
  {
    value = lw_dma(addr, loc);
    return value;
  }

  switch (addr)
  {
  case  0x1F801070:
    value = psx_mem.I_STAT.val;
    debug_printf(DEBUG_MASK_INTERRUPT, "file %s:%u: reading I_STAT: 0x%.8X 0x%.8X\n", loc.file, loc.line, value, psx_mem.I_MASK.val);
    break;
  case  0x1F801014:
    value = psx_mem.memctl.spu_delay;
    debug_printf(DEBUG_MASK_MEMCTR, "file %s:%u: reading spu delay: 0x%.8X\n", loc.file, loc.line, value);
    break;
  case  0x1F801074:
    value = psx_mem.I_MASK.val;
    debug_printf(DEBUG_MASK_INTERRUPT, "file %s:%u: reading I_MASK: 0x%.8X 0x%.8X\n", loc.file, loc.line, value, psx_mem.I_STAT.val);
    break;
  case 0x1F8010F0:
    value = psx_mem.dma.DPCR.val;
    debug_printf(DEBUG_MASK_DMA, "file %s:%u: reading from dma DPCR: %.8X\n", loc.file, loc.line, value);
    break;
  case 0x1F801810:
    debug_printf(DEBUG_MASK_GPU, "file %s:%u: reading GP0/GP1 response: %X\n", loc.file, loc.line, psx_mem.gpu.GPUREAD);
    if (stream_state2 == STREAM_STATE_VRAM_TO_CPU)
      value = GP0_read_stream(param_list, param_list_len);
    else
      value = psx_mem.gpu.GPUREAD;
    break;
  case 0x1F801814:
    debug_printf(DEBUG_MASK_GPU, "file %s:%u: receive GPU status register: %X\n", loc.file, loc.line, psx_mem.gpu.GPUSTAT.val);
    value = psx_gpustat().val;
    break;
  default:
    check_addr(addr, 4, loc, "lw", 0);
    uint32_t *ptr = addr_to_pointer(addr);
    value = *ptr;
  }
  return value;
}

uint32_t lw(uint32_t addr, file_loc loc)
{
  if(addr & 3) {
    printf("file %s:%u: lw, address %.8X unaligned\n", loc.file, loc.line, addr);
    BREAKPOINT;
  }

  return lw_unaligned(addr, loc);
}

uint32_t lh_spu_voice(uint32_t addr, file_loc loc)
{
  uint32_t reladdr = addr-0x1F801C00;
  uint32_t voice = reladdr/16;
  uint32_t reg = (reladdr/2)%8;

  uint32_t value = psx_mem.spu.voices[voice].regs[reg];

  debug_printf(DEBUG_MASK_SPU, "file %s:%u: reading voice %d register %d: %.4X\n", loc.file, loc.line, voice, reg, value);

  return value;
}

uint32_t lh_spu(uint32_t addr, file_loc loc)
{
  if (addr >= 0x1F801C00 && addr < 0x1F801D80)
  {
    uint16_t value = lh_spu_voice(addr, loc);
    return value;
  }

  uint32_t value = 0;
  switch (addr) {
  case  0x1F801DA6:
    value = psx_mem.spu.sound_ram_transfer_addr;
    debug_printf(DEBUG_MASK_SPU, "file %s:%u: reading spu RAM data transfer address: %.4X\n", loc.file, loc.line, value);
    break;
  case 0x1F801DAA:
    value = psx_mem.spu.spu_control_register;
    debug_printf(DEBUG_MASK_SPU, "file %s:%u: reading spu control register: %.4X\n", loc.file, loc.line, value);
    break;
  case  0x1F801DAE:
    debug_printf(DEBUG_MASK_SPU, "file %s:%u: reading spu status register: %.4X\n", loc.file, loc.line, value);
    value = psx_mem.spu.spu_status_register;
    break;
  case  0x1F801DB8:
    debug_printf(DEBUG_MASK_SPU, "file %s:%u: reading spu main volume left: %.4X\n", loc.file, loc.line, value);
    value = psx_mem.spu.current_main_volume.left;
    break;
  case  0x1F801DBA:
    debug_printf(DEBUG_MASK_SPU, "file %s:%u: reading spu main volume right: %.4X\n", loc.file, loc.line, value);
    value = psx_mem.spu.current_main_volume.right;
    break;
  default:
    printf("file %s:%u: reading %.4X from unknown spu address %.8X\n", loc.file, loc.line, value, addr);
    BREAKPOINT;
    break;
  }

  return value;
}

uint8_t lb_cdrom(uint32_t addr, file_loc loc)
{
  if (addr == 0x1F801800) {
    uint8_t value = psx_mem.cdrom.status.val;
    debug_printf(DEBUG_MASK_CDROM, "file %s:%u: reading 0x%.2X from CDROM status register\n", loc.file, loc.line, value);
    return value;
  }

  uint8_t index = psx_mem.cdrom.status.index;

  if (addr == 0x1F801803 && index == 1)
  {
    uint8_t value = psx_mem.cdrom.interrupt_flag.val;
    value |= 0xE0;
    debug_printf(DEBUG_MASK_CDROM, "file %s:%u: reading 0x%.2X from CDROM interrupt flag register %.8X\n", loc.file, loc.line, value, addr);
    return value;
  }

  if (addr == 0x1F801803 && index == 0)
  {
    uint8_t value = psx_mem.cdrom.interrupt_enable;
    value |= 0xE0;
    debug_printf(DEBUG_MASK_CDROM, "file %s:%u: reading 0x%.2X from CDROM interrupt enable register\n", loc.file, loc.line, value);
    return value;
  }

  if (addr == 0x1F801801 && index == 1)
  {
    uint8_t value = cdrom_pull_response();
    debug_printf(DEBUG_MASK_CDROM, "file %s:%u: reading 0x%.2X from CDROM response fifo\n", loc.file, loc.line, value);
    return value;
  }

  printf("file %s:%u: lb: unknown CDROM address %.8X %d\n", loc.file, loc.line, addr, index);
  BREAKPOINT;
  return 0;
}

uint32_t lbu(uint32_t addr, file_loc loc)
{

  #ifdef DEBUG_ALL_MEM
    printf("file %s:%u: lbu(%.8X)\n", loc.file, loc.line, addr);
  #endif

  addr = translate_addr(addr);

  if (addr >= 0x1F801800 && addr < 0x1F801804) {
    return lb_cdrom(addr, loc);
  }

  if (addr == 0x1F801040)
  {
    uint32_t value = lb_joy_rx();
    debug_printf(DEBUG_MASK_CONTROLLER, "file %s:%u: reading %.2X from RX FIFO, fifo size: %d\n", loc.file, loc.line, value, joy_fifo.size);
    return value;
  }

  check_addr(addr, 1, loc, "lb", 0);
  uint8_t *ptr = addr_to_pointer(addr);
  return *ptr;
}

uint32_t lb(uint32_t addr, file_loc loc)
{
  #ifdef DEBUG_ALL_MEM
    printf("file %s:%u: lb(%.8X)\n", loc.file, loc.line, addr);
  #endif

  uint32_t value = lbu(addr, loc);

  if (value&0x80)
    value |= 0xFFFFFF00;

  return value;
}

uint32_t lhu(uint32_t addr, file_loc loc)
{

  #ifdef DEBUG_ALL_MEM
    printf("file %s:%u: lhu(%.8X)\n", loc.file, loc.line, addr);
  #endif


  addr = translate_addr(addr);

  uint32_t value;
  if (addr >= 0x1F801100 && addr < 0x1F801130)
  {
    value = lh_timer(addr, loc);
    return value;
  }

  if (addr == 0x1F801044)
  {
    value = 5;
    if (joy_fifo.size > 0) value |= 0x02;
    debug_printf(DEBUG_MASK_CONTROLLER, "file %s:%u: reading %X from JOY STAT\n", loc.file, loc.line, value);
    return value;
  }

  if (addr == 0x1F80104A)
  {
    value = psx_mem.controller.joy_ctrl.val;
    debug_printf(DEBUG_MASK_CONTROLLER, "file %s:%u: reading %.8X from JOY_CTRL\n", loc.file, loc.line, value);
    return value;
  }

  if (addr >= 0x1F801C00 && addr < 0x1F801DC0)
  {
    value = lh_spu(addr, loc);
    return value;
  }

  switch (addr)
  {
  case  0x1F801070:
    value = psx_mem.I_STAT.val;
    debug_printf(DEBUG_MASK_INTERRUPT, "file %s:%u: reading I_STAT: %.4X\n", loc.file, loc.line, value);
    break;
  case  0x1F801074:
    value = psx_mem.I_MASK.val;
    debug_printf(DEBUG_MASK_INTERRUPT, "file %s:%u: reading I_MASK: %.4X\n", loc.file, loc.line, value);
    break;
  default:
    check_addr(addr, 2, loc, "lh", 0);
    uint16_t *ptr = addr_to_pointer(addr);

    value = *ptr;
  }
  
  return value;
}

uint32_t lh(uint32_t addr, file_loc loc)
{
  #ifdef DEBUG_ALL_MEM
    printf("file %s:%u: lh(%.8X)\n", loc.file, loc.line, addr);
  #endif

  uint32_t value = lhu(addr, loc);

  if (value&0x8000)
    value |= 0xFFFF0000;

  return value;
}

void psx_read_sectors(uint32_t dst, uint32_t sector, uint32_t sector_len)
{
#ifdef LOG_DISK_READ
  printf("reading to %.8X-%.8X sectors %d-%d %d\n", dst, dst+sector_len*2048, sector, sector+sector_len, sector_len);
#endif

  for (int i = 0; i < sector_len; i++) {

    uint32_t s = sector+i;
    uint32_t *ptr = (uint32_t *)(psx_mem.cdrom.disc + s*0x930 + 24);
    for (int j = 0; j < 0x200; j++)
    {
      sw(dst, *ptr, LOC);
      dst += 4;
      ptr++;
    }
  }
}

void start_frame()
{

  set_resolution((int[]){256, 320, 512, 640}[psx_mem.gpu.display_mode.res_horiz_1], (int[]){240, 480}[psx_mem.gpu.display_mode.res_vert]);

  uint32_t x = psx_mem.gpu.display_area_start & 0x2FF;
  uint32_t y = (psx_mem.gpu.display_area_start >> 10) & 0x1FF;
  update_vram(psx_mem.gpu.mem, x, y, psx_mem.gpu.display_mode.color_depth);

  wait_frame();

  handle_input();
}