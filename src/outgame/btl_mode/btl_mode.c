#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "btl_mode.h"

#include "graphics/graph2d/effect_sub2.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph3d/sgdma.h"
#include "ingame/enemy/ene_ctl.h"
#include "ingame/map/door_ctl.h"
#include "ingame/map/furn_ctl.h"
#include "ingame/menu/item.h"
#include "ingame/menu/pause.h"
#include "main/gamemain.h"
#include "main/glob.h"
#include "mc/mc_at.h"
#include "mc/mc_main.h"
#include "os/eeiop/adpcm/ea_btlmode.h"
#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_ctrl.h"
#include "os/eeiop/adpcm/ea_dat.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "outgame/btl_mode/btl_dat.h"
#include "outgame/btl_mode/btl_menu.h" // SaveStoryWrk
#include "outgame/btl_mode/btl_mode.h"
#include "outgame/mode_slct.h"

int rank_dat[] = {
    401, 1201, 2501, 4001, 7001,
};
int battle_scr_effct[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
};
int btlBonus[] = {
    1000, 0, 300, 700, 1200, 1800, 2000, 500, 0, 0,
};
BTL_MODE_WRK btl_wrk = {0};
STAGE_WRK stage_wrk[20] = {0};

static int sp_load_id = 0;
long int msn_high_score = 0;
long int msn_total_score = 0;
long int msn_bonus = 0;
int stage_all_clear_bak = 0;
int btl_mode_bgm_init = 0;
int btl_clear_disp = 0;

#define MC_WORK_ADDRESS 0x420000

#define PL_STTS_PK2_ADDRESS 0x1ce0000

#ifdef BUILD_EU_VERSION
#define PL_PSVP_PK2_ADDRESS 0x1d54030
#define PL_SAVE_PK2_ADDRESS 0x1d10000
#define SV_PHT_PK2_ADDRESS 0x1d23680
#else
#define PL_PSVP_PK2_ADDRESS 0x1d59630
#define PL_SAVE_PK2_ADDRESS 0x1d15600
#define SV_PHT_PK2_ADDRESS 0x1d28c80
#endif

void BattleModeStageInit()
{
    int i;
    STAGE_DAT *sdp;

    InitCallAnm();

    btl_wrk.mode = 0;

    msn_high_score = 0;
    msn_total_score = 0;
    msn_bonus = 0;

    NewgameItemInit();

    btl_wrk.time = 1;

    poss_item[stage_dat[btl_wrk.stage_no].film[0]] = stage_dat[btl_wrk.stage_no].film[1];

    plyr_wrk.film_no = stage_dat[btl_wrk.stage_no].film[0];

    poss_item[5] = stage_dat[btl_wrk.stage_no].reiseki;

    for (i = 0; i < 3; i++)
    {
        sdp = &stage_dat[btl_wrk.stage_no];

        if (sdp->ene_no[i] == 0xff || sdp->ene_ap[i] == 0)
        {
            break;
        }

        ene_wrk[i].dat_no = sdp->ene_no[i];
        ene_wrk[i].sta = 2;
        ene_wrk[i].move_box.pos[0] = sdp->epos[i][0];
        ene_wrk[i].move_box.pos[1] = sdp->epos[i][1];
        ene_wrk[i].move_box.pos[2] = sdp->epos[i][2];
    }
}

void BattleModeStageMain()
{
    int ret_num;

    switch(btl_wrk.mode)
    {
    case BTL_MODE_READY:
        if (CallReadyGo() != 0)
        {
            btl_wrk.mode = BTL_MODE_GAME;
            btl_wrk.time = stage_dat[btl_wrk.stage_no].time;

            ingame_wrk.stts &= ~0x80;

            pad_ctrl.no_pause = 0;

            stage_all_clear_bak = CheckClearAll();

            LockAllDoorInNowRoom();

            btl_mode_bgm_init = 1;
        }
    break;
    case BTL_MODE_GAME:
        if (btl_mode_bgm_init != 0)
        {
            if (IsEndAdpcmBtlmode() != 0)
            {
                AdpcmBtlmodeEnd();
                AdpcmMapUse();

                btl_mode_bgm_init = 0;
            }
        }

        if (btl_wrk.time != 0)
        {
            btl_wrk.time--;
        }
    break;
    case BTL_MODE_CLEAR:
        if (CheckClearAllFirst() != 0)
        {
            ret_num = CallMissionAllClear();

            if (ret_num != 0)
            {
                ret_num++;
            }
        }
        else
        {
            ret_num = CallMissionClear();
        }

        if (ret_num == 0)
        {
            break;
        }
        else if (ret_num == 1)
        {
            btl_wrk.mode = BTL_MODE_NEXT;
        }
        else if (ret_num == 2)
        {
            btl_wrk.mode = BTL_MODE_SAVE1;
            btl_wrk.time = 4;
        }
        else if (ret_num == 3)
        {
            btl_wrk.mode = BTL_MODE_EXIT;
        }
    break;
    case BTL_MODE_FAILED:
        ret_num = CallMissionFailed();

        if (ret_num == 0)
        {
            break;
        }
        else if (ret_num == 1)
        {
            btl_wrk.mode = BTL_MODE_NEXT;
        }
        else if (ret_num == 2)
        {
            btl_wrk.mode = BTL_MODE_SAVE1;
            btl_wrk.time = 4;
        }
        else if (ret_num == 3)
        {
            btl_wrk.mode = BTL_MODE_EXIT;
        }
    break;
    case BTL_MODE_NEXT:
        LoadStoryWrk();

        ingame_wrk.stts &= 0x40 | 0x10 | 0x4 | 0x2 | 0x1;

        GameModeChange(0x3);
        EAdpcmFadeOut(60);
    break;
    case BTL_MODE_SAVE1:
        if (btl_wrk.time == 4)
        {
            ingame_wrk.stts |= 0x20;
        }

        if (btl_wrk.time != 0)
        {
            btl_wrk.time--;
        }
        else
        {
            InitialDmaBuffer();

            btl_wrk.mode = BTL_MODE_SAVE2;
        }

        EAdpcmFadeOut(60);
    break;
    case BTL_MODE_SAVE2:
        if (IsEndAdpcmFadeOut() != 0)
        {
            mcInit(0, (u_int *)MC_WORK_ADDRESS, mc_msn_flg);

#ifdef BUILD_EU_VERSION
            sp_load_id = LoadReqLanguage(PL_PSVP_E_PK2, PL_PSVP_PK2_ADDRESS);
            sp_load_id = LoadReqLanguage(PL_SAVE_E_PK2, PL_SAVE_PK2_ADDRESS);
#else
            sp_load_id = LoadReq(PL_PSVP_PK2, PL_PSVP_PK2_ADDRESS);
            sp_load_id = LoadReq(PL_SAVE_PK2, PL_SAVE_PK2_ADDRESS);
#endif
            sp_load_id = LoadReq(SV_PHT_PK2, SV_PHT_PK2_ADDRESS);

            btl_wrk.mode = BTL_MODE_SAVE_WAIT;

            EAdpcmCmdPlay(0, 1, AB018_STR, 0, GetAdpcmVol(AB018_STR), 0x280, 0xfff, 0);
        }
    break;
    case BTL_MODE_SAVE_WAIT:
        if (IsLoadEnd(sp_load_id) != 0)
        {
            btl_wrk.mode = BTL_MODE_SAVE3;

            LoadStoryWrk();

            furn_disp_flg = 0;

            ingame_wrk.stts |= 0x80 | 0x20 | 0x8;
        }
    break;
    case BTL_MODE_SAVE3:
        SetSprFile(PL_STTS_PK2_ADDRESS);
        SetSprFile(PL_PSVP_PK2_ADDRESS);
        SetSprFile(PL_SAVE_PK2_ADDRESS);
        SetSprFile(SV_PHT_PK2_ADDRESS);

        if (McAtSaveChk() != 0)
        {
            SaveStoryWrk();

            btl_wrk.mode = BTL_MODE_NEXT;
        }
    break;
    case BTL_MODE_EXIT:
        LoadStoryWrk();
        GameModeChange(3);
        EAdpcmFadeOut(60);
        BattleModeExit();
    break;
    case BTL_MODE_END:
        GameModeChange(3);
        EAdpcmFadeOut(60);
    break;
    }

    if (sys_wrk.game_mode == GAME_MODE_INGAME)
    {
        if (isBattleModeGame() == 0)
        {
            ingame_wrk.stts |= 0x80;

            pad_ctrl.no_pause = 1;
        }
    }
    else
    {
        ingame_wrk.stts &= ~0x80;
        pad_ctrl.no_pause = 0;
    }

    BattleModeInfomationDisp();
}

void BattleModeInfomationDisp()
{
    return;
}

int CheckClearAllS()
{
    int i;

    for (i = 0; i < 20; i++)
    {
        if (stage_wrk[i].rank < 6)
        {
            break;
        }
    }

    return i >= 20;
}

void BattleModeClear()
{
    int i;
    int best_shot;
    int best_time;
    int rank;
    int no_dmg;

    best_shot = 0;
    best_time = 0;
    rank = 0;
    no_dmg = 0;

    btl_clear_disp = 0;

    if (EnemyUseJudge(0) != 0)
    {
        return;
    }

    if (stage_dat[btl_wrk.stage_no].time - btl_wrk.time < stage_wrk[btl_wrk.stage_no].best_time)
    {
        stage_wrk[btl_wrk.stage_no].best_time = stage_dat[btl_wrk.stage_no].time - btl_wrk.time;

        best_time = 1;
    }

    if (stage_wrk[btl_wrk.stage_no].best_shot < msn_high_score)
    {
        stage_wrk[btl_wrk.stage_no].best_shot = msn_high_score;

        best_shot = 1;

        stage_wrk[btl_wrk.stage_no].rank = 6;

        for (i = 0; i < 5; i++)
        {
            if (msn_high_score < rank_dat[i])
            {
                stage_wrk[btl_wrk.stage_no].rank = i + 1;

                break;
            }
        }

        rank = stage_wrk[btl_wrk.stage_no].rank;
    }

    no_dmg = plyr_wrk.hp == 500;

    msn_bonus = btlBonus[0] * best_time + btlBonus[rank + 1] * best_shot + btlBonus[7] * no_dmg;

    if (msn_bonus + cam_custom_wrk.point < 9999999)
    {
        cam_custom_wrk.point = msn_bonus + cam_custom_wrk.point;
    }
    else
    {
        cam_custom_wrk.point = 9999999;
    }

    if (CheckClearAllFirst() != 0)
    {
        if (cam_custom_wrk.func_spe[0] == 0)
        {
            cam_custom_wrk.func_spe[0] = 1;

            btl_clear_disp = 1;
        }

        CallMissionAllClear();
    }
    else
    {
        CallMissionClear();
    }

    if (cam_custom_wrk.func_spe[1] == 0 && CheckClearAllS() != 0)
    {
        cam_custom_wrk.func_spe[1] = 1;

        if (btl_clear_disp == 0)
        {
            btl_clear_disp = 2;
        }
    }

    btl_wrk.mode = BTL_MODE_CLEAR;
}

void BattleModeScoreRecord(long int score)
{
    if (msn_high_score < score)
    {
        msn_high_score = score;
    }

    msn_total_score = msn_total_score + score;
}

int isBattleModeGame()
{
    return btl_wrk.mode == BTL_MODE_GAME;
}

int CheckClearAllFirst()
{
    if (CheckClearAll() != 0 && stage_all_clear_bak == 0)
    {
        return 1;
    }

    return 0;
}

int CheckClearAll()
{
    int i;
    int ret;

    ret = 1;

    for (i = 0; i < 20; i++)
    {
        if (stage_wrk[i].rank == 0)
        {
            ret = 0;

            break;
        }
    }

    return ret;
}

void SetBattleEnd()
{
    btl_wrk.mode = BTL_MODE_END;
}

#ifdef BUILD_EU_VERSION
void SetBattleClear()
{
    int i;

    for (i = 0; i < 20; i++)
    {
        stage_wrk[i].rank = 6;
    }
}
#endif
