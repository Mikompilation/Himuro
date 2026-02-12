#ifndef INGAME_MENU_ITEM_GET_H
#define INGAME_MENU_ITEM_GET_H

#include "typedefs.h"

typedef struct {
    u_char msg_type_bak;
    u_char msg_no_bak;
    u_char str;
    u_char msg;
    u_char scn;
    u_char chr;
    u_char ply;
    u_char nam;
    u_char tim;
    u_char bak;
    u_char msg_end;
} FILE_GET_FADE;

typedef struct {
    u_long tex_addr;
    float scl;
    u_char alp;
} FILE2D;

typedef struct {
    int item_no;
    int adpcm_no;
} TAPE_DAT;

typedef struct {
    u_char type;
    u_char no;
    u_char msg0;
    u_char msg1;
    u_char flg;
    u_char itm_num;
} FILE_GET_CTRL;

typedef struct {
    u_short cnt;
    u_char stp;
    u_char dsp;
    u_char csr[1];
    u_char stts;
    u_char page_all;
    u_char open_page;
} FILE_GET_DSP;

// extern TAPE_DAT tape_dat[];
extern int play_tape_timer;
extern char tape_play;

void ItemGet(u_char get_type, u_char get_no, u_char msg0_no, u_char msg1_no);
int ItemGetCtrl();
void NextPageNavi(u_char hav_now, u_char dsp_max, u_short chr_top, short int ofs_x, short int ofs_y, u_char alpha);
int CheckTape(int item_no);
void PlayTape(int adpcm_no);
void StopTape(int item_no);

#endif // INGAME_MENU_ITEM_GET_H
