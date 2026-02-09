#ifndef INGAME_INFO_INF_DISP_H
#define INGAME_INFO_INF_DISP_H

#include "typedefs.h"

#include "graphics/graph2d/sprt.h"

typedef struct {
    float frot_x;
    u_short plyr_hp;
    u_short plyr_mp;
    short int plyr_pos_x;
    short int plyr_pos_z;
    u_char film_num;
    u_char film_bak;
    u_char focus;
} INFO_WRK;

typedef struct {
    u_short fndr_mode_tmr;
    short int fp_bak[2];
    u_char phot_shot;
    u_char fndr_dsp_flg;
    u_char fndr_fade_alp;
    u_char fndr_fade_tmr;
    u_char rmp_f;
    u_char rmp_pow;
    u_char flm_cng_tmr;
    u_char stts_fade_tmr;
    u_char stts_fade_alp;
    u_char stts_dsp_flg;
    u_char scor_dsp;
    u_char disp_pause;
    u_char weak5_tmr[4];
    u_char weak5_alp[4];
    u_char hint5_tmr[5];
    u_char hint5_alp[5];
    u_char save_col;
    int lucky7;
} INF_DSP;

typedef struct {
    float hx[5];
    float hy[5];
    float ex[4];
    float ey[4];
    u_int pht_cnt_bak;
    short int bon_mov_sx[12];
    short int bon_mov_sy[12];
    short int bon_mov_ex[12];
    short int bon_mov_ey[12];
    short int dmg_num_mov;
    short int dmg_str_mov;
    u_short ene_no[4];
    u_short ene_dmg[4];
    u_short pht_scr;
    u_short dsp_dmg_red;
    u_short dsp_dmg_blu;
    u_short liner_x;
    u_char liner_w;
    u_char ep_alp[4];
    u_char ep_red[4];
    u_char ep_bar;
    u_char ep_tmr;
    u_char bar_over;
    u_short liner_y;
    u_short red_bar;
    u_short blu_bak;
    u_char liner_wait;
    u_char bon_mov_cnt[12];
    u_char bon_mov_tm[12];
    u_char bon_bak;
    u_char mfb_stts;
    char mfb_wait;
    char mfb_drain;
    char mfb_effect;
    u_char mfb_lgt_alp;
    u_char mfb_lgt_bak;
    u_char mfb_lfl_alp;
    u_char mfb_num_alp;
    u_char mfb_num_siz_x;
    u_char mfb_num_siz_y;
    u_char mfb_nfl_alp;
    u_char shw_hp_tmr;
    u_char chrg_rot[12];
    u_char chrg_flr[12];
    u_char chrg_stp[12];
    u_char dray_tmr[12];
    u_char se_wait;
    u_char se_num;
    u_char level_bak;
    u_char buki_use_flg;
    u_char buki_use_alp;
    u_char buki_use_flr;
    u_char buki_use_tmr;
    u_char shot_kind[4];
    u_char shot_kind2;
    u_char shot_mov;
    u_char shot_pse;
    u_char shot_lgt;
    u_char shot_fad;
    u_char shot_num;
    u_char mfb2_stts;
    u_char mfb2_tim;
    u_char mfb2_tim2;
    u_char fire_mask;
    u_char tm_max;
    u_char issei_timer;
    u_char dsp_dmg_flg;
    u_char dsp_dmg_num;
    u_char dsp_dmg_step;
    u_char dsp_dmg_step2;
    u_char dsp_dmg_alp;
    u_char dmg_num_alp;
    u_char dmg_str_alp;
    u_char dmg_eff_alp;
    u_char dmg_wait_tm;
    u_char dmg_num_mov_tim;
    u_char dmg_str_mov_tim;
    u_char dmg_eff_tim;
    u_char ene_dead_tmr;
    u_char ene_dead_alp;
    u_char rmp_alp;
    u_char hit_hp_ovr_ene;
    u_char hit_hp_ovr_ene_wait;
} NEW_INF;

typedef struct {
    u_char err;
    u_char alp_c;
    u_char alp_f;
    u_char dead;
} LIFE_BALL;

typedef struct {
    short int aim;
    short int dst;
} PRTCL_BALL;

typedef struct {
    u_char num1[6];
    u_char num2[6];
    u_char num3[6];
    u_char num4[6];
} ZAN;

typedef struct {
    u_char tim;
    u_char tim_max;
    u_char spd;
    u_char mov_x;
    u_char mov_y;
} JET_SET;

extern SPRT_DAT spr_dat[];
extern short unsigned int flip_table[];
// extern float NC_ADJ;
// extern SPRT_DAT raze_sprt;
// extern INFO_WRK info_wrk;
extern char point_get_end;

void InformationDispInit();
void InformationDispMain();
void InformationDispModeCtrl();
void InformationDispWrkSet();
void PlayerInformationDisp(short int pos_x, short int pos_y);
void FinderDisp(short int pos_x, short int pos_y);
void PhotoScoreDispOnOff(int on_off);
void YVH_CUT(short int tmp_y, u_char tmp_h, short int up_lmt, short int dwn_lmt, short int *dsp_y, signed char *dsp_v, u_char *dsp_h);
void InfoDispPause();
void InfoDispRestart();
int isDispLamp();

#endif // INGAME_INFO_INF_DISP_H
