#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ig_spd_menu.h"

#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/tim2.h"
// #include "graphics/motion/mdlwork.h" // SendManMdlTex
#include "ingame/menu/ig_map.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/ig_option.h"
#include "ingame/menu/ig_spd_menu.h"
#include "ingame/menu/item.h"
#include "ingame/menu/pause.h"
#include "main/glob.h"
#include "os/eeiop/adpcm/ea_ctrl.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"

static void SpdMapStart();
static void SpdMapInit();
static void SpdMapInOut();
static void SpdOptInit();
static void SpdOptInOut();

SPD_MNU spd_mnu = {0};

static int smap_load_id;
static int sopt_load_id;
static int fnd_load_id;

void SpdMenuInit()
{
    spd_mnu = (SPD_MNU){0};

    smap_load_id = -1;
    sopt_load_id = -1;
    fnd_load_id = -1;
}

char SpdMenuCtrl()
{
    char rtrn;

    rtrn = 0;

    if (*key_now[13] == 1)
    {
        if (CanYouOpenMyMind(1) != 0 && spd_mnu.smap == 0)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);
            SpdMapStart();

            rtrn = 1;
        }
    }

    return rtrn;
}

static void SpdMapStart()
{
    spd_mnu.smap = 1;

    SpdMapInit();
    CaptureScreen(0x1a40);

    DrawScreen(0x7f000, 0x1a40, 0x80, 0x80, 0x80, 0x80);

    ingame_wrk.stts |= 0x20;

    StartMapModeInit();

    ingame_wrk.mode = INGAME_MODE_SPD_MAP;

    smap_load_id = -1;
}

static void SpdMapInit()
{
    int i;

    for (i = 0; i < 7; i++)
    {
        yw2d.io_x[i] = yw2d.io_y[i] = yw2d.io_a[i] = 0.0f;
    }

    yw2d.menu_io_flg = 0x1;
    yw2d.menu_io_cnt = 0;
    yw2d.pad_lock = 0x1;

    SetTargetVolSeMenuFade(0x600);
    SetTargetVolAdpcmMenuFade(0x600);
}

void SpdMapMain()
{
    if (yw2d.io_x[0] == 0.0f)
    {
        BgFusumaYW(0x606060, yw2d.io_x[0], yw2d.io_a[0], 0xffff7d0);
    }
    else
    {
        DrawScreen(0x7f000, 0x1a40, 0x80, 0x80, 0x80, 0x80);
        BgFusumaYW(0x606060, yw2d.io_x[0], yw2d.io_a[0], 0x7d000);
    }

    SpdMapInOut();

    if (yw2d.io_a[4] != 0.0f)
    {
        SetSprFile(0x1ce0000);
#ifdef BUILD_EU_VERSION
        SetSprFile(0x1e04000);
#else
        SetSprFile(0x1e05b00);
#endif
    }

    if (spd_mnu.smap != 0)
    {
        IngameMenuMap();
    }
}

static void SpdMapInOut()
{
    int i;
    u_char count;
    float fade_in;
    float fade_out;

    switch (yw2d.menu_io_flg)
    {
    case 1:
        if (yw2d.menu_io_cnt < 10)
        {
            count = yw2d.menu_io_cnt;

            fade_in = (count / 10.0f) * 128.0f;

            yw2d.io_a[0] = 0.0f;
            yw2d.io_a[1] = yw2d.io_a[4] = fade_in;
            yw2d.io_a[5] = 0.0f;
            yw2d.io_a[6] = 128.0f;
            yw2d.io_x[0] = 0.0f;
        }
        else if (yw2d.menu_io_cnt < 20)
        {
            if (yw2d.menu_io_cnt == 10)
            {
                yw2d.map_flg = 1;
            }

            count = yw2d.menu_io_cnt - 10;

            fade_in = (count * 128) / 10.0f;

            yw2d.io_a[0] = 0.0f;
            yw2d.io_a[1] = 128.0f;
            yw2d.io_a[4] = 128.0f;
            yw2d.io_a[5] = fade_in;
            yw2d.io_a[6] = fade_in;
            yw2d.io_x[0] = 0.0f;
        }

        if (smap_load_id != -1 && IsLoadEnd(smap_load_id) != 0)
        {
            smap_load_id = -1;
        }

        yw2d.pad_lock = 1;

        if (yw2d.menu_io_cnt < 20)
        {
            yw2d.menu_io_cnt++;
        }
        else
        {
            yw2d.menu_io_cnt = 20;
            yw2d.pad_lock = 0;
            yw2d.menu_io_flg = 0;
        }
    break;
    case 2:
        if (yw2d.menu_io_cnt > 10)
        {
            count = yw2d.menu_io_cnt - 10;

            fade_out = (count * 128) / 10.0f;

            yw2d.io_a[0] = 0.0f;
            yw2d.io_a[1] = 128.0f;
            yw2d.io_a[4] = 128.0f;
            yw2d.io_a[5] = fade_out;
            yw2d.io_a[6] = fade_out;
            yw2d.io_x[0] = 0.0f;
        }
        else  if (yw2d.menu_io_cnt != 0)
        {
            if (yw2d.menu_io_cnt == 10)
            {
                yw2d.map_flg = 0;
            }

            count = yw2d.menu_io_cnt;

            fade_out = (count / 10.0f) * 128.0f;

            yw2d.io_a[0] = 0.0f;
            yw2d.io_a[1] = yw2d.io_a[4] = fade_out;
            yw2d.io_a[5] = 0.0f;
            yw2d.io_a[6] = 128.0f;
            yw2d.io_x[0] = 0.0f;
        }
        else
        {
            for (i = 0; i < 7; i++)
            {
                yw2d.io_a[i] = 0.0f;
            }
        }

        if (fnd_load_id != -1 && IsLoadEnd(fnd_load_id) != 0)
        {
            fnd_load_id = -1;
        }

        yw2d.pad_lock = 1;

        if (yw2d.menu_io_cnt != 0)
        {
            yw2d.menu_io_cnt--;
        }
        else
        {
            yw2d.pad_lock = 0x0;
            yw2d.menu_io_flg = 0x0;

            SendManMdlTex();

            spd_mnu.smap = 0;
            ingame_wrk.mode = INGAME_MODE_NOMAL;
            ingame_wrk.stts &= ~0x20;

            SetTargetVolSeMenuFade(0xfff);
            SetTargetVolAdpcmMenuFade(0xfff);
        }
    break;
    }
}

void SpdOptStart()
{
    spd_mnu.sopt = 1;

    SpdOptInit();

#ifdef BUILD_EU_VERSION
    sopt_load_id = LoadReqLanguage(PL_OPTI_E_PK2, 0x1e90000);
#else
    sopt_load_id = LoadReq(PL_OPTI_PK2, 0x1e90000);
#endif

    ingame_wrk.mode = INGAME_MODE_SPD_OPT;
}

static void SpdOptInit()
{
    int i;

    for (i = 0; i < 7; i++)
    {
        yw2d.io_x[i] = yw2d.io_y[i] = yw2d.io_a[i] = 0.0f;
    }

    yw2d.menu_io_flg = 1;
    yw2d.menu_io_cnt = 0;
    yw2d.pad_lock = 1;
}

void SpdOptMain()
{
    if (yw2d.io_x[0] == 0.0f)
    {
        DrawScreen(0x7f000, 0x1a40, 0x80, 0x80, 0x80, 0x80);
        BgFusumaYW(0x606060, yw2d.io_x[0], yw2d.io_a[0], 0x6e000);
    }
    else
    {
        DrawScreen(0x7f000, 0x1a40, 0x80, 0x80, 0x80, 0x80);
        BgFusumaYW(0x606060, yw2d.io_x[0], yw2d.io_a[0], 0x6e000);
    }

    SpdOptInOut();

    if (yw2d.io_a[4] != 0.0f)
    {
        SetSprFile(0x1ce0000);
        SetSprFile(0x1e90000);
    }

    if (spd_mnu.sopt != 0x0)
    {
        IngameMenuOption();
    }
}

static void SpdOptInOut()
{
    int i;
    u_char count;

    if (sopt_load_id != -1 && IsLoadEnd(sopt_load_id) != 0)
    {
        sopt_load_id = -1;

        StartOptionModeInit(0);
    }

    if (fnd_load_id != -1 && IsLoadEnd(fnd_load_id) != 0)
    {
        fnd_load_id = -1;
    }

    switch (yw2d.menu_io_flg)
    {
    case 1:
        if (sopt_load_id == -1)
        {
            if (yw2d.menu_io_cnt < 11)
            {
                count = yw2d.menu_io_cnt;

                yw2d.io_a[0] = (count * 128.0f) / 10.0f;
                yw2d.io_a[1] = (count * 128.0f) / 10.0f;
                yw2d.io_a[4] = (count * 128.0f) / 10.0f;

                yw2d.io_x[0] = 0.0f;
            }

            yw2d.pad_lock = 1;

            if (yw2d.menu_io_cnt < 10)
            {
                yw2d.menu_io_cnt++;
            }
            else
            {
                yw2d.menu_io_cnt = 10;
                yw2d.pad_lock = 0;
                yw2d.menu_io_flg = 0;
            }
        }
    break;
    case 2:
        if (fnd_load_id == -1)
        {
            if (yw2d.menu_io_cnt != 0)
            {
                count = yw2d.menu_io_cnt;

                yw2d.io_a[0] = (count * 128.0f) / 10.0f;
                yw2d.io_a[1] = (count * 128.0f) / 10.0f;
                yw2d.io_a[4] = (count * 128.0f) / 10.0f;

                yw2d.io_x[0] = 0.0f;
            }
            else
            {
                for (i = 0; i < 7; i++)
                {
                    yw2d.io_a[i] = 0.0f;
                }
            }

            yw2d.pad_lock = 1;

            if (yw2d.menu_io_cnt != 0)
            {
                yw2d.menu_io_cnt--;
            }
            else
            {
                yw2d.menu_io_flg = 0;

                SendManMdlTex();

                spd_mnu.sopt = 0;
                ingame_wrk.mode = INGAME_MODE_NOMAL;
                pause_wrk.mode = 1;
            }
        }
    break;
    }
}
