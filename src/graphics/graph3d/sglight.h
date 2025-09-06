#ifndef GRAPHICS_GRAPH3D_SGLIGHT_H
#define GRAPHICS_GRAPH3D_SGLIGHT_H

#include "typedefs.h"

#include "graphics/graph3d/sg_dat.h"
#include "graphics/graph3d/sgsgd.h"

void SgPreRenderDbgOn();
void SgPreRenderDbgOff();
void QueSetMatrix(sceVu0FMATRIX m, int que, sceVu0FVECTOR v);
void QueSetMatrixNonNormalized(sceVu0FMATRIX m, int que, sceVu0FVECTOR v);
void SgSetAmbient(sceVu0FVECTOR ambient);
void SgSetDefaultLight(sceVu0FVECTOR eye, SgLIGHT *p0, SgLIGHT *p1, SgLIGHT *p2);
void _ReadDLightMtx(sceVu0FMATRIX tmp);
void _ReadDColor(sceVu0FMATRIX tmp);
int GetDecay();
float SetMaxColor255(sceVu0FVECTOR dcol, sceVu0FVECTOR col);
void SetBWLight(sceVu0FVECTOR col);
void SetMaterialPointer();
void ClearMaterialCache(HeaderSection *hs);
void SetMaterialDataVU(u_int *prim);
void SetMaterialPointerCoord();
void SetMaterialPointerCoordVU();
void SetMaterialDataPrerender();
void SetMaterialData(u_int *prim);
void SetPointGroup();
void SetSpotGroup(sceVu0FMATRIX wlmtx);
void SetLightData(SgCOORDUNIT *cp0, SgCOORDUNIT *cp1);
void SgSetInfiniteLights(sceVu0FVECTOR eye, SgLIGHT *lights, int num);
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
