#ifndef GRAPHICS_GRAPH3D_SHADOW_H
#define GRAPHICS_GRAPH3D_SHADOW_H

#include "typedefs.h"

#include "graphics/graph3d/sg_dat.h"
#include "graphics/graph3d/gra3d.h"

typedef struct {
    int s;
    int e;
} BoundLine;

typedef struct {
	void *shadow_model;
	int smodel_num;
	void **search_model;
	int search_num;
	void *source_model;
	sceVu0FVECTOR direction;
	sceVu0FVECTOR *bbox;
	SgCAMERA *camera;
	sceVu0IVECTOR color;
	u_long shadow_tex0;
} ShadowHandle;

typedef void (*EnvFuncCallback)();

// extern SgCAMERA scamera;

void ShadowDbgOn();
void ShadowDbgOff();
void ShadowInit();
void SetShadowTexSize(u_int width, u_int height);
void SetShadowTexAddr(u_int addr);
void SetShadowFundamentScale(float size);
void SetShadowAssignGroup(int gnum);
void TransShadowClut();
void GetShadowTexture();
void DispShadowSprite();
void GetInverseMatrix(float *inv[4], float *org[4]);
void SetVU1HeaderShadow();
void CalcVertexBufferShadow(u_int *prim);
u_int* SetVUVNDataShadowModel(u_int *prim);
void ShadowModelMesh(u_int *prim);
void DrawShadowModelPrim(u_int *prim);
void SetUpShadowModel();
void DrawShadowModel(void *sgd_top, int pnum);
void ShadowMeshDataVU(u_int *prim);
int ClipCheckShadow(sceVu0FVECTOR *vec, float *cul);
int ShadowBoundClip(float *v1);
int AppendShadowClipCheck(sceVu0FVECTOR *sts, BoundLine *bl);
int CheckBoundingBoxShadowTrace(float *lwmtx[4], sceVu0FVECTOR *tmpv, float *dir);
int CheckBoundingBoxShadow(u_int *prim);
void AssignShadowPrim(u_int *prim);
void AssignShadowPreProcess(u_int *prim);
void AssignShadow(void *sgd_top, int except_num);
void SetUpShadow(ShadowHandle *shandle);
void ClearShadowFrame();
void SetShadowEnvironment();
void GetRotMatrixYZPlain(float *rmat[4], float *vec);
void GetRotMatrixZAxis(float *rmat[4], float *vec);
void CalcShadowMatrix(float *center, float ax, float ay);
void CalcShadowHeight(sceVu0FVECTOR *bbox);
void SetShadowCamera(float *center, sceVu0FVECTOR *bbox, SgCOORDUNIT *cp);
void DrawShadow(ShadowHandle *shandle, EnvFuncCallback env_func);

#endif // GRAPHICS_GRAPH3D_SHADOW_H
