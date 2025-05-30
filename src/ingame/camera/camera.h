#ifndef INGAME_CAMERA_CAMERA_H
#define INGAME_CAMERA_CAMERA_H

#include "typedefs.h"

typedef struct {
	sceVu0FVECTOR p;
	sceVu0FVECTOR i;
	float roll;
	float fov;
	float nearz;
	float farz;
	float ax;
	float ay;
	float cx;
	float cy;
	float zmin;
	float zmax;
	float pad[2];
	sceVu0FMATRIX vs;
	sceVu0FMATRIX vc;
	sceVu0FMATRIX vcv;
	sceVu0FMATRIX wv;
	sceVu0FMATRIX ws;
	sceVu0FMATRIX wc;
	sceVu0FMATRIX wcv;
	sceVu0FVECTOR zd;
	sceVu0FVECTOR yd;
} SgCAMERA;

typedef struct {
	u_char type;
	u_char id;
	u_short p0[3];
	u_short p1[3];
	u_short p2[3];
	u_short p3[3];
	float roll[2];
	float fov[2];
} MAP_CAM_DAT;

typedef struct
{ // 0xc
    /* 0x0 */ u_short no;
    /* 0x2 */ u_short no_old;
    /* 0x4 */ u_char kind;
    /* 0x5 */ u_char type;
    /* 0x6 */ u_char change;
    /* 0x7 */ u_char pad;
    /* 0x8 */ MAP_CAM_DAT *mcd;
} MAP_CAM_INFO;

// extern u_int ncam_000[0];
// extern u_int ncam_001[0];
// extern u_int ncam_002[0];
// extern u_int ncam_003[0];
// extern u_int ncam_006[0];
// extern u_int ncam_007[0];
// extern u_int ncam_008[0];
// extern u_int ncam_009[0];
// extern u_int ncam_010[0];
// extern u_int ncam_011[0];
// extern u_int ncam_012[0];
// extern u_int ncam_013[0];
// extern u_int ncam_014[0];
// extern u_int ncam_015[0];
// extern u_int ncam_016[0];
// extern u_int ncam_021[0];
// extern u_int ncam_022[0];
// extern u_int ncam_023[0];
// extern u_int ncam_024[0];
// extern u_int ncam_025[0];
// extern u_int ncam_026[0];
// extern u_int *ncam_comple_dat1[0];
// extern u_int *ncam_comple_dat2[0];
// extern u_int *ncam_comple_dat3[0];
// extern u_int *ncam_comple_dat4[0];
// extern u_int bcam_000[0];
// extern u_int *bcam_comple_dat1[0];
// extern u_int *bcam_comple_dat2[0];
// extern u_int *bcam_comple_dat3[0];
// extern u_int *bcam_comple_dat4[0];
// extern u_int dcam_000[0];
// extern u_int *dcam_comple_dat1[0];
// extern u_int *dcam_comple_dat2[0];
// extern u_int *dcam_comple_dat3[0];
// extern u_int *dcam_comple_dat4[0];
// extern u_short dc0000[0];
// extern u_short dc0010[0];
// extern u_short dc0020[0];
// extern u_short dc0030[0];
// extern u_short dc0040[0];
// extern u_short dc0050[0];
// extern u_short dc0060[0];
// extern u_short dc0070[0];
// extern u_short dc0080[0];
// extern u_short dc0090[0];
// extern u_short dc0100[0];
// extern u_short dc0110[0];
// extern u_short dc0120[0];
// extern u_short dc0130[0];
// extern u_short dc0140[0];
// extern u_short dc0150[0];
// extern u_short dc0160[0];
// extern u_short dc0170[0];
// extern u_short dc0180[0];
// extern u_short dc0190[0];
// extern u_short dc0200[0];
// extern u_short dc0210[0];
// extern u_short dc0220[0];
// extern u_short dc0230[0];
// extern u_short dc0240[0];
// extern u_short dc0250[0];
// extern u_short dc0260[0];
// extern u_short dc0270[0];
// extern u_short dc0280[0];
// extern u_short dc0290[0];
// extern u_short dc0300[0];
// extern u_short dc0310[0];
// extern u_short dc0320[0];
// extern u_short dc0330[0];
// extern u_short dc0340[0];
// extern u_short dc0350[0];
// extern u_short dc0360[0];
// extern u_short dc0370[0];
// extern u_short dc0380[0];
// extern u_short dc0390[0];
// extern u_short dc0400[0];
// extern u_short dc0410[0];
// extern u_short dc0420[0];
// extern u_short dc0430[0];
// extern u_short dc0440[0];
// extern u_short dc0450[0];
// extern u_short dc0460[0];
// extern u_short dc0470[0];
// extern u_short dc0480[0];
// extern u_short dc0490[0];
// extern u_short dc0500[0];
// extern u_short dc0510[0];
// extern u_short *dc_no_tbl0[0];
// extern u_short dc1000[0];
// extern u_short dc1010[0];
// extern u_short dc1020[0];
// extern u_short dc1030[0];
// extern u_short dc1040[0];
// extern u_short dc1050[0];
// extern u_short dc1060[0];
// extern u_short dc1070[0];
// extern u_short dc1080[0];
// extern u_short dc1090[0];
// extern u_short dc1100[0];
// extern u_short dc1110[0];
// extern u_short dc1120[0];
// extern u_short dc1130[0];
// extern u_short dc1140[0];
// extern u_short dc1150[0];
// extern u_short dc1160[0];
// extern u_short dc1170[0];
// extern u_short dc1180[0];
// extern u_short dc1190[0];
// extern u_short dc1200[0];
// extern u_short dc1210[0];
// extern u_short dc1220[0];
// extern u_short dc1230[0];
// extern u_short dc1240[0];
// extern u_short dc1250[0];
// extern u_short dc1260[0];
// extern u_short dc1270[0];
// extern u_short dc1280[0];
// extern u_short dc1290[0];
// extern u_short *dc_no_tbl1[0];
// extern u_short dc2000[0];
// extern u_short dc2010[0];
// extern u_short dc2020[0];
// extern u_short dc2030[0];
// extern u_short dc2040[0];
// extern u_short dc2050[0];
// extern u_short dc2060[0];
// extern u_short dc2070[0];
// extern u_short dc2080[0];
// extern u_short dc2090[0];
// extern u_short dc2100[0];
// extern u_short dc2110[0];
// extern u_short dc2120[0];
// extern u_short dc2130[0];
// extern u_short dc2140[0];
// extern u_short dc2150[0];
// extern u_short dc2160[0];
// extern u_short dc2170[0];
// extern u_short dc2180[0];
// extern u_short dc2190[0];
// extern u_short dc2200[0];
// extern u_short dc2210[0];
// extern u_short dc2220[0];
// extern u_short dc2230[0];
// extern u_short dc2240[0];
// extern u_short dc2250[0];
// extern u_short dc2260[0];
// extern u_short dc2270[0];
// extern u_short dc2280[0];
// extern u_short dc2290[0];
// extern u_short *dc_no_tbl2[0];
// extern u_short dc3000[0];
// extern u_short dc3010[0];
// extern u_short dc3020[0];
// extern u_short dc3030[0];
// extern u_short dc3040[0];
// extern u_short dc3050[0];
// extern u_short dc3060[0];
// extern u_short dc3070[0];
// extern u_short dc3080[0];
// extern u_short dc3090[0];
// extern u_short dc3100[0];
// extern u_short dc3110[0];
// extern u_short dc3120[0];
// extern u_short dc3130[0];
// extern u_short dc3140[0];
// extern u_short dc3150[0];
// extern u_short dc3160[0];
// extern u_short dc3170[0];
// extern u_short dc3180[0];
// extern u_short dc3190[0];
// extern u_short dc3200[0];
// extern u_short dc3210[0];
// extern u_short dc3220[0];
// extern u_short dc3230[0];
// extern u_short dc3240[0];
// extern u_short dc3250[0];
// extern u_short dc3260[0];
// extern u_short dc3270[0];
// extern u_short dc3280[0];
// extern u_short dc3290[0];
// extern u_short *dc_no_tbl3[0];
// extern u_short drm_cam_no[12];
extern u_char DBG_cam_id_move_chk;
// extern u_char cd_edit_end;
// extern int msn_no;
// extern int renewal_data_chk;
// extern int renewal_data_chk_cnt;
// extern u_short cdcopy[2];
// extern u_char cam_info_disp;
// extern sceVu0FVECTOR adj_cam_pos;
// extern u_short fior_tm;
// extern CAM_ID_MOVE cam_id_move;
// extern int cam_kind;
// extern int cam_type;
// extern int cd_step;
// extern int cam_id;
// extern short int plyr_adj[4];

void CameraMain();
void KonwakuCamCtrl();
void FinderInCameraCtrl();
void NormalCameraCtrl();
int GetCameraInfo(MAP_CAM_INFO *mci);
void GetCameraData(u_char kind);
void SetCamPos0(SgCAMERA *tc);
void SetCamPos1(SgCAMERA *tc);
void SetCamPos2(SgCAMERA *tc);
void SetCamPos3(SgCAMERA *tc);
void SetCamPos4(SgCAMERA *tc);
void SetCamPos5(SgCAMERA *tc);
float GetMCLocalPosPer(u_short cn, u_char kind, u_char id);
void CompleCameraPos(SgCAMERA *tc, SgCAMERA *oc);
u_char CompleReqChk();
void FinderModeCameraCtrl();
void PlyrCamCondChk();
void PconMahiCameraCtrl();
void PconTebureCameraCtrl();
int FinderModePadChk(char *pad_x, char *pad_y, float *ax, float *ay, u_char *jpad_on);
void SetFinderRot();
void CameraIdMoveCtrl();
u_char SetMapCamDat0(MAP_CAM_DAT *mcd);
u_char SetMapCamDat1(MAP_CAM_DAT *mcd);
u_char SetMapCamDat2(MAP_CAM_DAT *mcd, u_char id);
u_char SetMapCamDat3(MAP_CAM_DAT *mcd, u_char id);
u_char SetMapCamDat4(MAP_CAM_DAT *mcd, u_char id);
u_char SetMapCamDat5(MAP_CAM_DAT *mcd);
void SaveCamDat(u_char kind, u_char mn);

#ifdef MATCHING_DECOMP
#ifdef INCLUDING_FROM_CAMERA_C
// fix for undeclared ReqFinderInOverRap in header
void ReqFinderInOverRap(u_short time);
#endif
#else
void ReqFinderInOverRap(u_short time);
#endif

void FinderInOverRapCtrl();
void PlyrDmgCameraCtrl();
void ReqDramaCamera(u_char req, u_short no, u_short time);
void DramaCameraReqCtrl();
void ClearDramaCamReq();

#endif // INGAME_CAMERA_CAMERA_H
