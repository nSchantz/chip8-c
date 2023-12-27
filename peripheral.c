#include "ref/peripheral.h"
#include <SDL_video.h>

sPeriph* initScreen() {
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
    {
        fprintf(stderr, "SDl2 | Could not init SDL: %s\n", SDL_GetError());
        return NULL;
    }

    sPeriph* psPeriph = (sPeriph*)malloc(sizeof(sPeriph));
    if (psPeriph == NULL) 
    {
        return NULL;
    }
    
    SDL_Window *screen = SDL_CreateWindow("Chip8 - Interpreter",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            (SCREEN_WIDTH * SCREEN_RES_UPSCALE), (SCREEN_HEIGHT * SCREEN_RES_UPSCALE),
            (SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE));
    if(!screen) 
    {
        fprintf(stderr, "SDL2 | Could not create window\n");
        return NULL;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_SOFTWARE);
    if(!renderer) {
        fprintf(stderr, "SDL2 | Could not create renderer\n");
        return NULL;
    }

    psPeriph->window = screen;
    psPeriph->renderer = renderer;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(3000);

    return psPeriph;
}

void updateScreen(sPeriph* psPeriph, sMem* psMem) {
    uint8_t* pGrid = psMem->pFrameBufSec;
    SDL_Rect* pPixel = (SDL_Rect*)malloc(sizeof(SDL_Rect));

    for (int row = 0; row < SCREEN_HEIGHT; row++) {
        for (int col = 0; col < (SCREEN_WIDTH / 8); col++) { // 1 col = 8 bits (or 8 pixels)
            for (int bit = 0; bit < 8; bit++) { // 8 pixels = 1 byte
                // Start with leftmost bit (MSB) and move right
                uint8_t pixel = (pGrid[col] >> (7 - bit)) & 0x01;
                if(pixel) 
                {
                    SDL_SetRenderDrawColor(psPeriph->renderer, 255, 255, 255, 255);
                } 
                else 
                {
                    SDL_SetRenderDrawColor(psPeriph->renderer, 0, 0, 0, 255);
                }
                // TODO: Try to malloc at start of function and just reassign values here instead of remallocing.
                //pPixel = (SDL_Rect*)malloc(sizeof(SDL_Rect));
                // this shouldnt clip right?
                pPixel->x = ((col * (SCREEN_WIDTH / 8)) + bit) * SCREEN_RES_UPSCALE;
                pPixel->y = row * SCREEN_RES_UPSCALE;
                pPixel->w = SCREEN_RES_UPSCALE;
                pPixel->h = SCREEN_RES_UPSCALE; 
                //SDL_RenderDrawPoint(psPeriph->renderer, (col * (SCREEN_WIDTH / 8)) + bit, row);
                SDL_RenderFillRect(psPeriph->renderer, pPixel);
            }
            pGrid = pGrid + SCREEN_WIDTH / 8;
        }
        //pGrid = pGrid + SCREEN_WIDTH;

        if (pGrid > psMem->pFrameBufSec + 0x100) {
            printf("Vid  | Attempting to access bytes outside of frame buffer. %p >= %p + 0x100\n", pGrid, psMem->pFrameBufSec);
            //return;
        }
    }

    // TODO: Try to free pPixel after re-rendering
    SDL_RenderPresent(psPeriph->renderer);
    free(pPixel);
    SDL_Delay(1000);
}

void closeScreen(sPeriph* psPeriph) {
    SDL_DestroyWindow(psPeriph->window);
    free(psPeriph);
    SDL_Quit();
}
