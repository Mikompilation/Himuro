#ifndef INGAME_ENTRY_AP_ZGOST_H
#define INGAME_ENTRY_AP_ZGOST_H

#include "typedefs.h"

typedef struct { // 0x30
	/* 0x00 */ sceVu0FVECTOR pos;
	/* 0x10 */ sceVu0FVECTOR rot;
	/* 0x20 */ u_char room;
	/* 0x21 */ u_char floor;
	/* 0x22 */ u_char zh_mdl_bak[2];
	/* 0x24 */ u_char zh_mot_bak[2];
	/* 0x26 */ u_char zh_mmt_bak[2];
	/* 0x28 */ u_short time;
} ZHOUR_WRK;

// extern ZHOUR_WRK zh_wrk;

int ZeroHourAppearMain();
void ZeroHourDataBackUp();
void ZeroHourEnemyReLoad();
void ZeroHourEnemyReLoadAfter();
void ZeroHourAfterPosReset();
void ZeroHourOutReq();
void ZeroHourAfterRoomLoadReq();
int ZeroHourAppearJudge();

#endif // INGAME_ENTRY_AP_ZGOST_H
