#ifndef CHIP_OP_H__
#define CHIP_OP_H__

#define OP_00ZZ 0x0
    #define OP_0000 0x0
    // May want to check Pre op byte for 0 in lower bits.
    #define OP_00E0 0x0E0
    #define OP_00EE 0x0EE

#define OP_1000 0x1
#define OP_2000 0x2
#define OP_3000 0x3
#define OP_4000 0x4
#define OP_5000 0x5
#define OP_6000 0x6
#define OP_7000 0x7

#define OP_800Z 0x8
    #define OP_8000 0x0
    #define OP_8001 0x1
    #define OP_8002 0x2
    #define OP_8003 0x3
    #define OP_8004 0x4
    #define OP_8005 0x5
    #define OP_8006 0x6
    #define OP_8007 0x7
    #define OP_800E 0xE

#define OP_9000 0x9
#define OP_A000 0xA
#define OP_B000 0xB
#define OP_C000 0xC
#define OP_D000 0xD

#define OP_E0ZZ 0xE
    #define OP_E09E 0x9E
    #define OP_E0A1 0xA1

#define OP_F0ZZ 0xF
    #define OP_F007 0x07
    #define OP_F00A 0x0A
    #define OP_F015 0x15
    #define OP_F018 0x18
    #define OP_F01E 0x1E
    #define OP_F029 0x29
    #define OP_F033 0x33
    #define OP_F055 0x55
    #define OP_F065 0x65

#endif // CHIP_OP_H__
