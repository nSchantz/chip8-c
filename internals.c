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
    psMem->pFontSec = psMem->memory;

    // Move to config file
    uint8_t fontset[FONT_SIZE] =
    {
    	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    	0x20, 0x60, 0x20, 0x20, 0x70, // 1
    	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    // move to memcopy
    for (int i = 0; i < FONT_SIZE; i++) {
        *(psMem->pFontSec + i) = fontset[i];
    }

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
    psProc->delTimer = 0;
    psProc->sndTimer = 0;
    return psProc;
}

int decode(sMem* psMem, sProc* psProc, sPeriph* psPeriph, uint16_t ins) {
    if (PROC_DEBUG)
    {
        printf("Proc | Decoding - 0x%04X\n", ins);
        printf("---- | \tOpcode - 0x%X\n", GetPreOp(ins));
    }
    
    switch(GetPreOp(ins))
    {
        case OP_00ZZ:
        {
            switch(GetLowThree(ins)) 
            {
                case OP_00E0: clearFrameBuf(psMem); goto INC_PC;
                case OP_00EE: 
                {
                    psProc->sp += INS_SIZE;
                    memcpy(&psProc->pc, &psMem->memory[psProc->sp], sizeof(psProc->pc)); 
                    if (PROC_DEBUG) { printf("---- | \t->Stack: 0x%04X\nReturning to: 0x%04X\n", psMem->memory[psProc->sp], psProc->pc); }
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
        case OP_1000: psProc->pc = GetLowThree(ins); goto UNALTER_PC;
        case OP_2000: 
        {
            uint16_t nextIns = psProc->pc + INS_SIZE;
            memcpy(&psMem->memory[psProc->sp], &nextIns, sizeof(nextIns)); 
            psProc->sp -= INS_SIZE;
            psProc->pc = GetLowThree(ins);   
            if (PROC_DEBUG) { printf("---- | \t->Calling 0x%04X. Pushing 0x%04X to stack.\n", psProc->pc, nextIns); }
            goto UNALTER_PC; 
        }
        case OP_3000:
        {
            if (psProc->reg[GetRegX(ins)] == GetByteLow(ins)) { goto SKIP_PC; } else { goto INC_PC; }
        }
        case OP_4000: 
        {
            if (psProc->reg[GetRegX(ins)] != GetByteLow(ins)) { goto SKIP_PC; } else { goto INC_PC; }
        }
        case OP_5000: 
        {
            if (psProc->reg[GetRegX(ins)] == psProc->reg[GetRegY(ins)]) { goto SKIP_PC; } else { goto INC_PC; }
        }
        case OP_6000: 
        {
            psProc->reg[GetRegX(ins)] = GetByteLow(ins);
            goto INC_PC;
        }
        case OP_7000: 
        {
            psProc->reg[GetRegX(ins)] = psProc->reg[GetRegX(ins)] + GetByteLow(ins);
            goto INC_PC;
        }
        case OP_800Z: 
        {
            switch(Get8PostOp(ins))
            {
                case OP_8000: psProc->reg[GetRegX(ins)] = psProc->reg[GetRegY(ins)];   goto INC_PC;
                case OP_8001: psProc->reg[GetRegX(ins)] |= psProc->reg[GetRegY(ins)];  goto INC_PC;
                case OP_8002: psProc->reg[GetRegX(ins)] &= psProc->reg[GetRegY(ins)];  goto INC_PC;
                case OP_8003: psProc->reg[GetRegX(ins)] ^= psProc->reg[GetRegY(ins)];  goto INC_PC;
                case OP_8004: 
                {
                    uint8_t regX = GetRegX(ins);
                    uint8_t regY = GetRegY(ins);
                    uint8_t tempX = psProc->reg[regX]; // Necessary for edgecase where RegX or RegY == FLAG_REG (0xF) 
                    uint8_t tempY = psProc->reg[regY]; //
                    psProc->reg[regX] = psProc->reg[regX] + psProc->reg[regY];
                    psProc->reg[FLAG_REG] = (tempX + tempY > UINT8_MAX) ? 1 : 0;
                    goto INC_PC;
                }
                case OP_8005:
                {
                    uint8_t regX = GetRegX(ins);
                    uint8_t regY = GetRegY(ins);
                    uint8_t tempX = psProc->reg[regX]; // Necessary for edgecase where RegX or RegY == FLAG_REG (0xF) 
                    uint8_t tempY = psProc->reg[regY]; //
                    psProc->reg[regX] = psProc->reg[regX] - psProc->reg[regY];
                    psProc->reg[FLAG_REG] = (tempX  - tempY < 0) ? 0 : 1;
                    goto INC_PC;
                }
                case OP_8006: 
                {
                    uint8_t flagBit = psProc->reg[GetRegX(ins)] & 0x01;
                    psProc->reg[GetRegX(ins)] = psProc->reg[GetRegX(ins)] >>= 1;
                    psProc->reg[FLAG_REG] = flagBit; 
                    goto INC_PC;
                }
                case OP_8007:
                {
                    uint8_t regX = GetRegX(ins);
                    uint8_t regY = GetRegY(ins);
                    uint8_t tempX = psProc->reg[regX]; // Necessary for edgecase where RegX or RegY == FLAG_REG (0xF) 
                    uint8_t tempY = psProc->reg[regY]; //
                    psProc->reg[regX] = psProc->reg[regY] - psProc->reg[regX];
                    psProc->reg[FLAG_REG] = (tempY - tempX < 0) ? 0 : 1;
                    goto INC_PC;
                }
                case OP_800E:
                {
                    uint8_t flagBit = psProc->reg[GetRegX(ins)] >> 7;
                    psProc->reg[GetRegX(ins)] = psProc->reg[GetRegX(ins)] <<= 1;
                    psProc->reg[FLAG_REG] = flagBit;
                    goto INC_PC;
                }
            }
            break;
        }
        case OP_9000: if (psProc->reg[GetRegX(ins)] != psProc->reg[GetRegY(ins)]) { goto SKIP_PC; } else { goto INC_PC; }       
        case OP_A000: psProc->ind = GetLowThree(ins);                               goto INC_PC;
        case OP_B000: psProc->pc = psProc->reg[0] + GetLowThree(ins);               goto UNALTER_PC;
        case OP_C000: 
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
        case OP_D000: writeFrameBuf(psMem, psProc, GetRegX(ins), GetRegY(ins), GetDrawN(ins)); goto INC_PC;
        case OP_E0ZZ: 
        {
            switch(ins & GetByteLow(ins))
            {
                case OP_E09E:  if (psPeriph->keys[psProc->reg[GetRegX(ins)]])  { goto SKIP_PC; } else { goto INC_PC; } 
                case OP_E0A1:  if (!psPeriph->keys[psProc->reg[GetRegX(ins)]]) { goto SKIP_PC; } else { goto INC_PC; }
            }
        }

        case OP_F0ZZ:
        {
            switch(ins & GetByteLow(ins))
            {
                case OP_F007: psProc->reg[GetRegX(ins)] = psProc->delTimer;               goto INC_PC;                
                case OP_F00A: psProc->reg[GetRegX(ins)] = getKeyBlock(psPeriph, psProc);  goto INC_PC;
                case OP_F015: psProc->delTimer = psProc->reg[GetRegX(ins)];               goto INC_PC;
                case OP_F018: psProc->sndTimer = psProc->reg[GetRegX(ins)];               goto INC_PC;
                case OP_F01E: psProc->ind += psProc->reg[GetRegX(ins)];                   goto INC_PC;
                case OP_F029: psProc->ind = psProc->reg[GetRegX(ins)] * 5;                goto INC_PC;
                case OP_F033: 
                {
                    uint8_t val = psProc->reg[GetRegX(ins)];
                    psMem->memory[psProc->ind] = val / 100;  
                    psMem->memory[psProc->ind + 1] = (val / 10) % 10;
                    psMem->memory[psProc->ind + 2] = val % 10;
                    goto INC_PC;
                }
                case OP_F055: regDump(GetRegX(ins), psMem, psProc); goto INC_PC;
                case OP_F065: regLoad(GetRegX(ins), psMem, psProc); goto INC_PC;
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

uint8_t getKeyBlock(sPeriph* psPeriph, sProc* psProc) {
    uint8_t* keys = psPeriph->keys;
    if      (keys[0x0]) { return 0x0; }
    else if (keys[0x1]) { return 0x1; }
    else if (keys[0x2]) { return 0x2; }
    else if (keys[0x3]) { return 0x3; }
    else if (keys[0x4]) { return 0x4; }
    else if (keys[0x5]) { return 0x5; }
    else if (keys[0x6]) { return 0x6; }
    else if (keys[0x7]) { return 0x7; }
    else if (keys[0x8]) { return 0x8; }
    else if (keys[0x9]) { return 0x9; }
    else if (keys[0xA]) { return 0xA; }
    else if (keys[0xB]) { return 0xB; }
    else if (keys[0xC]) { return 0xC; }
    else if (keys[0xD]) { return 0xD; }
    else if (keys[0xE]) { return 0xE; }
    else if (keys[0xF]) { return 0xF; }
    else { psProc->pc -= INS_SIZE; return 0xFF; } // Cycle instruction for blocking.
}

void cleanupInternals(sMem* psMem, sProc* psProc) {
    free(psMem);
    free(psProc);
}
