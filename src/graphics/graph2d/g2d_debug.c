#include "common.h"
#include "typedefs.h"
#include "g2d_debug.h"

#include "sce/libpc.h"

#include "main/glob.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "ingame/photo/pht_make.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph3d/leditor.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph2d/tim2_new.h"
#include "graphics/graph2d/g2d_main.h"

int dither_alp = 0x40;
int dither_col = 0x80;

#include "data/dbg_menu_t.h" /* DEBUG_MENU dbg_menu_t; */
#include "data/dbg_menu_lt.h" /* DEBUG_MENU dbg_menu_lt; */
#include "data/dbg_menu_ct.h" /* DEBUG_MENU dbg_menu_ct; */
#include "data/dbg_menu_c1.h" /* DEBUG_MENU dbg_menu_c1; */
#include "data/dbg_menu_et.h" /* DEBUG_MENU dbg_menu_et; */
#include "data/dbg_menu_e1.h" /* DEBUG_MENU dbg_menu_e1; */
#include "data/dbg_menu_e2.h" /* DEBUG_MENU dbg_menu_e2; */
#include "data/dbg_menu_ot.h" /* DEBUG_MENU dbg_menu_ot; */
#include "data/dbg_menu_e3.h" /* DEBUG_MENU dbg_menu_e3; */
#include "data/dbg_menu_e4.h" /* DEBUG_MENU dbg_menu_e4; */
#include "data/dbg_menu_e5.h" /* DEBUG_MENU dbg_menu_e5; */
#include "data/dbg_menu_e51.h" /* DEBUG_MENU dbg_menu_e51; */
#include "data/dbg_menu_e41.h" /* DEBUG_MENU dbg_menu_e41; */
#include "data/dbg_menu_e6.h" /* DEBUG_MENU dbg_menu_e6; */
#include "data/dbg_menu_e7.h" /* DEBUG_MENU dbg_menu_e7; */
#include "data/dbg_menu_e8.h" /* DEBUG_MENU dbg_menu_e8; */
#include "data/dbg_menu_e81.h" /* DEBUG_MENU dbg_menu_e81; */
#include "data/dbg_menu_e83.h" /* DEBUG_MENU dbg_menu_e83; */
#include "data/dbg_menu_le_top.h" /* DEBUG_MENU dbg_menu_le_top; */
#include "data/dbg_menu_le_save.h" /* DEBUG_MENU dbg_menu_le_save; */
#include "data/dbg_menu_le_infinite.h" /* DEBUG_MENU dbg_menu_le_infinite; */
#include "data/dbg_menu_le_point.h" /* DEBUG_MENU dbg_menu_le_point; */
#include "data/dbg_menu_le_spot.h" /* DEBUG_MENU dbg_menu_le_spot; */
#include "data/dbg_menu_le_sdone.h" /* DEBUG_MENU dbg_menu_le_sdone; */
#include "data/dbg_menu_le_ldone.h" /* DEBUG_MENU dbg_menu_le_ldone; */
#include "data/dbg_menu_le_infinite_value.h" /* DEBUG_MENU dbg_menu_le_infinite_value; */
#include "data/dbg_menu_le_point_value.h" /* DEBUG_MENU dbg_menu_le_point_value; */
#include "data/dbg_menu_le_spot_value.h" /* DEBUG_MENU dbg_menu_le_spot_value; */
#include "data/dbg_menu_e84.h" /* DEBUG_MENU dbg_menu_e84; */
#include "data/dbg_menu_e841.h" /* DEBUG_MENU dbg_menu_e841; */
#include "data/dbg_menu_e21.h" /* DEBUG_MENU dbg_menu_e21; */
#include "data/dbg_menu_le_infinite_apply.h" /* DEBUG_MENU dbg_menu_le_infinite_apply; */
#include "data/dbg_menu_le_point_apply.h" /* DEBUG_MENU dbg_menu_le_point_apply; */
#include "data/dbg_menu_le_spot_apply.h" /* DEBUG_MENU dbg_menu_le_spot_apply; */
#include "data/dbg_menu_e31.h" /* DEBUG_MENU dbg_menu_e31; */
#include "data/dbg_menu_e32.h" /* DEBUG_MENU dbg_menu_e32; */
#include "data/dbg_menu_e33.h" /* DEBUG_MENU dbg_menu_e33; */
#include "data/dbg_menu_esc.h" /* DEBUG_MENU dbg_menu_esc; */
#include "data/dbg_menu_esc_col.h" /* DEBUG_MENU dbg_menu_esc_col; */
#include "data/dbg_menu_esc_tp1.h" /* DEBUG_MENU dbg_menu_esc_tp1; */
#include "data/dbg_menu_esc_tp2.h" /* DEBUG_MENU dbg_menu_esc_tp2; */
#include "data/dbg_menu_esc_cont.h" /* DEBUG_MENU dbg_menu_esc_cont; */
#include "data/dbg_menu_eprt.h" /* DEBUG_MENU dbg_menu_eprt; */
#include "data/dbg_menu_eprt_blr.h" /* DEBUG_MENU dbg_menu_eprt_blr; */
#include "data/dbg_menu_fog_top.h" /* DEBUG_MENU dbg_menu_fog_top; */
#include "data/dbg_menu_fog_apply.h" /* DEBUG_MENU dbg_menu_fog_apply; */
#include "data/dbg_menu_sbtset.h" /* DEBUG_MENU dbg_menu_sbtset; */
#include "data/dbg_menu_ef_new_def.h" /* DEBUG_MENU dbg_menu_ef_new_def; */
#include "data/dbg_menu_3ddisp_menu.h" /* DEBUG_MENU dbg_menu_3ddisp_menu; */
#include "data/dbg_menu_le_ambient_value.h" /* DEBUG_MENU dbg_menu_le_ambient_value; */
#include "data/dbg_menu_le_ambient_apply.h" /* DEBUG_MENU dbg_menu_le_ambient_apply; */
#include "data/dbg_menu_c1_1.h" /* DEBUG_MENU dbg_menu_c1_1; */
#include "data/dbg_menu_hinttex.h" /* DEBUG_MENU dbg_menu_hinttex; */
#include "data/dbg_menu_ef_pe_ne.h" /* DEBUG_MENU dbg_menu_ef_pe_ne; */
#include "data/dbg_menu_esc_nega.h" /* DEBUG_MENU dbg_menu_esc_nega; */
#include "data/dbg_menu_param.h" /* DEBUG_MENU dbg_menu_param; */
#include "data/dbg_menu_ccustom.h" /* DEBUG_MENU dbg_menu_ccustom; */
#include "data/dbg_menu_param_item.h" /* DEBUG_MENU dbg_menu_param_item; */

static DEBUG_MENU *dbgmenu_tbl[] = {
    &dbg_menu_t,
    &dbg_menu_lt,
    &dbg_menu_ct,
    &dbg_menu_c1,
    &dbg_menu_et,
    &dbg_menu_e1,
    &dbg_menu_e2,
    &dbg_menu_ot,
    &dbg_menu_e3,
    &dbg_menu_e4,
    &dbg_menu_e5,
    &dbg_menu_e51,
    &dbg_menu_e41,
    &dbg_menu_e6,
    &dbg_menu_e7,
    &dbg_menu_e8,
    &dbg_menu_e81,
    &dbg_menu_e83,
    &dbg_menu_le_top,
    &dbg_menu_le_save,
    &dbg_menu_le_infinite,
    &dbg_menu_le_point,
    &dbg_menu_le_spot,
    &dbg_menu_le_sdone,
    &dbg_menu_le_ldone,
    &dbg_menu_le_infinite_value,
    &dbg_menu_le_point_value,
    &dbg_menu_le_spot_value,
    &dbg_menu_e84,
    &dbg_menu_e841,
    &dbg_menu_e21,
    &dbg_menu_le_infinite_apply,
    &dbg_menu_le_point_apply,
    &dbg_menu_le_spot_apply,
    &dbg_menu_e31,
    &dbg_menu_e32,
    &dbg_menu_e33,
    &dbg_menu_esc,
    &dbg_menu_esc_col,
    &dbg_menu_esc_tp1,
    &dbg_menu_esc_tp2,
    &dbg_menu_esc_cont,
    &dbg_menu_eprt,
    &dbg_menu_eprt_blr,
    &dbg_menu_fog_top,
    &dbg_menu_fog_apply,
    &dbg_menu_sbtset,
    &dbg_menu_ef_new_def,
    &dbg_menu_3ddisp_menu,
    &dbg_menu_le_ambient_value,
    &dbg_menu_le_ambient_apply,
    &dbg_menu_c1_1,
    &dbg_menu_hinttex,
    &dbg_menu_ef_pe_ne,
    &dbg_menu_esc_nega,
    &dbg_menu_param,
    &dbg_menu_ccustom,
    &dbg_menu_param_item,
};

static int *dbgmenu_inttbl[] = {
    &dbg_wrk.mode_on,
    &dbg_wrk.lgt_spot,
    &dbg_wrk.cam_mode,
    &dbg_wrk.eff_z_dep,
    &dbg_wrk.eff_dither,
    &dbg_wrk.oth_info,
    &dbg_wrk.lgt_point,
    &dbg_wrk.eff_pdef_p,
    &dbg_wrk.lgt_switch,
    &dbg_wrk.oth_perf,
    &dbg_wrk.eff_blur,
    &dbg_wrk.eff_dfm,
    &dbg_wrk.eff_dfmr,
    &dbg_wrk.eff_blra,
    &dbg_wrk.eff_blrs,
    &dbg_wrk.eff_test2,
    &dbg_wrk.eff_focus,
    &dbg_wrk.eff_test3,
    &dbg_wrk.eff_blrr,
    &dbg_wrk.eff_dithsp,
    &dbg_wrk.eff_dithal,
    &dbg_wrk.eff_ffr,
    &dbg_wrk.eff_ffra,
    &dbg_wrk.eff_frhl,
    &dbg_wrk.eff_renz,
    &dbg_wrk.eff_renzs,
    &dbg_wrk.light_infinite,
    &dbg_wrk.light_point,
    &dbg_wrk.light_spot,
    &dbg_wrk.le_R,
    &dbg_wrk.le_G,
    &dbg_wrk.le_B,
    &dbg_wrk.le_AR,
    &dbg_wrk.le_AG,
    &dbg_wrk.le_AB,
    &dbg_wrk.le_power,
    &dbg_wrk.le_cone,
    &dbg_wrk.eff_rip,
    &dbg_wrk.eff_rips,
    &dbg_wrk.sbtset,
    &dither_alp,
    &dither_col,
    &dbg_wrk.eff_pdef1,
    &dbg_wrk.eff_pdef2,
    &dbg_wrk.eff_pdef3,
    &dbg_wrk.eff_sccol_tp1,
    &dbg_wrk.eff_sccol_tp2,
    &dbg_wrk.eff_sccol_cont_col,
    &dbg_wrk.eff_sccol_cont_alp,
    &dbg_wrk.eff_sccol_over,
    &dbg_wrk.eff_amufire,
    &dbg_wrk.eff_itemfire,
    &dbg_wrk.eff_colmono,
    &dbg_wrk.eff_prtblr_sw,
    &dbg_wrk.eff_prtblr_alp,
    &dbg_wrk.fog_min,
    &dbg_wrk.fog_max,
    &dbg_wrk.fog_near,
    &dbg_wrk.fog_far,
    &dbg_wrk.fog_r,
    &dbg_wrk.fog_g,
    &dbg_wrk.fog_b,
    &dbg_wrk.eff_sccol_blk,
    &dbg_wrk.eff_leaf,
    &dbg_wrk.eff_new_def_sw,
    &dbg_wrk.eff_new_def_alp1,
    &dbg_wrk.eff_new_def_alp2,
    &dbg_wrk.eff_new_def_col1,
    &dbg_wrk.eff_new_def_col2,
    &dbg_wrk.disp_3d_all,
    &dbg_wrk.disp_3d_room,
    &dbg_wrk.disp_3d_furn,
    &dbg_wrk.disp_3d_girl,
    &dbg_wrk.disp_3d_enemy,
    &dbg_wrk.disp_3d_mirror,
    &dbg_wrk.disp_3d_textrans,
    &dbg_wrk.disp_3d_2ddraw,
    &dbg_wrk.cam_hold_mode,
    &dbg_wrk.oth_hint_sw,
    &dbg_wrk.oth_hint_no,
    &dbg_wrk.oth_hint_alp,
    &dbg_wrk.oth_pkt_num_sw,
    &hint_test_sw,
    &hint_test_posx,
    &dbg_wrk.eff_new_ene_ef_sw,
    &dbg_wrk.eff_new_ene_ef_alp,
    &dbg_wrk.eff_new_ene_ef_sz,
    &dbg_wrk.oth_bg_color,
    &dbg_wrk.eff_sccol_cont_alp2,
    &dbg_wrk.high_speed_mode,
    &dbg_wrk.param_film74,
    &dbg_wrk.param_film14,
    &dbg_wrk.param_film37,
    &dbg_wrk.param_manyo,
    &dbg_wrk.param_enestop,
    &dbg_wrk.disp_3d_girl_shadow,
    &dbg_wrk.disp_3d_room_shadow,
    &dbg_wrk.param_pad_type,
    &dbg_wrk.le_plen,
    &dbg_wrk.cc_charge_range,
    &dbg_wrk.cc_charge_max,
    &dbg_wrk.cc_charge_speed,
    &dbg_wrk.cc_set_sub,
    &dbg_wrk.cc_set_spe,
    &dbg_wrk.param_reiseki,
    &dbg_wrk.param_fuyurei,
    &dbg_wrk.param_enetype,
    &dbg_wrk.param_muteki,
    &dbg_wrk.oth_sub_menu,
    &dbg_wrk.black_white,
    &dbg_wrk.param_camera,
    &dbg_wrk.param_film90,
};

#define MIN(x, y) (x) < (y) ? (x) : (y)
#define MAX(x, y) (x) < (y) ? (y) : (x)
#define MIN0(x, y) (x) < (0) ? (y) : (x)
#define MAX0(x, y) (x) > (y) ? (0) : (x)

void gra2dInitDbgMenu()
{
    dbg_wrk.mode_on = 0;
    dbg_wrk.lgt_spot = 0;
    dbg_wrk.lgt_point = 0;
    dbg_wrk.lgt_switch = 100;
    dbg_wrk.cam_mode = 0;
    dbg_wrk.eff_z_dep = 0;
    dbg_wrk.eff_dither = 0;
    dbg_wrk.eff_dithsp = 8;
    dbg_wrk.eff_dithal = 32;
    dbg_wrk.eff_blur = 0;
    dbg_wrk.eff_blrs = 1000;
    dbg_wrk.eff_blra = 100;
    dbg_wrk.eff_blrr = 1800;
    dbg_wrk.eff_dfm = 0;
    dbg_wrk.eff_dfmr = 0;
    dbg_wrk.eff_focus = 0;
    dbg_wrk.eff_pdef_p = 0;
    dbg_wrk.eff_pdef1 = 0;
    dbg_wrk.eff_pdef2 = 0;
    dbg_wrk.eff_pdef3 = 0;
    dbg_wrk.eff_test2 = 0;
    dbg_wrk.eff_test3 = 0;
    dbg_wrk.eff_ffr = 0;
    dbg_wrk.eff_ffra = 128;
    dbg_wrk.eff_renz = 0;
    dbg_wrk.eff_renzs = 4;
    dbg_wrk.eff_rip = 0;
    dbg_wrk.eff_rips = 1;
    dbg_wrk.oth_info = 0;

    dbg_wrk.oth_perf = 0;

    dbg_wrk.oth_bg_color = 0;
    dbg_wrk.light_infinite = 0;
    dbg_wrk.light_point = 0;
    dbg_wrk.light_spot = 0;

    dbg_wrk.eff_sccol_cont_col = 0x80;
    dbg_wrk.eff_sccol_cont_alp = 0x80;
    dbg_wrk.eff_sccol_cont_alp2 = 0x80;

    dbg_wrk.eff_prtblr_sw = 0;
    dbg_wrk.eff_prtblr_alp = 0x46;

    dbg_wrk.eff_itemfire = 1;
    dbg_wrk.eff_frhl = 1;
    dbg_wrk.sbtset = ingame_wrk.msn_no != 0;

    dbg_wrk.eff_new_def_sw = 0;
    dbg_wrk.eff_new_def_alp1 = 0x64;
    dbg_wrk.eff_new_def_alp2 = 0x78;
    dbg_wrk.eff_new_def_col1 = 0x80;
    dbg_wrk.eff_new_def_col2 = 0x88;

    dbg_wrk.disp_3d_all = 1;
    dbg_wrk.disp_3d_room = 1;
    dbg_wrk.disp_3d_furn = 1;
    dbg_wrk.disp_3d_girl = 1;
    dbg_wrk.disp_3d_enemy = 1;
    dbg_wrk.disp_3d_mirror = 1;
    dbg_wrk.disp_3d_textrans = 1;
    dbg_wrk.disp_3d_2ddraw = 1;
    dbg_wrk.disp_3d_girl_shadow = 1;
    dbg_wrk.disp_3d_room_shadow = 1;

    dbg_wrk.eff_amufire = 1;

    dbg_wrk.oth_hint_sw = 0;
    dbg_wrk.oth_hint_no = 0;
    dbg_wrk.oth_hint_alp = 0x80;

    dbg_wrk.eff_new_ene_ef_sw = 0;
    dbg_wrk.eff_new_ene_ef_alp = 0x80;
    dbg_wrk.eff_new_ene_ef_sz = 0x32;

    dbg_wrk.high_speed_mode = 0;
    dbg_wrk.param_enestop = 0;

    dbg_wrk.param_pad_type = 0;

    dbg_wrk.cc_charge_range = 3;
    dbg_wrk.cc_charge_max = 3;
    dbg_wrk.cc_charge_speed = 3;
    dbg_wrk.cc_set_sub = 0;
    dbg_wrk.cc_set_spe = 0;

    dbg_wrk.param_fuyurei = 1;
    dbg_wrk.param_enetype = 0;
    dbg_wrk.param_muteki = 0;

    dbg_wrk.oth_sub_menu = 0;

    dbg_wrk.black_white = 0;
}

void gra2dDrawDbgMenuSub(DEBUG_MENU *wlp)
{
	int i;
	int n;
	int bx;
	int by;
	int sw;
	int sh;
    int var1;
    int var2;

start:
    bx = (wlp->kai * 32) - 300;
    by = (wlp->kai * 32) - 200;
    sw = (wlp->max * 12) + 12;
    sh = (wlp->mnum * 14) + 4;

    SetSquareSN((((0x30 - wlp->kai) & 0xff) * 16) - 0, bx + 0, by + 0, (bx + sw) + 14, (by + sh) + 35, 0x00, 0x00, 0x00, 0x80);
    SetSquareSN((((0x30 - wlp->kai) & 0xff) * 16) - 1, bx + 1, by + 1, (bx + sw) + 13, (by + sh) + 33, 0x40, 0x40, 0x40, 0x80);
    SetSquareSN((((0x30 - wlp->kai) & 0xff) * 16) - 2, bx + 3, by + 3, (bx + sw) + 11, (by + sh) + 31, 0x80, 0x80, 0x80, 0x80);
    SetSquareSN((((0x30 - wlp->kai) & 0xff) * 16) - 3, bx + 5, by + 5, (bx + sw) + 9, (by + sh) + 29, 0x40, 0x40, 0x40, 0x80);
    SetSquareSN((((0x30 - wlp->kai) & 0xff) * 16) - 4, bx + 7, by + 7, (bx + sw) + 7, (by + sh) + 27, 0x00, 0x00, 0x00, 0x80);
    SetSquareSN((((0x30 - wlp->kai) & 0xff) * 16) - 5, bx + 5, by + 24, (bx + sw) + 9, by + 29, 0x40, 0x40, 0x40, 0x80);
    SetSquareSN((((0x30 - wlp->kai) & 0xff) * 16) - 6, bx + 3, by + 26, (bx + sw) + 11, by + 27, 0x80, 0x80, 0x80, 0x80);
    SetSquareSN((((0x30 - wlp->kai) & 0xff) * 16) - 2, bx + 0, by + 0, bx + 4, by + 4, 0x00, 0x00, 0x00, 0x80);
    SetSquareSN((((0x30 - wlp->kai) & 0xff) * 16) - 2, bx + 0, (by + sh) + 31, bx + 4, (by + sh) + 35, 0x00, 0x00, 0x00, 0x80);
    SetSquareSN((((0x30 - wlp->kai) & 0xff) * 16) - 2, (bx + sw) + 10, by + 0, (bx + sw) + 14, by + 4, 0x00, 0x00, 0x00, 0x80);
    SetSquareSN((((0x30 - wlp->kai) & 0xff) * 16) - 2, (bx + sw) + 10, (by + sh) + 31, (bx + sw) + 14, (by + sh) + 35, 0x00, 0x00, 0x00, 0x80);

    SetASCIIString2((((0x30 - wlp->kai) & 0xff) * 16) - 7, bx + 333, by + 234, 1, 0x80, 0x80, 0x80, wlp->title);

    var1 = 2;

    for (i = wlp->mnum - 1; i >= 0; i--)
    {
        n = wlp->submenu[i].subnum & 0xfff;

        if (wlp->submenu[i].subnum & 0x8000)
        {
            SetString2((((0x30 - wlp->kai) & 0xff) * 16) - 7, bx + 333, ((by + var1) + 255) + (i * 14), 1, 0x80, 0x80, 0x80, "%s", wlp->submenu[i].name);
            SetString2((((0x30 - wlp->kai) & 0xff) * 16) - 7, (bx + ((wlp->max - 5) * 12)) + 333, ((by + var1) + 255) + (i * 14), 1, 0x80, 0x80, 0x80, "%4d", *dbgmenu_inttbl[n]);
        }
        else if (wlp->submenu[i].subnum & 0x2000)
        {
            SetString2((((0x30 - wlp->kai) & 0xff) * 16) - 7, bx + 333, ((by + var1) + 255) + (i * 14), 1, 0x80, 0x80, 0x80, "%s", wlp->submenu[i].name);
            SetString2((((0x30 - wlp->kai) & 0xff) * 16) - 7, (bx + ((wlp->max - 4) * 12)) + 333, ((by + var1) + 255) + (i * 14), 1, 0x80, 0x80, 0x80, "%s", (*dbgmenu_inttbl[n]) ? " ON" : "OFF");
        }
        else if (wlp->submenu[i].subnum & 0x1000)
        {
            SetString2((((0x30 - wlp->kai) & 0xff) * 16) - 7, bx + 333, ((by + var1) + 255) + (i * 14), 1, 0x80, 0x80, 0x80, "%s", wlp->submenu[i].name);
            SetString2((((0x30 - wlp->kai) & 0xff) * 16) - 7, (bx + ((wlp->max - 1) * 12)) + 333, ((by + var1) + 255) + (i * 14), 1, 0x80, 0x80, 0x80, "*");
        }
        else
        {
            SetASCIIString2((((0x30 - wlp->kai) & 0xff) * 16) - 7, bx + 333, ((by + var1) + 255) + (i * 14), 1, 0x80, 0x80, 0x80, wlp->submenu[i].name);
        }
    }

    sh = (wlp->pos * 14) + 31;

    var2 = by + 2;
    var1 = sh;

    SetSquareSN((((0x30 - wlp->kai) & 0xff) * 16) - 6, bx + 7, (by + sh) - 2, (bx + sw) + 7, (var2 + var1) + 12, 0x40, 0x40, 0x80, 0x40);

    if (wlp->parent == -1)
    {
        return;
    }

    wlp = dbgmenu_tbl[wlp->parent];

    goto start;
}

void gra2dDrawDbgMenu()
{
	static int now_tree = 0;
	int i;
	int l;
	int n;
	int ok;
	int pad;
	char cwo[16];
	DEBUG_MENU *nlp;
	DEBUG_MENU *wlp;

    dbg_wrk.param_film14 = poss_item[1];
    dbg_wrk.param_film37 = poss_item[2];
    dbg_wrk.param_film74 = poss_item[3];
    dbg_wrk.param_film90 = poss_item[4];
    dbg_wrk.param_manyo = poss_item[6];
    dbg_wrk.param_reiseki = poss_item[5];
    dbg_wrk.param_camera = poss_item[9];

    dbg_wrk.cc_charge_range = cam_custom_wrk.charge_range;
    dbg_wrk.cc_charge_max = cam_custom_wrk.charge_max;
    dbg_wrk.cc_charge_speed = cam_custom_wrk.charge_speed;
    dbg_wrk.cc_set_sub = cam_custom_wrk.set_sub;
    dbg_wrk.cc_set_spe = cam_custom_wrk.set_spe;

    dbg_wrk.oth_sub_menu = ingame_wrk.clear_count;

    nlp = dbgmenu_tbl[now_tree];

    if (nlp->off_num != -1)
    {
        nlp->pos = *dbgmenu_inttbl[nlp->off_num];
    }

    nlp->kai = 0;

    wlp = nlp;

    while (wlp->parent != -1)
    {
        wlp = dbgmenu_tbl[wlp->parent];
        nlp->kai++;
    }

    nlp->mnum = 0;

    while(strcmp(nlp->submenu[nlp->mnum].name, "_end_") != 0)
    {
        nlp->mnum++;
    }

    nlp->max = 0;

    for (i = 0; i < nlp->mnum; i++)
    {
        l = strlen(nlp->submenu[i].name);

        if (nlp->submenu[i].subnum & 0x1000)
        {
            l += 5;
        }

        if ((nlp->submenu[i].subnum & 0x8000))
        {
            l += 8;
        }

        if (nlp->submenu[i].subnum & 0x2000)
        {
            l += 7;
        }

        nlp->max = nlp->max < l ? l : nlp->max;
    }

    l = strlen(nlp->title);
    nlp->max = nlp->max < l ? l : nlp->max;

    if (*key_now[1] == 1)
    {
        nlp->pos = nlp->pos < nlp->mnum -1 ? nlp->pos + 1 : 0;

        if (nlp->off_num != -1)
        {
            *dbgmenu_inttbl[nlp->off_num] = nlp->pos;
        }
    }

    if (*key_now[0] == 1)
    {
        nlp->pos = nlp->pos < 1 ? nlp->mnum - 1 : nlp->pos - 1;

        if (nlp->off_num != -1)
        {
            *dbgmenu_inttbl[nlp->off_num] = nlp->pos;
        }
    }

    if (*key_now[3] != 0)
    {
        ok = 0;

        if (*key_now[9] == 0 || *key_now[3] == 1)
        {
            pad = *key_now[10] != 0 ? 3 : 1;

            if (nlp->submenu[nlp->pos].subnum & 0x10000)
            {
                ok = *key_now[3] != 1 ? ok : 1;
            }

            if ((nlp->submenu[nlp->pos].subnum & 0x10000) == 0 && nlp->submenu[nlp->pos].subnum & 0xa000)
            {
                ok = 1;
            }

            n = nlp->submenu[nlp->pos].subnum & 0xfff;

            if (ok)
            {
                if (n >= 29 && n <= 34)
                {
                    if (*key_now[11] != 0)
                    {
                        if (n >= 29 && n <= 31)
                        {
                            *dbgmenu_inttbl[29] = MIN(*dbgmenu_inttbl[29] + pad, nlp->submenu[nlp->pos].nmax);
                            *dbgmenu_inttbl[30] = MIN(*dbgmenu_inttbl[30] + pad, nlp->submenu[nlp->pos].nmax);
                            *dbgmenu_inttbl[31] = MIN(*dbgmenu_inttbl[31] + pad, nlp->submenu[nlp->pos].nmax);
                        }
                        else
                        {
                            *dbgmenu_inttbl[32] = MIN(*dbgmenu_inttbl[32] + pad, nlp->submenu[nlp->pos].nmax);
                            *dbgmenu_inttbl[33] = MIN(*dbgmenu_inttbl[33] + pad, nlp->submenu[nlp->pos].nmax);
                            *dbgmenu_inttbl[34] = MIN(*dbgmenu_inttbl[34] + pad, nlp->submenu[nlp->pos].nmax);
                        }
                    }
                    else
                    {
                        *dbgmenu_inttbl[n] = MIN(*dbgmenu_inttbl[n] + pad, nlp->submenu[nlp->pos].nmax);
                    }
                }
                else if ((nlp->submenu[nlp->pos].subnum & 0x2000) == 0)
                {
                    *dbgmenu_inttbl[n] = MIN(*dbgmenu_inttbl[n] + pad, nlp->submenu[nlp->pos].nmax);
                }
                else if (*key_now[3] == 1)
                {
                    *dbgmenu_inttbl[n] = MAX0(*dbgmenu_inttbl[n] + pad, nlp->submenu[nlp->pos].nmax);
                }
            }
        }
    }

    if (*key_now[2] != 0)
    {
        ok = 0;

        if (*key_now[9] == 0 || *key_now[2] == 1)
        {
            pad = *key_now[10] != 0 ? 3 : 1;

            if (nlp->submenu[nlp->pos].subnum & 0x10000)
            {
                ok = *key_now[3] != 1 ? ok : 1;
            }

            if (nlp->submenu[nlp->pos].subnum & 0x10000)
            {
                ok = *key_now[2] != 1 ? ok : 1;
            }

            if ((nlp->submenu[nlp->pos].subnum & 0x10000) == 0 && nlp->submenu[nlp->pos].subnum & 0xa000)
            {
                ok = 1;
            }

            n = nlp->submenu[nlp->pos].subnum & 0xfff;

            if (ok)
            {
                if (n >= 29 && n <= 34)
                {
                    if (*key_now[11] != 0)
                    {
                        if (n >= 29 && n <= 31)
                        {
                            *dbgmenu_inttbl[29] = MAX(*dbgmenu_inttbl[29] - pad, 0);
                            *dbgmenu_inttbl[30] = MAX(*dbgmenu_inttbl[30] - pad, 0);
                            *dbgmenu_inttbl[31] = MAX(*dbgmenu_inttbl[31] - pad, 0);
                        }
                        else
                        {
                            *dbgmenu_inttbl[32] = MAX(*dbgmenu_inttbl[32] - pad, 0);
                            *dbgmenu_inttbl[33] = MAX(*dbgmenu_inttbl[33] - pad, 0);
                            *dbgmenu_inttbl[34] = MAX(*dbgmenu_inttbl[34] - pad, 0);
                        }
                    }
                    else
                    {
                        *dbgmenu_inttbl[n] = MAX(*dbgmenu_inttbl[n] - pad, 0);
                    }
                }
                else if (nlp->submenu[nlp->pos].subnum & 0x2000)
                {
                    if (*key_now[2] == 1)
                    {
                        *dbgmenu_inttbl[n] = MIN0(*dbgmenu_inttbl[n] - pad, nlp->submenu[nlp->pos].nmax);
                    }
                }
                else
                {
                    *dbgmenu_inttbl[n] = MAX(*dbgmenu_inttbl[n] - pad, 0);
                }
            }
        }
    }

    if (nlp->submenu[nlp->pos].subnum & 0x8000 && *key_now[11] != 0 && *key_now[9] != 0)
    {
        n = nlp->submenu[nlp->pos].subnum & 0xfff;
        *dbgmenu_inttbl[n] = 0;
    }

    if (*key_now[5] == 1)
    {
        if ((nlp->submenu[nlp->pos].subnum & 0xe000))
        {
            if (nlp->parent != -1)
            {
                now_tree = nlp->parent;
            }
        }
        else
        {
            now_tree = nlp->submenu[nlp->pos].subnum & 0xfff;

            if ((now_tree >= 18 && now_tree <= 33) || (now_tree == 49 || now_tree == 50))
            {
                MakeLightEditorData(dbgmenu_tbl, now_tree);
            }
            else if (now_tree == 44)
            {
                MakeFogData((DEBUG_SUB_MENU **)dbgmenu_tbl, nlp->pos);
            }
            else if (now_tree == 45)
            {
                ApplyFogData();
            }
        }

        if (nlp->kai == 0 && strcmp(nlp->submenu[nlp->pos].name, "EXIT") == 0)
        {
            dbg_wrk.mode_on = 2;
        }
    }

    if (*key_now[4] == 1)
    {
        if (nlp->parent != -1)
        {
            now_tree = nlp->parent;
        }
        else
        {
            dbg_wrk.mode_on = 0;
        }
    }

    if (*key_now[13] == 1)
    {
        dbg_wrk.mode_on = 2;
    }

    gra2dDrawDbgMenuSub(nlp);

    poss_item[1] = dbg_wrk.param_film14;
    poss_item[2] = dbg_wrk.param_film37;
    poss_item[3] = dbg_wrk.param_film74;
    poss_item[4] = dbg_wrk.param_film90;
    poss_item[6] = dbg_wrk.param_manyo;
    poss_item[5] = dbg_wrk.param_reiseki;

    if (dbg_wrk.param_camera != poss_item[9] && dbg_wrk.param_camera == 1)
    {
        poss_item[1] = 0x32;
        poss_item[2] = 0x32;
        poss_item[3] = 0x32;
        poss_item[4] = 0x32;
    }

    poss_item[9] = dbg_wrk.param_camera;

    cam_custom_wrk.charge_range = dbg_wrk.cc_charge_range;
    cam_custom_wrk.charge_max = dbg_wrk.cc_charge_max;
    cam_custom_wrk.charge_speed = dbg_wrk.cc_charge_speed;
    cam_custom_wrk.set_sub = dbg_wrk.cc_set_sub;
    cam_custom_wrk.set_spe = dbg_wrk.cc_set_spe;

    ingame_wrk.clear_count = dbg_wrk.oth_sub_menu;
}

void SetMaxNumber(int num1, int num2, int nmax)
{
    dbgmenu_tbl[num1]->submenu[num2].nmax = nmax;
}

static int sbtset_old = 0;

void InitShibataSet()
{
    sbtset_old = 0;
}

void SetShibataSet()
{
    if (sbtset_old == dbg_wrk.sbtset)
    {
        return;
    }

    switch(dbg_wrk.sbtset)
    {
    case 0:
        dbg_wrk.eff_dither = 0;
        dbg_wrk.eff_dithsp = 8;
        dbg_wrk.eff_dithal = 0x20;
        dither_alp = 0x40;
        dither_col = 0x78;
        dbg_wrk.eff_focus = 0;
        dbg_wrk.eff_sccol_tp1 = 0;
        dbg_wrk.eff_sccol_tp2 = 0;
        dbg_wrk.eff_sccol_cont_col = 0x80;
        dbg_wrk.eff_sccol_cont_alp = 0x80;
        dbg_wrk.eff_sccol_blk = 0;
        dbg_wrk.eff_ffr = 0;
        dbg_wrk.eff_ffra = 0x80;
    break;
    case 1:
        dbg_wrk.eff_dither = 0;
        dbg_wrk.eff_dithsp = 8;
        dbg_wrk.eff_dithal = 0x20;
        dither_alp = 0x40;
        dither_col = 0x78;
        dbg_wrk.eff_focus = 0;
        dbg_wrk.eff_sccol_tp1 = 0;
        dbg_wrk.eff_sccol_tp2 = 3;
        dbg_wrk.eff_sccol_cont_col = 0x8c;
        dbg_wrk.eff_sccol_cont_alp = 0x8c;
        dbg_wrk.eff_sccol_blk = 0x28;
        dbg_wrk.eff_ffr = 1;
        dbg_wrk.eff_ffra = 0x3c;
    break;
    case 2:
        dbg_wrk.eff_dither = 2;
        dbg_wrk.eff_dithsp = 9;
        dbg_wrk.eff_dithal = 0x23;
        dither_alp = 0x43;
        dither_col = 0x66;
        dbg_wrk.eff_focus = 0;
        dbg_wrk.eff_sccol_tp1 = 0;
        dbg_wrk.eff_sccol_tp2 = 2;
        dbg_wrk.eff_sccol_cont_col = 0x87;
        dbg_wrk.eff_sccol_cont_alp = 0x87;
        dbg_wrk.eff_sccol_blk = 0x28;
        dbg_wrk.eff_ffr = 1;
        dbg_wrk.eff_ffra = 0x3c;
    break;
    case 3:
        dbg_wrk.eff_dither = 6;
        dbg_wrk.eff_dithsp = 10;
        dbg_wrk.eff_dithal = 0x4a;
        dither_alp = 0x43;
        dither_col = 0x66;
        dbg_wrk.eff_focus = 0;
        dbg_wrk.eff_sccol_tp1 = 1;
        dbg_wrk.eff_sccol_tp2 = 3;
        dbg_wrk.eff_sccol_cont_col = 200;
        dbg_wrk.eff_sccol_cont_alp = 200;
        dbg_wrk.eff_sccol_blk = 0x3c;
        dbg_wrk.eff_ffr = 1;
        dbg_wrk.eff_ffra = 0x3c;
    break;
    case 4:
        dbg_wrk.eff_dither = 4;
        dbg_wrk.eff_dithsp = 8;
        dbg_wrk.eff_dithal = 0x2e;
        dither_alp = 0x37;
        dither_col = 0x66;
        dbg_wrk.eff_focus = 0x14;
        dbg_wrk.eff_sccol_tp1 = 0;
        dbg_wrk.eff_sccol_tp2 = 2;
        dbg_wrk.eff_sccol_cont_col = 0x87;
        dbg_wrk.eff_sccol_cont_alp = 0x87;
        dbg_wrk.eff_sccol_blk = 0x28;
        dbg_wrk.eff_ffr = 1;
        dbg_wrk.eff_ffra = 0x3c;
    break;
    case 5:
        dbg_wrk.eff_blur = 1;
        dbg_wrk.eff_blra = 0x39;
        dbg_wrk.eff_blrs = 1000;
        dbg_wrk.eff_blrr = 0x708;
        dbg_wrk.eff_sccol_tp1 = 0;
        dbg_wrk.eff_sccol_tp2 = 2;
        dbg_wrk.eff_sccol_cont_col = 0x46;
        dbg_wrk.eff_sccol_cont_alp = 0x46;
        dbg_wrk.eff_ffr = 1;
        dbg_wrk.eff_ffra = 0x1e;
    break;
    }

    sbtset_old = dbg_wrk.sbtset;
}

static int control;

static int perf_sec = 0;
static float perf_max = 0.0f;

void InitPerformanceCounter()
{
    control = \
        SCE_PC_EXL0 | SCE_PC_K0 | SCE_PC_S0 | SCE_PC_U0 | \
        SCE_PC0_CPU_CYCLE | SCE_PC_EXL1 | SCE_PC_K1 | \
        SCE_PC_S1 | SCE_PC_U1 | SCE_PC1_NO_EVENT | SCE_PC_CTE;

    perf_sec = 0;
    perf_max = 0.0f;
}

void StartPerformanceCounter()
{
    scePcStart(control, 0, 0);
}

void StopPerformanceCounter()
{
	static u_char ow = 0;
	static int cnt = 0;
	// int ctr0;

    perf_sec = scePcGetCounter0();
    scePcStop();
}

void DrawPerformanceCounter()
{
    u_char r;
    u_char g;
    u_char b;
    int x1;
    int x2;
    int x3;
    int x4;
    int x5;
    int xx1;
    int xx2;
    float f;
    float d;

    x1 = -200;
    x2 = 200;
    x3 = 0;
    x4 = -100;
    x5 = 100;

    d = g_bInterlace != 0 ? 1.0f : 2.0f;

    if (dbg_wrk.oth_perf == 0)
    {
        return;
    }

    f = perf_sec / 4860800.0f;

    if (f > perf_max)
    {
        perf_max = f;
    }

    if (f < 1.0f)
    {
        r = f * 255.0f;
        g = f * 255.0f;
        b = 255.0f - f * 255.0f;
    }
    else if (f < 2.0)
    {
        r = 0xff;
        g = 255.0f - (f - 1.0f) * 255.0f;
        b = 0x0;
    }
    else
    {
        r = 0xff;
        g = 0x0;
        b = 0x0;
    }

    xx1 = (x1 + perf_max * x2) < 2047.0f ? (x1 + perf_max * x2) : 2047.0f;
    xx2 = (x1 + f * x2) < 2047.0f ? (x1 + f * x2) : 2047.0f;

    SetSquareS(16, x1, 204.0f / d, xx1, 214.0f / d, 0x40, 0x40, 0x40, 0x80);
    SetSquareS(16, x1, 198.0f / d, xx2, 208.0f / d,    r,    g,    b, 0x80);

    SetLine2(16, x1, 190.0f / d, x2, 190.0f / d, 0x80, 0x80, 0x80, 0x80);
    SetLine2(16, x1, 190.0f / d, x1, 220.0f / d, 0x80, 0x80, 0x80, 0x80);
    SetLine2(16, x2, 190.0f / d, x2, 220.0f / d, 0x80, 0x80, 0x80, 0x80);
    SetLine2(16, x1, 220.0f / d, x2, 220.0f / d, 0x80, 0x80, 0x80, 0x80);

    SetLine2(16, x3, 190.0f / d, x3, 220.0f / d, 0x60, 0x60, 0x80, 0x80);
    SetLine2(16, x4, 190.0f / d, x4, 220.0f / d, 0x40, 0x40, 0x60, 0x80);
    SetLine2(16, x5, 190.0f / d, x5, 220.0f / d, 0x40, 0x40, 0x60, 0x80);

    if (perf_sec != 0)
    {
        SetString2(16, 12.0f, 401.0f / d, 1, 0x80, 0x80, 0x80, "%4d/224", (int)(perf_sec * 224.0f / 4860800.0f));
    }

    //                                                      EUC-JP !! 2534648ea5253264252500 (%4d･%2d%%)
    SetString2(16, 12.0f, 416.0f / d, 1, 0x80, 0x80, 0x80, "%4d\x8e\xa5%2d%%", (int)(perf_sec * 100.0f / 4860800.0f), (int)(perf_sec * 10000.0f / 4860800.0f) % 100);
    SetString2(16, 12.0f, 431.0f / d, 1, 0x80, 0x80, 0x80, "%4d\x8e\xa5%2d%%", (int)(perf_max * 100.0f), (int)(perf_max * 10000.0f) % 100);
}

void WaitFrameTop(int type)
{
    if (type != 0)
    {
        while ((scePcGetCounter0() / 0x4a2b80 & 1) == 0) {};
    }

    while ((scePcGetCounter0() / 0x4a2b80 & 1) != 0) {};
}

void CheckHintTex()
{
	int n;
	int num;
	int target;
	u_char alp;
	float posy;
	static int num_old = -1;
	static int hint_tbl[48] = {
#ifdef BUILD_EU_VERSION
        548, 549, 550, 551, 552, 553, 554, 555,
        556, 557, 558, 559, 560, 561, 562, 563,
        564, 565, 566, 567, 568, 569, 570, 571,
        572, 573, 574, 575, 576, 577, 578, 579,
        580, 581, 582, 583, 584, 585, 586, 587,
        588, 589, 590, 591, 592, 593, 594, 595,
#else
        280, 281, 282, 283, 284, 285, 286, 287,
        288, 289, 290, 291, 292, 293, 294, 295,
        296, 297, 298, 299, 300, 301, 302, 303,
        304, 305, 306, 307, 308, 309, 310, 311,
        312, 313, 314, 315, 316, 317, 318, 319,
        320, 321, 322, 323, 324, 325, 326, 327,
#endif
    };
	DISP_SPRT ds;

    n = 0;

    if (dbg_wrk.oth_hint_sw == 0)
    {
        return;
    }

    while (hintdat[n].tex0 != 0xffffffffffffffff)
    {
        n++;
    }

    SetMaxNumber(0x34, 2, (n / 2) - 1);

    num = dbg_wrk.oth_hint_no;

    if (hint_test_sw != 0)
    {
        alp = ((100.0f - hint_test_posx) * 64.0f) / 100.0f;
        dbg_wrk.oth_hint_alp = alp;
    }
    else
    {
        alp = dbg_wrk.oth_hint_alp;
        hint_test_posx = 0;
    }

    posy = hint_test_posx / 25.0f;

    if (num != num_old)
    {
        g2d_load_flg.hint = LoadReq(hint_tbl[num], 0x1e90000);
    }

    MakeTim2ClutDirect4(0x1e90000, 0, -1, -1, 0);
    MakeTim2ClutDirect4(0x1e90000, 1, -1, -1, 0);

    if (*key_now[9] != 0)
    {
        target = num * 2 + 1;
    }
    else
    {
        target = num * 2;
    }

    CopySprDToSpr(&ds, &hintdat[target]);

    ds.tex1 = 0x161;
    ds.zbuf = 0x10100008c;
    ds.test = 0x3000d;
    ds.alphar = 0x8000000044;
    ds.pri = 0x470;
    ds.z = 0xffffb8f;
    ds.alpha = alp;

    if (hint_test_sw != 0)
    {
        ds.pri = 0x4b0;
        ds.z = 0xffffb4f;
        ds.x = (320 - (int)(hintdat[target].w / 2)) - posy;
        ds.y = (324 - (int)(hintdat[target].h / 2)) - posy;

        DispSprD(&ds);

        ds.pri = 0x4a0;
        ds.z = 0xffffb5f;
        ds.x = (320 - (int)(hintdat[target].w / 2)) - posy;
        ds.y = (324 - (int)(hintdat[target].h / 2)) + posy;

        DispSprD(&ds);

        ds.pri = 0x490;
        ds.z = 0xffffb6f;
        ds.x = (320 - (int)(hintdat[target].w / 2)) + posy;
        ds.y = (324 - (int)(hintdat[target].h / 2)) - posy;

        DispSprD(&ds);

        ds.pri = 0x480;
        ds.z = 0xffffb7f;
        ds.x = (320 - (int)(hintdat[target].w / 2)) + posy;
        ds.y = (324 - (int)(hintdat[target].h / 2)) + posy;

        DispSprD(&ds);
    }
    else
    {
        ds.x = 320 - (int)(hintdat[target].w / 2);
        ds.y = 324 - (int)(hintdat[target].h / 2);

        DispSprD(&ds);
    }

    num_old = num;
}

int hint_test_sw = 0;
int hint_test_posx = 0;
