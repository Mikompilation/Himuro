#ifndef OS_EEIOP_ADPCM_EA_TAPE_H
#define OS_EEIOP_ADPCM_EA_TAPE_H

#include "typedefs.h"

void EAdpcmTapeMain();
void AdpcmPlayTape(int file_no, u_short vol, int fade_flm);
void AdpcmStopTape(int fout_flm);
u_char IsEndAdpcmTape();

#endif // OS_EEIOP_ADPCM_EA_TAPE_H
