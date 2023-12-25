#include <stdint.h>

typedef struct sProc {
    uint8_t reg[16];
    uint8_t sp;
    uint8_t sndTimer;
    uint8_t delTimer;
    uint16_t ind;
    uint16_t pc;
} sProc;

int decode(sProc* psProc, uint16_t ins);
