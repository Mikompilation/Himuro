#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "inf_disp.h"

#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_ene.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph2d/number.h"
// #include "graphics/graph2d/tim2.h" // (miss) DispSprD
#include "graphics/graph3d/sglib.h"
#include "ingame/event/ev_main.h"
#include "ingame/info/inf_disp.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/item.h"
#include "ingame/plyr/plyr_ctl.h"
#include "main/glob.h"
#include "os/eeiop/eese.h"
#include "outgame/btl_mode/btl_mode.h"

static void FndrInit();
static void SttsFade();
static void FndrFade();
static void WeakPoint(short int pos_x, short int pos_y);
static void DspBigCircle(u_short lu_chr, short int pos_x, short int pos_y, u_char alp, short int size_r, u_char cl_ptn);
static void PointerNP(short int cx, short int cy, u_char red, u_char alp, float siz);
static void PointerEN(short int pos_x, short int pos_y, u_char red, u_char alp, float size);
static void FilmZansu(int number, short int pos_x, short int pos_y, short int num_x, short int num_y, int digit);
static void NumDisp(u_char number, u_char no, short int pos_x, short int pos_y, short int num_x, short int num_y, u_char mode);
static void EdogawaLamp(short int pos_x, short int pos_y, u_char out);
static void NewFndrBase(short int pos_x, short int pos_y);
static void BigBon(u_char bon_num, short int sx, short int sy, short int ex, short int ey, u_char mov_tim);
static void BigBon2(u_char bon_num, short int sx, short int sy, short int ex, short int ey, u_char mov_tim);
static char GetEnePoint();
static void MafubaInit(float t2_s1, float t2_s2);
static u_char Mafuba(short int pos_x, short int pos_y, u_char stts, u_char t1, u_char t2, u_char t3, u_char t4, u_char t5, u_char t1_sw, float t2_s1, float t2_s2);
static void MafubaFinish(short int pos_x, short int pos_y, u_char start, u_char time);
static void DameEffNumber(u_char type, u_short num, short int pos_x, short int pos_y, u_char stts, u_char num_alp, u_char num_siz_x, u_char num_siz_y);
static void SndEffLoop();
static void ShowEneCtrl(short int pos_x, short int pos_y);
static char EneDamegeCtrl(short int pos_x, short int pos_y);
static void ShowEnePower(u_char blu_bar, u_char red_bar, short int pos_x, short int pos_y, u_char alp, u_char bar_over);
static void Koutakun(u_char blu_bar, short int pos_x, short int pos_y, u_char alp);
static void ShowMikuPower(short int pos_x, short int pos_y);
static void ShowMikuPower2(short int pos_x, short int pos_y);
static void DspKotaku(u_short chr, short int pos_x, short int pos_y, u_char alp, u_char dsp_u, u_char dsp_w, u_short dsp_x);
static void GayBar(u_short chr, short int pos_x, short int pos_y, u_char alp, float scl);
static void GayBar2(u_short top_chr, u_short end_chr, short int pos_x, short int pos_y, u_char alp, float scl);
static void JetCD(JET_SET *ninjin, u_char num, u_char tim, u_char spd, u_char wid, u_short chr, u_char hgh, short int mov_h, short int mov_v, short int px, short int base, u_char alp);
static void SyonenJet(u_short chr, u_char hgh, short int mov_h, short int mov_v, short int px, short int base, u_char alp, float scl_x, float scl_y);
static void TameKin2(short int pos_x, short int pos_y);
static void ZanzoSS(ZAN *zz);
static void ZanzoSM(ZAN *zz, u_char num1, u_char num2, u_char num3, u_char num4, u_char mode);
static void ZanzoLS(u_char *c);
static void ZanzoLM(u_char *zz, u_char per);
static void AhoBon(u_short chr, short int px, short int py, u_char rot, u_char rgb, u_char alp, u_char siz, u_char now_num, u_char ini_num);
static void BakBon(u_short chr, short int px, short int py, u_char flr);
static void PhotoScoreDisp(int count, int wait);
static void BukiyouMan(short int px, short int py);
static void DameDameInit();
static void GekisyaBoyGeisyaGirl(short int px, short int py);
static char KanKinoSyogai(short int pos_x, short int pos_y);

#include "data/spr_dat.h" // SPRT_DAT spr_dat[];
#include "data/flip_table.h" // short unsigned int flip_table[];
#include "data/raze_sprt.h" // SPRT_DAT raze_sprt;
INFO_WRK info_wrk = {0};

static FLSH_CORE flsh_lgt;
static FLSH_CORE flsh_chrg[2];
static FLSH_CORE feramp;

static INF_DSP inf_dsp;
static NEW_INF new_inf;
static LIFE_BALL life_ball[5];
static PRTCL_BALL prtcl_ball[5][24];
static FLSH_CORE flsh_max[5];
static ZAN zanzo[12];
static ZAN dmg_scr;
static JET_SET jet1[25];
static JET_SET jet2[25];
static u_char znz[12][6];

#define PI 3.1415927f
#define DEG2RAD(x) ((float)(x)*PI/180.0f)


#ifdef BUILD_EU_VERSION
#define PL_FNDR_PK2_ADDRESS 0x1d83000
#define PL_LIFE_PK2_ADDRESS 0x1ded000
#else
#define PL_FNDR_PK2_ADDRESS 0x1d88100
#define PL_LIFE_PK2_ADDRESS 0x1df2100
#endif

void InformationDispInit()
{
    info_wrk = (INFO_WRK){0};
    inf_dsp = (INF_DSP){0};
    new_inf = (NEW_INF){0};

    memset(&jet1, 0, sizeof(jet1));
    memset(&jet2, 0, sizeof(jet2));

    point_get_end = 0;
}

void InformationDispMain()
{
    short int fndr_mx;
    short int fndr_my;

    if (inf_dsp.disp_pause != 0)
    {
        return;
    }

    fndr_mx = (((plyr_wrk.fp[0] - 320) * 8.5f) / 10.0f);
    fndr_my = (((plyr_wrk.fp[1] - 209) * 8.5f) / 10.0f);

    InformationDispModeCtrl();
    InformationDispWrkSet();

    if (inf_dsp.fndr_mode_tmr == 1)
    {
        FndrInit();
    }

    if (inf_dsp.fndr_dsp_flg != 0)
    {
        FinderDisp(fndr_mx, fndr_my);
    }

    if (sys_wrk.game_mode == GAME_MODE_INGAME)
    {
        if ((ingame_wrk.stts & 0x20) == 0 && MesStatusCheck() == 0)
        {
            if (inf_dsp.fndr_dsp_flg != 0)
            {
                EdogawaLamp(fndr_mx, fndr_my, 0);
            }
            else if (isDispLamp() != 0)
            {
                SetSprFile(PL_LIFE_PK2_ADDRESS);
                EdogawaLamp(230, 352, 1);
            }
            else
            {
                new_inf.rmp_alp = 0;
            }
        }
    }

    if (inf_dsp.stts_dsp_flg != 0)
    {
        if (inf_dsp.fndr_dsp_flg != 0)
        {
            PlayerInformationDisp(fndr_mx, fndr_my);
        }
        else
        {
            PlayerInformationDisp(0, 0);
        }
    }

    if (inf_dsp.scor_dsp != 0)
    {
        PhotoScoreDisp(plyr_wrk.ap_timer, plyr_wrk.sta & 0x1 ? 1 : 60);
    }
}

void InformationDispModeCtrl()
{
    if (inf_dsp.stts_dsp_flg != 0)
    {
        inf_dsp.stts_dsp_flg = 2;
    }

    if (inf_dsp.fndr_dsp_flg != 0)
    {
        inf_dsp.fndr_dsp_flg = 2;
    }

    if (plyr_wrk.mode == PMODE_FINDER)
    {
        inf_dsp.stts_dsp_flg = 3;
        inf_dsp.fndr_dsp_flg = 3;
    }

    if (plyr_wrk.hp < info_wrk.plyr_hp)
    {
        inf_dsp.stts_fade_tmr = 180;
    }

    if (inf_dsp.stts_fade_tmr != 0)
    {
        inf_dsp.stts_dsp_flg = 3;
    }

    if (ev_wrk.movie_on != 0)
    {
        inf_dsp.stts_dsp_flg = 0;
        inf_dsp.fndr_dsp_flg = 0;
    }

    if (ingame_wrk.stts & 0x20)
    {
        inf_dsp.stts_dsp_flg = 0;
        inf_dsp.fndr_dsp_flg = 0;
    }

    if (ingame_wrk.mode == INGAME_MODE_MENU)
    {
        inf_dsp.stts_dsp_flg = 0;
        inf_dsp.fndr_dsp_flg = 0;
    }

    if (plyr_wrk.ap_timer != 0)
    {
        inf_dsp.phot_shot = 10;
        inf_dsp.fndr_dsp_flg = 0;
        inf_dsp.stts_dsp_flg = 0;
    }

    else if (inf_dsp.phot_shot != 0)
    {
        inf_dsp.phot_shot--;
        inf_dsp.stts_dsp_flg = 1;
        inf_dsp.fndr_dsp_flg = 1;
    }

    SttsFade();
    FndrFade();

    if (inf_dsp.fndr_dsp_flg != 0)
    {
        SetSprFile(PL_FNDR_PK2_ADDRESS);
        SetSprFile(PL_LIFE_PK2_ADDRESS);
    }

    if (inf_dsp.stts_dsp_flg != 0)
    {
        SetSprFile(PL_LIFE_PK2_ADDRESS);
    }
    else
    {
        new_inf.red_bar = (plyr_wrk.hp * 243) / 500.0f;
    }
}

static void FndrInit()
{
    int i;
    short int init;

    for (i = 0; i < 5; i++)
    {
        init = (2883584 + 983040 * i) / 65536; // same as "44 + 15 * i", but with a higher timer-related resolution?

        if (init < 0)
        {
            init = 0;
        }

        flsh_max[i].flsh_tmr = init;
    }

    new_inf.bon_bak = 0xff;
    new_inf.mfb_stts = 0;

    new_inf.pht_scr = photo_wrk.score;
    new_inf.pht_cnt_bak = ingame_wrk.pht_cnt;

    new_inf.buki_use_flg = 0;
    new_inf.buki_use_alp = 0;
    new_inf.buki_use_flr = 0;

    new_inf.dsp_dmg_flg = 0;
    new_inf.rmp_alp = 0;

    for (i = 0; i < 3; i++)
    {
        new_inf.shot_kind[i] = 0;
    }

    new_inf.shot_kind2 = 0;

    for (i = 0; i < 12; i++)
    {
        ZanzoSS(&zanzo[i]);
    }

    MafubaInit(0.0f, 0.0f);

    new_inf.dsp_dmg_step = 0;
    new_inf.dsp_dmg_step2 = 0;
    new_inf.dsp_dmg_alp = 0;

    new_inf.dmg_num_alp = 0;
    new_inf.dmg_str_alp = 0;
    new_inf.dmg_eff_alp = 0;
    new_inf.dmg_wait_tm = 0;

    new_inf.tm_max = 0;

    for (i = 0; i < 12; i++)
    {
        new_inf.chrg_rot[i] = 0;
        new_inf.chrg_flr[i] = 0;
        new_inf.chrg_stp[i] = 0;
        new_inf.dray_tmr[i] = 0;
    }
}

void InformationDispWrkSet()
{
    info_wrk.plyr_hp = plyr_wrk.hp;
    info_wrk.film_num = poss_item[plyr_wrk.film_no];
    info_wrk.frot_x = plyr_wrk.frot_x;
    info_wrk.plyr_pos_x = plyr_wrk.move_box.pos[0];
    info_wrk.plyr_pos_z = plyr_wrk.move_box.pos[2];

    if (inf_dsp.stts_fade_tmr != 0)
    {
        inf_dsp.stts_fade_tmr--;
    }

    if (inf_dsp.flm_cng_tmr != 0)
    {
        inf_dsp.flm_cng_tmr--;
    }

    if (plyr_wrk.mode == PMODE_FINDER)
    {
        if (inf_dsp.fndr_mode_tmr < 0xffff)
        {
            inf_dsp.fndr_mode_tmr++;
        }

        inf_dsp.fndr_fade_tmr = 10;
    }
    else if (inf_dsp.fndr_fade_tmr != 0)
    {
        inf_dsp.fndr_fade_tmr--;
    }
    else
    {
        inf_dsp.fndr_mode_tmr = 0;
    }
}

void PlayerInformationDisp(short int pos_x, short int pos_y)
{
    if (ingame_wrk.mode != INGAME_MODE_SPD_OPT)
    {
        ShowMikuPower2(pos_x, pos_y);
    }
}

void FinderDisp(short int pos_x, short int pos_y)
{
    short bon_x[4] = { 116, 494, 116, 494 };
    short bon_y[4] = {  86,  86, 333, 333 };
    char bon_c[12] = {
        10, 8, 6, 0, 1, 2, 3, 4, 5, 7, 9, 11,
    };
    u_char n;
    u_char sx;

    inf_dsp.lucky7 = ShutterChanceChk();

    NewFndrBase(pos_x, pos_y);
    WeakPoint(pos_x, pos_y);
    FilmZansu(info_wrk.film_num, pos_x, pos_y, 41, 377, 2);
    Mafuba(pos_x, pos_y, enedmg_status, 10, 10, 15, 10, 10, 1, 2.0f, 1.0f);
    MafubaFinish(pos_x, pos_y, eneseal_status, 60);
    ShowEneCtrl(pos_x, pos_y);
    GekisyaBoyGeisyaGirl(pos_x, pos_y);
    BukiyouMan(pos_x, pos_y);
    KanKinoSyogai(pos_x, pos_y);
    TameKin2(pos_x, pos_y);

    if (plyr_wrk.charge_num != 0)
    {
        n = cam_custom_wrk.charge_max * 2 + 6;
        sx = (12 - n) / 2;

        sx = sx + plyr_wrk.charge_num - 1;
        n = sx % 4;

        BigBon2(sx, bon_x[n], bon_y[n], 320, 224, 20);
    }
}

float NC_ADJ = 0.95f;

void PhotoScoreDispOnOff(int on_off)
{
    if (on_off != 0)
    {
        inf_dsp.scor_dsp = 1;
    }
    else
    {
        inf_dsp.scor_dsp = 0;
    }
}

static void SttsFade()
{
    switch (inf_dsp.stts_dsp_flg)
    {
    case 0:
        inf_dsp.stts_fade_alp = 0;
    break;
    case 1:
        inf_dsp.stts_fade_alp = 100;
    break;
    case 2:
        if (inf_dsp.stts_fade_alp - 10 > 0)
        {
            inf_dsp.stts_fade_alp -= 10;
        }
        else
        {
            inf_dsp.stts_fade_alp = 0;
            inf_dsp.stts_dsp_flg = 0;
        }
    break;
    case 3:
        if (inf_dsp.stts_fade_alp + 10 < 100)
        {
            inf_dsp.stts_fade_alp += 10;
        }
        else
        {
            inf_dsp.stts_fade_alp = 100;
            inf_dsp.stts_dsp_flg = 1;
        }
    break;
    }
}

static void FndrFade()
{
    switch (inf_dsp.fndr_dsp_flg)
    {
    case 1:
        inf_dsp.fndr_fade_alp = 100;
    break;
    case 2:
        if (inf_dsp.fndr_fade_alp - 10 > 0)
        {
            inf_dsp.fndr_fade_alp -= 10;
        }
        else
        {
            inf_dsp.fndr_fade_alp = 0;
            inf_dsp.fndr_dsp_flg = 0;
        }
    break;
    case 0:
        inf_dsp.fndr_fade_alp = 0;
    break;
    case 3:
        if (inf_dsp.fndr_fade_alp + 5 < 100)
        {
            inf_dsp.fndr_fade_alp += 5;
        }
        else
        {
            inf_dsp.fndr_fade_alp = 100;
            inf_dsp.fndr_dsp_flg = 1;
        }
    break;
    }
}

static void WeakPoint(short int pos_x, short int pos_y)
{
    int i;
    u_char alp;
    u_char siz;
    float ex;
    float ey;
    u_char hnt_alp_num;
    u_char ray_alp_num;
    short int hnt_all_alp;
    u_char chrg_max;
    u_char chrg_alp;
    float chrg_rate;
    short int big_size;

    alp = (inf_dsp.fndr_fade_alp * 128) / 100.0f;

    PutSpriteYW(FND_CNT_CIR_SGT, FND_CNT_CIR_SGT, pos_x, pos_y, 0.0f, 0x808080, alp * 0.5f, 1.0f, 1.0f, 0, 0xff, 1, 0, 1);

    big_size = circle_radius_tbl[cam_custom_wrk.charge_range];

    DspBigCircle(0x57, pos_x, pos_y, alp / 3.0f, big_size, 0);

    for (i = 0; i < 4; i++)
    {
        if (ene_wrk[i].sta & 0x1)
        {
            if (inf_dsp.weak5_alp[i] + 8 < alp)
            {
                inf_dsp.weak5_alp[i] += 8;
            }
            else
            {
                inf_dsp.weak5_alp[i] = alp;
            }
        }
        else
        {
            if (alp < inf_dsp.weak5_alp[i])
            {
                inf_dsp.weak5_alp[i] = alp;
            }

            if (inf_dsp.weak5_alp[i] - 8 > 0)
            {
                inf_dsp.weak5_alp[i] -= 8;
            }
            else
            {
                inf_dsp.weak5_alp[i] = 0;
            }
        }

        if (plyr_wrk.mode != 0x1)
        {
            if (alp < inf_dsp.weak5_alp[i])
            {
                inf_dsp.weak5_alp[i] = alp;
            }
        }

        if (ene_wrk[i].sta & 0x400)
        {
            if (inf_dsp.weak5_tmr[i] < 10)
            {
                inf_dsp.weak5_tmr[i]++;
            }
            else
            {
                inf_dsp.weak5_tmr[i] = 10;
            }

            new_inf.ex[i] = ene_wrk[i].fp[0];
            new_inf.ey[i] = ene_wrk[i].fp[1];
        }
        else
        {
            if (inf_dsp.weak5_tmr[i] != 0)
            {
                inf_dsp.weak5_tmr[i]--;
            }
            else
            {
                inf_dsp.weak5_tmr[i] = 0;
                new_inf.ex[i] = plyr_wrk.fp[0];
                new_inf.ey[i] = plyr_wrk.fp[1];
            }
        }
    }

    for (i = 0; i < 1; i++)
    {
        if (plyr_wrk.fhp[i][3] != 0.0f)
        {
            GetCamI2DPos(plyr_wrk.fhp[i], &new_inf.hx[i], &new_inf.hy[i]);

            if (inf_dsp.hint5_alp[i] + 8 < alp)
            {
                inf_dsp.hint5_alp[i] += 8;
            }
            else
            {
                inf_dsp.hint5_alp[i] = alp;
            }

            if (inf_dsp.hint5_tmr[i] < 10)
            {
                inf_dsp.hint5_tmr[i]++;
            }
            else
            {
                inf_dsp.hint5_tmr[i] = 10;
            }
        }
        else
        {
            if (alp < inf_dsp.hint5_alp[i])
            {
                inf_dsp.hint5_alp[i] = alp;
            }
            if (inf_dsp.hint5_alp[i] - 8 > 0)
            {
                inf_dsp.hint5_alp[i] -= 8;
            }
            else
            {
                inf_dsp.hint5_alp[i] = 0;
            }

            if (inf_dsp.hint5_tmr[i] != 0)
            {
                inf_dsp.hint5_tmr[i]--;
            }
            else
            {
                inf_dsp.hint5_tmr[i] = 0;

                new_inf.hx[i] = plyr_wrk.fp[0];
                new_inf.hy[i] = plyr_wrk.fp[1];
            }
        }

        if (plyr_wrk.mode != 1)
        {
            if (inf_dsp.hint5_alp[i] > alp)
            {
                inf_dsp.hint5_alp[i] = alp;
            }
        }
    }

    hnt_alp_num = 0;
    hnt_all_alp = 0;

    for (i = 0; i < 1; i++)
    {
        if (inf_dsp.hint5_alp[i] != 0)
        {
            hnt_all_alp += inf_dsp.hint5_alp[i];
            hnt_alp_num++;
        }
    }

    if (hnt_alp_num != 0)
    {
        chrg_max = (float)hnt_all_alp / (float)hnt_alp_num;
        chrg_alp = 120.0f - (chrg_max * 60) / 128.0f;

        FlashStarYW(flsh_chrg, 0x64, 0x32, chrg_alp, 0);

        chrg_alp = chrg_max * flsh_chrg[0].alpha / 100.0f;
        big_size = circle_radius_tbl[cam_custom_wrk.charge_range];

        DspBigCircle(91, pos_x, pos_y, chrg_alp, big_size, 1);
    }

    ray_alp_num = 0;

    if (pa_radius_set != 0)
    {
        siz = ene_wrk[plyr_wrk.near_ene_no].pa_radius + ene_wrk[plyr_wrk.near_ene_no].pa_radius;
    }
    else
    {
        siz = 64;
    }

    for (i = 0; i < 4; i++)
    {
        ex = ((new_inf.ex[i] - plyr_wrk.fp[0]) * (inf_dsp.weak5_tmr[i] / 10.0f)) +
            (((plyr_wrk.fp[0] - 0x140) * 8.5f) / 10.0f);

        ey = ((new_inf.ey[i] - plyr_wrk.fp[1]) * (inf_dsp.weak5_tmr[i] / 10.0f)) +
            (((plyr_wrk.fp[1] - 0xd1) * 8.5f) / 10.0f);

        if (inf_dsp.weak5_alp[i] != 0)
        {
            char dummy = ~dummy; // HACK: fixes diff

            PointerNP(ex, ey, 0, inf_dsp.weak5_alp[i], siz);

            ray_alp_num++;
        }
    }

    if (GetEnePowerDegree() == -1.0f)
    {
        if (plyr_wrk.charge_rate < 0.1f)
        {
            chrg_rate = 0.1f;
        }
        else if (plyr_wrk.charge_rate <= 3.0f)
        {
            chrg_rate = plyr_wrk.charge_rate;
        }
        else
        {
            chrg_rate = 3.0f;
        }

        chrg_max = alp * chrg_rate / 3.0f;
        chrg_alp = 120.0f - (chrg_rate * 60.0f) / 3.0f;

        FlashStarYW(&flsh_chrg[1], 0x64, 0x32, chrg_alp, 0);

        chrg_alp = chrg_max * flsh_chrg[1].alpha / 100.0f;
        big_size = circle_radius_tbl[cam_custom_wrk.charge_range];

        if (chrg_alp != 0)
        {
            DspBigCircle(91, pos_x, pos_y, chrg_alp, big_size, 1);
        }
    }

    if (ray_alp_num == 0)
    {
        PointerNP(pos_x, pos_y, 0, alp, siz);
    }
}

static void DspBigCircle(u_short lu_chr, short int pos_x, short int pos_y, u_char alp, short int size_r, u_char cl_ptn)
{
    int i;
    DISP_SPRT ds;

    for (i = lu_chr; i <= lu_chr + 3; i++)
    {
        CopySprDToSpr(&ds, &spr_dat[i]);

        ds.x += pos_x;
        ds.y += pos_y;

        switch (cl_ptn)
        {
        case 1:
            ds.r = 55;
            ds.g = 70;
            ds.b = 255;
        break;
        case 2:
            ds.r = 0;
            ds.g = 161;
            ds.b = 38;
        break;
        }

        ds.alphar = 72;
        ds.alpha  = alp;

        if (inf_dsp.lucky7 != 0)
        {
            ds.r = 254;
            ds.g = 91;
            ds.b = 7;
        }

        ds.scw = size_r / 110.0f;
        ds.sch = ds.scw;
        ds.csx = spr_dat[lu_chr + 3].x;
        ds.csy = spr_dat[lu_chr + 3].y;

        if (flip_table[i] == 1)
        {
            ds.att |= 1;
        }

        if (flip_table[i] == 2)
        {
            ds.att |= 2;
        }

        if (flip_table[i] == 3)
        {
            ds.att |= 3;
        }

        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
        ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);

        DispSprD(&ds);
    }
}

static void PointerNP(short int cx, short int cy, u_char red, u_char alp, float siz)
{
    PointerEN(cx, cy, red, alp * 0.5f, siz);

    if (red != 0)
    {
        SetEffects(32, 1, (float)cx, (float)cy, siz * 0.5f, alp, red, 0x80 - red, 0x80 - red);
    }
}

static void PointerEN(short int pos_x, short int pos_y, u_char red, u_char alp, float size)
{
    float scl;
    DISP_SPRT ds;

    scl = size / 64.0f;

    CopySprDToSpr(&ds, &spr_dat[96]);

    ds.x += pos_x;
    ds.y += pos_y;
    ds.scw = scl;
    ds.sch = scl;
    ds.csx = ds.x + (ds.w >> 1) + pos_x;
    ds.csy = ds.y + (ds.h >> 1) + pos_y;
    ds.r = 0x80;
    ds.g = 0x80 - red;
    ds.b = ds.g;
    ds.alpha = alp;
    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);

    DispSprD(&ds);
}

static void FilmZansu(int number, short int pos_x, short int pos_y, short int num_x, short int num_y, int digit)
{
    int i;
    int multi10;
    int multi10_bak;
    int dsp_num;
    u_char dsp_mode;
    int disp_digit;

    multi10 = 1;

    PutSpriteYW(FND_CONT_BASE, FND_CONT_BASE, pos_x, pos_y, 0.0f, 0x808080,
                inf_dsp.fndr_fade_alp * 128 / 100.0f, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    for (i = 0; i < digit; i++)
    {
        multi10 *= 10;
        multi10_bak = multi10;
    }

    number = number % multi10;

    if (inf_dsp.fndr_mode_tmr == 1)
    {
        info_wrk.film_bak = info_wrk.film_num;
    }

    if (info_wrk.film_num != info_wrk.film_bak)
    {
        inf_dsp.flm_cng_tmr = 80;

        info_wrk.film_bak = info_wrk.film_num;
    }

    disp_digit = 1;

    multi10 = multi10_bak;

    for (i = 0; i < digit; i++)
    {
        multi10 /=10;

        dsp_mode = 0;

        dsp_num = (number % (multi10 * 10)) / multi10;

        if (inf_dsp.flm_cng_tmr != 0)
        {
            if (inf_dsp.flm_cng_tmr < 21)
            {
                if (i != digit - disp_digit)
                {
                    dsp_mode = ((number / (multi10 / 10)) % 10 != 9) ? dsp_mode : disp_digit;
                }
                else
                {
                    dsp_mode = 1;
                }
            }
            else
            {
                dsp_num = ((number + 1) % (multi10 * 10)) / multi10;
            }
        }

        NumDisp(dsp_num, i, pos_x, pos_y, num_x, num_y, dsp_mode);
    }

    PutSpriteYW(FND_CONT_MASK, FND_CONT_MASK, pos_x, pos_y, 0.0f, 0x808080,
                inf_dsp.fndr_fade_alp * 128 / 100.0f, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
}

static void NumDisp(u_char number, u_char no, short int pos_x, short int pos_y, short int num_x, short int num_y, u_char mode)
{
    float alp;
    float slot_mov;
    int i;
    short int dsp_y;
    signed char dsp_v;
    signed char dsp_h;
    DISP_SPRT ds;
    u_char spr_no;
    short int tmp_y;

    alp = inf_dsp.fndr_fade_alp * 128 / 100.0f;

    if (mode == 0)
    {
        slot_mov = 0.0f;
    }
    else
    {
        slot_mov = spr_dat[3].h * inf_dsp.flm_cng_tmr / 20.0f;
    }

    for (i = 0; i < 4; i++)
    {
        spr_no = (number + 9 + i) % 10;

        CopySprDToSpr(&ds, &spr_dat[spr_no + 3]);

        tmp_y = pos_y + num_y + slot_mov + ds.h - ds.h * i;

        YVH_CUT(tmp_y, ds.h, pos_y + 365, pos_y + 423, &dsp_y, (signed char *)&dsp_v, (u_char *)&dsp_h);

        ds.x = pos_x + num_x + no * 29;
        ds.y = tmp_y + dsp_y;
        ds.h = dsp_h;
        ds.v += dsp_v;
        ds.alpha = alp;
        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

        DispSprD(&ds);
    }
}

void YVH_CUT(short int tmp_y, u_char tmp_h, short int up_lmt, short int dwn_lmt, short int *dsp_y, signed char *dsp_v, u_char *dsp_h)
{
    if (tmp_y + tmp_h < up_lmt || tmp_y > dwn_lmt)
    {
        *dsp_v = 0;
        *dsp_h = 0;
        *dsp_y = tmp_y;
    }
    else if (tmp_y < up_lmt)
    {
        *dsp_v = up_lmt - tmp_y;
        *dsp_h = tmp_h - *dsp_v;
        *dsp_y = *dsp_v;
    }
    else if (tmp_y + tmp_h > dwn_lmt)
    {
        *dsp_v = 0;
        *dsp_h = dwn_lmt - tmp_y;
        *dsp_y = 0;
    }
    else
    {
        *dsp_v = 0;
        *dsp_h = tmp_h;
        *dsp_y = 0;
    }
}

static void EdogawaLamp(short int pos_x, short int pos_y, u_char out)
{
    float chk;
    float pow;
    float lag_pow;
    float cng_pow;
    float cmn_pow;
    signed char pow_add;
    u_char f_max;
    float f;
    u_char sml_alp;
    u_char lag_alp;
    u_char cmn_alp;
    int i;
    u_char ene_num;
    u_char obj;
    float tmp; // HACK: fixes operand swap

    chk = GetEnePowerDegree();

    if (chk == -1.0f)
    {
        pow = 100.0f;
    }
    else
    {
        pow = chk;
    }

    if (pow - inf_dsp.rmp_pow > 0.0f)
    {
        if (pow - inf_dsp.rmp_pow < 4.0f)
        {
            pow_add = pow - inf_dsp.rmp_pow;
        }
        else
        {
            pow_add = 4;
        }
    }
    else
    {
        if (pow - inf_dsp.rmp_pow < 0.0f)
        {
            if (inf_dsp.rmp_pow - pow < 4.0f)
            {
                pow_add = -(inf_dsp.rmp_pow - pow);
            }
            else
            {
                pow_add = -4;
            }
        }
        else
        {
            pow_add = 0;
        }
    }

    if (pow_add > 0)
    {
        if (inf_dsp.rmp_pow + pow_add < pow)
        {
            inf_dsp.rmp_pow += pow_add;
        }
        else
        {
            inf_dsp.rmp_pow = pow;
        }
    }
    else if (pow_add < 0)
    {
        if (inf_dsp.rmp_pow + pow_add > pow)
        {
            inf_dsp.rmp_pow += pow_add;

        }
        else
        {
            inf_dsp.rmp_pow = pow;
        }
    }

    cng_pow = inf_dsp.rmp_pow;

    tmp = 4.0f; if (cng_pow < 4.0f) tmp = (u_char)cng_pow;  // why the cast? maybe f_max is involved?

    cmn_pow = tmp * (SgCosf((inf_dsp.rmp_f * (PI * 2)) / 120.0f) - 1.0f) * 0.5f;

    if (inf_dsp.rmp_f < 120)
    {
        inf_dsp.rmp_f++;
    }
    else
    {
        inf_dsp.rmp_f = 0;
    }

    if (chk == -1.0f)
    {
        cmn_pow = 0;
    }

    f = cng_pow + cmn_pow;

    if (f != 0.0f)
    {
        lag_pow = f;
    }
    else
    {
        lag_pow = 0.0f;
    }

    if (out == 0)
    {
        cmn_alp = (inf_dsp.fndr_fade_alp * 128) / 100.0f;

        new_inf.rmp_alp = 0;
    }
    else
    {
        if (new_inf.rmp_alp + 8 < 0x80)
        {
            new_inf.rmp_alp += 8;
        }
        else
        {
            new_inf.rmp_alp = 0x80;
        }

        cmn_alp = new_inf.rmp_alp;
    }

    sml_alp = cmn_alp * cng_pow / 100.0f;
    lag_alp = cmn_alp * lag_pow / 100.0f;

    ene_num = 0;

    for (i = 0; i < 4; i++)
    {
        if (ene_wrk[i].sta & 0x1)
        {
            ene_num++;
        }
    }

#ifdef MATCHING_DECOMP
    {
        register int i asm("v1");

        for (i = 4; i >= 0; i--) // HACK: fixes wrong register
        {
            // debug code?
        }
    }
#endif

    if (ene_num != 0)
    {
        obj = 2;
    }
    else
    {
        obj = 1;
    }

    if (chk == 0.0f)
    {
        obj = 0;
    }

    if (out != 0)
    {
        PutSpriteYW(FND_RMP_CV, FND_RMP_CV, 0.0f, 0.0f, 0.0f, 0x808080, cmn_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 1);
    }

    PutSpriteYW(FND_RMP_BASE, FND_RMP_BASE, pos_x, pos_y, 0.0f, 0x808080, cmn_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 1);

    switch (obj)
    {
    case 0:
         // do nothing ...
    break;
    case 1:
        PutSpriteYW(FND_RMP_BLU_SML, FND_RMP_BLU_SML, pos_x, pos_y, 0.0f, 0x808080, sml_alp, 1.0f, 1.0f, 0, 0xff, 1, 1, 1);
        PutSpriteYW(FND_RMP_BLU_SML, FND_RMP_BLU_SML, pos_x, pos_y, 0.0f, 0x808080, sml_alp, 1.0f, 1.0f, 0, 0xff, 1, 1, 1);
        PutSpriteYW(FND_RMP_BLU_BIG, FND_RMP_BLU_BIG, pos_x, pos_y, 0.0f, 0x808080, lag_alp, 1.0f, 1.0f, 0, 0xff, 1, 1, 1);
        PutSpriteYW(FND_RMP_BLU_BIG, FND_RMP_BLU_BIG, pos_x, pos_y, 0.0f, 0x808080, lag_alp, 1.0f, 1.0f, 0, 0xff, 1, 1, 1);
    break;
    case 2:
        PutSpriteYW(FND_RMP_RED_SML, FND_RMP_RED_SML, pos_x, pos_y, 0.0f, 0x808080, sml_alp, 1.0f, 1.0f, 0, 0xff, 1, 1, 1);
        PutSpriteYW(FND_RMP_RED_SML, FND_RMP_RED_SML, pos_x, pos_y, 0.0f, 0x808080, sml_alp, 1.0f, 1.0f, 0, 0xff, 1, 1, 1);
        PutSpriteYW(FND_RMP_RED_BIG, FND_RMP_RED_BIG, pos_x, pos_y, 0.0f, 0x808080, lag_alp, 1.0f, 1.0f, 0, 0xff, 1, 1, 1);
        PutSpriteYW(FND_RMP_RED_BIG, FND_RMP_RED_BIG, pos_x, pos_y, 0.0f, 0x808080, lag_alp, 1.0f, 1.0f, 0, 0xff, 1, 1, 1);
    break;
    }

    PutSpriteYW(FND_RMP_GRS, FND_RMP_GRS, pos_x, pos_y, 0.0f, 0x808080, (int)(cmn_alp / 2), 1.0f, 1.0f, 0, 0xff, 1, 1, 1);
}

static void NewFndrBase(short int pos_x, short int pos_y)
{
    u_char alp;

    alp = (inf_dsp.fndr_fade_alp * 128) / 100.0f;

    PutSpriteYW(FND_FRAME_L, FND_FRAME_R, pos_x, pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0x0, 0xff, 0x1, 0x0, 0x0);
    PutSpriteYW(FND_STK_GAUGE_L, FND_STK_GAUGE_R, pos_x, pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0x0, 0xff, 0x1, 0x0, 0x0);
    PutSpriteYW(FND_SEAL_GAUGE, FND_SEAL_GAUGE, pos_x, pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0x0, 0xff, 0x1, 0x0, 0x0);
}

static void BigBon(u_char bon_num, short int sx, short int sy, short int ex, short int ey, u_char mov_tim)
{
    int i;
    int j;
    float pr1;
    short int msx;
    short int msy;
    short int mex;
    short int mey;
    u_char mtm;
    u_char per;
    u_char alp;
    short pos_x;
    short pos_y;

    if (bon_num != new_inf.bon_bak)
    {
        if (new_inf.bon_bak < bon_num)
        {
            if (new_inf.bon_mov_cnt[bon_num] == 0)
            {
                new_inf.bon_mov_cnt[bon_num] = mov_tim;
                new_inf.bon_mov_sx[bon_num] = sx;
                new_inf.bon_mov_sy[bon_num] = sy;
                new_inf.bon_mov_ex[bon_num] = ex;
                new_inf.bon_mov_ey[bon_num] = ey;
                new_inf.bon_mov_tm[bon_num] = mov_tim;

                ZanzoLS(znz[bon_num]);
            }
        }

        new_inf.bon_bak = bon_num;
    }

    for (i = 0; i < 12; i++)
    {
        if (new_inf.bon_mov_cnt[i] != 0)
        {
            msx = new_inf.bon_mov_sx[i];
            msy = new_inf.bon_mov_sy[i];
            mex = new_inf.bon_mov_ex[i];
            mey = new_inf.bon_mov_ey[i];
            mtm = new_inf.bon_mov_tm[i];

            per = (new_inf.bon_mov_cnt[i] * 100) / (float)mtm;

            ZanzoLM(znz[i], per);

            new_inf.bon_mov_cnt[i]--;

            for (j = 0; j < 6; j++)
            {
                if (znz[i][j] != 0xff)
                {
                    pr1 = znz[i][j] / 100.0f;

                    alp = SgSinf(pr1) * 320.0f * SgSinf(pr1) * (25 - j * j) / 25.0f;

                    pos_x = (msx - mex) * pr1 * pr1 + mex;
                    pos_y = (msy - mey) * pr1 * pr1 + mey;

                    PutSpriteYW(FND_BIG_NUM_00 + i, FND_BIG_NUM_00 + i, pos_x, pos_y, 0.0f, 0x808080, alp / 10.0f, 3.0f, 3.0f, 0, 0xff, 0, 1, 1);
                }
            }
        }
    }
}

static void BigBon2(u_char bon_num, short int sx, short int sy, short int ex, short int ey, u_char mov_tim)
{
    int i;
    int j;
    u_char alp;
    float spd;
    short int msx;
    short int msy;
    short int mex;
    short int mey;
    u_char mtm;
    u_char per;
    short pos_x;
    short pos_y;
    float pr1;
    float rad;

    if (bon_num != new_inf.bon_bak)
    {
        if (new_inf.bon_bak < bon_num)
        {
            if (new_inf.bon_mov_cnt[bon_num] == 0)
            {
                new_inf.bon_mov_cnt[bon_num] = mov_tim;
                new_inf.bon_mov_sx[bon_num] = sx;
                new_inf.bon_mov_sy[bon_num] = sy;
                new_inf.bon_mov_ex[bon_num] = ex;
                new_inf.bon_mov_ey[bon_num] = ey;
                new_inf.bon_mov_tm[bon_num] = mov_tim;

                ZanzoLS(znz[bon_num]);
            }
        }

        new_inf.bon_bak = bon_num;
    }

    for (i = 0; i < 12; i++)
    {
        if (new_inf.bon_mov_cnt[i] != 0)
        {
            msx = new_inf.bon_mov_sx[i];
            msy = new_inf.bon_mov_sy[i];
            mex = new_inf.bon_mov_ex[i];
            mey = new_inf.bon_mov_ey[i];
            mtm = new_inf.bon_mov_tm[i];

            per = (new_inf.bon_mov_cnt[i] * 100) / (float)mtm;

            ZanzoLM(znz[i], per);

            new_inf.bon_mov_cnt[i]--;

            for (j = 0; j < 3; j++)
            {
                if (znz[i][j] != 0xff)
                {
                    rad = znz[i][j] * PI / 100.0f;
                    alp = SgSinf(rad) * 160.0f * (25 - j * j) / 25.0f;

                    if (znz[i][j] <= 70)
                    {
                        spd = (70 - znz[i][j]) / 70.0f;
                    }
                    else
                    {
                        spd = 0.0f;
                    }

                    pos_x = (mex - msx) * spd * spd + msx;
                    pos_y = (mey - msy) * spd * spd + msy;

                    PutSpriteYW(FND_BIG_NUM_00 + i, FND_BIG_NUM_00 + i, pos_x,pos_y, 0.0f, 0x808080, alp / 10.0f, 3.0f , 3.0f, 0, 0xff, 0, 1, 1);
                }
            }
        }
    }
}

static char GetEnePoint()
{
    int i;

    if (new_inf.pht_cnt_bak == ingame_wrk.pht_cnt || photo_wrk.score == 0)
    {
        return 0;
    }

    new_inf.pht_cnt_bak = ingame_wrk.pht_cnt;
    new_inf.pht_scr = photo_wrk.score;

    for (i = 0; i < 4; i++)
    {
        new_inf.ene_no[i] = 0xffff;
        new_inf.ene_dmg[i] = 0xffff;
    }

    new_inf.hit_hp_ovr_ene = 60;
    new_inf.hit_hp_ovr_ene_wait = 0;
    new_inf.dsp_dmg_flg = 0;
    new_inf.dsp_dmg_num = 0;
    new_inf.dsp_dmg_step = 0;
    new_inf.dsp_dmg_step2 = 0;

    for (i = 0; i < 4; i++)
    {
        if (photo_wrk.ene[i].no != 0xffff && photo_wrk.ene[i].dmg != 0)
        {
            new_inf.ene_no[new_inf.dsp_dmg_flg] = photo_wrk.ene[i].wrk_no;
            new_inf.ene_dmg[new_inf.dsp_dmg_flg] = photo_wrk.ene[i].dmg;
            new_inf.dsp_dmg_flg++;
        }
    }

    if (new_inf.dsp_dmg_flg != 0)
    {
        new_inf.dsp_dmg_num = new_inf.dsp_dmg_flg;
        new_inf.dsp_dmg_step = 1;
        new_inf.dsp_dmg_step2 = 1;
    }

    return 1;
}

static void MafubaInit(float t2_s1, float t2_s2)
{
    new_inf.mfb_drain = 0;
    new_inf.mfb_wait = 0;
    new_inf.mfb_effect = 0;
    new_inf.se_wait = 0;
    new_inf.se_num = 0;
    new_inf.mfb_lgt_alp = 0;
    new_inf.mfb_lfl_alp = 0;
    new_inf.mfb_num_alp = 0;
    new_inf.mfb_nfl_alp = 0;
    new_inf.mfb_num_siz_x = t2_s1 * 10.0f;
    new_inf.ene_dead_tmr = 0;
    new_inf.ene_dead_alp = 0;
    new_inf.mfb_stts = 0;
    new_inf.mfb_num_siz_y = new_inf.mfb_num_siz_x;

    ZanzoSS(&dmg_scr);

    new_inf.shot_mov = 0;
    new_inf.shot_pse = 0;
    new_inf.shot_lgt = 0;
    new_inf.shot_fad = 0;
}

static u_char Mafuba(short int pos_x, short int pos_y, u_char stts, u_char t1, u_char t2, u_char t3, u_char t4, u_char t5, u_char t1_sw, float t2_s1, float t2_s2)
{
    int j;
    u_char alp_max;

    alp_max = (inf_dsp.fndr_fade_alp * 128) / 100.0f;

    if (GetEnePoint() != 0)
    {
        MafubaInit(t2_s1, t2_s2);
    }

    switch (stts)
    {
    case 1:
        if (new_inf.mfb_stts == 0x0)
        {
            new_inf.mfb_stts = stts;
        }
    break;
    case 2:
        if (new_inf.mfb_stts == 0x1)
        {
            DameDameInit();

            new_inf.mfb_lgt_bak = new_inf.mfb_lgt_alp;
            new_inf.mfb_stts = stts;
            new_inf.mfb_effect = t2;
        }

        if (new_inf.mfb_stts == (0x4 | 0x2))
        {
            new_inf.mfb_stts = 0x0;
        }
    break;
    default:
        new_inf.mfb_stts = 0x0;
    break;
    }

    switch(new_inf.mfb_stts)
    {
    case 1:
        SndEffLoop();
        if (t1_sw != 0)
        {
            if (new_inf.mfb_drain < t1)
            {
                new_inf.mfb_drain++;
            }
            else {
                new_inf.mfb_drain = t1;
            }
        }
        else
        {
            if (new_inf.mfb_drain > 0)
            {
                new_inf.mfb_drain--;
            }
            else {
                new_inf.mfb_drain = 0;
            }
        }

        new_inf.mfb_lgt_alp = alp_max * (new_inf.mfb_drain / (float)t1);
        new_inf.mfb_lfl_alp = new_inf.mfb_lgt_alp;
    break;
    case 2:
        if (new_inf.mfb_drain > 0)
        {
            new_inf.mfb_drain--;
        }

        new_inf.mfb_lfl_alp = (alp_max * new_inf.mfb_drain) / (float)t1;

        if (new_inf.mfb_effect > 0)
        {
            new_inf.mfb_effect--;
        }
        else
        {
            new_inf.mfb_stts = 0x2 | 0x1;
            new_inf.mfb_wait = t3;
        }

        new_inf.mfb_lgt_alp = (new_inf.mfb_lgt_bak * new_inf.mfb_effect) / (float)t2;
        new_inf.mfb_num_alp = (alp_max * (t2 - new_inf.mfb_effect)) / (float)t2;
        new_inf.mfb_num_siz_x = new_inf.mfb_num_siz_y = ((t2_s2 - (t2_s2 - t2_s1) * (new_inf.mfb_effect / (float)t2) * (new_inf.mfb_effect / (float)t2)) * 10.0f);
    break;
    case 3:
        if (new_inf.mfb_wait != 0)
        {
            new_inf.mfb_wait--;
        }
        else
        {
            new_inf.mfb_stts = 0x4;
            new_inf.mfb_effect = t4;
        }
    break;
    case 4:
        if (new_inf.mfb_effect > 0)
        {
            new_inf.mfb_effect--;
        }
        else
        {
            new_inf.mfb_stts = 0x4 | 0x1;
            new_inf.mfb_drain = t5;
        }

        new_inf.mfb_nfl_alp = alp_max * SgSinf(new_inf.mfb_effect * PI / t4);
    break;
    case 5:
        if (new_inf.mfb_drain > 0)
        {
            new_inf.mfb_drain = new_inf.mfb_drain + -1;
        }
        else
        {
            new_inf.mfb_stts = 0x4 | 0x2;
            point_get_end = 1;
        }
        new_inf.mfb_num_alp = (alp_max * new_inf.mfb_drain) / (float)t5;
    break;
    }

    if (new_inf.mfb_lgt_alp != 0)
    {
        PutSpriteYW(FND_SEAL_FNT, FND_SEAL_FNT, pos_x, pos_y, 0.0f, 0x808080, new_inf.mfb_lgt_alp, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
    }

    if (new_inf.mfb_lfl_alp != 0)
    {
        PutSpriteYW(FND_SEAL_LIT_FLA, FND_SEAL_LIT_FLA, pos_x - 5, pos_y+0x10f, 0.0f, 0x808080, new_inf.mfb_lfl_alp * 0.5f, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
    }

    ZanzoSM(&dmg_scr, new_inf.mfb_stts, new_inf.mfb_num_alp, new_inf.mfb_num_siz_x, new_inf.mfb_num_siz_y, 1);

    for (j = 0; j < 6; j++)
    {
        if (dmg_scr.num2[j] != 0xff)
        {
            DameEffNumber(1, new_inf.pht_scr, pos_x, pos_y, dmg_scr.num1[j], (dmg_scr.num2[j] * (6 - j)) / 6.0f,  dmg_scr.num3[j],  dmg_scr.num4[j]);
        }
    }

    if (new_inf.mfb_nfl_alp != 0)
    {
        PutSpriteYW(FND_DM_FNT_FLA, FND_DM_FNT_FLA, pos_x, pos_y, 0.0f, 0x808080, new_inf.mfb_nfl_alp * 0.5f, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
    }

    return new_inf.mfb_stts;
}

static void MafubaFinish(short int pos_x, short int pos_y, u_char start, u_char time)
{
    u_char alp_max;

    alp_max = (inf_dsp.fndr_fade_alp * 128) / 100.0f;

    if (start != 0)
    {
        new_inf.ene_dead_tmr = time;
    }
    else if (new_inf.ene_dead_tmr > 0)
    {
        new_inf.ene_dead_tmr--;
    }

    if (new_inf.ene_dead_tmr != 0)
    {
        if (new_inf.ene_dead_alp + 16 < alp_max)
        {
            new_inf.ene_dead_alp += 16;
        }
        else
        {
            new_inf.ene_dead_alp = alp_max;
        }
    }
    else
    {
        if (new_inf.ene_dead_alp - 16 > 0)
        {
            new_inf.ene_dead_alp -= 16;
        }
        else
        {
            new_inf.ene_dead_alp = 0x0;
        }
    }

    if (new_inf.ene_dead_alp != 0)
    {
        PutSpriteYW(FND_SEAL_FNT, FND_SEAL_FNT, pos_x, pos_y, 0.0f, 0x808080, new_inf.ene_dead_alp, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
        PutSpriteYW(FND_SEAL_LIT_FLA, FND_SEAL_LIT_FLA, pos_x - 5, pos_y + 271, 0.0f, 0x808080, new_inf.ene_dead_alp * 0.5f, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
    }
}

static void DameEffNumber(u_char type, u_short num, short int pos_x, short int pos_y, u_char stts, u_char num_alp, u_char num_siz_x, u_char num_siz_y)
{
    u_char keta;
    u_char adj;
    float temp;

    if (num_siz_x == 0xff || num_siz_y == 0xff)
    {
        return;
    }

    keta = 1;

    temp = num;

    while (10.0f <= temp)
    {
        temp /= 10.0f;

        keta++;
    }

    if (num_alp != 0)
    {
        if (type == 0)
        {
            adj = (keta * 18) * 0.5f;

            PutNumberYW(13, num, pos_x - (adj - 135), pos_y + 92, num_siz_x / 10.0f, num_siz_y / 10.0f, 0x808080, num_alp, -1, keta, 0);
        }
        else
        {
            adj = (keta * 15) * 0.5f;

            PutNumberYW(7, num, pos_x - (adj - 59), pos_y + 327, num_siz_x / 10.0f, num_siz_y / 10.0f, 0x808080, num_alp, -1, keta, 0);
            PutSpriteYW(FND_FNT_SPIRIT, FND_FNT_SPIRIT, pos_x, pos_y, 0.0f, 0x808080, num_alp, num_siz_x / 10.0f, num_siz_y / 10.0f, 0, 0xff, 1, 0, 0);
        }
    }
}

static void SndEffLoop()
{
    if (new_inf.se_wait != 0x0)
    {
        new_inf.se_wait--;
    }
    else
    {
        if (new_inf.se_num < 5)
        {
            SeStartFix(17, 0, 0x1000, 0x1000, 0);
        }

        new_inf.se_wait = 5;
        new_inf.se_num++;
    }
}

static void ShowEneCtrl(short int pos_x, short int pos_y)
{
    u_char ene_no;
    u_char alp_max;
    u_short ene_hp;
    u_char ep_bar;

    ene_no = plyr_wrk.near_ene_no;

    alp_max = (inf_dsp.fndr_fade_alp * 128) / 100.0f;

    if (ene_no != 0xff)
    {
        if (ene_wrk[ene_no].hp <= 500)
        {
            ene_hp = ene_wrk[ene_no].hp;

            new_inf.bar_over = 0;
        }
        else
        {
            ene_hp = 500;

            new_inf.bar_over = ene_wrk[ene_no].hp + 12;
        }

        ep_bar = (ene_hp * 100) / 500.0f;
        new_inf.ep_bar = (ep_bar * 153) / 100.0f;

        new_inf.ep_tmr = 180;

        if (new_inf.ep_alp[0] + 8 < alp_max)
        {
            new_inf.ep_alp[0] += 8;
        }
        else
        {
            new_inf.ep_alp[0] = alp_max;
        }
    }
    else
    {
        if (new_inf.ep_tmr != 0)
        {
            new_inf.ep_tmr--;
        }
        else
        {
            if (new_inf.ep_alp[0] - 8 > 0)
            {
                new_inf.ep_alp[0] -= 8;
            }
            else
            {
                new_inf.ep_alp[0] = 0;
            }
        }
    }

    if (new_inf.dsp_dmg_flg != 0)
    {
        switch (EneDamegeCtrl(pos_x, pos_y))
        {
        case 0:
        default:
            return;
        break;
        case 1:
            // do nothing ...
        break;
        case 2:
            new_inf.ep_alp[0] = 0x0;
            new_inf.ep_bar = 0x0;
        break;
        }
    }

    ShowEnePower(new_inf.ep_bar, new_inf.ep_bar, pos_x, pos_y, new_inf.ep_alp[0], new_inf.bar_over);
}

static char EneDamegeCtrl(short int pos_x, short int pos_y)
{
    u_char alp_max;
    u_char ep_bar1;
    u_char ep_bar2;
    u_char dsp_no;
    u_short ene_no;
    u_short ene_dmg;
    u_short bar_red;
    u_short bar_blu;
    u_char hp_perc_blu;
    u_char hp_perc_red;

    alp_max = (inf_dsp.fndr_fade_alp * 128) / 100.0f;

    dsp_no = new_inf.dsp_dmg_num - new_inf.dsp_dmg_flg;
    ene_no = new_inf.ene_no[dsp_no];
    ene_dmg = new_inf.ene_dmg[dsp_no];

    switch(new_inf.dsp_dmg_step)
    {
    case 1:
        new_inf.dsp_dmg_alp = 0;
        new_inf.dsp_dmg_red = ene_dmg + ene_wrk[ene_no].hp;
        new_inf.dsp_dmg_blu = ene_wrk[ene_no].hp;
        new_inf.dsp_dmg_step = 2;
        new_inf.dsp_dmg_step2 = 1;
    break;
    case 2:
        if (new_inf.dsp_dmg_alp + 8 < alp_max)
        {
            new_inf.dsp_dmg_alp += 8;
        }
        else
        {
            new_inf.dsp_dmg_alp = alp_max;
            new_inf.dsp_dmg_step = 3;
        }
    break;
    case 3:
        if (new_inf.dsp_dmg_blu < new_inf.dsp_dmg_red)
        {
            new_inf.dsp_dmg_red--;
        }
        else if (new_inf.dsp_dmg_step2 == 7)
        {
            new_inf.dsp_dmg_red = new_inf.dsp_dmg_blu;
            new_inf.dsp_dmg_step2 = 0;

            if (new_inf.dsp_dmg_flg == 1)
            {
                if (plyr_wrk.near_ene_no == 0xff)
                {
                    new_inf.dsp_dmg_step = 6;
                }
                else if (ene_no == plyr_wrk.near_ene_no)
                {
                    new_inf.dsp_dmg_step = 5;
                }
            }
            else
            {
                new_inf.dsp_dmg_step = 4;
            }
        }
    break;
    case 4:
        if (new_inf.dsp_dmg_alp - 8 > 0)
        {
            new_inf.dsp_dmg_alp -= 8;
        }
        else
        {
            new_inf.dsp_dmg_alp = 0;
            new_inf.dsp_dmg_step = 5;
        }
    break;
    case 5:
        if (new_inf.dsp_dmg_flg != 0)
        {
            new_inf.dsp_dmg_flg--;
        }
        if (new_inf.dsp_dmg_flg != 0)
        {
            new_inf.dsp_dmg_step = 1;
        }
        else
        {
            new_inf.dsp_dmg_alp = 0;
            new_inf.dsp_dmg_step = 0;

            return 1;
        }
    break;
    case 6:
        if (new_inf.dsp_dmg_alp - 8 > 0)
        {
            new_inf.dsp_dmg_alp -= 8;
        }
        else
        {
            new_inf.dsp_dmg_alp = 0;
            new_inf.dsp_dmg_step = 7;
        }
    break;
    case 7:
        if (new_inf.dsp_dmg_flg != 0)
        {
            new_inf.dsp_dmg_flg--;
        }

        if (new_inf.dsp_dmg_flg != 0)
        {
            new_inf.dsp_dmg_step = 1;
        }
        else
        {
            new_inf.dsp_dmg_alp = 0;
            new_inf.dsp_dmg_step = 0;

            return 2;
        }
    break;
    }

    if (new_inf.dsp_dmg_blu > 500)
    {
        bar_blu = 500;

        new_inf.bar_over = new_inf.dsp_dmg_blu + 12;
    }
    else
    {
        bar_blu = new_inf.dsp_dmg_blu;

        new_inf.bar_over = 0;
    }

    hp_perc_blu = (bar_blu * 100) / 500.0f;
    ep_bar1 = (hp_perc_blu * 153) / 100.0f;

    if (new_inf.dsp_dmg_red > 500)
    {
        bar_red = 500;
    }
    else
    {
        bar_red = new_inf.dsp_dmg_red;
    }

    hp_perc_red = (bar_red * 100) / 500.0f;
    ep_bar2 = (hp_perc_red * 153) / 100.0f;

    ShowEnePower(ep_bar1, ep_bar2, pos_x, pos_y, new_inf.dsp_dmg_alp, new_inf.bar_over);

    switch(new_inf.dsp_dmg_step2)
    {
    case 1:
        new_inf.dmg_num_mov_tim = 20;
        new_inf.dmg_str_mov_tim = 20;
        new_inf.dmg_num_mov = 100;
        new_inf.dmg_num_alp = 0;
        new_inf.dmg_str_mov = 100;
        new_inf.dmg_str_alp = 0;
        new_inf.dmg_eff_alp = 0;
        new_inf.dmg_wait_tm = 0;
        new_inf.dsp_dmg_step2 = 2;
    break;
    case 2:
        if (new_inf.dmg_num_mov_tim >= 16)
        {
            new_inf.dmg_num_mov_tim--;
        }
        else
        {
            new_inf.dsp_dmg_step2 = 3;
        }

        new_inf.dmg_num_mov =  100.0f * (new_inf.dmg_num_mov_tim / 20.0f) * (new_inf.dmg_num_mov_tim / 20.0f);
        new_inf.dmg_num_alp = alp_max * (1.0f - new_inf.dmg_num_mov_tim / 20.0f) * (1.0f - new_inf.dmg_num_mov_tim / 20.0f);
    break;

    case 3:
        if (new_inf.dmg_num_mov_tim != 0)
        {
            new_inf.dmg_num_mov_tim--;
        }

        if (new_inf.dmg_str_mov_tim != 0)
        {
            new_inf.dmg_str_mov_tim--;
        }

        if (new_inf.dmg_num_mov_tim == 0 && new_inf.dmg_str_mov_tim == 0 )
        {
            new_inf.dmg_wait_tm = 10;
            new_inf.dsp_dmg_step2 = 4;
        }

        new_inf.dmg_num_mov = 100.0f * (new_inf.dmg_num_mov_tim / 20.0f) * (new_inf.dmg_num_mov_tim / 20.0f);
        new_inf.dmg_num_alp = alp_max * (1.0f - new_inf.dmg_num_mov_tim / 20.0f) * (1.0f - new_inf.dmg_num_mov_tim / 20.0f);
        new_inf.dmg_str_mov = 100.0f * (new_inf.dmg_str_mov_tim / 20.0f) * (new_inf.dmg_str_mov_tim / 20.0f);
        new_inf.dmg_str_alp = alp_max * (1.0f - new_inf.dmg_str_mov_tim / 20.0f) * (1.0f - new_inf.dmg_str_mov_tim / 20.0f);
    break;
    case 4:
        if (new_inf.dmg_wait_tm != 0)
        {
            new_inf.dmg_wait_tm--;
        }
        else
        {
            new_inf.dmg_eff_tim = 10;
            new_inf.dsp_dmg_step2 = 5;
        }
    break;
    case 5:
        if (new_inf.dmg_eff_tim != 0)
        {
            new_inf.dmg_eff_tim--;
        }
        else
        {
            new_inf.dsp_dmg_step2 = 6;
        }

        new_inf.dmg_eff_alp = alp_max * SgSinf((new_inf.dmg_eff_tim * PI) / 10.0f);
    break;
    case 6:
        if (new_inf.dmg_num_alp - 8 > 0)
        {
            new_inf.dmg_num_alp -= 8;
        }
        else
        {
            new_inf.dmg_num_alp = 0;
        }

        if (new_inf.dmg_str_alp - 8 > 0)
        {
            new_inf.dmg_str_alp -= 8;
        }
        else
        {
            new_inf.dmg_str_alp = 0;
        }

        if (new_inf.dmg_num_alp == 0 && new_inf.dmg_str_alp == 0)
        {
            new_inf.dsp_dmg_step2 = 7;
        }
    }

    if (new_inf.dmg_num_alp != 0)
    {
        DameEffNumber(0, new_inf.ene_dmg[dsp_no], pos_x + new_inf.dmg_num_mov, pos_y, 0, new_inf.dmg_num_alp, 10, 10);
    }

    if (new_inf.dmg_str_alp != 0)
    {
        PutSpriteYW(FND_DM_FNT_DAMAGE, FND_DM_FNT_DAMAGE, pos_x + new_inf.dmg_str_mov, pos_y, 0.0f, 0x808080, new_inf.dmg_str_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }

    if (new_inf.dmg_eff_alp != 0)
    {
        DameEffNumber(0, new_inf.ene_dmg[dsp_no], pos_x, pos_y, 0, new_inf.dmg_eff_alp, 10, 10);

        PutSpriteYW(FND_DM_FNT_DAMAGE, FND_DM_FNT_DAMAGE, pos_x, pos_y, 0.0f, 0x808080, new_inf.dmg_eff_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }

    return 0;
}

static void ShowEnePower(u_char blu_bar, u_char red_bar, short int pos_x, short int pos_y, u_char alp, u_char bar_over)
{
    float rgb_per;
    float alp_per;
    int ovr_ene_rgb;
    u_char red0;
    u_char alp0;

    if (alp != 0)
    {
        PutSpriteYW(65, 66, pos_x, pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

#ifdef BUILD_EU_VERSION
        GayBar(143, pos_x, pos_y, alp, red_bar / 16.0f);
#else
        GayBar(147, pos_x, pos_y, alp, red_bar / 16.0f);
#endif
        GayBar(67, pos_x, pos_y, alp, blu_bar / 16.0f);

        if (bar_over & 0xFF)
        {
            rgb_per = (new_inf.hit_hp_ovr_ene / 60.0f);
            red0 = SgSinf(rgb_per * PI) * 32.0f;

            CLRtoRGB(&ovr_ene_rgb, red0 + 0x63, 0x4d, 0x43);

            alp_per = bar_over / 300.0f;
            alp0 = alp * alp_per * 0.5f + alp * 0.5f;

            PutSpriteYW(68, 68, pos_x, pos_y, 0.0f, ovr_ene_rgb, alp0, 9.5625f, 1.0f, 0, 0xff, 1, 0, 0);

            if (new_inf.hit_hp_ovr_ene_wait != 0)
            {
                new_inf.hit_hp_ovr_ene_wait--;
            }
            else if (new_inf.hit_hp_ovr_ene != 0)
            {
                new_inf.hit_hp_ovr_ene--;
            }
        }
        else
        {
            new_inf.hit_hp_ovr_ene = 0;
            new_inf.hit_hp_ovr_ene_wait = 0;
        }

        Koutakun(blu_bar, pos_x, pos_y, alp);
    }
}

static void Koutakun(u_char blu_bar, short int pos_x, short int pos_y, u_char alp)
{
    short int dsp_u;
    short int dsp_w;
    short int dsp_x;

    switch (new_inf.liner_w)
    {
    case 0:
        if (new_inf.liner_x + 1 < blu_bar + 128)
        {
            new_inf.liner_x++;
        }
        else
        {
            new_inf.liner_x = 0;
            new_inf.liner_w = 10;
        }
    break;
    default:
        if (new_inf.liner_w != 0)
        {
            new_inf.liner_w--;
        }
    break;
    }

    if (new_inf.liner_x < blu_bar)
    {
        dsp_u = 128 - new_inf.liner_x;
        dsp_w = new_inf.liner_x;
        dsp_x = 128;
    }
    else if (new_inf.liner_x < 128)
    {
        dsp_u = 128 - new_inf.liner_x;
        dsp_w = blu_bar;
        dsp_x = 128;
    }
    else if (new_inf.liner_x < blu_bar + 128)
    {
        dsp_u = 0;
        dsp_w = blu_bar - (new_inf.liner_x - 128);
        dsp_x = new_inf.liner_x;
    }

    DspKotaku(69, pos_x, pos_y, alp, dsp_u, dsp_w, dsp_x);
}

static void ShowMikuPower(short int pos_x, short int pos_y)
{
    u_char alp;
    u_short bar_h1;
    u_short bar_h2;

    alp = (inf_dsp.stts_fade_alp * 128) / 100.0f;

    bar_h1 = 284 * plyr_wrk.hp / 500.0f; // pixel value in range [0, 284]
    bar_h2 = 284 - bar_h1; // the rest of the 284 pixels

    CmnWindow(32, pos_x, pos_y, alp, 0x80);

    PolySquareYW(pos_x + 594,      0 + 140 + pos_y, 10, bar_h2, 0x991e2d, alp, 1.0f, 1.0f, 0x3000, 0, 0, 0);
    PolySquareYW(pos_x + 594, bar_h2 + 140 + pos_y, 10, bar_h1, 0x91bef0, alp, 1.0f, 1.0f, 0x3000, 0, 0, 0);
}

static void ShowMikuPower2(short int pos_x, short int pos_y)
{
    static FLSH_CORE flsh_life;
    u_char alp;
    u_short blu_bar;
    float scl_y;

    alp = (inf_dsp.stts_fade_alp * 128) / 100.0f;

    PutSpriteYW(LIFEBAR_WAKU_1, LIFEBAR_WAKU_5, pos_x, pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 1);

    blu_bar = (plyr_wrk.hp * 243) / 500.0f;

    if (new_inf.blu_bak == blu_bar)
    {
        if (blu_bar != new_inf.red_bar)
        {
            if (blu_bar < new_inf.red_bar - 1)
            {
                new_inf.red_bar--;
            }
            else
            {
                new_inf.red_bar = blu_bar;
            }
        }

        if (new_inf.fire_mask != 0)
        {
            new_inf.fire_mask--;
        }
    }
    else
    {
        if (new_inf.fire_mask < 7)
        {
            new_inf.fire_mask++;
        }

        new_inf.blu_bak = blu_bar;
    }

    GayBar2(110, 113, pos_x, pos_y - (short)(new_inf.red_bar - 243), (alp * 4) / 5, new_inf.red_bar / 243.0f);

    if (new_inf.fire_mask == 7)
    {
#ifdef BUILD_EU_VERSION
        JetCD(jet1, 20,  5, 10, 6, 141, 61, pos_x, pos_y - (short)(blu_bar - 243), pos_x + 596, pos_y - (short)(blu_bar - 243) + 174, alp);
        JetCD(jet2, 20, 24,  2, 1, 140, 62, pos_x, pos_y - (short)(blu_bar - 243), pos_x + 596, pos_y - (short)(blu_bar - 243) + 174, alp);
#else
        JetCD(jet1, 20,  5, 10, 6, 145, 61, pos_x, pos_y - (short)(blu_bar - 243), pos_x + 596, pos_y - (short)(blu_bar - 243) + 174, alp);
        JetCD(jet2, 20, 24,  2, 1, 144, 62, pos_x, pos_y - (short)(blu_bar - 243), pos_x + 596, pos_y - (short)(blu_bar - 243) + 174, alp);
#endif
    }

    GayBar2(106, 109, pos_x, pos_y - (short)(blu_bar - 243), (alp * 4) / 5, blu_bar / 243.0f);

    if (new_inf.fire_mask != 0)
    {
#ifdef BUILD_EU_VERSION
        SyonenJet(142, 35, pos_x, pos_y - (short)(blu_bar - 243), pos_x + 596, pos_y - (short)(blu_bar - 243) + 174, alp, 1.0f, new_inf.fire_mask / 10.0f);
#else
        SyonenJet(146, 35, pos_x, pos_y - (short)(blu_bar - 243), pos_x + 596, pos_y - (short)(blu_bar - 243) + 174, alp, 1.0f, new_inf.fire_mask / 10.0f);
#endif
    }

    if (plyr_wrk.hp == 500)
    {
        FlashStarYW(&flsh_life, alp, alp / 2, 120, 0);

        PutSpriteYW(LIFEBAR_WAKU_FLA1, LIFEBAR_WAKU_FLA1, pos_x, pos_y, 0.0f, 0x808080, flsh_life.alpha, 1.0f,  1.0f, 0, 0xff, 1, 1, 1);
        PutSpriteYW(LIFEBAR_WAKU_FLA2, LIFEBAR_WAKU_FLA2, pos_x, pos_y, 0.0f, 0x808080, flsh_life.alpha, 1.0f, 27.5f, 0, 0xff, 1, 1, 1);
        PutSpriteYW(LIFEBAR_WAKU_FLA3, LIFEBAR_WAKU_FLA3, pos_x, pos_y, 0.0f, 0x808080, flsh_life.alpha, 1.0f,  1.0f, 0, 0xff, 1, 1, 1);

        return;
    }

    if (blu_bar < new_inf.red_bar)
    {
        return;
    }

    switch (new_inf.liner_wait)
    {
    case 0:
        if (new_inf.liner_y + 4 < blu_bar)
        {
            new_inf.liner_y += 4;
        }
        else
        {
            new_inf.liner_y = 0;
            new_inf.liner_wait = 10;
        }
    break;
    default:
        if (new_inf.liner_wait != 0)
        {
            new_inf.liner_wait--;
        }
    break;
    }

    if (blu_bar == 0)
    {
        return;
    }

    scl_y = blu_bar * SgSinf((new_inf.liner_y * PI) / blu_bar) / 200.0f;

    PutSpriteYW(LIFEBAR_WAKU_FLA4, LIFEBAR_WAKU_FLA4, pos_x, pos_y - new_inf.liner_y, 0.0f, 0x808080, (int)(alp / 2), 4.0f / 23.0f, scl_y, 0, 0xff, 1, 0, 1);
}

static void DspKotaku(u_short chr, short int pos_x, short int pos_y, u_char alp, u_char dsp_u, u_char dsp_w, u_short dsp_x)
{
    DISP_SPRT ds;

    CopySprDToSpr(&ds, &spr_dat[chr]);

    ds.x += pos_x;
    ds.x += dsp_x;

    ds.y += pos_y;
    ds.u += dsp_u;
    ds.w = dsp_w;
    ds.alpha = alp * 0.5f;
    ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);

    DispSprD(&ds);
}

static void GayBar(u_short chr, short int pos_x, short int pos_y, u_char alp, float scl)
{
    DISP_SPRT ds;

    CopySprDToSpr(&ds, &spr_dat[chr]);

    ds.x += pos_x;
    ds.y += pos_y;
    ds.scw = scl;
    ds.sch = 1.0f;
    ds.csx = spr_dat[chr].x + pos_x;
    ds.csy = spr_dat[chr].y + pos_y;
    ds.alpha = alp;
    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);
}

static void GayBar2(u_short top_chr, u_short end_chr, short int pos_x, short int pos_y, u_char alp, float scl)
{
    int i;
    DISP_SPRT ds;

    if (scl != 0.0f && alp != 0)
    {
        for (i = top_chr; i <= end_chr; i++)
        {
            CopySprDToSpr(&ds, &spr_dat[i]);

            ds.x += pos_x;
            ds.y += pos_y;
            ds.csx = spr_dat[top_chr].x + pos_x + spr_dat[top_chr].w * 0.5f;
            ds.csy = spr_dat[top_chr].y + pos_y;
            ds.scw = 1.0f;
            ds.sch = scl;
            ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
            ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
            ds.alpha = alp;

            DispSprD(&ds);
        }
    }
}

static void JetCD(JET_SET *ninjin, u_char num, u_char tim, u_char spd, u_char wid, u_short chr, u_char hgh, short int mov_h, short int mov_v, short int px, short int base, u_char alp)
{
    int i;
    float nin_scl_x;
    float min_scl_y;
    u_char min_alp;

    for (i = 0; i < num; i++)
    {
        if (ninjin[i].tim == 0)
        {
            ninjin[i].tim = rand() % tim + 1;
            ninjin[i].spd = rand() % spd + 1;
            ninjin[i].mov_x = rand() % wid + 1;
            ninjin[i].mov_y = 0;
            ninjin[i].tim_max = ninjin[i].tim;
        }
        else
        {
            ninjin[i].tim--;
            ninjin[i].mov_y += ninjin[i].spd;
        }

        min_alp = (alp * ninjin[i].tim) / (float)ninjin[i].tim_max;
        nin_scl_x = ninjin[i].tim / (float)ninjin[i].tim_max;
        min_scl_y = (1.0f - nin_scl_x) * 4.0f;

        SyonenJet(chr, hgh, mov_h + ninjin[i].mov_x, mov_v - ninjin[i].mov_y, px, base, min_alp, nin_scl_x, min_scl_y);
    }

    if (new_inf.issei_timer != 0)
    {
        new_inf.issei_timer--;
    }
    else
    {
        new_inf.issei_timer = 0xff;
    }
}

static void SyonenJet(u_short chr, u_char hgh, short int mov_h, short int mov_v, short int px, short int base, u_char alp, float scl_x, float scl_y)
{
    DISP_SPRT ds;
    short hh;

    CopySprDToSpr(&ds,spr_dat + (u_short)chr);

    if (ds.y + mov_v < base)
    {
        if (base - (ds.y + mov_v) < hgh)
        {
            hh = base - (ds.y + mov_v);
            ds.h = hh;
        }
    }

    ds.x += mov_h;
    ds.y += mov_v;
    ds.scw = scl_x;
    ds.sch = scl_y;
    ds.csx = ds.x + (ds.w >> 1);
    ds.csy = base;
    ds.alpha = alp;
    ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);
}

static void TameKin2(short int pos_x, short int pos_y)
{
    int i;
    int j;
    u_char alp;
    float per;
    u_char charge_max;
    u_char ini_num;
    u_char top;
    u_char chrg_rot;
    u_char chrg_alp;
    u_char chrg_siz;
    u_char chrg_rgb;
    u_char chrg_flr;
    u_char init_alp;
    float rad;

    alp = (inf_dsp.fndr_fade_alp * 128) / 100.0f;

    charge_max = cam_custom_wrk.charge_max * 2 + 6;

    for (i = 0; i < charge_max; i++)
    {
        PutSpriteYW(FND_STK_NUM_BASE_00 + i, FND_STK_NUM_BASE_00 + i, pos_x + 143, pos_y + 389, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }

    top = (12 - charge_max) / 2;

    ini_num = ini_charge_tbl[plyr_wrk.film_no];

    for (i = top; i < top + ini_num; i++)
    {
        new_inf.chrg_stp[i] = 3;
    }

    if (ini_num != 0)
    {
        init_alp = (SgCosf((new_inf.tm_max * (PI * 2)) / 60.0f) + 1.0f) * 32.0f + 64.0f;
    }
    else
    {
        init_alp = 0;
    }

    for (i = top; i < top + charge_max; i++)
    {
        if (i - top < plyr_wrk.charge_num)
        {
            if (new_inf.chrg_stp[i] == 0)
            {
                new_inf.chrg_rot[i] = 0;
                new_inf.chrg_flr[i] = 0;
                new_inf.chrg_stp[i] = 1;
                new_inf.dray_tmr[i] = 0;
            }
        }
        else
        {
            new_inf.chrg_stp[i] = 0;

            ZanzoSS(zanzo + i);
        }

        switch (new_inf.chrg_stp[i])
        {
        case 0:
            new_inf.chrg_flr[i] = 0;

            if (new_inf.chrg_rot[i] != 0)
            {
                new_inf.chrg_rot[i]--;
            }

            chrg_alp = (new_inf.chrg_rot[i] * 64) / 20.0f;
            chrg_rot = 0;
            chrg_siz = 100;
            chrg_rgb = 128;
            chrg_flr = 0;
        break;
        case 1:
            if (new_inf.chrg_rot[i] < 20)
            {
                new_inf.chrg_rot[i]++;
            }
            else
            {
                new_inf.chrg_rot[i] = 20;
                new_inf.chrg_stp[i] = 2;
            }

            per = new_inf.chrg_rot[i] / 20.0f;

            chrg_rot = 180.0f - 180.0f * per * per;
            chrg_alp = 64.0f * per * per;
            chrg_siz = 375.0f - 275.0f * per * per;
            chrg_rgb = 128.0f * per * per;
            chrg_flr = 0;
        break;
        case 2:
            chrg_rot = 0;
            chrg_alp = 64;
            chrg_siz = 100;
            chrg_rgb = 128;

            if (new_inf.chrg_flr[i] < 20)
            {
                new_inf.chrg_flr[i]++;
            }
            else
            {
                new_inf.chrg_flr[i] = 20;
                new_inf.chrg_stp[i] = 3;
            }

            rad = (new_inf.chrg_flr[i] * PI) / 20.0f;
            chrg_flr = SgSinf(rad) * 128.0f;
        break;
        case 3:
            if (new_inf.chrg_stp[charge_max + top - 1] != 3)
            {
                chrg_rot = 0;
                chrg_alp = 64;
                chrg_siz = 100;
                chrg_rgb = 128;
                chrg_flr = 0;
            }
            else
            {
                chrg_rot = 0;
                chrg_siz = 100;
                chrg_rgb = 128;
                chrg_alp = (SgCosf((new_inf.tm_max * 6.2831855f) / 60.0f) + 1.0f) * 32.0f + 64.0f;
                chrg_flr = 0;
            }
        break;
        }

        if (new_inf.dray_tmr[i] == 0)
        {
            if (i - top < ini_num)
            {
                ZanzoSM(zanzo + i, chrg_rot, init_alp, chrg_siz, 0xff, 0);
            }
            else
            {
                ZanzoSM(zanzo + i, chrg_rot, chrg_alp, chrg_siz, 0xff, 0);
            }

            new_inf.dray_tmr[i] = 2;
        }
        else
        {
            new_inf.dray_tmr[i]--;
        }

        for (j = 0; j < 6; j++)
        {
            AhoBon(i + 26, pos_x + 143, pos_y + 389, zanzo[i].num1[j], chrg_rgb, zanzo[i].num2[j], zanzo[i].num3[j], i - top, ini_num);
        }

        BakBon(i + 26, pos_x + 143, pos_y + 389, chrg_flr);
    }

    if (new_inf.chrg_stp[charge_max + top - 1] == 3 || ini_num != 0)
    {
        if (new_inf.tm_max < 60)
        {
            new_inf.tm_max++;
        }
        else
        {
            new_inf.tm_max = 0;
        }
    }
    else
    {
        new_inf.tm_max = 0;
    }
}

static void ZanzoSS(ZAN *zz)
{
    int i;

    for (i = 0; i < 6; i++)
    {
        zz->num1[i] = 0xFF;
        zz->num2[i] = 0xFF;
        zz->num3[i] = 0xFF;
        zz->num4[i] = 0xFF;
    }
}

static void ZanzoSM(ZAN *zz, u_char num1, u_char num2, u_char num3, u_char num4, u_char mode)
{
    int i;

    for (i = 5; i > 0; i--)
    {
        zz->num1[i] = zz->num1[i-1];
        zz->num2[i] = zz->num2[i-1];
        zz->num3[i] = zz->num3[i-1];
        zz->num4[i] = zz->num4[i-1];
    }

    zz->num1[0] = num1;
    zz->num2[0] = num2;
    zz->num3[0] = num3;
    zz->num4[0] = num4;

    if (mode == 0)
    {
        for (i = 5; i > 0; i--)
        {
            if (zz->num1[i] == zz->num1[0])
            {
                zz->num1[i] = 0xff;
            }
            if (zz->num2[i] == zz->num2[0])
            {
                zz->num2[i] = 0xff;
            }
            if (zz->num3[i] == zz->num3[0])
            {
                zz->num3[i] = 0xff;
            }
            if (zz->num4[i] == zz->num4[0])
            {
                zz->num4[i] = 0xff;
            }
        }
    }
    else
    {
        for (i = 5; i > 0; i--)
        {
            if (zz->num3[i] == zz->num3[0])
            {
                zz->num3[i] = 0xff;
            }

            if (zz->num4[i] == zz->num4[0])
            {
                zz->num4[i] = 0xff;
            }
        }
    }
}

static void ZanzoLS(u_char *c)
{
    int i;

    for (i = 0; i < 6; i++)
    {
        c[i] = 0xFF;
    }
}

static void ZanzoLM(u_char *zz, u_char per)
{
    int i;

    for (i = 5; i > 0; i--)
    {
        zz[i] = zz[i-1];
    }

    zz[0] = per;

    for (i = 5; i > 0; i--)
    {
        if (zz[i] == per)
        {
            zz[i] = 0xff;

            per = zz[0];
        }
    }
}

static void AhoBon(u_short chr, short int px, short int py, u_char rot, u_char rgb, u_char alp, u_char siz, u_char now_num, u_char ini_num)
{
    float xo;
    float yo;
    DISP_SPRT ds;

    if (rot != 0xff && alp != 0xff && siz != 0xff)
    {
        CopySprDToSpr(&ds, &spr_dat[chr]);

        ds.x += px;
        ds.y += py;

        xo = ds.x + ds.w * 0.5f;
        yo = ds.y + ds.h * 0.5f;

        ds.crx = xo;
        ds.cry = yo;

        ds.rot = -rot;

        ds.csx = xo;
        ds.csy = yo;

        ds.scw = siz / 100.0f;
        ds.sch = siz / 100.0f;

        ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
        ds.alpha = alp * 0.5f;

        if (inf_dsp.lucky7 != 0)
        {
            ds.r = 0x80;
            ds.g = 0;
            ds.b = 0;
        }
        else if (now_num < ini_num)
        {
            ds.r = 0xff;
            ds.g = 0x7f;
            ds.b = 0;
        }
        else
        {
            ds.r = rgb;
            ds.g = rgb;
            ds.b = rgb;
        }

        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

        DispSprD(&ds);
    }
}

static void BakBon(u_short chr, short int px, short int py, u_char flr)
{
    float xo;
    float yo;
    DISP_SPRT ds;
    int rgb;

    CopySprDToSpr(&ds, &spr_dat[chr]);

    ds.x += px;
    ds.y += py;

    xo = ds.x + ds.w * 0.5f;
    yo = ds.y + ds.h * 0.5f;

    if (inf_dsp.lucky7 != 0)
    {
        rgb = 0x800000;
    }
    else
    {
        rgb = 0x808080;
    }

    if (flr != 0)
    {
        PutSpriteYW(FND_SEAL_LIT_FLA, FND_SEAL_LIT_FLA, xo - 63.0f, yo - 63.0f, 0.0f, rgb, flr, 25.0f / 63.0f, 25.0f / 63.0f, 0, 0xff, 1, 1, 0);
    }
}

static void PhotoScoreDisp(int count, int wait)
{
    u_char alp_table2[31] = {
         0,  3,  3,  4,  4,  5,  5,  6,  8, 10, 12, 13, 14,  14,  15, 15,
        16, 17, 18, 20, 23, 26, 30, 36, 44, 54, 66, 80, 96, 112, 128,
    };
    static int e_flow = 0xff;
    static float fcnt;
    float fnum;
    float fslide;
    u_char alp;
    int cnt;
    float fw;
    float f;
    DISP_SPRT ds;
    SPRT_DAT raze_sprt = {
        .tex0 = SCE_GS_SET_TEX0_1(0xa0900 / 64, 2, SCE_GS_PSMT8, 6, 4, 1, SCE_GS_MODULATE, 0xa0a80 / 64, SCE_GS_PSMCT32, 0, 0, 1),
        .u = 0,
        .v = 0,
        .w = 60,
        .h = 16,
        .x = 0,
        .y = 0,
        .pri = 0x1000,
        .alpha = 0x80,
    };
    short int pos_x;

    fnum = 24.0f;

    fslide = 4.0f;

    if (count + 1 == wait)
    {
        e_flow = 0;
    }

    CopySprDToSpr(&ds, &raze_sprt);

    switch (e_flow)
    {
    case 0:
        fcnt = (1.0f - SgSinf(0.0f)) * fnum;
        e_flow++;
    break;
    case 1:
        // do nothing ...
    break;
    case 2:
    default:
        return;
    break;
    }

    if (wait - count < 10)
    {
        alp = alp_table2[(wait - count) * 3];
        cnt = (wait - count) * 9;
    }
    else if (count <= 10)
    {
        alp = alp_table2[count * 3];
        cnt = count * 9;
    }
    else
    {
        alp = alp_table2[30];
        cnt = 90;
    }

    fw = fcnt * 2.0f + 1.0f;

    fw = fw != (int)fw ? (int)fw + 1 : fw; // (positive) round up to integer

    ds.alpha = alp;

    for (f = 0.0f; f < fcnt * 2.0f + 1.0f; f += 1.0f)
    {
        pos_x = (f - (fw - 1.0f) * 0.5f) * fslide + 273.0f;

        NumberDisp(photo_wrk.score, pos_x, 43, 2, 0xff, 0xff, 0xff, alp, 0, 7, 0);

        ds.x = pos_x + 0xaf;
        ds.y = 55.0f;

        DispSprD(&ds);
    }

    fcnt = (1.0f - SgSinf(DEG2RAD(cnt))) * fnum;

    if (count < 2)
    {
        e_flow++;
    }
}

char point_get_end = 0;

void InfoDispPause()
{
    inf_dsp.disp_pause = 1;
}

void InfoDispRestart()
{
    inf_dsp.disp_pause = 0;
}

static void BukiyouMan(short int px, short int py)
{
    u_char alp_max;
    u_char num;
    u_char buki_alp;
    u_char alp;

    alp_max = (inf_dsp.fndr_fade_alp * 128) / 100.0f;

    PutSpriteYW(FND_WEP_GAUGE, FND_WEP_GAUGE, px, py, 0.0f, 0x808080, alp_max, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    if (cam_custom_wrk.set_sub == 0xff)
    {
        return;
    }

    if (CheckCamSubEffect() != 0)
    {
        new_inf.buki_use_flg = 1;
    }
    else
    {
        new_inf.buki_use_flg = 0;
    }

    PutSpriteYW(FND_WEP_FNT_0B + cam_custom_wrk.set_sub, FND_WEP_FNT_0B + cam_custom_wrk.set_sub, px, py, 0.0f, 0x808080, (alp_max * 4) / 5.0f, 1.0f, 0.98f, 0, 0xff, 1, 0, 0);

    if (poss_item[5] > 9)
    {
        num = 9;
    }
    else
    {
        num = poss_item[5];
    }

    PutNumberYW(12, num, px + 570, py + 369, 1.0f, 1.0f, 0x808080, alp_max, -1, 1, 0);

    if (new_inf.buki_use_flg != 0)
    {
        if (new_inf.buki_use_alp + 0x10 >= alp_max)
        {
            new_inf.buki_use_alp = alp_max;
        }
        else
        {
            new_inf.buki_use_alp += 16;
        }
    }
    else
    {
        if (new_inf.buki_use_alp - 0x10 > 0)
        {
            new_inf.buki_use_alp -= 16;
        }
        else
        {
            new_inf.buki_use_alp = 0;
        }
    }

    if (new_inf.buki_use_flg != 0 && alp_max > new_inf.buki_use_alp)
    {
        new_inf.buki_use_flr = new_inf.buki_use_alp;
    }
    else
    {
        if (new_inf.buki_use_flr - 16 > 0)
        {
            new_inf.buki_use_flr -= 16;
        }
        else
        {
            new_inf.buki_use_flr = 0;
        }
    }

    if (new_inf.buki_use_alp == alp_max)
    {
        alp = (int)((SgCosf(new_inf.buki_use_tmr * (PI * 2) / 60.0f) + 1.0f) * 50.0f * 0.5f) + 50;

        buki_alp = new_inf.buki_use_alp * alp / 100.0f;

        if (new_inf.buki_use_tmr < 60)
        {
            new_inf.buki_use_tmr++;
        }
        else
        {
            new_inf.buki_use_tmr = 0;
        }
    }
    else
    {
        buki_alp = new_inf.buki_use_alp;

        new_inf.buki_use_tmr = 0;
    }

    if (new_inf.buki_use_alp != 0)
    {
        PutSpriteYW(cam_custom_wrk.set_sub + FND_WEP_FNT_0A, cam_custom_wrk.set_sub + FND_WEP_FNT_0A, px, py, 0.0f, 0x808080, buki_alp, 1.0f, 0.98f, 0, 0xff, 1, 1, 0);
    }

    if (new_inf.buki_use_flr != 0)
    {
        PutSpriteYW(FND_WEP_GAUGE_FLA, FND_WEP_GAUGE_FLA, px, py, 0.0f, 0x808080, new_inf.buki_use_alp, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
    }
}

static void DameDameInit()
{
    int i;

    for (i = 0; i < 4; i++)
    {
        new_inf.shot_kind[i] = 0xff;
        new_inf.shot_kind2 = 0xff;
    }

    new_inf.shot_num = 0;

    if (photo_wrk.bonus_sta & 0x20)
    {
        new_inf.shot_kind[new_inf.shot_num] = 1;
        new_inf.shot_num++;
    }

    if (photo_wrk.bonus_sta & 0x10)
    {
        new_inf.shot_kind[new_inf.shot_num] = 2;
        new_inf.shot_num++;
    }

    if (photo_wrk.bonus_sta & 0x8)
    {
        new_inf.shot_kind[new_inf.shot_num] = 3;
        new_inf.shot_num++;
    }

    if (photo_wrk.bonus_sta & 0x2)
    {
        new_inf.shot_kind[new_inf.shot_num] = 4;
        new_inf.shot_num++;
    }

    if (photo_wrk.bonus_sta & 0x4)
    {
        new_inf.shot_kind[new_inf.shot_num] = 5;
        new_inf.shot_num++;
    }

    if (photo_wrk.bonus_sta & 0x80)
    {
        new_inf.shot_kind[new_inf.shot_num] = 6;
        new_inf.shot_num++;
    }

    if (photo_wrk.bonus_sta & 0x100)
    {
        new_inf.shot_kind2 = 1;
    }

    if (photo_wrk.bonus_sta & 0x200)
    {
        new_inf.shot_kind2 = 2;
    }

    if (photo_wrk.bonus_sta & 0x400)
    {
        new_inf.shot_kind2 = 3;
    }

    if (photo_wrk.bonus_sta & 0x800)
    {
        new_inf.shot_kind2 = 4;
    }

    new_inf.shot_mov = 20;
    new_inf.shot_pse = 30;
    new_inf.shot_lgt = 20;
    new_inf.shot_fad = 20;
}

static void GekisyaBoyGeisyaGirl(short int px, short int py)
{
    int i;
    u_char alp1;
    u_char alp2;
    u_char alp_max;
    u_char mov;
    short int y[10];

    alp_max = (inf_dsp.fndr_fade_alp * 128) / 100.0f;

    if (new_inf.shot_mov != 0)
    {
        new_inf.shot_mov--;

        mov = 100.0f * (1.0f - new_inf.shot_mov / 20.0f) * (1.0f - new_inf.shot_mov / 20.0f);
        alp1 = alp_max * (1.0f - new_inf.shot_mov / 20.0f) * (1.0f - new_inf.shot_mov / 20.0f);
        alp2 = 0;
    }
    else if (new_inf.shot_pse != 0)
    {
        new_inf.shot_pse--;

        mov = 100;
        alp1 = 0x80;
        alp2 = 0;
    }
    else if (new_inf.shot_lgt != 0)
    {
        new_inf.shot_lgt--;

        mov = 100;
        alp1 = 0x80;
        alp2 = alp_max * SgSinf((new_inf.shot_lgt * PI) / 20.0f);
    }
    else if (new_inf.shot_fad != 0)
    {
        new_inf.shot_fad--;

        mov = 100;
        alp1 = (alp_max * new_inf.shot_fad) / 20.0f;
        alp2 = 0;
    }
    else
    {
        return;
    }

    for (i = 0; i < 7; i++)
    {
        y[i] = -1;
    }

    switch(new_inf.shot_num)
    {
    case 1:
        y[0] = 313;
    break;
    case 2:
        y[0] = 313;
        y[1] = 329;
    break;
    case 3:
        y[0] = 313;
        y[1] = 329;
        y[2] = 345;
    break;
    case 4:
        y[0] = 313;
        y[1] = 329;
        y[2] = 345;
        y[3] = 361;
    break;
    case 5:
        y[0] = 313;
        y[1] = 329;
        y[2] = 345;
        y[3] = 361;
        y[4] = 377;
    // forgot to break?
    case 6:
        y[0] = 313;
        y[1] = 329;
        y[2] = 345;
        y[3] = 361;
        y[4] = 377;
        y[5] = 393;
    break;
    }

    i = 0;

    SetSprFile(PL_LIFE_PK2_ADDRESS);

    while (y[i] != -1)
    {
        switch(new_inf.shot_kind[i])
        {
        case 1:
            if (alp1 != 0)
            {
                PutSpriteYW(FND_END, FND_END, mov + px, y[i] + py, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }

            if (alp2 != 0)
            {
                PutSpriteYW(FND_END, FND_END, mov + px, y[i] + py, 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
            }
        break;
        case 2:
            if (alp1 != 0)
            {
                PutSpriteYW(FND_FNT_TRIPLE1, FND_FNT_TRIPLE2, mov + px, y[i] + py, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }

            if (alp2 != 0)
            {
                PutSpriteYW(FND_FNT_TRIPLE1, FND_FNT_TRIPLE2, mov + px, y[i] + py, 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
            }
        break;
        case 3:
            if (alp1 != 0)
            {
                PutSpriteYW(FND_FNT_DOUBLE1, FND_FNT_DOUBLE2, mov + px, y[i] + py, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }

            if (alp2 != 0)
            {
                PutSpriteYW(FND_FNT_DOUBLE1, FND_FNT_DOUBLE2, mov + px, y[i] + py, 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
            }
        break;
        case 4:
            if (alp1 != 0)
            {
                PutSpriteYW(FND_DM_FNT_CLOSE, FND_DM_FNT_CLOSE, mov + px, y[i] + py, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }

            if (alp2 != 0)
            {
                PutSpriteYW(FND_DM_FNT_CLOSE, FND_DM_FNT_CLOSE, mov + px, y[i] + py, 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
            }
        break;
        case 5:
            if (alp1 != 0)
            {
                PutSpriteYW(FND_DM_FNT_CORE, FND_DM_FNT_CORE, mov + px, y[i] + py, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }

            if (alp2 != 0)
            {
                PutSpriteYW(FND_DM_FNT_CORE, FND_DM_FNT_CORE, mov + px, y[i] + py, 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
            }
        break;
        case 6:
            if (alp1 != 0)
            {
                PutSpriteYW(FND_DM_FNT_SPCIAL, FND_DM_FNT_SPCIAL, mov + px, y[i] + py, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }

            if (alp2 != 0)
            {
                PutSpriteYW(FND_DM_FNT_SPCIAL, FND_DM_FNT_SPCIAL, mov + px, y[i] + py, 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
            }
        break;
        }

        i++;
    }

    y[7] = 297;

    switch (new_inf.shot_kind2)
    {
    case 1:
        if (alp1 != 0)
        {
            PutSpriteYW(FND_FNT_IMPACT1, FND_FNT_IMPACT2, mov + px, y[7] + py, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }

        if (alp2 != 0)
        {
            PutSpriteYW(FND_FNT_IMPACT1, FND_FNT_IMPACT2, mov + px, y[7] + py, 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
        }
    break;
    case 2:
        if (alp1 != 0)
        {
            PutSpriteYW(FND_FNT_SLOW, FND_FNT_SLOW, mov + px, y[7] + py, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }

        if (alp2 != 0)
        {
            PutSpriteYW(FND_FNT_SLOW, FND_FNT_SLOW, mov + px, y[7] + py, 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
        }
    break;
    case 3:
        if (alp1 != 0)
        {
            PutSpriteYW(FND_FNT_FIND, FND_FNT_FIND, mov + px, y[7] + py, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }

        if (alp2 != 0)
        {
            PutSpriteYW(FND_FNT_FIND, FND_FNT_FIND, mov + px, y[7] + py, 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
        }
    break;
    case 4:
        if (alp1 != 0)
        {
            PutSpriteYW(FND_FNT_BIND, FND_FNT_BIND, mov + px,y[7] + py, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }

        if (alp2 != 0)
        {
            PutSpriteYW(FND_FNT_BIND, FND_FNT_BIND, mov + px, y[7] + py, 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
        }
    break;
    }
}

static char KanKinoSyogai(short int pos_x, short int pos_y)
{
    int i;
    char dir;
    u_char alp;

    if (cam_custom_wrk.set_spe == 0xff)
    {
        return;
    }

    if (cam_custom_wrk.set_spe != 0)
    {
        return;
    }

    alp = (inf_dsp.fndr_fade_alp * 128) / 100.0f;

    for (i = 0; i < 4; i++)
    {
        PutSpriteYW(KAN_BU + i, KAN_BU + i, pos_x, pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0x0, 0xff, 0x1, 0x0, 0x1);
    }

    if (plyr_wrk.spe1_dir & 0x1)
    {
        PutSpriteYW(KAN_FU, KAN_FU, pos_x, pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0x0, 0xff, 0x1, 0x1, 0x1);
    }

    if (plyr_wrk.spe1_dir & 0x2)
    {
        PutSpriteYW(KAN_FR, KAN_FR, pos_x, pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0x0, 0xff, 0x1, 0x1, 0x1);
    }

    if (plyr_wrk.spe1_dir & 0x4)
    {
        PutSpriteYW(KAN_FD, KAN_FD, pos_x, pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0x0, 0xff, 0x1, 0x1, 0x1);
    }

    if (plyr_wrk.spe1_dir & 0x8)
    {
        PutSpriteYW(KAN_FL, KAN_FL, pos_x, pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0x0, 0xff, 0x1, 0x1, 0x1);
    }

    return dir;
}

int isDispLamp()
{
    if (
        ingame_wrk.mode == INGAME_MODE_GAME_OVER ||
        efcnt[12].dat.uc8[0] != 0 ||
        (ingame_wrk.game == 0 && poss_item[9] == 0) ||
        eff_filament_off != 0 ||
        (ingame_wrk.game == 1 && isBattleModeGame() == 0) ||
        ingame_wrk.mode == INGAME_MODE_SPD_OPT ||
        plyr_wrk.mode == PMODE_MSG_DISP ||
        (ingame_wrk.game == 0 && ingame_wrk.difficult != 0)
    )
    {
        return 0;
    }

    return 1;
}
