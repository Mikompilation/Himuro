#include "common.h"
#include "typedefs.h"
#include "wan_dat.h"

#include "wan_soul.h"

#include "data/soul_des100.h" // sceVu0FVECTOR soul_des100[/*2*/];
#include "data/soul_des101.h" // sceVu0FVECTOR soul_des101[/*5*/];
#include "data/soul_des102.h" // sceVu0FVECTOR soul_des102[/*2*/];
#include "data/soul_des103.h" // sceVu0FVECTOR soul_des103[/*2*/];
#include "data/soul_des105.h" // sceVu0FVECTOR soul_des105[/*6*/];
#include "data/soul_des110.h" // sceVu0FVECTOR soul_des110[/*2*/];
#include "data/soul_des112.h" // sceVu0FVECTOR soul_des112[/*2*/];
#include "data/soul_des114.h" // sceVu0FVECTOR soul_des114[/*2*/];
#include "data/soul_des119.h" // sceVu0FVECTOR soul_des119[/*2*/];
#include "data/soul_des120.h" // sceVu0FVECTOR soul_des120[/*5*/];
#include "data/soul_des121.h" // sceVu0FVECTOR soul_des121[/*2*/];
#include "data/soul_des123.h" // sceVu0FVECTOR soul_des123[/*2*/];
#include "data/soul_des128.h" // sceVu0FVECTOR soul_des128[/*5*/];
#include "data/soul_des130.h" // sceVu0FVECTOR soul_des130[/*2*/];
#include "data/soul_des131.h" // sceVu0FVECTOR soul_des131[/*2*/];
#include "data/soul_des132.h" // sceVu0FVECTOR soul_des132[/*2*/];
#include "data/soul_des202.h" // sceVu0FVECTOR soul_des202[/*2*/];
#include "data/soul_des203.h" // sceVu0FVECTOR soul_des203[/*5*/];
#include "data/soul_des204.h" // sceVu0FVECTOR soul_des204[/*2*/];
#include "data/soul_des206.h" // sceVu0FVECTOR soul_des206[/*2*/];
#include "data/soul_des211.h" // sceVu0FVECTOR soul_des211[/*2*/];
#include "data/soul_des212.h" // sceVu0FVECTOR soul_des212[/*2*/];
#include "data/soul_des214.h" // sceVu0FVECTOR soul_des214[/*2*/];
#include "data/soul_des215.h" // sceVu0FVECTOR soul_des215[/*2*/];
#include "data/soul_des216.h" // sceVu0FVECTOR soul_des216[/*2*/];
#include "data/soul_des000.h" // sceVu0FVECTOR soul_des000[/*10*/];

sceVu0FVECTOR *soul_des_array[/*26*/] = {
    soul_des100,
    soul_des101,
    soul_des102,
    soul_des103,
    soul_des105,
    soul_des110,
    soul_des112,
    soul_des114,
    soul_des119,
    soul_des120,
    soul_des121,
    soul_des123,
    soul_des128,
    soul_des130,
    soul_des131,
    soul_des132,
    soul_des202,
    soul_des203,
    soul_des204,
    soul_des206,
    soul_des211,
    soul_des212,
    soul_des214,
    soul_des215,
    soul_des216,
    soul_des000,
};

#include "data/wander_soul_dat.h" // WANDER_SOUL_DAT wander_soul_dat[/*26*/];

void SetWanSoDat2Wrk(int data_id, WANDER_SOUL_WRK *wswrk)
{
    wswrk->message_id = wander_soul_dat[data_id].message_id;
    wswrk->face_id = wander_soul_dat[data_id].face_id;
    wswrk->adpcm_id = wander_soul_dat[data_id].adpcm_id;

    if (wander_soul_dat[data_id].destination_id == 0xff)
    {
        wswrk->destination = NULL;
    }
    else
    {
        wswrk->destination = soul_des_array[wander_soul_dat[data_id].destination_id];
    }
}
