#ifndef OS_EEIOP_ADPCM_EA_DAT_H
#define OS_EEIOP_ADPCM_EA_DAT_H

#include "typedefs.h"

typedef struct {
	int file_no;
	u_short vol;
} ADPCM_VOL;

#define MAX_VOLUME 0x3fff
#define NO_VOL_TABLE 368

extern ADPCM_VOL adpcm_vol_tbl[NO_VOL_TABLE];

u_short GetAdpcmVol(int file_no);

#endif // OS_EEIOP_ADPCM_EA_DAT_H
