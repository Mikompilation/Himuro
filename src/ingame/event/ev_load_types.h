#ifndef INGAME_EVENT_EV_LOAD_TYPES_H
#define INGAME_EVENT_EV_LOAD_TYPES_H

#include "typedefs.h"

typedef struct {
	u_short file_no;
	u_char file_type;
	u_char tmp_no;
	u_int addr;
} MSN_LOAD_DAT;

typedef struct {
	u_char scn_no;
	MSN_LOAD_DAT *load_dat;
	u_short *del_dat;
} SCN_LOAD_DAT;

typedef struct {
	u_char mode;
	u_char count;
	int load_id;
} EVENT_LOAD_WRK;

typedef struct {
	u_char mode;
	u_char load_mode;
	u_char load_count;
	u_char time;
	int load_id;
} MSN_TITLE_WRK;

#endif // INGAME_EVENT_EV_LOAD_TYPES_H
