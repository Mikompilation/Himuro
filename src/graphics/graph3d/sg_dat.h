#ifndef GRAPHICS_GRAPH3D_SG_DAT_H
#define GRAPHICS_GRAPH3D_SG_DAT_H

#include "typedefs.h"

typedef struct {
	int num;
	int lnum[3];
} MatCache;

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

typedef struct SgCOORDUNIT SgCOORDUNIT;

struct SgCOORDUNIT {
	sceVu0FMATRIX matrix;
	sceVu0FMATRIX lwmtx;
	sceVu0FMATRIX workm;
	sceVu0FVECTOR rot;
	SgCOORDUNIT *parent;
	u_int flg;
	u_int edge_check;
	u_int camin;
};

typedef struct {
	sceVu0FVECTOR pos;
	sceVu0FVECTOR direction;
	sceVu0FVECTOR ldirection;
	sceVu0FVECTOR spvector;
	sceVu0FVECTOR diffuse;
	sceVu0FVECTOR specular;
	sceVu0FVECTOR ambient;
	float intens;
	float intens_b;
	float power;
	float btimes;
	float spower;
	int Enable;
	int SEnable;
	int num;
} SgLIGHT;

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

typedef struct { // 0x90
	/* 0x00 */ u_int primtype;
	/* 0x04 */ char name[12];
	/* 0x10 */ sceVu0FVECTOR Ambient;
	/* 0x20 */ sceVu0FVECTOR Diffuse;
	/* 0x30 */ sceVu0FVECTOR Specular;
	/* 0x40 */ sceVu0FVECTOR Emission;
	/* 0x50 */ int cache_on;
	/* 0x54 */ u_int tagd_addr;
	/* 0x58 */ int qwc;
	/* 0x5c */ MatCache Parallel;
	/* 0x6c */ MatCache Point;
	/* 0x7c */ MatCache Spot;
} SgMaterialC;

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

typedef struct {
    short int vnum;
    short int nnum;
    char vif_size;
    char vtype;
} VUVN_PRIM;

#endif // GRAPHICS_GRAPH3D_SG_DAT_H
