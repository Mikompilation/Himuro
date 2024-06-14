#include "common.h"
#include "typedefs.h"

#include "main/glob.h"

u_short *default_key[32] = {
    &pad[0].cnt[0],  &pad[0].cnt[1],  &pad[0].cnt[2],  &pad[0].cnt[3],
    &pad[0].cnt[4],  &pad[0].cnt[5],  &pad[0].cnt[6],  &pad[0].cnt[7],
    &pad[0].cnt[8],  &pad[0].cnt[9],  &pad[0].cnt[10], &pad[0].cnt[11],
    &pad[0].cnt[12], &pad[0].cnt[13], &pad[0].cnt[14], &pad[0].cnt[15],
    &pad[1].cnt[0],  &pad[1].cnt[1],  &pad[1].cnt[2],  &pad[1].cnt[3],
    &pad[1].cnt[4],  &pad[1].cnt[5],  &pad[1].cnt[6],  &pad[1].cnt[7],
    &pad[1].cnt[8],  &pad[1].cnt[9],  &pad[1].cnt[10], &pad[1].cnt[11],
    &pad[1].cnt[12], &pad[1].cnt[13], &pad[1].cnt[14], &pad[1].cnt[15],
};
u_short *default_key_bak[32] = {
    &pad[0].cnt_bak[0],  &pad[0].cnt_bak[1],  &pad[0].cnt_bak[2],  &pad[0].cnt_bak[3],
    &pad[0].cnt_bak[4],  &pad[0].cnt_bak[5],  &pad[0].cnt_bak[6],  &pad[0].cnt_bak[7],
    &pad[0].cnt_bak[8],  &pad[0].cnt_bak[9],  &pad[0].cnt_bak[10], &pad[0].cnt_bak[11],
    &pad[0].cnt_bak[12], &pad[0].cnt_bak[13], &pad[0].cnt_bak[14], &pad[0].cnt_bak[15],
    &pad[1].cnt_bak[0],  &pad[1].cnt_bak[1],  &pad[1].cnt_bak[2],  &pad[1].cnt_bak[3],
    &pad[1].cnt_bak[4],  &pad[1].cnt_bak[5],  &pad[1].cnt_bak[6],  &pad[1].cnt_bak[7],
    &pad[1].cnt_bak[8],  &pad[1].cnt_bak[9],  &pad[1].cnt_bak[10], &pad[1].cnt_bak[11],
    &pad[1].cnt_bak[12], &pad[1].cnt_bak[13], &pad[1].cnt_bak[14], &pad[1].cnt_bak[15],
};
u_char key_type[32] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
};

void SetDefaultKeyType()
{
    memcpy(key_now, default_key, 32 * sizeof(u_short));
    memcpy(key_bak, default_key_bak, 32 * sizeof(u_short));
}

void SetKeyType()
{
    u_char i;

    for (i = 0; i < 16; i++)
    {
        key_now[i] = default_key[key_type[i]];
        key_bak[i] = default_key_bak[key_type[i]];
    }
}
