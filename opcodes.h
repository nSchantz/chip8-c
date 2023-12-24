#define PRE_OP_MULTI_0 0x0
#define POST_OP_CALL_MACH 0x0
// May want to check Pre op byte for 0 in lower bits.
#define POST_OP_DISP_CLEAR 0xE0
#define POST_OP_RET 0xEE

#define PRE_OP_JUMP 0x1
#define PRE_OP_CALL_SUB 0x2
#define PRE_OP_EQ_SKIP_CONS 0x3
#define PRE_OP_NEQ_SKIP_CONS 0x4
#define PRE_OP_EQ_SKIP_VAR 0x5
#define PRE_OP_ASSIGN_CONST 0x6
#define PRE_OP_ADD_CONST 0x7
#define PRE_OP_REG_OPER 0x8
#define PRE_OP_NEQ_SKIP_VAR 0x9
#define PRE_OP_SET_MEMADDR 0xA
#define PRE_OP_REL_JUMP 0xB
#define PRE_OP_RAND 0xC
#define PRE_OP_DISP_DRAW 0xD

#define PRE_OP_KEYPRESS 0xE
#define POST_OP_EQ_SKIP 0x9E
#define POST_OP_NEQ_SKIP 0xA1
