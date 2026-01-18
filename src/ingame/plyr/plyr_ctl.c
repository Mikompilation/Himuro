#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "plyr_ctl.h"

#include "sce/libvu0.h"

#include "os/eeiop/eese.h"
#include "os/pad.h"
#include "common/ul_math.h"
#include "main/glob.h"
#include "ingame/camera/camera.h"
#include "ingame/enemy/ene_ctl.h"
#include "ingame/entry/ap_pgost.h"
#include "ingame/entry/ap_rgost.h"
#include "ingame/entry/rgst_dat.h"
#include "ingame/event/ev_load.h"
#include "ingame/event/ev_main.h"
#include "ingame/ig_glob.h"
#include "ingame/info/inf_disp.h"
#include "ingame/map/door_ctl.h"
#include "ingame/map/find_ctl.h"
#include "ingame/map/furn_ctl.h"
#include "ingame/map/furn_dat.h"
#include "ingame/map/map_ctrl.h"
#include "ingame/menu/gameover.h"
#include "ingame/menu/item_get.h"
#include "ingame/plyr/plyr_ctl.h"
#include "ingame/plyr/time_ctl.h"
#include "ingame/plyr/unit_ctl.h"
#include "graphics/motion/motion.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_ene.h"
#include "graphics/graph2d/effect_scr.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/effect_sub2.h"
#include "graphics/graph3d/load3d.h"
#include "graphics/graph3d/sglib.h"

float photo_rng_tbl[] = {
    5000.0f
};
u_short photo_frame_tbl[][2] = {
    { 436, 300 },
};
float circle_radius_tbl[] = {
    90.0f, 105.0f, 120.0f, 145.0f
};
u_char charge_max_tbl[] = {
    6, 8, 10, 12
};
u_char ini_charge_tbl[] = {
    0, 0, 2, 4, 6
};
u_short photo_dmg_tbl[] = {
     10,  20,  30,  40,  50,  60,  70,  80,  90, 100, 110,
    120, 130, 140, 150, 160, 170, 180, 190, 200, 210, 250,
};
float photo_dist_ratio[] = {
    1.2f, 1.15f, 1.1f, 1.0f, 0.95f,
    0.9f, 0.85f, 0.8f, 0.75, 0.70f,
};
float photo_center_ratio[] = {
    1.6f, 1.4f, 1.2f, 1.1f, 1.0f,
    0.9f, 0.8f, 0.7f, 0.6f, 0.5f,
};
float photo_charge_ratio[] = {
    1.4f, 1.6f, 1.8f, 2.0f
};
PWARP_DAT pwarp_dat[] = {
    { .room = 19, .floor = 1, .pos_x = 21975, .pos_y = 65311, .pos_z = 25575 },
    { .room = 31, .floor = 0, .pos_x = 21975, .pos_y =  3000, .pos_z = 27000 },
};

int pa_radius_set = 0;
static u_char step_direction = 0;
static u_char no_photo_tm = 0;
static u_char dmg_step = 0;

static u_char avoid_chk;
static u_short hp_down_deg;

#define PI 3.1415927f
#define DEG2RAD(x) ((float)(x)*PI/180.0f)

void PlyrCtrlMain()
{
    if (dbg_wrk.high_speed_mode != 0 && *key_now[6] != 0)
    {
        plyr_wrk.spd = 27.5f;
    }

    PlyrLightSet();
    PlyrSpotLightOnChk();

    if (DBG_cam_id_move_chk != 0)
    {
        return;
    }

    PlyrBattleChk();
    PlyrTrembleChk();
    PlyrDmgChk();
    PlyrVibCtrl(0);
    PlyrHPdwonCtrl();

    if (plyr_wrk.sta & 0x200)
    {
        return;
    }

    if (PlyrDoorOpenChk() != 0)
    {
        return;
    }

    PlyrCondChk();
    PlyrTimerCtrl();

    if (ShortPauseChk() != 0)
    {
        return;
    }

    PlyrFinderModeChk();
    ClrEneSta();

    switch (plyr_wrk.mode)
    {
    case PMODE_NORMAL:
        PlyrNormalCtrl();
    break;
    case PMODE_FINDER:
        PlyrFinderCtrl();
    break;
    case PMODE_DMG:
        PlyrDmgCtrl();
    break;
    case PMODE_FINDER_END:
        PlyrFinderEnd();
    break;
    case PMODE_MSG_DISP:
        PlyrMessageDisp();
    break;
    case PMODE_ANIME:
        PlyrSpeAnimeCtrl();
    break;
    }
}

int PlyrDoorOpenChk()
{
    if (plyr_wrk.sta & 0x8)
    {
        return 1;
    }

    return 0;
}

u_char ShortPauseChk()
{
    u_char chk;

    chk = 0;

    if (plyr_wrk.ap_timer != 0)
    {
        chk = 1;

        if ((ingame_wrk.stts & 0x10) == 0)
        {
            ingame_wrk.stts |= 0x10;
        }
    }
    else if (ingame_wrk.stts & 0x10)
    {
        ingame_wrk.stts &= 0xef;
    }

    return chk;
}

void PlyrCondChk()
{
    if (plyr_wrk.hp > 0 && plyr_wrk.hp <= 50 && plyr_wrk.se_deadly == -1)
    {
        plyr_wrk.se_deadly = SeStartFix(36, 0, 0x1000, 0x1000, 0);
    }
    else if (plyr_wrk.se_deadly != -1)
    {
        if (plyr_wrk.hp > 50)
        {
            SeFadeFlame(plyr_wrk.se_deadly, 60, 0);

            plyr_wrk.se_deadly = -1;
        }
    }

    if (plyr_wrk.cond != 0)
    {
        if (plyr_wrk.cond_tm == 0)
        {
            plyr_wrk.cond = 0;
        }
        else
        {
            plyr_wrk.cond_tm -= 1;
        }
    }

    switch (plyr_wrk.cond)
    {
    case 2:
        SetEffects(6, 1, 7, 0x14);

        if (LeverGachaChk() != 0)
        {
            plyr_wrk.cond_tm = plyr_wrk.cond_tm < 3 ? 0 : plyr_wrk.cond_tm - 2;
        }
        break;
    case 3:
        SetEffects(6, 1, 4, 0x32);
    break;
    }
}

void PlyrDmgChk()
{
    static u_char avoid_chk_tm = 0;
    static u_char wrong_chk = 0;

    if (plyr_wrk.dmg == 0)
    {
        return;
    }

    dmg_step = 0;

    if (plyr_wrk.dmg_type < 2)
    {
      CallNega2(0, 30, 60);
      ReqPlyrHPdown(plyr_wrk.dmg, 1);
      PlyrVibCtrl(15);

      plyr_wrk.dmg = 0;

        if (plyr_wrk.dmg_type == 1)
        {
            dmg_step = plyr_wrk.dmg_type;

            plyr_wrk.mode = 2;
        }
    }
    else
    {
        if (avoid_chk_tm == 0)
        {
            avoid_chk = 0;
            wrong_chk = 0;
        }

        if (avoid_chk_tm < 18)
        {
            avoid_chk_tm += 1;

            if (avoid_chk_tm < 8)
            {
                if (pad[0].one & 0x40 || pad[0].one & 0xf000 || PlyrLeverInputChk() == 2)
                {
                    wrong_chk += 1;
                }
            }
            else
            {
                if (pad[0].one & 0x40 || pad[0].one & 0xf000)
                {
                    avoid_chk = avoid_chk + 1;
                }

                if (PlyrLeverInputChk() == 2)
                {
                    avoid_chk = 1;
                }
            }
        }
        else
        {
            if (avoid_chk != 1 || wrong_chk != 0)
            {
                avoid_chk = 0;

                CallNega2(0, 90, 60);
                ReqPlyrHPdown(plyr_wrk.dmg, 1);
            }

            plyr_wrk.mode = 2;
            plyr_wrk.dmg = 0;
            avoid_chk_tm = 0;
        }
    }
}

float paz = 20.0f;

void PlyrVibCtrl(u_char time)
{
    if (time != 0)
    {
        plyr_vib_time = time;
    }

    if (plyr_vib_time != 0)
    {
        VibrateRequest1(0, 1);

        plyr_vib_time -= 1;
    }
}

void PlyrFinderModeChk()
{
    u_short pad_finder;
    u_short pad_finder_bk;

    if (plyr_wrk.mode == PMODE_NORMAL)
    {
        if (opt_wrk.key_type == 0 || opt_wrk.key_type == 1 || opt_wrk.key_type == 4 || opt_wrk.key_type == 5)
        {
            pad_finder = *key_now[7];

            if (pad_finder != 0 && pad[0].push[5] < 7)
            {
                pad_finder = 0;
            }
        }
        else
        {
            pad_finder = *key_now[10];

            if (pad_finder != 0 && pad[0].push[9] < 7)
            {
                pad_finder = 0;
            }
        }

        if (
            (pad_finder == 1 || pad_finder == 2) &&
            (plyr_wrk.mvsta & 0x200000) == 0 &&
            plyr_wrk.cond != 1 && plyr_wrk.cond != 2 &&
            (plyr_wrk.sta & 2) == 0
        )
        {
            FinderInSet();
        }
    }
    else if (plyr_wrk.mode == PMODE_FINDER && (plyr_wrk.sta & 0x1000) == 0)
    {
        if (opt_wrk.key_type == 0 || opt_wrk.key_type == 1 || opt_wrk.key_type == 4 || opt_wrk.key_type == 5)
        {
            pad_finder = *key_now[7];
            pad_finder_bk = *key_bak[7];
#ifdef BUILD_EU_VERSION

            if (pad_finder != 0 && pad[0].push[5] < 7)
            {
                pad_finder = 0;
            }
#endif
        }
        else
        {
            pad_finder = *key_now[10];
            pad_finder_bk = *key_bak[10];
#ifdef BUILD_EU_VERSION

            if (pad_finder != 0 && pad[0].push[9] < 7)
            {
                pad_finder = 0;
            }
#endif
        }

        if (plyr_wrk.cond != 1)
        {
            if (pad_finder == 1 || (pad_finder_bk > 19 && pad_finder == 0))
            {
                FinderEndSet();
                plyr_wrk.mode = PMODE_FINDER_END;
            }
        }
    }
}

void FinderInSet()
{
    ENE_WRK *ew;
    sceVu0FVECTOR tv;
    float dist[2];
    u_char i;
    u_char trgt;

    plyr_wrk.mode = PMODE_FINDER_IN;

    plyr_wrk.sta |= 4;
    plyr_wrk.sta &= ~0x400;

    plyr_wrk.fp[0] = 320;
    plyr_wrk.fp[1] = 209;

    plyr_wrk.frot_x = 0.0f;

    if (plyr_wrk.move_mot == 11)
    {
        SetPlyrAnime(103, 0);
    }
    else
    {
        SetPlyrAnime(65, 0);
    }

    plyr_wrk.fh_no = -1;

    PlyrChargeCtrl(0xff, NULL);

    plyr_wrk.spe1_dir = 0;
    no_photo_tm = 20;

    ew = ene_wrk;
    dist[1] = 0.0f;

    for(i = 0, trgt = 0xff; i < 4; i++)
    {
        if (ew->sta & 0x1 && ew->hp != 0 && (ew->sta & 0x80000) == 0 && ew->sta2 & 0x1 && ew->tr_rate != 0)
        {
            dist[0] = GetDistV(plyr_wrk.move_box.pos, ew->move_box.pos);

            if (dist[0] <= 2500.0f)
            {
                if (dist[1] == 0.0f || dist[0] < dist[1])
                {
                    dist[1] = dist[0];
                    trgt = i;
                }
            }
        }

        ew++;
    }

    if (trgt != 0xff)
    {
        tv[0] = plyr_wrk.move_box.pos[0];
        tv[1] = plyr_wrk.move_box.pos[1],
        tv[2] = plyr_wrk.move_box.pos[2];
        tv[3] = plyr_wrk.move_box.pos[3];

        if (plyr_wrk.move_mot == 11)
        {
            tv[1] += -575.0f;
        }
        else
        {
            tv[1] += -687.0f;
        }

        GetTrgtRot(tv, ene_wrk[trgt].mpos.p0, plyr_wrk.move_box.rspd, 3);

        plyr_wrk.move_box.rspd[1] -= plyr_wrk.move_box.rot[1];

        RotLimitChk(&plyr_wrk.move_box.rspd[1]);

        plyr_wrk.frot_x = plyr_wrk.move_box.rspd[0];
        plyr_wrk.move_box.rspd[0] = 0.0f;
    }
    else
    {
        plyr_wrk.move_box.rspd[0] = 0.0f;
        plyr_wrk.move_box.rspd[1] = 0.0f,
        plyr_wrk.move_box.rspd[2] = 0.0f;
        plyr_wrk.move_box.rspd[3] = 0.0f;
    }

    for(i = 0; i < 4; i++)
    {
        if (ene_wrk[i].sta & 1)
        {
            ene_wrk[i].in_finder_tm = 0;
            ene_wrk[i].dist_p_e = 0;
        }
    }

    plyr_wrk.psave[0] = plyr_wrk.move_box.pos[0], plyr_wrk.psave[1] = plyr_wrk.move_box.pos[1],
    plyr_wrk.psave[2] = plyr_wrk.move_box.pos[2], plyr_wrk.psave[3] = plyr_wrk.move_box.pos[3];
}

void FinderEndSet()
{
    u_char i;

    plyr_wrk.sta &= ~0x10;
    plyr_wrk.fh_no = -1;
    plyr_wrk.mode = PMODE_NORMAL;

    if (plyr_wrk.move_mot == 11)
    {
        SetPlyrAnime(104, 0);
    }
    else
    {
        SetPlyrAnime(66, 0);
    }

    for (i = 0; i < 4; i++)
    {
        if (ene_wrk[i].sta & 1)
        {
            ene_wrk[i].in_finder_tm = 0;
            ene_wrk[i].dist_p_e = 0;
        }
    }

    SetDebugMenuSwitch(1);
}

void ClrEneSta()
{
    u_char i;

    if (plyr_wrk.aphoto_tm != 0)
    {
        plyr_wrk.aphoto_tm--;
    }

    for (i = 0; i < 4; i++)
    {
        ene_wrk[i].sta &= ~(0x400 | 0x200 | 0x100 | 0x40 | 0x20);
    }
}

void PlyrHeightCtrl(sceVu0FVECTOR tv)
{
    sceVu0FVECTOR p;
    sceVu0FVECTOR av;

    p[0] = plyr_wrk.move_box.pos[0];
    p[1] = plyr_wrk.move_box.pos[1],
    p[2] = plyr_wrk.move_box.pos[2];
    p[3] = plyr_wrk.move_box.pos[3];

    GetPointHeightChk(p);

    if (plyr_wrk.mvsta & 0xfff0)
    {
        if (tv[0] != 0.0f || tv[1] != 0.0f || tv[2] != 0.0f)
        {
            av[0] = 0.0f;
            av[1] = 0.0f;
            av[2] = paz;
            av[3] = 0.0f;

            RotFvector(plyr_wrk.move_box.rot, av);
            sceVu0AddVector(p, av, plyr_wrk.move_box.pos);
            GetPointHeightChk(p);
        }

        plyr_wrk.move_box.pos[1] = p[1];
    }
    else
    {
        if ((tv[0] != 0.0f || tv[1] != 0.0f || tv[2] != 0.0f) && plyr_wrk.move_box.pos[1] + 5.0f < p[1])
        {
            plyr_wrk.move_box.pos[1] += 5.0f;
        }
        else if ((tv[0] != 0.0f || tv[1] != 0.0f || tv[2] != 0.0f) && plyr_wrk.move_box.pos[1] - 5.0f > p[1])
        {
            plyr_wrk.move_box.pos[1] -= 5.0f;
        }
        else
        {
            plyr_wrk.move_box.pos[1] = p[1];
        }
    }

    if (dbg_wrk.high_speed_mode != 0)
    {
        plyr_wrk.move_box.pos[1] = p[1];
    }
}

void PlyrNormalCtrl()
{

    plyr_wrk.sta &= ~ 0x10;

    PlyrActionChk();
    PlyrNModeMoveCtrl();
    PlyrNAnimeCtrl();
    PlyrDWalkTmCtrl();
    PlyrSpotMoveCtrl();
}

void PlyrSpotMoveCtrl()
{
    sceVu0FVECTOR rv;
    float r;
    float rcng_adj;
    u_char d;

    if (plyr_wrk.cond == 1 || plyr_wrk.cond == 2)
    {
        return;
    }

    switch (plyr_wrk.mvsta & 0xfff0)
    {
    case 0x10:
        r = DEG2RAD(30);
    break;
    case 0x40:
        r = DEG2RAD(45);
    break;
    default:
        r = 0;
    break;
    }

    rcng_adj = DEG2RAD(1.0f);

    d = pad[0].analog[1];

    if ((plyr_wrk.mvsta & 0x200000) == 0 && (d < 48 || d > 208))
    {
        r = -(((d - 128) * (PI / 2)) / 127.0f);

        rcng_adj = DEG2RAD(3.0f);

        if (r > DEG2RAD(45.0f))
        {
            r = DEG2RAD(45.0f);
        }

        else if (r < 0.0f)
        {
            r = 0.0f;
        }
    }

    if (plyr_wrk.spot_rot[0] < r)
    {
        if (plyr_wrk.spot_rot[0] + rcng_adj < r)
        {
            r = plyr_wrk.spot_rot[0] + rcng_adj;
        }
    }
    else
    {
        if (r < plyr_wrk.spot_rot[0] - rcng_adj)
        {
            r = plyr_wrk.spot_rot[0] - rcng_adj;
        }
    }

    plyr_wrk.spot_rot[0] = r;

    rcng_adj = DEG2RAD(1.0f);

    d = pad[0].analog[0];

    if (d >= 48 && d <= 208)
    {
        r = 0.0f;
    }
    else if ((plyr_wrk.mvsta & 0x200000) == 0)
    {
        r = plyr_wrk.spot_rot[0];
        r = ((d - 128) * (PI / 2)) / 127.0f;

        rcng_adj = DEG2RAD(3.0f);

        if (r > DEG2RAD(40.0f))
        {
            r = DEG2RAD(40.0f);
        }
        else if (r < DEG2RAD(-40.0f))
        {
            r = DEG2RAD(-40.0f);
        }

        GetTrgtRot(plyr_wrk.move_box.pos, camera.p, rv, 2);

        rv[1] -= plyr_wrk.move_box.rot[1];

        RotLimitChk(&rv[1]);

        if (__builtin_fabsf(rv[1]) <= DEG2RAD(45.0f))
        {
            r = -r;
        }
    }

    if (plyr_wrk.spot_rot[1] < r)
    {
        if (plyr_wrk.spot_rot[1] + rcng_adj < r)
        {
            r = plyr_wrk.spot_rot[1] + rcng_adj;
        }
    }
    else
    {
        if (r < plyr_wrk.spot_rot[1] - rcng_adj)
        {
            r = plyr_wrk.spot_rot[1] - rcng_adj;
        }
    }

    plyr_wrk.spot_rot[1] = r;
}

void PlyrFinderCtrl()
{
    if (plyr_wrk.sta & 0x1000)
    {
        return;
    }

    plyr_wrk.sta |= 0x10;

    FModeScreenEffect();
    EneFrameHitChk();
    PlyrPhotoChk();
    PlyrSubAtkChk();
    PlyrCamRotCngChk();
    PlyrFModeMoveCtrl();
    PlyrDWalkTmCtrl();
    EneHPchk();
}

void PlyrCamRotCngChk()
{
    if (
        *key_now[4] == 1 &&
#ifdef BUILD_EU_VERSION
        plyr_wrk.cond != 1 &&
#endif
        (plyr_wrk.sta & (0x8000000 | 0x1000 | 0x400)) == 0
    )
    {
        plyr_wrk.sta |= 0x8000000;
        plyr_wrk.move_box.loop = 30;
        plyr_wrk.move_box.rspd[1] = DEG2RAD(6.0f);
    }
}

void EneHPchk()
{
    ENE_WRK *ew;
    float dist[2];
    u_char i;
    u_char no;

    dist[1] = 0.0f;
    no = 0xff;
    ew = ene_wrk;

    for (i = 0; i < 3; i++, ew++)
    {
        if (ew->sta & 1 && ew->hp != 0 && ew->sta & 0x40 && (ew->sta & 0x80000) == 0)
        {
            dist[0] = GetDistV(plyr_wrk.move_box.pos, ew->move_box.pos);

            if (dist[1] == 0.0f || dist[0] < dist[1])
            {
                dist[1] = dist[0];

                no = i;
            }
        }
    }

    plyr_wrk.near_ene_no = no;
}

void FModeScreenEffect()
{
    ENE_WRK *ew;
    float dist[2];
    float alpha;
    u_char i;
    u_char crate;

    for (i = 0; i < 3; i++)
    {
        if (req_dmg_ef[i])
        {
            SetEffects(9, 1, 0, 0x80000);

            return;
        }
    }

    dist[1] = 0.0f;
    ew = ene_wrk;

    for (i = 0; i < 4; i++)
    {
        if (ew->sta & 0x1 && ew->hp != 0 && (ew->sta & 0x80000) == 0)
        {
            dist[0] = GetDistV(plyr_wrk.move_box.pos, ew->move_box.pos);

            if (dist[1] == 0.0f || dist[0] < dist[1])
            {
                dist[1] = dist[0];
            }
        }

        ew++;
    }

    alpha = 53.0f;

    crate = 70;

    if (dist[1] != 0.0f)
    {
        if (dist[1] <= 500.0f)
        {
            alpha = 80.0f;
            crate = 120;
        }
        else if (dist[1] <= 1250.0f)
        {
            alpha = (1250.0f - dist[1]) * 27.0f / 750.0f + 53.0f;
            crate = (int)((1250.0f - dist[1]) * 50.0f / 750.0f) + 70;
        }
    }

    SetEffects(2, 1, 3, alpha, 8.0f, 101, 64);
    SetEffects(14, 1, crate, crate);
}

void PlyrDmgCtrl()
{
    ENE_WRK *ew;
    sceVu0FVECTOR tv;
    u_char n;

    ew = &ene_wrk[plyr_wrk.atk_no];

    switch (dmg_step)
    {
    case 0:
        if (avoid_chk != 0)
        {
            plyr_wrk.dwalk_tm = 0;

            ew->atk_tm = 0;
        }
        else
        {
            n = LeverGachaChk();

            if (ew->sta & 0x8000 && ew->atk_tm)
            {
                if (n != 0 && ew->atk_tm > 22)
                {
                    ew->atk_tm -= 3;
                }

                VibrateRequest1(0, 1);

                if (ew->atk_tm > 20)
                {
                    CallDeform2(0, 0, ew->atk_tm, 7, 13);
                }
            }
        }

        if (ew->atk_tm == 0)
        {
            switch (plyr_wrk.anime_no)
            {
            case PANI_DMG_SHLD_IN:
                SetPlyrAnime(PANI_DMG_SHLD_OUT, 10);

                dmg_step = 1;
            break;
            case PANI_DMG_LEG_IN:
                SetPlyrAnime(PANI_DMG_LEG_OUT, 10);

                dmg_step = 1;
            break;
            default:
                SetPlyrAnime(PANI_DMG_NECK_OUT, 10);

                dmg_step = 1;
            break;
            }

        }
    break;
    case 1:
        if (plyr_wrk.sta & 0x20)
        {
            plyr_wrk.mode = 0;
            dmg_step = 0;
        }
        else if (plyr_wrk.spd)
        {
            tv[0] = 0.0f;
            tv[1] = 0.0f;
            tv[2] = plyr_wrk.spd;
            tv[3] = 0.0f;

            RotFvector(plyr_wrk.move_box.rot,tv);

            PlyrMoveHitChk(&plyr_wrk.move_box,tv, 1);
            PlyrPosSet(&plyr_wrk.move_box,tv);
        }
    break;
    }
}

u_char LeverGachaChk()
{
    u_char result;
    static u_char lever_dir_old = 0xff;

    result = 0;

    if (PlyrLeverInputChk() == 2)
    {
        if (lever_dir_old == pad[0].an_dir[0])
        {
            result = 0;
        }
        else
        {
            lever_dir_old = pad[0].an_dir[0];

            result = 1;
        }
    }

    return result;
}

void PlyrFinderEnd()
{
    if (plyr_wrk.sta & 0x20 || plyr_wrk.anime_no != PANI_CAM_SET_OUT)
    {
        plyr_wrk.mode = PMODE_NORMAL;
    }
}

void PlyrNAnimeCtrl()
{
    u_int psta = plyr_wrk.mvsta;
    u_char anime_no = 0;
    u_char frame = 10;
    u_char rl_chk = 0;
    u_char i;
    u_char j;
    u_int psta_chk_tbl[18] = {
            16,   32,  64,    128,   256,   512,   1024,
          2048, 4096, 8192, 16384, 32768, 65536, 131072,
             1,    2,    4,     8,
    };
    u_char pmani_no_tbl[18][2] = {
        { 49, 48 }, {  51,  50 }, { 53, 52 }, { 54, 55 }, {  70,  69 }, {  72,  71 },
        { 74, 73 }, {  76,  75 }, { 98, 98 }, { 99, 99 }, { 105, 105 }, { 106, 106 },
        { 97, 96 }, { 102, 101 }, {  5,  6 }, {  3,  4 }, {   3,   4 }, {   8,   9 },
    };

    if (psta & 0xffff)
    {
        for (i = 0; i < 18; i++)
        {
            if (psta & psta_chk_tbl[i])
            {
                break;
            }
        }

        for (j = 0; j < 18; j++)
        {
            if (
                (plyr_wrk.anime_no != PANI_UP_LONG_R || plyr_wrk.anime_no != PANI_UP_LONG_L) &&
                (pmani_no_tbl[j][0] == plyr_wrk.anime_no || pmani_no_tbl[j][1] == plyr_wrk.anime_no)
            )
            {
                break;
            }
        }

        if (i != j)
        {
            if (j < 18)
            {
                rl_chk = GetPlyrFtype() == 1;
            }

            if ((psta & 0xfff0) == 0 && j < 8)
            {
                if ((j & 1) != 0)
                {
                    rl_chk ^= 1;
                }

                frame = 20;
            }

            anime_no = pmani_no_tbl[i][rl_chk];
        }
        else
        {
            anime_no = plyr_wrk.anime_no;
        }
    }

    if (anime_no == 0)
    {
        if (psta & 0x40000)
        {
            anime_no = 100;
        }
        else if (psta & 0x80000 || plyr_wrk.hp < 51)
        {
            anime_no = PANI_DMG_STAND;
        }
        else if (time_wrk.no_pad >= 3600 && ingame_wrk.msn_no != 0)
        {
            anime_no = PANI_STAND2;

            frame = 60;
        }
        else if (time_wrk.no_pad >= 1800)
        {
            anime_no = PANI_LOOK_LR;
        }
    }

    if (plyr_wrk.cond == 2)
    {
        anime_no = PANI_DMG_WALK_L;
    }

    if (anime_no == PANI_LAD_DOWN || anime_no == PANI_IDO_DOWN)
    {
        frame = 0;
    }

    if (plyr_wrk.anime_no != anime_no && (psta & 0x200000) == 0)
    {
        SetPlyrAnime(anime_no, frame);
    }
}

void SetPlyrAnime(u_char anime_no, u_char frame)
{
    plyr_wrk.sta &= ~0x20;
    plyr_wrk.anime_no = anime_no;

    ReqPlayerAnime(frame);
}

void PlyrTrembleChk()
{
    FURN_WRK *fw;
    ENE_WRK *ew;
    sceVu0FVECTOR tv;
    float dist;
    float dist_chk;
    u_int fsta;
    u_char i;
    u_char chk;
    u_char no;
    u_short mvib_time_tbl[5] = { 25, 30, 45, 60, 90 };
    u_char mvib_deg_tbl[5] = { 220, 200, 180, 150, 120 };
    static u_short mvib_time0 = 0;
    static u_short mvib_time1 = 0;
    static u_char mvib_degree;

    ew = ene_wrk;
    dist = 0.0f;
    chk = 0;
    no = 0;

    for (i = 0; i < 4; i++)
    {
        if (ew->sta & 0x1 && ew->hp != 0 && (ew->sta & 0x80000) == 0 && (ew->sta & 0x8000) == 0)
        {
            dist_chk = GetDistV2(plyr_wrk.move_box.pos, ew->move_box.pos);

            if (dist == 0.0f || dist_chk < dist)
            {
                dist = dist_chk;

                no = i;
            }
        }

        ew++;
    }

    if (dist >= 1.0f && dist <= 4000.0f)
    {
        chk = 1;

        if (mvib_time1 & 0xffff)
        {
            if (mvib_time1 == 18)
            {
                SeStartFix(15, 0, 0x1000, 0x1000, 0);
            }

            mvib_time1--;

            VibrateRequest2(0, mvib_degree);
        }
        else if (mvib_time0 == 0)
        {
            i = (dist / 500.0f);

            if (i > 4)
            {
                i = 4;
            }

            mvib_time0 = mvib_time_tbl[i];

            mvib_time1 = 18;

            mvib_degree = mvib_deg_tbl[i];

            if (ene_wrk[no].type == ET_AUTO)
            {
                mvib_degree /= 2;
            }
        } else {

            mvib_time0--;
        }
    }

    if (!chk)
    {
        for (dist = 0.0f, i = 0; i < 3; i++)
        {
            if (rg_dsp_wrk[i].mode != 0)
            {
                dist_chk = GetDistV(plyr_wrk.move_box.pos, &rg_dsp_wrk[i].pos[0]);

                if (dist_chk <= 5000.0f)
                {
                    if (dist == 0.0f || dist_chk < dist)
                    {
                        dist = dist_chk;
                    }
                }
            }
        }

        if (dist == 0.0f)
        {
            dist = 0.0f;
            fw = furn_wrk;

            for (i = 0; i < 60; i++)
            {
                if (GetFurnHintPos(fw, tv, &fsta) != 0)
                {
                    dist_chk = GetDistV(plyr_wrk.move_box.pos, tv);

                    if (dist_chk <= 2500.0f)
                    {
                        if (dist == 0.0f || dist_chk < dist)
                        {
                            dist = dist_chk;
                        }
                    }
                }

                fw++;
            }
        }

        if (dist >= 1.0f && dist <= 2500.0f)
        {
            if (mvib_time1 != 0)
            {
                mvib_time1--;

                VibrateRequest2(0, mvib_degree);
            }
            else if (mvib_time0 == 0)
            {
                mvib_time1 = 13;
                mvib_time0 = 150;
                mvib_degree = 115;
            }
            else
            {
                mvib_time0--;
            }
        }
    }
}

void ReqPlyrHPdown(u_short deg, u_char prio)
{
    if (dbg_wrk.param_muteki == 0)
    {
        if (((plyr_wrk.sta & 0x800) == 0 || prio != 0) && (plyr_wrk.sta & 0x4000) == 0)
        {
            plyr_wrk.sta |= 0x800;

            hp_down_deg = deg;
        }
    }
}

void PlyrHPdwonCtrl()
{
    u_short down;

    if (plyr_wrk.sta & 0x800)
    {
        if (hp_down_deg > 2)
        {
            down = 2;

            hp_down_deg -= 2;
        }
        else
        {
            down = hp_down_deg;

            plyr_wrk.sta &= ~0x800;
        }

        plyr_wrk.hp = plyr_wrk.hp < down ? 0 : plyr_wrk.hp - down;

        if (plyr_wrk.hp != 0)
        {
            return;
        }

        if (plyr_wrk.hp == 0)
        {
            if (poss_item[8] > 0)
            {
                poss_item[8] -= 1;

                plyr_wrk.sta &= ~0x800;
                plyr_wrk.sta |= 0x4000;

                SeStartFix(6, 0, 0x1000, 0x1000, 0);

                hp_down_deg = 0;
            }
            else if (ingame_wrk.game == 1)
            {
                CallMissionFailed();
            }
            else
            {
                plyr_wrk.sta &= ~0x800;

                if (plyr_wrk.se_deadly != -1)
                {
                    SeFadeFlame(plyr_wrk.se_deadly, 60, 0);
                }

                SetPlyrAnime(64, 10);
                StartGameOver();

            }
        }
    }
    else if (plyr_wrk.sta & 0x4000)
    {
        plyr_wrk.hp += 5;

        if (plyr_wrk.hp >= 500)
        {
            plyr_wrk.sta &= ~0x4000;
            plyr_wrk.hp = 500;
        }
    }
}

void PlyrSpotLightOnChk()
{
    SetPlyrSpotLight(1);

    if (plyr_wrk.mode == PMODE_NORMAL && plyr_wrk.anime_no != PANI_CAM_SET_OUT)
    {
        SetEffects(10, 1, 4, plyr_wrk.spot_pos, plyr_wrk.move_box.rot);
    }
}

void SetPlyrSpotLight(u_char id)
{
    SPOT_WRK ts0;
    SPOT_WRK ts1;
    sceVu0FVECTOR tv;
    sceVu0FVECTOR rv;
    u_char i;

    if (id != 0)
    {
        if (plyr_wrk.mode == PMODE_FINDER)
        {
            ts0.pos[0] = camera.p[0];
            ts0.pos[1] = camera.p[1];
            ts0.pos[2] = camera.p[2];
            ts0.pos[3] = camera.p[3];

            tv[0] = 0.0f;
            tv[1] = 0.0f;
            tv[2] = -150.0f;
            tv[3] = 0.0f;

            RotFvector(plyr_wrk.move_box.rot, tv);

            sceVu0AddVector(ts1.pos, camera.p, tv);
            sceVu0SubVector(ts0.direction, camera.p, camera.i);

            ts1.direction[0] = ts0.direction[0];
            ts1.direction[1] = ts0.direction[1];
            ts1.direction[2] = ts0.direction[2];
            ts1.direction[3] = ts0.direction[3];

            ts0.diffuse[0] = 0.22f;
            ts0.diffuse[1] = 0.21f;
            ts0.diffuse[2] = 0.2f;
            ts0.diffuse[3] = 255.0f;

            ts1.diffuse[0] = 0.07333333f;
            ts1.diffuse[1] = 0.07f;
            ts1.diffuse[2] = 0.06666667f;
            ts1.diffuse[3] = 255.0f;

            ts0.intens = 0.7f;
            ts1.intens = 0.4f;
        }
        else
        {
            tv[0] = 0.0f;
            tv[1] = 0.0f;
            tv[2] = -300.0f;
            tv[3] = 0.0f;

            RotFvector(plyr_wrk.move_box.rot, tv);

            sceVu0AddVector(ts0.pos, plyr_wrk.spot_pos, tv);

            tv[0] = 0.0f;
            tv[1] = 0.0f;
            tv[2] = -150.0f;
            tv[3] = 0.0f;

            RotFvector(plyr_wrk.move_box.rot, tv);

            sceVu0AddVector(ts1.pos, ts0.pos, tv);

            rv[0] = plyr_wrk.spot_rot[0];
            rv[1] = plyr_wrk.spot_rot[1];
            rv[2] = plyr_wrk.spot_rot[2];
            rv[3] = plyr_wrk.spot_rot[3];

            rv[1] += plyr_wrk.move_box.rot[1];

            RotLimitChk(&rv[1]);

            ts0.direction[0] = 0.0f;
            ts0.direction[1] = -800.0f;
            ts0.direction[2] = -3000.0f;
            ts0.direction[3] = 0.0f;

            RotFvector(rv,ts0.direction);

            ts1.direction[0] = ts0.direction[0];
            ts1.direction[1] = ts0.direction[1];
            ts1.direction[2] = ts0.direction[2];
            ts1.direction[3] = ts0.direction[3];

            ts0.diffuse[0] = 0.22f;
            ts0.diffuse[1] = 0.20999999f;
            ts0.diffuse[2] = 0.2f;
            ts0.diffuse[3] = 255.0f;

            ts0.diffuse[2] += 0.0f; // HACK: fixes stack

            ts1.diffuse[0] = 0.176f;
            ts1.diffuse[1] = 0.168f;
            ts1.diffuse[2] = 0.16f;
            ts1.diffuse[3] = 255.0f;

            ts0.intens = 0.69999999f;
            ts1.intens = 0.4f;
        }

        sceVu0ScaleVector(ts0.diffuse, ts0.diffuse, 0.001f);
        sceVu0ScaleVector(ts1.diffuse, ts1.diffuse, 0.001f);

        ts0.power = 7000000.0f;
        ts1.power = 7000000.0f;
    }

    for (i = 0; i < 2; i++)
    {
        if (id != 0)
        {
            if (room_wrk.mylight[i].spot_num < 2)
            {
                room_wrk.mylight[i].spot_num += 2;
            }
        }
        else
        {
            if (room_wrk.mylight[i].spot_num >= 2)
            {
                room_wrk.mylight[i].spot_num -= 2;
            }
        }

        room_wrk.mylight[i].spot[0] = ts0;
        room_wrk.mylight[i].spot[1] = ts1;
    };

    for (i = 0; i < 60; i++)
    {
        if (furn_wrk[i].furn_no == 0xffff)
        {
            break;
        }

        if (id != 0)
        {
            if (furn_wrk[i].mylight.spot_num < 1)
            {
                furn_wrk[i].mylight.spot_num += 1;
            }
        }
        else
        {
            if (furn_wrk[i].mylight.spot_num >= 1)
            {
                furn_wrk[i].mylight.spot_num -= 1;
            }
        }

        furn_wrk[i].mylight.spot[0] = ts0;
    }

    for (i = 0; i < 4; i++)
    {
        if (ene_wrk[i].sta & 0x1)
        {
            if (id != 0)
            {
                if (ene_wrk[i].mylight.spot_num < 1)
                {
                    ene_wrk[i].mylight.spot_num += 1;
                }
            }
            else
            {
                if (ene_wrk[i].mylight.spot_num >= 1)
                {
                    ene_wrk[i].mylight.spot_num -= 1;
                }
            }

            ene_wrk[i].mylight.spot[0] = ts0;
            ene_wrk[i].mylight.spot[0].power *= 0.01f;
        }
    }
}

void PlyrSubAtkChk()
{
    u_char hit = 0xff;
    int sub_se_tbl[4] = { 13, 24, 9, 24 };

    if (*key_now[8] == 1 && cam_custom_wrk.set_sub != 0xff && poss_item[5] != 0)
    {
        if (plyr_wrk.cond != 1)
        {
            poss_item[5] -= 1;

            switch(cam_custom_wrk.set_sub)
            {
            case 0:
                hit = PSAchk0();
            break;
            case 1:
                hit = PSAchk1();
            break;
            case 2:
                hit = PSAchk2();
            break;
            case 3:
                hit = PSAchk3();
            break;
            case 4:
                PSAchk4(0);
            break;
            }

            if (hit != 0xff)
            {
                if (hit != 0)
                {
                    SeStartFix(sub_se_tbl[cam_custom_wrk.set_sub], 0, 0x1000, 0x1000, 0);
                }
                else
                {
                    SeStartFix(12, 0, 0x1000, 0x1000, 0);
                }
            }
        }
    }

    if (cam_custom_wrk.set_spe != 0xff)
    {
        if (cam_custom_wrk.set_spe == 0)
        {
            PSAchk5();
        }
        else if (cam_custom_wrk.set_spe == 1)
        {
            if (*key_now[9] != 0 || *key_now[11] != 0)
            {
                if (plyr_wrk.cond != 1)
                {
                    PSAchk4(1);
                }
            }
        }
    }
}

u_char PSAchk0()
{
    ENE_WRK *ew;
    u_char i;
    u_char chk;

    ew = ene_wrk;

    for (i = 0, chk = 0; i < 3; i++)
    {
        if (
            ew->sta & 0x1 && ew->hp != 0 &&
            (ew->sta & 0x80000000) == 0 && (ew->sta & 0x80000) == 0 && ew->sta & 0x400 &&
            (ingame_wrk.msn_no != 4 || ew->dat_no != 2) &&
            (ew->stm_slow == 0 && ew->stm_view == 0 && ew->stm_stop == 0)
        )
        {
            EneActSet(ew, 10);
            SetCamPush(i);
            chk = 1;
        }

        ew++;
    }

    if (!chk)
    {
        SetCamPush(-1);
    }

    return chk;
}

u_char PSAchk1()
{
    ENE_WRK *ew;
    u_char i;
    u_char chk;

    chk = 0;
    ew = ene_wrk;

    for(i = 0; i < 3; i++)
    {
        if (
            ew->sta & 0x1 && ew->hp != 0 &&
            ((ew->sta & 0x80000000) == 0 && (ew->sta & 0x80000) == 0 && ew->sta & 0x400) &&
            (ingame_wrk.msn_no != 4 || ew->dat_no != 2) && ew->act_no != 10 &&
            (ew->stm_view == 0 && ew->stm_stop == 0)
        )
        {
            ew->stm_slow = 600;

            SetCamSlow(i);

            chk = 1;
        }

        ew++;
    }

    if (!chk)
    {
        SetCamSlow(-1);
    }

    return chk;
}

u_char PSAchk3()
{
    ENE_WRK *ew;
    u_char i;
    u_char chk;

    chk = 0;
    ew = ene_wrk;

    for (i = 0; i < 3; i++)
    {
        if (
            ew->sta & 0x1 && ew->hp != 0 &&
            (ew->sta & 0x80000000) == 0 && (ew->sta & 0x80000) == 0 && (ew->sta & 0x400) &&
            (ingame_wrk.msn_no != 4 || ew->dat_no != 2) && ew->act_no != 10 &&
            ew->stm_slow == 0 && ew->stm_view == 0
        )
        {
            ew->stm_stop = 180;
            ene_wrk[i].sta |= 0x10000000;

            SetCamStop(i);

            chk = 1;
        }

        ew++;
    }

    if (!chk)
    {
        SetCamStop(-1);
    }

    return chk;

}

u_char PSAchk2()
{
    ENE_WRK *ew;
    u_char i;
    u_char chk;

    chk = 0;
    ew = ene_wrk;

    for (i = 0; i < 3; i++)
    {
        if (
            ew->sta & 0x1 && ew->hp != 0 &&
            (ew->sta & 0x80000000) == 0 && (ew->sta & 0x80000) == 0 && (ew->sta & 0x20) &&
            (ingame_wrk.msn_no != 4 || ew->dat_no != 2) && ew->act_no != 10 &&
            ew->stm_slow == 0 && ew->stm_stop == 0
        )
        {
            ew->stm_view = 600;
            ew->tr_rate = 80;

            SetCamView(i);

            chk = 1;
        }

         ew++;
    }

    if (!chk)
    {
        SetCamView(-1);
    }

    return chk;

}

void PSAchk4(u_char id)
{
    sceVu0FVECTOR tv;
    sceVu0FVECTOR tr;
    sceVu0FVECTOR rv;
    ENE_WRK *ew;
    float dist[2]; float x;
    float rot;    /* s0 16 */
    u_char i;
    u_char j;
    u_char trgt;

    if (plyr_wrk.sta & 0x400)
    {
        return;
    }

    for (i = 0, j = 0, dist[1] = 0.0f, trgt = 0xff, ew = ene_wrk; i < 3; i++, j++, ew++)
    {
        if (
            ew->sta & 0x1 && ew->hp != 0 &&
            (ew->sta & 0x80000000) == 0 && (ew->sta & 0x80000) == 0 &&
            (ingame_wrk.msn_no != 4 || ew->dat_no != 2 || id)
        )
        {
            dist[0] = GetDistV(plyr_wrk.move_box.pos, ew->move_box.pos);

            if (dist[1] == 0.0f || dist[0] < dist[1])
            {
                trgt = i;

                dist[1] = dist[0];
            }
        }
    }

    if (trgt != 0xff)
    {
        ew = &ene_wrk[trgt];

        rot = SgAtan2f(-15.0f, 224.0f / SgSinf(camera.fov * 0.5f));

        GetTrgtRot(camera.p, ew->mpos.p0, tr, 3);

        tr[0] -= plyr_wrk.frot_x;
        tr[0] += rot;

        RotLimitChk(tr);

        tr[1] -= plyr_wrk.move_box.rot[1];

        RotLimitChk(&tr[1]);

        plyr_wrk.move_box.loop = GetDist(tr[0], tr[1]) / (PI / 1000.0f);

        if (plyr_wrk.move_box.loop != 0)
        {
            plyr_wrk.sta |= 0x400;

            plyr_wrk.move_box.rspd[0] = tr[0] / plyr_wrk.move_box.loop;
            plyr_wrk.move_box.rspd[1] = tr[1] / plyr_wrk.move_box.loop;

            rv[0] = 0;
            rv[1] = (__builtin_fabsf(tr[1]) < __builtin_fabsf(tr[0])) ? __builtin_fabsf(tr[0]) : __builtin_fabsf(tr[1]);
            rv[2] = (__builtin_fabsf(tr[1]) < __builtin_fabsf(tr[0])) ? __builtin_fabsf(plyr_wrk.move_box.rspd[0]) : __builtin_fabsf(plyr_wrk.move_box.rspd[1]);

            j = 0;

            while (1)
            {
                rv[0] += rv[2];
                rv[2] *= 1.1f;

                if (rv[1] < rv[0])
                {
                    break;
                }

                j++;
            }

            plyr_wrk.move_box.loop = j;

            if (id == 0)
            {
                SetCamSearch(trgt);
            }
        }
    }
    else
    {
        if (id == 0)
        {
            SetCamSearch(-1);
        }
    }
}

void PSAchk5()
{
    ENE_WRK *ew;
    sceVu0FVECTOR rv;
    float dist[2];
    u_char i;
    u_char t;

    plyr_wrk.spe1_dir = 0;

    dist[1] = 0.0f;
    t = 0xff;
    ew = ene_wrk;

    for (i = 0; i < 4; i++)
    {
        if (ew->sta & 1 && ew->hp != 0 && (ew->sta & 0x80000) == 0)
        {
            dist[0] = GetDistV(camera.p, ew->mpos.p0);

            if (dist[1] == 0.0f || dist[0] < dist[1])
            {

                dist[1] = dist[0];

                t = i;
            }
        }

        ew++;
    }

    if (t == 0xff)
    {
        return;
    }

    ew = &ene_wrk[t];

    if (ew->sta & 0x20)
    {
        return;
    }

    GetTrgtRot(camera.p, ew->mpos.p0, rv, 3);

    rv[1] -= plyr_wrk.move_box.rot[1];

    RotLimitChk(&rv[1]);

    rv[0] -= plyr_wrk.frot_x;

    RotLimitChk(&rv[0]);

    if (__builtin_fabsf(rv[0]) < __builtin_fabsf(rv[1]))
    {
        if (rv[1] > 0.0f)
        {
            plyr_wrk.spe1_dir |= 2;
        }
        else
        {
            plyr_wrk.spe1_dir |= 8;
        }
    }

    else if (rv[0] > 0.0f)
    {
        plyr_wrk.spe1_dir |= 1;
    }
    else
    {
        plyr_wrk.spe1_dir |= 4;
    }
}

void PlyrPhotoChk()
{
    u_short pad_shutter;
    int se_no;

    pad_shutter = 0;

    if (no_photo_tm != 0)
    {
        no_photo_tm -= 1;

        return;
    }

    if (opt_wrk.key_type < 2 || opt_wrk.key_type == 4 || opt_wrk.key_type == 5)
    {
        pad_shutter = *key_now[10];
    }
    else
    {
        pad_shutter = *key_now[7];
    }

    if (*key_now[5] != 1 && pad_shutter != 1)
    {
        return;
    }

    if (
        (photo_wrk.mode == 0 && plyr_wrk.cond != 1) &&
        (poss_item[plyr_wrk.film_no] != 0 || cam_custom_wrk.set_spe == 3)
    )
    {
        PhotoFlyChk();
        PhotoDmgChk();
        PhotoPointSet();

        if (plyr_wrk.sta & 0x8000000)
        {
            plyr_wrk.move_box.rspd[0] = 0.0f;
            plyr_wrk.move_box.rspd[1] = 0.0f;
            plyr_wrk.move_box.rspd[2] = 0.0f;
            plyr_wrk.move_box.rspd[3] = 0.0f;

            plyr_wrk.sta &= ~0x8000000;
            plyr_wrk.move_box.loop = 0;
        }

        switch(plyr_wrk.charge_num)
        {
        case 4:
        case 5:
        case 6:
            SeStartFix(0x15, 0, 0x1000, 0x1000, 0);
        break;
        case 7:
        case 8:
        case 9:
            SeStartFix(0x16, 0, 0x1000, 0x1000, 0);
        break;
        case 10:
        case 11:
        case 12:
            SeStartFix(0x17, 0, 0x1000, 0x1000, 0);
        break;
        default:
            SeStartFix(0x14, 0, 0x1000, 0x1000, 0);
        break;
        }

        PlyrChargeCtrl(255, NULL);

        if (cam_custom_wrk.set_spe != 3)
        {
            poss_item[plyr_wrk.film_no] -= 1;
        }

        plyr_wrk.ap_timer = 120;
        plyr_wrk.aphoto_tm = 600;
    }
    else if (poss_item[plyr_wrk.film_no] == 0)
    {
        SeStartFix(5, 0, 0x1000, 0xa00, 0);
    }
}

void EneFrameHitChk()
{
    ENE_WRK *ew;
    sceVu0FVECTOR tv;
    sceVu0FVECTOR rv;
    PP_JUDGE ppj;
    float dpe;
    float dce;
    float tx;
    float ty;
    u_char i;
    u_char chk;
    u_char chk2;

    chk2 = 0;
    chk = 0;
    ew = ene_wrk;

    for (i = 0; i < 4; i++, ew++)
    {
        if (ew->sta & 1 && ew->hp != 0 && (ew->sta & 0x80000) == 0)
        {
            chk2 = 1;

            dpe = ew->dist_p_e = GetDistV(plyr_wrk.move_box.pos, ew->move_box.pos);

            if (
                OutSightChk(ew->mpos.p0, camera.p, plyr_wrk.move_box.rot[1], DEG2RAD(120.0f), photo_rng_tbl[plyr_wrk.cam_type]) == 0 &&
                FrameInsideChk(ew->mpos.p0, &tx, &ty)
            )
            {
                ene_wrk[i].sta |= 0x20;

                chk = 1;

                if (ew->in_finder_tm < 0xffff)
                {
                    ew->in_finder_tm++;
                }

                ew->fp[0] = tx;
                ew->fp[1] = ty;

                dce = GetDist(tx -  plyr_wrk.fp[0], ty - plyr_wrk.fp[1]);

                if (dce <= circle_radius_tbl[cam_custom_wrk.charge_range])
                {
                    ene_wrk[i].sta |= 0x40;
                    ew->dist_c_e = dce;

                    if (ew->dist_c_e <= 25.0f)
                    {
                        ene_wrk[i].sta |= 0x100;
                    }
                }

                if ((ew->sta & 0x1000000) == 0)
                {
                    ppj.num = 1;
                    ppj.result[0] = 0;

                    ppj.p[0][0] = ew->mpos.p0[0];
                    ppj.p[0][1] = ew->mpos.p0[1];
                    ppj.p[0][2] = ew->mpos.p0[2];
                    ppj.p[0][3] = ew->mpos.p0[3];

                    tv[2] = 200.0f;
                    tv[0] = 0.0f;
                    tv[1] = 0.0f;
                    tv[3] = 0.0f;

                    GetTrgtRot(ppj.p[0], camera.p, rv, 3);
                    RotFvector(rv, tv);

                    sceVu0AddVector(ppj.p[0], ppj.p[0], tv);

                    CheckPointDepth(&ppj);

                    if (ppj.result[0] != 0 || dpe <= 300.f)
                    {
                          ene_wrk[i].sta |= 0x200;
                    }
                }

                if (ew->sta & 0x40 || (dpe <= 510.0f && ew->sta & 0x20))
                {
                    if (ew->sta & 0x200 && ew->tr_rate != 0)
                    {
                        ene_wrk[i].sta |= 0x400;

                        if (ew->type == 0 || ew->type == 1)
                        {
                            PlyrChargeCtrl(1, ew);
                        }
                        else
                        {
                            plyr_wrk.charge_rate = 3.0f;
                        }
                    }
                }
            }
            else
            {
                ew->dist_p_e = 0.0f;
                ew->in_finder_tm = 0;
            }
        }
    }

    if (chk == 0 || chk2 == 0)
    {
        PlyrChargeCtrl(0, NULL);
    }
}

void PlyrChargeCtrl(u_char req, ENE_WRK *ew)
{
    u_short ctime[4] = { 75, 70, 60, 50 };
    float cldown[4] = { 60.0f, 70.0f, 80.0f, 100.0f };
    float dpe;
    float rate0;
    float rate1;
    u_char cn = plyr_wrk.charge_num;
    u_char cl_max;

    switch (req)
    {
    case 0xff:
        plyr_wrk.charge_deg = 0.0f;
        plyr_wrk.charge_rate = 0.0f;
        plyr_wrk.charge_num = ini_charge_tbl[plyr_wrk.film_no];

        if (cam_custom_wrk.set_spe == 4)
        {
            plyr_wrk.charge_num = 12;
        }
    break;
    case 0:
        cn = ini_charge_tbl[plyr_wrk.film_no];

        if (cam_custom_wrk.set_spe == 4)
        {
            cn = 12;
        }

        plyr_wrk.charge_rate = 0.0f;

        if (plyr_wrk.charge_num == cn)
        {
            plyr_wrk.charge_deg = 0.0f;
        }
        else
        {
            if (plyr_wrk.charge_deg == plyr_wrk.charge_rate)
            {
                plyr_wrk.charge_deg = cldown[cam_custom_wrk.charge_speed];
                plyr_wrk.charge_num -= 1;

                break;
            }

            if (!(plyr_wrk.charge_deg >= 1.0f && plyr_wrk.charge_deg <= cldown[cam_custom_wrk.charge_speed]))
            {
                plyr_wrk.charge_deg = cldown[cam_custom_wrk.charge_speed];
            }

            plyr_wrk.charge_deg -= 1.0f;
        }
    break;
    default:
        dpe = ew->dist_p_e;

        if (plyr_wrk.film_no == 0)
        {
            cl_max = 1;
        }
        else
        {
            cl_max = charge_max_tbl[cam_custom_wrk.charge_max];
        }

        if (cn >= cl_max)
        {
            plyr_wrk.charge_rate = 4.5f;
        }
        else
        {
            if ((u_short)plyr_wrk.charge_deg >= ctime[cam_custom_wrk.charge_speed])
            {
                plyr_wrk.charge_deg = 0.0f;
                plyr_wrk.charge_num += 1;

                if (plyr_wrk.charge_num >= cl_max)
                {
                    plyr_wrk.charge_rate = 4.5f;

                    SeStartFix(19, 0, 0x1000, 0x1000, 0);
                }
                else
                {
                    SeStartFix(18, 0, 0x1000, 0x1000, 0);
                }
            }
            else
            {
                rate0 = 3.0f;
                rate1 = 0.8f;

                if (dpe <= 2000.0f)
                {
                    rate1 = ((2000.0f - dpe) * 0.7f) / 2000.0f + rate1;
                }

                plyr_wrk.charge_rate = rate1 * rate0;
                plyr_wrk.charge_deg += plyr_wrk.charge_rate;
            }
        }
    break;
    }
}

void PhotoFlyChk(void)
{
    FLY_WRK* fw;
    float tx;
    float ty;
    u_char i;
    float dist;

    for (i = 0, fw = fly_wrk; i < 10; i++, fw++)
    {
        if (fw->sta & 0x1 && fw->sta & 0x8)
        {
            dist = GetDistV(plyr_wrk.move_box.pos, fw->move_box.pos);

            if (dist <= photo_rng_tbl[plyr_wrk.cam_type])
            {
                if (
                    OutSightChk(fw->move_box.pos, camera.p,plyr_wrk.move_box.rot[1], DEG2RAD(120.0f), 5000.0f) == 0 &&
                    FrameInsideChk(fw->move_box.pos, &tx, &ty)
                )
                {
                    fw->sta |= 2;
                }
            }
        }
    }
}

int FrameInsideChk(float *tv, float *tx, float *ty)
{
    float minx;
    float maxx;
    float miny;
    float maxy;
    int result;

    result = 0;

    GetCamI2DPos(tv, tx, ty);

    ty[0] *= 2.0f;

    minx = plyr_wrk.fp[0] - photo_frame_tbl[plyr_wrk.cam_type][0] * 0.5f;
    maxx = plyr_wrk.fp[0] + photo_frame_tbl[plyr_wrk.cam_type][0] * 0.5f;
    miny = plyr_wrk.fp[1] - photo_frame_tbl[plyr_wrk.cam_type][1] * 0.5f;
    maxy = plyr_wrk.fp[1] + photo_frame_tbl[plyr_wrk.cam_type][1] * 0.5f;

    if (minx <= tx[0] && tx[0] <= maxx && miny <= ty[0] && ty[0] <= maxy)
    {
        result = 1;
    }

    return result;
}

int FrameInsideChkFurn(FURN_WRK *fw, float *degree, u_int fsta)
{
    PP_JUDGE ppj;
    sceVu0FVECTOR fpc;
    int i;
    int pchk;
    float magnify;
    int ret;
    float tx;
    float ty;

    pchk = 0;
    magnify = 0.0f;
    ret = 0;

    for (i = 0; i < furn_dat[fw->id].fp_num; i++)
    {
        fpc[0] = fw->pos[0] + fpc_dat[furn_dat[fw->id].fp_no][i][0];
        fpc[1] = fw->pos[1] + fpc_dat[furn_dat[fw->id].fp_no][i][1];
        fpc[2] = fw->pos[2] + fpc_dat[furn_dat[fw->id].fp_no][i][2];

        if (OutSightChk(fpc, camera.p, plyr_wrk.move_box.rot[1], PI, 5000.0f) == 0 && FrameInsideChk(fpc, &tx, &ty) != 0)
        {
            if (fsta & 0x2)
            {
                pchk = 1;
            }
            else
            {
                ppj.num = 1;
                ppj.result[0] = 0;

                ppj.p[0][0] = fpc[0];
                ppj.p[0][1] = fpc[1];
                ppj.p[0][2] = fpc[2];
                ppj.p[0][3] = fpc[3];

                CheckPointDepth(&ppj);

                if (ppj.result[0] != 0)
                {
                    pchk = 1;
                }
            }

            if (pchk)
            {
                magnify += fpc_dat[furn_dat[fw->id].fp_no][i][3];

                ret = 1;
            }
        }
    }

    *degree = magnify;

    return ret;
}

int FrameInsideChkRare(int wrk_no, float *degree)
{
    int i;
    int ret;
    float tx;
    float ty;
    sceVu0FVECTOR rpc;
    float magnify;
    int rg_no;

    ret = 0;

    rg_no = rg_dsp_wrk[wrk_no].rg_no;

    magnify = 0.0f;

    if (rg_no < 128)
    {
        for (i = 0; i < rg_dat[rg_no].center_num; i++)
        {
            rpc[0] = rg_dsp_wrk[wrk_no].pos[0] + rgc_dat[rg_dat[rg_no].center_no][i][0];
            rpc[1] = rg_dsp_wrk[wrk_no].pos[1] + rgc_dat[rg_dat[rg_no].center_no][i][1];
            rpc[2] = rg_dsp_wrk[wrk_no].pos[2] + rgc_dat[rg_dat[rg_no].center_no][i][2];

            if (OutSightChk(rpc, camera.p, plyr_wrk.move_box.rot[1], PI, rg_ap_dat[rg_no].ap_far) == 0)
            {
                if (FrameInsideChk(rpc, &tx, &ty))
                {
                    if (circle_radius_tbl[cam_custom_wrk.charge_range] >= GetDist(plyr_wrk.fp[0] - tx, plyr_wrk.fp[1] - ty))
                    {
                         magnify += rgc_dat[rg_dat[rg_no].center_no][i][3];

                         ret = 1;
                    }
                }
            }
        }
    }
    else
    {
        rpc[0] = rg_dsp_wrk[wrk_no].pos[0];
        rpc[1] = rg_dsp_wrk[wrk_no].pos[1];
        rpc[2] = rg_dsp_wrk[wrk_no].pos[2];

        rg_no = OutSightChk(rpc, camera.p, plyr_wrk.move_box.rot[1], PI, 5000.0f);

        if (rg_no == 0)
        {
            if (
                FrameInsideChk(rpc, &tx, &ty) != 0 &&
                circle_radius_tbl[cam_custom_wrk.charge_range] >= GetDist(plyr_wrk.fp[0] - tx, plyr_wrk.fp[1] - ty)
            )
            {
                ret = 1;

                magnify = 1.0f;
            }
        }
    }

    *degree = magnify;

    return ret;
}

u_short PhotoDmgChk()
{
    ENE_WRK *ew;
    u_short dmg;
    int i;

    plyr_wrk.sta &= ~0x100;

    ew = ene_wrk;

    dmg = 0;

    for (i = 0; i < 4; i++, ew++)
    {
        if (ew->sta & 0x400)
        {
            if (ew->type == 2)
            {
                ene_wrk[i].sta |=  0x800;
                ene_wrk[i].sta |=  0x80000;
            }
            else
            {
                dmg += PhotoDmgChkSub(ew);
            }
        }
    }

    if (dmg != 0)
    {
        point_get_end = 0;
    }

    return dmg;
}

u_short PhotoDmgChkSub(ENE_WRK *ew)
{
    u_int i;
    u_char film_up_tbl[5] = { 0, 0, 2, 3, 4 };

    i = plyr_wrk.charge_num;

    if (ew->sta & 0x20000)
    {
        i += 3;
    }

    if (ew->sta & 0x100)
    {
        i += 2;
    }

    i += film_up_tbl[plyr_wrk.film_no];

    if (i > 21)
    {
        i = 21;
    }

    ew->dmg = photo_dmg_tbl[i];

    if (ew->hp <= ew->dmg)
    {
        plyr_wrk.sta |= 0x100;

        if (ingame_wrk.ghost_cnt < 50000)
        {
            ingame_wrk.ghost_cnt += 1;
        }
    }

    ene_wrk[ew->move_box.idx].sta &= ~0x800;

    if (ew->sta & 0x20000)
    {
        ene_wrk[ew->move_box.idx].sta |= 0x800;

        if (charge_max_tbl[cam_custom_wrk.charge_max] == plyr_wrk.charge_num)
        {
            ew->dmg_type = 3;
        }
        else
        {
            ew->dmg_type = 2;
        }
    }
    else
    {
        ew->dmg_type = 0;
    }

    return ew->dmg;
}

void PhotoPointSet()
{
    InitPhotoWrk();
    PhotoPointChkEne();
    PhotoPointChkFurn();
    PhotoPointChkRare();

    photo_wrk.room = plyr_wrk.pr_info.room_no;
}

void PhotoPointChkEne()
{
    u_char i;
    u_char cnt;

    for (cnt = 0, i = 0; i < 4; i++)
    {
        if (ene_wrk[i].sta & 0x400)
        {
            PhotoPointCulcEne(&ene_wrk[i], &photo_wrk);

            cnt += 1;
        }
    }

    if (cnt >= 3)
    {
        photo_wrk.bonus_sta |= 0x10;
    }
    else if (cnt >= 2)
    {
        photo_wrk.bonus_sta |= 0x8;
    }
}

void PhotoPointChkFurn()
{
    PHOTO_WRK *pw;
    FURN_WRK *fw;
    sceVu0FVECTOR tv;
    u_int sta;
    u_int i;
    float dist;
    float degree;

    pw = &photo_wrk;

    if (plyr_wrk.fh_no != 0xffff)
    {
        fw = &furn_wrk[plyr_wrk.fh_no];

        dist = GetDistV(fw->pos, plyr_wrk.move_box.pos);
        sta = GetFurnAttrF(fw, ingame_wrk.msn_no);
        PhotoPointCulcFurn(fw, pw, dist, plyr_wrk.fh_deg, sta);
    }

    for (i = 0, fw = furn_wrk; i < 60; i++, fw++)
    {
        if (GetFurnHintPos(fw, tv, &sta) == 0)
        {
            sta = GetFurnAttrF(fw, ingame_wrk.msn_no);

            if (sta & 0x80 && (fw->furn_no < 1000 != 0 || fw->furn_no == 0xffff))
            {
                dist = GetDistV(fw->pos, plyr_wrk.move_box.pos);

                if (
                    furn_dat[fw->id].dist_n <= dist && dist <= furn_dat[fw->id].dist_f &&
                    FrameInsideChkFurn(fw, &degree, sta) != 0
                )
                {
                    PhotoPointCulcFurn(fw, pw, dist, degree, sta);
                }
            }
        }
    }
}

void PhotoPointChkRare()
{
    u_int i;
    float degree;

    for (i = 0; i < 3; i++)
    {
        if (
            rg_dsp_wrk[i].mode != 0 && rg_dsp_wrk[i].alpha >= 0x64 &&
            FrameInsideChkRare(i, &degree) != 0
        )
        {
            PhotoPointCulcRare(i, &photo_wrk, GetDistV(rg_dsp_wrk[i].pos, plyr_wrk.move_box.pos), degree);
            RareGhostDispEndSet(i);
        }
    }
}

void PhotoPointCulcEne(ENE_WRK *ew, PHOTO_WRK *pw)
{
    SUBJECT_WRK *sw;
    int i;
    int point;
    float point_plus;
    float ratio;
    float ratio_tmp;
    float dist;

    sw = pw->ene;

    for (i = 0; i < 4; i++, sw++)
    {
        if (sw->no == 0xffff)
        {
            break;
        }
    }

    if (i >= 4)
    {
        return;
    }

    switch (ew->type)
    {
    case 0:
    case 1:
        sw->dmg = ew->dmg;

        if (ew->sta & 0x80000000 && (ew->sta & 0x800) == 0)
        {
            pw->falth = 0x1;
        }

        point = plyr_wrk.charge_num;

        if (cam_custom_wrk.set_spe == 4)
        {
            point = plyr_wrk.charge_num + 3;
        }

        point_plus = 0.0f;

        ratio = 1.0f;

        if (ew->dist_p_e <= ew->dat->hit_rng)
        {
            point++;

            dist = 1.8f;

            pw->bonus_sta |= 0x2;

        }
        else
        {
            dist = ew->dist_p_e - ew->dat->hit_rng;

            if (dist <= 1000.0f)
            {
                dist = ((1000.0f - dist) / 1000.0f) * 0.6f + 1.0f;
            }
            else
            {
                if (2000.0f < dist)
                {
                    dist = 2000.0f;
                }

                dist = ((2000.0f - dist) / 1000.0f) * 0.5f + 0.5f;
            }
        }

        ratio *= dist;

        if (ew->dist_c_e <= 10.0f)
        {
            point++;

            ratio_tmp = 2.1f;

            pw->bonus_sta |= 0x4;

        }
        else
        {
            dist = ew->dist_c_e;

            if (dist < 20.0f)
            {
                ratio_tmp = ((20.0f - dist) / 10.0f) * 0.19999993f + 1.7f;
            }
            else if (dist < 40.0f)
            {
                ratio_tmp = ((40.0f - dist) / 20.0f) * 0.5f + 1.2f;
            }
            else if (dist < 80.0f)
            {
                ratio_tmp = ((80.0f - dist) / 40.0f) * 0.20000005f + 1.0f;
            }
            else
            {
                if (dist > 100.0f)
                {
                    dist = 100.0f;
                }

                ratio_tmp = ((100.0f - dist) / 40.0f) * 0.5f + 0.5f;

            }
        }

        ratio *= ratio_tmp;

        if (plyr_wrk.charge_num >= charge_max_tbl[cam_custom_wrk.charge_max])
        {
            point += 2;

            ratio *= photo_charge_ratio[cam_custom_wrk.charge_max];

            pw->bonus_sta |= 0x40;

            if (ew->sta & 0x800)
            {
                pw->bonus_sta |= 0x20;

                ratio *= 1.3f;
            }

        }

        if (ew->tr_rate < 10)
        {
            ratio *= 0.5f;
        }
        else if (ew->tr_rate >= 0x28)
        {
            if (ew->tr_rate < 60)
            {
                ratio *= 1.1f;
            }
            else
            {
                ratio *= 1.2f;
            }
        }

        if (ew->dmg >= ew->hp)
        {
            if (pw->bonus_sta & 0x20)
            {
                ratio *= 1.6f;

                pw->bonus_sta |= 0x80;

            }
            else
            {
                ratio *= 1.3f;
            }

            point_plus = (ew->dmg - ew->hp) * 0.2f;
        }
        else
        {
            ratio *= 0.7f;
        }

        if (ew->stm_view != 0)
        {
            pw->bonus_sta |= 0x400;
        }

        if (ew->act_no == 10)
        {
            ratio *= 1.3f;

            pw->bonus_sta |= 0x100;
        }

        if (ew->stm_slow != 0)
        {
            ratio *= 1.1f;

            pw->bonus_sta |= 0x200;
        }

        if (ew->stm_stop != 0)
        {
            ratio *= 0.9f;

            pw->bonus_sta |= 0x800;
        }

        sw->ratio = ratio;

        if (ew->sta & 0x800) {
            point += 4;
        }

        if (ew->type == 0)
        {
            point *= 25;
        }
        else
        {
            point *= 12;
        }

        sw->point = point + (int)point_plus;

        if ((int)(sw->point * sw->ratio) < ew->dmg)
        {
            sw->point = ew->dmg;
            sw->ratio = 1.0f;
        }

        sw->no = ew->dat_no;
        sw->type = ew->type;
        sw->wrk_no = ew->move_box.idx;

        if (
            pw->ene_dead == 0xff && ew->type == 0 && ew->dat->hint_pic != 0xff
            && ew->hp <= ew->dmg && ingame_wrk.game != 1
        )
        {
            pw->ene_dead = ew->dat->hint_pic;
        }
    break;
    case 2:
        sw->point = ew->dat->point_base;

        i = (ew->dist_p_e / 500.0f);

        if (i > 9)
        {
            i = 9;
        }

        ratio = photo_dist_ratio[i];

        i = (int)(ew->dist_c_e / 10.0f);

        if (i > 10)
        {
            i = 10;
        }

        ratio *= photo_center_ratio[i];

        sw->ratio = ratio;
        sw->no = ew->dat_no;
        sw->type = ew->type;
    break;
    }
}

void PhotoPointCulcFurn(FURN_WRK *fw, PHOTO_WRK *pw, float dist, float degree, u_int stts)
{
    SUBJECT_WRK *sw;
    int i;
    u_char para0;

    sw = pw->furn;

    for (i = 0; i < 5; i++)
    {
        if (sw->no == 0xffff)
        {
            break;
        }

        sw++;
    }

    if (i >= 5)
    {
        return;
    }

    if (stts & 0x8)
    {
        return;
    }

    if (stts & 0x40)
    {
        sw->type = 64;
    }
    else if (stts & 0x20)
    {
        if (photo_wrk.dist_2d < dist)
        {
            return;
        }

        pw->furn[photo_wrk.no_2d].no = -1;
        pw->furn[photo_wrk.no_2d].type = 0;
        pw->furn[photo_wrk.no_2d].score = 0;

        pw->dist_2d = dist;
        pw->no_2d = i;

        sw->type = 32;
    }

    sw->point = furn_dat[fw->id].score;

    para0 = dist / 400.0f;

    if (para0 > 9)
    {
        para0 = 9;
    }

    sw->ratio = photo_dist_ratio[para0] * degree;
    sw->no = fw->id;
}

void PhotoPointCulcRare(u_char wrk_no, PHOTO_WRK *pw, float dist, float degree)
{
    SUBJECT_WRK *sw;
    int i;
    int rg_no;
    u_char para0;

    rg_no = rg_dsp_wrk[wrk_no].rg_no;

    sw = pw->rare;

    for (i = 0; i < 3; i++)
    {
        if (sw->no == 0xffff)
        {
            break;
        }

        sw++;
    }

    if (i >= 3)
    {
        return;
    }

    if (rg_no < 128)
    {
        sw->point = rg_dat[rg_no].score;
    }
    else
    {
        sw->point = pg_dat[rg_no - 128].score;
    }

    if (rg_no < 128)
    {
        sw->ratio = rg_dsp_wrk[wrk_no].alpha / 100.0f;
    }
    else
    {
        sw->ratio = 1.0f;
    }

    if (rg_no < 128)
    {
        if (500.0f < dist)
        {
            para0 = ((dist - 500.0f) / 400.0f);
        }
        else
        {
            para0 = 0;
        }

        if (para0 > 9)
        {
            para0 = 9;
        }

        sw->ratio += photo_dist_ratio[para0];
    }

    sw->no = rg_no;
}

void PlyrMpRecoverChk(u_int recov)
{
    return;
}

void PlyrFModeMoveCtrl()
{
    MOVE_BOX *mb = &plyr_wrk.move_box;
    sceVu0FVECTOR tv = { 0.0f, 0.0f, 0.0f, 0.0f };

    plyr_wrk.mvsta &= ~(0x8 | 0x4 | 0x2 | 0x1);

    if (plyr_wrk.cond != 1)
    {
        if (plyr_wrk.move_mot == 0 || plyr_wrk.move_mot > 9)
        {
            if (dbg_wrk.mode_on == 0)
            {
                PlyrMovePadFind(mb, tv);
            }

            PlyrMoveHitChk(mb, tv, 1);
            PlyrPosSet(mb, tv);
        }
    }
}

void PlyrActionChk()
{
    if (*key_now[5] == 1)
    {
        DoorCheckOn(0);
    }
}

void PlyrNModeMoveCtrl()
{
    sceVu0FVECTOR tv;
    MOVE_BOX *mb;

    if (plyr_wrk.cond != 1)
    {
        mb = &plyr_wrk.move_box;

        if (plyr_wrk.cond == 2)
        {
            PlyrKonwakuMove(mb, tv);
        }
        else if (opt_wrk.key_type == 1 || opt_wrk.key_type == 3 || opt_wrk.key_type == 5 || opt_wrk.key_type == 7)
        {
            PlyrMovePadV(mb, tv);
        }
        else
        {
            PlyrMovePad(mb, tv);
        }

        if (PlyrMoveHitChk(mb, tv, 0))
        {
            PlyrHitTurnChk(mb, tv);
        }

        PlyrPosSet(mb, tv);

        if (dbg_wrk.high_speed_mode == 0)
        {
            PlyrSpecialMoveChk(tv);
        }
    }
}

void PlyrHitTurnChk(MOVE_BOX *mb, sceVu0FVECTOR tv)
{
    float rot;

    if (opt_wrk.key_type == 1 || opt_wrk.key_type == 3 || opt_wrk.key_type == 5 || opt_wrk.key_type == 7)
    {
        return;
    }

    if ((tv[0] == 0.0f && tv[1] == 0.0f && tv[2] == 0.0f) || plyr_wrk.mvsta & 0x3000000)
    {
        return;
    }

    rot = SgAtan2f(tv[0], tv[2]) - plyr_wrk.move_box.rot[1];

    RotLimitChk(&rot);

    if (rot != 0.0f && __builtin_fabsf(rot) <= DEG2RAD(30.0f))
    {
        plyr_wrk.mvsta &= ~0x800000;
        plyr_wrk.mvsta |= 0x2000000;

        if (rot > 0.0f)
        {
            mb->rspd[1] = DEG2RAD(5.0f);
        }
        else
        {
            mb->rspd[1] = DEG2RAD(-5.0f);
        }

        mb->loop = __builtin_fabsf(rot) / DEG2RAD(5.0f);

        mb->rot[1] += (rot - mb->rspd[1] * mb->loop);

        RotLimitChk(&mb->rot[1]);
    }
}

u_char PlyrSpecialMoveChk2(sceVu0FVECTOR mv)
{
    sceVu0FVECTOR rv;
    sceVu0FVECTOR tv;
    sceVu0FVECTOR cv;
    sceVu0FVECTOR bv;
    u_char i;
    u_char clr;

    bv[0] = 0.0f;
    bv[1] = 0.0f;
    bv[2] = 0.0f;
    bv[3] = 0.0f;

    bv[0] = mv[0] > 0.0f ? 300.0f : -300.0f;
    bv[0] = bv[0] / 60.0f;

    tv[0] = 0.0f;
    tv[1] = 0.0f;
    tv[2] = 0.0f;
    tv[3] = 0.0f;

    for(i = 0, clr = 0xff; i < 60; i++)
    {
        sceVu0AddVector(tv, tv, bv);
        sceVu0AddVector(cv, plyr_wrk.move_box.pos, tv);

        switch(GetPointMoveMotion(cv, plyr_wrk.pr_info.room_no))
        {
        case 1:
        case 3:
        case 5:
        case 7:
        case 12:
        case 13:
            clr = 2;
        break;
        case 2:
        case 4:
        case 6:
        case 8:
        case 9:
            clr = 0;
        break;
        }

        if (clr != 0xff)
        {
            mv[clr] = 0.0f;

            return 1;
        }
    }

    bv[0] = 0.0f;
    bv[1] = 0.0f;
    bv[2] = 0.0f;
    bv[3] = 0.0f;

    bv[2] = (bv[0] < mv[2]) ?  300.0f : -300.0f;
    bv[2] = (bv[2] / 60.0f);

    tv[0] = 0.0f;
    tv[1] = 0.0f;
    tv[2] = 0.0f;
    tv[3] = 0.0f;

    for (i = 0; i < 60; i++)
    {
        sceVu0AddVector(tv, tv, bv);
        sceVu0AddVector(cv, plyr_wrk.move_box.pos, tv);

        switch(GetPointMoveMotion(cv, plyr_wrk.pr_info.room_no))
        {
        case 1:
        case 3:
        case 5:
        case 7:
        case 12:
        case 13:
            clr = 2;
        break;
        case 2:
        case 4:
        case 6:
        case 8:
        case 9:
            clr = 0;
        break;
        }

        if (clr != 0xff)
        {
            mv[clr] = 0.0f;

            return 1;
        }
    }

    return 0;
}

void PlyrSpecialMoveChk(sceVu0FVECTOR mv)
{
    sceVu0FVECTOR tv;
    sceVu0FVECTOR cv;
    sceVu0FVECTOR rv;
    float rot;
    float adj;
    u_char move_mot;
    u_char up;
    float lstep_adju;
    float sstep_adju;
    float sstep_adjd;
    float nlad0_adju[2] = { 17050.0f, 18330.0f };
    float nlad0_adjd[2] = { 17730.0f, 18330.0f };
    float rlad0_adju[2] = { 10570.0f, 38910.0f };
    float rlad0_adjd[2] = { 10570.0f, 39350.0f };
    float rlad1_adju[2] = {  8530.0f, 38160.0f };
    float rlad1_adjd[2] = {  8530.0f, 38600.0f };

    lstep_adju = 100.0f;
    sstep_adju = 60.0f;
    sstep_adjd = 85.0f;

    if (plyr_wrk.mvsta & 0x200000)
    {
        return;
    }

    plyr_wrk.mvsta &= ~(0x100000 | 0x8000 | 0x4000 | 0x2000 | 0x1000 | 0x800 | 0x400 | 0x200 | 0x100 | 0x80 | 0x40 | 0x20 | 0x10);
    plyr_wrk.mvsta &= ~0x70000;

    if (plyr_wrk.mvsta & 0xf)
    {
        move_mot = plyr_wrk.move_mot;

        if (move_mot == 0)
        {
            tv[0] = 0.0f;
            tv[1] = 0.0f;
            tv[2] = lstep_adju;
            tv[3] = 0.0f;

            RotFvector(plyr_wrk.move_box.rot, tv);
            sceVu0AddVector(tv, plyr_wrk.move_box.pos, tv);

            move_mot = GetPointMoveMotion(tv,plyr_wrk.pr_info.room_no);

            if (move_mot == 12 || move_mot == 13)
            {
                move_mot = plyr_wrk.move_mot;
            }
        }

        switch(move_mot)
        {
        case 1:
        case 2:
        case 3:
        case 4:
            tv[0] = 0.0f;
            tv[1] = 0.0f;
            tv[2] = 200.0f;
            tv[3] = 0.0f;

            rv[0] = 0.0f;
            rv[1] = 0.0f;
            rv[2] = 0.0f;
            rv[3] = 0.0f;

            if ((step_direction & 1) != 0)
            {
                rv[1] = ConvertRot2Dir(plyr_wrk.move_box.rot[1], 3) * PI + PI / 2;
            }
            else
            {
                rv[1] = ConvertRot2Dir(plyr_wrk.move_box.rot[1], 3) * PI;
            }

            RotLimitChk(&rv[1]);
            RotFvector(rv, tv);

            if (PlyrMoveHitChk(&plyr_wrk.move_box, tv, 0) == 0)
            {
                step_direction = (move_mot - 1) & 3;
                rot = step_direction * PI / 2 - plyr_wrk.move_box.rot[1];
                RotLimitChk(&rot);

                up = 0;

                if (__builtin_fabsf(rot) <= PI / 2)
                {
                    up = 1;

                    if ((plyr_wrk.mvsta & 1) != 0)
                    {
                        plyr_wrk.mvsta = plyr_wrk.mvsta | 0x100;
                    }
                    else
                    {
                        plyr_wrk.mvsta = plyr_wrk.mvsta | 0x10;
                    }
                }
                else
                {
                    step_direction = (step_direction + 2) & 3;

                    if ((plyr_wrk.mvsta & 1) != 0)
                    {
                        plyr_wrk.mvsta |= 0x200;
                    }
                    else
                    {
                        plyr_wrk.mvsta |= 0x20;
                    }
                }

                if ((plyr_wrk.mvsta & 0xfff0) == 0)
                {
                    return;
                }

                plyr_wrk.move_box.rot[1] = step_direction * PI / 2;

                RotLimitChk(&plyr_wrk.move_box.rot[1]);

                if (plyr_wrk.move_mot != 0)
                {
                    return;
                }

                for (adj = 1.0f;;)
                {
                    tv[0] = 0.0f;
                    tv[1] = 0.0f;
                    tv[2] = adj;
                    tv[3] = 0.0f;

                    RotFvector(plyr_wrk.move_box.rot, tv);
                    sceVu0AddVector(tv, plyr_wrk.move_box.pos, tv);

                    if (GetPointMoveMotion(tv, plyr_wrk.pr_info.room_no) != 0)
                    {
                        break;
                    }

                    adj += 1.0f;
                }

                if (up)
                {
                    cv[0] = 0.0f;
                    cv[1] = 0.0f;
                    cv[2] = lstep_adju;
                    cv[3] = 0.0f;
                }
                else
                {
                    cv[0] = 0.0f;
                    cv[1] = 0.0f;
                    cv[2] = lstep_adju;
                    cv[3] = 0.0f;
                }

                rv[0] = plyr_wrk.move_box.rot[0];
                rv[1] = plyr_wrk.move_box.rot[1];
                rv[2] = plyr_wrk.move_box.rot[2];
                rv[3] = plyr_wrk.move_box.rot[3];

                rv[1] += PI;

                RotLimitChk(&rv[1]);
                RotFvector(rv, cv);
                sceVu0AddVector(tv, tv, cv);

                plyr_wrk.move_box.pos[0] = tv[0];
                plyr_wrk.move_box.pos[1] = tv[1];
                plyr_wrk.move_box.pos[2] = tv[2];
                plyr_wrk.move_box.pos[3] = tv[3];
            }
            else
            {
                plyr_wrk.mvsta |= 0x100000;
            }
        break;
        case 5:
        case 6:
        case 7:
        case 8:
            step_direction = (move_mot - 5) & 3;

            tv[0] = 0.0f;
            tv[2] = 125.0f;
            tv[1] = 0.0f;
            tv[3] = 0.0f;

            rv[0] = 0.0f;
            rv[1] = 0.0f;
            rv[2] = 0.0f;
            rv[3] = 0.0f;

            if ((move_mot - 5) & 1)
            {
                rv[1] = ConvertRot2Dir(plyr_wrk.move_box.rot[1], 3) * PI + PI / 2;
            }
            else
            {
                rv[1] = ConvertRot2Dir(plyr_wrk.move_box.rot[1], 3) * PI;
            }

            RotLimitChk(&rv[1]);
            RotFvector(rv, tv);

            if (PlyrMoveHitChk(&plyr_wrk.move_box, tv, 0) == 0)
            {
                rot = step_direction * PI / 2 - plyr_wrk.move_box.rot[1];
                RotLimitChk(&rot);

                up = 0;

                if (__builtin_fabsf(rot) <= PI / 2)
                {
                    up = 1;

                    if (plyr_wrk.mvsta & 0x1)
                    {
                        plyr_wrk.mvsta |= 0x400;
                    }
                    else
                    {
                        plyr_wrk.mvsta |= 0x40;
                    }
                }
                else
                {
                    step_direction = (step_direction + 2) & 3;

                    if (plyr_wrk.mvsta & 0x1)
                    {
                        plyr_wrk.mvsta |= 0x800;
                    }
                    else
                    {
                        plyr_wrk.mvsta |= 0x80;
                    }
                }

                if ((plyr_wrk.mvsta & 0xfff0) == 0)
                {
                    return;
                }

                plyr_wrk.move_box.rot[1] = step_direction * PI / 2;
                RotLimitChk(plyr_wrk.move_box.rot + 1);

                if (plyr_wrk.move_mot != 0)
                {
                    return;
                }

                for (adj = 1.0f;;)
                {
                    tv[0] = 0.0f;
                    tv[1] = 0.0f;
                    tv[2] = adj;
                    tv[3] = 0.0f;

                    RotFvector(plyr_wrk.move_box.rot,tv);
                    sceVu0AddVector(tv, plyr_wrk.move_box.pos, tv);

                    if (GetPointMoveMotion(tv,plyr_wrk.pr_info.room_no) != 0)
                    {
                        break;
                    }

                    adj += 1.0f;
                }

                if (up)
                {
                    if (ingame_wrk.msn_no == 0)
                    {
                        cv[0] = 0.0f;
                        cv[1] = 0.0f;
                        cv[2] = sstep_adju;
                        cv[3] = 0.0f;
                    }
                    else
                    {
                        cv[0] = 0.0f;
                        cv[1] = 0.0f;
                        cv[2] = sstep_adju;
                        cv[3] = 0.0f;
                    }
                }
                else
                {
                    if (ingame_wrk.msn_no == 0)
                    {
                        cv[0] = 0.0f;
                        cv[1] = 0.0f;
                        cv[2] = sstep_adjd;
                        cv[3] = 0.0f;
                    }
                    else
                    {
                        cv[0] = 0.0f;
                        cv[1] = 0.0f;
                        cv[2] = sstep_adjd;
                        cv[3] = 0.0f;
                    }
                }

                rv[1] = ((step_direction + 2) & 3) * PI / 2;

                RotLimitChk(&rv[1]);
                RotFvector(rv,cv);
                sceVu0AddVector(tv, tv, cv);

                plyr_wrk.move_box.pos[0] = tv[0];
                plyr_wrk.move_box.pos[1] = tv[1];
                plyr_wrk.move_box.pos[2] = tv[2];
                plyr_wrk.move_box.pos[3] = tv[3];
            }
            else
            {
                plyr_wrk.mvsta |= 0x100000;
            }
        break;
        case 9:
            if (plyr_wrk.anime_no == 99 || ConvertRot2Dir(plyr_wrk.move_box.rot[1], 3) == 1)
            {
                if ((mv[0] != 0.0f || mv[1] != 0.0f || mv[2] != 0.0f) && (plyr_wrk.mvsta & 0x1000000) == 0)
                {
                    step_direction = 1;
                    plyr_wrk.mvsta |= 0x2000;
                    plyr_wrk.move_box.pos[0] = nlad0_adjd[0];
                    plyr_wrk.move_box.pos[2] = nlad0_adjd[1];
                }
            }
            else if (ConvertRot2Dir(plyr_wrk.move_box.rot[1], 3) == 0)
            {
                step_direction = 1;
                plyr_wrk.mvsta |= 0x1000;
                plyr_wrk.move_box.pos[0] = nlad0_adju[0];
                plyr_wrk.move_box.pos[2] = nlad0_adju[1];
            }

            if (plyr_wrk.mvsta & 0xfff0) {
                plyr_wrk.move_box.rot[1] = step_direction * PI / 2;
                RotLimitChk(&plyr_wrk.move_box.rot[1]);
            }
        break;
        case 12:
            if (ConvertRot2Dir(plyr_wrk.move_box.rot[1], 2) == 1 && plyr_wrk.move_box.pos[1] == 0.0f)
            {
                step_direction = 0;
                plyr_wrk.mvsta |= 0x8000;
                plyr_wrk.move_box.pos[0] = rlad0_adjd[0];
                plyr_wrk.move_box.pos[2] = rlad0_adjd[1];
            }
            else if (ConvertRot2Dir(plyr_wrk.move_box.rot[1], 2) == 0 && plyr_wrk.move_box.pos[1] == 1500.0f)
            {
                step_direction = 0;
                plyr_wrk.mvsta |= 0x4000;
                plyr_wrk.move_box.pos[0] = rlad0_adju[0];
                plyr_wrk.move_box.pos[2] = rlad0_adju[1];
            }

            if (plyr_wrk.mvsta & 0xfff0) {
                plyr_wrk.move_box.rot[1] = step_direction * PI / 2;
                RotLimitChk(&plyr_wrk.move_box.rot[1]);
            }
        break;
        case 13:
            if (ConvertRot2Dir(plyr_wrk.move_box.rot[1], 2) == 1 && plyr_wrk.move_box.pos[1] == 1500.0f)
            {
                step_direction = 0;
                plyr_wrk.mvsta |= 0x8000;
                plyr_wrk.move_box.pos[0] = rlad1_adjd[0];
                plyr_wrk.move_box.pos[2] = rlad1_adjd[1];
            }
            else if (ConvertRot2Dir(plyr_wrk.move_box.rot[1], 2) == 0 && plyr_wrk.move_box.pos[1] == 3000.0f)
            {
                step_direction = 0;
                plyr_wrk.mvsta |= 0x4000;
                plyr_wrk.move_box.pos[0] = rlad1_adju[0];
                plyr_wrk.move_box.pos[2] = rlad1_adju[1];
            }

            if (plyr_wrk.mvsta & 0xfff0)
            {
                plyr_wrk.move_box.rot[1] = step_direction * PI / 2;
                RotLimitChk(&plyr_wrk.move_box.rot[1]);
            }
        break;
        case 10:
            plyr_wrk.mvsta |= 0x10000;
        break;
        case 11:
            plyr_wrk.mvsta |= 0x20000;
        break;
        }
    }
    else if (plyr_wrk.move_mot == 11)
    {
        plyr_wrk.mvsta |= 0x40000;
    }
}

void PlyrPosSet(MOVE_BOX *mb, sceVu0FVECTOR tv)
{
    if (plyr_wrk.pr_info.room_old != plyr_wrk.pr_info.room_no)
    {
        plyr_wrk.dop.dov[0] = plyr_wrk.op[0];
        plyr_wrk.dop.dov[1] = plyr_wrk.op[1];
        plyr_wrk.dop.dov[2] = plyr_wrk.op[2];
        plyr_wrk.dop.dov[3] = plyr_wrk.op[3];

        plyr_wrk.dop.room_no = plyr_wrk.pr_info.room_old;
    }

    plyr_wrk.op[0] = mb->pos[0];
    plyr_wrk.op[1] = mb->pos[1];
    plyr_wrk.op[2] = mb->pos[2];
    plyr_wrk.op[3] = mb->pos[3];

    plyr_wrk.mv[0] = tv[0];
    plyr_wrk.mv[1] = tv[1];
    plyr_wrk.mv[2] = tv[2];
    plyr_wrk.mv[3] = tv[3];

    sceVu0AddVector(mb->pos,mb->pos,tv);

    if (plyr_wrk.mvsta & 0x200000)
    {
        mb->pos[1] += plyr_wrk.spd_ud;
    }
    else
    {
        PlyrHeightCtrl(tv);
    }
}

void PlyrKonwakuMove(MOVE_BOX *mb, sceVu0FVECTOR tv)
{
    tv[0] = 0.0f;
    tv[1] = 0.0f;
    tv[2] = 2.0f;
    tv[3] = 0.0f;

    RotFvector(mb->rot, tv);
}

void PlyrMovePad(MOVE_BOX *mb, sceVu0FVECTOR tv)
{
    sceVu0FVECTOR fr = { 0.0f, 0.0f, 0.0f, 0.0f };
    sceVu0FVECTOR tr = { 0.0f, 0.0f, 0.0f, 0.0f };
    float r;
    float rcng;
    u_char run_chk;
    static u_char no_rot_cng2 = 0;
    static u_short cam_cng_tm = 0;
    static int plyr_cam_no_save = 0;
    static float keep_rot = 0.0f;
    static float rs[2] = { 0.0f, 0.0f };
    static u_char ds[5] = { 0, 0, 0, 0, 0 };

    r = GetMovePad(0);

    run_chk = PlyrMoveStaChk(r);

    GetMoveSpeed(tv);

    PadInfoTmpSave(ds, pad[0].an_dir[0], rs, r);

    rcng = keep_rot - rs[0];

    RotLimitChk(&rcng);

    rcng = __builtin_fabsf(rcng);

    if (plyr_cam_no_save != plyr_wrk.pr_info.camera_no)
    {
        cam_cng_tm = 10;

        if (plyr_wrk.pr_info.room_no != plyr_wrk.pr_info.room_old)
        {
            plyr_wrk.mvsta |= 0x400000;
        }
    }

    plyr_cam_no_save = plyr_wrk.pr_info.camera_no;

    if (plyr_wrk.mvsta & (0x2000000 | 0x1000000))
    {
        if ((plyr_wrk.move_mot >= 1 && plyr_wrk.move_mot <= 9) || plyr_wrk.move_mot == 12 || plyr_wrk.move_mot == 13)
        {
            plyr_wrk.mvsta &= ~(0x8 | 0x4 | 0x2 | 0x1);

            tv[0] = 0.0f;
            tv[1] = 0.0f;
            tv[2] = 0.0f;
            tv[3] = 0.0f;
        }

        if (mb->loop)
        {
            mb->loop -= 1;
            mb->rot[1] += mb->rspd[1];

            RotLimitChk(&mb->rot[1]);
            RotFvector(mb->rot, tv);
        }
        else
        {
            mb->rspd[1] = 0.0f;
            plyr_wrk.mvsta &= ~0x3000000;
        }

        return;
    }

    if (r == 10.0f)
    {
        plyr_wrk.mvsta &= ~0x400000;
        cam_cng_tm = 0;
        no_rot_cng2 = 0;
    }

    if (r != 10.0f || run_chk != 0)
    {
        if (cam_cng_tm != 0)
        {
            plyr_wrk.mvsta &= ~0x1000000;
            mb->rspd[1] = 0.0f;

            RotFvector(mb->rot, tv);

            cam_cng_tm--;

            if (cam_cng_tm != 0)
            {
                return;
            }

            if (RotRngChk(plyr_wrk.move_box.pos, plyr_wrk.cp_old, GetTrgtRotY(plyr_wrk.move_box.pos, camera.p), DEG2RAD(80.0f)) == 0)
            {
                keep_rot = rs[0];
                plyr_wrk.mvsta |= 0x400000;
            }

            return;
        }

        if ((plyr_wrk.mvsta & 0x200000) == 0 && plyr_wrk.mvsta & 0x400000 && rcng <= DEG2RAD(30.0f))
        {
            no_rot_cng2 = 1;

            RotFvector(mb->rot, tv);

            return;
        }

        if ((rs[1] == r && (plyr_wrk.mvsta & 0x800000) == 0) || run_chk != 0 || plyr_wrk.mvsta & 0x200000)
        {
            RotFvector(mb->rot,tv);

            if ((plyr_wrk.mvsta & 0x100000) == 0)
            {
                return;
            }

            if ((step_direction & 1) == 0)
            {
                tv[2] = 0.0f;
            }
            else
            {
                tv[0] = 0.0f;
            }

            return;
        }
    }

    plyr_wrk.mvsta &= ~0x400000;

    if (plyr_wrk.mvsta & 0x100000)
    {
        if ((step_direction % 2) == 0)
        {
            tv[2] = 0.0f;
        }
        else
        {
            tv[0] = 0.0f;
        }
    }

    if (r != 10.0f)
    {
        fr[1] = r + plyr_wrk.prot;

        RotLimitChk(&fr[1]);
        RotFvector(fr, tv);

        r = fr[1] - mb->rot[1];

        RotLimitChk(&r);

        if (r > 0.0f)
        {
            rcng = DEG2RAD(5.0f);
        }
        else
        {
            r = __builtin_fabsf(r);
            rcng = -DEG2RAD(5.0f);
        }

        if (r > DEG2RAD(120.0f) && no_rot_cng2 == 0)
        {
            CngPlyrRotRapid(mb, fr[1]);

            if ((plyr_wrk.move_mot >= 1 && plyr_wrk.move_mot <= 9) || plyr_wrk.move_mot == 12 || plyr_wrk.move_mot == 13)
            {
                tv[0] = 0.0f;
                tv[1] = 0.0f;
                tv[2] = 0.0f;
                tv[3] = 0.0f;

                plyr_wrk.mvsta &= ~(0x8 | 0x4 | 0x2 | 0x1);
            }

            plyr_wrk.mvsta &= ~0x800000;
            plyr_wrk.mvsta |= 0x1000000;
        }
        else if (plyr_wrk.mvsta & 0xfff0)
        {
            plyr_wrk.mvsta &= ~(0x1000000 | 0x800000);
        }
        else if (r > DEG2RAD(5.0f))
        {
            mb->rot[1] += rcng;

            RotLimitChk(&mb->rot[1]);

            plyr_wrk.mvsta &= ~0x1000000;
            plyr_wrk.mvsta |= 0x800000;
        }
        else
        {
            mb->rot[1] = fr[1];

            plyr_wrk.mvsta &= ~0x800000;
            plyr_wrk.mvsta &= ~0x1000000;
        }
    }
    else if (
        (tv[0] != 0.0f || tv[1] != 0.0f || tv[2] != 0.0f) &&
        plyr_wrk.mvsta & (0x8000 | 0x4000 | 0x2000 | 0x1000 | 0x800 | 0x400 | 0x200 | 0x100 | 0x80 | 0x40 | 0x20 | 0x10)
    )
    {
        RotFvector(plyr_wrk.move_box.rot, tv);
    }
}

void GetMoveSpeed(sceVu0FVECTOR tv)
{
    tv[0] = 0.0f;
    tv[1] = 0.0f;
    tv[2] = 0.0f;
    tv[3] = 0.0f;

    if (plyr_wrk.mvsta & 0xffff)
    {
        if (plyr_wrk.mvsta & (0x8000 | 0x2000))
        {
            tv[2] = -plyr_wrk.spd;
        }
        else
        {
            tv[2] = plyr_wrk.spd;
        }

#ifdef BUILD_EU_VERSION
        if ((plyr_wrk.mvsta & 0xfff0) == 0)
        {
            tv[2] *= sys_wrk.move_conv;
        }
#endif
    }
}

void PlyrMovePadV(MOVE_BOX *mb, sceVu0FVECTOR tv)
{
    sceVu0FVECTOR r;
    float delta;
    u_char dir;
    static u_char dir_old = 0xff;

    delta = DEG2RAD(2.0f);
    dir = 0xff;

    tv[0] = 0.0f;
    tv[1] = 0.0f;
    tv[2] = 0.0f;
    tv[3] = 0.0f;

    if (plyr_wrk.mvsta & 0x3000000)
    {
        if (
            (plyr_wrk.move_mot > 0 && plyr_wrk.move_mot < 10) ||
            plyr_wrk.move_mot == 12 || plyr_wrk.move_mot == 13
        )
        {
            plyr_wrk.mvsta &= ~(0x8 | 0x4 | 0x2 | 0x1);

            tv[0] = 0.0f;
            tv[1] = 0.0f;
            tv[2] = 0.0f;
            tv[3] = 0.0f;
        }

        if (mb->loop != 0)
        {
            mb->loop -= 1;
            mb->rot[1] += mb->rspd[1];

            RotLimitChk(&mb->rot[1]);
        }
        else
        {
            mb->rspd[1] = 0.0f;

            plyr_wrk.mvsta &= ~0x1000000;
            plyr_wrk.mvsta &= ~0x2000000;
        }

        return;
    }

    r[0] = 0.0f;
    r[1] = GetMovePad(0);
    r[2] = 0.0f;
    r[3] = 0.0f;

    if (plyr_wrk.mvsta & 0x200000)
    {
        r[1] = 0.0f;
    }

    PlyrMoveStaChk(r[1]);

    if (r[1] != 10.0f)
    {
        dir = ConvertRot2Dir(r[1], 1);
        switch(dir)
        {
        case 1:
        case 2:
        case 3:
            mb->rot[1] += delta;

            RotLimitChk(&mb->rot[1]);
        break;
        case 5:
        case 6:
        case 7:
            mb->rot[1] -= delta;

            RotLimitChk(&mb->rot[1]);
        break;
        case 4:
            if (dir_old != dir)
            {
                if (
                    (plyr_wrk.move_mot > 0 && plyr_wrk.move_mot < 10) ||
                    plyr_wrk.move_mot == 12 || plyr_wrk.move_mot == 13
                )
                {
                    tv[0] = 0.0f;
                    tv[1] = 0.0f;
                    tv[2] = 0.0f;
                    tv[3] = 0.0f;

                    plyr_wrk.mvsta &= ~(0x8 | 0x4 | 0x2 | 0x1);
                }

                r[1] = mb->rot[1] + PI;

                plyr_wrk.mvsta |= 0x1000000;

                RotLimitChk(&r[1]);
                CngPlyrRotRapid(mb, r[1]);
            }
        break;
        }
    }

    if (
        dir == 0 || dir == 1 || dir == 7 ||
        plyr_wrk.mvsta & 0x1 || plyr_wrk.mvsta & 0x200000
    )
    {
        GetMoveSpeed(tv);
        RotFvector(mb->rot, tv);
    }

    dir_old = dir;
}

void CngPlyrRotRapid(MOVE_BOX *mb, float rot0)
{
    rot0 = (rot0 - mb->rot[1]);

    RotLimitChk(&rot0);

    if (0.0f < rot0)
    {
        mb->rspd[1] = DEG2RAD(13.0f);
    }
    else
    {
        mb->rspd[1] = DEG2RAD(-13.0f);
    }

    mb->loop = (__builtin_fabsf(rot0) / DEG2RAD(13.0f));

    mb->rot[1] += (rot0 - mb->rspd[1] * mb->loop);

    RotLimitChk(&mb->rot[1]);
}

void PlyrMovePadFind(MOVE_BOX *mb, sceVu0FVECTOR tv)
{
    float rot;
    u_char anime;
    u_char frame;
    u_char anime_tbl[4] = { 77, 81, 79, 84 }; // "MQOT"
    PLYR_WRK *wrk = &plyr_wrk; // not in STAB

    frame = 10;
    rot = GetMovePad(1);

    if (rot != 10.0f)
    {
        plyr_wrk.mvsta |= 0x4;

#ifdef BUILD_EU_VERSION
        tv[0] = wrk->spd * sys_wrk.move_conv * SgSinf(rot);
        tv[2] = wrk->spd * sys_wrk.move_conv * SgCosf(rot);
#else
        tv[0] = wrk->spd * SgSinf(rot);
        tv[2] = wrk->spd * SgCosf(rot);
#endif

        RotFvector(mb->rot, tv);

        anime = anime_tbl[ConvertRot2Dir(rot, 0)];
    }
    else
    {
        anime = 67;
        frame = 20;
    }

    if (plyr_wrk.anime_no != anime)
    {
        if (anime == 67)
        {
            now_frot_x = 0.0f;
        }

        SetPlyrAnime(anime, frame);
    }
}

float spd_ld = 2.0f;
float spd_sd = 4.0f;
float spd_su = 0.0f;
float spd_sdr = 6.3f;
float spd_sur = 0.0f;
u_char plyr_vib_time = 0;
PWARP_WRK pwarp_wrk = {0};

float GetMovePad(u_char id)
{
    float rot;
    u_char dir;
    PAD_STRUCT *ppad;

    rot = 10.0f;
    dir = 0xff;
    ppad = &pad[id];

    if (plyr_wrk.mvsta & 0x200000)
    {
        rot = plyr_wrk.move_box.rot[1];
        plyr_wrk.mvsta &= ~(0x2000000 | 0x1000000);
    }
    else if (id == 0)
    {
        if (opt_wrk.key_type == 1 || opt_wrk.key_type == 3 || opt_wrk.key_type == 5 || opt_wrk.key_type == 7)
        {
            if (*key_now[0] != 0)
            {
                dir = 4;
            }
            else if (*key_now[1] != 0)
            {
                dir = 0;
            }

            if (*key_now[2] != 0)
            {
                if (dir == 0xff)
                {
                    dir = 2;
                }
                else
                {
                    dir = (dir + 2) / 2;
                }
            }
            else if (*key_now[3] != 0)
            {
                if (dir == 0xff)
                {
                    dir = 6;
                }
                else
                {
                    if (dir == 0)
                    {
                        dir = 8;
                    }

                    dir = (dir + 6) / 2;
                }
            }

            if (dir != 0xff)
            {
                rot = dir * DEG2RAD(45.0f) - PI;
            }
        }

        if (pad[0].id == 121)
        {
            if (pad[0].an_dir[0] != 0xff)
            {
                rot = pad[0].an_rot[0];
            }
        }
    }
    else if (pad[0].an_dir[1] != 0xff)
    {
        rot = pad[0].an_rot[1];
    }

    if (plyr_wrk.sta & 0x4000000)
    {
        if (rot != 10.0f)
        {
            rot = 10.0f;
        }
        else
        {
            plyr_wrk.sta &= ~0x4000000;
        }
    }

    return rot;
}

int MovePadEnableChk(u_char *dir_save)
{
    int chk;

    chk = 0;
    if (dir_save[0] == dir_save[1] && dir_save[0] == dir_save[2] && dir_save[0] == dir_save[3])
    {
        chk = (dir_save[0] != dir_save[4]);
    }
    return chk;
}

void PadInfoTmpSave(u_char *dir_save, u_char dir_now, float *rot_save, float rot_now)
{
    u_char i;

    if ((plyr_wrk.mvsta & 0x200000) != 0)
    {
        for (i = 0; i < 5; i++)
        {
            dir_save[i] = -1;
        }

        for (i = 0; i < 2; i++)
        {
            rot_save[i] = 10.0f;
        }
    }
    else
    {
        for (i = 4; i > 0; i--)
        {
            dir_save[i] = dir_save[i - 1];
        }

        for (i = 1; i > 0; i--)
        {
            rot_save[i] = rot_save[i - 1];
        }
    }

    dir_save[0] = dir_now;
    rot_save[0] = rot_now;
}

u_char PlyrMoveStaChk(float pad_chk)
{
    u_char run_chk;
    u_int psta;

    run_chk = 0;
    psta = 0;

    if (
        (plyr_wrk.mvsta & (0x8 | 0x4 | 0x2 | 0x1) && *key_now[6] != 0) ||
        ((plyr_wrk.mvsta & (0x8 | 0x4 | 0x2 | 0x1)) == 0 && *key_now[6] == 1)
    )
    {
        psta = 1;
    }

    plyr_wrk.mvsta &= ~(0x80000 | 0x8 | 0x1 | 0x2 | 0x4);

    if (psta == 0 && pad_chk != 10.0f)
    {
        if (PlyrLeverInputChk() != 0)
        {
            psta = 2;
        }
    }

    if ((psta & 1) != 0 && pad_chk == 10.0f)
    {
        run_chk = 1;
    }

    if (plyr_wrk.dwalk_tm != 0)
    {
        if (psta & 0xf)
        {
            psta = 0x80008;
        }
        else
        {
            psta = 0x80000;
        }
    }

    plyr_wrk.mvsta |= psta;

    return run_chk;
}

u_int PlyrLeverInputChk()
{
    u_char chkx;
    u_char chky;
    u_int result;

    result = 0;

    if (*key_now[0] != 0 || *key_now[1] != 0 || *key_now[3] != 0 || *key_now[2] != 0)
    {
        result = 1;
    }

    if (pad[0].id == 121)
    {
        chkx = pad[0].analog[2];
        chky = pad[0].analog[3];

        chkx = (chkx & 0x80) != 0 ? chkx - 128 : 127 - chkx;
        chky = (chky & 0x80) != 0 ? chky - 128 : 127 - chky;

        chkx = GetDist(chkx, chky);

        if (chkx >= 127)
        {
            result = 2;
        }
        else if (chkx >= 50)
        {
            result = 1;
        }
    }

    return result;
}

u_char PlyrMoveHitChk(MOVE_BOX *mb, sceVu0FVECTOR tv, u_char id)
{
    float dist;
    u_char result;
    u_char div;

    result = 0;

    if (tv[0] != 0.0f || tv[2] != 0.0f)
    {
        dist = GetDist(tv[0], tv[2]);

        if (24.0f < dist)
        {
            div = (dist / 24.0f);
        }
        else
        {
            div = dist;

            if ((div & 0xff) == 0)
            {
                div = 1;
            }
        }

        result = PlyrMapHitCheck(tv,mb->pos,div,plyr_wrk.pr_info.room_no);

        if (id != 0)
        {
            PlyrSpecialMoveChk2(tv);
        }
    }

    return result;
}

void InitPhotoWrk()
{
    PHOTO_WRK *pw;
    int i;

    pw = &photo_wrk;

    *pw = (PHOTO_WRK){0};

    InitSubjectWrk(&pw->plyr);

    for (i = 0; i <= 3; i++)
    {
        InitSubjectWrk(&pw->ene[i]);
    }

    for(i = 0; i <= 4; i++)
    {
        InitSubjectWrk(&pw->furn[i]);
    }

    for (i = 0; i <= 2; i++)
    {
        InitSubjectWrk(&pw->rare[i]);
    }

    pw->mode = 1;
    pw->dist_2d = -1;
    pw->ene_dead = 0xff;
    pw->ratio = 1.0f;
}

void InitSubjectWrk(SUBJECT_WRK *sw)
{
    sw->no = -1;
    sw->ratio = 1.0f;
}

void PlyrBattleChk()
{
    ENE_WRK *ew;
    int i;

    plyr_wrk.sta &= ~0x1;

    for (i = 0, ew = ene_wrk; i <= EWRK_GHOST2; i++, ew++)
    {
        if ((ew->sta & 1))
        {
            if (GetDistV(plyr_wrk.move_box.pos, ew->move_box.pos) <= 5000.0f
                || plyr_wrk.pr_info.room_no == ew->room_no)
            {
                plyr_wrk.sta |= 1;
                break;
            }
        }
    }
    return;
}

void PlyrLightSet()
{
    POINT_WRK p;
    sceVu0FVECTOR tv;
    LIGHT_PACK *lp;
    char pr_set;
    char po_set;

    lp = &plyr_wrk.mylight;

    pr_set = plyr_wrk.pr_set;
    po_set = plyr_wrk.po_set;

    if (pr_set == -1 || lp->parallel_num == 0)
    {
        pr_set = 0;
        plyr_wrk.pr_set = 0;
        lp->parallel_num += 1;
    }

    if (pr_set != -1)
    {
        lp->parallel[pr_set].direction[0] = camera.p[0] - plyr_wrk.move_box.pos[0];
        lp->parallel[pr_set].direction[1] = -1.0f;
        lp->parallel[pr_set].direction[2] = camera.p[2] - plyr_wrk.move_box.pos[2];
        lp->parallel[pr_set].direction[3] = 0.0f;

        lp->parallel[pr_set].diffuse[0] = 0.12f;
        lp->parallel[pr_set].diffuse[1] = 0.12f;
        lp->parallel[pr_set].diffuse[2] = 0.12f;
        lp->parallel[pr_set].diffuse[3] = 255.0f;
    }

    if (po_set == -1 || lp->point_num == 0)
    {
        po_set = 0;
        plyr_wrk.po_set = 0;
        lp->point_num += 1;
    }

    if (po_set != -1)
    {
        tv[0] = -300.0f;
        tv[1] = 0.0f;
        tv[2] = 180.0f;
        tv[3] = 0.0f;

        RotFvector(plyr_wrk.move_box.rot, tv);

        sceVu0AddVector(lp->point[po_set].pos, plyr_wrk.move_box.pos, tv);

        lp->point[po_set].diffuse[0] = 0.1f;
        lp->point[po_set].diffuse[1] = 0.1f;
        lp->point[po_set].diffuse[2] = 0.1f;
        lp->point[po_set].diffuse[3] = 255.0f;

        lp->point[po_set].power = 180.0f;
    }
}

void PlyrMessageDisp(void)
{
    if (find_wrk.mode != 0 && FindMapCtrl() != 0)
    {
        plyr_wrk.mode = PMODE_NORMAL;
        return;
    }
    if (ingame_wrk.mode == INGAME_MODE_GET_ITEM)
    {
        if (ItemGetCtrl() != 0)
        {
            ev_wrk.get_item = 0;
            plyr_wrk.mode = PMODE_NORMAL;
            ingame_wrk.mode = INGAME_MODE_NOMAL;
            eff_blur_off = 0;
        }
    }
    else if (ev_wrk.btl_lock != 0)
    {
        LockBattleDoorOpenMSGDisp();
    }
}

int ReqPlyrSpeAnime(u_char anime_no, u_char frame)
{
    int result;

    result = 0;

    if (plyr_wrk.mode == PMODE_NORMAL)
    {
        if ((plyr_wrk.mvsta & 0x200000) == 0 && (plyr_wrk.sta & 0x8) == 0 && (plyr_wrk.sta & 0x1) == 0)
        {
            if (anime_no != plyr_wrk.anime_no)
            {
                SetPlyrAnime(anime_no, frame);

                result = 1;
            }
        }
    }

    return result;
}

void PlyrSpeAnimeCtrl()
{
    MOVE_BOX *mb = &plyr_wrk.move_box;
    sceVu0FVECTOR tv;

    if (plyr_wrk.sta & 0x20)
    {
        plyr_wrk.mode = PMODE_NORMAL;
    }
    else
    {
        tv[0] = 0.0f;
        tv[2] = -plyr_wrk.spd;
        tv[1] = 0.0f;
        tv[3] = 0.0f;
        RotFvector(plyr_wrk.move_box.rot, tv);
        PlyrMoveHitChk(mb, tv, 1);
        PlyrPosSet(mb, tv);
    }
    return;
}

void PlyrDWalkTmCtrl()
{
    if (plyr_wrk.dwalk_tm != 0)
    {
        plyr_wrk.dwalk_tm -= 1;
    }
}

float GetEnePowerDegree()
{
    FURN_WRK *fw;
    ENE_WRK *ew;
    sceVu0FVECTOR tv;
    sceVu0FVECTOR sv;
    float dist[2];
    float degree;
    float far;
    float height;
    u_int fsta;
    u_char i;

    degree = 0.0f;

    if (ChkPhotoAble() != 0)
    {
        return -1.0f;
    }

    if (plyr_wrk.mode == 1)
    {
        dist[0] = 0.0f;
        ew = ene_wrk;

        for (i = 0; i < 4; i++)
        {
            if (ew->sta & 0x1 && ew->hp != 0 && (ew->sta & 0x80000) == 0)
            {
                dist[1] = GetDistV(camera.p, ew->mpos.p0);

                if (dist[0] == 0.0f || dist[1] < dist[0])
                {
                    dist[0] = dist[1];

                    sv[0] = ew->mpos.p0[0]; sv[1] = ew->mpos.p0[1]; sv[2] = ew->mpos.p0[2]; sv[3] = ew->mpos.p0[3];
                }
            }

            ew++;
        }

        if (dist[0] >= 1.0f && dist[0] <= 4000.0f)
        {
            degree = CulcEP(camera.p, sv);
        }
        else
        {
            dist[0] = 0.0f;

            for (i = 0; i < 3; i++)
            {
                if (rg_dsp_wrk[i].mode != 0 && rg_dsp_wrk[i].alpha >= 100)
                {
                    dist[1] = GetDistV(plyr_wrk.move_box.pos, rg_dsp_wrk[i].pos);

                    if (dist[0] == 0.0f || dist[1] < dist[0])
                    {
                        dist[0] = dist[1];

                        sv[0] = rg_dsp_wrk[i].pos[0];
                        sv[1] = rg_dsp_wrk[i].pos[1];
                        sv[2] = rg_dsp_wrk[i].pos[2];
                        sv[3] = rg_dsp_wrk[i].pos[3];

                        far = 4000.0f;
                    }
                }
            }

            if (dist[0] == 0.0f)
            {
                for (dist[0] = 0.0f, fw = furn_wrk, i = 0; i < 60; i++, fw++)
                {
                    if (GetFurnHintPos(fw, tv, &fsta) != 0) {
                        height = plyr_wrk.move_box.pos[1] + -687.0f;

                        if (height - 750.0f <= tv[1] && tv[1] <= height + 750.0f)
                        {
                            dist[1] = GetDistV(plyr_wrk.move_box.pos, tv);

                            if (dist[0] == 0.0f || dist[1] < dist[0])
                            {
                                dist[0] = dist[1];

                                sv[0] = tv[0];
                                sv[1] = tv[1];
                                sv[2] = tv[2];
                                sv[3] = tv[3];

                                far = furn_dat[fw->id].dist_f;
                            }
                        }
                    }
                }
            }

            if (dist[0] >= 1.0f && far >= dist[0])
            {
                degree = CulcEP(camera.p, sv);
            }
        }
    }
    else
    {
        dist[0] = 0.0f;
        ew = ene_wrk;

        for (i = 0; i < 4; i++)
        {
            if (ew->sta & 0x1 && ew->hp != 0 && (ew->sta & 0x80000) == 0)
            {
                dist[1] = GetDistV(plyr_wrk.move_box.pos, ew->mpos.p0);

                if (dist[0] == 0.0f || dist[1] < dist[0])
                {
                    dist[0] = dist[1];

                    sv[0] = ew->mpos.p0[0];
                    sv[1] = ew->mpos.p0[1];
                    sv[2] = ew->mpos.p0[2];
                    sv[3] = ew->mpos.p0[3];
                }
            }

            ew++;
        }

        if (dist[0] >= 1.0f && dist[0] <= 4000.0f)
        {
            degree = CulcEP2(plyr_wrk.move_box.pos, sv);
        }
        else
        {
            dist[0] = 0.0f;

            for (i = 0; i < 3; i++)
            {
                if (rg_dsp_wrk[i].mode != 0)
                {
                    dist[1] = GetDistV(plyr_wrk.move_box.pos, rg_dsp_wrk[i].pos);

                    if (dist[0] == 0.0f || dist[1] < dist[0])
                    {
                        dist[0] = dist[1];

                        sv[0] = rg_dsp_wrk[i].pos[0];
                        sv[1] = rg_dsp_wrk[i].pos[1];
                        sv[2] = rg_dsp_wrk[i].pos[2];
                        sv[3] = rg_dsp_wrk[i].pos[3];

                        far = 4000.0f;
                    }
                }
            }

            if (dist[0] == 0.0f)
            {
                dist[0] = 0.0f;
                fw = furn_wrk;

                for (i = 0; i < 60; i++)
                {
                    if (GetFurnHintPos(fw, tv, &fsta) != 0) {
                        height = plyr_wrk.move_box.pos[1] + -687.0f;

                        if (height - 750.0f <= tv[1] && tv[1] <= height + 750.0f)
                        {
                            dist[1] = GetDistV(plyr_wrk.move_box.pos, tv);

                            if (dist[0] == 0.0f || dist[1] < dist[0])
                            {
                                dist[0] = dist[1];

                                sv[0] = tv[0];
                                sv[1] = tv[1];
                                sv[2] = tv[2];
                                sv[3] = tv[3];

                                far = furn_dat[fw->id].dist_f;
                            }
                        }
                    }

                    fw++;
                }
            }

            if (dist[0] >= 1.0f && dist[0] <= far)
            {
                degree = CulcEP2(plyr_wrk.move_box.pos, sv);
            }
        }
    }

    if (ingame_wrk.difficult != 0 && ingame_wrk.game == 0)
    {
        degree = 0.0f;
    }

    return degree;
}

float CulcEP(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
    sceVu0FVECTOR rv;
    float degree;

    degree = 0.0f;

    GetTrgtRot(v0, v1, rv, 3);

    rv[1] -= plyr_wrk.move_box.rot[1];

    RotLimitChk(&rv[1]);

    rv[1] = __builtin_fabsf(rv[1]);

    if (rv[1] <= DEG2RAD(135.0f))
    {
        if (rv[1] < DEG2RAD(60.0f))
        {
            rv[0] -= plyr_wrk.frot_x;

            RotLimitChk(rv);

            rv[0] = __builtin_fabsf(rv[0]);
        }
        else
        {
            rv[0] = DEG2RAD(60.0f);
        }

    degree = ((DEG2RAD(240.0f) - (rv[0] + rv[1])) * 100.0f) / DEG2RAD(240.0f);
  }

  return degree;
}

float CulcEP2(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
    sceVu0FVECTOR rv;
    float degree;

    degree = 0.0f;

    GetTrgtRot(v0, v1, rv, 2);

    rv[1] -= plyr_wrk.move_box.rot[1];

    RotLimitChk(&rv[1]);

    rv[1] = __builtin_fabsf(rv[1]);

    if (rv[1] <= DEG2RAD(135.0f))
    {
        degree = ((DEG2RAD(135.0f) - rv[1]) * 100.0f) / DEG2RAD(135.0f);
    }

    return degree;
}

int ChkPhotoAble()
{
    int result;
    u_char i;

    result = 0;

    for (i = 0; i < 5; i++)
    {
        plyr_wrk.fhp[i][3] = 0.0f;
    }

    if (plyr_wrk.mode != PMODE_FINDER)
    {
        return 0;
    }

    if (plyr_wrk.cond != 1)
    {
        for (i = 0; i < 4; i++)
        {
            if (ene_wrk[i].sta & 0x400)
            {
                result = 1;

                break;
            }
        }

        if (i >= 4 && (plyr_wrk.sta & 0x1) == 0)
        {
            result = SearchRareEne();

            if (result == 0)
            {
                result = SearchFurnHint();
            }
        }
    }

    return result;
}

int SearchRareEne()
{
    sceVu0FVECTOR tv;
    float dist[2];
    float tx;
    float ty;
    int i;
    int result;

    result = 0;
    dist[1] = 0.0f;

    for (i = 0; i < 3; i++)
    {
        if (rg_dsp_wrk[i].mode != 0 && 99 < rg_dsp_wrk[i].alpha)
        {
            if (
                OutSightChk(rg_dsp_wrk[i].pos, camera.p, plyr_wrk.move_box.rot[1], PI, 5000.0f) == 0 &&
                FrameInsideChk(rg_dsp_wrk[i].pos, &tx, &ty) != 0
            )
            {
                dist[0] = GetDist(plyr_wrk.fp[0] - tx, plyr_wrk.fp[1] - ty);

                if (dist[0] <= circle_radius_tbl[cam_custom_wrk.charge_range])
                {
                    if (dist[1] == 0.0f || dist[0] < dist[1])
                    {
                        result = 1;

                        dist[1] = dist[0];

                        plyr_wrk.fhp[0][0] = rg_dsp_wrk[i].pos[0];
                        plyr_wrk.fhp[0][1] = rg_dsp_wrk[i].pos[1];
                        plyr_wrk.fhp[0][2] = rg_dsp_wrk[i].pos[2];
                        plyr_wrk.fhp[0][3] = 1.0f;
                    }
                }
            }
        }
    }

    return result;
}

int SearchFurnHint()
{
    PP_JUDGE ppj;
    FURN_WRK *fw;
    sceVu0FVECTOR tv;
    float dist[2];
    float tx;
    float ty;
    u_int fsta;
    int i;
    int result;
    u_char pchk;

    result = 0;
    pchk = 0;

    plyr_wrk.fh_no = -1;

    for (i = 0, dist[1] = 0.0f, fw = furn_wrk;  i < 60; i++, fw++)
    {
        if (GetFurnHintPos(fw, tv, &fsta) == 0)
        {
            continue;
        }

        if (
            OutSightChk(tv, camera.p, plyr_wrk.move_box.rot[1], PI, furn_dat[fw->id].dist_f) == 0 &&
            FrameInsideChk(tv, &tx, &ty) != 0
        )
        {
            dist[0] = GetDist(plyr_wrk.fp[0] - tx, plyr_wrk.fp[1] - ty);

            if (dist[0] <= circle_radius_tbl[cam_custom_wrk.charge_range])
            {
                if ((fsta & 0x2) != 0)
                {
                    pchk = 1;
                }
                else
                {
                    ppj.num = 1;
                    ppj.result[0] = 0;

                    ppj.p[0][0] = tv[0];
                    ppj.p[0][1] = tv[1];
                    ppj.p[0][2] = tv[2];
                    ppj.p[0][3] = tv[3];

                    CheckPointDepth(&ppj);

                    if (ppj.result[0] != 0)
                    {
                        pchk = 1;
                    }
                }

                if (pchk)
                {
                    if (dist[1] == 0.0f || dist[0] < dist[1])
                    {
                        result = 1;

                        dist[1] = dist[0];

                        plyr_wrk.fhp[0][0] = tv[0];
                        plyr_wrk.fhp[0][1] = tv[1];
                        plyr_wrk.fhp[0][2] = tv[2];
                        plyr_wrk.fhp[0][3] = 1.0f;

                        plyr_wrk.fh_no = i;
                    }
                }
            }
        }
    }

    if (plyr_wrk.fh_no != 0xffff)
    {
        FrameInsideChkFurn(&furn_wrk[plyr_wrk.fh_no], &plyr_wrk.fh_deg, 0);
    }

    return result;
}

int GetFurnHintPos(FURN_WRK *fw, sceVu0FVECTOR tv, u_int *fsta)
{
    u_int furn_attr;
    int result;

    result = 0;

    if (fw->use != 0  && fw->use != 2)
    {
        return 0;
    }

    furn_attr = GetFurnAttrF(fw,ingame_wrk.msn_no);

    if ((furn_attr & 0x8) == 0)
    {
        if (furn_attr & 0x60)
        {
            tv[0] = fw->pos[0] + (*fpc_dat[furn_dat[fw->id].fp_no])[0];
            tv[1] = fw->pos[1] + (*fpc_dat[furn_dat[fw->id].fp_no])[1];
            tv[2] = fw->pos[2] + (*fpc_dat[furn_dat[fw->id].fp_no])[2];
            tv[3] = 0.0f;

            if (plyr_wrk.pr_info.room_no == GetPointRoom(tv[0], tv[2]))
            {
                result = 1;
            }
        }
    }

    *fsta = furn_attr;

    return result;
}

float GetPlyrSpd()
{
    float spd;

    spd = 0.0f;

    if (plyr_wrk.mvsta & 0xfff0)
    {
        if (plyr_wrk.move_mot)
        {
            return 0.0f;
        }

        if (plyr_wrk.mvsta & 0x20)
        {
            return spd_ld;
        }

        if (plyr_wrk.mvsta & 0x800)
        {
            return spd_sdr;
        }

        if (plyr_wrk.mvsta & 0x400)
        {
            return spd_sur;
        }

        if (plyr_wrk.mvsta & 0x80)
        {
            return spd_sd;
        }

        if (plyr_wrk.mvsta & 0x40)
        {
            return spd_su;
        }
    }

    else
    {
        if (plyr_wrk.mvsta & 1)
        {
            return 11.0f;
        }

        if (plyr_wrk.mvsta & 2)
        {
            return 5.5f;
        }

        if (plyr_wrk.mvsta & 4)
        {
            return 2.75f;
        }

        if (plyr_wrk.mvsta & 8)
        {
            spd = 3.8499999f;
        }
    }

    return spd;
}

int PlyrNeckDirectionChk(sceVu0FVECTOR p)
{
    p[0] = 0.0f;
    p[1] = 0.0f;
    p[2] = 0.0f;
    p[3] = 0.0f;

    if (NeckTargetEneChk(p) == 0)
    {
        if (NeckTargetItemChk(p) == 0)
        {
            NeckTargetDoorChk(p);
        }
    }

    return p[0] != 0.0f || p[1] != 0.0f || p[2] != 0.0f;
}

u_char NeckTargetEneChk(sceVu0FVECTOR p)
{
    ENE_WRK *ew;
    float dist[2];
    u_char i;
    u_char no;

    dist[1] = 0.0f;
    no = 0xff;
    ew = ene_wrk;

    for (i = 0; i < 4; i++)
    {
        if (ew->sta & 0x1 && ew->hp != 0 && (ew->sta & 0x80000) == 0)
        {
            if (PlyrNoticeObjectChk(ew->move_box.pos, dist) != 0)
            {
                if (dist[1] == 0.0f || dist[0] < dist[1])
                {
                    dist[1] = dist[0];

                    no = i;
                }
            }
        }

        ew++;
    }

    if (no != 0xff)
    {

        p[0] = ene_wrk[no].mpos.p0[0];
        p[1] = ene_wrk[no].mpos.p0[1];
        p[2] = ene_wrk[no].mpos.p0[2];
        p[3] = ene_wrk[no].mpos.p0[3];
    }

    return no != 0xff;
}

u_char NeckTargetItemChk(sceVu0FVECTOR p)
{
    float dist[2];
    u_char i;
    u_char no;

    dist[1] = 0.0f;
    no = 0xff;

    for (i = 0; i < 16; i++)
    {
        if (item_dsp_wrk[0][i].item_no != 0xffff)
        {
            if (item_dsp_wrk[0][i].pos[1] < plyr_wrk.move_box.pos[1] + -1000.0f)
            {
                continue;
            }

            if (PlyrNoticeObjectChk(item_dsp_wrk[0][i].pos, dist) != 0)
            {
                if (dist[1] == 0.0f || dist[0] < dist[1])
                {
                    dist[1] = dist[0];

                    no = i;
                }
            }
        }
    }

    if (no != 0xff)
    {
        p[0] = item_dsp_wrk[0][no].pos[0];
        p[1] = item_dsp_wrk[0][no].pos[1];
        p[2] = item_dsp_wrk[0][no].pos[2];
        p[3] = item_dsp_wrk[0][no].pos[3];
    }

    return no != 0xff;
}

u_char NeckTargetDoorChk(sceVu0FVECTOR p)
{
    sceVu0FVECTOR rv;
    float dist[2];
    u_char i;
    u_char no;

    dist[1] = 0.0f;
    no = 0xFF;

    for (i = 0; i < 20; i++)
    {
        if (door_wrk[i].use == 1 || door_wrk[i].use == 3 || door_wrk[i].use == 4)
        {
            if ((door_keep[door_wrk[i].door_id].attr & 0x8000) == 0)
            {
                if (PlyrNoticeObjectChk(door_wrk[i].pos, &dist[0]) != 0)
                {
                    if (dist[1] == 0.0f || dist[0] < dist[1])
                    {
                        dist[1] = dist[0];

                        no = i;
                    }
                }
            }
        }
    }

    if (no != 0xFF)
    {
        if (dist[1] >= 350.0f)
        {
            if (plyr_wrk.move_mot == 11)
            {
                p[0] = 250.0f;
                p[1] = -575.0f;
                p[2] = 0.0f;
                p[3] = 0.0f;
            } else {
                p[0] = 250.0f;
                p[1] = -687.0f;
                p[2] = 0.0f;
                p[3] = 0.0f;
            }

            rv[0] = 0.0f;
            rv[1] = door_wrk[no].rot;
            rv[2] = 0.0f;
            rv[3] = 0.0f;

            RotFvector(rv, p);

            sceVu0AddVector(p, door_wrk[no].pos, p);
        }
    }

    return no != 0xFF;
}

u_char PlyrNoticeObjectChk(sceVu0FVECTOR ov, float *dist)
{
    sceVu0FVECTOR rv;
    u_char result;

    result = 0;

    GetTrgtRot(plyr_wrk.move_box.pos, ov, rv, 2);

    rv[1] -= plyr_wrk.move_box.rot[1];

    RotLimitChk(&rv[1]);

    if (__builtin_fabsf(rv[1]) <= DEG2RAD(120.0f))
    {
        *dist = GetDistV2(plyr_wrk.move_box.pos, ov);

        if (*dist <= 2000.0f)
        {
            result = 1;
        }
    }

    return result;
}

void ReqPlayerStop(u_char frame)
{
    if (
        (plyr_wrk.mode == PMODE_NORMAL || plyr_wrk.mode == PMODE_MSG_DISP) &&
        (plyr_wrk.mvsta & 0x200000) == 0 && (plyr_wrk.sta & 0x8) == 0 &&
        plyr_wrk.anime_no != 0
    )
    {
        plyr_wrk.mvsta &= ~(0x8 | 0x4 | 0x2 | 0x1);

        SetPlyrAnime(0, frame);
    }
}

void PlayerWarpReq(u_char dat_no)
{
    ingame_wrk.stts |= 0x80;

    pwarp_wrk.time = 32;
    pwarp_wrk.mode = 1;
    pwarp_wrk.req_no = dat_no;

    SetBlackOut2(32);
}

int PlayerWarpCtrl()
{
    switch (pwarp_wrk.mode)
    {
    case PWARP_MODE_IN_REQ:
        if (pwarp_wrk.time != 0)
        {
            pwarp_wrk.time -= 1;

            if (pwarp_wrk.time == 0)
            {
                ingame_wrk.stts |= 0x20;
            }
        }
        else
        {
            PlayerWarpRoomLoadReq();

            pwarp_wrk.mode = PWARP_MODE_ROOM_LOAD;
        }
    break;
    case PWARP_MODE_ROOM_LOAD:
        if (RoomMdlLoadWait() != 0)
        {
            plyr_wrk.move_box.pos[0] = pwarp_dat[pwarp_wrk.req_no].pos_x;
            plyr_wrk.move_box.pos[1] = pwarp_dat[pwarp_wrk.req_no].pos_y;
            plyr_wrk.move_box.pos[2] = pwarp_dat[pwarp_wrk.req_no].pos_z;

            printf("floor = %d\n", map_wrk.floor);

            if (pwarp_dat[pwarp_wrk.req_no].floor != map_wrk.floor)
            {
                MapFloorChange(pwarp_dat[pwarp_wrk.req_no].floor);
            }

            SetBlackIn2(16);

            ingame_wrk.stts &= ~0x20;

            pwarp_wrk.mode = PWARP_MODE_OUT_REQ;
        }
    break;
    case PWARP_MODE_OUT_REQ:
        if (pwarp_wrk.time != 0)
        {
            pwarp_wrk.time -= 1;
        }
        else
        {
            pwarp_wrk.mode = PWARP_MODE_END;
        }
    break;
    case PWARP_MODE_END:
        ingame_wrk.stts &= ~0x80;

        pwarp_wrk.mode = PWARP_MODE_NO_REQ;

        return 1;
    break;
    }

    return 0;
}

void PlayerWarpRoomLoadReq()
{
    RoomLoadReq(pwarp_dat[pwarp_wrk.req_no].room);
}

int ShutterChanceChk()
{
    ENE_WRK *ew;
    u_char i;
    int result;

    result = 0;

    for (i = 0, ew = ene_wrk; i <= EWRK_GHOST2; i++, ew++)
    {
        if (ew->sta & 0x1 && ew->hp != 0 && (ew->sta & 0x80000) == 0 && ew->sta & 0x400 && ew->sta & 0x30000)
        {
            result = 1;

            break;
        }
    }

    return result;
}

void PlayerWarpReq2(u_char dat_no)
{
    ingame_wrk.stts |= 0x80;
    pwarp_wrk.time = 0x20;
    pwarp_wrk.mode = PWARP_MODE_IN_REQ;
    pwarp_wrk.req_no = dat_no;

    SetPanel(0x10, 0.0f, 0.0f, 640.0f, 448.0f, 0, 0, 0, 0x80);
    SetBlackOut2(1);
}
