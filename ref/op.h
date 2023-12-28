#ifndef CHIP_OP_H__
#define CHIP_OP_H__

#define PRE_OP_MULTI_0 0x0
    #define POST_OP_0_CALL_MACH 0x0
    // May want to check Pre op byte for 0 in lower bits.
    #define POST_OP_0_DISP_CLEAR 0x0E0
    #define POST_OP_0_RET 0x0EE

#define PRE_OP_JUMP 0x1
#define PRE_OP_CALL_SUB 0x2
#define PRE_OP_EQ_SKIP_CONS 0x3
#define PRE_OP_NEQ_SKIP_CONS 0x4
#define PRE_OP_EQ_SKIP_VAR 0x5
#define PRE_OP_ASSIGN_CONST 0x6
#define PRE_OP_ADD_CONST 0x7

#define PRE_OP_REG_OPER 0x8
    #define POST_OP_8_ASSIGN_REG 0x0
    #define POST_OP_8_BIT_OR 0x1
    #define POST_OP_8_BIT_AND 0x2
    #define POST_OP_8_BIT_XOR 0x3
    #define POST_OP_8_ADD_REG 0x4
    #define POST_OP_8_SUB_REG 0x5
    #define POST_OP_8_SHIFT_RIGHT 0x6
    #define POST_OP_8_REV_SUB 0x7
    #define POST_OP_8_SHIFT_LEFT 0xE

#define PRE_OP_NEQ_SKIP_VAR 0x9
#define PRE_OP_SET_MEMADDR 0xA
#define PRE_OP_REL_JUMP 0xB
#define PRE_OP_RAND 0xC
#define PRE_OP_DISP_DRAW 0xD

#define PRE_OP_KEYPRESS 0xE
    #define POST_OP_E_EQ_SKIP 0x9E
    #define POST_OP_E_NEQ_SKIP 0xA1

#define PRE_OP_MULTI_F 0xF
    #define POST_OP_F_GET_DELAY_TIMER 0x07
    #define POST_OP_F_KEYPRESS_BLOCK 0x0A
    #define POST_OP_F_SET_DELAY_TIMER 0x15
    #define POST_OP_F_SET_SOUND_TIMER 0x18
    #define POST_OP_F_MEM_ADD 0x1E
    #define POST_OP_F_SPRITE_ADDR 0x29
    #define POST_OP_F_STORE_BCD 0x33
    #define POST_OP_F_REG_DUMP 0x55
    #define POST_OP_F_REG_LOAD 0x65

#endif // CHIP_OP_H__
