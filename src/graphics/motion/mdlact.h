#ifndef GRAPHICS_MOTION_MDLACT_H
#define GRAPHICS_MOTION_MDLACT_H

#include "typedefs.h"

#include "graphics/motion/accessory.h"

// extern char (*plyr_act_func[0])(/* parameters unknown */);
// extern PLYR_FURI_DAT plyr_act_furi0[0];
// extern PLYR_FURI_DAT plyr_act_furi1[0];
// extern PLYR_FURI_DAT plyr_act_furi2[0];
// extern PLYR_FURI_DAT plyr_act_furi3[0];

char motPlayerActCtrl(SgCOORDUNIT *cp);
void motPlyrActExec(SgCOORDUNIT *cp);
void motInitPlyrAct();
void motPlyrActReq(char act_no, sceVu0FVECTOR *pos, u_short cnt);
void motPlyrActStop();
char motPlyrActLookAt(SgCOORDUNIT *cp);
char motPlyrActSurprise(SgCOORDUNIT *cp);
char motPlyrActSowasowa(SgCOORDUNIT *cp);
char motPlyrActKyoro(SgCOORDUNIT *cp);
void motGetPlyrNeckRot(SgCOORDUNIT *cp, float *trot, float *tpos);
void InitQuake();
void StopQuake();
void ReqQuake(u_int furn_id, float pow, u_int time, u_int loop, u_int stop_flg);
char QuakeCamera();
float motGetRandom(float upper, float lower);
float motLinearSupValue(float moto, float saki, u_char mode, u_int cnt, u_int all_cnt);

#endif // GRAPHICS_MOTION_MDLACT_H
