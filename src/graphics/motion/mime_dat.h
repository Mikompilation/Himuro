#ifndef GRAPHICS_MOTION_MIME_DAT_H
#define GRAPHICS_MOTION_MIME_DAT_H

#include "typedefs.h"

#include "graphics/motion/acs_dat.h"

typedef struct {
	sceVu0FVECTOR *vtx;
	sceVu0FVECTOR *pkt;
	u_int *dat;
	u_int vtx_num;
	u_int p_no;
} MIME_DAT;

typedef struct {
	WMIM_DAT *wdat;
	sceVu0FVECTOR v;
	sceVu0FVECTOR w;
	sceVu0FVECTOR pbak;
} WMIM_CTRL;

typedef struct {
	MIME_DAT *mdat;
	int frame;
	u_short furn_id;
	u_short cnt;
	u_char weight_id;
	u_char parts_id;
	u_char reso;
	u_char loop;
	u_char stat;
	u_char rev;
	u_char flg;
	u_char pad;
} MIME_CTRL;

#endif // GRAPHICS_MOTION_MIME_H
