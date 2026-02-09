#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "item.h"

#include "graphics/graph2d/compress.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph2d/number.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph2d/tim2_new.h"
#include "graphics/graph3d/sglib.h"
#include "ingame/event/ev_main.h"
#include "ingame/ig_glob.h"
#include "ingame/info/inf_disp.h"
#include "ingame/map/item_ctl.h"
#include "ingame/menu/ig_file.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/item.h"
#include "ingame/menu/item_get.h"
#include "main/glob.h"
#include "os/eeiop/adpcm/ea_tape.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"

#ifdef BUILD_EU_VERSION
#include "graphics/graph2d/subtitles.h"
#endif

static void ItemUse(u_char item_no);
static int IngameMenuItemEventOpenJudge(u_char item_no);
static void ItemModeInOut();
static void ItemModeInOut2();
static void GageAnime(short int pos_x, short int pos_y, u_char alpha);
static void PutStsBar(u_char char_label, int rgb, u_char alp, float scl_x);
static void FilmAnime(short int pos_x, short int pos_y, short int alpha);
static void ItemList(short int pos_x, short int pos_y, short int alpha);
static void CameUp(u_char alp);
static void PlaySM(u_char alp);
static void ItemInTheLenz2D(short int pos_x, short int pos_y, short int alpha);
static void MessageWindow(short int pos_x, short int pos_y, short int alpha);
static void ItemCntInit();
static void ItemCntRenew();
static void GetItemNum();
static void WipeChange(u_char before_label, u_char after_label, float move_y);
static void PlusZanzo(u_short top_label, u_short end_label, float pos_x, float pos_y, int rgb, float alp, u_char num, u_char dry, u_char znz_no);
static void DigiPut01(u_char font, u_char num, u_char no, short int pos_x, short int pos_y, float sx, float sy, int rgb, short int alpha, int pri);
static void SttsRenew();
static void ItmTrFlsh(short int pos_x, short int pos_y, short int alpha);
static void YW_DBG();

#include "data/item_use_dat.h" // ITEM_USE_DAT item_use_dat[70];
short int item_sort[70] = {
     6, 66,  7,  8, 41, 42, 43, 44, 10, 16,
    37, 11, 25, 12, 26, 34, 39, 48, 13, 24,
    46, 47, 23, 15, 14, 40, 27, 49, 56, 57,
    58, 59, 17, 18, 60, 61, 19, 62, 63, 64,
    20, 65, 28, 29, 38, 45, 21, 22, 30, 31,
    32, 33, 36, 35, 50, 51, 52, 53, 54, 55,
     0, 67, 68, 69,  9,  5,  4,  3,  2,  1,
};
short int file2item[] = { 29, 45, 28, 38 };
char file2phot[] = { 0, 1, 2, 3 };
char pht_waku_put[] = { 1, 0, 1, 0 };
static u_short char_tex[7] = {0};
static u_short top_char[7] = {0};
static u_short end_char[7] = {0};
FOR_YW2D yw2d = {0};
INGAME_MENU_WRK ig_menu_wrk = {0};
MENU_ITEM_WRK menu_item_wrk[200] = {0};

static FLSH_CORE flsh[2];
static MSG_DSP msg;
static YWDBG ywdbg;
static FLSH_CORE dbg_flsh;

static ITM_EFF_WRK iew;

#define PI 3.1415927f

#ifdef BUILD_EU_VERSION
#define PL_BGBG_PK2_ADDRESS 0x1cfefc0
#else
#define PL_BGBG_PK2_ADDRESS 0x1d05140
#endif

#ifdef BUILD_EU_VERSION
#define LOAD_ITEM_2D_ADDR 0x1d51db0
#else
#define LOAD_ITEM_2D_ADDR 0x01d573b0
#endif

#ifdef BUILD_EU_VERSION
#define VRAM_ITEM_2D_ADDRESS 0x1d51db0
#else
#define VRAM_ITEM_2D_ADDRESS 0x1d573b0
#endif

void NewgameItemInit()
{
    memset(poss_item, 0, sizeof(poss_item));
    memset(poss_file, 0, sizeof(poss_file));
    memset(flm_exp_flg, 0, sizeof(flm_exp_flg));
    memset(item_dsp_wrk, 0, sizeof(item_dsp_wrk));

    iew = (ITM_EFF_WRK){0};
    msg = (MSG_DSP){0};
}

void NewgameItemInit2()
{
    if (ingame_wrk.clear_count != 0)
    {
        MissionSelectItemInit();
    }
    else
    {
        memset(poss_item, 0, sizeof(poss_item));
        memset(flm_exp_flg, 0, sizeof(flm_exp_flg));
    }

    memset(item_dsp_wrk, 0, sizeof(item_dsp_wrk));

    iew = (ITM_EFF_WRK){0};
    msg = (MSG_DSP){0};
}

void MissionSelectItemInit()
{
    int i;
    int j;
    int save_item[10] = {
        1, 2, 3, 4, 5, 6, 7, 8, 66, 9
    };
    int check_num;

    check_num = 10;

    for (i = 0; i < 200; i++)
    {
        for (j = 0; j < check_num; j++)
        {
            if (i == save_item[j])
            {
                break;
            }
        }

        if (j == check_num)
        {
            poss_item[i] = 0;
        }
    }
}

void MenuItemInit()
{
    memset(&item_dsp_wrk, 0, sizeof(item_dsp_wrk));

    iew = (ITM_EFF_WRK){0};
    msg = (MSG_DSP){0};

    FileDateInit();
}

void LoadgameMenuItemInit()
{
    memset(&item_dsp_wrk, 0, sizeof(item_dsp_wrk));

    iew = (ITM_EFF_WRK){0};
    msg = (MSG_DSP){0};
}

void StartItemModeInit()
{
    ig_menu_wrk.csr[2] = 0, ig_menu_wrk.csr[1] = 0;
    iew.itm_lst_strt = 0;
    iew.itm_can_dsp = 0xFF;
    iew.load_id = -1;

    yw2d.pad_lock = 1;

    iew.itm_dsp_flg = 0;
    iew.tex_now = 0;
    iew.tex_bak = 0;
    iew.alp_now = 0;
    iew.i_mode = 0;
    iew.msg_end = 0;
    iew.btn_flsh_cnt = 0;
    iew.cam_flr_tmr = 0;
    iew.tape_flsh = 0;

    tape_play = 0;
}

void ItemInitAtMenuOpen()
{
    iew.gage_hp = plyr_wrk.hp;
    iew.set_sub_flr = 0;
    iew.set_spe_flr = 0;
    iew.set_sub_bak = cam_custom_wrk.set_sub;
    iew.set_spe_bak = cam_custom_wrk.set_spe;
}

void FilmPossessionExp()
{
    int i;

    for (i = 0; i < 5; i++)
    {
        if (item_use_dat[i].type == 2 && poss_item[i] != 0)
        {
            flm_exp_flg[i] = 1;
        }
    }
}

void IngameMenuItem()
{
    IngameMenuItemDisp();

    if (ig_menu_wrk.item_num == 0)
    {
        if (yw2d.pad_lock == 0)
        {
            if (*key_now[4] == 1)
            {
                SeStartFix(3, 0, 0x1000, 0x1000, 1);

                yw2d.out_mode_cnt = 11;
            }
        }
    }
    else if (yw2d.pad_lock == 0)
    {
        if (*key_now[4] == 1)
        {
            if (IsLoadEndAll() != 0)
            {
                yw2d.out_mode_cnt = 0xb;

                SeStartFix(3, 0, 0x1000, 0x1000, 1);
            }
        }
        else if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            ig_menu_wrk.mode = IGMENU_MODE_ITEM_USE_SLCT;
            ig_menu_wrk.csr[2] = 0;

            iew.itm_eve_chk = 0;

            SttsRenew();

            if (
                item_use_dat[menu_item_wrk[ig_menu_wrk.csr[1]].item_no].type == 0 ||
                iew.hp_max_chk != 0 || iew.flm_dff_chk != 0
            )
            {
                SeStartFix(2, 0, 0x1000, 0x1000, 1);
            }
            else
            {
                SeStartFix(1, 0, 0x1000, 0x1000, 1);
            }

        }
        else if (
            *key_now[0] == 1 ||
            (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
            Ana2PadDirCnt(0) == 1 ||
            (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
        )
        {
            if (ig_menu_wrk.csr[1] != 0)
            {
                if (ig_menu_wrk.csr[1] == iew.itm_lst_strt)
                {
                    iew.itm_lst_strt--;
                }

                ig_menu_wrk.csr[1]--;
            }
            else
            {
                ig_menu_wrk.csr[1] = ig_menu_wrk.item_num - 1;

                if (ig_menu_wrk.item_num > 5)
                {
                    iew.itm_lst_strt = ig_menu_wrk.item_num - 6;
                }
                else
                {
                    iew.itm_lst_strt = 0;
                }
            }

            SeStartFix(0, 0, 0x1000, 0x1000, 1);
        }
        else if (
            *key_now[1] == 1 ||
            (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
            Ana2PadDirCnt(2) == 1 ||
            (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
        )
        {
            if (ig_menu_wrk.csr[1] < ig_menu_wrk.item_num - 1)
            {
                if (ig_menu_wrk.csr[1] == iew.itm_lst_strt + 5)
                {
                    iew.itm_lst_strt++;
                }

                ig_menu_wrk.csr[1]++;
            }
            else
            {
                ig_menu_wrk.csr[1] = 0;

                iew.itm_lst_strt = 0;
            }

            SeStartFix(0, 0, 0x1000, 0x1000, 1);
        }
    }
}

void IngameMenuItemUseSlct()
{
    IngameMenuItemDisp();

    if (tape_play != 0)
    {
#ifdef BUILD_EU_VERSION
        SetSubtitlesNCnt(1, tape_play);
#endif

        if (play_tape_timer != 0)
        {
            play_tape_timer--;

            if (play_tape_timer != 0)
            {
                return;
            }
        }

        if (IsEndAdpcmTape() != 0)
        {
#ifdef BUILD_EU_VERSION
            tape_play = 0;

            FinishSubtitlesSys();

            ig_menu_wrk.mode = IGMENU_MODE_ITEM;
#else
            ig_menu_wrk.mode = IGMENU_MODE_ITEM;

            tape_play = 0;
#endif

            return;
        }

        if (*key_now[5] == 1)
        {
            AdpcmStopTape(5);

            tape_play = 0;

#ifdef BUILD_EU_VERSION
            FinishSubtitlesSys();
#endif

            ig_menu_wrk.mode = IGMENU_MODE_ITEM;

            SeStartFix(1, 0, 0x1000, 0x1000, 1);
        }

        return;
    }

    if (
        item_use_dat[(menu_item_wrk + ig_menu_wrk.csr[1])->item_no].type == 0 ||
        ig_menu_wrk.mode == IGMENU_MODE_ITEM_USE_END && iew.msg_end != 0 ||
        (iew.hp_max_chk != 0 || iew.flm_dff_chk != 0 || iew.itm_eve_chk != 0)
    )
    {
        if (ig_menu_wrk.csr[1] > ig_menu_wrk.item_num - 1)
        {
            if (yw2d.pad_lock == 0)
            {
                if (*key_now[0] == 1 || Ana2PadDirCnt(0) == 1)
                {
                    if (ig_menu_wrk.csr[1] != 0)
                    {
                        if (ig_menu_wrk.csr[1] == iew.itm_lst_strt)
                        {
                            iew.itm_lst_strt--;
                        }

                        ig_menu_wrk.csr[1]--;
                    }
                    else
                    {
                        ig_menu_wrk.csr[1] = ig_menu_wrk.item_num - 1;

                        if (ig_menu_wrk.item_num > 5)
                        {
                            iew.itm_lst_strt = ig_menu_wrk.item_num - 6;
                        }
                        else
                        {
                            iew.itm_lst_strt = 0;
                        }
                    }

                    ig_menu_wrk.mode = IGMENU_MODE_ITEM;

                    SeStartFix(0, 0, 0x1000, 0x1000, 1);
                }
                else if (*key_now[1] == 1 || Ana2PadDirCnt(2) == 1)
                {
                    if (ig_menu_wrk.csr[1] < ig_menu_wrk.item_num - 1)
                    {
                        if (ig_menu_wrk.csr[1] == iew.itm_lst_strt + 5)
                        {
                            iew.itm_lst_strt++;
                        }

                        ig_menu_wrk.csr[1]++;
                    }
                    else
                    {
                        ig_menu_wrk.csr[1] = 0;

                        iew.itm_lst_strt = 0;
                    }

                    ig_menu_wrk.mode = IGMENU_MODE_ITEM;

                    SeStartFix(0, 0, 0x1000, 0x1000, 1);
                }
            }
        }
        else if (yw2d.pad_lock == 0)
        {
            if (*key_now[4] == 1)
            {
                ig_menu_wrk.mode = IGMENU_MODE_ITEM;

                SeStartFix(0, 0, 0x1000, 0x1000, 1);
            }
            else if (*key_now[6] == 1 || *key_now[5] == 1)
            {
                ig_menu_wrk.mode = IGMENU_MODE_ITEM;

                SeStartFix(0, 0, 0x1000, 0x1000, 1);
            }
            else if (
                *key_now[0] == 1 ||
                (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
                Ana2PadDirCnt(0) == 1 ||
                (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
            )
            {
                if (ig_menu_wrk.csr[1] != 0)
                {
                    if (ig_menu_wrk.csr[1] == iew.itm_lst_strt)
                    {
                        iew.itm_lst_strt--;
                    }

                    ig_menu_wrk.csr[1]--;
                }
                else
                {
                    ig_menu_wrk.csr[1] = ig_menu_wrk.item_num - 1;

                    if (ig_menu_wrk.item_num > 5)
                    {
                        iew.itm_lst_strt = ig_menu_wrk.item_num - 6;
                    }
                    else
                    {
                        iew.itm_lst_strt = 0;
                    }
                }

                ig_menu_wrk.mode = IGMENU_MODE_ITEM;

                SeStartFix(0, 0, 0x1000, 0x1000, 1);
            }
            else if (
                *key_now[1] == 1 ||
                (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
                Ana2PadDirCnt(2) == 1 ||
                (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
            )
            {
                if (ig_menu_wrk.csr[1] < ig_menu_wrk.item_num - 1)
                {
                    if (ig_menu_wrk.csr[1] == iew.itm_lst_strt + 5)
                    {
                        iew.itm_lst_strt += 1;
                    }

                    ig_menu_wrk.csr[1] += 1;
                }
                else
                {
                    ig_menu_wrk.csr[1] = 0;

                    iew.itm_lst_strt = 0;
                }

                ig_menu_wrk.mode = IGMENU_MODE_ITEM;

                SeStartFix(0, 0, 0x1000, 0x1000, 1);
            }
        }

        return;
    }

    if (ig_menu_wrk.mode == IGMENU_MODE_ITEM_USE_END && iew.msg_end == 0)
    {
        return;
    }

    if (yw2d.pad_lock != 0)
    {
        return;
    }

    if (*key_now[4] == 1)
    {
        ig_menu_wrk.mode = IGMENU_MODE_ITEM;

        SeStartFix(3, 0, 0x1000, 0x1000, 1);
    }
    else if (*key_now[6] == 1 || *key_now[5] == 1)
    {
        if (ig_menu_wrk.csr[2] != 0)
        {
            ig_menu_wrk.mode = IGMENU_MODE_ITEM;
        }
        else
        {
            if (item_use_dat[(menu_item_wrk + ig_menu_wrk.csr[1])->item_no].type == 1)
            {
                if (IngameMenuItemEventOpenJudge(menu_item_wrk[ig_menu_wrk.csr[1]].item_no) != 0)
                {
                    iew.itm_eve_chk = 0;
                }
                else
                {
                    iew.itm_eve_chk = 1;
                }
            }
            else
            {
                ItemUse(menu_item_wrk[ig_menu_wrk.csr[1]].item_no);

                ig_menu_wrk.use_item = menu_item_wrk[ig_menu_wrk.csr[1]].item_no;
                ig_menu_wrk.mode = IGMENU_MODE_ITEM_USE_END;

                RstMessageYW();
            }
        }

        if (ig_menu_wrk.csr[2] != 0)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 1);
        }
        else if (iew.itm_eve_chk != 0)
        {
            SeStartFix(2, 0, 0x1000, 0x1000, 1);
        }
    }
    else if (*key_now[3] == 1 || Ana2PadDirCnt(1) == 1)
    {
        ig_menu_wrk.csr[2] = 1 - ig_menu_wrk.csr[2];

        SeStartFix(0, 0, 0x1000, 0x1000, 1);

    }
    else if (*key_now[2] == 1 || Ana2PadDirCnt(3) == 1)
    {
        ig_menu_wrk.csr[2] = 1 - ig_menu_wrk.csr[2];

        SeStartFix(0, 0, 0x1000, 0x1000, 1);
    }

    return;
}

void IngameMenuItemDisp()
{
    ItemCntInit();
    ItemModeInOut();
    DspMenuTitle(yw2d.io_x[1], yw2d.io_y[1], yw2d.io_a[1], 50, 2);
    PlayerStatusYW(yw2d.io_a[2]);
    GetItemNum();

    if (ig_menu_wrk.item_num == 0)
    {
        DrawMessageBox(0x64000, 170.0f, 210.0f, 300.0f, 60.0f, yw2d.io_a[4]);
        PutStringYW(0x19, 6, 0xc3, 0xe4, 0x808080, yw2d.io_a[4], 0x1000, 0);
    }

    else
    {
        ItemList(yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);
        ItemInTheLenz2D(yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);
        MessageWindow( yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);
    }

    DispCaption(19, yw2d.io_a[4]);
    ItemModeInOut2();
    ItemCntRenew();

}

static void ItemUse(u_char item_no)
{
    int adpcm_no;

    switch (item_use_dat[item_no].type)
    {
    case 0:
        // do nothing ...
    break;
    case 1:
        // do nothing ...
    break;
    case 2:
        plyr_wrk.film_no = item_use_dat[item_no].value0;

        SeStartFix(5, 0, 0x1000, 0x1000, 1);
    break;
    case 3:
        plyr_wrk.hp += item_use_dat[item_no].value1;

        if (plyr_wrk.hp > 500)
        {
            plyr_wrk.hp = 500;
        }

        if (item_use_dat[item_no].value0 != 0)
        {
            plyr_wrk.cond = 0;
            plyr_wrk.cond_tm = 0;
        }

        poss_item[item_no]--;

        SeStartFix(6, 0, 0x1000, 0x1000, 1);
    break;
    case 4:
        // do nothing ...
    break;
    case 5:
        adpcm_no = CheckTape(item_no);

        if (adpcm_no != -1)
        {
#ifdef BUILD_EU_VERSION
            InitSubtitlesSys();
#endif

            PlayTape(adpcm_no);

#ifdef BUILD_EU_VERSION
            tape_play = item_no;
#endif
        }
    break;
    }
}

static int IngameMenuItemEventOpenJudge(u_char item_no)
{
    ev_wrk.use_item = item_no;

    if (item_use_dat[item_no].type == 1)
    {
        if (EventOpenCtrl() != 0)
        {
            ingame_wrk.mode = INGAME_MODE_EVENT;
            ingame_wrk.stts &= ~0x20;

            ev_wrk.mode = 1;

            return 1;
        }
    }

    ev_wrk.use_item = 0xff;

    return 0;
}

static void ItemModeInOut()
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
        yw2d.io_a[2] = 128.0f;
        yw2d.io_a[3] = fade_out;
        yw2d.io_a[4] = fade_in;

        DspTopMenu(yw2d.io_x[3], yw2d.io_y[3], yw2d.io_a[3], 0);
        DspPlayData(yw2d.io_x[3], yw2d.io_y[3], yw2d.io_a[3], 1);

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
        yw2d.io_a[2] = 128.0f;
        yw2d.io_a[3] = fade_in;
        yw2d.io_a[4] = fade_out;

        DspTopMenu(yw2d.io_x[3], yw2d.io_y[3], yw2d.io_a[3], 0);
        DspPlayData(yw2d.io_x[3], yw2d.io_y[3], yw2d.io_a[3], 1);

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

static void ItemModeInOut2()
{
    if (yw2d.inn_mode_cnt < 11)
    {
        DspPlayData(yw2d.io_x[3], yw2d.io_y[3], yw2d.io_a[3], 2);
    }
    else if (yw2d.out_mode_cnt != 0)
    {
        DspPlayData(yw2d.io_x[3], yw2d.io_y[3], yw2d.io_a[3], 2);
    }
}

void PlayerStatusYW(u_char alpha)
{
    u_char bak_alp;
    u_char tmp_alp;
    u_char flr_alp;

    PutSpriteYW(209, 209, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(210, 210, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);


    if (ingame_wrk.game == 0 && ingame_wrk.msn_no == 0)
    {
        PutSpriteYW(239, 239, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }
    else
    {
        PutSpriteYW(211, 211, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }

    GageAnime(0, 0, alpha);
    FilmAnime(0, 0, alpha);

    PutSpriteYW(240, 240, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 0.9f, 0.9f, 0, 0xff, 1, 0, 0);

    PutStringYW(45, 0, 453, 59, 0x808080, alpha, 0x5000, 0);

    PutNumberYW(3, poss_item[5], 463, 0x37, 1.0f, 1.0f, 0x808080, alpha, 0x5000, 2, 1);

#ifdef BUILD_EU_VERSION
    PutNumberYW(3, cam_custom_wrk. point, 491, 27, 1.0f, 1.0f, 0x808080, alpha, 0x5000, 7, 0);
#else
    PutNumberYW(3, cam_custom_wrk. point, 491, 28, 1.0f, 1.0f, 0x808080, alpha, 0x5000, 7, 0);
#endif

    if (iew.set_sub_bak != cam_custom_wrk.set_sub)
    {
        if (iew.set_sub_flr == 0)
        {
            iew.set_sub_flr = 30;
            iew.set_sub_tmp = iew.set_sub_bak;
            iew.set_sub_bak = cam_custom_wrk.set_sub;
        }
    }
    else if (iew.set_sub_flr > 20) {
            tmp_alp = (alpha * (iew.set_sub_flr - 10)) / 10.0f;

            bak_alp = alpha - tmp_alp;

            iew.set_sub_flr--;

            if (iew.set_sub_tmp != 0xff)
            {
                PutSpriteYW(iew.set_sub_tmp + 218, iew.set_sub_tmp + 218, 0.0f, 0.0f, 0.0f, 0x808080, tmp_alp, 1.0f, 1.0f, 0, 255, 1, 0, 0);
            }

            if (iew.set_sub_bak != 0xff)
            {
                PutSpriteYW(iew.set_sub_bak + 218, iew.set_sub_bak + 218, 0.0f, 0.0f, 0.0f, 0x808080, bak_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }
    }
    else if (iew.set_sub_flr != 0)
    {
        bak_alp = (alpha * iew.set_sub_flr) / 20.0f;

        iew.set_sub_flr--;

        if (iew.set_sub_bak != 0xff)
        {
            PutSpriteYW(iew.set_sub_bak + 218, iew.set_sub_bak + 218, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(iew.set_sub_bak + 218, iew.set_sub_bak + 218, 0.0f, 0.0f, 0.0f, 0x808080, bak_alp, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
        }
    }
    else if (iew.set_sub_bak != 0xff)
    {
        PutSpriteYW(iew.set_sub_bak + 218, iew.set_sub_bak + 218, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }

    if (iew.set_spe_bak != cam_custom_wrk.set_spe)
    {
        if (iew.set_spe_flr == 0)
        {
            iew.set_spe_flr = 30;
            iew.set_spe_tmp = iew.set_spe_bak;
            iew.set_spe_bak = cam_custom_wrk.set_spe;
        }
    }
    else if (iew.set_spe_flr > 20)
    {
        tmp_alp = (alpha * (iew.set_spe_flr - 10)) / 10.0f;

        bak_alp = alpha - tmp_alp;

        iew.set_spe_flr--;

        if (iew.set_spe_tmp != 0xff)
        {
            PutSpriteYW(iew.set_spe_tmp + 223, iew.set_spe_tmp + 223, 0.0f, 0.0f, 0.0f, 0x808080, tmp_alp, 1.0f, 1.0f, 0, 255, 1, 0, 0);
        }

        if (iew.set_spe_bak != 0xff)
        {
            PutSpriteYW(iew.set_spe_bak + 223, iew.set_spe_bak + 223, 0.0f, 0.0f, 0.0f, 0x808080, bak_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }
    }
    else if (iew.set_spe_flr != 0)
    {
        bak_alp = (alpha * iew.set_spe_flr) / 20.0f;

        iew.set_spe_flr--;

        if (iew.set_spe_bak != 0xff)
        {
            PutSpriteYW(iew.set_spe_bak + 223, iew.set_spe_bak + 223, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(iew.set_spe_bak + 223, iew.set_spe_bak + 223, 0.0f, 0.0f, 0.0f, 0x808080, bak_alp, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
        }

    }
    else if (iew.set_spe_bak != 0xff)
    {
        PutSpriteYW(iew.set_spe_bak + 223, iew.set_spe_bak + 223, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }
}

static void GageAnime(short int pos_x, short int pos_y, u_char alpha)
{
    if ((iew.gage_hp + 2) < plyr_wrk.hp)
    {
        iew.gage_hp += 2;
    }
    else
    {
        iew.gage_hp = plyr_wrk.hp;
    }
    PutStsBar(0xE5U, 0x808080, alpha, plyr_wrk.hp / 500.0f);
    PutStsBar(0xE8U, 0x808080, alpha, iew.gage_hp / 500.0f);
}

static void PutStsBar(u_char char_label, int rgb, u_char alp, float scl_x)
{
    int i;
    DISP_SPRT ds;

    for (i = 0; i < 3; i++)
    {
        CopySprDToSpr(&ds, &spr_dat[i + char_label]);

        ds.csx = spr_dat[char_label].x;
        ds.csy = spr_dat[char_label].y;
        ds.scw = scl_x;
        ds.sch = 1.0f;
        ds.alpha = alp;
        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

        DispSprD(&ds);
    }
}

static void FilmAnime(short int pos_x, short int pos_y, short int alpha)
{
    int i;

    for (i = 0; i < 5; i++)
    {
        if (i == plyr_wrk.film_no)
        {
            if (iew.flm_alp[i] + 8 < alpha)
            {
                iew.flm_alp[i] += 8;
            }
            else
            {
                iew.flm_alp[i] = alpha;
            }

        }
        else if (iew.flm_alp[i] - 8 > 0)
        {
            iew.flm_alp[i] -= 8;
        }
        else
        {
            iew.flm_alp[i] = 0;
        }
    }

    PutSpriteYW(212, 212, pos_x, pos_y, 0.0f, 0x808080, iew.flm_alp[0], 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(213, 213, pos_x, pos_y, 0.0f, 0x808080, iew.flm_alp[1], 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(214, 214, pos_x, pos_y, 0.0f, 0x808080, iew.flm_alp[2], 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(215, 215, pos_x, pos_y, 0.0f, 0x808080, iew.flm_alp[3], 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(216, 216, pos_x, pos_y, 0.0f, 0x808080, iew.flm_alp[4], 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    PutStringYW(45, 0, 347, 59, 0x808080, alpha, 0x5000, 0);

    PutNumberYW(3, poss_item[plyr_wrk.film_no], 358, 55, 1.0f, 1.0f, 0x808080, alpha, 0x5000, 2, 1);
}

static void ItemList(short int pos_x, short int pos_y, short int alpha)
{
    int i;
    float top_bar;
    float mdl_bar;
    float dwn_bar;
    u_short alp;
    int y_off_1 = 114;
    int y_off_2 = 115;
    int y_off_3 = 107;

    CmnWindow(3, pos_x, pos_y, alpha, 0x80);

    PutSpriteYW(ITM_SCRBAR_BASE_U, ITM_SCRBAR_BASE_U, pos_x, top_bar + pos_y, 0.0f, 0x808080, alpha, 1.0f,  1.0f, 0x0, 0xff, 1, 0, 0);
    PutSpriteYW(ITM_SCRBAR_BASE_M, ITM_SCRBAR_BASE_M, pos_x, mdl_bar + pos_y, 0.0f, 0x808080, alpha, 1.0f, 19.8f, 0x0, 0xff, 1, 0, 0);
    PutSpriteYW(ITM_SCRBAR_BASE_D, ITM_SCRBAR_BASE_D, pos_x, dwn_bar + pos_y, 0.0f, 0x808080, alpha, 1.0f,  1.0f, 0x0, 0xff, 1, 0, 0);

    SideBar(ig_menu_wrk.item_num, 6, 214, iew.itm_lst_strt, 284, alpha, 0, 0);

    ItmTrFlsh(pos_x, pos_y, alpha);

    if (ig_menu_wrk.item_num - 1 < ig_menu_wrk.csr[1])
    {
        ig_menu_wrk.csr[1] = ig_menu_wrk.item_num - 1;

        if (iew.itm_lst_strt != 0) {
            iew.itm_lst_strt--;
        }
    }
    else if (ig_menu_wrk.item_num - iew.itm_lst_strt < 6)
    {
        if (iew.itm_lst_strt != 0)
        {
            iew.itm_lst_strt--;
        }
    }

    for (i = 0; i < 6; i++)
    {
        if (menu_item_wrk[i + iew.itm_lst_strt].item_no == 0xff)
        {
            break;
        }

        alp = alpha;

        PutStringYW(24, menu_item_wrk[i + iew.itm_lst_strt].item_no,  pos_x +  52, i * 34 + (pos_y + y_off_1), 0x808080, alp, 0xf000, 0);
        PutNumberYW( 0, menu_item_wrk[i + iew.itm_lst_strt].item_num, pos_x + 274, i * 34 + (pos_y + y_off_2), 1.0f, 1.0f, 0x808080, alp, 0xf000, 2, 0);
    }

    FlashStarYW(flsh, 96, 64, 90, 0);

    CmnCursol(pos_x + 44, (ig_menu_wrk.csr[1] - iew.itm_lst_strt) * 34 + (pos_y + y_off_3), 274, 36, flsh[0].alpha, alpha, 0x14000);
}

static void CameUp(u_char alp)
{
    u_char alpha;

    if (iew.itm_csr_now != 9)
    {
        iew.cam_flr_tmr = 0;
    }
    else if (iew.alp_now != alp)
    {
        iew.cam_flr_tmr = 0;
    }
    else if (iew.cam_flr_tmr < 120)
    {
        iew.cam_flr_tmr++;
    }
    else
    {
        iew.cam_flr_tmr = 0;
    }

    alpha = alp * SgSinf((iew.cam_flr_tmr * PI) / 120.0f);

    PutSpriteYW(ITM_CAM_FLR, ITM_CAM_FLR, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
}

static void PlaySM(u_char alp)
{
    u_char alpha;

    if (tape_play == 0)
    {
        return;
    }

    if (iew.tape_flsh < 90)
    {
        iew.tape_flsh++;
    }
    else
    {
        iew.tape_flsh = 0;
    }

    alpha = alp * SgSinf((iew.tape_flsh * PI) / 90.0f);

#ifdef BUILD_EU_VERSION
    SetSprFile(0x1e2f000);

    PutSpriteYW(PLAYMATE, PLAYMATE, 130.0f, 104.0f, 0.0f, 0x303030, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PLAYMATE, PLAYMATE, 130.0f, 104.0f, 0.0f, 0x606060, (int)(alpha / 2), 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
#else
    SetSprFile(0x1e2f700);

    PutSpriteYW(PLAYMATE, PLAYMATE, 100.0f, 180.0f, 0.0f, 0x303030, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PLAYMATE, PLAYMATE, 100.0f, 180.0f, 0.0f, 0x606060, (int)(alpha / 2), 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
#endif
}

static void ItemInTheLenz2D(short int pos_x, short int pos_y, short int alpha)
{
    u_short tmp_alpha;
    u_short alpha_diff;

    if (ig_menu_wrk.item_num != 0)
    {
        iew.itm_csr_now = menu_item_wrk[ig_menu_wrk.csr[1]].item_no;

        if (iew.itm_csr_now != iew.itm_can_dsp)
        {
            if (iew.load_id == -1) {
                iew.itm_loading = iew.itm_csr_now;
                iew.load_id = LoadItem2D(4, iew.itm_loading);
                iew.itm_dsp_flg = 1;
            }
        }
        else if (iew.alp_now == alpha && iew.itm_dsp_flg != 0)
        {
            iew.tex_bak = iew.tex_now;
            iew.tex_now = VramItem2D(iew.i_mode, 1);
            iew.i_mode = 1 - iew.i_mode;
            iew.alp_now = 0;
            iew.itm_dsp_flg = 0;
        }

        if (iew.load_id != -1)
        {
            if (IsLoadEnd(iew.load_id) != 0)
            {
                iew.itm_can_dsp = iew.itm_loading;
                iew.load_id = -1;
            }
        }

        tmp_alpha = alpha / 30.0f;

        if (iew.alp_now + tmp_alpha < alpha)
        {
            iew.alp_now += tmp_alpha;
        }
        else
        {
            iew.alp_now = alpha;
        }

        alpha_diff = alpha - iew.alp_now;

        DspItem2D(iew.tex_now, pos_x, pos_y, 0x808080, iew.alp_now, 1.0f, 1.0f, 0x32);
        DspItem2D(iew.tex_bak, pos_x, pos_y, 0x808080,  alpha_diff, 1.0f, 1.0f, 0x32);

#ifdef BUILD_EU_VERSION
#else
        PlaySM(alpha);
#endif
    }

    PutSpriteYW(ITM_DSP_LENS1, ITM_DSP_LENS3, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
}

int LoadItem2D(u_char get_type, u_char get_no)
{
    int load_id;

    if (get_type != 0xff && get_no != 0xff)
    {
        switch(get_type)
        {
        case 0:
            load_id = LoadReq(get_no + ITEM_00_TM2, LOAD_ITEM_2D_ADDR);
        break;
        case 1:
            load_id = LoadReq(get_no + ITEM_00_TM2, LOAD_ITEM_2D_ADDR);
        break;
        case 2:
            load_id = LoadReq(get_no + ITEM_00_TM2, LOAD_ITEM_2D_ADDR);
        break;
        case 3:
            load_id = LoadReq(get_no + HINT_00_TM2, LOAD_ITEM_2D_ADDR);
        break;
        case 4:
            load_id = LoadReq(get_no + ITEM_00_TM2, LOAD_ITEM_2D_ADDR);
        break;
        }
    }
    else
    {
        return 0;
    }

    return load_id;
}

u_long VramItem2D(u_char mode, u_char type)
{
    u_int offset;
    SPRITE_DATA dmy;

    if (type == 0)
    {
        offset = 145 * 64;

        if (mode == 0)
        {
            offset = 140 * 64;
        }
    }
    else if (type == 1)
    {
        offset = 110 * 64;

        if (mode == 0)
        {
            offset = 105 * 64;
        }
    }
    else if (type == 2)
    {
        offset = 194 * 64;

        if (mode == 0)
        {
            offset = 189 * 64;
        }
    }
    else if (type == 3)
    {
        offset = 145 * 64;

        if (mode == 0)
        {
            offset = 140 * 64;
        }
    }

    MakeTim2SendPacket(VRAM_ITEM_2D_ADDRESS, offset);

    return GetTex0RegTM(VRAM_ITEM_2D_ADDRESS, offset);
}

char DspItem2D(u_long tex_addr, float pos_x, float pos_y, int rgb, float alp, float scl_x, float scl_y, char pri)
{
    float scl_px;
    float scl_py;
    DISP_SPRT ds;

    if (tex_addr == 0)
    {
        return 1;
    }

    CopySprDToSpr(&ds, &spr_dat[287]);

    ds.tex0 = tex_addr;

    ds.x = ds.x + pos_x;
    ds.y = ds.y + pos_y;

    ds.alpha = alp;

    ds.r = (u_int)rgb >> 16;
    ds.g = (u_int)rgb >>  8;
    ds.b = (u_int)rgb >>  0;

    scl_px = ds.x + (ds.w >> 1);
    scl_py = ds.y + (ds.h >> 1);

    ds.scw = scl_x;
    ds.sch = scl_y;

    ds.csx = scl_px;
    ds.csy = scl_py;

    ds.pri = (u_char)pri * 0x1000;
    ds.z = 0x0fffffff - ds.pri;

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);

    return 0;
}

void PkTm2(u_long tex_addr, short int pos_x, short int pos_y, short int w, short int h, u_char alp, int pri)
{
    DISP_SPRT ds;

    if (tex_addr == 0)
    {
        return;
    }

    CopySprDToSpr(&ds, &spr_dat[287]);

    ds.tex0 = tex_addr;

    ds.u = 1;
    ds.v = 1;

    ds.w = w - 2;
    ds.h = h - 2;

    ds.x = pos_x;
    ds.y = pos_y;

    ds.pri = pri;
    ds.z = 0x0fffffff - pri;

    ds.alpha = alp;

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);
}

static void MessageWindow(short int pos_x, short int pos_y, short int alpha)
{
    float btn_alp;

#ifdef BUILD_EU_VERSION
    if (tape_play != 0)
    {
        DrawMessageBox(0x19000, pos_x + 26, 332.0f, 588.0f, 96.0f, alpha);
    }
    else
    {
        DrawMessageBox(0x19000, pos_x + 26, pos_y + 0x154, 588.0f, 80.0f, alpha);
    }
#else
    DrawMessageBox(0x19000, pos_x + 26, pos_y + 340, 588.0f, 80.0f, alpha);
#endif

    if (ig_menu_wrk.item_num == 0)
    {
        return;
    }

    if (ig_menu_wrk.mode == 1)
    {
        PutStringYW(22, menu_item_wrk[ig_menu_wrk.csr[1]].item_no, pos_x + 48, pos_y + 356, 0x808080, alpha, 0x14000, 0);
    }
    else if (ig_menu_wrk.mode == 8)
    {
        if (item_use_dat[menu_item_wrk[ig_menu_wrk.csr[1]].item_no].type == 0)
        {
            PutStringYW(25, 2, pos_x + 48, pos_y + 356, 0x808080, alpha, 0xf000, 0);
        }
        else if (iew.hp_max_chk != 0)
        {
            PutStringYW(25, 4, pos_x + 48, pos_y + 356, 0x808080, alpha, 0xf000, 0);
        }
        else if (iew.flm_dff_chk != 0)
        {
            PutStringYW(25, 5, pos_x + 48, pos_y + 356, 0x808080, alpha, 0xf000, 0);
        }
        else if (iew.itm_eve_chk != 0)
        {
            PutStringYW(25, 2, pos_x + 48, pos_y + 356, 0x808080, alpha, 0xf000, 0);
        }
        else if (item_use_dat[menu_item_wrk[ig_menu_wrk.csr[1]].item_no].type == 2)
        {
            PutStringYW(25, 3, pos_x + 48, pos_y + 356, 0x808080, alpha, 0xf000, 0);
            PutStringYW(25, 1, pos_x + 48, pos_y + 382, 0x808080, alpha, 0xf000, 0);

#ifdef BUILD_EU_VERSION
            YesNoCrslOKR(0x14000, ig_menu_wrk.csr[2] * 132 + 178, 380.0f, 0x808080, 128.0f, 2.0f);
#else
            YesNoCrslOKR(0x14000, ig_menu_wrk.csr[2] * 132 + 178, 378.0f, 0x808080, 128.0f, 2.0f);
#endif
        }
        else
        {
            PutStringYW(25, 0, pos_x + 48, pos_y + 356, 0x808080, alpha, 0xf000, 0);
            PutStringYW(25, 1, pos_x + 48, pos_y + 382, 0x808080, alpha, 0xf000, 0);

#ifdef BUILD_EU_VERSION
            YesNoCrslOKR(0x14000, ig_menu_wrk.csr[2] * 132 + 178, 380.0f, 0x808080, 128.0f, 2.0f);
#else
            YesNoCrslOKR(0x14000, ig_menu_wrk.csr[2] * 132 + 178, 378.0f, 0x808080, 128.0f, 2.0f);
#endif
        }
    }
    else if (ig_menu_wrk.mode == 9)
    {
#ifdef BUILD_EU_VERSION
        if (tape_play == 0)
        {
            iew.msg_end = PutMessageYW(23, ig_menu_wrk.use_item, 48, 356, 0x808080, 0x80, 0xf000);

            btn_alp = (SgSinf((iew.btn_flsh_cnt * (PI * 2)) / 60.0f) + 1.0f) * 128.0f * 0.5f;

            DrawButtonTex(0xf000, 3, 552.0f, 380.0f, btn_alp);
        }
        else
        {
            PlaySM(alpha);
        }
#else
        iew.msg_end = PutMessageYW(23, ig_menu_wrk.use_item, 48, 356, 0x808080, 0x80, 0xf000);

        btn_alp = (SgSinf((iew.btn_flsh_cnt * (PI * 2)) / 60.0f) + 1.0f) * 128.0f * 0.5f;

        if (tape_play == 0)
        {
            DrawButtonTex(0xf000, 3, 552.0f, 380.0f, btn_alp);
        }
#endif
    }
}

static void ItemCntInit()
{
    return;
}

static void ItemCntRenew()
{
    if (iew.btn_flsh_cnt < 60)
    {
        iew.btn_flsh_cnt++;
    }
    else
    {
        iew.btn_flsh_cnt = 0x0;
    }

    if (yw2d.inn_mode_cnt < 0xffff)
    {
        yw2d.inn_mode_cnt++;
    }

    if (yw2d.out_mode_cnt != 0)
    {
        yw2d.out_mode_cnt--;
    }
}

static void GetItemNum()
{
    int i;
    int chk;

    ig_menu_wrk.item_num = 0;

    for (i = 0; i < 70; i++)
    {
        menu_item_wrk[i].item_no = 0xff;
    }

    for (i = 0; i < 70; i++)
    {
        chk = item_sort[i];

        if (poss_item[chk] != 0)
        {
            menu_item_wrk[ig_menu_wrk.item_num].item_no = chk;
            menu_item_wrk[ig_menu_wrk.item_num].item_num = poss_item[chk];

            ig_menu_wrk.item_num++;
        }
    }
}

void PutSpriteYW(u_short top_label, u_short end_label, float pos_x, float pos_y, float rot, int rgb, float alp, float scl_x, float scl_y, u_char scl_mode, int pri, u_char by, u_char blnd, u_char z_sw)
{
    int i;
    float rot_px;
    float rot_py;
    float scl_px;
    float scl_py;
    DISP_SPRT ds;
    u_char r;
    u_char g;
    u_char b;

    for (i = top_label; i <= end_label; i++)
    {
        CopySprDToSpr(&ds,spr_dat + i);

        ds.x = ds.x + pos_x;
        ds.y = ds.y + pos_y;

        if (blnd != 0)
        {
            ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
        }
        else
        {
            ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
        }

        ds.alpha = alp;

        rot_px = ds.x + (ds.w >> 1);
        rot_py = ds.y + (ds.h >> 1);

        ds.rot = rot;
        ds.crx = rot_px;
        ds.cry = rot_py;

        r = ((u_int)rgb & 0xff0000) >> 16;
        g = ((u_int)rgb & 0x00ff00) >>  8;
        b = ((u_int)rgb & 0x0000ff) >>  0;

        ds.r = r; ds.g = g; ds.b = b;

        if (scl_mode == 0)
        {
            scl_px = ds.x + (ds.w >> 1);
            scl_py = ds.y + (ds.h >> 1);
        }
        else
        {
            scl_px = spr_dat[top_label].x;
            scl_py = spr_dat[top_label].y;
        }

        ds.csx = scl_px;
        ds.csy = scl_py;

        ds.scw = scl_x;
        ds.sch = scl_y;

        if (flip_table[i] == 1)
        {
            ds.att |= 0x1;
        }

        if (flip_table[i] == 2)
        {
            ds.att |= 0x2;
        }

        if (flip_table[i] == (0x1 | 0x2))
        {
            ds.att |= 0x1 | 0x2;
        }

        if (pri != 0xff)
        {
            ds.z = 0x0fffffff - pri;
            ds.pri = pri;
        }

        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, by, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

        if (z_sw != 0)
        {
            ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
        }

        DispSprD(&ds);
    }
}

static void WipeChange(u_char before_label, u_char after_label, float move_y)
{
    DISP_SPRT ds;

    CopySprDToSpr(&ds, &spr_dat[before_label]);

    ds.y = ds.y + move_y;
    ds.h = ds.h - move_y;

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);
    CopySprDToSpr(&ds, &spr_dat[after_label]);

    ds.v = (ds.v + ds.h) - move_y;
    ds.h = move_y;

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);
}

static void PlusZanzo(u_short top_label, u_short end_label, float pos_x, float pos_y, int rgb, float alp, u_char num, u_char dry, u_char znz_no)
{
    int i;
    u_short znz_same;
    float alpha;

    if (yw2d.znz_tmr[znz_no] == 0)
    {
        for (i = 0; i < num + 1; i++)
        {
            yw2d.pos_x[znz_no][i] = pos_x;
            yw2d.pos_y[znz_no][i] = pos_y;
        }
    }

    if (yw2d.znz_tmr[znz_no] % dry == 0)
    {
        for (i = num; i >= 1; i--)
        {
            yw2d.pos_x[znz_no][i] = yw2d.pos_x[znz_no][i - 1];
            yw2d.pos_y[znz_no][i] = yw2d.pos_y[znz_no][i - 1];
        }

        yw2d.pos_x[znz_no][0] = pos_x;
        yw2d.pos_y[znz_no][0] = pos_y;
    }

    znz_same = 0;

    for (i = 1; i < num + 1; i++)
    {
        if (yw2d.pos_x[znz_no][i] != yw2d.pos_x[znz_no][0] || yw2d.pos_y[znz_no][i] != yw2d.pos_y[znz_no][0])
        {
            int j = i - 1; // not in STAB
            alpha = alp * (num - j) / (num + 1);

            PutSpriteYW(top_label, end_label, yw2d.pos_x[znz_no][i], yw2d.pos_y[znz_no][i], 0.0f, rgb, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }
        else
        {
            znz_same++;
        }
    }

    if (znz_same == num && yw2d.znz_tmr[znz_no] != 0)
    {
        yw2d.znz_tmr[znz_no] = 0;
    }
    else
    {
        yw2d.znz_tmr[znz_no]++;
    }
}

void FlashStarYW(FLSH_CORE *flsh, u_char flsh_max, u_char flsh_min, u_char flsh_flm, u_char mode)
{
    float alpha;

    if (mode == 0)
    {
        if (flsh->flsh_tmr == 0)
        {
            flsh->flsh_tmr = flsh_flm - 1;
        }
        else
        {
            flsh->flsh_tmr--;
        }

        alpha = (flsh->flsh_tmr % flsh_flm) * (PI * 2) / (flsh_flm - 1);
        alpha = (SgCosf(alpha) * (flsh_max - flsh_min) + (flsh_max - flsh_min)) * 0.5f + flsh_min;
    }
    else if (mode != 0xff)
    {
        alpha = ((flsh->flsh_tmr + mode) % flsh_flm) * (PI * 2) / (flsh_flm - 1);
        alpha = (SgCosf(alpha) * (flsh_max - flsh_min) + (flsh_max - flsh_min)) * 0.5f + flsh_min;
    }
    else
    {
        flsh->flsh_tmr = 0;

        alpha = flsh_max;
    }

    flsh->alpha = alpha;
}

void CLRtoRGB(int *rgb, u_char r, u_char g, u_char b)
{
    *rgb = (r << 16) + (g << 8) + (b << 0);
}

void RGBtoCLR(int rgb, u_char *r, u_char *g, u_char *b)
{
    *r = rgb >> 16;
    *g = rgb >> 8;
    *b = rgb >> 0;
}

void PutStringYW(u_char msg_knd, u_char msg_no, short int pos_x, short int pos_y, int rgb, u_char alpha, int pri, u_char type)
{
    DISP_STR ds;
    STR_DAT sd = {
        .str = NULL,
        .pos_x = 0,
        .pos_y = 0,
        .type = 0,
        .r = 0xFF,
        .g = 0xFF,
        .b = 0xFF,
        .alpha = 0x80,
        .pri = 0,
    };
    int cnt_ofs;
    u_char r;
    u_char g;
    u_char b;

    sd.str = (u_char *)GetIngameMSGAddr(msg_knd, msg_no);

    CopyStrDToStr(&ds, &sd);

    r = (u_int)rgb >> 16 & 0xff;
    g = ((u_int)rgb & 0xff00) >> 8;
    b = (u_int)rgb & 0xff;

    ds.r = r;
    ds.g = g;
    ds.b = b;

    ds.alpha = alpha;
    ds.pri = pri;

    switch (type)
    {
    case 0:
        ds.pos_x = pos_x; ds.pos_y = pos_y;
    break;
    case 1:
        cnt_ofs = ChkChrNumUS(msg_knd, msg_no) / 2;

        ds.pos_x = pos_x - cnt_ofs;
        ds.pos_y = pos_y;
    break;
    case 2:
        cnt_ofs = ChkChrNumUS(msg_knd, msg_no);

        ds.pos_x = pos_x - cnt_ofs;
        ds.pos_y = pos_y;
    break;
    }

    SetMessageV2(&ds);
}

void RstMessageYW()
{
    msg.no_bak = msg.knd_bak = 0xff;
}

u_char PutMessageYW(u_char msg_knd, u_char msg_no, short int pos_x, short int pos_y, int rgb, u_char alpha, int pri)
{
    u_char all_page;
    u_char rtrn;

    all_page = ChkPageYW(msg_knd, msg_no);

    if (msg_knd != msg.knd_bak || msg_no != msg.no_bak)
    {
        msg.open_page = 0x1;
        msg.knd_bak = msg_knd;
        msg.no_bak = msg_no;
    }

    rtrn = 0;

    if (msg.open_page < all_page)
    {
        if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            msg.open_page++;
        }
    }
    else
    {
        rtrn = all_page;
    }

    PutPageYW(msg_knd, msg_no, msg.open_page, pos_x, pos_y, rgb, alpha, pri);

    return rtrn;
}

char ChkChrNumYW(u_char msg_knd, u_char msg_no)
{
    return GetStrLength((u_char*)GetIngameMSGAddr(msg_knd, msg_no));
}

int ChkChrNumUS(u_char msg_knd, u_char msg_no)
{
    u_char *addr;
    int width;

    addr = (u_char *)GetIngameMSGAddr(msg_knd, msg_no);

    width = GetStrWidth(addr);

    return width;
}

char ChkPageYW(u_char msg_knd, u_char msg_no)
{
    u_char *chk_str;
    u_char *nxt_str;
    char page_now;

    chk_str = (u_char *)GetIngameMSGAddr(msg_knd, msg_no);
    nxt_str = (u_char *)GetIngameMSGAddr(msg_knd, msg_no + 1);

    page_now = 0;

    while (chk_str != nxt_str)
    {
        if (*chk_str == 0xff)
        {
            page_now++;
        }

        chk_str++;
    }

    return page_now;
}

void PutPageYW(u_char msg_knd, u_char msg_no, u_char open_page, short int pos_x, short int pos_y, int rgb, u_char alpha, int pri)
{
    DISP_STR ds;
    STR_DAT sd = {
        .str = NULL,
        .pos_x = 0,
        .pos_y = 0,
        .type = 0,
        .r = 0xff,
        .g = 0xff,
        .b = 0xff,
        .alpha = 0x80,
        .pri = 0,
    };
    u_char *chk_str;
    char page_now;
    u_char r;
    u_char g;
    u_char b;

    chk_str = (u_char *)GetIngameMSGAddr(msg_knd, msg_no);

    page_now = 1;

    while (page_now != open_page)
    {
        while (*chk_str != 0xff)
        {
            chk_str++;
        }
        page_now++;
        chk_str++;
    }

    sd.str = chk_str;

    CopyStrDToStr(&ds, &sd);

    r = ((u_int)rgb & 0xff0000) >> 16;
    g = ((u_int)rgb & 0x00ff00) >>  8;
    b = ((u_int)rgb & 0x0000ff) >>  0;

    ds.pos_x = pos_x;
    ds.pos_y = pos_y;

    ds.r = r;
    ds.g = g;
    ds.b = b;

    ds.alpha = alpha;

    ds.pri = pri;

    SetMessageV2(&ds);
}

void PutPage2YW(u_char msg_knd, u_char msg_no, u_char open_page, short int pos_x, short int pos_y, int rgb, u_char alpha, int pri)
{
    DISP_STR ds;
    STR_DAT sd = {
        .str = NULL,
        .pos_x = 0,
        .pos_y = 0,
        .type = 0,
        .r = 0xff,
        .g = 0xff,
        .b = 0xff,
        .alpha = 0x80,
        .pri = 0,
    };
    u_char *chk_str;
    char page_now;
    u_char r;
    u_char g;
    u_char b;

    chk_str = (u_char *)GetIngameMSGAddr(msg_knd, msg_no);

    page_now = 1;

    while (page_now != open_page)
    {
        while (*chk_str != 0xff)
        {
            chk_str++;
        }
        page_now++;
        chk_str++;
    }

    sd.str = chk_str;

    CopyStrDToStr(&ds, &sd);

    r = ((u_int)rgb & 0xff0000) >> 16;
    g = ((u_int)rgb & 0x00ff00) >>  8;
    b = ((u_int)rgb & 0x0000ff) >>  0;

    ds.pos_x = pos_x;
    ds.pos_y = pos_y;

    ds.r = r;
    ds.g = g;
    ds.b = b;

    ds.alpha = alpha;

    ds.pri = pri;

    SetMessageV2_2(&ds);
}

void PutNumberYW(u_char font, int num, short int pos_x, short int pos_y, float sx, float sy, int rgb, short int alpha, int pri, int digit, u_char mode)
{
    int i;
    int multi10;
    int multi10_bak;
    int disp_number;
    int disp_digit;

    multi10 = 1;

    for (i = 0; i < digit; i++)
    {
        multi10 *= 10;
        multi10_bak = multi10;
    }

    disp_number = num % multi10;

    if (disp_number != 0)
    {
        for (i = 0; i < digit; i++)
        {
            multi10 /= 10;

            if (disp_number / multi10 != 0)
            {
                disp_digit = digit - i;

                break;
            }
        }
    }
    else
    {
        disp_digit = 1;
    }

    multi10 = multi10_bak;

    for (i = 0; i < digit; i++)
    {
        multi10 /= 10;

        if (mode != 0)
        {
            DigiPut01(font, (disp_number / multi10), i, pos_x, pos_y, sx, sy, rgb, alpha, pri);

            disp_number = disp_number - (disp_number / multi10) * multi10;
        }
        else if (digit - disp_digit <= i)
        {
            DigiPut01(font, (disp_number / multi10), i, pos_x, pos_y, sx, sy, rgb, alpha, pri);

            disp_number = disp_number - (disp_number / multi10) * multi10;
        }
    }
}

static void DigiPut01(u_char font, u_char num, u_char no, short int pos_x, short int pos_y, float sx, float sy, int rgb, short int alpha, int pri)
{
    DISP_SPRT ds;
    u_char r;
    u_char g;
    u_char b;
    short int rot_x;
    short int rot_y;
    u_char fw[14] = {
        18, 24, 24, 16, 18, 16, 10, 15, 14, 17, 17, 15, 13, 18
    };
    u_char fh[14] = {
        24, 40, 28, 24, 24, 21, 12, 20, 20, 24, 24, 24, 16, 20
    };

    switch(font)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        CopySprDToSpr(&ds, number_tex[font] + num);

        ds.x = no * fw[font] + pos_x;
        ds.y = pos_y;
    break;
    case 4:
        CopySprDToSpr(&ds, &spr_dat[309 + num]);

        ds.x = (no - 1) * fw[font] + pos_x;
        ds.y = pos_y - fh[font];
    break;
    case 5:
        CopySprDToSpr(&ds, &spr_dat[347 + num]);

        ds.x = (no - 1) * fw[font] + pos_x;
        ds.y = pos_y - fh[font];
    break;
    case 6:
#ifdef BUILD_EU_VERSION
        CopySprDToSpr(&ds, &spr_dat[647 + num]);
#else
        CopySprDToSpr(&ds, &spr_dat[646 + num]);
#endif

        ds.x = pos_x;
        ds.y = pos_y - no * fw[font];

        rot_x = ds.x + (u_char)(ds.w * 0.5f);
        rot_y = ds.y + (u_char)(ds.h * 0.5f);

        ds.crx = rot_x;
        ds.cry = rot_y;

        ds.rot = -90.0f;
    break;
    case 7:
        CopySprDToSpr(&ds, &spr_dat[14 + num]);

        ds.x = pos_x + no * fw[font] * sx;
        ds.y = pos_y;

        ds.scw = sx;
        ds.sch = sy;

        ds.csx = ds.x + ds.w * sx * 0.5f;
        ds.csy = ds.y + ds.h * sy * 0.5f;
    break;
    case 8:
#ifdef BUILD_EU_VERSION
        CopySprDToSpr(&ds, &spr_dat[820 + num]);
#else
        CopySprDToSpr(&ds, &spr_dat[819 + num]);
#endif

        ds.x = pos_x + no * fw[font] * sx;
        ds.y = pos_y;

        ds.scw = sx;
        ds.sch = sy;

        ds.csx = ds.x + ds.w * sx * 0.5f;
        ds.csy = ds.y + ds.h * sy * 0.5f;
    break;
    case 9:
#ifdef BUILD_EU_VERSION
        CopySprDToSpr(&ds, &spr_dat[722 + num]);
#else
        CopySprDToSpr(&ds, &spr_dat[721 + num]);
#endif

        ds.x = pos_x + no * fw[font] * sx;
        ds.y = pos_y;

        ds.scw = sx;
        ds.sch = sy;

        ds.csx = ds.x + ds.w * sx * 0.5f;
        ds.csy = ds.y + ds.h * sy * 0.5f;
    break;
    case 10:
#ifdef BUILD_EU_VERSION
        CopySprDToSpr(&ds, &spr_dat[732 + num]);
#else
        CopySprDToSpr(&ds, &spr_dat[731 + num]);
#endif

        ds.x = pos_x + no * fw[font] * sx;
        ds.y = pos_y;

        ds.scw = sx;
        ds.sch = sy;

        ds.csx = ds.x + ds.w * sx * 0.5f;
        ds.csy = ds.y + ds.h * sy * 0.5f;
    break;
    case 11:
#ifdef BUILD_EU_VERSION
        CopySprDToSpr(&ds, &spr_dat[752 + num]);
#else
        CopySprDToSpr(&ds, &spr_dat[751 + num]);
#endif

        ds.x = pos_x + no * fw[font] * sx;
        ds.y = pos_y;

        ds.scw = sx;
        ds.sch = sy;

        ds.csx = ds.x + ds.w * sx * 0.5f;
        ds.csy = ds.y + ds.h * sy * 0.5f;
    break;
    case 12:
        CopySprDToSpr(&ds, &spr_dat[114 + num]);

        ds.x = pos_x + no * fw[font] * sx;
        ds.y = pos_y;

        ds.scw = sx;
        ds.sch = sy;

        ds.csx = ds.x + ds.w * sx * 0.5f;
        ds.csy = ds.y + ds.h * sy * 0.5f;
    break;
    case 13:
        CopySprDToSpr(&ds, &spr_dat[124 + num]);

        ds.x = pos_x + no * fw[font] * sx;
        ds.y = pos_y;

        ds.scw = sx;
        ds.sch = sy;

        ds.csx = ds.x + ds.w * sx * 0.5f;
        ds.csy = ds.y + ds.h * sy * 0.5f;
    break;
    }

    r = ((int)rgb & 0xff0000) >> 16;
    g = ((int)rgb & 0x00ff00) >>  8;
    b = ((int)rgb & 0x0000ff) >>  0;

    if (font == 0)
    {
        SetInteger3(pri, no * fw[0] + pos_x, pos_y, 0, r, g, b, alpha, num);
    }
    else if (font == 3)
    {
        SetInteger3(pri, no * fw[3] + pos_x, pos_y, 2, r, g, b, alpha, num);
    }
    else
    {
        ds.r = r;
        ds.g = g;
        ds.b = b;

        ds.alpha = alpha;

        if (pri != -1)
        {
            ds.z = 0x0fffffff - pri;
            ds.pri = pri;
        }

        DispSprD(&ds);
    }
}

void PolySquareYW(float pos_x, float pos_y, u_short bar_l, u_short bar_h, int rgb, float alp, float scl_x, float scl_y, int pri, u_char blnd, u_char sw_z, short int rz)
{
    u_char r;
    u_char g;
    u_char b;
    SQAR_DAT sd = {
        .w = 0,
        .h = 0,
        .x = 0,
        .y = 0,
        .pri = 0x1000,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x80,
    };
    DISP_SQAR ds;
    int i;

    CopySqrDToSqr(&ds, &sd);

    ds.x[0] = pos_x;
    ds.y[0] = pos_y;

    ds.x[2] = ds.x[0];
    ds.y[1] = ds.y[0];

    ds.crx = pos_x + (bar_l >> 1);
    ds.cry = pos_y + (bar_h >> 1);

    ds.rot = rz;

    ds.x[1] = ds.x[0] + bar_l * scl_x;
    ds.x[3] = ds.x[1];

    ds.y[2] = ds.y[0] + bar_h * scl_y;
    ds.y[3] = ds.y[2];

    r = ((int)rgb & 0xff0000) >> 16;
    g = ((int)rgb & 0x00ff00) >>  8;
    b = ((int)rgb & 0x0000ff) >>  0;

    for (i = 0; i < 4; i++)
    {
        ds.r[i] = r;
        ds.g[i] = g;
        ds.b[i] = b;
    }

    if (blnd == 0)
    {
        ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    }
    else
    {
        ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    }

    ds.alpha = alp;

    ds.z = 0xfffffff - pri;
    ds.pri = pri;

    if (sw_z != 0)
    {
        ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    }

    DispSqrD(&ds);
}

void YesNoCrslOKR(int pri, float pos_x, float pos_y, int rgb, float alp, float scl)
{
    DISP_SPRT ds;

    CopySprDToSpr(&ds, &effdat[56]);

    ds.r = (u_int)rgb >> 16;
    ds.g = (u_int)rgb >>  8;
    ds.b = (u_int)rgb >>  0;

    ds.x = ds.x + pos_x;
    ds.y = ds.y + pos_y;

    ds.scw = scl;
    ds.sch = 0.9f;

    ds.csx = ds.x;
    ds.csy = ds.y;

    ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);

    ds.alpha = alp;

    ds.pri = pri;
    ds.z = 0xfffffff - pri;

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);
}

void XYAdefaultYW(u_char no)
{
    yw2d.io_x[no] = 0;
    yw2d.io_y[no] = 0;
    yw2d.io_a[no] = 128.0f;
}

void BgFusumaYW(int rgb, float pos_x, float alpha, int pri)
{
    float scl_x;
    float scl_y;

    SetSprFile(PL_BGBG_PK2_ADDRESS);

    scl_x = 2.5396826f;
    scl_y = 2.516854f;

    PutSpriteYW(396, 396, -pos_x, 0.0f, 0.0f, rgb, alpha, scl_x, scl_y, 1, 0xff, 1, 0, 0);
    PutSpriteYW(397, 397, +pos_x, 0.0f, 0.0f, rgb, alpha, scl_x, scl_y, 1, 0xff, 1, 0, 0);

}

static void SttsRenew()
{
    iew.hp_max_chk = 0;

    if (
        item_use_dat[menu_item_wrk[ig_menu_wrk.csr[1]].item_no].type == 3 &&
        plyr_wrk.hp == 500
    )
    {
        iew.hp_max_chk = 1;
    }

    iew.flm_dff_chk = 0;

    if (
        item_use_dat[menu_item_wrk[ig_menu_wrk.csr[1]].item_no].type == 2 &&
         item_use_dat[menu_item_wrk[ig_menu_wrk.csr[1]].item_no].value0 == plyr_wrk.film_no
    )
    {
        iew.flm_dff_chk = 1;
    }
}

static void ItmTrFlsh(short int pos_x, short int pos_y, short int alpha)
{
    int rgb;

    if (ig_menu_wrk.item_num > 6)
    {
        FlashStarYW(&flsh[1], 192, 64, 60, 0);

        CLRtoRGB(&rgb, flsh[1].alpha, flsh[1].alpha, flsh[1].alpha);

        PutSpriteYW(282, 283, pos_x, pos_y, 0.0f, rgb, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }
    else
    {
        PutSpriteYW(282, 283, pos_x, pos_y, 0.0f, 0x404040, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }
}

u_int FromPKZ(u_int pkz_addr, u_char pk2_no)
{
    switch(pk2_no)
    {
    case 0:
        SetSprFile(0x1ce0000);
    break;
    case 1:
#ifdef BUILD_EU_VERSION
        SetSprFile(0x1cfefc0);
#else
        SetSprFile(0x1d05140);
#endif
    break;
    case 2:
#ifdef BUILD_EU_VERSION
        SetSprFile(0x1d210c0);
#else
        SetSprFile(0x1d266c0);
#endif
    break;
    case 3:
#ifdef BUILD_EU_VERSION
        SetSprFile(0x1d10000);
#else
        SetSprFile(0x1d15600);
#endif
    break;
    case 5:
#ifdef BUILD_EU_VERSION
        SetSprFile(0x1d54030);
#else
        SetSprFile(0x1d59630);
#endif
    break;
    case 4:
#ifdef BUILD_EU_VERSION
        SetSprFile(0x1d10000);
#else
        SetSprFile(0x1d15600);
#endif
    break;
    case 7:
#ifdef BUILD_EU_VERSION
        SetSprFile(0x1d83000);
#else
        SetSprFile(0x1d88100);
#endif
    break;
    case 8:
#ifdef BUILD_EU_VERSION
        SetSprFile(0x1ded000);
#else
        SetSprFile(0x1df2100);
#endif
    break;
    case 9:
#ifdef BUILD_EU_VERSION
        SetSprFile(0x1e04000);
#else
        SetSprFile(0x1e05b00);
#endif
    break;
    case 10:
        SetSprFile(0x1e90000);
    break;
    case 11:
#ifdef BUILD_EU_VERSION
        SetSprFile(0x1d51db0);
#else
        SetSprFile(0x1d573b0);
#endif
    break;
    case 12:
        SetSprFile(0x1e90000);
    break;
    case 13:
#ifdef BUILD_EU_VERSION
        SetSprFile(0x1d51db0);
#else
        SetSprFile(0x1d573b0);
#endif
    break;
    case 14:
        SetSprFile(0x1e90000);
    break;
    case 15:
        SetSprFile(0x1e90000);
    break;
    case 16:
        SetSprFile(0x1e90000);
    break;
    case 18:
#ifdef BUILD_EU_VERSION
        SetSprFile(0x1d23680);
#else
        SetSprFile(0x1d28c80);
#endif
    break;
    case 17:
#ifdef BUILD_EU_VERSION
        SetSprFile(0x1d10000);
#else
        SetSprFile(0x1d15600);
#endif
    break;
    }

    // missing return
}

void DcdCMP2PK2(u_int cmp_addr, u_int tmp_addr)
{
    SlideDecode((u_char*)cmp_addr, (u_char*)tmp_addr);

    SetSprFile(tmp_addr);
}

static void YW_DBG()
{
    int i;
    u_char spd;

    if (*key_now[11] != 0)
    {
        yw2d.pad_lock = 1;

        if (
            *key_now[8] == 1 ||
            (*key_now[8] > 25 && *key_now[8] % 5 == 1)
        )
        {
            if (ywdbg.knd_char != 0)
            {
                ywdbg.knd_char--;
            }
            else
            {
                ywdbg.knd_char = 6;
            }

            ywdbg.char_no = top_char[ywdbg.knd_char];
        }
        else if (
            *key_now[10] == 1 ||
            (*key_now[10] > 25 && *key_now[10] % 5 == 1)
        )
        {
            if (ywdbg.knd_char < 6)
            {
                ywdbg.knd_char++;
            }
            else
            {
                ywdbg.knd_char = 0;
            }

            ywdbg.char_no = top_char[ywdbg.knd_char];
        }
        else if (
            *key_now[2] == 1 ||
            (*key_now[2] > 25 && *key_now[2] % 5 == 1) ||
            Ana2PadDirCnt(3) == 1 ||
            (Ana2PadDirCnt(3) > 25 && Ana2PadDirCnt(3) % 5 == 1)
        )
        {
            if (ywdbg.char_no > top_char[0])
            {
                ywdbg.char_no--;
            }
            else
            {
                ywdbg.char_no = end_char[6];
            }

            for (i = 0; i < 7; i++)
            {
                if (top_char[i] <= ywdbg.char_no)
                {
                    ywdbg.knd_char = i;
                }
            }
        }
        else if (
            *key_now[3] == 1 ||
            (*key_now[3] > 25 && *key_now[3] % 5 == 1) ||
            Ana2PadDirCnt(1) == 1 ||
            (Ana2PadDirCnt(1) > 25 && Ana2PadDirCnt(1) % 5 == 1)
        )
        {
            if (ywdbg.char_no < end_char[6])
            {
                ywdbg.char_no++;
            }
            else
            {
                ywdbg.char_no = top_char[0];
            }

            for (i = 0; i < 7; i++)
            {
                if (top_char[i] <= ywdbg.char_no)
                {
                    ywdbg.knd_char = i;
                }
            }
        }
    }
    else
    {
        if (*key_now[10] != 0)
        {
            yw2d.pad_lock = 1;

            if (*key_now[6] != 0)
            {
                spd = 10;
            }
            else
            {
                spd = 1;
            }

            if (
                *key_now[0] == 1 ||
                (*key_now[0] > 25 && *key_now[0] % 5 == 1) ||
                Ana2PadDirCnt(0) == 1 ||
                (Ana2PadDirCnt(0) > 25 && Ana2PadDirCnt(0) % 5 == 1)
            )
            {
                spr_dat[ywdbg.char_no].y = spr_dat[ywdbg.char_no].y - spd;
            }
            else if (
                *key_now[1] == 1 ||
                (*key_now[1] > 25 && *key_now[1] % 5 == 1) ||
                Ana2PadDirCnt(2) == 1 ||
                (Ana2PadDirCnt(2) > 25 && Ana2PadDirCnt(2) % 5 == 1)
            )
            {
                spr_dat[ywdbg.char_no].y = spr_dat[ywdbg.char_no].y + spd;
            }
            else if (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && *key_now[2] % 5 == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && Ana2PadDirCnt(3) % 5 == 1)
            )
            {
                spr_dat[ywdbg.char_no].x = spr_dat[ywdbg.char_no].x - spd;
            }
            else if (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && *key_now[3] % 5 == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && Ana2PadDirCnt(1) % 5 == 1)
            )
            {
                spr_dat[ywdbg.char_no].x = spr_dat[ywdbg.char_no].x + spd;
            }
        }
        else
        {
            yw2d.pad_lock = 0;
        }
    }

    SetInteger2(0, 500.0f, 10.0f, 1, 0xff, 0xff, 0xff, ywdbg.knd_char);
    SetInteger2(0, 550.0f, 10.0f, 1, 0xff, 0xff, 0xff, ywdbg.char_no - top_char[ywdbg.knd_char]);
    SetInteger2(0, 500.0f, 30.0f, 1, 0xff, 0xff, 0xff, spr_dat[ywdbg.char_no].x);
    SetInteger2(0, 550.0f, 30.0f, 1, 0xff, 0xff, 0xff, spr_dat[ywdbg.char_no].y);
}
