#pragma once

#include <stdint.h>

struct cop2 {
  uint32_t VXY0;
  uint32_t VZ0;
  uint32_t VXY1;
  uint32_t VZ1;
  uint32_t VXY2;
  uint32_t VZ2;
  uint32_t RGBC;
  uint32_t OTZ;
  uint32_t IR0;
  uint32_t IR1;
  uint32_t IR2;
  uint32_t IR3;
  uint32_t SXY0;
  uint32_t SXY1;
  uint32_t SXY2;
  uint32_t SXYP;
  uint32_t SZ0;
  uint32_t SZ1;
  uint32_t SZ2;
  uint32_t SZ3;
  uint32_t RGB0;
  uint32_t RGB1;
  uint32_t RGB2;
  uint32_t RES1;
  uint32_t MAC0;
  uint32_t MAC1;
  uint32_t MAC2;
  uint32_t MAC3;
  uint32_t IRGB;
  uint32_t ORGB;
  uint32_t LZCS;
  uint32_t LZCR;
  uint32_t RTM0;
  uint32_t RTM1;
  uint32_t RTM2;
  uint32_t RTM3;
  uint32_t RTM4;
  uint32_t TRX;
  uint32_t TRY;
  uint32_t TRZ;
  uint32_t LLM0;
  uint32_t LLM1;
  uint32_t LLM2;
  uint32_t LLM3;
  uint32_t LLM4;
  uint32_t RBK;
  uint32_t GBK;
  uint32_t BBK;
  uint32_t LCM0;
  uint32_t LCM1;
  uint32_t LCM2;
  uint32_t LCM3;
  uint32_t LCM4;
  uint32_t RFC;
  uint32_t GFC;
  uint32_t BFC;
  uint32_t OFX;
  uint32_t OFY;
  uint32_t H;
  uint32_t DQA;
  uint32_t DQB;
  uint32_t ZSF3;
  uint32_t ZSF4;
  union {
    uint32_t FLAG;

    struct {
      uint32_t unused : 12;
      uint32_t IR0sat : 1;
      uint32_t SY2sat : 1;
      uint32_t SX2sat : 1;
      uint32_t MAC0underflow : 1;
      uint32_t MAC0overflow : 1;
      uint32_t div_overflow : 1;
      uint32_t SZ3OTZsat : 1;
      uint32_t Bsat : 1;
      uint32_t Gsat : 1;
      uint32_t Rsat : 1;
      uint32_t IR3sat : 1;
      uint32_t IR2sat : 1;
      uint32_t IR1sat : 1;
      uint32_t MAC3underflow : 1;
      uint32_t MAC2underflow : 1;
      uint32_t MAC1underflow : 1;
      uint32_t MAC3overflow : 1;
      uint32_t MAC2overflow : 1;
      uint32_t MAC1overflow : 1;
      uint32_t err : 1;
    };
  };
};

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

#define MX_RT 0
#define MX_LLM 1
#define MX_RCM 2
#define MX_ERROR 3
 
#define V_V0 0
#define V_V1 1
#define V_V2 2
#define V_IR 3

#define CV_TR 0
#define CV_BK 1
#define CV_FC 2
#define CV_NONE 3

#define SF_OFF 0
#define SF_ON 1

#define LM_OFF 0
#define LM_ON 1

extern struct cop2 cop2;

void MVMVA(uint32_t sf, uint32_t mx, uint32_t v, uint32_t cv, uint32_t lm);
void RTPS(void);
void RTPT(void);
void INTPL(void);
void DPCS(void);
void NCLIP(void);
void GPF(uint32_t sf, uint32_t lm);
void GPL(uint32_t sf, uint32_t lm);
void SQR(uint32_t sf);
void CC(void);
void OP(uint32_t sf, uint32_t lm);
uint32_t LZCR(void);