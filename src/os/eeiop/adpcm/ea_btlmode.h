#ifndef OS_EEIOP_ADPCM_EA_BTLMODE_H
#define OS_EEIOP_ADPCM_EA_BTLMODE_H

#include "typedefs.h"

void EAdpcmBtlmodeMain();
void AdpcmPreBtlmodeFadeOut(u_short fout_flm);
u_char IsEndAdpcmBtlmode();
void AdpcmPreBtlmodePreload(int file_no);
u_char IsPreLoadEndAdpcmBtlmode();
void AdpcmBtlmodePreLoadEndPlay();
void AdpcmBtlmodePlay(int file_no);
void AdpcmBtlmodeEnd();

#endif // OS_EEIOP_ADPCM_EA_BTLMODE_H
