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

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
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
    //SDL_Delay(10);
}

uint8_t getKey(sPeriph* psPeriph) {
    SDL_Event event;

    //printf("Key  | Checking key press\n");
    while(SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                case KEY_0: psPeriph->keys[0x0] = 1; break; 
                case KEY_1: psPeriph->keys[0x1] = 1; break;
                case KEY_2: psPeriph->keys[0x2] = 1; break;
                case KEY_3: psPeriph->keys[0x3] = 1; break;
                case KEY_4: psPeriph->keys[0x4] = 1; break;
                case KEY_5: psPeriph->keys[0x5] = 1; break;
                case KEY_6: psPeriph->keys[0x6] = 1; break;
                case KEY_7: psPeriph->keys[0x7] = 1; break;
                case KEY_8: psPeriph->keys[0x8] = 1; break;
                case KEY_9: psPeriph->keys[0x9] = 1; break;
                case KEY_A: psPeriph->keys[0xA] = 1; break;
                case KEY_B: psPeriph->keys[0xB] = 1; break;
                case KEY_C: psPeriph->keys[0xC] = 1; break;
                case KEY_D: psPeriph->keys[0xD] = 1; break;
                case KEY_E: psPeriph->keys[0xE] = 1; break;
                case KEY_F: psPeriph->keys[0xF] = 1; break;
                case KEY_QUIT: return INPUT_EXIT;
                default: break;
            }
        } 
        else if (event.type == SDL_KEYUP)
        {
            switch(event.key.keysym.sym)
            {
                case KEY_0: psPeriph->keys[0x0] = 0; break; 
                case KEY_1: psPeriph->keys[0x1] = 0; break;
                case KEY_2: psPeriph->keys[0x2] = 0; break;
                case KEY_3: psPeriph->keys[0x3] = 0; break;
                case KEY_4: psPeriph->keys[0x4] = 0; break;
                case KEY_5: psPeriph->keys[0x5] = 0; break;
                case KEY_6: psPeriph->keys[0x6] = 0; break;
                case KEY_7: psPeriph->keys[0x7] = 0; break;
                case KEY_8: psPeriph->keys[0x8] = 0; break;
                case KEY_9: psPeriph->keys[0x9] = 0; break;
                case KEY_A: psPeriph->keys[0xA] = 0; break;
                case KEY_B: psPeriph->keys[0xB] = 0; break;
                case KEY_C: psPeriph->keys[0xC] = 0; break;
                case KEY_D: psPeriph->keys[0xD] = 0; break;
                case KEY_E: psPeriph->keys[0xE] = 0; break;
                case KEY_F: psPeriph->keys[0xF] = 0; break;
                default: break;
            }
        }
    }
    return 0;
}

// void getKeyBlock(sProc* psProc, uint8_t reg) {
//     SDL_Event event;

//     printf("Key  | Waiting for keypress...\n");
//     while (1) {
//         SDL_WaitEvent(&event); 
//         if (event.type == SDL_KEYDOWN)
//         {
//             switch(event.key.keysym.sym)
//             {
//                 case KEY_0: psProc->reg[reg] = 0x0; return; 
//                 case KEY_1: psProc->reg[reg] = 0x1; return;
//                 case KEY_2: psProc->reg[reg] = 0x2; return;
//                 case KEY_3: psProc->reg[reg] = 0x3; return;
//                 case KEY_4: psProc->reg[reg] = 0x4; return;
//                 case KEY_5: psProc->reg[reg] = 0x5; return;
//                 case KEY_6: psProc->reg[reg] = 0x6; return;
//                 case KEY_7: psProc->reg[reg] = 0x7; return;
//                 case KEY_8: psProc->reg[reg] = 0x8; return;
//                 case KEY_9: psProc->reg[reg] = 0x9; return;
//                 case KEY_A: psProc->reg[reg] = 0xA; return;
//                 case KEY_B: psProc->reg[reg] = 0xB; return;
//                 case KEY_C: psProc->reg[reg] = 0xC; return;
//                 case KEY_D: psProc->reg[reg] = 0xD; return;
//                 case KEY_E: psProc->reg[reg] = 0xE; return;
//                 case KEY_F: psProc->reg[reg] = 0xF; return;
//                 default: break;
//             }
//         }    
//     }   
// }

void closeScreen(sPeriph* psPeriph) {
    SDL_DestroyWindow(psPeriph->window);
    free(psPeriph);
    SDL_Quit();
}
