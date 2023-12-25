#define EMU_STATE_RUNNING 1
#define EMU_STATE_STOPPED 0

enum eEmuError {
	E_EMU_SUCCESS,
	E_MEM_FETCH_FAIL,
	E_PROC_DECODE_FAIL,
	E_PROC_EXECUTE_FAIL,
	E_EMU_STOP_FAIL
};
