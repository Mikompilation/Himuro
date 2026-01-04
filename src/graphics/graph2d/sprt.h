#ifndef GRAPHICS_GRAPH2D_SPRT_H
#define GRAPHICS_GRAPH2D_SPRT_H

#include "typedefs.h"

typedef struct {
	u_long tex0;
	u_short u;
	u_short v;
	u_short w;
	u_short h;
	int x;
	int y;
	int pri;
	u_char alpha;
} SPRT_DAT;

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

typedef struct {
	u_int att;
	u_int u;
	u_int v;
	u_int w;
	u_int h;
	float crx;
	float cry;
	float csx;
	float csy;
	float x;
	float y;
	u_int z;
	float scw;
	float sch;
	float rot;
	u_long gftg;
	u_long tex0;
	u_long tex1;
	u_long texa;
	u_long alphar;
	u_long zbuf;
	u_long test;
	u_int pri;
	u_char r;
	u_char g;
	u_char b;
	u_char alpha;
	u_char col;
} DISP_SPRT;


typedef struct {
	u_int att;
	float crx;
	float cry;
	float csx;
	float csy;
	int x[4];
	int y[4];
	u_int z;
	float scw;
	float sch;
	float rot;
	u_long texa;
	u_long alphar;
	u_long zbuf;
	u_long test;
	u_int pri;
	u_char r[4];
	u_char g[4];
	u_char b[4];
	u_char alpha;
} DISP_SQAR;

typedef struct {
	u_int w;
	u_int h;
	int x;
	int y;
	u_int pri;
	u_char r;
	u_char g;
	u_char b;
	u_char alpha;
} SQAR_DAT;

typedef struct {
	u_int w;
	u_int h;
	int x;
	int y;
	u_int pri;
	u_char r[4];
	u_char g[4];
	u_char b[4];
	u_char alpha;
} GSQR_DAT;

typedef struct {
	int x[4];
	int y[4];
	u_int pri;
	u_char r;
	u_char g;
	u_char b;
	u_char alpha;
} SQR4_DAT;

typedef struct {
	int x[4];
	int y[4];
	u_int pri;
	u_char r[4];
	u_char g[4];
	u_char b[4];
	u_char alpha;
} GSQ4_DAT;

#endif // GRAPHICS_GRAPH2D_SPRT_H
