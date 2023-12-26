#include "ref/emu.h"
#include "ref/proc.h"
#include "ref/mem.h"
#include <stdio.h>
#include <stdlib.h>

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

    // Run Emulator, loop in run()

    // End Emulation
    return 1;
}

// run(memory, processor, video)
int run() {
    uint8_t emuState = EMU_STATE_RUNNING;
    while (emuState) {
        // Fetch memory
        // Decode proc
        // Exe proc
    }

    if (emuState == EMU_STATE_STOPPED) {
        return E_EMU_SUCCESS;
    } 
    else {
        return E_EMU_STOP_FAIL;
    }
}
