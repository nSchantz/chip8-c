#include "ref/emu.h"
#include "ref/proc.h"
#include "ref/mem.h"
#include <stdio.h>

int main(int argc, char* argv[]) {

    // Read Settings In
    printf("Read Settings In");

    // Create Proc, Create Memory
    sProc* psProc = initProc(EMU_STACK_ADDR, EMU_ROM_ADDR);
    if (psProc == NULL)
    {
        printf("Processor failed to initalize.\n");
    }

    sMem* psMem = initMem();
    if (psMem == NULL)
    {
        printf("Memory failed to initalize.\n");
    }

    // Setup SDL

    // Run Emulator
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
    else    {
        return E_EMU_STOP_FAIL;
    }
}
