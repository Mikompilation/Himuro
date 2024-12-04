#include "common.h"
#include "typedefs.h"
#include "g2d_main.h"

#include "main/glob.h"
#include "ingame/menu/pause.h"
#include "ingame/info/inf_disp.h"
#include "ingame/photo/pht_make.h"
#include "ingame/menu/ig_spd_menu.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph2d/tim2_new.h"
#include "graphics/graph2d/g2d_debug.h"
#include "graphics/graph2d/effect_oth.h"
#include "graphics/graph2d/effect_obj.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/effect_scr.h"

#ifdef BUILD_EU_VERSION
#include "graphics/graph2d/subtitles.h"
#endif

// #include <cstdlib.h>
// RAND_MAX = (2**31-1)
#define RAND_MAX 2147483647

G2D_LOAD_FLG g2d_load_flg = {0};

void gra2dInitBG()
{
	int i;

    g2d_load_flg.dmg = 0;
    g2d_load_flg.hint = 0;
    g2d_load_flg.pazz = 0;
    g2d_load_flg.eneface = 0;

    for (i = 0; i < 4; i++)
    {
        g2d_load_flg.enedmg[i] = 0;
    }

    InitTIM2Files();
    InitMessage();
    InitPerformanceCounter();
}

void gra2dInitST()
{
    InformationDispInit();
    PauseInit();
    SpdMenuInit();
#ifdef BUILD_EU_VERSION
    InitSubtitlesSys();
#endif
}

static void gra2dSubO()
{
    DrawAll2D();
    CallFontTexAndMesPacket();
}

static void gra2dSubG0(int fl)
{
    InitEffectsEF();
    EffectControl(fl);
}

static void gra2dSubG1(int fl)
{
    EffectControl(fl);
}

static void gra2dSubG2(int fl)
{
    EffectControl(fl);
}

static void gra2dSubG3(int fl)
{
    PhotoMakeSave();
    EffectControl(7);
    InformationDispMain();
    PhotoMakeSave2();
    EffectControl(8);
    PauseDisp();
#ifdef BUILD_EU_VERSION
    CallSubtitles();
#endif
    CheckHintTex();
    CallVibrate();
    gra2dSubLAST();
    SetScreenSaver();
}

static void gra2dSubLAST()
{
#ifdef BUILD_EU_VERSION
    CheckSubtitlesFlag();
#endif
    EffectEndSet();
    MesPassCheck();
    InitHeatHaze();
    InitMessageEF();
}

void gra2dDraw(int fl)
{
    SetVURand(rand() / (float)RAND_MAX);

    switch (fl)
    {
        case 0:
            gra2dSubO();
        break;
        case 1:
        case 3:
            gra2dSubG0(fl);
        break;
        case 2:
        case 4:
            gra2dSubG1(fl);
        break;
        case 5:
            gra2dSubG2(fl);
        break;
        case 6:
            gra2dSubG3(fl);
        break;
        case 9:
        break;
        case 10:
        break;
        case 11:
        break;
        case 0xff:
        break;
    }

    if (*key_now[13] != 0 && dbg_wrk.oth_perf != 0)
    {
        InitPerformanceCounter();
    }
}
