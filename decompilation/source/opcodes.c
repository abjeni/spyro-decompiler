#include "opcodes.h"
#include "decompilation.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>



/*
00h=SPECIAL 08h=ADDI  10h=COP0 18h=N/A   20h=LB   28h=SB   30h=LWC0 38h=SWC0
01h=BcondZ  09h=ADDIU 11h=COP1 19h=N/A   21h=LH   29h=SH   31h=LWC1 39h=SWC1
02h=J       0Ah=SLTI  12h=COP2 1Ah=N/A   22h=LWL  2Ah=SWL  32h=LWC2 3Ah=SWC2
03h=JAL     0Bh=SLTIU 13h=COP3 1Bh=N/A   23h=LW   2Bh=SW   33h=LWC3 3Bh=SWC3
04h=BEQ     0Ch=ANDI  14h=N/A  1Ch=N/A   24h=LBU  2Ch=N/A  34h=N/A  3Ch=N/A
05h=BNE     0Dh=ORI   15h=N/A  1Dh=N/A   25h=LHU  2Dh=N/A  35h=N/A  3Dh=N/A
06h=BLEZ    0Eh=XORI  16h=N/A  1Eh=N/A   26h=LWR  2Eh=SWR  36h=N/A  3Eh=N/A
07h=BGTZ    0Fh=LUI   17h=N/A  1Fh=N/A   27h=N/A  2Fh=N/A  37h=N/A  3Fh=N/A
*/

/*
00h=SLL   08h=JR      10h=MFHI 18h=MULT  20h=ADD  28h=N/A  30h=N/A  38h=N/A
01h=N/A   09h=JALR    11h=MTHI 19h=MULTU 21h=ADDU 29h=N/A  31h=N/A  39h=N/A
02h=SRL   0Ah=N/A     12h=MFLO 1Ah=DIV   22h=SUB  2Ah=SLT  32h=N/A  3Ah=N/A
03h=SRA   0Bh=N/A     13h=MTLO 1Bh=DIVU  23h=SUBU 2Bh=SLTU 33h=N/A  3Bh=N/A
04h=SLLV  0Ch=SYSCALL 14h=N/A  1Ch=N/A   24h=AND  2Ch=N/A  34h=N/A  3Ch=N/A
05h=N/A   0Dh=BREAK   15h=N/A  1Dh=N/A   25h=OR   2Dh=N/A  35h=N/A  3Dh=N/A
06h=SRLV  0Eh=N/A     16h=N/A  1Eh=N/A   26h=XOR  2Eh=N/A  36h=N/A  3Eh=N/A
07h=SRAV  0Fh=N/A     17h=N/A  1Fh=N/A   27h=NOR  2Fh=N/A  37h=N/A  3Fh=N/A
*/

char *ops[] = {
  "SPECIAL",
  "BcondZ ",
  "J      ",
  "JAL    ",
  "BEQ    ",
  "BNE    ",
  "BLEZ   ",
  "BGTZ   ",
  "ADDI   ",
  "ADDIU  ",
  "SLTI   ",
  "SLTIU  ",
  "ANDI   ",
  "ORI    ",
  "XORI   ",
  "LUI    ",
  "COP0   ",
  "COP1   ",
  "COP2   ",
  "COP3   ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "LB     ",
  "LH     ",
  "LWL    ",
  "LW     ",
  "LBU    ",
  "LHU    ",
  "LWR    ",
  "N/A    ",
  "SB     ",
  "SH     ",
  "SWL    ",
  "SW     ",
  "N/A    ",
  "N/A    ",
  "SWR    ",
  "N/A    ",
  "LWC0   ",
  "LWC1   ",
  "LWC2   ",
  "LWC3   ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "SWC0   ",
  "SWC1   ",
  "SWC2   ",
  "SWC3   ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "SLL    ",
  "N/A    ",
  "SRL    ",
  "SRA    ",
  "SLLV   ",
  "N/A    ",
  "SRLV   ",
  "SRAV   ",
  "JR     ",
  "JALR   ",
  "N/A    ",
  "N/A    ",
  "SYSCALL",
  "BREAK  ",
  "N/A    ",
  "N/A    ",
  "MFHI   ",
  "MTHI   ",
  "MFLO   ",
  "MTLO   ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "MULT   ",
  "MULTU  ",
  "DIV    ",
  "DIVU   ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "ADD    ",
  "ADDU   ",
  "SUB    ",
  "SUBU   ",
  "AND    ",
  "OR     ",
  "XOR    ",
  "NOR    ",
  "N/A    ",
  "N/A    ",
  "SLT    ",
  "SLTU   ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "N/A    ",
  "BLTZ   ",
  "BGEZ   ",
  "BLTZAL ",
  "BGEZAL "
};


int op_types[] = {
  /*"SPECIAL"*/INST_NONE,
  /*"BcondZ "*/INST_NONE,
  /*"J      "*/INST_IMM_0_26,
  /*"JAL    "*/INST_IMM_0_26,
  /*"BEQ    "*/INST_IMM11_16,
  /*"BNE    "*/INST_IMM11_16,
  /*"BLEZ   "*/INST_IMM10_16,
  /*"BGTZ   "*/INST_IMM10_16,
  /*"ADDI   "*/INST_IMM11_16,
  /*"ADDIU  "*/INST_IMM11_16,
  /*"SLTI   "*/INST_IMM11_16,
  /*"SLTIU  "*/INST_IMM11_16,
  /*"ANDI   "*/INST_IMM11_16,
  /*"ORI    "*/INST_IMM11_16,
  /*"XORI   "*/INST_IMM11_16,
  /*"LUI    "*/INST_IMM01_16,
  /*"COP0   "*/INST_IMM_0_25,
  /*"COP1   "*/INST_IMM_0_25,
  /*"COP2   "*/INST_IMM_0_25,
  /*"COP3   "*/INST_IMM_0_25,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"LB     "*/INST_IMM11_16_MEM_LOAD,
  /*"LH     "*/INST_IMM11_16_MEM_LOAD,
  /*"LWL    "*/INST_IMM11_16_MEM_LOAD,
  /*"LW     "*/INST_IMM11_16_MEM_LOAD,
  /*"LBU    "*/INST_IMM11_16_MEM_LOAD,
  /*"LHU    "*/INST_IMM11_16_MEM_LOAD,
  /*"LWR    "*/INST_IMM11_16_MEM_LOAD,
  /*"N/A    "*/INST_NONE,
  /*"SB     "*/INST_IMM11_16_MEM_STORE,
  /*"SH     "*/INST_IMM11_16_MEM_STORE,
  /*"SWL    "*/INST_IMM11_16_MEM_STORE,
  /*"SW     "*/INST_IMM11_16_MEM_STORE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"SWR    "*/INST_IMM11_16_MEM_STORE,
  /*"N/A    "*/INST_NONE,
  /*"LWC0   "*/INST_IMM11_16,
  /*"LWC1   "*/INST_IMM11_16,
  /*"LWC2   "*/INST_IMM11_16,
  /*"LWC3   "*/INST_IMM11_16,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"SWC0   "*/INST_IMM11_16,
  /*"SWC1   "*/INST_IMM11_16,
  /*"SWC2   "*/INST_IMM11_16,
  /*"SWC3   "*/INST_IMM11_16,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"SLL    "*/INST_R0111_SHIFT,
  /*"N/A    "*/INST_NONE,
  /*"SRL    "*/INST_R0111_SHIFT,
  /*"SRA    "*/INST_R0111_SHIFT,
  /*"SLLV   "*/INST_R1110,
  /*"N/A    "*/INST_NONE,
  /*"SRLV   "*/INST_R1110_SHIFT,
  /*"SRAV   "*/INST_R1110_SHIFT,
  /*"JR     "*/INST_R1000,
  /*"JALR   "*/INST_R1010,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"SYSCALL"*/INST_IMM_0_20,
  /*"BREAK  "*/INST_IMM_0_20,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"MFHI   "*/INST_R0010,
  /*"MTHI   "*/INST_R1000,
  /*"MFLO   "*/INST_R0010,
  /*"MTLO   "*/INST_R1000,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"MULT   "*/INST_R1100,
  /*"MULTU  "*/INST_R1100,
  /*"DIV    "*/INST_R1100,
  /*"DIVU   "*/INST_R1100 ,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"ADD    "*/INST_R1110,
  /*"ADDU   "*/INST_R1110,
  /*"SUB    "*/INST_R1110,
  /*"SUBU   "*/INST_R1110,
  /*"AND    "*/INST_R1110,
  /*"OR     "*/INST_R1110,
  /*"XOR    "*/INST_R1110,
  /*"NOR    "*/INST_R1110,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"SLT    "*/INST_R1110,
  /*"SLTU   "*/INST_R1110,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"N/A    "*/INST_NONE,
  /*"BLTZ   "*/INST_IMM01_16,
  /*"BGEZ   "*/INST_IMM01_16,
  /*"BLTZAL "*/INST_IMM01_16,
  /*"BGEZAL "*/INST_IMM01_16
};

char *registers[] = {
  "0 ",
  "at",
  "v0",
  "v1",
  "a0",
  "a1",
  "a2",
  "a3",
  "t0",
  "t1",
  "t2",
  "t3",
  "t4",
  "t5",
  "t6",
  "t7",
  "s0",
  "s1",
  "s2",
  "s3",
  "s4",
  "s5",
  "s6",
  "s7",
  "t8",
  "t9",
  "k0",
  "k1",
  "gp",
  "sp",
  "fp",
  "ra"
};

char *registers_nospace[] = {
  "0",
  "at",
  "v0",
  "v1",
  "a0",
  "a1",
  "a2",
  "a3",
  "t0",
  "t1",
  "t2",
  "t3",
  "t4",
  "t5",
  "t6",
  "t7",
  "s0",
  "s1",
  "s2",
  "s3",
  "s4",
  "s5",
  "s6",
  "s7",
  "t8",
  "t9",
  "k0",
  "k1",
  "gp",
  "sp",
  "fp",
  "ra"
};

/*
  Name       Alias    Common Usage
  R0         zero     Constant (always 0)
  R1         at       Assembler temporary (destroyed by some assembler pseudoinstructions!)
  R2-R3      v0-v1    Subroutine return values, may be changed by subroutines
  R4-R7      a0-a3    Subroutine arguments, may be changed by subroutines
  R8-R15     t0-t7    Temporaries, may be changed by subroutines
  R16-R23    s0-s7    Static variables, must be saved by subs
  R24-R25    t8-t9    Temporaries, may be changed by subroutines
  R26-R27    k0-k1    Reserved for kernel (destroyed by some IRQ handlers!)
  R28        gp       Global pointer (rarely used)
  R29        sp       Stack pointer
  R30        fp(s8)   Frame Pointer, or 9th Static variable, must be saved
  R31        ra       Return address (used so by JAL,BLTZAL,BGEZAL opcodes)
  -          pc       Program counter
  -          hi,lo    Multiply/divide results, may be changed by subroutines
*/

char *cop_regs[4][64] = {
  {
    "N/A     ",
    "N/A     ",
    "N/A     ",
    "BPC     ",
    "N/A     ",
    "BDA     ",
    "JUMPDEST",
    "DCIC    ",
    "BadVaddr",
    "BDAM    ",
    "N/A     ",
    "BPCM    ",
    "SR      ",
    "CAUSE   ",
    "EPC     ",
    "PRID    "
  },
  {},
  {
    "VXY0    ",
    "VZ0     ",
    "VXY1    ",
    "VZ1     ",
    "VXY2    ",
    "VZ2     ",
    "RGBC    ",
    "OTZ     ",
    "IR0     ",
    "IR1     ",
    "IR2     ",
    "IR3     ",
    "SXY0    ",
    "SXY1    ",
    "SXY2    ",
    "SXYP    ",
    "SZ0     ",
    "SZ1     ",
    "SZ2     ",
    "SZ3     ",
    "RGB0    ",
    "RGB1    ",
    "RGB2    ",
    "RES1    ",
    "MAC0    ",
    "MAC1    ",
    "MAC2    ",
    "MAC3    ",
    "IRGB    ",
    "ORGB    ",
    "LZCS    ",
    "LZCR    ",
    "RTM0    ",
    "RTM1    ",
    "RTM2    ",
    "RTM3    ",
    "RTM4    ",
    "TRX     ",
    "TRY     ",
    "TRZ     ",
    "LLM0    ",
    "LLM1    ",
    "LLM2    ",
    "LLM3    ",
    "LLM4    ",
    "RBK     ",
    "GBK     ",
    "BBK     ",
    "LCM0    ",
    "LCM1    ",
    "LCM2    ",
    "LCM3    ",
    "LCM4    ",
    "RFC     ",
    "GFC     ",
    "BFC     ",
    "OFX     ",
    "OFY     ",
    "H       ",
    "DQA     ",
    "DQB     ",
    "ZSF3    ",
    "ZSF4    ",
    "FLAG    "
  },
  {}
};

char *cop_regs_nospace[4][64] = {
  {
    "N/A",
    "N/A",
    "N/A",
    "BPC",
    "N/A",
    "BDA",
    "JUMPDEST",
    "DCIC",
    "BadVaddr",
    "BDAM",
    "N/A",
    "BPCM",
    "SR",
    "CAUSE",
    "EPC",
    "PRID"
  },
  {},
  {
    "VXY0",
    "VZ0",
    "VXY1",
    "VZ1",
    "VXY2",
    "VZ2",
    "RGBC",
    "OTZ",
    "IR0",
    "IR1",
    "IR2",
    "IR3",
    "SXY0",
    "SXY1",
    "SXY2",
    "SXYP",
    "SZ0",
    "SZ1",
    "SZ2",
    "SZ3",
    "RGB0",
    "RGB1",
    "RGB2",
    "RES1",
    "MAC0",
    "MAC1",
    "MAC2",
    "MAC3",
    "IRGB",
    "ORGB",
    "LZCS",
    "LZCR",
    "RTM0",
    "RTM1",
    "RTM2",
    "RTM3",
    "RTM4",
    "TRX",
    "TRY",
    "TRZ",
    "LLM0",
    "LLM1",
    "LLM2",
    "LLM3",
    "LLM4",
    "RBK",
    "GBK",
    "BBK",
    "LCM0",
    "LCM1",
    "LCM2",
    "LCM3",
    "LCM4",
    "RFC",
    "GFC",
    "BFC",
    "OFX",
    "OFY",
    "H",
    "DQA",
    "DQB",
    "ZSF3",
    "ZSF4",
    "FLAG"
  },
  {}
};

uint32_t get_opcode(instruction inst)
{
  uint32_t opcode = inst.rtype.opcode;
  if (opcode == 0)
    opcode = inst.rtype.special+64;

  if (opcode == 1)
    opcode = (((inst.inst>>16)&1)|((inst.inst>>20)&1))+128;

  return opcode;
}

uint32_t validate_opcode(uint32_t opcode)
{
  char *codename = ops[opcode];

  return strcmp(codename, "N/A    ");
}

int validate_instruction(instruction inst)
{
  if (!validate_opcode(inst.opcode))
    return 1;

  int type = op_types[inst.opcode];

  if (type == INST_NONE)
    return 1;
    
  switch (type) {
  case INST_R1000:
    if(inst.inst&0b111111111111111000000) return 1;
    break;
  case INST_R0010:
    if(inst.inst&0b11111111110000011111000000) return 1;
    break;
  case INST_R0111:
  case INST_R0111_SHIFT:
    if(inst.inst&0b11111000000000000000000000) return 1;
    break;
  case INST_R1010:
    if(inst.inst&0b111110000011111000000) return 1;
    break;
  case INST_R1100:
    if(inst.inst&0b1111111111000000) return 1;
    break;
  case INST_R1110:
  case INST_R1110_SHIFT:
    if(inst.inst&0b11111000000) return 1;
    break;
  case INST_IMM11_16:
    break;
  case INST_IMM11_16_MEM_LOAD:
    break;
  case INST_IMM11_16_MEM_STORE:
    break;
  case INST_IMM10_16:
    break;
  case INST_IMM01_16:
    break;
  case INST_IMM_0_20:
    break;
  case INST_IMM_0_25:
    break;
  case INST_IMM_0_26:
    break;
  default:
    return 1;
    break;
  }

  return 0;
}

int simplify_lui(struct program prog, instruction inst, instruction inst2, instruction inst3, instruction inst4, function_list *func_list, int double_inst)
{
  uint32_t num = inst.i16type.imm << 16;

  if (double_inst <= 1)
  {
    uint32_t addr = inst.i16type.imm << 16;
    fprintf(prog.output, "  %s = %s;%s\n", registers_nospace[inst.i16type.r2], get_address_name(prog, addr), address_description(prog, addr));
    return 0;
  }

  if (
       inst.i16type.r2 == inst2.rtype.r3 
    && (inst2.rtype.r3 == inst2.rtype.r1 || inst2.rtype.r3 == inst2.rtype.r2)
    && (inst2.opcode == ADD || inst2.opcode == ADDU)
  )
  {
    uint32_t add_reg = inst2.rtype.r2;
    if (inst2.rtype.r3 == inst2.rtype.r2)
      add_reg = inst2.rtype.r1;

    if (
      inst3.i16type.r1 == inst.i16type.r2 &&
      (inst3.opcode == LW || inst3.opcode == LH || inst3.opcode == LB || inst3.opcode == LHU || inst3.opcode == LBU)
    )
    {
      uint32_t addr = inst3.i16type.imm;
      if (addr&0x8000)
        addr |= 0xFFFF0000;

      addr += num;
      switch (inst3.opcode) {
      case LW:
        fprintf(prog.output, "  %s = lw(%s + %s);%s\n", registers_nospace[inst3.i16type.r2], get_address_name(prog, addr), registers_nospace[add_reg], address_description(prog, addr));
        break;
      case LH:
        fprintf(prog.output, "  %s = lh(%s + %s);%s\n", registers_nospace[inst3.i16type.r2], get_address_name(prog, addr), registers_nospace[add_reg], address_description(prog, addr));
        break;
      case LB:
        fprintf(prog.output, "  %s = lb(%s + %s);%s\n", registers_nospace[inst3.i16type.r2], get_address_name(prog, addr), registers_nospace[add_reg], address_description(prog, addr));
        break;
      case LHU:
        fprintf(prog.output, "  %s = lhu(%s + %s);%s\n", registers_nospace[inst3.i16type.r2], get_address_name(prog, addr), registers_nospace[add_reg], address_description(prog, addr));
        break;
      case LBU:
        fprintf(prog.output, "  %s = lbu(%s + %s);%s\n", registers_nospace[inst3.i16type.r2], get_address_name(prog, addr), registers_nospace[add_reg], address_description(prog, addr));
        break;
      default:
        assert(0);
      }
      return 2;
    }

    if (
      inst3.i16type.r1 == inst.i16type.r2 && inst3.i16type.r1 == 1 &&
      (inst3.opcode == SW || inst3.opcode == SH || inst3.opcode == SB)
    )
    {
      uint32_t addr = inst3.i16type.imm;
      if (addr&0x8000)
        addr |= 0xFFFF0000;
      
      addr += num;
      switch (inst3.opcode) {
      case SW:
        fprintf(prog.output, "  sw(%s + %s, %s);%s\n", get_address_name(prog, addr), registers_nospace[add_reg], registers_nospace[inst3.i16type.r2], address_description(prog, addr));
        break;
      case SH:
        fprintf(prog.output, "  sh(%s + %s, %s);%s\n", get_address_name(prog, addr), registers_nospace[add_reg], registers_nospace[inst3.i16type.r2], address_description(prog, addr));
        break;
      case SB:
        fprintf(prog.output, "  sb(%s + %s, %s);%s\n", get_address_name(prog, addr), registers_nospace[add_reg], registers_nospace[inst3.i16type.r2], address_description(prog, addr));
        break;
      default:
        assert(0);
      }
      return 2;
    }

    if (inst3.i16type.r1 == inst3.i16type.r2 && (inst3.opcode == ADDI || inst3.opcode == ADDIU))
    {
      uint32_t addr = inst3.i16type.imm;
      if (addr&0x8000)
        addr |= 0xFFFF0000;
      addr += num;
      fprintf(prog.output, "  %s = %s + %s;%s\n", registers_nospace[inst.i16type.r2], get_address_name(prog, addr), registers_nospace[add_reg], address_description(prog, addr));
      return 2;
    }

    const uint32_t addr = inst.i16type.imm << 16;
  
    fprintf(prog.output, "  %s = %s + %s;%s\n", registers_nospace[inst2.rtype.r3], get_address_name(prog, addr), registers_nospace[add_reg], address_description(prog, addr));
    return 1;
  }

  if (
    inst2.i16type.r1 == inst.i16type.r2 && inst2.i16type.r1 == inst2.i16type.r2
    && inst.i16type.imm > 0x8000 && inst.i16type.imm < 0x8200
    && (inst2.opcode == LW || inst2.opcode == LH || inst2.opcode == LB || inst2.opcode == LHU || inst2.opcode == LBU)
  )
  {
    uint32_t addr = inst2.i16type.imm;
    if (addr&0x8000)
      addr |= 0xFFFF0000;
    
    addr += num;
    switch (inst2.opcode) {
    case LW:
      fprintf(prog.output, "  %s = lw(%s);%s\n", registers_nospace[inst2.i16type.r2], get_address_name(prog, addr), address_description(prog, addr));
      break;
    case LH:
      fprintf(prog.output, "  %s = lh(%s);%s\n", registers_nospace[inst2.i16type.r2], get_address_name(prog, addr), address_description(prog, addr));
      break;
    case LB:
      fprintf(prog.output, "  %s = lb(%s);%s\n", registers_nospace[inst2.i16type.r2], get_address_name(prog, addr), address_description(prog, addr));
      break;
    case LHU:
      fprintf(prog.output, "  %s = lhu(%s);%s\n", registers_nospace[inst2.i16type.r2], get_address_name(prog, addr), address_description(prog, addr));
      break;
    case LBU:
      fprintf(prog.output, "  %s = lbu(%s);%s\n", registers_nospace[inst2.i16type.r2], get_address_name(prog, addr), address_description(prog, addr));
      break;
    default:
      assert(0);
    }
    return 1;
  }

  if (
    inst2.i16type.r1 == inst.i16type.r2 && inst2.i16type.r1 == 1
    && inst.i16type.imm > 0x8000 && inst.i16type.imm < 0x8200
    && (inst2.opcode == SW || inst2.opcode == SH || inst2.opcode == SB)
  )
  {
    uint32_t addr = inst2.i16type.imm;
    if (addr&0x8000)
      addr |= 0xFFFF0000;
    
    addr += num;
    switch (inst2.opcode) {
    case SW:
      fprintf(prog.output, "  sw(%s, %s);%s\n", get_address_name(prog, addr), registers_nospace[inst2.i16type.r2], address_description(prog, addr));
      break;
    case SH:
      fprintf(prog.output, "  sh(%s, %s);%s\n", get_address_name(prog, addr), registers_nospace[inst2.i16type.r2], address_description(prog, addr));
      break;
    case SB:
      fprintf(prog.output, "  sb(%s, %s);%s\n", get_address_name(prog, addr), registers_nospace[inst2.i16type.r2], address_description(prog, addr));
      break;
    default:
      assert(0);
    }
    return 1;
  }

  if (inst.i16type.r2 == inst2.i16type.r1 && inst2.i16type.r1 == inst2.i16type.r2 && inst2.opcode == ORI)
  {
    uint32_t addr = inst2.i16type.imm;
    addr += num;
    fprintf(prog.output, "  %s = %s;%s\n", registers_nospace[inst.i16type.r2], get_address_name(prog, addr), address_description(prog, addr));
    return 1;
  }

  if (inst.i16type.r2 == inst2.i16type.r1 && inst2.i16type.r1 == inst2.i16type.r2 && (inst2.opcode == ADDI || inst2.opcode == ADDIU))
  {
    uint32_t addr = inst2.i16type.imm;
    if (addr&0x8000)
      addr |= 0xFFFF0000;
    addr += num;
    fprintf(prog.output, "  %s = %s;%s\n", registers_nospace[inst.i16type.r2], get_address_name(prog, addr), address_description(prog, addr));
    return 1;
  }

  fprintf(prog.output, "  %s = %s;%s\n", registers_nospace[inst.i16type.r2], get_address_name(prog, num), address_description(prog, num));
  return 0;
}

typedef union {
  uint32_t val;
  struct {
    uint32_t cmd            : 6; // 0-5    Real GTE Command Number (00h..3Fh) (used by hardware)
    uint32_t zero2          : 4; // 6-9    Always zero                        (ignored by hardware)
    uint32_t saturate       : 1; // 10     lm - Saturate IR1,IR2,IR3 result (0=To -8000h..+7FFFh, 1=To 0..+7FFFh)
    uint32_t zero1          : 2; // 11-12  Always zero                        (ignored by hardware)
    uint32_t trans_vec      : 2; // 13-14  MVMVA Translation Vector (0=TR, 1=BK, 2=FC/Bugged, 3=None)
    uint32_t mul_vec        : 2; // 15-16  MVMVA Multiply Vector    (0=V0, 1=V1, 2=V2, 3=IR/long)
    uint32_t mul_mat        : 2; // 17-18  MVMVA Multiply Matrix    (0=Rotation. 1=Light, 2=Color, 3=Reserved)
    uint32_t shift_fraction : 1; // 19     sf - Shift Fraction in IR registers (0=No fraction, 1=12bit fraction)
    uint32_t fake_cmd       : 5; // 20-24  Fake GTE Command Number (00h..1Fh) (ignored by hardware)
    uint32_t out_of_bounds  : 7; // 31-25  Must be 0100101b for "COP2 imm25" instructions
  };
} gte_cmd;

void panic_incorrect_gte(uint32_t addr, gte_cmd cmd)
{
  printf("GTE code 0x%.8X at address 0x%.8X incorrect!\n", cmd.val, addr);
  assert(0);
}

void print_cop2_instruction(FILE *fd, instruction inst)
{
  //fprintf(fd, "  COP2(0x%X);", inst.i25type.imm);

  gte_cmd cmd = {.val = inst.i25type.imm};

  char *mx_name[] = {
    "MX_RT",
    "MX_LLM",
    "MX_RCM",
    "MX_ERROR"
  };

  char *v_name[] = {
    "V_V0",
    "V_V1",
    "V_V2",
    "V_IR"
  };

  char *cv_name[] = {
    "CV_TR",
    "CV_BK",
    "CV_FC",
    "CV_NONE"
  };

  char *sf_name[] = {
    "SF_OFF",
    "SF_ON"
  };

  char *lm_name[] = {
    "LM_OFF",
    "LM_ON"
  };

  switch (cmd.cmd)
  {
    case 0x01:
      if(cmd.val != 0x180001) panic_incorrect_gte(inst.addr, cmd);
      fprintf(fd, "  RTPS();");
      break;
    case 0x06:
      if(cmd.val != 0x1400006) panic_incorrect_gte(inst.addr, cmd);
      fprintf(fd, "  NCLIP();");
      break;
    case 0x0C:
      fprintf(fd, "  OP(%s, %s);", sf_name[cmd.shift_fraction], lm_name[cmd.saturate]);
      break;
    case 0x10:
      if(cmd.val != 0x780010) panic_incorrect_gte(inst.addr, cmd);
      fprintf(fd, "  DPCS();");
      break;
    case 0x11:
      if(cmd.val != 0x980011) panic_incorrect_gte(inst.addr, cmd);
      fprintf(fd, "  INTPL();");
      break;
    case 0x12:
      fprintf(fd, "  MVMVA(%s, %s, %s, %s, %s);", sf_name[cmd.shift_fraction], mx_name[cmd.mul_mat], v_name[cmd.mul_vec], cv_name[cmd.trans_vec], lm_name[cmd.saturate]);
      break;
    case 0x1C:
      if (cmd.val != 0x138041C) panic_incorrect_gte(inst.addr, cmd);
      fprintf(fd, "  CC();");
      break;
    case 0x28:
      fprintf(fd, "  SQR(%s);", sf_name[cmd.shift_fraction]);
      break;
    case 0x30:
      if(cmd.val != 0x280030) panic_incorrect_gte(inst.addr, cmd);
      fprintf(fd, "  RTPT();");
      break;
    case 0x3D:
      fprintf(fd, "  GPF(%s, %s);", sf_name[cmd.shift_fraction], lm_name[cmd.saturate]);
      break;
    case 0x3E:
      fprintf(fd, "  GPL(%s, %s);", sf_name[cmd.shift_fraction], lm_name[cmd.saturate]);
      break;
    default:
      panic_incorrect_gte(inst.addr, cmd);
  }
}

void print_function_call(struct program prog, instruction inst, uint32_t addr)
{
  fprintf(prog.output, "  ");
  print_func_name(prog, addr);
  fprintf(prog.output, ";\n");
}

int output_instruction(instruction inst, instruction inst2, instruction inst3, instruction inst4, struct program prog, function_list *func_list, int double_inst)
{
  if (validate_instruction(inst)) assert(0);

  if (inst.inst == 0)
  {
    return 0;
  }
  
  if (inst.inst == 0x03E00008)
  {
    assert(double_inst != -1);
    output_instruction(inst2, inst2, inst2, inst2, prog, func_list, -1);
    fprintf(prog.output, "  return;\n");
    return 1;
  }

  int32_t branch_jmp = ((int32_t)(int16_t)inst.i16type.imm+1)*4;
  uint32_t addr = (inst.i26type.imm<<2) | (inst.addr & 0xF0000000);

  int16_t num;

  uint32_t goto_addr = inst.addr+branch_jmp;

  switch (inst.opcode) {
  case ADDIU:
  case ADDI:
    num = inst.i16type.imm;

    if (inst.i16type.r1 == inst.i16type.r2)
    {
      switch (num)
      {
      case 0:
        break;
      case 1:
        fprintf(prog.output, "  %s++;", registers_nospace[inst.i16type.r2]);
        break;
      case -1:
        fprintf(prog.output, "  %s--;", registers_nospace[inst.i16type.r2]);
        break;
      default:
        if (num > 0)
          fprintf(prog.output, "  %s += %d; // 0x%4.4X", registers_nospace[inst.i16type.r2], num, num);
        else
          fprintf(prog.output, "  %s -= %d; // 0x%4.4X", registers_nospace[inst.i16type.r2], -num, num);
      }

      break;
    }

    if (num == 0)
    {
      fprintf(prog.output, "  %s = %s;", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1]);
      break;
    }

    if (inst.i16type.r1 == 0)
    {
      fprintf(prog.output, "  %s = %d; // 0x%4.4X", registers_nospace[inst.i16type.r2], num, num);
      break;
    }

    if (num > 0)
      fprintf(prog.output, "  %s = %s + %d; // 0x%4.4X", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], num, num);
    else
      fprintf(prog.output, "  %s = %s - %d; // 0x%4.4X", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], -num, num);
    break;
  case ADDU:
  case ADD:
    if (inst.rtype.r1 == 0 || inst.rtype.r2 == 0)
    {
      fprintf(prog.output, "  %s = %s;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r1 + inst.rtype.r2]);
      break;
    }
    if (inst.rtype.r3 == inst.rtype.r2)
    {
      fprintf(prog.output, "  %s += %s;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r1]);
      break;
    }

    if (inst.rtype.r3 == inst.rtype.r1)
    {
      fprintf(prog.output, "  %s += %s;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r2]);
      break;
    }

    fprintf(prog.output, "  %s = %s + %s;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r1], registers_nospace[inst.rtype.r2]);
    break;
  case SUBU:
  case SUB:
    if (inst.rtype.r1 == 0)
    {
      fprintf(prog.output, "  %s = -%s;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r2]);
      break;
    }

    if (inst.rtype.r1 == inst.rtype.r3)
    {
      fprintf(prog.output, "  %s -= %s;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r2]);
      break;
    }

    fprintf(prog.output, "  %s = %s - %s;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r1], registers_nospace[inst.rtype.r2]);
    break;
  case MULT:
    fprintf(prog.output, "  mult(%s, %s);", registers_nospace[inst.rtype.r1], registers_nospace[inst.rtype.r2]);
    break;
  case MULTU:
    fprintf(prog.output, "  multu(%s, %s);", registers_nospace[inst.rtype.r1], registers_nospace[inst.rtype.r2]);
    break;
  case DIV:
    fprintf(prog.output, "  div_psx(%s,%s);", registers_nospace[inst.rtype.r1], registers_nospace[inst.rtype.r2]);
    break;
  case DIVU:
    fprintf(prog.output, "  divu_psx(%s, %s);", registers_nospace[inst.rtype.r1], registers_nospace[inst.rtype.r2]);
    break;
  case MFHI:
    fprintf(prog.output, "  %s=hi;", registers_nospace[inst.rtype.r3]);
    break;
  case MFLO:
    fprintf(prog.output, "  %s=lo;", registers_nospace[inst.rtype.r3]);
    break;
  case MTHI:
    fprintf(prog.output, "  hi=%s;", registers_nospace[inst.rtype.r1]);
    break;
  case MTLO:
    fprintf(prog.output, "  lo=%s;", registers_nospace[inst.rtype.r1]);
    break;
  case LUI:
    return simplify_lui(prog, inst, inst2, inst3, inst4, func_list, double_inst);
  case SW:
    if (inst.i16type.imm&0x8000)
      fprintf(prog.output, "  sw(%s - 0x%.4X, %s); // 0xFFFF%X", registers_nospace[inst.i16type.r1], 0x10000-inst.i16type.imm, registers_nospace[inst.i16type.r2], inst.i16type.imm);
    else
      fprintf(prog.output, "  sw(%s + 0x%.4X, %s);", registers_nospace[inst.i16type.r1], inst.i16type.imm, registers_nospace[inst.i16type.r2]);
    break;
  case SH:
    if (inst.i16type.imm&0x8000)
      fprintf(prog.output, "  sh(%s - 0x%.4X, %s); // 0xFFFF%X", registers_nospace[inst.i16type.r1], 0x10000-inst.i16type.imm, registers_nospace[inst.i16type.r2], inst.i16type.imm);
    else
      fprintf(prog.output, "  sh(%s + 0x%.4X, %s);", registers_nospace[inst.i16type.r1], inst.i16type.imm, registers_nospace[inst.i16type.r2]);
    break;
  case SB:
    if (inst.i16type.imm&0x8000)
      fprintf(prog.output, "  sb(%s - 0x%.4X, %s); // 0xFFFF%X", registers_nospace[inst.i16type.r1], 0x10000-inst.i16type.imm, registers_nospace[inst.i16type.r2], inst.i16type.imm);
    else
      fprintf(prog.output, "  sb(%s + 0x%.4X, %s);", registers_nospace[inst.i16type.r1], inst.i16type.imm, registers_nospace[inst.i16type.r2]);
    break;
  case LW:
    if (inst.i16type.imm&0x8000)
      fprintf(prog.output, "  %s = lw(%s - 0x%.4X); // 0xFFFF%X", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], 0x10000-inst.i16type.imm, inst.i16type.imm);
    else
      fprintf(prog.output, "  %s = lw(%s + 0x%.4X);", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], inst.i16type.imm);
    break;
  case LH:
    if (inst.i16type.imm&0x8000)
      fprintf(prog.output, "  %s = lh(%s - 0x%.4X); // 0xFFFF%X", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], 0x10000-inst.i16type.imm, inst.i16type.imm);
    else
      fprintf(prog.output, "  %s = lh(%s + 0x%.4X);", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], inst.i16type.imm);
    break;
  case LHU:
    if (inst.i16type.imm&0x8000)
      fprintf(prog.output, "  %s = lhu(%s - 0x%.4X); // 0xFFFF%X", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], 0x10000-inst.i16type.imm, inst.i16type.imm);
    else
      fprintf(prog.output, "  %s = lhu(%s + 0x%.4X);", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], inst.i16type.imm);
    break;
  case LB:
    if (inst.i16type.imm&0x8000)
      fprintf(prog.output, "  %s = lb(%s - 0x%.4X); // 0xFFFF%X", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], 0x10000-inst.i16type.imm, inst.i16type.imm);
    else
      fprintf(prog.output, "  %s = lb(%s + 0x%.4X);", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], inst.i16type.imm);
    break;
  case LBU:
    if (inst.i16type.imm&0x8000)
      fprintf(prog.output, "  %s = lbu(%s - 0x%.4X); // 0xFFFF%X", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], 0x10000-inst.i16type.imm, inst.i16type.imm);
    else
      fprintf(prog.output, "  %s = lbu(%s + 0x%.4X);", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], inst.i16type.imm);
    break;
  case SLTU:
    fprintf(prog.output, "  %s = %s < %s;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r1], registers_nospace[inst.rtype.r2]);
    break;
  case SLT:
    fprintf(prog.output, "  %s = (int32_t)%s < (int32_t)%s;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r1], registers_nospace[inst.rtype.r2]);
    break;
  case SLTI:
    num = inst.i16type.imm;
    fprintf(prog.output, "  %s = (int32_t)%s < %d;", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], num);
    break;
  case SLTIU:
    fprintf(prog.output, "  %s = %s < %d;", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], inst.i16type.imm);
    break;
  case BNE:
    fprintf(prog.output, "  temp = %s != %s;", registers_nospace[inst.i16type.r1], registers_nospace[inst.i16type.r2]);
    goto goto_thing;
  case BEQ:
    fprintf(prog.output, "  temp = %s == %s;", registers_nospace[inst.i16type.r1], registers_nospace[inst.i16type.r2]);
    goto goto_thing;
  case BGTZ:
    fprintf(prog.output, "  temp = (int32_t)%s > 0;", registers_nospace[inst.i16type.r1]);
    goto goto_thing;
  case BLEZ:
    fprintf(prog.output, "  temp = (int32_t)%s <= 0;", registers_nospace[inst.i16type.r1]);
    goto goto_thing;
  case BLTZ:
    fprintf(prog.output, "  temp = (int32_t)%s < 0;", registers_nospace[inst.i16type.r1]);
    goto goto_thing;
  case BGEZ:
    fprintf(prog.output, "  temp = (int32_t)%s >= 0;", registers_nospace[inst.i16type.r1]);
    goto goto_thing;
  case BGEZAL:
  case BLTZAL:
    assert(0);
    break;
  case OR:
    fprintf(prog.output, "  %s = %s | %s;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r1], registers_nospace[inst.rtype.r2]);
    break;
  case XOR:
    fprintf(prog.output, "  %s = %s ^ %s;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r1], registers_nospace[inst.rtype.r2]);
    break;
  case NOR:
    if (inst.rtype.r1 == 0)
      fprintf(prog.output, "  %s = ~%s;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r2]);
    else if (inst.rtype.r2 == 0)
      fprintf(prog.output, "  %s = ~%s;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r1]);
    else
      fprintf(prog.output, "  %s = ~(%s | %s);", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r1], registers_nospace[inst.rtype.r2]);
    break;
  case AND:
    fprintf(prog.output, "  %s = %s & %s;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r1], registers_nospace[inst.rtype.r2]);
    break;
  case ORI:
    fprintf(prog.output, "  %s = %s | 0x%X;", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], inst.i16type.imm);
    break;
  case XORI:
    fprintf(prog.output, "  %s = %s ^ 0x%X;", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], inst.i16type.imm);
    break;
  case ANDI:
    fprintf(prog.output, "  %s = %s & 0x%X;", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], inst.i16type.imm);
    break;
  case SLL:
    fprintf(prog.output, "  %s = %s << %d;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r2], inst.rtype.r4);
    break;
  case SRL:
    fprintf(prog.output, "  %s = %s >> %d;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r2], inst.rtype.r4);
    break;
  case SLLV:
    fprintf(prog.output, "  %s = %s << %s;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r2], registers_nospace[inst.rtype.r1]);
    break;
  case SRLV:
    fprintf(prog.output, "  %s = %s >> %s;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r2], registers_nospace[inst.rtype.r1]);
    break;
  case SRAV:
    fprintf(prog.output, "  %s = (int32_t)%s >> %s;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r2], registers_nospace[inst.rtype.r1]);
    break;
  case SRA:
    fprintf(prog.output, "  %s = (int32_t)%s >> %d;", registers_nospace[inst.rtype.r3], registers_nospace[inst.rtype.r2], inst.rtype.r4);
    break;
  case JAL:

    assert(double_inst != -1);
    output_instruction(inst2, inst2, inst2, inst2, prog, func_list, -1);
    print_function_call(prog, inst, addr);
    
    return 1;
  case JALR:
    fprintf(prog.output, "  temp = %s;\n", registers_nospace[inst.rtype.r1]);
    assert(double_inst != -1);
    output_instruction(inst2, inst2, inst2, inst2, prog, func_list, -1);
    assert(inst.rtype.r3 == 31);
    //fprintf(prog.output, "  JALR(temp, 0x%.8X);\n", inst.addr);
    //fprintf(prog.output, "  fprintf(prog.output, \"line %%d: calling function %%.8X\\n\", __LINE__, temp); BREAKPOINT;\n");
    return 1;
  case J:
    // it doesn't matter what you return here, this is the final function in a subfunction
    if (double_inst > 0) {
      output_instruction(inst2, inst2, inst2, inst2, prog, func_list, -1);
      fprintf(prog.output, "  goto label%.8X;", addr);
    } else {
      assert(0);
    }
    break;
  case JR:
    fprintf(prog.output, "  temp = %s;\n", registers_nospace[inst.rtype.r1]);
    assert(double_inst != -1);
    output_instruction(inst2, inst2, inst2, inst2, prog, func_list, -1);

    //fprintf(prog.output, "  JR(temp, 0x%.8X);\n", inst.addr);
    //fprintf(prog.output, "  fprintf(prog.output, \"line %%d: goto %%.8X\\n\", __LINE__, temp); BREAKPOINT;\n");
    return 1;
  case BREAK:
    fprintf(prog.output, "  BREAKPOINT; // BREAK 0x%.5X", inst.i20type.imm);
    break;
  case COP0:
  case COP2: {}
    uint32_t n = inst.i25type.opcode-COP0;
    if (inst.i25type.one == 1)
    {
      if (inst.i25type.opcode == COP2)
        print_cop2_instruction(prog.output, inst);
      else
        assert(0);
    }
    else
    {
      if (inst.rtype.r1 == 8)
      {
        switch (inst.rtype.r2) {
        case 0:
          fprintf(prog.output, "  temp = BC%d() == 0;", n);
          break;
        case 1:
          fprintf(prog.output, "  temp = BC%d() != 0;", n);
          break;
        default:
          assert(0);
        }
        goto goto_thing;

        return 0;
      }
      else
      {
        uint32_t reg = inst.rtype.r3;
        switch (inst.rtype.r1) {
        case 0:
          if (reg == 31 && n == 2)
            fprintf(prog.output, "  %s = LZCR();", registers_nospace[inst.rtype.r2]);
          else
            fprintf(prog.output, "  %s = cop%d.%s;", registers_nospace[inst.rtype.r2], n, cop_regs_nospace[n][reg]);
          break;
        case 2:
          reg += 32;
          fprintf(prog.output, "  %s = cop%d.%s;", registers_nospace[inst.rtype.r2], n, cop_regs_nospace[n][reg]);
          break;
        case 4:
          fprintf(prog.output, "  cop%d.%s = %s;", n, cop_regs_nospace[n][reg], registers_nospace[inst.rtype.r2]);
          break;
        case 6:
          reg += 32;
          fprintf(prog.output, "  cop%d.%s = %s;", n, cop_regs_nospace[n][reg], registers_nospace[inst.rtype.r2]);
          break;
        default:
          assert(0);
        }
      }
    }
    break;
  case LWC0:
  case LWC2:
  {
    uint32_t cop = inst.opcode - LWC0;
    if (inst.i16type.imm&0x8000)
      fprintf(prog.output, "  cop%d.%s = lw(%s - 0x%.4X); // 0xFFFF%X", cop, cop_regs_nospace[cop][inst.i16type.r2], registers_nospace[inst.i16type.r1], 0x10000-inst.i16type.imm, inst.i16type.imm);
    else
      fprintf(prog.output, "  cop%d.%s = lw(%s + 0x%.4X);", cop, cop_regs_nospace[cop][inst.i16type.r2], registers_nospace[inst.i16type.r1], inst.i16type.imm);
    break;
  }
  case SWC0:
  case SWC2:
  {
    uint32_t cop = inst.opcode - SWC0;
    if (inst.i16type.imm&0x8000)
      if (inst.i16type.r2 == 31 && cop == 2)
        fprintf(prog.output, "  sw(%s - 0x%.4X, LZCR());", registers_nospace[inst.i16type.r1], 0x10000-inst.i16type.imm);
      else
        fprintf(prog.output, "  sw(%s - 0x%.4X, cop%d.%s); // 0xFFFF%X", registers_nospace[inst.i16type.r1], 0x10000-inst.i16type.imm, cop, cop_regs_nospace[cop][inst.i16type.r2], inst.i16type.imm);
    else
      if (inst.i16type.r2 == 31 && cop == 2)
        fprintf(prog.output, "  sw(%s + 0x%.4X, LZCR());", registers_nospace[inst.i16type.r1], inst.i16type.imm);
      else
        fprintf(prog.output, "  sw(%s + 0x%.4X, cop%d.%s);", registers_nospace[inst.i16type.r1], inst.i16type.imm, cop, cop_regs_nospace[cop][inst.i16type.r2]);
    break;
  }
  case LWR:
    if (inst2.opcode == LWL)
    {
      if (inst.i16type.r2 == inst2.i16type.r2 && inst.i16type.r1 == inst2.i16type.r1)
      {
        if (inst.i16type.imm+3 == inst2.i16type.imm)
        {
          fprintf(prog.output, "  %s = lw_unaligned(%s + 0x%.4X);\n", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], inst.i16type.imm);
          return 1;
        }
      }
    }
    fprintf(prog.output, "  %s = LWR(%s, %s + 0x%.4X);\n", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], inst.i16type.imm);
    break;
  case LWL:
    if (inst2.opcode == LWR)
    {
      if (inst.i16type.r2 == inst2.i16type.r2 && inst.i16type.r1 == inst2.i16type.r1)
      {
        if (inst2.i16type.imm+3 == inst.i16type.imm)
        {
          fprintf(prog.output, "  %s = lw_unaligned(%s + 0x%.4X);\n", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], inst2.i16type.imm);
          return 1;
        }
      }
    }
    fprintf(prog.output, "  %s = LWL(%s, %s + 0x%.4X);\n", registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r2], registers_nospace[inst.i16type.r1], inst.i16type.imm);
    break;
  case SWR:
    if (inst2.opcode == SWL)
    {
      if (inst.i16type.r2 == inst2.i16type.r2 && inst.i16type.r1 == inst2.i16type.r1)
      {
        if (inst.i16type.imm+3 == inst2.i16type.imm)
        {
          fprintf(prog.output, "  sw_unaligned(%s + 0x%.4X, %s);\n", registers_nospace[inst.i16type.r1], inst.i16type.imm, registers_nospace[inst.i16type.r2]);
          return 1;
        }
      }
    }
    assert(0);
    fprintf(prog.output, "  SWR(%s + 0x%.4X, %s)", registers_nospace[inst.i16type.r1], inst.i16type.imm, registers_nospace[inst.i16type.r2]);
  case SWL:
    if (inst2.opcode == SWR)
    {
      if (inst.i16type.r2 == inst2.i16type.r2 && inst.i16type.r1 == inst2.i16type.r1)
      {
        if (inst2.i16type.imm+3 == inst.i16type.imm)
        {
          fprintf(prog.output, "  sw_unaligned(%s + 0x%.4X, %s);\n", registers_nospace[inst.i16type.r1], inst2.i16type.imm, registers_nospace[inst.i16type.r2]);
          return 1;
        }
      }
    }
    assert(0);
    fprintf(prog.output, "  SWL(%s + 0x%.4X, %s)", registers_nospace[inst.i16type.r1], inst.i16type.imm, registers_nospace[inst.i16type.r2]);
    break;
  case SYSCALL:
    fprintf(prog.output, "  system_call();");
    break;
  goto_thing:
    if (double_inst < 2) {
      fprintf(prog.output, "\n");
      fprintf(prog.output, "  if (temp) {\n  ");
      output_instruction(inst2, inst2, inst2, inst2, prog, func_list, -1);
      fprintf(prog.output, "    goto label%.8X;\n  }\n", goto_addr);
      return 0;
    }
    fprintf(prog.output, "\n");
    output_instruction(inst2, inst2, inst2, inst2, prog, func_list, -1);
    fprintf(prog.output, "  if (temp) goto label%.8X;\n", goto_addr);
    return 1;
  default:
    fprintf(prog.output, "  work in progress");
    assert(0);
    break;
  }

  fprintf(prog.output, "\n");
  return 0;
}