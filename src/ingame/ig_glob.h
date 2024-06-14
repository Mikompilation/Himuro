#ifndef INGAME_IG_GLOB_H
#define INGAME_IG_GLOB_H

#include "typedefs.h"

typedef struct {
	void *addr;
	sceVu0FVECTOR pos0;
	sceVu0FVECTOR pos1;
	sceVu0FVECTOR rot0;
	u_char use;
} FURN_EFCT;

typedef struct {
	u_int flg;
	u_int padding;
} FURN_ATTR_FLG;

typedef struct {
	u_short mdl_no;
	u_short attr;
	u_char stts;
	u_char stts_map;
	u_char room_id;
	u_int padding;
} DOOR_STTS_KEEP;

typedef struct {
	u_short item_no;
	u_char disp;
	sceVu0FVECTOR pos;
} ITEM_DISP_WRK;


#include "ingame/enemy/move_ctl.h"

typedef struct {
	u_short sta;
	u_char ene;
	u_char reserve;
	MOVE_BOX move_box;
	FLY_DATA *dat;
	void *ep;
	sceVu0FVECTOR adjp;
	sceVu0FVECTOR adjmv;
} FLY_WRK;

#include "graphics/graph3d/gra3d.h"
#include "ingame/map/furn_ctl.h"

extern FURN_WRK furn_wrk[60];
extern FURN_EFCT fefct_wrk[60];
extern ITEM_DISP_WRK item_dsp_wrk[2][16];
extern u_char ene_cam_req_checker[50];
extern FURN_ATTR_FLG furn_attr_flg[500];
extern DOOR_STTS_KEEP door_keep[300];
extern FLY_WRK fly_wrk[10];
extern u_char ep_sensor[3];
extern int disp3d_room_req;
extern int disp3d_furn_req;


#endif // INGAME_IG_GLOB_H
