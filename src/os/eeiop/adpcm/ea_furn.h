#ifndef OS_EEIOP_ADPCM_EA_FURN_H
#define OS_EEIOP_ADPCM_EA_FURN_H

#include "typedefs.h"

void EAdpcmFurnMain();
void AdpcmPlayFurn(int file_no, float *pos, u_short vol, int fade_flm);
void AdpcmStopFurn(int fout_flm);

#endif // OS_EEIOP_ADPCM_EA_FURN_H
