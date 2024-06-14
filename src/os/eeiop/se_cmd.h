#ifndef OS_EEIOP_SE_CMD_H
#define OS_EEIOP_SE_CMD_H

#include "typedefs.h"

void SeCtrlInit();
void SeCtrlMain();
u_short SeCmdGetPan(float rot);
u_short SeCmdGetPhase(float rot);
u_short SeCmdGetPitch(float rot, u_short pitch);
u_short SeCmdGetVol(float dist, u_short vol);
void SeCmdSetVolForStts();
int SeCmdFdIn(int sw_no, u_short vol_tgt, short int vol_inc);
int SeCmdFdOut(int sw_no, short int vol_inc);
float SeCmdGetAngle(float rot0, float rot1);
int SeCmdSetPitch(int sw_no, u_short pitch);
u_short SeCmdGetDataPitch(int se_req_no);

#endif // OS_EEIOP_SE_CMD_H
