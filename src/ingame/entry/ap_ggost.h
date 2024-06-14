#ifndef INGAME_ENTRY_AP_GGOST_H
#define INGAME_ENTRY_AP_GGOST_H

#include "typedefs.h"

// extern GGOST_DAT msn03ggst_dat;
// extern GGOST_DAT msn04ggst_dat;
// extern GGOST_DAT *ggst_dat[0];
// extern int gg_load_mode;
// extern int gg_room_bak;

int GuardGhostAppearSet();
void GuardGhostLoadInit();
int GuardGhostAppearMain();
int GuardGhostReloadReq();
void GuardGhostLoadReq();
int GuardGhostLoad();
int GuardGhostAppear();
void GuardGhostBattleSet(int room_no);
void GuardGhostBattleEnd();

#endif // INGAME_ENTRY_AP_GGOST_H
