#pragma once

#include <stdint.h>
#include "debug.h"

struct controller {
  uint16_t joy_baud;

  union {
    uint16_t val;
    struct {
      uint16_t baudrate_reload_factor : 2;
      uint16_t character_length : 2;
      uint16_t parity_enable : 1;
      uint16_t parity_type : 1;
      uint16_t unknown1 : 2;
      uint16_t clk_output_polarity : 1;
      uint16_t unknown2 : 7;
    };
  } joy_mode;

  union {
    uint16_t val;
    struct {
      uint16_t tx_enable : 1;
      uint16_t joy_output : 1;
      uint16_t rx_enable : 1;
      uint16_t unknown1 : 1;
      uint16_t acknowledge : 1;
      uint16_t unknown2 : 1;
      uint16_t reset : 1;
      uint16_t not_used1 : 1;
      uint16_t rx_interrupt_mode : 2;
      uint16_t tx_interrupt_enable : 1;
      uint16_t rx_interrupt_enable : 1;
      uint16_t ack_interrupt_enable : 1;
      uint16_t desired_slot : 1;
      uint16_t not_used2 : 2;
    };
  } joy_ctrl;
};

struct cdrom {

  struct {
    uint8_t file;
    uint8_t channel;
  } filter;

  union {
    uint8_t val;
    struct {
      uint8_t cdda : 1;
      uint8_t auto_pause : 1;
      uint8_t report : 1;
      uint8_t xa_filter : 1;
      uint8_t ignore_bit : 1;
      uint8_t sector_size : 1;
      uint8_t xa_adpcm : 1;
      uint8_t speed : 1;
    };
  } mode;

  union{
    uint8_t val;
    struct {
      uint8_t index   : 2;
      uint8_t ADPBUSY : 1;
      uint8_t PRMEMPT : 1;
      uint8_t PRMWRDY : 1;
      uint8_t RSLRRDY : 1;
      uint8_t DRQSTS  : 1;
      uint8_t BUSYSTS : 1;
    };
  } status;

  union {
    uint8_t val;
    struct {
      uint8_t unused : 5;
      uint8_t SMEN : 1;
      uint8_t BFWR : 1;
      uint8_t BFRD : 1;
    };
  } request;

  union {
    uint8_t val;
    struct {
      uint8_t error : 1;
      uint8_t spindle_motor : 1;
      uint8_t seek_error : 1;
      uint8_t id_error : 1;
      uint8_t shell_open : 1;
      uint8_t read : 1;
      uint8_t seek : 1;
      uint8_t play : 1;
    };
  } stat;

  uint8_t cmd;

  uint8_t interrupt_enable;


  union{
    uint8_t val;
    struct {
      uint8_t response_received : 3;
      uint8_t unknown1 : 1;
      uint8_t command_start : 1;
      uint8_t unknown2 : 3;
    };
  } interrupt_flag;

  struct {
    uint8_t length;
    uint8_t parameters[16];
  } parameter_fifo;

  struct {
    uint8_t length;
    uint8_t parameters[16];
  } response_fifo;

  struct {
    uint8_t length;
    uint8_t parameters[16];
  } data_fifo;

  uint8_t volume_Left_CD_Left_SPU;
  uint8_t volume_Left_CD_Right_SPU;
  uint8_t volume_Right_CD_Right_SPU;
  uint8_t volume_Right_CD_Left_SPU;

  uint32_t sector_num;
  uint32_t disc_byte;
  uint8_t *disc;
  uint32_t *disc_ptr;
};

struct voice {
  uint16_t regs[8];
};

union volume {
  uint32_t main;
  struct {
    uint16_t left;
    uint16_t right;
  };
};

struct spu {

  uint32_t current_addr;

  uint8_t *mem;

  struct voice voices[24];

  union volume volume;
  union volume reverb_volume;
  uint32_t voice_key_on;
  uint32_t voice_key_off;
  uint32_t voice_channel_fm;
  uint32_t voice_channel_noise_mode;
  uint32_t voice_channel_reverb_mode;
  uint32_t voice_channel_status;
  uint16_t unknown1;
  uint16_t sound_ram_reverb_start_address;
  uint16_t sound_ram_irq_addr;
  uint16_t sound_ram_transfer_addr;
  uint16_t sound_ram_transfer_fifo;
  uint16_t spu_control_register;
  uint16_t sound_ram_transfer_control;
  uint16_t spu_status_register;
  union volume cd_volume;
  union volume extern_volume;
  union volume current_main_volume;
  uint32_t unknown2;
};

struct dma {
  uint32_t madr;
  uint32_t bcr;
  union {
    uint32_t val;
    struct {
      uint32_t dir : 1;
      uint32_t step : 1;

      uint32_t unused1 : 6;

      uint32_t chopping : 1;
      uint32_t sync_mode : 2;

      uint32_t unused2 : 5;

      uint32_t chopping_dma_size : 3;
      uint32_t unused3 : 1;

      uint32_t chopping_cpu_size : 3;
      uint32_t unused4 : 1;

      uint32_t enable : 1;
      uint32_t unused5 : 3;

      uint32_t start : 1;
      uint32_t unknown1 : 1;
      uint32_t unknown2 : 1;
      uint32_t unused6 : 1;
    };
  } chcr;
};

union dicr {
  uint32_t val;
  struct {
    uint32_t unknown : 6;
    uint32_t not_used : 6;
    uint32_t force_irq : 1;
    uint32_t irq_enable : 7;
    uint32_t irq_master_enable : 1;
    uint32_t irq_flags : 7;
    uint32_t irq_master_flag : 1;
  };
};

struct dmactl {
  struct dma DMA[7];

  union {
    uint32_t val;
    struct {
      uint32_t DMA0_pri : 3;
      uint32_t DMA0_enb : 1;
      uint32_t DMA1_pri : 3;
      uint32_t DMA1_enb : 1;
      uint32_t DMA2_pri : 3;
      uint32_t DMA2_enb : 1;
      uint32_t DMA3_pri : 3;
      uint32_t DMA3_enb : 1;
      uint32_t DMA4_pri : 3;
      uint32_t DMA4_enb : 1;
      uint32_t DMA5_pri : 3;
      uint32_t DMA5_enb : 1;
      uint32_t DMA6_pri : 3;
      uint32_t DMA6_enb : 1;
      uint32_t DMA7_pri : 3;
      uint32_t DMA7_enb : 1;
    };
  } DPCR; // 0x1F8010F0;
  
  union dicr DICR; // 0x1F8010F4;
};

struct timer {
  union {
    uint32_t val;
    struct {
      uint32_t counter : 16;
      uint32_t garbage : 16;
    };
  } counter;

  union {
    uint32_t val;
    struct {
      uint32_t sync_enable : 1;
      uint32_t sync_mode : 2;
      uint32_t reset_counter : 1;

      uint32_t IRQ_equals_target : 1;
      uint32_t IRQ_before_overflow : 1;
      uint32_t IRQ_repeat : 1;
      uint32_t IRQ_toggle : 1;

      uint32_t clock_source : 2;
      uint32_t int_req : 1;        // (W=1) (R) (0=Yes, 1=No) (Set after Writing)
      uint32_t reached_target : 1; // (R)
      uint32_t reached_end : 1;    // (R)
      
      uint32_t unknown :  3;
      uint32_t garbage : 16;
    };
  } ctrl;

  union {
    uint32_t val;
    struct {
      uint32_t counter : 16;
      uint32_t garbage : 16;
    };
  } target;
};

struct timers {
  struct timer timer[3];
};

struct draw_mode {
  uint32_t tex_x_base : 4;
  uint32_t tex_y_base_1 : 1;
  uint32_t semi_transparency : 2;
  uint32_t texture_bits : 2;
  uint32_t dither_24_to_15 : 1;
  uint32_t draw_display_area : 1;
  uint32_t tex_y_base_2 : 1;
  uint32_t tex_x_flip : 1;
  uint32_t tex_y_flip : 1;
  uint32_t unused : 10;
  uint32_t cmd : 8;
};

typedef union {
  uint32_t val;
  struct {
    uint32_t param : 29;
    uint32_t cmd : 3;
  };
  struct {
    uint32_t x : 16;
    uint32_t y : 16;
  } res;
  struct {
    uint32_t x : 10;
    uint32_t y : 10;
    uint32_t zero : 4;
    uint32_t cmd : 8;
  } area;

  struct {
    uint32_t x : 11;
    uint32_t y : 11;
    uint32_t zero : 2;
    uint32_t cmd : 8;
  } offs;

  struct {
    uint32_t maskx : 5;
    uint32_t masky : 5;
    uint32_t offsx : 5;
    uint32_t offsy : 5;
    uint32_t zero : 4;
    uint32_t cmd : 8;
  } tex_win;

  struct poly {
    uint8_t r : 8;
    uint8_t g : 8;
    uint8_t b : 8;
    uint32_t raw_texture : 1;
    uint32_t transparent : 1;
    uint32_t textured : 1;
    uint32_t quad : 1;
    uint32_t gouraud : 1;
    uint32_t cmd : 3;
  } poly;

  struct rect {
    uint8_t r : 8;
    uint8_t g : 8;
    uint8_t b : 8;
    uint32_t raw_texture : 1;
    uint32_t transparent : 1;
    uint32_t textured : 1;
    uint32_t size : 2;
    uint32_t cmd : 3;
  } rect;

  struct draw_mode draw_mode;

} gp0_cmd;

typedef struct poly poly;

union display_mode {
  uint32_t val;
  struct {
    uint32_t res_horiz_1 : 2;
    uint32_t res_vert : 1;
    uint32_t video_mode : 1;
    uint32_t color_depth : 1;
    uint32_t interlace : 1;
    uint32_t res_horiz_2 : 1;
    uint32_t horizontal_flip : 1;
    uint32_t unused : 24;
  };
};

union gpustat {
  uint32_t val;
  struct {
    uint32_t tex_x : 4;

    uint32_t tex_y : 1;
    uint32_t trans : 2;

    uint32_t tex_col_depth : 2;

    uint32_t dither : 1;
    uint32_t drawing_allowed : 1;
    uint32_t draw_mask_set : 1;

    uint32_t use_draw_mask : 1;
    uint32_t interlace : 1;
    uint32_t reverseflag : 1;
    uint32_t tex_y_2 : 1;

    uint32_t res_x_2 : 1;
    uint32_t res_x_1 : 2;
    uint32_t res_y : 1;

    uint32_t video_mode : 1;
    uint32_t display_col_depth : 1;
    uint32_t vertical_interlace : 1;
    uint32_t display_enable : 1;

    uint32_t irq : 1;
    uint32_t dreq : 1;
    uint32_t cmd_ready : 1;
    uint32_t vram_ready : 1;

    uint32_t dma_ready : 1;
    uint32_t dma_dir : 2;
    uint32_t even_odd_interlace : 1;
  };
};

struct gpu {
  uint8_t *mem;

  uint32_t display_area_start;
  uint32_t display_area_end;

  union display_mode display_mode;

  struct draw_mode draw_mode;

  struct {
    uint32_t x1, y1, x2, y2;
    int32_t x, y;
  } area;

  uint32_t GP0_params;
  uint32_t GP0_param_list[16];
  uint32_t GP0_param_list_len;

  union {
    uint32_t val;
    struct {
      uint32_t param : 24;
      uint32_t cmd : 8;
    };
    struct {
      uint32_t Y1 : 10;
      uint32_t Y2 : 10;
      uint32_t cmd : 8;
    } coord;
    struct {
      uint32_t X1 : 12;
      uint32_t X2 : 12;
      uint32_t cmd : 8;
    } coord2;
  } GP1;
  uint32_t GPUREAD;
  union gpustat GPUSTAT;
};

struct int_bits {
  uint16_t VBLANK : 1;
  uint16_t GPU : 1;
  uint16_t CDROM : 1;
  uint16_t DMA : 1;
  uint16_t TMR0 : 1;
  uint16_t TMR1 : 1;
  uint16_t TMR2 : 1;
  uint16_t CONTROLLER_AND_MEM_CARD : 1;
  uint16_t SIO : 1;
  uint16_t SPU : 1;
  uint16_t CONTROLLER : 1;
  uint16_t not_used : 5;
};

struct memctl {
  uint32_t spu_delay;
};

struct psx_mem {
  uint8_t *mem;
  file_loc *mem_access;
  uint8_t *scratchpad;
  union {
    uint16_t val;
    struct int_bits bits;
  } I_MASK;
  union {
    uint16_t val;
    struct int_bits bits;
  } I_STAT;

  struct controller controller;
  struct dmactl dma;
  struct timers timer;
  struct gpu gpu;
  struct spu spu;

  struct memctl memctl;

  struct cdrom cdrom;
};

union gpustat psx_gpustat(void);

void start_frame(void);
void init_psx_mem(void);

void enable_timer(void);
void disable_timer(void);

void set_lock(void);
void release_lock(void);

void psx_read_sectors(uint32_t dst, uint32_t sector, uint32_t sector_len);

file_loc get_access(uint32_t addr);
void print_access(uint32_t addr);

void sw_unaligned(uint32_t addr, uint32_t value, file_loc loc);
void sw(uint32_t addr, uint32_t value, file_loc loc);
void sh(uint32_t addr, uint16_t value, file_loc loc);
void sb(uint32_t addr, uint8_t  value, file_loc loc);
uint32_t lw_unaligned(uint32_t addr, file_loc loc);
uint32_t lw(uint32_t addr, file_loc loc);
uint32_t lh(uint32_t addr, file_loc loc);
uint32_t lb(uint32_t addr, file_loc loc);

uint32_t lhu(uint32_t addr, file_loc loc);
uint32_t lbu(uint32_t addr, file_loc loc);

void psx_test_render(uint32_t cmd_ptr, uint32_t depth, file_loc loc);

#define LOC (file_loc){__FILE__, __LINE__}

#define psx_test_render(cmd_ptr, depth) psx_test_render(cmd_ptr, depth, LOC)

#define sw_unaligned(addr, value) sw_unaligned(addr, value, LOC)
#define sw(addr, value) sw(addr, value, LOC)
#define sh(addr, value) sh(addr, value, LOC)
#define sb(addr, value) sb(addr, value, LOC)
#define lw_unaligned(addr) lw_unaligned(addr, LOC)
#define lw(addr) lw(addr, LOC)
#define lh(addr) lh(addr, LOC)
#define lb(addr) lb(addr, LOC)

#define lhu(addr) lhu(addr, LOC)
#define lbu(addr) lbu(addr, LOC)

void *addr_to_pointer(uint32_t addr);
uint32_t pointer_to_addr(void *ptr);
uint32_t pointer_to_addr_maybe(void *ptr);

void inter(int type);