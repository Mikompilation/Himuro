#ifndef INGAME_ENTRY_AP_ZGOST_H
#define INGAME_ENTRY_AP_ZGOST_H

#include "typedefs.h"

typedef struct {
    sceVu0FVECTOR pos;
    sceVu0FVECTOR rot;
    u_char room;
    u_char floor;
    u_char zh_mdl_bak[2];
    u_char zh_mot_bak[2];
    u_char zh_mmt_bak[2];
    u_short time;
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
