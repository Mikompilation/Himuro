#ifndef INGAME_MAP_MAP_AREA_H
#define INGAME_MAP_MAP_AREA_H

#include "typedefs.h"

typedef struct {
	u_int *sgd_room;
	u_int *sgd_furn[30];
	u_short mno_room;
	u_short mno_furn[30];
	u_char room_id;
} ROOM_MDATA;

typedef struct {
	u_int *sgd_door;
	u_short mno_door;
} DOOR_MDATA;

typedef struct {
	ROOM_MDATA rmd[6];
	DOOR_MDATA dmd[20];
	sceVu0FVECTOR tmp_after_pos;
	u_char area_no;
	u_char area_bak;
	u_char room[6];
	u_char rgst[5];
	u_char ev_se;
	u_char fg_max;
	u_int padding;
} AREA_WRK;

// extern u_char area_dat[11][6];
// extern u_char one_room_load_ok;
extern AREA_WRK area_wrk;

void MapAreaInit();
void MapAreaMain();
int MapAreaRoomLoadReq();
void MapAreaLoadSet();
u_char GetRoomArea(u_char room_id);
int SetAreaLoadRoom(u_char mode, u_char area_no);
void AreaRoomAllLoadInit();
int AreaRoomAllLoad(u_char area_no);
int GetEmptyRoomAddr();
void ClearDispRoom(int wrk_no);

#endif // INGAME_MAP_MAP_AREA_H
