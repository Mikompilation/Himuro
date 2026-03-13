#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ig_option.h"

#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/message.h"
// #include "graphics/graph2d/tim2.h"
#include "ingame/info/inf_disp.h"
#include "ingame/menu/ig_file.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/ig_spd_menu.h"
#include "ingame/menu/item.h"
#include "main/glob.h"
#include "os/eeiop/eese.h"
#include "os/key_cnf.h"
#include "os/pad.h"

static void MenuOptionDisp(u_char csr, u_char mode, u_char io);
static void OptionCntInit();
static void OptionCntRenew();
static void OptionModeInOut();
static void OptionModeInOut2();
static void DspAlpCtrl(u_char *src, u_char mode, u_char alpha);
static void DspOptMain(u_char csr, u_char mode, u_char alp0, u_char alp1, u_char alp2, u_char ttl);
static void DspOptCsr(u_char csr, u_char mode, u_char alpha);
static void DspBigFlame1(u_char alpha, u_char str_mode);
static void DspBigFlame2(u_char alpha, u_char str_mode);
static void DspSmlFlame1(u_char alpha, u_char str_mode);
static void DspSmlFlame2(u_char alpha, u_char str_mode);
static void DspMsgWin(u_char csr, u_char alpha);
static void DspPadType(u_char alpha, u_char ttl);
static void DspLgtScrn(u_char alpha, u_char ttl);

static MENU_OPT menu_opt;
static FLSH_CORE flsh[2];

void NewgameMenuOptionInit()
{
    menu_opt = (MENU_OPT){0};
}

void LoadgameMenuOptionInit()
{
    menu_opt = (MENU_OPT){0};
}

void StartOptionModeInit(u_char mode)
{
    if (mode == 0)
    {
        ig_menu_wrk.csr[1] = 0;

        menu_opt.mode = 7;

        yw2d.pad_lock = 1;
    }
    else
    {
        yw2d.pad_lock = 0;
    }

    menu_opt.snd_vol = opt_wrk.bgm_vol;
    menu_opt.vive_flg = 0;

    menu_opt.fade_alp[0] = menu_opt.fade_alp[1] = menu_opt.fade_alp[2] = 0;
}

void IngameMenuOption()
{
    if (MenuOptionPad(&ig_menu_wrk.csr[1], &menu_opt.mode) != 0)
    {
        if (spd_mnu.sopt != 0)
        {
            yw2d.menu_io_flg = 2;
            yw2d.menu_io_cnt = 10;
        }
        else
        {
            yw2d.out_mode_cnt = 11;
        }
    }

    MenuOptionDisp(ig_menu_wrk.csr[1], menu_opt.mode, spd_mnu.sopt);
}

char MenuOptionPad(u_char *csr, u_char *mode)
{
    switch(*mode)
    {
    case 7:
        if (yw2d.pad_lock == 0)
        {
            if (*key_now[4] == 1)
            {
                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(3, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(3, 0, 0x1000, 0x1000, 0);
                }

                return 1;
            }
            else if (*key_now[6] == 1 || *key_now[5] == 1)
            {
                if (*csr == 5)
                {
                    if (ingame_wrk.stts & 0x80)
                    {
                        SeStartFix(1, 0, 0x1000, 0x1000, 1);
                    }
                    else
                    {
                        SeStartFix(1, 0, 0x1000, 0x1000, 0);
                    }

                    return 1;
                }

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);
                }

                *mode = *csr;
            }
            else if (
                *key_now[0] == 1 ||
                (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
                Ana2PadDirCnt(0) == 1 ||
                (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
            )
            {
                if (*csr != 0)
                {
                    (*csr)--;
                }
                else
                {
                    *csr = 5;
                }

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                }
            }
            else if (
                *key_now[1] == 1 ||
                (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
                Ana2PadDirCnt(2) == 1 ||
                (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
            )
            {
                if (*csr < 5)
                {
                    (*csr)++;
                }
                else
                {
                    *csr = 0;
                }

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                }
            }
        }
    break;
    case 0:
        if (yw2d.pad_lock == 0)
        {
            if (*key_now[4] == 1)
            {
                *mode = 7;

                SetKeyType();

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(3, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(3, 0, 0x1000, 0x1000, 0);
                }
            }
            else if ((*key_now[6] == 1) || (*key_now[5] == 1))
            {
                *mode = 7;

                SetKeyType();

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);
                }
            }
            else if (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            )
            {
                if (opt_wrk.key_type != 0)
                {
                    opt_wrk.key_type--;
                }
                else
                {
                    opt_wrk.key_type = 7;
                }

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                }
            }
            else if (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && Ana2PadDirCnt(1) % 5 == 1)
            )
            {
                if (opt_wrk.key_type < 7)
                {
                    opt_wrk.key_type++;
                }
                else
                {
                    opt_wrk.key_type = 0;
                }

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                }
            }
        }
    break;
    case 1:
        if (yw2d.pad_lock == 0)
        {
            if (*key_now[4] == 1)
            {
                *mode = 7;

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(3, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(3, 0, 0x1000, 0x1000, 0);
                }
            }
            else if (*key_now[6] == 1 || *key_now[5] == 1)
            {
                *mode = 7;

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);
                }
            }
            else if (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            )
            {
                if (opt_wrk.pad_move != 0)
                {
                    opt_wrk.pad_move--;
                }
                else
                {
                    opt_wrk.pad_move = 1;
                }

                if (opt_wrk.pad_move == 0)
                {
                    menu_opt.vive_flg = 60;
                }
                else
                {
                    menu_opt.vive_flg = 0;
                }

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                }
            }
            else if (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && Ana2PadDirCnt(1) % 5 == 1)
            )
            {
                if (opt_wrk.pad_move == 0)
                {
                    opt_wrk.pad_move++;
                }
                else
                {
                    opt_wrk.pad_move = 0;
                }

                if (opt_wrk.pad_move == 0)
                {
                    menu_opt.vive_flg = 60;
                }
                else
                {
                    menu_opt.vive_flg = 0;
                }

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                }
            }
        }
    break;
    case 2:
        if (yw2d.pad_lock == 0)
        {
            if (*key_now[4] == 1)
            {
                *mode = 7;

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(3, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(3, 0, 0x1000, 0x1000, 0);
                }
            }
            else if (*key_now[6] == 1 || *key_now[5] == 1)
            {
                *mode = 7;

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);
                }
            }
            else if (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            )
            {
                opt_wrk.sound_mode = 1 - opt_wrk.sound_mode;

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                }
            }
            else if (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && Ana2PadDirCnt(1) % 5 == 1)
            )
            {
                opt_wrk.sound_mode = 1 - opt_wrk.sound_mode;

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                }
            }
        }

        SeSetSteMono(opt_wrk.sound_mode);
    break;
    case 3:
        if (yw2d.pad_lock == 0)
        {
            if (*key_now[4] == 1)
            {
                *mode = 7;

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(3, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(3, 0, 0x1000, 0x1000, 0);
                }
            }
            else if (*key_now[6] == 1 || *key_now[5] == 1)
            {
                *mode = 7;

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);
                }
            }
            else if (*key_now[2] != 0 || Ana2PadDirCnt(3) != 0)
            {
                if (menu_opt.snd_vol - 64 < 1)
                {
                    menu_opt.snd_vol = 0;
                }
                else
                {
                    menu_opt.snd_vol -= 64;
                }

                SeSetMVol(menu_opt.snd_vol);

                opt_wrk.bgm_vol = menu_opt.snd_vol;
            }
            else if (*key_now[3] != 0 || Ana2PadDirCnt(1) != 0)
            {
                if (menu_opt.snd_vol + 64 < 0xfff)
                {
                    menu_opt.snd_vol += 64;
                }
                else
                {
                    menu_opt.snd_vol = 0xfff;
                }

                SeSetMVol(menu_opt.snd_vol);

                opt_wrk.bgm_vol = menu_opt.snd_vol;
            }
        }
    break;
    case 4:
        if (yw2d.pad_lock == 0)
        {
            if (*key_now[4] == 1)
            {
                *mode = 7;

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(3, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(3, 0, 0x1000, 0x1000, 0);
                }
            }
            else if (*key_now[6] == 1 || *key_now[5] == 1)
            {
                *mode = 7;

                if (ingame_wrk.stts & 0x80)
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);
                }
            }
        }
    break;
    }

    return 0;
}

static void MenuOptionDisp(u_char csr, u_char mode, u_char io)
{
    OptionCntInit();

    if (io == 0)
    {
        OptionModeInOut();
    }

    DspMenuTitle(0, 0, yw2d.io_a[1], 0x32, 9);
    DspOptCtrl(csr, mode, yw2d.io_a[4], 1);

    if (io == 0)
    {
        OptionModeInOut2();
    }

    OptionCntRenew();
}

static void OptionCntInit()
{
    return;
}

static void OptionCntRenew()
{
    if (yw2d.inn_mode_cnt < 65535)
    {
        yw2d.inn_mode_cnt++;
    }

    if (yw2d.out_mode_cnt != 0)
    {
        yw2d.out_mode_cnt--;
    }
}

static void OptionModeInOut()
{
    int i;
    float fade_in;
    float fade_out;

    if (yw2d.inn_mode_cnt < 11)
    {
        fade_in = (yw2d.inn_mode_cnt * 128) / 10.0f;
        fade_out = (1.0f - yw2d.inn_mode_cnt / 10.0f) * 128.0f;

        yw2d.io_a[0] = 128.0f;
        yw2d.io_a[1] = fade_in;
        yw2d.io_a[2] = fade_out;
        yw2d.io_a[3] = fade_out;
        yw2d.io_a[4] = fade_in;

        PlayerStatusYW(yw2d.io_a[2]);
        DspTopMenu(yw2d.io_x[3], yw2d.io_y[3], yw2d.io_a[3], 0);
        DspPlayData(yw2d.io_x[2], yw2d.io_y[2], yw2d.io_a[2], 1);

        if (yw2d.inn_mode_cnt == 10)
        {
            yw2d.pad_lock = 0;
        }
    }
    else if (yw2d.out_mode_cnt != 0)
    {
        fade_in = (1.0f - (yw2d.out_mode_cnt - 1) / 10.0f) * 128.0f;
        fade_out = ((yw2d.out_mode_cnt - 1) * 128) / 10.0f;

        yw2d.io_a[0] = 128.0f;
        yw2d.io_a[1] = fade_out;
        yw2d.io_a[2] = fade_in;
        yw2d.io_a[3] = fade_in;
        yw2d.io_a[4] = fade_out;

        PlayerStatusYW(yw2d.io_a[2]);
        DspTopMenu(yw2d.io_x[3], yw2d.io_y[3], yw2d.io_a[3], 0);
        DspPlayData(yw2d.io_x[2], yw2d.io_y[2], yw2d.io_a[2], 1);

        yw2d.pad_lock = 1;

        if (yw2d.out_mode_cnt == 1)
        {
            ig_menu_wrk.mode = IGMENU_MODE_SLCT;

            IngameMenuModeSlctDispInit();

            yw2d.menu_io_flg = 0;
            yw2d.pad_lock = 0;
        }
    }
    else
    {
        for (i = 0; i < 7; i++)
        {
            XYAdefaultYW(i);
        }
    }
}

static void OptionModeInOut2()
{
    if (yw2d.inn_mode_cnt < 11)
    {
        DspPlayData(yw2d.io_x[2], yw2d.io_y[2], yw2d.io_a[2], 2);
    }
    else if (yw2d.out_mode_cnt != 0)
    {
        DspPlayData(yw2d.io_x[2], yw2d.io_y[2], yw2d.io_a[2], 2);
    }
}

void DspOptCtrl(u_char csr, u_char mode, u_char alpha, u_char ttl)
{
    switch (mode)
    {
    case 7:
        if (yw2d.pad_lock != 0)
        {
            DspAlpCtrl(&menu_opt.fade_alp[0], 1, alpha);
            DspAlpCtrl(&menu_opt.fade_alp[1], 2, alpha);
            DspAlpCtrl(&menu_opt.fade_alp[2], 2, alpha);
        }
        else
        {
            DspAlpCtrl(&menu_opt.fade_alp[0], 3, alpha);
            DspAlpCtrl(&menu_opt.fade_alp[1], 2, alpha);
            DspAlpCtrl(&menu_opt.fade_alp[2], 2, alpha);
        }
    break;
    case 0:
        DspAlpCtrl(&menu_opt.fade_alp[0], 2, alpha);
        DspAlpCtrl(&menu_opt.fade_alp[1], 3, alpha);
        DspAlpCtrl(&menu_opt.fade_alp[2], 2, alpha);
    break;
    case 4:
        DspAlpCtrl(&menu_opt.fade_alp[0], 2, alpha);
        DspAlpCtrl(&menu_opt.fade_alp[1], 2, alpha);
        DspAlpCtrl(&menu_opt.fade_alp[2], 3, alpha);
    break;
    }

    DspOptMain(csr, mode, menu_opt.fade_alp[0], menu_opt.fade_alp[1], menu_opt.fade_alp[2], ttl);
}

static void DspAlpCtrl(u_char *src, u_char mode, u_char alpha)
{
    switch (mode)
    {
    case 0:
        *src = 0;
    break;
    case 1:
        *src = alpha;
    break;
    case 2:
        if (*src - 12 > 0)
        {
            *src -= 12;
        }
        else
        {
            *src = 0;
        }
    break;
    case 3:
        if (*src + 12 < alpha)
        {
            *src += 12;
        }
        else
        {
            *src = alpha;
        }
    break;
    }
}

static void DspOptMain(u_char csr, u_char mode, u_char alp0, u_char alp1, u_char alp2, u_char ttl)
{
    DspPadType(alp1, ttl);
    DspLgtScrn(alp2, ttl);
    DspBigFlame1(alp0, 0);
    DspSmlFlame1(alp0, 0);
    DspBigFlame2(alp0, 0);
    DspSmlFlame2(alp0, 0);
    DspOptCsr(csr, mode, alp0);
    DspBigFlame1(alp0, 1);
    DspSmlFlame1(alp0, 1);
    DspBigFlame2(alp0, 1);
    DspSmlFlame2(alp0, 1);
    DspMsgWin(csr, alp0);
    DispCaption(1, alp0);
}

static void DspOptCsr(u_char csr, u_char mode, u_char alpha)
{
    OPT_CSR_POS csr_pos[12] = {
        { .x = 131, .y =  97 }, { .x = 131, .y = 129 }, { .x = 131, .y = 175 }, { .x = 131, .y = 207 },
        { .x = 131, .y = 253 }, { .x = 131, .y = 299 }, { .x = 365, .y =  97 }, { .x = 365, .y = 129 },
        { .x = 365, .y = 175 }, { .x = 365, .y = 207 }, { .x = 365, .y = 253 }, { .x = 365, .y = 299 },
    };
    int rgb1;
    int rgb2;

    if (mode == 7)
    {
        FlashStarYW(&flsh[0], 64, 96, 90, 0);
        CLRtoRGB(&rgb1, flsh[0].alpha, flsh[0].alpha, flsh[0].alpha);
        FlashStarYW(&flsh[1], 64, 96, 90, 0xff);
        CLRtoRGB(&rgb2, flsh[1].alpha, flsh[1].alpha, flsh[1].alpha);
    }
    else
    {
        FlashStarYW(&flsh[0], 64, 96, 90, 0xff);
        CLRtoRGB(&rgb1, flsh[0].alpha, flsh[0].alpha, flsh[0].alpha);
        FlashStarYW(&flsh[1], 64, 96, 90, 0);
        CLRtoRGB(&rgb2, flsh[1].alpha, flsh[1].alpha, flsh[1].alpha);
    }

    CmnCursol(csr_pos[csr].x, csr_pos[csr].y, 212, 32, flsh[0].alpha, alpha, 0x28000);

    if (mode == 1 || mode == 2)
    {
        CmnCursol(csr_pos[csr+6].x, csr_pos[csr+6].y, 194, 32, flsh[1].alpha, alpha, 0x28000);
    }

    if (mode == 1 || mode == 2 || mode == 3)
    {
        PutSpriteYW(MNU_OPT_SEL_TRI_L, MNU_OPT_SEL_TRI_R, csr_pos[csr+6].x, csr_pos[csr+6].y, 0.0f, rgb2, alpha, 1.0f, 1.0f, 0, 0x14000, 1, 1, 0);
    }
}

static void DspBigFlame1(u_char alpha, u_char str_mode)
{
    if (alpha == 0)
    {
        return;
    }

    if (str_mode == 0)
    {
        CmnWindow(15, 0, 0, alpha, 0x80);

        PutSpriteYW(MNU_OPT_ICON_00, MNU_OPT_ICON_00, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }
    else
    {
        PutStringYW(28, 0, 235, 101, 0x808080, alpha, 0x23000, 1);
        PutStringYW(28, opt_wrk.key_type + 7, 464, 101, 0x808080, alpha, 0x23000, 1);
        PutStringYW(28, 1, 235, 133, 0x808080, alpha, 0x23000, 1);

        if (opt_wrk.pad_move == 0)
        {
            PutStringYW(28, 15, 464, 133, 0x808080, alpha, 0x23000, 1);
        }
        else
        {
            PutStringYW(28, 0x10, 464, 133, 0x808080, alpha, 0x23000, 1);
        }

        if (menu_opt.vive_flg != 0)
        {
            VibrateRequest2(0, 120);
        }
        else
        {
            VibrateRequest2(0, 0);
        }

        if (menu_opt.vive_flg != 0)
        {
            menu_opt.vive_flg--;
        }
    }
}

static void DspBigFlame2(u_char alpha, u_char str_mode)
{
    DISP_SPRT ds;
    short pos_x;

    if (alpha == 0)
    {
        return;
    }

    if (str_mode == 0)
    {
        CmnWindow(15, 0, 78, alpha, 0x80);

        PutSpriteYW(MNU_OPT_ICON_01, MNU_OPT_ICON_01, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(MNU_OPT_VOLUME_FRM, MNU_OPT_VOLUME_FRM, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        pos_x = (menu_opt.snd_vol * 162) / 4095.0f;

        CopySprDToSpr(&ds, &spr_dat[559]);

        ds.w = pos_x + 1;

        ds.alpha = alpha;

        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

        DispSprD(&ds);

        PutSpriteYW(MNU_OPT_SND_TUNE, MNU_OPT_SND_TUNE, pos_x, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0 , 0);
    }
    else
    {
        PutStringYW(28, 2, 235, 179, 0x808080, alpha, 0x23000, 1);
        PutStringYW(28, opt_wrk.sound_mode + 17, 464, 179, 0x808080, alpha, 0x23000, 1);
        PutStringYW(28, 3, 235, 211, 0x808080, alpha, 0x23000, 1);
    }
}

static void DspSmlFlame1(u_char alpha, u_char str_mode)
{
    if (alpha == 0)
    {
        return;
    }

    if (str_mode == 0)
    {
        CmnWindow(16, 0, 0, alpha, 0x80);

        PutSpriteYW(MNU_OPT_ICON_02, MNU_OPT_ICON_02, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 0.8f, 0.55f, 0, 0xff, 1, 0, 0);
    }
    else
    {
        PutStringYW(28, 4, 235, 258, 0x808080, alpha, 0x23000, 1);
    }
}

static void DspSmlFlame2(u_char alpha, u_char str_mode)
{
    if (alpha == 0)
    {
        return;
    }

    if (str_mode == 0)
    {
        CmnWindow(16, 0, 46, alpha, 0x80);

        PutSpriteYW(MNU_OPT_ICON_03, MNU_OPT_ICON_03, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }
    else
    {
        PutStringYW(28, 6, 235, 304, 0x808080, alpha, 0x23000, 1);
    }
}

static void DspMsgWin(u_char csr, u_char alpha)
{
    if (alpha == 0)
    {
        return;
    }

    DrawMessageBox(0x2d000, 26.0f, 340.0f, 588.0f, 80.0f, alpha);

    PutStringYW(29, csr, 48, 356, 0x808080, alpha, 0x23000, 0);
}

void StepLinesSimpleDraw(short int (*p_stp)[2], u_char r, u_char g, u_char b, u_char alpha)
{
    int i;

    i = 0;

    while (1)
    {
        if (p_stp[i][0] == 0x7fff)
        {
            break;
        }

        if (p_stp[i+1][0] == 0x7fff)
        {
            break;
        }

        if (p_stp[i+1][0] == 0x7ff0)
        {
            i += 2;
        }
        else
        {
            SetLine(0x2d000, p_stp[i][0] - 320, p_stp[i][1] - 224, p_stp[i+1][0] - 320, p_stp[i+1][1] - 224, r, g, b, alpha);

            i++;
        }
    }
}

void SimpleHorLine(short int (*p_stp)[3], u_char r, u_char g, u_char b, u_char alpha, int typ)
{
    int i;

    i = 0;

    while (1)
    {
        if (p_stp[i][0] == 0x7fff)
        {
            break;
        }

        if ((typ == 1 || typ == 3) && i == 6)
        {
            SetLine(0x1e000, p_stp[i][0] - 320, p_stp[i][1] - 204, p_stp[i][0] + p_stp[i][2] - 320, p_stp[i][1] - 204, r, g, b, alpha);
            SetLine(0x1e000, p_stp[i][0] - 320, p_stp[i][1] - 203, p_stp[i][0] + p_stp[i][2] - 320, p_stp[i][1] - 203, r, g, b, alpha);
        }
        else
        {
            SetLine(0x1e000, p_stp[i][0] - 320, p_stp[i][1] - 224, p_stp[i][0] + p_stp[i][2] - 320, p_stp[i][1] - 224, r, g, b, alpha);
            SetLine(0x1e000, p_stp[i][0] - 320, p_stp[i][1] - 223, p_stp[i][0] + p_stp[i][2] - 320, p_stp[i][1] - 223, r, g, b, alpha);
        }

        i++;
    }
}

static void DspPadType(u_char alpha, u_char ttl)
{
    int i;
    int typ;
    int rev;
    int mov;
    SPRT_DAT ssd;
    DISP_SPRT ds;
    short int pnt_stp3[12][3] = {
        {    10,   135,   170 }, {    10,   170,   170 }, {    10,   235,   170 },
        {    10,   295,   170 }, {    10,   350,   170 }, {   460,   135,   170 },
        {   460,   170,   170 }, {   460,   235,   170 }, {   460,   295,   170 },
        {   460,   350,   170 }, {   460,   380,   170 }, { 32767,     0,     0 },
    };
    short int pnt_stp4[62][2] = {
        {    10,   135 }, {   190,   135 }, {   190,   145 }, {   230,   145 }, { 32752,     0 }, {    10,   170 },
        {   190,   170 }, {   190,   160 }, {   230,   160 }, { 32752,     0 }, {    10,   235 }, {   190,   235 },
        {   190,   225 }, {   230,   225 }, { 32752,     0 }, {    10,   295 }, {   190,   295 }, {   190,   285 },
        {   285,   285 }, {   285,   270 }, { 32752,     0 }, {    10,   354 }, {   190,   354 }, {   190,   344 },
        {   360,   344 }, {   360,   280 }, { 32752,     0 }, {   630,   135 }, {   445,   135 }, {   445,   145 },
        {   410,   145 }, { 32752,     0 }, {   630,   170 }, {   445,   170 }, {   445,   160 }, {   410,   160 },
        { 32752,     0 }, {   630,   222 }, {   445,   222 }, {   445,   205 }, {   410,   205 }, { 32752,     0 },
        {   630,   274 }, {   445,   274 }, {   445,   264 }, {   428,   264 }, {   428,   242 }, { 32752,     0 },
        {   630,   326 }, {   445,   326 }, {   445,   316 }, {   406,   316 }, {   406,   260 }, { 32752,     0 },
        {   630,   378 }, {   445,   378 }, {   445,   368 }, {   380,   368 }, {   380,   240 }, { 32752,     0 },
        { 32767,     0 }, {     0,     0 },
    };
    short int pnt_stp5[62][2] = {
        {    10,   135 }, {   190,   135 }, {   190,   145 }, {   230,   145 }, { 32752,     0 }, {    10,   170 },
        {   190,   170 }, {   190,   160 }, {   230,   160 }, { 32752,     0 }, {    10,   235 }, {   190,   235 },
        {   190,   225 }, {   230,   225 }, { 32752,     0 }, {    10,   295 }, {   190,   295 }, {   190,   285 },
        {   285,   285 }, {   285,   270 }, { 32752,     0 }, {    10,   354 }, {   190,   354 }, {   190,   344 },
        {   360,   344 }, {   360,   280 }, { 32752,     0 }, {   630,   135 }, {   445,   135 }, {   445,   145 },
        {   410,   145 }, { 32752,     0 }, {   630,   190 }, {   445,   190 }, {   445,   160 }, {   410,   160 },
        { 32752,     0 }, {   630,   242 }, {   445,   242 }, {   445,   205 }, {   410,   205 }, { 32752,     0 },
        {   630,   274 }, {   445,   274 }, {   445,   264 }, {   428,   264 }, {   428,   242 }, { 32752,     0 },
        {   630,   326 }, {   445,   326 }, {   445,   316 }, {   406,   316 }, {   406,   260 }, { 32752,     0 },
        {   630,   378 }, {   445,   378 }, {   445,   368 }, {   380,   368 }, {   380,   240 }, { 32752,     0 },
        { 32767,     0 }, {     0,     0 },
    };
    short int pnt_stp6[5][2] = {
        {    30,   190 }, {   460,   190 }, {   460,   160 }, {   410,   160 }, { 32752,     0 },
    };
#ifdef BUILD_EU_VERSION
    short int p_idx4[18][2] = {
        {  15, 111 }, {  15, 147 }, {  15, 192 }, {  15, 212 }, {  15, 252 }, {  15, 272 }, {  15, 310 },
        {  15, 330 }, { 445, 111 }, { 445, 147 }, { 445, 179 }, { 445, 199 }, { 445, 231 }, { 445, 251 },
        { 445, 283 }, { 445, 303 }, { 445, 335 }, { 445, 355 },
    };
    short int p_idx5[18][2] = {
        {  15, 111 }, {  15, 147 }, {  15, 192 }, {  15, 212 }, {  15, 252 }, {  15, 272 }, {  15, 310 },
        {  15, 330 }, { 445, 111 }, { 445, 147 }, { 445, 167 }, { 445, 199 }, { 445, 219 }, { 445, 251 },
        { 445, 283 }, { 445, 303 }, { 445, 335 }, { 445, 355 },
    };
#else
    short int p_idx4[18][2] = {
        {  15, 113 }, {  15, 147 }, {  15, 192 }, {  15, 212 }, {  15, 252 }, {  15, 272 }, {  15, 310 },
        {  15, 330 }, { 445, 113 }, { 445, 147 }, { 445, 179 }, { 445, 199 }, { 445, 231 }, { 445, 251 },
        { 445, 283 }, { 445, 303 }, { 445, 335 }, { 445, 355 },
    };
    short int p_idx5[18][2] = {
        {  15, 113 }, {  15, 147 }, {  15, 192 }, {  15, 212 }, {  15, 252 }, {  15, 272 }, {  15, 310 },
        {  15, 330 }, { 445, 113 }, { 445, 147 }, { 445, 167 }, { 445, 199 }, { 445, 219 }, { 445, 251 },
        { 445, 283 }, { 445, 303 }, { 445, 335 }, { 445, 355 },
    };
#endif
    short int f_idx[3][18] = {
        { 16, 14, 14, 17, 17, 18, 18, 15, 15, 21, 21, 22, 22, 23, 23, 24, 24, 16 },
        { 14, 14, 17, 17, 18, 18, 15, 15, 21, 21, 22, 22, 23, 23, 23, 24, 16, 14 },
        { 14, 17, 17, 18, 18, 15, 15, 21, 21, 22, 22, 23, 23, 22, 24,  0,  0,  0 }
    };
#ifdef BUILD_EU_VERSION
    short int f_idx2[4][18] = {
        { 595, 586, 587, 578, 587, 579, 588, 580, 595, 589, 593, 583, 590, 584, 589, 581, 594, 582 },
        { 595, 586, 587, 585, 587, 585, 588, 580, 595, 589, 593, 583, 590, 584, 589, 581, 594, 582 },
        { 595, 586, 587, 578, 587, 579, 588, 580, 595, 590, 584, 593, 583, 589, 589, 581, 594, 582 },
        { 595, 586, 587, 585, 587, 585, 588, 580, 595, 590, 584, 593, 583, 589, 589, 581, 594, 582 }
    };
#else
    short int f_idx2[4][18] = {
        { 578, 586, 587, 578, 587, 579, 588, 580, 578, 589, 593, 583, 590, 584, 589, 581, 594, 582 },
        { 578, 586, 587, 585, 587, 585, 588, 580, 578, 589, 593, 583, 590, 584, 589, 581, 594, 582 },
        { 578, 586, 587, 578, 587, 579, 588, 580, 578, 590, 584, 593, 583, 589, 589, 581, 594, 582 },
        { 578, 586, 587, 585, 587, 585, 588, 580, 578, 590, 584, 593, 583, 589, 589, 581, 594, 582 }
    };
#endif
    short int f_col[4][18] = {
        { 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0 },
        { 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0 },
        { 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0 },
        { 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0 }
    };

    if (alpha == 0)
    {
        return;
    }

    if (ttl != 0)
    {
        CmnWindow(0, 0, 0, alpha, 0x80);

        PutStringYW(28, 0, 274, 37, 0x808080, alpha, 0x23000, 0);
    }

    if (opt_wrk.key_type == 0)
    {
#ifdef BUILD_EU_VERSION
        typ = 0;
        rev = 0;
#else
        typ = 0;
        mov = 0;
        rev = 0;
#endif
    }
    else if (opt_wrk.key_type == 1)
    {
#ifdef BUILD_EU_VERSION
        typ = 1;
        rev = 0;
#else
        typ = 1;
        mov = 0;
        rev = 0;
#endif
    }
    else if (opt_wrk.key_type == 2)
    {
#ifdef BUILD_EU_VERSION
        typ = 2;
        rev = 0;
#else
        typ = 2;
        mov = 1;
        rev = 0;
#endif
    }
    else if (opt_wrk.key_type == 3)
    {
#ifdef BUILD_EU_VERSION
        typ = 3;
        rev = 0;
#else
        typ = 3;
        mov = 1;
        rev = 0;
#endif
    }
    else if (opt_wrk.key_type == 4)
    {
#ifdef BUILD_EU_VERSION
        typ = 0;
        rev = 1;
#else
        typ = 0;
        mov = 0;
        rev = 1;
#endif
    }
    else if (opt_wrk.key_type == 5)
    {
#ifdef BUILD_EU_VERSION
        typ = 1;
        rev = 1;
#else
        typ = 1;
        mov = 0;
        rev = 1;
#endif
    }
    else if (opt_wrk.key_type == 6)
    {
#ifdef BUILD_EU_VERSION
        typ = 2;
        rev = 1;
#else
        typ = 2;
        mov = 1;
        rev = 1;
#endif
    }
    else if (opt_wrk.key_type == 7)
    {
#ifdef BUILD_EU_VERSION
        typ = 3;
        rev = 1;
#else
        typ = 3;
        mov = 1;
        rev = 1;
#endif
    }
    else
    {
#ifdef BUILD_EU_VERSION
        typ = 0;
        rev = 0;
#else
        typ = 0;
        mov = 0;
        rev = 0;
#endif
    }

#ifdef BUILD_EU_VERSION
    ssd = spr_dat[596];
#else
    ssd = spr_dat[595];
#endif

    ssd.alpha = alpha;

    CopySprDToSpr(&ds, &ssd);
    DispSprD(&ds);

#ifdef BUILD_EU_VERSION
    ssd = spr_dat[597];
#else
    ssd = spr_dat[596];
#endif

    ssd.alpha = alpha;

    CopySprDToSpr(&ds, &ssd);

    ds.att |= 2;

    DispSprD(&ds);


#ifdef BUILD_EU_VERSION
    ssd = spr_dat[598];
#else
    ssd = spr_dat[597];
#endif

    ssd.alpha = alpha;

    CopySprDToSpr(&ds, &ssd);
    DispSprD(&ds);

#ifdef BUILD_EU_VERSION
    ssd = spr_dat[599 + typ + rev * 4];
#else
    ssd = spr_dat[598 + typ + rev * 4];
#endif

    ssd.alpha = alpha;

    CopySprDToSpr(&ds, &ssd);
    DispSprD(&ds);

    PutSpriteYW(MENU_OPT_CNT00, MENU_OPT_CNT04, 0.0f, -82.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

#ifdef BUILD_EU_VERSION
#else
    DrawButtonTex(0x2d000, 1, 600.0f, mov ? 215.0f : 195.0f, alpha);
    DrawButtonTex(0x2d000, 2, 600.0f, 247.0f, alpha);
    DrawButtonTex(0x2d000, 3, 600.0f, 299.0f, alpha);
    DrawButtonTex(0x2d000, 0, 600.0f, 351.0f, alpha);
#endif

    if (typ == 0 || typ == 1)
    {
        StepLinesSimpleDraw(pnt_stp4, 166, 166, 166, alpha);
    }
    else
    {
        StepLinesSimpleDraw(pnt_stp5, 166, 166, 166, alpha);
    }

    ssd = spr_dat[591];

    ssd.alpha = alpha;

    CopySprDToSpr(&ds, &ssd);
    DispSprD(&ds);

    ssd = spr_dat[592];

    ssd.alpha = alpha;

    CopySprDToSpr(&ds, &ssd);
    DispSprD(&ds);

    ssd = spr_dat[566];

    ssd.x += 230;
    ssd.y += 90;

    ssd.alpha = alpha;

    CopySprDToSpr(&ds, &ssd);
    DispSprD(&ds);

    ssd = spr_dat[567];

    ssd.x += 230;
    ssd.y += 90;

    ssd.alpha = alpha;

    CopySprDToSpr(&ds, &ssd);
    DispSprD(&ds);

    CmnWindow(40, 0, 0, alpha, 0x80);

    ssd = spr_dat[587];

    CopySprDToSpr(&ds, &ssd);

    ds.r = 0;
    ds.g = 0;
    ds.b = 0;

    ds.x = 40.0f;
    ds.y = 373.0f;

    ds.alpha = alpha;

    DispSprD(&ds);

    ds.x = 41.0f;
    ds.y = 373.0f;

    DispSprD(&ds);

    ssd = spr_dat[575];
    ssd.alpha = alpha;

    CopySprDToSpr(&ds, &ssd);
    DispSprD(&ds);

    ssd = rev ? spr_dat[577] : spr_dat[576];
    ssd.alpha = alpha;

    CopySprDToSpr(&ds, &ssd);
    DispSprD(&ds);

    for (i = 0; i < 18; i++)
    {
        ssd = spr_dat[f_idx2[typ][i]];

        if (typ < 2)
        {
            ssd.x = p_idx4[i][0];
            ssd.y = p_idx4[i][1];
        }
        else
        {
            ssd.x = p_idx5[i][0];
            ssd.y = p_idx5[i][1];
        }

        ssd.alpha = alpha;

        CopySprDToSpr(&ds, &ssd);
        DispSprD(&ds);
    }

    DispCaption(7, alpha);
}

static void DspLgtScrn(u_char alpha, u_char ttl)
{
    if (alpha == 0)
    {
        return;
    }

    DrawMessageBox(0x64000, 36.0f, 307.0f, 568.0f, 120.0f, alpha);

    PutStringYW(29, 7, 77, 332, 0x808080, alpha, 0xf000, 0);

    PutSpriteYW(BRGHT_PHT, BRGHT_PHT, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(BRGHT_SEN, BRGHT_SEN, 149.0f, 262.0f, 0.0f, 0x808080, alpha, 1.0175439f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(BRGHT_SEN, BRGHT_SEN, 111.0f, 127.0f, 0.0f, 0x808080, alpha, 0.3508772f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(BRGHT_SEN, BRGHT_SEN, 412.0f, 125.0f, 0.0f, 0x808080, alpha, 0.5877193f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(BRGHT_SEN, BRGHT_SEN, 427.0f, 263.0f, 0.0f, 0x808080, alpha, 0.32456142f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(BRGHT_ONE, BRGHT_ONE, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(BRGHT_TWO, BRGHT_TWO, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(BRGHT_THR, BRGHT_THR, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(BRGHT_FOR, BRGHT_FOR, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    if (ttl != 0)
    {
        CmnWindow(0, 0, 0, alpha, 0x80);

        PutStringYW(28, 4, 262, 37, 0x808080, alpha, 0xf000, 0);
    }

    DispCaption(6, alpha);
}
