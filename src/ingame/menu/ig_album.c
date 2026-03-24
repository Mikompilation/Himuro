#include "common.h"
#include "typedefs.h"
#include "enums.h"

#ifdef MATCHING_DECOMP
#define INCLUDING_FROM_IG_ALBUM_C
#include "ig_album.h"
#undef INCLUDING_FROM_IG_ALBUM_C
#else
#include "ig_album.h"
#endif

#include "graphics/graph2d/compress.h"
#include "graphics/graph2d/message.h"
#include "ingame/menu/ig_file.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/item.h"
#include "ingame/photo/photo.h"
// #include "ingame/photo/pht_make.h"
#include "main/glob.h"
#include "os/eeiop/eese.h"

static void IngameAlbumMenu(u_char *csr0, u_char *csr1, u_char *pic, u_char *mode, u_char *list_top);
static void CmndMenuVrgn(u_char *csr0, u_char *csr1, u_char *pic, u_char *mode, u_char *list_top);
static void CmndMenuClrB(u_char *csr0, u_char *csr1, u_char *pic, u_char *mode, u_char *list_top);
static void IngameAlbumMenuDsp(u_char *csr0, u_char *csr1, u_char *pic, u_char *mode);
static void DrawAlbum(u_char csr, u_char csr_top, u_char mode, u_char str);
static void PhotOnlyAll(u_char csr, short int pos_x, short int pos_y, short int alpha);
static void PhotOnlyLst(u_char csr, short int pos_x, short int pos_y, short int alpha);
static void PhotOnlyBig(u_char csr, short int pos_x, short int pos_y, short int alpha);
static void IngameAlbumSort();
static void DspMainMenu(u_char csr0, u_char csr1, u_char mode, short int pos_x, short int pos_y, short int alpha);
static void DspSideMenu(u_char csr0, u_char csr1, u_char mode, short int pos_x, short int pos_y, short int alpha);
static void DspMenuWin(u_char type, u_char csr, u_char mode, short int pos_x, short int pos_y, float flash, short int alpha, u_short anm);
static void DspDetail(u_char csr, short int pos_x, short int pos_y, short int alpha, u_char dsp);
static void DspNotice(short int pos_x, short int pos_y, short int alpha, u_char now, u_char max, u_char btn);
static void DspSumnale(u_char csr, short int pos_x, short int pos_y, short int alpha);
static void DspPhtLst(u_char csr, u_char csr_top, short int pos_x, short int pos_y, short int alpha);
static void DspFilmCut(u_char csr, short int pos_x, short int pos_y, short int alpha);
static void DspSideSmnl(u_char csr, short int pos_x, short int pos_y, short int alpha);
static void DspBtmDtl(u_char csr, short int pos_x, short int pos_y, short int alpha);
static void DspCherry(short int pos_x, short int pos_y, short int alpha);
static void AlbmTrFlsh(short int pos_x, short int pos_y, short int alpha);
static void AlbmCntInit();
static void AlbmCntRenew();
static void AlbmModeInOut(char is_outgame);
static void AlbmModeInOut2(char is_outgame);
static u_char SealChk();

#define BASE_ADDRESS 0x84a000

static ALBM_WRK albm_wrk;
static FLSH_CORE flsh[6];

void NewgameMenuAlbumInit()
{
    int i;

    pfile_wrk = (PFILE_WRK){0};
    albm_wrk = (ALBM_WRK){0};

    for (i = 0; i < 24; i++)
    {
        pfile_wrk.pic[i].adr_no = i;
    }
}

void LoadgameMenuAlbumInit()
{
    int i;

    pfile_wrk = (PFILE_WRK){0};
    albm_wrk = (ALBM_WRK){0};

    for (i = 0; i < 24; i++)
    {
        pfile_wrk.pic[i].adr_no = i;
    }
}

void StartAlbumModeInit()
{
    ig_menu_wrk.csr[1] = 0;
    ig_menu_wrk.csr[2] = ig_menu_wrk.csr_bak[2] = 0;
    ig_menu_wrk.csr[3] = 0;
    ig_menu_wrk.csr[5] = 0xff;

    albm_wrk.dsp_pct_bak = 0xff;

    yw2d.pad_lock = 1;
    yw2d.out_mode_cnt = 0;

    InitPhotoExpand();
}

void IngameMenuAlbum(char is_outgame)
{
    IngameAlbumDisp(ig_menu_wrk.csr[1], ig_menu_wrk.csr[3], ig_menu_wrk.csr[2], is_outgame);

    if (pfile_wrk.pic_num == 0)
    {
        if (yw2d.pad_lock == 0)
        {
            if (*key_now[4] != 1)
            {
                return;
            }

            SeStartFix(3, 0, 0x1000, 0x1000, 1);

            yw2d.out_mode_cnt = 11;
        }
    }
    else if (ig_menu_wrk.csr[5] != 0xff)
    {
        if (ig_menu_wrk.csr_bak[0] == 0xff && yw2d.pad_lock == 0 && *key_now[4] == 1)
        {
            ig_menu_wrk.csr[5] = ig_menu_wrk.csr_bak[0];

            SeStartFix(3, 0, 0x1000, 0x1000, 1);
        }

        if (ig_menu_wrk.csr[5] != 0xff)
        {
            IngameAlbumMenu(&ig_menu_wrk.csr[5], &ig_menu_wrk.csr_bak[0], &ig_menu_wrk.csr[1], &ig_menu_wrk.csr[2], &ig_menu_wrk.csr[3]);
        }
    }
    else
    {
        if (ig_menu_wrk.csr[2] != 2)
        {
            if (yw2d.pad_lock == 0)
            {
                if (*key_now[6] == 1)
                {
                    ig_menu_wrk.csr_bak[0] = ig_menu_wrk.csr[5];
                    ig_menu_wrk.csr[5] = 0;

                    SeStartFix(1, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    if (*key_now[4] == 1)
                    {
                        SeStartFix(3, 0, 0x1000, 0x1000, 1);

                        yw2d.out_mode_cnt = 21;
                    }
                    else if (*key_now[5] == 1)
                    {
                        SeStartFix(1, 0, 0x1000, 0x1000, 1);

                        ig_menu_wrk.csr_bak[2] = ig_menu_wrk.csr[2];
                        ig_menu_wrk.csr[2] = 2;
                    }
                }
            }
        }
        else
        {
            if (yw2d.pad_lock == 0)
            {
                if (*key_now[6] == 1 || *key_now[5] == 1)
                {
                    ig_menu_wrk.csr_bak[0] = ig_menu_wrk.csr[5];
                    ig_menu_wrk.csr[5] = 0;

                    SeStartFix(1, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    if (*key_now[4] == 1)
                    {
                        SeStartFix(3, 0, 0x1000, 0x1000, 1);

                        ig_menu_wrk.csr[2] = ig_menu_wrk.csr_bak[2];

                        if (ig_menu_wrk.csr_bak[2] == 1)
                        {
                            if (pfile_wrk.pic_num >= 7)
                            {
                                if (ig_menu_wrk.csr[1] < pfile_wrk.pic_num - 6)
                                {
                                    ig_menu_wrk.csr[3] = ig_menu_wrk.csr[1];
                                }
                                else
                                {
                                    ig_menu_wrk.csr[3] = pfile_wrk.pic_num - 6;
                                }

                                if (ig_menu_wrk.csr[1] > pfile_wrk.pic_num - 1)
                                {
                                    ig_menu_wrk.csr[3]++;
                                }
                            }
                            else
                            {
                                ig_menu_wrk.csr[3] = 0;
                            }
                        }
                    }
                }
            }
        }

        if (pfile_wrk.pic_num != 0)
        {
            switch (ig_menu_wrk.csr[2])
            {
            case 0:
                IngameAlbumAllPad(&ig_menu_wrk.csr[1], &ig_menu_wrk.csr[3], &ig_menu_wrk.csr[2]);
            break;
            case 1:
                IngameAlbumLstPad(&ig_menu_wrk.csr[1], &ig_menu_wrk.csr[3], &ig_menu_wrk.csr[2]);
            break;
            case 2:
                IngameAlbumBigPad(&ig_menu_wrk.csr[1], &ig_menu_wrk.csr[3], &ig_menu_wrk.csr[2]);
            break;
            }
        }
    }
}

void IngameAlbumAllPad(u_char *csr, u_char *csr_top, u_char *mode)
{
    u_char csr_x;
    u_char csr_y;
    u_char max_x;
    u_char max_y;

    csr_x = *csr & 7;
    csr_y = *csr >> 3;

    max_x = (pfile_wrk.pic_num - 1) % 8;
    max_y = (pfile_wrk.pic_num - 1) / 8;

    if (yw2d.pad_lock == 0)
    {
        if (
            *key_now[2] == 1 ||
            (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
            Ana2PadDirCnt(3) == 1 ||
            (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
        )
        {
            if (csr_x != 0)
            {
                csr_x = csr_x - 1 & 0xff;
            }
            else if (csr_y == max_y)
            {
                csr_x = max_x;
            }
            else
            {
                csr_x = 7;
            }

            *key_now[7] = 0;

            if (max_x != 0 || csr_y != max_y)
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 1);
            }
        }
        else if (
            *key_now[3] == 1 ||
            (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
            Ana2PadDirCnt(1) == 1 ||
            (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
        )
        {
            if (csr_y == max_y)
            {
                if (csr_x < max_x)
                {
                    csr_x++;
                }
                else
                {
                    csr_x = 0;
                }
            }
            else
            {
                if (csr_x < 7)
                {
                    csr_x++;
                }
                else
                {
                    csr_x = 0;
                }
            }

            *key_now[7] = 0;

            if (max_x != 0 || csr_y != max_y)
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 1);
            }
        }
        else if (
            *key_now[0] == 1 ||
            (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
            Ana2PadDirCnt(0) == 1 ||
            (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
        )
        {
            if (csr_y != 0)
            {
                csr_y--;
            }
            else if (max_x < csr_x && max_y != 0)
            {
                csr_y = max_y - 1;
            }
            else
            {
                csr_y = max_y;
            }

            *key_now[7] = 0;

            if (max_y != 0)
            {
                if (csr_x <= max_x || max_y != 1)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 1);
                }
            }
        }
        else if (
            *key_now[1] == 1 ||
            (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
            Ana2PadDirCnt(2) == 1 ||
            (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
        )
        {
            if (csr_x > max_x)
            {
                if (max_y - 2 < csr_y)
                {
                    csr_y = 0;
                }
                else
                {
                    csr_y++;
                }
            }
            else
            {
                if (csr_y < max_y)
                {
                    csr_y++;
                }
                else
                {
                    csr_y = 0;
                }
            }

            *key_now[7] = 0;

            if (max_y != 0)
            {
                if (csr_x <= max_x || max_y != 1)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 1);
                }
            }
        }
    }

    *csr = csr_x + csr_y * 8;

    if (yw2d.pad_lock == 0 && *key_now[7] == 1)
    {
        if (*csr < pfile_wrk.pic_num)
        {
            if (pfile_wrk.pic[*csr].status & 0x2)
            {
                pfile_wrk.pic[*csr].status &= ~0x2;

                SeStartFix(5, 0, 0x1000, 0x1000, 1);
            }
            else if (SealChk() < 23)
            {
                pfile_wrk.pic[*csr].status |= 2;

                SeStartFix(5, 0, 0x1000, 0x1000, 1);
            }
            else
            {
                SeStartFix(2, 0, 0x1000, 0x1000, 1);
            }
        }
        else
        {
            SeStartFix(2, 0, 0x1000, 0x1000, 1);
        }
    }

    if (ingame_wrk.clear_count != 0)
    {
        if (yw2d.pad_lock == 0)
        {
            if (*key_now[10] == 1 || *key_now[8] == 1)
            {
                SeStartFix(1, 0, 0x1000, 0x1000, 1);

                *mode = 1;

                if (pfile_wrk.pic_num > 6)
                {
                    if (*csr < pfile_wrk.pic_num - 6)
                    {
                        *csr_top = *csr;
                    }
                    else
                    {
                        *csr_top = pfile_wrk.pic_num - 6;
                    }

                    if (pfile_wrk.pic_num - 1 < *csr)
                    {
                        (*csr_top)++;
                    }
                }
                else
                {
                    *csr_top = 0;
                }
            }
        }
    }
}

void IngameAlbumLstPad(u_char *csr, u_char *csr_top, u_char *mode)
{
    if (yw2d.pad_lock == 0)
    {
        if (
            *key_now[0] == 1 ||
            (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
            Ana2PadDirCnt(0) == 1 ||
            (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
        )
        {
            if (*csr != 0)
            {
                if (*csr == *csr_top)
                {
                    (*csr_top)--;
                }

                (*csr)--;
            }
            else
            {
                *csr = pfile_wrk.pic_num - 1;

                if (pfile_wrk.pic_num >= 6)
                {
                    *csr_top = pfile_wrk.pic_num - 6;
                }
                else
                {
                    *csr_top = 0;
                }
            }

            *key_now[7] = 0;

            if (pfile_wrk.pic_num > 1)
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 1);
            }
        }
        else if (
            *key_now[1] == 1 ||
            (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
            Ana2PadDirCnt(2) == 1 ||
            (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
        )
        {
            if (*csr < pfile_wrk.pic_num - 1)
            {
                if (*csr == *csr_top + 5)
                {
                    (*csr_top)++;
                }

                (*csr)++;
            }
            else
            {
                *csr = 0;
                *csr_top = 0;
            }

            *key_now[7] = 0;

            if (pfile_wrk.pic_num > 1)
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 1);
            }
        }

        if (yw2d.pad_lock == 0 && *key_now[7] == 1)
        {
            if (*csr < pfile_wrk.pic_num)
            {
                if (pfile_wrk.pic[*csr].status & 0x2)
                {
                    pfile_wrk.pic[*csr].status &= ~0x2;

                    SeStartFix(5, 0, 0x1000, 0x1000, 1);
                }
                else if (SealChk() < 23)
                {
                    pfile_wrk.pic[*csr].status |= 0x2;

                    SeStartFix(5, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    SeStartFix(2, 0, 0x1000, 0x1000, 1);
                }
            }
            else
            {
                SeStartFix(2, 0, 0x1000, 0x1000, 1);
            }
        }
    }

    if (ingame_wrk.clear_count != 0)
    {
        if (yw2d.pad_lock == 0)
        {
            if (*key_now[10] == 1 || *key_now[8] == 1)
            {
                SeStartFix(1, 0, 0x1000, 0x1000, 1);

                *mode = 0;
            }
        }
    }
}

void IngameAlbumBigPad(u_char *csr, u_char *csr_top, u_char *mode)
{
    if (
        *key_now[2] == 1 ||
        (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
        Ana2PadDirCnt(3) == 1 ||
        (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
    )
    {
        if (*csr != 0)
        {
            (*csr)--;
        }
        else
        {
            *csr = pfile_wrk.pic_num - 1;
        }

        if (pfile_wrk.pic_num > 1)
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 1);
        }

    }
    else if (
        *key_now[3] == 1 ||
        (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
        Ana2PadDirCnt(1) == 1 ||
        (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
    )
    {
        if (*csr < pfile_wrk.pic_num - 1)
        {
            (*csr)++;
        }
        else
        {
            *csr = 0;
        }

        if (pfile_wrk.pic_num > 1)
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 1);
        }
    }

    if (yw2d.pad_lock == 0 && *key_now[7] == 1)
    {
        if (*csr < pfile_wrk.pic_num)
        {
            if (pfile_wrk.pic[*csr].status & 0x2)
            {
                pfile_wrk.pic[*csr].status &= ~0x2;

                SeStartFix(5, 0, 0x1000, 0x1000, 1);
            }
            else if (SealChk() < 23)
            {
                pfile_wrk.pic[*csr].status |= 0x2;

                SeStartFix(5, 0, 0x1000, 0x1000, 1);
            }
            else
            {
                SeStartFix(2, 0, 0x1000, 0x1000, 1);
            }
        }
        else
        {
            SeStartFix(2, 0, 0x1000, 0x1000, 1);
        }
    }
}

static void IngameAlbumMenu(u_char *csr0, u_char *csr1, u_char *pic, u_char *mode, u_char *list_top)
{
    if (ingame_wrk.clear_count != 0)
    {
        CmndMenuClrB(csr0, csr1, pic, mode, list_top);
    }
    else
    {
        CmndMenuVrgn(csr0, csr1, pic, mode, list_top);
    }
}

static void CmndMenuVrgn(u_char *csr0, u_char *csr1, u_char *pic, u_char *mode, u_char *list_top)
{
    if (*csr1 == 0xff)
    {
        if (yw2d.pad_lock == 0)
        {
            if (*key_now[6] == 1 || *key_now[5] == 1)
            {
                switch (*csr0)
                {
                case 0:
                    if (*pic < pfile_wrk.pic_num)
                    {
                        if (pfile_wrk.pic[*pic].status & 0x2)
                        {
                            pfile_wrk.pic[*pic].status &= ~0x2;

                            SeStartFix(5, 0, 0x1000, 0x1000, 1);
                        }
                        else if (SealChk() < 23)
                        {
                            pfile_wrk.pic[*pic].status |= 0x2;

                            SeStartFix(5, 0, 0x1000, 0x1000, 1);
                        }
                        else
                        {
                            SeStartFix(2, 0, 0x1000, 0x1000, 1);
                        }

                    }
                    else
                    {
                        SeStartFix(2, 0, 0x1000, 0x1000, 1);
                    }

                    *csr0 = 0xff;
                break;
                case 1:
                    if (*pic < pfile_wrk.pic_num)
                    {
                        SeStartFix(1, 0, 0x1000, 0x1000, 1);

                        *csr1 = 1;
                    }
                    else
                    {
                        SeStartFix(2, 0, 0x1000, 0x1000, 1);
                    }
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
                if (*csr0 != 0)
                {
                    (*csr0)--;
                }
                else
                {
                    *csr0 = 1;
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
                if (*csr0 != 1)
                {
                    (*csr0)++;
                }
                else
                {
                    *csr0 = 0;
                }

                SeStartFix(0, 0, 0x1000, 0x1000, 1);
            }
        }
    }
    else if (*csr0 == 1)
    {
        albm_wrk.smenu_tmr = 0;
        albm_wrk.mmenu_tmr = 0;

        if (yw2d.pad_lock == 0)
        {
            if (*key_now[4] == 1)
            {
                *csr1 = 0xff;

                SeStartFix(3, 0, 0x1000, 0x1000, 1);
            }
            else if (*key_now[6] == 1 || *key_now[5] == 1)
            {
                if (*csr1 == 0)
                {
                    DeletePhotoData(*pic);

                    SeStartFix(1, 0, 0x1000, 0x1000, 1);

                    if (pfile_wrk.pic_num - 1 < *pic)
                    {
                        *pic = pfile_wrk.pic_num - 1;

                        if (*list_top != 0)
                        {
                            (*list_top)--;
                        }
                    }
                    else
                    {
                        if (pfile_wrk.pic_num - *list_top < 6 && *list_top != 0)
                        {
                            (*list_top)--;
                        }
                    }
                }
                else
                {
                    SeStartFix(3, 0, 0x1000, 0x1000, 1);
                }

                *csr0 = 0xff;
                *csr1 = 0xff;
            }
            else if (*key_now[2] == 1 || Ana2PadDirCnt(3) == 1)
            {
                *csr1 = 1 - *csr1;

                SeStartFix(0, 0, 0x1000, 0x1000, 1);
            }
            else if (*key_now[3] == 1 || Ana2PadDirCnt(1) == 1)
            {
                *csr1 = 1 - *csr1;

                SeStartFix(0, 0, 0x1000, 0x1000, 1);
            }
        }
    }
}

static void CmndMenuClrB(u_char *csr0, u_char *csr1, u_char *pic, u_char *mode, u_char *list_top)
{
    if (*csr1 == 0xff)
    {
        if (yw2d.pad_lock == 0)
        {
            if (*key_now[6] == 1 || *key_now[5] == 1)
            {
                switch (*csr0)
                {
                case 0:
                    if (*pic < pfile_wrk.pic_num)
                    {
                        if (pfile_wrk.pic[*pic].status & 0x2)
                        {
                            pfile_wrk.pic[*pic].status &= ~0x2;

                            SeStartFix(5, 0, 0x1000, 0x1000, 1);
                        }
                        else if (SealChk() < 23)
                        {
                            pfile_wrk.pic[*pic].status |= 0x2;

                            SeStartFix(5, 0, 0x1000, 0x1000, 1);
                        }
                        else
                        {
                            SeStartFix(2, 0, 0x1000, 0x1000, 1);
                        }

                    }
                    else
                    {
                        SeStartFix(2, 0, 0x1000, 0x1000, 1);
                    }

                    *csr0 = 0xff;
                break;
                case 1:
                    if (*pic < pfile_wrk.pic_num)
                    {
                        SeStartFix(1, 0, 0x1000, 0x1000, 1);

                        *csr1 = 1;
                    }
                    else
                    {
                        SeStartFix(2, 0, 0x1000, 0x1000, 1);
                    }
                break;
                case 2:
                    SeStartFix(1, 0, 0x1000, 0x1000, 1);

                    albm_wrk.mmenu_tmr = 0;

                    *csr0 = 0xff;
                    *csr1 = 0xff;

                    *mode = 1 - *mode;

                    if (*mode == 1)
                    {
                        if (pfile_wrk.pic_num > 6)
                        {
                            if (*pic < pfile_wrk.pic_num - 6)
                            {
                                *list_top = *pic;
                            }
                            else
                            {
                                *list_top = pfile_wrk.pic_num - 6;
                            }

                            if (*pic > pfile_wrk.pic_num - 1)
                            {
                                (*list_top)++;
                            }
                        }
                        else
                        {
                            *list_top = 0;
                        }
                    }
                break;
                case 3:
                    SeStartFix(1, 0, 0x1000, 0x1000, 1);

                    *csr1 = 0;
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
                if (*mode != 2)
                {
                    if (*csr0 != 0)
                    {
                        (*csr0)--;
                    }
                    else
                    {
                        *csr0 = 3;
                    }
                }
                else
                {
                    if (*csr0 != 0)
                    {
                        (*csr0)--;
                    }
                    else
                    {
                        *csr0 = 1;
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
                if (*mode != 2)
                {
                    if (*csr0 != 3)
                    {
                        (*csr0)++;
                    }
                    else
                    {
                        *csr0 = 0;
                    }
                }
                else
                {
                    if (*csr0 != 1)
                    {
                        (*csr0)++;
                    }
                    else
                    {
                        *csr0 = 0;
                    }
                }

                SeStartFix(0, 0, 0x1000, 0x1000, 1);
            }
        }
    }
    else
    {
        if (*csr0 == 3)
        {
            if (yw2d.pad_lock == 0)
            {
                if (*key_now[4] == 1)
                {
                    *csr1 = 0xff;

                    SeStartFix(3, 0, 0x1000, 0x1000, 1);
                }
                else if ((*key_now[6] == 1) || (*key_now[5] == 1))
                {
                    if (pfile_wrk.sort_key == *csr1)
                    {
                        if (pfile_wrk.skey_bak == 0)
                        {
                            pfile_wrk.skey_bak = 1;
                        }
                        else
                        {
                            pfile_wrk.skey_bak = 0;
                        }
                    }
                    else
                    {
                        pfile_wrk.skey_bak = 0;
                        pfile_wrk.sort_key = *csr1;
                    }

                    *csr0 = 0xff;
                    *csr1 = 0xff;

                    SeStartFix(1, 0, 0x1000, 0x1000, 1);

                    IngameAlbumSort();
                }
                else if (
                    *key_now[0] == 1 ||
                    (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
                    Ana2PadDirCnt(0) == 1 ||
                    (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
                )
                {
                    if (*csr1 != 0)
                    {
                        (*csr1)--;
                    }
                    else
                    {
                        *csr1 = 2;
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
                    if (*csr1 != 2)
                    {
                        (*csr1)++;
                    }
                    else
                    {
                        *csr1 = 0;
                    }

                    SeStartFix(0, 0, 0x1000, 0x1000, 1);
                }
            }
        }
        else if (*csr0 == 1)
        {
            albm_wrk.smenu_tmr = 0;
            albm_wrk.mmenu_tmr = 0;

            if (yw2d.pad_lock == 0)
            {
                if (*key_now[4] == 1)
                {
                    *csr1 = 0xff;

                    SeStartFix(3, 0, 0x1000, 0x1000, 1);
                }
                else if (*key_now[6] == 1 || *key_now[5] == 1)
                {
                    if (*csr1 == 0)
                    {
                        DeletePhotoData(*pic);
                        SeStartFix(1, 0, 0x1000, 0x1000, 1);
                        if (*pic > pfile_wrk.pic_num - 1)
                        {
                            *pic = pfile_wrk.pic_num - 1;

                            if (*list_top != 0)
                            {
                                (*list_top)--;
                            }
                        }
                        else if (pfile_wrk.pic_num - *list_top < 6 && *list_top != 0)
                        {
                            (*list_top)--;
                        }
                    }
                    else
                    {
                        SeStartFix(3, 0, 0x1000, 0x1000, 1);
                    }

                    *csr0 = 0xff;
                    *csr1 = 0xff;
                }
                else if (*key_now[2] == 1 || Ana2PadDirCnt(3) == 1)
                {
                    *csr1 = 1 - *csr1;

                    SeStartFix(0, 0, 0x1000, 0x1000, 1);
                }
                else if (*key_now[3] == 1 || Ana2PadDirCnt(1) == 1)
                {
                    *csr1 = 1 - *csr1;

                    SeStartFix(0, 0, 0x1000, 0x1000, 1);
                }
            }
        }
    }
}

static void IngameAlbumMenuDsp(u_char *csr0, u_char *csr1, u_char *pic, u_char *mode)
{
    short int mmx;
    short int mmy;
    short int smx;
    short int smy;
    short int mnx;

    if (ingame_wrk.clear_count != 0)
    {
        if (*csr0 == 1)
        {
            if (*csr1 != 0xff)
            {
                DrawMessageBox(0xa000, 26.0f, 163.0f, 585.0f, 80.0f, 0x80);

                if (pfile_wrk.pic[*pic].status & 0x2)
                {
                    PutStringYW(33, 2, 48, 179, 0x808080, 0x80, 0x5000, 0);
                }
                else
                {
                    PutStringYW(33, 1, 48, 179, 0x808080, 0x80, 0x5000, 0);
                }

                PutStringYW(33, 4, 48, 205, 0x808080, 0x80, 0x5000, 0);

#ifdef BUILD_EU_VERSION
                YesNoCrslOKR(0xa000, *csr1 * 132 + 178, 203.0f, 0x808080, 128.0f, 2.0f);
#else
                YesNoCrslOKR(0xa000, *csr1 * 132 + 178, 201.0f, 0x808080, 128.0f, 2.0f);
#endif

                return;
            }
        }

        switch (*mode)
        {
        case 0:
            mnx = (*pic & 7) * 72 + 35;

            if ((*pic & 7) < 4)
            {
                mmx = mnx + 72;
                mmy = 54;
                smx = mmx + 120;
                smy = mmy;
            }
            else
            {
                mmx = mnx - 140;
                mmy = 54;
                smx = mmx - 120;
                smy = mmy;
            }
        break;
        case 1:
            mmx = 485;
            mmy = 54;
            smx = mmx - 120;
            smy = mmy;
        break;
        case 2:
            mmx = 385;
            mmy = 54;
            smx = mmx - 120;
            smy = mmy;
        break;
        }

        DspMainMenu(*csr0, *csr1, *mode, mmx, mmy, 0x80);
        DspSideMenu(*csr0, *csr1, *mode, smx, smy, 0x80);

        return;
    }

    if (*csr0 == 1)
    {
        if (*csr1 != 0xff)
        {
            DrawMessageBox(0xa000, 26.0f, 163.0f, 585.0f, 80.0f, 0x80);

            if (pfile_wrk.pic[*pic].status & 0x2)
            {
                PutStringYW(33, 2, 48, 179, 0x808080, 0x80, 0x5000, 0);
            }
            else
            {
                PutStringYW(33, 1, 48, 179, 0x808080, 0x80, 0x5000, 0);
            }

            PutStringYW(33, 4, 48, 205, 0x808080, 0x80, 0x5000, 0);

            YesNoCrslOKR(0xa000, *csr1 * 132 + 184, 203.0f, 0x808080, 128.0f, 2.0f);

            return;
        }
    }

    switch (*mode)
    {
    case 0:
        mnx = (*pic & 7) * 72 + 35;

        if ((*pic & 7) < 4)
        {
            mmx = mnx + 72;
            mmy = 54;
        }
        else
        {
            mmx = mnx - 140;
            mmy = 54;
        }
    break;
    case 2:
        mmx = 385;
        mmy = 54;
    break;
    }

    DspMainMenu(*csr0, *csr1, *mode, mmx, mmy, 0x80);
}

void IngameAlbumDispPhotoLarge(u_char csr)
{
    return;
}

void IngameAlbumDisp(u_char csr, u_char csr_top, u_char mode, char is_outgame)
{
    AlbmCntInit();
    AlbmModeInOut(is_outgame);

    if (yw2d.inn_mode_cnt != 10)
    {
        DrawAlbum(csr, csr_top, mode, 0);

        if (mode == 2)
        {
            DispCaption(11, yw2d.io_a[4]);
        }
        else
        {
            DispCaption(10, yw2d.io_a[4]);
        }
    }
    else
    {
        DrawAlbum(csr, csr_top, mode, 2);
    }

    AlbmModeInOut2(is_outgame);

    if (pfile_wrk.pic_num != 0)
    {
        if (yw2d.io_a[5] != 0.0f)
        {
            if (ingame_wrk.clear_count != 0)
            {
                switch (mode)
                {
                case 0:
                    PhotOnlyAll(csr, yw2d.io_x[5], yw2d.io_y[5], yw2d.io_a[5]);
                break;
                case 1:
                    PhotOnlyLst(csr, yw2d.io_x[5], yw2d.io_y[5], yw2d.io_a[5]);
                break;
                case 2:
                    PhotOnlyBig(csr, yw2d.io_x[5], yw2d.io_y[5], yw2d.io_a[5]);
                break;
                }
            }
            else
            {
                switch (mode)
                {
                case 0:
                    PhotOnlyAll(csr, yw2d.io_x[5], yw2d.io_y[5], yw2d.io_a[5]);
                break;
                case 2:
                    PhotOnlyBig(csr, yw2d.io_x[5], yw2d.io_y[5], yw2d.io_a[5]);
                break;
                }
            }
        }
        if (yw2d.out_mode_cnt == 12)
        {
            BgFusumaYW(0x606060, yw2d.io_x[0], yw2d.io_a[0], 0xffff7f0);

            DrawAlbum(csr, csr_top, mode, 1);
        }
    }

    AlbmCntRenew();
}

static void DrawAlbum(u_char csr, u_char csr_top, u_char mode, u_char str)
{
    DspMenuTitle(yw2d.io_x[1], yw2d.io_y[1], yw2d.io_a[1], 0x32, 3);
    if (pfile_wrk.pic_num != 0)
    {
        if (ingame_wrk.clear_count != 0)
        {
            switch (mode)
            {
            case 0:
                DspNotice(yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4], csr + 1, pfile_wrk.pic_num, 1);
                DspSumnale(csr, yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);
                DspDetail(csr, yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4], str);
            break;
            case 1:
                DspNotice(yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4], csr + 1, pfile_wrk.pic_num, 1);
                DspDetail(csr, yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4], str);
                DspPhtLst(csr, csr_top, yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);
            break;
            case 2:
                DspNotice(yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4], csr + 1, pfile_wrk.pic_num, 0);
                DspFilmCut(csr, yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);
                DspCherry(yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);
                DspBtmDtl(csr, yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);
                DspSideSmnl(csr, yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);
            break;
            }
        }
        else
        {
            switch (mode)
            {
            case 0:
                DspNotice(yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4], csr + 1, pfile_wrk.pic_num, 1);
                DspSumnale(csr, yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);
                DspDetail(csr, yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4], str);
            break;
            case 2:
                DspNotice(yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4], csr + 1, pfile_wrk.pic_num, 0);
                DspFilmCut(csr, yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);
                DspCherry(yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);
                DspBtmDtl(csr, yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);
                DspSideSmnl(csr, yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);
            break;
            }
        }
    }
    else
    {
        DrawMessageBox(0x14000, 170.0f, 210.0f, 300.0f, 60.0f, yw2d.io_a[4]);

        PutStringYW(33, 0, 215, 228, 0x808080, yw2d.io_a[4], 0xf000, 0);
    }
}

static void PhotOnlyAll(u_char csr, short int pos_x, short int pos_y, short int alpha)
{
    int i;
    int pic_num;
    int rgb;
    u_short x_i;
    u_short y_i;
    u_short xx;
    u_short yy;

    DrawSPhotoFromSmallPhotoArea(0, 0x7f000, 0, 0, 0, 0);

    pic_num = pfile_wrk.pic_num;

    for (i = 0; i < 24; i++)
    {
        x_i = (i % 8) * 72 + 35 + pos_x;
        y_i = (i / 8) * 57 + 85 + pos_y;

        if (i < pic_num)
        {
            DrawSPhotoFromSmallPhotoArea(pfile_wrk.pic[i].adr_no, 0x28000, 0, x_i + 2, y_i + 6, alpha);

            if (pfile_wrk.pic[i].status & 0x2)
            {
                PutSpriteYW(PHT_SAM_ACT_FRAME2, PHT_SAM_ACT_FRAME2, x_i + 3, y_i + 2, 0.0f, 0xc0c0c0, (int)((alpha * 4) / 5), 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
            }
        }
    }

    xx = (csr & 7) * 72 + 35 + pos_x;
    yy = (csr >> 3) * 57 + 85 + pos_y;

    FlashStarYW(&flsh[2], 64, 128, 120, 0);

    CLRtoRGB(&rgb, flsh[2].alpha, flsh[2].alpha, flsh[2].alpha);

    PutSpriteYW(PHT_SAM_BLANK, PHT_SAM_BLANK, xx + 9, yy + 7, 0.0f, rgb, alpha * 0.5, 1.25f, 1.25f, 0, 0xff, 1, 1, 0); // 0.5 is double!!

    if (csr < pic_num)
    {
        if (pfile_wrk.pic[csr].adr_no != albm_wrk.dsp_pct_bak)
        {
            albm_wrk.dsp_pct_bak = pfile_wrk.pic[csr].adr_no;

            ReqPhotoExpand();
        }

        PolySquareYW(76.0f, 272.0f, 216, 144, 0, (float)alpha, 1.0f, 1.0f, 0x29000, 0, 0, 0);

        UncompressPhoto(pfile_wrk.pic[csr].adr_no);

        if (CheckPhotoExpandEnd() != 0)
        {
            DrawPhotoFromPhotoWrk(pfile_wrk.pic[csr].adr_no, 0x28000, 0, pos_x + 76, pos_y + 272, 216, 144, alpha);
        }
    }

    IngameAlbumMenuDsp(&ig_menu_wrk.csr[5], ig_menu_wrk.csr_bak, &ig_menu_wrk.csr[1], &ig_menu_wrk.csr[2]);

    DrawSPhotoFromSmallPhotoArea(0, 0x7f000, 0, 0, 0, 0);
}

static void PhotOnlyLst(u_char csr, short int pos_x, short int pos_y, short int alpha)
{
    int pic_num;

    DrawSPhotoFromSmallPhotoArea(0, 0x7f000, 0, 0, 0, 0);

    pic_num = pfile_wrk.pic_num;

    if (csr < pic_num)
    {
        if (pfile_wrk.pic[csr].adr_no != albm_wrk.dsp_pct_bak)
        {
            albm_wrk.dsp_pct_bak = pfile_wrk.pic[csr].adr_no;

            ReqPhotoExpand();
        }

        PolySquareYW(76.0f, 272.0f, 216, 144, 0, alpha, 1.0f, 1.0f, 0x29000, 0, 0, 0);
        UncompressPhoto(pfile_wrk.pic[csr].adr_no);

        if (CheckPhotoExpandEnd() != 0)
        {
            DrawPhotoFromPhotoWrk(pfile_wrk.pic[csr].adr_no, 0x28000, 0, pos_x + 76, pos_y + 272, 216, 144, alpha);
        }
    }

    IngameAlbumMenuDsp(&ig_menu_wrk.csr[5], ig_menu_wrk.csr_bak, &ig_menu_wrk.csr[1], &ig_menu_wrk.csr[2]);
    DrawSPhotoFromSmallPhotoArea(0, 0x7f000, 0, 0, 0, 0);
}

static void PhotOnlyBig(u_char csr, short int pos_x, short int pos_y, short int alpha)
{
    int pic_num;
    int pht_no;

    DrawSPhotoFromSmallPhotoArea(0, 0x7f000, 0, 0, 0, 0);

    pic_num = pfile_wrk.pic_num;

    if (pfile_wrk.pic[csr].adr_no != albm_wrk.dsp_pct_bak)
    {
        albm_wrk.dsp_pct_bak = pfile_wrk.pic[csr].adr_no;

        ReqPhotoExpand();
    }

    UncompressPhoto(pfile_wrk.pic[csr].adr_no);

    PolySquareYW(127.0f, 76.0f, 384, 0xff, 0, alpha, 1.0f, 1.0f, 0x2e000, 0, 0, 0);

    if (CheckPhotoExpandEnd() != 0)
    {
        DrawPhotoFromPhotoWrk(pfile_wrk.pic[csr].adr_no, 0x2d000, 0, pos_x + 127, pos_y + 76, 384, 255, alpha);

        if (pfile_wrk.pic[csr].status & 0x2)
        {
            PutSpriteYW(PHT_ZOOM_SEALING1, PHT_ZOOM_SEALING4, pos_x, pos_y, 0.0f, 0xc0c0c0, (int)((alpha * 4) / 5), 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
        }
    }

    if (pic_num > 1)
    {
        if (pic_num - 1 > csr)
        {
            pht_no = csr + 1;
        }
        else
        {
            pht_no = 0;
        }

        DrawSPhotoFromSmallPhotoArea2(pfile_wrk.pic[pht_no].adr_no, 0x28000, 0, pos_x + 0x222, pos_y + 189, 48, 32, alpha);

        if (csr != 0)
        {
            pht_no = csr - 1;
        }
        else
        {
            pht_no = pic_num - 1;
        }

        DrawSPhotoFromSmallPhotoArea2(pfile_wrk.pic[pht_no].adr_no, 0x28000, 0, pos_x + 0x32, pos_y + 189, 48, 32, alpha);
    }

    IngameAlbumMenuDsp(&ig_menu_wrk.csr[5], ig_menu_wrk.csr_bak, &ig_menu_wrk.csr[1], &ig_menu_wrk.csr[2]);

    DrawSPhotoFromSmallPhotoArea(0, 0x7f000, 0, 0, 0, 0);
}

u_char* GetSubjectNameAddr(u_short kind, u_short no, u_short show)
{
    int *addr;

    switch(kind)
    {
    case 0:
        return (u_char *)GetIngameMSGAddr(0x21, 3);
    break;
    case 1:
        addr = (int *)(BASE_ADDRESS + 31 * 4);
        addr = (int *)(Get4Byte((u_char *)addr) + BASE_ADDRESS + show * 4);
        addr = (int *)(Get4Byte((u_char *)addr) + BASE_ADDRESS + no * 4);
        addr = (int *)(Get4Byte((u_char *)addr) + BASE_ADDRESS);

        return (u_char *)addr;
    break;
    case 2:
        addr = (int *)(BASE_ADDRESS + 31 * 4);
        addr = (int *)(Get4Byte((u_char *)addr) + BASE_ADDRESS + (show + 5) * 4);
        addr = (int *)(Get4Byte((u_char *)addr) + BASE_ADDRESS + no * 4);
        addr = (int *)(Get4Byte((u_char *)addr) + BASE_ADDRESS);

        return (u_char *)addr;
    break;
    case 3:
        addr = (int *)(BASE_ADDRESS + 31 * 4);
        addr = (int *)(Get4Byte((u_char *)addr) + BASE_ADDRESS + (show + 10) * 4);
        addr = (int *)(Get4Byte((u_char *)addr) + BASE_ADDRESS + no * 4);
        addr = (int *)(Get4Byte((u_char *)addr) + BASE_ADDRESS);

        return (u_char *)addr;
    break;
    case 4:
        addr = (int *)(BASE_ADDRESS + 32 * 4);
        addr = (int *)(Get4Byte((u_char *)addr) + BASE_ADDRESS + no * 4);
        addr = (int *)(Get4Byte((u_char *)addr) + BASE_ADDRESS);

        return (u_char *)addr;
    break;
    case 5:
        if (no < 0x80) {
            addr = (int *)(BASE_ADDRESS + 37 * 4);
            addr = (int *)(Get4Byte((u_char *)addr) + BASE_ADDRESS + no * 4);
            addr = (int *)(Get4Byte((u_char *)addr) + BASE_ADDRESS);
        }
        else
        {
            addr = (int *)(BASE_ADDRESS + 38 * 4);
            addr = (int *)(Get4Byte((u_char *)addr) + BASE_ADDRESS + (no - 0x80) * 4);
            addr = (int *)(Get4Byte((u_char *)addr) + BASE_ADDRESS);
        }

        return (u_char *)addr;
    break;
    case 6:
        return (u_char *)GetIngameMSGAddr(0x21, 3);
    break;
    }

    // missing return!
}

static void IngameAlbumSort()
{
    int i;
    int j;
    int tmp;
    PICTURE_WRK tmp_pic;
    int k;

    switch (pfile_wrk.sort_key)
    {
    case 0:
        for (i = 0; i < pfile_wrk.pic_num - 1; i++)
        {
            tmp_pic = pfile_wrk.pic[i];
            tmp = i;

            for (j = i+1; j < pfile_wrk.pic_num; j++)
            {
                if (TimeIsMoney(tmp_pic.time.year) <  TimeIsMoney(pfile_wrk.pic[j].time.year)) {
                    tmp_pic = pfile_wrk.pic[j];
                    tmp = j;
                }
                else if (TimeIsMoney(tmp_pic.time.year) == TimeIsMoney(pfile_wrk.pic[j].time.year))
                {
                    if (TimeIsMoney(tmp_pic.time.month) < TimeIsMoney(pfile_wrk.pic[j].time.month)) {
                        tmp_pic = pfile_wrk.pic[j];
                        tmp = j;
                    }
                    else if (TimeIsMoney(tmp_pic.time.month) == TimeIsMoney(pfile_wrk.pic[j].time.month))
                    {
                        if (TimeIsMoney(tmp_pic.time.day) < TimeIsMoney(pfile_wrk.pic[j].time.day))
                        {
                            tmp_pic = pfile_wrk.pic[j];
                            tmp = j;
                        }
                        else if (TimeIsMoney(tmp_pic.time.day) == TimeIsMoney(pfile_wrk.pic[j].time.day))
                        {
                            if (TimeIsMoney(tmp_pic.time.hour) < TimeIsMoney(pfile_wrk.pic[j].time.hour))
                            {
                                tmp_pic = pfile_wrk.pic[j];
                                tmp = j;
                            }
                            else if (TimeIsMoney(tmp_pic.time.hour) == TimeIsMoney(pfile_wrk.pic[j].time.hour))
                            {
                                if (TimeIsMoney(tmp_pic.time.minute) < TimeIsMoney(pfile_wrk.pic[j].time.minute))
                                {
                                    tmp_pic = pfile_wrk.pic[j];
                                    tmp = j;
                                }
                                else if (TimeIsMoney(tmp_pic.time.minute) == TimeIsMoney(pfile_wrk.pic[j].time.minute))
                                {
                                    if (TimeIsMoney(tmp_pic.time.second) < TimeIsMoney(pfile_wrk.pic[j].time.second))
                                    {
                                        tmp_pic = pfile_wrk.pic[j];
                                        tmp = j;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            pfile_wrk.pic[tmp] = pfile_wrk.pic[i];
            pfile_wrk.pic[i] = tmp_pic;
        }
    break;
    case 1:
        for (i = 0; i < pfile_wrk.pic_num - 1; i++)
        {
            tmp_pic = pfile_wrk.pic[i];
            tmp = i;

            for (j = i+1; j < pfile_wrk.pic_num; j++)
            {
                if (tmp_pic.score < pfile_wrk.pic[j].score)
                {
                    tmp_pic = pfile_wrk.pic[j];
                    tmp = j;
                }
            }

            pfile_wrk.pic[tmp] = pfile_wrk.pic[i];
            pfile_wrk.pic[i] = tmp_pic;
        }
    break;
    case 2:
        for (i = 0; i < pfile_wrk.pic_num - 1; i++)
        {
            if (pfile_wrk.pic[i].status & 0x2)
            {
                continue;
            }

            for (j = i+1; j < pfile_wrk.pic_num; j++)
            {
                if (pfile_wrk.pic[j].status & 0x2)
                {
                    tmp_pic = pfile_wrk.pic[j];
                    pfile_wrk.pic[j] = pfile_wrk.pic[i];
                    pfile_wrk.pic[i] = tmp_pic;

                    break;
                }
            }
        }
    break;
    }

    if (pfile_wrk.skey_bak != 0)
    {
        for (i = 0, k = 1; i < pfile_wrk.pic_num - k; i++, k=i+1)
        {
            tmp_pic = pfile_wrk.pic[i];
            pfile_wrk.pic[i] = pfile_wrk.pic[pfile_wrk.pic_num-i-1];
            pfile_wrk.pic[pfile_wrk.pic_num-i-1] = tmp_pic;
        }
    }
}

static void DspMainMenu(u_char csr0, u_char csr1, u_char mode, short int pos_x, short int pos_y, short int alpha)
{
    u_short anm;

    if (csr0 != 0xff)
    {
        if (albm_wrk.mmenu_tmr < 15)
        {
            albm_wrk.mmenu_tmr++;
        }
    }
    else
    {
        if (albm_wrk.mmenu_tmr != 0)
        {
            albm_wrk.mmenu_tmr--;
        }
    }

    if (csr1 == 0xff)
    {
        FlashStarYW(flsh, 64, 128, 120, 0);
    }
    else
    {
        FlashStarYW(flsh, 64, 128, 120, 0xff);
    }

    anm = ((15 - albm_wrk.mmenu_tmr) * 300) / 15.0f;

    if (ingame_wrk.clear_count != 0)
    {
        if (mode != 2)
        {
            DspMenuWin(0, csr0, mode, pos_x, pos_y, flsh[0].alpha, alpha, anm);
        }
        else
        {
            DspMenuWin(1, csr0, mode, pos_x, pos_y, flsh[0].alpha, alpha, anm);
        }
    }
    else
    {
        DspMenuWin(1, csr0, mode, pos_x, pos_y, flsh[0].alpha, alpha, anm);
    }
}

static void DspSideMenu(u_char csr0, u_char csr1, u_char mode, short int pos_x, short int pos_y, short int alpha)
{
    u_short anm;

    if (csr0 != 0xff && csr1 != 0xff)
    {
        albm_wrk.menu_csr_bak = csr0;

        FlashStarYW(&flsh[1], 64, 128, 120, 0);

        if (albm_wrk.smenu_tmr < 0xf)
        {
            albm_wrk.smenu_tmr++;
        }
    }
    else
    {
        FlashStarYW(&flsh[1], 64, 128, 120, 0xff);

        if (albm_wrk.smenu_tmr != 0)
        {
            albm_wrk.smenu_tmr--;
        }
        else
        {
            albm_wrk.menu_csr_bak = 0xff;
        }
    }
    anm = ((15 - albm_wrk.smenu_tmr) * 300) / 15.0f;

    if (albm_wrk.menu_csr_bak == 2)
    {
        DspMenuWin(2, csr1, mode, pos_x, pos_y, flsh[1].alpha, alpha, anm);
    }
    else if (albm_wrk.menu_csr_bak == 3)
    {
        DspMenuWin(3, csr1, mode, pos_x, pos_y + 38, flsh[1].alpha, alpha, anm);
    }
}

static void DspMenuWin(u_char type, u_char csr, u_char mode, short int pos_x, short int pos_y, float flash, short int alpha, u_short anm)
{
    int i;
    short int pos_v;
    short int pos_h;
    short int alp1;
    short int alp2;
    short int alp3;
    short int mode_char;
    short int char_posy;
    int rgb;
    int j; // not in STAB
    int y_off_1;
    int y_off_2;
    int v_off;
    float fvar;

    y_off_1 = 37;
    y_off_2 = 67;
    v_off = 86;

    fvar = 0.6803279f;

    if (anm != 0)
    {
        if (anm >= 200)
        {
            pos_h = ((anm - 200) * 25) / 100.0f;
            pos_v = 30;
            alp1 = (alpha * (300 - anm)) / 100.0f;
            alp2 = alp3 = 0;
        }
        else if (anm >= 100)
        {
            pos_h = 0;
            pos_v = ((anm - 100) * 30) / 100.0f;
            alp1 = alpha;
            alp2 = (alpha * (200 - anm)) / 100.0f;
            alp3 = 0;
        }
        else
        {
            pos_h = 0;
            pos_v = 0;
            alp1 = alp2 = alpha;
            alp3 = (alpha * (100 - anm)) / 100.0f;
        }
    }
    else
    {
        pos_h = pos_v = 0;
        alp1 = alp2 = alp3 = alpha;
    }

    CLRtoRGB(&rgb, flash, flash, flash);

    if (type == 0)
    {
        PutSpriteYW(MNU_SEL_BACK1, MNU_SEL_BACK4, pos_x - 33, pos_y - 102, 0.0f, 0x808080, alp3, fvar, 1.0f, 0, 0x14000, 1, 0, 0);

        for (i = 0; i < 3; i++)
        {
            PutSpriteYW(MNU_MAIN_BASE_HOL2, MNU_MAIN_BASE_HOL2_B, pos_x + 75 + pos_h, pos_y + 29 + i * 76, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }

        for (i = 0; i < 2; i++)
        {
            PutSpriteYW(MNU_MAIN_BASE_HOL2, MNU_MAIN_BASE_HOL2_B, pos_x + 75 - pos_h, pos_y + y_off_2 + i * 76, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }

        PutSpriteYW(MNU_MAIN_BASE_VAT4, MNU_MAIN_BASE_VAT4_B, pos_x + 17, pos_y + 105 + pos_v, 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(MNU_MAIN_BASE_VAT4, MNU_MAIN_BASE_VAT4_B, pos_x + 108, pos_y + 105 - pos_v, 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        if (mode == 0)
        {
            mode_char = 300;
        }
        else if (mode == 1)
        {
            mode_char = 299;
        }

        switch (csr)
        {
        case 0:
            PutSpriteYW(PHT_SAVE_ACT, PHT_SAVE_ACT, pos_x, pos_y, 0.0f, rgb, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(PHT_DELT_ACT, PHT_DELT_ACT, pos_x, pos_y, 0.0f, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(mode_char, mode_char, pos_x, pos_y, 0.0f, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(PHT_SORT_ACT, PHT_SORT_ACT, pos_x, pos_y, 0.0f, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        break;
        case 1:
            PutSpriteYW(PHT_SAVE_ACT, PHT_SAVE_ACT, pos_x, pos_y, 0.0f, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(PHT_DELT_ACT, PHT_DELT_ACT, pos_x, pos_y, 0.0f, rgb, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(mode_char, mode_char, pos_x, pos_y, 0.0f, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(PHT_SORT_ACT, PHT_SORT_ACT, pos_x, pos_y, 0.0f, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        break;
        case 2:
            PutSpriteYW(PHT_SAVE_ACT, PHT_SAVE_ACT, pos_x, pos_y, 0.0f, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(PHT_DELT_ACT, PHT_DELT_ACT, pos_x, pos_y, 0.0f, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(mode_char, mode_char, pos_x, pos_y, 0.0f, rgb, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(PHT_SORT_ACT, PHT_SORT_ACT, pos_x, pos_y, 0.0f, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        break;
        case 3:
            PutSpriteYW(PHT_SAVE_ACT, PHT_SAVE_ACT, pos_x, pos_y, 0.0f, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(PHT_DELT_ACT, PHT_DELT_ACT, pos_x, pos_y, 0.0f, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(mode_char, mode_char, pos_x, pos_y, 0.0f, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(PHT_SORT_ACT, PHT_SORT_ACT, pos_x, pos_y, 0.0f, rgb, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        break;
        }

        CmnCursol(pos_x + 36, csr * 38 + (pos_y + y_off_1), 82, 30, flash, alp3, 0x5000);
    }
    else if (type == 1)
    {
        PutSpriteYW(MNU_SEL_BACK1, MNU_SEL_BACK2, pos_x - 33, pos_y - 102, 0.0f, 0x808080, alp3, fvar, 1.0f, 0, 0x14000, 1, 0, 0);

        PutSpriteYW(MNU_MAIN_BASE_HOL2, MNU_MAIN_BASE_HOL2_B, pos_x + 75 + pos_h, pos_y + 29, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(MNU_MAIN_BASE_HOL2, MNU_MAIN_BASE_HOL2_B, pos_x + 75 - pos_h, pos_y + 67, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(MNU_MAIN_BASE_HOL2, MNU_MAIN_BASE_HOL2_B, pos_x + 75 + pos_h, pos_y + 105, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(MNU_MAIN_BASE_VAT2, MNU_MAIN_BASE_VAT2_B, pos_x + 17, pos_y + 67 + pos_v, 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(MNU_MAIN_BASE_VAT2, MNU_MAIN_BASE_VAT2_B, pos_x + 108, pos_y + 67 - pos_v, 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        switch (csr)
        {
        case 0:
            PutSpriteYW(PHT_SAVE_ACT, PHT_SAVE_ACT, pos_x, pos_y, 0.0f, rgb, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(PHT_DELT_ACT, PHT_DELT_ACT, pos_x, pos_y, 0.0f, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff,1, 0, 0);
        break;
        case 1:
            PutSpriteYW(PHT_SAVE_ACT, PHT_SAVE_ACT, pos_x, pos_y, 0.0f, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(PHT_DELT_ACT, PHT_DELT_ACT, pos_x, pos_y, 0.0f, rgb, alp3, 1.0f, 1.0f, 0, 0xff, 1 , 0, 0);
        break;
        }

        CmnCursol(pos_x + 36, csr * 38 + (pos_y + y_off_1), 82, 30, flash, alp3, 0x5000);
    }
    else if (type == 2)
    {
        PutSpriteYW(MNU_SEL_BACK1, MNU_SEL_BACK3, pos_x - 33, pos_y - 102, 0.0f, 0x808080, alp3, fvar, 1.0f, 0, 0x14000, 1, 0, 0);

        for (i = 0; i < 2; i++)
        {
            PutSpriteYW(MNU_MAIN_BASE_HOL2, MNU_MAIN_BASE_HOL2_B, pos_x + 75 + pos_h, pos_y + 29 + i * 76, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(MNU_MAIN_BASE_HOL2, MNU_MAIN_BASE_HOL2_B, pos_x + 75 - pos_h, pos_y + y_off_2 + i * 76, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }

        PutSpriteYW(MNU_MAIN_BASE_VAT3, MNU_MAIN_BASE_VAT3_B, pos_x + 17, pos_y + (pos_v + v_off), 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(MNU_MAIN_BASE_VAT3, MNU_MAIN_BASE_VAT3_B, pos_x + 108, pos_y - (pos_v - v_off), 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        for (i = 0; i < 3; i++)
        {
            j = i + 1;

            mode_char = (mode + j) % 3 + 299;
            char_posy = pos_y + i * 38;

            if (i == csr)
            {
                PutSpriteYW(mode_char, mode_char, pos_x, char_posy, 0.0f, rgb, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }
            else if (i == 2)
            {
                PutSpriteYW(mode_char, mode_char, pos_x, char_posy, 0.0f, 0x404040, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }
            else
            {
                PutSpriteYW(mode_char, mode_char, pos_x, char_posy, 0.0f, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0 , 0);
            }
        }

        CmnCursol(pos_x + 36, csr * 38 + (pos_y + y_off_1), 82, 30, flash, alp3, 0x5000);
    }
    else if (type == 3)
    {
        PutSpriteYW(MNU_SEL_BACK1, MNU_SEL_BACK3, pos_x - 33, pos_y - 102, 0.0f, 0x808080, alp3, fvar, 1.0f, 0, 0x14000, 1, 0, 0);

        for (i = 0; i < 2; i++)
        {
            PutSpriteYW(MNU_MAIN_BASE_HOL2, MNU_MAIN_BASE_HOL2_B, pos_x + 75 + pos_h, pos_y + 29 + i * 76, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(MNU_MAIN_BASE_HOL2, MNU_MAIN_BASE_HOL2_B, pos_x + 75 - pos_h, pos_y + y_off_2 + i * 76, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }

        PutSpriteYW(MNU_MAIN_BASE_VAT3, MNU_MAIN_BASE_VAT3_B, pos_x + 17, pos_y + (pos_v + v_off), 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(MNU_MAIN_BASE_VAT3, MNU_MAIN_BASE_VAT3_B, pos_x + 108, pos_y - (pos_v - v_off), 0.0f, 0x808080, alp2, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        for (i = PHT_TIME_ACT; i < PHT_ALL_ACT; i++)
        {
            if (i == csr + PHT_TIME_ACT)
            {
                PutSpriteYW(i, i, pos_x, pos_y, 0.0f, rgb, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }
            else
            {
                PutSpriteYW(i, i, pos_x, pos_y, 0.0f, 0x808080, alp3, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }
        }

        CmnCursol(pos_x + 36, csr * 38 + (pos_y + y_off_1), 82, 30, flash, alp3, 0x5000);
    }
}

static void DspDetail(u_char csr, short int pos_x, short int pos_y, short int alpha, u_char dsp)
{
    int i;
    DISP_STR disp_str;
    u_char obj_num;
    u_short obj_py[4][3] = {
        { 24, 24, 24 },
        { 24, 24, 24 },
        { 12, 36, 24 },
        {  0, 24, 48 }
    };
    int width;

    if (dsp == 0 || dsp == 1)
    {
        CmnWindow(5, pos_x, pos_y, alpha, 0x80);

        PutSpriteYW(PHT_FNT_JOHO, PHT_FNT_JOHO, 15.0f, 332.0f, -90.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(PHT_MNU_FNT_TARGET, PHT_MNU_FNT_POINT, pos_x, pos_y, 0.0f, 0x505050, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        if (csr < pfile_wrk.pic_num)
        {
            PutSpriteYW(PHT_PHOTO_EDGE_HU, PHT_PHOTO_EDGE_HD, 70.0f, 267.0f, 0.0f, 0x808080, alpha, 18.833334f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(PHT_PHOTO_EDGE_VL, PHT_PHOTO_EDGE_VR, 70.0f, 267.0f, 0.0f, 0x808080, alpha, 1.0f, 12.0f, 0, 0xff, 1, 0, 0);

            if (pfile_wrk.pic[csr].status & 2)
            {
                PutSpriteYW(PHT_PICT_SEALMARK1, PHT_PICT_SEALMARK4, pos_x, pos_y, 0.0f, 0xc0c0c0, (int)((alpha * 4) / 5), 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
            }
        }
    }

    if (1) { char c = ~c; } // HACK: fixes stack swap

    if (dsp == 0 || dsp == 2)
    {
        if (csr < pfile_wrk.pic_num)
        {
            obj_num = 0;

            for (i = 0; i < 3; i++)
            {
                if (pfile_wrk.pic[csr].subject[i][0] != 0)
                {
                    obj_num++;
                }
            }

            if (obj_num != 0)
            {
                for (i = 0; i < obj_num; i++)
                {
                    if (pfile_wrk.pic[csr].subject[i][0] != 0)
                    {
                        ig_menu_str.str = GetSubjectNameAddr(
                            pfile_wrk.pic[csr].subject[i][0],
                            pfile_wrk.pic[csr].subject[i][1],
                            pfile_wrk.pic[csr].msn_no);

                        width = GetStrWidth(ig_menu_str.str);

                        CopyStrDToStr(&disp_str, &ig_menu_str);

                        disp_str.pos_x = (pos_x + 482) - (width / 2);
                        disp_str.pos_y = obj_py[obj_num][i] + 267 + pos_y;

                        disp_str.r = 0x80;
                        disp_str.g = 0x80;
                        disp_str.b = 0x80;

                        disp_str.alpha = alpha;
                        disp_str.pri = 0x28000;

                        SetMessageV2(&disp_str);
                    }
                }
            }
            else
            {
                PutStringYW(33, 3, (u_short)pos_x + 452, obj_py[0][0] + 267 + pos_y, 0x808080, alpha, 0x28000, 0);
            }
        }

        PutDate(pfile_wrk.pic + csr, pos_x + 0x172, pos_y + 0x154, alpha, 0x28000);

        PutStringYW(30, pfile_wrk.pic[csr].room, pos_x + 482, pos_y + 371, 0x808080, alpha, 0x28000, 1);
        PutNumberYW(0, pfile_wrk.pic[csr].score, pos_x + 421, pos_y + 399, 1.0f, 1.0f, 0x808080, alpha, 0x28000, 7, 0);
    }
}

static void DspNotice(short int pos_x, short int pos_y, short int alpha, u_char now, u_char max, u_char btn)
{
    return;
}

static void DspSumnale(u_char csr, short int pos_x, short int pos_y, short int alpha)
{
    int i;
    int pic_num;
    u_short pic_x;
    u_short pic_y;

    for (i = 0; i < 24; i++)
    {
        pic_x = (i % 8) * 72 + 35 + pos_x;
        pic_y = (i / 8) * 57 + 85 + pos_y;

        PutSpriteYW(PHT_SAM_FRAME, PHT_SAM_FRAME, pic_x, pic_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f , 0, 0xff, 1, 0, 0);

        PutNumberYW(5, i+1, pic_x + 31, pic_y + 34, 1.0f, 1.0f, 0x808080, alpha, 0x2d000, 2, 1);
    }

    PutSpriteYW(PHT_ALL_FRAME_01, PHT_ALL_FRAME_12, pos_x + 24, pos_y + 75, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    pic_num = pfile_wrk.pic_num;

    for (i = 0; i < 24; i++)
    {
        pic_x = (i % 8) * 72 + 35 + pos_x;
        pic_y = (i / 8) * 57 + 85 + pos_y;

        if (i < pic_num)
        {
            PutSpriteYW(PHT_SAM_SEAT2, PHT_SAM_SEAT2, pic_x, pic_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }
    }
}

static void DspPhtLst(u_char csr, u_char csr_top, short int pos_x, short int pos_y, short int alpha)
{
    int i;
    int pic_num;
    int y_off_1;
    int y_off_2;
    int y_off_4;
    int y_off_3;
    int v_off_2;
    int v_off_1;
    u_char csr_top_1; // HACK: fixes stack swap
    u_int csr_top_2; // HACK: fixes stack swap

    y_off_1 = 94;
    y_off_2 = 95;
    y_off_3 = 116;
    y_off_4 = 93;
    v_off_1 = 2;
    v_off_2 = 22;

    csr_top_2 = csr_top; // HACK: fixes stack swap
    csr_top_1 = csr_top_2; // HACK: fixes stack swap

    CmnWindow(4, pos_x, pos_y, alpha, 0x80);

    PutSpriteYW(PHT_SCRBAR_BASE_U, PHT_SCRBAR_BASE_D, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PHT_SCRBAR_BASE_M, PHT_SCRBAR_BASE_M, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 13.6f, 0, 0xff, 1, 0, 0);

    if (pfile_wrk.pic_num - csr_top_1 >= 6)
    {
        pic_num = 6;
    }
    else
    {
        pic_num = pfile_wrk.pic_num - csr_top_1;
    }

    SideBar(pfile_wrk.pic_num, 6, 149, csr_top_1, 366, alpha, 0, 0);

    AlbmTrFlsh(pos_x, pos_y, alpha);

    PutSpriteYW(PHT_FNT_HOZ, PHT_FNT_HOZ, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PHT_FNT_HRD, PHT_FNT_HRD, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PHT_FNT_PLC, PHT_FNT_PLC, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PHT_FNT_PNT, PHT_FNT_PNT, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    for (i = 0; i < pic_num; i++)
    {
        if (pfile_wrk.pic[i + csr_top_1].status & 0x2)
        {
            PutStringYW(33, 7, pos_x + 48, i * 24 + (pos_y + y_off_1), 0x808080, alpha, 0x23000, 0);
        }

        PutDate(&pfile_wrk.pic[i + csr_top_1], pos_x + 73, i * 24 + (pos_y + y_off_2), alpha, 0x23000);

#ifdef BUILD_EU_VERSION
        PutStringYW(30, pfile_wrk.pic[i + csr_top_1].room, pos_x + 302, i * 24 + (pos_y + y_off_2) - v_off_1, 0x808080, alpha, 0x23000, 0);
#else
        PutStringYW(30, pfile_wrk.pic[i + csr_top_1].room, pos_x + 307, i * 24 + (pos_y + y_off_2) - v_off_1, 0x808080, alpha, 0x23000, 0);
#endif

        PutNumberYW(3, pfile_wrk.pic[i + csr_top_1].score, pos_x + 497, (i * 24 + (pos_y + y_off_3)) - v_off_2, 1.0f, 1.0f, 0x808080, alpha, 0x23000, 7, 0);
    }

    FlashStarYW(&flsh[3], 64, 96, 90, 0);

    CmnCursol(pos_x + 44, (csr - csr_top_1) * 24 + (pos_y + y_off_4), 572, 26, flsh[3].alpha, alpha, 0x23000);
}

static void DspFilmCut(u_char csr, short int pos_x, short int pos_y, short int alpha)
{
    PutSpriteYW(PHT_ZOOM_DECO_BACK_L1, PHT_ZOOM_DECO_BACK_R3, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PHT_ZOOM_DAISHI_HU, PHT_ZOOM_DAISHI_HD, pos_x, pos_y, 0.0f, 0x808080, alpha, 19.3f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PHT_ZOOM_DAISHI_VL, PHT_ZOOM_DAISHI_VR, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 16.0f, 0, 0xff, 1, 0, 0);
}

static void DspSideSmnl(u_char csr, short int pos_x, short int pos_y, short int alpha)
{
    int pht_no;

    if (pfile_wrk.pic_num > 1)
    {
        if (csr == 0)
        {
            pht_no = pfile_wrk.pic_num - 1;
        }
        else
        {
            pht_no = csr - 1;
        }

        PutSpriteYW(PHT_SAM_SEAT, PHT_SAM_SEAT, pos_x + 46, pos_y + 185, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        if (pfile_wrk.pic[pht_no].status & 0x2)
        {
            PutSpriteYW(PHT_SAM_ACT_FRAME, PHT_SAM_ACT_FRAME, pos_x + 45, pos_y + 183, 0.0f, 0xc0c0c0, (int)((alpha * 4) / 5), 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
        }

        if (csr < pfile_wrk.pic_num - 1)
        {
            pht_no = csr + 1;
        }
        else
        {
            pht_no = 0;
        }

        PutSpriteYW(PHT_SAM_SEAT, PHT_SAM_SEAT, pos_x + 542, pos_y + 185, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        if (pfile_wrk.pic[pht_no].status & 0x2)
        {
            PutSpriteYW(PHT_SAM_ACT_FRAME, PHT_SAM_ACT_FRAME, pos_x + 541, pos_y + 183, 0.0f, 0xc0c0c0, (int)((alpha * 4) / 5), 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
        }
    }
}

static void DspBtmDtl(u_char csr, short int pos_x, short int pos_y, short int alpha)
{
    int i;
    DISP_STR disp_str;
    u_char obj_num;
    u_short obj_py[4][3] = {
        { 24, 24, 24 },
        { 24, 24, 24 },
        { 12, 36, 24 },
        {  0, 24, 48 }
    };
    int width;

    CmnWindow(6, pos_x, pos_y, alpha, 0x80);
    PutSpriteYW(PHT_FNT_JOHO, PHT_FNT_JOHO, 14.0f, 369.0f, -90.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PHT_ZOOM_FNT_TARGET, PHT_ZOOM_FNT_POINT, pos_x, pos_y, 0.0f, 0x505050, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    obj_num = 0;

    for (i = 0; i < 3; i++)
    {
        if (pfile_wrk.pic[csr].subject[i][0] != 0)
        {
            obj_num++;
        }
    }

    if (obj_num != 0)
    {
        for (i = 0; i < obj_num; i++)
        {
            if (pfile_wrk.pic[csr].subject[i][0] != 0)
            {
                ig_menu_str.str = GetSubjectNameAddr(
                    pfile_wrk.pic[csr].subject[i][0],
                    pfile_wrk.pic[csr].subject[i][1],
                    pfile_wrk.pic[csr].msn_no);

                width = GetStrWidth(ig_menu_str.str);

                CopyStrDToStr(&disp_str, &ig_menu_str);

                disp_str.pos_x = (pos_x + 216) - (width / 2);
                disp_str.pos_y = obj_py[obj_num][i] + 347 + pos_y;

                disp_str.r = 0x80;
                disp_str.g = 0x80;
                disp_str.b = 0x80;

                disp_str.alpha = alpha;
                disp_str.pri = 0x28000;

                SetMessageV2(&disp_str);
            }
        }
    }
    else
    {
        PutStringYW(33, 3, (u_short)pos_x + 192, obj_py[0][0] + 347 + pos_y, 0x808080, alpha, 0x28000, 0);
    }
    PutDate(pfile_wrk.pic + csr, pos_x + 386, pos_y + 340, alpha, 0x28000);
    PutStringYW(30, pfile_wrk.pic[csr].room, pos_x + 496, pos_y + 371, 0x808080, alpha, 0x28000, 1);
    PutNumberYW(0, pfile_wrk.pic[csr].score, pos_x + 435, pos_y + 399, 1.0f, 1.0f, 0x808080, alpha, 0x28000, 7, 0);
}

static void DspCherry(short int pos_x, short int pos_y, short int alpha)
{
    int rgb;

    if (pfile_wrk.pic_num > 1)
    {
        PutSpriteYW(PHT_ZOOM_FNT_BACK, PHT_ZOOM_FNT_NEXT, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        FlashStarYW(&flsh[4], 192, 64, 60, 0);

        CLRtoRGB(&rgb, flsh[4].alpha, flsh[4].alpha, flsh[4].alpha);

        PutSpriteYW(PHT_ZOOM_TRIANGLE_LFT, PHT_ZOOM_TRIANGLE_RGT, pos_x, pos_y, 0.0f, rgb, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }
    else
    {
        FlashStarYW(&flsh[4], 192, 64, 60, 0xff);

        CLRtoRGB(&rgb, flsh[4].alpha, flsh[4].alpha, flsh[4].alpha);
    }
}

static void AlbmTrFlsh(short int pos_x, short int pos_y, short int alpha)
{
    int rgb;

    if (pfile_wrk.pic_num > 6)
    {
        FlashStarYW(&flsh[5], 64, 192, 60, 0);

        CLRtoRGB(&rgb, flsh[5].alpha, flsh[5].alpha, flsh[5].alpha);

        PutSpriteYW(PHT_SCR_TRIANGLE_U, PHT_SCR_TRIANGLE_D, pos_x, pos_y, 0.0f, rgb, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }
    else
    {
        FlashStarYW(&flsh[6], 64, 192, 60, 0xff);

        CLRtoRGB(&rgb, flsh[6].alpha, flsh[6].alpha, flsh[6].alpha); // OOB access!

        PutSpriteYW(PHT_SCR_TRIANGLE_U, PHT_SCR_TRIANGLE_D, pos_x, pos_y, 0.0f, 0x404040, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }
}

u_char TimeIsMoney(u_char hexhex)
{
    return ((hexhex >> 4) * 10) + (hexhex & 0xF);
}

u_char MoneyIsTime(u_char time)
{
    // returns a BCD (Binary-Coded Decimal) value of the input: 42 -> 0x42, 34 -> 0x34, etc (from 0 to 99)
    u_char ones;
    u_char tens;

    ones = time % 10;
    tens = (time - ones) / 10;

    ones += tens * 16;

    return ones;
}

void PutDate(PICTURE_WRK *pic_inf, short int pos_x, short int pos_y, short int alpha, int pri)
{
#ifdef BUILD_EU_VERSION
    PutNumberYW(3, TimeIsMoney(pic_inf->time.day), pos_x + 4, pos_y, 1.0f, 1.0f, 0x808080, alpha, pri, 2, 1);
    PutNumberYW(3, TimeIsMoney(pic_inf->time.month), pos_x + 41, pos_y, 1.0f, 1.0f, 0x808080, alpha, pri, 2, 1);
    PutNumberYW(3, TimeIsMoney(pic_inf->time.year), pos_x + 78, pos_y, 1.0f, 1.0f, 0x808080, alpha, pri, 2, 1);
#else
    PutNumberYW(3, TimeIsMoney(pic_inf->time.year), pos_x + 4, pos_y, 1.0f, 1.0f, 0x808080, alpha, pri, 2, 1);
    PutNumberYW(3, TimeIsMoney(pic_inf->time.month), pos_x + 41, pos_y, 1.0f, 1.0f, 0x808080, alpha, pri, 2, 1);
    PutNumberYW(3, TimeIsMoney(pic_inf->time.day), pos_x + 78, pos_y, 1.0f, 1.0f, 0x808080, alpha, pri, 2, 1);
#endif
    PutNumberYW(3, TimeIsMoney(pic_inf->time.hour), pos_x + 118, pos_y, 1.0f, 1.0f, 0x808080, alpha, pri, 2, 1);
    PutNumberYW(3, TimeIsMoney(pic_inf->time.minute), pos_x + 155, pos_y, 1.0f, 1.0f, 0x808080, alpha, pri, 2, 1);
    PutNumberYW(3, TimeIsMoney(pic_inf->time.second), pos_x + 192, pos_y, 1.0f, 1.0f, 0x808080, alpha, pri, 2, 1);

    PutStringYW(33, 5, pos_x + 29, pos_y + 1, 0x808080, alpha, pri, 0);
    PutStringYW(33, 5, pos_x + 66, pos_y + 1, 0x808080, alpha, pri, 0);
    PutStringYW(33, 6, pos_x + 145, pos_y + 2, 0x808080, alpha, pri, 0);
    PutStringYW(33, 6, pos_x + 182, pos_y + 2, 0x808080, alpha, pri, 0);
}

static void AlbmCntInit()
{
    return;
}

static void AlbmCntRenew()
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

static void AlbmModeInOut(char is_outgame)
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
            fade_in = ((yw2d.inn_mode_cnt - 10) * 0x80) / 10.0f;

            yw2d.io_a[0] = 128.0f;
            yw2d.io_a[1] = 128.0f;
            yw2d.io_a[2] = 0.0f;
            yw2d.io_a[3] = 0.0f;
            yw2d.io_a[4] = 128.0f;
            yw2d.io_a[5] = fade_in;
        }

        if (is_outgame == 0)
        {
            PlayerStatusYW(yw2d.io_a[2]);
            DspTopMenu(yw2d.io_x[3], yw2d.io_y[3], yw2d.io_a[3], 0);
            DspPlayData(yw2d.io_x[2], yw2d.io_y[2], yw2d.io_a[2], 1);
        }

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

        if (is_outgame == 0)
        {
            PlayerStatusYW(yw2d.io_a[2]);
            DspTopMenu(yw2d.io_x[3], yw2d.io_y[3], yw2d.io_a[3], 0);
            DspPlayData(yw2d.io_x[2], yw2d.io_y[2], yw2d.io_a[2], 1);
        }

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

static void AlbmModeInOut2(char is_outgame)
{
    if (is_outgame == 0)
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
}

static u_char SealChk()
{
    int i;
    u_char seal_num;

    seal_num = 0;

    for (i = 0; i < 24; i++)
    {
        if (pfile_wrk.pic[i].status & 0x2)
        {
            seal_num++;
        }
    }

    return seal_num;
}

void OutGameInitPhoto()
{
    yw2d.inn_mode_cnt = 0;

    ig_menu_wrk.mode = 2;
}

int isPhotoEnd()
{
    return ig_menu_wrk.mode != IGMENU_MODE_ALBM;
}

float GetPhtDispAlpha()
{
    if (yw2d.inn_mode_cnt != 0)
    {
        return yw2d.io_a[5];
    }

    return 0.0f;
}
