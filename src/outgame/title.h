#ifndef OUTGAME_TITLE_H
#define OUTGAME_TITLE_H

#include "typedefs.h"

#include "graphics/graph2d/sprt.h"

typedef struct {
	int load_id;
	u_char mode;
	u_char sub_mode;
	u_char next_mode;
	u_char csr;
	u_char load_side;
} TITLE_WRK;

typedef struct {
	float cnt;
	float alp;
	float spd;
	u_char alpmx;
	u_char colmx;
	u_short type;
} OUT_DITHER_STR;

typedef struct {
	u_short timer;
	u_char mode;
#ifdef BUILD_EU_VERSION
	u_char no_disp;
#endif
} TTL_DSP_WRK;

#ifdef BUILD_EU_VERSION
typedef struct {
	int logo_flow;
	int lang_sel_flow;
	int cnt;
	u_char alp;
	u_char dummy;
	u_short load_id;
} TITLE_SYS;
#endif

extern SPRT_DAT title_sprt[11];
#ifdef BUILD_EU_VERSION
extern SPRT_DAT font_sprt[23];
#else
extern SPRT_DAT font_sprt[20];
#endif;
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
