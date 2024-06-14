#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "gamemain.h"

#include "main/glob.h"
#include "outgame/title.h"
#include "outgame/outgame.h"
#include "outgame/mode_slct.h"
#include "ingame/ingame.h"
#include "mc/mc_main.h"
#include "mc/mc_start.h"
#include "graphics/graph3d/gra3d.h"
#include "graphics/graph2d/tim2_new.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/mov/movie.h"
#include "os/eeiop/eese.h"
#include "os/eeiop/cdvd/eecdvd.h"

#ifdef BUILD_EU_VERSION
#include "os/system.h"
#endif

int init_load_id = 0;
int init_load_adr = 0;
int init_load_size = 0;

void GameMain()
{
#ifdef BUILD_EU_VERSION
    switch (sys_wrk.game_mode)
    {
    case 0:
        if (GameInit())
        {
            if (mc_start_flg == 1)
            {
                sys_wrk.game_mode = mc_start_flg;
                mcInit(7, (u_int *)0x00420000, 0);
                mc_start_flg = 0;
            }
            else
            {
                sys_wrk.game_mode = GAME_MODE_OUTGAME;
            }
        }
    break;
    case 1:
        switch (mcLanguageLoadMain())
        {
        case 0:
            // nothing to do
        break;
        case 1:
            sys_wrk.game_mode = GAME_MODE_LANGUAGE;
            InitSelectLanguage();
        break;
        case 2:
            sys_wrk.game_mode = GAME_MODE_MCINIT;
            if (mc_language & 0x80)
            {
                g_db.disp[1].display.DX = 0x27c;
                g_db.disp[1].display.DY = 0x32;
                g_db.disp[0].display.DX = 0x27c;
                g_db.disp[0].display.DY = 0x32;
                sceGsResetGraph(0, 1, 2, 1);
            }
        break;
        }
    break;
    case 2:
        if (LanguageSelectMain())
        {
            init_load_id = LoadReqLanguage(FNT001_E_PK2, 0x1e30000);
            sys_wrk.game_mode = GAME_MODE_FONT_LOAD_WAIT;
        }
    break;
    case 3:
        if (IsLoadEnd(init_load_id))
        {
            sys_wrk.game_mode = GAME_MODE_MCINIT;
        }
    break;
    case 4:
        if (IsLoadEnd(init_load_id))
        {
            sys_wrk.game_mode = GAME_MODE_OUTGAME;
        }
    break;
    case 5:
        sys_wrk.game_mode = GAME_MODE_MCCHECK;
        mcInit(7, NULL, 0);
    break;
    case 6:
        if (mcStartCheckMain())
        {
            mcEnd();
            sys_wrk.game_mode = GAME_MODE_MSG_LOAD;
            init_load_id = LoadReqLanguage(FNT001_E_PK2, 0x1e30000);
            init_load_id = LoadReqLanguage(IG_MSG_E_OBJ, 0x84a000);
        }
    break;
    case 7:
        OutGameCtrl();
    break;
    case 8:
        InGameCtrl();
    break;
    }
#else
    switch (sys_wrk.game_mode)
    {
    case 0:
        if (GameInit())
        {
            if (mc_start_flg == 1)
            {
                sys_wrk.game_mode = mc_start_flg;
                mcInit(7, NULL, 0);
                mc_start_flg = 0;
            }
            else
            {
                sys_wrk.game_mode = GAME_MODE_OUTGAME;
            }
        }
    break;
    case 1:
        if (mcStartCheckMain())
        {
            sys_wrk.game_mode = GAME_MODE_OUTGAME;
            mcEnd();
        }
    break;
    case 2:
        OutGameCtrl();
    break;
    case 3:
        InGameCtrl();
    break;
    }
#endif
}

int GameInit()
{
    if (GameInitLoad())
    {
        gra3dInit();
        return 1;
  }
    
  return 0;
}

int GameInitLoad()
{
    switch (sys_wrk.load_mode)
    {
    case 0:
        sys_wrk.load_mode = 1;
    case 1:
#ifdef BUILD_EU_VERSION
        init_load_id = LoadReqLanguage(IG_MSG_E_OBJ, 0x84a000);
#else
        init_load_id = LoadReq(IG_MSG_OBJ, 0x84a000);
#endif
        sys_wrk.load_mode = 2;
    break;
    case 2:
        if (!IsLoadEnd(init_load_id))
        {
            return 0;
        }
        sys_wrk.load_mode = 3;
    case 3:
#ifdef BUILD_EU_VERSION
        init_load_id = LoadReqLanguage(FNT001_E_PK2, 0x1e30000);
#else
        init_load_id = LoadReq(FNT001_PK2, 0x1e30000);
#endif
        init_load_id = LoadReq(EFF001_PK2, 0x1e90000);
        sys_wrk.load_mode = 4;
    break;
    case 4:
        if (!IsLoadEnd(init_load_id))
        {
            return 0;
        }
        MakeFontTexSendPacket();
        SetETIM2File(0x1e90000);
        sys_wrk.load_mode = 5;
    case 5:
#ifdef BUILD_EU_VERSION
        init_load_id = SeFileLoadAndSet(SSYSTEM_BD, 0);
        init_load_id = SeFileLoadAndSet(SGY000_BD, 1);
#else
        init_load_id = SeFileLoadAndSet(SSYSTEM_BD, 0);
        init_load_id = SeFileLoadAndSet(SGY000_BD, 1);
#endif
        sys_wrk.load_mode = 6;
    break;
    case 6:
        if (!IsLoadEnd(init_load_id))
        {
            return 0;
        }
        sys_wrk.load_mode = 7;
    case 7:
        sys_wrk.load_mode = 0;
        return 1;
    }
    
    return 0;
}

void GameModeChange(u_char mode)
{
    switch (mode)
    {
    case 0:
        MovieInitWrk();
        sys_wrk.game_mode = GAME_MODE_INGAME;
        ingame_wrk.mode = 0;
    break;
    case 1:
        sys_wrk.game_mode = GAME_MODE_OUTGAME;
        if (ingame_wrk.game == 1)
        {
            title_wrk.mode = 11;
            outgame_wrk.mode = 6;
            BattleModeNext();
        }
        else
        {
            title_wrk.mode = 11;
            outgame_wrk.mode = 3;
        }
        SetReverbVolume(0x2fff);
    break;
    case 2:
        sys_wrk.game_mode = GAME_MODE_OUTGAME;
        if (ingame_wrk.game != 1)
        {
            title_wrk.mode = 11;
        }
        SetReverbVolume(0x2fff);
    break;
    case 3:
        sys_wrk.game_mode = GAME_MODE_OUTGAME;
        title_wrk.mode = 11;
        SetReverbVolume(0x2fff);
    break;
    }
}

#ifdef BUILD_EU_VERSION
int LanguageSelectMain()
{
    return SetSelectLanguage(sys_wrk.language);
}
#endif
