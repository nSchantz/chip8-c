#include "SDL.h"
#include "internals.h"

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_RES_UPSCALE 10 // (64x32)*10

typedef struct sPeriph {
    SDL_Window* window;    
    SDL_Renderer* renderer;
} sPeriph;

sPeriph* initScreen();
void updateScreen(sPeriph* psPeriph, sMem* psMem);
void closeScreen(sPeriph* psPeriph);
