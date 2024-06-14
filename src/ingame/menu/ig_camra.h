#ifndef INGAME_MENU_IG_CAMRA_H
#define INGAME_MENU_IG_CAMRA_H

#include "typedefs.h"
#include "structs.h"

typedef struct {
	u_char flsh_tmr;
	u_char alpha;
} FLSH_CORE;

// extern int camera_power_up_point[3][3];
// extern int camera_sub_abi_point[5];
// extern int camera_spe_abi_point[5];
// extern CAMERA_MENU_WRK cmr_menu_wrk;
// extern FLSH_CORE flsh;
// extern FLSH_CORE flsh2;

void CameraCustomNewgameInit();
void CameraCustomInit();
void CameraCustomMain();
void CameraCustomMenuSlct(char *err);
void CameraCustomFilm(char *err);
void CameraCustomPowerUp(char *err);
void CameraCustomSubAbility(char *err);
void CameraCustomSpecialAbility(char *err);
void CameraDspCtrl(u_char err);
void CameraDsp(short int pos_x, short int pos_y, u_char alp, u_char msg);
char StrKndChk(u_char err);
char FilmPossChk(u_char knd);
void CameraModeInOut();
void CameraModeInOut2();
void CameraCntRenew();
int isCameraTopMenu();
void OutGameInitCamera();
int isCameraEnd();
float GetCamDispAlpha();
void OpenCameraMenu();
float NeonAlp(short int num, short int exe, short int dly, short int stp, short int no, short int *timer);
float BeMax(u_char no);

#endif // INGAME_MENU_IG_CAMRA_H
