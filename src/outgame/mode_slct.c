#include "common.h"
#include "typedefs.h"
#include "structs.h"
#include "enums.h"
#include "mode_slct.h"

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

#include "data/msl_bg_btl.h" // SPRT_DAT msl_bg_btl[11] = {...};
#include "data/msl_bg_fsm.h" // SPRT_DAT msl_bg_fsm[11] = {...};
#include "data/msl_bg_opt.h" // SPRT_DAT msl_bg_opt[11] = {...};
#include "data/msl_bg_snd.h" // SPRT_DAT msl_bg_snd[11] = {...};
#include "data/msl_bg_sty.h" // SPRT_DAT msl_bg_sty[11] = {...};
#include "data/msel_sprt.h"  // SPRT_DAT msel_sprt[148] = {...};

static u_char sm_csr_info[2] = {0, 0};
int cmn_tex_load = 0;
int title_bgm_no = -1;
static DSP_M_SLCT_WRK dsp_ms;
static int ms_load_id;

void ModeSlctInit(u_char top,u_char end)
{
#ifdef BUILD_EU_VERSION
    ms_load_id = LoadReqLanguage(M_SLCT_CMN_E_PK2, 0xc80000);
#else
    ms_load_id = LoadReq(M_SLCT_CMN_PK2, 0xc80000);
#endif
    memset(&dsp_ms, 0, sizeof(DSP_M_SLCT_WRK));
    dsp_ms.now_mode = top;
    dsp_ms.end_mode = end;
    cmn_tex_load = 1;
    dsp_ms.unsel_col[0] = 20;
    dsp_ms.unsel_col[1] = 15;
    dsp_ms.unsel_col[2] = 10;
    dsp_ms.unsel_col[3] = 128;
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
    case 0:
        if (cmn_tex_load == 0)
        {
#ifdef BUILD_EU_VERSION
            ms_load_id = LoadReqLanguage(M_SLCT_CMN_E_PK2, 0xc80000);
#else
            ms_load_id = LoadReq(M_SLCT_CMN_PK2, 0xc80000);
#endif
        }
        adpcm_no = -1;
        MsLoadCtrl(mode);
        dsp_ms.main_step = 1;
        
        if (dsp_ms.now_mode == 2 || dsp_ms.now_mode == 7)
        {
            adpcm_no = AB000_STR;
            SeStopAll();
        }
        else if (dsp_ms.now_mode == 0)
        {
            adpcm_no = AB027_STR;
        }
        else if (dsp_ms.now_mode == 1)
        {
            adpcm_no = AB027_STR;
        }
        else if (dsp_ms.now_mode == 4)
        {
            title_bgm_no = -1;
        }
        else if (dsp_ms.now_mode == 3)
        {
            title_bgm_no = -1;
        }
        
        if (adpcm_no != -1)
        {
            if (title_bgm_no != adpcm_no)
            {
                EAdpcmCmdPlay(0, 1, adpcm_no, 0, GetAdpcmVol(adpcm_no), 0x280, 0xfff, 0);
                title_bgm_no = adpcm_no;
            }
        }
        break;
    case 1:
        if (ms_load_id == -1)
        {
            if (cmn_tex_load == 0)
            {
                cmn_tex_load = 1;
            }
            dsp_ms.bak_alp = 0;
            dsp_ms.chr_alp = 0;
            dsp_ms.csr[1] = 0;
            if (mode == 3)
            {
                StartOptionModeInit(1);
                dsp_ms.csr[2] = 0;
                dsp_ms.opt_mode = 7;
            }
            else if (mode == 1)
            {
                dsp_ms.sm_slct[2] = cribo.costume;
                dsp_ms.sm_slct[1] = ingame_wrk.difficult;
                dsp_ms.sm_slct[0] = 0;
                SetCsrInfoStoryMode();
            }
            else if (mode == 2)
            {
                dsp_ms.bm_slct[0] = cribo.costume;
#ifdef BUILD_EU_VERSION
                SetCsrInfoBattleMode();
#endif
            }
            dsp_ms.main_step = 2;
            SeSetMVol(opt_wrk.bgm_vol);
            SeSetSteMono(opt_wrk.sound_mode);
        }
    break;
    case 2:
        if (dsp_ms.bak_alp + 8 < 128)
        {
            dsp_ms.bak_alp += 8;
            dsp_ms.chr_alp = dsp_ms.bak_alp;
            if (dsp_ms.flm_alp != 128)
            {
                dsp_ms.flm_alp = dsp_ms.chr_alp;
            }
            
            if (dsp_ms.win_alp != 128)
            {
                dsp_ms.win_alp = dsp_ms.bak_alp;
            }
        }
        else
        {
            dsp_ms.flm_alp = 128;
            dsp_ms.win_alp = 128;
            dsp_ms.bak_alp = 128;
            dsp_ms.chr_alp = 128;
            dsp_ms.flm_lng = 0;
            dsp_ms.main_step = 3;
        }
    break;
    case 3:
        if (dsp_ms.flm_lng + 20 < 100)
        {
            dsp_ms.flm_lng += 20;
        }
        else if (dsp_ms.now_mode == 3)
        {
            if (IsEndAdpcmFadeOut())
            {
            EAdpcmCmdPlay(0, 1, AB018_STR, 0, GetAdpcmVol(AB018_STR), 0x280, 0xfff, 0);
                dsp_ms.flm_lng = 100;
                dsp_ms.main_step = 4;
            }
        }
        else
        {
            dsp_ms.flm_lng = 100;
            dsp_ms.main_step = 4;
        }
    break;
    case 4:
        if (ModeSlctPad(mode))
        {
            dsp_ms.main_step = 5;
        }
    break;
    case 5:
        if (dsp_ms.flm_lng - 20 >= 1)
        {
            dsp_ms.flm_lng = dsp_ms.flm_lng - 20;
        }
        else
        {
            dsp_ms.flm_lng = 0;
            dsp_ms.main_step = 6;
        }
    break;
    case 6:
        if (dsp_ms.chr_alp - 8 >= 1)
        {
            dsp_ms.chr_alp -= 8;
            dsp_ms.win_alp = dsp_ms.chr_alp;
            dsp_ms.bak_alp = dsp_ms.chr_alp;
            
            if (
                dsp_ms.next_mode == 5 || dsp_ms.next_mode == 0xb ||
                dsp_ms.next_mode == 8 || dsp_ms.next_mode == 9 ||
                dsp_ms.next_mode == 10 || dsp_ms.next_mode == 12
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
            if (dsp_ms.next_mode == 5 || dsp_ms.next_mode == 11 || dsp_ms.next_mode == 12)
            {
                dsp_ms.flm_alp = 0;
            }
            dsp_ms.main_step = 7;
        }
    break;
    case 7:
        dsp_ms.main_step = 0;
        if (dsp_ms.now_mode == 4)
        {
            SeSetMVol(opt_wrk.bgm_vol);
        }
        
        if (dsp_ms.next_mode == 5)
        {
            outgame_wrk.mode = 4;
            EAdpcmFadeOut(0x3c);
            if (title_wrk.mode != 27)
            {
                title_wrk.mode = 26;
            }
            title_bgm_no = -1;
        }
        else if (dsp_ms.next_mode == 11)
        {
            outgame_wrk.mode = 4;
        }
        else if (dsp_ms.next_mode == 12)
        {
            ingame_wrk.game = 0;
            GameModeChange(0);
            title_bgm_no = -1;
        }
        else
        {
            int now_mode_4 = 4;
            dsp_ms.now_mode = dsp_ms.next_mode;
            if (dsp_ms.now_mode == now_mode_4)
            {
                SoundTestForModeSlectInit();
            }
        }
    break;
    }
    
    if (dsp_ms.now_mode != 8 && dsp_ms.now_mode != 9 && dsp_ms.now_mode != 10 && cmn_tex_load != 0)
    {
        ModeSlctDspBak(dsp_ms.bak_alp, mode);
        ModeSlctDspFlm(dsp_ms.flm_lng, dsp_ms.flm_alp, mode);
        ModeSlctDspChr(dsp_ms.chr_alp, mode);
        if (dsp_ms.now_mode != 3)
        {
            ModeSlctDspWin(dsp_ms.win_alp);
        }
        ModeSlctDspMsg(dsp_ms.win_alp, mode);
    }
}

// u_char sm_slct_num[3] = {6, 2, 4};
char ModeSlctPad(u_char mode)
{
    u_char rtrn;
    u_char sm_slct_num[3] = {6, 2, 4};
    u_char bm_slct_num[1] = {4};
    int level;

    rtrn = 0;
    switch (mode)
    {
    case 0:
        if (*key_now[4] == 1)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);
            dsp_ms.next_mode = 5;
            rtrn = 1;
        }
        else if (*key_now[5] == 1)
        { 
            SeStartFix(1, 0, 0x1000, 0x1000, 0);
            dsp_ms.next_mode = dsp_ms.csr[0] + 1;
            if (dsp_ms.next_mode == 3 || dsp_ms.next_mode == 4)
            {
                EAdpcmFadeOut(0x3c);
            }
            rtrn = 1;
        }
        else
        {
            if (
                *key_now[0] == 1 ||
                (*key_now[0] > 0x19 && (*key_now[0] % 5) == 1) ||
                Ana2PadDirCnt(0) == 1 ||
                (Ana2PadDirCnt(0) > 0x19 && (Ana2PadDirCnt(0) % 5) == 1)
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
                *key_now[1] == 1 ||
                (*key_now[1] > 0x19 && (*key_now[1] % 5) == 1) ||
                Ana2PadDirCnt(2) == 1 ||
                (Ana2PadDirCnt(2) > 0x19 && Ana2PadDirCnt(2) % 5 == 1)
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
    case 1:
        if (*key_now[4] == 1)
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
#ifdef BUILD_EU_VERSION
            cribo.costume = dsp_ms.sm_slct[2];
#endif
        }
        else if (*key_now[5] == 1)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);
            switch (dsp_ms.csr[1])
            {
            case 0:
                dsp_ms.side = 1 - dsp_ms.side;
                if (
                    dsp_ms.side == 0 &&
                    dsp_ms.sm_slct[0] == 0 &&
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
                EAdpcmFadeOut(0x3c);
                dsp_ms.next_mode = 12;
                if (dsp_ms.sm_slct[0] == 0)
                {
                    sys_wrk.load = 1;
                    title_wrk.csr = 0;
                    ingame_wrk.mode = 6;
                }
                else
                {
                    ingame_wrk.msn_no = dsp_ms.sm_slct[0] - 1;
                    sys_wrk.load = 0;
                    mc_msn_flg = 0;
                    title_wrk.csr = 0;
                    if (ingame_wrk.msn_no != 0)
                    {
                        cribo.clear_info = cribo.clear_info & 0xfd;
                    }
                }
                ingame_wrk.difficult = dsp_ms.sm_slct[1];
                cribo.costume = dsp_ms.sm_slct[2];
                if (ingame_wrk.msn_no == 0)
                {
                    cribo.clear_info = cribo.clear_info | 2;
                }
                rtrn = 1;
            break;
            case 4:
                dsp_ms.next_mode = 0;
#ifdef BUILD_EU_VERSION
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
                    *key_now[0] == 1 ||
                    (*key_now[0] > 0x19 && (*key_now[0] % 5) == 1) ||
                    Ana2PadDirCnt(0) == 1 ||
                    (Ana2PadDirCnt(0) > 0x19 && (Ana2PadDirCnt(0) % 5) == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                    if (dsp_ms.csr[1] > sm_csr_info[0])
                    {
                        dsp_ms.csr[1]--;
                    }
                    else
                    {
                        dsp_ms.csr[1] = 4;
                    }
                }
                else if (
                    *key_now[1] == 1 ||
                    (*key_now[1] > 0x19 && (*key_now[1] % 5) == 1) ||
                    Ana2PadDirCnt(2) == 1 ||
                    (Ana2PadDirCnt(2) > 0x19 && (Ana2PadDirCnt(2) % 5) == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                    if (dsp_ms.csr[1] <= 3)
                    {
                        dsp_ms.csr[1]++;
                    }
                    else
                    {
                        dsp_ms.csr[1] = sm_csr_info[0];
                    }
                }
            break;
            case 1:
                if (
                    *key_now[2] == 1 ||
                     (*key_now[2] > 0x19 && (*key_now[2] % 5) == 1) ||
                    Ana2PadDirCnt(3) == 1 ||
                    (Ana2PadDirCnt(3) > 0x19 && (Ana2PadDirCnt(3) % 5) == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
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
                    *key_now[3] == 1 ||
                    (*key_now[3] > 0x19 && (*key_now[3] % 5) == 1) ||
                    Ana2PadDirCnt(1) == 1 ||
                    (Ana2PadDirCnt(1) > 0x19 && (Ana2PadDirCnt(1) % 5) == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
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
    case 2:
        if (*key_now[4] == 1)
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
        else if (*key_now[5] == 1)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);
            switch (dsp_ms.csr[1])
            {
            case 0:
                dsp_ms.side = 1 - dsp_ms.side;
            break;
            case 1:
                dsp_ms.next_mode = 8;
                rtrn = 1;
            break;
            case 2:
                dsp_ms.next_mode = 9;
                rtrn = 1;
            break;
            case 3:
                dsp_ms.next_mode = 10;
                rtrn = 1;
            break;
            case 4:
                dsp_ms.next_mode = 7;
                btl_wrk.stage_no = 0;
                cribo.costume = dsp_ms.bm_slct[0];
                rtrn = 1;
            break;
            case 5:
                dsp_ms.next_mode = 0;
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
                    *key_now[0] == 1 ||
                     (*key_now[0] > 0x19 && (*key_now[0] % 5) == 1) ||
                    Ana2PadDirCnt(0) == 1 ||
                    (Ana2PadDirCnt(0) > 0x19 && (Ana2PadDirCnt(0) % 5) == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                    if (dsp_ms.csr[1] != 0)
                    {
                        dsp_ms.csr[1]--;
                    }
                    else
                    {
                        dsp_ms.csr[1] = 5;
                    }
                }
                else if (
                    *key_now[1] == 1 ||
                    (*key_now[1] > 0x19 && (*key_now[1] % 5) == 1) ||
                    Ana2PadDirCnt(2) == 1 ||
                    (Ana2PadDirCnt(2) > 0x19 && (Ana2PadDirCnt(2) % 5) == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                    if (dsp_ms.csr[1] < 5)
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
                    *key_now[2] == 1 ||
                    (*key_now[2] > 0x19 && (*key_now[2] % 5) == 1) ||
                    Ana2PadDirCnt(3) == 1 ||
                    (Ana2PadDirCnt(3) > 0x19 && (Ana2PadDirCnt(3) % 5) == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
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
                    *key_now[3] == 1 ||
                    (*key_now[3] > 0x19 && (*key_now[3] % 5) == 1) ||
                    Ana2PadDirCnt(1) == 1 ||
                    (Ana2PadDirCnt(1) > 0x19 && Ana2PadDirCnt(1) % 5 == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
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
    case 3:
        if (MenuOptionPad(&dsp_ms.csr[2], &dsp_ms.opt_mode))
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);
            EAdpcmFadeOut(0x3c);
            dsp_ms.next_mode = dsp_ms.end_mode;
            rtrn = 1;
        }
    break;
    case 4:
        if (SoundTestForModeSlect())
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);
            dsp_ms.next_mode = dsp_ms.end_mode;
            rtrn = 1;
        }
    break;
    case 7:
        level = CheckClearStage();
        if (*key_now[4] == 1)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);
            dsp_ms.next_mode = 0x2;
            OutGameModeChange(0x8);
            rtrn = 1;
        }
        else if (*key_now[5] == 1)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);
            ingame_wrk.game = 1;
            TitleModeEnd();
            sys_wrk.load = 0;
            SaveStoryWrk();
            GameModeChange(0);
            title_bgm_no = -1;
            if (*key_now[10] != 0)
            {
                battle_scr_effct[btl_wrk.stage_no] = 6;
            }
            else if (*key_now[11] != 0)
            {
                battle_scr_effct[btl_wrk.stage_no] = 4;
            }
            else if (*key_now[8] != 0)
            {
                battle_scr_effct[btl_wrk.stage_no] = 2;
            }
            else if (*key_now[9] != 0)
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
                *key_now[0] == 1 ||
                (*key_now[0] > 0x19 && (*key_now[0] % 5) == 1) ||
                Ana2PadDirCnt(0) == 1 ||
                (Ana2PadDirCnt(0) > 0x19 && (Ana2PadDirCnt(0) % 5) == 1)
            )
            {
                if (btl_wrk.stage_no % 5 != 0)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                    btl_wrk.stage_no--;
                }
                else
                {
                    btl_wrk.stage_no += 4;
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                }
            }
            else
            {
                if (
                    *key_now[1] == 1 ||
                    (*key_now[1] > 0x19 && (*key_now[1] % 5) == 1) ||
                    Ana2PadDirCnt(2) == 1 ||
                    (Ana2PadDirCnt(2) > 0x19 && (Ana2PadDirCnt(2) % 5) == 1)
                )
                {
                    if ((btl_wrk.stage_no + 1) % 5 != 0)
                    {
                        SeStartFix(0, 0, 0x1000, 0x1000, 0);
                        btl_wrk.stage_no++;
                    }
                    else
                    {
                        btl_wrk.stage_no -= 4;
                        SeStartFix(0, 0, 0x1000, 0x1000, 0);
                    }
                }
                else
                {
                    if (
                        *key_now[2] == 1 ||
                        (*key_now[2] > 0x19 && (*key_now[2] % 5) == 1) ||
                        Ana2PadDirCnt(3) == 1 ||
                        (Ana2PadDirCnt(3) > 0x19 && (Ana2PadDirCnt(3) % 5) == 1)
                    )
                    {
                        if (btl_wrk.stage_no >= 5)
                        {
                            SeStartFix(0, 0, 0x1000, 0x1000, 0);
                            btl_wrk.stage_no -= 5;
                        }
                    }
                    else if (
                        *key_now[3] == 1 || 
                        (*key_now[3] > 0x19 && (*key_now[3] % 5) == 1) ||
                        Ana2PadDirCnt(1) == 1 ||
                        (Ana2PadDirCnt(1) > 0x19 && (Ana2PadDirCnt(1) % 5) == 1)
                    )
                    {
                        if (btl_wrk.stage_no <= 0xe && btl_wrk.stage_no / 5 < level)
                        {
                            SeStartFix(0, 0, 0x1000, 0x1000, 0);
                            btl_wrk.stage_no += 5;
                        }
                    }
                }
            }
        }
    break;
    case 8:
        if (!isCameraEnd())
        {
            BgFusumaYW(0x606060, 0.0f, GetCamDispAlpha(), 0x7d000);
#ifdef BUILD_EU_VERSION
            SetSprFile(0x1e90000);
            SetSprFile(0x1ce0000);
            SetSprFile(0x1d10000);
#else
            SetSprFile(0x1e90000);
            SetSprFile(0x1ce0000);
            SetSprFile(0x1d15600);
#endif
            CameraCustomMain();
        }
        else
        {
            dsp_ms.next_mode = 2;
            OutGameModeChange(8);
            rtrn = 1;
        }
    break;
    case 9:
        if (!isPhotoEnd())
        {
            BgFusumaYW(0x606060, 0.0f, GetPhtDispAlpha(), 0x7d000);
#ifdef BUILD_EU_VERSION
            SetSprFile(0x1d51db0);
            SetSprFile(0x1ce0000);
            SetSprFile(0x1d10000);
#else
            SetSprFile(0x1d573b0);
            SetSprFile(0x1ce0000);
            SetSprFile(0x1d15600);
#endif
            IngameMenuAlbum(1);
        }
        else
        {
            dsp_ms.next_mode = 2;
            OutGameModeChange(8);
            rtrn = 1;
        }
    break;
    case 10:
        if (SavePointMenuMain(3))
        {
            dsp_ms.next_mode = 2;
            OutGameModeChange(8);
            rtrn = 1;
        }
    break;
    }
    return rtrn;
}

void ModeSlctDspBak(u_char alp, u_char mode)
{
    int i;
    DISP_SPRT ds;

    if (alp == 0x0)
    {
        return;
    }
    
    SetSprFile(0xc80000);
    SetSprFile(0xcc0470);
    
    for (i = 0; i < 11; i++)
    {
        switch (mode)
        {
        case 0:
            CopySprDToSpr(&ds, &msl_bg_fsm[i]);
            break;
        case 1:
            CopySprDToSpr(&ds, &msl_bg_sty[i]);
            ds.r = 0x64;
            ds.g = 0x64;
            ds.b = 0x64;
            break;
        case 2:
            CopySprDToSpr(&ds, &msl_bg_btl[i]);
            ds.r = 0x64;
            ds.g = 0x64;
            ds.b = 0x64;
            break;
        case 3:
            CopySprDToSpr(&ds, &msl_bg_opt[i]);
            break;
        case 4:
            CopySprDToSpr(&ds, &msl_bg_snd[i]);
            break;
        case 7:
            CopySprDToSpr(&ds, &msl_bg_btl[i]);
            if (0x50 < alp)
            {
                alp = 0x50;
            }
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

    if (alp != 0x0)
    {
        flsh = SgSinf(((dsp_ms.mode_tmr % 0x3c) * 6.2831855f) / 60.0f);
        flsh = ((flsh + 1.0f) * 0.5f + 1.0f) * 0.5f;
        switch (mode)
        {
        case 0:
            DspModeSlect(alp);
        break;
        case 1:
            DspStoryMode(alp, flsh);
        break;
        case 2:
            DspBattleMode(alp, flsh);
        break;
        case 3:
            SetSprFile(0x1ce0000);
            SetSprFile(0xddc430);
            DspOptCtrl(dsp_ms.csr[2], dsp_ms.opt_mode, alp, 0x0);
        break;
        case 4:
            SoundTestForModeSlectDisp(alp, flsh);
        break;
        case 7:
            SetSprFile(0xdcb100);
            DspMissionSelect(alp);
        }
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
    
    PutChrOne(0x46, 0, 0, dsp_rgb[0], dsp_alp[0], 0x0);
    PutChrOne(0x47, 0, 0, dsp_rgb[1], dsp_alp[1], 0x0);
    PutChrOne(0x4a, 0, 0, dsp_rgb[2], dsp_alp[2], 0x0);
    PutChrOne(0x48, 0, 0, dsp_rgb[3], dsp_alp[3], 0x0);
    PutChrOne(0x49, 0, 0, dsp_rgb[4], dsp_alp[4], 0x0);
    
    SetSprFile(0x1ce0000);
    DispCaption(3, alp);
}

void DspStoryMode(u_char alp, float flsh)
{
    int i;
    u_int dsp_rgb[11];
    u_char dsp_alp[11];

    for (i = 0; i < 5; i++)
    {
        if (i == dsp_ms.csr[1])
        {
            if (dsp_ms.side)
            {
                dsp_alp[i] = (alp * 7) / 10;
                dsp_rgb[i] = 0x595959;
            }
            else
            {
                dsp_alp[i] = alp;
                dsp_rgb[i] = 0x808080;
            }
        }
        else
        {
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
        }
    }
    
    for (i = 0; i < 3; i++)
    {
        if ((dsp_ms.side) && (i == dsp_ms.csr[1] ))
        {
            dsp_alp[(i+8)-3] = alp;
            dsp_rgb[(i+8)-3] = 0x808080;
            dsp_alp[i+8] = alp * flsh;
            dsp_rgb[i+8] = 0x808080;
            
        }
        else
        {
            if (i >= sm_csr_info[0])
            {
                dsp_alp[(i+8)-3] = alp / 2;
                dsp_rgb[(i+8)-3] = 0x404040;
                dsp_alp[i+8] = alp / 2;
                dsp_rgb[i+8] = 0x808080;
            }
            else
            {
                dsp_alp[(i+8)-3] = dsp_ms.unsel_col[3];
                dsp_rgb[(i+8)-3] = 
                    dsp_ms.unsel_col[0] |
                    dsp_ms.unsel_col[1] << 8 |
                    dsp_ms.unsel_col[2] << 16;
                dsp_alp[i+8] = dsp_ms.unsel_col[3];
                dsp_rgb[i+8] =
                    dsp_ms.unsel_col[0] |
                    dsp_ms.unsel_col[1] << 8 |
                    dsp_ms.unsel_col[2] << 16;
            }
        }
    }
    
    PutChrOneRGB(0x65, 0, 0, dsp_rgb[0], dsp_alp[0], 0x0);
    PutChrOneRGB(0x66, 0, 0, dsp_rgb[1], dsp_alp[1], 0x0);
    PutChrOneRGB(0x72, 0, 0, dsp_rgb[2], dsp_alp[2], 0x0);
    PutChrOneRGB(0x67, 0, 0, dsp_rgb[3], dsp_alp[3], 0x0);
    PutChrOneRGB(0x68, 0, 0, dsp_rgb[4], dsp_alp[4], 0x0);
    PutChrOneRGB(0x7b, 0x148, 0x65, dsp_rgb[8], dsp_alp[8], 0x2);
    PutChrOneRGB(0x7b, 0x23a, 0x65, dsp_rgb[8], dsp_alp[8], 0x0);
    PutChrOneRGB(0x79, 0x15c, 100, dsp_rgb[8], dsp_alp[8], 0x0);
    PutChrOneRGB(0x7a, 0x225, 100, dsp_rgb[8], dsp_alp[8], 0x0);
    
    switch (dsp_ms.sm_slct[0])
    {
    case 0:
        PutChrOneRGB(0x84, 0, 0, dsp_rgb[5], dsp_alp[5], 0x0);
    break;
    case 1:
        PutChrOneRGB(0x7f, 0, 0, dsp_rgb[5], dsp_alp[5], 0x0);
    break;
    case 2:
        PutChrOneRGB(0x80, 0, 0, dsp_rgb[5], dsp_alp[5], 0x0);
    break;
    case 3:
        PutChrOneRGB(0x81, 0, 0, dsp_rgb[5], dsp_alp[5], 0x0);
    break;
    case 4:
        PutChrOneRGB(0x82, 0, 0, dsp_rgb[5], dsp_alp[5], 0x0);
    break;
    case 5:
        PutChrOneRGB(0x83, 0, 0, dsp_rgb[5], dsp_alp[5], 0x0);
    break;
    }
    
    PutChrOneRGB(0x7b, 0x148, 0x91, dsp_rgb[9], dsp_alp[9], 0x2);
    PutChrOneRGB(0x7b, 0x23a, 0x91, dsp_rgb[9], dsp_alp[9], 0x0);
    PutChrOneRGB(0x79, 0x15c, 0x90, dsp_rgb[9], dsp_alp[9], 0x0);
    PutChrOneRGB(0x7a, 0x225, 0x90, dsp_rgb[9], dsp_alp[9], 0x0);
    
    switch (dsp_ms.sm_slct[1])
    {
    case 0:
        PutChrOneRGB(0x74, 0x17e, 0x8d, dsp_rgb[6], dsp_alp[6], 0x0);
    break;
    case 1:
        PutChrOneRGB(0x76, 0xfffb, 0, dsp_rgb[6], dsp_alp[6], 0x0);
    break;
    }
    
    PutChrOneRGB(0x7b, 0x148, 0xc0, dsp_rgb[10], dsp_alp[10], 0x2);
    PutChrOneRGB(0x7b, 0x23a, 0xc0, dsp_rgb[10], dsp_alp[10], 0x0);
    PutChrOneRGB(0x79, 0x15c, 0xbf, dsp_rgb[10], dsp_alp[10], 0x0);
    PutChrOneRGB(0x7a, 0x225, 0xbf, dsp_rgb[10], dsp_alp[10], 0x0);

    switch (dsp_ms.sm_slct[2])
    {
    case 0:
        PutChrOneRGB(0x74, 0x17e, 0xbb, dsp_rgb[7], dsp_alp[7], 0x0);
    break;
    case 1:
        PutChrOneRGB(0x77, 0xfff5, 0, dsp_rgb[7], dsp_alp[7], 0x0);
        PutChrOneRGB(0x85, 0x20f, 0xbb, dsp_rgb[7], dsp_alp[7], 0x0);
    break;
    case 2:
        PutChrOneRGB(0x77, 0xfff5, 0, dsp_rgb[7], dsp_alp[7], 0x0);
        PutChrOneRGB(0x86, 0x20f, 0xbb, dsp_rgb[7], dsp_alp[7], 0x0);
    break;
    case 3:
        PutChrOneRGB(0x77, 0xfff5, 0, dsp_rgb[7], dsp_alp[7], 0x0);
        PutChrOneRGB(0x87, 0x20f, 0xbb, dsp_rgb[7], dsp_alp[7], 0x0);
    break;
    }
    
    SetSprFile(0x1ce0000);
    DispCaption(3, alp);
}

void DspBattleMode(u_char alp, float flsh)
{
    int i;
    u_char dsp_rgb[9];
    u_char dsp_alp[9];

    for (i = 0; i < 6; i++)
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
            dsp_alp[i+6] = alp;
            dsp_rgb[i+6] = 0x80;
            dsp_rgb[i+7] = 0x80;
            dsp_alp[i+7] = alp * flsh;
        }
        else
        {
            dsp_alp[i+6] = alp / 2;
            dsp_rgb[i+6] = 0x40;
            dsp_rgb[i+7] = 0x80;
            dsp_alp[i+7] = alp / 2;
        }
    }
    
    PutChrOne(10, 0, 0xffe7, dsp_rgb[0], dsp_alp[0], 0x0);
#ifdef BUILD_EU_VERSION
    // nothing to do
#else
    PutChrOne(0x16, 0, 0xffec, dsp_rgb[1], dsp_alp[1], 0x0);
#endif
    PutChrOne(0x17, 0, 0xffec, dsp_rgb[1], dsp_alp[1], 0x0);
    PutChrOne(0x19, 0, 0xfff1, dsp_rgb[2], dsp_alp[2], 0x0);
    PutChrOne(0x1a, 0, 0xfff6, dsp_rgb[3], dsp_alp[3], 0x0);
    
    PutChrOne(9, 0, 0xfffb, dsp_rgb[4], dsp_alp[4], 0x0);
    PutChrOne(0x18, 0, 0, dsp_rgb[5], dsp_alp[5], 0x0);
    PutChrOne(0x12, 0x13a, 99, dsp_rgb[7], dsp_alp[7], 0x2);
    PutChrOne(0x12, 0x248, 99, dsp_rgb[7], dsp_alp[7], 0x0);
    PutChrOne(0x10, 0x150, 0x62, dsp_rgb[7], dsp_alp[7], 0x0);
    PutChrOne(0x11, 0x230, 0x62, dsp_rgb[7], dsp_alp[7], 0x0);

    switch (dsp_ms.bm_slct[0])
    {
    case 0:
        PutChrOne(0xb, 0x17b, 0x5f, dsp_rgb[6], dsp_alp[6], 0x0);
    break;
    case 1:
        PutChrOne(0xe, 0xfff2, 0xffd2, dsp_rgb[6], dsp_alp[6], 0x0);
        PutChrOne(0x1b, 0x20c, 0x5f, dsp_rgb[6], dsp_alp[6], 0x0);
    break;
    case 2:
        PutChrOne(0xe, 0xfff2, 0xffd2, dsp_rgb[6], dsp_alp[6], 0x0);
        PutChrOne(0x1c, 0x20c, 0x5f, dsp_rgb[6], dsp_alp[6], 0x0);
    break;
    case 3:
        PutChrOne(0xe, 0xfff2, 0xffd2, dsp_rgb[6], dsp_alp[6], 0x0);
        PutChrOne(0x1d, 0x20c, 0x5f, dsp_rgb[6], dsp_alp[6], 0x0);
    break;
    }
    
    SetSprFile(0x1ce0000);
    DispCaption(3, alp);
}

// static u_char csr_alpha = 0;
// static u_char csr_brink = 1;
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
    
    PutChrOne(0x3a, 0, 0, 0x80, alp, 0x0);
    PutChrOne(0x3b, 0, 0, 0x80, alp, 0x0);

    for (i = 0; i <= 4; i++)
    {
        PutChrOne(0x43, 0, i * 0x28, 0x80, alp, 0);
    }
    
    for (i = 0; i <= 4; i++)
    {
        if (stage_wrk[no + i].rank)
        {
            PutChrOne(0x44, 0, i * 0x28, 0x80, alp, 0x0);
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
    // iVar8 = (btl_wrk.stage_no % 5) * 0x28;
    SetSquareS(0x25000, -240.0f, (page_base_no % 5) * 0x28 -0x5e, 196.0f, (page_base_no % 5) * 0x28 - 0x40, 0x8c, 0x80, 0x3c, csr_alpha);

    if (page_base_no > 4)
    {
        PutChrOne(0x45, 0x17, 0xcb, 0x80, alp, 0x0);
    }
    
    if ((page_base_no < 0xf) && (no / 5 < level))
    {
        PutChrOne(0x45, 0x24b, 0xcb, 0x80, alp, 0x2);
    }
    
    for (i = 0; i < 5; i++)
    {
        PutStringYW(0x2e, 0xa, 0x55, i * 0x28 + 0x85, 0x808080, alp, 0x23000, 0x0);
        PutNumberYW(0x3, no + i + 1, 0xcc, i * 0x28 + 0x85, 1.0, 1.0, 0x808080, alp, 0x23000, 2, 1);
        PutNumberYW(0x3, stage_wrk[no + i].best_time / 0xe10, 0x104, i * 0x28 + 0x85, 1.0, 1.0, 0x808080, alp, 0x23000, 1, 1);
        PutStringYW(0x2e, 0xb, 0x111, i * 0x28 + 0x85, 0x808080, alp, 0x23000, 0x0);
        PutNumberYW(0x3, (stage_wrk[no + i].best_time / 0x3c) % 0x3c, 0x11f, i * 0x28 + 0x85, 1.0, 1.0, 0x808080, alp, 0x23000, 2, 1);
        PutStringYW(0x2e, 0xb, 0x13e, i * 0x28 + 0x85, 0x808080, alp, 0x23000, 0x0);
        PutNumberYW(0x3, ((stage_wrk[no + i].best_time * 100) / 0x3c) % 100, 0x14a, i * 0x28 + 0x85, 1.0, 1.0, 0x808080, alp, 0x23000, 2, 1);
        PutNumberYW(0x3, stage_wrk[no + i].best_shot, 0x193, i * 0x28 + 0x85, 1.0, 1.0, 0x808080, alp, 0x23000, 4, 0);
        PutStringYW(0x2e, 0xc, 0x1d4, i * 0x28 + 0x85, 0x808080, alp, 0x23000, 0x0);

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
            PutChrOne(0x41, 0, i * 0x28, 0x80, alp, 0x0);
        break;
        case 2:
            PutChrOne(0x40, 0, i * 0x28, 0x80, alp, 0x0);
        break;
        case 3:
            PutChrOne(0x3f, 0, i * 0x28, 0x80, alp, 0x0);
        break;
        case 4:
            PutChrOne(0x3e, 0, i * 0x28, 0x80, alp, 0x0);
        break;
        case 5:
            PutChrOne(0x3d, 0, i * 0x28, 0x80, alp, 0x0);
        break;
        case 6:
            PutChrOne(0x3c, 0, i * 0x28, 0x80, alp, 0x0);
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
            PutRayName(2, 1, 3, 0x50, i * 0x18 + 0x165, 0x808080, alp);
        }
        else
        {
            PutRayName(1, ene_no, msn_no, 0x50, i * 0x18 + 0x165, 0x808080, alp);
        }
    }

    SetSprFile(0x1ce0000);
    DispCaption(4, alp);
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

void ModeSlctDspFlm(u_char per, u_char alp, u_char mode)
{
    short int mx;
    DISP_SPRT ds;

    if (alp == 0)
    {
        return;
    }

    mx = ((100 - per) * 0x1f9) / 100.0f;

    switch (mode)
    {
    case 0:
        FilmCutter(0, -mx, 0, 0x59, 0x80, alp);
        FilmCutter(3, -mx, 0, 0x59, 0x80, alp);
        PutChrOne(7, 0, 0, 0x80, alp, 0x0);
    break;
    case 1:
        FilmCutter(0, -mx, 0, 0x59, 0x80, alp);
        FilmCutter(4, -mx, 0, 0x59, 0x80, alp);
        PutChrOne(7, 0, 0, 0x80, alp, 0x0);
    break;
    case 2:
        FilmCutter(0, -mx, 0, 0x59, 0x80, alp);
        FilmCutter(2, -mx, 0, 0x59, 0x80, alp);
        PutChrOne(7, 0, 0, 0x80, alp, 0x0);
    break;
    case 3:
        FilmCutter(0, -mx, 0, 0x59, 0x80, alp);
        FilmCutter(6, -mx, 0, 0x59, 0x80, alp);
        PutChrOne(7, 0, 0, 0x80, alp, 0x0);
    break;
    case 4:
        FilmCutter(0, -mx, 0, 0x59, 0x80, alp);
        FilmCutter(5, -mx, 0, 0x59, 0x80, alp);
        PutChrOne(7, 0, 0, 0x80, alp, 0x0);
    break;
    case 7:
        FilmCutter(0, -mx, 0, 0x59, 0x80, alp);
        FilmCutter(1, -mx, 0, 0x59, 0x80, alp);
        PutChrOne(7, 0, 0, 0x80, alp, 0x0);
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
    ds.tex1 = 0x161;
    
    DispSprD(&ds);
}

void ModeSlctDspWin(u_char alp)
{
    DISP_SPRT ds;
    
    if (alp != 0x0)
    {
        DrawMessageBox(0x19000, 24.0f, 340.0f, 593.0f, 84.0f, alp);
    }
}

void MsLoadCtrl(u_char mode)
{    
    switch(mode)
    {
    case 0:
        ms_load_id = LoadReq(EFF001_PK2, 0x1e90000);
#ifdef BUILD_EU_VERSION
        ms_load_id = LoadReqLanguage(M_SLCT_FSM_E_PK2,0xcc0470);
#else
        ms_load_id = LoadReq(M_SLCT_FSM_PK2,0xcc0470);
#endif
    break;
    case 1:
        ingame_wrk.game = 0;
#ifdef BUILD_EU_VERSION
        ms_load_id = LoadReqLanguage(M_SLCT_STY_E_PK2, 0xcc0470);
#else
        ms_load_id = LoadReq(M_SLCT_STY_PK2, 0xcc0470);
#endif
        if (ingame_wrk.difficult != 0x0)
        {
            dsp_ms.sm_slct[1] = 0x1;
        }
        
    break;
    case 2:
        ingame_wrk.game = 1;
#ifdef BUILD_EU_VERSION
        ms_load_id = LoadReqLanguage(M_SLCT_BTL_E_PK2, 0xcc0470);
        ms_load_id = LoadReqLanguage(M_SLCT_BTL_CHR_E_PK2, 0xd4a850);
#else
        ms_load_id = LoadReq(M_SLCT_BTL_PK2, 0xcc0470);
        ms_load_id = LoadReq(M_SLCT_BTL_CHR_PK2, 0xd4a850);
#endif
        
    break;
    case 3:
#ifdef BUILD_EU_VERSION
        ms_load_id = LoadReqLanguage(M_SLCT_OPT_E_PK2, 0xcc0470);
        ms_load_id = LoadReqLanguage(PL_OPTI_E_PK2, 0xddc430);
        ms_load_id = LoadReqLanguage(PL_STTS_E_PK2, 0x1ce0000);
#else
        ms_load_id = LoadReq(M_SLCT_OPT_PK2, 0xcc0470);
        ms_load_id = LoadReq(PL_OPTI_PK2, 0xddc430);
        ms_load_id = LoadReq(PL_STTS_PK2, 0x1ce0000);
#endif
        break;
    case 4:
#ifdef BUILD_EU_VERSION
        ms_load_id = LoadReqLanguage(M_SLCT_SND_E_PK2, 0xcc0470);
#else
        ms_load_id = LoadReq(M_SLCT_SND_PK2, 0xcc0470);
#endif
    break;
    case 7:
#ifdef BUILD_EU_VERSION
        if (cmn_tex_load == 0)
        {
            ms_load_id = LoadReqLanguage(M_SLCT_BTL_E_PK2, 0xcc0470);
        }        
        ms_load_id = LoadReqLanguage(M_SLCT_BTL_MSN_E_PK2, 0xdcb100);
#else
        if (cmn_tex_load == 0)
        {
            ms_load_id = LoadReq(M_SLCT_BTL_PK2, 0xcc0470);
        }        
        ms_load_id = LoadReq(M_SLCT_BTL_MSN_PK2, 0xdcb100);
#endif
    break;
    case 8:
        CameraCustomInit();
        OutGameInitCamera();
#ifdef BUILD_EU_VERSION
        ms_load_id = LoadReq(PL_BGBG_PK2, 0x1cfefc0);
        ms_load_id = LoadReqLanguage(PL_MTOP_E_PK2, 0x1d10000);
        ms_load_id = LoadReqLanguage(PL_STTS_E_PK2, 0x1ce0000);
        ms_load_id = LoadReqLanguage(PL_CAME_E_PK2, 0x1e90000);
#else
        ms_load_id = LoadReq(PL_BGBG_PK2, 0x1d05140);
        ms_load_id = LoadReq(PL_MTOP_PK2, 0x1d15600);
        ms_load_id = LoadReq(PL_STTS_PK2, 0x1ce0000);
        ms_load_id = LoadReq(PL_CAME_PK2, 0x1e90000);
#endif
    break;
    case 9:
#ifdef BUILD_EU_VERSION
        ms_load_id = LoadReq(PL_BGBG_PK2, 0x1cfefc0);
        ms_load_id = LoadReqLanguage(PL_MTOP_E_PK2, 0x1d10000);
        ms_load_id = LoadReqLanguage(PL_STTS_E_PK2, 0x1ce0000);
        ms_load_id = LoadReqLanguage(PL_PHOT_E_PK2, 0x1d51db0);
#else
        ms_load_id = LoadReq(PL_BGBG_PK2, 0x1d05140);
        ms_load_id = LoadReq(PL_MTOP_PK2, 0x1d15600);
        ms_load_id = LoadReq(PL_STTS_PK2, 0x1ce0000);
        ms_load_id = LoadReq(PL_PHOT_PK2, 0x1d573b0);
#endif
        StartAlbumModeInit();
        OutGameInitPhoto();
    break;
    case 10:
        motInitMsn();
#ifdef BUILD_EU_VERSION
        ms_load_id = LoadReq(PL_BGBG_PK2, 0x1cfefc0);
#else
        ms_load_id = LoadReq(PL_BGBG_PK2, 0x1d05140);
#endif
        BtlModSaveInit();
    break;
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
    
    if (dsp_ms.next_mode == 5)
    {
        title_wrk.mode = 26;
    }
    else if (dsp_ms.next_mode == 11)
    {
        outgame_wrk.mode = 4;
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
    OutGameModeChange(8);
}

void BattleModeExit()
{
    ChangeModeSel(2);
}

void BattleModeNext()
{
    ChangeModeSel(7);
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
    case 0:
        PutStringYW(0x31, dsp_ms.csr[0], 0x50, 0x165, 0x808080, alp, 0x1000, 0x0);
    break;
    case 1:
        PutStringYW(0x31, dsp_ms.csr[1] + 0x5, 0x50, 0x165, 0x808080, alp, 0x1000, 0x0);
    break;
    case 2:
        PutStringYW(0x31, dsp_ms.csr[1] + 0xa, 0x50, 0x165, 0x808080, alp, 0x1000, 0x0);
    break;
    }
}

void SetCsrInfoStoryMode()
{
    sm_csr_info[0] = 2;
    sm_csr_info[1] = 1;
    dsp_ms.csr[1] = 2;
    
    if (cribo.clear_info & 1)
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

#ifdef BUILD_EU_VERSION
void SetCsrInfoBattleMode()
{
    sm_csr_info[1] = 1;

    if (cribo.clear_info & 1)
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
