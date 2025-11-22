#ifndef GRAPHICS_GRAPH2D_EFFECT_OTH_H
#define GRAPHICS_GRAPH2D_EFFECT_OTH_H

#include "typedefs.h"

typedef struct {
	sceVu0FVECTOR opos[24];
	int tbl[24];
} FACESPIRIT_ST;

typedef struct {
	sceVu0FVECTOR position;
	sceVu0FVECTOR color;
	sceVu0FVECTOR velocity;
	float alp_step;
	int lifetime;
	int dummy[2];
} PARTICLE;

typedef struct {
	PARTICLE particles[200];
	int head;
	int cnt;
	short int flag;
	short int max;
	short int disp;
	short int blife;
} HEAT_HAZE;

typedef struct {
	sceVu0FVECTOR npos;
	sceVu0FVECTOR bpos;
	sceVu0FVECTOR vel;
	int time;
	int life;
	int alp;
	int dummy;
} FIREFLY;

typedef struct {
    sceVu0FVECTOR pos;
    int flow;
    float rot;
    float cnt;
    int dummy;
} NEW_ITEM;

typedef struct {
	u_int u1;
	u_int u2;
	u_int v1;
	u_int v2;
	u_int x1;
	u_int x2;
	u_int y1;
	u_int y2;
	u_int zz;
	float alp;
	u_int dummy[2];
} HAZE_WORK;

typedef struct {
	sceVu0FVECTOR oposp;
	sceVu0FVECTOR oposc;
	float scr;
	float mov;
	float lng;
	float dummy;
} HAZE_NUMS;

typedef struct {
	sceVu0IVECTOR iv;
	u_int tx;
	u_int ty;
	u_int fg;
	u_short alp;
	u_short cl;
} TEX_WORK;

typedef struct {
	float v;
	float oy;
	float t;
	u_short dist;
	short int mode;
} RIPPLE2_M;

typedef struct {
	RIPPLE2_M rip2m[275];
	float texsc;
	float time;
	int sufno;
	int dummy;
} RIPPLE2;

typedef struct {
	/* 0x00 */ sceVu0FVECTOR pos;
	/* 0x10 */ sceVu0FVECTOR rot;
	/* 0x20 */ float scl;
	/* 0x24 */ float mscl;
	/* 0x28 */ float spd;
	/* 0x2c */ u_char r;
	/* 0x2d */ u_char g;
	/* 0x2e */ u_char b;
	/* 0x2f */ u_char alp;
} RIPPLE_SUB;

typedef struct {
	sceVu0FVECTOR bpos;
	sceVu0FVECTOR vt[275];
	sceVu0IVECTOR vtiw[275];
	sceVu0IVECTOR col[275];
	u_int tx[275];
	u_int ty[275];
	short int clip[275];
	short int fg[275];
	int pnumw;
	int vnumw;
	int pnumh;
	int vnumh;
	float tsw;
	float tsh;
	sceVu0FMATRIX slm;
} RIP_WORK1;

typedef struct {
	int fl;
	u_char r;
	u_char g;
	u_char b;
	u_char a;
	float mang;
	float cnt;
	sceVu0FVECTOR pos;
	sceVu0FVECTOR opos;
	sceVu0FVECTOR vel;
	sceVu0FVECTOR ang;
} EFF_LEAF_ONE;

typedef struct {
	int flag;
	int type;
	sceVu0FVECTOR bpos;
	EFF_LEAF_ONE lo[16];
} EFF_LEAF;

typedef struct {
	float alp;
	float cnt;
	sceVu0IVECTOR ivec[4];
	sceVu0FVECTOR pos;
	sceVu0FVECTOR vel;
	sceVu0FVECTOR opos;
} EFF_DUST;

typedef struct { // 0x300
	sceVu0FVECTOR vt[15];
	sceVu0IVECTOR vtiw[15];
	sceVu0FVECTOR bpos;
	u_int tx[15];
	u_int ty[15];
	short int clip[15];
	short int fg[15];
	int pnumw;
	int vnumw;
	int pnumh;
	int vnumh;
	float tsw;
	float tsh;
	sceVu0FMATRIX slm;
} RIP_WORK2;

typedef struct {
	sceVu0FVECTOR fpos[8];
	sceVu0IVECTOR ipos[8];
	u_char clip[8];
	u_char alp[8];
	U32DATA ts[8];
	U32DATA tt[8];
	U32DATA tq[8];
} EFF_SUNSHINE;

#include "graphics/graph2d/effect.h"

extern int stop_lf;
extern u_char prgb;
extern u_char pa;
extern float wind1;
extern float wind2;
extern float ppos1;
extern float pvel1;
extern float pvel2;
extern float pvel3;
extern float psize;
extern sceVu0FVECTOR canal_bpos[3];
extern float canal_size[3][2];
extern float texsc[3];
extern float canaltm[3];
extern float ligscl;
extern float cana1;
extern float cana2;
extern float cana3;
extern float cana4;
extern int setlight;
extern int setpoly;
extern int cont_facespr_tbl[3];
extern int cont_facespr_otbl[3];
extern FACESPIRIT_ST facespirit[3];

void InitEffectOth();
void InitEffectOthEF();
int SearchEmptyRippleBuf();
void* CallRipple(void *pos, void *rot, float scale, int num, int time);
void SetRipple(EFFECT_CONT *ec);
void RunRipple2();
void SetEffSQTex(int n, float *v, int tp, float w, float h, u_char r, u_char g, u_char b, u_char a);
void SetEffSQITex(int n, int *v, int tp, float w, float h, u_char r, u_char g, u_char b, u_char a);
void* CallFire(void *pos, u_char r, u_char g, u_char b, float scale);
void* CallFire2(void *pos, u_char r, u_char g, u_char b, float scl, u_char r2, u_char g2, u_char b2, float scl2);
void* CallFire3(void *pos, int type, u_char r, u_char g, u_char b, float scl, u_char r2, u_char g2, u_char b2, float scl2);
void SubFire1(EFFECT_CONT *ec);
void SetFire(EFFECT_CONT *ec);
void SetFire2(EFFECT_CONT *ec);
void SetHalo(EFFECT_CONT *ec);
void SubHalo(float *p, int type, int textp, u_int z, u_char r, u_char g, u_char b, u_char alp, float sc);
void InitHeatHaze();
int draw_distortion_particles(sceVu0FMATRIX *local_screen, sceVu0FMATRIX *local_clip, int fr, int t_particles, void *particles, int size_of_particle, float psize, float distortion_amount, int type);
void add_particle(int type, HEAT_HAZE *hh, float *pos, float *vel, float r, float g, float b, float a);
void update_particles(PARTICLE *prt);
void add_particle2(int type, HEAT_HAZE *hh, float *pos, float *vel, float r, float g, float b, float a);
void update_particles2(HEAT_HAZE *hh, float lng, float arate);
void sceVu0RotCameraMatrix(sceVu0FMATRIX m, float *p, float *zd, float *yd, float *rot);
void sceVu0ViewClipMatrix(sceVu0FMATRIX vc, float scrw, float scrh, float scrz, float zmin, float zmax);
void* ContHeatHaze(void *addr, int type, float *pos, float *pos2, int st, float r, float g, float b, float a, float size, float arate);
void SetPartInit(HEAT_HAZE *addr, int type, int lifetime);
void* GetItemPartAddr(void *addr, int type, int lifetime);
void* GetEnePartAddr(void *addr, int type, int lifetime);
void* GetAmuPartAddr(void *addr, int type, int lifetime);
void* GetTorchPartAddr(void *addr, int type, int lifetime);
void* GetSmokePartAddr(void *addr, int type, int lifetime);
void SetNewItemEffect(NEW_ITEM *nip);
void CheckItemEffect();
void SetEneFire(EFFECT_CONT *ec);
void CallAmuletFire();
int SetAmuletFire();
void* ContTorch(void *addr, int type, float *pos, float *pos2, int st, float r, float g, float b, float a, float size, float arate, int tp2, float ar, float sr);
void SetTorch(EFFECT_CONT *ec);
void* ContSmoke(void *addr, int type, float *pos, float *pos2, int st, float r, float g, float b, float a, float size, float arate);
void SetSmoke(EFFECT_CONT *ec);
void SetSunshine(EFFECT_CONT *ec);
void SetDust(EFFECT_CONT *ec);
void SetWaterdrop(EFFECT_CONT *ec);
void SetDustLeaf(sceVu0FVECTOR pos, int type);
void SetLeaf(sceVu0FVECTOR pos);
void SetDust2(sceVu0FVECTOR pos);
void RunLeafSub(EFF_LEAF *lep);
void RunLeaf();
void light_rgb(sceVu0IVECTOR c0, sceVu0FMATRIX LocalLightMatrix, sceVu0FMATRIX LightColorMatrix, float *v0, float *v1, float *v2, float *c1);
u_char SetCanalSub(int no, float *mpos);
void Call3DRipple(sceVu0FVECTOR bpos);
void ContCanal();
void SetCanal();
void SetGlowOfAFirefly(float *p, float sc, u_char r1, u_char g1, u_char b1, u_char r2, u_char g2, u_char b2, u_char alp);
void SetFireflySub(FIREFLY *ffp);
void SetFirefly();
void SetSky();
void SetPond();
void SetHaze_Pond_SW(int sw);
void SetHaze_Pond();
void DrawNewPerticleSub(int num, sceVu0FVECTOR *pos, u_char r1, u_char g1, u_char b1, u_char r2, u_char g2, u_char b2, u_char a);
void SetEneFace(EFFECT_CONT *ec);
void SetSpirit(EFFECT_CONT *ec);
void SetFaceSpirit(EFFECT_CONT *ec);
void SetSaveCameraLamp();

#endif // GRAPHICS_GRAPH2D_EFFECT_OTH_H
