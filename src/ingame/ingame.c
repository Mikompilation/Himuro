#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ingame.h"

#include "main/glob.h" // *
#include "mc/mc_main.h"
#include "ingame/ig_glob.h" //
#include "ingame/map/furn_ctl.h" //
#include "ingame/entry/entry.h"
#include "ingame/event/ev_main.h"
#include "ingame/map/map_ctrl.h"
#include "ingame/map/map_ctrl.h"
#include "os/eeiop/eese.h"
#include "ingame/ig_init.h"
#include "os/eeiop/adpcm/ea_ctrl.h"
#include "ingame/menu/ig_menu.h"
#include "graphics/graph3d/gra3d.h" //
#include "outgame/btl_mode/btl_mode.h"
#include "ingame/map/find_ctl.h"
#include "ingame/photo/pht_main.h"
#include "ingame/map/door_ctl.h"
#include "ingame/map/door_ctl.h"
#include "ingame/event/wan_soul.h" //
#include "os/eeiop/se_ev.h"
// #include "os/key_cnf.h"
#include "ingame/plyr/plyr_ctl.h"
#include "main/gamemain.h"

/* sdata 356ef0 */ int start_msn = 0;
/* sdata 356ef4 */ int msn0_wait_timer = 0;

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
  switch(sys_wrk.load_mode) {
    case INGAME_INIT_LOAD_START:
        ingame_wrk.stts = 0;
        sys_wrk.load_mode = INGAME_INIT_LOAD_MENU;
        
        if (sys_wrk.load == 0 && ingame_wrk.game != 1 && ingame_wrk.clear_count == 0)
        {
          ingame_wrk.ghost_cnt = 0;
          ingame_wrk.rg_pht_cnt = 0;
          ingame_wrk.pht_cnt = 0;
          ingame_wrk.high_score = 0;
        }
        InitSeGhost();
      
    case INGAME_INIT_LOAD_MENU:
#ifdef BUILD_EU_VERSION
        init_load_id = LoadReqLanguage(PL_SMAP_E_PK2, 0x1e04000);
        init_load_id = LoadReq(PL_PLAY_PK2, 0x1e2f000);
        init_load_id = LoadReqLanguage(PL_STTS_E_PK2, 0x1ce0000);
        init_load_id = LoadReq(PL_BGBG_PK2, 0x1cfefc0);
        init_load_id = LoadReqLanguage(PL_PLDT_E_PK2, 0x1d210c0);
        init_load_id = LoadReqLanguage(PL_MTOP_E_PK2, 0x1d10000);
        init_load_id = LoadReqLanguage(N_LOAD_E_PK2, 0x1f1c000);
#else
        init_load_id = LoadReq(PL_SMAP_PK2, 0x1e05b00);
        init_load_id = LoadReq(PL_PLAY_PK2, 0x1e2f700);
        init_load_id = LoadReq(PL_STTS_PK2, 0x1ce0000);
        init_load_id = LoadReq(PL_BGBG_PK2, 0x1d05140);
        init_load_id = LoadReq(PL_PLDT_PK2, 0x1d266c0);
        init_load_id = LoadReq(PL_MTOP_PK2, 0x1d15600);
        init_load_id = LoadReq(N_LOAD_PK2, 0x1f1c000);
#endif
        sys_wrk.load_mode = INGAME_INIT_WAIT_MENU;
    break;
      
    case INGAME_INIT_WAIT_MENU:
        if (IsLoadEnd(init_load_id) == 0) {
          return 0;
        }
        sys_wrk.load_mode = INGAME_INIT_LOAD_EFCT;
      
    case INGAME_INIT_LOAD_EFCT:
        init_load_id = LoadReq(HAND_PK2, 0x1fc8000);
        init_load_id = LoadReq(EFF001_PK2, 0x1e90000);
        sys_wrk.load_mode = INGAME_INIT_WAIT_EFCT;
    break;
      
    case INGAME_INIT_WAIT_EFCT:
        if (IsLoadEnd(init_load_id) == 0) {
          return 0;
        }
        SetETIM2File(0x1e90000);
        sys_wrk.load_mode = INGAME_INIT_LOAD_CAME;
      
    case INGAME_INIT_LOAD_CAME:
        init_load_id = LoadReq(ENEDMG_PK2, 0x1fa8000);
        sys_wrk.load_mode = INGAME_INIT_WAIT_CAME;
    break;
      
    case INGAME_INIT_WAIT_CAME:
        if (IsLoadEnd(init_load_id) == 0) {
          return 0;
        }
        sys_wrk.load_mode = INGAME_INIT_LOAD_PHOTO;
      
    case INGAME_INIT_LOAD_PHOTO:
        init_load_id = LoadReq(PHOTO001_PK2, 0x1e85000);
        sys_wrk.load_mode = INGAME_INIT_WAIT_PHOTO;
    break;
      
    case INGAME_INIT_WAIT_PHOTO:
        if (IsLoadEnd(init_load_id) == 0) {
          return 0;
        }
        sys_wrk.load_mode = INGAME_INIT_LOAD_FNDR;
      
    case INGAME_INIT_LOAD_FNDR:
#ifdef BUILD_EU_VERSION
        init_load_id = LoadReqLanguage(PL_FNDR_E_PK2, 0x1d83000);
        init_load_id = LoadReqLanguage(PL_LIFE_E_PK2, 0x1ded000);
#else
        init_load_id = LoadReq(PL_FNDR_PK2, 0x1d88100);
        init_load_id = LoadReq(PL_LIFE_PK2, 0x1df2100);
#endif
        sys_wrk.load_mode = INGAME_INIT_WAIT_FNDR;
    break;
      
    case INGAME_INIT_WAIT_FNDR:
        if (IsLoadEnd(init_load_id) == 0) {
          return 0;
        }
        sys_wrk.load_mode = INGAME_INIT_LOAD_BHSE;
      
    case INGAME_INIT_LOAD_BHSE:
        init_load_id = SeFileLoadAndSet(SGY000_BD, 1);
        sys_wrk.load_mode = INGAME_INIT_WAIT_BHSE;
    break;
      
    case INGAME_INIT_WAIT_BHSE:
        if (IsLoadEnd(init_load_id) == 0) {
          return 0;
        }
        sys_wrk.load_mode = INGAME_INIT_LOAD_END;
      
    case INGAME_INIT_LOAD_END:
        sys_wrk.load_mode = INGAME_INIT_LOAD_START;
    return 1;
    }
    
    return 0;
}

void InGameInit(void) 
{
    /* s2 18 */ int next_msn_chk = 0;
    
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
    disp3d_room_req = 1;
    disp3d_furn_req = 1;
    
    gra3dInitFirst();
    InitSeMenuFade();
    InitEffects();
    InitEffectSub2();
    InitLoadStartLock();
    InitFurnDispFlg();
    InitFlyWrk();
    InitSceneWork();

#ifdef BUILD_EU_VERSION
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
    SeSetMVol(opt_wrk.bgm_vol);
    SeSetSteMono(opt_wrk.sound_mode);
}

void InGameInit2()
{
    /* s2 18 */ int mode;
    
    mode = 0;
    
    if (ingame_wrk.mode == 0x5)
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
            ingame_wrk.stts = ingame_wrk.stts | 0x20;
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
    /* s1 17 */ u_char tmp_room_no;
    
    if ((ingame_wrk.stts & 0x90) == 0 && ingame_wrk.mode == 6)
    {
        MapCtrlMain();
    }
    else if (furn_disp_flg == 1)
    {
        MapCtrlMain();
    }
    
    if ((ingame_wrk.stts & 8) == 0)
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
    case 6:
        if (start_msn != 0)
        {
            start_msn = 0;
            TmpPadOff();
        }
        
        if (!SpdMenuCtrl() && !PauseMain())
        {
            if (ingame_wrk.game == 1 && !isBattleModeGame())
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
                    
                    if (ItemGetMain())
                    {
                        ingame_wrk.mode = 14;
                        break;
                    }
                    else if (
                        plyr_wrk.mode != 1 &&
                        (plyr_wrk.sta & 1) == 0 &&
                        ev_wrk.mode == 0 &&
                        FindMapReqJudge()
                    )
                    {
                        plyr_wrk.mode = 4;
                    }
                }
                else if (ingame_wrk.game == 1 && isBattleModeGame())
                {
                    BattleModeStageMain();
                }
            }
            
            DoorOpenCheckMain();

#ifdef BUILD_EU_VERSION
            if (
                dbg_wrk.mode_on == 0 &&
                dbg_wrk.cam_mode != 1 &&
                *key_now[4] == 1 &&
                pad[0].push[4] > 6 && 
                plyr_wrk.mode == 0 &&
                !GetLoadStartLock() &&
                plyr_wrk.cond != 1 &&
                ev_wrk.mode == 0
            )
#else
            if (
                dbg_wrk.mode_on == 0 &&
                dbg_wrk.cam_mode != 1 &&
                *key_now[4] == 1 &&
                plyr_wrk.mode == 0 &&
                !GetLoadStartLock() &&
                plyr_wrk.cond != 1 &&
                ev_wrk.mode == 0
            )
#endif
            {
                IngameMenuOpenStart();
            }
        }
    break;
    case 7:
        EventMain();
    break;
    case 10:
        IngameMenuMain();
    break;
    case 0x10:
        SavePointMenuMain(0);
    break;
    case 8:
        SpecialEventMain();
    break;
    case 0x11:
        PhotoCtrl();
    break;
    case 0xe:
        PlyrCtrlMain();
    break;
    case 0x14:
        GameOverMenuMain();
    break;
    case 0x15:
        if (AlbumModeInGameOver())
        {
            ReturnGameOver();
        }
    break;
    case 5:
        if (MissionTitleMain(ingame_wrk.msn_no))
        {
            InGameInit2();
            
            ingame_wrk.mode = 6;
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
    case 4:
        if (LoadStartDataSet())
        {
            InGameInit2();
        }
    break;
    case 9:
        MapAreaMain();
    break;
    case 0x12:
        GhostDeadMain();
    break;
    case 0x13:
        SettleGhostMain();
    break;
    case 0xf:
        WanderSoulCtrl();
    break;
    case 0xc:
        SpdMapMain();
    break;
    case 0xd:
        SpdOptMain();
    break;
    case 0x16:
        InterMissionMain();
    break;
    case 0x17:
        EndingMain();
    break;
    }
    
    if (ingame_wrk.game != 1 || isBattleModeGame())
    {
        if (ingame_wrk.mode != 4 && ingame_wrk.mode != 5)
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
        EAdpcmFadeOutGameEnd(0x1e);
        SeStopAll();
    }
}

void TmpPadOff()
{
    /* a0 4 */ int i;
    
    for (i = 0; i < 16; i++)
    {
        *key_now[i] = 0;
    }
}
