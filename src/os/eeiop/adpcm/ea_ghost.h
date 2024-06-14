#ifndef OS_EEIOP_ADPCM_EA_GHOST_H
#define OS_EEIOP_ADPCM_EA_GHOST_H

#include "typedefs.h"

void AdpcmPlayGhost(int file_no, sceVu0FVECTOR *mpos, u_short vol, u_char ewrk_no, int fade_flm);
void AdpcmStopGhost(int fout_flm);
void EAdpcmGhostMain();

#endif // OS_EEIOP_ADPCM_EA_GHOST_H
