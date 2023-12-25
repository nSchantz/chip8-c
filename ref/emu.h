#ifndef CHIP_EMU_H__
#define CHIP_EMU_H__

#include <stdint.h>

#define EMU_STATE_RUNNING 1
#define EMU_STATE_STOPPED 0

#define EMU_STACK_ADDR 0x999
#define EMU_ROM_ADDR 0x100

enum eEmuError {
	E_EMU_SUCCESS,
	E_MEM_FETCH_FAIL,
	E_PROC_DECODE_FAIL,
	E_PROC_EXECUTE_FAIL,
	E_EMU_STOP_FAIL
};

#endif // CHIP_EMU_H__
