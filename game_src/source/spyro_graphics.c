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
uint32_t ram_to_vram(RECT *rect, uint32_t *data)
{
  gpu_start_timeout();
  rect->w = clamp_int(rect->w, 0, lh(VRAM_SIZE_X));
  rect->h = clamp_int(rect->h, 0, lh(VRAM_SIZE_Y));
  uint32_t pixels = rect->w*rect->h + 1; // why add 1 ?
  uint32_t num_words = pixels/2;
  uint32_t num_blocks = pixels/32;
  uint32_t remaining_words = num_words - num_blocks*16;

  // while not ready to receive new command
  while (psx_gpustat().cmd_ready == 0)
    if (gpu_check_timeout())
      return -1;
  
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
  return 0;
}

// size: 0x00023C
void function_80061234(void)
{
  BREAKPOINT;
  v0 = ram_to_vram(addr_to_pointer(a0), addr_to_pointer(a1));
}

// size: 0x000284
uint32_t vram_to_ram(RECT *rect, uint32_t *data)
{
  gpu_start_timeout();
  rect->w = clamp_int(rect->w, 0, lh(VRAM_SIZE_X));
  rect->h = clamp_int(rect->h, 0, lh(VRAM_SIZE_Y));
  uint32_t pixels = rect->w*rect->h + 1; // why add 1 ?
  uint32_t num_words = pixels/2;
  uint32_t num_blocks = pixels/32;
  uint32_t remaining_words = num_words - num_blocks*16;

  while (psx_gpustat().cmd_ready == 0)
    if (gpu_check_timeout())
      return -1;

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
  return 0;
}

// size: 0x000284
void function_80061470(void)
{
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
  if ((int32_t)a0 >= 0) {
    t4 = lw(lw(at + 0x0C) + a0*4);
    indices = addr_to_pointer(t4);
  } else {
    t4 = 0;
    len = lw(at + 0x00);
  }

  uint32_t visible_points[0x100];
  int num_visible_points = 0;

  while (1) {
    if ((int32_t)a0 >= 0) {
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
    t4 = t3;

    mat3 m = rtm;

    uint32_t rotY = (t4 >> 16) & 0xFF;
    if (rotY) m = mat3_mul(m, mat3rotY(-rotY*16));

    uint32_t rotX = (t4 >>  8) & 0xFF;
    if (rotX) m = mat3_mul(m, mat3rotX(rotX*16));
    
    uint32_t rotZ = (t4 <<  0) & 0xFF;
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
  t0 = lw(v1 + 0x04);
  t1 = lw(a0 + 0x04);
  t2 = cop2.RGB2;
  at += a3;
  sw(at + 0x00, t2);
  cop2.RGBC = t0;
  t2 = t1 << 4;
  t2 = t2 & 0xFF0;
  cop2.RFC = t2;
  t2 = t1 >> 4;
  t2 = t2 & 0xFF0;
  cop2.GFC = t2;
  t2 = t1 >> 12;
  t2 = t2 & 0xFF0;
  cop2.BFC = t2;
  v1 += 8; // 0x08
  a0 += 8; // 0x08
  DPCS();
  t0 = lw(v1 + 0x00);
  t1 = lw(a0 + 0x00);
  t2 = cop2.RGB2;
  v0 += a3;
  temp = v1 != a1;
  sw(v0 + 0x00, t2);
  if (temp) goto label80026100;
  goto label800259FC;
label800261A0:
  sw(s0 + 0x00, 0);
  sw(t7 + 0x00, 0);
  at = 0x80076DD0;
  t2 = lw(at + 0x00);
  t3 = lw(at + 0x04);
  t4 = lw(at + 0x08);
  t5 = lw(at + 0x0C);
  t6 = lw(at + 0x10);
  cop2.RTM0 = t2;
  cop2.RTM1 = t3;
  cop2.RTM2 = t4;
  cop2.RTM3 = t5;
  cop2.RTM4 = t6;
  at = 0x8007591C;
  at = lw(at + 0x00);
  ra = 0x8006FCF4; // &0x0EA69B
  ra += 8192; // 0x2000
  fp = allocator1_ptr;
  temp = at != 0;
  fp = lw(fp + 0x00);
  if (temp) goto label80026788;
  at = 0x800785A8;
  gp = ordered_linked_list;
  s2 = lw(at + 0x28);
  s3 = lw(at + 0x24);
  gp = lw(gp + 0x00);
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
  s4 -= s7;
  s5 = t8 - s5;
  s6 = t9 - s6;
  t5 = lw(t7 + 0x10);
  t7 += 28; // 0x1C
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