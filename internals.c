#include "ref/internals.h"
#include <stdlib.h>
#include <netinet/in.h>
#include <math.h>

sMem* initMem(uint16_t stackAddr, uint16_t romAddr, uint16_t frameBufAddr) {
    sMem* psMem = (sMem*)malloc(sizeof(sMem));
    if(psMem == NULL)
    {
        return NULL;
    }
    memset(psMem, 0, sizeof(sMem));
    psMem->pFrameBufSec = psMem->memory + frameBufAddr;
    psMem->pStackSec = (uint16_t*)psMem->memory + stackAddr;
    psMem->pTextSec = (uint16_t*)psMem->memory + romAddr;
    return psMem;
}

int loadROM(sMem* psMem, uint16_t romAddr, FILE* rom, uint16_t romLen) {
    unsigned long ret;

    ret = fread(&psMem->memory[romAddr], romLen, sizeof(uint8_t), rom);
    psMem->textSecLen = romLen;

    if (MEM_DEBUG) 
    {
        uint16_t cnt = 0;
        printf("Mem  | ROM in Memory: ");
        for(int i = 0; i < romLen; i++) {
            if (cnt % 2 == 0) 
            {
                printf(" ");
            }
            cnt++;
            printf("%02hhX", psMem->memory[romAddr + i]);
        }
        printf("\n");
    }
    
    return ret;
}

uint16_t fetch(sMem* psMem, sProc* psProc) {
    uint16_t ins = 0;
    memcpy(&ins, &psMem->memory[psProc->pc], sizeof(ins));

    // Big Endian
    ins = ntohs(ins);

    if (MEM_DEBUG)
    {
        printf("Mem  | PC: 0x%04X, Fetched: 0x%04X\n", psProc->pc, ins);
    }
    
    return ins;
}

sProc* initProc(uint16_t stackAddr, uint16_t romAddr) {
    sProc* psProc = (sProc*)malloc(sizeof(sProc));
    if(psProc == NULL)
    {
        return NULL;
    }
    memset(psProc, 0, sizeof(sProc));
    psProc->sp = stackAddr;
    psProc->pc = romAddr;
    return psProc;
}

int decode(sMem* psMem, sProc* psProc, uint16_t ins) {
    if (PROC_DEBUG)
    {
        printf("Proc | Decoding - 0x%04X\n", ins);
        printf("---- | \tOpcode - 0x%X\n", GetPreOp(ins));
    }
    
    switch(GetPreOp(ins))
    {
        case PRE_OP_MULTI_0:
        {
            switch(GetLowThree(ins)) 
            {
                case POST_OP_0_DISP_CLEAR: clearFrameBuf(psMem); goto INC_PC;
                case POST_OP_0_RET: 
                {
                    psProc->sp += INS_SIZE;
                    memcpy(&psProc->pc, &psMem->memory[psProc->sp], sizeof(psProc->pc)); 
                    goto UNALTER_PC;
                }
                default: 
                {
                    uint16_t nextIns = psProc->pc + INS_SIZE;
                    memcpy(&psMem->memory[psProc->sp], &nextIns, sizeof(nextIns)); 
                    psProc->sp -= INS_SIZE;
                    psProc->pc = GetLowThree(ins);   
                    goto UNALTER_PC;                
                } //POST_OP_0_CALL_MACH
            }
            printf("cursed. this should not be reached."); break;
        }
        case PRE_OP_JUMP: psProc->pc = GetLowThree(ins); goto UNALTER_PC;
        case PRE_OP_CALL_SUB: 
        {
            uint16_t nextIns = psProc->pc + INS_SIZE;
            memcpy(&psMem->memory[psProc->sp], &nextIns, sizeof(nextIns)); 
            psProc->sp -= INS_SIZE;
            psProc->pc = GetLowThree(ins);   
            goto UNALTER_PC; 
        }
        case PRE_OP_EQ_SKIP_CONS:
        {
            if (psProc->reg[GetRegX(ins)] == GetByteLow(ins)) { goto SKIP_PC; } else { goto INC_PC; }
        }
        case PRE_OP_NEQ_SKIP_CONS: 
        {
            if (psProc->reg[GetRegX(ins)] != GetByteLow(ins)) { goto SKIP_PC; } else { goto INC_PC; }
        }
        case PRE_OP_EQ_SKIP_VAR: 
        {
            if (psProc->reg[GetRegX(ins)] == psProc->reg[GetRegY(ins)]) { goto SKIP_PC; } else { goto INC_PC; }
        }
        case PRE_OP_ASSIGN_CONST: 
        {
            psProc->reg[GetRegX(ins)] = GetByteLow(ins);
            goto INC_PC;
        }
        case PRE_OP_ADD_CONST: 
        {
            psProc->reg[GetRegX(ins)] = GetRegX(ins) + GetByteLow(ins);
            goto INC_PC;
        }
        case PRE_OP_REG_OPER: 
        {
            switch(Get8PostOp(ins))
            {
                case POST_OP_8_ASSIGN_REG:  psProc->reg[GetRegX(ins)] = psProc->reg[GetRegY(ins)];   goto INC_PC;
                case POST_OP_8_BIT_OR:      psProc->reg[GetRegX(ins)] |= psProc->reg[GetRegY(ins)];  goto INC_PC;
                case POST_OP_8_BIT_AND:     psProc->reg[GetRegX(ins)] &= psProc->reg[GetRegY(ins)];  goto INC_PC;
                case POST_OP_8_BIT_XOR:     psProc->reg[GetRegX(ins)] ^= psProc->reg[GetRegY(ins)];  goto INC_PC;
                case POST_OP_8_ADD_REG: 
                {
                    uint8_t regX = GetRegX(ins);
                    uint8_t regY = GetRegY(ins);
                    psProc->reg[FLAG_REG] = (psProc->reg[regX] + psProc->reg[regY] > UINT8_MAX) ? 1 : 0; 
                    psProc->reg[regX] = psProc->reg[regX] + psProc->reg[regY];
                    goto INC_PC;
                }
                case POST_OP_8_SUB_REG:
                {
                    uint8_t regX = GetRegX(ins);
                    uint8_t regY = GetRegY(ins);
                    psProc->reg[FLAG_REG] = (psProc->reg[regX] - psProc->reg[regY] < 0) ? 0 : 1; 
                    psProc->reg[regX] = psProc->reg[regX] - psProc->reg[regY];
                    goto INC_PC;
                }
                case POST_OP_8_SHIFT_RIGHT: psProc->reg[FLAG_REG] = (psProc->reg[GetRegX(ins)] & 0x01); psProc->reg[GetRegX(ins)] >>= 1; goto INC_PC;
                case POST_OP_8_REV_SUB:
                {
                    uint8_t regX = GetRegX(ins);
                    uint8_t regY = GetRegY(ins);
                    psProc->reg[FLAG_REG] = (psProc->reg[regY] - psProc->reg[regX] < 0) ? 0 : 1; 
                    psProc->reg[regX] = psProc->reg[regY] - psProc->reg[regX];
                    goto INC_PC;
                }
                case POST_OP_8_SHIFT_LEFT:  psProc->reg[FLAG_REG] = (psProc->reg[GetRegX(ins)] & 0x80); psProc->reg[GetRegX(ins)] <<= 1; goto INC_PC;
            }
            break;
        }
        case PRE_OP_NEQ_SKIP_VAR: 
        {  
            if (psProc->reg[GetRegX(ins)] != psProc->reg[GetRegY(ins)]) { goto SKIP_PC; } else { goto INC_PC; }       
        } 
        case PRE_OP_SET_MEMADDR:  psProc->ind = GetLowThree(ins);                   goto INC_PC;
        case PRE_OP_REL_JUMP:     psProc->pc = psProc->reg[0] + GetLowThree(ins);   goto UNALTER_PC;
        case PRE_OP_RAND: 
        {
            uint8_t randVal = ((rand() % 256) & 0xFF);
            uint8_t cns = GetByteLow(ins);
            if (PROC_DEBUG) 
            {
                printf("---- | \t->Rand Value: %d & %d = %d\n", randVal, cns, randVal & cns);
            }
            psProc->reg[GetRegX(ins)] = randVal & cns;  
            goto INC_PC;
        }
        case PRE_OP_DISP_DRAW: writeFrameBuf(psMem, psProc); goto INC_PC;
        case PRE_OP_KEYPRESS: break;

        case PRE_OP_MULTI_F:
        {
            switch(ins & 0x00FF)
            {
                case POST_OP_F_GET_DELAY_TIMER:  psProc->reg[GetRegX(ins)] = psProc->delTimer;  goto INC_PC;                
                case POST_OP_F_KEYPRESS_BLOCK:   psProc->reg[GetRegX(ins)] = 1;                 goto INC_PC; // Replace with blocking function
                case POST_OP_F_SET_DELAY_TIMER:  psProc->delTimer = psProc->reg[GetRegX(ins)];  goto INC_PC;
                case POST_OP_F_SET_SOUND_TIMER:  psProc->sndTimer = psProc->reg[GetRegX(ins)];  goto INC_PC;
                case POST_OP_F_MEM_ADD: psProc->ind += psProc->reg[GetRegX(ins)]; goto INC_PC;
                case POST_OP_F_SPRITE_ADD: break;
                case POST_OP_F_STORE_BCD: break;
                case POST_OP_F_REG_DUMP: regDump(GetRegX(ins), psMem, psProc); goto INC_PC;
                case POST_OP_F_REG_LOAD: regLoad(GetRegX(ins), psMem, psProc); goto INC_PC;
            }
        }        
        default: return -1;    
    }
INC_PC:
    psProc->pc = psProc->pc + INS_SIZE;
    return 0;

SKIP_PC:
    psProc->pc = psProc->pc + (INS_SIZE * 2);
UNALTER_PC:
    return 0;
}

void regDump(uint8_t regLimit, sMem* psMem, sProc* psProc) {
    for (int i = 0; i <= regLimit; i++) {
        psMem->memory[psProc->ind + i] = psProc->reg[i];
    }
}

void regLoad(uint8_t regLimit, sMem* psMem, sProc* psProc) {
    for (int i = 0; i <= regLimit; i++) {
        psProc->reg[i] = psMem->memory[psProc->ind + i];
    }
}

void clearFrameBuf(sMem* psMem) {
    memset(psMem->pFrameBufSec, 0, 0x100);
}

void writeFrameBuf(sMem* psMem, sProc* psProc) {
    for (int i = 0; i < 0x100; i++) {
        psMem->pFrameBufSec[i] = (rand() % 256) & 0xFF;
    }
}

void cleanupInternals(sMem* psMem, sProc* psProc) {
    free(psMem);
    free(psProc);
}
