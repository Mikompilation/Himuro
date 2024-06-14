#ifndef OUTGAME_TITLE_H
#define OUTGAME_TITLE_H

#include "typedefs.h"

typedef struct {
	int load_id;
	u_char mode;
	u_char sub_mode;
	u_char next_mode;
	u_char csr;
	u_char load_side;
} TITLE_WRK;

#include "outgame/mode_slct.h"

// extern SPRT_DAT title_sprt[11];
// extern SPRT_DAT font_sprt[20];
extern int opening_movie_type;
extern TITLE_WRK title_wrk;

void TitleCtrl();
void TitleWaitMode();
void TitleStartSlct();
void TitleStartSlctYW(u_char pad_off, u_char alp_max);
void TitleLoadCtrl();
void TitleSelectMode();
void TitleSelectModeYW(u_char pad_off, u_char alp_max);
void TitleSelectDifficultyYW();
void NewGameInit();
void LoadGameInit();
void InitOutDither();
void MakeOutDither();
void DispOutDither();

#ifdef BUILD_EU_VERSION
void InitTecmotLogo();
int SetTecmoLogo();
void InitSelectLanguage();
int SetSelectLanguage(int cur_pos);
#endif

#endif // OUTGAME_TITLE_H
