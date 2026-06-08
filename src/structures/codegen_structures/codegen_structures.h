#ifndef NLC_CODEGEN_STRUCTURES_H
#define NLC_CODEGEN_STRUCTURES_H

enum REGISTER : unsigned int {
    EAX = 0, RAX = EAX,
    ECX = 1, RCX = ECX,
    EDX = 2, RDX = EDX,
    EBX = 3, RBX = EBX,
    ESP = 4, RSP = ESP,
    EBP = 5, RBP = EBP,
    ESI = 6, RSI = ESI,
    EDI = 7, RDI = EDI,

    R8  = 8,
    R9  = 9,
    R10 = 10,
    R11 = 11,
    R12 = 12,
    R13 = 13,
    R14 = 14,
    R15 = 15
};

enum REGISTER_8BIT : unsigned int {
    AL = 0,
    CL = 1,
    DL = 2,
    BL = 3,
    AH = 4,
    CH = 5,
    DH = 6,
    BH = 7
};

#endif
