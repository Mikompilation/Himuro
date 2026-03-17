#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ig_glst.h"

#include "graphics/graph2d/message.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/motion/mdlact.h"
#include "ingame/enemy/ene_ctl.h"
#include "ingame/info/inf_disp.h"
#include "ingame/menu/ig_album.h"
#include "ingame/menu/ig_file.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/item.h"
#include "main/glob.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"

static void IngameMenuGlstData();
static void IngameMenuGlstDataDisp();
static void GlstCntInit();
static void GlstCntRenew();
static void GlstModeInOut();
static void GlstModeInOut2();
static void DspGhostList(short int pos_x, short int pos_y, u_char alpha);
static void PutPtrn(u_short chr_lbl, u_char w, u_char h, short int x, short int y, int pri, u_char alp);

#include "data/glist_order.h" // static GLIST_ORDER glist_order[];
GLIST_INDEX glist_index[108] = {0};
GLIST_PHOTO glist_photo = {0};

static FLSH_CORE flsh;

static MENU_GLST menu_glist;

#ifdef BUILD_EU_VERSION
#define LOAD_ADDRESS 0x1d51db0
#else
#define LOAD_ADDRESS 0x1d573b0
#endif

void NewgameMenuGlstInit()
{
    u_int i;

    menu_glist = (MENU_GLST){0};

    for (i = 0; i < 108; i++)
    {
        glist_index[i].new_flg = 1;
        glist_index[i].disp_flg = 0;
    }
}

void StartGlstModeInit()
{
    ig_menu_wrk.csr[1] = 0;

    yw2d.pad_lock = 1;

    menu_glist.mode = 0;
    menu_glist.lst_top = 0;
    menu_glist.dtl_alp = 0;
    menu_glist.all_num = GetGlistAllNum();
    menu_glist.g_num = GetGlistGNum();
    menu_glist.complete = CheckGlistComplete(0xffff);
    menu_glist.cnt = 0;
    menu_glist.flg = 0;

    glist_photo.mode = 0;
    glist_photo.load_id = -1;
    glist_photo.tex = 0;
}

void IngameMenuGlst()
{
    IngameMenuGlstDisp();

    switch (menu_glist.mode)
    {
    case 0:
        if (yw2d.pad_lock == 0)
        {
            if (*key_now[4] == 1)
            {
                if (IsLoadEndAll() != 0)
                {
                    yw2d.out_mode_cnt = 11;

                    SeStartFix(3, 0, 0x1000, 0x1000, 1);
                }
            }
            else if (*key_now[6] == 1 || *key_now[5] == 1)
            {
                if (glist_index[ig_menu_wrk.csr[1]].disp_flg == 1 && glist_index[ig_menu_wrk.csr[1]].new_flg == 1)
                {
                    switch (CheckGlistComplete(ig_menu_wrk.csr[1]))
                    {
                    case 0:
                        menu_glist.mode = 1;
                    break;
                    case 1:
                        menu_glist.mode = 1;
                        menu_glist.complete = 1;
                    break;
                    case 2:
                        menu_glist.mode = 2;
                        menu_glist.complete = 1;
                    break;
                    }

                    menu_glist.flg = glist_index[ig_menu_wrk.csr[1]].new_flg;

                    glist_index[ig_menu_wrk.csr[1]].new_flg = 0;

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
                glist_photo.mode = 0;

                if (ig_menu_wrk.csr[1] != 0)
                {
                    if (ig_menu_wrk.csr[1] == menu_glist.lst_top)
                    {
                        menu_glist.lst_top --;
                    }

                    ig_menu_wrk.csr[1]--;
                }
                else
                {
                    ig_menu_wrk.csr[1] = menu_glist.all_num - 1;

                    if (menu_glist.all_num >= 10)
                    {
                        menu_glist.lst_top = menu_glist.all_num - 10;
                    }
                    else
                    {
                        menu_glist.lst_top = 0;
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
                glist_photo.mode = 0;

                if (ig_menu_wrk.csr[1] < menu_glist.all_num - 1)
                {
                    if (ig_menu_wrk.csr[1] == menu_glist.lst_top + 9)
                    {
                        menu_glist.lst_top++;
                    }

                    ig_menu_wrk.csr[1]++;
                }
                else
                {
                    ig_menu_wrk.csr[1] = 0;

                    menu_glist.lst_top = 0;
                }

                SeStartFix(0, 0, 0x1000, 0x1000, 1);
            }
            else if (pad[0].one & 0x8)
            {
                if (ig_menu_wrk.csr[1] != 107)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 1);

                    glist_photo.mode = 0;
                }

                menu_glist.lst_top = menu_glist.lst_top + 10;

                if (menu_glist.lst_top > 98)
                {
                    menu_glist.lst_top = 98;
                }

                ig_menu_wrk.csr[1] += 10;

                if (ig_menu_wrk.csr[1] > 107)
                {
                    ig_menu_wrk.csr[1] = 107;
                }
            }
            else if (pad[0].one & 0x4)
            {
                if (ig_menu_wrk.csr[1] != 0)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 1);

                    glist_photo.mode = 0;
                }

                if (menu_glist.lst_top > 10)
                {
                    menu_glist.lst_top -= 10;
                }
                else
                {
                    menu_glist.lst_top = 0;
                }

                if (ig_menu_wrk.csr[1] > 10)
                {
                    ig_menu_wrk.csr[1] -= 10;
                }
                else
                {
                    ig_menu_wrk.csr[1] = 0;
                }
            }
        }
    break;
    case 1:
        // do nothing ...
    break;
    case 2:
        menu_glist.cnt++;

        if (menu_glist.cnt > 30)
        {
            menu_glist.mode = 1;
            menu_glist.cnt = 0;
        }
    break;
    }
}

void IngameMenuGlstDisp()
{
    GlstCntInit();
    GlstModeInOut();

    IngameMenuGlstData();

    GlstModeInOut2();
    GlstCntRenew();
}

static void IngameMenuGlstData()
{
    IngameMenuGlstDataDisp();
}

static void IngameMenuGlstDataDisp()
{
    DspMenuTitle(yw2d.io_x[1], yw2d.io_y[1], yw2d.io_a[1], 0x32, 5);

    DspGhostList(yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);
}

static void GlstCntInit()
{
    return;
}

static void GlstCntRenew()
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

static void GlstModeInOut()
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

static void GlstModeInOut2()
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

static void DspGhostList(short int pos_x, short int pos_y, u_char alpha)
{
    DspGlst(alpha);
    DspGdtl(alpha);
    DspGInf(alpha);

    DispCaption(15, alpha);
}

void DspGlst(u_char alpha)
{
    CmnWindow(24, 0, 0, alpha, 0x80);

    DspLstOfs(menu_glist.lst_top, alpha);

    if (menu_glist.mode == 0)
    {
        FlashStarYW(&flsh, 64, 128, 120, 0);
    }
    else
    {
        FlashStarYW(&flsh, 64, 128, 120, 0xff);
    }

    CmnCursol(59, (ig_menu_wrk.csr[1] - menu_glist.lst_top) * 0x1e + 0x69, 0xfe, 0x1e, flsh.alpha, alpha, 0x1e000);

    PutSpriteYW(PHT_SCRBAR_BASE_U, PHT_SCRBAR_BASE_U, 0.0f, 17.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PHT_SCRBAR_BASE_D, PHT_SCRBAR_BASE_D, 0.0f, 167.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PHT_SCR_TRIANGLE_U, PHT_SCR_TRIANGLE_U, 0.0f, 17.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PHT_SCR_TRIANGLE_D, PHT_SCR_TRIANGLE_D, 0.0f, 167.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PHT_SCRBAR_BASE_M, PHT_SCRBAR_BASE_M, 0.0f, 92.0f, 0.0f, 0x808080, alpha, 1.0f, 28.6f, 0, 0xff, 1, 0, 0);

    SideBar(menu_glist.all_num, 10, 304, menu_glist.lst_top, 366, alpha, 0, 0xf);
}

void DspLstOfs(u_short start, u_char alpha)
{
    int i;

    for (i = 0; i < 10; i++)
    {
        if (glist_index[start+i].disp_flg == 1)
        {
            PutRayName(glist_order[start+i].knd, glist_order[start+i].no, glist_order[start+i].msn, 72, i * 30 + 108, 0x808080, alpha);

            if (glist_index[start+i].new_flg == 1)
            {
                PutSpriteYW(GLST_COMPLETE, GLST_COMPLETE, 0.0f, i * 30 - 1, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }
        }
        else
        {
            PutStringYW(6, 33, 75, i * 30 + 107, 0x808080, alpha, 0x1000, 0);
        }
    }
}

void PutRayName(u_char ray_knd, u_short ray_no, u_char ray_msn, short int pos_x, short int pos_y, int rgb, u_char alpha)
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
    u_char r;
    u_char g;
    u_char b;

    sd.str = GetSubjectNameAddr(ray_knd,ray_no, ray_msn);

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

    ds.pri = 0x5000;

    SetMessageV2(&ds);
}

void DspGInf(u_char alpha)
{
    u_int per;
    float scale;

    CmnWindow(25, 0, 0, alpha, 0x80);

    PutSpriteYW(GLST_HIT_NUM, GLST_HIT_NUM, -4.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    PutNumberYW(8, menu_glist.g_num, 355, 51, 1.0f, 1.0f, 0x808080, alpha, -1, 3, 0);
    PutSpriteYW(GLST_ALL_NUM, GLST_ALL_NUM, -4.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    PutNumberYW(8, menu_glist.all_num, 355, 30, 1.0f, 1.0f, 0x808080, alpha, -1, 3, 0);
    PutSpriteYW(GLST_SXES_PER, GLST_SXES_PER, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    per = (menu_glist.g_num * 100) / menu_glist.all_num;

    if (per == 0)
    {
        per = (u_int)(menu_glist.g_num != 0);
    }

    PutNumberYW(8, per, 528, 41, 1.0f, 1.0f, 0x808080, alpha, -1, 3, 0);
    PutSpriteYW(GLST_NUM2_PER, GLST_NUM2_PER, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    if (menu_glist.complete != 0)
    {
        if (menu_glist.mode == 2)
        {
            scale = motLinearSupValue(20.0f, 1.0f, 1, menu_glist.cnt, 30);

            PutSpriteYW(GLST_CHRY, GLST_CHRY, 0.0f, 0.0f, (menu_glist.cnt * 703.0f) / 30.0f, 0x808080, alpha, scale, scale, 0, 0xff, 1, 1, 0);
        }
        else
        {
            PolySquareYW(491.0f, 39.0f, 0x7e, 30, 0, (int)(alpha / 2), 1.0f, 1.0f, 0x7000, 0, 1, 0xffef);

            PutSpriteYW(GLST_CHRY, GLST_CHRY, 0.0f, 0.0f, -17.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
        }
    }
}

void DspGdtl(u_char alpha)
{
    u_char photo_alpha;
    u_int cur;

    photo_alpha = 0;

    if (glist_index[ig_menu_wrk.csr[1]].disp_flg == 1)
    {
        if (menu_glist.flg != 0 && menu_glist.mode != 0)
        {
            PolySquareYW(379.0f, 157.0f, 190, 126, 0x800000, (alpha - menu_glist.dtl_alp), 1.0f, 1.0f, 0x1000, 0, 0, 0);

            photo_alpha = menu_glist.dtl_alp;
        }
        else if (glist_index[ig_menu_wrk.csr[1]].new_flg == 1)
        {
            PolySquareYW(379.0f, 157.0f, 190, 126, 0x808080, (alpha - menu_glist.dtl_alp), 1.0f, 1.0f, 0x1000, 0, 0, 0);

            photo_alpha = menu_glist.dtl_alp;
        }
        else
        {
            photo_alpha = alpha;
        }

        DispGlistPhoto(ig_menu_wrk.csr[1], photo_alpha);
    }

    Sheet(GLST_PHT_DAI, 322, 86, 296, 340, 0x32, alpha);

    Fuchidori(GLST_FUTI1, 322, 86, 296, 340, 0x2d, alpha);

    PutSpriteYW(GLST_PHT_WAKU, GLST_PHT_WAKU, 374.0f, 153.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(GLST_DSP_PLACE, GLST_DSP_PLACE, 95.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    if (menu_glist.mode != 0)
    {
        if (menu_glist.dtl_alp + 2 < alpha)
        {
            menu_glist.dtl_alp += 2;
        }
        else
        {
            menu_glist.dtl_alp = alpha;
            menu_glist.mode = 0;
        }
    }
    else
    {
        menu_glist.dtl_alp = 0;
    }

    if (glist_index[ig_menu_wrk.csr[1]].disp_flg == 1)
    {
        if (glist_order[ig_menu_wrk.csr[1]].knd == 4)
        {
            cur = glist_order[ig_menu_wrk.csr[1]].no - 301;
        }
        else
        {
            cur = ig_menu_wrk.csr[1];
        }

        PutStringYW(47, cur, 469, 122, 0, photo_alpha, 0x5000, 1);
        PutStringYW(48, cur, 335, 295, 0, photo_alpha, 0x5000, 0);
    }
}

void Sheet(u_short chr_lbl, short int pos_x, short int pos_y, u_short siz_x, u_short siz_y, u_char pri, u_char alp)
{
    int i;
    int j;
    u_char sozai_w;
    u_char sozai_h;
    u_char h_num;
    u_char h_edg;
    u_char v_num;
    u_char v_edg;
    short int put_x;
    short int put_y;
    short int put_w;
    short int put_h;

    sozai_w = spr_dat[chr_lbl].w;
    sozai_h = spr_dat[chr_lbl].h;

    h_num = (float)siz_x / sozai_w;
    h_edg = siz_x % sozai_w;

    v_num = (float)siz_y / sozai_h;
    v_edg = siz_y % sozai_h;

    for (j = 0; j < v_num + 1; j++)
    {
        for (i = 0; i < h_num + 1; i++)
        {
            put_x = pos_x + i * sozai_w;
            put_y = pos_y + j * sozai_h;

            if (i != h_num)
            {
                put_w = sozai_w;
            }
            else
            {
                put_w = h_edg;
            }

            if (j != v_num)
            {
                put_h = sozai_h;
            }
            else
            {
                put_h = v_edg;
            }

            PutPtrn(chr_lbl, put_w, put_h, put_x, put_y, pri << 12, alp);
        }
    }
}

void Fuchidori(u_short chr_lbl, short int pos_x, short int pos_y, u_short siz_x, u_short siz_y, u_char pri, u_char alp)
{
    u_char h;

    h = spr_dat[chr_lbl].h;

    Naraberu(chr_lbl + 0, pos_x,             pos_y,             0, siz_x, pri, alp);
    Naraberu(chr_lbl + 1, pos_x,             pos_y + siz_y - h, 0, siz_x, pri, alp);
    Naraberu(chr_lbl + 2, pos_x,             pos_y,             1, siz_y, pri, alp);
    Naraberu(chr_lbl + 3, pos_x + siz_x - h, pos_y,             1, siz_y, pri, alp);
}

void Naraberu(u_short chr_lbl, short int pos_x, short int pos_y, u_char dir, u_short dst, u_char pri, u_char alp)
{
    int i;
    u_char sozai_w;
    u_char sozai_h;
    u_char h_num;
    u_char h_edg;
    u_char v_num;
    u_char v_edg;
    short int put_x;
    short int put_y;
    short int put_w;
    short int put_h;

    sozai_w = spr_dat[chr_lbl].w;
    sozai_h = spr_dat[chr_lbl].h;

    switch (dir)
    {
    case 0:
        h_num = (float)dst / sozai_w;
        h_edg = dst % sozai_w;

        for (i = 0; i < h_num + 1; i++)
        {
            put_x =  pos_x + i * sozai_w;

            if (i != h_num)
            {
                put_w = sozai_w;
            }
            else
            {
                put_w = h_edg;
            }

            PutPtrn(chr_lbl, put_w, sozai_h, put_x, pos_y, pri << 12, alp);
        }
    break;
    case 1:
        v_num = (float)dst / sozai_h;
        v_edg = dst % sozai_h;

        for (i = 0; i < v_num + 1; i++)
        {
            put_y = pos_y + i * sozai_h;

            if (i != v_num)
            {
                put_h = sozai_h;
            }
            else
            {
                put_h = v_edg;
            }

            PutPtrn(chr_lbl, sozai_w, put_h, pos_x, put_y, pri << 12, alp);
        }
    break;
    }
}

static void PutPtrn(u_short chr_lbl, u_char w, u_char h, short int x, short int y, int pri, u_char alp)
{
    DISP_SPRT ds;

    CopySprDToSpr(&ds, &spr_dat[chr_lbl]);

    ds.w = w;
    ds.h = h;

    ds.x = x;
    ds.y = y;

    ds.pri = pri;
    ds.z = 0xfffffff - pri;

    ds.alpha = alp;

    DispSprD(&ds);
}

void ReSetGhostList(PICTURE_WRK new_pic)
{
    int i;
    int j;
    u_short ray_knd;
    u_short ray_no;
    u_char ray_msn;
    ENE_DAT *edat;
    char flg;

    ray_msn = new_pic.msn_no;

    for (j = 0; j < 3; j++)
    {
        ray_knd = new_pic.subject[j][0];
        ray_no = new_pic.subject[j][1];

        flg = 0;

        switch(ray_knd)
        {
        case 1:
            edat = jene_dat[ray_msn];

            for (i = 0; i < 29; i++)
            {
                if (glist_order[i].mdl_no == edat[ray_no].mdl_no)
                {
                    glist_index[i].disp_flg = 1;

                    break;
                }
                else
                {
                    switch(edat[ray_no].mdl_no)
                    {
                    case 62:
                        if (glist_order[i].mdl_no == 19)
                        {
                            flg = 1;
                        }
                    break;
                    case 63:
                        if (glist_order[i].mdl_no == 20)
                        {
                            flg = 1;
                        }
                    break;
                    case 64:
                        if (glist_order[i].mdl_no == 21)
                        {
                            flg = 1;
                        }
                    break;
                    case 65:
                        if (glist_order[i].mdl_no == 27)
                        {
                            flg = 1;
                        }
                    break;
                    case 66:
                        if (glist_order[i].mdl_no == 28)
                        {
                            flg = 1;
                        }
                    break;
                    }

                    if (flg == 1)
                    {
                        glist_index[i].disp_flg = 1;

                        break;
                    }
                }
            }
            break;
        case 2:
            edat = fene_dat[ray_msn];

            for (i = 0; i < 29; i++)
            {
                if (glist_order[i].mdl_no == edat[ray_no].mdl_no)
                {
                    glist_index[i].disp_flg = 1;

                    break;
                }
                else
                {
                    switch(edat[ray_no].mdl_no)
                    {
                    case 62:
                        if (glist_order[i].mdl_no == 19)
                        {
                            flg = 1;
                        }
                    break;
                    case 63:
                        if (glist_order[i].mdl_no == 20)
                        {
                            flg = 1;
                        }
                    break;
                    case 64:
                        if (glist_order[i].mdl_no == 21)
                        {
                            flg = 1;
                        }
                    break;
                    case 65:
                        if (glist_order[i].mdl_no == 27)
                        {
                            flg = 1;
                        }
                    break;
                    case 66:
                        if (glist_order[i].mdl_no == 28)
                        {
                            flg = 1;
                        }
                    break;
                    }

                    if (flg == 1)
                    {
                        glist_index[i].disp_flg = 1;

                        break;
                    }
                }
            }
            break;
        case 3:
            i = 0;

            while (glist_order[i].mdl_no != 0xff)
            {
                if (glist_order[i].msn == ray_msn && glist_order[i].knd == ray_knd && glist_order[i].no == ray_no)
                {
                    glist_index[i].disp_flg = 1;

                    break;
                }

                i++;
            }
            break;
        case 4:
        case 5:
            i = 0;

            while (glist_order[i].mdl_no != 0xff)
            {
                if (glist_order[i].knd == ray_knd && glist_order[i].no == ray_no)
                {
                    glist_index[i].disp_flg = 1;

                    break;
                }

                i++;
            }
        break;
        }
    }

    if (GetGlistGNum() == 108 && cam_custom_wrk.func_spe[4] == 0)
    {
        cam_custom_wrk.func_spe[4] = 1;
    }
}

u_short GetGlistAllNum()
{
    u_int i;

    i = 0;

    while (glist_order[i].mdl_no != 0xFF)
    {
        i++;
    }

    return i;
}

u_short GetGlistGNum()
{
    u_int i;
    u_int j;

    i = 0;
    j = 0;

    while (glist_order[i].mdl_no != 0xff)
    {
        if (glist_index[i].disp_flg == 1)
        {
            j++;
        }

        i++;
    }

    return j;
}

void DispGlistPhoto(u_int line, u_char alpha)
{
    u_int id;

    switch(glist_order[line].knd)
    {
    case 1:
    case 2:
        id = M000_DMY_TM2 + glist_order[line].mdl_no;
    break;
    case 3:
        id = M033_LIST_TM2 + line;
    break;
    case 4:
#ifdef BUILD_EU_VERSION
        id = ITEM_26_TM2 + glist_order[line].no;
#else
        id = STG16TTL_PK2 + glist_order[line].no;
#endif
    break;
    case 5:
    default:
        id = M006_LIST_TM2 + glist_order[line].no;
    break;
    }

    switch (glist_photo.mode)
    {
    case 0:
        if (glist_photo.load_id == -1)
        {
            glist_photo.load_id = LoadReq(id, LOAD_ADDRESS);
            glist_photo.mode = 1;
        }
        else if (IsLoadEnd(glist_photo.load_id) != 0)
        {
            glist_photo.load_id = -1;
        }
    break;
    case 1:
        if (IsLoadEnd(glist_photo.load_id) != 0)
        {
            glist_photo.mode = 2;
            glist_photo.load_id = -1;
        }
    break;
    case 2:
        glist_photo.tex = VramItem2D(0, 3);

        PkTm2(glist_photo.tex, 379, 157, 192, 128, alpha, 0x1d);
    break;
    }
}

char CheckGlistComplete(u_int list)
{
    u_int i;
    u_int flg;

    flg = 0;

    if (list == 0xffff)
    {
        for (i = 0; i < 108; i++)
        {
            if (glist_index[i].new_flg == 1)
            {
                return 0;
            }
        }
    }
    else
    {
        for (i = 0; i < 108; i++)
        {
            if (glist_index[i].new_flg == 1)
            {
                flg = 1;

                if (i != list)
                {
                    return 0;
                }
            }
        }
    }

    if (flg)
    {
        return 2;
    }

    return 1;
}

char CheckGlistRareGhost(u_int kind, u_int no)
{
    if (kind == 4 && (no >= 377 && no < 402))
    {
        return 1;
    }

    return 0;
}
