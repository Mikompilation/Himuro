#ifndef GRAPHICS_MOTION_ACS_DAT_H
#define GRAPHICS_MOTION_ACS_DAT_H

#include "typedefs.h"

typedef struct {
	float center[3];
	float r;
} SPHERE;

typedef struct {
	float p0[3];
	float p1[3];
	float r;
	u_int axis;
} TUBE;

typedef struct {
	void *dat;
	u_char type;
	u_char bone_id;
} COLLISION_DAT;

typedef struct {
	float *vtx;
	float gravity;
	float Kd;
	float Ke;
	u_int fixed_num;
} ROPE_DAT;

typedef struct {
	float gravity;
	float Kd;
	float Ke;
	u_char w;
	u_char h;
	u_char type;
} CLOTH;

typedef struct {
	CLOTH *dat;
	sceVu0FVECTOR *rist_vtx;
	u_char bone_id;
	u_char bone_id2;
	u_char sgd_id;
	u_char flg;
} CLOTH_DAT;

typedef struct {
	sceVu0FVECTOR p;
	sceVu0FVECTOR old;
	sceVu0FVECTOR v;
	sceVu0FVECTOR f;
} C_PARTICLE;

typedef struct {
	u_short p1;
	u_short p2;
	float ldef;
} SPRING;

typedef struct {
	C_PARTICLE *particle;
	SPRING *spring;
	CLOTH_DAT *cdat;
	u_short p_num;
	u_short spring_num;
	u_char w;
	u_char h;
	u_char reset_flg;
	u_char pad;
} CLOTH_CTRL;

typedef struct {
	short int alpha;
	short int stat;
} ACS_ALPHA;

typedef struct {
	u_int type;
	u_int furn_id;
	u_int timer;
	u_int req;
	u_char alpha;
} CMOVE_CTRL;

typedef struct {
	u_char *name;
	float mass;
	float Ks;
	float dec;
	sceVu0FVECTOR pos;
	sceVu0FVECTOR gravity;
} WMIM;

typedef struct {
	WMIM *dat;
	u_char bone_id;
} WMIM_DAT;

// extern SPHERE sphere_R100;
// extern SPHERE sphere_R150;
// extern SPHERE sphere_R200;
// extern TUBE tube_R150_H200;
// extern TUBE tube_R90_H200;
// extern WMIM mimacs_ribon_dat;
// extern WMIM mimacs_hair_dat;
// extern WMIM_DAT m000_wmim_tbl[0];
// extern WMIM mimacs_hair00_dat;
// extern WMIM mimacs_hair01_dat;
// extern WMIM_DAT m021_wmim_tbl[0];
// extern WMIM_DAT m023_wmim_tbl[0];
// extern WMIM mimacs_lhand_dat;
// extern WMIM mimacs_rhand_dat;
// extern WMIM_DAT m027_wmim_tbl[0];
// extern WMIM_DAT m028_wmim_tbl[0];
// extern float rope_vtx0[0];
// extern float rope_vtx1[0];
// extern float rope_vtx2[0];
// extern ROPE_DAT rope_dat0;
// extern ROPE_DAT rope_dat1;
// extern ROPE_DAT rope_dat2;
// extern float rope_vtx3[0];
// extern ROPE_DAT rope_dat3;
// extern float rope_vtx4[0];
// extern ROPE_DAT rope_dat4;
// extern float rope_vtx5[0];
// extern ROPE_DAT rope_dat5;
// extern float rope_vtx6[0];
// extern ROPE_DAT rope_dat6;
// extern float rope_vtx7[0];
// extern ROPE_DAT rope_dat7;
// extern float rope_vtx8[0];
// extern float rope_vtx9[0];
// extern float rope_vtx10[0];
// extern ROPE_DAT rope_dat8;
// extern ROPE_DAT rope_dat9;
// extern ROPE_DAT rope_dat10;
extern ROPE_DAT *rope_tbl[];
// extern CLOTH cloth_manto0;
// extern CLOTH cloth_manto1;
// extern CLOTH cloth_manto2;
// extern CLOTH cloth_manto3;
// extern CLOTH cloth_manto4;
// extern CLOTH cloth_manto5;
// extern CLOTH cloth_manto6;
// extern CLOTH cloth_nawa0;
// extern CLOTH cloth_nawa1;
// extern CLOTH cloth_furisode;
// extern COLLISION_DAT m000_collision[0];
// extern sceVu0FVECTOR m006_rist_vtx0[0];
// extern sceVu0FVECTOR m006_rist_vtx1[0];
// extern CLOTH_DAT m006_cloth[0];
// extern COLLISION_DAT m006_collision[0];
// extern sceVu0FVECTOR m015_rist_vtx0[0];
// extern sceVu0FVECTOR m015_rist_vtx1[0];
// extern CLOTH_DAT m015_cloth[0];
// extern COLLISION_DAT m015_collision[0];
// extern sceVu0FVECTOR m016_rist_vtx0[0];
// extern sceVu0FVECTOR m016_rist_vtx1[0];
// extern CLOTH_DAT m016_cloth[0];
// extern sceVu0FVECTOR m017_rist_vtx0[0];
// extern sceVu0FVECTOR m017_rist_vtx1[0];
// extern CLOTH_DAT m017_cloth[0];
// extern CLOTH_DAT m019_cloth[0];
// extern sceVu0FVECTOR m020_rist_vtx0[0];
// extern sceVu0FVECTOR m020_rist_vtx1[0];
// extern CLOTH_DAT m020_cloth[0];
// extern sceVu0FVECTOR m023_rist_vtx0[0];
// extern CLOTH_DAT m023_cloth[0];
// extern sceVu0FVECTOR m024_rist_vtx0[0];
// extern sceVu0FVECTOR m024_rist_vtx1[0];
// extern sceVu0FVECTOR m024_rist_vtx2[0];
// extern sceVu0FVECTOR m024_rist_vtx3[0];
// extern CLOTH_DAT m024_cloth[0];
// extern sceVu0FVECTOR m026_rist_vtx0[0];
// extern sceVu0FVECTOR m026_rist_vtx1[0];
// extern CLOTH_DAT m026_cloth[0];
// extern sceVu0FVECTOR m029_rist_vtx0[0];
// extern sceVu0FVECTOR m029_rist_vtx1[0];
// extern CLOTH_DAT m029_cloth[0];
// extern sceVu0FVECTOR m032_rist_vtx0[0];
// extern sceVu0FVECTOR m032_rist_vtx1[0];
// extern CLOTH_DAT m032_cloth[0];
// extern COLLISION_DAT m032_collision[0];
// extern sceVu0FVECTOR m033_rist_vtx0[0];
// extern sceVu0FVECTOR m033_rist_vtx1[0];
// extern CLOTH_DAT m033_cloth[0];
// extern sceVu0FVECTOR m034_rist_vtx0[0];
// extern sceVu0FVECTOR m034_rist_vtx1[0];
// extern CLOTH_DAT m034_cloth[0];
// extern CLOTH_DAT m035_cloth[0];
// extern sceVu0FVECTOR m036_rist_vtx0[0];
// extern CLOTH_DAT m036_cloth[0];
// extern sceVu0FVECTOR m037_rist_vtx0[0];
// extern sceVu0FVECTOR m037_rist_vtx1[0];
// extern CLOTH_DAT m037_cloth[0];
// extern COLLISION_DAT m037_collision[0];
// extern CLOTH_DAT m038_cloth[0];
// extern sceVu0FVECTOR m040_rist_vtx0[0];
// extern sceVu0FVECTOR m040_rist_vtx1[0];
// extern CLOTH_DAT m040_cloth[0];
// extern CLOTH_DAT m051_cloth[0];
// extern CLOTH_DAT m058_cloth[0];
// extern CLOTH_DAT m059_cloth[0];

#endif // GRAPHICS_MOTION_ACS_DAT_H
