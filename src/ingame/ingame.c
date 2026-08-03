#include "common.h"
#include "addresses.h"
#include "typedefs.h"
#include "enums.h"
#include "ingame.h"

#include "graphics/graph3d/gra3d.h"
// #include "graphics/motion/mdlwork.h"
#include "ingame/entry/entry.h"
#include "ingame/event/ev_main.h"
#include "ingame/event/wan_soul.h"
#include "ingame/ig_glob.h"
#include "ingame/ig_init.h"
#include "ingame/map/door_ctl.h"
#include "ingame/map/door_ctl.h"
#include "ingame/map/find_ctl.h"
#include "ingame/map/furn_ctl.h"
#include "ingame/map/map_ctrl.h"
#include "ingame/map/map_ctrl.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/photo/pht_main.h"
#include "ingame/plyr/plyr_ctl.h"
#include "main/gamemain.h"
#include "main/glob.h"
#include "mc/mc_main.h"
#include "os/eeiop/adpcm/ea_ctrl.h"
#include "os/eeiop/eese.h"
#include "os/eeiop/se_ev.h"
// #include "os/key_cnf.h"
#include "outgame/btl_mode/btl_mode.h"

#if defined(BUILD_JP_VERSION)
#define PAD_ACTION_BACK PAD_SQUARE
#elif defined(BUILD_US_VERSION)
#define PAD_ACTION_BACK PAD_TRIANGLE
#elif defined(BUILD_EU_VERSION)
#define PAD_ACTION_BACK PAD_TRIANGLE
#endif

int start_msn = 0;
int msn0_wait_timer = 0;

void InGameCtrl(void)
{
    switch (ingame_wrk.mode)
    {
    case INGAME_MODE_FIRST_LOAD:
        if (InGameFirstLoad())
        {
            if (ingame_wrk.game == 0 && ingame_wrk.msn_no == 0)
            {
                ingame_wrk.mode = INGAME_MODE_WAIT_MSN0;
                msn0_wait_timer = 90;
            }
            else
            {
                ingame_wrk.mode = INGAME_MODE_INIT;
            }
        }
    break;
    case INGAME_MODE_WAIT_MSN0:
        if (msn0_wait_timer == 0 || --msn0_wait_timer == 0)
        {
                ingame_wrk.mode = INGAME_MODE_INIT;
        }
    break;
    case INGAME_MODE_INIT:
        InGameInit();

        if (ingame_wrk.game == 0)
        {
            if (sys_wrk.load == 0 || (sys_wrk.load == 1 && mc_msn_flg == 1))
            {
                ingame_wrk.mode = INGAME_MODE_MSN_TITLE;
                if (ingame_wrk.msn_no == 0)
                {
                    MoviePlay(0);
                }

                MissionTitleInit(ingame_wrk.msn_no);
            }
            else
            {
                ingame_wrk.mode = INGAME_MODE_LOAD_START;

                LoadStartDataInit();
            }
        }
        else if (ingame_wrk.game == 1)
        {
            ingame_wrk.mode = INGAME_MODE_MSN_TITLE;

            MissionTitleInit(ingame_wrk.msn_no);
        }
    break;
    default:
        InGameMain();
    break;
    }
}

int InGameFirstLoad(void)
{
    switch(sys_wrk.load_mode)
    {
    case INGAME_INIT_LOAD_START:
        ingame_wrk.stts = 0x0;
        sys_wrk.load_mode = INGAME_INIT_LOAD_MENU;

        if (sys_wrk.load == 0 && ingame_wrk.game != 1 && ingame_wrk.clear_count == 0)
        {
            ingame_wrk.ghost_cnt = 0;
            ingame_wrk.rg_pht_cnt = 0;
            ingame_wrk.pht_cnt = 0;
            ingame_wrk.high_score = 0;
        }

        InitSeGhost();
    // case fall-through
    case INGAME_INIT_LOAD_MENU:
        init_load_id = VER_LOAD_REQ_LANG(PL_SMAP_PK2, LOAD_ADDRESS_40);
        init_load_id = LoadReq(PL_PLAY_PK2, LOAD_ADDRESS_41);
        init_load_id = VER_LOAD_REQ_LANG(PL_STTS_PK2, LOAD_ADDRESS_29);
        init_load_id = LoadReq(PL_BGBG_PK2, LOAD_ADDRESS_30);
        init_load_id = VER_LOAD_REQ_LANG(PL_PLDT_PK2, LOAD_ADDRESS_32);
        init_load_id = VER_LOAD_REQ_LANG(PL_MTOP_PK2, LOAD_ADDRESS_31);
        init_load_id = VER_LOAD_REQ_LANG(N_LOAD_PK2, LOAD_ADDRESS_47);

        sys_wrk.load_mode = INGAME_INIT_WAIT_MENU;
    break;
    case INGAME_INIT_WAIT_MENU:
        if (IsLoadEnd(init_load_id) == 0)
        {
            return 0;
        }

        sys_wrk.load_mode = INGAME_INIT_LOAD_EFCT;
    // case fall-through
    case INGAME_INIT_LOAD_EFCT:
        init_load_id = LoadReq(HAND_PK2, LOAD_ADDRESS_49);
        init_load_id = LoadReq(EFF001_PK2, LOAD_ADDRESS_44);

        sys_wrk.load_mode = INGAME_INIT_WAIT_EFCT;
    break;
    case INGAME_INIT_WAIT_EFCT:
        if (IsLoadEnd(init_load_id) == 0)
        {
            return 0;
        }

        SetETIM2File(LOAD_ADDRESS_44);

        sys_wrk.load_mode = INGAME_INIT_LOAD_CAME;
    // case fall-through
    case INGAME_INIT_LOAD_CAME:
        init_load_id = LoadReq(ENEDMG_PK2, LOAD_ADDRESS_48);

        sys_wrk.load_mode = INGAME_INIT_WAIT_CAME;
    break;
    case INGAME_INIT_WAIT_CAME:
        if (IsLoadEnd(init_load_id) == 0)
        {
            return 0;
        }

        sys_wrk.load_mode = INGAME_INIT_LOAD_PHOTO;
    // case fall-through
    case INGAME_INIT_LOAD_PHOTO:
        init_load_id = LoadReq(PHOTO001_PK2, LOAD_ADDRESS_43);

        sys_wrk.load_mode = INGAME_INIT_WAIT_PHOTO;
    break;
    case INGAME_INIT_WAIT_PHOTO:
        if (IsLoadEnd(init_load_id) == 0)
        {
          return 0;
        }

        sys_wrk.load_mode = INGAME_INIT_LOAD_FNDR;
    // case fall-through
    case INGAME_INIT_LOAD_FNDR:
        init_load_id = VER_LOAD_REQ_LANG(PL_FNDR_PK2, LOAD_ADDRESS_36);
        init_load_id = VER_LOAD_REQ_LANG(PL_LIFE_PK2, LOAD_ADDRESS_39);

        sys_wrk.load_mode = INGAME_INIT_WAIT_FNDR;
    break;
    case INGAME_INIT_WAIT_FNDR:
        if (IsLoadEnd(init_load_id) == 0)
        {
            return 0;
        }

        sys_wrk.load_mode = INGAME_INIT_LOAD_BHSE;
    // case fall-through
    case INGAME_INIT_LOAD_BHSE:
        init_load_id = SeFileLoadAndSet(SGY000_BD, 1);

        sys_wrk.load_mode = INGAME_INIT_WAIT_BHSE;
    break;
    case INGAME_INIT_WAIT_BHSE:
        if (IsLoadEnd(init_load_id) == 0)
        {
            return 0;
        }

        sys_wrk.load_mode = INGAME_INIT_LOAD_END;
    // case fall-through
    case INGAME_INIT_LOAD_END:
        sys_wrk.load_mode = INGAME_INIT_LOAD_START;

        return 1;
    break;
    }

    return 0;
}

void InGameInit(void)
{
    int next_msn_chk = 0;

    if (sys_wrk.load == 1 && mc_msn_flg == 1 || ev_wrk.next_msn == 1)
    {
        InitModelLoad();

        next_msn_chk = 1;

        InitPlyr2(plyr_wrk.film_no);

        memset(&event_stts, 0, sizeof(event_stts));

        InitFindStatus();
        MapAreaInit();
        EntryInit();
        MapItemInit();
        RareGhostInit();
        PuzzleGhostInit();
        DataLoadWrkInit();
        MovieInitWrk();
        motInitAniMdlBuf();

        ev_wrk.next_msn = 0;

        if (ingame_wrk.msn_no == 0)
        {
            EventWrkInit();

            time_wrk.one_game = 0;

            MissionSelectItemInit();
        }
    }
    else if (sys_wrk.load == 0)
    {
        InitModelLoad();
        InitPlyr();
        EventWrkInit();

        if (ingame_wrk.clear_count == 0)
        {
            NewgameItemInit();
            TimeCtrlInit();
        }
        else
        {
            MissionSelectItemInit();
        }

        InitFindStatus();
        MapAreaInit();
        EntryInit();
        MapItemInit();
        RareGhostInit();
        PuzzleGhostInit();
        DataLoadWrkInit();
        MovieInitWrk();
        motInitAniMdlBuf();

        time_wrk.one_game = 0;
    }
    else
    {
        InitModelLoad();
        InitPlyrAfterLoad();
        MovieInitWrk();
        motInitAniMdlBuf();
        SettleGhostInit();
    }

    plyr_vib_time = 0;

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    disp3d_room_req = 1;
    disp3d_furn_req = 1;
#endif

    gra3dInitFirst();
    InitSeMenuFade();
    InitEffects();
    InitEffectSub2();
    InitLoadStartLock();
    InitFurnDispFlg();
    InitFlyWrk();
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    InitSceneWork();
#endif

#if defined(BUILD_EU_VERSION)
    sys_wrk.move_conv = sys_wrk.pal_disp_mode == 0 ? 1.2f : 1.0f;
#else
#endif

    if (next_msn_chk != 0)
    {
        LoadgameMenuInit();
    }
    else if (sys_wrk.load == 0 && ingame_wrk.clear_count == 0)
    {
        NewgameMenuInit();
    }
    else
    {
        LoadgameMenuInit();
    }

    gra2dInitST();

    if (ingame_wrk.game == 0 && ingame_wrk.clear_count != 0)
    {
        poss_item[9] = 1;
    }

    SetRealTime();

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    SeSetMVol(opt_wrk.bgm_vol);
    SeSetSteMono(opt_wrk.sound_mode);
#endif
}

void InGameInit2()
{
    int mode;

    mode = 0;

    if (ingame_wrk.mode == INGAME_MODE_MSN_TITLE)
    {

        if (ingame_wrk.game == 0)
        {
            MapMissionChange(ingame_wrk.msn_no);
        }

        mode = 1;
    }

    if (sys_wrk.load == 0)
    {
        InitMapStatus(ingame_wrk.msn_no);
    }

    InitCamera();
    Init3D();
    InitEnemy();

    if (ingame_wrk.game == 0)
    {
        EventInit();
        if (mode)
        {
            ingame_wrk.stts |= 0x20;
        }
    }
    else
    {
        if (ingame_wrk.game == 1)
        {
            BattleModeStageInit();
        }
    }

    FindMapInit();
    MapDataInit();
    PhotoCtrlInit();
    IngameMenuInit();
    InitAreaReadWrk();
    InitFogSelection();
    ReviveWanderSoul();
    SeBackupInit();
    SeEvInit();
    AdpcmMapCtrlInit();
    AdpcmMapUse();
    SetKeyType();

    sys_wrk.load = 0;
}

void InGameMain()
{
    u_char tmp_room_no;

    if ((ingame_wrk.stts & (0x80 | 0x10)) == 0 && ingame_wrk.mode == INGAME_MODE_NOMAL)
    {
        MapCtrlMain();
    }
    else if (furn_disp_flg == 1)
    {
        MapCtrlMain();
    }

    if ((ingame_wrk.stts & 0x8) == 0)
    {
        CameraMain();
    }

    if ((ingame_wrk.stts & 0x20) == 0 || furn_disp_flg == 1)
    {
        PointLightCtrl();

        tmp_room_no = room_wrk.disp_no[1];

        if (ChkNextRoomIsInsite() == 0xff)
        {
            room_wrk.disp_no[1] = 0xff;
        }

        NakasuHazeSet();

        gra3dDraw();
        gra2dDraw(5);

        room_wrk.disp_no[1] = tmp_room_no;
    }

    AnaPonChk();

    switch(ingame_wrk.mode)
    {
    case INGAME_MODE_NOMAL:
        if (start_msn != 0)
        {
            start_msn = 0;

            TmpPadOff();
        }

        if (SpdMenuCtrl() == 0 && PauseMain() == 0)
        {
            if (ingame_wrk.game == 1 && isBattleModeGame() == 0)
            {
                BattleModeStageMain();
                break;
            }

            if ((ingame_wrk.stts & 0x80) != 0)
            {
                break;
            }

            PlyrCtrlMain();
            PhotoCtrl();

            if ((ingame_wrk.stts & 0x10) == 0)
            {
                EneCtrlMain();

                if ((ap_wrk.stts & 0x20) == 0)
                {
                    RareGhostMain();
                }

                if (ingame_wrk.game == 0)
                {
                    EventOpenMain();
                    WanderSoulCtrl();

                    if (ItemGetMain() != 0)
                    {
                        ingame_wrk.mode = INGAME_MODE_GET_ITEM;
                        break;
                    }
                    else if (
                        plyr_wrk.mode != PMODE_FINDER && (plyr_wrk.sta & 0x1) == 0 &&
                        ev_wrk.mode == 0 && FindMapReqJudge() != 0
                    )
                    {
                        plyr_wrk.mode = PMODE_MSG_DISP;
                    }
                }
                else if (ingame_wrk.game == 1 && isBattleModeGame())
                {
                    BattleModeStageMain();
                }
            }

            DoorOpenCheckMain();

            if (
                dbg_wrk.mode_on == 0 && dbg_wrk.cam_mode != 1 &&
                PAD_BTN_PRESSED(PAD_ACTION_BACK) &&
#if defined(BUILD_EU_VERSION)
                pad[0].push[PAD_ACTION_BACK] > 6 &&
#endif
                plyr_wrk.mode == PMODE_NORMAL &&
                GetLoadStartLock() == 0 &&
                plyr_wrk.cond != 1 &&
                ev_wrk.mode == 0
            )
            {
                IngameMenuOpenStart();
            }
        }
    break;
    case INGAME_MODE_EVENT:
        EventMain();
    break;
    case INGAME_MODE_MENU:
        IngameMenuMain();
    break;
    case INGAME_MODE_SAVE_POINT:
        SavePointMenuMain(0);
    break;
    case INGAME_MODE_SPECIAL_EVENT:
        SpecialEventMain();
    break;
    case INGAME_MODE_PHOTO_AFTER:
        PhotoCtrl();
    break;
    case INGAME_MODE_GET_ITEM:
        PlyrCtrlMain();
    break;
    case INGAME_MODE_GAME_OVER:
        GameOverMenuMain();
    break;
    case INGAME_MODE_GAME_OVER_ALBUM:
        if (AlbumModeInGameOver())
        {
            ReturnGameOver();
        }
    break;
    case INGAME_MODE_MSN_TITLE:
        if (MissionTitleMain(ingame_wrk.msn_no))
        {
            InGameInit2();

            ingame_wrk.mode = INGAME_MODE_NOMAL;
            start_msn = 1;

            if (ingame_wrk.game != 1)
            {
                if (ingame_wrk.msn_no != 0)
                {
                    AdpcmMapNoUse();
                }

                mc_msn_flg = 0;
            }
        }
    break;
    case INGAME_MODE_LOAD_START:
        if (LoadStartDataSet() != 0)
        {
            InGameInit2();
        }
    break;
    case INGAME_MODE_AREA_MOVE:
        MapAreaMain();
    break;
    case INGAME_MODE_GHOST_DEAD:
        GhostDeadMain();
    break;
    case INGAME_MODE_SGST_DISP:
        SettleGhostMain();
    break;
    case INGAME_MODE_WANDER_SOUL:
        WanderSoulCtrl();
    break;
    case INGAME_MODE_SPD_MAP:
        SpdMapMain();
    break;
    case INGAME_MODE_SPD_OPT:
        SpdOptMain();
    break;
    case INGAME_MODE_INTER_MSN:
        InterMissionMain();
    break;
    case INGAME_MODE_ENDING:
        EndingMain();
    break;
    }

    if (ingame_wrk.game != 1 || isBattleModeGame() != 0)
    {
        if (ingame_wrk.mode != INGAME_MODE_LOAD_START && ingame_wrk.mode != INGAME_MODE_MSN_TITLE)
        {
            EntryMain();
        }

        ItemCtrl();
        GameTimeCtrl();
    }

    gra2dDraw(6);

    AdpcmMapCtrl();

    if (dbg_wrk.mode_on == 2)
    {
        GameModeChange(3);
        SetTargetVolSeMenuFade(0xfff);
        SetTargetVolAdpcmMenuFade(0xfff);
        EAdpcmFadeOutGameEnd(30);
        SeStopAll();
    }
}

void TmpPadOff()
{
    int i;

    for (i = 0; i < 16; i++)
    {
        *key_now[i] = 0;
    }
}
