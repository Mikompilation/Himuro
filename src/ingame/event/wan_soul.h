#ifndef INGAME_EVENT_WAN_SOUL_H
#define INGAME_EVENT_WAN_SOUL_H

#include "typedefs.h"

typedef enum
{
    ST_WANSO_VACANT = 0,
    ST_WANSO_INIT = 1,
    ST_WANSO_WANDER = 2,
    ST_WANSO_SET_MOVE = 3,
    ST_WANSO_MOVE = 4,
    ST_WANSO_SPEED_DOWN = 5,
    ST_WANSO_SPLINE = 6,
    ST_WANSO_FLOAT = 10,
    ST_WANSO_WAIT = 11,
    ST_WANSO_TELLIN = 12,
    ST_WANSO_ADPCM_OUT = 13,
    ST_WANSO_TELL = 14,
    ST_WANSO_EXTINCT = 20,
    SOUL_LIGHT_WAIT = 21,
    SOUL_LIGHT_BIGGER = 22,
    SOUL_LIGHT_LESSER = 23,
    SOUL_LIGHT_END = 24,
    SOUL_LIGHT_VANISH_IN = 30,
    SOUL_LIGHT_VANISH = 31,
    SOUL_LIGHT_APPEAR_IN = 32,
    SOUL_LIGHT_APPEAR = 33
} EN_ST_WANDER_SOUL;

typedef struct {
	EN_ST_WANDER_SOUL state;
	EN_ST_WANDER_SOUL vanish;
	u_char id;
	u_char disp_flg;
	u_char turn;
	u_char move_flg;
	u_short count;
	float spl_speed;
	sceVu0FMATRIX sp_mat;
	sceVu0FVECTOR ori_pos;
	sceVu0FVECTOR disp_pos;
	sceVu0FVECTOR speedv;
	sceVu0FVECTOR accelv;
	u_char type;
	u_char message_id;
	u_short face_id;
	u_short adpcm_id;
	u_char spl_flg;
	sceVu0FVECTOR *destination;
	u_char room_no;
	float dist;
	void *eff_buff;
	u_int wanso_wait_time;
	float srate;
	float arate;
	float lightpower;
} WANDER_SOUL_WRK;

typedef struct {
	u_char r1;
	u_char g1;
	u_char b1;
	float scale1;
	u_char r2;
	u_char g2;
	u_char b2;
	float scale2;
} SOUL_EFF_PARAM;

#include "ingame/enemy/move_ctl.h"

// extern WANDER_SOUL_WRK wander_soul_wrk[1];

sceVu0FVECTOR* SetFirstDestination(sceVu0FVECTOR *destination, float *pos);
float GetDist3(float *v1, float *v2);
float GetDistPlyr(float *plyr, float *soul);
void* SetEffectsWithScale(float *pos, SOUL_EFF_PARAM *param, float scale, float rgb);
int GetUsableWanSoWrkID();
void SetNewSpeed(WANDER_SOUL_WRK *wswrk);
void NewMissionInitWanderSoul();
void ReviveWanderSoul();
void BSplinePos(float *pos, float *mat_point_array[4], float t1);
void ClearWanderSoulOne();
void CallWanderSoul(u_char data_id, float *pos);
void RegisterWansoEffect();
void WansoLateInit();
void ReleaseWanderSoul(u_char data_id);
int WanSoNearJudge(float dist, float distv);
int WanSoTouchJudge(float *soul_pos, MOVE_BOX *mbp);
void SoulFloating(float *pos, float *rgb);
void SetSoulNewMove();
void Change2WanSoExtinct();
void FinishWanSoWrk();
void ComeOnSignal(float *scale, float *rgb, u_short time);
float StopAbleDist(float speed, float accel, u_short *count);
float GetRot(float *a, float *b);
int RotCheck2(float rot1, float rot2, u_short range);
void GetBSplinePos(float *pos, float t1);
int SetMoveDirect(float s2p_len);
void AddVec2MatD(float *mat[4], float *vec);
void Change2WanSoFloat();
float GetKnotDist(float *mat[4]);
void SetWansoSpeed(float dist);
void WansoPreTell();
u_char WansoAlphaCtrl(float dist);
void WansoExtinctCtrl();
void OneSoulCtrl(float *srate, float *arate);
void WanderSoulCtrl();
u_char CallSoulTellingCamera(float *soul_pos, float *relative_camera_pos, float *relative_interest, float *relative_soul_pos);
void CallSoulTellingCameraIn(float *soul_pos, float *mic_eye_pos, int time);
int SoulTellingCameraInCtrl();

#endif // INGAME_EVENT_WAN_SOUL_H
