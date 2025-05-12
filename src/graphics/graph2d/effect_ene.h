#ifndef GRAPHICS_GRAPH2D_EFFECT_ENE_H
#define GRAPHICS_GRAPH2D_EFFECT_ENE_H

#include "typedefs.h"

typedef struct {
	sceVu0FVECTOR npos;
	sceVu0FVECTOR *oposp;
	sceVu0FMATRIX *wmtxp;
	int time;
	short int top;
	short int num;
	float xp;
	float xm;
	float rot;
	float rotp;
	float x;
	float y;
	float n;
	float cnt;
} NEW_PERTICLE;

typedef struct {
	sceVu0FMATRIX wmtx[12];
	sceVu0FVECTOR opos[12];
} TAIL_DMG2_DAT;

typedef struct {
	sceVu0FVECTOR wbpos[2];
	float scw[4];
	float sch[4];
	float rot_z[4];
	float alp[4];
	int cnt[4];
	int flow[4];
	int enedmg_no;
	int enedmg1_flg;
	int enedmg_chance;
	int dummy;
} ENDMG1;

typedef struct {
	int enedmg_no;
	int enedmg2_flg;
	int enedmg_chance;
} ENDMG2;

typedef struct { // 0x8
	/* 0x0 */ u_char mdl_no;
	/* 0x2 */ u_short tex_no;
	/* 0x4 */ int addr;
} EDMG_TEX_WRK;

typedef struct {
	int sw;
	int num;
	int top;
	int dummy;
} SWORD_LINE;

typedef struct {
	int cam_search_flow;
	u_char walp;
	int top;
	int num;
	float alp;
	float cnt;
	sceVu0FVECTOR cpos[30];
} CAMSEARCH_STR;

typedef struct {
	int cam_view_flow;
	u_char alpn;
	int cnt;
	float alpc;
	float rot_z;
	u_short ligtime[3];
	float rrad[48];
	float rrot[48];
	float racc[48];
	u_char ralp[48];
} CAMVIEW_STR;

typedef struct {
	int cam_push_flow;
	u_char alpn;
	int cnt;
	float alpc;
	float rot_z;
	float size;
	sceVu0FVECTOR rpos[64];
	float rrad[64];
	float rrot[64];
	float racc[64];
	float ralp[64];
} CAMPUSH_STR;

typedef struct {
	int cam_slow_flow;
	float pos_y[3];
	float rot_xx[3];
	float rot_yy[3];
	float rot_zz[3];
	float hrt;
	float rrt;
	int cnt;
	u_short ligtime[3];
	sceVu0FVECTOR rpos[32];
	float rrad[32];
	float rrot[32];
	float racc[32];
	float ralp[32];
	float rlif[32];
	int fl;
	sceVu0FVECTOR ncf[2][33];
} CAMSLOW_STR;

typedef struct {
	int cam_stop_flow;
	int flow2;
	int cnt;
	u_short ligtime[3];
	float rot_z;
	float size[3];
	u_char alp[3];
	float rrad[32];
	float rrotx[32];
	float rroty[32];
	float racc[32];
	float rbrk[32];
	u_char rscl[32];
} CAMSTOP_STR;

extern u_char sr1;
extern u_char sg1;
extern u_char sb1;
extern u_char sa1;
extern u_char sr2;
extern u_char sg2;
extern u_char sb2;
extern u_char sa2;
extern int SWORDTYPE;
extern float rac1;
extern float rac2;
extern float rbre;
extern u_char rrr;
extern u_char ggg;
extern u_char bbb;
extern float enedmg2_sp;
extern ENDMG1 enedmg1[3];
extern ENDMG2 enedmg2;
extern int eneseal_status;
extern int enedmg_status;
extern SWORD_LINE sw_line;
extern int camsearch_miss;
extern CAMSEARCH_STR camsearch[3];
extern int camview_miss;
extern CAMVIEW_STR camview[3];
extern int campush_miss;
extern CAMPUSH_STR campush[3];
extern int camslow_miss;
extern CAMSLOW_STR camslow[3];
extern int camstop_miss;
extern CAMSTOP_STR camstop[3];
extern sceVu0FVECTOR sword_line[2];
extern EDMG_TEX_WRK edmg_tex_wrk[10];
extern TAIL_DMG2_DAT enedmg2_tail[44];

#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/effect.h"

void InitEffectEne();
void InitEffectEneEF();
void SetETOCircleTexure(/* t8 24 */ sceVu0FMATRIX wlm, DRAW_ENV *de, float w, float h, u_char r, u_char g, u_char b, u_char a);
int SetCamFont(int no, int fl);
int SetEFLight(int flg, int eneno, int ligno, int in, int keep, int out);
void IniCamSearch();
void SetCamSearch(int eneno);
void RunCamSearch();
void IniCamView();
void SetCamView(int eneno);
void RunCamViewP(int o, int fl, float *cpos, float ap);
void RunCamView();
void IniCamPush();
void SetCamPush(int eneno);
void RunCamPushP(int o, int fl, float *cpos, float ap);
void RunCamPush();
void RunCamSlow2(int o, float hrt, float rrt, u_int alp);
void IniCamSlow();
void SetCamSlow(int eneno);
void RunCamSlow();
void IniCamStop();
void SetCamStop(int eneno);
void RunCamStop();
int CheckCamSubEffect();
void SetSwordLineSub(void *pos, int num, u_char r1, u_char g1, u_char b1, u_char r2, u_char g2, u_char b2);
void SetSwordSwitch(int sw);
void SetSwordLine();
int SetSpiritAway(int fl, float *tpos);
int SetNewEneOut(int flag, u_char eneno, u_char type, float *bpos, float sc);
void SetEneDmgEffect3_Sub2(NEW_PERTICLE *np, float *bpos1, float *bpos2, u_char r1, u_char g1, u_char b1, u_char r2, u_char g2, int b2);
int SetEneDmgEffect3(int flag, u_char eneno, sceVu0FVECTOR *pos);
void SetEneSeal(EFFECT_CONT *ec);
void EneDmgTexInit();
void LoadEneDmgTex(int no, u_int *addr);
void SetEneDmgTex(int wrk_no);
void EneDmgTexDel(int wrk_no);
void ZeroHourEneDmgTexDel(int mdl_no);
void SetEneDmgEffect1_Sub2(int num);
void SetEneDmgEffect1_Sub();
void SetEneDmgEffect1(int num);
void SetEneDmgEffect2_Sub2(NEW_PERTICLE *np, float *bpos1, float *bpos2, u_char r1, u_char g1, u_char b1, u_char r2, u_char g2, u_char b2);
void SetEneDmgEffect2_Sub();
void SetEneDmgEffect2();

#endif // GRAPHICS_GRAPH2D_EFFECT_ENE_H
