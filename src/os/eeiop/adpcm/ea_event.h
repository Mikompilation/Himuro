#ifndef OS_EEIOP_ADPCM_EA_EVENT_H
#define OS_EEIOP_ADPCM_EA_EVENT_H

#include "typedefs.h"

void EAdpcmEventMain();
void AdpcmPlayEvent(int file_no, u_short start_flm, int fade_flm);
void AdpcmStopEvent(int fout_flm);

#endif // OS_EEIOP_ADPCM_EA_EVENT_H
