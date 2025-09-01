#pragma once

#include <stdint.h>

#include "functions.h"
#include "decompilation.h"

extern char *ops[];
extern int op_types[];
extern char *registers[];
extern char *cop_regs[4][64];

typedef struct {
  uint32_t special : 6;
  uint32_t r4 : 5;
  uint32_t r3 : 5;
  uint32_t r2 : 5;
  uint32_t r1 : 5;
  uint32_t opcode : 6;
} r_type;

typedef struct {
  uint32_t imm : 16;
  uint32_t r2 : 5;
  uint32_t r1 : 5;
  uint32_t opcode : 6;
} i16_type;

typedef struct {
  uint32_t special : 6;
  uint32_t imm : 20;
  uint32_t opcode : 6;
} i20_type;

typedef struct {
  uint32_t imm : 25;
  uint32_t one : 1;
  uint32_t opcode : 6;
} i25_type;

typedef struct {
  uint32_t imm : 26;
  uint32_t opcode : 6;
} i26_type;

typedef struct {
  uint32_t target : 26;
  uint32_t opcode : 6;
} j_type;

typedef struct {
  union{
    uint32_t inst;
    r_type rtype;
    j_type jtype;
    i16_type i16type;
    i20_type i20type;
    i25_type i25type;
    i26_type i26type;
  };
  uint32_t addr;
  uint32_t opcode;
} instruction;

#define INST_NONE 0
#define INST_IMM11_16 1
#define INST_IMM11_16_MEM_LOAD 13
#define INST_IMM11_16_MEM_STORE 16
#define INST_IMM01_16 2
#define INST_IMM10_16 3
#define INST_IMM_0_20 4
#define INST_IMM_0_25 5
#define INST_IMM_0_26 6
#define INST_R1000 7
#define INST_R0010 8
#define INST_R0111 9
#define INST_R0111_SHIFT 15
#define INST_R1010 10
#define INST_R1100 11
#define INST_R1110 12
#define INST_R1110_SHIFT 14

#define J       2
#define JAL     3
#define BEQ     4
#define BNE     5
#define BLEZ    6
#define BGTZ    7
#define ADDI    8
#define ADDIU   9
#define SLTI    10
#define SLTIU   11
#define ANDI    12
#define ORI     13
#define XORI    14
#define LUI     15
#define COP0    16
#define COP1    17
#define COP2    18
#define COP3    19
#define LB      32
#define LH      33
#define LWL     34
#define LW      35
#define LBU     36
#define LHU     37
#define LWR     38
#define SB      40
#define SH      41
#define SWL     42
#define SW      43
#define SWR     46
#define LWC0    48
#define LWC1    49
#define LWC2    50
#define LWC3    51
#define SWC0    56
#define SWC1    57
#define SWC2    58
#define SWC3    59
#define SLL     64
#define SRL     66
#define SRA     67
#define SLLV    68
#define SRLV    70
#define SRAV    71
#define JR      72
#define JALR    73
#define SYSCALL 76
#define BREAK   77
#define MFHI    80
#define MTHI    81
#define MFLO    82
#define MTLO    83
#define MULT    88
#define MULTU   89
#define DIV     90
#define DIVU    91
#define ADD     96
#define ADDU    97
#define SUB     98
#define SUBU    99
#define AND     100
#define OR      101
#define XOR     102
#define NOR     103
#define SLT     106
#define SLTU    107
#define BLTZ    128
#define BGEZ    129
#define BLTZAL  130
#define BGEZAL  131

// encoding for JR ra, which jumps to the return address.
#define INSTRUCTION_RETURN 0x03E00008

uint32_t get_opcode(instruction inst);
uint32_t validate_opcode(uint32_t opcode);
int validate_instruction(instruction inst);
int output_instruction(instruction inst, instruction inst2, instruction inst3, instruction inst4, struct program prog, function_list *func_list, int double_inst);