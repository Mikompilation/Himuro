#ifndef INGAME_MAP_FIND_DAT_H
#define INGAME_MAP_FIND_DAT_H

#include "typedefs.h"

typedef struct {
	u_char type;
	u_char dummy;
	u_short after;
	u_short pos_x;
	u_short pos_y;
	u_short msg_no;
} FIND_DAT;

// extern FIND_DAT find_dat[0];

#endif // INGAME_MAP_FIND_DAT_H
