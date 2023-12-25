#ifndef CHIP_MEM_H__
#define CHIP_MEM_H__

#include <stdint.h>

typedef struct sMem {
    uint8_t memory[4096];
} sMem;

sMem* initMem();
//uint16_t fetch(sMem* psMem, sProc* psProc);

#endif // CHIP_MEM_H__
