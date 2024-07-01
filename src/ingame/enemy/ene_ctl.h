#ifndef INGAME_ENEMY_ENE_CTL_H
#define INGAME_ENEMY_ENE_CTL_H

#include "typedefs.h"

typedef struct {
	u_int attr1;
	u_short dst_gthr;
	u_char way_gthr;
	u_char atk_ptn;
	u_char wspd;
	u_char rspd;
	u_short hp;
	u_short atk_rng;
	u_short hit_rng;
	u_short chance_rng;
	short int hit_adjx;
	u_short atk_p;
	u_short atk_h;
	u_char atk;
	u_char atk_tm;
	u_short mdl_no;
	u_short anm_no;
	u_int se_no;
	u_int adpcm_no;
	int dead_adpcm;
	u_short point_base;
	u_char hint_pic;
	u_char aura_alp;
	u_char area[6];
	u_short dir;
	u_short px;
	short int py;
	u_short pz;
} ENE_DAT;

typedef struct {
	u_char dat_no;
	u_char soul_no;
	u_short dir;
	u_short px;
	short int py;
	u_short pz;
	u_short adpcm_tm;
	int adpcm_no;
	u_short rng;
	u_short mdl_no;
	u_short anm_no;
	u_short point_base;
	u_int se_no;
	int se_foot;
} AENE_INFO_DAT;

typedef struct {
	u_short dmg;
	u_short hit_rng;
	u_short mdl_no;
	u_short cond;
} FLY_DATA;

// extern ENE_DAT jene_dat0[0];
// extern AENE_INFO_DAT aene_info_dat0[0];
// extern FLY_DATA fly_dat[0];
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
// extern ENE_DAT *jene_dat[0];
extern ENE_DAT *fene_dat[0];
// extern AENE_INFO_DAT *aene_info_dat[0];
// extern u_char req_dmg_ef[3];
// extern int ene_dead_load;
// extern int ene_dead_mode;
// extern int erootd0[20][3];
// extern int erootd1[20][3];
// extern int erootd2[20][3];
// extern u_char er_max_tbl[3];

#include "ingame/ig_glob.h"

void EneCtrlMain();
void EneEntryChk(u_char no);
void JEneEntry(u_char wrk_no, u_char dat_no);
void FEneEntry(u_char wrk_no, u_char dat_no);
void AEneEntry(u_char wrk_no, u_char dat_no);
void EneRule();
void EneCondCtrl();
void EneAuraCtrl();
void InitEneWrk(u_char wrk_no);
int EneWrkReleaseChk();
void FuyuEneRelease();
void EnePosInfoSet();
void EneSeOmenChk(u_char id);
void EneMoveCtrl();
u_char EnePRecogChkChk();
u_char EnePRecogChk(u_char *act_no);
u_char HitDmgChk();
u_char CaptureOnChk1();
u_char CaptureOnChk3();
u_char CaptureOnChk4();
u_char CaptureOnChk2();
u_char PlyrSeFootChk();
void EneActIniChk(u_char view_chk, u_char act_no);
void EneActIniSet(u_char act_no);
void EneActSet(u_char act_no);
void EneBlinkDataSet();
void EneARatioDataSet(u_char anime_no);
void EneActRule();
int EneActPreferChk(u_char *act_no);
void EneAniResolutionCtrl();
void EneAtkCtrl();
u_char EneDmgChk();
void ClrEneStaDmg();
u_char PlyrOutAreaChk();
u_char EneTrtryChk(u_char room_no);
void EneLightCtrl();
void EneInDispChk();
void EneBlinkPosSet();
void FlyRule(FLY_WRK *fw);
int FlyPlyrHitChk(float *tv);
void FlyAtkHit();
void EneMinDmgCtrl();
void EneNormalEffectCtrl();
char EndAutoEne(u_char type);
void ReqEneSpeakAdpcmCnt(u_short time);
void EneSpeakAdpcmCtrl();
void GhostDeadInit(int wrk_no);
void GhostDeadMain();
int GhostDeadLoadEndJudge();
int EnemyUseJudge(u_char ene_type);
int ZeroGhostBattleJudge();
void EneAdpcmPlayChk();

#endif // INGAME_ENEMY_ENE_CTL_H
