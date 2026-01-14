#ifndef INGAME_MAP_MAP_CTRL_H
#define INGAME_MAP_MAP_CTRL_H

#include "typedefs.h"

typedef struct {
    u_short camera_no;
    u_char room_id;
    u_char pad;
    u_short door_id[2];
    u_char sq_num;
} CAMERA4_DATA_POP;

// extern u_char floor_exist[5][4];
extern u_char room_pass[42];
// extern u_char gtmp_cam_type;
// extern u_char gtmp_room;
// extern u_short gtmp_cam_no;

int MissonMapDataLoad(u_char msn_no);
void InitMapStatus(u_char msn_no);
void MapDataInit();
void MapMissionChange(u_char new_msn);
void RoomPassInit();
void RoomPass(u_char room_id);
int IsRoomPass(u_char room_id);
void MapFloorChange(u_char new_floor);
int GetFloorTopAddr(u_char floor);
void InitMap();
void MapCtrlMain();
void GetNewRoomData();
u_char RoomWrkRenew();
void CameraNoRenewEach(u_char cam_type);
void DbgGetDrmCamNo();
u_short CameraGetDoorCameraNo(u_short door_id0, u_short door_id1);
u_char GetRoomIdFromRoomNo(u_char map, u_char room_no);
u_char GetRoomIdFromRoomNoFloor(u_char map, u_char room_no, u_char floor);
u_char GetDataRoom(u_char map, u_char room_id);
int GetPointRoomNo(u_short pos_x, u_short pos_z);
int GetPointRoomNoFloor(u_short pos_x, u_short pos_z, u_char floor);
int GetPointRoom(u_short pos_x, u_short pos_z);
float GetPointHeight(u_short pos_x, u_short pos_z);
u_char PosInAreaJudge0(u_char room, u_short pos_x, u_short pos_y);
u_char PosInAreaJudge1(u_char map, u_char room, u_char data, u_short pos_x, u_short pos_y);
u_char MapHitCheck(u_short pos_x, u_short pos_y, u_char room_id);
u_char MapHitCheckAll(u_short pos_x, u_short pos_y, u_char room_id);
void PlyrHitRot(sceVu0FVECTOR pos, sceVu0FVECTOR dst, float *go_rot, float *no_rot, u_char div, u_char room_id);
u_char PlyrMapHitCheck(sceVu0FVECTOR dst, sceVu0FVECTOR pos, u_char div, u_char room_id);
int PlyrMapHitMoveOfs(sceVu0FVECTOR dst, sceVu0FVECTOR pos, u_char room, u_char flr_room);
u_char GetPointMoveMotion(sceVu0FVECTOR p, u_char no);
u_char MapCameraCdivideAB(u_short data_no, u_short *xmin, u_short *xmax, u_short *ymin, u_short *ymax, u_char cam_type);
void NakasuHazeSet();
void DebugMemoryCheck(u_char *addr, int size);
int GetRoomPos(u_char room_no, sceVu0FVECTOR room_pos);

#endif // INGAME_MAP_MAP_CTRL_H
