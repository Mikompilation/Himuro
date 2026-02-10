#ifndef INGAME_MAP_DOORTYPE_H
#define INGAME_MAP_DOORTYPE_H

#include "typedefs.h"

typedef struct {
	u_short attribute;
	short int open_wait;
	short int close_wait;
	short int move_max;
	int se_file;
	u_char room_id;
	u_short dbl_did;
} DOOR_TYPE_DAT;

typedef struct {
	float sx;
	float sy;
	u_char dmd_no1;
	u_char dmd_no2;
	u_char anm_no;
	u_char anm_last_no;
	u_char c_dmd_no1;
	u_char c_dmd_no2;
	u_char c_anm_no;
} DOOR_MTN_DAT;

typedef struct {
	u_short st_tbl_pos;
	u_short start_cnt;
	u_short end_cnt;
	u_short sel_cnt;
} DOOR_MOVE_DAT;

// extern DOOR_TYPE_DAT door_type_dat[];
extern float *dtbl_tbl[];
extern DOOR_MTN_DAT door_mtn_dat[];
// extern DOOR_MOVE_DAT door_move_dat[];

DOOR_MTN_DAT* GetDoorMtnDat(u_char dtype);
DOOR_TYPE_DAT* GetDoorTypeDatP(int no);
DOOR_TYPE_DAT GetDoorTypeDat(int no);
u_short GetDoorMotionTblPos(u_char door_motion_dat_no);
int IsFusumaType(u_char type);
int IsRotType(u_char type);
int IsFusumaAttr(u_short attr);
int MotionIsDouble(u_char motion_no);

#endif // INGAME_MAP_DOORTYPE_H
