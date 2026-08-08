#include "common.h"
#include "typedefs.h"
#include "rgst_dat.h"

#include "data/rg_ap_dat.h"       // RGOST_AP_DAT rg_ap_dat[];
#include "data/rg_dat.h"          // RGOST_DAT rg_dat[];
#include "data/rg_center00.h"     // sceVu0FVECTOR rg_center00[];
#include "data/rg_center01.h"     // sceVu0FVECTOR rg_center01[];
sceVu0FVECTOR *rgc_dat[] = {
    (sceVu0FVECTOR *)rg_center00,
    (sceVu0FVECTOR *)rg_center01,
};
#include "data/rg_start_dat.h"    // RG_DISP_DAT rg_start_dat[];
#include "data/rg_inter_dat.h"    // RG_DISP_DAT rg_inter_dat[];
#include "data/rg_end_dat.h"      // RG_DISP_DAT rg_end_dat[];
#include "data/rg_alp_start00.h"  // RG_ALP_DAT rg_alp_start00[];
#include "data/rg_alp_start01.h"  // RG_ALP_DAT rg_alp_start01[];
RG_ALP_DAT *rg_alp_start[] = {
    rg_alp_start00,
    rg_alp_start01,
};
#include "data/rg_alp_inter00.h"  // RG_ALP_DAT rg_alp_inter00[];
#include "data/rg_alp_inter01.h"  // RG_ALP_DAT rg_alp_inter01[];
RG_ALP_DAT *rg_alp_inter[] = {
    rg_alp_inter00,
    rg_alp_inter01,
};
#include "data/rg_alp_end00.h"    // RG_ALP_DAT rg_alp_end00[];
#include "data/rg_alp_end01.h"    // RG_ALP_DAT rg_alp_end01[];
RG_ALP_DAT *rg_alp_end[] = {
    rg_alp_end00,
    rg_alp_end01,
};
#include "data/rg_ani_start00.h"  // RG_ANI_DAT rg_ani_start00[];
#include "data/rg_ani_start01.h"  // RG_ANI_DAT rg_ani_start01[];
RG_ANI_DAT *rg_ani_start[] = {
    rg_ani_start00,
    rg_ani_start01,
};
#include "data/rg_ani_inter00.h"  // RG_ANI_DAT rg_ani_inter00[];
#include "data/rg_ani_inter01.h"  // RG_ANI_DAT rg_ani_inter01[];
RG_ANI_DAT *rg_ani_inter[] = {
    rg_ani_inter00,
    rg_ani_inter01,
};
#include "data/rg_ani_end00.h"    // RG_ANI_DAT rg_ani_end00[];
#include "data/rg_ani_end01.h"    // RG_ANI_DAT rg_ani_end01[];
RG_ANI_DAT *rg_ani_end[] = {
    rg_ani_end00,
    rg_ani_end01,
};
#include "data/rg_rate_start00.h" // RG_RATE_DAT rg_rate_start00[];
#include "data/rg_rate_start01.h" // RG_RATE_DAT rg_rate_start01[];
RG_RATE_DAT *rg_rate_start[] = {
    rg_rate_start00,
    rg_rate_start01,
};
#include "data/rg_rate_inter00.h" // RG_RATE_DAT rg_rate_inter00[];
#include "data/rg_rate_inter01.h" // RG_RATE_DAT rg_rate_inter01[];
RG_RATE_DAT *rg_rate_inter[] = {
    rg_rate_inter00,
    rg_rate_inter01,
};
#include "data/rg_rate_end00.h"   // RG_RATE_DAT rg_rate_end00[];
#include "data/rg_rate_end01.h"   // RG_RATE_DAT rg_rate_end01[];
RG_RATE_DAT *rg_rate_end[] = {
    rg_rate_end00,
    rg_rate_end01,
};
