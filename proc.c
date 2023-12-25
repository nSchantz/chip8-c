#include "ref/proc.h"
#include "ref/op.h"

int decode(sProc* psProc, uint16_t ins) {
    switch((ins & 0xF000) >> 24)
    {
        case PRE_OP_MULTI_0:
        {
            switch(ins & 0x0FFF) 
            {
                case POST_OP_0_DISP_CLEAR: break;
                case POST_OP_0_RET: break;
                default: break; //POST_OP_0_CALL_MACH
            }

            break;
        }

        case PRE_OP_JUMP: break;
        case PRE_OP_CALL_SUB: break;
        case PRE_OP_EQ_SKIP_CONS: break;
        case PRE_OP_NEQ_SKIP_CONS: break;
        case PRE_OP_EQ_SKIP_VAR: break;
        case PRE_OP_ASSIGN_CONST: break;
        case PRE_OP_ADD_CONST: break;
        case PRE_OP_REG_OPER: break;
        case PRE_OP_NEQ_SKIP_VAR: break;
        case PRE_OP_SET_MEMADDR: break;
        case PRE_OP_REL_JUMP: break;
        case PRE_OP_RAND: break;
        case PRE_OP_DISP_DRAW: break;
        case PRE_OP_KEYPRESS: break;

        case PRE_OP_MULTI_F:
        {
            switch(ins & 0x00FF)
            {
                case POST_OP_F_GET_DELAY_TIMER: break;                
                case POST_OP_F_KEYPRESS_BLOCK: break;
                case POST_OP_F_SET_DELAY_TIMER: break;
                case POST_OP_F_SET_SOUND_TIMER: break;
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

