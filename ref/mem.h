#ifndef CHIP_MEM_H__
#define CHIP_MEM_H__

#include <stdint.h>
#include <stdio.h>

typedef struct sMem {
    uint8_t memory[4096];
} sMem;

sMem* initMem();
//uint16_t fetch(sMem* psMem, sProc* psProc);

int loadROM(sMem* psMem, uint16_t romAddr, FILE* rom, uint16_t romLen);

#endif // CHIP_MEM_H__
