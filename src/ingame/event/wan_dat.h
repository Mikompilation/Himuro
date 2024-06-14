#ifndef INGAME_EVENT_WAN_DAT_H
#define INGAME_EVENT_WAN_DAT_H

#include "typedefs.h"

typedef struct {
	u_char destination_id;
	u_char message_id;
	u_short face_id;
	u_short adpcm_id;
} WANDER_SOUL_DAT;

#include "ingame/event/wan_soul.h"

// extern sceVu0FVECTOR soul_des100[0];
// extern sceVu0FVECTOR soul_des101[0];
// extern sceVu0FVECTOR soul_des102[0];
// extern sceVu0FVECTOR soul_des103[0];
// extern sceVu0FVECTOR soul_des105[0];
// extern sceVu0FVECTOR soul_des110[0];
// extern sceVu0FVECTOR soul_des112[0];
// extern sceVu0FVECTOR soul_des114[0];
// extern sceVu0FVECTOR soul_des119[0];
// extern sceVu0FVECTOR soul_des120[0];
// extern sceVu0FVECTOR soul_des121[0];
// extern sceVu0FVECTOR soul_des123[0];
// extern sceVu0FVECTOR soul_des128[0];
// extern sceVu0FVECTOR soul_des130[0];
// extern sceVu0FVECTOR soul_des131[0];
// extern sceVu0FVECTOR soul_des132[0];
// extern sceVu0FVECTOR soul_des202[0];
// extern sceVu0FVECTOR soul_des203[0];
// extern sceVu0FVECTOR soul_des204[0];
// extern sceVu0FVECTOR soul_des206[0];
// extern sceVu0FVECTOR soul_des211[0];
// extern sceVu0FVECTOR soul_des212[0];
// extern sceVu0FVECTOR soul_des214[0];
// extern sceVu0FVECTOR soul_des215[0];
// extern sceVu0FVECTOR soul_des216[0];
// extern sceVu0FVECTOR soul_des000[0];
// extern sceVu0FVECTOR *soul_des_array[0];
// extern WANDER_SOUL_DAT wander_soul_dat[0];

void SetWanSoDat2Wrk(int data_id, WANDER_SOUL_WRK *wswrk);

#endif // INGAME_EVENT_WAN_DAT_H
