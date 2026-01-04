#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ev_spcl.h"

// #include "graphics/graph2d/effect_scr.h" // SetBlackIn, SetBlackOut
#include "graphics/graph2d/message.h"
#include "graphics/graph2d/sprt.h"
// #include "graphics/graph2d/tim2.h" // SetSprFile
#include "graphics/graph3d/sglib.h"
// #include "graphics/motion/mdlwork.h" // SendManMdlTex
// #include "ingame/camera/camera.h" // ReqDramaCamera
#include "ingame/event/ev_main.h"
#include "ingame/event/ev_spcl.h"
#include "ingame/ig_glob.h"
#include "ingame/map/door_ctl.h"
// #include "ingame/map/furn_ctl.h" // SetFurnAttrEve
// #include "ingame/map/rotd_ctl.h" // ChangeRotDoorFace2Room
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/item.h"
#include "ingame/plyr/plyr_ctl.h"
#include "main/glob.h"
#include "os/eeiop/adpcm/ea_puzzle.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"
#include "os/fileload.h"

void (*SpecialEventInitTbl[])() = {
    SpecialEventInit000, SpecialEventInit001, SpecialEventInit002, SpecialEventInit003, SpecialEventInit004,
    SpecialEventInit005, SpecialEventInit006, SpecialEventInit007, SpecialEventInit008, SpecialEventInit009,
    SpecialEventInit010, SpecialEventInit011, SpecialEventInit012, SpecialEventInit013, SpecialEventInit014,
    SpecialEventInit015, SpecialEventInit016, SpecialEventInit017, SpecialEventInit018, SpecialEventInit019,
    SpecialEventInit020, SpecialEventInit021, SpecialEventInit022, SpecialEventInit023, SpecialEventInit024,
    SpecialEventInit025, SpecialEventInit026, SpecialEventInit027, SpecialEventInit028, SpecialEventInit029,
    SpecialEventInit030, SpecialEventInit031, SpecialEventInit032, SpecialEventInit033, SpecialEventInit034,
    SpecialEventInit035, SpecialEventInit036, SpecialEventInit037, SpecialEventInit038, SpecialEventInit039,
    SpecialEventInit040, SpecialEventInit041, SpecialEventInit042, SpecialEventInit043, SpecialEventInit044,
    SpecialEventInit045, SpecialEventInit046, SpecialEventInit047, SpecialEventInit048, SpecialEventInit049,
    SpecialEventInit050, SpecialEventInit051, SpecialEventInit052, SpecialEventInit053, SpecialEventInit054,
    SpecialEventInit055, SpecialEventInit056, SpecialEventInit057, SpecialEventInit058, SpecialEventInit059,
    SpecialEventInit060, SpecialEventInit061, SpecialEventInit062, SpecialEventInit063, SpecialEventInit064,
    SpecialEventInit065, SpecialEventInit066, SpecialEventInit067, SpecialEventInit068, SpecialEventInit069,
    SpecialEventInit070, SpecialEventInit071, SpecialEventInit072, SpecialEventInit073, SpecialEventInit074,
    SpecialEventInit075, SpecialEventInit076, SpecialEventInit077, SpecialEventInit078, SpecialEventInit079,
    SpecialEventInit080, SpecialEventInit081, SpecialEventInit082, SpecialEventInit083, SpecialEventInit084,
    SpecialEventInit085, SpecialEventInit086, SpecialEventInit087, SpecialEventInit088, SpecialEventInit089,
    SpecialEventInit090,
};
void (*SpecialEventMainTbl[])() = {
    SpecialEventMain000, SpecialEventMain001, SpecialEventMain002, SpecialEventMain003, SpecialEventMain004,
    SpecialEventMain005, SpecialEventMain006, SpecialEventMain007, SpecialEventMain008, SpecialEventMain009,
    SpecialEventMain010, SpecialEventMain011, SpecialEventMain012, SpecialEventMain013, SpecialEventMain014,
    SpecialEventMain015, SpecialEventMain016, SpecialEventMain017, SpecialEventMain018, SpecialEventMain019,
    SpecialEventMain020, SpecialEventMain021, SpecialEventMain022, SpecialEventMain023, SpecialEventMain024,
    SpecialEventMain025, SpecialEventMain026, SpecialEventMain027, SpecialEventMain028, SpecialEventMain029,
    SpecialEventMain030, SpecialEventMain031, SpecialEventMain032, SpecialEventMain033, SpecialEventMain034,
    SpecialEventMain035, SpecialEventMain036, SpecialEventMain037, SpecialEventMain038, SpecialEventMain039,
    SpecialEventMain040, SpecialEventMain041, SpecialEventMain042, SpecialEventMain043, SpecialEventMain044,
    SpecialEventMain045, SpecialEventMain046, SpecialEventMain047, SpecialEventMain048, SpecialEventMain049,
    SpecialEventMain050, SpecialEventMain051, SpecialEventMain052, SpecialEventMain053, SpecialEventMain054,
    SpecialEventMain055, SpecialEventMain056, SpecialEventMain057, SpecialEventMain058, SpecialEventMain059,
    SpecialEventMain060, SpecialEventMain061, SpecialEventMain062, SpecialEventMain063, SpecialEventMain064,
    SpecialEventMain065, SpecialEventMain066, SpecialEventMain067, SpecialEventMain068, SpecialEventMain069,
    SpecialEventMain070, SpecialEventMain071, SpecialEventMain072, SpecialEventMain073, SpecialEventMain074,
    SpecialEventMain075, SpecialEventMain076, SpecialEventMain077, SpecialEventMain078, SpecialEventMain079,
    SpecialEventMain080, SpecialEventMain081, SpecialEventMain082, SpecialEventMain083, SpecialEventMain084,
    SpecialEventMain085, SpecialEventMain086, SpecialEventMain087, SpecialEventMain088, SpecialEventMain089,
    SpecialEventMain090,
};
#include "data/spev00_sp_bak.h" // SPRT_SDAT spev00_sp_bak[];
#include "data/spev00_sp_bff.h" // SPRT_SDAT spev00_sp_bff[];
#include "data/spev00_sp_bft.h" // SPRT_SDAT spev00_sp_bft[];
#include "data/spev00_sp_stn.h" // SPRT_SDAT spev00_sp_stn[];
#include "data/spev00_stn_pos.h" // short int spev00_stn_pos[][2];
#include "data/spev00_sp_lin.h" // SPRT_SDAT spev00_sp_lin[];
#include "data/spev00_lin_rot.h" // SPRT_SROT spev00_lin_rot[];
#include "data/spev00_lin_scl.h" // SPRT_SSCL spev00_lin_scl[];
#include "data/spev00_sp_num.h" // SPRT_SDAT spev00_sp_num[];
#include "data/spev00_sp_sta.h" // SPRT_SDAT spev00_sp_sta[];
#include "data/spev00_sta_pos.h" // short int spev00_sta_pos[][2];
#include "data/spev00_sp_lia.h" // SPRT_SDAT spev00_sp_lia[];
#include "data/spev00_lia_rot.h" // SPRT_SROT spev00_lia_rot[];
#include "data/spev00_lia_scl.h" // SPRT_SSCL spev00_lia_scl[];
#include "data/spev00_sp_hln.h" // SPRT_SDAT spev00_sp_hln[];
#include "data/spev00_sp_csr.h" // SPRT_SDAT spev00_sp_csr[];
#include "data/spev00_csr_scl.h" // SPRT_SSCL spev00_csr_scl[];
#include "data/spev00_sp_cpf.h" // SPRT_SDAT spev00_sp_cpf[];
#include "data/spev00_sp_cpf2.h" // SPRT_SDAT spev00_sp_cpf2[];
#include "data/spev00_cbt_pos.h" // short int spev00_cbt_pos[][3];
#include "data/spev00_sp_cap.h" // SPRT_SDAT spev00_sp_cap[];
#include "data/spev01_sp_bak.h" // SPRT_SDAT spev01_sp_bak[];
#include "data/spev01_sp_gl3.h" // SPRT_SDAT spev01_sp_gl3[];
#include "data/spev01_sp_gl5.h" // SPRT_SDAT spev01_sp_gl5[];
#include "data/spev01_sp_btn.h" // SPRT_SDAT spev01_sp_btn[];
#include "data/spev01_sp_btp.h" // SPRT_SDAT spev01_sp_btp[];
#include "data/spev01_sp_btf.h" // SPRT_SDAT spev01_sp_btf[];
#include "data/spev01_sp_num.h" // SPRT_SDAT spev01_sp_num[];
#include "data/spev01_nm3_pos.h" // short int spev01_nm3_pos[][2];
#include "data/spev01_nm4_pos.h" // short int spev01_nm4_pos[][2];
#include "data/spev01_nm5_pos.h" // short int spev01_nm5_pos[][2];
#include "data/spev01_sp_cpf.h" // SPRT_SDAT spev01_sp_cpf[];
#include "data/spev01_cbt_pos.h" // short int spev01_cbt_pos[][2];
#include "data/spev01_sp_cap.h" // SPRT_SDAT spev01_sp_cap[];
#include "data/spev02_sp_bak.h" // SPRT_SDAT spev02_sp_bak[];
#include "data/spev02_sp_of1.h" // SPRT_SDAT spev02_sp_of1[];
#include "data/spev02_sp_of3.h" // SPRT_SDAT spev02_sp_of3[];
#include "data/spev03_sp_bak.h" // SPRT_SDAT spev03_sp_bak[];
#include "data/spev03_sp_dol.h" // SPRT_SDAT spev03_sp_dol[];
#include "data/spev03_dol_scl.h" // SPRT_SSCL spev03_dol_scl[];
#include "data/spev03_sp_dla.h" // SPRT_SDAT spev03_sp_dla[];
#include "data/spev03_dla_scl.h" // SPRT_SSCL spev03_dla_scl[];
#include "data/spev03_sp_cap.h" // SPRT_SDAT spev03_sp_cap[];
#ifdef BUILD_EU_VERSION
#include "data/spev03_cbt_pos.h" // short int spev03_cbt_pos[][2];
#endif
#include "data/spev04_sp_bk0.h" // SPRT_SDAT spev04_sp_bk0[];
#include "data/spev04_sp_bk1.h" // SPRT_SDAT spev04_sp_bk1[];
#include "data/spev04_sp_bk2.h" // SPRT_SDAT spev04_sp_bk2[];
#include "data/spev04_sp_bk3.h" // SPRT_SDAT spev04_sp_bk3[];
#include "data/spev04_sp_bd1.h" // SPRT_SDAT spev04_sp_bd1[];
#include "data/spev04_sp_bzf.h" // SPRT_SDAT spev04_sp_bzf[];
#include "data/spev04_sp_bld.h" // SPRT_SDAT spev04_sp_bld[];
#include "data/spev04_sp_cap.h" // SPRT_SDAT spev04_sp_cap[];
#include "data/spev04_cbt_pos.h" // short int spev04_cbt_pos[][2];
#include "data/spev04_sp_slf.h" // SPRT_SDAT spev04_sp_slf[];
#include "data/spev04_sp_sel.h" // SPRT_SDAT spev04_sp_sel[];
#include "data/spev04_bd1_scl.h" // SPRT_SSCL spev04_bd1_scl[];
#include "data/spev04_bz2_scl.h" // SPRT_SSCL spev04_bz2_scl[];
#include "data/spev04_sel_scl.h" // SPRT_SSCL spev04_sel_scl[];
#include "data/spev04_sp_bd2.h" // SPRT_SDAT spev04_sp_bd2[];
#include "data/spev04_sp_cpt.h" // SPRT_SDAT spev04_sp_cpt[];
#include "data/spev20_sp_bak.h" // SPRT_SDAT spev20_sp_bak[];
#include "data/spev20_sp_cdl.h" // SPRT_SDAT spev20_sp_cdl[];
#include "data/spev20_sp_cla.h" // SPRT_SDAT spev20_sp_cla[];
#include "data/spev20_sp_cpf.h" // SPRT_SDAT spev20_sp_cpf[];
#include "data/spev20_sp_cfa.h" // SPRT_SDAT spev20_sp_cfa[];
#include "data/spev20_sp_cdl0.h" // SPRT_SDAT spev20_sp_cdl0[];
#include "data/spev20_sp_cdl1.h" // SPRT_SDAT spev20_sp_cdl1[];
#include "data/spev20_sp_cdl2.h" // SPRT_SDAT spev20_sp_cdl2[];
#include "data/spev20_sp_cdl3.h" // SPRT_SDAT spev20_sp_cdl3[];
#include "data/spev20_sp_cdl5.h" // SPRT_SDAT spev20_sp_cdl5[];
#include "data/spev20_sp_cdl4.h" // SPRT_SDAT spev20_sp_cdl4[];
#include "data/pzl_men_bg.h" // SPRT_SDAT pzl_men_bg[];
#include "data/spev21_sp_menz.h" // SPRT_SDAT spev21_sp_menz[];
#include "data/spev21_sp_bak.h" // SPRT_SDAT spev21_sp_bak[];
#include "data/spev21_sp_zft.h" // SPRT_SDAT spev21_sp_zft[];
#include "data/spev21_sp_zfl.h" // SPRT_SDAT spev21_sp_zfl[];
#include "data/spev21_sp_zbt.h" // SPRT_SDAT spev21_sp_zbt[];
#include "data/spev21_sp_cpf.h" // SPRT_SDAT spev21_sp_cpf[];
#include "data/spev21_sp_cap.h" // SPRT_SDAT spev21_sp_cap[];
#include "data/spev_str.h" // STR_DAT spev_str;
#include "data/spev_sq0.h" // SQAR_DAT spev_sq0;
#include "data/star_pzl_dat.h" // STAR_PZL_DAT star_pzl_dat[];
#include "data/dkey_dat.h" // DIAL_KEY_DAT dkey_dat[];
#include "data/pghost_no_dat.h" // u_char pghost_no_dat[];
#include "data/evdl_dat.h" // EV_DOLL_DAT evdl_dat[];
#include "data/evbtz_dat.h" // EV_BTZ_DAT evbtz_dat[5];
#include "data/evbtz_ans.h" // EV_BTZ_DAT evbtz_ans[5];
#include "data/evbld_dat.h" // EV_BLD_DAT evbld_dat;
#include "data/spev04_sel_scl2.h" // SPRT_SSCL spev04_sel_scl2[];
#include "data/evcdl_dat.h" // EV_CDL_DAT evcdl_dat;
#include "data/spev20_cbt_pos.h" // static short int spev20_cbt_pos[][2];
#include "data/spev20_sp_flm.h" // static short int spev20_sp_flm[][6];
#include "data/spev20_sp_sflm.h" // static short int spev20_sp_sflm[][6];
#include "data/id_table.h" // static short int id_table[][2];
#include "data/dmodel_table.h" // static short int dmodel_table[][2];
#include "data/doorid_table.h" // static short int doorid_table[][2];
#include "data/camera_table.h" // static short int camera_table[];
STAR_PZL_WRK star_pzl_wrk = {0};
DIAL_KEY_WRK dkey_wrk = {0};

SPRT_SROT spev03_dol_left = {
    .cx = 0x7fff,
    .cy = 2,
    .rot = 0.0f,
};
static int pzl_load_id = 0;

static char order_keep[5];
static char order_no;
static u_char butsuzo_alpha[3];
static u_char zushi_alpha[3];

#define PI 3.1415927f
#define DEG2RAD(x) ((float)(x)*PI/180.0f)

#define BASE_ADDRESS 0x7f0000

#define EVENT_ADDRESS 0x1e90000
#define EVENT_ADDRESS_2 0x1ed8f10

void SpecialEventInit(u_char spev_no)
{
    spev_wrk = (SPECIAL_EVENT_WRK){0};

    spev_wrk.no = spev_no;

    SpecialEventInitTbl[spev_no]();
}

void SpecialEventMain()
{
    SpecialEventMainTbl[spev_wrk.no]();

    if (spev_wrk.ret != 0)
    {
        ingame_wrk.mode = 6;

        ev_wrk.evt_no = 0xFF;
        ev_wrk.mode = 0;
    }
}

int GetSpecialEventMessageAddr(short int msg_no)
{
    int addr;

    addr = Get4Byte((u_char *)(BASE_ADDRESS + 0x10));
    addr = Get4Byte((u_char *)(BASE_ADDRESS + addr + msg_no * 4));

    return BASE_ADDRESS + addr;
}

void SimpleDispSprt(SPRT_SDAT *ssd, u_int addr, int sp_no, SPRT_SROT *srot, SPRT_SSCL *sscl, u_char alp_rate)
{
    DISP_SPRT ds;
    SPRT_DAT sd;

    sd.tex0 = GetTex0Reg(addr, sp_no, 0);

    sd.u = ssd->u;
    sd.v = ssd->v;

    sd.w = ssd->w;
    sd.h = ssd->h;

    sd.x = ssd->x;
    sd.y = ssd->y;

    sd.pri = ssd->pri << 12;

    if (alp_rate == 0xFF)
    {
        ds.alphar = 72;
    }
    else
    {
        sd.alpha = (ssd->alp * alp_rate) / 100;
    }

    CopySprDToSpr(&ds, &sd);

    if (srot != NULL)
    {
        if (srot->cx != 0x7FFF)
        {
            ds.rot = srot->rot;

            ds.crx = srot->cx;
            ds.cry = srot->cy;
        }
        else
        {
            ds.att |= srot->cy;
        }
    }

    if (sscl != NULL)
    {
        ds.scw = sscl->sw / 100.0f;
        ds.sch = sscl->sh / 100.0f;

        ds.csx = sscl->cx;
        ds.csy = sscl->cy;
    }

    DispSprD(&ds);
}

void SimpleDispAlphaSprt(SPRT_SDAT *ssd, u_int addr, int sp_no, u_char alp_rate, u_char alp_type)
{
    DISP_SPRT ds;
    SPRT_DAT sd;

    sd.tex0 = GetTex0Reg(addr, sp_no, 0);

    sd.u = ssd->u;
    sd.v = ssd->v;

    sd.w = ssd->w;
    sd.h = ssd->h;

    sd.x = ssd->x;
    sd.y = ssd->y;

    sd.pri = ssd->pri << 12;

    if (alp_type != 0)
    {
        ds.alphar = 72;
    }
    else
    {
        ds.alphar = 68;
    }

    sd.alpha = (ssd->alp * alp_rate) / 100;

    CopySprDToSpr(&ds, &sd);

    DispSprD(&ds);
}

void SimpleDispSprtRGB(SPRT_SDAT *ssd, u_int addr, int sp_no, SPRT_SROT *srot, SPRT_SSCL *sscl, u_char alp_rate, u_char rr, u_char gg, u_char bb)
{
    DISP_SPRT ds;
    SPRT_DAT sd;

    sd.tex0 = GetTex0Reg(addr, sp_no, 0);
    sd.u = ssd->u;
    sd.v = ssd->v;
    sd.w = ssd->w;
    sd.h = ssd->h;
    sd.x = ssd->x;
    sd.y = ssd->y;
    sd.pri = ssd->pri << 12;
    sd.alpha = ((ssd->alp * alp_rate) / 100);

    CopySprDToSpr(&ds,&sd);

    if (srot != NULL)
    {
        if (srot->cx != 0x7fff)
        {
            ds.rot = srot->rot;
            ds.crx = srot->cx;
            ds.cry = srot->cy;
        }
        else
        {
            ds.att |= srot->cy;
        }
    }

    if (sscl != NULL)
    {
        ds.scw = sscl->sw / 100.0f;
        ds.sch = sscl->sh / 100.0f;
        ds.csx = sscl->cx;
        ds.csy = sscl->cy;
    }

    ds.r = rr;
    ds.g = gg;
    ds.b = bb;

    DispSprD(&ds);
}

void SimpleDispSprtLNR(SPRT_SDAT *ssd, u_int addr, int sp_no, SPRT_SROT *srot, SPRT_SSCL *sscl, u_char alp_rate, int lnr)
{
    DISP_SPRT ds;
    SPRT_DAT sd;

    sd.tex0 = GetTex0Reg(addr, sp_no, 0);

    sd.u = ssd->u;
    sd.v = ssd->v;

    sd.w = ssd->w;
    sd.h = ssd->h;

    sd.x = ssd->x;
    sd.y = ssd->y;

    sd.pri = ssd->pri << 12;

    if (alp_rate == 0xff)
    {
        ds.alphar = 72;
    }
    else
    {
        sd.alpha = (ssd->alp * alp_rate) / 100;
    }

    CopySprDToSpr(&ds, &sd);

    if (srot != NULL)
    {
        if (srot->cx != 0x7fff)
        {
            ds.rot = srot->rot;

            ds.crx = srot->cx;
            ds.cry = srot->cy;
        }
        else
        {
            ds.att |= srot->cy;
        }
    }

    if (sscl != NULL)
    {
        ds.scw = sscl->sw / 100.0f;
        ds.sch = sscl->sh / 100.0f;

        ds.csx = sscl->cx;
        ds.csy = sscl->cy;
    }

    if (lnr != 0)
    {
        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    }
    else
    {
        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_NEAREST, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    }

    DispSprD(&ds);
}

void SimpleDispSprtDatCopy(SPRT_SDAT *org, SPRT_SDAT *cpy)
{
    cpy->u = org->u;
    cpy->v = org->v;

    cpy->w = org->w;
    cpy->h = org->h;

    cpy->x = org->x;
    cpy->y = org->y;

    cpy->pri = org->pri;
    cpy->alp = org->alp;
}

void TestPk2Data(long int sendtexaddr)
{
    static int ttest_count = 0;
    SPRT_SDAT ssd;

    if (*key_now[8] == 1)
    {
        ttest_count++;
    }

    if (*key_now[9] == 1)
    {
        ttest_count--;
    }

    SimpleDispSprtDatCopy((SPRT_SDAT*)&spev20_sp_cdl1, &ssd);

    ssd.u = 1;
    ssd.v = 1;

    ssd.w = 680;
    ssd.h = 512;

    ssd.x = 0;
    ssd.y = 0;

    ssd.pri = 0;

    SimpleDispSprt(&ssd, sendtexaddr, ttest_count, NULL, NULL, 100);
}

static int button_time = 0;
static int spev_alp_cnt = 0;

int ButtonMarkNext(int x_off, int y_off, int se_flg)
{
    u_char alp;

    alp = (SgCosf(DEG2RAD(spev_alp_cnt * 6.0f)) + 1.0f) * 64.0f;

    if (alp == 0)
    {
        alp = 1;
    }

    DrawButtonTex(0x1000, 3, x_off + 550, y_off + 386, alp);

    if (++spev_alp_cnt > 0xfffffff0)
    {
        spev_alp_cnt = 0;
    }

    if ((*key_now[5] == 1) && (button_time == 0))
    {
        button_time = 20;

        if (se_flg == 1)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);
        }
    }

    return button_time;
}

int ButtonMarkWait()
{
    if (button_time > 0)
    {
        button_time--;
    }

    return button_time;
}

void ButtonMarkTimeClear()
{
    if (button_time > 0)
    {
        button_time = 0;
    }
}

int CsrInclease(u_char *csr_idx, u_char alpha_max, u_char inclease)
{
    int ret_num;

    ret_num = 0;

    if (alpha_max - inclease <= *csr_idx)
    {
        ret_num = 1;
    }
    else
    {
        *csr_idx = *csr_idx + inclease;

        if (alpha_max - inclease <= *csr_idx)
        {
            *csr_idx = alpha_max;

            ret_num = 1;
        }
    }

    return ret_num;
}

int CsrDeclease(u_char *csr_idx, u_char alpha_min, int inclease)
{
    int ret_num;

    ret_num = 0;

    if (alpha_min + inclease >= *csr_idx)
    {
        ret_num = 1;
    }
    else
    {
        *csr_idx = *csr_idx - inclease;

        if (alpha_min + inclease >= *csr_idx)
        {
            *csr_idx = alpha_min;

            ret_num = 1;
        }
    }

    return ret_num;
}

int CsrBlink(u_char *csr_idx, u_char alpha_max, u_char alpha_min, u_char inclease, u_char *blink)
{
    int ret_num;

    ret_num = 0;

    if (*blink == 0)
    {
        if (*csr_idx < alpha_max - inclease)
        {
            *csr_idx = inclease + *csr_idx;

            if (alpha_max - inclease <= *csr_idx)
            {
                *csr_idx = alpha_max;
            }
        }
        else
        {
            *blink = 1;
        }
    }
    else
    {
        if (alpha_min + inclease < *csr_idx)
        {
            *csr_idx = *csr_idx - inclease;

            if (alpha_min + inclease >= *csr_idx)
            {
                *csr_idx = alpha_min;

                ret_num = 1;
            }
        }
        else
        {
            *blink = 0;

            ret_num = 1;
        }

    }

    return ret_num;
}

void CsrClear(u_char *csr_idx)
{
    *csr_idx = 0;
}

void CsrClearAll()
{
    spev_wrk.csr[0] = 0;
    spev_wrk.csr[1] = 0;
    spev_wrk.csr[2] = 0;
}

void SpevStrInit()
{
    spev_str.r = 0x80;
    spev_str.g = 0x80;
    spev_str.b = 0x80;

    spev_str.alpha = 0x80;
}

void SpevWrkInit()
{
    spev_wrk.csr[0] = 0;
    spev_wrk.csr[1] = 0;
    spev_wrk.csr[2] = 0;

    spev_wrk.count = 0;

    spev_wrk.time = 0;
}

void SpevSelectYesNoCsr(float pos_x, float pos_y, int pri, float alp)
{
    YesNoCrslOKR((u_char)pri * 0x1000, pos_x, pos_y, 0x808080, 128.0f, 2.0f);
}

int DeadlySeStopWait()
{
    static u_char dead_time = 0;

    if (plyr_wrk.se_deadly != -1)
    {
        dead_time = 60;

        SeFadeFlame(plyr_wrk.se_deadly, 60, 0);

        plyr_wrk.se_deadly = -1;
    }
    else if (dead_time != 0)
    {
        dead_time--;
    }

    return dead_time;
}

void DummyProg()
{
    spev_wrk.ret = 1;
}

void StarPuzzleInit(int pzl_no)
{
    if (plyr_wrk.se_deadly != -1)
    {
        SeFadeFlame(plyr_wrk.se_deadly,0x3c,0);
        plyr_wrk.se_deadly = -1;
        star_pzl_wrk.time = 60;
        star_pzl_wrk.mode = 0;
    }
    else
    {
        if (pzl_no == 4)
        {
            LoadReq(EVTEX00G_PK2, EVENT_ADDRESS);
        }
        else if (pzl_no == 9)
        {
            LoadReq(EVTEX00F_PK2, EVENT_ADDRESS);
        }
        else if (pzl_no == 12)
        {
#ifdef BUILD_EU_VERSION
            LoadReqLanguage(EVTEX00_E_PK2, EVENT_ADDRESS);
#else
            LoadReq(EVTEX00_PK2, EVENT_ADDRESS);
#endif
        }
        else if (pzl_no == 18)
        {
#ifdef BUILD_EU_VERSION
            LoadReqLanguage(EVTEX00B_E_PK2, EVENT_ADDRESS);
#else
            LoadReq(EVTEX00B_PK2, EVENT_ADDRESS);
#endif
        }
        else if (pzl_no == 20)
        {
#ifdef BUILD_EU_VERSION
            LoadReqLanguage(EVTEX00D_E_PK2, EVENT_ADDRESS);
#else
            LoadReq(EVTEX00D_PK2, EVENT_ADDRESS);
#endif
        }
        else if (pzl_no == 36)
        {
#ifdef BUILD_EU_VERSION
            LoadReqLanguage(EVTEX00C_E_PK2, EVENT_ADDRESS);
#else
            LoadReq(EVTEX00C_PK2, EVENT_ADDRESS);
#endif
        }
        else if (pzl_no == 41)
        {
#ifdef BUILD_EU_VERSION
            LoadReqLanguage(EVTEX00E_E_PK2, EVENT_ADDRESS);
#else
            LoadReq(EVTEX00E_PK2, EVENT_ADDRESS);
#endif
        }

        star_pzl_wrk.mode = 1;
        star_pzl_wrk.time = 40;
    }

    if (pzl_no == 12)
    {
        star_pzl_wrk.pzl_no = 0;
    }
    else if (pzl_no == 18)
    {
        star_pzl_wrk.pzl_no = 1;
    }
    else if (pzl_no == 20)
    {
        star_pzl_wrk.pzl_no = 2;
    }
    else if (pzl_no == 36)
    {
        star_pzl_wrk.pzl_no = 3;
    }
    else if (pzl_no == 41)
    {
        star_pzl_wrk.pzl_no = 4;
    }
    else if (pzl_no == 9 || pzl_no == 4)
    {
        star_pzl_wrk.pzl_no = 6;
    }
    else
    {
        star_pzl_wrk.pzl_no = 5;
    }

    StarPuzzleDataSet(star_pzl_wrk.pzl_no);
    SetBlackOut();
    SpevStrInit();
    CsrClearAll();
}

void StarPuzzleDataSet(int pzl_no)
{
    int i;
    int j;
    u_char line_dat[10][2] = {
        {0, 1}, {1, 3}, {3, 4}, {4, 2}, {2, 0},
        {0, 3}, {3, 2}, {2, 1}, {1, 4}, {4, 0},
    };
    u_char stone_odr[5] = { 0, 1, 3, 4, 2 };

    star_pzl_wrk.count = star_pzl_dat[star_pzl_wrk.pzl_no].move_num;

    for (i = 0; i < 5; i++)
    {
        if (star_pzl_dat[star_pzl_wrk.pzl_no].start[i] == star_pzl_dat[star_pzl_wrk.pzl_no].empty_no)
        {
            star_pzl_wrk.empty = i;
        }

        star_pzl_wrk.stone[i] = star_pzl_dat[star_pzl_wrk.pzl_no].start[i];
    }

    memset(star_pzl_wrk.line, 0xff, sizeof(star_pzl_wrk.line));

    for (i = 0; i < 10; i++)
    {
        if (star_pzl_dat[star_pzl_wrk.pzl_no].line[i] != 0)
        {
            star_pzl_wrk.line[line_dat[i][0]][line_dat[i][1]] = i;
            star_pzl_wrk.line[line_dat[i][1]][line_dat[i][0]] = i;
        }
    }

    if (star_pzl_wrk.empty != 0)
    {
        star_pzl_wrk.slct_no = 0;
    }
    else
    {
        star_pzl_wrk.slct_no = 1;
    }

    for (i = 0; i < 5; i++)
    {
        if (star_pzl_wrk.slct_no == stone_odr[i])
        {
            for (j = 1; j < 6; j++)
            {
                if (star_pzl_wrk.line[stone_odr[(i + j) % 5]][star_pzl_wrk.empty] != 0xff)
                {
                    star_pzl_wrk.slct_no = stone_odr[(i + j) % 5];
                    break;
                }
            }
            break;
        }
    }
}

int StarPuzzleMain(int pzl_no)
{
    int i; int j; int tmp0;
    u_char stone_odr[5] = {0, 1, 3, 4, 2};

    switch(star_pzl_wrk.mode)
    {
    case STAR_PZL_MODE_DEADWAIT:
        if (star_pzl_wrk.time != 0)
        {
            star_pzl_wrk.time--;
        }
        else
        {
            if (pzl_no == 4)
            {
                LoadReq(EVTEX00G_PK2, EVENT_ADDRESS);
            }
            else if (pzl_no == 9)
            {
                LoadReq(EVTEX00F_PK2, EVENT_ADDRESS);
            }
            else if (pzl_no == 12)
            {
#ifdef BUILD_EU_VERSION
                LoadReqLanguage(EVTEX00_E_PK2, EVENT_ADDRESS);
#else
                LoadReq(EVTEX00_PK2, EVENT_ADDRESS);
#endif
            }
            else if (pzl_no == 18)
            {
#ifdef BUILD_EU_VERSION
                LoadReqLanguage(EVTEX00B_E_PK2, EVENT_ADDRESS);
#else
                LoadReq(EVTEX00B_PK2, EVENT_ADDRESS);
#endif
            }
            else if (pzl_no == 20)
            {
#ifdef BUILD_EU_VERSION
                LoadReqLanguage(EVTEX00D_E_PK2, EVENT_ADDRESS);
#else
                LoadReq(EVTEX00D_PK2, EVENT_ADDRESS);
#endif
            }
            else if (pzl_no == 36)
            {
#ifdef BUILD_EU_VERSION
                LoadReqLanguage(EVTEX00C_E_PK2, EVENT_ADDRESS);
#else
                LoadReq(EVTEX00C_PK2, EVENT_ADDRESS);
#endif
            }
            else if (pzl_no == 41)
            {
#ifdef BUILD_EU_VERSION
                LoadReqLanguage(EVTEX00E_E_PK2, EVENT_ADDRESS);
#else
                LoadReq(EVTEX00E_PK2, EVENT_ADDRESS);
#endif
            }

            star_pzl_wrk.mode = STAR_PZL_MODE_READY;
            star_pzl_wrk.time = 40;
        }
    break;
    case STAR_PZL_MODE_READY:
        if (star_pzl_wrk.time != 0)
        {
            star_pzl_wrk.time--;
        }
        else
        {
            if (IsLoadEndAll() != 0)
            {
                SetSprFile(EVENT_ADDRESS);

                star_pzl_wrk.mode = STAR_PZL_MODE_SELOAD;

                pzl_load_id = SeFileLoadAndSet(SP000_KAMON_BD, 2);

                ingame_wrk.stts |= 0x20;
            }
        }
    break;
    case STAR_PZL_MODE_SELOAD:
        if (star_pzl_wrk.time != 0)
        {
            star_pzl_wrk.time--;
            break;
        }
        else
        {
            if (IsLoadEnd(pzl_load_id) != 0)
            {
                pzl_load_id = -1;

                SetSprFile(EVENT_ADDRESS);

                star_pzl_wrk.mode = STAR_PZL_MODE_IN;
                star_pzl_wrk.time = 40;

                ingame_wrk.stts |= 0x20;

                SetBlackIn();
            }
        }
    break;
    case STAR_PZL_MODE_IN:
        if (star_pzl_wrk.time != 0)
        {
            star_pzl_wrk.time--;
        }
        else
        {
            if (star_pzl_wrk.pzl_no == 0)
            {
                star_pzl_wrk.mode = STAR_PZL_MODE_MSG0;

                break;
            }

            star_pzl_wrk.mode = STAR_PZL_MODE_START;
            star_pzl_wrk.time = 40;
        }
    break;
    case STAR_PZL_MODE_MSG0:
        if (ButtonMarkNext(0, 2, 1) != 0)
        {
            star_pzl_wrk.mode = STAR_PZL_MODE_START;

            ButtonMarkTimeClear();
        }
    break;
    case STAR_PZL_MODE_SLCT:
        if (star_pzl_wrk.time != 0)
        {
            star_pzl_wrk.time--;
        }

        if (*key_now[4] == 1)
        {
            if (star_pzl_wrk.count < star_pzl_dat[star_pzl_wrk.pzl_no].move_num)
            {
                SeStartFix(3, 0, 0x1000, 0x1000, 0);

                tmp0 = star_pzl_wrk.stone[star_pzl_wrk.bak_no[star_pzl_wrk.count]];

                star_pzl_wrk.stone[star_pzl_wrk.bak_no[star_pzl_wrk.count]] = star_pzl_wrk.stone[star_pzl_wrk.empty];
                star_pzl_wrk.stone[star_pzl_wrk.empty] = tmp0;
                star_pzl_wrk.slct_no = star_pzl_wrk.empty;
                star_pzl_wrk.empty = star_pzl_wrk.bak_no[star_pzl_wrk.count];
                star_pzl_wrk.count += 1;
                star_pzl_wrk.time = 30;
            }
            else
            {
                star_pzl_wrk.mode = STAR_PZL_MODE_OUT;
                star_pzl_wrk.time = 40;

                SetBlackOut();
            }
        }
        else if (*key_now[7] == 1)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            if (star_pzl_wrk.count != star_pzl_dat[star_pzl_wrk.pzl_no].move_num)
            {
                StarPuzzleDataSet(star_pzl_wrk.pzl_no);

                star_pzl_wrk.time = 15;
                star_pzl_wrk.mode = STAR_PZL_MODE_RESL;
            }
        }
        else if (*key_now[5] == 1 && star_pzl_wrk.count != 0)
        {
            if (star_pzl_wrk.line[star_pzl_wrk.slct_no][star_pzl_wrk.empty] != 0xff)
            {
                int no = star_pzl_wrk.slct_no;

                tmp0 = star_pzl_wrk.stone[no];

                star_pzl_wrk.stone[no] = star_pzl_wrk.stone[star_pzl_wrk.empty];

                star_pzl_wrk.slct_no = star_pzl_wrk.empty;
                star_pzl_wrk.stone[star_pzl_wrk.empty] = tmp0;
                star_pzl_wrk.empty = no;
                star_pzl_wrk.mode = STAR_PZL_MODE_MOVE;
                star_pzl_wrk.time = 30;

                SeStartFix(33, 0, 0x1000, 0x1000, 0);
            }
            else
            {
                SeStartFix(2, 0, 0x1000, 0x1000, 0);
            }
        }
        else if (
            *key_now[3] == 1 ||
            (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
            Ana2PadDirCnt(1) == 1 ||
            (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
        )
        {
            for (i = 0; i < 5; i++)
            {
                if (star_pzl_wrk.slct_no == stone_odr[i])
                {
                    for (j = 1; j < 6; j++)
                    {
                        if (star_pzl_wrk.line[stone_odr[(i + j) % 5]][star_pzl_wrk.empty] != 0xff)
                        {
                            star_pzl_wrk.slct_no = stone_odr[(i + j) % 5];

                            break;
                        }
                    }
                    break;
                }
            }

            star_pzl_wrk.time = 30;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
        else if (
            *key_now[2] == 1 ||
            (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
            Ana2PadDirCnt(3) == 1 ||
            (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
        )
        {
            for (i = 0; i < 5; i++)
            {
                if (star_pzl_wrk.slct_no == stone_odr[i])
                {
                    for (j = 1; j < 6; j++)
                    {
                        if (star_pzl_wrk.line[stone_odr[(i + 5 - j) % 5]][star_pzl_wrk.empty] != 0xff)
                        {
                            star_pzl_wrk.slct_no = stone_odr[(i + 5 - j) % 5];

                            break;
                        }
                    }
                    break;
                }
            }

            star_pzl_wrk.time = 30;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
    break;
    case STAR_PZL_MODE_MOVE:
        if (star_pzl_wrk.time != 0)
        {
            star_pzl_wrk.time--;

            break;
        }

        if (star_pzl_wrk.count != 0)
        {
            star_pzl_wrk.count--;
            star_pzl_wrk.bak_no[star_pzl_wrk.count] = star_pzl_wrk.slct_no;
        }

        if (star_pzl_wrk.count != 0)
        {
            star_pzl_wrk.mode = STAR_PZL_MODE_SLCT;
        }
        else if (StarPuzzleClearJudge() != 0)
        {
            star_pzl_wrk.mode = STAR_PZL_MODE_READYGOOD;
            star_pzl_wrk.time = 60;

            SetBlackOut();
        }
        else
        {
            star_pzl_wrk.mode = STAR_PZL_MODE_BEFBAD;
            star_pzl_wrk.time = 15;
        }
    break;
    case STAR_PZL_MODE_MENU:
        if (star_pzl_wrk.time != 0)
        {
            star_pzl_wrk.time--;
        }

        if (star_pzl_wrk.menu_mode == STAR_PZL_MENU_SLCT)
        {
            if (*key_now[4] == 1)
            {
                star_pzl_wrk.time = 15;
                star_pzl_wrk.mode = STAR_PZL_MODE_RESL;
            }
            else if (*key_now[5] == 1)
            {
                if (star_pzl_wrk.menu_csr[0] == 0)
                {
                    star_pzl_wrk.time = 15;
                    star_pzl_wrk.mode = STAR_PZL_MODE_RESL;
                }
                else if (star_pzl_wrk.menu_csr[0] == 1)
                {
                    star_pzl_wrk.menu_mode = STAR_PZL_MENU_RESET;
                    star_pzl_wrk.menu_csr[1] = 0;
                }
                else if (star_pzl_wrk.menu_csr[0] == 2)
                {
                    star_pzl_wrk.menu_mode = STAR_PZL_MENU_EXIT;
                    star_pzl_wrk.menu_csr[1] = 0;
                }
            }
            else if (*key_now[0] == 1)
            {
                if (star_pzl_wrk.menu_csr[0] != 0)
                {
                    star_pzl_wrk.menu_csr[0]--;
                }
                else
                {
                    star_pzl_wrk.menu_csr[0] = 2;
                }
            }
            else if (*key_now[1] == 1)
            {
                if (star_pzl_wrk.menu_csr[0] != 2)
                {
                    star_pzl_wrk.menu_csr[0]++;
                }
                else
                {
                    star_pzl_wrk.menu_csr[0] = 0;
                }
            }
        }
        else if (star_pzl_wrk.menu_mode == STAR_PZL_MENU_RESET)
        {
            if (*key_now[4] == 1)
            {
                star_pzl_wrk.menu_mode = STAR_PZL_MENU_SLCT;

                SeStartFix(3, 0, 0x1000, 0x1000, 0);
            }
            else if (*key_now[5] == 1)
            {
                if (star_pzl_wrk.menu_csr[1] != 0)
                {
                    star_pzl_wrk.menu_mode = STAR_PZL_MENU_SLCT;
                }
                else
                {
                    StarPuzzleDataSet(star_pzl_wrk.pzl_no);

                    star_pzl_wrk.time = 15;
                    star_pzl_wrk.mode = STAR_PZL_MODE_RESL;
                }

                SeStartFix(1, 0, 0x1000, 0x1000, 0);
            }
            else if (*key_now[3] == 1 || *key_now[2] == 1)
            {
                if (star_pzl_wrk.menu_csr[1] != 0)
                {
                    star_pzl_wrk.menu_csr[1] = 0;
                }
                else
                {
                    star_pzl_wrk.menu_csr[1] = 1;
                }

                SeStartFix(0, 0, 0x1000, 0x1000, 0);
            }
        }
        else if (star_pzl_wrk.menu_mode == STAR_PZL_MENU_EXIT)
        {
            if (*key_now[4] == 1)
            {
                star_pzl_wrk.menu_mode = STAR_PZL_MENU_SLCT;
            }
            else if (*key_now[5] == 1)
            {
                if (star_pzl_wrk.menu_csr[1] != 0)
                {
                    star_pzl_wrk.menu_mode = STAR_PZL_MENU_SLCT;
                }
                else
                {
                    star_pzl_wrk.mode = STAR_PZL_MODE_OUT;
                    star_pzl_wrk.time = 40;
                }
            }
            else if (*key_now[3] == 1 || *key_now[2] == 1)
            {
                if (star_pzl_wrk.menu_csr[1] != 0)
                {
                    star_pzl_wrk.menu_csr[1] = 0;
                }
                else
                {
                    star_pzl_wrk.menu_csr[1] = 1;
                }
            }
        }
    break;
    case STAR_PZL_MODE_START:
    case STAR_PZL_MODE_RESL:
        if (star_pzl_wrk.time != 0)
        {
            star_pzl_wrk.time--;
        }
        else
        {
            star_pzl_wrk.mode = STAR_PZL_MODE_SLCT;
        }
    break;
    case STAR_PZL_MODE_READYGOOD:
        if (star_pzl_wrk.time != 0)
        {
            star_pzl_wrk.time--;
        }
        else
        {
            SeStartFix(34, 0, 0x1000, 0x1000, 0);

            star_pzl_wrk.mode = STAR_PZL_MODE_READYGOOD2;
            star_pzl_wrk.time = 50;

            SetBlackIn();
        }
    break;
    case STAR_PZL_MODE_READYGOOD2:
        if (star_pzl_wrk.time != 0)
        {
            star_pzl_wrk.time--;
        }
        else
        {
            star_pzl_wrk.mode = STAR_PZL_MODE_GOOD;
            star_pzl_wrk.time = 20;
        }
    break;
    case STAR_PZL_MODE_GOOD:
        if (*key_now[5] == 1 || *key_now[4] == 1)
        {
            star_pzl_wrk.time = 0;
        }

        ButtonMarkNext(0, 0, 0);

        if (star_pzl_wrk.time == 0)
        {
            ButtonMarkTimeClear();
            SeStartFix(35, 0, 0x1000, 0x1000, 0);

            star_pzl_wrk.mode = STAR_PZL_MODE_GOUT;
            star_pzl_wrk.time = 60;

            SetBlackOut();
        }
    break;
    case STAR_PZL_MODE_BEFBAD:
        if (star_pzl_wrk.time != 0)
        {
            star_pzl_wrk.time--;
        }
        else
        {
            star_pzl_wrk.mode = STAR_PZL_MODE_BAD;
        }
    break;
    case STAR_PZL_MODE_BAD:
        if (ButtonMarkNext(0, 0, 0) != 0)
        {
            ButtonMarkTimeClear();
            SeStartFix(3, 0, 0x1000, 0x1000, 0);
            StarPuzzleDataSet(star_pzl_wrk.pzl_no);

            star_pzl_wrk.time = 15;
            star_pzl_wrk.mode = STAR_PZL_MODE_RESL;
        }
    break;
    case STAR_PZL_MODE_GOUT:
        if (star_pzl_wrk.time != 0)
        {
            star_pzl_wrk.time--;
        }
        else
        {
            SetBlackIn();

            event_stts[ev_wrk.evt_no] = 1;

            star_pzl_wrk.mode = STAR_PZL_MODE_END;
        }
    break;
    case STAR_PZL_MODE_OUT:
        if (star_pzl_wrk.time != 0)
        {
            star_pzl_wrk.time--;
        }
        else
        {
            star_pzl_wrk.mode = STAR_PZL_MODE_END;

            SetBlackIn();
        }
    break;
    case STAR_PZL_MODE_END:
        if (pzl_load_id == -1)
        {
            pzl_load_id = SeFileLoadAndSet(SV000_MIKU_BD, 2);
        }
        else
        {
            if (IsLoadEnd(pzl_load_id) != 0)
            {
                ingame_wrk.stts &= ~0x20;

                spev_wrk.ret = 1;

                pzl_load_id = -1;

                SendManMdlTex();
            }
        }
    break;
    }

    if (star_pzl_wrk.mode != STAR_PZL_MODE_DEADWAIT && star_pzl_wrk.mode != STAR_PZL_MODE_READY && star_pzl_wrk.mode != STAR_PZL_MODE_END)
    {
        StarPuzzleDisp();
    }

    return 0;
}

int StarPuzzleClearJudge()
{
    int i;

    for (i = 0; i < 5; i++)
    {
        if (star_pzl_wrk.stone[i] != i)
        {
            return 0;
        }
    }

    return 1;
}

void StarPuzzleDisp()
{
    int i;
    u_char alp_rate;
    SPRT_SDAT ssd;
    DISP_STR ds;
    DISP_SQAR dsq;
    SQAR_DAT menu_sq0 = {
        .w = 572,
        .h = 92,
        .x = 29,
        .y = 330,
        .pri = 0x5000,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x80,
    };
    u_char stone_odr[5] = { 0, 1, 3, 4, 2 };
#ifdef BUILD_EU_VERSION
    int mx;
    int w1;
    int w2;
#endif

    if (star_pzl_wrk.mode == STAR_PZL_MODE_IN)
    {
        alp_rate = ((40 - star_pzl_wrk.time) * 100) / 40;
    }
    else if (star_pzl_wrk.mode == STAR_PZL_MODE_OUT)
    {
        alp_rate = (star_pzl_wrk.time * 100) / 40;
    }
    else
    {
        alp_rate = 0x64;
    }

    for (i = 0; i < 11; i++)
    {
        SimpleDispSprt(&spev00_sp_bak[i], EVENT_ADDRESS, i, NULL, NULL, alp_rate);
    }

    if (star_pzl_wrk.mode == STAR_PZL_MODE_MENU)
    {
        alp_rate = star_pzl_wrk.time * 5;
    }
    else if (star_pzl_wrk.mode == STAR_PZL_MODE_OUT)
    {
        alp_rate = 0;
    }

    for (i = 0; i < 10; i++)
    {
        if (
            star_pzl_wrk.mode != STAR_PZL_MODE_BAD && star_pzl_wrk.mode != STAR_PZL_MODE_GOOD &&
            star_pzl_wrk.mode != STAR_PZL_MODE_READYGOOD2 && star_pzl_wrk.mode != STAR_PZL_MODE_OUT &&
            star_pzl_wrk.mode != STAR_PZL_MODE_GOUT
        )
        {
            if (star_pzl_wrk.line[star_pzl_wrk.slct_no][star_pzl_wrk.empty] == i)
            {
                SimpleDispSprtDatCopy(&spev00_sp_lin[i], &ssd);

                if (star_pzl_wrk.mode == STAR_PZL_MODE_SLCT)
                {
                    ssd.alp = ssd.alp * (30 - star_pzl_wrk.time) / 30;
                }

                SimpleDispSprt(&ssd, EVENT_ADDRESS, 14, &spev00_lin_rot[i], &spev00_lin_scl[i], alp_rate);
            }
        }
    }

    for (i = 0; i < 10; i++)
    {
        if (star_pzl_wrk.mode != STAR_PZL_MODE_GOUT && star_pzl_wrk.mode != STAR_PZL_MODE_BAD && star_pzl_wrk.mode != STAR_PZL_MODE_OUT)
        {
            if (star_pzl_dat[star_pzl_wrk.pzl_no].line[i] != 0)
            {
                SimpleDispSprt(spev00_sp_lia + i, EVENT_ADDRESS, 14, &spev00_lia_rot[i], &spev00_lia_scl[i], alp_rate);
            }
        }
    }

    for (i = 0; i < 5; i++)
    {
        if (star_pzl_wrk.mode == STAR_PZL_MODE_READYGOOD2 || star_pzl_wrk.mode == STAR_PZL_MODE_GOOD || star_pzl_wrk.mode == STAR_PZL_MODE_GOUT)
        {
            SimpleDispSprtDatCopy(&spev00_sp_stn[i], &ssd);

            ssd.x = spev00_stn_pos[i][0];
            ssd.y = spev00_stn_pos[i][1];

            SimpleDispSprt(&ssd, EVENT_ADDRESS, 13, NULL, NULL, alp_rate);

            if (star_pzl_wrk.mode == STAR_PZL_MODE_GOOD)
            {
                SimpleDispSprtDatCopy(&spev00_sp_sta[star_pzl_wrk.empty], &ssd);

                if (spev_wrk.csr[2] == 0)
                {
                    spev_wrk.csr[2] = CsrBlink(&spev_wrk.csr[1], 0x80, 0, 2, spev_wrk.csr);
                }

                ssd.x = spev00_sta_pos[star_pzl_wrk.empty][0];
                ssd.y = spev00_sta_pos[star_pzl_wrk.empty][1];
                ssd.alp = spev_wrk.csr[1];

                SimpleDispSprt(&ssd, EVENT_ADDRESS, 14, NULL, NULL, alp_rate);
            }
        }
        else if (i == star_pzl_wrk.slct_no)
        {
            if (star_pzl_wrk.mode != STAR_PZL_MODE_IN && star_pzl_wrk.mode != STAR_PZL_MODE_OUT)
            {
                SimpleDispSprtDatCopy(&spev00_sp_sta[star_pzl_wrk.stone[i]], &ssd);

                if (star_pzl_wrk.mode == STAR_PZL_MODE_START)
                {
                    ssd.alp = ssd.alp * (40 - star_pzl_wrk.time) / 40;
                }
                else if (star_pzl_wrk.mode == STAR_PZL_MODE_SLCT)
                {
                    ssd.alp = ssd.alp * (30 - star_pzl_wrk.time) / 30;
                }

                if (star_pzl_wrk.mode == STAR_PZL_MODE_MOVE)
                {
                    ssd.x = spev00_sta_pos[star_pzl_wrk.slct_no][0] +
                        ((spev00_sta_pos[star_pzl_wrk.empty][0] - spev00_sta_pos[star_pzl_wrk.slct_no][0]) * star_pzl_wrk.time / 30);

                    ssd.y = spev00_sta_pos[star_pzl_wrk.slct_no][1] +
                        ((spev00_sta_pos[star_pzl_wrk.empty][1] - spev00_sta_pos[star_pzl_wrk.slct_no][1]) * star_pzl_wrk.time / 30);
                }
                else
                {
                    ssd.x = spev00_sta_pos[i][0];
                    ssd.y = spev00_sta_pos[i][1];
                }

                SimpleDispSprt(&ssd, EVENT_ADDRESS, 14, NULL, NULL, alp_rate);
            }

            SimpleDispSprtDatCopy(&spev00_sp_stn[star_pzl_wrk.stone[i]], &ssd);

            if (star_pzl_wrk.mode == STAR_PZL_MODE_MOVE)
            {
                ssd.x = spev00_stn_pos[star_pzl_wrk.slct_no][0] +
                    ((spev00_stn_pos[star_pzl_wrk.empty][0] - spev00_stn_pos[star_pzl_wrk.slct_no][0]) * star_pzl_wrk.time / 30);

                ssd.y = spev00_stn_pos[star_pzl_wrk.slct_no][1] +
                        (((spev00_stn_pos[star_pzl_wrk.empty][1] - spev00_stn_pos[star_pzl_wrk.slct_no][1]) * star_pzl_wrk.time) / 30);
            }
            else
            {
                ssd.x = spev00_stn_pos[i][0];
                ssd.y = spev00_stn_pos[i][1];
            }

            SimpleDispSprt(&ssd, EVENT_ADDRESS, 13, NULL, NULL, alp_rate);
        }
        else if (i != star_pzl_wrk.empty)
        {
            SimpleDispSprtDatCopy(&spev00_sp_stn[star_pzl_wrk.stone[i]], &ssd);

            ssd.x = spev00_stn_pos[i][0];
            ssd.y = spev00_stn_pos[i][1];

            SimpleDispSprt(&ssd, EVENT_ADDRESS, 13, NULL, NULL, alp_rate);
        }
    }

    if (star_pzl_wrk.mode != STAR_PZL_MODE_BAD && star_pzl_wrk.line[star_pzl_wrk.slct_no][star_pzl_wrk.empty] != 0xff)
    {
        SimpleDispSprt(&spev00_sp_lia[star_pzl_wrk.empty + 10], EVENT_ADDRESS, 14, NULL, NULL, alp_rate);
        SimpleDispSprt(&spev00_sp_lia[star_pzl_wrk.slct_no + 10], EVENT_ADDRESS, 14, NULL, NULL, alp_rate);
    }

    SimpleDispSprt(&spev00_sp_hln[star_pzl_wrk.empty], EVENT_ADDRESS, 16, NULL, NULL, alp_rate);
    SimpleDispSprt(&spev00_sp_hln[star_pzl_wrk.slct_no], EVENT_ADDRESS, 16, NULL, NULL, alp_rate);

    if (star_pzl_wrk.mode == STAR_PZL_MODE_MOVE)
    {
        SimpleDispSprtDatCopy(&spev00_sp_num[star_pzl_wrk.count-1], &ssd);
        ssd.alp = ssd.alp * (30 - star_pzl_wrk.time) / 30;
        SimpleDispSprt(&ssd, EVENT_ADDRESS, 15, NULL, NULL, alp_rate);

        SimpleDispSprtDatCopy(&spev00_sp_num[star_pzl_wrk.count], &ssd);
        ssd.alp = ssd.alp * star_pzl_wrk.time / 30;
        SimpleDispSprt(&ssd, EVENT_ADDRESS, 15, NULL, NULL, alp_rate);
    }
    else
    {
        SimpleDispSprt(&spev00_sp_num[star_pzl_wrk.count], EVENT_ADDRESS, 15, NULL, NULL, alp_rate);
    }

    if (star_pzl_wrk.mode == STAR_PZL_MODE_RESL)
    {
        alp_rate = (15 - star_pzl_wrk.time) * 100 / 15;
    }

    if (star_pzl_wrk.mode != STAR_PZL_MODE_IN && star_pzl_wrk.mode != STAR_PZL_MODE_READYGOOD2 && star_pzl_wrk.mode != STAR_PZL_MODE_GOOD && star_pzl_wrk.mode != STAR_PZL_MODE_GOUT && star_pzl_wrk.mode != STAR_PZL_MODE_OUT)
    {
        for (i = 0; i < 5; i++)
        {
            if (star_pzl_wrk.stone[star_pzl_wrk.slct_no] == i)
            {
                if (star_pzl_wrk.mode == STAR_PZL_MODE_START)
                {
                    SimpleDispSprtDatCopy(&spev00_sp_bff[i], &ssd);
                    ssd.alp = ssd.alp * (40 - star_pzl_wrk.time) / 40;
                    SimpleDispSprt(&ssd, EVENT_ADDRESS, 11, NULL, NULL, alp_rate);
                }
                else
                {
                    SimpleDispSprt(&spev00_sp_bff[i], EVENT_ADDRESS, 11, NULL, NULL, alp_rate);
                }
            }
        }
    }

    if (star_pzl_wrk.mode != STAR_PZL_MODE_IN && star_pzl_wrk.mode != STAR_PZL_MODE_OUT)
    {
        for (i = 0; i < 5; i++)
        {
            if (star_pzl_wrk.mode == STAR_PZL_MODE_START)
            {
                SimpleDispSprtDatCopy(&spev00_sp_bft[i], &ssd);
                ssd.alp = ssd.alp * (40 - star_pzl_wrk.time) / 40;
                SimpleDispSprt(&ssd, EVENT_ADDRESS, 12, NULL, NULL, alp_rate);
            }
            else
            {
                SimpleDispSprt(&spev00_sp_bft[i], EVENT_ADDRESS, 12, NULL, NULL, alp_rate);
            }
        }
    }

    if (star_pzl_wrk.mode == STAR_PZL_MODE_MENU)
    {
        CopySqrDToSqr(&dsq, &spev_sq0);
        DispSqrD(&dsq);

        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 142.0f, 131.0f, 346.0f, 145.0f, 0x7f);

        if (star_pzl_wrk.menu_mode == STAR_PZL_MENU_SLCT)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG00);
            spev_str.pos_x = 231;
            spev_str.pos_y = 157;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG01);
            spev_str.pos_x = 219;
            spev_str.pos_y = 191;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG02);
            spev_str.pos_x = 243;
            spev_str.pos_y = 225;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            for (i = 0; i < 3; i++)
            {
                SimpleDispSprtDatCopy(&spev00_sp_csr[i], &ssd);

                ssd.y += star_pzl_wrk.menu_csr[0] * 34;

                if (i < 2)
                {
                    SimpleDispSprt(&ssd, EVENT_ADDRESS, 17, NULL, NULL, 0x64);
                }
                else
                {
                    SimpleDispSprt(&ssd, EVENT_ADDRESS, 17, NULL, &spev00_csr_scl[0], 0x64);
                }
            }
        }
        else if (star_pzl_wrk.menu_mode == STAR_PZL_MENU_RESET)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG03);
            spev_str.pos_x = 171;
            spev_str.pos_y = 166;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG05);
            spev_str.pos_x = 219;
            spev_str.pos_y = 215;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG06);
            spev_str.pos_x = 351;
            spev_str.pos_y = 215;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            for (i = 3; i < 6; i++)
            {
                SimpleDispSprtDatCopy(&spev00_sp_csr[i], &ssd);

                ssd.x += star_pzl_wrk.menu_csr[1] * 141;

                if (i < 5)
                {
                    SimpleDispSprt(&ssd, EVENT_ADDRESS, 17, NULL, NULL, 0x64);
                }
                else
                {
                    SimpleDispSprt(&ssd, EVENT_ADDRESS, 17, NULL, &spev00_csr_scl[star_pzl_wrk.menu_csr[1] + 1], 0x64);
                }
            }
        }
        else if (star_pzl_wrk.menu_mode == STAR_PZL_MENU_EXIT)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG04);
            spev_str.pos_x = 207;
            spev_str.pos_y = 166;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG05);
            spev_str.pos_x = 219;
            spev_str.pos_y = 215;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG06);
            spev_str.pos_x = 351;
            spev_str.pos_y = 215;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            for (i = 3; i < 6; i++)
            {
                SimpleDispSprtDatCopy(&spev00_sp_csr[i], &ssd);

                ssd.x += star_pzl_wrk.menu_csr[1] * 141;

                if (i < 5)
                {
                    SimpleDispSprt(&ssd, EVENT_ADDRESS, 17, NULL, NULL, 0x64);
                }
                else
                {
                    SimpleDispSprt(&ssd, EVENT_ADDRESS, 17, NULL, &spev00_csr_scl[star_pzl_wrk.menu_csr[1] + 1], 0x64);
                }
            }
        }
    }

    if (star_pzl_wrk.mode == STAR_PZL_MODE_GOOD)
    {
        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

#ifdef BUILD_EU_VERSION
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG06);
        w2 = GetStrWidth(spev_str.str);
#endif

        if (star_pzl_wrk.pzl_no == 0)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG07);
#ifdef BUILD_EU_VERSION
            w1 = GetStrWidth(spev_str.str);

            mx = 320 - ((w1 + 12) + w2) / 2;

            spev_str.pos_x = mx;
#else
            spev_str.pos_x = 173;
#endif
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (star_pzl_wrk.pzl_no == 1)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG08);
#ifdef BUILD_EU_VERSION
            w1 = GetStrWidth(spev_str.str);

            mx = 320 - ((w1 + 12) + w2) / 2;

            spev_str.pos_x = mx;
#else
            spev_str.pos_x = 187;
#endif
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (star_pzl_wrk.pzl_no == 2)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG09);
#ifdef BUILD_EU_VERSION
            w1 = GetStrWidth(spev_str.str);

            mx = 320 - ((w1 + 12) + w2) / 2;

            spev_str.pos_x = mx;
#else
            spev_str.pos_x = 163;
#endif
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (star_pzl_wrk.pzl_no == 3)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG10);
#ifdef BUILD_EU_VERSION
            w1 = GetStrWidth(spev_str.str);

            mx = 320 - ((w1 + 12) + w2) / 2;

            spev_str.pos_x = mx;
#else
            spev_str.pos_x = 187;
#endif
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG11);
#ifdef BUILD_EU_VERSION
            w1 = GetStrWidth(spev_str.str);

            mx = 320 - ((w1 + 12) + w2) / 2;

            spev_str.pos_x = mx;
#else
            spev_str.pos_x = 187;
#endif
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG06);
#ifdef BUILD_EU_VERSION
        spev_str.pos_x =  mx + w1 + 12;
#else
        spev_str.pos_x = 253;
#endif
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }
    else if (star_pzl_wrk.mode == STAR_PZL_MODE_MSG0)
    {
        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG14);
        spev_str.pos_x = 50;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = 340;
#else
        spev_str.pos_y = 345;
#endif

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }
    else if (star_pzl_wrk.mode == STAR_PZL_MODE_BAD)
    {
        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV04_MSG00);
        spev_str.pos_x = 50;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = 340;
#else
        spev_str.pos_y = 345;
#endif

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }

    if (star_pzl_wrk.mode == STAR_PZL_MODE_BEFBAD)
    {
        alp_rate = star_pzl_wrk.time * 100 / 15;
    }
    else if (star_pzl_wrk.mode == STAR_PZL_MODE_BAD)
    {
        alp_rate = 0;
    }

    SimpleDispSprt(&spev00_sp_cpf[0], EVENT_ADDRESS, 18, NULL, NULL, alp_rate);
    SimpleDispSprt(&spev00_sp_cpf[2], EVENT_ADDRESS, 18, NULL, NULL, alp_rate);
#ifdef BUILD_EU_VERSION
    SimpleDispSprt(&spev00_sp_cpf[4], EVENT_ADDRESS, 18, NULL, NULL, alp_rate);
#endif

    if (star_pzl_wrk.count < star_pzl_dat[star_pzl_wrk.pzl_no].move_num)
    {
        SimpleDispSprt(&spev00_sp_cpf[1], EVENT_ADDRESS, 18, NULL, NULL, alp_rate);
    }
    else
    {
        SimpleDispSprt(&spev00_sp_cpf[3], EVENT_ADDRESS, 18, NULL, NULL, alp_rate);
    }

    if (star_pzl_wrk.count != 0)
    {
        SimpleDispSprt(&spev00_sp_cpf2[8], EVENT_ADDRESS, 20, NULL, NULL, alp_rate);
        SimpleDispSprt(&spev00_sp_cpf2[0], EVENT_ADDRESS, 20, NULL, NULL, alp_rate);
        SimpleDispSprt(&spev00_sp_cpf2[star_pzl_wrk.count], EVENT_ADDRESS, 20, NULL, NULL, alp_rate);
    }
    else
    {
        SimpleDispSprt(&spev00_sp_cpf2[8], EVENT_ADDRESS, 20, NULL, NULL, alp_rate);
        SimpleDispSprt(&spev00_sp_cpf2[0], EVENT_ADDRESS, 20, NULL, NULL, alp_rate);
        SimpleDispSprt(&spev00_sp_cpf2[1], EVENT_ADDRESS, 20, NULL, NULL, alp_rate);
    }

    SimpleDispSprt(spev00_sp_cap, EVENT_ADDRESS, 19, NULL, NULL, alp_rate);

    DrawButtonTex(0xa000, 3, spev00_cbt_pos[0][0], spev00_cbt_pos[0][1], alp_rate);
    DrawButtonTex(0xa000, 1, spev00_cbt_pos[1][0], spev00_cbt_pos[1][1], alp_rate);
    DrawButtonTex(0xa000, 2, spev00_cbt_pos[2][0], spev00_cbt_pos[2][1], alp_rate);
}

int StarPuzzleMSGMain(int pzl_no)
{
    u_char stone_odr[5] = { 0, 1, 3, 4, 2 };

    switch(star_pzl_wrk.mode)
    {
    case STAR_PZL_MODE_DEADWAIT:
        if (star_pzl_wrk.time != 0)
        {
            star_pzl_wrk.time--;
        }
        else
        {
            if (pzl_no == 4)
            {
                LoadReq(EVTEX00G_PK2, EVENT_ADDRESS);
            }
            else if (pzl_no == 9)
            {
                LoadReq(EVTEX00F_PK2, EVENT_ADDRESS);
            }
            else if (pzl_no == 12)
            {
#ifdef BUILD_EU_VERSION
                LoadReqLanguage(EVTEX00_E_PK2, EVENT_ADDRESS);
#else
                LoadReq(EVTEX00_PK2, EVENT_ADDRESS);
#endif
            }
            else if (pzl_no == 18)
            {
#ifdef BUILD_EU_VERSION
                LoadReqLanguage(EVTEX00B_E_PK2, EVENT_ADDRESS);
#else
                LoadReq(EVTEX00B_PK2, EVENT_ADDRESS);
#endif
            }
            else if (pzl_no == 20)
            {
#ifdef BUILD_EU_VERSION
                LoadReqLanguage(EVTEX00D_E_PK2, EVENT_ADDRESS);
#else
                LoadReq(EVTEX00D_PK2, EVENT_ADDRESS);
#endif
            }
            else if (pzl_no == 36)
            {
#ifdef BUILD_EU_VERSION
                LoadReqLanguage(EVTEX00C_E_PK2, EVENT_ADDRESS);
#else
                LoadReq(EVTEX00C_PK2, EVENT_ADDRESS);
#endif
            }
            else if (pzl_no == 41)
            {
#ifdef BUILD_EU_VERSION
                LoadReqLanguage(EVTEX00E_E_PK2, EVENT_ADDRESS);
#else
                LoadReq(EVTEX00E_PK2, EVENT_ADDRESS);
#endif
            }

            star_pzl_wrk.mode = STAR_PZL_MODE_READY;
            star_pzl_wrk.time = 40;
        }
    break;
    case STAR_PZL_MODE_READY:
        if (star_pzl_wrk.time != 0)
        {
            star_pzl_wrk.time--;
        }
        else
        {
            if (IsLoadEndAll() != 0)
            {
                SetSprFile(EVENT_ADDRESS);

                star_pzl_wrk.mode = STAR_PZL_MODE_IN;
                star_pzl_wrk.time = 40;

                ingame_wrk.stts |= 0x20;

                SetBlackIn();
            }
        }
    break;
    case STAR_PZL_MODE_IN:
        if (star_pzl_wrk.time != 0)
        {
            star_pzl_wrk.time--;
        }
        else
        {
            if (star_pzl_wrk.pzl_no != 6)
            {
                star_pzl_wrk.mode = STAR_PZL_MODE_MSG0;
            }
            else
            {
                star_pzl_wrk.mode = STAR_PZL_MODE_MSG2;
            }
        }
    break;
    case STAR_PZL_MODE_MSG0:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            ButtonMarkTimeClear();

            star_pzl_wrk.mode = STAR_PZL_MODE_MSG;
        }
    break;
    case STAR_PZL_MODE_MSG2:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            ButtonMarkTimeClear();

            star_pzl_wrk.mode = STAR_PZL_MODE_MSG3;
        }
    break;
    case STAR_PZL_MODE_MSG:
    case STAR_PZL_MODE_MSG3:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            ButtonMarkTimeClear();

            star_pzl_wrk.mode = STAR_PZL_MODE_OUT;
            star_pzl_wrk.time = 40;

            SetBlackOut();
        }
    break;
    case STAR_PZL_MODE_OUT:
        if (star_pzl_wrk.time != 0)
        {
            star_pzl_wrk.time--;
        }
        else
        {
            star_pzl_wrk.mode = STAR_PZL_MODE_END;
        }
    break;
    case STAR_PZL_MODE_END:
        SendManMdlTex();

        ingame_wrk.stts &= ~0x20;

        spev_wrk.ret = 1;

        SetBlackIn();
    break;
    }

    if (star_pzl_wrk.mode != STAR_PZL_MODE_DEADWAIT && star_pzl_wrk.mode != STAR_PZL_MODE_READY && star_pzl_wrk.mode != STAR_PZL_MODE_END)
    {
        StarPuzzleMSGDisp();
    }

    return 0;
}

void StarPuzzleMSGDisp()
{
    int i;
    u_char alp_rate;
    SPRT_SDAT ssd;
    DISP_STR ds;
    DISP_SQAR dsq;
    SQAR_DAT menu_sq0 = {
        .w = 572,
        .h = 92,
        .x = 29,
        .y = 330,
        .pri = 0x5000,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x80,
    };

    if (star_pzl_wrk.mode == STAR_PZL_MODE_IN)
    {
        alp_rate = (40 - star_pzl_wrk.time) * 100 / 40;
    }
    else if (star_pzl_wrk.mode == STAR_PZL_MODE_OUT)
    {
        alp_rate = (star_pzl_wrk.time * 100) / 40;
    }
    else
    {
        alp_rate = 0x64;
    }

    for (i = 0; i < 11; i++)
    {
        SimpleDispSprt(&spev00_sp_bak[i], EVENT_ADDRESS, i, NULL, NULL, alp_rate);
    }

    if (star_pzl_wrk.pzl_no != STAR_PZL_MODE_SLCT && star_pzl_wrk.pzl_no != STAR_PZL_MODE_MSG0)
    {
        for (i = 0; i < 5; i++)
        {
            if (i != star_pzl_wrk.empty)
            {
                SimpleDispSprtDatCopy((&spev00_sp_stn[star_pzl_wrk.stone[i]]), &ssd);

                ssd.x = spev00_stn_pos[i][0];
                ssd.y = spev00_stn_pos[i][1];

                SimpleDispSprt(&ssd, EVENT_ADDRESS, 13, NULL, NULL, alp_rate);
            }
        }

        if (star_pzl_wrk.mode != STAR_PZL_MODE_BAD && star_pzl_wrk.line[star_pzl_wrk.slct_no][star_pzl_wrk.empty] != 0xff)
        {
            SimpleDispSprt((&spev00_sp_lia[star_pzl_wrk.slct_no+10]), EVENT_ADDRESS, 14, NULL, NULL, alp_rate);
        }

        SimpleDispSprt(&spev00_sp_hln[star_pzl_wrk.empty], EVENT_ADDRESS, 16, NULL, NULL, alp_rate);
        SimpleDispSprt(&spev00_sp_hln[star_pzl_wrk.slct_no], EVENT_ADDRESS, 16, NULL, NULL, alp_rate);
    }

    if (star_pzl_wrk.mode == STAR_PZL_MODE_MSG0)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG05);
        spev_str.pos_x = 50;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = 340;
#else
        spev_str.pos_y = 350;
#endif

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);
    }

    if (star_pzl_wrk.mode == STAR_PZL_MODE_MSG)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG15);
        spev_str.pos_x = 50;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = 340;
#else
        spev_str.pos_y = 350;
#endif

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);
    }

    if (star_pzl_wrk.mode == STAR_PZL_MODE_MSG2)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG12);
        spev_str.pos_x = 50;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = 340;
#else
        spev_str.pos_y = 350;
#endif

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);
    }

    if (star_pzl_wrk.mode == STAR_PZL_MODE_MSG3)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG13);
        spev_str.pos_x = 50;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = 340;
#else
        spev_str.pos_y = 350;
#endif

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);
    }
}

void DialKeyDoorInit(int door_no)
{
    DialKeyDoorDataSet(door_no);

    if (plyr_wrk.se_deadly != -1)
    {
        SeFadeFlame(plyr_wrk.se_deadly, 60, 0);

        plyr_wrk.se_deadly = -1;

        dkey_wrk.mode = DIAL_KEY_MODE_DEADWAIT;
        dkey_wrk.time = 60;
    }
    else
    {
#ifdef BUILD_EU_VERSION
        LoadReqLanguage(EVTEX01_E_PK2, EVENT_ADDRESS);
#else
        LoadReq(EVTEX01_PK2, EVENT_ADDRESS);
#endif

        pzl_load_id = -1;

        dkey_wrk.mode = DIAL_KEY_MODE_READY;
        dkey_wrk.time = 40;
    }

    CsrClear(&spev_wrk.csr[0]);
    CsrClear(&spev_wrk.csr[1]);
    CsrClear(&spev_wrk.csr[2]);

    SetBlackOut();
    SpevStrInit();
}

void DialKeyDoorDataSet(int door_no)
{
    dkey_wrk.door_no = door_no;
    dkey_wrk.mode = DIAL_KEY_MODE_READY;
    dkey_wrk.time = 40;
    dkey_wrk.count = 0;
    dkey_wrk.slct_no = 0;

    memset(dkey_wrk.btn, 0, sizeof(dkey_wrk.btn));
    memset(dkey_wrk.push_no, 0xff, sizeof(dkey_wrk.push_no));
}

int DialKeyDoorMain()
{
    int i;

    switch(dkey_wrk.mode)
    {
    case DIAL_KEY_MODE_DEADWAIT:
        if (dkey_wrk.time != 0)
        {
            dkey_wrk.time--;
        }
        else
        {
#ifdef BUILD_EU_VERSION
            LoadReqLanguage(EVTEX01_E_PK2, EVENT_ADDRESS);
#else
            LoadReq(EVTEX01_PK2, EVENT_ADDRESS);
#endif

            pzl_load_id = -1;

            dkey_wrk.mode = 1;
            dkey_wrk.time = 40;
        }
    break;
    case DIAL_KEY_MODE_READY:
        if (dkey_wrk.time != 0)
        {
            dkey_wrk.time--;
        }
        else
        {
            if (IsLoadEndAll() != 0)
            {
                dkey_wrk.mode = DIAL_KEY_MODE_SELOAD;

                ingame_wrk.stts |= 0x20;

                pzl_load_id = SeFileLoadAndSet(SP002_BANGOU_BD, 2);
            }
        }
    break;
    case DIAL_KEY_MODE_SELOAD:
        if (dkey_wrk.time != 0)
        {
            dkey_wrk.time--;
        }
        else
        {
            if (IsLoadEnd(pzl_load_id) != 0)
            {
                pzl_load_id = -1;

                dkey_wrk.mode = DIAL_KEY_MODE_IN;
                dkey_wrk.time = 40;

                ingame_wrk.stts |= 0x20;

                SetBlackIn();
            }
        }
    break;
    case DIAL_KEY_MODE_IN:
        if (dkey_wrk.time != 0)
        {
            dkey_wrk.time--;
        }
        else
        {
            dkey_wrk.mode = DIAL_KEY_MODE_MSG0;
        }
    break;
    case DIAL_KEY_MODE_MSG0:
        if (ButtonMarkNext(0, 3, 1) != 0)
        {
            dkey_wrk.mode = DIAL_KEY_MODE_MSG01;
        }
    break;
    case DIAL_KEY_MODE_MSG01:
        if (ButtonMarkWait() != 0)
        {
            break;
        }

        if (ButtonMarkNext(0, 3, 1) != 0)
        {
            dkey_wrk.mode = DIAL_KEY_MODE_MSG02;
        }
    break;
    case DIAL_KEY_MODE_MSG02:
        if (ButtonMarkWait() != 0)
        {
            break;
        }

        if (ButtonMarkNext(0, 3, 1) != 0)
        {
            dkey_wrk.mode = DIAL_KEY_MODE_MSG03;
        }
    break;
    case DIAL_KEY_MODE_MSG03:
        if (ButtonMarkWait() != 0)
        {
            break;
        }

        if (ButtonMarkNext(0, 3, 1) != 0)
        {
            dkey_wrk.mode = DIAL_KEY_MODE_SLCT;
        }
    break;
    case DIAL_KEY_MODE_SLCT:
        if (ButtonMarkWait() != 0)
        {
            break;
        }

        if (*key_now[4] == 1)
        {
            if (dkey_wrk.count != 0)
            {
                dkey_wrk.count--;

                dkey_wrk.push_no[dkey_wrk.count] = 0xff;

                SeStartFix(34, 0, 0x1000, 0x1000, 0);
            }
            else
            {
                dkey_wrk.mode = DIAL_KEY_MODE_OUT;
                dkey_wrk.time = 40;
            }
        }
        else if (*key_now[5] == 1)
        {
            if (dkey_wrk.count < dkey_dat[dkey_wrk.door_no].dial_num)
            {
                dkey_wrk.push = 0;
                dkey_wrk.push_no[dkey_wrk.count] = dkey_wrk.slct_no;
                dkey_wrk.count++;
                dkey_wrk.mode = DIAL_KEY_MODE_PUSH;
                dkey_wrk.time = 40;

                SeStartFix(33, 0, 0x1000, 0x1000, 0);
            }
        }
        else if (
            *key_now[3] == 1 ||
            (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
            Ana2PadDirCnt(1) == 1 ||
            (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
        )
        {
            if (dkey_wrk.slct_no != 0)
            {
                dkey_wrk.slct_no--;
            }
            else
            {
                dkey_wrk.slct_no = 9;
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
            if (dkey_wrk.slct_no != 9)
            {
                dkey_wrk.slct_no++;
            }
            else
            {
                dkey_wrk.slct_no = 0;
            }

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
    break;
    case DIAL_KEY_MODE_PUSH:
        if (dkey_wrk.time != 0)
        {
            dkey_wrk.time--;
        }
        else
        {
            if (dkey_wrk.count == dkey_dat[dkey_wrk.door_no].dial_num)
            {
                for (i = 0; i < dkey_dat[dkey_wrk.door_no].dial_num; i++)
                {
                    if (dkey_wrk.push_no[i] != dkey_dat[dkey_wrk.door_no].dial_no[i])
                    {
                        break;
                    }
                }

                if (i == dkey_dat[dkey_wrk.door_no].dial_num)
                {
                    dkey_wrk.mode = DIAL_KEY_MODE_GOOD;
                    dkey_wrk.time = 180;

                    SeStartFix(35, 0, 0x1000, 0x1000, 0);
                }
                else
                {
                    dkey_wrk.mode = DIAL_KEY_MODE_BAD;
                    dkey_wrk.time = 40;
                }
            }
            else
            {
                dkey_wrk.mode = DIAL_KEY_MODE_SLCT;
            }
        }
    break;
    case DIAL_KEY_MODE_GOOD:
        if (dkey_wrk.time != 0)
        {
            dkey_wrk.time--;
        }
        else
        {
            dkey_wrk.mode = DIAL_KEY_MODE_GOUT;
            dkey_wrk.time = 40;
        }
    break;
    case DIAL_KEY_MODE_BAD:
        if (dkey_wrk.time != 0)
        {
            dkey_wrk.time--;
        }
        else
        {
            dkey_wrk.mode = DIAL_KEY_MODE_BOUT;
            dkey_wrk.count = 0;

            memset(dkey_wrk.btn, 0, sizeof(dkey_wrk.btn));
            memset(dkey_wrk.push_no, 0xff, sizeof(dkey_wrk.push_no));
        }
    break;
    case DIAL_KEY_MODE_BOUT:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            dkey_wrk.mode = DIAL_KEY_MODE_SLCT;
        }
    break;
    case DIAL_KEY_MODE_GOUT:
    case DIAL_KEY_MODE_OUT:
        if (dkey_wrk.time != 0)
        {
            dkey_wrk.time--;
        }
        else
        {
            if (pzl_load_id == -1)
            {
                pzl_load_id = SeFileLoadAndSet(SV000_MIKU_BD, 2);
            }
            else if (IsLoadEnd(pzl_load_id) != 0)
            {
                if (dkey_wrk.mode != DIAL_KEY_MODE_GOUT)
                {
                    dkey_wrk.mode = DIAL_KEY_MODE_EXIT;
                }
                else
                {
                    dkey_wrk.mode = DIAL_KEY_MODE_GEXT;
                }

                dkey_wrk.time = 40;

                SendManMdlTex();

                ingame_wrk.stts &= ~0x20;

                SetBlackIn();
            }
        }
    break;
    case DIAL_KEY_MODE_EXIT:
    case DIAL_KEY_MODE_GEXT:
        if (dkey_wrk.time != 0)
        {
            dkey_wrk.time--;
        }
        else
        {
            if (dkey_wrk.mode == DIAL_KEY_MODE_GEXT)
            {
                SetMessageAddr(GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV00_MSG00));

                if (SetMessage() == 0)
                {
                    event_stts[ev_wrk.evt_no] = 1;

                    dkey_wrk.mode = DIAL_KEY_MODE_END;

                    DoorKeyUnlockOnly(dkey_dat[dkey_wrk.door_no].lock_door);
                }
            }
            else
            {
                dkey_wrk.mode = DIAL_KEY_MODE_END;
            }
        }
    break;
    case DIAL_KEY_MODE_END:
        spev_wrk.ret = 1;
    break;
    }

    if (
        dkey_wrk.mode != DIAL_KEY_MODE_READY && dkey_wrk.mode != DIAL_KEY_MODE_IN && dkey_wrk.mode != DIAL_KEY_MODE_SELOAD &&
        dkey_wrk.mode != DIAL_KEY_MODE_MSG0 && dkey_wrk.mode != DIAL_KEY_MODE_MSG01 && dkey_wrk.mode != DIAL_KEY_MODE_MSG02 &&
        dkey_wrk.mode != DIAL_KEY_MODE_MSG03 && dkey_wrk.mode != DIAL_KEY_MODE_OUT && dkey_wrk.mode != DIAL_KEY_MODE_GOUT
    )
    {
        CsrInclease(&spev_wrk.csr[1], 0x80, 3);
    }
    else if (dkey_wrk.mode == DIAL_KEY_MODE_OUT || dkey_wrk.mode == DIAL_KEY_MODE_GOUT)
    {
        CsrDeclease(&spev_wrk.csr[1], 0, 6);
    }

    if (dkey_wrk.mode == DIAL_KEY_MODE_SLCT)
    {
        CsrBlink(&spev_wrk.csr[2], 0x4e, 0, 2, spev_wrk.csr);
    }
    else if (dkey_wrk.mode == DIAL_KEY_MODE_BAD || dkey_wrk.mode == DIAL_KEY_MODE_BOUT || dkey_wrk.mode == DIAL_KEY_MODE_OUT || dkey_wrk.mode == DIAL_KEY_MODE_GOUT)
    {
        CsrDeclease(&spev_wrk.csr[2], 0, 3);
    }

    if (
        dkey_wrk.mode != DIAL_KEY_MODE_DEADWAIT && dkey_wrk.mode != DIAL_KEY_MODE_READY &&  dkey_wrk.mode != DIAL_KEY_MODE_GEXT &&
        dkey_wrk.mode != DIAL_KEY_MODE_EXIT && dkey_wrk.mode != DIAL_KEY_MODE_END
    )
    {
        DialKeyDoorDisp();
    }

    return; // missing return value
}

void DialKeyDoorDisp()
{
    int i;
    u_char alp_rate;
    SPRT_SDAT ssd;
    SPRT_SSCL sscl = {
        .cx = 0,
        .cy = 0,
        .sw = 80,
        .sh = 80,
    };
    DISP_STR ds;
    DISP_SQAR dsq;
    SQAR_DAT menu_sq0 = {
        .w = 592,
        .h = 100,
        .x = 19,
        .y = 328,
        .pri = 0x5000,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x64,
    };
    int msg_index;

    SetSprFile(EVENT_ADDRESS);

    if (dkey_wrk.mode == DIAL_KEY_MODE_IN)
    {
        alp_rate = (40 - dkey_wrk.time) * 100 / 40;
    }
    else if (dkey_wrk.mode == DIAL_KEY_MODE_OUT || dkey_wrk.mode == DIAL_KEY_MODE_GOUT)
    {
        alp_rate = dkey_wrk.time * 100 / 40;
    }
    else
    {
        alp_rate = 0x64;
    }

    for (i = 0; i < 11; i++)
    {
        SimpleDispSprt(&spev01_sp_bak[i], EVENT_ADDRESS, i, NULL, NULL, alp_rate);
    }

    sscl.cx = spev01_sp_btp[dkey_wrk.slct_no].x + spev01_sp_btp[i].w + 24;
    sscl.cy = spev01_sp_btp[dkey_wrk.slct_no].y + spev01_sp_btp[i].h + 20;

    SimpleDispSprtDatCopy(spev01_sp_btf + dkey_wrk.slct_no, &ssd);

    ssd.alp = spev_wrk.csr[2];

    SimpleDispSprt(&ssd, EVENT_ADDRESS, 16, NULL, &sscl, alp_rate);

    for (i = 0; i < 10; i++)
    {
        if (dkey_wrk.slct_no == i && dkey_wrk.mode == DIAL_KEY_MODE_PUSH)
        {
            SimpleDispSprt(&spev01_sp_btp[i], EVENT_ADDRESS, 17, NULL, NULL, alp_rate);
        }
        else
        {
            SimpleDispSprt(&spev01_sp_btn[i], EVENT_ADDRESS, 11, NULL, NULL, alp_rate);
        }
    }

    for (i = 0; i < dkey_dat[dkey_wrk.door_no].dial_num; i++)
    {
        if (dkey_wrk.slct_no == dkey_wrk.push_no[i])
        {
            break;
        }
    }

    SimpleDispSprtDatCopy(spev01_sp_cpf, &ssd);

    ssd.alp = spev_wrk.csr[1];

    SimpleDispSprt(&ssd, EVENT_ADDRESS, 18, NULL, NULL, alp_rate);

    if (dkey_wrk.count == 0)
    {
        SimpleDispSprtDatCopy(&spev01_sp_cpf[1], &ssd);

        ssd.h = spev01_sp_cpf[1].h / 2;
        ssd.alp = spev_wrk.csr[1];

        SimpleDispSprt(&ssd, EVENT_ADDRESS, 18, NULL, NULL, alp_rate);
    }
    else
    {
        SimpleDispSprtDatCopy(&spev01_sp_cpf[2], &ssd);

        ssd.y = spev01_sp_cpf[1].y;
        ssd.alp = spev_wrk.csr[1];

        SimpleDispSprt(&ssd, EVENT_ADDRESS, 18, NULL, NULL, alp_rate);
    }

    SimpleDispSprtDatCopy(&spev01_sp_cpf[1], &ssd);

    ssd.h = spev01_sp_cpf[1].h / 2;
    ssd.v = spev01_sp_cpf[1].v + ssd.h;
    ssd.y = spev01_sp_cpf[1].y + ssd.h;
    ssd.alp = spev_wrk.csr[1];

    SimpleDispSprt(&ssd, EVENT_ADDRESS, 18, NULL, NULL, alp_rate);

    SimpleDispSprtDatCopy(spev01_sp_cap, &ssd);

    ssd.alp = spev_wrk.csr[1];

    SimpleDispSprt(&ssd, EVENT_ADDRESS, 19, NULL, NULL, alp_rate);

    DrawButtonTex(0xa000, 3, spev01_cbt_pos[0][0], spev01_cbt_pos[0][1], spev_wrk.csr[1]);
    DrawButtonTex(0xa000, 1, spev01_cbt_pos[1][0], spev01_cbt_pos[1][1], spev_wrk.csr[1]);

    if (dkey_wrk.mode == DIAL_KEY_MODE_GOOD)
    {
        alp_rate = 150;
    }
    else if (dkey_wrk.mode == DIAL_KEY_MODE_BAD)
    {
        alp_rate = dkey_wrk.time * 100 / 40;
    }

    for (i = 0; i < dkey_dat[dkey_wrk.door_no].dial_num; i++)
    {
        if (dkey_wrk.push_no[i] != 0xff)
        {
            SimpleDispSprtDatCopy(&spev01_sp_num[dkey_wrk.push_no[i]], &ssd);

            ssd.x = spev01_nm4_pos[i][0];
            ssd.y = spev01_nm4_pos[i][1];

            if (i == dkey_wrk.count - 1 && dkey_wrk.mode == DIAL_KEY_MODE_PUSH)
            {
                ssd.alp = (ssd.alp * (40 - dkey_wrk.time)) / 40;
            }

            SimpleDispSprt(&ssd, EVENT_ADDRESS, 15, NULL, NULL, alp_rate);
        }
    }

    if (dkey_wrk.mode == DIAL_KEY_MODE_MSG02 || dkey_wrk.mode == DIAL_KEY_MODE_MSG03 || dkey_wrk.mode == DIAL_KEY_MODE_BOUT)
    {
        if (dkey_wrk.mode == DIAL_KEY_MODE_MSG02)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV00_MSG13);
        }
        else if (dkey_wrk.mode == DIAL_KEY_MODE_MSG03)
        {
            switch(dkey_wrk.door_no)
            {
            case 2:
                msg_index = SPEV00_MSG06;
            break;
            case 3:
                msg_index = SPEV00_MSG07;
            break;
            case 5:
                msg_index = SPEV00_MSG08;
            break;
            case 6:
                msg_index = SPEV00_MSG09;
            break;
            case 7:
                msg_index = SPEV00_MSG10;
            break;
            case 1:
            default:
                msg_index = SPEV00_MSG04;
            break;
            }

            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, msg_index);
        }
        else if (dkey_wrk.mode == DIAL_KEY_MODE_BOUT)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV00_MSG05);
        }

        spev_str.pos_x = 40;
        spev_str.pos_y = 345;

        CopyStrDToStr(&ds,&spev_str);
        SetMessageV2(&ds);

        CopySqrDToSqr(&dsq,&menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 100.0f, 0x7f);
    }
    else if (dkey_wrk.mode == DIAL_KEY_MODE_MSG0)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV00_MSG11);
        spev_str.pos_x = 40;
        spev_str.pos_y = 345;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 100.0f, 0x7f);
    }
    else if (dkey_wrk.mode == DIAL_KEY_MODE_MSG01)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV00_MSG12);
        spev_str.pos_x = 40;
        spev_str.pos_y = 345;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 100.0f, 0x7f);
    }
}

void DialKeyMSGDoorInit()
{
#ifdef BUILD_EU_VERSION
        LoadReqLanguage(EVTEX01_E_PK2, EVENT_ADDRESS);
#else
        FileLoadB(EVTEX01_PK2, EVENT_ADDRESS);
#endif

    dkey_wrk.mode = DIAL_KEY_MODE_READY;
    dkey_wrk.time = 40;

    SetBlackOut();
}

int DialKeyMSGDoorMain(int msg_no)
{
    switch (dkey_wrk.mode)
    {
    case DIAL_KEY_MODE_READY:
        if (dkey_wrk.time != 0)
        {
            dkey_wrk.time--;
        }
        else
        {
            if (IsLoadEndAll() != 0)
            {
                ingame_wrk.stts |= 0x20;

                dkey_wrk.mode = DIAL_KEY_MODE_IN;
                dkey_wrk.time = 40;

                SetBlackIn();
            }
        }
    break;
    case DIAL_KEY_MODE_IN:
        if (dkey_wrk.time != 0)
        {
            dkey_wrk.time--;
        }
        else
        {
            dkey_wrk.mode = DIAL_KEY_MODE_SLCT;
        }
    break;
    case DIAL_KEY_MODE_SLCT:
        if (*key_now[4] == 1 || *key_now[5] == 1)
        {
            dkey_wrk.mode = DIAL_KEY_MODE_OUT;
            dkey_wrk.time = 40;
        }
    break;
    case DIAL_KEY_MODE_OUT:
        if (dkey_wrk.time != 0)
        {
            dkey_wrk.time--;
        }
        else
        {
            ingame_wrk.stts &= ~0x20;

            dkey_wrk.mode = DIAL_KEY_MODE_EXIT;
            dkey_wrk.time = 40;

            SetBlackIn();
        }
    break;
    case DIAL_KEY_MODE_EXIT:
        if (dkey_wrk.time != 0)
        {
            dkey_wrk.time--;
            break;
        }

        dkey_wrk.mode = DIAL_KEY_MODE_END;
    break;
    case DIAL_KEY_MODE_END:
        spev_wrk.ret = 1;
    break;
    }

    if (dkey_wrk.mode != DIAL_KEY_MODE_READY && dkey_wrk.mode != DIAL_KEY_MODE_EXIT && dkey_wrk.mode != DIAL_KEY_MODE_END)
    {
        DialKeyMSGDoorDisp(msg_no);
    }

    return; // missing return value
}

void DialKeyMSGDoorDisp(int msg_no)
{
    int i;
    u_char alp_rate;
    SPRT_SDAT ssd;
    DISP_STR ds;
    DISP_SQAR dsq;
    SQAR_DAT menu_sq0 = {
        .w = 592,
        .h = 96,
        .x = 19,
        .y = 328,
        .pri = 0x5000,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x64
    };

    SetSprFile(EVENT_ADDRESS);

    if (dkey_wrk.mode == DIAL_KEY_MODE_IN)
    {
        alp_rate = ((40 - dkey_wrk.time) * 100) / 40;
    }
    else if (dkey_wrk.mode == DIAL_KEY_MODE_OUT || dkey_wrk.mode == DIAL_KEY_MODE_GOUT)
    {
        alp_rate = (dkey_wrk.time * 100) / 40;
    }
    else
    {
        alp_rate = 0x64;
    }

    for (i = 0; i < 11; i++)
    {
        SimpleDispSprt(&spev01_sp_bak[i], EVENT_ADDRESS, i, NULL, NULL, alp_rate);
    }

    for (i = 0; i < 10; i++)
    {
        SimpleDispSprt(&spev01_sp_btn[i], EVENT_ADDRESS, 11, NULL, NULL, alp_rate);
    }

    for (i = 0; i < 2; i++)
    {
        SimpleDispSprt(&spev01_sp_gl3[i], EVENT_ADDRESS, 12, NULL, NULL, alp_rate);
    }

    if (dkey_wrk.mode == DIAL_KEY_MODE_SLCT)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(7, msg_no);
        spev_str.pos_x = 50;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);
    }
}

void GhostDoorInit(int door_no)
{
    int i;

    spev_wrk.time = 40;
    spev_wrk.mode = 0;
    spev_wrk.count = 0;

    for (i = 0; i < 2; i++)
    {
        if (ev_wrk.gst_door[i] == door_no)
        {
            if (ev_wrk.spev_tmp[i] == 0xff)
            {
                spev_wrk.count = 1;
            }

            break;
        }
    }

    if (i == 2)
    {
        GhostDoorSet(door_no);
    }

    if (door_no != 5)
    {
#ifdef BUILD_EU_VERSION
        LoadReqLanguage(EVTEX02A_E_PK2, EVENT_ADDRESS);
#else
        LoadReq(EVTEX02A_PK2, EVENT_ADDRESS);
#endif
    }
    else if (spev_wrk.count == 0)
    {
#ifdef BUILD_EU_VERSION
        LoadReqLanguage(EVTEX02B_E_PK2, EVENT_ADDRESS);
#else
        LoadReq(EVTEX02B_PK2, EVENT_ADDRESS);
#endif
    }
    else
    {
#ifdef BUILD_EU_VERSION
        LoadReqLanguage(EVTEX02C_E_PK2, EVENT_ADDRESS);
#else
        LoadReq(EVTEX02C_PK2, EVENT_ADDRESS);
#endif
    }

    SpevStrInit();
    SetBlackOut();
}

void GhostDoorSet(int door_no)
{
    int i;

    for (i = 0; i < 2; i++)
    {
        if (ev_wrk.gst_door[i] == door_no)
        {
            return;
        }
    }

    for (i = 0; i < 2; i++)
    {
        if (ev_wrk.gst_door[i] == 0xff)
        {
            ev_wrk.gst_door[i] = door_no;
            ev_wrk.spev_tmp[i] = pghost_no_dat[door_no];

            break;
        }
    }
}

void GhostDoorMain(int door_no)
{
    int i;

    switch(spev_wrk.mode)
    {
    case 0:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEndAll() != 0)
            {
                spev_wrk.mode = 1;

                ingame_wrk.stts |= 0x20;

                spev_wrk.time = 40;

                SetBlackIn();
            }
        }
    break;
    case 1:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 2;
        }
    break;
    case 2:
        spev_wrk.mode = 3;
    break;
    case 3:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            ButtonMarkTimeClear();

            spev_wrk.mode = 4;
            spev_wrk.time = 40;

            SetBlackOut();
        }
    break;
    case 4:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            ingame_wrk.stts &= ~0x20;

            spev_wrk.mode = 5;
            spev_wrk.time = 40;

            SetBlackIn();
        }
    break;
    case 5:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (spev_wrk.count != 0)
            {
                event_stts[ev_wrk.evt_no] = 1;

                for (i = 0; i < 2; i++)
                {
                    if (ev_wrk.gst_door[i] == door_no)
                    {
                        ev_wrk.gst_door[i] = 0xff;
                    }
                }
            }

            SendManMdlTex();

            spev_wrk.ret = 1;
        }
    break;
    }

    if (spev_wrk.mode != 0 && spev_wrk.mode != 5)
    {
        GhostDoorDisp(door_no);
    }
}

void GhostDoorDisp(int door_no)
{
    int i;
    SPRT_SDAT ssd;
    DISP_STR ds;
    DISP_SQAR dsq;
    SQAR_DAT menu_sq0 = {
        .w = 592,
        .h = 96,
        .x = 19,
        .y = 328,
        .pri = 0x5000,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x3c,
    };

    SetSprFile(EVENT_ADDRESS);

    if (door_no != 5)
    {
        for (i = 0; i < 11; i++)
        {
            SimpleDispSprt(&spev02_sp_bak[i], EVENT_ADDRESS, i, NULL, NULL, 100);
        }

        if (spev_wrk.count != 0)
        {
            SimpleDispSprt(&spev02_sp_of1[0], EVENT_ADDRESS, 11, NULL, NULL, 100);
        }
    }
    else
    {
        for (i = 0; i < 11; i++)
        {
            SimpleDispSprt(&spev02_sp_bak[i], EVENT_ADDRESS, i, NULL, NULL, 100);
        }
    }

    if (spev_wrk.mode == 2 || spev_wrk.mode == 3)
    {
        if (spev_wrk.count != 0)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV01_MSG01);
        }
        else if (door_no != 0)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV01_MSG02);
        }
        else
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV01_MSG00);
        }

        spev_str.pos_x = 50;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = 338;
#else
        spev_str.pos_y = 350;
#endif

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0, 328.0, 592.0, 96.0, 0x7f);
    }
}

void DollPzlInit()
{
    AdpcmPrePuzzleFadeOut(40);

    pzl_load_id = -1;

    if (plyr_wrk.se_deadly != -1)
    {
        SeFadeFlame(plyr_wrk.se_deadly, 0x3c ,0);

        plyr_wrk.se_deadly = -1;

        spev_wrk.time = 60;
        spev_wrk.mode = 0;
    }
    else
    {
#ifdef BUILD_EU_VERSION
        LoadReqLanguage(EVTEX03_E_PK2, EVENT_ADDRESS);
#else
        LoadReq(EVTEX03_PK2, EVENT_ADDRESS);
#endif

        spev_wrk.mode = 1;
        spev_wrk.time = 40;
    }

    SeStopAndBackup();

    spev_wrk.csr[0] = 4;
    spev_wrk.csr[1] = 0;
    spev_wrk.count = 0;

    SpevStrInit();
    SetBlackOut();
}

void DollPzlMain()
{
    int no;
    int i;
    static int blink_dir = 1;

    no = spev_wrk.count;

    switch(spev_wrk.mode)
    {
    case 0:
#ifdef BUILD_EU_VERSION
        LoadReqLanguage(EVTEX03_E_PK2, EVENT_ADDRESS);
#else
        LoadReq(EVTEX03_PK2, EVENT_ADDRESS);
#endif

        spev_wrk.mode = 1;
        spev_wrk.time = 40;
    break;
    case 1:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEndAll() != 0)
            {
                SetSprFile(EVENT_ADDRESS);

                spev_wrk.mode = 2;

                ingame_wrk.stts |= 0x20;

                pzl_load_id = SeFileLoadAndSet(SP001_NINGYOU_BD, 2);
            }
        }
    break;
    case 2:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEnd(pzl_load_id) != 0)
            {
                pzl_load_id = -1;

                spev_wrk.mode = 4;

                SetBlackIn();
            }
        }
    break;
    case 4:
        if (IsEndAdpcmPuzzle() != 0)
        {
            AdpcmPreLoadPuzzle(AE001_USIRO_STR);

            spev_wrk.mode = 5;
        }
    break;
    case 5:
        if (IsPreLoadEndAdpcmPuzzle() != 0)
        {
            spev_wrk.mode = 6;
            spev_wrk.time = 60;

            AdpcmPuzzlePreLoadEndPlay();
        }
    break;
    case 6:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            for (i = 0; i < 9; i++)
            {
                if (evdl_dat[no].order[i] != 0xff)
                {
                    spev_wrk.csr[0] = i;
                }
            }

            spev_wrk.mode = 7;
            spev_wrk.time = 60;
        }
    break;
    case 7:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 8;
            spev_wrk.time = 20;
        }
    break;
    case 8:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsEndAdpcmPuzzle() != 0)
            {
                spev_wrk.mode = 10;
                spev_wrk.time = 20;
            }
        }
    break;
    case 9:
        if (*key_now[5] == 1 || *key_now[4] == 1)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            spev_wrk.mode = 10;
            spev_wrk.time = 20;
        }
    break;
    case 10:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }

        if (*key_now[4] == 1)
        {
            spev_wrk.mode = 15;
            spev_wrk.time = 40;

            SetBlackOut();
            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (*key_now[5] == 1)
        {
            spev_wrk.mode = 11;

            SeStartFix(1, 0, 0x1000, 0x1000, 0);
        }
        else if (
            *key_now[0] == 1 ||
            (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
            Ana2PadDirCnt(0) == 1 ||
            (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
        )
        {
            if (spev_wrk.csr[0] >= 3)
            {
                if (evdl_dat[no].order[spev_wrk.csr[0] - 3] != 0xff)
                {
                    spev_wrk.csr[0] -= 3;
                    spev_wrk.csr[1] = 0;
                    spev_wrk.time = 20;
                }

                SeStartFix(0, 0, 0x1000, 0x1000, 0);
            }
        }
        else if (
            *key_now[1] == 1 ||
            (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
            Ana2PadDirCnt(2) == 1 ||
            (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
        )
        {
            if (spev_wrk.csr[0] < 6)
            {
                if (evdl_dat[no].order[spev_wrk.csr[0] + 3] != 0xff)
                {
                    spev_wrk.csr[0] += 3;
                    spev_wrk.csr[1] = 0;
                    spev_wrk.time = 20;
                }

                SeStartFix(0, 0, 0x1000, 0x1000, 0);
            }
        }
        else if (
            *key_now[2] == 1 ||
            (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
            Ana2PadDirCnt(3) == 1 ||
            (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
        )
        {
            if (spev_wrk.csr[0] % 3 != 0)
            {
                if (evdl_dat[no].order[spev_wrk.csr[0] - 1] != 0xff)
                {
                    spev_wrk.csr[0]--;
                    spev_wrk.csr[1] = 0;
                    spev_wrk.time = 20;
                }

                SeStartFix(0, 0, 0x1000, 0x1000, 0);
            }
        }
        else if (
            *key_now[3] == 1 ||
            (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
            Ana2PadDirCnt(1) == 1 ||
            (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
        )
        {
            if (spev_wrk.csr[0] % 3 != 2)
            {
                if (evdl_dat[no].order[spev_wrk.csr[0] + 1] != 0xff)
                {
                    spev_wrk.csr[0]++;
                    spev_wrk.csr[1] = 0;
                    spev_wrk.time = 20;
                }

                SeStartFix(0, 0, 0x1000, 0x1000, 0);
            }
        }
    break;
    case 11:
        if (*key_now[4] == 1)
        {
            spev_wrk.mode = 10;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (*key_now[5] == 1)
        {
            SeStartFix(35, 0, 0x1000, 0x1000, 0);
            if (spev_wrk.csr[0] == evdl_dat[no].answer)
            {
                spev_wrk.mode = 21;
                spev_wrk.time = 180;

                AdpcmPuzzlePlay(AVP001_STR);
            }
            else
            {
                spev_wrk.mode = 16;
                spev_wrk.time = 130;

                AdpcmPuzzlePlay(AVP002_STR);
            }
        }
        else if (
            (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            ) || (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            )
        )
        {
            spev_wrk.mode = 12;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
    break;
    case 12:
        if (*key_now[4] == 1)
        {
            spev_wrk.mode = 10;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (*key_now[5] == 1)
            {
                spev_wrk.mode = 10;

                SeStartFix(3, 0, 0x1000, 0x1000, 0);
            }
        else if (
            (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            ) || (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            )
        )
        {
            spev_wrk.mode = 11;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
    break;
    case 13:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (no < 5)
            {
                spev_wrk.count = spev_wrk.count + 1;
            }
            else
            {
                spev_wrk.count = 4;
            }

            spev_wrk.mode = 6;
            spev_wrk.time = 60;

            AdpcmPuzzlePreLoadEndPlay();
            SetBlackIn();
        }
    break;
    case 16:
        if (spev_wrk.time > 2)
        {
            spev_wrk.time -= 2;
        }

        if (IsEndAdpcmPuzzle() != 0)
        {
            if (spev_wrk.time <= 2)
            {
                spev_wrk.mode = 13;
                spev_wrk.time = 40;

                SetBlackOut();
                AdpcmPreLoadPuzzle(AE001_USIRO_STR);
            }
        }
    break;
    case 17:
        if (IsEndAdpcmPuzzle() != 0)
        {
            if ((*key_now[5] == 1) || (*key_now[4] == 1))
            {
                spev_wrk.mode = 20;

                AdpcmPuzzlePlay(AVP000_STR);
            }
            else if (spev_wrk.time > 2)
            {
                spev_wrk.time -= 2;
            }
            else
            {
                spev_wrk.mode = 18;
                spev_wrk.time = 130;
            }
        }
    break;
    case 18:
        if (IsEndAdpcmPuzzle() != 0)
        {
            if (*key_now[5] == 1 || *key_now[4] == 1)
            {
                spev_wrk.mode = 20;

                AdpcmPuzzlePlay(AVP000_STR);
            }
            else if (spev_wrk.time > 2)
            {
                spev_wrk.time -= 2;
            }

            else if (evdl_dat[no].unfade_doll[2] == 0xff) {
                spev_wrk.mode = 19;
                spev_wrk.time = 10;
            }
            else
            {
                spev_wrk.mode = 20;
                spev_wrk.time = 130;

                AdpcmPuzzlePlay(AVP000_STR);
            }
        }
    break;
    case 19:
        if (IsEndAdpcmPuzzle() != 0)
        {
            if (*key_now[5] == 1 || *key_now[4] == 1)
            {
                spev_wrk.mode = 20;

                AdpcmPuzzlePlay(AVP000_STR);
            }
            else if (spev_wrk.time < 228)
            {
                spev_wrk.time += 2;
            }
            else
            {
                spev_wrk.mode = 20;
                spev_wrk.time = 230;

                AdpcmPuzzlePlay(AVP000_STR);
            }
        }
    break;
    case 20:
        if (spev_wrk.time < 230 && blink_dir > 0)
        {
            spev_wrk.time += 2;
        }
        else if (spev_wrk.time >= 64 && blink_dir < 0)
        {
            spev_wrk.time -= 2;
        }
        else if (spev_wrk.time < 64 && blink_dir < 0)
        {
            blink_dir = 1;
        }
        else if (spev_wrk.time >= 230 && blink_dir > 0)
        {
            blink_dir = -1;
        }

        if (IsEndAdpcmPuzzle() != 0)
        {
            if (*key_now[5] == 1)
            {
                spev_wrk.time = 0;

                SeStartFix(1, 0, 0x1000, 0x1000, 0);
            }

            if (spev_wrk.time == 0)
            {
                blink_dir = 1;

                spev_wrk.mode = 13;
                spev_wrk.time = 40;

                SetBlackOut();
                AdpcmPreLoadPuzzle(AE001_USIRO_STR);
            }
        }
    break;
    case 21:
        if (spev_wrk.time > 5)
        {
            spev_wrk.time -= 5;
        }

        if (IsEndAdpcmPuzzle() != 0)
        {
            spev_wrk.mode = 24;

            SetBlackOut();

            spev_wrk.time = 40;

            event_stts[ev_wrk.evt_no] = 1;
        }
    case 22:
        if (spev_wrk.time > 5)
        {
            spev_wrk.time -= 5;
        }

        if (IsEndAdpcmPuzzle() != 0 && *key_now[5] == 1)
        {
            spev_wrk.mode = 23;
        }
    break;
    case 23:
        if (spev_wrk.time < 230 && blink_dir > 0)
        {
            spev_wrk.time += 5;
        }
        else if (spev_wrk.time >= 64 && blink_dir < 0)
        {
            spev_wrk.time -= 5;
        }
        else if (spev_wrk.time < 64 && blink_dir < 0)
        {
            blink_dir = 1;
        }
        else if (spev_wrk.time >= 230 && blink_dir > 0)
        {
            blink_dir = -1;
        }

        if (*key_now[5] == 1)
        {
            spev_wrk.time = 0;

            SeStartFix(1, 0, 0x1000, 0x1000, 0);
        }
        else if (*key_now[4] == 1)
        {
            spev_wrk.time = 0;

            SeStartFix(1, 0, 0x1000, 0x1000, 0);
        }

        if (spev_wrk.time == 0)
        {
            spev_wrk.mode = 24;
            spev_wrk.time = 40;

            blink_dir = 1;

            event_stts[ev_wrk.evt_no] = 1;

            SetBlackOut();
        }
    break;
    case 15:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 14;
        }
    break;
    case 24:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 14;
        }
    break;
    case 14:
        if (pzl_load_id == -1)
        {
            pzl_load_id = SeFileLoadAndSet(SV000_MIKU_BD, 2);
        }
        else
        {
            if (IsLoadEnd(pzl_load_id) != 0)
            {
                spev_wrk.ret = 1;

                ingame_wrk.stts &= ~0x20;

                SetBlackIn();

                pzl_load_id = -1;

                AdpcmPuzzleEnd();
                SeRevival(30);
                SendManMdlTex();
            }
        }
    break;
    }

    if (spev_wrk.mode != 0 && spev_wrk.mode != 1 && spev_wrk.mode != 3 && spev_wrk.mode != 14)
    {
        SpecialEventDisp003(no);
    }
}

int CursorManagerEvent003()
{
    static int blink_dir = 1;

    if (spev_wrk.mode != 6 && spev_wrk.mode != 7 && spev_wrk.mode != 8 && spev_wrk.mode != 9)
    {
        if (spev_wrk.mode == 10)
        {
            if (spev_wrk.csr[1] < 125)
            {
                spev_wrk.csr[1] += 5;
            }
            else
            {
                spev_wrk.csr[1] = 130;
            }
        }
        else if (spev_wrk.mode == 21 || spev_wrk.mode == 22 || spev_wrk.mode == 23)
        {
            if (spev_wrk.csr[1] <= 229 && blink_dir > 0)
            {
                spev_wrk.csr[1] += 5;
            }
            else
            {
                if (spev_wrk.csr[1] >= 64 && blink_dir < 0)
                {
                    spev_wrk.csr[1] -= 5;
                }
                else if (blink_dir < 0)
                {
                    blink_dir = 1;
                }
                else if (spev_wrk.csr[1] > 229 && blink_dir > 0)
                {
                    blink_dir = -1;
                }
            }
        }
        else if (spev_wrk.mode == 16)
        {
            if (spev_wrk.csr[1] > 2)
            {
                spev_wrk.csr[1] -= 2;
            }
            else
            {
                spev_wrk.csr[1] = 0;
            }
        }
        else if (spev_wrk.mode == 17 || spev_wrk.mode == 18 || spev_wrk.mode == 19 || spev_wrk.mode == 20)
        {
            spev_wrk.csr[1] = 0;
        }
        else if (spev_wrk.mode == 13)
        {
            spev_wrk.csr[1] = 0;
        }
        else if (spev_wrk.mode == 15 || spev_wrk.mode == 24 || spev_wrk.mode == 14)
        {
            spev_wrk.csr[1] = 0;
        }
    }
    else
    {
        spev_wrk.csr[1] = 0;
    }

    return spev_wrk.csr[1];
}

void SpecialEventDisp003(int no)
{
    int i;
    static int next_count = 0;
    u_char ssd_tmp_col;
    SPRT_SDAT ssd;
    SPRT_SDAT ssd_csr;
    DISP_STR ds;
    DISP_SQAR dsq;
    SQAR_DAT menu_sq0 = {
        .w = 576,
        .h = 90,
        .x = 27,
        .y = 331,
        .pri = 0x5000,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x3c,
    };

    SetSprFile(EVENT_ADDRESS);

    for (i = 0; i < 11; i++)
    {
        SimpleDispSprt(&spev03_sp_bak[i], EVENT_ADDRESS, i, NULL, NULL, 0x64);
    }

    SimpleDispSprtDatCopy(&spev03_sp_dla[spev_wrk.csr[0]], &ssd_csr);

    ssd_csr.alp = CursorManagerEvent003();

    if (evdl_dat[no].order[spev_wrk.csr[0]] == 1 || evdl_dat[no].order[spev_wrk.csr[0]] == 3)
    {
        ssd_csr.u = 1;
        ssd_csr.v = 257;
    }
    else if (evdl_dat[no].order[spev_wrk.csr[0]] == 2)
    {
        ssd_csr.u = 196;
        ssd_csr.v = 1;
    }

    for (i = 0; i < 9; i++)
    {
        ssd_tmp_col = 0x80;

        if (evdl_dat[no].order[i] != 0xff)
        {
            SimpleDispSprtDatCopy(&spev03_sp_dol[i], &ssd);

            if (evdl_dat[no].order[i] == 1 || evdl_dat[no].order[i] == 3)
            {
                ssd.u = 295;
            }
            else if (evdl_dat[no].order[i] == 2)
            {
                ssd.u = 148;
            }

            if (spev_wrk.mode == 16)
            {
                if (spev_wrk.time > 10)
                {
                    ssd_tmp_col = spev_wrk.time;
                }
                else if (spev_wrk.time <= 10)
                {
                    ssd_tmp_col = 10;
                }
            }

            if (spev_wrk.mode == 13)
            {
                ssd_tmp_col = 10;
            }

            if ((spev_wrk.mode == 21 || spev_wrk.mode == 22) && evdl_dat[no].answer != i)
            {
                if (spev_wrk.time != 0)
                {
                    ssd.alp = spev_wrk.time;
                }
                else
                {
                    ssd.alp = 0;
                }
            }

            if ((spev_wrk.mode == 23 || spev_wrk.mode== 24 || spev_wrk.mode == 14) && evdl_dat[no].answer != i)
            {
                ssd.alp = 0;
            }

            if (i < 6)
            {
                if (spev_wrk.csr[0] == i)
                {
                    if (spev_wrk.csr[0] < 6)
                    {
                        if (evdl_dat[no].order[i] == 1)
                        {
                            SimpleDispSprt(&ssd_csr, EVENT_ADDRESS, 12, &spev03_dol_left, &spev03_dla_scl[i], 0x64);
                        }
                        else
                        {
                            SimpleDispSprt(&ssd_csr, EVENT_ADDRESS, 12, NULL, &spev03_dla_scl[i], 0x64);
                        }
                    }
                    else
                    {
                        if (evdl_dat[no].order[i] == 1)
                        {
                            SimpleDispSprt(&ssd_csr, EVENT_ADDRESS, 12, &spev03_dol_left, NULL, 0x64);
                        }
                        else
                        {
                            SimpleDispSprt(&ssd_csr, EVENT_ADDRESS, 12, NULL, NULL, 0x64);
                        }
                    }
                }

                if (evdl_dat[no].order[i] == 1)
                {
                    SimpleDispSprtRGB(&ssd, EVENT_ADDRESS, 11, &spev03_dol_left, &spev03_dol_scl[i], 0x64, ssd_tmp_col, ssd_tmp_col, ssd_tmp_col);
                }
                else
                {
                    SimpleDispSprtRGB(&ssd, EVENT_ADDRESS, 11, NULL, &spev03_dol_scl[i], 0x64, ssd_tmp_col, ssd_tmp_col, ssd_tmp_col);
                }
            }
            else
            {
                if (spev_wrk.csr[0] == i)
                {
                    if (spev_wrk.csr[0] < 6)
                    {
                        if (evdl_dat[no].order[i] == 1)
                        {
                            SimpleDispSprt(&ssd_csr, EVENT_ADDRESS, 12, &spev03_dol_left, &spev03_dla_scl[i], 0x64);
                        }
                        else
                        {
                            SimpleDispSprt(&ssd_csr, EVENT_ADDRESS, 12, NULL, &spev03_dla_scl[i], 0x64);
                        }

                    }
                    else {
                        if (evdl_dat[no].order[i] == 1)
                        {
                            SimpleDispSprt(&ssd_csr, EVENT_ADDRESS, 12, &spev03_dol_left, NULL, 0x64);
                        }
                        else
                        {
                            SimpleDispSprt(&ssd_csr, EVENT_ADDRESS, 12, NULL, NULL, 0x64);
                        }
                    }
                }

                if (evdl_dat[no].order[i] == 1)
                {
                    SimpleDispSprtRGB(&ssd, EVENT_ADDRESS, 11, &spev03_dol_left, NULL, 0x64, ssd_tmp_col, ssd_tmp_col, ssd_tmp_col);
                }
                else
                {
                    SimpleDispSprtRGB(&ssd, EVENT_ADDRESS, 11, NULL, NULL, 0x64, ssd_tmp_col, ssd_tmp_col, ssd_tmp_col);
                }
            }
        }
    }

    if (spev_wrk.mode == 6)
    {
        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV03_MSG09);
#ifdef BUILD_EU_VERSION
        spev_str.pos_x = 320 - GetStrWidth(spev_str.str) / 2;
#else
        spev_str.pos_x = 120;
#endif
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }

    if (spev_wrk.mode == 7)
    {
        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV03_MSG09);
#ifdef BUILD_EU_VERSION
        spev_str.pos_x = 320 - GetStrWidth(spev_str.str) / 2;
#else
        spev_str.pos_x = 120;
#endif
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }

    if (spev_wrk.mode == 8)
    {
        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV03_MSG09);
#ifdef BUILD_EU_VERSION
        spev_str.pos_x = 320 - GetStrWidth(spev_str.str) / 2;
#else
        spev_str.pos_x = 120;
#endif
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }

    if (spev_wrk.mode == 11 || spev_wrk.mode == 12)
    {
        CopySqrDToSqr(&dsq, &spev_sq0);
        DispSqrD(&dsq);

        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV03_MSG00);
#ifdef BUILD_EU_VERSION
        spev_str.pos_x = 320 - GetStrWidth(spev_str.str) / 2;
#else
        spev_str.pos_x = 195;
#endif
        spev_str.pos_y = 345;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_NO);
        spev_str.pos_x = 320;
        spev_str.pos_y = 375;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_YES);
        spev_str.pos_x = 220;
        spev_str.pos_y = 375;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (spev_wrk.mode == 11)
        {
            YesNoCrslOKR(0x2000, 200.0f, 370.0f, 0x808080, 100.0f, 1.5f);
        }
        else
        {
            YesNoCrslOKR(0x2000, 290.0f, 370.0f, 0x808080, 100.0f, 1.5f);
        }
    }
    else if (spev_wrk.mode == 23)
    {
        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV03_MSG02);
        spev_str.pos_x = 170;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }
#ifdef BUILD_EU_VERSION
    else if (spev_wrk.mode == 21)
    {
        if (sys_wrk.language != 0)
        {
            DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV03_MSG03);
            spev_str.pos_x = 320 - GetStrWidth(spev_str.str) / 2;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
    }
    else if (spev_wrk.mode == 16)
    {
        if (sys_wrk.language != 0)
        {
            DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV03_MSG04);
            spev_str.pos_x = 320 - GetStrWidth(spev_str.str) / 2;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
    }
#endif

    if (spev_wrk.mode == 23)
    {
        ButtonMarkNext(0, 0, 1);
    }

    next_count++;

    if (next_count > 80)
    {
        next_count = 0;
    }

    SimpleDispSprt(&spev03_sp_cap[0], EVENT_ADDRESS, 11, NULL, NULL, 0x64);
#ifdef BUILD_EU_VERSION
    SimpleDispSprt(&spev03_sp_cap[1], EVENT_ADDRESS, 11, NULL, NULL, 0x64);
    SimpleDispSprt(&spev03_sp_cap[2], EVENT_ADDRESS, 11, NULL, NULL, 0x64);
    SimpleDispSprt(&spev03_sp_cap[3], EVENT_ADDRESS, 11, NULL, NULL, 0x64);

    DrawButtonTex(0xa000, 3, spev03_cbt_pos[0][0], spev03_cbt_pos[0][1], 0x80);
    DrawButtonTex(0xa000, 1, spev03_cbt_pos[1][0], spev03_cbt_pos[1][1], 0x80);
#else
    DrawButtonTex(0xa000, 3, spev04_cbt_pos[0][0] - 6, spev04_cbt_pos[0][1], 0x80);
    DrawButtonTex(0xa000, 1, spev04_cbt_pos[1][0] - 6, spev04_cbt_pos[1][1], 0x80);
#endif
}

void ButsuzoPzlInit()
{
    int i;

    if (plyr_wrk.se_deadly != -1)
    {
        SeFadeFlame(plyr_wrk.se_deadly, 0x3c, 0);

        plyr_wrk.se_deadly = -1;

        spev_wrk.time = 60;
        spev_wrk.mode = 0;
    }
    else
    {
#ifdef BUILD_EU_VERSION
        LoadReqLanguage(EVTEX04_E_PK2, EVENT_ADDRESS);
#else
        LoadReq(EVTEX04_PK2, EVENT_ADDRESS);
#endif

        pzl_load_id = -1;

        spev_wrk.mode = 1;
        spev_wrk.time = 40;
    }

    spev_wrk.csr[0] = 0;
    spev_wrk.csr[1] = 0;
    spev_wrk.csr[2] = 0;

    spev_wrk.count = 0;

    for (i = 0; i < 5; i++)
    {
        evbtz_dat[i].use_flg = 0;
        evbtz_dat[i].set_place = -1;

        order_keep[i] = -1;
    }

    order_no = 0;

    evbld_dat.t_counter = 0;

    SpevStrInit();
    SetBlackOut();
}

void ButsuzoPzlMain()
{
    int i;
    static int t_counter = 0;

    switch(spev_wrk.mode)
    {
    case 0:
#ifdef BUILD_EU_VERSION
        LoadReqLanguage(EVTEX04_E_PK2, EVENT_ADDRESS);
#else
        LoadReq(EVTEX04_PK2, EVENT_ADDRESS);
#endif

        SeFileLoadAndSet(SP003_BUTUDAN_BD, 2);

        pzl_load_id = -1;

        spev_wrk.mode = 1;
        spev_wrk.time = 40;
    break;
    case 1:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEndAll() != 0)
            {
                SetSprFile(EVENT_ADDRESS);

                spev_wrk.mode = 2;

                ingame_wrk.stts |= 0x20;

                pzl_load_id = SeFileLoadAndSet(SP003_BUTUDAN_BD, 2);
            }
        }
    break;
    case 2:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEnd(pzl_load_id) != 0)
            {
                pzl_load_id = -1;

                spev_wrk.mode = 3;
                spev_wrk.time = 40;

                SetBlackIn();
            }
        }
    break;
    case 3:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 4;
            spev_wrk.time = 20;
        }
    break;
    case 4:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }

        if (*key_now[4] == 1)
        {
            if (order_no != 0)
            {
                order_no--;

                evbtz_dat[order_keep[order_no]].use_flg = 0;
                evbtz_dat[order_keep[order_no]].set_place = -1;

                order_keep[order_no] = -1;
            }
            else
            {
                spev_wrk.mode = 15;
                spev_wrk.time = 60;

                SetBlackOut();
            }
            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (*key_now[5] == 1)
        {
            if (evbtz_dat[spev_wrk.csr[0]].set_place == -1)
            {
                spev_wrk.csr[1] = 8;
            }
            else
            {
                spev_wrk.csr[1] = evbtz_dat[spev_wrk.csr[0]].set_place << 1;
            }

            spev_wrk.mode = 5;
            spev_wrk.time = 20;

            SeStartFix(33, 0, 0x1000, 0x1000, 0);
        }
        else if (
            *key_now[3] == 1 ||
            (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
            Ana2PadDirCnt(1) == 1 ||
            (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
        )
        {
            for (i = 1; i < 6; i++)
            {
                if (evbtz_dat[(spev_wrk.csr[0] + i) % 5].use_flg == 0)
                {
                    spev_wrk.csr[0] = (spev_wrk.csr[0] + i) % 5;
                    spev_wrk.time = 20;

                    break;
                }

                if (i == 6) // bug ??? should have been outside the loop ???
                {
                    spev_wrk.mode = 5;
                }
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
            for (i = 1; i < 6; i++)
            {
                if (spev_wrk.csr[0] - i >= 0)
                {
                    if (evbtz_dat[spev_wrk.csr[0] - i].use_flg == 0)
                    {
                        spev_wrk.csr[0] = ((spev_wrk.csr[0] - i) % 5);
                        spev_wrk.time = 20;

                        break;
                    }
                }
                else
                {
                    if (evbtz_dat[spev_wrk.csr[0] + 5 - i].use_flg == 0)
                    {
                        spev_wrk.csr[0] = spev_wrk.csr[0] + 5 - i;
                        spev_wrk.time = 20;

                        break;
                    }
                }
            }

            if (i == 6)
            {
                spev_wrk.mode = 5;
            }

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
    break;
    case 5:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }

        if (*key_now[4] == 1)
        {
            evbtz_dat[spev_wrk.csr[0]].use_flg = 0;
            evbtz_dat[spev_wrk.csr[0]].set_place = -1;

            spev_wrk.mode = 4;
        }
        else if (*key_now[5] == 1)
        {
            for (i = 0; i < 5; i++)
            {
                if (evbtz_dat[i].set_place == (spev_wrk.csr[1] >> 1) && i != spev_wrk.csr[0])
                {
                    break;
                }
            }

            if (i != 5)
            {
                evbtz_dat[spev_wrk.csr[0]].set_place = -1;
                spev_wrk.mode = 9;
                spev_wrk.time = 60;

                break;
            }

            evbtz_dat[spev_wrk.csr[0]].use_flg = 1;
            evbtz_dat[spev_wrk.csr[0]].set_place = spev_wrk.csr[1] >> 1;

            order_keep[order_no] = spev_wrk.csr[0];
            order_no++;

            spev_wrk.mode = 8;
            spev_wrk.time = 40;

            for (i = 0; i < 5; i++)
            {
                if (evbtz_dat[i].use_flg == 0)
                {
                    spev_wrk.csr[0] = i;

                    break;
                }
            }

            if (i == 5)
            {
                spev_wrk.mode = 10;
            }

            SeStartFix(34, 0, 0x1000, 0x1000, 0);
        }
        else if (
            *key_now[0] == 1 ||
            (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
            Ana2PadDirCnt(0) == 1 ||
            (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
        )
        {
            if ((spev_wrk.csr[1] + 2) % 6 == 0)
            {
                spev_wrk.csr[1] -= 4;

                evbtz_dat[spev_wrk.csr[0]].set_place = spev_wrk.csr[1] >> 1;
            }
            else
            {
                spev_wrk.csr[1] += 2;

                evbtz_dat[spev_wrk.csr[0]].set_place = spev_wrk.csr[1] >> 1;
            }

            spev_wrk.time = 20;
        }
        else
        {
            if (
                *key_now[1] == 1 ||
                (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
                Ana2PadDirCnt(2) == 1 ||
                (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
            )
            {
                if (spev_wrk.csr[1] % 6 == 0)
                {
                    spev_wrk.csr[1] += 4;

                    evbtz_dat[spev_wrk.csr[0]].set_place = spev_wrk.csr[1] >> 1;
                }
                else
                {
                    spev_wrk.csr[1] -= 2;

                    evbtz_dat[spev_wrk.csr[0]].set_place = spev_wrk.csr[1] >> 1;
                }

                spev_wrk.time = 20;
            }
            else if (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            )
            {
                if (spev_wrk.csr[1] < 5)
                {
                    spev_wrk.csr[1] += 12;

                    evbtz_dat[spev_wrk.csr[0]].set_place = spev_wrk.csr[1] >> 1;
                }
                else
                {
                    spev_wrk.csr[1] -= 6;

                    evbtz_dat[spev_wrk.csr[0]].set_place = spev_wrk.csr[1] >> 1;
                }

                spev_wrk.time = 20;
            }
            else if (
                    *key_now[3] == 1 ||
                    (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                    Ana2PadDirCnt(1) == 1 ||
                    (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            )
            {
                if (spev_wrk.csr[1] >= 11)
                {
                    spev_wrk.csr[1] -= 12;

                    evbtz_dat[spev_wrk.csr[0]].set_place = spev_wrk.csr[1] >> 1;
                }
                else
                {
                    spev_wrk.csr[1] += 6;

                    evbtz_dat[spev_wrk.csr[0]].set_place = spev_wrk.csr[1] >> 1;
                }

                spev_wrk.time = 20;
            }
        }
    break;
    case 6:
    case 7:
        if (*key_now[5] == 1)
        {
            spev_wrk.time = 0;
        }

        if (*key_now[4] == 1)
        {
            spev_wrk.mode = 5;
            spev_wrk.time = 20;
        }
        if (
            (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            ) || (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            )
        )
        {
            spev_wrk.mode = spev_wrk.mode == 6 ? 7 : 6;
        }

        if (spev_wrk.time == 0)
        {
            if (spev_wrk.mode == 6)
            {
                evbtz_dat[spev_wrk.csr[0]].use_flg = 1;
                evbtz_dat[spev_wrk.csr[0]].set_place = spev_wrk.csr[1] >> 1;

                order_keep[order_no] = spev_wrk.csr[0];
                order_no++;

                spev_wrk.mode = 8;
                spev_wrk.time = 40;

                for (i = 0; i < 5; i++)
                {
                    if (evbtz_dat[i].use_flg == 0)
                    {
                        spev_wrk.csr[0] = i;

                        break;
                    }
                }

                if (i == 5)
                {
                    spev_wrk.mode = 10;
                }
            }
            else
            {
                spev_wrk.mode = 5;
                spev_wrk.time = 20;
            }
        }
    break;
    case 8:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 4;
            spev_wrk.time = 20;
        }
    break;
    case 9:
        if (*key_now[5] == 1)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            spev_wrk.mode = 4;
            spev_wrk.time = 20;
        }
    break;
    case 10:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            for (i = 0; i < 5; i++)
            {
                if (evbtz_dat[i].set_place != evbtz_ans[i].set_place)
                {
                    break;
                }
            }

            if (i == 5)
            {
                spev_wrk.mode = 13;
                spev_wrk.time = 80;
                SeStartFix(35, 0, 0x1000, 0x1000, 0);
            }
            else
            {
                spev_wrk.mode = 12;
                spev_wrk.time = 80;
            }
        }
    break;
    case 13:
        if (ButtonMarkNext(-180, 0, 1) != 0)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            ButtonMarkTimeClear();

            spev_wrk.mode = 15;
            spev_wrk.time = 60;

            event_stts[ev_wrk.evt_no] = 1;

            SetBlackOut();
        }
    break;
    case 12:
        if (ButtonMarkNext(-180, 8, 1) != 0)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0 && ButtonMarkWait() == 0)
        {
            spev_wrk.time = 20;
            spev_wrk.csr[0] = 0;
            spev_wrk.csr[1] = 0;
            spev_wrk.count = 0;

            order_no = 0;

            for (i = 0; i < 5; i++)
            {
                evbtz_dat[i].use_flg = 0;
                evbtz_dat[i].set_place = -1;

                order_keep[i] = -1;
            }

            spev_wrk.mode = 4;
        }
    break;
    case 15:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 16;
        }
    break;
    case 16:
        if (pzl_load_id == -1)
        {
            pzl_load_id = SeFileLoadAndSet(SV000_MIKU_BD, 2);

            break;
        }

        if (IsLoadEnd(pzl_load_id) != 0)
        {
            SendManMdlTex();

            ingame_wrk.stts &= ~0x20;

            spev_wrk.ret = 1;

            pzl_load_id = -1;

            SetBlackIn();
        }
    break;
    }

    evbld_dat.t_counter++;

    if (spev_wrk.mode > 1 && spev_wrk.mode != 16)
    {
        SpecialEventDisp004();
    }
}

u_char BldAlpRetern(short int time_cnt, short int bld_appear, short int bld_end, short int bld_alp)
{
    int tmp_alp;

    tmp_alp = 0;

    if (bld_appear < time_cnt)
    {
        if (time_cnt < bld_end && bld_end != 0)
        {
            tmp_alp = ((bld_alp * 4096) / (bld_end - bld_appear)) * (time_cnt - bld_appear);
            tmp_alp = tmp_alp / 4096;
        }
        else
        {
            tmp_alp = bld_alp;
        }
    }

    return tmp_alp;
}

void SpecialEventDisp004()
{
    int i;
    int msg_no;
    static u_char blink_flg = 0;
    SPRT_SDAT ssd;
    SPRT_SDAT ssd2;
    SPRT_SDAT ssd3;
    SPRT_SDAT ssd4;
    DISP_SQAR dsq;
    DISP_STR ds;
    SQAR_DAT menu_sq0 = {
        .w = 418,
        .h = 96,
        .x = 7,
        .y = 333,
        .pri = 0x5000,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x3c,
    };

    SetSprFile(EVENT_ADDRESS);

    for (i = 0; i < 3; i++)
    {
        SimpleDispSprt(&spev04_sp_bk0[i], EVENT_ADDRESS, i, NULL, NULL, 0x64);
    }

    for (i = 0; i < 3; i++)
    {
        SimpleDispSprt(&spev04_sp_bk1[i], EVENT_ADDRESS, 3, NULL, NULL, 0x64);
    }

    for (i = 0; i < 3; i++)
    {
        SimpleDispSprt(&spev04_sp_bk2[i], EVENT_ADDRESS, 4, NULL, NULL, 0x64);
    }

    for (i = 0; i < 3; i++)
    {
        SimpleDispSprt(&spev04_sp_bk3[i], EVENT_ADDRESS, i + 5, NULL, NULL, 0x64);
    }

    for (i = 0; i < 5; i++)
    {
        if (evbtz_dat[i].use_flg == 0)
        {
            SimpleDispSprt(&spev04_sp_bd1[i], EVENT_ADDRESS, 8, NULL, &spev04_bd1_scl[i], 0x64);
        }
        else
        {
            SimpleDispSprtDatCopy(&spev04_sp_bd1[i], &ssd);

            ssd.x = spev04_bz2_scl[evbtz_dat[i].set_place].cx - 46;
            ssd.y = spev04_bz2_scl[evbtz_dat[i].set_place].cy - 302;

            if (evbtz_dat[i].set_place % 3 == 0)
            {
                ssd.pri = 35;
            }
            else if ((evbtz_dat[i].set_place + 2) % 3 == 0)
            {
                ssd.pri = 65;
            }
            else
            {
                ssd.pri = 95;
            }

            SimpleDispSprt(&ssd, EVENT_ADDRESS, 8, NULL, &spev04_bz2_scl[evbtz_dat[i].set_place], 0x64);
        }
    }

    SimpleDispSprtDatCopy(&spev04_sp_bld[6], &ssd);
    ssd.alp = BldAlpRetern(evbld_dat.t_counter, evbld_dat.bld_apr[6], evbld_dat.bld_end[6], spev04_sp_bld[6].alp);
    SimpleDispSprt(&ssd, EVENT_ADDRESS, 8, NULL, NULL, 0x64);

    SimpleDispSprtDatCopy(&spev04_sp_bld[5], &ssd);
    ssd.alp = BldAlpRetern(evbld_dat.t_counter, evbld_dat.bld_apr[5], evbld_dat.bld_end[5], spev04_sp_bld[5].alp);
    SimpleDispSprt(&ssd, EVENT_ADDRESS, 8, &spev03_dol_left, NULL, 0x64);

    SimpleDispSprtDatCopy(&spev04_sp_bld[4], &ssd);
    ssd.alp = BldAlpRetern(evbld_dat.t_counter, evbld_dat.bld_apr[4], evbld_dat.bld_end[4], spev04_sp_bld[4].alp);
    SimpleDispSprt(&ssd, EVENT_ADDRESS, 8, NULL, NULL, 0x64);

    SimpleDispSprtDatCopy(&spev04_sp_bld[3], &ssd);
    ssd.alp = BldAlpRetern(evbld_dat.t_counter, evbld_dat.bld_apr[3], evbld_dat.bld_end[3], spev04_sp_bld[3].alp);
    SimpleDispSprt(&ssd, EVENT_ADDRESS, 8, &spev03_dol_left, NULL, 0x64);

    SimpleDispSprtDatCopy(&spev04_sp_bld[2], &ssd);
    ssd.alp = BldAlpRetern(evbld_dat.t_counter, evbld_dat.bld_apr[2], evbld_dat.bld_end[2], spev04_sp_bld[2].alp);
    SimpleDispSprt(&ssd, EVENT_ADDRESS, 8, &spev03_dol_left, NULL, 0x64);

    SimpleDispSprtDatCopy(&spev04_sp_bld[1], &ssd);
    ssd.alp = BldAlpRetern(evbld_dat.t_counter, evbld_dat.bld_apr[1], evbld_dat.bld_end[1], spev04_sp_bld[1].alp);
    SimpleDispSprt(&ssd, EVENT_ADDRESS, 8, NULL, NULL, 0x64);

    SimpleDispSprtDatCopy(&spev04_sp_bld[0], &ssd);
    ssd.alp = BldAlpRetern(evbld_dat.t_counter, evbld_dat.bld_apr[0], evbld_dat.bld_end[0], spev04_sp_bld[0].alp);
    SimpleDispSprt(&ssd, EVENT_ADDRESS, 8, NULL, NULL, 0x64);

    SimpleDispSprtDatCopy(&spev04_sp_bld[7], &ssd);
    ssd.alp = BldAlpRetern(evbld_dat.t_counter, evbld_dat.bld_apr[7], evbld_dat.bld_end[7], spev04_sp_bld[7].alp);
    SimpleDispSprt(&ssd, EVENT_ADDRESS, 8, NULL, NULL, 0x64);

    SimpleDispSprtDatCopy(&spev04_sp_bld[8], &ssd);
    ssd.alp = BldAlpRetern(evbld_dat.t_counter, evbld_dat.bld_apr[8], evbld_dat.bld_end[8], spev04_sp_bld[8].alp);
    SimpleDispSprt(&ssd, EVENT_ADDRESS, 8, NULL, NULL, 0x64);

    SimpleDispSprtDatCopy(&spev04_sp_bld[9], &ssd);
    ssd.alp = BldAlpRetern(evbld_dat.t_counter, evbld_dat.bld_apr[9], evbld_dat.bld_end[9], spev04_sp_bld[9].alp);
    SimpleDispSprt(&ssd, EVENT_ADDRESS, 8, NULL, NULL, 0x64);

    if (spev_wrk.mode == 5)
    {
        CsrBlink(&spev_wrk.csr[2], 0x80, 0, 3, &blink_flg);

        SimpleDispSprtDatCopy(&spev04_sp_sel[spev_wrk.csr[1]+0], &ssd);
        SimpleDispSprtDatCopy(&spev04_sp_sel[spev_wrk.csr[1]+1], &ssd2);
        SimpleDispSprtDatCopy(&spev04_sp_slf[spev_wrk.csr[1]+0], &ssd3);

        ssd3.alp = spev_wrk.csr[2];

        SimpleDispSprtDatCopy(&spev04_sp_slf[spev_wrk.csr[1]+1], &ssd4);

        ssd4.alp = spev_wrk.csr[2];

        if (spev_wrk.csr[1] < 12)
        {
            SimpleDispSprt(&ssd3, EVENT_ADDRESS, 8, NULL, &spev04_sel_scl2[spev_wrk.csr[1] >> 1], 0x64);
            SimpleDispSprt(&ssd4, EVENT_ADDRESS, 8, NULL, &spev04_sel_scl2[spev_wrk.csr[1] >> 1], 0x64);
        }
        else
        {
            SimpleDispSprt(&ssd3, EVENT_ADDRESS, 8, &spev03_dol_left, &spev04_sel_scl2[spev_wrk.csr[1] >> 1], 0x64);
            SimpleDispSprt(&ssd4, EVENT_ADDRESS, 8, &spev03_dol_left, &spev04_sel_scl2[spev_wrk.csr[1] >> 1], 0x64);
        }
    }

    if (evbtz_dat[spev_wrk.csr[0]].use_flg != 1)
    {
        SimpleDispSprtDatCopy(&spev04_sp_bzf[spev_wrk.csr[0]], &ssd);

        if (spev_wrk.mode == 3)
        {
            ssd.alp = 0;
        }
        else if (spev_wrk.mode == 4)
        {
            ssd.alp = ((20 - spev_wrk.time) * ssd.alp) / 20;
        }
        else if (spev_wrk.mode != 5)
        {
            ssd.alp = 0;
        }

        SimpleDispSprt(&ssd, EVENT_ADDRESS, 8, NULL, NULL, 0x64);
    }

    SimpleDispSprtDatCopy(&spev04_sp_bd1[spev_wrk.csr[0]], &ssd);

    ssd.x = 464;
    ssd.y = 28;
    ssd.pri = 20;

    SimpleDispSprt(&ssd, EVENT_ADDRESS, 8, NULL, NULL, 0x64);

    for (i = 0; i < 3; i++)
    {
        SimpleDispSprt(&spev04_sp_bd2[i], EVENT_ADDRESS, 9, NULL, NULL, 0x64);
    }

    SimpleDispSprtDatCopy(&spev04_sp_cpt[0], &ssd);
#ifdef BUILD_EU_VERSION
#else

    ssd.u += 12;
    ssd.w -= 24;
    ssd.x += 12;

#endif
    SimpleDispSprt(&ssd, EVENT_ADDRESS, 10, NULL, NULL, 0x64);

    SimpleDispSprtDatCopy(&spev04_sp_cap[0], &ssd);
#ifdef BUILD_EU_VERSION
#else

    ssd.x = 470;
    ssd.y = 343;

#endif
    SimpleDispSprt(&ssd, EVENT_ADDRESS, 11, NULL, NULL, 0x64);

    if (spev_wrk.mode == 5)
    {
#ifdef BUILD_EU_VERSION
        SimpleDispSprtDatCopy(&spev04_sp_cap[1], &ssd);
#else
        SimpleDispSprtDatCopy(&spev04_sp_cap[0], &ssd);

        ssd.v += 27;
        ssd.w += 80;
        ssd.x = 470;
        ssd.y = 368;

#endif
        SimpleDispSprt(&ssd, EVENT_ADDRESS, 11, NULL, NULL, 0x64);
    }
    else
    {
#ifdef BUILD_EU_VERSION
        SimpleDispSprtDatCopy(&spev04_sp_cap[4], &ssd);
#else
        SimpleDispSprtDatCopy(&spev04_sp_cap[0], &ssd);

        ssd.v += 100;
        ssd.w += 80;
        ssd.x = 470;
        ssd.y = 363;

#endif
        SimpleDispSprt(&ssd, EVENT_ADDRESS, 11, NULL, NULL, 0x64);
    }

    if (order_no == 0)
    {
#ifdef BUILD_EU_VERSION
        SimpleDispSprtDatCopy(&spev04_sp_cap[2], &ssd);
#else
        SimpleDispSprtDatCopy(&spev04_sp_cap[0], &ssd);

        ssd.v += 54;
        ssd.w += 80;
        ssd.x = 470;
        ssd.y = 393;

#endif
        SimpleDispSprt(&ssd, EVENT_ADDRESS, 11, NULL, NULL, 0x64);
    }
    else
    {
#ifdef BUILD_EU_VERSION
        SimpleDispSprtDatCopy(&spev04_sp_cap[3], &ssd);
#else
        SimpleDispSprtDatCopy(&spev04_sp_cap[0], &ssd);

        ssd.v += 80;
        ssd.w += 80;
        ssd.x = 470;
        ssd.y = 395;

#endif
        SimpleDispSprt(&ssd, EVENT_ADDRESS, 11, NULL, NULL, 0x64);
    }

    DrawButtonTex(0xa000, 3, spev04_cbt_pos[0][0], spev04_cbt_pos[0][1], 0x64);
    DrawButtonTex(0xa000, 1, spev04_cbt_pos[1][0], spev04_cbt_pos[1][1], 0x64);

    CopySqrDToSqr(&dsq, &menu_sq0);
    DispSqrD(&dsq);

    DrawMessageBox(0x2000, 7.0f, 333.0f, 428.0f, 96.0f, 0x7f);

    if (spev_wrk.mode == 4)
    {
        if (spev_wrk.csr[0] == 0)
        {
            msg_no = 55;
        }
        else if (spev_wrk.csr[0] == 1)
        {
            msg_no = 57;
        }
        else if (spev_wrk.csr[0] == 2)
        {
            msg_no = 58;
        }
        else if (spev_wrk.csr[0] == 3)
        {
            msg_no = 56;
        }
        else
        {
            msg_no = 54;
        }

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, msg_no);
        spev_str.pos_x = 30;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }
    else if (spev_wrk.mode == 5)
    {
        if (spev_wrk.csr[0] == 0)
        {
            msg_no = 55;
        }
        else if (spev_wrk.csr[0] == 1)
        {
            msg_no = 57;
        }
        else if (spev_wrk.csr[0] == 2)
        {
            msg_no = 58;
        }
        else if (spev_wrk.csr[0] == 3)
        {
            msg_no = 56;
        }
        else
        {
            msg_no = 54;
        }

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, msg_no);
        spev_str.pos_x = 30;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }
    else if (spev_wrk.mode == 6 || spev_wrk.mode== 7)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV04_MSG05);
        spev_str.pos_x = 100;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_YES);
        spev_str.pos_x = 130;
        spev_str.pos_y = 370;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_NO);
        spev_str.pos_x = 220;
        spev_str.pos_y = 370;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (spev_wrk.mode == 6)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_CHECK);
            spev_str.pos_x = 110;
            spev_str.pos_y = 370;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_CHECK);
            spev_str.pos_x = 200;
            spev_str.pos_y = 370;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
    }
    else if (spev_wrk.mode == 9)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV04_MSG02);
        spev_str.pos_x = 30;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }
    else if (spev_wrk.mode == 12)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV04_MSG00);
        spev_str.pos_x = 30;
        spev_str.pos_y = 347;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }
    else if (spev_wrk.mode == 13)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV04_MSG01);
        spev_str.pos_x = 30;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }
}

void ButsuzoMSGInit()
{
#ifdef BUILD_EU_VERSION
    LoadReqLanguage(EVTEX04_E_PK2, EVENT_ADDRESS);
#else
    LoadReq(EVTEX04_PK2, EVENT_ADDRESS);
#endif

    pzl_load_id = -1;

    spev_wrk.mode = 1;
    spev_wrk.time = 40;

    butsuzo_alpha[1] = 0;

    SetBlackOut();
}

void ButsuzoMSGMain()
{
    switch(spev_wrk.mode)
    {
    case 1:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEndAll() != 0)
            {
                SetSprFile(EVENT_ADDRESS);

                spev_wrk.mode = 2;

                ingame_wrk.stts |= 0x20;

                SetBlackIn();

                pzl_load_id = SeFileLoadAndSet(ST000_ZOU_OKU_BD, 21);
            }
        }
    break;
    case 2:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEnd(pzl_load_id) != 0)
            {
                pzl_load_id = -1;

                spev_wrk.mode = 3;
                spev_wrk.time = 40;

                SeStartFix(93, 0, 0x1000, 0x1000, 0);
            }
        }
    break;
    case 3:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 8;
            spev_wrk.time = 1;
        }

    break;
    case 8:
        if (*key_now[5] == 1 || *key_now[4] == 1)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            spev_wrk.mode = 15;
            spev_wrk.time = 60;

            SetBlackOut();
        }
    break;
    case 15:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 16;
        }
    break;
    case 16:
        if (pzl_load_id == -1)
        {
            pzl_load_id = SeFileLoadAndSet(SV000_MIKU_BD, 2);
        }
        else if (IsLoadEnd(pzl_load_id) != 0)
        {
            ingame_wrk.stts &= ~0x20;

            pzl_load_id = -1;

            event_stts[ev_wrk.evt_no] = 1;

            spev_wrk.ret = 1;

            SetBlackIn();
        }
    break;
    }

    if (spev_wrk.mode == 8)
    {
        if (butsuzo_alpha[1] < 0x80)
        {
            butsuzo_alpha[1] += 6;

            if (butsuzo_alpha[1] >= 0x80)
            {
                 butsuzo_alpha[1] = 0x80;
            }
        }
    }
    else if (butsuzo_alpha[1] > 6)
    {
        butsuzo_alpha[1] -= 6;

        if (butsuzo_alpha[1] <= 6)
        {
            butsuzo_alpha[1] = 0;
        }
    }

    if (spev_wrk.mode != 1)
    {
        ButsuzoMSGDisp();
    }
}

void ButsuzoMSGDisp()
{
    int i;
    static int next_count = 0;
    SPRT_SDAT ssd;
    SPRT_SDAT ssd_csr;
    DISP_STR ds;
    DISP_SQAR dsq;
    SQAR_DAT menu_sq0 = {
        .w = 576,
        .h = 90,
        .x = 27,
        .y = 331,
        .pri = 0x5000,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x3c,
    };

    SetSprFile(EVENT_ADDRESS);

    for (i = 0; i < 3; i++)
    {
        SimpleDispSprt(&spev04_sp_bk0[i], EVENT_ADDRESS, i, NULL, NULL, 0x64);
    }

    for (i = 0; i < 3; i++)
    {
        SimpleDispSprt(&spev04_sp_bk1[i], EVENT_ADDRESS, 3, NULL, NULL, 0x64);
    }

    for (i = 0; i < 3; i++)
    {
        SimpleDispSprt(&spev04_sp_bk2[i], EVENT_ADDRESS, 4, NULL, NULL, 0x64);
    }

    for (i = 0; i < 3; i++)
    {
        SimpleDispSprt(&spev04_sp_bk3[i], EVENT_ADDRESS, i + 5, NULL, NULL, 0x64);
    }

    for (i = 0; i < 3; i++)
    {
        SimpleDispSprt(&spev04_sp_bd2[i], EVENT_ADDRESS, 9, NULL, NULL, 0x64);
    }

    for (i = 0; i < 5; i++)
    {
        SimpleDispSprt(&spev04_sp_bd1[i], EVENT_ADDRESS, 8, NULL, &spev04_bd1_scl[i], 0x64);
    }

    DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, butsuzo_alpha[1]);

    spev_str.str = (u_char*)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV39_MSG00);

    spev_str.pos_x = 50;
    spev_str.pos_y = 350;
    spev_str.alpha = butsuzo_alpha[1];

    CopyStrDToStr(&ds, &spev_str);
    SetMessageV2(&ds);
}

void LightsOutInit()
{
    int i;

    if (plyr_wrk.se_deadly != -1)
    {
        SeFadeFlame(plyr_wrk.se_deadly, 60, 0);

        plyr_wrk.se_deadly = -1;

        spev_wrk.mode = 0;
        spev_wrk.time = 60;
        spev_wrk.count = 0;
    }
    else
    {
#ifdef BUILD_EU_VERSION
        LoadReqLanguage(EVTEX20_E_PK2, EVENT_ADDRESS);
#else
        LoadReq(EVTEX20_PK2, EVENT_ADDRESS);
#endif

        spev_wrk.mode = 1;
        spev_wrk.time = 40;
        spev_wrk.count = 0;
    }

    CsrClearAll();

    for (i = 0; i < 6; i++)
    {
        evcdl_dat.flame_time[i] = 0;
        evcdl_dat.cdl_flg[0][i] = evcdl_dat.cdl_flg[1][i];
        evcdl_dat.flame_alpha[i] = 0;
        evcdl_dat.stflame_alpha[i] = 0;
        evcdl_dat.stflame_time[i] = 0;
        evcdl_dat.stflame_shape[i] = 0;
    }

    SetBlackOut();
}

void LightsOutMain()
{
    int no;
    int i;

    no = spev_wrk.count;

    switch(spev_wrk.mode)
    {
    case 0:
        if (spev_wrk.time == 0)
        {
#ifdef BUILD_EU_VERSION
        LoadReqLanguage(EVTEX20_E_PK2, EVENT_ADDRESS);
#else
            LoadReq(EVTEX20_PK2, EVENT_ADDRESS);
#endif

            pzl_load_id = -1;

            spev_wrk.mode = 1;
            spev_wrk.time = 40;
        }
        else
        {
            spev_wrk.time--;
        }
    break;
    case 1:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEndAll() != 0)
            {
                SetSprFile(EVENT_ADDRESS);

                spev_wrk.mode = 2;

                ingame_wrk.stts |= 0x20;

                pzl_load_id = SeFileLoadAndSet(SP004_ROUSOKU_BD, 2);
            }
        }
    break;
    case 2:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEnd(pzl_load_id) != 0)
            {
                pzl_load_id = -1;

                spev_wrk.mode = 3;
                spev_wrk.time = 30;

                SetBlackIn();
            }
        }
    break;
    case 3:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (CsrInclease(&spev_wrk.csr[1], 0x54, 4) != 0)
            {
                spev_wrk.mode = 4;
                spev_wrk.time = 40;
            }
        }
    break;
    case 4:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }

        CsrInclease(&spev_wrk.csr[1], 0x54, 4);

        if (*key_now[4] == 1)
        {
            spev_wrk.mode = 12;
            spev_wrk.time = 40;

            SetBlackOut();
        }
        else if (*key_now[7] == 1)
        {
            for (i = 0; i < 6; i++)
            {
                evcdl_dat.cdl_flg[0][i] = evcdl_dat.cdl_flg[1][i];
            }

            spev_wrk.mode = 7;
            spev_wrk.time = 20;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (*key_now[5] == 1)
        {
            if (evcdl_dat.cdl_flg[0][spev_wrk.csr[0]] == 0)
            {
                evcdl_dat.cdl_flg[0][spev_wrk.csr[0]] = 1;

                spev_wrk.mode = 5;
                spev_wrk.time = 30;

                SeStartFix(33, 0, 0x1000, 0x1000, 0);
            }
        }
        else if (
            *key_now[3] == 1 ||
            (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
            Ana2PadDirCnt(1) == 1 ||
            (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
        )
        {
            spev_wrk.csr[0]++;

            if (spev_wrk.csr[0] == 6)
            {
                spev_wrk.csr[0] = 0;
            }

            CsrClear(&spev_wrk.csr[1]);
            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
        else if (
            *key_now[2] == 1 ||
            (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
            Ana2PadDirCnt(3) == 1 ||
            (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
        )
        {
            if (spev_wrk.csr[0] == 0)
            {
                spev_wrk.csr[0] = 5;
            }
            else
            {
                spev_wrk.csr[0]--;
            }

            CsrClear(&spev_wrk.csr[1]);
            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
    break;
    case 5:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;

            CsrInclease(&spev_wrk.csr[1], 84, 4);
        }
        else
        {
            if (spev_wrk.csr[0] == 0)
            {
                if (evcdl_dat.cdl_flg[0][spev_wrk.csr[0]+1] != 0)
                {
                    evcdl_dat.cdl_flg[0][spev_wrk.csr[0]+1] = 0;
                }
                else
                {
                    evcdl_dat.cdl_flg[0][spev_wrk.csr[0]+1] = 1;

                    SeStartFix(33, 0, 0x1000, 0x1000, 0);
                }
            }
            else if (spev_wrk.csr[0] == 5)
            {
                if (evcdl_dat.cdl_flg[0][spev_wrk.csr[0]-1] != 0)
                {
                    evcdl_dat.cdl_flg[0][spev_wrk.csr[0]-1] = 0;
                }
                else
                {
                    evcdl_dat.cdl_flg[0][spev_wrk.csr[0]-1] = 1;

                    SeStartFix(33, 0, 0x1000, 0x1000, 0);
                }
            }
            else
            {
                if (evcdl_dat.cdl_flg[0][spev_wrk.csr[0]+1] != 0)
                {
                    evcdl_dat.cdl_flg[0][spev_wrk.csr[0]+1] = 0;
                }
                else
                {
                    evcdl_dat.cdl_flg[0][spev_wrk.csr[0]+1] = 1;

                    SeStartFix(33, 0, 0x1000, 0x1000, 0);
                }

                if (evcdl_dat.cdl_flg[0][spev_wrk.csr[0]-1] != 0)
                {
                    evcdl_dat.cdl_flg[0][spev_wrk.csr[0]-1] = 0;
                }
                else
                {
                    evcdl_dat.cdl_flg[0][spev_wrk.csr[0]-1] = 1;

                    SeStartFix(33, 0, 0x1000, 0x1000, 0);
                }
            }

            spev_wrk.mode = 6;
            spev_wrk.time = 20;
        }
    break;
    case 6:
        if (spev_wrk.time != 0)
        {
            CsrInclease(&spev_wrk.csr[1], 84, 4);

            spev_wrk.time--;
        }
        else
        {
            for (i = 0; i < 6; i++)
            {
                if (evcdl_dat.cdl_flg[0][i] != evcdl_dat.cdl_flg[2][i])
                {
                    break;
                }
            }

            if (i != 6)
            {
                spev_wrk.mode = 4;
                spev_wrk.time = 40;
            }
            else
            {
                spev_wrk.mode = 8;
                spev_wrk.time = 8;
            }
        }
    break;
    case 7:
        if (spev_wrk.time == 0)
        {
            spev_wrk.mode = 4;
            spev_wrk.time = 40;
        }
        else
        {
            spev_wrk.time--;
        }
    break;
    case 8:
        if (spev_wrk.time == 0)
        {
            spev_wrk.mode = 9;
            spev_wrk.time = 60;

            SeStartFix(34, 0, 0x1000, 0x1000, 0);
        }
        else
        {
            spev_wrk.time--;
        }
    break;
    case 9:
        if (spev_wrk.time == 0)
        {
            spev_wrk.mode = 10;
            spev_wrk.time = 0xff;

            pzl_load_id = SeStartFix(35, 0, 0x1000, 0x1000, 0);
        }
        else
        {
            spev_wrk.time--;
        }
    break;
    case 10:
        if (ButtonMarkNext(0, 0, 0) != 0)
        {
            if (spev_wrk.time != 0)
            {
                spev_wrk.time--;
            }

            if (IsEndSe(pzl_load_id) || spev_wrk.time == 0)
            {
                pzl_load_id = -1;

                ButtonMarkTimeClear();

                event_stts[ev_wrk.evt_no] = 1;

                spev_wrk.mode = 11;
                spev_wrk.time = 40;

                SetBlackOut();
            }
        }
    break;
    case 11:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            ReqDramaCamera(1, 13, 0);

            spev_wrk.mode = 13;
        }
    break;
    case 12:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 13;
        }
    break;
    case 13:
        if (pzl_load_id == -1)
        {
            pzl_load_id = SeFileLoadAndSet(SV000_MIKU_BD, 2);
        }
        else if (IsLoadEnd(pzl_load_id) != 0)
        {
            SendManMdlTex();

            spev_wrk.ret = 1;

            ingame_wrk.stts &= ~0x20;

            SetBlackIn();
        }
    break;
    }

    if (spev_wrk.mode != 0 && spev_wrk.mode != 1 && spev_wrk.mode != 13)
    {
        SpecialEventDisp014(no);
    }
}

void SpecialEventDisp014(int no)
{
    int i;
    static int next_count = 0;
    SPRT_SDAT ssd;
    SPRT_SDAT ssd_csr;
    DISP_STR ds;
    DISP_SQAR dsq;
    SQAR_DAT menu_sq0 = {
        .w = 576,
        .h = 90,
        .x = 27,
        .y = 331,
        .pri = 0x5000,
        .r = 0x00,
        .g = 0x00,
        .b = 0x00,
        .alpha = 0x80,
    };

    SetSprFile(EVENT_ADDRESS);

    for (i = 0; i < 6; i++)
    {
        if ((rand() % 10) != 6)
        {
            evcdl_dat.flame_time[i]++;
        }

        if (evcdl_dat.flame_time[i] == 5)
        {
            evcdl_dat.flame_time[i] = 0;
            evcdl_dat.flame_shape[i]++;

            if (evcdl_dat.flame_shape[i] > 6)
            {
                evcdl_dat.flame_shape[i] = 0;
            }
        }
    }

    for (i = 0; i < 6; i++)
    {
        if (evcdl_dat.cdl_flg[0][i] > 0 || spev_wrk.mode == 8 || spev_wrk.mode == 9 || spev_wrk.mode == 10 || spev_wrk.mode == 11)
        {
            if (evcdl_dat.flame_alpha[i] + 10 < 129)
            {
                evcdl_dat.flame_alpha[i] += 10;
            }
            else
            {
                evcdl_dat.flame_alpha[i] = 128;
            }
        }
        else if (evcdl_dat.cdl_flg[0][i] == 0)
        {
            if (evcdl_dat.flame_alpha[i] - 10 >= 0)
            {
                evcdl_dat.flame_alpha[i] -= 10;
            }
            else
            {
                evcdl_dat.flame_alpha[i] = 0;
            }
        }
    }

    if ((spev_wrk.mode == 9 || spev_wrk.mode == 10 || spev_wrk.mode == 11) && evcdl_dat.stflame_shape[0] != -1)
    {
        for (i = 0; i < 6; i++)
        {
            evcdl_dat.stflame_time[i]++;

            if (evcdl_dat.stflame_alpha[i] + 10 <= 0x80)
            {
                evcdl_dat.stflame_alpha[i] += 10;
            }
            else
            {
                evcdl_dat.stflame_alpha[i] = 0x80;
            }

            if (evcdl_dat.stflame_time[i] == 5)
            {
                evcdl_dat.stflame_time[i] = 0;
                evcdl_dat.stflame_shape[i]++;

                if (evcdl_dat.stflame_shape[i] > 14)
                {
                    evcdl_dat.stflame_shape[0] = -1;

                    break;
                }
            }
        }
    }
    else
    {
        for (i = 0; i < 6; i++)
        {
            if (evcdl_dat.stflame_alpha[i] - 10 > 0)
            {
                evcdl_dat.stflame_alpha[i] -= 10;
            }
            else
            {
                evcdl_dat.stflame_alpha[i] = 0;
            }
        }
    }

    for (i = 0; i < 11; i++)
    {
        SimpleDispSprt(&spev20_sp_bak[i], EVENT_ADDRESS, i, NULL, NULL, 0x64);
    }

    for (i = 0; i < 6; i++)
    {
        SimpleDispSprtDatCopy(&spev20_sp_cla[i], &ssd);

        ssd.alp = evcdl_dat.flame_alpha[i];

        SimpleDispSprt(&ssd, EVENT_ADDRESS, 11, NULL, NULL, 0x64);
    }

    for (i = 0; i < 6; i++)
    {
        if (spev_wrk.csr[0] == i)
        {
            SimpleDispSprtDatCopy(&spev20_sp_cfa[i], &ssd);

            ssd.alp = spev_wrk.csr[1];

            SimpleDispSprt(&ssd, EVENT_ADDRESS, 12, NULL, NULL, 0x64);
        }
    }

    for (i = 0; i < 6; i++)
    {
        SimpleDispSprtDatCopy(&spev20_sp_cdl[i], &ssd);
        SimpleDispSprt(&ssd, EVENT_ADDRESS, 11, NULL, NULL, 0x64);
    }

    if (evcdl_dat.stflame_shape[0] != -1)
    {
        for (i = 0; i < 6; i++)
        {
            SimpleDispSprtDatCopy(&spev20_sp_cdl4[evcdl_dat.stflame_shape[i]], &ssd);

            ssd.x = spev20_sp_sflm[i][0];
            ssd.y = spev20_sp_sflm[i][1];

            ssd.alp = evcdl_dat.stflame_alpha[i];

            SimpleDispSprt(&ssd, EVENT_ADDRESS, 14, NULL, NULL, 0x64);
        }
    }

    for (i = 0; i < 6; i++)
    {
        SimpleDispSprtDatCopy(&spev20_sp_cdl3[evcdl_dat.flame_shape[i]], &ssd);

        ssd.x = spev20_sp_flm[i][0];
        ssd.y = spev20_sp_flm[i][1];

        ssd.alp = evcdl_dat.flame_alpha[i];

        SimpleDispSprt(&ssd, EVENT_ADDRESS, 13, NULL, NULL, 0x64);
    }

    if (spev_wrk.mode == 10)
    {
        CopySqrDToSqr(&dsq, &menu_sq0);

        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV20_MSG00);

        spev_str.pos_x = 50;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);

        SetMessageV2(&ds);
    }

    SimpleDispSprt(&spev20_sp_cpf[0], EVENT_ADDRESS, 11, NULL, NULL, 0x64);
    SimpleDispSprt(&spev20_sp_cdl5[0], EVENT_ADDRESS, 15, NULL, NULL, 0x64);

    DrawButtonTex(0xa000, 3, spev20_cbt_pos[0][0], spev20_cbt_pos[0][1], 0x64);
    DrawButtonTex(0xa000, 2, spev20_cbt_pos[1][0], spev20_cbt_pos[1][1], 0x64);
    DrawButtonTex(0xa000, 1, spev20_cbt_pos[2][0], spev20_cbt_pos[2][1], 0x64);
}

short int yoro_door = 26;
short int ika_door = 17;
short int kana_door = 34;
short int tano_door = 19;
static u_char open_close = 0;
static short int go_flg = 0;
static short int go_flg_s = 0;
static short int choudo_no = 0;
static char alpha_keep = 0;
static char first_meet = 0;

void FaceDoorInit(int face_no)
{
    if ((ev_wrk.face_stts[1] & 0x1) == 0)
    {
        ev_wrk.face_stts[2] |= 0x4 | 0x1;
        ev_wrk.face_stts[1] &= 0x80 | 0x40 | 0x20 | 0x1;
        ev_wrk.face_stts[1] |= 0x1;

        door_keep[47].attr |= 0x2;
        door_keep[26].attr &= ~0x2;
        door_keep[34].attr |= 0x2;
        door_keep[49].attr &= ~0x2;
    }

    SpevStrInit();
    SpevWrkInit();

    go_flg = 0;

    if (face_no == 0 && plyr_wrk.pr_info.room_no != door_keep[47].room_id)
    {
        spev_wrk.mode = 4;
        spev_wrk.time = 40;
    }
    else if (face_no == 1 && plyr_wrk.pr_info.room_no != door_keep[26].room_id)
    {
        spev_wrk.mode = 4;
        spev_wrk.time = 40;
    }
    else if (face_no == 2 && plyr_wrk.pr_info.room_no != door_keep[34].room_id)
    {
        spev_wrk.mode = 4;
        spev_wrk.time = 40;
    }
    else if (face_no == 3 && plyr_wrk.pr_info.room_no != door_keep[49].room_id)
    {
        spev_wrk.mode = 4;
        spev_wrk.time = 40;
    }
    else
    {
        spev_wrk.mode = 0;
        spev_wrk.time = 40;

        SetBlackOut();
    }
}

void FaceDoorMain(int face_no)
{
    short int omen_no;
    short int have_omen;
    short int door_avail;

    omen_no = 0;

    door_avail = 0;

    if (face_no == 0)
    {
        omen_no = 41;

        if (door_keep[47].attr & 2)
        {
            door_avail = 1;
            spev_wrk.csr[1] = 0x80;
        }
    }
    else if (face_no == 1)
    {
        omen_no = 42;

        if (door_keep[26].attr & 2) {
            door_avail = 1;
            spev_wrk.csr[1] = 0x80;
        }
    }
    else if (face_no == 2)
    {
        omen_no = 43;

        if (door_keep[34].attr & 2)
        {
            door_avail = 1;
            spev_wrk.csr[1] = 0x80;
        }
    }
    else if (face_no == 3)
    {
        omen_no = 44;

        if (door_keep[49].attr & 2)
        {
            door_avail = 1;
            spev_wrk.csr[1] = 0x80;
        }
    }

    have_omen = poss_item[omen_no] != 0;

    switch(spev_wrk.mode)
    {
    case 4:
        SetMessageAddr(GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG12));

        if (SetMessage() == 0)
        {
            spev_wrk.ret = 1;
        }
    break;
    case 0:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (face_no == 0)
            {
                LoadReq(TX_PZL_BG_KI_PK2, EVENT_ADDRESS);
            }
            else if (face_no == 1)
            {
                LoadReq(TX_PZL_BG_DO_PK2, EVENT_ADDRESS);
            }
            else if (face_no == 2)
            {
                LoadReq(TX_PZL_BG_AI_PK2, EVENT_ADDRESS);
            }
            else if (face_no == 3)
            {
                LoadReq(TX_PZL_BG_RK_PK2, EVENT_ADDRESS);
            }

            LoadReq(TX_PZL_MENZ_PK2, EVENT_ADDRESS_2);

            spev_wrk.mode = 1;
        }
    break;
    case 1:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEndAll() != 0)
            {
                SetSprFile(EVENT_ADDRESS);

                spev_wrk.mode = 2;

                ingame_wrk.stts |= 0x20;

                pzl_load_id = SeFileLoadAndSet(ST001_MEN_KAKE_BD, 21);
            }
        }
    break;
    case 2:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEnd(pzl_load_id) != 0)
            {
                pzl_load_id = -1;

                SetBlackIn();

                spev_wrk.mode = 3;
                spev_wrk.time = 30;
            }
        }
    break;
    case 3:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (door_avail == 1)
            {
                spev_wrk.mode = 5;
                spev_wrk.csr[1] = 0x80;
                spev_wrk.time = 1;
            }
            else if (have_omen == 1)
            {
                spev_wrk.mode = 18;
                spev_wrk.time = 1;
            }
            else if (poss_item[46] != 0)
            {
                spev_wrk.mode = 18;
                spev_wrk.time = 1;
            }
            else
            {
                spev_wrk.mode = 17;
                spev_wrk.time = 1;
            }
        }
    break;
    case 5:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            ButtonMarkTimeClear();

            spev_wrk.mode = 7;
            spev_wrk.time = 1;
        }
    break;
    case 7:
    case 8:
    case 9:
        spev_wrk.csr[1] = 0x80;

        if (ButtonMarkWait() != 0)
        {
            break;
        }

        if (*key_now[5] == 1)
        {
            spev_wrk.time = 0;
        }
        else if (*key_now[4] == 1)
        {
            spev_wrk.mode = 26;
            spev_wrk.time = 40;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
            SetBlackOut();
        }
        else if (
            (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            ) || (
                *key_now[1] == 1 ||
                (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
                Ana2PadDirCnt(2) == 1 ||
                (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
            )
        )
        {
            if (spev_wrk.mode == 7)
            {
                spev_wrk.mode = 8;
            }
            else if (spev_wrk.mode == 8)
            {
                spev_wrk.mode = 9;
            }
            else
            {
                spev_wrk.mode = 7;
            }

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
        else if (
            (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            ) || (
                *key_now[0] == 1 ||
                (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
                Ana2PadDirCnt(0) == 1 ||
                (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
            )
        )
        {
            if (spev_wrk.mode == 7)
            {
                spev_wrk.mode = 9;
            }
            else if (spev_wrk.mode == 8)
            {
                spev_wrk.mode = 7;
            }
            else
            {
                spev_wrk.mode = 8;
            }

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }

        if (spev_wrk.time == 0)
        {
            if (spev_wrk.mode == 7)
            {
                spev_wrk.mode = 26;
                spev_wrk.time = 40;

                go_flg = 1;

                SeStartFix(1, 0, 0x1000, 0x1000, 0);
                SetBlackOut();
            }
            else if (spev_wrk.mode == 8)
            {
                if (NisUseCheck(face_no) != 0)
                {
                    poss_item[46] = 1;

                    NisUseUnSet(face_no);

                    spev_wrk.csr[2] = 1;
                }
                else
                {
                    poss_item[omen_no] = 1;
                }

                FaceDoorOkSet(face_no);
                SeStartFix(1, 0, 0x1000, 0x1000, 0);

                spev_wrk.mode = 27;
                spev_wrk.time = 40;

                SetBlackOut();
            }
            else
            {
                spev_wrk.mode = 26;
                spev_wrk.time = 40;

                SeStartFix(3, 0, 0x1000, 0x1000, 0);
                SetBlackOut();
            }
        }
    break;
    case '\n':
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 26;
            spev_wrk.time = 40;

            SetBlackOut();
        }
    break;
    case '\v':
    case '\f':
        if (*key_now[5] == 1)
        {
            spev_wrk.time = 0;
        }
        else if (*key_now[4] == 1)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            if (have_omen == 1)
            {
                spev_wrk.mode = 19;
                spev_wrk.time = 1;
            }
            else
            {
                spev_wrk.mode = 17;
                spev_wrk.time = 1;
            }

            break;
        }
        else if (
            (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            ) || (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            )
        )
        {
            spev_wrk.mode = spev_wrk.mode == 11 ? 12 : 11;
            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }

        if (spev_wrk.time == 0)
        {
            if (spev_wrk.mode == 11)
            {
                spev_wrk.mode = 14;
                spev_wrk.time = 30;
                spev_wrk.csr[0] = 0x80;
                spev_wrk.csr[1] = 0;

                poss_item[46] = 0;

                NisUseSet(face_no);
                SeStartFix(93, 0, 0x1000, 0x1000, 0);
            }
            else if (have_omen == 1)
            {
                spev_wrk.mode = 19;
                spev_wrk.time = 1;

                SeStartFix(3, 0, 0x1000, 0x1000, 0);
            }
            else
            {
                spev_wrk.mode = 17;
                spev_wrk.time = 1;

                SeStartFix(3, 0, 0x1000, 0x1000, 0);
            }
        }
    break;
    case 13:
        if (*key_now[5] == 1 || *key_now[4] == 1)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            if (have_omen == 1)
            {
                spev_wrk.mode = 19;
                spev_wrk.time = 1;
            }
            else
            {
                spev_wrk.mode = 17;
                spev_wrk.time = 1;
            }
        }
    break;
    case 14:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 15;
            spev_wrk.time = 140;

            pzl_load_id = SeStartFixFlame(10, 60, 0xc00, 0xb60, 0);
        }
    break;
    case 15:
        if (spev_wrk.csr[0] != 0)
        {
            spev_wrk.csr[0]--;
            spev_wrk.csr[1]++;
        }

        if (*key_now[5] == 1 || *key_now[4] == 1)
        {
            spev_wrk.time = 0;
            spev_wrk.csr[0] = 0;
            spev_wrk.csr[1] = 0x80;
        }

        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 16;
            spev_wrk.time = 1;

            SeFadeFlame(pzl_load_id, 20, 0);

            pzl_load_id = -1;
        }
    break;
    case 16:
        if (*key_now[5] == 1 || *key_now[4] == 1)
        {
            spev_wrk.time = 0;

            SeStartFix(1, 0, 0x1000, 0x1000, 0);
        }

        if (spev_wrk.time == 0)
        {
            FaceDoorOkSet(face_no);

            spev_wrk.mode = 26;
            spev_wrk.time = 40;

            SetBlackOut();
        }
    break;
    case 17:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            ButtonMarkTimeClear();

            spev_wrk.mode = 26;
            spev_wrk.time = 40;

            SetBlackOut();
        }
    break;
    case 18:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            ButtonMarkTimeClear();

            spev_wrk.mode = 19;

            if (poss_item[46] != 0)
            {
                spev_wrk.mode = 11;
            }

            spev_wrk.time = 1;
        }
    break;
    case 19:
    case 20:
        if (*key_now[5] == 1)
        {
            spev_wrk.time = 0;
        }
        else if (
            (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            ) || (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            )
        )
        {
            spev_wrk.mode = spev_wrk.mode == 19 ? 20 : 19;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
        else if (*key_now[4] == 1)
        {
            spev_wrk.mode = 26;
            spev_wrk.time = 40;

            SetBlackOut();
        }

        if (spev_wrk.time == 0)
        {
            if (spev_wrk.mode != 19)
            {
                spev_wrk.mode = 26;
                spev_wrk.time = 40;
            }
            else
            {
                spev_wrk.mode = 21;
                spev_wrk.time = 40;
            }

            SetBlackOut();
        }
    break;
    case 21:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.csr[1] = 0x80;
            spev_wrk.mode = 22;
            spev_wrk.time = 40;

            SeStartFix(93, 0, 0x1000, 0x1000, 0);
            SetBlackIn();
        }
    break;
    case 22:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            ButtonMarkTimeClear();

            spev_wrk.mode = 26;
            spev_wrk.time = 40;

            poss_item[omen_no] = 0;

            FaceDoorOkSet(face_no);

            go_flg = 1;

            SetBlackOut();
        }
    break;
    case 26:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (go_flg == 1)
            {
                if (face_no == 0 && DoorKeyUnlockOnly(47) != 0)
                {
                    DoorOpenShiftForce(47);
                }
                if (face_no == 1 && DoorKeyUnlockOnly(26) != 0)
                {
                    DoorOpenShiftForce(26);
                }
                if (face_no == 2 && DoorKeyUnlockOnly(34) != 0)
                {
                    DoorOpenShiftForce(34);
                }
                if (face_no == 3 && DoorKeyUnlockOnly(49) != 0)
                {
                    DoorOpenShiftForce(49);
                }

                go_flg = 0;

            }

            spev_wrk.mode = 31;
        }
    break;
    case 27:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            ingame_wrk.stts &= ~0x20;

            spev_wrk.mode = 30;
            spev_wrk.time = 40;
            spev_wrk.csr[1] = 0;

            SetBlackIn();
            SeStartFix(93, 0, 0x1000, 0x1000, 0);
        }
    break;
    case 30:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            ButtonMarkTimeClear();

            spev_wrk.ret = 1;
        }
    break;
    case 31:
        SendManMdlTex();

        spev_wrk.ret = 1;

        ingame_wrk.stts &= ~0x20;

        SetBlackIn();
    break;
    }

    if (spev_wrk.mode != 0 && spev_wrk.mode != 1 && spev_wrk.mode != 4 && spev_wrk.mode != 31)
    {
        FaceDoorDisp(face_no);
    }
}

void FaceDoorOkSet(int face_no)
{
    u_short model_no;

    if (face_no == 0)
    {
        door_keep[47].attr ^= 0x2;

        model_no = door_keep[47].attr & 0x2 ? dmodel_table[0][1] : dmodel_table[0][0];

        DoorChangeModel(47, model_no);
    }
    else if (face_no == 1)
    {
        door_keep[26].attr ^= 0x2;

        model_no = door_keep[26].attr & 0x2 ? dmodel_table[1][1] : dmodel_table[1][0];

        DoorChangeModel(26, model_no);
    }
    else if (face_no == 2)
    {
        door_keep[34].attr ^= 0x2;

        model_no = door_keep[34].attr & 0x2 ? dmodel_table[2][1] : dmodel_table[2][0];

        DoorChangeModel(34, model_no);
    }
    else if (face_no == 3)
    {
        door_keep[49].attr ^= 0x2;

        model_no = door_keep[49].attr & 0x2 ? dmodel_table[3][1] : dmodel_table[3][0];

        DoorChangeModel(49, model_no);
    }
}

void FaceDoorAimSet(int face_no)
{
    if (face_no == 0)
    {
        ev_wrk.face_stts[2] ^= 0x10;
    }
    else if (face_no == 1)
    {
        ev_wrk.face_stts[2] ^= 0x20;
    }
    else if (face_no == 2)
    {
        ev_wrk.face_stts[2] ^= 0x40;
    }
    else if (face_no == 3)
    {
        ev_wrk.face_stts[2] ^= 0x80;
    }
}

int NisUseCheck(int face_no)
{
    int ret_num;

    ret_num = 0;

    if (face_no == 0 && ev_wrk.face_stts[1] & 0x2)
    {
        ret_num = 1;
    }
    else if (face_no == 1 && ev_wrk.face_stts[1] & 0x4)
    {
        ret_num = 1;
    }
    else if (face_no == 2 && ev_wrk.face_stts[1] & 0x8)
    {
        ret_num = 1;
    }
    else if (face_no == 3 && ev_wrk.face_stts[1] & 0x10)
    {
        ret_num = 1;
    }

    return ret_num;
}

void NisUseSet(int face_no)
{
    if (face_no == 0)
    {
        ev_wrk.face_stts[1] |= 0x2;
    }
    else if (face_no == 1)
    {
        ev_wrk.face_stts[1] |= 0x4;
    }
    else if (face_no == 2)
    {
        ev_wrk.face_stts[1] |= 0x8;
    }
    else if (face_no == 3)
    {
        ev_wrk.face_stts[1] |= 0x10;
    }
}

void NisUseUnSet(int face_no)
{
    if (face_no == 0)
    {
        ev_wrk.face_stts[1] &= ~0x2;
    }
    else if (face_no == 1)
    {
        ev_wrk.face_stts[1] &= ~0x4;
    }
    else if (face_no == 2)
    {
        ev_wrk.face_stts[1] &= ~0x8;
    }
    else if (face_no == 3)
    {
        ev_wrk.face_stts[1] &= ~0x10;
    }
}

void FaceDoorDisp(int face_no)
{
#ifdef BUILD_EU_VERSION
    int lng;
    int mx;
    int lng0;
    int lng1;
    int lng2;
#endif
    int i;
    int omen_no;
    static int next_count = 0;
    SPRT_SDAT ssd;
    SPRT_SDAT ssd_csr;
    DISP_STR ds;
    DISP_SQAR dsq;
    SQAR_DAT menu_sq0 = {
        .w = 572,
        .h = 92,
        .x = 29,
        .y = 330,
        .pri = 0x5000,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x80,
    };

    spev_str.alpha = 0x80;

    SetSprFile(EVENT_ADDRESS);
    SetSprFile(EVENT_ADDRESS_2);

    if (spev_wrk.mode != 4 && spev_wrk.mode != 30)
    {
        for (i = 0; i < 11; i++)
        {
            SimpleDispSprt(&pzl_men_bg[i], EVENT_ADDRESS, i, NULL, NULL, 0x64);
        }
    }

    SimpleDispSprtDatCopy(&spev21_sp_menz[0], &ssd);
    ssd.alp = spev_wrk.csr[0];
    SimpleDispSprt(&ssd, EVENT_ADDRESS_2, 0, NULL, NULL, 0x64);

    if (face_no == 0)
    {
        SimpleDispSprtDatCopy(&spev21_sp_menz[1], &ssd);
        ssd.alp = spev_wrk.csr[1];
        SimpleDispSprt(&ssd, EVENT_ADDRESS_2, 1, NULL, NULL, 0x64);
    }
    else if (face_no == 1)
    {
        SimpleDispSprtDatCopy(&spev21_sp_menz[2], &ssd);
        ssd.alp = spev_wrk.csr[1];
        SimpleDispSprt(&ssd, EVENT_ADDRESS_2, 2, NULL, NULL, 0x64);
    }
    else if (face_no == 2)
    {
        SimpleDispSprtDatCopy(&spev21_sp_menz[3], &ssd);
        ssd.alp = spev_wrk.csr[1];
        SimpleDispSprt(&ssd, EVENT_ADDRESS_2, 3, NULL, NULL, 0x64);
    }
    else if (face_no == 3)
    {
        SimpleDispSprtDatCopy(&spev21_sp_menz[4], &ssd);
        ssd.alp = spev_wrk.csr[1];
        SimpleDispSprt(&ssd, EVENT_ADDRESS_2, 4, NULL, NULL, 0x64);
    }

    if (face_no == 0)
    {
        if (ev_wrk.face_stts[1] & 0x2 && spev_wrk.mode != 16)
        {
            omen_no = SPEV21_MSG18;
        }
        else
        {
            omen_no = SPEV21_MSG04;
        }
    }
    else if (face_no == 1)
    {
        if (ev_wrk.face_stts[1] & 0x4 && spev_wrk.mode != 16)
        {
            omen_no = SPEV21_MSG18;
        }
        else
        {
            omen_no = SPEV21_MSG05;
        }
    }
    else if (face_no == 2)
    {
        if (ev_wrk.face_stts[1] & 0x8 && spev_wrk.mode != 16)
        {
            omen_no = SPEV21_MSG18;
        }
        else
        {
            omen_no = SPEV21_MSG06;
        }
    }
    else if (face_no == 3)
    {
        if (ev_wrk.face_stts[1] & 0x10 && spev_wrk.mode != 16)
        {
            omen_no = SPEV21_MSG18;
        }
        else
        {
            omen_no = SPEV21_MSG07;
        }
    }

    if (spev_wrk.mode == 5)
    {
#ifdef BUILD_EU_VERSION
        int mes1;
        int mes2;

        if (sys_wrk.language == LANG_SPANISH)
        {
            mes1 = SPEV34_MSG00;
            mes2 = omen_no;
        }
        else
        {
            mes1 = omen_no;
            mes2 = SPEV34_MSG00;
        }

        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes1);
        lng1 = GetStrWidth(spev_str.str);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
        lng2 = GetStrWidth(spev_str.str);

        if (sys_wrk.language == LANG_FRENCH)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG06);
            lng0 = GetStrWidth(spev_str.str);

            mx = 320 - ((lng0 + 12 + lng1 + 12) + lng2) / 2;

            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            mx += lng0 + 12;
        }
        else
        {
            mx = 320 - (lng1 + 12 + lng2) / 2;
        }

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes1);
        lng1 = GetStrWidth(spev_str.str);

        spev_str.pos_x = mx;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        mx += lng1 + 12;

        if (omen_no == SPEV21_MSG18)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (face_no == 0)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (face_no == 1)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (face_no == 2)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (face_no == 3) {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }

        if (sys_wrk.language == LANG_SPANISH)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG06);
            spev_str.pos_x = mx + lng2;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
#else
        int lng1;
        int lng2;
        int mx;

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG00);
        lng2 = GetStrWidth(spev_str.str);

        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, omen_no);
        lng1 = GetStrWidth(spev_str.str);

        mx = (lng1 + lng2 + 12) / 2;

        spev_str.pos_x = 320 - mx;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (omen_no == SPEV21_MSG18)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG00);
            spev_str.pos_x = 320 + (lng1 + 12 - mx);
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (face_no == 0)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG00);
            spev_str.pos_x = 320 + (lng1 + 12 - mx);
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (face_no == 1)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG00);
            spev_str.pos_x = 320 + (lng1 + 12 - mx);
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (face_no == 2)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG00);
            spev_str.pos_x = 320 + (lng1 + 12 - mx);
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (face_no == 3) {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG00);
            spev_str.pos_x = 320 + (lng1 + 12 - mx);
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
#endif
    }

    if (spev_wrk.mode == 7 || spev_wrk.mode== 8 || spev_wrk.mode== 9)
    {
        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG00);
        spev_str.pos_x = 250;
        spev_str.pos_y = 340;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG01);
        spev_str.pos_x = 190;
        spev_str.pos_y = 365;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG17);
        spev_str.pos_x = 290;
        spev_str.pos_y = 390;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (spev_wrk.mode == 7)
        {
            YesNoCrslOKR(0x2000, 220.0f, 337.0f, 0x808080, 100.0f, 3.4f);
        }
        else if (spev_wrk.mode == 8)
        {
            YesNoCrslOKR(0x2000, 140.0f, 362.0f, 0x808080, 100.0f, 6.0f);
        }
        else if (spev_wrk.mode == 9)
        {
            YesNoCrslOKR(0x2000, 270.0f, 387.0f, 0x808080, 100.0f, 1.8f);
        }
    }

    if (spev_wrk.mode == 11 || spev_wrk.mode == 12)
    {
        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG02);
        spev_str.pos_x = 50;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_YES);
        spev_str.pos_x = 75;
        spev_str.pos_y = 373;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_NO);
        spev_str.pos_x = 180;
        spev_str.pos_y = 373;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (spev_wrk.mode != 11)
        {
            YesNoCrslOKR(0x2000, 150.0f, 370.0f, 0x808080, 100.0f, 1.5f);
        }
        else
        {
            YesNoCrslOKR(0x2000, 50.0f, 370.0f, 0x808080, 100.0f, 1.5f);
        }
    }
    else if (spev_wrk.mode == 13)
    {
        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG14);
        spev_str.pos_x = 50;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }
    else if (spev_wrk.mode == 16)
    {
        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG10);
        spev_str.pos_x = 50;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, omen_no);
#ifdef BUILD_EU_VERSION
        spev_str.pos_x = 50;
#else
        spev_str.pos_x = 161;
#endif
        spev_str.pos_y = 375;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

#ifdef BUILD_EU_VERSION
        lng = GetStrWidth(spev_str.str) + spev_str.pos_x + 12;
#endif

        if (face_no == 0)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG11);
#ifdef BUILD_EU_VERSION
            spev_str.pos_x = lng;
#else
            spev_str.pos_x = 250;
#endif
            spev_str.pos_y = 375;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (face_no == 1)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG11);
#ifdef BUILD_EU_VERSION
            spev_str.pos_x = lng;
#else
            spev_str.pos_x = 250;
#endif
            spev_str.pos_y = 375;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (face_no == 2)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG11);
#ifdef BUILD_EU_VERSION
            spev_str.pos_x = lng;
#else
            spev_str.pos_x = 218;
#endif
            spev_str.pos_y = 375;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG11);
#ifdef BUILD_EU_VERSION
            spev_str.pos_x = lng;
#else
            spev_str.pos_x = 250;
#endif
            spev_str.pos_y = 375;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
    }
    else if (spev_wrk.mode == 17)
    {
        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG03);
        spev_str.pos_x = 50;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }
    else if (spev_wrk.mode == 18)
    {
        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV02_MSG12);
        spev_str.pos_x = 50;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }
    else if (spev_wrk.mode == 19 || spev_wrk.mode == 20)
    {
#ifdef BUILD_EU_VERSION
        mx = 50;
#endif

        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

#ifdef BUILD_EU_VERSION
        if (sys_wrk.language == LANG_FRENCH)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG22);
            spev_str.pos_y = 350;
            spev_str.pos_x = mx;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            mx += GetStrWidth(spev_str.str) + 12;
        }
#endif

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, omen_no);
#ifdef BUILD_EU_VERSION
        spev_str.pos_x = mx;
#else
        spev_str.pos_x = 50;
#endif
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

#ifdef BUILD_EU_VERSION
        if (sys_wrk.language == LANG_FRENCH)
        {
            mx += GetStrWidth(spev_str.str);
        }
        else
        {
            mx += GetStrWidth(spev_str.str) + 12;
        }
#endif

        if (face_no == 0)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG16);
#ifdef BUILD_EU_VERSION
            spev_str.pos_x = mx;
#else
            spev_str.pos_x = 141;
#endif
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (face_no == 1)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG16);
#ifdef BUILD_EU_VERSION
            spev_str.pos_x = mx;
#else
            spev_str.pos_x = 138;
#endif
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (face_no == 2)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG16);
#ifdef BUILD_EU_VERSION
            spev_str.pos_x = mx;
#else
            spev_str.pos_x = 102;
#endif
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG16);
#ifdef BUILD_EU_VERSION
            spev_str.pos_x = mx;
#else
            spev_str.pos_x = 138;
#endif
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_YES);
        spev_str.pos_x = 75;
        spev_str.pos_y = 373;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_NO);
        spev_str.pos_x = 180;
        spev_str.pos_y = 373;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (spev_wrk.mode == 19)
        {
            YesNoCrslOKR(0x2000, 50.0f, 370.0f, 0x808080, 100.0f, 1.5f);
        }
        else
        {
            YesNoCrslOKR(0x2000, 150.0f, 370.0f, 0x808080, 100.0f, 1.5f);
        }
    }
    else if (spev_wrk.mode == 22)
    {
#ifdef BUILD_EU_VERSION
        mx = 50;
#endif

        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

#ifdef BUILD_EU_VERSION
        if (sys_wrk.language == LANG_FRENCH)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG21);
            spev_str.pos_x = mx;
            spev_str.pos_y = 342;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            mx += GetStrWidth(spev_str.str) + 12;
        }
#endif

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, omen_no);
#ifdef BUILD_EU_VERSION
        spev_str.pos_x = mx;
        spev_str.pos_y = 342;
#else
        spev_str.pos_x = 50;
        spev_str.pos_y = 345;
#endif

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

#ifdef BUILD_EU_VERSION
        mx += GetStrWidth(spev_str.str) + 12;
#endif

        if (face_no == 0)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG09);
#ifdef BUILD_EU_VERSION
            spev_str.pos_x = mx;
            spev_str.pos_y = 342;
#else
            spev_str.pos_x = 138;
            spev_str.pos_y = 345;
#endif

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (face_no == 1)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG09);
#ifdef BUILD_EU_VERSION
            spev_str.pos_x = mx;
            spev_str.pos_y = 342;
#else
            spev_str.pos_x = 138;
            spev_str.pos_y = 345;
#endif

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (face_no == 2)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG09);
#ifdef BUILD_EU_VERSION
            spev_str.pos_x = mx;
            spev_str.pos_y = 342;
#else
            spev_str.pos_x = 108;
            spev_str.pos_y = 345;
#endif

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG09);
#ifdef BUILD_EU_VERSION
            spev_str.pos_x = mx;
            spev_str.pos_y = 342;
#else
            spev_str.pos_x = 138;
            spev_str.pos_y = 345;
#endif

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG20);
        spev_str.pos_x = 50;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = 366;
#else
        spev_str.pos_y = 369;
#endif

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }
    else if (spev_wrk.mode == 30)
    {
#ifdef BUILD_EU_VERSION
        int mes2;
#endif

        if (spev_wrk.csr[2] == 1)
        {
#ifdef BUILD_EU_VERSION
            mx = 50;
#endif

            CopySqrDToSqr(&dsq, &menu_sq0);
            DispSqrD(&dsq);

            DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

#ifdef BUILD_EU_VERSION
            if (sys_wrk.language == LANG_FRENCH)
            {
                mes2 = 71;

                spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG19);
                spev_str.pos_x = mx;
                spev_str.pos_y = 350;

                CopyStrDToStr(&ds, &spev_str);
                SetMessageV2(&ds);

                mx += GetStrWidth(spev_str.str) + 12;
            }
            else
            {
                mes2 = 79;
            }

            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG18);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            mx += GetStrWidth(spev_str.str) + 12;

            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
#else
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG18);
            spev_str.pos_x = 50;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG19);
            spev_str.pos_x = 190;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
#endif
        }
        else
        {
#ifdef BUILD_EU_VERSION
            mx = 50;

            CopySqrDToSqr(&dsq, &menu_sq0);
            DispSqrD(&dsq);

            DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

            if (sys_wrk.language == LANG_FRENCH)
            {
                mes2 = 71;

                spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, 79);
                spev_str.pos_x = 50;
                spev_str.pos_y = 350;

                CopyStrDToStr(&ds, &spev_str);
                SetMessageV2(&ds);

                mx += GetStrWidth(spev_str.str) + 12;
            }
            else
            {
                mes2 = 79;
            }

            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, omen_no);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            mx += GetStrWidth(spev_str.str) + 12;

            if (face_no == 0) {
                spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
                spev_str.pos_x = mx;
                spev_str.pos_y = 350;

                CopyStrDToStr(&ds, &spev_str);
                SetMessageV2(&ds);
            }

            else if (face_no == 1) {
                spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
                spev_str.pos_x = mx;
                spev_str.pos_y = 350;

                CopyStrDToStr(&ds, &spev_str);
                SetMessageV2(&ds);
            }

            else if (face_no == 2)
            {
                spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
                spev_str.pos_x = mx;
                spev_str.pos_y = 350;

                CopyStrDToStr(&ds, &spev_str);
                SetMessageV2(&ds);
            }
            else if (face_no == 3)
            {
                spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
                spev_str.pos_x = mx;
                spev_str.pos_y = 350;

                CopyStrDToStr(&ds, &spev_str);
                SetMessageV2(&ds);
            }
#else
            CopySqrDToSqr(&dsq, &menu_sq0);
            DispSqrD(&dsq);

            DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, omen_no);
            spev_str.pos_x = 50;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            if (face_no == 0)
            {
                spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG19);
                spev_str.pos_x = 140;
                spev_str.pos_y = 350;

                CopyStrDToStr(&ds, &spev_str);
                SetMessageV2(&ds);
            }
            else if (face_no == 1)
            {
                spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG19);
                spev_str.pos_x = 140;
                spev_str.pos_y = 350;

                CopyStrDToStr(&ds, &spev_str);
                SetMessageV2(&ds);
            }
            else if (face_no == 2)
            {
                spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG19);
                spev_str.pos_x = 105;
                spev_str.pos_y = 350;

                CopyStrDToStr(&ds, &spev_str);
                SetMessageV2(&ds);
            }
            else if (face_no == 3)
            {
                spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG19);
                spev_str.pos_x = 140;
                spev_str.pos_y = 350;

                CopyStrDToStr(&ds, &spev_str);
                SetMessageV2(&ds);
            }
#endif
        }
    }

    next_count++;

    if (next_count > 80)
    {
        next_count = 0;
    }
}

void SurpriseDoorInit()
{
    if (SimenCheck() != 0)
    {
        open_close = 1;

        spev_wrk.mode = 4;
        spev_wrk.time = 40;
    }
    else
    {
        open_close = 0;
        spev_wrk.csr[0] = 0;
        spev_wrk.count = 0;
        spev_wrk.mode = 0;
        spev_wrk.time = 40;
        spev_wrk.csr[1] = 0x80;
        spev_str.alpha = 0x80;

        SetBlackOut();
    }

    spev_wrk.csr[0] = 0x80;
    spev_wrk.csr[1] = 0x80;

    go_flg_s = 0;
}

void SurpriseDoorMain()
{
    switch(spev_wrk.mode)
    {
    case 4:
        spev_wrk.ret = 1;

        if (DoorKeyUnlockOnly(29) != 0)
        {
            DoorOpenShiftForce(29);
        }
    break;
    case 0:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            LoadReq(TX_PZL_BG_AI_PK2, EVENT_ADDRESS);
            LoadReq(TX_PZL_MENZ_PK2, EVENT_ADDRESS_2);

            spev_wrk.mode = 1;
        }
    break;
    case 1:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEndAll() != 0)
            {
                SetSprFile(EVENT_ADDRESS);
                SetSprFile(EVENT_ADDRESS_2);

                spev_wrk.mode = 2;

                ingame_wrk.stts |= 0x20;

                pzl_load_id = SeFileLoadAndSet(ST003_DBIG_AKANAI_BD, 21);
            }
        }
    break;
    case 2:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEnd(pzl_load_id) != 0)
            {
                pzl_load_id = -1;

                spev_wrk.mode = 3;
                spev_wrk.time = 30;

                SetBlackIn();
            }
        }
    break;
    case 3:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (open_close == 0x1)
            {
                spev_wrk.mode = 23;
                spev_wrk.csr[1] = 0x80;
                spev_wrk.time = 1;
            }
            else
            {
                spev_wrk.mode = 24;
                spev_wrk.csr[1] = 0x80;
                spev_wrk.time = 1;

                SeStartFix(93, 0, 0x1000, 0x1000, 0);
            }
        }
    break;
    case 23:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            ButtonMarkTimeClear();

            go_flg_s = 1;

            spev_wrk.mode = 26;
            spev_wrk.time = 40;

            SetBlackOut();
        }
    break;
    case 24:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            ButtonMarkTimeClear();

            spev_wrk.mode = 26;
            spev_wrk.time = 40;

            SetBlackOut();
        }
    break;
    case 26:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (go_flg_s == 1)
            {
                if (DoorKeyUnlockOnly(29) != 0)
                {
                    DoorOpenShiftForce(29);
                }

                go_flg_s = 0;
            }

            spev_wrk.mode = 31;
        }
    break;
    case 31:
        SendManMdlTex();

        spev_wrk.ret = 1;

        ingame_wrk.stts &= ~0x20;

        SetBlackIn();
    break;
    }

    if (spev_wrk.mode > 1 && spev_wrk.mode != 4 && spev_wrk.mode != 31)
    {
        SurpriseDoorDisp(open_close);
    }
}

void SurpriseDoorDisp(int face_no)
{
    int i;
    static int next_count = 0;
    SPRT_SDAT ssd;
    SPRT_SDAT ssd_csr;
    DISP_STR ds;
    DISP_SQAR dsq;
    SQAR_DAT menu_sq0 = {
        .w = 572,
        .h = 92,
        .x = 29,
        .y = 330,
        .pri = 0x5000,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x80,
    };

    SetSprFile(EVENT_ADDRESS);
    SetSprFile(EVENT_ADDRESS_2);

    spev_str.alpha = 0x80;

    for (i = 0 ; i < 11; i++)
    {
        SimpleDispSprt(&pzl_men_bg[i], EVENT_ADDRESS, i, NULL, NULL, 0x64);
    }

    SimpleDispSprtDatCopy(&spev21_sp_menz[5], &ssd);

    ssd.alp = spev_wrk.csr[1];

    SimpleDispSprt(&ssd, EVENT_ADDRESS_2, 5, NULL, NULL, 0x64);

    if (spev_wrk.mode == 23)
    {
        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);
        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV04_MSG01);
        spev_str.pos_x = 50;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }
    else
    {
        if (spev_wrk.mode == 0x18)
        {
            CopySqrDToSqr(&dsq, &menu_sq0);
            DispSqrD(&dsq);
            DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, 0x7f);

            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG15);
            spev_str.pos_x = 50;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
    }
}

static u_char omen_no = 0;
static u_char stts_no = 0;
static u_char omen_msgno = 0;
static u_char omen_itemno = 0;

void SimenPillarInit(int event_no)
{
    open_close = SimenCheck();

    alpha_keep = 0;

    if ((ev_wrk.face_stts[0] & 0x1) == 0)
    {
        ev_wrk.face_stts[0] &= (0x80 | 0x40 | 0x20 | 0x10);
        ev_wrk.face_stts[0] |= (0x10 | 0x1);
    }

    SpevWrkInit();

    pzl_load_id = SeFileLoadAndSet(ST001_MEN_KAKE_BD, 21);

    spev_wrk.csr[1] = 0x80;
    spev_wrk.mode = 4;
    spev_wrk.time = 20;
}

void SimenPillarMain(int event_no)
{
    if (spev_wrk.mode > 1)
    {
        SimenPillarDisp();
    }

    switch(spev_wrk.mode)
    {
    case 0:
        LoadReq(TX_PZL_BG_AI_PK2, EVENT_ADDRESS);
        LoadReq(TX_PZL_MENZ_PK2, EVENT_ADDRESS_2);

        spev_wrk.mode = 1;
        spev_wrk.time = 40;

        SetBlackOut();
    break;
    case 1:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEndAll() == 0)
            {
                return;
            }

            SetSprFile(EVENT_ADDRESS);
            SetSprFile(EVENT_ADDRESS_2);

            ingame_wrk.stts |= 0x20;

            spev_wrk.mode = 3;
            spev_wrk.time = 30;

            SetBlackIn();
            SeStartFix(93, 0, 0x1000, 0x1000, 0);
        }
    break;
    case 3:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (open_close == 1)
            {
                spev_wrk.mode = 23;
                spev_wrk.time = 1;
            }
            else
            {
                spev_wrk.mode = 24;
                spev_wrk.time = 1;
            }
        }
    break;
    case 4:
        if (IsLoadEnd(pzl_load_id) == 0)
        {
            return;
        }

        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            pzl_load_id = -1;

            if (event_no == 34)
            {
                omen_no = 0;
                stts_no = 2;
                omen_msgno = 64;
                choudo_no = 126;
                omen_itemno = 41;

                ReqDramaCamera(1, 23, 0);
            }
            else if (event_no == 54)
            {
                omen_no = 1;
                stts_no = 4;
                omen_msgno = 65;
                choudo_no = 125;
                omen_itemno = 42;

                ReqDramaCamera(1, 24, 0);
            }
            else if (event_no == 55)
            {
                omen_no = 2;
                stts_no = 8;
                omen_msgno = 66;
                choudo_no = 127;
                omen_itemno = 43;

                ReqDramaCamera(1, 21, 0);
            }
            else if (event_no == 56)
            {
                omen_no = 3;
                stts_no = 16;
                omen_msgno = 67;
                choudo_no = 124;
                omen_itemno = 44;

                ReqDramaCamera(1, 22, 0);
            }

            if (ev_wrk.face_stts[0] & stts_no)
            {
                spev_wrk.mode = 7;
                spev_wrk.time = 1;
            }
            else if (poss_item[omen_itemno] == 0)
            {
                spev_wrk.mode = 17;
                spev_wrk.time = 1;
            }
            else
            {
                spev_wrk.mode = 19;
                spev_wrk.time = 1;
            }
        }
    break;
    case 7:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0 && ButtonMarkWait() == 0)
        {
            spev_wrk.mode = 8;
            spev_wrk.time = 1;
        }
    break;
    case 6:
    case 8:
        if (*key_now[5] == 1)
        {
            spev_wrk.time = 0;
        }
        else if (*key_now[4] == 1)
        {
            spev_wrk.mode = 32;
            spev_wrk.time = 10;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (
            (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            ) || (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            )
        )
        {
            spev_wrk.mode = spev_wrk.mode == 6 ? 8 : 6;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }

        if (spev_wrk.time != 0)
        {
            return;
        }

        if (spev_wrk.mode == 8)
        {
            open_close = SimenCheck();

            poss_item[omen_itemno] = 1;

            ev_wrk.face_stts[0] &= ~stts_no;

            if (open_close == 1)
            {
                open_close = SimenCheck();

                spev_wrk.mode = 0;
                spev_wrk.time = 40;
            }
            else
            {
                spev_wrk.mode = 27;
                spev_wrk.time = 40;

                SetBlackOut();
            }
        }
        else
        {
            spev_wrk.mode = 32;
            spev_wrk.time = 10;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
    break;
    case 17:
        if (*key_now[5] == 1 || *key_now[4] == 1)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            spev_wrk.mode = 32;
            spev_wrk.time = 10;
        }
    break;
    case 19:
    case 20:
        if (*key_now[5] == 1)
        {
            spev_wrk.time = 0;
        }
        else if (*key_now[4] == 1)
        {
            spev_wrk.mode = 32;
            spev_wrk.time = 10;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (
            (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            ) || (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            )
        )
        {
            spev_wrk.mode = spev_wrk.mode == 19 ? 20 : 19;
            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }

        if (spev_wrk.time != 0)
        {
            return;
        }

        if (spev_wrk.mode == 19)
        {
            ev_wrk.face_stts[0] |= stts_no;

            poss_item[omen_itemno] = 0;

            open_close = SimenCheck();

            if (open_close == 1)
            {
                spev_wrk.mode = 0;
                spev_wrk.time = 40;
            }
            else
            {
                spev_wrk.mode = 29;
                spev_wrk.time = 40;

                SetBlackOut();
            }
        }
        else
        {
            spev_wrk.mode = 32;
            spev_wrk.time = 10;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
    break;
    case 23:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            ButtonMarkTimeClear();

            spev_wrk.mode = 26;
            spev_wrk.time = 40;

            SetBlackOut();
            SetFurnAttrEve(choudo_no, 0x4000, 0);
        }
    break;
    case 24:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time != 0)
        {
            return;
        }

        ButtonMarkTimeClear();
        SetFurnAttrEve(choudo_no, 0x4000, 1);

        spev_wrk.mode = 26;
        spev_wrk.time = 40;

        SetBlackOut();
    break;
    case 26:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 31;
        }
    break;
    case 27:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            SetFurnAttrEve(choudo_no, 0x4000, 1);
            SeStartFix(93, 0, 0x1000, 0x1000, 0);

            spev_wrk.mode = 30;

            SetBlackIn();
        }
    break;
    case 29:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            SetFurnAttrEve(choudo_no, 0x4000, 0);
            SeStartFix(93, 0, 0x1000, 0x1000, 0);

            spev_wrk.mode = 32;

            SetBlackIn();
        }
    break;
    case 30:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            ButtonMarkTimeClear();

            spev_wrk.mode = 32;
        }
    break;
    case 31:
        if (event_no == 34)
        {
            ReqDramaCamera(0, 23, 0);
        }

        if (event_no == 54)
        {
            ReqDramaCamera(0, 24, 0);
        }

        if (event_no == 55)
        {
            ReqDramaCamera(0, 21, 0);
        }

        if (event_no == 56)
        {
            ReqDramaCamera(0, 22, 0);
        }

        SendManMdlTex();

        spev_wrk.ret = 1;

        ingame_wrk.stts &= ~0x20;

        SetBlackIn();
    break;
    case 32:
        if (ButtonMarkNext(0, 0, 1) == 0)
        {
            if (event_no == 34)
            {
                ReqDramaCamera(0, 23, 0);
            }

            if (event_no == 54)
            {
                ReqDramaCamera(0, 24, 0);
            }

            if (event_no == 55)
            {
                ReqDramaCamera(0, 21, 0);
            }

            if (event_no == 56)
            {
                ReqDramaCamera(0, 22, 0);
            }

            ButtonMarkTimeClear();

            spev_wrk.ret = 1;

            break;
        }
    case 33:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.ret = 1;
        }
    break;
    }
}

int SimenCheck()
{
    int i;

    i = 0;

    if (ev_wrk.face_stts[0] & 0x2)
    {
        i++;
    }

    if (ev_wrk.face_stts[0] & 0x4)
    {
        i++;
    }

    if (ev_wrk.face_stts[0] & 0x8)
    {
        i++;
    }

    if (ev_wrk.face_stts[0] & 0x10)
    {
        i++;
    }

    if (i != 4)
    {
        return 0;
    }

    return 1;
}

void SimenPillarDisp()
{
#ifdef BUILD_EU_VERSION
    int lng0;
    int lng1;
    int lng2;
    int mx;
#endif
    int i;
    static int next_count = 0;
    SPRT_SDAT ssd;
    SPRT_SDAT ssd_csr;
    DISP_STR ds;
    DISP_SQAR dsq;
    SQAR_DAT menu_sq0 = {
        .w = 572,
        .h = 92,
        .x = 29,
        .y = 330,
        .pri = 0x5000,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x80,
    };

    menu_sq0.alpha = alpha_keep;

    spev_str.alpha = 0x80;

    if (spev_wrk.mode == 3 || spev_wrk.mode == 23 || spev_wrk.mode == 24 || spev_wrk.mode == 26)
    {
        SetSprFile(EVENT_ADDRESS);

        for (i = 0; i < 11; i++)
        {
            SimpleDispSprt(&pzl_men_bg[i], EVENT_ADDRESS, i, NULL, NULL, 0x64);
        }

        SimpleDispSprtDatCopy(&spev21_sp_menz[5], &ssd);

        ssd.alp = spev_wrk.csr[1];

        SimpleDispSprt(&ssd, EVENT_ADDRESS_2, 5, NULL, NULL, 0x64);
    }

    if (alpha_keep <= 0x76 && spev_wrk.mode != 32)
    {
        alpha_keep += 9;
    }
    else if (alpha_keep > 0 && spev_wrk.mode == 32)
    {
        alpha_keep -= 9;

        if (alpha_keep <= 0)
        {
            alpha_keep = 0;
        }
    }

    if (spev_wrk.mode == 7)
    {
#ifdef BUILD_EU_VERSION
        int mes1;
        int mes2;

        if (sys_wrk.language == LANG_SPANISH)
        {
            mes1 = SPEV34_MSG00;
            mes2 = omen_msgno;
        }
        else {
            mes1 = omen_msgno;
            mes2 = SPEV34_MSG00;
        }

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, alpha_keep);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes1);
        lng1 = GetStrWidth(spev_str.str);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
        lng2 = GetStrWidth(spev_str.str);

        if (sys_wrk.language == LANG_FRENCH)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG06);
            lng0 = GetStrWidth(spev_str.str);

            mx = 320 - ((lng0 + 12 + lng1 + 12) + lng2) / 2;

            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            mx += lng0 + 12;
        }
        else
        {
            mx = 320 - (lng1 + 12 + lng2) / 2;
        }

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes1);
        lng1 = GetStrWidth(spev_str.str);

        spev_str.pos_x = mx;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        mx += lng1 + 12;

        if (omen_no == 0)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (omen_no == 1)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (omen_no == 2)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }

        if (sys_wrk.language == LANG_SPANISH)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG06);
            spev_str.pos_x = mx + lng2;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
#else
        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, alpha_keep);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, omen_msgno);
        spev_str.pos_x = 130;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (omen_no == 0)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG00);
            spev_str.pos_x = 218;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (omen_no == 1)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG00);
            spev_str.pos_x = 218;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (omen_no == 2)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG00);
            spev_str.pos_x = 185;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG00);
            spev_str.pos_x = 218;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
#endif
    }

    if (spev_wrk.mode == 6 || spev_wrk.mode == 8)
    {
#ifdef BUILD_EU_VERSION
        int mes1;
        int mes2;

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, alpha_keep);

        if (sys_wrk.language == LANG_FRENCH)
        {
            mes1 = omen_msgno;
            mes2 = 76;
        }
        else
        {
            mes1 = omen_msgno;
            mes2 = 84;
        }

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, omen_msgno);
        lng1 = GetStrWidth(spev_str.str);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
        lng2 = GetStrWidth(spev_str.str);

        if (sys_wrk.language == LANG_FRENCH)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG01);
            lng0 = GetStrWidth(spev_str.str);

            mx = 320 - (((lng0 + 12) + lng1 + lng2) / 2);

            spev_str.pos_x =  mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            mx += lng0 + 12;
        }
        else
        {
            mx = 320 - (lng1 + 12 + lng2) / 2;
        }

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes1);
        spev_str.pos_x = mx;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (sys_wrk.language == LANG_FRENCH)
        {
            mx += lng1;
        }
        else
        {
            mx += lng1 + 12;
        }

        if (omen_no == 0)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (omen_no == 1)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (omen_no == 2)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_YES);
        spev_str.pos_x = 215;
        spev_str.pos_y = 378;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_NO);
        spev_str.pos_x = 320;
        spev_str.pos_y = 378;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (spev_wrk.mode == 8)
        {
            YesNoCrslOKR(0x2000, 190.0f, 375.0f, 0x808080, 100.0f, 1.5f);
        }
        else
        {
            YesNoCrslOKR(0x2000, 290.0f, 375.0f, 0x808080, 100.0f, 1.5f);
        }
#else
        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, alpha_keep);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, omen_msgno);
        spev_str.pos_x = 130;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (omen_no == 0)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG01);
            spev_str.pos_x = 218;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (omen_no == 1)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG01);
            spev_str.pos_x = 218;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (omen_no == 2)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG01);
            spev_str.pos_x = 185;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG01);
            spev_str.pos_x = 218;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_YES);
        spev_str.pos_x = 215;
        spev_str.pos_y = 378;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_NO);
        spev_str.pos_x = 320;
        spev_str.pos_y = 378;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (spev_wrk.mode == 8)
        {
            YesNoCrslOKR(0x2000, 190.0f, 375.0f, 0x808080, 100.0f, 1.5f);
        }
        else
        {
            YesNoCrslOKR(0x2000, 290.0f, 375.0f, 0x808080, 100.0f, 1.5f);
        }
#endif
    }

    if (spev_wrk.mode == 17)
    {
        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, alpha_keep);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG02);
        spev_str.pos_x = 50;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }

    if (spev_wrk.mode == 19 || spev_wrk.mode == 20)
    {
#ifdef BUILD_EU_VERSION
        int mes1;

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, alpha_keep);

        mes1 = omen_msgno;

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes1);
        lng1 = GetStrWidth(spev_str.str);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG03);
        lng2 = GetStrWidth(spev_str.str);

        if (sys_wrk.language == LANG_FRENCH)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG07);
            lng0 = GetStrWidth(spev_str.str);

            mx = 320 - ((lng0 + 12 + lng1 + 12) + lng2) / 2;

            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            mx += lng0 + 12;
        }
        else
        {
            mx = 320 - (lng1 + 12 + lng2) / 2;
        }

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes1);
        lng1 = GetStrWidth(spev_str.str);

        spev_str.pos_x = mx;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        mx += 12 + lng1;

        if (omen_no == 0)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG03);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (omen_no == 1)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG03);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (omen_no == 2)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG03);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG03);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_YES);
        spev_str.pos_x = 215;
        spev_str.pos_y = 378;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_NO);
        spev_str.pos_x = 320;
        spev_str.pos_y = 378;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (spev_wrk.mode == 19)
        {
            YesNoCrslOKR(0x2000, 190.0f, 375.0f, 0x808080, 100.0f, 1.5f);
        }
        else
        {
            YesNoCrslOKR(0x2000, 290.0f, 375.0f, 0x808080, 100.0f, 1.5f);
        }
#else
        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, alpha_keep);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, omen_msgno);
        spev_str.pos_x = 130;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (omen_no == 0)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG03);
            spev_str.pos_x = 218;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (omen_no == 1)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG03);
            spev_str.pos_x = 218;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (omen_no == 2)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG03);
            spev_str.pos_x = 187;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG03);
            spev_str.pos_x = 218;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_YES);
        spev_str.pos_x = 215;
        spev_str.pos_y = 378;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_NO);
        spev_str.pos_x = 320;
        spev_str.pos_y = 378;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (spev_wrk.mode == 19)
        {
            YesNoCrslOKR(0x2000, 190.0f, 375.0f, 0x808080, 100.0f, 1.5f);
        }
        else
        {
            YesNoCrslOKR(0x2000, 290.0f, 375.0f, 0x808080, 100.0f, 1.5f);
        }
#endif
    }

    if (spev_wrk.mode == 23)
    {
        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, alpha_keep);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG05);
        spev_str.pos_x = 50;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = 340;
#else
        spev_str.pos_y = 350;
#endif

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }

    if (spev_wrk.mode == 30)
    {
#ifdef BUILD_EU_VERSION
        int mes1;
        int mes2;

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, alpha_keep);

        if (sys_wrk.language == LANG_FRENCH)
        {
            mes1 = omen_msgno;
            mes2 = SPEV21_MSG11;
        }
        else
        {
            mes1 = omen_msgno;
            mes2 = SPEV21_MSG19;
        }

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes1);
        lng1 = GetStrWidth(spev_str.str);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
        lng2 = GetStrWidth(spev_str.str);

        if (sys_wrk.language == LANG_FRENCH)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG19);
            lng0 = GetStrWidth(spev_str.str);

            mx = 320 - (((lng0 + 12) + lng1 + lng2) / 2);

            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);

            mx += 12 + lng0;
        }
        else
        {
            mx = 320 - ((lng1 + 12) + lng2) / 2;
        }

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes1);
        spev_str.pos_x = mx;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (sys_wrk.language == LANG_FRENCH)
        {
            mx += lng1;
        }
        else
        {
            mx += lng1 + 12;
        }

        if (omen_no == 0)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (omen_no == 1)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (omen_no == 2)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, mes2);
            spev_str.pos_x = mx;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
#else
        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, alpha_keep);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, omen_msgno);
        spev_str.pos_x = 130;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (omen_no == 0)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG19);
            spev_str.pos_x = 218;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (omen_no == 1)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG19);
            spev_str.pos_x = 218;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else if (omen_no == 2)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG19);
            spev_str.pos_x = 188;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG19);
            spev_str.pos_x = 218;
            spev_str.pos_y = 350;

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
#endif
    }
    else if (spev_wrk.mode == 24)
    {
        CopySqrDToSqr(&dsq, &menu_sq0);
        DispSqrD(&dsq);

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, alpha_keep);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV34_MSG04);
        spev_str.pos_x = 50;
        spev_str.pos_y = 350;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }
    else if (spev_wrk.mode == 32)
    {
        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, alpha_keep);
    }
}

ZUSHI_WRK zushi_dat[4] = {
    { .answer = 2, .ansflg = 0 },
    { .answer = 1, .ansflg = 0 },
    { .answer = 0, .ansflg = 0 },
    { .answer = 3, .ansflg = 0 },
};

void IkariMenInit()
{
    return;
}

void IkariMenComeOn()
{
    ev_wrk.face_stts[0] &= ~0x4;

    SetFurnAttrEve(125, 0x4000, 1);

    DoorChangeModel(26, dmodel_table[1][1]);

    if (door_keep[0x1a].room_id != id_table[1][1])
    {
        ChangeRotDoorFace2Room(26, id_table[1][1]);
    }

    door_keep[26].attr |= 0x2;

    event_stts[ev_wrk.evt_no] = 1;

    poss_item[42] = 0;

    spev_wrk.ret = 1;
}

void HanyouKaitenInit(int event_no)
{
    return;
}

void HanyouKaitenMain(int event_no)
{
    switch(event_no)
    {
    case 86:
        if (plyr_wrk.pr_info.room_no == door_keep[47].room_id)
        {
            if (DoorKeyUnlockOnly(47) != 0)
            {
                DoorOpenShiftForce(47);
            }
        }
        else
        {
            SetMessageAddr(GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG12));

            if (SetMessage() != 0)
            {
                return;
            }
        }

        spev_wrk.ret = 1;
    break;
    case 87:
        if (plyr_wrk.pr_info.room_no == door_keep[26].room_id)
        {
            if (DoorKeyUnlockOnly(26) != 0)
            {
                DoorOpenShiftForce(26);
            }
        }
        else
        {
            SetMessageAddr(GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG12));

            if (SetMessage() != 0)
            {
                return;
            }
        }

        spev_wrk.ret = 1;
    break;
    case 88:
        if (plyr_wrk.pr_info.room_no == door_keep[0x22].room_id)
        {
            if (DoorKeyUnlockOnly(34) != 0)
            {
                DoorOpenShiftForce(34);
            }
        }
        else
        {
            SetMessageAddr(GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG12));

            if (SetMessage() != 0)
            {
                return;
            }
        }

        spev_wrk.ret = 1;
    break;
    case 89:
        if (plyr_wrk.pr_info.room_no == door_keep[49].room_id)
        {
            if (DoorKeyUnlockOnly(49) != 0)
            {
                DoorOpenShiftForce(49);
            }
        }
        else
        {
            SetMessageAddr(GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG12));

            if (SetMessage() != 0)
            {
                return;
            }
        }

        spev_wrk.ret = 1;
    break;
    case 90:
        if (plyr_wrk.pr_info.room_no == door_keep[54].room_id)
        {
            if (DoorKeyUnlockOnly(54) != 0)
            {
                DoorOpenShiftForce(54);
            }
        }
        else
        {
            SetMessageAddr(GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG12));

            if (SetMessage() != 0)
            {
                return;
            }
        }

        spev_wrk.ret = 1;
    break;
    }
}

void ZushiBonjiInit(int bonji_no)
{
    if (plyr_wrk.se_deadly != -1)
    {
        SeFadeFlame(plyr_wrk.se_deadly, 60, 0);

        plyr_wrk.se_deadly = -1;

        spev_wrk.time = 60;
        spev_wrk.mode = 0;
    }
    else
    {

#ifdef BUILD_EU_VERSION
        LoadReqLanguage(EVTEX_ZUSI_E_PK2, EVENT_ADDRESS);
#else
        LoadReq(EVTEX_ZUSI_PK2, EVENT_ADDRESS);
#endif

        pzl_load_id = -1;

        spev_wrk.mode = 1;
        spev_wrk.time = 40;
    }

    SetBlackOut();

    spev_wrk.csr[0] = 0;

    zushi_alpha[0] = 0;
    zushi_alpha[1] = 0;

    if (bonji_no == 42)
    {
        spev_wrk.csr[1] = 0;
    }
    else if (bonji_no == 43)
    {
        spev_wrk.csr[1] = 1;
    }
    else if (bonji_no == 44)
    {
        spev_wrk.csr[1] = 2;
    }
    else
    {
        spev_wrk.csr[1] = 3;
    }

    SpevStrInit();

    spev_wrk.count = 0;
}

void ZushiBonjiMain(int bonji_no)
{
    switch(spev_wrk.mode)
    {
    case 0:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
#ifdef BUILD_EU_VERSION
        LoadReqLanguage(EVTEX_ZUSI_E_PK2, EVENT_ADDRESS);
#else
            LoadReq(EVTEX_ZUSI_PK2, EVENT_ADDRESS);
#endif

            pzl_load_id = -1;

            spev_wrk.mode = 1;
            spev_wrk.time = 40;
        }
    break;
    case 1:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEndAll() != 0)
            {
                SetSprFile(EVENT_ADDRESS);

                spev_wrk.mode = 3;

                ingame_wrk.stts |= 0x20;

                pzl_load_id = SeFileLoadAndSet(SP005_BONJI_BD, 2);
            }
        }
    break;
    case 3:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEnd(pzl_load_id) != 0)
            {
                pzl_load_id = -1;

                spev_wrk.mode = 2;
                spev_wrk.time = 40;

                SetBlackIn();
            }
        }
    break;
    case 2:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 4;
            spev_wrk.time = 1;
        }
    break;
    case 4:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            spev_wrk.mode = 6;
            spev_wrk.time = 1;
        }
    break;
    case 6:
        if (ButtonMarkWait() != 0)
        {
            break;
        }

        if (*key_now[4] == 1)
        {
            spev_wrk.mode = 10;
            spev_wrk.time = 70;

            SetBlackOut();
        }
        else if (*key_now[5] == 1)
        {
            spev_wrk.mode = 7;
            spev_wrk.time = 40;

            SeStartFix(33, 0, 0x1000, 0x1000, 0);
        }
        else if (
            *key_now[3] == 1 ||
            (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
            Ana2PadDirCnt(1) == 1 ||
            (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
        )
        {
            if (spev_wrk.csr[0] <= 2)
            {
                spev_wrk.csr[0]++;
            }
            else
            {
                spev_wrk.csr[0] = 0;
            }

            zushi_alpha[0] = 0;
        }
        else if (
            *key_now[2] == 1 ||
            (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
            Ana2PadDirCnt(3) == 1 ||
            (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
        )
        {
            if (spev_wrk.csr[0] != 0)
            {
                spev_wrk.csr[0]--;
            }
            else
            {
                spev_wrk.csr[0] = 3;
            }

            zushi_alpha[0] = 0;
        }
    break;
    case 7:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (spev_wrk.csr[0] == zushi_dat[spev_wrk.csr[1]].answer)
            {
                spev_wrk.mode = 5;
                spev_wrk.time = 20;
            }
            else
            {
                spev_wrk.mode = 9;
                spev_wrk.time = 40;
            }
        }
    break;
    case 5:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            ButtonMarkTimeClear();

            spev_wrk.mode = 8;
            spev_wrk.time = 20;

            SeStartFix(34, 0, 0x1000, 0x1000, 0);
        }
    break;
    case 8:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 11;
            spev_wrk.time = 70;

            zushi_dat[spev_wrk.csr[1]].ansflg = 1;

            event_stts[ev_wrk.evt_no] = 1;

            SetWhiteOut();
        }
    break;
    case 9:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            spev_wrk.mode = 6;
            spev_wrk.time = 1;

            spev_wrk.csr[0] = 0;
        }
    break;
    case 11:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (pzl_load_id == -1)
            {
                pzl_load_id = SeFileLoadAndSet(SV000_MIKU_BD, 2);
            }
            else
            {
                if (IsLoadEnd(pzl_load_id) == 0)
                {
                    return;
                }

                spev_wrk.mode = 12;

                SetWhiteIn();
            }
        }
    break;
    case 10:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (pzl_load_id == -1)
            {
                pzl_load_id = SeFileLoadAndSet(SV000_MIKU_BD, 2);
            }
            else
            {
                if (IsLoadEnd(pzl_load_id) == 0)
                {
                    return;
                }

                spev_wrk.mode = 12;

                SetBlackIn();
            }
        }
    break;
    case 12:
        SendManMdlTex();
        ButtonMarkTimeClear();

        ingame_wrk.stts &= ~0x20;

        spev_wrk.ret = 1;

        spev_str.alpha = 0x80;
    break;
    }

    if (spev_wrk.mode == 6)
    {
        if (zushi_alpha[0] < 0x80)
        {
            zushi_alpha[0] += 3;

            if (zushi_alpha[0] >= 0x80)
            {
                zushi_alpha[0] = 0x80;
            }
        }
    }

    if (spev_wrk.mode == 7)
    {
        zushi_alpha[0] = 0;
    }

    if (spev_wrk.mode == 9)
    {
        if (zushi_alpha[0] > 3)
        {
            zushi_alpha[0] -= 3;

            if  (zushi_alpha[0] <= 3)
            {
                zushi_alpha[0] = 0;
            }
        }
    }

    if (spev_wrk.mode == 4 || spev_wrk.mode == 5 || spev_wrk.mode == 9)
    {
        if (zushi_alpha[1] < 0x80)
        {
            zushi_alpha[1] += 6;

            if (zushi_alpha[1] >= 0x80)
            {
                zushi_alpha[1] = 0x80;
            }
        }
    }
    else
    {
        if (zushi_alpha[1] > 6)
        {
            zushi_alpha[1] -= 6;

            if (zushi_alpha[1] <= 6)
            {
                zushi_alpha[1] = 0;
            }
        }
    }

    if (spev_wrk.mode > 1 && spev_wrk.mode != 12)
    {
        ZushiBonjiDisp(bonji_no);
    }
}

void ZushiBonjiDisp(int bonji_no)
{
    int i;
    static int next_count = 0;
    SPRT_SDAT ssd;
    SPRT_SDAT ssd_csr;
    DISP_STR ds;
    DISP_SQAR dsq;
    SQAR_DAT menu_sq0  = {
        .w = 572,
        .h = 92,
        .x = 29,
        .y = 330,
        .pri = 0x5000,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x80,
    };

    menu_sq0.alpha = zushi_alpha[1];

    SetSprFile(EVENT_ADDRESS);

    for (i = 0; i < 11; i++)
    {
        SimpleDispSprtLNR(&spev21_sp_bak[i], EVENT_ADDRESS, i, NULL, NULL, 0x64, 0);
    }

    if (spev_wrk.mode == 7 || spev_wrk.mode == 5 || spev_wrk.mode == 8 || spev_wrk.mode == 11)
    {
        for (i = 0; i < 4; i++)
        {
            if (i == spev_wrk.csr[0])
            {
                SimpleDispSprtLNR(&spev21_sp_zbt[i], EVENT_ADDRESS, i + 13, NULL, NULL, 0x64, 1);
            }
        }
    }

    for (i = 0; i < 4; i++)
    {
        if (i == spev_wrk.csr[0])
        {
            SimpleDispSprtDatCopy(&spev21_sp_zfl[i], &ssd);

            ssd.alp = zushi_alpha[0];

            SimpleDispSprtLNR(&ssd, EVENT_ADDRESS, 12, NULL, NULL, 0x64, 0);
        }
    }

    for (i = 0; i < 4; i++)
    {
        if (
            (spev_wrk.mode == 7 || spev_wrk.mode == 5 || spev_wrk.mode == 8 || spev_wrk.mode == 11) &&
            (i == spev_wrk.csr[0])
        )
        {
            SimpleDispSprtLNR(&spev21_sp_zft[i+4], EVENT_ADDRESS, 11, NULL, NULL, 0x64, 0);
        }
        else
        {
            SimpleDispSprtLNR(&spev21_sp_zft[i], EVENT_ADDRESS, 11, NULL, NULL, 0x64, 0);
        }
    }

    CopySqrDToSqr(&dsq, &menu_sq0);
    DispSqrD(&dsq);

    DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, zushi_alpha[1]);

    if (spev_wrk.mode == 4)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV42_MSG01);

        spev_str.pos_x = 50;
        spev_str.pos_y = 350;
        spev_str.alpha = zushi_alpha[1];

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }

    if (spev_wrk.mode == 5)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV42_MSG06);
        spev_str.pos_x = 50;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = 340;
#else
        spev_str.pos_y = 350;
#endif
        spev_str.alpha = zushi_alpha[1];

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }

    if (spev_wrk.mode == 9)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV42_MSG02);
        spev_str.pos_x = 50;
        spev_str.pos_y = 350;
        spev_str.alpha = zushi_alpha[1];

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }

    for (i = 0; i < 1; i++)
    {
        SimpleDispSprtLNR(&spev21_sp_cpf[i], EVENT_ADDRESS, 17, NULL, NULL, 0x64, 0);
    }

    SimpleDispSprtLNR(spev21_sp_cap, EVENT_ADDRESS, 18, NULL, NULL, 0x64, 0);

    DrawButtonTex(0xa000, 3, spev21_sp_cap[1].x, spev21_sp_cap[1].y, 0x64);
    DrawButtonTex(0xa000, 1, spev21_sp_cap[2].x, spev21_sp_cap[2].y, 0x64);
}

void ZushiBonjiMSGInit()
{
#ifdef BUILD_EU_VERSION
        LoadReqLanguage(EVTEX_ZUSI_E_PK2, EVENT_ADDRESS);
#else
    LoadReq(EVTEX_ZUSI_PK2, EVENT_ADDRESS);
#endif

    spev_wrk.mode = 1;
    spev_wrk.time = 40;

    SetBlackOut();
    SpevStrInit();

    spev_wrk.count = 0;
}

void ZushiBonjiMSGMain()
{
    switch(spev_wrk.mode)
    {
    case 1:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEndAll() != 0)
            {
                SetSprFile(EVENT_ADDRESS);

                spev_wrk.mode = 2;
                spev_wrk.time = 40;

                ingame_wrk.stts |= 0x20;

                SetBlackIn();
            }
            break;
        }
    case 2:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 4;
            spev_wrk.time = 1;
        }
    break;
    case 4:
        if (*key_now[5] == 1 || *key_now[4] == 1)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            spev_wrk.mode = 5;
            spev_wrk.time = 1;
        }
    break;
    case 5:
        if (*key_now[5] == 1 || *key_now[4] == 1)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            spev_wrk.mode = 10;
            spev_wrk.time = 70;

            SetBlackOut();
        }
    break;
    case 10:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 12;
        }
    break;
    case 12:
        ingame_wrk.stts &= ~0x20;

        spev_wrk.ret = 1;

        SetBlackIn();
    break;
    }

    if (spev_wrk.mode == 4 || spev_wrk.mode == 5)
    {
        if (zushi_alpha[1] < 0x80)
        {
            zushi_alpha[1] += 6;

            if (zushi_alpha[1] >= 0x80)
            {
                zushi_alpha[1] = 0x80;
            }
        }
    }
    else
    {
        if (zushi_alpha[1] > 6)
        {
            zushi_alpha[1] -= 6;

            if (zushi_alpha[1] <= 6)
            {
                zushi_alpha[1] = 0;
            }
        }
    }

    if (spev_wrk.mode != 1)
    {
        ZushiBonjiMSGDisp();
    }
}

void ZushiBonjiMSGDisp()
{
    int i;
    static int next_count = 0;
    SPRT_SDAT ssd;
    SPRT_SDAT ssd_csr;
    DISP_STR ds;
    DISP_SQAR dsq;
    SQAR_DAT menu_sq0 = {
        .w = 572,
        .h = 92,
        .x = 29,
        .y = 330,
        .pri = 0x5000,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x80,
    };

    menu_sq0.alpha = zushi_alpha[1];

    SetSprFile(EVENT_ADDRESS);

    for (i = 0; i < 11; i++)
    {
        SimpleDispSprtLNR(&spev21_sp_bak[i], EVENT_ADDRESS, i, NULL, NULL, 0x64, 0);
    }

    for (i = 0; i < 4; i++)
    {
        SimpleDispSprtLNR(&spev21_sp_zft[i], EVENT_ADDRESS, 11, NULL, NULL, 0x64, 0);
    }

    CopySqrDToSqr(&dsq, &menu_sq0);
    DispSqrD(&dsq);
    DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, zushi_alpha[1]);

    if (spev_wrk.mode == i)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV42_MSG00);
    }
    else
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV42_MSG05);
    }

    spev_str.pos_x = 0x32;
    spev_str.pos_y = 0x15e;
    spev_str.alpha = zushi_alpha[1];

    CopyStrDToStr(&ds, &spev_str);
    SetMessageV2(&ds);
}

void ZushiBonjiAfterInit(int bonji_no)
{
#ifdef BUILD_EU_VERSION
        LoadReqLanguage(EVTEX_ZUSI_E_PK2, EVENT_ADDRESS);
#else
    LoadReq(EVTEX_ZUSI_PK2, EVENT_ADDRESS);
#endif

    spev_wrk.mode = 1;
    spev_wrk.time = 40;

    SetBlackOut();

    if (bonji_no == 60)
    {
        spev_wrk.csr[1] = zushi_dat[0].answer;
    }
    else if (bonji_no == 61)
    {
        spev_wrk.csr[1] = zushi_dat[1].answer;
    }
    else if (bonji_no == 62)
    {
        spev_wrk.csr[1] = zushi_dat[2].answer;
    }
    else
    {
        spev_wrk.csr[1] = zushi_dat[3].answer;
    }

    spev_wrk.count = 0;
}

void ZushiBonjiAfterMain(int bonji_no)
{
    switch(spev_wrk.mode)
    {
    case 1:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            if (IsLoadEndAll() != 0)
            {
                SetSprFile(EVENT_ADDRESS);

                spev_wrk.mode = 2;
                spev_wrk.time = 40;

                ingame_wrk.stts |= 0x20;

                SetBlackIn();
            }
            break;
        }
    case 2:
        if (spev_wrk.time != 0x0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 4;
            spev_wrk.time = 1;
        }
    break;
    case 4:
        if (*key_now[5] == 1 || *key_now[4] == 1)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            spev_wrk.mode = 10;
            spev_wrk.time = 70;

            SetBlackOut();
        }
    break;
    case 10:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 12;
        }
    break;
    case 12:
        ingame_wrk.stts &= ~0x20;

        spev_wrk.ret = 1;

        SetBlackIn();
    break;
    }

    if (spev_wrk.mode == 4 || spev_wrk.mode == 9)
    {
        if (zushi_alpha[1] < 0x80)
        {
            zushi_alpha[1] += 6;

            if (zushi_alpha[1] >= 0x80)
            {
                zushi_alpha[1] = 0x80;
            }
        }
    }
    else
    {
        if (zushi_alpha[1] > 6)
        {
            zushi_alpha[1] -= 6;

            if (zushi_alpha[1] <= 6)
            {
                zushi_alpha[1] = 0;
            }
        }
    }

    if (spev_wrk.mode != 1)
    {
        ZushiBonjiAfterDisp(bonji_no);
    }
}

void ZushiBonjiAfterDisp(int bonji_no)
{
    int i;
    static int next_count = 0;
    SPRT_SDAT ssd;
    SPRT_SDAT ssd_csr;
    DISP_STR ds;
    DISP_SQAR dsq;
    SQAR_DAT menu_sq0  = {
        .w = 572,
        .h = 92,
        .x = 29,
        .y = 330,
        .pri = 0x5000,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x80,
    };

    menu_sq0.alpha = zushi_alpha[1];

    SetSprFile(EVENT_ADDRESS);

    for (i = 0; i < 11; i++)
    {
        SimpleDispSprtLNR(&spev21_sp_bak[i], EVENT_ADDRESS, i, NULL, NULL, 0x64, 0);
    }

    for (i = 0; i < 4; i++)
    {
        if (i == spev_wrk.csr[1])
        {
            SimpleDispSprtLNR(&spev21_sp_zbt[i], EVENT_ADDRESS, i + 13, NULL, NULL, 0x64, 0);
            SimpleDispSprtLNR(&spev21_sp_zft[i+4], EVENT_ADDRESS, 11, NULL, NULL, 0x64, 0);
        }
        else
        {
            SimpleDispSprtLNR(&spev21_sp_zft[i], EVENT_ADDRESS, 11, NULL, NULL, 0x64, 0);
        }
    }

    CopySqrDToSqr(&dsq, &menu_sq0);
    DispSqrD(&dsq);

    DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, zushi_alpha[1]);

    if (bonji_no == 60)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV60_MSG00);
    }
    else if (bonji_no == 61)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV61_MSG00);
    }
    else if (bonji_no == 62)
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV62_MSG00);
    }
    else
    {
        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV63_MSG00);
    }

    spev_str.pos_x = 50;
    spev_str.pos_y = 350;
    spev_str.alpha = zushi_alpha[1];

    CopyStrDToStr(&ds, &spev_str);

    SetMessageV2(&ds);
}

SPECIAL_EVENT_WRK spev_wrk = {0};

void KakejikuDoorInit()
{
    spev_wrk.mode = 1;
}

void KakejikuDoorMain()
{
    if (spev_wrk.mode == 1)
    {
        SetMessageAddr(GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV21_MSG12));

        if (SetMessage() == 0)
        {
            spev_wrk.ret = 1;
        }
    }
}

void IdoFirstIntoInit()
{
    spev_wrk.mode = 2;
    spev_wrk.time = 60;

    PlayerWarpReq2(1);

    spev_wrk.count = 0;

    ingame_wrk.stts |= 0x20;

    SpevStrInit();
}

void IdoFirstIntoMain()
{
    switch (spev_wrk.mode)
    {
    case 2:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 3;
            spev_wrk.time = 40;

            ingame_wrk.stts |= 0x20;
        }
    break;
    case 3:
        if (PlayerWarpCtrl() != 0)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            spev_wrk.mode = 5;

            ingame_wrk.stts &= ~0x20;

            SetBlackIn();
        }
    break;
    case 5:
        spev_wrk.ret = 1;

        ingame_wrk.stts &= ~0x20;
    break;
    }
}

void IdoIntoInit()
{
    spev_wrk.mode = 0;

    spev_wrk.csr[0] = 0;

    SpevStrInit();
}

void IdoIntoMain()
{
    switch(spev_wrk.mode)
    {
    case 0:
        if (*key_now[5] == 1)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);

            spev_wrk.mode = 2;
            spev_wrk.time = 60;

            SetBlackOut();
            PlayerWarpReq(1);

            spev_wrk.count = 0;
        }
        else if (*key_now[4] == 1)
        {
            spev_wrk.mode = 4;
            spev_wrk.time = 20;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (
            (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            ) || (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            )
        )
        {
            spev_wrk.mode = 1;
            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
    break;
    case 1:
        if (*key_now[5] == 1 || *key_now[4] == 1)
        {
            spev_wrk.mode = 4;
            spev_wrk.time = 20;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (
            (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            ) || (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            )
        )
        {
            spev_wrk.mode = 0;
            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
    break;
    case 2:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 3;
            spev_wrk.time = 40;

            ingame_wrk.stts |= 0x20;

            SetBlackIn();
        }
    break;
    case 3:
        if (PlayerWarpCtrl() != 0)
        {
            spev_wrk.time = 0;
        }
        if (spev_wrk.time == 0)
        {
            spev_wrk.mode = 5;

            ingame_wrk.stts &= ~ 0x20;

            SetBlackIn();
        }
    break;
    case 4:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 5;
        }
    break;
    case 5:
        spev_wrk.ret = 1;
    break;
    }

    if (spev_wrk.mode == 0 || spev_wrk.mode == 1)
    {
        if (spev_wrk.csr[0] < 120)
        {
            spev_wrk.csr[0] += 8;

            if (spev_wrk.csr[0] >= 120)
            {
                spev_wrk.csr[0] = 0x80;
            }
        }
    }
    else if (spev_wrk.csr[0] >= 9)
    {
        spev_wrk.csr[0] -= 8;

        if (spev_wrk.csr[0] < 9)
        {
            spev_wrk.csr[0] = 0;
        }
    }
    else
    {
        spev_wrk.csr[0] = 0;
    }

    IdoInOutDisp(1);
}

void IdoFirstOutInit()
{
    spev_wrk.mode = 2;
    spev_wrk.time = 60;

    PlayerWarpReq2(0);

    spev_wrk.count = 0;

    ingame_wrk.stts |= 0x20;

    SpevStrInit();
}

void IdoFirstOutMain()
{
    switch (spev_wrk.mode)
    {
    case 2:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 3;
            spev_wrk.time = 40;

            break;
        }
    case 3:
        if (PlayerWarpCtrl() != 0)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            spev_wrk.mode = 5;

            ingame_wrk.stts &= ~0x20;

            SetBlackIn();
        }
    break;
    case 5:
        ingame_wrk.stts &= ~0x20;

        spev_wrk.ret = 1;
    break;
    }
}

void IdoOutInit()
{
    spev_wrk.mode = 0;

    SpevStrInit();
}

void IdoOutMain()
{
    switch(spev_wrk.mode)
    {
    case 0:
        if (*key_now[5] == 1)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);

            spev_wrk.mode = 2;
            spev_wrk.time = 60;

            SetBlackOut();
            PlayerWarpReq(0);

            spev_wrk.count = 0;
        }
        else if (*key_now[4] == 1)
        {
            spev_wrk.mode = 4;
            spev_wrk.time = 20;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (
            (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            ) || (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            )
        )
        {
            spev_wrk.mode = 1;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
    break;
    case 1:
        if (*key_now[5] == 1 || *key_now[4] == 1)
        {
            spev_wrk.mode = 4;
            spev_wrk.time = 20;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (
            (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            ) || (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            )
        )
        {
            spev_wrk.mode = 0;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
    break;
    case 2:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 3;
            spev_wrk.time = 40;

            ingame_wrk.stts |= 0x20;

            break;
        }
    case 3:
        if (PlayerWarpCtrl() != 0)
        {
            spev_wrk.time = 0;
        }

        if (spev_wrk.time == 0)
        {
            spev_wrk.mode = 5;

            ingame_wrk.stts &= ~0x20;

            SetBlackIn();
        }
    break;
    case 4:
        if (spev_wrk.time != 0)
        {
            spev_wrk.time--;
        }
        else
        {
            spev_wrk.mode = 5;
        }
    break;
    case 5:
        spev_wrk.ret = 1;
    break;
    }

    if (spev_wrk.mode == 0 || spev_wrk.mode == 1)
    {
        if (spev_wrk.csr[0] < 120)
        {
            spev_wrk.csr[0] += 8;

            if (spev_wrk.csr[0] >= 120)
            {
                spev_wrk.csr[0] = 0x80;
            }
        }
    }
    else if (spev_wrk.csr[0] >= 9)
    {
        spev_wrk.csr[0] -= 8;

        if (spev_wrk.csr[0] < 9)
        {
            spev_wrk.csr[0] = 0;
        }
    }
    else
    {
        spev_wrk.csr[0] = 0;
    }

    IdoInOutDisp(0);
}

void IdoInOutDisp(int inout)
{
    DISP_STR ds;

    if (spev_wrk.mode == 0 || spev_wrk.mode== 1 || spev_wrk.mode == 2 || spev_wrk.mode == 4)
    {
        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, spev_wrk.csr[0]);

        if (inout == 1)
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV58_MSG00);
            spev_str.pos_x = 195;
            spev_str.pos_y = 345;
            spev_str.alpha = spev_wrk.csr[0];

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }
        else
        {
            spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV59_MSG00);
            spev_str.pos_x = 320 - GetStrWidth(spev_str.str) / 2;
            spev_str.pos_y = 345;
            spev_str.alpha = spev_wrk.csr[0];

            CopyStrDToStr(&ds, &spev_str);
            SetMessageV2(&ds);
        }

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_YES);
        spev_str.pos_x = 215;
        spev_str.pos_y = 373;
        spev_str.alpha = spev_wrk.csr[0];

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_NO);
        spev_str.pos_x = 315;
        spev_str.pos_y = 373;
        spev_str.alpha = spev_wrk.csr[0];

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (spev_wrk.mode == 0)
        {
            YesNoCrslOKR(0x2000, 192.0f, 370.0f, 0x808080, 100.0f, 1.5f);
        }
        else if (spev_wrk.mode == 1)
        {
            YesNoCrslOKR(0x2000, 290.0f, 370.0f, 0x808080, 100.0f, 1.5f);
        }
    }
}

void ItemEventInit(int event_no)
{
    switch(event_no)
    {
    case 65:
        spev_wrk.csr[0] = camera_table[1];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 107;
        spev_wrk.csr[2] = 108;
#else
        spev_wrk.csr[1] = 103;
        spev_wrk.csr[2] = 104;
#endif
    break;
    case 66:
        spev_wrk.csr[0] = camera_table[2];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 109;
        spev_wrk.csr[2] = 110;
#else
        spev_wrk.csr[1] = 105;
        spev_wrk.csr[2] = 106;
#endif
    break;
    case 67:
        spev_wrk.csr[0] = camera_table[3];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 111;
        spev_wrk.csr[2] = 112;
#else
        spev_wrk.csr[1] = 107;
        spev_wrk.csr[2] = 108;
#endif
    break;
    case 68:
        spev_wrk.csr[0] = camera_table[4];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 113;
        spev_wrk.csr[2] = 114;
#else
        spev_wrk.csr[1] = 109;
        spev_wrk.csr[2] = 110;
#endif
    break;
    case 69:
        spev_wrk.csr[0] = camera_table[5];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 115;
        spev_wrk.csr[2] = 116;
#else
        spev_wrk.csr[1] = 111;
        spev_wrk.csr[2] = 112;
#endif
    break;
    case 70:
        spev_wrk.csr[0] = camera_table[6];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 117;
        spev_wrk.csr[2] = 118;
#else
        spev_wrk.csr[1] = 113;
        spev_wrk.csr[2] = 114;
#endif
    break;
    case 71:
        spev_wrk.csr[0] = camera_table[7];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 119;
        spev_wrk.csr[2] = 120;
#else
        spev_wrk.csr[1] = 115;
        spev_wrk.csr[2] = 116;
#endif
    break;
    case 72:
        spev_wrk.csr[0] = camera_table[8];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 121;
        spev_wrk.csr[2] = 122;
#else
        spev_wrk.csr[1] = 117;
        spev_wrk.csr[2] = 118;
#endif
    break;
    case 73:
        spev_wrk.csr[0] = camera_table[9];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 123;
        spev_wrk.csr[2] = 124;
#else
        spev_wrk.csr[1] = 119;
        spev_wrk.csr[2] = 120;
#endif
    break;
    case 74:
        spev_wrk.csr[0] = camera_table[10];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 125;
        spev_wrk.csr[2] = 126;
#else
        spev_wrk.csr[1] = 121;
        spev_wrk.csr[2] = 122;
#endif
    break;
    case 75:
        spev_wrk.csr[0] = camera_table[11];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 127;
        spev_wrk.csr[2] = 128;
#else
        spev_wrk.csr[1] = 123;
        spev_wrk.csr[2] = 124;
#endif
    break;
    case 76:
        spev_wrk.csr[0] = camera_table[12];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 129;
        spev_wrk.csr[2] = 130;
#else
        spev_wrk.csr[1] = 125;
        spev_wrk.csr[2] = 126;
#endif
    break;
    case 77:
        spev_wrk.csr[0] = camera_table[13];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 131;
        spev_wrk.csr[2] = 132;
#else
        spev_wrk.csr[1] = 127;
        spev_wrk.csr[2] = 128;
#endif
    break;
    case 78:
        spev_wrk.csr[0] = camera_table[13];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 133;
        spev_wrk.csr[2] = 134;
#else
        spev_wrk.csr[1] = 129;
        spev_wrk.csr[2] = 130;
#endif
    break;
    case 82:
        spev_wrk.csr[0] = camera_table[15];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 123;
        spev_wrk.csr[2] = 124;
#else
        spev_wrk.csr[1] = 119;
        spev_wrk.csr[2] = 120;
#endif
    break;
    case 83:
        spev_wrk.csr[0] = camera_table[16];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 125;
        spev_wrk.csr[2] = 126;
#else
        spev_wrk.csr[1] = 121;
        spev_wrk.csr[2] = 122;
#endif
    break;
    case 84:
        spev_wrk.csr[0] = camera_table[17];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 127;
        spev_wrk.csr[2] = 128;
#else
        spev_wrk.csr[1] = 123;
        spev_wrk.csr[2] = 124;
#endif
    break;
    case 85:
        spev_wrk.csr[0] = camera_table[18];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 129;
        spev_wrk.csr[2] = 130;
#else
        spev_wrk.csr[1] = 125;
        spev_wrk.csr[2] = 126;
#endif
    break;
    case 64:
    default:
        spev_wrk.csr[0] = camera_table[0];
#ifdef BUILD_EU_VERSION
        spev_wrk.csr[1] = 105;
        spev_wrk.csr[2] = 106;
#else
        spev_wrk.csr[1] = 101;
        spev_wrk.csr[2] = 102;
#endif
    break;
    }

    if (event_no == 78)
    {
        pzl_load_id = SeFileLoadAndSet(ST001_MEN_KAKE_BD, 21);
    }
    else
    {
        pzl_load_id = SeFileLoadAndSet(ST011_NAWA_KAKE_BD, 21);
    }

    spev_wrk.mode = 0;

    SpevStrInit();

    spev_wrk.count = 0;
}

void ItemEventMain(int event_no)
{
    ItemEventDisp();

    switch(spev_wrk.mode)
    {
    case 0:
        if (IsLoadEnd(pzl_load_id) != 0)
        {
            pzl_load_id = -1;

            spev_wrk.mode = 2;
        }
    break;
    case 2:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            spev_wrk.mode = 5;
        }
    break;
    case 5:
        if (*key_now[5] == 1)
        {
            spev_wrk.mode = 8;

            switch(event_no)
            {
            case 73:
                SeStartFix(93, 0, 0x1000, 0x1000, 0);
            break;
            case 74:
                SeStartFix(93, 0, 0x1000, 0x1000, 0);
            break;
            case 75:
                SeStartFix(93, 0, 0x1000, 0x1000, 0);
            break;
            case 76:
                SeStartFix(93, 0, 0x1000, 0x1000, 0);
            break;
            case 78:
                spev_wrk.mode = 7;
                spev_wrk.count = 40;

                SetBlackOut();
            break;
            default:
                SeStartFix(1, 0, 0x1000, 0x1000, 0);
            break;
            }
        }
        else if (*key_now[4] == 1)
        {
            spev_wrk.mode = 9;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (
            (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            ) || (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            )
        )
        {
            spev_wrk.mode = 6;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
    break;
    case 6:
        if (*key_now[5] == 1)
        {
            spev_wrk.mode = 9;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (*key_now[4] == 1)
        {
            spev_wrk.mode = 9;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (
            (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            ) || (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            )
        )
        {
            spev_wrk.mode = 5;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
    break;
    case 7:
        if (spev_wrk.count != 0)
        {
            spev_wrk.count--;
        }
        else
        {
            if ((ev_wrk.face_stts[1] & 0x1) == 0)
            {

                ev_wrk.face_stts[1] &= 0x80 | 0x40 | 0x20 | 0x1;

                ev_wrk.face_stts[2] |= 0x4;
                ev_wrk.face_stts[2] |= 0x1;

                door_keep[47].attr |= 0x2;
                door_keep[26].attr &= ~0x2;
                door_keep[34].attr |= 0x2;
                door_keep[49].attr &= ~0x2;

                ev_wrk.face_stts[1] |= 0x1;
            }

            door_keep[26].attr |= 0x2;

            poss_item[42] = 0;

            DoorChangeModel(26, dmodel_table[1][1]);

            if (DoorKeyUnlockOnly(26) != 0)
            {
                DoorPassSetHide(47);
                DoorPassSetHide(26);
                DoorPassSetHide(34);
                DoorPassSetHide(49);
                DoorPassSetHide(29);
                DoorOpenShiftForce(26);
            }

            SetBlackIn();
            SeStartFix(93, 0, 0x1000, 0x1000, 0);

            spev_wrk.mode = 8;
        }
    break;
    case 8:
        if (spev_wrk.time < 11)
        {
            spev_wrk.mode = 10;
        }
    break;
    case 9:
        if (spev_wrk.time < 11)
        {
            spev_wrk.mode = 11;
        }
    break;
    case 10:
        event_stts[ev_wrk.evt_no] = 1;

        spev_wrk.ret = 1;

        spev_str.alpha = 0x80;

        ButtonMarkTimeClear();
    break;
    case 11:
        spev_wrk.ret = 1;

        spev_str.alpha = 0x80;

        ButtonMarkTimeClear();
    break;
    }

    if (spev_wrk.mode == 2 || spev_wrk.mode == 5 || spev_wrk.mode == 6)
    {
        if (spev_wrk.time < 112)
        {
            spev_wrk.time += 8;

            if (spev_wrk.time >= 112)
            {
                spev_wrk.time = 120;
            }
        }
    }
    else
    {
        if (spev_wrk.time > 18)
        {
            spev_wrk.time  -= 8;

            if (spev_wrk.time <= 18)
            {
                spev_wrk.time = 10;
            }
        }
        else
        {
            spev_wrk.time = 10;
        }
    }
}

void ItemEventDisp()
{
    DISP_STR ds;
#ifdef BUILD_EU_VERSION
    int yy;
#endif

    if (spev_wrk.mode == 2)
    {
        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, spev_wrk.time);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, spev_wrk.csr[1]);
        spev_str.pos_x = 50;
#ifdef BUILD_EU_VERSION

        switch(spev_wrk.csr[1])
        {
        case 115:
        case 121:
        case 123:
        case 125:
        case 127:
        case 129:
            spev_str.pos_y = 340;
        break;
        default:
            spev_str.pos_y = 350;
        break;
        }

#else
        spev_str.pos_y = 350;
#endif
        spev_str.alpha = spev_wrk.time;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }
    else if (spev_wrk.mode == 5 || spev_wrk.mode == 6 || spev_wrk.mode == 8)
    {
#ifdef BUILD_EU_VERSION
            switch(spev_wrk.csr[2])
            {
            case 106:
            case 108:
            case 110:
            case 112:
            case 114:
            case 116:
            case 118:
            case 120:
            case 122:
            case 124:
            case 126:
            case 128:
            case 130:
            case 132:
            case 134:
                yy = 340;
            break;
            default:
                yy = 350;
            break;
            }
#endif

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, spev_wrk.time);
        ButtonMarkNext(0, 0, 0);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, spev_wrk.csr[2]);
        spev_str.pos_x = 50;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = yy;
#else
        spev_str.pos_y = 350;
#endif
        spev_str.alpha = spev_wrk.time;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_YES);
        spev_str.pos_x = 100;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = yy + 48;
#else
        spev_str.pos_y = 380;
#endif
        spev_str.alpha = spev_wrk.time;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_NO);
        spev_str.pos_x = 200;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = yy + 48;
#else
        spev_str.pos_y = 380;
#endif
        spev_str.alpha = spev_wrk.time;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (spev_wrk.mode == 5)
        {
#ifdef BUILD_EU_VERSION
            YesNoCrslOKR(0x2000, 77.0f, 386.0f, 0x808080, 100.0f, 1.5f);
#else
            YesNoCrslOKR(0x2000, 77.0f, 377.0f, 0x808080, 100.0f, 1.5f);
#endif
        }
        else if (spev_wrk.mode == 6)
        {
#ifdef BUILD_EU_VERSION
            YesNoCrslOKR(0x2000, 170.0f, 386.0f, 0x808080, 100.0f, 1.5f);
#else
            YesNoCrslOKR(0x2000, 170.0f, 377.0f, 0x808080, 100.0f, 1.5f);
#endif
        }
    }
}

void NawakakeFalseMain(int event_no)
{
    NawakakeFalseDisp();

    switch(spev_wrk.mode)
    {
    case 0:
        if (IsLoadEndAll() != 0)
        {
            spev_wrk.mode = 2;

            break;
        }
    case 2:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            ButtonMarkTimeClear();

            spev_wrk.mode = 5;
        }
    break;
    case 5:
        if (*key_now[5] == 1)
        {
            spev_wrk.mode = 3;

            switch (event_no)
            {
            case 82:
                SeStartFix(93, 0, 0x1000, 0x1000, 0);
            break;
            case 83:
                SeStartFix(93, 0, 0x1000, 0x1000, 0);
            break;
            case 84:
                SeStartFix(93, 0, 0x1000, 0x1000, 0);
            break;
            case 85:
                SeStartFix(93, 0, 0x1000, 0x1000, 0);
            break;
            }
        }
        else if (*key_now[4] == 1)
        {
            spev_wrk.mode = 9;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (
            (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            ) || (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            )
        )
        {
            spev_wrk.mode = 6;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
    break;
    case 6:
        if (*key_now[5] == 1)
        {
            spev_wrk.mode = 9;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (*key_now[4] == 1)
        {
            spev_wrk.mode = 9;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (
            (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            ) || (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            )
        )
        {
            spev_wrk.mode = 5;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }
    break;
    case 3:
        if (ButtonMarkNext(0, 0, 1) != 0)
        {
            ButtonMarkTimeClear();

            spev_wrk.mode = 4;
        }
    break;
    case 4:
        if (ButtonMarkNext(0, 0, 0) != 0)
        {
            ButtonMarkTimeClear();

            spev_wrk.mode = 8;
        }
    break;
    case 8:
        if (spev_wrk.time <= 10)
        {
            spev_wrk.mode = 10;
        }
    break;
    case 9:
        if (spev_wrk.time < 11)
        {
            spev_wrk.mode = 11;
        }
    break;
    case 10:
        spev_wrk.ret = 1;

        spev_str.alpha = 0x80;

        ButtonMarkTimeClear();
    break;
    case 11:
        spev_wrk.ret = 1;

        spev_str.alpha = 0x80;

        ButtonMarkTimeClear();
    break;
    }

    if (spev_wrk.mode == 2 || spev_wrk.mode == 3 || spev_wrk.mode == 4 || spev_wrk.mode == 5 || spev_wrk.mode == 6)
    {
        if (spev_wrk.time < 112)
        {
            spev_wrk.time += 8;

            if (spev_wrk.time >= 112)
            {
                spev_wrk.time = 120;
            }
        }
    }
    else
    {
        if (spev_wrk.time > 18)
        {
            spev_wrk.time  -= 8;

            if (spev_wrk.time <= 18)
            {
                spev_wrk.time = 10;
            }
        }
        else
        {
            spev_wrk.time = 10;
        }
    }
}

void NawakakeFalseDisp()
{
    DISP_STR ds;
#ifdef BUILD_EU_VERSION
    int yy;
#endif

    if (spev_wrk.mode == 2)
    {
        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, spev_wrk.time);

        spev_str.str = (u_char *)GetIngameMSGAddr(0x7, spev_wrk.csr[1]);
        spev_str.pos_x = 50;
#ifdef BUILD_EU_VERSION

        switch (spev_wrk.csr[1])
        {
        case 123:
        case 125:
        case 127:
        case 129:
            spev_str.pos_y = 340;
        break;
        default:
            spev_str.pos_y = 350;
        break;
        }

#else
        spev_str.pos_y = 350;
#endif
        spev_str.alpha = spev_wrk.time;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }
    else if (spev_wrk.mode == 5 || spev_wrk.mode == 6)
    {
#ifdef BUILD_EU_VERSION
        switch (spev_wrk.csr[2])
        {
        case 124:
        case 126:
        case 128:
        case 130:
            yy = 340;
        break;
        default:
            yy = 350;
        break;
        }
#endif

        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, spev_wrk.time);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, spev_wrk.csr[2]);
        spev_str.pos_x = 50;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = yy;
#else
        spev_str.pos_y = 350;
#endif
        spev_str.alpha = spev_wrk.time;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_YES);
        spev_str.pos_x = 100;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = yy + 48;
#else
        spev_str.pos_y = 380;
#endif
        spev_str.alpha = spev_wrk.time;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV_MSG_NO);
        spev_str.pos_x = 200;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = yy + 48;
#else
        spev_str.pos_y = 380;
#endif
        spev_str.alpha = spev_wrk.time;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);

        if (spev_wrk.mode == 5)
        {
#ifdef BUILD_EU_VERSION
            YesNoCrslOKR(0x2000, 77.0f, 386.0f, 0x808080, 100.0f, 1.5f);
#else
            YesNoCrslOKR(0x2000, 77.0f, 377.0f, 0x808080, 100.0f, 1.5f);
#endif
        }
        else if (spev_wrk.mode == 6)
        {
#ifdef BUILD_EU_VERSION
            YesNoCrslOKR(0x2000, 170.0f, 386.0f, 0x808080, 100.0f, 1.5f);
#else
            YesNoCrslOKR(0x2000, 170.0f, 377.0f, 0x808080, 100.0f, 1.5f);
#endif
        }
    }

    if (spev_wrk.mode == 3)
    {
        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, spev_wrk.time);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV03_MSG07);
        spev_str.pos_x = 50;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = 340;
#else
        spev_str.pos_y = 350;
#endif
        spev_str.alpha = spev_wrk.time;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }

    if (spev_wrk.mode == 4 || spev_wrk.mode == 8)
    {
        DrawMessageBox(0x2000, 19.0f, 328.0f, 592.0f, 96.0f, spev_wrk.time);

        spev_str.str = (u_char *)GetIngameMSGAddr(IGMSG_SPEV_MSG, SPEV03_MSG08);
        spev_str.pos_x = 50;
#ifdef BUILD_EU_VERSION
        spev_str.pos_y = 340;
#else
        spev_str.pos_y = 350;
#endif
        spev_str.alpha = spev_wrk.time;

        CopyStrDToStr(&ds, &spev_str);
        SetMessageV2(&ds);
    }
}

void SpecialEventInit000()
{
    DialKeyDoorInit(0);
}

void SpecialEventMain000()
{
    DialKeyDoorMain();
}

void SpecialEventInit001()
{
    DialKeyDoorInit(1);
}

void SpecialEventMain001()
{
    DialKeyDoorMain();
}

void SpecialEventInit002()
{
    GhostDoorInit(0);
}

void SpecialEventMain002()
{
    GhostDoorMain(0);
}

void SpecialEventInit003()
{
    GhostDoorSet(0);

    event_stts[ev_wrk.evt_no] = 1;

    ingame_wrk.mode = INGAME_MODE_NOMAL;

    ev_wrk.evt_no = 0xff;
    ev_wrk.mode = 0;
}

void SpecialEventMain003()
{
    return;
}

void SpecialEventInit004()
{
    StarPuzzleInit(4);
}

void SpecialEventMain004()
{
    StarPuzzleMSGMain(4);
}

void SpecialEventInit005()
{
    GhostDoorInit(1);
}

void SpecialEventMain005()
{
    GhostDoorMain(1);
}

void SpecialEventInit006()
{
    GhostDoorSet(1);

    event_stts[ev_wrk.evt_no] = 1;

    ingame_wrk.mode = INGAME_MODE_NOMAL;

    ev_wrk.evt_no = 0xff;
    ev_wrk.mode = 0;
}

void SpecialEventMain006()
{
    return;
}

void SpecialEventInit007()
{
    GhostDoorInit(2);
}

void SpecialEventMain007()
{
    GhostDoorMain(2);
}

void SpecialEventInit008()
{
    GhostDoorSet(2);

    event_stts[ev_wrk.evt_no] = 1;

    ingame_wrk.mode = INGAME_MODE_NOMAL;

    ev_wrk.evt_no = 0xff;
    ev_wrk.mode = 0;
}

void SpecialEventMain008()
{
    return;
}

void SpecialEventInit009()
{
    StarPuzzleInit(9);
}

void SpecialEventMain009()
{
    StarPuzzleMSGMain(9);
}

void SpecialEventInit010()
{
    DialKeyDoorInit(2);
}

void SpecialEventMain010()
{
    DialKeyDoorMain();
}

void SpecialEventInit011()
{
    StarPuzzleInit(12);
}

void SpecialEventMain011()
{
    StarPuzzleMSGMain(12);
}

void SpecialEventInit012()
{
    StarPuzzleInit(12);
}

void SpecialEventMain012()
{
    StarPuzzleMain(12);
}

void SpecialEventInit013()
{
    ButsuzoPzlInit();
}

void SpecialEventMain013()
{
    ButsuzoPzlMain();
}

void SpecialEventInit014()
{
    GhostDoorInit(3);
}

void SpecialEventMain014()
{
    GhostDoorMain(3);
}

void SpecialEventInit015()
{
    GhostDoorSet(3);

    event_stts[ev_wrk.evt_no] = 1;

    ingame_wrk.mode = INGAME_MODE_NOMAL;

    ev_wrk.evt_no = 0xff;
    ev_wrk.mode = 0;
}

void SpecialEventMain015()
{
    return;
}

void SpecialEventInit016()
{
    DialKeyDoorInit(3);
}


void SpecialEventMain016()
{
    DialKeyDoorMain();
}

void SpecialEventInit017()
{
    StarPuzzleInit(18);
}

void SpecialEventMain017()
{
    StarPuzzleMSGMain(18);
}

void SpecialEventInit018()
{
    StarPuzzleInit(18);
}


void SpecialEventMain018()
{
    StarPuzzleMain(18);
}

void SpecialEventInit019()
{
    StarPuzzleInit(20);
}

void SpecialEventMain019()
{
    StarPuzzleMSGMain(20);
}


void SpecialEventInit020()
{
    StarPuzzleInit(20);
}

void SpecialEventMain020()
{
    StarPuzzleMain(20);
}

void SpecialEventInit021()
{
    LightsOutInit();
}

void SpecialEventMain021()
{
    LightsOutMain();
}

void SpecialEventInit022()
{
    GhostDoorInit(4);
}

void SpecialEventMain022()
{
    GhostDoorMain(4);
}

void SpecialEventInit023()
{
    GhostDoorSet(4);

    event_stts[ev_wrk.evt_no] = 1;

    ingame_wrk.mode = INGAME_MODE_NOMAL;

    ev_wrk.evt_no = 0xff;
    ev_wrk.mode = 0;
}

void SpecialEventMain023()
{
    return;
}

void SpecialEventInit024()
{
    DialKeyDoorInit(4);
}

void SpecialEventMain024()
{
    DialKeyDoorMain();
}

void SpecialEventInit025()
{
    GhostDoorInit(5);
}

void SpecialEventMain025()
{
    GhostDoorMain(5);
}

void SpecialEventInit026()
{
    GhostDoorSet(5);

    event_stts[ev_wrk.evt_no] = 1;

    ingame_wrk.mode = INGAME_MODE_NOMAL;

    ev_wrk.evt_no = 0xff;
    ev_wrk.mode = 0;
}

void SpecialEventMain026()
{
    return;
}

void SpecialEventInit027()
{
    DialKeyDoorInit(5);
}

void SpecialEventMain027()
{
    DialKeyDoorMain();
}

void SpecialEventInit028()
{
    DollPzlInit();
}

void SpecialEventMain028()
{
    DollPzlMain();
}

void SpecialEventInit029()
{
    FaceDoorInit(0);
}

void SpecialEventMain029()
{
    FaceDoorMain(0);
}

void SpecialEventInit030()
{
    FaceDoorInit(1);
}

void SpecialEventMain030()
{
    FaceDoorMain(1);
}

void SpecialEventInit031()
{
    FaceDoorInit(2);
}

void SpecialEventMain031()
{
    FaceDoorMain(2);
}

void SpecialEventInit032()
{
    FaceDoorInit(3);
}

void SpecialEventMain032()
{
    FaceDoorMain(3);
}

void SpecialEventInit033()
{
    SurpriseDoorInit();
}

void SpecialEventMain033()
{
    SurpriseDoorMain();
}

void SpecialEventInit034()
{
    SimenPillarInit(34);
}

void SpecialEventMain034()
{
    SimenPillarMain(34);
}

void SpecialEventInit035()
{
    StarPuzzleInit(36);
}

void SpecialEventMain035()
{
    StarPuzzleMSGMain(36);
}

void SpecialEventInit036()
{
    StarPuzzleInit(36);
}

void SpecialEventMain036()
{
    StarPuzzleMain(36);
}

void SpecialEventInit037()
{
    GhostDoorInit(6);
}

void SpecialEventMain037()
{
    GhostDoorMain(6);
}

void SpecialEventInit038()
{
    GhostDoorSet(6);

    event_stts[ev_wrk.evt_no] = 1;

    ingame_wrk.mode = INGAME_MODE_NOMAL;

    ev_wrk.evt_no = 0xff;
    ev_wrk.mode = 0;
}

void SpecialEventMain038()
{
    return;
}

void SpecialEventInit039()
{
    ButsuzoMSGInit();
}

void SpecialEventMain039()
{
    ButsuzoMSGMain();
}

void SpecialEventInit040()
{
    StarPuzzleInit(41);
}

void SpecialEventMain040()
{
    StarPuzzleMSGMain(41);
}

void SpecialEventInit041()
{
    StarPuzzleInit(41);
}

void SpecialEventMain041()
{
    StarPuzzleMain(41);
}

void SpecialEventInit042()
{
    ZushiBonjiInit(42);
}

void SpecialEventMain042()
{
    ZushiBonjiMain(42);
}

void SpecialEventInit043()
{
    ZushiBonjiInit(43);
}

void SpecialEventMain043()
{
    ZushiBonjiMain(43);
}

void SpecialEventInit044()
{
    ZushiBonjiInit(44);
}

void SpecialEventMain044()
{
    ZushiBonjiMain(44);
}

void SpecialEventInit045()
{
    ZushiBonjiInit(45);
}

void SpecialEventMain045()
{
    ZushiBonjiMain(45);
}

void SpecialEventInit046()
{
    DialKeyDoorInit(6);
}

void SpecialEventMain046()
{
    DialKeyDoorMain();
}

void SpecialEventInit047()
{
    DialKeyDoorInit(7);
}

void SpecialEventMain047()
{
    DialKeyDoorMain();
}

void SpecialEventInit048()
{
    DialKeyDoorInit(8);
}

void SpecialEventMain048()
{
    DialKeyDoorMain();
}

void SpecialEventInit049()
{
    KakejikuDoorInit();
}

void SpecialEventMain049()
{
    KakejikuDoorMain();
}

void SpecialEventInit050()
{
    DialKeyDoorInit(9);
}

void SpecialEventMain050()
{
    DialKeyDoorMain();
}

void SpecialEventInit051()
{
    DialKeyDoorInit(10);
}

void SpecialEventMain051()
{
    DialKeyDoorMain();
}

void SpecialEventInit052()
{
    DialKeyDoorInit(11);
}

void SpecialEventMain052()
{
    DialKeyDoorMain();
}

void SpecialEventInit053()
{
    DialKeyDoorInit(12);
}

void SpecialEventMain053()
{
    DialKeyDoorMain();
}

void SpecialEventInit054()
{
    SimenPillarInit(54);
}

void SpecialEventMain054()
{
    SimenPillarMain(54);
}

void SpecialEventInit055()
{
    SimenPillarInit(55);
}

void SpecialEventMain055()
{
    SimenPillarMain(55);
}

void SpecialEventInit056()
{
    SimenPillarInit(56);
}

void SpecialEventMain056()
{
    SimenPillarMain(56);
}

void SpecialEventInit057()
{
    ZushiBonjiMSGInit();
}

void SpecialEventMain057()
{
    ZushiBonjiMSGMain();
}

void SpecialEventInit058()
{
    IdoFirstIntoInit();
}

void SpecialEventMain058()
{
    IdoFirstIntoMain();
}

void SpecialEventInit059()
{
    IdoFirstOutInit();
}

void SpecialEventMain059()
{
    IdoFirstOutMain();
}

void SpecialEventInit060()
{
    ZushiBonjiAfterInit(60);
}

void SpecialEventMain060()
{
    ZushiBonjiAfterMain(60);
}

void SpecialEventInit061()
{
    ZushiBonjiAfterInit(61);
}

void SpecialEventMain061()
{
    ZushiBonjiAfterMain(61);
}

void SpecialEventInit062()
{
    ZushiBonjiAfterInit(62);
}

void SpecialEventMain062()
{
    ZushiBonjiAfterMain(62);
}

void SpecialEventInit063()
{
    ZushiBonjiAfterInit(63);
}

void SpecialEventMain063()
{
    ZushiBonjiAfterMain(63);
}

void SpecialEventInit064()
{
    ItemEventInit(64);
}

void SpecialEventMain064()
{
    ItemEventMain(64);
}

void SpecialEventInit065()
{
    ItemEventInit(65);
}

void SpecialEventMain065()
{
    ItemEventMain(65);
}

void SpecialEventInit066()
{
    ItemEventInit(66);
}

void SpecialEventMain066()
{
    ItemEventMain(66);
}

void SpecialEventInit067()
{
    ItemEventInit(67);
}

void SpecialEventMain067()
{
    ItemEventMain(67);
}

void SpecialEventInit068()
{
    ItemEventInit(68);
}

void SpecialEventMain068()
{
    ItemEventMain(68);
}

void SpecialEventInit069()
{
    ItemEventInit(69);
}

void SpecialEventMain069()
{
    ItemEventMain(69);
}

void SpecialEventInit070()
{
    ItemEventInit(70);
}

void SpecialEventMain070()
{
    ItemEventMain(70);
}

void SpecialEventInit071()
{
    ItemEventInit(71);
}

void SpecialEventMain071()
{
    ItemEventMain(71);
}

void SpecialEventInit072()
{
    ItemEventInit(72);
}

void SpecialEventMain072()
{
    ItemEventMain(72);
}

void SpecialEventInit073()
{
    ItemEventInit(73);
}

void SpecialEventMain073()
{
    ItemEventMain(73);
}

void SpecialEventInit074()
{
    ItemEventInit(74);
}

void SpecialEventMain074()
{
    ItemEventMain(74);
}

void SpecialEventInit075()
{
    ItemEventInit(75);
}

void SpecialEventMain075()
{
    ItemEventMain(75);
}

void SpecialEventInit076()
{
    ItemEventInit(76);
}

void SpecialEventMain076()
{
    ItemEventMain(76);
}

void SpecialEventInit077()
{
    ItemEventInit(77);
}

void SpecialEventMain077()
{
    ItemEventMain(77);
}

void SpecialEventInit078()
{
    ItemEventInit(78);
}

void SpecialEventMain078()
{
    ItemEventMain(78);
}

void SpecialEventInit079()
{
    IdoIntoInit();
}

void SpecialEventMain079()
{
    IdoIntoMain();
}

void SpecialEventInit080()
{
    IdoOutInit();
}

void SpecialEventMain080()
{
    IdoOutMain();
}

void SpecialEventInit081()
{
    IkariMenInit();
}

void SpecialEventMain081()
{
    IkariMenComeOn();
}

void SpecialEventInit082()
{
    ItemEventInit(82);
}

void SpecialEventMain082()
{
    NawakakeFalseMain(82);
}

void SpecialEventInit083()
{
    ItemEventInit(83);
}

void SpecialEventMain083()
{
    NawakakeFalseMain(83);
}

void SpecialEventInit084()
{
    ItemEventInit(84);
}

void SpecialEventMain084()
{
    NawakakeFalseMain(84);
}

void SpecialEventInit085()
{
    ItemEventInit(85);
}

void SpecialEventMain085()
{
    NawakakeFalseMain(85);
}

void SpecialEventInit086()
{
    HanyouKaitenInit(86);
}

void SpecialEventMain086()
{
    HanyouKaitenMain(86);
}

void SpecialEventInit087()
{
    HanyouKaitenInit(87);
}

void SpecialEventMain087()
{
    HanyouKaitenMain(87);
}

void SpecialEventInit088()
{
    HanyouKaitenInit(88);
}

void SpecialEventMain088()
{
    HanyouKaitenMain(88);
}

void SpecialEventInit089()
{
    HanyouKaitenInit(89);
}

void SpecialEventMain089()
{
    HanyouKaitenMain(89);
}

void SpecialEventInit090()
{
    HanyouKaitenInit(90);
}

void SpecialEventMain090()
{
    HanyouKaitenMain(90);
}
