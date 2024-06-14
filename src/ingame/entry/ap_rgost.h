#ifndef INGAME_ENTRY_AP_RGOST_H
#define INGAME_ENTRY_AP_RGOST_H

#include "typedefs.h"

typedef struct {
	u_char mode;
	u_char rg_no;
	u_char dsp_no;
	u_char alpha;
	u_char alp_bak;
	u_char se_wrk_no;
	u_short count;
	u_short time;
	sceVu0FVECTOR pos;
} RGOST_DSP_WRK;

// extern RGOST_DSP_WRK rg_dsp_wrk[3];
// extern RGOST_WRK rg_wrk[200];

void RareGhostInit();
void RareGhostEntrySet();
int RareGhostLoadReq();
int RareGhostLoadGameLoadReq();
void RareGhostMain();
void RareGhostAppearCtrl();
void RareGhostDispCtrl();
int RareGhostDispStartJudge(int rg_no);
int RareGhostDispEndJudge(int wrk_no);
void RareGhostDispTimeCtrl(int wrk_no, int rg_no);
void RareGhostDispEndSet(int wrk_no);
void RareGhostDispWrkSet(int wrk_no, int rg_no);
void SetRareGhostDispAlpha(int wrk_no, int rg_no);
void SetRareGhostDispAnimation(int wrk_no, int rg_no);
float SetRareGhostDispRate(int wrk_no);

#endif // INGAME_ENTRY_AP_RGOST_H
