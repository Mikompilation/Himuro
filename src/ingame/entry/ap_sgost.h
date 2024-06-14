#ifndef INGAME_ENTRY_AP_SGOST_H
#define INGAME_ENTRY_AP_SGOST_H

#include "typedefs.h"

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
