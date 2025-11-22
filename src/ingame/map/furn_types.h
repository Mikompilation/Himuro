#ifndef INGAME_MAP_FURN_TYPES_H
#define INGAME_MAP_FURN_TYPES_H

#include "typedefs.h"

#include "graphics/graph3d/light_types.h"

typedef struct {
	u_short attr_no;
	u_short dist_n;
	u_short dist_f;
	u_short score;
	u_short fact_no;
	u_char fp_num;
	u_char fp_no;
	u_char fp_rot;
	u_char fp_mak;
	u_char acs_flg;
	u_char ef_type;
	u_char ef_no;
} FURN_DAT;

typedef enum
{
    ST_FACT_VACANT = 0,
    ST_FACT_WAITWAIT = 1,
    ST_FACT_WAIT = 2,
    ST_FACT_EXEC = 3,
    ST_FACT_END = 4
} ST_FACT;

typedef struct {
	u_char TexNum;
	u_char pads;
	u_char AnmON;
	u_char AnmStep;
	u_char AnmCnt;
	u_char AnmLoop;
} TextureAnimation;

typedef struct {
	signed char nochkflg;
	void *p;
	u_char chknum[4];
} SPE_CHK_COND;

typedef struct {
	u_char regmode;
	u_char looptype;
	char nowstep;
} FSPE_TEXTURE_ANM;

typedef struct {
	u_char attribute;
	void *wp;
	void *ap;
	SPE_CHK_COND chk_occ;
	SPE_CHK_COND chk_end;
	ST_FACT state;
	u_char flg;
	u_char eventflg;
	signed char trembleflg;
	u_char prejudge;
	signed char lw_id;
	void *buff;
	u_short furn_id;
	u_short fact_no;
	int fw_id;
	u_char room_no;
	sceVu0FVECTOR *vecp_temp;
	sceVu0FVECTOR util_v;
	sceVu0FVECTOR pos;
	sceVu0FVECTOR rot_speed;
	sceVu0FVECTOR rot_speed2;
	u_short count;
	int adj_valg_num;
	u_short se_no;
	TextureAnimation *pta;
	FSPE_TEXTURE_ANM ta;
} FURN_ACT_WRK;

typedef struct {
	u_short furn_no;
	short int stts;
	sceVu0FVECTOR pos;
	sceVu0FVECTOR rotate;
	float rot;
	int fewrk_no;
	u_short id;
	u_short fno_bk;
	u_short dist;
	u_short score;
	float ratio;
	u_char use;
	u_char cmdflg;
	u_short attr_id;
	u_char room_id;
	u_char fs_flg;
	LIGHT_PACK mylight;
} FURN_WRK;

typedef struct {
	float rot_y;
	float rot_x;
	u_short pos_x;
	short int pos_y;
	u_short pos_z;
	u_short attr_id;
	u_short model_no;
	u_short id;
	short int top;
	short int btm;
	u_char snum;
} FURN_DATA_POP;

#endif // INGAME_MAP_FURN_TYPES_H
