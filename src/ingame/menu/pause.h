#ifndef INGAME_MENU_PAUSE_H
#define INGAME_MENU_PAUSE_H

#include "typedefs.h"

typedef struct {
	u_char lock;
	u_char no_pause;
} PAD_CTRL;

typedef struct {
	u_char mode;
	u_char opti;
	u_char csr[3];
	u_short timer;
} PAUSE_WRK;

extern PAD_CTRL pad_ctrl;
// extern PAUSE_WRK pause_wrk;
// extern char in_pause;

void PauseInit();
int PauseMain();
void PauseDisp();

#endif // INGAME_MENU_PAUSE_H
