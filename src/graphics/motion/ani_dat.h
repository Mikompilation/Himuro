#ifndef GRAPHICS_MOTION_ANI_DAT_H
#define GRAPHICS_MOTION_ANI_DAT_H

#include "typedefs.h"

#include "graphics/motion/mime_dat.h"
#include "graphics/motion/mot_dat.h"

typedef u_short ANI_CODE;

typedef struct {
    ANI_CODE code;
    u_short cnt;
    u_int stat;
} ANI_BUF;

typedef struct {
	int playnum;
	int stat;
	int loop_rest;
	int timer;
	ANI_BUF buf[10];
	ANI_CODE *code_head;
	ANI_CODE *code_now;
	ANI_CODE *loop_start;
} ANI_CODE_CTRL;

typedef struct {
	ANI_CODE_CTRL anm;
	MOT_CTRL mot;
	sceVu0FVECTOR pbak;
	u_int *mdl_p;
	u_int *pk2_p;
	u_int *mpk_p;
	u_int *base_p;
	u_int *anm_p;
	u_int *mtop;
	u_int *mdat;
	u_int *tanm_p;
	MIME_CTRL *mim;
	MIME_CTRL *bgmim;
	WMIM_CTRL *wmim;
	CLOTH_CTRL *cloth_ctrl;
	u_int mot_num;
	u_int mim_num;
	u_int bg_num;
	u_int wmim_num;
	u_short ftype;
	u_short interp_flg;
} ANI_CTRL;

typedef struct {
	u_char map_flg;
	u_short anm_no;
	u_int *anm_p;
	u_char pkt_no;
} ANI_MDL_CTRL;

#endif // GRAPHICS_MOTION_ANI_DAT_H
