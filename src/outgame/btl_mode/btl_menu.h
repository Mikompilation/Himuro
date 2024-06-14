#ifndef OUTGAME_BTL_MODE_BTL_MENU_H
#define OUTGAME_BTL_MODE_BTL_MENU_H

#include "typedefs.h"

// extern MSN_LOAD_DAT stage_load_dat0[0];
// extern MSN_LOAD_DAT stage_load_dat1[0];
// extern MSN_LOAD_DAT stage_load_dat2[0];
// extern MSN_LOAD_DAT stage_load_dat3[0];
// extern MSN_LOAD_DAT stage_load_dat4[0];
// extern MSN_LOAD_DAT *stage_load_dat[0];
// extern FREE_DAT free_dat[0];
// extern BTL_SAVE_STR btl_save_str[0];
// extern u_long btl_save_str_num;

void FreeModeMain();
void FreeModePosSet();
void BattleModeInit();
void ClearStageWrk();
void BattleModeMain();
int StageTitleInit();
int StageTitleLoad();
void StageGhostLoadReq();
void StageGhostLoadAfter();
void SaveStoryWrk();
void LoadStoryWrk();

#endif // OUTGAME_BTL_MODE_BTL_MENU_H
