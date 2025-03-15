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
  *dst = (*dst & 0xFF000000) | (three_bytes & 0x00FFFFFF);
}

void append_to_linked_list(link_list *link_list, uint32_t *first_node, uint32_t *last_node)
{
  uint32_t *v0 = addr_to_pointer(link_list->end);
  link_list->end = pointer_to_addr(last_node);
  if (v0) {
    write_three_bytes(v0, pointer_to_addr(first_node));
  } else {
    link_list->base = pointer_to_addr(first_node);
  }
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

// size: 0x000000CC
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

// size: 0x000000CC
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

// size: 0x00000044
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

// size: 0x00000058
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

// size: 0x00000084
void function_80060DA4(void)
{
  BREAKPOINT;
  v0 = spyro_set_texture_window_setting_command(addr_to_pointer(a0));
}

// size: 0x00000030
uint32_t gpu_internal_register(uint32_t reg)
{
  sw(lw(GPU_GP1_cmd_ptr), reg | 0x10000000);
  return lw(lw(GPU_GP0_cmd_ptr)) & 0x00FFFFFF;
}

// size: 0x00000030
void function_800617CC(void)
{
  BREAKPOINT;
  v0 = gpu_internal_register(a0);
}

// size: 0x00000028
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

// size: 0x0000004C
void execute_gpu_linked_list(void *node)
{
  sw(lw(GPU_GP1_cmd_ptr), 0x04000002); // DMA cpu to gpu
  sw(lw(DMA_GPU_address_ptr), pointer_to_addr(node));
  sw(lw(DMA_GPU_block_control_ptr), 0);
  sw(lw(DMA_GPU_channel_control_ptr), 0x01000401); // linked list dma start
}

// size: 0x0000004C
void function_80061780(void)
{
  BREAKPOINT;
  execute_gpu_linked_list(addr_to_pointer(a0));
}

// size: 0x000002EC
uint32_t command_queue_advance(void)
{
  if (lw(lw(DMA_GPU_channel_control_ptr)) & 0x01000000)
    return 1;

  a0 = 0;
  ra = 0x80061B30;
  sw(saved_I_STAT3, set_I_MASK(0));
  while (lw(0x80074B6C) != lw(0x80074B68) && (lw(lw(DMA_GPU_channel_control_ptr)) & 0x01000000) == 0) {
    if (((lw(0x80074B6C)+1) & 0x3F) == lw(0x80074B68) && lw(0x80074A70))
      dma_callback2(2, 0);

    while (psx_gpustat().cmd_ready == 0) BREAKPOINT;

    a0 = lw(0x80078EA4 + lw(0x80074B6C)*96);
    a1 = lw(0x80078EA8 + lw(0x80074B6C)*96);
    v0 = lw(0x80078EA0 + lw(0x80074B6C)*96);
    ra = 0x80061C5C;
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
      ra = 0x80061DBC;
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

// size: 0x000002EC
void function_80061B00(void)
{
  v0 = command_queue_advance();
}

// size: 0x00000290
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
  dr_env->tag = (dr_env->tag & 0x00FFFFFF) | (cmd_num << 24);
}

void function_800608E0(void)
{
  BREAKPOINT;
  spyro_clear_screen((DR_ENV*)addr_to_pointer(a0), *(DRAWENV*)addr_to_pointer(a1));
}

// size: 0x00000034
void gpu_start_timeout(void)
{
  sw(gpu_frame_limit, VSync(-1) + 0xF0);
  sw(gpu_retry_counter, 0);
}

// size: 0x00000034
void function_80062090(void)
{
  BREAKPOINT;
  gpu_start_timeout();
}

// size: 0x0000016C
uint32_t gpu_check_timeout(void)
{
  if (VSync(-1) <= lw(gpu_frame_limit)) {
    v1 = lw(gpu_retry_counter);
    sw(gpu_retry_counter, v1 + 1);
    if ((int32_t)v1 <= 0x000F0000) {
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

// size: 0x0000016C
void function_800620C4(void)
{
  BREAKPOINT;
  v0 = gpu_check_timeout();
}

// size: 0x0000023C
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

// size: 0x0000023C
void function_80061234(void)
{
  BREAKPOINT;
  v0 = ram_to_vram(addr_to_pointer(a0), addr_to_pointer(a1));
}

// size: 0x00000284
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

// size: 0x00000284
void function_80061470(void)
{
  v0 = vram_to_ram(addr_to_pointer(a0), addr_to_pointer(a1));
}

// size: 0x0000025C
uint32_t fill_color(RECT *rect, uint32_t color)
{
  rect->w = clamp_int(rect->w, 0, lh(VRAM_SIZE_X)-1);
  rect->h = clamp_int(rect->h, 0, lh(VRAM_SIZE_Y)-1);

  if (rect->x & 0x3F || rect->w & 0x3F) {
    sw(0x80075978, 0x08000000 | (0x8007599C & 0x00FFFFFF));
    sw(0x8007597C, 0xE3000000);
    sw(0x80075980, 0xE4FFFFFF);
    sw(0x80075984, 0xE5000000);
    sw(0x80075988, 0xE6000000);
    sw(0x8007598C, 0xE1000000 | ((color >> 31) << 10) | (lw(lw(GPU_GP1_cmd_ptr)) & 0x7FF));
    sw(0x80075990, 0x60000000 | (color & 0x00FFFFFF));
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
    sw(0x80075984, 0x02000000 | (color & 0x00FFFFFF));
    sw(0x80075988, (rect->y << 16) | rect->x);
    sw(0x8007598C, (rect->h << 16) | rect->w);
  }
  execute_gpu_linked_list(addr_to_pointer(0x80075978));

  return 0;
}

// size: 0x0000025C
void function_80060FD8(void)
{
  BREAKPOINT;
  v0 = fill_color(addr_to_pointer(a0), a1);
}

// size: 0x000002E0
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

// size: 0x00000458
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

        last_node[0] = (t9 << 24) | (pointer_to_addr(node) & 0x00FFFFFF);
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

// size: 0x000000B0
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

// size: 0x000000B8
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

// size: 0x000000B8
void function_8005EA94(void)
{
  v0 = pointer_to_addr(SetDefDrawEnv(addr_to_pointer(a0), a1, a2, a3, lw(sp + 0x10)));
}

// size: 0x0000003C
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

// size: 0x00000120
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

// size: 0x00000098
void function_8001256C(void)
{
  const static uint32_t array1[] = { // 0x8006F200
    0x0000EA60,
    0x00014A68,
    0x0001BB28,
    0x00023EF0,
    0x0002D1E8,
    0x00037450,
  };

  const static uint32_t array2[] = { // 0x8006EEDC
    0x000042F8,
    0x000049BB,
    0x00004AF4,
    0x00005533,
    0x00005710,
    0x000058BC,
    0x00005881,
    0x00005B0F,
    0x00006073,
    0x000065EF,
    0x000066C6,
    0x00006B88,
    0x00006C88,
    0x00006D99,
    0x0000702D,
    0x00007049,
    0x00007003,
    0x000071D9,
    0x0000736D,
    0x00007778,
    0x00007D0E,
    0x00007D82,
    0x00007FD5,
    0x00008399,
    0x000085AB,
    0x000088DA,
    0x00008A74,
    0x00008BFB,
    0x00008B95,
    0x00008F26,
    0x000091F8,
    0x00009249,
    0x00009416,
    0x000095A6,
    0x0000A21A,
    0x0000A227,
    0x00009049,
    0x00007CD8,
    0x00006FCB,
    0x00008188,
    0x00007754,
    0x0000577B,
    0x00008FE4,
    0x00007CD2,
    0x0000700F,
    0x0000818B,
    0x0000768A,
    0x0000583F,
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

// size: 0x00000080
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

// size: 0x00000080
void function_80062350(void)
{
  InitGeom();
}

// size: 0x00000020
void function_800625F8(void)
{
  cop2.TRX = lw(a0 + 0x14);
  cop2.TRY = lw(a0 + 0x18);
  cop2.TRZ = lw(a0 + 0x1C);
}

// size: 0x00000018
void SetGeomOffset(uint32_t ofx, uint32_t ofy) 
{
  cop2.OFX = ofx << 16;
  cop2.OFY = ofy << 16;
}

void function_80062618(void)
{
  SetGeomOffset(a0, a1);
}

// size: 0x0000000C
void SetGeomScreen(uint32_t h)
{
  cop2.H = h;
}

void function_80062638(void)
{
  SetGeomScreen(a0);
}

// size: 0x00000060
void init_wad(void)
{
  sw(WAD_sector, 0x25);
  read_disk1(lw(WAD_sector), lw(exe_end_ptr), 0x800, 0, 0x258);
  spyro_memcpy32(WAD_header, lw(exe_end_ptr), 0x330);
}

// size: 0x00000060
void function_8001250C(void)
{
  init_wad();
}

// size: 0x0000011C
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

// size: 0x00000530
void initial_loading_screen(void)
{
  sp -= 72;
  sw(sp + 0x0040, ra);
  sw(sp + 0x003C, s5);
  sw(sp + 0x0034, s3);
  sw(sp + 0x0030, s2);
  sw(sp + 0x002C, s1);
  sw(sp + 0x0028, s0);

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
  s5 = 0x801C0000 - lw(0x800755A4);
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
    ra = 0x80012B30;
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
  ra = 0x80012C8C;
  function_8005B7D8();
  ra = 0x80012C94;
  function_8002D338();
  sw(0x80075918, 15);
  ra = 0x80012CA8;
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

// size: 0x00000530
void function_800127C0(void)
{
  initial_loading_screen();
}