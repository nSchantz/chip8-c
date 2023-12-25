#include <stdint.h>
#include "ref/emu.h"

int main() {
    /**
    Setup:
        Settings.
        Create Proc, Create Memory
        Load ROM into memory.
        Setup SDL.

    Run Emulator:
        Fetch from memory.
        Decode in processor.
        Execute in processor.
    */
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
