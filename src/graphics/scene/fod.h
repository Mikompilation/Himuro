#ifndef GRAPHICS_SCENE_FOD_H
#define GRAPHICS_SCENE_FOD_H

#include "typedefs.h"

#include "ingame/camera/camera.h"
#include "graphics/graph3d/sglight.h"

typedef struct {
	sceVu0FVECTOR p;
	sceVu0FVECTOR i;
	float roll;
	float fov;
	float near;
	float far;
} FOD_FIRST_CAM;

typedef struct {
	u_int frame;
	u_int size;
	u_char anm[8];
} FOD_CAM_FRAME;

typedef struct {
	u_int frame;
	u_int size;
	u_int pad[2];
} FOD_LIT_FRAME;

typedef struct {
	u_char light_no;
	u_char light_type;
	u_char anm_flg;
	char pad;
	char light_name[28];
} FOD_LIT_SERIAL;

typedef struct {
	FOD_LIT_SERIAL lit_serial[36];
	SgLIGHT all_lit[36];
	sceVu0FVECTOR amb[6];
	u_int *lit_top;
	u_int ilit_num;
	u_int slit_num;
	u_int plit_num;
	u_int all_lit_num;
	int hand_spot_no;
} FOD_LIGHT;

typedef struct {
	u_int frame_no;
	u_int size;
	u_char zdepth;
	u_char mono;
	u_char sepia;
	u_char color_change;
	u_int pad;
} FOD_EFF_FRAME;

typedef struct {
	u_char efct_id;
	u_char type;
	u_char alpmax;
	u_char colmax;
	float speed;
	float alpha;
	u_int ipad;
} FOD_EF_DITHER;

typedef struct {
	u_char efct_id;
	u_char blur_type;
	u_char alpha;
	u_char cpad;
	u_int scale;
	u_int rot;
	u_int ipad;
} FOD_EF_BLUR;

typedef struct {
	u_char efct_id;
	u_char type;
	u_char volume;
	u_char cpad;
	u_int ipad[3];
} FOD_EF_DEFORM;

typedef struct {
	u_char efct_id;
	u_char volume;
	u_char cpad[2];
	u_int ipad[3];
} FOD_EF_FOCUS;

typedef struct {
	u_char efct_id;
	u_char color;
	u_char alpha;
	u_char cont_type;
	u_int ipad[3];
} FOD_EF_CONTRAST;

typedef struct {
	u_char efct_id;
	u_char volume;
	u_char cpad[2];
	u_int ipad[3];
} FOD_EF_F_FRAME;

typedef struct {
	u_char efct_id;
	u_char type;
	u_char cpad[2];
	float x;
	float y;
	float z;
} FOD_EF_LENZ_F;

typedef struct {
	u_char efct_id;
	u_char volume;
	u_char cpad[2];
	u_int ipad[3];
} FOD_EF_CROSS_F;

typedef struct {
	u_char efct_id;
	u_char cpad[3];
	u_char r;
	u_char g;
	u_char b;
	u_char a;
	u_int ipad[2];
} FOD_EF_FADE_SCR;

typedef struct {
	u_char efct_id;
	u_char alpha;
	u_char cpad[2];
	u_char prefix[4];
	u_int mdl_id;
	u_int ipad;
} FOD_EF_FADE_MDL;

typedef struct {
	u_char efct_id;
	u_char data_type;
	u_char type;
	u_char alpha;
	float rate;
	float trate;
	int pad;
} FOD_EF_P_DEFORM1;

typedef struct {
	u_char efct_id;
	u_char data_type;
	u_char cpad[2];
	float posx;
	float posy;
	float posz;
} FOD_EF_P_DEFORM2;

typedef struct {
	u_char efct_id;
	u_char data_type;
	u_char cpad[2];
	float sclx;
	float scly;
	float pad;
} FOD_EF_P_DEFORM3;

typedef struct {
	u_char efct_id;
	u_char cpad[3];
	float posx;
	float posy;
	float posz;
} FOD_EF_FIRE;

typedef struct {
	u_char efct_id;
	u_char cpad[3];
	float posx;
	float posy;
	float posz;
} FOD_EF_ITEM;

typedef struct {
	u_char efct_id;
	u_char type;
	u_char cpad[2];
	float posx;
	float posy;
	float posz;
} FOD_EF_AMULET;

typedef struct {
	u_char efct_id;
	u_char cpad[3];
	u_char prefix[4];
	u_int ipad[2];
} FOD_EF_ENEMY;

typedef struct {
	u_char efct_id;
	u_char type;
	u_char cpad[2];
	u_int frame;
	u_int ipad[2];
} FOD_EF_VIBRATE;

typedef union {
	FOD_EF_DITHER dither;
	FOD_EF_BLUR blur;
	FOD_EF_DEFORM deform;
	FOD_EF_FOCUS focus;
	FOD_EF_CONTRAST contrast;
	FOD_EF_F_FRAME f_frame;
	FOD_EF_LENZ_F lenz_f;
	FOD_EF_CROSS_F cross_f;
	FOD_EF_P_DEFORM1 pdf1;
	FOD_EF_P_DEFORM2 pdf2;
	FOD_EF_P_DEFORM3 pdf3;
	FOD_EF_FADE_SCR fade_scr;
	FOD_EF_FADE_MDL fade_mdl;
	FOD_EF_FIRE fire;
	FOD_EF_ITEM item;
	FOD_EF_AMULET amulet;
	FOD_EF_ENEMY enemy;
	FOD_EF_VIBRATE vibrate;
	u_char unit[16];
} FOD_EFF_DATA;

typedef struct {
	FOD_EFF_DATA *fix[12];
	FOD_EF_FADE_MDL *fade_mdl[4];
	sceVu0FVECTOR fire_pos[4];
	sceVu0FVECTOR lenz_pos;
	sceVu0FVECTOR lenz_rot;
	sceVu0FVECTOR pdf_pos;
	float pdf_spd;
	float pdf_rate;
	float pdf_trate;
	void *pdf_p;
	u_int fix_eff_num;
	u_int fade_mdl_cnt;
	u_int fire_num;
	u_int mono_flg;
	int mdl_efct_sw[4];
} FOD_EFF_PARAM;

typedef struct {
	u_int file_id;
	float version;
	u_int reso;
	u_int frame;
} FOD_FILE_HDR;

typedef struct {
	FOD_LIGHT fod_light;
	FOD_FILE_HDR *cam_file_hdr;
	FOD_CAM_FRAME *cam_frame_top;
	FOD_CAM_FRAME *cam_frame;
	FOD_FILE_HDR *lit_file_hdr;
	FOD_LIT_FRAME *lit_frame_top;
	FOD_LIT_FRAME *lit_frame;
	FOD_FILE_HDR *eff_file_hdr;
	FOD_EFF_FRAME *eff_frame_top;
	FOD_EFF_FRAME *eff_frame;
	u_int now_frame;
	u_int frame_max;
	u_int total_frame;
	u_int now_reso;
	u_char resolution;
	u_char end_flg;
#ifdef BUILD_EU_VERSION
	u_char check_cnt;
	u_char pad;
#else
	u_char pad[2];
#endif
} FOD_CTRL;

extern sceVu0FMATRIX fod_cmn_mtx;
extern FOD_EFF_PARAM eff_param;

void FodInit(FOD_CTRL *fc, u_int *tcp, u_int *tlp, u_int *tep);
int FodNextFrame(FOD_CTRL *fc);
void FodSetFrame(FOD_CTRL *fc, u_int frame);
void FodGetLightNum(FOD_LIGHT *fl);
void FodGetLightSerial(FOD_LIGHT *fl);
u_int* FodGetFirstLight(FOD_LIGHT *fl);
void FodSetMyLight(FOD_LIGHT *fl, char *pfx, float *eye);
void FodSetSpotLights(SgLIGHT *sl, u_int num);
void FodGetToSgLight(FOD_CTRL *fc);
void FodGetHandSpotPos(FOD_LIGHT *fl, float *p, float *i);
void FodGetDropSpotPos(FOD_LIGHT *fl, char *pfx, float *lp, float *li);
int FodGetLightType(FOD_LIGHT *fl, int id);
void FodGetFirstCam(SgCAMERA *cam, FOD_CTRL *fc);
void FodGetCamData(SgCAMERA *cam, FOD_CTRL *fc);
u_int* FodGetFixEffect(u_int *tep);
void FodSetEffect(FOD_CTRL *fc);
void FodSetEffectParam(FOD_EFF_DATA *fed);

#endif // GRAPHICS_SCENE_FOD_H
