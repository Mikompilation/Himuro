#ifndef INGAME_EVENT_EV_MAIN_H
#define INGAME_EVENT_EV_MAIN_H

#include "typedefs.h"

typedef struct {
	u_char mode;
	u_char evt_no;
	u_char movie_on;
	u_char next_msn;
	u_char msg_init;
	u_char use_item;
	u_char get_item;
	u_char btl_ene;
	u_char end_ev;
	u_char btl_lock;
	u_char pht_2d;
	u_char pht_furn;
	u_char pht_ev[10];
	u_char pos_req[16];
	u_char spev_tmp[4];
	u_char gst_door[2];
	u_char face_stts[4];
} EVENT_WRK;

// extern u_char event_stts[250];
extern EVENT_WRK ev_wrk;
// extern int black_time;
// extern int load_file_id;
// extern int ending_mode;
// extern int clear_disp;
// extern int clear_disp_mode;
// extern int first_clear_flg;

void EventWrkInit();
void EventInit();
void EventMain();
void EventOpenMain();
u_char EventOpenCtrl();
u_char EventOpenJudge(short int event_no);
int GetEventMessageAddr(short int msg_no);
void EventEnemySet(u_char *addr);
int BattleEndEventOpenJudge(u_char ene_no);
int PhotoEventOpenJudge(u_char pht_2d, u_char pht_furn);
float SetRot360(short int rot360);
int EventSceneCtrl(short int scene_no);
int SceneFadeOut();
void InterMissionInit();
void InterMissionEnd();
void InterMissionMain();
void DoorLockBattleSet();
void DoorLockBattleAfter();
void LockBattleDoorOpen();
int LockBattleAfterJudge();
void LockBattleDoorOpenMSGDisp();
int ScenePlay(short int scene_no);
void EndingInit();
void EndingMain();

#endif // INGAME_EVENT_EV_MAIN_H
