#ifndef MAIN_GLOB_H
#define MAIN_GLOB_H

#include "typedefs.h"

#include "glob_types.h"
#include "ingame/camera/camera_types.h"

extern u_int realtime_scene_flg;
extern u_int scene_bg_load_flg;
extern SYS_WRK sys_wrk;
extern TIME_WRK time_wrk;
extern OPTION_WRK opt_wrk;
extern OUTGAME_WRK outgame_wrk;
extern PAD_STRUCT pad[2];
extern u_short *key_now[32];
extern u_short *key_bak[32];
extern SgCAMERA camera;
extern INGAME_WRK ingame_wrk;
extern PLYR_WRK plyr_wrk;
extern ENE_WRK ene_wrk[4];
extern MAP_WRK map_wrk;
extern ROOM_WRK room_wrk;
extern u_char poss_item[200];
extern u_char poss_file[4][50];
extern u_char flm_exp_flg[5];
extern MAP_CAM_DAT map_cam_dat[500];
extern MAP_CAM_DAT map_cam_dat2[500];
extern MAP_CAM_DAT map_cam_dat3[300];
extern MAP_CAM_DAT map_cam_dat4[300];
extern PHOTO_WRK photo_wrk;
extern PFILE_WRK pfile_wrk;
extern CAM_CUSTOM_WRK cam_custom_wrk;
extern DEBUG_NUMBERS dbg_wrk;

#endif // MAIN_GLOB_H
