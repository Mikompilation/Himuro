#ifndef INGAME_MENU_IG_GLST_H
#define INGAME_MENU_IG_GLST_H

#include "typedefs.h"

#include "ingame/menu/ig_types.h"

typedef struct {
	char disp_flg;
	char new_flg;
} GLIST_INDEX;

typedef struct {
	u_short lst_top;
	u_short mode;
	u_short dtl_alp;
	u_short g_num;
	u_short all_num;
	u_short complete;
	u_short cnt;
	u_short flg;
} MENU_GLST;

typedef struct {
	u_long tex;
	int load_id;
	char mode;
} GLIST_PHOTO;

typedef struct {
	u_short mdl_no;
	u_short knd;
	u_short no;
	u_short msn;
} GLIST_ORDER;

extern GLIST_INDEX glist_index[108];
// extern GLIST_PHOTO glist_photo;

void NewgameMenuGlstInit();
void StartGlstModeInit();
void IngameMenuGlst();
void IngameMenuGlstDisp();
void DspGlst(u_char alpha);
void DspLstOfs(u_short start, u_char alpha);
void PutRayName(u_char ray_knd, u_short ray_no, u_char ray_msn, short int pos_x, short int pos_y, int rgb, u_char alpha);
void DspGInf(u_char alpha);
void DspGdtl(u_char alpha);
void Sheet(u_short chr_lbl, short int pos_x, short int pos_y, u_short siz_x, u_short siz_y, u_char pri, u_char alp);
void Fuchidori(u_short chr_lbl, short int pos_x, short int pos_y, u_short siz_x, u_short siz_y, u_char pri, u_char alp);
void Naraberu(u_short chr_lbl, short int pos_x, short int pos_y, u_char dir, u_short dst, u_char pri, u_char alp);
void ReSetGhostList(PICTURE_WRK new_pic);
u_short GetGlistAllNum();
u_short GetGlistGNum();
void DispGlistPhoto(u_int line, u_char alpha);
char CheckGlistComplete(u_int list);
char CheckGlistRareGhost(u_int kind, u_int no);

#endif // INGAME_MENU_IG_GLST_H
