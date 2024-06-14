#ifndef OS_EEIOP_ADPCM_EA_GDEAD_H
#define OS_EEIOP_ADPCM_EA_GDEAD_H

#include "typedefs.h"

void EAdpcmGdeadMain();
void AdpcmPlayGhostDead(int file_no, u_short vol, int fade_flm);
void AdpcmStopGhostDead(int fout_flm);

#endif // OS_EEIOP_ADPCM_EA_GDEAD_H
