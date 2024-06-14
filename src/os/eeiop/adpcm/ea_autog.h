#ifndef OS_EEIOP_ADPCM_EA_AUTOG_H
#define OS_EEIOP_ADPCM_EA_AUTOG_H

#include "typedefs.h"

void AdpcmPlayAutoGhost(int file_no, sceVu0FVECTOR *mpos, u_short vol, u_char ewrk_no, int fade_flm);
void AdpcmStopAutoGhost(int fout_flm);
void EAdpcmAutogMain();

#endif // OS_EEIOP_ADPCM_EA_AUTOG_H
