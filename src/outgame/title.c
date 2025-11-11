#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "title.h"

// #include <cstdlib.h>
// RAND_MAX = (2**31-1)
#define RAND_MAX 2147483647

#include "ee/eestruct.h"
#include "ee/kernel.h"

#include "sce/libgraph.h"
#ifdef BUID_EU_VERSION
#include "sce/libdma.h"
#endif

#include "main/glob.h"
#include "main/gamemain.h"
#include "mc/mc_at.h"
#include "mc/mc_main.h"
#ifdef BUILD_EU_VERSION
#include "os/system.h"
#endif
#include "os/eeiop/eese.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/adpcm/ea_dat.h"
#include "os/eeiop/adpcm/ea_ctrl.h"
#include "outgame/outgame.h"
#include "outgame/mode_slct.h"
#include "outgame/memory_album.h"
#include "outgame/btl_mode/btl_menu.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/ig_rank.h"
#include "ingame/menu/ig_glst.h"
#include "ingame/menu/ig_album.h"
#include "ingame/menu/ig_camra.h"
#include "graphics/mov/movie.h"
#include "graphics/motion/mdlwork.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph2d/effect_obj.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph3d/sgdma.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/libsg.h"

static int AlbmDesignLoad(u_char side, u_char type);

#define PI 3.1415927f
#define DEG2RAD(x) ((float)(x)*PI/180.0f)

#include "data/title_sprt.h" // data 342c90 */ SPRT_DAT title_sprt[11];
#include "data/font_sprt.h" // data 342df0 */ SPRT_DAT font_sprt[20];
#ifdef BUILD_EU_VERSION
#include "data/logotex.h" // static SPRT_DAT logotex[];
#endif
TITLE_WRK title_wrk = {0};

int opening_movie_type = 0;

static TTL_DSP_WRK ttl_dsp;

static OUT_DITHER_STR out_dither;
#ifdef BUILD_EU_VERSION
static TITLE_SYS title_sys;
#endif

#ifdef BUILD_EU_VERSION
#define SPRITE_ADDRESS 0xa30000
#define TIM2_ADDRESS 0x1e30000
#define MC_WORK_ADDRESS 0x420000

#define SPRITE_ADDR_1 0x0c80000
#define SPRITE_ADDR_2 0x1d10000
#define SPRITE_ADDR_3 0x1d23680
#define SPRITE_ADDR_4 0x1d54030

#define ALBUM_DESIGN_SIDE_0_ADDRESS 0x1d83000
#define ALBUM_DESIGN_SIDE_1_ADDRESS 0x1dc3470
#else
#define SPRITE_ADDRESS 0xa30000
#define TIM2_ADDRESS 0x1e30000
#define MC_WORK_ADDRESS 0x420000

#define SPRITE_ADDR_1 0x0c80000
#define SPRITE_ADDR_2 0x1d15600
#define SPRITE_ADDR_3 0x1d28c80
#define SPRITE_ADDR_4 0x1d59630

#define ALBUM_DESIGN_SIDE_0_ADDRESS 0x1d88100
#define ALBUM_DESIGN_SIDE_1_ADDRESS 0x1dc8570
#endif

#ifdef BUILD_EU_VERSION
void ChangeTVMode(int mode)
{
    if (sys_wrk.pal_disp_mode == mode)
    {
        return;
    }
    sys_wrk.pal_disp_mode = mode;

    if (mode == 0)
    {
        g_db.disp[1].display.DX = 0x290;
        g_db.disp[1].display.DY = 0x68;
        g_db.disp[0].display.DX = 0x290;
        g_db.disp[0].display.DY = 0x68;

        mc_language = mc_language & 0x7f;
    }
    else
    {
        g_db.disp[1].display.DX = 0x27c;
        g_db.disp[1].display.DY = 0x32;
        g_db.disp[0].display.DX = 0x27c;
        g_db.disp[0].display.DY = 0x32;

        mc_language |= 0x80;
    }

    vfunc();

    clearGsMem(0, 0, 0, 640, 448);
    sceGsSyncPath(0, 0);

    vfunc();

    clearGsMem(0, 0, 0, 640, 448);
    sceGsSyncPath(0, 0);

    vfunc();
    vfunc();

    sceGsResetPath();
    sceDmaReset(1);
    sceGsResetGraph(0, SCE_GS_INTERLACE, mode == 0 ? SCE_GS_PAL: SCE_GS_NTSC, SCE_GS_FRAME);

    vfunc();
    vfunc();

    ttl_dsp.no_disp = 0xf;
}
#endif

void TitleCtrl()
{
    static u_int mc_pnum1;
    static u_int mc_pnum2;
    static u_int mc_atyp1;
    static u_int mc_atyp2;
    static u_int mc_slot1;
    static u_int mc_slot2;
    static u_int mc_file1;
    static u_int mc_file2;
    static int title_cnt = 0;

    switch(title_wrk.mode)
    {
    case TITLE_INIT:
        ttl_dsp = (TTL_DSP_WRK){0};

#ifdef BUILD_EU_VERSION
        title_wrk.load_id = LoadReqLanguage(TITLE_E_PK2, SPRITE_ADDRESS);
#else
        title_wrk.load_id = LoadReq(TITLE_PK2, SPRITE_ADDRESS);
#endif

        InitOutDither();
        MakeOutDither();

        title_wrk.mode = TITLE_WAIT;
    break;

    case TITLE_WAIT:
        if (IsLoadEnd(title_wrk.load_id) == 0)
        {
            break;
        }

        SeStopAll();

        title_wrk.csr = 0;
        ttl_dsp.timer = 0;
        title_wrk.mode = TITLE_TITLE_WAIT;
        title_cnt = 0;
    case TITLE_TITLE_WAIT:
        title_wrk.mode = TITLE_TITLE_WAIT2;

        EAdpcmFadeOut(0x3c);
    case TITLE_TITLE_WAIT2:
        SetSprFile(SPRITE_ADDRESS);

        TitleWaitMode();
        DispOutDither();

        if (IsEndAdpcmFadeOut() != 0)
        {
            EAdpcmCmdPlay(0, 1, AO000_TITLE_STR, 0, GetAdpcmVol(AO000_TITLE_STR), 0x280, 0xfff, 0);

            title_wrk.mode = TITLE_TITLE;
            ttl_dsp.timer = 0;
        }
    break;
    case TITLE_TITLE:
        title_cnt++;

        SetSprFile(SPRITE_ADDRESS);
        TitleWaitMode();
        DispOutDither();

        if (title_cnt >= 60*46 && title_wrk.mode != TITLE_TITLE_SEL_INIT)
        {
            title_wrk.mode = TITLE_MOVIE_STEP1;
            EAdpcmFadeOut(0x3c);
        }
    break;
    case TITLE_MOVIE_STEP1:
        if (IsEndAdpcmFadeOut() != 0)
        {
            title_cnt = 60*3;
            title_wrk.mode = TITLE_MOVIE_STEP2;
        }
    break;
    case TITLE_MOVIE_STEP2:
        if (title_cnt != 0)
        {
            if (--title_cnt != 0)
            {
                break;
            }
        }

        if (opening_movie_type != 0)
        {
            MoviePlay(SCENE_NO_9_20_0);
        }
        else
        {
            MoviePlay(SCENE_NO_9_10_0);
        }

        opening_movie_type = opening_movie_type == 0;
        title_wrk.mode = TITLE_INIT;
    break;
    case TITLE_INIT_FROM_IN_BGMREQ:
        if (IsEndAdpcmFadeOut() != 0)
        {
            EAdpcmCmdPlay(0, 1, AO000_TITLE_STR, 0, GetAdpcmVol(AO000_TITLE_STR), 0x280, 0xfff, 0);

            title_wrk.mode = TITLE_INIT_FROM_IN;
        }
    break;
    case TITLE_INIT_FROM_IN:
#ifdef BUILD_EU_VERSION
        title_wrk.load_id = LoadReqLanguage(TITLE_E_PK2, SPRITE_ADDRESS);
#else
        title_wrk.load_id = LoadReq(TITLE_PK2, SPRITE_ADDRESS);
#endif

        title_wrk.mode = TITLE_WAIT_FROM_IN;
    break;
    case TITLE_WAIT_FROM_IN:
        if (IsLoadEnd(title_wrk.load_id) == 0)
        {
            break;
        }

        SeStopAll();

        title_wrk.csr = 0;
        title_wrk.mode = TITLE_TITLE_SEL;
    case TITLE_TITLE_SEL_BGMREQ:
        if (IsEndAdpcmFadeOut() != 0)
        {
            EAdpcmCmdPlay(0, 1, AO000_TITLE_STR, 0, GetAdpcmVol(AO000_TITLE_STR), 0x280, 0xfff, 0);

            title_wrk.mode = TITLE_TITLE_SEL_INIT;
        }
    break;
    case TITLE_TITLE_SEL_INIT:
        title_wrk.csr = 1;
        title_wrk.mode = TITLE_TITLE_SEL;
    case TITLE_TITLE_SEL:
        SetSprFile(SPRITE_ADDRESS);

        if (ttl_dsp.mode != 0)
        {
            TitleStartSlctYW(0, 0x80);
        }
        else
        {
            SetFTIM2File(TIM2_ADDRESS);
            TitleStartSlct();
        }

        DispOutDither();
    break;
    case TITLE_LOAD_PRE:
        if (IsLoadEnd(title_wrk.load_id) != 0)
        {
            sys_wrk.load = 1;
            title_wrk.mode = TITLE_MEMCA_LOAD_WAIT;
            title_wrk.sub_mode = 0;
        }
    break;
    case TITLE_MEMCA_LOAD_WAIT:
        if (IsEndAdpcmFadeOut() != 0)
        {
            EAdpcmCmdPlay(0, 1, AB018_STR, 0, GetAdpcmVol(AB018_STR), 0x280, 0xfff, 0);

            title_wrk.mode = TITLE_MEMCA_LOAD;
        }
    break;
    case TITLE_MEMCA_LOAD:
        if (title_wrk.sub_mode == 0)
        {
            mcInit(1, (u_int *)MC_WORK_ADDRESS, 0);
            title_wrk.sub_mode = 7;
        }

        SetSprFile(SPRITE_ADDR_4);
        SetSprFile(SPRITE_ADDR_2);
        SetSprFile(SPRITE_ADDR_3);

        switch (McAtLoadChk(1))
        {
            case 0:
                // do nothing ...
            break;
            case 1:
                ingame_wrk.stts &= (0x80 | 0x40 | 0x10 | 0x8 | 0x4 | 0x2 | 0x1);

                InitialDmaBuffer();

                if (ingame_wrk.clear_count != 0)
                {
                    ModeSlctInit(0, 0);

                    OutGameModeChange(OUTGAME_MODE_MODESEL);
                }
                else
                {
                    EAdpcmFadeOut(0x3c);

                    LoadGameInit();

                    ingame_wrk.game = 0;

                    GameModeChange(GAME_MODE_INIT);
                }
            break;
            case 2:
                EAdpcmFadeOut(0x3c);

                title_wrk.mode = TITLE_TITLE_SEL_BGMREQ;

                ingame_wrk.stts &= (0x80 | 0x40 | 0x10 | 0x8 | 0x4 | 0x2 | 0x1);

                InitialDmaBuffer();
            break;
        }
    break;
    case TITLE_MODE_SEL_BGMREQ:
        if (IsEndAdpcmFadeOut() != 0)
        {
            EAdpcmCmdPlay(0, 1, AO000_TITLE_STR, 0, GetAdpcmVol(AO000_TITLE_STR), 0x280, 0xfff, 0);

            title_wrk.mode = TITLE_MODE_SEL;
        }
    break;
    case TITLE_MODE_SEL:
        SetSprFile(SPRITE_ADDRESS);

        if (ttl_dsp.mode != 0)
        {
            TitleSelectModeYW(0, 0x80);
        }
        else
        {
            SetFTIM2File(TIM2_ADDRESS);
            TitleSelectMode();
        }

        DispOutDither();
    break;
    case TITLE_DIFF_SEL:
        TitleSelectDifficultyYW();
    break;
    case TITLE_ALBM_LOAD_PRE:
        if (IsLoadEnd(title_wrk.load_id) != 0)
        {
            sys_wrk.load = 1;

            mc_pnum2 = mc_pnum1 = 0xff;
            mc_atyp2 = mc_atyp1 = 0xff;
            mc_slot2 = mc_slot1 = 0xff;
            mc_file2 = mc_file1 = 0xff;

            mcInit(5, (u_int *)MC_WORK_ADDRESS, 0);

            title_wrk.mode = TITLE_ALBM_LOAD0;
        }
    break;
    case TITLE_ALBM_LOAD0:
        if (IsEndAdpcmFadeOut() != 0)
        {
            EAdpcmCmdPlay(0, 1, AB018_STR, 0, GetAdpcmVol(AB018_STR), 0x280, 0xfff, 0);

            title_wrk.mode = TITLE_ALBM_LOAD1;
        }
    break;
    case TITLE_ALBM_LOAD1:
        SetSprFile(SPRITE_ADDR_4);
        SetSprFile(SPRITE_ADDR_2);
        SetSprFile(SPRITE_ADDR_3);

        switch (McAtLoadChk(2))
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            album_save_dat[0] = mc_album_save;

            mc_pnum1 = mc_photo_num;
            mc_atyp1 = mc_album_type;
            mc_slot1 = mc_ctrl.port + 1;
            mc_file1 = mc_ctrl.sel_file + 1;

            memcpy((void *)0xE80000, (void *)0x5a0000, 0x180000);

            mcInit(6, (u_int *)MC_WORK_ADDRESS, 0);

            title_wrk.mode = TITLE_ALBM_LOAD2;
        break;
        case 3:
            printf("ここに来るわけがない！\n");
        break;
        case 2:
            EAdpcmFadeOut(0x3c);

            title_wrk.mode = TITLE_TITLE_SEL_BGMREQ;
        break;
        }
    break;
    case TITLE_ALBM_LOAD2:
        SetSprFile(SPRITE_ADDR_2);
        SetSprFile(SPRITE_ADDR_3);

        switch (McAtLoadChk(2))
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            album_save_dat[1] = mc_album_save;

            mc_pnum2 = mc_photo_num;
            mc_atyp2 = mc_album_type;
            mc_slot2 = mc_ctrl.port + 1;
            mc_file2 = mc_ctrl.sel_file + 1;

            memcpy((void *)0x1000000, (void *)0x5a0000, 0x180000);

            MemAlbmInit(1, mc_pnum1, mc_pnum2, mc_atyp1, mc_atyp2, mc_slot1, mc_slot2, mc_file1, mc_file2 & 0xff);

#ifdef BUILD_EU_VERSION
            title_wrk.load_id = LoadReqLanguage(PL_ALBM_FSM_E_PK2, SPRITE_ADDR_1);
            title_wrk.load_id = LoadReqLanguage(PL_ALBM_E_PK2, SPRITE_ADDR_2);
#else
            title_wrk.load_id = LoadReq(PL_ALBM_FSM_PK2, SPRITE_ADDR_1);
            title_wrk.load_id = LoadReq(PL_ALBM_PK2, SPRITE_ADDR_2);
#endif
            title_wrk.load_id = AlbmDesignLoad(0, mc_atyp1);
            title_wrk.load_id = AlbmDesignLoad(1, mc_atyp2);

            title_wrk.mode = TITLE_ALBM_MAIN_PRE;
        break;
        case 3:
            mc_pnum2 = 0;
            mc_atyp2 = 5;
            mc_slot2 = 0;
            mc_file2 = 0;

            memcpy((void *)0x1000000, (void *)0x5a0000, 0x180000);

            MemAlbmInit(1, mc_pnum1, mc_pnum2, mc_atyp1, mc_atyp2, mc_slot1, mc_slot2, mc_file1, mc_file2 & 0xff);
            NewAlbumInit(1);

#ifdef BUILD_EU_VERSION
            title_wrk.load_id = LoadReqLanguage(PL_ALBM_FSM_E_PK2, SPRITE_ADDR_1);
            title_wrk.load_id = LoadReqLanguage(PL_ALBM_E_PK2, SPRITE_ADDR_2);
#else
            title_wrk.load_id = LoadReq(PL_ALBM_FSM_PK2, SPRITE_ADDR_1);
            title_wrk.load_id = LoadReq(PL_ALBM_PK2, SPRITE_ADDR_2);
#endif
            title_wrk.load_id = AlbmDesignLoad(0, mc_atyp1);
            title_wrk.load_id = AlbmDesignLoad(1, mc_atyp2);

            title_wrk.mode = TITLE_ALBM_MAIN_PRE;
        break;
        case 2:
            EAdpcmFadeOut(0x3c);

            title_wrk.mode = TITLE_TITLE_SEL_BGMREQ;
        break;
        }
    break;
    case TITLE_ALBM_MAIN_PRE:
        if (IsLoadEnd(title_wrk.load_id) != 0)
        {
            title_wrk.mode = TITLE_ALBM_MAIN;
        }
    break;
    case TITLE_ALBM_MAIN:
        SetSprFile(SPRITE_ADDR_1);
        SetSprFile(SPRITE_ADDR_2);

        switch(SweetMemories(1, 0x80))
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            memcpy((void *)0x5a0000, (void *)0xe80000, 0x180000);

            mcInit(2, (u_int *)MC_WORK_ADDRESS, 0);

            title_wrk.load_side = 0;

#ifdef BUILD_EU_VERSION
            title_wrk.load_id = LoadReqLanguage(PL_PSVP_E_PK2, SPRITE_ADDR_4);
            title_wrk.load_id = LoadReqLanguage(PL_SAVE_E_PK2, SPRITE_ADDR_2);
            title_wrk.load_id = LoadReqLanguage(PL_ALBM_SAVE_E_PK2, SPRITE_ADDR_3);
#else
            title_wrk.load_id = LoadReq(PL_PSVP_PK2, SPRITE_ADDR_4);
            title_wrk.load_id = LoadReq(PL_SAVE_PK2, SPRITE_ADDR_2);
            title_wrk.load_id = LoadReq(PL_ALBM_SAVE_PK2, SPRITE_ADDR_3);
#endif

            title_wrk.mode = TITLE_ALBM_SAVE_PRE;
        break;
        case 2:
            memcpy((void *)0x5a0000, (void *)0x1000000, 0x180000);

            mcInit(2, (u_int *)MC_WORK_ADDRESS, 0);

            title_wrk.load_side = 1;

#ifdef BUILD_EU_VERSION
            title_wrk.load_id = LoadReqLanguage(PL_PSVP_E_PK2, SPRITE_ADDR_4);
            title_wrk.load_id = LoadReqLanguage(PL_SAVE_E_PK2, SPRITE_ADDR_2);
            title_wrk.load_id = LoadReqLanguage(PL_ALBM_SAVE_E_PK2, SPRITE_ADDR_3);
#else
            title_wrk.load_id = LoadReq(PL_PSVP_PK2, SPRITE_ADDR_4);
            title_wrk.load_id = LoadReq(PL_SAVE_PK2, SPRITE_ADDR_2);
            title_wrk.load_id = LoadReq(PL_ALBM_SAVE_PK2, SPRITE_ADDR_3);
#endif

            title_wrk.mode = TITLE_ALBM_SAVE_PRE;
        break;
        case 3:
#ifdef BUILD_EU_VERSION
            title_wrk.load_id = LoadReqLanguage(PL_PSVP_E_PK2, SPRITE_ADDR_4);
            title_wrk.load_id = LoadReqLanguage(PL_SAVE_E_PK2, SPRITE_ADDR_2);
            title_wrk.load_id = LoadReqLanguage(PL_ALBM_SAVE_E_PK2, SPRITE_ADDR_3);
#else
            title_wrk.load_id = LoadReq(PL_PSVP_PK2, SPRITE_ADDR_4);
            title_wrk.load_id = LoadReq(PL_SAVE_PK2, SPRITE_ADDR_2);
            title_wrk.load_id = LoadReq(PL_ALBM_SAVE_PK2, SPRITE_ADDR_3);
#endif

            title_wrk.load_side = 0;

            mcInit(5, (u_int *)MC_WORK_ADDRESS, 0);

            title_wrk.mode = TITLE_ALBM_LOAD_MODE_PRE;
        break;
        case 4:
#ifdef BUILD_EU_VERSION
            title_wrk.load_id = LoadReqLanguage(PL_PSVP_E_PK2, SPRITE_ADDR_4);
            title_wrk.load_id = LoadReqLanguage(PL_SAVE_E_PK2, SPRITE_ADDR_2);
            title_wrk.load_id = LoadReqLanguage(PL_ALBM_SAVE_E_PK2, SPRITE_ADDR_3);
#else
            title_wrk.load_id = LoadReq(PL_PSVP_PK2, SPRITE_ADDR_4);
            title_wrk.load_id = LoadReq(PL_SAVE_PK2, SPRITE_ADDR_2);
            title_wrk.load_id = LoadReq(PL_ALBM_SAVE_PK2, SPRITE_ADDR_3);
#endif

            title_wrk.load_side = 1;

            mcInit(5, (u_int *)MC_WORK_ADDRESS, 0);

            title_wrk.mode = TITLE_ALBM_LOAD_MODE_PRE;
        break;
        case 5:
            EAdpcmFadeOut(0x3c);

            title_wrk.mode = TITLE_TITLE_SEL_BGMREQ;
            title_wrk.csr = 0;

            ingame_wrk.stts &= (0x80 | 0x40 | 0x10 | 0x8 | 0x4 | 0x2 | 0x1);

            InitialDmaBuffer();
        break;
        }
    break;
    case TITLE_ALBM_LOAD_MODE_PRE:
        if (IsLoadEnd(title_wrk.load_id) != 0)
        {
            title_wrk.mode = TITLE_ALBM_LOAD_MODE;
        }
    break;
    case TITLE_ALBM_LOAD_MODE:
        SetSprFile(SPRITE_ADDR_4);
        SetSprFile(SPRITE_ADDR_2);
        SetSprFile(SPRITE_ADDR_3);

        switch (McAtLoadChk(2))
        {
        case 0:
            // do nothing ...
        break;
        case 3:
        case 1:
            if (title_wrk.load_side == 0)
            {
                memcpy((void *)0xe80000, (void *)0x5a0000, 0x180000);

                mc_pnum1 = mc_photo_num;
                mc_atyp1 = mc_album_type;
                mc_slot1 = mc_ctrl.port + 1;
                mc_file1 = mc_ctrl.sel_file + 1;

                album_save_dat[0] = mc_album_save;

                MemAlbmInit2(0, mc_pnum1, mc_atyp1, mc_slot1, mc_file1);
            }
            else
            {
                memcpy((void *)0x1000000, (void *)0x5a0000, 0x180000);

                mc_pnum2 = mc_photo_num;
                mc_atyp2 = mc_album_type;
                mc_slot2 = mc_ctrl.port + 1;
                mc_file2 = mc_ctrl.sel_file + 1;

                album_save_dat[1] = mc_album_save;

                MemAlbmInit2(1, mc_pnum2, mc_atyp2, mc_slot2, mc_file2);
            }

            title_wrk.load_id = AlbmDesignLoad(0, mc_atyp1);
            title_wrk.load_id = AlbmDesignLoad(1, mc_atyp2);
#ifdef BUILD_EU_VERSION
            title_wrk.load_id = LoadReqLanguage(PL_ALBM_E_PK2, SPRITE_ADDR_2);
#else
            title_wrk.load_id = LoadReq(PL_ALBM_PK2, SPRITE_ADDR_2);
#endif

            title_wrk.mode = TITLE_ALBM_MAIN_PRE;
        break;
        case 2:
            MemAlbmInit3();
            AlbmDesignLoad(0, mc_atyp1);
            AlbmDesignLoad(1, mc_atyp2);

#ifdef BUILD_EU_VERSION
            title_wrk.load_id = LoadReqLanguage(PL_ALBM_E_PK2, SPRITE_ADDR_2);
#else
            title_wrk.load_id = LoadReq(PL_ALBM_PK2, SPRITE_ADDR_2);
#endif

            title_wrk.mode = TITLE_ALBM_MAIN_PRE;
        break;
        }
    break;
    case TITLE_ALBM_SAVE_PRE:
        if (IsLoadEnd(title_wrk.load_id) != 0)
        {
            title_wrk.mode = TITLE_ALBM_SAVE;
        }
    break;
    case TITLE_ALBM_SAVE:
        SetSprFile(SPRITE_ADDR_4);
        SetSprFile(SPRITE_ADDR_2);
        SetSprFile(SPRITE_ADDR_3);

        switch (McAtAlbmChk())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            if (title_wrk.load_side == 0)
            {
                mc_pnum1 = mc_photo_num;
                mc_atyp1 = mc_album_type;
                mc_slot1 = mc_ctrl.port + 1;
                mc_file1 = mc_ctrl.sel_file + 1;

                MemAlbmInit2(0, mc_pnum1, mc_atyp1, mc_slot1, mc_file1);
            }
            else
            {
                mc_pnum2 = mc_photo_num;
                mc_atyp2 = mc_album_type;
                mc_slot2 = mc_ctrl.port + 1;
                mc_file2 = mc_ctrl.sel_file + 1;

                MemAlbmInit2(1, mc_pnum2, mc_atyp2, mc_slot2, mc_file2);
            }

            AlbmDesignLoad(0, mc_atyp1);
            AlbmDesignLoad(1, mc_atyp2);

#ifdef BUILD_EU_VERSION
            title_wrk.load_id = LoadReqLanguage(PL_ALBM_E_PK2, SPRITE_ADDR_2);
#else
            title_wrk.load_id = LoadReq(PL_ALBM_PK2, SPRITE_ADDR_2);
#endif

            title_wrk.mode = TITLE_ALBM_MAIN_PRE;
        break;
        case 2:
            MemAlbmInit3();

            AlbmDesignLoad(0, mc_atyp1);
            AlbmDesignLoad(1, mc_atyp2);

#ifdef BUILD_EU_VERSION
            title_wrk.load_id = LoadReqLanguage(PL_ALBM_E_PK2, SPRITE_ADDR_2);
#else
            title_wrk.load_id = LoadReq(PL_ALBM_PK2, SPRITE_ADDR_2);
#endif

            title_wrk.mode = TITLE_ALBM_MAIN_PRE;
        break;
        }
    break;
    case TITLE_MODE_SELECT:
        ModeSlctLoop();
    break;
    }

    if (ttl_dsp.timer < 60*2)
    {
        ttl_dsp.timer++;
    }
    else
    {
        ttl_dsp.timer = 0;
    }
}

static int AlbmDesignLoad(u_char side, u_char type)
{
    u_int addr;
    int load_id;

    if (side == 0)
    {
        addr = ALBUM_DESIGN_SIDE_0_ADDRESS;
    }

    else if (side == 1)
    {
        addr = ALBUM_DESIGN_SIDE_1_ADDRESS;
    }

    switch(type)
    {
    case 0:
#ifdef BUILD_EU_VERSION
        load_id = LoadReqLanguage(PL_ALBM_BW_E_PK2, addr);
#else
        load_id = LoadReq(PL_ALBM_BW_PK2, addr);
#endif
    break;
    case 1:
#ifdef BUILD_EU_VERSION
        load_id = LoadReqLanguage(PL_ALBM_BP_E_PK2, addr);
#else
        load_id = LoadReq(PL_ALBM_BP_PK2, addr);
#endif
    break;
    case 2:
#ifdef BUILD_EU_VERSION
        load_id = LoadReqLanguage(PL_ALBM_BR_E_PK2, addr);
#else
        load_id = LoadReq(PL_ALBM_BR_PK2, addr);
#endif
    break;
    case 3:
#ifdef BUILD_EU_VERSION
        load_id = LoadReqLanguage(PL_ALBM_BG_E_PK2, addr);
#else
        load_id = LoadReq(PL_ALBM_BG_PK2, addr);
#endif
    break;
    case 4:
#ifdef BUILD_EU_VERSION
        load_id = LoadReqLanguage(PL_ALBM_BB_E_PK2, addr);
#else
        load_id = LoadReq(PL_ALBM_BB_PK2, addr);
#endif
    break;
    case 5:
#ifdef BUILD_EU_VERSION
        load_id = LoadReqLanguage(PL_ALBM_BO_E_PK2, addr);
#else
        load_id = LoadReq(PL_ALBM_BO_PK2, addr);
#endif
    break;
    default:
        load_id = -1;
    break;
    }

    return load_id;
}

void TitleWaitMode()
{
    int i;
    float alp;
    DISP_SPRT ds;
    int f;

    for (i = 0; i < 11; i++)
    {
        CopySprDToSpr(&ds, &title_sprt[i]);
        DispSprD(&ds);
    }

    if (title_wrk.mode == 3)
    {
        return;
    }

    f = ttl_dsp.timer % 120;
    alp = (SgSinf(f / 120.0f * (PI * 2)) + 1.0f) * 64.0f;

    CopySprDToSpr(&ds, &font_sprt[17]);

    ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    ds.alpha = alp;
    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);

    ttl_dsp.mode = 0;

    if (*key_now[5] == 1 || *key_now[12] == 1)
    {
        ttl_dsp.mode = 1;

        title_wrk.mode = 23;
        title_wrk.csr = 0;

        SeStartFix(1, 0, 0x1000, 0x1000, 0);
    }
}

void TitleStartSlct()
{
    char *str0 = "ZERO HOUR";
    char *str1 = "NEW GAME";
    char *str2 = "LOAD GAME";
    char *str3 = "ALBUM";
    char *str4 = "MISSION";
    char *csr0 = "o";

    SetASCIIString(70.0f, 110.0f, str0);

    SetASCIIString(160.0f, 190.0f, str1);
    SetASCIIString(160.0f, 230.0f, str2);
    SetASCIIString(160.0f, 270.0f, str3);
    SetASCIIString(230.0f, 350.0f, str4);

    SetInteger2(0, 350.0f, 350.0f, 0, 0x80, 0x80, 0x80, ingame_wrk.msn_no);

    SetASCIIString(120.0f, title_wrk.csr * 40 + 190, csr0);

    if (
        *key_now[3] == 1 ||
        (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
        Ana2PadDirCnt(1) == 1 ||
        (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
    )
    {
        ingame_wrk.msn_no++;
    }
    else if (
        *key_now[2] == 1 ||
        (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
        Ana2PadDirCnt(3) == 1 ||
        (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
    )
    {
        if (ingame_wrk.msn_no != 0)
        {
            ingame_wrk.msn_no--;
        }
    }

    if (*key_now[5] == 1 || *key_now[12] == 1)
    {
        if (title_wrk.csr != 0x0)
        {
#ifdef BUILD_EU_VERSION
            title_wrk.load_id = LoadReq(PL_BGBG_PK2, 0x1cfefc0);
            title_wrk.load_id = LoadReqLanguage(PL_STTS_E_PK2, 0x1ce0000);
            title_wrk.load_id = LoadReqLanguage(PL_PSVP_E_PK2, SPRITE_ADDR_4);
            title_wrk.load_id = LoadReqLanguage(PL_SAVE_E_PK2, SPRITE_ADDR_2);
            title_wrk.load_id = LoadReq(SV_PHT_PK2, SPRITE_ADDR_3);
#else
            title_wrk.load_id = LoadReq(PL_BGBG_PK2, 0x1d05140);
            title_wrk.load_id = LoadReq(PL_STTS_PK2, 0x1ce0000);
            title_wrk.load_id = LoadReq(PL_PSVP_PK2, SPRITE_ADDR_4);
            title_wrk.load_id = LoadReq(PL_SAVE_PK2, SPRITE_ADDR_2);
            title_wrk.load_id = LoadReq(SV_PHT_PK2, SPRITE_ADDR_3);
#endif

            title_wrk.mode = 6;

            EAdpcmFadeOut(0x3c);
        }
        else
        {
            NewGameInit();

            title_wrk.mode = 9;
            title_wrk.csr = 0;
        }

        SeStartFix(1, 0, 0x1000, 0x1000, 0);
    }
    else if (*key_now[4] == 1)
    {
        ttl_dsp.timer = 0;
        title_wrk.mode = 2;
        SeStartFix(3, 0, 0x1000, 0x1000, 0);
    }
    else if (
        *key_now[0] == 1 ||
        (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
        Ana2PadDirCnt(0) == 1 ||
        (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
    )
    {
        title_wrk.csr = 0;
        SeStartFix(0, 0, 0x1000, 0x1000, 0);
    }
    else if (
        *key_now[1] == 1 ||
        (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
        Ana2PadDirCnt(2) == 1 ||
        (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
    )
    {
        title_wrk.csr = 1;
        SeStartFix(0, 0, 0x1000, 0x1000, 0);
    }
}

#ifdef BUILD_EU_VERSION
void TitleStartSlctYW(u_char pad_off, u_char alp_max)
{
    int i;
    u_char mode;
    int adj;
    u_char dsp;
    u_char rgb;
    u_char chr;
    u_char alp;
    u_int textbl[4] = { 8, 0, 7, 20 };
    DISP_SPRT ds;

    adj = 28;

    if (
        title_wrk.csr == 3 && (
            *key_now[3] == 1 ||
            (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
            Ana2PadDirCnt(1) == 1 ||
            (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
        )
    )
    {
        ChangeTVMode(1);
        SeStartFix(0, 0, 0x1000, 0x1000, 0);
    } else if (
        title_wrk.csr == 3 && (
            *key_now[2] == 1 ||
            (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
            Ana2PadDirCnt(3) == 1 ||
            (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
        )
    )
    {
        ChangeTVMode(0);
        SeStartFix(0, 0, 0x1000, 0x1000, 0);
    }

    if (ttl_dsp.no_disp == 0)
    {
        for (i = 0; i < 11; i++)
        {
            CopySprDToSpr(&ds, &title_sprt[i]);

            ds.alpha = alp_max;

            DispSprD(&ds);
        }
    }

    for (mode = 0; mode < 4; mode++)
    {
        if (ttl_dsp.no_disp == 0)
        {
            chr = textbl[mode];
            dsp = mode == title_wrk.csr ? 1 : 0;

            CopySprDToSpr(&ds, &font_sprt[chr]);

            ds.y += mode * adj;

            alp = rgb = dsp * (alp_max / 2) + (alp_max / 2);

            ds.alpha = alp;
            ds.r = rgb;
            ds.g = rgb;
            ds.b = rgb;

            if (dsp != 0)
            {
                ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
            }

            ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

            DispSprD(&ds);

            if (mode == 3 && title_wrk.csr == 3)
            {
                dsp = sys_wrk.pal_disp_mode == 0;

                CopySprDToSpr(&ds, &font_sprt[21]);

                ds.y += mode * adj;

                alp = rgb = dsp * (alp_max / 2) + (alp_max / 2);

                ds.alpha = alp;
                ds.r = rgb;
                ds.g = rgb;
                ds.b = rgb;

                if (dsp != 0)
                {
                    ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
                }

                DispSprD(&ds);

                dsp = sys_wrk.pal_disp_mode == 1;

                CopySprDToSpr(&ds, &font_sprt[22]);

                ds.y += mode * adj;

                alp = rgb = dsp * (alp_max / 2) + (alp_max / 2);

                ds.alpha = alp;
                ds.r = rgb;
                ds.g = rgb;
                ds.b = rgb;

                if (dsp)
                {
                    ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
                }

                DispSprD(&ds);
            }
        }
    }

    if (pad_off != 0)
    {
        return;
    }

    if (*key_now[5] == 1 || *key_now[0xc] == 1)
    {
        ingame_wrk.clear_count = 0;
        ingame_wrk.ghost_cnt = 0;
        ingame_wrk.rg_pht_cnt = 0;
        ingame_wrk.pht_cnt = 0;
        ingame_wrk.high_score = 0;
        ingame_wrk.difficult = 0;

        cribo.costume = 0;
        cribo.clear_info = 0;

        NewgameMenuAlbumInit();

        realtime_scene_flg = 0;

        MovieInitWrk();

        motInitMsn();

        switch (title_wrk.csr)
        {
        case 0:
            NewGameInit();

            title_wrk.mode = 9;
            title_wrk.csr = 0;

            SeStartFix(1, 0, 0x1000, 0x1000, 0);
        break;
        case 1:
            title_wrk.load_id = LoadReq(PL_BGBG_PK2, 0x1cfefc0);
            title_wrk.load_id = LoadReqLanguage(PL_STTS_E_PK2, 0x1ce0000);
            title_wrk.load_id = LoadReqLanguage(PL_PSVP_E_PK2, SPRITE_ADDR_4);
            title_wrk.load_id = LoadReqLanguage(PL_SAVE_E_PK2, SPRITE_ADDR_2);
            title_wrk.load_id = LoadReq(SV_PHT_PK2, SPRITE_ADDR_3);

            ingame_wrk.stts |= 0x20;

            InitialDmaBuffer();

            title_wrk.mode = 6;

            SeStartFix(1, 0, 0x1000, 0x1000, 0);
            EAdpcmFadeOut(60);
        break;
        case 2:
            title_wrk.load_id = LoadReq(PL_BGBG_PK2, 0x1cfefc0);
            title_wrk.load_id = LoadReqLanguage(PL_STTS_E_PK2, 0x1ce0000);
            title_wrk.load_id = LoadReqLanguage(PL_PSVP_E_PK2, SPRITE_ADDR_4);
            title_wrk.load_id = LoadReqLanguage(PL_SAVE_E_PK2, SPRITE_ADDR_2);
            title_wrk.load_id = LoadReqLanguage(PL_ALBM_SAVE_E_PK2, SPRITE_ADDR_3);

            ingame_wrk.stts |= 0x20;

            InitialDmaBuffer();

            title_wrk.mode = 14;

            SeStartFix(1, 0, 0x1000, 0x1000, 0);
            EAdpcmFadeOut(60);
        break;
        }
    }
    else if (*key_now[4] == 1)
    {
        ttl_dsp.timer = 0;
        title_wrk.mode = 2;

        SeStartFix(3, 0, 0x1000, 0x1000, 0);
    }
    else if (
        *key_now[1] == 1 ||
        (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
        Ana2PadDirCnt(2) == 1 ||
        (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
    )
    {
        if (title_wrk.csr < 3)
        {
            title_wrk.csr++;
        }
        else
        {
            title_wrk.csr = 0;
        }

        SeStartFix(0, 0, 0x1000, 0x1000, 0);
    }
    else if (
        *key_now[0] == 1 ||
        (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
        Ana2PadDirCnt(0) == 1 ||
        (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
    )
    {
        if (title_wrk.csr > 0)
        {
            title_wrk.csr--;
        }
        else
        {
            title_wrk.csr = 3;
        }

        SeStartFix(0, 0, 0x1000, 0x1000, 0);
    }

    if (ttl_dsp.no_disp != 0)
    {
        ttl_dsp.no_disp--;
    }
}
#else
void TitleStartSlctYW(u_char pad_off, u_char alp_max)
{
    int i;
    u_char mode;
    u_char adj;
    u_char dsp;
    u_char chr1;
    u_char chr2;
    u_char alp;
    u_char rgb;
    DISP_SPRT ds;

    for (i = 0; i < 11; i++)
    {
        CopySprDToSpr(&ds, &title_sprt[i]);

        ds.alpha = alp_max;

        DispSprD(&ds);
    }

    for (mode = 0; mode < 3; mode++)
    {
        switch (mode)
        {
        case 0:
            chr1 = 8;
            chr2 = 4;

            switch (title_wrk.csr)
            {
            case 0:
                adj = 0;
                dsp = 1;
            break;
            case 1:
                adj = 0;
                dsp = 0;
            break;
            case 2:
                adj = 0;
                dsp = 0;
            break;
            }
        break;
        case 1:
            chr1 = 0;
            chr2 = 5;

            switch (title_wrk.csr)
            {
            case 0:
                adj = 35;
                dsp = 0;
            break;
            case 1:
                adj = 35;
                dsp = 1;
            break;
            case 2:
                adj = 35;
                dsp = 0;
            break;
            }
        break;
        case 2:
            chr1 = 7;
            chr2 = 0xff;

            switch (title_wrk.csr)
            {
            case 0:
                adj = 70;
                dsp = 0;
            break;
            case 1:
                adj = 70;
                dsp = 0;
            break;
            case 2:
                adj = 70;
                dsp = 1;
            break;
            }
        break;
        }

        CopySprDToSpr(&ds, &font_sprt[chr1]);

        ds.y += adj;

        alp = rgb = dsp * (alp_max / 2) + (alp_max / 2);

        ds.alpha = alp;
        ds.r = rgb; ds.g = rgb; ds.b = rgb;

        if (dsp != 0)
        {
            ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
        }

        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

        DispSprD(&ds);

        if (chr2 != 0xff)
        {
            CopySprDToSpr(&ds, &font_sprt[chr2]);

            ds.y += adj;

            alp = rgb;

            ds.alpha = alp;
            ds.r = alp; ds.g = alp; ds.b = alp;

            if (dsp != 0)
            {
                ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
            }

            ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

            DispSprD(&ds);
        }
    }

    if (pad_off != 0)
    {
        return;
    }

    if (*key_now[5] == 1 || *key_now[12] == 1)
    {
        ingame_wrk.clear_count = 0;
        ingame_wrk.ghost_cnt = 0;
        ingame_wrk.rg_pht_cnt = 0;
        ingame_wrk.pht_cnt = 0;
        ingame_wrk.high_score = 0;
        ingame_wrk.difficult = 0;

        cribo.costume = 0;
        cribo.clear_info = 0;

        NewgameMenuAlbumInit();

        realtime_scene_flg = 0;

        MovieInitWrk();

        motInitMsn();

        switch (title_wrk.csr)
        {
        case 0:
            NewGameInit();

            title_wrk.mode = 9;
            title_wrk.csr = 0;

            SeStartFix(1, 0, 0x1000, 0x1000, 0);
        break;
        case 1:
            title_wrk.load_id = LoadReq(PL_BGBG_PK2, 0x1d05140);
            title_wrk.load_id = LoadReq(PL_STTS_PK2, 0x1ce0000);
            title_wrk.load_id = LoadReq(PL_PSVP_PK2, SPRITE_ADDR_4);
            title_wrk.load_id = LoadReq(PL_SAVE_PK2, SPRITE_ADDR_2);
            title_wrk.load_id = LoadReq(SV_PHT_PK2, SPRITE_ADDR_3);

            ingame_wrk.stts |= 0x20;

            InitialDmaBuffer();

            title_wrk.mode = 6;

            SeStartFix(1, 0, 0x1000, 0x1000, 0);
            EAdpcmFadeOut(60);
        break;
        case 2:
            title_wrk.load_id = LoadReq(PL_BGBG_PK2, 0x1d05140);
            title_wrk.load_id = LoadReq(PL_STTS_PK2, 0x1ce0000);
            title_wrk.load_id = LoadReq(PL_PSVP_PK2, SPRITE_ADDR_4);
            title_wrk.load_id = LoadReq(PL_SAVE_PK2, SPRITE_ADDR_2);
            title_wrk.load_id = LoadReq(PL_ALBM_SAVE_PK2, SPRITE_ADDR_3);

            ingame_wrk.stts |= 0x20;

            InitialDmaBuffer();

            title_wrk.mode = 14;

            SeStartFix(1, 0, 0x1000, 0x1000, 0);
            EAdpcmFadeOut(60);
        break;
        }
    }
    else if (*key_now[4] == 1)
    {
        ttl_dsp.timer = 0;
        title_wrk.mode = 2;

        SeStartFix(3, 0, 0x1000, 0x1000, 0);
    }
    else if (
        *key_now[1] == 1 ||
        (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
        Ana2PadDirCnt(2) == 1 ||
        (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
    )
    {
        if (title_wrk.csr < 2)
        {
            title_wrk.csr++;
        }
        else
        {
            title_wrk.csr = 0;
        }

        SeStartFix(0, 0, 0x1000, 0x1000, 0);
    }
    else if (
        *key_now[0] == 1 ||
        (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
        Ana2PadDirCnt(0) == 1 ||
        (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
    )
    {
        if (title_wrk.csr > 0)
        {
            title_wrk.csr--;
        }
        else
        {
            title_wrk.csr = 2;
        }

        SeStartFix(0, 0, 0x1000, 0x1000, 0);
    }
}
#endif

void TitleLoadCtrl()
{
    return;
}

void TitleSelectMode()
{
    int i;
    char *mode_str[9] = {
        "STORY MODE",
        "BATTLE MODE",
        "OPTION",
        "MAP DATA EDIT",
        "SOUND TEST",
        "SCENE TEST",
        "MOTION TEST",
        "ROOM SIZE CHECK",
        "LAYOUT TEST",
    };
    char *csr0 = "o";

    for (i = 0; i < 9; i++)
    {
        SetASCIIString(110.0f, 30 + 40 * i, mode_str[i]);
    }

    SetASCIIString(30.0f, 30 + title_wrk.csr * 40, csr0);


    if (
        *key_now[0] == 1 ||
        (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
        Ana2PadDirCnt(0) == 1 ||
        (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
    )
    {
        if (title_wrk.csr > 0)
        {
            title_wrk.csr--;
        }
        else
        {
            title_wrk.csr = 0x8;
        }

        SeStartFix(0, 0, 0x1000, 0x1000, 0);
    }
    else if (
        *key_now[1] == 1 ||
        (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
        Ana2PadDirCnt(2) == 1 ||
        (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
    )
    {
        if (title_wrk.csr < 8)
        {
            title_wrk.csr++;
        }
        else
        {
            title_wrk.csr = 0;
        }

        SeStartFix(0, 0, 0x1000, 0x1000, 0);
    }
    else if (*key_now[4] == 1)
    {
        title_wrk.mode = 3;

        SeStartFix(3, 0, 0x1000, 0x1000, 0);
    }
    else if (*key_now[5] == 1)
    {
        switch(title_wrk.csr)
        {
        case 0:
            ingame_wrk.game = 0;

            GameModeChange(0);
        break;
        case 1:
            OutGameModeChange(6);
        break;
        case 2:
            OutGameModeChange(7);
        break;
        case 5:
            OutGameModeChange(11);
        break;
        case 6:
            OutGameModeChange(12);
        break;
        case 7:
            OutGameModeChange(14);
        break;
        }

        SeStartFix(1, 0, 0x1000, 0x1000, 0);
    }
}

void TitleSelectModeYW(u_char pad_off, u_char alp_max)
{
    int i;
    u_char mode;
    u_char adj;
    u_char dsp;
    u_char chr1;
    u_char chr2;
    u_char alp;
    u_char rgb;
    DISP_SPRT ds;

    for (i = 0; i < 11; i++)
    {
        CopySprDToSpr(&ds, &title_sprt[i]);

        ds.alpha = alp_max;

        DispSprD(&ds);
    }

    for (mode = 0; mode < 2; mode++)
    {
        switch (mode)
        {
        case 0:
            chr1 = 3;
            chr2 = 6;

            switch (title_wrk.csr)
            {
            case 0:
                adj = 17;
                dsp = 1;
            break;
            case 1:
                adj = 17;
                dsp = 0;
            break;
            case 2:
                adj = 17;
                dsp = 0;
            break;
            }
        break;
        case 1:
            chr1 = 9;
            chr2 = 0xff;

            switch (title_wrk.csr)
            {
            case 0:
                adj = 52;
                dsp = 0;
            break;
            case 1:
                adj = 52;
                dsp = 1;
            break;
            case 2:
                adj = 52;
                dsp = 0;
            break;
            }
        break;
        }

        CopySprDToSpr(&ds, &font_sprt[chr1]);

        ds.y += adj;

        alp = rgb = dsp * (alp_max / 2) + (alp_max / 2);

        ds.alpha = alp;
        ds.r = rgb; ds.g = rgb; ds.b = rgb;

        if (dsp != 0)
        {
            ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
        }

        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

        DispSprD(&ds);

        if (chr2 != 0xff)
        {
            CopySprDToSpr(&ds, &font_sprt[chr2]);

            ds.y += adj;

            alp = rgb;

            ds.alpha = alp;
            ds.r = alp; ds.g = alp; ds.b = alp;

            if (dsp != 0)
            {
                ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
            }

            ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

            DispSprD(&ds);
        }
    }

    if (pad_off != 0)
    {
        return;
    }

    if (*key_now[5] == 1 || *key_now[12] == 1)
    {
        switch (title_wrk.csr)
        {
        case 0:
            EAdpcmFadeOut(60);
            SeStartFix(1, 0, 0x1000, 0x1000, 0);

            ingame_wrk.game = 0;

            GameModeChange(0);
        break;
        case 1:
            SeStartFix(1, 0, 0x1000, 0x1000, 0);
            ModeSlctInit(3, 5);

            title_wrk.mode = 27;

            OutGameModeChange(8);
            EAdpcmFadeOut(60);
        break;
        }
    }
    else if (*key_now[4] == 1)
    {
        title_wrk.mode = 23;
        SeStartFix(3, 0, 0x1000, 0x1000, 0);
    }
    else if (
        *key_now[1] == 1 ||
        (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
        Ana2PadDirCnt(2) == 1 ||
        (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
    )
    {
        if (title_wrk.csr == 0)
        {
            title_wrk.csr++;
        }
        else
        {
            title_wrk.csr = 0;
        }

        SeStartFix(0, 0, 0x1000, 0x1000, 0);
    }
    else if (
        *key_now[0] == 1 ||
        (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
        Ana2PadDirCnt(0) == 1 ||
        (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
    )
    {
        if (title_wrk.csr > 0)
        {
            title_wrk.csr--;
        }
        else
        {
            title_wrk.csr = 1;
        }

        SeStartFix(0, 0, 0x1000, 0x1000, 0);
    }
}

void TitleSelectDifficultyYW()
{
    int i;
    u_char chr;
    float alp;
    DISP_SPRT ds;
    int f;

    for (i = 0; i < 11; i++)
    {
        CopySprDToSpr(&ds, &title_sprt[i]);
        DispSprD(&ds);
    }

    CopySprDToSpr(&ds, &font_sprt[16]);

    ds.y += 35.0f;
    ds.alpha = 0x40;
    ds.r = 0x40;
    ds.g = 0x40;
    ds.b = 0x40;
    ds.tex1 = 0x161;

    DispSprD(&ds);

    f = ttl_dsp.timer % 120;
    alp = (SgSinf(f / 120.0f * (PI * 2)) + 1.0f) * 64.0f;

    for (chr = 11; chr < 15; chr++)
    {
        CopySprDToSpr(&ds, &font_sprt[chr]);

        if (chr == 12 || chr == 14)
        {
            ds.att |= 0x2;
        }

        ds.alpha = alp;
        ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

        DispSprD(&ds);
    }

    switch (title_wrk.csr)
    {
    case 0:
        chr = 1;
    break;
    case 1:
        chr = 10;
    break;
    case 2:
        chr = 2;
    break;
    }

    CopySprDToSpr(&ds, &font_sprt[chr]);

    ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);

    if (*key_now[5] == 1 || *key_now[0xc] == 1)
    {
        title_wrk.mode = 9;

        SeStartFix(1, 0, 0x1000, 0x1000, 0);
    }
    else if (*key_now[4] == 1)
    {
        title_wrk.mode = 9;

        SeStartFix(3, 0, 0x1000, 0x1000, 0);
    }
    else if (
        *key_now[3] == 1 ||
        (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
        Ana2PadDirCnt(1) == 1 ||
        (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
    )
    {
        if (title_wrk.csr <= 1)
        {
            title_wrk.csr++;
        }
        else
        {
            title_wrk.csr = 0;
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
        if (title_wrk.csr > 0)
        {
            title_wrk.csr--;
        }
        else
        {
            title_wrk.csr = 0x2;
        }

        SeStartFix(0, 0, 0x1000, 0x1000, 0);
    }
}

void NewGameInit()
{
    if (ttl_dsp.mode != 0)
    {
        ingame_wrk.msn_no = 0;
    }

    sys_wrk.load = 0;

    title_wrk.csr = 0;

    cribo.costume = 0;
    cribo.clear_info = 0;

    mc_msn_flg = 0;

    NewgameMenuRankInit();
    CameraCustomNewgameInit();
    NewgameMenuGlstInit();
    ClearStageWrk();
}

void LoadGameInit()
{
    sys_wrk.load = 1;
    title_wrk.csr = 0;
    ingame_wrk.mode = 6;

    cribo.clear_info &= 0x4;
    cribo.costume = 0;

    LoadgameMenuRankInit();
}

void InitOutDither()
{
    out_dither.cnt = 0.0f;
    out_dither.spd = 8.0f;
    out_dither.alp = 64.0f;
    out_dither.alpmx = 0x40;
    out_dither.colmx = 0x40;
    out_dither.type = 7;
}

void MakeOutDither()
{
    u_char pat[0x4000];
    u_int pal[0x100];
    int i;
    static sceGsLoadImage gs_limage1;
    static sceGsLoadImage gs_limage2;

    SetVURand(rand() / (float)RAND_MAX);

    for (i = 0; i < 0x4000; i++)
    {
        pat[i] = out_dither.alpmx * vu0Rand();
    }

    for (i = 0; i < 0x100; i++)
    {

        pal[i] = (u_char)(out_dither.colmx * vu0Rand());
        pal[i] = 0 \
            | i      << 24 \
            | pal[i] << 16 \
            | pal[i] <<  8 \
            | pal[i] <<  0;
    }

    sceGsSetDefLoadImage(&gs_limage1, 0x37fc, 2, SCE_GS_PSMT8, 0, 0, 128, 128);
    sceGsSetDefLoadImage(&gs_limage2, 0x383c, 1, SCE_GS_PSMCT32, 0, 0, 16, 32);

    FlushCache(0);

    sceGsExecLoadImage(&gs_limage1, (u_long128 *)pat);
    sceGsExecLoadImage(&gs_limage2, (u_long128 *)pal);

    sceGsSyncPath(0, 0);
}

void DispOutDither()
{
    SPRITE_DATA sd2 = {
        .g_GsTex0 = {
              .TBP0 = 0x37FC,
              .TBW = 0x2,
              .PSM = 0x13,
              .TW = 0x7,
              .TH = 0x7,
              .TCC = 0x1,
              .TFX = 0x0,
              .CBP = 0x383C,
              .CPSM = 0x0,
              .CSM = 0x0,
              .CSA = 0x0,
              .CLD = 0x1,
        },
           .g_nTexSizeW = 0x80,
           .g_nTexSizeH = 0x80,
           .g_bMipmapLv = 0,
           .g_GsMiptbp1 = 0,
           .g_GsMiptbp2 = 0,
           .pos_x = -320.5f,
           .pos_y = -224.5f,
           .pos_z = 0xFFFFF000,
           .size_w = 640.0f,
           .size_h = 448.0f,
           .scale_w = 1.0f,
           .scale_h = 1.0f,
           .clamp_u = 0,
           .clamp_v = 0,
           .rot_center = 0,
           .angle = 0.0f,
           .r = 0x80,
           .g = 0x80,
           .b = 0x80,
           .alpha = 0x80,
           .mask = 0x0,
    };
    DRAW_ENV de2 = {
        .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GEQUAL, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GREATER),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 5)
    };

    switch(out_dither.type)
    {
    case 1:
        de2.alpha = 0x44;
    break;
    case 2:
        de2.alpha = 0x48;
    break;
    case 3:
        de2.alpha = 0x41;
    break;
    case 4:
        sd2.r = 0x80;
        sd2.g = 0x0;
        sd2.b = 0x80;
        de2.alpha = 0x41;
    break;
    case 5:
        sd2.r = 0x80;
        sd2.g = 0x80;
        sd2.b = 0x0;
        de2.alpha = 0x41;
    break;
    case 6:
        de2.alpha = 0x49;
    break;
    case 7:
        de2.alpha = 0x42;
    break;
    }

    sd2.clamp_u = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 40, 0);
    sd2.clamp_v = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 32, 0);
    sd2.alpha = (SgSinf(DEG2RAD(out_dither.cnt)) + 1.0f) * out_dither.alp;
    SetTexDirectS2(-2, &sd2, &de2, 0);

    sd2.clamp_u = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 64, 0, 44, 0);
    sd2.clamp_v = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 32, 0);
    sd2.alpha = (SgSinf(DEG2RAD(out_dither.cnt + 120.0f)) + 1.0f) * out_dither.alp;
    SetTexDirectS2(-2, &sd2, &de2, 0);

    sd2.clamp_u = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 40, 0);
    sd2.clamp_v = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 64, 0, 36, 0);
    sd2.alpha = (SgSinf(DEG2RAD(out_dither.cnt + 240.0f)) + 1.0f) * out_dither.alp;
    SetTexDirectS2(-2, &sd2, &de2, 0);

    out_dither.cnt += 8.0f;
    out_dither.spd = 8.0f;
}

#ifdef BUILD_EU_VERSION
void InitTecmotLogo()
{
    title_sys.logo_flow = 0;
    title_sys.cnt = 0;
    title_sys.alp = 0;
}

int SetTecmoLogo() {
    DISP_SPRT ds;
    int sec1;
    int sec2;
    int sec3;
    int sec4;
    int n;

    sec1 = 60;
    sec2 = 90;
    sec3 = 60;
    sec4 = 30;

    switch (title_sys.logo_flow)
    {
    case 0:
        title_sys.cnt = 0;
        title_sys.alp = 0;
        title_sys.logo_flow = 1;
    case 1:
        title_sys.alp = title_sys.cnt * 128 / sec1;
        title_sys.cnt++;
        if (title_sys.cnt >= sec1)
        {
            title_sys.cnt = 0;
            title_sys.logo_flow = 2;
        }
    break;
    case 2:
        title_sys.alp = 128;
        title_sys.cnt++;
        if (title_sys.cnt >= sec2)
        {
            title_sys.cnt = 0;
            title_sys.logo_flow = 3;
        }
    break;
    case 3:
        title_sys.alp = (sec3 - title_sys.cnt) * 128 / sec3;
        title_sys.cnt++;
        if (title_sys.cnt >= sec3)
        {
            title_sys.logo_flow = 4;
            title_sys.cnt = 0;
        }
    break;
    case 4:
        title_sys.alp = 0;
        title_sys.cnt++;
        if (title_sys.cnt >= sec4) {
            title_sys.cnt = 0;
            title_sys.alp = 0;
            title_sys.logo_flow = 5;
        }
    break;
    case 5:
        title_sys.alp = title_sys.cnt * 128 / sec1;
        title_sys.cnt++;
        if (title_sys.cnt >= sec1)
        {
            title_sys.cnt = 0;
            title_sys.logo_flow = 6;
        }
    break;
    case 6:
        title_sys.alp = 128;
        title_sys.cnt++;
        if (title_sys.cnt >= sec2)
        {
            title_sys.cnt = 0;
            title_sys.logo_flow = 7;
        }
    break;
    case 7:
        title_sys.alp = (sec3 - title_sys.cnt) * 128 / sec3;
        title_sys.cnt++;
        if (title_sys.cnt >= sec3)
        {
            title_sys.cnt = 0;
            title_sys.logo_flow = 8;
        }
    break;
    case 8:
        return 1;
    default:
    break;
    }

    SetSprFile3(0x1e90000, 0);

    n = title_sys.logo_flow >= 0 && title_sys.logo_flow < 4 ? 0 : 7;

    CopySprDToSpr(&ds, &logotex[n]);


    ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    ds.pri = logotex[n].pri;
    ds.z = 0x0fffffff - logotex[n].pri;
    ds.x = logotex[n].x;
    ds.y = logotex[n].y;
    ds.r = 0x60;
    ds.g = 0x60;
    ds.b = 0x60;
    ds.alpha = title_sys.alp;

    DispSprD(&ds);

    return 0;
}

void InitSelectLanguage()
{
    title_sys.lang_sel_flow = 0;
    title_sys.cnt = 0;
    title_sys.alp = 0;
}

int SetSelectLanguage(int cur_pos)
{
    int i;
    DISP_SPRT ds;
    int n;

    switch(title_sys.lang_sel_flow)
    {
    case 0:
        title_sys.load_id = LoadReq(LOGO_PK2, 0x1e90000);
        title_sys.lang_sel_flow = 1;
    case 1:
        if (IsLoadEnd(title_sys.load_id) != 0)
        {
            title_sys.alp = 0;
            title_sys.cnt = 0;
            title_sys.lang_sel_flow = 2;
        }
    break;
    case 2:
        if (title_sys.alp + 0x10 < 0x80)
        {
            title_sys.alp += 0x10;
        }
        else
        {
            title_sys.alp = 0x80;
            title_sys.lang_sel_flow = 3;
        }
    break;
    case 3:
        title_sys.alp = 0x80;
    break;
    case 4:
        if (title_sys.alp - 0x10 > 0)
        {
            title_sys.alp -= 0x10;
        }
        else
        {
            if (IsLoadEnd(init_load_id) != 0)
            {
                title_sys.lang_sel_flow = 5;
                title_sys.alp = 0;
            }
        }
    break;
    case 5:
        // do nothing ...
    break;
    }

    if (title_sys.lang_sel_flow > 1)
    {
        if (title_sys.lang_sel_flow == 3)
        {
            if (*key_now[1] == 1)
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);

                if (sys_wrk.language < 4)
                {
                    sys_wrk.language++;
                }
                else
                {
                    sys_wrk.language = 0;
                }
            }
            else if (*key_now[0] == 1)
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);

                if (sys_wrk.language > 0)
                {
                    sys_wrk.language--;
                }
                else
                {
                    sys_wrk.language = 4;
                }
            }
            else if (*key_now[5] == 1)
            {
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

                title_sys.lang_sel_flow = 4;

                mc_language = mc_language | sys_wrk.language;

                init_load_id = LoadReqLanguage(IG_MSG_E_OBJ, 0x84a000);
            }
        }

        SetSprFile3(0x1e90000, 0);

        n = 1;

        CopySprDToSpr(&ds, &logotex[n]);

        ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
        ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
        ds.pri = logotex[n].pri;
        ds.z = 0x0fffffff - logotex[n].pri;
        ds.x = logotex[n].x;
        ds.y = logotex[n].y;
        ds.r = 0x80;
        ds.g = 0x80;
        ds.b = 0x80;
        ds.alpha = title_sys.alp;

        DispSprD(&ds);

        for (i = 0; i < 5; i++)
        {
            n = i + 2;

            CopySprDToSpr(&ds, &logotex[n]);

            ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
            ds.x = 0x140 - (int)(logotex[n].w / 2);
            ds.y = 0x20 + (i) * 0x50;

            if (cur_pos == i)
            {
                ds.r = 0x80;
                ds.g = 0x80;
                ds.b = 0x80;
            }
            else
            {
                ds.r = 0x20;
                ds.g = 0x20;
                ds.b = 0x20;
            }

            ds.alpha = title_sys.alp;

            DispSprD(&ds);
        }
    }

    return title_sys.lang_sel_flow == 5;
}
#endif
