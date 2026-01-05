#ifndef INGAME_EVENT_MV_CMND_H
#define INGAME_EVENT_MV_CMND_H

#include "typedefs.h"

typedef struct {
    short int msg_no;
    short int msg_wait;
} MOVIE_CMD_WRK;

// extern MOVIE_CMD_WRK mvcmd_wrk;

void EventMovieCommandInit();
void EventMovieCommandMain(int flame, int top_addr);
int GetMovieMessageAddr(short int msg_no);

#endif // INGAME_EVENT_MV_CMND_H
