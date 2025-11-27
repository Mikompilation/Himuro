#ifndef INGAME_ENTRY_RGST_DAT_H
#define INGAME_ENTRY_RGST_DAT_H

#include "typedefs.h"

typedef struct {
	u_char room;
	u_char ap_step0;
	u_char ap_step1[4];
	u_char ap_step2[6];
	u_short ap_near;
	u_short ap_far;
	u_short ap_stts;
} RGOST_AP_DAT;

typedef struct {
	u_short score;
	u_char center_no;
	u_char center_num;
	u_char start;
	u_char inter;
	u_char end;
	u_char dummy;
	short int pos[3];
} RGOST_DAT;

typedef struct {
	u_short flame;
	u_char alp_no;
	u_char ani_no;
	u_char efct_no;
	u_char rate_no;
} RG_DISP_DAT;

typedef struct {
	u_short flame;
	u_char alp;
	u_char stts;
} RG_ALP_DAT;

typedef struct {
	u_char flame;
	u_char tim_no;
} RG_ANI_DAT;

typedef struct {
	u_short flame;
	u_char rate;
	u_char stts;
} RG_RATE_DAT;

extern RGOST_AP_DAT rg_ap_dat[];
extern RGOST_DAT rg_dat[];
// extern sceVu0FVECTOR rg_center00[0];
// extern sceVu0FVECTOR rg_center01[0];
// extern sceVu0FVECTOR *rgc_dat[0];
extern RG_DISP_DAT rg_start_dat[];
extern RG_DISP_DAT rg_inter_dat[];
extern RG_DISP_DAT rg_end_dat[];
// extern RG_ALP_DAT rg_alp_start00[0];
// extern RG_ALP_DAT rg_alp_start01[0];
extern RG_ALP_DAT *rg_alp_start[];
// extern RG_ALP_DAT rg_alp_inter00[0];
// extern RG_ALP_DAT rg_alp_inter01[0];
extern RG_ALP_DAT *rg_alp_inter[];
// extern RG_ALP_DAT rg_alp_end00[0];
// extern RG_ALP_DAT rg_alp_end01[0];
extern RG_ALP_DAT *rg_alp_end[];
// extern RG_ANI_DAT rg_ani_start00[0];
// extern RG_ANI_DAT rg_ani_start01[0];
// extern RG_ANI_DAT *rg_ani_start[0];
// extern RG_ANI_DAT rg_ani_inter00[0];
// extern RG_ANI_DAT rg_ani_inter01[0];
// extern RG_ANI_DAT *rg_ani_inter[0];
// extern RG_ANI_DAT rg_ani_end00[0];
// extern RG_ANI_DAT rg_ani_end01[0];
// extern RG_ANI_DAT *rg_ani_end[0];
// extern RG_RATE_DAT rg_rate_start00[0];
// extern RG_RATE_DAT rg_rate_start01[0];
// extern RG_RATE_DAT *rg_rate_start[0];
// extern RG_RATE_DAT rg_rate_inter00[0];
// extern RG_RATE_DAT rg_rate_inter01[0];
// extern RG_RATE_DAT *rg_rate_inter[0];
// extern RG_RATE_DAT rg_rate_end00[0];
// extern RG_RATE_DAT rg_rate_end01[0];
// extern RG_RATE_DAT *rg_rate_end[0];

#endif // INGAME_ENTRY_RGST_DAT_H
