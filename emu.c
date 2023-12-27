#include "ref/emu.h"

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
    srand(time(NULL));

    // Open ROM file, get length
    pfROM = fopen(argv[1], "r");
    fseek(pfROM, 0, SEEK_END);
    romLen = ftell(pfROM);
    rewind(pfROM);

    // Create/Init Memory
    sMem* psMem = initMem(EMU_STACK_ADDR, EMU_ROM_ADDR, EMU_FRAMEBUF_ADDR);
    if (psMem == NULL)
    {
        printf("Chip | Memory failed to initalize.\n");
    }

    // Load ROM
    loadROM(psMem, EMU_ROM_ADDR, pfROM, romLen);
    fclose(pfROM);
    
    // Create/Init Processor
    sProc* psProc = initProc(EMU_STACK_ADDR, EMU_ROM_ADDR);
    if (psProc == NULL)
    {
        printf("Chip | Processor failed to initalize.\n");
    }
    
    // Setup SDL
    sPeriph* psPeriph = initScreen();
    if (psPeriph == NULL)
    {
        printf("Chip | Peripherals failed to initalize.\n");
    }
    
    // Run Intepreter
    run(psMem, psProc, psPeriph);
    
    // End Intepreter
    cleanupInternals(psMem, psProc);
    closeScreen(psPeriph);
    return 0;
}

int run(sMem* psMem, sProc* psProc, sPeriph* psPeriph) {
    uint8_t emuState = EMU_STATE_RUNNING;
    uint32_t emuCycle = 0;
    
    while (emuState) {
        if (EMU_DEBUG) { printf("Emu  | Cycle: %d\n", emuCycle); }

        if (psProc->pc > EMU_ROM_ADDR + psMem->textSecLen || psProc->pc < EMU_ROM_ADDR)
        {
            if (EMU_DEBUG) { printf("---- | \tPC outside of text section. Exiting...\n"); }
            emuState = EMU_STATE_STOPPED;
            continue;
        }
        
        // Fetch Instruction from Memory, Decode Instruction
        decode(psMem, psProc, fetch(psMem, psProc));

        // Update Peripheral
        updateScreen(psPeriph, psMem);
        
        // Increment Cycle
        emuCycle += 1;

        if (EMU_DEBUG && emuCycle >= EMU_DEBUG_CYCLE_CNT) { emuState = EMU_STATE_STOPPED; break; }
    }

    if (emuState == EMU_STATE_STOPPED) 
    {
        return E_EMU_SUCCESS;
    } 
    else 
    {
        return E_EMU_STOP_FAIL;
    }
}
