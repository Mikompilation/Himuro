#ifndef INGAME_MENU_IG_FILE_H
#define INGAME_MENU_IG_FILE_H

#include "typedefs.h"
#include "ingame/event/ev_spcl.h"

typedef struct RELATION_DAT RELATION_DAT;
typedef struct RELATION_PRT RELATION_PRT;

struct RELATION_PRT
{ // 0x54
    /* 0x00 */ RELATION_PRT *left_prt;
    /* 0x04 */ RELATION_PRT *right_prt;
    /* 0x08 */ SPRT_DAT *ssd_p;
    /* 0x0c */ short int st_index[4];
    /* 0x14 */ short int start_no;
    /* 0x16 */ short int end_no;
    /* 0x18 */ short int attribute;
    /* 0x1c */ int csr_y;
    /* 0x20 */ short int wk_scl[8][2];
    /* 0x40 */ short int wk_pos[2][4];
    /* 0x50 */ short int mission_no;
};

typedef struct
{ // 0x12
    /* 0x00 */ short int x;
    /* 0x02 */ short int y;
    /* 0x04 */ short int u;
    /* 0x06 */ short int v;
    /* 0x08 */ short int exp_u;
    /* 0x0a */ short int exp_v;
    /* 0x0c */ short int n_rabel;
    /* 0x0e */ short int tbl_no;
    /* 0x10 */ short int pad;
} RELATION_NAME_PLATE;

typedef struct
{ // 0xc
    /* 0x0 */ short int com_idx;
    /* 0x2 */ short int attribute;
    /* 0x4 */ short int x1;
    /* 0x6 */ short int y1;
    /* 0x8 */ short int len;
    /* 0xa */ short int pad;
} RELATION_YAJI;

typedef struct
{ // 0x24
    /* 0x00 */ short int disp_pos;
    /* 0x02 */ short int attribute;
    /* 0x04 */ SPRT_DAT *ssd_p[4];
    /* 0x14 */ short int flp_num[4];
    /* 0x1c */ short int dat_idx[2];
    /* 0x20 */ short int pad;
} RELATION_COM;

typedef struct
{ // 0x28
    /* 0x00 */ short int offx;
    /* 0x02 */ short int offy;
    /* 0x04 */ short int a;
    /* 0x06 */ short int attribute;
    /* 0x08 */ short int mode;
    /* 0x0a */ short int load_id;
    /* 0x10 */ u_long tex_keep;
    /* 0x18 */ short int init_switch;
    /* 0x1c */ RELATION_DAT *now_dat;
    /* 0x20 */ RELATION_PRT *now_prt;
    /* 0x24 */ short int pad;
} RELATION_CSR;

struct RELATION_DAT
{ // 0x30
    /* 0x00 */ RELATION_DAT *up_p;
    /* 0x04 */ RELATION_DAT *down_p;
    /* 0x08 */ RELATION_DAT *left_p;
    /* 0x0c */ RELATION_DAT *right_p;
    /* 0x10 */ RELATION_PRT *parent_p;
    /* 0x14 */ RELATION_NAME_PLATE *plate_p;
    /* 0x18 */ SPRT_DAT *ssd_p;
    /* 0x1c */ SPRT_DAT *fnt_p;
    /* 0x20 */ SPRT_DAT *fla_p;
    /* 0x24 */ short int dat_idx;
    /* 0x26 */ short int attribute;
    /* 0x28 */ short int msg_idx;
    /* 0x2a */ short int tm2_id;
    /* 0x2c */ short int dsp_pos;
    /* 0x2e */ short int pad;
};

typedef struct
{ // 0x6
    /* 0x0 */ short int x;
    /* 0x2 */ short int y;
    /* 0x4 */ u_char mark;
    /* 0x5 */ u_char str;
} CAPTION_DATA_SUB;

typedef struct
{ // 0x1c
    /* 0x00 */ short int w;
    /* 0x02 */ short int h;
    /* 0x04 */ CAPTION_DATA_SUB cds[4];
} CAPTION_DATA;

typedef struct
{ // 0x8
    /* 0x0 */ short int x;
    /* 0x2 */ short int y;
    /* 0x4 */ short int no;
    /* 0x6 */ short int pnl;
} CAPTION_LIST;

enum NAME_PLATE_RABEL {
	SAKKA = 0,
	SAKKA2 = 1,
	JYOSYU = 2,
	JYOSYU2 = 3,
	HENSYU = 4,
	HENSYU2 = 5,
	TOUSYU = 6,
	TOUSYU2 = 7,
	KIRIE = 8,
	KIRIE2 = 9,
	KIRIE3 = 10,
	KIRIE4 = 11,
	SINKAN = 12,
	SINKAN2 = 13,
	KIMONO = 14,
	KIMONO2 = 15,
	KIMONO3 = 16,
	RYOZO = 17,
	RYOZO2 = 18,
	YAE = 19,
	YAE2 = 20,
	MIKOTO = 21,
	MIKOTO2 = 22,
	MIKOTO3 = 23,
	MAFUYU = 24,
	MAFUYU2 = 25,
	MAFUYU3 = 26,
	MIKU = 27,
	MIYUKI = 28,
	MIYUKI2 = 29,
	OMOI = 30,
	OMOI2 = 31
};

enum PARENT_RABEL {
	SAKKA_PRT = 0,
	HIMURO_PRT = 1,
	MUNA_PRT = 2,
	HINA_PRT = 3,
	OMOI_PRT = 4
};

enum REL_DIRECTION {
	R_UP = 0,
	R_DOWN = 1,
	R_LEFT = 2,
	R_RIGHT = 3
};

enum REL_ALPB {
	R_A = 0,
	R_B = 1,
	R_C = 2,
	R_D = 3,
	R_E = 4,
	R_F = 5,
	R_G = 6,
	R_G2 = 7,
	R_H = 8,
	R_I = 9,
	R_J = 10,
	R_J2 = 11,
	R_K = 12,
	R_K2 = 13,
	R_L = 14,
	R_L2 = 15,
	R_N = 16,
	R_O = 17,
	R_P = 18,
	R_Q = 19,
	R_R = 20,
	R_S = 21,
	R_T = 22,
	R_U = 23,
	R_V = 24,
	R_W = 25,
	R_X = 26,
	R_Y = 27,
	R_Z = 28
};

extern u_char file_kind[0];
extern SPRT_DAT spr_dat_tmp[0];
extern RELATION_COM rel_com[0];
extern RELATION_PRT rel_prt[0];
extern RELATION_DAT rel_dat[0];
extern RELATION_YAJI rel_yaj[0];
extern CAPTION_LIST caption_list[0];
extern CAPTION_DATA caption_data[0];
extern RELATION_CSR rel_csr;

void NewgameMenuFileInit();
void LoadgameMenuFileInit();
void StartFileModeInit();
void IngameMenuFile();
void IngameMenuFileDisp();
void IngameMenuFileData();
void IngameMenuFileDataDisp();
void GetDispMenuFile(u_char csr);
void SideBar(u_char hav_now, u_char dsp_max, u_short mov_lng, u_char lst_top, u_short chr_top, u_char alp, short int dx, short int dy);
void PageInfo(u_char now, u_char all, short int pos_x, short int pos_y, u_char alpha);
void DokiDokiLetter(u_char level);
void RelationShip();
void RelationMapInit();
void RelAtributeChg();
void RelComAtributeChg();
void MoveBeyondData();
void MoveBeyondGroup();
void RelMapScroolX(RELATION_DAT *r_dat);
void RelMapScroolY(RELATION_PRT *r_prt);
int ButtonManager();
void RelationDispDat();
void RelDspYajirusi(RELATION_YAJI *r_yaj);
void RelDspComment();
void RelDspPrtFla(RELATION_PRT *r_prt);
void RelDspPrt(RELATION_PRT *r_prt);
void RelDspBackGrd();
void RelationDispMsg();
void SimpleDispSprtDS(SPRT_DAT *ssd, u_int addr, int sp_no, SPRT_SROT *srot, SPRT_SSCL *sscl, u_char alp_rate);
void TestPk2DataSou(long int sendtexaddr);
void DispCaption(int no, u_char alp);

#endif // INGAME_MENU_IG_FILE_H
