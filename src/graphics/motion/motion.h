#ifndef GRAPHICS_MOTION_MOTION_H
#define GRAPHICS_MOTION_MOTION_H

#include "typedefs.h"

#include "graphics/motion/ani_dat.h"
#include "graphics/motion/mot_dat.h"
#include "graphics/graph3d/sg_dat.h"

typedef struct {
	C_PARTICLE *particle;
	SPRING *spring;
	ROPE_DAT *rdat;
	sceVu0FVECTOR dir;
	sceVu0FVECTOR top;
	float pow;
	u_int furn_id;
	u_char p_num;
	u_char spring_num;
	u_char stat;
	u_char move_mode;
} ROPE_CTRL;

typedef struct {
	u_int *mdl_p;
	u_int flg;
	u_int mdl_no;
	int offset;
} ENE_VRAM_CTRL;

typedef struct {
	float r;
	float ry;
	u_char bone_id;
	u_char flg;
} ENE_CHILD_CTRL;

typedef struct {
	u_char buf_no;
	u_char pkt_no;
} ENE_PKT_CTRL;

typedef struct {
	u_char msn_no;
	u_char step;
} MSN_PLYR_INIT;

typedef struct {
	float pow[2];
	int cnt;
	int all_cnt;
	short int req;
	char loop;
	char flg;
} QUAKE_CTRL;

typedef struct {
	sceVu0FVECTOR pos;
	sceVu0FVECTOR trot;
	float spd;
	u_int timer;
	u_short timer2;
	u_int move_type;
	u_char req;
	u_char act_no;
} PLYR_ACT_WRK;

extern MIME_CTRL mim_chodo[20];
extern u_char mim_chodo_se[20];
extern MIME_DAT mim_cdat[2][20];
extern ANI_CTRL ani_mdl[20];
extern ANI_MDL_CTRL ani_mdl_ctrl[20];
extern ENE_PKT_CTRL ene_pkt_ctrl[4];
extern ACS_ALPHA plyracs_ctrl[2];
extern ROPE_CTRL rope_ctrl[20];
extern CMOVE_CTRL cmove_ctrl[10];
extern ENE_VRAM_CTRL ene_vram_ctrl[4];
extern ENE_VRAM_CTRL ene_vram_bak[4];
extern ENE_CHILD_CTRL ene_child_ctrl[4];
extern MSN_PLYR_INIT plyr_init_ctrl;
extern char plyr_mdl_no;
extern u_char mim_mepati_id;
extern u_char mim_nigiri_l_id;
extern u_char mim_nigiri_r_id;
extern QUAKE_CTRL quake;
extern PLYR_ACT_WRK plyr_act_wrk;
// extern float now_frot_x;

void motInitPlayerAnm(char mdl_no);
void motInitEnemyAnm(u_int *anm_p, u_int mdl_no, u_int anm_no);
void motInitOneEnemyAnm(u_int *anm_p, u_int mdl_no, u_int anm_no);
void motInitMultiEnemyAnm(u_int *anm_p, u_int mdl_no, u_int anm_no, u_int num);
void motInitEnemyMdl(u_int *mdl_p, u_int mdl_no);
void motInitOneEnemyMdl(u_int *mdl_p, u_int mdl_no);
void motInitMultiEnemyMdl(u_int *mdl_addr, u_int mdl_no);
void motInitFlyMdl(u_int *mdl_p, u_int mdl_no);
void motInitMultiFlyMdl(u_int *mpk_p, u_int mdl_no);
void motInitOneFlyMdl(u_int *sgd_p, u_int mdl_no);
static void motInitClearAniCtrl(ANI_CTRL *ani_ctrl);
u_int* motInitAniCtrl(ANI_CTRL *ani_ctrl, u_int *anm_p, u_int *mdl_p, u_int *pkt_p, u_int mdl_no, u_int anm_no);
u_int* motInitMotCtrl(MOT_CTRL *m_ctrl, u_int *mot_addr, u_int *rst_addr);
void motSetCoordCamera(ANI_CTRL *ani_ctrl);
u_char motSetCoord(ANI_CTRL *ani_ctrl, u_char work_id);
static void motInitNewMotion(ANI_CTRL *ani_ctrl);
static u_char motAddFrame(MOT_CTRL *m_ctrl);
u_int motGetNowFrame(MOT_CTRL *m_ctrl);
void ReqPlayerAnime(u_char flame);
void ReqEnemyAnime(u_char work_no, u_char flame);
void motSetAnime(ANI_CTRL *ani_ctrl, ANI_CODE **tbl, int req_no);
void* GetFileInPak(void *pak_head, int num);
void* GetPakTaleAddr(void *pak_head);
void GetMdlNeckPos(sceVu0FVECTOR pos, ANI_CTRL *ani_ctrl, u_short mdl_no);
u_int GetMdlBonePos(sceVu0FVECTOR *pos, u_char work_no);
void GetMdlWaistPos(sceVu0FVECTOR pos, ANI_CTRL *ani_ctrl, u_short mdl_no);
void GetMdlLegPos(sceVu0FVECTOR pos, ANI_CTRL *ani_ctrl, u_short mdl_no);
void GetMdlShldPos(sceVu0FVECTOR pos, ANI_CTRL *ani_ctrl, u_char lr);
void GetPlyrFootPos(sceVu0FVECTOR pos, ANI_CTRL *ani_ctrl, u_char lr);
void GetPlyrAcsLightPos(sceVu0FVECTOR pos, ANI_CTRL *ani_ctrl);
void GetToushuKatanaPos(sceVu0FVECTOR p0, sceVu0FVECTOR p1, ANI_CTRL *ani_ctrl);
u_short GetPlyrFtype();
void motInitInterpAnime(ANI_CTRL *ani_ctrl, int flame);
static void motSetInterpMatrix(ANI_CTRL *ani_ctrl, sceVu0FMATRIX *start, sceVu0FMATRIX *end);
static void motInterpAnm(ANI_CTRL *ani_ctrl, sceVu0FMATRIX *start, sceVu0FMATRIX *end);
void motInterpMatrix(sceVu0FMATRIX interp, sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rate);
void motMatrix2Quaternion(sceVu0FMATRIX q, sceVu0FVECTOR m);
void motQuaternion2Matrix(sceVu0FMATRIX m, sceVu0FVECTOR q);
void motQuaternionSlerp(sceVu0FVECTOR q, sceVu0FVECTOR q1, sceVu0FVECTOR q2, float rate);
void LocalRotMatrixX(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rx);
void LocalRotMatrixY(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float ry);
void LocalRotMatrixZ(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rz);
void motInversKinematics(SgCOORDUNIT *cp, sceVu0FVECTOR target, u_int *top_addr, u_char bone_id);
static u_int movGetFrameNum(u_int *mov_p);
static float movGetMaxval(u_int *mov_p);
void movGetMoveval(ANI_CTRL *ani_ctrl, u_int frame_num);
static u_int motGetBoneNum(u_int *mot_p);
static u_int motGetTransBoneNum(u_int *mot_p);
static u_int motGetFrameNum(u_int *mot_p);
static u_int motGetSIFrameNum(u_int *mot_p);
static u_int motGetInterpFrameNum(u_int *mot_p);
static u_int* motGetRstBuffer(u_int *mot_p);
static u_int motCheckRstFull(u_int *mot_p);
static u_int motCheckTransCut(u_int *mot_p);
static u_int motCheckIncludeRstPacket(u_int *mot_p);
static void motAddressMapping(u_int *top_addr);
static u_int* motGetFrameDataAddr(u_int *top_addr, u_int frame);
static void motGetFrameData(RST_DATA *rst, u_int *top_addr, u_int frame);
static void motGetFrameDataRST(RST_DATA *rst, u_int *top_addr, u_int frame);
static void motGetFrameDataRT(RST_DATA *rst, u_int *top_addr, u_int frame, u_int init_flg);
static u_int motGetTransId(u_int *top_addr, u_int id);
static u_int motGetParentId(u_int *top_addr, u_int id);
void motSetHierarchy(SgCOORDUNIT *coord, u_int *top_addr);
u_int* SceneInitAnime(ANI_CTRL *ani_ctrl, u_int *mdl_p, u_int *mot_p, u_int *mim_p, u_int *pkt_p, u_int mdl_no);
u_int* SceneInitOtherAnime(ANI_CTRL *ani_ctrl, u_int *mdl_p, u_int *mot_p, u_int *mim_p, u_int *pkt_p);
void motSetCoordFrame(ANI_CTRL *ani_ctrl, u_int frame);
void SceneSetCoordFrame(ANI_CTRL *ani_ctrl, u_int frame, u_int type);
void motSetInvMatrix(sceVu0FMATRIX m1, sceVu0FMATRIX m0);
u_int* motAlign128(u_int *addr);
void motPrintVector(char *str, sceVu0FVECTOR vec);
void sceRotMatrixXYZ(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FVECTOR rot);

#endif // GRAPHICS_MOTION_MOTION_H
