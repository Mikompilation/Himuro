#ifndef INGAME_ENTRY_AP_SGOST_H
#define INGAME_ENTRY_AP_SGOST_H

#include "typedefs.h"

typedef struct {
    u_short st_x0;
    short st_y0;
    u_short st_z0;
    u_short st_x1;
    short st_y1;
    u_short st_z1;
    u_short ed_x0;
    short ed_y0;
    u_short ed_z0;
    u_short ed_x1;
    short ed_y1;
    u_short ed_z1;
} SGEF_DAT;

typedef struct {
    u_char mode;
    u_char load_mode;
    u_char disp_mode;
    u_char dat_no;
    u_char sg_no;
    u_char bak_ene;
    short int time;
} SGST_WRK;

// extern SGEF_DAT sgef_dat[0];
// extern u_short sgef_adpcm_dat[0];
// extern SGST_WRK sgst_wrk;
// extern SgCAMERA sg_cmr_bak;

void SettleGhostInit();
int SettleGhostAppearReq(int sg_no);
void SettleGhostMain();
int SettleGhostLoadOneSize();
int SettleGhostLoadTwoSize();
void SettleGhostBattleSet();
int SettleGhostStartDisp();
int SettleGhostBattleEnd();
void SettleGhostDispEffect();

#endif // INGAME_ENTRY_AP_SGOST_H
