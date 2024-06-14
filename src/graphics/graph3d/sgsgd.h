#ifndef GRAPHICS_GRAPH3D_SGSGD_H
#define GRAPHICS_GRAPH3D_SGSGD_H

#include "typedefs.h"

#include "graphics/motion/accessory.h"

// typedef struct {
// 	long unsigned int LCM : 1;
// 	long unsigned int pad01 : 1;
// 	long unsigned int MXL : 3;
// 	long unsigned int MMAG : 1;
// 	long unsigned int MMIN : 3;
// 	long unsigned int MTBA : 1;
// 	long unsigned int pad10 : 9;
// 	long unsigned int L : 2;
// 	long unsigned int pad21 : 11;
// 	long unsigned int K : 12;
// 	long unsigned int pad44 : 20;
// } sceGsTex1;

typedef struct {
	short int cn0;
	short int cn1;
	u_short vIdx;
	u_short vOff;
} ONELIST;

typedef struct {
	int list_num;
	ONELIST lists[1];
} VERTEXLIST;

typedef struct {
	u_int primtype;
	char name[12];
	sceVu0FVECTOR Ambient;
	sceVu0FVECTOR Diffuse;
	sceVu0FVECTOR Specular;
	sceVu0FVECTOR Emission;
	u_int vifcode[4];
	u_long128 giftag;
	u_int Tex0[4];
	u_int Tex1[4];
	u_int Miptbp1[4];
	u_int Miptbp2[4];
} SgMaterial;

typedef struct {
	u_int VersionID;
	u_char MAPFLAG;
	u_char kind;
	u_short materials;
	SgCOORDUNIT *coordp;
	SgMaterial *matp;
	u_int *phead;
	u_int blocks;
	u_int **primitives;
} HeaderSection;
// 
// 
// void MappingVUVNData(u_int *intpointer, HeaderSection *hs);
// void MappingVUVNDataPreset(u_int *intpointer, int mtype, int gloops, int hsize);
// void MappingTYPE0(int gloops, u_int *prim);
// int MappingTYPE2(int gloops, u_int *prim);
// void MappingMeshData(u_int *intpointer, u_int *vuvnprim);
// void MappingCoordinateData(u_int *intpointer);
// void MappingVertexList(VERTEXLIST *vli);
// void SgMapRebuld(void *sgd_top);
// void SgMapUnit(void *sgd_top);
// u_int* GetPrimitiveAddr(void *sgd_top, int num);
// u_int* GetStartPacketAddr(void *sgd_top, int num);
// u_int* GetEndPacketAddr(void *sgd_top, int num);
// void PostChainPacket(u_int *dest, u_int *source);
// int GetModelKind(void *sgd_top);
// void SgSetTEX1Prim(u_int *prim, sceGsTex1 *ptex1);
// void SgSetTEX1(void *sgd_top);

#endif // GRAPHICS_GRAPH3D_SGSGD_H
