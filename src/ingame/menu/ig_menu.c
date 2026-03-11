#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ig_menu.h"

#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_sub.h"
// #include "graphics/graph2d/tim2.h"
#include "graphics/motion/mdlwork.h"
#include "ingame/info/inf_disp.h"
#include "ingame/menu/ig_album.h"
#include "ingame/menu/ig_camra.h"
#include "ingame/menu/ig_file.h"
#include "ingame/menu/ig_glst.h"
#include "ingame/menu/ig_map.h"
#include "ingame/menu/ig_option.h"
#include "ingame/menu/ig_rank.h"
#include "ingame/menu/item.h"
#include "ingame/menu/pause.h"
#include "ingame/plyr/time_ctl.h"
#include "ingame/plyr/unit_ctl.h"
#include "main/glob.h"
#include "os/eeiop/adpcm/ea_ctrl.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"
#include "outgame/mode_slct.h"

static void IngameMenuOpenInit();
static u_char MenuInOut();
static void TimeZone(short int pos_x, short int pos_y, float alp);
static void ClockHari(u_short char_lbl, short int pos_x, short int pos_y, float rot, short int rot_x, short int rot_y, float alp);
static void ComingOut(short int pos_x, short int pos_y, float alp, u_char shadow);
static void MenuAdjClock(sceCdCLOCK *time);
static void MenuPlayTime(sceCdCLOCK *time);
static void CmnWakuForWin(short int pos_x, short int pos_y, u_short siz_x, u_short siz_y, u_char u_hgh, u_char d_hgh, u_char l_wid, u_char r_wid, u_char pri, u_char alp, u_char rgb, u_char msk_sw);
static void PutParts(u_char u, u_char v, u_char w, u_char h, short int x, short int y, int pri, u_char alp, u_char rgb);
static void BehindTheMask(short int pos_x, short int pos_y, short int siz_x, short int siz_y, u_char alp, int pri, u_char top);
static void PutMask(short int pos_x, short int pos_y, u_char alp, u_char flp, int pri);
static void BehindTheMask2(short int pos_x, short int pos_y, short int siz_x, short int siz_y, u_char alp, int pri, u_char top);
static void CmnLineForWin(short int win_x, short int win_y, short int ofs_x, short int ofs_y, u_short siz_x, u_short siz_y, u_char pri, u_char alp, u_char rgb);

#include "data/ig_menu_str.h" // STR_DAT ig_menu_str;
#include "data/sozai.h" // SPRT_DAT sozai;
#include "data/mask_l.h" // SPRT_DAT mask_l;
#include "data/mask_s.h" // SPRT_DAT mask_s;

static ANA_PAD ana_pad;
static FLSH_CORE flsh;
static int menu_load_id;
static int fndr_load_id;
static int mode_load_id;

#ifdef BUILD_EU_VERSION
#define SPR_ADDRESS_1 0x1ce0000
#define SPR_ADDRESS_2 0x1d10000
#define SPR_ADDRESS_3 0x1d210c0
#define SPR_ADDRESS_4 0x1e04000
#define SPR_ADDRESS_5 0x1e90000
#define SPR_ADDRESS_6 0x1d51db0
#else
#define SPR_ADDRESS_1 0x1ce0000
#define SPR_ADDRESS_2 0x1d15600
#define SPR_ADDRESS_3 0x1d266c0
#define SPR_ADDRESS_4 0x1e05b00
#define SPR_ADDRESS_5 0x1e90000
#define SPR_ADDRESS_6 0x1d573b0
#endif

#define MSN_FILE_ADDRESS 0x84a000

void NewgameMenuInit()
{
    NewgameMenuMapInit();
    MenuItemInit();
    NewgameMenuFileInit();
    NewgameMenuOptionInit();
}

void LoadgameMenuInit()
{
    LoadgameMenuMapInit();
    LoadgameMenuItemInit();
    LoadgameMenuFileInit();
    LoadgameMenuOptionInit();
}

void IngameMenuInit()
{
    ig_menu_wrk = (INGAME_MENU_WRK){0};
    yw2d = (FOR_YW2D){0};

    menu_load_id = -1;
    fndr_load_id = -1;
    mode_load_id = -1;
}

void IngameMenuOpenStart()
{
    if (CanYouOpenMyMind(0) != 0)
    {
        IngameMenuOpenInit();
        SeStartFix(1, 0, 0x1000, 0x1000, 1);
        CaptureScreen(0x1a40);
        DrawScreen(0x7f000, 0x1a40, 0x80, 0x80, 0x80, 0x80);

        ingame_wrk.stts |= 0x20;
        ingame_wrk.mode = INGAME_MODE_MENU;
    }
}

static void IngameMenuOpenInit()
{
    int i;

    memset(menu_item_wrk, 0xff, sizeof(ANA_PAD)*3);

    ana_pad = (ANA_PAD){0};

    yw2d.menu_io_flg = 1;
    yw2d.menu_io_cnt = 0;
    yw2d.pad_lock = 1;
    yw2d.menu_cnt = 0;

    ItemInitAtMenuOpen();

    for (i = 0; i < 7; i++)
    {
        yw2d.io_x[i] = yw2d.io_y[i] = yw2d.io_a[i] = 0.0f;
    }

    ig_menu_wrk.mode = IGMENU_MODE_SLCT;

    yw2d.inn_mode_cnt = 0;
    yw2d.io_x[3] = 300.0f;

    SetTargetVolSeMenuFade(0x600);
    SetTargetVolAdpcmMenuFade(0x600);
}

void IngameMenuMain()
{
    u_char ret;

    SetSprFile(SPR_ADDRESS_1);
    SetSprFile(SPR_ADDRESS_2);
    SetSprFile(SPR_ADDRESS_3);

    if (yw2d.inn_mode_cnt != 0)
    {
        BgFusumaYW(0x606060, yw2d.io_x[0], yw2d.io_a[0], 0xffff7d0);
    }
    else
    {
        ret = MenuInOut();

        DrawScreen(0x7f000, 0x1a40, 0x80, 0x80, 0x80, 0x80);
        BgFusumaYW(0x606060, yw2d.io_x[0], yw2d.io_a[0], 0x7d000);

        if (ret != 0)
        {
            return;
        }
    }

    if (mode_load_id != -1)
    {
        if (IsLoadEnd(mode_load_id) != 0)
        {
            mode_load_id = -1;
        }

        IngameMenuModeSlctDisp();

        return;
    }

    switch(ig_menu_wrk.mode)
    {
    case IGMENU_MODE_SLCT:
        IngameMenuModeSlct();
    break;
    case IGMENU_MODE_MAP:
        SetSprFile(SPR_ADDRESS_4);

        IngameMenuMap();
    break;
    case IGMENU_MODE_ITEM:
        SetSprFile(SPR_ADDRESS_5);

        IngameMenuItem();
    break;
    case IGMENU_MODE_ALBM:
        SetSprFile(SPR_ADDRESS_6);

        IngameMenuAlbum(0);
    break;
    case IGMENU_MODE_FILE:
        SetSprFile(SPR_ADDRESS_5);

        IngameMenuFile();
    break;
    case IGMENU_MODE_GHOST:
        SetSprFile(SPR_ADDRESS_5);

        IngameMenuGlst();
    break;
    case IGMENU_MODE_RANKING:
        SetSprFile(SPR_ADDRESS_6);

        IngameMenuRank();
    break;
    case IGMENU_MODE_CAMERA:
        SetSprFile(SPR_ADDRESS_5);

        CameraCustomMain();
    break;
    case IGMENU_MODE_OPTN:
        SetSprFile(SPR_ADDRESS_5);

        IngameMenuOption();
    break;
    case IGMENU_MODE_ITEM_USE_SLCT:
        IngameMenuItemUseSlct();
    break;
    case IGMENU_MODE_ITEM_USE_END:
        IngameMenuItemUseSlct();
    break;
    case IGMENU_MODE_FILE_DATA:
        IngameMenuFileData();
    break;
    case IGMENU_MODE_RELATION:
        RelationShip();
    break;
    }
}

void IngameMenuModeSlct()
{
    u_char mode_limit;

    IngameMenuModeSlctDisp();

    mode_limit = 4;

    if (ingame_wrk.clear_count != 0)
    {
        mode_limit = 6;
    }

    if (StopCameraMenu() != 0)
    {
        mode_limit--;
    }

    if (yw2d.pad_lock == 0)
    {
        if (*key_now[4] == 1)
        {
            yw2d.menu_io_flg = 2;
            yw2d.menu_io_cnt = 45;

            SeStartFix(3, 0, 0x1000, 0x1000, 1);

            SetTargetVolSeMenuFade(0xfff);
            SetTargetVolAdpcmMenuFade(0xfff);
        }
        else if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            switch(ig_menu_wrk.csr[0])
            {
            case 0:
                SeStartFix(1, 0, 0x1000, 0x1000, 1);

                ig_menu_wrk.mode = IGMENU_MODE_MAP;

                mode_load_id = -1;

                StartMapModeInit();
            break;
            case 1:
                SeStartFix(1, 0, 0x1000, 0x1000, 1);

                ig_menu_wrk.mode = IGMENU_MODE_ITEM;

                mode_load_id = LoadReq(PL_ITEM_PK2, SPR_ADDRESS_5);

                StartItemModeInit();
            break;
            case 2:
                SeStartFix(1, 0, 0x1000, 0x1000, 1);

                ig_menu_wrk.mode = IGMENU_MODE_ALBM;

#ifdef BUILD_EU_VERSION
                mode_load_id = LoadReqLanguage(PL_PHOT_E_PK2, SPR_ADDRESS_6);
#else
                mode_load_id = LoadReq(PL_PHOT_PK2, SPR_ADDRESS_6);
#endif

                StartAlbumModeInit();
            break;
            case 3:
                SeStartFix(1, 0, 0x1000, 0x1000, 1);

                ig_menu_wrk.mode = IGMENU_MODE_FILE;

#ifdef BUILD_EU_VERSION
                mode_load_id = LoadReqLanguage(PL_FILE_E_PK2, SPR_ADDRESS_5);
#else
                mode_load_id = LoadReq(PL_FILE_PK2, SPR_ADDRESS_5);
#endif

                StartFileModeInit();
            break;
            case 4:
                if (ingame_wrk.clear_count != 0)
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 1);

                    ig_menu_wrk.mode = IGMENU_MODE_GHOST;

#ifdef BUILD_EU_VERSION
                    mode_load_id = LoadReqLanguage(PL_GLST_E_PK2, SPR_ADDRESS_5);
#else
                    mode_load_id = LoadReq(PL_GLST_PK2, SPR_ADDRESS_5);
#endif

                    StartGlstModeInit();
                }
                else
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 1);

                    ig_menu_wrk.mode = IGMENU_MODE_CAMERA;

#ifdef BUILD_EU_VERSION
                    mode_load_id = LoadReqLanguage(PL_CAME_E_PK2, SPR_ADDRESS_5);
#else
                    mode_load_id = LoadReq(PL_CAME_PK2, SPR_ADDRESS_5);
#endif

                    CameraCustomInit();
                }
            break;
            case 5:
                SeStartFix(1, 0, 0x1000, 0x1000, 1);

                ig_menu_wrk.mode = IGMENU_MODE_RANKING;

#ifdef BUILD_EU_VERSION
                mode_load_id = LoadReqLanguage(PL_RANK_E_PK2, SPR_ADDRESS_6);
#else
                mode_load_id = LoadReq(PL_RANK_PK2, SPR_ADDRESS_6);
#endif

                StartRankModeInit();
            break;
            case 6:
                SeStartFix(1, 0, 0x1000, 0x1000, 1);

                ig_menu_wrk.mode = IGMENU_MODE_CAMERA;

#ifdef BUILD_EU_VERSION
                mode_load_id = LoadReqLanguage(PL_CAME_E_PK2, SPR_ADDRESS_5);
#else
                mode_load_id = LoadReq(PL_CAME_PK2, SPR_ADDRESS_5);
#endif

                CameraCustomInit();
            break;
            case 12:
                SeStartFix(1, 0, 0x1000, 0x1000, 1);

                ig_menu_wrk.mode = IGMENU_MODE_OPTN;

#ifdef BUILD_EU_VERSION
                mode_load_id = LoadReqLanguage(PL_OPTI_E_PK2, SPR_ADDRESS_5);
#else
                mode_load_id = LoadReq(PL_OPTI_PK2, SPR_ADDRESS_5);
#endif

                StartOptionModeInit(0);
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
            if (ig_menu_wrk.csr[0] != 0)
            {
                ig_menu_wrk.csr[0]--;
            }
            else
            {
                ig_menu_wrk.csr[0] = mode_limit;
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
            if (ig_menu_wrk.csr[0] < mode_limit)
            {
                ig_menu_wrk.csr[0]++;
            }
            else
            {
                ig_menu_wrk.csr[0] = 0;
            }

            SeStartFix(0, 0, 0x1000, 0x1000, 1);
        }
    }
}

int Get4Byte(u_char *addr)
{
    return ((u_int)addr[3] << 24) + ((u_int)addr[2] << 16) + ((u_int)addr[1] << 8) + (u_int)addr[0];
}

int GetIngameMSGAddr(u_char type, int msg_no)
{
    int addr;

    addr = MSN_FILE_ADDRESS + type * 4;
    addr = MSN_FILE_ADDRESS + Get4Byte((u_char *)addr) + msg_no * 4;
    addr = Get4Byte((u_char *)addr);

    return MSN_FILE_ADDRESS + addr;
}

void IngameMenuModeSlctDispInit()
{
    yw2d.menu_io_cnt = 45;
}

void IngameMenuModeSlctDisp()
{
    PlayerStatusYW(yw2d.io_a[2]);

    DspTopMenu(0, 0, 0x80, yw2d.io_x[3]);
    DspPlayData(yw2d.io_x[2], yw2d.io_y[2], yw2d.io_a[2], 0);

    yw2d.inn_mode_cnt = 0;

    if (yw2d.menu_cnt < 0xffff)
    {
        yw2d.menu_cnt++;
    }
}

void DspTopMenu(short int pos_x, short int pos_y, short int alpha, u_short anm)
{
    int i;
    short int pos_v;
    short int pos_h;
    short int alp1;
    short int alp2;
    short int alp3;
    float alp_per;
    int rgb;
    float rot;
    int y_off_1 = 130;
    int y_off_2 = 168;
    int y_off_cur = 138;
    int v_off_clear = 265;
    int v_off_noclear = 228;
    int h_off = 9;

    if (anm != 0)
    {
        if (anm >= 200)
        {
            pos_h = (anm - 200) * 25 / 100.0f;
            pos_v = 30;

            alp_per = (300 - anm) / 100.0f;

            alp1 = alpha * (alp_per * alp_per);
            alp2 = 0;
            alp3 = 0;
        }
        else if (anm >= 100)
        {
            pos_h = 0;
            pos_v = (anm - 100) * 30 / 100.0f;

            alp_per = (200 - anm) / 100.0f;

            alp1 = alpha;
            alp2 = alpha * (alp_per * alp_per);
            alp3 = 0;
        }
        else
        {
            pos_h = 0;
            pos_v = 0;

            alp_per = (100 - anm) / 100.0f;

            alp1 = alpha;
            alp2 = alpha;
            alp3 = alpha * (alp_per * alp_per);
        }
    }
    else
    {
        pos_h = pos_v = 0;
        alp1 = alp2 = alp3 = alpha;
    }

    rot = 0.0f;

    if (ingame_wrk.clear_count != 0)
    {
        PutSpriteYW(MNU_SEL_BACK1, MNU_SEL_BACK7, pos_x - 15, pos_y, rot, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        FlashStarYW(&flsh, 128, 64, 120, 0);
        CLRtoRGB(&rgb, flsh.alpha, flsh.alpha, flsh.alpha);

        for (i = 191; i < 198; i++)
        {
            if (i == ig_menu_wrk.csr[0] + 191)
            {
                PutSpriteYW(i, i, pos_x - 15, pos_y, rot, rgb, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }
            else
            {
                PutSpriteYW(i, i, pos_x - 15, pos_y, rot, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }
        }

        if (StopCameraMenu() != 0)
        {
            PolySquareYW(pos_x + 35, pos_y + 366, 121, 30, 0, alp3 * 0.5f, 1.0f, 1.0f, 0x73000, 0, 0, 0 );
        }

        CmnCursol(pos_x + 33, (pos_y + y_off_cur) + ig_menu_wrk.csr[0] * 38, 123, 30, flsh.alpha, alp3, 0x73000);

        for (i = 0; i < 4; i++)
        {
            PutSpriteYW(MNU_MAIN_BASE_HOL1, MNU_MAIN_BASE_HOL1, pos_x + (pos_h + h_off), (pos_y + y_off_1) + i * 76, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(MNU_MAIN_BASE_HOL1, MNU_MAIN_BASE_HOL1, pos_x - (pos_h - h_off), (pos_y + y_off_2) + i * 76, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }

        PutSpriteYW(MNU_MAIN_BASE_VAT7, MNU_MAIN_BASE_VAT7_B, pos_x +  16, pos_y + (pos_v + v_off_clear), 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(MNU_MAIN_BASE_VAT7, MNU_MAIN_BASE_VAT7_B, pos_x + 146, pos_y - (pos_v - v_off_clear), 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }
    else
    {
        pos_y += 76;

        PutSpriteYW(MNU_SEL_BACK1, MNU_SEL_BACK5, pos_x - 15, pos_y, rot, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        FlashStarYW(&flsh, 128, 64, 120, 0);
        CLRtoRGB(&rgb, flsh.alpha, flsh.alpha, flsh.alpha);

        for (i = 191; i < 196; i++)
        {
            if (i != 195)
            {
                if (i == ig_menu_wrk.csr[0] + 191)
                {
                    PutSpriteYW(i, i, pos_x - 15, pos_y, rot, rgb, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                }
                else
                {
                    PutSpriteYW(i, i, pos_x - 15, pos_y, rot, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                }
            }
            else
            {
                if (i == ig_menu_wrk.csr[0] + 191)
                {
                    PutSpriteYW(MNU_FNT_CAMERA, MNU_FNT_CAMERA, pos_x - 15, pos_y - 76, rot, rgb, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                }
                else
                {
                    PutSpriteYW(MNU_FNT_CAMERA, MNU_FNT_CAMERA, pos_x - 15, pos_y - 76, rot, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                }
            }
        }

        if (StopCameraMenu() != 0)
        {
            PolySquareYW(pos_x + 35, pos_y + 290, 121, 30, 0, alp3 * 0.5f, 1.0f, 1.0f, 0x73000, 0, 0, 0);
        }

        CmnCursol(pos_x + 33, (pos_y + y_off_cur) + ig_menu_wrk.csr[0] * 38, 123, 30, flsh.alpha, alp3, 0x73000);

        for (i = 0; i < 3; i++)
        {
            PutSpriteYW(MNU_MAIN_BASE_HOL1, MNU_MAIN_BASE_HOL1, pos_x + (pos_h + h_off), (pos_y + y_off_1) + i * 76, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(MNU_MAIN_BASE_HOL1, MNU_MAIN_BASE_HOL1, pos_x - (pos_h - h_off), (pos_y + y_off_2) + i * 76, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }

        PutSpriteYW(MNU_MAIN_BASE_VAT5, MNU_MAIN_BASE_VAT5_B, pos_x +  16, pos_y + (pos_v + v_off_noclear), 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(MNU_MAIN_BASE_VAT5, MNU_MAIN_BASE_VAT5_B, pos_x + 146, pos_y - (pos_v - v_off_noclear), 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }
}

void WakuWaku(short int pos_x, short int pos_y, u_char alp, u_char num, u_char pri)
{
    int i;
    u_char adj_h[7] = { 33, 67, 86, 105, 124, 143, 161 };
    u_char adj_h2[7] = { 33, 67, 107, 107, 124, 143, 161 };
    int offset = 0x18;

    if (num < 1 || num > 7)
    {
        return;
    }

    for (i = 0; i < num; i++)
    {
        PutSpriteYW(MNU_SEL_BACK1 + i, MNU_SEL_BACK1 + i, pos_x, pos_y - adj_h2[num-1], 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, ((pri + 2) & 0xff) << 12, 1, 0, 0);
    }

    for (i = 0; i < num+1; i++)
    {
        PutSpriteYW(MNU_MAIN_BASE_HOL1, MNU_MAIN_BASE_HOL1, pos_x + offset, (pos_y + offset) + i * 0x26, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, ((pri + 1) & 0xff) << 12, 1, 0, 0);
    }

    PutSpriteYW(MNU_MAIN_BASE_VAT1 + (num - 1) * 2, MNU_MAIN_BASE_VAT1_B + (num - 1) * 2, pos_x + 0x1f, adj_h[num-1] + pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, pri << 12, 1, 0, 0);
    PutSpriteYW(MNU_MAIN_BASE_VAT1 + (num - 1) * 2, MNU_MAIN_BASE_VAT1_B + (num - 1) * 2, pos_x + 0xa1, adj_h[num-1] + pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, pri << 12, 1, 0, 0);
}

void WakuWaku2(short int pos_x, short int pos_y, u_char alp, u_char num, u_char pri)
{
    int i;

    for (i = 0; i < num; i++)
    {
        PutSpriteYW(MNU_SEL_BACK1, MNU_SEL_BACK1, pos_x, (pos_y - 13) + i * 50, 0.0f, 0x808080, alp, 1.0f, 1.3125f, 0, ((pri + 2) & 0xff) << 12, 1, 0, 0);
    }

    for (i = 0; i < num+1; i++)
    {
        PutSpriteYW(MNU_MAIN_BASE_HOL1, MNU_MAIN_BASE_HOL1, pos_x + 24, i * 50 + (pos_y + 112), 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, ((pri + 1) & 0xff) << 12, 1, 0, 0);
    }

    PutSpriteYW(MNU_MAIN_BASE_VAT1 + num * 2, MNU_MAIN_BASE_VAT1_B + num * 2, pos_x  + 28, pos_y + 214, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, pri << 12, 1, 0, 0);
    PutSpriteYW(MNU_MAIN_BASE_VAT1 + num * 2, MNU_MAIN_BASE_VAT1_B + num * 2, pos_x + 159, pos_y + 214, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, pri << 12, 1, 0, 0);
}

static u_char MenuInOut()
{
    u_char count;
    float per;
    u_short cnt;
    u_char rtn;

    rtn = 0;

    switch (yw2d.menu_io_flg)
    {
    case 1:
        if (yw2d.menu_io_cnt < 30)
        {
            count = yw2d.menu_io_cnt;

            yw2d.io_a[0] = (count * 128) / 30.0f;
            yw2d.io_a[1] = 0.0f;
            yw2d.io_a[2] = 0.0f;

            cnt = yw2d.menu_io_cnt;

            SimpleMoveRuler(&per, 20, 0, 10, 100, &cnt);

            yw2d.io_x[0] = (1.0f - per / 100.0f) * 84.0f;
            yw2d.io_x[3] = 300.0f;
        }
        else
        {
            yw2d.io_x[0] = 0.0f;

            count = yw2d.menu_io_cnt - 30;

            yw2d.io_a[0] = 128.0f;
            yw2d.io_a[1] = (count * 128) / 15.0f;
            yw2d.io_a[2] = (count * 128) / 15.0f;

            yw2d.io_x[3] = (15 - count) * 300 / 15.0f;
        }

        if (menu_load_id != -1 && IsLoadEnd(menu_load_id) != 0)
        {
            menu_load_id = -1;
        }

        if (yw2d.menu_io_cnt < 30)
        {
            rtn = 1;

            yw2d.pad_lock = 1;
            yw2d.menu_io_cnt++;
        }
        else if (yw2d.menu_io_cnt < 45)
        {
            if (menu_load_id == -1)
            {
                yw2d.menu_io_cnt++;
            }
            else
            {
                yw2d.menu_io_cnt = 30;

                rtn = 1;
            }

            yw2d.pad_lock = 1;
        }
        else
        {
            yw2d.pad_lock = 0;
            yw2d.menu_io_cnt = 45;
            yw2d.menu_io_flg = 0;
        }
    break;
    case 2:
        if (yw2d.menu_io_cnt >= 30)
        {
            yw2d.io_x[0] = 0.0f;

            count = yw2d.menu_io_cnt - 30;

            yw2d.io_a[0] = 128.0f;
            yw2d.io_a[1] = (count * 128) / 15.0f;
            yw2d.io_a[2] = (count * 128) / 15.0f;
            yw2d.io_x[3] = (15 - count) * 300 / 15.0f;
        }
        else
        {
            count = yw2d.menu_io_cnt;

            yw2d.io_a[0] = (count * 128) / 30.0f;
            yw2d.io_a[1] = 0.0f;
            yw2d.io_a[2] = 0.0f;

            cnt = 29 - yw2d.menu_io_cnt;

            SimpleMoveRuler(&per, 10, 0, 0x14, 100, &cnt);

            yw2d.io_x[0] = (per * 84.0f) / 100.0f;
            yw2d.io_x[3] = 300.0f;
        }

        if (fndr_load_id != -1 && IsLoadEnd(fndr_load_id) != 0)
        {
            fndr_load_id = -1;
        }

        if (yw2d.menu_io_cnt >= 30)
        {
            yw2d.menu_io_cnt--;
            yw2d.pad_lock = 1;
        }
        else if (yw2d.menu_io_cnt != 0)
        {
            if (yw2d.menu_io_cnt == 28)
            {
                fndr_load_id = -1;
            }

            if (yw2d.menu_io_cnt <= 28)
            {
                rtn = 1;
            }

            yw2d.menu_io_cnt--;
            yw2d.pad_lock = 1;
        }
        else if (fndr_load_id == -1)
        {
            rtn = 1;

            yw2d.menu_io_cnt = 0;
            yw2d.pad_lock = 0;
            yw2d.menu_io_flg = 0;

            ingame_wrk.stts &= ~0x20;
            ingame_wrk.mode = 6;

            SendManMdlTex();
        }
        else
        {
            rtn = 1;
        }
    break;
    }

    return rtn;
}

void DspPlayData(short int pos_x, short int pos_y, float alpha, u_char str_mode)
{
    if (str_mode == 0 || str_mode == 1)
    {
        PutSpriteYW(PLD_00, PLD_05, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        TimeZone(pos_x + 456, pos_y + 88, alpha);

        DspMenuTitle(pos_x, pos_y, alpha, 0x6e, 8);
        DispCaption(0, alpha);
    }

    if (str_mode == 0 || str_mode == 2)
    {
        ComingOut(pos_x + 1, pos_y + 1, alpha, 1);
        ComingOut(pos_x, pos_y, alpha, 0);
    }
}

static void TimeZone(short int pos_x, short int pos_y, float alp)
{
    float rot_s;
    float rot_m;
    float rot_h;
    sceCdCLOCK tmp;
    u_char s;
    u_char m;
    u_char h;

    MenuAdjClock(&tmp);

    s = TimeIsMoney(tmp.second);
    m = TimeIsMoney(tmp.minute);
    h = TimeIsMoney(tmp.hour);

    rot_s = s * 6;
    rot_m = m * 6 + rot_s / 60.0f;
    rot_h = h * 30 + rot_m / 12.0f;

    PutSpriteYW(PLD_CLK_BASE, PLD_CLK_BASE, pos_x, pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    ClockHari(PLD_CLK_SHRT, pos_x, pos_y, rot_h, 80, 74, alp);
    ClockHari(PLD_CLK_SHRT_SDW, pos_x + 2, pos_y + 2, rot_h, 82, 76, (alp + alp) / 3.0f);
    ClockHari(PLD_CLK_LONG, pos_x, pos_y, rot_m, 80, 74, alp);
    ClockHari(PLD_CLK_LONG_SDW, pos_x + 2, pos_y + 2, rot_m, 82, 76, (alp + alp) / 3.0f);
    ClockHari(PLD_CLK_SEC, pos_x, pos_y, rot_s, 80, 74, alp);
    ClockHari(PLD_CLK_SEC_SDW, pos_x + 2, pos_y + 2, rot_s, 82, 76, (alp + alp) / 3.0f);
}

static void ClockHari(u_short char_lbl, short int pos_x, short int pos_y, float rot, short int rot_x, short int rot_y, float alp)
{
    DISP_SPRT ds;

    CopySprDToSpr(&ds, &spr_dat[char_lbl]);

    ds.x += pos_x;
    ds.y += pos_y;

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    ds.crx = pos_x + rot_x;
    ds.cry = pos_y + rot_y;

    ds.alpha = alp;

    ds.rot = rot;

    DispSprD(&ds);
}

static void ComingOut(short int pos_x, short int pos_y, float alp, u_char shadow)
{
    int rgb;
    int pri;
    sceCdCLOCK tmp;
    u_char y;
    u_char m;
    u_char h;
    u_char dy;
    u_char hr2;
    u_char mn;
    u_char sc;

    if (shadow == 0)
    {
        pri = 0x1000;
        rgb = 0x808080;
    }
    else
    {
        rgb = 0;
        pri = 0x2000;
    }

    MenuAdjClock(&tmp);

    y = TimeIsMoney(tmp.year);
    m = TimeIsMoney(tmp.month);
    dy = TimeIsMoney(tmp.day);
    h = TimeIsMoney(tmp.hour);

#ifdef BUILD_EU_VERSION

    PutNumberYW(3, dy, pos_x + 222, pos_y + 130, 1.0f, 1.0f, rgb, alp, pri, 2, 0);

    if (sys_wrk.language == LANG_GERMAN)
    {
        u_char str_dot[2] = { 150, 0xff };
        STR_DAT sd = {
            .str = NULL,
            .pos_x = 0,
            .pos_y = 0,
            .type = 1,
            .r = 0x80,
            .g = 0x80,
            .b = 0x80,
            .alpha = 0x80,
            .pri = 0x5000,
        };
        DISP_STR ds;

        CopyStrDToStr(&ds, &sd);

        ds.pri = pri;
        ds.r = rgb;
        ds.g = rgb;
        ds.b = rgb;

        ds.pos_x = pos_x + 250;
        ds.pos_y = pos_y + 132;

        ds.str = str_dot;
        ds.alpha = alp;

        SetMessageV2(&ds);
    }

    PutStringYW(40, m + 23, pos_x + 267, pos_y + 130, rgb, alp, pri, 0);
#else
    PutStringYW(40, m + 23, pos_x + 222, pos_y + 130, rgb, alp, pri, 0);
    PutNumberYW(3, dy, pos_x + 277, pos_y + 130, 1.0f, 1.0f, rgb, alp, pri, 2, 0);
#endif

    PutStringYW(40, 23, pos_x + 324, pos_y + 130, rgb, alp, pri, 0);
    PutNumberYW(3, y, pos_x + 343, pos_y + 130, 1.0f, 1.0f, rgb, alp, pri, 2, 1);

    if (h < 12)
    {
        PutStringYW(40, 3, pos_x + 389, pos_y + 130, rgb, alp, pri, 0);
    }
    else
    {
        PutStringYW(40, 4, pos_x + 389, pos_y + 130, rgb, alp, pri, 0);
    }

    PutStringYW(40, 5, pos_x + 220, pos_y + 185, rgb, alp, pri, 0);
    PutStringYW(40, 22, pos_x + 335, pos_y + 215, rgb, alp, pri, 0);
    PutStringYW(40, 22, pos_x + 388, pos_y + 215, rgb, alp, pri, 0);

    MenuPlayTime(&tmp);

    hr2 = TimeIsMoney(tmp.hour);
    mn = TimeIsMoney(tmp.minute);
    sc = TimeIsMoney(tmp.second);

    PutNumberYW(3, hr2, pos_x + 299, pos_y + 217, 1.0f, 1.0f, rgb, alp, pri, 2, 0);
    PutNumberYW(3, mn, pos_x + 352, pos_y + 217, 1.0f, 1.0f, rgb, alp, pri, 2, 1);
    PutNumberYW(3, sc, pos_x + 405, pos_y + 217, 1.0f, 1.0f, rgb, alp, pri, 2, 1);

    PutStringYW(40, 9, pos_x + 220, pos_y + 255, rgb, alp, pri, 0);
    PutNumberYW(0, ingame_wrk.pht_cnt, pos_x + 389, pos_y + 255, 1.0f, 1.0f, rgb, alp, pri, 7, 0);

    PutStringYW(40, 11, pos_x + 220, pos_y + 295, rgb, alp, pri, 0);
    PutNumberYW(0, ingame_wrk.ghost_cnt, pos_x + 425, pos_y + 295, 1.0f, 1.0f, rgb, alp, pri, 5, 0);

    if (ingame_wrk.clear_count != 0)
    {
        PutStringYW(40, 15, pos_x + 220, pos_y + 335, rgb, alp, pri, 0);
        PutNumberYW(0, ingame_wrk.clear_count, pos_x + 460, pos_y + 335, 1.0f, 1.0f, rgb, alp, pri, 3, 0);

        if (ingame_wrk.clear_count != 0)
        {
            PutStringYW(40, 17, pos_x + 220, pos_y + 375, rgb, alp, pri, 0);
            PutNumberYW(0, save_rank.best_score, pos_x + 386, pos_y + 375, 1.0f, 1.0f, rgb, alp, pri, 7, 0);
        }
    }
}

int get_time_once = 0;

static void MenuAdjClock(sceCdCLOCK *time)
{
    u_long all_second;
    u_char adj_second;
    u_char adj_minute;
    u_char adj_hour;
    u_char rslt_second;
    u_char rslt_minute;
    u_char rslt_hour;

    all_second = (time_wrk.real_ofs / 60.0f);

    adj_second = all_second % 60;
    adj_minute = (all_second / 60) % 60;
    adj_hour = (all_second / 3600) % 60;

    rslt_second = TimeIsMoney(time_wrk.real.second) + adj_second;

    if (rslt_second >= 60)
    {
        rslt_second %= 60;
        adj_minute++;
    }

    rslt_minute = TimeIsMoney(time_wrk.real.minute) + adj_minute;

    if (rslt_minute >= 60)
    {
        rslt_minute %= 60;
        adj_hour++;
    }

    rslt_hour = TimeIsMoney(time_wrk.real.hour) + adj_hour;

    if (rslt_hour >= 24)
    {
        rslt_hour %= 24;

        SetRealTime();

        get_time_once = 0;
    }

    time->second = MoneyIsTime(rslt_second);
    time->minute = MoneyIsTime(rslt_minute);
    time->hour = MoneyIsTime(rslt_hour);

    time->day = time_wrk.real.day;
    time->month = time_wrk.real.month;
    time->year = time_wrk.real.year;
}

static void MenuPlayTime(sceCdCLOCK *time)
{
    u_long all_second;
    u_char adj_second;
    u_char adj_minute;
    u_char adj_hour;
    u_char rslt_second;

    all_second = (time_wrk.game / 60.0f);

    adj_second = all_second % 60;
    adj_minute = (all_second / 60) % 60;
    adj_hour = all_second / 3600;

    time->second = MoneyIsTime(adj_second);
    time->minute = MoneyIsTime(adj_minute);
    time->hour = MoneyIsTime(adj_hour);
}

void DspMenuTitle(short int pos_x, short int pos_y, float alp, u_char pri, u_char mode)
{
    PutSpriteYW(MNU_CURSOR1, MNU_CURSOR1, pos_x - 15, pos_y - 103, 0.0f, 0x808080, alp * 0.5f, 1.0f, 1.0f, 0, (pri + 4 & 0xff) << 12, 1, 0, 0);

    switch(mode)
    {
    case 1:
        PutSpriteYW(MNU_FNT_MAP, MNU_FNT_MAP, pos_x - 15, pos_y - 103, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, ((pri + 3) & 0xff) << 12, 1, 0, 0);
    break;
    case 2:
        PutSpriteYW(MNU_FNT_ITEM, MNU_FNT_ITEM, pos_x - 15, pos_y - 141, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, ((pri + 3) & 0xff) << 12, 1, 0, 0);
    break;
    case 3:
        PutSpriteYW(MNU_FNT_PHOT, MNU_FNT_PHOT, pos_x - 15, pos_y - 179, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, ((pri + 3) & 0xff) << 12, 1, 0, 0);
    break;
    case 4:
        PutSpriteYW(MNU_FNT_FILE, MNU_FNT_FILE, pos_x - 15, pos_y - 217, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, ((pri + 3) & 0xff) << 12, 1, 0, 0);
    break;
    case 5:
        PutSpriteYW(MNU_FNT_GHOST, MNU_FNT_GHOST, pos_x - 15, pos_y - 255, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, ((pri + 3) & 0xff) << 12, 1, 0, 0);
    break;
    case 6:
        PutSpriteYW(MNU_FNT_RANKING, MNU_FNT_RANKING, pos_x - 15, pos_y - 293, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, ((pri + 3) & 0xff) << 12, 1, 0, 0);
    break;
    case 7:
        PutSpriteYW(MNU_FNT_CAMERA, MNU_FNT_CAMERA, pos_x - 15, pos_y - 331, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, ((pri + 3) & 0xff) << 12, 1, 0, 0);
    break;
    case 8:
        PutSpriteYW(MNU_FNT_MENU, MNU_FNT_MENU, pos_x - 15, pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, ((pri + 3) & 0xff) << 12, 1, 0, 0);
    break;
    case 9:
        PutSpriteYW(MNU_FNT_OPTION, MNU_FNT_OPTION, pos_x - 15, pos_y - 331, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, ((pri + 3) & 0xff) << 12, 1, 0, 0);
    break;
    }

    PutSpriteYW(MNU_MAIN_BASE_HOL1, MNU_MAIN_BASE_HOL1, pos_x + 9, pos_y + 27, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, ((pri + 1) & 0xff) << 12, 1, 0, 0);
    PutSpriteYW(MNU_MAIN_BASE_HOL1, MNU_MAIN_BASE_HOL1, pos_x + 9, pos_y + 65, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, ((pri + 1) & 0xff) << 12, 1, 0, 0);
    PutSpriteYW(MNU_MAIN_BASE_VAT1, MNU_MAIN_BASE_VAT1_B, pos_x + 16, pos_y + 48, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, pri << 12, 1, 0, 0);
    PutSpriteYW(MNU_MAIN_BASE_VAT1, MNU_MAIN_BASE_VAT1_B, pos_x + 146, pos_y + 48, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, pri << 12, 1, 0, 0);
}

char SimpleMoveRuler(float *rslt, u_short t1, u_short t2, u_short t3, u_short dst, u_short *cnt)
{
    u_short count;
    float vmax;
    float a;
    float v;
    static float move;

    if (*cnt == 0)
    {
        move = 0.0f;
    }

    count = (float)((t1 + t2 + t3) * *cnt) / (t1 + t2 + t3);
    vmax = (float)(dst * 2) / (t1 + t2 + t3 + t2);

    if (count < t1)
    {
        a = (vmax / t1);
        v = a * count;

        move += v;
    }
    else if (count < t1 + t2)
    {
        move = move + vmax;
    }
    else if (count < (t1 + t2 + t3) - 1)
    {
        a = (vmax / t3);
        v = a * ((t1 + t2 + t3) - count);

        move += v;
    }
    else
    {
        move = dst;
    }

    *rslt = move;

    return t1 + t2 + t3 <= *cnt;
}

char MoveRuler(short int *mov_x, short int *mov_y, short int x0, short int y0, short int x1, short int y1, u_short t1, u_short t2, int t3, u_short *cnt)
{
    float per;
    char ret;

    ret = SimpleMoveRuler(&per, t1, t2, t3, 100, cnt);

    *mov_x = (x1 - x0) * (per / 100.0f) + x0;
    *mov_y = (y1 - y0) * (per / 100.0f) + x0;

    return ret;
}

static void CmnWakuForWin(short int pos_x, short int pos_y, u_short siz_x, u_short siz_y, u_char u_hgh, u_char d_hgh, u_char l_wid, u_char r_wid, u_char pri, u_char alp, u_char rgb, u_char msk_sw)
{
    int i;
    int j;
    int k;
    u_char h_num[3];
    u_char h_edg[3];
    u_char v_num[3];
    u_char v_edg[3];
    short int ofs_x[2];
    short int ofs_y[2];
    short int put_x;
    short int put_y;
    short int put_w;
    short int put_h;
    short int put_u;
    short int put_v;

    SetSprFile(0x1ce0000);

    if (msk_sw != 0)
    {
        PolySquareYW(pos_x + l_wid, pos_y + u_hgh, siz_x - (l_wid + r_wid), siz_y - (u_hgh + d_hgh), 0, alp * 0.5, 1.0f, 1.0f, ((pri + 1) & 0xff) << 12, 0, 0, 0);
    }

    h_num[2] = (float)siz_x / sozai.w;
    h_edg[2] = siz_x % sozai.w;

    v_num[0] = (float)u_hgh / sozai.h;
    v_edg[0] = u_hgh % sozai.h;

    ofs_x[0] = 0;
    ofs_y[0] = 0;

    v_num[1] = (float)d_hgh / sozai.h;
    v_edg[1] = d_hgh % sozai.h;

    ofs_x[1] = 0;
    ofs_y[1] = siz_y - d_hgh;

    for (k = 0; k < 2; k++)
    {
        for (j = 0; j < v_num[k] + 1; j++)
        {
            for (i = 0; i < h_num[2] + 1; i++)
            {
                put_x = pos_x + (ofs_x[k] + i * sozai.w);
                put_y = pos_y + (ofs_y[k] + j * sozai.h);

                put_u = put_x % sozai.w;
                put_v = put_y % sozai.h;

                if (i != h_num[2])
                {
                    put_w = sozai.w;
                }
                else
                {
                    put_w = h_edg[2];
                }

                if (j != v_num[k])
                {
                    put_h = sozai.h;
                }
                else
                {
                    put_h = v_edg[k];
                }

                PutParts(put_u, put_v, put_w, put_h, put_x, put_y, pri << 12, alp, rgb);
            }
        }
    }

    v_num[2] = (float)(siz_y - (u_hgh + d_hgh)) / sozai.h;
    v_edg[2] = (siz_y - (u_hgh + d_hgh)) % sozai.h;

    h_num[0] = (float)l_wid / sozai.w;
    h_edg[0] = l_wid % sozai.w;

    ofs_x[0] = 0;
    ofs_y[0] = u_hgh;

    h_num[1] = (float)r_wid / sozai.w;
    h_edg[1] = r_wid % sozai.w;

    ofs_x[1] = siz_x - r_wid;
    ofs_y[1] = ofs_y[0];

    for (k = 0; k < 2; k++)
    {
        for (j = 0; j < v_num[2] + 1; j++)
        {
            for(i = 0; i < h_num[k] + 1; i++)
            {
                put_x = pos_x + (ofs_x[k] + i * sozai.w);
                put_y = pos_y + (ofs_y[k] + j * sozai.h);

                put_u = put_x % sozai.w;
                put_v = put_y % sozai.h;

                if (i != h_num[k])
                {
                    put_w = sozai.w;
                }
                else
                {
                    put_w = h_edg[k];
                }

                if (j == v_num[2])
                {
                    put_h = v_edg[2];
                }
                else
                {
                    put_h = sozai.h;
                }

                PutParts(put_u, put_v, put_w, put_h, put_x, put_y, pri << 12, alp, rgb);
            }
        }
    }
}

static void PutParts(u_char u, u_char v, u_char w, u_char h, short int x, short int y, int pri, u_char alp, u_char rgb)
{
    DISP_SPRT ds;

    CopySprDToSpr(&ds, &sozai);

    ds.u = u;
    ds.v = v;

    ds.w = w;
    ds.h = h;

    ds.x = x;
    ds.y = y;

    ds.alpha = alp;

    ds.r = rgb;
    ds.g = rgb;
    ds.b = rgb;

    ds.pri = pri;
    ds.z = 0xfffffff - pri;

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GEQUAL, 1, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GREATER);

    DispSprD(&ds);

    ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GEQUAL, 1, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
}

static void BehindTheMask(short int pos_x, short int pos_y, short int siz_x, short int siz_y, u_char alp, int pri, u_char top)
{
    u_char i;
    u_char j;
    short put_x;
    short put_y;
    u_char flp;

    if (top > 1)
    {
        top = 1;
    }

    for (j = 0; j < 2; j++)
    {
        for (i = 0; i < 2 - top; i++)
        {
            put_x = pos_x + (j * (siz_x - mask_s.w));
            put_y = pos_y + (i * (siz_y - mask_s.h));

            flp = i + j * 2;

            PutMask(put_x, put_y, alp, flp, pri);
        }
    }
}

static void PutMask(short int pos_x, short int pos_y, u_char alp, u_char flp, int pri)
{
    DISP_SPRT ds;

    CopySprDToSpr(&ds, &mask_s);

    ds.x = pos_x;
    ds.y = pos_y;

    switch (flp)
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

    ds.alpha = alp;

    ds.pri = pri;
    ds.z = 0xfffffff - pri;

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);
}

static void BehindTheMask2(short int pos_x, short int pos_y, short int siz_x, short int siz_y, u_char alp, int pri, u_char top)
{
    u_char i;
    u_char j;
    short xo;
    short yo;
    u_char ptn;

    if (top > 1)
    {
        top = 1;
    }

    for (j = 0; j < 2 - top; j++)
    {
        for (i = 0; i < 2; i++)
        {
            ptn = i + j * 2;

            xo = pos_x + i * siz_x - 320;
            yo = pos_y + j * (siz_y - 1) - 224;

            AngleForWin(pri, xo, yo, 6, ptn);
        }
    }
}

static void CmnLineForWin(short int win_x, short int win_y, short int ofs_x, short int ofs_y, u_short siz_x, u_short siz_y, u_char pri, u_char alp, u_char rgb)
{
    int i;
    int j;
    u_char h_num;
    u_char h_edg;
    u_char v_num;
    u_char v_edg;
    short int put_x;
    short int put_y;
    short int put_w;
    short int put_h;
    short int put_u;
    short int put_v;

    h_num = (float)siz_x / sozai.w;
    h_edg = siz_x % sozai.w;

    v_num = (float)siz_y / sozai.h;
    v_edg = siz_y % sozai.h;

    for (j = 0; j < v_num + 1; j++)
    {
        for (i = 0; i < h_num + 1; i++)
        {
            put_x = win_x + ofs_x + i * sozai.w;
            put_y = win_y + ofs_y + j * sozai.h;

            put_u = ofs_x % sozai.w;
            put_v = ofs_y % sozai.h;

            if (i != h_num)
            {
                put_w = sozai.w;
            }
            else
            {
                put_w = h_edg;
            }

            if (j != v_num)
            {
                put_h = sozai.h;
            }
            else
            {
                put_h = v_edg;
            }

            PutParts(put_u, put_v, put_w, put_h, put_x, put_y, pri << 12, alp, rgb);
        }
    }
}

void CmnWindow(u_char win_no, short int pos_x, short int pos_y, u_char alp, u_char rgb)
{
    int i;
    #include "data/win_ptn.h" // WIN_PTN win_ptn[41];
    #include "data/msk_dat_top.h" // signed char msk_dat_top[41];
    #include "data/msk_siz.h" // MSK_SIZ msk_siz[55];
    #include "data/lin_dat_top.h" // signed char lin_dat_top[41];
    #include "data/lin_ptn.h" // PLS_LIN lin_ptn[129];
    int top;

    top = win_no == 2;

    BehindTheMask2(pos_x + win_ptn[win_no].pos_x, pos_y + win_ptn[win_no].pos_y, win_ptn[win_no].siz_x,
                   win_ptn[win_no].siz_y, alp, ((win_ptn[win_no].pri - 1) & 0xff) << 12, top);

    i = msk_dat_top[win_no];

    if (i != -1)
    {
        while (msk_siz[i].win_no == win_no)
        {
            PolySquareYW(msk_siz[i].pos_x + pos_x, msk_siz[i].pos_y + pos_y, msk_siz[i].siz_x, msk_siz[i].siz_y,
                         0, 1.0f, 1.0f, 1.0f, ((win_ptn[win_no].pri - 1) & 0xff) << 12, 0, 0, 0);

            i++;
        }
    }

    CmnWakuForWin(pos_x + win_ptn[win_no].pos_x, pos_y + win_ptn[win_no].pos_y, win_ptn[win_no].siz_x,
                  win_ptn[win_no].siz_y, win_ptn[win_no].u_hgh, win_ptn[win_no].d_hgh, win_ptn[win_no].l_wid,
                  win_ptn[win_no].r_wid, win_ptn[win_no].pri, alp, rgb, win_ptn[win_no].bak);

    i = lin_dat_top[win_no];

    if (i != -1)
    {
        while (lin_ptn[i].win_no == win_no)
        {
            CmnLineForWin(pos_x + win_ptn[win_no].pos_x, pos_y + win_ptn[win_no].pos_y,
                          lin_ptn[i].pos_x - win_ptn[win_no].pos_x, lin_ptn[i].pos_y - win_ptn[win_no].pos_y,
                          lin_ptn[i].siz_x, lin_ptn[i].siz_y, win_ptn[win_no].pri, alp, rgb);

            i++;
        }
    }
}

void CmnCursol(short int pos_x, short int pos_y, short int wide, short int high, float flsh, u_char alp, int pri)
{
    PolySquareYW(pos_x, pos_y, wide, high, 0xe9c489, flsh * alp / 128.0f * 0.5f, 1.0f, 1.0f, pri, 1, 0, 0);
}

u_short Ana2PadDirCnt(u_char chk)
{
    return ana_pad.on_cnt[chk];
}

void AnaPonChk()
{
    int i;
    u_char dir_now;

    if (pad[0].id != 121)
    {
        return;
    }

    if (pad[0].an_dir[0] != 0xff)
    {
        dir_now = ConvertRot2Dir(pad[0].an_rot[0], 0);

        for (i = 0; i < 4; i++)
        {
            if (i == dir_now)
            {
                if (ana_pad.on_cnt[i] < 10000)
                {
                    ana_pad.on_cnt[i]++;
                }
                else
                {
                    ana_pad.on_cnt[i] = 0;
                }
            }
            else
            {
                ana_pad.on_cnt[i] = 0;
            }
        }
    }
    else
    {
        for (i = 0; i < 4; i++)
        {
            ana_pad.on_cnt[i] = 0;
        }
    }
}

char CanYouOpenMyMind(char mode)
{
    char can;

    can = 1;

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

    if (ingame_wrk.stts & 0x80)
    {
        can = 0;
    }

    if (in_pause != 0)
    {
        can = 0;
    }

    if (ingame_wrk.mode == INGAME_MODE_WANDER_SOUL)
    {
        can = 0;
    }

    if (ingame_wrk.mode == INGAME_MODE_SPECIAL_EVENT)
    {
        can = 0;
    }

    if (plyr_wrk.mode == INGAME_MODE_AREA_MOVE)
    {
        can = 0;
    }

    if (eff_filament_off != 0)
    {
        can = 0;
    }

    return can;
}

int StopCameraMenu()
{
    if (ingame_wrk.game == 1)
    {
        return 1;
    }

    if (ingame_wrk.clear_count != 0)
    {
        return 0;
    }

    if (ingame_wrk.msn_no == 4 && poss_item[9] == 0)
    {
        return 1;
    }

    if (cribo.clear_info & 0x4)
    {
        return 0;
    }

    return 1;
}
