#ifndef CHIP_EMU_H__
#define CHIP_EMU_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "internals.h"
#include "peripheral.h"

#define EMU_STATE_RUNNING 1
#define EMU_STATE_STOPPED 0

#define EMU_STACK_ADDR 0xFFF
#define EMU_FRAMEBUF_ADDR 0x100 // Framebuffer is 0x100-0x200 (8 bytes per row (64 bits / pixels), 32 rows: 32 * 8 = 256 bytes total)
#define EMU_FONT_ADDR 0x000
#define EMU_ROM_ADDR 0x200

#define EMU_REFRESH_RATE 60

#define EMU_DEBUG 1
#define EMU_DEBUG_CYCLE_CNT 20

enum eEmuError {
	E_EMU_SUCCESS,
	E_MEM_FETCH_FAIL,
	E_PROC_DECODE_FAIL,
	E_PROC_EXECUTE_FAIL,
	E_EMU_STOP_FAIL
};

int run(sMem* psMem, sProc* psProc, sPeriph* psPeriph);

#endif // CHIP_EMU_H__
