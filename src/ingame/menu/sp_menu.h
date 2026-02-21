#ifndef INGAME_MENU_SP_MENU_H
#define INGAME_MENU_SP_MENU_H

#include "typedefs.h"

typedef struct { // 0x7
	/* 0x0 */ u_char mode;
	/* 0x1 */ u_char sub_mode;
	/* 0x2 */ u_char csr[5];
} SAVE_POINT_MENU_WRK;

typedef struct { // 0x6
	/* 0x0 */ u_short dmy_tmr;
	/* 0x2 */ u_char step;
	/* 0x3 */ u_char sv_ctrl;
	/* 0x4 */ u_char slot;
	/* 0x5 */ u_char file;
} SAVE_CONTE;

typedef struct { // 0xc
	/* 0x0 */ short int siz_w;
	/* 0x2 */ short int siz_h;
	/* 0x4 */ short int pos_x;
	/* 0x6 */ short int pos_y;
	/* 0x8 */ u_char ttl;
	/* 0x9 */ u_char alp;
	/* 0xa */ u_char rgb;
} BOOK;

typedef struct { // 0xc
	/* 0x0 */ int load_id;
	/* 0x4 */ u_char step;
	/* 0x5 */ u_char type[2];
	/* 0x7 */ u_char load_side;
	/* 0x8 */ u_char in_album;
} ALBM_MODE;


typedef struct { // 0xb
	/* 0x0 */ signed char slct_mode;
	/* 0x1 */ u_char str_no;
	/* 0x2 */ u_char top_alp;
	/* 0x3 */ u_char bak_alp;
	/* 0x4 */ u_char msg_alp;
	/* 0x5 */ u_char ans_alp;
	/* 0x6 */ u_char btn_tmr;
	/* 0x7 */ u_char atype_buf[2];
	/* 0x9 */ u_char load_side;
	/* 0xa */ u_char in_album;
} DSP_SVP;

typedef struct { // 0xc
	/* 0x0 */ float slot1_alp;
	/* 0x4 */ float slot2_alp;
	/* 0x8 */ float slot3_alp;
} MIRACLE_LENZ;

typedef struct { // 0x14
	/* 0x00 */ u_short msk_alp;
	/* 0x02 */ u_short msg_alp;
	/* 0x04 */ u_short fdt_alp;
	/* 0x06 */ u_short svp_alp;
	/* 0x08 */ u_short alt_alp;
	/* 0x0a */ u_short yno_alp;
	/* 0x0c */ u_char bck;
	/* 0x0d */ u_char msk;
	/* 0x0e */ u_char msg;
	/* 0x0f */ u_char fdt;
	/* 0x10 */ u_char svp;
	/* 0x11 */ u_char alt;
	/* 0x12 */ u_char alt_bak_alp;
} SV_DSP_SW;

extern SAVE_POINT_MENU_WRK spmenu_wrk;

void SavePointMenuInit();
void SavePointMenuOpen();
void SavePointMenuOpenInit(u_char msn);
void BtlModSaveInit();
char SavePointMenuMain(u_char msn);
void SavePointMenuModeSlct(u_char msn, u_char mode);
void SavePointMenuModeSlctDisp(u_char msn, u_char csr3, u_char csr4, u_char alp_max, u_char mode);
void SavePointMenuAlbum();
void SavePointMenuSave();
void DspMemSavePoint(u_char msk, u_char msg, u_char fdt, u_char svp, u_char alt, u_char csr0, u_char csr1, u_char csr2, u_char yes_no, u_char alp_max, u_char type);
char SaveCamChk();
char AlbmSlct(u_char csr3, u_char step, u_char alp_max, u_char ini);
void AlbumModeInGameOverInit();
char AlbumModeInGameOver();
int SavePointLightJudge();

#endif // INGAME_MENU_SP_MENU_H
