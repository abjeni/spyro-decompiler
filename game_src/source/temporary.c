#include <stdint.h>
#include "main.h"
#include "decompilation.h"
#include "psx_mem.h"
#include "int_math.h"
#include "spyro_psy.h"
#include "spyro_vsync.h"

////////////////////
// spyro_system.h //
////////////////////

uint32_t dma_callback(uint32_t dma_num, uint32_t callback)
{
  a0 = dma_num;
  a1 = callback;
  function_8005E804();
  return v0;
}

uint32_t init_hook_entry_int2(void)
{
  function_8005DDC8();
  return v0;
}

////////////////////
// spyro_system.h //
////////////////////

////////////////////////
// spyro_controller.h //
////////////////////////

void init_controller(void)
{
    function_800123C8();
}

void controller_wait_for_data(void)
{
    function_8006A0A4();
}

void stop_registering_gamepads(void)
{
    function_80069060();
}

////////////////////////
// spyro_controller.h //
////////////////////////

//////////////////////
// spyro_graphics.h //
//////////////////////


// size: 0x000530
void initial_loading_screen(void)
{
  function_800127C0();
}

void GP1_command(uint32_t cmd)
{
  a0 = cmd;
  function_800616F4();
}

// size: 0x000030
uint32_t gpu_internal_register(uint32_t reg)
{
  sw(lw(GPU_GP1_cmd_ptr), reg | 0x10000000);
  return lw(lw(GPU_GP0_cmd_ptr)) & 0xFFFFFF;
}

// size: 0x00004C
void execute_gpu_linked_list(void *node)
{
  sw(lw(GPU_GP1_cmd_ptr), 0x04000002); // DMA cpu to gpu
  sw(lw(DMA_GPU_address_ptr), pointer_to_addr(node));
  sw(lw(DMA_GPU_block_control_ptr), 0);
  sw(lw(DMA_GPU_channel_control_ptr), 0x01000401); // linked list dma start
}

uint32_t fill_color(RECT *rect, uint32_t color)
{
  rect->w = clamp_int(rect->w, 0, lh(VRAM_SIZE_X)-1);
  rect->h = clamp_int(rect->h, 0, lh(VRAM_SIZE_Y)-1);

  if (rect->x & 0x3F || rect->w & 0x3F) {
    sw(0x80075978, 0x08000000 | (0x8007599C & 0xFFFFFF));
    sw(0x8007597C, 0xE3000000);
    sw(0x80075980, 0xE4FFFFFF);
    sw(0x80075984, 0xE5000000);
    sw(0x80075988, 0xE6000000);
    sw(0x8007598C, 0xE1000000 | ((color >> 31) << 10) | (lw(lw(GPU_GP1_cmd_ptr)) & 0x7FF));
    sw(0x80075990, 0x60000000 | (color & 0xFFFFFF));
    sw(0x80075994, (rect->y << 16) | rect->x);
    sw(0x80075998, (rect->h << 16) | rect->w);

    sw(0x8007599C, 0x03FFFFFF);
    sw(0x800759A0, 0xE3000000 | gpu_internal_register(3));
    sw(0x800759A4, 0xE4000000 | gpu_internal_register(4));
    sw(0x800759A8, 0xE5000000 | gpu_internal_register(5));
  } else {
    sw(0x80075978, 0x05FFFFFF);
    sw(0x8007597C, 0xE6000000);
    sw(0x80075980, 0xE1000000 | ((color >> 31) << 10) | (lw(lw(GPU_GP1_cmd_ptr)) & 0x7FF));
    sw(0x80075984, 0x02000000 | (color & 0xFFFFFF));
    sw(0x80075988, (rect->y << 16) | rect->x);
    sw(0x8007598C, (rect->h << 16) | rect->w);
  }
  execute_gpu_linked_list(addr_to_pointer(0x80075978));

  return 0;
}

uint32_t gpu_check_timeout(void)
{
  if (VSync(-1) <= lw(gpu_frame_limit)) {
    v1 = lw(gpu_retry_counter);
    sw(gpu_retry_counter, v1 + 1);
    if ((int32_t)v1 <= 0x0F0000) {
      return 0;
    }
  }
  // timeout happened
  printf(
    "GPU timeout:que=%d,stat=0x%.8X,chcr=0x%.8X,madr=0x%.8X,",
    (lw(0x80074B68) - lw(0x80074B6C)) & 0x3F,
    psx_gpustat().val,
    lw(lw(DMA_GPU_channel_control_ptr)), lw(lw(DMA_GPU_address_ptr))
  );
  printf("function_%.8X(0x%.8X, 0x%.8X)\n", lw(0x80074B58), lw(0x80074B5C), lw(0x80074B60));
  // reset gpu and dma
  sw(0x80074B6C, 0);
  sw(0x80074B68, 0);
  uint32_t istat = set_I_MASK(0);
  sw(lw(DMA_GPU_channel_control_ptr), 0x401);
  v1 = lw(DMA_control_register_copy_2_ptr);
  sw(v1, lw(v1) | 0x800);
  sw(lw(GPU_GP1_cmd_ptr), 0x02000000);
  sw(lw(GPU_GP1_cmd_ptr), 0x01000000);
  set_I_MASK(istat);
  return -1;
}

// size: 0x000034
void gpu_start_timeout(void)
{
  sw(gpu_frame_limit, VSync(-1) + 0xF0);
  sw(gpu_retry_counter, 0);
}

// size: 0x00023C
uint32_t ram_to_vram(RECT *rect, uint16_t *data)
{
  gpu_start_timeout();
  rect->w = clamp_int(rect->w, 0, lh(VRAM_SIZE_X));
  rect->h = clamp_int(rect->h, 0, lh(VRAM_SIZE_Y));

  // while not ready to receive new command
  while (psx_gpustat().cmd_ready == 0)
    if (gpu_check_timeout())
      return -1;


  // to avoid using 32-bit pointers, avoid dma and write the vram directly
  uint16_t *ptr = (void*)psx_mem.gpu.mem;

  for (int j = 0; j < rect->h; j++)
  {
    for (int i = 0; i < rect->w; i++)
    {
      int x = i + rect->x;
      int y = j + rect->y;

      ptr[x+y*1024] = *data++;
    }
  }

  return 0;
}

// size: 0x000284
uint32_t vram_to_ram(RECT *rect, uint16_t *data)
{
  gpu_start_timeout();
  rect->w = clamp_int(rect->w, 0, lh(VRAM_SIZE_X));
  rect->h = clamp_int(rect->h, 0, lh(VRAM_SIZE_Y));

  while (psx_gpustat().cmd_ready == 0)
    if (gpu_check_timeout())
      return -1;
  
  // to avoid using 32-bit pointers, avoid dma and read the vram directly
  uint16_t *ptr = (void*)psx_mem.gpu.mem;

  for (int j = 0; j < rect->h; j++)
  {
    for (int i = 0; i < rect->w; i++)
    {
      int x = i + rect->x;
      int y = j + rect->y;

      *data++ = ptr[x+y*1024];
    }
  }
  
  return 0;
}

uint32_t command_queue_append(uint32_t func, uint32_t data, uint32_t data_size, uint32_t data_or_color)
{
  a0 = func;
  a1 = data;
  a2 = data_size;
  a3 = data_or_color;
  function_80061820();
  return v0;
}

uint32_t psx_has_2mb_vram(void)
{
  uint32_t version = lbu(0x80074A64);
  return version == 1 || version == 2;
}

uint32_t spyro_set_drawing_area_top_left_command(int16_t x, int16_t y)
{
  x = clamp_int(x, 0, lh(VRAM_SIZE_X)-1);
  y = clamp_int(y, 0, lh(VRAM_SIZE_Y)-1);
  if (psx_has_2mb_vram()) {
    v1 = (y & 0xFFF) << 12;
    v0 = x & 0xFFF;
  } else {
    v1 = (y & 0x3FF) << 10;
    v0 = x & 0x3FF;
  }
  return 0xE3000000 | v0 | v1;
}

uint32_t spyro_set_drawing_area_bottom_right_command(int16_t x, int16_t y)
{
  x = clamp_int(x, 0, lh(VRAM_SIZE_X)-1);
  y = clamp_int(y, 0, lh(VRAM_SIZE_Y)-1);
  if (psx_has_2mb_vram()) {
    v1 = (y & 0xFFF) << 12;
    v0 = x & 0xFFF;
  } else {
    v1 = (y & 0x3FF) << 10;
    v0 = x & 0x3FF;
  }
  return 0xE4000000 | v1 | v0;
}

uint32_t spyro_set_drawing_offset_command(int16_t x, int16_t y)
{
  if (psx_has_2mb_vram()) {
    v1 = (y & 0xFFF) << 12;
    v0 = x & 0xFFF;
  } else {
    v1 = (y & 0x7FF) << 11;
    v0 = x & 0x7FF;
  }
  return 0xE5000000 | v1 | v0;
}

uint32_t spyro_draw_mode_setting_command(uint8_t dfe, uint8_t dtd, uint16_t tpage)
{
  if (psx_has_2mb_vram()) {
    v0 = tpage & 0x27FF;
    if (dfe)
      v0 = v0 | 0x1000;

    if (dtd)
      v0 = v0 | 0x800;
  } else {
    v0 = tpage & 0x9FF;
    if (dfe)
      v0 = v0 | 0x400;

    if (dtd)
      v0 = v0 | 0x200;
  }
  return 0xE1000000 | v0;
}

uint32_t spyro_set_texture_window_setting_command(RECT *tw)
{
  if (tw == NULL) return 0;
  a1 = tw->x >> 3;
  a2 = ((-tw->w) >> 3) & 0x1F;
  v0 = tw->y >> 3;
  v1 = ((-tw->h) >> 3) & 0x1F;
  return 0xE2000000 | (v0 << 15) | (a1 << 10) | (v1 << 5) | (a2 << 0);
}

// size: 0x000290
void spyro_clear_screen(DR_ENV *dr_env, DRAWENV env)
{
  uint32_t cmd_num = 0;
  dr_env->code[cmd_num++] = spyro_set_drawing_area_top_left_command(env.clip.x, env.clip.y);
  dr_env->code[cmd_num++] = spyro_set_drawing_area_bottom_right_command(env.clip.x + env.clip.w - 1, env.clip.y + env.clip.h - 1);
  dr_env->code[cmd_num++] = spyro_set_drawing_offset_command(env.ofs[0], env.ofs[1]);
  dr_env->code[cmd_num++] = spyro_draw_mode_setting_command(env.dfe, env.dtd, env.tpage);
  dr_env->code[cmd_num++] = spyro_set_texture_window_setting_command(&env.tw);
  dr_env->code[cmd_num++] = 0xE6000000;
  if (env.isbg) {
    if ((env.clip.x & 0x3F) || (env.clip.w & 0x3F)) {
      dr_env->code[cmd_num++] = 0x60000000
        | (env.b0 << 16)
        | (env.g0 <<  8)
        | (env.r0 <<  0);
      dr_env->code[cmd_num++] = (env.clip.x - env.ofs[0]) | ((env.clip.y - env.ofs[1]) << 16);
      dr_env->code[cmd_num++] = env.clip.w | (env.clip.h << 16);
    } else {
      dr_env->code[cmd_num++] = 0x02000000
        | (env.b0 << 16)
        | (env.g0 <<  8)
        | (env.r0 <<  0);
      dr_env->code[cmd_num++] = env.clip.x | (env.clip.y << 16);
      dr_env->code[cmd_num++] = env.clip.w | (env.clip.h << 16);
    }
  }
  dr_env->tag = (dr_env->tag & 0xFFFFFF) | (cmd_num << 24);
}

//////////////////////
// spyro_graphics.h //
//////////////////////