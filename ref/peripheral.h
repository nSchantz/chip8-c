#include "SDL.h"

typedef struct sPeriph {
    SDL_Window* window;    
    SDL_Renderer* renderer;
} sPeriph;

sPeriph* initScreen();
void closeScreen(sPeriph* psPeriph);
