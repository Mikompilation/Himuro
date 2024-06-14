#include "glob.h"

u_int realtime_scene_flg = 0; // sdata
u_int scene_bg_load_flg = 0; // sdata
SYS_WRK sys_wrk = {0};
TIME_WRK time_wrk = {0};
OPTION_WRK opt_wrk = {0};
OUTGAME_WRK outgame_wrk = {0};
PAD_STRUCT pad[2] = {0};
u_short *key_now[32] = {0};
u_short *key_bak[32] = {0};
SgCAMERA camera = {0};
INGAME_WRK ingame_wrk = {0};
PLYR_WRK plyr_wrk = {0};
ENE_WRK ene_wrk[4] = {0};
MAP_WRK map_wrk = {0};
ROOM_WRK room_wrk = {0};
u_char poss_item[200] = {0};
u_char poss_file[4][50] = {0};
u_char flm_exp_flg[5] = {0}; // sdata
MAP_CAM_DAT map_cam_dat[500] = {0};
MAP_CAM_DAT map_cam_dat2[500] = {0};
MAP_CAM_DAT map_cam_dat3[300] = {0};
MAP_CAM_DAT map_cam_dat4[300] = {0};
PHOTO_WRK photo_wrk = {0};
PFILE_WRK pfile_wrk = {0};
CAM_CUSTOM_WRK cam_custom_wrk = {0};
DEBUG_NUMBERS dbg_wrk = {0};
