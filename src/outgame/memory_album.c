#include "common.h"
#include "typedefs.h"
#include "addresses.h"
#include "structs.h"
#include "enums.h"
#include "memory_album.h"

#include "ingame/menu/item.h"
#include "ingame/menu/ig_camra.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/ig_album.h"
#include "ingame/menu/ig_file.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph2d/compress.h"
#include "main/glob.h"
#include "mc/mc_exec.h"
#include "mc/mc_main.h"
#include "ingame/photo/pht_make.h"
#include "graphics/graph3d/sglib.h"
#include "os/eeiop/eese.h"
#include "ingame/menu/ig_menu.h"
#include "graphics/graph2d/tim2.h"

MC_ALBUM_SAVE mc_album_save = {0};
MC_ALBUM_SAVE album_save_dat[2] = {0};
char is_newdata_albmside = 0;
static DSP_MEM_ALBM dm_albm;

#if defined(BUILD_JP_VERSION)
#define PAD_ACTION_CONFIRM PAD_CIRCLE
#define PAD_ACTION_EXIT PAD_CROSS
#define PAD_ACTION_MOVE PAD_TRIANGLE
#elif defined(BUILD_US_VERSION)
#define PAD_ACTION_CONFIRM PAD_CROSS
#define PAD_ACTION_EXIT PAD_TRIANGLE
#define PAD_ACTION_MOVE PAD_CIRCLE
#elif defined(BUILD_EU_VERSION)
#define PAD_ACTION_CONFIRM PAD_CROSS
#define PAD_ACTION_EXIT PAD_TRIANGLE
#define PAD_ACTION_MOVE PAD_CIRCLE
#endif

#define PI 3.1415927f

void MemAlbmInit(u_char h_type, u_char src_pht_max, u_char cpy_pht_max, u_char src_cl_type, u_char cpy_cl_type, u_char src_slot, u_char cpy_slot, u_char src_file, u_char cpy_file)
{
    dm_albm = (DSP_MEM_ALBM){0};

    dm_albm.is_title = h_type;

    switch (dm_albm.is_title)
    {
    case 0:
        dm_albm.src_addr = MEM_ALBUM_SRC_ADDRESS_0;
        dm_albm.cpy_addr = MEM_ALBUM_CPY_ADDRESS_0;
        dm_albm.pht_max[0] = pfile_wrk.pic_num;
        dm_albm.cl_type[0] = 5;
        dm_albm.slot[0] = 0;
        dm_albm.file[0] = 0;
        dm_albm.pht_max[1] = cpy_pht_max;
        dm_albm.cl_type[1] = cpy_cl_type;
        dm_albm.slot[1] = cpy_slot;
        dm_albm.file[1] = cpy_file;
    break;
    case 1:
        dm_albm.src_addr = MEM_ALBUM_SRC_ADDRESS_1;
        dm_albm.cpy_addr = MEM_ALBUM_CPY_ADDRESS_1;
        dm_albm.pht_max[0] = src_pht_max;
        dm_albm.pht_max[1] = cpy_pht_max;
        dm_albm.cl_type[0] = src_cl_type;
        dm_albm.cl_type[1] = cpy_cl_type;
        dm_albm.slot[0] = src_slot;
        dm_albm.slot[1] = cpy_slot;
        dm_albm.file[0] = src_file;
        dm_albm.file[1] = cpy_file;
    break;
    }

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    dm_albm.csr[0] = 0;
    dm_albm.flsh_tmr = 0;
    dm_albm.side_chng_wait = 0;
    dm_albm.copy_wait = 0;
    dm_albm.paste_wait = 0;
    dm_albm.side = 0;
#endif

    dm_albm.mode_chng = 20;

    InitPhotoExpand();
}

void MemAlbmInit2(u_char side, u_char pht_max, u_char cl_type, u_char slot, u_char file)
{
    dm_albm.pht_max[side] = pht_max;
    dm_albm.cl_type[side] = cl_type;
    dm_albm.slot[side] = slot;
    dm_albm.file[side] = file;

    dm_albm.mode_chng = 20;

#if defined(BUILD_JP_VERSION)
    dm_albm.csr[side] = 0;
#endif

    dm_albm.csr_mode = 0;
    dm_albm.exit_realy = 0;
    dm_albm.pst_mode = 0;
    dm_albm.menu_alp = 0;
    dm_albm.mem = 0;
    dm_albm.buf_set = 0;
    dm_albm.rtrn_bak = 0;
    dm_albm.io_mode = 0;
    dm_albm.all_alp = 0;
    dm_albm.yes_no = 0;
    dm_albm.pst_yes_no = 0;

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    dm_albm.flsh_tmr = 0;
    dm_albm.side_chng_wait = 0;
    dm_albm.copy_wait = 0;
    dm_albm.paste_wait = 0;
#endif

    dm_albm.side = side;

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    dm_albm.csr[side] = 0;
#endif
}

void MemAlbmInit3()
{
    dm_albm.csr_mode = 0;
    dm_albm.exit_realy = 0;
    dm_albm.pst_mode = 0;
    dm_albm.menu_alp = 0;
    dm_albm.mode_chng = 20;
    dm_albm.buf_set = 0;
    dm_albm.rtrn_bak = 0;
    dm_albm.io_mode = 0;
    dm_albm.all_alp = 0;
    dm_albm.yes_no = 0;
    dm_albm.pst_yes_no = 0;

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    dm_albm.flsh_tmr = 0;
    dm_albm.side_chng_wait = 0;
    dm_albm.copy_wait = 0;
    dm_albm.paste_wait = 0;
#endif
}

void NewAlbumInit(u_char side)
{
    int i;

    memset(&album_save_dat[side], 0, sizeof(MC_ALBUM_SAVE));

    for (i = 0; i < 24; i++)
    {
        album_save_dat[side].pic[i].adr_no = i;
    }

    dm_albm.mode_chng = 20;

    dm_albm.pht_max[side] = 0;
    dm_albm.cl_type[side] = 5;
    dm_albm.slot[side] = 3;
    dm_albm.file[side] = 0;
    dm_albm.csr[side] = 0;

#if defined(BUILD_JP_VERSION)
    dm_albm.side = side;
#endif

    dm_albm.csr_mode = 0;
    dm_albm.exit_realy = 0;
    dm_albm.pst_mode = 0;
    dm_albm.mem = 0;
    dm_albm.menu_alp = 0;
    dm_albm.buf_set = 0;
    dm_albm.rtrn_bak = 0;
    dm_albm.io_mode = 0;
    dm_albm.all_alp = 0;

    dm_albm.yes_no = 0;
    dm_albm.pst_yes_no = 0;

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    dm_albm.side = 0;
    dm_albm.csr[0] = 0;
    dm_albm.flsh_tmr = 0;
#endif
}

char SweetMemories(u_char mode, u_char alp)
{
    u_char rtrn;
    u_char alp_max;
    u_char alp_m0;
    u_char alp_m1;
    u_char alp_s0;
    u_char alp_s1;

    switch (dm_albm.io_mode)
    {
    case 0:
        if (dm_albm.all_alp + 8 < alp)
        {
            dm_albm.all_alp += 8;
        }
        else
        {
            dm_albm.all_alp = alp;
        }
    break;
    case 1:
        if (dm_albm.all_alp - 8 > 0)
        {
            dm_albm.all_alp -= 8;
        }
        else
        {
            dm_albm.all_alp = 0;
        }
    break;
    }

    alp_max = dm_albm.all_alp;

    if (mode != 0)
    {
        PutSpriteYW(OA_TP_BG_L, OA_TP_BG_L, 0.0f, 0.0f, 0.0f, 0x606060, alp_max, 1.2598425f, 1.258427f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(OA_TP_BG_R, OA_TP_BG_R, 0.0f, 0.0f, 0.0f, 0x606060, alp_max, 1.2598425f, 1.258427f, 0, 0xff, 1, 0, 0);
    }

    if (dm_albm.mode_chng != 0)
    {
        dm_albm.mode_chng--;
    }

    switch (dm_albm.mode)
    {
    case 0:
        alp_m1 = (alp_max * dm_albm.mode_chng) / 20.0f;
        alp_m0 = (alp_max - alp_m1);
    break;
    case 1:
        alp_m0 = (alp_max * dm_albm.mode_chng) / 20.0f;
        alp_m1 = alp_max - alp_m0;
    break;
    }

    if (dm_albm.side_chng != 0)
    {
        dm_albm.side_chng--;
    }

    switch (dm_albm.side)
    {
    case 0:
        alp_s1 = (alp_m0 * dm_albm.side_chng) / 20.0f;
        alp_s0 = alp_m0 - alp_s1;
    break;
    case 1:
        alp_s0 = (alp_m0 * dm_albm.side_chng) / 20.0f;
        alp_s1 = alp_m0 - alp_s0;
    break;
    }

    switch (dm_albm.mode)
    {
    case 0:
        DspOgAlbumTop1(alp_m0, alp_s0, alp_s1);

        switch (dm_albm.side)
        {
        case 0:
            DspPhotInTopPage(alp_s0);
        break;
        case 1:
            DspPhotInTopPage(alp_s1);
        break;
        }

        DspOgAlbumTop2(alp_m0, alp_s0, alp_s1);
    break;
    case 1:
        DspOgAlbumBig(alp_m1);
    break;
    }

    switch (dm_albm.mode)
    {
    case 0:
        rtrn = NmlPadInTopPage();

        if (rtrn != 0 && dm_albm.rtrn_bak == 0)
        {
            dm_albm.rtrn_bak = rtrn;
            dm_albm.io_mode = 1;
        }
    break;
    case 1:
        AbnPadInBigPage(alp_m1);
    break;
    }

    if (dm_albm.csr_mode == 1 || dm_albm.csr_mode == 3)
    {
        if (dm_albm.menu_alp + 8 < alp_max)
        {
            dm_albm.menu_alp += 8;
        }
        else
        {
            dm_albm.menu_alp = alp_max;
        }
    }
    else if (dm_albm.menu_alp - 8 > 0)
    {
        dm_albm.menu_alp -= 8;
    }
    else
    {
        dm_albm.menu_alp = 0;
    }

    if (dm_albm.menu_alp != 0)
    {
        switch (dm_albm.csr[dm_albm.side] & 0x7)
        {
            case 0:
            case 1:
            case 2:
            case 3:
                ComWin(1, dm_albm.csr[3], dm_albm.menu_alp);
            break;
            case 4:
            case 5:
            case 6:
            case 7:
                ComWin(0, dm_albm.csr[3], dm_albm.menu_alp);
            break;
        }
    }

    if (InKeyTamusi() == 0)
    {
        if (dm_albm.key_nashi_nori_take < 600)
        {
            dm_albm.key_nashi_nori_take++;
        }
        else
        {
            dm_albm.key_nashi_nori_take = 600;
        }
    }
    else
    {
        dm_albm.key_nashi_nori_take = 0;
    }

    if (
        dm_albm.key_nashi_nori_take == 600 && dm_albm.csr_mode == 0 &&
        dm_albm.csr[dm_albm.side] < dm_albm.pht_max[dm_albm.side]
    )
    {
        if (dm_albm.dat_alp == 0)
        {
            ReqPhotoExpand();

            switch (dm_albm.csr[dm_albm.side])
            {
            case 0:
            case 1:
            case 4:
            case 5:
                dm_albm.pop_ud = 1;
            break;
            case 2:
            case 3:
            case 6:
            case 7:
                dm_albm.pop_ud = 0;
            break;
            }
        }

        if (dm_albm.dat_alp + 8 < alp_max)
        {
            dm_albm.dat_alp += 8;
        }
        else
        {
            dm_albm.dat_alp = alp_max;
        }
    }
    else if (dm_albm.dat_alp - 8 > 0)
    {
        dm_albm.dat_alp -= 8;
    }
    else
    {
        dm_albm.dat_alp = 0;
    }

    if (dm_albm.io_mode != 0 && dm_albm.all_alp == 0)
    {
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        is_newdata_albmside = 0;

        switch(dm_albm.rtrn_bak)
        {
        case 1:
        case 2:
            if (dm_albm.slot[dm_albm.side] == 3)
            {
                is_newdata_albmside = 1;
            }
        break;
        }
#endif

        return dm_albm.rtrn_bak;
    }

    return 0;
}

void DspPhotInTopPage(u_char alp)
{
    int i;
    u_char pht_no;
    u_char pht_adr;
    u_char csr_alp;
    int csr_rgb;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    float flsh;
    u_char base_alp;
#endif
    short int px[8] = {  57, 176,  57, 176, 354, 473, 354, 473 };
    short int py[8] = { 100, 133, 229, 262, 100, 133, 229, 262 };

    DrawSPhotoFromSmallPhotoArea(0, 0x1000, 0, 0, 0, 0);


    for (i = 0; i < 8; i++)
    {
        pht_no = ((u_char)(dm_albm.csr[dm_albm.side] / 8.0f) * 8) + i;
        pht_adr = album_save_dat[dm_albm.side].pic[pht_no].adr_no;

        if (pht_no <= dm_albm.pht_max[dm_albm.side])
        {
            if (pht_no < dm_albm.pht_max[dm_albm.side])
            {
                if (dm_albm.side == 0)
                {
                    DrawSPhotoFromSmallPhotoArea2AD(dm_albm.src_addr, pht_adr, 0x14000, 0, px[i] + 1, py[i] + 1, 110, 74, alp);
                }
                else
                {
                    DrawSPhotoFromSmallPhotoArea2AD(dm_albm.cpy_addr, pht_adr, 0x14000, 0, px[i] + 1, py[i] + 1, 110, 74, alp);
                }

                PutSpriteYW(OA_TP_PHT_WAKU, OA_TP_PHT_WAKU, px[i], py[i], 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            if (dm_albm.paste_wait != 0)
            {
                u_char pos_idx;

                flsh = dm_albm.paste_wait / 15.0f;
                pos_idx = dm_albm.csr[dm_albm.side] & 0x7;

                if (pos_idx != 0)
                {
                    base_alp = 0x40;
                }
                else
                {
                    base_alp = 0x80;
                }

                VER_POLY_SQUARE_YW(px[pos_idx], py[pos_idx], 110, 74, 0x808080, base_alp * (dm_albm.paste_wait / 15.0f), 1.0f, 1.0f, 0xf000, 1, 0);
            }
#endif

            if (i == (dm_albm.csr[dm_albm.side] & 0x7) && dm_albm.csr_mode != 2 && dm_albm.csr_mode != 3)
            {
#if defined(BUILD_JP_VERSION)
                if (dm_albm.buf_set == 0)
                {
                    csr_alp = alp;
                    csr_rgb = 0x808080;

                    dm_albm.csr_tim = 0;
                }
                else
                {
                    csr_alp = alp * (cosf((dm_albm.csr_tim * (PI * 2)) / 50.0f) + 1.0f) * 0.5f;

                    if (dm_albm.csr_tim < 50.0f)
                    {
                        dm_albm.csr_tim++;
                    }
                    else
                    {
                        dm_albm.csr_tim = 0;
                    }

                    csr_rgb = 0x800000;
                }
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                if (dm_albm.buf_set != 0)
                {
                    csr_alp = alp * (SgCosf((dm_albm.csr_tim * (PI * 2)) / 50.0f) + 1.0f) * 0.5f;

                    if (dm_albm.csr_tim < 50.0f)
                    {
                        dm_albm.csr_tim++;
                    }
                    else
                    {
                        dm_albm.csr_tim = 0;
                    }

                    csr_rgb = 0x800000;
                }
                else if (dm_albm.copy_wait != 0)
                {
                    float rad = ((dm_albm.copy_wait % 15) * (PI * 2)) / 15.0f;

                    flsh = ((SgSinf(rad) + 1.0f) * 0.5f + 1.0f) * 0.5f;

                    csr_alp = alp;
                    csr_rgb = 0x808080;

                }
                else
                {
                    csr_alp = alp;
                    csr_rgb = 0x606060;

                    dm_albm.csr_tim = 0;
                }
#endif

                PutSpriteYW(OA_TP_PHT_SELE1, OA_TP_PHT_SELE1, px[i] -  3, py[i] -  5, 0.0f, csr_rgb, csr_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                PutSpriteYW(OA_TP_PHT_SELE2, OA_TP_PHT_SELE2, px[i] + 85, py[i] -  4, 0.0f, csr_rgb, csr_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                PutSpriteYW(OA_TP_PHT_SELE3, OA_TP_PHT_SELE3, px[i] -  4, py[i] + 57, 0.0f, csr_rgb, csr_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                PutSpriteYW(OA_TP_PHT_SELE4, OA_TP_PHT_SELE4, px[i] + 84, py[i] + 56, 0.0f, csr_rgb, csr_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                if (dm_albm.copy_wait != 0)
                {
                    PutSpriteYW(OA_TP_PHT_SELE1, OA_TP_PHT_SELE1, px[i] -  3, py[i] -  5, 0.0f, csr_rgb, csr_alp * flsh, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
                    PutSpriteYW(OA_TP_PHT_SELE2, OA_TP_PHT_SELE2, px[i] + 85, py[i] -  4, 0.0f, csr_rgb, csr_alp * flsh, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
                    PutSpriteYW(OA_TP_PHT_SELE3, OA_TP_PHT_SELE3, px[i] -  4, py[i] + 57, 0.0f, csr_rgb, csr_alp * flsh, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
                    PutSpriteYW(OA_TP_PHT_SELE4, OA_TP_PHT_SELE4, px[i] + 84, py[i] + 56, 0.0f, csr_rgb, csr_alp * flsh, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
                }
#endif
            }
        }
    }

    DrawSPhotoFromSmallPhotoArea(0, 0x1000, 0, 0, 0, 0);
}

char NmlPadInTopPage()
{
    char rtrn;
    char rtrn2;
    u_char csr_movable;
    CSR_ADD csr_ax[8] = {
        { .p = -3, .n = 1 },
        { .p = -1, .n = 3 },
        { .p = -3, .n = 1 },
        { .p = -1, .n = 3 },
        { .p = -3, .n = 1 },
        { .p = -1, .n = 3 },
        { .p = -3, .n = 1 },
        { .p = -1, .n = 3 },
    };
    CSR_ADD csr_ay[8] = {
        { .p = 99, .n =  2 },
        { .p = 99, .n =  2 },
        { .p = -2, .n = 82 },
        { .p = -2, .n = 82 },
        { .p = 99, .n =  2 },
        { .p = 99, .n =  2 },
        { .p = -2, .n = 82 },
        { .p = -2, .n = 82 },
    };

    rtrn = 0;
    rtrn2 = 0;

    if (dm_albm.side == 0 && dm_albm.is_title == 0)
    {
        if (dm_albm.pht_max[dm_albm.side] == 0)
        {
            csr_movable = 0;
        }
        else
        {
            csr_movable = dm_albm.pht_max[dm_albm.side] - 1;
        }
    }
    else
    {
        csr_movable = dm_albm.pht_max[dm_albm.side];
    }

    if (dm_albm.exit_realy == 1)
    {
        if (PAD_BTN_PRESSED(PAD_ACTION_EXIT))
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            dm_albm.exit_realy = 0;
        }
        else if (PAD_BTN_PRESSED(PAD_SQUARE) || PAD_BTN_PRESSED(PAD_ACTION_CONFIRM) == 1)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);

#if defined(BUILD_JP_VERSION)
            if (dm_albm.csr[4] == 0)
            {
                dm_albm.csr_mode = 0;
                dm_albm.exit_realy = 0;

                rtrn2 = 5;
            }
            else
            {
                dm_albm.exit_realy = 0;
            }
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            switch(dm_albm.csr[4])
            {
            case 0:
                switch (dm_albm.side)
                {
                case 0:
                    dm_albm.csr_mode = 0;
                    dm_albm.exit_realy = 0;

                    save_pht_in_game = 0;

                    mc_album_save = album_save_dat[0];
                    mc_photo_num = dm_albm.pht_max[0];

                    rtrn2 = 1;
                break;
                case 1:
                    dm_albm.csr_mode = 0;
                    dm_albm.exit_realy = 0;

                    save_pht_in_game = 0;

                    mc_album_save = album_save_dat[1];
                    mc_photo_num = dm_albm.pht_max[1];

                    rtrn2 = 2;
                break;
                }
            break;
            case 1:
                dm_albm.csr_mode = 0;
                dm_albm.exit_realy = 0;

                rtrn2 = 5;
            break;
            case 2:
                dm_albm.exit_realy = 0;
            break;
            }
#endif
        }
        else if (PAD_BTN_PRESSED(PAD_DPAD_RIGHT) || PAD_LANA_PRESSED(PAD_LANA_RIGHT))
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

#if defined(BUILD_JP_VERSION)
            dm_albm.csr[4] = 1 - dm_albm.csr[4];
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            if (dm_albm.side == 0 && dm_albm.is_title == 0)
            {
                if (dm_albm.csr[4] == 0)
                {
                    dm_albm.csr[4] = 2;
                }

                if (dm_albm.csr[4] < 2)
                {
                    dm_albm.csr[4]++;
                }
                else
                {
                    dm_albm.csr[4] = 1;
                }
            }
            else
            {
                if (dm_albm.csr[4] < 2)
                {
                    dm_albm.csr[4]++;
                }
                else
                {
                    dm_albm.csr[4] = 0;
                }
            }
#endif
        }
        else if (PAD_BTN_PRESSED(PAD_DPAD_LEFT) || PAD_LANA_PRESSED(PAD_LANA_LEFT))
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

#if defined(BUILD_JP_VERSION)
            dm_albm.csr[4] = 1 - dm_albm.csr[4];
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            if (dm_albm.side == 0 && dm_albm.is_title == 0)
            {
                if (dm_albm.csr[4] == 0)
                {
                    dm_albm.csr[4] = 1;
                }

                if (dm_albm.csr[4] >= 2)
                {
                    dm_albm.csr[4]--;
                }
                else
                {
                    dm_albm.csr[4] = 2;
                }
            }
            else
            {
                if (dm_albm.csr[4] != 0)
                {
                    dm_albm.csr[4]--;
                }
                else
                {
                    dm_albm.csr[4] = 2;
                }
            }
#endif
        }
    }
    else if (dm_albm.pst_mode == 1)
    {
        if (PasteCtrl() != 0)
        {
            dm_albm.pst_mode = 0;
        }
    }
    else if (dm_albm.csr_mode == 1)
    {
        if (dm_albm.menu_alp == dm_albm.all_alp)
        {
            rtrn = MenuPadCmn();
        }
        else
        {
            rtrn = 0;
        }
    }
    else if (dm_albm.csr_mode == 2)
    {
        BuffPadCmn();
    }
    else if (dm_albm.csr_mode == 3)
    {
        BuffPadMnu();
    }
    else if (dm_albm.io_mode == 0)
    {
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        if (dm_albm.side_chng_wait == 0 && dm_albm.copy_wait == 0 && dm_albm.paste_wait == 0)
        {
#endif
            if (PAD_BTN_PRESSED(PAD_ACTION_EXIT))
            {
                if (dm_albm.buf_set != 0)
                {
                    SeStartFix(3, 0, 0x1000, 0x1000, 0);

                    dm_albm.buf_set = 0;
                    dm_albm.csr_mode = 0;
                }
                else
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);

                    dm_albm.exit_realy = 1;
#if defined(BUILD_JP_VERSION)
                    dm_albm.csr[4] = 1;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                    dm_albm.csr[4] = 2;
#endif
                }
            }
            else if (PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
            {
                if (dm_albm.buf_set == 0)
                {
                    switch (dm_albm.side)
                    {
                    case 0:
                        if (dm_albm.csr[0] < dm_albm.pht_max[0])
                        {
                            SeStartFix(1, 0, 0x1000, 0x1000, 0);

                            ReqPhotoExpand();

                            dm_albm.mode = 1;
                            dm_albm.mode_chng = 20;
                        }
                        else
                        {
                            SeStartFix(2, 0, 0x1000, 0x1000, 0);
                        }
                    break;
                    case 1:
                        if (dm_albm.csr[1] < dm_albm.pht_max[1])
                        {
                            SeStartFix(1, 0, 0x1000, 0x1000, 0);

                            ReqPhotoExpand();

                            dm_albm.mode = 1;
                            dm_albm.mode_chng = 20;
                        }
                        else
                        {
                            SeStartFix(2, 0, 0x1000, 0x1000, 0);
                        }
                    break;
                    }
                }
                else
                {
                    dm_albm.pst_mode = 1;
                }
            }
            else if (PAD_BTN_PRESSED(PAD_SQUARE))
            {
                if (dm_albm.buf_set == 0)
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);

                    dm_albm.csr[3] = 0;
                    dm_albm.csr_mode = 1;
                }
                else
                {
                    SeStartFix(2, 0, 0x1000, 0x1000, 0);
                }
            }
            else if (PAD_BTN_PRESSED(PAD_ACTION_MOVE))
            {
                if (dm_albm.buf_set == 0)
                {
                    if (dm_albm.csr[dm_albm.side] < dm_albm.pht_max[dm_albm.side])
                    {
                        if (SamePhotoChk() == 0)
                        {
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                            dm_albm.copy_wait = 15;
#endif
                            KillRingBuffer();
                        }
                        else
                        {
                            SeStartFix(2, 0, 0x1000, 0x1000, 0);
                        }
                    }
                    else
                    {
                        SeStartFix(2, 0, 0x1000, 0x1000, 0);
                    }

                }
                else
                {
                    dm_albm.pst_mode = 1;
                }
            }
            else if (PAD_BTN_REPEAT(PAD_DPAD_RIGHT) || PAD_LANA_REPEAT(PAD_LANA_RIGHT))
            {
                if (dm_albm.csr[dm_albm.side] + csr_ax[dm_albm.csr[dm_albm.side] % 8].n < csr_movable)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);

                    dm_albm.csr[dm_albm.side] += csr_ax[dm_albm.csr[dm_albm.side] % 8].n;
                }
                else
                {
                    if (dm_albm.side == 0 && dm_albm.is_title == 0)
                    {
                        if (csr_movable < 23)
                        {
                            if (dm_albm.csr[dm_albm.side] != csr_movable)
                            {
                                SeStartFix(0, 0, 0x1000, 0x1000, 0);

                                dm_albm.csr[dm_albm.side] = csr_movable;
                            }
                        }
                        else if (dm_albm.csr[dm_albm.side] != 23)
                        {
                            SeStartFix(0, 0, 0x1000, 0x1000, 0);

                            dm_albm.csr[dm_albm.side] = 23;
                        }
                    }
                    else if (csr_movable < 24)
                    {
                        if (dm_albm.csr[dm_albm.side] != csr_movable)
                        {
                            SeStartFix(0, 0, 0x1000, 0x1000, 0);

                            dm_albm.csr[dm_albm.side] = csr_movable;
                        }
                    }
                }
            }
            else if (PAD_BTN_REPEAT(PAD_DPAD_LEFT) || PAD_LANA_REPEAT(PAD_LANA_LEFT))
            {
                if (dm_albm.csr[dm_albm.side] + csr_ax[dm_albm.csr[dm_albm.side] % 8].p >= 0)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);

                    dm_albm.csr[dm_albm.side] += csr_ax[dm_albm.csr[dm_albm.side] % 8].p;
                }
            }
            else if (PAD_BTN_REPEAT(PAD_DPAD_DOWN) || PAD_LANA_REPEAT(PAD_LANA_DOWN))
            {
                if (csr_ay[dm_albm.csr[dm_albm.side] % 8].n != 82)
                {
                    if (csr_movable >= dm_albm.csr[dm_albm.side] + csr_ay[dm_albm.csr[dm_albm.side] % 8].n)
                    {
                        SeStartFix(0, 0, 0x1000, 0x1000, 0);

                        dm_albm.csr[dm_albm.side] += csr_ay[dm_albm.csr[dm_albm.side] % 8].n;
                    }
                    else if (dm_albm.buf_set == 0 && dm_albm.mem != 0)
                    {
                        SeStartFix(0, 0, 0x1000, 0x1000, 0);

                        dm_albm.csr_mode = 2;
                        dm_albm.csr[2] = dm_albm.mem - 1;
                    }
                }
                else if (dm_albm.buf_set == 0 && dm_albm.mem != 0)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);

                    dm_albm.csr_mode = 2;
                    dm_albm.csr[2] = dm_albm.mem - 1;
                }
            }
            else if (PAD_BTN_REPEAT(PAD_DPAD_UP) || PAD_LANA_REPEAT(PAD_LANA_UP))
            {
                if (csr_ay[dm_albm.csr[dm_albm.side] % 8].p != 99)
                {
                    if (dm_albm.csr[dm_albm.side] + csr_ay[dm_albm.csr[dm_albm.side] % 8].p >= 0)
                    {
                        SeStartFix(0, 0, 0x1000, 0x1000, 0);

                        dm_albm.csr[dm_albm.side] += csr_ay[dm_albm.csr[dm_albm.side] % 8].p;
                    }
                }
            }
            else if (PAD_BTN_PRESSED(PAD_R1))
            {
                if (dm_albm.side == 0)
                {
#if defined(BUILD_JP_VERSION)
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);

                    dm_albm.side = 1;
                    dm_albm.inf_alp = 0;
                    dm_albm.side_chng = 20;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                    dm_albm.side_chng_mode = 1;
                    dm_albm.side_chng_wait = 15;
#endif
                }
            }
            else if (PAD_BTN_PRESSED(PAD_L1))
            {
                if (dm_albm.side == 1)
                {
#if defined(BUILD_JP_VERSION)
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);

                    dm_albm.side = 0;
                    dm_albm.inf_alp = 0;
                    dm_albm.side_chng = 20;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                    dm_albm.side_chng_mode = 0;
                    dm_albm.side_chng_wait = 15;
#endif
                }
            }
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        }
#endif
    }

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    if (dm_albm.side_chng_wait != 0)
    {
        dm_albm.side_chng_wait--;

        if (dm_albm.side_chng_wait == 0)
        {
            SeStartFix(4, 0, 0x1000, 0x1000, 0);

            dm_albm.side = dm_albm.side_chng_mode;
            dm_albm.inf_alp = 0;
            dm_albm.side_chng = 20;
        }
    }

    if (dm_albm.copy_wait != 0)
    {
        dm_albm.copy_wait--;

        if (dm_albm.copy_wait == 0)
        {
            SeStartFix(11, 0, 0x1000, 0x1000, 0);
        }
    }
#endif

    if (dm_albm.exit_realy == 1 || dm_albm.yes_no == 1)
    {
        VER_POLY_SQUARE_YW(34.0f, 315.0f, 577, 107, 0, 48.0f, 1.0f, 1.0f, 0x4000, 0, 0);

        DrawMessageBox(0x3000, 22.0f, 308.0f, 596.0f, 118.0f, 0x80);

#if defined(BUILD_JP_VERSION)
        YesNoCrslOKR(0x2000, dm_albm.csr[4] * 120 + 212, 382.0f, 0x808080, 128.0f, 1.5f);

        PutStringYW(IGMSG_PRE_SV_MSG, 1, 320, 385, 0x808080, 0x80, 0x1000, 1);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)

#if defined(BUILD_EU_VERSION)
        if (sys_wrk.language == LANG_FRENCH && dm_albm.csr[4] == 0)
        {
            YesNoCrslOKR(0x2000, 98.0f, 382.0f, 0x808080, 128.0f, 2.6f);
        }
        else
        {
            YesNoCrslOKR(0x2000, dm_albm.csr[4] * 132 + 106, 382.0f, 0x808080, 128.0f, 2.2f);
        }
#else
        YesNoCrslOKR(0x2000, dm_albm.csr[4] * 132 + 114, 380.0f, 0x808080, 128.0f, 2.0f);
#endif

        if (dm_albm.side == 0 && dm_albm.is_title == 0)
        {
            PutStringYW(IGMSG_MENU_MSG, 46, 68, 385, 0x808080, 0x80, 0x1000, 0);
        }
        else
        {
            PutStringYW(IGMSG_MENU_MSG, 40, 59, 385, 0x808080, 0x80, 0x1000, 0);
        }
#endif

        PutStringYW(IGMSG_MENU_MSG, 32, 320, 323, 0x808080, 0x80, 0x1000, 1);
        PutStringYW(IGMSG_MENU_MSG, 37, 60, 353, 0x808080, 0x80, 0x1000, 0);

        DrawSPhotoFromSmallPhotoArea(0, 0x1000, 0, 0, 0, 0);
    }

    if (rtrn2 != 0)
    {
        return rtrn2;
    }

    return rtrn;
}

char SamePhotoChk()
{
    int i;
    char same;

    same = 0;

    for (i = 0; i < dm_albm.mem; i++)
    {
        if (dm_albm.buf[i] == dm_albm.csr[dm_albm.side] && dm_albm.buf_sid[i] == dm_albm.side)
        {
            same++;
        }
    }

    return same;
}

void KillRingBuffer()
{
    int i;

    if (dm_albm.mem < 8)
    {
#if defined(BUILD_JP_VERSION)
        SeStartFix(1, 0, 0x1000, 0x1000, 0);
#endif

        dm_albm.buf[dm_albm.mem] = dm_albm.csr[dm_albm.side];
        dm_albm.buf_sid[dm_albm.mem] = dm_albm.side;
        dm_albm.mem++;
        dm_albm.buf[dm_albm.mem] = 0xff;
        dm_albm.buf_sid[dm_albm.mem] = 0xff;
    }
    else
    {
#if defined(BUILD_JP_VERSION)
        SeStartFix(1, 0, 0x1000, 0x1000, 0);
#endif

        for (i = 0; i < 8; i++)
        {
            dm_albm.buf[i] = dm_albm.buf[i + 1];
            dm_albm.buf_sid[i] = dm_albm.buf_sid[i + 1];
        }

        dm_albm.buf[7] = dm_albm.csr[dm_albm.side];
        dm_albm.buf_sid[7] = dm_albm.side;

        dm_albm.buf[8] = 0xff;
        dm_albm.buf_sid[8] = 0xff;
    }
}

char AbnPadInBigPage(u_char alp)
{
    u_char target;

    if (PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
    {
        SeStartFix(1, 0, 0x1000, 0x1000, 0);

        dm_albm.mode_chng = 20;
        dm_albm.mode = 0;
    }
    else
    {
        if (PAD_BTN_PRESSED(PAD_ACTION_EXIT))
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            dm_albm.mode_chng = 20;
            dm_albm.mode = 0;
        }
        else if (PAD_BTN_REPEAT(PAD_DPAD_LEFT) || PAD_LANA_REPEAT(PAD_LANA_LEFT))
        {
            ReqPhotoExpand();

            if (dm_albm.csr_mode == 2)
            {
                if (dm_albm.mem > 1)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                }

                if (dm_albm.mem - 1 > dm_albm.csr[2])
                {
                    dm_albm.csr[2]++;
                }
                else
                {
                    dm_albm.csr[2] = 0;
                }
            }
            else
            {
                if (dm_albm.pht_max[dm_albm.side] > 1)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                }

                if (dm_albm.csr[dm_albm.side] == 0)
                {
                    dm_albm.csr[dm_albm.side] = dm_albm.pht_max[dm_albm.side] - 1;
                }
                else
                {
                    dm_albm.csr[dm_albm.side]--;
                }
            }
        }
        else if (PAD_BTN_REPEAT(PAD_DPAD_RIGHT) || PAD_LANA_REPEAT(PAD_LANA_RIGHT))
        {
            ReqPhotoExpand();

            if (dm_albm.csr_mode == 2)
            {
                if (dm_albm.mem > 1)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                }

                if (dm_albm.csr[2] == 0)
                {
                    dm_albm.csr[2] = dm_albm.mem - 1;
                }
                else
                {
                    dm_albm.csr[2]--;
                }
            }
            else
            {
                if (dm_albm.pht_max[dm_albm.side] > 1)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                }

                if (dm_albm.csr[dm_albm.side] < dm_albm.pht_max[dm_albm.side] - 1)
                {
                    dm_albm.csr[dm_albm.side]++;
                }
                else
                {
                    dm_albm.csr[dm_albm.side] = 0;
                }
            }
        }
    }

    if (dm_albm.csr_mode == 2)
    {
        if (dm_albm.buf_exp_flg != 0)
        {
            dm_albm.buf_exp_flg = 0;

            ReqPhotoExpand();
        }

        target = album_save_dat[dm_albm.buf_sid[dm_albm.csr[2]]].pic[dm_albm.buf[dm_albm.csr[2]]].adr_no;

        if (dm_albm.buf_sid[dm_albm.csr[2]] == 0)
        {
            UncompressPhotoAD(dm_albm.src_addr + 0x15400, target);
        }
        else
        {
            UncompressPhotoAD(dm_albm.cpy_addr + 0x15400, target);
        }
    }
    else
    {
        target = album_save_dat[dm_albm.side].pic[dm_albm.csr[dm_albm.side]].adr_no;

        if (dm_albm.side == 0)
        {
            UncompressPhotoAD(dm_albm.src_addr + 0x15400, target);
        }
        else
        {
            UncompressPhotoAD(dm_albm.cpy_addr + 0x15400, target);
        }
    }

    VER_POLY_SQUARE_YW(128.0f, 73.0f, 383, 255, 0, alp, 1.0f, 1.0f, 0x1f000, 0, 0);

    if (CheckPhotoExpandEnd())
    {
        DrawSPhotoFromSmallPhotoArea(0, 0x1000, 0, 0, 0, 0);
        DrawPhotoFromPhotoWrk(target, 0x14000, 0, 128, 73, 383, 255, alp);
    }

    PutSpriteYW(ALBM_WAKU_BIG_U, ALBM_WAKU_BIG_U, 0.0f, -5.0f, 0.0f, 0x808080, alp, 19.3f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(ALBM_WAKU_BIG_D, ALBM_WAKU_BIG_D, 0.0f, -5.0f, 0.0f, 0x808080, alp, 19.3f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(ALBM_WAKU_BIG_L, ALBM_WAKU_BIG_L, 0.0f, -5.0f, 0.0f, 0x808080, alp, 1.0f, 15.875f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(ALBM_WAKU_BIG_R, ALBM_WAKU_BIG_R, 0.0f, -5.0f, 0.0f, 0x808080, alp, 1.0f, 15.875f, 0, 0xff, 1, 0, 0);

    MiniPht(alp);

    return 0;
}

void MiniPht(u_char alp)
{
    u_char n_mode;
    u_char p_mode;
    u_char pre;
    u_char nxt;
    u_char pht_adr;

    if (dm_albm.csr_mode == 2)
    {
        if (dm_albm.mem < 2)
        {
            return;
        }
    }
    else if (dm_albm.pht_max[dm_albm.side] < 2)
    {
        return;
    }

    if (dm_albm.csr_mode == 2)
    {
        if (dm_albm.csr[2] < (dm_albm.mem - 1))
        {
            pre = dm_albm.buf[dm_albm.csr[2] + 1];
            p_mode = dm_albm.buf_sid[dm_albm.csr[2] + 1];
        }
        else
        {
            pre = dm_albm.buf[0];
            p_mode = dm_albm.buf_sid[0];
        }

        pht_adr = dm_albm.csr[2];

        if (dm_albm.csr[2] == 0)
        {
            pht_adr = dm_albm.mem;
        }

        nxt = dm_albm.buf[pht_adr - 1];
        n_mode = dm_albm.buf_sid[pht_adr - 1];
    }
    else
    {
        p_mode = dm_albm.side;
        n_mode = dm_albm.side;

        if (dm_albm.csr[dm_albm.side] == 0)
        {
            pre = dm_albm.pht_max[dm_albm.side] - 1;
        }
        else
        {
            pre = dm_albm.csr[dm_albm.side] - 1;
        }

        if (dm_albm.csr[dm_albm.side] < dm_albm.pht_max[dm_albm.side] - 1)
        {
            nxt = dm_albm.csr[dm_albm.side] + 1;
        }
        else
        {
            nxt = 0;
        }
    }

    switch (p_mode)
    {
    case 0:
        DrawSPhotoFromSmallPhotoArea2AD(dm_albm.src_addr, album_save_dat[0].pic[pre].adr_no, 0x14000, 0.0f, 0x37, 0x103, 0x34, 0x24, alp);
    break;
    case 1:
        DrawSPhotoFromSmallPhotoArea2AD(dm_albm.cpy_addr, album_save_dat[1].pic[pre].adr_no, 0x14000, 0.0f, 0x37, 0x103, 0x34, 0x24, alp);
    break;
    }

    PutSpriteYW(ALBM_SAM_WAKU, ALBM_SAM_WAKU, 54.0f, 258.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    switch (n_mode)
    {
    case 0:
        DrawSPhotoFromSmallPhotoArea2AD(dm_albm.src_addr, album_save_dat[0].pic[nxt].adr_no, 0x14000, 0.0f, 0x217, 0x103, 0x34, 0x24, alp);
    break;
    case 1:
        DrawSPhotoFromSmallPhotoArea2AD(dm_albm.cpy_addr, album_save_dat[1].pic[nxt].adr_no, 0x14000, 0.0f, 0x217, 0x103, 0x34, 0x24, alp);
    break;
    }

    PutSpriteYW(ALBM_SAM_WAKU, ALBM_SAM_WAKU, 534.0f, 258.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0,  0);
}

char MenuPadCmn()
{
    u_char rtrn;

    rtrn = 0;

    if (dm_albm.yes_no == 0)
    {
        if (PAD_BTN_PRESSED(PAD_ACTION_EXIT))
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            dm_albm.csr_mode = 0;
        }
        else if (PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
        {
            switch (dm_albm.csr[3])
            {
            case 0:
                if (dm_albm.csr[dm_albm.side] < dm_albm.pht_max[dm_albm.side])
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);

                    ReqPhotoExpand();

                    dm_albm.mode = 1;
                    dm_albm.mode_chng = 20;
                    dm_albm.csr_mode = 0;
                }
                else
                {
                    SeStartFix(2, 0, 0x1000, 0x1000, 0);
                }
            break;
            case 1:
                if (dm_albm.buf_set == 0)
                {
                    if (dm_albm.csr[dm_albm.side] < dm_albm.pht_max[dm_albm.side])
                    {
                        if (SamePhotoChk() == 0)
                        {
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                            dm_albm.copy_wait = 15;
#endif

                            KillRingBuffer();
                        }
                        else
                        {
                            SeStartFix(2, 0, 0x1000, 0x1000, 0);
                        }
                    }
                    else
                    {
                        SeStartFix(2, 0, 0x1000, 0x1000, 0);
                    }
                }
                else
                {
                    dm_albm.pst_mode = 1;
                }

            break;
            case 2:
                if (
                    (dm_albm.side == 0 && dm_albm.is_title == 0) ||
                    dm_albm.csr[dm_albm.side] == dm_albm.pht_max[dm_albm.side]
                )
                {
                    SeStartFix(2, 0, 0x1000, 0x1000, 0);
                }
                else
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);

                    dm_albm.yes_no = 2;
                    dm_albm.csr[4] = 1;
                }
            break;
            case 3:
                switch (dm_albm.side)
                {
                case 0:
                    if (dm_albm.is_title == 0)
                    {
                        SeStartFix(2, 0, 0x1000, 0x1000, 0);
                    }
                    else
                    {
                        SeStartFix(1, 0, 0x1000, 0x1000, 0);

                        save_pht_in_game = 0;

                        mc_album_save = album_save_dat[0];
                        mc_photo_num = dm_albm.pht_max[0];

                        rtrn = 1;
                    }
                break;
                case 1:
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);

                    save_pht_in_game = 0;

                    mc_album_save = album_save_dat[1];
                    mc_photo_num = dm_albm.pht_max[1];

                    rtrn = 2;
                break;
                }
            break;
            case 4:
                switch (dm_albm.side)
                {
                case 0:
                    if (dm_albm.is_title != 0)
                    {
                        SeStartFix(1, 0, 0x1000, 0x1000, 0);

                        dm_albm.yes_no = 3;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                        dm_albm.csr[4] = 1;
#endif
                    }
                    else
                    {
                        SeStartFix(2, 0, 0x1000, 0x1000, 0);
                    }
                break;
                case 1:
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);

                    dm_albm.yes_no = 3;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                    dm_albm.csr[4] = 1;
#endif
                break;
                }
            break;
            case 5:
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

                dm_albm.yes_no = 1;
#if defined(BUILD_JP_VERSION)
                dm_albm.csr[4] = 1;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                dm_albm.csr[4] = 2;
#endif
            break;
            }
        }
        else if (PAD_BTN_REPEAT(PAD_DPAD_DOWN) || PAD_LANA_REPEAT(PAD_LANA_DOWN))
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            if (dm_albm.side == 0 && dm_albm.is_title == 0)
            {
                if (dm_albm.csr[3] == 0)
                {
                    dm_albm.csr[3]++;
                }
                else if (dm_albm.csr[3] == 1)
                {
                    dm_albm.csr[3] = 5;
                }
                else if (dm_albm.csr[3] == 5)
                {
                    dm_albm.csr[3] = 0;
                }
            }
            else if (dm_albm.csr[3] < 5)
            {
                dm_albm.csr[3]++;
            }
            else
            {
                dm_albm.csr[3] = 0;
            }
        }
        else if (PAD_BTN_REPEAT(PAD_DPAD_UP) || PAD_LANA_REPEAT(PAD_LANA_UP))
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            if (dm_albm.side == 0 && dm_albm.is_title == 0)
            {
                if (dm_albm.csr[3] == 5)
                {
                    dm_albm.csr[3] = 1;
                }
                else if (dm_albm.csr[3] > 0)
                {
                    dm_albm.csr[3]--;
                }
                else if (dm_albm.csr[3] == 0)
                {
                    dm_albm.csr[3] = 5;
                }
            }
            else if (dm_albm.csr[3] > 0)
            {
                dm_albm.csr[3]--;
            }
            else
            {
                dm_albm.csr[3] = 5;
            }
        }
    }
    else
    {
        switch (dm_albm.yes_no)
        {
        case 1:
            if (PAD_BTN_PRESSED(PAD_ACTION_EXIT))
            {
                SeStartFix(3, 0, 0x1000, 0x1000, 0);

                dm_albm.yes_no = 0;
            }
            else if (PAD_BTN_PRESSED(PAD_SQUARE) || PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
            {
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

#if defined(BUILD_JP_VERSION)
                if (dm_albm.csr[4] == 0)
                {
                    dm_albm.csr_mode = 0;
                    dm_albm.yes_no = 0;

                    rtrn = 5;
                }
                else
                {
                    dm_albm.yes_no = 0;
                }
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                switch (dm_albm.csr[4])
                {
                case 0:
                    switch (dm_albm.side)
                    {
                    case 0:
                        dm_albm.csr_mode = 0;
                        dm_albm.yes_no = 0;

                        save_pht_in_game = 0;

                        mc_album_save = album_save_dat[0];
                        mc_photo_num = dm_albm.pht_max[0];

                        rtrn = 1;
                    break;

                    case 1:
                        dm_albm.csr_mode = 0;
                        dm_albm.yes_no = 0;

                        save_pht_in_game = 0;

                        mc_album_save = album_save_dat[1];
                        mc_photo_num = dm_albm.pht_max[1];

                        rtrn = 2;
                    break;
                    }

                    break;

                case 1:
                    dm_albm.csr_mode = 0;
                    dm_albm.yes_no = 0;

                    rtrn = 5;
                break;
                case 2:
                    dm_albm.yes_no = 0;
                break;
                }
#endif
            }
            else if (PAD_BTN_PRESSED(PAD_DPAD_RIGHT) || PAD_LANA_PRESSED(PAD_LANA_RIGHT))
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);

#if defined(BUILD_JP_VERSION)
                dm_albm.csr[4] = 1 - dm_albm.csr[4];
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                if (dm_albm.side == 0 && dm_albm.is_title == 0)
                {
                    if (dm_albm.csr[4] == 0)
                    {
                        dm_albm.csr[4] = 2;
                    }

                    if (dm_albm.csr[4] < 2)
                    {
                        dm_albm.csr[4]++;
                    }
                    else
                    {
                        dm_albm.csr[4] = 1;
                    }
                }
                else if (dm_albm.csr[4] < 2)
                {
                    dm_albm.csr[4]++;
                }
                else
                {
                    dm_albm.csr[4] = 0;
                }
#endif
            }
            else if (PAD_BTN_PRESSED(PAD_DPAD_LEFT) || PAD_LANA_PRESSED(PAD_LANA_LEFT))
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);

#if defined(BUILD_JP_VERSION)
                dm_albm.csr[4] = 1 - dm_albm.csr[4];
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                if (dm_albm.side == 0 && dm_albm.is_title == 0)
                {
                    if (dm_albm.csr[4] == 0)
                    {
                        dm_albm.csr[4] = 1;
                    }

                    if (dm_albm.csr[4] >= 2)
                    {
                        dm_albm.csr[4]--;
                    }
                    else
                    {
                        dm_albm.csr[4] = 2;
                    }
                }
                else if (dm_albm.csr[4] > 0)
                {
                    dm_albm.csr[4]--;
                }
                else
                {
                    dm_albm.csr[4] = 2;
                }
#endif
            }
        break;
        case 2:
#if defined(BUILD_JP_VERSION)
            YesNoCrslOKR(0x2000, dm_albm.csr[4] * 120 + 367, 387.0f, 0x808080, 128.0f, 1.5f);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            YesNoCrslOKR(0x2000, dm_albm.csr[4] * 132 + 318, 387.0f, 0x808080, 128.0f, 2.0f);
#endif

            PutStringYW(IGMSG_MENU_MSG, 34, 60, 390, 0x808080, 0x80, 0x1000, 0);

#if defined(BUILD_JP_VERSION)
            PutStringYW(IGMSG_PRE_SV_MSG, 1, 475, 390, 0x808080, 0x80, 0x1000, 1);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            PutStringYW(IGMSG_PRE_SV_MSG, 1, 358, 390, 0x808080, 0x80, 0x1000, 1);
#endif

            if (PAD_BTN_PRESSED(PAD_ACTION_EXIT))
            {
                SeStartFix(3, 0, 0x1000, 0x1000, 0);

                dm_albm.yes_no = 0;
            }
            else if (PAD_BTN_PRESSED(PAD_SQUARE) || PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
            {
                if (dm_albm.csr[4] == 0)
                {
                    DelPyon(dm_albm.side, dm_albm.csr[dm_albm.side]);

                    dm_albm.yes_no = 0;
                }
                else
                {
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);

                    dm_albm.yes_no = 0;
                }
            }
            else if (PAD_BTN_PRESSED(PAD_DPAD_RIGHT) || PAD_LANA_PRESSED(PAD_LANA_RIGHT))
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);

                dm_albm.csr[4] = 1 - dm_albm.csr[4];
            }
            else if (PAD_BTN_PRESSED(PAD_DPAD_LEFT) || PAD_LANA_PRESSED(PAD_LANA_LEFT))
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);

                dm_albm.csr[4] = 1 - dm_albm.csr[4];
            }
        break;
        case 3:
            VER_POLY_SQUARE_YW(34.0f, 315.0f, 577, 107, 0, 48.0f, 1.0f, 1.0f, 0x4000, 0, 0);

            DrawMessageBox(0x3000, 22.0f, 308.0f, 596.0f, 118.0f, 0x80);

#if defined(BUILD_JP_VERSION)
            YesNoCrslOKR(0x2000, dm_albm.csr[4] * 120 + 212, 382.0f, 0x808080, 128.0f, 1.5f);
            PutStringYW(IGMSG_PRE_SV_MSG, 1, 320, 385, 0x808080, 0x80, 0x1000, 1);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            YesNoCrslOKR(0x2000, dm_albm.csr[4] * 132 + 184, 382.0f, 0x808080, 128.0f, 2.0f);
            PutStringYW(IGMSG_PRE_SV_MSG, 1, 225, 385, 0x808080, 0x80, 0x1000, 1);
#endif

            PutStringYW(IGMSG_MENU_MSG, 39, 320, 323, 0x808080, 0x80, 0x1000, 1);
            PutStringYW(IGMSG_MENU_MSG, 37, 60, 353, 0x808080, 0x80, 0x1000, 0);

            if (PAD_BTN_PRESSED(PAD_ACTION_EXIT))
            {
                SeStartFix(3, 0, 0x1000, 0x1000, 0);

                dm_albm.yes_no = 0;
            }
            else if (PAD_BTN_PRESSED(PAD_SQUARE) || PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
            {
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

                if (dm_albm.csr[4] == 0)
                {
                    switch (dm_albm.side)
                    {
                    case 0:
                        rtrn = 3;
                    break;
                    case 1:
                        rtrn = 4;
                    break;
                    }

                    dm_albm.yes_no = 0;
                }
                else
                {
                    dm_albm.yes_no = 0;
                }
            }
            else if (PAD_BTN_PRESSED(PAD_DPAD_RIGHT) || PAD_LANA_PRESSED(PAD_LANA_RIGHT))
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);

                dm_albm.csr[4] = 1 - dm_albm.csr[4];
            }
            else if (PAD_BTN_PRESSED(PAD_DPAD_LEFT) || PAD_LANA_PRESSED(PAD_LANA_LEFT))
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);

                dm_albm.csr[4] = 1 - dm_albm.csr[4];
            }
        break;
        }
    }

    return rtrn;
}

char PasteCtrl()
{
    char rtrn;

    rtrn = 0;

#if defined(BUILD_JP_VERSION)
    if (dm_albm.pst_yes_no == 0)
    {
        if (dm_albm.side == 0 && dm_albm.is_title == 0)
        {
            SeStartFix(2, 0, 0x1000, 0x1000, 0);

            rtrn = 1;
        }
        else if (dm_albm.mem != 0)
        {
            if (dm_albm.csr[dm_albm.side] < dm_albm.pht_max[dm_albm.side])
            {
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

                dm_albm.pst_yes_no = 1;
            }
            else
            {
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

                dm_albm.pst_yes_no = 0;

                Paste();

                rtrn = 1;
            }
        }
        else
        {
            SeStartFix(2, 0, 0x1000, 0x1000, 0);

            rtrn = 1;
        }
    }
    else
    {
        YesNoCrslOKR(0x2000, dm_albm.csr[4] * 120 + 367, 387.0f, 0x808080, 128.0f, 1.5f);
        PutStringYW(IGMSG_PRE_SV_MSG, 1, 475, 390, 0x808080, 0x80, 0x1000, 1);
        PutStringYW(IGMSG_MENU_MSG, 38, 60, 390, 0x808080, 0x80, 0x1000, 0);

        if (PAD_BTN_PRESSED(PAD_ACTION_EXIT))
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            dm_albm.pst_yes_no = 0;

            rtrn = 1;

        }
        else if (PAD_BTN_PRESSED(PAD_SQUARE) || PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);

            if (dm_albm.csr[4] == 0)
            {
                Paste();

                dm_albm.pst_yes_no = 0;

                rtrn = 1;
            }
            else
            {
                dm_albm.pst_yes_no = 0;

                rtrn = 1;
            }
        }
        else if (PAD_BTN_PRESSED(PAD_DPAD_RIGHT) || PAD_LANA_PRESSED(PAD_LANA_RIGHT))
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            dm_albm.csr[4] = 1 - dm_albm.csr[4];
        }
        else if (PAD_BTN_PRESSED(PAD_DPAD_LEFT) || PAD_LANA_PRESSED(PAD_LANA_LEFT))
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            dm_albm.csr[4] = 1 - dm_albm.csr[4];
        }
    }
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    if (dm_albm.pst_yes_no != 0)
    {
        YesNoCrslOKR(0x2000, dm_albm.csr[4] * 132 + 318, 387.0f, 0x808080, 128.0f, 2.0f);
        PutStringYW(IGMSG_PRE_SV_MSG, 1, 358, 390, 0x808080, 0x80, 0x1000, 1);
        PutStringYW(IGMSG_MENU_MSG, 38, 60, 390, 0x808080, 0x80, 0x1000, 0);

        if (PAD_BTN_PRESSED(PAD_ACTION_EXIT))
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            dm_albm.pst_yes_no = 0;

            rtrn = 1;
        }
        else if (PAD_BTN_PRESSED(PAD_SQUARE) || PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
        {
            if (dm_albm.csr[4] == 0)
            {
                dm_albm.paste_wait = 15;

                Paste();

                dm_albm.pst_yes_no = 0;
            } else {
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

                dm_albm.pst_yes_no = 0;

                rtrn = 1;
            }
        }
        else if (PAD_BTN_PRESSED(PAD_DPAD_RIGHT) || PAD_LANA_PRESSED(PAD_LANA_RIGHT))
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            dm_albm.csr[4] = 1 - dm_albm.csr[4];
        }
        else if (PAD_BTN_PRESSED(PAD_DPAD_LEFT) || PAD_LANA_PRESSED(PAD_LANA_LEFT))
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            dm_albm.csr[4] = 1 - dm_albm.csr[4];
        }
    }
    else if (dm_albm.paste_wait != 0)
    {
        dm_albm.paste_wait--;

        if (dm_albm.paste_wait == 0)
        {
            SeStartFix(0xb, 0, 0x1000, 0x1000, 0);

            rtrn = 1;
        }
    }
    else if (dm_albm.side == 0 && dm_albm.is_title == 0)
    {
        SeStartFix(2, 0, 0x1000, 0x1000, 0);

        rtrn = 1;
    }
    else if (dm_albm.mem != 0)
    {
        if (dm_albm.csr[dm_albm.side] < dm_albm.pht_max[dm_albm.side])
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);

            dm_albm.csr[4] = 1;
            dm_albm.pst_yes_no = 1;
        }
        else
        {
            dm_albm.paste_wait = 15;

            Paste();
        }
    }
    else
    {
        SeStartFix(2, 0, 0x1000, 0x1000, 0);

        rtrn = 1;
    }
#endif

    return rtrn;
}

void Paste()
{
    int i;

    CopyToOsugi(dm_albm.buf_sid[dm_albm.csr[2]], dm_albm.buf[dm_albm.csr[2]], dm_albm.side, dm_albm.csr[dm_albm.side]);

    dm_albm.buf[8] = 0xff;
    dm_albm.buf_sid[8] = 0xff;

    for (i = dm_albm.csr[2]; i < dm_albm.mem; i++)
    {
        dm_albm.buf[i] = dm_albm.buf[i + 1];
        dm_albm.buf_sid[i] = dm_albm.buf_sid[i + 1];
    }

    dm_albm.buf_set = 0;
    dm_albm.mem--;

    if (dm_albm.mem == 0)
    {
        dm_albm.csr_mode = 0;
    }
    else
    {
        dm_albm.csr_mode = 2;
    }

    if (dm_albm.mem - 1 < dm_albm.csr[2] && dm_albm.csr[2] != 0)
    {
        dm_albm.csr[2]--;
    }

    if (
        dm_albm.pht_max[dm_albm.side] - 1 < dm_albm.csr[dm_albm.side] &&
        dm_albm.pht_max[dm_albm.side] < 24
    )
    {
        dm_albm.pht_max[dm_albm.side]++;
    }
}

void BuffPadCmn()
{
    if (dm_albm.mode != 0)
    {
        return;
    }

    if (PAD_BTN_PRESSED(PAD_SQUARE))
    {
        SeStartFix(1, 0, 0x1000, 0x1000, 0);

        dm_albm.csr[3] = 0;
        dm_albm.csr_mode = 3;
    }
    else if (PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
    {
        SeStartFix(1, 0, 0x1000, 0x1000, 0);

        dm_albm.buf_exp_flg = 1;
        dm_albm.mode_chng = 20;
        dm_albm.mode = 1;
    }
    else if (PAD_BTN_PRESSED(PAD_ACTION_MOVE))
    {
        SeStartFix(1, 0, 0x1000, 0x1000, 0);

        dm_albm.buf_set = 1;
        dm_albm.csr_mode = 0;
        dm_albm.csr[dm_albm.side] = GetAlbumCurPos(dm_albm.side);
    }
    else if (PAD_BTN_REPEAT(PAD_DPAD_UP) || PAD_LANA_REPEAT(PAD_LANA_UP))
    {
        SeStartFix(0, 0, 0x1000, 0x1000, 0);

        dm_albm.csr_mode = 0;
    }
    else if (PAD_BTN_REPEAT(PAD_DPAD_LEFT) || PAD_LANA_REPEAT(PAD_LANA_LEFT))
    {
        if (dm_albm.csr[2] < dm_albm.mem - 1)
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            dm_albm.csr[2]++;
        }
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        else if (dm_albm.mem >= 2)
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            dm_albm.csr[2] = 0;
        }
        else
        {
            dm_albm.csr[2] = 0;
        }
#endif
    }
    else if (PAD_BTN_REPEAT(PAD_DPAD_RIGHT) || PAD_LANA_REPEAT(PAD_LANA_RIGHT))
    {
        if (dm_albm.csr[2] != 0)
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            dm_albm.csr[2]--;
        }
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        else
        {
            if (dm_albm.mem > 1)
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);
            }

            dm_albm.csr[2] = dm_albm.mem - 1;
        }
#endif
    }
    else if (PAD_BTN_PRESSED(PAD_R1))
    {
        if (dm_albm.side == 0)
        {
#if defined(BUILD_JP_VERSION)
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            dm_albm.side = 1;
            dm_albm.inf_alp = 0;
            dm_albm.side_chng = 20;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            dm_albm.side_chng_mode = 1;
            dm_albm.side_chng_wait = 15;
#endif
        }
    }
    else if (PAD_BTN_PRESSED(PAD_L1))
    {
        if (dm_albm.side == 1)
        {
#if defined(BUILD_JP_VERSION)
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            dm_albm.side = 0;
            dm_albm.inf_alp = 0;
            dm_albm.side_chng = 20;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            dm_albm.side_chng_mode = 0;
            dm_albm.side_chng_wait = 15;
#endif
        }
    }
}

void BuffPadMnu()
{
    int i;

    if (PAD_BTN_PRESSED(PAD_ACTION_EXIT))
    {
        SeStartFix(3, 0, 0x1000, 0x1000, 0);

        dm_albm.csr_mode = 2;
    }
    else if (PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
    {
        switch(dm_albm.csr[3])
        {
            case 0:
            {
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

                dm_albm.buf_exp_flg = 1;
                dm_albm.mode = 1;
                dm_albm.csr_mode = 2;
                dm_albm.mode_chng = 20;
            }
            break;
            case 1:
            {
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

                dm_albm.buf_set = 1;
                dm_albm.csr_mode = 0;
                dm_albm.csr[dm_albm.side] = GetAlbumCurPos(dm_albm.side);
            }
            break;
            case 2:
            {
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

                dm_albm.buf[8] = 0xff;
                dm_albm.buf_sid[8] = 0xff;
                
                for (i = dm_albm.csr[2]; i < dm_albm.mem; i++)
                {
                    dm_albm.buf[i] = dm_albm.buf[i+1];
                    dm_albm.buf_sid[i] = dm_albm.buf_sid[i+1];
                }
                
                dm_albm.buf_set = 0;
                dm_albm.mem--;
                
                if (dm_albm.mem == 0)
                {
                    dm_albm.csr_mode = 0;
                }
                else
                {
                    dm_albm.csr_mode = 2;
                }

                if (dm_albm.csr[2] > dm_albm.mem - 1 && dm_albm.csr[2] != 0)
                {
                    dm_albm.csr[2]--;
                }
            }
        break;
        }
    }
    else if (PAD_BTN_REPEAT(PAD_DPAD_DOWN) || PAD_LANA_REPEAT(PAD_LANA_DOWN))
    {
        SeStartFix(0, 0, 0x1000, 0x1000, 0);

        if (dm_albm.csr[3] > 1)
        {
            dm_albm.csr[3] = 0;
        }
        else
        {
            dm_albm.csr[3]++;
        }
    }
    else if (PAD_BTN_REPEAT(PAD_DPAD_UP) || PAD_LANA_REPEAT(PAD_LANA_UP))
    {
        SeStartFix(0, 0, 0x1000, 0x1000, 0);

        if (dm_albm.csr[3] != 0)
        {
            dm_albm.csr[3]--;
        }
        else
        {
            dm_albm.csr[3] = 2;
        }
    }
}

void CopyToOsugi(u_char from, u_char no1, u_char to, u_char no2)
{
    u_char pht_adr2;
    u_char pht_adr1;

    pht_adr1 = album_save_dat[from].pic[no1].adr_no;
    pht_adr2 = album_save_dat[to].pic[no2].adr_no;

    CopyDataWithoutAdr(from, no1, to, no2);

    switch (from)
    {
        case 0:
            if (to == 0)
            {
                CopySPhoto(dm_albm.src_addr, pht_adr1, dm_albm.src_addr, pht_adr2);
                CopyPhoto(dm_albm.src_addr + 0x15400, pht_adr1, dm_albm.src_addr + 0x15400, pht_adr2);
            }
            else
            {
                CopySPhoto(dm_albm.src_addr, pht_adr1, dm_albm.cpy_addr, pht_adr2);
                CopyPhoto(dm_albm.src_addr + 0x15400, pht_adr1, dm_albm.cpy_addr + 0x15400, pht_adr2);
            }
        break;
        case 1:
            if (to == 0)
            {
                CopySPhoto(dm_albm.cpy_addr, pht_adr1, dm_albm.src_addr, pht_adr2);
                CopyPhoto(dm_albm.cpy_addr + 0x15400, pht_adr1, dm_albm.src_addr + 0x15400, pht_adr2);
            }
            else
            {
                CopySPhoto(dm_albm.cpy_addr, pht_adr1, dm_albm.cpy_addr, pht_adr2);
                CopyPhoto(dm_albm.cpy_addr + 0x15400, pht_adr1, dm_albm.cpy_addr + 0x15400, pht_adr2);
            }
        break;
    }
}

void CopyDataWithoutAdr(u_char from, u_char no1, u_char to, u_char no2)
{
    u_char adr_no = album_save_dat[to].pic[no2].adr_no;
    album_save_dat[to].pic[no2] = album_save_dat[from].pic[no1];
    album_save_dat[to].pic[no2].adr_no = adr_no;
}

void DelPyon(u_char file, u_char csr)
{
    int i;
    int j;
    int dust;

    dm_albm.buf_sid[dm_albm.mem] = 0xff;
    dm_albm.buf[dm_albm.mem] = 0xff;

    i = 0;

    while (dm_albm.buf[i] != 0xff)
    {
        if (dm_albm.buf_sid[i] == file && dm_albm.buf[i] == csr)
        {
            for (j = i; j < dm_albm.mem; j++)
            {
                dm_albm.buf_sid[j] = dm_albm.buf_sid[j + 1];
                dm_albm.buf[j] = dm_albm.buf[j + 1];
            }

            dm_albm.mem--;
        }
        else
        {
            i++;
        }
    }

    for (j = 0; j < dm_albm.mem; j++)
    {
        if (dm_albm.buf_sid[j] == file && csr < dm_albm.buf[j])
        {
            dm_albm.buf[j]--;
        }
    }

    if (dm_albm.pht_max[file] > csr)
    {
        SeStartFix(1, 0, 0x1000, 0x1000, 0);

        dust = album_save_dat[file].pic[csr].adr_no;

        for (i = csr; i < 23; i++)
        {
            album_save_dat[file].pic[i] = album_save_dat[file].pic[i + 1];
        }

        album_save_dat[file].pic[23].status = 0;
        album_save_dat[file].pic[23].adr_no = dust;

        dm_albm.pht_max[file]--;
    }
    else
    {
        SeStartFix(2, 0, 0x1000, 0x1000, 0);
    }
}

void DspOgAlbumTop1(u_char alp, u_char alp0, u_char alp1)
{
    short int px[8] = {
        0x39, 0xb0, 0x162, 0x1d9,
        0x39, 0xb0, 0x162, 0x1d9,
    };
    short int py[8] = {
        0x64,  0x85, 0x64,  0x85,
        0xe5, 0x106, 0xe5, 0x106,
    };

    switch (dm_albm.is_title)
    {
    case 0:
        switch (dm_albm.side)
        {
        case 0:
            SetSprFile(LOAD_ADDRESS_33);
        break;
        case 1:
            SetSprFile(LOAD_ADDRESS_34);
        break;
        }
    break;
    case 1:
        switch (dm_albm.side)
        {
        case 0:
            SetSprFile(LOAD_ADDRESS_33);
        break;
        case 1:
            SetSprFile(LOAD_ADDRESS_35);
        break;
        }
    break;
    }

    switch (dm_albm.side)
    {
    case 0:
        switch (dm_albm.is_title)
        {
        case 0:
            PutSpriteYW(TP_BG_SML_WS + dm_albm.cl_type[0], TP_BG_SML_WS + dm_albm.cl_type[0], 193.0f, 154.0f, 0.0f, 0x808080, alp0, 2.3307087f, 2.3410852f, 0, 0xff, 1, 0, 0);
        break;
        case 1:
            PutSpriteYW(TP_BG_BIG_WS + dm_albm.cl_type[0], TP_BG_BIG_WS + dm_albm.cl_type[0], 64.0f, 90.0f, 0.0f, 0x808080, alp0, 1.1607844f, 1.1615385f, 0, 0xff, 1, 0, 0);
        break;
        }
    break;
    case 1:
        switch (dm_albm.is_title)
        {
        case 0:
            PutSpriteYW(TP_BG_SML_WS + dm_albm.cl_type[1], TP_BG_SML_WS + dm_albm.cl_type[1], 193.0f, 154.0f, 0.0f, 0x808080, alp1, 2.3307087f, 2.3410852f, 0, 0xff, 1, 0, 0);
        break;
        case 1:
            PutSpriteYW(TP_BG_BIG_WS + dm_albm.cl_type[1], TP_BG_BIG_WS + dm_albm.cl_type[1], 64.0f, 90.0f, 0.0f, 0x808080, alp1, 1.1607844f, 1.1615385f, 0, 0xff, 1, 0, 0);
        break;
        }
    break;
    }

    switch (dm_albm.side)
    {
    case 0:
        InfoWin(0, 0, alp0);
    break;
    case 1:
        InfoWin(0, 1, alp1);
    break;
    }

    if (dm_albm.dat_alp != 0)
    {
        switch (dm_albm.pop_ud)
        {
        case 0:
            BtnExt(1, alp);
            Navion(1, 0, alp);
        break;
        case 1:
            BtnExt(0, alp);
            Navion(0, 0, alp);
        break;
        }
    }
    else
    {
        if (dm_albm.menu_alp != 0)
        {
            Navion(1, 0, alp);
        }
        else
        {
            BtnExt(0, alp);
            Navion(1, 0, alp);
        }
    }

    if (dm_albm.yes_no == 2 || dm_albm.pst_yes_no == 1)
    {
        BufWin(1, alp);
    }
    else
    {
        BufWin(0, alp);
    }
}

void DspOgAlbumTop2(u_char alp, u_char alp0, u_char alp1)
{
    if (dm_albm.dat_alp != 0)
    {
        switch (dm_albm.pop_ud)
        {
        case 0:
            DtlWin(0, 0, dm_albm.dat_alp);
        break;
        case 1:
            DtlWin(0, 1, dm_albm.dat_alp);
        break;
        }
    }
}

void DspOgAlbumBig(u_char alp)
{
    u_char side;

    if (dm_albm.csr_mode == 2)
    {
        side = dm_albm.buf_sid[dm_albm.csr[2]];
    }
    else
    {
        side = dm_albm.side;
    }

    switch (dm_albm.is_title)
    {
        case 0:
            switch (side)
            {
            case 0:
                SetSprFile(LOAD_ADDRESS_33);
            break;
            case 1:
                SetSprFile(LOAD_ADDRESS_34);
            break;
            }
        break;
        case 1:
            switch (side)
            {
            case 0:
                SetSprFile(LOAD_ADDRESS_33);
            break;
            case 1:
                SetSprFile(LOAD_ADDRESS_35);
            break;
            }
        break;
    }

    switch (dm_albm.is_title)
    {
    case 0:
        PutSpriteYW(KK_BG_SML_WX + dm_albm.cl_type[side], KK_BG_SML_WX + dm_albm.cl_type[side], 193.0f, 139.0f, 0.0f, 0x808080, alp, 2.3307087f, 2.35f, 0, 0xff, 1, 0, 0);
    break;
    case 1:
        PutSpriteYW(KK_BG_BIG_WX + dm_albm.cl_type[side], KK_BG_BIG_WX + dm_albm.cl_type[side], 64.0f, 79.0f, 0.0f, 0x808080, alp, 1.1607844f, 1.1652893f,  0, 0xff, 1, 0, 0);
    break;
    }

    InfoWin(1, side, alp);
    DtlWin(1, 0, alp);
    Navion(1, 1, alp);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    DispCaption(20, alp);
#endif
}

void InfoWin(u_char mode, u_char side, u_char alp)
{
    u_char alp0;
    u_char alp1;
    short int adj1;
    short int adj2;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    float rad;
    float flsh1;
#endif

    if (dm_albm.inf_alp + 8 < alp) {
        dm_albm.inf_alp += 8;
    }
    else
    {
        dm_albm.inf_alp = alp;
    }

    switch (side)
    {
    case 0:
        alp0 = dm_albm.inf_alp;
        alp1 = alp - dm_albm.inf_alp;
    break;
        case 1:
        alp1 = dm_albm.inf_alp;
        alp0 = alp - dm_albm.inf_alp;
    break;
    }

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    if (dm_albm.flsh_tmr <= 58)
    {
        dm_albm.flsh_tmr++;
    }
    else
    {
        dm_albm.flsh_tmr = 0;
    }

    rad = (dm_albm.flsh_tmr % 60) * (PI * 2) / 60.0f;

    if (dm_albm.side_chng_wait == 0)
    {
        flsh1 = ((SgSinf(rad) + 1.0f) * 0.5f + 1.0f) * 0.6f * 0.5f + 0.3f;
    }
    else
    {
        flsh1 = 1.0f;
    }
#endif

    if (mode == 0)
    {
        adj1 = 0;
        adj2 = 0;
    }
    else
    {
        adj1 = -34;
        adj2 = 26;
    }

    if (alp1 != 0)
    {
        DrawMessageBox(0x32000, adj1 + 60, 17.0f, 582.0f, 48.0f, alp1);

        if (mode != 1)
        {
#if defined(BUILD_JP_VERSION)
            PutSpriteYW(ALBM_CAPT_L1, ALBM_CAPT_L1, -52.0f, -6.0f, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0x30000, 1, 0, 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            if (dm_albm.side_chng_wait != 0)
            {
                PutSpriteYW(ALBM_CAPT_L1, ALBM_CAPT_L1, -52.0f, -6.0f, 0.0f, 0x808080, alp1 * flsh1, 1.0f, 1.0f, 0, 0x30000, 1, 1, 0);
            }
            else
            {
                PutSpriteYW(ALBM_CAPT_L1, ALBM_CAPT_L1, -52.0f, -6.0f, 0.0f, 0x808080, alp1 * flsh1, 1.0f, 1.0f, 0, 0x30000, 1, 0, 0);
            }
#endif
        }

        PutSpriteYW(OA_TP_FNT_SLSH, OA_TP_FNT_SLSH, adj1 + 102, 2.0f, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0x30000, 1, 0, 0);
#if defined(BUILD_JP_VERSION)
        PutSpriteYW(OA_TP_FNT_MAI, OA_TP_FNT_MAI, adj1 + 102, 2.0f, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0x30000, 1, 0, 0);
#endif
        PutNumberYW(11, dm_albm.pht_max[1], adj1 + 522, 29, 1.0f, 1.0f, 0x808080, alp1, 0x30000, 2, 0);
        PutNumberYW(11, 24, adj1 + 564, 29, 1.0f, 1.0f, 0x808080, alp1, 0x30000, 2, 0);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        if (dm_albm.csr_mode == 2)
        {
            if (mode != 0)
            {
                PutSpriteYW(OA_TP_FNT_SLSH, OA_TP_FNT_SLSH, adj1 + 60, 2.0f, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0x30000, 1, 0, 0);
                PutNumberYW(11, dm_albm.buf[dm_albm.csr[2]] + 1, adj1 + 480, 29, 1.0f, 1.0f, 0x808080, alp1, 0x30000, 2, 0);
            }
        }
        else if (dm_albm.csr[1] < dm_albm.pht_max[1])
        {
            PutSpriteYW(OA_TP_FNT_SLSH, OA_TP_FNT_SLSH, adj1 + 60, 2.0f, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0x30000, 1, 0, 0);
            PutNumberYW(11, dm_albm.csr[1] + 1, adj1 + 480, 29, 1.0f, 1.0f, 0x808080, alp1, 0x30000, 2, 0);
        }
#endif

        switch (dm_albm.slot[1])
        {
            case 0:
                if (alp1 == 0x80)
                {
                    PutStringYW(IGMSG_PRE_SV_MSG, 13, adj1 + 90, 29, 0x909090, 0x80, 0x2f000, 0);
                }
            break;
            case 1:
            case 2:
#if defined(BUILD_JP_VERSION)
                if (alp1 == 0x80)
                {
                    PutStringYW(IGMSG_PRE_SV_MSG, 14, adj1 + 90, 29, 0x909090, 0x80, 0x2f000, 0);
                    PutNumberYW(0, dm_albm.file[1], adj1 + 200, 29, 1.0f, 1.0f, 0x808080, 0x80, 0x2f000, 1, 0);
                }

                PutSpriteYW(OA_TP_FNT_MMC, OA_TP_FNT_MMC, adj1 + 240, 2.0f, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0x2f000, 1, 0, 0);
                PutNumberYW(11, dm_albm.slot[1], adj1 + 424, 29, 1.0f, 1.0f, 0x808080, alp1, 0x2f000, 1, 0);
#elif defined(BUILD_US_VERSION)
                if (alp1 == 0x80)
                {
                    PutStringYW(IGMSG_PRE_SV_MSG, 14, adj1 + 90, 29, 0x909090, 0x80, 0x2f000, 0);
                    PutNumberYW(0, dm_albm.file[1], adj1 + 180, 29, 1.0f, 1.0f, 0x808080, 0x80, 0x2f000, 1, 0);
                    PutStringYW(IGMSG_SV_PNT_MSG, 109, adj1 + 240, 29, 0x909090, 0x80, 0x2f000, 0);
                    PutNumberYW(0, dm_albm.slot[1], adj1 + 422, 29, 1.0f, 1.0f, 0x808080, 0x80, 0x2f000, 1, 0);
                }
#elif defined(BUILD_EU_VERSION)
                if (alp1 == 0x80)
                {
                    int pos_x;
                    int width;

                    PutStringYW(IGMSG_PRE_SV_MSG, 14, adj1 + 90, 29, 0x909090, 0x80, 0x2f000, 0);

                    width = GetStrWidth((u_char *)GetIngameMSGAddr(IGMSG_PRE_SV_MSG, 14));
                    pos_x = adj1 + 102;

                    PutNumberYW(0, dm_albm.file[1], pos_x + width, 29, 1.0f, 1.0f, 0x808080, 0x80, 0x2f000, 1, 0);
                }
#endif
            break;
            case 3:
                if (alp1 == 0x80)
                {
                    PutStringYW(IGMSG_PRE_SV_MSG, 15, adj1 + 90, 29, 0x909090, 0x80, 0x2f000, 0);
                }
            break;
        }
    }

    if (alp0 != 0)
    {
        DrawMessageBox(0x2d000, adj2, 17.0f, 582.0f, 48.0f, alp0);

        if (mode != 1)
        {
#if defined(BUILD_JP_VERSION)
            PutSpriteYW(ALBM_CAPT_R1, ALBM_CAPT_R1, 53.0f, adj2 + 1, 0.0f, 0x808080, alp0, 1.0f, 1.0f, 0, 0x2b000, 1, 0, 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            if (dm_albm.side_chng_wait != 0)
            {
                PutSpriteYW(ALBM_CAPT_R1, ALBM_CAPT_R1, 53.0f, adj2 + 1, 0.0f, 0x808080, alp0 * flsh1, 1.0f, 1.0f, 0, 0x2b000, 1, 1, 0);
            }
            else
            {
                PutSpriteYW(ALBM_CAPT_R1, ALBM_CAPT_R1, 53.0f, adj2 + 1, 0.0f, 0x808080, alp0 * flsh1, 1.0f, 1.0f, 0, 0x2b000, 1, 0, 0);
            }
#endif
        }

        PutSpriteYW(OA_TP_FNT_SLSH, OA_TP_FNT_SLSH, adj2 + 42, 2.0f, 0.0f, 0x808080, alp0, 1.0f, 1.0f, 0, 0x2b000, 1, 0, 0);
#if defined(BUILD_JP_VERSION)
        PutSpriteYW(OA_TP_FNT_MAI, OA_TP_FNT_MAI, adj2 + 42, 2.0f, 0.0f, 0x808080, alp0, 1.0f, 1.0f, 0, 0x2b000, 1, 0, 0);
#endif
        PutNumberYW(11, dm_albm.pht_max[0], adj2 + 462, 29, 1.0f, 1.0f, 0x808080, alp0, 0x2b000, 2, 0);
        PutNumberYW(11, 24, adj2 + 504, 29, 1.0f, 1.0f, 0x808080, alp0, 0x2b000, 2, 0);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        if (dm_albm.csr_mode == 2)
        {
            if (mode != 0)
            {
                PutSpriteYW(OA_TP_FNT_SLSH, OA_TP_FNT_SLSH, adj2, 2.0f, 0.0f, 0x808080, alp0, 1.0f, 1.0f, 0, 0x2b000, 1, 0, 0);
                PutNumberYW(11, dm_albm.buf[dm_albm.csr[2]] + 1, adj2 + 420, 29, 1.0f, 1.0f, 0x808080, alp0, 0x2b000, 2, 0);
            }
        }
        else if (dm_albm.csr[0] < dm_albm.pht_max[0])
        {
            PutSpriteYW(OA_TP_FNT_SLSH, OA_TP_FNT_SLSH, adj2, 2.0f, 0.0f, 0x808080, alp0, 1.0f, 1.0f, 0, 0x2b000, 1, 0, 0);
            PutNumberYW(11, dm_albm.csr[0] + 1, adj2 + 420, 29, 1.0f, 1.0f, 0x808080, alp0, 0x2b000, 2, 0);
        }
#endif

        switch (dm_albm.slot[0])
        {
            case 0:
                if (alp0 == 0x80)
                {
                    PutStringYW(IGMSG_PRE_SV_MSG, 13, adj2 + 30, 29, 0x909090, 0x80, 0x2a000, 0);
                }
            break;
            case 1:
            case 2:
#if defined(BUILD_JP_VERSION)
                if (alp0 == 0x80)
                {
                    PutStringYW(IGMSG_PRE_SV_MSG, 14, adj2 + 30, 29, 0x909090, 0x80, 0x2a000, 0);
                    PutNumberYW(0, dm_albm.file[0], adj2 + 140, 29, 1.0f, 1.0f, 0x808080, 0x80, 0x2a000, 1, 0);
                }

                PutSpriteYW(OA_TP_FNT_MMC, OA_TP_FNT_MMC, adj2 + 180, 2.0f, 0.0f, 0x808080, alp0, 1.0f, 1.0f, 0, 0x2a000, 1, 0, 0);
                PutNumberYW(11, dm_albm.slot[0], adj2 + 364, 29, 1.0f, 1.0f, 0x808080, alp0, 0x2a000, 1, 0);
#elif defined(BUILD_US_VERSION)
                if (alp0 == 0x80)
                {
                    PutStringYW(IGMSG_PRE_SV_MSG, 14, adj2 + 30, 29, 0x909090, 0x80, 0x2a000, 0);
                    PutNumberYW(0, dm_albm.file[0], adj2 + 120, 29, 1.0f, 1.0f, 0x808080, 0x80, 0x2a000, 1, 0);
                    PutStringYW(IGMSG_SV_PNT_MSG, 109, adj2 + 180, 29, 0x909090, 0x80, 0x2a000, 0);
                    PutNumberYW(0, dm_albm.slot[0], adj2 + 362, 29, 1.0f, 1.0f, 0x808080, 0x80, 0x2a000, 1, 0);
                }
#elif defined(BUILD_EU_VERSION)
                if (alp0 == 0x80)
                {
                    int pos_x;
                    int width;

                    PutStringYW(IGMSG_PRE_SV_MSG, 14, adj2 + 30, 29, 0x909090, 0x80, 0x2a000, 0);

                    width = GetStrWidth((u_char *)GetIngameMSGAddr(IGMSG_PRE_SV_MSG, 14));
                    pos_x = adj2 + 42;

                    PutNumberYW(0, dm_albm.file[0], pos_x + width, 29, 1.0f, 1.0f, 0x808080, 0x80, 0x2a000, 1, 0);
                }
#endif
            break;
            case 3:
                if (alp0 == 0x80)
                {
                    PutStringYW(IGMSG_PRE_SV_MSG, 15, adj2 + 30, 29, 0x909090, 0x80, 0x2a000, 0);
                }
            break;
        }
    }
}

void Navion(u_char type, u_char sw, u_char alp)
{
    u_char max;
    u_char csr;
    u_char alp_l;
    u_char alp_r;
    static FLSH_CORE flsh;

    if (alp != 0x80)
    {
        return;
    }

    FlashStarYW(&flsh, 192, 64, 60, 0);

    if (sw == 0)
    {
        max = dm_albm.pht_max[dm_albm.side];
        csr = dm_albm.csr[dm_albm.side];

        if (max > 7)
        {
            if (csr < 8)
            {
                if (dm_albm.side == 0 && dm_albm.is_title == 0)
                {
                    if (max < 9)
                    {
                       alp_l = 0; alp_r = 0;
                    }
                    else
                    {
                        alp_l = 0; alp_r = flsh.alpha;
                    }
                }
                else
                {
                    alp_l = 0;
                    alp_r = flsh.alpha;
                }
            }
            else if (csr < 16)
            {
                if (dm_albm.side == 0 && dm_albm.is_title == 0)
                {
                    if (max < 17)
                    {
                        alp_r = 0;
                        alp_l = flsh.alpha;
                    }
                    else
                    {
                        alp_r = flsh.alpha;
                        alp_l = flsh.alpha;
                    }
                }
                else
                {
                    if (max < 16)
                    {
                        alp_l = flsh.alpha;
                        alp_r = 0;
                    }
                    else
                    {
                        alp_l = flsh.alpha;
                        alp_r = alp_l;
                    }
                }
            }
            else
            {
                alp_l = flsh.alpha;
                alp_r = 0;
            }
        }
        else
        {
            alp_l = 0;
            alp_r = 0;
        }
    }
    else
    {

        if (dm_albm.csr_mode == 2)
        {
            max = dm_albm.mem;
        }
        else
        {
            max = dm_albm.pht_max[dm_albm.side];
        }

        if (max > 1)
        {
            alp_l = flsh.alpha;
            alp_r = flsh.alpha;
        }
        else
        {
            alp_l = 0;
            alp_r = 0;
        }
    }

    SetSprFile(LOAD_ADDRESS_26);

    switch (type)
    {
    case 0:
        PutSpriteYW(TRIANGLE_LFT, TRIANGLE_LFT, -1.0f, -96.0f, 0.0f, 0x808080, alp_l, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(TRIANGLE_RGT, TRIANGLE_RGT, 1.0f, -96.0f, 0.0f, 0x808080, alp_r, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    break;
    case 1:
        PutSpriteYW(TRIANGLE_LFT, TRIANGLE_LFT, -1.0f, 4.0f, 0.0f, 0x808080, alp_l, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(TRIANGLE_RGT, TRIANGLE_RGT, 1.0f, 4.0f, 0.0f, 0x808080, alp_r, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    break;
    }
}

void ComWin(u_char mode, u_char csr, u_char alp)
{
    static FLSH_CORE flsh;
    u_char alp_csr;

    FlashStarYW(&flsh, 64, 128, 120, 0);

    if (alp == 0x80)
    {
        alp_csr = flsh.alpha;
    }
    else
    {
        alp_csr = alp;
    }

    switch (mode)
    {
    case 0:
    {
        VER_POLY_SQUARE_YW(30.0f, 66.0f, 104, 181, 0, (int)(alp / 3), 1.0f, 1.0f, 0x4000, 0, 0);

        CmnWindow(27, 0, 0, alp, 0x80);

        PutSpriteYW(OA_TP_FNT_ZM2, OA_TP_FNT_EXT, 40.0f, 68.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        if (dm_albm.csr_mode == 3)
        {
            VER_POLY_SQUARE_YW(38.0f, 158.0f, 88, 28, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0);
            VER_POLY_SQUARE_YW(38.0f, 188.0f, 88, 28, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0);
            VER_POLY_SQUARE_YW(38.0f, 218.0f, 88, 28, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0);
        }
        else if (dm_albm.side == 0 && dm_albm.is_title == 0)
        {
            VER_POLY_SQUARE_YW(38.0f, 128.0f, 88, 28, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0);
            VER_POLY_SQUARE_YW(38.0f, 158.0f, 88, 28, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0);
            VER_POLY_SQUARE_YW(38.0f, 188.0f, 88, 28, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0);
        }

        CmnCursol(37, (u_short)csr * 30 + 67, 90, 30, 64.0f, alp_csr, 0x1000);
    }
    break;
    case 1:
    {
        VER_POLY_SQUARE_YW(505.0f, 66.0f, 104, 181, 0, (int)(alp / 3), 1.0f, 1.0f, 0x4000, 0, 0);

        CmnWindow(27, 475, 0, alp, 0x80);

        PutSpriteYW(OA_TP_FNT_ZM2, OA_TP_FNT_EXT, 515.0f, 68.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        if (dm_albm.csr_mode == 3)
        {
            VER_POLY_SQUARE_YW(513.0f, 158.0f, 88, 28, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0);
            VER_POLY_SQUARE_YW(513.0f, 188.0f, 88, 28, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0);
            VER_POLY_SQUARE_YW(513.0f, 218.0f, 88, 28, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0);
        }
        else if (dm_albm.side == 0 && dm_albm.is_title == 0)
        {
            VER_POLY_SQUARE_YW(513.0f, 128.0f, 88, 28, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0);
            VER_POLY_SQUARE_YW(513.0f, 158.0f, 88, 28, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0);
            VER_POLY_SQUARE_YW(513.0f, 188.0f, 88, 28, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0);
        }

        CmnCursol(512, csr * 30 + 67, 90, 30, 64.0f, alp_csr, 0x1000);
    }
    break;
    }

    DrawSPhotoFromSmallPhotoArea(0, 0x1000, 0, 0, 0, 0);
}

void BufWin(u_char type, u_char alp)
{
    int i;
    u_char tmp;
    u_char pht_adr;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    float flsh1;
    float flsh2;
#endif

    switch (type)
    {
    case 0:
    {
        DrawSPhotoFromSmallPhotoArea(0, 0, 0, 0, 0, 0);

        PutSpriteYW(OA_TP_FNT_BFFR, OA_TP_FNT_BFFR, 0.0f, 0.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        CmnWindow(29, 0, 0, alp, 0x80);

        tmp = 0;

        for (i = dm_albm.mem - 1; i >= 0; i--)
        {
            if (dm_albm.buf_sid[i] == 0)
            {
                pht_adr = album_save_dat[dm_albm.buf_sid[i]].pic[dm_albm.buf[i]].adr_no;

                DrawSPhotoFromSmallPhotoArea2AD(dm_albm.src_addr, pht_adr, 0x14000, 0, tmp * 58 + 137, 385, 52, 36, alp);
            }
            else
            {
                pht_adr = album_save_dat[dm_albm.buf_sid[i]].pic[dm_albm.buf[i]].adr_no;

                DrawSPhotoFromSmallPhotoArea2AD(dm_albm.cpy_addr, pht_adr, 0x14000, 0, tmp * 58 + 137, 385, 52, 36, alp);
            }

            PutSpriteYW(ALBM_SAM_WAKU, ALBM_SAM_WAKU, tmp * 58 + 137, 384.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            if (dm_albm.copy_wait != 0 && tmp == 0)
            {
                flsh1 = (dm_albm.copy_wait / 15.0f);

                VER_POLY_SQUARE_YW(137.0f, 385.0f, 52, 36, 0x808080, flsh1 * 128.0f, 1.0f, 1.0f, 0xf000, 1, 0);
            }
#endif

            if (dm_albm.csr[2] == i)
            {
                if ((dm_albm.csr_mode == 2 || dm_albm.csr_mode == 3) || dm_albm.buf_set != 0)
                {
#if defined(BUILD_JP_VERSION)
                    PutSpriteYW(ALBM_MINI_SEALING, ALBM_MINI_SEALING, tmp * 58 + 136, 383.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                    if (dm_albm.paste_wait != 0)
                    {
                        float rad = ((dm_albm.paste_wait % 15) * (PI * 2)) / 15.0f;

                        flsh2 = ((SgSinf(rad) + 1.0f) * 0.5f + 1.0f) * 0.5f;

                        PutSpriteYW(ALBM_MINI_SEALING, ALBM_MINI_SEALING, tmp * 58 + 136, 383.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                        PutSpriteYW(ALBM_MINI_SEALING, ALBM_MINI_SEALING, tmp * 58 + 136, 383.0f, 0.0f, 0x808080, alp * flsh2, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
                    }
                    else
                    {
                        PutSpriteYW(ALBM_MINI_SEALING, ALBM_MINI_SEALING, tmp * 58 + 136, 383.0f, 0.0f, 0x606060, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                    }
#endif
                }
            }

            tmp++;
        }
    }
    break;
    case 1:
        DrawMessageBox(0x19000, 24.0f, 377.0f, 592.0f, 48.0f, alp);
    break;
    }
}

void DtlWin(u_char mode, u_char type, u_char alp)
{
    u_char target;

    switch (mode)
    {
    case 0: {
        target = album_save_dat[dm_albm.side].pic[dm_albm.csr[dm_albm.side]].adr_no;

        if (dm_albm.side == 0)
        {
            UncompressPhotoAD(dm_albm.src_addr + 0x15400, target);
        }
        else
        {
            UncompressPhotoAD(dm_albm.cpy_addr + 0x15400, target);
        }

        switch (type)
        {
        case 0:
            if (CheckPhotoExpandEnd())
            {
                CmnWindow(39, 0, -196, alp, 0x80);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                PutSpriteYW(ALBM_FNT_JOHO, ALBM_FNT_JOHO, 15.0f, 133.0f, -90.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
#endif
                PutSpriteYW(ALBM_FNT_TARGET2, ALBM_FNT_POINT2, 0.0f, -149.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0x6000, 1, 0, 0);

                DrawSPhotoFromSmallPhotoArea(0, 0x5000, 0, 0, 0, 0);

                VER_POLY_SQUARE_YW(71.0f, 76.0f, 226, 144, 0, alp, 1.0f, 1.0f, 0x4000, 0, 0);

                DrawPhotoFromPhotoWrk(target, 0x3000, 0, 71, 76, 226, 144, alp);

                PutSpriteYW(ALBM_WAKU_BIG_U2, ALBM_WAKU_BIG_U2, -25.0f, -31.0f, 0.0f, 0x808080, alp, 18.833334f, 1.0f, 0, 0x2000, 1, 0, 0);
                PutSpriteYW(ALBM_WAKU_BIG_D2, ALBM_WAKU_BIG_D2, -25.0f, -31.0f, 0.0f, 0x808080, alp, 18.833334f, 1.0f, 0, 0x2000, 1, 0, 0);
                PutSpriteYW(ALBM_WAKU_BIG_L2, ALBM_WAKU_BIG_L2, -25.0f, -31.0f, 0.0f, 0x808080, alp, 1.0f, 12.0f, 0, 0x2000, 1, 0, 0);
                PutSpriteYW(ALBM_WAKU_BIG_R2, ALBM_WAKU_BIG_R2, -25.0f, -31.0f, 0.0f, 0x808080, alp, 1.0f, 12.0f, 0, 0x2000, 1, 0, 0);

                TruthPhoto(0, 0, -195, alp);
                DrawSPhotoFromSmallPhotoArea(0, 0x1000, 0, 0, 0, 0);
            }
            break;
        case 1:
            if (CheckPhotoExpandEnd())
            {
                CmnWindow(39, 0, -47, alp, 0x80);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                PutSpriteYW(ALBM_FNT_JOHO, ALBM_FNT_JOHO, 15.0f, 289.0f, -90.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
#endif
                PutSpriteYW(ALBM_FNT_TARGET2, ALBM_FNT_POINT2, 0.0f, 0.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0x6000, 1, 0, 0);

                DrawSPhotoFromSmallPhotoArea(0, 0x5000, 0, 0, 0, 0);

                VER_POLY_SQUARE_YW(71.0f, 225.0f, 226, 144, 0, alp, 1.0f, 1.0f, 0x4000, 0, 0);

                DrawPhotoFromPhotoWrk(target, 0x3000, 0, 71, 225, 226, 144, alp);

                PutSpriteYW(ALBM_WAKU_BIG_U2, ALBM_WAKU_BIG_U2, -25.0f, 118.0f, 0.0f, 0x808080, alp, 18.833334f, 1.0f, 0, 0x2000, 1, 0, 0);
                PutSpriteYW(ALBM_WAKU_BIG_D2, ALBM_WAKU_BIG_D2, -25.0f, 118.0f, 0.0f, 0x808080, alp, 18.833334f, 1.0f, 0, 0x2000, 1, 0, 0);
                PutSpriteYW(ALBM_WAKU_BIG_L2, ALBM_WAKU_BIG_L2, -25.0f, 118.0f, 0.0f, 0x808080, alp, 1.0f, 12.0f, 0, 0x2000, 1, 0, 0);
                PutSpriteYW(ALBM_WAKU_BIG_R2, ALBM_WAKU_BIG_R2, -25.0f, 118.0f, 0.0f, 0x808080, alp, 1.0f, 12.0f, 0, 0x2000, 1, 0, 0);

                TruthPhoto(0, 0, -46, alp);
                DrawSPhotoFromSmallPhotoArea(0, 0x1000, 0, 0, 0, 0);
            }
            break;
        }
    }
    break;
    case 1:
        CmnWindow(6, 0, 0, alp, 0x80);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutSpriteYW(ALBM_FNT_JOHO, ALBM_FNT_JOHO, 14.0f, 371.0f, -90.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
#endif
        PutSpriteYW(ALBM_FNT_TARGET, ALBM_FNT_POINT, 0.0f, 0.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        TruthPhoto(1, 0, 0, alp);
    break;
    }
}

void TruthPhoto(u_char mode, short int px, short int py, u_char alp)
{
    int i;
    DISP_STR ds;
    u_char obj_num;
    u_char str_alp;
    u_short obj_py[4][3] = {
        { 24, 24, 24 },
        { 24, 24, 24 },
        { 12, 36, 24 },
        {  0, 24, 48 },
    };
    u_short obj_py2[4][3] = {
        { 24, 24, 24 },
        { 24, 24, 24 },
        { 12, 36, 24 },
        {  0, 24, 48 },
    };
    PICTURE_WRK tmp_pic;
    u_short off_x_1;
    u_short off_x_2;

    off_x_1 = 440;
    off_x_2 = 185;

    if (alp == dm_albm.all_alp)
    {
        str_alp = dm_albm.all_alp;
    }
    else
    {
        str_alp = 0;
    }

    switch (mode)
    {
    case 0:
    {
        tmp_pic = album_save_dat[dm_albm.side].pic[dm_albm.csr[dm_albm.side]];

        for (obj_num = 0, i = 0; i < 3; i++)
        {
            if (tmp_pic.subject[i][0] != 0)
            {
                obj_num++;
            }
        }

        if (obj_num != 0)
        {
            for (i = 0; i < obj_num; i++)
            {
                if (tmp_pic.subject[i][0] != 0)
                {
                    ig_menu_str.str = GetSubjectNameAddr(tmp_pic.subject[i][0], tmp_pic.subject[i][1], tmp_pic.msn_no);

                    CopyStrDToStr(&ds, &ig_menu_str);

#if defined(BUILD_JP_VERSION)
                    ds.pos_x = px + 380;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                    ds.pos_x = px + 370;
#endif
                    ds.pos_y = obj_py[obj_num][i] + 267 + py;

                    ds.r = 0x80;
                    ds.g = 0x80;
                    ds.b = 0x80;

                    ds.alpha = str_alp;

                    ds.pri = 0x1000;

                    SetMessageV2(&ds);
                }
            }
        }
        else
        {
            PutStringYW(33, 3, px + off_x_1, obj_py[0][0] + 267 + py, 0x808080, str_alp, 0x1000, 0);
        }

        PutDate(&tmp_pic, px + 373, py + 340, str_alp, 0x1000);

        PutStringYW(30, tmp_pic.room, px + 385, py + 369, 0x808080, str_alp, 0x1000, 0);
        PutNumberYW(0, tmp_pic.score, px + 421, py + 397, 1.0f, 1.0f, 0x808080, str_alp, 0x1000, 7, 0);
    }
    break;
    case 1:
    {
        if (dm_albm.csr_mode == 2)
        {
            tmp_pic = album_save_dat[dm_albm.buf_sid[dm_albm.csr[2]]].pic[dm_albm.buf[dm_albm.csr[2]]];
        }
        else
        {
            tmp_pic = album_save_dat[dm_albm.side].pic[dm_albm.csr[dm_albm.side]];
        }

        for (obj_num = 0, i = 0; i < 3; i++)
        {
            if (tmp_pic.subject[i][0] != 0)
            {
                obj_num++;
            }
        }

        if (obj_num != 0)
        {
            for (i = 0; i < obj_num; i++)
            {
                if (tmp_pic.subject[i][0] != 0)
                {
                    ig_menu_str.str = GetSubjectNameAddr(tmp_pic.subject[i][0], tmp_pic.subject[i][1], tmp_pic.msn_no);

                    CopyStrDToStr(&ds, &ig_menu_str);

#if defined(BUILD_JP_VERSION)
                    ds.pos_x = px + 120;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                    ds.pos_x = px + 102;
#endif
                    ds.pos_y = obj_py2[obj_num][i] + 347 + py;

                    ds.r = 0x80;
                    ds.g = 0x80;
                    ds.b = 0x80;

                    ds.alpha = str_alp;

                    ds.pri = 0x1000;

                    SetMessageV2(&ds);
                }
            }
        }
        else
        {
            PutStringYW(33, 3, px + off_x_2, obj_py2[0][0] + 347 + py, 0x808080, str_alp, 0x1000, 0);
        }

        PutDate(&tmp_pic, px + 388, py + 340, str_alp, 0x1000);

        PutStringYW(30, tmp_pic.room, px + 399, py + 369, 0x808080, str_alp, 0x1000, 0);
        PutNumberYW(0, tmp_pic.score, px + 435, py + 396, 1.0f, 1.0f, 0x808080, str_alp, 0x1000, 7, 0);
    }
    break;
    }
}

void BtnExt(u_char mode, u_char alp)
{
    if (dm_albm.buf_set != 0)
    {
        return;
    }

    switch (mode)
    {
    case 0:
#if defined(BUILD_JP_VERSION)
        DrawButtonTex(0x5000, 0, 365.0f, 61.0f, alp);
        DrawButtonTex(0x5000, 2, 471.0f, 61.0f, alp);
        DrawButtonTex(0x5000, 1, 550.0f, 61.0f, alp);

        PutSpriteYW(OA_TP_FNT_MNU, OA_TP_FNT_MNU, 0.0f, 0.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(OA_TP_FNT_ZOM, OA_TP_FNT_ZOM, -6.0f, 0.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(OA_TP_FNT_MOV, OA_TP_FNT_MOV, 0.0f, 0.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        SetSprFile(LOAD_ADDRESS_26);

        DispCaption(22, alp);
#endif
    break;
    case 1:
#if defined(BUILD_JP_VERSION)
        DrawButtonTex(0x5000, 0, 365.0f, 349.0f, alp);
        DrawButtonTex(0x5000, 2, 471.0f, 349.0f, alp);
        DrawButtonTex(0x5000, 1, 550.0f, 349.0f, alp);

        PutSpriteYW(OA_TP_FNT_MNU, OA_TP_FNT_MNU, 0.0f, 288.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(OA_TP_FNT_ZOM, OA_TP_FNT_ZOM, -6.0f, 288.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(OA_TP_FNT_MOV, OA_TP_FNT_MOV, 0.0f, 288.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        SetSprFile(LOAD_ADDRESS_26);

        DispCaption(22, alp);
#endif
    break;
    case 2:
#if defined(BUILD_JP_VERSION)
        DrawButtonTex(0x5000, 0, 0.0f, 20.0f, alp);
        DrawButtonTex(0x5000, 1, 0.0f, 40.0f, alp);

        PutSpriteYW(OA_TP_FNT_MNU, OA_TP_FNT_MNU, 0.0f, 0.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(OA_TP_FNT_MOV, OA_TP_FNT_MOV, 0.0f, 0.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        // do nothing ...
#endif
    break;
    }
}

char InKeyTamusi()
{
	u_char hit;

    hit = 0;

    if (*key_now[PAD_DPAD_UP] != 0 || Ana2PadDirCnt(PAD_LANA_UP))
    {
        hit++;
    }

    if (*key_now[PAD_DPAD_DOWN] != 0 || Ana2PadDirCnt(PAD_LANA_DOWN))
    {
        hit++;
    }

    if (*key_now[PAD_DPAD_LEFT] != 0 || Ana2PadDirCnt(PAD_LANA_LEFT))
    {
        hit++;
    }

    if (*key_now[PAD_DPAD_RIGHT] != 0 || Ana2PadDirCnt(PAD_LANA_RIGHT))
    {
        hit++;
    }

    if (*key_now[PAD_SQUARE] != 0)
    {
        hit++;
    }

    if (*key_now[PAD_ACTION_MOVE] != 0)
    {
        hit++;
    }

    if (*key_now[PAD_ACTION_EXIT] != 0)
    {
        hit++;
    }

    if (*key_now[PAD_R1] != 0)
    {
        hit++;
    }

    if (*key_now[PAD_R2] != 0)
    {
        hit++;
    }

    if (*key_now[PAD_L1] != 0)
    {
        hit++;
    }

    if (*key_now[PAD_L2] != 0)
    {
        hit++;
    }

    if (*key_now[PAD_SELECT] != 0)
    {
        hit++;
    }

#if defined(BUILD_JP_VERSION)
    if (*key_now[PAD_START] != 0)
    {
        hit++;
    }
#endif

    if (dm_albm.exit_realy != 0 || dm_albm.yes_no != 0)
    {
        hit++;
    }

    return hit;
}

char GetAlbumCurPos(char side)
{
    if (dm_albm.side != 0 || dm_albm.is_title != 0)
    {
        if (dm_albm.pht_max[side] < 24)
        {
            return dm_albm.pht_max[side];
        }
    }

    return dm_albm.csr[side];
}
