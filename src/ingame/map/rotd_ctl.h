#ifndef INGAME_MAP_ROTD_CTL_H
#define INGAME_MAP_ROTD_CTL_H

#include "typedefs.h"

int CheckRotDoorOpenStatus(int dwrk_no, u_char type);
void ChangeRotDoorFace(u_short door_id);
void ChangeRotDoorFaceRoomId(u_short door_id, u_char room_id);
void ChangeRotDoorFace2Room(u_short door_id, u_char room_id);
int JudgePlyrRotForRotDoor(int dwrk_no);
u_char ChkRotDoorRot(int dwrk_no);

#endif // INGAME_MAP_ROTD_CTL_H
