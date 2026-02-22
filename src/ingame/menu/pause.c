#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "pause.h"

#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/g2d_debug.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph2d/tim2.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/ig_spd_menu.h"
#include "ingame/menu/item.h"
#include "ingame/menu/pause.h"
#include "main/gamemain.h"
#include "main/glob.h"
#include "os/eeiop/adpcm/ea_ctrl.h"
#include "os/eeiop/eese.h"
#include "outgame/btl_mode/btl_menu.h"
#include "outgame/btl_mode/btl_mode.h"

static void PauseDraw(u_char alp);
static char CanPauseCHK();

PAD_CTRL pad_ctrl = {0};
PAUSE_WRK pause_wrk = {0};
char in_pause = 0;

static PAUSE_DSP ps_dsp;
static FLSH_CORE flsh;

void PauseInit()
{
    pause_wrk = (PAUSE_WRK){0};
    ps_dsp = (PAUSE_DSP){0};

    gra2dInitDbgMenu();

    in_pause = 0;
}

int PauseMain()
{
    if (*key_now[12] == 1 || (pad[0].flags & 0x1) == 0 && pause_wrk.mode == PAUSE_MODE_NO_REQ)
    {
        if (pause_wrk.mode == PAUSE_MODE_NO_REQ)
        {
            if (CanPauseCHK() != 0) {
                SeStartFix(1, 0, 0x1000, 0x1000, 1);
                CaptureScreen(0x1a40);
                DrawScreen(0x7f000, 0x1a40, 0x80, 0x80, 0x80, 0x80);

                pause_wrk.csr[0] = 0;
                pause_wrk.timer = 0;

                ingame_wrk.stts |= 0x80;

                pause_wrk.mode = PAUSE_MODE_MENU;

                ps_dsp.yn_mode = 0;
                ps_dsp.msg_alp = 0;
                ps_dsp.alp = 0;

                SetTargetVolSeMenuFade(0x600);
                SetTargetVolAdpcmMenuFade(0x600);
            }
        }
        else if (pause_wrk.mode == PAUSE_MODE_MENU)
        {
            pause_wrk.mode = PAUSE_MODE_MENU_OUT;
            SeStartFix(3, 0, 0x1000, 0x1000, 1);
        }
    }
    else if (pause_wrk.mode == PAUSE_MODE_MENU_OUT)
    {
        if (in_pause == 0)
        {
            ingame_wrk.stts &= 0x40 | 0x10 | 0x8 | 0x4 | 0x2 | 0x1;

            SetTargetVolSeMenuFade(0xfff);
            SetTargetVolAdpcmMenuFade(0xfff);

            pause_wrk.mode = PAUSE_MODE_NO_REQ;

            return 1;
        }
    }
    else if (dbg_wrk.mode_on != 0)
    {
        // debug code ?
    }
    else if (pause_wrk.mode == PAUSE_MODE_MENU)
    {
        if (pause_wrk.timer == 0)
        {
            ingame_wrk.stts |= 0x20;
        }

        if (ps_dsp.yn_mode == 0)
        {
            if (*key_now[4] == 1)
            {
                SeStartFix(3, 0, 0x1000, 0x1000, 1);

                pause_wrk.mode = PAUSE_MODE_MENU_OUT;
            }
            else if (*key_now[6] == 1 || *key_now[5] == 1)
            {
                SeStartFix(1, 0, 0x1000, 0x1000, 1);

                *key_now[4] = 2;

                switch (pause_wrk.csr[0])
                {
                case 0:
                    *key_now[5] = 2;

                    pause_wrk.mode = PAUSE_MODE_MENU_OUT;
                break;
                case 1:
                    SpdOptStart();

                    pause_wrk.mode = PAUSE_MODE_NO_REQ;
                break;
                case 2:
                    pause_wrk.csr[1] = 1;

                    ps_dsp.yn_mode = 1;
                break;
                }
            }
            else if (
                *key_now[0] == 1 ||
                (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
                Ana2PadDirCnt(0) == 1 ||
                (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
            )
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 1);

                if (pause_wrk.csr[0] != 0)
                {
                    pause_wrk.csr[0]--;
                }
                else
                {
                    pause_wrk.csr[0] = 2;
                }
            }
            else if (
                *key_now[1] == 1 ||
                (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
                Ana2PadDirCnt(2) == 1 ||
                (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
            )
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 1);

                if (pause_wrk.csr[0] < 2)
                {
                    pause_wrk.csr[0]++;
                }
                else
                {
                    pause_wrk.csr[0] = 0;
                }
            }

        }
        else if (*key_now[4] != 0)
        {
                SeStartFix(3, 0, 0x1000, 0x1000, 1);

                *key_now[4] = 2;

                ps_dsp.yn_mode = 0;
        }
        else if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            if (pause_wrk.csr[1] == 0)
            {
                if (ingame_wrk.game == 1)
                {
                    LoadStoryWrk();
                    SetBattleEnd();
                }

                GameModeChange(1);
                EAdpcmFadeOutGameEnd(0x1e);

                SetTargetVolSeMenuFade(0xfff);
                SetTargetVolAdpcmMenuFade(0xfff);
            }
            else
            {
                ps_dsp.yn_mode = 0;
            }

            SeStartFix(1, 0, 0x1000, 0x1000, 1);
        }
        else if (*key_now[2] == 1 || Ana2PadDirCnt(3) == 1)
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 1);

            pause_wrk.csr[1] = 1 - pause_wrk.csr[1];
        }
        else if (*key_now[3] == 1 || Ana2PadDirCnt(1) == 1)
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 1);

            pause_wrk.csr[1] = 1 - pause_wrk.csr[1];
        }

    }

    if (pause_wrk.timer < 0xffff)
    {
        pause_wrk.timer++;
    }

    return 0;
}

void PauseDisp()
{
    if (pause_wrk.mode == PAUSE_MODE_NO_REQ || pause_wrk.mode == PAUSE_MODE_MENU_OUT)
    {
        if (ps_dsp.alp - 6 > 0)
        {
            ps_dsp.alp -= 6;
        }
        else
        {
            ps_dsp.alp = 0;

            in_pause = 0;

            return;
        }
    }
    else
    {
        if (ps_dsp.alp + 6 < 0x80)
        {
            ps_dsp.alp += 6;
        }
        else
        {
            ps_dsp.alp = 0x80;
        }
    }

    in_pause = 0;

    if (ps_dsp.alp != 0 && spd_mnu.sopt == 0)
    {
        DrawScreen(0x7f000, 0x1a40, 0x80, 0x80, 0x80, 0x80);

        PauseDraw(ps_dsp.alp);

        in_pause = 1;
    }
}

static void PauseDraw(u_char alp)
{
    int i;

    SetSprFile(0x1ce0000);

    CmnWindow(20, 0, 0, alp, 0x80);

    PutSpriteYW(PSE_FNT, PSE_FNT, 0.0f, 0.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    FlashStarYW(&flsh, 96, 64, 90, 0);

#ifdef BUILD_EU_VERSION
    CmnCursol(145, pause_wrk.csr[0] * 29 + 181, 350, 29, flsh.alpha, alp, 0x78000);
#else
    CmnCursol(170, pause_wrk.csr[0] * 29 + 181, 300, 29, flsh.alpha, alp, 0x78000);
#endif

    for (i = 0; i < 3; i++)
    {
        if (ingame_wrk.game == 1 && i == 2)
        {
            PutStringYW(6, 35, 320, 242, 0x808080, alp, 0x75000, 1);
        }
        else
        {
            PutStringYW(6, i + 13, 320, i * 29 + 184, 0x808080, alp, 0x75000, 1);
        }
    }
    if (ps_dsp.yn_mode != 0x0)
    {
        if (ps_dsp.msg_alp + 8 < alp)
        {
            ps_dsp.msg_alp += 8;
        }
        else
        {
            ps_dsp.msg_alp = alp;
        }
    }
    else
    {
        if (ps_dsp.msg_alp - 8 > alp)
        {
            ps_dsp.msg_alp -= 8;
        }
        else
        {
            ps_dsp.msg_alp = 0;
        }
    }

    if (ps_dsp.msg_alp != 0)
    {
        DrawMessageBox(0xf000, 56.0f, 329.0f, 528.0f, 81.0f, ps_dsp.msg_alp);

#ifdef BUILD_EU_VERSION
        YesNoCrslOKR(0xa000, pause_wrk.csr[1] * 132 + 178, 368.0f, 0x808080, ps_dsp.msg_alp, 2.0f);
#else
        YesNoCrslOKR(0xa000, pause_wrk.csr[1] * 132 + 178, 366.0f, 0x808080, ps_dsp.msg_alp, 2.0f);
#endif

        if (ingame_wrk.game == 1)
        {
            PutStringYW(6, 36, 320, 346, 0x808080, ps_dsp.msg_alp, 0x5000, 1);
        }
        else
        {
            PutStringYW(6, 30, 320, 346, 0x808080, ps_dsp.msg_alp, 0x5000, 1);
        }

        PutStringYW(6, 31, 48, 371, 0x808080, ps_dsp.msg_alp, 0x5000, 0);
    }
}

static char CanPauseCHK()
{
    char can;

    can = ingame_wrk.mode == INGAME_MODE_NOMAL;

    if (plyr_wrk.sta & 0x8)
    {
        can = 0;
    }

    if (plyr_wrk.mode == PMODE_MSG_DISP)
    {
        can = 0;
    }

    if (plyr_wrk.mode == PMODE_FINDER_IN)
    {
        can = 0;
    }

    if (plyr_wrk.mode == PMODE_FINDER)
    {
        can = 0;
    }

    if (plyr_wrk.mode == PMODE_FINDER_END)
    {
        can = 0;
    }

    if (plyr_wrk.mode == PMODE_FIN_CAM)
    {
        can = 0;
    }

    if (eff_filament_off != 0)
    {
        can = 0;
    }

    if (pad_ctrl.no_pause != 0)
    {
        pad_ctrl.no_pause = 0;

        can = 0;
    }

    return can;
}
