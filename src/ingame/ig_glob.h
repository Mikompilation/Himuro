#ifndef INGAME_IG_GLOB_H
#define INGAME_IG_GLOB_H

#include "typedefs.h"
#include "ig_glob_types.h"

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
