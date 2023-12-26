#include "ref/internals.h"
#include <stdlib.h>
#include <netinet/in.h>
#include <math.h>

sMem* initMem() {
    sMem* psMem = (sMem*)malloc(sizeof(sMem));
    if(psMem == NULL)
    {
        return NULL;
    }
    memset(psMem, 0, sizeof(sMem));
    return psMem;
}

int loadROM(sMem* psMem, uint16_t romAddr, FILE* rom, uint16_t romLen) {
    unsigned long ret;

    ret = fread(&psMem->memory[romAddr], romLen, sizeof(uint8_t), rom);

    if (MEM_DEBUG) 
    {
        printf("Mem  | ROM in Memory: ");
        for(int i = 0; i < romLen; i++) {
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
        printf("Proc | \t\tOpcode - 0x%X\n", GetPreOp(ins));
    }
    
    switch(GetPreOp(ins))
    {
        case PRE_OP_MULTI_0:
        {
            switch(GetLowThree(ins)) 
            {
                case POST_OP_0_DISP_CLEAR: break;
                case POST_OP_0_RET: break;
                default: 
                {
                    psProc->pc = GetLowThree(ins);   
                    // Push on stack
                    goto UNALTER_PC;                
                } //POST_OP_0_CALL_MACH
            }
            break;
        }
        case PRE_OP_JUMP: psProc->pc = GetLowThree(ins); goto UNALTER_PC;
        case PRE_OP_CALL_SUB: 
        {
            psProc->pc = GetLowThree(ins);   
            // Push on stack
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
                case POST_OP_8_ADD_REG: break;
                case POST_OP_8_SUB_REG: break;
                case POST_OP_8_SHIFT_RIGHT: break;
                case POST_OP_8_REV_SUB: break;
                case POST_OP_8_SHIFT_LEFT: break;
            }
            break;
        }
        case PRE_OP_NEQ_SKIP_VAR: 
        {  
            if (psProc->reg[GetRegX(ins)] != psProc->reg[GetRegY(ins)]) { goto SKIP_PC; } else { goto INC_PC; }       
        } 
        case PRE_OP_SET_MEMADDR:  psProc->ind = GetLowThree(ins);                                goto INC_PC;
        case PRE_OP_REL_JUMP:     psProc->pc = psProc->reg[0] + GetLowThree(ins);                goto UNALTER_PC;
        case PRE_OP_RAND:         psProc->reg[GetRegX(ins)] = (rand() % 256) & GetByteLow(ins);  goto INC_PC;
        case PRE_OP_DISP_DRAW: break;
        case PRE_OP_KEYPRESS: break;

        case PRE_OP_MULTI_F:
        {
            switch(ins & 0x00FF)
            {
                case POST_OP_F_GET_DELAY_TIMER:  psProc->reg[GetRegX(ins)] = psProc->delTimer;  break;                
                case POST_OP_F_KEYPRESS_BLOCK:   psProc->reg[GetRegX(ins)] = 1;                 break; // Replace with blocking function
                case POST_OP_F_SET_DELAY_TIMER:  psProc->delTimer = psProc->reg[GetRegX(ins)];  break;
                case POST_OP_F_SET_SOUND_TIMER:  psProc->sndTimer = psProc->reg[GetRegX(ins)];  break;
                case POST_OP_F_MEM_ADD: break;
                case POST_OP_F_SPRITE_ADD: break;
                case POST_OP_F_STORE_BCD: break;
                case POST_OP_F_REG_DUMP: break;
                case POST_OP_F_REG_LOAD: break;
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