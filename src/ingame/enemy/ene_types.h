#ifndef INGAME_ENEMY_ENE_TYPES_H
#define INGAME_ENEMY_ENE_TYPES_H

#include "typedefs.h"

typedef struct {
	u_int attr1;
	u_short dst_gthr;
	u_char way_gthr;
	u_char atk_ptn;
	u_char wspd;
	u_char rspd;
	u_short hp;
	u_short atk_rng;
	u_short hit_rng;
	u_short chance_rng;
	short int hit_adjx;
	u_short atk_p;
	u_short atk_h;
	u_char atk;
	u_char atk_tm;
	u_short mdl_no;
	u_short anm_no;
	u_int se_no;
	u_int adpcm_no;
	int dead_adpcm;
	u_short point_base;
	u_char hint_pic;
	u_char aura_alp;
	u_char area[6];
	u_short dir;
	u_short px;
	short int py;
	u_short pz;
} ENE_DAT;


typedef struct {
	u_char dat_no;
	u_char soul_no;
	u_short dir;
	u_short px;
	short int py;
	u_short pz;
	u_short adpcm_tm;
	int adpcm_no;
	u_short rng;
	u_short mdl_no;
	u_short anm_no;
	u_short point_base;
	u_int se_no;
	int se_foot;
} AENE_INFO_DAT;

typedef struct {
	u_short dmg;
	u_short hit_rng;
	u_short mdl_no;
	u_short cond;
} FLY_DATA;

typedef struct {
	int flow;
	float rrad[64];
	float rrotx[64];
	float rroty[64];
	float racc[64];
	float rbrk[64];
	u_char rscl[64];
} SPAWAY;

#endif // INGAME_ENEMY_ENE_TYPES_H