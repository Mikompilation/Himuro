#ifndef INGAME_MAP_FURN_CTL_H
#define INGAME_MAP_FURN_CTL_H

#include "typedefs.h"

#include "graphics/graph3d/gra3d.h"

typedef struct {
	u_short furn_no;
	short int stts;
	sceVu0FVECTOR pos;
	sceVu0FVECTOR rotate;
	float rot;
	int fewrk_no;
	u_short id;
	u_short fno_bk;
	u_short dist;
	u_short score;
	float ratio;
	u_char use;
	u_char cmdflg;
	u_short attr_id;
	u_char room_id;
	u_char fs_flg;
	LIGHT_PACK mylight;
} FURN_WRK;

typedef struct {
	float rot_y;
	float rot_x;
	u_short pos_x;
	short int pos_y;
	u_short pos_z;
	u_short attr_id;
	u_short model_no;
	u_short id;
	short int top;
	short int btm;
	u_char snum;
} FURN_DATA_POP;

// extern int furn_disp_flg;

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
