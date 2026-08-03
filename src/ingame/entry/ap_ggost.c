#include "common.h"
#include "typedefs.h"
#include "addresses.h"
#include "enums.h"
#include "ap_ggost.h"

#include "sce/libvu0.h"

// #include "graphics/graph2d/effect_ene.h"
#include "graphics/motion/mdlwork.h"
// #include "graphics/motion/motion.h"
#include "ingame/enemy/ene_ctl.h"
#include "ingame/entry/entry.h"
#include "ingame/event/ev_main.h"
#include "ingame/map/door_ctl.h"
#include "main/glob.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"

GGOST_DAT msn03ggst_dat = {
    .ggst = { 4,  0,  4, 0xff, 0xff },
    .room = { 3, 23, 27, 0xff, 0xff },
    .pos = {
        {  2500, 65476, 1600 },
        {  2000,     0, 3000 },
        { 63536,     0, 3200 },
        {     0,     0,    0 },
        {     0,     0,    0 },
    }
};
GGOST_DAT msn04ggst_dat = {
    .ggst = {  7, 11, 0xff, 0xff, 0xff },
    .room = { 16, 22, 0xff, 0xff, 0xff },
    .pos = {
        {  5500,     0, 2500 },
        {  5100,     0, 4400 },
        {     0,     0,    0 },
        {     0,     0,    0 },
        {     0,     0,    0 },
    }
};
GGOST_DAT *ggst_dat[] = { &msn03ggst_dat, &msn04ggst_dat };
int gg_load_ = 0;
int gg_room_ = 0;

#define GGOST_KIND_MAX 5

int GuardGhostAppearSet(void) {
    int i;
    int dat_no;

    if (ingame_wrk.msn_no != 3)
    {
        return 0;
    }

    dat_no = ingame_wrk.msn_no - 3;

    for (i = 0; i < 5; i++)
    {
        ap_wrk.gg_room[i] = ggst_dat[dat_no]->room[i];
    }

    ap_wrk.ggst_cnt = 0;
    ap_wrk.gg_mode = 0;
    ap_wrk.ggst_no = ggst_dat[dat_no]->ggst[0];

    gg_load_mode = GGLOAD_MODE_REQ;

    return 1;
}

void GuardGhostLoadInit()
{
    gg_load_mode = 0;
}

int GuardGhostAppearMain()
{
    if (ingame_wrk.msn_no != 3)
    {
        return 0;
    }

    switch (ap_wrk.gg_mode)
    {
    case 1:
        GuardGhostLoad();
    break;
    case 2:
        if (GuardGhostAppear() != 0)
        {
            ap_wrk.gg_mode = 3;
        }
    break;
    case 3:
        // do nothing ...
    break;
    }
}

int GuardGhostReloadReq()
{
    int dat_no;

    dat_no = ingame_wrk.msn_no - 3;

    if (ap_wrk.ggst_cnt == GGOST_KIND_MAX)
    {
        ene_dead_load = 0;

        return 0;
    }

    if (ggst_dat[dat_no]->ggst[ap_wrk.ggst_cnt] == 0xff)
    {
        ene_dead_load = 0;

        return 0;
    }

    gg_load_mode = 1;

    return 1;
}

void GuardGhostLoadReq()
{
    int dat_no;

    if (ingame_wrk.msn_no != 3)
    {
        ap_wrk.ggst_no = 0xff;
        ap_wrk.gg_mode = 0;

        ene_dead_load = 0;
    }
    else
    {
        dat_no = ingame_wrk.msn_no - 3;

        if (ap_wrk.ggst_cnt == GGOST_KIND_MAX)
        {
#if defined(BUILD_JP_VERSION)
            printf("ap_wrk.ggst_cnt == GGOST_KIND_MAX\n");
#endif

            ap_wrk.ggst_no = 0xff;
            ap_wrk.gg_mode = 0;

            ene_dead_load = 0;
        }
        else if (ggst_dat[dat_no]->ggst[ap_wrk.ggst_cnt] == 0xff)
        {
#if defined(BUILD_JP_VERSION)
            printf("(ggst_dat[dat_no])->ggst[ap_wrk.ggst_cnt] == 0xFF\n");
#endif

#if defined(BUILD_JP_VERSION)
            ap_wrk.ggst_no = 0xff;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            ap_wrk.ggst_no = ggst_dat[dat_no]->ggst[ap_wrk.ggst_cnt];
#endif
            ap_wrk.gg_mode = 0;

            ene_dead_load = 0;
        }
        else
        {
#if defined(BUILD_JP_VERSION)
            printf("             else\n");
#endif

            ap_wrk.gg_mode = 1;

            gg_load_mode = 0;

            ene_dead_load = 1;
        }
    }
}

int GuardGhostLoad()
{
    int dat_no;
    int gg_no;

    dat_no = ingame_wrk.msn_no - 3;
    gg_no = ap_wrk.ggst_no;

    if (ingame_wrk.msn_no != 3)
    {
        return 1;
    }

    if (gg_no == 0xff)
    {
        return 1;
    }

    switch(gg_load_mode)
    {
    case 0:
        gg_load_mode = 2;
    break;
    case 1:
        gg_load_mode = 3;
    break;
    case 2:
        if (ap_wrk.ggst_cnt != 0)
        {
            motReleaseAniMdlBuf(fene_dat[ingame_wrk.msn_no][ap_wrk.ggst_no].anm_no, (u_int *)LOAD_ADDRESS_09);

            ap_wrk.ggst_no = ggst_dat[dat_no]->ggst[ap_wrk.ggst_cnt];
        }

        LoadReq(M000_MIKU_MDL + fene_dat[ingame_wrk.msn_no][ap_wrk.ggst_no].mdl_no, LOAD_ADDRESS_15);

        gg_load_mode = 4;
    break;
    case 3:
        LoadReq(M000_MIKU_MDL + fene_dat[ingame_wrk.msn_no][ap_wrk.ggst_no].mdl_no, LOAD_ADDRESS_15);

        gg_load_mode = 4;
    break;
    case 4:
        if (IsLoadEndAll() != 0)
        {
            motInitEnemyMdl((u_int *)LOAD_ADDRESS_15, fene_dat[ingame_wrk.msn_no][gg_no].mdl_no);
            LoadEneDmgTex(fene_dat[ingame_wrk.msn_no][gg_no].mdl_no, (u_int *)LOAD_ADDRESS_10);
            LoadReq(M000_MIKU_ANM + fene_dat[ingame_wrk.msn_no][gg_no].anm_no, LOAD_ADDRESS_09);

            gg_load_mode = 5;
        }
    break;
    case 5:
        if (IsLoadEndAll() != 0)
        {
#if defined(BUILD_JP_VERSION)
            printf("IsLoadEndAllMotend\n");
#endif

            motInitEnemyAnm((u_int *)LOAD_ADDRESS_09, fene_dat[ingame_wrk.msn_no][gg_no].mdl_no, fene_dat[ingame_wrk.msn_no][gg_no].anm_no);
            SeFileLoadAndSetFGhost(fene_dat[ingame_wrk.msn_no][gg_no].se_no, 18);

            gg_load_mode = 6;
        }
    break;
    case 6:
        if (IsLoadEndAll() != 0)
        {
#if defined(BUILD_JP_VERSION)
            printf("IsLoadEndAllSeEnd\n");
#endif

            gg_load_mode = 7;
            ene_dead_load = 0;
            ap_wrk.gg_mode = 2;

            return 1;
        }
    break;
    }

    return 0;
}

int GuardGhostAppear()
{
    int i;

    if (IsAllConnectDoorClose() == 0)
    {
        return 0;
    }


    for (i = 0; i < 5; i++)
    {
        if (ap_wrk.gg_room[i] == plyr_wrk.pr_info.room_no)
        {
            GuardGhostBattleSet(i);

            return 1;
        }
    }

    return 0;
}

void GuardGhostBattleSet(int room_no)
{
    int dat_no;
    sceVu0FVECTOR pos;

    dat_no = ingame_wrk.msn_no - 3;

    gg_room_bak = ap_wrk.room_fg[plyr_wrk.pr_info.room_no];

    ap_wrk.room_fg[plyr_wrk.pr_info.room_no] = 1;

    ene_wrk[0].dat_no = ap_wrk.ggst_no;
    ene_wrk[0].sta = 2;
    ene_wrk[0].type = 1;

    pos[0] = ggst_dat[dat_no]->pos[ap_wrk.ggst_cnt][0];
    pos[1] = ggst_dat[dat_no]->pos[ap_wrk.ggst_cnt][1];
    pos[2] = ggst_dat[dat_no]->pos[ap_wrk.ggst_cnt][2];

    sceVu0AddVector(pos, room_wrk.pos[0], pos);

    ene_wrk[0].move_box.pos[0] = pos[0];
    ene_wrk[0].move_box.pos[1] = pos[1];
    ene_wrk[0].move_box.pos[2] = pos[2];
    ene_wrk[0].move_box.pos[3] = pos[3];

    DoorLockBattleSet();

    ap_wrk.gg_room[room_no] = 0xff;
}

void GuardGhostBattleEnd()
{
    ap_wrk.room_fg[plyr_wrk.pr_info.room_no] = gg_room_bak;
    ap_wrk.ggst_cnt++;

    GuardGhostLoadReq();
    DoorLockBattleAfter();
}
