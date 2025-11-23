#ifndef INGAME_ENTRY_AP_GGOST_H
#define INGAME_ENTRY_AP_GGOST_H

#include "typedefs.h"

typedef struct { // 0x28
	/* 0x00 */ u_char ggst[5];
	/* 0x05 */ u_char room[5];
	/* 0x0a */ short int pos[5][3];
} GGOST_DAT;

// extern GGOST_DAT msn03ggst_dat;
// extern GGOST_DAT msn04ggst_dat;
// extern GGOST_DAT *ggst_dat[];
extern int gg_load_mode;
extern int gg_room_bak;

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
