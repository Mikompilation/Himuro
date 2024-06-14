#ifndef GRAPHICS_SCENE_SCENE_H
#define GRAPHICS_SCENE_SCENE_H

#include "typedefs.h"

#include "graphics/motion/mdlwork.h"
#include "graphics/scene/fod.h"

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
	float min;
	float max;
	float near;
	float far;
	int r;
	int g;
	int b;
	int pad;
} SCENE_FOG;

typedef struct {
	SCN_ANM_MDL man_mdl[4];
	SCN_ANM_MDL furn_mdl[14];
	SCN_ANM_MDL item_mdl[8];
	SCN_ANM_MDL door_mdl[8];
	int man_mdl_tex[4];
	FOD_CTRL fod_ctrl;
	SCENE_FOG fog;
	SgCAMERA tmp_cam;
	SgCAMERA *cam;
	int man_mdl_num;
	int door_num;
	int furn_num;
	int item_num;
	u_int *scn_data_addr;
	u_int *light_rev_addr;
	int scene_no;
	int chapter_no;
	int room_no;
	int hero_mdl_no;
	int count_flg;
	int mirror_flg;
	int init_flg;
} SCENE_CTRL;

typedef struct {
	u_int *scn_file_addr;
	u_int file_num;
	u_int *ofs_top_addr;
	u_int *hdr_addr;
	u_int *cam_fod_addr;
	u_int *lit_fod_addr;
	u_int *eff_fod_addr;
	u_int *man_mot_addr;
	u_int *man_mim_addr;
	u_int *furn_mot_addr;
	u_int *door_mot_addr;
	u_int *item_mot_addr;
} SCENE_FILE;

// extern SCENE_CTRL scene_ctrl[2];
// extern SCENE_FILE scene_file;
// extern int scn_vib_time0;
// extern int scn_vib_time1;
// extern int scn_load_buf_id;
// extern int scn_now_play_id;

int SceneAllLoad(int scene_no, u_int *load_addr);
u_int* SceneDataLoadReq(int scene_no, u_int *load_addr);
int SceneDataLoadWait();
void SceneDataAnalyze(SCENE_FILE *sf, u_int *scn_addr);
void SceneGetHeaderData(u_int *hdr_top);
void SceneInitEnvironment();
void SceneLightRevision();
void SceneAllMdlInit();
void SceneInitManMdl(SCN_ANM_MDL *sam, u_int *mot_addr, u_int *mim_addr, u_int mdl_id);
void SceneInitOtherMdl(u_int *pk2_mot_addr, u_int mdl_id);
void SceneDraw(int scene_no);
void SceneRenewEnvironment();
void SceneMirrorMdlDraw();
int SceneIsEnd();
void SceneLightClear();
void SceneScenePrerender();
sceVu0FVECTOR* SceneGetMdlWaistPos(ANI_CTRL *ani_ctrl, u_short mdl_no);
void SceneSetEneEffect();
void SceneReleaseEffect();
SCN_ENE_EFCT* SceneGetEneEfctAddr(u_int mdl_no);
void SceneSetFadeMdlEffect();
void SceneSetVibrate(int scene_no, int frame);
void SceneDrawManMdl(u_int mdl_id);
void SceneDrawManShadow();
void SceneDrawOtherMdl();
void SceneDrawRoom();
void SceneSpecialManage();
int SceneDecisionMovie(int scene_no);
u_int* SceneGetMotAddr(u_int *pak_top, u_int no, char *pfx);
u_int* SceneGetMimAddr(u_int *pak_top, char *pfx);
char* GetHeaderMdlNo(int num, char *hdr_pfx);
u_int GetPrefixNo(char *pfx);
u_int* GetADRTBL(u_int *top, u_int no);
void SceneSetManMdlTexOffset();
int SceneGetChapterNo(int scene_no);
void SceneSetSquare(int pri, float x, float y, float w, float h, u_char r, u_char g, u_char b, u_char a);
void SceneCheckModelEntry();
void InitSceneWork();

#endif // GRAPHICS_SCENE_SCENE_H
