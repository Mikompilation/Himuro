#ifndef GRAPHICS_GRAPH3D_SGLIB_H
#define GRAPHICS_GRAPH3D_SGLIB_H

#include "typedefs.h"

#include "sce/libdma.h"

#include "graphics/graph3d/sg_dat.h"

// typedef sceVu0FMATRIX sceVu0FMATRIX[4];
typedef float LMATRIX[3][4];

typedef struct {
	sceVu0FVECTOR pos[3];
	sceVu0FVECTOR btimes;
	sceVu0FVECTOR eyevec;
	LMATRIX DColorMtx;
	LMATRIX SColorMtx;
	int lnum[3];
	u_int pad;
} SgPOINTGROUP;

typedef struct {
	sceVu0FVECTOR pos[3];
	sceVu0FVECTOR intens;
	sceVu0FVECTOR intens_b;
	sceVu0FVECTOR btimes;
	LMATRIX WDLightMtx;
	LMATRIX SLightMtx;
	LMATRIX DColorMtx;
	LMATRIX SColorMtx;
	int lnum[3];
	u_int pad;
} SgSPOTGROUP;

typedef struct {
	LMATRIX Parallel_DLight;
	LMATRIX Parallel_SLight;
	LMATRIX Spot_pos;
	sceVu0FVECTOR Spot_intens;
	sceVu0FVECTOR Spot_intens_b;
	LMATRIX Spot_WDLightMtx;
	LMATRIX Spot_SLightMtx;
	LMATRIX Point_pos;
	sceVu0FVECTOR Point_eyevec;
} SgVULightCoord;

typedef struct {
	qword ParallelVif;
	sceVu0FVECTOR Parallel_Ambient;
	LMATRIX Parallel_DColor;
	LMATRIX Parallel_SColor;
} SgVULightParallel;

typedef struct {
	qword SpotVif;
	sceVu0FVECTOR Spot_btimes;
	LMATRIX Spot_DColor;
	LMATRIX Spot_SColor;
} SgVULightSpot;

typedef struct {
	qword PointVif;
	sceVu0FVECTOR Point_btimes;
	LMATRIX Point_DColor;
	LMATRIX Point_SColor;
} SgVULightPoint;

typedef struct {
	u_short QWC;
	unsigned int DUMMY : 10;
	unsigned int PCE : 2;
	unsigned int ID : 3;
	unsigned int IRQ : 1;
	unsigned int ADDR : 31;
	unsigned int SPR : 1;
	u_int pad[2];
} SgSourceChainTag;

typedef struct {
	int num;
	int lnum[3];
} MatCache;

typedef struct {
	int cache_on;
	int edge_check;
	int cn0;
	int cn1;
	u_int tagd_addr;
	int qwc;
	MatCache Parallel;
	MatCache Point;
	MatCache Spot;
} CoordCache;

typedef struct {
	u_int addr;
	u_int max_width;
	u_int max_height;
	u_int width;
	u_int height;
	u_int width_p;
	u_int height_p;
	float scale;
	float fund_scale;
} ShadowTexture;

extern SgLIGHT *SgInfiniteLight;
extern SgLIGHT *SgPointLight;
extern SgLIGHT *SgSpotLight;
extern int model_buffer_size;
extern int model_tag_size;
extern sceVu0FVECTOR *vertex_buffer;
extern sceVu0FVECTOR *normal_buffer;
extern int vnbuf_size;
extern sceVu0FVECTOR vf12reg[2];
extern int loadtri2_flg;
extern int loadbw_flg;
extern int set_bw_color;
extern sceVu0FVECTOR *objwork;
extern SgSourceChainTag *cachetag;
extern int vu1tag_num;
extern int sbuffer_p;
extern int edge_check;
extern sceVu0FMATRIX SgWSMtx;
extern sceVu0FMATRIX SgCMtx;
extern sceVu0FMATRIX SgCMVtx;
extern sceVu0FMATRIX DLightMtx;
extern sceVu0FMATRIX SLightMtx;
extern sceVu0FMATRIX DColorMtx;
extern sceVu0FMATRIX SColorMtx;
extern sceVu0FVECTOR TAmbient;
extern int ptag;
extern qword *SgWorkBase;
extern SgVULightCoord *SgLightCoordp;
extern SgVULightParallel *SgLightParallelp;
extern SgVULightSpot *SgLightSpotp;
extern SgVULightPoint *SgLightPointp;
extern int SgInfiniteNum;
extern int SgPointNum;
extern int SgPointGroupNum;
extern SgPOINTGROUP SgPointGroup[1];
extern sceVu0FVECTOR ieye;
extern int SgSpotNum;
extern int SgSpotGroupNum;
extern SgSPOTGROUP SgSpotGroup[1];
extern void *sgd_top_addr;
extern CoordCache ccahe;
extern sceDmaChan *dvif0;
extern sceDmaChan *dvif1;
extern sceDmaChan *dgif;
extern sceDmaChan *dfspr;
extern SgSourceChainTag *cachetagdbuf[2];
extern sceVu0FVECTOR *objworkdbuf[2];
extern int tagswap;
extern sceVu0FVECTOR fog_value;
extern sceVu0IVECTOR fog_color;
extern ShadowTexture shadowtex;
extern int vu_prog_num;
extern int clip_value_check;
extern sceVu0FVECTOR clip_value;
extern int wscissor_flg;
extern sceVu0FVECTOR wscissor_upper;
extern sceVu0FVECTOR wscissor_lower;
extern u_int *pGroupPacket;

static inline qword* getObjWrk()
{
    return (qword *)&objwork[sbuffer_p];
}

void _GetNormalVectorFromVector(sceVu0FVECTOR norm, sceVu0FVECTOR p0, sceVu0FVECTOR p1);
void WaitVU1();
float _TransPPower(float scale);
float _TransSPower(float scale);
void Set12Register();
void SetVF2Register(sceVu0FVECTOR vf2reg);
void GetVF2Register(sceVu0FVECTOR vf2reg);
void printVector(sceVu0FVECTOR v);
void printVectorC(sceVu0FVECTOR v, char *s);
void printMat(sceVu0FMATRIX m0);
void printMatC(sceVu0FMATRIX m0, char *s);
void printLMatC(sceVu0FMATRIX m0, char *str);
void SgSetWsMtx(sceVu0FMATRIX m0);
void SgSetClipMtx(sceVu0FMATRIX m0);
void SgSetClipVMtx(sceVu0FMATRIX m0);
void SgSetWorkBase(qword *base);
qword* SgGetWorkBase();
void SgSetProjection(float scrz);
float SgGetProjection();
void SgInit3D();
void GetMatrixFromQuaternion(sceVu0FMATRIX quat, sceVu0FVECTOR qvert);
void GetMatrixRotateAxis(sceVu0FMATRIX quat, sceVu0FVECTOR axis, float theta);
float SgSinf(float rad);
float SgCosf(float rad);
float SgSinfd(float degree);
float SgCosfd(float degree);
float SgACosf(float ccos);
static void GetATanf(sceVu0FVECTOR *tmpv);
float SgAtanf(float x);
float SgAtan2f(float y, float x);
float SgSqrtf(float len);
float SgRSqrtf(float len);
static void GetSgCalclen(sceVu0FVECTOR tmpv);
float SgCalcLen(float x, float y, float z);
float _CalcLen(sceVu0FVECTOR v0, sceVu0FVECTOR v1);
void _MulRotMatrix(sceVu0FMATRIX ans, sceVu0FMATRIX m0, sceVu0FMATRIX m1);
void _MulMatrix(sceVu0FMATRIX ans, sceVu0FMATRIX m0, sceVu0FMATRIX m1);
void _SetMulMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1);
void _CalcLenASM(sceVu0FVECTOR out, sceVu0FVECTOR v0, sceVu0FVECTOR v1);
// void _NormalizeVector(sceVu0FVECTOR v, sceVu0FVECTOR v0);
void _NormalizeVector2(sceVu0FVECTOR v, sceVu0FVECTOR v0);
void _ApplyRotMatrix(sceVu0FVECTOR v0, sceVu0FVECTOR v1);
void _ApplyMatrixXYZ(sceVu0FVECTOR v0, sceVu0FMATRIX m, sceVu0FVECTOR v1);

#endif // GRAPHICS_GRAPH3D_SGLIB_H
