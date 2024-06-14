#ifndef GRAPHICS_GRAPH3D_SCE_PP3_H
#define GRAPHICS_GRAPH3D_SCE_PP3_H

#include "typedefs.h"


void scePP13_Sync();
void scePP1_Kick(void *addr);
void scePP3_Kick(u_int *addr, int qwc);
int scePP1_Disp(u_int mode);
int scePP1_Init(u_int mode);
int scePP1_End(u_int mode);

#endif // GRAPHICS_GRAPH3D_SCE_PP3_H
