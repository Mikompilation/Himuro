#ifndef INGAME_MENU_IG_RANK_H
#define INGAME_MENU_IG_RANK_H

#include "typedefs.h"
#include "ig_types.h"

extern SAVE_RANK save_rank;

void NewgameMenuRankInit();
void LoadgameMenuRankInit();
void StartRankModeInit();
void IngameMenuRank();
void IngameMenuRankDisp();
void RankingChkMem(PICTURE_WRK new_pic);
int CheckSamePic(PICTURE_WRK *newp);

#endif // INGAME_MENU_IG_RANK_H
