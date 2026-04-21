#include "common.h"
#include "typedefs.h"
#include "addresses.h"
#include "structs.h"
#include "enums.h"
#include "mode_slct.h"

// newlib/libm/math/s_sin.c
double sin(double x);

#include "main/glob.h"
#include "main/gamemain.h"
#include "outgame/title.h"
#include "mc/mc_main.h"
#include "outgame/btl_mode/btl_mode.h"
#include "outgame/btl_mode/btl_dat.h"
#include "ingame/menu/ig_option.h"
#include "ingame/menu/sp_menu.h"
#include "ingame/menu/ig_camra.h"
#include "ingame/menu/ig_album.h"
#include "ingame/menu/item.h"
#include "ingame/menu/ig_file.h"
// #include "ingame/menu/ig_glst.h"
#include "ingame/menu/ig_menu.h"
#include "outgame/sound_test.h"
#include "outgame/outgame.h"
#include "outgame/btl_mode/btl_menu.h"
#include "outgame/btl_mode/btl_mode.h"
#include "graphics/graph2d/sprt.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/motion/mdlwork.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph3d/sglib.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/adpcm/ea_ctrl.h"
#include "os/eeiop/adpcm/ea_dat.h"
#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/eese.h"

#include "data/msl_bg_btl.h" // SPRT_DAT msl_bg_btl[11];
#include "data/msl_bg_fsm.h" // SPRT_DAT msl_bg_fsm[11];
#include "data/msl_bg_opt.h" // SPRT_DAT msl_bg_opt[11];
#include "data/msl_bg_snd.h" // SPRT_DAT msl_bg_snd[11];
#include "data/msl_bg_sty.h" // SPRT_DAT msl_bg_sty[11];
#include "data/msel_sprt.h"  // SPRT_DAT msel_sprt[140(jp)|148(us,eu)];

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
static u_char sm_csr_info[2] = { 0, 0 };
#endif

int cmn_tex_load = 0;
int title_bgm_no = -1;
static DSP_M_SLCT_WRK dsp_ms;
static int ms_load_id;

#if defined(BUILD_JP_VERSION)
#define PAD_ACTION_CONFIRM PAD_CIRCLE
#define PAD_ACTION_EXIT PAD_CROSS
#elif defined(BUILD_US_VERSION)
#define PAD_ACTION_CONFIRM PAD_CROSS
#define PAD_ACTION_EXIT PAD_TRIANGLE
#elif defined(BUILD_EU_VERSION)
#define PAD_ACTION_CONFIRM PAD_CROSS
#define PAD_ACTION_EXIT PAD_TRIANGLE
#endif

#define PI 3.1415927f

void ModeSlctInit(u_char top,u_char end)
{
    ms_load_id = VER_LOAD_REQ_LANG(M_SLCT_CMN_PK2, LOAD_ADDRESS_10);

    dsp_ms = (DSP_M_SLCT_WRK){0};
    dsp_ms.now_mode = top;
    dsp_ms.end_mode = end;

    cmn_tex_load = 1;

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    dsp_ms.unsel_col[0] = 20;
    dsp_ms.unsel_col[1] = 15;
    dsp_ms.unsel_col[2] = 10;
    dsp_ms.unsel_col[3] = 128;
#endif
}

void ModeSlctLoop()
{
    ModeSlctCtrl(dsp_ms.now_mode);

    dsp_ms.mode_tmr++;
}

void ModeSlctCtrl(u_char mode)
{
    int adpcm_no;

    if (ms_load_id != -1 && IsLoadEnd(ms_load_id))
    {
        ms_load_id = -1;
    }

    switch (dsp_ms.main_step)
    {
    case MS_MODE_START:
        if (cmn_tex_load == 0)
        {
            ms_load_id = VER_LOAD_REQ_LANG(M_SLCT_CMN_PK2, LOAD_ADDRESS_10);
        }

        adpcm_no = -1;

        MsLoadCtrl(mode);

        dsp_ms.main_step = MS_MODE_INIT;

        if (dsp_ms.now_mode == MS_BATTLE_MODE || dsp_ms.now_mode == MS_MISSION_SELECT)
        {
            adpcm_no = AB000_STR;

            SeStopAll();
        }
        else if (dsp_ms.now_mode == MS_MODE_SLCT)
        {
            adpcm_no = AB027_STR;
        }
        else if (dsp_ms.now_mode == MS_STORY_MODE)
        {
            adpcm_no = AB027_STR;
        }
        else if (dsp_ms.now_mode == MS_SOUND_TEST)
        {
            title_bgm_no = -1;
        }
        else if (dsp_ms.now_mode == MS_OPTION)
        {
            title_bgm_no = -1;
        }

        if (adpcm_no != -1)
        {
            if (title_bgm_no != adpcm_no)
            {
                EAdpcmCmdPlay(0, 1, adpcm_no, 0, GetAdpcmVol(adpcm_no), 640, 0xfff, 0);

                title_bgm_no = adpcm_no;
            }
        }
    break;
    case MS_MODE_INIT:
        if (ms_load_id == -1)
        {
            if (cmn_tex_load == 0)
            {
                cmn_tex_load = 1;
            }

            dsp_ms.bak_alp = 0;
            dsp_ms.chr_alp = 0;
            dsp_ms.csr[1] = 0;

            if (mode == MS_OPTION)
            {
                StartOptionModeInit(1);

                dsp_ms.csr[2] = 0;
                dsp_ms.opt_mode = MS_MISSION_SELECT;
            }
            else if (mode == MS_STORY_MODE)
            {
                dsp_ms.sm_slct[2] = cribo.costume;
                dsp_ms.sm_slct[1] = ingame_wrk.difficult;

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                dsp_ms.sm_slct[0] = 0;

                SetCsrInfoStoryMode();
#endif
            }
            else if (mode == MS_BATTLE_MODE)
            {
                dsp_ms.bm_slct[0] = cribo.costume;

#if defined(BUILD_EU_VERSION)
                SetCsrInfoBattleMode();
#endif
            }
            dsp_ms.main_step = MS_MODE_FADE_INN;

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            SeSetMVol(opt_wrk.bgm_vol);
            SeSetSteMono(opt_wrk.sound_mode);
#endif
        }
    break;
    case MS_MODE_FADE_INN:
        if (dsp_ms.bak_alp + 8 < 0x80)
        {
            dsp_ms.bak_alp += 8;
            dsp_ms.chr_alp = dsp_ms.bak_alp;

            if (dsp_ms.flm_alp != 0x80)
            {
                dsp_ms.flm_alp = dsp_ms.chr_alp;
            }

            if (dsp_ms.win_alp != 0x80)
            {
                dsp_ms.win_alp = dsp_ms.bak_alp;
            }
        }
        else
        {
            dsp_ms.flm_alp = 0x80;
            dsp_ms.win_alp = 0x80;
            dsp_ms.bak_alp = 0x80;
            dsp_ms.chr_alp = 0x80;
            dsp_ms.flm_lng = 0;
            dsp_ms.main_step = MS_MODE_FILM_INN;
        }
    break;
    case MS_MODE_FILM_INN:
        if (dsp_ms.flm_lng + 20 < 100)
        {
            dsp_ms.flm_lng += 20;
        }
        else if (dsp_ms.now_mode == MS_OPTION)
        {
            if (IsEndAdpcmFadeOut())
            {
                EAdpcmCmdPlay(0, 1, AB018_STR, 0, GetAdpcmVol(AB018_STR), 640, 0xfff, 0);

                dsp_ms.flm_lng = 100;
                dsp_ms.main_step = MS_MODE_LOOP;
            }
        }
        else
        {
            dsp_ms.flm_lng = 100;
            dsp_ms.main_step = MS_MODE_LOOP;
        }
    break;
    case MS_MODE_LOOP:
        if (ModeSlctPad(mode))
        {
            dsp_ms.main_step = MS_MODE_FILM_OUT;
        }
    break;
    case MS_MODE_FILM_OUT:
        if (dsp_ms.flm_lng - 20 > 0)
        {
            dsp_ms.flm_lng -= 20;
        }
        else
        {
            dsp_ms.flm_lng = 0;
            dsp_ms.main_step = MS_MODE_FADE_OUT;
        }
    break;
    case MS_MODE_FADE_OUT:
        if (dsp_ms.chr_alp - 8 > 0)
        {
            dsp_ms.chr_alp -= 8;
            dsp_ms.win_alp = dsp_ms.chr_alp;
            dsp_ms.bak_alp = dsp_ms.chr_alp;

            if (
                dsp_ms.next_mode == MS_EXIT || dsp_ms.next_mode == MS_BREAK || dsp_ms.next_mode == MS_CAMERA ||
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                dsp_ms.next_mode == MS_PHOT || dsp_ms.next_mode == MS_SAVE ||
#endif
                dsp_ms.next_mode == MS_STORY_EXIT
            )
            {
                dsp_ms.flm_alp = dsp_ms.chr_alp;
            }
        }
        else
        {
            dsp_ms.chr_alp = 0;
            dsp_ms.win_alp = 0;
            dsp_ms.bak_alp = 0;

            if (dsp_ms.next_mode == MS_EXIT || dsp_ms.next_mode == MS_BREAK || dsp_ms.next_mode == MS_STORY_EXIT)
            {
                dsp_ms.flm_alp = 0;
            }

            dsp_ms.main_step = MS_MODE_END;
        }
    break;
    case MS_MODE_END:
        dsp_ms.main_step = MS_MODE_START;

        if (dsp_ms.now_mode == MS_SOUND_TEST)
        {
            SeSetMVol(opt_wrk.bgm_vol);
        }

        if (dsp_ms.next_mode == MS_EXIT)
        {
            outgame_wrk.mode = OUTGAME_MODE_TITLE;

            EAdpcmFadeOut(60);

            if (title_wrk.mode != TITLE_MODE_SEL_BGMREQ)
            {
                title_wrk.mode = TITLE_INIT_FROM_IN_BGMREQ;
            }

            title_bgm_no = -1;
        }
        else if (dsp_ms.next_mode == MS_BREAK)
        {
            outgame_wrk.mode = OUTGAME_MODE_TITLE;
        }
        else if (dsp_ms.next_mode == MS_STORY_EXIT)
        {
            ingame_wrk.game = 0;

            GameModeChange(0);

            title_bgm_no = -1;
        }
        else
        {
            int new_now_mode = MS_SOUND_TEST; // HACK: fixes codegen

            dsp_ms.now_mode = dsp_ms.next_mode;

            if (dsp_ms.now_mode == new_now_mode)
            {
                SoundTestForModeSlectInit();
            }
        }
    break;
    }

    if (
        dsp_ms.now_mode != MS_CAMERA &&
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        dsp_ms.now_mode != MS_PHOT && dsp_ms.now_mode != MS_SAVE &&
#endif
        cmn_tex_load != 0
    )
    {
        ModeSlctDspBak(dsp_ms.bak_alp, mode);

#if defined(BUILD_JP_VERSION)
        ModeSlctDspChr(dsp_ms.chr_alp, mode);
        ModeSlctDspFlm(dsp_ms.flm_lng, dsp_ms.flm_alp, mode);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        ModeSlctDspFlm(dsp_ms.flm_lng, dsp_ms.flm_alp, mode);
        ModeSlctDspChr(dsp_ms.chr_alp, mode);
#endif

        if (dsp_ms.now_mode != MS_OPTION)
        {
            ModeSlctDspWin(dsp_ms.win_alp);
        }

        ModeSlctDspMsg(dsp_ms.win_alp, mode);
    }
}

char ModeSlctPad(u_char mode)
{
    u_char rtrn;
    u_char sm_slct_num[3] = { 6, 2, 4 };
    u_char bm_slct_num[1] = { 4 };
    int level;

    rtrn = 0;

    switch (mode)
    {
    case MS_MODE_SLCT:
        if (*key_now[PAD_ACTION_EXIT] == 1)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            dsp_ms.next_mode = MS_EXIT;

            rtrn = 1;
        }
        else if (*key_now[PAD_ACTION_CONFIRM] == 1)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);

            dsp_ms.next_mode = dsp_ms.csr[0] + 1;

            if (dsp_ms.next_mode == MS_OPTION || dsp_ms.next_mode == MS_SOUND_TEST)
            {
                EAdpcmFadeOut(60);
            }
            rtrn = 1;
        }
        else
        {
            if (
                *key_now[PAD_DPAD_UP] == 1 ||
                (*key_now[PAD_DPAD_UP] > 25 && (*key_now[PAD_DPAD_UP] % 5) == 1) ||
                Ana2PadDirCnt(0) == 1 ||
                (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
            )
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);

                if (dsp_ms.csr[0])
                {
                    dsp_ms.csr[0]--;
                }
                else
                {
                    dsp_ms.csr[0] = 4;
                }
            }
            else if (
                *key_now[PAD_DPAD_DOWN] == 1 ||
                (*key_now[PAD_DPAD_DOWN] > 25 && (*key_now[PAD_DPAD_DOWN] % 5) == 1) ||
                Ana2PadDirCnt(2) == 1 ||
                (Ana2PadDirCnt(2) > 25 && Ana2PadDirCnt(2) % 5 == 1)
            )
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);

                if (dsp_ms.csr[0] <= 3)
                {
                    dsp_ms.csr[0]++;
                }
                else
                {
                    dsp_ms.csr[0] = 0;
                }
            }
        }
    break;
    case MS_STORY_MODE:
        if (*key_now[PAD_ACTION_EXIT] == 1)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            if (dsp_ms.side == 0)
            {
                dsp_ms.next_mode = dsp_ms.end_mode;

                rtrn = 1;
            }
            else
            {
                dsp_ms.side = 0;
                if (
                    dsp_ms.csr[1] == 0 &&
                    dsp_ms.sm_slct[0] == 0 &&
                    (ingame_wrk.msn_no != 0 || mc_msn_flg != 1)
                )
                {
                    dsp_ms.sm_slct[1] = ingame_wrk.difficult;
                }
            }

#if defined(BUILD_EU_VERSION)
            cribo.costume = dsp_ms.sm_slct[2];
#endif
        }
        else if (*key_now[PAD_ACTION_CONFIRM] == 1)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);

            switch (dsp_ms.csr[1])
            {
            case 0:
                dsp_ms.side = 1 - dsp_ms.side;

                if (
                    dsp_ms.side == 0 && dsp_ms.sm_slct[0] == 0 &&
                    (ingame_wrk.msn_no != 0 || mc_msn_flg != 1)
                )
                {
                    dsp_ms.sm_slct[1] = ingame_wrk.difficult;
                }
            break;
            case 1:
                if (
                    dsp_ms.sm_slct[0] != 0 ||
                    (ingame_wrk.msn_no == 0 && mc_msn_flg == 1)
                )
                dsp_ms.side = 1 - dsp_ms.side;
            break;
            case 2:
                dsp_ms.side = 1 - dsp_ms.side;
            break;
            case 3:
                EAdpcmFadeOut(60);

                dsp_ms.next_mode = MS_STORY_EXIT;

                if (dsp_ms.sm_slct[0] == 0)
                {
                    sys_wrk.load = 1;

                    title_wrk.csr = 0;

                    ingame_wrk.mode = INGAME_MODE_NOMAL;
                }
                else
                {
                    ingame_wrk.msn_no = dsp_ms.sm_slct[0] - 1;

                    sys_wrk.load = 0;

                    mc_msn_flg = 0;

                    title_wrk.csr = 0;

                    if (ingame_wrk.msn_no != 0)
                    {
                        cribo.clear_info &= ~0x2;
                    }
                }

                ingame_wrk.difficult = dsp_ms.sm_slct[1];

                cribo.costume = dsp_ms.sm_slct[2];

                if (ingame_wrk.msn_no == 0)
                {
                    cribo.clear_info |= 2;
                }

                rtrn = 1;
            break;
            case 4:
                dsp_ms.next_mode = MS_MODE_SLCT;

#if defined(BUILD_EU_VERSION)
                cribo.costume = dsp_ms.sm_slct[2];
#endif

                rtrn = 1;
            break;
            }
        }
        else
        {
            switch (dsp_ms.side)
            {
            case 0:
                if (
                    *key_now[PAD_DPAD_UP] == 1 ||
                    (*key_now[PAD_DPAD_UP] > 25 && (*key_now[PAD_DPAD_UP] % 5) == 1) ||
                    Ana2PadDirCnt(0) == 1 ||
                    (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);

#if defined(BUILD_JP_VERSION)
                    if (dsp_ms.csr[1] > 0)
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                    if (dsp_ms.csr[1] > sm_csr_info[0])
#endif
                    {
                        dsp_ms.csr[1]--;
                    }
                    else
                    {
                        dsp_ms.csr[1] = 4;
                    }
                }
                else if (
                    *key_now[PAD_DPAD_DOWN] == 1 ||
                    (*key_now[PAD_DPAD_DOWN] > 25 && (*key_now[PAD_DPAD_DOWN] % 5) == 1) ||
                    Ana2PadDirCnt(2) == 1 ||
                    (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);

                    if (dsp_ms.csr[1] <= 3)
                    {
                        dsp_ms.csr[1]++;
                    }
                    else
                    {
#if defined(BUILD_JP_VERSION)
                        dsp_ms.csr[1] = 0;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                        dsp_ms.csr[1] = sm_csr_info[0];
#endif
                    }
                }
            break;
            case 1:
                if (
                    *key_now[PAD_DPAD_LEFT] == 1 ||
                     (*key_now[PAD_DPAD_LEFT] > 25 && (*key_now[PAD_DPAD_LEFT] % 5) == 1) ||
                    Ana2PadDirCnt(3) == 1 ||
                    (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                    if (dsp_ms.csr[1] == 2)
                    {
                        if (dsp_ms.sm_slct[2] != 0)
                        {
                            dsp_ms.sm_slct[2]--;
                        }
                        else
                        {
                            dsp_ms.sm_slct[2] = sm_csr_info[1];
                        }
                    }
                    else
#endif
                    {
                        if (dsp_ms.sm_slct[dsp_ms.csr[1]] == 0)
                        {
                            dsp_ms.sm_slct[dsp_ms.csr[1]] = sm_slct_num[dsp_ms.csr[1]] - 1;
                        }
                        else
                        {
                            dsp_ms.sm_slct[dsp_ms.csr[1]]--;
                        }
                    }
                }
                else if (
                    *key_now[PAD_DPAD_RIGHT] == 1 ||
                    (*key_now[PAD_DPAD_RIGHT] > 25 && (*key_now[PAD_DPAD_RIGHT] % 5) == 1) ||
                    Ana2PadDirCnt(1) == 1 ||
                    (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                    if (dsp_ms.csr[1] == 2)
                    {
                        if (sm_csr_info[1] > dsp_ms.sm_slct[2])
                        {
                            dsp_ms.sm_slct[2]++;
                        }
                        else
                        {
                            dsp_ms.sm_slct[2] = 0;
                        }
                    }
                    else
#endif
                    {
                        if (sm_slct_num[dsp_ms.csr[1]] - 1 > dsp_ms.sm_slct[dsp_ms.csr[1]])
                        {
                            dsp_ms.sm_slct[dsp_ms.csr[1]]++;
                        }
                        else
                        {
                            dsp_ms.sm_slct[dsp_ms.csr[1]] = 0;
                        }
                    }
                }
            break;
            }
        }
    break;
    case MS_BATTLE_MODE:
        if (*key_now[PAD_ACTION_EXIT] == 1)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            if (dsp_ms.side == 0)
            {
                dsp_ms.next_mode = dsp_ms.end_mode;

                rtrn = 1;
            }
            else
            {
                dsp_ms.side = 0;
            }
        }
        else if (*key_now[PAD_ACTION_CONFIRM] == 1)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);
            switch (dsp_ms.csr[1])
            {
            case 0:
                dsp_ms.side = 1 - dsp_ms.side;
            break;
            case 1:
                dsp_ms.next_mode = MS_CAMERA;

                rtrn = 1;
            break;
#if defined(BUILD_JP_VERSION)
            case 2:
                dsp_ms.next_mode = MS_MISSION_SELECT;

                btl_wrk.stage_no = 0;

                cribo.costume = dsp_ms.bm_slct[0];

                rtrn = 1;
            break;
            case 3:
                dsp_ms.next_mode = MS_MODE_SLCT;

                rtrn = 1;
            break;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            case 2:
                dsp_ms.next_mode = MS_PHOT;

                rtrn = 1;
            break;
            case 3:
                dsp_ms.next_mode = MS_SAVE;

                rtrn = 1;
            break;
            case 4:
                dsp_ms.next_mode = MS_MISSION_SELECT;

                btl_wrk.stage_no = 0;

                cribo.costume = dsp_ms.bm_slct[0];

                rtrn = 1;
            break;
            case 5:
                dsp_ms.next_mode = MS_MODE_SLCT;

                rtrn = 1;
            break;
#endif
            }
        }
        else
        {
            switch (dsp_ms.side)
            {
            case 0:
                if (
                    *key_now[PAD_DPAD_UP] == 1 ||
                     (*key_now[PAD_DPAD_UP] > 25 && (*key_now[PAD_DPAD_UP] % 5) == 1) ||
                    Ana2PadDirCnt(0) == 1 ||
                    (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);

                    if (dsp_ms.csr[1] != 0)
                    {
                        dsp_ms.csr[1]--;
                    }
                    else
                    {
#if defined(BUILD_JP_VERSION)
                        dsp_ms.csr[1] = 3;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                        dsp_ms.csr[1] = 5;
#endif
                    }
                }
                else if (
                    *key_now[PAD_DPAD_DOWN] == 1 ||
                    (*key_now[PAD_DPAD_DOWN] > 25 && (*key_now[PAD_DPAD_DOWN] % 5) == 1) ||
                    Ana2PadDirCnt(2) == 1 ||
                    (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);

#if defined(BUILD_JP_VERSION)
                    if (dsp_ms.csr[1] < 3)
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                    if (dsp_ms.csr[1] < 5)
#endif
                    {
                        dsp_ms.csr[1]++;
                    }
                    else
                    {
                        dsp_ms.csr[1] = 0;
                    }
                }
            break;
            case 1:
                if (
                    *key_now[PAD_DPAD_LEFT] == 1 ||
                    (*key_now[PAD_DPAD_LEFT] > 25 && (*key_now[PAD_DPAD_LEFT] % 5) == 1) ||
                    Ana2PadDirCnt(3) == 1 ||
                    (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                    if (dsp_ms.csr[1] == 0)
                    {
                        if (dsp_ms.bm_slct[0] != 0)
                        {
                            dsp_ms.bm_slct[0]--;
                        }
                        else
                        {
                            dsp_ms.bm_slct[0] = sm_csr_info[1];
                        }
                    }
                    else
#endif
                    {
                        if (dsp_ms.bm_slct[dsp_ms.csr[1]] == 0)
                        {
                            dsp_ms.bm_slct[dsp_ms.csr[1]] = bm_slct_num[dsp_ms.csr[1]] - 1;
                        }
                        else
                        {
                            dsp_ms.bm_slct[dsp_ms.csr[1]]--;
                        }
                    }

                }
                else if (
                    *key_now[PAD_DPAD_RIGHT] == 1 ||
                    (*key_now[PAD_DPAD_RIGHT] > 25 && (*key_now[PAD_DPAD_RIGHT] % 5) == 1) ||
                    Ana2PadDirCnt(1) == 1 ||
                    (Ana2PadDirCnt(1) > 25 && Ana2PadDirCnt(1) % 5 == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                    if (dsp_ms.csr[1] == 0)
                    {
                        if (dsp_ms.bm_slct[0] < sm_csr_info[1])
                        {
                            dsp_ms.bm_slct[0]++;
                        }
                        else
                        {
                            dsp_ms.bm_slct[0] = 0;
                        }
                    }
                    else
#endif
                    {
                        if (dsp_ms.bm_slct[dsp_ms.csr[1]] < bm_slct_num[dsp_ms.csr[1]] - 1)
                        {
                            dsp_ms.bm_slct[dsp_ms.csr[1]]++;
                        }
                        else
                        {
                            dsp_ms.bm_slct[dsp_ms.csr[1]] = 0;
                        }
                    }
                }
            }
        }

        cribo.costume = dsp_ms.bm_slct[0];
    break;
    case MS_OPTION:
        if (MenuOptionPad(&dsp_ms.csr[2], &dsp_ms.opt_mode))
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            EAdpcmFadeOut(60);

            dsp_ms.next_mode = dsp_ms.end_mode;

            rtrn = 1;
        }
    break;
    case MS_SOUND_TEST:
        if (SoundTestForModeSlect())
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            dsp_ms.next_mode = dsp_ms.end_mode;

            rtrn = 1;
        }
    break;
    case MS_MISSION_SELECT:
        level = CheckClearStage();

        if (*key_now[PAD_ACTION_EXIT] == 1)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            dsp_ms.next_mode = MS_BATTLE_MODE;

            OutGameModeChange(OUTGAME_MODE_MODESEL);

            rtrn = 1;
        }
        else if (*key_now[PAD_ACTION_CONFIRM] == 1)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);

            ingame_wrk.game = 1;

            TitleModeEnd();

            sys_wrk.load = 0;

            SaveStoryWrk();
            GameModeChange(0);

            title_bgm_no = -1;

            if (*key_now[PAD_R1] != 0)
            {
                battle_scr_effct[btl_wrk.stage_no] = 6;
            }
            else if (*key_now[PAD_R2] != 0)
            {
                battle_scr_effct[btl_wrk.stage_no] = 4;
            }
            else if (*key_now[PAD_L1] != 0)
            {
                battle_scr_effct[btl_wrk.stage_no] = 2;
            }
            else if (*key_now[PAD_L2] != 0)
            {
                battle_scr_effct[btl_wrk.stage_no] = 3;
            }
            else
            {
                battle_scr_effct[btl_wrk.stage_no] = 1;
            }
            if (btl_wrk.stage_no == 19)
            {
                battle_scr_effct[btl_wrk.stage_no] = 0;
            }
        }
        else
        {
            if (
                *key_now[PAD_DPAD_UP] == 1 ||
                (*key_now[PAD_DPAD_UP] > 25 && (*key_now[PAD_DPAD_UP] % 5) == 1) ||
                Ana2PadDirCnt(0) == 1 ||
                (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
            )
            {
                if (btl_wrk.stage_no % 5 != 0)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);

                    btl_wrk.stage_no--;
                }
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                else
                {
                    btl_wrk.stage_no += 4;

                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                }
#endif
            }
            else
            {
                if (
                    *key_now[PAD_DPAD_DOWN] == 1 ||
                    (*key_now[PAD_DPAD_DOWN] > 25 && (*key_now[PAD_DPAD_DOWN] % 5) == 1) ||
                    Ana2PadDirCnt(2) == 1 ||
                    (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
                )
                {
                    if ((btl_wrk.stage_no + 1) % 5 != 0)
                    {
                        SeStartFix(0, 0, 0x1000, 0x1000, 0);

                        btl_wrk.stage_no++;
                    }
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                    else
                    {
                        btl_wrk.stage_no -= 4;

                        SeStartFix(0, 0, 0x1000, 0x1000, 0);
                    }
#endif
                }
                else
                {
                    if (
                        *key_now[PAD_DPAD_LEFT] == 1 ||
                        (*key_now[PAD_DPAD_LEFT] > 25 && (*key_now[PAD_DPAD_LEFT] % 5) == 1) ||
                        Ana2PadDirCnt(3) == 1 ||
                        (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
                    )
                    {
                        if (btl_wrk.stage_no >= 5)
                        {
                            SeStartFix(0, 0, 0x1000, 0x1000, 0);

                            btl_wrk.stage_no -= 5;
                        }
                    }
                    else if (
                        *key_now[PAD_DPAD_RIGHT] == 1 ||
                        (*key_now[PAD_DPAD_RIGHT] > 25 && (*key_now[PAD_DPAD_RIGHT] % 5) == 1) ||
                        Ana2PadDirCnt(1) == 1 ||
                        (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
                    )
                    {
                        if (btl_wrk.stage_no <= 14 && btl_wrk.stage_no / 5 < level)
                        {
                            SeStartFix(0, 0, 0x1000, 0x1000, 0);

                            btl_wrk.stage_no += 5;
                        }
                    }
                }
            }
        }
    break;
    case MS_CAMERA:
        if (isCameraEnd())
        {
            dsp_ms.next_mode = MS_BATTLE_MODE;

            OutGameModeChange(OUTGAME_MODE_MODESEL);

            rtrn = 1;
        }
        else
        {
            BgFusumaYW(0x606060, 0.0f, GetCamDispAlpha(), 0x7d000);

            SetSprFile(LOAD_ADDRESS_41);
            SetSprFile(LOAD_ADDRESS_26);
            SetSprFile(LOAD_ADDRESS_28);

            CameraCustomMain();
        }
    break;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    case MS_PHOT:
        if (isPhotoEnd())
        {
            dsp_ms.next_mode = MS_BATTLE_MODE;

            OutGameModeChange(OUTGAME_MODE_MODESEL);

            rtrn = 1;
        }
        else
        {
            BgFusumaYW(0x606060, 0.0f, GetPhtDispAlpha(), 0x7d000);

            SetSprFile(LOAD_ADDRESS_31);
            SetSprFile(LOAD_ADDRESS_26);
            SetSprFile(LOAD_ADDRESS_28);

            IngameMenuAlbum(1);
        }
    break;
    case MS_SAVE:
        if (SavePointMenuMain(3))
        {
            dsp_ms.next_mode = MS_BATTLE_MODE;

            OutGameModeChange(OUTGAME_MODE_MODESEL);

            rtrn = 1;
        }
    break;
#endif
    }

    return rtrn;
}

void ModeSlctDspBak(u_char alp, u_char mode)
{
    int i;
    DISP_SPRT ds;

    if (alp == 0)
    {
        return;
    }

    SetSprFile(LOAD_ADDRESS_10);
    SetSprFile(LOAD_ADDRESS_11);

    for (i = 0; i < 11; i++)
    {
        switch (mode)
        {
        case MS_MODE_START:
            CopySprDToSpr(&ds, &msl_bg_fsm[i]);
        break;
        case MS_MODE_INIT:
            CopySprDToSpr(&ds, &msl_bg_sty[i]);

            ds.r = 0x64;
            ds.g = 0x64;
            ds.b = 0x64;
        break;
        case MS_MODE_FADE_INN:
            CopySprDToSpr(&ds, &msl_bg_btl[i]);

            ds.r = 0x64;
            ds.g = 0x64;
            ds.b = 0x64;
        break;
        case MS_MODE_FILM_INN:
            CopySprDToSpr(&ds, &msl_bg_opt[i]);
        break;
        case MS_MODE_LOOP:
            CopySprDToSpr(&ds, &msl_bg_snd[i]);
        break;
        case MS_MODE_END:
            CopySprDToSpr(&ds, &msl_bg_btl[i]);

            if (alp > 0x50)
            {
                alp = 0x50;
            }
        break;
        }

        ds.alpha = alp;

        DispSprD(&ds);
    }
}

void ModeSlctDspChr(u_char alp, u_char mode)
{
    u_char dsp_rgb[11];
    u_char dsp_alp[11];
    float flsh;

    if (alp == 0)
    {
        return;
    }

    flsh = (dsp_ms.mode_tmr % 60) * (PI * 2) / 60.0f;

#if defined(BUILD_JP_VERSION)
    flsh = ((sin(flsh) + 1.0) * 0.5f + 1.0f) * 0.5f; // 1.0 is double!!
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    flsh = ((SgSinf(flsh) + 1.0f) * 0.5f + 1.0f) * 0.5f;
#endif

    switch (mode)
    {
    case MS_MODE_SLCT:
        DspModeSlect(alp);
    break;
    case MS_STORY_MODE:
        DspStoryMode(alp, flsh);
    break;
    case MS_BATTLE_MODE:
        DspBattleMode(alp, flsh);
    break;
    case MS_OPTION:
        SetSprFile(LOAD_ADDRESS_26);
        SetSprFile(LOAD_ADDRESS_16);

        DspOptCtrl(dsp_ms.csr[2], dsp_ms.opt_mode, alp, 0);
    break;
    case MS_SOUND_TEST:
        SoundTestForModeSlectDisp(alp, flsh);
    break;
    case MS_MISSION_SELECT:
        SetSprFile(LOAD_ADDRESS_15);

        DspMissionSelect(alp);
    break;
    }
}

void DspModeSlect(u_char alp)
{
    int i;
    u_char dsp_rgb[5];
    u_char dsp_alp[5];

    for (i = 0; i < 6; i++)
    {
        if (i == dsp_ms.csr[0])
        {
            dsp_alp[i] = alp;
            dsp_rgb[i] = 0x80;
        }
        else
        {
            dsp_alp[i] = alp / 2;
            dsp_rgb[i] = 0x40;
        }
    };

    PutChrOne(TM_MS_STORY_MODE, 0, 0, dsp_rgb[0], dsp_alp[0], 0);
    PutChrOne(TM_MS_BATTLE_MODE, 0, 0, dsp_rgb[1], dsp_alp[1], 0);
    PutChrOne(TM_MS_OPTION, 0, 0, dsp_rgb[2], dsp_alp[2], 0);
    PutChrOne(TM_MS_SOUND_TEST, 0, 0, dsp_rgb[3], dsp_alp[3], 0);
    PutChrOne(TM_MS_EXIT, 0, 0, dsp_rgb[4], dsp_alp[4], 0);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    SetSprFile(LOAD_ADDRESS_26);

    DispCaption(3, alp);
#endif
}

void DspStoryMode(u_char alp, float flsh)
{
    int i;
#if defined(BUILD_JP_VERSION)
    u_char dsp_rgb[11];
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    u_int dsp_rgb[11];
#endif
    u_char dsp_alp[11];

    for (i = 0; i < 5; i++)
    {
        if (i == dsp_ms.csr[1])
        {
            if (dsp_ms.side)
            {
                dsp_alp[i] = (alp * 7) / 10;
#if defined(BUILD_JP_VERSION)
                dsp_rgb[i] = 0x59;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                dsp_rgb[i] = 0x595959;
#endif
            }
            else
            {
                dsp_alp[i] = alp;
#if defined(BUILD_JP_VERSION)
                dsp_rgb[i] = 0x80;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                dsp_rgb[i] = 0x808080;
#endif
            }
        }
        else
        {
#if defined(BUILD_JP_VERSION)
            dsp_alp[i] = alp / 2;
            dsp_rgb[i] = 0x40;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            if (sm_csr_info[0] <= i)
            {
                dsp_alp[i] = alp / 2;
                dsp_rgb[i] = 0x404040;
            }
            else
            {
                dsp_alp[i] = dsp_ms.unsel_col[3];
                dsp_rgb[i] =
                    dsp_ms.unsel_col[0] |
                    dsp_ms.unsel_col[1] << 8 |
                    dsp_ms.unsel_col[2] << 16;
            }
#endif
        }
    }

    for (i = 0; i < 3; i++)
    {
        if (dsp_ms.side != 0 && i == dsp_ms.csr[1])
        {
#if defined(BUILD_JP_VERSION)
            dsp_alp[i+5] = alp;
            dsp_rgb[i+5] = 0x80;
            dsp_rgb[i+8] = 0x80;
            dsp_alp[i+8] = alp * flsh;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            dsp_alp[i+5] = alp;
            dsp_rgb[i+5] = 0x808080;
            dsp_alp[i+8] = alp * flsh;
            dsp_rgb[i+8] = 0x808080;
#endif
        }
        else
        {
#if defined(BUILD_JP_VERSION)
            dsp_alp[i+5] = alp / 2;
            dsp_rgb[i+5] = 0x40;
            dsp_rgb[i+8] = 0x80;
            dsp_alp[i+8] = alp / 2;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            if (i >= sm_csr_info[0])
            {
                dsp_alp[i+5] = alp / 2;
                dsp_rgb[i+5] = 0x404040;
                dsp_alp[i+8] = alp / 2;
                dsp_rgb[i+8] = 0x808080;
            }
            else
            {
                dsp_alp[i+5] = dsp_ms.unsel_col[3];
                dsp_rgb[i+5] =
                    dsp_ms.unsel_col[0] |
                    dsp_ms.unsel_col[1] << 8 |
                    dsp_ms.unsel_col[2] << 16;
                dsp_alp[i+8] = dsp_ms.unsel_col[3];
                dsp_rgb[i+8] =
                    dsp_ms.unsel_col[0] |
                    dsp_ms.unsel_col[1] << 8 |
                    dsp_ms.unsel_col[2] << 16;
            }
#endif
        }
    }

#if defined(BUILD_JP_VERSION)
    PutChrOne(TM_SM_CHAPTER, 0, 0, dsp_rgb[0], dsp_alp[0], 0);
    PutChrOne(TM_SM_DIFFICULTY, 0, 0, dsp_rgb[1], dsp_alp[1], 0);
    PutChrOne(TM_SM_COSTUME, 0, 0, dsp_rgb[2], dsp_alp[2], 0);
    PutChrOne(TM_SM_GAME_START, 0, 0, dsp_rgb[3], dsp_alp[3], 0);
    PutChrOne(TM_SM_EXIT, 0, 0, dsp_rgb[4], dsp_alp[4], 0);
    PutChrOne(TM_SM_PLAY_S, 328, 101, dsp_rgb[8], dsp_alp[8], 2);
    PutChrOne(TM_SM_PLAY_S, 570, 101, dsp_rgb[8], dsp_alp[8], 0);
    PutChrOne(TM_SM_KAKKO_L, 348, 100, dsp_rgb[8], dsp_alp[8], 0);
    PutChrOne(TM_SM_KAKKO_R, 549, 100, dsp_rgb[8], dsp_alp[8], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    PutChrOneRGB(TM_SM_CHAPTER, 0, 0, dsp_rgb[0], dsp_alp[0], 0);
    PutChrOneRGB(TM_SM_DIFFICULTY, 0, 0, dsp_rgb[1], dsp_alp[1], 0);
    PutChrOneRGB(TM_SM_COSTUME, 0, 0, dsp_rgb[2], dsp_alp[2], 0);
    PutChrOneRGB(TM_SM_GAME_START, 0, 0, dsp_rgb[3], dsp_alp[3], 0);
    PutChrOneRGB(TM_SM_EXIT, 0, 0, dsp_rgb[4], dsp_alp[4], 0);
    PutChrOneRGB(TM_SM_PLAY_S, 328, 101, dsp_rgb[8], dsp_alp[8], 2);
    PutChrOneRGB(TM_SM_PLAY_S, 570, 101, dsp_rgb[8], dsp_alp[8], 0);
    PutChrOneRGB(TM_SM_KAKKO_L, 348, 100, dsp_rgb[8], dsp_alp[8], 0);
    PutChrOneRGB(TM_SM_KAKKO_R, 549, 100, dsp_rgb[8], dsp_alp[8], 0);
#endif

    switch (dsp_ms.sm_slct[0])
    {
    case 0:
#if defined(BUILD_JP_VERSION)
        PutChrOne(TM_SM_CONTINUE, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutChrOneRGB(TM_SM_CONT, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
#endif
    break;
    case 1:
#if defined(BUILD_JP_VERSION)
        PutChrOne(TM_SM_JO, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
        PutChrOne(TM_SM_SYO, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutChrOneRGB(TM_SM_SHW0, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
#endif
    break;
    case 2:
#if defined(BUILD_JP_VERSION)
        PutChrOne(TM_SM_DAI, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
        PutChrOne(TM_SM_ONE, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
        PutChrOne(TM_SM_YA, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutChrOneRGB(TM_SM_SHW1, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
#endif
    break;
    case 3:
#if defined(BUILD_JP_VERSION)
        PutChrOne(TM_SM_DAI, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
        PutChrOne(TM_SM_TWO, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
        PutChrOne(TM_SM_YA, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutChrOneRGB(TM_SM_SHW2, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
#endif
    break;
    case 4:
#if defined(BUILD_JP_VERSION)
        PutChrOne(TM_SM_DAI, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
        PutChrOne(TM_SM_THREE, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
        PutChrOne(TM_SM_YA, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutChrOneRGB(TM_SM_SHW3, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
#endif
    break;
    case 5:
#if defined(BUILD_JP_VERSION)
        PutChrOne(TM_SM_SAI, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
        PutChrOne(TM_SM_SYU, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
        PutChrOne(TM_SM_YA, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutChrOneRGB(TM_SM_SHW4, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
#endif
    break;
    }

#if defined(BUILD_JP_VERSION)
    PutChrOne(TM_SM_PLAY_S, 328, 145, dsp_rgb[9], dsp_alp[9], 2);
    PutChrOne(TM_SM_PLAY_S, 570, 145, dsp_rgb[9], dsp_alp[9], 0);
    PutChrOne(TM_SM_KAKKO_L, 348, 144, dsp_rgb[9], dsp_alp[9], 0);
    PutChrOne(TM_SM_KAKKO_R, 549, 144, dsp_rgb[9], dsp_alp[9], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    PutChrOneRGB(TM_SM_PLAY_S, 328, 145, dsp_rgb[9], dsp_alp[9], 2);
    PutChrOneRGB(TM_SM_PLAY_S, 570, 145, dsp_rgb[9], dsp_alp[9], 0);
    PutChrOneRGB(TM_SM_KAKKO_L, 348, 144, dsp_rgb[9], dsp_alp[9], 0);
    PutChrOneRGB(TM_SM_KAKKO_R, 549, 144, dsp_rgb[9], dsp_alp[9], 0);
#endif

    switch (dsp_ms.sm_slct[1]) // ingame_wrk.difficult
    {
    case 0:
#if defined(BUILD_JP_VERSION)
        PutChrOne(TM_SM_NORMAL, 382, 141, dsp_rgb[6], dsp_alp[6], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutChrOneRGB(TM_SM_NORMAL, 382, 141, dsp_rgb[6], dsp_alp[6], 0);
#endif
    break;
    case 1:
#if defined(BUILD_JP_VERSION)
        PutChrOne(TM_SM_NIGHTMARE, -5, 0, dsp_rgb[6], dsp_alp[6], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutChrOneRGB(TM_SM_NIGHTMARE, -5, 0, dsp_rgb[6], dsp_alp[6], 0);
#endif
    break;
    }

#if defined(BUILD_JP_VERSION)
    PutChrOne(TM_SM_PLAY_S, 328, 192, dsp_rgb[10], dsp_alp[10], 2);
    PutChrOne(TM_SM_PLAY_S, 570, 192, dsp_rgb[10], dsp_alp[10], 0);
    PutChrOne(TM_SM_KAKKO_L, 348, 191, dsp_rgb[10], dsp_alp[10], 0);
    PutChrOne(TM_SM_KAKKO_R, 549, 191, dsp_rgb[10], dsp_alp[10], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    PutChrOneRGB(TM_SM_PLAY_S, 328, 192, dsp_rgb[10], dsp_alp[10], 2);
    PutChrOneRGB(TM_SM_PLAY_S, 570, 192, dsp_rgb[10], dsp_alp[10], 0);
    PutChrOneRGB(TM_SM_KAKKO_L, 348, 191, dsp_rgb[10], dsp_alp[10], 0);
    PutChrOneRGB(TM_SM_KAKKO_R, 549, 191, dsp_rgb[10], dsp_alp[10], 0);
#endif

    switch (dsp_ms.sm_slct[2]) // cribo.costume
    {
    case 0:
#if defined(BUILD_JP_VERSION)
        PutChrOne(TM_SM_NORMAL, 382, 187, dsp_rgb[7], dsp_alp[7], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutChrOneRGB(TM_SM_NORMAL, 0x17e, 0xbb, dsp_rgb[7], dsp_alp[7], 0x0);
#endif
    break;
    case 1:
#if defined(BUILD_JP_VERSION)
        PutChrOne(TM_SM_SPECIAL, -11, 0, dsp_rgb[7], dsp_alp[7], 0);
        PutChrOne(TM_SM_1, 527, 187, dsp_rgb[7], dsp_alp[7], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutChrOneRGB(TM_SM_SPECIAL, -11, 0, dsp_rgb[7], dsp_alp[7], 0);
        PutChrOneRGB(TM_SM_1, 527, 187, dsp_rgb[7], dsp_alp[7], 0);
#endif
    break;
    case 2:
#if defined(BUILD_JP_VERSION)
        PutChrOne(TM_SM_SPECIAL, -11, 0, dsp_rgb[7], dsp_alp[7], 0);
        PutChrOne(TM_SM_2, 527, 187, dsp_rgb[7], dsp_alp[7], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutChrOneRGB(TM_SM_SPECIAL, -11, 0, dsp_rgb[7], dsp_alp[7], 0);
        PutChrOneRGB(TM_SM_2, 527, 187, dsp_rgb[7], dsp_alp[7], 0);
#endif
    break;
    case 3:
#if defined(BUILD_JP_VERSION)
        PutChrOne(TM_SM_SPECIAL, -11, 0, dsp_rgb[7], dsp_alp[7], 0);
        PutChrOne(TM_SM_3, 527, 187, dsp_rgb[7], dsp_alp[7], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutChrOneRGB(TM_SM_SPECIAL, -11, 0, dsp_rgb[7], dsp_alp[7], 0);
        PutChrOneRGB(TM_SM_3, 527, 187, dsp_rgb[7], dsp_alp[7], 0);
#endif
    break;
    }

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    SetSprFile(LOAD_ADDRESS_26);

    DispCaption(3, alp);
#endif
}

void DspBattleMode(u_char alp, float flsh)
{
    int i;
    u_char dsp_rgb[9];
    u_char dsp_alp[9];

#if defined(BUILD_JP_VERSION)
    for (i = 0; i < 4; i++)
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    for (i = 0; i < 6; i++)
#endif
    {
        if (i == dsp_ms.csr[1])
        {
            if (dsp_ms.side)
            {
                dsp_alp[i] = (alp * 7) / 10;
                dsp_rgb[i] = 0x59;
            }
            else
            {
                dsp_alp[i] = alp;
                dsp_rgb[i] = 0x80;
            }
        }
        else
        {
            dsp_alp[i] = alp / 2;
            dsp_rgb[i] = 0x40;
        }
    }

    for (i = 0; i < 1; i++)
    {
        if (dsp_ms.side && i == dsp_ms.csr[1])
        {
#if defined(BUILD_JP_VERSION)
            dsp_alp[i+4] = alp;
            dsp_rgb[i+4] = 0x80;
            dsp_rgb[i+5] = 0x80;
            dsp_alp[i+5] = alp * flsh;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            dsp_alp[i+6] = alp;
            dsp_rgb[i+6] = 0x80;
            dsp_rgb[i+7] = 0x80;
            dsp_alp[i+7] = alp * flsh;
#endif
        }
        else
        {
#if defined(BUILD_JP_VERSION)
            dsp_alp[i+4] = alp / 2;
            dsp_rgb[i+4] = 0x40;
            dsp_rgb[i+5] = 0x80;
            dsp_alp[i+5] = alp / 2;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            dsp_alp[i+6] = alp / 2;
            dsp_rgb[i+6] = 0x40;
            dsp_rgb[i+7] = 0x80;
            dsp_alp[i+7] = alp / 2;
#endif
        }
    }

#if defined(BUILD_JP_VERSION)
    PutChrOne(TM_BM_COSTUME, 0, -45, dsp_rgb[0], dsp_alp[0], 0);
    PutChrOne(TM_BM_CAMERA, 0, -32, dsp_rgb[1], dsp_alp[1], 0);
    PutChrOne(TM_BM_SETTING, 0, -32, dsp_rgb[1], dsp_alp[1], 0);
    PutChrOne(TM_BM_MISSION_SELECT, 0, -15, dsp_rgb[2], dsp_alp[2], 0);
    PutChrOne(TM_BM_EXIT, 0, 0, dsp_rgb[3], dsp_alp[3], 0);
    PutChrOne(TM_BM_PLAY_S, 314, 101, dsp_rgb[5], dsp_alp[5], 2);
    PutChrOne(TM_BM_PLAY_S, 584, 101, dsp_rgb[5], dsp_alp[5], 0);
    PutChrOne(TM_BM_KAKKO_L, 336, 100, dsp_rgb[5], dsp_alp[5], 0);
    PutChrOne(TM_BM_KAKKO_R, 560, 100, dsp_rgb[5], dsp_alp[5], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    PutChrOne(TM_BM_COSTUME, 0, -25, dsp_rgb[0], dsp_alp[0], 0);
#if defined(BUILD_US_VERSION)
    PutChrOne(TM_BM_CAMERA, 0, -20, dsp_rgb[1], dsp_alp[1], 0);
#endif
    PutChrOne(TM_BM_SETTING, 0, -20, dsp_rgb[1], dsp_alp[1], 0);
    PutChrOne(TM_BM_PHOT, 0, -15, dsp_rgb[2], dsp_alp[2], 0);
    PutChrOne(TM_BM_SAVE, 0, -10, dsp_rgb[3], dsp_alp[3], 0);
    PutChrOne(TM_BM_MISSION_SELECT, 0, -5, dsp_rgb[4], dsp_alp[4], 0);
    PutChrOne(TM_BM_EXIT, 0, 0, dsp_rgb[5], dsp_alp[5], 0);
    PutChrOne(TM_BM_PLAY_S, 314, 99, dsp_rgb[7], dsp_alp[7], 2);
    PutChrOne(TM_BM_PLAY_S, 584, 99, dsp_rgb[7], dsp_alp[7], 0);
    PutChrOne(TM_BM_KAKKO_L, 336, 98, dsp_rgb[7], dsp_alp[7], 0);
    PutChrOne(TM_BM_KAKKO_R, 560, 98, dsp_rgb[7], dsp_alp[7], 0);
#endif

    switch (dsp_ms.bm_slct[0])
    {
    case 0:
#if defined(BUILD_JP_VERSION)
        PutChrOne(TM_BM_NORMAL, 379, 97, dsp_rgb[4], dsp_alp[4], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutChrOne(TM_BM_NORMAL, 379, 95, dsp_rgb[6], dsp_alp[6], 0);
#endif
    break;
    case 1:
#if defined(BUILD_JP_VERSION)
        PutChrOne(TM_BM_SPECIAL, -14, -44, dsp_rgb[4], dsp_alp[4], 0);
        PutChrOne(TM_BM_1, 524, 97, dsp_rgb[4], dsp_alp[4], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutChrOne(TM_BM_SPECIAL, -14, -46, dsp_rgb[6], dsp_alp[6], 0);
        PutChrOne(TM_BM_1, 524, 95, dsp_rgb[6], dsp_alp[6], 0);
#endif
    break;
    case 2:
#if defined(BUILD_JP_VERSION)
        PutChrOne(TM_BM_SPECIAL, -14, -44, dsp_rgb[4], dsp_alp[4], 0);
        PutChrOne(TM_BM_2, 524, 97, dsp_rgb[4], dsp_alp[4], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutChrOne(TM_BM_SPECIAL, -14, -46, dsp_rgb[6], dsp_alp[6], 0);
        PutChrOne(TM_BM_2, 524, 95, dsp_rgb[6], dsp_alp[6], 0);
#endif
    break;
    case 3:
#if defined(BUILD_JP_VERSION)
        PutChrOne(TM_BM_SPECIAL, -14, -44, dsp_rgb[4], dsp_alp[4], 0);
        PutChrOne(TM_BM_3, 524, 97, dsp_rgb[4], dsp_alp[4], 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutChrOne(TM_BM_SPECIAL, -14, -46, dsp_rgb[6], dsp_alp[6], 0);
        PutChrOne(TM_BM_3, 524, 95, dsp_rgb[6], dsp_alp[6], 0);
#endif
    break;
    }

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    SetSprFile(LOAD_ADDRESS_26);

    DispCaption(3, alp);
#endif
}

void DspMissionSelect(u_char alp)
{
    int i;
    int ene_no;
    int msn_no;
    int rank;
    int no;
    int page_base_no;
    static u_char csr_alpha = 0;
    static u_char csr_brink = 1;
    int level;

    page_base_no = btl_wrk.stage_no;
    no = (page_base_no / 5) * 5;

    level = CheckClearStage();

    PutChrOne(TM_NS_MN_BT_BS, 0, 0, 0x80, alp, 0);
    PutChrOne(TM_NS_RANK, 0, 0, 0x80, alp, 0);

    for (i = 0; i <= 4; i++)
    {
        PutChrOne(TM_NS_CHK_BOX, 0, i * 40, 0x80, alp, 0);
    }

    for (i = 0; i <= 4; i++)
    {
        if (stage_wrk[no + i].rank)
        {
            PutChrOne(TM_NS_CHK_RED, 0, i * 40, 0x80, alp, 0);
        }
    }

    if (csr_brink)
    {
        csr_alpha++;
        if (csr_alpha > 100)
        {
            csr_brink = 0;
        }
    }
    else
    {
        csr_alpha--;
        if (csr_alpha < 0x14)
        {
            csr_brink = 1;
        }
    }

    SetSquareS(0x25000, -240.0f, (page_base_no % 5) * 40 - 94, 196.0f, (page_base_no % 5) * 40 - 0x40, 0x8c, 0x80, 60, csr_alpha);

    if (page_base_no > 4)
    {
        PutChrOne(TM_NS_NEXT_TRI, 23, 203, 0x80, alp, 0);
    }

    if (page_base_no < 15 && no / 5 < level)
    {
        PutChrOne(TM_NS_NEXT_TRI, 587, 203, 0x80, alp, 2);
    }

    for (i = 0; i < 5; i++)
    {
        PutStringYW(46, 10, 85, i * 40 + 133, 0x808080, alp, 0x23000, 0);
#if defined(BUILD_JP_VERSION)
        PutNumberYW(3, no + i + 1, 192, i * 40 + 133, 1.0f, 1.0f, 0x808080, alp, 0x23000, 2, 1);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutNumberYW(3, no + i + 1, 204, i * 40 + 133, 1.0f, 1.0f, 0x808080, alp, 0x23000, 2, 1);
#endif
        PutNumberYW(3, stage_wrk[no + i].best_time / 3600, 260, i * 40 + 133, 1.0f, 1.0f, 0x808080, alp, 0x23000, 1, 1);
        PutStringYW(46, 11, 273, i * 40 + 133, 0x808080, alp, 0x23000, 0);
        PutNumberYW(3, (stage_wrk[no + i].best_time / 60) % 60, 287, i * 40 + 133, 1.0f, 1.0f, 0x808080, alp, 0x23000, 2, 1);
        PutStringYW(46, 11, 318, i * 40 + 133, 0x808080, alp, 0x23000, 0);
        PutNumberYW(3, ((stage_wrk[no + i].best_time * 100) / 60) % 100, 330, i * 40 + 133, 1.0f, 1.0f, 0x808080, alp, 0x23000, 2, 1);
        PutNumberYW(3, stage_wrk[no + i].best_shot, 403, i * 40 + 133, 1.0f, 1.0f, 0x808080, alp, 0x23000, 4, 0);
        PutStringYW(46, 12, 468, i * 40 + 133, 0x808080, alp, 0x23000, 0);

        rank = stage_wrk[no + i].rank;

        if ((rank ^ 1) == 0)
        {
            rank = 2;
        }

        switch (rank)
        {
        case 0:
        break;
        case 1:
            PutChrOne(TM_NS_E, 0, i * 40, 0x80, alp, 0);
        break;
        case 2:
            PutChrOne(TM_NS_D, 0, i * 40, 0x80, alp, 0);
        break;
        case 3:
            PutChrOne(TM_NS_C, 0, i * 40, 0x80, alp, 0);
        break;
        case 4:
            PutChrOne(TM_NS_B, 0, i * 40, 0x80, alp, 0);
        break;
        case 5:
            PutChrOne(TM_NS_A, 0, i * 40, 0x80, alp, 0);
        break;
        case 6:
            PutChrOne(TM_NS_S, 0, i * 40, 0x80, alp, 0);
        break;
        }
    }

    for (i = 0; i <= 2; i++)
    {
         ene_no = stage_dat[btl_wrk.stage_no].ene_no[i];
         msn_no = stage_dat[btl_wrk.stage_no].map_no;

        if (ene_no == 0xff)
        {
            break;
        }

        if (ene_no == 7 && msn_no == 3)
        {
            PutRayName(2, 1, 3, 80, i * 24 + 357, 0x808080, alp);
        }
        else
        {
            PutRayName(1, ene_no, msn_no, 80, i * 24 + 357, 0x808080, alp);
        }
    }

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    SetSprFile(LOAD_ADDRESS_26);

    DispCaption(4, alp);
#endif
}

CLEAR_BONUS cribo = {0, 0};

void PutChrOne(u_short chr, short int px, short int py, u_char rgb, u_char alp, u_char flp)
{
    DISP_SPRT ds;

    CopySprDToSpr(&ds, &msel_sprt[chr]);

    ds.x += px;
    ds.y += py;

    ds.alpha = alp;

    ds.r = rgb;
    ds.g = rgb;
    ds.b = rgb;

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    switch(flp)
    {
    case 1:
        ds.att |= 1;
    break;
    case 2:
        ds.att |= 2;
    break;
    case 3:
        ds.att |= 3;
    break;
    }

    DispSprD(&ds);
}

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
void PutChrOneRGB(u_short chr, short int px, short int py, u_int rgb, u_char alp, u_char flp)
{
    DISP_SPRT ds;

    u_char r = rgb;
    u_char g = rgb >> 8;
    u_char b = rgb >> 16;

    CopySprDToSpr(&ds, &msel_sprt[chr]);

    ds.x += px;
    ds.y += py;
    ds.alpha = alp;
    ds.r = r;
    ds.g = g;
    ds.b = b;
    ds.tex1 = 0x161;

    switch(flp)
    {
    case 1:
        ds.att |= 1;
    break;
    case 2:
        ds.att |= 2;
    break;
    case 3:
        ds.att |= 3;
    break;
    }

    DispSprD(&ds);
}
#endif

void ModeSlctDspFlm(u_char per, u_char alp, u_char mode)
{
    short int mx;
    DISP_SPRT ds;

    if (alp == 0)
    {
        return;
    }

    mx = ((100 - per) * 505) / 100.0f;

    switch (mode)
    {
    case MS_MODE_START:
        FilmCutter(TM_FLM_OBI, -mx, 0, 89, 0x80, alp);
        FilmCutter(TM_FLM_M_SLCT, -mx, 0, 89, 0x80, alp);

        PutChrOne(TM_FLM_BODY, 0, 0, 0x80, alp, 0);
    break;
    case MS_MODE_INIT:
        FilmCutter(TM_FLM_OBI, -mx, 0, 89, 0x80, alp);
        FilmCutter(TM_FLM_S_MODE, -mx, 0, 89, 0x80, alp);

        PutChrOne(TM_FLM_BODY, 0, 0, 0x80, alp, 0);
    break;
    case MS_MODE_FADE_INN:
        FilmCutter(TM_FLM_OBI, -mx, 0, 89, 0x80, alp);
        FilmCutter(TM_FLM_B_MODE, -mx, 0, 89, 0x80, alp);

        PutChrOne(TM_FLM_BODY, 0, 0, 0x80, alp, 0);
    break;
    case MS_MODE_FILM_INN:
        FilmCutter(TM_FLM_OBI, -mx, 0, 89, 0x80, alp);
        FilmCutter(TM_FLM_OPTION, -mx, 0, 89, 0x80, alp);

        PutChrOne(TM_FLM_BODY, 0, 0, 0x80, alp, 0);
    break;
    case MS_MODE_LOOP:
        FilmCutter(TM_FLM_OBI, -mx, 0, 89, 0x80, alp);
        FilmCutter(TM_FLM_S_TEST, -mx, 0, 89, 0x80, alp);

        PutChrOne(TM_FLM_BODY, 0, 0, 0x80, alp, 0);
    break;
    case MS_MODE_END:
        FilmCutter(TM_FLM_OBI, -mx, 0, 89, 0x80, alp);
        FilmCutter(TM_FLM_MSN_SL, -mx, 0, 89, 0x80, alp);

        PutChrOne(TM_FLM_BODY, 0, 0, 0x80, alp, 0);
    break;
    }
}

void FilmCutter(u_short chr, short int px, short int py, short int ex, u_char rgb, u_char alp)
{
    short int cut;
    DISP_SPRT ds;

    CopySprDToSpr(&ds, &msel_sprt[chr]);

    cut = 0;

    if (ds.x + px < ex)
    {
        cut = ex - (ds.x + px);
        px = ex - ds.x;

        if (ds.w < cut)
        {
            cut = ds.w;
        }
    }

    ds.x += px;
    ds.y += py;

    ds.u += cut;
    ds.w -= cut;

    ds.alpha = alp;

    ds.r = rgb;
    ds.g = rgb;
    ds.b = rgb;

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);
}

void ModeSlctDspWin(u_char alp)
{
    DISP_SPRT ds;

    if (alp == 0)
    {
        return;
    }

    DrawMessageBox(0x19000, 24.0f, 340.0f, 593.0f, 84.0f, alp);
}

void MsLoadCtrl(u_char mode)
{
    switch(mode)
    {
    case MS_MODE_SLCT:
        ms_load_id = LoadReq(EFF001_PK2, LOAD_ADDRESS_41);
        ms_load_id = VER_LOAD_REQ_LANG(M_SLCT_FSM_PK2, LOAD_ADDRESS_11);
    break;
    case MS_STORY_MODE:
        ingame_wrk.game = 0;

        ms_load_id = VER_LOAD_REQ_LANG(M_SLCT_STY_PK2, LOAD_ADDRESS_11);

        if (ingame_wrk.difficult != 0)
        {
            dsp_ms.sm_slct[1] = 1;
        }
    break;
    case MS_BATTLE_MODE:
        ingame_wrk.game = 1;

        ms_load_id = VER_LOAD_REQ_LANG(M_SLCT_BTL_PK2, LOAD_ADDRESS_11);
        ms_load_id = VER_LOAD_REQ_LANG(M_SLCT_BTL_CHR_PK2, LOAD_ADDRESS_13);
    break;
    case MS_OPTION:
        ms_load_id = VER_LOAD_REQ_LANG(M_SLCT_OPT_PK2, LOAD_ADDRESS_11);
        ms_load_id = VER_LOAD_REQ_LANG(PL_OPTI_PK2, LOAD_ADDRESS_16);
        ms_load_id = VER_LOAD_REQ_LANG(PL_STTS_PK2, LOAD_ADDRESS_26);
    break;
    case MS_SOUND_TEST:
        ms_load_id = VER_LOAD_REQ_LANG(M_SLCT_SND_PK2, LOAD_ADDRESS_11);
    break;
    case MS_MISSION_SELECT:
        if (cmn_tex_load == 0)
        {
            ms_load_id = VER_LOAD_REQ_LANG(M_SLCT_BTL_PK2, LOAD_ADDRESS_11);
        }

        ms_load_id = VER_LOAD_REQ_LANG(M_SLCT_BTL_MSN_PK2, LOAD_ADDRESS_15);
    break;
    case MS_CAMERA:
        CameraCustomInit();
        OutGameInitCamera();

        ms_load_id = LoadReq(PL_BGBG_PK2, LOAD_ADDRESS_27);
        ms_load_id = VER_LOAD_REQ_LANG(PL_MTOP_PK2, LOAD_ADDRESS_28);
        ms_load_id = VER_LOAD_REQ_LANG(PL_STTS_PK2, LOAD_ADDRESS_26);
        ms_load_id = VER_LOAD_REQ_LANG(PL_CAME_PK2, LOAD_ADDRESS_41);
    break;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    case MS_PHOT:
        ms_load_id = LoadReq(PL_BGBG_PK2, LOAD_ADDRESS_27);
        ms_load_id = VER_LOAD_REQ_LANG(PL_MTOP_PK2, LOAD_ADDRESS_28);
        ms_load_id = VER_LOAD_REQ_LANG(PL_STTS_PK2, LOAD_ADDRESS_26);
        ms_load_id = VER_LOAD_REQ_LANG(PL_PHOT_PK2, LOAD_ADDRESS_31);

        StartAlbumModeInit();
        OutGameInitPhoto();
    break;
    case MS_SAVE:
        motInitMsn();

        ms_load_id = LoadReq(PL_BGBG_PK2, LOAD_ADDRESS_27);

        BtlModSaveInit();
    break;
#endif
    }
}

void TitleModeEnd()
{
    dsp_ms.main_step = 0;
    dsp_ms.flm_alp = 0;
    dsp_ms.win_alp = 0;
    dsp_ms.bak_alp = 0;
    dsp_ms.chr_alp = 0;
    dsp_ms.flm_lng = 0;

    if (dsp_ms.next_mode == MS_EXIT)
    {
        title_wrk.mode = TITLE_INIT_FROM_IN_BGMREQ;
    }
    else if (dsp_ms.next_mode == MS_BREAK)
    {
        outgame_wrk.mode = OUTGAME_MODE_TITLE;
    }
    else
    {
        dsp_ms.now_mode = dsp_ms.next_mode;
    }

    cmn_tex_load = 0;
}

void ChangeModeSel(int mode)
{
    dsp_ms.next_mode = mode;
    dsp_ms.now_mode = dsp_ms.next_mode;

    OutGameModeChange(OUTGAME_MODE_MODESEL);
}

void BattleModeExit()
{
    ChangeModeSel(MS_BATTLE_MODE);
}

void BattleModeNext()
{
    ChangeModeSel(MS_MISSION_SELECT);
}

int CheckClearStage()
{
    int i;
    int j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 5; j++)
        {
            if (stage_wrk[i*5+j].rank == 0)
            {
                break;
            }
        }

        if (j != 5)
        {
            break;
        }
    }

    return i;
}

void ModeSlctDspMsg(u_char alp, u_char mode)
{
    switch (mode)
    {
    case MS_MODE_START:
        PutStringYW(IGMSG_MODE_SLCT, dsp_ms.csr[0] + 0, 80, 357, 0x808080, alp, 0x1000, 0);
    break;
    case MS_MODE_INIT:
        PutStringYW(IGMSG_MODE_SLCT, dsp_ms.csr[1] + 5, 80, 357, 0x808080, alp, 0x1000, 0);
    break;
    case MS_MODE_FADE_INN:
        PutStringYW(IGMSG_MODE_SLCT, dsp_ms.csr[1] + 10, 80, 357, 0x808080, alp, 0x1000, 0);
    break;
    }
}

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
void SetCsrInfoStoryMode()
{
    sm_csr_info[0] = 2;
    sm_csr_info[1] = 1;

    dsp_ms.csr[1] = 2;

    if (cribo.clear_info & 0x1)
    {
        dsp_ms.csr[1] = 0;

        sm_csr_info[0] = 0;

        if (ingame_wrk.clear_count >= 3)
        {
            sm_csr_info[1] = 3;
        }
        else
        {
            sm_csr_info[1] = 2;
        }
    }
    else
    {
        if (CheckClearAll() != 0)
        {
            dsp_ms.csr[1] = 1;

            sm_csr_info[0] = 1;
        }
    }
}
#endif

#if defined(BUILD_EU_VERSION)
void SetCsrInfoBattleMode()
{
    sm_csr_info[1] = 1;

    if (cribo.clear_info & 0x1)
    {
        if (ingame_wrk.clear_count > 2)
        {
            sm_csr_info[1] = 3;
        }
        else
        {
            sm_csr_info[1] = 2;
        }
    }
}
#endif
