#ifndef INGAME_PHOTO_PHOTO_TYPES_H
#define INGAME_PHOTO_PHOTO_TYPES_H

#include "typedefs.h"

typedef struct {
	int score;
	int point;
	float ratio;
	u_short no;
	u_short type;
	u_short dmg;
	u_short wrk_no;
} SUBJECT_WRK;

typedef struct {
	int mode;
	u_long score;
	float ratio;
	SUBJECT_WRK plyr;
	SUBJECT_WRK ene[4];
	SUBJECT_WRK furn[5];
	SUBJECT_WRK rare[3];
	u_short spcl_pht_score[4];
	u_char spcl_pht_no[4][2];
	u_short bonus_sta;
	u_short room;
	u_short dist_2d;
	u_short no_2d;
	u_char adr_no;
	u_char ene_dead;
	u_char no_item;
	u_char falth;
} PHOTO_WRK;

#endif // INGAME_PHOTO_PHOTO_TYPES_H
