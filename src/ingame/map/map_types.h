#ifndef INGAME_MAP_MAP_TYPES_H
#define INGAME_MAP_MAP_TYPES_H

#include "typedefs.h"

typedef struct {
	int dat_adr;
	u_char floor;
	u_char load_area;
	u_char now_room;
	u_char next_room;
	u_char room_update_flg;
	u_char mirror_flg;
	u_char flr_exist[4];
	u_int padding;
} MAP_WRK;

#endif // INGAME_MAP_MAP_TYPES_H
