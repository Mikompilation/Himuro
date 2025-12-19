#ifndef INGAME_ENTRY_AP_FGOST_H
#define INGAME_ENTRY_AP_FGOST_H

#include "typedefs.h"

typedef struct {
    u_char mode;
    u_char load_id;
    u_char load_num;
    u_char load_no;
} FG_LOAD_WRK;

extern int load_mdl_addr[];
extern int load_mot_addr[];
extern int load_se_addr[];
extern FG_LOAD_WRK fg_load_wrk;

void FloatGhostAppearInit();
int FloatGhostEntrySet(u_char area);
int FloatGhostAppearMain();
int FloatGhostAppearJudge();
// int FloatGhostAppearTypeSet(u_char fg_no, u_char wrk_no, u_char room);
int GetFloatGhostAppearPosType(u_char *type);
void FloatGhostAppearPosSet(u_char dat_no, float *set_pos, float *set_rot);
int FloatGhostBattleEnd();
int FloatGhostEscapeEnd();
void FloatGhostLoadReq();
int FloatGhostLoadMain();
int FloatGhostLoadSet();
int FloatGhostSetJudge();
void GetLoadFloatGhost(u_char set_num, u_char *set_fgst);
void GetFloatGhostModelLoad();
void GetFloatGhostModelLoadAfter();
void GetFloatGhostMotionLoad();
void GetFloatGhostMotionLoadAfter();
void GetFloatGhostSELoad();
void FloatGhostAppearStop();
void FloatGhostAppearStart();

#endif // INGAME_ENTRY_AP_FGOST_H
