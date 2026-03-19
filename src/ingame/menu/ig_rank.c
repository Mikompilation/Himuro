#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ig_rank.h"

#include "graphics/graph2d/compress.h"
#include "graphics/graph2d/message.h"
#include "graphics/motion/mdlact.h"
#define INCLUDING_FROM_IG_RANK_C
#include "ingame/menu/ig_album.h"
#undef INCLUDING_FROM_IG_RANK_C
#include "ingame/menu/ig_file.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/item.h"
#include "ingame/photo/pht_make.h"
#include "main/glob.h"
#include "os/eeiop/eese.h"

static void IngameMenuRankData(u_char dsp);
static void IngameMenuRankDataDisp(u_char dsp);
static void RankCntInit();
static void RankCntRenew();
static void RankModeInOut();
static void RankModeInOut2();
static void DspTopRankLst(u_char dsp, short int pos_x, short int pos_y, u_char alpha);
static void DspRankersCHR(u_char pos, u_char rank, u_char alpha);
static void RankEngine(u_char rank, short int pos_x, short int pos_y, u_char alpha);
static char SrankCHK();
static void DspRankersSTR(u_char pos, u_char rank, u_char alpha);
static void DspRankersPHT(u_char pos, u_char rank, u_char alpha);
static void DspTopRankBig(u_char dsp, short int pos_x, short int pos_y, u_char alpha);
static void DspRankBigCHR(u_char rank, u_char alpha);
static void DspRankBigSTR(u_char rank, u_char alpha);
static void DspRankBigPHT(u_char rank, u_char alpha);
static void RankingChkNow();

SAVE_RANK save_rank = {0};

static MENU_RANK menu_rank;
static FLSH_CORE flsh;

void NewgameMenuRankInit()
{
    menu_rank = (MENU_RANK){0};
    save_rank = (SAVE_RANK){0};
}

void LoadgameMenuRankInit()
{
    menu_rank = (MENU_RANK){0};

    menu_rank.exist_s_rank = SrankCHK();
}


void StartRankModeInit()
{
    ig_menu_wrk.csr[1] = 0;

    menu_rank.csr_bak = 0xff;
    menu_rank.mode = 0;
    menu_rank.cnt = 0;

    yw2d.pad_lock = 1;

    InitPhotoExpand();
}

void IngameMenuRank()
{
    IngameMenuRankDisp();

    if (save_rank.pic_num == 0)
    {
        if (yw2d.pad_lock == 0)
        {
            if (*key_now[4] == 1)
            {
                yw2d.out_mode_cnt = 11;

                SeStartFix(3, 0, 0x1000, 0x1000, 1);
            }
        }
    }
    else
    {
        switch (menu_rank.mode)
        {
        case 0:
            if (yw2d.pad_lock == 0)
            {
                if (*key_now[4] == 1)
                {
                    yw2d.out_mode_cnt = 11;

                    SeStartFix(3, 0, 0x1000, 0x1000, 1);
                }
                else if ((*key_now[6] == 1) || (*key_now[5] == 1))
                {
                    menu_rank.mode = 1;

                    SeStartFix(1, 0, 0x1000, 0x1000, 1);
                }
                else if (
                    *key_now[1] == 1 ||
                    (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
                    Ana2PadDirCnt(2) == 1 ||
                    (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 1);

                    if (ig_menu_wrk.csr[1] < save_rank.pic_num - 1)
                    {
                        ig_menu_wrk.csr[1]++;
                    }
                    else
                    {
                        ig_menu_wrk.csr[1] = save_rank.pic_num - 1;
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

                    if (ig_menu_wrk.csr[1] != 0)
                    {
                        ig_menu_wrk.csr[1]--;
                    }
                    else
                    {
                        ig_menu_wrk.csr[1] = 0;
                    }
                }
            }
        break;
        case 1:
            if (yw2d.pad_lock == 0)
            {
                if (*key_now[4] == 1)
                {
                    menu_rank.mode = 0;

                    SeStartFix(3, 0, 0x1000, 0x1000, 1);
                }
                else if (
                    *key_now[3] == 1 ||
                    (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                    Ana2PadDirCnt(1) == 1 ||
                    (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 1);

                    if (ig_menu_wrk.csr[1] < save_rank.pic_num - 1)
                    {
                        ig_menu_wrk.csr[1]++;
                    }
                    else
                    {
                        ig_menu_wrk.csr[1] = 0;
                    }
                }
                else if (
                    *key_now[2] == 1 ||
                    (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                    Ana2PadDirCnt(3) == 1 ||
                    (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 1);

                    if (ig_menu_wrk.csr[1] != 0)
                    {
                        ig_menu_wrk.csr[1]--;
                    }
                    else
                    {
                        ig_menu_wrk.csr[1] = save_rank.pic_num - 1;
                    }
                }
            }
        break;
        }
    }
}

void IngameMenuRankDisp()
{
    RankCntInit();
    RankModeInOut();

    if (save_rank.pic_num == 0)
    {
        DrawMessageBox(0x14000, 170.0f, 210.0f, 300.0f, 60.0f, yw2d.io_a[4]);

        PutStringYW(33, 0, 215, 228, 0x808080, yw2d.io_a[4], 0xf000, 0);

        DspMenuTitle(yw2d.io_x[1], yw2d.io_y[1], yw2d.io_a[1], 0x32, 6);

        RankModeInOut2();
    }
    else
    {
        if (yw2d.inn_mode_cnt != 10)
        {
            IngameMenuRankData(0);
        }
        else
        {
            IngameMenuRankData(2);
        }

        RankModeInOut2();

        IngameMenuRankData(3);

        if (yw2d.out_mode_cnt == 12)
        {
            BgFusumaYW(0x606060, yw2d.io_x[0], yw2d.io_a[0], 0xffff7f0);

            IngameMenuRankData(2);
        }
    }

    RankCntRenew();
}

static void IngameMenuRankData(u_char dsp)
{
    switch (dsp)
    {
    case 0:
        DspMenuTitle(yw2d.io_x[1], yw2d.io_y[1], yw2d.io_a[1], 0x32, 6);

        IngameMenuRankDataDisp(0);
    break;
    case 1:
        DspMenuTitle(yw2d.io_x[1], yw2d.io_y[1], yw2d.io_a[1], 0x32, 6);

        IngameMenuRankDataDisp(1);
    break;
    case 2:
        IngameMenuRankDataDisp(2);
    break;
    case 3:
        IngameMenuRankDataDisp(3);
    break;
    }
}

static void IngameMenuRankDataDisp(u_char dsp)
{
    switch (dsp)
    {
    case 0:
    case 1:
    case 2:
        if (menu_rank.mode == 0)
        {
            DspTopRankLst(dsp, yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);

            DispCaption(16, yw2d.io_a[4]);
        }
        else
        {
            DspTopRankBig(dsp, yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);

            DispCaption(17, yw2d.io_a[4]);
        }
    break;
    case 3:
        if (menu_rank.mode == 0)
        {
            DspTopRankLst(3, yw2d.io_x[5], yw2d.io_y[5], yw2d.io_a[5]);
        }
        else
        {
            DspTopRankBig(3, yw2d.io_x[5], yw2d.io_y[5], yw2d.io_a[5]);
        }
    break;
    }
}

static void RankCntInit()
{
    return;
}

static void RankCntRenew()
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

static void RankModeInOut()
{
    int i;
    float fade_in;
    float fade_out;

    if (yw2d.inn_mode_cnt < 21)
    {
        if (yw2d.inn_mode_cnt < 11)
        {
            fade_in = (yw2d.inn_mode_cnt * 128) / 10.0f;
            fade_out = (1.0f - yw2d.inn_mode_cnt / 10.0f) * 128.0f;

            yw2d.io_a[0] = 128.0f;
            yw2d.io_a[1] = fade_in;
            yw2d.io_a[2] = fade_out;
            yw2d.io_a[3] = fade_out;
            yw2d.io_a[4] = fade_in;
            yw2d.io_a[5] = 0.0f;
        }
        else
        {
            fade_in = ((yw2d.inn_mode_cnt - 10) * 128) / 10.0f;

            yw2d.io_a[0] = 128.0f;
            yw2d.io_a[1] = 128.0f;
            yw2d.io_a[2] = 0.0f;
            yw2d.io_a[3] = 0.0f;
            yw2d.io_a[4] = 128.0f;
            yw2d.io_a[5] = fade_in;
        }

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
        if (yw2d.out_mode_cnt > 11)
        {
            fade_in = ((yw2d.out_mode_cnt - 11) * 128) / 10.0f;

            yw2d.io_a[0] = 128.0f;
            yw2d.io_a[1] = 128.0f;
            yw2d.io_a[2] = 0.0f;
            yw2d.io_a[3] = 0.0f;
            yw2d.io_a[4] = 128.0f;
            yw2d.io_a[5] = fade_in;

        } else {
            fade_in = (1.0f - (yw2d.out_mode_cnt - 1) / 10.0f) * 128.0f;
            fade_out = ((yw2d.out_mode_cnt - 1) * 128) / 10.0f;

            yw2d.io_a[0] = 128.0f;
            yw2d.io_a[1] = fade_out;
            yw2d.io_a[2] = fade_in;
            yw2d.io_a[3] = fade_in;
            yw2d.io_a[4] = fade_out;
            yw2d.io_a[5] = 0.0f;
        }

        PlayerStatusYW(yw2d.io_a[2]);

        DspTopMenu(yw2d.io_x[3], yw2d.io_y[3], yw2d.io_a[3], 0);
        DspPlayData(yw2d.io_x[2], yw2d.io_y[2], yw2d.io_a[2], 1);

        yw2d.pad_lock = 1;

        if (yw2d.out_mode_cnt == 1)
        {
            ig_menu_wrk.mode = 7;

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

static void RankModeInOut2()
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

static void DspTopRankLst(u_char dsp, short int pos_x, short int pos_y, u_char alpha)
{
    if (alpha == 0)
    {
        return;
    }

    if (dsp == 0 ||  dsp== 1)
    {
        CmnWindow(11, 0, 0, alpha, 0x80);

        DspRankersCHR(4, ig_menu_wrk.csr[1] + 4, alpha);
        DspRankersCHR(3, ig_menu_wrk.csr[1] + 3, alpha);
        DspRankersCHR(2, ig_menu_wrk.csr[1] + 2, alpha);
        DspRankersCHR(1, ig_menu_wrk.csr[1] + 1, alpha);

        CmnWindow(14, 0, 0, alpha, 0x80);

        PutSpriteYW(PHT_SCRBAR_BASE_U, PHT_SCRBAR_BASE_U, 0.0f, 17.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1 , 0, 0);
        PutSpriteYW(PHT_SCRBAR_BASE_D, PHT_SCRBAR_BASE_D, 0.0f, 167.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(PHT_SCR_TRIANGLE_U, PHT_SCR_TRIANGLE_U, 0.0f, 17.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(PHT_SCR_TRIANGLE_D, PHT_SCR_TRIANGLE_D, 0.0f, 167.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(PHT_SCRBAR_BASE_M, PHT_SCRBAR_BASE_M, 0.0f, 92.0f, 0.0f, 0x808080, alpha, 1.0f, 28.6f, 0, 0xff, 1, 0, 0);

        SideBar(save_rank.pic_num + 4, 5, 304, ig_menu_wrk.csr[1], 366, alpha, 0, 0xf);
    }

    if (dsp == 0 || dsp == 2)
    {
        DspRankersSTR(1, ig_menu_wrk.csr[1] + 1, alpha);
        DspRankersSTR(2, ig_menu_wrk.csr[1] + 2, alpha);
        DspRankersSTR(3, ig_menu_wrk.csr[1] + 3, alpha);
        DspRankersSTR(4, ig_menu_wrk.csr[1] + 4, alpha);
    }

    if (dsp == 3)
    {
        DspRankersPHT(1, ig_menu_wrk.csr[1] + 1, alpha);
        DspRankersPHT(2, ig_menu_wrk.csr[1] + 2, alpha);
        DspRankersPHT(3, ig_menu_wrk.csr[1] + 3, alpha);
        DspRankersPHT(4, ig_menu_wrk.csr[1] + 4, alpha);
    }
}

static void DspRankersCHR(u_char pos, u_char rank, u_char alpha)
{
    short int pos_y;

    if (rank > save_rank.pic_num)
    {
        return;
    }

    if (pos == 1)
    {
        CmnWindow(12, 0, 0, alpha, 0x80);

        PutSpriteYW(RNK_FNT1_RNK, RNK_FNT1_RNK, 52.0f, 90.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(RNK_FNT_TARGET2, RNK_FNT_POINT2, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(RNK_FNT1_I, RNK_FNT1_I, 46.0f, 205.0f, -90.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(rank + RNK_FNT1_MNS, rank + RNK_FNT1_MNS, 54.0f, 159.0f, -90.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        RankEngine(rank, 52, 107, alpha);

        PutSpriteYW(RNK_WAKU_BIG_U2, RNK_WAKU_BIG_U2, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 18.833334f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(RNK_WAKU_BIG_D2, RNK_WAKU_BIG_D2, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 18.833334f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(RNK_WAKU_BIG_L2, RNK_WAKU_BIG_L2, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 12.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(RNK_WAKU_BIG_R2, RNK_WAKU_BIG_R2, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 12.0f, 0, 0xff, 1, 0, 0);
    }
    else if (1 < pos)
    {
        pos_y = (pos - 2) * 53;

        CmnWindow(13, 0, pos_y, alpha, 0x80);

        PutSpriteYW(RNK_FNT1_RNK, RNK_FNT1_RNK, 39.0f, pos_y + 289, -90.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(RNK_FNT_POINT2, RNK_FNT_POINT2, 0.0f, pos_y + 192, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(rank + RNK_FNT1_MNS, rank + RNK_FNT1_MNS, 182.0f, pos_y + 279, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(RNK_FNT1_I, RNK_FNT1_I, 125.0f, pos_y + 279, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        RankEngine(rank, 66, pos_y + 280, alpha);

        PutSpriteYW(RNK_SAM_WAKU, RNK_SAM_WAKU, 256.0f, pos_y + 278, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }
}

static void RankEngine(u_char rank, short int pos_x, short int pos_y, u_char alpha)
{
    u_int sc;
    u_char sm;
    u_char ss;

    sc = save_rank.pic_inf[rank - 1].score;

    if (sc > 8500)
    {
        menu_rank.star = 0;
    }
    else if (sc > 7500)
    {
        menu_rank.star = 1;
    }
    else if (sc > 7000)
    {
        menu_rank.star = 2;
    }
    else if (sc > 6000)
    {
        menu_rank.star = 3;
    }
    else if (sc > 5000)
    {
        menu_rank.star = 4;
    }
    else if (sc > 4000)
    {
        menu_rank.star = 5;
    }
    else if (sc > 3500)
    {
        menu_rank.star = 6;
    }
    else if (sc > 3000)
    {
        menu_rank.star = 7;
    }
    else if (sc > 2500)
    {
        menu_rank.star = 8;
    }
    else if (sc > 2000)
    {
        menu_rank.star = 9;
    }
    else if (sc > 1600)
    {
        menu_rank.star = 10;
    }
    else if (sc > 1200)
    {
        menu_rank.star = 11;
    }
    else if (sc > 800)
    {
        menu_rank.star = 12;
    }
    else if (sc > 400)
    {
        menu_rank.star = 13;
    }
    else
    {
        menu_rank.star = 14;
    }

    sm = menu_rank.star / 3;

    if (menu_rank.star < 3)
    {
        ss = 1;
    }
    else
    {
        ss = menu_rank.star % 3;
    }

    PutSpriteYW(RNK_FNT2_S + sm, RNK_FNT2_S + sm, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    if (ss == 0)
    {
        PutSpriteYW(RNK_FNT1_PLS, RNK_FNT1_PLS, pos_x - 1, pos_y - 8, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }
    else if (ss == 2)
    {
        PutSpriteYW(RNK_FNT1_MNS, RNK_FNT1_MNS, pos_x + 20, pos_y - 8, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0 , 0xff, 1, 0, 0);
    }
}

static char SrankCHK()
{
    int i;
    u_int sc;
    char rtrn = 0;

    for (i = 0; i < 10; i++)
    {
        sc = save_rank.pic_inf[i].score;

        if (sc > 7000)
        {
            rtrn++;
        }
    }

    return rtrn;
}

static void DspRankersSTR(u_char pos, u_char rank, u_char alpha)
{
    int i;
    DISP_STR ds;
    u_char obj_num;
    u_short obj_py[4][3] = {
        { 24, 24, 24 }, { 24, 24, 24 }, { 12, 36, 24 }, { 0, 24, 48 },
    };
    STR_DAT dmy_str = {
        .str = NULL,
        .pos_x = 0,
        .pos_y = 0,
        .type = 0,
        .r = 0xff,
        .g = 0xff,
        .b = 0xff,
        .alpha = 0x80,
        .pri = 0x2,
    };

    if (rank > save_rank.pic_num)
    {
        return;
    }

    if (pos == 1)
    {
#ifdef BUILD_EU_VERSION
        PutStringYW(39, 0, 245, 34, 0x808080, alpha, 0x14000, 0);
#else
        PutStringYW(39, 0, 254, 34, 0x808080, alpha, 0x14000, 0);
#endif

        PutNumberYW(0, save_rank.best_score, 429, 35, 1.0f, 1.0f, 0x808080, alpha, 0x14000, 7, 0);
        PutNumberYW(0, save_rank.pic_inf[rank-1].score, 429, 94, 1.0f, 1.0f, 0x808080, alpha, 0x14000, 7, 0);

        PutDate(&save_rank.pic_inf[rank-1], 380, 123, alpha, 0x14000);

        PutStringYW(30, save_rank.pic_inf[rank-1].room, 393, 153, 0x808080, alpha, 0x14000, 0);

        obj_num = 0;

        for (i = 0; i < 3; i++)
        {
            if (save_rank.pic_inf[rank-1].subject[i][0] != 0)
            {
                obj_num++;
            }
        }

        if (obj_num != 0)
        {
            for (i = 0; i < obj_num; i++)
            {
                if (save_rank.pic_inf[rank-1].subject[i][0] != 0)
                {
                    dmy_str.str = GetSubjectNameAddr(
                        save_rank.pic_inf[rank-1].subject[i][0],
                        save_rank.pic_inf[rank-1].subject[i][1],
                        save_rank.pic_inf[rank-1].msn_no);

                    CopyStrDToStr(&ds, &dmy_str);

                    ds.pos_x = 382;
                    ds.pos_y = obj_py[obj_num][i] + 185;
                    ds.r = 0x80;
                    ds.g = 0x80;
                    ds.b = 0x80;
                    ds.alpha = alpha;
                    ds.pri = 0x14000;

                    SetMessageV2(&ds);
                }
            }
        }
        else
        {
            PutStringYW(33, 3, 448, obj_py[0][0] + 185, 0x808080, alpha, 0x14000, 0);
        }
    }
    else if (pos > 1)
    {
        PutNumberYW(0, save_rank.pic_inf[rank-1].score, 0x1ad, (pos - 2) * 53 + 284, 1.0f, 1.0f, 0x808080, alpha, 0x14000, 7, 0);
    }
}

static void DspRankersPHT(u_char pos, u_char rank, u_char alpha)
{
    u_char pic_no;
    float scale;

    if (rank > save_rank.pic_num)
    {
        return;
    }

    DrawSPhotoFromSmallPhotoArea(0, 0x7f000, 0, 0, 0, 0);

    pic_no = save_rank.pic_inf[rank-1].adr_no;

    if (pos == 1)
    {
        if (ig_menu_wrk.csr[1] != menu_rank.csr_bak)
        {
            menu_rank.csr_bak = ig_menu_wrk.csr[1];

            ReqPhotoExpand();
        }

        UncompressPhoto(pic_no + 24);

        PolySquareYW(101.0f, 106.0f, 216, 144, 0, alpha, 1.0f, 1.0f, 0x1f000, 0, 0, 0);

        if (CheckPhotoExpandEnd() != 0)
        {
            DrawPhotoFromPhotoWrk(pic_no + 24, 0x1e000, 0, 101, 106, 216, 144, alpha);
        }

        if (menu_rank.star < 3)
        {
            if (menu_rank.cnt < 30 && menu_rank.exist_s_rank == 0)
            {
                scale = motLinearSupValue(20.0f, 1.0f, 1, menu_rank.cnt, 30);

                menu_rank.cnt++;

                PutSpriteYW(RNK_CHRY, RNK_CHRY, 0.0f, 0.0f, (menu_rank.cnt * 695.0f) / 30.0f, 0x808080, alpha, scale, scale, 0, 0xff, 1, 1, 0);
            }
            else
            {
                PutSpriteYW(RNK_CHRY, RNK_CHRY, 0.0f, 0.0f, -25.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);

                menu_rank.exist_s_rank = 1;
            }
        }
        else
        {
            menu_rank.cnt = 0;
        }
    }
    else if (pos > 1)
    {
        DrawSPhotoFromSmallPhotoArea2(pic_no + 24, 0x1e000, 0, 0x102, (pos - 2) * 53 + 280, 0x30, 0x20, alpha);
    }
}

static void DspTopRankBig(u_char dsp, short int pos_x, short int pos_y, u_char alpha)
{
    if (alpha == 0)
    {
        return;
    }

    if (dsp == 0 || dsp == 1)
    {
        DspRankBigCHR(ig_menu_wrk.csr[1] + 1, alpha);
    }

    if (dsp == 0 || dsp == 2)
    {
        DspRankBigSTR(ig_menu_wrk.csr[1] + 1, alpha);
    }

    if (dsp == 3)
    {
        DspRankBigPHT(ig_menu_wrk.csr[1] + 1, alpha);
    }
}

static void DspRankBigCHR(u_char rank, u_char alpha)
{
    int rgb;

    PutSpriteYW(RNK_BASE_LU, RNK_BASE_LU, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(RNK_BASE_LD, RNK_BASE_LD, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(RNK_BASE_RU, RNK_BASE_RU, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(RNK_BASE_RD, RNK_BASE_RD, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(RNK_EN_LU, RNK_EN_LU, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(RNK_EN_LD, RNK_EN_LD, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(RNK_EN_RU, RNK_EN_RU, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(RNK_EN_RD, RNK_EN_RD, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(RNK_WAKU_BIG_U, RNK_WAKU_BIG_U, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 19.3f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(RNK_WAKU_BIG_D, RNK_WAKU_BIG_D, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 19.3f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(RNK_WAKU_BIG_L, RNK_WAKU_BIG_L, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 15.875f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(RNK_WAKU_BIG_R, RNK_WAKU_BIG_R, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 15.875f, 0, 0xff, 1, 0, 0);

    CmnWindow(11, 0, 0, alpha, 0x80);
    CmnWindow(6, 0, 0, alpha, 0x80);

    PutSpriteYW(RNK_FNT_TARGET, RNK_FNT_POINT, 0.0f, 0.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(rank + RNK_FNT2_MNS, rank + RNK_FNT2_MNS, 63.0f, 114.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(RNK_FNT3_I, RNK_FNT3_I, 36.0f, 114.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(RNK_SAM_WAKU, RNK_SAM_WAKU, 38.0f, 259.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(RNK_FNT1_RNK, RNK_FNT1_RNK, 557.0f, 107.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    RankEngine(rank, 558, 124, alpha);

    PutSpriteYW(RNK_SAM_WAKU, RNK_SAM_WAKU, 550.0f, 259.0f, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    if (save_rank.pic_num > 1)
    {
        FlashStarYW(&flsh, 64, 192, 60, 0);

        CLRtoRGB(&rgb, flsh.alpha, flsh.alpha, flsh.alpha);

        PutSpriteYW(TRIANGLE_LFT, TRIANGLE_LFT, 1.0f, 16.0f, 0.0f, rgb, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(PSE_END, PSE_END, -3.0f, 16.0f, 0.0f, rgb, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }
    else
    {
        FlashStarYW(&flsh, 64, 192, 60, 0xff);

        CLRtoRGB(&rgb, flsh.alpha, flsh.alpha, flsh.alpha);
    }
}

static void DspRankBigSTR(u_char rank, u_char alpha)
{
    int i;
    DISP_STR ds;
    u_char obj_num;
    u_short obj_py[4][3] = {
        { 24, 24, 24 }, { 24, 24, 24 }, { 12, 36, 24 }, { 0, 24, 48 },
    };
    STR_DAT dmy_str = {
        .str = NULL,
        .pos_x = 0,
        .pos_y = 0,
        .type = 0,
        .r = 0xff,
        .g = 0xff,
        .b = 0xff,
        .alpha = 0x80,
        .pri = 2,
    };

#ifdef BUILD_EU_VERSION
    PutStringYW(39, 0, 245, 34, 0x808080, alpha, 0x14000, 0);
#else
    PutStringYW(39, 0, 254, 34, 0x808080, alpha, 0x14000, 0);
#endif

    PutNumberYW(0, save_rank.best_score, 429, 35, 1.0f, 1.0f, 0x808080, alpha, 0x14000, 7, 0);

    PutDate(&save_rank.pic_inf[rank-1], 386, 341, alpha, 0x14000);

    PutStringYW(30, save_rank.pic_inf[rank-1].room, 399, 370, 0x808080, alpha, 0x14000, 0);

    PutNumberYW(0, save_rank.pic_inf[rank-1].score, 435, 398, 1.0f, 1.0f, 0x808080, alpha, 0x14000, 7, 0);

    obj_num = 0;

    for (i = 0; i < 3; i++)
    {
        if (save_rank.pic_inf[rank-1].subject[i][0] != 0)
        {
            obj_num++;
        }
    }

    if (obj_num != 0)
    {
        for (i = 0; i < obj_num; i++)
        {
            if (save_rank.pic_inf[rank-1].subject[i][0] != 0)
            {
                dmy_str.str = GetSubjectNameAddr(
                    save_rank.pic_inf[rank-1].subject[i][0],
                    save_rank.pic_inf[rank-1].subject[i][1],
                    save_rank.pic_inf[rank-1].msn_no);

                CopyStrDToStr(&ds, &dmy_str);

                ds.pos_x = 102;
                ds.pos_y = obj_py[obj_num][i] + 347;
                ds.r = 0x80;
                ds.g = 0x80;
                ds.b = 0x80;
                ds.alpha = alpha;
                ds.pri = 0x14000;

                SetMessageV2(&ds);
            }
        }
    }
    else
    {
        PutStringYW(33, 3, 180, obj_py[0][0] + 347, 0x808080, alpha, 0x14000, 0);
    }
}

static void DspRankBigPHT(u_char rank, u_char alpha)
{
    u_char pic_no;
    signed char tmp;

    DrawSPhotoFromSmallPhotoArea(0, 0x7f000, 0, 0, 0, 0);

    pic_no = save_rank.pic_inf[rank-1].adr_no;

    if (ig_menu_wrk.csr[1] != menu_rank.csr_bak)
    {
        menu_rank.csr_bak = ig_menu_wrk.csr[1];

        ReqPhotoExpand();
    }

    UncompressPhoto(pic_no + 24);

    PolySquareYW(128.0f, 77.0f, 384, 255, 0, alpha, 1.0f, 1.0f, 0x1f000, 0, 0, 0);

    if (CheckPhotoExpandEnd() != 0)
    {
        DrawPhotoFromPhotoWrk(pic_no + 24, 0x1e000, 0, 128, 77, 384, 255, alpha);
    }

    if (save_rank.pic_num > 1)
    {
        tmp = rank - 2;

        if (tmp < 0)
        {
            tmp = save_rank.pic_num - 1;
        }

        pic_no = save_rank.pic_inf[tmp].adr_no;

        DrawSPhotoFromSmallPhotoArea2(pic_no + 24, 0x1e000, 0, 40, 261, 48, 32, alpha);

        tmp = rank;

        if (save_rank.pic_num - 1 < tmp)
        {
            tmp = 0;
        }

        pic_no = save_rank.pic_inf[tmp].adr_no;

        DrawSPhotoFromSmallPhotoArea2(pic_no + 24, 0x1e000, 0, 552, 261, 48, 32, alpha);
    }
}

static void RankingChkNow()
{
    int i;
    int j;
    int tmp;
    PFILE_WRK tmp_srt;
    PICTURE_WRK tmp_pic;

    memset(save_rank.pic_inf, -1, sizeof(save_rank.pic_inf));

    tmp_srt = pfile_wrk;

    for (i = 0; i < tmp_srt.pic_num - 1; i++)
    {
        tmp_pic = tmp_srt.pic[i];

        tmp = i;

        for (j = i + 1; j < tmp_srt.pic_num; j++)
        {
            if (tmp_pic.score < tmp_srt.pic[j].score)
            {
                tmp_pic = tmp_srt.pic[j];

                tmp = j;
            }
        }

        tmp_srt.pic[tmp] = tmp_srt.pic[i];
        tmp_srt.pic[i] = tmp_pic;
    }

    if (tmp_srt.pic_num < 10)
    {
        save_rank.pic_num = tmp_srt.pic_num;
    }
    else
    {
        save_rank.pic_num = 10;
    }

    for (i = 0; i < save_rank.pic_num; i++)
    {
        save_rank.pic_inf[i] = tmp_srt.pic[i];
    }

    save_rank.best_score = save_rank.pic_inf[0].score;
}

void RankingChkMem(PICTURE_WRK new_pic)
{
    int i;
    int j;
    int same_pic;
    int del_no;
    int del_pic_id;
    int copy_flg;

    copy_flg = 0;

    for (i = 0; i < save_rank.pic_num; i++)
    {
        if (new_pic.score > save_rank.pic_inf[i].score)
        {
            break;
        }
    }

    if (i < 10)
    {
        same_pic = CheckSamePic(&new_pic);

        if (same_pic != -1)
        {
            if (new_pic.score > save_rank.pic_inf[same_pic].score)
            {
                del_no = same_pic;
            }
            else
            {
                return;
            }
        }
        else
        {
            copy_flg = 1;

            if (save_rank.pic_num == 10)
            {
                del_no = 9;
            }
            else
            {
                del_no = save_rank.pic_num;
            }
        }

        if (i == 0)
        {
            save_rank.best_score = new_pic.score;
        }

        if (save_rank.pic_num == 10)
        {
            del_pic_id = save_rank.pic_inf[del_no].adr_no;
        }
        else
        {
            if (copy_flg != 0)
            {
                del_pic_id = save_rank.pic_num;
            }
            else
            {
                del_pic_id = save_rank.pic_inf[del_no].adr_no;
            }
        }

        for (j = 9; j > i; j--)
        {
            if (del_no >= j)
            {
                save_rank.pic_inf[j] = save_rank.pic_inf[j-1];
            }
        }

        save_rank.pic_inf[j] = new_pic;
        save_rank.pic_inf[j].adr_no = del_pic_id;

        CopySPhoto(0x1a90000, new_pic.adr_no, 0x1a90000, save_rank.pic_inf[j].adr_no + 24);

        CopyPhoto(0x1aa5400, new_pic.adr_no, 0x1aa5400, save_rank.pic_inf[j].adr_no + 24);

        if (save_rank.pic_num < 10 && copy_flg)
        {
            save_rank.pic_num++;
        }
    }
}

int CheckSamePic(PICTURE_WRK *newp)
{
    int i;
    int j;
    int k;
    int no;
    PICTURE_WRK *savep;
    SUBJECT_CHK newp_chk[3];
    SUBJECT_CHK save_chk[3];

    no = -1;

    for (i = 0; i < 3; i++)
    {
        newp_chk[i].type = newp->subject[i][0];
        newp_chk[i].id = newp->subject[i][1];
    }

    for (i = 0; i < 3; i++)
    {
        if (newp_chk[i].type != 0)
        {
            break;
        }
    }

    if (i == 3)
    {
        return no;
    }

    savep = save_rank.pic_inf;

    for (i = 0; i < save_rank.pic_num; i++)
    {
        if (newp->msn_no == savep->msn_no)
        {
            for (j = 0; j < 3; j++)
            {
                save_chk[j].type = savep->subject[j][0];
                save_chk[j].id = savep->subject[j][1];
            }

            for (j = 0; j < 3; j++)
            {
                for (k = 0; k < 3; k++)
                {
                    if (newp_chk[j].type == 0 && save_chk[k].type == 0)
                    {
                        break;
                    }

                    if (newp_chk[j].type == save_chk[k].type && newp_chk[j].id == save_chk[k].id)
                    {
                        break;
                    }
                }

                if (k == 3)
                {
                    break;
                }
            }

            if (j == 3)
            {
                no = i;

                break;
            }
        }

        savep++;
    }

    return no;
}
