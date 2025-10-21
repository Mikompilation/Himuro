#ifndef INGAME_ENTRY_AP_BGOST_H
#define INGAME_ENTRY_AP_BGOST_H

#include "typedefs.h"

typedef struct {
    u_char load_mode;
    u_char rel_mot;
    u_char bg_no;
    u_int mdl_adr;
    u_int mot_adr;
    u_int se_adr;
} BGST_WRK;

// extern BGST_WRK bgst_wrk;

void Mission03BindGhost01Delete();
void Mission04BindGhostLoad01Req();
void Mission04BindGhostLoad02Req();
int BindGhostBattleEnd(int wrk_no);
int BindGhostLoad();

#endif // INGAME_ENTRY_AP_BGOST_H
