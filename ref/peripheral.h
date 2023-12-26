#include "SDL.h"

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

typedef struct sPeriph {
    SDL_Window* window;    
    SDL_Renderer* renderer;
} sPeriph;

sPeriph* initScreen();
void closeScreen(sPeriph* psPeriph);
