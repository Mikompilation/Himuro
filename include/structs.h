#ifndef STRUCTS_H
#define STRUCTS_H

#include "typedefs.h"

typedef struct
{ // 0x8
    /* 0x0 */ u_char mode;
    /* 0x1 */ u_char csr[6];
    /* 0x7 */ u_char yn_mode;
} CAMERA_MENU_WRK;

typedef struct
{ // 0x4
    /* 0x0 */ short int p;
    /* 0x2 */ short int n;
} CSR_ADD;

typedef struct
{ // 0x20
    /* 0x00 */ sceVu0FVECTOR stq;
    /* 0x10 */ sceVu0FVECTOR vtw;
} SCRDEF;

#endif // STRUCTS_H