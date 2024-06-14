#ifndef INGAME_ENTRY_AP_DGOST_H
#define INGAME_ENTRY_AP_DGOST_H

#include "typedefs.h"

// extern u_char dgst_room[0];
// extern MSN_LOAD_DAT dgst_load[3];
// extern DGST_WRK dgst_wrk;

void DeadGhostAppearReq();
void DeadGhostAppearMain();
int DeadGhostAppearJudge();
void DeadGhostLoadReq();
int DeadGhostLoad();
void DeadGhostBattleEnd();
int EnemyNoDeadGhostJudge(int ene_no);
void DeadGhostSetLoad();
void DeadGhostLoadDel();

#endif // INGAME_ENTRY_AP_DGOST_H
