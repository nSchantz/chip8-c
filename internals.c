#include "ref/internals.h"

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
                    printf("---- | \t->Stack: 0x%04X\nReturning to: 0x%04X\n", psMem->memory[psProc->sp], psProc->pc);
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
            printf("---- | \t->Calling 0x%04X. Pushing 0x%04X to stack.\n", psProc->pc, nextIns);
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
            psProc->reg[GetRegX(ins)] = psProc->reg[GetRegX(ins)] + GetByteLow(ins);
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
                    uint8_t tempX = psProc->reg[regX]; // Necessary for edgecase where RegX or RegY == FLAG_REG (0xF) 
                    uint8_t tempY = psProc->reg[regY]; //
                    psProc->reg[regX] = psProc->reg[regX] + psProc->reg[regY];
                    psProc->reg[FLAG_REG] = (tempX + tempY > UINT8_MAX) ? 1 : 0;
                    goto INC_PC;
                }
                case POST_OP_8_SUB_REG:
                {
                    uint8_t regX = GetRegX(ins);
                    uint8_t regY = GetRegY(ins);
                    uint8_t tempX = psProc->reg[regX]; // Necessary for edgecase where RegX or RegY == FLAG_REG (0xF) 
                    uint8_t tempY = psProc->reg[regY]; //
                    psProc->reg[regX] = psProc->reg[regX] - psProc->reg[regY];
                    psProc->reg[FLAG_REG] = (tempX  - tempY < 0) ? 0 : 1;
                    goto INC_PC;
                }
                case POST_OP_8_SHIFT_RIGHT: 
                {
                    uint8_t flagBit = psProc->reg[GetRegX(ins)] & 0x01;
                    psProc->reg[GetRegX(ins)] = psProc->reg[GetRegX(ins)] >>= 1;
                    psProc->reg[FLAG_REG] = flagBit; 
                    goto INC_PC;
                }
                case POST_OP_8_REV_SUB:
                {
                    uint8_t regX = GetRegX(ins);
                    uint8_t regY = GetRegY(ins);
                    uint8_t tempX = psProc->reg[regX]; // Necessary for edgecase where RegX or RegY == FLAG_REG (0xF) 
                    uint8_t tempY = psProc->reg[regY]; //
                    psProc->reg[regX] = psProc->reg[regY] - psProc->reg[regX];
                    psProc->reg[FLAG_REG] = (tempY - tempX < 0) ? 0 : 1;
                    goto INC_PC;
                }
                case POST_OP_8_SHIFT_LEFT:
                {
                    uint8_t flagBit = psProc->reg[GetRegX(ins)] >> 7;
                    psProc->reg[GetRegX(ins)] = psProc->reg[GetRegX(ins)] <<= 1;
                    psProc->reg[FLAG_REG] = flagBit;
                    goto INC_PC;
                }
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
        case PRE_OP_DISP_DRAW: writeFrameBuf(psMem, psProc, GetRegX(ins), GetRegY(ins), GetDrawN(ins)); goto INC_PC;
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
                case POST_OP_F_STORE_BCD: 
                {
                    uint8_t val = psProc->reg[GetRegX(ins)];
                    psMem->memory[psProc->ind] = val / 100;  
                    psMem->memory[psProc->ind + 1] = (val / 10) % 10;
                    psMem->memory[psProc->ind + 2] = val % 10;
                    goto INC_PC;
                }
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

// A little trickier since the frame buffer is [8 Bytes (* 8 Bits/Pixels) * 32 Rows].
// TODO: Checking wrapping.
void writeFrameBuf(sMem* psMem, sProc* psProc, uint8_t regX, uint8_t regY, uint8_t n) {
    uint8_t* pData = &psMem->memory[psProc->ind]; // I
    uint8_t* pFrameBuf = psMem->pFrameBufSec;
    uint8_t x = psProc->reg[regX] % 64;
    uint8_t y = psProc->reg[regY] % 32;
    psProc->reg[FLAG_REG] = 0;

    // For each line in sprite
    for (int row = 0; row < n; row++) {
        uint8_t sprite = pData[row];
        
        //printf("Printing Sprite 0x%02X at (%d, %d)\n", sprite, x, (y + row));
        for (int spriteBit = 0; spriteBit < 8; spriteBit++) {
            // Offset to location in frameBuf to store bit.
            uint16_t yOffset = (y * 8) + (row * 8); // Check for wrapping or stop if bottom of screen?
            uint16_t xByteOffset = (x + spriteBit) / 8;
            uint8_t xBitOffset = (x + spriteBit) % 8; // For bit mask. May need to check for wrapping here.
            uint8_t bitMask = 0x80; // 0x1000 0000. Combine with xBitOffset to modify frame bits.
            bitMask = bitMask >> xBitOffset;

            // Start with MSB
            uint8_t currSpriteBit = (sprite >> (7 - spriteBit)) & 0x01;
            //printf("Current Sprite: 0x%02X. Current Bit Ind: 0x%02X, Bit Value: 0x%02X\n", sprite, spriteBit, currSpriteBit);
            if (currSpriteBit == 0x01) 
            {
                uint8_t currFrameBit = *(pFrameBuf + xByteOffset + yOffset) & bitMask; // Extract bit of interest in byte.
                currFrameBit = (currFrameBit >> (7 - xBitOffset)) & 0x01; // Shift to get 1/0

                if(currFrameBit == 0x01)
                {
                    *(pFrameBuf + xByteOffset + yOffset) ^= bitMask; // Set bit to 0
                    psProc->reg[FLAG_REG] = 1;
                    //printf("Pixel (%d, %d): OFF\n", (x + spriteBit), (y+row));
                }
                else
                {
                    *(pFrameBuf + xByteOffset + yOffset) |= bitMask; // Set bit to 1
                    //printf("Pixel (%d, %d): ON\n", (x + spriteBit), (y+row));
                }
            }  
        }
    }
    
    // for (int i = 0; i < 0x100; i++) {
    //     psMem->pFrameBufSec[i] = (rand() % 256) & 0xFF;
    // }
}

void cleanupInternals(sMem* psMem, sProc* psProc) {
    free(psMem);
    free(psProc);
}
