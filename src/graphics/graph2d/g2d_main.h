#ifndef GRAPHICS_GRAPH2D_G2D_MAIN_H
#define GRAPHICS_GRAPH2D_G2D_MAIN_H

#include "typedefs.h"

typedef struct {
	u_char dmg;
	u_char hint;
	u_char pazz;
	u_char eneface;
	u_char enedmg[4];
} G2D_LOAD_FLG;

extern G2D_LOAD_FLG g2d_load_flg;

void gra2dInitBG();
void gra2dInitST();
void gra2dDraw(int fl);

#endif // GRAPHICS_GRAPH2D_G2D_MAIN_H
