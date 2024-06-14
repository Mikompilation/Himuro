#ifndef INGAME_PLYR_TIME_CTL_H
#define INGAME_PLYR_TIME_CTL_H

#include "typedefs.h"

#include "os/system.h"

void TimeCtrlInit();
void GameTimeCtrl();
void SetNowClock(sceCdCLOCK *nc);
void PlyrTimerCtrl();
void EntryTimeCtrl();
void SetRealTime();

#endif // INGAME_PLYR_TIME_CTL_H
