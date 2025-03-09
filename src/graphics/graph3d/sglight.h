#ifndef GRAPHICS_GRAPH3D_SGLIGHT_H
#define GRAPHICS_GRAPH3D_SGLIGHT_H

#include "typedefs.h"

#include "graphics/graph3d/sg_dat.h"
#include "graphics/graph3d/sgsgd.h"

void SgPreRenderDbgOn();
void SgPreRenderDbgOff();
void QueSetMatrix(float *m[4], int que, float *v);
void QueSetMatrixNonNormalized(int que);
void SgSetAmbient(float *ambient);
void SgSetDefaultLight(float *eye, SgLIGHT *p0, SgLIGHT *p1, SgLIGHT *p2);
void _ReadDLightMtx(float *tmp[4]);
void _ReadDColor(float *tmp[4]);
int GetDecay();
float SetMaxColor255(float *dcol, float *col);
void SetBWLight(float *col);
void SetMaterialPointer();
void ClearMaterialCache(HeaderSection *hs);
void SetMaterialDataVU(u_int *prim);
void SetMaterialPointerCoord();
void SetMaterialPointerCoordVU();
void SetMaterialDataPrerender();
void SetMaterialData(u_int *prim);
void SetPointGroup();
void SetSpotGroup(float *wlmtx[4]);
void SetLightData(SgCOORDUNIT *cp0, SgCOORDUNIT *cp1);
void SgSetInfiniteLights(float *eye, SgLIGHT *lights, int num);
void SgSetPointLights(SgLIGHT *lights, int num);
void SgSetSpotLights(SgLIGHT *lights, int num);
void PushLight();
void PopLight();
void ClearLightStack();
void CalcPointLight();
void CalcSpotLight();
void SgReadLights(void *sgd_top, void *light_top, float *Ambient, SgLIGHT *Ilight, int imax, SgLIGHT *Plight, int pmax, SgLIGHT *Slight, int smax);
u_int* GetNextUnpackAddr(u_int *prim);
void SetPreRenderTYPE0(int gloops, u_int *prim);
void SetPreRenderTYPE2(int gloops, u_int *prim);
void SetPreRenderTYPE2F(int gloops, u_int *prim);
void SetPreRenderMeshData(u_int *prim);
void SelectLight(u_int *prim);
// void SgPreRenderPrim(u_int *prim);
// void SgPreRender(void *sgd_top, int pnum);
void ClearPreRenderMeshData(u_int *prim);
void SgClearPreRenderPrim(u_int *prim);
void SgClearPreRender(void *sgd_top, int pnum);

#endif // GRAPHICS_GRAPH3D_SGLIGHT_H
