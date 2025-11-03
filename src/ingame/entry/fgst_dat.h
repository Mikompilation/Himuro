#ifndef INGAME_ENTRY_FGST_DAT_H
#define INGAME_ENTRY_FGST_DAT_H

#include "typedefs.h"

typedef struct {
	u_char room_no;
	u_char ene_type;
	u_char pos_num;
	u_short cmr_no;
	u_short rot[3];
	int pos[3][3];
} FGST_AP_DAT;

extern u_char fgst_ap_no[5][20];
extern FGST_AP_DAT fg_ap_dat1[];
extern FGST_AP_DAT fg_ap_dat2[];
extern FGST_AP_DAT fg_ap_dat3[];
extern FGST_AP_DAT fg_ap_dat4[];
extern FGST_AP_DAT *fg_ap_dat[];


#endif // INGAME_ENTRY_FGST_DAT_H
