#include "typedefs.h"
#include "ig_glob.h"

FURN_WRK furn_wrk[60] = {0};
FURN_EFCT fefct_wrk[60] = {0};
ITEM_DISP_WRK item_dsp_wrk[2][16] = {0};
u_char ene_cam_req_checker[50] = {0};
FURN_ATTR_FLG furn_attr_flg[500] = {0};
DOOR_STTS_KEEP door_keep[300] = {0};
FLY_WRK fly_wrk[10] = {0};
u_char ep_sensor[3] = { 0, 0, 0 };
int disp3d_room_req = 0;
int disp3d_furn_req = 0;
