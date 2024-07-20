#ifndef GRAPHICS_GRAPH3D_SGLIB_H
#define GRAPHICS_GRAPH3D_SGLIB_H

#include "typedefs.h"

// typedef struct {
// 	unsigned int DIR : 1;
// 	unsigned int p0 : 1;
// 	unsigned int MOD : 2;
// 	unsigned int ASP : 2;
// 	unsigned int TTE : 1;
// 	unsigned int TIE : 1;
// 	unsigned int STR : 1;
// 	unsigned int p1 : 7;
// 	unsigned int TAG : 16;
// } tD_CHCR;
// 
// typedef _sceDmaTag sceDmaTag;
// 
// typedef struct {
// 	tD_CHCR chcr;
// 	u_int p0[3];
// 	void *madr;
// 	u_int p1[3];
// 	u_int qwc;
// 	u_int p2[3];
// 	sceDmaTag *tadr;
// 	u_int p3[3];
// 	void *as0;
// 	u_int p4[3];
// 	void *as1;
// 	u_int p5[3];
// 	u_int p6[4];
// 	u_int p7[4];
// 	void *sadr;
// 	u_int p8[3];
// } sceDmaChan;
// 
// typedef float sceVu0FMATRIX[4][4];
// typedef float LMATRIX[3][4];
// 
// typedef struct {
// 	sceVu0FVECTOR pos[3];
// 	sceVu0FVECTOR btimes;
// 	sceVu0FVECTOR eyevec;
// 	LMATRIX DColorMtx;
// 	LMATRIX SColorMtx;
// 	int lnum[3];
// 	u_int pad;
// } SgPOINTGROUP;
// 
// typedef struct {
// 	sceVu0FVECTOR pos[3];
// 	sceVu0FVECTOR intens;
// 	sceVu0FVECTOR intens_b;
// 	sceVu0FVECTOR btimes;
// 	LMATRIX WDLightMtx;
// 	LMATRIX SLightMtx;
// 	LMATRIX DColorMtx;
// 	LMATRIX SColorMtx;
// 	int lnum[3];
// 	u_int pad;
// } SgSPOTGROUP;
// 
// typedef struct {
// 	LMATRIX Parallel_DLight;
// 	LMATRIX Parallel_SLight;
// 	LMATRIX Spot_pos;
// 	sceVu0FVECTOR Spot_intens;
// 	sceVu0FVECTOR Spot_intens_b;
// 	LMATRIX Spot_WDLightMtx;
// 	LMATRIX Spot_SLightMtx;
// 	LMATRIX Point_pos;
// 	sceVu0FVECTOR Point_eyevec;
// } SgVULightCoord;
// 
// typedef struct {
// 	qword ParallelVif;
// 	sceVu0FVECTOR Parallel_Ambient;
// 	LMATRIX Parallel_DColor;
// 	LMATRIX Parallel_SColor;
// } SgVULightParallel;
// 
// typedef struct {
// 	qword SpotVif;
// 	sceVu0FVECTOR Spot_btimes;
// 	LMATRIX Spot_DColor;
// 	LMATRIX Spot_SColor;
// } SgVULightSpot;
// 
// typedef struct {
// 	qword PointVif;
// 	sceVu0FVECTOR Point_btimes;
// 	LMATRIX Point_DColor;
// 	LMATRIX Point_SColor;
// } SgVULightPoint;
// 
// typedef struct {
// 	u_short QWC;
// 	unsigned int DUMMY : 10;
// 	unsigned int PCE : 2;
// 	unsigned int ID : 3;
// 	unsigned int IRQ : 1;
// 	unsigned int ADDR : 31;
// 	unsigned int SPR : 1;
// 	u_int pad[2];
// } SgSourceChainTag;
// 
// typedef struct {
// 	int num;
// 	int lnum[3];
// } MatCache;
// 
// typedef struct {
// 	int cache_on;
// 	int edge_check;
// 	int cn0;
// 	int cn1;
// 	u_int tagd_addr;
// 	int qwc;
// 	MatCache Parallel;
// 	MatCache Point;
// 	MatCache Spot;
// } CoordCache;
// 
// typedef struct {
// 	u_int addr;
// 	u_int max_width;
// 	u_int max_height;
// 	u_int width;
// 	u_int height;
// 	u_int width_p;
// 	u_int height_p;
// 	float scale;
// 	float fund_scale;
// } ShadowTexture;
// 
// extern SgLIGHT *SgInfiniteLight;
// extern SgLIGHT *SgPointLight;
// extern SgLIGHT *SgSpotLight;
// extern int model_buffer_size;
// extern int model_tag_size;
// extern sceVu0FVECTOR *vertex_buffer;
// extern sceVu0FVECTOR *normal_buffer;
// extern int vnbuf_size;
// extern sceVu0FVECTOR vf12reg[2];
// extern int loadtri2_flg;
// extern int loadbw_flg;
// extern int set_bw_color;
// extern sceVu0FVECTOR *objwork;
// extern SgSourceChainTag *cachetag;
// extern int vu1tag_num;
// extern int sbuffer_p;
// extern int edge_check;
extern sceVu0FMATRIX SgWSMtx;
// extern sceVu0FMATRIX SgCMtx;
// extern sceVu0FMATRIX SgCMVtx;
// extern sceVu0FMATRIX DLightMtx;
// extern sceVu0FMATRIX SLightMtx;
// extern sceVu0FMATRIX DColorMtx;
// extern sceVu0FMATRIX SColorMtx;
// extern sceVu0FVECTOR TAmbient;
// extern int ptag;
// extern qword *SgWorkBase;
// extern SgVULightCoord *SgLightCoordp;
// extern SgVULightParallel *SgLightParallelp;
// extern SgVULightSpot *SgLightSpotp;
// extern SgVULightPoint *SgLightPointp;
// extern int SgInfiniteNum;
// extern int SgPointNum;
// extern int SgPointGroupNum;
// extern SgPOINTGROUP SgPointGroup[1];
// extern sceVu0FVECTOR ieye;
// extern int SgSpotNum;
// extern int SgSpotGroupNum;
// extern SgSPOTGROUP SgSpotGroup[1];
// extern void *sgd_top_addr;
// extern CoordCache ccahe;
// extern sceDmaChan *dvif0;
// extern sceDmaChan *dvif1;
// extern sceDmaChan *dgif;
// extern sceDmaChan *dfspr;
// extern SgSourceChainTag *cachetagdbuf[2];
// extern sceVu0FVECTOR *objworkdbuf[2];
// extern int tagswap;
// extern sceVu0FVECTOR fog_value;
// extern sceVu0IVECTOR fog_color;
// extern ShadowTexture shadowtex;
// extern int vu_prog_num;
// extern int clip_value_check;
// extern sceVu0FVECTOR clip_value;
// extern int wscissor_flg;
// extern sceVu0FVECTOR wscissor_upper;
// extern sceVu0FVECTOR wscissor_lower;
// extern u_int *pGroupPacket;

void _GetNormalVectorFromVector(float *norm, float *p0, float *p1);
void WaitVU1();
float _TransPPower(float scale);
float _TransSPower(float scale);
void Set12Register();
void SetVF2Register(float *vf2reg);
void GetVF2Register(float *vf2reg);
void printVector(float *v);
void printVectorC(float *v, char *s);
void printMat(float *m0[4]);
void printMatC(char *s);
void printLMatC(char *str);
void SgSetWsMtx();
void SgSetClipMtx();
void SgSetClipVMtx();
void SgSetWorkBase(qword *base);
qword* SgGetWorkBase();
void SgSetProjection(float scrz);
float SgGetProjection();
void SgInit3D();
void GetMatrixFromQuaternion(float *quat[4], float *qvert);
void GetMatrixRotateAxis(float *quat[4], float *axis, float theta);
float SgSinf(float rad);
float SgCosf(float rad);
float SgSinfd(float degree);
float SgCosfd(float degree);
float SgACosf(float ccos);
float SgAtanf(float x);
float SgAtan2f(float y, float x);
float SgSqrtf(float len);
float SgRSqrtf(float len);
float SgCalcLen(float x, float y, float z);
float _CalcLen(float *v0, float *v1);
void _MulRotMatrix(float *ans[4], float *m1[4]);
void _MulMatrix(float *ans[4], float *m1[4]);
void _SetMulMatrix(float *m1[4]);
void _CalcLenASM(float *out, float *v0, float *v1);
void _NormalizeVector(float *v, float *v0);
void _NormalizeVector2(float *v, float *v0);
void _ApplyRotMatrix(float *v0, float *v1);
void _ApplyMatrixXYZ(float *v0, float *m[4], float *v1);

#endif // GRAPHICS_GRAPH3D_SGLIB_H
