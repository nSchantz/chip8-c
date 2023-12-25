#ifndef CHIP_PROC_H__
#define CHIP_PROC_H__

#include <stdint.h>
#include "mem.h"
#include "op.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define GetRegX(ins) (ins & 0x0F00 >> 8)
#define GetRegY(ins) (ins & 0x00F0 >> 4)
#define GetPreOp(ins) (ins & 0xF000 >> 12)
#define GetLowThree(ins) (ins & 0x0FFF)
#define Get8PostOp(ins) (ins & 0x000F)
#define GetByteLow(ins) (ins & 0x00FF)

typedef struct sProc {
    uint8_t reg[16];
    uint8_t sp;
    uint8_t sndTimer;
    uint8_t delTimer;
    uint16_t ind;
    uint16_t pc;
} sProc;

int decode(sProc* psProc, sMem* psMem, uint16_t ins);

#endif // CHIP_PROC_H__
