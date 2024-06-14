#ifndef OUTGAME_OUTGAME_H
#define OUTGAME_OUTGAME_H

#include "typedefs.h"

void OutGameCtrl();
void OutGameModeChange(u_char mode);
void OrgSetSquare(int pri, float x, float y, float w, float h, u_char r, u_char g, u_char b, u_char a);
void RoomSizeDisp();
void RoomSizeCheckCtrl();
void RoomSizeCheckInit();

#endif // OUTGAME_OUTGAME_H
