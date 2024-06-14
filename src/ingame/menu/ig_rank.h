#ifndef INGAME_MENU_IG_RANK_H
#define INGAME_MENU_IG_RANK_H

#include "typedefs.h"
#include "os/system.h"

typedef struct {
	u_char adr_no;
	u_char msn_no;
	u_char room;
	u_char status;
	u_short subject[3][2];
	u_int score;
	sceCdCLOCK time;
} PICTURE_WRK;

typedef struct {
	PICTURE_WRK pic_inf[10];
	u_short pic_num;
	u_short best_score;
} SAVE_RANK;

// extern SAVE_RANK save_rank;

void NewgameMenuRankInit();
void LoadgameMenuRankInit();
void StartRankModeInit();
void IngameMenuRank();
void IngameMenuRankDisp();
void RankingChkMem(PICTURE_WRK new_pic);
int CheckSamePic(PICTURE_WRK *newp);

#endif // INGAME_MENU_IG_RANK_H
