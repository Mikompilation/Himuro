#ifndef OUTGAME_MODE_SLCT_H
#define OUTGAME_MODE_SLCT_H

#include "typedefs.h"

#include "graphics/graph2d/sprt.h"

typedef struct {
	u_char costume;
	u_char clear_info;
} CLEAR_BONUS;

typedef struct {
    u_long mode_tmr;
    u_char main_step;
    u_char mode_step;
    u_char chr_alp;
    u_char bak_alp;
    u_char win_alp;
    u_char flm_alp;
    u_char flm_lng;
    u_char pre_mode;
    u_char now_mode;
    u_char next_mode;
    u_char end_mode;
    u_char side;
    u_char opt_mode;
    u_char csr[3];
    u_char sm_slct[3];
    u_char bm_slct[2];
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    u_char unsel_col[4];
#endif
} DSP_M_SLCT_WRK;

extern SPRT_DAT msl_bg_btl[11];
extern SPRT_DAT msl_bg_fsm[11];
extern SPRT_DAT msl_bg_opt[11];
extern SPRT_DAT msl_bg_snd[11];
extern SPRT_DAT msl_bg_sty[11];
#if defined(BUILD_JP_VERSION)
extern SPRT_DAT msel_sprt[140];
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
extern SPRT_DAT msel_sprt[148];
#endif
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

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
void SetCsrInfoStoryMode();
#endif

#if defined(BUILD_EU_VERSION)
void SetCsrInfoBattleMode();
#endif

#endif // OUTGAME_MODE_SLCT_H
