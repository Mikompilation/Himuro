#ifndef GRAPHICS_GRAPH3D_SGSGD_H
#define GRAPHICS_GRAPH3D_SGSGD_H

#include "typedefs.h"
#include "ee/eestruct.h"

#include "graphics/graph3d/sg_dat.h"

void MappingVUVNData(u_int *intpointer, HeaderSection *hs);
void MappingVUVNDataPreset(u_int *intpointer, int mtype, int gloops, int hsize);
void MappingTYPE0(int gloops, u_int *prim);
int MappingTYPE2(int gloops, u_int *prim);
void MappingMeshData(u_int *intpointer, u_int *vuvnprim, HeaderSection *hs);
void MappingCoordinateData(u_int *intpointer, HeaderSection *hs);
void MappingVertexList(VERTEXLIST *vli, PHEAD *ph);
void SgMapRebuld(void *sgd_top);
void SgMapUnit(void *sgd_top);
u_int* GetPrimitiveAddr(void *sgd_top, int num);
u_int* GetStartPacketAddr(void *sgd_top, int num);
u_int* GetEndPacketAddr(void *sgd_top, int num);
void PostChainPacket(u_int *dest, u_int *source);
int GetModelKind(void *sgd_top);
void SgSetTEX1Prim(u_int *prim, sceGsTex1 *ptex1);
void SgSetTEX1(void *sgd_top, sceGsTex1 *ptex1);

#endif // GRAPHICS_GRAPH3D_SGSGD_H
