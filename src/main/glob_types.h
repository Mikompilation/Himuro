#ifndef MAIN_GLOB_TYPES_H
#define MAIN_GLOB_TYPES_H

#include "typedefs.h"

#include "sce/libcdvd.h" // sceCdCLOCK

#include "os/pad_types.h"
#include "ingame/map/map_types.h"
#include "ingame/menu/ig_types.h"
#include "ingame/enemy/ene_types.h"
#include "ingame/enemy/move_types.h"
#include "ingame/photo/photo_types.h"
#include "graphics/graph3d/light_types.h"

typedef struct {
	sceVu0FVECTOR p0;
	sceVu0FVECTOR p1;
	sceVu0FVECTOR p2;
	sceVu0FVECTOR p3;
	sceVu0FVECTOR p4;
	sceVu0FVECTOR p5;
	sceVu0FVECTOR p6;
	sceVu0FVECTOR p7;
	sceVu0FVECTOR p8;
	sceVu0FVECTOR p9;
} MPOS;

typedef struct {
	u_short door_id;
	u_char flag;
	u_char room_no;
	sceVu0FVECTOR dov;
} DOPEN_POS;

typedef struct {
	u_char se_foot;
	u_char room_no;
	u_char room_old;
	u_char pad;
	u_short camera_no;
	u_short camera_no_old;
	u_short camera_btl;
	u_short camera_btl_old;
	u_short camera_drm;
	u_short camera_drm_old;
	u_short camera_door;
	u_short camera_door_old;
	u_short camera_door_did;
	u_char camera_door_rid;
	u_char cam_type;
	float hight;
} PROOM_INFO;

typedef struct {
	sceCdCLOCK start;
	sceCdCLOCK real;
	u_long real_ofs;
	u_long game;
	u_long one_game;
	u_long play;
	u_long area;
	u_long room;
	u_long finder;
	u_long swalk;
	u_long nwalk;
	u_long fwalk;
	u_long run;
	u_long stop;
	u_long zerohour;
	u_long no_pad;
	u_int padding;
} TIME_WRK;

typedef struct {
	int load_id;
	u_char mode;
	u_char mode_next;
	u_int padding;
} OUTGAME_WRK;

typedef struct {
	u_char game;
	u_char mode;
	u_char init_flg;
	u_char stts;
	u_char msn_no;
	u_char difficult;
	u_char clear_count;
	u_char rg_pht_cnt;
	u_short ghost_cnt;
	u_int pht_cnt;
	u_int high_score;
	u_int total_point;
	u_int padding;
} INGAME_WRK;

typedef struct {
	u_char pad_mode;
	u_char pad_move;
	u_char key_type;
	u_char sound_mode;
	u_short bgm_vol;
	u_short bgm_vol_bak;
	u_short se_vol;
	u_int padding;
} OPTION_WRK;

typedef struct {
	u_long count;
	sceCdCLOCK rtc;
	u_int sreset_count;
	u_char sreset_ng;
	u_char reset;
	u_char pause;
	u_char game_mode;
	u_char mode;
	u_char init;
	u_char load;
	u_char load_mode;
#ifdef BUILD_EU_VERSION
	u_char language;
	u_char pal_disp_mode;
	float move_conv;
#endif
	u_int padding;
} SYS_WRK;

typedef struct {
	u_char room_no;
	u_char room_num;
	u_char camera_no;
	u_char camera_bno;
	u_char camera_dno;
	u_char camera_tno;
	u_char camera_num;
	u_char camera_bnum;
	u_char camera_dnum;
	u_char camera_tnum;
	u_char height_no;
	u_char height_num;
	u_char hit_num;
	u_char hit_num_next;
	u_char ev_num;
	u_char find_num;
	u_char se_num;
	u_char mot_num;
	u_char disp_no[2];
	u_char disp_bak[2];
	sceVu0FVECTOR pos[2];
	LIGHT_PACK mylight[2];
	short int room_height;
} ROOM_WRK;

typedef struct {
	u_int sta;
	u_int mvsta;
	u_char mode;
	u_char cam_type;
	u_char anime_no;
	u_char cond;
	u_short cond_tm;
	u_short flash_tm;
	u_short aphoto_tm;
	u_short dmg;
	u_short hp;
	u_short ap_timer;
	u_short dwalk_tm;
	u_short fh_no;
	float fh_deg;
	float spd;
	float spd_ud;
	float prot;
	float frot_x;
	float charge_deg;
	float charge_rate;
	u_char charge_num;
	u_char rock_trgt;
	u_char film_no;
	u_char dmg_cam_flag;
	u_char hp_num;
	u_char mode_save;
	u_char move_mot;
	u_char atk_no;
	u_char near_ene_no;
	u_char photo_charge;
	u_char dmg_type;
	u_char spe1_dir;
	char pr_set;
	char po_set;
	u_char reserve[2];
	sceVu0FVECTOR mv;
	sceVu0FVECTOR op;
	sceVu0FVECTOR rock_adj;
	sceVu0FVECTOR bwp;
	sceVu0FVECTOR cp_old;
	sceVu0FVECTOR soulp;
	sceVu0FVECTOR spot_rot;
	sceVu0FVECTOR spot_pos;
	sceVu0FVECTOR shadow_direction;
	sceVu0FVECTOR psave;
	sceVu0FVECTOR fhp[5];
	short int fp[2];
	MOVE_BOX move_box;
	LIGHT_PACK mylight;
	PROOM_INFO pr_info;
	DOPEN_POS dop;
	int se_deadly;
	u_int padding;
} PLYR_WRK;

typedef struct {
	u_int sta;
	u_int sta2;
	u_short hp;
	u_short dmg;
	u_short dmg_old;
	u_short atk_tm;
	u_char type;
	u_char dat_no;
	u_char act_no;
	u_char anime_no;
	MOVE_BOX move_box;
	ENE_DAT *dat;
	MPOS mpos;
	LIGHT_PACK mylight;
	float dist_p_e;
	float dist_c_e;
	float pra_per;
	float pr_anime;
	u_short pra_time;
	u_short bloop;
	u_char bjob_no;
	u_char bpos_no;
	u_char bwait_time;
	u_char recog_tm;
	P_INT bcomm_add;
	long int bcomm_add_top;
	u_char tr_rate;
	u_char ajob_no;
	u_char apos_no;
	u_char await_time;
	P_INT acomm_add;
	long int acomm_add_top;
	void *pdf;
	float d_pd;
	void *pdf2;
	float d_pd2;
	void *nee;
	float nee_rate;
	float nee_size;
	u_int nee_col;
	int se_omen[2];
	sceVu0FVECTOR adjp;
	sceVu0FVECTOR pp;
	sceVu0FVECTOR sp;
	sceVu0FVECTOR si;
	sceVu0FVECTOR bep;
	u_short in_finder_tm;
	u_char area[6];
	u_short tr_time;
	u_char tr_max;
	u_char dmg_type;
	u_char room_no;
	char se_area_no;
	u_char ani_reso;
	u_char ani_reso_tm;
	short int fp[2];
	short int fp2[3][2];
	u_char eroot[2];
	u_char plight_svo;
	u_char slight_svo;
	u_char plight_svm[2];
	u_char slight_svm[2];
	float pa_radius;
	AENE_INFO_DAT *aie;
	u_short stm_slow;
	u_short stm_view;
	u_short stm_stop;
	u_char tr_rate2;
} ENE_WRK;

typedef struct {
	u_char charge_range;
	u_char charge_max;
	u_char charge_speed;
	u_char func_sub[5];
	u_char func_spe[5];
	u_char set_sub;
	u_char set_spe;
	u_long point;
} CAM_CUSTOM_WRK;

typedef struct {
	u_char pic_num;
	u_char sort_key;
	u_char skey_bak;
	PICTURE_WRK pic[34];
	u_int padding;
} PFILE_WRK;

typedef struct {
	int mode_on;
	int lgt_spot;
	int lgt_point;
	int lgt_switch;
	int cam_mode;
	int eff_z_dep;
	int eff_dither;
	int eff_dithsp;
	int eff_dithal;
	int eff_pdef1;
	int eff_pdef2;
	int eff_pdef3;
	int eff_test2;
	int eff_test3;
	int eff_blur;
	int eff_blra;
	int eff_blrs;
	int eff_blrr;
	int eff_dfm;
	int eff_dfmr;
	int eff_focus;
	int eff_ffr;
	int eff_ffra;
	int eff_frhl;
	int eff_renz;
	int eff_renzs;
	int eff_rip;
	int eff_rips;
	int oth_info;
	int oth_perf;
	int oth_bg_color;
	int light_infinite;
	int light_point;
	int light_spot;
	int le_R;
	int le_G;
	int le_B;
	int le_AR;
	int le_AG;
	int le_AB;
	int le_power;
	int le_cone;
	int sbtset;
	int eff_pdef_p;
	int eff_sccol_tp1;
	int eff_sccol_tp2;
	int eff_sccol_cont_col;
	int eff_sccol_cont_alp;
	int eff_sccol_cont_alp2;
	int eff_sccol_over;
	int eff_amufire;
	int eff_itemfire;
	int eff_colmono;
	int eff_prtblr_sw;
	int eff_prtblr_alp;
	int fog_min;
	int fog_max;
	int fog_near;
	int fog_far;
	int fog_r;
	int fog_g;
	int fog_b;
	int eff_sccol_blk;
	int eff_leaf;
	int eff_new_def_sw;
	int eff_new_def_alp1;
	int eff_new_def_alp2;
	int eff_new_def_col1;
	int eff_new_def_col2;
	int disp_3d_all;
	int disp_3d_room;
	int disp_3d_furn;
	int disp_3d_girl;
	int disp_3d_enemy;
	int disp_3d_mirror;
	int disp_3d_textrans;
	int disp_3d_2ddraw;
	int cam_hold_mode;
	int oth_hint_sw;
	int oth_hint_no;
	int oth_hint_alp;
	int oth_pkt_num_sw;
	int eff_new_ene_ef_sw;
	int eff_new_ene_ef_alp;
	int eff_new_ene_ef_sz;
	int high_speed_mode;
	int param_film74;
	int param_film14;
	int param_film37;
	int param_film90;
	int param_manyo;
	int param_enestop;
	int disp_3d_girl_shadow;
	int disp_3d_room_shadow;
	int param_pad_type;
	int le_plen;
	int cc_charge_range;
	int cc_charge_max;
	int cc_charge_speed;
	int cc_set_sub;
	int cc_set_spe;
	int param_reiseki;
	int param_fuyurei;
	int param_enetype;
	int param_muteki;
	int oth_sub_menu;
	int black_white;
	int param_camera;
} DEBUG_NUMBERS;

#endif // MAIN_GLOB_TYPES_H
