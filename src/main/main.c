#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "main.h"

#include "main/glob.h"
#include "main/gamemain.h"
#include "outgame/title.h"
#include "os/system.h"
#include "os/eeiop/eeiop.h"
#include "os/eeiop/eese.h"
#include "os/eeiop/se_cmd.h"
#include "graphics/mov/movie.h"
#include "graphics/graph2d/tim2_new.h"
#include "graphics/graph2d/g2d_main.h"
#include "graphics/graph3d/sgdma.h"
#include "graphics/graph3d/gra3d.h"
// #include "os/eeiop/adpcm/ea_cmd.h"

int main()
{
    InitSystem();

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    do
    {
#endif
        InitGameFirst();
        do
        {
            while (PlayMpegEvent()) {};
            EiMain();
            GameMain();
            CheckDMATrans();
            sceGsSyncPath(0, 0);
            vfunc();
            DrawAll2DMes_P2();
            FlushModel(1);
            ClearTextureCache();
            SeCtrlMain();
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        } while (!SoftResetChk());
#endif
    } while (1);
}

void InitGameFirst()
{
    sys_wrk.reset = 0;

    gra3dInitFirst();
    gra2dInitBG();

    MovieInitWrk();

#if defined(BUILD_JP_VERSION)
    sys_wrk.game_mode = GAME_MODE_INIT;

    realtime_scene_flg = 0;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    realtime_scene_flg = 0;

    sys_wrk.game_mode = GAME_MODE_INIT;
#endif

    outgame_wrk.mode = OUTGAME_MODE_INIT;
    outgame_wrk.mode_next = OUTGAME_MODE_TITLE;

    ingame_wrk = (INGAME_WRK){0};

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    sys_wrk.sreset_ng = 0;
#endif

    opening_movie_type = 0;
}

void CallSoftReset()
{
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    int lcount;

    lcount = 0;
#endif

    SeStopAll();
    InitSe();
    SetIopCmdSm(1, 0, 0, 0);

#if defined(BUILD_JP_VERSION)
    EAdpcmCmdInit(1);

    sys_wrk.reset = 0;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    EAdpcmCmdStop(0, 0, 0);

    sys_wrk.reset = 0;

    SetSysBackColor(0, 0, 0);

    scene_bg_color = 0;

    while (1)
    {
        EiMain();

        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            EAdpcmCmdInit(1);
            break;
        }

        lcount++;

        if (lcount > 30)
        {
            EAdpcmCmdStop(0, 0, 0);
            lcount = 0;
        }

        vfunc();
    }
#endif
}

int SoftResetChk()
{
    if (
        *key_now[PAD_L1] && *key_now[PAD_L2] &&
        *key_now[PAD_R1] && *key_now[PAD_R2] &&
        *key_now[PAD_START] && *key_now[PAD_SELECT]
    )
    {
#if defined(BUILD_JP_VERSION)
        sys_wrk.sreset_count++;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        sys_wrk.sreset_count = 0;
#endif
    }
    else
    {
        sys_wrk.sreset_count = 0;
    }

    if (sys_wrk.sreset_ng != 0)
    {
        sys_wrk.sreset_count = 0;
    }

    if (sys_wrk.sreset_count >= 60)
    {
        sys_wrk.reset = 1;
    }

    if (sys_wrk.reset == 1)
    {
        sys_wrk.sreset_count = 0;

        CallSoftReset();

        return 1;
    }

    return 0;
}
