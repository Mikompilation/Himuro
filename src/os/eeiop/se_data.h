#ifndef OS_EEIOP_SE_DATA_H
#define OS_EEIOP_SE_DATA_H

#include "typedefs.h"

typedef struct {
	u_short se_no0;
	u_short se_no1;
} SE_STE;

// extern char se_use_static_tbl[0][30];
// extern char se_use_btlhit_tbl[0][3];
// extern char se_use_voice_tbl[0][5];
// extern char se_use_door_tbl[0][2];
// extern char se_use_foot_tbl[0][1];
// extern char se_use_srund_tbl[0][1];
// extern char se_use_ghost_tbl[0][11];
// extern char se_use_event_tbl[0][3];
// extern char se_use_wide_tbl[0][1];
// extern char se_use_jidou_tbl[0][1];

char CheckSeUse(int se_no);
char GetSeUseTbl(u_char type, u_char tbl_no, u_char se_no);
SE_STE* SeGetSeSte(int num);

#endif // OS_EEIOP_SE_DATA_H
