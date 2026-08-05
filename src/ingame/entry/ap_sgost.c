#include "common.h"
#include "typedefs.h"
#include "addresses.h"
#include "enums.h"
#include "ap_sgost.h"

// #include "graphics/graph2d/effect_ene.h" //
#include "graphics/graph2d/effect_scr.h"
// #include "graphics/motion/mdlwork.h" //
// #include "graphics/motion/motion.h" //
#include "ingame/enemy/ene_ctl.h"
#include "ingame/entry/ap_dgost.h"
#include "ingame/entry/ap_ggost.h"
#include "ingame/entry/entry.h"
#include "ingame/plyr/plyr_ctl.h"
#include "main/glob.h"
#include "os/eeiop/adpcm/ea_shinkan.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"

#include "data/sgef_dat.h" // SGEF_DAT sgef_dat[];
#include "data/sgef_adpcm_dat.h" // u_short sgef_adpcm_dat[];
SgCAMERA sg_cmr_bak = {0};
SGST_WRK sgst_wrk = {0};

void SettleGhostInit()
{
    sgst_wrk = (SGST_WRK){0};
}

int SettleGhostAppearReq(int sg_no)
{
    sgst_wrk.sg_no = sg_no;
    sgst_wrk.dat_no = ingame_wrk.msn_no - 3;
    sgst_wrk.mode = SGST_MODE_AP_IN;
    ingame_wrk.mode = INGAME_MODE_SGST_DISP;

    ReqPlayerStop(1);

    // missing return
}

void SettleGhostMain()
{
    int end;
    static int sg_load_id;

    switch(sgst_wrk.mode)
    {
    case SGST_MODE_AP_IN:
        sgst_wrk.mode = SGST_MODE_AP_LOAD;
        sgst_wrk.load_mode = SGLOAD_MODE_REQ;
        sgst_wrk.disp_mode = SGDISP_MODE_REQ;
    break;
    case SGST_MODE_AP_LOAD:
        end = 1;

        if (sgst_wrk.sg_no == 5)
        {
            if (SettleGhostLoadTwoSize() == 0)
            {
                end = 0;
            }
        }
        else
        {
            if (SettleGhostLoadOneSize() == 0)
            {
                end = 0;
            }
        }

        if (SettleGhostStartDisp() == 0)
        {
            end = 0;
        }

        if (end)
        {
            sgst_wrk.mode = SGST_MODE_BATTLE;
            ingame_wrk.mode = INGAME_MODE_NOMAL;

            SettleGhostBattleSet();

            return;
        }
    break;
    case SGST_MODE_DEAD_IN:
        sgst_wrk.mode = SGST_MODE_MOT_REL;
    // case fall-through
    case SGST_MODE_MOT_REL:
        if (sgst_wrk.sg_no == 5)
        {
            DeadGhostLoadReq();

            sg_load_id = SeFileLoadAndSet(SGY000_BD, 1);

            sgst_wrk.mode = SGST_MODE_BHSE_WAIT;
        }
        else
        {
            if (GuardGhostReloadReq() != 0)
            {
                motReleaseAniMdlBuf(jene_dat[ingame_wrk.msn_no][sgst_wrk.sg_no].anm_no, (u_int *)LOAD_ADDRESS_09);

                sgst_wrk.mode = SGST_MODE_DD_LOAD;
            }
            else
            {
                sgst_wrk.mode = SGST_MODE_END;
            }
        }
    break;
    case SGST_MODE_BHSE_WAIT:
        if (IsLoadEnd(sg_load_id) == 0)
        {
            return;
        }

        sgst_wrk.mode = SGST_MODE_DD_LOAD;
    // case fall-through
    case 7:
        if (sgst_wrk.sg_no == 5)
        {
            if (DeadGhostLoad() == 0)
            {
                return;
            }

            ene_dead_load = 0;
            sgst_wrk.mode = SGST_MODE_END;

            return;
        }

        if (GuardGhostLoad() == 0)
        {
            return;
        }

        sgst_wrk.mode = SGST_MODE_END;
    break;
    case SGST_MODE_END:
        ingame_wrk.mode = INGAME_MODE_NOMAL;
        sgst_wrk.mode = SGST_MODE_NO_REQ;
    break;
    }
}

int SettleGhostLoadOneSize()
{
    switch(sgst_wrk.load_mode)
    {
    case SGLOAD_MODE_REQ:
        sgst_wrk.load_mode = SGLOAD_MODE_START;
    break;
    case SGLOAD_MODE_START:
        if (ap_wrk.ggst_no != 0xff)
        {
            motReleaseAniMdlBuf(fene_dat[ingame_wrk.msn_no][ap_wrk.ggst_no].anm_no, (u_int *)LOAD_ADDRESS_09);
        }

        LoadReq(M000_MIKU_MDL + (u_short)jene_dat[ingame_wrk.msn_no][sgst_wrk.sg_no].mdl_no, LOAD_ADDRESS_15);

        sgst_wrk.load_mode = SGLOAD_MODE_MDL;
    break;
    case SGLOAD_MODE_MDL:
        if (IsLoadEndAll() != 0)
        {
            motInitEnemyMdl((u_int *)LOAD_ADDRESS_15, jene_dat[ingame_wrk.msn_no][sgst_wrk.sg_no].mdl_no);
            LoadEneDmgTex(jene_dat[ingame_wrk.msn_no][sgst_wrk.sg_no].mdl_no, (u_int *)LOAD_ADDRESS_10);
            LoadReq(M000_MIKU_ANM + jene_dat[ingame_wrk.msn_no][sgst_wrk.sg_no].anm_no, LOAD_ADDRESS_09);

            sgst_wrk.load_mode = SGLOAD_MODE_MOT;
        }
    break;
    case SGLOAD_MODE_MOT:
        if (IsLoadEndAll() != 0)
        {
            motInitEnemyAnm((u_int *)LOAD_ADDRESS_09, jene_dat[ingame_wrk.msn_no][sgst_wrk.sg_no].mdl_no, jene_dat[ingame_wrk.msn_no][sgst_wrk.sg_no].anm_no);
            SeFileLoadAndSet(jene_dat[ingame_wrk.msn_no][sgst_wrk.sg_no].se_no, 18);

            sgst_wrk.load_mode = SGLOAD_MODE_SE;
        }
    break;
    case SGLOAD_MODE_SE:
        if (IsLoadEndAll() != 0)
        {
            sgst_wrk.load_mode = SGLOAD_MODE_END;
        }
    break;
    case SGLOAD_MODE_END:
        return 1;
    break;
    }

    return 0;
}

int SettleGhostLoadTwoSize()
{
    static int sg_load_id;

    switch(sgst_wrk.load_mode)
    {
    case SGLOAD_MODE_REQ:
        sgst_wrk.load_mode = SGLOAD_MODE_START;
    break;
    case SGLOAD_MODE_START:
        DeadGhostLoadDel();
        motReleaseAniMdlBuf(30, (u_int *)LOAD_ADDRESS_05);
        LoadReq(M037_TOUSHU_MDL, LOAD_ADDRESS_11);
        LoadReq(M900_ONNRYOU_MPK, LOAD_ADDRESS_13);

        sgst_wrk.load_mode = SGLOAD_MODE_MDL;
    break;
    case SGLOAD_MODE_MDL:
        if (IsLoadEndAll() != 0)
        {
            motInitEnemyMdl((u_int *)LOAD_ADDRESS_11, 37);
            motInitEnemyMdl((u_int *)LOAD_ADDRESS_13, 67);
            LoadEneDmgTex(37, (u_int *)LOAD_ADDRESS_06);
            LoadReq(M037_TOUSHU_ANM, LOAD_ADDRESS_05);

            sgst_wrk.load_mode = SGLOAD_MODE_MOT;
        }
    break;
    case SGLOAD_MODE_MOT:
        if (IsLoadEndAll() != 0)
        {
            motInitEnemyAnm((u_int *)LOAD_ADDRESS_05, 37, 25);
            SeFileLoadAndSet(SG037_TOUSHU_BD, 16);

            sgst_wrk.load_mode = SGLOAD_MODE_SE;
        }
    break;
    case SGLOAD_MODE_SE:
        if (IsLoadEndAll() != 0)
        {
            sg_load_id = SeFileLoadAndSet(SGY001_BD, 1);

            sgst_wrk.load_mode = SGLOAD_MODE_SE2;
        }
    break;
    case SGLOAD_MODE_SE2:
        if (IsLoadEnd(sg_load_id) == 0)
        {
            break;
        }

        sgst_wrk.load_mode = SGLOAD_MODE_END;
    // case fall-through
    case SGLOAD_MODE_END:
        return 1;
    break;
    }

    return 0;
}

void SettleGhostBattleSet()
{
    sceVu0FVECTOR pos;

    ene_wrk[0].dat_no = sgst_wrk.sg_no;
    ene_wrk[0].sta = 0x2;
    ene_wrk[0].type = 0;

    pos[0] = jene_dat[ingame_wrk.msn_no][sgst_wrk.sg_no].px;
    pos[1] = jene_dat[ingame_wrk.msn_no][sgst_wrk.sg_no].py;
    pos[2] = jene_dat[ingame_wrk.msn_no][sgst_wrk.sg_no].pz;

    ene_wrk[0].move_box.pos[0] = pos[0];
    ene_wrk[0].move_box.pos[1] = pos[1];
    ene_wrk[0].move_box.pos[2] = pos[2];
    ene_wrk[0].move_box.pos[3] = pos[3];
}

int SettleGhostStartDisp()
{
    switch(sgst_wrk.disp_mode)
    {
    case SGDISP_MODE_REQ:
        if (sgst_wrk.sg_no == 5)
        {
            sgst_wrk.disp_mode = SGDISP_MODE_WHITE_OUT0_1;
        }
        else
        {
            SetBlackOut2(16);

            sgst_wrk.time = 16;
            sgst_wrk.disp_mode = SGDISP_MODE_BLACK_OUT0;
        }
    break;
    case SGDISP_MODE_WHITE_OUT0_1:
        sgst_wrk.disp_mode = SGDISP_MODE_WHITE_OUT0_2;

        AdpcmPlayShinkan(sgef_adpcm_dat[sgst_wrk.sg_no], 0);
    break;
    case SGDISP_MODE_WHITE_OUT0_2:
        if (sgst_wrk.time == 0)
        {
            sg_cmr_bak.i[0] = camera.i[0];
            sg_cmr_bak.i[1] = camera.i[1];
            sg_cmr_bak.i[2] = camera.i[2];

            sg_cmr_bak.p[0] = camera.p[0];
            sg_cmr_bak.p[1] = camera.p[1];
            sg_cmr_bak.p[2] = camera.p[2];

            camera.i[0] = sgef_dat[sgst_wrk.sg_no].st_x0;
            camera.i[1] = sgef_dat[sgst_wrk.sg_no].st_y0;
            camera.i[2] = sgef_dat[sgst_wrk.sg_no].st_z0;

            camera.p[0] = sgef_dat[sgst_wrk.sg_no].st_x1;
            camera.p[1] = sgef_dat[sgst_wrk.sg_no].st_y1;
            camera.p[2] = sgef_dat[sgst_wrk.sg_no].st_z1;

            ingame_wrk.stts |= 0x8;
            plyr_wrk.sta |= 0x10;

            sgst_wrk.time = 256;
            sgst_wrk.disp_mode = SGDISP_MODE_CMR_ZOOM;
        }
        else
        {
            sgst_wrk.time--;
        }
    break;
    case SGDISP_MODE_BLACK_OUT0:
        if (sgst_wrk.time == 0)
        {
            sg_cmr_bak.i[0] = camera.i[0];
            sg_cmr_bak.i[1] = camera.i[1];
            sg_cmr_bak.i[2] = camera.i[2];

            sg_cmr_bak.p[0] = camera.p[0];
            sg_cmr_bak.p[1] = camera.p[1];
            sg_cmr_bak.p[2] = camera.p[2];

            camera.i[0] = sgef_dat[sgst_wrk.sg_no].st_x0;
            camera.i[1] = sgef_dat[sgst_wrk.sg_no].st_y0;
            camera.i[2] = sgef_dat[sgst_wrk.sg_no].st_z0;

            camera.p[0] = sgef_dat[sgst_wrk.sg_no].st_x1;
            camera.p[1] = sgef_dat[sgst_wrk.sg_no].st_y1;
            camera.p[2] = sgef_dat[sgst_wrk.sg_no].st_z1;

            ingame_wrk.stts |= 0x8;
            plyr_wrk.sta |= 0x10;

            AdpcmPlayShinkan(sgef_adpcm_dat[sgst_wrk.sg_no], 0);
            SetBlackIn2(16);

            sgst_wrk.time = 16;
            sgst_wrk.disp_mode = SGDISP_MODE_BLACK_IN0;
        }
        else
        {
            sgst_wrk.time--;
        }
    break;
    case SGDISP_MODE_BLACK_IN0:
        if (sgst_wrk.time == 0)
        {
            sgst_wrk.time = 256;
            sgst_wrk.disp_mode = SGDISP_MODE_CMR_ZOOM;
        }
        else
        {
            sgst_wrk.time--;
        }
    break;
    case SGDISP_MODE_CMR_ZOOM:
        if (sgst_wrk.time != 0)
        {
            sgst_wrk.time--;

            camera.i[0] = (int)(sgef_dat[sgst_wrk.sg_no].ed_x0 - (((sgef_dat[sgst_wrk.sg_no].ed_x0 - sgef_dat[sgst_wrk.sg_no].st_x0) * sgst_wrk.time) / 256));
            camera.i[1] = (int)(sgef_dat[sgst_wrk.sg_no].ed_y0 - (((sgef_dat[sgst_wrk.sg_no].ed_y0 - sgef_dat[sgst_wrk.sg_no].st_y0) * sgst_wrk.time) / 256));
            camera.i[2] = (int)(sgef_dat[sgst_wrk.sg_no].ed_z0 - (((sgef_dat[sgst_wrk.sg_no].ed_z0 - sgef_dat[sgst_wrk.sg_no].st_z0) * sgst_wrk.time) / 256));

            camera.p[0] = (int)(sgef_dat[sgst_wrk.sg_no].ed_x1 - (((sgef_dat[sgst_wrk.sg_no].ed_x1 - sgef_dat[sgst_wrk.sg_no].st_x1) * sgst_wrk.time) / 256));
            camera.p[1] = (int)(sgef_dat[sgst_wrk.sg_no].ed_y1 - (((sgef_dat[sgst_wrk.sg_no].ed_y1 - sgef_dat[sgst_wrk.sg_no].st_y1) * sgst_wrk.time) / 256));
            camera.p[2] = (int)(sgef_dat[sgst_wrk.sg_no].ed_z1 - (((sgef_dat[sgst_wrk.sg_no].ed_z1 - sgef_dat[sgst_wrk.sg_no].st_z1) * sgst_wrk.time) / 256));
        }
        else if (IsEndAdpcmShinkan() != 0)
        {
            if (sgst_wrk.sg_no == 5)
            {
                sgst_wrk.disp_mode = SGDISP_MODE_WHITE_OUT1_1;
            }
            else
            {
                SetBlackOut2(16);

                sgst_wrk.time = 16;
                sgst_wrk.disp_mode = SGDISP_MODE_BLACK_OUT1;
            }
        }
    break;
    case SGDISP_MODE_WHITE_OUT1_1:
        sgst_wrk.time = 85;
        sgst_wrk.disp_mode = SGDISP_MODE_WHITE_OUT1_2;
    break;
    case SGDISP_MODE_WHITE_OUT1_2:
        if (sgst_wrk.time == 0)
        {
            camera.i[0] = sg_cmr_bak.i[0];
            camera.i[1] = sg_cmr_bak.i[1];
            camera.i[2] = sg_cmr_bak.i[2];

            camera.p[0] = sg_cmr_bak.p[0];
            camera.p[1] = sg_cmr_bak.p[1];
            camera.p[2] = sg_cmr_bak.p[2];

            plyr_wrk.sta &= ~0x10;
            sgst_wrk.time = 85;

            sgst_wrk.disp_mode = SGDISP_MODE_WHITE_OUT1_3;
        }
        else
        {
            sgst_wrk.time--;
        }
    break;
    case SGDISP_MODE_WHITE_OUT1_3:
        if (sgst_wrk.time != 0)
        {
            sgst_wrk.time--;
        }
        else
        {
            sgst_wrk.disp_mode = SGDISP_MODE_END;
        }
    break;
    case SGDISP_MODE_BLACK_OUT1:
        if (sgst_wrk.time == 0)
        {
            camera.i[0] = sg_cmr_bak.i[0];
            camera.i[1] = sg_cmr_bak.i[1];
            camera.i[2] = sg_cmr_bak.i[2];

            camera.p[0] = sg_cmr_bak.p[0];
            camera.p[1] = sg_cmr_bak.p[1];
            camera.p[2] = sg_cmr_bak.p[2];

            plyr_wrk.sta &= ~0x10;
            sgst_wrk.time = 16;

            SetBlackIn2(16);

            sgst_wrk.disp_mode = SGDISP_MODE_BLACK_IN1;
        }
        else
        {
            sgst_wrk.time--;
        }
    break;
    case SGDISP_MODE_BLACK_IN1:
        if (sgst_wrk.time != 0)
        {
            sgst_wrk.time --;
        }
        else
        {
            sgst_wrk.disp_mode = SGDISP_MODE_END;
        }
    break;
    case SGDISP_MODE_END:
        ingame_wrk.stts &= ~0x8;

        return 1;
    break;
    case SGDISP_MODE_START:
        // do nothing ...
    break;
    }

    return 0;
}

int SettleGhostBattleEnd()
{
    if (sgst_wrk.mode != SGST_MODE_NO_REQ)
    {
        sgst_wrk.mode = SGST_MODE_DEAD_IN;
        ingame_wrk.mode = INGAME_MODE_SGST_DISP;

        return 1;
    }

    return 0;
}

void SettleGhostDispEffect()
{
    switch(sgst_wrk.disp_mode)
    {
    case SGDISP_MODE_REQ:
        // do nothing ...
    break;
    case SGDISP_MODE_START:
        // do nothing ...
    break;
    case SGDISP_MODE_WHITE_OUT0_1:
        InitExFade2(0, 85, 85, 85);
    break;
    case SGDISP_MODE_BLACK_IN1:
        // do nothing ...
    break;
    case SGDISP_MODE_END:
        // do nothing ...
    break;
    case SGDISP_MODE_WHITE_OUT0_2:
        SetExFade2();
    break;
    case SGDISP_MODE_BLACK_OUT0:
        if (sgst_wrk.time == 0)
        {
            InitExFade1(0, 256);
        }
    break;
    case SGDISP_MODE_BLACK_IN0:
        SetExFade1();
    break;
    case SGDISP_MODE_CMR_ZOOM:
        if (sgst_wrk.sg_no == 5)
        {
            SetExFade2();
        }
        else
        {
            SetExFade1();
        }
    break;
    case SGDISP_MODE_WHITE_OUT1_1:
        InitExFade2(1, 85, 85, 85);
        SetExFade2();
    break;
    case SGDISP_MODE_WHITE_OUT1_2:
        SetExFade2();
    break;
    case SGDISP_MODE_WHITE_OUT1_3:
        SetExFade2();
    break;
    case SGDISP_MODE_BLACK_OUT1:
        if (sgst_wrk.time == 0)
        {
            InitExFade1(1, 256);
        }

        SetExFade1();
    break;
    }
}
