#ifndef INGAME_MENU_IG_MAP_H
#define INGAME_MENU_IG_MAP_H

#include "typedefs.h"

typedef struct {
	short int px;
	short int py;
	u_char rt;
	u_char flr;
	u_char id;
} MAP_DOOR_POS;

typedef struct {
	u_short px;
	u_short py;
	short int rt;
	u_char flr;
	u_char room;
} MAP_SVP_POS;

// extern u_char rm_in_flr0[8];
// extern u_char rm_in_flr1[29];
// extern u_char rm_in_flr2[12];
// extern u_char rm_in_flr3[2];
// extern MAP_DOOR_POS map_door_pos[0];
extern MAP_SVP_POS map_svp_pos[];

void NewgameMenuMapInit();
void LoadgameMenuMapInit();
void StartMapModeInit();
void IngameMenuMap();
void IngameMenuMapDisp(u_char mod);
void AngleForWin(int pri, short int xo, short int yo, short int r, u_char ptn);

#endif // INGAME_MENU_IG_MAP_H
