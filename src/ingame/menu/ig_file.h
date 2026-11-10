#ifndef INGAME_MENU_IG_FILE_H
#define INGAME_MENU_IG_FILE_H

#include "typedefs.h"
#include "ingame/event/ev_spcl.h"

typedef struct RELATION_DAT RELATION_DAT;
typedef struct RELATION_PRT RELATION_PRT;

struct RELATION_PRT {
    RELATION_PRT *left_prt;
    RELATION_PRT *right_prt;
    SPRT_DAT *ssd_p;
    short int st_index[4];
    short int start_no;
    short int end_no;
    short int attribute;
    int csr_y;
    short int wk_scl[8][2];
    short int wk_pos[2][4];
    short int mission_no;
};

typedef struct {
    short int x;
    short int y;
    short int u;
    short int v;
    short int exp_u;
    short int exp_v;
    short int n_rabel;
    short int tbl_no;
    short int pad;
} RELATION_NAME_PLATE;

typedef struct {
    short int com_idx;
    short int attribute;
    short int x1;
    short int y1;
    short int len;
    short int pad;
} RELATION_YAJI;

typedef struct {
    short int disp_pos;
    short int attribute;
    SPRT_DAT *ssd_p[4];
    short int flp_num[4];
    short int dat_idx[2];
    short int pad;
} RELATION_COM;

typedef struct {
    short int offx;
    short int offy;
    short int a;
    short int attribute;
    short int mode;
    short int load_id;
    u_long tex_keep;
    short int init_switch;
    RELATION_DAT *now_dat;
    RELATION_PRT *now_prt;
    short int pad;
} RELATION_CSR;

struct RELATION_DAT {
    RELATION_DAT *up_p;
    RELATION_DAT *down_p;
    RELATION_DAT *left_p;
    RELATION_DAT *right_p;
    RELATION_PRT *parent_p;
    RELATION_NAME_PLATE *plate_p;
    SPRT_DAT *ssd_p;
    SPRT_DAT *fnt_p;
    SPRT_DAT *fla_p;
    short int dat_idx;
    short int attribute;
    short int msg_idx;
    short int tm2_id;
    short int dsp_pos;
    short int pad;
};

typedef struct {
    short int x;
    short int y;
    u_char mark;
    u_char str;
} CAPTION_DATA_SUB;

typedef struct {
    short int w;
    short int h;
    CAPTION_DATA_SUB cds[4];
} CAPTION_DATA;

typedef struct {
    short int x;
    short int y;
    short int no;
    short int pnl;
} CAPTION_LIST;

typedef struct {
    u_long tx;
    int lod_id;
    short int lod_no;
    short int vrm_no;
    short int dsp_ok;
} MEM_BOX;

typedef struct {
    u_long tex_now;
    u_long tex_bak;
    int load_id;
    u_char alp_now;
    u_char alp_bak;
    u_char f_mode;
    u_char mod;
    u_char knd;
    u_char dtl;
    u_char lst_top;
    u_char pic_alp[4];
    u_char page_all;
    u_char open_page;
    u_char poss_chk;
    u_char csr_bak;
    u_char csr_now;
    u_char loading;
    u_char can_dsp;
    u_char dsp_flg;
    u_char pht_alp;
    u_char pad_lock;
} MENU_FILE;

typedef struct {
    u_long tex;
    u_char space;
    u_char rot;
} PHOT_FILE;

enum {
    FILE_REL_START = 0,
    FILE_REL_LOAD_WAIT = 1,
    FILE_REL_TATEI = 2,
    FILE_REL_MENU_LOAD_WAIT = 3,
    FILE_REL_END = 4
};

typedef struct {
    int load_id;
    char step;
} FILE_REL;

typedef struct {
    short int px;
    short int py;
    short int sx;
    short int sy;
} DSP_PHT;

typedef struct {
    short int pht_no;
    short int lod_rq;
    short int mem_no;
} DSP_BOX;

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

extern u_char file_kind[];
extern SPRT_DAT spr_dat_tmp[];
extern RELATION_COM rel_com[];
extern RELATION_PRT rel_prt[];
extern RELATION_DAT rel_dat[];
extern RELATION_YAJI rel_yaj[];
extern CAPTION_LIST caption_list[];
extern CAPTION_DATA caption_data[];
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
void RelationDispDat(RELATION_DAT *r_dat);
void RelDspYajirusi(RELATION_YAJI *r_yaj);
void RelDspComment();
void RelDspPrtFla(RELATION_PRT *r_prt);
void RelDspPrt(RELATION_PRT *r_prt);
void RelDspBackGrd();
void RelationDispMsg(RELATION_DAT *r_dat);
void SimpleDispSprtDS(SPRT_DAT *ssd, u_int addr, int sp_no, SPRT_SROT *srot, SPRT_SSCL *sscl, u_char alp_rate);
void TestPk2DataSou(long int sendtexaddr);
void DispCaption(int no, u_char alp);

#endif // INGAME_MENU_IG_FILE_H
