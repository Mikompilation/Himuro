#ifndef INGAME_MAP_FURN_CTL_H
#define INGAME_MAP_FURN_CTL_H

#include "typedefs.h"

#include "graphics/graph3d/light_types.h"
#include "ingame/map/furn_types.h"

typedef struct {
    u_int ck_item;
    u_int ck_rate;
    float distn;
    float distf;
    float rot_range;
    float vlow;
    float vhigh;
    u_int delay;
    u_int stat;
    u_short obj;
} FACT_CHK_DAT;

typedef struct {
    u_short type;
    u_short no;
} FACT_EXEC_DAT;

typedef struct {
    FACT_CHK_DAT *chk[2];
    FACT_EXEC_DAT *exec[2];
    float dist_tgt;
    float fade_rate;
    float fade;
    u_int glb_cnt;
    u_int dly_cnt;
    u_int exe_cnt;
    u_int attr;
    int fw_no;
    int exec_rate;
    int hndl;
    u_short furn_id;
    u_char stts;
    u_char exec_mode[2];
    u_char exec_flg;
    u_char eve_flg;
    u_char room_id;
} F_ACT_WRK;

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
u_char FurnHitCheck(u_char *dx_max, u_char *dz_max, sceVu0FVECTOR pos, sceVu0FVECTOR dst, u_char div, u_char room);
u_char FurnHitCheck2(u_short pos_x, u_short pos_y, u_char room_id);

#endif // INGAME_MAP_FURN_CTL_H
