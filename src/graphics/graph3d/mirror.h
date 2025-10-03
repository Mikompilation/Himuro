#ifndef GRAPHICS_GRAPH3D_MIRROR_H
#define GRAPHICS_GRAPH3D_MIRROR_H

#include "typedefs.h"

typedef struct {
	int mask;
	int xyz;
	float sgn;
} ClipData;

typedef struct
{ // 0x20
    /* 0x00 */ sceVu0FVECTOR vp;
    /* 0x10 */ sceVu0FVECTOR clip;
} MNODE;

typedef struct
{ // 0x290
    /* 0x000 */ MNODE node0[10];
    /* 0x140 */ MNODE node1[10];
    /* 0x280 */ MNODE *in;
    /* 0x284 */ MNODE *out;
    /* 0x288 */ int nodes;
} MFlipNode;

#include "ingame/camera/camera.h"

extern sceVu0FMATRIX mir_mtx;
extern sceVu0FVECTOR mir_norm;
extern sceVu0FVECTOR mir_center;
extern sceVu0FVECTOR mirror_lpos[5];
extern sceVu0FVECTOR mirror_cval[5];
extern int mirror_points;

int CheckMirrorModel(void *sgd_top);
void MirrorInterPNode(MNODE *dst, MNODE *inner, MNODE *outer, ClipData *cldata);
void SliceMirrorPolygon(MFlipNode *fn, ClipData *cldata);
void CalcScreenMirror(sceVu0FVECTOR vp0, sceVu0FVECTOR vp1, sceVu0FVECTOR vp2, float sgn);
void AppendLocalMPos(sceVu0FVECTOR vp);
int MakeMirrorEnvironment(u_int *prim);
void MirrorPrim(u_int *prim);
int PreMirrorPrim(SgCAMERA *camera, u_int *prim);
void MirrorBufferFlush(int tlen);
void CalcMirrorMatrix(SgCAMERA *camera);
// void MirrorDraw(SgCAMERA *camera, void *sgd_top, void (*render_func)(/* parameters unknown */));
void MirrorRender(SgCAMERA *camera, void (*render_func)(/* parameters unknown */));

#endif // GRAPHICS_GRAPH3D_MIRROR_H
