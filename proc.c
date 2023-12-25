#include "ref/proc.h"
#include <stdlib.h>
#include <math.h>

sProc* initProc(uint16_t stackAddr, uint16_t romAddr) {
    sProc* psProc = (sProc*)malloc(sizeof(sProc));
    memset(psProc, 0, sizeof(sProc));
    return psProc;
}

int decode(sProc* psProc, sMem* psMem, uint16_t ins) {
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
                    break;                
                } //POST_OP_0_CALL_MACH
            }
            break;
        }
        case PRE_OP_JUMP: psProc->pc = GetLowThree(ins); break;
        case PRE_OP_CALL_SUB: 
        {
            psProc->pc = GetLowThree(ins);   
            // Push on stack
            break;
        }
        case PRE_OP_EQ_SKIP_CONS:
        {
            psProc->pc = (psProc->reg[GetRegX(ins)] == GetByteLow(ins)) ? (psProc->pc + 1) : psProc->pc;
            break;
        }
        case PRE_OP_NEQ_SKIP_CONS: 
        {
            psProc->pc = (psProc->reg[GetRegX(ins)] != GetByteLow(ins)) ? (psProc->pc + 1) : psProc->pc;
            break;
        }
        case PRE_OP_EQ_SKIP_VAR: 
        {
            psProc->pc = (psProc->reg[GetRegX(ins)] == psProc->reg[GetRegY(ins)]) ? (psProc->pc + 1) : psProc->pc;
            break;
        }
        case PRE_OP_ASSIGN_CONST: 
        {
            psProc->reg[GetRegX(ins)] = GetByteLow(ins);
            break;
        }
        case PRE_OP_ADD_CONST: 
        {
            psProc->reg[GetRegX(ins)] = GetRegX(ins) + GetByteLow(ins);
            break;
        }
        case PRE_OP_REG_OPER: 
        {
            switch(Get8PostOp(ins))
            {
                case POST_OP_8_ASSIGN_REG:  psProc->reg[GetRegX(ins)] = psProc->reg[GetRegY(ins)];   break;
                case POST_OP_8_BIT_OR:      psProc->reg[GetRegX(ins)] |= psProc->reg[GetRegY(ins)];  break;
                case POST_OP_8_BIT_AND:     psProc->reg[GetRegX(ins)] &= psProc->reg[GetRegY(ins)];  break;
                case POST_OP_8_BIT_XOR:     psProc->reg[GetRegX(ins)] ^= psProc->reg[GetRegY(ins)];  break;
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
            psProc->pc = (psProc->reg[GetRegX(ins)] != psProc->reg[GetRegY(ins)]) ? (psProc->pc + 1) : psProc->pc;       
            break;
        } 
        case PRE_OP_SET_MEMADDR:  psProc->ind = GetLowThree(ins);                                break;
        case PRE_OP_REL_JUMP:     psProc->pc = psProc->reg[0] + GetLowThree(ins);                break;
        case PRE_OP_RAND:         psProc->reg[GetRegX(ins)] = (rand() % 256) & GetByteLow(ins);  break;
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
    return 0;
}
