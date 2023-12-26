#include "ref/emu.h"

#include "SDL.h"

int main(int argc, char* argv[]) {
    FILE* pfROM;
    uint16_t romLen = 0;
    unsigned char* pROMBuf;

    // Read Args/Settings In
    if (argc != 2)
    {
        printf("Usage: ./emu {rom path}");
        return 0;
    }

    // Open ROM file, get length
    pfROM = fopen(argv[1], "r");
    fseek(pfROM, 0, SEEK_END);
    romLen = ftell(pfROM);
    rewind(pfROM);

    // Create/Init Memory
    sMem* psMem = initMem();
    if (psMem == NULL)
    {
        printf("Memory failed to initalize.\n");
    }

    // Load ROM
    loadROM(psMem, EMU_ROM_ADDR, pfROM, romLen);
    fclose(pfROM);
    
    // Create/Init Processor
    sProc* psProc = initProc(EMU_STACK_ADDR, EMU_ROM_ADDR);
    if (psProc == NULL)
    {
        printf("Processor failed to initalize.\n");
    }
    
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Could not init SDL: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *screen = SDL_CreateWindow("My application",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            640, 480,
            0);
    if(!screen) {
        fprintf(stderr, "Could not create window\n");
        return 1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_SOFTWARE);
    if(!renderer) {
        fprintf(stderr, "Could not create renderer\n");
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(3000);

    SDL_DestroyWindow(screen);
    SDL_Quit();
    return 0;

    // Run Emulator, loop in run()
    run(psMem, psProc);
    
    // End Emulation
    return 0;
}

// run(memory, processor, peripheral)
int run(sMem* psMem, sProc* psProc) {
    uint8_t emuState = EMU_STATE_RUNNING;
    uint32_t emuCycle = EMU_DEBUG ? 0 : -1;
    
    while (emuState) {
        // Fetch Memory
        // Decode Proc
        decode(psMem, psProc, fetch(psMem, psProc));
        // Exe/Update Peripheral

        if (EMU_DEBUG)
        {
            emuCycle += 1;
            if (emuCycle >= EMU_DEBUG_CYCLE_CNT) { emuState = EMU_STATE_STOPPED; break; }
        }
    }

    if (emuState == EMU_STATE_STOPPED) {
        return E_EMU_SUCCESS;
    } 
    else {
        return E_EMU_STOP_FAIL;
    }
}
