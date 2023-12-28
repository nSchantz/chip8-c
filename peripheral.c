#include "ref/peripheral.h"

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

    SDL_SetRenderDrawColor(renderer, 0, 0, 139, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    //SDL_Delay(100);

    return psPeriph;
}

void updateScreen(sPeriph* psPeriph, sMem* psMem) {
    uint8_t* pGrid = psMem->pFrameBufSec;
    SDL_Rect* pPixel = (SDL_Rect*)malloc(sizeof(SDL_Rect));

    for (int row = 0; row < SCREEN_HEIGHT; row++) {
        for (int col = 0; col < (SCREEN_WIDTH / 8); col++) { // 1 col = 8 bits (or 8 pixels)
            if (pGrid > psMem->pFrameBufSec + 0x100) 
            {
                printf("Vid  | Attempting to access bytes outside of frame buffer. %p >= %p + 0x100\n", pGrid, psMem->pFrameBufSec);
                return;
            }
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
                
                // this shouldnt clip right?
                pPixel->x = ((col * (SCREEN_WIDTH / 8)) + bit) * SCREEN_RES_UPSCALE;
                pPixel->y = row * SCREEN_RES_UPSCALE;
                pPixel->w = SCREEN_RES_UPSCALE;
                pPixel->h = SCREEN_RES_UPSCALE; 
    
                SDL_RenderFillRect(psPeriph->renderer, pPixel);
            }
        }
        pGrid = pGrid + SCREEN_WIDTH / 8;
    }

    SDL_RenderPresent(psPeriph->renderer);
    free(pPixel);
    SDL_Delay(10);
}

uint8_t getKey() {
    SDL_Event event;

    printf("Key  | Checking key press\n");
    while(SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case KEY_0: return 0x0; 
                case KEY_1: return 0x1;
                case KEY_2: return 0x2;
                case KEY_3: return 0x3;
                case KEY_4: return 0x4;
                case KEY_5: return 0x5;
                case KEY_6: return 0x6;
                case KEY_7: return 0x7;
                case KEY_8: return 0x8;
                case KEY_9: return 0x9;
                case KEY_A: return 0xA;
                case KEY_B: return 0xB;
                case KEY_C: return 0xC;
                case KEY_D: return 0xD;
                case KEY_E: return 0xE;
                case KEY_F: return 0xF;
                default: return 0xFF;
            }
        }
    }
}

void getKeyBlock(sProc* psProc, uint8_t reg) {
    SDL_Event event;

    printf("Key  | Waiting for keypress...\n");
    while (1) {
        SDL_WaitEvent(&event); 
        if (event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case KEY_0: psProc->reg[reg] = 0x0; return; 
                case KEY_1: psProc->reg[reg] = 0x1; return;
                case KEY_2: psProc->reg[reg] = 0x2; return;
                case KEY_3: psProc->reg[reg] = 0x3; return;
                case KEY_4: psProc->reg[reg] = 0x4; return;
                case KEY_5: psProc->reg[reg] = 0x5; return;
                case KEY_6: psProc->reg[reg] = 0x6; return;
                case KEY_7: psProc->reg[reg] = 0x7; return;
                case KEY_8: psProc->reg[reg] = 0x8; return;
                case KEY_9: psProc->reg[reg] = 0x9; return;
                case KEY_A: psProc->reg[reg] = 0xA; return;
                case KEY_B: psProc->reg[reg] = 0xB; return;
                case KEY_C: psProc->reg[reg] = 0xC; return;
                case KEY_D: psProc->reg[reg] = 0xD; return;
                case KEY_E: psProc->reg[reg] = 0xE; return;
                case KEY_F: psProc->reg[reg] = 0xF; return;
                default: break;
            }
        }    
    }   
}

void closeScreen(sPeriph* psPeriph) {
    SDL_DestroyWindow(psPeriph->window);
    free(psPeriph);
    SDL_Quit();
}
