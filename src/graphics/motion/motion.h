#ifndef GRAPHICS_MOTION_MOTION_H
#define GRAPHICS_MOTION_MOTION_H

#include "typedefs.h"

#include "graphics/motion/accessory.h"

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
	sceVu0FVECTOR rot;
	sceVu0FVECTOR scale;
	sceVu0FVECTOR trans;
} RST_DATA;

typedef struct {
	u_int *top;
	u_int *dat;
	u_int play_id;
	int cnt;
	int all_cnt;
	int inp_allcnt;
	int inp_cnt;
	int reso;
	int reso_cnt;
	RST_DATA *rst0;
	RST_DATA *rst1;
} MOT_CTRL;

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

// extern MIME_CTRL mim_chodo[20];
// extern u_char mim_chodo_se[20];
// extern MIME_DAT mim_cdat[2][20];
// extern ANI_CTRL ani_mdl[20];
// extern ANI_MDL_CTRL ani_mdl_ctrl[20];
// extern ENE_PKT_CTRL ene_pkt_ctrl[4];
// extern ACS_ALPHA plyracs_ctrl[2];
// extern ROPE_CTRL rope_ctrl[20];
// extern CMOVE_CTRL cmove_ctrl[10];
// extern ENE_VRAM_CTRL ene_vram_ctrl[4];
// extern ENE_VRAM_CTRL ene_vram_bak[4];
// extern ENE_CHILD_CTRL ene_child_ctrl[4];
// extern MSN_PLYR_INIT plyr_init_ctrl;
// extern char plyr_mdl_no;
// extern u_char mim_mepati_id;
// extern u_char mim_nigiri_l_id;
// extern u_char mim_nigiri_r_id;
// extern QUAKE_CTRL quake;
// extern PLYR_ACT_WRK plyr_act_wrk;
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
u_int* motInitAniCtrl(u_int *anm_p, u_int *mdl_p, u_int *pkt_p, u_int mdl_no, u_int anm_no);
u_int* motInitMotCtrl(MOT_CTRL *m_ctrl, u_int *mot_addr, u_int *rst_addr);
void motSetCoordCamera();
u_char motSetCoord(u_char work_id);
u_int motGetNowFrame();
void ReqPlayerAnime(u_char flame);
void ReqEnemyAnime(u_char work_no, u_char flame);
void motSetAnime(ANI_CODE **tbl, int req_no);
void* GetFileInPak(void *pak_head, int num);
void* GetPakTaleAddr(void *pak_head);
void GetMdlNeckPos(float *pos, u_short mdl_no);
u_int GetMdlBonePos(sceVu0FVECTOR *pos, u_char work_no);
void GetMdlWaistPos(float *pos, u_short mdl_no);
void GetMdlLegPos(float *pos, u_short mdl_no);
void GetMdlShldPos(float *pos, u_char lr);
void GetPlyrFootPos(float *pos, u_char lr);
void GetPlyrAcsLightPos(float *pos);
void GetToushuKatanaPos(float *p0, float *p1);
u_short GetPlyrFtype();
void motInitInterpAnime(int flame);
void motInterpMatrix(float *interp[4], float *m1[4], float rate);
void motMatrix2Quaternion(float *q, float *m[4]);
void motQuaternion2Matrix(float *m[4], float *q);
void motQuaternionSlerp(float *q, float *q1, float *q2, float rate);
void LocalRotMatrixX(float *m1[4], float rx);
void LocalRotMatrixY(float *m1[4], float ry);
void LocalRotMatrixZ(float *m1[4], float rz);
void motInversKinematics(SgCOORDUNIT *cp, float *target, u_int *top_addr, u_char bone_id);
void movGetMoveval(u_int frame_num);
void motSetHierarchy(SgCOORDUNIT *coord, u_int *top_addr);
u_int* SceneInitAnime(u_int *mdl_p, u_int *mot_p, u_int *mim_p, u_int *pkt_p, u_int mdl_no);
u_int* SceneInitOtherAnime(u_int *mdl_p, u_int *mot_p, u_int *mim_p, u_int *pkt_p);
void motSetCoordFrame(u_int frame);
void SceneSetCoordFrame(u_int frame, u_int type);
void motSetInvMatrix(float *m1[4]);
u_int* motAlign128(u_int *addr);
void motPrintVector(char *str, float *vec);
void sceRotMatrixXYZ(float *m1[4], float *rot);

#endif // GRAPHICS_MOTION_MOTION_H
