#include "ref/peripheral.h"

sPeriph* initScreen() {
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
    {
            fprintf(stderr, "Could not init SDL: %s\n", SDL_GetError());
            return NULL;
    }

    sPeriph* psPeriph = (sPeriph*)malloc(sizeof(sPeriph));
    
    SDL_Window *screen = SDL_CreateWindow("Chip8 - Interpreter",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            64, 32,
            0);
    if(!screen) 
    {
        fprintf(stderr, "Could not create window\n");
        return NULL;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_SOFTWARE);
    if(!renderer) {
        fprintf(stderr, "Could not create renderer\n");
        return NULL;
    }

    psPeriph->window = screen;
    psPeriph->renderer = renderer;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(3000);
}

void closeScreen(sPeriph* psPeriph) {
    SDL_DestroyWindow(psPeriph->window);
    free(psPeriph);
    SDL_Quit();
}
