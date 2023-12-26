#ifndef CHIP_INTERNALS_H__
#define CHIP_INTERNALS_H__

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include "op.h"

// Mem Defines
#define MEM_DEBUG 1

// Proc Defines
#define PROC_DEBUG 1
#define INS_SIZE 2
#define GetRegX(ins) ((ins & 0x0F00) >> 8)
#define GetRegY(ins) ((ins & 0x00F0) >> 4)
#define GetPreOp(ins) ((ins & 0xF000) >> 12)
#define GetLowThree(ins) (ins & 0x0FFF)
#define Get8PostOp(ins) (ins & 0x000F)
#define GetByteLow(ins) (ins & 0x00FF)

// Mem Structs
typedef struct sMem {
    uint8_t memory[4096];
} sMem;

// Proc Structs
typedef struct sProc {
    uint8_t reg[16];
    uint8_t sp;
    uint8_t sndTimer;
    uint8_t delTimer;
    uint16_t ind;
    uint16_t pc;
} sProc;

// Mem Function Declarations
sMem* initMem();
uint16_t fetch(sMem* psMem, sProc* psProc);
int loadROM(sMem* psMem, uint16_t romAddr, FILE* rom, uint16_t romLen);

// Proc Function Declarations
sProc* initProc(uint16_t stackAddr, uint16_t romAddr);
int decode(sMem* psMem, sProc* psProc, uint16_t ins);

void cleanupInternals(sMem* psMem, sProc* psProc);

#endif // CHIP_INTERNALS_H__
