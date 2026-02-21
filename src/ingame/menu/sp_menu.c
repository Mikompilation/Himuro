#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "sp_menu.h"

#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/message.h"
// #include "graphics/graph2d/tim2.h" // CopySprDToSpr
#include "graphics/graph3d/sgdma.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/motion/mdlwork.h"
#include "ingame/enemy/ene_ctl.h"
#include "ingame/event/ev_main.h"
#include "ingame/info/inf_disp.h"
#include "ingame/map/map_ctrl.h"
#include "ingame/menu/ig_file.h"
#include "ingame/menu/ig_map.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/item.h"
#include "ingame/menu/sp_menu.h"
#include "ingame/plyr/plyr_ctl.h"
#include "main/glob.h"
#include "mc/mc_at.h"
#include "mc/mc_disp.h"
#include "mc/mc_exec.h"
#include "mc/mc_main.h"
#include "os/backup.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"

#define INCLUDING_FROM_SP_MENU_C
#include "outgame/memory_album.h"
#undef INCLUDING_FROM_SP_MENU_C

static u_char SaveConte();
static void WarningMsg(u_char msg, u_char csr0, u_char alp, u_char ha);
static void GetSvpNo(u_char *no);
static void DspSavePoint(u_char csr1, u_char csr2, u_char alp, u_char type);
static void DspSpLenz(u_char dsp_no, u_char alp);
static void DspSpCate(u_char csr1, u_char csr2, u_char alp);
static void DspSpData(u_char csr2, u_char alp, u_char type);
static void DspSpFile(u_char file_no, u_char pos_y, u_char alp, u_char type);
static void DspSpInfo(u_char file_no, u_char pos_y, u_char alp, u_char type);
static void DspSpFile2(u_char file_no, u_char pos_y, u_char alp, u_char type);
static void DspSpInfo2(u_char csr_no, u_char pos_y, u_char alp, u_char type);
static void OpenBook(int i, BOOK *book);
static void SpMsgWin(u_char alpha);
static void SpSideBar(short int pos_x, short int pos_y, u_char alpha);
static void SpTrFlsh(short int pos_x, short int pos_y, short int alpha);
static void DspSaveTitle(u_char type, short int pos_x, short int pos_y, u_char alp, u_char pri);
static void PhotoDataExchange();
static int AlbmDesignLoadInGame(u_char side, u_char type);

static SAVE_CONTE sav_con;
static FLSH_CORE flsh[3];
static int sp_load_id;
static u_char btn_flsh_cnt;

static MIRACLE_LENZ mrcl_lnz;
static SV_DSP_SW sv_dsp_sw;
static DSP_SVP dsp_svp;
static ALBM_MODE albm_mode;
static BOOK book[6];

#define PI 3.1415927f

#ifdef BUILD_EU_VERSION
#define SIDE_0_ADDRESS 0x1d83000;
#define SIDE_1_ADDRESS 0x1d93470;
#else
#define SIDE_0_ADDRESS 0x1d88100;
#define SIDE_1_ADDRESS 0x1d98570;
#endif

#ifdef BUILD_EU_VERSION
#define LOAD_ADDRESS_1 0x1d54030
#define LOAD_ADDRESS_2 0x1d10000
#define LOAD_ADDRESS_3 0x1d23680
#define LOAD_ADDRESS_4 0x1d210c0
#define LOAD_ADDRESS_5 0x1d83000
#else
#define LOAD_ADDRESS_1 0x1d59630
#define LOAD_ADDRESS_2 0x1d15600
#define LOAD_ADDRESS_3 0x1d28c80
#define LOAD_ADDRESS_4 0x1d266c0
#define LOAD_ADDRESS_5 0x1d88100
#endif

void SavePointMenuInit()
{
    spmenu_wrk = (SAVE_POINT_MENU_WRK){0};
}

void SavePointMenuOpen()
{
    if (SavePointLightJudge() != 0)
    {
        SavePointMenuOpenInit(0);
    }
}

void SavePointMenuOpenInit(u_char msn)
{
    sav_con = (SAVE_CONTE){0};
    mrcl_lnz = (MIRACLE_LENZ){0};
    sv_dsp_sw = (SV_DSP_SW){0};
    dsp_svp = (DSP_SVP){0};

    spmenu_wrk.csr[3] = 0;
    spmenu_wrk.csr[4] = 1;

    dsp_svp.slct_mode = -1;
    dsp_svp.top_alp = 0;
    dsp_svp.bak_alp = 0;
    dsp_svp.msg_alp = 0;

    CaptureScreen(0x1a40);
    DrawScreen(0x7f000, 0x1a40, 0x80, 0x80, 0x80, 0x80);

    ingame_wrk.stts |= 0x20;

    InitialDmaBuffer();

    if (msn == 0)
    {
        spmenu_wrk.mode = SPMENU_START_INN;
        ingame_wrk.mode = INGAME_MODE_SAVE_POINT;
    }
    else
    {
        spmenu_wrk.mode = INGAME_MODE_SGST_DISP;
    }

    dsp_svp.slct_mode = -1;

    sp_load_id = -1;

    btn_flsh_cnt = 0;
}

void BtlModSaveInit()
{
    sav_con = (SAVE_CONTE){0};
    mrcl_lnz = (MIRACLE_LENZ){0};
    sv_dsp_sw = (SV_DSP_SW){0};
    dsp_svp = (DSP_SVP){0};

    spmenu_wrk.csr[3] = 0;
    spmenu_wrk.csr[4] = 1;

    dsp_svp.slct_mode = -1;
    dsp_svp.top_alp = 0;
    dsp_svp.bak_alp = 0;
    dsp_svp.msg_alp = 0;

    InitialDmaBuffer();

    spmenu_wrk.mode = SPMENU_OPEN_FOR_MSN;
    ingame_wrk.mode = INGAME_MODE_SAVE_POINT;

    dsp_svp.slct_mode = -1;

    sp_load_id = -1;

    btn_flsh_cnt = 0;
}

char SavePointMenuMain(u_char msn)
{
    u_char mode;
    u_char rtrn;
    int is_btlmode;
    u_char alp;

    is_btlmode = 0;

    if (msn == 3)
    {
        msn = 2;

        is_btlmode = 1;
    }

    if (msn != 2)
    {
        if (poss_item[9] != 0)
        {
            if (poss_item[1] < 30)
            {
                mode = 2;
            }
            else
            {
                mode = 1;
            }
        }
        else
        {
            mode = 0;
        }
    }
    else
    {
        mode = 0;
    }

    if (sp_load_id != -1)
    {
        if (IsLoadEnd(sp_load_id) != 0)
        {
            sp_load_id = -1;
        }
    }

    if (msn != 2)
    {
        DrawScreen(0x0ffff7f0, 0x1a40, 0x80, 0x80, 0x80, 0x80);
    }

    rtrn = 0;

    BgFusumaYW(0x606060, 0.0f, dsp_svp.bak_alp, 0x7d000);

    switch(spmenu_wrk.mode)
    {
    case 0:
        if (dsp_svp.msg_alp + 8 < 0x80)
        {
            dsp_svp.msg_alp += 8;
        }
        else
        {
            dsp_svp.msg_alp = 0x80;

            spmenu_wrk.mode = 1;
        }
    break;
    case 1:
        if (*key_now[4] == 1)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            spmenu_wrk.mode = 2;
        }
        else if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);

#ifdef BUILD_EU_VERSION
            sp_load_id = LoadReqLanguage(PL_PSVP_E_PK2, LOAD_ADDRESS_1);
#else
            sp_load_id = LoadReq(PL_PSVP_PK2, LOAD_ADDRESS_1);
#endif

            spmenu_wrk.mode = 3;
            spmenu_wrk.csr[4] = 0;
        }
    break;
    case 2:
        if (dsp_svp.msg_alp - 8 > 0)
        {
            dsp_svp.msg_alp -= 8;
        }
        else
        {
            dsp_svp.msg_alp = 0;

            ingame_wrk.stts &= ~0x20;

            InitialDmaBuffer();

            ingame_wrk.mode = 6;
        }
    break;
    case 3:
        if (sp_load_id == -1)
        {
            spmenu_wrk.mode = 4;
        }
    break;
    case 4:
        SetSprFile(LOAD_ADDRESS_1);

        if (dsp_svp.top_alp + 8 < 0x80)
        {
            dsp_svp.top_alp += 8;
            dsp_svp.bak_alp = dsp_svp.top_alp;
            dsp_svp.msg_alp = 0x80 - dsp_svp.top_alp;
        }
        else
        {
            dsp_svp.bak_alp = 0x80;
            dsp_svp.top_alp = 0x80;
            dsp_svp.msg_alp = 0;
            dsp_svp.slct_mode = -1;

            spmenu_wrk.mode = 5;
        }
    break;
    case 5:
        SetSprFile(LOAD_ADDRESS_1);
        SavePointMenuModeSlct(msn, mode);
    break;
    case 6:
        SetSprFile(LOAD_ADDRESS_1);

        if (sp_load_id == -1)
        {
            if (dsp_svp.top_alp - 8 > 0)
            {
                dsp_svp.top_alp -= 8;
                dsp_svp.bak_alp = 0x80;
            }
            else
            {
                dsp_svp.top_alp = 0;
                dsp_svp.bak_alp = 0x80;

                if (is_btlmode)
                {
                    mcInit(0, (u_int *)0x420000, mc_header.game.msn_flg);
                }
                else if (msn == 0)
                {
                    mcInit(0, (u_int *)0x420000, 0);
                }
                else
                {
                    mcInit(0, (u_int *)0x420000, 1);
                }

                spmenu_wrk.mode = 7;

                sav_con.step = 1;
            }
        }
    break;
    case 7:
        SetSprFile(LOAD_ADDRESS_2);
        SetSprFile(LOAD_ADDRESS_1);
        SetSprFile(LOAD_ADDRESS_3);

        if (McAtSaveChk() != 0)
        {
            SendManMdlTex();

            spmenu_wrk.mode = 8;
        }
    break;
    case 8:
        SetSprFile(LOAD_ADDRESS_1);

        if (sp_load_id == -1)
        {
            dsp_svp.bak_alp = 0x80;

            if (dsp_svp.top_alp + 8 < 0x80)
            {
                dsp_svp.top_alp += 8;
            }
            else
            {
                dsp_svp.top_alp = 0x80;
                dsp_svp.slct_mode = -1;

                spmenu_wrk.mode = 5;
            }
        }
    break;
    case 9:
        SetSprFile(LOAD_ADDRESS_1);

        if (sp_load_id == -1)
        {
            spmenu_wrk.mode = 10;
        }
    break;
    case 10:
        SetSprFile(LOAD_ADDRESS_1);

        dsp_svp.bak_alp = 0x80;

        if (dsp_svp.top_alp - 8 > 0)
        {
            dsp_svp.top_alp -= 8;
        }
        else
        {
            dsp_svp.top_alp = 0;

            InitialDmaBuffer();

            if (dsp_svp.in_album != 0)
            {
                mcInit(4, (u_int *)0x420000, 0);
            }
            else
            {
                mcInit(3, (u_int *)0x420000, 0);
            }

            spmenu_wrk.mode = 11;
        }
    break;
    case 11:
        SetSprFile(LOAD_ADDRESS_1);
        SetSprFile(LOAD_ADDRESS_2);
        SetSprFile(LOAD_ADDRESS_3);

        switch (McAtLoadChk(2))
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            if (dsp_svp.in_album != 0)
            {
                album_save_dat[1] = mc_album_save;

                dsp_svp.atype_buf[1] = mc_album_type;

                MemAlbmInit2(1, mc_photo_num, mc_album_type, mc_ctrl.port + 1, mc_ctrl.sel_file + 1);

                sp_load_id = AlbmDesignLoadInGame(0, 5);
                sp_load_id = AlbmDesignLoadInGame(1, mc_album_type);
#ifdef BUILD_EU_VERSION
                sp_load_id = LoadReqLanguage(PL_ALBM_E_PK2, LOAD_ADDRESS_2);
#else
                sp_load_id = LoadReq(PL_ALBM_PK2, LOAD_ADDRESS_2);
#endif

                spmenu_wrk.mode = 12;

            } else {
                PhotoDataExchange();

                album_save_dat[1] = mc_album_save;

                dsp_svp.atype_buf[0] = 5;
                dsp_svp.atype_buf[1] = mc_album_type;

                MemAlbmInit(0, 0xff, mc_photo_num, 0xff, mc_album_type, 0xff, mc_ctrl.port + 1, 0xff, (u_char)(mc_ctrl.sel_file + 1));

                sp_load_id = AlbmDesignLoadInGame(0, 5);
                sp_load_id = AlbmDesignLoadInGame(1, mc_album_type);
#ifdef BUILD_EU_VERSION
                sp_load_id = LoadReqLanguage(PL_ALBM_E_PK2, LOAD_ADDRESS_2);
#else
                sp_load_id = LoadReq(PL_ALBM_PK2, LOAD_ADDRESS_2);
#endif

                spmenu_wrk.mode = 12;
            }
        break;
        case 2:
            if (dsp_svp.in_album != 0)
            {
                MemAlbmInit3();

                sp_load_id = AlbmDesignLoadInGame(0, dsp_svp.atype_buf[0]);
                sp_load_id = AlbmDesignLoadInGame(1, dsp_svp.atype_buf[1]);
#ifdef BUILD_EU_VERSION
                sp_load_id = LoadReqLanguage(PL_ALBM_E_PK2, LOAD_ADDRESS_2);
#else
                sp_load_id = LoadReq(PL_ALBM_PK2, LOAD_ADDRESS_2);
#endif

                spmenu_wrk.mode = 12;
            }
            else
            {
#ifdef BUILD_EU_VERSION
                sp_load_id = LoadReqLanguage(PL_PLDT_E_PK2, LOAD_ADDRESS_4);
                sp_load_id = LoadReqLanguage(PL_MTOP_E_PK2, LOAD_ADDRESS_2);
#else
                sp_load_id = LoadReq(PL_PLDT_PK2, LOAD_ADDRESS_4);
                sp_load_id = LoadReq(PL_MTOP_PK2, LOAD_ADDRESS_2);
#endif

                spmenu_wrk.mode = 0x10;
            }
        break;
        case 3:
            if (dsp_svp.in_album != 0)
            {
                dsp_svp.atype_buf[1] = 5;

                NewAlbumInit(1);

                sp_load_id = AlbmDesignLoadInGame(0, 5);
                sp_load_id = AlbmDesignLoadInGame(1, 5);
#ifdef BUILD_EU_VERSION
                sp_load_id = LoadReqLanguage(PL_ALBM_E_PK2, LOAD_ADDRESS_2);
#else
                sp_load_id = LoadReq(PL_ALBM_PK2, LOAD_ADDRESS_2);
#endif

                spmenu_wrk.mode = 12;
            }
            else
            {
                dsp_svp.atype_buf[0] = 5;

                PhotoDataExchange();
                MemAlbmInit(0, 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0);

                sp_load_id = AlbmDesignLoadInGame(0, 5);

                dsp_svp.atype_buf[1] = 5;

                NewAlbumInit(1);

                sp_load_id = AlbmDesignLoadInGame(1, 5);

                spmenu_wrk.mode = 12;

#ifdef BUILD_EU_VERSION
                sp_load_id = LoadReqLanguage(PL_ALBM_E_PK2, LOAD_ADDRESS_2);
#else
                sp_load_id = LoadReq(PL_ALBM_PK2, LOAD_ADDRESS_2);
#endif
            }
        break;
        }
    break;
    case 12:
        if (sp_load_id == -1)
        {
            spmenu_wrk.mode = 0xd;
        }
    break;
    case 13:
        SetSprFile(LOAD_ADDRESS_2);

        switch(SweetMemories(0, 0x80))
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            if (is_btlmode)
            {
                mcInit(2, (u_int *)0x420000, mc_header.game.msn_flg);
            }
            else if (msn == 0)
            {
                mcInit(2, (u_int *)0x420000, 0);
            }
            else
            {
                mcInit(2, (u_int *)0x420000, 1);
            }

            dsp_svp.load_side = 0;

#ifdef BUILD_EU_VERSION
            sp_load_id = LoadReq(PL_SAVE_E_PK2, LOAD_ADDRESS_2);
            sp_load_id = LoadReqLanguage(PL_ALBM_SAVE_E_PK2, LOAD_ADDRESS_3);
#else
            sp_load_id = LoadReq(PL_SAVE_PK2, LOAD_ADDRESS_2);
            sp_load_id = LoadReq(PL_ALBM_SAVE_PK2, LOAD_ADDRESS_3);
#endif

            spmenu_wrk.mode = 14;

            dsp_svp.in_album = 1;
        break;
        case 2:
            if (is_btlmode)
            {
                mcInit(2, (u_int *)0x420000, mc_header.game.msn_flg);
            }
            else if (msn == 0)
            {
                mcInit(2, (u_int *)0x420000, 0);
            }
            else
            {
                mcInit(2, (u_int *)0x420000, 1);
            }

            dsp_svp.load_side = 1;

#ifdef BUILD_EU_VERSION
            sp_load_id = LoadReqLanguage(PL_SAVE_E_PK2, LOAD_ADDRESS_2);
            sp_load_id = LoadReqLanguage(PL_ALBM_SAVE_E_PK2, LOAD_ADDRESS_3);
#else
            sp_load_id = LoadReq(PL_SAVE_PK2, LOAD_ADDRESS_2);
            sp_load_id = LoadReq(PL_ALBM_SAVE_PK2, LOAD_ADDRESS_3);
#endif

            dsp_svp.in_album = 1;

            spmenu_wrk.mode = 14;
        break;
        case 3:
            dsp_svp.load_side = 0;

#ifdef BUILD_EU_VERSION
            sp_load_id = LoadReqLanguage(PL_SAVE_E_PK2, LOAD_ADDRESS_2);
            sp_load_id = LoadReqLanguage(PL_ALBM_SAVE_E_PK2, LOAD_ADDRESS_3);
#else
            sp_load_id = LoadReq(PL_SAVE_PK2, LOAD_ADDRESS_2);
            sp_load_id = LoadReq(PL_ALBM_SAVE_PK2, LOAD_ADDRESS_3);
#endif

            spmenu_wrk.mode = 9;

            dsp_svp.in_album = 1;
        break;
        case 4:
            dsp_svp.load_side = 1;

#ifdef BUILD_EU_VERSION
            sp_load_id = LoadReqLanguage(PL_SAVE_E_PK2, LOAD_ADDRESS_2);
            sp_load_id = LoadReqLanguage(PL_ALBM_SAVE_E_PK2, LOAD_ADDRESS_3);
#else
            sp_load_id = LoadReq(PL_SAVE_PK2, LOAD_ADDRESS_2);
            sp_load_id = LoadReq(PL_ALBM_SAVE_PK2, LOAD_ADDRESS_3);
#endif

            dsp_svp.in_album = 1;

            spmenu_wrk.mode = 9;
        break;
        case 5:
#ifdef BUILD_EU_VERSION
            sp_load_id = LoadReqLanguage(PL_FNDR_E_PK2, LOAD_ADDRESS_5);
#else
            sp_load_id = LoadReq(PL_FNDR_PK2, LOAD_ADDRESS_5);
#endif

            spmenu_wrk.mode = 16;
        break;
        }
    break;
    case 14:
        SetSprFile(LOAD_ADDRESS_1);

        if (sp_load_id == -1)
        {
            spmenu_wrk.mode = 15;
        }
    break;
    case 15:
        SetSprFile(LOAD_ADDRESS_1);
        SetSprFile(LOAD_ADDRESS_2);
        SetSprFile(LOAD_ADDRESS_3);

        switch (McAtAlbmChk())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            if (dsp_svp.in_album != 0)
            {
                dsp_svp.atype_buf[dsp_svp.load_side] = mc_album_type;

                MemAlbmInit2(dsp_svp.load_side, mc_photo_num, mc_album_type, mc_ctrl.port + 1, mc_ctrl.sel_file + 1);

                sp_load_id = AlbmDesignLoadInGame(0, dsp_svp.atype_buf[0]);
                sp_load_id = AlbmDesignLoadInGame(1, dsp_svp.atype_buf[1]);
#ifdef BUILD_EU_VERSION
                sp_load_id = LoadReqLanguage(PL_ALBM_E_PK2, LOAD_ADDRESS_2);
#else
                sp_load_id = LoadReq(PL_ALBM_PK2, LOAD_ADDRESS_2);
#endif

                spmenu_wrk.mode = 12;
            }
            else
            {
                PhotoDataExchange();

                dsp_svp.atype_buf[0] = 5;
                dsp_svp.atype_buf[1] = mc_album_type;

                MemAlbmInit(0, 0xff, mc_photo_num, 0xff, mc_album_type, 0xff, mc_ctrl.port + 1, 0xff, (u_char)(mc_ctrl.sel_file + 1));
                NewAlbumInit(1);

                sp_load_id = AlbmDesignLoadInGame(0, dsp_svp.atype_buf[0]);
                sp_load_id = AlbmDesignLoadInGame(1, dsp_svp.atype_buf[1]);
#ifdef BUILD_EU_VERSION
                sp_load_id = LoadReqLanguage(PL_ALBM_E_PK2, LOAD_ADDRESS_2);
#else
                sp_load_id = LoadReq(PL_ALBM_PK2, LOAD_ADDRESS_2);
#endif

                spmenu_wrk.mode = 12;
            }
        break;
        case 2:
            if (dsp_svp.in_album != 0)
            {
                MemAlbmInit3();

                sp_load_id = AlbmDesignLoadInGame(0, dsp_svp.atype_buf[0]);
                sp_load_id = AlbmDesignLoadInGame(1, dsp_svp.atype_buf[1]);
#ifdef BUILD_EU_VERSION
                sp_load_id = LoadReqLanguage(PL_ALBM_E_PK2, LOAD_ADDRESS_2);
#else
                sp_load_id = LoadReq(PL_ALBM_PK2, LOAD_ADDRESS_2);
#endif

                spmenu_wrk.mode = 12;
            }
            else
            {
                mcInit(3, (u_int *)0x420000, 0);

                spmenu_wrk.mode = 11;
            }
        break;
        case 3:
            // do nothing ...
        break;
        }
    break;
    case 16:
        SetSprFile(LOAD_ADDRESS_1);

        if (sp_load_id == -1)
        {
            dsp_svp.bak_alp = 0x80;

            if (dsp_svp.top_alp + 8 < 128)
            {
                dsp_svp.top_alp += 8;
            }
            else
            {
                dsp_svp.top_alp = 0x80;

                InitialDmaBuffer();

                dsp_svp.slct_mode = -1;

                spmenu_wrk.mode = 5;
            }
        }
    break;
    case 17:
        // do nothing ...
    break;
    case 18:
        SetSprFile(LOAD_ADDRESS_1);

        if (sp_load_id == -1)
        {
            if (dsp_svp.top_alp - 8 > 0)
            {
                dsp_svp.top_alp = (u_char)(dsp_svp.top_alp - 8);
                dsp_svp.bak_alp = dsp_svp.top_alp;
            }
            else
            {
                dsp_svp.top_alp = 0;
                dsp_svp.bak_alp = 0;

                if (msn == 0)
                {
                    ingame_wrk.stts &= ~0x20;

                    InitialDmaBuffer();

                    ingame_wrk.mode = 6;
                }
                else
                {
                    spmenu_wrk.mode = 20;
                }
            }
        }
    break;
    case 19:
#ifdef BUILD_EU_VERSION
        sp_load_id = LoadReqLanguage(PL_PSVP_E_PK2, LOAD_ADDRESS_1);
#else
        sp_load_id = LoadReq(PL_PSVP_PK2, LOAD_ADDRESS_1);
#endif

        spmenu_wrk.mode = 3;
        spmenu_wrk.csr[4] = 0;
    break;
    case 20:
        rtrn = 1;
    break;
    }

    if (dsp_svp.top_alp != 0)
    {
        SavePointMenuModeSlctDisp(msn, spmenu_wrk.csr[3], spmenu_wrk.csr[4], dsp_svp.top_alp, mode);
    }
    else
    {
        dsp_svp.slct_mode = -1;
    }

    if (msn == 1 || msn == 2)
    {
        dsp_svp.msg_alp = 0;
    }

    if (dsp_svp.msg_alp != 0)
    {
        DrawMessageBox(0xf000, 56.0f, 304.0f, 528.0f, 106.0f, dsp_svp.msg_alp);

        PutStringYW(9, 0, 78, 321, 0x808080, dsp_svp.msg_alp, 0x5000, 0);

        if (dsp_svp.btn_tmr < 60)
        {
            dsp_svp.btn_tmr++;
        }
        else
        {
            dsp_svp.btn_tmr = 0;
        }

        alp = (SgSinf(dsp_svp.btn_tmr * (PI * 2) / 60.0f) * dsp_svp.msg_alp + dsp_svp.msg_alp) * 0.5f;

        DrawButtonTex(0xa000, 3, 536.0f, 368.0f, alp);
    }

    return rtrn;
}

void SavePointMenuModeSlct(u_char msn, u_char mode)
{
    if (dsp_svp.slct_mode == -1)
    {
        if (*key_now[4] == 1)
        {
            if (spmenu_wrk.csr[3] != 3)
            {
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

                spmenu_wrk.csr[3] = 3;
                spmenu_wrk.csr[4] = 1;

                dsp_svp.slct_mode = 3;
                dsp_svp.str_no = 3;
            }
        }
        else if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            if (mode < 2 && spmenu_wrk.csr[3] == 2)
            {
                SeStartFix(2, 0, 0x1000, 0x1000, 0);
            }
            else
            {
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

                spmenu_wrk.csr[4] = 1;

                dsp_svp.slct_mode = spmenu_wrk.csr[3];
                dsp_svp.str_no = spmenu_wrk.csr[3];
            }
        }
        else if (
            *key_now[3] == 1 ||
            (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
            Ana2PadDirCnt(1) == 1 ||
            (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
        )
        {
            if (spmenu_wrk.csr[3] < 3)
            {
                spmenu_wrk.csr[3]++;
            }
            else
            {
                spmenu_wrk.csr[3] = 0;
            }

            if (mode < 2 && spmenu_wrk.csr[3] == 2)
            {
                spmenu_wrk.csr[3] = 3;
            }

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
        else if (
            *key_now[2] == 1 ||
            (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
            Ana2PadDirCnt(3) == 1 ||
            (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
        )
        {
            if (spmenu_wrk.csr[3] == 0)
            {
                spmenu_wrk.csr[3] = 3;
            }
            else
            {
                spmenu_wrk.csr[3]--;
            }

            if (mode < 2 && spmenu_wrk.csr[3] == 2)
            {
                spmenu_wrk.csr[3] = 1;
            }

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
    }
    else if (*key_now[4] == 1)
    {
        SeStartFix(3, 0, 0x1000, 0x1000, 0);

        dsp_svp.slct_mode = -1;
    }
    else if (*key_now[6] == 1 || *key_now[5] == 1)
    {
        if (spmenu_wrk.csr[4] == 0)
        {
            switch (dsp_svp.slct_mode)
            {
            case 0:
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

#ifdef BUILD_EU_VERSION
                sp_load_id = LoadReqLanguage(PL_SAVE_E_PK2, LOAD_ADDRESS_2);
#else
                sp_load_id = LoadReq(PL_SAVE_PK2, LOAD_ADDRESS_2);
#endif
                sp_load_id = LoadReq(SV_PHT_PK2, LOAD_ADDRESS_3);

                spmenu_wrk.mode = SPMODE_SAVE_INN;
            break;
            case 1:
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

#ifdef BUILD_EU_VERSION
                sp_load_id = LoadReqLanguage(PL_SAVE_E_PK2, LOAD_ADDRESS_2);
                sp_load_id = LoadReqLanguage(PL_ALBM_SAVE_E_PK2, LOAD_ADDRESS_3);
#else
                sp_load_id = LoadReq(PL_SAVE_PK2, LOAD_ADDRESS_2);
                sp_load_id = LoadReq(PL_ALBM_SAVE_PK2, LOAD_ADDRESS_3);
#endif

                dsp_svp.atype_buf[0] = 5;
                dsp_svp.atype_buf[1] = 5;
                dsp_svp.in_album = 0;

                spmenu_wrk.mode = SPMODE_ALBM_PSVP_WAIT;
            break;
            case 2:
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

                poss_item[1] = 30;

                dsp_svp.slct_mode = -1;
            break;
            case 3:
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

#ifdef BUILD_EU_VERSION
                sp_load_id = LoadReqLanguage(PL_PLDT_E_PK2, LOAD_ADDRESS_4);
                sp_load_id = LoadReqLanguage(PL_MTOP_E_PK2, LOAD_ADDRESS_2);
#else
                sp_load_id = LoadReq(PL_PLDT_PK2, LOAD_ADDRESS_4);
                sp_load_id = LoadReq(PL_MTOP_PK2, LOAD_ADDRESS_2);
#endif

                spmenu_wrk.mode = SPMENU_EXIT;
            break;
            }
        }
        else
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);

            dsp_svp.slct_mode = -1;
        }
    }
    else if (*key_now[3] == 1 || Ana2PadDirCnt(1) == 1)
    {
        SeStartFix(0, 0, 0x1000, 0x1000, 0);

        spmenu_wrk.csr[4] = 1 - spmenu_wrk.csr[4];

    }
    else if (*key_now[2] == 1 || Ana2PadDirCnt(3) == 1)
    {
        SeStartFix(0, 0, 0x1000, 0x1000, 0);

        spmenu_wrk.csr[4] = 1 - spmenu_wrk.csr[4];
    }
}

void SavePointMenuModeSlctDisp(u_char msn, u_char csr3, u_char csr4, u_char alp_max, u_char mode)
{
    int i;
    u_char alp;
    int rgb;
    short int csr_x0[4] = { 25, 243, -1, 461 };
    short int csr_x1[4] = { 25, 168, 319, 461 };
    short int csr_x2[4] = { 25, 168, 319, 461 };
    short int csr;
    static short int adj_x = 0;
    static short int adj_y = 0;

    if (*key_now[0] != 0 || *key_now[8] == 1)
    {
        adj_y--;
    }
    else if (*key_now[1] != 0 || *key_now[10] == 1)
    {
        adj_y++;
    }
    else if (*key_now[2] != 0 || *key_now[9] == 1)
    {
        adj_x--;
    }
    else if (*key_now[3] != 0 || *key_now[11] == 1)
    {
        adj_x++;
    }

    PutSpriteYW(PSP_FNT_SP, PSP_FNT_SP, 0.0f, 0.0f, 0.0f, 0x808080, alp_max, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PSP_FNT_SAVE, PSP_FNT_SAVE, 0.0f, 0.0f, 0.0f, 0x808080, alp_max, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PSP_FNT_RETURN, PSP_FNT_RETURN, 0.0f, 0.0f, 0.0f, 0x808080, alp_max, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    switch (mode)
    {
    case 0:
        PutSpriteYW(PSP_FNT_ALBUM, PSP_FNT_ALBUM, 75.0f, 0.0f, 0.0f, 0x808080, alp_max, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    break;
    case 1:
        PutSpriteYW(PSP_FNT_ALBUM, PSP_FNT_ALBUM, 0.0f, 0.0f, 0.0f, 0x808080, alp_max, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(PSP_FNT_FILM, PSP_FNT_FILM, 0.0f, 0.0f, 0.0f, 0x303030, alp_max, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    break;
    case 2:
        PutSpriteYW(PSP_FNT_ALBUM, PSP_FNT_ALBUM, 0.0f, 0.0f, 0.0f, 0x808080, alp_max, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(PSP_FNT_FILM, PSP_FNT_FILM, 0.0f, 0.0f, 0.0f, 0x808080, alp_max, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    break;
    }
    FlashStarYW(flsh + 2, 96, 64, 90, 0);

    for (i = 0; i < 4; i++)
    {
        if (i == csr3)
        {
            rgb = 0x808080;
            alp = (flsh[2].alpha * alp_max) / 128.0f;
        }
        else
        {
            rgb = 0x404040;
            alp = alp_max;
        }

        switch (mode)
        {
        case 0:
            if (i == 1)
            {
                PutSpriteYW(PSP_ICN_ALBUM, PSP_ICN_ALBUM, 75.0f, 0.0f, 0.0f, rgb, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }
            else if (i != 2)
            {
                PutSpriteYW(PSP_ICN_MEMOCA + i, PSP_ICN_MEMOCA + i, 0.0f, 0.0f, 0.0f, rgb, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }
        break;
        case 1:
            if (i == 2)
            {
                PutSpriteYW(PSP_ICN_FILM, PSP_ICN_FILM, 0.0f, 0.0f, 0.0f, 0x303030, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }
            else
            {
                PutSpriteYW(PSP_ICN_MEMOCA + i, PSP_ICN_MEMOCA + i, 0.0f, 0.0f, 0.0f, rgb, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }
        break;
        case 2:
            PutSpriteYW(PSP_ICN_MEMOCA + i, PSP_ICN_MEMOCA + i, 0.0f, 0.0f, 0.0f, rgb, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        break;
        }
    }

    switch (mode)
    {
    case 0:
        csr = csr_x0[csr3];
    break;
    case 1:
        csr = csr_x1[csr3];
    break;
    case 2:
        csr = csr_x2[csr3];
    break;
    }

    PutSpriteYW(PSP_KAKKO_LU, PSP_KAKKO_LU, csr, 0.0f, 0.0f, 0x808080, alp_max, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PSP_KAKKO_LD, PSP_KAKKO_LD, csr, 0.0f, 0.0f, 0x808080, alp_max, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PSP_KAKKO_RU, PSP_KAKKO_RU, csr, 0.0f, 0.0f, 0x808080, alp_max, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PSP_KAKKO_RD, PSP_KAKKO_RD, csr, 0.0f, 0.0f, 0x808080, alp_max, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    DrawMessageBox(0x19000, 22.0f, 308.0f, 596.0f, 118.0f, alp_max);

    if (dsp_svp.slct_mode != -1)
    {
#ifdef BUILD_EU_VERSION
        YesNoCrslOKR(0x14000, csr4 * 132 + 178, 372.0f, 0x808080, alp_max, 2.0f);
#else
        YesNoCrslOKR(0x14000, csr4 * 132 + 178, 370.0f, 0x808080, alp_max, 2.0f);
#endif

        if (dsp_svp.str_no == 3 && msn == 2)
        {
            if (ingame_wrk.game == 1)
            {
                PutStringYW(9, 21, 320, 333, 0x808080, alp_max, 0xf000, 1);
            }
            else
            {
                PutStringYW(9, 7, 320, 333, 0x808080, alp_max, 0xf000, 1);
            }
        }
        else
        {
            PutStringYW(9, dsp_svp.str_no + 3, 320, 333, 0x808080, alp_max, 0xf000, 1);
        }
        PutStringYW(9, 1, 48, 375, 0x808080, alp_max, 0xf000, 0);
    }
    else
    {
        switch (spmenu_wrk.csr[3])
        {
        case 0:
            PutStringYW(9, 8, 320, 333, 0x808080, alp_max, 0xf000, 1);
            PutStringYW(9, 16, 320, 359, 0x808080, alp_max, 0xf000, 1);
            PutStringYW(9, 17, 320, 385, 0x808080, alp_max, 0xf000, 1);
        break;
        case 1:
            PutStringYW(9, 9, 320, 333, 0x808080, alp_max, 0xf000, 1);
            PutStringYW(9, 18, 320, 359, 0x808080, alp_max, 0xf000, 1);
            PutStringYW(9, 19, 320, 385, 0x808080, alp_max, 0xf000, 1);
        break;
        case 2:
            PutStringYW(9, 10, 320, 333, 0x808080, alp_max, 0xf000, 1);
            PutStringYW(9, 20, 320, 359, 0x808080, alp_max, 0xf000, 1);
        break;
        case 3:
            if (msn == 2)
            {
                if (ingame_wrk.game == 1)
                {
                    PutStringYW(9, 22, 320, 333, 0x808080, alp_max, 0xf000, 1);
                }
                else
                {
                    PutStringYW(9, 12, 320, 333, 0x808080, alp_max, 0xf000, 1);
                }
            } else {
                PutStringYW(9, 11, 320, 333, 0x808080, alp_max, 0xf000, 1);
            }
        break;
        }
    }

    SetSprFile(0x1ce0000);

    DispCaption(2, alp_max);
}

SAVE_POINT_MENU_WRK spmenu_wrk = {0};

void SavePointMenuAlbum()
{
    return;
}

void SavePointMenuSave()
{
    if (sav_con.step == 0)
    {
        return;
    }

    PlyrSpotLightOnChk();
    PlyrSpotMoveCtrl();

    if (SaveConte() != 0)
    {
#ifdef BUILD_EU_VERSION
        sp_load_id = LoadReqLanguage(PL_PLDT_E_PK2, LOAD_ADDRESS_4);
        sp_load_id = LoadReqLanguage(PL_MTOP_E_PK2, LOAD_ADDRESS_2);
#else
        sp_load_id = LoadReq(PL_PLDT_PK2, LOAD_ADDRESS_4);
        sp_load_id = LoadReq(PL_MTOP_PK2, LOAD_ADDRESS_2);
#endif

        spmenu_wrk.mode = SPMODE_SAVE_OUT;
    }
}

static u_char SaveConte()
{
    switch(sav_con.step)
    {
    case CNT_START:
        sv_dsp_sw.fdt = 0xff;
        sv_dsp_sw.alt = 0xff;
        sv_dsp_sw.msg = 0xff;
        sv_dsp_sw.msk = 0xff;
        sv_dsp_sw.bck = 0xff;

        sv_dsp_sw.fdt_alp = 0;
        sv_dsp_sw.alt_alp = 0;
        sv_dsp_sw.svp_alp = 0;
        sv_dsp_sw.msg_alp = 0;
        sv_dsp_sw.msk_alp = 0;

        spmenu_wrk.csr[0] = spmenu_wrk.csr[2] = 0xff;
        spmenu_wrk.csr[1] = 0;

        if (sav_con.dmy_tmr == 0)
        {
            sv_dsp_sw.bck = 1;
            sv_dsp_sw.svp = 1;
            sv_dsp_sw.msk = 1;

            sav_con.step = CNT_HALF_MASK;
        }
        else
        {
            sav_con.dmy_tmr--;
        }
    break;
    case CNT_HALF_MASK:
        if (sv_dsp_sw.msk_alp == 0x1900)
        {
            sav_con.dmy_tmr = 60;

            sav_con.step = CNT_FILE_CHECK;
        }
    break;
    case CNT_FILE_CHECK:
        if (sav_con.dmy_tmr == 0)
        {
            sv_dsp_sw.alt = 0xff;

            spmenu_wrk.csr[1] = sav_con.slot;

            sav_con.step = CNT_SLOT_SELECT;
        }
        else
        {
            sv_dsp_sw.alt = 1;

            sav_con.dmy_tmr--;
        }
    break;
    case CNT_SLOT_SELECT:
        sv_dsp_sw.msg = 0;

        if (*key_now[4] == 1)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            sv_dsp_sw.msk = 0xff;
            sv_dsp_sw.svp = 0xff;
            sv_dsp_sw.msg = 0xff;

            sav_con.step = CNT_BREAK;
        }
        else if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);

            sav_con.slot = spmenu_wrk.csr[1];
            spmenu_wrk.csr[2] = sav_con.file;

            sav_con.step = CNT_FILE_SELECT;
        }
        else if (
            *key_now[1] == 1 ||
            (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
            Ana2PadDirCnt(2) == 1 ||
            (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
        )
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            if (spmenu_wrk.csr[1] < 2)
            {
                spmenu_wrk.csr[1]++;
            }
            else
            {
                spmenu_wrk.csr[1] = 0;
            }
        }
        else if (
            *key_now[0] == 1 ||
            (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
            Ana2PadDirCnt(0) == 1 ||
            (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
        )
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            if (spmenu_wrk.csr[1] > 0)
            {
                spmenu_wrk.csr[1]--;
            }
            else
            {
                spmenu_wrk.csr[1] = 2;
            }
        }
    break;
    case CNT_FILE_SELECT:
        sv_dsp_sw.msg = 1;

        if (*key_now[4] == 1)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            spmenu_wrk.csr[1] = sav_con.slot;
            spmenu_wrk.csr[2] = 0xff;

            sav_con.step = CNT_SLOT_SELECT;
        }
        else if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            sav_con.file = spmenu_wrk.csr[2];

            sv_dsp_sw.msg = 0xff;

            spmenu_wrk.csr[0] = 1;

            sav_con.step = CNT_FINAL_ANSWER;

            SeStartFix(1, 0, 0x1000, 0x1000, 0);
        }
        else if (
            *key_now[1] == 1 ||
            (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
            Ana2PadDirCnt(2) == 1 ||
            (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
        )
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            if (spmenu_wrk.csr[2] < 2)
            {
                spmenu_wrk.csr[2]++;
            }
            else
            {
                spmenu_wrk.csr[2] = 0;
            }
        }
        else if (
            *key_now[0] == 1 ||
            (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
            Ana2PadDirCnt(0) == 1 ||
            (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
        )
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            if (spmenu_wrk.csr[2] > 0)
            {
                spmenu_wrk.csr[2]--;
            }
            else
            {
                spmenu_wrk.csr[2] = 2;
            }
        }
    break;
    case CNT_FINAL_ANSWER:
        sv_dsp_sw.msg = 18;

        if (*key_now[4] == 1)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            spmenu_wrk.csr[0] = 0xff;
            spmenu_wrk.csr[2] = sav_con.file;

            sav_con.step = CNT_FILE_SELECT;
        }
        else if ((*key_now[6] == 1) || (*key_now[5] == 1))
        {
            if (spmenu_wrk.csr[0] == 0)
            {
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

                sav_con.sv_ctrl = 1;

                SavePCFile();

                sav_con.step = CNT_WAIT_SAVE_END;
            }
            else
            {
                SeStartFix(3, 0, 0x1000, 0x1000, 0);

                sv_dsp_sw.msk = 0xff;
                sv_dsp_sw.msg = 0xff;
                sv_dsp_sw.svp = 0xff;

                sav_con.step = CNT_BREAK;
            }
        }
        else if (*key_now[3] == 1 || Ana2PadDirCnt(1) == 1)
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            spmenu_wrk.csr[0] = 1;
        }
        else if (*key_now[2] == 1 || Ana2PadDirCnt(3) == 1)
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            spmenu_wrk.csr[0] = 0;
        }
    break;
    case CNT_WAIT_SAVE_END:
        sv_dsp_sw.msk = 0xff;
        sv_dsp_sw.msg = 0xff;
        sv_dsp_sw.svp = 0xff;

        sav_con.step = CNT_BREAK;
    break;
    case CNT_BREAK:
        spmenu_wrk.csr[0] = spmenu_wrk.csr[1] = spmenu_wrk.csr[2] = 0xff;
        spmenu_wrk.csr[3] = 0;
        spmenu_wrk.csr[4] = 1;

        sav_con.step = CNT_END;
    break;
    case CNT_END:
        sv_dsp_sw.bck = 0xff;

        sav_con.step = CNT_STTS_IDOL;

        SendManMdlTex();

        return 1;
    break;
    }

    BgFusumaYW(0x606060, 0.0f, 128.0f, 0x7d000);
    DspMemSavePoint(sv_dsp_sw.msk, sv_dsp_sw.msg, sv_dsp_sw.svp, sv_dsp_sw.fdt, sv_dsp_sw.alt, spmenu_wrk.csr[0], spmenu_wrk.csr[1], spmenu_wrk.csr[2], 0, 0x80, 0);

    return 0;
}

void DspMemSavePoint(u_char msk, u_char msg, u_char fdt, u_char svp, u_char alt, u_char csr0, u_char csr1, u_char csr2, u_char yes_no, u_char alp_max, u_char type)
{
    int i;
    char num;
    u_char flg;
    u_short msk_add;
    u_short fdt_add;
    u_short svp_add;
    u_char alpha;

    flg = 0;

    msk_add = (alp_max / 64) * 100;

    if (msk != 0xff)
    {
        if (sv_dsp_sw.msk_alp + msk_add >= alp_max * 50)
        {
            sv_dsp_sw.msk_alp = alp_max * 50;
        }
        else
        {
            sv_dsp_sw.msk_alp += msk_add;
        }
    }
    else
    {
        if (sv_dsp_sw.msk_alp - msk_add > 0)
        {
            sv_dsp_sw.msk_alp -= msk_add;
        }
        else
        {
            sv_dsp_sw.msk_alp = 0;
        }
    }

    if (sv_dsp_sw.msk_alp != 0)
    {
        PolySquareYW(0.0f, 0.0f, 640, 448, 0, sv_dsp_sw.msk_alp / 100.0f, 1.0f, 1.0f, 0x7d000, 0, 0, 0);
    }

    svp_add = (alp_max / 32) * 100;

    if (svp != 0xff)
    {
        if (sv_dsp_sw.svp_alp + svp_add >= alp_max * 100)
        {
            sv_dsp_sw.svp_alp = alp_max * 100;
        }
        else
        {
            sv_dsp_sw.svp_alp += svp_add;
        }
    }
    else
    {
        if (sv_dsp_sw.svp_alp - svp_add > 0)
        {
            sv_dsp_sw.svp_alp -= svp_add;
        }
        else
        {
            sv_dsp_sw.svp_alp = 0;
        }
    }

    if (sv_dsp_sw.svp_alp != 0)
    {
        DspSavePoint(csr1, csr2, sv_dsp_sw.svp_alp / 100.0f, type);
    }

    fdt_add = (alp_max / 32) * 100;

    if (fdt != 0xff)
    {
        if (sv_dsp_sw.fdt_alp + fdt_add < alp_max * 100)
        {
            sv_dsp_sw.fdt_alp += fdt_add;
        }
        else
        {
            sv_dsp_sw.fdt_alp = alp_max * 100;
        }
    }
    else
    {
        if (sv_dsp_sw.fdt_alp - fdt_add  > 0)
        {
            sv_dsp_sw.fdt_alp -= fdt_add;
        }
        else
        {
            sv_dsp_sw.fdt_alp = 0;
        }

        if (sv_dsp_sw.fdt_alp > alp_max * 100)
        {
            sv_dsp_sw.fdt_alp = alp_max * 100;
        }
    }

    if (sv_dsp_sw.fdt_alp != 0)
    {
        if (type == 0 || type == 1)
        {
            if (mc_ctrl.mode == 0 || mc_ctrl.mode == 2)
            {
                num = mc_ctrl.dir.gamefile_num;

                if (++num > 3)
                {
                    num = 3;
                }
            }
            else
            {
                num = mc_ctrl.dir.gamefile_num;
            }

            for (i = 0; i < num; i++)
            {
                DspSpInfo(i, i * 98, sv_dsp_sw.fdt_alp / 100.0f, type);
            }
        }
        else
        {
            num = mc_ctrl.album.file_num;

            if (num < 5)
            {
                if (mc_ctrl.mode == 0 || mc_ctrl.mode == 2)
                {
                    if (mcCheckEmpty(0) == 0)
                    {
                        num++;
                    }
                }
            }

            for (i = 0; i < num; i++)
            {
                DspSpInfo2(i, i * 59, sv_dsp_sw.fdt_alp / 100.0f, type);
            }
        }

        flg = 1;
    }

    if (sv_dsp_sw.svp_alp != 0 && (type != 0 && type != 1))
    {
        for (i = 0; i < 5; i++)
        {
            PutSpriteYW(OA_LD_SML_RNZ, OA_LD_SML_RNZ, 0.0f, i * 59, 0.0f, 0x808080, (u_char)(sv_dsp_sw.svp_alp / 100.0f), 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }
    }

    if (flg)
    {
        alpha = sv_dsp_sw.fdt_alp / 100.0f;

        if (alpha > 0x50)
        {
            alpha = 0x50;
        }

        if (type == 0 || type == 1)
        {
            for (i = num; i < 3; i++)
            {
                PolySquareYW(209.0f, i * 98 + 33, 376, 96, 0, alpha, 1.0f, 1.0f, 0xa000, 0, 0, 0);
            }
        }
        else
        {
            for (i = num; i < 5; i++)
            {
                PolySquareYW(209.0f, i * 59 + 33, 376, 54, 0, alpha, 1.0f, 1.0f, 0xa000, 0, 0, 0);
            }
        }
    }

    if (msg != 0xff)
    {
        sv_dsp_sw.msg_alp = alp_max * 100;
    }
    else
    {
        sv_dsp_sw.msg_alp = 0;
    }

    if (sv_dsp_sw.msg_alp != 0 && msg != 0xff && mc_ctrl.step == mc_ctrl.stepbak)
    {
        if (csr0 != 0xff)
        {
            if (mc_ctrl.step == 2)
            {
#ifdef BUILD_EU_VERSION
                YesNoCrslOKR(0x14000, csr0 * 132 + 114, 379.0f, 0x808080, sv_dsp_sw.msg_alp / 100.0f, 2.0f);
#else
                YesNoCrslOKR(0x14000, csr0 * 132 + 114, 378.0f, 0x808080, sv_dsp_sw.msg_alp / 100.0f, 2.0f);
#endif
            }
            else
            {
#ifdef BUILD_EU_VERSION
                YesNoCrslOKR(0x14000, csr0 * 132 + 178, 379.0f, 0x808080, sv_dsp_sw.msg_alp / 100.0f, 2.0f);
#else
                YesNoCrslOKR(0x14000, csr0 * 132 + 178, 376.0f, 0x808080, sv_dsp_sw.msg_alp / 100.0f, 2.0f);
#endif
            }
        }

        if (mc_ctrl.mode == 2 && mc_ctrl.step == 6)
        {
            if (mc_ctrl.album.sta[mc_ctrl.now_cur] == 3)
            {
#ifdef BUILD_EU_VERSION
                u_char alpha = sv_dsp_sw.fdt_alp / 100.0f;

                if (alpha > 0x50)
                {
                    alpha = 0x50;
                }

                PolySquareYW(35.0f, 198.0f, 570, 144, 0, alpha, 1.0f, 1.0f, 0x4000, 0, 0, 0);
                DrawMessageBox(0x3000, 26.0f, 195.0f, 585.0f, 149.0f, 0x80);
                PutStringYW(8, 33, 48, 208, 0x808080, 0x80, 0x2000, 0);
#else
                PutStringYW(8, 33, 48, 356, 0x808080, 0x80, 0xf000, 0);
#endif
            }
            else
            {
                PutStringYW(8, msg, 48, 356, 0x808080, sv_dsp_sw.msg_alp / 100.0f, 0xf000, 0);

                if (mcCheckEmpty(0) != 0 && mc_ctrl.album.file_num < 5)
                {
                    PutStringYW(8, 40, 38, 382, 0x808080, 0x80, 0xf000, 0);
                }
            }
        }
        else
        {
            PutStringYW(8, msg, 48, 356, 0x808080, sv_dsp_sw.msg_alp / 100.0f, 0xf000, 0);
        }

        if (mcCheckDrawButton(msg) != 0)
        {
            alpha = (SgSinf((btn_flsh_cnt * (PI * 2)) / 60.0f) + 1.0f) * 128.0f * 0.5f;

            DrawButtonTex(0xf000, 3, 552.0f, 380.0f, alpha);

            if (btn_flsh_cnt < 60)
            {
                btn_flsh_cnt++;
            }
            else
            {
                btn_flsh_cnt = 0;
            }
        }
    }

    if (alt != 0xff)
    {
        sv_dsp_sw.alt_alp = alp_max * 100;
    }
    else
    {
        sv_dsp_sw.alt_alp = 0;
    }

    if (yes_no != 0xff)
    {
        sv_dsp_sw.yno_alp = alp_max * 100;
    }
    else
    {
        sv_dsp_sw.yno_alp = 0;
    }

    WarningMsg(alt, csr0, sv_dsp_sw.alt_alp / 100.0f, sv_dsp_sw.yno_alp / 100.0f);

    SetSprFile(0x1ce0000);

    DispCaption(9, alp_max);
}

static void WarningMsg(u_char msg, u_char csr0, u_char alp, u_char ha)
{
    float pos_y;
    u_char alpha;

    if (alp == 0)
    {
        if (sv_dsp_sw.alt_bak_alp - 8 > 0)
        {
            sv_dsp_sw.alt_bak_alp -= 8;
        }
        else
        {
            sv_dsp_sw.alt_bak_alp = 0;
        }

        PolySquareYW(0.0f, 0.0f, 640, 448, 0, sv_dsp_sw.alt_bak_alp, 1.0f, 1.0f, 0x5000, 0, 0, 0);
    }
    else
    {
        if (sv_dsp_sw.alt_bak_alp + 8 < 96)
        {
            sv_dsp_sw.alt_bak_alp += 8;
        }
        else
        {
            sv_dsp_sw.alt_bak_alp = 96;
        }

        PolySquareYW(0.0f, 0.0f, 640, 448, 0, sv_dsp_sw.alt_bak_alp, 1.0f, 1.0f, 0x5000, 0, 0, 0);
#ifdef BUILD_EU_VERSION
        PolySquareYW(35.0f, 198.0f, 570, 144, 0, alp, 1.0f, 1.0f, 0x4000, 0, 0, 0);
        DrawMessageBox(0x3000, 26.0f, 195.0f, 585.0f, 149.0f, alp);
#else
        PolySquareYW(35.0f, 198.0f, 570, 120, 0, alp, 1.0f, 1.0f, 0x4000, 0, 0, 0);
        DrawMessageBox(0x3000, 26.0f, 195.0f, 585.0f, 125.0f, alp);
#endif

        if (msg == 12)
        {
#ifdef BUILD_EU_VERSION
            pos_y = 303.0f;
#else
            pos_y = 281.0f;
#endif
        }
        else if (msg == 37)
        {
            pos_y = 229.0f;
        }
        else if (msg == 39)
        {
            pos_y = 229.0f;
        }
#ifdef BUILD_EU_VERSION
        else if (msg == 16)
        {
            pos_y = 303.0f;
        }
#endif
        else
        {
            pos_y = 255.0f;
        }

        if (mc_ctrl.step == 2)
        {
#ifdef BUILD_EU_VERSION
            YesNoCrslOKR(0x2000, csr0 * 132 + 114, pos_y + 3.0f, 0x808080, ha, 2.0f);
#else
            YesNoCrslOKR(0x2000, csr0 * 132 + 114, pos_y + 1.0f, 0x808080, ha, 2.0f);
#endif
        }
        else
        {
#ifdef BUILD_EU_VERSION
            YesNoCrslOKR(0x2000, csr0 * 132 + 184, pos_y - 1.0f, 0x808080, ha, 2.0f);
#else
            YesNoCrslOKR(0x2000, csr0 * 132 + 184, pos_y + 1.0f, 0x808080, ha, 2.0f);
#endif
        }

        if (msg != 0xff)
        {
#ifdef BUILD_EU_VERSION
            PutStringYW(8, msg, 48, 209, 0x808080, alp, 0x1000, 0);
#else
            PutStringYW(8, msg, 48, 211, 0x808080, alp, 0x1000, 0);
#endif

            if (mcCheckDrawButton(msg) != 0)
            {
                alpha = (SgSinf((btn_flsh_cnt * (PI * 2)) / 60.0f) + 1.0f) * 128.0f * 0.5f;

#ifdef BUILD_EU_VERSION
                DrawButtonTex(0x2000, 3, 552.0f, 304.0f, alpha);
#else
                DrawButtonTex(0x2000, 3, 552.0f, 280.0f, alpha);
#endif

                if (btn_flsh_cnt < 60)
                {
                    btn_flsh_cnt++;
                }
                else
                {
                    btn_flsh_cnt = 0;
                }
            }
        }
    }
}

static void GetSvpNo(u_char *no)
{
    int i;
    u_char svp_no;
    u_char room_from;
    u_char room_no;

    room_no = GetPointRoomNoFloor(plyr_wrk.move_box.pos[0], plyr_wrk.move_box.pos[2], map_wrk.floor);
    room_from = GetRoomIdFromRoomNoFloor(0, room_no, map_wrk.floor);

    svp_no = 0xff;

    for (i = 0; i < 7; i++)
    {
        if (map_svp_pos[i].room == room_from)
        {
            svp_no = i;
        }
    }

    *no = svp_no;
}

char SaveCamChk()
{
    return 0;
}

static void DspSavePoint(u_char csr1, u_char csr2, u_char alp, u_char type)
{
    FlashStarYW(&flsh[1], 96, 64, 90, 0);
    DspSpLenz(csr1, alp);
    DspSpData(csr2, alp, type);
    DspSpCate(csr1, csr2, alp);
    SpMsgWin(alp);
    DspSaveTitle(type, 0, 0, alp, 5);
}

static void DspSpLenz(u_char dsp_no, u_char alp)
{
    float dec;

    dec = 64.0f / 15.0f;

    switch (dsp_no + 1)
    {
    case 1:
        if (mrcl_lnz.slot1_alp + dec < alp)
        {
            mrcl_lnz.slot1_alp += dec;
        }
        else
        {
            mrcl_lnz.slot1_alp = alp;
        }

        if (mrcl_lnz.slot2_alp - dec > 0)
        {
            mrcl_lnz.slot2_alp -= dec;
        }
        else
        {
            mrcl_lnz.slot2_alp = 0.0f;
        }

        if (mrcl_lnz.slot3_alp - dec > 0)
        {
            mrcl_lnz.slot3_alp -= dec;
        }
        else
        {
            mrcl_lnz.slot3_alp = 0.0f;
        }
    break;
    case 2:
        if (mrcl_lnz.slot1_alp - dec > 0)
        {
            mrcl_lnz.slot1_alp -= dec;
        }
        else
        {
            mrcl_lnz.slot1_alp = 0.0f;
        }

        if (mrcl_lnz.slot2_alp + dec < alp)
        {
            mrcl_lnz.slot2_alp += dec;
        }
        else
        {
            mrcl_lnz.slot2_alp = alp;
        }

        if (mrcl_lnz.slot3_alp - dec > 0)
        {
            mrcl_lnz.slot3_alp -= dec;
        }
        else
        {
            mrcl_lnz.slot3_alp = 0.0f;
        }
    break;
    case 3:
        if (mrcl_lnz.slot1_alp - dec > 0)
        {
            mrcl_lnz.slot1_alp -= dec;
        }
        else
        {
            mrcl_lnz.slot1_alp = 0.0f;
        }

        if (mrcl_lnz.slot2_alp - dec > 0)
        {
            mrcl_lnz.slot2_alp -= dec;
        }
        else
        {
            mrcl_lnz.slot2_alp = 0.0f;
        }

        if (mrcl_lnz.slot3_alp + dec < alp)
        {
            mrcl_lnz.slot3_alp += dec;
        }
        else
        {
            mrcl_lnz.slot3_alp = alp;
        }
    break;
    default:
        if (mrcl_lnz.slot1_alp - dec > 0)
        {
            mrcl_lnz.slot1_alp -= dec;
        }
        else
        {
            mrcl_lnz.slot1_alp = 0.0f;
        }

        if (mrcl_lnz.slot2_alp - dec > 0)
        {
            mrcl_lnz.slot2_alp -= dec;
        }
        else
        {
            mrcl_lnz.slot2_alp = 0.0f;
        }

        if (mrcl_lnz.slot3_alp - dec > 0)
        {
            mrcl_lnz.slot3_alp -= dec;
        }
        else
        {
            mrcl_lnz.slot3_alp = 0.0f;
        }
    break;
    }

    PutSpriteYW(SV_LNZ_NO1, SV_LNZ_NO1, 79.0f, 138.0f, 0.0f, 0x808080, mrcl_lnz.slot1_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(SV_LNZ_NO2, SV_LNZ_NO2, 75.0f, 138.0f, 0.0f, 0x808080, mrcl_lnz.slot2_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PSP_ICN_MEMOCA, PSP_ICN_MEMOCA, -6.0f, -37.0f, 0.0f, 0x808080, mrcl_lnz.slot1_alp, 0.85f, 0.85f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PSP_ICN_MEMOCA, PSP_ICN_MEMOCA, -6.0f, -37.0f, 0.0f, 0x808080, mrcl_lnz.slot2_alp, 0.85f, 0.85f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(PSP_END, PSP_END, -441.0f, -41.0f, 0.0f, 0x808080, mrcl_lnz.slot3_alp, 0.85f, 0.85f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(SV_LNZ_LNZ, SV_LNZ_LNZ, 22.0f, 77.0f, 0.0f, 0x808080, alp, 0.85f, 0.85f, 0, 0xff, 1, 0, 0);
}

static void DspSpCate(u_char csr1, u_char csr2, u_char alp)
{
    WakuWaku(0xfff0, 186, alp, 3, 30);

    PutSpriteYW(SV_LNZ_SLT1, SV_LNZ_SLT1, 36.0f, 218.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(SV_LNZ_SLT2, SV_LNZ_SLT2, 36.0f, 256.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(SV_LNZ_HDD3, SV_LNZ_HDD3, 36.0f, 295.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    if (csr2 == 0xff)
    {
        CmnCursol(33, csr1 * 38 + 218, 122, 30, flsh[1].alpha, alp, 0xa000);
    }
    else
    {
        CmnCursol(33, csr1 * 38 + 218, 122, 30, 64.0f, alp, 0xa000);
    }
}

#ifdef BUILD_EU_VERSION
#ifdef MATCHING_DECOMP
static void DspSpData(u_char csr2, u_char alp, u_char type)
{
    int i;
    float xx1;
    float xx2;
    float yy1;
    float yy2;
    u_short pos_y;
    float csr_alp;

    xx1 = 462.0f;
    xx2 = 569.0f;
    yy1 = 41.0f;
    yy2 = 119.0f;

    if (type == 0 || type == 1) {
        if (csr2 != 0xff) {
            pos_y = csr2 * 0x62 + 0x23;
            csr_alp = (flsh[1].alpha * alp) / 128.0f;
        } else {
            pos_y = 0x23;
        }{ register int j asm("v1");
        for (i = 0; (j = i < 3, j); i++) {
            if (1) {
                SQAR_DAT sq = { .w = 640, .h = 448, .x = 0, .y = 0, .pri = 0, .r = 0, .g = 0, .b = 0, .alpha = 0x80, };
                DISP_SQAR dq;
                register u_long z asm("a1"); register u_long t asm("a2");
                CopySqrDToSqr(&dq, &sq);
                { register u_int prim asm("a0"); u_int x = 0x0fffffff; prim = 0x1000; dq.pri = prim; dq.z = x -= 0x1000; }
                z = SCE_GS_SET_ZBUF_1(0x2300 / 64, SCE_GS_PSMCT24, 0);
                t = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS); {dq.zbuf = z; dq.test = t;}
                dq.x[2] = dq.x[0] = xx1; dq.y[1] = dq.y[0] = yy1 + (i * 0x62);
                dq.x[3] = dq.x[1] = dq.x[0] + (xx2 - xx1);
                dq.y[3] = dq.y[2] = dq.y[0] + (yy2 - yy1);
                {register int i asm("a1"); for (i = 0; i < 4; i++) { dq.r[i] = 0; dq.g[i] = 0; dq.b[i] = 0; }}
                dq.alpha = 1;
                DispSqrD(&dq);
            }
        }}
        if (csr_alp != 0.0f) {
            PutSpriteYW(0x266, 0x269, 217.0f, pos_y, 0.0f, 0x808080, csr_alp, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
            PutSpriteYW(0x26a, 0x26b, 217.0f, pos_y, 0.0f, 0x808080, csr_alp, 6.869565f, 1.0f, 0, 0xff, 1, 1, 0);
        }{ register int j asm("v1");
        for (i = 0; (j = i < 3, j); i++) {
            if (1) {
                SQAR_DAT sq = { .w = 640, .h = 448, .x = 0, .y = 0, .pri = 0, .r = 0, .g = 0, .b = 0, .alpha = 0x80, };
                DISP_SQAR dq;
                register u_long z asm("a1"); register u_long t asm("a2");
                CopySqrDToSqr(&dq, &sq);
                dq.pri = 0x64000; dq.z = 0x0fffffff - dq.pri;
                z = SCE_GS_SET_ZBUF_1(0x2300 / 64, SCE_GS_PSMCT24, 0);
                t = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS); {dq.zbuf = z; dq.test = t;}
                dq.x[2] = dq.x[0] = xx1; dq.y[1] = dq.y[0] = yy1 + (i * 0x62);
                dq.x[3] = dq.x[1] = dq.x[0] + (xx2 - xx1);
                dq.y[3] = dq.y[2] = dq.y[0] + (yy2 - yy1);
                {int i; for (i = 0; i < 4; i++) { dq.r[i] = 0; dq.g[i] = 0; dq.b[i] = 0; }}
                dq.alpha = 1;
                DispSqrD(&dq);
            }
        }}
    } else {
        if (csr2 != 0xff) {
            pos_y = csr2 * 0x3b;
            csr_alp = (flsh[1].alpha * alp) / 128.0f;
        } else {
            pos_y = 0;
        }
        if (csr_alp != 0.0f) {
            PutSpriteYW(0x2d0, 0x2d0, 0.0f, pos_y, 0.0f, 0x808080, csr_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(0x2d1, 0x2d1, 0.0f, pos_y, 0.0f, 0x808080, csr_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }
    }


    CmnWindow(18, 0, 0, alp, 0x80);

    SpSideBar(0, 0, alp);


    if (type == 0 || type == 1) {
        for (i = 0; i < 3; i++) {
            DspSpFile(i, i * 98, alp, type);
        }
    } else {
        for (i = 0; i < 5; i++) {
            DspSpFile2(i, i * 59, alp, type);
        }
    }
}
#else
static void DspSpData(u_char csr2, u_char alp, u_char type)
{
    int i;
    float xx1;
    float xx2;
    float yy1;
    float yy2;
    u_short pos_y;
    float csr_alp;

    xx1 = 462.0f;
    xx2 = 569.0f;
    yy1 = 41.0f;
    yy2 = 119.0f;

    if (type == 0 || type == 1)
    {
        if (csr2 != 0xff)
        {
            pos_y = csr2 * 0x62 + 0x23;
            csr_alp = (flsh[1].alpha * alp) / 128.0f;
        }
        else
        {
            pos_y = 0x23;
        }

        for (i = 0; i < 3; i++)
        {
            if (1)
            {
                SQAR_DAT sq = { .w = 640, .h = 448, .x = 0, .y = 0, .pri = 0, .r = 0, .g = 0, .b = 0, .alpha = 0x80, };
                DISP_SQAR dq;

                CopySqrDToSqr(&dq, &sq);
                dq.pri = 0x1000; dq.z = 0x0fffffff - dq.pri;
                dq.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
                dq.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS);
                dq.x[2] = dq.x[0] = xx1; dq.y[1] = dq.y[0] = yy1 + (i * 0x62);
                dq.x[3] = dq.x[1] = dq.x[0] + (xx2 - xx1);
                dq.y[3] = dq.y[2] = dq.y[0] + (yy2 - yy1);
                {int i; for (i = 0; i < 4; i++) { dq.r[i] = 0; dq.g[i] = 0; dq.b[i] = 0; }}
                dq.alpha = 1;
                DispSqrD(&dq);
            }
        }

        if (csr_alp != 0.0f)
        {
            PutSpriteYW(0x266, 0x269, 217.0f, pos_y, 0.0f, 0x808080, csr_alp, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
            PutSpriteYW(0x26a, 0x26b, 217.0f, pos_y, 0.0f, 0x808080, csr_alp, 6.869565f, 1.0f, 0, 0xff, 1, 1, 0);
        }

        for (i = 0; i < 3; i++)
        {
            if (1)
            {
                SQAR_DAT sq = { .w = 640, .h = 448, .x = 0, .y = 0, .pri = 0, .r = 0, .g = 0, .b = 0, .alpha = 0x80, };
                DISP_SQAR dq;

                CopySqrDToSqr(&dq, &sq);
                dq.pri = 0x64000; dq.z = 0x0fffffff - dq.pri;
                dq.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
                dq.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS);
                dq.x[2] = dq.x[0] = xx1; dq.y[1] = dq.y[0] = yy1 + (i * 0x62);
                dq.x[3] = dq.x[1] = dq.x[0] + (xx2 - xx1);
                dq.y[3] = dq.y[2] = dq.y[0] + (yy2 - yy1);
                {int i; for (i = 0; i < 4; i++) { dq.r[i] = 0; dq.g[i] = 0; dq.b[i] = 0; }}
                dq.alpha = 1;
                DispSqrD(&dq);
            }
        }
    }
    else
    {
        if (csr2 != 0xff)
        {
            pos_y = csr2 * 0x3b;
            csr_alp = (flsh[1].alpha * alp) / 128.0f;
        }
        else
        {
            pos_y = 0;
        }

        if (csr_alp != 0.0f)
        {
            PutSpriteYW(0x2d0, 0x2d0, 0.0f, pos_y, 0.0f, 0x808080, csr_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(0x2d1, 0x2d1, 0.0f, pos_y, 0.0f, 0x808080, csr_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }
    }

    CmnWindow(18, 0, 0, alp, 0x80);

    SpSideBar(0, 0, alp);

    if (type == 0 || type == 1)
    {
        for (i = 0; i < 3; i++)
        {
            DspSpFile(i, i * 98, alp, type);
        }
    }
    else
    {
        for (i = 0; i < 5; i++)
        {
            DspSpFile2(i, i * 59, alp, type);
        }
    }
}
#endif
#else
static void DspSpData(u_char csr2, u_char alp, u_char type)
{
    int i;
    u_short pos_y;
    float csr_alp;

    CmnWindow(18, 0, 0, alp, 0x80);
    SpSideBar(0, 0, alp);

    if (type == 0 || type == 1)
    {
        for (i = 0; i < 3; i++)
        {
            DspSpFile(i, i * 98, alp, type);
        }
    }
    else
    {
        for (i = 0; i < 5; i++)
        {
            DspSpFile2(i, i * 59, alp, type);
        }
    }

    if (type == 0 || type == 1)
    {
        if (csr2 != 0xff)
        {
            pos_y = csr2 * 98 + 35;
            csr_alp = (flsh[1].alpha * alp) / 128.0f;
        }
        else
        {
            pos_y = 35;
        }

        if (csr_alp != 0.0f)
        {
            PutSpriteYW(SV_FSL_V01, SV_FSL_V04, 217.0f, pos_y, 0.0f, 0x808080, csr_alp, 1.0f, 1.0f, 0, 0xff, 1, 1, 0);
            PutSpriteYW(SV_FSL_H01, SV_FSL_H02, 217.0f, pos_y, 0.0f, 0x808080, csr_alp, 6.869565f, 1.0f, 0, 0xff, 1, 1, 0);
        }
    }
    else
    {
        if (csr2 != 0xff)
        {
            pos_y = csr2 * 59;
            csr_alp = (flsh[1].alpha * alp) / 128.0f;
        }
        else
        {
            pos_y = 0;
        }

        if (csr_alp != 0.0f)
        {
            PutSpriteYW(OA_LD_CSR_L, OA_LD_CSR_L, 0.0f, pos_y, 0.0f, 0x808080, csr_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(OA_LD_CSR_R, OA_LD_CSR_R, 0.0f, pos_y, 0.0f, 0x808080, csr_alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }
    }
}
#endif

static void DspSpFile(u_char file_no, u_char pos_y, u_char alp, u_char type)
{
    CmnWindow(19, 0, pos_y, alp, 0x80);

    PutSpriteYW(SV_LNZ_FILE1, SV_LNZ_FILE1, 206.0f, pos_y + 78, -90.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
}

static void DspSpInfo(u_char file_no, u_char pos_y, u_char alp, u_char type)
{
    u_char end_showzo;
    u_char sv_pht;

    PutNumberYW(6, file_no + 1, 218, pos_y + 50, 1.0f, 1.0f, 0x808080, alp, 0xa000, 2, 0);

    if (mc_header_buf[file_no].game.map_flg == 0)
    {
        PutStringYW(8, 100, 250, pos_y + 70, 0x808080, alp, 0xa000, 0);
    }
    else
    {
        if (mc_header_buf[file_no].game.msn_flg == 0)
        {
            if (mc_header_buf[file_no].game.difficult == 0)
            {
#ifdef BUILD_EU_VERSION
                PutStringYW(8, mc_header_buf[file_no].game.msn_no + 101, 244, pos_y + 44, 0x808080, alp, 0xa000, 0);
#else
                PutStringYW(8, mc_header_buf[file_no].game.msn_no + 101, 250, pos_y + 46, 0x808080, alp, 0xa000, 0);
#endif
            }
            else
            {
#ifdef BUILD_EU_VERSION
                PutStringYW(8, mc_header_buf[file_no].game.msn_no + 101, 244, pos_y + 44, 0x800000, alp, 0xa000, 0);
#else
                PutStringYW(8, mc_header_buf[file_no].game.msn_no + 101, 250, pos_y + 46, 0x800000, alp, 0xa000, 0);
#endif
            }

#ifdef BUILD_EU_VERSION
            PutStringYW(30, mc_header_buf[file_no].game.room_no, 244, pos_y + 66, 0x808080, alp, 0xa000, 0);
#else
            PutStringYW(30, mc_header_buf[file_no].game.room_no, 250, pos_y + 70, 0x808080, alp, 0xa000, 0);
#endif
            PutNumberYW(0, mc_header_buf[file_no].game.hour, 0xff, pos_y + 91, 1.0f, 1.0f, 0x808080, alp, 0xa000, 2, 1);
            PutStringYW(8, 108, 306, pos_y + 91, 0x808080, alp, 0xa000, 0);
            PutNumberYW(0, mc_header_buf[file_no].game.minute, 325, pos_y + 91, 1.0f, 1.0f, 0x808080, alp, 0xa000, 2, 1);
            PutStringYW(8, 108, 376, pos_y + 91, 0x808080, alp, 0xa000, 0);
            PutNumberYW(0, mc_header_buf[file_no].game.sec, 395, pos_y + 91, 1.0f, 1.0f, 0x808080, alp, 0xa000, 2, 1);

            switch(mc_header_buf[file_no].game.room_no)
            {
            case 0:
                sv_pht = 0;
            break;
            case 6:
                sv_pht = 1;
            break;
            case 14:
                sv_pht = 2;
            break;
            case 15:
                sv_pht = 3;
            break;
            case 21:
                sv_pht = 4;
            break;
            case 31:
                sv_pht = 5;
            break;
            case 41:
                sv_pht = 6;
            break;
            default:
                sv_pht = 0xff;
            break;
            }

            if (sv_pht != 0xff)
            {
                SetSprFile(LOAD_ADDRESS_3);

#ifdef BUILD_EU_VERSION
                PutSpriteYW(SP_R000 + sv_pht, SP_R000 + sv_pht, 471.0f, pos_y + 50, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                PutSpriteYW(SV_PHT_V01, SV_PHT_H04, 468.0f, pos_y + 47, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
#else
                PutSpriteYW(SP_R000 + sv_pht, SP_R000 + sv_pht, 459.0f, pos_y + 50, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                PutSpriteYW(SV_PHT_V01, SV_PHT_H04, 456.0f, pos_y + 47, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
#endif
            }
        }
        else
        {
            end_showzo = mc_header_buf[file_no].game.msn_no;

            if (end_showzo > 5)
            {
                end_showzo = 5;
            }

            if (mc_header_buf[file_no].game.difficult == 0)
            {
#ifdef BUILD_EU_VERSION
                PutStringYW(8, end_showzo + 101, 244, pos_y + 44, 0x808080, alp, 0xa000, 0);
#else
                PutStringYW(8, end_showzo + 101, 250, pos_y + 46, 0x808080, alp, 0xa000, 0);
#endif
            }
            else
            {
#ifdef BUILD_EU_VERSION
                PutStringYW(8, end_showzo + 101, 244, pos_y + 44, 0x800000, alp, 0xa000, 0);
#else
                PutStringYW(8, end_showzo + 101, 250, pos_y + 46, 0x800000, alp, 0xa000, 0);
#endif
            }

            PutNumberYW(0, mc_header_buf[file_no].game.hour, 0xff, pos_y + 91, 1.0f, 1.0f, 0x808080, alp, 0xa000, 2, 1);
            PutStringYW(8, 108, 306, pos_y + 91, 0x808080, alp, 0xa000, 0);
            PutNumberYW(0, mc_header_buf[file_no].game.minute, 325, pos_y + 91, 1.0f, 1.0f, 0x808080, alp, 0xa000, 2, 1);
            PutStringYW(8, 108, 376, pos_y + 91, 0x808080, alp, 0xa000, 0);
            PutNumberYW(0, mc_header_buf[file_no].game.sec, 395, pos_y + 91, 1.0f, 1.0f, 0x808080, alp, 0xa000, 2, 1);

            switch(end_showzo)
            {
            case 0:
                sv_pht = 4;
            break;
            case 1:
                sv_pht = 0;
            break;
            case 2:
                sv_pht = 1;
            break;
            case 3:
                sv_pht = 2;
            break;
            case 4:
                sv_pht = 3;
            break;
            case 5:
                sv_pht = 4;
            break;
            default:
                sv_pht = 0xff;
            break;
            }

            if (sv_pht != 0xff)
            {
                SetSprFile(LOAD_ADDRESS_3);

#ifdef BUILD_EU_VERSION
                PutSpriteYW(SP_R042 + sv_pht, SP_R042 + sv_pht, 471.0f, pos_y + 50, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                PutSpriteYW(SV_PHT_V01, SV_PHT_H04, 468.0f, pos_y + 47, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
#else
                PutSpriteYW(SP_R042 + sv_pht, SP_R042 + sv_pht, 459.0f, pos_y + 50, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                PutSpriteYW(SV_PHT_V01, SV_PHT_H04, 456.0f, pos_y + 47, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
#endif
            }
        }

        if (mc_header_buf[file_no].game.clear_flg != 0)
        {
#ifdef BUILD_EU_VERSION
            PutSpriteYW(SV_PHT_V01R, SV_PHT_H04R, 468.0f, pos_y + 47, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
#else
            PutSpriteYW(SV_PHT_V01R, SV_PHT_H04R, 456.0f, pos_y + 47, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
#endif
        }
    }
}

static void DspSpFile2(u_char file_no, u_char pos_y, u_char alp, u_char type)
{
    CmnWindow(28, 0, pos_y, alp, 0x80);

    PutSpriteYW(OA_LD_FNT2_SLSH, OA_LD_FNT2_SLSH, -7.0f, pos_y + 2, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
}

static void DspSpInfo2(u_char csr_no, u_char pos_y, u_char alp, u_char type)
{
    u_char no;

    switch (mc_ctrl.album.sta[csr_no])
    {
    case 0:
        no = mc_ctrl.album.file_no[csr_no];

        PutSpriteYW(OA_LD_FNT2_ALBM, OA_LD_FNT2_ALBM, 0.0f, pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutNumberYW(9, no + 1, 319, pos_y + 49, 1.0f, 1.0f, 0x808080, alp, 0xa000, 2, 0);
        PutNumberYW(10, mc_header_buf[no].game.msn_no, 443, pos_y + 48, 1.0f, 1.0f, 0x808080, alp, 0xa000, 2, 0);
        PutNumberYW(10, 24, 485, pos_y + 48, 1.0f, 1.0f, 0x808080, alp, 0xa000, 2, 0);
        PutSpriteYW(OA_LD_OUT_SIR + mc_header_buf[no].game.room_no, OA_LD_OUT_SIR + mc_header_buf[no].game.room_no, 313.0f, pos_y + 1, 0.0f, 0x808080, alp, 0.25f, 0.25f, 0, 0xff, 1, 0, 0);
    break;
    case 3:
        PutStringYW(8, 107, 271, pos_y + 47, 0x808080, alp, 0xa000, 0);
    break;
    case 9:
        PutStringYW(8, 107, 271, pos_y + 47, 0x808080, alp, 0xa000, 0);
    break;
    case 0xff:
        PutSpriteYW(OA_LD_FNT2_NEW, OA_LD_FNT2_NEW, 0.0f, pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(OA_LD_FNT2_HIFN, OA_LD_FNT2_HIFN, 449.0f, pos_y + 48, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(OA_LD_FNT2_HIFN, OA_LD_FNT2_HIFN, 491.0f, pos_y + 49, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    break;
    }
}

char AlbmSlct(u_char csr3, u_char step, u_char alp_max, u_char ini)
{
    int i;
    float per;
    char rtrn;
    static u_char book_anm;
    static u_char back_alp;
    static u_char csr_bak;
    static u_char csr_bak2;
    BOOK init[6] = {
        { .siz_w = 122, .siz_h = 83, .pos_x = 120, .pos_y =  71, .ttl = 0, .alp = 0, .rgb = 0x80, },
        { .siz_w = 122, .siz_h = 83, .pos_x = 260, .pos_y =  71, .ttl = 0, .alp = 0, .rgb = 0x80, },
        { .siz_w = 122, .siz_h = 83, .pos_x = 400, .pos_y =  71, .ttl = 0, .alp = 0, .rgb = 0x80, },
        { .siz_w = 122, .siz_h = 83, .pos_x = 120, .pos_y = 181, .ttl = 0, .alp = 0, .rgb = 0x80, },
        { .siz_w = 122, .siz_h = 83, .pos_x = 260, .pos_y = 181, .ttl = 0, .alp = 0, .rgb = 0x80, },
        { .siz_w = 122, .siz_h = 83, .pos_x = 400, .pos_y = 181, .ttl = 0, .alp = 0, .rgb = 0x80, },
    };
    BOOK move = {
        .siz_w = 162, .siz_h = 110, .pos_x = 240, .pos_y = 112, .ttl = 0x80, .alp = 0x80, .rgb = 0x80,
    };

    switch (step)
    {
    case 12:
        if (csr_bak2 != 0xff)
        {
            csr_bak2 = 0xff;

            book[csr_bak] = init[csr_bak];
        }

        if (back_alp + 8 < alp_max)
        {
            back_alp += 8;
        }
        else
        {
            back_alp = alp_max;
        }

        for (i = 0; i < 6; i++)
        {
            if (book[i].alp + 8 < alp_max)
            {
                book[i].alp += 8;
            }
            else
            {
                book[i].alp = alp_max;
            }

            if (i == csr3)
            {
                book[i].rgb = 0x80;
            }
            else
            {
                book[i].rgb = 0x40;
            }
        }

        csr_bak = csr3;

        book_anm = 0;

        rtrn = 1;
    break;
    case 13:
        if (book_anm < 16)
        {
            book_anm++;
        }

        per = book_anm / 16.0f;

        for (i = 0; i < 6; i++)
        {
            if (i == csr_bak)
            {
                book[i].siz_w = (move.siz_w - init[i].siz_w) * per + init[i].siz_w;
                book[i].siz_h = (move.siz_h - init[i].siz_h) * per + init[i].siz_h;
                book[i].pos_x = (move.pos_x - init[i].pos_x) * per + init[i].pos_x;
                book[i].pos_y = (move.pos_y - init[i].pos_y) * per + init[i].pos_y;
                book[i].ttl = (move.ttl - init[i].ttl) * per + init[i].ttl;
                book[i].rgb = 0x80;
                book[i].alp = alp_max;
            }
            else
            {
                book[i].rgb = 0x40;
                book[i].alp = alp_max * (1.0f - per);
            }
        }

        csr_bak2 = csr_bak;

        rtrn = 2;
    break;
    default:
        for (i = 0; i < 6; i++)
        {
            book[i] = init[i];
        }

        back_alp = 0;
        book_anm = 0;

        csr_bak = 0xff;
        csr_bak2 = 0xff;

        rtrn = 0;
    break;
    }

    if (csr3 == 0xff)
    {
        return -1;
    }

    PolySquareYW(80.0f, 54.0f, 0x1e0, 0x100, 0, back_alp * 0.5f, 1.0f, 1.0f, 0x5000, 0, 0, 0);

    for (i = 0; i < 6; i++)
    {
        OpenBook(i, &book[i]);
    }

    return rtrn;
}

static void OpenBook(int i, BOOK *book)
{
    DISP_SPRT ds;
    u_short no;
    short pos_x;
    short pos_y;
    u_char scw;
    u_char sch;
    u_char alp;
    u_char rgb;

#ifdef BUILD_EU_VERSION
    no = i + 714;
#else
    no = i + 713;
#endif

    CopySprDToSpr(&ds, &spr_dat[no]);

    pos_x = book->pos_x + (book->siz_w - spr_dat[no].w) * 0.5f;
    pos_y = book->pos_y + (book->siz_h - spr_dat[no].h) * 0.5f;

    scw = (book->siz_w * 100) / spr_dat[no].w;
    sch = (book->siz_h * 100) / spr_dat[no].h;

    rgb = book->rgb;

    alp = book->alp;

    ds.x = pos_x;
    ds.y = pos_y;

    ds.alpha = alp;

    ds.r = rgb;
    ds.g = rgb;
    ds.b = rgb;

    pos_x += spr_dat[no].w * 0.5f;
    pos_y += spr_dat[no].h * 0.5f;

    ds.csx = pos_x;
    ds.csy = pos_y;

    ds.scw = scw / 100.0f;
    ds.sch = sch / 100.0f;

    ds.pri = 0x1000;
    ds.z = 0x0fffffff - ds.pri;

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);
}

static void SpMsgWin(u_char alpha)
{
    DrawMessageBox(0x19000, 26.0f, 340.0f, 588.0f, 80.0f, alpha);
}

static void SpSideBar(short int pos_x, short int pos_y, u_char alpha)
{
    PutSpriteYW(SV_LNZ_SCRBAR_BASE_U, SV_LNZ_SCRBAR_BASE_U, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(SV_LNZ_SCRBAR_BASE_D, SV_LNZ_SCRBAR_BASE_D, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(SV_LNZ_SCRBAR_BASE_M, SV_LNZ_SCRBAR_BASE_M, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 26.4f, 0, 0xff, 1, 0, 0);

#ifdef BUILD_EU_VERSION
    SideBar(3, 3, 282, 0, 662, alpha, 0, 0);
#else
    SideBar(3, 3, 282, 0, 661, alpha, 0, 0);
#endif

    SpTrFlsh(pos_x, pos_y, alpha);
}

static void SpTrFlsh(short int pos_x, short int pos_y, short int alpha)
{
    PutSpriteYW(SV_LNZ_SCR_TRIANGLE_U, SV_LNZ_SCR_TRIANGLE_D, pos_x, pos_y, 0.0f, 0x404040, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
}

static void DspSaveTitle(u_char type, short int pos_x, short int pos_y, u_char alp, u_char pri)
{
    PutSpriteYW(MNU_CURSOR1, MNU_CURSOR1, pos_x - 15, pos_y - 103, 0.0f, 0x808080, alp * 0.5f, 1.0f, 1.0f, 0, (u_char)(pri + 4) << 12, 1, 0, 0);

    switch (type) {
    case 0:
        PutSpriteYW(TTL_FNT_SAVE, TTL_FNT_SAVE, pos_x - 15, pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, (u_char)(pri + 3) << 12, 1, 0, 0);
    break;
    case 1:
        PutSpriteYW(TTL_FNT_LOAD, TTL_FNT_LOAD, pos_x - 15, pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, (u_char)(pri + 3) << 12, 1, 0, 0);
    break;
    case 2:
        PutSpriteYW(TTL_FNT_ALBM, TTL_FNT_ALBM, pos_x - 15, pos_y, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, (u_char)(pri + 3) << 12, 1, 0, 0);
    break;
    }

    PutSpriteYW(MNU_MAIN_BASE_HOL1, MNU_MAIN_BASE_HOL1, pos_x + 9, pos_y + 27, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, (u_char)(pri + 1) << 12, 1, 0, 0);
    PutSpriteYW(MNU_MAIN_BASE_HOL1, MNU_MAIN_BASE_HOL1, pos_x + 9, pos_y + 65, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, (u_char)(pri + 1) << 12, 1, 0, 0);
    PutSpriteYW(MNU_MAIN_BASE_VAT1, MNU_MAIN_BASE_VAT1_B, pos_x + 16, pos_y + 48, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, pri << 12, 1, 0, 0);
    PutSpriteYW(MNU_MAIN_BASE_VAT1, MNU_MAIN_BASE_VAT1_B, pos_x + 146, pos_y + 48, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, pri << 12, 1, 0, 0);
}

static void PhotoDataExchange()
{
    int i;

    for (i = 0; i < 24; i++)
    {
        album_save_dat[0].pic[i] = pfile_wrk.pic[i];
    }
}

static int AlbmDesignLoadInGame(u_char side, u_char type)
{
    u_int addr;
    int load_id;

    if (side == 0)
    {
        addr = SIDE_0_ADDRESS;
    }
    else if (side == 1)
    {
        addr = SIDE_1_ADDRESS;
    }

    switch(type)
    {
    case 0:
#ifdef BUILD_EU_VERSION
        load_id = LoadReqLanguage(PL_ALBM_SW_E_PK2, addr);
#else
        load_id = LoadReq(PL_ALBM_SW_PK2, addr);
#endif
    break;
    case 1:
#ifdef BUILD_EU_VERSION
        load_id = LoadReqLanguage(PL_ALBM_SP_E_PK2, addr);
#else
        load_id = LoadReq(PL_ALBM_SP_PK2, addr);
#endif
    break;
    case 2:
#ifdef BUILD_EU_VERSION
        load_id = LoadReqLanguage(PL_ALBM_SR_E_PK2, addr);
#else
        load_id = LoadReq(PL_ALBM_SR_PK2, addr);
#endif
    break;
    case 3:
#ifdef BUILD_EU_VERSION
        load_id = LoadReqLanguage(PL_ALBM_SG_E_PK2, addr);
#else
        load_id = LoadReq(PL_ALBM_SG_PK2, addr);
#endif
    break;
    case 4:
#ifdef BUILD_EU_VERSION
        load_id = LoadReqLanguage(PL_ALBM_SB_E_PK2, addr);
#else
        load_id = LoadReq(PL_ALBM_SB_PK2, addr);
#endif
    break;
    case 5:
#ifdef BUILD_EU_VERSION
        load_id = LoadReqLanguage(PL_ALBM_SO_E_PK2, addr);
#else
        load_id = LoadReq(PL_ALBM_SO_PK2, addr);
#endif
    break;
    default:
        load_id = -1;
    break;
    }

    return load_id;
}

void AlbumModeInGameOverInit()
{
    ingame_wrk.mode = 21;

    albm_mode.load_id = -1;

    ingame_wrk.stts |= 0x20;

    albm_mode.step = 0;
}

char AlbumModeInGameOver()
{
    char rtrn;

    if (albm_mode.load_id != -1 && IsLoadEnd(albm_mode.load_id) != 0)
    {
        albm_mode.load_id = -1;
    }

    rtrn = 0;

    switch(albm_mode.step)
    {
    case ALBM_MODE_INIT:
        albm_mode.load_side = 0;
        albm_mode.in_album = 0;
        albm_mode.type[0] = albm_mode.type[1] = 5;
#ifdef BUILD_EU_VERSION
        albm_mode.load_id = LoadReqLanguage(PL_PSVP_E_PK2, LOAD_ADDRESS_1);
        albm_mode.load_id = LoadReqLanguage(PL_SAVE_E_PK2, LOAD_ADDRESS_2);
        albm_mode.load_id = LoadReqLanguage(PL_ALBM_SAVE_E_PK2, LOAD_ADDRESS_3);
#else
        albm_mode.load_id = LoadReq(PL_PSVP_PK2, LOAD_ADDRESS_1);
        albm_mode.load_id = LoadReq(PL_SAVE_PK2, LOAD_ADDRESS_2);
        albm_mode.load_id = LoadReq(PL_ALBM_SAVE_PK2, LOAD_ADDRESS_3);
#endif
        albm_mode.step = ALBM_MODE_LOAD_FOR_INN;
    break;
    case ALBM_MODE_LOAD_FOR_INN:
        if (albm_mode.load_id == -1)
        {
            InitialDmaBuffer();

            if (albm_mode.in_album != 0)
            {
                mcInit(MC_MODE_ALBUMLOAD_GAME2, (u_int *)0x420000, 0);
            }
            else
            {
                mcInit(MC_MODE_ALBUMLOAD_GAME1, (u_int *)0x420000, 0);
            }

            albm_mode.step = ALBM_MODE_DATA_LOAD;
        }
    break;
    case ALBM_MODE_DATA_LOAD:
        SetSprFile(LOAD_ADDRESS_1);
        SetSprFile(LOAD_ADDRESS_2);
        SetSprFile(LOAD_ADDRESS_3);

        switch (McAtLoadChk(2))
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            if (albm_mode.in_album != 0) {
                album_save_dat[1] = mc_album_save;
                albm_mode.type[1] = mc_album_type;
                MemAlbmInit2(1, mc_photo_num, mc_album_type, mc_ctrl.port + 1, mc_ctrl.sel_file + 1);
                albm_mode.load_id = AlbmDesignLoadInGame(0, albm_mode.type[0]);
                albm_mode.load_id = AlbmDesignLoadInGame(1, mc_album_type);
#ifdef BUILD_EU_VERSION
                albm_mode.load_id = LoadReqLanguage(PL_ALBM_E_PK2, LOAD_ADDRESS_2);
#else
                albm_mode.load_id = LoadReq(PL_ALBM_PK2, LOAD_ADDRESS_2);
#endif
                albm_mode.step = ALBM_MODE_PRE;

            } else {
                PhotoDataExchange();
                albm_mode.type[0] = 5;
                album_save_dat[1] = mc_album_save;
                albm_mode.type[1] = mc_album_type;
                MemAlbmInit(0, 0xff, mc_photo_num, 0xff, mc_album_type, 0xff, mc_ctrl.port + 1, 0xff, (u_char)(mc_ctrl.sel_file + 1));
                albm_mode.load_id = AlbmDesignLoadInGame(0, 5);
                albm_mode.load_id = AlbmDesignLoadInGame(1, mc_album_type);
#ifdef BUILD_EU_VERSION
                albm_mode.load_id = LoadReqLanguage(PL_ALBM_E_PK2, LOAD_ADDRESS_2);
#else
                albm_mode.load_id = LoadReq(PL_ALBM_PK2, LOAD_ADDRESS_2);
#endif
                albm_mode.step = ALBM_MODE_PRE;
            }
        break;
        case 2:
            if (albm_mode.in_album != 0)
            {
                MemAlbmInit3();

                albm_mode.load_id = AlbmDesignLoadInGame(0, 5);
                albm_mode.load_id = AlbmDesignLoadInGame(1, mc_album_type);
#ifdef BUILD_EU_VERSION
                albm_mode.load_id = LoadReqLanguage(PL_ALBM_E_PK2, LOAD_ADDRESS_2);
#else
                albm_mode.load_id = LoadReq(PL_ALBM_PK2, LOAD_ADDRESS_2);
#endif
                albm_mode.step = ALBM_MODE_PRE;
            }
            else
            {
                albm_mode.step = ALBM_MODE_OUT;
            }
        break;
        case 3:
            if (albm_mode.in_album != 0)
            {
                albm_mode.type[1] = 5;

                NewAlbumInit(1);

                albm_mode.load_id = AlbmDesignLoadInGame(0, albm_mode.type[0]);
                albm_mode.load_id = AlbmDesignLoadInGame(1, 5);
#ifdef BUILD_EU_VERSION
                albm_mode.load_id = LoadReqLanguage(PL_ALBM_E_PK2, LOAD_ADDRESS_2);
#else
                albm_mode.load_id = LoadReq(PL_ALBM_PK2, LOAD_ADDRESS_2);
#endif
                albm_mode.step = ALBM_MODE_PRE;
            } else {
                albm_mode.type[0] = 5;

                PhotoDataExchange();

                MemAlbmInit(0, 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0);

                albm_mode.load_id = AlbmDesignLoadInGame(0, albm_mode.type[0]);
                albm_mode.type[1] = 5;

                NewAlbumInit(1);

                albm_mode.load_id = AlbmDesignLoadInGame(1, 5);
                albm_mode.step = ALBM_MODE_PRE;
#ifdef BUILD_EU_VERSION
                albm_mode.load_id = LoadReqLanguage(PL_ALBM_E_PK2, LOAD_ADDRESS_2);
#else
                albm_mode.load_id = LoadReq(PL_ALBM_PK2, LOAD_ADDRESS_2);
#endif
            }
        break;
        }
    break;
    case ALBM_MODE_PRE:
        if (albm_mode.load_id == -1)
        {
            albm_mode.step = ALBM_MODE_BODY;
        }
    break;
    case ALBM_MODE_BODY:
        BgFusumaYW(0x606060, 0.0f, 128.0f, 0x7d000);
        SetSprFile(LOAD_ADDRESS_2);

        switch(SweetMemories(0, 0x80))
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            mcInit(MC_MODE_ALBUMSAVE, (u_int *)0x420000, 1);

            albm_mode.load_side = 0;
#ifdef BUILD_EU_VERSION
            albm_mode.load_id = LoadReqLanguage(PL_SAVE_E_PK2, LOAD_ADDRESS_2);
            albm_mode.load_id = LoadReqLanguage(PL_ALBM_SAVE_E_PK2, LOAD_ADDRESS_3);
#else
            albm_mode.load_id = LoadReq(PL_SAVE_PK2, LOAD_ADDRESS_2);
            albm_mode.load_id = LoadReq(PL_ALBM_SAVE_PK2, LOAD_ADDRESS_3);
#endif
            albm_mode.in_album = 1;
            albm_mode.step = ALBM_MODE_AFT;
        break;
        case 2:
            mcInit(MC_MODE_ALBUMSAVE, (u_int *)0x420000, 1);

            albm_mode.load_side = 1;
#ifdef BUILD_EU_VERSION
            albm_mode.load_id = LoadReqLanguage(PL_SAVE_E_PK2, LOAD_ADDRESS_2);
            albm_mode.load_id = LoadReqLanguage(PL_ALBM_SAVE_E_PK2, LOAD_ADDRESS_3);
#else
            albm_mode.load_id = LoadReq(PL_SAVE_PK2, LOAD_ADDRESS_2);
            albm_mode.load_id = LoadReq(PL_ALBM_SAVE_PK2, LOAD_ADDRESS_3);
#endif
            albm_mode.in_album = 1;
            albm_mode.step = ALBM_MODE_AFT;
        break;
        case 3:
            albm_mode.load_side = 0;
#ifdef BUILD_EU_VERSION
            albm_mode.load_id = LoadReqLanguage(PL_SAVE_E_PK2, LOAD_ADDRESS_2);
            albm_mode.load_id = LoadReqLanguage(PL_ALBM_SAVE_E_PK2, LOAD_ADDRESS_3);
#else
            albm_mode.load_id = LoadReq(PL_SAVE_PK2, LOAD_ADDRESS_2);
            albm_mode.load_id = LoadReq(PL_ALBM_SAVE_PK2, LOAD_ADDRESS_3);
#endif
            albm_mode.in_album = 1;
            albm_mode.step = ALBM_MODE_LOAD_FOR_INN;
        break;
        case 4:
            albm_mode.load_side = 1;
#ifdef BUILD_EU_VERSION
            albm_mode.load_id = LoadReqLanguage(PL_SAVE_E_PK2, LOAD_ADDRESS_2);
            albm_mode.load_id = LoadReqLanguage(PL_ALBM_SAVE_E_PK2, LOAD_ADDRESS_3);
#else
            albm_mode.load_id = LoadReq(PL_SAVE_PK2, LOAD_ADDRESS_2);
            albm_mode.load_id = LoadReq(PL_ALBM_SAVE_PK2, LOAD_ADDRESS_3);
#endif
            albm_mode.in_album = 1;
            albm_mode.step = ALBM_MODE_LOAD_FOR_INN;
        break;
        case 5:
            albm_mode.step = ALBM_MODE_OUT;
        break;
        }
    break;
    case ALBM_MODE_AFT:
        if (albm_mode.load_id == -1)
        {
            albm_mode.step = ALBM_MODE_DATA_SAVE;
        }
    break;
    case ALBM_MODE_DATA_SAVE:
        SetSprFile(LOAD_ADDRESS_1);
        SetSprFile(LOAD_ADDRESS_2);
        SetSprFile(LOAD_ADDRESS_3);

        switch (McAtAlbmChk())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            if (albm_mode.in_album != 0)
            {
                albm_mode.type[albm_mode.load_side] = mc_album_type;

                MemAlbmInit2(albm_mode.load_side, mc_photo_num, mc_album_type, mc_ctrl.port + 1, mc_ctrl.sel_file + 1);

                albm_mode.load_id = AlbmDesignLoadInGame(0, albm_mode.type[0]);
                albm_mode.load_id = AlbmDesignLoadInGame(1, albm_mode.type[1]);
#ifdef BUILD_EU_VERSION
                albm_mode.load_id = LoadReqLanguage(PL_ALBM_E_PK2, LOAD_ADDRESS_2);
#else
                albm_mode.load_id = LoadReq(PL_ALBM_PK2, LOAD_ADDRESS_2);
#endif
                albm_mode.step = ALBM_MODE_PRE;
            }
            else
            {
                PhotoDataExchange();

                albm_mode.type[0] = 5;
                albm_mode.type[1] = mc_album_type;

                MemAlbmInit(0, 0xff, 0, 0xff, 0, 0xff, 0, 0xff, 0);
                NewAlbumInit(1);

                albm_mode.load_id = AlbmDesignLoadInGame(0, albm_mode.type[0]);
                albm_mode.load_id = AlbmDesignLoadInGame(1, albm_mode.type[1]);
#ifdef BUILD_EU_VERSION
                albm_mode.load_id = LoadReqLanguage(PL_ALBM_E_PK2, LOAD_ADDRESS_2);
#else
                albm_mode.load_id = LoadReq(PL_ALBM_PK2, LOAD_ADDRESS_2);
#endif
                albm_mode.step = ALBM_MODE_PRE;
            }
    break;
        case 2:
            if (albm_mode.in_album != 0)
            {
                MemAlbmInit3();

                albm_mode.load_id = AlbmDesignLoadInGame(0, albm_mode.type[0]);
                albm_mode.load_id = AlbmDesignLoadInGame(1, albm_mode.type[1]);
#ifdef BUILD_EU_VERSION
                albm_mode.load_id = LoadReqLanguage(PL_ALBM_E_PK2, LOAD_ADDRESS_2);
#else
                albm_mode.load_id = LoadReq(PL_ALBM_PK2, LOAD_ADDRESS_2);
#endif
                albm_mode.step = ALBM_MODE_PRE;
            }
            else
            {
                mcInit(MC_MODE_ALBUMLOAD_GAME1, (u_int *)0x420000, 0);

                albm_mode.step = ALBM_MODE_DATA_LOAD;
            }
    break;
        }
    break;
    case ALBM_MODE_OUT:
        if (albm_mode.load_id == -1)
        {
            ingame_wrk.stts&= ~0x20;

            InitialDmaBuffer();

            rtrn = 1;
        }
    break;
    }

    return rtrn;
}

int SavePointLightJudge()
{
    if (EnemyUseJudge(0) != 0)
    {
        return 0;
    }

    return ev_wrk.btl_lock < 1;
}
