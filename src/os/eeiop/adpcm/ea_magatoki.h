#ifndef OS_EEIOP_ADPCM_EA_MAGATOKI_H
#define OS_EEIOP_ADPCM_EA_MAGATOKI_H

#include "typedefs.h"

void EAdpcmMagatokiMain();
void AdpcmPreMagatokiFadeOut(u_short fout_flm);
u_char IsFadeoutEndAdpcmMagatoki();
void AdpcmMagatokiPlay(int file_no);
void AdpcmMagatokiStop();

#endif // OS_EEIOP_ADPCM_EA_MAGATOKI_H
