#ifndef INGAME_MAP_DOOR_CTL_H
#define INGAME_MAP_DOOR_CTL_H

#include "typedefs.h"

typedef struct {
	u_char stat;
	u_char rm_from;
	u_char rm_to;
	u_char next_area;
	u_char room[6];
	u_char fgst[3];
	u_char rgst[5];
	u_char load_mode;
	u_char load_count;
	u_char load_id;
} AREA_READ_WRK;

typedef struct {
	float rot;
	u_short pos_x;
	u_short pos_z;
	u_char stts;
} DOOR_STTS_MAP;

typedef struct {
	sceVu0FVECTOR spos;
	sceVu0FVECTOR epos;
	float srot;
	float erot;
	float door_pre;
	u_char stts;
	u_char mode;
	u_short move_flame;
	u_short mtn_flame;
	u_short wait_flame;
	u_short count;
	u_char mtn_dat_no;
	u_char open[2];
	u_char chk_flg;
} DOOR_OPEN_CTRL;

typedef struct {
	sceVu0FVECTOR pos;
	sceVu0FVECTOR npos;
	float rot;
	float nrot;
	float *open_tbl;
	u_short door_id;
	u_short count;
	u_short mdl_no;
	u_short type;
	u_char fwrk_no;
	u_char stts;
	u_char use;
	u_char dmd_no;
	u_char hchk_mode;
	u_char lock;
} DOOR_WRK;

typedef struct {
	float *open_tbl;
	sceVu0FVECTOR npos;
	float nrot;
	float inert;
	u_char stts;
	u_short type;
	u_short door_id;
	u_short count;
	u_short dw_no;
	u_short stop_cnt;
	u_char dmd_no;
	u_char opnmtn_flg;
	u_char reopen;
} AUTO_CLOSE_DOOR_WRK;

typedef struct {
	float *open_tbl;
	sceVu0FVECTOR npos;
	float nrot;
	float inert;
	u_char stts;
	u_short type;
	u_short door_id;
	u_short count;
	u_short dw_no;
	u_char dmd_no;
	u_char opnmtn_flg;
} NON_CLOSE_DOOR_WRK;

extern AREA_READ_WRK area_read_wrk;
// extern DOOR_OPEN_CTRL door_open_ctrl;
extern DOOR_WRK door_wrk[20];
// extern AUTO_CLOSE_DOOR_WRK ac_door_wrk[10];
// extern NON_CLOSE_DOOR_WRK nc_door_wrk[50];
// extern DOJ_SQUARE_MTN doj_square_mtn;
// extern sceVu0FVECTOR tmp_area_end_door_pos;

u_char IsUseDoor(u_char use);
short int SearchDoorWrk(int door_id);
void ClearDoorAttrFlg();
void InitDoorAttrFlg();
void DoorPassRoom(u_char room_id);
void DoorRDoorSetRotOK(u_short door_id);
void DoorPassSetSttsMap(u_short door_id);
void DoorPassSetHide(u_short door_id);
void DoorPassSetTouch(u_short door_id);
void DoorAcInit();
void DoorCtrlInit();
void InitAreaReadWrk();
void DoorDataInit();
void DoorSetAllLocked();
int IsInDoorSpuare();
u_char GetRoomIdBeyondDoor2();
u_char GetRoomIdBeyondDoor(u_short door_id, u_char now_room_id);
u_char IsAllConnectDoorClose();
void LockAllDoorInNowRoom();
void UnlockAllDoorInNowRoom();
u_char IsDoorLoad(u_char type);
int GetRoomDoorID(u_char room_id, u_short *door_id, u_char msn_no);
void DoorCtrlMain();
u_char IsDoorOpenCtrlMode();
void SetPlyrPosAfterDOpen();
void AllCloseAcDoor();
void AllCloseConnectDoor();
void DoorOpenCheckMain();
void DoorCheckOn(u_char mode);
void DoorCheckOff();
u_char DoorOpenJudge();
void SetDoorOpenCameraNo();
int AreaReadCheck();
int DoorKeyLock(u_short door_id);
int DoorKeyLockOnly(u_short door_id);
int DoorKeyUnlock(u_short door_id);
int DoorKeyUnlockOnly(u_short door_id);
int DoorSttsChange(u_short door_id, u_char stts);
u_char DoorHitCheck(u_char *dx_max, u_char *dz_max, float *dst, float *ppos, u_char div);
u_char DoorHitCheck2(u_short pos_x, u_short pos_y, u_char room_id);
u_char DoorCoverCheck(u_short pos_x, short int pos_y, u_short pos_z, u_char room_id);
void DoorDataRenewNow();
void DoorFreeFurnWrk(u_char room_id);
void DoorJoint2Next();
void DoorDataRenewNext(u_char room_id);
u_char GetNextRIdFromRNoDId(u_char room_no, u_short door_id);
u_char NextRoomRenew();
u_char ChkNextRoomIsInsite();
u_char* GetDoorRoomNumP();
u_char* GetDoorRoomNumPFloor(u_char floor);
u_char* GetDoorRoomConectDataP(u_char room_id);
int ChkDoorIsInsite(float *dpos, float dpre_rot, u_short door_id);
int ChkDoorIsInsite2(float *dpos, float dpre_rot, u_short door_id);
int TmpMapHit(u_short door_id);
int GetDoorSttsForMap(DOOR_STTS_MAP *dsmp, u_short door_id, u_char floor);
u_short DoorChangeModel(u_short door_id, u_short model_no);
void DoorDispOnOff(u_short door_id, u_char on_off);
void DoorOpenShiftForce(u_short door_id);
char TargetDoorKindChk();
void PlyrPosAdj(u_char mtn_no, u_char mtn_type);
void DoorMoveStart(u_char mtn_no, u_char door_close_flg);
void OneRoomLoadReq();
void PrintDoorID(int no);

#endif // INGAME_MAP_DOOR_CTL_H
