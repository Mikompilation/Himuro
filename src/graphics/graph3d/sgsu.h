#ifndef GRAPHICS_GRAPH3D_SGSU_H
#define GRAPHICS_GRAPH3D_SGSU_H

#include "typedefs.h"

typedef struct {
	long unsigned int TBP0 : 14;
	long unsigned int TBW : 6;
	long unsigned int PSM : 6;
	long unsigned int TW : 4;
	long unsigned int TH : 4;
	long unsigned int TCC : 1;
	long unsigned int TFX : 2;
	long unsigned int CBP : 14;
	long unsigned int CPSM : 4;
	long unsigned int CSM : 1;
	long unsigned int CSA : 5;
	long unsigned int CLD : 3;
} sceGsTex0;

typedef struct {
	u_int HeaderSections;
	u_int UniqHeaderSize;
	sceVu0FVECTOR *pUniqVertex;
	sceVu0FVECTOR *pUniqNormal;
	u_int *pUniqList;
	u_int CommonHeaderSize;
	sceVu0FVECTOR *pCommonVertex;
	sceVu0FVECTOR *pCommonNormal;
	u_int *pCommonList;
	u_int WeightedHeaderSize;
	sceVu0FVECTOR *pWeightedVertex;
	sceVu0FVECTOR *pWeightedNormal;
	u_int *pWeightedList;
} PHEAD;

// extern SgCOORDUNIT *lcp;
// extern PHEAD *lphead;
// extern u_int nextprim;
// extern u_int *vuvnprim;
// extern int blocksm;
// extern int write_coord;

void _AddColor(float *v);
void SgSuDebugOn();
void SgSuDebugOff();
void SetDebugSign(int num);
void PutDebugSign();
void SgSetVNBuffer(sceVu0FVECTOR *vnarray, int size);
int CheckCoordCache(int cn);
void SetVU1Header();
void CalcVertexBuffer(u_int *prim);
u_int* SetVUVNData(u_int *prim);
u_int* SetVUVNDataPost(u_int *prim);
void printTEX0(sceGsTex0 *tex0);
void SetVUMeshData(u_int *prim);
void SetVUMeshDataPost(u_int *prim);
void SetCoordData(u_int *prim);
void SgSortUnitPrim(u_int *prim);
void SgSortUnitPrimPost(u_int *prim);
void SgSortPreProcess(u_int *prim);
void AppendVUProgTag(u_int *prog);
void LoadSgProg(int load_prog);
void SetUpSortUnit();
void SgSortUnit(void *sgd_top, int pnum);
void SgSortUnitKind(void *sgd_top, int num);
void _SetLWMatrix0();
void _SetLWMatrix1();
void _SetRotTransPersMatrix();
void _CalcVertex(float *dp, float *n);
void _vfito0(int *v0);

#endif // GRAPHICS_GRAPH3D_SGSU_H
