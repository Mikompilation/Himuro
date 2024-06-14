#ifndef OUTGAME_MODE_SLCT_H
#define OUTGAME_MODE_SLCT_H

#include "typedefs.h"

typedef struct {
	u_long tex0;
	u_short u;
	u_short v;
	u_short w;
	u_short h;
	int x;
	int y;
	int pri;
	u_char alpha;
} SPRT_DAT;

typedef struct {
	u_char costume;
	u_char clear_info;
} CLEAR_BONUS;

typedef struct
{ // 0x28
    /* 0x00 */ u_long mode_tmr;
    /* 0x08 */ u_char main_step;
    /* 0x09 */ u_char mode_step;
    /* 0x0a */ u_char chr_alp;
    /* 0x0b */ u_char bak_alp;
    /* 0x0c */ u_char win_alp;
    /* 0x0d */ u_char flm_alp;
    /* 0x0e */ u_char flm_lng;
    /* 0x0f */ u_char pre_mode;
    /* 0x10 */ u_char now_mode;
    /* 0x11 */ u_char next_mode;
    /* 0x12 */ u_char end_mode;
    /* 0x13 */ u_char side;
    /* 0x14 */ u_char opt_mode;
    /* 0x15 */ u_char csr[3];
    /* 0x18 */ u_char sm_slct[3];
    /* 0x1b */ u_char bm_slct[2];
    /* 0x1d */ u_char unsel_col[4];
} DSP_M_SLCT_WRK;

extern SPRT_DAT msl_bg_btl[11];
extern SPRT_DAT msl_bg_fsm[11];
extern SPRT_DAT msl_bg_opt[11];
extern SPRT_DAT msl_bg_snd[11];
extern SPRT_DAT msl_bg_sty[11];
extern SPRT_DAT msel_sprt[148];
extern int cmn_tex_load;
extern int title_bgm_no;
extern CLEAR_BONUS cribo;

void ModeSlctInit(u_char top, u_char end);
void ModeSlctLoop();
void ModeSlctCtrl(u_char mode);
char ModeSlctPad(u_char mode);
void ModeSlctDspBak(u_char alp, u_char mode);
void ModeSlctDspChr(u_char alp, u_char mode);
void DspModeSlect(u_char alp);
void DspStoryMode(u_char alp, float flsh);
void DspBattleMode(u_char alp, float flsh);
void DspMissionSelect(u_char alp);
void PutChrOne(u_short chr, short int px, short int py, u_char rgb, u_char alp, u_char flp);
void PutChrOneRGB(u_short chr, short int px, short int py, u_int rgb, u_char alp, u_char flp);
void ModeSlctDspFlm(u_char per, u_char alp, u_char mode);
void FilmCutter(u_short chr, short int px, short int py, short int ex, u_char rgb, u_char alp);
void ModeSlctDspWin(u_char alp);
void MsLoadCtrl(u_char mode);
void TitleModeEnd();
void ChangeModeSel(int mode);
void BattleModeExit();
void BattleModeNext();
int CheckClearStage();
void ModeSlctDspMsg(u_char alp, u_char mode);
void SetCsrInfoStoryMode();

#ifdef BUILD_EU_VERSION
void SetCsrInfoBattleMode();
#endif

#endif // OUTGAME_MODE_SLCT_H
