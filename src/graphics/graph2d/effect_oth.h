#ifndef GRAPHICS_GRAPH2D_EFFECT_OTH_H
#define GRAPHICS_GRAPH2D_EFFECT_OTH_H

#include "typedefs.h"

typedef union {
	int ui32;
	float fl32;
} U32DATA;

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

typedef struct
{ // 0x20
    /* 0x00 */ sceVu0FVECTOR pos;
    /* 0x10 */ int flow;
    /* 0x14 */ float rot;
    /* 0x18 */ float cnt;
    /* 0x1c */ int dummy;
} NEW_ITEM;

#include "graphics/graph2d/effect.h"

// extern int stop_lf;
// extern u_char prgb;
// extern u_char pa;
// extern float wind1;
// extern float wind2;
// extern float ppos1;
// extern float pvel1;
// extern float pvel2;
// extern float pvel3;
// extern float psize;
// extern sceVu0FVECTOR canal_bpos[3];
// extern float canal_size[3][2];
// extern float texsc[3];
// extern float canaltm[3];
// extern float ligscl;
// extern float cana1;
// extern float cana2;
// extern float cana3;
// extern float cana4;
// extern int setlight;
// extern int setpoly;
// extern int cont_facespr_tbl[3];
// extern int cont_facespr_otbl[3];
// extern FACESPIRIT_ST facespirit[3];

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
void SubFire1();
void SetFire();
void SetFire2();
void SetHalo();
void SubHalo(float *p, int type, int textp, u_int z, u_char r, u_char g, u_char b, u_char alp, float sc);
void InitHeatHaze();
int draw_distortion_particles(sceVu0FMATRIX *local_screen, sceVu0FMATRIX *local_clip, int fr, int t_particles, void *particles, int size_of_particle, float psize, float distortion_amount, int type);
void add_particle(int type, HEAT_HAZE *hh, float *pos, float *vel, float r, float g, float b, float a);
void update_particles(PARTICLE *prt);
void add_particle2(int type, float *pos, float *vel, float r, float g, float b, float a);
void update_particles2(float lng, float arate);
void sceVu0RotCameraMatrix(float *m[4], float *p, float *zd, float *yd, float *rot);
void sceVu0ViewClipMatrix(float *vc[4], float scrw, float scrh, float scrz, float zmin, float zmax);
void* ContHeatHaze(void *addr, int type, float *pos, float *pos2, int st, float r, float g, float b, float a, float size, float arate);
void SetPartInit(HEAT_HAZE *addr, int type, int lifetime);
void* GetItemPartAddr(void *addr, int type, int lifetime);
void* GetEnePartAddr(void *addr, int type, int lifetime);
void* GetAmuPartAddr(void *addr, int type, int lifetime);
void* GetTorchPartAddr(void *addr, int type, int lifetime);
void* GetSmokePartAddr(void *addr, int type, int lifetime);
void SetNewItemEffect(NEW_ITEM *nip);
void CheckItemEffect();
void SetEneFire();
void CallAmuletFire();
int SetAmuletFire();
void* ContTorch(void *addr, int type, float *pos, float *pos2, int st, float r, float g, float b, float a, float size, float arate, int tp2, float ar, float sr);
void SetTorch();
void* ContSmoke(void *addr, int type, float *pos, float *pos2, int st, float r, float g, float b, float a, float size, float arate);
void SetSmoke();
void SetSunshine();
void SetDust();
void SetWaterdrop();
void SetDustLeaf(float *pos, int type);
void SetLeaf(float *pos);
void SetDust2(float *pos);
void RunLeafSub();
void RunLeaf();
void light_rgb(int *c0, float *LocalLightMatrix[4], float *LightColorMatrix[4], float *v0, float *v1, float *v2, float *c1);
u_char SetCanalSub(int no, float *mpos);
void Call3DRipple(float *bpos);
void ContCanal();
void SetCanal();
void SetGlowOfAFirefly(float *p, float sc, u_char r1, u_char g1, u_char b1, u_char r2, u_char g2, u_char b2, u_char alp);
void SetFireflySub(FIREFLY *ffp);
void SetFirefly();
void SetSky();
void SetPond();
void SetHaze_Pond_SW(int sw);
void SetHaze_Pond();
void DrawNewPerticleSub(int num, sceVu0FVECTOR *pos, u_char r1, u_char g1, u_char b1, u_char r2, u_char g2, u_char b2, int a);
void SetEneFace();
void SetSpirit();
void SetFaceSpirit();
void SetSaveCameraLamp();

#endif // GRAPHICS_GRAPH2D_EFFECT_OTH_H
