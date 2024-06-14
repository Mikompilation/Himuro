#include "common.h"
#include "typedefs.h"
#include "structs.h"
#include "enums.h"
#include "memory_album.h"

#include "ingame/menu/item.h"
#include "ingame/menu/ig_camra.h"
#include "ingame/menu/ig_menu.h"
// #include "ingame/menu/ig_album.h"
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

void MemAlbmInit(
    u_char h_type, u_char src_pht_max, u_char cpy_pht_max, u_char src_cl_type,
    u_char cpy_cl_type, u_char src_slot, u_char cpy_slot, u_char src_file, u_char cpy_file)
{
    memset(&dm_albm, 0, sizeof(DSP_MEM_ALBM));

    dm_albm.is_title = h_type;
    
    switch (dm_albm.is_title)
    {
        case 0:
            dm_albm.src_addr = 0x1a90000;
            dm_albm.cpy_addr = 0x05a0000;
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
            dm_albm.src_addr = 0x0e80000;
            dm_albm.cpy_addr = 0x1000000;
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
    
    dm_albm.csr[0] = 0;
    dm_albm.flsh_tmr = 0;
    dm_albm.side_chng_wait = 0;
    dm_albm.copy_wait = 0;
    dm_albm.paste_wait = 0;
    dm_albm.side = 0;
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
    dm_albm.flsh_tmr = 0;
    dm_albm.side_chng_wait = 0;
    dm_albm.copy_wait = 0;
    dm_albm.paste_wait = 0;
    dm_albm.side = side;
    dm_albm.csr[side] = 0;
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
  dm_albm.flsh_tmr = 0;
  dm_albm.side_chng_wait = 0;
  dm_albm.copy_wait = 0;
  dm_albm.paste_wait = 0;
}

void NewAlbumInit(u_char side)
{
    int i;
    
    memset(&album_save_dat[side], 0, sizeof(MC_ALBUM_SAVE));

    for (i = 23; i >= 0; i--)
    {
        album_save_dat[side].pic[i].adr_no = i;
    }
    
    dm_albm.mode_chng = 20;
    
    dm_albm.pht_max[side] = 0;
    dm_albm.cl_type[side] = 5;
    dm_albm.slot[side] = 3;
    dm_albm.file[side] = 0;
    dm_albm.csr[side] = 0;
    
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
    dm_albm.side = 0;
    dm_albm.csr[0] = 0;
    dm_albm.flsh_tmr = 0;
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
    case 0: {
        u_char alp_l = dm_albm.all_alp;
        if (alp_l + 8 < alp)
        {
            dm_albm.all_alp = alp_l + 8;
        } else { 
            dm_albm.all_alp = alp;
        }
    }
    break;

    case 1: {
        if (dm_albm.all_alp - 8 > 0)
        {
            dm_albm.all_alp -= 8;
        }
        else
        {
            dm_albm.all_alp = 0;
        }
    }
    break;
    }

    rtrn = dm_albm.all_alp;
    if (mode != 0)
    {
        float alp_l = dm_albm.all_alp;
        PutSpriteYW(OA_TP_BG_L, OA_TP_BG_L, 0.0f, 0.0f, 0.0f, 0x606060, alp_l, 1.2598425f, 1.258427f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(OA_TP_BG_R, OA_TP_BG_R, 0.0f, 0.0f, 0.0f, 0x606060, alp_l, 1.2598425f, 1.258427f, 0, 0xff, 1, 0, 0);
    }
    
    if (dm_albm.mode_chng != 0)
    {
        dm_albm.mode_chng--;
    }
    
    switch (dm_albm.mode)
    {
    case 0:
        alp_m1 = (rtrn * dm_albm.mode_chng) / 20.0f;
        alp_m0 = (rtrn - alp_m1);
    break;
    case 1:
        alp_m0 = (rtrn * dm_albm.mode_chng) / 20.0f;
        alp_m1 = rtrn - alp_m0;
    break;
    }
    
    if (dm_albm.side_chng != 0)
    {
        dm_albm.side_chng--;
    }
    
    alp_max = alp_m0;
    switch (dm_albm.side)
    {
    case 0:
        alp_s1 = (alp_max * dm_albm.side_chng) / 20.0f;
        alp_s0 = alp_max - alp_s1;
    break;
    case 1:
        alp_s0 = (alp_max * dm_albm.side_chng) / 20.0f;
        alp_s1 = alp_max - alp_s0;
    break;
    }
    
    switch (dm_albm.mode)
    {
    case 0:
        DspOgAlbumTop1(alp_max, alp_s0, alp_s1);
        switch (dm_albm.side)
        {
        case 0:
            DspPhotInTopPage(alp_s0);
        break;
        case 1:
            DspPhotInTopPage(alp_s1);
        break;
        }
        DspOgAlbumTop2(alp_max, alp_s0, alp_s1);
    break;
    case 1:
        DspOgAlbumBig(alp_m1);
    break;
    }
    
    switch (dm_albm.mode)
    {
    case 0: {
        u_char res = NmlPadInTopPage();
        if (res != 0 && dm_albm.rtrn_bak == 0)
        {
            dm_albm.rtrn_bak = res;
            dm_albm.io_mode = 1;
        }
    } break;
    case 1:
        AbnPadInBigPage(alp_m1);
    }
    
    if ((dm_albm.csr_mode == 1) || (dm_albm.csr_mode == 3))
    {
        if (dm_albm.menu_alp + 8 < rtrn)
        {
            dm_albm.menu_alp += 8;
        }
        else
        {
            dm_albm.menu_alp = rtrn;
        }
    }
    else if ((int)(dm_albm.menu_alp - 8) >= 1)
    {
        dm_albm.menu_alp -= 8;
    }
    else
    {
        dm_albm.menu_alp = 0;
    }
    
    if (dm_albm.menu_alp != 0)
    {
        int val_l = (dm_albm.csr[dm_albm.side] & 7);
        if (val_l >= 0)
        {
            if ( val_l < 4)
            {
                ComWin(1, dm_albm.csr[3], dm_albm.menu_alp);
            }
            else if (val_l < 8)
            {
                ComWin(0, dm_albm.csr[3], dm_albm.menu_alp);
            }
        }
    }
    
    if (InKeyTamusi() == 0)
    {
        if ((u_short)dm_albm.key_nashi_nori_take < 600)
        {
            dm_albm.key_nashi_nori_take++;
        } else {
            dm_albm.key_nashi_nori_take = 600;
        }
    }
    else
    {
        dm_albm.key_nashi_nori_take = 0;
    }
    
    if (
        ((u_short)dm_albm.key_nashi_nori_take == 600) &&
        (dm_albm.csr_mode == 0) &&
        (dm_albm.csr[dm_albm.side] < dm_albm.pht_max[dm_albm.side])
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
            }
        }
        if (dm_albm.dat_alp + 8 < rtrn)
        {
            dm_albm.dat_alp += 8;
        }
        else
        {
            dm_albm.dat_alp = rtrn;
        }
    }
    else if ((int)(dm_albm.dat_alp - 8) >= 1)
    {
        dm_albm.dat_alp = (u_char)(dm_albm.dat_alp - 8);
    }
    else
    {
        dm_albm.dat_alp = 0;
    }
    
    if ((dm_albm.io_mode != 0) && (dm_albm.all_alp == 0))
    {
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
        return dm_albm.rtrn_bak;
        
    }
    return 0;
}

void DspPhotInTopPage(u_char alp)
{
    u_int rgb;
    //float scl_x;
    short int px[8] = {57, 176, 57, 176, 354, 473, 354, 473};
    short int py[8] = {100, 133, 229, 262, 100, 133, 229, 262};
    int i;

    DrawSPhotoFromSmallPhotoArea(0, 4096, 0, 0, 0, 0);
    
    // scl_x = 1.0f;

    for (i = 0; i < 8; i++)
    {
        u_char pht_no;
        u_char adr_no;
        float in_f23;

        pht_no = ((u_char)(dm_albm.csr[dm_albm.side] / 8.0f) * 8) + i;
        adr_no = album_save_dat[dm_albm.side].pic[pht_no].adr_no;

        if (pht_no <= dm_albm.pht_max[dm_albm.side])
        {
            if (pht_no < dm_albm.pht_max[dm_albm.side])
            {
                if (dm_albm.side == 0)
                {
                    DrawSPhotoFromSmallPhotoArea2AD(dm_albm.src_addr, adr_no, 81920, 0, px[i] + 1, py[i] + 1, 110, 74, alp);
                }
                else
                {
                    DrawSPhotoFromSmallPhotoArea2AD(dm_albm.cpy_addr, adr_no, 81920, 0, px[i] + 1, py[i] + 1, 110, 74, alp);
                }

                PutSpriteYW(OA_TP_PHT_WAKU, OA_TP_PHT_WAKU, px[i], py[i], 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }
            
            if (dm_albm.paste_wait != 0)
            {
                u_char alp_l;
                
                alp_l = ((dm_albm.csr[dm_albm.side] & 7) != 0) ? 64 : 128;
                in_f23 = dm_albm.paste_wait / 15.0f;
                
                PolySquareYW(
                    px[dm_albm.csr[dm_albm.side] & 7],
                    py[dm_albm.csr[dm_albm.side] & 7],
                    110,
                    74,
                    0x808080,
                    alp_l * in_f23,
                    1.0f,
                    1.0f,
                    61440,
                    1,
                    0,
                    0
                );
            }
            
            if (
                (i == (dm_albm.csr[dm_albm.side] & 7)) &&
                (dm_albm.csr_mode != 2) &&
                (dm_albm.csr_mode != 3)
                )
            {
                u_char alp_l;
                
                if (dm_albm.buf_set != 0)
                {
                    float cos;
                    
                    cos= SgCosf((dm_albm.csr_tim * 6.2831855f) / 50.0f);
                    alp_l = alp * (cos + 1.0f) * 0.5f;
                    
                    if (dm_albm.csr_tim < 50.0f)
                    {
                        dm_albm.csr_tim++;
                    }
                    else
                    {
                        dm_albm.csr_tim = 0;
                    }
                    rgb = 0x800000;
                } else {
                    if (dm_albm.copy_wait != 0)
                    {
                        float sin;
                        
                        sin = SgSinf(((dm_albm.copy_wait % 0xf) * 6.2831855f) / 15.0f);
                        rgb = 0x808080;
                        in_f23 = ((sin + 1.0f) * 0.5f + 1.0f) * 0.5f;
                        alp_l = alp;
                    }
                    else
                    {
                        rgb = 0x606060;
                        dm_albm.csr_tim = 0;
                        alp_l = alp;
                    }
                }\

                PutSpriteYW(OA_TP_PHT_SELE1, OA_TP_PHT_SELE1, px[i] -  3, py[i] -  5, 0.0f, rgb, alp_l, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                PutSpriteYW(OA_TP_PHT_SELE2, OA_TP_PHT_SELE2, px[i] + 85, py[i] -  4, 0.0f, rgb, alp_l, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                PutSpriteYW(OA_TP_PHT_SELE3, OA_TP_PHT_SELE3, px[i] -  4, py[i] + 57, 0.0f, rgb, alp_l, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                PutSpriteYW(OA_TP_PHT_SELE4, OA_TP_PHT_SELE4, px[i] + 84, py[i] + 56, 0.0f, rgb, alp_l, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                
                if (dm_albm.copy_wait != 0)
                {
                    PutSpriteYW(OA_TP_PHT_SELE1, OA_TP_PHT_SELE1, px[i] -  3, py[i] -  5, 0.0f, rgb, alp_l * in_f23, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
                    PutSpriteYW(OA_TP_PHT_SELE2, OA_TP_PHT_SELE2, px[i] + 85, py[i] -  4, 0.0f, rgb, alp_l * in_f23, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
                    PutSpriteYW(OA_TP_PHT_SELE3, OA_TP_PHT_SELE3, px[i] -  4, py[i] + 57, 0.0f, rgb, alp_l * in_f23, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
                    PutSpriteYW(OA_TP_PHT_SELE4, OA_TP_PHT_SELE4, px[i] + 84, py[i] + 56, 0.0f, rgb, alp_l * in_f23, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
                }
            }
        }   
    }

    DrawSPhotoFromSmallPhotoArea(0, 4096, 0, 0, 0, 0);
}

char NmlPadInTopPage()
{    
    char rtrn;
    char rtrn2;
    u_int csr_movable;
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
        // TODO: check (it matches but feels off ...)
        u_char csr_l = dm_albm.pht_max[dm_albm.side];
        if (csr_l == 0)
        {
            csr_movable = 0;
        }
        else
        {
            // (... especially here)
            csr_movable = (csr_l - 1) & 0xff;
        }
    }
    else
    {
        csr_movable = dm_albm.pht_max[dm_albm.side];
    }
    
    if (dm_albm.exit_realy == 1)
    {
        if (*key_now[4] == 1)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);
            dm_albm.exit_realy = 0;
        }
        else if ((*key_now[6] == 1) || (*key_now[5] == 1))
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);
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
                    rtrn2 = 1;
                    mc_photo_num = dm_albm.pht_max[0];
                break;
                case 1:
                    dm_albm.csr_mode = 0;
                    dm_albm.exit_realy = 0;
                    save_pht_in_game = 0;
                    mc_album_save = album_save_dat[1];
                    rtrn2 = 2;
                    mc_photo_num = dm_albm.pht_max[1];
                break;
                }
            break;
                
            case 1:
                rtrn2 = 5;
                dm_albm.csr_mode = 0;
                dm_albm.exit_realy = 0;
            break;
                
            case 2:
                dm_albm.exit_realy = 0;
            break;
            }
        }
        else if (*key_now[3] == 1 || Ana2PadDirCnt(1) == 1)
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);
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
        }
        else if ((*key_now[2] == 1) || (Ana2PadDirCnt(3) == 1))
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);
            if (dm_albm.side == 0 && dm_albm.is_title == 0)
            {
                if (dm_albm.csr[4] == 0)
                {
                    dm_albm.csr[4] = 1;
                }
                
                if (dm_albm.csr[4] < 2)
                {
                    dm_albm.csr[4] = 2;
                }
                else
                {
                    dm_albm.csr[4]--;
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
        }
    }
    else
    {
        if (dm_albm.pst_mode == 1)
        {
            if (PasteCtrl() != 0)
            {
                dm_albm.pst_mode = 0;
            }
        }
        else if (dm_albm.csr_mode == 1)
        {
            rtrn = 0;
            if (dm_albm.menu_alp == dm_albm.all_alp)
            {
                rtrn = MenuPadCmn();
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
        else if (dm_albm.io_mode == 0 && dm_albm.side_chng_wait == 0)
        {
            if (dm_albm.copy_wait == 0 && dm_albm.paste_wait == 0)
            {
                if (*key_now[4] == 1)
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
                        dm_albm.csr[4] = 2;
                    }
                }
                else if (*key_now[5] == 1)
                {
                    if (dm_albm.buf_set == 0)
                    {
                        switch (dm_albm.side)
                        {
                            case 0:
                            if (dm_albm.csr[0] < dm_albm.pht_max[0])
                            {
                            LAB_0010515c:
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
                else if (*key_now[6] == 1)
                {
                    if (dm_albm.buf_set != 0)
                    {
                        SeStartFix(2, 0, 0x1000, 0x1000, 0);
                    }
                    else
                    {
                        SeStartFix(1, 0, 0x1000, 0x1000, 0);
                        dm_albm.csr_mode = 1;
                        dm_albm.csr[3] = 0;
                    }
                }
                else if (*key_now[7] == 1)
                {
                    if (dm_albm.buf_set == 0)
                    {
                        if ((dm_albm.csr[dm_albm.side] < dm_albm.pht_max[dm_albm.side]) && SamePhotoChk() == 0)
                        {
                            dm_albm.copy_wait = 15;
                            KillRingBuffer();
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
                else if (
                    *key_now[3] == 1 ||
                    (*key_now[3] > 0x19 && (*key_now[3] % 5 == 1) ||
                    Ana2PadDirCnt(1) == 1) ||
                    (Ana2PadDirCnt(1) > 0x19 && (Ana2PadDirCnt(1) % 5 == 1))
                )
                {
                    if ((dm_albm.csr[dm_albm.side] + csr_ax[dm_albm.csr[dm_albm.side] % 8].n) < (int)csr_movable)
                    {
                        SeStartFix(0, 0, 0x1000, 0x1000, 0);
                        dm_albm.csr[dm_albm.side] += csr_ax[dm_albm.csr[dm_albm.side] % 8].n;
                    }
                    else 
                    {
                        if (dm_albm.side == 0 && dm_albm.is_title == 0)
                        {
                            if (csr_movable < 0x17)
                            {
                                if (dm_albm.csr[dm_albm.side] != csr_movable)
                                {
                                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                                    dm_albm.csr[dm_albm.side] = csr_movable;
                                }
                            }
                            else if (dm_albm.csr[dm_albm.side] != 0x17)
                            {
                                SeStartFix(0, 0, 0x1000, 0x1000, 0);
                                dm_albm.csr[dm_albm.side] = 23;
                            }
                        }
                        else if (csr_movable < 0x18)
                        {
                            if (dm_albm.csr[dm_albm.side] != csr_movable)
                            {
                                SeStartFix(0, 0, 0x1000, 0x1000, 0);
                                dm_albm.csr[dm_albm.side] = csr_movable;
                            }
                        }
                    }
                }
                else if (
                    *key_now[2] == 1 ||
                    (*key_now[2] > 0x19 && (*key_now[2] % 5 == 1)) ||
                    Ana2PadDirCnt(3) == 1 ||
                    (Ana2PadDirCnt(3) > 0x19 && (Ana2PadDirCnt(3) % 5 == 1))
                )
                {
                    if ((dm_albm.csr[dm_albm.side] + csr_ax[dm_albm.csr[dm_albm.side] % 8].p) >= 0)
                    {
                        SeStartFix(0, 0, 0x1000, 0x1000, 0);
                        dm_albm.csr[dm_albm.side] += csr_ax[dm_albm.csr[dm_albm.side] % 8].p;
                    }
                }
                else if (
                        *key_now[1] == 1 ||
                        (*key_now[1] > 0x19 && *key_now[1] % 5 == 1) ||
                        Ana2PadDirCnt(2) == 1 ||
                        (Ana2PadDirCnt(2) > 0x19 && (Ana2PadDirCnt(2) % 5 == 1))
                    )
                {
                    if (
                        (csr_ay[dm_albm.csr[dm_albm.side] % 8].n == 0x52) ||
                        ((int)csr_movable < dm_albm.csr[dm_albm.side] + csr_ay[dm_albm.csr[dm_albm.side] % 8].n)
                       )
                    {
                        if ((dm_albm.buf_set == 0) && (dm_albm.mem != 0))
                        {
                            SeStartFix(0, 0, 0x1000, 0x1000, 0);
                            dm_albm.csr_mode = 2;
                            dm_albm.csr[2] = dm_albm.mem - 1;
                        }
                    }
                    else
                    {
                        SeStartFix(0, 0, 0x1000, 0x1000, 0);
                        dm_albm.csr[dm_albm.side] += csr_ay[dm_albm.csr[dm_albm.side] % 8].n;
                    }
                }
                else if (
                    *key_now[0] == 1 ||
                    (*key_now[0] > 0x19 && (*key_now[0] % 5 == 1)) ||
                    Ana2PadDirCnt(0) == 1 ||
                    (Ana2PadDirCnt(0) > 0x19 && (Ana2PadDirCnt(0) % 5 == 1))
                )
                {
                    if ((csr_ay[dm_albm.csr[dm_albm.side] % 8].p != 99) &&
                        (dm_albm.csr[dm_albm.side] + csr_ay[dm_albm.csr[dm_albm.side] % 8].p >= 0))
                        {
                            SeStartFix(0, 0, 0x1000, 0x1000, 0);
                            dm_albm.csr[dm_albm.side] += csr_ay[dm_albm.csr[dm_albm.side] % 8].p;
                        }
                }
                else if (*key_now[10] == 1)
                {
                    if (dm_albm.side == 0)
                    {
                        dm_albm.side_chng_mode = 1;
                        dm_albm.side_chng_wait = 15;
                    }
                }
                else if (*key_now[8] == 1 && dm_albm.side == 1)
                {
                    dm_albm.side_chng_mode = 0;
                    dm_albm.side_chng_wait = 15;
                }
            }
        }
    }
    
    if (dm_albm.side_chng_wait != 0)
    {
        dm_albm.side_chng_wait--;
        
        if (dm_albm.side_chng_wait == 0)
        {
            SeStartFix(4, 0, 0x1000, 0x1000, 0);
            dm_albm.side_chng = 20;
            dm_albm.side = dm_albm.side_chng_mode;
            dm_albm.inf_alp = 0;
        }
    }

    if (dm_albm.copy_wait != 0)
    {
        dm_albm.copy_wait = dm_albm.copy_wait - 1;
        if (dm_albm.copy_wait == 0)
        {
            SeStartFix(11, 0, 0x1000, 0x1000, 0);
        }
    }

    if (dm_albm.exit_realy == 1 || dm_albm.yes_no == 1)
    {
        PolySquareYW(34.0f, 315.0f, 0x241, 0x6b,  0, 48.0f, 1.0f, 1.0f, 0x4000, 0, 0, 0);
        DrawMessageBox(0x3000, 22.0f, 308.0f, 596.0f, 118.0f, 0x80);

#ifdef BUILD_EU_VERSION
        if (sys_wrk.language == LANG_FRENCH && dm_albm.csr[4] == 0)
        {
            YesNoCrslOKR(0x2000, 98.0f, 382.0f, 0x808080, 128.0f, 2.6f);
        }
        else
        {
            YesNoCrslOKR(0x2000, (dm_albm.csr[4] * 0x84 + 0x6a), 382.0f, 0x808080, 128.0f, 2.2f);
        }
#else
        YesNoCrslOKR(0x2000, (dm_albm.csr[4] * 0x84 + 0x72), 380.0f, 0x808080, 128.0f, 2.0f);
#endif

        if (dm_albm.side == 0 && dm_albm.is_title == 0)
        {
            PutStringYW(6, 0x2e, 0x44, 0x181, 0x808080, 0x80, 0x1000, 0);
        }
        else
        {
            PutStringYW(6, 0x28, 0x3b, 0x181, 0x808080, 0x80, 0x1000, 0);
        }
        PutStringYW(6, 0x20, 0x140, 0x143, 0x808080, 0x80, 0x1000, 1);
        PutStringYW(6, 0x25, 0x3c, 0x161, 0x808080, 0x80, 0x1000, 0);
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
        dm_albm.buf[dm_albm.mem] = dm_albm.csr[dm_albm.side];
        dm_albm.buf_sid[dm_albm.mem] = dm_albm.side;
        dm_albm.mem++;
        dm_albm.buf[dm_albm.mem] = 0xff;
        dm_albm.buf_sid[dm_albm.mem] = 0xff;
        return;
    }

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

char AbnPadInBigPage(u_char alp)
{
    u_char target;

    if (*key_now[5] == 1)
    {
        SeStartFix(1, 0, 0x1000, 0x1000, 0);
        dm_albm.mode_chng = 20;
        dm_albm.mode = 0;
    }
    else
    {
        if (*key_now[4] == 1)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);
            dm_albm.mode_chng = 20;
            dm_albm.mode = 0;
        }
        else if (
            *key_now[2] == 1 ||
            (*key_now[2] > 0x19 && (*key_now[2] % 5) == 1) ||
            Ana2PadDirCnt(3) == 1 ||
            (Ana2PadDirCnt(3) > 0x19 && (Ana2PadDirCnt(3) % 5 == 1))
        )
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
                } else {
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
        else if (
            *key_now[3] == 1 ||
            (*key_now[3] > 0x19 && (*key_now[3] % 5) == 1) ||
            Ana2PadDirCnt(1) == 1 ||
            (Ana2PadDirCnt(1) > 0x19 && (Ana2PadDirCnt(1) % 5 == 1))
        )
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
                if (1 < dm_albm.pht_max[dm_albm.side])
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

    PolySquareYW(128.0f, 73.0f, 0x17f, 0xff, 0, alp, 1.0f, 1.0f, 0x1f000, 0, 0, 0);

    if (CheckPhotoExpandEnd())
    {
        DrawSPhotoFromSmallPhotoArea(0, 0x1000, 0, 0, 0, 0);
        DrawPhotoFromPhotoWrk(target, 0x14000, 0, 0x80, 0x49, 0x17f, 0xff, alp);
    }

    PutSpriteYW(ALBM_WAKU_BIG_U, ALBM_WAKU_BIG_U, 0.0f, -5.0f, 0.0f, 0x808080, alp, 19.299999f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(ALBM_WAKU_BIG_D, ALBM_WAKU_BIG_D, 0.0f, -5.0f, 0.0f, 0x808080, alp, 19.299999f, 1.0f, 0, 0xff, 1, 0, 0);
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

    // using an 'if' only for 'case 0' fixes the switch
    // jump logic later on int the asm ... but why??
    if (dm_albm.yes_no == 0)
    {
        if (*key_now[4] == 1)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);
            dm_albm.csr_mode = 0;
        }
        else if ((*key_now[5]) == 1)
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
                    if (
                        dm_albm.csr[dm_albm.side] < dm_albm.pht_max[dm_albm.side] &&
                        SamePhotoChk() == 0
                    )
                    {
                        dm_albm.copy_wait = 15;
                        KillRingBuffer();
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
                    if (dm_albm.is_title != 0)
                    {
                        SeStartFix(1, 0, 0x1000, 0x1000, 0);
                        save_pht_in_game = 0;
                        mc_album_save = album_save_dat[0];
                        mc_photo_num = dm_albm.pht_max[0];
                        rtrn = 1;
                    }
                    else
                    {
                        SeStartFix(2, 0, 0x1000, 0x1000, 0);
                    }
                break;
                case 1: {
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);
                    save_pht_in_game = 0;
                    mc_album_save = album_save_dat[1];
                    mc_photo_num = dm_albm.pht_max[1];
                    rtrn = 2;
                }
                break;
                }

                break;

            case 4: {
                // needs a local var for case 1
                u_char side = dm_albm.side;
                switch (side)
                {
                case 0:
                    if (dm_albm.is_title != 0)
                    {
                        SeStartFix(1, 0, 0x1000, 0x1000, 0);
                        dm_albm.yes_no = 3;
                        dm_albm.csr[4] = 1;
                    }
                    else
                    {
                        SeStartFix(2, 0, 0x1000, 0x1000, 0);
                    }
                    break;

                case 1: {
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);
                    dm_albm.yes_no = 3;
                    dm_albm.csr[4] = side;
                }
                break;
                }
            }
            break;

            case 5:
                SeStartFix(1, 0, 0x1000, 0x1000, 0);
                dm_albm.yes_no = 1;
                dm_albm.csr[4] = 2;
                break;
            }
        }
        else if (
            *key_now[1] == 1 ||
            (*key_now[1] >= 0x1a && (*key_now[1] % 5) == 1) ||
            Ana2PadDirCnt(2) == 1 ||
            (Ana2PadDirCnt(2) >= 0x1a && (Ana2PadDirCnt(2) % 5) == 1)
        )
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);
            if ((dm_albm.side == 0) && (dm_albm.is_title == 0))
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
        else if (
            *key_now[0] == 1 ||
            (*key_now[0] >= 0x1a && (*key_now[0] % 5) == 1) ||
            Ana2PadDirCnt(0) == 1 ||
            (Ana2PadDirCnt(0) >= 0x1a && (Ana2PadDirCnt(0) % 5) == 1)
        )
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);
            if ((dm_albm.side == 0) && (dm_albm.is_title == 0))
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
        case 1: {
            if (*key_now[4] == 1)
            {
                SeStartFix(3, 0, 0x1000, 0x1000, 0);
                dm_albm.yes_no = 0;
            }
            else if (*key_now[6] == 1 || *key_now[5] == 1)
            {
                SeStartFix(1, 0, 0x1000, 0x1000, 0);
                switch (dm_albm.csr[4])
                {
                case 0:
                    switch (dm_albm.side)
                    {
                    case 0: {
                        dm_albm.csr_mode = 0;
                        save_pht_in_game = 0;
                        dm_albm.yes_no = 0;
                        mc_album_save = album_save_dat[0];
                        mc_photo_num = dm_albm.pht_max[0];
                        rtrn = 1;
                    }
                    break;

                    case 1: {
                        dm_albm.csr_mode = 0;
                        dm_albm.yes_no = 0;
                        save_pht_in_game = 0;
                        mc_album_save = album_save_dat[1];
                        mc_photo_num = dm_albm.pht_max[1];
                        rtrn = 2;
                    }
                    break;
                    }

                    break;

                case 1: {
                    dm_albm.csr_mode = 0;
                    dm_albm.yes_no = 0;
                    rtrn = 5;
                }
                break;

                case 2: {
                    dm_albm.yes_no = 0;
                }
                break;
                }
            }
            else if (*key_now[3] == 1 || Ana2PadDirCnt(1) == 1)
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);
                if (dm_albm.side == 0 && dm_albm.is_title == 0)
                {
                    if (dm_albm.csr[4] == 0)
                    {
                        dm_albm.csr[4] = 2;
                    }
                    if (dm_albm.csr[4] <= 1)
                    {
                        dm_albm.csr[4]++;
                    }
                    else
                    {
                        dm_albm.csr[4] = 1;
                    }
                }
                else if (dm_albm.csr[4] <= 1)
                {
                    dm_albm.csr[4]++;
                }
                else
                {
                    dm_albm.csr[4] = 0;
                }
            }
            else if (*key_now[2] == 1 || Ana2PadDirCnt(3) == 1)
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);
                if (dm_albm.side == 0 && dm_albm.is_title == 0)
                {
                    if (dm_albm.csr[4] == 0)
                    {
                        dm_albm.csr[4] = 1;
                    }
                    if (dm_albm.csr[4] > 1)
                    {
                        dm_albm.csr[4]--;
                    }
                    else
                    {
                        dm_albm.csr[4] = 2;
                    }
                }
                else if (dm_albm.csr[4] >= 1)
                {
                    dm_albm.csr[4]--;
                }
                else
                {
                    dm_albm.csr[4] = 2;
                }
            }
        }
        break;

        case 2: {
            int one = 1;
            DSP_MEM_ALBM *dm_albm_p = &dm_albm;
            
            YesNoCrslOKR(0x2000, (dm_albm.csr[4] * 0x84) + 0x13e, 387.0f, 0x808080, 128.0f, 2.0f);
            PutStringYW(6, 0x22, 0x3c, 0x186, 0x808080, 0x80, 0x1000, 0);
            PutStringYW(9, 1, 0x166, 0x186, 0x808080, 0x80, 0x1000, 1);
            
            if (*key_now[4] == 1)
            {
                SeStartFix(3, 0, 0x1000, 0x1000, 0);
                dm_albm.yes_no = 0;
            }
            else
            {
                if (*key_now[6] == 1 || *key_now[5] == 1)
                {
                    if (dm_albm.csr[4] != 0)
                    {
                        SeStartFix(1, 0, 0x1000, 0x1000, 0);
                        dm_albm.yes_no = 0;
                    }
                    else
                    {
                        DelPyon(dm_albm.side, dm_albm.csr[dm_albm.side]);
                        dm_albm.yes_no = 0;
                    }
                }
                // this uses the asm generated in case 3, so it must be exactly the same code
                else if (
                    *key_now[3] == 1 || Ana2PadDirCnt(1) == 1 ||
                    *key_now[2] == 1 || Ana2PadDirCnt(3) == 1
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                    dm_albm_p->csr[4] = one - dm_albm_p->csr[4];
                }
            }
        }
        break;

        case 3: {
            // used in the last else if
            int one = 1;
            DSP_MEM_ALBM *dm_albm_p = &dm_albm;
            
            PolySquareYW(34.0f, 315.0f, 0x241, 0x6b, 0, 48.0f, 1.0f, 1.0f, 0x4000, 0, 0, 0);
            DrawMessageBox(0x3000, 22.0f, 308.0f, 596.0f, 118.0f, 0x80);
            YesNoCrslOKR(0x2000, (dm_albm.csr[4] * 0x84) + 0xb8, 382.0f, 0x808080, 128.0f, 2.0f);
            PutStringYW(9, 1, 0xe1, 0x181, 0x808080, 0x80, 0x1000, 1);
            PutStringYW(6, 0x27, 0x140, 0x143, 0x808080, 0x80, 0x1000, 1);
            PutStringYW(6, 0x25, 0x3c, 0x161, 0x808080, 0x80, 0x1000, 0);
            
            if (*key_now[4] == 1)
            {
                SeStartFix(3, 0, 0x1000, 0x1000, 0);
                dm_albm.yes_no = 0;
            }
            else
            {
                if (*key_now[6] == 1 || *key_now[5] == 1)
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
                else if (
                    *key_now[3] == 1 || Ana2PadDirCnt(1) == 1 ||
                    *key_now[2] == 1 || Ana2PadDirCnt(3) == 1
                )
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                    // needs the two local vars, 'one' and 'dm_albm_p', to match ... why??
                    // also, what's the meaning of this? csr is a u_char so doing a subtraction
                    // does not make any sense! is it a compiler thing to leverage
                    // the overflow to loop back to 0xff?
                    dm_albm_p->csr[4] = one - dm_albm_p->csr[4];
                }
            }
        }
        break;
        }
    }
    return rtrn;
}

char PasteCtrl()
{
    char rtrn;

    rtrn = 0; // Line 1441

    if (dm_albm.pst_yes_no != 0)
    {
        int one = 1;
        DSP_MEM_ALBM *dm_albm_p = &dm_albm;
        
        YesNoCrslOKR(0x2000, (dm_albm.csr[4] * 0x84 + 0x13e), 387.0f, 0x808080, 128.0f, 2.0f); // Line 1443
        PutStringYW(9, 1, 0x166, 0x186, 0x808080, 0x80, 0x1000, 1); // Line 1444
        PutStringYW(6, 0x26, 0x3c, 0x186, 0x808080, 0x80, 0x1000, 0); // Line 1447
        if (*key_now[4] == 1) // Line 1450
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0); // Line 1455
            dm_albm.pst_yes_no = 0;
            rtrn = 1;
        }
        else if (*key_now[6] == 1 || *key_now[5] == 1) // Line 1456
        {
            if (dm_albm.csr[4] == 0)
            {
                dm_albm.paste_wait = 15; // Line 1458
                Paste(); // Line 1459
                dm_albm.pst_yes_no = 0; // Line 1461
            }
            else
            {            
                SeStartFix(1, 0, 0x1000, 0x1000, 0); // Line 1462
                dm_albm.pst_yes_no = 0;
                rtrn = 1; // Line 1467
            }
        }
        else if (( // Line 1470
            *key_now[3] == one || Ana2PadDirCnt(1) == one ||
            *key_now[2] == one || Ana2PadDirCnt(3) == one
        ))
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0); // Line 1471
            // dm_albm.csr[4] = one - dm_albm.csr[4];
            dm_albm_p->csr[4] = one - dm_albm_p->csr[4]; // Line 1472 - 1477
        }
    }
    else if (dm_albm.paste_wait != 0) // Line 1478
    {
        if (--dm_albm.paste_wait == 0) // Line 1478 - 1479
        {
            SeStartFix(0xb, 0, 0x1000, 0x1000, 0); // Line 1485
            rtrn = 1;
        }
    }
    else  if (dm_albm.side == 0 && dm_albm.is_title == 0) // Line 1487
    {
        SeStartFix(2, 0, 0x1000, 0x1000, 0); // Line 1488
        rtrn = 1; // Line 1490
    }
    else if (dm_albm.mem != 0) // Line 1491
    {
        if (dm_albm.csr[dm_albm.side] < dm_albm.pht_max[dm_albm.side]) // Line 1493
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0); // Line 1494
            dm_albm.csr[4] = 1; // Line 1495 -
            dm_albm.pst_yes_no = 1; // - 1499
        }
        else
        {
            dm_albm.paste_wait = 15; // Line 1501
            Paste(); // Line 1502
        }
    }
    else
    {
        SeStartFix(2, 0, 0x1000, 0x1000, 0); // Line 1505
        rtrn = 1;
    }

    return rtrn;
}

void Paste()
{
    int i;

    CopyToOsugi(
        dm_albm.buf_sid[dm_albm.csr[2]],
        dm_albm.buf[dm_albm.csr[2]],
        dm_albm.side,
        dm_albm.csr[dm_albm.side]
    );
    
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
    
    if (
        (dm_albm.mem - 1) < dm_albm.csr[2] &&
        dm_albm.csr[2] != 0
    )
    {
        dm_albm.csr[2]--;
    }
    
    if (
        (dm_albm.pht_max[dm_albm.side] - 1) < dm_albm.csr[dm_albm.side] &&
        dm_albm.pht_max[dm_albm.side] < 0x18
    )
    {
        dm_albm.pht_max[dm_albm.side]++;
    }
}

void BuffPadCmn()
{
    if (dm_albm.mode == 0)
    {
        if (*key_now[6] == 1)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);
            dm_albm.csr[3] = 0;
            dm_albm.csr_mode = 3;
        }
        else if (*key_now[5] == 1)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);
            dm_albm.buf_exp_flg = 1;
            dm_albm.mode_chng = 20;
            dm_albm.mode = 1;
        }
        else if (*key_now[7] == 1)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);
            dm_albm.buf_set = 1;
            dm_albm.csr_mode = 0;
            dm_albm.csr[dm_albm.side] = GetAlbumCurPos(dm_albm.side);
        }
        else if (
            *key_now[0] == 1 || 
            (*key_now[0] > 0x19 && (*key_now[0] % 5) == 1) ||
            Ana2PadDirCnt(0) == 1 ||
            (Ana2PadDirCnt(0) > 0x19 && (Ana2PadDirCnt(0) % 5) == 1)
        )
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);
            dm_albm.csr_mode = 0;
        }
        else if (
            (*key_now[2] == 1) ||
            (*key_now[2] > 0x19 && (*key_now[2] % 5) == 1) ||
            Ana2PadDirCnt(3)  == 1 ||
            (Ana2PadDirCnt(3) > 0x19 && (Ana2PadDirCnt(3) % 5) == 1)
        )
        {
            if (dm_albm.csr[2] < (dm_albm.mem - 1))
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);
                dm_albm.csr[2] = dm_albm.csr[2] + 1;
            }
            else if (dm_albm.mem >= 2)
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);
                dm_albm.csr[2] = 0;
            }
            else
            {
                dm_albm.csr[2] = 0;
            }
        }
        else if (
            (*key_now[3] == 1) ||
            (*key_now[3] > 0x19 && (*key_now[3] % 5) == 1) ||
            Ana2PadDirCnt(1)  == 1 ||
            (Ana2PadDirCnt(1) > 0x19 && (Ana2PadDirCnt(1) % 5) == 1)
        )
        {
            if (dm_albm.csr[2] != 0)
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);
                dm_albm.csr[2]--;
            }
            else
            {
                if (dm_albm.mem > 1)
                {
                    SeStartFix(0, 0, 0x1000, 0x1000, 0);
                }
                dm_albm.csr[2] = dm_albm.mem - 1;
            }
        }
        else
        {
            if (*key_now[10] == 1)
            {
                if (dm_albm.side == 0)
                {
                    dm_albm.side_chng_mode = 1;
                    dm_albm.side_chng_wait = 15;
                }
            }
            else
            {
                if (*key_now[8] == 1 && dm_albm.side == 1)
                {
                    dm_albm.side_chng_mode = 0;
                    dm_albm.side_chng_wait = 15;
                }
            }
        }
    }
}

void BuffPadMnu()
{
    int i;

    if (*key_now[4] == 1)
    {
        SeStartFix(3, 0, 0x1000, 0x1000, 0);
        dm_albm.csr_mode = 2;
    }
    else if (*key_now[5] == 1)
    {
        switch(dm_albm.csr[3])
        {
            case 0:
                SeStartFix(1, 0, 0x1000, 0x1000, 0);
                dm_albm.buf_exp_flg = 1;
                dm_albm.mode = 1;
                dm_albm.csr_mode = 2;
                dm_albm.mode_chng = 20;
            break;
            case 1:
                SeStartFix(1, 0, 0x1000, 0x1000, 0);
                dm_albm.buf_set = 1;
                dm_albm.csr_mode = 0;
                dm_albm.csr[dm_albm.side] = GetAlbumCurPos(dm_albm.side);
            break;
            case 2:
                SeStartFix(1, 0, 0x1000, 0x1000, 0);
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
                if (
                    dm_albm.csr[2] > (dm_albm.mem - 1) &&
                    dm_albm.csr[2] != 0
                )
                {
                    dm_albm.csr[2]--;
                }
        break;
        }
    }
    else if (
            (*key_now[1] == 1) ||
            (*key_now[1] > 0x19 && (*key_now[1] % 5) == 1) ||
            Ana2PadDirCnt(2)  == 1 ||
            (Ana2PadDirCnt(2) > 0x19 && (Ana2PadDirCnt(2) % 5) == 1)
        )
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
    else if (
            (*key_now[0] == 1) ||
            (*key_now[0] > 0x19 && (*key_now[0] % 5) == 1) ||
            Ana2PadDirCnt(0)  == 1 ||
            (Ana2PadDirCnt(0) > 0x19 && (Ana2PadDirCnt(0) % 5) == 1)
        )
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
        return;
    }
}

void DspOgAlbumTop1(u_char alp, u_char alp0, u_char alp1)
{
    short int px[8] = {
        0x39, 0xB0, 0x162, 0x1D9,
        0x39, 0xB0, 0x162, 0x1D9,
    };
    short int py[8] = {
        0x64, 0x85, 0x64, 0x85,
        0xE5, 0x106, 0xE5, 0x106,
    };

    switch (dm_albm.is_title)
    {
    case 0:
        switch (dm_albm.side)
        {
        case 0:
#ifdef BUILD_EU_VERSION
            SetSprFile(0x1d83000);
#else
            SetSprFile(0x1d88100);
#endif
        break;
        case 1:
#ifdef BUILD_EU_VERSION
            SetSprFile(0x1d93470);
#else
            SetSprFile(0x1d98570);
#endif
        break;
        }
    break;
    case 1:
        switch (dm_albm.side)
        {
        case 0:
#ifdef BUILD_EU_VERSION
            SetSprFile(0x1d83000);
#else
            SetSprFile(0x1d88100);
#endif
        break;
        case 1:
#ifdef BUILD_EU_VERSION
            SetSprFile(0x1dc3470);
#else
            SetSprFile(0x1dc8570);
#endif
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
                PutSpriteYW(
                    dm_albm.cl_type[0] + TP_BG_SML_WS,
                    dm_albm.cl_type[0] + TP_BG_SML_WS,
                    193.0f, 154.0f,
                    0.0f, 0x808080, alp0,
                    2.3307087f, 2.3410852f,
                    0, 0xff, 1, 0, 0);
            break;
            case 1:
                PutSpriteYW(
                    dm_albm.cl_type[0] + TP_BG_BIG_WS,
                    dm_albm.cl_type[0] + TP_BG_BIG_WS,
                    64.0f, 90.0f,
                    0.0f, 0x808080, alp0,
                    1.1607844f, 1.1615385f,
                    0, 0xff, 1, 0, 0);
            break;
        }
    break;
    case 1:
        switch (dm_albm.is_title)
        {
        case 0:
            PutSpriteYW(
                dm_albm.cl_type[1] + TP_BG_SML_WS,
                dm_albm.cl_type[1] + TP_BG_SML_WS,
                193.0f, 154.0f,
                0.0f, 0x808080, alp1,
                2.3307087f, 2.3410852f,
                0, 0xff, 1, 0, 0);
        break;
        case 1:
            PutSpriteYW(
                dm_albm.cl_type[1] + TP_BG_BIG_WS,
                dm_albm.cl_type[1] + TP_BG_BIG_WS,
                64.0f, 90.0f,
                0.0f, 0x808080, alp1,
                1.1607844f, 1.1615385f,
                0, 0xff, 1, 0, 0);
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
        }
    } else {
        if (dm_albm.menu_alp != 0)
        {
            Navion(1, 0, alp);
        } else {
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
#ifdef BUILD_EU_VERSION
                SetSprFile(0x1d83000);
#else
                SetSprFile(0x1d88100);
#endif
            break;
            case 1:
#ifdef BUILD_EU_VERSION
                SetSprFile(0x1d93470);
#else
                SetSprFile(0x1d98570);
#endif
            break;
            }
        break;
        case 1:
            switch (side)
            {
            case 0:
#ifdef BUILD_EU_VERSION
                SetSprFile(0x1d83000);
#else
                SetSprFile(0x1d88100);
#endif
            break;
            case 1:
#ifdef BUILD_EU_VERSION
                SetSprFile(0x1dc3470);
#else
                SetSprFile(0x1dc8570);
#endif
            break;
            }
        break;
    }
    
    switch (dm_albm.is_title)
    {
    case 0:
        PutSpriteYW(
            dm_albm.cl_type[side] + KK_BG_SML_WX,
            dm_albm.cl_type[side] + KK_BG_SML_WX,
            193.0, 139.0,
            0.0, 0x808080, alp,
            2.3307087f, 2.3499999f,
            0, 0xff, 1, 0, 0);
    break;
    case 1:
        PutSpriteYW(
            dm_albm.cl_type[side] + KK_BG_BIG_WX,
            dm_albm.cl_type[side] + KK_BG_BIG_WX,
            64.0, 79.0,
            0.0, 0x808080, alp,
            1.1607844f, 1.1652893f,
            0, 0xff, 1, 0, 0);
    break;
    }
    
    InfoWin(1, side, alp);
    DtlWin(1, 0, alp);
    Navion(1, 1, alp);
    DispCaption(0x14, alp);
}

void InfoWin(u_char mode, u_char side, u_char alp)
{
    u_char alp0;
    u_char alp1;
    short int adj1;
    short int adj2;
    float rad;
    float flsh1;
    
    if (dm_albm.inf_alp + 8 < alp)
    {
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
    
    if (dm_albm.flsh_tmr <= 0x3a)
    {
        dm_albm.flsh_tmr++;
    }
    else
    {
        dm_albm.flsh_tmr = 0;
    }
    
    rad = (dm_albm.flsh_tmr % 0x3c) * 6.2831855f / 60.0f;
    if (dm_albm.side_chng_wait == 0)
    {
        flsh1 = ((SgSinf(rad) + 1.0f) * 0.5f + 1.0f) * 0.6f * 0.5f + 0.3f;
    }
    else
    {
        flsh1 = 1.0f;
    }
    
    if (mode == 0)
    {
        adj1 = 0;
        adj2 = 0;
    }
    else
    {
        adj1 = -0x22;
        adj2 = 0x1a;
    }
    
    if (alp1 != 0)
    {
        int pos_x;
        
        DrawMessageBox(0x32000, adj1 + 0x3c, 17.0f, 582.0f, 48.0f, alp1);
        
        if (mode != 1)
        {
            if (dm_albm.side_chng_wait != 0)
            {
                PutSpriteYW(ALBM_CAPT_L1, ALBM_CAPT_L1, -52.0f, -6.0f, 0.0f, 0x808080, alp1 * flsh1, 1.0f, 1.0f, 0, 0x30000, 1, 0x1, 0);
            }
            else
            {
                PutSpriteYW(ALBM_CAPT_L1, ALBM_CAPT_L1, -52.0f, -6.0f, 0.0f, 0x808080, alp1 * flsh1, 1.0f, 1.0f, 0, 0x30000, 1, 0, 0);
            }
        }

        pos_x = adj1 + 0x66;
        
        PutSpriteYW(OA_TP_FNT_SLSH, OA_TP_FNT_SLSH, pos_x, 2.0f, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0x30000, 1, 0, 0);
        PutNumberYW(0xb, dm_albm.pht_max[1], adj1 + 0x20a, 0x1d, 1.0f, 1.0f, 0x808080, alp1, 0x30000, 2, 0);
        PutNumberYW(0xb, 0x18, adj1 + 0x234, 0x1d, 1.0f, 1.0f, 0x808080, alp1, 0x30000, 2, 0);
        
        if (dm_albm.csr_mode == 2)
        {
            if (mode != 0)
            {
                PutSpriteYW(OA_TP_FNT_SLSH, OA_TP_FNT_SLSH, adj1 + 0x3c, 2.0f, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0x30000, 1, 0, 0);
                PutNumberYW(0xb, dm_albm.buf[dm_albm.csr[2]] + 1, adj1 + 0x1e0, 0x1d, 1.0f, 1.0f, 0x808080, alp1, 0x30000, 2, 0);
            }
        }
        else if (dm_albm.csr[1] < dm_albm.pht_max[1])
        {
            PutSpriteYW(OA_TP_FNT_SLSH, OA_TP_FNT_SLSH, adj1 + 0x3c, 2.0f, 0.0f, 0x808080, alp1, 1.0f, 1.0f, 0, 0x30000, 1, 0, 0);
            PutNumberYW(0xb, dm_albm.csr[1] + 1, adj1 + 0x1e0, 0x1d, 1.0f, 1.0f, 0x808080, alp1, 0x30000, 2, 0);
        }
        
        switch (dm_albm.slot[1])
        {
            case 0:
                if (alp1 == 0x80)
                {
                    PutStringYW(9, 13, adj1 + 0x5a, 0x1d, 0x909090, 0x80, 0x2f000, 0);
                }
            break;
            case 1:
            case 2:
                if (alp1 == 0x80)
                {
                    PutStringYW(9, 0xe, adj1 + 0x5a, 0x1d, 0x909090, 0x80, 0x2f000, 0);
#ifdef BUILD_EU_VERSION
                    PutNumberYW(0, dm_albm.file[1],
                                pos_x + GetStrWidth((u_char *)GetIngameMSGAddr(9, 0xe)), 0x1d,
                                1.0f, 1.0f, 0x808080, 0x80, 0x2f000, 1, 0);
#else
                    PutNumberYW(0, dm_albm.file[1], adj1 + 0xb4, 0x1d, 1.0f, 1.0f, 0x808080, 0x80, 0x2f000, 1, 0);
                    PutStringYW(8, 0x6d, adj1 + 0xf0, 0x1d, 0x909090, 0x80, 0x2f000, 0);
                    PutNumberYW(0, dm_albm.slot[1], adj1 + 0x1a6, 0x1d, 1.0f, 1.0f, 0x808080, 0x80, 0x2f000, 1, 0);
#endif
                }
            break;
            case 3:
                if (alp1 == 0x80)
                {
                    PutStringYW(9, 0xf, adj1 + 0x5a, 0x1d, 0x909090, 0x80, 0x2f000, 0);
                }
            break;
        }
    }
    
    if (alp0 != 0)
    {
        int pos_x;
        
        DrawMessageBox(0x2d000, adj2, 17.0f, 582.0f, 48.0f, alp0);

        if (mode != 1)
        {
            if (dm_albm.side_chng_wait != 0)
            {
                PutSpriteYW(ALBM_CAPT_R1, ALBM_CAPT_R1, 53.0f, adj2 + 1, 0.0f, 0x808080, alp0 * flsh1, 1.0f, 1.0f, 0, 0x2b000, 1, 0x1, 0);
            }
            else
            {
                PutSpriteYW(ALBM_CAPT_R1, ALBM_CAPT_R1, 53.0f, adj2 + 1, 0.0f, 0x808080, alp0 * flsh1, 1.0f, 1.0f, 0, 0x2b000, 1, 0, 0);
            }
        }

        pos_x = adj2 + 0x2a;
        
        PutSpriteYW(OA_TP_FNT_SLSH, OA_TP_FNT_SLSH, pos_x, 2.0f, 0.0f, 0x808080, alp0, 1.0f, 1.0f, 0, 0x2b000, 1, 0, 0);
        PutNumberYW(0xb, dm_albm.pht_max[0], adj2 + 0x1ce, 0x1d, 1.0f, 1.0f, 0x808080, alp0, 0x2b000, 2, 0);
        PutNumberYW(0xb, 0x18, adj2 + 0x1f8, 0x1d, 1.0f, 1.0f, 0x808080, alp0, 0x2b000, 2, 0);
        
        if (dm_albm.csr_mode == 2)
        {
            if (mode != 0)
            {
                PutSpriteYW(OA_TP_FNT_SLSH, OA_TP_FNT_SLSH, adj2, 2.0f, 0.0f, 0x808080, alp0, 1.0f, 1.0f, 0, 0x2b000, 1, 0, 0);
                PutNumberYW(0xb, dm_albm.buf[dm_albm.csr[2]] + 1, adj2 + 0x1a4, 0x1d, 1.0f, 1.0f, 0x808080, alp0, 0x2b000, 2, 0);
            }
        }
        else if (dm_albm.csr[0] < dm_albm.pht_max[0])
        {
            PutSpriteYW(OA_TP_FNT_SLSH, OA_TP_FNT_SLSH, adj2, 2.0f, 0.0f, 0x808080, alp0, 1.0f, 1.0f, 0, 0x2b000, 1, 0, 0);
            PutNumberYW(0xb, dm_albm.csr[0] + 1, adj2 + 0x1a4, 0x1d, 1.0f, 1.0f, 0x808080, alp0, 0x2b000, 2, 0);
        }

        switch (dm_albm.slot[0])
        {
            case 0:
                if (alp0 == 0x80)
                {
                    PutStringYW(9, 13, adj2 + 0x1e, 0x1d, 0x909090, 0x80, 0x2a000, 0);
                }
            break;
            case 1:
            case 2:
                if (alp0 == 0x80)
                {
                    PutStringYW(9, 0xe, adj2 + 0x1e, 0x1d, 0x909090, 0x80, 0x2a000, 0);
#ifdef BUILD_EU_VERSION
                    PutNumberYW(0, dm_albm.file[0],
                                pos_x + GetStrWidth((u_char *)GetIngameMSGAddr(9, 14)), 0x1d,
                                1.0f, 1.0f, 0x808080, 0x80, 0x2a000, 1, 0);
#else
                    PutNumberYW(0, dm_albm.file[0], adj2 + 0x78, 0x1d, 1.0f, 1.0f, 0x808080, 0x80, 0x2a000, 1, 0);
                    PutStringYW(8, 0x6d, adj2 + 0xb4, 0x1d, 0x909090, 0x80, 0x2a000, 0);
                    PutNumberYW(0, dm_albm.slot[0], adj2 + 0x16a, 0x1d, 1.0f, 1.0f, 0x808080, 0x80, 0x2a000, 1, 0);
#endif
                }
            break;
            case 3:
                if (alp0 == 0x80)
                {
                    PutStringYW(9, 0xf, adj2 + 0x1e, 0x1d, 0x909090, 0x80, 0x2a000, 0);
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

    if (alp != 0x80) // Line 2129
    {
        return;
    }
    
    FlashStarYW(&flsh, 0xc0, 0x40, 0x3c, 0); // Line 2133

    
    if (sw == 0) // Line 2134
    {
        u_char csr;
        
        csr= dm_albm.csr[dm_albm.side];
        max = dm_albm.pht_max[dm_albm.side];
        if (max > 7) // Line 2138
        {
            if (csr <= 7) // Line 2140
            {
                if (
                    (dm_albm.side != 0 || dm_albm.is_title != 0) ||  // Line 2142
                    (max >= 9)  // Line 2143
                )
                {
                    alp_l = 0; // Line 2151
                    alp_r = flsh.alpha; // Line 2152
                }
                else
                {
                    alp_l = 0;
                    alp_r = 0;
                }
            }
            else if (csr < 16) // Line 2142 (2157)
            {
                if (dm_albm.side == 0 && dm_albm.is_title == 0)  // Line 2159
                {
                    if (max < 0x11)  // Line 2160
                    {
                        alp_r = 0;  // Line 2163
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
                    if (max < 0x10)  // Line 2167
                    {
                        alp_l = flsh.alpha;  // Line 2170
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
                alp_l = flsh.alpha;  // Line 2175
                alp_r = 0;
            }
        }
        else
        {
            alp_l = 0;  // Line 2189 ??
            alp_r = 0;
        }
    }
    else
    {
        
        if (dm_albm.csr_mode == 2)  // Line 2183
        {
            max = dm_albm.mem;
        }
        else
        {
            max = dm_albm.pht_max[dm_albm.side];  // Line 2184
        }
        
        if (max >= 2)  // Line 2186
        {
            alp_l = flsh.alpha;  // Line 2187
            alp_r = flsh.alpha;
        }
        else
        {
            alp_l = 0;  // Line 2189 ??
            alp_r = 0;
        }
    }
    
    SetSprFile(0x1ce0000);  // Line 2193

    switch (type)  // Line 2194
    {
    case 0:
        PutSpriteYW(TRIANGLE_LFT, TRIANGLE_LFT, -1.0f, -96.0f, 0.0f, 0x808080, alp_l, 1.0f, 1.0f, 0, 0xff, 1, 0, 0); // Line 2196
        PutSpriteYW(TRIANGLE_RGT, TRIANGLE_RGT, 1.0f, -96.0f, 0.0f, 0x808080, alp_r, 1.0f, 1.0f, 0, 0xff, 1, 0, 0); // Line 2197
    break;
    case 1:
        PutSpriteYW(TRIANGLE_LFT, TRIANGLE_LFT, -1.0f, 4.0f, 0.0f, 0x808080, alp_l, 1.0f, 1.0f, 0, 0xff, 1, 0, 0); // Line 2200
        PutSpriteYW(TRIANGLE_RGT, TRIANGLE_RGT, 1.0f, 4.0f, 0.0f, 0x808080, alp_r, 1.0f, 1.0f, 0, 0xff, 1, 0, 0); // Line 2201
    break;
    }
}

void ComWin(u_char mode, u_char csr, u_char alp)
{
    static FLSH_CORE flsh;
    u_char alp_csr;

    FlashStarYW(&flsh, 0x40, 0x80, 0x78, 0);
    
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
        PolySquareYW(30.0f, 66.0f, 0x68, 0xb5, 0, (int)(alp / 3), 1.0f, 1.0f, 0x4000, 0, 0, 0);
        CmnWindow(0x1b, 0, 0, alp, 0x80);
        PutSpriteYW(
            OA_TP_FNT_ZM2, OA_TP_FNT_EXT,
            40.0f, 68.0f,
            0.0f, 0x808080, alp,
            1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        if (dm_albm.csr_mode == 3)
        {
            PolySquareYW(38.0f, 158.0f, 0x58, 0x1c, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0, 0);
            PolySquareYW(38.0f, 188.0f, 0x58, 0x1c, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0, 0);
            PolySquareYW(38.0f, 218.0f, 0x58, 0x1c, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0, 0);
        }
        else if (dm_albm.side == 0 && dm_albm.is_title == 0)
        {
            PolySquareYW(38.0f, 128.0f, 0x58, 0x1c, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0, 0);
            PolySquareYW(38.0f, 158.0f, 0x58, 0x1c, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0, 0);
            PolySquareYW(38.0f, 188.0f, 0x58, 0x1c, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0, 0);
        }
        CmnCursol(0x25, (u_short)csr * 0x1e + 0x43, 0x5a, 0x1e, 64.0f, alp_csr, 0x1000);
    }
    break;
    case 1:
    {
        PolySquareYW(505.0f, 66.0f, 0x68, 0xb5, 0, (int)(alp / 3), 1.0f, 1.0f, 0x4000, 0, 0, 0);
        CmnWindow(0x1b, 0x1db, 0, alp, 0x80);
        PutSpriteYW(
            OA_TP_FNT_ZM2, OA_TP_FNT_EXT,
            515.0f, 68.0f,
            0.0f, 0x808080, alp,
            1.0f, 1.0f, 0, 0xff, 1, 0, 0);

        if (dm_albm.csr_mode == 3)
        {
            PolySquareYW(513.0f, 158.0f, 0x58, 0x1c, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0, 0);
            PolySquareYW(513.0f, 188.0f, 0x58, 0x1c, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0, 0);
            PolySquareYW(513.0f, 218.0f, 0x58, 0x1c, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0, 0);
        }
        else if (dm_albm.side == 0 && dm_albm.is_title == 0)
        {
            PolySquareYW(513.0f, 128.0f, 0x58, 0x1c, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0, 0);
            PolySquareYW(513.0f, 158.0f, 0x58, 0x1c, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0, 0);
            PolySquareYW(513.0f, 188.0f, 0x58, 0x1c, 0, (int)(alp * 2 / 3), 1.0f, 1.0f, 0x2000, 0, 0, 0);
        }
        CmnCursol(0x200, csr * 0x1e + 0x43, 0x5a, 0x1e, 64.0f, alp_csr, 0x1000);
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
    float flsh1;
    float flsh2;

    switch (type)
    {
    case 0:
    {
        DrawSPhotoFromSmallPhotoArea(0, 0, 0, 0, 0, 0x0);
        PutSpriteYW(
            OA_TP_FNT_BFFR, OA_TP_FNT_BFFR,
            0.0f, 0.0f,
            0.0f, 0x808080, alp,
            1.0f, 1.0f, 0x0, 0xff, 0x1, 0x0, 0x0);
        CmnWindow(0x1d, 0, 0, alp, 0x80);
        
        for (i = dm_albm.mem - 1, tmp = 0; i >= 0; i--, tmp++)
        {
            if (dm_albm.buf_sid[i] == 0)
            {
                pht_adr = album_save_dat[dm_albm.buf_sid[i]].pic[dm_albm.buf[i]].adr_no;
                DrawSPhotoFromSmallPhotoArea2AD(
                    dm_albm.src_addr,
                    pht_adr,
                    0x14000, 0, tmp * 0x3a + 0x89, 0x181, 0x34, 0x24, alp);
            }
            else
            {
                pht_adr = album_save_dat[dm_albm.buf_sid[i]].pic[dm_albm.buf[i]].adr_no;
                DrawSPhotoFromSmallPhotoArea2AD(
                    dm_albm.cpy_addr,
                    pht_adr,
                    0x14000, 0, tmp * 0x3a + 0x89, 0x181, 0x34, 0x24, alp);
            }
            
            PutSpriteYW(
                ALBM_SAM_WAKU, ALBM_SAM_WAKU,
                tmp * 0x3a + 0x89, 384.0f, 0.0f,
                0x808080,
                alp, 1.0f, 1.0f,
                0x0, 0xff, 0x1, 0x0, 0x0);
            
            if (dm_albm.copy_wait != 0x0 && tmp == 0)
            {
                PolySquareYW(
                    137.0f, 385.0f, 0x34, 0x24, 0x808080,
                    (dm_albm.copy_wait / 15.0f) * 128.0f, 1.0f,
                    1.0f, 0xf000, 0x1, 0x0, 0);
            }
            
            if (
                dm_albm.csr[2] == i &&
                (dm_albm.csr_mode - 2 < 2U || dm_albm.buf_set != 0x0) // 2U ??
            )
            {
                if (dm_albm.paste_wait != 0x0)
                {
                    flsh2 = SgSinf(((dm_albm.paste_wait % 0xf) * 6.2831855f) / 15.0f);
                    flsh2 = ((flsh2 + 1.0f) * 0.5f + 1.0f) * 0.5f;

                    PutSpriteYW(ALBM_MINI_SEALING, ALBM_MINI_SEALING, tmp * 0x3a + 0x88, 383.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0x0, 0xff, 0x1, 0x0, 0x0);
                    PutSpriteYW(ALBM_MINI_SEALING, ALBM_MINI_SEALING, tmp * 0x3a + 0x88, 383.0f, 0.0f, 0x808080, alp * flsh2, 1.0f, 1.0f, 0x0, 0xff, 0x1, 0x1, 0x0);

                }
                else
                {
                    flsh1 = 1.0f;

                    PutSpriteYW(ALBM_MINI_SEALING, ALBM_MINI_SEALING, tmp * 0x3a + 0x88, 383.0f, 0.0f, 0x606060, alp * flsh1, 1.0f, 1.0f, 0x0, 0xff, 0x1, 0x0, 0x0);
                }
            }
        }
    }
    break;
    case 1:
    {
        DrawMessageBox(0x19000, 24.0f, 377.0f, 592.0f, 48.0f, alp);
    }
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
        
        if (dm_albm.side == 0x0)
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
                CmnWindow(0x27, 0, -0xc4, alp, 0x80);

                PutSpriteYW(ALBM_FNT_JOHO, ALBM_FNT_JOHO, 15.0f, 133.0f, -90.0f, 0x808080, alp, 1.0f, 1.0f, 0x0, 0xff, 0x1, 0x0, 0x0);
                PutSpriteYW(ALBM_FNT_TARGET2, ALBM_FNT_POINT2, 0.0f, -149.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0x0, 0x6000, 0x1, 0x0, 0x0);
 
                DrawSPhotoFromSmallPhotoArea(0, 0x5000, 0, 0, 0, 0x0);
                PolySquareYW(71.0f, 76.0f, 0xe2, 0x90, 0, alp, 1.0f, 1.0f, 0x4000, 0x0, 0x0, 0);
                DrawPhotoFromPhotoWrk(target, 0x3000, 0, 0x47, 0x4c, 0xe2, 0x90, alp);

                PutSpriteYW(ALBM_WAKU_BIG_U2, ALBM_WAKU_BIG_U2, -25.0f, -31.0f, 0.0f, 0x808080, alp, 18.833334f, 1.0f, 0x0, 0x2000, 0x1, 0x0, 0x0);
                PutSpriteYW(ALBM_WAKU_BIG_D2, ALBM_WAKU_BIG_D2, -25.0f, -31.0f, 0.0f, 0x808080, alp, 18.833334f, 1.0f, 0x0, 0x2000, 0x1, 0x0, 0x0);
                PutSpriteYW(ALBM_WAKU_BIG_L2, ALBM_WAKU_BIG_L2, -25.0f, -31.0f, 0.0f, 0x808080, alp, 1.0f, 12.0f, 0x0, 0x2000, 0x1, 0x0, 0x0);
                PutSpriteYW(ALBM_WAKU_BIG_R2, ALBM_WAKU_BIG_R2, -25.0f, -31.0f, 0.0f, 0x808080, alp, 1.0f, 12.0f, 0x0, 0x2000, 0x1, 0x0, 0x0);

                TruthPhoto(0, 0, -0xc3, alp);
                DrawSPhotoFromSmallPhotoArea(0, 0x1000, 0, 0, 0, 0x0);
            }
            break;
        case 1:
            if (CheckPhotoExpandEnd())
            {
                CmnWindow(0x27, 0, -0x2f, alp, 0x80);

                PutSpriteYW(ALBM_FNT_JOHO, ALBM_FNT_JOHO, 15.0f, 289.0f, -90.0f, 0x808080, alp, 1.0f, 1.0f, 0x0, 0xff, 0x1, 0x0, 0x0);
                PutSpriteYW(ALBM_FNT_TARGET2, ALBM_FNT_POINT2, 0.0f, 0.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0x0, 0x6000, 0x1, 0x0, 0x0);

                DrawSPhotoFromSmallPhotoArea(0, 0x5000, 0, 0, 0, 0x0);
                PolySquareYW(71.0f, 225.0f, 0xe2, 0x90, 0, alp, 1.0f, 1.0f, 0x4000, 0x0, 0x0, 0);
                DrawPhotoFromPhotoWrk(target, 0x3000, 0, 0x47, 0xe1, 0xe2, 0x90, alp);

                PutSpriteYW(ALBM_WAKU_BIG_U2, ALBM_WAKU_BIG_U2, -25.0f, 118.0f, 0.0f, 0x808080, alp, 18.833334f, 1.0f, 0x0, 0x2000, 0x1, 0x0, 0x0);
                PutSpriteYW(ALBM_WAKU_BIG_D2, ALBM_WAKU_BIG_D2, -25.0f, 118.0f, 0.0f, 0x808080, alp, 18.833334f, 1.0f, 0x0, 0x2000, 0x1, 0x0, 0x0);
                PutSpriteYW(ALBM_WAKU_BIG_L2, ALBM_WAKU_BIG_L2, -25.0f, 118.0f, 0.0f, 0x808080, alp, 1.0f, 12.0f, 0x0, 0x2000, 0x1, 0x0, 0x0);
                PutSpriteYW(ALBM_WAKU_BIG_R2, ALBM_WAKU_BIG_R2, -25.0f, 118.0f, 0.0f, 0x808080, alp, 1.0f, 12.0f, 0x0, 0x2000, 0x1, 0x0, 0x0);

                TruthPhoto(0, 0, -0x2e, alp);
                DrawSPhotoFromSmallPhotoArea(0, 0x1000, 0, 0, 0, 0x0);
            }
            break;
        }
    }
    break;
    case 1:
        CmnWindow(0x6, 0, 0, alp, 0x80);

        PutSpriteYW(ALBM_FNT_JOHO, ALBM_FNT_JOHO, 14.0f, 371.0f, -90.0f, 0x808080, alp, 1.0f, 1.0f, 0x0, 0xff, 0x1, 0x0, 0x0);
        PutSpriteYW(ALBM_FNT_TARGET, ALBM_FNT_POINT, 0.0f, 0.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0x0, 0xff, 0x1, 0x0, 0x0);

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
        {0x18, 0x18, 0x18},
        {0x18, 0x18, 0x18},
        {0xc, 0x24, 0x18},
        {0x0, 0x18, 0x30},
    };
    u_short obj_py2[4][3] = {
        {0x18, 0x18, 0x18},
        {0x18, 0x18, 0x18},
        {0xc, 0x24, 0x18},
        {0x0, 0x18, 0x30},
    };
    PICTURE_WRK tmp_pic;

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
                    ig_menu_str.str = GetSubjectNameAddr(
                        tmp_pic.subject[i][0],
                        tmp_pic.subject[i][1],
                        tmp_pic.msn_no);
                    CopyStrDToStr(&ds, &ig_menu_str);
                    ds.pos_x = px + 0x172;
                    ds.pos_y = obj_py[obj_num][i] + 0x10b + py;
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
            PutStringYW(0x21, 0x3, px + 0x1b8U, obj_py[0][0] + 0x10b + py, 0x808080, str_alp, 0x1000, 0);
        }
        PutDate(&tmp_pic, px + 0x175, py + 0x154, str_alp, 0x1000);
        PutStringYW(0x1e, tmp_pic.room, px + 0x181, py + 0x171, 0x808080, str_alp, 0x1000, 0);
        PutNumberYW(0, tmp_pic.score, px + 0x1a5, py + 0x18d, 1.0f, 1.0f, 0x808080, str_alp, 0x1000, 7, 0);
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
                    ig_menu_str.str = GetSubjectNameAddr(
                        tmp_pic.subject[i][0],
                        tmp_pic.subject[i][1],
                        tmp_pic.msn_no);
                    CopyStrDToStr(&ds, &ig_menu_str);
                    ds.pos_x = px + 0x66;
                    ds.pos_y = obj_py2[obj_num][i] + 0x15b + py;
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
            PutStringYW(0x21, 0x3, px + 0xb9U, obj_py2[0][0] + 0x15b + py, 0x808080, str_alp, 0x1000, 0x0);
        }
        PutDate(&tmp_pic, px + 0x184, py + 0x154, str_alp, 0x1000);
        PutStringYW(0x1e, tmp_pic.room, px + 0x18f, py + 0x171, 0x808080, str_alp, 0x1000, 0x0);
        PutNumberYW(0, tmp_pic.score, px + 0x1b3, py + 0x18c, 1.0f, 1.0f, 0x808080, str_alp, 0x1000, 7, 0);
    }
    break;
    }
}

void BtnExt(u_char mode, u_char alp)
{
    if (dm_albm.buf_set == 0x0)
    {
        switch (mode)
        {
            case 0:
                SetSprFile(0x1ce0000);
                DispCaption(0x16, alp);
                break;
            case 1:
                SetSprFile(0x1ce0000);
                DispCaption(0x16, alp);
            break;
            case 2:
            default:
            return;
        }
    }
}

char InKeyTamusi()
{
    u_char hit;

    hit = 0;
    if ((*key_now[0] != 0) || Ana2PadDirCnt(0))
    {
        hit = 1;
    }
    if ((*key_now[1] != 0) || Ana2PadDirCnt(2))
    {
        hit = hit + 1;
    }
    if ((*key_now[2] != 0) || Ana2PadDirCnt(3))
    {
        hit = hit + 1;
    }
    if ((*key_now[3] != 0) || Ana2PadDirCnt(1))
    {
        hit = hit + 1;
    }
    if (*key_now[6] != 0)
    {
        hit = hit + 1;
    }
    if (*key_now[7] != 0)
    {
        hit = hit + 1;
    }
    if (*key_now[4] != 0)
    {
        hit = hit + 1;
    }
    if (*key_now[10] != 0)
    {
        hit = hit + 1;
    }
    if (*key_now[11] != 0)
    {
        hit = hit + 1;
    }
    if (*key_now[8] != 0)
    {
        hit = hit + 1;
    }
    if (*key_now[9] != 0)
    {
        hit = hit + 1;
    }
    if (*key_now[13] != 0)
    {
        hit = hit + 1;
    }
    if ((dm_albm.exit_realy != 0x0) || (dm_albm.yes_no != 0x0))
    {
        hit = hit + 1;
    }
    return hit;
}

char GetAlbumCurPos(char side)
{
    if (
        (dm_albm.side != 0 || dm_albm.is_title != 0) &&
        (dm_albm.pht_max[side] < 0x18)
    )
    {
        return dm_albm.pht_max[side];
    }
    else
    {
        return dm_albm.csr[side];
    }
}
