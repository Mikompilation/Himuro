#ifndef GRAPHICS_MOTION_MDLACT_H
#define GRAPHICS_MOTION_MDLACT_H

#include "typedefs.h"

#include "graphics/graph3d/sg_dat.h"

typedef char (*plyr_act_func_t)(SgCOORDUNIT *cp);

typedef struct {
	float rot0;
	float rot1;
	float spd;
	u_short timer;
	u_short move_type;
} PLYR_FURI_DAT;

// extern plyr_act_func_t plyr_act_func[];
// extern PLYR_FURI_DAT plyr_act_furi0[];
// extern PLYR_FURI_DAT plyr_act_furi1[];
// extern PLYR_FURI_DAT plyr_act_furi2[];
// extern PLYR_FURI_DAT plyr_act_furi3[];

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
