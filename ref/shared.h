#ifndef CHIP_SHARED_H__
#define CHIP_SHARED_H__

#include <stdint.h>
#include <SDL.h>

// Mem Structs
typedef struct sMem {
    uint8_t memory[4096];
    uint8_t* pFrameBufSec;
    uint16_t* pStackSec;
    uint16_t* pTextSec;
    uint8_t* pFontSec;
    uint16_t textSecLen;
} sMem;

typedef struct sPeriph {
    SDL_Window* window;    
    SDL_Renderer* renderer;
    uint8_t keys[0x10];
} sPeriph;

typedef struct sProc {
    uint8_t reg[16];
    uint8_t sp;
    uint8_t sndTimer;
    uint8_t delTimer;
    uint16_t ind;
    uint16_t pc;
} sProc;

#endif // CHIP_SHARED_H__
