#ifndef INGAME_MENU_ITEM_H
#define INGAME_MENU_ITEM_H

#include "typedefs.h"

#include "ingame/menu/ig_camra.h"

typedef struct {
	float io_x[7];
	float io_y[7];
	float io_a[7];
	float pos_x[1][2];
	float pos_y[1][2];
	u_short inn_mode_cnt;
	u_short out_mode_cnt;
	u_short znz_tmr[1];
	u_short menu_cnt;
	u_short menu_io_cnt;
	u_char menu_io_flg;
	u_char map_flg;
	u_char pad_lock;
} FOR_YW2D;

typedef struct {
	u_char knd_char;
	short int char_no;
} YWDBG;

typedef struct {
	u_char mode;
	u_char csr[6];
	u_char csr_bak[6];
	u_char item_num;
	u_char use_item;
	u_char file_num;
} INGAME_MENU_WRK;

typedef struct {
	u_short item_no;
	u_short item_num;
} MENU_ITEM_WRK;

typedef struct {
	u_char type;
	u_char value0;
	short int value1;
} ITEM_USE_DAT;

typedef struct {
	u_char open_page;
	u_char knd_bak;
	u_char no_bak;
} MSG_DSP;

typedef struct {
	u_long tex_now;
	u_long tex_bak;
	int load_id;
	short int gage_hp;
	u_char alp_now;
	u_char itm_lst_strt;
	u_char flm_alp[5];
	u_char hp_max_chk;
	u_char flm_dff_chk;
	u_char itm_eve_chk;
	u_char itm_csr_now;
	u_char itm_loading;
	u_char itm_can_dsp;
	u_char itm_dsp_flg;
	u_char i_mode;
	u_char msg_end;
	u_char btn_flsh_cnt;
	u_char set_sub_tmp;
	u_char set_sub_flr;
	u_char set_sub_bak;
	u_char set_spe_tmp;
	u_char set_spe_flr;
	u_char set_spe_bak;
	u_char cam_flr_tmr;
	u_char tape_flsh;
} ITM_EFF_WRK;

extern ITEM_USE_DAT item_use_dat[70];
extern short int item_sort[70];
extern short int file2item[0];
extern char file2phot[0];
extern char pht_waku_put[0];
extern FOR_YW2D yw2d;
extern INGAME_MENU_WRK ig_menu_wrk;
extern MENU_ITEM_WRK menu_item_wrk[200];

void NewgameItemInit();
void NewgameItemInit2();
void MissionSelectItemInit();
void MenuItemInit();
void LoadgameMenuItemInit();
void StartItemModeInit();
void ItemInitAtMenuOpen();
void FilmPossessionExp();
void IngameMenuItem();
void IngameMenuItemUseSlct();
void IngameMenuItemDisp();
void PlayerStatusYW(u_char alpha);
int LoadItem2D(u_char get_type, u_char get_no);
u_long VramItem2D(u_char mode, u_char type);
char DspItem2D(u_long tex_addr, float pos_x, float pos_y, int rgb, float alp, float scl_x, float scl_y, char pri);
void PkTm2(u_long tex_addr, short int pos_x, short int pos_y, short int w, short int h, u_char alp, int pri);
void PutSpriteYW(u_short top_label, u_short end_label, float pos_x, float pos_y, float rot, int rgb, float alp, float scl_x, float scl_y, u_char scl_mode, int pri, u_char by, u_char blnd, u_char z_sw);
void FlashStarYW(FLSH_CORE *flsh, u_char flsh_max, u_char flsh_min, u_char flsh_flm, u_char mode);
void CLRtoRGB(int *rgb, u_char r, u_char g, u_char b);
void RGBtoCLR(int rgb, u_char *r, u_char *g, u_char *b);
void PutStringYW(u_char msg_knd, u_char msg_no, short int pos_x, short int pos_y, int rgb, u_char alpha, int pri, u_char type);
void RstMessageYW();
u_char PutMessageYW(u_char msg_knd, u_char msg_no, short int pos_x, short int pos_y, int rgb, u_char alpha, int pri);
char ChkChrNumYW(u_char msg_knd, u_char msg_no);
int ChkChrNumUS(u_char msg_knd, u_char msg_no);
char ChkPageYW(u_char msg_knd, u_char msg_no);
void PutPageYW(u_char msg_knd, u_char msg_no, u_char open_page, short int pos_x, short int pos_y, int rgb, u_char alpha, int pri);
void PutPage2YW(u_char msg_knd, u_char msg_no, u_char open_page, short int pos_x, short int pos_y, int rgb, u_char alpha, int pri);
void PutNumberYW(u_char font, int num, short int pos_x, short int pos_y, float sx, float sy, int rgb, short int alpha, int pri, int digit, u_char mode);
void PolySquareYW(float pos_x, float pos_y, u_short bar_l, u_short bar_h, int rgb, float alp, float scl_x, float scl_y, int pri, u_char blnd, u_char sw_z, short int rz);
void YesNoCrslOKR(int pri, float pos_x, float pos_y, int rgb, float alp, float scl);
void XYAdefaultYW(u_char no);
void BgFusumaYW(int rgb, float pos_x, float alpha, int pri);
u_int FromPKZ(u_int pkz_addr, u_char pk2_no);
void DcdCMP2PK2(u_int cmp_addr, u_int tmp_addr);

#endif // INGAME_MENU_ITEM_H
