#ifndef INGAME_IG_GLOB_TYPES_H
#define INGAME_IG_GLOB_TYPES_H

#include "typedefs.h"

#include "ingame/map/furn_types.h"
#include "ingame/enemy/move_types.h"
#include "ingame/enemy/ene_types.h"

typedef struct {
	void *addr;
	sceVu0FVECTOR pos0;
	sceVu0FVECTOR pos1;
	sceVu0FVECTOR rot0;
	u_char use;
} FURN_EFCT;

typedef struct {
	u_int flg;
	u_int padding;
} FURN_ATTR_FLG;

typedef struct {
	u_short mdl_no;
	u_short attr;
	u_char stts;
	u_char stts_map;
	u_char room_id;
	u_int padding;
} DOOR_STTS_KEEP;

typedef struct {
	u_short item_no;
	u_char disp;
	sceVu0FVECTOR pos;
} ITEM_DISP_WRK;

typedef struct {
	u_short sta;
	u_char ene;
	u_char reserve;
	MOVE_BOX move_box;
	FLY_DATA *dat;
	void *ep;
	sceVu0FVECTOR adjp;
	sceVu0FVECTOR adjmv;
} FLY_WRK;

#endif // INGAME_IG_GLOB_TYPES_H
