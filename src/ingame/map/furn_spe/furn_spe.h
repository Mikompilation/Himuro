#ifndef INGAME_MAP_FURN_SPE_FURN_SPE_H
#define INGAME_MAP_FURN_SPE_FURN_SPE_H

#include "typedefs.h"
#include "graphics/graph3d/libsg.h"

typedef struct {
	u_char eventflg;
	u_char data;
} FURN_DAT_SAVE;

typedef enum {
    ST_TREMBLE_VACANT = 0,
    ST_TREMBLE_NOR = 1,
    ST_TREMBLE_REV = 2
} ST_TREMBLE;

typedef struct {
	sceVu0FVECTOR *pos;
	sceVu0FVECTOR *rot;
	sceVu0FVECTOR ori_pos;
	sceVu0FVECTOR ori_rot;
	ST_TREMBLE state;
	u_char time;
	float speed;
	float nowdist;
	sceVu0FMATRIX mat;
} TREMBLE_H_WRK;

#include "ingame/map/furn_dat.h"
#include "ingame/enemy/move_ctl.h"

// extern sceVu0FVECTOR special_furn_pos[];
extern FURN_DAT_SAVE f_dat_save[430];
// extern FURN_ACT_WRK f_act_wrk[20];
// extern TREMBLE_H_WRK tremble_h_wrk[5];
// extern u_char fspe_miku_act;
extern u_char temp_miku_act;
// extern u_char thunder_time;
// extern u_char lightning_time;
// extern u_char thunder_state;
// extern u_char lightning_state;
// extern sceVu0FVECTOR *thunder_pos_p;
// extern sceVu0FVECTOR thunder_direction;
// extern u_short thunder_sound_no;
// extern u_char thunder_room;

void NewgameFActInit();
void InitFurnDatSave();
void PreGameInitFActWrk();
void InitFActWrk();
void FActWaitExecJob(FURN_ACT_WRK *fawp);
void FActWrkMain();
int SearchRegisterFW2FAW(u_short fw_id, u_short fact_no);
void ExchangeFAWFWID(int fw_id1, int fw_id2);
void FSpeFinishFlgOff(u_short furn_id);
// void FurnActOffEve(u_short furn_id); `EventMain` in ev_main.c needs them to implicitly declared
// void FurnActOnEve(u_short furn_id); `EventMain` in ev_main.c needs them to implicitly declared
int SearchFActWrkIDFromFurnID(u_short furn_id);
void AddRotVector(sceVu0FVECTOR rot, sceVu0FVECTOR rot1);
void FActCheckExec(FURN_ACT_WRK *fawp);
void FreeFActFwrk(FURN_ACT_WRK *fawp);
int SearchFurnWrkIDFromFurnID(u_short furn_id);
void FreeFActFwrkNo(int fw_id);
u_char* ForwardActionPointer(u_char *pointer);
u_char* ForwardConditionPointer(u_char *cp, u_char count);
int SetFurnActData(FURN_ACT_WRK *fawp, u_char fact_no);
void RegisterFW2FAW(u_short fw_id, FURN_ACT_WRK *fawp, u_short fact_no);
int GetOpenedFAWID();
int SearchFActWrkIDFromFWID(u_short fw_id);
void FActStopAction(FURN_ACT_WRK *fawp, void *point);
void FActExecOccur(FURN_ACT_WRK *fawp, void *point);
void FActWait(FURN_ACT_WRK *fawp);
int CompareFloatVal(float a, float b);
int ChkANDCondition(FURN_ACT_WRK *fawp, u_char **cpp, u_char count);
int FSpeCheckOutOfRoom(u_char room_no);
int FACheckCondition(FURN_ACT_WRK *fawp, SPE_CHK_COND *p_con);
int FACheckDirect(u_short range, u_short obj, float *fpos);
int RotCheck(float *fpos, MOVE_BOX *mbp, u_short range);
int FACheckDistNear(u_short obj, float dist, float *fpos);
int CheckSquareAreaOne(sceVu0FVECTOR pos, sceVu0FVECTOR square_center, sceVu0FMATRIX mat, float tate, float yoko);
int FACheckCircleArea(float x, float y, float z, float dist, u_short obj);
int FACheckSquareArea(float x, float y, float z, float tate, float yoko, float y_rot, u_short obj);
int FACheckDistFar(u_short obj, float dist, float *fpos);
int FACheckStatus(u_short a, u_short b);
int SpeedLessOrVecF(float s_speed, sceVu0FVECTOR vec);
int SpeedLessOrVecVec(sceVu0FVECTOR vec1, sceVu0FVECTOR vec2);
int FACheckSpeedLess(float speed, u_short obj);
TextureAnimation* FSpeTexAnmCtrl(TextureAnimation *pta, FSPE_TEXTURE_ANM *ta);
void FSpeTexAnmStop(FURN_ACT_WRK *fawp);
void FSpeTexAnmExe(u_char modelnum, FURN_ACT_WRK *fawp, u_char regmode, u_char looptype, u_char speed);
void FSpeDelPointLight(FURN_ACT_WRK *fawp);
void FSpeSetPointLight(FURN_ACT_WRK *fawp, float r, float g, float b);
float GetRandVal(float min, float max);
void StopTrembleH(FURN_ACT_WRK *fawp, signed char num);
signed char GetVacantTrembleHWrk();
signed char CallTrembleH(FURN_ACT_WRK *fawp);
void TrembleHCtrl();
void FSpeTrembleCtrl(FURN_ACT_WRK *fawp);
void CallThunderLight(float x, float y, float z, u_char delay, sceVu0FVECTOR *pos, u_char room_no);
void SetThunderLightEach(PARARELL_WRK *par, int *num);
void SetThunderLight();
void InitThunderLight();
void ThunderLight();
u_char* CallLampTremble(u_char *pointer);
u_char* StopLampTremble(u_char *pointer);

#endif // INGAME_MAP_FURN_SPE_FURN_SPE_H
