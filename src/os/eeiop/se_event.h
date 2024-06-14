#ifndef OS_EEIOP_SE_EVENT_H
#define OS_EEIOP_SE_EVENT_H

#include "typedefs.h"

int SEventLoadReqAndSetSub(int load_id, u_char room_from, u_char room_to);
int SEventLoadReqAndSet(int load_id, u_short door_id);
u_char SEventGetSeNo(int file_no);

#endif // OS_EEIOP_SE_EVENT_H
