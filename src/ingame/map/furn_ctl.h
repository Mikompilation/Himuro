#ifndef INGAME_MAP_FURN_CTL_H
#define INGAME_MAP_FURN_CTL_H

#include "typedefs.h"

#include "graphics/graph3d/light_types.h"
#include "ingame/map/furn_types.h"

extern int furn_disp_flg;

void FurnCtrlMain();
void InitFurnDispFlg();
FURN_WRK* FurnGetFurnWrk(u_char fw_no);
u_char ChkFurnGetAlready(u_short furn_id);
void FurnSetWrkNoUse(FURN_WRK *fwp, int no);
int FurnSetFurnWork(FURN_DATA_POP *fdpp, int fw_no, u_char msn_no, u_char use, u_char room_id);
int FurnIsPicDispOnly(u_short id, u_char msn_no);
void FurnAplyAmbient();
int GetRoomFurnID(u_char room_id, u_short *furn_id, u_char msn_no);
int FurnIsWrkUse(FURN_WRK *fw);
int FurnIsSeUse(u_short id, u_char msn_no);
int FurnIsUsable(u_short id, u_char msn_no);
int FurnIsPicDisp(u_short id, u_char msn_no);
int FurnIsDisp(u_short id, u_char msn_no);
int FurnIsHit(u_short id, u_char msn_no);
void FurnPicExecStart();
void FurnPicExecEnd();
void FurnPicNoDspOff();
void FurnPicNoDspOn();
void FurnSortFurnWrk(u_char mode);
void FurnDataRenewNow();
void FurnFreeFurnWrk(u_char rm_id);
void FurnDataInit();
void FurnDataRenewNext(u_char room_id);
void FurnPicDispOn();
void FurnPicDispOff();
void ClearFurnAttrFlg();
void InitFurnAttrFlg();
void SetFurnAttrEve(u_short id, u_short flg, int on_flg);
u_int GetFurnAttrF(FURN_WRK *fwp, u_char msn_no);
u_int GetFurnAttr(u_short id, u_char msn_no);
u_char FurnHitCheck(u_char *dx_max, u_char *dz_max, float *pos, float *dst, u_char div, u_char room);
u_char FurnHitCheck2(u_short pos_x, u_short pos_y, u_char room_id);

#endif // INGAME_MAP_FURN_CTL_H
