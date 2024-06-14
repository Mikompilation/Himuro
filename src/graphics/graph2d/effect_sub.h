#ifndef GRAPHICS_GRAPH2D_EFFECT_SUB_H
#define GRAPHICS_GRAPH2D_EFFECT_SUB_H

#include "typedefs.h"

#include "graphics/graph3d/sgsu.h"

typedef struct {
	u_char num;
	u_char result[10];
	sceVu0FVECTOR p[10];
} PP_JUDGE;

typedef struct {
	sceGsTex0 g_GsTex0;
	u_int g_nTexSizeW;
	u_int g_nTexSizeH;
	u_char g_bMipmapLv;
	long unsigned int g_GsMiptbp1;
	long unsigned int g_GsMiptbp2;
	float pos_x;
	float pos_y;
	int pos_z;
	float size_w;
	float size_h;
	float scale_w;
	float scale_h;
	int clamp_u;
	int clamp_v;
	u_int rot_center;
	float angle;
	u_char r;
	u_char g;
	u_char b;
	u_char alpha;
	u_int mask;
} SPRITE_DATA;

typedef struct {
	u_long tex1;
	u_long alpha;
	u_long zbuf;
	u_long test;
	u_long clamp;
	u_long prim;
} DRAW_ENV;

// extern int vib1_time;
// extern int vib2_time;
// extern int vib2_pow;
// extern int tecmo_logo_flow;

#include "graphics/graph2d/effect_sub.h"

void InitEffectSub();
void SetParam(int alp, int time, u_char r, u_char g, u_char b, int flag);
int ScreenCtrl();
void SetSquareS(int pri, float x1, float y1, float x4, float y4, u_char r, u_char g, u_char b, u_char a);
void SetSquare(int pri, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, u_char r, u_char g, u_char b, u_char a);
void SetSquare2s(int pri, float x1, float y1, float x4, float y4, u_char r1, u_char g1, u_char b1, u_char r2, u_char g2, u_char b2, u_char a);
void SetSquareZ(int pri, float x1, float y1, float x4, float y4, int z);
void SetPanel(u_int pri, float x1, float y1, float x2, float y2, u_char r, u_char g, u_char b, u_char a);
void SetPanel2(u_int pri, float x1, float y1, float x2, float y2, int z, u_char r, u_char g, u_char b, u_char a);
void SetSquareSN(int pri, float x1, float y1, float x4, float y4, u_char r, u_char g, u_char b, u_char a);
void SetSquareN(int pri, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, u_char r, u_char g, u_char b, u_char a);
void SetTriangle(int pri, float x1, float y1, float x2, float y2, float x3, float y3, u_char r, u_char g, u_char b, u_char a);
void SetTriangleZ(int pri, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, u_char r, u_char g, u_char b, u_char a);
void SetLine(int pri, float x1, float y1, float x2, float y2, u_char r, u_char g, u_char b, u_char a);
void SetLine2(int pri, float x1, float y1, float x2, float y2, u_char r, u_char g, u_char b, u_char a);
void DrawPoint(float *mpos, int no);
void DrawPoint2(float *mpos, u_char r, u_char g, u_char b, u_char a);
void DrawLine(float *mpos1, u_char r1, u_char g1, u_char b1, u_char a1, float *mpos2, u_char r2, u_char g2, int b2, int a2);
void Set3DPosTexure(float *wlm[4], DRAW_ENV *de, int texno, float w, float h, u_char r, u_char g, u_char b, u_char a);
void _SetTexDirectS(int pri, SPRITE_DATA *sd, int atype);
void SetTexDirectS(int pri, int atype);
void SetTexDirectS2(int pri, int type);
void SetTexDirect2(int pri, sceVu0FVECTOR *v);
void SetTexDirect(int atype);
void SetScreenDSlide(int addr, int type, int alpha, float ang, float scl, int z);
void SetScreenZ(int addr);
void CaptureScreen(u_int addr);
void DrawScreen(u_int pri, u_int addr, u_char r, u_char g, u_char b, u_char a);
void ClearFBuffer();
void ClearZBuffer();
void CamSave();
int CamChangeCheck();
void* GetEmptyBuffer(int no);
void CheckPointDepth(PP_JUDGE *ppj);
void GetCamI2DPos(float *pos, float *tx, float *ty);
void Get2PosRot(float *v1, float *v2, float *x, float *y);
void Get2PosRot2(float *v1, float *v2, float *x, float *z);
void GetTrgtRotType2(float *p0, float *p1, float *rot, int id);
void CalcSimEquations(double *sq[3], float *x, float *y);
float Get2PLength(float *v1, float *v2);
int log_2(float num);
void LocalCopyLtoB_Sub(int no, int type, int addr);
void LocalCopyLtoB_Sub2(int no, int type, int addr);
void LocalCopyBtoL_Sub(int no, int type, int addr);
int GetYOffset();
float GetYOffsetf();
void LocalCopyLtoB2(int no, int addr);
void LocalCopyLtoB(int no, int addr);
void LocalCopyBtoL(int no, int addr);
void LocalCopyLtoB_NB(int no, int addr);
void LocalCopyBtoL_NB(int no, int addr);
void ClearLocalCopyLtoLCache();
int LocalCopyLtoLDraw(int addr1, int addr2);
void LocalCopyLtoL(int addr1, int addr2);
void LocalCopyZtoBZ();
void LocalCopyBZtoZ();
void LocalCopyLtoBD(int addr, void *outbuf);
void SetVibrate(int type, int time, int pow);
void CallVibrate();

#ifdef BUILD_EU_VERSION
#else
void InitTecmotLogo();
int SetTecmoLogo();
#endif

void set_vect(float *v, float x, float y, float z, float w);
void Vu0SubOuterProduct(float *v0, float *v1, float *v2, float *v3);
void Vu0Normalize(float *v0, float *v1);
void Vu0ApplyMatrix(float *v0, float *m0[4], float *v1);
void Vu0MulVector(float *v0, float *v1, float *v2);
void Vu0FTOI0Vector(int *v0, float *v1);

#endif // GRAPHICS_GRAPH2D_EFFECT_SUB_H
