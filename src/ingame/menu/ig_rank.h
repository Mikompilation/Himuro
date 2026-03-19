#ifndef INGAME_MENU_IG_RANK_H
#define INGAME_MENU_IG_RANK_H

#include "typedefs.h"

#include "ingame/menu/ig_types.h"

typedef struct {
    u_char mode;
    u_char csr_bak;
    u_char star;
    u_char cnt;
    char exist_s_rank;
} MENU_RANK;

typedef struct {
    u_short type;
    u_short id;
} SUBJECT_CHK;

extern SAVE_RANK save_rank;

void NewgameMenuRankInit();
void LoadgameMenuRankInit();
void StartRankModeInit();
void IngameMenuRank();
void IngameMenuRankDisp();
void RankingChkMem(PICTURE_WRK new_pic);
int CheckSamePic(PICTURE_WRK *newp);

#endif // INGAME_MENU_IG_RANK_H
