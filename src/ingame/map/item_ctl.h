#ifndef INGAME_MAP_ITEM_CTL_H
#define INGAME_MAP_ITEM_CTL_H

#include "typedefs.h"

typedef struct {
	u_char item_no;
	u_char stts;
	u_char room;
	u_char msn_no;
	short int pos_x;
	short int pos_y;
	short int pos_z;
	u_short get_msg[2];
} MAP_ITEM_DAT;

typedef struct {
	u_char stts;
	u_char room;
	short int pos_x;
	short int pos_y;
	short int pos_z;
	u_char get_msg[2];
	u_char dsp_pct;
	u_char msn_no;
	u_char map_dsp;
} MAP_FILE_DAT;

extern MAP_ITEM_DAT map_item_dat[162];
// extern u_char map_item_ap_type[][2];
// extern MAP_FILE_DAT map_file_dat0[50];
// extern MAP_FILE_DAT map_file_dat1[50];
// extern MAP_FILE_DAT map_file_dat2[50];
// extern MAP_FILE_DAT map_file_dat3[50];
extern MAP_FILE_DAT *map_file_dat[4];
extern u_short item_ap[300][2];

void MapItemInit();
void MissionStartMapItemInit(int msn_no);
void FileDateInit();
#ifndef INCLUDING_FROM_ITEM_GET
// fix for static redefinition of ItemGetMain in item_get.c
int ItemGetMain();
#endif
void ItemCtrl();
void ItemDispCtrl();
void SetItemDispData(u_char id);
void ItemDispDataRenew();
int GetMapItemJudge();
void GetMapItem(u_short get_no);
void ItemAppearCtrl();
int ItemGetContinueJudge();

#endif // INGAME_MAP_ITEM_CTL_H
