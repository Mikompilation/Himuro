#ifndef INGAME_MAP_DOOR_CONTE_H
#define INGAME_MAP_DOOR_CONTE_H

#include "typedefs.h"

typedef enum {
	DR_CNT_IDOL = 0,
	DR_CNT_START = 1,
	DR_CNT_LOAD_WAIT = 2,
	DR_CNT_OPEN_STRT = 3,
	DR_CNT_DOR_OPEN = 4,
	DR_CNT_CLOSE_STRT = 5,
	DR_CNT_DOR_CLOSE = 6,
	DR_CNT_END = 7
} ST_DOOR_CONTE;

typedef struct {
    float adj_r0;
    float adj_r1;
    float adj_org;
    u_short adj_x0;
    u_short adj_x1;
    u_short adj_z0;
    u_short adj_z1;
    ST_DOOR_CONTE state;
    u_char timer;
    u_char door_mtn_dat_no;
    u_char mtn_no;
} DOOR_CONTE;

void DoorOpenConteInit();
u_char DoorOpenConte();

#endif // INGAME_MAP_DOOR_CONTE_H
