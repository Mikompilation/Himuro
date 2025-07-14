#ifndef SCE_DEVVIF1_H
#define SCE_DEVVIF1_H

#include "typedefs.h"

typedef struct {
    u_int row[4];
    u_int col[4];
    u_int mask;
    u_int code;
    u_int stat;
    u_short itop;
    u_short itops;
    u_short base;
    u_short offset;
    u_short top;
    u_short tops;
    u_short mark;
    u_short num;
    u_char error;
    u_char cl;
    u_char wl;
    u_char cmod;
    u_char pad;
} sceDevVif1Cnd;

void sceDevVif1Reset(void);
int sceDevVif1Pause(int mode);
int sceDevVif1Continue(void);
u_int sceDevVif1PutErr(int interrupt, int miss1, int miss2);
u_int sceDevVif1GetErr(void);
int sceDevVif1GetCnd(sceDevVif1Cnd *);
int sceDevVif1PutFifo(u_long128 *addr, int n);
int sceDevVif1GetFifo(u_long128 *addr, int n);

#endif // SCE_DEVVIF1_H
