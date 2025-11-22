#ifndef OS_PAD_H
#define OS_PAD_H

#include "typedefs.h"
#include "pad_types.h"

extern u_short sce_pad[16];

int InitPad();
int PadSyncCallback();
int PadReadFunc(PAD_STRUCT *psp, int p_id);
void SetAnlgInfo(PAD_STRUCT *psp, int p_id);
// u_short VibrateRequest(u_short p_id, u_short act1, u_short act2);
// u_short VibrateRequest1(u_short p_id, u_short act_1);
// u_short VibrateRequest2(u_short p_id, u_short act_2);
void PadClearCount(int p_id);

#ifdef MATCHING_DECOMP
#ifdef INCLUDING_FROM_PAD_C
// fix for undeclared VibrateRequest in header
u_short VibrateRequest(u_short p_id, u_short act1, u_short act2);
#endif
#else
u_short VibrateRequest(u_short p_id, u_short act1, u_short act2);
#endif

#endif // OS_PAD_H
