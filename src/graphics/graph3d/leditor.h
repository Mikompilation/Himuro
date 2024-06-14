#ifndef GRAPHICS_GRAPH3D_LEDITOR_H
#define GRAPHICS_GRAPH3D_LEDITOR_H

#include "typedefs.h"

#include "graphics/graph3d/sglight.h"

typedef struct
{ // 0x30
    /* 0x00 */ SgLIGHT *lights;
    /* 0x04 */ SgLIGHT *plights;
    /* 0x08 */ SgLIGHT *slights;
    /* 0x0c */ int lnum;
    /* 0x10 */ int pnum;
    /* 0x14 */ int snum;
    /* 0x18 */ int lmax;
    /* 0x1c */ int pmax;
    /* 0x20 */ int smax;
    /* 0x24 */ u_int *ldata;
    /* 0x28 */ u_int *pdata;
    /* 0x2c */ u_int *sdata;
} ROOM_LIGHT;

#include "graphics/graph2d/g2d_debug.h"

void ReadLights(ROOM_LIGHT *rdata, void *buf);
void ApplyLight(int room_no);
void MakeLightEditorData(int now_tree);
void MakeFogData(DEBUG_SUB_MENU **dbgmenu_tbl, int pos);
void ApplyFogData();

#endif // GRAPHICS_GRAPH3D_LEDITOR_H
