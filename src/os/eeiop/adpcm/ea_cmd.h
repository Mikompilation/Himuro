#ifndef OS_EEIOP_ADPCM_EA_CMD_H
#define OS_EEIOP_ADPCM_EA_CMD_H

#include "typedefs.h"

void EAdpcmCmdInit(u_int dev_init);
void EAdpcmCmdPlay(u_char channel, u_char loop, int file_no, int start_flm, u_short vol, u_short pan, u_short pitch, u_short fin_flm);
void EAdpcmCmdPreload(u_char channel, int file_no, int start_flm, u_short fin_flm);
void EAdpcmCmdPreEndPlay(u_char channel, u_char loop, int file_no, u_short vol, u_short pan, u_short pitch, u_short fin_flm);
void EAdpcmCmdStop(u_char channel, int file_no, u_short fout_flm);
void EAdpcmCmdPause(u_char channel, int file_no, u_short fout_flm);
void EAdpcmCmdRestart(u_char channel, int file_no, u_short fin_flm);
void EAdpcmCmdFadeVol(u_char channel, int file_no, u_short vol_percent, u_short fin_flm);
void EAdpcmCmdPos(u_char channel, int file_no, u_short vol, u_short pan, u_short pitch);
short int EAGetRetStat();
short int EAGetRetTune();
int EAGetRetCount();
u_char IsAdpcmPlay(int file_no);
u_short EAdpcmGetPan(float rot);
u_short EAdpcmGetVol(float dist, u_short vol);
u_short EAdpcmGetPitch(float rot, float dist, u_short pitch);

#endif // OS_EEIOP_ADPCM_EA_CMD_H
