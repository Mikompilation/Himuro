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

static inline float vu0Rand()
{
    float r;
    
    __asm__ volatile(
        "vrnext.x $vf12x,R\n"
        "vsubw.x $vf12x,$vf12x,$vf0w\n"
        "qmfc2   %0,$vf12\n"
        :"=r"(r)
    );

    return r;
}

extern SPRT_DAT effdat[]; // [60] = {...}
extern SPRT_DAT camdat[]; // [14] = {...}
extern int change_efbank; // = 0
extern int eff_blur_off; // = 0
extern int effect_disp_flg; // = 0
extern int eff_filament_off; // = 0
extern int now_buffer[2]; // = {0,0}
extern int eff_dith_off; // = 0
extern int stop_effects; // = 0
extern int monochrome_mode; // = 0
extern int magatoki_mode; // = 0
extern int shibata_set_off; // = 0
extern SBTSET msbtset; // = {0}
extern EFFECT_CONT efcnt[64]; // = {0}
extern EFFECT_CONT efcntm[48]; // = {0}
extern EFFECT_CONT efcnt_cnt[64]; // = {0}
extern EFFECT_CONT efcntm_cnt[48]; // = {0}
extern int look_debugmenu;
extern int set_buffer[2];

void InitEffects();
void InitEffectsEF();
void EffectEndSet();

#ifdef MATCHING_DECOMP
#ifdef INCLUDING_FROM_EFFECT_C
// fix for undeclared SetEffects in header
void* SetEffects(int id, int fl, ...);
#else
#define SetEffects(args...) (void *)SetEffects(args)
#endif
#else
void* SetEffects(int id, int fl, ...);
#endif

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
