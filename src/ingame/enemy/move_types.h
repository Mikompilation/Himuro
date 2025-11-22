#ifndef INGAME_ENEMY_MOVE_TYPES_H
#define INGAME_ENEMY_MOVE_TYPES_H

#include "typedefs.h"

typedef struct {
	u_char job_no;
	u_char pos_no;
	u_char wait_time;
	u_char idx;
	u_short loop;
	u_short reserve;
	int se;
	sceVu0FVECTOR pos;
	sceVu0FVECTOR tpos;
	sceVu0FVECTOR spd;
	sceVu0FVECTOR rot;
	sceVu0FVECTOR rspd;
	sceVu0FVECTOR trot;
	sceVu0FVECTOR pos_mid;
	P_INT comm_add;
	long int comm_add_top;
} MOVE_BOX;

#endif // INGAME_ENEMY_MOVE_TYPES_H
