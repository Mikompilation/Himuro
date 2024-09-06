#ifndef GRAPHICS_GRAPH2D_SPRT_H
#define GRAPHICS_GRAPH2D_SPRT_H

#include "typedefs.h"

typedef struct {
	short int u;
	short int v;
	short int w;
	short int h;
	short int x;
	short int y;
	u_char pri;
	u_char alp;
} SPRT_SDAT;

typedef struct {
	short int cx;
	short int cy;
	float rot;
} SPRT_SROT;

typedef struct {
	short int cx;
	short int cy;
	short int sw;
	short int sh;
} SPRT_SSCL;

#endif // GRAPHICS_GRAPH2D_SPRT_H