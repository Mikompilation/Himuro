#ifndef OS_PAD_H
#define OS_PAD_H

#include "typedefs.h"
#include "main/glob.h"

extern u_short sce_pad[16];

int InitPad();
int PadSyncCallback();
int PadReadFunc(PAD_STRUCT *psp, int p_id);
void SetAnlgInfo(PAD_STRUCT *psp, int p_id);
u_short VibrateRequest(u_short p_id, u_short act1, u_short act2);
u_short VibrateRequest1(u_short p_id, u_short act_1);
u_short VibrateRequest2(u_short p_id, u_short act_2);
void PadClearCount(int p_id);

#endif // OS_PAD_H
