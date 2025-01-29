#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>

#include "opcodes.h"
#include "functions.h"
#include "decompilation.h"

char *used_skips = NULL;

struct function_name function_names[] = {
  {0x80016500,"read_disk1(a0, a1, a2, a3, lw(sp+0x10))","void read_disk1(uint32_t sector, uint32_t dst, uint32_t len, uint32_t offset, uint32_t num)"},
  {0x80016698,"read_disk2(a0, a1, a2, a3, lw(sp+0x10))","void read_disk2(uint32_t sector, uint32_t dst, uint32_t len, uint32_t offset, uint32_t num)"},
  {0x80016914,"spyro_memset32(a0, a1, a2)","void spyro_memset32(uint32_t dst, uint32_t set, uint32_t len)"},
  {0x80016930,"assert((a2%16) == 0);spyro_memset32(a0, a1, a2)"},
  {0x80016958,"spyro_memcpy32(a0, a1, a2)","void spyro_memcpy32(uint32_t dst, uint32_t src, uint32_t len)"},
  {0x8005DB14,"InitHeap(a0, a1)"},
  {0x8005DB24,"LoadExec((char *)addr_to_pointer(a0), a1, a2)"},
  {0x8005DB34,"_96_init()"},
  {0x8005DB4C,"_96_remove()"},
  {0x8005DB64,"DeliverEvent(a0, a1)"},
  {0x8005DB74,"v0 = OpenEvent(a0, a1, a2, a3)"},
  {0x8005DB84,"v0 = TestEvent(a0)"},
  {0x8005DB94,"v0 = EnableEvent(a0)"},
  {0x8005DBA4,"v0 = EnterCriticalSection()"},
  {0x8005DBB4,"ExitCriticalSection()"},
  {0x8005DDA8,"v0 = ChangeClearPAD(a0)"},
  {0x8005DDB8,"v0 = ChangeClearRCnt(a0, a1)"},
  {0x8005E4AC,"spyro_memclr32(a0, a1)", "void spyro_memclr32(uint32_t dst, uint32_t len)"},
  {0x8005E4D8,"ReturnFromException()"},
  {0x8005E4E8,"ResetEntryInt(a0)"},
  {0x8005E4F8,"HookEntryInt(a0)"},
  {0x8005E604,"spyro_memclr32(a0, a1)"},
  {0x8005E8AC,"spyro_memclr32(a0, a1)"},
  {0x8005F2A4,"v0 = ResetGraph(a0)","uint32_t ResetGraph(uint32_t mode)"},
  {0x8005F53C,"v0 = SetGraphDebug(a0)","uint32_t SetGraphDebug(uint32_t level)"},
  {0x8005F6C8,"SetDispMask(a0)","void SetDispMask(uint32_t mask)"},
  {0x8005F764,"v0 = DrawSync(a0)","uint32_t DrawSync(uint32_t mode)"},
  {0x8005F8F8,"v0 = ClearImage(a0, a1, a2, a3)","uint32_t ClearImage(uint32_t box_ptr, uint32_t r, uint32_t g, uint32_t b)"},
  {0x8005FA28,"v0 = LoadImage(a0, a1)","uint32_t LoadImage(uint32_t box_ptr, uint32_t img_ptr)"},
  {0x8005FA8C,"v0 = StoreImage(a0, a1)","uint32_t StoreImage(uint32_t box_ptr, uint32_t img_ptr)"},
  {0x8005FAF0,"v0 = MoveImage(a0, a1, a2)","uint32_t MoveImage(uint32_t box_ptr, uint32_t x, uint32_t y)"},
  {0x8005FD64,"DrawOTag(a0)","void DrawOTag(uint32_t of)"},
  {0x8005FDD8,"v0 = PutDrawEnv(a0)","uint32_t PutDrawEnv(uint32_t env_ptr)"},
  {0x80060030,"v0 = PutDispEnv(a0)","uint32_t PutDispEnv(uint32_t env_ptr)"},
  {0x80060B70,"v0 = spyro_draw_mode_setting_command(a0, a1, a2)","uint32_t spyro_draw_mode_setting_command(uint32_t allow_drawing_to_display, uint32_t dithering_enabled, uint32_t tex_page)"},
  {0x80060BC8,"v0 = spyro_set_drawing_area_top_left_command(a0, a1)","uint32_t spyro_set_drawing_area_top_left_command(int16_t x, int16_t y)"},
  {0x80060C94,"v0 = spyro_set_drawing_area_bottom_right_command(a0, a1)","uint32_t spyro_set_drawing_area_bottom_right_command(int16_t x, int16_t y)"},
  {0x80060D60,"v0 = spyro_set_drawing_offset_command(a0, a1)","uint32_t spyro_set_drawing_offset_command(uint32_t x, uint32_t y)"},
  {0x80060DA4,"v0 = spyro_set_texture_window_setting_command(a0)","uint32_t spyro_set_texture_window_setting_command(uint32_t texture_window)"},
  {0x800608E0,"spyro_clear_screen(a0, a1)","void spyro_clear_screen(uint32_t cmd_ptr, uint32_t screen_info)"},
  {0x8006230C,"spyro_memset8(a0, a1, a2)","void spyro_memset8(uint32_t dst, uint8_t set, uint32_t len)"},
  {0x80062338,"GPU_cw(a0)"},
  {0x800626E8,"FlushCache()"},
  {0x8006276C,"v0 = spyro_strlen(a0)","uint32_t spyro_strlen(uint32_t str)"},
  {0x8006279C,"spyro_printf(a0, a1, a2, a3)","void spyro_printf(uint32_t fmt, uint32_t param1, uint32_t param2, uint32_t param3)"},
  {0x80062EC0,"v0 = spyro_strchr(a0, a1, a2)","uint32_t spyro_strchr(uint32_t str, uint32_t chr, uint32_t len)"},
  {0x80062F10,"spyro_putchar(a0)","void spyro_putchar(uint32_t chr)"},
  {0x80062FD4,"spyro_sprintf()","void spyro_sprintf(void)"},
  {0x80062FC4,"v0 = psx_write(a0, addr_to_pointer(a1), a2)","uint32_t psx_write(int fd, char *str, uint32_t len)"},
  {0x8006389C,"spyro_puts(a0)","void spyro_puts(uint32_t str)"},
  {0x8006396C,"psx_exit(a0)"},
  {0x80068494,"_bu_init()"},
  {0x800684A4,"v0 = CloseEvent(a0)"},
  {0x800684B4,"v0 = psx_open(addr_to_pointer(a0), a1)"},
  {0x800684C4,"v0 = psx_lseek(a0, a1, a2)"},
  {0x800684D4,"v0 = psx_read(a0, addr_to_pointer(a1), a2)"},
  {0x800684E4,"v0 = psx_close(a0)"},
  {0x800684F4,"v0 = format(a0)"},
  {0x800688E0,"v0 = _card_info(a0)"},
  {0x800688F0,"v0 = _card_load(a0)"},
  {0x80068900,"_card_write(a0, a1, a2)"},
  {0x80068910,"_new_card()"},
  {0x80068D50,"SysEnqIntRP(a0, a1)"},
  {0x80068D60,"SysDeqIntRP(a0, a1)"},
  {0x80068E80,"InitCARD2(a0)"},
  {0x80068E90,"StartCARD2()"},
  {0x8006B670,"v0 = spyro_memclr8(a0, a1)","uint32_t spyro_memclr8(uint32_t dst, uint32_t len)"},
};

struct address_name address_names[] = {
  {0x1F801120, "TIMER2"},
  {0x1F801124, "TIMER2_MODE"},
  {0x1F801128, "TIMER2_TARGET"},
  {0x8006CBF8, "spyro_sin_lut"},
  {0x8006CC78, "spyro_cos_lut"},
  {0x800730E8, "spu_event"},
  {0x800738BC, "GPUSTAT_ptr"},
  {0x800738C0, "TIMER_1_horizontal_retrace_ptr"},
  {0x800749B0, "I_STAT_ptr"},
  {0x800749B4, "I_MASK_ptr"},
  {0x800749B8, "DMA_control_register_copy_1_ptr"},
  {0x800749E0, "frame_counter"},
  {0x800749E4, "TIMER_1_horizontal_retrace_counter_mode_ptr"},
  {0x800749E8, "DMA_interrupt_register_ptr"},
  {0x80074A66, "psy_debug_level_ptr"},
  {0x80074A68, "VRAM_SIZE_X"},
  {0x80074A6A, "VRAM_SIZE_Y"},
  {0x80074B34, "GPU_GP0_cmd_ptr"},
  {0x80074B38, "GPU_GP1_cmd_ptr"},
  {0x80074B3C, "DMA_GPU_address_ptr"},
  {0x80074B40, "DMA_GPU_block_control_ptr"},
  {0x80074B44, "DMA_GPU_channel_control_ptr"},
  {0x80074B48, "DMA_OTC_address_ptr"},
  {0x80074B4C, "DMA_OTC_block_control_ptr"},
  {0x80074B54, "DMA_control_register_copy_2_ptr"},
  {0x800750FC, "CDROM_INDEX_ptr"},
  {0x80075100, "CDROM_REG_1_ptr"},
  {0x80075104, "CDROM_REG_2_ptr"},
  {0x80075108, "CDROM_REG_3_ptr"},
  {0x8007510C, "MEM_COMMON_DELAY_ptr"},
  {0x80075110, "SPU_voice_base_ptr"},
  {0x80075130, "MEM_CDROM_DELAY_ptr"},
  {0x80075134, "DMA_control_register_copy_3_ptr"},
  {0x80075138, "DMA_CDROM_address_ptr"},
  {0x8007513C, "DMA_CDROM_block_control_ptr"},
  {0x80075140, "DMA_CDROM_channel_control_ptr"},
  {0x8007521C, "I_STAT2_ptr"},
  {0x80075220, "JOY_BASE_ptr"},
  {0x80075254, "JOY_BASE2_ptr"},
  {0x80075238, "JOY_BASE3_ptr"},
  {0x8007572C, "level_frame_counter"},
  {0x80075750, "total_found_dragons"},
  {0x80075810, "total_found_eggs"},
  {0x80075860, "total_found_gems"},
  {0x80075AC0, "rng_seed"},
  {0x80075B0C, "memcard_events"},
  {0x80076B78, "timeout1_start"},
  {0x80077378, "buttons_press"},
  {0x8007737C, "buttons_unpress"},
  {0x80077380, "buttons"},
  {0x80078A58, "spyro_position_x"},
  {0x80078A5C, "spyro_position_y"},
  {0x80078A60, "spyro_position_z"},
  {0x80078E9C, "timeout1_duration"},
};

uint32_t jump_list_get_jump_list(struct program prog, struct jump_list jump_list, uint32_t list[static jump_list.num_jumps])
{
  switch (jump_list.type) {
  case TYPE_CUSTOM_LIST:
    for (int i = 0; i < jump_list.num_jumps; i++)
      list[i] = jump_list.list[i];
    return jump_list.num_jumps;
  case TYPE_PSXMEM_LIST: {}
    uint32_t j = 0;
    for (int i = 0; i < jump_list.num_jumps; i++)
    {
      uint32_t addr = jump_list.psx_addr+i*4;
      assert(addr >= 0x80000000 && addr < 0x80200000);
      uint32_t offset = addr - 0x80000000;
      uint32_t value = *(uint32_t *)(prog.psx_mem + offset);

      uint32_t is_duplicate = 0;

      // remove duplicates
      for (int k = 0; k < j; k++)
      {
        if (list[k] == value)
        {
          is_duplicate = 1;
          break;
        }
      }

      if (is_duplicate) continue;

      list[j] = value;
      j++;
    }
    return j;
  default:
    assert(0);
  }

  return -1;
}

int addr_in_range(struct program prog, uint32_t addr)
{
  return addr >= prog.range.base && addr < prog.range.base + prog.range.size;
}

void print_func_name(struct program prog, uint32_t addr)
{
  for (int i = 0; i < sizeof(function_names)/sizeof(function_names[0]); i++)
  {
    struct function_name rn = function_names[i];

    int same_id = 0;
    if (prog.id == NULL)
      same_id = rn.id == NULL;
    else
      same_id = (rn.id == NULL) || (strcmp(rn.id, prog.id) == 0);

    if (addr == rn.func && same_id)
    {
      fprintf(prog.output, "%s", rn.func_call);
      return;
    }
  }

  fprintf(prog.output, "function_%.8X", addr);
  if (prog.id && addr_in_range(prog, addr))
    fprintf(prog.output, "_%s()", prog.id);
  else
    fprintf(prog.output, "()");
}

char *get_address_name(struct program prog, uint32_t addr)
{
  for (int i = 0; i < sizeof(address_names)/sizeof(address_names[0]); i++)
  {
    struct address_name an = address_names[i];

    int same_id = 0;
    if (prog.id == NULL)
      same_id = an.id == NULL;
    else
      same_id = (an.id == NULL) || (strcmp(an.id, prog.id) == 0);

    if (addr == an.addr && same_id)
      return an.name;
  }

  static char addr_hex[16];
  snprintf(addr_hex, sizeof(addr_hex), "0x%.8X", addr);
  return addr_hex;
}

#define FUNC_NULL_DECLARATION 0
#define FUNC_HAS_NAME 1
#define FUNC_NOT_RENAMED 2

uint32_t print_func_declaration(FILE *file, struct program prog, uint32_t addr)
{
  for (int i = 0; i < sizeof(function_names)/sizeof(function_names[0]); i++)
  {
    struct function_name rn = function_names[i];

    int same_id = 0;
    if (prog.id == NULL)
      same_id = rn.id == NULL;
    else
      same_id = (rn.id == NULL) || (strcmp(rn.id, prog.id) == 0);

    if (addr == rn.func && same_id)
    {
      if (rn.declaration) {
        fprintf(file, "%s", rn.declaration);
        return FUNC_HAS_NAME;
      } else {
        return FUNC_NULL_DECLARATION;
      }
    }
  }

  fprintf(file, "void function_%.8X", addr);
  if (prog.id && addr_in_range(prog, addr))
    fprintf(file, "_%s(void)", prog.id);
  else
    fprintf(file, "(void)");
  return FUNC_NOT_RENAMED;
}

instruction load_instruction(char *psx_mem, uint32_t addr)
{
  assert(addr >= 0x80000000 && addr < 0x80200000);
  uint32_t offset = addr - 0x80000000;

  instruction inst = {};
  inst.addr = addr;
  inst.inst = *(uint32_t *)(psx_mem + offset);
  inst.opcode = get_opcode(inst);
  return inst;
}

int is_skipped(struct program prog, uint32_t addr)
{
  for (int i = 0; i < prog.skips.n; i++)
    if (prog.skips.funcs[i] == addr)
    {
      used_skips[i] = 1;
      return 1;
    }
  return 0;
}

int read_function_instructions(struct program prog, function_list *func_list, uint32_t addr, int depth_limit, int called, int single_function)
{
  //printf("addr: %.8X\n", addr);

  function func = function_alloc();

  if (called) func.function_info = FUNCTION_CALLED;

  func.address = addr;

  int i;
  for (i = 0; i < 1000000; i++)
  {
    instruction inst = load_instruction(prog.psx_mem, addr + i*4);

    if (validate_instruction(inst)) break;

    if (inst.opcode == JAL)
    {
      uint32_t func_addr = inst.i26type.imm<<2;
      func_addr |= inst.addr & 0xF0000000;

      if (!is_skipped(prog, func_addr))
        function_add_call(&func, func_addr);
    }

    if (inst.opcode == J)
    {
      uint32_t func_addr = (inst.i26type.imm<<2) | (inst.addr & 0xF0000000);

      //if (func_addr >= func.address)
      function_add_jump(&func, func_addr);
    }

    if (inst.opcode == BLTZ || inst.opcode == BGEZ || inst.opcode == BLTZAL || inst.opcode == BGEZAL
     || inst.opcode == BEQ  || inst.opcode == BNE  || inst.opcode == BLEZ   || inst.opcode == BGTZ)
    {
      uint32_t jmp = inst.addr+(int16_t)(inst.i16type.imm+1)*4;

      //if (jmp >= func.address)
      function_add_jump(&func, jmp);
    }

    if (inst.opcode == JALR)
    {
      for (int i = 0; i < prog.jumpss.n; i++)
      {
        struct jump_list jump = prog.jumpss.jumps[i];
        for (int k = 0; k < jump.num_addrs; k++)
        {
          if (jump.addrs[k] == inst.addr)
          {
            uint32_t jump_list[jump.num_jumps];
            uint32_t num_jumps = jump_list_get_jump_list(prog, jump, jump_list);
            for (int j = 0; j < num_jumps; j++)
            {
              uint32_t addr = jump_list[j];
              if (!is_skipped(prog, addr))
                function_add_call(&func, addr);
            }
            goto end_loop4;
          }
        }
      }
    }
end_loop4:

    if (inst.opcode == JR)
    {
      for (int i = 0; i < prog.jumpss.n; i++)
      {
        struct jump_list jump = prog.jumpss.jumps[i];
        for (int k = 0; k < jump.num_addrs; k++)
        {
          if (jump.addrs[k] == inst.addr)
          {
            uint32_t jump_list[jump.num_jumps];
            uint32_t num_jumps = jump_list_get_jump_list(prog, jump, jump_list);
            for (int j = 0; j < num_jumps; j++)
            {
              uint32_t addr = jump_list[j];
              //if (addr >= func.address)
              function_add_jump(&func, addr);
            }
            goto end_loop3;
          }
        }
      }
    }
end_loop3:

    if (inst.opcode == J || inst.opcode == JR || inst.opcode == BREAK) break;
  }

  func.end = addr+i*4;
  func.size = i+2;

  function_list_insert(func_list, func);

  if (depth_limit > 0)
  {
    for (int j = 0; j < func.jumps.size; j++)
    {
      uint32_t jump = func.jumps.addrs[j];
      if (!function_list_contains_address(func_list, jump)) {
        //printf("jump: ");
        read_function_instructions(prog, func_list, jump, depth_limit-1, 0, single_function);
      }
    }

    if (!single_function)
      for (int j = 0; j < func.calls.size; j++)
      {
        uint32_t jump = func.calls.addrs[j];
        if (!function_list_contains_address(func_list, jump)) {
          //printf("function: ");
          read_function_instructions(prog, func_list, jump, depth_limit-1, 1, single_function);
        }
      }
  }

  return 0;
}

void output_function(struct program prog, function_list *func_list, uint32_t func_num)
{
  function func = func_list->funcs[func_num];

  if (!(func.function_info & FUNCTION_CALLED)) return;

  function_list func_set = function_list_alloc();
  addr_list labels = addr_list_alloc();
  function_list_insert(&func_set, func);

  for (int i = 0; i < func_set.size; i++)
  {
    function func = func_set.funcs[i];

    for (int j = 0; j < func.jumps.size; j++)
    {
      uint32_t jump = func.jumps.addrs[j];
      addr_list_insert(&labels, jump);
      int func_num = function_list_contains_address(func_list, jump);
      if (func_num)
        if (!function_list_contains_address(&func_set, jump))
          function_list_insert(&func_set, func_list->funcs[func_num-1]);
    }
  }
  
  addr_list_sort(&labels);
  function_list_sort(&func_set);

  uint32_t func_size = func_set.funcs[func_set.size-1].address + func_set.funcs[func_set.size-1].size*4 - func.address;

  fprintf(prog.output, "// size: 0x%.8X\n", func_size);
  int no_declaration = print_func_declaration(prog.output, prog, func.address);
  if (no_declaration != FUNC_NOT_RENAMED)
    printf("Error function 0x%.8X renamed but not skipped\n", func.address);
  fprintf(prog.output, "\n{\n");
  fprintf(prog.output, "  uint32_t temp, return_address = ra;\n");

  assert(func_set.funcs[0].address == func.address);

  uint32_t label_num = 0;
  
  for (int i = 0; i < func_set.size; i++)
  {
    function func = func_set.funcs[i];
    uint32_t addr = func.address;

    for (uint32_t i = 0; i < func.size-1; i++)
    {

      instruction inst1 = load_instruction(prog.psx_mem, addr + i*4 + 0);
      instruction inst2 = load_instruction(prog.psx_mem, addr + i*4 + 4);
      instruction inst3 = load_instruction(prog.psx_mem, addr + i*4 + 8);
      instruction inst4 = load_instruction(prog.psx_mem, addr + i*4 + 12);

      int label_next = func.size-i;
      if (label_num < labels.size)
      {
        label_next = (labels.addrs[label_num] - inst1.addr) / 4;
        if (label_next < 0)
        {
          printf("inst.addr %.8X %.8X %d\n", inst1.addr, labels.addrs[label_num], labels.size);
          assert(0);
        }
        else if (label_next == 0)
        {
          fprintf(prog.output, "label%.8X:\n", labels.addrs[label_num]);
          // spyro specific instruction
          if (labels.addrs[label_num] == 0x8001222C)
            fprintf(prog.output, "  start_frame();\n");
          label_num++;
          if (label_num < labels.size)
            label_next = (labels.addrs[label_num] - inst1.addr) / 4;
          else 
            label_next = func.size-i;
        }
      }

      i += output_instruction(inst1, inst2, inst3, inst4, prog, func_list, label_next);

      if (inst1.opcode == JALR)
      {
        fprintf(prog.output, "  switch (temp)\n  {\n");
        for (int i = 0; i < prog.jumpss.n; i++)
        {
          struct jump_list jump = prog.jumpss.jumps[i];
          for (int k = 0; k < jump.num_addrs; k++)
          {
            if (jump.addrs[k] == inst1.addr)
            {
              uint32_t jump_list[jump.num_jumps];
              uint32_t num_jumps = jump_list_get_jump_list(prog, jump, jump_list);
              for (int j = 0; j < num_jumps; j++)
              {
                uint32_t addr = jump_list[j];
                fprintf(prog.output, "  case 0x%.8X:\n    ", addr);
                print_func_name(prog, addr);
                fprintf(prog.output, ";\n    break;\n");
              }
              goto end_loop2;
            }
          }
        }
end_loop2:
        fprintf(prog.output, "  default:\n    JALR(temp, 0x%.8X);\n  }\n", inst1.addr);
      }
      else if (inst1.opcode == JR)
      {
        fprintf(prog.output, "  if (temp == return_address) return;\n");
        fprintf(prog.output, "  switch (temp)\n  {\n");
        for (int i = 0; i < prog.jumpss.n; i++)
        {
          struct jump_list jump = prog.jumpss.jumps[i];
          for (int k = 0; k < jump.num_addrs; k++)
          {
            if (jump.addrs[k] == inst1.addr)
            {
              uint32_t jump_list[jump.num_jumps];
              uint32_t num_jumps = jump_list_get_jump_list(prog, jump, jump_list);
              for (int j = 0; j < num_jumps; j++)
              {
                uint32_t addr = jump_list[j];
                fprintf(prog.output, "  case 0x%.8X:", addr);
                if (jump.type == TYPE_PSXMEM_LIST)
                {
                  fprintf(prog.output, " //");
                  for (int i = 0; i < jump.num_jumps; i++)
                  {
                    uint32_t addr2 = jump.psx_addr+i*4;
                    assert(addr >= 0x80000000 && addr2 < 0x80200000);
                    uint32_t offset = addr2 - 0x80000000;
                    uint32_t value = *(uint32_t *)(prog.psx_mem + offset);
                    if (value == addr)
                      fprintf(prog.output, " %d", i);
                  }
                }
                fprintf(prog.output, "\n    goto label%.8X;\n    break;\n", addr);
              }
              goto end_loop1;
            }
          }
        }
end_loop1:
        fprintf(prog.output, "  default:\n    JR(temp, 0x%.8X);\n    return;\n  }\n", inst1.addr);
      }
    }
  }

  fprintf(prog.output, "}\n");
  fprintf(prog.output, "\n");
}

void output_function_list(struct program prog, function_list *func_list)
{

  int i = 0;
  int j = 0;

  while (i < prog.skips.n && j < func_list->size)
  {
    uint32_t addr1 = prog.skips.funcs[i];
    uint32_t addr2 = func_list->funcs[j].address;

    if (addr1 < addr2)
    {
      if (addr_in_range(prog, prog.skips.funcs[i]))
        if (print_func_declaration(prog.header, prog, prog.skips.funcs[i]))
          fprintf(prog.header, ";\n");
      i++;
    } else {
      if (addr_in_range(prog, func_list->funcs[j].address))
        if (func_list->funcs[j].function_info & FUNCTION_CALLED)
          if (print_func_declaration(prog.header, prog, func_list->funcs[j].address))
            fprintf(prog.header, ";\n");
      j++;
    }
  }

  for (; i < prog.skips.n; i++)
  {
    if (addr_in_range(prog, prog.skips.funcs[i]))
      if (print_func_declaration(prog.header, prog, prog.skips.funcs[i]))
        fprintf(prog.header, ";\n");
  }

  for (; j < func_list->size; j++) {
    if (addr_in_range(prog, func_list->funcs[j].address))
      if (func_list->funcs[j].function_info & FUNCTION_CALLED)
      {
        if (print_func_declaration(prog.header, prog, func_list->funcs[j].address))
          fprintf(prog.header, ";\n");
      }
  }

  for (int i = 0; i < func_list->size; i++)
  {
    uint32_t addr = func_list->funcs[i].address;
    if (addr_in_range(prog, addr))
      output_function(prog, func_list, i);
    else
      if (prog.external_calls)
        if (!addr_list_contains(*prog.external_calls, addr))
          addr_list_insert(prog.external_calls, addr);
  }
}

void print_function_name(struct program prog, uint32_t addr)
{
  for (int i = 0; i < sizeof(function_names)/sizeof(function_names[0]); i++)
  {
    struct function_name rn = function_names[i];

    if (addr == rn.func)
    {
      int i = 0;
      int j = 0;
      while (1) {
        if (rn.func_call[i] == ' ') j = i+1;
        if (rn.func_call[i] == 0 || rn.func_call[i] == '(') break;
        i++;
      }
      printf("%.*s", i-j, rn.func_call+j);
      return;
    }
  }

  printf("function_%.8X", addr);
}

void output_function_list_graphviz(struct program prog, function_list *func_list)
{
  for (int i = 0; i < func_list->size; i++)
  {
    function func = func_list->funcs[i];
    if (func.calls.size > 0)
    {
      printf("\t");
      print_function_name(prog, func.address);
      printf(" -> {");
      for (int j = 0; j < func.calls.size-1; j++)
      {
        print_function_name(prog, func.calls.addrs[j]);
        printf("; ");
      }
      print_function_name(prog, func.calls.addrs[func.calls.size-1]);
      printf("};\n");
    }
  }
}

int read_instructions(struct program prog)
{
  if (prog.output == NULL)
    prog.output = stdout;

  used_skips = calloc(prog.skips.n, sizeof(char));

  function_list func_list = function_list_alloc();

  for (int i = 0; i < prog.entries.n; i++)
  {
    if (is_skipped(prog, prog.entries.entries[i]))
      printf("function_%.8X is both skipped and entry\n", prog.entries.entries[i]);

    for (int j = 0; j < prog.entries2n; j++)
      if (prog.entries.entries[i] == prog.entries2[j])
        printf("function_%.8X is already included\n", prog.entries.entries[i]);

    if (read_function_instructions(prog, &func_list, prog.entries.entries[i], 10000, 1, 0))
      return 1;
  }

  for (int i = 0; i < prog.entries2n; i++)
    if (!is_skipped(prog, prog.entries2[i]))
      if (read_function_instructions(prog, &func_list, prog.entries2[i], 10000, 1, 0))
        return 1;

  function_list_sort(&func_list);

  uint32_t unused_skips = 0;
  for (int i = 0; i < prog.skips.n; i++)
    if (!used_skips[i])
    {
      printf("unused skip  0x%.8X,\n", prog.skips.funcs[i]);
      unused_skips = 1;
    }
  

  if (unused_skips) {
    printf("used skips for %s:\n", prog.id);
    for (int i = 0; i < prog.skips.n; i++)
      if (used_skips[i])
        printf("  0x%.8X,\n", prog.skips.funcs[i]);
  }

  //output_function_list_graphviz(prog, &func_list);

  fprintf(prog.output, "#include <stdint.h>\n\n");
  fprintf(prog.output, "#include \"decompilation.h\"\n");
  fprintf(prog.output, "#include \"psx_ops.h\"\n");
  fprintf(prog.output, "#include \"psx_bios.h\"\n");
  fprintf(prog.output, "#include \"psx_mem.h\"\n");
  fprintf(prog.output, "#include \"main.h\"\n");
  fprintf(prog.output, "#include \"gte.h\"\n");
  fprintf(prog.output, "#include \"cop0.h\"\n\n");

  if (prog.id)
    fprintf(prog.output, "#include \"%s.h\"\n\n", prog.id);
  else
    fprintf(prog.output, "#include \"function_chooser.h\"\n\n");

  fprintf(prog.header, "#pragma once\n\n");
  fprintf(prog.header, "#include <stdint.h>\n\n");

  output_function_list(prog, &func_list);

  fprintf(prog.header, "\n");

  for (int i = 0; i < sizeof(address_names)/sizeof(address_names[0]); i++)
  {
    struct address_name an = address_names[i];

    int same_id = 0;
    if (prog.id == NULL)
      same_id = an.id == NULL;
    else
      same_id = (an.id != NULL) && (strcmp(an.id, prog.id) == 0);

    if (same_id)
      fprintf(prog.header, "#define %s 0x%.8X\n", an.name, an.addr);
  }

  function_list_free(func_list);

  return 0;
}
