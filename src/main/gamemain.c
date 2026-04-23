#include "common.h"
#include "typedefs.h"
#include "addresses.h"
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

#if defined(BUILD_EU_VERSION)
#include "os/system.h"
#endif

int init_load_id = 0;
int init_load_adr = 0;
int init_load_size = 0;

void GameMain()
{
    switch (sys_wrk.game_mode)
    {
    case GAME_MODE_INIT:
        if (GameInit())
        {
            if (mc_start_flg == 1)
            {
                sys_wrk.game_mode = mc_start_flg;

#if defined(BUILD_JP_VERSION) || defined(BUILD_US_VERSION)
                mcInit(MC_MODE_STARTCHECK, NULL, 0);
#elif defined(BUILD_EU_VERSION)
                mcInit(MC_MODE_STARTCHECK, (u_int *)MC_WORK_ADDRESS, 0);
#endif

                mc_start_flg = 0;
            }
            else
            {
                sys_wrk.game_mode = GAME_MODE_OUTGAME;
            }
        }
    break;
#if defined(BUILD_EU_VERSION)
    case GAME_MODE_LOAD_LANGUAGE:
        switch (mcLanguageLoadMain())
        {
        case 0:
            // do nothing ...
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
    case GAME_MODE_LANGUAGE:
        if (LanguageSelectMain())
        {
            init_load_id = VER_LOAD_REQ_LANG(FNT001_E_PK2, LOAD_ADDRESS_39);

            sys_wrk.game_mode = GAME_MODE_FONT_LOAD_WAIT;
        }
    break;
    case GAME_MODE_FONT_LOAD_WAIT:
        if (IsLoadEnd(init_load_id))
        {
            sys_wrk.game_mode = GAME_MODE_MCINIT;
        }
    break;
    case GAME_MODE_MSG_LOAD:
        if (IsLoadEnd(init_load_id))
        {
            sys_wrk.game_mode = GAME_MODE_OUTGAME;
        }
    break;
    case GAME_MODE_MCINIT:
        sys_wrk.game_mode = GAME_MODE_MCCHECK;

        mcInit(MC_MODE_STARTCHECK, NULL, 0);
    break;
#endif
    case GAME_MODE_MCCHECK:
        if (mcStartCheckMain())
        {
#if defined(BUILD_JP_VERSION)
            sys_wrk.game_mode = GAME_MODE_OUTGAME;
#elif defined(BUILD_US_VERSION)
            sys_wrk.game_mode = GAME_MODE_OUTGAME;

            mcEnd();
#elif defined(BUILD_EU_VERSION)
            mcEnd();

            sys_wrk.game_mode = GAME_MODE_MSG_LOAD;

            init_load_id = VER_LOAD_REQ_LANG(FNT001_E_PK2, LOAD_ADDRESS_39);
            init_load_id = VER_LOAD_REQ_LANG(IG_MSG_E_OBJ, LOAD_ADDRESS_01);
#endif
        }
    break;
    case GAME_MODE_OUTGAME:
        OutGameCtrl();
    break;
    case GAME_MODE_INGAME:
        InGameCtrl();
    break;
    }
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
    case GAME_INIT_LOAD_START:
        sys_wrk.load_mode = GAME_INIT_LOAD_MSG_DAT;
    // case fall-through
    case GAME_INIT_LOAD_MSG_DAT:
        init_load_id = VER_LOAD_REQ_LANG(IG_MSG_OBJ, LOAD_ADDRESS_01);

        sys_wrk.load_mode = GAME_INIT_WAIT_MSG_DAT;
    break;
    case GAME_INIT_WAIT_MSG_DAT:
        if (!IsLoadEnd(init_load_id))
        {
            return 0;
        }

        sys_wrk.load_mode = GAME_INIT_LOAD_FONT_TEX;
    case GAME_INIT_LOAD_FONT_TEX:
        init_load_id = VER_LOAD_REQ_LANG(FNT001_PK2, LOAD_ADDRESS_39);
        init_load_id = LoadReq(EFF001_PK2, LOAD_ADDRESS_41);

        sys_wrk.load_mode = GAME_INIT_WAIT_FONT_TEX;
    break;
    case GAME_INIT_WAIT_FONT_TEX:
        if (!IsLoadEnd(init_load_id))
        {
            return 0;
        }

        MakeFontTexSendPacket();
        SetETIM2File(LOAD_ADDRESS_41);

        sys_wrk.load_mode = GAME_INIT_LOAD_SE_STAT;
    case GAME_INIT_LOAD_SE_STAT:
        init_load_id = SeFileLoadAndSet(SSYSTEM_BD, 0);
        init_load_id = SeFileLoadAndSet(SGY000_BD, 1);

        sys_wrk.load_mode = GAME_INIT_WAIT_SE_STAT;
    break;
    case GAME_INIT_WAIT_SE_STAT:
        if (!IsLoadEnd(init_load_id))
        {
            return 0;
        }

        sys_wrk.load_mode = GAME_INIT_LOAD_END;
    case GAME_INIT_LOAD_END:
        sys_wrk.load_mode = GAME_INIT_LOAD_START;

        return 1;
    break;
    }

    return 0;
}

void GameModeChange(u_char mode)
{
    switch (mode)
    {
    case GMC_OUT_MENU_IN:
        MovieInitWrk();

        sys_wrk.game_mode = GAME_MODE_INGAME;

        ingame_wrk.mode = INGAME_MODE_FIRST_LOAD;
    break;
    case GMC_IN_GAMEOVER_OUT:
        sys_wrk.game_mode = GAME_MODE_OUTGAME;

        if (ingame_wrk.game == 1)
        {
            title_wrk.mode = TITLE_INIT_FROM_IN;

            outgame_wrk.mode = OUTGAME_MODE_BATTLE;

            BattleModeNext();
        }
        else
        {
            title_wrk.mode = TITLE_INIT_FROM_IN;

            outgame_wrk.mode = OUTGAME_MODE_TITLE_TOP;
        }
        SetReverbVolume(0x2fff);
    break;
    case GMC_IN_GAMECLEAR_OUT:
        sys_wrk.game_mode = GAME_MODE_OUTGAME;

        if (ingame_wrk.game != 1)
        {
            title_wrk.mode = TITLE_INIT_FROM_IN;
        }

        SetReverbVolume(0x2fff);
    break;
    case GMC_IN_GAMERETIRE_OUT:
        sys_wrk.game_mode = GAME_MODE_OUTGAME;

        title_wrk.mode = TITLE_INIT_FROM_IN;

        SetReverbVolume(0x2fff);
    break;
    }
}

#if defined(BUILD_EU_VERSION)
int LanguageSelectMain()
{
    return SetSelectLanguage(sys_wrk.language);
}
#endif
