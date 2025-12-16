#ifndef INGAME_ENEMY_ENE_CTL_H
#define INGAME_ENEMY_ENE_CTL_H

#include "typedefs.h"
#include "ene_types.h"
#include "main/glob_types.h"
#include "ingame/ig_glob_types.h"

// extern ENE_DAT jene_dat0[0];
// extern AENE_INFO_DAT aene_info_dat0[0];
extern FLY_DATA fly_dat[];
// extern ENE_DAT jene_dat1[0];
// extern ENE_DAT fene_dat1[0];
// extern AENE_INFO_DAT aene_info_dat1[0];
// extern ENE_DAT jene_dat2[0];
// extern ENE_DAT fene_dat2[0];
// extern AENE_INFO_DAT aene_info_dat2[0];
// extern ENE_DAT jene_dat3[0];
// extern ENE_DAT fene_dat3[0];
// extern AENE_INFO_DAT aene_info_dat3[0];
// extern ENE_DAT jene_dat4[0];
// extern ENE_DAT fene_dat4[0];
// extern AENE_INFO_DAT aene_info_dat4[0];
extern ENE_DAT *jene_dat[];
extern ENE_DAT *fene_dat[];
// extern AENE_INFO_DAT *aene_info_dat[0];
extern u_char req_dmg_ef[3];
extern int ene_dead_load;
// extern int ene_dead_mode;
extern int erootd0[20][3];
extern int erootd1[20][3];
extern int erootd2[20][3];

void EneCtrlMain();
void EneEntryChk(u_char no);
void JEneEntry(u_char wrk_no, u_char dat_no);
void FEneEntry(u_char wrk_no, u_char dat_no);
void AEneEntry(u_char wrk_no, u_char dat_no);
void EneRule(ENE_WRK *ew);
void EneCondCtrl(ENE_WRK *ew);
void EneAuraCtrl(ENE_WRK *ew);
void InitEneWrk(u_char wrk_no);
int EneWrkReleaseChk(ENE_WRK *ew);
void FuyuEneRelease();
void EnePosInfoSet(ENE_WRK *ew);
void EneSeOmenChk(ENE_WRK *ew, u_char id);
void EneMoveCtrl(ENE_WRK *ew);
u_char EnePRecogChkChk(ENE_WRK *ew);
u_char EnePRecogChk(ENE_WRK *ew, u_char *act_no);
u_char HitDmgChk(ENE_WRK *ew);
u_char CaptureOnChk1(ENE_WRK *ew);
u_char CaptureOnChk3(ENE_WRK *ew);
u_char CaptureOnChk4(ENE_WRK *ew);
u_char CaptureOnChk2(ENE_WRK *ew);
u_char PlyrSeFootChk(ENE_WRK *ew);
void EneActIniChk(ENE_WRK *ew, u_char view_chk, u_char act_no);
void EneActIniSet(ENE_WRK *ew, u_char act_no);
void EneActSet(ENE_WRK *ew, u_char act_no);
void EneBlinkDataSet(ENE_WRK *ew);
void EneARatioDataSet(ENE_WRK *ew, u_char anime_no);
void EneActRule(ENE_WRK *ew);
int EneActPreferChk(ENE_WRK *ew, u_char *act_no);
void EneAniResolutionCtrl(ENE_WRK *ew);
void EneAtkCtrl(ENE_WRK *ew);
u_char EneDmgChk(ENE_WRK *ew);
void ClrEneStaDmg(ENE_WRK *ew);
u_char PlyrOutAreaChk(ENE_WRK *ew);
u_char EneTrtryChk(ENE_WRK *ew, u_char room_no);
void EneLightCtrl(ENE_WRK *ew);
void EneInDispChk(ENE_WRK *ew);
void EneBlinkPosSet(ENE_WRK *ew);
void FlyRule(FLY_WRK *fw);
int FlyPlyrHitChk(FLY_WRK *fw, sceVu0FVECTOR tv);
void FlyAtkHit(FLY_WRK *fw);
void EneMinDmgCtrl(ENE_WRK *ew);
void EneNormalEffectCtrl(ENE_WRK *ew);
char EndAutoEne(u_char type);
void ReqEneSpeakAdpcmCnt(u_short time);
void EneSpeakAdpcmCtrl(ENE_WRK *ew);
void GhostDeadInit(int wrk_no);
void GhostDeadMain();
int GhostDeadLoadEndJudge();
int EnemyUseJudge(u_char ene_type);
int ZeroGhostBattleJudge();
void EneAdpcmPlayChk(ENE_WRK *ew);

#endif // INGAME_ENEMY_ENE_CTL_H
