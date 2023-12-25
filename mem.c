#include "ref/proc.h"
#include "ref/mem.h"
#include "stdlib.h"

sMem* initMem() {
    sMem* psMem = (sMem*)malloc(sizeof(sMem));
    return psMem;
}

int loadROM() {
    return 1; 
}

uint16_t fetch(sMem* psMem, sProc* psProc) {
    uint16_t ins = 0;
    memcpy(&ins, &psMem->memory[psProc->pc], sizeof(ins));

    return ins;
}
