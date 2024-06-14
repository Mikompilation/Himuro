#ifndef GRAPHICS_GRAPH2D_EFFECT_H
#define GRAPHICS_GRAPH2D_EFFECT_H

#include "typedefs.h"

#include "graphics/graph2d/tim2.h"

typedef struct {
	Q_WORDDATA dat;
	void *pnt[6];
	float fw[3];
	u_int z;
	u_int flow;
	u_int cnt;
	u_int in;
	u_int keep;
	u_int out;
	u_int max;
} EFFECT_CONT;

typedef struct {
	u_char sw;
	u_char alp;
	int scl;
	int rot;
	float x;
	float y;
} BLUR_STR;

typedef struct {
	u_char sw;
	u_char type;
	u_char col;
	u_char alp;
} CONTRAST_STR;

typedef struct {
	u_char sw;
	u_char alp;
} FFRAME_STR;

typedef struct {
	u_char sw;
	u_char type;
	float spd;
	float alp;
	u_char amax;
	u_char cmax;
} DITHER_STR;

typedef struct {
	u_char sw;
	u_char type;
	u_char rate;
} DEFORM_STR;

typedef struct {
	u_char sw;
	u_char col;
	u_char alp;
	u_char alp2;
} NEGA_STR;

typedef struct {
	u_char sw;
} MONO_STR;

typedef struct {
	BLUR_STR bl;
	CONTRAST_STR cn;
	FFRAME_STR ff;
	DITHER_STR dt;
	DEFORM_STR df;
	NEGA_STR ng;
	MONO_STR mn;
} SBTSET;

// extern SPRT_DAT effdat[0];
// extern SPRT_DAT camdat[0];
// extern int change_efbank;
// extern int eff_blur_off;
// extern int effect_disp_flg;
// extern int eff_filament_off;
// extern int now_buffer[2];
// extern int eff_dith_off;
// extern int stop_effects;
// extern int monochrome_mode;
// extern int magatoki_mode;
// extern int shibata_set_off;
// extern SBTSET msbtset;
// extern EFFECT_CONT efcnt[64];
// extern EFFECT_CONT efcntm[48];
// extern EFFECT_CONT efcnt_cnt[64];
// extern EFFECT_CONT efcntm_cnt[48];
// extern int look_debugmenu;

void InitEffects();
void InitEffectsEF();
void EffectEndSet();
void* SetEffects(int id, int fl);
void ResetEffects(void *p);
int SearchEmptyEffectBuf();
void EffectZSort();
void EffectZSort2();
void EffectZSort3();
int CheckEffectScrBuffer(int eno);
void ResetEffectScrBuffer(int eno);
void EffectControl(int no);
void SetBlurOff();
void SetDebugMenuSwitch(int sw);
void pblur();
void TmpEffectOff(int id);
void tes_p10();
void tes_p11();
void tes_p20();
void tes_p21();
void tes_p3();

#endif // GRAPHICS_GRAPH2D_EFFECT_H
