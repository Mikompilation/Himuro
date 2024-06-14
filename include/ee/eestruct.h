#ifndef EE_EESTRUCT_H
#define EE_EESTRUCT_H

#include "typedefs.h"

typedef struct {
    unsigned long NLOOP:15;
    unsigned long EOP:1;
    unsigned long pad16:16;
    unsigned long id:14;
    unsigned long PRE:1;
    unsigned long PRIM:11;
    unsigned long FLG:2;
    unsigned long NREG:4;
    unsigned long REGS0:4;
    unsigned long REGS1:4;
    unsigned long REGS2:4;
    unsigned long REGS3:4;
    unsigned long REGS4:4;
    unsigned long REGS5:4;
    unsigned long REGS6:4;
    unsigned long REGS7:4;
    unsigned long REGS8:4;
    unsigned long REGS9:4;
    unsigned long REGS10:4;
    unsigned long REGS11:4;
    unsigned long REGS12:4;
    unsigned long REGS13:4;
    unsigned long REGS14:4;
    unsigned long REGS15:4;
} sceGifTag __attribute__((aligned(16)));

typedef struct {
    unsigned long CLAMP:1;
    unsigned long pad01:63;
} sceGsColclamp;

typedef struct {
    unsigned long DTHE:1;
    unsigned long pad01:63;
} sceGsDthe;

typedef struct {
    unsigned long FBP:9;
    unsigned long pad09:7;
    unsigned long FBW:6;
    unsigned long pad22:2;
    unsigned long PSM:6;
    unsigned long pad30:2;
    unsigned long FBMSK:32;
} sceGsFrame;

typedef struct {
    unsigned long AC:1;
    unsigned long pad01:63;
} sceGsPrmodecont;

typedef struct {
    unsigned long SCAX0:11;
    unsigned long pad11:5;
    unsigned long SCAX1:11;
    unsigned long pad27:5;
    unsigned long SCAY0:11;
    unsigned long pad43:5;
    unsigned long SCAY1:11;
    unsigned long pad59:5;
} sceGsScissor;

typedef struct {
    unsigned long ATE:1;
    unsigned long ATST:3;
    unsigned long AREF:8;
    unsigned long AFAIL:2;
    unsigned long DATE:1;
    unsigned long DATM:1;
    unsigned long ZTE:1;
    unsigned long ZTST:2;
    unsigned long pad19:45;
} sceGsTest;

typedef struct {
    unsigned long OFX:16;
    unsigned long pad16:16;
    unsigned long OFY:16;
    unsigned long pad48:16;
} sceGsXyoffset;

typedef struct {
    unsigned long ZBP:9;
    unsigned long pad09:15;
    unsigned long PSM:4;
    unsigned long pad28:4;
    unsigned long ZMSK:1;
    unsigned long pad33:31;
} sceGsZbuf;

typedef struct {
    unsigned long PRIM:3;
    unsigned long IIP:1;
    unsigned long TME:1;
    unsigned long FGE:1;
    unsigned long ABE:1;
    unsigned long AA1:1;
    unsigned long FST:1;
    unsigned long CTXT:1;
    unsigned long FIX:1;
    unsigned long pad11:53;
} sceGsPrim;

typedef struct {
    u_int R:8;
    u_int G:8;
    u_int B:8;
    u_int A:8;
    float Q;
} sceGsRgbaq;

typedef struct {
    unsigned long X:16;
    unsigned long Y:16;
    unsigned long Z:32;
} sceGsXyz;

#endif // EE_EESTRUCT_H