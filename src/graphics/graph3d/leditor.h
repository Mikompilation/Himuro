#ifndef GRAPHICS_GRAPH3D_LEDITOR_H
#define GRAPHICS_GRAPH3D_LEDITOR_H

#include "typedefs.h"

#include "graphics/graph3d/sglight.h"
#include "graphics/graph2d/g2d_debug.h"

typedef struct {
    SgLIGHT *lights;
    SgLIGHT *plights;
    SgLIGHT *slights;
    int lnum;
    int pnum;
    int snum;
    int lmax;
    int pmax;
    int smax;
    u_int *ldata;
    u_int *pdata;
    u_int *sdata;
} ROOM_LIGHT;

void ReadLights(ROOM_LIGHT *rdata, void *buf);
void ApplyLight(int room_no);
void MakeLightEditorData(DEBUG_MENU **dbgmenu_tbl, int now_tree);
void MakeFogData(DEBUG_SUB_MENU **dbgmenu_tbl, int pos);
void ApplyFogData();

#endif // GRAPHICS_GRAPH3D_LEDITOR_H
