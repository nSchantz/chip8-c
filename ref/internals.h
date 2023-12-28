#ifndef CHIP_INTERNALS_H__
#define CHIP_INTERNALS_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <netinet/in.h>
#include "shared.h"
#include "op.h"

// Mem Defines
#define MEM_DEBUG 0
#define FONT_SIZE 80

// Proc Defines
#define PROC_DEBUG 0
#define INS_SIZE 2
#define CARRY 1
#define FLAG_REG 0xF
#define GetRegX(ins) ((ins & 0x0F00) >> 8)
#define GetRegY(ins) ((ins & 0x00F0) >> 4)
#define GetPreOp(ins) ((ins & 0xF000) >> 12)
#define GetLowThree(ins) (ins & 0x0FFF)
#define GetDrawN(ins) (ins & 0x000F)
#define Get8PostOp(ins) (ins & 0x000F)
#define GetByteLow(ins) (ins & 0x00FF)

// // Mem Structs
// typedef struct sMem {
//     uint8_t memory[4096];
//     uint8_t* pFrameBufSec;
//     uint16_t* pStackSec;
//     uint16_t* pTextSec;
//     uint16_t textSecLen;
// } sMem;

// Proc Structs
// typedef struct sProc {
//     uint8_t reg[16];
//     uint8_t sp;
//     uint8_t sndTimer;
//     uint8_t delTimer;
//     uint16_t ind;
//     uint16_t pc;
// } sProc;

// Periph Forward Declarations (Avoid cyclic dependence)
// struct sPeriph;
// typedef struct sPeriph sPeriph;
// uint8_t getKey(sPeriph* psPeriph);
// void getKeyBlock(sProc* psProc, uint8_t reg);


// Mem Function Declarations
sMem* initMem(uint16_t stackAddr, uint16_t romAddr, uint16_t frameBufAddr);
uint16_t fetch(sMem* psMem, sProc* psProc);
int loadROM(sMem* psMem, uint16_t romAddr, FILE* rom, uint16_t romLen);

// Proc Function Declarations
sProc* initProc(uint16_t stackAddr, uint16_t romAddr);
int decode(sMem* psMem, sProc* psProc, sPeriph* psPeriph, uint16_t ins);

static void regDump(uint8_t regLimit, sMem* psMem, sProc* psProc);
static void regLoad(uint8_t regLimit, sMem* psMem, sProc* psProc);
static void clearFrameBuf(sMem* psMem);
static void writeFrameBuf(sMem* psMem, sProc* psProc, uint8_t regX, uint8_t regY, uint8_t n);
static uint8_t getKeyBlock(sPeriph* psPeriph, sProc* psProc);

void cleanupInternals(sMem* psMem, sProc* psProc);

#endif // CHIP_INTERNALS_H__
