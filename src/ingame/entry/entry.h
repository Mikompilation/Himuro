#ifndef INGAME_ENTRY_ENTRY_H
#define INGAME_ENTRY_ENTRY_H

#include "typedefs.h"

typedef struct {
	u_char stts;
	u_char gtime;
	u_char rtime;
	u_char ptime;
	u_char atime[11];
	u_char room_fg[42];
	u_char sound;
	u_char pic_num;
	u_char raze;
	u_char fg_mode;
	u_char fgst_no;
	u_char gg_mode;
	u_char ggst_no;
	u_char zh_mode;
	u_char zh_efct;
	u_char fg_pos[2][3];
	u_char fg_set_num;
	u_char fg_se_empty[3];
	u_char gg_room[5];
	u_char pg_req[5];
	u_char ggst_cnt;
	u_short fgst_cnt;
	u_short dgst_cnt;
	u_short zh_ap;
	u_short fg_ap;
} AP_WRK;

// extern AP_WRK ap_wrk;

void EntryInit();
void ClearAppearWrkInfo();
void EntryMain();
void AreaEntrySet();

#endif // INGAME_ENTRY_ENTRY_H
