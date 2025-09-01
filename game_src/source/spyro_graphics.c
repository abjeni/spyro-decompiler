#include "debug.h"
#include "main.h"
#include "psx_mem.h"
#include "spyro_psy.h"
#include "int_math.h"
#include "decompilation.h"
#include "gte.h"
#include "cop0.h"
#include "extra_gte.h"
#include "spyro_memory_card.h"
#include "spyro_controller.h"
#include "spyro_vsync.h"
#include "spyro_spu.h"
#include "spyro_cdrom.h"
#include "spyro_system.h"
#include "spyro_string.h"
#include "psx_ops.h"
#include "not_renamed.h"
#include "spyro_math.h"

#include <stdint.h>
#include <string.h>

#define RESX 512
#define RESY 256

typedef struct {
  uint32_t end; // node*
  uint32_t base; // node*
} link_list;

void write_three_bytes(uint32_t *dst, uint32_t three_bytes)
{
  *dst = (*dst & 0xFF000000) | (three_bytes & 0xFFFFFF);
}

void append_to_linked_list(link_list *link_list, uint32_t *first_node, uint32_t *last_node)
{
  //printf("link_list: 0x%.8X\n", pointer_to_addr(link_list));
  //for (int i = 0; i < 8; i++)
  //  print_access(pointer_to_addr(link_list)+i);
  uint32_t *v0 = addr_to_pointer(link_list->end);
  link_list->end = pointer_to_addr(last_node);
  if (v0) {
    write_three_bytes(v0, pointer_to_addr(first_node));
  } else {
    link_list->base = pointer_to_addr(first_node);
  }
}

void append_gpu_command_block(void *node)
{
  append_to_linked_list(addr_to_pointer(lw(linked_list1)), node, node);
}

// size: 0x000038
void function_800168DC(void)
{
  BREAKPOINT;
  append_gpu_command_block(addr_to_pointer(a0));
}

void append_gpu_command_block_depth_slot(void *node, uint32_t depth_slot)
{
  if (depth_slot >= 0x800) BREAKPOINT;
  append_to_linked_list(addr_to_pointer(lw(ordered_linked_list) + depth_slot*8), node, node);
}

// size: 0x00003C
void function_800168A0(void)
{
  BREAKPOINT;
  append_gpu_command_block_depth_slot(addr_to_pointer(a0), a1);
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

// size: 0x0000CC
void function_80060BC8(void)
{
  BREAKPOINT;
  v0 = spyro_set_drawing_area_top_left_command(a0, a1);
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

// size: 0x0000CC
void function_80060C94(void)
{
  BREAKPOINT;
  v0 = spyro_set_drawing_area_bottom_right_command(a0, a1);
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

// size: 0x000044
void function_80060D60(void)
{
  BREAKPOINT;
  v0 = spyro_set_drawing_offset_command(a0, a1);
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

// size: 0x000058
// get gpu info, and calculate a draw mode setting command
void function_80060B70(void)
{
  BREAKPOINT;
  v0 = spyro_draw_mode_setting_command(a0, a1, a2);
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

// size: 0x000084
void function_80060DA4(void)
{
  BREAKPOINT;
  v0 = spyro_set_texture_window_setting_command(addr_to_pointer(a0));
}

// size: 0x000030
uint32_t gpu_internal_register(uint32_t reg)
{
  sw(lw(GPU_GP1_cmd_ptr), reg | 0x10000000);
  return lw(lw(GPU_GP0_cmd_ptr)) & 0xFFFFFF;
}

// size: 0x000030
void function_800617CC(void)
{
  BREAKPOINT;
  v0 = gpu_internal_register(a0);
}

// size: 0x000028
void GP1_command(uint32_t cmd)
{
  sw(lw(GPU_GP1_cmd_ptr), a0);
  sb(0x800759BC + (a0 >> 24), a0);
}

void function_800616F4(void)
{
  BREAKPOINT;
  GP1_command(a0);
}

// size: 0x00004C
void execute_gpu_linked_list(void *node)
{
  sw(lw(GPU_GP1_cmd_ptr), 0x04000002); // DMA cpu to gpu
  sw(lw(DMA_GPU_address_ptr), pointer_to_addr(node));
  sw(lw(DMA_GPU_block_control_ptr), 0);
  sw(lw(DMA_GPU_channel_control_ptr), 0x01000401); // linked list dma start
}

// size: 0x00004C
void function_80061780(void)
{
  BREAKPOINT;
  execute_gpu_linked_list(addr_to_pointer(a0));
}

// size: 0x0002EC
uint32_t command_queue_advance(void)
{
  if (lw(lw(DMA_GPU_channel_control_ptr)) & 0x01000000)
    return 1;

  a0 = 0;
  sw(saved_I_STAT3, set_I_MASK(0));
  while (lw(0x80074B6C) != lw(0x80074B68) && (lw(lw(DMA_GPU_channel_control_ptr)) & 0x01000000) == 0) {
    if (((lw(0x80074B6C)+1) & 0x3F) == lw(0x80074B68) && lw(0x80074A70))
      dma_callback2(2, 0);

    BREAKPOINT;

    while (psx_gpustat().cmd_ready == 0) BREAKPOINT;

    a0 = lw(0x80078EA4 + lw(0x80074B6C)*96);
    a1 = lw(0x80078EA8 + lw(0x80074B6C)*96);
    v0 = lw(0x80078EA0 + lw(0x80074B6C)*96);
    switch (v0)
    {
    default:
      BREAKPOINT;
      //JALR(v0, 0x80061C54);
    }
    sw(0x80074B58, lw(0x80078EA0 + lw(0x80074B6C)*96));
    sw(0x80074B5C, lw(0x80078EA4 + lw(0x80074B6C)*96));
    sw(0x80074B60, lw(0x80078EA8 + lw(0x80074B6C)*96));
    sw(0x80074B6C, (lw(0x80074B6C)+1) & 0x3F);
  }

  set_I_MASK(lw(saved_I_STAT3));

  if (lw(0x80074B68) == lw(0x80074B6C)
   && (lw(lw(DMA_GPU_channel_control_ptr)) & 0x01000000) == 0
   && lw(0x80074A6C))
  {
    v0 = lw(0x80074A70);
    if (v0) {
      sw(0x80074A6C, 0);
      switch (v0)
      {
      default:
        BREAKPOINT;
        //JALR(v0, 0x80061DB4);
      }
    }
  }
  
  return (lw(0x80074B68) - lw(0x80074B6C)) & 0x3F;
}

// size: 0x0002EC
void function_80061B00(void)
{
  BREAKPOINT;
  v0 = command_queue_advance();
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

void function_800608E0(void)
{
  BREAKPOINT;
  spyro_clear_screen((DR_ENV*)addr_to_pointer(a0), *(DRAWENV*)addr_to_pointer(a1));
}

// size: 0x000034
void gpu_start_timeout(void)
{
  sw(gpu_frame_limit, VSync(-1) + 0xF0);
  sw(gpu_retry_counter, 0);
}

// size: 0x000034
void function_80062090(void)
{
  BREAKPOINT;
  gpu_start_timeout();
}

// size: 0x00016C
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
  sw(saved_I_STAT2, set_I_MASK(0));
  sw(lw(DMA_GPU_channel_control_ptr), 0x401);
  v1 = lw(DMA_control_register_copy_2_ptr);
  sw(v1, lw(v1) | 0x800);
  v1 = lw(GPU_GP1_cmd_ptr);
  sw(v1, 0x02000000);
  sw(v1, 0x01000000);
  set_I_MASK(lw(saved_I_STAT2));
  return -1;
}

// size: 0x00016C
void function_800620C4(void)
{
  BREAKPOINT;
  v0 = gpu_check_timeout();
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

  /*
  uint32_t pixels = rect->w*rect->h + 1; // why add 1 ?
  uint32_t num_words = pixels/2;
  uint32_t num_blocks = pixels/32;
  uint32_t remaining_words = num_words - num_blocks*16;
  
  sw(lw(GPU_GP1_cmd_ptr), 0x04000000);
  sw(lw(GPU_GP0_cmd_ptr), 0x01000000);
  sw(lw(GPU_GP0_cmd_ptr), 0xA0000000);
  sw(lw(GPU_GP0_cmd_ptr), (rect->y << 16) | rect->x);
  sw(lw(GPU_GP0_cmd_ptr), (rect->h << 16) | rect->w);
  
  for (int i = 0; i < remaining_words; i++)
    sw(lw(GPU_GP0_cmd_ptr), *data++);

  if (num_blocks) {
    sw(lw(GPU_GP1_cmd_ptr), 0x04000002);
    sw(lw(DMA_GPU_address_ptr), pointer_to_addr(data));
    sw(lw(DMA_GPU_block_control_ptr), (num_blocks << 16) | 0x10);
    sw(lw(DMA_GPU_channel_control_ptr), 0x01000201);
  }
  */

  return 0;
}

// size: 0x00023C
void function_80061234(void)
{
  BREAKPOINT;
  v0 = ram_to_vram(addr_to_pointer(a0), addr_to_pointer(a1));
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

  /*
  uint32_t pixels = rect->w*rect->h + 1; // why add 1 ?
  uint32_t num_words = pixels/2;
  uint32_t num_blocks = pixels/32;
  uint32_t remaining_words = num_words - num_blocks*16;

  sw(lw(GPU_GP1_cmd_ptr), 0x04000000);
  sw(lw(GPU_GP0_cmd_ptr), 0x01000000);
  sw(lw(GPU_GP0_cmd_ptr), 0xC0000000);
  sw(lw(GPU_GP0_cmd_ptr), (rect->y << 16) | rect->x);
  sw(lw(GPU_GP0_cmd_ptr), (rect->h << 16) | rect->w);

  while (psx_gpustat().vram_ready == 0)
    if (gpu_check_timeout())
      return -1;

  for (int i = 0; i < remaining_words; i++)
    *data++ = lw(lw(GPU_GP0_cmd_ptr));

  if (num_blocks) {
    sw(lw(GPU_GP1_cmd_ptr), 0x04000003);
    sw(lw(DMA_GPU_address_ptr), pointer_to_addr(data));
    sw(lw(DMA_GPU_block_control_ptr), (num_blocks << 16) | 0x10);
    sw(lw(DMA_GPU_channel_control_ptr), 0x01000200);
  }
  */
  return 0;
}

// size: 0x000284
void function_80061470(void)
{
  BREAKPOINT;
  v0 = vram_to_ram(addr_to_pointer(a0), addr_to_pointer(a1));
}

// size: 0x00025C
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

// size: 0x00025C
void function_80060FD8(void)
{
  BREAKPOINT;
  v0 = fill_color(addr_to_pointer(a0), a1);
}

// size: 0x0002E0
uint32_t command_queue_append(uint32_t func, uint32_t data, uint32_t data_size, uint32_t data_or_color)
{
  gpu_start_timeout();

  // wait if command queue is full
  while (((lw(0x80074B68)+1) & 0x3F) == lw(0x80074B6C)) {
    if (gpu_check_timeout())
      return -1;
    command_queue_advance();
  }
  
  a0 = 0;
  sw(saved_I_STAT, set_I_MASK(a0));
  sw(0x80074A6C, 1);
  if (lbu(0x80074A65)
    && (lw(0x80074B68) != lw(0x80074B6C)
    || lw(lw(DMA_GPU_channel_control_ptr)) & 0x01000000
    || lw(0x80074A70)))
  {
    dma_callback2(2, 0x80061B00);
    
    if (data_size) {
      for (int32_t i = 0; i < ((int32_t)data_size)/4; i++)
        sw(0x80078EAC + lw(0x80074B68)*96 + i*4, lw(data + i*4));

      sw(0x80078EA4 + lw(0x80074B68)*96, 0x80078EAC + lw(0x80074B68)*96);
    } else {
      sw(0x80078EA4 + lw(0x80074B68)*96, data);
    }
    sw(0x80078EA8 + lw(0x80074B68)*96, data_or_color);
    sw(0x80078EA0 + lw(0x80074B68)*96, func);
    sw(0x80074B68, (lw(0x80074B68)+1) & 0x3F);
    set_I_MASK(lw(saved_I_STAT));
    command_queue_advance();
    return (lw(0x80074B68) - lw(0x80074B6C)) & 0x3F;
  }
  
  if (psx_gpustat().cmd_ready == 0);
  switch (func)
  {
  case 0x80060FD8:
    v0 = fill_color(addr_to_pointer(data), data_or_color);
    break;
  case 0x80061780:
    execute_gpu_linked_list(addr_to_pointer(data));
    break;
  case 0x80061470:
    vram_to_ram(addr_to_pointer(data), addr_to_pointer(data_or_color));
    break;
  case 0x80061234:
    ram_to_vram(addr_to_pointer(data), addr_to_pointer(data_or_color));
    break;
  default:
    BREAKPOINT;
  }
  sw(0x80074B58, func);
  sw(0x80074B5C, data);
  sw(0x80074B60, data_or_color);
  set_I_MASK(lw(saved_I_STAT));
  return 0;
}

void function_80061820(void)
{
  BREAKPOINT;
  v0 = command_queue_append(a0, a1, a2, a3);
}

// size: 0x000458
void draw_skybox(int32_t index, uint32_t matrix1, uint32_t matrix2)
{
  at = 0x80077DD8;
  sw(at + 0x04, s1);
  sw(at + 0x08, s2);
  sw(at + 0x0C, s3);
  sw(at + 0x10, s4);
  sw(at + 0x14, s5);
  sw(at + 0x18, s6);
  load_RTM(matrix1);
  set_TR(0, 0, 0);

  at = SKYBOX_DATA;
  t3 = lw(at + 0x04);
  uint8_t *indices = NULL;
  uint32_t len = 0;
  if ((int32_t)index >= 0) {
    t4 = lw(lw(at + 0x0C) + index*4);
    indices = addr_to_pointer(t4);
  } else {
    t4 = 0;
    len = lw(at + 0x00);
  }

  uint32_t visible_points[0x100];
  int num_visible_points = 0;

  while (1) {
    if ((int32_t)index >= 0) {
      if (pointer_to_addr_maybe(indices) == 0xDEADBEEF) break;
      at = *indices++;
      if (at == 0xFF) break;
      t6 = lw(t3 + at*4);
    } else {
      if (t4++ >= len) break;
      t6 = lw(t3);
      t3 += 4;
    }

    set_V0_vec3(xy_z_to_vec(lw(t6 + 0x00), lh(t6 + 0x06)));
    RTPS();
    if ((int16_t)cop2.MAC3 > (int16_t)lh(t6 + 0x04)) {
      if (num_visible_points >= sizeof(visible_points)/sizeof(visible_points[0])) BREAKPOINT;
      visible_points[num_visible_points++] = t6;
    }
  }

  load_RTM(matrix2);

  uint32_t *last_node = addr_to_pointer(lw(allocator1_ptr));
  t8 = lw(allocator1_end) - 0x400;
  uint32_t *node = last_node + 4;
  uint32_t *first_node = node;
  t9 = 0x80;
  for (int j = 0; j < num_visible_points; j++) {
    s3 = visible_points[j];

    vec3 offset = xy_z_to_vec(lw(s3 + 0x08), lh(s3 + 0x0E));
    s1 = s3 + 0x18;

    uint32_t *vertices_world = addr_to_pointer(s1);

    uint32_t vertices_screen[0x100] = {};

    uint8_t combined_bound_mask = 0xF;

    uint32_t size = lhu(s3 + 0x0C);

    if (size != lhu(s3 + 0x0C)) BREAKPOINT;

    for (int i = 0; i < size; i++)
    {
      vec3 p = unpack_uint32_vec3(vertices_world[i]);
      p = (vec3){
        .x = offset.x - p.y,
        .y = offset.y - p.z,
        .z = offset.z + p.x
      };
      set_V0_vec3(p);

      RTPS();

      vec3 pixel_coord = get_S2();

      int16_t x = pixel_coord.x;
      int16_t y = pixel_coord.y;
      uint8_t bounds_mask = 0;
      if (y <     0) bounds_mask += 1;
      if (y >= RESY) bounds_mask += 2;
      if (x <     0) bounds_mask += 4;
      if (x >= RESX) bounds_mask += 8;

      combined_bound_mask &= bounds_mask;
      vertices_screen[i] = (((y << 16) + x) << 5) + bounds_mask;
    }

    s4 = s1 + size*4;

    uint32_t *color_palette = addr_to_pointer(s4);

    if (combined_bound_mask == 0) {
      t4 = lw(s3 + 0x10);
      s5 = s4 + (t4 >> 14);
      uint32_t size = t4 & 0x1FFF;
      for (int i = 0; i < size; i++) {
        t4 = lw(s5 + 0);
        t5 = lw(s5 + 4);
        s5 += 8;

        if (pointer_to_addr(node) >= t8) {
          sw(0x800758B0, 1);
          goto end_skybox;
        }

        int32_t vert1 = vertices_screen[(t4 >> 22) & 0xFF];
        int32_t vert2 = vertices_screen[(t4 >> 12) & 0xFF];
        int32_t vert3 = vertices_screen[(t4 >>  2) & 0xFF];
        if (vert1 & vert2 & vert3 & 0x1F) continue;
        vert1 >>= 5;
        vert2 >>= 5;
        vert3 >>= 5;

        last_node[0] = (t9 << 24) | (pointer_to_addr(node) & 0xFFFFFF);
        last_node = node;
        uint32_t col1 = (t5 >> 22) & 0xFF;
        uint32_t col2 = (t5 >> 12) & 0xFF;
        uint32_t col3 = (t5 >>  2) & 0xFF;
        if (col1 == col2 && col1 == col3) {
          t9 = 4;
          node[0] = 0x04000000;
          node[1] = color_palette[col1] - 0x010000000; // color
          node[2] = vert1; // vertex1
          node[3] = vert2; // vertex2
          node[4] = vert3; // vertex3
          node += 5;
        } else {
          t9 = 6;
          node[0] = 0x06000000;
          node[1] = color_palette[col1]; // color1
          node[2] = vert1; // vertex1
          node[3] = color_palette[col2]; // color2
          node[4] = vert2; // vertex2
          node[5] = color_palette[col3]; // color3
          node[6] = vert3; // vertex3
          node += 7;
        }
      }
    }
  }
end_skybox:
  sw(allocator1_ptr, pointer_to_addr(node));
  if (node != first_node) {
    sw(pointer_to_addr(last_node), t9 << 24);
    append_to_linked_list(addr_to_pointer(lw(ordered_linked_list) + 0x3FF8), first_node, last_node);
  }

  at = 0x80077DD8;
  s7 = lw(at + 0x1C);
  s6 = lw(at + 0x18);
  s5 = lw(at + 0x14);
  s4 = lw(at + 0x10);
  s3 = lw(at + 0x0C);
  s2 = lw(at + 0x08);
  s1 = lw(at + 0x04);
  return;
}

void function_8004EBA8(void)
{
  BREAKPOINT;
  draw_skybox(a0, a1, a2);
}

// size: 0x0000B0
void spyro_image_unpack(uint32_t *src, uint32_t *dst, uint32_t fade)
{
  while (1) {
    uint32_t header = *src++;
    if ((int32_t)header < 0) return;
    dst += header & 0x1FFFF;
    for (int i = 0; i < header >> 17; i++) {
      uint32_t colorin = *src++;
      uint32_t colorout = (max_int(((colorin >> 24) & 0xFF) + fade, 0) << 24)
                        + (max_int(((colorin >> 16) & 0xFF) + fade, 0) << 16)
                        + (max_int(((colorin >>  8) & 0xFF) + fade, 0) <<  8)
                        + (max_int(((colorin >>  0) & 0xFF) + fade, 0) <<  0);
      *dst++ = colorout;
    }
  }
}


void function_80017F24(void)
{
  BREAKPOINT;
  spyro_image_unpack(addr_to_pointer(a0), addr_to_pointer(a1), a2);
}

// size: 0x0000B8
DRAWENV *SetDefDrawEnv(DRAWENV *env, int32_t x, int32_t y, int32_t w, int32_t h)
{
  
  function_8005EBA0(); // v0 = lw(0x80074A10);

  if (v0)
    v0 = (int32_t)h < 289;
  else
    v0 = (int32_t)h < 257;

  *env = (DRAWENV){
    .clip = (RECT){x, y, w, h},
    .ofs = {x, y},
    .tpage = 10,
    .dtd = 1,
    .dfe = v0
  };

  return env;
}

// size: 0x0000B8
void function_8005EA94(void)
{
  v0 = pointer_to_addr(SetDefDrawEnv(addr_to_pointer(a0), a1, a2, a3, lw(sp + 0x10)));
}

// size: 0x00003C
DISPENV *SetDefDispEnv(DISPENV *disp, int x, int y, int w, int h)
{
  *disp = (DISPENV){
    .disp = {x, y, w, y}
  };
  return disp;
}

void function_8005EB4C(void)
{
  v0 = pointer_to_addr(SetDefDispEnv(addr_to_pointer(a0), a1, a2, a3, lw(sp + 0x10)));
}

// size: 0x000120
void init_framebuffers(void)
{
  uint32_t framebuffers = 0x80076EE0;

  // fb1 drawenv: 0x80076EE0 - 0x80076F3C
  // fb1 dispenv: 0x80076F3C - 0x80076F50
  // fb2 drawenv: 0x80076F64 - 0x80076FC0
  // fb2 dispenv: 0x80076FC0 - 0x80076FD4

  SetDispMask(0);
  ResetGraph(0);
  SetGraphDebug(0);

  DRAWENV *fb1_drawenv = addr_to_pointer(framebuffers);
  DRAWENV *fb2_drawenv = addr_to_pointer(framebuffers + 0x84);
  SetDefDrawEnv(fb1_drawenv, 0, 0x08, 0x200, 0xE0);
  SetDefDrawEnv(fb2_drawenv, 0, 0xF8, 0x200, 0xE0);

  DISPENV *fb1_dispenv = addr_to_pointer(framebuffers + 0x5C);
  DISPENV *fb2_dispenv = addr_to_pointer(framebuffers + 0xE0);

  SetDefDispEnv(fb1_dispenv, 0, 0xF0, 0x200, 0xF0);
  SetDefDispEnv(fb2_dispenv, 0, 0x00, 0x200, 0xF0);

  fb2_drawenv->ofs[1] = 0xF0;
  fb1_drawenv->ofs[1] = 0x00;

  fb1_drawenv->dtd = 1;
  fb2_drawenv->dtd = 1;
  fb1_drawenv->isbg = 1;
  fb2_drawenv->isbg = 1;

  RECT rect = {0, 0, 0x200, 0x200};
  ClearImage(&rect, 0, 0, 0);
  DrawSync(0);
  SetDispMask(1);
}

// size: 0x000098
void function_8001256C(void)
{
  const static uint32_t array1[] = { // 0x8006F200
    0x00EA60,
    0x014A68,
    0x01BB28,
    0x023EF0,
    0x02D1E8,
    0x037450,
  };

  const static uint32_t array2[] = { // 0x8006EEDC
    0x0042F8,
    0x0049BB,
    0x004AF4,
    0x005533,
    0x005710,
    0x0058BC,
    0x005881,
    0x005B0F,
    0x006073,
    0x0065EF,
    0x0066C6,
    0x006B88,
    0x006C88,
    0x006D99,
    0x00702D,
    0x007049,
    0x007003,
    0x0071D9,
    0x00736D,
    0x007778,
    0x007D0E,
    0x007D82,
    0x007FD5,
    0x008399,
    0x0085AB,
    0x0088DA,
    0x008A74,
    0x008BFB,
    0x008B95,
    0x008F26,
    0x0091F8,
    0x009249,
    0x009416,
    0x0095A6,
    0x00A21A,
    0x00A227,
    0x009049,
    0x007CD8,
    0x006FCB,
    0x008188,
    0x007754,
    0x00577B,
    0x008FE4,
    0x007CD2,
    0x00700F,
    0x00818B,
    0x00768A,
    0x00583F,
  };

  for (int i = 0; i < 6; i++)
  {
    for (int j = 0; j < 8; j++) {
      sw(0x800774B8 + i*64 + j*8, array1[i]);
      sw(0x800774BC + i*64 + j*8, array1[i] + array2[i*8+j]);
    }
  }
  sw(0x800774B4, 0x40);
  v0 = 1;
  return;
}

// size: 0x000080
void InitGeom(void)
{
  spyro_patch_bios();
  cop0.SR = cop0.SR | 0x40000000;
  cop2.ZSF3 = 0x155;
  cop2.ZSF4 = 0x100;
  cop2.H = 0x3E8;
  cop2.DQA = -0x1062;
  cop2.DQB = 0x01400000;
  cop2.OFX = 0;
  cop2.OFY = 0;
}

// size: 0x000080
void function_80062350(void)
{
  BREAKPOINT;
  InitGeom();
}

// size: 0x000020
void mat3x4setTR(uint32_t mat3x4)
{
  load_TR(mat3x4 + 0x14);
}

// size: 0x000020
void function_800625F8(void)
{
  BREAKPOINT;
  mat3x4setTR(a0);
}

// size: 0x000018
void SetGeomOffset(uint32_t ofx, uint32_t ofy) 
{
  cop2.OFX = ofx << 16;
  cop2.OFY = ofy << 16;
}

void function_80062618(void)
{
  BREAKPOINT;
  SetGeomOffset(a0, a1);
}

// size: 0x00000C
void SetGeomScreen(uint32_t h)
{
  cop2.H = h;
}

void function_80062638(void)
{
  BREAKPOINT;
  SetGeomScreen(a0);
}

// size: 0x000060
void init_wad(void)
{
  sw(WAD_sector, 0x25);
  read_disk1(lw(WAD_sector), lw(exe_end_ptr), 0x800, 0, 0x258);
  spyro_memcpy32(WAD_header, lw(exe_end_ptr), 0x330);
}

// size: 0x000060
void function_8001250C(void)
{
  BREAKPOINT;
  init_wad();
}

// size: 0x00011C
uint32_t *spyro_combine_all_command_buffers(uint32_t depth_command_buffers)
{
  link_list *ordered_base = addr_to_pointer(lw(ordered_linked_list));
  link_list *ordered = ordered_base + depth_command_buffers;

  do {
    if (ordered == ordered_base) {
      // command buffer empty
      link_list *list = addr_to_pointer(lw(linked_list1));
      link_list list2 = *list;
      *list = (link_list){0};
      if (list2.end) {
        write_three_bytes(addr_to_pointer(list2.end), 0x8006BBE0);
        return addr_to_pointer(list2.base);
      }
      return addr_to_pointer(0x8006BBE0);
    }
    ordered--;
  } while (ordered->end == 0);

  link_list list = *ordered;
  *ordered = (link_list){0};
  while (ordered != ordered_base) {
    ordered--;
    if (ordered->base) {
      write_three_bytes(addr_to_pointer(list.end), ordered->base);
      list.end = ordered->end;
      *ordered = (link_list){0};
    }
  }

  link_list *list2 = addr_to_pointer(lw(linked_list1));
  if (list2->base) {
    write_three_bytes(addr_to_pointer(list.end), list2->base);
    list.end = list2->end;
    *list2 = (link_list){0};
  }
  write_three_bytes(addr_to_pointer(list.end), 0x8006BBE0);
  return addr_to_pointer(list.base);
}

void function_80016784(void)
{
  BREAKPOINT;
  v0 = pointer_to_addr(spyro_combine_all_command_buffers(a0));
}

// size: 0x000530
void initial_loading_screen(void)
{
  sp -= 72;
  sw(sp + 0x40, ra);
  sw(sp + 0x3C, s5);
  sw(sp + 0x34, s3);
  sw(sp + 0x30, s2);
  sw(sp + 0x2C, s1);
  sw(sp + 0x28, s0);

  RECT logo_rect = {0, 0, 0x300, 0xF0};

  init_framebuffers();
  init_memory_card();
  init_controller();
  init_spu();
  init_cdrom();
  function_8001256C();
  InitGeom();
  SetGeomOffset(0x100, 120);
  SetGeomScreen(341);
  sw(0x800785D8, 0x8007DDE8);
  sw(0x800785DC, 0x8007DDE8);

  int frame_num = VSync(-1);

  SetDispMask(0);
  sb(0x80076F3C + 0x11, 1); // fb1 dispenv isrgb24
  sb(0x80076FC0 + 0x11, 1); // fb2 dispenv isrgb24


  uint32_t *imagergb = addr_to_pointer(lw(0x800785D8));
  memset(imagergb, 0, 0x5A000);

  for (int i = 0; i < 8; i++)
  {
    spyro_image_unpack(addr_to_pointer(0x8006FCF4), imagergb, -0xE0 + i*0x20);
    LoadImage(&logo_rect, imagergb);
    DrawSync(0);
    VSync(0);
    PutDispEnv(addr_to_pointer(0x80076FC0));
    if (i == 0) SetDispMask(1);
  }
  
  init_wad();

  read_disk1(lw(WAD_sector), lw(exe_end_ptr), 0x800, lw(WAD_header + 0x18), 0x258);
  memcpy(addr_to_pointer(WAD_nested_header), addr_to_pointer(lw(exe_end_ptr)), 0x1D0);
  s5 = 0x801C0000 - lw(0x800755A4); // 0x800
  read_disk1(lw(WAD_sector), s5, 0x40000, lw(WAD_header + 0x18) + lw(WAD_nested_header), 0x258);
  read_disk1(lw(WAD_sector), lw(exe_end_ptr), lw(WAD_header + 0x14), lw(WAD_header + 0x10), 0x258);
  s3 = s5 - lw(WAD_header + 0x04);
  read_disk1(lw(WAD_sector), s3, lw(WAD_header + 0x04), lw(WAD_header + 0x00), 0x258);

  while (VSync(-1) - frame_num < 210) VSync(0);

  memset(imagergb, 0, 0x5A000);
  for (int i = 0; i < 8; i++)
  {
    spyro_image_unpack(addr_to_pointer(0x8006FCF4), imagergb, (-i-1) * 0x20);
    LoadImage(&logo_rect, imagergb);
    DrawSync(0);
    VSync(0);
    PutDispEnv(addr_to_pointer(0x80076FC0));
  }

  frame_num = VSync(-1);
  
  uint32_t *universalrgb = addr_to_pointer(s3 - 0x5A000);
  memset(universalrgb, 0, 0x5A000);

  for (int i = 0; i < 8; i++)
  {
    spyro_image_unpack(addr_to_pointer(s3), universalrgb, -0xE0 + i * 0x20);
    LoadImage(&logo_rect, universalrgb);
    DrawSync(0);
    VSync(0);
    PutDispEnv(addr_to_pointer(0x80076FC0));
  }
  
  sw(0x80075864, 3);
  sw(0x8007566C, 0);

  do {
    function_80014564();
  } while ((int32_t)lw(0x80075864) < 10);

  while (VSync(-1) - frame_num < 210) VSync(0);
  
  for (int i = 0; i < 8; i++)
  {
    spyro_image_unpack(addr_to_pointer(s3), universalrgb, (-i-1) * 0x20);
    LoadImage(&logo_rect, universalrgb);
    DrawSync(0);
    VSync(0);
    PutDispEnv(addr_to_pointer(0x80076FC0));
  }
  
  SetDispMask(0);
  sb(0x80076F3C + 0x11, 0); // fb1 dispenv isrgb24
  sb(0x80076FC0 + 0x11, 0); // fb2 dispenv isrgb24
  PutDispEnv(addr_to_pointer(0x80076FC0));
  
  RECT clear_rect = {0, 0, 0x200, 0x1E0};
  ClearImage(&clear_rect, 0, 0, 0);
  DrawSync(0);
  SetDispMask(1);

  RECT rect2 = {0x200, 0, 0x200, 0x100};
  LoadImage(&rect2, addr_to_pointer(s5));
  DrawSync(0);
  sw(0x800785FC, 0x80200000 - lw(0x800755A4));
  function_8005B7D8();
  function_8002D338();
  sw(0x80075918, 15);
  function_8002D170();
  sw(0x800756CC, 2);
  sw(0x80075760, 0);
  sw(0x800785CC, 0x8000);
  ra = lw(sp + 0x40);
  s5 = lw(sp + 0x3C);
  s3 = lw(sp + 0x34);
  s2 = lw(sp + 0x30);
  s1 = lw(sp + 0x2C);
  s0 = lw(sp + 0x28);
  sp += 72;
}

// size: 0x000530
void function_800127C0(void)
{
  BREAKPOINT;
  initial_loading_screen();
}

// size: 0x000001F0
void function_80033C50(void)
{
  set_TR(0, 0, 0);

  mat3 m = mat3_mul(mat3_mul(
    mat3rotX(lh(0x80076E1E)),
    mat3rotY(lh(0x80076E20))
  ),mat3rotZ(lh(0x80076E1C)));

  save_mat3(0x80076DE4, m);

  m.m[1][0] = m.m[1][0]*320/512;
  m.m[1][1] = m.m[1][1]*320/512;
  m.m[1][2] = m.m[1][2]*320/512;

  save_mat3(0x80076DD0, m);
}


// size: 0x004E0C
void function_800258F0(void)
{
  uint32_t temp;
  at = 0x80077DD8;
  sw(at + 0x00, s0);
  sw(at + 0x04, s1);
  sw(at + 0x08, s2);
  sw(at + 0x0C, s3);
  sw(at + 0x10, s4);
  sw(at + 0x14, s5);
  sw(at + 0x18, s6);
  sw(at + 0x1C, s7);
  sw(at + 0x20, gp);
  sw(at + 0x24, sp);
  sw(at + 0x28, fp);
  sw(at + 0x2C, ra);
  ra = a0;
  at = 0x80076DD0;
  load_RTM(0x80076DD0 + 0x14);
  set_TR(0, 0, 0);
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  s7 = (int32_t)lw(at + 0x28) >> 4;
  t8 = (int32_t)lw(at + 0x2C) >> 4;
  t9 = (int32_t)lw(at + 0x30) >> 4;
  s4 = 0x800771C8;
  memset(addr_to_pointer(0x800771C8), 0, 0x100);
  at = 0x800785A8;
  s6 = lw(at + 0x24);
  v0 = 0x8006FCF4;
  t7 = v0 + 0x1C00;
  s0 = v0 + 0x2000;
  sp = -1; // 0xFFFFFFFF
  fp = 255; // 0xFF
  s6 = s6 >> 4;
  temp = (int32_t)ra < 0;
  s2 = lw(at + 0x00);
  if (temp) goto label800259EC;
  v0 = lw(at + 0x08);
  v1 = ra << 2;
  v0 += v1;
  s3 = lw(v0);
  goto label800259FC;
label800259EC:
  at = lw(at + 0x04);
  s5 = s4 - 1; // 0xFFFFFFFF
  at = at << 2;
  s3 = s2 + at;
label800259FC:
  temp = (int32_t)ra >= 0;
  s5++;
  if (temp) goto label80025A14;
  temp = s2 == s3;
  s1 = lw(s2);
  if (temp) goto label800261A0;
  s2 += 4; // 0x04
  goto label80025A30;
label80025A14:
  at = lbu(s3 + 0x00);
  s3++;
  temp = at == fp;
  v0 = at << 2;
  if (temp) goto label800261A0;
  v0 += s2;
  s1 = lw(v0 + 0x00);
  s5 = s4 + at;
label80025A30:
  v0 = lw(s1 + 0x00);
  a0 = lw(s1 + 0x04);
  a1 = s1 | 0x1;
  at = v0 >> 16;
  v0 = v0 & 0xFFFF;
  at -= s7;
  v0 = t8 - v0;
  cop2.IR3 = at;
  cop2.IR1 = v0;
  v1 = a0 >> 16;
  v1 = t9 - v1;
  cop2.IR2 = v1;
  t1 = a0 & 0xE000;
  t2 = a0 & 0x1FFF;
  MVMVA(SF_ON, MX_RT, V_IR, CV_NONE, LM_OFF);
  t3 = t2 >> 1;
  t4 = t2 >> 1;
  t5 = t2 >> 2;
  t3 += t5;
  t5 = t2 >> 4;
  t4 += t5;
  t5 = t2 >> 5;
  t3 += t5;
  t0 = cop2.IR3;
  a2 = cop2.IR1;
  at = t0 + t2;
  temp = (int32_t)at <= 0;
  t4 += t5;
  if (temp) goto label800259FC;
  temp = (int32_t)a2 >= 0;
  v0 = t0 + t4;
  if (temp) goto label80025AAC;
  a2 = -a2;
label80025AAC:
  at = a2 - t3;
  at = at << 2;
  v1 = v0 << 1;
  v0 += v1;
  at -= v0;
  temp = (int32_t)at >= 0;
  t5 = t2 >> 3;
  if (temp) goto label800259FC;
  t5 = t2 - t5;
  t6 = t2 >> 1;
  at = t2 >> 4;
  a3 = cop2.IR2;
  t6 -= at;
  temp = (int32_t)a3 >= 0;
  v0 = t0 + t6;
  if (temp) goto label80025AE8;
  a3 = -a3;
label80025AE8:
  at = a3 - t5;
  at = at << 5;
  v1 = v0 << 4;
  v0 += v1;
  at -= v0;
  temp = (int32_t)at >= 0;
  at = t0 - t2;
  if (temp) goto label800259FC;
  temp = (int32_t)at <= 0;
  gp = lw(s1 + 0x18);
  if (temp) goto label80025B4C;
  at = a2 + t3;
  v0 = t0 - t4;
  at = at << 2;
  v1 = v0 << 1;
  v0 += v1;
  at -= v0;
  temp = (int32_t)at >= 0;
  at = a3 + t5;
  if (temp) goto label80025B4C;
  v0 = t0 - t6;
  at = at << 5;
  v1 = v0 << 4;
  v0 += v1;
  at -= v0;
  temp = (int32_t)at >= 0;
  if (temp) goto label80025B4C;
  a1 = a1 ^ 0x1;
label80025B4C:
  v0 = sp;
  sb(s5 + 0x00, fp);
  at = t1 & 0x2000;
  temp = (int32_t)at > 0;
  at = t1 & 0x8000;
  if (temp) goto label80025B80;
  temp = (int32_t)at > 0;
  at = t0 + t2;
  if (temp) goto label80025B74;
  at += 256; // 0x0100
  at = s6 - at;
  temp = (int32_t)at >= 0;
  if (temp) {
    sw(s0 + 0x00, a1);
    goto label80025B80;
  }
label80025B74:
  sw(s0 + 0x00, a1);
  s0 += 4; // 0x04
  v0 = v0 << 16;
label80025B80:
  at = t1 & 0x4000;
  temp = (int32_t)at > 0;
  at = t0 - t2;
  if (temp) goto label80025BAC;
  v1 = s6 - at;
  temp = (int32_t)v1 <= 0;
  at -= 256; // 0xFFFFFF00
  if (temp) goto label80025BAC;
  temp = (int32_t)at >= 0;
  v0 = v0 & 0xFFFF;
  if (temp) goto label80025BA4;
  a1 += 2; // 0x02
label80025BA4:
  sw(t7 + 0x00, a1);
  t7 += 4; // 0x04
label80025BAC:
  gp = gp | v0;
  temp = gp == sp;
  if (temp) goto label800259FC;
  at = gp << 24;
  temp = (int32_t)at < 0;
  at = at >> 24;
  if (temp) goto label80025D14;
  sb(s1 + 0x18, sp);
  v0 = 0x80078560;
  v0 = lw(v0 + 0x14);
  at = at << 2;
  at += v0;
  at = lw(at + 0x00);
  v1 = lbu(at + 0x02);
  v0 = at + 12; // 0x0C
  v1 = v1 << 3;
  v0 += v1;
  a1 = lbu(v0 + 0x04);
  v1 = lbu(v0 + 0x05);
  a2 = lhu(at + 0x06);
  temp = (int32_t)a1 > 0;
  mult(v1, a2);
  if (temp) goto label80025C3C;
  v1 = lw(at + 0x08);
  a0=lo;
  at += a0;
  v1 += at;
  v0 = v1 + a2;
  at = s1 + 28; // 0x1C
label80025C20:
  a0 = lw(v1 + 0x00);
  v1 += 4; // 0x04
  at += 4; // 0x04
  temp = v1 != v0;
  sw(at - 0x04, a0); // 0xFFFFFFFC
  if (temp) goto label80025C20;
  goto label80025D14;
label80025C3C:
  a1 = a1 << 4;
  cop2.IR0 = a1;
  v1 = lw(at + 0x08);
  v0 = lbu(v0 + 0x06);
  at += v1;
  a0=lo;
  v1 = at + a0;
  mult(v0, a2);
  v0 = v1 + a2;
  v0 += 4; // 0x04
  a0=lo;
  a0 += at;
  at = s1 + 28; // 0x1C
  a2 = lw(v1 + 0x00);
  v1 += 4; // 0x04
  a3 = lw(a0 + 0x00);
  a0 += 4; // 0x04
  t0 = a2 >> 21;
  t1 = a2 >> 10;
  t1 = t1 & 0x7FF;
  t2 = a2 & 0x3FF;
  cop2.IR1 = t0;
  cop2.IR2 = t1;
  cop2.IR3 = t2;
label80025CA0:
  t0 = a3 >> 21;
  t1 = a3 >> 10;
  t1 = t1 & 0x7FF;
  t2 = a3 & 0x3FF;
  cop2.RFC = t0;
  cop2.GFC = t1;
  cop2.BFC = t2;
  a2 = lw(v1 + 0x00);
  a3 = lw(a0 + 0x00);
  INTPL();
  at += 4; // 0x04
  v1 += 4; // 0x04
  a0 += 4; // 0x04
  t0 = a2 >> 21;
  t1 = a2 >> 10;
  t1 = t1 & 0x7FF;
  t2 = a2 & 0x3FF;
  cop2.IR1 = t0;
  cop2.IR2 = t1;
  cop2.IR3 = t2;
  t0 = cop2.MAC1;
  t1 = cop2.MAC2;
  t2 = cop2.MAC3;
  t0 = t0 << 21;
  t1 = t1 << 10;
  t0 += t1;
  t0 += t2;
  temp = v0 != v1;
  sw(at - 0x04, t0); // 0xFFFFFFFC
  if (temp) goto label80025CA0;
label80025D14:
  at = gp << 16;
  temp = (int32_t)at < 0;
  at = at >> 24;
  if (temp) goto label80025E5C;
  sb(s1 + 0x19, sp);
  v0 = 0x80078560;
  v0 = lw(v0 + 0x1C);
  at = at << 2;
  at += v0;
  at = lw(at + 0x00);
  v1 = lbu(at + 0x02);
  v0 = at + 12; // 0x0C
  v1 = v1 << 3;
  v0 += v1;
  a1 = lbu(v0 + 0x04);
  v1 = lbu(v0 + 0x05);
  a2 = lhu(at + 0x06);
  temp = (int32_t)a1 > 0;
  mult(v1, a2);
  if (temp) goto label80025DB8;
  v1 = lw(at + 0x08);
  a0=lo;
  at += a0;
  v1 += at;
  at = lbu(s1 + 0x10);
  v0 = v1 + a2;
  at = at << 2;
  at += 28; // 0x1C
  at += s1;
  a2 = 0xFFFFFF;
label80025D90:
  a0 = lw(v1 + 0x00);
  v1 += 4; // 0x04
  a1 = a0 >> 22;
  a1 = a1 & 0x3FC;
  at += a1;
  a0 = a0 & a2;
  temp = v1 != v0;
  sw(at + 0x00, a0);
  if (temp) goto label80025D90;
  goto label80025E5C;
label80025DB8:
  v1 = lw(at + 0x08);
  v0 = lbu(v0 + 0x06);
  a0=lo;
  at += v1;
  v1 = at + a0;
  mult(v0, a2);
  v0 = v1 + a2;
  a1 = a1 << 4;
  cop2.IR0 = a1;
  a0=lo;
  a0 += at;
  at = lbu(s1 + 0x10);
  a1 = 0xFFFFFF;
  at = at << 2;
  at += 28; // 0x1C
  at += s1;
  a2 = lw(v1 + 0x00);
  a3 = lw(a0 + 0x00);
label80025E04:
  t0 = a2 & a1;
  cop2.RGBC = t0;
  t0 = a3 << 4;
  t0 = t0 & 0xFF0;
  cop2.RFC = t0;
  t0 = a3 >> 4;
  t0 = t0 & 0xFF0;
  cop2.GFC = t0;
  t0 = a3 >> 12;
  t0 = t0 & 0xFF0;
  cop2.BFC = t0;
  t0 = a2 >> 22;
  t0 = t0 & 0x3FC;
  DPCS();
  a2 = lw(v1 + 0x04);
  a3 = lw(a0 + 0x04);
  v1 += 4; // 0x04
  a0 += 4; // 0x04
  at += t0;
  t1 = cop2.RGB2;
  temp = v0 != v1;
  sw(at + 0x00, t1);
  if (temp) goto label80025E04;
label80025E5C:
  at = gp << 8;
  temp = (int32_t)at < 0;
  at = at >> 24;
  if (temp) goto label80025FD0;
  sb(s1 + 0x1A, sp);
  v0 = 0x80078560;
  v0 = lw(v0 + 0x24);
  at = at << 2;
  at += v0;
  at = lw(at + 0x00);
  v1 = lbu(at + 0x02);
  v0 = at + 12; // 0x0C
  v1 = v1 << 3;
  v0 += v1;
  a1 = lbu(v0 + 0x04);
  v1 = lbu(v0 + 0x05);
  a2 = lhu(at + 0x06);
  temp = (int32_t)a1 > 0;
  mult(v1, a2);
  if (temp) goto label80025EEC;
  v1 = lw(at + 0x08);
  a0=lo;
  at += a0;
  v1 += at;
  v0 = v1 + a2;
  a2 = lbu(s1 + 0x17);
  at = s1 + 28; // 0x1C
  a2 = a2 << 2;
  at += a2;
label80025ED0:
  a0 = lw(v1 + 0x00);
  v1 += 4; // 0x04
  at += 4; // 0x04
  temp = v1 != v0;
  sw(at - 0x04, a0); // 0xFFFFFFFC
  if (temp) goto label80025ED0;
  goto label80025FD0;
label80025EEC:
  a1 = a1 << 4;
  cop2.IR0 = a1;
  v1 = lw(at + 0x08);
  v0 = lbu(v0 + 0x06);
  at += v1;
  a0=lo;
  v1 = at + a0;
  mult(v0, a2);
  v0 = v1 + a2;
  v0 += 4; // 0x04
  a2 = lbu(s1 + 0x17);
  a0=lo;
  a0 += at;
  at = s1 + 28; // 0x1C
  a2 = a2 << 2;
  at += a2;
  a2 = lw(v1 + 0x00);
  v1 += 4; // 0x04
  a3 = lw(a0 + 0x00);
  a0 += 4; // 0x04
  t0 = a2 >> 21;
  t1 = a2 >> 10;
  t1 = t1 & 0x7FF;
  t2 = a2 & 0x3FF;
  cop2.IR1 = t0;
  cop2.IR2 = t1;
  cop2.IR3 = t2;
label80025F5C:
  t0 = a3 >> 21;
  t1 = a3 >> 10;
  t1 = t1 & 0x7FF;
  t2 = a3 & 0x3FF;
  cop2.RFC = t0;
  cop2.GFC = t1;
  cop2.BFC = t2;
  a2 = lw(v1 + 0x00);
  a3 = lw(a0 + 0x00);
  INTPL();
  at += 4; // 0x04
  v1 += 4; // 0x04
  a0 += 4; // 0x04
  t0 = a2 >> 21;
  t1 = a2 >> 10;
  t1 = t1 & 0x7FF;
  t2 = a2 & 0x3FF;
  cop2.IR1 = t0;
  cop2.IR2 = t1;
  cop2.IR3 = t2;
  t0 = cop2.MAC1;
  t1 = cop2.MAC2;
  t2 = cop2.MAC3;
  t0 = t0 << 21;
  t1 = t1 << 10;
  t0 += t1;
  t0 += t2;
  temp = v0 != v1;
  sw(at - 0x04, t0); // 0xFFFFFFFC
  if (temp) goto label80025F5C;
label80025FD0:
  temp = (int32_t)gp < 0;
  at = gp >> 24;
  if (temp) goto label800259FC;
  sb(s1 + 0x1B, sp);
  v0 = 0x80078560;
  v0 = lw(v0 + 0x2C);
  at = at << 2;
  at += v0;
  at = lw(at + 0x00);
  v1 = lbu(at + 0x02);
  v0 = at + 12; // 0x0C
  v1 = v1 << 3;
  v0 += v1;
  a1 = lbu(v0 + 0x04);
  v1 = lbu(v0 + 0x05);
  a2 = lhu(at + 0x06);
  temp = (int32_t)a1 > 0;
  mult(v1, a2);
  if (temp) goto label80026098;
  v1 = lw(at + 0x08);
  a0=lo;
  at += a0;
  v1 += at;
  a0 = v1 + a2;
  a1 = lw(s1 + 0x14);
  at = s1 + 28; // 0x1C
  a2 = a1 >> 22;
  a2 = a2 & 0x3FC;
  at += a2;
  a2 = a1 << 2;
  a2 = a2 & 0x3FC;
  at += a2;
  a2 = a1 >> 6;
  a2 = a2 & 0x3FC;
  v0 = at + a2;
  a1 = 0xFFFFFF;
label80026064:
  a2 = lw(v1 + 0x00);
  a3 = lw(v1 + 0x04);
  v1 += 8; // 0x08
  t1 = a2 >> 22;
  t1 = t1 & 0x3FC;
  at += t1;
  v0 += t1;
  a2 = a2 & a1;
  sw(at + 0x00, a2);
  temp = v1 != a0;
  sw(v0 + 0x00, a3);
  if (temp) goto label80026064;
  goto label800259FC;
label80026098:
  v1 = lw(at + 0x08);
  v0 = lbu(v0 + 0x06);
  a0=lo;
  at += v1;
  v1 = at + a0;
  mult(v0, a2);
  a1 = a1 << 4;
  cop2.IR0 = a1;
  a1 = v1 + a2;
  a2 = lw(s1 + 0x14);
  a0=lo;
  a0 += at;
  at = s1 + 28; // 0x1C
  a3 = a2 >> 22;
  t0 = lw(v1 + 0x00);
  a3 = a3 & 0x3FC;
  at += a3;
  a3 = a2 << 2;
  a3 = a3 & 0x3FC;
  at += a3;
  t1 = lw(a0 + 0x00);
  a3 = a2 >> 6;
  a3 = a3 & 0x3FC;
  v0 = at + a3;
  a2 = 0xFFFFFF;
label80026100:
  t2 = t0 & a2;
  cop2.RGBC = t2;
  t2 = t1 << 4;
  t2 = t2 & 0xFF0;
  cop2.RFC = t2;
  t2 = t1 >> 4;
  t2 = t2 & 0xFF0;
  cop2.GFC = t2;
  t2 = t1 >> 12;
  t2 = t2 & 0xFF0;
  cop2.BFC = t2;
  a3 = t0 >> 22;
  a3 = a3 & 0x3FC;
  DPCS();
  t2 = cop2.RGB2;
  t0 = lw(v1 + 0x04);
  t1 = lw(a0 + 0x04);
  at += a3;
  sw(at, t2);
  cop2.RGBC = t0;
  cop2.RFC = ((t1 >>  0) & 0xFF) << 4;
  cop2.GFC = ((t1 >>  8) & 0xFF) << 4;
  cop2.BFC = ((t1 >> 16) & 0xFF) << 4;
  DPCS();
  t2 = cop2.RGB2;

  v1 += 8; // 0x08
  a0 += 8; // 0x08
  t0 = lw(v1);
  t1 = lw(a0);
  v0 += a3;
  temp = v1 != a1;
  sw(v0, t2);
  if (temp) goto label80026100;
  goto label800259FC;
label800261A0:
  sw(s0, 0);
  sw(t7, 0);
  load_RTM(0x80076DD0);
  ra = 0x8006FCF4 + 0x2000;
  fp = lw(allocator1_ptr);
  if (lw(0x8007591C)) goto label80026788;
  at = 0x800785A8;
  s2 = lw(at + 0x28);
  s3 = lw(at + 0x24);
  gp = lw(ordered_linked_list);
  s2 = s2 >> 7;
  s3 = s3 >> 7;
  s3 -= 32; // 0xFFFFFFE0
label80026228:
  t7 = lw(ra + 0x00);
  ra += 4; // 0x04
  temp = t7 == 0;
  s1 = t7 & 0x1;
  if (temp) goto label80026788;
  t7 = t7 ^ s1;
  at = lw(t7 + 0x08);
  s6 = lhu(t7 + 0x0E);
  s4 = at >> 16;
  s5 = at & 0xFFFF;
  s4 = s4 - s7;
  s5 = t8 - s5;
  s6 = t9 - s6;
  t5 = lw(t7 + 0x10);
  t7 += 0x1C;
  sp = 0x1F800000;
  t2 = 0x010000;
  t3 = 0x01000000;
  t4 = 0x02000000;
  t6 = -1; // 0xFFFFFFFF
  at = lw(t7 + 0x00);
  t7 += 4; // 0x04
  s0 = t5 & 0xFF;
  s0 = s0 << 2;
  s0 += t7;
  s0 += 4; // 0x04
  v0 = at >> 21;
  v1 = at >> 10;
  v1 = v1 & 0x7FF;
  a0 = at & 0x3FF;
  v0 += s4;
  v1 = s5 - v1;
  a0 = s6 - a0;
  a0 = a0 << 16;
  v1 += a0;
  cop2.VZ0 = v0;
  cop2.VXY0 = v1;
  at = lw(t7 + 0x00);
  t7 += 4; // 0x04
label800262C0:
  RTPS();
  v0 = at >> 21;
  v1 = at >> 10;
  a0 = at & 0x3FF;
  at = lw(t7 + 0x00);
  v1 = v1 & 0x7FF;
  v0 += s4;
  v1 = s5 - v1;
  a0 = s6 - a0;
  a0 = a0 << 16;
  v1 += a0;
  cop2.VZ0 = v0;
  cop2.VXY0 = v1;
  v0 = cop2.SXY2;
  v1 = cop2.SZ3;
  temp = (int32_t)s1 > 0;
  t7 += 4; // 0x04
  if (temp) goto label8002631C;
  sw(sp + 0x00, v0);
  sw(sp + 0x04, v1);
  temp = t7 != s0;
  sp += 8; // 0x08
  if (temp) goto label800262C0;
  goto label80026364;
label8002631C:
  a0 = v0 << 5;
  a1 = v0 - t2;
  temp = (int32_t)a1 > 0;
  a1 = v0 - t3;
  if (temp) goto label80026330;
  a0++;
label80026330:
  temp = (int32_t)a1 < 0;
  a1 = v0 & 0xFE00;
  if (temp) goto label8002633C;
  a0 += 2; // 0x02
label8002633C:
  temp = a1 == 0;
  sw(sp + 0x04, v1);
  if (temp) goto label80026348;
  a0 += 4; // 0x04
label80026348:
  t6 = t6 & a0;
  sw(sp + 0x00, a0);
  temp = t7 != s0;
  sp += 8; // 0x08
  if (temp) goto label800262C0;
  t6 = t6 & 0xF;
  temp = t6 != 0;
  if (temp) goto label80026228;
label80026364:
  s4 = s0 - 8; // 0xFFFFFFF8
  s5 = t5 >> 6;
  s5 = s5 & 0x3FC;
  s5 += s4;
  s6 = t5 >> 13;
  t5 = lw(s5 + 0x00);
  s6 = s6 & 0x7F8;
  s6 += s5;
  t7 = 0x06000000;
  s0 = 0x08000000;
  sp = 0x1F800000;
label80026390:
  temp = s5 == s6;
  t6 = lw(s5 + 0x04);
  if (temp) goto label80026228;
  t1 = t5 >> 23;
  t1 = t1 & 0x1F8;
  t2 = t5 >> 17;
  t2 = t2 & 0x1F8;
  t3 = t5 >> 11;
  t3 = t3 & 0x1F8;
  t4 = t5 >> 5;
  t4 = t4 & 0x1F8;
  temp = t3 == t4;
  t1 += sp;
  if (temp) goto label800265D0;
  t2 += sp;
  t3 += sp;
  t4 += sp;
  at = lw(t1 + 0x00);
  v0 = lw(t2 + 0x00);
  v1 = lw(t3 + 0x00);
  temp = s1 == 0;
  a0 = lw(t4 + 0x00);
  if (temp) goto label80026404;
  a1 = at & v0;
  a1 = a1 & v1;
  a1 = a1 & a0;
  a1 = a1 & 0xF;
  temp = (int32_t)a1 > 0;
  at = (int32_t)at >> 5;
  if (temp) goto label8002677C;
  v0 = (int32_t)v0 >> 5;
  v1 = (int32_t)v1 >> 5;
  a0 = (int32_t)a0 >> 5;
label80026404:
  cop2.SXY0 = at;
  cop2.SXY1 = v0;
  cop2.SXY2 = v1;
  t1 = lw(t1 + 0x04);
  t2 = lw(t2 + 0x04);
  NCLIP();
  t3 = lw(t3 + 0x04);
  t4 = lw(t4 + 0x04);
  t0 = t1 + t2;
  t0 += t3;
  t0 += t4;
  a3 = t5 & 0x80;
  a2 = t5 & 0x1F;
  a1 = cop2.MAC0;
  cop2.SXY0 = a0;
  temp = (int32_t)a1 > 0;
  s5 += 8; // 0x08
  if (temp) goto label8002647C;
  NCLIP();
  t5 = lw(s5 + 0x00);
  temp = (int32_t)a3 > 0;
  t0 = t0 >> 5;
  if (temp) goto label80026484;
  a1 = t0 - s2;
  temp = (int32_t)a1 >= 0;
  a2 = a2 << 3;
  if (temp) goto label80026390;
  a3 = cop2.MAC0;
  a1 = a2 + t0;
  temp = (int32_t)a3 >= 0;
  a1 -= s3;
  if (temp) goto label80026390;
  a3 = t6 >> 24;
  goto label8002649C;
label8002647C:
  t5 = lw(s5 + 0x00);
  t0 = t0 >> 5;
label80026484:
  a1 = t0 - s2;
  temp = (int32_t)a1 >= 0;
  a2 = a2 << 3;
  if (temp) goto label80026390;
  a1 = a2 + t0;
  a1 -= s3;
  a3 = t6 >> 24;
label8002649C:
  temp = (int32_t)a1 >= 0;
  a3 = a3 & 0xFC;
  if (temp) goto label800264D4;
  a1 += 32; // 0x20
  temp = (int32_t)a1 <= 0;
  a2 = s3 - a2;
  if (temp) goto label80026390;
  a2 = a2 << 3;
  t1 -= a2;
  t2 -= a2;
  t3 -= a2;
  t4 -= a2;
  a2 = t1 & t2;
  a2 = a2 & t3;
  a2 = a2 & t4;
  temp = (int32_t)a2 < 0;
  if (temp) {
    a3 += s4;
    goto label80026390;
  }
label800264D4:
  a3 += s4;
  t1 = lw(a3 + 0x00);
  t2 = t6 >> 18;
  t2 = t2 & 0xFC;
  t2 += s4;
  t3 = t6 >> 12;
  t3 = t3 & 0xFC;
  t2 = lw(t2 + 0x00);
  t3 += s4;
  t4 = t6 >> 6;
  t4 = t4 & 0xFC;
  t4 += s4;
  a2 = t6 & 0xF8;
  t3 = lw(t3 + 0x00);
  t0 += a2;
  t0 += 64; // 0x40
  t0 = t0 << 3;
  t0 += gp;
  a3 = t6 & 0x4;
  t4 = lw(t4 + 0x00);
  temp = a3 == 0;
  a2 = 0x38000000;
  if (temp) goto label8002657C;
  a1 = 0x02000000;
  a2 += a1;
  sw(fp + 0x00, a1);
  sw(fp + 0x08, 0);
  a1 = t6 << 5;
  a1 = a1 & 0x7F;
  a1 = a1 | 0x600;
  sh(fp + 0x04, a1);
  a1 = 0 | 0xE100;
  sh(fp + 0x06, a1);
  a1 = lw(t0 + 0x00);
  sw(t0 + 0x00, fp);
  temp = a1 == 0;
  a3 = fp >> 16;
  if (temp) goto label80026574;
  sh(a1 + 0x00, fp);
  sb(a1 + 0x02, a3);
  fp += 12; // 0x0C
  goto label8002657C;
label80026574:
  sw(t0 + 0x04, fp);
  fp += 12; // 0x0C
label8002657C:
  t1 += a2;
  sw(fp + 0x00, s0);
  sw(fp + 0x04, t1);
  sw(fp + 0x08, at);
  sw(fp + 0x0C, t2);
  sw(fp + 0x10, v0);
  sw(fp + 0x14, t3);
  sw(fp + 0x18, v1);
  sw(fp + 0x1C, t4);
  sw(fp + 0x20, a0);
  at = lw(t0 + 0x00);
  sw(t0 + 0x00, fp);
  temp = at == 0;
  v0 = fp >> 16;
  if (temp) goto label800265C4;
  sh(at + 0x00, fp);
  sb(at + 0x02, v0);
  fp += 36; // 0x24
  goto label80026390;
label800265C4:
  sw(t0 + 0x04, fp);
  fp += 36; // 0x24
  goto label80026390;
label800265D0:
  t2 += sp;
  t3 += sp;
  at = lw(t1 + 0x00);
  v0 = lw(t2 + 0x00);
  temp = s1 == 0;
  v1 = lw(t3 + 0x00);
  if (temp) goto label80026604;
  a1 = at & v0;
  a1 = a1 & v1;
  a1 = a1 & 0x1F;
  temp = (int32_t)a1 > 0;
  at = (int32_t)at >> 5;
  if (temp) goto label8002677C;
  v0 = (int32_t)v0 >> 5;
  v1 = (int32_t)v1 >> 5;
label80026604:
  cop2.SXY0 = at;
  cop2.SXY1 = v0;
  cop2.SXY2 = v1;
  a3 = t5 & 0x80;
  a2 = t5 & 0x1F;
  NCLIP();
  t5 = lw(s5 + 0x08);
  s5 += 8; // 0x08
  a2 = a2 << 3;
  t4 = t6 >> 24;
  a3 = a3 << 23;
  t4 = t4 & 0xFC;
  a1 = cop2.MAC0;
  t1 = lw(t1 + 0x04);
  a1 += a3;
  temp = (int32_t)a1 <= 0;
  t2 = lw(t2 + 0x04);
  if (temp) goto label80026390;
  t3 = lw(t3 + 0x04);
  t0 = t1 + t2;
  t0 += t3;
  t0 += t3;
  t0 = t0 >> 5;
  a1 = t0 - s2;
  temp = (int32_t)a1 >= 0;
  t4 += s4;
  if (temp) goto label80026390;
  t4 = lw(t4 + 0x00);
  a1 = a2 + t0;
  a1 -= s3;
  temp = (int32_t)a1 >= 0;
  a1 += 32; // 0x20
  if (temp) goto label800266A0;
  temp = (int32_t)a1 <= 0;
  a2 = s3 - a2;
  if (temp) goto label80026390;
  a2 = a2 << 3;
  t1 -= a2;
  t2 -= a2;
  t3 -= a2;
  a2 = t1 & t2;
  a2 = a2 & t3;
  temp = (int32_t)a2 < 0;
  if (temp) {
    t2 = t6 >> 18;
    goto label80026390;
  }
label800266A0:
  t2 = t6 >> 18;
  t2 = t2 & 0xFC;
  t2 += s4;
  t2 = lw(t2 + 0x00);
  t3 = t6 >> 12;
  t3 = t3 & 0xFC;
  t3 += s4;
  a2 = t6 & 0xF8;
  t0 += a2;
  t3 = lw(t3 + 0x00);
  t0 += 64; // 0x40
  t0 = t0 << 3;
  t0 += gp;
  a3 = t6 & 0x4;
  temp = a3 == 0;
  a2 = 0x30000000;
  if (temp) goto label80026730;
  a1 = 0x02000000;
  a2 += a1;
  sw(fp + 0x00, a1);
  sw(fp + 0x08, 0);
  a1 = t6 << 5;
  a1 = a1 & 0x7F;
  a1 = a1 | 0x600;
  sh(fp + 0x04, a1);
  a1 = 0 | 0xE100;
  sh(fp + 0x06, a1);
  a1 = lw(t0 + 0x00);
  sw(t0 + 0x00, fp);
  temp = a1 == 0;
  a3 = fp >> 16;
  if (temp) goto label80026728;
  sh(a1 + 0x00, fp);
  sb(a1 + 0x02, a3);
  fp += 12; // 0x0C
  goto label80026730;
label80026728:
  sw(t0 + 0x04, fp);
  fp += 12; // 0x0C
label80026730:
  sw(fp + 0x00, t7);
  t4 += a2;
  sw(fp + 0x04, t4);
  sw(fp + 0x08, at);
  sw(fp + 0x0C, t2);
  sw(fp + 0x10, v0);
  sw(fp + 0x14, t3);
  sw(fp + 0x18, v1);
  at = lw(t0 + 0x00);
  sw(t0 + 0x00, fp);
  temp = at == 0;
  v0 = fp >> 16;
  if (temp) goto label80026770;
  sh(at + 0x00, fp);
  sb(at + 0x02, v0);
  fp += 28; // 0x1C
  goto label80026390;
label80026770:
  sw(t0 + 0x04, fp);
  fp += 28; // 0x1C
  goto label80026390;
label8002677C:
  t5 = lw(s5 + 0x08);
  s5 += 8; // 0x08
  goto label80026390;
label80026788:
  at = ordered_linked_list;
  s4 = lw(at + 0x00);
  at = 0x8006FCF4; // &0x0EA69B
  ra = at + 7168; // 0x1C00
  sp = at + 7680; // 0x1E00
  gp = at + 8192; // 0x2000
  at = 0x800785A8;
  s5 = lw(at + 0x24);
  s3 = lw(at + 0x18);
  v1 = 0x80076DD0;
  at = lw(v1 + 0x28);
  v0 = lw(v1 + 0x2C);
  v1 = lw(v1 + 0x30);
  at = at >> 2;
  v0 = v0 >> 2;
  v1 = v1 >> 2;
  cop2.RBK = at;
  cop2.GBK = v0;
  cop2.BBK = v1;
label800267E4:
  t9 = lw(ra + 0x00);
  ra += 4; // 0x04
  temp = t9 == 0;
  s1 = t9 & 0x3;
  if (temp) goto label80027688;
  t9 = t9 ^ s1;
  at = lw(t9 + 0x08);
  v1 = cop2.RBK;
  a0 = cop2.GBK;
  a1 = cop2.BBK;
  s6 = 0x8006FCF4; // &0x0EA69B
  v0 = lw(t9 + 0x0C);
  t4 = at >> 14;
  t5 = at & 0xFFFF;
  t5 = t5 << 2;
  t4 -= v1;
  t5 = a0 - t5;
  t6 = v0 >> 14;
  t6 = a1 - t6;
  v0 = v0 & 0xFFFF;
  s6 += v0;
  s2 = lw(t9 + 0x14);
  s7 = 0x1F800000;
  t8 = s7 + 680; // 0x02A8
  t1 = 0x010000;
  t2 = 0x01000000;
  t3 = 0x02000000;
  t0 = -1; // 0xFFFFFFFF
  t7 = s2 >> 22;
  t7 = t7 & 0x3FC;
  t7 += 28; // 0x1C
  t7 += t9;
  at = lw(t7 + 0x00);
  t7 += 4; // 0x04
  s0 = s2 & 0xFF;
  s0 = s0 << 2;
  s0 += t7;
  s0 += 4; // 0x04
  v0 = at >> 19;
  v0 = v0 & 0x1FFC;
  v1 = at >> 8;
  v1 = v1 & 0x1FFC;
  a0 = at << 2;
  at = lw(t7 + 0x00);
  a0 = a0 & 0xFFC;
  v0 += t4;
  v1 = t5 - v1;
  a0 = t6 - a0;
  a0 = a0 << 16;
  v1 += a0;
  cop2.VZ0 = v0;
  cop2.VXY0 = v1;
  v0 = s1 & 0x2;
  temp = (int32_t)v0 > 0;
  t7 += 4; // 0x04
  if (temp) goto label800269CC;
  temp = (int32_t)s1 > 0;
  if (temp) goto label80026930;
label800268C8:
  RTPS();
  v0 = at >> 19;
  v1 = at >> 8;
  a0 = at << 2;
  at = lw(t7 + 0x00);
  v0 = v0 & 0x1FFC;
  v1 = v1 & 0x1FFC;
  a0 = a0 & 0xFFC;
  v0 += t4;
  v1 = t5 - v1;
  a0 = t6 - a0;
  a0 = a0 << 16;
  v1 += a0;
  t7 += 4; // 0x04
  cop2.VZ0 = v0;
  cop2.VXY0 = v1;
  v0 = cop2.SXY2;
  v1 = cop2.SZ3;
  sw(s7 + 0x00, v0);
  sh(t8 + 0x00, v1);
  s7 += 4; // 0x04
  temp = t7 != s0;
  t8 += 2; // 0x02
  if (temp) goto label800268C8;
  s6 = s6 << 1;
  s6 = s6 >> 1;
  goto label80026B48;
label80026930:
  RTPS();
  v0 = at >> 19;
  v1 = at >> 8;
  a0 = at << 2;
  at = lw(t7 + 0x00);
  v0 = v0 & 0x1FFC;
  v1 = v1 & 0x1FFC;
  a0 = a0 & 0xFFC;
  v0 += t4;
  v1 = t5 - v1;
  a0 = t6 - a0;
  a0 = a0 << 16;
  v1 += a0;
  t7 += 4; // 0x04
  cop2.VZ0 = v0;
  cop2.VXY0 = v1;
  a0 = cop2.SXY2;
  a1 = cop2.SZ3;
  v0 = a0 << 5;
  v1 = a0 - t1;
  temp = (int32_t)v1 > 0;
  v1 = a0 - t2;
  if (temp) goto label8002698C;
  v0++;
label8002698C:
  temp = (int32_t)v1 < 0;
  v1 = a0 & 0xFE00;
  if (temp) goto label80026998;
  v0 += 2; // 0x02
label80026998:
  temp = v1 == 0;
  sh(t8 + 0x00, a1);
  if (temp) goto label800269A4;
  v0 += 12; // 0x0C
label800269A4:
  t0 = t0 & v0;
  sw(s7 + 0x00, v0);
  s7 += 4; // 0x04
  temp = t7 != s0;
  t8 += 2; // 0x02
  if (temp) goto label80026930;
  t0 = t0 & 0xF;
  temp = t0 != 0;
  if (temp) goto label800267E4;
  goto label80026B48;
label800269CC:
  RTPS();
  v0 = at >> 19;
  v1 = at >> 8;
  a0 = at << 2;
  at = lw(t7 + 0x00);
  v0 = v0 & 0x1FFC;
  v1 = v1 & 0x1FFC;
  a0 = a0 & 0xFFC;
  v0 += t4;
  v1 = t5 - v1;
  a0 = t6 - a0;
  a0 = a0 << 16;
  v1 += a0;
  t7 += 4; // 0x04
  a1 = cop2.SZ3;
  a0 = cop2.SXY2;
  a2 = a1 - 1536; // 0xFFFFFA00
  temp = (int32_t)a2 >= 0;
  s7 += 4; // 0x04
  if (temp) goto label80026AF8;
  a2 = a1 - 256; // 0xFFFFFF00
  temp = (int32_t)a2 >= 0;
  t8 += 2; // 0x02
  if (temp) goto label80026A8C;
  temp = a2 == 0;
  if (temp) goto label80026A8C;
  a2 = cop2.MAC1;
  a3 = cop2.MAC2;
  a2 += 256; // 0x0100
  temp = (int32_t)a2 <= 0;
  a2 -= 512; // 0xFFFFFE00
  if (temp) goto label80026A8C;
  temp = (int32_t)a2 >= 0;
  a3 += 256; // 0x0100
  if (temp) goto label80026A8C;
  temp = (int32_t)a3 <= 0;
  a3 -= 512; // 0xFFFFFE00
  if (temp) goto label80026A8C;
  temp = (int32_t)a3 >= 0;
  if (temp) goto label80026A8C;
  a2 = cop2.VZ0;
  a3 = cop2.VXY0;
  a2 = a2 << 4;
  cop2.VZ0 = a2;
  a3 = a3 << 4;
  a2 = 0xFFF0FFF0;
  a3 = a3 & a2;
  cop2.VXY0 = a3;
  RTPS();
  a0 = cop2.SXY2;
label80026A8C:
  a2 = cop2.FLAG;
  cop2.VZ0 = v0;
  cop2.VXY0 = v1;
  v0 = a0 << 5;
  temp = (int32_t)a2 >= 0;
  v1 = a0 - t1;
  if (temp) goto label80026AA8;
  v0 += 16; // 0x10
label80026AA8:
  temp = (int32_t)v1 > 0;
  v1 = a0 - t2;
  if (temp) goto label80026AB4;
  v0++;
label80026AB4:
  temp = (int32_t)v1 < 0;
  v1 = a0 << 16;
  if (temp) goto label80026AC0;
  v0 += 2; // 0x02
label80026AC0:
  temp = (int32_t)v1 > 0;
  v1 -= t3;
  if (temp) goto label80026ACC;
  v0 += 4; // 0x04
label80026ACC:
  temp = (int32_t)v1 < 0;
  sh(t8 - 0x02, a1); // 0xFFFFFFFE
  if (temp) goto label80026AD8;
  v0 += 8; // 0x08
label80026AD8:
  t0 = t0 & v0;
  temp = t7 != s0;
  sw(s7 - 0x04, v0); // 0xFFFFFFFC
  if (temp) goto label800269CC;
  t0 = t0 & 0xF;
  temp = t0 != 0;
  if (temp) goto label800267E4;
  goto label80026B48;
label80026AF8:
  t8 += 2; // 0x02
  cop2.VZ0 = v0;
  cop2.VXY0 = v1;
  v0 = a0 << 5;
  v1 = a0 - t1;
  temp = (int32_t)v1 > 0;
  v1 = a0 - t2;
  if (temp) goto label80026B18;
  v0++;
label80026B18:
  temp = (int32_t)v1 < 0;
  v1 = a0 & 0xFE00;
  if (temp) goto label80026B24;
  v0 += 2; // 0x02
label80026B24:
  temp = v1 == 0;
  sh(t8 - 0x02, a1); // 0xFFFFFFFE
  if (temp) goto label80026B30;
  v0 += 12; // 0x0C
label80026B30:
  t0 = t0 & v0;
  temp = t7 != s0;
  sw(s7 - 0x04, v0); // 0xFFFFFFFC
  if (temp) goto label800269CC;
  t0 = t0 & 0xF;
  temp = t0 != 0;
  if (temp) goto label800267E4;
label80026B48:
  t7 = s0 - 8; // 0xFFFFFFF8
  s1 = s2 >> 6;
  s1 = s1 & 0x3FC;
  s0 = t7 + s1;
  s1 += s0;
  t6 = lw(s1 + 0x00);
  s2 = s2 >> 12;
  s2 = s2 & 0xFF0;
  s2 += s1;
  s7 = 0x1F800000;
  t8 = s7 + 680; // 0x02A8
label80026B74:
  temp = s1 == s2;
  at = t6 >> 22;
  if (temp) goto label800267E4;
  v0 = t6 >> 14;
  v1 = t6 >> 6;
  a0 = t6 << 2;
  t6 = lw(s1 + 0x10);
  at = at & 0x3FC;
  v0 = v0 & 0x3FC;
  v1 = v1 & 0x3FC;
  a0 = a0 & 0x3FC;
  temp = v1 == a0;
  s1 += 16; // 0x10
  if (temp) goto label80027168;
  temp = (int32_t)s6 > 0;
  s6++;
  if (temp) goto label80026C08;
  a1 = at + s7;
  a2 = v0 + s7;
  a3 = v1 + s7;
  t0 = a0 + s7;
  a1 = lw(a1 + 0x00);
  a2 = lw(a2 + 0x00);
  a3 = lw(a3 + 0x00);
  t0 = lw(t0 + 0x00);
  t1 = a1 & a2;
  t1 = t1 & a3;
  t1 = t1 & t0;
  t1 = t1 & 0xF;
  temp = (int32_t)t1 > 0;
  t1 = a1 | a2;
  if (temp) goto label80026B74;
  t1 = t1 | a3;
  t1 = t1 | t0;
  t5 = lw(s1 - 0x04); // 0xFFFFFFFC
  a1 = (int32_t)a1 >> 5;
  a2 = (int32_t)a2 >> 5;
  a3 = (int32_t)a3 >> 5;
  t0 = (int32_t)t0 >> 5;
  t1 = t1 & 0x10;
  goto label80026C30;
label80026C08:
  t5 = lw(s1 - 0x04); // 0xFFFFFFFC
  t1 = 0;
  a1 = at + s7;
  a2 = v0 + s7;
  a3 = v1 + s7;
  t0 = a0 + s7;
  a1 = lw(a1 + 0x00);
  a2 = lw(a2 + 0x00);
  a3 = lw(a3 + 0x00);
  t0 = lw(t0 + 0x00);
label80026C30:
  cop2.SXY0 = a1;
  cop2.SXY1 = a2;
  cop2.SXY2 = t0;
  at = at >> 1;
  at += t8;
  NCLIP();
  v0 = v0 >> 1;
  v0 += t8;
  v1 = v1 >> 1;
  v1 += t8;
  a0 = a0 >> 1;
  temp = (int32_t)t1 > 0;
  t4 = t5 & 0x2;
  if (temp) goto label80026CD8;
  t2 = cop2.MAC0;
  temp = t4 == 0;
  a0 += t8;
  if (temp) goto label80026C74;
  t2 = -t2;
label80026C74:
  temp = (int32_t)t2 >= 0;
  at = lhu(at + 0x00);
  if (temp) goto label80026CE0;
  cop2.SXY0 = a3;
  v0 = lhu(v0 + 0x00);
  v1 = lhu(v1 + 0x00);
  NCLIP();
  a0 = lhu(a0 + 0x00);
  t2 = t5 & 0x4;
  temp = (int32_t)t2 > 0;
  t2 = at + v0;
  if (temp) goto label80026CF0;
  t2 += v1;
  temp = (int32_t)t4 > 0;
  t2 += a0;
  if (temp) goto label80026CC0;
  t4 = cop2.MAC0;
  t3 = s5 - t2;
  temp = (int32_t)t4 > 0;
  if (temp) goto label80026B74;
  goto label80026CFC;
label80026CC0:
  t4 = cop2.MAC0;
  t3 = s5 - t2;
  temp = (int32_t)t4 < 0;
  if (temp) goto label80026B74;
  goto label80026CFC;
label80026CD8:
  a0 += t8;
  at = lhu(at + 0x00);
label80026CE0:
  v0 = lhu(v0 + 0x00);
  v1 = lhu(v1 + 0x00);
  a0 = lhu(a0 + 0x00);
  t2 = at + v0;
label80026CF0:
  t2 += v1;
  t2 += a0;
  t3 = s5 - t2;
label80026CFC:
  temp = (int32_t)t3 <= 0;
  t3 = t2 - 8192; // 0xFFFFE000
  if (temp) goto label80026B74;
  temp = (int32_t)t3 >= 0;
  t3 = t5 & 0x80;
  if (temp) goto label80026DB8;
  temp = (int32_t)t3 > 0;
  t3 = at - 320; // 0xFFFFFEC0
  if (temp) goto label80026DB8;
  temp = (int32_t)t3 < 0;
  t3 = v0 - 320; // 0xFFFFFEC0
  if (temp) goto label80026D70;
  temp = (int32_t)t3 < 0;
  t3 = v1 - 320; // 0xFFFFFEC0
  if (temp) goto label80026D70;
  temp = (int32_t)t3 < 0;
  t3 = a0 - 320; // 0xFFFFFEC0
  if (temp) goto label80026D70;
  temp = (int32_t)t3 < 0;
  t3 = t5 & 0x40;
  if (temp) goto label80026D70;
  temp = (int32_t)t3 > 0;
  t3 = t9 & 0x1;
  if (temp) goto label80026DB8;
label80026D3C:
  temp = (int32_t)t3 > 0;
  t9 = t9 | 0x1;
  if (temp) goto label80026D50;
  t3 = t9 >> 2;
  sw(gp + 0x00, t3);
  gp += 4; // 0x04
label80026D50:
  t3 = s1 - 16; // 0xFFFFFFF0
  t1 = t1 >> 4;
  t3 += t1;
  sw(gp + 0x00, t3);
  gp += 4; // 0x04
  t3 = 2; // 0x02
  sb(s6 - 0x01, t3); // 0xFFFFFFFF
  goto label80026B74;
label80026D70:
  temp = t2 == 0;
  t3 = t5 & 0x40;
  if (temp) goto label80026B74;
  temp = (int32_t)t3 > 0;
  t3 = t9 & 0x1;
  if (temp) goto label80026D3C;
  t3 = t9 & 0x2;
  temp = (int32_t)t3 > 0;
  t9 = t9 | 0x2;
  if (temp) goto label80026D98;
  t3 = t9 >> 2;
  sw(sp + 0x00, t3);
  sp += 4; // 0x04
label80026D98:
  t3 = s1 - 16; // 0xFFFFFFF0
  t1 = t1 >> 4;
  t3 += t1;
  sw(sp + 0x00, t3);
  sp += 4; // 0x04
  t3 = 4; // 0x04
  sb(s6 - 0x01, t3); // 0xFFFFFFFF
  goto label80026B74;
label80026DB8:
  t4 = lw(s1 - 0x0C); // 0xFFFFFFF4
  t2 = t2 >> 7;
  t3 = t5 & 0x38;
  t3 = t3 >> 1;
  t3 += t2;
  t3 = t3 << 3;
  sw(fp + 0x08, a1);
  sw(fp + 0x14, a2);
  sw(fp + 0x20, t0);
  sw(fp + 0x2C, a3);
  a1 = t5 & 0x1;
  temp = a1 == 0;
  t5 = s5 >> 2;
  if (temp) goto label80026DF0;
  t5 += 8192; // 0x2000
label80026DF0:
  a2 = t4 >> 22;
  a2 = a2 & 0x3FC;
  a1 = a2 + t7;
  a1 = lw(a1 + 0x00);
  at = t5 - at;
  t0 = at - 4096; // 0xFFFFF000
  temp = (int32_t)t0 >= 0;
  a2 += s0;
  if (temp) goto label80026FB4;
  temp = (int32_t)at <= 0;
  a2 = lw(a2 + 0x00);
  if (temp) goto label80026FAC;
  a3 = a1 << 4;
  a3 = a3 & 0xFF0;
  t0 = a1 >> 4;
  t0 = t0 & 0xFF0;
  t1 = a1 >> 12;
  cop2.RFC = a3;
  cop2.GFC = t0;
  cop2.BFC = t1;
  cop2.RGBC = a2;
  cop2.IR0 = at;
  a2 = t4 >> 14;
  a2 = a2 & 0x3FC;
  DPCS();
  a1 = a2 + t7;
  a1 = lw(a1 + 0x00);
  v0 = t5 - v0;
  t0 = v0 - 4096; // 0xFFFFF000
  a2 += s0;
  a3 = cop2.RGB2;
  at = 0x3C000000;
  a3 = a3 | at;
  sw(fp + 0x04, a3);
label80026E70:
  temp = (int32_t)t0 >= 0;
  if (temp) goto label80026FE8;
  temp = (int32_t)v0 <= 0;
  a2 = lw(a2 + 0x00);
  if (temp) goto label80026FE0;
  a3 = a1 << 4;
  a3 = a3 & 0xFF0;
  t0 = a1 >> 4;
  t0 = t0 & 0xFF0;
  t1 = a1 >> 12;
  cop2.RFC = a3;
  cop2.GFC = t0;
  cop2.BFC = t1;
  cop2.RGBC = a2;
  cop2.IR0 = v0;
  a2 = t4 >> 6;
  a2 = a2 & 0x3FC;
  DPCS();
  a1 = a2 + t7;
  a1 = lw(a1 + 0x00);
  v1 = t5 - v1;
  t0 = v1 - 4096; // 0xFFFFF000
  a2 += s0;
  a3 = cop2.RGB2;
  sw(fp + 0x10, a3);
label80026ED4:
  temp = (int32_t)t0 >= 0;
  if (temp) goto label80027014;
  temp = (int32_t)v1 <= 0;
  a2 = lw(a2 + 0x00);
  if (temp) goto label8002700C;
  a3 = a1 << 4;
  a3 = a3 & 0xFF0;
  t0 = a1 >> 4;
  t0 = t0 & 0xFF0;
  t1 = a1 >> 12;
  cop2.RFC = a3;
  cop2.GFC = t0;
  cop2.BFC = t1;
  cop2.RGBC = a2;
  cop2.IR0 = v1;
  a2 = t4 << 2;
  a2 = a2 & 0x3FC;
  DPCS();
  a1 = a2 + t7;
  a1 = lw(a1 + 0x00);
  a0 = t5 - a0;
  t0 = a0 - 4096; // 0xFFFFF000
  a2 += s0;
  a3 = cop2.RGB2;
  sw(fp + 0x28, a3);
label80026F38:
  temp = (int32_t)t0 >= 0;
  if (temp) goto label80027040;
  temp = (int32_t)a0 <= 0;
  a2 = lw(a2 + 0x00);
  if (temp) goto label80027038;
  a3 = a1 << 4;
  a3 = a3 & 0xFF0;
  t0 = a1 >> 4;
  t0 = t0 & 0xFF0;
  t1 = a1 >> 12;
  cop2.RFC = a3;
  cop2.GFC = t0;
  cop2.BFC = t1;
  cop2.RGBC = a2;
  cop2.IR0 = a0;
  t0 = t2 << 5;
  t0 -= t5;
  DPCS();
  at = lb(s1 - 0x08); // 0xFFFFFFF8
  t0 += 4096; // 0x1000
  t5 = s3;
  temp = (int32_t)t0 > 0;
  t0 = t0 >> 8;
  if (temp) goto label80026F98;
  t0 = 0;
  t5 += 8; // 0x08
label80026F98:
  v0 = 1; // 0x01
  sb(s6 - 0x01, v0); // 0xFFFFFFFF
  a3 = cop2.RGB2;
  sw(fp + 0x1C, a3);
  goto label80027070;
label80026FAC:
  a1 = a2;
label80026FB4:
  at = 0x3C000000;
  a1 = a1 | at;
  sw(fp + 0x04, a1);
  a2 = t4 >> 14;
  a2 = a2 & 0x3FC;
  a1 = a2 + t7;
  a1 = lw(a1 + 0x00);
  v0 = t5 - v0;
  t0 = v0 - 4096; // 0xFFFFF000
  a2 += s0;
  goto label80026E70;
label80026FE0:
  a1 = a2;
label80026FE8:
  sw(fp + 0x10, a1);
  a2 = t4 >> 6;
  a2 = a2 & 0x3FC;
  a1 = a2 + t7;
  a1 = lw(a1 + 0x00);
  v1 = t5 - v1;
  t0 = v1 - 4096; // 0xFFFFF000
  a2 += s0;
  goto label80026ED4;
label8002700C:
  a1 = a2;
label80027014:
  sw(fp + 0x28, a1);
  a2 = t4 << 2;
  a2 = a2 & 0x3FC;
  a1 = a2 + t7;
  a1 = lw(a1 + 0x00);
  a0 = t5 - a0;
  t0 = a0 - 4096; // 0xFFFFF000
  a2 += s0;
  goto label80026F38;
label80027038:
  a1 = a2;
label80027040:
  sw(fp + 0x1C, a1);
  t0 = t2 << 5;
  t0 -= t5;
  t0 += 4096; // 0x1000
  at = lb(s1 - 0x08); // 0xFFFFFFF8
  t5 = s3;
  temp = (int32_t)t0 > 0;
  t0 = t0 >> 8;
  if (temp) goto label80027068;
  t0 = 0;
  t5 += 8; // 0x08
label80027068:
  v0 = 1; // 0x01
  sb(s6 - 0x01, v0); // 0xFFFFFFFF
label80027070:
  t4 = at << 4;
  t4 += t5;
  temp = (int32_t)at >= 0;
  a1 = 0x0C000000;
  if (temp) goto label8002709C;
  v0 = 62; // 0x3E
  sb(fp + 0x07, v0);
  v0 = at + 1; // 0x01
  temp = v0 == 0;
  at = at & 0x7F;
  if (temp) goto label800270F8;
  t4 = at << 4;
  t4 += t5;
label8002709C:
  at = lw(t4 + 0x00);
  v0 = lw(t4 + 0x04);
  t0 = t0 << 22;
  at += t0;
  v1 = at + 7936; // 0x1F00
  a0 = v1 + 31; // 0x1F
  sw(fp + 0x0C, at);
  sw(fp + 0x18, v0);
  sw(fp + 0x24, v1);
  sw(fp + 0x30, a0);
  sw(fp + 0x00, a1);
  t3 += s4;
  at = lw(t3 + 0x00);
  sw(t3 + 0x00, fp);
  temp = at == 0;
  v0 = fp >> 16;
  if (temp) goto label800270EC;
  sh(at + 0x00, fp);
  sb(at + 0x02, v0);
  fp += 52; // 0x34
  goto label80026B74;
label800270EC:
  sw(t3 + 0x04, fp);
  fp += 52; // 0x34
  goto label80026B74;
label800270F8:
  at = lw(fp + 0x14);
  v0 = lw(fp + 0x20);
  v1 = lw(fp + 0x2C);
  a0 = lw(fp + 0x10);
  a1 = lw(fp + 0x1C);
  a2 = lw(fp + 0x28);
  sw(fp + 0x10, at);
  sw(fp + 0x18, v0);
  sw(fp + 0x20, v1);
  sw(fp + 0x0C, a0);
  sw(fp + 0x14, a1);
  sw(fp + 0x1C, a2);
  at = 56; // 0x38
  sb(fp + 0x07, at);
  at = 0x08000000;
  sw(fp + 0x00, at);
  t3 += s4;
  at = lw(t3 + 0x00);
  sw(t3 + 0x00, fp);
  temp = at == 0;
  v0 = fp >> 16;
  if (temp) goto label8002715C;
  sh(at + 0x00, fp);
  sb(at + 0x02, v0);
  fp += 36; // 0x24
  goto label80026B74;
label8002715C:
  sw(t3 + 0x04, fp);
  fp += 36; // 0x24
  goto label80026B74;
label80027168:
  temp = (int32_t)s6 > 0;
  s6++;
  if (temp) goto label800271B8;
  a1 = at + s7;
  a2 = v0 + s7;
  a3 = v1 + s7;
  a1 = lw(a1 + 0x00);
  a2 = lw(a2 + 0x00);
  a3 = lw(a3 + 0x00);
  t1 = a1 & a2;
  t1 = t1 & a3;
  t1 = t1 & 0xF;
  temp = (int32_t)t1 > 0;
  t1 = a1 | a2;
  if (temp) goto label80026B74;
  t1 = t1 | a3;
  t5 = lw(s1 - 0x04); // 0xFFFFFFFC
  a1 = (int32_t)a1 >> 5;
  a2 = (int32_t)a2 >> 5;
  a3 = (int32_t)a3 >> 5;
  t1 = t1 & 0x10;
  goto label800271D8;
label800271B8:
  t5 = lw(s1 - 0x04); // 0xFFFFFFFC
  t1 = 0;
  a1 = at + s7;
  a2 = v0 + s7;
  a3 = v1 + s7;
  a1 = lw(a1 + 0x00);
  a2 = lw(a2 + 0x00);
  a3 = lw(a3 + 0x00);
label800271D8:
  cop2.SXY0 = a1;
  cop2.SXY1 = a2;
  cop2.SXY2 = a3;
  at = at >> 1;
  at += t8;
  NCLIP();
  v0 = v0 >> 1;
  v0 += t8;
  t2 = t5 & 0x4;
  temp = (int32_t)t2 > 0;
  v1 = v1 >> 1;
  if (temp) goto label80027234;
  temp = (int32_t)t1 > 0;
  v1 += t8;
  if (temp) goto label80027238;
  t2 = cop2.MAC0;
  t4 = t5 & 0x2;
  temp = t4 == 0;
  at = lhu(at + 0x00);
  if (temp) goto label80027220;
  t2 = -t2;
label80027220:
  temp = (int32_t)t2 < 0;
  v0 = lhu(v0 + 0x00);
  if (temp) goto label80026B74;
  v1 = lhu(v1 + 0x00);
  goto label80027244;
label80027234:
  v1 += t8;
label80027238:
  at = lhu(at + 0x00);
  v0 = lhu(v0 + 0x00);
  v1 = lhu(v1 + 0x00);
label80027244:
  t2 = at + v0;
  t2 += v1;
  t2 += v1;
  t3 = s5 - t2;
  temp = (int32_t)t3 <= 0;
  t3 = t2 - 8192; // 0xFFFFE000
  if (temp) goto label80026B74;
  temp = (int32_t)t3 >= 0;
  t3 = t5 & 0x80;
  if (temp) goto label80027310;
  temp = (int32_t)t3 > 0;
  t3 = at - 320; // 0xFFFFFEC0
  if (temp) goto label80027310;
  temp = (int32_t)t3 < 0;
  t3 = v0 - 320; // 0xFFFFFEC0
  if (temp) goto label800272C4;
  temp = (int32_t)t3 < 0;
  t3 = v1 - 320; // 0xFFFFFEC0
  if (temp) goto label800272C4;
  temp = (int32_t)t3 < 0;
  t3 = t5 & 0x40;
  if (temp) goto label800272C4;
  temp = (int32_t)t3 > 0;
  t3 = t9 & 0x1;
  if (temp) goto label80027310;
label8002728C:
  temp = (int32_t)t3 > 0;
  t9 = t9 | 0x1;
  if (temp) goto label800272A0;
  t3 = t9 >> 2;
  sw(gp + 0x00, t3);
  gp += 4; // 0x04
label800272A0:
  t3 = s1 - 16; // 0xFFFFFFF0
  t1 = t1 >> 4;
  t3 += t1;
  t3 = t3 | 0x2;
  sw(gp + 0x00, t3);
  gp += 4; // 0x04
  t3 = 2; // 0x02
  sb(s6 - 0x01, t3); // 0xFFFFFFFF
  goto label80026B74;
label800272C4:
  temp = t2 == 0;
  t3 = t5 & 0x40;
  if (temp) goto label80026B74;
  temp = (int32_t)t3 > 0;
  t3 = t9 & 0x1;
  if (temp) goto label8002728C;
  t3 = t9 & 0x2;
  temp = (int32_t)t3 > 0;
  t9 = t9 | 0x2;
  if (temp) goto label800272EC;
  t3 = t9 >> 2;
  sw(sp + 0x00, t3);
  sp += 4; // 0x04
label800272EC:
  t3 = s1 - 16; // 0xFFFFFFF0
  t1 = t1 >> 4;
  t3 += t1;
  t3 = t3 | 0x2;
  sw(sp + 0x00, t3);
  sp += 4; // 0x04
  t3 = 4; // 0x04
  sb(s6 - 0x01, t3); // 0xFFFFFFFF
  goto label80026B74;
label80027310:
  t4 = lw(s1 - 0x0C); // 0xFFFFFFF4
  t2 = t2 >> 7;
  t3 = t5 & 0x38;
  t3 = t3 >> 1;
  t3 += t2;
  t3 = t3 << 3;
  sw(fp + 0x08, a1);
  sw(fp + 0x14, a2);
  sw(fp + 0x20, a3);
  a1 = t5 & 0x1;
  temp = a1 == 0;
  t5 = s5 >> 2;
  if (temp) goto label80027344;
  t5 += 8192; // 0x2000
label80027344:
  a2 = t4 >> 22;
  a2 = a2 & 0x3FC;
  a1 = a2 + t7;
  a1 = lw(a1 + 0x00);
  at = t5 - at;
  t0 = at - 4096; // 0xFFFFF000
  temp = (int32_t)t0 >= 0;
  a2 += s0;
  if (temp) goto label800274A4;
  temp = (int32_t)at <= 0;
  a2 = lw(a2 + 0x00);
  if (temp) goto label8002749C;
  a3 = a1 << 4;
  a3 = a3 & 0xFF0;
  t0 = a1 >> 4;
  t0 = t0 & 0xFF0;
  t1 = a1 >> 12;
  cop2.RFC = a3;
  cop2.GFC = t0;
  cop2.BFC = t1;
  cop2.RGBC = a2;
  cop2.IR0 = at;
  a2 = t4 >> 14;
  a2 = a2 & 0x3FC;
  DPCS();
  a1 = a2 + t7;
  a1 = lw(a1 + 0x00);
  v0 = t5 - v0;
  t0 = v0 - 4096; // 0xFFFFF000
  a2 += s0;
  a3 = cop2.RGB2;
  at = 0x34000000;
  a3 = a3 | at;
  sw(fp + 0x04, a3);
label800273C4:
  temp = (int32_t)t0 >= 0;
  if (temp) goto label800274D8;
  temp = (int32_t)v0 <= 0;
  a2 = lw(a2 + 0x00);
  if (temp) goto label800274D0;
  a3 = a1 << 4;
  a3 = a3 & 0xFF0;
  t0 = a1 >> 4;
  t0 = t0 & 0xFF0;
  t1 = a1 >> 12;
  cop2.RFC = a3;
  cop2.GFC = t0;
  cop2.BFC = t1;
  cop2.RGBC = a2;
  cop2.IR0 = v0;
  a2 = t4 >> 6;
  a2 = a2 & 0x3FC;
  DPCS();
  a1 = a2 + t7;
  a1 = lw(a1 + 0x00);
  v1 = t5 - v1;
  t0 = v1 - 4096; // 0xFFFFF000
  a2 += s0;
  a3 = cop2.RGB2;
  sw(fp + 0x10, a3);
label80027428:
  temp = (int32_t)t0 >= 0;
  if (temp) goto label80027504;
  temp = (int32_t)v1 <= 0;
  a2 = lw(a2 + 0x00);
  if (temp) goto label800274FC;
  a3 = a1 << 4;
  a3 = a3 & 0xFF0;
  t0 = a1 >> 4;
  t0 = t0 & 0xFF0;
  t1 = a1 >> 12;
  cop2.RFC = a3;
  cop2.GFC = t0;
  cop2.BFC = t1;
  cop2.RGBC = a2;
  cop2.IR0 = v1;
  t0 = t2 << 5;
  t0 -= t5;
  DPCS();
  t4 = lw(s1 - 0x08); // 0xFFFFFFF8
  t0 += 4096; // 0x1000
  t5 = s3;
  temp = (int32_t)t0 > 0;
  t0 = t0 >> 8;
  if (temp) goto label80027488;
  t0 = 0;
  t5 += 8; // 0x08
label80027488:
  v0 = 1; // 0x01
  sb(s6 - 0x01, v0); // 0xFFFFFFFF
  a3 = cop2.RGB2;
  sw(fp + 0x1C, a3);
  goto label80027534;
label8002749C:
  a1 = a2;
label800274A4:
  at = 0x34000000;
  a1 = a1 | at;
  sw(fp + 0x04, a1);
  a2 = t4 >> 14;
  a2 = a2 & 0x3FC;
  a1 = a2 + t7;
  a1 = lw(a1 + 0x00);
  v0 = t5 - v0;
  t0 = v0 - 4096; // 0xFFFFF000
  a2 += s0;
  goto label800273C4;
label800274D0:
  a1 = a2;
label800274D8:
  sw(fp + 0x10, a1);
  a2 = t4 >> 6;
  a2 = a2 & 0x3FC;
  a1 = a2 + t7;
  a1 = lw(a1 + 0x00);
  v1 = t5 - v1;
  t0 = v1 - 4096; // 0xFFFFF000
  a2 += s0;
  goto label80027428;
label800274FC:
  a1 = a2;
label80027504:
  sw(fp + 0x1C, a1);
  t0 = t2 << 5;
  t0 -= t5;
  t0 += 4096; // 0x1000
  t4 = lw(s1 - 0x08); // 0xFFFFFFF8
  t5 = s3;
  temp = (int32_t)t0 > 0;
  t0 = t0 >> 8;
  if (temp) goto label8002752C;
  t0 = 0;
  t5 += 8; // 0x08
label8002752C:
  v0 = 1; // 0x01
  sb(s6 - 0x01, v0); // 0xFFFFFFFF
label80027534:
  t2 = t4 & 0x7F;
  t2 = t2 << 4;
  t2 += t5;
  at = t4 << 24;
  temp = (int32_t)at >= 0;
  a1 = 0x09000000;
  if (temp) goto label8002756C;
  v0 = 54; // 0x36
  sb(fp + 0x07, v0);
  at = (int32_t)at >> 24;
  v0 = at + 1; // 0x01
  temp = v0 == 0;
  at = at & 0x7F;
  if (temp) goto label80027628;
  t2 = at << 4;
  t2 += t5;
label8002756C:
  at = lw(t2 + 0x00);
  v0 = lw(t2 + 0x04);
  t4 = t4 & 0x300;
  t0 = t0 << 22;
  at += t0;
  temp = (int32_t)t4 > 0;
  t4 -= 256; // 0xFFFFFF00
  if (temp) goto label8002759C;
  v1 = at + 7936; // 0x1F00
  sw(fp + 0x0C, at);
  sw(fp + 0x18, v0);
  sw(fp + 0x24, v1);
  goto label800275F4;
label8002759C:
  temp = (int32_t)t4 > 0;
  t4 -= 256; // 0xFFFFFF00
  if (temp) goto label800275BC;
  v1 = at + 31; // 0x1F
  sw(fp + 0x0C, v1);
  v0 += 7936; // 0x1F00
  sw(fp + 0x18, v0);
  sw(fp + 0x24, at);
  goto label800275F4;
label800275BC:
  temp = (int32_t)t4 > 0;
  if (temp) goto label800275DC;
  at += 7967; // 0x1F1F
  sw(fp + 0x0C, at);
  v1 = v0 + 7905; // 0x1EE1
  sw(fp + 0x18, v1);
  sw(fp + 0x24, v0);
  goto label800275F4;
label800275DC:
  v1 = at + 7936; // 0x1F00
  sw(fp + 0x0C, v1);
  v0 -= 31; // 0xFFFFFFE1
  sw(fp + 0x18, v0);
  at += 7967; // 0x1F1F
  sw(fp + 0x24, at);
label800275F4:
  sw(fp + 0x00, a1);
  t3 += s4;
  at = lw(t3 + 0x00);
  sw(t3 + 0x00, fp);
  temp = at == 0;
  v0 = fp >> 16;
  if (temp) goto label8002761C;
  sh(at + 0x00, fp);
  sb(at + 0x02, v0);
  fp += 40; // 0x28
  goto label80026B74;
label8002761C:
  sw(t3 + 0x04, fp);
  fp += 40; // 0x28
  goto label80026B74;
label80027628:
  at = lw(fp + 0x14);
  v0 = lw(fp + 0x20);
  v1 = lw(fp + 0x10);
  a0 = lw(fp + 0x1C);
  sw(fp + 0x10, at);
  sw(fp + 0x18, v0);
  sw(fp + 0x0C, v1);
  sw(fp + 0x14, a0);
  at = 48; // 0x30
  sb(fp + 0x07, at);
  at = 0x06000000;
  sw(fp + 0x00, at);
  t3 += s4;
  at = lw(t3 + 0x00);
  sw(t3 + 0x00, fp);
  temp = at == 0;
  v0 = fp >> 16;
  if (temp) goto label8002767C;
  sh(at + 0x00, fp);
  sb(at + 0x02, v0);
  fp += 28; // 0x1C
  goto label80026B74;
label8002767C:
  sw(t3 + 0x04, fp);
  fp += 28; // 0x1C
  goto label80026B74;
label80027688:
  sw(gp + 0x00, 0);
  sw(sp + 0x00, 0);
  ra = 0x8006FCF4; // &0x0EA69B
  ra += 8192; // 0x2000
  sp = 0x800785A8;
  sp = lw(sp + 0x1C);
  gp = ordered_linked_list;
  gp = lw(gp + 0x00);
label800276B4:
  s3 = lw(ra + 0x00);
  ra += 4; // 0x04
  temp = s3 == 0;
  t7 = 0x1F800000;
  if (temp) goto label8002857C;
  temp = (int32_t)s3 < 0;
  at = s3 << 2;
  if (temp) goto label8002773C;
  v0 = lw(at + 0x08);
  a0 = cop2.RBK;
  a1 = cop2.GBK;
  a2 = cop2.BBK;
  v1 = lw(at + 0x0C);
  s7 = v0 >> 14;
  t8 = v0 & 0xFFFF;
  t8 = t8 << 2;
  s7 -= a0;
  t8 = a1 - t8;
  v0 = lw(at + 0x14);
  t9 = v1 >> 14;
  t9 = a2 - t9;
  v1 = v1 & 0xFFFF;
  s4 = at + 28; // 0x1C
  a0 = v0 >> 22;
  a0 = a0 & 0x3FC;
  s4 += a0;
  a0 = v0 << 2;
  a0 = a0 & 0x3FC;
  s5 = s4 + a0;
  s3 = lw(ra + 0x00);
  ra += 4; // 0x04
  a0 = v0 >> 5;
  a0 = a0 & 0x7F8;
  s6 = s5 + a0;
  v1 = v1 << 4;
  s6 -= v1;
label8002773C:
  s2 = s3 & 0x1;
  at = s3 & 0x2;
  s3 = s3 >> 2;
  temp = (int32_t)at > 0;
  s3 = s3 << 2;
  if (temp) goto label80027F28;
  t2 = lw(s3 + 0x00);
  at = t2 >> 22;
  at = at & 0x3FC;
  at += s4;
  at = lw(at + 0x00);
  a0 = t2 >> 14;
  a0 = a0 & 0x3FC;
  a0 += s4;
  a0 = lw(a0 + 0x00);
  a3 = t2 << 2;
  a3 = a3 & 0x3FC;
  a3 += s4;
  a3 = lw(a3 + 0x00);
  t2 = t2 >> 6;
  t2 = t2 & 0x3FC;
  t2 += s4;
  t2 = lw(t2 + 0x00);
  v1 = at << 2;
  v1 = v1 & 0xFFC;
  v0 = at >> 8;
  v0 = v0 & 0x1FFC;
  at = at >> 19;
  at = at & 0x1FFC;
  a2 = a0 << 2;
  a2 = a2 & 0xFFC;
  a1 = a0 >> 8;
  a1 = a1 & 0x1FFC;
  a0 = a0 >> 19;
  a0 = a0 & 0x1FFC;
  s0 = lw(s3 + 0x08);
  t1 = a3 << 2;
  t1 = t1 & 0xFFC;
  t0 = a3 >> 8;
  t0 = t0 & 0x1FFC;
  a3 = a3 >> 19;
  a3 = a3 & 0x1FFC;
  s1 = lw(s3 + 0x0C);
  t4 = t2 << 2;
  t4 = t4 & 0xFFC;
  t3 = t2 >> 8;
  t3 = t3 & 0x1FFC;
  t2 = t2 >> 19;
  t2 = t2 & 0x1FFC;
  at += s7;
  v0 = t8 - v0;
  v1 = t9 - v1;
  a0 += s7;
  a1 = t8 - a1;
  a2 = t9 - a2;
  a3 += s7;
  t0 = t8 - t0;
  t1 = t9 - t1;
  t2 += s7;
  t3 = t8 - t3;
  t4 = t9 - t4;
  sh(t7 + 0x00, at);
  sh(t7 + 0x02, v0);
  sh(t7 + 0x04, v1);
  sh(t7 + 0x20, a0);
  sh(t7 + 0x22, a1);
  sh(t7 + 0x24, a2);
  sh(t7 + 0x60, a3);
  sh(t7 + 0x62, t0);
  sh(t7 + 0x64, t1);
  sh(t7 + 0x80, t2);
  sh(t7 + 0x82, t3);
  sh(t7 + 0x84, t4);
  t5 = at + a0;
  t5 = (int32_t)t5 >> 1;
  sh(t7 + 0x10, t5);
  t5 = v0 + a1;
  t5 = (int32_t)t5 >> 1;
  sh(t7 + 0x12, t5);
  t5 = v1 + a2;
  t5 = (int32_t)t5 >> 1;
  sh(t7 + 0x14, t5);
  t5 = a3 + t2;
  t5 = (int32_t)t5 >> 1;
  sh(t7 + 0x70, t5);
  t5 = t0 + t3;
  t5 = (int32_t)t5 >> 1;
  sh(t7 + 0x72, t5);
  t5 = t1 + t4;
  t5 = (int32_t)t5 >> 1;
  sh(t7 + 0x74, t5);
  at += a3;
  at = (int32_t)at >> 1;
  sh(t7 + 0x30, at);
  v0 += t0;
  v0 = (int32_t)v0 >> 1;
  sh(t7 + 0x32, v0);
  v1 += t1;
  v1 = (int32_t)v1 >> 1;
  sh(t7 + 0x34, v1);
  a0 += t2;
  a0 = (int32_t)a0 >> 1;
  sh(t7 + 0x50, a0);
  a1 += t3;
  a1 = (int32_t)a1 >> 1;
  sh(t7 + 0x52, a1);
  a2 += t4;
  a2 = (int32_t)a2 >> 1;
  sh(t7 + 0x54, a2);
  t5 = at + a0;
  t5 = (int32_t)t5 >> 1;
  sh(t7 + 0x40, t5);
  t5 = v0 + a1;
  t5 = (int32_t)t5 >> 1;
  sh(t7 + 0x42, t5);
  t5 = v1 + a2;
  t5 = (int32_t)t5 >> 1;
  sh(t7 + 0x44, t5);
  at = lw(s3 + 0x04);
  v0 = s0 & 0x80;
  temp = v0 == 0;
  t0 = 0x3C000000;
  if (temp) goto label8002792C;
  v0 = 0x02000000;
  t0 += v0;
label8002792C:
  a0 = at >> 6;
  a0 = a0 & 0x3FC;
  a0 += s5;
  a0 = lw(a0 + 0x00);
  v1 = at << 2;
  v1 = v1 & 0x3FC;
  v1 += s5;
  v1 = lw(v1 + 0x00);
  v0 = at >> 14;
  v0 = v0 & 0x3FC;
  v0 += s5;
  v0 = lw(v0 + 0x00);
  at = at >> 22;
  at = at & 0x3FC;
  at += s5;
  at = lw(at + 0x00);
  a3 = 0xFFFEFEFF;
  a0 += t0;
  v1 += t0;
  v0 += t0;
  at += t0;
  sw(t7 + 0x88, a0);
  sw(t7 + 0x68, v1);
  sw(t7 + 0x28, v0);
  sw(t7 + 0x08, at);
  a0 = a0 & a3;
  v1 = v1 & a3;
  v0 = v0 & a3;
  at = at & a3;
  a1 = at + v0;
  a1 = a1 >> 1;
  sw(t7 + 0x18, a1);
  a1 = v1 + a0;
  a1 = a1 >> 1;
  sw(t7 + 0x78, a1);
  a1 = at + v1;
  a1 = a1 >> 1;
  sw(t7 + 0x38, a1);
  a1 = v0 + a0;
  a1 = a1 >> 1;
  sw(t7 + 0x58, a1);
  a1 = v0 + v1;
  a1 = a1 >> 1;
  sw(t7 + 0x48, a1);
  t0 = 0x1F800000;
  t1 = t0 + 160; // 0xA0
  sw(t1 - 0x10, 0); // 0xFFFFFFF0
  at = lh(t0 + 0x00);
  v0 = lh(t0 + 0x02);
  v1 = lh(t0 + 0x04);
  a0 = (int32_t)at >> 8;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = (int32_t)v0 >> 8;
  if (temp) goto label80027A40;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = (int32_t)v1 >> 8;
  if (temp) goto label80027A40;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = v1 << 20;
  if (temp) goto label80027A40;
  at = at << 4;
  v0 = v0 << 4;
  v0 += a0;
  a2 = 1; // 0x01
  goto label80027A4C;
label80027A40:
  v1 = v1 << 16;
  v0 += v1;
  a2 = 0;
label80027A4C:
  cop2.VZ0 = at;
  cop2.VXY0 = v0;
  t0 += 16; // 0x10
  t2 = 0x010000;
  t3 = 0x01000000;
  t4 = 0x02000000;
label80027A64:
  RTPS();
  a3 = a2;
  at = lh(t0 + 0x00);
  v0 = lh(t0 + 0x02);
  v1 = lh(t0 + 0x04);
  a0 = (int32_t)at >> 8;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = (int32_t)v0 >> 8;
  if (temp) goto label80027AC0;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = (int32_t)v1 >> 8;
  if (temp) goto label80027AC0;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = v1 << 20;
  if (temp) goto label80027AC0;
  at = at << 4;
  v0 = v0 << 4;
  v0 += a0;
  a2 = 1; // 0x01
  goto label80027ACC;
label80027AC0:
  v1 = v1 << 16;
  v0 += v1;
  a2 = 0;
label80027ACC:
  v1 = cop2.SXY2;
  a0 = cop2.SZ3;
  cop2.VZ0 = at;
  cop2.VXY0 = v0;
  temp = a3 == 0;
  t0 += 16; // 0x10
  if (temp) goto label80027AE8;
  a0 = a0 >> 4;
label80027AE8:
  v0 = v1 - t2;
  temp = (int32_t)v0 > 0;
  v0 = v1 - t3;
  if (temp) goto label80027AF8;
  a0 = a0 | 0x1000;
label80027AF8:
  temp = (int32_t)v0 < 0;
  v0 = v1 << 16;
  if (temp) goto label80027B04;
  a0 = a0 | 0x2000;
label80027B04:
  temp = (int32_t)v0 > 0;
  v0 -= t4;
  if (temp) goto label80027B10;
  a0 = a0 | 0x4000;
label80027B10:
  temp = (int32_t)v0 < 0;
  sw(t0 - 0x20, v1); // 0xFFFFFFE0
  if (temp) goto label80027B1C;
  a0 = a0 | 0x8000;
label80027B1C:
  temp = t0 != t1;
  sw(t0 - 0x1C, a0); // 0xFFFFFFE4
  if (temp) goto label80027A64;
  at = lw(t7 + 0x44);
  v1 = lw(t7 + 0x40);
  a0 = at & 0xF000;
  temp = (int32_t)a0 > 0;
  at = at & 0xFFF;
  if (temp) goto label80027BD0;
  at -= 1792; // 0xFFFFF900
  temp = (int32_t)at <= 0;
  v0 = at - 256; // 0xFFFFFF00
  if (temp) goto label80027BD0;
  v0 = -v0;
  a0 = (int32_t)v1 >> 16;
  v1 = v1 << 16;
  v1 = (int32_t)v1 >> 16;
  cop2.IR0 = v0;
  cop2.IR1 = v1;
  cop2.IR2 = a0;
  v0 = lw(t7 + 0x20);
  v1 = lw(t7 + 0x60);
  GPF(SF_OFF, LM_OFF);
  a0 = (int32_t)v0 >> 16;
  a1 = (int32_t)v1 >> 16;
  v0 = v0 << 16;
  v0 = (int32_t)v0 >> 16;
  v1 = v1 << 16;
  v1 = (int32_t)v1 >> 16;
  v0 += v1;
  a0 += a1;
  at = at >> 1;
  cop2.IR0 = at;
  cop2.IR1 = v0;
  cop2.IR2 = a0;
  at = cop2.MAC1;
  v0 = cop2.MAC2;
  GPF(SF_OFF, LM_OFF);
  v1 = cop2.MAC1;
  a0 = cop2.MAC2;
  at += v1;
  v0 += a0;
  at = (int32_t)at >> 8;
  v0 = (int32_t)v0 >> 8;
  v1 = at & 0xFFFF;
  a0 = v0 << 16;
  v1 += a0;
  sw(t7 + 0x40, v1);
label80027BD0:
  t5 = s3 - s6;
  t5 = t5 >> 4;
  at = 0x8006FCF4; // &0x0EA69B
  t5 += at;
  at = (int32_t)s0 >> 20;
  at += t5;
  at = lb(at + 0x00);
  v0 = s0 << 12;
  v0 = (int32_t)v0 >> 20;
  v0 += t5;
  t6 = s0 & 0x7F;
  t4 = 168; // 0xA8
  mult(t6, t4);
  v0 = lb(v0 + 0x00);
  at = at & 0x1;
  v1 = (int32_t)s1 >> 20;
  v1 += t5;
  t6=lo;
  t6 += sp;
  v1 = lb(v1 + 0x00);
  v0 = v0 & 0x1;
  v0 = v0 << 8;
  a0 = s1 << 12;
  a0 = (int32_t)a0 >> 20;
  a0 += t5;
  a0 = lb(a0 + 0x00);
  v1 = v1 & 0x1;
  v1 = v1 << 16;
  t4 = 0x8006CF98; // &0x000020
  at = at | v0;
  at = at | v1;
  a0 = a0 & 0x1;
  a0 = a0 << 24;
  at = at | a0;
label80027C60:
  temp = at == 0;
  v0 = at & 0xFF;
  if (temp) goto label80027D7C;
  at = at >> 8;
  temp = v0 == 0;
  t4 += 12; // 0x0C
  if (temp) goto label80027C60;
  a0 = lw(t4 - 0x0C); // 0xFFFFFFF4
  a1 = lw(t4 - 0x08); // 0xFFFFFFF8
  v0 = a0 >> 16;
  v0 += t7;
  v1 = a0 & 0xFFFF;
  v1 += t7;
  a0 = a1 >> 16;
  a0 += t7;
  a3 = lw(v0 + 0x04);
  t0 = lw(v1 + 0x04);
  t1 = lw(a0 + 0x04);
  a2 = lw(t4 - 0x04); // 0xFFFFFFFC
  t2 = a3 & t0;
  t2 = t2 & t1;
  t2 = t2 & 0xF000;
  temp = (int32_t)t2 > 0;
  a3 = a3 & 0xFFF;
  if (temp) goto label80027C60;
  t2 = lw(t6 + 0x00);
  t0 = t0 & 0xFFF;
  t1 = t1 & 0xFFF;
  t3 = a3 + t0;
  t3 += t1;
  t3 += t1;
  a3 = lw(v0 + 0x00);
  t0 = lw(v1 + 0x00);
  t1 = lw(a0 + 0x00);
  sw(fp + 0x08, a3);
  sw(fp + 0x14, t0);
  sw(fp + 0x20, t1);
  a3 = lw(v0 + 0x08);
  t0 = 0x08000000;
  a3 -= t0;
  t0 = lw(v1 + 0x08);
  t1 = lw(a0 + 0x08);
  sw(fp + 0x04, a3);
  sw(fp + 0x10, t0);
  sw(fp + 0x1C, t1);
  a3 = a1 & 0xFFFF;
  t0 = (int32_t)a2 >> 16;
  t1 = a2 & 0xFFFF;
  t3 = t3 >> 7;
  v0 = s1 & 0x38;
  v0 = v0 >> 1;
  t3 += v0;
  a2 = lw(t6 + 0x04);
  a3 += t2;
  t1 += t2;
  t0 += a2;
  sw(fp + 0x0C, a3);
  sw(fp + 0x18, t0);
  sw(fp + 0x24, t1);
  v1 = 0x09000000;
  sw(fp + 0x00, v1);
  t3 = t3 << 3;
  t3 += gp;
  v1 = lw(t3 + 0x00);
  sw(t3 + 0x00, fp);
  temp = v1 == 0;
  a0 = fp >> 16;
  if (temp) goto label80027D70;
  sh(v1 + 0x00, fp);
  sb(v1 + 0x02, a0);
  fp += 40; // 0x28
  goto label80027C60;
label80027D70:
  sw(t3 + 0x04, fp);
  fp += 40; // 0x28
  goto label80027C60;
label80027D7C:
  at = 0x8006CF48; // &0x1F800000
  v0 = at + 16; // 0x10
label80027D88:
  temp = at == v0;
  t6 += 8; // 0x08
  if (temp) goto label800276B4;
  v1 = lw(at + 0x00);
  at += 4; // 0x04
  a0 = lw(v1 + 0x04);
  a1 = lw(v1 + 0x14);
  a2 = lw(v1 + 0x34);
  a3 = lw(v1 + 0x44);
  t1 = lw(t6 + 0x00);
  t0 = a0 & a1;
  t0 = t0 & a2;
  t0 = t0 & a3;
  t0 = t0 & 0xF000;
  temp = (int32_t)t0 > 0;
  a0 = a0 & 0xFFF;
  if (temp) goto label80027D88;
  t2 = lw(t6 + 0x04);
  a1 = a1 & 0xFFF;
  a2 = a2 & 0xFFF;
  a3 = a3 & 0xFFF;
  t0 = a0 + a1;
  t0 += a2;
  t0 += a3;
  a0 = lw(v1 + 0x00);
  a1 = lw(v1 + 0x10);
  a2 = lw(v1 + 0x30);
  temp = s2 == 0;
  a3 = lw(v1 + 0x40);
  if (temp) goto label80027E58;
  cop2.SXY0 = a0;
  cop2.SXY1 = a1;
  cop2.SXY2 = a2;
  t3 = s1 & 0x4;
  NCLIP();
  temp = (int32_t)t3 > 0;
  t3 = s1 & 0x2;
  if (temp) goto label80027E58;
  t4 = cop2.MAC0;
  temp = t3 == 0;
  if (temp) goto label80027E24;
  t4 = -t4;
label80027E24:
  temp = (int32_t)t4 >= 0;
  if (temp) goto label80027E58;
  cop2.SXY0 = a3;
  cop2.SXY1 = a2;
  cop2.SXY2 = a1;
  NCLIP();
  t4 = cop2.MAC0;
  temp = t3 == 0;
  if (temp) goto label80027E54;
  t4 = -t4;
label80027E54:
  temp = (int32_t)t4 < 0;
  if (temp) {
    sw(fp + 0x08, a0);
    goto label80027D88;
  }
label80027E58:
  sw(fp + 0x08, a0);
  sw(fp + 0x14, a1);
  sw(fp + 0x20, a2);
  sw(fp + 0x2C, a3);
  a0 = lw(v1 + 0x08);
  a1 = lw(v1 + 0x18);
  a2 = lw(v1 + 0x38);
  a3 = lw(v1 + 0x48);
  sw(fp + 0x04, a0);
  sw(fp + 0x10, a1);
  sw(fp + 0x1C, a2);
  sw(fp + 0x28, a3);
  t0 = t0 >> 7;
  a0 = s1 & 0x38;
  a0 = a0 >> 1;
  t0 += a0;
  t3 = t1 + 7936; // 0x1F00
  v1 = t2 >> 25;
  temp = v1 == 0;
  t4 = t1 + 7967; // 0x1F1F
  if (temp) goto label80027EDC;
  a0 = 0x8006D058; // &0x000000
  v1 += a0;
  a0 = lw(v1 + 0x04);
  v1 = lw(v1 + 0x00);
  a1 = a0 & 0xFFFF;
  t3 = t1 + a1;
  a1 = a0 >> 16;
  t4 = t1 + a1;
  a1 = v1 & 0xFFFF;
  t1 += a1;
  a1 = (int32_t)v1 >> 16;
  t2 += a1;
label80027EDC:
  sw(fp + 0x0C, t1);
  sw(fp + 0x18, t2);
  sw(fp + 0x24, t3);
  sw(fp + 0x30, t4);
  v1 = 0x0C000000;
  sw(fp + 0x00, v1);
  t0 = t0 << 3;
  t0 += gp;
  v1 = lw(t0 + 0x00);
  sw(t0 + 0x00, fp);
  temp = v1 == 0;
  a0 = fp >> 16;
  if (temp) goto label80027F1C;
  sh(v1 + 0x00, fp);
  sb(v1 + 0x02, a0);
  fp += 52; // 0x34
  goto label80027D88;
label80027F1C:
  sw(t0 + 0x04, fp);
  fp += 52; // 0x34
  goto label80027D88;
label80027F28:
  t2 = lw(s3 + 0x00);
  at = t2 >> 22;
  at = at & 0x3FC;
  at += s4;
  at = lw(at + 0x00);
  a0 = t2 >> 14;
  a0 = a0 & 0x3FC;
  a0 += s4;
  a0 = lw(a0 + 0x00);
  a3 = t2 << 2;
  a3 = a3 & 0x3FC;
  a3 += s4;
  a3 = lw(a3 + 0x00);
  v1 = at << 2;
  v1 = v1 & 0xFFC;
  v0 = at >> 8;
  v0 = v0 & 0x1FFC;
  at = at >> 19;
  at = at & 0x1FFC;
  a2 = a0 << 2;
  a2 = a2 & 0xFFC;
  a1 = a0 >> 8;
  a1 = a1 & 0x1FFC;
  a0 = a0 >> 19;
  a0 = a0 & 0x1FFC;
  s0 = lw(s3 + 0x08);
  t1 = a3 << 2;
  t1 = t1 & 0xFFC;
  t0 = a3 >> 8;
  t0 = t0 & 0x1FFC;
  a3 = a3 >> 19;
  a3 = a3 & 0x1FFC;
  s1 = lw(s3 + 0x0C);
  at += s7;
  v0 = t8 - v0;
  v1 = t9 - v1;
  a0 += s7;
  a1 = t8 - a1;
  a2 = t9 - a2;
  a3 += s7;
  t0 = t8 - t0;
  t1 = t9 - t1;
  sh(t7 + 0x00, at);
  sh(t7 + 0x02, v0);
  sh(t7 + 0x04, v1);
  sh(t7 + 0x20, a0);
  sh(t7 + 0x22, a1);
  sh(t7 + 0x24, a2);
  sh(t7 + 0x50, a3);
  sh(t7 + 0x52, t0);
  sh(t7 + 0x54, t1);
  t5 = at + a0;
  t5 = (int32_t)t5 >> 1;
  sh(t7 + 0x10, t5);
  t5 = v0 + a1;
  t5 = (int32_t)t5 >> 1;
  sh(t7 + 0x12, t5);
  t5 = v1 + a2;
  t5 = (int32_t)t5 >> 1;
  sh(t7 + 0x14, t5);
  at += a3;
  at = (int32_t)at >> 1;
  sh(t7 + 0x30, at);
  v0 += t0;
  v0 = (int32_t)v0 >> 1;
  sh(t7 + 0x32, v0);
  v1 += t1;
  v1 = (int32_t)v1 >> 1;
  sh(t7 + 0x34, v1);
  a0 += a3;
  a0 = (int32_t)a0 >> 1;
  sh(t7 + 0x40, a0);
  a1 += t0;
  a1 = (int32_t)a1 >> 1;
  sh(t7 + 0x42, a1);
  a2 += t1;
  a2 = (int32_t)a2 >> 1;
  sh(t7 + 0x44, a2);
  at = lw(s3 + 0x04);
  v0 = s0 & 0x80;
  temp = v0 == 0;
  t0 = 0x34000000;
  if (temp) goto label8002807C;
  v0 = 0x02000000;
  t0 += v0;
label8002807C:
  v1 = at << 2;
  v1 = v1 & 0x3FC;
  v1 += s5;
  v1 = lw(v1 + 0x00);
  v0 = at >> 14;
  v0 = v0 & 0x3FC;
  v0 += s5;
  v0 = lw(v0 + 0x00);
  at = at >> 22;
  at = at & 0x3FC;
  at += s5;
  at = lw(at + 0x00);
  a3 = 0xFFFEFEFF;
  v1 += t0;
  v0 += t0;
  at += t0;
  sw(t7 + 0x58, v1);
  sw(t7 + 0x28, v0);
  sw(t7 + 0x08, at);
  v1 = v1 & a3;
  v0 = v0 & a3;
  at = at & a3;
  a1 = at + v0;
  a1 = a1 >> 1;
  sw(t7 + 0x18, a1);
  a1 = at + v1;
  a1 = a1 >> 1;
  sw(t7 + 0x38, a1);
  a1 = v0 + v1;
  a1 = a1 >> 1;
  sw(t7 + 0x48, a1);
  t0 = 0x1F800000;
  t1 = t0 + 112; // 0x70
  sw(t1 - 0x10, 0); // 0xFFFFFFF0
  at = lh(t0 + 0x00);
  v0 = lh(t0 + 0x02);
  v1 = lh(t0 + 0x04);
  a0 = (int32_t)at >> 8;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = (int32_t)v0 >> 8;
  if (temp) goto label8002815C;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = (int32_t)v1 >> 8;
  if (temp) goto label8002815C;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = v1 << 20;
  if (temp) goto label8002815C;
  at = at << 4;
  v0 = v0 << 4;
  v0 += a0;
  a2 = 1; // 0x01
  goto label80028168;
label8002815C:
  v1 = v1 << 16;
  v0 += v1;
  a2 = 0;
label80028168:
  cop2.VZ0 = at;
  cop2.VXY0 = v0;
  t0 += 16; // 0x10
  t2 = 0x010000;
  t3 = 0x01000000;
  t4 = 0x02000000;
label80028180:
  RTPS();
  a3 = a2;
  at = lh(t0 + 0x00);
  v0 = lh(t0 + 0x02);
  v1 = lh(t0 + 0x04);
  a0 = (int32_t)at >> 8;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = (int32_t)v0 >> 8;
  if (temp) goto label800281DC;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = (int32_t)v1 >> 8;
  if (temp) goto label800281DC;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = v1 << 20;
  if (temp) goto label800281DC;
  at = at << 4;
  v0 = v0 << 4;
  v0 += a0;
  a2 = 1; // 0x01
  goto label800281E8;
label800281DC:
  v1 = v1 << 16;
  v0 += v1;
  a2 = 0;
label800281E8:
  v1 = cop2.SXY2;
  a0 = cop2.SZ3;
  cop2.VZ0 = at;
  cop2.VXY0 = v0;
  temp = a3 == 0;
  t0 += 16; // 0x10
  if (temp) goto label80028204;
  a0 = a0 >> 4;
label80028204:
  v0 = v1 - t2;
  temp = (int32_t)v0 > 0;
  v0 = v1 - t3;
  if (temp) goto label80028214;
  a0 = a0 | 0x1000;
label80028214:
  temp = (int32_t)v0 < 0;
  v0 = v1 << 16;
  if (temp) goto label80028220;
  a0 = a0 | 0x2000;
label80028220:
  temp = (int32_t)v0 > 0;
  v0 -= t4;
  if (temp) goto label8002822C;
  a0 = a0 | 0x4000;
label8002822C:
  temp = (int32_t)v0 < 0;
  sw(t0 - 0x20, v1); // 0xFFFFFFE0
  if (temp) goto label80028238;
  a0 = a0 | 0x8000;
label80028238:
  temp = t0 != t1;
  sw(t0 - 0x1C, a0); // 0xFFFFFFE4
  if (temp) goto label80028180;
  t5 = s3 - s6;
  t5 = t5 >> 4;
  at = 0x8006FCF4; // &0x0EA69B
  t5 += at;
  at = (int32_t)s0 >> 20;
  at += t5;
  at = lb(at + 0x00);
  v0 = s1 << 12;
  v0 = (int32_t)v0 >> 20;
  v0 += t5;
  t6 = s0 & 0x7F;
  t4 = 168; // 0xA8
  mult(t6, t4);
  v0 = lb(v0 + 0x00);
  at = at & 0x1;
  v1 = (int32_t)s1 >> 20;
  v1 += t5;
  t6=lo;
  t6 += sp;
  v1 = lb(v1 + 0x00);
  t4 = 0x8006D138; // &0x000020
  v0 = v0 & 0x1;
  v0 = v0 << 8;
  v1 = v1 & 0x1;
  v1 = v1 << 16;
  at = at | v0;
  at = at | v1;
  v0 = s0 & 0x300;
  v0 = v0 >> 3;
  v1 = v0 >> 3;
  t4 += v0;
  t4 += v1;
label800282C8:
  temp = at == 0;
  v0 = at & 0xFF;
  if (temp) goto label800283DC;
  at = at >> 8;
  temp = v0 == 0;
  t4 += 12; // 0x0C
  if (temp) goto label800282C8;
  a0 = lw(t4 - 0x0C); // 0xFFFFFFF4
  a1 = lw(t4 - 0x08); // 0xFFFFFFF8
  v0 = a0 >> 16;
  v0 += t7;
  v1 = a0 & 0xFFFF;
  v1 += t7;
  a0 = a1 >> 16;
  a0 += t7;
  a3 = lw(v0 + 0x04);
  t0 = lw(v1 + 0x04);
  t1 = lw(a0 + 0x04);
  a2 = lw(t4 - 0x04); // 0xFFFFFFFC
  t2 = a3 & t0;
  t2 = t2 & t1;
  t2 = t2 & 0xF000;
  temp = (int32_t)t2 > 0;
  a3 = a3 & 0xFFF;
  if (temp) goto label800282C8;
  t2 = lw(t6 + 0x00);
  t0 = t0 & 0xFFF;
  t1 = t1 & 0xFFF;
  t3 = a3 + t0;
  t3 += t1;
  t3 += t1;
  a3 = lw(v0 + 0x00);
  t0 = lw(v1 + 0x00);
  t1 = lw(a0 + 0x00);
  sw(fp + 0x08, a3);
  sw(fp + 0x14, t0);
  sw(fp + 0x20, t1);
  a3 = lw(v0 + 0x08);
  t0 = lw(v1 + 0x08);
  t1 = lw(a0 + 0x08);
  sw(fp + 0x04, a3);
  sw(fp + 0x10, t0);
  sw(fp + 0x1C, t1);
  a3 = a1 & 0xFFFF;
  t0 = (int32_t)a2 >> 16;
  t1 = a2 & 0xFFFF;
  t3 = t3 >> 7;
  v0 = s1 & 0x38;
  v0 = v0 >> 1;
  t3 += v0;
  a2 = lw(t6 + 0x04);
  a3 += t2;
  t1 += t2;
  t0 += a2;
  sw(fp + 0x0C, a3);
  sw(fp + 0x18, t0);
  sw(fp + 0x24, t1);
  v1 = 0x09000000;
  sw(fp + 0x00, v1);
  t3 = t3 << 3;
  t3 += gp;
  v1 = lw(t3 + 0x00);
  sw(t3 + 0x00, fp);
  temp = v1 == 0;
  a0 = fp >> 16;
  if (temp) goto label800283D0;
  sh(v1 + 0x00, fp);
  sb(v1 + 0x02, a0);
  fp += 40; // 0x28
  goto label800282C8;
label800283D0:
  sw(t3 + 0x04, fp);
  fp += 40; // 0x28
  goto label800282C8;
label800283DC:
  t2 = 0x8006D0E8; // &0x010300
  t3 = t2 + 16; // 0x10
  t4 = 0x8006D378; // &0x10180800
  t5 = 0x8006D3C8; // &0x000000
  t6 += 8; // 0x08
label800283FC:
  at = lw(t2 + 0x00);
  temp = t2 == t3;
  t2 += 4; // 0x04
  if (temp) goto label800276B4;
  v0 = s0 >> 8;
  v0 = v0 & 0x3;
  v1 = at & 0xC;
  v1 += v0;
  v1 += t4;
  v1 = lb(v1 + 0x00);
  a3 = at >> 20;
  a3 = a3 & 0xFF0;
  t0 = at >> 12;
  t0 = t0 & 0xFF0;
  t1 = at >> 4;
  t1 = t1 & 0xFF0;
  v1 += t6;
  a0 = lw(v1 + 0x04);
  a3 += t7;
  t0 += t7;
  t1 += t7;
  v0 += at;
  v0 = v0 & 0x3;
  v0 = v0 << 7;
  v1 = lw(v1 + 0x00);
  a1 = a0 >> 25;
  a1 = a1 & 0x78;
  v0 += a1;
  v0 += t5;
  a1 = lw(v0 + 0x00);
  v0 = lh(v0 + 0x04);
  a2 = a1 & 0xFFFF;
  a1 = (int32_t)a1 >> 16;
  a2 += v1;
  a1 += a0;
  a0 = v0 + v1;
  sw(fp + 0x0C, a2);
  sw(fp + 0x18, a1);
  sw(fp + 0x24, a0);
  at = lw(a3 + 0x04);
  v0 = lw(t0 + 0x04);
  v1 = lw(t1 + 0x04);
  a0 = at & v0;
  a0 = a0 & v1;
  a0 = a0 & 0xF000;
  temp = (int32_t)a0 > 0;
  at = at & 0xFFF;
  if (temp) goto label800283FC;
  v0 = v0 & 0xFFF;
  v1 = v1 & 0xFFF;
  a0 = at + v0;
  a0 += v1;
  a0 += v1;
  at = lw(a3 + 0x00);
  v0 = lw(t0 + 0x00);
  temp = s2 == 0;
  v1 = lw(t1 + 0x00);
  if (temp) goto label8002850C;
  cop2.SXY0 = at;
  cop2.SXY1 = v0;
  cop2.SXY2 = v1;
  a1 = s1 & 0x4;
  NCLIP();
  temp = (int32_t)a1 > 0;
  a1 = s1 & 0x2;
  if (temp) goto label8002850C;
  a2 = cop2.MAC0;
  temp = a1 == 0;
  if (temp) goto label80028508;
  a2 = -a2;
label80028508:
  temp = (int32_t)a2 < 0;
  if (temp) {
    sw(fp + 0x08, at);
    goto label800283FC;
  }
label8002850C:
  sw(fp + 0x08, at);
  sw(fp + 0x14, v0);
  sw(fp + 0x20, v1);
  a0 = a0 >> 7;
  a1 = s1 & 0x38;
  a1 = a1 >> 1;
  a0 += a1;
  at = lw(a3 + 0x08);
  v0 = lw(t0 + 0x08);
  v1 = lw(t1 + 0x08);
  sw(fp + 0x04, at);
  sw(fp + 0x10, v0);
  sw(fp + 0x1C, v1);
  v0 = 0x09000000;
  sw(fp + 0x00, v0);
  a0 = a0 << 3;
  a0 += gp;
  v0 = lw(a0 + 0x00);
  sw(a0 + 0x00, fp);
  temp = v0 == 0;
  v1 = fp >> 16;
  if (temp) goto label80028570;
  sh(v0 + 0x00, fp);
  sb(v0 + 0x02, v1);
  fp += 40; // 0x28
  goto label800283FC;
label80028570:
  sw(a0 + 0x04, fp);
  fp += 40; // 0x28
  goto label800283FC;
label8002857C:
  at = 0x8006FCF4; // &0x0EA69B
  ra = at + 7680; // 0x1E00
  at += 8192; // 0x2000
  sw(t7 + 0x03FC, at);
label80028590:
  s3 = lw(ra + 0x00);
  ra += 4; // 0x04
  temp = s3 == 0;
  t7 = 0x1F800000;
  if (temp) goto label800294F4;
  temp = (int32_t)s3 < 0;
  at = s3 << 2;
  if (temp) goto label80028618;
  v0 = lw(at + 0x08);
  a0 = cop2.RBK;
  a1 = cop2.GBK;
  a2 = cop2.BBK;
  v1 = lw(at + 0x0C);
  s7 = v0 >> 14;
  t8 = v0 & 0xFFFF;
  t8 = t8 << 2;
  s7 -= a0;
  t8 = a1 - t8;
  v0 = lw(at + 0x14);
  t9 = v1 >> 14;
  t9 = a2 - t9;
  v1 = v1 & 0xFFFF;
  s4 = at + 28; // 0x1C
  a0 = v0 >> 22;
  a0 = a0 & 0x3FC;
  s4 += a0;
  a0 = v0 << 2;
  a0 = a0 & 0x3FC;
  s5 = s4 + a0;
  s3 = lw(ra + 0x00);
  ra += 4; // 0x04
  a0 = v0 >> 5;
  a0 = a0 & 0x7F8;
  s6 = s5 + a0;
  v1 = v1 << 4;
  s6 -= v1;
label80028618:
  at = s3 & 0x2;
  temp = (int32_t)at > 0;
  s2 = s3 & 0x1;
  if (temp) goto label80028590;
  s3 = s3 >> 2;
  s3 = s3 << 2;
  a0 = lw(s3 + 0x00);
  at = a0 >> 22;
  at = at & 0x3FC;
  at += s4;
  at = lw(at + 0x00);
  v0 = a0 >> 14;
  v0 = v0 & 0x3FC;
  v0 += s4;
  v0 = lw(v0 + 0x00);
  v1 = a0 << 2;
  v1 = v1 & 0x3FC;
  v1 += s4;
  v1 = lw(v1 + 0x00);
  a0 = a0 >> 6;
  a0 = a0 & 0x3FC;
  a0 += s4;
  a0 = lw(a0 + 0x00);
  s0 = lw(s3 + 0x08);
  a1 = at >> 19;
  a1 = a1 & 0x1FFC;
  a1 += s7;
  a2 = v0 >> 19;
  a2 = a2 & 0x1FFC;
  a2 += s7;
  s1 = lw(s3 + 0x0C);
  a3 = v1 >> 19;
  a3 = a3 & 0x1FFC;
  a3 += s7;
  t0 = a0 >> 19;
  t0 = t0 & 0x1FFC;
  t0 += s7;
  sh(t7 + 0x00, a1);
  sh(t7 + 0x40, a2);
  sh(t7 + 0x0140, a3);
  sh(t7 + 0x0180, t0);
  t1 = a1 + a2;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x20, t1);
  t2 = a1 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x10, t2);
  t2 = a2 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x30, t2);
  t1 = a3 + t0;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x0160, t1);
  t2 = a3 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x0150, t2);
  t2 = t0 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x0170, t2);
  t1 = a1 + a3;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0xA0, t1);
  t2 = a2 + t0;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0xE0, t2);
  t3 = t1 + t2;
  t3 = (int32_t)t3 >> 1;
  sh(t7 + 0xC0, t3);
  t4 = t1 + t3;
  t4 = (int32_t)t4 >> 1;
  sh(t7 + 0xB0, t4);
  t4 = t2 + t3;
  t4 = (int32_t)t4 >> 1;
  sh(t7 + 0xD0, t4);
  a1 += t1;
  a1 = (int32_t)a1 >> 1;
  sh(t7 + 0x50, a1);
  a2 += t2;
  a2 = (int32_t)a2 >> 1;
  sh(t7 + 0x90, a2);
  a3 += t1;
  a3 = (int32_t)a3 >> 1;
  sh(t7 + 0xF0, a3);
  t0 += t2;
  t0 = (int32_t)t0 >> 1;
  sh(t7 + 0x0130, t0);
  t1 = a1 + a2;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x70, t1);
  t2 = a1 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x60, t2);
  t2 = a2 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x80, t2);
  t1 = a3 + t0;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x0110, t1);
  t2 = a3 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x0100, t2);
  t2 = t0 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x0120, t2);
  a1 = at >> 8;
  a1 = a1 & 0x1FFC;
  a1 = t8 - a1;
  sh(t7 + 0x02, a1);
  a2 = v0 >> 8;
  a2 = a2 & 0x1FFC;
  a2 = t8 - a2;
  sh(t7 + 0x42, a2);
  a3 = v1 >> 8;
  a3 = a3 & 0x1FFC;
  a3 = t8 - a3;
  sh(t7 + 0x0142, a3);
  t0 = a0 >> 8;
  t0 = t0 & 0x1FFC;
  t0 = t8 - t0;
  sh(t7 + 0x0182, t0);
  t1 = a1 + a2;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x22, t1);
  t2 = a1 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x12, t2);
  t2 = a2 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x32, t2);
  t1 = a3 + t0;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x0162, t1);
  t2 = a3 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x0152, t2);
  t2 = t0 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x0172, t2);
  t1 = a1 + a3;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0xA2, t1);
  t2 = a2 + t0;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0xE2, t2);
  t3 = t1 + t2;
  t3 = (int32_t)t3 >> 1;
  sh(t7 + 0xC2, t3);
  t4 = t1 + t3;
  t4 = (int32_t)t4 >> 1;
  sh(t7 + 0xB2, t4);
  t4 = t2 + t3;
  t4 = (int32_t)t4 >> 1;
  sh(t7 + 0xD2, t4);
  a1 += t1;
  a1 = (int32_t)a1 >> 1;
  sh(t7 + 0x52, a1);
  a2 += t2;
  a2 = (int32_t)a2 >> 1;
  sh(t7 + 0x92, a2);
  a3 += t1;
  a3 = (int32_t)a3 >> 1;
  sh(t7 + 0xF2, a3);
  t0 += t2;
  t0 = (int32_t)t0 >> 1;
  sh(t7 + 0x0132, t0);
  t1 = a1 + a2;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x72, t1);
  t2 = a1 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x62, t2);
  t2 = a2 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x82, t2);
  t1 = a3 + t0;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x0112, t1);
  t2 = a3 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x0102, t2);
  t2 = t0 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x0122, t2);
  a1 = at << 2;
  a1 = a1 & 0xFFC;
  a1 = t9 - a1;
  sh(t7 + 0x04, a1);
  a2 = v0 << 2;
  a2 = a2 & 0xFFC;
  a2 = t9 - a2;
  sh(t7 + 0x44, a2);
  a3 = v1 << 2;
  a3 = a3 & 0xFFC;
  a3 = t9 - a3;
  sh(t7 + 0x0144, a3);
  t0 = a0 << 2;
  t0 = t0 & 0xFFC;
  t0 = t9 - t0;
  sh(t7 + 0x0184, t0);
  t1 = a1 + a2;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x24, t1);
  t2 = a1 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x14, t2);
  t2 = a2 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x34, t2);
  t1 = a3 + t0;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x0164, t1);
  t2 = a3 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x0154, t2);
  t2 = t0 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x0174, t2);
  t1 = a1 + a3;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0xA4, t1);
  t2 = a2 + t0;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0xE4, t2);
  t3 = t1 + t2;
  t3 = (int32_t)t3 >> 1;
  sh(t7 + 0xC4, t3);
  t4 = t1 + t3;
  t4 = (int32_t)t4 >> 1;
  sh(t7 + 0xB4, t4);
  t4 = t2 + t3;
  t4 = (int32_t)t4 >> 1;
  sh(t7 + 0xD4, t4);
  a1 += t1;
  a1 = (int32_t)a1 >> 1;
  sh(t7 + 0x54, a1);
  a2 += t2;
  a2 = (int32_t)a2 >> 1;
  sh(t7 + 0x94, a2);
  a3 += t1;
  a3 = (int32_t)a3 >> 1;
  sh(t7 + 0xF4, a3);
  t0 += t2;
  t0 = (int32_t)t0 >> 1;
  sh(t7 + 0x0134, t0);
  t1 = a1 + a2;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x74, t1);
  t2 = a1 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x64, t2);
  t2 = a2 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x84, t2);
  t1 = a3 + t0;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x0114, t1);
  t2 = a3 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x0104, t2);
  t2 = t0 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x0124, t2);
  at = lw(s3 + 0x04);
  v0 = s0 & 0x80;
  temp = v0 == 0;
  a1 = 0x3C000000;
  if (temp) goto label80028A48;
  v0 = 0x02000000;
  a1 += v0;
label80028A48:
  a0 = at >> 6;
  a0 = a0 & 0x3FC;
  a0 += s5;
  a0 = lw(a0 + 0x00);
  v1 = at << 2;
  v1 = v1 & 0x3FC;
  v1 += s5;
  v1 = lw(v1 + 0x00);
  v0 = at >> 14;
  v0 = v0 & 0x3FC;
  v0 += s5;
  v0 = lw(v0 + 0x00);
  at = at >> 22;
  at = at & 0x3FC;
  at += s5;
  at = lw(at + 0x00);
  a0 += a1;
  v1 += a1;
  v0 += a1;
  at += a1;
  t0 = 0xFFFEFEFF;
  sw(t7 + 0x0188, a0);
  sw(t7 + 0x0148, v1);
  sw(t7 + 0x48, v0);
  sw(t7 + 0x08, at);
  a0 = a0 & t0;
  v1 = v1 & t0;
  v0 = v0 & t0;
  at = at & t0;
  a1 = v0 + v1;
  a1 = a1 >> 1;
  sw(t7 + 0xC8, a1);
  a1 = a1 & t0;
  a2 = at + v0;
  a2 = a2 >> 1;
  sw(t7 + 0x28, a2);
  a2 = a2 & t0;
  a3 = at + a2;
  a3 = a3 >> 1;
  sw(t7 + 0x18, a3);
  a3 = v0 + a2;
  a3 = a3 >> 1;
  sw(t7 + 0x38, a3);
  a3 = a1 + a2;
  a3 = a3 >> 1;
  sw(t7 + 0x78, a3);
  a2 = at + v1;
  a2 = a2 >> 1;
  sw(t7 + 0xA8, a2);
  a2 = a2 & t0;
  a3 = at + a2;
  a3 = a3 >> 1;
  sw(t7 + 0x58, a3);
  a3 = v1 + a2;
  a3 = a3 >> 1;
  sw(t7 + 0xF8, a3);
  a3 = a1 + a2;
  a3 = a3 >> 1;
  sw(t7 + 0xB8, a3);
  a2 = v0 + a0;
  a2 = a2 >> 1;
  sw(t7 + 0xE8, a2);
  a2 = a2 & t0;
  a3 = v0 + a2;
  a3 = a3 >> 1;
  sw(t7 + 0x98, a3);
  a3 = a0 + a2;
  a3 = a3 >> 1;
  sw(t7 + 0x0138, a3);
  a3 = a1 + a2;
  a3 = a3 >> 1;
  sw(t7 + 0xD8, a3);
  a2 = v1 + a0;
  a2 = a2 >> 1;
  sw(t7 + 0x0168, a2);
  a2 = a2 & t0;
  a3 = v1 + a2;
  a3 = a3 >> 1;
  sw(t7 + 0x0158, a3);
  a3 = a0 + a2;
  a3 = a3 >> 1;
  sw(t7 + 0x0178, a3);
  a3 = a1 + a2;
  a3 = a3 >> 1;
  sw(t7 + 0x0118, a3);
  at += a1;
  at = at >> 1;
  sw(t7 + 0x68, at);
  v0 += a1;
  v0 = v0 >> 1;
  sw(t7 + 0x88, v0);
  v1 += a1;
  v1 = v1 >> 1;
  sw(t7 + 0x0108, v1);
  a0 += a1;
  a0 = a0 >> 1;
  sw(t7 + 0x0128, a0);
  t0 = 0x1F800000;
  t1 = t0 + 416; // 0x01A0
  sw(t1 - 0x10, 0); // 0xFFFFFFF0
  at = lh(t0 + 0x00);
  v0 = lh(t0 + 0x02);
  v1 = lh(t0 + 0x04);
  a0 = (int32_t)at >> 8;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = (int32_t)v0 >> 8;
  if (temp) goto label80028C30;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = (int32_t)v1 >> 8;
  if (temp) goto label80028C30;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = v1 << 20;
  if (temp) goto label80028C30;
  at = at << 4;
  v0 = v0 << 4;
  v0 += a0;
  a2 = 1; // 0x01
  goto label80028C3C;
label80028C30:
  v1 = v1 << 16;
  v0 += v1;
  a2 = 0;
label80028C3C:
  cop2.VZ0 = at;
  cop2.VXY0 = v0;
  t0 += 16; // 0x10
  t2 = 0x010000;
  t3 = 0x01000000;
  t4 = 0x02000000;
label80028C54:
  RTPS();
  a3 = a2;
  at = lh(t0 + 0x00);
  v0 = lh(t0 + 0x02);
  v1 = lh(t0 + 0x04);
  a0 = (int32_t)at >> 8;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = (int32_t)v0 >> 8;
  if (temp) goto label80028CB0;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = (int32_t)v1 >> 8;
  if (temp) goto label80028CB0;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = v1 << 20;
  if (temp) goto label80028CB0;
  at = at << 4;
  v0 = v0 << 4;
  v0 += a0;
  a2 = 1; // 0x01
  goto label80028CBC;
label80028CB0:
  v1 = v1 << 16;
  v0 += v1;
  a2 = 0;
label80028CBC:
  v1 = cop2.SXY2;
  a0 = cop2.SZ3;
  cop2.VZ0 = at;
  cop2.VXY0 = v0;
  temp = a3 == 0;
  t0 += 16; // 0x10
  if (temp) goto label80028CD8;
  a0 = a0 >> 4;
label80028CD8:
  temp = (int32_t)a0 > 0;
  v0 = v1 - t2;
  if (temp) goto label80028CE4;
  a0 = a0 | 0xF000;
label80028CE4:
  temp = (int32_t)v0 > 0;
  v0 = v1 - t3;
  if (temp) goto label80028CF0;
  a0 = a0 | 0x1000;
label80028CF0:
  temp = (int32_t)v0 < 0;
  v0 = v1 << 16;
  if (temp) goto label80028CFC;
  a0 = a0 | 0x2000;
label80028CFC:
  temp = (int32_t)v0 > 0;
  v0 -= t4;
  if (temp) goto label80028D08;
  a0 = a0 | 0x4000;
label80028D08:
  temp = (int32_t)v0 < 0;
  sw(t0 - 0x20, v1); // 0xFFFFFFE0
  if (temp) goto label80028D14;
  a0 = a0 | 0x8000;
label80028D14:
  temp = t0 != t1;
  sw(t0 - 0x1C, a0); // 0xFFFFFFE4
  if (temp) goto label80028C54;
  at = lw(t7 + 0x04);
  v0 = lw(t7 + 0x44);
  v1 = lw(t7 + 0x0144);
  a0 = lw(t7 + 0x0184);
  at = at & 0xFFF;
  at -= 64; // 0xFFFFFFC0
  temp = (int32_t)at <= 0;
  v0 = v0 & 0xFFF;
  if (temp) goto label80028E48;
  v0 -= 64; // 0xFFFFFFC0
  temp = (int32_t)v0 <= 0;
  v1 = v1 & 0xFFF;
  if (temp) goto label80028E48;
  v1 -= 64; // 0xFFFFFFC0
  temp = (int32_t)v1 <= 0;
  a0 = a0 & 0xFFF;
  if (temp) goto label80028E48;
  a0 -= 64; // 0xFFFFFFC0
  temp = (int32_t)a0 <= 0;
  a1 = v0 - at;
  if (temp) goto label80028E48;
  temp = (int32_t)a1 >= 0;
  if (temp) goto label80028D6C;
  at = v0;
label80028D6C:
  a1 = v1 - at;
  temp = (int32_t)a1 >= 0;
  if (temp) goto label80028D7C;
  at = v1;
label80028D7C:
  a1 = a0 - at;
  temp = (int32_t)a1 >= 0;
  if (temp) goto label80028D8C;
  at = a0;
label80028D8C:
  v0 = at - 256; // 0xFFFFFF00
  v0 = -v0;
  at = at >> 1;
  v1 = 0x8006D0D8; // &0x1F800060
  a0 = v1 + 16; // 0x10
label80028DA4:
  temp = v1 == a0;
  a1 = lw(v1 + 0x00);
  if (temp) goto label80028E48;
  v1 += 4; // 0x04
  a3 = lw(a1 + 0x04);
  a2 = lw(a1 + 0x00);
  a3 = a3 & 0xF000;
  temp = (int32_t)a3 > 0;
  a3 = (int32_t)a2 >> 16;
  if (temp) goto label80028DA4;
  a2 = a2 << 16;
  a2 = (int32_t)a2 >> 16;
  cop2.IR0 = v0;
  cop2.IR1 = a2;
  cop2.IR2 = a3;
  a2 = lw(a1 - 0x40); // 0xFFFFFFC0
  a3 = lw(a1 + 0x40);
  GPF(SF_OFF, LM_OFF);
  t0 = (int32_t)a2 >> 16;
  t1 = (int32_t)a3 >> 16;
  a2 = a2 << 16;
  a2 = (int32_t)a2 >> 16;
  a3 = a3 << 16;
  a3 = (int32_t)a3 >> 16;
  a2 += a3;
  t0 += t1;
  cop2.IR0 = at;
  cop2.IR1 = a2;
  cop2.IR2 = t0;
  a2 = cop2.MAC1;
  a3 = cop2.MAC2;
  GPF(SF_OFF, LM_OFF);
  t0 = cop2.MAC1;
  t1 = cop2.MAC2;
  a2 += t0;
  a3 += t1;
  a2 = (int32_t)a2 >> 8;
  a3 = (int32_t)a3 >> 8;
  t0 = a2 & 0xFFFF;
  t1 = a3 << 16;
  t0 += t1;
  sw(a1 + 0x00, t0);
  goto label80028DA4;
label80028E48:
  t5 = s3 - s6;
  t5 = t5 >> 4;
  at = 0x8006FCF4; // &0x0EA69B
  t5 += at;
  at = (int32_t)s0 >> 20;
  at += t5;
  at = lb(at + 0x00);
  v0 = s0 << 12;
  v0 = (int32_t)v0 >> 20;
  v0 += t5;
  t6 = s0 & 0x7F;
  t4 = 168; // 0xA8
  mult(t6, t4);
  v0 = lb(v0 + 0x00);
  at = at & 0x3;
  v1 = (int32_t)s1 >> 20;
  v1 += t5;
  t6=lo;
  t6 += sp;
  v1 = lb(v1 + 0x00);
  v0 = v0 & 0x3;
  v0 = v0 << 8;
  a0 = s1 << 12;
  a0 = (int32_t)a0 >> 20;
  a0 += t5;
  a0 = lb(a0 + 0x00);
  v1 = v1 & 0x3;
  v1 = v1 << 16;
  t4 = 0x8006CFC8; // &0x000020
  at = at | v0;
  at = at | v1;
  a0 = a0 & 0x3;
  a0 = a0 << 24;
  at = at | a0;
label80028ED8:
  temp = at == 0;
  t5 = at & 0xFF;
  if (temp) goto label80029204;
  at = at >> 8;
  temp = t5 == 0;
  t4 += 36; // 0x24
  if (temp) goto label80028ED8;
  a0 = lw(t4 - 0x24); // 0xFFFFFFDC
  a1 = lw(t4 - 0x20); // 0xFFFFFFE0
  v0 = a0 >> 16;
  v0 += t7;
  v1 = a0 & 0xFFFF;
  v1 += t7;
  a0 = a1 >> 16;
  a0 += t7;
  a3 = lw(v0 + 0x04);
  t0 = lw(v1 + 0x04);
  t1 = lw(a0 + 0x04);
  a2 = lw(t4 - 0x1C); // 0xFFFFFFE4
  t2 = a3 & t0;
  t2 = t2 & t1;
  t2 = t2 & 0xF000;
  temp = (int32_t)t2 > 0;
  a3 = a3 & 0xFFF;
  if (temp) goto label80028FF0;
  t2 = lw(t6 + 0x00);
  t0 = t0 & 0xFFF;
  t1 = t1 & 0xFFF;
  t3 = a3 + t0;
  t3 += t1;
  t3 += t1;
  a3 = lw(v0 + 0x00);
  t0 = lw(v1 + 0x00);
  t1 = lw(a0 + 0x00);
  sw(fp + 0x08, a3);
  sw(fp + 0x14, t0);
  sw(fp + 0x20, t1);
  a3 = lw(v0 + 0x08);
  t0 = 0x08000000;
  a3 -= t0;
  t0 = lw(v1 + 0x08);
  t1 = lw(a0 + 0x08);
  sw(fp + 0x04, a3);
  sw(fp + 0x10, t0);
  sw(fp + 0x1C, t1);
  a3 = a1 & 0xFFFF;
  t0 = (int32_t)a2 >> 16;
  t1 = a2 & 0xFFFF;
  t3 = t3 >> 7;
  v0 = s1 & 0x38;
  v0 = v0 >> 1;
  t3 += v0;
  a2 = lw(t6 + 0x04);
  a3 += t2;
  t1 += t2;
  t0 += a2;
  sw(fp + 0x0C, a3);
  sw(fp + 0x18, t0);
  sw(fp + 0x24, t1);
  v1 = 0x09000000;
  sw(fp + 0x00, v1);
  t3 = t3 << 3;
  t3 += gp;
  v1 = lw(t3 + 0x00);
  sw(t3 + 0x00, fp);
  temp = v1 == 0;
  a0 = fp >> 16;
  if (temp) goto label80028FE8;
  sh(v1 + 0x00, fp);
  sb(v1 + 0x02, a0);
  fp += 40; // 0x28
  goto label80028FF0;
label80028FE8:
  sw(t3 + 0x04, fp);
  fp += 40; // 0x28
label80028FF0:
  a0 = lw(t4 - 0x18); // 0xFFFFFFE8
  a1 = lw(t4 - 0x14); // 0xFFFFFFEC
  v0 = a0 >> 16;
  v0 += t7;
  v1 = a0 & 0xFFFF;
  v1 += t7;
  a0 = a1 >> 16;
  a0 += t7;
  a3 = lw(v0 + 0x04);
  t0 = lw(v1 + 0x04);
  t1 = lw(a0 + 0x04);
  a2 = lw(t4 - 0x10); // 0xFFFFFFF0
  t2 = a3 & t0;
  t2 = t2 & t1;
  t2 = t2 & 0xF000;
  temp = (int32_t)t2 > 0;
  a3 = a3 & 0xFFF;
  if (temp) goto label800290F4;
  t2 = lw(t6 + 0x00);
  t0 = t0 & 0xFFF;
  t1 = t1 & 0xFFF;
  t3 = a3 + t0;
  t3 += t1;
  t3 += t1;
  a3 = lw(v0 + 0x00);
  t0 = lw(v1 + 0x00);
  t1 = lw(a0 + 0x00);
  sw(fp + 0x08, a3);
  sw(fp + 0x14, t0);
  sw(fp + 0x20, t1);
  a3 = lw(v0 + 0x08);
  t0 = 0x08000000;
  a3 -= t0;
  t0 = lw(v1 + 0x08);
  t1 = lw(a0 + 0x08);
  sw(fp + 0x04, a3);
  sw(fp + 0x10, t0);
  sw(fp + 0x1C, t1);
  a3 = a1 & 0xFFFF;
  t0 = (int32_t)a2 >> 16;
  t1 = a2 & 0xFFFF;
  t3 = t3 >> 7;
  v0 = s1 & 0x38;
  v0 = v0 >> 1;
  t3 += v0;
  a2 = lw(t6 + 0x04);
  a3 += t2;
  t1 += t2;
  t0 += a2;
  sw(fp + 0x0C, a3);
  sw(fp + 0x18, t0);
  sw(fp + 0x24, t1);
  v1 = 0x09000000;
  sw(fp + 0x00, v1);
  t3 = t3 << 3;
  t3 += gp;
  v1 = lw(t3 + 0x00);
  sw(t3 + 0x00, fp);
  temp = v1 == 0;
  a0 = fp >> 16;
  if (temp) goto label800290EC;
  sh(v1 + 0x00, fp);
  sb(v1 + 0x02, a0);
  fp += 40; // 0x28
  goto label800290F4;
label800290EC:
  sw(t3 + 0x04, fp);
  fp += 40; // 0x28
label800290F4:
  t5 = t5 & 0x1;
  temp = t5 == 0;
  a0 = lw(t4 - 0x0C); // 0xFFFFFFF4
  if (temp) goto label80028ED8;
  a1 = lw(t4 - 0x08); // 0xFFFFFFF8
  v0 = a0 >> 16;
  v0 += t7;
  v1 = a0 & 0xFFFF;
  v1 += t7;
  a0 = a1 >> 16;
  a0 += t7;
  a3 = lw(v0 + 0x04);
  t0 = lw(v1 + 0x04);
  t1 = lw(a0 + 0x04);
  a2 = lw(t4 - 0x04); // 0xFFFFFFFC
  t2 = a3 & t0;
  t2 = t2 & t1;
  t2 = t2 & 0xF000;
  temp = (int32_t)t2 > 0;
  a3 = a3 & 0xFFF;
  if (temp) goto label80028ED8;
  t2 = lw(t6 + 0x00);
  t0 = t0 & 0xFFF;
  t1 = t1 & 0xFFF;
  t3 = a3 + t0;
  t3 += t1;
  t3 += t1;
  a3 = lw(v0 + 0x00);
  t0 = lw(v1 + 0x00);
  t1 = lw(a0 + 0x00);
  sw(fp + 0x08, a3);
  sw(fp + 0x14, t0);
  sw(fp + 0x20, t1);
  a3 = lw(v0 + 0x08);
  t0 = 0x08000000;
  a3 -= t0;
  t0 = lw(v1 + 0x08);
  t1 = lw(a0 + 0x08);
  sw(fp + 0x04, a3);
  sw(fp + 0x10, t0);
  sw(fp + 0x1C, t1);
  a3 = a1 & 0xFFFF;
  t0 = (int32_t)a2 >> 16;
  t1 = a2 & 0xFFFF;
  t3 = t3 >> 7;
  v0 = s1 & 0x38;
  v0 = v0 >> 1;
  t3 += v0;
  a2 = lw(t6 + 0x04);
  a3 += t2;
  t1 += t2;
  t0 += a2;
  sw(fp + 0x0C, a3);
  sw(fp + 0x18, t0);
  sw(fp + 0x24, t1);
  v1 = 0x09000000;
  sw(fp + 0x00, v1);
  t3 = t3 << 3;
  t3 += gp;
  v1 = lw(t3 + 0x00);
  sw(t3 + 0x00, fp);
  temp = v1 == 0;
  a0 = fp >> 16;
  if (temp) goto label800291F8;
  sh(v1 + 0x00, fp);
  sb(v1 + 0x02, a0);
  fp += 40; // 0x28
  goto label80028ED8;
label800291F8:
  sw(t3 + 0x04, fp);
  fp += 40; // 0x28
  goto label80028ED8;
label80029204:
  at = 0x8006CF58; // &0x1F800000
  v0 = at + 64; // 0x40
  t6 += 32; // 0x20
label80029214:
  temp = at == v0;
  t6 += 8; // 0x08
  if (temp) goto label80028590;
  v1 = lw(at + 0x00);
  at += 4; // 0x04
  a0 = lw(v1 + 0x04);
  a1 = lw(v1 + 0x14);
  a2 = lw(v1 + 0x54);
  a3 = lw(v1 + 0x64);
  t1 = lw(t6 + 0x00);
  t0 = a0 & a1;
  t0 = t0 & a2;
  t0 = t0 & a3;
  t0 = t0 & 0xF000;
  temp = (int32_t)t0 > 0;
  a0 = a0 & 0xFFF;
  if (temp) goto label80029214;
  t2 = lw(t6 + 0x04);
  a1 = a1 & 0xFFF;
  a2 = a2 & 0xFFF;
  a3 = a3 & 0xFFF;
  t0 = a0 + a1;
  t0 += a2;
  t0 += a3;
  a0 = lw(v1 + 0x08);
  a1 = lw(v1 + 0x18);
  a2 = lw(v1 + 0x58);
  a3 = lw(v1 + 0x68);
  sw(fp + 0x04, a0);
  sw(fp + 0x10, a1);
  sw(fp + 0x1C, a2);
  sw(fp + 0x28, a3);
  t3 = t1 + 3840; // 0x0F00
  a0 = t2 >> 25;
  temp = a0 == 0;
  t4 = t1 + 3855; // 0x0F0F
  if (temp) goto label800292D0;
  a1 = 0x8006D058; // &0x000000
  a0 += a1;
  a1 = lw(a0 + 0x04);
  a0 = lw(a0 + 0x00);
  a2 = a1 & 0xFFFF;
  t3 = t1 + a2;
  a2 = a1 >> 16;
  t4 = t1 + a2;
  a2 = a0 & 0xFFFF;
  t1 += a2;
  a2 = (int32_t)a0 >> 16;
  t2 += a2;
label800292D0:
  sw(fp + 0x0C, t1);
  sw(fp + 0x18, t2);
  sw(fp + 0x24, t3);
  sw(fp + 0x30, t4);
  a0 = lw(v1 + 0x00);
  a1 = lw(v1 + 0x10);
  a2 = lw(v1 + 0x50);
  temp = s2 == 0;
  a3 = lw(v1 + 0x60);
  if (temp) goto label80029358;
  cop2.SXY0 = a0;
  cop2.SXY1 = a1;
  cop2.SXY2 = a2;
  t3 = s1 & 0x4;
  NCLIP();
  temp = (int32_t)t3 > 0;
  t3 = s1 & 0x2;
  if (temp) goto label80029358;
  t4 = cop2.MAC0;
  temp = t3 == 0;
  if (temp) goto label80029324;
  t4 = -t4;
label80029324:
  temp = (int32_t)t4 >= 0;
  if (temp) goto label80029358;
  cop2.SXY0 = a3;
  cop2.SXY1 = a2;
  cop2.SXY2 = a1;
  NCLIP();
  t4 = cop2.MAC0;
  temp = t3 == 0;
  if (temp) goto label80029354;
  t4 = -t4;
label80029354:
  temp = (int32_t)t4 < 0;
  if (temp) {
    sw(fp + 0x08, a0);
    goto label80029214;
  }
label80029358:
  sw(fp + 0x08, a0);
  sw(fp + 0x14, a1);
  sw(fp + 0x20, a2);
  sw(fp + 0x2C, a3);
  a0 = lh(v1 + 0x02);
  a1 = lh(v1 + 0x12);
  a2 = lh(v1 + 0x52);
  a3 = lh(v1 + 0x62);
  t1 = a0 - a1;
  t1 -= 512; // 0xFFFFFE00
  temp = (int32_t)t1 >= 0;
  t1 += 1024; // 0x0400
  if (temp) goto label800294A0;
  temp = (int32_t)t1 <= 0;
  t1 = a0 - a2;
  if (temp) goto label800294A0;
  t1 -= 512; // 0xFFFFFE00
  temp = (int32_t)t1 >= 0;
  t1 += 1024; // 0x0400
  if (temp) goto label800294A0;
  temp = (int32_t)t1 <= 0;
  t1 = a1 - a2;
  if (temp) goto label800294A0;
  t1 -= 512; // 0xFFFFFE00
  temp = (int32_t)t1 >= 0;
  t1 += 1024; // 0x0400
  if (temp) goto label800294A0;
  temp = (int32_t)t1 <= 0;
  t1 = a1 - a3;
  if (temp) goto label800294A0;
  t1 -= 512; // 0xFFFFFE00
  temp = (int32_t)t1 >= 0;
  t1 += 1024; // 0x0400
  if (temp) goto label800294A0;
  temp = (int32_t)t1 <= 0;
  t1 = a2 - a3;
  if (temp) goto label800294A0;
  t1 -= 512; // 0xFFFFFE00
  temp = (int32_t)t1 >= 0;
  t1 += 1024; // 0x0400
  if (temp) goto label800294A0;
  temp = (int32_t)t1 <= 0;
  a0 = lh(v1 + 0x00);
  if (temp) goto label800294A0;
  a1 = lh(v1 + 0x10);
  a2 = lh(v1 + 0x50);
  a3 = lh(v1 + 0x60);
  t1 = a0 - a1;
  t1 -= 1024; // 0xFFFFFC00
  temp = (int32_t)t1 >= 0;
  t1 += 2048; // 0x0800
  if (temp) goto label800294A0;
  temp = (int32_t)t1 <= 0;
  t1 = a0 - a2;
  if (temp) goto label800294A0;
  t1 -= 1024; // 0xFFFFFC00
  temp = (int32_t)t1 >= 0;
  t1 += 2048; // 0x0800
  if (temp) goto label800294A0;
  temp = (int32_t)t1 <= 0;
  t1 = a1 - a2;
  if (temp) goto label800294A0;
  t1 -= 1024; // 0xFFFFFC00
  temp = (int32_t)t1 >= 0;
  t1 += 2048; // 0x0800
  if (temp) goto label800294A0;
  temp = (int32_t)t1 <= 0;
  t1 = a1 - a3;
  if (temp) goto label800294A0;
  t1 -= 1024; // 0xFFFFFC00
  temp = (int32_t)t1 >= 0;
  t1 += 2048; // 0x0800
  if (temp) goto label800294A0;
  temp = (int32_t)t1 <= 0;
  t1 = a2 - a3;
  if (temp) goto label800294A0;
  t1 -= 1024; // 0xFFFFFC00
  temp = (int32_t)t1 >= 0;
  t1 += 2048; // 0x0800
  if (temp) goto label800294A0;
  temp = (int32_t)t1 <= 0;
  v1 = t0 >> 7;
  if (temp) goto label800294A0;
  temp = t0 == 0;
  a0 = s1 & 0x38;
  if (temp) goto label80029214;
  a0 = a0 >> 1;
  t0 = v1 + a0;
  v1 = 0x0C000000;
  sw(fp + 0x00, v1);
  t0 = t0 << 3;
  t0 += gp;
  v1 = lw(t0 + 0x00);
  sw(t0 + 0x00, fp);
  temp = v1 == 0;
  a0 = fp >> 16;
  if (temp) goto label80029494;
  sh(v1 + 0x00, fp);
  sb(v1 + 0x02, a0);
  fp += 52; // 0x34
  goto label80029214;
label80029494:
  sw(t0 + 0x04, fp);
  fp += 52; // 0x34
  goto label80029214;
label800294A0:
  v1 = lw(t7 + 0x03FC);
  temp = t0 == 0;
  sw(v1 + 0x00, fp);
  if (temp) goto label80029214;
  v1 += 4; // 0x04
  sw(t7 + 0x03FC, v1);
  t0 = t0 >> 7;
  a0 = s1 & 0x38;
  a0 = a0 >> 1;
  t0 += a0;
  v1 = 0x0C000000;
  sw(fp + 0x00, v1);
  t0 = t0 << 3;
  t0 += gp;
  v1 = lw(t0 + 0x00);
  sw(t0 + 0x00, fp);
  temp = v1 == 0;
  a0 = fp >> 16;
  if (temp) goto label80029494;
  sh(v1 + 0x00, fp);
  sb(v1 + 0x02, a0);
  fp += 52; // 0x34
  goto label80029214;
label800294F4:
  ra = 0x8006FCF4; // &0x0EA69B
  ra += 7680; // 0x1E00
label80029500:
  s3 = lw(ra + 0x00);
  ra += 4; // 0x04
  temp = s3 == 0;
  t7 = 0x1F800000;
  if (temp) goto label8002A0A0;
  temp = (int32_t)s3 < 0;
  at = s3 << 2;
  if (temp) goto label80029588;
  v0 = lw(at + 0x08);
  a0 = cop2.RBK;
  a1 = cop2.GBK;
  a2 = cop2.BBK;
  v1 = lw(at + 0x0C);
  s7 = v0 >> 14;
  t8 = v0 & 0xFFFF;
  t8 = t8 << 2;
  s7 -= a0;
  t8 = a1 - t8;
  v0 = lw(at + 0x14);
  t9 = v1 >> 14;
  t9 = a2 - t9;
  v1 = v1 & 0xFFFF;
  s4 = at + 28; // 0x1C
  a0 = v0 >> 22;
  a0 = a0 & 0x3FC;
  s4 += a0;
  a0 = v0 << 2;
  a0 = a0 & 0x3FC;
  s5 = s4 + a0;
  s3 = lw(ra + 0x00);
  ra += 4; // 0x04
  a0 = v0 >> 5;
  a0 = a0 & 0x7F8;
  s6 = s5 + a0;
  v1 = v1 << 4;
  s6 -= v1;
label80029588:
  at = s3 & 0x2;
  temp = at == 0;
  s2 = s3 & 0x1;
  if (temp) goto label80029500;
  s3 = s3 >> 2;
  s3 = s3 << 2;
  a0 = lw(s3 + 0x00);
  at = a0 >> 22;
  at = at & 0x3FC;
  at += s4;
  at = lw(at + 0x00);
  v0 = a0 >> 14;
  v0 = v0 & 0x3FC;
  v0 += s4;
  v0 = lw(v0 + 0x00);
  v1 = a0 << 2;
  v1 = v1 & 0x3FC;
  v1 += s4;
  v1 = lw(v1 + 0x00);
  s0 = lw(s3 + 0x08);
  a0 = at >> 19;
  a0 = a0 & 0x1FFC;
  a0 += s7;
  a1 = v0 >> 19;
  a1 = a1 & 0x1FFC;
  a1 += s7;
  a2 = v1 >> 19;
  a2 = a2 & 0x1FFC;
  a2 += s7;
  sh(t7 + 0x00, a0);
  sh(t7 + 0x40, a1);
  sh(t7 + 0xE0, a2);
  a3 = a0 + a1;
  a3 = (int32_t)a3 >> 1;
  sh(t7 + 0x20, a3);
  t0 = a0 + a3;
  t0 = (int32_t)t0 >> 1;
  sh(t7 + 0x10, t0);
  t0 = a1 + a3;
  t0 = (int32_t)t0 >> 1;
  sh(t7 + 0x30, t0);
  t0 = a0 + a2;
  t0 = (int32_t)t0 >> 1;
  sh(t7 + 0x90, t0);
  t1 = a0 + t0;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x50, t1);
  t1 = a2 + t0;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0xC0, t1);
  t1 = a1 + a2;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0xB0, t1);
  t2 = a1 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x80, t2);
  t2 = a2 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0xD0, t2);
  t2 = a0 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x60, t2);
  t2 = a1 + t0;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x70, t2);
  t2 = a2 + a3;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0xA0, t2);
  s1 = lw(s3 + 0x0C);
  a0 = at >> 8;
  a0 = a0 & 0x1FFC;
  a0 = t8 - a0;
  a1 = v0 >> 8;
  a1 = a1 & 0x1FFC;
  a1 = t8 - a1;
  a2 = v1 >> 8;
  a2 = a2 & 0x1FFC;
  a2 = t8 - a2;
  sh(t7 + 0x02, a0);
  sh(t7 + 0x42, a1);
  sh(t7 + 0xE2, a2);
  a3 = a0 + a1;
  a3 = (int32_t)a3 >> 1;
  sh(t7 + 0x22, a3);
  t0 = a0 + a3;
  t0 = (int32_t)t0 >> 1;
  sh(t7 + 0x12, t0);
  t0 = a1 + a3;
  t0 = (int32_t)t0 >> 1;
  sh(t7 + 0x32, t0);
  t0 = a0 + a2;
  t0 = (int32_t)t0 >> 1;
  sh(t7 + 0x92, t0);
  t1 = a0 + t0;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x52, t1);
  t1 = a2 + t0;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0xC2, t1);
  t1 = a1 + a2;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0xB2, t1);
  t2 = a1 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x82, t2);
  t2 = a2 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0xD2, t2);
  t2 = a0 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x62, t2);
  t2 = a1 + t0;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x72, t2);
  t2 = a2 + a3;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0xA2, t2);
  a0 = at << 2;
  a0 = a0 & 0xFFC;
  a0 = t9 - a0;
  a1 = v0 << 2;
  a1 = a1 & 0xFFC;
  a1 = t9 - a1;
  a2 = v1 << 2;
  a2 = a2 & 0xFFC;
  a2 = t9 - a2;
  sh(t7 + 0x04, a0);
  sh(t7 + 0x44, a1);
  sh(t7 + 0xE4, a2);
  a3 = a0 + a1;
  a3 = (int32_t)a3 >> 1;
  sh(t7 + 0x24, a3);
  t0 = a0 + a3;
  t0 = (int32_t)t0 >> 1;
  sh(t7 + 0x14, t0);
  t0 = a1 + a3;
  t0 = (int32_t)t0 >> 1;
  sh(t7 + 0x34, t0);
  t0 = a0 + a2;
  t0 = (int32_t)t0 >> 1;
  sh(t7 + 0x94, t0);
  t1 = a0 + t0;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x54, t1);
  t1 = a2 + t0;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0xC4, t1);
  t1 = a1 + a2;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0xB4, t1);
  t2 = a1 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x84, t2);
  t2 = a2 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0xD4, t2);
  t2 = a0 + t1;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x64, t2);
  t2 = a1 + t0;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0x74, t2);
  t2 = a2 + a3;
  t2 = (int32_t)t2 >> 1;
  sh(t7 + 0xA4, t2);
  at = lw(s3 + 0x04);
  v0 = s0 & 0x80;
  temp = v0 == 0;
  a1 = 0x34000000;
  if (temp) goto label80029834;
  v0 = 0x02000000;
  a1 += v0;
label80029834:
  v1 = at << 2;
  v1 = v1 & 0x3FC;
  v1 += s5;
  v1 = lw(v1 + 0x00);
  v0 = at >> 14;
  v0 = v0 & 0x3FC;
  v0 += s5;
  v0 = lw(v0 + 0x00);
  at = at >> 22;
  at = at & 0x3FC;
  at += s5;
  at = lw(at + 0x00);
  v1 += a1;
  v0 += a1;
  at += a1;
  t0 = 0xFFFEFEFF;
  sw(t7 + 0xE8, v1);
  sw(t7 + 0x48, v0);
  sw(t7 + 0x08, at);
  v1 = v1 & t0;
  v0 = v0 & t0;
  at = at & t0;
  a0 = at + v0;
  a0 = a0 >> 1;
  sw(t7 + 0x28, a0);
  a0 = a0 & t0;
  a1 = at + a0;
  a1 = a1 >> 1;
  sw(t7 + 0x18, a1);
  a1 = v0 + a0;
  a1 = a1 >> 1;
  sw(t7 + 0x38, a1);
  a1 = at + v1;
  a1 = a1 >> 1;
  sw(t7 + 0x98, a1);
  a1 = a1 & t0;
  a2 = at + a1;
  a2 = a2 >> 1;
  sw(t7 + 0x58, a2);
  a2 = v1 + a1;
  a2 = a2 >> 1;
  sw(t7 + 0xC8, a2);
  a2 = v0 + v1;
  a2 = a2 >> 1;
  sw(t7 + 0xB8, a2);
  a2 = a2 & t0;
  a3 = v0 + a2;
  a3 = a3 >> 1;
  sw(t7 + 0x88, a3);
  a3 = v1 + a2;
  a3 = a3 >> 1;
  sw(t7 + 0xD8, a3);
  a3 = at + a2;
  a3 = a3 >> 1;
  sw(t7 + 0x68, a3);
  a3 = v0 + a1;
  a3 = a3 >> 1;
  sw(t7 + 0x78, a3);
  a3 = v1 + a0;
  a3 = a3 >> 1;
  sw(t7 + 0xA8, a3);
  t0 = 0x1F800000;
  t1 = t0 + 256; // 0x0100
  sw(t1 - 0x10, 0); // 0xFFFFFFF0
  at = lh(t0 + 0x00);
  v0 = lh(t0 + 0x02);
  v1 = lh(t0 + 0x04);
  a0 = (int32_t)at >> 8;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = (int32_t)v0 >> 8;
  if (temp) goto label8002998C;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = (int32_t)v1 >> 8;
  if (temp) goto label8002998C;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = v1 << 20;
  if (temp) goto label8002998C;
  at = at << 4;
  v0 = v0 << 4;
  v0 += a0;
  a2 = 1; // 0x01
  goto label80029998;
label8002998C:
  v1 = v1 << 16;
  v0 += v1;
  a2 = 0;
label80029998:
  cop2.VZ0 = at;
  cop2.VXY0 = v0;
  t0 += 16; // 0x10
  t2 = 0x010000;
  t3 = 0x01000000;
  t4 = 0x02000000;
label800299B0:
  RTPS();
  a3 = a2;
  at = lh(t0 + 0x00);
  v0 = lh(t0 + 0x02);
  v1 = lh(t0 + 0x04);
  a0 = (int32_t)at >> 8;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = (int32_t)v0 >> 8;
  if (temp) goto label80029A0C;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = (int32_t)v1 >> 8;
  if (temp) goto label80029A0C;
  a0++;
  a0 = (int32_t)a0 >> 1;
  temp = a0 != 0;
  a0 = v1 << 20;
  if (temp) goto label80029A0C;
  at = at << 4;
  v0 = v0 << 4;
  v0 += a0;
  a2 = 1; // 0x01
  goto label80029A18;
label80029A0C:
  v1 = v1 << 16;
  v0 += v1;
  a2 = 0;
label80029A18:
  v1 = cop2.SXY2;
  a0 = cop2.SZ3;
  cop2.VZ0 = at;
  cop2.VXY0 = v0;
  temp = a3 == 0;
  t0 += 16; // 0x10
  if (temp) goto label80029A34;
  a0 = a0 >> 4;
label80029A34:
  temp = (int32_t)a0 > 0;
  v0 = v1 - t2;
  if (temp) goto label80029A40;
  a0 = a0 | 0xF000;
label80029A40:
  temp = (int32_t)v0 > 0;
  v0 = v1 - t3;
  if (temp) goto label80029A4C;
  a0 = a0 | 0x1000;
label80029A4C:
  temp = (int32_t)v0 < 0;
  v0 = v1 << 16;
  if (temp) goto label80029A58;
  a0 = a0 | 0x2000;
label80029A58:
  temp = (int32_t)v0 > 0;
  v0 -= t4;
  if (temp) goto label80029A64;
  a0 = a0 | 0x4000;
label80029A64:
  temp = (int32_t)v0 < 0;
  sw(t0 - 0x20, v1); // 0xFFFFFFE0
  if (temp) goto label80029A70;
  a0 = a0 | 0x8000;
label80029A70:
  temp = t0 != t1;
  sw(t0 - 0x1C, a0); // 0xFFFFFFE4
  if (temp) goto label800299B0;
  t5 = s3 - s6;
  t5 = t5 >> 4;
  at = 0x8006FCF4; // &0x0EA69B
  t5 += at;
  at = (int32_t)s0 >> 20;
  at += t5;
  at = lb(at + 0x00);
  v0 = s1 << 12;
  v0 = (int32_t)v0 >> 20;
  v0 += t5;
  t6 = s0 & 0x7F;
  t4 = 168; // 0xA8
  mult(t6, t4);
  v0 = lb(v0 + 0x00);
  at = at & 0x3;
  v1 = (int32_t)s1 >> 20;
  v1 += t5;
  t6=lo;
  t6 += sp;
  v1 = lb(v1 + 0x00);
  t4 = 0x8006D1C8; // &0x000020
  a0 = s0 & 0x300;
  a0 = a0 >> 8;
  a1 = 108; // 0x6C
  mult(a0, a1);
  v0 = v0 & 0x3;
  v0 = v0 << 8;
  at = at | v0;
  v1 = v1 & 0x3;
  v1 = v1 << 16;
  at = at | v1;
  a0=lo;
  t4 += a0;
label80029B04:
  temp = at == 0;
  t5 = at & 0xFF;
  if (temp) goto label80029E18;
  at = at >> 8;
  temp = t5 == 0;
  t4 += 36; // 0x24
  if (temp) goto label80029B04;
  a0 = lw(t4 - 0x24); // 0xFFFFFFDC
  a1 = lw(t4 - 0x20); // 0xFFFFFFE0
  v0 = a0 >> 16;
  v0 += t7;
  v1 = a0 & 0xFFFF;
  v1 += t7;
  a0 = a1 >> 16;
  a0 += t7;
  a3 = lw(v0 + 0x04);
  t0 = lw(v1 + 0x04);
  t1 = lw(a0 + 0x04);
  a2 = lw(t4 - 0x1C); // 0xFFFFFFE4
  t2 = a3 & t0;
  t2 = t2 & t1;
  t2 = t2 & 0xF000;
  temp = (int32_t)t2 > 0;
  a3 = a3 & 0xFFF;
  if (temp) goto label80029C14;
  t2 = lw(t6 + 0x00);
  t0 = t0 & 0xFFF;
  t1 = t1 & 0xFFF;
  t3 = a3 + t0;
  t3 += t1;
  t3 += t1;
  a3 = lw(v0 + 0x00);
  t0 = lw(v1 + 0x00);
  t1 = lw(a0 + 0x00);
  sw(fp + 0x08, a3);
  sw(fp + 0x14, t0);
  sw(fp + 0x20, t1);
  a3 = lw(v0 + 0x08);
  t0 = lw(v1 + 0x08);
  t1 = lw(a0 + 0x08);
  sw(fp + 0x04, a3);
  sw(fp + 0x10, t0);
  sw(fp + 0x1C, t1);
  a3 = a1 & 0xFFFF;
  t0 = (int32_t)a2 >> 16;
  t1 = a2 & 0xFFFF;
  t3 = t3 >> 7;
  v0 = s1 & 0x38;
  v0 = v0 >> 1;
  t3 += v0;
  a2 = lw(t6 + 0x04);
  a3 += t2;
  t1 += t2;
  t0 += a2;
  sw(fp + 0x0C, a3);
  sw(fp + 0x18, t0);
  sw(fp + 0x24, t1);
  v1 = 0x09000000;
  sw(fp + 0x00, v1);
  t3 = t3 << 3;
  t3 += gp;
  v1 = lw(t3 + 0x00);
  sw(t3 + 0x00, fp);
  temp = v1 == 0;
  a0 = fp >> 16;
  if (temp) goto label80029C0C;
  sh(v1 + 0x00, fp);
  sb(v1 + 0x02, a0);
  fp += 40; // 0x28
  goto label80029C14;
label80029C0C:
  sw(t3 + 0x04, fp);
  fp += 40; // 0x28
label80029C14:
  a0 = lw(t4 - 0x18); // 0xFFFFFFE8
  a1 = lw(t4 - 0x14); // 0xFFFFFFEC
  v0 = a0 >> 16;
  v0 += t7;
  v1 = a0 & 0xFFFF;
  v1 += t7;
  a0 = a1 >> 16;
  a0 += t7;
  a3 = lw(v0 + 0x04);
  t0 = lw(v1 + 0x04);
  t1 = lw(a0 + 0x04);
  a2 = lw(t4 - 0x10); // 0xFFFFFFF0
  t2 = a3 & t0;
  t2 = t2 & t1;
  t2 = t2 & 0xF000;
  temp = (int32_t)t2 > 0;
  a3 = a3 & 0xFFF;
  if (temp) goto label80029D10;
  t2 = lw(t6 + 0x00);
  t0 = t0 & 0xFFF;
  t1 = t1 & 0xFFF;
  t3 = a3 + t0;
  t3 += t1;
  t3 += t1;
  a3 = lw(v0 + 0x00);
  t0 = lw(v1 + 0x00);
  t1 = lw(a0 + 0x00);
  sw(fp + 0x08, a3);
  sw(fp + 0x14, t0);
  sw(fp + 0x20, t1);
  a3 = lw(v0 + 0x08);
  t0 = lw(v1 + 0x08);
  t1 = lw(a0 + 0x08);
  sw(fp + 0x04, a3);
  sw(fp + 0x10, t0);
  sw(fp + 0x1C, t1);
  a3 = a1 & 0xFFFF;
  t0 = (int32_t)a2 >> 16;
  t1 = a2 & 0xFFFF;
  t3 = t3 >> 7;
  v0 = s1 & 0x38;
  v0 = v0 >> 1;
  t3 += v0;
  a2 = lw(t6 + 0x04);
  a3 += t2;
  t1 += t2;
  t0 += a2;
  sw(fp + 0x0C, a3);
  sw(fp + 0x18, t0);
  sw(fp + 0x24, t1);
  v1 = 0x09000000;
  sw(fp + 0x00, v1);
  t3 = t3 << 3;
  t3 += gp;
  v1 = lw(t3 + 0x00);
  sw(t3 + 0x00, fp);
  temp = v1 == 0;
  a0 = fp >> 16;
  if (temp) goto label80029D08;
  sh(v1 + 0x00, fp);
  sb(v1 + 0x02, a0);
  fp += 40; // 0x28
  goto label80029D10;
label80029D08:
  sw(t3 + 0x04, fp);
  fp += 40; // 0x28
label80029D10:
  t5 = t5 & 0x1;
  temp = t5 == 0;
  a0 = lw(t4 - 0x0C); // 0xFFFFFFF4
  if (temp) goto label80029B04;
  a1 = lw(t4 - 0x08); // 0xFFFFFFF8
  v0 = a0 >> 16;
  v0 += t7;
  v1 = a0 & 0xFFFF;
  v1 += t7;
  a0 = a1 >> 16;
  a0 += t7;
  a3 = lw(v0 + 0x04);
  t0 = lw(v1 + 0x04);
  t1 = lw(a0 + 0x04);
  a2 = lw(t4 - 0x04); // 0xFFFFFFFC
  t2 = a3 & t0;
  t2 = t2 & t1;
  t2 = t2 & 0xF000;
  temp = (int32_t)t2 > 0;
  a3 = a3 & 0xFFF;
  if (temp) goto label80029B04;
  t2 = lw(t6 + 0x00);
  t0 = t0 & 0xFFF;
  t1 = t1 & 0xFFF;
  t3 = a3 + t0;
  t3 += t1;
  t3 += t1;
  a3 = lw(v0 + 0x00);
  t0 = lw(v1 + 0x00);
  t1 = lw(a0 + 0x00);
  sw(fp + 0x08, a3);
  sw(fp + 0x14, t0);
  sw(fp + 0x20, t1);
  a3 = lw(v0 + 0x08);
  t0 = lw(v1 + 0x08);
  t1 = lw(a0 + 0x08);
  sw(fp + 0x04, a3);
  sw(fp + 0x10, t0);
  sw(fp + 0x1C, t1);
  a3 = a1 & 0xFFFF;
  t0 = (int32_t)a2 >> 16;
  t1 = a2 & 0xFFFF;
  t3 = t3 >> 7;
  v0 = s1 & 0x38;
  v0 = v0 >> 1;
  t3 += v0;
  a2 = lw(t6 + 0x04);
  a3 += t2;
  t1 += t2;
  t0 += a2;
  sw(fp + 0x0C, a3);
  sw(fp + 0x18, t0);
  sw(fp + 0x24, t1);
  v1 = 0x09000000;
  sw(fp + 0x00, v1);
  t3 = t3 << 3;
  t3 += gp;
  v1 = lw(t3 + 0x00);
  sw(t3 + 0x00, fp);
  temp = v1 == 0;
  a0 = fp >> 16;
  if (temp) goto label80029E0C;
  sh(v1 + 0x00, fp);
  sb(v1 + 0x02, a0);
  fp += 40; // 0x28
  goto label80029B04;
label80029E0C:
  sw(t3 + 0x04, fp);
  fp += 40; // 0x28
  goto label80029B04;
label80029E18:
  t2 = 0x8006D0F8; // &0x010500
  t3 = t2 + 64; // 0x40
  t4 = 0x8006D388; // &0x60781800
  t5 = 0x8006D3C8; // &0x000000
  t6 += 40; // 0x28
label80029E38:
  at = lw(t2 + 0x00);
  temp = t2 == t3;
  t2 += 4; // 0x04
  if (temp) goto label80029500;
  v0 = s0 >> 8;
  v0 = v0 & 0x3;
  v1 = at & 0x3C;
  v1 += v0;
  v1 += t4;
  v1 = lb(v1 + 0x00);
  a3 = at >> 20;
  a3 = a3 & 0xFF0;
  t0 = at >> 12;
  t0 = t0 & 0xFF0;
  t1 = at >> 4;
  t1 = t1 & 0xFF0;
  v1 += t6;
  a0 = lw(v1 + 0x04);
  a3 += t7;
  t0 += t7;
  t1 += t7;
  v0 += at;
  v0 = v0 & 0x3;
  v0 = v0 << 7;
  v1 = lw(v1 + 0x00);
  a1 = a0 >> 25;
  a1 = a1 & 0x78;
  v0 += a1;
  v0 += t5;
  a1 = lw(v0 + 0x00);
  v0 = lh(v0 + 0x04);
  a2 = a1 & 0xFFFF;
  a1 = (int32_t)a1 >> 16;
  a2 += v1;
  a1 += a0;
  a0 = v0 + v1;
  sw(fp + 0x0C, a2);
  sw(fp + 0x18, a1);
  sw(fp + 0x24, a0);
  at = lw(a3 + 0x04);
  v0 = lw(t0 + 0x04);
  v1 = lw(t1 + 0x04);
  a0 = at & v0;
  a0 = a0 & v1;
  a0 = a0 & 0xF000;
  temp = (int32_t)a0 > 0;
  a0 = lw(a3 + 0x08);
  if (temp) goto label80029E38;
  a1 = lw(t0 + 0x08);
  a2 = lw(t1 + 0x08);
  sw(fp + 0x04, a0);
  sw(fp + 0x10, a1);
  sw(fp + 0x1C, a2);
  at = at & 0xFFF;
  v0 = v0 & 0xFFF;
  v1 = v1 & 0xFFF;
  a0 = at + v0;
  a0 += v1;
  a0 += v1;
  at = lw(a3 + 0x00);
  v0 = lw(t0 + 0x00);
  temp = s2 == 0;
  v1 = lw(t1 + 0x00);
  if (temp) goto label80029F60;
  cop2.SXY0 = at;
  cop2.SXY1 = v0;
  cop2.SXY2 = v1;
  a1 = s1 & 0x4;
  NCLIP();
  temp = (int32_t)a1 > 0;
  a1 = s1 & 0x2;
  if (temp) goto label80029F60;
  a2 = cop2.MAC0;
  temp = a1 == 0;
  if (temp) goto label80029F5C;
  a2 = -a2;
label80029F5C:
  temp = (int32_t)a2 < 0;
  if (temp) {
    sw(fp + 0x08, at);
    goto label80029E38;
  }
label80029F60:
  sw(fp + 0x08, at);
  sw(fp + 0x14, v0);
  sw(fp + 0x20, v1);
  at = lh(a3 + 0x02);
  v0 = lh(t0 + 0x02);
  v1 = lh(t1 + 0x02);
  a1 = at - v0;
  a1 -= 512; // 0xFFFFFE00
  temp = (int32_t)a1 >= 0;
  a1 += 1024; // 0x0400
  if (temp) goto label8002A04C;
  temp = (int32_t)a1 <= 0;
  a1 = at - v1;
  if (temp) goto label8002A04C;
  a1 -= 512; // 0xFFFFFE00
  temp = (int32_t)a1 >= 0;
  a1 += 1024; // 0x0400
  if (temp) goto label8002A04C;
  temp = (int32_t)a1 <= 0;
  a1 = v0 - v1;
  if (temp) goto label8002A04C;
  a1 -= 512; // 0xFFFFFE00
  temp = (int32_t)a1 >= 0;
  a1 += 1024; // 0x0400
  if (temp) goto label8002A04C;
  temp = (int32_t)a1 <= 0;
  at = lh(a3 + 0x00);
  if (temp) goto label8002A04C;
  v0 = lh(t0 + 0x00);
  v1 = lh(t1 + 0x00);
  a1 = at - v0;
  a1 -= 1024; // 0xFFFFFC00
  temp = (int32_t)a1 >= 0;
  a1 += 2048; // 0x0800
  if (temp) goto label8002A04C;
  temp = (int32_t)a1 <= 0;
  a1 = at - v1;
  if (temp) goto label8002A04C;
  a1 -= 1024; // 0xFFFFFC00
  temp = (int32_t)a1 >= 0;
  a1 += 2048; // 0x0800
  if (temp) goto label8002A04C;
  temp = (int32_t)a1 <= 0;
  a1 = v0 - v1;
  if (temp) goto label8002A04C;
  a1 -= 1024; // 0xFFFFFC00
  temp = (int32_t)a1 >= 0;
  a1 += 2048; // 0x0800
  if (temp) goto label8002A04C;
  temp = (int32_t)a1 <= 0;
  v1 = a0 >> 7;
  if (temp) goto label8002A04C;
  temp = a0 == 0;
  a1 = s1 & 0x38;
  if (temp) goto label80029E38;
  a1 = a1 >> 1;
  a0 = v1 + a1;
  v0 = 0x09000000;
  sw(fp + 0x00, v0);
  a0 = a0 << 3;
  a0 += gp;
  v0 = lw(a0 + 0x00);
  sw(a0 + 0x00, fp);
  temp = v0 == 0;
  v1 = fp >> 16;
  if (temp) goto label8002A040;
  sh(v0 + 0x00, fp);
  sb(v0 + 0x02, v1);
  fp += 40; // 0x28
  goto label80029E38;
label8002A040:
  sw(a0 + 0x04, fp);
  fp += 40; // 0x28
  goto label80029E38;
label8002A04C:
  v1 = lw(t7 + 0x03FC);
  temp = a0 == 0;
  sw(v1 + 0x00, fp);
  if (temp) goto label80029E38;
  v1 += 4; // 0x04
  sw(t7 + 0x03FC, v1);
  a0 = a0 >> 7;
  a1 = s1 & 0x38;
  a1 = a1 >> 1;
  a0 += a1;
  v0 = 0x09000000;
  sw(fp + 0x00, v0);
  a0 = a0 << 3;
  a0 += gp;
  v0 = lw(a0 + 0x00);
  sw(a0 + 0x00, fp);
  temp = v0 == 0;
  v1 = fp >> 16;
  if (temp) goto label8002A040;
  sh(v0 + 0x00, fp);
  sb(v0 + 0x02, v1);
  fp += 40; // 0x28
  goto label80029E38;
label8002A0A0:
  sp = lw(t7 + 0x03FC);
  ra = 0x8006FCF4; // &0x0EA69B
  ra += 8192; // 0x2000
label8002A0B0:
  temp = ra == sp;
  gp = lw(ra + 0x00);
  if (temp) goto label8002A6B0;
  ra += 4; // 0x04
  at = lb(gp + 0x03);
  v0 = 12; // 0x0C
  temp = at != v0;
  sb(gp + 0x03, 0);
  if (temp) goto label8002A354;
  at = lw(gp + 0x08);
  v0 = lw(gp + 0x14);
  v1 = lw(gp + 0x20);
  a0 = lw(gp + 0x2C);
  a1 = at << 16;
  a2 = v0 << 16;
  a3 = v1 << 16;
  t0 = a0 << 16;
  a1 = (int32_t)a1 >> 16;
  a2 = (int32_t)a2 >> 16;
  a3 = (int32_t)a3 >> 16;
  t0 = (int32_t)t0 >> 16;
  t1 = a1 + a2;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x10, t1);
  t1 = a1 + a3;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x30, t1);
  t1 = a2 + a3;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x40, t1);
  t1 = a2 + t0;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x50, t1);
  t1 = a3 + t0;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x70, t1);
  a1 = (int32_t)at >> 16;
  a2 = (int32_t)v0 >> 16;
  a3 = (int32_t)v1 >> 16;
  t0 = (int32_t)a0 >> 16;
  t1 = a1 + a2;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x12, t1);
  t1 = a1 + a3;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x32, t1);
  t1 = a2 + a3;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x42, t1);
  t1 = a2 + t0;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x52, t1);
  t1 = a3 + t0;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x72, t1);
  sw(t7 + 0x00, at);
  sw(t7 + 0x20, v0);
  sw(t7 + 0x60, v1);
  sw(t7 + 0x80, a0);
  at = lw(gp + 0x04);
  v0 = lw(gp + 0x10);
  v1 = lw(gp + 0x1C);
  a0 = lw(gp + 0x28);
  a1 = 0x08000000;
  at -= a1;
  v0 -= a1;
  v1 -= a1;
  a0 -= a1;
  sw(t7 + 0x04, at);
  sw(t7 + 0x24, v0);
  sw(t7 + 0x64, v1);
  sw(t7 + 0x84, a0);
  a1 = 0xFFFEFEFF;
  at = at & a1;
  v0 = v0 & a1;
  v1 = v1 & a1;
  a0 = a0 & a1;
  a1 = at + v0;
  a1 = a1 >> 1;
  sw(t7 + 0x14, a1);
  a1 = at + v1;
  a1 = a1 >> 1;
  sw(t7 + 0x34, a1);
  a1 = v0 + v1;
  a1 = a1 >> 1;
  sw(t7 + 0x44, a1);
  a1 = v0 + a0;
  a1 = a1 >> 1;
  sw(t7 + 0x54, a1);
  a1 = v1 + a0;
  a1 = a1 >> 1;
  sw(t7 + 0x74, a1);
  at = lw(gp + 0x0C);
  v0 = lw(gp + 0x18);
  v1 = lw(gp + 0x24);
  a0 = lw(gp + 0x30);
  t5 = at >> 16;
  t5 = t5 << 16;
  t6 = v0 >> 16;
  t6 = t6 << 16;
  sh(t7 + 0x08, at);
  sh(t7 + 0x28, v0);
  sh(t7 + 0x68, v1);
  sh(t7 + 0x88, a0);
  a1 = at & 0xFF00;
  a2 = v0 & 0xFF00;
  a3 = v1 & 0xFF00;
  t0 = a0 & 0xFF00;
  t1 = a1 + a2;
  t1 = t1 >> 1;
  sh(t7 + 0x18, t1);
  t1 = a1 + a3;
  t1 = t1 >> 1;
  sh(t7 + 0x38, t1);
  t1 = a2 + a3;
  t1 = t1 >> 1;
  sh(t7 + 0x48, t1);
  t1 = a2 + t0;
  t1 = t1 >> 1;
  sh(t7 + 0x58, t1);
  t1 = a3 + t0;
  t1 = t1 >> 1;
  sh(t7 + 0x78, t1);
  a1 = at & 0xFF;
  a2 = v0 & 0xFF;
  a3 = v1 & 0xFF;
  t0 = a0 & 0xFF;
  t1 = a1 + a2;
  t1 = t1 >> 1;
  sb(t7 + 0x18, t1);
  t1 = a1 + a3;
  t1 = t1 >> 1;
  sb(t7 + 0x38, t1);
  t1 = a2 + a3;
  t1 = t1 >> 1;
  sb(t7 + 0x48, t1);
  t1 = a2 + t0;
  t1 = t1 >> 1;
  sb(t7 + 0x58, t1);
  t1 = a3 + t0;
  t1 = t1 >> 1;
  sb(t7 + 0x78, t1);
  at = t7;
  v0 = t7 + 144; // 0x90
  v1 = 0x010000;
  a0 = 0x01000000;
  a1 = 0x02000000;
label8002A2F8:
  a2 = lw(at + 0x00);
  t0 = 0;
  a3 = a2 - v1;
  temp = (int32_t)a3 > 0;
  a3 = a2 - a0;
  if (temp) goto label8002A310;
  t0 = t0 | 0x1;
label8002A310:
  temp = (int32_t)a3 < 0;
  a3 = a2 << 16;
  if (temp) goto label8002A31C;
  t0 = t0 | 0x2;
label8002A31C:
  temp = (int32_t)a3 > 0;
  a3 -= a1;
  if (temp) goto label8002A328;
  t0 = t0 | 0x4;
label8002A328:
  temp = (int32_t)a3 < 0;
  at += 16; // 0x10
  if (temp) goto label8002A334;
  t0 = t0 | 0x8;
label8002A334:
  temp = at != v0;
  sb(at - 0x01, t0); // 0xFFFFFFFF
  if (temp) goto label8002A2F8;
  t4 = lw(gp + 0x00);
  t3 = gp;
  s0 = 0x8006D5E4; // &0x103000
  s1 = s0 + 48; // 0x30
  goto label8002A51C;
label8002A354:
  at = lw(gp + 0x08);
  v0 = lw(gp + 0x14);
  v1 = lw(gp + 0x20);
  a1 = at << 16;
  a2 = v0 << 16;
  a3 = v1 << 16;
  a1 = (int32_t)a1 >> 16;
  a2 = (int32_t)a2 >> 16;
  a3 = (int32_t)a3 >> 16;
  t1 = a1 + a2;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x10, t1);
  t1 = a1 + a3;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x30, t1);
  t1 = a2 + a3;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x40, t1);
  a1 = (int32_t)at >> 16;
  a2 = (int32_t)v0 >> 16;
  a3 = (int32_t)v1 >> 16;
  t1 = a1 + a2;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x12, t1);
  t1 = a1 + a3;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x32, t1);
  t1 = a2 + a3;
  t1 = (int32_t)t1 >> 1;
  sh(t7 + 0x42, t1);
  sw(t7 + 0x00, at);
  sw(t7 + 0x20, v0);
  sw(t7 + 0x50, v1);
  at = lw(gp + 0x04);
  v0 = lw(gp + 0x10);
  v1 = lw(gp + 0x1C);
  sw(t7 + 0x04, at);
  sw(t7 + 0x24, v0);
  sw(t7 + 0x54, v1);
  a1 = 0xFFFEFEFF;
  at = at & a1;
  v0 = v0 & a1;
  v1 = v1 & a1;
  a1 = at + v0;
  a1 = a1 >> 1;
  sw(t7 + 0x14, a1);
  a1 = at + v1;
  a1 = a1 >> 1;
  sw(t7 + 0x34, a1);
  a1 = v0 + v1;
  a1 = a1 >> 1;
  sw(t7 + 0x44, a1);
  at = lw(gp + 0x0C);
  v0 = lw(gp + 0x18);
  v1 = lw(gp + 0x24);
  t5 = at >> 16;
  t5 = t5 << 16;
  t6 = v0 >> 16;
  t6 = t6 << 16;
  sh(t7 + 0x08, at);
  sh(t7 + 0x28, v0);
  sh(t7 + 0x58, v1);
  a1 = at & 0xFF00;
  a2 = v0 & 0xFF00;
  a3 = v1 & 0xFF00;
  t1 = a1 + a2;
  t1 = t1 >> 1;
  sh(t7 + 0x18, t1);
  t1 = a1 + a3;
  t1 = t1 >> 1;
  sh(t7 + 0x38, t1);
  t1 = a2 + a3;
  t1 = t1 >> 1;
  sh(t7 + 0x48, t1);
  a1 = at & 0xFF;
  a2 = v0 & 0xFF;
  a3 = v1 & 0xFF;
  t1 = a1 + a2;
  t1 = t1 >> 1;
  sb(t7 + 0x18, t1);
  t1 = a1 + a3;
  t1 = t1 >> 1;
  sb(t7 + 0x38, t1);
  t1 = a2 + a3;
  t1 = t1 >> 1;
  sb(t7 + 0x48, t1);
  at = t7;
  v0 = t7 + 96; // 0x60
  v1 = 0x010000;
  a0 = 0x01000000;
  a1 = 0x02000000;
label8002A4C4:
  a2 = lw(at + 0x00);
  t0 = 0;
  a3 = a2 - v1;
  temp = (int32_t)a3 > 0;
  a3 = a2 - a0;
  if (temp) goto label8002A4DC;
  t0 = t0 | 0x1;
label8002A4DC:
  temp = (int32_t)a3 < 0;
  a3 = a2 << 16;
  if (temp) goto label8002A4E8;
  t0 = t0 | 0x2;
label8002A4E8:
  temp = (int32_t)a3 > 0;
  a3 -= a1;
  if (temp) goto label8002A4F4;
  t0 = t0 | 0x4;
label8002A4F4:
  temp = (int32_t)a3 < 0;
  at += 16; // 0x10
  if (temp) goto label8002A500;
  t0 = t0 | 0x8;
label8002A500:
  temp = at != v0;
  sb(at - 0x01, t0); // 0xFFFFFFFF
  if (temp) goto label8002A4C4;
  t4 = lw(gp + 0x00);
  t3 = gp;
  s0 = 0x8006D5C8; // &0x103000
  s1 = s0 + 28; // 0x1C
label8002A51C:
  temp = s0 == s1;
  t2 = lw(s0 + 0x00);
  if (temp) goto label8002A674;
  s0 += 4; // 0x04
  at = t2 >> 24;
  v0 = t2 >> 16;
  v0 = v0 & 0xFF;
  v1 = t2 >> 8;
  v1 = v1 & 0xFF;
  at += t7;
  v0 += t7;
  v1 += t7;
  a0 = lb(at + 0x0F);
  a1 = lb(v0 + 0x0F);
  a2 = lb(v1 + 0x0F);
  a0 = a0 & a1;
  a0 = a0 & a2;
  temp = (int32_t)a0 > 0;
  a0 = lhu(at + 0x08);
  if (temp) goto label8002A51C;
  a1 = lhu(v0 + 0x08);
  a2 = lhu(v1 + 0x08);
  a3 = lw(at + 0x04);
  t0 = lw(v0 + 0x04);
  t1 = lw(v1 + 0x04);
  sw(fp + 0x04, a3);
  sw(fp + 0x10, t0);
  sw(fp + 0x1C, t1);
  a0 += t5;
  a1 += t6;
  sw(fp + 0x0C, a0);
  sw(fp + 0x18, a1);
  sw(fp + 0x24, a2);
  a3 = lw(at + 0x00);
  t0 = lw(v0 + 0x00);
  t1 = lw(v1 + 0x00);
  t2 = t2 & 0x1;
  temp = (int32_t)t2 > 0;
  a0 = lh(at + 0x02);
  if (temp) goto label8002A63C;
  a1 = lh(v0 + 0x02);
  a2 = lh(v1 + 0x02);
  t2 = a0 - a1;
  t2 -= 512; // 0xFFFFFE00
  temp = (int32_t)t2 >= 0;
  t2 += 1024; // 0x0400
  if (temp) goto label8002A668;
  temp = (int32_t)t2 <= 0;
  t2 = a0 - a2;
  if (temp) goto label8002A668;
  t2 -= 512; // 0xFFFFFE00
  temp = (int32_t)t2 >= 0;
  t2 += 1024; // 0x0400
  if (temp) goto label8002A668;
  temp = (int32_t)t2 <= 0;
  t2 = a1 - a2;
  if (temp) goto label8002A668;
  t2 -= 512; // 0xFFFFFE00
  temp = (int32_t)t2 >= 0;
  t2 += 1024; // 0x0400
  if (temp) goto label8002A668;
  temp = (int32_t)t2 <= 0;
  a0 = lh(at + 0x00);
  if (temp) goto label8002A668;
  a1 = lh(v0 + 0x00);
  a2 = lh(v1 + 0x00);
  t2 = a0 - a1;
  t2 -= 1024; // 0xFFFFFC00
  temp = (int32_t)t2 >= 0;
  t2 += 2048; // 0x0800
  if (temp) goto label8002A668;
  temp = (int32_t)t2 <= 0;
  t2 = a0 - a2;
  if (temp) goto label8002A668;
  t2 -= 1024; // 0xFFFFFC00
  temp = (int32_t)t2 >= 0;
  t2 += 2048; // 0x0800
  if (temp) goto label8002A668;
  temp = (int32_t)t2 <= 0;
  t2 = a1 - a2;
  if (temp) goto label8002A668;
  t2 -= 1024; // 0xFFFFFC00
  temp = (int32_t)t2 >= 0;
  t2 += 2048; // 0x0800
  if (temp) goto label8002A668;
  temp = (int32_t)t2 <= 0;
  if (temp) {
    sw(fp + 0x08, a3);
    goto label8002A668;
  }
label8002A63C:
  sw(fp + 0x08, a3);
  sw(fp + 0x14, t0);
  sw(fp + 0x20, t1);
  at = 0x09000000;
  sw(fp + 0x00, at);
  at = fp >> 16;
  sh(t3 + 0x00, fp);
  sb(t3 + 0x02, at);
  t3 = fp;
  fp += 40; // 0x28
  goto label8002A51C;
label8002A668:
  sw(sp + 0x00, fp);
  sp += 4; // 0x04
  goto label8002A63C;
label8002A674:
  temp = t4 == 0;
  at = t4 >> 16;
  if (temp) goto label8002A688;
  sh(t3 + 0x00, t4);
  sb(t3 + 0x02, at);
  goto label8002A0B0;
label8002A688:
  at = ordered_linked_list;
  at = lw(at + 0x00);
label8002A698:
  v0 = lw(at + 0x00);
  at += 8; // 0x08
  temp = v0 != gp;
  if (temp) goto label8002A698;
  sw(at - 0x08, t3); // 0xFFFFFFF8
  goto label8002A0B0;
label8002A6B0:
  sw(allocator1_ptr, fp);
  at = 0x80077DD8;
  ra = lw(at + 0x2C);
  fp = lw(at + 0x28);
  sp = lw(at + 0x24);
  gp = lw(at + 0x20);
  s7 = lw(at + 0x1C);
  s6 = lw(at + 0x18);
  s5 = lw(at + 0x14);
  s4 = lw(at + 0x10);
  s3 = lw(at + 0x0C);
  s2 = lw(at + 0x08);
  s1 = lw(at + 0x04);
  s0 = lw(at + 0x00);
}

// size: 0x00001E2C
void function_80023AC4(void)
{
  uint32_t temp;
  at = 0x80077DD8;
  sw(at + 0x00, s0);
  sw(at + 0x04, s1);
  sw(at + 0x08, s2);
  sw(at + 0x0C, s3);
  sw(at + 0x10, s4);
  sw(at + 0x14, s5);
  sw(at + 0x18, s6);
  sw(at + 0x1C, s7);
  sw(at + 0x20, gp);
  sw(at + 0x24, sp);
  sw(at + 0x28, fp);
  sw(at + 0x2C, ra);
  ra = spyro_position;
  fp = lw(0x80076378);
  sp = 0x80076DD0;
  t9 = 0x8006FCF4 + 0x1900;
  s7 = 0;
  s6 = 0;

  t6 = lbu(ra + 0x24);
  
  gp = lbu(ra + 0x18);
  a0 = lbu(ra + 0x1E);

  gp = lw(fp + 0x38 + gp*4);
  a0 = lw(gp + 0x24 + a0*4);
  
  a1 = lbu(gp + 0x08);
  s6 = lbu(gp + 0x0B);
  at = a0 >> 9;
  at = at & 0x1000;
  t6 = t6 << 8;
  at += t6;
  a0 = a0 << 11;
  a0 = a0 >> 10;
  a1 = a1 << 16;
  at += a1;
  a2 = lw(gp + 0x0010);
  a3 = lw(a0 + 0x0008);
  t2 = lw(a0 + 0x0010);
  t3 = lw(a0 + 0x0014);
  t0 = a0 + 24; // 0x0018
  a3 = a3 >> 20;
  a3 += t0;
  sw(t9 + 0x0024, a2);
  sw(t9 + 0x0028, a3);
  temp = t6 == 0;
  sw(t9 + 0x002C, t0);
  if (temp) goto label80023C04;
  v1 = lbu(ra + 0x0019);
  a0 = lbu(ra + 0x001F);
  v1 = v1 << 2;
  v1 += fp;
  v1 = lw(v1 + 0x0038);
  a0 = a0 << 2;
  a0 += v1;
  a0 = lw(a0 + 0x0024);
  s7 = lbu(v1 + 0x000B);
  a1 = a0 >> 8;
  a1 = a1 & 0x2000;
  at += a1;
  a0 = a0 << 11;
  a0 = a0 >> 10;
  a2 = lw(v1 + 0x0010);
  a3 = lw(a0 + 0x0008);
  t4 = lw(a0 + 0x0010);
  t5 = lw(a0 + 0x0014);
  t0 = a0 + 24; // 0x0018
  a3 = a3 >> 20;
  a3 += t0;
  sw(t9 + 0x0034, a2);
  sw(t9 + 0x0038, a3);
  sw(t9 + 0x003C, t0);
label80023C04:
  sw(t9 + 0x0000, at);
  t9 += 68; // 0x0044
  v0 = lbu(ra + 0x0025);
  v1 = lbu(ra + 0x001A);
  a0 = lbu(ra + 0x0020);
  v1 = v1 << 2;
  v1 += fp;
  v1 = lw(v1 + 0x0038);
  a0 = a0 << 2;
  a0 += v1;
  a0 = lw(a0 + 0x0024);
  a1 = lbu(v1 + 0x0009);
  at = a0 >> 10;
  at = at & 0x1000;
  v0 = v0 << 8;
  at += v0;
  a0 = a0 << 11;
  a0 = a0 >> 10;
  a1 = a1 << 16;
  at += a1;
  a1 = lbu(v1 + 0x0008);
  a2 = lw(v1 + 0x0010);
  a1 = a1 << 2;
  a2 += a1;
  t0 = lw(a0 + 0x000C);
  a3 = lw(a0 + 0x0008);
  t1 = a0 + 24; // 0x0018
  t0 = t0 >> 16;
  t0 += t1;
  a3 = a3 >> 10;
  a3 = a3 & 0x3FF;
  a3 += t1;
  sw(t9 + 0x0024, a2);
  sw(t9 + 0x0028, a3);
  temp = v0 == 0;
  sw(t9 + 0x002C, t0);
  if (temp) goto label80023D08;
  v1 = lbu(ra + 0x001B);
  a0 = lbu(ra + 0x0021);
  v1 = v1 << 2;
  v1 += fp;
  v1 = lw(v1 + 0x0038);
  a0 = a0 << 2;
  a0 += v1;
  a0 = lw(a0 + 0x0024);
  a1 = a0 >> 9;
  a1 = a1 & 0x2000;
  at += a1;
  a0 = a0 << 11;
  a0 = a0 >> 10;
  a1 = lbu(v1 + 0x0008);
  a2 = lw(v1 + 0x0010);
  a1 = a1 << 2;
  a2 += a1;
  t0 = lw(a0 + 0x000C);
  a3 = lw(a0 + 0x0008);
  t1 = a0 + 24; // 0x0018
  t0 = t0 >> 16;
  t0 += t1;
  a3 = a3 >> 10;
  a3 = a3 & 0x3FF;
  a3 += t1;
  sw(t9 + 0x0034, a2);
  sw(t9 + 0x0038, a3);
  sw(t9 + 0x003C, t0);
label80023D08:
  sw(t9 + 0x0000, at);
  t9 += 68; // 0x0044
  v0 = lbu(ra + 0x0026);
  v1 = lbu(ra + 0x001C);
  a0 = lbu(ra + 0x0022);
  v1 = v1 << 2;
  v1 += fp;
  v1 = lw(v1 + 0x0038);
  a0 = a0 << 2;
  a0 += v1;
  a0 = lw(a0 + 0x0024);
  a1 = lbu(v1 + 0x000A);
  at = a0 >> 11;
  at = at & 0x1000;
  v0 = v0 << 8;
  at += v0;
  a0 = a0 << 11;
  a0 = a0 >> 10;
  a1 = a1 << 16;
  at += a1;
  a1 = lbu(v1 + 0x0009);
  a2 = lw(v1 + 0x0010);
  a1 = a1 << 2;
  a2 += a1;
  t0 = lw(a0 + 0x000C);
  a3 = lw(a0 + 0x0008);
  t1 = a0 + 24; // 0x0018
  t0 = t0 & 0xFFFF;
  t0 += t1;
  a3 = a3 & 0x3FF;
  a3 += t1;
  sw(t9 + 0x0024, a2);
  sw(t9 + 0x0028, a3);
  temp = v0 == 0;
  sw(t9 + 0x002C, t0);
  if (temp) goto label80023E04;
  v1 = lbu(ra + 0x001D);
  a0 = lbu(ra + 0x0023);
  v1 = v1 << 2;
  v1 += fp;
  v1 = lw(v1 + 0x0038);
  a0 = a0 << 2;
  a0 += v1;
  a0 = lw(a0 + 0x0024);
  a1 = a0 >> 10;
  a1 = a1 & 0x2000;
  at += a1;
  a0 = a0 << 11;
  a0 = a0 >> 10;
  a1 = lbu(v1 + 0x0009);
  a2 = lw(v1 + 0x0010);
  a1 = a1 << 2;
  a2 += a1;
  t0 = lw(a0 + 0x000C);
  a3 = lw(a0 + 0x0008);
  t1 = a0 + 24; // 0x0018
  t0 = t0 & 0xFFFF;
  t0 += t1;
  a3 = a3 & 0x3FF;
  a3 += t1;
  sw(t9 + 0x0034, a2);
  sw(t9 + 0x0038, a3);
  sw(t9 + 0x003C, t0);
label80023E04:
  sw(t9 + 0x0000, at);
  sw(t9 + 0x0044, 0);
  temp = (int32_t)t6 > 0;
  s4 = t3 << 22;
  if (temp) goto label80023E3C;
  s4 = (int32_t)s4 >> 21;
  s3 = t3 << 11;
  s3 = (int32_t)s3 >> 21;
  s2 = (int32_t)t3 >> 21;
  s1 = t2 << 22;
  s1 = (int32_t)s1 >> 21;
  s0 = t2 << 11;
  s0 = (int32_t)s0 >> 21;
  t7 = (int32_t)t2 >> 21;
  goto label80023EE0;
label80023E3C:
  cop2.IR0 = t6;
  a1 = (int32_t)t2 >> 21;
  a2 = t2 << 11;
  a2 = (int32_t)a2 >> 21;
  a3 = t2 << 22;
  a3 = (int32_t)a3 >> 21;
  cop2.IR1 = a1;
  cop2.IR2 = a2;
  cop2.IR3 = a3;
  a1 = (int32_t)t4 >> 21;
  a2 = t4 << 11;
  a2 = (int32_t)a2 >> 21;
  a3 = t4 << 22;
  a3 = (int32_t)a3 >> 21;
  cop2.RFC = a1;
  cop2.GFC = a2;
  cop2.BFC = a3;
  a1 = (int32_t)t3 >> 21;
  a2 = t3 << 11;
  INTPL();
  a2 = (int32_t)a2 >> 21;
  a3 = t3 << 22;
  a3 = (int32_t)a3 >> 21;
  t0 = (int32_t)t5 >> 21;
  t1 = t5 << 11;
  t1 = (int32_t)t1 >> 21;
  t2 = t5 << 22;
  t2 = (int32_t)t2 >> 21;
  cop2.IR1 = a1;
  cop2.IR2 = a2;
  cop2.IR3 = a3;
  cop2.RFC = t0;
  cop2.GFC = t1;
  cop2.BFC = t2;
  t7 = cop2.MAC1;
  s0 = cop2.MAC2;
  s1 = cop2.MAC3;
  INTPL();
  s2 = cop2.MAC1;
  s3 = cop2.MAC2;
  s4 = cop2.MAC3;
label80023EE0:
  at = lw(ra + 0x0000);
  v0 = lw(ra + 0x0004);
  v1 = lw(ra + 0x0008);
  a0 = lw(sp + 0x0028);
  a1 = lw(sp + 0x002C);
  a2 = lw(sp + 0x0030);
  at -= a0;
  v0 = a1 - v0;
  v1 = a2 - v1;
  a0 = lw(sp + 0x0000);
  a1 = lw(sp + 0x0004);
  a2 = lw(sp + 0x0008);
  a3 = lw(sp + 0x000C);
  t0 = lw(sp + 0x0010);
  cop2.RTM0 = a0;
  cop2.RTM1 = a1;
  cop2.RTM2 = a2;
  cop2.RTM3 = a3;
  cop2.RTM4 = t0;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  cop2.IR3 = at;
  cop2.IR1 = v0;
  cop2.IR2 = v1;
  MVMVA(SF_ON, MX_RT, V_IR, CV_NONE, LM_OFF);
  v1 = cop2.MAC3;
  v0 = cop2.MAC2;
  at = cop2.MAC1;
  a0 = v1 - 16384; // 0xFFFFC000
  temp = (int32_t)a0 >= 0;
  a0 = v1 + 2048; // 0x0800
  if (temp) goto label800258B0;
  a1 = a0 - at;
  temp = (int32_t)a1 <= 0;
  a1 = a0 + at;
  if (temp) goto label800258B0;
  temp = (int32_t)a1 <= 0;
  a1 = a0 - v0;
  if (temp) goto label800258B0;
  temp = (int32_t)a1 <= 0;
  a1 = a0 + v0;
  if (temp) goto label800258B0;
  temp = (int32_t)a1 <= 0;
  if (temp) goto label800258B0;
  a0 = lbu(ra + 0x0027);
  a1 = (int32_t)v1 >> 7;
  a1 -= a0;
  temp = (int32_t)a1 >= 0;
  if (temp) goto label80023FA4;
  a1 = 0;
label80023FA4:
  a2 = s7 - s6;
  temp = (int32_t)a2 >= 0;
  a2 = 512; // 0x0200
  if (temp) goto label80023FB4;
  s7 = s6;
label80023FB4:
  a2 = a2 << s7;
  a2 = v1 - a2;
  temp = (int32_t)a2 >= 0;
  if (temp) goto label80023FC8;
  a2 = 0;
label80023FC8:
  cop2.RBK = s7;
  cop2.GBK = a1;
  cop2.BBK = a2;
  t9 = 0x8006FCF4 + 0x1900;
  sw(t9 + 0xCC, 0);
  sw(t9 + 0xD0, 0);
  sw(t9 + 0xD4, 0);
  sw(t9 + 0xD8, t7);
  sw(t9 + 0xDC, s0);
  sw(t9 + 0xE0, s1);
  sw(t9 + 0xE4, s2);
  sw(t9 + 0xE8, s3);
  sw(t9 + 0xEC, s4);
  cop2.TRX = at;
  cop2.TRY = v0;
  cop2.TRZ = v1;
  sw(t9 + 0x04, at);
  sw(t9 + 0x08, v0);
  sw(t9 + 0x0C, v1);
  a3 = cop2.RTM0;
  t0 = cop2.RTM1;
  t1 = cop2.RTM2;
  t2 = cop2.RTM3;
  t3 = cop2.RTM4;
  t4 = lw(ra + 0x000C);
  t5 = spyro_sin_lut;
  t6 = spyro_cos_lut;
  t8 = 0x80024054; // &0xAF270010
  goto label800241E0;
label80024054:
  sw(t9 + 0x0010, a3);
  sw(t9 + 0x0014, t0);
  sw(t9 + 0x0018, t1);
  sw(t9 + 0x001C, t2);
  sw(t9 + 0x0020, t3);
  s0 = -s0;
  s1 = -s1;
  s1 = s1 << 16;
  s0 += s1;
  cop2.VXY0 = s0;
  cop2.VZ0 = t7;
  s3 = -s3;
  s4 = -s4;
  RTPS();
  s4 = s4 << 16;
  s3 += s4;
  t7 = cop2.IR1;
  cop2.VXY0 = s3;
  cop2.VZ0 = s2;
  s0 = cop2.IR2;
  s1 = cop2.IR3;
  RTPS();
  sw(t9 + 0x0048, t7);
  sw(t9 + 0x004C, s0);
  sw(t9 + 0x0050, s1);
  s2 = cop2.IR1;
  s3 = cop2.IR2;
  s4 = cop2.IR3;
  sw(t9 + 0x008C, s2);
  sw(t9 + 0x0090, s3);
  sw(t9 + 0x0094, s4);
  t7 = a3;
  s0 = t0;
  s1 = t1;
  s2 = t2;
  s3 = t3;
  t4 = lw(ra + 0x0010);
  t8 = 0x800240F8; // &0x3C018008
  temp = (int32_t)t4 > 0;
  if (temp) goto label800241E0;
label800240F8:
  at = 0x800786C8;
  v0 = lbu(at + 0x009A);
  temp = v0 == 0;
  if (temp) goto label80024124;
  sw(at + 0x00B8, a3);
  sw(at + 0x00BC, t0);
  sw(at + 0x00C0, t1);
  sw(at + 0x00C4, t2);
  sw(at + 0x00C8, t3);
label80024124:
  temp = t4 == 0;
  if (temp) goto label8002418C;
  sw(t9 + 0x0054, a3);
  sw(t9 + 0x0058, t0);
  sw(t9 + 0x005C, t1);
  sw(t9 + 0x0060, t2);
  sw(t9 + 0x0064, t3);
  at = 128; // 0x0080
  sb(t9 + 0x0047, at);
  a3 = t7;
  t0 = s0;
  t1 = s1;
  t2 = s2;
  t3 = s3;
  cop2.RTM0 = t7;
  cop2.RTM1 = s0;
  cop2.RTM2 = s1;
  cop2.RTM3 = s2;
  cop2.RTM4 = s3;
  sw(t9 + 0x0098, a3);
  sw(t9 + 0x009C, t0);
  sw(t9 + 0x00A0, t1);
  sw(t9 + 0x00A4, t2);
  sw(t9 + 0x00A8, t3);
  at = 128; // 0x0080
  sb(t9 + 0x008B, at);
label8002418C:
  t4 = lw(ra + 0x0014);
  t8 = 0x800241A8; // &0xAF270098
  temp = (int32_t)t4 > 0;
  if (temp) goto label800241E0;
  goto label800241D8;
label800241A8:
  sw(t9 + 0x0098, a3);
  sw(t9 + 0x009C, t0);
  sw(t9 + 0x00A0, t1);
  sw(t9 + 0x00A4, t2);
  sw(t9 + 0x00A8, t3);
  at = 128; // 0x0080
  sb(t9 + 0x008B, at);
  cop2.RTM0 = t7;
  cop2.RTM1 = s0;
  cop2.RTM2 = s1;
  cop2.RTM3 = s2;
  cop2.RTM4 = s3;
label800241D8:
  hi=gp;
  goto label80024404;
label800241E0:
  at = t4 >> 15;
  at = at & 0x1FE;
  temp = at == 0;
  v0 = at + t6;
  if (temp) goto label80024290;
  at += t5;
  v0 = lhu(v0 + 0x0000);
  at = lhu(at + 0x0000);
  v0 = v0 & 0xFFFF;
  cop2.VXY0 = v0;
  cop2.VZ0 = at;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  at = -at;
  at = at & 0xFFFF;
  a0 = cop2.IR1;
  a1 = cop2.IR2;
  a2 = cop2.IR3;
  cop2.VXY0 = at;
  cop2.VZ0 = v0;
  v1 = 0xFFFF0000;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  a3 = a3 & v1;
  a0 = a0 & 0xFFFF;
  a3 += a0;
  t2 = t2 & v1;
  a2 = a2 & 0xFFFF;
  t2 += a2;
  a1 = a1 << 16;
  t1 = t1 & 0xFFFF;
  at = cop2.IR1;
  v0 = cop2.IR2;
  v1 = cop2.IR3;
  at = at & 0xFFFF;
  t0 = at + a1;
  v0 = v0 << 16;
  t1 += v0;
  t3 = v1 & 0xFFFF;
  cop2.RTM0 = a3;
  cop2.RTM1 = t0;
  cop2.RTM2 = t1;
  cop2.RTM3 = t2;
  cop2.RTM4 = t3;
label80024290:
  at = t4 & 0xFF00;
  temp = at == 0;
  at = at >> 8;
  if (temp) goto label80024344;
  at = at << 1;
  v0 = at + t6;
  at += t5;
  v0 = lhu(v0 + 0x0000);
  at = lhu(at + 0x0000);
  v1 = v0 << 16;
  cop2.VXY0 = v1;
  cop2.VZ0 = at;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  v1 = at << 16;
  v1 = -v1;
  a0 = cop2.IR1;
  a1 = cop2.IR2;
  a2 = cop2.IR3;
  cop2.VXY0 = v1;
  cop2.VZ0 = v0;
  v1 = 0xFFFF0000;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  a3 = a3 & 0xFFFF;
  a0 = a0 << 16;
  a3 += a0;
  t2 = t2 & 0xFFFF;
  a2 = a2 << 16;
  t2 += a2;
  a1 = a1 & 0xFFFF;
  t0 = t0 & v1;
  at = cop2.IR1;
  v0 = cop2.IR2;
  v1 = cop2.IR3;
  at = at & 0xFFFF;
  t0 += at;
  v0 = v0 << 16;
  t1 = v0 + a1;
  t3 = v1 & 0xFFFF;
  cop2.RTM0 = a3;
  cop2.RTM1 = t0;
  cop2.RTM2 = t1;
  cop2.RTM3 = t2;
  cop2.RTM4 = t3;
label80024344:
  at = t4 & 0xFF;
  temp = at == 0;
  at = at << 1;
  if (temp) goto label800243FC;
  v0 = at + t6;
  at += t5;
  v0 = lhu(v0 + 0x0000);
  at = lhu(at + 0x0000);
  cop2.VZ0 = 0;
  v1 = at << 16;
  v1 += v0;
  cop2.VXY0 = v1;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  at = -at;
  at = at & 0xFFFF;
  v0 = v0 << 16;
  at += v0;
  a0 = cop2.IR1;
  a1 = cop2.IR2;
  a2 = cop2.IR3;
  cop2.VXY0 = at;
  cop2.VZ0 = 0;
  v1 = 0xFFFF0000;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  a1 = a1 << 16;
  t0 = t0 & 0xFFFF;
  t0 += a1;
  a0 = a0 & 0xFFFF;
  a2 = a2 & 0xFFFF;
  t1 = t1 & v1;
  at = cop2.IR1;
  v0 = cop2.IR2;
  v1 = cop2.IR3;
  at = at << 16;
  a3 = at + a0;
  v1 = v1 << 16;
  t2 = v1 + a2;
  v0 = v0 & 0xFFFF;
  t1 += v0;
  cop2.RTM0 = a3;
  cop2.RTM1 = t0;
  cop2.RTM2 = t1;
  cop2.RTM3 = t2;
  cop2.RTM4 = t3;
label800243FC:
  temp = t8;
  switch (temp)
  {
  case 0x80024054:
    goto label80024054;
    break;
  case 0x800240F8:
    goto label800240F8;
    break;
  case 0x800241A8:
    goto label800241A8;
    break;
  default:
    JR(temp, 0x800243FC);
    return;
  }
label80024404:
  at = 0x8006FCF4;
  t8 = at + 6400; // 0x1900
  s1 = lw(t8 + 0x0000);
  fp = 0x1F800000;
  sp = at + 2304; // 0x0900
  gp = 0x8006D614; // &0xFE9FD3FA
  t9 = spyro_position;
  t9 = lw(t9 + 0x0030);
label80024430:
  at = lw(t8 + 0x0004);
  v0 = lw(t8 + 0x0008);
  v1 = lw(t8 + 0x000C);
  cop2.TRX = at;
  cop2.TRY = v0;
  cop2.TRZ = v1;
  temp = (int32_t)s1 >= 0;
  if (temp) goto label80024478;
  at = lw(t8 + 0x0010);
  v0 = lw(t8 + 0x0014);
  v1 = lw(t8 + 0x0018);
  a0 = lw(t8 + 0x001C);
  a1 = lw(t8 + 0x0020);
  cop2.RTM0 = at;
  cop2.RTM1 = v0;
  cop2.RTM2 = v1;
  cop2.RTM3 = a0;
  cop2.RTM4 = a1;
label80024478:
  at = s1 >> 14;
  at = at & 0x3FF;
  ra = 0x1F7FFFFC + at;
  lo=ra;
  ra = fp + 4; // 0x0004
  t0 = s1 & 0x1000;
  at = lw(t8 + 0x0024);
  v0 = lw(t8 + 0x0028);
  v1 = lw(t8 + 0x002C);
  a1 = 0;
  a2 = 0;
  a3 = 0;
  s7 = s1 & 0xFFF;
  temp = s7 == 0;
  s0 = s1 & 0x2000;
  if (temp) goto label800244D8;
  t1 = lw(t8 + 0x0034);
  t2 = lw(t8 + 0x0038);
  t3 = lw(t8 + 0x003C);
  t5 = 0;
  t6 = 0;
  t7 = 0;
  goto label800246B4;
label800244D8:
  temp = (int32_t)t0 > 0;
  at += 4; // 0x0004
  if (temp) goto label80024580;
  RTPS();
  s1 = lbu(v0 + 0x0000);
  s2 = lw(at - 0x0004); // 0xFFFFFFFC
  t0 = s1 & 0x1;
  s1 = s1 << 1;
  s1 = s1 & 0x1FC;
  s1 += gp;
  s1 = lw(s1 + 0x0000);
  v0++;
  sp += 16; // 0x0010
  fp += 4; // 0x0004
  temp = t9 == 0;
  s1 += s2;
  if (temp) goto label80024524;
  s4 = cop2.VXY0;
  s5 = cop2.VZ0;
  sw(sp - 0x000C, s4); // 0xFFFFFFF4
  sw(sp - 0x0008, s5); // 0xFFFFFFF8
label80024524:
  s2 = (int32_t)s1 >> 21;
  a1 += s2;
  s2 = s1 << 11;
  s2 = (int32_t)s2 >> 21;
  a2 -= s2;
  s2 = s1 << 22;
  s2 = (int32_t)s2 >> 21;
  a3 -= s2;
  s1 = a3 << 16;
  s1 += a2;
  cop2.VXY0 = s1;
  cop2.VZ0 = a1;
  s3 = cop2.SZ3;
  s1 = cop2.SXY2;
  sw(sp - 0x0010, s3); // 0xFFFFFFF0
  temp = fp != ra;
  sw(fp - 0x0004, s1); // 0xFFFFFFFC
  if (temp) goto label800244D8;
  s1=lo;
  temp = s1 == ra;
  ra = s1;
  if (temp) goto label80024A14;
  sp -= 16; // 0xFFFFFFF0
  fp -= 4; // 0xFFFFFFFC
  goto label800244D8;
label80024580:
  RTPS();
  s1 = lh(v1 + 0x0000);
  v1 += 2; // 0x0002
  sp += 16; // 0x0010
  fp += 4; // 0x0004
  temp = (int32_t)s1 < 0;
  t0 = s1 & 0x4000;
  if (temp) goto label80024634;
  s2 = lw(at - 0x0004); // 0xFFFFFFFC
  s3 = s1 << 18;
  s3 = (int32_t)s3 >> 25;
  a1 += s3;
  s3 = s1 << 23;
  s3 = (int32_t)s3 >> 25;
  a2 -= s3;
  s3 = s1 << 28;
  s3 = (int32_t)s3 >> 25;
  a3 -= s3;
  s1 = cop2.SZ3;
  temp = t9 == 0;
  sw(sp - 0x0010, s1); // 0xFFFFFFF0
  if (temp) goto label800245E0;
  s3 = cop2.VXY0;
  s4 = cop2.VZ0;
  sw(sp - 0x000C, s3); // 0xFFFFFFF4
  sw(sp - 0x0008, s4); // 0xFFFFFFF8
label800245E0:
  s1 = (int32_t)s2 >> 21;
  a1 += s1;
  s1 = s2 << 11;
  s1 = (int32_t)s1 >> 21;
  a2 -= s1;
  s1 = s2 << 22;
  s1 = (int32_t)s1 >> 21;
  a3 -= s1;
  s1 = a3 << 16;
  s1 += a2;
  cop2.VZ0 = a1;
  cop2.VXY0 = s1;
  s1 = cop2.SXY2;
  temp = fp != ra;
  sw(fp - 0x0004, s1); // 0xFFFFFFFC
  if (temp) goto label800244D8;
  s1=lo;
  temp = s1 == ra;
  ra = s1;
  if (temp) goto label80024A14;
  sp -= 16; // 0xFFFFFFF0
  fp -= 4; // 0xFFFFFFFC
  goto label800244D8;
label80024634:
  s2 = lhu(v1 + 0x0000);
  v1 += 2; // 0x0002
  s3 = s1 << 18;
  a1 = (int32_t)s3 >> 21;
  s3 = s1 << 28;
  s1 = cop2.SZ3;
  temp = t9 == 0;
  sw(sp - 0x0010, s1); // 0xFFFFFFF0
  if (temp) goto label80024664;
  s1 = cop2.VXY0;
  s4 = cop2.VZ0;
  sw(sp - 0x000C, s1); // 0xFFFFFFF4
  sw(sp - 0x0008, s4); // 0xFFFFFFF8
label80024664:
  s1 = s2 << 12;
  s1 = s1 | s3;
  a2 = (int32_t)s1 >> 21;
  a2 = -a2;
  s3 = s2 << 22;
  a3 = (int32_t)s3 >> 21;
  a3 = -a3;
  s1 = a3 << 16;
  s1 += a2;
  cop2.VXY0 = s1;
  cop2.VZ0 = a1;
  s1 = cop2.SXY2;
  temp = fp != ra;
  sw(fp - 0x0004, s1); // 0xFFFFFFFC
  if (temp) goto label800244D8;
  s1=lo;
  temp = s1 == ra;
  ra = s1;
  if (temp) goto label80024A14;
  sp -= 16; // 0xFFFFFFF0
  fp -= 4; // 0xFFFFFFFC
  goto label800244D8;
label800246B4:
  temp = (int32_t)t0 > 0;
  at += 4; // 0x0004
  if (temp) goto label800247D8;
  RTPS();
  s1 = lbu(v0 + 0x0000);
  s2 = lw(at - 0x0004); // 0xFFFFFFFC
  t0 = s1 & 0x1;
  s1 = s1 << 1;
  s1 = s1 & 0x1FC;
  s1 += gp;
  s1 = lw(s1 + 0x0000);
  v0++;
  sp += 16; // 0x0010
  fp += 4; // 0x0004
  t1 += 4; // 0x0004
  s1 += s2;
  s3 = (int32_t)s1 >> 21;
  a1 += s3;
  s2 = lw(t1 - 0x0004); // 0xFFFFFFFC
  s3 = s1 << 11;
  s3 = (int32_t)s3 >> 21;
  a2 -= s3;
  s3 = s1 << 22;
  s3 = (int32_t)s3 >> 21;
  temp = (int32_t)s0 > 0;
  a3 -= s3;
  if (temp) goto label80024848;
label80024718:
  s3 = lbu(t2 + 0x0000);
  cop2.IR0 = s7;
  s1 = s3 << 1;
  s1 = s1 & 0x1FC;
  s1 += gp;
  s1 = lw(s1 + 0x0000);
  cop2.IR1 = a1;
  cop2.IR2 = a2;
  cop2.IR3 = a3;
  s1 += s2;
  s2 = (int32_t)s1 >> 21;
  t5 += s2;
  s2 = s1 << 11;
  s2 = (int32_t)s2 >> 21;
  t6 -= s2;
  s2 = s1 << 22;
  s2 = (int32_t)s2 >> 21;
  t7 -= s2;
  cop2.RFC = t5;
  cop2.GFC = t6;
  cop2.BFC = t7;
  s1 = cop2.SXY2;
  s2 = cop2.SZ3;
  INTPL();
  sw(fp - 0x0004, s1); // 0xFFFFFFFC
  sw(sp - 0x0010, s2); // 0xFFFFFFF0
  s0 = s3 & 0x1;
  temp = t9 == 0;
  t2++;
  if (temp) goto label8002479C;
  s4 = cop2.VXY0;
  s5 = cop2.VZ0;
  sw(sp - 0x000C, s4); // 0xFFFFFFF4
  sw(sp - 0x0008, s5); // 0xFFFFFFF8
label8002479C:
  s1 = cop2.MAC1;
  s2 = cop2.MAC2;
  s3 = cop2.MAC3;
  cop2.VZ0 = s1;
  s3 = s3 << 16;
  s2 += s3;
  cop2.VXY0 = s2;
  temp = fp != ra;
  if (temp) goto label800246B4;
  s1=lo;
  temp = s1 == ra;
  ra = s1;
  if (temp) goto label80024A14;
  sp -= 16; // 0xFFFFFFF0
  fp -= 4; // 0xFFFFFFFC
  goto label800246B4;
label800247D8:
  RTPS();
  s1 = lh(v1 + 0x0000);
  v1 += 2; // 0x0002
  sp += 16; // 0x0010
  fp += 4; // 0x0004
  temp = (int32_t)s1 < 0;
  t0 = s1 & 0x4000;
  if (temp) goto label8002491C;
  s3 = lw(at - 0x0004); // 0xFFFFFFFC
  s2 = s1 << 18;
  s2 = (int32_t)s2 >> 25;
  a1 += s2;
  s2 = s1 << 23;
  s2 = (int32_t)s2 >> 25;
  a2 -= s2;
  s2 = s1 << 28;
  s2 = (int32_t)s2 >> 25;
  a3 -= s2;
  s2 = lw(t1 + 0x0000);
  s1 = (int32_t)s3 >> 21;
  a1 += s1;
  s1 = s3 << 11;
  s1 = (int32_t)s1 >> 21;
  a2 -= s1;
  s1 = s3 << 22;
  s1 = (int32_t)s1 >> 21;
  a3 -= s1;
  temp = s0 == 0;
  t1 += 4; // 0x0004
  if (temp) goto label80024718;
label80024848:
  s1 = lh(t3 + 0x0000);
  s3 = (int32_t)s2 >> 21;
  t5 += s3;
  s3 = s2 << 11;
  s3 = (int32_t)s3 >> 21;
  t6 -= s3;
  s3 = s2 << 22;
  s3 = (int32_t)s3 >> 21;
  t7 -= s3;
  temp = (int32_t)s1 < 0;
  s0 = s1 & 0x4000;
  if (temp) goto label80024960;
  cop2.IR0 = s7;
  cop2.IR1 = a1;
  cop2.IR2 = a2;
  cop2.IR3 = a3;
  s2 = s1 << 18;
  s2 = (int32_t)s2 >> 25;
  t5 += s2;
  s2 = s1 << 23;
  s2 = (int32_t)s2 >> 25;
  t6 -= s2;
  s2 = s1 << 28;
  s2 = (int32_t)s2 >> 25;
  t7 -= s2;
  cop2.RFC = t5;
  cop2.GFC = t6;
  cop2.BFC = t7;
  s1 = cop2.SXY2;
  s3 = cop2.SZ3;
  INTPL();
  sw(fp - 0x0004, s1); // 0xFFFFFFFC
  sw(sp - 0x0010, s3); // 0xFFFFFFF0
  temp = t9 == 0;
  t3 += 2; // 0x0002
  if (temp) goto label800248E0;
  s4 = cop2.VXY0;
  s5 = cop2.VZ0;
  sw(sp - 0x000C, s4); // 0xFFFFFFF4
  sw(sp - 0x0008, s5); // 0xFFFFFFF8
label800248E0:
  s1 = cop2.MAC1;
  s2 = cop2.MAC2;
  s3 = cop2.MAC3;
  cop2.VZ0 = s1;
  s3 = s3 << 16;
  s2 += s3;
  cop2.VXY0 = s2;
  temp = fp != ra;
  if (temp) goto label800246B4;
  s1=lo;
  temp = s1 == ra;
  ra = s1;
  if (temp) goto label80024A14;
  sp -= 16; // 0xFFFFFFF0
  fp -= 4; // 0xFFFFFFFC
  goto label800246B4;
label8002491C:
  s2 = lhu(v1 + 0x0000);
  v1 += 2; // 0x0002
  s3 = s1 << 18;
  a1 = (int32_t)s3 >> 21;
  s3 = s1 << 28;
  s1 = s2 << 12;
  s1 = s1 | s3;
  a2 = (int32_t)s1 >> 21;
  a2 = -a2;
  s3 = s2 << 22;
  s2 = lw(t1 + 0x0000);
  a3 = (int32_t)s3 >> 21;
  a3 = -a3;
  temp = s0 == 0;
  t1 += 4; // 0x0004
  if (temp) goto label80024718;
  goto label80024848;
label80024960:
  s2 = lhu(t3 + 0x0002);
  t3 += 4; // 0x0004
  s3 = s1 << 18;
  t5 = (int32_t)s3 >> 21;
  s3 = s1 << 28;
  cop2.IR0 = s7;
  cop2.IR1 = a1;
  cop2.IR2 = a2;
  cop2.IR3 = a3;
  s1 = s2 << 12;
  s1 = s1 | s3;
  t6 = (int32_t)s1 >> 21;
  t6 = -t6;
  s3 = s2 << 22;
  t7 = (int32_t)s3 >> 21;
  t7 = -t7;
  cop2.RFC = t5;
  cop2.GFC = t6;
  cop2.BFC = t7;
  s1 = cop2.SXY2;
  s3 = cop2.SZ3;
  INTPL();
  sw(fp - 0x0004, s1); // 0xFFFFFFFC
  sw(sp - 0x0010, s3); // 0xFFFFFFF0
  temp = t9 == 0;
  if (temp) goto label800249D8;
  s4 = cop2.VXY0;
  s5 = cop2.VZ0;
  sw(sp - 0x000C, s4); // 0xFFFFFFF4
  sw(sp - 0x0008, s5); // 0xFFFFFFF8
label800249D8:
  s1 = cop2.MAC1;
  s2 = cop2.MAC2;
  s3 = cop2.MAC3;
  cop2.VZ0 = s1;
  s3 = s3 << 16;
  s2 += s3;
  cop2.VXY0 = s2;
  temp = fp != ra;
  if (temp) goto label800246B4;
  s1=lo;
  temp = s1 == ra;
  ra = s1;
  if (temp) goto label80024A14;
  sp -= 16; // 0xFFFFFFF0
  fp -= 4; // 0xFFFFFFFC
  goto label800246B4;
label80024A14:
  RTPS();
  sp += 16; // 0x0010
  fp += 4; // 0x0004
  t8 += 68; // 0x0044
  s1 = lw(t8 + 0x0000);
  at = cop2.SXY2;
  v0 = cop2.SZ3;
  sw(fp - 0x0004, at); // 0xFFFFFFFC
  temp = t9 == 0;
  sw(sp - 0x0010, v0); // 0xFFFFFFF0
  if (temp) goto label80024A4C;
  at = cop2.VXY0;
  v0 = cop2.VZ0;
  sw(sp - 0x000C, at); // 0xFFFFFFF4
  sw(sp - 0x0008, v0); // 0xFFFFFFF8
label80024A4C:
  temp = s1 != 0;
  if (temp) goto label80024430;
  temp = t9 == 0;
  ra=hi;
  if (temp) goto label80024B60;
  at = spyro_position;
  t8 = lw(at + 0x0008);
  v0 = lw(at + 0x0034);
  v1 = lw(at + 0x0038);
  a0 = lw(at + 0x003C);
  a1 = lw(at + 0x0040);
  a2 = lw(at + 0x0044);
  cop2.RTM0 = v0;
  cop2.RTM1 = v1;
  cop2.RTM2 = a0;
  cop2.RTM3 = a1;
  cop2.RTM4 = a2;
  t8 -= t9;
  fp = 0;
  gp = 0x8006FCF4;
  gp += 2304; // 0x0900
label80024AA4:
  at = fp + ra;
  at = lbu(at + 0x0008);
  v0 = fp << 2;
  v1 = fp << 3;
  v0 += v1;
  v1 = 0x8006FCF4;
  sp = v1 + 2304; // 0x0900
  v1 += 6400; // 0x1900
  v1 += v0;
  a0 = lw(v1 + 0x00CC);
  v0 = lw(v1 + 0x00D0);
  v1 = lw(v1 + 0x00D4);
  v0 = -v0;
  v1 = -v1;
  cop2.IR1 = v0;
  cop2.IR2 = v1;
  cop2.IR3 = a0;
  at = at << 4;
  sp += at;
  MVMVA(SF_ON, MX_RT, V_IR, CV_NONE, LM_OFF);
  v0 = cop2.MAC1;
  v1 = cop2.MAC2;
  a0 = cop2.MAC3;
  cop2.TRX = v0;
  cop2.TRY = v1;
  cop2.TRZ = a0;
  at = lw(gp + 0x0004);
  v0 = lw(gp + 0x0008);
  cop2.VXY0 = at;
  cop2.VZ0 = v0;
  gp += 16; // 0x0010
label80024B28:
  MVMVA(SF_ON, MX_RT, V_V0, CV_TR, LM_OFF);
  at = lw(gp + 0x0004);
  v0 = lw(gp + 0x0008);
  v1 = cop2.MAC2;
  cop2.VXY0 = at;
  cop2.VZ0 = v0;
  v1 = t8 - v1;
  sw(gp - 0x000C, v1); // 0xFFFFFFF4
  temp = gp != sp;
  gp += 16; // 0x0010
  if (temp) goto label80024B28;
  gp -= 16; // 0xFFFFFFF0
  at = fp - 2; // 0xFFFFFFFE
  temp = at != 0;
  fp++;
  if (temp) goto label80024AA4;
label80024B60:
  a0 = spyro_position;
  a0 = lw(a0 + 0x0028);
  at = lw(ra + 0x0018);
  a1 = a0 >> 24;
  temp = a1 == 0;
  hi=at;
  if (temp) goto label80024C30;
  v0 = lhu(ra + 0x0002);
  v1 = 0x8006FCF4;
  v1 += 7424; // 0x1D00
  hi=v1;
  a1 = a1 << 4;
  cop2.IR0 = a1;
  a1 = a0 << 4;
  a1 = a1 & 0xFF0;
  cop2.RFC = a1;
  a1 = a0 >> 4;
  a1 = a1 & 0xFF0;
  cop2.GFC = a1;
  a1 = a0 >> 12;
  a1 = a1 & 0xFF0;
  cop2.BFC = a1;
  a1 = lw(at + 0x0000);
  at += 4; // 0x0004
  a3 = a1 >> 12;
  a3 = a3 & 0xFF0;
  a2 = a1 >> 4;
  a2 = a2 & 0xFF0;
  a1 = a1 << 4;
  a1 = a1 & 0xFF0;
  cop2.IR1 = a1;
  cop2.IR2 = a2;
  cop2.IR3 = a3;
  v0 = v0 << 2;
  v0 += at;
label80024BF0:
  INTPL();
  a1 = lw(at + 0x0000);
  at += 4; // 0x0004
  a0 = cop2.RGB2;
  a3 = a1 >> 12;
  a3 = a3 & 0xFF0;
  a2 = a1 >> 4;
  a2 = a2 & 0xFF0;
  a1 = a1 << 4;
  a1 = a1 & 0xFF0;
  sw(v1 + 0x0000, a0);
  cop2.IR1 = a1;
  cop2.IR2 = a2;
  cop2.IR3 = a3;
  temp = at != v0;
  v1 += 4; // 0x0004
  if (temp) goto label80024BF0;
label80024C30:
  s2 = 0x00FFFFFF;
  s3 = 0x8006FCF4;

  s4 = s3 + 2304; // 0x0900
  s5 = s3;
  s6 = cop2.BBK;
  s7 = cop2.RBK;
  s6 = s6 << 2;
  s7 += 4; // 0x0004
  t8 = allocator1_ptr;
  t8 = lw(t8 + 0x0000);
  temp = t9 != 0;
  t9=hi;
  if (temp) goto label80025048;
  gp = 0x8006FCF4;
  gp += 2304; // 0x0900
  ra = lw(ra + 0x0014);
  sp = 0x1F800000;
  at = lw(ra + 0x0004);
  fp = lw(ra + 0x0000);
  ra += 4; // 0x0004
  fp += ra;
label80024C94:
  temp = (int32_t)at >= 0;
  s0 = lw(ra + 0x0004);
  if (temp) goto label80024EA8;
  temp = ra == fp;
  v0 = at >> 20;
  if (temp) goto label800257A0;
  v0 = v0 & 0x7FC;
  a2 = v0 + sp;
  v1 = at >> 11;
  v1 = v1 & 0x7FC;
  t5 = lw(ra + 0x0008);
  a3 = v1 + sp;
  a0 = at >> 2;
  a0 = a0 & 0x7FC;
  t0 = a0 + sp;
  t6 = at & 0x1;
  a2 = lw(a2 + 0x0000);
  a3 = lw(a3 + 0x0000);
  t0 = lw(t0 + 0x0000);
  cop2.SXY0 = a2;
  cop2.SXY1 = a3;
  cop2.SXY2 = t0;
  a1 = t5 & 0x7FC;
  t1 = a1 + sp;
  NCLIP();
  t1 = lw(t1 + 0x0000);
  sw(t8 + 0x0008, a2);
  sw(t8 + 0x0014, a3);
  sw(t8 + 0x0020, t0);
  sw(t8 + 0x002C, t1);
  temp = (int32_t)t6 > 0;
  t2 = s0 >> 17;
  if (temp) goto label80024D70;
  t6 = cop2.MAC0;
  cop2.SXY0 = t1;
  t2 = t2 & 0x7FC;
  t2 += t9;
  NCLIP();
  at = lw(ra + 0x0018);
  ra += 24; // 0x0018
  t5 = t5 >> 9;
  t5 = t5 & 0x7FC;
  t5 += t9;
  t3 = s0 >> 8;
  t7 = cop2.MAC0;
  temp = (int32_t)t6 < 0;
  t3 = t3 & 0x7FC;
  if (temp) goto label80024D60;
  temp = (int32_t)t7 <= 0;
  t3 += t9;
  if (temp) goto label80024C94;
  sw(t8 + 0x0008, t1);
  v0 = a1;
  t2 = t5;
  s1 = 16; // 0x0010
  goto label80024F3C;
label80024D60:
  temp = (int32_t)t7 > 0;
  t3 += t9;
  if (temp) goto label80024D98;
  s1 = 0;
  goto label80024F3C;
label80024D70:
  t2 = t2 & 0x7FC;
  t2 += t9;
  t5 = t5 >> 9;
  t5 = t5 & 0x7FC;
  t5 += t9;
  at = lw(ra + 0x0018);
  ra += 24; // 0x0018
  t3 = s0 >> 8;
  t3 = t3 & 0x7FC;
  t3 += t9;
label80024D98:
  t2 = lw(t2 + 0x0000);
  t4 = s0 << 1;
  t4 = t4 & 0x7FC;
  t4 += t9;
  t6 = 0x3C000000;
  v0 = v0 << 2;
  t3 = lw(t3 + 0x0000);
  t4 = lw(t4 + 0x0000);
  t5 = lw(t5 + 0x0000);
  t2 = t2 & s2;
  t2 += t6;
  v0 += gp;
  v1 = v1 << 2;
  v1 += gp;
  v0 = lhu(v0 + 0x0000);
  a0 = a0 << 2;
  a0 += gp;
  a1 = a1 << 2;
  a1 += gp;
  v0 -= s6;
  v1 = lhu(v1 + 0x0000);
  a0 = lhu(a0 + 0x0000);
  a1 = lhu(a1 + 0x0000);
  v0 += v1;
  v0 += a0;
  t6 = (int32_t)s0 >> 28;
  t6 = t6 << 2;
  t6 = t6 << s7;
  a2 = lw(ra - 0x000C); // 0xFFFFFFF4
  v0 += t6;
  v0 += a1;
  temp = (int32_t)v0 <= 0;
  v0 = v0 >> s7;
  if (temp) goto label80024C94;
  v0 = v0 << 3;
  v0 += s3;
  v1 = lw(v0 + 0x0000);
  a3 = lw(ra - 0x0008); // 0xFFFFFFF8
  t0 = lw(ra - 0x0004); // 0xFFFFFFFC
  sw(t8 + 0x0010, t3);
  sw(t8 + 0x001C, t4);
  sw(t8 + 0x0028, t5);
  s0 = s0 & 0x1;
  s0 = s0 << 25;
  t2 += s0;
  sw(t8 + 0x0004, t2);
  a0 = 0x0C000000;
  sw(t8 + 0x0000, a0);
  sw(t8 + 0x000C, a2);
  sw(t8 + 0x0018, a3);
  sw(t8 + 0x0024, t0);
  t1 = t0 >> 16;
  sw(t8 + 0x0030, t1);
  temp = v1 == 0;
  sw(v0 + 0x0000, t8);
  if (temp) goto label80024E84;
  sh(v1 + 0x0000, t8);
  a0 = t8 >> 16;
  sb(v1 + 0x0002, a0);
  t8 += 52; // 0x0034
  goto label80024C94;
label80024E84:
  v1 = s4 - v0;
  temp = (int32_t)v1 <= 0;
  v1 = s5 - v0;
  if (temp) goto label80024E94;
  s4 = v0;
label80024E94:
  temp = (int32_t)v1 >= 0;
  sw(v0 + 0x0004, t8);
  if (temp) goto label80024EA0;
  s5 = v0;
label80024EA0:
  t8 += 52; // 0x0034
  goto label80024C94;
label80024EA8:
  temp = ra == fp;
  v0 = at >> 20;
  if (temp) goto label800257A0;
  v0 = v0 & 0x7FC;
  a2 = v0 + sp;
  v1 = at >> 11;
  v1 = v1 & 0x7FC;
  a3 = v1 + sp;
  a0 = at >> 2;
  a0 = a0 & 0x7FC;
  t0 = a0 + sp;
  a2 = lw(a2 + 0x0000);
  a3 = lw(a3 + 0x0000);
  t0 = lw(t0 + 0x0000);
  cop2.SXY0 = a2;
  cop2.SXY1 = a3;
  cop2.SXY2 = t0;
  sw(t8 + 0x0008, a2);
  sw(t8 + 0x0014, a3);
  NCLIP();
  sw(t8 + 0x0020, t0);
  t2 = s0 >> 17;
  t2 = t2 & 0x7FC;
  t2 += t9;
  t6 = at & 0x1;
  temp = (int32_t)t6 > 0;
  s1 = 0;
  if (temp) goto label80024F28;
  t7 = cop2.MAC0;
  at = lw(ra + 0x0014);
  temp = (int32_t)t7 > 0;
  ra += 20; // 0x0014
  if (temp) goto label80024F30;
  goto label80024C94;
label80024F28:
  at = lw(ra + 0x0014);
  ra += 20; // 0x0014
label80024F30:
  t3 = s0 >> 8;
  t3 = t3 & 0x7FC;
  t3 += t9;
label80024F3C:
  t4 = s0 << 1;
  t2 = lw(t2 + 0x0000);
  t4 = t4 & 0x7FC;
  t4 += t9;
  t6 = 0x34000000;
  v0 = v0 << 2;
  v0 += gp;
  t3 = lw(t3 + 0x0000);
  t4 = lw(t4 + 0x0000);
  t2 = t2 & s2;
  t2 += t6;
  v1 = v1 << 2;
  v1 += gp;
  a0 = a0 << 2;
  v0 = lhu(v0 + 0x0000);
  v1 = lhu(v1 + 0x0000);
  a0 += gp;
  t6 = v0 >> 1;
  v0 += t6;
  v0 -= s6;
  t6 = (int32_t)s0 >> 28;
  a0 = lhu(a0 + 0x0000);
  t6 = t6 << 2;
  t6 = t6 << s7;
  v0 += v1;
  v1 = v1 >> 1;
  v0 += v1;
  a2 = lw(ra - 0x000C); // 0xFFFFFFF4
  v0 += a0;
  v0 += t6;
  temp = (int32_t)v0 <= 0;
  v0 = v0 >> s7;
  if (temp) goto label80024C94;
  v0 = v0 << 3;
  v0 += s3;
  v1 = lw(v0 + 0x0000);
  a3 = lw(ra - 0x0008); // 0xFFFFFFF8
  t0 = lw(ra - 0x0004); // 0xFFFFFFFC
  sw(t8 + 0x0010, t3);
  sw(t8 + 0x001C, t4);
  s0 = s0 & 0x1;
  s0 = s0 << 25;
  t2 += s0;
  sw(t8 + 0x0004, t2);
  temp = (int32_t)s1 <= 0;
  sw(t8 + 0x000C, a2);
  if (temp) goto label80024FF8;
  a2 = t0 >> 16;
  sh(t8 + 0x000C, a2);
label80024FF8:
  a0 = 0x09000000;
  sw(t8 + 0x0000, a0);
  sw(t8 + 0x0018, a3);
  sw(t8 + 0x0024, t0);
  temp = v1 == 0;
  sw(v0 + 0x0000, t8);
  if (temp) goto label80025024;
  sh(v1 + 0x0000, t8);
  a0 = t8 >> 16;
  sb(v1 + 0x0002, a0);
  t8 += 40; // 0x0028
  goto label80024C94;
label80025024:
  v1 = s4 - v0;
  temp = (int32_t)v1 <= 0;
  v1 = s5 - v0;
  if (temp) goto label80025034;
  s4 = v0;
label80025034:
  temp = (int32_t)v1 >= 0;
  sw(v0 + 0x0004, t8);
  if (temp) goto label80025040;
  s5 = v0;
label80025040:
  t8 += 40; // 0x0028
  goto label80024C94;
label80025048:
  gp = 0x8006FCF4;
  gp += 2304; // 0x0900
  ra = lw(ra + 0x0014);
  sp = 0x1F800000;
  fp = lw(ra + 0x0000);
  ra += 4; // 0x0004
  fp += ra;
label80025068:
  temp = ra == fp;
  s1 = lw(ra + 0x0000);
  if (temp) goto label800257A0;
  s2 = lw(ra + 0x0004);
  temp = (int32_t)s1 >= 0;
  at = s1 >> 18;
  if (temp) goto label80025104;
  at = at & 0xFF0;
  v0 = s1 >> 9;
  v0 = v0 & 0xFF0;
  v1 = s1 & 0xFF0;
  a3 = s2 >> 17;
  a3 = a3 & 0x7FC;
  t0 = s2 >> 8;
  t0 = t0 & 0x7FC;
  t1 = s2 << 1;
  t1 = t1 & 0x7FC;
  t2 = lw(ra + 0x000C);
  t3 = lw(ra + 0x0010);
  t4 = lw(ra + 0x0014);
  ra += 24; // 0x0018
  goto label80025140;
label800250B8:
  t7 = lw(ra - 0x0010); // 0xFFFFFFF0
  v0 = s1 >> 9;
  v0 = v0 & 0xFF0;
  v1 = s1 & 0xFF0;
  t0 = s2 >> 8;
  t0 = t0 & 0x7FC;
  t1 = s2 << 1;
  t1 = t1 & 0x7FC;
  at = t7 << 2;
  at = at & 0xFF0;
  a3 = t7 >> 9;
  a3 = a3 & 0x7FC;
  t2 = t2 >> 16;
  t2 = t2 << 16;
  t5 = t4 >> 16;
  t2 = t2 | t5;
  s1 = s1 << 1;
  s1 = s1 >> 1;
  goto label80025140;
label80025104:
  at = s1 >> 18;
  at = at & 0xFF0;
  v0 = s1 >> 9;
  v0 = v0 & 0xFF0;
  v1 = s1 & 0xFF0;
  a3 = s2 >> 17;
  a3 = a3 & 0x7FC;
  t0 = s2 >> 8;
  t0 = t0 & 0x7FC;
  t1 = s2 << 1;
  t1 = t1 & 0x7FC;
  t2 = lw(ra + 0x0008);
  t3 = lw(ra + 0x000C);
  t4 = lw(ra + 0x0010);
  ra += 20; // 0x0014
label80025140:
  t5 = at + gp;
  t6 = v0 + gp;
  t7 = v1 + gp;
  a0 = lw(t5 + 0x0004);
  a1 = lw(t6 + 0x0004);
  a2 = lw(t7 + 0x0004);
  s0 = a0 & a1;
  s0 = s0 & a2;
  temp = (int32_t)s0 < 0;
  t5 = lw(t5 + 0x0000);
  if (temp) goto label800252B0;
  t6 = lw(t6 + 0x0000);
  t7 = lw(t7 + 0x0000);
  at = at >> 2;
  v0 = v0 >> 2;
  v1 = v1 >> 2;
  at += sp;
  v0 += sp;
  v1 += sp;
  at = lw(at + 0x0000);
  v0 = lw(v0 + 0x0000);
  v1 = lw(v1 + 0x0000);
  cop2.SXY0 = at;
  cop2.SXY1 = v0;
  cop2.SXY2 = v1;
  sw(t8 + 0x0008, at);
  sw(t8 + 0x0014, v0);
  NCLIP();
  sw(t8 + 0x0020, v1);
  s0 = t5 >> 1;
  t5 += s0;
  s0 = t6 >> 1;
  t5 += s0;
  s0 = s1 & 0x1;
  temp = (int32_t)s0 > 0;
  a3 += t9;
  if (temp) goto label800251E0;
  s0 = cop2.MAC0;
  temp = (int32_t)s1 >= 0;
  if (temp) goto label800251DC;
  s0 = -s0;
label800251DC:
  temp = (int32_t)s0 <= 0;
  if (temp) {
    t0 += t9;
    goto label800252B0;
  }
label800251E0:
  t0 += t9;
  t1 += t9;
  a3 = lw(a3 + 0x0000);
  t0 = lw(t0 + 0x0000);
  t1 = lw(t1 + 0x0000);
  s0 = a0 | a1;
  s0 = s0 | a2;
  temp = (int32_t)s0 < 0;
  s0 = 0x34000000;
  if (temp) goto label800252C0;
  a3 = a3 << 8;
  a3 = a3 >> 8;
  a3 += s0;
  t5 -= s6;
  t5 += t6;
  t5 += t7;
  s0 = (int32_t)s2 >> 28;
  s0 = s0 << 2;
  s0 = s0 << s7;
  t5 += s0;
  temp = (int32_t)t5 <= 0;
  t5 = t5 >> s7;
  if (temp) goto label800252B0;
  t5 = t5 << 3;
  t5 += s3;
  t6 = lw(t5 + 0x0000);
  sw(t8 + 0x0004, a3);
  sw(t8 + 0x0010, t0);
  sw(t8 + 0x001C, t1);
  sw(t8 + 0x000C, t2);
  sw(t8 + 0x0018, t3);
  sw(t8 + 0x0024, t4);
  s0 = 0x09000000;
  sw(t8 + 0x0000, s0);
  temp = t6 == 0;
  sw(t5 + 0x0000, t8);
  if (temp) goto label80025284;
  sh(t6 + 0x0000, t8);
  t7 = t8 >> 16;
  sb(t6 + 0x0002, t7);
  temp = (int32_t)s1 < 0;
  t8 += 40; // 0x0028
  if (temp) goto label800250B8;
  goto label80025068;
label80025284:
  t6 = s4 - t5;
  temp = (int32_t)t6 <= 0;
  t6 = s5 - t5;
  if (temp) goto label80025294;
  s4 = t5;
label80025294:
  temp = (int32_t)t6 >= 0;
  sw(t5 + 0x0004, t8);
  if (temp) goto label800252A0;
  s5 = t5;
label800252A0:
  temp = (int32_t)s1 < 0;
  t8 += 40; // 0x0028
  if (temp) goto label800250B8;
  goto label80025068;
label800252B0:
  temp = (int32_t)s1 < 0;
  if (temp) goto label800250B8;
  goto label80025068;
label800252C0:
  sw(t8 + 0x000C, t2);
  sw(t8 + 0x0018, t3);
  t5 -= s6;
  t5 += t6;
  t5 += t7;
  s0 = (int32_t)s2 >> 28;
  s0 = s0 << 2;
  s0 = s0 << s7;
  t5 += s0;
  temp = (int32_t)t5 <= 0;
  t5 = t5 >> s7;
  if (temp) goto label800252B0;
  t5 = t5 << 3;
  t5 += s3;
  s0 = t8;
  temp = (int32_t)a0 < 0;
  if (temp) goto label80025310;
  sw(s0 + 0x0004, a3);
  sw(s0 + 0x0008, at);
  sh(s0 + 0x000C, t2);
  s0 += 12; // 0x000C
label80025310:
  t6 = a0 ^ a1;
  temp = (int32_t)t6 >= 0;
  t6 = a0 - a1;
  if (temp) goto label80025430;
  t7 = a0 << 12;
  div_psx(t7,t6);
  t6 = a3 & 0xFF;
  t7 = t0 & 0xFF;
  cop2.IR1 = t6;
  cop2.RFC = t7;
  t6 = a3 >> 8;
  t7 = t0 >> 8;
  t6 = t6 & 0xFF;
  t7 = t7 & 0xFF;
  cop2.IR2 = t6;
  cop2.GFC = t7;
  t6 = a3 >> 16;
  t7 = t0 >> 16;
  t6 = t6 & 0xFF;
  t7 = t7 & 0xFF;
  cop2.IR3 = t6;
  cop2.BFC = t7;
  t6=lo;
  cop2.IR0 = t6;
  INTPL();
  t7 = cop2.MAC3;
  t6 = cop2.MAC2;
  t7 = t7 << 8;
  t7 += t6;
  t6 = cop2.MAC1;
  t7 = t7 << 8;
  t7 += t6;
  sw(s0 + 0x0004, t7);
  t6 = at << 16;
  t7 = v0 << 16;
  t6 = (int32_t)t6 >> 16;
  t7 = (int32_t)t7 >> 16;
  cop2.IR1 = t6;
  cop2.RFC = t7;
  t6 = (int32_t)at >> 16;
  t7 = (int32_t)v0 >> 16;
  cop2.IR2 = t6;
  cop2.GFC = t7;
  INTPL();
  t6 = cop2.MAC2;
  t7 = cop2.MAC1;
  t6 = t6 << 16;
  t7 = t7 & 0xFFFF;
  t7 += t6;
  sw(s0 + 0x0008, t7);
  t6 = t2 & 0xFF;
  t7 = t3 & 0xFF;
  cop2.IR1 = t6;
  cop2.RFC = t7;
  t6 = t2 >> 8;
  t7 = t3 >> 8;
  t6 = t6 & 0xFF;
  t7 = t7 & 0xFF;
  cop2.IR2 = t6;
  cop2.GFC = t7;
  INTPL();
  t6 = cop2.MAC2;
  t7 = cop2.MAC1;
  t6 = t6 << 8;
  t7 += t6;
  sh(s0 + 0x000C, t7);
  s0 += 12; // 0x000C
label80025430:
  temp = (int32_t)a1 < 0;
  if (temp) goto label80025448;
  sw(s0 + 0x0004, t0);
  sw(s0 + 0x0008, v0);
  sh(s0 + 0x000C, t3);
  s0 += 12; // 0x000C
label80025448:
  t6 = a1 ^ a2;
  temp = (int32_t)t6 >= 0;
  t6 = a1 - a2;
  if (temp) goto label80025568;
  t7 = a1 << 12;
  div_psx(t7,t6);
  t6 = t0 & 0xFF;
  t7 = t1 & 0xFF;
  cop2.IR1 = t6;
  cop2.RFC = t7;
  t6 = t0 >> 8;
  t7 = t1 >> 8;
  t6 = t6 & 0xFF;
  t7 = t7 & 0xFF;
  cop2.IR2 = t6;
  cop2.GFC = t7;
  t6 = t0 >> 16;
  t7 = t1 >> 16;
  t6 = t6 & 0xFF;
  t7 = t7 & 0xFF;
  cop2.IR3 = t6;
  cop2.BFC = t7;
  t6=lo;
  cop2.IR0 = t6;
  INTPL();
  t7 = cop2.MAC3;
  t6 = cop2.MAC2;
  t7 = t7 << 8;
  t7 += t6;
  t6 = cop2.MAC1;
  t7 = t7 << 8;
  t7 += t6;
  sw(s0 + 0x0004, t7);
  t6 = v0 << 16;
  t7 = v1 << 16;
  t6 = (int32_t)t6 >> 16;
  t7 = (int32_t)t7 >> 16;
  cop2.IR1 = t6;
  cop2.RFC = t7;
  t6 = (int32_t)v0 >> 16;
  t7 = (int32_t)v1 >> 16;
  cop2.IR2 = t6;
  cop2.GFC = t7;
  INTPL();
  t6 = cop2.MAC2;
  t7 = cop2.MAC1;
  t6 = t6 << 16;
  t7 = t7 & 0xFFFF;
  t7 += t6;
  sw(s0 + 0x0008, t7);
  t6 = t3 & 0xFF;
  t7 = t4 & 0xFF;
  cop2.IR1 = t6;
  cop2.RFC = t7;
  t6 = t3 >> 8;
  t7 = t4 >> 8;
  t6 = t6 & 0xFF;
  t7 = t7 & 0xFF;
  cop2.IR2 = t6;
  cop2.GFC = t7;
  INTPL();
  t6 = cop2.MAC2;
  t7 = cop2.MAC1;
  t6 = t6 << 8;
  t7 += t6;
  sh(s0 + 0x000C, t7);
  s0 += 12; // 0x000C
label80025568:
  temp = (int32_t)a2 < 0;
  if (temp) goto label80025580;
  sw(s0 + 0x0004, t1);
  sw(s0 + 0x0008, v1);
  sh(s0 + 0x000C, t4);
  s0 += 12; // 0x000C
label80025580:
  t6 = a2 ^ a0;
  temp = (int32_t)t6 >= 0;
  t6 = a2 - a0;
  if (temp) goto label800256A0;
  t7 = a2 << 12;
  div_psx(t7,t6);
  t6 = t1 & 0xFF;
  t7 = a3 & 0xFF;
  cop2.IR1 = t6;
  cop2.RFC = t7;
  t6 = t1 >> 8;
  t7 = a3 >> 8;
  t6 = t6 & 0xFF;
  t7 = t7 & 0xFF;
  cop2.IR2 = t6;
  cop2.GFC = t7;
  t6 = t1 >> 16;
  t7 = a3 >> 16;
  t6 = t6 & 0xFF;
  t7 = t7 & 0xFF;
  cop2.IR3 = t6;
  cop2.BFC = t7;
  t6=lo;
  cop2.IR0 = t6;
  INTPL();
  t7 = cop2.MAC3;
  t6 = cop2.MAC2;
  t7 = t7 << 8;
  t7 += t6;
  t6 = cop2.MAC1;
  t7 = t7 << 8;
  t7 += t6;
  sw(s0 + 0x0004, t7);
  t6 = v1 << 16;
  t7 = at << 16;
  t6 = (int32_t)t6 >> 16;
  t7 = (int32_t)t7 >> 16;
  cop2.IR1 = t6;
  cop2.RFC = t7;
  t6 = (int32_t)v1 >> 16;
  t7 = (int32_t)at >> 16;
  cop2.IR2 = t6;
  cop2.GFC = t7;
  INTPL();
  t6 = cop2.MAC2;
  t7 = cop2.MAC1;
  t6 = t6 << 16;
  t7 = t7 & 0xFFFF;
  t7 += t6;
  sw(s0 + 0x0008, t7);
  t6 = t4 & 0xFF;
  t7 = t2 & 0xFF;
  cop2.IR1 = t6;
  cop2.RFC = t7;
  t6 = t4 >> 8;
  t7 = t2 >> 8;
  t6 = t6 & 0xFF;
  t7 = t7 & 0xFF;
  cop2.IR2 = t6;
  cop2.GFC = t7;
  INTPL();
  t6 = cop2.MAC2;
  t7 = cop2.MAC1;
  t6 = t6 << 8;
  t7 += t6;
  sh(s0 + 0x000C, t7);
  s0 += 12; // 0x000C
label800256A0:
  s0 -= t8;
  s0 -= 48; // 0xFFFFFFD0
  temp = s0 == 0;
  t6 = lw(t5 + 0x0000);
  if (temp) goto label80025710;
  s0 = 52; // 0x0034
  sb(t8 + 0x0007, s0);
  s0 = 0x09000000;
  sw(t8 + 0x0000, s0);
  temp = t6 == 0;
  sw(t5 + 0x0000, t8);
  if (temp) goto label800256E4;
  sh(t6 + 0x0000, t8);
  t7 = t8 >> 16;
  sb(t6 + 0x0002, t7);
  temp = (int32_t)s1 < 0;
  t8 += 40; // 0x0028
  if (temp) goto label800250B8;
  goto label80025068;
label800256E4:
  t6 = s4 - t5;
  temp = (int32_t)t6 <= 0;
  t6 = s5 - t5;
  if (temp) goto label800256F4;
  s4 = t5;
label800256F4:
  temp = (int32_t)t6 >= 0;
  sw(t5 + 0x0004, t8);
  if (temp) goto label80025700;
  s5 = t5;
label80025700:
  temp = (int32_t)s1 < 0;
  t8 += 40; // 0x0028
  if (temp) goto label800250B8;
  goto label80025068;
label80025710:
  at = lw(t8 + 0x001C);
  v0 = lw(t8 + 0x0020);
  v1 = lw(t8 + 0x0024);
  a0 = lw(t8 + 0x0028);
  a1 = lw(t8 + 0x002C);
  a2 = lw(t8 + 0x0030);
  sw(t8 + 0x001C, a0);
  sw(t8 + 0x0020, a1);
  sw(t8 + 0x0024, a2);
  sw(t8 + 0x0028, at);
  sw(t8 + 0x002C, v0);
  sw(t8 + 0x0030, v1);
  s0 = 60; // 0x003C
  sb(t8 + 0x0007, s0);
  s0 = 0x0C000000;
  sw(t8 + 0x0000, s0);
  temp = t6 == 0;
  sw(t5 + 0x0000, t8);
  if (temp) goto label80025774;
  sh(t6 + 0x0000, t8);
  t7 = t8 >> 16;
  sb(t6 + 0x0002, t7);
  temp = (int32_t)s1 < 0;
  t8 += 52; // 0x0034
  if (temp) goto label800250B8;
  goto label80025068;
label80025774:
  t6 = s4 - t5;
  temp = (int32_t)t6 <= 0;
  t6 = s5 - t5;
  if (temp) goto label80025784;
  s4 = t5;
label80025784:
  temp = (int32_t)t6 >= 0;
  sw(t5 + 0x0004, t8);
  if (temp) goto label80025790;
  s5 = t5;
label80025790:
  temp = (int32_t)s1 < 0;
  t8 += 52; // 0x0034
  if (temp) goto label800250B8;
  goto label80025068;
label800257A0:
  at = s5 - s4;
  temp = (int32_t)at < 0;
  if (temp) goto label800258B0;
  at = cop2.GBK;
  v0 = cop2.RBK;
  at = at << 3;
  v1 = 32; // 0x0020
  v1 = v1 << v0;
  v1 -= 8; // 0xFFFFFFF8
  at += v1;
  v1 = 0x8006FCF4;
  v1 += 2048; // 0x0800
  v1 -= s5;
  temp = (int32_t)v1 >= 0;
  v1 = v1 << v0;
  if (temp) goto label800257E4;
  v1 = 0;
label800257E4:
  v1 = v1 >> 8;
  v1 = v1 << 3;
  at -= v1;
  temp = (int32_t)at >= 0;
  a0 = s5;
  if (temp) goto label800257FC;
  at = 0;
label800257FC:
  v1 = 256; // 0x0100
  v1 = v1 >> v0;
  v0 = ordered_linked_list;
  v0 = lw(v0 + 0x0000);
  a2 = s4 - 8; // 0xFFFFFFF8
  at += v0;
label80025818:
  a3 = lw(at + 0x0000);
  a1 = a0 - v1;
  t0 = a2 - a1;
  temp = (int32_t)t0 <= 0;
  if (temp) goto label80025830;
  a1 = a2;
label80025830:
  temp = a3 != 0;
  if (temp) goto label8002585C;
label80025838:
  temp = a0 == a1;
  t0 = lw(a0 + 0x0004);
  if (temp) goto label8002588C;
  a0 -= 8; // 0xFFFFFFF8
  temp = t0 == 0;
  if (temp) goto label80025838;
  a3 = lw(a0 + 0x0008);
  sw(at + 0x0004, t0);
  sw(a0 + 0x000C, 0);
  sw(a0 + 0x0008, 0);
label8002585C:
  temp = a0 == a1;
  t0 = lw(a0 + 0x0004);
  if (temp) goto label8002588C;
  a0 -= 8; // 0xFFFFFFF8
  temp = t0 == 0;
  t1 = t0 >> 16;
  if (temp) goto label8002585C;
  t2 = lw(a0 + 0x0008);
  sh(a3 + 0x0000, t0);
  sb(a3 + 0x0002, t1);
  a3 = t2;
  sw(a0 + 0x0008, 0);
  temp = a0 != a1;
  sw(a0 + 0x000C, 0);
  if (temp) goto label8002585C;
label8002588C:
  temp = a1 == a2;
  sw(at + 0x0000, a3);
  if (temp) goto label800258A4;
  temp = at != v0;
  at -= 8; // 0xFFFFFFF8
  if (temp) goto label80025818;
  at += 8; // 0x0008
  goto label80025818;
label800258A4:
  sw(allocator1_ptr, t8);
label800258B0:
  at = 0x80077DD8;
  ra = lw(at + 0x2C);
  fp = lw(at + 0x28);
  sp = lw(at + 0x24);
  gp = lw(at + 0x20);
  s7 = lw(at + 0x1C);
  s6 = lw(at + 0x18);
  s5 = lw(at + 0x14);
  s4 = lw(at + 0x10);
  s3 = lw(at + 0x0C);
  s2 = lw(at + 0x08);
  s1 = lw(at + 0x04);
  s0 = lw(at + 0x00);
  return;
}

// size: 0x00000064
void function_8002B390(void)
{
  for (int j = 0; j < 9; j++) {
    t4 = lw(0x80078560 + j*8 + 0);
    t3 = lw(0x80078560 + j*8 + 4);
    for (int i = 0; i < t4; i++) {
      t2 = lw(t3 + i*4);
      if (lbu(t2 + 0) == a0)
        sb(t2 + 1, (lbu(t2 + 1) & a1) | a2);
    }
  }
}

// size: 0x00000050
void function_8002B3F4(void)
{
  for (int j = 0; j < 9; j++) {
    t4 = lw(0x80078560 + j*8 + 0);
    t3 = lw(0x80078560 + j*8 + 4);
    for (int i = 0; i < t4; i++) {
      t2 = lw(t3 + i*4);
      at = lw(t2);
      v0 = at >> 8;
      if ((at&0xFF) == (a0&0xFF)) return;
    }
  }
}

// size: 0x00000068
void function_8002B444(void)
{
  for (int j = 0; j < 9; j++) {
    t4 = lw(0x80078560 + j*8 + 0);
    t3 = lw(0x80078560 + j*8 + 4);
    for (int i = 0; i < t4; i++) {
      t2 = lw(t3 + i*4);
      if (lbu(t2 + 0) == a0) {
        sb(t2 + 2, a1);
        if (a2) sb(t2 + 3, 1);
      }
    }
  }
}

// size: 0x00000044
void function_80058C7C(void)
{
  a0 = -2517; // 0xFFFFF62B
  v1 = -2458; // 0xFFFFF666
  v0 = -2848; // 0xFFFFF4E0
  sw(0x800770C8, a0);
  sw(0x800770CC, v1);
  sw(0x800770D0, v0);
  sw(0x800770D4, a0);
  sw(0x800770D8, v1);
  sw(0x800770DC, v0);
}

// size: 0x00000070
void function_8002C8A4(void)
{
  v0 = lbu(SKYBOX_DATA + 0x10);
  v1 = lbu(SKYBOX_DATA + 0x11);
  a0 = lbu(SKYBOX_DATA + 0x12);
  sp -= 24; // 0xFFFFFFE8
  sw(sp + 0x0010, ra);
  sw(0x800757D8, 0); // &0x00000000
  sb(DISP1 + 0x19, v0);
  sb(DISP1 + 0x1A, v1);
  sb(DISP1 + 0x1B, a0);
  sb(DISP2 + 0x19, v0);
  sb(DISP2 + 0x1A, v1);
  sb(DISP2 + 0x1B, a0);
  function_80058C7C();
  ra = lw(sp + 0x0010);
  sp += 24; // 0x0018
  return;
}

// size: 0x000000A4
void function_80058CC0(void)
{
  v1 = 0x800770F4;
  sw(v1, (lw(v1) + a0) & 0xFF);
  sw(0x800770C8, cos_lut[lw(0x800770F4)]*1983/2048);
  sw(0x800770D0, -0x400);
  sw(0x800770CC, sin_lut[lw(0x800770F4)]*6337/4096);
}

// size: 0x000000A4
void function_80058BD8(void)
{
  vec3_32 v1 = {
    .x = 0x1000
  };
  spyro_camera_mat_vec_multiply(&v1, &v1);

  vec3 mv1 = vec3_32_to_vec3(v1);

  vec3_32 v2 = {
    .y = 0x1000
  };
  spyro_camera_mat_vec_multiply(&v2, &v2);

  vec3 mv2 = vec3_32_to_vec3(v2);

  vec3_32 v3 = {
    .z = -0x1000
  };
  spyro_camera_mat_vec_multiply(&v3, &v3);

  vec3 mv3 = vec3_32_to_vec3(v3);

  mat3 m = mat3_cv(mv1, mv2, mv3);

  save_mat3(0x800770E0, m);
}


// traverses a zero terminated pointer
// array starting at 0x8006FCF4
/*void function_8001F158()
{
  uint32_t at = 0x80077DD8;
  sw(at + 0x08, s2);
  sw(at + 0x0C, s3);
  sw(at + 0x10, s4);
  sw(at + 0x14, s5);
  sw(at + 0x18, s6);
  sw(at + 0x1C, s7);
  sw(at + 0x24, sp);
  sw(at + 0x28, fp);

  ra = 0x8006FCF4 + 0x00;
  fp = 0x8006FCF4 + 0x1600;
  uint32_t lo = 0x8006FCF4 + 0x2800;

  t9 = 0x80076DD0;
  
  s2 = lw(t9 + 0x28);
  s3 = lw(t9 + 0x2C);
  s4 = lw(t9 + 0x30);

  mat3 rtm = load_mat3(t9);

  while(fp + 0x38 <= 0x8006FCF4 + 0x2200) {
    sp = lw(ra);
    ra += 4;

    if (sp == 0) break;

    a0 = lhu(sp + 0x50);
    sb(sp + 0x51, 0);
    a1 = (a0 & 0x100) << 1;
    t4 = ((a0 & 0xFF) << 8) + a1;

    vec3_32 x = {
      -(int32_t)(s2 - lw(sp + 0x0C)) >> 2,
       (int32_t)(s3 - lw(sp + 0x10)) >> 2,
       (int32_t)(s4 - lw(sp + 0x14)) >> 2
    };
    
    if (abs_int(x.x) >= t4
     || abs_int(x.y) >= t4
     || abs_int(x.z) >= t4) continue;

    t6 = lw(sp + 0x3C);
    t7 = lw(0x80076378 + lhu(sp + 0x36)*4);
    t5 = lw(((t6 & 0xFF) << 2) + t7 + 0x38);
    
    a1 = lbu(t5 + 7) << 4;
    t0 = (a1 >> 1) + (a1 >> 5);
    a2 = t0 + (a1 >> 2);
    a3 = t0 + (a1 >> 4);

    x = vec3_32_mat_mul((vec3_32){x.y, x.z, x.x}, rtm);

    // frustum culling
    if (x.z >= (int32_t)t4
     || x.z <= (int32_t)-a1
     || (int32_t)(abs_int(x.x) - a2)*4 >= (int32_t)(x.z + a3)*3) continue;

    if ((int32_t)lw(sp + 0x1C) < 0 && x.z < 4608)
    {
      sw(lo + 0, sp);
      sw(lo + 4, t5 + 6 + 0x24 + lbu((lbu(sp + 0x3E)*8)));
      lo += 8;
    }

    t1 = (a1 >> 2) + (a1 >> 4);
    t0 = a1 - (a1 >> 5) - (a1 >> 6);
    
    t3 = x.z + t1 - (abs_int(x.y) - t0)*3;
    if ((int32_t)t3 <= 0) continue;
    
    if ((int32_t)(abs_int(x.x) + a2)*4 >= (int32_t)(x.z - a3)*3
     || (int32_t)(x.z - t1) <= (int32_t)(abs_int(x.y) + t0)*3)
      a0 = 0x80000000;
    else
      a0 = 0x40000000;

    a1 = lbu(sp + 0x40);
    t3 = lw(sp + 0x44); // rotation

    if (a1) {
      // t6 = lw(sp + 0x3C);
      t4 = lw(t7 + 0x38 + ((t6 >> 8) & 0xFF)*4);

      a0 += (max_int(lbu(t5 + 0xB), lbu(t4 + 0xB)) << 24)
        + (a1 << 8) + ((t3 >> 24) << 16) + lbu(sp + 0x57);
          
      a1 = (((t6 >> 16) & 0xFF) << 3) + 36 + t5;
      a2 = ((t6 >> 24) << 3) + 36 + t4;
    } else {
      // t6 = lw(sp + 0x3C);
      a0 += ((t3 >> 24) << 16) + (lbu(t5 + 0xB) << 24) + lbu(sp + 0x57);
      a1 = (((t6 >> 16) & 0xFF) << 3) + 36 + t5;
      a2 = 0;
    }

    t2 = ((lbu(sp + 0x4B) & 0x3F) << 8) - x.z;

    sb(sp + 0x51, 1);
    sw(fp + 0x00, a0); // four 8-bit numbers
    sw(fp + 0x04, t5); // pointer to another struct1
    sw(fp + 0x08, a1); // pointer to another struct2
    sw(fp + 0x0C, a2); // pointer to another struct3?
    sw(fp + 0x10, x.x); //vec1 x
    sw(fp + 0x14, x.y); //vec1 y
    sw(fp + 0x18, x.z); //vec1 z
    sh(fp + 0x2E, t2);
    sw(fp + 0x34, sp);

    mat3 m = rtm;

    uint32_t rotY = (t3 >> 16) & 0xFF;
    if (rotY) m = mat3_mul(m, mat3rotY(-rotY*16));

    uint32_t rotX = (t3 >>  8) & 0xFF;
    if (rotX) m = mat3_mul(m, mat3rotX(rotX*16));
    
    uint32_t rotZ = (t3 >>  0) & 0xFF;
    if (rotZ) m = mat3_mul(m, mat3rotZ(-rotZ*16));

    save_mat3(fp + 0x1C, m);

    fp += 0x38;
  }

  sw(fp, 0);
  v0 = 0x80075EF8;
  sw(v0 + 8, lo);

  at = 0x80077DD8;
  fp = lw(at + 0x28);
  sp = lw(at + 0x24);
  s4 = lw(at + 0x10);
  s3 = lw(at + 0x0C);
  s2 = lw(at + 0x08);
  return;
}*/

// size: 0x00001164
void function_8001F798(void)
{
  uint32_t temp;
  at = 0x80077DD8;
  sw(at + 0x0000, s0);
  sw(at + 0x0004, s1);
  sw(at + 0x0008, s2);
  sw(at + 0x000C, s3);
  sw(at + 0x0010, s4);
  sw(at + 0x0014, s5);
  sw(at + 0x0018, s6);
  sw(at + 0x001C, s7);
  sw(at + 0x0020, gp);
  sw(at + 0x0024, sp);
  sw(at + 0x0028, fp);
  sw(at + 0x002C, ra);
  at = allocator1_ptr;
  at = lw(at + 0x0000);
  gp = 0x8006FCF4;
  gp += 5632; // 0x1600
  v0 = 0x8007591C;
  v0 = lw(v0 + 0x0000);
  hi=at;
  cop2.ZSF3 = v0;
label8001F7FC:
  s3 = lw(gp + 0x0000);
  ra = lw(gp + 0x0004);
  temp = s3 == 0;
  s4 = s3 & 0xFF00;
  if (temp) goto label800208AC;
  s4 = s4 >> 2;
  a0 = s3 << 8;
  a0 = (int32_t)a0 >> 24;
  v1 = lw(gp + 0x0018);
  v0 = lw(gp + 0x0014);
  at = lw(gp + 0x0010);
  a2 = (int32_t)v1 >> 5;
  a2 -= a0;
  temp = (int32_t)a2 >= 0;
  a0 = lbu(ra + 0x0005);
  if (temp) goto label8001F838;
  a2 = 0;
label8001F838:
  a3 = a2 - 272; // 0xFFFFFEF0
  temp = (int32_t)a3 < 0;
  a1 = s3 >> 24;
  if (temp) goto label8001F848;
  a2 += 32; // 0x0020
label8001F848:
  a1 = a1 & 0x1F;
  at = at << 2;
  v0 = v0 << 2;
  v1 = v1 << 2;
  at = (int32_t)at >> a0;
  v0 = (int32_t)v0 >> a0;
  v1 = (int32_t)v1 >> a0;
  a3 = s3 & 0xFF;
  temp = a3 == 0;
  s5 = lbu(ra + 0x0006);
  if (temp) goto label8001F8B0;
  cop2.IR0 = a3;
  cop2.IR1 = at;
  cop2.IR2 = v0;
  cop2.IR3 = v1;
  GPF(SF_OFF, LM_OFF);
  v1 = cop2.MAC3;
  v0 = cop2.MAC2;
  at = cop2.MAC1;
  v1 = (int32_t)v1 >> 5;
  v0 = (int32_t)v0 >> 5;
  a3 = v1 - 31500; // 0xFFFF84F4
  a3 -= 31500; // 0xFFFF84F4
  temp = (int32_t)a3 > 0;
  at = (int32_t)at >> 5;
  if (temp) goto label800208A4;
label8001F8B0:
  a3 = 512; // 0x0200
  a3 = a3 << a1;
  a3 = v1 - a3;
  s5++;
  a0 = a0 << 8;
  a1 += a0;
  cop2.TRX = at;
  cop2.TRY = v0;
  cop2.TRZ = v1;
  cop2.RBK = a1;
  cop2.GBK = a2;
  cop2.BBK = a3;
  at = lw(gp + 0x001C);
  v0 = lw(gp + 0x0020);
  v1 = lw(gp + 0x0024);
  a0 = lw(gp + 0x0028);
  a1 = lw(gp + 0x002C);
  cop2.RTM0 = at;
  cop2.RTM1 = v0;
  cop2.RTM2 = v1;
  cop2.RTM3 = a0;
  cop2.RTM4 = a1;
  a1 = (int32_t)a1 >> 16;
  cop2.ZSF4 = a1;
  t2 = lw(gp + 0x0008);
  t4 = lw(gp + 0x000C);
  t3 = lw(t2 + 0x0004);
  v0 = 0x8006FCF4;
  temp = (int32_t)a1 <= 0;
  v0 += 10752; // 0x2A00
  if (temp) goto label8001F998;
  at = t3 << 16;
  at = at >> 14;
  temp = at == 0;
  at += t2;
  if (temp) goto label8001F998;
  v1 = lw(ra + 0x0014);
label8001F940:
  a0 = lw(at + 0x0000);
  at += 4; // 0x0004
  a1 = a0 & 0xFFFF;
  a1 += v1;
  a2 = a0 >> 24;
  a2 = a2 & 0x7F;
  a3 = a1 << 8;
  a3 += a2;
  sw(v0 + 0x0000, a3);
  v0 += 4; // 0x0004
label8001F968:
  temp = a2 == 0;
  a2--;
  if (temp) goto label8001F990;
  a3 = lw(a1 + 0x0000);
  a1 += 4; // 0x0004
  sw(v0 + 0x0000, a3);
  v0 += 4; // 0x0004
  a3 = lw(at + 0x0000);
  at += 4; // 0x0004
  sw(a1 - 0x0004, a3); // 0xFFFFFFFC
  goto label8001F968;
label8001F990:
  temp = (int32_t)a0 > 0;
  if (temp) goto label8001F940;
label8001F998:
  sw(v0 + 0x0000, 0);
  v0 = lw(t2 + 0x0000);
  at = t3 >> 24;
  at = at << 2;
  v0 = v0 << 11;
  v0 = v0 >> 11;
  temp = t4 == 0;
  at += v0;
  if (temp) goto label8001F9D4;
  a3 = lw(t4 + 0x0004);
  t0 = lw(t4 + 0x0000);
  a3 = a3 >> 24;
  a3 = a3 << 2;
  t0 = t0 << 11;
  t0 = t0 >> 11;
  a3 += t0;
label8001F9D4:
  fp = cop2.ZSF4;
  lo=ra;
  fp = fp >> 31;
  ra += fp;
  ra = lbu(ra + 0x0008);
  s6 = -1; // 0xFFFFFFFF
  s7 = 0x00010000;
  t8 = 0x01000000;
  t9 = 0x02000000;
  fp = 0x1F800000;
  a2 = lw(v0 + 0x0000);
  temp = ra == 0;
  sp = ra - 128; // 0xFFFFFF80
  if (temp) goto label80020860;
  temp = (int32_t)sp <= 0;
  sp = fp + 512; // 0x0200
  if (temp) goto label8001FA1C;
  sp = 0x8006FCF4;
  sp += 2304; // 0x0900
label8001FA1C:
  ra = ra << 2;
  ra += fp;
  v0 += 4; // 0x0004
  v1 = (int32_t)a2 >> 21;
  a0 = a2 << 10;
  a0 = (int32_t)a0 >> 21;
  a1 = a2 << 20;
  a1 = (int32_t)a1 >> 19;
  temp = s4 == 0;
  a2 = a2 & 0x1;
  if (temp) goto label8001FCC8;
  t4 = lw(t0 + 0x0000);
  t0 += 4; // 0x0004
  t1 = (int32_t)t4 >> 21;
  t2 = t4 << 10;
  t2 = (int32_t)t2 >> 21;
  t3 = t4 << 20;
  t3 = (int32_t)t3 >> 19;
  cop2.IR0 = s4;
  cop2.IR1 = v1;
  cop2.IR2 = a0;
  cop2.IR3 = a1;
  cop2.RFC = t1;
  cop2.GFC = t2;
  cop2.BFC = t3;
  t4 = t4 & 0x1;
  INTPL();
  temp = a2 != 0;
  if (temp) goto label8001FA98;
  t5 = lw(v0 + 0x0000);
  goto label8001FA9C;
label8001FA98:
  t5 = lh(at + 0x0000);
label8001FA9C:
  t6 = cop2.MAC1;
  t7 = cop2.MAC2;
  s0 = cop2.MAC3;
  cop2.VZ0 = t6;
  s0 = s0 << 16;
  t7 += s0;
  cop2.VXY0 = t7;
label8001FAB8:
  temp = a2 != 0;
  sp += 4; // 0x0004
  if (temp) goto label8001FB80;
  RTPS();
  temp = t4 != 0;
  v1 = (int32_t)t5 >> 21;
  if (temp) goto label8001FAD4;
  t6 = lw(t0 + 0x0000);
  goto label8001FAD8;
label8001FAD4:
  t6 = lh(a3 + 0x0000);
label8001FAD8:
  t7 = t5 << 10;
  a0 = (int32_t)t7 >> 21;
  t7 = t5 << 20;
  a1 = (int32_t)t7 >> 19;
  v0 += 4; // 0x0004
  temp = t4 != 0;
  a2 = t5 & 0x1;
  if (temp) goto label8001FBCC;
label8001FAF4:
  t1 = (int32_t)t6 >> 21;
  t5 = t6 << 10;
  t2 = (int32_t)t5 >> 21;
  t5 = t6 << 20;
  t3 = (int32_t)t5 >> 19;
  cop2.IR0 = s4;
  cop2.IR1 = v1;
  cop2.IR2 = a0;
  cop2.IR3 = a1;
  cop2.RFC = t1;
  cop2.GFC = t2;
  cop2.BFC = t3;
  t7 = cop2.SXY2;
  s0 = cop2.SZ3;
  INTPL();
  temp = a2 != 0;
  t0 += 4; // 0x0004
  if (temp) goto label8001FB40;
  t5 = lw(v0 + 0x0000);
  goto label8001FB44;
label8001FB40:
  t5 = lh(at + 0x0000);
label8001FB44:
  t4 = t6 & 0x1;
  fp += 4; // 0x0004
  t6 = cop2.MAC1;
  s1 = cop2.MAC3;
  cop2.VZ0 = t6;
  t6 = cop2.MAC2;
  s1 = s1 << 16;
  t6 += s1;
  cop2.VXY0 = t6;
  temp = (int32_t)s3 < 0;
  sw(sp - 0x0004, s0); // 0xFFFFFFFC
  if (temp) goto label8001FC70;
  temp = fp != ra;
  sw(fp - 0x0004, t7); // 0xFFFFFFFC
  if (temp) goto label8001FAB8;
  goto label8001FDD0;
label8001FB80:
  RTPS();
  temp = t4 != 0;
  t7 = (int32_t)t5 >> 11;
  if (temp) goto label8001FB94;
  t6 = lw(t0 + 0x0000);
  goto label8001FB98;
label8001FB94:
  t6 = lh(a3 + 0x0000);
label8001FB98:
  t7 = t7 << s5;
  v1 += t7;
  t7 = t5 << 21;
  t7 = (int32_t)t7 >> 27;
  t7 = t7 << s5;
  a0 -= t7;
  t7 = t5 << 26;
  t7 = (int32_t)t7 >> 27;
  t7 = t7 << s5;
  a1 -= t7;
  at += 2; // 0x0002
  temp = t4 == 0;
  a2 = t5 & 0x1;
  if (temp) goto label8001FAF4;
label8001FBCC:
  t5 = (int32_t)t6 >> 11;
  t5 = t5 << s5;
  t1 += t5;
  t5 = t6 << 21;
  t5 = (int32_t)t5 >> 27;
  t5 = t5 << s5;
  t2 -= t5;
  t5 = t6 << 26;
  t5 = (int32_t)t5 >> 27;
  t5 = t5 << s5;
  t3 -= t5;
  cop2.IR0 = s4;
  cop2.IR1 = v1;
  cop2.IR2 = a0;
  cop2.IR3 = a1;
  cop2.RFC = t1;
  cop2.GFC = t2;
  cop2.BFC = t3;
  t7 = cop2.SXY2;
  s0 = cop2.SZ3;
  INTPL();
  temp = a2 != 0;
  a3 += 2; // 0x0002
  if (temp) goto label8001FC30;
  t5 = lw(v0 + 0x0000);
  goto label8001FC34;
label8001FC30:
  t5 = lh(at + 0x0000);
label8001FC34:
  t4 = t6 & 0x1;
  fp += 4; // 0x0004
  t6 = cop2.MAC1;
  s1 = cop2.MAC3;
  cop2.VZ0 = t6;
  t6 = cop2.MAC2;
  s1 = s1 << 16;
  t6 += s1;
  cop2.VXY0 = t6;
  temp = (int32_t)s3 < 0;
  sw(sp - 0x0004, s0); // 0xFFFFFFFC
  if (temp) goto label8001FC70;
  temp = fp != ra;
  sw(fp - 0x0004, t7); // 0xFFFFFFFC
  if (temp) goto label8001FAB8;
  goto label8001FDD0;
label8001FC70:
  s0 = t7 << 5;
  t6 = t7 - s7;
  temp = (int32_t)t6 > 0;
  t6 = t7 - t8;
  if (temp) goto label8001FC84;
  s0 = s0 | 0x1;
label8001FC84:
  temp = (int32_t)t6 < 0;
  t6 = t7 << 16;
  if (temp) goto label8001FC90;
  s0 = s0 | 0x2;
label8001FC90:
  temp = (int32_t)t6 > 0;
  t6 -= t9;
  if (temp) goto label8001FC9C;
  s0 = s0 | 0x4;
label8001FC9C:
  temp = (int32_t)t6 < 0;
  if (temp) goto label8001FCA8;
  s0 = s0 | 0x8;
label8001FCA8:
  s6 = s6 & s0;
  temp = fp != ra;
  sw(fp - 0x0004, s0); // 0xFFFFFFFC
  if (temp) goto label8001FAB8;
  s6 = s6 & 0x1F;
  temp = (int32_t)s6 > 0;
  if (temp) goto label80020860;
  goto label8001FDD0;
label8001FCC8:
  cop2.VZ0 = v1;
  t5 = a1 << 16;
  t5 += a0;
  cop2.VXY0 = t5;
label8001FCD8:
  temp = a2 != 0;
  sp += 4; // 0x0004
  if (temp) goto label8001FD10;
  RTPS();
  t5 = lw(v0 + 0x0000);
  v0 += 4; // 0x0004
  fp += 4; // 0x0004
  v1 = (int32_t)t5 >> 21;
  t6 = t5 << 10;
  a0 = (int32_t)t6 >> 21;
  t6 = t5 << 20;
  a1 = (int32_t)t6 >> 19;
  t6 = a1 << 16;
  t6 += a0;
  goto label8001FD54;
label8001FD10:
  t5 = lh(at + 0x0000);
  RTPS();
  at += 2; // 0x0002
  fp += 4; // 0x0004
  t6 = (int32_t)t5 >> 11;
  t6 = t6 << s5;
  v1 += t6;
  t6 = t5 << 21;
  t6 = (int32_t)t6 >> 27;
  t6 = t6 << s5;
  a0 -= t6;
  t6 = t5 << 26;
  t6 = (int32_t)t6 >> 27;
  t6 = t6 << s5;
  a1 -= t6;
  t6 = a1 << 16;
  t6 += a0;
label8001FD54:
  cop2.VXY0 = t6;
  cop2.VZ0 = v1;
  t6 = cop2.SZ3;
  t7 = cop2.SXY2;
  temp = (int32_t)s3 < 0;
  sw(sp - 0x0004, t6); // 0xFFFFFFFC
  if (temp) goto label8001FD80;
  a2 = t5 & 0x1;
  temp = fp != ra;
  sw(fp - 0x0004, t7); // 0xFFFFFFFC
  if (temp) goto label8001FCD8;
  goto label8001FDD0;
label8001FD80:
  s0 = t7 << 5;
  t6 = t7 - s7;
  temp = (int32_t)t6 > 0;
  t6 = t7 - t8;
  if (temp) goto label8001FD94;
  s0 = s0 | 0x1;
label8001FD94:
  temp = (int32_t)t6 < 0;
  t6 = t7 << 16;
  if (temp) goto label8001FDA0;
  s0 = s0 | 0x2;
label8001FDA0:
  temp = (int32_t)t6 > 0;
  t6 -= t9;
  if (temp) goto label8001FDAC;
  s0 = s0 | 0x4;
label8001FDAC:
  temp = (int32_t)t6 < 0;
  a2 = t5 & 0x1;
  if (temp) goto label8001FDB8;
  s0 = s0 | 0x8;
label8001FDB8:
  s6 = s6 & s0;
  temp = fp != ra;
  sw(fp - 0x0004, s0); // 0xFFFFFFFC
  if (temp) goto label8001FCD8;
  s6 = s6 & 0x1F;
  temp = (int32_t)s6 > 0;
  if (temp) goto label80020860;
label8001FDD0:
  v0 = cop2.ZSF4;
  ra=lo;
  temp = (int32_t)v0 <= 0;
  v0 -= 1024; // 0xFFFFFC00
  if (temp) goto label8001FE98;
  temp = (int32_t)v0 >= 0;
  s2 = v0 << 2;
  if (temp) goto label8001FF58;
  s2 = -s2;
  cop2.IR0 = s2;
  at = lw(ra + 0x0018);
  v0 = lw(ra + 0x0020);
  a0 = 0x8006FCF4;
  a0 += 4352; // 0x1100
  s2 = s2 >> 9;
  s2 = s2 << 22;
  t0 = lw(v0 + 0x0000);
  v1 = lhu(ra + 0x0002);
  a2 = t0 << 4;
  a2 = a2 & 0xFF0;
  a3 = t0 >> 4;
  a3 = a3 & 0xFF0;
  t0 = t0 >> 12;
  a1 = lw(at + 0x0000);
  t0 = t0 & 0xFF0;
  v1 = v1 << 2;
  v1 += v0;
label8001FE38:
  cop2.RGBC = a1;
  cop2.RFC = a2;
  cop2.GFC = a3;
  cop2.BFC = t0;
  t0 = lw(v0 + 0x0004);
  at += 4; // 0x0004
  v0 += 4; // 0x0004
  DPCS();
  a1 = lw(at + 0x0000);
  a2 = t0 << 4;
  a2 = a2 & 0xFF0;
  a3 = t0 >> 4;
  a3 = a3 & 0xFF0;
  t0 = t0 >> 12;
  t1 = cop2.RGB2;
  t0 = t0 & 0xFF0;
  sw(a0 + 0x0000, t1);
  temp = v0 != v1;
  a0 += 4; // 0x0004
  if (temp) goto label8001FE38;
  fp = lw(ra + 0x0014);
  t9 = 0x8006FCF4;
  t9 += 4352; // 0x1100
  goto label8001FF64;
label8001FE98:
  at = cop2.ZSF3;
  fp = lw(ra + 0x001C);
  temp = (int32_t)at > 0;
  t9 = lw(ra + 0x0020);
  if (temp) goto label8001FEB0;
  s2 = 0;
  goto label8001FF64;
label8001FEB0:
  v0 += 3072; // 0x0C00
  temp = (int32_t)v0 >= 0;
  s2 = 0;
  if (temp) goto label8001FF64;
  at = v0 + 2048; // 0x0800
  temp = (int32_t)at > 0;
  v0 = -v0;
  if (temp) goto label8001FECC;
  v0 = 2047; // 0x07FF
label8001FECC:
  s2 = v0 << 1;
  cop2.IR0 = s2;
  at = lw(ra + 0x0020);
  v1 = 0x8006FCF4;
  v1 += 4352; // 0x1100
  t1 = SKYBOX_DATA;
  t1 = lw(t1 + 0x0010);
  v0 = lhu(ra + 0x0002);
  a3 = t1 << 4;
  a3 = a3 & 0xFF0;
  t0 = t1 >> 4;
  t0 = t0 & 0xFF0;
  t1 = t1 >> 12;
  t1 = t1 & 0xFF0;
  a0 = lw(at + 0x0000);
  cop2.RFC = a3;
  cop2.GFC = t0;
  cop2.BFC = t1;
  cop2.RGBC = a0;
  v0 = v0 << 2;
  v0 += at;
label8001FF28:
  DPCS();
  a0 = lw(at + 0x0004);
  at += 4; // 0x0004
  a1 = cop2.RGB2;
  cop2.RGBC = a0;
  sw(v1 + 0x0000, a1);
  temp = at != v0;
  v1 += 4; // 0x0004
  if (temp) goto label8001FF28;
  t9 = 0x8006FCF4;
  t9 += 4352; // 0x1100
  goto label8001FF64;
label8001FF58:
  fp = lw(ra + 0x0014);
  t9 = lw(ra + 0x0018);
  s2 = 0;
label8001FF64:
  sp = cop2.ZSF4;
  lo=gp;
  sp = sp >> 31;
  gp = ra + sp;
  gp = lbu(gp + 0x0008);
  sp = 0x1F800000;
  s3 = s3 >> 31;
  s3 = s3 << 31;
  t8=hi;
  s6 = cop2.BBK;
  s7 = cop2.RBK;
  s6 = s6 << 2;
  s7 = s7 & 0xFF;
  s7 += 4; // 0x0004
  s7 += s3;
  gp -= 128; // 0xFFFFFF80
  temp = (int32_t)gp <= 0;
  gp = sp + 512; // 0x0200
  if (temp) goto label8001FFB8;
  gp = 0x8006FCF4;
  gp += 2304; // 0x0900
label8001FFB8:
  ra = lw(fp + 0x0000);
  temp = (int32_t)s6 > 0;
  at = 0;
  if (temp) goto label8001FFD4;
  at = -s6;
  at = at >> s7;
  at = at << 3;
  s6 = 1; // 0x0001
label8001FFD4:
  s3 = 0x8006FCF4;

  s3 += at;
  s4 = s3 + 2304; // 0x0900
  s5 = s3;
  at = lw(fp + 0x0004);
  fp += 4; // 0x0004
  ra += fp;
label8001FFF8:
  temp = (int32_t)at >= 0;
  s0 = lw(fp + 0x0004);
  if (temp) goto label8002031C;
  temp = ra == fp;
  v0 = at >> 20;
  if (temp) goto label8002074C;
  v0 = v0 & 0x7FC;
  a2 = v0 + sp;
  t6 = at & 0x4;
  temp = (int32_t)t6 > 0;
  t6 = at & 0x8;
  if (temp) goto label800205C4;
  t5 = lw(fp + 0x0008);
  v1 = at >> 11;
  v1 = v1 & 0x7FC;
  a3 = v1 + sp;
  a0 = at >> 2;
  a0 = a0 & 0x7FC;
  t0 = a0 + sp;
  a2 = lw(a2 + 0x0000);
  a3 = lw(a3 + 0x0000);
  t0 = lw(t0 + 0x0000);
  a1 = t5 & 0x7FC;
  t1 = a1 + sp;
  temp = (int32_t)s7 >= 0;
  t1 = lw(t1 + 0x0000);
  if (temp) goto label80020078;
  t7 = a2 & a3;
  t7 = t7 & t0;
  t7 = t7 & t1;
  t7 = t7 & 0x1F;
  temp = (int32_t)t7 > 0;
  a2 = (int32_t)a2 >> 5;
  if (temp) goto label80020300;
  a3 = (int32_t)a3 >> 5;
  t0 = (int32_t)t0 >> 5;
  t1 = (int32_t)t1 >> 5;
label80020078:
  cop2.SXY0 = a2;
  cop2.SXY1 = a3;
  cop2.SXY2 = t0;
  temp = (int32_t)t6 > 0;
  t7 = at & 0x2;
  if (temp) goto label80020304;
  NCLIP();
  temp = t7 == 0;
  sw(t8 + 0x0008, a2);
  if (temp) goto label800200AC;
  fp += 12; // 0x000C
  sw(t8 + 0x0014, a3);
  sw(t8 + 0x0020, t0);
  sw(t8 + 0x002C, t1);
  goto label800200B8;
label800200AC:
  sw(t8 + 0x0010, a3);
  sw(t8 + 0x0018, t0);
  sw(t8 + 0x0020, t1);
label800200B8:
  s1 = at << 31;
  t2 = s0 >> 17;
  t6 = cop2.MAC0;
  cop2.SXY0 = t1;
  temp = s1 == 0;
  t2 = t2 & 0x7FC;
  if (temp) goto label800200DC;
  temp = (int32_t)t6 <= 0;
  if (temp) goto label800200DC;
  t6 = -t6;
label800200DC:
  t2 += t9;
  NCLIP();
  at = lw(fp + 0x000C);
  fp += 12; // 0x000C
  t5 = t5 >> 9;
  t5 = t5 & 0x7FC;
  temp = (int32_t)t6 < 0;
  t5 += t9;
  if (temp) goto label80020130;
  t6 = cop2.MAC0;
  temp = s1 == 0;
  t3 = s0 >> 8;
  if (temp) goto label80020114;
  temp = (int32_t)t6 >= 0;
  if (temp) goto label80020114;
  t6 = -t6;
label80020114:
  temp = (int32_t)t6 <= 0;
  t3 = t3 & 0x7FC;
  if (temp) goto label8001FFF8;
  sw(t8 + 0x0008, t1);
  v0 = a1;
  t2 = t5;
  s1 = 16; // 0x0010
  goto label800203F4;
label80020130:
  t6 = cop2.MAC0;
  temp = s1 == 0;
  t3 = s0 >> 8;
  if (temp) goto label80020148;
  temp = (int32_t)t6 >= 0;
  if (temp) goto label80020148;
  t6 = -t6;
label80020148:
  temp = (int32_t)t6 > 0;
  t3 = t3 & 0x7FC;
  if (temp) goto label80020158;
  s1 = 0;
  goto label800203F4;
label80020158:
  t2 = lw(t2 + 0x0000);
  t3 += t9;
  t4 = s0 << 1;
  t4 = t4 & 0x7FC;
  t4 += t9;
  v0 += gp;
  t3 = lw(t3 + 0x0000);
  t4 = lw(t4 + 0x0000);
  t5 = lw(t5 + 0x0000);
  v1 += gp;
  a0 += gp;
  a1 += gp;
  t2 = t2 << 8;
  t2 = t2 >> 8;
  v0 = lw(v0 + 0x0000);
  v1 = lw(v1 + 0x0000);
  a0 = lw(a0 + 0x0000);
  a1 = lw(a1 + 0x0000);
  temp = (int32_t)t7 > 0;
  v0 -= s6;
  if (temp) goto label8002023C;
  t6 = 0x38000000;
  t2 += t6;
  v0 += v1;
  v0 += a0;
  sw(t8 + 0x0004, t2);
  sw(t8 + 0x000C, t3);
  sw(t8 + 0x0014, t4);
  sw(t8 + 0x001C, t5);
  t6 = (int32_t)s0 >> 28;
  t6 = t6 << 1;
  t6 = t6 << s7;
  v0 += a1;
  temp = (int32_t)v0 < 0;
  a0 = 0x08000000;
  if (temp) goto label8001FFF8;
  sw(t8 + 0x0000, a0);
  v0 += t6;
  temp = (int32_t)v0 < 0;
  v0 = (int32_t)v0 >> s7;
  if (temp) goto label8001FFF8;
  v0 = v0 << 3;
  v0 += s3;
  v1 = lw(v0 + 0x0000);
  a0 = t8 >> 16;
  temp = v1 == 0;
  sw(v0 + 0x0000, t8);
  if (temp) goto label80020218;
  sh(v1 + 0x0000, t8);
  sb(v1 + 0x0002, a0);
  t8 += 36; // 0x0024
  goto label8001FFF8;
label80020218:
  v1 = s4 - v0;
  temp = (int32_t)v1 <= 0;
  v1 = s5 - v0;
  if (temp) goto label80020228;
  s4 = v0;
label80020228:
  temp = (int32_t)v1 >= 0;
  sw(v0 + 0x0004, t8);
  if (temp) goto label80020234;
  s5 = v0;
label80020234:
  t8 += 36; // 0x0024
  goto label8001FFF8;
label8002023C:
  t6 = 0x3C000000;
  t2 += t6;
  v0 += v1;
  a2 = lw(fp - 0x000C); // 0xFFFFFFF4
  v0 += a0;
  t6 = (int32_t)s0 >> 28;
  t6 = t6 << 1;
  t6 = t6 << s7;
  v0 += a1;
  a3 = lw(fp - 0x0008); // 0xFFFFFFF8
  temp = (int32_t)v0 < 0;
  v0 += t6;
  if (temp) goto label8001FFF8;
  temp = (int32_t)v0 < 0;
  v0 = (int32_t)v0 >> s7;
  if (temp) goto label8001FFF8;
  v0 = v0 << 3;
  v0 += s3;
  v1 = lw(v0 + 0x0000);
  t0 = lw(fp - 0x0004); // 0xFFFFFFFC
  sw(t8 + 0x0028, t5);
  sw(t8 + 0x001C, t4);
  sw(t8 + 0x0010, t3);
  s0 = s0 & 0x1;
  s0 = s0 << 25;
  t2 += s0;
  sw(t8 + 0x0004, t2);
  a0 = 0x0C000000;
  sw(t8 + 0x0000, a0);
  a2 += s2;
  sw(t8 + 0x000C, a2);
  sw(t8 + 0x0018, a3);
  sw(t8 + 0x0024, t0);
  t1 = t0 >> 16;
  sw(t8 + 0x0030, t1);
  temp = v1 == 0;
  sw(v0 + 0x0000, t8);
  if (temp) goto label800202DC;
  sh(v1 + 0x0000, t8);
  a0 = t8 >> 16;
  sb(v1 + 0x0002, a0);
  t8 += 52; // 0x0034
  goto label8001FFF8;
label800202DC:
  v1 = s4 - v0;
  temp = (int32_t)v1 <= 0;
  v1 = s5 - v0;
  if (temp) goto label800202EC;
  s4 = v0;
label800202EC:
  temp = (int32_t)v1 >= 0;
  sw(v0 + 0x0004, t8);
  if (temp) goto label800202F8;
  s5 = v0;
label800202F8:
  t8 += 52; // 0x0034
  goto label8001FFF8;
label80020300:
  t7 = at & 0x2;
label80020304:
  temp = t7 == 0;
  fp += 12; // 0x000C
  if (temp) goto label80020310;
  fp += 12; // 0x000C
label80020310:
  at = lw(fp + 0x0000);
  goto label8001FFF8;
label8002031C:
  temp = ra == fp;
  v0 = at >> 20;
  if (temp) goto label8002074C;
  v0 = v0 & 0x7FC;
  a2 = v0 + sp;
  v1 = at >> 11;
  v1 = v1 & 0x7FC;
  a3 = v1 + sp;
  a0 = at >> 2;
  a0 = a0 & 0x7FC;
  t0 = a0 + sp;
  a2 = lw(a2 + 0x0000);
  a3 = lw(a3 + 0x0000);
  temp = (int32_t)s7 >= 0;
  t0 = lw(t0 + 0x0000);
  if (temp) goto label80020370;
  t7 = a2 & a3;
  t7 = t7 & t0;
  t7 = t7 & 0x1F;
  temp = (int32_t)t7 > 0;
  a2 = (int32_t)a2 >> 5;
  if (temp) goto label800205A8;
  a3 = (int32_t)a3 >> 5;
  t0 = (int32_t)t0 >> 5;
label80020370:
  cop2.SXY0 = a2;
  cop2.SXY1 = a3;
  cop2.SXY2 = t0;
  t6 = at & 0x8;
  t7 = at & 0x2;
  NCLIP();
  temp = t7 == 0;
  sw(t8 + 0x0008, a2);
  if (temp) goto label800203A0;
  fp += 12; // 0x000C
  sw(t8 + 0x0014, a3);
  sw(t8 + 0x0020, t0);
  goto label800203A8;
label800203A0:
  sw(t8 + 0x0010, a3);
  sw(t8 + 0x0018, t0);
label800203A8:
  temp = (int32_t)t6 > 0;
  t6 = at & 0x1;
  if (temp) goto label800205B8;
  temp = (int32_t)t6 > 0;
  s1 = 0;
  if (temp) goto label800203D0;
  t6 = cop2.MAC0;
  fp += 8; // 0x0008
  temp = (int32_t)t6 > 0;
  at = lw(fp + 0x0000);
  if (temp) goto label800203E0;
  goto label8001FFF8;
label800203D0:
  fp += 8; // 0x0008
  t6 = cop2.MAC0;
  at = lw(fp + 0x0000);
  temp = t6 == 0;
  if (temp) {
    t2 = s0 >> 17;
    goto label8001FFF8;
  }
label800203E0:
  t2 = s0 >> 17;
  t2 = t2 & 0x7FC;
  t2 += t9;
  t3 = s0 >> 8;
  t3 = t3 & 0x7FC;
label800203F4:
  t2 = lw(t2 + 0x0000);
  t3 += t9;
  t4 = s0 << 1;
  t4 = t4 & 0x7FC;
  t4 += t9;
  v0 += gp;
  t3 = lw(t3 + 0x0000);
  t4 = lw(t4 + 0x0000);
  v1 += gp;
  a0 += gp;
  t2 = t2 << 8;
  t2 = t2 >> 8;
  temp = (int32_t)t7 > 0;
  v0 = lw(v0 + 0x0000);
  if (temp) goto label800204D0;
  v1 = lw(v1 + 0x0000);
  t6 = 0x30000000;
  t2 += t6;
  t6 = v0 >> 1;
  v0 += t6;
  v0 -= s6;
  a0 = lw(a0 + 0x0000);
  v0 += v1;
  v1 = v1 >> 1;
  v0 += v1;
  t6 = (int32_t)s0 >> 28;
  t6 = t6 << 1;
  sw(t8 + 0x0004, t2);
  sw(t8 + 0x000C, t3);
  sw(t8 + 0x0014, t4);
  v0 += a0;
  temp = (int32_t)v0 < 0;
  v0 = (int32_t)v0 >> s7;
  if (temp) goto label8001FFF8;
  v0 += t6;
  temp = (int32_t)v0 < 0;
  a0 = 0x06000000;
  if (temp) goto label8001FFF8;
  sw(t8 + 0x0000, a0);
  v0 = v0 << 3;
  v0 += s3;
  v1 = lw(v0 + 0x0000);
  a0 = t8 >> 16;
  temp = v1 == 0;
  sw(v0 + 0x0000, t8);
  if (temp) goto label800204AC;
  sh(v1 + 0x0000, t8);
  sb(v1 + 0x0002, a0);
  t8 += 28; // 0x001C
  goto label8001FFF8;
label800204AC:
  v1 = s4 - v0;
  temp = (int32_t)v1 <= 0;
  v1 = s5 - v0;
  if (temp) goto label800204BC;
  s4 = v0;
label800204BC:
  temp = (int32_t)v1 >= 0;
  sw(v0 + 0x0004, t8);
  if (temp) goto label800204C8;
  s5 = v0;
label800204C8:
  t8 += 28; // 0x001C
  goto label8001FFF8;
label800204D0:
  v1 = lw(v1 + 0x0000);
  a0 = lw(a0 + 0x0000);
  t6 = v0 >> 1;
  v0 += t6;
  v0 -= s6;
  v0 += v1;
  v1 = v1 >> 1;
  a2 = lw(fp - 0x000C); // 0xFFFFFFF4
  v0 += v1;
  t6 = (int32_t)s0 >> 28;
  t6 = t6 << 1;
  v0 += a0;
  temp = (int32_t)v0 < 0;
  v0 = (int32_t)v0 >> s7;
  if (temp) goto label8001FFF8;
  a3 = lw(fp - 0x0008); // 0xFFFFFFF8
  v0 += t6;
  temp = (int32_t)v0 < 0;
  v0 = v0 << 3;
  if (temp) goto label8001FFF8;
  v0 += s3;
  t6 = 0x34000000;
  t2 += t6;
  v1 = lw(v0 + 0x0000);
  t0 = lw(fp - 0x0004); // 0xFFFFFFFC
  sw(t8 + 0x001C, t4);
  sw(t8 + 0x0010, t3);
  s0 = s0 & 0x1;
  s0 = s0 << 25;
  t2 += s0;
  sw(t8 + 0x0004, t2);
  a2 += s2;
  temp = (int32_t)s1 <= 0;
  sw(t8 + 0x000C, a2);
  if (temp) goto label80020558;
  a2 = t0 >> 16;
  sh(t8 + 0x000C, a2);
label80020558:
  a0 = 0x09000000;
  sw(t8 + 0x0000, a0);
  sw(t8 + 0x0018, a3);
  sw(t8 + 0x0024, t0);
  temp = v1 == 0;
  sw(v0 + 0x0000, t8);
  if (temp) goto label80020584;
  sh(v1 + 0x0000, t8);
  a0 = t8 >> 16;
  sb(v1 + 0x0002, a0);
  t8 += 40; // 0x0028
  goto label8001FFF8;
label80020584:
  v1 = s4 - v0;
  temp = (int32_t)v1 <= 0;
  v1 = s5 - v0;
  if (temp) goto label80020594;
  s4 = v0;
label80020594:
  temp = (int32_t)v1 >= 0;
  sw(v0 + 0x0004, t8);
  if (temp) goto label800205A0;
  s5 = v0;
label800205A0:
  t8 += 40; // 0x0028
  goto label8001FFF8;
label800205A8:
  t7 = at & 0x2;
  temp = t7 == 0;
  if (temp) goto label800205B8;
  fp += 12; // 0x000C
label800205B8:
  at = lw(fp + 0x0008);
  fp += 8; // 0x0008
  goto label8001FFF8;
label800205C4:
  a2 = lw(a2 + 0x0000);
  temp = (int32_t)s7 >= 0;
  t2 = a2 & 0x1F;
  if (temp) goto label800205D8;
  temp = (int32_t)t2 > 0;
  a2 = (int32_t)a2 >> 5;
  if (temp) goto label80020740;
label800205D8:
  v0 += gp;
  v0 = lw(v0 + 0x0000);
  a3 = s0 >> 17;
  a3 = a3 & 0x7FC;
  a3 += t9;
  a0 = s0 >> 10;
  a0 = a0 & 0x1FF;
  v1 = 256; // 0x0100
  cop2.DQA = v1;
  cop2.DQB = 0;
  cop2.RTM3 = 0;
  cop2.RTM4 = 0;
  cop2.TRZ = v0;
  a1 = s0 >> 1;
  a1 = a1 & 0x1FF;
  RTPS();
  a3 = lw(a3 + 0x0000);
  v0 = v0 << 2;
  v1 = (int32_t)s0 >> 28;
  v1 = v1 << 1;
  v1 = v1 << s7;
  v0 += t6;
  v0 -= s6;
  at = lw(fp + 0x0014);
  fp += 20; // 0x0014
  temp = (int32_t)v0 <= 0;
  v0 = (int32_t)v0 >> s7;
  if (temp) goto label8001FFF8;
  v0 = v0 << 3;
  v0 += s3;
  v1 = cop2.MAC0;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  v1 = v1 >> 12;
  cop2.IR0 = v1;
  a3 = a3 << 8;
  a3 = a3 >> 8;
  GPF(SF_OFF, LM_OFF);
  v1 = 0x2C000000;
  a3 += v1;
  s0 = s0 & 0x1;
  s0 = s0 << 25;
  a3 += s0;
  v1 = cop2.MAC1;
  a0 = cop2.MAC2;
  v1 = v1 >> 11;
  a0 = a0 >> 11;
  t0 = lw(fp - 0x000C); // 0xFFFFFFF4
  t1 = lw(fp - 0x0008); // 0xFFFFFFF8
  t2 = lw(fp - 0x0004); // 0xFFFFFFFC
  a1 = a2 & 0xFFFF;
  a2 = a2 >> 16;
  t3 = v1 >> 1;
  a1 += t3;
  v1 = a1 - v1;
  sw(t8 + 0x0004, a3);
  sw(t8 + 0x000C, t0);
  sw(t8 + 0x0014, t1);
  sw(t8 + 0x001C, t2);
  t2 = t2 >> 16;
  sw(t8 + 0x0024, t2);
  t3 = a0 >> 1;
  a2 += t3;
  a0 = a2 - a0;
  sh(t8 + 0x0008, v1);
  sh(t8 + 0x000A, a0);
  sh(t8 + 0x0010, a1);
  sh(t8 + 0x0012, a0);
  sh(t8 + 0x0018, v1);
  sh(t8 + 0x001A, a2);
  sh(t8 + 0x0020, a1);
  sh(t8 + 0x0022, a2);
  v1 = 0x09000000;
  sw(t8 + 0x0000, v1);
  v1 = lw(v0 + 0x0000);
  a0 = t8 >> 16;
  temp = v1 == 0;
  sw(v0 + 0x0000, t8);
  if (temp) goto label8002071C;
  sh(v1 + 0x0000, t8);
  sb(v1 + 0x0002, a0);
  t8 += 40; // 0x0028
  goto label8001FFF8;
label8002071C:
  v1 = s4 - v0;
  temp = (int32_t)v1 <= 0;
  v1 = s5 - v0;
  if (temp) goto label8002072C;
  s4 = v0;
label8002072C:
  temp = (int32_t)v1 >= 0;
  sw(v0 + 0x0004, t8);
  if (temp) goto label80020738;
  s5 = v0;
label80020738:
  t8 += 40; // 0x0028
  goto label8001FFF8;
label80020740:
  at = lw(fp + 0x0014);
  fp += 20; // 0x0014
  goto label8001FFF8;
label8002074C:
  at = s5 - s4;
  temp = (int32_t)at < 0;
  gp=lo;
  if (temp) goto label80020860;
  at = cop2.GBK;
  v0 = cop2.RBK;
  at = at << 3;
  v1 = v0 & 0xFF;
  v0 = v0 >> 8;
  v0 += v1;
  v1 = 32; // 0x0020
  v1 = v1 << v0;
  v1 -= 8; // 0xFFFFFFF8
  at += v1;
  v1 = 0x8006FCF4;
  v1 += 2048; // 0x0800
  v1 -= s5;
  temp = (int32_t)v1 >= 0;
  v1 = v1 << v0;
  if (temp) goto label8002079C;
  v1 = 0;
label8002079C:
  v1 = v1 >> 8;
  v1 = v1 << 3;
  at -= v1;
  temp = (int32_t)at >= 0;
  a0 = s5;
  if (temp) goto label800207B4;
  at = 0;
label800207B4:
  v1 = 256; // 0x0100
  v1 = v1 >> v0;
  v0 = ordered_linked_list;
  v0 = lw(v0 + 0x0000);
  a2 = s4 - 8; // 0xFFFFFFF8
  at += v0;
label800207D0:
  a3 = lw(at + 0x0000);
  a1 = a0 - v1;
  t0 = a2 - a1;
  temp = (int32_t)t0 <= 0;
  if (temp) goto label800207E8;
  a1 = a2;
label800207E8:
  temp = a3 != 0;
  if (temp) goto label80020814;
label800207F0:
  temp = a0 == a1;
  t0 = lw(a0 + 0x0004);
  if (temp) goto label80020844;
  a0 -= 8; // 0xFFFFFFF8
  temp = t0 == 0;
  if (temp) goto label800207F0;
  a3 = lw(a0 + 0x0008);
  sw(at + 0x0004, t0);
  sw(a0 + 0x000C, 0);
  sw(a0 + 0x0008, 0);
label80020814:
  temp = a0 == a1;
  t0 = lw(a0 + 0x0004);
  if (temp) goto label80020844;
  a0 -= 8; // 0xFFFFFFF8
  temp = t0 == 0;
  t1 = t0 >> 16;
  if (temp) goto label80020814;
  t2 = lw(a0 + 0x0008);
  sh(a3 + 0x0000, t0);
  sb(a3 + 0x0002, t1);
  a3 = t2;
  sw(a0 + 0x0008, 0);
  temp = a0 != a1;
  sw(a0 + 0x000C, 0);
  if (temp) goto label80020814;
label80020844:
  temp = a1 == a2;
  sw(at + 0x0000, a3);
  if (temp) goto label8002085C;
  temp = at != v0;
  at -= 8; // 0xFFFFFFF8
  if (temp) goto label800207D0;
  at += 8; // 0x0008
  goto label800207D0;
label8002085C:
  hi=t8;
label80020860:
  at = 0x8006FCF4;
  at += 10752; // 0x2A00
label8002086C:
  v0 = lw(at + 0x0000);
  at += 4; // 0x0004
  temp = v0 == 0;
  v1 = v0 & 0xFF;
  if (temp) goto label800208A4;
  v0 = v0 >> 8;
  a0 = 0x80000000;
  v0 = v0 | a0;
label80020888:
  temp = v1 == 0;
  v1--;
  if (temp) goto label8002086C;
  a0 = lw(at + 0x0000);
  at += 4; // 0x0004
  sw(v0 + 0x0000, a0);
  v0 += 4; // 0x0004
  goto label80020888;
label800208A4:
  gp += 56; // 0x0038
  goto label8001F7FC;
label800208AC:
  v0=hi;
  at = allocator1_ptr;
  sw(at + 0x0000, v0);
  at = 0x80077DD8;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  return;
}

// size: 0x00000638
void function_800208FC(void)
{
  uint32_t temp;
  at = 0x80077DD8;
  sw(at + 0x0000, s0);
  sw(at + 0x0004, s1);
  sw(at + 0x0008, s2);
  sw(at + 0x000C, s3);
  sw(at + 0x0010, s4);
  sw(at + 0x0014, s5);
  sw(at + 0x0018, s6);
  sw(at + 0x001C, s7);
  sw(at + 0x0020, gp);
  sw(at + 0x0024, sp);
  sw(at + 0x0028, fp);
  sw(at + 0x002C, ra);
  at = 0x80075EF8;
  at = lw(at + 0x0008);
  fp = 0x8006FCF4;
  ra = fp + 8704; // 0x2200
  fp += 5632; // 0x1600
  lo=at;
  t9 = 0x80076DD0;
  s2 = lw(t9 + 0x0028);
  s3 = lw(t9 + 0x002C);
  s4 = lw(t9 + 0x0030);
  s5 = lw(t9 + 0x0000);
  s6 = lw(t9 + 0x0004);
  s7 = lw(t9 + 0x0008);
  t8 = lw(t9 + 0x000C);
  t9 = lw(t9 + 0x0010);
  s0 = spyro_sin_lut;
  s1 = spyro_cos_lut;
  gp = 0x80076378;
label80020994:
  sp = lw(ra + 0x0000);
  ra += 4; // 0x0004
  temp = sp == 0;
  a0 = lhu(sp + 0x0050);
  if (temp) goto label80020EE0;
  at = lw(sp + 0x000C);
  sb(sp + 0x0051, 0);
  a1 = a0 & 0x100;
  a1 = a1 << 1;
  a0 = a0 & 0xFF;
  a0 = a0 << 8;
  t4 = a0 + a1;
  v0 = lw(sp + 0x0010);
  at -= s2;
  at = (int32_t)at >> 2;
  a1 = at + t4;
  temp = (int32_t)a1 <= 0;
  a1 = at - t4;
  if (temp) goto label80020994;
  temp = (int32_t)a1 >= 0;
  v0 = s3 - v0;
  if (temp) goto label80020994;
  v1 = lw(sp + 0x0014);
  v0 = (int32_t)v0 >> 2;
  a1 = v0 + t4;
  temp = (int32_t)a1 <= 0;
  a1 = v0 - t4;
  if (temp) goto label80020994;
  temp = (int32_t)a1 >= 0;
  v1 = s4 - v1;
  if (temp) goto label80020994;
  v1 = (int32_t)v1 >> 2;
  a1 = v1 + t4;
  temp = (int32_t)a1 <= 0;
  a1 = v1 - t4;
  if (temp) goto label80020994;
  temp = (int32_t)a1 >= 0;
  if (temp) goto label80020994;
  t7 = lhu(sp + 0x0036);
  t6 = lw(sp + 0x003C);
  t7 = t7 << 2;
  t7 += gp;
  t7 = lw(t7 + 0x0000);
  t5 = t6 & 0xFF;
  t5 = t5 << 2;
  t5 += t7;
  t5 = lw(t5 + 0x0038);
  cop2.RTM0 = s5;
  cop2.RTM1 = s6;
  cop2.RTM2 = s7;
  cop2.RTM3 = t8;
  cop2.RTM4 = t9;
  a1 = lbu(t5 + 0x0007);
  cop2.IR3 = at;
  cop2.IR1 = v0;
  cop2.IR2 = v1;
  a1 = a1 << 4;
  a2 = a1 >> 1;
  MVMVA(SF_ON, MX_RT, V_IR, CV_NONE, LM_OFF);
  a3 = a1 >> 1;
  t0 = a1 >> 2;
  a2 += t0;
  t0 = a1 >> 4;
  a3 += t0;
  t0 = a1 >> 5;
  a2 += t0;
  a3 += t0;
  v1 = cop2.MAC3;
  v0 = cop2.MAC2;
  at = cop2.MAC1;
  t0 = v1 - t4;
  temp = (int32_t)t0 >= 0;
  t0 = v1 + a1;
  if (temp) goto label80020994;
  temp = (int32_t)t0 <= 0;
  t4 = v1;
  if (temp) goto label80020994;
  temp = (int32_t)at >= 0;
  t2 = at;
  if (temp) goto label80020AB4;
  t2 = -at;
label80020AB4:
  t3 = v1 + a3;
  t2 -= a2;
  t2 = t2 << 2;
  t2 -= t3;
  t3 = t3 << 1;
  t2 -= t3;
  temp = (int32_t)t2 >= 0;
  t3 = lw(sp + 0x001C);
  if (temp) goto label80020994;
  t0 = a1;
  t1 = a1 >> 2;
  t2 = a1 >> 4;
  t1 += t2;
  t2 = a1 >> 5;
  t0 -= t2;
  t2 = a1 >> 6;
  temp = (int32_t)t3 >= 0;
  t0 -= t2;
  if (temp) goto label80020B28;
  t3 = v1 - 4608; // 0xFFFFEE00
  temp = (int32_t)t3 >= 0;
  t3=lo;
  if (temp) goto label80020B28;
  a0 = lbu(sp + 0x003E);
  sw(t3 + 0x0000, sp);
  a0 = a0 << 3;
  a0 += t5;
  a0 += 6; // 0x0006
  a0 = lbu(a0 + 0x0024);
  t3 += 8; // 0x0008
  sw(t3 - 0x0004, a0); // 0xFFFFFFFC
  lo=t3;
label80020B28:
  temp = (int32_t)v0 >= 0;
  t2 = v0;
  if (temp) goto label80020B34;
  t2 = -v0;
label80020B34:
  t3 = v1 + t1;
  t2 -= t0;
  t3 -= t2;
  t2 = t2 << 1;
  t3 -= t2;
  temp = (int32_t)t3 <= 0;
  if (temp) goto label80020994;
  temp = (int32_t)at >= 0;
  t2 = at;
  if (temp) goto label80020B5C;
  t2 = -at;
label80020B5C:
  t3 = v1 - a3;
  t2 += a2;
  t2 = t2 << 2;
  t2 -= t3;
  t3 = t3 << 1;
  t2 -= t3;
  temp = (int32_t)t2 >= 0;
  if (temp) goto label80020BAC;
  temp = (int32_t)v0 >= 0;
  t2 = v0;
  if (temp) goto label80020B88;
  t2 = -v0;
label80020B88:
  t3 = v1 - t1;
  t2 += t0;
  t3 -= t2;
  t2 = t2 << 1;
  t3 -= t2;
  temp = (int32_t)t3 <= 0;
  if (temp) goto label80020BAC;
  a0 = 0x40000000;
  goto label80020BB0;
label80020BAC:
  a0 = 0x80000000;
label80020BB0:
  t2 = lbu(sp + 0x004B);
  a1 = lbu(sp + 0x0040);
  t3 = lw(sp + 0x0044);
  t2 = t2 & 0x3F;
  t2 = t2 << 8;
  t2 -= t4;
  temp = a1 == 0;
  a1 = a1 << 8;
  if (temp) goto label80020C3C;
  a0 += a1;
  t4 = t6 & 0xFF00;
  t4 = t4 >> 6;
  t4 += t7;
  t4 = lw(t4 + 0x0038);
  a1 = t6 >> 16;
  a1 = a1 & 0xFF;
  a1 = a1 << 3;
  a1 += 36; // 0x0024
  a1 += t5;
  a3 = lbu(t5 + 0x000B);
  a2 = t6 >> 24;
  a2 = a2 << 3;
  a2 += 36; // 0x0024
  a2 += t4;
  t0 = lbu(t4 + 0x000B);
  t1 = t3 >> 24;
  t1 = t1 << 16;
  a0 += t1;
  t1 = a3 - t0;
  temp = (int32_t)t1 >= 0;
  t1 = lbu(sp + 0x0057);
  if (temp) goto label80020C2C;
  a3 = t0;
label80020C2C:
  a3 = a3 << 24;
  a0 += a3;
  a0 += t1;
  goto label80020C74;
label80020C3C:
  a3 = lbu(t5 + 0x000B);
  a1 = t6 >> 16;
  a1 = a1 & 0xFF;
  a1 = a1 << 3;
  a1 += 36; // 0x0024
  a1 += t5;
  t0 = lbu(sp + 0x0057);
  t1 = t3 >> 24;
  t1 = t1 << 16;
  a0 += t1;
  a3 = a3 << 24;
  a0 += a3;
  a2 = 0;
  a0 += t0;
label80020C74:
  a3 = lw(sp + 0x004C);
  t0 = 1; // 0x0001
  sb(sp + 0x0051, t0);
  sw(fp + 0x0000, a0);
  sw(fp + 0x0004, t5);
  sw(fp + 0x0008, a1);
  sw(fp + 0x000C, a2);
  sw(fp + 0x0010, at);
  sw(fp + 0x0014, v0);
  sw(fp + 0x0018, v1);
  sh(fp + 0x002E, t2);
  sw(fp + 0x0030, a3);
  sw(fp + 0x0034, sp);
  t4 = t3;
  a3 = s5;
  t0 = s6;
  t1 = s7;
  t2 = t8;
  t3 = t9;
  at = t4 >> 15;
  at = at & 0x1FE;
  temp = at == 0;
  v0 = at + s1;
  if (temp) goto label80020D70;
  at += s0;
  v0 = lhu(v0 + 0x0000);
  at = lhu(at + 0x0000);
  v0 = v0 & 0xFFFF;
  cop2.VXY0 = v0;
  cop2.VZ0 = at;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  at = -at;
  at = at & 0xFFFF;
  a0 = cop2.IR1;
  a1 = cop2.IR2;
  a2 = cop2.IR3;
  cop2.VXY0 = at;
  cop2.VZ0 = v0;
  v1 = 0xFFFF0000;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  a3 = a3 & v1;
  a0 = a0 & 0xFFFF;
  a3 += a0;
  t2 = t2 & v1;
  a2 = a2 & 0xFFFF;
  t2 += a2;
  a1 = a1 << 16;
  t1 = t1 & 0xFFFF;
  at = cop2.IR1;
  v0 = cop2.IR2;
  v1 = cop2.IR3;
  at = at & 0xFFFF;
  t0 = at + a1;
  v0 = v0 << 16;
  t1 += v0;
  t3 = v1 & 0xFFFF;
  cop2.RTM0 = a3;
  cop2.RTM1 = t0;
  cop2.RTM2 = t1;
  cop2.RTM3 = t2;
  cop2.RTM4 = t3;
label80020D70:
  at = t4 & 0xFF00;
  temp = at == 0;
  at = at >> 7;
  if (temp) goto label80020E20;
  v0 = at + s1;
  at += s0;
  v0 = lhu(v0 + 0x0000);
  at = lhu(at + 0x0000);
  v1 = v0 << 16;
  cop2.VXY0 = v1;
  cop2.VZ0 = at;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  v1 = at << 16;
  v1 = -v1;
  a0 = cop2.IR1;
  a1 = cop2.IR2;
  a2 = cop2.IR3;
  cop2.VXY0 = v1;
  cop2.VZ0 = v0;
  v1 = 0xFFFF0000;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  a3 = a3 & 0xFFFF;
  a0 = a0 << 16;
  a3 += a0;
  t2 = t2 & 0xFFFF;
  a2 = a2 << 16;
  t2 += a2;
  a1 = a1 & 0xFFFF;
  t0 = t0 & v1;
  at = cop2.IR1;
  v0 = cop2.IR2;
  v1 = cop2.IR3;
  at = at & 0xFFFF;
  t0 += at;
  v0 = v0 << 16;
  t1 = v0 + a1;
  t3 = v1 & 0xFFFF;
  cop2.RTM0 = a3;
  cop2.RTM1 = t0;
  cop2.RTM2 = t1;
  cop2.RTM3 = t2;
  cop2.RTM4 = t3;
label80020E20:
  at = t4 & 0xFF;
  temp = at == 0;
  at = at << 1;
  if (temp) goto label80020EC4;
  v0 = at + s1;
  at += s0;
  v0 = lhu(v0 + 0x0000);
  at = lhu(at + 0x0000);
  cop2.VZ0 = 0;
  v1 = at << 16;
  v1 += v0;
  cop2.VXY0 = v1;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  at = -at;
  at = at & 0xFFFF;
  v0 = v0 << 16;
  at += v0;
  a0 = cop2.IR1;
  a1 = cop2.IR2;
  a2 = cop2.IR3;
  cop2.VXY0 = at;
  cop2.VZ0 = 0;
  v1 = 0xFFFF0000;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  a1 = a1 << 16;
  t0 = t0 & 0xFFFF;
  t0 += a1;
  a0 = a0 & 0xFFFF;
  a2 = a2 & 0xFFFF;
  t1 = t1 & v1;
  at = cop2.IR1;
  v0 = cop2.IR2;
  v1 = cop2.IR3;
  at = at << 16;
  a3 = at + a0;
  v1 = v1 << 16;
  t2 = v1 + a2;
  v0 = v0 & 0xFFFF;
  t1 += v0;
label80020EC4:
  sw(fp + 0x001C, a3);
  sw(fp + 0x0020, t0);
  sw(fp + 0x0024, t1);
  sw(fp + 0x0028, t2);
  sh(fp + 0x002C, t3);
  fp += 56; // 0x0038
  goto label80020994;
label80020EE0:
  sw(fp + 0x0000, 0);
  at=lo;
  v0 = 0x80075EF8;
  sw(v0 + 0x0008, at);
  at = 0x80077DD8;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  return;
}

// size: 0x00001AF8
void function_80020F34(void)
{
  uint32_t temp;
  at = 0x80077DD8;
  sw(at + 0x0000, s0);
  sw(at + 0x0004, s1);
  sw(at + 0x0008, s2);
  sw(at + 0x000C, s3);
  sw(at + 0x0010, s4);
  sw(at + 0x0014, s5);
  sw(at + 0x0018, s6);
  sw(at + 0x001C, s7);
  sw(at + 0x0020, gp);
  sw(at + 0x0024, sp);
  sw(at + 0x0028, fp);
  sw(at + 0x002C, ra);
  at = allocator1_ptr;
  at = lw(at + 0x0000);
  gp = 0x8006FCF4;
  gp += 5632; // 0x1600
  v0 = 0x8007591C;
  v0 = lw(v0 + 0x0000);
  hi=at;
  cop2.ZSF3 = v0;
label80020F98:
  s3 = lw(gp + 0x0000);
  ra = lw(gp + 0x0004);
  temp = s3 == 0;
  s4 = s3 & 0xFF00;
  if (temp) goto label800229DC;
  s4 = s4 >> 2;
  a0 = s3 << 8;
  a0 = (int32_t)a0 >> 24;
  v1 = lw(gp + 0x0018);
  v0 = lw(gp + 0x0014);
  at = lw(gp + 0x0010);
  a2 = (int32_t)v1 >> 5;
  a2 -= a0;
  temp = (int32_t)a2 >= 0;
  a0 = lbu(ra + 0x0005);
  if (temp) goto label80020FD4;
  a2 = 0;
label80020FD4:
  a3 = a2 - 272; // 0xFFFFFEF0
  temp = (int32_t)a3 < 0;
  a1 = s3 >> 24;
  if (temp) goto label80020FE4;
  a2 += 32; // 0x0020
label80020FE4:
  a1 = a1 & 0x1F;
  at = at << 2;
  v0 = v0 << 2;
  v1 = v1 << 2;
  at = (int32_t)at >> a0;
  v0 = (int32_t)v0 >> a0;
  v1 = (int32_t)v1 >> a0;
  a3 = s3 & 0xFF;
  temp = a3 == 0;
  s5 = lbu(ra + 0x0006);
  if (temp) goto label8002104C;
  cop2.IR0 = a3;
  cop2.IR1 = at;
  cop2.IR2 = v0;
  cop2.IR3 = v1;
  GPF(SF_OFF, LM_OFF);
  v1 = cop2.MAC3;
  v0 = cop2.MAC2;
  at = cop2.MAC1;
  v1 = (int32_t)v1 >> 5;
  v0 = (int32_t)v0 >> 5;
  a3 = v1 - 31500; // 0xFFFF84F4
  a3 -= 31500; // 0xFFFF84F4
  temp = (int32_t)a3 > 0;
  at = (int32_t)at >> 5;
  if (temp) goto label800229D4;
label8002104C:
  a3 = 512; // 0x0200
  a3 = a3 << a1;
  a3 = v1 - a3;
  s5++;
  a0 = a0 << 8;
  a1 += a0;
  cop2.TRX = at;
  cop2.TRY = v0;
  cop2.TRZ = v1;
  cop2.RBK = a1;
  cop2.GBK = a2;
  cop2.BBK = a3;
  at = lw(gp + 0x001C);
  v0 = lw(gp + 0x0020);
  v1 = lw(gp + 0x0024);
  a0 = lw(gp + 0x0028);
  a1 = lw(gp + 0x002C);
  cop2.RTM0 = at;
  cop2.RTM1 = v0;
  cop2.RTM2 = v1;
  cop2.RTM3 = a0;
  cop2.RTM4 = a1;
  a1 = (int32_t)a1 >> 16;
  cop2.ZSF4 = a1;
  t2 = lw(gp + 0x0008);
  t4 = lw(gp + 0x000C);
  t3 = lw(t2 + 0x0004);
  v0 = 0x8006FCF4;
  temp = (int32_t)a1 <= 0;
  v0 += 10752; // 0x2A00
  if (temp) goto label80021134;
  at = t3 << 16;
  at = at >> 14;
  temp = at == 0;
  at += t2;
  if (temp) goto label80021134;
  v1 = lw(ra + 0x0014);
label800210DC:
  a0 = lw(at + 0x0000);
  at += 4; // 0x0004
  a1 = a0 & 0xFFFF;
  a1 += v1;
  a2 = a0 >> 24;
  a2 = a2 & 0x7F;
  a3 = a1 << 8;
  a3 += a2;
  sw(v0 + 0x0000, a3);
  v0 += 4; // 0x0004
label80021104:
  temp = a2 == 0;
  a2--;
  if (temp) goto label8002112C;
  a3 = lw(a1 + 0x0000);
  a1 += 4; // 0x0004
  sw(v0 + 0x0000, a3);
  v0 += 4; // 0x0004
  a3 = lw(at + 0x0000);
  at += 4; // 0x0004
  sw(a1 - 0x0004, a3); // 0xFFFFFFFC
  goto label80021104;
label8002112C:
  temp = (int32_t)a0 > 0;
  if (temp) goto label800210DC;
label80021134:
  sw(v0 + 0x0000, 0);
  v0 = lw(t2 + 0x0000);
  at = t3 >> 24;
  at = at << 2;
  v0 = v0 << 11;
  v0 = v0 >> 11;
  temp = t4 == 0;
  at += v0;
  if (temp) goto label80021170;
  a3 = lw(t4 + 0x0004);
  t0 = lw(t4 + 0x0000);
  a3 = a3 >> 24;
  a3 = a3 << 2;
  t0 = t0 << 11;
  t0 = t0 >> 11;
  a3 += t0;
label80021170:
  fp = cop2.ZSF4;
  lo=ra;
  fp = fp >> 31;
  ra += fp;
  ra = lbu(ra + 0x0008);
  s6 = -1; // 0xFFFFFFFF
  s7 = 0x00010000;
  t8 = 0x01000000;
  t9 = 0x02000000;
  fp = 0x1F800000;
  temp = ra == 0;
  a2 = lw(v0 + 0x0000);
  if (temp) goto label80022990;
  sp = 0x8006FCF4;
  sp += 2304; // 0x0900
  ra = ra << 2;
  ra += fp;
  v0 += 4; // 0x0004
  v1 = (int32_t)a2 >> 21;
  a0 = a2 << 10;
  a0 = (int32_t)a0 >> 21;
  a1 = a2 << 20;
  a1 = (int32_t)a1 >> 19;
  temp = s4 == 0;
  a2 = a2 & 0x1;
  if (temp) goto label80021488;
  t4 = lw(t0 + 0x0000);
  t0 += 4; // 0x0004
  t1 = (int32_t)t4 >> 21;
  t2 = t4 << 10;
  t2 = (int32_t)t2 >> 21;
  t3 = t4 << 20;
  t3 = (int32_t)t3 >> 19;
  cop2.IR0 = s4;
  cop2.IR1 = v1;
  cop2.IR2 = a0;
  cop2.IR3 = a1;
  cop2.RFC = t1;
  cop2.GFC = t2;
  cop2.BFC = t3;
  t4 = t4 & 0x1;
  INTPL();
  temp = a2 != 0;
  if (temp) goto label80021228;
  t5 = lw(v0 + 0x0000);
  goto label8002122C;
label80021228:
  t5 = lh(at + 0x0000);
label8002122C:
  t6 = cop2.MAC1;
  t7 = cop2.MAC2;
  s0 = cop2.MAC3;
  cop2.VZ0 = t6;
  s0 = s0 << 16;
  t7 += s0;
  cop2.VXY0 = t7;
label80021248:
  temp = a2 != 0;
  sp += 8; // 0x0008
  if (temp) goto label80021328;
  RTPS();
  temp = t4 != 0;
  v1 = (int32_t)t5 >> 21;
  if (temp) goto label80021264;
  t6 = lw(t0 + 0x0000);
  goto label80021268;
label80021264:
  t6 = lh(a3 + 0x0000);
label80021268:
  t7 = t5 << 10;
  a0 = (int32_t)t7 >> 21;
  t7 = t5 << 20;
  a1 = (int32_t)t7 >> 19;
  v0 += 4; // 0x0004
  temp = t4 != 0;
  a2 = t5 & 0x1;
  if (temp) goto label80021374;
label80021284:
  t1 = (int32_t)t6 >> 21;
  t5 = t6 << 10;
  t2 = (int32_t)t5 >> 21;
  t5 = t6 << 20;
  t3 = (int32_t)t5 >> 19;
  cop2.IR0 = s4;
  cop2.IR1 = v1;
  cop2.IR2 = a0;
  cop2.IR3 = a1;
  cop2.RFC = t1;
  cop2.GFC = t2;
  cop2.BFC = t3;
  t7 = cop2.MAC1;
  s0 = cop2.MAC2;
  s1 = cop2.MAC3;
  sh(sp - 0x0006, t7); // 0xFFFFFFFA
  sh(sp - 0x0004, s0); // 0xFFFFFFFC
  sh(sp - 0x0002, s1); // 0xFFFFFFFE
  t7 = cop2.SXY2;
  s0 = cop2.SZ3;
  INTPL();
  temp = a2 != 0;
  t0 += 4; // 0x0004
  if (temp) goto label800212E8;
  t5 = lw(v0 + 0x0000);
  goto label800212EC;
label800212E8:
  t5 = lh(at + 0x0000);
label800212EC:
  t4 = t6 & 0x1;
  fp += 4; // 0x0004
  t6 = cop2.MAC1;
  s1 = cop2.MAC3;
  cop2.VZ0 = t6;
  t6 = cop2.MAC2;
  s1 = s1 << 16;
  t6 += s1;
  cop2.VXY0 = t6;
  temp = (int32_t)s3 < 0;
  sh(sp - 0x0008, s0); // 0xFFFFFFF8
  if (temp) goto label80021430;
  temp = fp != ra;
  sw(fp - 0x0004, t7); // 0xFFFFFFFC
  if (temp) goto label80021248;
  goto label800215A8;
label80021328:
  RTPS();
  temp = t4 != 0;
  t7 = (int32_t)t5 >> 11;
  if (temp) goto label8002133C;
  t6 = lw(t0 + 0x0000);
  goto label80021340;
label8002133C:
  t6 = lh(a3 + 0x0000);
label80021340:
  t7 = t7 << s5;
  v1 += t7;
  t7 = t5 << 21;
  t7 = (int32_t)t7 >> 27;
  t7 = t7 << s5;
  a0 -= t7;
  t7 = t5 << 26;
  t7 = (int32_t)t7 >> 27;
  t7 = t7 << s5;
  a1 -= t7;
  at += 2; // 0x0002
  temp = t4 == 0;
  a2 = t5 & 0x1;
  if (temp) goto label80021284;
label80021374:
  t5 = (int32_t)t6 >> 11;
  t5 = t5 << s5;
  t1 += t5;
  t5 = t6 << 21;
  t5 = (int32_t)t5 >> 27;
  t5 = t5 << s5;
  t2 -= t5;
  t5 = t6 << 26;
  t5 = (int32_t)t5 >> 27;
  t5 = t5 << s5;
  t3 -= t5;
  cop2.IR0 = s4;
  cop2.IR1 = v1;
  cop2.IR2 = a0;
  cop2.IR3 = a1;
  cop2.RFC = t1;
  cop2.GFC = t2;
  cop2.BFC = t3;
  t7 = cop2.MAC1;
  s0 = cop2.MAC2;
  s1 = cop2.MAC3;
  sh(sp - 0x0006, t7); // 0xFFFFFFFA
  sh(sp - 0x0004, s0); // 0xFFFFFFFC
  sh(sp - 0x0002, s1); // 0xFFFFFFFE
  t7 = cop2.SXY2;
  s0 = cop2.SZ3;
  INTPL();
  temp = a2 != 0;
  a3 += 2; // 0x0002
  if (temp) goto label800213F0;
  t5 = lw(v0 + 0x0000);
  goto label800213F4;
label800213F0:
  t5 = lh(at + 0x0000);
label800213F4:
  t4 = t6 & 0x1;
  fp += 4; // 0x0004
  t6 = cop2.MAC1;
  s1 = cop2.MAC3;
  cop2.VZ0 = t6;
  t6 = cop2.MAC2;
  s1 = s1 << 16;
  t6 += s1;
  cop2.VXY0 = t6;
  temp = (int32_t)s3 < 0;
  sh(sp - 0x0008, s0); // 0xFFFFFFF8
  if (temp) goto label80021430;
  temp = fp != ra;
  sw(fp - 0x0004, t7); // 0xFFFFFFFC
  if (temp) goto label80021248;
  goto label800215A8;
label80021430:
  s0 = t7 << 5;
  t6 = t7 - s7;
  temp = (int32_t)t6 > 0;
  t6 = t7 - t8;
  if (temp) goto label80021444;
  s0 = s0 | 0x1;
label80021444:
  temp = (int32_t)t6 < 0;
  t6 = t7 << 16;
  if (temp) goto label80021450;
  s0 = s0 | 0x2;
label80021450:
  temp = (int32_t)t6 > 0;
  t6 -= t9;
  if (temp) goto label8002145C;
  s0 = s0 | 0x4;
label8002145C:
  temp = (int32_t)t6 < 0;
  if (temp) goto label80021468;
  s0 = s0 | 0x8;
label80021468:
  s6 = s6 & s0;
  temp = fp != ra;
  sw(fp - 0x0004, s0); // 0xFFFFFFFC
  if (temp) goto label80021248;
  s6 = s6 & 0x1F;
  temp = (int32_t)s6 > 0;
  if (temp) goto label80022990;
  goto label800215A8;
label80021488:
  cop2.VZ0 = v1;
  t5 = a1 << 16;
  t5 += a0;
  cop2.VXY0 = t5;
label80021498:
  temp = a2 != 0;
  sp += 8; // 0x0008
  if (temp) goto label800214D0;
  RTPS();
  t5 = lw(v0 + 0x0000);
  v0 += 4; // 0x0004
  fp += 4; // 0x0004
  v1 = (int32_t)t5 >> 21;
  t6 = t5 << 10;
  a0 = (int32_t)t6 >> 21;
  t6 = t5 << 20;
  a1 = (int32_t)t6 >> 19;
  t6 = a1 << 16;
  t6 += a0;
  goto label80021514;
label800214D0:
  t5 = lh(at + 0x0000);
  RTPS();
  at += 2; // 0x0002
  fp += 4; // 0x0004
  t6 = (int32_t)t5 >> 11;
  t6 = t6 << s5;
  v1 += t6;
  t6 = t5 << 21;
  t6 = (int32_t)t6 >> 27;
  t6 = t6 << s5;
  a0 -= t6;
  t6 = t5 << 26;
  t6 = (int32_t)t6 >> 27;
  t6 = t6 << s5;
  a1 -= t6;
  t6 = a1 << 16;
  t6 += a0;
label80021514:
  cop2.VXY0 = t6;
  cop2.VZ0 = v1;
  t6 = cop2.MAC1;
  t7 = cop2.MAC2;
  s0 = cop2.MAC3;
  sh(sp - 0x0006, t6); // 0xFFFFFFFA
  sh(sp - 0x0004, t7); // 0xFFFFFFFC
  sh(sp - 0x0002, s0); // 0xFFFFFFFE
  t6 = cop2.SZ3;
  t7 = cop2.SXY2;
  temp = (int32_t)s3 < 0;
  sh(sp - 0x0008, t6); // 0xFFFFFFF8
  if (temp) goto label80021558;
  a2 = t5 & 0x1;
  temp = fp != ra;
  sw(fp - 0x0004, t7); // 0xFFFFFFFC
  if (temp) goto label80021498;
  goto label800215A8;
label80021558:
  s0 = t7 << 5;
  t6 = t7 - s7;
  temp = (int32_t)t6 > 0;
  t6 = t7 - t8;
  if (temp) goto label8002156C;
  s0 = s0 | 0x1;
label8002156C:
  temp = (int32_t)t6 < 0;
  t6 = t7 << 16;
  if (temp) goto label80021578;
  s0 = s0 | 0x2;
label80021578:
  temp = (int32_t)t6 > 0;
  t6 -= t9;
  if (temp) goto label80021584;
  s0 = s0 | 0x4;
label80021584:
  temp = (int32_t)t6 < 0;
  a2 = t5 & 0x1;
  if (temp) goto label80021590;
  s0 = s0 | 0x8;
label80021590:
  s6 = s6 & s0;
  temp = fp != ra;
  sw(fp - 0x0004, s0); // 0xFFFFFFFC
  if (temp) goto label80021498;
  s6 = s6 & 0x1F;
  temp = (int32_t)s6 > 0;
  if (temp) goto label80022990;
label800215A8:
  v0 = cop2.ZSF4;
  ra=lo;
  temp = (int32_t)v0 <= 0;
  v0 -= 1024; // 0xFFFFFC00
  if (temp) goto label80021670;
  temp = (int32_t)v0 >= 0;
  s2 = v0 << 2;
  if (temp) goto label80021730;
  s2 = -s2;
  cop2.IR0 = s2;
  at = lw(ra + 0x0018);
  v0 = lw(ra + 0x0020);
  a0 = 0x8006FCF4;
  a0 += 4352; // 0x1100
  s2 = s2 >> 9;
  s2 = s2 << 22;
  t0 = lw(v0 + 0x0000);
  v1 = lhu(ra + 0x0002);
  a2 = t0 << 4;
  a2 = a2 & 0xFF0;
  a3 = t0 >> 4;
  a3 = a3 & 0xFF0;
  t0 = t0 >> 12;
  a1 = lw(at + 0x0000);
  t0 = t0 & 0xFF0;
  v1 = v1 << 2;
  v1 += v0;
label80021610:
  cop2.RGBC = a1;
  cop2.RFC = a2;
  cop2.GFC = a3;
  cop2.BFC = t0;
  t0 = lw(v0 + 0x0004);
  at += 4; // 0x0004
  v0 += 4; // 0x0004
  DPCS();
  a1 = lw(at + 0x0000);
  a2 = t0 << 4;
  a2 = a2 & 0xFF0;
  a3 = t0 >> 4;
  a3 = a3 & 0xFF0;
  t0 = t0 >> 12;
  t1 = cop2.RGB2;
  t0 = t0 & 0xFF0;
  sw(a0 + 0x0000, t1);
  temp = v0 != v1;
  a0 += 4; // 0x0004
  if (temp) goto label80021610;
  fp = lw(ra + 0x0014);
  t9 = 0x8006FCF4;
  t9 += 4352; // 0x1100
  goto label8002173C;
label80021670:
  at = cop2.ZSF3;
  fp = lw(ra + 0x001C);
  temp = (int32_t)at > 0;
  t9 = lw(ra + 0x0020);
  if (temp) goto label80021688;
  s2 = 0;
  goto label8002173C;
label80021688:
  v0 += 3072; // 0x0C00
  temp = (int32_t)v0 >= 0;
  s2 = 0;
  if (temp) goto label8002173C;
  at = v0 + 2048; // 0x0800
  temp = (int32_t)at > 0;
  v0 = -v0;
  if (temp) goto label800216A4;
  v0 = 2047; // 0x07FF
label800216A4:
  s2 = v0 << 1;
  cop2.IR0 = s2;
  at = lw(ra + 0x0020);
  v1 = 0x8006FCF4;
  v1 += 4352; // 0x1100
  t1 = SKYBOX_DATA;
  t1 = lw(t1 + 0x0010);
  v0 = lhu(ra + 0x0002);
  a3 = t1 << 4;
  a3 = a3 & 0xFF0;
  t0 = t1 >> 4;
  t0 = t0 & 0xFF0;
  t1 = t1 >> 12;
  t1 = t1 & 0xFF0;
  a0 = lw(at + 0x0000);
  cop2.RFC = a3;
  cop2.GFC = t0;
  cop2.BFC = t1;
  cop2.RGBC = a0;
  v0 = v0 << 2;
  v0 += at;
label80021700:
  DPCS();
  a0 = lw(at + 0x0004);
  at += 4; // 0x0004
  a1 = cop2.RGB2;
  cop2.RGBC = a0;
  sw(v1 + 0x0000, a1);
  temp = at != v0;
  v1 += 4; // 0x0004
  if (temp) goto label80021700;
  t9 = 0x8006FCF4;
  t9 += 4352; // 0x1100
  goto label8002173C;
label80021730:
  fp = lw(ra + 0x0014);
  t9 = lw(ra + 0x0018);
  s2 = 0;
label8002173C:
  lo=gp;
  s6 = cop2.BBK;
  s7 = cop2.RBK;
  t8=hi;
  at = lw(gp + 0x0030);
  s6 = s6 << 2;
  s7 = s7 & 0xFF;
  s7 += 4; // 0x0004
  s3 = s3 >> 31;
  s3 = s3 << 31;
  s7 += s3;
  hi=at;
  at = at >> 24;
  temp = at != 0;
  sp = 0x1F800000;
  if (temp) goto label800226F8;
label80021778:
  gp = 0x8006FCF4;
  gp += 2304; // 0x0900
  ra = lw(fp + 0x0000);
  temp = (int32_t)s6 > 0;
  at = 0;
  if (temp) goto label800217A0;
  at = -s6;
  at = at >> s7;
  at = at << 3;
  s6 = 1; // 0x0001
label800217A0:
  s3 = 0x8006FCF4;

  s3 += at;
  s4 = s3 + 2304; // 0x0900
  s5 = s3;
  at = lw(fp + 0x0004);
  fp += 4; // 0x0004
  ra += fp;
label800217C4:
  temp = (int32_t)at >= 0;
  s0 = lw(fp + 0x0004);
  if (temp) goto label80021AF8;
  temp = ra == fp;
  v0 = at >> 20;
  if (temp) goto label8002287C;
  v0 = v0 & 0x7FC;
  a2 = v0 + sp;
  t6 = at & 0x4;
  temp = (int32_t)t6 > 0;
  t6 = at & 0x8;
  if (temp) goto label8002256C;
  t5 = lw(fp + 0x0008);
  v1 = at >> 11;
  v1 = v1 & 0x7FC;
  a3 = v1 + sp;
  a0 = at >> 2;
  a0 = a0 & 0x7FC;
  t0 = a0 + sp;
  a2 = lw(a2 + 0x0000);
  a3 = lw(a3 + 0x0000);
  t0 = lw(t0 + 0x0000);
  a1 = t5 & 0x7FC;
  t1 = a1 + sp;
  temp = (int32_t)s7 >= 0;
  t1 = lw(t1 + 0x0000);
  if (temp) goto label80021844;
  t7 = a2 & a3;
  t7 = t7 & t0;
  t7 = t7 & t1;
  t7 = t7 & 0x1F;
  temp = (int32_t)t7 > 0;
  a2 = (int32_t)a2 >> 5;
  if (temp) goto label80021ADC;
  a3 = (int32_t)a3 >> 5;
  t0 = (int32_t)t0 >> 5;
  t1 = (int32_t)t1 >> 5;
label80021844:
  cop2.SXY0 = a2;
  cop2.SXY1 = a3;
  cop2.SXY2 = t0;
  temp = (int32_t)t6 > 0;
  t7 = at & 0x2;
  if (temp) goto label80021AE0;
  NCLIP();
  temp = t7 == 0;
  sw(t8 + 0x0008, a2);
  if (temp) goto label80021878;
  fp += 12; // 0x000C
  sw(t8 + 0x0014, a3);
  sw(t8 + 0x0020, t0);
  sw(t8 + 0x002C, t1);
  goto label80021884;
label80021878:
  sw(t8 + 0x0010, a3);
  sw(t8 + 0x0018, t0);
  sw(t8 + 0x0020, t1);
label80021884:
  t6 = at & 0x1;
  temp = (int32_t)t6 > 0;
  t2 = s0 >> 17;
  if (temp) goto label80021900;
  t6 = cop2.MAC0;
  cop2.SXY0 = t1;
  t2 = t2 & 0x7FC;
  t2 += t9;
  NCLIP();
  at = lw(fp + 0x000C);
  fp += 12; // 0x000C
  t5 = t5 >> 9;
  t5 = t5 & 0x7FC;
  temp = (int32_t)t6 < 0;
  t5 += t9;
  if (temp) goto label800218E4;
  t6 = cop2.MAC0;
  t3 = s0 >> 8;
  temp = (int32_t)t6 <= 0;
  t3 = t3 & 0x7FC;
  if (temp) goto label800217C4;
  sw(t8 + 0x0008, t1);
  v0 = a1;
  t2 = t5;
  a1 = 0;
  s1 = 16; // 0x0010
  goto label80021BD4;
label800218E4:
  t6 = cop2.MAC0;
  t3 = s0 >> 8;
  temp = (int32_t)t6 > 0;
  t3 = t3 & 0x7FC;
  if (temp) goto label80021924;
  a1 = 0;
  s1 = 0;
  goto label80021BD4;
label80021900:
  t2 = t2 & 0x7FC;
  t2 += t9;
  t5 = t5 >> 9;
  t5 = t5 & 0x7FC;
  t5 += t9;
  at = lw(fp + 0x000C);
  fp += 12; // 0x000C
  t3 = s0 >> 8;
  t3 = t3 & 0x7FC;
label80021924:
  t2 = lw(t2 + 0x0000);
  t3 += t9;
  t4 = s0 << 1;
  t4 = t4 & 0x7FC;
  t4 += t9;
  v0 = v0 << 1;
  v0 += gp;
  t3 = lw(t3 + 0x0000);
  t4 = lw(t4 + 0x0000);
  t5 = lw(t5 + 0x0000);
  v1 = v1 << 1;
  v1 += gp;
  a0 = a0 << 1;
  a0 += gp;
  a1 = a1 << 1;
  a1 += gp;
  t2 = t2 << 8;
  t2 = t2 >> 8;
  v0 = lhu(v0 + 0x0000);
  v1 = lhu(v1 + 0x0000);
  a0 = lhu(a0 + 0x0000);
  a1 = lhu(a1 + 0x0000);
  temp = (int32_t)t7 > 0;
  v0 -= s6;
  if (temp) goto label80021A18;
  t6 = 0x38000000;
  t2 += t6;
  v0 += v1;
  v0 += a0;
  sw(t8 + 0x0004, t2);
  sw(t8 + 0x000C, t3);
  sw(t8 + 0x0014, t4);
  sw(t8 + 0x001C, t5);
  t6 = (int32_t)s0 >> 28;
  t6 = t6 << 1;
  t6 = t6 << s7;
  v0 += a1;
  temp = (int32_t)v0 < 0;
  a0 = 0x08000000;
  if (temp) goto label800217C4;
  sw(t8 + 0x0000, a0);
  v0 += t6;
  temp = (int32_t)v0 < 0;
  v0 = (int32_t)v0 >> s7;
  if (temp) goto label800217C4;
  v0 = v0 << 3;
  v0 += s3;
  v1 = lw(v0 + 0x0000);
  a0 = t8 >> 16;
  temp = v1 == 0;
  sw(v0 + 0x0000, t8);
  if (temp) goto label800219F4;
  sh(v1 + 0x0000, t8);
  sb(v1 + 0x0002, a0);
  t8 += 36; // 0x0024
  goto label800217C4;
label800219F4:
  v1 = s4 - v0;
  temp = (int32_t)v1 <= 0;
  v1 = s5 - v0;
  if (temp) goto label80021A04;
  s4 = v0;
label80021A04:
  temp = (int32_t)v1 >= 0;
  sw(v0 + 0x0004, t8);
  if (temp) goto label80021A10;
  s5 = v0;
label80021A10:
  t8 += 36; // 0x0024
  goto label800217C4;
label80021A18:
  t6 = 0x3C000000;
  t2 += t6;
  v0 += v1;
  a2 = lw(fp - 0x000C); // 0xFFFFFFF4
  v0 += a0;
  t6 = (int32_t)s0 >> 28;
  t6 = t6 << 1;
  t6 = t6 << s7;
  v0 += a1;
  a3 = lw(fp - 0x0008); // 0xFFFFFFF8
  temp = (int32_t)v0 < 0;
  v0 += t6;
  if (temp) goto label800217C4;
  temp = (int32_t)v0 < 0;
  v0 = (int32_t)v0 >> s7;
  if (temp) goto label800217C4;
  v0 = v0 << 3;
  v0 += s3;
  v1 = lw(v0 + 0x0000);
  t0 = lw(fp - 0x0004); // 0xFFFFFFFC
  sw(t8 + 0x0028, t5);
  sw(t8 + 0x001C, t4);
  sw(t8 + 0x0010, t3);
  s0 = s0 & 0x1;
  s0 = s0 << 25;
  t2 += s0;
  sw(t8 + 0x0004, t2);
  a0 = 0x0C000000;
  sw(t8 + 0x0000, a0);
  a2 += s2;
  sw(t8 + 0x000C, a2);
  sw(t8 + 0x0018, a3);
  sw(t8 + 0x0024, t0);
  t1 = t0 >> 16;
  sw(t8 + 0x0030, t1);
  temp = v1 == 0;
  sw(v0 + 0x0000, t8);
  if (temp) goto label80021AB8;
  sh(v1 + 0x0000, t8);
  a0 = t8 >> 16;
  sb(v1 + 0x0002, a0);
  t8 += 52; // 0x0034
  goto label800217C4;
label80021AB8:
  v1 = s4 - v0;
  temp = (int32_t)v1 <= 0;
  v1 = s5 - v0;
  if (temp) goto label80021AC8;
  s4 = v0;
label80021AC8:
  temp = (int32_t)v1 >= 0;
  sw(v0 + 0x0004, t8);
  if (temp) goto label80021AD4;
  s5 = v0;
label80021AD4:
  t8 += 52; // 0x0034
  goto label800217C4;
label80021ADC:
  t7 = at & 0x2;
label80021AE0:
  temp = t7 == 0;
  fp += 12; // 0x000C
  if (temp) goto label80021AEC;
  fp += 12; // 0x000C
label80021AEC:
  at = lw(fp + 0x0000);
  goto label800217C4;
label80021AF8:
  temp = ra == fp;
  v0 = at >> 20;
  if (temp) goto label8002287C;
  v0 = v0 & 0x7FC;
  a2 = v0 + sp;
  v1 = at >> 11;
  v1 = v1 & 0x7FC;
  a3 = v1 + sp;
  a0 = at >> 2;
  a0 = a0 & 0x7FC;
  t0 = a0 + sp;
  a2 = lw(a2 + 0x0000);
  a3 = lw(a3 + 0x0000);
  temp = (int32_t)s7 >= 0;
  t0 = lw(t0 + 0x0000);
  if (temp) goto label80021B4C;
  t7 = a2 & a3;
  t7 = t7 & t0;
  t7 = t7 & 0x1F;
  temp = (int32_t)t7 > 0;
  a2 = (int32_t)a2 >> 5;
  if (temp) goto label80021D98;
  a3 = (int32_t)a3 >> 5;
  t0 = (int32_t)t0 >> 5;
label80021B4C:
  cop2.SXY0 = a2;
  cop2.SXY1 = a3;
  cop2.SXY2 = t0;
  t6 = at & 0x8;
  t7 = at & 0x2;
  NCLIP();
  a1 = at & 0x4;
  temp = t7 == 0;
  sw(t8 + 0x0008, a2);
  if (temp) goto label80021B80;
  fp += 12; // 0x000C
  sw(t8 + 0x0014, a3);
  sw(t8 + 0x0020, t0);
  goto label80021B88;
label80021B80:
  sw(t8 + 0x0010, a3);
  sw(t8 + 0x0018, t0);
label80021B88:
  temp = (int32_t)t6 > 0;
  t6 = at & 0x1;
  if (temp) goto label80021DA8;
  temp = (int32_t)t6 > 0;
  s1 = 0;
  if (temp) goto label80021BB0;
  t6 = cop2.MAC0;
  fp += 8; // 0x0008
  temp = (int32_t)t6 > 0;
  at = lw(fp + 0x0000);
  if (temp) goto label80021BC0;
  goto label800217C4;
label80021BB0:
  fp += 8; // 0x0008
  t6 = cop2.MAC0;
  at = lw(fp + 0x0000);
  temp = t6 == 0;
  if (temp) {
    t2 = s0 >> 17;
    goto label800217C4;
  }
label80021BC0:
  t2 = s0 >> 17;
  t2 = t2 & 0x7FC;
  t2 += t9;
  t3 = s0 >> 8;
  t3 = t3 & 0x7FC;
label80021BD4:
  t2 = lw(t2 + 0x0000);
  t3 += t9;
  t4 = s0 << 1;
  t4 = t4 & 0x7FC;
  t4 += t9;
  v0 = v0 << 1;
  v0 += gp;
  t3 = lw(t3 + 0x0000);
  t4 = lw(t4 + 0x0000);
  v1 = v1 << 1;
  v1 += gp;
  a0 = a0 << 1;
  a0 += gp;
  t2 = t2 << 8;
  temp = (int32_t)a1 > 0;
  t2 = t2 >> 8;
  if (temp) goto label80021DB4;
label80021C14:
  temp = (int32_t)t7 > 0;
  v0 = lhu(v0 + 0x0000);
  if (temp) goto label80021CC0;
  v1 = lhu(v1 + 0x0000);
  t6 = 0x30000000;
  t2 += t6;
  t6 = v0 >> 1;
  v0 += t6;
  v0 -= s6;
  a0 = lhu(a0 + 0x0000);
  v0 += v1;
  v1 = v1 >> 1;
  v0 += v1;
  t6 = (int32_t)s0 >> 28;
  t6 = t6 << 1;
  sw(t8 + 0x0004, t2);
  sw(t8 + 0x000C, t3);
  sw(t8 + 0x0014, t4);
  v0 += a0;
  temp = (int32_t)v0 < 0;
  v0 = (int32_t)v0 >> s7;
  if (temp) goto label800217C4;
  v0 += t6;
  temp = (int32_t)v0 < 0;
  a0 = 0x06000000;
  if (temp) goto label800217C4;
  sw(t8 + 0x0000, a0);
  v0 = v0 << 3;
  v0 += s3;
  v1 = lw(v0 + 0x0000);
  a0 = t8 >> 16;
  temp = v1 == 0;
  sw(v0 + 0x0000, t8);
  if (temp) goto label80021C9C;
  sh(v1 + 0x0000, t8);
  sb(v1 + 0x0002, a0);
  t8 += 28; // 0x001C
  goto label800217C4;
label80021C9C:
  v1 = s4 - v0;
  temp = (int32_t)v1 <= 0;
  v1 = s5 - v0;
  if (temp) goto label80021CAC;
  s4 = v0;
label80021CAC:
  temp = (int32_t)v1 >= 0;
  sw(v0 + 0x0004, t8);
  if (temp) goto label80021CB8;
  s5 = v0;
label80021CB8:
  t8 += 28; // 0x001C
  goto label800217C4;
label80021CC0:
  v1 = lhu(v1 + 0x0000);
  a0 = lhu(a0 + 0x0000);
  t6 = v0 >> 1;
  v0 += t6;
  v0 -= s6;
  v0 += v1;
  v1 = v1 >> 1;
  a2 = lw(fp - 0x000C); // 0xFFFFFFF4
  v0 += v1;
  t6 = (int32_t)s0 >> 28;
  t6 = t6 << 1;
  v0 += a0;
  temp = (int32_t)v0 < 0;
  v0 = (int32_t)v0 >> s7;
  if (temp) goto label800217C4;
  a3 = lw(fp - 0x0008); // 0xFFFFFFF8
  v0 += t6;
  temp = (int32_t)v0 < 0;
  v0 = v0 << 3;
  if (temp) goto label800217C4;
  v0 += s3;
  t6 = 0x34000000;
  t2 += t6;
  v1 = lw(v0 + 0x0000);
  t0 = lw(fp - 0x0004); // 0xFFFFFFFC
  sw(t8 + 0x001C, t4);
  sw(t8 + 0x0010, t3);
  s0 = s0 & 0x1;
  s0 = s0 << 25;
  t2 += s0;
  sw(t8 + 0x0004, t2);
  a2 += s2;
  temp = (int32_t)s1 <= 0;
  sw(t8 + 0x000C, a2);
  if (temp) goto label80021D48;
  a2 = t0 >> 16;
  sh(t8 + 0x000C, a2);
label80021D48:
  a0 = 0x09000000;
  sw(t8 + 0x0000, a0);
  sw(t8 + 0x0018, a3);
  sw(t8 + 0x0024, t0);
label80021D58:
  temp = v1 == 0;
  sw(v0 + 0x0000, t8);
  if (temp) goto label80021D74;
  sh(v1 + 0x0000, t8);
  a0 = t8 >> 16;
  sb(v1 + 0x0002, a0);
  t8 += 40; // 0x0028
  goto label800217C4;
label80021D74:
  v1 = s4 - v0;
  temp = (int32_t)v1 <= 0;
  v1 = s5 - v0;
  if (temp) goto label80021D84;
  s4 = v0;
label80021D84:
  temp = (int32_t)v1 >= 0;
  sw(v0 + 0x0004, t8);
  if (temp) goto label80021D90;
  s5 = v0;
label80021D90:
  t8 += 40; // 0x0028
  goto label800217C4;
label80021D98:
  t7 = at & 0x2;
  temp = t7 == 0;
  if (temp) goto label80021DA8;
  fp += 12; // 0x000C
label80021DA8:
  at = lw(fp + 0x0008);
  fp += 8; // 0x0008
  goto label800217C4;
label80021DB4:
  t6=hi;
  a1 = (int32_t)t6 >> 24;
  temp = a1 != 0;
  t5=lo;
  if (temp) goto label80021FE0;
  a1 = lh(v0 + 0x0002);
  a2 = lh(v1 + 0x0002);
  a3 = lh(a0 + 0x0002);
  t2 = lw(v0 + 0x0004);
  t3 = lw(v1 + 0x0004);
  t4 = lw(a0 + 0x0004);
  t0 = a2 - a1;
  t1 = a3 - a1;
  a1 = t2 << 16;
  t2 = (int32_t)t2 >> 16;
  a2 = t3 << 16;
  t3 = (int32_t)t3 >> 16;
  a3 = t4 << 16;
  t4 = (int32_t)t4 >> 16;
  a2 -= a1;
  a3 -= a1;
  a2 = (int32_t)a2 >> 16;
  a3 = (int32_t)a3 >> 16;
  t3 -= t2;
  t4 -= t2;
  cop2.IR1 = t0;
  cop2.IR2 = a2;
  cop2.IR3 = t3;
  cop2.RTM0 = t1;
  cop2.RTM2 = a3;
  cop2.RTM4 = t4;
  t3 = cop2.RBK;
  t4 = cop2.GBK;
  OP(SF_OFF, LM_OFF);
  t0 = t6 >> 6;
  t0 = t0 & 0xFF0;
  t1 = t6 & 0xFF0;
  t2 = t6 << 6;
  t2 = t2 & 0xFF0;
  a1 = cop2.MAC1;
  a2 = cop2.MAC3;
  a3 = cop2.MAC2;
  a1 = (int32_t)a1 >> 4;
  a2 = (int32_t)a2 >> 4;
  a3 = (int32_t)a3 >> 4;
  cop2.IR1 = a1;
  cop2.IR2 = a2;
  cop2.IR3 = a3;
  SQR(SF_OFF);
  cop2.RBK = t0;
  cop2.GBK = t1;
  cop2.BBK = t2;
  cop2.IR1 = a1;
  cop2.IR2 = a2;
  cop2.IR3 = a3;
  a1 = cop2.MAC1;
  a2 = cop2.MAC2;
  a3 = cop2.MAC3;
  a1 += a2;
  a1 += a3;
  cop2.LZCS = a1;
  temp = a1 == 0;
  if (temp) goto label80021F10;
  a2 = LZCR();
  a3 = -2; // 0xFFFFFFFE
  a3 = a2 & a3;
  a2 = 31; // 0x001F
  a2 -= a3;
  a2 = (int32_t)a2 >> 1;
  a3 -= 24; // 0xFFFFFFE8
  temp = (int32_t)a3 >= 0;
  t0 = a1 << a3;
  if (temp) goto label80021EEC;
  a3 = -a3;
  t0 = (int32_t)a1 >> a3;
label80021EEC:
  t0 -= 64; // 0xFFFFFFC0
  t0 = t0 << 1;
  a3 = 0x80074B84; // &0x101F1000
  a3 += t0;
  a3 = lh(a3 + 0x0000);
  a3 = a3 << a2;
  a1 = a3 >> 12;
label80021F10:
  a2 = t6 >> 18;
  a2 = a2 << 14;
  div_psx(a2,a1);
  a3 = 0x800770C8;
  a1 = lw(a3 + 0x000C);
  a2 = lw(a3 + 0x0010);
  a3 = lw(a3 + 0x0014);
  a1 = a1 & 0xFFFF;
  a2 = a2 & 0xFFFF;
  a3 = a3 & 0xFFFF;
  t0 = a2 << 16;
  t0 += a1;
  cop2.LCM0 = t0;
  t0 = a1 << 16;
  t0 += a3;
  cop2.LCM1 = t0;
  t0 = a3 << 16;
  t0 += a2;
  cop2.LCM2 = t0;
  t0 = a2 << 16;
  t0 += a1;
  cop2.LCM3 = t0;
  cop2.LCM4 = a3;
  a1 = 0x00FFFFFF;
  cop2.RGBC = a1;
  a1=lo;
  cop2.IR0 = a1;
  GPF(SF_OFF, LM_OFF);
  a1 = cop2.MAC1;
  a2 = cop2.MAC2;
  a3 = cop2.MAC3;
  a1 = (int32_t)a1 >> 8;
  a2 = (int32_t)a2 >> 8;
  a3 = (int32_t)a3 >> 8;
  cop2.IR1 = a1;
  cop2.IR2 = a2;
  cop2.IR3 = a3;
  CC();
  t2 = cop2.RGB2;
  cop2.RBK = t3;
  cop2.GBK = t4;
  t3 = t2;
  t4 = t2;
  lo=t5;
  hi=t6;
  goto label80021C14;
label80021FE0:
  a2 = t6 >> 16;
  a2 = a2 & 0xFF;
  temp = a2 == 0;
  a3 = t2 & 0xFF;
  if (temp) goto label800220D4;
  t0 = a3 + a2;
  t1 = t0 - 256; // 0xFFFFFF00
  temp = (int32_t)t1 < 0;
  a3 = t3 & 0xFF;
  if (temp) goto label80022004;
  t0 = 255; // 0x00FF
label80022004:
  sb(t8 + 0x0004, t0);
  t0 = a3 + a2;
  t1 = t0 - 256; // 0xFFFFFF00
  temp = (int32_t)t1 < 0;
  a3 = t4 & 0xFF;
  if (temp) goto label8002201C;
  t0 = 255; // 0x00FF
label8002201C:
  sb(t8 + 0x0010, t0);
  t0 = a3 + a2;
  t1 = t0 - 256; // 0xFFFFFF00
  temp = (int32_t)t1 < 0;
  a3 = t2 >> 8;
  if (temp) goto label80022034;
  t0 = 255; // 0x00FF
label80022034:
  sb(t8 + 0x001C, t0);
  a3 = t2 >> 8;
  a3 = a3 & 0xFF;
  t0 = a3 - a2;
  temp = (int32_t)t0 >= 0;
  sb(t8 + 0x0005, t0);
  if (temp) goto label80022050;
  sb(t8 + 0x0005, 0);
label80022050:
  a3 = t3 >> 8;
  a3 = a3 & 0xFF;
  t0 = a3 - a2;
  temp = (int32_t)t0 >= 0;
  sb(t8 + 0x0011, t0);
  if (temp) goto label80022068;
  sb(t8 + 0x0011, 0);
label80022068:
  a3 = t4 >> 8;
  a3 = a3 & 0xFF;
  t0 = a3 - a2;
  temp = (int32_t)t0 >= 0;
  sb(t8 + 0x001D, t0);
  if (temp) goto label80022080;
  sb(t8 + 0x001D, 0);
label80022080:
  a3 = t2 >> 16;
  a3 = a3 & 0xFF;
  t0 = a3 - a2;
  temp = (int32_t)t0 >= 0;
  sb(t8 + 0x0006, t0);
  if (temp) goto label80022098;
  sb(t8 + 0x0006, 0);
label80022098:
  a3 = t3 >> 16;
  a3 = a3 & 0xFF;
  t0 = a3 - a2;
  temp = (int32_t)t0 >= 0;
  sb(t8 + 0x0012, t0);
  if (temp) goto label800220B0;
  sb(t8 + 0x0012, 0);
label800220B0:
  a3 = t4 >> 16;
  a3 = a3 & 0xFF;
  t0 = a3 - a2;
  temp = (int32_t)t0 >= 0;
  sb(t8 + 0x001E, t0);
  if (temp) goto label800220C8;
  sb(t8 + 0x001E, 0);
label800220C8:
  a3 = 52; // 0x0034
  sb(t8 + 0x0007, a3);
  goto label800220E8;
label800220D4:
  sw(t8 + 0x0010, t3);
  sw(t8 + 0x001C, t4);
  t3 = 0x34000000;
  t2 += t3;
  sw(t8 + 0x0004, t2);
label800220E8:
  a1 = lh(v0 + 0x0002);
  a2 = lw(v0 + 0x0004);
  t0 = 0 | 0x4000;
  temp = a1 == t0;
  a3 = (int32_t)a2 >> 16;
  if (temp) goto label8002211C;
  a2 = a2 << 16;
  t4 = 0x80022110; // &0x34084000
  a2 = (int32_t)a2 >> 16;
  goto label800222C0;
label80022110:
  t0 = 0 | 0x4000;
  sh(v0 + 0x0002, t0);
  sw(v0 + 0x0004, a2);
label8002211C:
  cop2.RFC = a2;
  a1 = lh(v1 + 0x0002);
  a2 = lw(v1 + 0x0004);
  t0 = 0 | 0x4000;
  temp = a1 == t0;
  a3 = (int32_t)a2 >> 16;
  if (temp) goto label80022154;
  a2 = a2 << 16;
  t4 = 0x80022148; // &0x34084000
  a2 = (int32_t)a2 >> 16;
  goto label800222C0;
label80022148:
  t0 = 0 | 0x4000;
  sh(v1 + 0x0002, t0);
  sw(v1 + 0x0004, a2);
label80022154:
  cop2.GFC = a2;
  a1 = lh(a0 + 0x0002);
  a2 = lw(a0 + 0x0004);
  t0 = 0 | 0x4000;
  temp = a1 == t0;
  a3 = (int32_t)a2 >> 16;
  if (temp) goto label8002218C;
  a2 = a2 << 16;
  t4 = 0x80022180; // &0x34084000
  a2 = (int32_t)a2 >> 16;
  goto label800222C0;
label80022180:
  t0 = 0 | 0x4000;
  sh(a0 + 0x0002, t0);
  sw(a0 + 0x0004, a2);
label8002218C:
  cop2.BFC = a2;
  v0 = lhu(v0 + 0x0000);
  v1 = lhu(v1 + 0x0000);
  lo=t5;
  hi=t6;
  t4 = v0 >> 1;
  v0 += t4;
  v0 -= s6;
  a0 = lhu(a0 + 0x0000);
  v0 += v1;
  v1 = v1 >> 1;
  v0 += v1;
  t4 = (int32_t)s0 >> 28;
  t4 = t4 << 1;
  v0 += a0;
  temp = (int32_t)v0 < 0;
  v0 = (int32_t)v0 >> s7;
  if (temp) goto label800217C4;
  v0 += t4;
  temp = (int32_t)v0 < 0;
  v0 = v0 << 3;
  if (temp) goto label800217C4;
  v0 += s3;
  v1 = lw(v0 + 0x0000);
  a0 = 0x800756F0;
  a1 = cop2.RFC;
  a2 = cop2.GFC;
  a3 = cop2.BFC;
  temp = (int32_t)t6 < 0;
  t0 = a1 & 0xFF;
  if (temp) goto label80022294;
  t1 = a2 & 0xFF;
  t2 = a3 & 0xFF;
  t0 -= 64; // 0xFFFFFFC0
  t1 -= 64; // 0xFFFFFFC0
  t2 -= 64; // 0xFFFFFFC0
  t3 = t0 | t1;
  t3 = t3 | t2;
  temp = (int32_t)t3 >= 0;
  t3 = t0 & t1;
  if (temp) goto label80022294;
  t3 = t3 & t2;
  temp = (int32_t)t3 < 0;
  t3 = t0 - t1;
  if (temp) goto label80022294;
  t3 -= 64; // 0xFFFFFFC0
  temp = (int32_t)t3 >= 0;
  t3 += 128; // 0x0080
  if (temp) goto label80022270;
  temp = (int32_t)t3 <= 0;
  t3 = t0 - t2;
  if (temp) goto label80022270;
  t3 -= 64; // 0xFFFFFFC0
  temp = (int32_t)t3 >= 0;
  t3 += 128; // 0x0080
  if (temp) goto label80022270;
  temp = (int32_t)t3 <= 0;
  t3 = t1 - t2;
  if (temp) goto label80022270;
  t3 -= 64; // 0xFFFFFFC0
  temp = (int32_t)t3 >= 0;
  t3 += 128; // 0x0080
  if (temp) goto label80022270;
  temp = (int32_t)t3 <= 0;
  if (temp) goto label80022270;
  t0 = t0 >> 31;
  goto label80022294;
label80022270:
  t0 = t0 >> 31;
  t1 = t1 >> 31;
  t2 = t2 >> 31;
  t0 = t0 << 7;
  t1 = t1 << 7;
  t2 = t2 << 7;
  a1 += t0;
  a2 += t1;
  a3 += t2;
label80022294:
  t0 = lw(a0 + 0x0000);
  t1 = lw(a0 + 0x0004);
  t0 += s2;
  t2 = 0x09000000;
  sw(t8 + 0x0000, t2);
  sw(t8 + 0x000C, t0);
  sw(t8 + 0x0018, t1);
  sh(t8 + 0x000C, a1);
  sh(t8 + 0x0018, a2);
  sh(t8 + 0x0024, a3);
  goto label80021D58;
label800222C0:
  t0 = cop2.TRX;
  t1 = cop2.TRY;
  t2 = cop2.TRZ;
  a1 -= t0;
  a2 -= t1;
  a3 -= t2;
  cop2.IR1 = a1;
  cop2.IR2 = a2;
  cop2.IR3 = a3;
  MVMVA(SF_OFF, MX_LLM, V_IR, CV_NONE, LM_OFF);
  cop2.IR1 = a1;
  cop2.IR2 = a2;
  cop2.IR3 = a3;
  t1 = cop2.MAC1;
  t2 = cop2.MAC2;
  t3 = cop2.MAC3;
  SQR(SF_OFF);
  temp = (int32_t)t1 >= 0;
  a1 = t1;
  if (temp) goto label80022334;
  a1 = -a1;
label80022334:
  temp = (int32_t)t2 >= 0;
  a2 = t2;
  if (temp) goto label80022340;
  a2 = -a2;
label80022340:
  a3 = a1 - a2;
  temp = (int32_t)a3 <= 0;
  a3 = a1;
  if (temp) goto label80022354;
  a1 = a2;
  a2 = a3;
label80022354:
  temp = a2 != 0;
  a1 = a1 << 6;
  if (temp) goto label80022360;
  a2 = 1; // 0x0001
label80022360:
  div_psx(a1,a2);
  a1 = cop2.MAC1;
  a2 = cop2.MAC2;
  a3 = cop2.MAC3;
  a1 += a2;
  a1 += a3;
  cop2.LZCS = a1;
  temp = a1 == 0;
  if (temp) goto label800223D4;
  a2 = LZCR();
  a3 = -2; // 0xFFFFFFFE
  a3 = a2 & a3;
  a2 = 31; // 0x001F
  a2 -= a3;
  a2 = (int32_t)a2 >> 1;
  a3 -= 24; // 0xFFFFFFE8
  temp = (int32_t)a3 >= 0;
  t0 = a1 << a3;
  if (temp) goto label800223B0;
  a3 = -a3;
  t0 = (int32_t)a1 >> a3;
label800223B0:
  t0 -= 64; // 0xFFFFFFC0
  t0 = t0 << 1;
  a3 = 0x80074B84; // &0x101F1000
  a3 += t0;
  a3 = lh(a3 + 0x0000);
  t0=lo;
  a3 = a3 << a2;
  a1 = a3 >> 12;
label800223D4:
  div_psx(t3,a1);
  temp = (int32_t)t1 < 0;
  if (temp) goto label8002242C;
  temp = (int32_t)t2 < 0;
  if (temp) goto label80022408;
  a3 = t1 - t2;
  temp = (int32_t)a3 < 0;
  a1 = 0;
  if (temp) goto label800223FC;
  a2 = 0;
  goto label80022480;
label800223FC:
  a1 = 1; // 0x0001
  a2 = 64; // 0x0040
  goto label80022480;
label80022408:
  t2 = -t2;
  a3 = t1 - t2;
  temp = (int32_t)a3 < 0;
  a1 = 1; // 0x0001
  if (temp) goto label80022420;
  a2 = 256; // 0x0100
  goto label80022480;
label80022420:
  a1 = 0;
  a2 = 192; // 0x00C0
  goto label80022480;
label8002242C:
  temp = (int32_t)t2 < 0;
  if (temp) goto label80022458;
  t1 = -t1;
  a3 = t1 - t2;
  temp = (int32_t)a3 < 0;
  a1 = 1; // 0x0001
  if (temp) goto label8002244C;
  a2 = 128; // 0x0080
  goto label80022480;
label8002244C:
  a1 = 0;
  a2 = 64; // 0x0040
  goto label80022480;
label80022458:
  t1 = -t1;
  t2 = -t2;
  a3 = t1 - t2;
  temp = (int32_t)a3 < 0;
  a1 = 0;
  if (temp) goto label80022474;
  a2 = 128; // 0x0080
  goto label80022480;
label80022474:
  a1 = 1; // 0x0001
  a2 = 192; // 0x00C0
  goto label80022480;
label80022480:
  a3 = 0x8006D908; // &0x02010100
  a3 += t0;
  a3 = lbu(a3 + 0x0000);
  temp = a1 == 0;
  t0 = a2 + a3;
  if (temp) goto label8002249C;
  t0 = a2 - a3;
label8002249C:
  a1=lo;
  a1 = (int32_t)a1 >> 5;
  a1 += 128; // 0x0080
  temp = (int32_t)a1 >= 0;
  a2 = a1 - 256; // 0xFFFFFF00
  if (temp) goto label800224B4;
  a1 = 0;
label800224B4:
  temp = (int32_t)a2 < 0;
  if (temp) goto label800224C0;
  a1 = 255; // 0x00FF
label800224C0:
  a3 = cop2.TRZ;
  t1 = cop2.RBK;
  a2 = 0x8006D808; // &0x0A080604
  a2 += a1;
  a2 = lb(a2 + 0x0000);
  t1 = t1 >> 8;
  a3 = a3 << t1;
  a3 = a3 >> 6;
  t1 = t6 & 0x1FF;
  a3 += t1;
  a3 += t0;
  temp = (int32_t)t6 < 0;
  a3 = a3 & 0xFF;
  if (temp) goto label80022508;
  a3 = a3 >> 1;
  a2 = a2 << 8;
  temp = t4;
  a2 = a2 | a3;
  switch (temp)
  {
  case 0x80022110:
    goto label80022110;
    break;
  case 0x80022148:
    goto label80022148;
    break;
  case 0x80022180:
    goto label80022180;
    break;
  default:
    JR(temp, 0x80022500);
    return;
  }
label80022508:
  a1 = a2 - 63; // 0xFFFFFFC1
  temp = (int32_t)a1 <= 0;
  a3 = a3 << 1;
  if (temp) goto label80022518;
  a2 = 63; // 0x003F
label80022518:
  a1 = spyro_sin_lut;
  t0 = a3 + a1;
  a3 = t0 + 128; // 0x0080
  a3 = lh(a3 + 0x0000);
  t0 = lh(t0 + 0x0000);
  cop2.IR0 = a2;
  cop2.IR1 = a3;
  cop2.IR2 = t0;
  GPF(SF_OFF, LM_OFF);
  a1 = cop2.MAC1;
  a2 = cop2.MAC2;
  a1 = (int32_t)a1 >> 12;
  a2 = (int32_t)a2 >> 12;
  a1 += 64; // 0x0040
  a2 += 64; // 0x0040
  a2 = a2 << 8;
  temp = t4;
  a2 = a2 | a1;
  switch (temp)
  {
  case 0x80022110:
    goto label80022110;
    break;
  case 0x80022148:
    goto label80022148;
    break;
  case 0x80022180:
    goto label80022180;
    break;
  default:
    JR(temp, 0x80022564);
    return;
  }
label8002256C:
  a2 = lw(a2 + 0x0000);
  temp = (int32_t)s7 >= 0;
  t2 = a2 & 0x1F;
  if (temp) goto label80022580;
  temp = (int32_t)t2 > 0;
  a2 = (int32_t)a2 >> 5;
  if (temp) goto label800226EC;
label80022580:
  v0 = v0 << 1;
  v0 += gp;
  v0 = lhu(v0 + 0x0000);
  a3 = s0 >> 17;
  a3 = a3 & 0x7FC;
  a3 += t9;
  a0 = s0 >> 10;
  a0 = a0 & 0x1FF;
  v1 = 256; // 0x0100
  cop2.DQA = v1;
  cop2.DQB = 0;
  cop2.RTM3 = 0;
  cop2.RTM4 = 0;
  cop2.TRZ = v0;
  a1 = s0 >> 1;
  a1 = a1 & 0x1FF;
  RTPS();
  a3 = lw(a3 + 0x0000);
  v0 = v0 << 2;
  v1 = (int32_t)s0 >> 28;
  v1 = v1 << 1;
  v1 = v1 << s7;
  v0 += t6;
  v0 -= s6;
  at = lw(fp + 0x0014);
  fp += 20; // 0x0014
  temp = (int32_t)v0 <= 0;
  v0 = (int32_t)v0 >> s7;
  if (temp) goto label800217C4;
  v0 = v0 << 3;
  v0 += s3;
  v1 = cop2.MAC0;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  v1 = v1 >> 12;
  cop2.IR0 = v1;
  a3 = a3 << 8;
  a3 = a3 >> 8;
  GPF(SF_OFF, LM_OFF);
  v1 = 0x2C000000;
  a3 += v1;
  s0 = s0 & 0x1;
  s0 = s0 << 25;
  a3 += s0;
  v1 = cop2.MAC1;
  a0 = cop2.MAC2;
  v1 = v1 >> 11;
  a0 = a0 >> 11;
  t0 = lw(fp - 0x000C); // 0xFFFFFFF4
  t1 = lw(fp - 0x0008); // 0xFFFFFFF8
  t2 = lw(fp - 0x0004); // 0xFFFFFFFC
  a1 = a2 & 0xFFFF;
  a2 = a2 >> 16;
  t3 = v1 >> 1;
  a1 += t3;
  v1 = a1 - v1;
  sw(t8 + 0x0004, a3);
  sw(t8 + 0x000C, t0);
  sw(t8 + 0x0014, t1);
  sw(t8 + 0x001C, t2);
  t2 = t2 >> 16;
  sw(t8 + 0x0024, t2);
  t3 = a0 >> 1;
  a2 += t3;
  a0 = a2 - a0;
  sh(t8 + 0x0008, v1);
  sh(t8 + 0x000A, a0);
  sh(t8 + 0x0010, a1);
  sh(t8 + 0x0012, a0);
  sh(t8 + 0x0018, v1);
  sh(t8 + 0x001A, a2);
  sh(t8 + 0x0020, a1);
  sh(t8 + 0x0022, a2);
  v1 = 0x09000000;
  sw(t8 + 0x0000, v1);
  v1 = lw(v0 + 0x0000);
  a0 = t8 >> 16;
  temp = v1 == 0;
  sw(v0 + 0x0000, t8);
  if (temp) goto label800226C8;
  sh(v1 + 0x0000, t8);
  sb(v1 + 0x0002, a0);
  t8 += 40; // 0x0028
  goto label800217C4;
label800226C8:
  v1 = s4 - v0;
  temp = (int32_t)v1 <= 0;
  v1 = s5 - v0;
  if (temp) goto label800226D8;
  s4 = v0;
label800226D8:
  temp = (int32_t)v1 >= 0;
  sw(v0 + 0x0004, t8);
  if (temp) goto label800226E4;
  s5 = v0;
label800226E4:
  t8 += 40; // 0x0028
  goto label800217C4;
label800226EC:
  at = lw(fp + 0x0014);
  fp += 20; // 0x0014
  goto label800217C4;
label800226F8:
  a3=lo;
  t0=hi;
  at = t0 >> 24;
  at--;
  temp = at == 0;
  at = 0xFF000000;
  if (temp) goto label80022714;
  t0 = t0 | at;
label80022714:
  at = 0x800770C8;
  v0 = lw(at + 0x0018);
  v1 = lw(at + 0x001C);
  a0 = lw(at + 0x0020);
  a1 = lw(at + 0x0024);
  a2 = lw(at + 0x0028);
  cop2.LLM0 = v0;
  cop2.LLM1 = v1;
  cop2.LLM2 = a0;
  cop2.LLM3 = a1;
  cop2.LLM4 = a2;
  at = cop2.TRZ;
  v0 = cop2.TRX;
  temp = (int32_t)at >= 0;
  v1 = at;
  if (temp) goto label80022758;
  v1 = -v1;
label80022758:
  temp = (int32_t)v0 >= 0;
  a0 = v0;
  if (temp) goto label80022764;
  a0 = -a0;
label80022764:
  a1 = v1 - a0;
  temp = (int32_t)a1 <= 0;
  a1 = v1;
  if (temp) goto label80022778;
  v1 = a0;
  a0 = a1;
label80022778:
  temp = a0 != 0;
  v1 = v1 << 6;
  if (temp) goto label80022784;
  a0 = 1; // 0x0001
label80022784:
  div_psx(v1,a0);
  temp = (int32_t)at < 0;
  if (temp) goto label800227DC;
  temp = (int32_t)v0 < 0;
  if (temp) goto label800227B8;
  a1 = at - v0;
  temp = (int32_t)a1 < 0;
  v1 = 0;
  if (temp) goto label800227AC;
  a0 = 0;
  goto label80022830;
label800227AC:
  v1 = 1; // 0x0001
  a0 = 64; // 0x0040
  goto label80022830;
label800227B8:
  v0 = -v0;
  a1 = at - v0;
  temp = (int32_t)a1 < 0;
  v1 = 1; // 0x0001
  if (temp) goto label800227D0;
  a0 = 256; // 0x0100
  goto label80022830;
label800227D0:
  v1 = 0;
  a0 = 192; // 0x00C0
  goto label80022830;
label800227DC:
  temp = (int32_t)v0 < 0;
  if (temp) goto label80022808;
  at = -at;
  a1 = at - v0;
  temp = (int32_t)a1 < 0;
  v1 = 1; // 0x0001
  if (temp) goto label800227FC;
  a0 = 128; // 0x0080
  goto label80022830;
label800227FC:
  v1 = 0;
  a0 = 64; // 0x0040
  goto label80022830;
label80022808:
  at = -at;
  v0 = -v0;
  a1 = at - v0;
  temp = (int32_t)a1 < 0;
  v1 = 0;
  if (temp) goto label80022824;
  a0 = 128; // 0x0080
  goto label80022830;
label80022824:
  v1 = 1; // 0x0001
  a0 = 192; // 0x00C0
  goto label80022830;
label80022830:
  v0 = 0x80076DD0;
  v0 = lw(v0 + 0x0050);
  a1 = 0x8006D908; // &0x02010100
  a2=lo;
  a1 += a2;
  a1 = lbu(a1 + 0x0000);
  temp = v1 == 0;
  at = a0 + a1;
  if (temp) goto label8002285C;
  at = a0 - a1;
label8002285C:
  v0 = (int32_t)v0 >> 3;
  at = at << 1;
  v0 -= at;
  v0 = v0 & 0x1FF;
  t0 += v0;
  lo=a3;
  hi=t0;
  goto label80021778;
label8002287C:
  gp=lo;
  at = s5 - s4;
  temp = (int32_t)at < 0;
  hi=t8;
  if (temp) goto label80022990;
  at = cop2.GBK;
  v0 = cop2.RBK;
  at = at << 3;
  v1 = v0 & 0xFF;
  v0 = v0 >> 8;
  v0 += v1;
  v1 = 32; // 0x0020
  v1 = v1 << v0;
  v1 -= 8; // 0xFFFFFFF8
  at += v1;
  v1 = 0x8006FCF4;
  v1 += 2048; // 0x0800
  v1 -= s5;
  temp = (int32_t)v1 >= 0;
  v1 = v1 << v0;
  if (temp) goto label800228D0;
  v1 = 0;
label800228D0:
  v1 = v1 >> 8;
  v1 = v1 << 3;
  at -= v1;
  temp = (int32_t)at >= 0;
  a0 = s5;
  if (temp) goto label800228E8;
  at = 0;
label800228E8:
  v1 = 256; // 0x0100
  v1 = v1 >> v0;
  v0 = ordered_linked_list;
  v0 = lw(v0 + 0x0000);
  a2 = s4 - 8; // 0xFFFFFFF8
  at += v0;
label80022904:
  a3 = lw(at + 0x0000);
  a1 = a0 - v1;
  t0 = a2 - a1;
  temp = (int32_t)t0 <= 0;
  if (temp) goto label8002291C;
  a1 = a2;
label8002291C:
  temp = a3 != 0;
  if (temp) goto label80022948;
label80022924:
  temp = a0 == a1;
  t0 = lw(a0 + 0x0004);
  if (temp) goto label80022978;
  a0 -= 8; // 0xFFFFFFF8
  temp = t0 == 0;
  if (temp) goto label80022924;
  a3 = lw(a0 + 0x0008);
  sw(at + 0x0004, t0);
  sw(a0 + 0x000C, 0);
  sw(a0 + 0x0008, 0);
label80022948:
  temp = a0 == a1;
  t0 = lw(a0 + 0x0004);
  if (temp) goto label80022978;
  a0 -= 8; // 0xFFFFFFF8
  temp = t0 == 0;
  t1 = t0 >> 16;
  if (temp) goto label80022948;
  t2 = lw(a0 + 0x0008);
  sh(a3 + 0x0000, t0);
  sb(a3 + 0x0002, t1);
  a3 = t2;
  sw(a0 + 0x0008, 0);
  temp = a0 != a1;
  sw(a0 + 0x000C, 0);
  if (temp) goto label80022948;
label80022978:
  temp = a1 == a2;
  sw(at + 0x0000, a3);
  if (temp) goto label80022990;
  temp = at != v0;
  at -= 8; // 0xFFFFFFF8
  if (temp) goto label80022904;
  at += 8; // 0x0008
  goto label80022904;
label80022990:
  at = 0x8006FCF4;
  at += 10752; // 0x2A00
label8002299C:
  v0 = lw(at + 0x0000);
  at += 4; // 0x0004
  temp = v0 == 0;
  v1 = v0 & 0xFF;
  if (temp) goto label800229D4;
  v0 = v0 >> 8;
  a0 = 0x80000000;
  v0 = v0 | a0;
label800229B8:
  temp = v1 == 0;
  v1--;
  if (temp) goto label8002299C;
  a0 = lw(at + 0x0000);
  at += 4; // 0x0004
  sw(v0 + 0x0000, a0);
  v0 += 4; // 0x0004
  goto label800229B8;
label800229D4:
  gp += 56; // 0x0038
  goto label80020F98;
label800229DC:
  v0=hi;
  at = allocator1_ptr;
  sw(at + 0x0000, v0);
  at = 0x80077DD8;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  return;
}


// size: 0x00001098
void function_80022A2C(void)
{
  uint32_t temp;
  at = 0x80077DD8;
  sw(at + 0x0000, s0);
  sw(at + 0x0004, s1);
  sw(at + 0x0008, s2);
  sw(at + 0x000C, s3);
  sw(at + 0x0010, s4);
  sw(at + 0x0014, s5);
  sw(at + 0x0018, s6);
  sw(at + 0x001C, s7);
  sw(at + 0x0020, gp);
  sw(at + 0x0024, sp);
  sw(at + 0x0028, fp);
  sw(at + 0x002C, ra);
  ra = 0x8006FCF4;
  ra += 9216; // 0x2400
  sp = 0x80075EF8;
  sp = lw(sp + 0x0008);
  at = 0x80076DD0;
  v0 = lw(at + 0x0028);
  v1 = lw(at + 0x002C);
  a0 = lw(at + 0x0030);
  a1 = lw(at + 0x0000);
  a2 = lw(at + 0x0004);
  a3 = lw(at + 0x0008);
  t0 = lw(at + 0x000C);
  t1 = lw(at + 0x0010);
  gp = 0x1F800000;
  sw(gp + 0x03E0, v0);
  sw(gp + 0x03E4, v1);
  sw(gp + 0x03E8, a0);
  sw(gp + 0x03EC, a1);
  sw(gp + 0x03F0, a2);
  sw(gp + 0x03F4, a3);
  sw(gp + 0x03F8, t0);
  sw(gp + 0x03FC, t1);
  v1 = 0x800770C8;
  a0 = lw(v1 + 0x0000);
  a1 = lw(v1 + 0x0004);
  a2 = lw(v1 + 0x0008);
  a3 = a1 << 16;
  a3 += a0;
  cop2.LCM0 = a3;
  cop2.LCM3 = a3;
  a3 = a0 << 16;
  a3 += a2;
  cop2.LCM1 = a3;
  a3 = a2 << 16;
  a3 += a1;
  cop2.LCM2 = a3;
  cop2.LCM4 = a2;
label80022B08:
  fp = lw(ra + 0x0000);
  s7 = lw(gp + 0x03E0);
  t8 = lw(gp + 0x03E4);
  t9 = lw(gp + 0x03E8);
  ra += 4; // 0x0004
  temp = fp == 0;
  a0 = lhu(fp + 0x0050);
  if (temp) goto label80023A78;
  at = lw(fp + 0x000C);
  sb(fp + 0x0051, 0);
  a1 = a0 & 0x100;
  a1 = a1 << 1;
  a0 = a0 << 24;
  temp = (int32_t)a0 < 0;
  a0 = (int32_t)a0 >> 16;
  if (temp) goto label80022D1C;
  a0 += a1;
  v0 = lw(fp + 0x0010);
  at -= s7;
  at = (int32_t)at >> 2;
  a1 = at + a0;
  temp = (int32_t)a1 <= 0;
  a1 = at - a0;
  if (temp) goto label80022B08;
  temp = (int32_t)a1 >= 0;
  v0 = t8 - v0;
  if (temp) goto label80022B08;
  v1 = lw(fp + 0x0014);
  v0 = (int32_t)v0 >> 2;
  a1 = v0 + a0;
  temp = (int32_t)a1 <= 0;
  a1 = v0 - a0;
  if (temp) goto label80022B08;
  temp = (int32_t)a1 >= 0;
  v1 = t9 - v1;
  if (temp) goto label80022B08;
  v1 = (int32_t)v1 >> 2;
  a1 = v1 + a0;
  temp = (int32_t)a1 <= 0;
  a1 = v1 - a0;
  if (temp) goto label80022B08;
  temp = (int32_t)a1 >= 0;
  a3 = lw(gp + 0x03EC);
  if (temp) goto label80022B08;
  t0 = lw(gp + 0x03F0);
  t1 = lw(gp + 0x03F4);
  t2 = lw(gp + 0x03F8);
  t3 = lw(gp + 0x03FC);
  cop2.RTM0 = a3;
  cop2.RTM1 = t0;
  cop2.RTM2 = t1;
  cop2.RTM3 = t2;
  cop2.RTM4 = t3;
  cop2.IR3 = at;
  cop2.IR1 = v0;
  cop2.IR2 = v1;
  s6 = 0x80076378;
  MVMVA(SF_ON, MX_RT, V_IR, CV_NONE, LM_OFF);
  s5 = lhu(fp + 0x0036);
  v1 = cop2.MAC3;
  v0 = cop2.MAC2;
  at = cop2.MAC1;
  a1 = v1 - a0;
  temp = (int32_t)a1 >= 0;
  a1 = v1 + 128; // 0x0080
  if (temp) goto label80022B08;
  temp = (int32_t)a1 <= 0;
  s5 = s5 << 2;
  if (temp) goto label80022B08;
  temp = (int32_t)at >= 0;
  a1 = at;
  if (temp) goto label80022C04;
  a1 = -at;
label80022C04:
  a0 = lw(fp + 0x001C);
  a2 = v1 + 77; // 0x004D
  a1 -= 102; // 0xFFFFFF9A
  a1 = a1 << 2;
  a1 -= a2;
  a2 = a2 << 1;
  a1 -= a2;
  temp = (int32_t)a1 >= 0;
  s6 += s5;
  if (temp) goto label80022B08;
  s6 = lw(s6 + 0x0000);
  temp = (int32_t)a0 >= 0;
  a0 = v1 - 4352; // 0xFFFFEF00
  if (temp) goto label80022C48;
  temp = (int32_t)a0 >= 0;
  sw(sp + 0x0000, fp);
  if (temp) goto label80022C48;
  a0 = lb(s6 + 0x0002);
  sp += 8; // 0x0008
  sw(sp - 0x0004, a0); // 0xFFFFFFFC
label80022C48:
  temp = (int32_t)v0 >= 0;
  a1 = v0;
  if (temp) goto label80022C54;
  a1 = -v0;
label80022C54:
  a2 = v1 + 40; // 0x0028
  a1 -= 121; // 0xFFFFFF87
  a2 -= a1;
  a1 = a1 << 1;
  a2 -= a1;
  temp = (int32_t)a2 <= 0;
  s7 = 0x80000000;
  if (temp) goto label80022B08;
  s4 = lw(fp + 0x0044);
  temp = (int32_t)at >= 0;
  a1 = at;
  if (temp) goto label80022C80;
  a1 = -at;
label80022C80:
  a2 = v1 - 77; // 0xFFFFFFB3
  a1 += 102; // 0x0066
  a1 = a1 << 2;
  a1 -= a2;
  a2 = a2 << 1;
  a1 -= a2;
  temp = (int32_t)a1 >= 0;
  a0 = lbu(fp + 0x0057);
  if (temp) goto label80022CCC;
  temp = (int32_t)v0 >= 0;
  a1 = v0;
  if (temp) goto label80022CAC;
  a1 = -v0;
label80022CAC:
  a2 = v1 - 40; // 0xFFFFFFD8
  a1 += 121; // 0x0079
  a2 -= a1;
  a1 = a1 << 1;
  a2 -= a1;
  temp = (int32_t)a2 <= 0;
  if (temp) goto label80022CCC;
  s7 = 0;
label80022CCC:
  at = at << 1;
  v0 = v0 << 1;
  v1 = v1 << 1;
  t9 = (int32_t)v1 >> 6;
  temp = a0 == 0;
  a1 = 0;
  if (temp) goto label80022D90;
  cop2.IR0 = a0;
  cop2.IR1 = at;
  cop2.IR2 = v0;
  cop2.IR3 = v1;
  GPF(SF_OFF, LM_OFF);
  v1 = cop2.MAC3;
  v0 = cop2.MAC2;
  at = cop2.MAC1;
  v1 = (int32_t)v1 >> 5;
  v0 = (int32_t)v0 >> 5;
  at = (int32_t)at >> 5;
  goto label80022D90;
label80022D1C:
  v0 = lw(fp + 0x0010);
  v1 = lw(fp + 0x0014);
  at = at << 16;
  v0 = v0 << 16;
  cop2.OFX = at;
  cop2.OFY = v0;
  at = 0;
  v0 = 0;
  v1 = (int32_t)v1 >> 1;
  t9 = (int32_t)v1 >> 6;
  s7 = 0;
  a0 = lhu(fp + 0x0036);
  s4 = lw(fp + 0x0044);
  s6 = 0x80076378;
  a0 = a0 << 2;
  s6 += a0;
  s6 = lw(s6 + 0x0000);
  a3 = 4096; // 0x1000
  t0 = 0;
  t1 = 2560; // 0x0A00
  t2 = 0;
  t3 = 4096; // 0x1000
  cop2.RTM0 = a3;
  cop2.RTM1 = t0;
  cop2.RTM2 = t1;
  cop2.RTM3 = t2;
  cop2.RTM4 = t3;
  a1 = 1; // 0x0001
label80022D90:
  cop2.DQA = a1;
  a0 = 1; // 0x0001
  sb(fp + 0x0051, a0);
  a0 = (int32_t)s4 >> 24;
  t9 -= a0;
  temp = (int32_t)t9 >= 0;
  t8 = v1 - 256; // 0xFFFFFF00
  if (temp) goto label80022DB0;
  t9 = 0;
label80022DB0:
  temp = (int32_t)t8 >= 0;
  t8 = t8 << 2;
  if (temp) goto label80022DBC;
  t8 = 0;
label80022DBC:
  cop2.TRX = at;
  cop2.TRY = v0;
  cop2.TRZ = v1;
  t4 = spyro_sin_lut;
  t5 = t4 + 128; // 0x0080
  at = s4 >> 15;
  at = at & 0x1FE;
  temp = at == 0;
  v0 = at + t5;
  if (temp) goto label80022E84;
  at += t4;
  v0 = lhu(v0 + 0x0000);
  at = lhu(at + 0x0000);
  v0 = v0 & 0xFFFF;
  cop2.VXY0 = v0;
  cop2.VZ0 = at;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  at = -at;
  at = at & 0xFFFF;
  a0 = cop2.IR1;
  a1 = cop2.IR2;
  a2 = cop2.IR3;
  cop2.VXY0 = at;
  cop2.VZ0 = v0;
  v1 = 0xFFFF0000;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  a3 = a3 & v1;
  a0 = a0 & 0xFFFF;
  a3 += a0;
  t2 = t2 & v1;
  a2 = a2 & 0xFFFF;
  t2 += a2;
  a1 = a1 << 16;
  t1 = t1 & 0xFFFF;
  at = cop2.IR1;
  v0 = cop2.IR2;
  v1 = cop2.IR3;
  at = at & 0xFFFF;
  t0 = at + a1;
  v0 = v0 << 16;
  t1 += v0;
  t3 = v1 & 0xFFFF;
  cop2.RTM0 = a3;
  cop2.RTM1 = t0;
  cop2.RTM2 = t1;
  cop2.RTM3 = t2;
  cop2.RTM4 = t3;
label80022E84:
  at = s4 & 0xFF00;
  temp = at == 0;
  at = at >> 7;
  if (temp) goto label80022F34;
  v0 = at + t5;
  at += t4;
  v0 = lhu(v0 + 0x0000);
  at = lhu(at + 0x0000);
  v1 = v0 << 16;
  cop2.VXY0 = v1;
  cop2.VZ0 = at;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  v1 = at << 16;
  v1 = -v1;
  a0 = cop2.IR1;
  a1 = cop2.IR2;
  a2 = cop2.IR3;
  cop2.VXY0 = v1;
  cop2.VZ0 = v0;
  v1 = 0xFFFF0000;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  a3 = a3 & 0xFFFF;
  a0 = a0 << 16;
  a3 += a0;
  t2 = t2 & 0xFFFF;
  a2 = a2 << 16;
  t2 += a2;
  a1 = a1 & 0xFFFF;
  t0 = t0 & v1;
  at = cop2.IR1;
  v0 = cop2.IR2;
  v1 = cop2.IR3;
  at = at & 0xFFFF;
  t0 += at;
  v0 = v0 << 16;
  t1 = v0 + a1;
  t3 = v1 & 0xFFFF;
  cop2.RTM0 = a3;
  cop2.RTM1 = t0;
  cop2.RTM2 = t1;
  cop2.RTM3 = t2;
  cop2.RTM4 = t3;
label80022F34:
  at = s4 & 0xFF;
  temp = at == 0;
  at = at << 1;
  if (temp) goto label80022FEC;
  v0 = at + t5;
  at += t4;
  v0 = lhu(v0 + 0x0000);
  at = lhu(at + 0x0000);
  cop2.VZ0 = 0;
  v1 = at << 16;
  v1 += v0;
  cop2.VXY0 = v1;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  at = -at;
  at = at & 0xFFFF;
  v0 = v0 << 16;
  at += v0;
  a0 = cop2.IR1;
  a1 = cop2.IR2;
  a2 = cop2.IR3;
  cop2.VXY0 = at;
  cop2.VZ0 = 0;
  v1 = 0xFFFF0000;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  a1 = a1 << 16;
  t0 = t0 & 0xFFFF;
  t0 += a1;
  a0 = a0 & 0xFFFF;
  a2 = a2 & 0xFFFF;
  t1 = t1 & v1;
  at = cop2.IR1;
  v0 = cop2.IR2;
  v1 = cop2.IR3;
  at = at << 16;
  a3 = at + a0;
  v1 = v1 << 16;
  t2 = v1 + a2;
  v0 = v0 & 0xFFFF;
  t1 += v0;
  cop2.RTM0 = a3;
  cop2.RTM1 = t0;
  cop2.RTM2 = t1;
  cop2.RTM3 = t2;
  cop2.RTM4 = t3;
label80022FEC:
  s5 = lw(s6 + 0x0004);
  t6 = 0x00010000;
  t7 = 0x01000000;
  s0 = 0x02000000;
  s1 = -1; // 0xFFFFFFFF
  s3 = 0x1F800000;
  v1 = lw(s5 + 0x0000);
  s4 = lb(s6 + 0x0000);
  s2 = s5 + 3; // 0x0003
  at = v1 << 24;
  at = (int32_t)at >> 23;
  v0 = v1 << 16;
  v0 = (int32_t)v0 >> 23;
  v1 = v1 << 8;
  v1 = (int32_t)v1 >> 23;
  v1 = v1 << 16;
  v0 += v1;
  cop2.VXY0 = v0;
  cop2.VZ0 = at;
  s4 = s4 << 2;
  s4 += gp;
label80023040:
  RTPS();
  v1 = lw_unaligned(s2 + 0x0000);
  s2 += 3; // 0x0003
  at = v1 << 24;
  at = (int32_t)at >> 23;
  v0 = v1 << 16;
  v0 = (int32_t)v0 >> 23;
  v1 = v1 << 8;
  v1 = (int32_t)v1 >> 23;
  v1 = v1 << 16;
  v0 += v1;
  cop2.VXY0 = v0;
  cop2.VZ0 = at;
  at = cop2.SXY2;
  v0 = cop2.SZ3;
  temp = s7 == 0;
  s3 += 4; // 0x0004
  if (temp) goto label800230E0;
  v1 = at << 5;
  a0 = at - t6;
  temp = (int32_t)a0 > 0;
  a0 = at - t7;
  if (temp) goto label8002309C;
  v1 = v1 | 0x1;
label8002309C:
  temp = (int32_t)a0 < 0;
  a0 = at << 16;
  if (temp) goto label800230A8;
  v1 = v1 | 0x2;
label800230A8:
  temp = (int32_t)a0 > 0;
  a0 -= s0;
  if (temp) goto label800230B4;
  v1 = v1 | 0x4;
label800230B4:
  temp = (int32_t)a0 < 0;
  sw(s3 + 0x01FC, v0);
  if (temp) goto label800230C0;
  v1 = v1 | 0x8;
label800230C0:
  s1 = s1 & v1;
  temp = s3 != s4;
  sw(s3 - 0x0004, v1); // 0xFFFFFFFC
  if (temp) goto label80023040;
  s1 = s1 & 0xF;
  temp = (int32_t)s1 > 0;
  if (temp) goto label80022B08;
  goto label800230EC;
label800230E0:
  sw(s3 - 0x0004, at); // 0xFFFFFFFC
  temp = s3 != s4;
  sw(s3 + 0x01FC, v0);
  if (temp) goto label80023040;
label800230EC:
  s3 = s6 + 16; // 0x0010
  s5 = s5 << 1;
  s5 = s5 >> 1;
  temp = s3 == s5;
  s4 = lw(s6 + 0x0008);
  if (temp) goto label80023268;
  s5 = lb(s6 + 0x0000);
  s1 = lw(fp + 0x004C);
  s4 = 0x8006FCF4;
  s4 += 10752; // 0x2A00
  s5 = s5 << 2;
  s5 += s4;
  s2 = 0x8006E3D8; // &0x00808080
  s1 = (int32_t)s1 >> 22;
  s2 += s1;
  s2 = lw(s2 + 0x0000);
  at = lb(s3 + 0x0000);
  v0 = lb(s3 + 0x0001);
  v1 = lb(s3 + 0x0002);
  s3 += 3; // 0x0003
  s1 = s2 << 8;
  s1 = s1 >> 8;
label80023148:
  cop2.IR3 = at;
  cop2.IR1 = v0;
  cop2.IR2 = v1;
  t6 = s2 << 4;
  t7 = s2 >> 4;
  MVMVA(SF_ON, MX_RT, V_IR, CV_NONE, LM_OFF);
  at = lb(s3 + 0x0000);
  s0 = s2 >> 12;
  t5 = s2 >> 23;
  t5 = t5 & 0x1E;
  t6 = t6 & 0xFF0;
  t7 = t7 & 0xFF0;
  s0 = s0 & 0xFF0;
  cop2.IR0 = t5;
  t5 = 0x00FFFFFF;
  GPF(SF_OFF, LM_OFF);
  cop2.RGBC = t5;
  cop2.RBK = t6;
  cop2.GBK = t7;
  cop2.BBK = s0;
  CC();
  v0 = lb(s3 + 0x0001);
  v1 = lb(s3 + 0x0002);
  a0 = cop2.RGB2;
  s3 += 3; // 0x0003
  a1 = a0 - s1;
  temp = (int32_t)a1 >= 0;
  s4 += 4; // 0x0004
  if (temp) goto label800231D8;
  a0 = s1;
  goto label80023254;
label800231D8:
  a1 = s2 >> 28;
  temp = a1 == 0;
  a1 = a1 << 7;
  if (temp) goto label80023254;
  a2 = cop2.IR1;
  a3 = cop2.RBK;
  a1 = a2 - a1;
  a1 -= a3;
  temp = (int32_t)a1 <= 0;
  a1 = a1 << 1;
  if (temp) goto label80023254;
  a2 = cop2.IR1;
  a3 = cop2.IR2;
  t0 = cop2.IR3;
  a2 += a1;
  a3 += a1;
  t0 += a1;
  a1 = a2 - 4096; // 0xFFFFF000
  temp = (int32_t)a1 < 0;
  a2 = a2 >> 4;
  if (temp) goto label80023224;
  a2 = 255; // 0x00FF
label80023224:
  a1 = a3 - 4096; // 0xFFFFF000
  temp = (int32_t)a1 < 0;
  a3 = a3 >> 4;
  if (temp) goto label80023234;
  a3 = 255; // 0x00FF
label80023234:
  a1 = t0 - 4096; // 0xFFFFF000
  temp = (int32_t)a1 < 0;
  t0 = t0 >> 4;
  if (temp) goto label80023244;
  t0 = 255; // 0x00FF
label80023244:
  a3 = a3 << 8;
  t0 = t0 << 16;
  a0 = a2 + a3;
  a0 += t0;
label80023254:
  temp = s4 != s5;
  sw(s4 - 0x0004, a0); // 0xFFFFFFFC
  if (temp) goto label80023148;
  s4 = 0x8006FCF4;
  s4 += 10752; // 0x2A00
label80023268:
  at = lw(fp + 0x004C);
  v0 = lbu(s6 + 0x0001);
  s5 = lw(s6 + 0x000C);
  lo=at;
  v0 = v0 << 3;
  s6 = s5 + v0;
  at = allocator1_ptr;
  cop2.DQB = t9;
  t9 = lw(at + 0x0000);
  s1 = 0x8006FCF4;

  s2 = s1 + 2304; // 0x0900
  s3 = s1;
  at = lw(s5 + 0x0000);
label800232A8:
  temp = s5 == s6;
  s0 = lw(s5 + 0x0004);
  if (temp) goto label80023958;
  v0 = at >> 21;
  v0 = v0 & 0x1FC;
  v0 += gp;
  v1 = at >> 14;
  v1 = v1 & 0x1FC;
  v1 += gp;
  a0 = at >> 7;
  a0 = a0 & 0x1FC;
  a0 += gp;
  a1 = at & 0x1FC;
  a1 += gp;
  a2 = lw(v0 + 0x0000);
  a3 = lw(v1 + 0x0000);
  t0 = lw(a0 + 0x0000);
  temp = (int32_t)s7 >= 0;
  t1 = lw(a1 + 0x0000);
  if (temp) goto label80023314;
  t2 = a2 & a3;
  t2 = t2 & t0;
  t2 = t2 & t1;
  t2 = t2 & 0xF;
  temp = (int32_t)t2 > 0;
  a2 = (int32_t)a2 >> 5;
  if (temp) goto label8002394C;
  a3 = (int32_t)a3 >> 5;
  t0 = (int32_t)t0 >> 5;
  t1 = (int32_t)t1 >> 5;
label80023314:
  cop2.SXY0 = a2;
  cop2.SXY1 = a3;
  cop2.SXY2 = t0;
  t6 = at & 0x1;
  t7 = at & 0x2;
  NCLIP();
  at = lw(s5 + 0x0008);
  temp = (int32_t)t6 > 0;
  s5 += 8; // 0x0008
  if (temp) goto label80023534;
  t2 = s0 >> 21;
  temp = a0 == a1;
  t2 = t2 & 0x1FC;
  if (temp) goto label80023464;
  t6 = cop2.MAC0;
  cop2.SXY0 = t1;
  temp = (int32_t)t6 > 0;
  t2 += s4;
  if (temp) goto label80023388;
  NCLIP();
  t2 = lw(t2 + 0x0000);
  t3 = s0 >> 14;
  t3 = t3 & 0x1FC;
  t3 += s4;
  t4 = s0 >> 7;
  t4 = t4 & 0x1FC;
  t6 = cop2.MAC0;
  temp = (int32_t)t6 < 0;
  if (temp) goto label800233A0;
  goto label800232A8;
label80023388:
  t2 = lw(t2 + 0x0000);
  t3 = s0 >> 14;
  t3 = t3 & 0x1FC;
  t3 += s4;
  t4 = s0 >> 7;
  t4 = t4 & 0x1FC;
label800233A0:
  t3 = lw(t3 + 0x0000);
  t4 += s4;
  t5 = s0 & 0x1FC;
  t5 += s4;
  t6 = 0x38000000;
  t2 += t6;
  t4 = lw(t4 + 0x0000);
  t5 = lw(t5 + 0x0000);
  v0 = lw(v0 + 0x0200);
  v1 = lw(v1 + 0x0200);
  a0 = lw(a0 + 0x0200);
  a1 = lw(a1 + 0x0200);
  sw(t9 + 0x0008, a2);
  sw(t9 + 0x0010, a3);
  sw(t9 + 0x0018, t0);
  sw(t9 + 0x0020, t1);
  v0 += v1;
  v0 += a0;
  v0 += a1;
  v0 -= t8;
  temp = (int32_t)v0 <= 0;
  v0 = (int32_t)v0 >> 5;
  if (temp) goto label800232A8;
  s0 = s0 & 0x3;
  v0 += s0;
  sw(t9 + 0x0004, t2);
  sw(t9 + 0x000C, t3);
  sw(t9 + 0x0014, t4);
  sw(t9 + 0x001C, t5);
  a0 = 0x08000000;
  sw(t9 + 0x0000, a0);
  v0 = v0 << 3;
  v0 += s1;
  v1 = lw(v0 + 0x0000);
  a0 = t9 >> 16;
  temp = v1 == 0;
  sw(v0 + 0x0000, t9);
  if (temp) goto label80023440;
  sh(v1 + 0x0000, t9);
  sb(v1 + 0x0002, a0);
  t9 += 36; // 0x0024
  goto label800232A8;
label80023440:
  v1 = s2 - v0;
  temp = (int32_t)v1 <= 0;
  v1 = s3 - v0;
  if (temp) goto label80023450;
  s2 = v0;
label80023450:
  temp = (int32_t)v1 >= 0;
  sw(v0 + 0x0004, t9);
  if (temp) goto label8002345C;
  s3 = v0;
label8002345C:
  t9 += 36; // 0x0024
  goto label800232A8;
label80023464:
  t6 = cop2.MAC0;
  t2 += s4;
  temp = (int32_t)t6 <= 0;
  t3 = s0 >> 14;
  if (temp) goto label800232A8;
  t2 = lw(t2 + 0x0000);
  t3 = t3 & 0x1FC;
  t3 += s4;
  t4 = s0 >> 7;
  t4 = t4 & 0x1FC;
  t4 += s4;
  t3 = lw(t3 + 0x0000);
  t4 = lw(t4 + 0x0000);
  v0 = lw(v0 + 0x0200);
  v1 = lw(v1 + 0x0200);
  a0 = lw(a0 + 0x0200);
  sw(t9 + 0x0008, a2);
  sw(t9 + 0x0010, a3);
  sw(t9 + 0x0018, t0);
  t6 = 0x30000000;
  t2 += t6;
  v0 += v1;
  v0 += a0;
  v0 += a0;
  v0 -= t8;
  temp = (int32_t)v0 <= 0;
  sw(t9 + 0x0004, t2);
  if (temp) goto label800232A8;
  sw(t9 + 0x000C, t3);
  sw(t9 + 0x0014, t4);
  v0 = (int32_t)v0 >> 5;
  s0 = s0 & 0x3;
  v0 += s0;
  a0 = 0x06000000;
  sw(t9 + 0x0000, a0);
  v0 = v0 << 3;
  v0 += s1;
  v1 = lw(v0 + 0x0000);
  a0 = t9 >> 16;
  temp = v1 == 0;
  sw(v0 + 0x0000, t9);
  if (temp) goto label80023510;
  sh(v1 + 0x0000, t9);
  sb(v1 + 0x0002, a0);
  t9 += 28; // 0x001C
  goto label800232A8;
label80023510:
  v1 = s2 - v0;
  temp = (int32_t)v1 <= 0;
  v1 = s3 - v0;
  if (temp) goto label80023520;
  s2 = v0;
label80023520:
  temp = (int32_t)v1 >= 0;
  sw(v0 + 0x0004, t9);
  if (temp) goto label8002352C;
  s3 = v0;
label8002352C:
  t9 += 28; // 0x001C
  goto label800232A8;
label80023534:
  temp = (int32_t)t7 > 0;
  t7=lo;
  if (temp) goto label80023720;
  t3 = 0x8006E3D8; // &0x00808080
  t6 = cop2.MAC0;
  t7 = (int32_t)t7 >> 22;
  temp = (int32_t)t6 > 0;
  t3 += t7;
  if (temp) goto label80023574;
  cop2.SXY0 = t1;
  temp = a0 == a1;
  if (temp) goto label800232A8;
  NCLIP();
  t6 = cop2.MAC0;
  temp = (int32_t)t6 >= 0;
  if (temp) goto label800232A8;
label80023574:
  t3 = lw(t3 + 0x0000);
  t4 = (int32_t)s0 >> 24;
  t5 = s0 << 8;
  t5 = (int32_t)t5 >> 24;
  t6 = s0 << 16;
  t6 = (int32_t)t6 >> 24;
  cop2.IR3 = t4;
  cop2.IR1 = t5;
  cop2.IR2 = t6;
  sw(t9 + 0x0008, a2);
  sw(t9 + 0x000C, a3);
  MVMVA(SF_ON, MX_RT, V_IR, CV_NONE, LM_OFF);
  sw(t9 + 0x0010, t0);
  sw(t9 + 0x0014, t1);
  t0 = t3 << 4;
  t1 = t3 >> 4;
  t2 = t3 >> 12;
  t4 = t3 >> 23;
  t4 = t4 & 0x1E;
  cop2.IR0 = t4;
  t4 = 0x00FFFFFF;
  GPF(SF_OFF, LM_OFF);
  t0 = t0 & 0xFF0;
  t1 = t1 & 0xFF0;
  t2 = t2 & 0xFF0;
  cop2.RBK = t0;
  cop2.GBK = t1;
  cop2.BBK = t2;
  cop2.RGBC = t4;
  a2 = lw(v0 + 0x0200);
  a3 = lw(v1 + 0x0200);
  CC();
  t0 = lw(a0 + 0x0200);
  t1 = lw(a1 + 0x0200);
  a2 += a3;
  a2 += t0;
  a2 += t1;
  a2 -= t8;
  temp = (int32_t)a2 <= 0;
  a2 = (int32_t)a2 >> 5;
  if (temp) goto label800232A8;
  temp = a0 == a1;
  a2 = a2 << 3;
  if (temp) goto label80023630;
  v0 = 0x05000000;
  v1 = 0x28000000;
  a0 = 24; // 0x0018
  goto label8002363C;
label80023630:
  v0 = 0x04000000;
  v1 = 0x20000000;
  a0 = 20; // 0x0014
label8002363C:
  temp = t7 != 0;
  a2 += s1;
  if (temp) goto label80023650;
  a1 = 0x02000000;
  v1 += a1;
  s7 = s7 | 0x1;
label80023650:
  a1 = cop2.RGB2;
  t3 = t3 >> 28;
  temp = t3 == 0;
  sw(t9 + 0x0000, v0);
  if (temp) goto label800236D4;
  t3 = t3 << 7;
  t4 = cop2.IR1;
  t5 = cop2.RBK;
  t3 = t4 - t3;
  t3 -= t5;
  temp = (int32_t)t3 <= 0;
  t3 = t3 << 1;
  if (temp) goto label800236D4;
  t4 = cop2.IR1;
  t5 = cop2.IR2;
  t6 = cop2.IR3;
  t4 += t3;
  t5 += t3;
  t6 += t3;
  t3 = t4 - 4096; // 0xFFFFF000
  temp = (int32_t)t3 < 0;
  t4 = t4 >> 4;
  if (temp) goto label800236A4;
  t4 = 255; // 0x00FF
label800236A4:
  t3 = t5 - 4096; // 0xFFFFF000
  temp = (int32_t)t3 < 0;
  t5 = t5 >> 4;
  if (temp) goto label800236B4;
  t5 = 255; // 0x00FF
label800236B4:
  t3 = t6 - 4096; // 0xFFFFF000
  temp = (int32_t)t3 < 0;
  t6 = t6 >> 4;
  if (temp) goto label800236C4;
  t6 = 255; // 0x00FF
label800236C4:
  t5 = t5 << 8;
  t6 = t6 << 16;
  a1 = t4 + t5;
  a1 += t6;
label800236D4:
  a1 = a1 | v1;
  sw(t9 + 0x0004, a1);
  a3 = lw(a2 + 0x0000);
  t0 = t9 >> 16;
  temp = a3 == 0;
  sw(a2 + 0x0000, t9);
  if (temp) goto label800236FC;
  sh(a3 + 0x0000, t9);
  sb(a3 + 0x0002, t0);
  t9 += a0;
  goto label800232A8;
label800236FC:
  a3 = s2 - a2;
  temp = (int32_t)a3 <= 0;
  a3 = s3 - a2;
  if (temp) goto label8002370C;
  s2 = a2;
label8002370C:
  temp = (int32_t)a3 >= 0;
  sw(a2 + 0x0004, t9);
  if (temp) goto label80023718;
  s3 = a2;
label80023718:
  t9 += a0;
  goto label800232A8;
label80023720:
  t7 = (int32_t)t7 >> 21;
  t2 = 0x8006E44C;
  t3 = cop2.TRZ;
  t6 = cop2.MAC0;
  t3 -= 2048; // 0xFFFFF800
  temp = (int32_t)t3 >= 0;
  t2 += t7;
  if (temp) goto label8002374C;
  t7 = t6 >> 31;
  t7 = t7 << 25;
  goto label80023774;
label8002374C:
  temp = (int32_t)t6 > 0;
  t7 = 0x02000000;
  if (temp) goto label80023774;
  cop2.SXY0 = t1;
  temp = a0 == a1;
  if (temp) goto label800232A8;
  NCLIP();
  t6 = cop2.MAC0;
  temp = (int32_t)t6 >= 0;
  if (temp) goto label800232A8;
label80023774:
  sw(t9 + 0x0008, a2);
  sw(t9 + 0x000C, a3);
  sw(t9 + 0x0010, t0);
  sw(t9 + 0x0014, t1);
  t3 = (int32_t)s0 >> 24;
  t4 = s0 << 8;
  t4 = (int32_t)t4 >> 24;
  t5 = s0 << 16;
  t5 = (int32_t)t5 >> 24;
  cop2.IR3 = t3;
  cop2.IR1 = t4;
  cop2.IR2 = t5;
  v0 = lw(v0 + 0x0200);
  v1 = lw(v1 + 0x0200);
  MVMVA(SF_ON, MX_RT, V_IR, CV_NONE, LM_OFF);
  t1 = lw(t2 + 0x0000);
  t2 = lw(t2 + 0x0004);
  v0 += v1;
  v0 -= t8;
  MVMVA(SF_OFF, MX_RCM, V_IR, CV_NONE, LM_OFF);
  s7 = s7 | 0x2;
  t3 = t2 << 4;
  t3 = t3 & 0xFF0;
  t4 = t2 >> 4;
  t4 = t4 & 0xFF0;
  t5 = t2 >> 12;
  t5 = t5 & 0xFF0;
  t2 = cop2.MAC1;
  cop2.IR1 = t3;
  cop2.IR2 = t4;
  cop2.IR3 = t5;
  temp = (int32_t)t6 >= 0;
  t2 = (int32_t)t2 >> 8;
  if (temp) goto label80023804;
  t2 = (int32_t)t2 >> 2;
  t1 = t1 >> 1;
  v0 += 512; // 0x0200
label80023804:
  cop2.IR0 = t2;
  t3 = t1 << 4;
  t3 = t3 & 0xFF0;
  GPF(SF_OFF, LM_OFF);
  t4 = t1 >> 4;
  t4 = t4 & 0xFF0;
  t5 = t1 >> 12;
  t5 = t5 & 0xFF0;
  a3 = cop2.MAC1;
  t0 = cop2.MAC2;
  t1 = cop2.MAC3;
  a3 = (int32_t)a3 >> 12;
  t0 = (int32_t)t0 >> 12;
  t1 = (int32_t)t1 >> 12;
  t3 += a3;
  t4 += t0;
  t5 += t1;
  t2 -= 1472; // 0xFFFFFA40
  temp = (int32_t)t2 <= 0;
  t2 = t2 << 3;
  if (temp) goto label80023860;
  t3 += t2;
  t4 += t2;
  t5 += t2;
label80023860:
  temp = (int32_t)t3 >= 0;
  t2 = t3 - 4095; // 0xFFFFF001
  if (temp) goto label8002386C;
  t3 = 0;
label8002386C:
  temp = (int32_t)t2 < 0;
  t3 = t3 >> 4;
  if (temp) goto label80023878;
  t3 = 255; // 0x00FF
label80023878:
  temp = (int32_t)t4 >= 0;
  t2 = t4 - 4095; // 0xFFFFF001
  if (temp) goto label80023884;
  t4 = 0;
label80023884:
  temp = (int32_t)t2 < 0;
  t4 = t4 >> 4;
  if (temp) goto label80023890;
  t4 = 255; // 0x00FF
label80023890:
  temp = (int32_t)t5 >= 0;
  t2 = t5 - 4095; // 0xFFFFF001
  if (temp) goto label8002389C;
  t5 = 0;
label8002389C:
  temp = (int32_t)t2 < 0;
  t5 = t5 >> 4;
  if (temp) goto label800238A8;
  t5 = 255; // 0x00FF
label800238A8:
  temp = a0 == a1;
  a0 = lw(a0 + 0x0200);
  if (temp) goto label800238C0;
  a2 = 0x05000000;
  a3 = 0x2A000000;
  t0 = 24; // 0x0018
  goto label800238CC;
label800238C0:
  a2 = 0x04000000;
  a3 = 0x22000000;
  t0 = 20; // 0x0014
label800238CC:
  a1 = lw(a1 + 0x0200);
  a3 -= t7;
  t4 = t4 << 8;
  t5 = t5 << 16;
  a3 += t3;
  a3 += t4;
  a3 += t5;
  sw(t9 + 0x0000, a2);
  sw(t9 + 0x0004, a3);
  v0 += a0;
  v0 += a1;
  temp = (int32_t)v0 <= 0;
  v0 = (int32_t)v0 >> 5;
  if (temp) goto label800232A8;
  v0 = v0 << 3;
  v0 += s1;
  v1 = lw(v0 + 0x0000);
  a0 = t9 >> 16;
  temp = v1 == 0;
  sw(v0 + 0x0000, t9);
  if (temp) goto label80023928;
  sh(v1 + 0x0000, t9);
  sb(v1 + 0x0002, a0);
  t9 += t0;
  goto label800232A8;
label80023928:
  v1 = s2 - v0;
  temp = (int32_t)v1 <= 0;
  v1 = s3 - v0;
  if (temp) goto label80023938;
  s2 = v0;
label80023938:
  temp = (int32_t)v1 >= 0;
  sw(v0 + 0x0004, t9);
  if (temp) goto label80023944;
  s3 = v0;
label80023944:
  t9 += t0;
  goto label800232A8;
label8002394C:
  at = lw(s5 + 0x0008);
  s5 += 8; // 0x0008
  goto label800232A8;
label80023958:
  at = 0x01000000;
  cop2.OFX = at;
  at = 0x00780000;
  cop2.OFY = at;
  at = allocator1_ptr;
  v0 = s3 - s2;
  temp = (int32_t)v0 < 0;
  sw(at + 0x0000, t9);
  if (temp) goto label80022B08;
  at = lw(s3 + 0x0000);
  v0 = lw(s3 + 0x0004);
  sw(s3 + 0x0000, 0);
  sw(s3 + 0x0004, 0);
label8002398C:
  temp = s2 == s3;
  if (temp) {
    v1 = lw(s3 - 0x0004); // 0xFFFFFFFC
    goto label800239B8;
  }
label80023990:
  v1 = lw(s3 - 0x0004); // 0xFFFFFFFC
  s3 -= 8; // 0xFFFFFFF8
  temp = v1 == 0;
  a0 = v1 >> 16;
  if (temp) goto label8002398C;
  sh(at + 0x0000, v1);
  sb(at + 0x0002, a0);
  at = lw(s3 + 0x0000);
  sw(s3 + 0x0000, 0);
  temp = s2 != s3;
  sw(s3 + 0x0004, 0);
  if (temp) goto label80023990;
label800239B8:
  v1 = ordered_linked_list;
  v1 = lw(v1 + 0x0000);
  a0 = cop2.DQB;
  a1 = cop2.DQA;
  temp = a0 != 0;
  a0 = a0 << 3;
  if (temp) goto label800239EC;
  temp = a1 == 0;
  a0 = 0;
  if (temp) goto label800239EC;
  v1 = linked_list1;
  v1 = lw(v1 + 0x0000);
label800239EC:
  v1 += a0;
  a0 = lw(v1 + 0x0000);
  s7 = s7 & 0xF;
  temp = s7 == 0;
  s7--;
  if (temp) goto label80023A58;
  s7 = s7 << 5;
  s7 = s7 | 0x600;
  a1 = 0xE1000000;
  s7 = s7 | a1;
  a1 = 0x02000000;
  sw(t9 + 0x0000, a1);
  sw(t9 + 0x0004, s7);
  sw(t9 + 0x0008, 0);
  t9 += 12; // 0x000C
  a1 = allocator1_ptr;
  sw(a1 + 0x0000, t9);
  a1 = t9 - 12; // 0xFFFFFFF4
  sw(v1 + 0x0000, a1);
  temp = a0 == 0;
  a2 = a1 >> 16;
  if (temp) goto label80023A50;
  sh(a0 + 0x0000, a1);
  sb(a0 + 0x0002, a2);
  a0 = a1;
  goto label80023A58;
label80023A50:
  sw(v1 + 0x0004, a1);
  a0 = a1;
label80023A58:
  temp = a0 == 0;
  sw(v1 + 0x0000, at);
  if (temp) goto label80023A70;
  sh(a0 + 0x0000, v0);
  v0 = v0 >> 16;
  sb(a0 + 0x0002, v0);
  goto label80022B08;
label80023A70:
  sw(v1 + 0x0004, v0);
  goto label80022B08;
label80023A78:
  at = 0x80075EF8;
  sw(at + 0x0008, sp);
  at = 0x80077DD8;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  return;
}

// size: 0x00001014
void function_8004AE38(void)
{
  uint32_t temp;
  at = 0x80077DD8;
  sw(at + 0x0000, s0);
  sw(at + 0x0004, s1);
  sw(at + 0x0008, s2);
  sw(at + 0x000C, s3);
  sw(at + 0x0010, s4);
  sw(at + 0x0014, s5);
  sw(at + 0x0018, s6);
  sw(at + 0x001C, s7);
  sw(at + 0x0020, gp);
  sw(at + 0x0024, sp);
  sw(at + 0x0028, fp);
  sw(at + 0x002C, ra);
  s6 = lw(a0 + 0x0008);
  s5 = lw(a0 + 0x0004);
  s4 = lw(a0 + 0x0000);
  t9 = lw(a1 + 0x0008);
  t8 = lw(a1 + 0x0004);
  s7 = lw(a1 + 0x0000);
  at = s4 | s5;
  at = at | s6;
  at = at | s7;
  at = at | t8;
  at = at | t9;
  temp = (int32_t)at < 0;
  v0 = 0;
  if (temp) goto label8004B6C0;
  at = 0x1F800000;
  v0 = 0x80075778;
  v0 = lw(v0 + 0x0000);
  v1 = -1024; // 0xFFFFFC00
  v0 += 4096; // 0x1000
  a0 = s4 >> 13;
  a1 = s5 >> 13;
  a1 = a1 << 5;
  t0 = a0 + a1;
  t1 = t0 << 2;
  t1 += v0;
  t1 = lw(t1 + 0x0000);
  t2 = s4 & 0x1000;
  temp = t1 == 0;
  sw(at + 0x0000, t1);
  if (temp) goto label8004AEE8;
  at += 4; // 0x0004
label8004AEE8:
  temp = t2 == 0;
  a0 = -1; // 0xFFFFFFFF
  if (temp) goto label8004AEF4;
  a0 = 1; // 0x0001
label8004AEF4:
  t1 = t0 + a0;
  t2 = t1 & v1;
  temp = (int32_t)t2 > 0;
  t1 = t1 << 2;
  if (temp) goto label8004AF1C;
  t1 += v0;
  t1 = lw(t1 + 0x0000);
  t2 = s5 & 0x1000;
  temp = t1 == 0;
  sw(at + 0x0000, t1);
  if (temp) goto label8004AF1C;
  at += 4; // 0x0004
label8004AF1C:
  temp = t2 == 0;
  a1 = -32; // 0xFFFFFFE0
  if (temp) goto label8004AF28;
  a1 = 32; // 0x0020
label8004AF28:
  t1 = t0 + a1;
  t2 = t1 & v1;
  temp = (int32_t)t2 > 0;
  t1 = t1 << 2;
  if (temp) goto label8004AF50;
  t1 += v0;
  t1 = lw(t1 + 0x0000);
  temp = t1 == 0;
  sw(at + 0x0000, t1);
  if (temp) goto label8004AF50;
  at += 4; // 0x0004
label8004AF50:
  t1 = t0 + a0;
  t1 += a1;
  t2 = t1 & v1;
  temp = (int32_t)t2 > 0;
  t1 = t1 << 2;
  if (temp) goto label8004AF7C;
  t1 += v0;
  t1 = lw(t1 + 0x0000);
  temp = t1 == 0;
  sw(at + 0x0000, t1);
  if (temp) goto label8004AF7C;
  at += 4; // 0x0004
label8004AF7C:
  s3 = 0x1F800000;
  temp = at != s3;
  sw(at + 0x0000, 0);
  if (temp) goto label8004B700;
label8004AF88:
  at = s4 >> 4;
  v1 = s4 - s7;
  temp = (int32_t)v1 <= 0;
  v0 = s7 >> 4;
  if (temp) goto label8004AFA0;
  at = s7 >> 4;
  v0 = s4 >> 4;
label8004AFA0:
  v0++;
  at = v0 - at;
  sw(s3 + 0x0030, at);
  sw(s3 + 0x0034, v0);
  at = s5 >> 4;
  v1 = s5 - t8;
  temp = (int32_t)v1 <= 0;
  v0 = t8 >> 4;
  if (temp) goto label8004AFC8;
  at = t8 >> 4;
  v0 = s5 >> 4;
label8004AFC8:
  v0++;
  at = v0 - at;
  sw(s3 + 0x0038, at);
  sw(s3 + 0x003C, v0);
  at = s6 >> 4;
  v1 = s6 - t9;
  temp = (int32_t)v1 <= 0;
  v0 = t9 >> 4;
  if (temp) goto label8004AFF0;
  at = t9 >> 4;
  v0 = s6 >> 4;
label8004AFF0:
  v0++;
  at = v0 - at;
  sw(s3 + 0x0040, at);
  sw(s3 + 0x0044, v0);
  at = 0x40000000;
  sw(s3 + 0x0054, at);
  a3 = 0x800785A8;
  a3 = lw(a3 + 0x002C);
  at = s4 >> 12;
  v0 = s5 >> 12;
  v1 = s6 >> 12;
  a0 = s7 >> 12;
  a1 = t8 >> 12;
  a2 = t9 >> 12;
  sp = lw(a3 + 0x0010);
  s7 -= s4;
  t8 -= s5;
  t9 -= s6;
  ra = 0x1F800000;
  sh(ra + 0x0000, at);
  sh(ra + 0x0002, v0);
  temp = at == a0;
  sh(ra + 0x0004, v1);
  if (temp) goto label8004B0EC;
  sh(ra + 0x0006, a0);
  sh(ra + 0x0008, v0);
  temp = v0 == a1;
  sh(ra + 0x000A, v1);
  if (temp) goto label8004B0BC;
  sh(ra + 0x000C, at);
  sh(ra + 0x000E, a1);
  sh(ra + 0x0010, v1);
  sh(ra + 0x0012, a0);
  sh(ra + 0x0014, a1);
  temp = v1 == a2;
  sh(ra + 0x0016, v1);
  if (temp) goto label8004B0B4;
  sh(ra + 0x0018, at);
  sh(ra + 0x001A, v0);
  sh(ra + 0x001C, a2);
  sh(ra + 0x001E, at);
  sh(ra + 0x0020, a1);
  sh(ra + 0x0022, a2);
  sh(ra + 0x0024, a0);
  sh(ra + 0x0026, v0);
  sh(ra + 0x0028, a2);
  sh(ra + 0x002A, a0);
  sh(ra + 0x002C, a1);
  sh(ra + 0x002E, a2);
  ra += 48; // 0x0030
  goto label8004B14C;
label8004B0B4:
  ra += 24; // 0x0018
  goto label8004B14C;
label8004B0BC:
  temp = v1 == a2;
  if (temp) goto label8004B0E4;
  sh(ra + 0x000C, at);
  sh(ra + 0x000E, v0);
  sh(ra + 0x0010, a2);
  sh(ra + 0x0012, a0);
  sh(ra + 0x0014, v0);
  sh(ra + 0x0016, a2);
  ra += 24; // 0x0018
  goto label8004B14C;
label8004B0E4:
  ra += 12; // 0x000C
  goto label8004B14C;
label8004B0EC:
  temp = v0 == a1;
  if (temp) goto label8004B12C;
  sh(ra + 0x0006, at);
  sh(ra + 0x0008, a1);
  temp = v1 == a2;
  sh(ra + 0x000A, v1);
  if (temp) goto label8004B124;
  sh(ra + 0x000C, at);
  sh(ra + 0x000E, v0);
  sh(ra + 0x0010, a2);
  sh(ra + 0x0012, at);
  sh(ra + 0x0014, a1);
  sh(ra + 0x0016, a2);
  ra += 24; // 0x0018
  goto label8004B14C;
label8004B124:
  ra += 12; // 0x000C
  goto label8004B14C;
label8004B12C:
  temp = v1 == a2;
  if (temp) goto label8004B148;
  sh(ra + 0x0006, at);
  sh(ra + 0x0008, v0);
  sh(ra + 0x000A, a2);
  ra += 12; // 0x000C
  goto label8004B14C;
label8004B148:
  ra += 6; // 0x0006
label8004B14C:
  at = ra << 16;
  temp = (int32_t)at <= 0;
  ra -= 6; // 0xFFFFFFFA
  if (temp) goto label8004B6AC;
  at = lh(ra + 0x0004);
  a0 = 0x800785A8;
  a0 = lw(a0 + 0x002C);
  temp = (int32_t)at < 0;
  a3 = 0 | 0xFFFF;
  if (temp) goto label8004B14C;
  a1 = lw(a0 + 0x0008);
  v0 = at << 1;
  v1 = lhu(a1 + 0x0000);
  a2 = a1 + v0;
  a2 = lhu(a2 + 0x0002);
  v1 -= at;
  temp = a2 == a3;
  a2 += a1;
  if (temp) goto label8004B14C;
  temp = (int32_t)v1 <= 0;
  at = lh(ra + 0x0002);
  if (temp) goto label8004B14C;
  v1 = lhu(a2 + 0x0000);
  temp = (int32_t)at < 0;
  v0 = at << 1;
  if (temp) goto label8004B14C;
  a2 += v0;
  a2 = lhu(a2 + 0x0002);
  v1 -= at;
  temp = a2 == a3;
  a2 += a1;
  if (temp) goto label8004B14C;
  temp = (int32_t)v1 <= 0;
  at = lh(ra + 0x0000);
  if (temp) goto label8004B14C;
  v1 = lhu(a2 + 0x0000);
  temp = (int32_t)at < 0;
  v0 = at << 1;
  if (temp) goto label8004B14C;
  a2 += v0;
  a2 = lhu(a2 + 0x0002);
  v1 -= at;
  temp = (int32_t)v1 <= 0;
  fp = lw(a0 + 0x000C);
  if (temp) goto label8004B14C;
  temp = a2 == a3;
  a2 = a2 << 1;
  if (temp) goto label8004B14C;
  fp += a2;
  gp = lh(fp + 0x0000);
  gp = gp & 0x7FFF;
  goto label8004B1FC;
label8004B1F8:
  gp = lh(fp + 0x0000);
label8004B1FC:
  fp += 2; // 0x0002
  temp = (int32_t)gp < 0;
  at = gp << 2;
  if (temp) goto label8004B14C;
  v0 = gp << 3;
  at += v0;
  t2 = at + sp;
  s2 = lw(t2 + 0x0008);
  t3 = lw(s3 + 0x0044);
  t1 = s2 & 0x3FFF;
  t4 = t1 - t3;
  temp = (int32_t)t4 > 0;
  v1 = s2 >> 24;
  if (temp) goto label8004B1F8;
  a2 = s2 >> 16;
  t3 = lw(s3 + 0x0040);
  a2 = a2 & 0xFF;
  t4 += t3;
  t5 = t4 + v1;
  t4 += a2;
  t4 = t4 & t5;
  temp = (int32_t)t4 < 0;
  t3 = lw(s3 + 0x003C);
  if (temp) goto label8004B1F8;
  t0 = lw(t2 + 0x0004);
  t4 = lw(s3 + 0x0038);
  v0 = (int32_t)t0 >> 23;
  a1 = t0 << 9;
  a1 = (int32_t)a1 >> 23;
  t0 = t0 & 0x3FFF;
  t5 = t0 - t3;
  t6 = t5 + a1;
  t7 = t5 + v0;
  t3 = t5 | t6;
  t3 = t3 | t7;
  temp = (int32_t)t3 > 0;
  t5 += t4;
  if (temp) goto label8004B1F8;
  t6 = t5 + a1;
  t7 = t5 + v0;
  t4 = t5 & t6;
  t4 = t4 & t7;
  temp = (int32_t)t4 < 0;
  t3 = lw(s3 + 0x0034);
  if (temp) goto label8004B1F8;
  a3 = lw(t2 + 0x0000);
  t4 = lw(s3 + 0x0030);
  at = (int32_t)a3 >> 23;
  a0 = a3 << 9;
  a0 = (int32_t)a0 >> 23;
  a3 = a3 & 0x3FFF;
  t5 = a3 - t3;
  t6 = t5 + a0;
  t7 = t5 + at;
  t3 = t5 | t6;
  t3 = t3 | t7;
  temp = (int32_t)t3 > 0;
  t5 += t4;
  if (temp) goto label8004B1F8;
  t6 = t5 + a0;
  t7 = t5 + at;
  t4 = t5 & t6;
  t4 = t4 & t7;
  temp = (int32_t)t4 < 0;
  s0 = a3 << 4;
  if (temp) goto label8004B1F8;
  s2 = s2 & 0x4000;
  temp = (int32_t)s2 > 0;
  s1 = t0 << 4;
  if (temp) goto label8004B1F8;
  s2 = t1 << 4;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = a2;
  cop2.RTM0 = at;
  cop2.RTM2 = v0;
  cop2.RTM4 = v1;
  a3 = s4 - s0;
  t0 = s5 - s1;
  OP(SF_OFF, LM_OFF);
  t1 = s6 - s2;
  t2 = a3 + s7;
  t3 = t0 + t8;
  t4 = t1 + t9;
  t5 = t0 << 16;
  t6 = a3 & 0xFFFF;
  t5 += t6;
  cop2.RTM0 = t5;
  cop2.RTM1 = t1;
  t5 = t3 << 16;
  t6 = t2 & 0xFFFF;
  t5 += t6;
  cop2.RTM3 = t5;
  cop2.RTM4 = t4;
  t5 = cop2.MAC1;
  t6 = cop2.MAC2;
  t7 = cop2.MAC3;
  t5 = (int32_t)t5 >> 2;
  t6 = (int32_t)t6 >> 2;
  t7 = (int32_t)t7 >> 2;
  cop2.IR1 = t5;
  cop2.IR2 = t6;
  cop2.IR3 = t7;
  sw(s3 + 0x0048, s0);
  sw(s3 + 0x004C, s1);
  MVMVA(SF_OFF, MX_RT, V_IR, CV_NONE, LM_OFF);
  sw(s3 + 0x0050, s2);
  t2 = t0 << 16;
  t3 = a3 & 0xFFFF;
  t2 += t3;
  t3 = t8 << 16;
  s0 = cop2.MAC3;
  s1 = cop2.MAC1;
  temp = (int32_t)s0 >= 0;
  t4 = s7 & 0xFFFF;
  if (temp) goto label8004B1F8;
  temp = (int32_t)s1 < 0;
  t3 += t4;
  if (temp) goto label8004B1F8;
  cop2.IR1 = t5;
  cop2.IR2 = t6;
  cop2.IR3 = t7;
  cop2.RTM0 = t2;
  cop2.RTM1 = t1;
  cop2.RTM3 = t3;
  cop2.RTM4 = t9;
  MVMVA(SF_OFF, MX_RT, V_IR, CV_NONE, LM_OFF);
  s0 = cop2.MAC1;
  s1 = cop2.MAC3;
  cop2.LZCS = s0;
  t2 = LZCR();
  t2 -= 2; // 0xFFFFFFFE
  s0 = s0 << t2;
  t2 -= 12; // 0xFFFFFFF4
  temp = (int32_t)t2 >= 0;
  t3 = -t2;
  if (temp) goto label8004B410;
  s1 = (int32_t)s1 >> t3;
  goto label8004B414;
label8004B410:
  s1 = s1 << t2;
label8004B414:
  div_psx(s0,s1);
  at = at << 4;
  v0 = v0 << 4;
  v1 = v1 << 4;
  a0 = a0 << 4;
  a1 = a1 << 4;
  a2 = a2 << 4;
  cop2.MAC1 = 0;
  cop2.MAC2 = 0;
  cop2.MAC3 = 0;
  cop2.IR1 = s7;
  cop2.IR2 = t8;
  cop2.IR3 = t9;
  t2=lo;
  cop2.IR0 = t2;
  GPL(SF_OFF, LM_OFF);
  t2 = cop2.MAC1;
  t3 = cop2.MAC2;
  t4 = cop2.MAC3;
  t2 = (int32_t)t2 >> 12;
  t3 = (int32_t)t3 >> 12;
  t4 = (int32_t)t4 >> 12;
  cop2.IR1 = t2;
  cop2.IR2 = t3;
  cop2.IR3 = t4;
  a3 -= t2;
  t0 -= t3;
  SQR(SF_OFF);
  t1 -= t4;
  s0 = lw(s3 + 0x0054);
  t2 = cop2.MAC1;
  t3 = cop2.MAC2;
  t4 = cop2.MAC3;
  t2 += t3;
  t2 += t4;
  t3 = t2 - s0;
  temp = (int32_t)t3 >= 0;
  sw(s3 + 0x0058, t2);
  if (temp) goto label8004B1F8;
  t2 = t5;
  t3 = 0;
  s0 = t2 + t6;
  s1 = t2 - t6;
  s0 = s0 ^ s1;
  temp = (int32_t)s0 >= 0;
  if (temp) goto label8004B4D8;
  t2 = t6;
  t3 = 1; // 0x0001
label8004B4D8:
  s0 = t2 + t7;
  s1 = t2 - t7;
  s0 = s0 ^ s1;
  temp = (int32_t)s0 >= 0;
  if (temp) goto label8004B4F4;
  t2 = t7;
  t3 = 2; // 0x0002
label8004B4F4:
  t3 = t3 << 4;
  cop2.IR1 = at;
  cop2.IR2 = v0;
  cop2.IR3 = v1;
  cop2.RTM0 = a3;
  cop2.RTM2 = t0;
  cop2.RTM4 = t1;
  s0 = 0x8004B528; // &0x4810C800
  OP(SF_OFF, LM_OFF);
  s0 += t3;
  temp = s0;
  switch (temp)
  {
  case 0x8004B528:
    goto label8004B528;
    break;
  case 0x8004B538:
    goto label8004B538;
    break;
  case 0x8004B548:
    goto label8004B548;
    break;
  default:
    JR(temp, 0x8004B520);
    return;
  }
label8004B528:
  s0 = cop2.MAC1;
  goto label8004B550;
label8004B538:
  s0 = cop2.MAC2;
  goto label8004B550;
label8004B548:
  s0 = cop2.MAC3;
label8004B550:
  temp = s0 == 0;
  s0 = s0 ^ t2;
  if (temp) goto label8004B560;
  temp = (int32_t)s0 >= 0;
  if (temp) goto label8004B1F8;
label8004B560:
  cop2.IR1 = a3;
  cop2.IR2 = t0;
  cop2.IR3 = t1;
  cop2.RTM0 = a0;
  cop2.RTM2 = a1;
  cop2.RTM4 = a2;
  s0 = 0x8004B590; // &0x4810C800
  OP(SF_OFF, LM_OFF);
  s0 += t3;
  temp = s0;
  switch (temp)
  {
  case 0x8004B590:
    goto label8004B590;
    break;
  case 0x8004B5A0:
    goto label8004B5A0;
    break;
  case 0x8004B5B0:
    goto label8004B5B0;
    break;
  default:
    JR(temp, 0x8004B588);
    return;
  }
label8004B590:
  s0 = cop2.MAC1;
  goto label8004B5B8;
label8004B5A0:
  s0 = cop2.MAC2;
  goto label8004B5B8;
label8004B5B0:
  s0 = cop2.MAC3;
label8004B5B8:
  temp = s0 == 0;
  s0 = s0 ^ t2;
  if (temp) goto label8004B5C8;
  temp = (int32_t)s0 >= 0;
  if (temp) goto label8004B1F8;
label8004B5C8:
  a0 -= at;
  a1 -= v0;
  a2 -= v1;
  at = a3 - at;
  v0 = t0 - v0;
  v1 = t1 - v1;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = a2;
  cop2.RTM0 = at;
  cop2.RTM2 = v0;
  cop2.RTM4 = v1;
  s0 = 0x8004B610; // &0x4810C800
  OP(SF_OFF, LM_OFF);
  s0 += t3;
  temp = s0;
  switch (temp)
  {
  case 0x8004B610:
    goto label8004B610;
    break;
  case 0x8004B620:
    goto label8004B620;
    break;
  case 0x8004B630:
    goto label8004B630;
    break;
  default:
    JR(temp, 0x8004B608);
    return;
  }
label8004B610:
  s0 = cop2.MAC1;
  goto label8004B638;
label8004B620:
  s0 = cop2.MAC2;
  goto label8004B638;
label8004B630:
  s0 = cop2.MAC3;
label8004B638:
  temp = s0 == 0;
  s0 = s0 ^ t2;
  if (temp) goto label8004B648;
  temp = (int32_t)s0 >= 0;
  if (temp) goto label8004B1F8;
label8004B648:
  at = lw(s3 + 0x0048);
  v0 = lw(s3 + 0x004C);
  v1 = lw(s3 + 0x0050);
  at += a3;
  v0 += t0;
  v1 += t1;
  a0 = 0x80076B80;
  sw(a0 + 0x0000, at);
  sw(a0 + 0x0004, v0);
  sw(a0 + 0x0008, v1);
  a0 = 0x80077368;
  sw(a0 + 0x0000, t5);
  sw(a0 + 0x0004, t6);
  sw(a0 + 0x0008, t7);
  a0 = 0x800757E4;
  sw(a0 + 0x0000, 0);
  a0 = 0x80075808;
  sw(a0 + 0x0000, gp);
  at = lw(s3 + 0x0058);
  sw(s3 + 0x0054, at);
  goto label8004B1F8;
label8004B6AC:
  at = lw(s3 + 0x0054);
  v0 = 0x40000000;
  temp = at == v0;
  v0 = 0;
  if (temp) goto label8004B6C0;
  v0 = 1; // 0x0001
label8004B6C0:
  at = 0x80077DD8;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  return;
label8004B700:
  at = s4;
  v1 = s4 - s7;
  temp = (int32_t)v1 <= 0;
  v0 = s7;
  if (temp) goto label8004B718;
  at = s7;
  v0 = s4;
label8004B718:
  sw(s3 + 0x0030, at);
  sw(s3 + 0x0034, v0);
  at = s5;
  v1 = s5 - t8;
  temp = (int32_t)v1 <= 0;
  v0 = t8;
  if (temp) goto label8004B738;
  at = t8;
  v0 = s5;
label8004B738:
  sw(s3 + 0x0038, at);
  sw(s3 + 0x003C, v0);
  at = s6;
  v1 = s6 - t9;
  temp = (int32_t)v1 <= 0;
  v0 = t9;
  if (temp) goto label8004B758;
  at = t9;
  v0 = s6;
label8004B758:
  sw(s3 + 0x0040, at);
  sw(s3 + 0x0044, v0);
  s7 -= s4;
  t8 -= s5;
  t9 -= s6;
  at = 0x40000000;
  sw(s3 + 0x0054, at);
  ra = 0x1F800000;
  s3 += 128; // 0x0080
  temp = (int32_t)s7 >= 0;
  at = s7;
  if (temp) goto label8004B788;
  at = -s7;
label8004B788:
  temp = (int32_t)t8 >= 0;
  v0 = t8;
  if (temp) goto label8004B794;
  v0 = -t8;
label8004B794:
  temp = (int32_t)t9 >= 0;
  v1 = t9;
  if (temp) goto label8004B7A0;
  v1 = -t9;
label8004B7A0:
  at += v0;
  at += v1;
  sw(s3 - 0x0024, at); // 0xFFFFFFDC
label8004B7AC:
  at = lw(ra + 0x0000);
  fp = ra - 4; // 0xFFFFFFFC
  temp = at == 0;
  ra += 4; // 0x0004
  if (temp) goto label8004BDE8;
label8004B7BC:
  fp = lw(fp + 0x0004);
  temp = fp == 0;
  at = lw(fp + 0x000C);
  if (temp) goto label8004B7AC;
  v0 = lw(fp + 0x0010);
  v1 = lw(fp + 0x0014);
  a0 = at - s4;
  a1 = v0 - s5;
  a2 = v1 - s6;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = a2;
  sp = lw(fp + 0x0008);
  a0 = 0x80000000;
  SQR(SF_OFF);
  temp = sp == 0;
  sp = sp | a0;
  if (temp) goto label8004B7BC;
  a0 = lhu(sp + 0x0000);
  a1 = lw(s3 - 0x0024); // 0xFFFFFFDC
  sp += 4; // 0x0004
  a0 += a1;
  mult(a0, a0);
  a2 = cop2.MAC1;
  a3 = cop2.MAC2;
  t0 = cop2.MAC3;
  t1 = a2 + a3;
  t1 += t0;
  a0=lo;
  a0 = t1 - a0;
  temp = (int32_t)a0 >= 0;
  if (temp) goto label8004B7BC;
  t2 = lw(fp + 0x0020);
  t3 = lw(fp + 0x0024);
  t4 = lw(fp + 0x0028);
  t5 = lw(fp + 0x002C);
  t6 = lw(fp + 0x0030);
  cop2.RTM0 = t2;
  cop2.RTM1 = t3;
  cop2.RTM2 = t4;
  cop2.RTM3 = t5;
  cop2.RTM4 = t6;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  a1 = lhu(fp + 0x0036);
  a2 = 0x80076378;
  a1 = a1 << 2;
  a1 += a2;
  a2 = lbu(fp + 0x003C);
  a1 = lw(a1 + 0x0000);
  a2 = a2 << 2;
  a1 += a2;
  a2 = lbu(fp + 0x003E);
  t0 = lw(a1 + 0x0038);
  a2 = a2 << 3;
  a3 = t0 + a2;
  a1 = lw(a3 + 0x0024);
  a2 = lbu(t0 + 0x0005);
  a0 = lhu(sp - 0x0002); // 0xFFFFFFFE
  a1 = a1 << 11;
  a1 = a1 >> 11;
  a3 = 0x80000000;
  a1 = a1 | a3;
  a0 = a0 << 2;
  a0 += a1;
  a0 += 4; // 0x0004
  a3 = s3 - 16; // 0xFFFFFFF0
label8004B8CC:
  RTPS();
  t0 = lw(a1 + 0x0000);
  a1 += 4; // 0x0004
  t2 = t0 << 20;
  t2 = (int32_t)t2 >> 19;
  t1 = t0 << 10;
  t1 = (int32_t)t1 >> 21;
  t0 = (int32_t)t0 >> 21;
  t0 = t0 << a2;
  t1 = t1 << a2;
  t2 = t2 << a2;
  t2 = t2 << 16;
  t1 += t2;
  cop2.VZ0 = t0;
  cop2.VXY0 = t1;
  t0 = cop2.IR3;
  t1 = cop2.IR1;
  t2 = cop2.IR2;
  t0 += at;
  t1 = v0 - t1;
  t2 = v1 - t2;
  sw(a3 + 0x0000, t0);
  sw(a3 + 0x0004, t1);
  sw(a3 + 0x0008, t2);
  temp = a1 != a0;
  a3 += 16; // 0x0010
  if (temp) goto label8004B8CC;
label8004B934:
  gp = lw(sp + 0x0000);
  sp += 4; // 0x0004
  temp = (int32_t)gp < 0;
  s2 = gp >> 20;
  if (temp) goto label8004B7BC;
  s2 = s2 & 0x3F0;
  s2 += s3;
  s0 = lw(s2 + 0x0000);
  s1 = lw(s2 + 0x0004);
  s2 = lw(s2 + 0x0008);
  a2 = gp >> 12;
  a2 = a2 & 0x3F0;
  a2 += s3;
  a0 = lw(a2 + 0x0000);
  a1 = lw(a2 + 0x0004);
  a2 = lw(a2 + 0x0008);
  v1 = gp >> 4;
  v1 = v1 & 0x3F0;
  v1 += s3;
  at = lw(v1 + 0x0000);
  v0 = lw(v1 + 0x0004);
  v1 = lw(v1 + 0x0008);
  t3 = lw(s3 - 0x0050); // 0xFFFFFFB0
  t4 = lw(s3 - 0x004C); // 0xFFFFFFB4
  t5 = at - t3;
  t6 = a0 - t3;
  t7 = s0 - t3;
  t5 = t5 & t6;
  t5 = t5 & t7;
  temp = (int32_t)t5 < 0;
  t5 = t4 - at;
  if (temp) goto label8004B934;
  t6 = t4 - a0;
  t7 = t4 - s0;
  t5 = t5 & t6;
  t5 = t5 & t7;
  temp = (int32_t)t5 < 0;
  t3 = lw(s3 - 0x0048); // 0xFFFFFFB8
  if (temp) goto label8004B934;
  t4 = lw(s3 - 0x0044); // 0xFFFFFFBC
  t5 = v0 - t3;
  t6 = a1 - t3;
  t7 = s1 - t3;
  t5 = t5 & t6;
  t5 = t5 & t7;
  temp = (int32_t)t5 < 0;
  t5 = t4 - v0;
  if (temp) goto label8004B934;
  t6 = t4 - a1;
  t7 = t4 - s1;
  t5 = t5 & t6;
  t5 = t5 & t7;
  temp = (int32_t)t5 < 0;
  t3 = lw(s3 - 0x0040); // 0xFFFFFFC0
  if (temp) goto label8004B934;
  t4 = lw(s3 - 0x003C); // 0xFFFFFFC4
  t5 = v1 - t3;
  t6 = a2 - t3;
  t7 = s2 - t3;
  t5 = t5 & t6;
  t5 = t5 & t7;
  temp = (int32_t)t5 < 0;
  t5 = t4 - v1;
  if (temp) goto label8004B934;
  t6 = t4 - a2;
  t7 = t4 - s2;
  t5 = t5 & t6;
  t5 = t5 & t7;
  temp = (int32_t)t5 < 0;
  at -= s0;
  if (temp) goto label8004B934;
  v0 -= s1;
  v1 -= s2;
  a0 -= s0;
  a1 -= s1;
  a2 -= s2;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = a2;
  cop2.RTM0 = at;
  cop2.RTM2 = v0;
  cop2.RTM4 = v1;
  a3 = s4 - s0;
  t0 = s5 - s1;
  OP(SF_OFF, LM_OFF);
  t1 = s6 - s2;
  t2 = a3 + s7;
  t3 = t0 + t8;
  t4 = t1 + t9;
  t5 = t0 << 16;
  t6 = a3 & 0xFFFF;
  t5 += t6;
  cop2.RTM0 = t5;
  cop2.RTM1 = t1;
  t5 = t3 << 16;
  t6 = t2 & 0xFFFF;
  t5 += t6;
  cop2.RTM3 = t5;
  cop2.RTM4 = t4;
  t5 = cop2.MAC1;
  t6 = cop2.MAC2;
  t7 = cop2.MAC3;
  t5 = (int32_t)t5 >> 10;
  t6 = (int32_t)t6 >> 10;
  t7 = (int32_t)t7 >> 10;
  cop2.IR1 = t5;
  cop2.IR2 = t6;
  cop2.IR3 = t7;
  sw(s3 - 0x0038, s0); // 0xFFFFFFC8
  sw(s3 - 0x0034, s1); // 0xFFFFFFCC
  MVMVA(SF_OFF, MX_RT, V_IR, CV_NONE, LM_OFF);
  sw(s3 - 0x0030, s2); // 0xFFFFFFD0
  t2 = t0 << 16;
  t3 = a3 & 0xFFFF;
  t2 += t3;
  t3 = t8 << 16;
  s0 = cop2.MAC3;
  s1 = cop2.MAC1;
  temp = (int32_t)s0 >= 0;
  t4 = s7 & 0xFFFF;
  if (temp) goto label8004B934;
  temp = (int32_t)s1 < 0;
  t3 += t4;
  if (temp) goto label8004B934;
  cop2.IR1 = t5;
  cop2.IR2 = t6;
  cop2.IR3 = t7;
  cop2.RTM0 = t2;
  cop2.RTM1 = t1;
  cop2.RTM3 = t3;
  cop2.RTM4 = t9;
  MVMVA(SF_OFF, MX_RT, V_IR, CV_NONE, LM_OFF);
  s0 = cop2.MAC1;
  s1 = cop2.MAC3;
  cop2.LZCS = s0;
  t2 = LZCR();
  t2 -= 2; // 0xFFFFFFFE
  s0 = s0 << t2;
  t2 -= 12; // 0xFFFFFFF4
  temp = (int32_t)t2 >= 0;
  t3 = -t2;
  if (temp) goto label8004BB60;
  s1 = (int32_t)s1 >> t3;
  goto label8004BB64;
label8004BB60:
  s1 = s1 << t2;
label8004BB64:
  div_psx(s0,s1);
  cop2.MAC1 = 0;
  cop2.MAC2 = 0;
  cop2.MAC3 = 0;
  cop2.IR1 = s7;
  cop2.IR2 = t8;
  cop2.IR3 = t9;
  t2=lo;
  cop2.IR0 = t2;
  GPL(SF_OFF, LM_OFF);
  t2 = cop2.MAC1;
  t3 = cop2.MAC2;
  t4 = cop2.MAC3;
  t2 = (int32_t)t2 >> 12;
  t3 = (int32_t)t3 >> 12;
  t4 = (int32_t)t4 >> 12;
  cop2.IR1 = t2;
  cop2.IR2 = t3;
  cop2.IR3 = t4;
  a3 -= t2;
  t0 -= t3;
  SQR(SF_OFF);
  t1 -= t4;
  s0 = lw(s3 - 0x002C); // 0xFFFFFFD4
  t2 = cop2.MAC1;
  t3 = cop2.MAC2;
  t4 = cop2.MAC3;
  t2 += t3;
  t2 += t4;
  t3 = t2 - s0;
  temp = (int32_t)t3 >= 0;
  sw(s3 - 0x0028, t2); // 0xFFFFFFD8
  if (temp) goto label8004B934;
  t2 = t5;
  t3 = 0;
  s0 = t2 + t6;
  s1 = t2 - t6;
  s0 = s0 ^ s1;
  temp = (int32_t)s0 >= 0;
  if (temp) goto label8004BC10;
  t2 = t6;
  t3 = 1; // 0x0001
label8004BC10:
  s0 = t2 + t7;
  s1 = t2 - t7;
  s0 = s0 ^ s1;
  temp = (int32_t)s0 >= 0;
  if (temp) goto label8004BC2C;
  t2 = t7;
  t3 = 2; // 0x0002
label8004BC2C:
  t3 = t3 << 4;
  cop2.IR1 = at;
  cop2.IR2 = v0;
  cop2.IR3 = v1;
  cop2.RTM0 = a3;
  cop2.RTM2 = t0;
  cop2.RTM4 = t1;
  s0 = 0x8004BC60; // &0x4810C800
  OP(SF_OFF, LM_OFF);
  s0 += t3;
  temp = s0;
  switch (temp)
  {
  case 0x8004BC60:
    goto label8004BC60;
    break;
  case 0x8004BC70:
    goto label8004BC70;
    break;
  case 0x8004BC80:
    goto label8004BC80;
    break;
  default:
    JR(temp, 0x8004BC58);
    return;
  }
label8004BC60:
  s0 = cop2.MAC1;
  goto label8004BC88;
label8004BC70:
  s0 = cop2.MAC2;
  goto label8004BC88;
label8004BC80:
  s0 = cop2.MAC3;
label8004BC88:
  temp = s0 == 0;
  s0 = s0 ^ t2;
  if (temp) goto label8004BC98;
  temp = (int32_t)s0 >= 0;
  if (temp) goto label8004B934;
label8004BC98:
  cop2.IR1 = a3;
  cop2.IR2 = t0;
  cop2.IR3 = t1;
  cop2.RTM0 = a0;
  cop2.RTM2 = a1;
  cop2.RTM4 = a2;
  s0 = 0x8004BCC8; // &0x4810C800
  OP(SF_OFF, LM_OFF);
  s0 += t3;
  temp = s0;
  switch (temp)
  {
  case 0x8004BCC8:
    goto label8004BCC8;
    break;
  case 0x8004BCD8:
    goto label8004BCD8;
    break;
  case 0x8004BCE8:
    goto label8004BCE8;
    break;
  default:
    JR(temp, 0x8004BCC0);
    return;
  }
label8004BCC8:
  s0 = cop2.MAC1;
  goto label8004BCF0;
label8004BCD8:
  s0 = cop2.MAC2;
  goto label8004BCF0;
label8004BCE8:
  s0 = cop2.MAC3;
label8004BCF0:
  temp = s0 == 0;
  s0 = s0 ^ t2;
  if (temp) goto label8004BD00;
  temp = (int32_t)s0 >= 0;
  if (temp) goto label8004B934;
label8004BD00:
  a0 -= at;
  a1 -= v0;
  a2 -= v1;
  at = a3 - at;
  v0 = t0 - v0;
  v1 = t1 - v1;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = a2;
  cop2.RTM0 = at;
  cop2.RTM2 = v0;
  cop2.RTM4 = v1;
  s0 = 0x8004BD48; // &0x4810C800
  OP(SF_OFF, LM_OFF);
  s0 += t3;
  temp = s0;
  switch (temp)
  {
  case 0x8004BD48:
    goto label8004BD48;
    break;
  case 0x8004BD58:
    goto label8004BD58;
    break;
  case 0x8004BD68:
    goto label8004BD68;
    break;
  default:
    JR(temp, 0x8004BD40);
    return;
  }
label8004BD48:
  s0 = cop2.MAC1;
  goto label8004BD70;
label8004BD58:
  s0 = cop2.MAC2;
  goto label8004BD70;
label8004BD68:
  s0 = cop2.MAC3;
label8004BD70:
  temp = s0 == 0;
  s0 = s0 ^ t2;
  if (temp) goto label8004BD80;
  temp = (int32_t)s0 >= 0;
  if (temp) goto label8004B934;
label8004BD80:
  at = lw(s3 - 0x0038); // 0xFFFFFFC8
  v0 = lw(s3 - 0x0034); // 0xFFFFFFCC
  v1 = lw(s3 - 0x0030); // 0xFFFFFFD0
  at += a3;
  v0 += t0;
  v1 += t1;
  a0 = 0x80076B80;
  sw(a0 + 0x0000, at);
  sw(a0 + 0x0004, v0);
  sw(a0 + 0x0008, v1);
  a0 = 0x80077368;
  sw(a0 + 0x0000, t5);
  sw(a0 + 0x0004, t6);
  sw(a0 + 0x0008, t7);
  a0 = 0x800757E4;
  sw(a0 + 0x0000, fp);
  a0 = 0x80075808;
  at = -1; // 0xFFFFFFFF
  sw(a0 + 0x0000, at);
  at = lw(s3 - 0x0028); // 0xFFFFFFD8
  sw(s3 - 0x002C, at); // 0xFFFFFFD4
  goto label8004B934;
label8004BDE8:
  at = lw(s3 - 0x002C); // 0xFFFFFFD4
  v0 = 0x40000000;
  temp = at == v0;
  v0 = 0;
  if (temp) goto label8004BE3C;
  v0 = 1; // 0x0001
  at = 0x80077DD8;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  return;
label8004BE3C:
  s7 += s4;
  t8 += s5;
  t9 += s6;
  goto label8004AF88;
}

// size: 0x000017A0
void function_8004BE4C(void)
{
  uint32_t temp;
  at = 0x80077DD8;
  sw(at + 0x0000, s0);
  sw(at + 0x0004, s1);
  sw(at + 0x0008, s2);
  sw(at + 0x000C, s3);
  sw(at + 0x0010, s4);
  sw(at + 0x0014, s5);
  sw(at + 0x0018, s6);
  sw(at + 0x001C, s7);
  sw(at + 0x0020, gp);
  sw(at + 0x0024, sp);
  sw(at + 0x0028, fp);
  sw(at + 0x002C, ra);
  mult(a1, a1);
  t9 = lw(a0 + 0x0008);
  t8 = lw(a0 + 0x0004);
  s7 = lw(a0 + 0x0000);
  s6 = a1;
  s5 = 0x1F800000;
  a1=lo;
  sw(s5 + 0x0054, a1);
  sw(s5 + 0x005C, a2);
  a2 = 0x8006FCF4;
  sw(s5 + 0x0058, a2);
  v0 = 0;
  at = t9 - s6;
  temp = (int32_t)at < 0;
  at = t8 - s6;
  if (temp) goto label8004CAE4;
  temp = (int32_t)at < 0;
  at = s7 - s6;
  if (temp) goto label8004CAE4;
  temp = (int32_t)at < 0;
  at = 0x1F800000;
  if (temp) goto label8004CAE4;
  v0 = 0x80075778;
  v0 = lw(v0 + 0x0000);
  v1 = -1024; // 0xFFFFFC00
  v0 += 4096; // 0x1000
  a0 = s7 >> 13;
  a1 = t8 >> 13;
  a1 = a1 << 5;
  t0 = a0 + a1;
  t1 = t0 << 2;
  t1 += v0;
  t1 = lw(t1 + 0x0000);
  t2 = s7 & 0x1000;
  temp = t1 == 0;
  sw(at + 0x0000, t1);
  if (temp) goto label8004BF14;
  at += 4; // 0x0004
label8004BF14:
  temp = t2 == 0;
  a0 = -1; // 0xFFFFFFFF
  if (temp) goto label8004BF20;
  a0 = 1; // 0x0001
label8004BF20:
  t1 = t0 + a0;
  t2 = t1 & v1;
  temp = (int32_t)t2 > 0;
  t1 = t1 << 2;
  if (temp) goto label8004BF48;
  t1 += v0;
  t1 = lw(t1 + 0x0000);
  t2 = t8 & 0x1000;
  temp = t1 == 0;
  sw(at + 0x0000, t1);
  if (temp) goto label8004BF48;
  at += 4; // 0x0004
label8004BF48:
  temp = t2 == 0;
  a1 = -32; // 0xFFFFFFE0
  if (temp) goto label8004BF54;
  a1 = 32; // 0x0020
label8004BF54:
  t1 = t0 + a1;
  t2 = t1 & v1;
  temp = (int32_t)t2 > 0;
  t1 = t1 << 2;
  if (temp) goto label8004BF7C;
  t1 += v0;
  t1 = lw(t1 + 0x0000);
  temp = t1 == 0;
  sw(at + 0x0000, t1);
  if (temp) goto label8004BF7C;
  at += 4; // 0x0004
label8004BF7C:
  t1 = t0 + a0;
  t1 += a1;
  t2 = t1 & v1;
  temp = (int32_t)t2 > 0;
  t1 = t1 << 2;
  if (temp) goto label8004BFA8;
  t1 += v0;
  t1 = lw(t1 + 0x0000);
  temp = t1 == 0;
  sw(at + 0x0000, t1);
  if (temp) goto label8004BFA8;
  at += 4; // 0x0004
label8004BFA8:
  s5 = 0x1F800000;
  temp = at != s5;
  sw(at + 0x0000, 0);
  if (temp) goto label8004CB44;
label8004BFB4:
  at = s7 + s6;
  at = at >> 4;
  at++;
  sw(s5 + 0x0034, at);
  at = t8 + s6;
  at = at >> 4;
  at++;
  sw(s5 + 0x003C, at);
  at = t9 + s6;
  at = at >> 4;
  at++;
  sw(s5 + 0x0044, at);
  v0 = s6 >> 3;
  v0++;
  sw(s5 + 0x0030, v0);
  sw(s5 + 0x0038, v0);
  sw(s5 + 0x0040, v0);
  a0 = 0x800785A8;
  a0 = lw(a0 + 0x002C);
  at = s7 >> 12;
  v0 = t8 >> 12;
  v1 = t9 >> 12;
  sp = lw(a0 + 0x0010);
  a0 = s7 + s6;
  a0 = a0 >> 12;
  temp = a0 != at;
  a1 = s7 - s6;
  if (temp) goto label8004C028;
  a0 = a1 >> 12;
label8004C028:
  a1 = t8 + s6;
  a1 = a1 >> 12;
  temp = a1 != v0;
  a2 = t8 - s6;
  if (temp) goto label8004C03C;
  a1 = a2 >> 12;
label8004C03C:
  a2 = t9 + s6;
  a2 = a2 >> 12;
  temp = a2 != v1;
  a3 = t9 - s6;
  if (temp) goto label8004C050;
  a2 = a3 >> 12;
label8004C050:
  ra = 0x1F800000;
  sh(ra + 0x0000, at);
  sh(ra + 0x0002, v0);
  temp = at == a0;
  sh(ra + 0x0004, v1);
  if (temp) goto label8004C100;
  sh(ra + 0x0006, a0);
  sh(ra + 0x0008, v0);
  temp = v0 == a1;
  sh(ra + 0x000A, v1);
  if (temp) goto label8004C0D0;
  sh(ra + 0x000C, at);
  sh(ra + 0x000E, a1);
  sh(ra + 0x0010, v1);
  sh(ra + 0x0012, a0);
  sh(ra + 0x0014, a1);
  temp = v1 == a2;
  sh(ra + 0x0016, v1);
  if (temp) goto label8004C0C8;
  sh(ra + 0x0018, at);
  sh(ra + 0x001A, v0);
  sh(ra + 0x001C, a2);
  sh(ra + 0x001E, at);
  sh(ra + 0x0020, a1);
  sh(ra + 0x0022, a2);
  sh(ra + 0x0024, a0);
  sh(ra + 0x0026, v0);
  sh(ra + 0x0028, a2);
  sh(ra + 0x002A, a0);
  sh(ra + 0x002C, a1);
  sh(ra + 0x002E, a2);
  ra += 48; // 0x0030
  goto label8004C160;
label8004C0C8:
  ra += 24; // 0x0018
  goto label8004C160;
label8004C0D0:
  temp = v1 == a2;
  if (temp) goto label8004C0F8;
  sh(ra + 0x000C, at);
  sh(ra + 0x000E, v0);
  sh(ra + 0x0010, a2);
  sh(ra + 0x0012, a0);
  sh(ra + 0x0014, v0);
  sh(ra + 0x0016, a2);
  ra += 24; // 0x0018
  goto label8004C160;
label8004C0F8:
  ra += 12; // 0x000C
  goto label8004C160;
label8004C100:
  temp = v0 == a1;
  if (temp) goto label8004C140;
  sh(ra + 0x0006, at);
  sh(ra + 0x0008, a1);
  temp = v1 == a2;
  sh(ra + 0x000A, v1);
  if (temp) goto label8004C138;
  sh(ra + 0x000C, at);
  sh(ra + 0x000E, v0);
  sh(ra + 0x0010, a2);
  sh(ra + 0x0012, at);
  sh(ra + 0x0014, a1);
  sh(ra + 0x0016, a2);
  ra += 24; // 0x0018
  goto label8004C160;
label8004C138:
  ra += 12; // 0x000C
  goto label8004C160;
label8004C140:
  temp = v1 == a2;
  if (temp) goto label8004C15C;
  sh(ra + 0x0006, at);
  sh(ra + 0x0008, v0);
  sh(ra + 0x000A, a2);
  ra += 12; // 0x000C
  goto label8004C160;
label8004C15C:
  ra += 6; // 0x0006
label8004C160:
  at = ra << 16;
  temp = (int32_t)at <= 0;
  ra -= 6; // 0xFFFFFFFA
  if (temp) goto label8004C978;
  at = lh(ra + 0x0004);
  a0 = 0x800785A8;
  a0 = lw(a0 + 0x002C);
  temp = (int32_t)at < 0;
  a3 = 0 | 0xFFFF;
  if (temp) goto label8004C160;
  a1 = lw(a0 + 0x0008);
  v0 = at << 1;
  v1 = lhu(a1 + 0x0000);
  a2 = a1 + v0;
  a2 = lhu(a2 + 0x0002);
  v1 -= at;
  temp = a2 == a3;
  a2 += a1;
  if (temp) goto label8004C160;
  temp = (int32_t)v1 <= 0;
  at = lh(ra + 0x0002);
  if (temp) goto label8004C160;
  v1 = lhu(a2 + 0x0000);
  temp = (int32_t)at < 0;
  v0 = at << 1;
  if (temp) goto label8004C160;
  a2 += v0;
  a2 = lhu(a2 + 0x0002);
  v1 -= at;
  temp = a2 == a3;
  a2 += a1;
  if (temp) goto label8004C160;
  temp = (int32_t)v1 <= 0;
  at = lh(ra + 0x0000);
  if (temp) goto label8004C160;
  v1 = lhu(a2 + 0x0000);
  temp = (int32_t)at < 0;
  v0 = at << 1;
  if (temp) goto label8004C160;
  a2 += v0;
  a2 = lhu(a2 + 0x0002);
  v1 -= at;
  temp = (int32_t)v1 <= 0;
  fp = lw(a0 + 0x000C);
  if (temp) goto label8004C160;
  temp = a2 == a3;
  a2 = a2 << 1;
  if (temp) goto label8004C160;
  fp += a2;
  gp = lh(fp + 0x0000);
  gp = gp & 0x7FFF;
  goto label8004C210;
label8004C20C:
  gp = lh(fp + 0x0000);
label8004C210:
  fp += 2; // 0x0002
  temp = (int32_t)gp < 0;
  at = gp << 2;
  if (temp) goto label8004C160;
  v0 = gp << 3;
  at += v0;
  t2 = at + sp;
  s4 = lw(t2 + 0x0008);
  t3 = lw(s5 + 0x0044);
  t1 = s4 & 0x3FFF;
  t4 = t1 - t3;
  temp = (int32_t)t4 > 0;
  v1 = s4 >> 24;
  if (temp) goto label8004C20C;
  a2 = s4 >> 16;
  t3 = lw(s5 + 0x0040);
  a2 = a2 & 0xFF;
  t4 += t3;
  t5 = t4 + v1;
  t4 += a2;
  t4 = t4 & t5;
  temp = (int32_t)t4 < 0;
  t3 = lw(s5 + 0x003C);
  if (temp) goto label8004C20C;
  t0 = lw(t2 + 0x0004);
  t4 = lw(s5 + 0x0038);
  v0 = (int32_t)t0 >> 23;
  a1 = t0 << 9;
  a1 = (int32_t)a1 >> 23;
  t0 = t0 & 0x3FFF;
  t5 = t0 - t3;
  t6 = t5 + a1;
  t7 = t5 + v0;
  t3 = t5 | t6;
  t3 = t3 | t7;
  temp = (int32_t)t3 > 0;
  t5 += t4;
  if (temp) goto label8004C20C;
  t6 = t5 + a1;
  t7 = t5 + v0;
  t4 = t5 & t6;
  t4 = t4 & t7;
  temp = (int32_t)t4 < 0;
  t3 = lw(s5 + 0x0034);
  if (temp) goto label8004C20C;
  a3 = lw(t2 + 0x0000);
  t4 = lw(s5 + 0x0030);
  at = (int32_t)a3 >> 23;
  a0 = a3 << 9;
  a0 = (int32_t)a0 >> 23;
  a3 = a3 & 0x3FFF;
  t5 = a3 - t3;
  t6 = t5 + a0;
  t7 = t5 + at;
  t3 = t5 | t6;
  t3 = t3 | t7;
  temp = (int32_t)t3 > 0;
  t5 += t4;
  if (temp) goto label8004C20C;
  t6 = t5 + a0;
  t7 = t5 + at;
  t4 = t5 & t6;
  t4 = t4 & t7;
  temp = (int32_t)t4 < 0;
  a3 = a3 << 4;
  if (temp) goto label8004C20C;
  s4 = s4 & 0x4000;
  temp = (int32_t)s4 > 0;
  t0 = t0 << 4;
  if (temp) goto label8004C20C;
  t1 = t1 << 4;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = a2;
  cop2.RTM0 = at;
  cop2.RTM2 = v0;
  cop2.RTM4 = v1;
  t2 = s7 - a3;
  t3 = t8 - t0;
  OP(SF_OFF, LM_OFF);
  t4 = t9 - t1;
  t5 = t3 << 16;
  t6 = t2 & 0xFFFF;
  t5 += t6;
  cop2.RTM1 = t4;
  cop2.RTM0 = t5;
  t5 = cop2.MAC1;
  t6 = cop2.MAC2;
  t7 = cop2.MAC3;
  t5 = (int32_t)t5 >> 2;
  t6 = (int32_t)t6 >> 2;
  t7 = (int32_t)t7 >> 2;
  cop2.IR1 = t5;
  cop2.IR2 = t6;
  cop2.IR3 = t7;
  s0 = t3 << 16;
  s1 = t2 & 0xFFFF;
  MVMVA(SF_OFF, MX_RT, V_IR, CV_NONE, LM_OFF);
  s0 += s1;
  s1 = t6 << 16;
  s2 = t5 & 0xFFFF;
  s1 += s2;
  s2 = cop2.MAC1;
  temp = (int32_t)s2 <= 0;
  if (temp) goto label8004C20C;
  cop2.IR1 = t5;
  cop2.IR2 = t6;
  cop2.IR3 = t7;
  cop2.RTM0 = s0;
  cop2.RTM1 = t4;
  cop2.RTM3 = s1;
  cop2.RTM4 = t7;
  MVMVA(SF_OFF, MX_RT, V_IR, CV_NONE, LM_OFF);
  s0 = cop2.MAC1;
  s1 = cop2.MAC3;
  s0 = s0 << 4;
  s1 = (int32_t)s1 >> 6;
  div_psx(s0,s1);
  at = at << 4;
  v0 = v0 << 4;
  v1 = v1 << 4;
  a0 = a0 << 4;
  a1 = a1 << 4;
  a2 = a2 << 4;
  cop2.MAC1 = 0;
  cop2.MAC2 = 0;
  cop2.MAC3 = 0;
  cop2.IR1 = t5;
  cop2.IR2 = t6;
  cop2.IR3 = t7;
  s0=lo;
  cop2.IR0 = s0;
  GPL(SF_OFF, LM_OFF);
  s0 = cop2.MAC1;
  s1 = cop2.MAC2;
  s2 = cop2.MAC3;
  s0 = (int32_t)s0 >> 10;
  s1 = (int32_t)s1 >> 10;
  s2 = (int32_t)s2 >> 10;
  cop2.IR1 = s0;
  cop2.IR2 = s1;
  cop2.IR3 = s2;
  mult(s6, s6);
  t2 -= s0;
  SQR(SF_OFF);
  t3 -= s1;
  t4 -= s2;
  s0 = cop2.MAC1;
  s1 = cop2.MAC2;
  s2 = cop2.MAC3;
  s3=lo;
  s3 -= s0;
  s3 -= s1;
  s3 -= s2;
  temp = (int32_t)s3 <= 0;
  if (temp) goto label8004C20C;
  s0 = t5;
  s1 = 0;
  s2 = s0 + t6;
  s3 = s0 - t6;
  s2 = s2 ^ s3;
  temp = (int32_t)s2 >= 0;
  if (temp) goto label8004C498;
  s0 = t6;
  s1 = 1; // 0x0001
label8004C498:
  s2 = s0 + t7;
  s3 = s0 - t7;
  s2 = s2 ^ s3;
  temp = (int32_t)s2 >= 0;
  if (temp) goto label8004C4B4;
  s0 = t7;
  s1 = 2; // 0x0002
label8004C4B4:
  s1 = s1 << 4;
  s4 = 0;
  cop2.IR1 = at;
  cop2.IR2 = v0;
  cop2.IR3 = v1;
  cop2.RTM0 = t2;
  cop2.RTM2 = t3;
  cop2.RTM4 = t4;
  s2 = 0x8004C4EC; // &0x4812C800
  OP(SF_OFF, LM_OFF);
  s2 += s1;
  temp = s2;
  switch (temp)
  {
  case 0x8004C4EC:
    goto label8004C4EC;
    break;
  case 0x8004C4FC:
    goto label8004C4FC;
    break;
  case 0x8004C50C:
    goto label8004C50C;
    break;
  default:
    JR(temp, 0x8004C4E4);
    return;
  }
label8004C4EC:
  s2 = cop2.MAC1;
  goto label8004C514;
label8004C4FC:
  s2 = cop2.MAC2;
  goto label8004C514;
label8004C50C:
  s2 = cop2.MAC3;
label8004C514:
  s2 = s2 ^ s0;
  s2 = s2 >> 31;
  s4 = s4 | s2;
  cop2.IR1 = t2;
  cop2.IR2 = t3;
  cop2.IR3 = t4;
  cop2.RTM0 = a0;
  cop2.RTM2 = a1;
  cop2.RTM4 = a2;
  s2 = 0x8004C550; // &0x4812C800
  OP(SF_OFF, LM_OFF);
  s2 += s1;
  temp = s2;
  switch (temp)
  {
  case 0x8004C550:
    goto label8004C550;
    break;
  case 0x8004C560:
    goto label8004C560;
    break;
  case 0x8004C570:
    goto label8004C570;
    break;
  default:
    JR(temp, 0x8004C548);
    return;
  }
label8004C550:
  s2 = cop2.MAC1;
  goto label8004C578;
label8004C560:
  s2 = cop2.MAC2;
  goto label8004C578;
label8004C570:
  s2 = cop2.MAC3;
label8004C578:
  s2 = s2 ^ s0;
  s2 = s2 >> 31;
  s2 = s2 << 1;
  s4 = s4 | s2;
  s2 = a0 - at;
  cop2.IR1 = s2;
  s2 = a1 - v0;
  cop2.IR2 = s2;
  s2 = a2 - v1;
  cop2.IR3 = s2;
  s2 = t2 - at;
  cop2.RTM0 = s2;
  s2 = t3 - v0;
  cop2.RTM2 = s2;
  s2 = t4 - v1;
  cop2.RTM4 = s2;
  s2 = 0x8004C5D0; // &0x4812C800
  OP(SF_OFF, LM_OFF);
  s2 += s1;
  temp = s2;
  switch (temp)
  {
  case 0x8004C5D0:
    goto label8004C5D0;
    break;
  case 0x8004C5E0:
    goto label8004C5E0;
    break;
  case 0x8004C5F0:
    goto label8004C5F0;
    break;
  default:
    JR(temp, 0x8004C5C8);
    return;
  }
label8004C5D0:
  s2 = cop2.MAC1;
  goto label8004C5F8;
label8004C5E0:
  s2 = cop2.MAC2;
  goto label8004C5F8;
label8004C5F0:
  s2 = cop2.MAC3;
label8004C5F8:
  s2 = s2 ^ s0;
  s2 = s2 >> 31;
  s2 = s2 << 2;
  s4 = s4 | s2;
  s4 = s4 << 3;
  s3 = 0x8004C620;
  s4 += s3;
  temp = s4;
  switch (temp)
  {
  case 0x8004C620:
    goto label8004C620;
    break;
  case 0x8004C628:
    goto label8004C628;
    break;
  case 0x8004C630:
    goto label8004C630;
    break;
  case 0x8004C638:
    goto label8004C638;
    break;
  case 0x8004C640:
    goto label8004C640;
    break;
  case 0x8004C648:
    goto label8004C648;
    break;
  case 0x8004C650:
    goto label8004C650;
    break;
  case 0x8004C658:
    goto label8004C658;
    break;
  default:
    JR(temp, 0x8004C618);
    return;
  }
label8004C620:
label8004C628:
  goto label8004C660;
label8004C630:
  goto label8004C670;
label8004C638:
  goto label8004C680;
label8004C640:
  goto label8004C6B8;
label8004C648:
  goto label8004C6C8;
label8004C650:
  goto label8004C6E8;
label8004C658:
  goto label8004C708;
label8004C660:
  at = a0 + a3;
  v0 = a1 + t0;
  v1 = a2 + t1;
  goto label8004C88C;
label8004C670:
  at += a3;
  v0 += t0;
  v1 += t1;
  goto label8004C88C;
label8004C680:
  at = (int32_t)at >> 4;
  v0 = (int32_t)v0 >> 4;
  v1 = (int32_t)v1 >> 4;
  a0 = (int32_t)a0 >> 4;
  a1 = (int32_t)a1 >> 4;
  a2 = (int32_t)a2 >> 4;
  s0 = a0 << 4;
  s1 = a1 << 4;
  s2 = a2 << 4;
  t2 -= s0;
  t3 -= s1;
  t4 -= s2;
  goto label8004C718;
label8004C6B8:
  at = a3;
  v0 = t0;
  v1 = t1;
  goto label8004C88C;
label8004C6C8:
  at = (int32_t)a0 >> 4;
  v0 = (int32_t)a1 >> 4;
  v1 = (int32_t)a2 >> 4;
  a0 = 0;
  a1 = 0;
  a2 = 0;
  goto label8004C718;
label8004C6E8:
  at = (int32_t)at >> 4;
  v0 = (int32_t)v0 >> 4;
  v1 = (int32_t)v1 >> 4;
  a0 = 0;
  a1 = 0;
  a2 = 0;
  goto label8004C718;
label8004C708:
  at = t2 + a3;
  v0 = t3 + t0;
  v1 = t4 + t1;
  goto label8004C88C;
label8004C718:
  s0 = t3 << 16;
  s1 = t2 & 0xFFFF;
  s0 += s1;
  cop2.RTM3 = s0;
  cop2.RTM4 = t4;
  t2 = at - a0;
  t3 = v0 - a1;
  t4 = v1 - a2;
  cop2.IR1 = t2;
  cop2.IR2 = t3;
  cop2.IR3 = t4;
  s0 = t3 << 16;
  s1 = t2 & 0xFFFF;
  s0 += s1;
  cop2.RTM0 = s0;
  cop2.RTM1 = t4;
  MVMVA(SF_OFF, MX_RT, V_IR, CV_NONE, LM_OFF);
  s0 = cop2.MAC3;
  s1 = cop2.MAC1;
  s0 = s0 << 4;
  div_psx(s0,s1);
  at = at << 4;
  v0 = v0 << 4;
  v1 = v1 << 4;
  a0 = a0 << 4;
  a1 = a1 << 4;
  a2 = a2 << 4;
  cop2.MAC1 = 0;
  cop2.MAC2 = 0;
  cop2.MAC3 = 0;
  cop2.IR1 = t2;
  cop2.IR2 = t3;
  cop2.IR3 = t4;
  s0 = at - a0;
  s1 = 0;
  t2 = v0 - a1;
  t3 = s0 + t2;
  t2 = s0 - t2;
  t2 = t2 ^ t3;
  temp = (int32_t)t2 >= 0;
  if (temp) goto label8004C7CC;
  s0 = v0 - a1;
  s1 = 8; // 0x0008
label8004C7CC:
  t2 = v1 - a2;
  t3 = s0 + t2;
  t2 = s0 - t2;
  t2 = t2 ^ t3;
  temp = (int32_t)t2 >= 0;
  if (temp) goto label8004C7EC;
  s0 = v1 - a2;
  s1 = 16; // 0x0010
label8004C7EC:
  t2=lo;
  cop2.IR0 = t2;
  t3 = 0x8004C820; // &0x0801320E
  GPL(SF_OFF, LM_OFF);
  s1 += t3;
  t2 = cop2.MAC1;
  t3 = cop2.MAC2;
  t4 = cop2.MAC3;
  t2 = (int32_t)t2 >> 4;
  t3 = (int32_t)t3 >> 4;
  temp = s1;
  t4 = (int32_t)t4 >> 4;
  switch (temp)
  {
  case 0x8004C820:
    goto label8004C820;
    break;
  case 0x8004C828:
    goto label8004C828;
    break;
  case 0x8004C830:
    goto label8004C830;
    break;
  default:
    JR(temp, 0x8004C818);
    return;
  }
label8004C820:
  s1 = t2;
  goto label8004C838;
label8004C828:
  s1 = t3;
  goto label8004C838;
label8004C830:
  s1 = t4;
  goto label8004C838;
label8004C838:
  s2 = s0 ^ s1;
  temp = (int32_t)s2 >= 0;
  if (temp) goto label8004C854;
  t2 = 0;
  t3 = 0;
  t4 = 0;
  goto label8004C874;
label8004C854:
  s2 = s0 - s1;
  s3 = s0 + s1;
  s2 = s2 ^ s3;
  temp = (int32_t)s2 >= 0;
  if (temp) goto label8004C874;
  t2 = at - a0;
  t3 = v0 - a1;
  t4 = v1 - a2;
label8004C874:
  t2 += a0;
  t3 += a1;
  t4 += a2;
  at = t2 + a3;
  v0 = t3 + t0;
  v1 = t4 + t1;
label8004C88C:
  a0 = s7 - at;
  a1 = t8 - v0;
  a2 = t9 - v1;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = a2;
  SQR(SF_OFF);
  mult(s6, s6);
  a0 = cop2.MAC1;
  a1 = cop2.MAC2;
  a2 = cop2.MAC3;
  a0 += a1;
  a0 += a2;
  s4=lo;
  s4 -= a0;
  temp = (int32_t)s4 <= 0;
  s4 = lw(s5 + 0x0058);
  if (temp) goto label8004C20C;
  a1 = lw(s5 + 0x0054);
  sw(s4 + 0x0000, at);
  sw(s4 + 0x0004, v0);
  sw(s4 + 0x0008, v1);
  sw(s4 + 0x000C, a0);
  s4 += 16; // 0x0010
  sw(s5 + 0x0058, s4);
  a1 -= a0;
  temp = (int32_t)a1 <= 0;
  if (temp) goto label8004C20C;
  sw(s5 + 0x0048, at);
  sw(s5 + 0x004C, v0);
  sw(s5 + 0x0050, v1);
  sw(s5 + 0x0054, a0);
  at = 0x80077368;
  sw(at + 0x0000, t5);
  sw(at + 0x0004, t6);
  sw(at + 0x0008, t7);
  at = 0x800757E4;
  sw(at + 0x0000, 0);
  at = 0x80075808;
  sw(at + 0x0000, gp);
  at = 0x800785A8;
  at = lw(at + 0x002C);
  v0 = lw(at + 0x0004);
  at = lw(at + 0x0018);
  v0 -= gp;
  temp = (int32_t)v0 <= 0;
  v0 = 255; // 0x00FF
  if (temp) goto label8004C968;
  at += gp;
  v0 = lbu(at + 0x0000);
label8004C968:
  a0 = 0x80075718;
  sw(a0 + 0x0000, v0);
  goto label8004C20C;
label8004C978:
  s6 = lw(s5 + 0x005C);
  a0 = lw(s5 + 0x0054);
  mult(s6, s6);
  temp = a0 == 0;
  v0 = 0;
  if (temp) goto label8004CAE4;
  a1=lo;
  a1 = a0 - a1;
  temp = (int32_t)a1 >= 0;
  v0 = 0;
  if (temp) goto label8004CAE4;
  at = lw(s5 + 0x0048);
  v0 = lw(s5 + 0x004C);
  v1 = lw(s5 + 0x0050);
  cop2.LZCS = a0;
  a2 = 0x80077858;
  a1 = LZCR();
  t0 = -2; // 0xFFFFFFFE
  t0 = a1 & t0;
  a3 = 31; // 0x001F
  a3 -= t0;
  a3 = (int32_t)a3 >> 1;
  t1 = t0 - 24; // 0xFFFFFFE8
  sw(a2 + 0x0000, at);
  sw(a2 + 0x0004, v0);
  temp = (int32_t)t1 < 0;
  sw(a2 + 0x0008, v1);
  if (temp) goto label8004C9E8;
  t2 = a0 << t1;
  t1 = 24; // 0x0018
  goto label8004C9F4;
label8004C9E8:
  t1 = 24; // 0x0018
  t1 -= t0;
  t2 = (int32_t)a0 >> t1;
label8004C9F4:
  t2 -= 64; // 0xFFFFFFC0
  t2 = t2 << 1;
  t3 = 0x80074B84; // &0x101F1000
  t3 += t2;
  t3 = lh(t3 + 0x0000);
  t3 = t3 << a3;
  a3 = t3 >> 12;
label8004CA18:
  a0 = s7 - at;
  a1 = t8 - v0;
  a2 = t9 - v1;
  cop2.MAC1 = 0;
  cop2.MAC2 = 0;
  cop2.MAC3 = 0;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = a2;
  cop2.IR0 = s6;
  GPL(SF_OFF, LM_OFF);
  a0 = cop2.MAC1;
  a1 = cop2.MAC2;
  a2 = cop2.MAC3;
  div_psx(a0,a3);
  a0=lo;
  div_psx(a1,a3);
  a1=lo;
  div_psx(a2,a3);
  a2=lo;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = a2;
  SQR(SF_OFF);
  t0 = cop2.MAC1;
  t1 = cop2.MAC2;
  t2 = cop2.MAC3;
  t0 += t1;
  t0 += t2;
  mult(s6, s6);
  t1=lo;
  t0 -= t1;
  temp = (int32_t)t0 < 0;
  a3--;
  if (temp) goto label8004CA18;
  at += a0;
  v0 += a1;
  v1 += a2;
  a0 = 0x80076B80;
  sw(a0 + 0x0000, at);
  sw(a0 + 0x0004, v0);
  sw(a0 + 0x0008, v1);
  v0 = 1; // 0x0001
label8004CAE4:
  a0 = 0x80075774;
  a1 = lw(s5 + 0x0058);
  a2 = 0x8006FCF4;
  a1 -= a2;
  a1 = a1 >> 4;
  sw(a0 + 0x0000, a1);
  at = 0x80077DD8;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  return;
label8004CB44:
  ra = 0x1F800000;
  s5 += 128; // 0x0080
label8004CB4C:
  at = lw(ra + 0x0000);
  fp = ra - 4; // 0xFFFFFFFC
  temp = at == 0;
  ra += 4; // 0x0004
  if (temp) goto label8004D428;
label8004CB5C:
  fp = lw(fp + 0x0004);
  temp = fp == 0;
  at = lw(fp + 0x000C);
  if (temp) goto label8004CB4C;
  v0 = lw(fp + 0x0010);
  v1 = lw(fp + 0x0014);
  a0 = at - s7;
  a1 = v0 - t8;
  a2 = v1 - t9;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = a2;
  sp = lw(fp + 0x0008);
  a0 = 0x80000000;
  temp = sp == 0;
  sp = sp | a0;
  if (temp) goto label8004CB5C;
  SQR(SF_OFF);
  a0 = lhu(sp + 0x0000);
  sp += 4; // 0x0004
  a0 += s6;
  mult(a0, a0);
  a2 = cop2.MAC1;
  a3 = cop2.MAC2;
  t0 = cop2.MAC3;
  t1 = a2 + a3;
  t1 += t0;
  a0=lo;
  a0 = t1 - a0;
  temp = (int32_t)a0 >= 0;
  if (temp) goto label8004CB5C;
  t2 = lw(fp + 0x0020);
  t3 = lw(fp + 0x0024);
  t4 = lw(fp + 0x0028);
  t5 = lw(fp + 0x002C);
  t6 = lw(fp + 0x0030);
  cop2.RTM0 = t2;
  cop2.RTM1 = t3;
  cop2.RTM2 = t4;
  cop2.RTM3 = t5;
  cop2.RTM4 = t6;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  a1 = lhu(fp + 0x0036);
  a2 = 0x80076378;
  a1 = a1 << 2;
  a1 += a2;
  a2 = lbu(fp + 0x003C);
  a1 = lw(a1 + 0x0000);
  a2 = a2 << 2;
  a1 += a2;
  a2 = lbu(fp + 0x003E);
  t0 = lw(a1 + 0x0038);
  a2 = a2 << 3;
  a3 = t0 + a2;
  a1 = lw(a3 + 0x0024);
  a2 = lbu(t0 + 0x0005);
  a0 = lhu(sp - 0x0002); // 0xFFFFFFFE
  a1 = a1 << 11;
  a1 = a1 >> 11;
  a3 = 0x80000000;
  a1 = a1 | a3;
  a0 = a0 << 2;
  a0 += a1;
  a0 += 4; // 0x0004
  a3 = s5 - 16; // 0xFFFFFFF0
label8004CC68:
  RTPS();
  t0 = lw(a1 + 0x0000);
  a1 += 4; // 0x0004
  t2 = t0 << 20;
  t2 = (int32_t)t2 >> 19;
  t1 = t0 << 10;
  t1 = (int32_t)t1 >> 21;
  t0 = (int32_t)t0 >> 21;
  t0 = t0 << a2;
  t1 = t1 << a2;
  t2 = t2 << a2;
  t2 = t2 << 16;
  t1 += t2;
  cop2.VZ0 = t0;
  cop2.VXY0 = t1;
  t0 = cop2.IR3;
  t1 = cop2.IR1;
  t2 = cop2.IR2;
  t0 += at;
  t1 = v0 - t1;
  t2 = v1 - t2;
  sw(a3 + 0x0000, t0);
  sw(a3 + 0x0004, t1);
  sw(a3 + 0x0008, t2);
  temp = a1 != a0;
  a3 += 16; // 0x0010
  if (temp) goto label8004CC68;
label8004CCD0:
  gp = lw(sp + 0x0000);
  sp += 4; // 0x0004
  temp = (int32_t)gp < 0;
  t1 = gp >> 20;
  if (temp) goto label8004CB5C;
  t1 = t1 & 0x3F0;
  t1 += s5;
  a3 = lw(t1 + 0x0000);
  t0 = lw(t1 + 0x0004);
  t1 = lw(t1 + 0x0008);
  a2 = gp >> 12;
  a2 = a2 & 0x3F0;
  a2 += s5;
  a0 = lw(a2 + 0x0000);
  a1 = lw(a2 + 0x0004);
  a2 = lw(a2 + 0x0008);
  v1 = gp >> 4;
  v1 = v1 & 0x3F0;
  v1 += s5;
  at = lw(v1 + 0x0000);
  v0 = lw(v1 + 0x0004);
  v1 = lw(v1 + 0x0008);
  t3 = s7 - s6;
  t4 = at - t3;
  t5 = a0 - t3;
  t6 = a3 - t3;
  t4 = t4 & t5;
  t4 = t4 & t6;
  temp = (int32_t)t4 < 0;
  t3 = s7 + s6;
  if (temp) goto label8004CCD0;
  t4 = t3 - at;
  t5 = t3 - a0;
  t6 = t3 - a3;
  t4 = t4 & t5;
  t4 = t4 & t6;
  temp = (int32_t)t4 < 0;
  t3 = t8 - s6;
  if (temp) goto label8004CCD0;
  t4 = v0 - t3;
  t5 = a1 - t3;
  t6 = t0 - t3;
  t4 = t4 & t5;
  t4 = t4 & t6;
  temp = (int32_t)t4 < 0;
  t3 = t8 + s6;
  if (temp) goto label8004CCD0;
  t4 = t3 - v0;
  t5 = t3 - a1;
  t6 = t3 - t0;
  t4 = t4 & t5;
  t4 = t4 & t6;
  temp = (int32_t)t4 < 0;
  t3 = t9 - s6;
  if (temp) goto label8004CCD0;
  t4 = v1 - t3;
  t5 = a2 - t3;
  t6 = t1 - t3;
  t4 = t4 & t5;
  t4 = t4 & t6;
  temp = (int32_t)t4 < 0;
  t3 = t9 + s6;
  if (temp) goto label8004CCD0;
  t4 = t3 - v1;
  t5 = t3 - a2;
  t6 = t3 - t1;
  t4 = t4 & t5;
  t4 = t4 & t6;
  temp = (int32_t)t4 < 0;
  at -= a3;
  if (temp) goto label8004CCD0;
  v0 -= t0;
  v1 -= t1;
  a0 -= a3;
  a1 -= t0;
  a2 -= t1;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = a2;
  cop2.RTM0 = at;
  cop2.RTM2 = v0;
  cop2.RTM4 = v1;
  t2 = s7 - a3;
  t3 = t8 - t0;
  OP(SF_OFF, LM_OFF);
  t4 = t9 - t1;
  t5 = t3 << 16;
  t6 = t2 & 0xFFFF;
  t5 += t6;
  cop2.RTM1 = t4;
  cop2.RTM0 = t5;
  t5 = cop2.MAC1;
  t6 = cop2.MAC2;
  t7 = cop2.MAC3;
  t5 = (int32_t)t5 >> 10;
  t6 = (int32_t)t6 >> 10;
  t7 = (int32_t)t7 >> 10;
  cop2.IR1 = t5;
  cop2.IR2 = t6;
  cop2.IR3 = t7;
  s0 = t3 << 16;
  s1 = t2 & 0xFFFF;
  MVMVA(SF_OFF, MX_RT, V_IR, CV_NONE, LM_OFF);
  s0 += s1;
  s1 = t6 << 16;
  s2 = t5 & 0xFFFF;
  s1 += s2;
  s2 = cop2.MAC1;
  temp = (int32_t)s2 <= 0;
  if (temp) goto label8004CCD0;
  cop2.IR1 = t5;
  cop2.IR2 = t6;
  cop2.IR3 = t7;
  cop2.RTM0 = s0;
  cop2.RTM1 = t4;
  cop2.RTM3 = s1;
  cop2.RTM4 = t7;
  MVMVA(SF_OFF, MX_RT, V_IR, CV_NONE, LM_OFF);
  s0 = cop2.MAC1;
  s1 = cop2.MAC3;
  s0 = s0 << 4;
  s1 = (int32_t)s1 >> 6;
  div_psx(s0,s1);
  cop2.MAC1 = 0;
  cop2.MAC2 = 0;
  cop2.MAC3 = 0;
  cop2.IR1 = t5;
  cop2.IR2 = t6;
  cop2.IR3 = t7;
  s0=lo;
  cop2.IR0 = s0;
  GPL(SF_OFF, LM_OFF);
  s0 = cop2.MAC1;
  s1 = cop2.MAC2;
  s2 = cop2.MAC3;
  s0 = (int32_t)s0 >> 10;
  s1 = (int32_t)s1 >> 10;
  s2 = (int32_t)s2 >> 10;
  cop2.IR1 = s0;
  cop2.IR2 = s1;
  cop2.IR3 = s2;
  mult(s6, s6);
  t2 -= s0;
  SQR(SF_OFF);
  t3 -= s1;
  t4 -= s2;
  s0 = cop2.MAC1;
  s1 = cop2.MAC2;
  s2 = cop2.MAC3;
  s3=lo;
  s3 -= s0;
  s3 -= s1;
  s3 -= s2;
  temp = (int32_t)s3 <= 0;
  if (temp) goto label8004CCD0;
  s0 = t5;
  s1 = 0;
  s2 = s0 + t6;
  s3 = s0 - t6;
  s2 = s2 ^ s3;
  temp = (int32_t)s2 >= 0;
  if (temp) goto label8004CF58;
  s0 = t6;
  s1 = 1; // 0x0001
label8004CF58:
  s2 = s0 + t7;
  s3 = s0 - t7;
  s2 = s2 ^ s3;
  temp = (int32_t)s2 >= 0;
  if (temp) goto label8004CF74;
  s0 = t7;
  s1 = 2; // 0x0002
label8004CF74:
  s1 = s1 << 4;
  s4 = 0;
  cop2.IR1 = at;
  cop2.IR2 = v0;
  cop2.IR3 = v1;
  cop2.RTM0 = t2;
  cop2.RTM2 = t3;
  cop2.RTM4 = t4;
  s2 = 0x8004CFAC; // &0x4812C800
  OP(SF_OFF, LM_OFF);
  s2 += s1;
  temp = s2;
  switch (temp)
  {
  case 0x8004CFAC:
    goto label8004CFAC;
    break;
  case 0x8004CFBC:
    goto label8004CFBC;
    break;
  case 0x8004CFCC:
    goto label8004CFCC;
    break;
  default:
    JR(temp, 0x8004CFA4);
    return;
  }
label8004CFAC:
  s2 = cop2.MAC1;
  goto label8004CFD4;
label8004CFBC:
  s2 = cop2.MAC2;
  goto label8004CFD4;
label8004CFCC:
  s2 = cop2.MAC3;
label8004CFD4:
  s2 = s2 ^ s0;
  s2 = s2 >> 31;
  s4 = s4 | s2;
  cop2.IR1 = t2;
  cop2.IR2 = t3;
  cop2.IR3 = t4;
  cop2.RTM0 = a0;
  cop2.RTM2 = a1;
  cop2.RTM4 = a2;
  s2 = 0x8004D010; // &0x4812C800
  OP(SF_OFF, LM_OFF);
  s2 += s1;
  temp = s2;
  switch (temp)
  {
  case 0x8004D010:
    goto label8004D010;
    break;
  case 0x8004D020:
    goto label8004D020;
    break;
  case 0x8004D030:
    goto label8004D030;
    break;
  default:
    JR(temp, 0x8004D008);
    return;
  }
label8004D010:
  s2 = cop2.MAC1;
  goto label8004D038;
label8004D020:
  s2 = cop2.MAC2;
  goto label8004D038;
label8004D030:
  s2 = cop2.MAC3;
label8004D038:
  s2 = s2 ^ s0;
  s2 = s2 >> 31;
  s2 = s2 << 1;
  s4 = s4 | s2;
  s2 = a0 - at;
  cop2.IR1 = s2;
  s2 = a1 - v0;
  cop2.IR2 = s2;
  s2 = a2 - v1;
  cop2.IR3 = s2;
  s2 = t2 - at;
  cop2.RTM0 = s2;
  s2 = t3 - v0;
  cop2.RTM2 = s2;
  s2 = t4 - v1;
  cop2.RTM4 = s2;
  s2 = 0x8004D090; // &0x4812C800
  OP(SF_OFF, LM_OFF);
  s2 += s1;
  temp = s2;
  switch (temp)
  {
  case 0x8004D090:
    goto label8004D090;
    break;
  case 0x8004D0A0:
    goto label8004D0A0;
    break;
  case 0x8004D0B0:
    goto label8004D0B0;
    break;
  default:
    JR(temp, 0x8004D088);
    return;
  }
label8004D090:
  s2 = cop2.MAC1;
  goto label8004D0B8;
label8004D0A0:
  s2 = cop2.MAC2;
  goto label8004D0B8;
label8004D0B0:
  s2 = cop2.MAC3;
label8004D0B8:
  s2 = s2 ^ s0;
  s2 = s2 >> 31;
  s2 = s2 << 2;
  s4 = s4 | s2;
  s4 = s4 << 3;
  s3 = 0x8004D0E0;
  s4 += s3;
  temp = s4;
  switch (temp)
  {
  case 0x8004D0E0:
    goto label8004D0E0;
    break;
  case 0x8004D0E8:
    goto label8004D0E8;
    break;
  case 0x8004D0F0:
    goto label8004D0F0;
    break;
  case 0x8004D0F8:
    goto label8004D0F8;
    break;
  case 0x8004D100:
    goto label8004D100;
    break;
  case 0x8004D108:
    goto label8004D108;
    break;
  case 0x8004D110:
    goto label8004D110;
    break;
  case 0x8004D118:
    goto label8004D118;
    break;
  default:
    JR(temp, 0x8004D0D8);
    return;
  }
label8004D0E0:
label8004D0E8:
  goto label8004D120;
label8004D0F0:
  goto label8004D130;
label8004D0F8:
  goto label8004D140;
label8004D100:
  goto label8004D178;
label8004D108:
  goto label8004D188;
label8004D110:
  goto label8004D1A8;
label8004D118:
  goto label8004D1C8;
label8004D120:
  at = a0 + a3;
  v0 = a1 + t0;
  v1 = a2 + t1;
  goto label8004D34C;
label8004D130:
  at += a3;
  v0 += t0;
  v1 += t1;
  goto label8004D34C;
label8004D140:
  at = (int32_t)at >> 4;
  v0 = (int32_t)v0 >> 4;
  v1 = (int32_t)v1 >> 4;
  a0 = (int32_t)a0 >> 4;
  a1 = (int32_t)a1 >> 4;
  a2 = (int32_t)a2 >> 4;
  s0 = a0 << 4;
  s1 = a1 << 4;
  s2 = a2 << 4;
  t2 -= s0;
  t3 -= s1;
  t4 -= s2;
  goto label8004D1D8;
label8004D178:
  at = a3;
  v0 = t0;
  v1 = t1;
  goto label8004D34C;
label8004D188:
  at = (int32_t)a0 >> 4;
  v0 = (int32_t)a1 >> 4;
  v1 = (int32_t)a2 >> 4;
  a0 = 0;
  a1 = 0;
  a2 = 0;
  goto label8004D1D8;
label8004D1A8:
  at = (int32_t)at >> 4;
  v0 = (int32_t)v0 >> 4;
  v1 = (int32_t)v1 >> 4;
  a0 = 0;
  a1 = 0;
  a2 = 0;
  goto label8004D1D8;
label8004D1C8:
  at = t2 + a3;
  v0 = t3 + t0;
  v1 = t4 + t1;
  goto label8004D34C;
label8004D1D8:
  s0 = t3 << 16;
  s1 = t2 & 0xFFFF;
  s0 += s1;
  cop2.RTM3 = s0;
  cop2.RTM4 = t4;
  t2 = at - a0;
  t3 = v0 - a1;
  t4 = v1 - a2;
  cop2.IR1 = t2;
  cop2.IR2 = t3;
  cop2.IR3 = t4;
  s0 = t3 << 16;
  s1 = t2 & 0xFFFF;
  s0 += s1;
  cop2.RTM0 = s0;
  cop2.RTM1 = t4;
  MVMVA(SF_OFF, MX_RT, V_IR, CV_NONE, LM_OFF);
  s0 = cop2.MAC3;
  s1 = cop2.MAC1;
  s0 = s0 << 4;
  div_psx(s0,s1);
  at = at << 4;
  v0 = v0 << 4;
  v1 = v1 << 4;
  a0 = a0 << 4;
  a1 = a1 << 4;
  a2 = a2 << 4;
  cop2.MAC1 = 0;
  cop2.MAC2 = 0;
  cop2.MAC3 = 0;
  cop2.IR1 = t2;
  cop2.IR2 = t3;
  cop2.IR3 = t4;
  s0 = at - a0;
  s1 = 0;
  t2 = v0 - a1;
  t3 = s0 + t2;
  t2 = s0 - t2;
  t2 = t2 ^ t3;
  temp = (int32_t)t2 >= 0;
  if (temp) goto label8004D28C;
  s0 = v0 - a1;
  s1 = 8; // 0x0008
label8004D28C:
  t2 = v1 - a2;
  t3 = s0 + t2;
  t2 = s0 - t2;
  t2 = t2 ^ t3;
  temp = (int32_t)t2 >= 0;
  if (temp) goto label8004D2AC;
  s0 = v1 - a2;
  s1 = 16; // 0x0010
label8004D2AC:
  t2=lo;
  cop2.IR0 = t2;
  t3 = 0x8004D2E0; // &0x080134BE
  GPL(SF_OFF, LM_OFF);
  s1 += t3;
  t2 = cop2.MAC1;
  t3 = cop2.MAC2;
  t4 = cop2.MAC3;
  t2 = (int32_t)t2 >> 4;
  t3 = (int32_t)t3 >> 4;
  temp = s1;
  t4 = (int32_t)t4 >> 4;
  switch (temp)
  {
  case 0x8004D2E0:
    goto label8004D2E0;
    break;
  case 0x8004D2E8:
    goto label8004D2E8;
    break;
  case 0x8004D2F0:
    goto label8004D2F0;
    break;
  default:
    JR(temp, 0x8004D2D8);
    return;
  }
label8004D2E0:
  s1 = t2;
  goto label8004D2F8;
label8004D2E8:
  s1 = t3;
  goto label8004D2F8;
label8004D2F0:
  s1 = t4;
  goto label8004D2F8;
label8004D2F8:
  s2 = s0 ^ s1;
  temp = (int32_t)s2 >= 0;
  if (temp) goto label8004D314;
  t2 = 0;
  t3 = 0;
  t4 = 0;
  goto label8004D334;
label8004D314:
  s2 = s0 - s1;
  s3 = s0 + s1;
  s2 = s2 ^ s3;
  temp = (int32_t)s2 >= 0;
  if (temp) goto label8004D334;
  t2 = at - a0;
  t3 = v0 - a1;
  t4 = v1 - a2;
label8004D334:
  t2 += a0;
  t3 += a1;
  t4 += a2;
  at = t2 + a3;
  v0 = t3 + t0;
  v1 = t4 + t1;
label8004D34C:
  a0 = s7 - at;
  a1 = t8 - v0;
  a2 = t9 - v1;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = a2;
  SQR(SF_OFF);
  mult(s6, s6);
  a0 = cop2.MAC1;
  a1 = cop2.MAC2;
  a2 = cop2.MAC3;
  a0 += a1;
  a0 += a2;
  s4=lo;
  s4 -= a0;
  temp = (int32_t)s4 <= 0;
  s4 = lw(s5 - 0x0028); // 0xFFFFFFD8
  if (temp) goto label8004CCD0;
  a1 = lw(s5 - 0x002C); // 0xFFFFFFD4
  sw(s4 + 0x0000, at);
  sw(s4 + 0x0004, v0);
  sw(s4 + 0x0008, v1);
  sw(s4 + 0x000C, a0);
  s4 += 16; // 0x0010
  sw(s5 - 0x0028, s4); // 0xFFFFFFD8
  a1 -= a0;
  temp = (int32_t)a1 <= 0;
  if (temp) goto label8004CCD0;
  sw(s5 - 0x0038, at); // 0xFFFFFFC8
  sw(s5 - 0x0034, v0); // 0xFFFFFFCC
  sw(s5 - 0x0030, v1); // 0xFFFFFFD0
  sw(s5 - 0x002C, a0); // 0xFFFFFFD4
  at = 0x80077368;
  sw(at + 0x0000, t5);
  sw(at + 0x0004, t6);
  sw(at + 0x0008, t7);
  at = 0x800757E4;
  sw(at + 0x0000, fp);
  at = 0x80075808;
  v0 = -1; // 0xFFFFFFFF
  sw(at + 0x0000, v0);
  at = 255; // 0x00FF
  v0 = gp & 0xFF;
  temp = at == v0;
  v0 = lw(fp + 0x0000);
  if (temp) goto label8004D418;
  at = lw(v0 + 0x0000);
label8004D418:
  v1 = 0x80075718;
  sw(v1 + 0x0000, at);
  goto label8004CCD0;
label8004D428:
  s6 = lw(s5 - 0x0024); // 0xFFFFFFDC
  a0 = lw(s5 - 0x002C); // 0xFFFFFFD4
  mult(s6, s6);
  temp = a0 == 0;
  s5 = 0x1F800000;
  if (temp) goto label8004BFB4;
  a1=lo;
  a1 = a0 - a1;
  temp = (int32_t)a1 >= 0;
  at = lw(s5 + 0x0048);
  if (temp) goto label8004BFB4;
  v0 = lw(s5 + 0x004C);
  v1 = lw(s5 + 0x0050);
  cop2.LZCS = a0;
  a2 = 0x80077858;
  a1 = LZCR();
  t0 = -2; // 0xFFFFFFFE
  t0 = a1 & t0;
  a3 = 31; // 0x001F
  a3 -= t0;
  a3 = (int32_t)a3 >> 1;
  t1 = t0 - 24; // 0xFFFFFFE8
  sw(a2 + 0x0000, at);
  sw(a2 + 0x0004, v0);
  temp = (int32_t)t1 < 0;
  sw(a2 + 0x0008, v1);
  if (temp) goto label8004D494;
  t2 = a0 << t1;
  t1 = 24; // 0x0018
  goto label8004D4A0;
label8004D494:
  t1 = 24; // 0x0018
  t1 -= t0;
  t2 = (int32_t)a0 >> t1;
label8004D4A0:
  t2 -= 64; // 0xFFFFFFC0
  t2 = t2 << 1;
  t3 = 0x80074B84; // &0x101F1000
  t3 += t2;
  t3 = lh(t3 + 0x0000);
  t3 = t3 << a3;
  a3 = t3 >> 12;
label8004D4C4:
  a0 = s7 - at;
  a1 = t8 - v0;
  a2 = t9 - v1;
  cop2.MAC1 = 0;
  cop2.MAC2 = 0;
  cop2.MAC3 = 0;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = a2;
  cop2.IR0 = s6;
  GPL(SF_OFF, LM_OFF);
  a0 = cop2.MAC1;
  a1 = cop2.MAC2;
  a2 = cop2.MAC3;
  div_psx(a0,a3);
  a0=lo;
  div_psx(a1,a3);
  a1=lo;
  div_psx(a2,a3);
  a2=lo;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = a2;
  SQR(SF_OFF);
  t0 = cop2.MAC1;
  t1 = cop2.MAC2;
  t2 = cop2.MAC3;
  t0 += t1;
  t0 += t2;
  mult(s6, s6);
  t1=lo;
  t0 -= t1;
  temp = (int32_t)t0 < 0;
  a3--;
  if (temp) goto label8004D4C4;
  at += a0;
  v0 += a1;
  v1 += a2;
  a0 = 0x80076B80;
  sw(a0 + 0x0000, at);
  sw(a0 + 0x0004, v0);
  sw(a0 + 0x0008, v1);
  a0 = 0x80075774;
  a1 = lw(s5 + 0x0058);
  a2 = 0x8006FCF4;
  a1 -= a2;
  a1 = a1 >> 4;
  sw(a0 + 0x0000, a1);
  at = 0x80077DD8;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  v0 = 1; // 0x0001
  return;
}

// size: 0x00000938
void function_8004D5EC(void)
{
  uint32_t temp;
  at = 0x80077DD8;
  sw(at + 0x0000, s0);
  sw(at + 0x0004, s1);
  sw(at + 0x0008, s2);
  sw(at + 0x000C, s3);
  sw(at + 0x0010, s4);
  sw(at + 0x0014, s5);
  sw(at + 0x0018, s6);
  sw(at + 0x001C, s7);
  sw(at + 0x0020, gp);
  sw(at + 0x0024, sp);
  sw(at + 0x0028, fp);
  sw(at + 0x002C, ra);
  t9 = lw(a0 + 0x0008);
  t8 = lw(a0 + 0x0004);
  s7 = lw(a0 + 0x0000);
  at = t9 | t8;
  at = at | s7;
  temp = (int32_t)at < 0;
  v0 = 0;
  if (temp) goto label8004DABC;
  s6 = t9 - a1;
  temp = (int32_t)s6 >= 0;
  s6 = s6 & 0xFFF0;
  if (temp) goto label8004D650;
  s6 = 0;
label8004D650:
  at = 0x1F800000;
  sw(at + 0x0020, s6);
  v0 = 0x80075778;
  v0 = lw(v0 + 0x0000);
  v1 = -1024; // 0xFFFFFC00
  v0 += 4096; // 0x1000
  a0 = s7 >> 13;
  a1 = t8 >> 13;
  a1 = a1 << 5;
  t0 = a0 + a1;
  t1 = t0 << 2;
  t1 += v0;
  t1 = lw(t1 + 0x0000);
  t2 = s7 & 0x1000;
  temp = t1 == 0;
  sw(at + 0x0000, t1);
  if (temp) goto label8004D698;
  at += 4; // 0x0004
label8004D698:
  temp = t2 == 0;
  a0 = -1; // 0xFFFFFFFF
  if (temp) goto label8004D6A4;
  a0 = 1; // 0x0001
label8004D6A4:
  t1 = t0 + a0;
  t2 = t1 & v1;
  temp = (int32_t)t2 > 0;
  t1 = t1 << 2;
  if (temp) goto label8004D6CC;
  t1 += v0;
  t1 = lw(t1 + 0x0000);
  t2 = t8 & 0x1000;
  temp = t1 == 0;
  sw(at + 0x0000, t1);
  if (temp) goto label8004D6CC;
  at += 4; // 0x0004
label8004D6CC:
  temp = t2 == 0;
  a1 = -32; // 0xFFFFFFE0
  if (temp) goto label8004D6D8;
  a1 = 32; // 0x0020
label8004D6D8:
  t1 = t0 + a1;
  t2 = t1 & v1;
  temp = (int32_t)t2 > 0;
  t1 = t1 << 2;
  if (temp) goto label8004D700;
  t1 += v0;
  t1 = lw(t1 + 0x0000);
  temp = t1 == 0;
  sw(at + 0x0000, t1);
  if (temp) goto label8004D700;
  at += 4; // 0x0004
label8004D700:
  t1 = t0 + a0;
  t1 += a1;
  t2 = t1 & v1;
  temp = (int32_t)t2 > 0;
  t1 = t1 << 2;
  if (temp) goto label8004D72C;
  t1 += v0;
  t1 = lw(t1 + 0x0000);
  temp = t1 == 0;
  sw(at + 0x0000, t1);
  if (temp) goto label8004D72C;
  at += 4; // 0x0004
label8004D72C:
  s5 = 0x1F800000;
  temp = at != s5;
  sw(at + 0x0000, 0);
  if (temp) goto label8004DB04;
label8004D738:
  at = 0x800785A8;
  at = lw(at + 0x002C);
  s6 = s6 >> 4;
  s7 = s7 >> 4;
  t8 = t8 >> 4;
  t9 = t9 >> 4;
  s4 = 0x1F800000;
  sw(s4 + 0x0024, s6);
  s5 = lw(at + 0x0010);
  gp = s7 >> 8;
  sp = t8 >> 8;
  fp = t9 >> 8;
  ra = s6 >> 8;
  ra--;
label8004D774:
  v0 = 0x1F800000;
  temp = fp == ra;
  v0 = lw(v0 + 0x0024);
  if (temp) goto label8004DAA4;
  a0 = 0x800785A8;
  temp = v0 != s6;
  a0 = lw(a0 + 0x002C);
  if (temp) goto label8004DAA4;
  at = fp;
  fp--;
  a3 = 0 | 0xFFFF;
  a1 = lw(a0 + 0x0008);
  v0 = at << 1;
  v1 = lhu(a1 + 0x0000);
  a2 = a1 + v0;
  a2 = lhu(a2 + 0x0002);
  v1 -= at;
  temp = a2 == a3;
  a2 += a1;
  if (temp) goto label8004D774;
  temp = (int32_t)v1 <= 0;
  if (temp) goto label8004D774;
  v1 = lhu(a2 + 0x0000);
  temp = (int32_t)sp < 0;
  v0 = sp << 1;
  if (temp) goto label8004D774;
  a2 += v0;
  a2 = lhu(a2 + 0x0002);
  v1 -= sp;
  temp = a2 == a3;
  a2 += a1;
  if (temp) goto label8004D774;
  temp = (int32_t)v1 <= 0;
  if (temp) goto label8004D774;
  v1 = lhu(a2 + 0x0000);
  temp = (int32_t)gp < 0;
  v0 = gp << 1;
  if (temp) goto label8004D774;
  a2 += v0;
  a2 = lhu(a2 + 0x0002);
  v1 -= gp;
  temp = (int32_t)v1 <= 0;
  s4 = lw(a0 + 0x000C);
  if (temp) goto label8004D774;
  temp = a2 == a3;
  a2 = a2 << 1;
  if (temp) goto label8004D774;
  s4 += a2;
  s3 = lhu(s4 + 0x0000);
  s3 = s3 & 0x7FFF;
  goto label8004D828;
label8004D824:
  s3 = lh(s4 + 0x0000);
label8004D828:
  s4 += 2; // 0x0002
  temp = (int32_t)s3 < 0;
  at = s3 << 2;
  if (temp) goto label8004D774;
  v0 = s3 << 3;
  at += v0;
  t2 = at + s5;
  a3 = lw(t2 + 0x0000);
  at = (int32_t)a3 >> 23;
  a0 = a3 << 9;
  a0 = (int32_t)a0 >> 23;
  a3 = a3 & 0x3FFF;
  temp = a3 == s7;
  t3 = a3 - s7;
  if (temp) goto label8004D890;
  temp = (int32_t)t3 < 0;
  t4 = t3 + at;
  if (temp) goto label8004D880;
  temp = (int32_t)t4 <= 0;
  t4 = t3 + a0;
  if (temp) goto label8004D890;
  temp = (int32_t)t4 > 0;
  if (temp) goto label8004D824;
  goto label8004D890;
label8004D880:
  temp = (int32_t)t4 >= 0;
  t4 = t3 + a0;
  if (temp) goto label8004D890;
  temp = (int32_t)t4 < 0;
  if (temp) goto label8004D824;
label8004D890:
  t0 = lw(t2 + 0x0004);
  v0 = (int32_t)t0 >> 23;
  a1 = t0 << 9;
  a1 = (int32_t)a1 >> 23;
  t0 = t0 & 0x3FFF;
  temp = t0 == t8;
  t3 = t0 - t8;
  if (temp) goto label8004D8E0;
  temp = (int32_t)t3 < 0;
  t4 = t3 + v0;
  if (temp) goto label8004D8D0;
  temp = (int32_t)t4 <= 0;
  t4 = t3 + a1;
  if (temp) goto label8004D8E0;
  temp = (int32_t)t4 > 0;
  if (temp) goto label8004D824;
  goto label8004D8E0;
label8004D8D0:
  temp = (int32_t)t4 >= 0;
  t4 = t3 + a1;
  if (temp) goto label8004D8E0;
  temp = (int32_t)t4 < 0;
  if (temp) goto label8004D824;
label8004D8E0:
  t2 = lw(t2 + 0x0008);
  t1 = t2 & 0x3FFF;
  t3 = t1 - t9;
  temp = (int32_t)t3 > 0;
  v1 = t2 >> 24;
  if (temp) goto label8004D824;
  a2 = t2 >> 16;
  a2 = a2 & 0xFF;
  t3 = s6 - t1;
  t4 = t3 - v1;
  t3 -= a2;
  t3 = t3 | t4;
  temp = (int32_t)t3 > 0;
  t2 = t2 & 0x4000;
  if (temp) goto label8004D824;
  temp = (int32_t)t2 > 0;
  if (temp) goto label8004D824;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = a2;
  cop2.RTM0 = at;
  cop2.RTM2 = v0;
  cop2.RTM4 = v1;
  t3 = s7 - a3;
  t4 = t8 - t0;
  OP(SF_OFF, LM_OFF);
  t7 = cop2.MAC3;
  t6 = cop2.MAC2;
  t5 = cop2.MAC1;
  temp = (int32_t)t7 <= 0;
  t5 = (int32_t)t5 >> 2;
  if (temp) goto label8004D824;
  t6 = (int32_t)t6 >> 2;
  t7 = (int32_t)t7 >> 2;
  cop2.IR1 = t3;
  cop2.IR2 = t4;
  cop2.RTM0 = at;
  cop2.RTM2 = v0;
  OP(SF_OFF, LM_OFF);
  t2 = cop2.MAC3;
  temp = (int32_t)t2 < 0;
  if (temp) goto label8004D824;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.RTM0 = t3;
  cop2.RTM2 = t4;
  OP(SF_OFF, LM_OFF);
  t2 = cop2.MAC3;
  temp = (int32_t)t2 < 0;
  if (temp) goto label8004D824;
  s0 = t3 - at;
  s1 = t4 - v0;
  cop2.IR1 = s0;
  cop2.IR2 = s1;
  s0 = a0 - at;
  s1 = a1 - v0;
  cop2.RTM0 = s0;
  cop2.RTM2 = s1;
  OP(SF_OFF, LM_OFF);
  t2 = cop2.MAC3;
  temp = (int32_t)t2 < 0;
  if (temp) goto label8004D824;
  mult(t3, t5);
  s0=lo;
  s0 = -s0;
  mult(t4, t6);
  s1=lo;
  s0 -= s1;
  s0 = s0 << 4;
  div_psx(s0,t7);
  s0=lo;
  s0 = (int32_t)s0 >> 4;
  t1 += s0;
  s0 = t9 - t1;
  temp = (int32_t)s0 <= 0;
  s0 = s6 - t1;
  if (temp) goto label8004D824;
  temp = (int32_t)s0 >= 0;
  if (temp) goto label8004D824;
  s6 = t1;
  at = 0x80077368;
  sw(at + 0x0000, t5);
  sw(at + 0x0004, t6);
  sw(at + 0x0008, t7);
  at = 0x800757E4;
  sw(at + 0x0000, 0);
  at = 0x80075808;
  sw(at + 0x0000, s3);
  at = 0x800785A8;
  at = lw(at + 0x002C);
  v0 = lw(at + 0x0004);
  at = lw(at + 0x0018);
  v0 -= s3;
  temp = (int32_t)v0 <= 0;
  v0 = 255; // 0x00FF
  if (temp) goto label8004DA94;
  at += s3;
  v0 = lbu(at + 0x0000);
label8004DA94:
  a0 = 0x80075718;
  sw(a0 + 0x0000, v0);
  goto label8004D824;
label8004DAA4:
  at = 0x1F800000;
  at = lw(at + 0x0020);
  s6 = s6 << 4;
  temp = at == s6;
  v0 = 0;
  if (temp) goto label8004DABC;
  v0 = s6;
label8004DABC:
  v1 = 0x80076B80;
  at = 0x80077DD8;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  sw(v1 + 0x0008, v0);
  return;
label8004DB04:
  ra = 0x1F800000;
  s5 += 128; // 0x0080
label8004DB0C:
  at = lw(ra + 0x0000);
  fp = ra - 4; // 0xFFFFFFFC
  temp = at == 0;
  ra += 4; // 0x0004
  if (temp) goto label8004DF1C;
label8004DB1C:
  fp = lw(fp + 0x0004);
  temp = fp == 0;
  at = lw(fp + 0x000C);
  if (temp) goto label8004DB0C;
  v0 = lw(fp + 0x0010);
  v1 = lw(fp + 0x0014);
  a0 = at - s7;
  a1 = v0 - t8;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = 0;
  sp = lw(fp + 0x0008);
  a0 = 0x80000000;
  SQR(SF_OFF);
  temp = sp == 0;
  sp = sp | a0;
  if (temp) goto label8004DB1C;
  a0 = lhu(sp + 0x0000);
  sp += 4; // 0x0004
  mult(a0, a0);
  a2 = cop2.MAC1;
  a3 = cop2.MAC2;
  t1 = a2 + a3;
  a1=lo;
  a1 = t1 - a1;
  temp = (int32_t)a1 >= 0;
  a1 = t9 + a0;
  if (temp) goto label8004DB1C;
  a1 = v1 - a1;
  temp = (int32_t)a1 > 0;
  a1 = s6 - a0;
  if (temp) goto label8004DB1C;
  a1 = v1 - a1;
  temp = (int32_t)a1 < 0;
  if (temp) goto label8004DB1C;
  t2 = lw(fp + 0x0020);
  t3 = lw(fp + 0x0024);
  t4 = lw(fp + 0x0028);
  t5 = lw(fp + 0x002C);
  t6 = lw(fp + 0x0030);
  cop2.RTM0 = t2;
  cop2.RTM1 = t3;
  cop2.RTM2 = t4;
  cop2.RTM3 = t5;
  cop2.RTM4 = t6;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  a1 = lhu(fp + 0x0036);
  a2 = 0x80076378;
  a1 = a1 << 2;
  a1 += a2;
  a2 = lbu(fp + 0x003C);
  a1 = lw(a1 + 0x0000);
  a2 = a2 << 2;
  a1 += a2;
  a2 = lbu(fp + 0x003E);
  t0 = lw(a1 + 0x0038);
  a2 = a2 << 3;
  a3 = t0 + a2;
  a1 = lw(a3 + 0x0024);
  a2 = lbu(t0 + 0x0005);
  a0 = lhu(sp - 0x0002); // 0xFFFFFFFE
  a1 = a1 << 11;
  a1 = a1 >> 11;
  a3 = 0x80000000;
  a1 = a1 | a3;
  a0 = a0 << 2;
  a0 += a1;
  a0 += 4; // 0x0004
  a3 = s5 - 16; // 0xFFFFFFF0
label8004DC34:
  RTPS();
  t0 = lw(a1 + 0x0000);
  a1 += 4; // 0x0004
  t2 = t0 << 20;
  t2 = (int32_t)t2 >> 19;
  t1 = t0 << 10;
  t1 = (int32_t)t1 >> 21;
  t0 = (int32_t)t0 >> 21;
  t0 = t0 << a2;
  t1 = t1 << a2;
  t2 = t2 << a2;
  t2 = t2 << 16;
  t1 += t2;
  cop2.VZ0 = t0;
  cop2.VXY0 = t1;
  t0 = cop2.IR3;
  t1 = cop2.IR1;
  t2 = cop2.IR2;
  t0 += at;
  t1 = v0 - t1;
  t2 = v1 - t2;
  sw(a3 + 0x0000, t0);
  sw(a3 + 0x0004, t1);
  sw(a3 + 0x0008, t2);
  temp = a1 != a0;
  a3 += 16; // 0x0010
  if (temp) goto label8004DC34;
label8004DC9C:
  gp = lw(sp + 0x0000);
  sp += 4; // 0x0004
  temp = (int32_t)gp < 0;
  t1 = gp >> 20;
  if (temp) goto label8004DB1C;
  t1 = t1 & 0x3F0;
  t1 += s5;
  a3 = lw(t1 + 0x0000);
  t0 = lw(t1 + 0x0004);
  t1 = lw(t1 + 0x0008);
  a2 = gp >> 12;
  a2 = a2 & 0x3F0;
  a2 += s5;
  a0 = lw(a2 + 0x0000);
  a1 = lw(a2 + 0x0004);
  a2 = lw(a2 + 0x0008);
  v1 = gp >> 4;
  v1 = v1 & 0x3F0;
  v1 += s5;
  at = lw(v1 + 0x0000);
  v0 = lw(v1 + 0x0004);
  v1 = lw(v1 + 0x0008);
  temp = a3 == s7;
  t3 = a3 - s7;
  if (temp) goto label8004DD28;
  temp = (int32_t)t3 < 0;
  t3 = at - s7;
  if (temp) goto label8004DD18;
  temp = (int32_t)t3 <= 0;
  t3 = a0 - s7;
  if (temp) goto label8004DD28;
  temp = (int32_t)t3 > 0;
  if (temp) goto label8004DC9C;
  goto label8004DD28;
label8004DD18:
  temp = (int32_t)t3 >= 0;
  t3 = a0 - s7;
  if (temp) goto label8004DD28;
  temp = (int32_t)t3 < 0;
  if (temp) goto label8004DC9C;
label8004DD28:
  temp = t0 == t8;
  t3 = t0 - t8;
  if (temp) goto label8004DD60;
  temp = (int32_t)t3 < 0;
  t3 = v0 - t8;
  if (temp) goto label8004DD50;
  temp = (int32_t)t3 <= 0;
  t3 = a1 - t8;
  if (temp) goto label8004DD60;
  temp = (int32_t)t3 > 0;
  if (temp) goto label8004DC9C;
  goto label8004DD60;
label8004DD50:
  temp = (int32_t)t3 >= 0;
  t3 = a1 - t8;
  if (temp) goto label8004DD60;
  temp = (int32_t)t3 < 0;
  if (temp) goto label8004DC9C;
label8004DD60:
  t5 = v1 - s6;
  t6 = a2 - s6;
  t7 = t1 - s6;
  t5 = t5 & t6;
  t5 = t5 & t7;
  temp = (int32_t)t5 < 0;
  t5 = t9 - v1;
  if (temp) goto label8004DC9C;
  t6 = t9 - a2;
  t7 = t9 - t1;
  t5 = t5 & t6;
  t5 = t5 & t7;
  temp = (int32_t)t5 < 0;
  at -= a3;
  if (temp) goto label8004DC9C;
  v0 -= t0;
  v1 -= t1;
  a0 -= a3;
  a1 -= t0;
  a2 -= t1;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = a2;
  cop2.RTM0 = at;
  cop2.RTM2 = v0;
  cop2.RTM4 = v1;
  t3 = s7 - a3;
  t4 = t8 - t0;
  OP(SF_OFF, LM_OFF);
  t7 = cop2.MAC3;
  t6 = cop2.MAC2;
  t5 = cop2.MAC1;
  temp = (int32_t)t7 <= 0;
  t5 = (int32_t)t5 >> 10;
  if (temp) goto label8004DC9C;
  t6 = (int32_t)t6 >> 10;
  t7 = (int32_t)t7 >> 10;
  cop2.IR1 = t3;
  cop2.IR2 = t4;
  cop2.RTM0 = at;
  cop2.RTM2 = v0;
  OP(SF_OFF, LM_OFF);
  t2 = cop2.MAC3;
  temp = (int32_t)t2 < 0;
  if (temp) goto label8004DC9C;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.RTM0 = t3;
  cop2.RTM2 = t4;
  OP(SF_OFF, LM_OFF);
  t2 = cop2.MAC3;
  temp = (int32_t)t2 < 0;
  if (temp) goto label8004DC9C;
  s0 = t3 - at;
  s1 = t4 - v0;
  cop2.IR1 = s0;
  cop2.IR2 = s1;
  s0 = a0 - at;
  s1 = a1 - v0;
  cop2.RTM0 = s0;
  cop2.RTM2 = s1;
  OP(SF_OFF, LM_OFF);
  t2 = cop2.MAC3;
  temp = (int32_t)t2 < 0;
  if (temp) goto label8004DC9C;
  mult(t3, t5);
  s0=lo;
  s0 = -s0;
  mult(t4, t6);
  s1=lo;
  s0 -= s1;
  s0 = s0 << 4;
  div_psx(s0,t7);
  s0=lo;
  s0 = (int32_t)s0 >> 4;
  t1 += s0;
  s0 = t9 - t1;
  temp = (int32_t)s0 <= 0;
  s0 = s6 - t1;
  if (temp) goto label8004DC9C;
  temp = (int32_t)s0 >= 0;
  if (temp) goto label8004DC9C;
  s6 = t1;
  at = 0x80077368;
  sw(at + 0x0000, t5);
  sw(at + 0x0004, t6);
  sw(at + 0x0008, t7);
  at = 0x800757E4;
  sw(at + 0x0000, fp);
  at = 0x80075808;
  v0 = -1; // 0xFFFFFFFF
  sw(at + 0x0000, v0);
  at = 255; // 0x00FF
  v0 = gp & 0xFF;
  temp = at == v0;
  v0 = lw(fp + 0x0000);
  if (temp) goto label8004DF0C;
  at = lw(v0 + 0x0000);
label8004DF0C:
  v1 = 0x80075718;
  sw(v1 + 0x0000, at);
  goto label8004DC9C;
label8004DF1C:
  goto label8004D738;
}

// size: 0x000003C4
void function_8004DF24(void)
{
  uint32_t temp;
  at = 0x80077DD8;
  sw(at + 0x0000, s0);
  sw(at + 0x0004, s1);
  sw(at + 0x0008, s2);
  sw(at + 0x000C, s3);
  sw(at + 0x0010, s4);
  sw(at + 0x0014, s5);
  sw(at + 0x0018, s6);
  sw(at + 0x001C, s7);
  sw(at + 0x0020, gp);
  sw(at + 0x0024, sp);
  sw(at + 0x0028, fp);
  sw(at + 0x002C, ra);
  t9 = lw(a0 + 0x0008);
  t8 = lw(a0 + 0x0004);
  s7 = lw(a0 + 0x0000);
  at = t9 | t8;
  at = at | s7;
  temp = (int32_t)at < 0;
  s6 = 0;
  if (temp) goto label8004E28C;
  t9 = t9 >> 4;
  t8 = t8 >> 4;
  s7 = s7 >> 4;
  at = 0x800785A8;
  at = lw(at + 0x002C);
  gp = s7 >> 8;
  sp = t8 >> 8;
  fp = t9 >> 8;
  ra = -1; // 0xFFFFFFFF
  s5 = lw(at + 0x0010);
label8004DFA4:
  a0 = 0x800785A8;
  temp = (int32_t)s6 > 0;
  a0 = lw(a0 + 0x002C);
  if (temp) goto label8004E28C;
  temp = fp == ra;
  at = fp;
  if (temp) goto label8004E28C;
  fp--;
  a3 = 0 | 0xFFFF;
  a1 = lw(a0 + 0x0008);
  v0 = at << 1;
  v1 = lhu(a1 + 0x0000);
  a2 = a1 + v0;
  a2 = lhu(a2 + 0x0002);
  v1 -= at;
  temp = a2 == a3;
  a2 += a1;
  if (temp) goto label8004DFA4;
  temp = (int32_t)v1 <= 0;
  if (temp) goto label8004DFA4;
  v1 = lhu(a2 + 0x0000);
  temp = (int32_t)sp < 0;
  v0 = sp << 1;
  if (temp) goto label8004DFA4;
  a2 += v0;
  a2 = lhu(a2 + 0x0002);
  v1 -= sp;
  temp = a2 == a3;
  a2 += a1;
  if (temp) goto label8004DFA4;
  temp = (int32_t)v1 <= 0;
  if (temp) goto label8004DFA4;
  v1 = lhu(a2 + 0x0000);
  temp = (int32_t)gp < 0;
  v0 = gp << 1;
  if (temp) goto label8004DFA4;
  a2 += v0;
  a2 = lhu(a2 + 0x0002);
  v1 -= gp;
  temp = (int32_t)v1 <= 0;
  s4 = lw(a0 + 0x000C);
  if (temp) goto label8004DFA4;
  temp = a2 == a3;
  a2 = a2 << 1;
  if (temp) goto label8004DFA4;
  s4 += a2;
  s3 = lhu(s4 + 0x0000);
  s3 = s3 & 0x7FFF;
  goto label8004E050;
label8004E04C:
  s3 = lh(s4 + 0x0000);
label8004E050:
  s4 += 2; // 0x0002
  temp = (int32_t)s3 < 0;
  at = s3 << 2;
  if (temp) goto label8004DFA4;
  v0 = s3 << 3;
  at += v0;
  t2 = at + s5;
  a3 = lw(t2 + 0x0000);
  at = (int32_t)a3 >> 23;
  a0 = a3 << 9;
  a0 = (int32_t)a0 >> 23;
  a3 = a3 & 0x3FFF;
  temp = a3 == s7;
  t3 = a3 - s7;
  if (temp) goto label8004E0B8;
  temp = (int32_t)t3 < 0;
  t4 = t3 + at;
  if (temp) goto label8004E0A8;
  temp = (int32_t)t4 <= 0;
  t4 = t3 + a0;
  if (temp) goto label8004E0B8;
  temp = (int32_t)t4 > 0;
  if (temp) goto label8004E04C;
  goto label8004E0B8;
label8004E0A8:
  temp = (int32_t)t4 >= 0;
  t4 = t3 + a0;
  if (temp) goto label8004E0B8;
  temp = (int32_t)t4 < 0;
  if (temp) goto label8004E04C;
label8004E0B8:
  t0 = lw(t2 + 0x0004);
  v0 = (int32_t)t0 >> 23;
  a1 = t0 << 9;
  a1 = (int32_t)a1 >> 23;
  t0 = t0 & 0x3FFF;
  temp = t0 == t8;
  t3 = t0 - t8;
  if (temp) goto label8004E108;
  temp = (int32_t)t3 < 0;
  t4 = t3 + v0;
  if (temp) goto label8004E0F8;
  temp = (int32_t)t4 <= 0;
  t4 = t3 + a1;
  if (temp) goto label8004E108;
  temp = (int32_t)t4 > 0;
  if (temp) goto label8004E04C;
  goto label8004E108;
label8004E0F8:
  temp = (int32_t)t4 >= 0;
  t4 = t3 + a1;
  if (temp) goto label8004E108;
  temp = (int32_t)t4 < 0;
  if (temp) goto label8004E04C;
label8004E108:
  t2 = lw(t2 + 0x0008);
  t1 = t2 & 0x3FFF;
  t3 = t1 - t9;
  temp = (int32_t)t3 > 0;
  v1 = t2 >> 24;
  if (temp) goto label8004E04C;
  a2 = t2 >> 16;
  a2 = a2 & 0xFF;
  t3 = s6 - t1;
  t4 = t3 - v1;
  t3 -= a2;
  t3 = t3 | t4;
  temp = (int32_t)t3 > 0;
  if (temp) goto label8004E04C;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.IR3 = a2;
  cop2.RTM0 = at;
  cop2.RTM2 = v0;
  cop2.RTM4 = v1;
  t3 = s7 - a3;
  t4 = t8 - t0;
  OP(SF_OFF, LM_OFF);
  t7 = cop2.MAC3;
  t6 = cop2.MAC2;
  t5 = cop2.MAC1;
  temp = (int32_t)t7 <= 0;
  t5 = (int32_t)t5 >> 2;
  if (temp) goto label8004E04C;
  t6 = (int32_t)t6 >> 2;
  t7 = (int32_t)t7 >> 2;
  cop2.IR1 = t3;
  cop2.IR2 = t4;
  cop2.RTM0 = at;
  cop2.RTM2 = v0;
  OP(SF_OFF, LM_OFF);
  t2 = cop2.MAC3;
  temp = (int32_t)t2 < 0;
  if (temp) goto label8004E04C;
  cop2.IR1 = a0;
  cop2.IR2 = a1;
  cop2.RTM0 = t3;
  cop2.RTM2 = t4;
  OP(SF_OFF, LM_OFF);
  t2 = cop2.MAC3;
  temp = (int32_t)t2 < 0;
  if (temp) goto label8004E04C;
  s0 = t3 - at;
  s1 = t4 - v0;
  cop2.IR1 = s0;
  cop2.IR2 = s1;
  s0 = a0 - at;
  s1 = a1 - v0;
  cop2.RTM0 = s0;
  cop2.RTM2 = s1;
  OP(SF_OFF, LM_OFF);
  t2 = cop2.MAC3;
  temp = (int32_t)t2 < 0;
  if (temp) goto label8004E04C;
  mult(t3, t5);
  s0=lo;
  s0 = -s0;
  mult(t4, t6);
  s1=lo;
  s0 -= s1;
  s0 = s0 << 4;
  div_psx(s0,t7);
  s0=lo;
  s0 = (int32_t)s0 >> 4;
  t1 += s0;
  s0 = t9 - t1;
  temp = (int32_t)s0 <= 0;
  s0 = s6 - t1;
  if (temp) goto label8004E04C;
  temp = (int32_t)s0 >= 0;
  if (temp) goto label8004E04C;
  s6 = t1;
  at = 0x800785A8;
  at = lw(at + 0x002C);
  at = lw(at + 0x0014);
  at += s3;
  at = lbu(at + 0x0000);
  v0 = 0x80075844;
  sw(v0 + 0x0000, at);
  goto label8004E04C;
label8004E28C:
  temp = (int32_t)s6 > 0;
  v0 = s6 << 4;
  if (temp) goto label8004E2A8;
  v0 = -1; // 0xFFFFFFFF
  at = 0x80075844;
  sw(at + 0x0000, v0);
  v0 = 0;
label8004E2A8:
  at = 0x80077DD8;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  return;
}

// size: 0x000007E0
void function_8004E3C8(void)
{
  uint32_t temp;
  at = 0x80077DD8;
  sw(at + 0x0000, s0);
  sw(at + 0x0004, s1);
  sw(at + 0x0008, s2);
  sw(at + 0x000C, s3);
  sw(at + 0x0010, s4);
  sw(at + 0x0014, s5);
  sw(at + 0x0018, s6);
  sw(at + 0x001C, s7);
  sw(at + 0x0020, gp);
  sw(at + 0x0024, sp);
  sw(at + 0x0028, fp);
  sw(at + 0x002C, ra);
  s1 = 0;
  s2 = 0;
  s3 = lw(sp + 0x0014);
  s4 = a2;
  s5 = a3;
  s6 = a1;
  s7 = lw(a0 + 0x0000);
  t8 = lw(a0 + 0x0004);
  t9 = lw(a0 + 0x0008);
  gp = a0;
  sp = lw(sp + 0x0010);
  at = s7 | t8;
  at = at | t9;
  temp = (int32_t)at < 0;
  v0 = 0;
  if (temp) goto label8004EB68;
  at = 0x80075778;
  at = lw(at + 0x0000);
  v1 = s7 >> 13;
  a0 = t8 >> 13;
  a0 = a0 << 5;
  a3 = 32; // 0x0020
  a1 = s7 & 0x1000;
  temp = a1 == 0;
  a1 = v1 - 1; // 0xFFFFFFFF
  if (temp) goto label8004E474;
  a1 = v1 + 1; // 0x0001
  temp = a1 != a3;
  if (temp) goto label8004E474;
  a1 = -1; // 0xFFFFFFFF
label8004E474:
  a2 = t8 & 0x1000;
  temp = a2 == 0;
  a2 = a0 - a3;
  if (temp) goto label8004E494;
  a2 = a0 + a3;
  a3 = 1024; // 0x0400
  temp = a2 != a3;
  if (temp) goto label8004E494;
  a2 = -1; // 0xFFFFFFFF
label8004E494:
  v0 = 0x1F800000;
  a3 = v1 + a0;
  a3 = a3 << 2;
  a3 += at;
  sw(v0 + 0x0000, a3);
  v0 += 4; // 0x0004
  temp = (int32_t)a1 < 0;
  a3 = a1 + a0;
  if (temp) goto label8004E4C4;
  a3 = a3 << 2;
  a3 += at;
  sw(v0 + 0x0000, a3);
  v0 += 4; // 0x0004
label8004E4C4:
  temp = (int32_t)a2 < 0;
  a3 = v1 + a2;
  if (temp) goto label8004E4DC;
  a3 = a3 << 2;
  a3 += at;
  sw(v0 + 0x0000, a3);
  v0 += 4; // 0x0004
label8004E4DC:
  temp = (int32_t)a1 < 0;
  if (temp) goto label8004E4FC;
  temp = (int32_t)a2 < 0;
  a3 = a1 + a2;
  if (temp) goto label8004E4FC;
  a3 = a3 << 2;
  a3 += at;
  sw(v0 + 0x0000, a3);
  v0 += 4; // 0x0004
label8004E4FC:
  sw(v0 + 0x0000, 0);
  ra = 0x1F800000;
label8004E504:
  fp = lw(ra + 0x0000);
  ra += 4; // 0x0004
  temp = fp == 0;
  fp -= 4; // 0xFFFFFFFC
  if (temp) goto label8004EB58;
label8004E514:
  fp = lw(fp + 0x0004);
  temp = fp == 0;
  v1 = lw(fp + 0x000C);
  if (temp) goto label8004E504;
  temp = fp == sp;
  a0 = lw(fp + 0x0010);
  if (temp) goto label8004E514;
  a1 = lw(fp + 0x0014);
  v1 -= s7;
  a0 -= t8;
  a1 -= t9;
  cop2.IR1 = v1;
  cop2.IR2 = a0;
  cop2.IR3 = a1;
  at = lw(fp + 0x0008);
  SQR(SF_OFF);
  temp = at == 0;
  v0 = lhu(at + 0x0000);
  if (temp) goto label8004E514;
  at += 4; // 0x0004
  v0 += s6;
  mult(v0, v0);
  a2 = cop2.MAC1;
  a3 = cop2.MAC2;
  t0 = cop2.MAC3;
  t1 = a2 + a3;
  t1 += t0;
  v0=lo;
  v0 = t1 - v0;
  temp = (int32_t)v0 >= 0;
  if (temp) goto label8004E514;
  v0 = 0;
label8004E590:
  v0 = v0 & 0x1;
  temp = v0 != 0;
  v0 = lw(at + 0x0000);
  if (temp) goto label8004E514;
  t3 = 0x8004E5B8; // &0x08013976
  t2 = v0 & 0xFF00;
  t2 = t2 >> 5;
  t2 += t3;
  temp = t2;
  at += 4; // 0x0004
  switch (temp)
  {
  case 0x8004E5B8:
    goto label8004E5B8;
    break;
  case 0x8004E5C0:
    goto label8004E5C0;
    break;
  case 0x8004E5C8:
    goto label8004E5C8;
    break;
  case 0x8004E5D0:
    goto label8004E5D0;
    break;
  default:
    JR(temp, 0x8004E5B0);
    return;
  }
label8004E5B8:
  goto label8004E5D8;
label8004E5C0:
  goto label8004E668;
label8004E5C8:
  goto label8004E718;
label8004E5D0:
  goto label8004E830;
label8004E5D8:
  t2 = v0 >> 16;
  t2 += s6;
  mult(t2, t2);
  t2=lo;
  t3 = t1 - t2;
  temp = (int32_t)t3 >= 0;
  at += 4; // 0x0004
  if (temp) goto label8004E590;
  s2 = fp;
  t7 = v0 & 0x4;
  temp = t7 != 0;
  if (temp) goto label8004E614;
  t7 = lw(fp + 0x0018);
  t7 = t7 | s5;
  sw(fp + 0x0018, t7);
label8004E614:
  temp = s4 == 0;
  if (temp) goto label8004E630;
  t7 = lw(s4 + 0x0000);
  s0 = lw(at - 0x0004); // 0xFFFFFFFC
  t7 = t7 | s0;
  sw(s4 + 0x0000, t7);
label8004E630:
  t7 = v0 & 0x2;
  temp = t7 != 0;
  if (temp) goto label8004E590;
  t7 = s3 & 0x1;
  temp = t7 == 0;
  if (temp) goto label8004E654;
  v0 = t2;
  at = t1;
  goto label8004E9D8;
label8004E654:
  t7 = s3 & 0x2;
  temp = t7 != 0;
  if (temp) goto label8004E590;
  goto label8004EB58;
label8004E668:
  t2 = v0 >> 16;
  t2 += s6;
  mult(t2, t2);
  t3 = lw(at + 0x0000);
  t2=lo;
  t3 = (int32_t)t3 >> 16;
  t3 += a1;
  mult(t3, t3);
  t4=lo;
  t4 += a2;
  t4 += a3;
  t5 = t4 - t2;
  temp = (int32_t)t5 >= 0;
  at += 8; // 0x0008
  if (temp) goto label8004E590;
  s2 = fp;
  t7 = v0 & 0x4;
  temp = t7 != 0;
  if (temp) goto label8004E6C0;
  t7 = lw(fp + 0x0018);
  t7 = t7 | s5;
  sw(fp + 0x0018, t7);
label8004E6C0:
  temp = s4 == 0;
  if (temp) goto label8004E6DC;
  t7 = lw(s4 + 0x0000);
  s0 = lw(at - 0x0004); // 0xFFFFFFFC
  t7 = t7 | s0;
  sw(s4 + 0x0000, t7);
label8004E6DC:
  t7 = v0 & 0x2;
  temp = t7 != 0;
  if (temp) goto label8004E590;
  t7 = s3 & 0x1;
  temp = t7 == 0;
  if (temp) goto label8004E704;
  at = t4;
  v0 = t2;
  a1 = t3;
  goto label8004E9D8;
label8004E704:
  t7 = s3 & 0x2;
  temp = t7 != 0;
  if (temp) goto label8004E590;
  goto label8004EB58;
label8004E718:
  t2 = v0 >> 16;
  t2 += s6;
  mult(t2, t2);
  t4 = lw(at + 0x0000);
  at += 16; // 0x0010
  t2=lo;
  t3 = t2 - a2;
  t3 -= a3;
  temp = (int32_t)t3 <= 0;
  t3 = (int32_t)t4 >> 16;
  if (temp) goto label8004E590;
  t3 += a1;
  t4 = t4 << 16;
  t4 = (int32_t)t4 >> 16;
  t4 += a1;
  t5 = -t4;
  t5 = t3 | t5;
  temp = (int32_t)t5 >= 0;
  if (temp) goto label8004E7B0;
  temp = (int32_t)t3 < 0;
  t3 = 6; // 0x0006
  if (temp) goto label8004E76C;
  t3 = 4; // 0x0004
label8004E76C:
  t3 += at;
  t3 = lh(t3 - 0x0010); // 0xFFFFFFF0
  t3 += a1;
  mult(t3, t3);
  t2 = lw(at - 0x0008); // 0xFFFFFFF8
  t4=lo;
  t4 += a2;
  t4 += a3;
  t2 += s6;
  mult(t2, t2);
  t2=lo;
  t5 = t4 - t2;
  temp = (int32_t)t5 >= 0;
  if (temp) goto label8004E590;
  s2 = fp;
  goto label8004E7BC;
label8004E7B0:
  s2 = fp;
  t3 = 0;
  t4 = a2 + a3;
label8004E7BC:
  t7 = v0 & 0x4;
  temp = t7 != 0;
  if (temp) goto label8004E7D8;
  t7 = lw(fp + 0x0018);
  t7 = t7 | s5;
  sw(fp + 0x0018, t7);
label8004E7D8:
  temp = s4 == 0;
  if (temp) goto label8004E7F4;
  t7 = lw(s4 + 0x0000);
  s0 = lw(at - 0x0004); // 0xFFFFFFFC
  t7 = t7 | s0;
  sw(s4 + 0x0000, t7);
label8004E7F4:
  t7 = v0 & 0x2;
  temp = t7 != 0;
  if (temp) goto label8004E590;
  t7 = s3 & 0x1;
  temp = t7 == 0;
  if (temp) goto label8004E81C;
  at = t4;
  v0 = t2;
  a1 = t3;
  goto label8004E9D8;
label8004E81C:
  t7 = s3 & 0x2;
  temp = t7 != 0;
  if (temp) goto label8004E590;
  goto label8004EB58;
label8004E830:
  t2 = lw(fp + 0x0020);
  t3 = lw(fp + 0x0024);
  t4 = lw(fp + 0x0028);
  t5 = lw(fp + 0x002C);
  t6 = lw(fp + 0x0030);
  cop2.RTM0 = t2;
  cop2.RTM1 = t3;
  cop2.RTM2 = t4;
  cop2.RTM3 = t5;
  cop2.RTM4 = t6;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  t3 = lhu(fp + 0x0036);
  t4 = 0x80076378;
  t3 = t3 << 2;
  t3 += t4;
  t4 = lbu(fp + 0x003C);
  t3 = lw(t3 + 0x0000);
  t4 = t4 << 2;
  t3 += t4;
  t4 = lbu(fp + 0x003E);
  t3 = lw(t3 + 0x0038);
  t4 = t4 << 3;
  t4 += t3;
  t5 = lw(t4 + 0x0024);
  t6 = lbu(t3 + 0x0005);
  t3 = lhu(at + 0x0000);
  t5 = t5 << 11;
  t5 = t5 >> 11;
  t4 = 0x80000000;
  t5 = t5 | t4;
  t3 = t3 << 2;
  t5 += t3;
  t5 = lw(t5 + 0x0000);
  t3 = (int32_t)t5 >> 21;
  t4 = t5 << 10;
  t4 = (int32_t)t4 >> 21;
  t5 = t5 << 20;
  t5 = (int32_t)t5 >> 19;
  t3 = t3 << t6;
  t4 = t4 << t6;
  t5 = t5 << t6;
  t6 = t5 << 16;
  t6 += t4;
  cop2.VZ0 = t3;
  cop2.VXY0 = t6;
  RTPS();
  t4 = cop2.IR3;
  t5 = cop2.IR1;
  t6 = cop2.IR2;
  t4 += v1;
  t5 = a0 - t5;
  t6 = a1 - t6;
  cop2.IR1 = t4;
  cop2.IR2 = t5;
  cop2.IR3 = t6;
  t2 = v0 >> 16;
  t2 += s6;
  SQR(SF_OFF);
  mult(t2, t2);
  t3 = cop2.MAC1;
  t7 = cop2.MAC2;
  s0 = cop2.MAC3;
  t2=lo;
  t3 += t7;
  t3 += s0;
  t7 = t3 - t2;
  temp = (int32_t)t7 >= 0;
  at += 8; // 0x0008
  if (temp) goto label8004E590;
  s2 = fp;
  t7 = v0 & 0x4;
  temp = t7 != 0;
  if (temp) goto label8004E978;
  t7 = lw(fp + 0x0018);
  t7 = t7 | s5;
  sw(fp + 0x0018, t7);
label8004E978:
  temp = s4 == 0;
  if (temp) goto label8004E994;
  t7 = lw(s4 + 0x0000);
  s0 = lw(at - 0x0004); // 0xFFFFFFFC
  t7 = t7 | s0;
  sw(s4 + 0x0000, t7);
label8004E994:
  t7 = v0 & 0x2;
  temp = t7 != 0;
  if (temp) goto label8004E590;
  t7 = s3 & 0x1;
  temp = t7 == 0;
  if (temp) goto label8004E9C4;
  at = t3;
  v0 = t2;
  v1 = t4;
  a0 = t5;
  a1 = t6;
  goto label8004E9D8;
label8004E9C4:
  t7 = s3 & 0x2;
  temp = t7 != 0;
  if (temp) goto label8004E590;
  goto label8004EB58;
label8004E9D8:
  cop2.LZCS = v0;
  s1 = fp;
  a2 = LZCR();
  temp = (int32_t)at > 0;
  a2 -= 2; // 0xFFFFFFFE
  if (temp) goto label8004E9F8;
  at = 1; // 0x0001
  a1 = -1; // 0xFFFFFFFF
label8004E9F8:
  a3 = v0 << a2;
  a2 -= 16; // 0xFFFFFFF0
  temp = (int32_t)a2 >= 0;
  if (temp) goto label8004EA20;
  a2 = -a2;
  at = at >> a2;
  temp = (int32_t)at > 0;
  if (temp) goto label8004EA24;
  at = 1; // 0x0001
  goto label8004EA24;
label8004EA20:
  at = at << a2;
label8004EA24:
  div_psx(a3,at);
  at=lo;
  cop2.LZCS = at;
  temp = at == 0;
  if (temp) goto label8004EA98;
  a2 = LZCR();
  t0 = -2; // 0xFFFFFFFE
  t0 = a2 & t0;
  a3 = 31; // 0x001F
  a3 -= t0;
  a3 = (int32_t)a3 >> 1;
  t1 = t0 - 24; // 0xFFFFFFE8
  temp = (int32_t)t1 < 0;
  if (temp) goto label8004EA68;
  t2 = at << t1;
  t1 = 24; // 0x0018
  goto label8004EA74;
label8004EA68:
  t1 = 24; // 0x0018
  t1 -= t0;
  t2 = (int32_t)at >> t1;
label8004EA74:
  t2 -= 64; // 0xFFFFFFC0
  t2 = t2 << 1;
  t3 = 0x80074B84; // &0x101F1000
  t3 += t2;
  t3 = lh(t3 + 0x0000);
  t3 = t3 << a3;
  at = t3 >> 12;
label8004EA98:
  at++;
  cop2.IR1 = v1;
  cop2.IR2 = a0;
  cop2.IR3 = a1;
  cop2.IR0 = at;
  GPF(SF_OFF, LM_OFF);
  a2 = cop2.MAC1;
  a3 = cop2.MAC2;
  t0 = cop2.MAC3;
  a2 = (int32_t)a2 >> 8;
  a3 = (int32_t)a3 >> 8;
  t0 = (int32_t)t0 >> 8;
  cop2.IR1 = a2;
  cop2.IR2 = a3;
  cop2.IR3 = t0;
  SQR(SF_OFF);
  t1 = cop2.MAC1;
  t2 = cop2.MAC2;
  t3 = cop2.MAC3;
  t1 += t2;
  t1 += t3;
  t2 = t1 - v0;
  temp = (int32_t)t2 >= 0;
  if (temp) goto label8004EB34;
  s7 += v1;
  t8 += a0;
  t9 += a1;
  s7 -= a2;
  t8 -= a3;
  t9 -= t0;
  at = t1;
  v1 = a2;
  a0 = a3;
  a1 = t0;
  goto label8004E9D8;
label8004EB34:
  s7 += v1;
  t8 += a0;
  t9 += a1;
  s7 -= a2;
  t8 -= a3;
  t9 -= t0;
  sw(gp + 0x0000, s7);
  sw(gp + 0x0004, t8);
  sw(gp + 0x0008, t9);
label8004EB58:
  at = s3 & 0x1;
  temp = at == 0;
  v0 = s2;
  if (temp) goto label8004EB68;
  v0 = s1;
label8004EB68:
  at = 0x80077DD8;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  return;
}

// size: 0x000004BC
void function_8004F000(void)
{
  uint32_t temp;
  at = 0x80077DD8;
  sw(at + 0x0000, s0);
  sw(at + 0x0004, s1);
  sw(at + 0x0008, s2);
  sw(at + 0x000C, s3);
  sw(at + 0x0010, s4);
  sw(at + 0x0014, s5);
  sw(at + 0x0018, s6);
  sw(at + 0x001C, s7);
  sw(at + 0x0020, gp);
  sw(at + 0x0024, sp);
  sw(at + 0x0028, fp);
  sw(at + 0x002C, ra);
  a1 = 0x80076DD0;
  t3 = lw(a1 + 0x0014);
  t4 = lw(a1 + 0x0018);
  t5 = lw(a1 + 0x001C);
  t6 = lw(a1 + 0x0020);
  t7 = lw(a1 + 0x0024);
  cop2.RTM0 = t3;
  cop2.RTM1 = t4;
  cop2.RTM2 = t5;
  cop2.RTM3 = t6;
  cop2.RTM4 = t7;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  at = SKYBOX_DATA;
  t5 = 0x8006FCF4;

  t3 = lw(at + 0x0004);
  at = lw(at + 0x0000);
  at = at << 2;
  t4 = t3 + at;
label8004F09C:
  temp = t3 == t4;
  t6 = lw(t3 + 0x0000);
  if (temp) goto label8004F0FC;
  t3 += 4; // 0x0004
  at = lw(t6 + 0x0000);
  v0 = lw(t6 + 0x0004);
  cop2.VXY0 = at;
  at = (int32_t)v0 >> 16;
  cop2.VZ0 = at;
  at = v0 << 16;
  at = (int32_t)at >> 16;
  RTPS();
  v1 = lw(t6 + 0x0014);
  v0 = cop2.SZ3;
  v0 = cop2.MAC3;
  v0 -= at;
  temp = (int32_t)v0 <= 0;
  a0 = v1 + 1; // 0x0001
  if (temp) goto label8004F09C;
  sw(t5 + 0x0000, t6);
  temp = a0 == 0;
  t5 += 4; // 0x0004
  if (temp) goto label8004F09C;
  goto label8004F09C;
label8004F0FC:
  sw(t5 + 0x0000, 0);
  at = lw(a1 + 0x0000);
  v0 = lw(a1 + 0x0004);
  v1 = lw(a1 + 0x0008);
  a0 = lw(a1 + 0x000C);
  a1 = lw(a1 + 0x0010);
  cop2.RTM0 = at;
  cop2.RTM1 = v0;
  cop2.RTM2 = v1;
  cop2.RTM3 = a0;
  cop2.RTM4 = a1;
  fp = allocator1_ptr;
  fp = lw(fp + 0x0000);
  ra = 0x8006FCF4;

  t8 = allocator1_end;
  t8 = lw(t8 + 0x0000);
  fp += 4; // 0x0004
  gp = fp;
  sp = fp - 4; // 0xFFFFFFFC
  t9 = 0;
  t8 -= 1024; // 0xFFFFFC00
label8004F160:
  s3 = lw(ra + 0x0000);
  ra += 4; // 0x0004
  temp = s3 == 0;
  at = lw(s3 + 0x0008);
  if (temp) goto label8004F434;
  v0 = lw(s3 + 0x000C);
  s6 = (int32_t)at >> 16;
  s5 = at << 16;
  s5 = (int32_t)s5 >> 16;
  s4 = (int32_t)v0 >> 16;
  s1 = s3 + 24; // 0x0018
  s2 = v0 & 0xFFFF;
  s2 = s2 << 2;
  s2 += s1;
  s2 += 8; // 0x0008
  t4 = lw(s3 + 0x0010);
  s7 = 0x1F800000;
  t5 = 0x00010000;
  t6 = 0x01000000;
  t7 = 0x02000000;
  s0 = -1; // 0xFFFFFFFF
  at = 0x800757D4;
  at = lw(at + 0x0000);
  a0 = 0x8006FCF4;
  a0 += 10240; // 0x2800
  v0 = at & 0xFF;
  v0 = v0 << 4;
  at = at >> 8;
  v1 = at & 0xFF;
  v1 = v1 << 4;
  at = at >> 8;
  at = at & 0xFF;
  at = at << 4;
  cop2.BFC = at;
  cop2.RFC = v0;
  cop2.GFC = v1;
  v0 = 0x8007575C;
  v0 = lw(v0 + 0x0000);
  t3 = s2 - 8; // 0xFFFFFFF8
  t2 = t4 >> 14;
  t2 += t3;
  cop2.IR0 = v0;
  at = lw(t3 + 0x0000);
  t3 += 4; // 0x0004
  cop2.RGBC = at;
  t2 += 4; // 0x0004
label8004F224:
  DPCS();
  at = lw(t3 + 0x0000);
  a1 = cop2.RGB2;
  t3 += 4; // 0x0004
  sw(a0 + 0x0000, a1);
  cop2.RGBC = at;
  temp = t2 != t3;
  a0 += 4; // 0x0004
  if (temp) goto label8004F224;
  at = lw(s1 + 0x0000);
  s1 += 4; // 0x0004
  v0 = at >> 21;
  v1 = at >> 10;
  v1 = v1 & 0x7FF;
  a0 = at & 0x3FF;
  v0 += s4;
  v1 = s5 - v1;
  a0 = s6 - a0;
  a0 = a0 << 16;
  v1 += a0;
  cop2.VZ0 = v0;
  cop2.VXY0 = v1;
  at = lw(s1 + 0x0000);
  s1 += 4; // 0x0004
label8004F280:
  RTPS();
  v0 = (at >> 21) & 0x7FF;
  v1 = (at >> 10) & 0x7FF;
  a0 = (at >>  0) & 0x3FF;

  v0 += s4;
  v1 = (s5 - v1);
  a0 = (s6 - a0) << 16;
  v1 += a0;
  at = lw(s1 + 0x0000);
  cop2.VZ0 = v0;
  cop2.VXY0 = v1;
  v0 = cop2.SXY2;
  a0 = v0 << 5;
  a1 = v0 - t5;
  temp = (int32_t)a1 > 0;
  a1 = v0 - t6;
  if (temp) goto label8004F2CC;
  a0++;
label8004F2CC:
  temp = (int32_t)a1 < 0;
  a1 = v0 << 16;
  if (temp) goto label8004F2D8;
  a0 += 2; // 0x0002
label8004F2D8:
  temp = (int32_t)a1 > 0;
  a1 -= t7;
  if (temp) goto label8004F2E4;
  a0 += 4; // 0x0004
label8004F2E4:
  temp = (int32_t)a1 < 0;
  s1 += 4; // 0x0004
  if (temp) goto label8004F2F0;
  a0 += 8; // 0x0008
label8004F2F0:
  s0 = s0 & a0;
  sw(s7 + 0x0000, a0);
  temp = s1 != s2;
  s7 += 4; // 0x0004
  if (temp) goto label8004F280;
  s0 = s0 & 0xF;
  temp = s0 != 0;
  if (temp) goto label8004F160;
  s7 = 0x1F800000;
  s4 = s2 - 8; // 0xFFFFFFF8
  s5 = t4 >> 14;
  s5 += s4;
  s4 = 0x8006FCF4;
  s4 += 10240; // 0x2800
  s6 = t4 << 3;
  s6 = s6 & 0xFFF8;
  s6 += s5;
  s3 = 0x86000000;
  s2 = 0x84000000;
  s1 = 0x10000000;
label8004F340:
  t4 = lw(s5 + 0x0000);
  temp = s5 == s6;
  s5 += 8; // 0x0008
  if (temp) goto label8004F160;
  at = t8 - fp;
  temp = (int32_t)at <= 0;
  t6 = t4 >> 20;
  if (temp) goto label8004F424;
  t7 = t4 >> 10;
  t7 = t7 & 0x3FC;
  s0 = t4 & 0x3FC;
  t6 += s7;
  t7 += s7;
  s0 += s7;
  at = lw(t6 + 0x0000);
  v0 = lw(t7 + 0x0000);
  v1 = lw(s0 + 0x0000);
  t5 = lw(s5 - 0x0004); // 0xFFFFFFFC
  a0 = at & v0;
  a0 = a0 & v1;
  a0 = a0 & 0x1F;
  temp = (int32_t)a0 > 0;
  a0 = t9 ^ fp;
  if (temp) goto label8004F340;
  sw(sp + 0x0000, a0);
  sp = fp;
  at = (int32_t)at >> 5;
  v0 = (int32_t)v0 >> 5;
  v1 = (int32_t)v1 >> 5;
  sw(fp + 0x0008, at);
  sw(fp + 0x0010, v0);
  sw(fp + 0x0018, v1);
  t6 = t5 >> 20;
  t7 = t5 >> 10;
  t7 = t7 & 0x3FC;
  temp = t6 != t7;
  s0 = t5 & 0x3FC;
  if (temp) goto label8004F3F4;
  temp = t6 != s0;
  t6 += s4;
  if (temp) goto label8004F3F8;
  t6 = lw(t6 + 0x0000);
  t9 = s2;
  t6 -= s1;
  sw(fp + 0x0004, t6);
  sw(fp + 0x0008, at);
  sw(fp + 0x000C, v0);
  sw(fp + 0x0010, v1);
  fp += 20; // 0x0014
  goto label8004F340;
label8004F3F4:
  t6 += s4;
label8004F3F8:
  t7 += s4;
  s0 += s4;
  t6 = lw(t6 + 0x0000);
  t7 = lw(t7 + 0x0000);
  s0 = lw(s0 + 0x0000);
  t9 = s3;
  sw(fp + 0x0004, t6);
  sw(fp + 0x000C, t7);
  sw(fp + 0x0014, s0);
  fp += 28; // 0x001C
  goto label8004F340;
label8004F424:
  at = 0x800758B0;
  v0 = 1; // 0x0001
  sw(at + 0x0000, v0);
label8004F434:
  at = allocator1_ptr;
  temp = fp == gp;
  sw(at + 0x0000, fp);
  if (temp) goto label8004F47C;
  at = ordered_linked_list;
  at = lw(at + 0x0000);
  v0 = 0x80000000;
  t9 = t9 ^ v0;
  v0 = lw(at + 0x3FF8);
  sw(sp + 0x0000, t9);
  temp = v0 == 0;
  sw(at + 0x3FF8, sp);
  if (temp) goto label8004F478;
  v1 = gp >> 16;
  sh(v0 + 0x0000, gp);
  sb(v0 + 0x0002, v1);
  goto label8004F47C;
label8004F478:
  sw(at + 0x3FFC, gp);
label8004F47C:
  at = 0x80077DD8;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  return;
}

// size: 0x000008F0
void function_8004F4BC(void)
{
  uint32_t temp;
  at = 0x80077DD8;
  sw(at + 0x0000, s0);
  sw(at + 0x0004, s1);
  sw(at + 0x0008, s2);
  sw(at + 0x000C, s3);
  sw(at + 0x0010, s4);
  sw(at + 0x0014, s5);
  sw(at + 0x0018, s6);
  sw(at + 0x001C, s7);
  sw(at + 0x0020, gp);
  sw(at + 0x0024, sp);
  sw(at + 0x0028, fp);
  sw(at + 0x002C, ra);
  t3 = lw(a1 + 0x0000);
  t4 = lw(a1 + 0x0004);
  t5 = lw(a1 + 0x0008);
  t6 = lw(a1 + 0x000C);
  t7 = lw(a1 + 0x0010);
  cop2.RTM0 = t3;
  cop2.RTM1 = t4;
  cop2.RTM2 = t5;
  cop2.RTM3 = t6;
  cop2.RTM4 = t7;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  t3 = lw(a0 + 0x0004);
  at = lw(a0 + 0x0000);
  t5 = 0x8006FCF4;

  at = at << 2;
  t4 = t3 + at;
label8004F544:
  temp = t3 == t4;
  t6 = lw(t3 + 0x0000);
  if (temp) goto label8004F5A4;
  t3 += 4; // 0x0004
  at = lw(t6 + 0x0000);
  v0 = lw(t6 + 0x0004);
  cop2.VXY0 = at;
  at = (int32_t)v0 >> 16;
  cop2.VZ0 = at;
  at = v0 << 16;
  at = (int32_t)at >> 16;
  RTPS();
  v1 = lw(t6 + 0x0014);
  v0 = cop2.SZ3;
  v0 = cop2.MAC3;
  v0 -= at;
  temp = (int32_t)v0 <= 0;
  a0 = v1 + 1; // 0x0001
  if (temp) goto label8004F544;
  sw(t5 + 0x0000, t6);
  temp = a0 == 0;
  t5 += 4; // 0x0004
  if (temp) goto label8004F544;
  goto label8004F544;
label8004F5A4:
  sw(t5 + 0x0000, 0);
  at = lw(a2 + 0x0000);
  v0 = lw(a2 + 0x0004);
  v1 = lw(a2 + 0x0008);
  a0 = lw(a2 + 0x000C);
  a1 = lw(a2 + 0x0010);
  cop2.RTM0 = at;
  cop2.RTM1 = v0;
  cop2.RTM2 = v1;
  cop2.RTM3 = a0;
  cop2.RTM4 = a1;
  fp = allocator1_ptr;
  fp = lw(fp + 0x0000);
  ra = 0x8006FCF4;

  fp += 4; // 0x0004
  gp = fp;
  sp = fp - 4; // 0xFFFFFFFC
  t9 = 0;
  v1 = 0x8007AA00;
  a3 = lw(v1 + 0x0000);
  t0 = lw(v1 + 0x0004);
  t1 = lw(v1 + 0x0008);
  a2 = lw(v1 + 0x000C);
label8004F610:
  s3 = lw(ra + 0x0000);
  ra += 4; // 0x0004
  temp = s3 == 0;
  at = lw(s3 + 0x0008);
  if (temp) goto label8004FD10;
  v0 = lw(s3 + 0x000C);
  s6 = (int32_t)at >> 16;
  s5 = at << 16;
  s5 = (int32_t)s5 >> 16;
  s4 = (int32_t)v0 >> 16;
  s1 = s3 + 24; // 0x0018
  s2 = v0 & 0xFFFF;
  s2 = s2 << 2;
  s2 += s1;
  s2 += 8; // 0x0008
  t8 = lw(s3 + 0x0010);
  s7 = 0x1F800000;
  s0 = -1; // 0xFFFFFFFF
  at = lw(s1 + 0x0000);
  s1 += 4; // 0x0004
  v0 = at >> 21;
  v1 = at >> 10;
  v1 = v1 & 0x7FF;
  a0 = at & 0x3FF;
  v0 += s4;
  v1 = s5 - v1;
  a0 = s6 - a0;
  a0 = a0 << 16;
  v1 += a0;
  cop2.VZ0 = v0;
  cop2.VXY0 = v1;
  at = lw(s1 + 0x0000);
  s1 += 4; // 0x0004
label8004F690:
  RTPS();
  v0 = at >> 21;
  v1 = at >> 10;
  v1 = v1 & 0x7FF;
  a0 = at & 0x3FF;
  v0 += s4;
  v1 = s5 - v1;
  a0 = s6 - a0;
  a0 = a0 << 16;
  v1 += a0;
  at = lw(s1 + 0x0000);
  cop2.VZ0 = v0;
  v0 = cop2.SXY2;
  cop2.VXY0 = v1;
  a0 = v0 << 5;
  a1 = v0 - a3;
  temp = (int32_t)a1 > 0;
  a1 = v0 - t0;
  if (temp) goto label8004F6DC;
  a0++;
label8004F6DC:
  temp = (int32_t)a1 < 0;
  a1 = v0 << 16;
  if (temp) goto label8004F6E8;
  a0 += 2; // 0x0002
label8004F6E8:
  v1 = a1 - a2;
  temp = (int32_t)v1 > 0;
  a1 -= t1;
  if (temp) goto label8004F6F8;
  a0 += 4; // 0x0004
label8004F6F8:
  temp = (int32_t)a1 < 0;
  s1 += 4; // 0x0004
  if (temp) goto label8004F704;
  a0 += 8; // 0x0008
label8004F704:
  s0 = s0 & a0;
  sw(s7 + 0x0000, a0);
  temp = s1 != s2;
  s7 += 4; // 0x0004
  if (temp) goto label8004F690;
  s0 = s0 & 0xF;
  temp = s0 != 0;
  if (temp) goto label8004F610;
  s7 = 0x1F800000;
  s4 = s2 - 8; // 0xFFFFFFF8
  s5 = t8 >> 14;
  s5 += s4;
  s6 = t8 << 3;
  s6 = s6 & 0xFFF8;
  s6 += s5;
  s3 = 0x86000000;
  s2 = 0x84000000;
  s1 = 0x10000000;
  a0 = 0x80077DD8;
  sw(a0 + 0x0054, ra);
label8004F754:
  temp = s5 == s6;
  t4 = lw(s5 + 0x0000);
  if (temp) goto label8004FC98;
  s5 += 8; // 0x0008
  t6 = t4 >> 20;
  t7 = t4 >> 10;
  t7 = t7 & 0x3FC;
  s0 = t4 & 0x3FC;
  t6 += s7;
  t7 += s7;
  s0 += s7;
  at = lw(t6 + 0x0000);
  v0 = lw(t7 + 0x0000);
  v1 = lw(s0 + 0x0000);
  t5 = lw(s5 - 0x0004); // 0xFFFFFFFC
  a0 = at & v0;
  a0 = a0 & v1;
  a0 = a0 & 0x1F;
  temp = (int32_t)a0 > 0;
  t6 = t5 >> 20;
  if (temp) goto label8004F754;
  t6 += s4;
  t6 = lw(t6 + 0x0000);
  t7 = t5 >> 10;
  t7 = t7 & 0x3FC;
  t7 += s4;
  t7 = lw(t7 + 0x0000);
  s0 = t5 & 0x3FC;
  s0 += s4;
  s0 = lw(s0 + 0x0000);
  at = (int32_t)at >> 5;
  v0 = (int32_t)v0 >> 5;
  v1 = (int32_t)v1 >> 5;
  t2 = 0x80077EA0;
  function_8004F7E8();
  goto label8004F754;
label8004FC98:
  a0 = 0x80075798;
  t3 = lw(a0 + 0x0000);
  t2 = 0x8006FCF4;
  t2 += 2048; // 0x0800
  temp = t2 == t3;
  t3 -= 28; // 0xFFFFFFE4
  if (temp) goto label8004FCE8;
  sw(a0 + 0x0000, t3);
  at = lw(t3 + 0x0000);
  v0 = lw(t3 + 0x0004);
  v1 = lw(t3 + 0x0008);
  t6 = lw(t3 + 0x000C);
  t7 = lw(t3 + 0x0010);
  s0 = lw(t3 + 0x0014);
  t2 = lw(t3 + 0x0018);
  t2 += 24; // 0x0018
  function_8004F7E8();
  goto label8004FC98;
label8004FCE8:
  t3 = 0x80075798;
  t2 = 0x8006FCF4;
  t2 += 2048; // 0x0800
  a0 = 0x80077DD8;
  ra = lw(a0 + 0x0054);
  sw(t3 + 0x0000, t2);
  goto label8004F610;
label8004FD10:
  at = allocator1_ptr;
  temp = fp == gp;
  sw(at + 0x0000, fp);
  if (temp) goto label8004FD6C;
  at = ordered_linked_list;
  at = lw(at + 0x0000);
  a0 = 0x80075934;
  a0 = lw(a0 + 0x0000);
  v0 = 0x80000000;
  t9 = t9 ^ v0;
  a0 = a0 << 3;
  at += a0;
  v0 = lw(at + 0x0000);
  sw(sp + 0x0000, t9);
  temp = v0 == 0;
  sw(at + 0x0000, sp);
  if (temp) goto label8004FD68;
  v1 = gp >> 16;
  sh(v0 + 0x0000, gp);
  sb(v0 + 0x0002, v1);
  goto label8004FD6C;
label8004FD68:
  sw(at + 0x0004, gp);
label8004FD6C:
  at = 0x80077DD8;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  return;
}

// size: 0x000004B0
void function_8004F7E8(void)
{
  uint32_t temp;
label8004F7E8:
  a0 = lw(t2 + 0x0000);
  temp = a0 == 0;
  a0 = lw(t2 + 0x0004);
  if (temp) goto label8004FC3C;
  cop2.SXY0 = a0;
  a0 = lw(t2 + 0x0008);
  cop2.SXY1 = a0;
  cop2.SXY2 = at;
  NCLIP();
  a1 = 0;
  a0 = cop2.MAC0;
  temp = (int32_t)a0 > 0;
  if (temp) goto label8004F834;
  a1 = a1 | 0x8;
label8004F834:
  cop2.SXY2 = v0;
  NCLIP();
  a0 = cop2.MAC0;
  temp = (int32_t)a0 > 0;
  if (temp) goto label8004F858;
  a1 = a1 | 0x10;
label8004F858:
  cop2.SXY2 = v1;
  NCLIP();
  a0 = cop2.MAC0;
  temp = (int32_t)a0 > 0;
  if (temp) goto label8004F87C;
  a1 = a1 | 0x20;
label8004F87C:
  a0 = 0x8004F890; // &0x08013DFA
  a0 += a1;
  temp = a0;
  switch (temp)
  {
  case 0x8004F890:
    goto label8004F890;
    break;
  case 0x8004F898:
    goto label8004F898;
    break;
  case 0x8004F8A0:
    goto label8004F8A0;
    break;
  case 0x8004F8A8:
    goto label8004F8A8;
    break;
  case 0x8004F8B0:
    goto label8004F8B0;
    break;
  case 0x8004F8B8:
    goto label8004F8B8;
    break;
  case 0x8004F8C0:
    goto label8004F8C0;
    break;
  case 0x8004F8C8:
    goto label8004F8C8;
    break;
  default:
    JR(temp, 0x8004F888);
    return;
  }
label8004F890:
  t2 += 24; // 0x0018
  goto label8004F7E8;
label8004F898:
  goto label8004F8D0;
label8004F8A0:
  goto label8004F978;
label8004F8A8:
  goto label8004FA20;
label8004F8B0:
  goto label8004FA9C;
label8004F8B8:
  goto label8004FB44;
label8004F8C0:
  goto label8004FBC0;
label8004F8C8:
  return;
label8004F8D0:
  t3 = 0x80077DD8;
  sw(t3 + 0x0030, at);
  sw(t3 + 0x0034, v0);
  sw(t3 + 0x0038, v1);
  sw(t3 + 0x003C, a2);
  sw(t3 + 0x0040, a3);
  sw(t3 + 0x0044, t0);
  sw(t3 + 0x0048, ra);
  a0 = v0;
  a1 = at;
  a2 = t7;
  a3 = t6;
  function_8004FDAC();
  a0 = lw(t3 + 0x0038);
  a1 = lw(t3 + 0x0030);
  a2 = s0;
  a3 = t6;
  sw(t3 + 0x0030, at);
  t6 = v0;
  function_8004FDAC();
  a0 = 0x80075798;
  a1 = lw(a0 + 0x0000);
  v1 = lw(t3 + 0x0038);
  sw(a1 + 0x0000, at);
  sw(a1 + 0x000C, v0);
  sw(a1 + 0x0010, t6);
  sw(a1 + 0x0014, s0);
  at = lw(t3 + 0x0030);
  v0 = lw(t3 + 0x0034);
  sw(a1 + 0x0004, at);
  sw(a1 + 0x0008, v1);
  sw(a1 + 0x0018, t2);
  a1 += 28; // 0x001C
  sw(a0 + 0x0000, a1);
  a2 = lw(t3 + 0x003C);
  a3 = lw(t3 + 0x0040);
  t0 = lw(t3 + 0x0044);
  ra = lw(t3 + 0x0048);
  t2 += 24; // 0x0018
  goto label8004F7E8;
label8004F978:
  t3 = 0x80077DD8;
  sw(t3 + 0x0030, at);
  sw(t3 + 0x0034, v0);
  sw(t3 + 0x0038, v1);
  sw(t3 + 0x003C, a2);
  sw(t3 + 0x0040, a3);
  sw(t3 + 0x0044, t0);
  sw(t3 + 0x0048, ra);
  a0 = v1;
  a1 = v0;
  a2 = s0;
  a3 = t7;
  function_8004FDAC();
  a0 = lw(t3 + 0x0030);
  a1 = lw(t3 + 0x0034);
  a2 = t6;
  a3 = t7;
  sw(t3 + 0x0034, at);
  t7 = v0;
  function_8004FDAC();
  a0 = 0x80075798;
  a1 = lw(a0 + 0x0000);
  v1 = lw(t3 + 0x0038);
  sw(a1 + 0x0000, at);
  sw(a1 + 0x000C, v0);
  sw(a1 + 0x0010, t7);
  sw(a1 + 0x0014, t6);
  at = lw(t3 + 0x0030);
  v0 = lw(t3 + 0x0034);
  sw(a1 + 0x0008, at);
  sw(a1 + 0x0004, v0);
  sw(a1 + 0x0018, t2);
  a1 += 28; // 0x001C
  sw(a0 + 0x0000, a1);
  a2 = lw(t3 + 0x003C);
  a3 = lw(t3 + 0x0040);
  t0 = lw(t3 + 0x0044);
  ra = lw(t3 + 0x0048);
  t2 += 24; // 0x0018
  goto label8004F7E8;
label8004FA20:
  t3 = 0x80077DD8;
  sw(t3 + 0x0030, at);
  sw(t3 + 0x0034, v0);
  sw(t3 + 0x0038, v1);
  sw(t3 + 0x003C, a2);
  sw(t3 + 0x0040, a3);
  sw(t3 + 0x0044, t0);
  sw(t3 + 0x0048, ra);
  a0 = v1;
  a1 = at;
  a2 = s0;
  a3 = t6;
  function_8004FDAC();
  sw(t3 + 0x0030, at);
  t6 = v0;
  a0 = lw(t3 + 0x0038);
  a1 = lw(t3 + 0x0034);
  a2 = s0;
  a3 = t7;
  function_8004FDAC();
  t7 = v0;
  v0 = at;
  at = lw(t3 + 0x0030);
  v1 = lw(t3 + 0x0038);
  a2 = lw(t3 + 0x003C);
  a3 = lw(t3 + 0x0040);
  t0 = lw(t3 + 0x0044);
  ra = lw(t3 + 0x0048);
  t2 += 24; // 0x0018
  goto label8004F7E8;
label8004FA9C:
  t3 = 0x80077DD8;
  sw(t3 + 0x0030, at);
  sw(t3 + 0x0034, v0);
  sw(t3 + 0x0038, v1);
  sw(t3 + 0x003C, a2);
  sw(t3 + 0x0040, a3);
  sw(t3 + 0x0044, t0);
  sw(t3 + 0x0048, ra);
  a0 = at;
  a1 = v1;
  a2 = t6;
  a3 = s0;
  function_8004FDAC();
  a0 = lw(t3 + 0x0034);
  a1 = lw(t3 + 0x0038);
  a2 = t7;
  a3 = s0;
  sw(t3 + 0x0038, at);
  s0 = v0;
  function_8004FDAC();
  a0 = 0x80075798;
  a1 = lw(a0 + 0x0000);
  v1 = lw(t3 + 0x0038);
  sw(a1 + 0x0000, at);
  sw(a1 + 0x000C, v0);
  sw(a1 + 0x0010, s0);
  sw(a1 + 0x0014, t7);
  v0 = lw(t3 + 0x0034);
  sw(a1 + 0x0004, v1);
  sw(a1 + 0x0008, v0);
  sw(a1 + 0x0018, t2);
  a1 += 28; // 0x001C
  sw(a0 + 0x0000, a1);
  at = lw(t3 + 0x0030);
  a2 = lw(t3 + 0x003C);
  a3 = lw(t3 + 0x0040);
  t0 = lw(t3 + 0x0044);
  ra = lw(t3 + 0x0048);
  t2 += 24; // 0x0018
  goto label8004F7E8;
label8004FB44:
  t3 = 0x80077DD8;
  sw(t3 + 0x0030, at);
  sw(t3 + 0x0034, v0);
  sw(t3 + 0x0038, v1);
  sw(t3 + 0x003C, a2);
  sw(t3 + 0x0040, a3);
  sw(t3 + 0x0044, t0);
  sw(t3 + 0x0048, ra);
  a0 = v0;
  a1 = at;
  a2 = t7;
  a3 = t6;
  function_8004FDAC();
  sw(t3 + 0x0030, at);
  t6 = v0;
  a0 = lw(t3 + 0x0034);
  a1 = lw(t3 + 0x0038);
  a2 = t7;
  a3 = s0;
  function_8004FDAC();
  s0 = v0;
  v1 = at;
  at = lw(t3 + 0x0030);
  v0 = lw(t3 + 0x0034);
  a2 = lw(t3 + 0x003C);
  a3 = lw(t3 + 0x0040);
  t0 = lw(t3 + 0x0044);
  ra = lw(t3 + 0x0048);
  t2 += 24; // 0x0018
  goto label8004F7E8;
label8004FBC0:
  t3 = 0x80077DD8;
  sw(t3 + 0x0030, at);
  sw(t3 + 0x0034, v0);
  sw(t3 + 0x0038, v1);
  sw(t3 + 0x003C, a2);
  sw(t3 + 0x0040, a3);
  sw(t3 + 0x0044, t0);
  sw(t3 + 0x0048, ra);
  a0 = at;
  a1 = v0;
  a2 = t6;
  a3 = t7;
  function_8004FDAC();
  t7 = v0;
  sw(t3 + 0x0034, at);
  a0 = lw(t3 + 0x0030);
  a1 = lw(t3 + 0x0038);
  a2 = t6;
  a3 = s0;
  function_8004FDAC();
  s0 = v0;
  v1 = at;
  at = lw(t3 + 0x0030);
  v0 = lw(t3 + 0x0034);
  a2 = lw(t3 + 0x003C);
  a3 = lw(t3 + 0x0040);
  t0 = lw(t3 + 0x0044);
  ra = lw(t3 + 0x0048);
  t2 += 24; // 0x0018
  goto label8004F7E8;
label8004FC3C:
  a0 = t9 ^ fp;
  sw(sp + 0x0000, a0);
  temp = t6 != t7;
  sp = fp;
  if (temp) goto label8004FC74;
  temp = t6 != s0;
  if (temp) goto label8004FC74;
  t9 = s2;
  t6 -= s1;
  sw(fp + 0x0004, t6);
  sw(fp + 0x0008, at);
  sw(fp + 0x000C, v0);
  sw(fp + 0x0010, v1);
  fp += 20; // 0x0014
  return;
label8004FC74:
  t9 = s3;
  sw(fp + 0x0008, at);
  sw(fp + 0x0010, v0);
  sw(fp + 0x0018, v1);
  sw(fp + 0x0004, t6);
  sw(fp + 0x000C, t7);
  sw(fp + 0x0014, s0);
  fp += 28; // 0x001C
  return;
}

// size: 0x000003A0
void function_8004FEA0(void)
{
  uint32_t temp;
  at = 0x80077DD8;
  sw(at + 0x0000, s0);
  sw(at + 0x0004, s1);
  sw(at + 0x0008, s2);
  sw(at + 0x000C, s3);
  sw(at + 0x0010, s4);
  sw(at + 0x0014, s5);
  sw(at + 0x0018, s6);
  sw(at + 0x001C, s7);
  sw(at + 0x0020, gp);
  sw(at + 0x0024, sp);
  sw(at + 0x0028, fp);
  sw(at + 0x002C, ra);
  t1 = lw(a0 + 0x0000);
  a0 = 0x20000000;
  t1 = t1 << 8;
  t1 = t1 >> 8;
  t1 = t1 | a0;
  t7 = 0 | 0x2;
  t3 = 0x8006CB8C;
  s1 = 0x8006FCF4;
  s1 += 2048; // 0x0800
  s2 = allocator1_ptr;
  s2 = lw(s2 + 0x0000);
  a0 = 0x80075934;
  a0 = lw(a0 + 0x0000);
  s3 = ordered_linked_list;
  s3 = lw(s3 + 0x0000);
  a0 = a0 << 3;
  s3 += a0;
  s4 = 0x04000000;
label8004FF34:
  temp = (int32_t)t7 <= 0;
  t4 = lw(t3 + 0x0000);
  if (temp) goto label8004FF60;
  t5 = lw(t3 + 0x0004);
  t6 = lw(t3 + 0x0008);
  t3 += 12; // 0x000C
  t2 = 0x80077EA0;
  t7--;
  function_8004FF90();
  goto label8004FF34;
label8004FF60:
  t3 = 0x8006FCF4;
  t3 += 2048; // 0x0800
label8004FF6C:
  temp = t3 == s1;
  t4 = lw(t3 + 0x0000);
  if (temp) goto label800501F4;
  t5 = lw(t3 + 0x0004);
  t6 = lw(t3 + 0x0008);
  t2 = lw(t3 + 0x000C);
  t3 += 16; // 0x0010
  function_8004FF90();
  goto label8004FF6C;
label800501F4:
  a0 = allocator1_ptr;
  sw(a0 + 0x0000, s2);
  at = 0x80077DD8;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  return;
}

// size: 0x00000990
void function_80050240(void)
{
  uint32_t temp;
  at = 0x80077DD8;
  sw(at + 0x0000, s0);
  sw(at + 0x0004, s1);
  sw(at + 0x0008, s2);
  sw(at + 0x000C, s3);
  sw(at + 0x0010, s4);
  sw(at + 0x0014, s5);
  sw(at + 0x0018, s6);
  sw(at + 0x001C, s7);
  sw(at + 0x0020, gp);
  sw(at + 0x0024, sp);
  sw(at + 0x0028, fp);
  sw(at + 0x002C, ra);
  t3 = lw(a1 + 0x0000);
  t4 = lw(a1 + 0x0004);
  t5 = lw(a1 + 0x0008);
  t6 = lw(a1 + 0x000C);
  t7 = lw(a1 + 0x0010);
  cop2.RTM0 = t3;
  cop2.RTM1 = t4;
  cop2.RTM2 = t5;
  cop2.RTM3 = t6;
  cop2.RTM4 = t7;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  t3 = lw(a0 + 0x0004);
  at = lw(a0 + 0x0000);
  t5 = 0x8006FCF4;

  at = at << 2;
  t4 = t3 + at;
label800502C8:
  temp = t3 == t4;
  t6 = lw(t3 + 0x0000);
  if (temp) goto label80050328;
  t3 += 4; // 0x0004
  at = lw(t6 + 0x0000);
  v0 = lw(t6 + 0x0004);
  cop2.VXY0 = at;
  at = (int32_t)v0 >> 16;
  cop2.VZ0 = at;
  at = v0 << 16;
  at = (int32_t)at >> 16;
  RTPS();
  v1 = lw(t6 + 0x0014);
  v0 = cop2.SZ3;
  v0 = cop2.MAC3;
  v0 -= at;
  temp = (int32_t)v0 <= 0;
  a0 = v1 + 1; // 0x0001
  if (temp) goto label800502C8;
  sw(t5 + 0x0000, t6);
  temp = a0 == 0;
  t5 += 4; // 0x0004
  if (temp) goto label800502C8;
  goto label800502C8;
label80050328:
  sw(t5 + 0x0000, 0);
  at = lw(a2 + 0x0000);
  v0 = lw(a2 + 0x0004);
  v1 = lw(a2 + 0x0008);
  a0 = lw(a2 + 0x000C);
  a1 = lw(a2 + 0x0010);
  cop2.RTM0 = at;
  cop2.RTM1 = v0;
  cop2.RTM2 = v1;
  cop2.RTM3 = a0;
  cop2.RTM4 = a1;
  fp = allocator1_ptr;
  fp = lw(fp + 0x0000);
  ra = 0x8006FCF4;

  fp += 4; // 0x0004
  gp = fp;
  sp = fp - 4; // 0xFFFFFFFC
  t9 = 0;
  v1 = 0x8007AA00;
  a3 = lw(v1 + 0x0000);
  t0 = lw(v1 + 0x0004);
  t1 = lw(v1 + 0x0008);
  a2 = lw(v1 + 0x000C);
label80050394:
  s3 = lw(ra + 0x0000);
  ra += 4; // 0x0004
  temp = s3 == 0;
  at = lw(s3 + 0x0008);
  if (temp) goto label80050B34;
  v0 = lw(s3 + 0x000C);
  s6 = (int32_t)at >> 16;
  s5 = at << 16;
  s5 = (int32_t)s5 >> 16;
  s4 = (int32_t)v0 >> 16;
  s1 = s3 + 24; // 0x0018
  s2 = v0 & 0xFFFF;
  s2 = s2 << 2;
  s2 += s1;
  s2 += 8; // 0x0008
  t8 = lw(s3 + 0x0010);
  s7 = 0x1F800000;
  s0 = -1; // 0xFFFFFFFF
  at = 0x800757D4;
  at = lw(at + 0x0000);
  a0 = 0x8006FCF4;
  a0 += 10240; // 0x2800
  v0 = at & 0xFF;
  v0 = v0 << 4;
  at = at >> 8;
  v1 = at & 0xFF;
  v1 = v1 << 4;
  at = at >> 8;
  at = at & 0xFF;
  at = at << 4;
  cop2.BFC = at;
  cop2.RFC = v0;
  cop2.GFC = v1;
  v0 = 0x8007575C;
  v0 = lw(v0 + 0x0000);
  t3 = s2 - 8; // 0xFFFFFFF8
  t2 = t8 >> 14;
  t2 += t3;
  cop2.IR0 = v0;
  at = lw(t3 + 0x0000);
  t3 += 4; // 0x0004
  cop2.RGBC = at;
  t2 += 4; // 0x0004
label8005044C:
  DPCS();
  at = lw(t3 + 0x0000);
  a1 = cop2.RGB2;
  t3 += 4; // 0x0004
  sw(a0 + 0x0000, a1);
  cop2.RGBC = at;
  temp = t2 != t3;
  a0 += 4; // 0x0004
  if (temp) goto label8005044C;
  at = lw(s1 + 0x0000);
  s1 += 4; // 0x0004
  v0 = at >> 21;
  v1 = at >> 10;
  v1 = v1 & 0x7FF;
  a0 = at & 0x3FF;
  v0 += s4;
  v1 = s5 - v1;
  a0 = s6 - a0;
  a0 = a0 << 16;
  v1 += a0;
  cop2.VZ0 = v0;
  cop2.VXY0 = v1;
  at = lw(s1 + 0x0000);
  s1 += 4; // 0x0004
label800504A8:
  RTPS();
  v0 = at >> 21;
  v1 = at >> 10;
  v1 = v1 & 0x7FF;
  a0 = at & 0x3FF;
  v0 += s4;
  v1 = s5 - v1;
  a0 = s6 - a0;
  a0 = a0 << 16;
  v1 += a0;
  at = lw(s1 + 0x0000);
  cop2.VZ0 = v0;
  v0 = cop2.SXY2;
  cop2.VXY0 = v1;
  a0 = v0 << 5;
  a1 = v0 - a3;
  temp = (int32_t)a1 > 0;
  a1 = v0 - t0;
  if (temp) goto label800504F4;
  a0++;
label800504F4:
  temp = (int32_t)a1 < 0;
  a1 = v0 << 16;
  if (temp) goto label80050500;
  a0 += 2; // 0x0002
label80050500:
  v1 = a1 - a2;
  temp = (int32_t)v1 > 0;
  a1 -= t1;
  if (temp) goto label80050510;
  a0 += 4; // 0x0004
label80050510:
  temp = (int32_t)a1 < 0;
  s1 += 4; // 0x0004
  if (temp) goto label8005051C;
  a0 += 8; // 0x0008
label8005051C:
  s0 = s0 & a0;
  sw(s7 + 0x0000, a0);
  temp = s1 != s2;
  s7 += 4; // 0x0004
  if (temp) goto label800504A8;
  s0 = s0 & 0xF;
  temp = s0 != 0;
  if (temp) goto label80050394;
  s7 = 0x1F800000;
  s4 = s2 - 8; // 0xFFFFFFF8
  s5 = t8 >> 14;
  s5 += s4;
  s4 = 0x8006FCF4;
  s4 += 10240; // 0x2800
  s6 = t8 << 3;
  s6 = s6 & 0xFFF8;
  s6 += s5;
  s3 = 0x86000000;
  s2 = 0x84000000;
  s1 = 0x10000000;
  a0 = 0x80077DD8;
  sw(a0 + 0x0054, ra);
label80050578:
  temp = s5 == s6;
  t4 = lw(s5 + 0x0000);
  if (temp) goto label80050ABC;
  s5 += 8; // 0x0008
  t6 = t4 >> 20;
  t7 = t4 >> 10;
  t7 = t7 & 0x3FC;
  s0 = t4 & 0x3FC;
  t6 += s7;
  t7 += s7;
  s0 += s7;
  at = lw(t6 + 0x0000);
  v0 = lw(t7 + 0x0000);
  v1 = lw(s0 + 0x0000);
  t5 = lw(s5 - 0x0004); // 0xFFFFFFFC
  a0 = at & v0;
  a0 = a0 & v1;
  a0 = a0 & 0x1F;
  temp = (int32_t)a0 > 0;
  t6 = t5 >> 20;
  if (temp) goto label80050578;
  t6 += s4;
  t6 = lw(t6 + 0x0000);
  t7 = t5 >> 10;
  t7 = t7 & 0x3FC;
  t7 += s4;
  t7 = lw(t7 + 0x0000);
  s0 = t5 & 0x3FC;
  s0 += s4;
  s0 = lw(s0 + 0x0000);
  at = (int32_t)at >> 5;
  v0 = (int32_t)v0 >> 5;
  v1 = (int32_t)v1 >> 5;
  t2 = 0x80077EA0;
  function_8005060C();
  goto label80050578;
label80050ABC:
  a0 = 0x80075798;
  t3 = lw(a0 + 0x0000);
  t2 = 0x8006FCF4;
  t2 += 2048; // 0x0800
  temp = t2 == t3;
  t3 -= 28; // 0xFFFFFFE4
  if (temp) goto label80050B0C;
  sw(a0 + 0x0000, t3);
  at = lw(t3 + 0x0000);
  v0 = lw(t3 + 0x0004);
  v1 = lw(t3 + 0x0008);
  t6 = lw(t3 + 0x000C);
  t7 = lw(t3 + 0x0010);
  s0 = lw(t3 + 0x0014);
  t2 = lw(t3 + 0x0018);
  t2 += 24; // 0x0018
  function_8005060C();
  goto label80050ABC;
label80050B0C:
  t3 = 0x80075798;
  t2 = 0x8006FCF4;
  t2 += 2048; // 0x0800
  a0 = 0x80077DD8;
  ra = lw(a0 + 0x0054);
  sw(t3 + 0x0000, t2);
  goto label80050394;
label80050B34:
  at = allocator1_ptr;
  temp = fp == gp;
  sw(at + 0x0000, fp);
  if (temp) goto label80050B90;
  at = ordered_linked_list;
  at = lw(at + 0x0000);
  a0 = 0x80075934;
  a0 = lw(a0 + 0x0000);
  v0 = 0x80000000;
  t9 = t9 ^ v0;
  a0 = a0 << 3;
  at += a0;
  v0 = lw(at + 0x0000);
  sw(sp + 0x0000, t9);
  temp = v0 == 0;
  sw(at + 0x0000, sp);
  if (temp) goto label80050B8C;
  v1 = gp >> 16;
  sh(v0 + 0x0000, gp);
  sb(v0 + 0x0002, v1);
  goto label80050B90;
label80050B8C:
  sw(at + 0x0004, gp);
label80050B90:
  at = 0x80077DD8;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  return;
}

// size: 0x000004B0
void function_8005060C(void)
{
  uint32_t temp;
label8005060C:
  a0 = lw(t2 + 0x0000);
  temp = a0 == 0;
  a0 = lw(t2 + 0x0004);
  if (temp) goto label80050A60;
  cop2.SXY0 = a0;
  a0 = lw(t2 + 0x0008);
  cop2.SXY1 = a0;
  cop2.SXY2 = at;
  NCLIP();
  a1 = 0;
  a0 = cop2.MAC0;
  temp = (int32_t)a0 > 0;
  if (temp) goto label80050658;
  a1 = a1 | 0x8;
label80050658:
  cop2.SXY2 = v0;
  NCLIP();
  a0 = cop2.MAC0;
  temp = (int32_t)a0 > 0;
  if (temp) goto label8005067C;
  a1 = a1 | 0x10;
label8005067C:
  cop2.SXY2 = v1;
  NCLIP();
  a0 = cop2.MAC0;
  temp = (int32_t)a0 > 0;
  if (temp) goto label800506A0;
  a1 = a1 | 0x20;
label800506A0:
  a0 = 0x800506B4; // &0x08014183
  a0 += a1;
  temp = a0;
  switch (temp)
  {
  case 0x800506B4:
    goto label800506B4;
    break;
  case 0x800506BC:
    goto label800506BC;
    break;
  case 0x800506C4:
    goto label800506C4;
    break;
  case 0x800506CC:
    goto label800506CC;
    break;
  case 0x800506D4:
    goto label800506D4;
    break;
  case 0x800506DC:
    goto label800506DC;
    break;
  case 0x800506E4:
    goto label800506E4;
    break;
  case 0x800506EC:
    goto label800506EC;
    break;
  default:
    JR(temp, 0x800506AC);
    return;
  }
label800506B4:
  t2 += 24; // 0x0018
  goto label8005060C;
label800506BC:
  goto label800506F4;
label800506C4:
  goto label8005079C;
label800506CC:
  goto label80050844;
label800506D4:
  goto label800508C0;
label800506DC:
  goto label80050968;
label800506E4:
  goto label800509E4;
label800506EC:
  return;
label800506F4:
  t3 = 0x80077DD8;
  sw(t3 + 0x0030, at);
  sw(t3 + 0x0034, v0);
  sw(t3 + 0x0038, v1);
  sw(t3 + 0x003C, a2);
  sw(t3 + 0x0040, a3);
  sw(t3 + 0x0044, t0);
  sw(t3 + 0x0048, ra);
  a0 = v0;
  a1 = at;
  a2 = t7;
  a3 = t6;
  function_8004FDAC();
  a0 = lw(t3 + 0x0038);
  a1 = lw(t3 + 0x0030);
  a2 = s0;
  a3 = t6;
  sw(t3 + 0x0030, at);
  t6 = v0;
  function_8004FDAC();
  a0 = 0x80075798;
  a1 = lw(a0 + 0x0000);
  v1 = lw(t3 + 0x0038);
  sw(a1 + 0x0000, at);
  sw(a1 + 0x000C, v0);
  sw(a1 + 0x0010, t6);
  sw(a1 + 0x0014, s0);
  at = lw(t3 + 0x0030);
  v0 = lw(t3 + 0x0034);
  sw(a1 + 0x0004, at);
  sw(a1 + 0x0008, v1);
  sw(a1 + 0x0018, t2);
  a1 += 28; // 0x001C
  sw(a0 + 0x0000, a1);
  a2 = lw(t3 + 0x003C);
  a3 = lw(t3 + 0x0040);
  t0 = lw(t3 + 0x0044);
  ra = lw(t3 + 0x0048);
  t2 += 24; // 0x0018
  goto label8005060C;
label8005079C:
  t3 = 0x80077DD8;
  sw(t3 + 0x0030, at);
  sw(t3 + 0x0034, v0);
  sw(t3 + 0x0038, v1);
  sw(t3 + 0x003C, a2);
  sw(t3 + 0x0040, a3);
  sw(t3 + 0x0044, t0);
  sw(t3 + 0x0048, ra);
  a0 = v1;
  a1 = v0;
  a2 = s0;
  a3 = t7;
  function_8004FDAC();
  a0 = lw(t3 + 0x0030);
  a1 = lw(t3 + 0x0034);
  a2 = t6;
  a3 = t7;
  sw(t3 + 0x0034, at);
  t7 = v0;
  function_8004FDAC();
  a0 = 0x80075798;
  a1 = lw(a0 + 0x0000);
  v1 = lw(t3 + 0x0038);
  sw(a1 + 0x0000, at);
  sw(a1 + 0x000C, v0);
  sw(a1 + 0x0010, t7);
  sw(a1 + 0x0014, t6);
  at = lw(t3 + 0x0030);
  v0 = lw(t3 + 0x0034);
  sw(a1 + 0x0008, at);
  sw(a1 + 0x0004, v0);
  sw(a1 + 0x0018, t2);
  a1 += 28; // 0x001C
  sw(a0 + 0x0000, a1);
  a2 = lw(t3 + 0x003C);
  a3 = lw(t3 + 0x0040);
  t0 = lw(t3 + 0x0044);
  ra = lw(t3 + 0x0048);
  t2 += 24; // 0x0018
  goto label8005060C;
label80050844:
  t3 = 0x80077DD8;
  sw(t3 + 0x0030, at);
  sw(t3 + 0x0034, v0);
  sw(t3 + 0x0038, v1);
  sw(t3 + 0x003C, a2);
  sw(t3 + 0x0040, a3);
  sw(t3 + 0x0044, t0);
  sw(t3 + 0x0048, ra);
  a0 = v1;
  a1 = at;
  a2 = s0;
  a3 = t6;
  function_8004FDAC();
  sw(t3 + 0x0030, at);
  t6 = v0;
  a0 = lw(t3 + 0x0038);
  a1 = lw(t3 + 0x0034);
  a2 = s0;
  a3 = t7;
  function_8004FDAC();
  t7 = v0;
  v0 = at;
  at = lw(t3 + 0x0030);
  v1 = lw(t3 + 0x0038);
  a2 = lw(t3 + 0x003C);
  a3 = lw(t3 + 0x0040);
  t0 = lw(t3 + 0x0044);
  ra = lw(t3 + 0x0048);
  t2 += 24; // 0x0018
  goto label8005060C;
label800508C0:
  t3 = 0x80077DD8;
  sw(t3 + 0x0030, at);
  sw(t3 + 0x0034, v0);
  sw(t3 + 0x0038, v1);
  sw(t3 + 0x003C, a2);
  sw(t3 + 0x0040, a3);
  sw(t3 + 0x0044, t0);
  sw(t3 + 0x0048, ra);
  a0 = at;
  a1 = v1;
  a2 = t6;
  a3 = s0;
  function_8004FDAC();
  a0 = lw(t3 + 0x0034);
  a1 = lw(t3 + 0x0038);
  a2 = t7;
  a3 = s0;
  sw(t3 + 0x0038, at);
  s0 = v0;
  function_8004FDAC();
  a0 = 0x80075798;
  a1 = lw(a0 + 0x0000);
  v1 = lw(t3 + 0x0038);
  sw(a1 + 0x0000, at);
  sw(a1 + 0x000C, v0);
  sw(a1 + 0x0010, s0);
  sw(a1 + 0x0014, t7);
  v0 = lw(t3 + 0x0034);
  sw(a1 + 0x0004, v1);
  sw(a1 + 0x0008, v0);
  sw(a1 + 0x0018, t2);
  a1 += 28; // 0x001C
  sw(a0 + 0x0000, a1);
  at = lw(t3 + 0x0030);
  a2 = lw(t3 + 0x003C);
  a3 = lw(t3 + 0x0040);
  t0 = lw(t3 + 0x0044);
  ra = lw(t3 + 0x0048);
  t2 += 24; // 0x0018
  goto label8005060C;
label80050968:
  t3 = 0x80077DD8;
  sw(t3 + 0x0030, at);
  sw(t3 + 0x0034, v0);
  sw(t3 + 0x0038, v1);
  sw(t3 + 0x003C, a2);
  sw(t3 + 0x0040, a3);
  sw(t3 + 0x0044, t0);
  sw(t3 + 0x0048, ra);
  a0 = v0;
  a1 = at;
  a2 = t7;
  a3 = t6;
  function_8004FDAC();
  sw(t3 + 0x0030, at);
  t6 = v0;
  a0 = lw(t3 + 0x0034);
  a1 = lw(t3 + 0x0038);
  a2 = t7;
  a3 = s0;
  function_8004FDAC();
  s0 = v0;
  v1 = at;
  at = lw(t3 + 0x0030);
  v0 = lw(t3 + 0x0034);
  a2 = lw(t3 + 0x003C);
  a3 = lw(t3 + 0x0040);
  t0 = lw(t3 + 0x0044);
  ra = lw(t3 + 0x0048);
  t2 += 24; // 0x0018
  goto label8005060C;
label800509E4:
  t3 = 0x80077DD8;
  sw(t3 + 0x0030, at);
  sw(t3 + 0x0034, v0);
  sw(t3 + 0x0038, v1);
  sw(t3 + 0x003C, a2);
  sw(t3 + 0x0040, a3);
  sw(t3 + 0x0044, t0);
  sw(t3 + 0x0048, ra);
  a0 = at;
  a1 = v0;
  a2 = t6;
  a3 = t7;
  function_8004FDAC();
  t7 = v0;
  sw(t3 + 0x0034, at);
  a0 = lw(t3 + 0x0030);
  a1 = lw(t3 + 0x0038);
  a2 = t6;
  a3 = s0;
  function_8004FDAC();
  s0 = v0;
  v1 = at;
  at = lw(t3 + 0x0030);
  v0 = lw(t3 + 0x0034);
  a2 = lw(t3 + 0x003C);
  a3 = lw(t3 + 0x0040);
  t0 = lw(t3 + 0x0044);
  ra = lw(t3 + 0x0048);
  t2 += 24; // 0x0018
  goto label8005060C;
label80050A60:
  a0 = t9 ^ fp;
  sw(sp + 0x0000, a0);
  temp = t6 != t7;
  sp = fp;
  if (temp) goto label80050A98;
  temp = t6 != s0;
  if (temp) goto label80050A98;
  t9 = s2;
  t6 -= s1;
  sw(fp + 0x0004, t6);
  sw(fp + 0x0008, at);
  sw(fp + 0x000C, v0);
  sw(fp + 0x0010, v1);
  fp += 20; // 0x0014
  return;
label80050A98:
  t9 = s3;
  sw(fp + 0x0008, at);
  sw(fp + 0x0010, v0);
  sw(fp + 0x0018, v1);
  sw(fp + 0x0004, t6);
  sw(fp + 0x000C, t7);
  sw(fp + 0x0014, s0);
  fp += 28; // 0x001C
  return;
}

// size: 0x00000204
void function_800522C0(void)
{
  uint32_t temp;
  at = 0x8006FCF4;
  lo=at;
  t9 = a0;
  t8 = 0x1F800000;
  t7 = 0x80076378;
  t6 = a1;
label800522E0:
  t5 = lw(t9 + 0x0000);
  t9 += 4; // 0x0004
  temp = t5 == 0;
  at = lw(t5 + 0x0040);
  if (temp) goto label80052448;
  v0 = lw(t5 + 0x003C);
  sw(t8 + 0x0000, at);
  a0 = lbu(t8 + 0x0000);
  a1 = lbu(t8 + 0x0001);
  sw(t8 + 0x0004, v0);
  a1 = a1 >> t6;
  a0 += a1;
  at = a0 >> 6;
  temp = at == 0;
  a0 = a0 & 0x3F;
  if (temp) goto label80052424;
  a2 = lbu(t8 + 0x0004);
  a3 = lbu(t8 + 0x0005);
  t2 = lhu(t5 + 0x0036);
  temp = a2 != a3;
  t0 = lbu(t8 + 0x0007);
  if (temp) goto label80052398;
  temp = (int32_t)t6 >= 0;
  v0 = 0;
  if (temp) goto label8005234C;
  v0 = lb(t8 + 0x0002);
  temp = v0 == 0;
  at = v0 & 0x2;
  if (temp) goto label8005234C;
  temp = (int32_t)at > 0;
  v0 = v0 | 0x4;
  if (temp) goto label800522E0;
label8005234C:
  sb(t8 + 0x0000, a0);
  sb(t8 + 0x0006, t0);
  t2 = t2 << 2;
  t2 += t7;
  t2 = lw(t2 + 0x0000);
  t3 = a3 << 2;
  t3 += t2;
  t3 = lw(t3 + 0x0038);
  t1 = t0 + 1; // 0x0001
  at = lhu(t3 + 0x0000);
  v0 = v0 | 0x1;
  sb(t8 + 0x0002, v0);
  at = t1 - at;
  temp = (int32_t)at < 0;
  sb(t8 + 0x0007, t1);
  if (temp) goto label800523E0;
  sb(t8 + 0x0007, 0);
  v0 = v0 | 0x2;
  sb(t8 + 0x0002, v0);
  goto label800523E0;
label80052398:
  sb(t8 + 0x0000, 0);
  sb(t8 + 0x0004, a3);
  sb(t8 + 0x0006, t0);
  t2 = t2 << 2;
  t2 += t7;
  t2 = lw(t2 + 0x0000);
  t3 = a3 << 2;
  t3 += t2;
  t3 = lw(t3 + 0x0038);
  t1 = t0 + 1; // 0x0001
  sb(t8 + 0x0007, t1);
  a1 = lbu(t3 + 0x000C);
  at = 0 | 0x1;
  sb(t8 + 0x0002, at);
  temp = (int32_t)a1 > 0;
  sb(t8 + 0x0001, a1);
  if (temp) goto label800523E0;
  t1--;
  sb(t8 + 0x0007, t1);
label800523E0:
  at = lw(t8 + 0x0000);
  v0 = lw(t8 + 0x0004);
  sw(t5 + 0x0040, at);
  sw(t5 + 0x003C, v0);
  at = t0 << 3;
  t4 = t3 + at;
  t4 += 36; // 0x0024
  at = lb(t4 + 0x0003);
  v0=lo;
  sw(v0 + 0x0004, t5);
  temp = (int32_t)at < 0;
  at += t2;
  if (temp) goto label800522E0;
  at = lbu(at + 0x0004);
  v0 += 8; // 0x0008
  lo=v0;
  sw(v0 - 0x0008, at); // 0xFFFFFFF8
  goto label800522E0;
label80052424:
  temp = (int32_t)t6 < 0;
  if (temp) goto label80052430;
  sb(t8 + 0x0002, 0);
label80052430:
  sb(t8 + 0x0000, a0);
  at = lw(t8 + 0x0000);
  v0 = lw(t8 + 0x0004);
  sw(t5 + 0x0040, at);
  sw(t5 + 0x003C, v0);
  goto label800522E0;
label80052448:
  at = 0x8006FCF4;
  a0=lo;
  temp = at != a0;
  if (temp) goto label80052464;
  return;
label80052464:
  t7 = 0x80077DD8;
  sw(t7 + 0x0000, at);
  sw(t7 + 0x0004, a0);
  sw(t7 + 0x0008, ra);
label80052478:
  a0 = lw(at + 0x0000);
  a1 = lw(at + 0x0004);
  at += 8; // 0x0008
  sw(t7 + 0x0000, at);
  sw(t7 + 0x000C, a1);
  a2 = 8; // 0x0008
  a3 = a1 + 84; // 0x0054
  function_80055A78();
  t7 = 0x80077DD8;
  at = lw(t7 + 0x0000);
  a0 = lw(t7 + 0x0004);
  temp = at != a0;
  if (temp) goto label80052478;
  ra = lw(t7 + 0x0008);
  return;
}

// size: 0x000003D0
void function_800580F4(void)
{
  uint32_t temp;
  at = 0x80077DD8;
  sw(at + 0x0000, s0);
  sw(at + 0x0004, s1);
  sw(at + 0x0008, s2);
  sw(at + 0x000C, s3);
  sw(at + 0x0010, s4);
  sw(at + 0x0014, s5);
  sw(at + 0x0018, s6);
  sw(at + 0x001C, s7);
  sw(at + 0x0020, gp);
  sw(at + 0x0024, sp);
  sw(at + 0x0028, fp);
  sw(at + 0x002C, ra);
  a2 = 0x80076DD0;
  at = lw(a2 + 0x0000);
  v0 = lw(a2 + 0x0004);
  v1 = lw(a2 + 0x0008);
  a0 = lw(a2 + 0x000C);
  a1 = lw(a2 + 0x0010);
  cop2.RTM0 = at;
  cop2.RTM1 = v0;
  cop2.RTM2 = v1;
  cop2.RTM3 = a0;
  cop2.RTM4 = a1;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  at = lw(a2 + 0x0028);
  v0 = lw(a2 + 0x002C);
  v1 = lw(a2 + 0x0030);
  a0 = 0x80078800;
  t0 = 0 | 0x10;
  s0 = ordered_linked_list;
  s0 = lw(s0 + 0x0000);
  s1 = allocator1_ptr;
  s1 = lw(s1 + 0x0000);
label80058198:
  t6 = lw(a0 + 0x0000);
  t3 = lw(a0 + 0x0008);
  temp = t6 == 0;
  a1 = lw(t3 + 0x0000);
  if (temp) goto label8005846C;
  a2 = lw(t3 + 0x0004);
  a3 = lw(t3 + 0x0008);
  t3 = lw(a0 + 0x0014);
  t4 = lw(a0 + 0x0018);
  t5 = lw(a0 + 0x001C);
  a1 += t3;
  a2 += t4;
  a3 += t5;
  a1 -= at;
  a2 = v0 - a2;
  a3 = v1 - a3;
  temp = (int32_t)a1 >= 0;
  t1 = a1;
  if (temp) goto label800581E0;
  t1 = -t1;
label800581E0:
  temp = (int32_t)a2 >= 0;
  t2 = a2;
  if (temp) goto label800581EC;
  t2 = -t2;
label800581EC:
  t2 += t1;
  temp = (int32_t)a3 >= 0;
  t1 = a3;
  if (temp) goto label800581FC;
  t1 = -t1;
label800581FC:
  t2 += t1;
  temp = (int32_t)t2 <= 0;
  s2 = 0;
  if (temp) goto label80058228;
  s2 = t2 >> 13;
  t1 = s2 - 4; // 0xFFFFFFFC
  temp = (int32_t)t1 <= 0;
  if (temp) goto label8005821C;
  s2 = 0 | 0x4;
label8005821C:
  a1 = (int32_t)a1 >> s2;
  a2 = (int32_t)a2 >> s2;
  a3 = (int32_t)a3 >> s2;
label80058228:
  a3 = a3 << 16;
  a2 = a2 & 0xFFFF;
  a2 = a2 | a3;
  cop2.VZ0 = a1;
  cop2.VXY0 = a2;
  RTPS();
  t1 = 0x02000000;
  sw(s1 + 0x0000, t1);
  t1 = 0xE1000220;
  sw(s1 + 0x0004, t1);
  sw(s1 + 0x0008, 0);
  t3 = cop2.SXY2;
  a3 = cop2.MAC3;
  a2 = (int32_t)t3 >> 16;
  a1 = t3 << 16;
  a1 = (int32_t)a1 >> 16;
  t1 = lw(a0 + 0x0010);
  a3 = a3 << s2;
  t1 = t1 << 12;
  div_psx(t1,a3);
  t1 = lw(a0 + 0x0020);
  a3 = a3 >> 7;
  a3 += t1;
  temp = (int32_t)a3 <= 0;
  t2 = a3 - 255; // 0xFFFFFF01
  if (temp) goto label8005846C;
  temp = (int32_t)t2 <= 0;
  if (temp) goto label800582A4;
  a3 += 64; // 0x0040
label800582A4:
  t2 = a3 - 2047; // 0xFFFFF801
  temp = (int32_t)t2 <= 0;
  if (temp) goto label800582B4;
  a3 = 0 | 0x7FF;
label800582B4:
  a3 = a3 << 3;
  a3 += s0;
  t1 = a2 - 1; // 0xFFFFFFFF
  temp = (int32_t)t1 > 0;
  t7 = 0;
  if (temp) goto label800582CC;
  t7 = t7 | 0x1;
label800582CC:
  t1 = a2 - 256; // 0xFFFFFF00
  temp = (int32_t)t1 < 0;
  if (temp) goto label800582DC;
  t7 = t7 | 0x2;
label800582DC:
  t1 = a1 - 512; // 0xFFFFFE00
  temp = (int32_t)t1 < 0;
  if (temp) goto label800582EC;
  t7 = t7 | 0x4;
label800582EC:
  temp = (int32_t)a1 > 0;
  if (temp) goto label800582F8;
  t7 = t7 | 0x8;
label800582F8:
  t1=lo;
  cop2.IR0 = t1;
  t1 = lw(a3 + 0x0000);
  temp = t1 == 0;
  sw(a3 + 0x0000, s1);
  if (temp) goto label80058320;
  sh(t1 + 0x0000, s1);
  t2 = s1 >> 16;
  sb(t1 + 0x0002, t2);
  goto label80058324;
label80058320:
  sw(a3 + 0x0004, s1);
label80058324:
  s1 += 12; // 0x000C
  t1 = lw(a0 + 0x0004);
  t2 = lw(t1 + 0x0000);
  t4 = lw(t1 + 0x0004);
  cop2.IR1 = t2;
  cop2.IR2 = t4;
  t1 += 8; // 0x0008
  t6--;
  GPF(SF_ON, LM_OFF);
  s4 = lw(a0 + 0x000C);
  t5 = 0x32000000;
  s4 = s4 << 8;
  s4 = s4 >> 8;
  s4 = s4 | t5;
  t2 = cop2.MAC1;
  t4 = cop2.MAC2;
  t2 += a1;
  t4 += a2;
  t5 = t4 - 1; // 0xFFFFFFFF
  temp = (int32_t)t5 > 0;
  s2 = 0;
  if (temp) goto label80058384;
  s2 = s2 | 0x1;
label80058384:
  t5 = t4 - 256; // 0xFFFFFF00
  temp = (int32_t)t5 < 0;
  t4 = t4 << 16;
  if (temp) goto label80058394;
  s2 = s2 | 0x2;
label80058394:
  t5 = t2 - 512; // 0xFFFFFE00
  temp = (int32_t)t5 < 0;
  s3 = t2 & 0xFFFF;
  if (temp) goto label800583A4;
  s2 = s2 | 0x4;
label800583A4:
  temp = (int32_t)t2 > 0;
  s3 = s3 | t4;
  if (temp) goto label800583B0;
  s2 = s2 | 0x8;
label800583B0:
  t2 = lw(t1 + 0x0000);
  t4 = lw(t1 + 0x0004);
  cop2.IR1 = t2;
  cop2.IR2 = t4;
  t5 = 0x06000000;
  sw(s1 + 0x0000, t5);
  GPF(SF_ON, LM_OFF);
  sw(s1 + 0x0008, t3);
  sw(s1 + 0x0004, s4);
  sw(s1 + 0x0010, s3);
  sw(s1 + 0x000C, 0);
  sw(s1 + 0x0014, 0);
  t2 = cop2.MAC1;
  t4 = cop2.MAC2;
  t2 += a1;
  t4 += a2;
  t5 = t4 - 1; // 0xFFFFFFFF
  temp = (int32_t)t5 > 0;
  s5 = 0;
  if (temp) goto label80058404;
  s5 = s5 | 0x1;
label80058404:
  t5 = t4 - 256; // 0xFFFFFF00
  temp = (int32_t)t5 < 0;
  t4 = t4 << 16;
  if (temp) goto label80058414;
  s5 = s5 | 0x2;
label80058414:
  t5 = t2 - 512; // 0xFFFFFE00
  temp = (int32_t)t5 < 0;
  s6 = t2 & 0xFFFF;
  if (temp) goto label80058424;
  s5 = s5 | 0x4;
label80058424:
  temp = (int32_t)t2 > 0;
  s6 = s6 | t4;
  if (temp) goto label80058430;
  s5 = s5 | 0x8;
label80058430:
  t5 = t7 & s2;
  t5 = t5 & s5;
  temp = (int32_t)t5 > 0;
  sw(s1 + 0x0018, s6);
  if (temp) goto label80058458;
  t5 = lw(a3 + 0x0000);
  sw(a3 + 0x0000, s1);
  sh(t5 + 0x0000, s1);
  t2 = s1 >> 16;
  sb(t5 + 0x0002, t2);
  s1 += 28; // 0x001C
label80058458:
  t6--;
  t1 += 8; // 0x0008
  s3 = s6;
  temp = (int32_t)t6 > 0;
  s2 = s5;
  if (temp) goto label800583B0;
label8005846C:
  t0--;
  temp = (int32_t)t0 > 0;
  a0 += 36; // 0x0024
  if (temp) goto label80058198;
  t5 = allocator1_ptr;
  sw(t5 + 0x0000, s1);
  at = 0x80077DD8;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  return;
}

// size: 0x00000710
void function_80058D64(void)
{
  uint32_t temp;
  at = 0x80077DD8;
  sw(at + 0x0000, s0);
  sw(at + 0x0004, s1);
  sw(at + 0x0008, s2);
  sw(at + 0x000C, s3);
  sw(at + 0x0010, s4);
  sw(at + 0x0014, s5);
  sw(at + 0x0018, s6);
  sw(at + 0x001C, s7);
  sw(at + 0x0020, gp);
  sw(at + 0x0024, sp);
  sw(at + 0x0028, fp);
  sw(at + 0x002C, ra);
  sp = ordered_linked_list;
  sp = lw(sp + 0x0000);
  fp = allocator1_ptr;
  fp = lw(fp + 0x0000);
  ra = 0x800786C8;
  t2 = 0x80076DD0;
  at = lw(ra + 0x0000);
  v0 = lw(ra + 0x0004);
  v1 = lw(ra + 0x0008);
  a0 = lw(t2 + 0x0028);
  a1 = lw(t2 + 0x002C);
  a2 = lw(t2 + 0x0030);
  at -= a0;
  v0 = a1 - v0;
  v1 = a2 - v1;
  a0 = lw(t2 + 0x0000);
  a1 = lw(t2 + 0x0004);
  a2 = lw(t2 + 0x0008);
  a3 = lw(t2 + 0x000C);
  t0 = lw(t2 + 0x0010);
  cop2.RTM0 = a0;
  cop2.RTM1 = a1;
  cop2.RTM2 = a2;
  cop2.RTM3 = a3;
  cop2.RTM4 = t0;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  cop2.IR3 = at;
  cop2.IR1 = v0;
  cop2.IR2 = v1;
  MVMVA(SF_ON, MX_RT, V_IR, CV_NONE, LM_OFF);
  v1 = cop2.MAC3;
  v0 = cop2.MAC2;
  at = cop2.MAC1;
  cop2.TRZ = v1;
  cop2.TRY = v0;
  cop2.TRX = at;
  a3 = lw(ra + 0x00B8);
  t0 = lw(ra + 0x00BC);
  t1 = lw(ra + 0x00C0);
  t2 = lw(ra + 0x00C4);
  t3 = lw(ra + 0x00C8);
  cop2.RTM0 = a3;
  cop2.RTM1 = t0;
  cop2.RTM2 = t1;
  cop2.RTM3 = t2;
  cop2.RTM4 = t3;
  s2 = lw(ra + 0x009C);
  s3 = spyro_sin_lut;
  t9 = 8; // 0x0008
label80058E84:
  temp = t9 == 0;
  t9--;
  if (temp) goto label80059428;
  at = t9 + ra;
  a0 = lb(at + 0x0020);
  s5 = lb(at + 0x0028);
  at = 0x8006D94C; // &0x02000000
  temp = (int32_t)a0 <= 0;
  a0--;
  if (temp) goto label80058E84;
  a0 = a0 << 2;
  a0 += at;
  at = t9 - 4; // 0xFFFFFFFC
  temp = (int32_t)at >= 0;
  at = 192; // 0x00C0
  if (temp) goto label80058EC8;
  v0 = t9;
  v1 = 0;
  goto label80058ED8;
label80058EC8:
  at = 256; // 0x0100
  v0 = t9 - 4; // 0xFFFFFFFC
  v1 = 768; // 0x0300
  a0 += 128; // 0x0080
label80058ED8:
  mult(at, v0);
  t8 = lw(a0 + 0x0000);
  v0=lo;
  v0 += v1;
  s6 = 0x8006DAA8;
  s6 += v0;
  s7 = s6;
  v1 = s6 + at;
  gp = 0;
  at = t8 >> 24;
  v0 = at << 3;
  v0 = v0 & 0x7F8;
  s7 += v0;
  s5 = s5 << 3;
  s5 += s6;
  at = s7 - s5;
  temp = (int32_t)at > 0;
  v1 -= s7;
  if (temp) goto label8005920C;
  temp = (int32_t)v1 <= 0;
  if (temp) goto label8005920C;
  gp = 1; // 0x0001
  v0 = lw(s7 + 0x0000);
  v1 = lw(s7 + 0x0004);
  at = v0 << 16;
  at = (int32_t)at >> 16;
  v0 = (int32_t)v0 >> 16;
  v0 = -v0;
  v0 = v0 & 0xFFFF;
  v1 = v1 << 16;
  v1 = -v1;
  v0 = v0 | v1;
  cop2.VZ0 = at;
  cop2.VXY0 = v0;
  v0 = lw(s7 - 0x0010); // 0xFFFFFFF0
  v1 = lw(s7 - 0x000C); // 0xFFFFFFF4
  RTPS();
  at = v0 << 16;
  at = (int32_t)at >> 16;
  v0 = (int32_t)v0 >> 16;
  temp = s2 == 0;
  a0 = 44; // 0x002C
  if (temp) goto label80058F84;
  a0 = 64; // 0x0040
label80058F84:
  a1 = v1 >> 15;
  a1 = a1 & 0x1FE;
  v1 = v1 << 16;
  v1 = (int32_t)v1 >> 16;
  t1 = s3 + a1;
  t0 = t1 + 128; // 0x0080
  t0 = lh(t0 + 0x0000);
  t1 = lh(t1 + 0x0000);
  s1 = cop2.SZ3;
  t5 = cop2.SXY2;
  mult(a0, t0);
  t0=lo;
  t0 = (int32_t)t0 >> 11;
  mult(a0, t1);
  t1=lo;
  t1 = (int32_t)t1 >> 11;
  a0 = t0 - v0;
  a1 = t1 - v1;
  a2 = t0 << 1;
  a3 = t1 << 1;
  a2 = a0 - a2;
  a3 = a1 - a3;
  temp = s2 == 0;
  t0 = -t0;
  if (temp) goto label80058FF0;
  t0 = (int32_t)t0 >> 1;
  t1 = (int32_t)t1 >> 1;
label80058FF0:
  v0 = t1 - v0;
  v1 = t0 - v1;
  cop2.VZ0 = at;
  cop2.VZ1 = at;
  cop2.VZ2 = at;
  v0 = v0 & 0xFFFF;
  a0 = a0 & 0xFFFF;
  a2 = a2 & 0xFFFF;
  v1 = v1 << 16;
  a1 = a1 << 16;
  a3 = a3 << 16;
  v0 = v0 | v1;
  a0 = a0 | a1;
  a2 = a2 | a3;
  cop2.VXY0 = v0;
  cop2.VXY1 = a0;
  cop2.VXY2 = a2;
  a0 = 0x8006E1A8; // &0x3060F0F0
  RTPT();
  temp = s2 == 0;

  if (temp) goto label8005904C;
  a0 += 16; // 0x0010
label8005904C:
  at = lw(a0 + 0x0000);
  v0 = lw(a0 + 0x0004);
  v1 = lw(a0 + 0x0008);
  a0 = lw(a0 + 0x000C);
  s1 = s1 >> 7;
  s1 -= 2; // 0xFFFFFFFE
  temp = (int32_t)s1 < 0;
  s1 = s1 << 3;
  if (temp) goto label80058E84;
  s1 += sp;
  t6 = cop2.SXY0;
  t7 = cop2.SXY1;
  s0 = cop2.SXY2;
  cop2.SXY0 = t5;
  cop2.SXY1 = t7;
  cop2.SXY2 = t6;
  sw(fp + 0x0008, t5);
  sw(fp + 0x0010, t7);
  NCLIP();
  sw(fp + 0x0018, t6);
  sw(fp + 0x0004, at);
  sw(fp + 0x000C, v1);
  sw(fp + 0x0014, v0);
  a1 = cop2.MAC0;
  a2 = 0x06000000;
  temp = (int32_t)a1 <= 0;
  sw(fp + 0x0000, a2);
  if (temp) goto label800590E0;
  a1 = lw(s1 + 0x0000);
  sw(s1 + 0x0000, fp);
  temp = a1 == 0;
  a2 = fp >> 16;
  if (temp) goto label800590D4;
  sh(a1 + 0x0000, fp);
  sb(a1 + 0x0002, a2);
  fp += 28; // 0x001C
  goto label800590E0;
label800590D4:
  sw(s1 + 0x0004, fp);
  fp += 28; // 0x001C
  goto label800590E0;
label800590E0:
  cop2.SXY0 = t5;
  cop2.SXY1 = t6;
  cop2.SXY2 = s0;
  sw(fp + 0x0008, t5);
  sw(fp + 0x0010, t6);
  NCLIP();
  sw(fp + 0x0018, s0);
  sw(fp + 0x0004, at);
  sw(fp + 0x000C, v0);
  sw(fp + 0x0014, a0);
  a1 = cop2.MAC0;
  a2 = 0x06000000;
  temp = (int32_t)a1 <= 0;
  sw(fp + 0x0000, a2);
  if (temp) goto label80059144;
  a1 = lw(s1 + 0x0000);
  sw(s1 + 0x0000, fp);
  temp = a1 == 0;
  a2 = fp >> 16;
  if (temp) goto label80059138;
  sh(a1 + 0x0000, fp);
  sb(a1 + 0x0002, a2);
  fp += 28; // 0x001C
  goto label80059144;
label80059138:
  sw(s1 + 0x0004, fp);
  fp += 28; // 0x001C
  goto label80059144;
label80059144:
  cop2.SXY0 = t5;
  cop2.SXY1 = s0;
  cop2.SXY2 = t7;
  sw(fp + 0x0008, t5);
  sw(fp + 0x0010, s0);
  NCLIP();
  sw(fp + 0x0018, t7);
  sw(fp + 0x0004, at);
  sw(fp + 0x000C, a0);
  sw(fp + 0x0014, v1);
  a1 = cop2.MAC0;
  a2 = 0x06000000;
  temp = (int32_t)a1 <= 0;
  sw(fp + 0x0000, a2);
  if (temp) goto label800591A8;
  a1 = lw(s1 + 0x0000);
  sw(s1 + 0x0000, fp);
  temp = a1 == 0;
  a2 = fp >> 16;
  if (temp) goto label8005919C;
  sh(a1 + 0x0000, fp);
  sb(a1 + 0x0002, a2);
  fp += 28; // 0x001C
  goto label800591A8;
label8005919C:
  sw(s1 + 0x0004, fp);
  fp += 28; // 0x001C
  goto label800591A8;
label800591A8:
  cop2.SXY0 = t6;
  cop2.SXY1 = t7;
  cop2.SXY2 = s0;
  sw(fp + 0x0008, t6);
  sw(fp + 0x0010, t7);
  NCLIP();
  sw(fp + 0x0018, s0);
  sw(fp + 0x0004, v0);
  sw(fp + 0x000C, v1);
  sw(fp + 0x0014, a0);
  a1 = cop2.MAC0;
  a2 = 0x06000000;
  temp = (int32_t)a1 <= 0;
  sw(fp + 0x0000, a2);
  if (temp) goto label8005920C;
  a1 = lw(s1 + 0x0000);
  sw(s1 + 0x0000, fp);
  temp = a1 == 0;
  a2 = fp >> 16;
  if (temp) goto label80059200;
  sh(a1 + 0x0000, fp);
  sb(a1 + 0x0002, a2);
  fp += 28; // 0x001C
  goto label8005920C;
label80059200:
  sw(s1 + 0x0004, fp);
  fp += 28; // 0x001C
  goto label8005920C;
label8005920C:
  s7 -= 16; // 0xFFFFFFF0
  at = t8 >> 8;
  at = at & 0xFF;
  t8 = t8 >> 16;
  t8 = t8 & 0xFF;
  v0 = s7 - s5;
  temp = (int32_t)v0 <= 0;
  v0 = v0 >> 3;
  if (temp) goto label80059238;
  at += v0;
  t8 -= v0;
  s7 = s5;
label80059238:
  v0 = 5; // 0x0005
  mult(at, v0);
  v0 = 128; // 0x0080
  at=lo;
  v0 -= at;
  v1 = v0 << 8;
  v0 = v0 | v1;
  v1 = v1 << 8;
  v0 = v0 | v1;
  a1 = 0x3E000000;
  a1 = a1 | v0;
  temp = s2 == 0;
  a0 = ra;
  if (temp) goto label80059270;
  a0 += 8; // 0x0008
label80059270:
  v1 = lw(a0 + 0x0010);
  a0 = lw(a0 + 0x0014);
  at = at << 8;
  v1 += at;
  a0 += at;
  s4 = 2560; // 0x0A00
  s5 = 0x000A0A0A;
  at = 0;
  goto label800592B8;
label80059298:
  temp = (int32_t)t8 <= 0;
  t8--;
  if (temp) goto label80058E84;
  temp = (int32_t)t8 > 0;
  t8--;
  if (temp) goto label800592B8;
  s7 += 8; // 0x0008
  s4 = 1280; // 0x0500
  s5 = 0x00050505;
label800592B8:
  a3 = lw(s7 + 0x0000);
  t0 = lw(s7 + 0x0004);
  s7 -= 16; // 0xFFFFFFF0
  a2 = a3 << 16;
  a2 = (int32_t)a2 >> 16;
  a3 = (int32_t)a3 >> 16;
  t2 = t0 >> 15;
  t2 = t2 & 0x1FE;
  t4 = s3 + t2;
  t3 = t4 + 128; // 0x0080
  t3 = lh(t3 + 0x0000);
  t4 = lh(t4 + 0x0000);
  temp = (int32_t)t8 <= 0;
  t1 = 8; // 0x0008
  if (temp) goto label80059308;
  gp += at;
  temp = gp == 0;
  if (temp) goto label80059308;
  temp = s2 == 0;
  t1 = 44; // 0x002C
  if (temp) goto label80059308;
  t1 = 64; // 0x0040
label80059308:
  t0 = t0 << 16;
  t0 = (int32_t)t0 >> 16;
  mult(t1, t3);
  t3=lo;
  t3 = (int32_t)t3 >> 11;
  mult(t1, t4);
  t4=lo;
  t4 = (int32_t)t4 >> 11;
  t1 = t3 - a3;
  t2 = t4 - t0;
  t3 = t3 << 1;
  t4 = t4 << 1;
  t3 = t1 - t3;
  t4 = t2 - t4;
  t1 = t1 & 0xFFFF;
  t3 = t3 & 0xFFFF;
  t2 = t2 << 16;
  t4 = t4 << 16;
  t1 = t1 | t2;
  t3 = t3 | t4;
  cop2.VZ0 = a2;
  cop2.VZ1 = a2;
  cop2.VXY0 = t1;
  cop2.VXY1 = t3;
  RTPT();
  sw(fp + 0x0008, at);
  sw(fp + 0x0014, v0);
  sw(fp + 0x000C, v1);
  sw(fp + 0x0018, a0);
  sw(fp + 0x0004, a1);
  sw(fp + 0x0010, a1);
  a2 = 0x0C000000;
  sw(fp + 0x0000, a2);
  a2 = cop2.SXY0;
  a3 = cop2.SXY1;
  t0 = cop2.SZ1;
  t1 = cop2.SZ2;
  temp = at == 0;
  t0 += t1;
  if (temp) goto label8005941C;
  t0 = t0 >> 8;
  sw(fp + 0x0020, a2);
  sw(fp + 0x002C, a3);
  at = a2;
  v0 = a3;
  v1 += s4;
  sw(fp + 0x0024, v1);
  a0 += s4;
  sw(fp + 0x0030, a0);
  a1 -= s5;
  sw(fp + 0x001C, a1);
  sw(fp + 0x0028, a1);
  t0 -= 2; // 0xFFFFFFFE
  temp = (int32_t)t0 < 0;
  t0 = t0 << 3;
  if (temp) goto label80059298;
  t0 += sp;
  a2 = lw(t0 + 0x0000);
  sw(t0 + 0x0000, fp);
  temp = a2 == 0;
  a3 = fp >> 16;
  if (temp) goto label80059410;
  sh(a2 + 0x0000, fp);
  sb(a2 + 0x0002, a3);
  fp += 52; // 0x0034
  goto label80059298;
label80059410:
  sw(t0 + 0x0004, fp);
  fp += 52; // 0x0034
  goto label80059298;
label8005941C:
  at = a2;
  v0 = a3;
  goto label80059298;
label80059428:
  at = allocator1_ptr;
  sw(at + 0x0000, fp);
  at = 0x80077DD8;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  return;
}

// size: 0x00000264
void function_8004FF90(void)
{
  uint32_t temp;
label8004FF90:
  a0 = lw(t2 + 0x0000);
  a1 = lw(t2 + 0x0008);
  temp = a0 == 0;
  a0 = lw(t2 + 0x0004);
  if (temp) goto label800501B8;
  cop2.SXY1 = a1;
  cop2.SXY0 = a0;
  cop2.SXY2 = t4;
  NCLIP();
  a0 = cop2.MAC0;
  temp = (int32_t)a0 > 0;
  a1 = 0;
  if (temp) goto label8004FFCC;
  a1 = a1 | 0x8;
label8004FFCC:
  cop2.SXY2 = t5;
  NCLIP();
  a0 = cop2.MAC0;
  temp = (int32_t)a0 > 0;
  if (temp) goto label8004FFF0;
  a1 = a1 | 0x10;
label8004FFF0:
  cop2.SXY2 = t6;
  NCLIP();
  a0 = cop2.MAC0;
  temp = (int32_t)a0 > 0;
  if (temp) goto label80050014;
  a1 = a1 | 0x20;
label80050014:
  a0 = 0x80050028; // &0x08013FE4
  a0 += a1;
  temp = a0;
  switch (temp)
  {
  case 0x80050028:
    goto label80050028;
    break;
  case 0x80050030:
    goto label80050030;
    break;
  case 0x80050038:
    goto label80050038;
    break;
  case 0x80050040:
    goto label80050040;
    break;
  case 0x80050048:
    goto label80050048;
    break;
  case 0x80050050:
    goto label80050050;
    break;
  case 0x80050058:
    goto label80050058;
    break;
  case 0x80050060:
    goto label80050060;
    break;
  default:
    JR(temp, 0x80050020);
    return;
  }
label80050028:
  t2 += 24; // 0x0018
  goto label8004FF90;
label80050030:
  goto label80050068;
label80050038:
  goto label800500A8;
label80050040:
  goto label800500E8;
label80050048:
  goto label80050118;
label80050050:
  goto label80050158;
label80050058:
  goto label80050188;
label80050060:
  return;
label80050068:
  s0 = ra;
  a0 = t5;
  a1 = t4;
  function_8004FDAC();
  a0 = t6;
  a1 = t4;
  t4 = at;
  function_8004FDAC();
  sw(s1 + 0x0000, t4);
  sw(s1 + 0x0004, t6);
  sw(s1 + 0x0008, at);
  t2 += 24; // 0x0018
  sw(s1 + 0x000C, t2);
  s1 += 16; // 0x0010
  ra = s0;
  goto label8004FF90;
label800500A8:
  s0 = ra;
  a0 = t4;
  a1 = t5;
  function_8004FDAC();
  a0 = t6;
  a1 = t5;
  t5 = at;
  function_8004FDAC();
  sw(s1 + 0x0000, t5);
  sw(s1 + 0x0008, t6);
  sw(s1 + 0x0004, at);
  t2 += 24; // 0x0018
  sw(s1 + 0x000C, t2);
  s1 += 16; // 0x0010
  ra = s0;
  goto label8004FF90;
label800500E8:
  s0 = ra;
  a0 = t6;
  a1 = t4;
  function_8004FDAC();
  a0 = t6;
  a1 = t5;
  t4 = at;
  function_8004FDAC();
  t5 = at;
  ra = s0;
  t2 += 24; // 0x0018
  goto label8004FF90;
label80050118:
  s0 = ra;
  a0 = t4;
  a1 = t6;
  function_8004FDAC();
  a0 = t5;
  a1 = t6;
  t6 = at;
  function_8004FDAC();
  sw(s1 + 0x0000, t6);
  sw(s1 + 0x0004, t5);
  sw(s1 + 0x0008, at);
  t2 += 24; // 0x0018
  sw(s1 + 0x000C, t2);
  s1 += 16; // 0x0010
  ra = s0;
  goto label8004FF90;
label80050158:
  s0 = ra;
  a0 = t5;
  a1 = t4;
  function_8004FDAC();
  a0 = t5;
  a1 = t6;
  t4 = at;
  function_8004FDAC();
  t6 = at;
  ra = s0;
  t2 += 24; // 0x0018
  goto label8004FF90;
label80050188:
  s0 = ra;
  a0 = t4;
  a1 = t5;
  function_8004FDAC();
  a0 = t4;
  a1 = t6;
  t5 = at;
  function_8004FDAC();
  t6 = at;
  ra = s0;
  t2 += 24; // 0x0018
  goto label8004FF90;
label800501B8:
  sw(s2 + 0x0000, s4);
  sw(s2 + 0x0004, t1);
  sw(s2 + 0x0008, t4);
  sw(s2 + 0x000C, t5);
  a0 = lw(s3 + 0x0000);
  sw(s2 + 0x0010, t6);
  temp = a0 == 0;
  sw(s3 + 0x0000, s2);
  if (temp) goto label800501E8;
  sh(a0 + 0x0000, s2);
  a1 = s2 >> 16;
  sb(a0 + 0x0002, a1);
  goto label800501EC;
label800501E8:
  sw(s3 + 0x0004, s2);
label800501EC:
  s2 += 20; // 0x0014
  return;
}

// size: 0x0000141C
void function_80050BD0(void)
{
  uint32_t temp;
  v0 = lw(0x80075858);
  v1 = lw(0x800758FC);
  sp -= 416; // 0xFFFFFE60
  sw(sp + 0x019C, ra);
  sw(sp + 0x0198, fp);
  sw(sp + 0x0194, s7);
  sw(sp + 0x0190, s6);
  sw(sp + 0x018C, s5);
  sw(sp + 0x0188, s4);
  sw(sp + 0x0184, s3);
  sw(sp + 0x0180, s2);
  sw(sp + 0x017C, s1);
  sw(sp + 0x0178, s0);
  v0 += 2; // 0x0002
  v0 = v0 & 0xFFF;
  v1 += 2; // 0x0002
  v1 = v1 & 0xFFF;
  sw(0x80075858, v0);
  v0 = (int32_t)v1 < 2049;
  sw(0x800758FC, v1);
  temp = v0 != 0;
  v0 = v1 - 4096; // 0xFFFFF000
  if (temp) goto label80050C40;
  sw(0x800758FC, v0);
label80050C40:
  v0 = lw(0x800758FC);
  v0 = (int32_t)v0 < 129;
  temp = v0 != 0;
  v0 = 128; // 0x0080
  if (temp) goto label80050C60;
  sw(0x800758FC, v0);
label80050C60:
  a0 = sp + 16; // 0x0010
  a1 = 0;
  a2 = 32; // 0x0020
  spyro_memset32(a0, a1, a2);
  a0 = sp + 16; // 0x0010
  mat3x4setTR(a0);
  s0 = 0x80076E1E;
  v0 = lh(s0 + 0x0000);
  a0 = lw(0x800758FC);
  s1 = 4096; // 0x1000
  sh(sp + 0x0010, s1);
  a0 = v0 - a0;
  v0 = spyro_cos(a0);
  v1 = lh(s0 + 0x0000);
  a0 = lw(0x800758FC);
  sh(sp + 0x0018, v0);
  a0 = v1 - a0;
  v0 = spyro_sin(a0);
  v1 = lh(s0 + 0x0000);
  a0 = lw(0x800758FC);
  sh(sp + 0x001E, v0);
  a0 = v1 - a0;
  v0 = spyro_sin(a0);
  v1 = lh(s0 + 0x0000);
  a0 = lw(0x800758FC);
  v0 = -v0;
  sh(sp + 0x001A, v0);
  a0 = v1 - a0;
  v0 = spyro_cos(a0);
  s0 = sp + 48; // 0x0030
  a0 = s0;
  a1 = 0;
  a2 = 32; // 0x0020
  sh(sp + 0x0020, v0);
  spyro_memset32(a0, a1, a2);
  v0 = lh(0x80076E20);
  a0 = lw(0x80075858);
  a0 += v0;
  v0 = spyro_cos(a0);
  v1 = lh(0x80076E20);
  a0 = lw(0x80075858);
  sh(sp + 0x0030, v0);
  a0 += v1;
  v0 = spyro_sin(a0);
  v1 = lh(0x80076E20);
  a0 = lw(0x80075858);
  v0 = -v0;
  sh(sp + 0x003C, v0);
  sh(sp + 0x0038, s1);
  a0 += v1;
  v0 = spyro_sin(a0);
  v1 = lh(0x80076E20);
  a0 = lw(0x80075858);
  sh(sp + 0x0034, v0);
  a0 += v1;
  v0 = spyro_cos(a0);
  a0 = sp + 16; // 0x0010
  a1 = s0;
  sh(sp + 0x0040, v0);
  v0 = spyro_mat_mul_2(a0, a1);
  a0 = s0;
  a1 = 0;
  a2 = 32; // 0x0020
  spyro_memset32(a0, a1, a2);
  a0 = lh(0x80076E1C);
  v0 = spyro_cos(a0);
  a0 = lh(0x80076E1C);
  sh(sp + 0x0030, v0);
  v0 = spyro_sin(a0);
  a0 = lh(0x80076E1C);
  v0 = -v0;
  sh(sp + 0x0036, v0);
  v0 = spyro_sin(a0);
  a0 = lh(0x80076E1C);
  sh(sp + 0x0032, v0);
  v0 = spyro_cos(a0);
  a0 = sp + 16; // 0x0010
  a1 = s0;
  sh(sp + 0x0038, v0);
  sh(sp + 0x0040, s1);
  v0 = spyro_mat_mul_2(a0, a1);
  a0 = s0;
  a1 = sp + 16; // 0x0010
  a2 = 20; // 0x0014
  spyro_memcpy32(a0, a1, a2);
  v1 = lh(sp + 0x0016);
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 6;
  temp = (int32_t)v0 >= 0;
  if (temp) goto label80050E1C;
  v0 += 511; // 0x01FF
label80050E1C:
  v1 = lh(sp + 0x0018);
  v0 = (int32_t)v0 >> 9;
  sh(sp + 0x0036, v0);
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 6;
  temp = (int32_t)v0 >= 0;
  if (temp) goto label80050E40;
  v0 += 511; // 0x01FF
label80050E40:
  v1 = lh(sp + 0x001A);
  v0 = (int32_t)v0 >> 9;
  sh(sp + 0x0038, v0);
  v0 = v1 << 2;
  v0 += v1;
  v0 = v0 << 6;
  temp = (int32_t)v0 >= 0;
  if (temp) goto label80050E64;
  v0 += 511; // 0x01FF
label80050E64:
  v1 = lw(0x800758BC);
  v0 = (int32_t)v0 >> 9;
  sw(sp + 0x0108, 0);
  temp = (int32_t)v1 <= 0;
  sh(sp + 0x003A, v0);
  if (temp) goto label80051F88;
  t4 = sp + 80; // 0x0050
  s5 = 0x80078640;
  sw(sp + 0x0140, t4);
  sw(sp + 0x0170, 0);
label80050E90:
  v0 = lw(s5 + 0x0000);
  v0 = lw(v0 + 0x0014);
  temp = (int32_t)v0 < 0;
  if (temp) goto label80050EC0;
  v0 = lbu(0x800771C8 + v0);
  temp = v0 == 0;
  if (temp) goto label80051F5C;
label80050EC0:
  v0 = lw(0x800758C8);
  t4 = 1; // 0x0001
  sw(sp + 0x0118, t4);
  sw(sp + 0x0110, t4);
  t4 = lw(sp + 0x0108);
  v0 = v0 << 4;
  a0 = t4 << 9;
  a0 += v0;
  v0 = spyro_cos(a0);
  a1 = 0x00FF0000;
  v1 = lw(s5 + 0x0000);
  temp = (int32_t)v0 >= 0;
  a2 = v0;
  if (temp) goto label80050EFC;
  a2 = -a2;
label80050EFC:
  v0 = lw(v1 + 0x0000);
  a2 = (int32_t)a2 >> 1;
  sw(sp + 0x00B8, a2);
  a0 = lw(v0 + 0x0010);
  a1 = a1 | 0xFFFF;
  v0 = interpolate_color(a0, a1, a2);
  a0 = lw(sp + 0x0140);
  a1 = lw(s5 + 0x0000);
  a2 = 0x80076DF8;
  sw(sp + 0x00B8, v0);
  a1 += 32; // 0x0020
  spyro_vec3_sub(a0, a1, a2);
  v1 = lw(sp + 0x0050);
  v0 = lw(sp + 0x0054);
  a0 = lw(sp + 0x0058);
  temp = (int32_t)v1 >= 0;
  if (temp) goto label80050F48;
  v1 = -v1;
label80050F48:
  temp = (int32_t)v0 >= 0;
  if (temp) goto label80050F54;
  v0 = -v0;
label80050F54:
  v1 += v0;
  temp = (int32_t)a0 >= 0;
  if (temp) goto label80050F64;
  a0 = -a0;
label80050F64:
  a0 += v1;
  v0 = spyro_log2_uint(a0);
  s4 = v0;
  v0 = (int32_t)s4 < 15;
  temp = v0 != 0;
  s4 = (int32_t)s4 >> 1;
  if (temp) goto label80050F9C;
  a0 = lw(sp + 0x0140);
  a1 = s4;
  spyro_vec3_shift_right(a0, a1);
  a0 = lw(sp + 0x0140);
  a1 = 1; // 0x0001
  v0 = spyro_vec_length(a0, a1);
  v0 = v0 << s4;
  goto label80050FA8;
label80050F9C:
  a0 = lw(sp + 0x0140);
  a1 = 1; // 0x0001
  v0 = spyro_vec_length(a0, a1);
label80050FA8:
  sw(sp + 0x0138, v0);
  a0 = lw(sp + 0x0138);
  v0 = spyro_log2_uint(a0);
  s4 = v0 - 13; // 0xFFFFFFF3
  temp = (int32_t)s4 >= 0;
  if (temp) goto label80050FC8;
  s4 = 0;
label80050FC8:
  v0 = lw(0x8007591C);
  temp = v0 == 0;
  if (temp) goto label8005102C;
  t4 = lw(sp + 0x0138);
  v0 = (int32_t)t4 < 16385;
  temp = v0 != 0;
  v0 = 0 | 0x8000;
  if (temp) goto label8005102C;
  v0 -= t4;
  s2 = (int32_t)v0 >> 2;
  temp = (int32_t)s2 >= 0;
  v0 = (int32_t)s2 < 4097;
  if (temp) goto label80051008;
  s2 = 0;
  v0 = (int32_t)s2 < 4097;
label80051008:
  temp = v0 != 0;
  if (temp) goto label80051014;
  s2 = 4096; // 0x1000
label80051014:
  a0 = lw(SKYBOX_DATA + 0x0010);
  a1 = lw(sp + 0x00B8);
  a2 = s2;
  v0 = interpolate_color(a0, a1, a2);
  sw(sp + 0x00B8, v0);
label8005102C:
  a0 = sp + 96; // 0x0060
  spyro_vec3_clear(a0);
  v0 = lw(s5 + 0x0000);
  v0 = lw(v0 + 0x0004);
  temp = (int32_t)v0 <= 0;
  s2 = 0;
  if (temp) goto label800510A4;
  s3 = sp + 96; // 0x0060
  s0 = 32; // 0x0020
  s1 = 0x80078DD8;
label8005105C:
  a0 = s1;
  a2 = s4;
  a1 = lw(s5 + 0x0000);
  s1 += 12; // 0x000C
  a1 += s0;
  spyro_world_to_screen_projection_with_right_shift(a0, a1, a2);
  a0 = s3;
  a2 = lw(s5 + 0x0000);
  a1 = s3;
  a2 += s0;
  spyro_vec3_add(a0, a1, a2);
  v0 = lw(s5 + 0x0000);
  s2++;
  v0 = lw(v0 + 0x0004);
  v0 = (int32_t)s2 < (int32_t)v0;
  temp = v0 != 0;
  s0 += 12; // 0x000C
  if (temp) goto label8005105C;
label800510A4:
  v0 = lw(s5 + 0x0000);
  v1 = lw(sp + 0x0060);
  v0 = lw(v0 + 0x0004);
  div_psx(v1,v0);
  temp = v0 != 0;
  if (temp) goto label800510C4;
  BREAKPOINT; // BREAK 0x01C00
label800510C4:
  at = -1; // 0xFFFFFFFF
  temp = v0 != at;
  at = 0x80000000;
  if (temp) goto label800510DC;
  temp = v1 != at;
  if (temp) goto label800510DC;
  BREAKPOINT; // BREAK 0x01800
label800510DC:
  v1=lo;
  sw(sp + 0x0060, v1);
  v0 = lw(s5 + 0x0000);
  v1 = lw(sp + 0x0064);
  v0 = lw(v0 + 0x0004);
  div_psx(v1,v0);
  temp = v0 != 0;
  if (temp) goto label80051108;
  BREAKPOINT; // BREAK 0x01C00
label80051108:
  at = -1; // 0xFFFFFFFF
  temp = v0 != at;
  at = 0x80000000;
  if (temp) goto label80051120;
  temp = v1 != at;
  if (temp) goto label80051120;
  BREAKPOINT; // BREAK 0x01800
label80051120:
  v1=lo;
  sw(sp + 0x0064, v1);
  v0 = lw(s5 + 0x0000);
  v1 = lw(sp + 0x0068);
  v0 = lw(v0 + 0x0004);
  div_psx(v1,v0);
  temp = v0 != 0;
  if (temp) goto label8005114C;
  BREAKPOINT; // BREAK 0x01C00
label8005114C:
  at = -1; // 0xFFFFFFFF
  temp = v0 != at;
  at = 0x80000000;
  if (temp) goto label80051164;
  temp = v1 != at;
  if (temp) goto label80051164;
  BREAKPOINT; // BREAK 0x01800
label80051164:
  v1=lo;
  a1 = 0x80076DF8;
  sw(sp + 0x0068, v1);
  a2 = lw(s5 + 0x0000);
  a0 = lw(sp + 0x0140);
  a2 += 44; // 0x002C
  spyro_vec3_sub(a0, a1, a2);
  a0 = lw(sp + 0x0140);
  a1 = s4;
  spyro_vec3_shift_right(a0, a1);
  a0 = lw(s5 + 0x0000);
  v0 = lw(sp + 0x0050);
  v1 = lw(a0 + 0x0008);
  v0 = -v0;
  mult(v0, v1);
  v1 = lw(sp + 0x0054);
  a2=lo;
  v0 = lw(a0 + 0x000C);
  mult(v1, v0);
  fp = 5112; // 0x13F8
  s6 = 240; // 0x00F0
  s7 = 0;
  v1 = lw(sp + 0x0058);
  a1=lo;
  v0 = lw(a0 + 0x0010);
  s1 = 0;
  mult(v1, v0);
  s2 = 0;
  s3 = 0;
  s4 = 0;
  sw(0x80075934, 0);
  sw(sp + 0x0128, 0);
  sw(sp + 0x0130, 0);
  v0 = 0x8006FCF4 + 0x0800;
  sw(0x80075798, v0);
  v0 = lw(s5 + 0x0000);
  v1 = a2 - a1;
  v0 = lw(v0 + 0x0004);
  a3=lo;
  v1 -= a3;
  temp = (int32_t)v0 <= 0;
  sw(sp + 0x0120, v1);
  if (temp) goto label8005155C;
  t3 = s5;
  t0 = 0;
  t2 = 0x80077EAC;
  t1 = 0;
label80051234:
  t4 = lw(sp + 0x0120);
  temp = (int32_t)t4 <= 0;
  a1 = s2 + 1; // 0x0001
  if (temp) goto label80051264;
  v0 = lw(t3 + 0x0000);
  v0 = lw(v0 + 0x0004);
  temp = a1 != v0;
  v0 = a1 << 1;
  if (temp) goto label80051288;
  a1 = 0;
  goto label80051284;
label80051264:
  a1 = s2 - 1; // 0xFFFFFFFF
  temp = (int32_t)a1 >= 0;
  v0 = a1 << 1;
  if (temp) goto label80051288;
  v0 = lw(t3 + 0x0000);
  v0 = lw(v0 + 0x0004);
  a1 = v0 - 1; // 0xFFFFFFFF
label80051284:
  v0 = a1 << 1;
label80051288:
  v0 += a1;
  a2 = v0 << 2;
  v0 = lw(0x80078DD8 + a2);
  t4 = lw(sp + 0x0128);
  v1 = lw(0x80078DDC + a2);
  t4 += v0;
  sw(sp + 0x0128, t4);
  t4 = lw(sp + 0x0130);
  t4 += v1;
  v1 = lw(0x80075934);
  sw(sp + 0x0130, t4);
  a0 = lw(0x80078DE0 + a2);
  v0 = lw(0x80078DD8 + a2);
  v1 += a0;
  v0--;
  v0 = v0 < 511;
  sw(0x80075934, v1);
  temp = v0 == 0;
  v0 = a1 << 1;
  if (temp) goto label8005133C;
  v0 = lw(0x80078DDC + a2);
  v0--;
  v0 = v0 < 239;
  temp = v0 == 0;
  v0 = a1 << 1;
  if (temp) goto label8005133C;
  v0 = lw(0x80078DE0 + a2);
  temp = (int32_t)v0 <= 0;
  v0 = a1 << 1;
  if (temp) goto label8005133C;
  s1 = 1; // 0x0001
label8005133C:
  v0 += a1;
  a3 = v0 << 2;
  a0 = lw(0x80078DD8 + a3);
  v0 = (int32_t)s4 < (int32_t)a0;
  temp = v0 == 0;
  v0 = (int32_t)a0 < (int32_t)fp;
  if (temp) goto label80051364;
  s4 = a0;
label80051364:
  temp = v0 == 0;
  if (temp) goto label80051370;
  fp = a0;
label80051370:
  v1 = lw(0x80078DDC + a3);
  v0 = (int32_t)s7 < (int32_t)v1;
  temp = v0 == 0;
  v0 = (int32_t)v1 < (int32_t)s6;
  if (temp) goto label80051390;
  s7 = v1;
label80051390:
  temp = v0 == 0;
  if (temp) goto label8005139C;
  s6 = v1;
label8005139C:
  a2 = lw(0x80078DD8 + t0);
  v0 = (int32_t)a2 < 512;
  temp = v0 != 0;
  v0 = (int32_t)a0 < 512;
  if (temp) goto label800513C0;
  temp = v0 == 0;
  if (temp) goto label80051540;
label800513C0:
  temp = (int32_t)a2 > 0;
  if (temp) goto label800513D0;
  temp = (int32_t)a0 <= 0;
  if (temp) goto label80051540;
label800513D0:
  a1 = lw(0x80078DDC + t0);
  v0 = (int32_t)a1 < 240;
  temp = v0 != 0;
  v0 = (int32_t)v1 < 240;
  if (temp) goto label800513F4;
  temp = v0 == 0;
  if (temp) goto label80051540;
label800513F4:
  temp = (int32_t)a1 > 0;
  v0 = a2 - a0;
  if (temp) goto label80051404;
  temp = (int32_t)v1 <= 0;
  if (temp) goto label80051540;
label80051404:
  temp = (int32_t)v0 >= 0;
  if (temp) goto label80051410;
  v0 = -v0;
label80051410:
  v0 = (int32_t)v0 < 3;
  temp = v0 != 0;
  v0 = a1 - v1;
  if (temp) goto label80051420;
  sw(sp + 0x0110, 0);
label80051420:
  temp = (int32_t)v0 >= 0;
  if (temp) goto label8005142C;
  v0 = -v0;
label8005142C:
  v0 = (int32_t)v0 < 3;
  temp = v0 != 0;
  v0 = 1; // 0x0001
  if (temp) goto label8005143C;
  sw(sp + 0x0118, 0);
label8005143C:
  sw(0x80077EA0 + t1, v0);
  v0 = lw(0x80078DDC + t0);
  v1 = lhu(0x80078DD8 + t0);
  v0 = v0 << 16;
  v0 = v0 | v1;
  sw(0x80077EA4 + t1, v0);
  v0 = lw(0x80078DDC + a3);
  v1 = lhu(0x80078DD8 + a3);
  v0 = v0 << 16;
  v0 = v0 | v1;
  sw(0x80077EA8 + t1, v0);
  v0 = lw(0x80078DDC + t0);
  v1 = lw(0x80078DDC + a3);
  v0 -= v1;
  sw(t2 + 0x0000, v0);
  a0 = lw(0x80078DD8 + a3);
  v0 = lw(0x80078DD8 + t0);
  a0 -= v0;
  sw(0x80077EB0 + t1, a0);
  v0 = lw(t2 + 0x0000);
  v1 = lw(0x80078DD8 + t0);
  v0 = -v0;
  mult(v0, v1);
  v1=lo;
  v0 = lw(0x80078DDC + t0);
  mult(a0, v0);
  s3++;
  t2 += 24; // 0x0018
  a3=lo;
  v0 = v1 - a3;
  sw(0x80077EB4 + t1, v0);
  t1 += 24; // 0x0018
label80051540:
  v0 = lw(t3 + 0x0000);
  v0 = lw(v0 + 0x0004);
  s2++;
  v0 = (int32_t)s2 < (int32_t)v0;
  temp = v0 != 0;
  t0 += 12; // 0x000C
  if (temp) goto label80051234;
label8005155C:
  t4 = lw(sp + 0x0110);
  temp = t4 != 0;
  if (temp) goto label8005157C;
  t4 = lw(sp + 0x0118);
  temp = t4 == 0;
  if (temp) goto label80051584;
label8005157C:
  temp = s3 != 0;
  if (temp) goto label80051F5C;
label80051584:
  v0 = lw(s5 + 0x0000);
  t4 = lw(sp + 0x0128);
  v0 = lw(v0 + 0x0004);
  div_psx(t4,v0);
  temp = v0 != 0;
  if (temp) goto label800515A4;
  BREAKPOINT; // BREAK 0x01C00
label800515A4:
  at = -1; // 0xFFFFFFFF
  temp = v0 != at;
  at = 0x80000000;
  if (temp) goto label800515BC;
  temp = t4 != at;
  if (temp) goto label800515BC;
  BREAKPOINT; // BREAK 0x01800
label800515BC:
  t4=lo;
  v1 = lw(0x80075934);
  div_psx(v1,v0);
  temp = v0 != 0;
  if (temp) goto label800515DC;
  BREAKPOINT; // BREAK 0x01C00
label800515DC:
  at = -1; // 0xFFFFFFFF
  temp = v0 != at;
  at = 0x80000000;
  if (temp) goto label800515F4;
  temp = v1 != at;
  if (temp) goto label800515F4;
  BREAKPOINT; // BREAK 0x01800
label800515F4:
  v1=lo;
  sw(sp + 0x0128, t4);
  t4 = lw(sp + 0x0130);
  div_psx(t4,v0);
  temp = v0 != 0;
  if (temp) goto label80051614;
  BREAKPOINT; // BREAK 0x01C00
label80051614:
  at = -1; // 0xFFFFFFFF
  temp = v0 != at;
  at = 0x80000000;
  if (temp) goto label8005162C;
  temp = t4 != at;
  if (temp) goto label8005162C;
  BREAKPOINT; // BREAK 0x01800
label8005162C:
  t4=lo;
  v1 = (int32_t)v1 >> 7;
  v0 = (int32_t)v1 < 256;
  sw(0x80075934, v1);
  temp = v0 != 0;
  sw(sp + 0x0130, t4);
  if (temp) goto label80051654;
  v0 = v1 + 64; // 0x0040
  sw(0x80075934, v0);
label80051654:
  v0 = lw(0x80075934);
  v0 = (int32_t)v0 < 2048;
  temp = v0 != 0;
  v0 = (int32_t)fp < 512;
  if (temp) goto label8005167C;
  v0 = 2047; // 0x07FF
  sw(0x80075934, v0);
  v0 = (int32_t)fp < 512;
label8005167C:
  temp = v0 == 0;
  v0 = (int32_t)s6 < 240;
  if (temp) goto label800516B8;
  temp = v0 == 0;
  if (temp) goto label800516B8;
  temp = (int32_t)s4 <= 0;
  if (temp) goto label800516B8;
  temp = (int32_t)s7 <= 0;
  if (temp) goto label800516B8;
  v0 = lw(0x80075934);
  temp = (int32_t)v0 <= 0;
  if (temp) goto label800516B8;
  temp = s3 != 0;
  if (temp) goto label800516EC;
label800516B8:
  temp = (int32_t)fp > 0;
  if (temp) goto label800516F4;
  temp = (int32_t)s6 > 0;
  v0 = (int32_t)s4 < 512;
  if (temp) goto label800516F4;
  temp = v0 != 0;
  v0 = (int32_t)s7 < 240;
  if (temp) goto label800516F4;
  temp = v0 != 0;
  if (temp) goto label800516F4;
  v0 = lw(0x80075934);
  temp = (int32_t)v0 <= 0;
  if (temp) goto label800516F4;
label800516EC:
  s1 = s1 | 0x2;
  goto label800516F8;
label800516F4:
  s1 = 0;
label800516F8:
  v0 = 2; // 0x0002
  temp = s1 != v0;
  if (temp) goto label800517DC;
  t4 = lw(sp + 0x0138);
  v0 = (int32_t)t4 < 4096;
  temp = v0 == 0;
  a0 = sp + 128; // 0x0080
  if (temp) goto label800517DC;
  a2 = 0x80076DF8;
  a1 = sp + 96; // 0x0060
  spyro_vec3_sub(a0, a1, a2);
  a0 = sp + 144; // 0x0090
  s0 = sp + 152; // 0x0098
  a1 = s0;
  a3 = lhu(0x80076E1C);
  v0 = 4096; // 0x1000
  sw(sp + 0x0070, v0);
  v0 = lhu(0x80076E1E);
  v1 = lhu(0x80076E20);
  a2 = 0;
  sw(sp + 0x0074, 0);
  sw(sp + 0x0078, 0);
  a3 = a3 >> 4;
  v0 = v0 >> 4;
  v1 = v1 >> 4;
  sb(sp + 0x0090, a3);
  sb(sp + 0x0091, v0);
  sb(sp + 0x0092, v1);
  spyro_mat3_rotation(a0, a1, a2);
  a0 = s0;
  a1 = sp + 112; // 0x0070
  a2 = a1;
  spyro_set_mat_mirrored_vec_multiply(a0, a1, a2);
  v1 = lw(sp + 0x0070);
  v0 = lw(sp + 0x0080);
  mult(v1, v0);
  v1 = lw(sp + 0x0074);
  a1=lo;
  v0 = lw(sp + 0x0084);
  mult(v1, v0);
  v1 = lw(sp + 0x0078);
  a0=lo;
  v0 = lw(sp + 0x0088);
  mult(v1, v0);
  v0 = a1 + a0;
  v1=lo;
  v0 += v1;
  temp = (int32_t)v0 >= 0;
  if (temp) goto label800517DC;
  s1 = 0;
label800517DC:
  temp = s1 == 0;
  if (temp) goto label800518CC;
  temp = (int32_t)fp >= 0;
  if (temp) goto label800517F0;
  fp = 0;
label800517F0:
  temp = (int32_t)s6 >= 0;
  v0 = (int32_t)s4 < 513;
  if (temp) goto label800517FC;
  s6 = 0;
label800517FC:
  temp = v0 != 0;
  v0 = (int32_t)s7 < 241;
  if (temp) goto label80051808;
  s4 = 512; // 0x0200
label80051808:
  temp = v0 != 0;
  v0 = s6 << 16;
  if (temp) goto label80051814;
  s7 = 240; // 0x00F0
label80051814:
  sw(0x8007AA00, v0);
  v0 = s7 << 16;
  sw(0x8007AA04, v0);
  v0 = s4 << 16;
  sw(0x8007AA08, v0);
  v0 = fp << 16;
  sw(0x8007AA0C, v0);
  v0 = s3 << 1;
  v0 += s3;
  v0 = v0 << 3;
  sw(0x80077EA0 + v0, 0);
  t4 = lw(sp + 0x0138);
  v0 = (int32_t)t4 < 16384;
  temp = v0 == 0;
  v0 = (int32_t)t4 < 12289;
  if (temp) goto label800518C0;
  temp = v0 != 0;
  a2 = t4 - 12288; // 0xFFFFD000
  if (temp) goto label800518AC;
  v0 = lw(s5 + 0x0000);
  a1 = lw(sp + 0x00B8);
  v0 = lw(v0 + 0x0000);
  sw(0x8007575C, a2);
  a0 = lw(v0 + 0x0010);
  v0 = interpolate_color(a0, a1, a2);
  a0 = 0x800757D4;
  sw(0x800757D4, v0);
  goto label800518C4;
label800518AC:
  v0 = lw(s5 + 0x0000);
  a0 = lw(v0 + 0x0000);
  a0 += 16; // 0x0010
  goto label800518C4;
label800518C0:
  a0 = sp + 184; // 0x00B8
label800518C4:
  function_8004FEA0();
label800518CC:
  t4 = lw(sp + 0x0138);
  v0 = (int32_t)t4 < 16384;
  temp = v0 == 0;
  fp = 512; // 0x0200
  if (temp) goto label80051F5C;
  s4 = 0;
  s6 = 240; // 0x00F0
  s7 = 0;
  s1 = 0;
  s2 = 0;
  v0 = lw(s5 + 0x0000);
  v1 = lw(v0 + 0x0004);
  v0 = 0x8006FCF4 + 0x0800;
  sw(0x80075798, v0);
  temp = (int32_t)v1 <= 0;
  s3 = 0;
  if (temp) goto label80051A04;
  v0 = 0x80078640;
  t4 = lw(sp + 0x0170);
  a1 = 0x80078DD8;
  a0 = 0;
  a2 = t4 + v0;
label80051934:
  v1 = lw(a1 + 0x0000);
  t4 = lw(sp + 0x0128);
  v0 = (int32_t)t4 < (int32_t)v1;
  temp = v0 != 0;
  v0 = v1 + 2; // 0x0002
  if (temp) goto label80051958;
  v0 = (int32_t)v1 < (int32_t)t4;
  temp = v0 == 0;
  v0 = v1 - 2; // 0xFFFFFFFE
  if (temp) goto label8005195C;
label80051958:
  sw(a1 + 0x0000, v0);
label8005195C:
  v1 = lw(0x80078DDC + a0);
  t4 = lw(sp + 0x0130);
  v0 = (int32_t)t4 < (int32_t)v1;
  temp = v0 != 0;
  v0 = v1 + 2; // 0x0002
  if (temp) goto label80051988;
  v0 = (int32_t)v1 < (int32_t)t4;
  temp = v0 == 0;
  v0 = v1 - 2; // 0xFFFFFFFE
  if (temp) goto label80051994;
label80051988:
  sw(0x80078DDC + a0, v0);
label80051994:
  v1 = lw(a1 + 0x0000);
  v0 = (int32_t)s4 < (int32_t)v1;
  temp = v0 == 0;
  v0 = (int32_t)v1 < (int32_t)fp;
  if (temp) goto label800519AC;
  s4 = v1;
label800519AC:
  temp = v0 == 0;
  if (temp) goto label800519B8;
  fp = v1;
label800519B8:
  v1 = lw(0x80078DDC + a0);
  v0 = (int32_t)s7 < (int32_t)v1;
  temp = v0 == 0;
  v0 = (int32_t)v1 < (int32_t)s6;
  if (temp) goto label800519D8;
  s7 = v1;
label800519D8:
  temp = v0 == 0;
  if (temp) goto label800519E4;
  s6 = v1;
label800519E4:
  a1 += 12; // 0x000C
  v0 = lw(a2 + 0x0000);
  v0 = lw(v0 + 0x0004);
  s2++;
  v0 = (int32_t)s2 < (int32_t)v0;
  temp = v0 != 0;
  a0 += 12; // 0x000C
  if (temp) goto label80051934;
label80051A04:
  v0 = lw(s5 + 0x0000);
  v0 = lw(v0 + 0x0004);
  temp = (int32_t)v0 <= 0;
  s2 = 0;
  if (temp) goto label80051D0C;
  t1 = s5;
  a2 = 0;
  v0 = s3 << 1;
  v0 += s3;
  v0 = v0 << 3;
  v1 = 0x80077EAC;
  t0 = v0 + v1;
  a3 = v0;
label80051A40:
  t4 = lw(sp + 0x0120);
  temp = (int32_t)t4 <= 0;
  a1 = s2 + 1; // 0x0001
  if (temp) goto label80051A70;
  v0 = lw(s5 + 0x0000);
  v0 = lw(v0 + 0x0004);
  temp = a1 != v0;
  v0 = a1 << 1;
  if (temp) goto label80051A94;
  a1 = 0;
  goto label80051A90;
label80051A70:
  a1 = s2 - 1; // 0xFFFFFFFF
  temp = (int32_t)a1 >= 0;
  v0 = a1 << 1;
  if (temp) goto label80051A94;
  v0 = lw(t1 + 0x0000);
  v0 = lw(v0 + 0x0004);
  a1 = v0 - 1; // 0xFFFFFFFF
label80051A90:
  v0 = a1 << 1;
label80051A94:
  v0 += a1;
  v1 = v0 << 2;
  v0 = lw(0x80078DD8 + v1);
  v0--;
  v0 = v0 < 511;
  temp = v0 == 0;
  if (temp) goto label80051AF8;
  v0 = lw(0x80078DDC + v1);
  v0--;
  v0 = v0 < 239;
  temp = v0 == 0;
  if (temp) goto label80051AF8;
  v0 = lw(0x80078DE0 + v1);
  temp = (int32_t)v0 <= 0;
  if (temp) goto label80051AF8;
  s1 = 1; // 0x0001
label80051AF8:
  v1 = lw(0x80078DD8 + a2);
  v0 = (int32_t)v1 < 512;
  temp = v0 != 0;
  v0 = a1 << 1;
  if (temp) goto label80051B38;
  v0 += a1;
  v0 = v0 << 2;
  v0 = lw(0x80078DD8 + v0);
  v0 = (int32_t)v0 < 512;
  temp = v0 == 0;
  if (temp) goto label80051CDC;
label80051B38:
  temp = (int32_t)v1 > 0;
  v0 = a1 << 1;
  if (temp) goto label80051B60;
  v0 += a1;
  v0 = v0 << 2;
  v0 = lw(0x80078DD8 + v0);
  temp = (int32_t)v0 <= 0;
  if (temp) goto label80051CDC;
label80051B60:
  v1 = lw(0x80078DDC + a2);
  v0 = (int32_t)v1 < 240;
  temp = v0 != 0;
  v0 = a1 << 1;
  if (temp) goto label80051BA0;
  v0 += a1;
  v0 = v0 << 2;
  v0 = lw(0x80078DDC + v0);
  v0 = (int32_t)v0 < 240;
  temp = v0 == 0;
  if (temp) goto label80051CDC;
label80051BA0:
  temp = (int32_t)v1 > 0;
  v0 = 1; // 0x0001
  if (temp) goto label80051BCC;
  v0 = a1 << 1;
  v0 += a1;
  v0 = v0 << 2;
  v0 = lw(0x80078DDC + v0);
  temp = (int32_t)v0 <= 0;
  v0 = 1; // 0x0001
  if (temp) goto label80051CDC;
label80051BCC:
  a0 = a1 << 1;
  a0 += a1;
  a0 = a0 << 2;
  sw(0x80077EA0 + a3, v0);
  v0 = lw(0x80078DDC + a2);
  v1 = lhu(0x80078DD8 + a2);
  v0 = v0 << 16;
  v0 = v0 | v1;
  sw(0x80077EA4 + a3, v0);
  v0 = lw(0x80078DDC + a0);
  v1 = lhu(0x80078DD8 + a0);
  v0 = v0 << 16;
  v0 = v0 | v1;
  sw(0x80077EA8 + a3, v0);
  v0 = lw(0x80078DDC + a2);
  v1 = lw(0x80078DDC + a0);
  v0 -= v1;
  sw(t0 + 0x0000, v0);
  a0 = lw(0x80078DD8 + a0);
  v0 = lw(0x80078DD8 + a2);
  a0 -= v0;
  sw(0x80077EB0 + a3, a0);
  v0 = lw(t0 + 0x0000);
  v1 = lw(0x80078DD8 + a2);
  v0 = -v0;
  mult(v0, v1);
  v1=lo;
  v0 = lw(0x80078DDC + a2);
  mult(a0, v0);
  s3++;
  t0 += 24; // 0x0018
  t2=lo;
  v0 = v1 - t2;
  sw(0x80077EB4 + a3, v0);
  a3 += 24; // 0x0018
label80051CDC:
  t4 = lw(sp + 0x0108);
  v0 = t4 << 2;
  v0 = lw(0x80078640 + v0);
  v0 = lw(v0 + 0x0004);
  s2++;
  v0 = (int32_t)s2 < (int32_t)v0;
  temp = v0 != 0;
  a2 += 12; // 0x000C
  if (temp) goto label80051A40;
label80051D0C:
  v0 = (int32_t)fp < 512;
  temp = v0 == 0;
  v0 = (int32_t)s6 < 240;
  if (temp) goto label80051D4C;
  temp = v0 == 0;
  if (temp) goto label80051D4C;
  temp = (int32_t)s4 <= 0;
  if (temp) goto label80051D4C;
  temp = (int32_t)s7 <= 0;
  if (temp) goto label80051D4C;
  v0 = lw(0x80075934);
  temp = (int32_t)v0 <= 0;
  if (temp) goto label80051D4C;
  temp = s3 != 0;
  if (temp) goto label80051D80;
label80051D4C:
  temp = (int32_t)fp > 0;
  if (temp) goto label80051D88;
  temp = (int32_t)s6 > 0;
  v0 = (int32_t)s4 < 512;
  if (temp) goto label80051D88;
  temp = v0 != 0;
  v0 = (int32_t)s7 < 240;
  if (temp) goto label80051D88;
  temp = v0 != 0;
  if (temp) goto label80051D88;
  v0 = lw(0x80075934);
  temp = (int32_t)v0 <= 0;
  if (temp) goto label80051D88;
label80051D80:
  s1 = s1 | 0x2;
  goto label80051D8C;
label80051D88:
  s1 = 0;
label80051D8C:
  v0 = 2; // 0x0002
  temp = s1 != v0;
  if (temp) goto label80051E70;
  t4 = lw(sp + 0x0138);
  v0 = (int32_t)t4 < 4096;
  temp = v0 == 0;
  a0 = sp + 208; // 0x00D0
  if (temp) goto label80051E70;
  a2 = 0x80076DF8;
  a1 = sp + 96; // 0x0060
  spyro_vec3_sub(a0, a1, a2);
  a0 = sp + 224; // 0x00E0
  s0 = sp + 232; // 0x00E8
  a1 = s0;
  a3 = lhu(0x80076E1C);
  v0 = 4096; // 0x1000
  sw(sp + 0x00C0, v0);
  v0 = lhu(0x80076E1E);
  v1 = lhu(0x80076E20);
  a2 = 0;
  sw(sp + 0x00C4, 0);
  sw(sp + 0x00C8, 0);
  a3 = a3 >> 4;
  v0 = v0 >> 4;
  v1 = v1 >> 4;
  sb(sp + 0x00E0, a3);
  sb(sp + 0x00E1, v0);
  sb(sp + 0x00E2, v1);
  spyro_mat3_rotation(a0, a1, a2);
  a0 = s0;
  a1 = sp + 192; // 0x00C0
  a2 = a1;
  spyro_set_mat_mirrored_vec_multiply(a0, a1, a2);
  v1 = lw(sp + 0x00C0);
  v0 = lw(sp + 0x00D0);
  mult(v1, v0);
  v1 = lw(sp + 0x00C4);
  a1=lo;
  v0 = lw(sp + 0x00D4);
  mult(v1, v0);
  v1 = lw(sp + 0x00C8);
  a0=lo;
  v0 = lw(sp + 0x00D8);
  mult(v1, v0);
  v0 = a1 + a0;
  v1=lo;
  v0 += v1;
  temp = (int32_t)v0 >= 0;
  if (temp) goto label80051E70;
  s1 = 0;
label80051E70:
  temp = s1 == 0;
  if (temp) goto label80051F5C;
  temp = (int32_t)fp >= 0;
  if (temp) goto label80051E84;
  fp = 0;
label80051E84:
  temp = (int32_t)s6 >= 0;
  v0 = (int32_t)s4 < 513;
  if (temp) goto label80051E90;
  s6 = 0;
label80051E90:
  temp = v0 != 0;
  v0 = (int32_t)s7 < 241;
  if (temp) goto label80051E9C;
  s4 = 512; // 0x0200
label80051E9C:
  temp = v0 != 0;
  v0 = s6 << 16;
  if (temp) goto label80051EA8;
  s7 = 240; // 0x00F0
label80051EA8:
  sw(0x8007AA00, v0);
  v0 = s7 << 16;
  sw(0x8007AA04, v0);
  v0 = s4 << 16;
  sw(0x8007AA08, v0);
  v0 = fp << 16;
  sw(0x8007AA0C, v0);
  v0 = s3 << 1;
  v0 += s3;
  v0 = v0 << 3;
  sw(0x80077EA0 + v0, 0);
  t4 = lw(sp + 0x0138);
  v0 = (int32_t)t4 < 12289;
  temp = v0 != 0;
  a1 = sp + 16; // 0x0010
  if (temp) goto label80051F48;
  v0 = lw(s5 + 0x0000);
  a1 = lw(sp + 0x00B8);
  v0 = lw(v0 + 0x0000);
  a2 = t4 - 12288; // 0xFFFFD000
  sw(0x8007575C, a2);
  a0 = lw(v0 + 0x0010);
  v0 = interpolate_color(a0, a1, a2);
  v1 = lw(s5 + 0x0000);
  a1 = sp + 16; // 0x0010
  a0 = lw(v1 + 0x0000);
  sw(0x800757D4, v0);
  a2 = sp + 48; // 0x0030
  function_80050240();
  goto label80051F5C;
label80051F48:
  v0 = lw(s5 + 0x0000);
  a0 = lw(v0 + 0x0000);
  a2 = sp + 48; // 0x0030
  function_8004F4BC();
label80051F5C:
  t4 = lw(sp + 0x0170);
  s5 += 4; // 0x0004
  t4 += 4; // 0x0004
  sw(sp + 0x0170, t4);
  t4 = lw(sp + 0x0108);
  v0 = lw(0x800758BC);
  t4++;
  v0 = (int32_t)t4 < (int32_t)v0;
  temp = v0 != 0;
  sw(sp + 0x0108, t4);
  if (temp) goto label80050E90;
label80051F88:
  a2 = 0x80076E24;
  a0 = lw(a2 + 0x0000);
  v0 = lw(0x800785B4);
  v0 = (int32_t)a0 < (int32_t)v0;
  temp = v0 != 0;
  a1 = a2 - 64; // 0xFFFFFFC0
  if (temp) goto label80051FB0;
  a0 = -1; // 0xFFFFFFFF
label80051FB0:
  a2 -= 84; // 0xFFFFFFAC
  draw_skybox(a0, a1, a2);
  ra = lw(sp + 0x019C);
  fp = lw(sp + 0x0198);
  s7 = lw(sp + 0x0194);
  s6 = lw(sp + 0x0190);
  s5 = lw(sp + 0x018C);
  s4 = lw(sp + 0x0188);
  s3 = lw(sp + 0x0184);
  s2 = lw(sp + 0x0180);
  s1 = lw(sp + 0x017C);
  s0 = lw(sp + 0x0178);
  sp += 416; // 0x01A0
  return;
}

// size: 0x00000100
void function_800521C0(void)
{
  at = lw(0x80075828);
  v0 = 0x8006FCF4;
  a0 = 0x8006FCF4 + 0x2400;
  v1 = 0x8006FCF4 + 0x2200;

  a1 = 0x800771C8;
  a3 = lw(at + 0x0048);
  a2 = 0xFF;
  at -= 88;
label800521F4:
  t0 = a3;
  at += 88;
  a3 = lw(at + 88 + 0x48);
  if (t0 & 0x80) {
    if ((t0 & 0xFF) != a2) goto label800521F4;
    sw(v0, 0);
    sw(v1, 0);
    sw(a0, 0);
    return;
  }
  t1 = (int32_t)t0 >> 24;
  if (t1 == 0) {
    sw(a0, at);
    a0 += 4;
    if ((t0 >> 16) == a2) goto label800521F4;
    t1 = lbu(t1 + a1);
    if ((int32_t)t1 <= 0) {
      sb(at + 0x51, 0);
      a0 -= 4;
    }
    goto label800521F4;
  }
  t1 = t0 >> 16;
  t1 = t1 & 0xFF;
  if ((t0 >> 24) & 0x80) {
    sw(v1, at);
    v1 += 4;
    if (t1 == a2) goto label800521F4;
    t1 = lbu(t1 + a1);
    if ((int32_t)t1 <= 0) {
      sb(at + 0x51, 0);
      v1 -= 4;
    }
    goto label800521F4;
  }
  sw(v0, at);
  v0 += 4;
  if (t1 == a2) goto label800521F4;
  t1 = lbu(t1 + a1);
  if ((int32_t)t1 <= 0) {
    sb(at + 0x51, 0);
    v0 -= 4;
  }
  goto label800521F4;
}

// size: 0x000004E4
void function_80059F8C(void)
{
  uint32_t temp;
  at = 0x80076DD0;
  v0 = lw(at + 0x0028);
  v1 = lw(at + 0x002C);
  a0 = lw(at + 0x0030);
  a1 = lw(at + 0x0000);
  a2 = lw(at + 0x0004);
  a3 = lw(at + 0x0008);
  t0 = lw(at + 0x000C);
  t1 = lw(at + 0x0010);
  v0 = v0 >> 2;
  v1 = v1 >> 2;
  a0 = a0 >> 2;
  at = 0x1F800000;
  sw(at + 0x0000, v0);
  sw(at + 0x0004, v1);
  sw(at + 0x0008, a0);
  sw(at + 0x000C, a1);
  sw(at + 0x0010, a2);
  sw(at + 0x0014, a3);
  sw(at + 0x0018, t0);
  sw(at + 0x001C, t1);
  t8 = 0x8006FCF4;
  t8 += 10240; // 0x2800
  t9 = 0x80075EF8;
  v0 = lw(t9 + 0x0000);
  v1 = lw(t9 + 0x0004);
  t9 = lw(t9 + 0x0008);
  sw(at + 0x0020, v0);
  sw(at + 0x0024, v1);
  v0 = allocator1_ptr;
  v0 = lw(v0 + 0x0000);
  v1 = ordered_linked_list;
  v1 = lw(v1 + 0x0000);
  sw(at + 0x0028, v0);
  sw(at + 0x002C, v1);
label8005A02C:
  temp = t8 == t9;
  t8 += 8; // 0x0008
  if (temp) goto label8005A458;
  t2 = 0x1F800000;
  at = lw(t8 - 0x0008); // 0xFFFFFFF8
  t3 = lw(t2 + 0x000C);
  t4 = lw(t2 + 0x0010);
  t5 = lw(t2 + 0x0014);
  t6 = lw(t2 + 0x0018);
  t7 = lw(t2 + 0x001C);
  cop2.RTM0 = t3;
  cop2.RTM1 = t4;
  cop2.RTM2 = t5;
  cop2.RTM3 = t6;
  cop2.RTM4 = t7;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  a3 = lw(t2 + 0x0000);
  t0 = lw(t2 + 0x0004);
  t1 = lw(t2 + 0x0008);
  a2 = lw(at + 0x001C);
  a0 = lw(at + 0x0010);
  v1 = lw(at + 0x000C);
  a1 = a2 & 0xFFFF;
  temp = a1 == 0;
  a1 = t1 - a1;
  if (temp) goto label8005A02C;
  a1 = a1 << 16;
  a0 = a0 >> 2;
  a0 = t0 - a0;
  a0 += a1;
  v1 = v1 >> 2;
  v1 -= a3;
  cop2.VXY0 = a0;
  cop2.VZ0 = v1;
  a1 = a2 >> 16;
  a0 = a2 >> 22;
  RTPS();
  v0 = lw(t8 - 0x0004); // 0xFFFFFFFC
  v1 = spyro_sin_lut;
  a0 = a0 & 0x3F;
  a1 = a1 & 0x3F;
  a0 = a0 << 3;
  a1 = a1 << 3;
  t0 = cop2.MAC3;
  a3 = cop2.MAC2;
  a2 = cop2.MAC1;
  cop2.TRZ = t0;
  cop2.TRY = a3;
  cop2.TRX = a2;
  t0 -= 4096; // 0xFFFFF000
  temp = (int32_t)t0 >= 0;
  if (temp) goto label8005A02C;
  temp = a1 == 0;
  a1 += v1;
  if (temp) goto label8005A1A4;
  a2 = lhu(a1 + 0x0080);
  a1 = lhu(a1 + 0x0000);
  a3 = a2 << 16;
  cop2.VXY0 = a3;
  cop2.VZ0 = a1;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  a3 = a1 << 16;
  a3 = -a3;
  t0 = cop2.IR1;
  t1 = cop2.IR2;
  t2 = cop2.IR3;
  cop2.VXY0 = a3;
  cop2.VZ0 = a2;
  a3 = 0xFFFF0000;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  t3 = t3 & 0xFFFF;
  t0 = t0 << 16;
  t3 += a0;
  t6 = t6 & 0xFFFF;
  t2 = t2 << 16;
  t6 += t2;
  t1 = t1 & 0xFFFF;
  t4 = t4 & a3;
  a1 = cop2.IR1;
  a2 = cop2.IR2;
  a3 = cop2.IR3;
  a1 = a1 & 0xFFFF;
  t4 += a1;
  a2 = a2 << 16;
  t5 = a2 + t1;
  t7 = a3 & 0xFFFF;
  cop2.RTM0 = t3;
  cop2.RTM1 = t4;
  cop2.RTM2 = t5;
  cop2.RTM3 = t6;
  cop2.RTM4 = t7;
label8005A1A4:
  temp = a0 == 0;
  a0 += v1;
  if (temp) goto label8005A23C;
  a2 = lhu(a0 + 0x0080);
  a1 = lhu(a0 + 0x0000);
  cop2.VZ0 = 0;
  a3 = a1 << 16;
  a3 += a2;
  cop2.VXY0 = a3;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  a1 = -a1;
  a1 = a1 & 0xFFFF;
  a2 = a2 << 16;
  a1 += a2;
  t0 = cop2.IR1;
  t1 = cop2.IR2;
  t2 = cop2.IR3;
  cop2.VXY0 = a1;
  cop2.VZ0 = 0;
  a3 = 0xFFFF0000;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  t1 = t1 << 16;
  t4 = t4 & 0xFFFF;
  t4 += t1;
  t0 = t0 & 0xFFFF;
  t2 = t2 & 0xFFFF;
  t5 = t5 & a3;
  a1 = cop2.IR1;
  a2 = cop2.IR2;
  a3 = cop2.IR3;
  a1 = a1 << 16;
  t3 = a1 + t0;
  a3 = a3 << 16;
  t6 = a3 + t2;
  a2 = a2 & 0xFFFF;
  t5 += a2;
label8005A23C:
  cop2.RTM0 = t3;
  cop2.RTM1 = t4;
  cop2.RTM2 = t5;
  cop2.RTM3 = t6;
  cop2.RTM4 = t7;
  t6 = cop2.SXY2;
  t7 = cop2.SZ3;
  cop2.VXY0 = 0;
  cop2.VZ0 = v0;
  cop2.VXY1 = v0;
  cop2.VZ1 = 0;
  cop2.VXY2 = 0;
  v1 = -v0;
  cop2.VZ2 = v1;
  t1 = cop2.TRZ;
  RTPT();
  v1 = 0x1F800000;
  a0 = lw(v1 + 0x0028);
  a1 = lw(v1 + 0x002C);
  a3 = lw(v1 + 0x0020);
  t0 = lw(v1 + 0x0024);
  at = lb(at + 0x0047);
  a2 = 0x26808080;
  t1 -= 3072; // 0xFFFFF400
  temp = (int32_t)t1 < 0;
  t1 = -t1;
  if (temp) goto label8005A2CC;
  t1 += 1024; // 0x0400
  t1 = t1 >> 3;
  a2 = 0x26000000;
  a2 = a2 | t1;
  t1 = t1 << 8;
  a2 = a2 | t1;
  t1 = t1 << 8;
  a2 = a2 | t1;
label8005A2CC:
  v0 = -v0;
  v0 = v0 & 0xFFFF;
  t1 = cop2.SXY0;
  t2 = cop2.SXY1;
  t3 = cop2.SXY2;
  NCLIP();
  sw(v1 + 0x0030, t1);
  sw(v1 + 0x0034, t2);
  sw(v1 + 0x0038, t3);
  sw(v1 + 0x0040, t1);
  t1 = cop2.SZ1;
  t2 = cop2.SZ2;
  t3 = cop2.SZ3;
  t4 = cop2.MAC0;
  sw(v1 + 0x0044, t1);
  temp = (int32_t)t4 < 0;
  sw(v1 + 0x0048, t2);
  if (temp) goto label8005A02C;
  sw(v1 + 0x004C, t3);
  sw(v1 + 0x0054, t1);
  cop2.VXY0 = v0;
  cop2.VZ0 = 0;
  at--;
  RTPS();
  temp = (int32_t)t6 <= 0;
  t5 = 0x00F00000;
  if (temp) goto label8005A02C;
  t5 = t6 - t5;
  temp = (int32_t)t5 >= 0;
  t5 = t6 << 16;
  if (temp) goto label8005A02C;
  t5 = (int32_t)t5 >> 16;
  t4 = t5 + 8; // 0x0008
  temp = (int32_t)t4 <= 0;
  t4 = t5 - 520; // 0xFFFFFDF8
  if (temp) goto label8005A02C;
  temp = (int32_t)t4 >= 0;
  if (temp) goto label8005A02C;
  t1 = cop2.SXY2;
  t2 = cop2.SZ3;
  sw(v1 + 0x003C, t1);
  sw(v1 + 0x0050, t2);
  v0 = v1 + 48; // 0x0030
  v1 = v0 + 16; // 0x0010
  t1 = cop2.TRZ;
  t3 = 0x00010000;
  t1 -= 1024; // 0xFFFFFC00
  temp = (int32_t)t1 <= 0;
  t1 = v0;
  if (temp) goto label8005A3BC;
  t2 = v1 + 4; // 0x0004
label8005A388:
  temp = t1 == t2;
  t4 = lw(t1 + 0x0014);
  if (temp) goto label8005A3BC;
  t5 = lw(t1 + 0x0000);
  t4 -= t7;
  temp = (int32_t)t4 < 0;
  t1 += 4; // 0x0004
  if (temp) goto label8005A3B0;
  temp = t4 == 0;
  t5 -= t3;
  if (temp) goto label8005A388;
  sw(t1 - 0x0004, t5); // 0xFFFFFFFC
  goto label8005A388;
label8005A3B0:
  t5 += t3;
  sw(t1 - 0x0004, t5); // 0xFFFFFFFC
  goto label8005A388;
label8005A3BC:
  temp = v0 == v1;
  t2 = lw(v0 + 0x0000);
  if (temp) goto label8005A44C;
  t3 = lw(v0 + 0x0004);
  t4 = lw(v0 + 0x0014);
  t5 = lw(v0 + 0x0018);
  v0 += 4; // 0x0004
  sw(a0 + 0x0004, a2);
  sw(a0 + 0x0008, t6);
  sw(a0 + 0x000C, a3);
  sw(a0 + 0x0014, t0);
  t1 = a3 + 7936; // 0x1F00
  sw(a0 + 0x001C, t1);
  sw(a0 + 0x0010, t2);
  sw(a0 + 0x0018, t3);
  t1 = 0x07000000;
  sw(a0 + 0x0000, t1);
  t4 += t5;
  t5 = t4 >> 1;
  t4 += t5;
  t4 += t7;
  t4 = t4 >> 7;
  t4 -= at;
  temp = (int32_t)t4 < 0;
  t4 = t4 << 3;
  if (temp) goto label8005A3BC;
  t4 += a1;
  t5 = lw(t4 + 0x0000);
  sw(t4 + 0x0000, a0);
  temp = t5 == 0;
  t3 = a0 >> 16;
  if (temp) goto label8005A440;
  sh(t5 + 0x0000, a0);
  sb(t5 + 0x0002, t3);
  a0 += 32; // 0x0020
  goto label8005A3BC;
label8005A440:
  sw(t4 + 0x0004, a0);
  a0 += 32; // 0x0020
  goto label8005A3BC;
label8005A44C:
  at = 0x1F800000;
  sw(at + 0x0028, a0);
  goto label8005A02C;
label8005A458:
  at = 0x1F800000;
  at = lw(at + 0x0028);
  v0 = allocator1_ptr;
  sw(v0 + 0x0000, at);
  return;
}

// size: 0x00000284
void function_80058864(void)
{
  uint32_t temp;
  t5 = 0x80076248;
  a3 = 0x80076DD0;
  at = lw(a3 + 0x0000);
  v0 = lw(a3 + 0x0004);
  v1 = lw(a3 + 0x0008);
  a0 = lw(a3 + 0x000C);
  a1 = lw(a3 + 0x0010);
  cop2.RTM0 = at;
  cop2.RTM1 = v0;
  cop2.RTM2 = v1;
  cop2.RTM3 = a0;
  cop2.RTM4 = a1;
  at = lw(a3 + 0x0028);
  v0 = lw(a3 + 0x002C);
  v1 = lw(a3 + 0x0030);
  a0 = lw(t5 + 0x0004);
  a1 = lw(t5 + 0x0008);
  a2 = lw(t5 + 0x000C);
  at = a0 - at;
  v0 -= a1;
  v1 -= a2;
  cop2.IR3 = at;
  cop2.IR1 = v0;
  cop2.IR2 = v1;
  MVMVA(SF_ON, MX_RT, V_IR, CV_NONE, LM_OFF);
  at = cop2.MAC1;
  v0 = cop2.MAC2;
  v1 = cop2.MAC3;
  cop2.TRX = at;
  cop2.TRY = v0;
  cop2.TRZ = v1;
  at = lw(t5 + 0x0010);
  v0 = lw(t5 + 0x0014);
  v1 = lw(t5 + 0x0018);
  a0 = lw(t5 + 0x001C);
  a1 = lw(t5 + 0x0020);
  cop2.RTM0 = at;
  cop2.RTM1 = v0;
  cop2.RTM2 = v1;
  cop2.RTM3 = a0;
  cop2.RTM4 = a1;
  cop2.VXY0 = 0;
  cop2.VZ0 = 0;
  t8 = spyro_cos_lut;
  t9 = spyro_sin_lut;
  RTPS();
  t3 = cop2.SXY2;
  at = 448; // 0x01C0
  v0 = 0x1F800000;
  a1 = lw(t5 + 0x0024);
label80058944:
  v1 = t8 + at;
  a0 = t9 + at;
  v1 = lh(v1 + 0x0000);
  a0 = lh(a0 + 0x0000);
  mult(v1, a1);
  v1=lo;
  v1 = (int32_t)v1 >> 12;
  cop2.VZ0 = v1;
  mult(a0, a1);
  a0=lo;
  a0 = (int32_t)a0 >> 12;
  cop2.VXY0 = a0;
  at -= 64; // 0xFFFFFFC0
  v0 += 4; // 0x0004
  RTPS();
  v1 = cop2.SXY2;
  temp = (int32_t)at >= 0;
  sw(v0 + 0x0000, v1);
  if (temp) goto label80058944;
  sw(v0 - 0x0020, v1); // 0xFFFFFFE0
  v0 += 4; // 0x0004
  at = 480; // 0x01E0
label80058998:
  v1 = t8 + at;
  a0 = t9 + at;
  v1 = lh(v1 + 0x0000);
  a0 = lh(a0 + 0x0000);
  mult(v1, a1);
  v1=lo;
  v1 = (int32_t)v1 >> 10;
  cop2.VZ0 = v1;
  mult(a0, a1);
  a0=lo;
  a0 = (int32_t)a0 >> 10;
  cop2.VXY0 = a0;
  at -= 64; // 0xFFFFFFC0
  v0 += 4; // 0x0004
  RTPS();
  v1 = cop2.SXY2;
  temp = (int32_t)at >= 0;
  sw(v0 - 0x0004, v1); // 0xFFFFFFFC
  if (temp) goto label80058998;
  t6 = allocator1_ptr;
  t6 = lw(t6 + 0x0000);
  t7 = linked_list1;
  t7 = lw(t7 + 0x0000);
  sw(t6 + 0x0008, 0);
  at = 0xE1000620;
  sw(t6 + 0x0004, at);
  at = 0x02000000;
  sw(t6 + 0x0000, at);
  at = lw(t7 + 0x0000);
  sw(t7 + 0x0000, t6);
  temp = at == 0;
  v0 = t6 >> 16;
  if (temp) goto label80058A2C;
  sh(at + 0x0000, t6);
  sb(at + 0x0002, v0);
  goto label80058A30;
label80058A2C:
  sw(t7 + 0x0004, t6);
label80058A30:
  t6 += 12; // 0x000C
  at = 0x1F800000;
  v0 = at + 36; // 0x0024
  v1 = at + 68; // 0x0044
  t4 = lbu(t5 + 0x002B);
label80058A44:
  a0 = lw(at + 0x0000);
  a1 = lw(at + 0x0004);
  a2 = lw(v0 + 0x0000);
  sw(t6 + 0x0008, a0);
  sw(t6 + 0x000C, a1);
  sw(t6 + 0x0010, a2);
  sw(t6 + 0x001C, a0);
  sw(t6 + 0x0020, a1);
  sw(t6 + 0x0024, t3);
  sb(t6 + 0x0004, t4);
  sb(t6 + 0x0005, t4);
  sb(t6 + 0x0006, t4);
  sb(t6 + 0x0018, t4);
  sb(t6 + 0x0019, t4);
  sb(t6 + 0x001A, t4);
  a0 = 34; // 0x0022
  sb(t6 + 0x0007, a0);
  sb(t6 + 0x001B, a0);
  a0 = 0x04000000;
  sw(t6 + 0x0000, a0);
  sw(t6 + 0x0014, a0);
  at += 4; // 0x0004
  v0 += 4; // 0x0004
  a0 = lw(t7 + 0x0000);
  sw(t7 + 0x0000, t6);
  a1 = t6 >> 16;
  sh(a0 + 0x0000, t6);
  sb(a0 + 0x0002, a1);
  t6 += 20; // 0x0014
  a0 = lw(t7 + 0x0000);
  sw(t7 + 0x0000, t6);
  a1 = t6 >> 16;
  sh(a0 + 0x0000, t6);
  sb(a0 + 0x0002, a1);
  temp = v0 != v1;
  t6 += 20; // 0x0014
  if (temp) goto label80058A44;
  at = allocator1_ptr;
  sw(at + 0x0000, t6);
  return;
}

// size: 0x00000544
void function_80059A48(void)
{
  uint32_t temp;
  t8 = spyro_position;
  t9 = 0x80076DD0;
  t7 = 0x8007AA10;
  a3 = lw(t7 + 0x0024);
  at = lw(t7 + 0x0010);
  v0 = lw(t7 + 0x0014);
  v1 = lw(t7 + 0x0018);
  a0 = lw(t9 + 0x0028);
  a1 = lw(t9 + 0x002C);
  a2 = lw(t9 + 0x0030);
  temp = (int32_t)a3 > 0;
  at -= a0;
  if (temp) goto label80059F84;
  v0 = a1 - v0;
  v1 = a2 - v1;
  a3 = lw(t9 + 0x0000);
  t0 = lw(t9 + 0x0004);
  t1 = lw(t9 + 0x0008);
  t2 = lw(t9 + 0x000C);
  t3 = lw(t9 + 0x0010);
  cop2.RTM0 = a3;
  cop2.RTM1 = t0;
  cop2.RTM2 = t1;
  cop2.RTM3 = t2;
  cop2.RTM4 = t3;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  cop2.VZ0 = at;
  v1 = v1 << 16;
  v0 = v0 & 0xFFFF;
  v0 += v1;
  cop2.VXY0 = v0;
  RTPS();
  v1 = lbu(t8 + 0x000E);
  at = spyro_sin_lut;
  v0 = spyro_cos_lut;
  v1 = v1 << 1;
  at += v1;
  v0 += v1;
  t6 = cop2.SXY2;
  t7 = cop2.SZ3;
  a1 = cop2.MAC3;
  a0 = cop2.MAC2;
  v1 = cop2.MAC1;
  cop2.TRZ = a1;
  cop2.TRY = a0;
  cop2.TRX = v1;
  v0 = lhu(v0 + 0x0000);
  at = lhu(at + 0x0000);
  v0 = v0 & 0xFFFF;
  cop2.VXY0 = v0;
  cop2.VZ0 = at;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  at = -at;
  at = at & 0xFFFF;
  a0 = cop2.IR1;
  a1 = cop2.IR2;
  a2 = cop2.IR3;
  cop2.VXY0 = at;
  cop2.VZ0 = v0;
  v1 = 0xFFFF0000;
  MVMVA(SF_ON, MX_RT, V_V0, CV_NONE, LM_OFF);
  a3 = a3 & v1;
  a0 = a0 & 0xFFFF;
  a3 += a0;
  t2 = t2 & v1;
  a2 = a2 & 0xFFFF;
  t2 += a2;
  a1 = a1 << 16;
  t1 = t1 & 0xFFFF;
  at = cop2.IR1;
  v0 = cop2.IR2;
  v1 = cop2.IR3;
  at = at & 0xFFFF;
  t0 = at + a1;
  v0 = v0 << 16;
  t1 += v0;
  t3 = v1 & 0xFFFF;
  cop2.RTM0 = a3;
  cop2.RTM1 = t0;
  cop2.RTM2 = t1;
  cop2.RTM3 = t2;
  cop2.RTM4 = t3;
  a3 = lbu(t8 + 0x0024);
  v0 = 0x80076378;
  temp = a3 == 0;
  v0 = lw(v0 + 0x0000);
  if (temp) goto label80059CE4;
  v1 = lhu(t8 + 0x0018);
  a0 = lhu(t8 + 0x001E);
  a1 = v1 >> 8;
  v1 = v1 & 0xFF;
  v1 = v1 << 2;
  v1 += v0;
  a1 = a1 << 2;
  a1 += v0;
  v1 = lw(v1 + 0x0038);
  a1 = lw(a1 + 0x0038);
  a2 = a0 >> 8;
  a0 = a0 & 0xFF;
  a0 = a0 << 2;
  a2 = a2 << 2;
  v1 += a0;
  a1 += a2;
  v1 = lw(v1 + 0x0024);
  a1 = lw(a1 + 0x0024);
  at = 0x1F800000;
  v1 = v1 << 11;
  v1 = v1 >> 10;
  a1 = a1 << 11;
  a1 = a1 >> 10;
  v0 = lw(v1 + 0x0000);
  v1 = lw(v1 + 0x0004);
  a0 = lw(a1 + 0x0000);
  a1 = lw(a1 + 0x0004);
  sw(at + 0x0000, v0);
  sw(at + 0x0004, v1);
  sw(at + 0x0008, a0);
  sw(at + 0x000C, a1);
  v1 = a3;
  v0 = 16; // 0x0010
  v0 -= v1;
  a0 = 3; // 0x0003
label80059C5C:
  a1 = lbu(at + 0x0000);
  a2 = lbu(at + 0x0001);
  a3 = lbu(at + 0x0002);
  cop2.IR0 = v0;
  cop2.IR1 = a1;
  cop2.IR2 = a2;
  cop2.IR3 = a3;
  a1 = lbu(at + 0x0008);
  a2 = lbu(at + 0x0009);
  GPF(SF_OFF, LM_OFF);
  a3 = lbu(at + 0x000A);
  cop2.IR0 = v1;
  cop2.IR1 = a1;
  cop2.IR2 = a2;
  cop2.IR3 = a3;
  a0--;
  at += 3; // 0x0003
  GPL(SF_OFF, LM_OFF);
  a1 = cop2.MAC1;
  a2 = cop2.MAC2;
  a3 = cop2.MAC3;
  a1 = a1 >> 4;
  a2 = a2 >> 4;
  a3 = a3 >> 4;
  sb(at - 0x0003, a1); // 0xFFFFFFFD
  sb(at - 0x0002, a2); // 0xFFFFFFFE
  temp = (int32_t)a0 > 0;
  sb(at - 0x0001, a3); // 0xFFFFFFFF
  if (temp) goto label80059C5C;
  at = 0x1F800000;
  v0 = 0x1F800000;
  goto label80059D10;
label80059CE4:
  v1 = lbu(t8 + 0x0018);
  a0 = lbu(t8 + 0x001E);
  v1 = v1 << 2;
  v0 += v1;
  v0 = lw(v0 + 0x0038);
  a0 = a0 << 2;
  v0 += a0;
  v0 = lw(v0 + 0x0024);
  at = 0x1F800000;
  v0 = v0 << 11;
  v0 = v0 >> 10;
label80059D10:
  v1 = lw(v0 + 0x0004);
  v0 = lw(v0 + 0x0000);
  sw(at + 0x0004, v1);
  sw(at + 0x0000, v0);
  sw(at + 0x0008, v0);
  v0 = 0x8007AA10;
  a1 = lw(v0 + 0x000C);
  a0 = lw(v0 + 0x0008);
  v1 = lw(v0 + 0x0004);
  v0 = lw(v0 + 0x0000);
  a0 = a0 | a1;
  temp = a0 == 0;
  t0 = 0;
  if (temp) goto label80059D4C;
  t0 = 1; // 0x0001
label80059D4C:
  sw(at + 0x0010, v1);
  sw(at + 0x000C, v0);
  sw(at + 0x0014, v0);
  v0 = at + 8; // 0x0008
  v1 = at + 128; // 0x0080
label80059D60:
  a0 = lbu(at + 0x0000);
  a1 = lbu(at + 0x0001);
  a2 = lb(at + 0x000C);
  a3 = lb(at + 0x000D);
  temp = t0 == 0;
  t1 = a0 << 1;
  if (temp) goto label80059DA4;
  a0 += t1;
  a0 = (int32_t)a0 >> 2;
  t1 = a1 << 1;
  a1 += t1;
  a1 = (int32_t)a1 >> 2;
  t1 = a2 << 1;
  a2 += t1;
  a2 = (int32_t)a2 >> 2;
  t1 = a3 << 1;
  a3 += t1;
  a3 = (int32_t)a3 >> 2;
label80059DA4:
  sb(v1 + 0x0000, a0);
  a0 += a1;
  a0 = a0 >> 1;
  sb(v1 + 0x0001, a0);
  sb(v1 + 0x0010, a2);
  a2 += a3;
  a2 = (int32_t)a2 >> 1;
  sb(v1 + 0x0011, a2);
  at++;
  temp = at != v0;
  v1 += 2; // 0x0002
  if (temp) goto label80059D60;
  v0 = 0x1F800080;
  at = v0 + 16; // 0x0010
  v1 = 0x8006E268; // &0x00001000
  a0 = 0x1F800000;
  a1 = a0 + 120; // 0x0078
  t0 = lw(v1 + 0x0000);
  a3 = lbu(v0 + 0x0000);
  v0++;
  v1 += 4; // 0x0004
  t1 = (int32_t)t0 >> 16;
  t0 = t0 << 16;
  t0 = (int32_t)t0 >> 16;
label80059E08:
  mult(a3, t0);
  a2 = lb(at + 0x0000);
  t0=lo;
  t0 = (int32_t)t0 >> 10;
  cop2.VZ0 = t0;
  mult(a3, t1);
  a2 = a2 << 18;
  t1=lo;
  t1 = (int32_t)t1 >> 10;
  t1 = t1 & 0xFFFF;
  t1 += a2;
  cop2.VXY0 = t1;
  at++;
  v0++;
  RTPS();
  t0 = lw(v1 + 0x0000);
  a3 = lbu(v0 - 0x0001); // 0xFFFFFFFF
  v1 += 4; // 0x0004
  t1 = (int32_t)t0 >> 16;
  t0 = t0 << 16;
  t0 = (int32_t)t0 >> 16;
  t2 = cop2.SXY2;
  t3 = cop2.SZ3;
  sw(a0 + 0x0000, t2);
  sw(a0 + 0x0004, t3);
  temp = a0 != a1;
  a0 += 8; // 0x0008
  if (temp) goto label80059E08;
  at = 0x1F800000;
  v0 = at + 128; // 0x0080
  a3 = ordered_linked_list;
  a3 = lw(a3 + 0x0000);
  t0 = allocator1_ptr;
  t0 = lw(t0 + 0x0000);
  v1 = t6;
  a0 = t7;
  t1 = 0x8007AA10;
  t1 = lw(t1 + 0x001C);
  a1 = 0x32608080;
  a2 = 0x00080808;
  t4 = lw(at + 0x0000);
  t5 = lw(at + 0x0004);
  sw(v0 + 0x0000, t4);
  sw(v0 + 0x0004, t5);
label80059EC8:
  temp = at == v0;
  t4 = lw(at + 0x0000);
  if (temp) goto label80059F74;
  t5 = lw(at + 0x0004);
  t6 = lw(at + 0x0008);
  t7 = lw(at + 0x000C);
  at += 8; // 0x0008
  sw(t0 + 0x0010, a1);
  sw(t0 + 0x0014, v1);
  sw(t0 + 0x0018, 0);
  sw(t0 + 0x001C, t4);
  t5 += t7;
  t7 = t5 >> 1;
  t5 += t7;
  t5 += a0;
  t5 = t5 >> 9;
  sw(t0 + 0x0020, 0);
  sw(t0 + 0x0024, t6);
  sw(t0 + 0x0008, 0);
  t6 = 0xE1000640;
  sw(t0 + 0x0004, t6);
  t6 = 0x82000000;
  t6 = t6 ^ t0;
  t6 += 12; // 0x000C
  sw(t0 + 0x0000, t6);
  t6 = 0x06000000;
  sw(t0 + 0x000C, t6);
  t5 -= t1;
  temp = (int32_t)t5 < 0;
  t5 = t5 << 3;
  if (temp) goto label80059EC8;
  t5 += a3;
  t6 = lw(t5 + 0x0000);
  t7 = t0 + 12; // 0x000C
  sw(t5 + 0x0000, t7);
  temp = t6 == 0;
  t7 = t0 >> 16;
  if (temp) goto label80059F68;
  sh(t6 + 0x0000, t0);
  sb(t6 + 0x0002, t7);
  t0 += 40; // 0x0028
  goto label80059EC8;
label80059F68:
  sw(t5 + 0x0004, t0);
  t0 += 40; // 0x0028
  goto label80059EC8;
label80059F74:
  at = allocator1_ptr;
  sw(at + 0x0000, t0);
  return;
label80059F84:
  return;
}

// size: 0x00000D2C
void function_800573C8(void)
{
  uint32_t temp;
  at = 0x1F800000;
  sw(at + 0x0000, s0);
  sw(at + 0x0004, s1);
  sw(at + 0x0008, s2);
  sw(at + 0x000C, s3);
  sw(at + 0x0010, s4);
  sw(at + 0x0014, s5);
  sw(at + 0x0018, s6);
  sw(at + 0x001C, s7);
  sw(at + 0x0020, gp);
  sw(at + 0x0024, sp);
  sw(at + 0x0028, fp);
  sw(at + 0x002C, ra);
  at = 0x80076DD0;
  t2 = lw(at + 0x0000);
  t3 = lw(at + 0x0004);
  t4 = lw(at + 0x0008);
  t5 = lw(at + 0x000C);
  t6 = lw(at + 0x0010);
  cop2.RTM0 = t2;
  cop2.RTM1 = t3;
  cop2.RTM2 = t4;
  cop2.RTM3 = t5;
  cop2.RTM4 = t6;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  at = 0x80076DD0;
  s7 = lw(at + 0x0028);
  t8 = lw(at + 0x002C);
  t9 = lw(at + 0x0030);
  s7 = (int32_t)s7 >> 2;
  t8 = (int32_t)t8 >> 2;
  t9 = (int32_t)t9 >> 2;
  cop2.DQB = 0;
  at = 256; // 0x0100
  cop2.DQA = at;
  t7 = allocator1_end;
  t7 = lw(t7 + 0x0000);
  ra = 0x80075824;
  ra = lw(ra + 0x0000);
  fp = allocator1_ptr;
  fp = lw(fp + 0x0000);
  sp = ordered_linked_list;
  sp = lw(sp + 0x0000);
  gp = lh(ra + 0x0000);
  t7 -= 1024; // 0xFFFFFC00
  s0 = spyro_sin_lut;
  s1 = 0x80076278;
  s2 = 1; // 0x0001
  s3 = 2; // 0x0002
  s4 = 0x00010000;
  s5 = 0x01000000;
  s6 = 0x02000000;
label800574C0:
  temp = (int32_t)gp < 0;
  at = (int32_t)gp >> 8;
  if (temp) goto label80058080;
  temp = at == 0;
  at--;
  if (temp) goto label80057F98;
  temp = at == 0;
  at--;
  if (temp) goto label80057E7C;
  temp = at == 0;
  at--;
  if (temp) goto label80057C08;
  temp = at == 0;
  at--;
  if (temp) goto label80057A74;
  temp = at == 0;
  at--;
  if (temp) goto label80057954;
  temp = at == 0;
  if (temp) goto label80057750;
  at = lw(ra + 0x0004);
  a0 = lw(ra + 0x0008);
  v0 = t7 - fp;
  temp = (int32_t)v0 <= 0;
  v0 = at >> 16;
  if (temp) goto label8005809C;
  at = at & 0xFFFF;
  at -= s7;
  sb(ra + 0x0003, 0);
  v1 = a0 & 0xFFFF;
  a1 = a0 >> 23;
  a1 = a1 & 0x1FE;
  a1 += s0;
  a2 = a1 + 128; // 0x0080
  a1 = lh(a1 + 0x0000);
  a2 = lh(a2 + 0x0000);
  a0 = a0 >> 16;
  a0 = a0 & 0xFF;
  v0 = t8 - v0;
  v1 = t9 - v1;
  v1 = v1 << 16;
  cop2.IR0 = a0;
  cop2.IR1 = a1;
  cop2.IR2 = a2;
  t6 = gp & 0xFF;
  t6 = t6 << 2;
  GPF(SF_OFF, LM_OFF);
  t6 += s1;
  t6 = lw(t6 + 0x0000);
  a1 = cop2.MAC1;
  a0 = cop2.MAC2;
  gp = lh(ra + 0x0020);
  ra += 32; // 0x0020
  a1 = (int32_t)a1 >> 12;
  a0 = (int32_t)a0 >> 12;
  a3 = at + a0;
  t0 = v0 - a1;
  t0 += v1;
  cop2.VXY0 = t0;
  cop2.VZ0 = a3;
  a3 = at + a1;
  t0 = v0 + a0;
  t0 += v1;
  cop2.VXY1 = t0;
  cop2.VZ1 = a3;
  a3 = at - a1;
  t0 = v0 - a0;
  t0 += v1;
  cop2.VXY2 = t0;
  cop2.VZ2 = a3;
  a3 = at - a0;
  t0 = v0 + a1;
  RTPT();
  t0 += v1;
  t2 = lhu(ra - 0x0010); // 0xFFFFFFF0
  t4 = lw(ra - 0x0014); // 0xFFFFFFEC
  t3 = t2 >> 8;
  t2 = t2 & 0xFF;
  t2 = t2 << 3;
  t2 += t6;
  t1 = lw(t2 + 0x0004);
  t2 = lw(t2 + 0x0008);
  at = cop2.SXY0;
  v0 = cop2.SXY1;
  v1 = cop2.SXY2;
  cop2.VXY0 = t0;
  cop2.VZ0 = a3;
  a1 = cop2.SZ1;
  a2 = cop2.SZ2;
  a3 = cop2.SZ3;
  RTPS();
  sw(fp + 0x0004, t4);
  t4 = 0x09000000;
  sw(fp + 0x0000, t4);
  sw(fp + 0x000C, t1);
  sw(fp + 0x0024, t2);
  t4 = t1 & 0xFF;
  t5 = t2 & 0xFF00;
  t4 = t4 | t5;
  sw(fp + 0x001C, t4);
  t4 = t2 & 0xFF00;
  t2 = t2 ^ t4;
  t5 = t1 & 0xFF00;
  t2 = t2 | t5;
  sw(fp + 0x0014, t2);
  a0 = cop2.SXY2;
  t0 = cop2.SZ3;
  a1 += a2;
  a1 += a3;
  a1 += t0;
  a2 = a1 - 512; // 0xFFFFFE00
  temp = (int32_t)a2 < 0;
  a2 = a1 - 32768; // 0xFFFF8000
  if (temp) goto label800574C0;
  temp = (int32_t)a2 >= 0;
  a1 = a1 >> 7;
  if (temp) goto label800574C0;
  t3 = a1 - t3;
  temp = (int32_t)t3 < 0;
  t3 = t3 << 3;
  if (temp) goto label800574C0;
  sw(fp + 0x0008, at);
  sw(fp + 0x0010, v0);
  sw(fp + 0x0018, v1);
  sw(fp + 0x0020, a0);
  a1 = at - s4;
  temp = (int32_t)a1 > 0;
  a1 = v0 - s4;
  if (temp) goto label800576AC;
  temp = (int32_t)a1 > 0;
  a1 = v1 - s4;
  if (temp) goto label800576AC;
  temp = (int32_t)a1 > 0;
  a1 = a0 - s4;
  if (temp) goto label800576AC;
  temp = (int32_t)a1 <= 0;
  if (temp) {
    a1 = at - s5;
    goto label800574C0;
  }
label800576AC:
  a1 = at - s5;
  temp = (int32_t)a1 < 0;
  a1 = v0 - s5;
  if (temp) goto label800576CC;
  temp = (int32_t)a1 < 0;
  a1 = v1 - s5;
  if (temp) goto label800576CC;
  temp = (int32_t)a1 < 0;
  a1 = a0 - s5;
  if (temp) goto label800576CC;
  temp = (int32_t)a1 >= 0;
  if (temp) {
    a1 = at << 16;
    goto label800574C0;
  }
label800576CC:
  a1 = at << 16;
  temp = (int32_t)a1 > 0;
  a1 = v0 << 16;
  if (temp) goto label800576EC;
  temp = (int32_t)a1 > 0;
  a1 = v1 << 16;
  if (temp) goto label800576EC;
  temp = (int32_t)a1 > 0;
  a1 = a0 << 16;
  if (temp) goto label800576EC;
  temp = (int32_t)a1 <= 0;
  if (temp) {
    a1 = at << 16;
    goto label800574C0;
  }
label800576EC:
  a1 = at << 16;
  a1 -= s6;
  temp = (int32_t)a1 < 0;
  a1 = v0 << 16;
  if (temp) goto label8005771C;
  a1 -= s6;
  temp = (int32_t)a1 < 0;
  a1 = v1 << 16;
  if (temp) goto label8005771C;
  a1 -= s6;
  temp = (int32_t)a1 < 0;
  a1 = a0 << 16;
  if (temp) goto label8005771C;
  a1 -= s6;
  temp = (int32_t)a1 >= 0;
  if (temp) {
    a1 = t3 + sp;
    goto label800574C0;
  }
label8005771C:
  a1 = t3 + sp;
  sb(ra - 0x001D, s2); // 0xFFFFFFE3
  at = lw(a1 + 0x0000);
  sw(a1 + 0x0000, fp);
  temp = at == 0;
  v0 = fp >> 16;
  if (temp) goto label80057744;
  sh(at + 0x0000, fp);
  sb(at + 0x0002, v0);
  fp += 40; // 0x0028
  goto label800574C0;
label80057744:
  sw(a1 + 0x0004, fp);
  fp += 40; // 0x0028
  goto label800574C0;
label80057750:
  a1 = lw(ra + 0x0004);
  a2 = lw(ra + 0x0008);
  at = t7 - fp;
  temp = (int32_t)at <= 0;
  at = gp & 0xFF;
  if (temp) goto label8005809C;
  at = at << 2;
  at += s1;
  at = lw(at + 0x0000);
  v0 = a2 >> 16;
  v0 = v0 & 0xFF;
  v1 = a2 >> 23;
  v1 += 64; // 0x0040
  a0 = v1 & 0x1FE;
  gp = lh(ra + 0x0020);
  ra += 32; // 0x0020
  v1 = a0 + s0;
  a0 = v1 + 128; // 0x0080
  v1 = lh(v1 + 0x0000);
  a0 = lh(a0 + 0x0000);
  cop2.IR0 = v0;
  cop2.IR1 = v1;
  cop2.IR2 = a0;
  v0 = a1 << 16;
  v0 = (int32_t)v0 >> 14;
  GPF(SF_OFF, LM_OFF);
  v1 = (int32_t)a1 >> 14;
  a0 = a2 << 16;
  a0 = (int32_t)a0 >> 14;
  a1 = a0 >> 7;
  cop2.TRX = v0;
  cop2.TRY = v1;
  cop2.TRZ = a0;
  a2 = cop2.MAC1;
  a3 = cop2.MAC2;
  a2 = (int32_t)a2 >> 10;
  a3 = (int32_t)a3 >> 10;
  cop2.VZ0 = 0;
  t0 = -a2;
  t1 = -a3;
  t1 = t1 << 16;
  t0 = t0 & 0xFFFF;
  t0 += t1;
  cop2.VXY0 = t0;
  cop2.VZ1 = 0;
  t1 = -a2;
  t1 = t1 << 16;
  t0 = a3 & 0xFFFF;
  t0 += t1;
  cop2.VXY1 = t0;
  cop2.VZ2 = 0;
  t0 = -a3;
  t1 = a2 << 16;
  t0 = t0 & 0xFFFF;
  t0 += t1;
  cop2.VXY2 = t0;
  v0 = a2;
  v1 = a3;
  a2 = cop2.RTM0;
  a3 = cop2.RTM1;
  t0 = cop2.RTM2;
  t1 = cop2.RTM3;
  t2 = cop2.RTM4;
  cop2.RTM1 = 0;
  cop2.RTM3 = 0;
  t3 = 4096; // 0x1000
  cop2.RTM0 = t3;
  cop2.RTM4 = t3;
  t3 = 2560; // 0x0A00
  cop2.RTM2 = t3;
  v1 = v1 << 16;
  v0 = v0 & 0xFFFF;
  RTPT();
  v0 += v1;
  t3 = lhu(ra - 0x0010); // 0xFFFFFFF0
  v1 = lw(ra - 0x0014); // 0xFFFFFFEC
  t4 = t3 >> 8;
  a1 -= t4;
  temp = (int32_t)a1 < 0;
  t4 = t3 & 0xFF;
  if (temp) goto label800574C0;
  t4 = t4 << 3;
  at += t4;
  t3 = lw(at + 0x0004);
  t4 = lw(at + 0x0008);
  at = cop2.SXY0;
  cop2.VXY0 = v0;
  cop2.VZ0 = 0;
  v0 = cop2.SXY1;
  a0 = cop2.SXY2;
  RTPS();
  sw(fp + 0x0008, at);
  sw(fp + 0x0010, v0);
  sw(fp + 0x0018, a0);
  sw(fp + 0x0004, v1);
  at = 0x09000000;
  sw(fp + 0x0000, at);
  sw(fp + 0x000C, t3);
  sw(fp + 0x0024, t4);
  at = t3 & 0xFF;
  v0 = t4 & 0xFF00;
  at = at | v0;
  sw(fp + 0x001C, at);
  at = t4 & 0xFF00;
  t4 = t4 ^ at;
  at = t3 & 0xFF00;
  t4 = t4 | at;
  at = cop2.SXY2;
  sw(fp + 0x0014, t4);
  sw(fp + 0x0020, at);
  cop2.RTM0 = a2;
  cop2.RTM1 = a3;
  cop2.RTM2 = t0;
  cop2.RTM3 = t1;
  cop2.RTM4 = t2;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  a1 = a1 << 3;
  a1 += sp;
  at = lw(a1 + 0x0000);
  sw(a1 + 0x0000, fp);
  temp = at == 0;
  v0 = fp >> 16;
  if (temp) goto label80057948;
  sh(at + 0x0000, fp);
  sb(at + 0x0002, v0);
  fp += 40; // 0x0028
  goto label800574C0;
label80057948:
  sw(a1 + 0x0004, fp);
  fp += 40; // 0x0028
  goto label800574C0;
label80057954:
  at = lw(ra + 0x0004);
  a0 = lw(ra + 0x0008);
  v0 = (int32_t)at >> 16;
  at = at << 16;
  at = (int32_t)at >> 16;
  v1 = a0 << 16;
  v1 = (int32_t)v1 >> 16;
  cop2.TRX = at;
  cop2.TRY = v0;
  cop2.TRZ = v1;
  cop2.VXY0 = 0;
  cop2.VZ0 = 0;
  at = t7 - fp;
  temp = (int32_t)at <= 0;
  at = (int32_t)a0 >> 16;
  if (temp) goto label8005809C;
  RTPS();
  v1 = lw(ra + 0x000C);
  gp = lh(ra + 0x0020);
  ra += 32; // 0x0020
  v0 = v1 << 16;
  v0 = (int32_t)v0 >> 16;
  v1 = (int32_t)v1 >> 16;
  sb(ra - 0x001D, 0); // 0xFFFFFFE3
  cop2.TRX = at;
  cop2.TRY = v0;
  cop2.TRZ = v1;
  v0 = cop2.SZ3;
  at = cop2.SXY2;
  RTPS();
  a0 = lw(ra - 0x0010); // 0xFFFFFFF0
  a1 = lw(ra - 0x000C); // 0xFFFFFFF4
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  v1 = v0 - 64; // 0xFFFFFFC0
  temp = (int32_t)v1 <= 0;
  v1 = v0 - 2048; // 0xFFFFF800
  if (temp) goto label800574C0;
  temp = (int32_t)v1 >= 0;
  v1 = at - s4;
  if (temp) goto label800574C0;
  temp = (int32_t)v1 <= 0;
  v1 = at - s5;
  if (temp) goto label800574C0;
  temp = (int32_t)v1 >= 0;
  v1 = at << 16;
  if (temp) goto label800574C0;
  temp = (int32_t)v1 <= 0;
  v1 -= s6;
  if (temp) goto label800574C0;
  temp = (int32_t)v1 >= 0;
  v0 = v0 >> 5;
  if (temp) goto label800574C0;
  sb(ra - 0x001D, s2); // 0xFFFFFFE3
  v1 = cop2.SXY2;
  sw(fp + 0x0008, at);
  sw(fp + 0x0010, v1);
  sw(fp + 0x0004, a0);
  sw(fp + 0x000C, a1);
  a1 = a1 >> 24;
  v0 -= a1;
  temp = (int32_t)v0 >= 0;
  at = 0x04000000;
  if (temp) goto label80057A3C;
  v0 = 0;
label80057A3C:
  sw(fp + 0x0000, at);
  v0 = v0 << 3;
  v1 = v0 + sp;
  at = lw(v1 + 0x0000);
  sw(v1 + 0x0000, fp);
  temp = at == 0;
  v0 = fp >> 16;
  if (temp) goto label80057A68;
  sh(at + 0x0000, fp);
  sb(at + 0x0002, v0);
  fp += 20; // 0x0014
  goto label800574C0;
label80057A68:
  sw(v1 + 0x0004, fp);
  fp += 20; // 0x0014
  goto label800574C0;
label80057A74:
  at = lw(ra + 0x0004);
  a0 = lw(ra + 0x0008);
  v0 = t7 - fp;
  temp = (int32_t)v0 <= 0;
  v0 = at >> 16;
  if (temp) goto label8005809C;
  at = at & 0xFFFF;
  at -= s7;
  v0 = t8 - v0;
  cop2.VZ0 = at;
  v1 = a0 & 0xFFFF;
  v1 = t9 - v1;
  v1 = v1 << 16;
  v0 += v1;
  cop2.VXY0 = v0;
  sb(ra + 0x0003, 0);
  at = gp & 0xFF;
  RTPS();
  at = at << 2;
  at += s1;
  at = lw(at + 0x0000);
  v0 = a0 >> 16;
  v0 = v0 & 0xFF;
  v1 = a0 >> 24;
  cop2.IR1 = v0;
  cop2.IR2 = v1;
  gp = lh(ra + 0x0020);
  ra += 32; // 0x0020
  v1 = cop2.SZ3;
  v0 = cop2.SXY2;
  a0 = v1 - 128; // 0xFFFFFF80
  temp = (int32_t)a0 < 0;
  a0 = v1 - 8192; // 0xFFFFE000
  if (temp) goto label800574C0;
  temp = (int32_t)a0 >= 0;
  a0 = v0 - s4;
  if (temp) goto label800574C0;
  temp = (int32_t)a0 <= 0;
  a0 = v0 - s5;
  if (temp) goto label800574C0;
  a1 = lhu(ra - 0x0010); // 0xFFFFFFF0
  temp = (int32_t)a0 >= 0;
  a0 = v0 << 16;
  if (temp) goto label800574C0;
  temp = (int32_t)a0 <= 0;
  a0 -= s6;
  if (temp) goto label800574C0;
  temp = (int32_t)a0 >= 0;
  v1 = v1 >> 5;
  if (temp) goto label800574C0;
  sb(ra - 0x001D, s2); // 0xFFFFFFE3
  a0 = cop2.MAC0;
  a2 = a1 >> 8;
  a0 = a0 >> 12;
  cop2.IR0 = a0;
  v1 -= a2;
  temp = (int32_t)v1 < 0;
  a1 = a1 & 0xFF;
  if (temp) goto label800574C0;
  GPF(SF_OFF, LM_OFF);
  a1 = a1 << 3;
  at += a1;
  a0 = lw(at + 0x0004);
  a1 = lw(at + 0x0008);
  a2 = cop2.MAC1;
  a3 = cop2.MAC2;
  a2 = a2 >> 12;
  a3 = a3 >> 12;
  t0 = a2 >> 1;
  at = lw(ra - 0x0014); // 0xFFFFFFEC
  t1 = a3 >> 1;
  a3 = a3 << 16;
  t1 = t1 << 16;
  v0 += t0;
  v0 += t1;
  sw(fp + 0x0004, at);
  sw(fp + 0x0020, v0);
  at = v0 - a2;
  sw(fp + 0x0018, at);
  at = v0 - a3;
  sw(fp + 0x0010, at);
  at -= a2;
  sw(fp + 0x0008, at);
  sw(fp + 0x000C, a0);
  sw(fp + 0x0024, a1);
  at = a0 & 0xFF;
  v0 = a1 & 0xFF00;
  at = at | v0;
  sw(fp + 0x001C, at);
  at = a1 & 0xFF00;
  a1 = a1 ^ at;
  at = a0 & 0xFF00;
  a1 = a1 | at;
  sw(fp + 0x0014, a1);
  at = 0x09000000;
  sw(fp + 0x0000, at);
  v1 = v1 << 3;
  v1 += sp;
  at = lw(v1 + 0x0000);
  sw(v1 + 0x0000, fp);
  temp = at == 0;
  v0 = fp >> 16;
  if (temp) goto label80057BFC;
  sh(at + 0x0000, fp);
  sb(at + 0x0002, v0);
  fp += 40; // 0x0028
  goto label800574C0;
label80057BFC:
  sw(v1 + 0x0004, fp);
  fp += 40; // 0x0028
  goto label800574C0;
label80057C08:
  at = lw(ra + 0x0004);
  a0 = lw(ra + 0x0008);
  v0 = t7 - fp;
  temp = (int32_t)v0 <= 0;
  v0 = at >> 16;
  if (temp) goto label8005809C;
  at = at & 0xFFFF;
  at -= s7;
  v0 = t8 - v0;
  cop2.VZ0 = at;
  v1 = a0 & 0xFFFF;
  v1 = t9 - v1;
  v1 = v1 << 16;
  v0 += v1;
  sb(ra + 0x0003, 0);
  cop2.VXY0 = v0;
  at = gp & 0xFF;
  at = at << 2;
  RTPS();
  at += s1;
  at = lw(at + 0x0000);
  v0 = a0 >> 16;
  v0 = v0 & 0xFF;
  v1 = a0 >> 23;
  v1 += 64; // 0x0040
  a0 = v1 & 0x1FE;
  gp = lh(ra + 0x0020);
  ra += 32; // 0x0020
  v1 = a0 + s0;
  a0 = v1 + 128; // 0x0080
  a1 = cop2.SZ3;
  a2 = cop2.SXY2;
  v1 = lh(v1 + 0x0000);
  a3 = a1 - 128; // 0xFFFFFF80
  temp = (int32_t)a3 < 0;
  a3 = a1 - 8192; // 0xFFFFE000
  if (temp) goto label800574C0;
  temp = (int32_t)a3 >= 0;
  a3 = a2 - s4;
  if (temp) goto label800574C0;
  temp = (int32_t)a3 <= 0;
  a3 = a2 - s5;
  if (temp) goto label800574C0;
  a0 = lh(a0 + 0x0000);
  temp = (int32_t)a3 >= 0;
  a3 = a2 << 16;
  if (temp) goto label800574C0;
  temp = (int32_t)a3 <= 0;
  a3 -= s6;
  if (temp) goto label800574C0;
  temp = (int32_t)a3 >= 0;
  a1 = a1 >> 5;
  if (temp) goto label800574C0;
  cop2.IR0 = v0;
  cop2.IR1 = v1;
  cop2.IR2 = a0;
  v0 = cop2.MAC1;
  v1 = cop2.MAC2;
  a0 = cop2.MAC3;
  GPF(SF_OFF, LM_OFF);
  sb(ra - 0x001D, s2); // 0xFFFFFFE3
  v0 = v0 << 2;
  v1 = v1 << 2;
  a0 = a0 << 2;
  cop2.TRX = v0;
  cop2.TRY = v1;
  cop2.TRZ = a0;
  a2 = cop2.MAC1;
  a3 = cop2.MAC2;
  a2 = (int32_t)a2 >> 10;
  a3 = (int32_t)a3 >> 10;
  cop2.VZ0 = 0;
  t0 = -a2;
  t1 = -a3;
  t1 = t1 << 16;
  t0 = t0 & 0xFFFF;
  t0 += t1;
  cop2.VXY0 = t0;
  cop2.VZ1 = 0;
  t1 = -a2;
  t1 = t1 << 16;
  t0 = a3 & 0xFFFF;
  t0 += t1;
  cop2.VXY1 = t0;
  cop2.VZ2 = 0;
  t0 = -a3;
  t1 = a2 << 16;
  t0 = t0 & 0xFFFF;
  t0 += t1;
  cop2.VXY2 = t0;
  v0 = a2;
  v1 = a3;
  a2 = cop2.RTM0;
  a3 = cop2.RTM1;
  t0 = cop2.RTM2;
  t1 = cop2.RTM3;
  t2 = cop2.RTM4;
  cop2.RTM1 = 0;
  cop2.RTM3 = 0;
  t3 = 4096; // 0x1000
  cop2.RTM0 = t3;
  cop2.RTM4 = t3;
  t3 = 2560; // 0x0A00
  cop2.RTM2 = t3;
  v1 = v1 << 16;
  v0 = v0 & 0xFFFF;
  RTPT();
  v0 += v1;
  t3 = lhu(ra - 0x0010); // 0xFFFFFFF0
  v1 = lw(ra - 0x0014); // 0xFFFFFFEC
  t4 = t3 >> 8;
  a1 -= t4;
  temp = (int32_t)a1 < 0;
  t4 = t3 & 0xFF;
  if (temp) goto label800574C0;
  t4 = t4 << 3;
  at += t4;
  t3 = lw(at + 0x0004);
  t4 = lw(at + 0x0008);
  at = cop2.SXY0;
  cop2.VXY0 = v0;
  cop2.VZ0 = 0;
  v0 = cop2.SXY1;
  a0 = cop2.SXY2;
  RTPS();
  sw(fp + 0x0008, at);
  sw(fp + 0x0010, v0);
  sw(fp + 0x0018, a0);
  sw(fp + 0x0004, v1);
  at = 0x09000000;
  sw(fp + 0x0000, at);
  sw(fp + 0x000C, t3);
  sw(fp + 0x0024, t4);
  at = t3 & 0xFF;
  v0 = t4 & 0xFF00;
  at = at | v0;
  sw(fp + 0x001C, at);
  at = t4 & 0xFF00;
  t4 = t4 ^ at;
  at = t3 & 0xFF00;
  t4 = t4 | at;
  at = cop2.SXY2;
  sw(fp + 0x0014, t4);
  sw(fp + 0x0020, at);
  cop2.RTM0 = a2;
  cop2.RTM1 = a3;
  cop2.RTM2 = t0;
  cop2.RTM3 = t1;
  cop2.RTM4 = t2;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  a1 = a1 << 3;
  a1 += sp;
  at = lw(a1 + 0x0000);
  sw(a1 + 0x0000, fp);
  temp = at == 0;
  v0 = fp >> 16;
  if (temp) goto label80057E70;
  sh(at + 0x0000, fp);
  sb(at + 0x0002, v0);
  fp += 40; // 0x0028
  goto label800574C0;
label80057E70:
  sw(a1 + 0x0004, fp);
  fp += 40; // 0x0028
  goto label800574C0;
label80057E7C:
  at = lw(ra + 0x0004);
  a0 = lw(ra + 0x0008);
  v0 = at >> 16;
  at = at & 0xFFFF;
  at -= s7;
  v0 = t8 - v0;
  v1 = t7 - fp;
  temp = (int32_t)v1 <= 0;
  v1 = a0 & 0xFFFF;
  if (temp) goto label8005809C;
  v1 = t9 - v1;
  v1 = v1 << 16;
  v0 += v1;
  cop2.VXY0 = v0;
  cop2.VZ0 = at;
  at = a0 >> 16;
  at -= s7;
  RTPS();
  v1 = lw(ra + 0x000C);
  gp = lh(ra + 0x0020);
  ra += 32; // 0x0020
  v0 = v1 & 0xFFFF;
  v0 = t8 - v0;
  v1 = v1 >> 16;
  v1 = t9 - v1;
  v1 = v1 << 16;
  sb(ra - 0x001D, 0); // 0xFFFFFFE3
  v0 += v1;
  cop2.VXY0 = v0;
  cop2.VZ0 = at;
  v0 = cop2.SZ3;
  at = cop2.SXY2;
  RTPS();
  a0 = lw(ra - 0x0010); // 0xFFFFFFF0
  temp = v0 == 0;
  v1 = v0 - 8192; // 0xFFFFE000
  if (temp) goto label800574C0;
  temp = (int32_t)v1 >= 0;
  v1 = at - s4;
  if (temp) goto label800574C0;
  temp = (int32_t)v1 <= 0;
  v1 = at - s5;
  if (temp) goto label800574C0;
  temp = (int32_t)v1 >= 0;
  a1 = lw(ra - 0x000C); // 0xFFFFFFF4
  if (temp) goto label800574C0;
  v1 = at << 16;
  temp = (int32_t)v1 <= 0;
  v1 -= s6;
  if (temp) goto label800574C0;
  temp = (int32_t)v1 >= 0;
  v0 = v0 >> 5;
  if (temp) goto label800574C0;
  v1 = cop2.SXY2;
  sw(fp + 0x0008, at);
  sw(fp + 0x0010, v1);
  sw(fp + 0x0004, a0);
  sw(fp + 0x000C, a1);
  a1 = a1 >> 24;
  v0 -= a1;
  at = v0 - s3;
  temp = (int32_t)at <= 0;
  at = 0x04000000;
  if (temp) goto label800574C0;
  sw(fp + 0x0000, at);
  sb(ra - 0x001D, s2); // 0xFFFFFFE3
  v0 = v0 << 3;
  v1 = v0 + sp;
  at = lw(v1 + 0x0000);
  sw(v1 + 0x0000, fp);
  temp = at == 0;
  v0 = fp >> 16;
  if (temp) goto label80057F8C;
  sh(at + 0x0000, fp);
  sb(at + 0x0002, v0);
  fp += 20; // 0x0014
  goto label800574C0;
label80057F8C:
  sw(v1 + 0x0004, fp);
  fp += 20; // 0x0014
  goto label800574C0;
label80057F98:
  at = lw(ra + 0x0004);
  a0 = lw(ra + 0x0008);
  v0 = at >> 16;
  at = at & 0xFFFF;
  at -= s7;
  v0 = t8 - v0;
  v1 = t7 - fp;
  temp = (int32_t)v1 <= 0;
  v1 = a0 & 0xFFFF;
  if (temp) goto label8005809C;
  v1 = t9 - v1;
  v1 = v1 << 16;
  v0 += v1;
  cop2.VXY0 = v0;
  cop2.VZ0 = at;
  at = a0 >> 16;
  at = at & 0xFF;
  RTPS();
  v0 = lw(ra + 0x000C);
  gp = lh(ra + 0x0020);
  sw(fp + 0x0004, v0);
  sb(ra + 0x0003, 0);
  ra += 32; // 0x0020
  v1 = cop2.SZ3;
  v0 = cop2.SXY2;
  temp = v1 == 0;
  a0 = v1 - 8192; // 0xFFFFE000
  if (temp) goto label800574C0;
  temp = (int32_t)a0 >= 0;
  a0 = v0 - s4;
  if (temp) goto label800574C0;
  temp = (int32_t)a0 <= 0;
  a0 = v0 - s5;
  if (temp) goto label800574C0;
  temp = (int32_t)a0 >= 0;
  a0 = v0 << 16;
  if (temp) goto label800574C0;
  temp = (int32_t)a0 <= 0;
  a0 -= s6;
  if (temp) goto label800574C0;
  temp = (int32_t)a0 >= 0;
  v1 = v1 >> 5;
  if (temp) goto label800574C0;
  sb(ra - 0x001D, s2); // 0xFFFFFFE3
  sw(fp + 0x0008, v0);
  v0++;
  sw(fp + 0x000C, v0);
  v0 = 0x03000000;
  sw(fp + 0x0000, v0);
  v1 -= at;
  v0 = v1 - s3;
  temp = (int32_t)v0 <= 0;
  v1 = v1 << 3;
  if (temp) goto label800574C0;
  v1 += sp;
  at = lw(v1 + 0x0000);
  sw(v1 + 0x0000, fp);
  temp = at == 0;
  v0 = fp >> 16;
  if (temp) goto label80058074;
  sh(at + 0x0000, fp);
  sb(at + 0x0002, v0);
  fp += 16; // 0x0010
  goto label800574C0;
label80058074:
  sw(v1 + 0x0004, fp);
  fp += 16; // 0x0010
  goto label800574C0;
label80058080:
  gp = lh(ra + 0x0020);
  ra += 32; // 0x0020
  at++;
  temp = at != 0;
  if (temp) goto label800574C0;
  goto label800580AC;
label8005809C:
  at = 0x800758B0;
  v0 = 1; // 0x0001
  sw(at + 0x0000, v0);
label800580AC:
  at = allocator1_ptr;
  sw(at + 0x0000, fp);
  at = 0x1F800000;
  ra = lw(at + 0x002C);
  fp = lw(at + 0x0028);
  sp = lw(at + 0x0024);
  gp = lw(at + 0x0020);
  s7 = lw(at + 0x001C);
  s6 = lw(at + 0x0018);
  s5 = lw(at + 0x0014);
  s4 = lw(at + 0x0010);
  s3 = lw(at + 0x000C);
  s2 = lw(at + 0x0008);
  s1 = lw(at + 0x0004);
  s0 = lw(at + 0x0000);
  return;
}

// size: 0x00000368
void function_800584C4(void)
{
  uint32_t temp;
  t7 = a0;
  a2 = 0x80076DD0;
  at = lw(a2 + 0x0000);
  v0 = lw(a2 + 0x0004);
  v1 = lw(a2 + 0x0008);
  a0 = lw(a2 + 0x000C);
  a1 = lw(a2 + 0x0010);
  cop2.RTM0 = at;
  cop2.RTM1 = v0;
  cop2.RTM2 = v1;
  cop2.RTM3 = a0;
  cop2.RTM4 = a1;
  cop2.LLM0 = at;
  cop2.LLM1 = v0;
  cop2.LLM2 = v1;
  cop2.LLM3 = a0;
  cop2.LLM4 = a1;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  cop2.DQB = 0;
  at = 256; // 0x0100
  cop2.DQA = at;
  t1 = spyro_sin_lut;
  t2 = allocator1_ptr;
  t2 = lw(t2 + 0x0000);
  t3 = ordered_linked_list;
  t3 = lw(t3 + 0x0000);
  t4 = lw(a2 + 0x0028);
  t5 = lw(a2 + 0x002C);
  t6 = lw(a2 + 0x0030);
  t8 = 0x80077108;
  t8 -= 24; // 0xFFFFFFE8
  t9 = t8 + 192; // 0x00C0
label80058560:
  temp = t8 == t9;
  t8 += 24; // 0x0018
  if (temp) goto label8005881C;
  at = lw(t8 + 0x000C);
  v1 = at & 0xFF;
  v1 -= t7;
  v0 = (int32_t)at >> 24;
  temp = (int32_t)v1 > 0;
  mult(v0, t7);
  if (temp) goto label8005858C;
  sb(t8 + 0x000C, 0);
  goto label80058560;
label8005858C:
  sb(t8 + 0x000C, v1);
  a0 = at >> 8;
  a0 = a0 & 0xFF;
  v1 = a0 - v1;
  temp = (int32_t)v1 >= 0;
  v1 = v1 << 8;
  if (temp) goto label800585A8;
  v1 = -v1;
label800585A8:
  at = at >> 16;
  at = at & 0xFF;
  v0=lo;
  at += v0;
  at = at & 0xFF;
  sb(t8 + 0x000E, at);
  div_psx(v1,a0);
  v0 = lw(t8 + 0x0000);
  v1 = lw(t8 + 0x0004);
  a0 = lw(t8 + 0x0008);
  v0 -= t4;
  v1 = t5 - v1;
  a0 = t6 - a0;
  v0 = (int32_t)v0 >> 2;
  v1 = (int32_t)v1 >> 2;
  a0 = (int32_t)a0 >> 2;
  a0 = a0 << 16;
  v1 = v1 & 0xFFFF;
  v1 = v1 | a0;
  cop2.VXY0 = v1;
  cop2.VZ0 = v0;
  at = at << 1;
  at += t1;
  RTPS();
  a0 = lw(t8 + 0x0014);
  v1 = lw(t8 + 0x0010);
  a1 = a0 & 0xFF00;
  a0 = a0 & 0xFF;
  a2 = cop2.SZ3;
  a3 = cop2.SXY2;
  a1 -= a2;
  temp = (int32_t)a1 <= 0;
  a1 = a2 - 128; // 0xFFFFFF80
  if (temp) goto label80058814;
  temp = (int32_t)a1 <= 0;
  t0 = 0x00010000;
  if (temp) goto label80058814;
  a1 = a3 - t0;
  temp = (int32_t)a1 <= 0;
  t0 = 0x01000000;
  if (temp) goto label80058814;
  a1 = a3 - t0;
  temp = (int32_t)a1 >= 0;
  a1 = a3 << 16;
  if (temp) goto label80058814;
  temp = (int32_t)a1 <= 0;
  t0 = 0x02000000;
  if (temp) goto label80058814;
  a1 -= t0;
  temp = (int32_t)a1 >= 0;
  a1 = 0x40000000;
  if (temp) goto label80058814;
  v1 = v1 | a1;
  sw(t2 + 0x0004, v1);
  sw(t2 + 0x0014, v1);
  v1=lo;
  v0 = 256; // 0x0100
  v1 = v0 - v1;
  mult(v1, a0);
  v0 = lh(at + 0x0000);
  at = lh(at + 0x0080);
  a0=lo;
  cop2.IR0 = a0;
  cop2.IR2 = v0;
  cop2.IR1 = at;
  at = cop2.MAC1;
  v0 = cop2.MAC2;
  v1 = cop2.MAC3;
  GPF(SF_OFF, LM_OFF);
  a3 = v1 - 4096; // 0xFFFFF000
  temp = (int32_t)a3 < 0;
  a3 = 4096; // 0x1000
  if (temp) goto label800586B4;
  a3 = v1;
label800586B4:
  at = at << 1;
  v0 = v0 << 1;
  v1 = v1 << 1;
  cop2.TRX = at;
  cop2.TRY = v0;
  cop2.TRZ = v1;
  at = cop2.MAC1;
  v0 = cop2.MAC2;
  at = (int32_t)at >> 19;
  v0 = (int32_t)v0 >> 19;
  cop2.VZ0 = 0;
  v1 = -at;
  a0 = -v0;
  a0 = a0 << 16;
  v1 = v1 & 0xFFFF;
  v1 += a0;
  cop2.VXY0 = v1;
  cop2.VZ1 = 0;
  a0 = -at;
  a0 = a0 << 16;
  v1 = v0 & 0xFFFF;
  v1 += a0;
  cop2.VXY1 = v1;
  cop2.VZ2 = 0;
  v1 = -v0;
  a0 = at << 16;
  v1 = v1 & 0xFFFF;
  v1 += a0;
  cop2.VXY2 = v1;
  cop2.RTM1 = 0;
  cop2.RTM3 = 0;
  cop2.RTM0 = a3;
  cop2.RTM2 = a3;
  cop2.RTM4 = a3;
  v0 = v0 << 16;
  at = at & 0xFFFF;
  RTPT();
  v0 += at;
  at = cop2.SXY0;
  cop2.VXY0 = v0;
  cop2.VZ0 = 0;
  v0 = cop2.SXY1;
  v1 = cop2.SXY2;
  RTPS();
  sw(t2 + 0x0008, at);
  sw(t2 + 0x0018, v0);
  sw(t2 + 0x001C, v1);
  a2 = a2 >> 5;
  a2 -= 6; // 0xFFFFFFFA
  temp = (int32_t)a2 >= 0;
  at = 0x03000000;
  if (temp) goto label80058784;
  a2 = 0;
label80058784:
  a1 = a2 - 256; // 0xFFFFFF00
  temp = (int32_t)a1 <= 0;
  sw(t2 + 0x0000, at);
  if (temp) goto label80058794;
  a2 += 70; // 0x0046
label80058794:
  at = 0x83000000;
  at = at ^ t2;
  sw(t2 + 0x0010, at);
  a2 = a2 << 3;
  at = cop2.SXY2;
  a2 += t3;
  sw(t2 + 0x000C, at);
  at = cop2.LLM0;
  v0 = cop2.LLM1;
  v1 = cop2.LLM2;
  a0 = cop2.LLM3;
  a1 = cop2.LLM4;
  cop2.RTM0 = at;
  cop2.RTM1 = v0;
  cop2.RTM2 = v1;
  cop2.RTM3 = a0;
  cop2.RTM4 = a1;
  cop2.TRX = 0;
  cop2.TRY = 0;
  cop2.TRZ = 0;
  at = lw(a2 + 0x0000);
  sw(a2 + 0x0000, t2);
  temp = at == 0;
  v0 = t2 + 16; // 0x0010
  if (temp) goto label80058808;
  v1 = v0 >> 16;
  sh(at + 0x0000, v0);
  sb(at + 0x0002, v1);
  t2 += 32; // 0x0020
  goto label80058560;
label80058808:
  sw(a2 + 0x0004, v0);
  t2 += 32; // 0x0020
  goto label80058560;
label80058814:
  sb(t8 + 0x000C, 0);
  goto label80058560;
label8005881C:
  at = allocator1_ptr;
  sw(at + 0x0000, t2);
  return;
}

// size: 0x00000C94
void function_8002A6FC(void)
{
  uint32_t temp;
  t9 = 0x80078560;
  t8 = a0;
  t7 = lw(t9 + 0x0000);
  t6 = lw(t9 + 0x0004);
  t7 = t7 << 2;
  t7 += t6;
label8002A718:
  temp = t6 == t7;
  t5 = lw(t6 + 0x0000);
  if (temp) goto label8002A840;
  t6 += 4; // 0x0004
  at = lbu(t5 + 0x0001);
  v0 = lbu(t5 + 0x0003);
  v1 = at & 0x2;
  temp = (int32_t)v1 > 0;
  t4 = t8;
  if (temp) goto label8002A718;
  v0 -= t4;
  temp = (int32_t)v0 <= 0;
  t4 = -v0;
  if (temp) goto label8002A74C;
  sb(t5 + 0x0003, v0);
  goto label8002A718;
label8002A74C:
  v1 = lbu(t5 + 0x0002);
  t3 = t5 + 8; // 0x0008
  a0 = v1 << 2;
  a0 += t3;
  at = at & 0x1;
  a0 += at;
  v1 = lbu(a0 + 0x0001);
label8002A768:
  a0 = v1 << 2;
  a0 += t3;
  v0 = lbu(a0 + 0x0000);
  at = v0 & 0x3;
  sb(t5 + 0x0001, at);
  a1 = at & 0x2;
  temp = (int32_t)a1 > 0;
  v0 = v0 >> 2;
  if (temp) goto label8002A7AC;
  v0 -= t4;
  temp = (int32_t)v0 > 0;
  t4 = -v0;
  if (temp) goto label8002A7AC;
  at = at & 0x1;
  a0 += at;
  v1 = lbu(a0 + 0x0001);
  goto label8002A768;
label8002A7AC:
  sb(t5 + 0x0003, v0);
  sb(t5 + 0x0002, v1);
  v1 = lbu(a0 + 0x0003);
  a0 = lw(t5 + 0x0004);
  at = 0x800785A8;
  v0 = lw(at + 0x001C);
  at = lw(at + 0x0018);
  a1 = v1 << 4;
  a2 = a0 << 4;
  a1 += at;
  a2 += at;
  at = 168; // 0x00A8
  mult(at, v1);
  a3 = lw(a1 + 0x0000);
  t0 = lw(a1 + 0x0004);
  t1 = lw(a1 + 0x0008);
  t2 = lw(a1 + 0x000C);
  v1=lo;
  sw(a2 + 0x0000, a3);
  sw(a2 + 0x0004, t0);
  mult(at, a0);
  sw(a2 + 0x0008, t1);
  sw(a2 + 0x000C, t2);
  at = v0 + v1;
  v0 += lo;
  v1 = at + 0xA8;
label8002A81C:
  a0 = lw(at + 0x0000);
  a1 = lw(at + 0x0004);
  sw(v0 + 0x0000, a0);
  sw(v0 + 0x0004, a1);
  at += 8; // 0x0008
  temp = at != v1;
  v0 += 8; // 0x0008
  if (temp) goto label8002A81C;
  goto label8002A718;
label8002A840:
  t7 = lw(t9 + 0x0008);
  t6 = lw(t9 + 0x000C);
  t7 = t7 << 2;
  t7 += t6;
label8002A850:
  temp = t6 == t7;
  t5 = lw(t6 + 0x0000);
  if (temp) goto label8002AA18;
  t6 += 4; // 0x0004
  at = lbu(t5 + 0x0001);
  v0 = lbu(t5 + 0x0003);
  v1 = at & 0x2;
  temp = (int32_t)v1 > 0;
  t4 = t8;
  if (temp) goto label8002A850;
  v0 -= t4;
  temp = (int32_t)v0 <= 0;
  t4 = -v0;
  if (temp) goto label8002A884;
  sb(t5 + 0x0003, v0);
  goto label8002A850;
label8002A884:
  v1 = lbu(t5 + 0x0002);
  t3 = t5 + 8; // 0x0008
  a0 = v1 << 2;
  a0 += t3;
  at = at & 0x1;
  a0 += at;
  v1 = lbu(a0 + 0x0001);
  a2 = 0;
label8002A8A4:
  a0 = v1 << 2;
  a0 += t3;
  v0 = lbu(a0 + 0x0000);
  a3 = lb(a0 + 0x0003);
  at = v0 & 0x3;
  sb(t5 + 0x0001, at);
  a2 += a3;
  a1 = at & 0x2;
  temp = (int32_t)a1 > 0;
  v0 = v0 >> 2;
  if (temp) goto label8002A8EC;
  v0 -= t4;
  temp = (int32_t)v0 > 0;
  t4 = -v0;
  if (temp) goto label8002A8EC;
  at = at & 0x1;
  a0 += at;
  v1 = lbu(a0 + 0x0001);
  goto label8002A8A4;
label8002A8EC:
  sb(t5 + 0x0003, v0);
  sb(t5 + 0x0002, v1);
  v1 = lhu(t5 + 0x0004);
  at = 0x800785A8;
  v0 = 168; // 0x00A8
  mult(v0, v1);
  v0 = lw(at + 0x001C);
  at = lw(at + 0x0018);
  a0 = lhu(t5 + 0x0006);
  v1 = v1 << 4;
  at += v1;
  v1=lo;
  v0 += v1;
  v1 = a0 + a2;
  v1 = v1 & 0x7F;
  sh(t5 + 0x0006, v1);
  a0 = v1 >> 2;
  sb(at + 0x0001, a0);
  sb(at + 0x0005, a0);
  sb(at + 0x0009, a0);
  sb(at + 0x000D, a0);
  sb(v0 + 0x0001, a0);
  sb(v0 + 0x0005, a0);
  a0 = v1 >> 1;
  sb(v0 + 0x0009, a0);
  sb(v0 + 0x000D, a0);
  sb(v0 + 0x0011, a0);
  sb(v0 + 0x0015, a0);
  a0 += 32; // 0x0020
  a0 = a0 & 0x3F;
  sb(v0 + 0x0019, a0);
  sb(v0 + 0x001D, a0);
  sb(v0 + 0x0021, a0);
  sb(v0 + 0x0025, a0);
  a0 = v1 >> 1;
  sb(v0 + 0x0029, a0);
  sb(v0 + 0x002D, a0);
  sb(v0 + 0x0031, a0);
  sb(v0 + 0x0035, a0);
  sb(v0 + 0x0039, a0);
  sb(v0 + 0x003D, a0);
  sb(v0 + 0x0041, a0);
  sb(v0 + 0x0045, a0);
  a0 += 16; // 0x0010
  a0 = a0 & 0x3F;
  sb(v0 + 0x0049, a0);
  sb(v0 + 0x004D, a0);
  sb(v0 + 0x0051, a0);
  sb(v0 + 0x0055, a0);
  sb(v0 + 0x0059, a0);
  sb(v0 + 0x005D, a0);
  sb(v0 + 0x0061, a0);
  sb(v0 + 0x0065, a0);
  a0 += 16; // 0x0010
  a0 = a0 & 0x3F;
  sb(v0 + 0x0069, a0);
  sb(v0 + 0x006D, a0);
  sb(v0 + 0x0071, a0);
  sb(v0 + 0x0075, a0);
  sb(v0 + 0x0079, a0);
  sb(v0 + 0x007D, a0);
  sb(v0 + 0x0081, a0);
  sb(v0 + 0x0085, a0);
  a0 += 16; // 0x0010
  a0 = a0 & 0x3F;
  sb(v0 + 0x0089, a0);
  sb(v0 + 0x008D, a0);
  sb(v0 + 0x0091, a0);
  sb(v0 + 0x0095, a0);
  sb(v0 + 0x0099, a0);
  sb(v0 + 0x009D, a0);
  sb(v0 + 0x00A1, a0);
  sb(v0 + 0x00A5, a0);
  goto label8002A850;
label8002AA18:
  t7 = lw(t9 + 0x0010);
  t6 = lw(t9 + 0x0014);
  t5 = -1; // 0xFFFFFFFF
  t7 = t7 << 2;
  t7 += t6;
label8002AA2C:
  temp = t6 == t7;
  t5++;
  if (temp) goto label8002AAE8;
  t4 = lw(t6 + 0x0000);
  t6 += 4; // 0x0004
  at = lbu(t4 + 0x0001);
  v0 = lbu(t4 + 0x0003);
  v1 = at & 0x2;
  temp = (int32_t)v1 > 0;
  t3 = t8;
  if (temp) goto label8002AA2C;
  v0 -= t3;
  temp = (int32_t)v0 <= 0;
  t3 = -v0;
  if (temp) goto label8002AA64;
  sb(t4 + 0x0003, v0);
  goto label8002AA2C;
label8002AA64:
  v1 = lbu(t4 + 0x0002);
  t2 = t4 + 12; // 0x000C
  a0 = v1 << 3;
  a0 += t2;
  at = at & 0x1;
  a0 += at;
  v1 = lbu(a0 + 0x0002);
label8002AA80:
  a0 = v1 << 3;
  a0 += t2;
  at = lbu(a0 + 0x0000);
  v0 = lbu(a0 + 0x0001);
  a1 = at & 0x2;
  temp = (int32_t)a1 > 0;
  sb(t4 + 0x0001, at);
  if (temp) goto label8002AABC;
  v0 -= t3;
  temp = (int32_t)v0 > 0;
  t3 = -v0;
  if (temp) goto label8002AABC;
  at = at & 0x1;
  a0 += at;
  v1 = lbu(a0 + 0x0002);
  goto label8002AA80;
label8002AABC:
  sb(t4 + 0x0003, v0);
  sb(t4 + 0x0002, v1);
  at = lhu(t4 + 0x0004);
  v0 = 0x800785A8;
  v0 = lw(v0 + 0x0000);
  at = at << 2;
  v0 += at;
  at = lw(v0 + 0x0000);
  sb(at + 0x0018, t5);
  goto label8002AA2C;
label8002AAE8:
  t7 = lw(t9 + 0x0018);
  t6 = lw(t9 + 0x001C);
  t5 = -1; // 0xFFFFFFFF
  t7 = t7 << 2;
  t7 += t6;
label8002AAFC:
  temp = t6 == t7;
  t5++;
  if (temp) goto label8002ABB8;
  t4 = lw(t6 + 0x0000);
  t6 += 4; // 0x0004
  at = lbu(t4 + 0x0001);
  v0 = lbu(t4 + 0x0003);
  v1 = at & 0x2;
  temp = (int32_t)v1 > 0;
  t3 = t8;
  if (temp) goto label8002AAFC;
  v0 -= t3;
  temp = (int32_t)v0 <= 0;
  t3 = -v0;
  if (temp) goto label8002AB34;
  sb(t4 + 0x0003, v0);
  goto label8002AAFC;
label8002AB34:
  v1 = lbu(t4 + 0x0002);
  t2 = t4 + 12; // 0x000C
  a0 = v1 << 3;
  a0 += t2;
  at = at & 0x1;
  a0 += at;
  v1 = lbu(a0 + 0x0002);
label8002AB50:
  a0 = v1 << 3;
  a0 += t2;
  at = lbu(a0 + 0x0000);
  v0 = lbu(a0 + 0x0001);
  a1 = at & 0x2;
  temp = (int32_t)a1 > 0;
  sb(t4 + 0x0001, at);
  if (temp) goto label8002AB8C;
  v0 -= t3;
  temp = (int32_t)v0 > 0;
  t3 = -v0;
  if (temp) goto label8002AB8C;
  at = at & 0x1;
  a0 += at;
  v1 = lbu(a0 + 0x0002);
  goto label8002AB50;
label8002AB8C:
  sb(t4 + 0x0003, v0);
  sb(t4 + 0x0002, v1);
  at = lhu(t4 + 0x0004);
  v0 = 0x800785A8;
  v0 = lw(v0 + 0x0000);
  at = at << 2;
  v0 += at;
  at = lw(v0 + 0x0000);
  sb(at + 0x0019, t5);
  goto label8002AAFC;
label8002ABB8:
  t7 = lw(t9 + 0x0020);
  t6 = lw(t9 + 0x0024);
  t5 = -1; // 0xFFFFFFFF
  t7 = t7 << 2;
  t7 += t6;
label8002ABCC:
  temp = t6 == t7;
  t5++;
  if (temp) goto label8002AC88;
  t4 = lw(t6 + 0x0000);
  t6 += 4; // 0x0004
  at = lbu(t4 + 0x0001);
  v0 = lbu(t4 + 0x0003);
  v1 = at & 0x2;
  temp = (int32_t)v1 > 0;
  t3 = t8;
  if (temp) goto label8002ABCC;
  v0 -= t3;
  temp = (int32_t)v0 <= 0;
  t3 = -v0;
  if (temp) goto label8002AC04;
  sb(t4 + 0x0003, v0);
  goto label8002ABCC;
label8002AC04:
  v1 = lbu(t4 + 0x0002);
  t2 = t4 + 12; // 0x000C
  a0 = v1 << 3;
  a0 += t2;
  at = at & 0x1;
  a0 += at;
  v1 = lbu(a0 + 0x0002);
label8002AC20:
  a0 = v1 << 3;
  a0 += t2;
  at = lbu(a0 + 0x0000);
  v0 = lbu(a0 + 0x0001);
  a1 = at & 0x2;
  temp = (int32_t)a1 > 0;
  sb(t4 + 0x0001, at);
  if (temp) goto label8002AC5C;
  v0 -= t3;
  temp = (int32_t)v0 > 0;
  t3 = -v0;
  if (temp) goto label8002AC5C;
  at = at & 0x1;
  a0 += at;
  v1 = lbu(a0 + 0x0002);
  goto label8002AC20;
label8002AC5C:
  sb(t4 + 0x0003, v0);
  sb(t4 + 0x0002, v1);
  at = lhu(t4 + 0x0004);
  v0 = 0x800785A8;
  v0 = lw(v0 + 0x0000);
  at = at << 2;
  v0 += at;
  at = lw(v0 + 0x0000);
  sb(at + 0x001A, t5);
  goto label8002ABCC;
label8002AC88:
  t7 = lw(t9 + 0x0028);
  t6 = lw(t9 + 0x002C);
  t5 = -1; // 0xFFFFFFFF
  t7 = t7 << 2;
  t7 += t6;
label8002AC9C:
  temp = t6 == t7;
  t5++;
  if (temp) goto label8002AD58;
  t4 = lw(t6 + 0x0000);
  t6 += 4; // 0x0004
  at = lbu(t4 + 0x0001);
  v0 = lbu(t4 + 0x0003);
  v1 = at & 0x2;
  temp = (int32_t)v1 > 0;
  t3 = t8;
  if (temp) goto label8002AC9C;
  v0 -= t3;
  temp = (int32_t)v0 <= 0;
  t3 = -v0;
  if (temp) goto label8002ACD4;
  sb(t4 + 0x0003, v0);
  goto label8002AC9C;
label8002ACD4:
  v1 = lbu(t4 + 0x0002);
  t2 = t4 + 12; // 0x000C
  a0 = v1 << 3;
  a0 += t2;
  at = at & 0x1;
  a0 += at;
  v1 = lbu(a0 + 0x0002);
label8002ACF0:
  a0 = v1 << 3;
  a0 += t2;
  at = lbu(a0 + 0x0000);
  v0 = lbu(a0 + 0x0001);
  a1 = at & 0x2;
  temp = (int32_t)a1 > 0;
  sb(t4 + 0x0001, at);
  if (temp) goto label8002AD2C;
  v0 -= t3;
  temp = (int32_t)v0 > 0;
  t3 = -v0;
  if (temp) goto label8002AD2C;
  at = at & 0x1;
  a0 += at;
  v1 = lbu(a0 + 0x0002);
  goto label8002ACF0;
label8002AD2C:
  sb(t4 + 0x0003, v0);
  sb(t4 + 0x0002, v1);
  at = lhu(t4 + 0x0004);
  v0 = 0x800785A8;
  v0 = lw(v0 + 0x0000);
  at = at << 2;
  v0 += at;
  at = lw(v0 + 0x0000);
  sb(at + 0x001B, t5);
  goto label8002AC9C;
label8002AD58:
  t7 = lw(t9 + 0x0040);
  t6 = lw(t9 + 0x0044);
  t7 = t7 << 2;
  t7 += t6;
label8002AD68:
  temp = t6 == t7;
  t5 = lw(t6 + 0x0000);
  if (temp) goto label8002B388;
  t6 += 4; // 0x0004
  at = lbu(t5 + 0x0001);
  v0 = lbu(t5 + 0x0003);
  v1 = at & 0x2;
  temp = (int32_t)v1 > 0;
  t3 = t8;
  if (temp) goto label8002AD68;
  v0 -= t3;
  temp = (int32_t)v0 <= 0;
  t3 = -v0;
  if (temp) goto label8002AD9C;
  sb(t5 + 0x0003, v0);
  goto label8002AD68;
label8002AD9C:
  v1 = lbu(t5 + 0x0002);
  t2 = t5 + 12; // 0x000C
  a0 = v1 << 3;
  a0 += t2;
  at = at & 0x1;
  a0 += at;
  v1 = lbu(a0 + 0x0002);
label8002ADB8:
  a0 = v1 << 3;
  a0 += t2;
  at = lbu(a0 + 0x0000);
  v0 = lbu(a0 + 0x0001);
  a1 = at & 0x2;
  temp = (int32_t)a1 > 0;
  sb(t5 + 0x0001, at);
  if (temp) goto label8002ADF4;
  v0 -= t3;
  temp = (int32_t)v0 > 0;
  t3 = -v0;
  if (temp) goto label8002ADF4;
  at = at & 0x1;
  a0 += at;
  v1 = lbu(a0 + 0x0002);
  goto label8002ADB8;
label8002ADF4:
  sb(t5 + 0x0003, v0);
  sb(t5 + 0x0002, v1);
  t2 = lw(a0 + 0x0004);
  at = lw(t5 + 0x0004);
  t4 = t2 & 0xFF;
  t3 = t2 >> 8;
  t3 = t3 & 0xFF;
  t2 = t2 >> 16;
  t2 = t2 & 0xFF;
  a1 = lw(t5 + 0x0008);
  v0 = at & 0xFFFF;
  at = at >> 16;
  v1 = v0 << 2;
  v0 = v0 << 3;
  v0 += v1;
  a2 = 0x800785A8;
  a1 += t5;
  temp = (int32_t)t4 > 0;
  a2 = lw(a2 + 0x002C);
  if (temp) goto label8002AFE4;
  mult(v0, t3);
  v1 = at << 2;
  at = at << 3;
  at += v1;
  t3=lo;
  a2 = lw(a2 + 0x0010);
  a0 = a1 + t3;
  a0 += v0;
  v0 = a1 + t3;
  at += a2;
label8002AE6C:
  t3 = lw(v0 + 0x0008);
  t0 = lw(v0 + 0x0004);
  a1 = lw(v0 + 0x0000);
  t4 = t3 << 9;
  t4 = (int32_t)t4 >> 23;
  temp = (int32_t)t4 < 0;
  t5 = (int32_t)t3 >> 23;
  if (temp) goto label8002AEB0;
  temp = (int32_t)t5 < 0;
  t3 = t3 & 0x3FFF;
  if (temp) goto label8002AEB0;
  sw(at + 0x0000, a1);
  sw(at + 0x0004, t0);
  t4 = t4 << 16;
  t5 = t5 << 24;
  t3 = t3 | t4;
  t3 = t3 | t5;
  sw(at + 0x0008, t3);
  goto label8002AFD0;
label8002AEB0:
  v1 = t5 - t4;
  t3 = t3 & 0x3FFF;
  t5 += t3;
  t4 += t3;
  a3 = (int32_t)a1 >> 23;
  a2 = a1 << 9;
  a2 = (int32_t)a2 >> 23;
  a1 = a1 & 0x3FFF;
  a3 += a1;
  a2 += a1;
  t2 = (int32_t)t0 >> 23;
  t1 = t0 << 9;
  t1 = (int32_t)t1 >> 23;
  t0 = t0 & 0x3FFF;
  t2 += t0;
  temp = (int32_t)v1 < 0;
  t1 += t0;
  if (temp) goto label8002AF64;
  a1 -= a2;
  a1 = a1 << 23;
  a3 -= a2;
  a3 = a3 & 0x1FF;
  a3 = a3 << 14;
  a2 = a2 & 0x3FFF;
  a2 = a2 | a1;
  a2 = a2 | a3;
  sw(at + 0x0000, a2);
  t0 -= t1;
  t0 = t0 << 23;
  t2 -= t1;
  t2 = t2 & 0x1FF;
  t2 = t2 << 14;
  t1 = t1 & 0x3FFF;
  t1 = t1 | t0;
  t1 = t1 | t2;
  sw(at + 0x0004, t1);
  t3 -= t4;
  t3 = t3 << 24;
  t5 -= t4;
  t5 = t5 & 0x1FF;
  t5 = t5 << 16;
  t4 = t4 & 0x3FFF;
  t4 = t4 | t3;
  t4 = t4 | t5;
  sw(at + 0x0008, t4);
  goto label8002AFD0;
label8002AF64:
  a2 -= a3;
  a2 = a2 << 23;
  a1 -= a3;
  a1 = a1 & 0x1FF;
  a1 = a1 << 14;
  a3 = a3 & 0x3FFF;
  a3 = a3 | a2;
  a3 = a3 | a1;
  sw(at + 0x0000, a3);
  t1 -= t2;
  t1 = t1 << 23;
  t0 -= t2;
  t0 = t0 & 0x1FF;
  t0 = t0 << 14;
  t2 = t2 & 0x3FFF;
  t2 = t2 | t1;
  t2 = t2 | t0;
  sw(at + 0x0004, t2);
  t4 -= t5;
  t4 = t4 << 24;
  t3 -= t5;
  t3 = t3 & 0x1FF;
  t3 = t3 << 16;
  t5 = t5 & 0x3FFF;
  t5 = t5 | t4;
  t5 = t5 | t3;
  sw(at + 0x0008, t5);
label8002AFD0:
  v0 += 12; // 0x000C
  temp = v0 != a0;
  at += 12; // 0x000C
  if (temp) goto label8002AE6C;
  goto label8002AD68;
label8002AFE4:
  mult(v0, t2);
  v1 = at << 2;
  at = at << 3;
  at += v1;
  t2=lo;
  a2 = lw(a2 + 0x0010);
  v1 = a1 + t2;
  mult(v0, t3);
  at += a2;
  t3=lo;
  a0 = a1 + t3;
  a0 += v0;
  v0 = a1 + t3;
  a1 = t4 << 4;
  a2 = 4096; // 0x1000
  a2 -= a1;
label8002B024:
  a3 = lw(v0 + 0x0000);
  t2 = lw(v1 + 0x0000);
  t1 = (int32_t)a3 >> 23;
  t0 = a3 << 9;
  t0 = (int32_t)t0 >> 23;
  a3 = a3 & 0x3FFF;
  t0 += a3;
  t1 += a3;
  cop2.IR0 = a2;
  cop2.IR1 = a3;
  cop2.IR2 = t0;
  cop2.IR3 = t1;
  t4 = (int32_t)t2 >> 23;
  t3 = t2 << 9;
  GPF(SF_OFF, LM_OFF);
  t3 = (int32_t)t3 >> 23;
  t2 = t2 & 0x3FFF;
  t3 += t2;
  t4 += t2;
  cop2.IR0 = a1;
  cop2.IR1 = t2;
  cop2.IR2 = t3;
  cop2.IR3 = t4;
  t0 = 0x1F800000;
  GPL(SF_OFF, LM_OFF);
  a3 = lw(v0 + 0x0004);
  t3 = cop2.MAC1;
  t4 = cop2.MAC2;
  t5 = cop2.MAC3;
  t2 = lw(v1 + 0x0004);
  t3 = (int32_t)t3 >> 12;
  t4 = (int32_t)t4 >> 12;
  t5 = (int32_t)t5 >> 12;
  sw(t0 + 0x0000, t3);
  sw(t0 + 0x0004, t4);
  sw(t0 + 0x0008, t5);
  t1 = (int32_t)a3 >> 23;
  t0 = a3 << 9;
  t0 = (int32_t)t0 >> 23;
  a3 = a3 & 0x3FFF;
  t0 += a3;
  t1 += a3;
  cop2.IR0 = a2;
  cop2.IR1 = a3;
  cop2.IR2 = t0;
  cop2.IR3 = t1;
  t4 = (int32_t)t2 >> 23;
  t3 = t2 << 9;
  GPF(SF_OFF, LM_OFF);
  t3 = (int32_t)t3 >> 23;
  t2 = t2 & 0x3FFF;
  t3 += t2;
  t4 += t2;
  cop2.IR0 = a1;
  cop2.IR1 = t2;
  cop2.IR2 = t3;
  cop2.IR3 = t4;
  t0 = 0x1F800000;
  GPL(SF_OFF, LM_OFF);
  a3 = lw(v0 + 0x0008);
  t3 = cop2.MAC1;
  t4 = cop2.MAC2;
  t5 = cop2.MAC3;
  t2 = lw(v1 + 0x0008);
  t3 = (int32_t)t3 >> 12;
  t4 = (int32_t)t4 >> 12;
  t5 = (int32_t)t5 >> 12;
  sw(t0 + 0x000C, t3);
  sw(t0 + 0x0010, t4);
  sw(t0 + 0x0014, t5);
  t1 = (int32_t)a3 >> 23;
  t0 = a3 << 9;
  t0 = (int32_t)t0 >> 23;
  a3 = a3 & 0x3FFF;
  t0 += a3;
  t1 += a3;
  cop2.IR0 = a2;
  cop2.IR1 = a3;
  cop2.IR2 = t0;
  cop2.IR3 = t1;
  t4 = (int32_t)t2 >> 23;
  t3 = t2 << 9;
  GPF(SF_OFF, LM_OFF);
  t3 = (int32_t)t3 >> 23;
  t2 = t2 & 0x3FFF;
  t3 += t2;
  t4 += t2;
  cop2.IR0 = a1;
  cop2.IR1 = t2;
  cop2.IR2 = t3;
  cop2.IR3 = t4;
  t0 = 0x1F800000;
  GPL(SF_OFF, LM_OFF);
  t3 = cop2.MAC1;
  t4 = cop2.MAC2;
  t5 = cop2.MAC3;
  t3 = t3 >> 12;
  t4 = t4 >> 12;
  t5 = t5 >> 12;
  a3 = t4 - t3;
  temp = (int32_t)a3 < 0;
  a3 = t5 - t3;
  if (temp) goto label8002B258;
  temp = (int32_t)a3 < 0;
  a3 = 0x1F800000;
  if (temp) goto label8002B258;
  t5 -= t3;
  t5 = t5 << 24;
  t4 -= t3;
  t4 = t4 & 0xFF;
  t4 = t4 << 16;
  t3 = t3 & 0x3FFF;
  t3 = t3 | t4;
  t3 = t3 | t5;
  sw(at + 0x0008, t3);
  t3 = lw(a3 + 0x000C);
  t5 = lw(a3 + 0x0014);
  t4 = lw(a3 + 0x0010);
  t5 -= t3;
  t5 = t5 << 23;
  t4 -= t3;
  t4 = t4 & 0x1FF;
  t4 = t4 << 14;
  t3 = t3 & 0x3FFF;
  t3 = t3 | t4;
  t3 = t3 | t5;
  sw(at + 0x0004, t3);
  t3 = lw(a3 + 0x0000);
  t5 = lw(a3 + 0x0008);
  t4 = lw(a3 + 0x0004);
  t5 -= t3;
  t5 = t5 << 23;
  t4 -= t3;
  t4 = t4 & 0x1FF;
  t4 = t4 << 14;
  t3 = t3 & 0x3FFF;
  t3 = t3 | t4;
  t3 = t3 | t5;
  sw(at + 0x0000, t3);
  goto label8002B370;
label8002B258:
  a3 = t5 - t4;
  temp = (int32_t)a3 < 0;
  a3 = 0x1F800000;
  if (temp) goto label8002B2EC;
  t3 -= t4;
  t3 = t3 << 24;
  t5 -= t4;
  t5 = t5 & 0xFF;
  t5 = t5 << 16;
  t4 = t4 & 0x3FFF;
  t4 = t4 | t5;
  t4 = t4 | t3;
  sw(at + 0x0008, t4);
  t3 = lw(a3 + 0x000C);
  t4 = lw(a3 + 0x0010);
  t5 = lw(a3 + 0x0014);
  t3 -= t4;
  t3 = t3 << 23;
  t5 -= t4;
  t5 = t5 & 0x1FF;
  t5 = t5 << 14;
  t4 = t4 & 0x3FFF;
  t4 = t4 | t5;
  t4 = t4 | t3;
  sw(at + 0x0004, t4);
  t3 = lw(a3 + 0x0000);
  t4 = lw(a3 + 0x0004);
  t5 = lw(a3 + 0x0008);
  t3 -= t4;
  t3 = t3 << 23;
  t5 -= t4;
  t5 = t5 & 0x1FF;
  t5 = t5 << 14;
  t4 = t4 & 0x3FFF;
  t4 = t4 | t5;
  t4 = t4 | t3;
  sw(at + 0x0000, t4);
  goto label8002B370;
label8002B2EC:
  t4 -= t5;
  t4 = t4 << 24;
  t3 -= t5;
  t3 = t3 & 0xFF;
  t3 = t3 << 16;
  t5 = t5 & 0x3FFF;
  t5 = t5 | t3;
  t5 = t5 | t4;
  sw(at + 0x0008, t5);
  t4 = lw(a3 + 0x0010);
  t5 = lw(a3 + 0x0014);
  t3 = lw(a3 + 0x000C);
  t4 -= t5;
  t4 = t4 << 23;
  t3 -= t5;
  t3 = t3 & 0x1FF;
  t3 = t3 << 14;
  t5 = t5 & 0x3FFF;
  t5 = t5 | t3;
  t5 = t5 | t4;
  sw(at + 0x0004, t5);
  t4 = lw(a3 + 0x0004);
  t5 = lw(a3 + 0x0008);
  t3 = lw(a3 + 0x0000);
  t4 -= t5;
  t4 = t4 << 23;
  t3 -= t5;
  t3 = t3 & 0x1FF;
  t3 = t3 << 14;
  t5 = t5 & 0x3FFF;
  t5 = t5 | t3;
  t5 = t5 | t4;
  sw(at + 0x0000, t5);
label8002B370:
  v0 += 12; // 0x000C
  v1 += 12; // 0x000C
  temp = v0 != a0;
  at += 12; // 0x000C
  if (temp) goto label8002B024;
  goto label8002AD68;
label8002B388:
  return;
}

// size: 0x00000520
void function_8002B4AC(void)
{
  uint32_t temp;
  t9 = 0x80078560;
  t7 = lw(t9 + 0x0000);
  t6 = lw(t9 + 0x0004);
  t7 = t7 << 2;
  t7 += t6;
label8002B4C4:
  temp = t6 == t7;
  t5 = lw(t6 + 0x0000);
  if (temp) goto label8002B56C;
  t6 += 4; // 0x0004
  t3 = lbu(t5 + 0x0002);
  t4 = t5 + 8; // 0x0008
  t3 = t3 << 2;
  t4 += t3;
  v1 = lbu(t4 + 0x0003);
  a0 = lw(t5 + 0x0004);
  at = 0x800785A8;
  v0 = lw(at + 0x001C);
  at = lw(at + 0x0018);
  a1 = v1 << 4;
  a2 = a0 << 4;
  a1 += at;
  a2 += at;
  at = 168; // 0x00A8
  mult(at, v1);
  a3 = lw(a1 + 0x0000);
  t0 = lw(a1 + 0x0004);
  t1 = lw(a1 + 0x0008);
  t2 = lw(a1 + 0x000C);
  v1=lo;
  sw(a2 + 0x0000, a3);
  sw(a2 + 0x0004, t0);
  mult(at, a0);
  sw(a2 + 0x0008, t1);
  sw(a2 + 0x000C, t2);
  at = v0 + v1;
  v1=lo;
  v0 += v1;
  v1 = at + 168; // 0x00A8
label8002B548:
  a0 = lw(at + 0x0000);
  a1 = lw(at + 0x0004);
  sw(v0 + 0x0000, a0);
  sw(v0 + 0x0004, a1);
  at += 8; // 0x0008
  temp = at != v1;
  v0 += 8; // 0x0008
  if (temp) goto label8002B548;
  goto label8002B4C4;
label8002B56C:
  t7 = lw(t9 + 0x0008);
  t6 = lw(t9 + 0x000C);
  t7 = t7 << 2;
  t7 += t6;
label8002B57C:
  temp = t6 == t7;
  t5 = lw(t6 + 0x0000);
  if (temp) goto label8002B6A0;
  t6 += 4; // 0x0004
  a0 = lhu(t5 + 0x0004);
  at = 0x800785A8;
  v0 = 168; // 0x00A8
  mult(v0, a0);
  v0 = lw(at + 0x001C);
  at = lw(at + 0x0018);
  v1 = lhu(t5 + 0x0006);
  a0 = a0 << 4;
  at += a0;
  a0=lo;
  v0 += a0;
  a0 = v1 >> 2;
  sb(at + 0x0001, a0);
  sb(at + 0x0005, a0);
  sb(at + 0x0009, a0);
  sb(at + 0x000D, a0);
  sb(v0 + 0x0001, a0);
  sb(v0 + 0x0005, a0);
  a0 = v1 >> 1;
  sb(v0 + 0x0009, a0);
  sb(v0 + 0x000D, a0);
  sb(v0 + 0x0011, a0);
  sb(v0 + 0x0015, a0);
  a0 += 32; // 0x0020
  a0 = a0 & 0x3F;
  sb(v0 + 0x0019, a0);
  sb(v0 + 0x001D, a0);
  sb(v0 + 0x0021, a0);
  sb(v0 + 0x0025, a0);
  a0 = v1 >> 1;
  sb(v0 + 0x0029, a0);
  sb(v0 + 0x002D, a0);
  sb(v0 + 0x0031, a0);
  sb(v0 + 0x0035, a0);
  sb(v0 + 0x0039, a0);
  sb(v0 + 0x003D, a0);
  sb(v0 + 0x0041, a0);
  sb(v0 + 0x0045, a0);
  a0 += 16; // 0x0010
  a0 = a0 & 0x3F;
  sb(v0 + 0x0049, a0);
  sb(v0 + 0x004D, a0);
  sb(v0 + 0x0051, a0);
  sb(v0 + 0x0055, a0);
  sb(v0 + 0x0059, a0);
  sb(v0 + 0x005D, a0);
  sb(v0 + 0x0061, a0);
  sb(v0 + 0x0065, a0);
  a0 += 16; // 0x0010
  a0 = a0 & 0x3F;
  sb(v0 + 0x0069, a0);
  sb(v0 + 0x006D, a0);
  sb(v0 + 0x0071, a0);
  sb(v0 + 0x0075, a0);
  sb(v0 + 0x0079, a0);
  sb(v0 + 0x007D, a0);
  sb(v0 + 0x0081, a0);
  sb(v0 + 0x0085, a0);
  a0 += 16; // 0x0010
  a0 = a0 & 0x3F;
  sb(v0 + 0x0089, a0);
  sb(v0 + 0x008D, a0);
  sb(v0 + 0x0091, a0);
  sb(v0 + 0x0095, a0);
  sb(v0 + 0x0099, a0);
  sb(v0 + 0x009D, a0);
  sb(v0 + 0x00A1, a0);
  sb(v0 + 0x00A5, a0);
  goto label8002B57C;
label8002B6A0:
  t7 = lw(t9 + 0x0010);
  t6 = lw(t9 + 0x0014);
  t5 = -1; // 0xFFFFFFFF
  t7 = t7 << 2;
  t7 += t6;
label8002B6B4:
  temp = t6 == t7;
  t5++;
  if (temp) goto label8002B6E8;
  t4 = lw(t6 + 0x0000);
  t6 += 4; // 0x0004
  at = lhu(t4 + 0x0004);
  v0 = 0x800785A8;
  v0 = lw(v0 + 0x0000);
  at = at << 2;
  v0 += at;
  at = lw(v0 + 0x0000);
  sb(at + 0x0018, t5);
  goto label8002B6B4;
label8002B6E8:
  t7 = lw(t9 + 0x0018);
  t6 = lw(t9 + 0x001C);
  t5 = -1; // 0xFFFFFFFF
  t7 = t7 << 2;
  t7 += t6;
label8002B6FC:
  temp = t6 == t7;
  t5++;
  if (temp) goto label8002B730;
  t4 = lw(t6 + 0x0000);
  t6 += 4; // 0x0004
  at = lhu(t4 + 0x0004);
  v0 = 0x800785A8;
  v0 = lw(v0 + 0x0000);
  at = at << 2;
  v0 += at;
  at = lw(v0 + 0x0000);
  sb(at + 0x0019, t5);
  goto label8002B6FC;
label8002B730:
  t7 = lw(t9 + 0x0020);
  t6 = lw(t9 + 0x0024);
  t5 = -1; // 0xFFFFFFFF
  t7 = t7 << 2;
  t7 += t6;
label8002B744:
  temp = t6 == t7;
  t5++;
  if (temp) goto label8002B778;
  t4 = lw(t6 + 0x0000);
  t6 += 4; // 0x0004
  at = lhu(t4 + 0x0004);
  v0 = 0x800785A8;
  v0 = lw(v0 + 0x0000);
  at = at << 2;
  v0 += at;
  at = lw(v0 + 0x0000);
  sb(at + 0x001A, t5);
  goto label8002B744;
label8002B778:
  t7 = lw(t9 + 0x0028);
  t6 = lw(t9 + 0x002C);
  t5 = -1; // 0xFFFFFFFF
  t7 = t7 << 2;
  t7 += t6;
label8002B78C:
  temp = t6 == t7;
  t5++;
  if (temp) goto label8002B7C0;
  t4 = lw(t6 + 0x0000);
  t6 += 4; // 0x0004
  at = lhu(t4 + 0x0004);
  v0 = 0x800785A8;
  v0 = lw(v0 + 0x0000);
  at = at << 2;
  v0 += at;
  at = lw(v0 + 0x0000);
  sb(at + 0x001B, t5);
  goto label8002B78C;
label8002B7C0:
  t7 = lw(t9 + 0x0040);
  t6 = lw(t9 + 0x0044);
  t7 = t7 << 2;
  t7 += t6;
label8002B7D0:
  temp = t6 == t7;
  t5 = lw(t6 + 0x0000);
  if (temp) goto label8002B9C4;
  t6 += 4; // 0x0004
  t3 = lbu(t5 + 0x0002);
  t4 = t5 + 12; // 0x000C
  t3 = t3 << 3;
  t4 += t3;
  t2 = lw(t4 + 0x0004);
  at = lw(t5 + 0x0004);
  t3 = t2 >> 8;
  t3 = t3 & 0xFF;
  a1 = lw(t5 + 0x0008);
  v0 = at & 0xFFFF;
  at = at >> 16;
  v1 = v0 << 2;
  v0 = v0 << 3;
  v0 += v1;
  a2 = 0x800785A8;
  a1 += t5;
  a2 = lw(a2 + 0x002C);
  mult(v0, t3);
  v1 = at << 2;
  at = at << 3;
  at += v1;
  t3=lo;
  a2 = lw(a2 + 0x0010);
  a0 = a1 + t3;
  a0 += v0;
  v0 = a1 + t3;
  at += a2;
label8002B84C:
  t3 = lw(v0 + 0x0008);
  t0 = lw(v0 + 0x0004);
  a1 = lw(v0 + 0x0000);
  t4 = t3 << 9;
  t4 = (int32_t)t4 >> 23;
  temp = (int32_t)t4 < 0;
  t5 = (int32_t)t3 >> 23;
  if (temp) goto label8002B890;
  temp = (int32_t)t5 < 0;
  t3 = t3 & 0x3FFF;
  if (temp) goto label8002B890;
  sw(at + 0x0000, a1);
  sw(at + 0x0004, t0);
  t4 = t4 << 16;
  t5 = t5 << 24;
  t3 = t3 | t4;
  t3 = t3 | t5;
  sw(at + 0x0008, t3);
  goto label8002B9B0;
label8002B890:
  v1 = t5 - t4;
  t3 = t3 & 0x3FFF;
  t5 += t3;
  t4 += t3;
  a3 = (int32_t)a1 >> 23;
  a2 = a1 << 9;
  a2 = (int32_t)a2 >> 23;
  a1 = a1 & 0x3FFF;
  a3 += a1;
  a2 += a1;
  t2 = (int32_t)t0 >> 23;
  t1 = t0 << 9;
  t1 = (int32_t)t1 >> 23;
  t0 = t0 & 0x3FFF;
  t2 += t0;
  temp = (int32_t)v1 < 0;
  t1 += t0;
  if (temp) goto label8002B944;
  a1 -= a2;
  a1 = a1 << 23;
  a3 -= a2;
  a3 = a3 & 0x1FF;
  a3 = a3 << 14;
  a2 = a2 & 0x3FFF;
  a2 = a2 | a1;
  a2 = a2 | a3;
  sw(at + 0x0000, a2);
  t0 -= t1;
  t0 = t0 << 23;
  t2 -= t1;
  t2 = t2 & 0x1FF;
  t2 = t2 << 14;
  t1 = t1 & 0x3FFF;
  t1 = t1 | t0;
  t1 = t1 | t2;
  sw(at + 0x0004, t1);
  t3 -= t4;
  t3 = t3 << 24;
  t5 -= t4;
  t5 = t5 & 0x1FF;
  t5 = t5 << 16;
  t4 = t4 & 0x3FFF;
  t4 = t4 | t3;
  t4 = t4 | t5;
  sw(at + 0x0008, t4);
  goto label8002B9B0;
label8002B944:
  a2 -= a3;
  a2 = a2 << 23;
  a1 -= a3;
  a1 = a1 & 0x1FF;
  a1 = a1 << 14;
  a3 = a3 & 0x3FFF;
  a3 = a3 | a2;
  a3 = a3 | a1;
  sw(at + 0x0000, a3);
  t1 -= t2;
  t1 = t1 << 23;
  t0 -= t2;
  t0 = t0 & 0x1FF;
  t0 = t0 << 14;
  t2 = t2 & 0x3FFF;
  t2 = t2 | t1;
  t2 = t2 | t0;
  sw(at + 0x0004, t2);
  t4 -= t5;
  t4 = t4 << 24;
  t3 -= t5;
  t3 = t3 & 0x1FF;
  t3 = t3 << 16;
  t5 = t5 & 0x3FFF;
  t5 = t5 | t4;
  t5 = t5 | t3;
  sw(at + 0x0008, t5);
label8002B9B0:
  v0 += 12; // 0x000C
  temp = v0 != a0;
  at += 12; // 0x000C
  if (temp) goto label8002B84C;
  goto label8002B7D0;
label8002B9C4:
  return;
}

// size: 0x000000E0
void function_8004E2E8(void)
{
  uint32_t temp;
  a1 += 356; // 0x0164
  t1 = spyro_position;
  a3 = lw(t1 + 0x0000);
  at = lw(a0 + 0x0000);
  at -= a3;
  temp = (int32_t)at >= 0;
  t0 = at - a1;
  if (temp) goto label8004E314;
  at = -at;
  t0 = at - a1;
label8004E314:
  temp = (int32_t)t0 > 0;
  if (temp) goto label8004E3C0;
  a3 = lw(t1 + 0x0004);
  v0 = lw(a0 + 0x0004);
  v0 -= a3;
  temp = (int32_t)v0 >= 0;
  t0 = v0 - a1;
  if (temp) goto label8004E33C;
  v0 = -v0;
  t0 = v0 - a1;
label8004E33C:
  temp = (int32_t)t0 > 0;
  if (temp) goto label8004E3C0;
  a3 = lw(t1 + 0x0008);
  v1 = lw(a0 + 0x0008);
  v1 -= a3;
  temp = (int32_t)v1 >= 0;
  t0 = v1 - a1;
  if (temp) goto label8004E364;
  v1 = -v1;
  t0 = v1 - a1;
label8004E364:
  temp = (int32_t)t0 > 0;
  mult(a1, a1);
  if (temp) goto label8004E3C0;
  cop2.IR1 = at;
  cop2.IR2 = v0;
  cop2.IR3 = v1;
  SQR(SF_OFF);
  at = cop2.MAC1;
  v0 = cop2.MAC2;
  v1 = cop2.MAC3;
  at += v0;
  at += v1;
  t0=lo;
  at -= t0;
  temp = (int32_t)at > 0;
  if (temp) goto label8004E3C0;
  at = lw(t1 + 0x002C);
  at = at | a2;
  sw(t1 + 0x002C, at);
  v0 = 1; // 0x0001
  return;
label8004E3C0:
  v0 = 0;
  return;
}

// size: 0x000000F4
void function_8004FDAC(void)
{
  cop2.RGBC = a2;
  cop2.RFC = ((a3 >>  0) & 0xFF) << 4;
  cop2.GFC = ((a3 >>  8) & 0xFF) << 4;
  cop2.BFC = ((a3 >> 16) & 0xFF) << 4;

  vec2 vec1 = vec2_from_32(a0);
  vec2 vec2 = vec2_from_32(a1);

  vec2 = vec2_sub(vec2, vec1);
  
  at = vec1.x;
  v0 = vec1.y;
  v1 = vec2.x;
  a0 = vec2.y;

  int32_t a1 = lw(t2 + 0x0C);
  int32_t a2 = lw(t2 + 0x10);
  int32_t a3 = lw(t2 + 0x14);

  int32_t t0 = vec2.x * a1;
  cop2.MAC1 = vec1.x;
  cop2.MAC2 = vec1.y;
  cop2.IR1 = vec2.x;
  cop2.IR2 = vec2.y;
  t0 = vec2.x * a1 + vec2.y * a2;
  mult(a1, at);
  if (t0) {
    cop2.IR0 = (-(((vec1.x * a1 + vec1.y * a2 + a3)*0x80 + 0x40) / t0))*0x20 + 0x10;
    GPL(SF_ON, LM_OFF);
    t0 = cop2.MAC1;
    v1 = cop2.MAC2;
    DPCS();
    at = (v1 << 16) | (t0 & 0xFFFF);
    v0 = 0x30000000 | cop2.RGB2;
  } else {
    v0 = v0 | a0;
  }
  return;
}

/*
// size: 0x000000F4
void function_8004FDAC(void)
{
  uint32_t temp;
  cop2.RGBC = a2;
  a2 = a3 & 0xFF;
  a2 = a2 << 4;
  a3 = a3 >> 8;
  t0 = a3 & 0xFF;
  t0 = t0 << 4;
  a3 = a3 >> 8;
  a3 = a3 & 0xFF;
  a3 = a3 << 4;
  at = a0 << 16;
  at = (int32_t)at >> 16;
  v0 = (int32_t)a0 >> 16;
  v1 = a1 << 16;
  v1 = (int32_t)v1 >> 16;
  a0 = (int32_t)a1 >> 16;
  a1 = lw(t2 + 0x000C);
  v1 -= at;
  mult(a1, v1);
  a0 -= v0;
  cop2.RFC = a2;
  cop2.GFC = t0;
  cop2.BFC = a3;
  t0=lo;
  a2 = lw(t2 + 0x0010);
  a3 = lw(t2 + 0x0014);
  mult(a2, a0);
  cop2.MAC1 = at;
  cop2.MAC2 = v0;
  cop2.IR1 = v1;
  cop2.IR2 = a0;
  v1=lo;
  t0 += v1;
  temp = t0 == 0;
  mult(a1, at);
  if (temp) goto label8004FE98;
  v1=lo;
  v1 += a3;
  mult(a2, v0);
  a0=lo;
  v1 += a0;
  v1 = v1 << 7;
  v1 += 64; // 0x0040
  div_psx(v1,t0);
  v1=lo;
  v1 = -v1;
  v1 = v1 << 5;
  v1 += 16; // 0x0010
  cop2.IR0 = v1;
  GPL(SF_ON, LM_OFF);
  t0 = cop2.MAC1;
  v1 = cop2.MAC2;
  DPCS();
  at = v1 << 16;
  t0 = t0 & 0xFFFF;
  at = at | t0;
  v0 = 0x30000000;
  a0 = cop2.RGB2;
label8004FE98:
  v0 = v0 | a0;
  return;
}
*/