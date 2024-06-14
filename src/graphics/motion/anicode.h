#ifndef GRAPHICS_MOTION_ANICODE_H
#define GRAPHICS_MOTION_ANICODE_H

#include "typedefs.h"

#include "graphics/motion/mdlwork.h"
#include "graphics/motion/mdldat.h"

void motAniCodeClearBuf(ANI_CTRL *ani_ctrl);
void motAniCodeSetBuf(ANI_CODE code);
u_char motAniCodeRead();
void motAniCodeExec(ANI_CODE code, int *args);
void motAniTimerCodeExec();
int motAniCodeIsEnd(ANI_CODE code);
void GetAniCodeArgs(ANI_CODE code, int *args);
void motAniCodeReadCTRL(int *args);
void motAniCodeReadTIMER(int *args);
void motAniCodeReadMOT(int *args);
void motAniCodeReadMIM(int *args);
void motAniCodeReadSE(int *args);
void motAniCodeReadDOOR(int *args);
void motAniCodeReadEFCT(int *args);
u_char motGetNextMotion();

#endif // GRAPHICS_MOTION_ANICODE_H
