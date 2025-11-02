#ifndef GRAPHICS_SCENE_SCENE_DAT_H
#define GRAPHICS_SCENE_SCENE_DAT_H

#include "typedefs.h"

#include "graphics/motion/ani_dat.h"

typedef struct {
	u_int type;
	float sclx;
	float scly;
	u_int alpha;
	float rate;
	float trate;
} SCN_ENE_EF_PDF;

typedef struct {
	u_short scene_no;
	u_short mdl_no;
	float aura_size;
	float aura_rate;
	u_int aura_rgba;
	SCN_ENE_EF_PDF pdf1;
	SCN_ENE_EF_PDF pdf2;
	float pdf_dist;
	float aura_pos_ajst;
	int mdl_alpha;
} SCN_ENE_EFCT;

typedef struct {
	ANI_CTRL mdl_anm;
	SCN_ENE_EFCT *ene_efct;
	void *efct_addr[4];
	char prefix[4];
	u_int mdl_no;
	u_int mdl_alpha;
	u_int scn_mdl_no;
	u_int *mdl_addr;
	u_int *mot_addr;
	u_int *mim_addr;
	u_int *mim_buf_addr;
	u_int disp_flg;
} SCN_ANM_MDL;

typedef struct {
	u_short start;
	u_short end;
	u_short type;
} SCN_VIB_DATA;

typedef struct {
	int time0;
	int time1;
	int val;
} SCN_VIB_TYPE;

extern SCN_ENE_EFCT scn_ene_efct[];
extern SCN_VIB_TYPE scn_vib_type[];
// extern SCN_VIB_DATA scn_vib_0020[0];
// extern SCN_VIB_DATA scn_vib_0022[0];
// extern SCN_VIB_DATA scn_vib_0030[0];
// extern SCN_VIB_DATA scn_vib_0031[0];
// extern SCN_VIB_DATA scn_vib_1010[0];
// extern SCN_VIB_DATA scn_vib_1030[0];
// extern SCN_VIB_DATA scn_vib_1031[0];
// extern SCN_VIB_DATA scn_vib_1034[0];
// extern SCN_VIB_DATA scn_vib_1070[0];
// extern SCN_VIB_DATA scn_vib_1080[0];
// extern SCN_VIB_DATA scn_vib_1090[0];
// extern SCN_VIB_DATA scn_vib_1100[0];
// extern SCN_VIB_DATA scn_vib_1101[0];
// extern SCN_VIB_DATA scn_vib_1102[0];
// extern SCN_VIB_DATA scn_vib_1120[0];
// extern SCN_VIB_DATA scn_vib_1150[0];
// extern SCN_VIB_DATA scn_vib_1160[0];
// extern SCN_VIB_DATA scn_vib_1170[0];
// extern SCN_VIB_DATA scn_vib_1180[0];
// extern SCN_VIB_DATA scn_vib_1220[0];
// extern SCN_VIB_DATA scn_vib_1230[0];
// extern SCN_VIB_DATA scn_vib_1240[0];
// extern SCN_VIB_DATA scn_vib_1250[0];
// extern SCN_VIB_DATA scn_vib_1260[0];
// extern SCN_VIB_DATA scn_vib_1300[0];
// extern SCN_VIB_DATA scn_vib_1301[0];
// extern SCN_VIB_DATA scn_vib_1330[0];
// extern SCN_VIB_DATA scn_vib_1332[0];
// extern SCN_VIB_DATA scn_vib_1340[0];
// extern SCN_VIB_DATA scn_vib_1410[0];
// extern SCN_VIB_DATA scn_vib_2010[0];
// extern SCN_VIB_DATA scn_vib_2013[0];
// extern SCN_VIB_DATA scn_vib_2020[0];
// extern SCN_VIB_DATA scn_vib_2030[0];
// extern SCN_VIB_DATA scn_vib_2040[0];
// extern SCN_VIB_DATA scn_vib_2060[0];
// extern SCN_VIB_DATA scn_vib_2072[0];
// extern SCN_VIB_DATA scn_vib_2090[0];
// extern SCN_VIB_DATA scn_vib_2091[0];
// extern SCN_VIB_DATA scn_vib_2092[0];
// extern SCN_VIB_DATA scn_vib_2110[0];
// extern SCN_VIB_DATA scn_vib_2130[0];
// extern SCN_VIB_DATA scn_vib_2131[0];
// extern SCN_VIB_DATA scn_vib_2132[0];
// extern SCN_VIB_DATA scn_vib_2140[0];
// extern SCN_VIB_DATA scn_vib_2141[0];
// extern SCN_VIB_DATA scn_vib_2142[0];
// extern SCN_VIB_DATA scn_vib_2150[0];
// extern SCN_VIB_DATA scn_vib_2160[0];
// extern SCN_VIB_DATA scn_vib_3010[0];
// extern SCN_VIB_DATA scn_vib_3030[0];
// extern SCN_VIB_DATA scn_vib_3040[0];
// extern SCN_VIB_DATA scn_vib_3060[0];
// extern SCN_VIB_DATA scn_vib_3070[0];
// extern SCN_VIB_DATA scn_vib_3080[0];
// extern SCN_VIB_DATA scn_vib_3090[0];
// extern SCN_VIB_DATA scn_vib_3100[0];
// extern SCN_VIB_DATA scn_vib_3110[0];
// extern SCN_VIB_DATA scn_vib_4011[0];
// extern SCN_VIB_DATA scn_vib_4020[0];
// extern SCN_VIB_DATA scn_vib_4041[0];
// extern SCN_VIB_DATA scn_vib_4042[0];
// extern SCN_VIB_DATA scn_vib_4050[0];
// extern SCN_VIB_DATA scn_vib_4070[0];
// extern SCN_VIB_DATA scn_vib_4090[0];
// extern SCN_VIB_DATA scn_vib_4100[0];
// extern SCN_VIB_DATA scn_vib_4110[0];
// extern SCN_VIB_DATA scn_vib_4120[0];
// extern SCN_VIB_DATA scn_vib_5010[0];
// extern SCN_VIB_DATA scn_vib_5020[0];
// extern SCN_VIB_DATA scn_vib_dmy[0];
extern SCN_VIB_DATA *scn_vib_tbl[];
extern u_char scene_movie_no[];


#endif // GRAPHICS_SCENE_SCENE_DAT_H
