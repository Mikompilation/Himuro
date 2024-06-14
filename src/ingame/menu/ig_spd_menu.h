#ifndef INGAME_MENU_IG_SPD_MENU_H
#define INGAME_MENU_IG_SPD_MENU_H

#include "typedefs.h"

typedef struct {
	u_char sopt;
	u_char smap;
	u_char slod;
	u_char lod_ret;
} SPD_MNU;

// extern SPD_MNU spd_mnu;

void SpdMenuInit();
char SpdMenuCtrl();
void SpdMapMain();
void SpdOptStart();
void SpdOptMain();

#endif // INGAME_MENU_IG_SPD_MENU_H
