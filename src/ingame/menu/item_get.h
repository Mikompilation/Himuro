#ifndef INGAME_MENU_ITEM_GET_H
#define INGAME_MENU_ITEM_GET_H

#include "typedefs.h"

// extern TAPE_DAT tape_dat[0];
// extern int play_tape_timer;
// extern char tape_play;

void ItemGet(u_char get_type, u_char get_no, u_char msg0_no, u_char msg1_no);
int ItemGetCtrl();
void NextPageNavi(u_char hav_now, u_char dsp_max, u_short chr_top, short int ofs_x, short int ofs_y, u_char alpha);
int CheckTape(int item_no);
void PlayTape(int adpcm_no);
void StopTape(int item_no);

#endif // INGAME_MENU_ITEM_GET_H
