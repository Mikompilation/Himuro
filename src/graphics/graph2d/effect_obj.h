#ifndef GRAPHICS_GRAPH2D_EFFECT_OBJ_H
#define GRAPHICS_GRAPH2D_EFFECT_OBJ_H

#include "typedefs.h"

typedef struct {
	float r;
	float add;
	float h;
	float x;
	float y;
	float z;
	float ox;
	float oy;
	float oz;
	float lng;
} EFFPOS;

typedef struct {
	EFFPOS ep[289];
	short int use;
	short int pass;
	float r;
} EFFINFO2;

#include "graphics/graph2d/effect.h"

extern EFFINFO2 efi[8];

void InitEffectObj();
void InitEffectObjEF();
void SetPartsBlurSW(int sw);
void SetPartsBlur1Sub(u_char alp);
void SetPartsBlur1(u_char alp);
void SetPartsBlur2();
void RunPartsBlur();
void CallPartsBlur2(int no, int in, int keep, int out, u_char alp);
void CallPartsBlur4(int no, float *vol);
void ResetPartsBlur();
void* CallPartsDeform2(int type, float scale, void *pos, u_int in, u_int keep, u_int out);
void* CallPartsDeform3(int type, float scale, void *pos, u_int in, u_int keep, u_int out, int alp);
void* CallPartsDeform3_2(int type, float sclx, float scly, void *pos, u_int in, u_int keep, u_int out, int alp);
void* CallPartsDeform4(int type, float scale, void *pos, float *vol);
void* CallPartsDeform5(int type, float sclx, float scly, void *pos, float *vol);
void SetPartsDeform(EFFECT_CONT *ec);
void SetVURand(float x);
int CalcPartsDeformXYZ(sceVu0IVECTOR vi, sceVu0FVECTOR vf);
void MakePartsDeformPacket(int pnumw, int pnumh, sceVu0FVECTOR *vt, sceVu0FMATRIX wlm, sceVu0FVECTOR *stq, u_char *use_alpha, float aprate, u_long tex0);
u_char SubPartsDeform1(EFFECT_CONT *ec, u_char num, int page, int sbj, float sclx, float scly, float vol, int fl, float spd, float rate, float trate);
u_char SubPartsDeform2(EFFECT_CONT *ec, u_char num, int page, int sbj, float sclx, float scly, float vol, int fl, float spd, float rate, float trate);
int GetCornHitCheck(float *bpos, float power);
int GetCornHitCheck2(float *bpos, float power, float *rrate, float *lrate);
void SetRenzFlare(EFFECT_CONT *ec);
void SetStarRay(float *bpos, int tp, float sc, int num, float aang);
void SetStarRay(float *bpos, int tp, float sc, int num, float aang);
void _SetPartsDeformSTQRegs(sceVu0FVECTOR *params);
void _CalcParstDeformSTQ(float *stq, float *vt);

#endif // GRAPHICS_GRAPH2D_EFFECT_OBJ_H
