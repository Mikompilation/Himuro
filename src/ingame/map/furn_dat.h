#ifndef INGAME_MAP_FURN_DAT_H
#define INGAME_MAP_FURN_DAT_H

#include "typedefs.h"

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

// extern FURN_DAT furn_dat[0];
// extern u_int furn_attr_dat[0][5];
// extern sceVu0FVECTOR furn_photo_center00[0];
// extern sceVu0FVECTOR furn_photo_center01[0];
// extern sceVu0FVECTOR furn_photo_center02[0];
// extern sceVu0FVECTOR furn_photo_center03[0];
// extern sceVu0FVECTOR furn_photo_center04[0];
// extern sceVu0FVECTOR furn_photo_center05[0];
// extern sceVu0FVECTOR furn_photo_center06[0];
// extern sceVu0FVECTOR furn_photo_center07[0];
// extern sceVu0FVECTOR furn_photo_center08[0];
// extern sceVu0FVECTOR furn_photo_center09[0];
// extern sceVu0FVECTOR furn_photo_center10[0];
// extern sceVu0FVECTOR furn_photo_center11[0];
// extern sceVu0FVECTOR furn_photo_center12[0];
// extern sceVu0FVECTOR furn_photo_center13[0];
// extern sceVu0FVECTOR furn_photo_center14[0];
// extern sceVu0FVECTOR furn_photo_center15[0];
// extern sceVu0FVECTOR furn_photo_center16[0];
// extern sceVu0FVECTOR furn_photo_center17[0];
// extern sceVu0FVECTOR furn_photo_center18[0];
// extern sceVu0FVECTOR furn_photo_center19[0];
// extern sceVu0FVECTOR furn_photo_center20[0];
// extern sceVu0FVECTOR furn_photo_center21[0];
// extern sceVu0FVECTOR furn_photo_center22[0];
// extern sceVu0FVECTOR furn_photo_center23[0];
// extern sceVu0FVECTOR furn_photo_center24[0];
// extern sceVu0FVECTOR furn_photo_center25[0];
// extern sceVu0FVECTOR furn_photo_center26[0];
// extern sceVu0FVECTOR furn_photo_center27[0];
// extern sceVu0FVECTOR furn_photo_center28[0];
// extern sceVu0FVECTOR furn_photo_center29[0];
// extern sceVu0FVECTOR furn_photo_center30[0];
// extern sceVu0FVECTOR furn_photo_center31[0];
// extern sceVu0FVECTOR furn_photo_center32[0];
// extern sceVu0FVECTOR furn_photo_center33[0];
// extern sceVu0FVECTOR furn_photo_center34[0];
// extern sceVu0FVECTOR furn_photo_center35[0];
// extern sceVu0FVECTOR furn_photo_center36[0];
// extern sceVu0FVECTOR furn_photo_center37[0];
// extern sceVu0FVECTOR furn_photo_center38[0];
// extern sceVu0FVECTOR furn_photo_center39[0];
// extern sceVu0FVECTOR furn_photo_center40[0];
// extern sceVu0FVECTOR furn_photo_center41[0];
// extern sceVu0FVECTOR furn_photo_center42[0];
// extern sceVu0FVECTOR furn_photo_center43[0];
// extern sceVu0FVECTOR furn_photo_center44[0];
// extern sceVu0FVECTOR furn_photo_center45[0];
// extern sceVu0FVECTOR furn_photo_center46[0];
// extern sceVu0FVECTOR furn_photo_center47[0];
// extern sceVu0FVECTOR furn_photo_center48[0];
// extern sceVu0FVECTOR furn_photo_center49[0];
// extern sceVu0FVECTOR furn_photo_center50[0];
// extern sceVu0FVECTOR furn_photo_center51[0];
// extern sceVu0FVECTOR furn_photo_center52[0];
// extern sceVu0FVECTOR furn_photo_center53[0];
// extern sceVu0FVECTOR furn_photo_center54[0];
// extern sceVu0FVECTOR furn_photo_center55[0];
// extern sceVu0FVECTOR furn_photo_center56[0];
// extern sceVu0FVECTOR furn_photo_center57[0];
// extern sceVu0FVECTOR furn_photo_center58[0];
// extern sceVu0FVECTOR furn_photo_center59[0];
// extern sceVu0FVECTOR furn_photo_center60[0];
// extern sceVu0FVECTOR furn_photo_center61[0];
// extern sceVu0FVECTOR furn_photo_center62[0];
// extern sceVu0FVECTOR furn_photo_center63[0];
// extern sceVu0FVECTOR furn_photo_center64[0];
// extern sceVu0FVECTOR furn_photo_center65[0];
// extern sceVu0FVECTOR furn_photo_center66[0];
// extern sceVu0FVECTOR furn_photo_center67[0];
// extern sceVu0FVECTOR furn_photo_center68[0];
// extern sceVu0FVECTOR furn_photo_center69[0];
// extern sceVu0FVECTOR furn_photo_center70[0];
// extern sceVu0FVECTOR furn_photo_center71[0];
// extern sceVu0FVECTOR furn_photo_center72[0];
// extern sceVu0FVECTOR furn_photo_center73[0];
// extern sceVu0FVECTOR furn_photo_center74[0];
// extern sceVu0FVECTOR furn_photo_center75[0];
// extern sceVu0FVECTOR furn_photo_center76[0];
// extern sceVu0FVECTOR furn_photo_center77[0];
// extern sceVu0FVECTOR furn_photo_center78[0];
// extern sceVu0FVECTOR furn_photo_center79[0];
// extern sceVu0FVECTOR furn_photo_center80[0];
// extern sceVu0FVECTOR furn_photo_center81[0];
// extern sceVu0FVECTOR furn_photo_center82[0];
// extern sceVu0FVECTOR furn_photo_center83[0];
// extern sceVu0FVECTOR furn_photo_center84[0];
// extern sceVu0FVECTOR furn_photo_center85[0];
// extern sceVu0FVECTOR furn_photo_center86[0];
// extern sceVu0FVECTOR furn_photo_center87[0];
// extern sceVu0FVECTOR furn_photo_center88[0];
// extern sceVu0FVECTOR furn_photo_center89[0];
// extern sceVu0FVECTOR furn_photo_center90[0];
// extern sceVu0FVECTOR furn_photo_center91[0];
// extern sceVu0FVECTOR furn_photo_center92[0];
// extern sceVu0FVECTOR furn_photo_center93[0];
// extern sceVu0FVECTOR furn_photo_center94[0];
// extern sceVu0FVECTOR furn_photo_center95[0];
// extern sceVu0FVECTOR furn_photo_center96[0];
// extern sceVu0FVECTOR furn_photo_center97[0];
// extern sceVu0FVECTOR furn_photo_center98[0];
// extern sceVu0FVECTOR furn_photo_center99[0];
// extern sceVu0FVECTOR furn_photo_center100[0];
// extern sceVu0FVECTOR furn_photo_center101[0];
// extern sceVu0FVECTOR furn_photo_center102[0];
// extern sceVu0FVECTOR *fpc_dat[0];
// extern u_char fpc_rot_dat[0][8];

FURN_DAT* GetFurnDatP(u_short furn_id);

#endif // INGAME_MAP_FURN_DAT_H
