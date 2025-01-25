#ifndef GRAPHICS_MOTION_MOT_DAT_H
#define GRAPHICS_MOTION_MOT_DAT_H

#include "typedefs.h"

typedef struct {
	sceVu0FVECTOR rot;
	sceVu0FVECTOR scale;
	sceVu0FVECTOR trans;
} RST_DATA;

typedef struct {
	u_int *top;
	u_int *dat;
	u_int play_id;
	int cnt;
	int all_cnt;
	int inp_allcnt;
	int inp_cnt;
	int reso;
	int reso_cnt;
	RST_DATA *rst0;
	RST_DATA *rst1;
} MOT_CTRL;

#endif // GRAPHICS_MOTION_MOT_DAT_H
