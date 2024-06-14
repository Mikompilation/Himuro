#ifndef INGAME_EVENT_MV_CMND_H
#define INGAME_EVENT_MV_CMND_H

#include "typedefs.h"

// extern MOVIE_CMD_WRK mvcmd_wrk;

void EventMovieCommandInit();
void EventMovieCommandMain(int flame, int top_addr);
int GetMovieMessageAddr(short int msg_no);

#endif // INGAME_EVENT_MV_CMND_H
