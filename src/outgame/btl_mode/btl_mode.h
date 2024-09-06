#ifndef OUTGAME_BTL_MODE_BTL_MODE_H
#define OUTGAME_BTL_MODE_BTL_MODE_H

#include "typedefs.h"

typedef struct {
	u_char mode;
	u_char csr[4];
	u_char stage_no;
	u_char load_mode;
	u_char load_count;
	u_char load_id;
	u_short time;
	u_short count;
} BTL_MODE_WRK;

typedef struct {
	u_char rank;
	u_short best_time;
	long int best_shot;
} STAGE_WRK;

// extern int rank_dat[0];
extern int battle_scr_effct[0];
// extern int btlBonus[0];
extern BTL_MODE_WRK btl_wrk;
extern STAGE_WRK stage_wrk[20];
extern long int msn_high_score;
extern long int msn_total_score;
extern long int msn_bonus;
// extern int stage_all_clear_bak;
// extern int btl_mode_bgm_init;
extern int btl_clear_disp;

void BattleModeStageInit();
void BattleModeStageMain();
void BattleModeInfomationDisp();
int CheckClearAllS();
void BattleModeClear();
void BattleModeScoreRecord(long int score);
int isBattleModeGame();
int CheckClearAllFirst();
int CheckClearAll();
void SetBattleEnd();

#ifdef BUILD_EU_VERSION
void SetBattleClear();
#endif

#endif // OUTGAME_BTL_MODE_BTL_MODE_H
