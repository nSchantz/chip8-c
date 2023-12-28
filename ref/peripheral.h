#ifndef CHIP_PERIPH_H__
#define CHIP_PERIPH_H__

#include <SDL.h>
#include <SDL_keycode.h>
#include <SDL_video.h>
// #include "internals.h"
#include "shared.h"

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_RES_UPSCALE 10 // (64x32)*10

#define KEY_0 SDLK_0
#define KEY_1 SDLK_1
#define KEY_2 SDLK_2
#define KEY_3 SDLK_3
#define KEY_4 SDLK_4
#define KEY_5 SDLK_5
#define KEY_6 SDLK_6
#define KEY_7 SDLK_7
#define KEY_8 SDLK_8
#define KEY_9 SDLK_9
#define KEY_A SDLK_a
#define KEY_B SDLK_b
#define KEY_C SDLK_c
#define KEY_D SDLK_d
#define KEY_E SDLK_e
#define KEY_F SDLK_f
#define KEY_QUIT SDLK_q

#define INPUT_EXIT 0xFF

// typedef struct sPeriph {
//     SDL_Window* window;    
//     SDL_Renderer* renderer;
//     uint8_t keys[0x10];
// } sPeriph;

sPeriph* initScreen();
void updateScreen(sPeriph* psPeriph, sMem* psMem);
void closeScreen(sPeriph* psPeriph);

uint8_t getKey(sPeriph* psPeriph);
// void getKeyBlock(sProc *psProc, uint8_t reg);

#endif // CHIP_PERIPH_H__
