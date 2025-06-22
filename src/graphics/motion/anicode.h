#ifndef GRAPHICS_MOTION_ANICODE_H
#define GRAPHICS_MOTION_ANICODE_H

#include "typedefs.h"

#include "graphics/motion/mdlwork.h"
#include "graphics/motion/mdldat.h"

void motAniCodeClearBuf(ANI_CTRL *ani_ctrl);
void motAniCodeSetBuf(ANI_CTRL *ani_ctrl, ANI_CODE code);
u_char motAniCodeRead(ANI_CTRL *ani_ctrl);
void motAniCodeExec(ANI_CTRL *ani_ctrl, ANI_CODE code, int *args);
void motAniTimerCodeExec(ANI_CTRL *ani_ctrl);
int motAniCodeIsEnd(ANI_CODE code);
void GetAniCodeArgs(ANI_CODE code, int *args);
void motAniCodeReadCTRL(ANI_CTRL *ani_ctrl, int *args);
void motAniCodeReadTIMER(ANI_CTRL *ani_ctrl, int *args);
void motAniCodeReadMOT(ANI_CTRL *ani_ctrl, int *args);
void motAniCodeReadMIM(ANI_CTRL *ani_ctrl, int *args);
void motAniCodeReadSE(ANI_CTRL *ani_ctrl, int *args);
void motAniCodeReadDOOR(ANI_CTRL *ani_ctrl, int *args);
void motAniCodeReadEFCT(ANI_CTRL *ani_ctrl, int *args);
u_char motGetNextMotion(ANI_CTRL *ani_ctrl);

#endif // GRAPHICS_MOTION_ANICODE_H
