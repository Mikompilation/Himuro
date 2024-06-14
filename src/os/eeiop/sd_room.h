#ifndef OS_EEIOP_SD_ROOM_H
#define OS_EEIOP_SD_ROOM_H

#include "typedefs.h"

typedef struct {
	int file_no;
	u_short vol;
} ADPCM_ROOM_PLAY;

typedef struct {
	u_short rvol;
	u_char door_no;
	u_char foot_no;
	u_char srund_no;
	u_char event_no;
	u_char adpcm[5];
} ROOM_SOUND_INFO;

// extern ADPCM_ROOM_PLAY adpcm_param_tbl[0];
// extern int foot_se_index[0];

ROOM_SOUND_INFO* GetSdrDatP(u_char room_id);
int GetSdrAdpcmFno(u_char room_id);
u_short GetSdrAdpcmVol(u_char room_id);
u_short GetSdrReverbVol(u_char room_id);
void SetRoomReverbVol(u_char room_id);
u_int* GetSdrDoorSeTblP(u_char room_id);
u_int* GetSdrFootSeTblP(u_char room_id);
u_int GetSdrSrundSe(u_char room_id);
u_int GetSdrEventSe(u_char room_id);

#endif // OS_EEIOP_SD_ROOM_H
