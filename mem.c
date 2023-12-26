#include "ref/proc.h"
#include "ref/mem.h"
#include <stdlib.h>

sMem* initMem() {
    sMem* psMem = (sMem*)malloc(sizeof(sMem));
    memset(psMem, 0, sizeof(sMem));
    return psMem;
}

int loadROM(sMem* psMem, uint16_t romAddr, FILE* rom, uint16_t romLen) {
    unsigned long ret;

    ret = fread(&psMem->memory[romAddr], romLen, sizeof(uint8_t), rom);

    //Debug
    printf("ROM in Memory:\n");
    for(int i = 0; i < romLen; i++) {
        printf("%02hhX", psMem->memory[romAddr + i]);
    }
    
    return ret;
}

uint16_t fetch(sMem* psMem, sProc* psProc) {
    uint16_t ins = 0;
    memcpy(&ins, &psMem->memory[psProc->pc], sizeof(ins));

    return ins;
}
