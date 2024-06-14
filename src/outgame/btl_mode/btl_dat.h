#ifndef OUTGAME_BTL_MODE_BTL_DAT_H
#define OUTGAME_BTL_MODE_BTL_DAT_H

#include "typedefs.h"

typedef struct {
	u_short time;
	u_short score;
	u_short rank[5];
	u_char clear;
	u_char film[2];
	u_char reiseki;
	u_char map_no;
	u_char room;
	u_char floor;
	u_char ene_no[3];
	u_char ene_ap[3];
	u_char rgst_no[5];
	u_short fly_mdl_no;
	short int prot;
	sceVu0FVECTOR ppos;
	sceVu0FVECTOR epos[3];
	sceVu0FVECTOR rgpos[5];
} STAGE_DAT;

extern STAGE_DAT stage_dat[0];


#endif // OUTGAME_BTL_MODE_BTL_DAT_H
