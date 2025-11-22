#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "camera.h"

#include "sce/sifdev.h"
#include "sce/libvu0.h"

#include "main/glob.h"
#include "common/ul_math.h"
#include "ingame/map/map_ctrl.h"
#include "ingame/menu/sp_menu.h"
#include "ingame/plyr/unit_ctl.h"
#include "ingame/event/wan_soul.h"
#include "graphics/motion/mdlact.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_scr.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph3d/sglib.h"

#include "data/ncam_000.h" // u_int ncam_000[];
#include "data/ncam_001.h" // u_int ncam_001[];
#include "data/ncam_002.h" // u_int ncam_002[];
#include "data/ncam_003.h" // u_int ncam_003[];
#include "data/ncam_006.h" // u_int ncam_006[];
#include "data/ncam_007.h" // u_int ncam_007[];
#include "data/ncam_008.h" // u_int ncam_008[];
#include "data/ncam_009.h" // u_int ncam_009[];
#include "data/ncam_010.h" // u_int ncam_010[];
#include "data/ncam_011.h" // u_int ncam_011[];
#include "data/ncam_012.h" // u_int ncam_012[];
#include "data/ncam_013.h" // u_int ncam_013[];
#include "data/ncam_014.h" // u_int ncam_014[];
#include "data/ncam_015.h" // u_int ncam_015[];
#include "data/ncam_016.h" // u_int ncam_016[];
#include "data/ncam_021.h" // u_int ncam_021[];
#include "data/ncam_022.h" // u_int ncam_022[];
#include "data/ncam_023.h" // u_int ncam_023[];
#include "data/ncam_024.h" // u_int ncam_024[];
#include "data/ncam_025.h" // u_int ncam_025[];
#include "data/ncam_026.h" // u_int ncam_026[];
#include "data/ncam_comple_dat1.h" // u_int *ncam_comple_dat1[];
#include "data/ncam_comple_dat2.h" // u_int *ncam_comple_dat2[];
#include "data/ncam_comple_dat3.h" // u_int *ncam_comple_dat3[];
#include "data/ncam_comple_dat4.h" // u_int *ncam_comple_dat4[];
#include "data/bcam_000.h" // u_int bcam_000[];
#include "data/bcam_comple_dat1.h" // u_int *bcam_comple_dat1[];
#include "data/bcam_comple_dat2.h" // u_int *bcam_comple_dat2[];
#include "data/bcam_comple_dat3.h" // u_int *bcam_comple_dat3[];
#include "data/bcam_comple_dat4.h" // u_int *bcam_comple_dat4[];
#include "data/dcam_000.h" // u_int dcam_000[];
#include "data/dcam_comple_dat1.h" // u_int *dcam_comple_dat1[];
#include "data/dcam_comple_dat2.h" // u_int *dcam_comple_dat2[];
#include "data/dcam_comple_dat3.h" // u_int *dcam_comple_dat3[];
#include "data/dcam_comple_dat4.h" // u_int *dcam_comple_dat4[];
#include "data/dc0000.h" // u_short dc0000[];
#include "data/dc0010.h" // u_short dc0010[];
#include "data/dc0020.h" // u_short dc0020[];
#include "data/dc0030.h" // u_short dc0030[];
#include "data/dc0040.h" // u_short dc0040[];
#include "data/dc0050.h" // u_short dc0050[];
#include "data/dc0060.h" // u_short dc0060[];
#include "data/dc0070.h" // u_short dc0070[];
#include "data/dc0080.h" // u_short dc0080[];
#include "data/dc0090.h" // u_short dc0090[];
#include "data/dc0100.h" // u_short dc0100[];
#include "data/dc0110.h" // u_short dc0110[];
#include "data/dc0120.h" // u_short dc0120[];
#include "data/dc0130.h" // u_short dc0130[];
#include "data/dc0140.h" // u_short dc0140[];
#include "data/dc0150.h" // u_short dc0150[];
#include "data/dc0160.h" // u_short dc0160[];
#include "data/dc0170.h" // u_short dc0170[];
#include "data/dc0180.h" // u_short dc0180[];
#include "data/dc0190.h" // u_short dc0190[];
#include "data/dc0200.h" // u_short dc0200[];
#include "data/dc0210.h" // u_short dc0210[];
#include "data/dc0220.h" // u_short dc0220[];
#include "data/dc0230.h" // u_short dc0230[];
#include "data/dc0240.h" // u_short dc0240[];
#include "data/dc0250.h" // u_short dc0250[];
#include "data/dc0260.h" // u_short dc0260[];
#include "data/dc0270.h" // u_short dc0270[];
#include "data/dc0280.h" // u_short dc0280[];
#include "data/dc0290.h" // u_short dc0290[];
#include "data/dc0300.h" // u_short dc0300[];
#include "data/dc0310.h" // u_short dc0310[];
#include "data/dc0320.h" // u_short dc0320[];
#include "data/dc0330.h" // u_short dc0330[];
#include "data/dc0340.h" // u_short dc0340[];
#include "data/dc0350.h" // u_short dc0350[];
#include "data/dc0360.h" // u_short dc0360[];
#include "data/dc0370.h" // u_short dc0370[];
#include "data/dc0380.h" // u_short dc0380[];
#include "data/dc0390.h" // u_short dc0390[];
#include "data/dc0400.h" // u_short dc0400[];
#include "data/dc0410.h" // u_short dc0410[];
#include "data/dc0420.h" // u_short dc0420[];
#include "data/dc0430.h" // u_short dc0430[];
#include "data/dc0440.h" // u_short dc0440[];
#include "data/dc0450.h" // u_short dc0450[];
#include "data/dc0460.h" // u_short dc0460[];
#include "data/dc0470.h" // u_short dc0470[];
#include "data/dc0480.h" // u_short dc0480[];
#include "data/dc0490.h" // u_short dc0490[];
#include "data/dc0500.h" // u_short dc0500[];
#include "data/dc0510.h" // u_short dc0510[];
#include "data/dc_no_tbl0.h" // u_short *dc_no_tbl0[];
#include "data/dc1000.h" // u_short dc1000[];
#include "data/dc1010.h" // u_short dc1010[];
#include "data/dc1020.h" // u_short dc1020[];
#include "data/dc1030.h" // u_short dc1030[];
#include "data/dc1040.h" // u_short dc1040[];
#include "data/dc1050.h" // u_short dc1050[];
#include "data/dc1060.h" // u_short dc1060[];
#include "data/dc1070.h" // u_short dc1070[];
#include "data/dc1080.h" // u_short dc1080[];
#include "data/dc1090.h" // u_short dc1090[];
#include "data/dc1100.h" // u_short dc1100[];
#include "data/dc1110.h" // u_short dc1110[];
#include "data/dc1120.h" // u_short dc1120[];
#include "data/dc1130.h" // u_short dc1130[];
#include "data/dc1140.h" // u_short dc1140[];
#include "data/dc1150.h" // u_short dc1150[];
#include "data/dc1160.h" // u_short dc1160[];
#include "data/dc1170.h" // u_short dc1170[];
#include "data/dc1180.h" // u_short dc1180[];
#include "data/dc1190.h" // u_short dc1190[];
#include "data/dc1200.h" // u_short dc1200[];
#include "data/dc1210.h" // u_short dc1210[];
#include "data/dc1220.h" // u_short dc1220[];
#include "data/dc1230.h" // u_short dc1230[];
#include "data/dc1240.h" // u_short dc1240[];
#include "data/dc1250.h" // u_short dc1250[];
#include "data/dc1260.h" // u_short dc1260[];
#include "data/dc1270.h" // u_short dc1270[];
#include "data/dc1280.h" // u_short dc1280[];
#include "data/dc1290.h" // u_short dc1290[];
#include "data/dc_no_tbl1.h" // u_short *dc_no_tbl1[];
#include "data/dc2000.h" // u_short dc2000[];
#include "data/dc2010.h" // u_short dc2010[];
#include "data/dc2020.h" // u_short dc2020[];
#include "data/dc2030.h" // u_short dc2030[];
#include "data/dc2040.h" // u_short dc2040[];
#include "data/dc2050.h" // u_short dc2050[];
#include "data/dc2060.h" // u_short dc2060[];
#include "data/dc2070.h" // u_short dc2070[];
#include "data/dc2080.h" // u_short dc2080[];
#include "data/dc2090.h" // u_short dc2090[];
#include "data/dc2100.h" // u_short dc2100[];
#include "data/dc2110.h" // u_short dc2110[];
#include "data/dc2120.h" // u_short dc2120[];
#include "data/dc2130.h" // u_short dc2130[];
#include "data/dc2140.h" // u_short dc2140[];
#include "data/dc2150.h" // u_short dc2150[];
#include "data/dc2160.h" // u_short dc2160[];
#include "data/dc2170.h" // u_short dc2170[];
#include "data/dc2180.h" // u_short dc2180[];
#include "data/dc2190.h" // u_short dc2190[];
#include "data/dc2200.h" // u_short dc2200[];
#include "data/dc2210.h" // u_short dc2210[];
#include "data/dc2220.h" // u_short dc2220[];
#include "data/dc2230.h" // u_short dc2230[];
#include "data/dc2240.h" // u_short dc2240[];
#include "data/dc2250.h" // u_short dc2250[];
#include "data/dc2260.h" // u_short dc2260[];
#include "data/dc2270.h" // u_short dc2270[];
#include "data/dc2280.h" // u_short dc2280[];
#include "data/dc2290.h" // u_short dc2290[];
#include "data/dc_no_tbl2.h" // u_short *dc_no_tbl2[];
#include "data/dc3000.h" // u_short dc3000[];
#include "data/dc3010.h" // u_short dc3010[];
#include "data/dc3020.h" // u_short dc3020[];
#include "data/dc3030.h" // u_short dc3030[];
#include "data/dc3040.h" // u_short dc3040[];
#include "data/dc3050.h" // u_short dc3050[];
#include "data/dc3060.h" // u_short dc3060[];
#include "data/dc3070.h" // u_short dc3070[];
#include "data/dc3080.h" // u_short dc3080[];
#include "data/dc3090.h" // u_short dc3090[];
#include "data/dc3100.h" // u_short dc3100[];
#include "data/dc3110.h" // u_short dc3110[];
#include "data/dc3120.h" // u_short dc3120[];
#include "data/dc3130.h" // u_short dc3130[];
#include "data/dc3140.h" // u_short dc3140[];
#include "data/dc3150.h" // u_short dc3150[];
#include "data/dc3160.h" // u_short dc3160[];
#include "data/dc3170.h" // u_short dc3170[];
#include "data/dc3180.h" // u_short dc3180[];
#include "data/dc3190.h" // u_short dc3190[];
#include "data/dc3200.h" // u_short dc3200[];
#include "data/dc3210.h" // u_short dc3210[];
#include "data/dc3220.h" // u_short dc3220[];
#include "data/dc3230.h" // u_short dc3230[];
#include "data/dc3240.h" // u_short dc3240[];
#include "data/dc3250.h" // u_short dc3250[];
#include "data/dc3260.h" // u_short dc3260[];
#include "data/dc3270.h" // u_short dc3270[];
#include "data/dc3280.h" // u_short dc3280[];
#include "data/dc3290.h" // u_short dc3290[];
#include "data/dc_no_tbl3.h" // u_short *dc_no_tbl3[];

u_short drm_cam_no[12] = {
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
};
# include "data/adj_cam_pos.h" // sceVu0FVECTOR adj_cam_pos;

static u_short cam_no_save = 0;
static u_short compling = 0xFFFF;
static u_char cam_mode_old = 0;
static u_char req_rc_fade = 0;
static u_char drm_cam_req = 0;
static u_short drm_cam_tm = 0;
u_char DBG_cam_id_move_chk = 0;
u_char cd_edit_end = 0;
int msn_no = 0;
int renewal_data_chk = 0;
int renewal_data_chk_cnt = 0;
u_short cdcopy[2] = {0};
u_char cam_info_disp = 1;

#define PI 3.1415927f
#define DEG2RAD(x) ((float)(x)*PI/180.0f)

void CameraMain()
{
    float fov = DEG2RAD(51.0f);

    if (dbg_wrk.cam_mode == 1 && DBG_cam_id_move_chk == 0)
    {
        cam_id_move.i[0] = camera.i[0];
        cam_id_move.i[1] = camera.i[1];
        cam_id_move.i[2] = camera.i[2];
        cam_id_move.i[3] = camera.i[3];

        cam_id_move.p[0] = camera.p[0];
        cam_id_move.p[1] = camera.p[1];
        cam_id_move.p[2] = camera.p[2];
        cam_id_move.p[3] = camera.p[3];

        cam_id_move.rot_x[0] = 0.0f;
        cam_id_move.rot_x[1] = 0.0f;
        cam_id_move.rot_x[2] = 0.0f;
        cam_id_move.rot_x[3] = 0.0f;

        cam_id_move.rot_y[0] = 0.0f;
        cam_id_move.rot_y[1] = 0.0f;
        cam_id_move.rot_y[2] = 0.0f;
        cam_id_move.rot_y[3] = 0.0f;

        GetTrgtRot(camera.p, camera.i, cam_id_move.rot_x, 1);
        GetTrgtRot(camera.p, camera.i, cam_id_move.rot_y, 2);

        plyr_adj[0] = plyr_adj[1] = plyr_adj[2] = plyr_adj[3] = 0;

        cam_type = 0;
        cd_step = 0;
        cam_id = 0;
        cd_edit_end = 0;
    }

    if (dbg_wrk.cam_mode == 1)
    {
        DBG_cam_id_move_chk = 1;
    }
    else
    {
        DBG_cam_id_move_chk = 0;
    }

    if (DBG_cam_id_move_chk)
    {
        CameraIdMoveCtrl();
        return;
    }

    plyr_wrk.pr_info.cam_type = 0xff;

    if (plyr_wrk.ap_timer != 0 || SaveCamChk() != 0 || SoulTellingCameraInCtrl() != 0)
    {
        return;
    }

    switch(plyr_wrk.mode)
    {
    case PMODE_FINDER_IN:
        FinderInCameraCtrl();
        cam_mode_old = 6;
    break;
    case PMODE_FIN_CAM:
        if (ene_wrk[plyr_wrk.rock_trgt].sta & 1 && ene_wrk[plyr_wrk.rock_trgt].tr_rate != 0)
        {
            camera.i[0] = ene_wrk[plyr_wrk.rock_trgt].mpos.p0[0];
            camera.i[1] = ene_wrk[plyr_wrk.rock_trgt].mpos.p0[1];
            camera.i[2] = ene_wrk[plyr_wrk.rock_trgt].mpos.p0[2];
            camera.i[3] = ene_wrk[plyr_wrk.rock_trgt].mpos.p0[3];
        }
    break;
    case PMODE_FINDER:
          if (cam_mode_old != 1)
          {
              camera.fov = fov;
          }
          FinderModeCameraCtrl();
          cam_mode_old = 1;
    break;
    case PMODE_DMG:
    case PMODE_PRE_DMG:
        PlyrDmgCameraCtrl();
        cam_mode_old = 2;
    break;
    default:
        if (plyr_wrk.cond == 2)
        {
            KonwakuCamCtrl();
            cam_mode_old = 0;
        }
        else
        {
            NormalCameraCtrl();
            cam_mode_old = 0;
        }
    break;
    }

    plyr_wrk.pr_info.camera_no_old = plyr_wrk.pr_info.camera_no;
    plyr_wrk.pr_info.camera_btl_old = plyr_wrk.pr_info.camera_btl;
    plyr_wrk.pr_info.camera_drm_old = plyr_wrk.pr_info.camera_drm;

    FinderInOverRapCtrl();
    QuakeCamera();
}

void KonwakuCamCtrl()
{
    sceVu0FVECTOR rv;
    sceVu0FVECTOR tv;

    tv[0] = 0.0f;
    tv[1] = -500.0f;
    tv[2] = 800.0f;
    tv[3] = 0.0f;

    RotFvector(plyr_wrk.move_box.rot, tv);
    sceVu0AddVector(camera.i, plyr_wrk.move_box.pos, tv);

    tv[0] = 0.0f;
    tv[1] = -950.0f;
    tv[2] = -400.0f;
    tv[3] = 0.0f;

    RotFvector(plyr_wrk.move_box.rot, tv);
    sceVu0AddVector(camera.p, plyr_wrk.move_box.pos, tv);

    camera.roll = PI;
    camera.fov = DEG2RAD(44.0f);
}

void FinderInCameraCtrl()
{
    sceVu0FVECTOR tv;
    sceVu0FVECTOR rv;
    static sceVu0FVECTOR pv;
    static sceVu0FVECTOR iv;
    static u_char time = 0;

    if ((plyr_wrk.sta & 4) != 0)
    {
        plyr_wrk.sta &= ~0x4;

        time = 25;

        if (plyr_wrk.move_box.rspd[1] != 0)
        {
            plyr_wrk.move_box.rspd[1] /= 25;
        }
    }
    if (time == 0)
    {
        plyr_wrk.mode = PMODE_FINDER;

        plyr_wrk.move_box.rspd[0] = 0;
        plyr_wrk.move_box.rspd[1] = 0;
        plyr_wrk.move_box.rspd[2] = 0;
        plyr_wrk.move_box.rspd[3] = 0;

        SetDebugMenuSwitch(0);
    }
    else
    {
        time -= 1;

        if (plyr_wrk.move_box.rspd[1] != 0)
        {
            plyr_wrk.move_box.rot[1] += plyr_wrk.move_box.rspd[1];

            RotLimitChk(&plyr_wrk.move_box.rot[1]);
        }

        if (time == 1)
        {
            ReqFinderInOverRap(30);
        }
  }
}

void NormalCameraCtrl()
{
    MAP_CAM_INFO mci;
    SgCAMERA tc;
    SgCAMERA tc2;
    static SgCAMERA oc;

    if (dbg_wrk.cam_mode == 2)
    {
        tc.i[0] = plyr_wrk.move_box.pos[0];
        tc.i[1] = plyr_wrk.move_box.pos[1];
        tc.i[2] = plyr_wrk.move_box.pos[2];
        tc.i[3] = plyr_wrk.move_box.pos[3];

        tc.p[0] = plyr_wrk.move_box.pos[0];
        tc.p[1] = plyr_wrk.move_box.pos[1];
        tc.p[2] = plyr_wrk.move_box.pos[2];
        tc.p[3] = plyr_wrk.move_box.pos[3];

        sceVu0AddVector(tc.p, tc.p, adj_cam_pos);
        tc.roll = PI;
        tc.fov = DEG2RAD(44.0f);
    }
    else
    {
        DramaCameraReqCtrl();

        if (GetCameraInfo(&mci) != 0)
        {
            return;
        }

        if (mci.change != 0)
        {
            plyr_wrk.cp_old[0] = camera.p[0];
            plyr_wrk.cp_old[1] = camera.p[1];
            plyr_wrk.cp_old[2] = camera.p[2];
            plyr_wrk.cp_old[3] = camera.p[3];
        }

        switch(mci.type)
        {
        case 0:
            SetCamPos0(&tc, &mci);
        break;
        case 1:
            SetCamPos1(&tc, &mci);
        break;
        case 2:
            SetCamPos2(&tc, &mci);
        break;
        case 3:
            SetCamPos3(&tc, &mci);
        break;
        case 4:
            SetCamPos4(&tc, &mci);
        break;
        case 5:
            SetCamPos5(&tc, &mci);
        break;
        }

        tc2 = tc;

        CompleCameraPos(&tc, &oc, &mci);

        oc = tc2;
    }

    camera.i[0] = tc.i[0];
    camera.i[1] = tc.i[1];
    camera.i[2] = tc.i[2];
    camera.i[3] = tc.i[3];

    camera.p[0] = tc.p[0];
    camera.p[1] = tc.p[1];
    camera.p[2] = tc.p[2];
    camera.p[3] = tc.p[3];

    camera.roll = tc.roll;
    camera.fov = tc.fov;

    plyr_wrk.prot = GetTrgtRotY(camera.p, plyr_wrk.move_box.pos);
}

int GetCameraInfo(MAP_CAM_INFO *mci)
{
    static u_char kind_old = 0;

    mci->change = 0;

    GetCameraData(kind_old, mci);
    if (
        (kind_old != 2 && drm_cam_req != 0) ||
        (
            (kind_old == 2 && drm_cam_req == 0) ||
             plyr_wrk.pr_info.camera_door != 0xffff ||
            mci->no != mci->no_old
        )
    )
    {
        mci->change = 1;

        if (drm_cam_req != 0)
        {
            if (kind_old == 2)
            {
                mci->change = 0;
            }

            mci->kind = 2;
        }
        else if (plyr_wrk.pr_info.camera_door != 0xffff)
        {
            mci->change = 0;
            mci->kind = 3;
        }
        else
        {
            if (plyr_wrk.sta & 1)
            {
                mci->kind = 1;
            }
            else
            {
                mci->kind = 0;
            }
        }

        if (dbg_wrk.cam_mode == 0)
        {
            switch (dbg_wrk.cam_hold_mode)
            {
            case 1:
                mci->kind = 1;
            break;
            case 2:
                mci->kind = 2;
            break;
            }
        }

        if (mci->kind != kind_old)
        {
            compling = 0xffff;
            GetCameraData(mci->kind, mci);
        }

        kind_old = mci->kind;
    }
    else
    {
        mci->kind = kind_old;
    }

    plyr_wrk.pr_info.cam_type = mci->kind;

    return 0;
}

void GetCameraData(u_char kind, MAP_CAM_INFO *mci)
{
    switch (kind)
    {
        case 0:
            mci->no = plyr_wrk.pr_info.camera_no;
            mci->no_old = plyr_wrk.pr_info.camera_no_old;
            mci->mcd = &map_cam_dat[mci->no];
            mci->type = map_cam_dat[mci->no].type;
        break;
        case 1:
            mci->no = plyr_wrk.pr_info.camera_btl;
            mci->no_old = plyr_wrk.pr_info.camera_btl_old;
            mci->mcd = &map_cam_dat2[mci->no];
            mci->type = map_cam_dat2[mci->no].type;
        break;
        case 2:
            mci->no = plyr_wrk.pr_info.camera_drm;
            mci->no_old = plyr_wrk.pr_info.camera_drm_old;
            mci->mcd = &map_cam_dat3[mci->no];
            mci->type = map_cam_dat3[mci->no].type;
        break;
        case 3:
            mci->no = plyr_wrk.pr_info.camera_door;
            mci->no_old = plyr_wrk.pr_info.camera_door_old;
            mci->mcd = &map_cam_dat4[mci->no];
            mci->type = map_cam_dat4[mci->no].type;
        break;
    }
}


float GetMCLocalPosPer(u_short cn, u_char kind, u_char id);

void SetCamPos0(SgCAMERA *tc, MAP_CAM_INFO *mci)
{
    sceVu0FVECTOR tv;

    GetMCLocalPosPer(0, 0, 0xff);

    tc->i[1] = (short)mci->mcd->p0[1];
    tc->i[0] = mci->mcd->p0[0];
    tc->i[2] = mci->mcd->p0[2];

    tv[3] = 0.0f;

    tc->i[3] = tv[3] = 0.0f;

    tv[0] = mci->mcd->p1[0];
    tv[1] = (short)mci->mcd->p1[1];
    tv[2] = mci->mcd->p1[2];
    tv[3] = tc->i[3];

    tc->p[0] = tv[0];
    tc->p[1] = tv[1];
    tc->p[2] = tv[2];
    tc->p[3] = tv[3];

    tc->roll = mci->mcd->roll[0];
    tc->fov = mci->mcd->fov[0];
}


void SetCamPos1(SgCAMERA *tc, MAP_CAM_INFO *mci)
{
    sceVu0FVECTOR tv2;
    static sceVu0FVECTOR tv = { 0.0f, 0.0f, 0.0f, 0.0f };

    GetMCLocalPosPer(0, 0, 0xff);

    if (mci->change != 0)
    {
        tv[0] = (short)mci->mcd->p1[0];
        tv[1] = (short)mci->mcd->p1[1];
        tv[2] = (short)mci->mcd->p1[2];
        tv[3] = 0.0f;

        RotFvector(plyr_wrk.move_box.rot, tv);
    }

    sceVu0AddVector(tv2, plyr_wrk.move_box.pos, tv);

    tc->i[0] = tv2[0];
    tc->i[1] = tv2[1] + -400.0f;
    tc->i[2] = tv2[2];
    tc->i[3] = tv2[3];

    tv2[0] = mci->mcd->p0[0];
    tv2[1] = (short)mci->mcd->p0[1];
    tv2[2] = mci->mcd->p0[2];
    tv2[3] = 0.0f;

    tc->p[0] = tv2[0];
    tc->p[1] = tv2[1];
    tc->p[2] = tv2[2];
    tc->p[3] = tv2[3];

    tc->roll = mci->mcd->roll[0];
    tc->fov = mci->mcd->fov[0];
}

void SetCamPos2(SgCAMERA *tc, MAP_CAM_INFO *mci)
{
    sceVu0FVECTOR tv;
    sceVu0FVECTOR bv;
    float per;

    tv[0] = (u_short)mci->mcd->p0[0];
    tv[1] = (short)mci->mcd->p0[1];
    tv[2] = (u_short)mci->mcd->p0[2];
    tv[3] = 0.0f;

    tc->i[0] = tv[0];
    tc->i[1] = tv[1];
    tc->i[2] = tv[2];
    tc->i[3] = tv[3];

    per = GetMCLocalPosPer(mci->no, mci->kind, mci->mcd->id);

    tv[0] = ((u_short)mci->mcd->p2[0] - (u_short)mci->mcd->p1[0]) * per;
    tv[1] = ((short)mci->mcd->p2[1] - (short)mci->mcd->p1[1]) * per;
    tv[2] = ((u_short)mci->mcd->p2[2] - (u_short)mci->mcd->p1[2]) * per;
    tv[3] = 0.0f;

    bv[0] = (u_short)mci->mcd->p1[0];
    bv[1] = (short)mci->mcd->p1[1];
    bv[2] = (u_short)mci->mcd->p1[2];
    bv[3] = 0.0f;

    sceVu0AddVector(tc->p, bv, tv);

    if (mci->mcd->roll[1] != 0.0f)
    {
        bv[0] = mci->mcd->roll[1] - mci->mcd->roll[0];

        RotLimitChk(bv);

        bv[0] *= per;
        tc->roll = mci->mcd->roll[0] + bv[0];

        RotLimitChk(&tc->roll);
    }
    else
    {
        tc->roll = mci->mcd->roll[0];
    }

    if (mci->mcd->fov[1] != 0.0f)
    {
        tv[0] = (mci->mcd->fov[1] - mci->mcd->fov[0]) * per;
        tc->fov = mci->mcd->fov[0] + tv[0];
    }
    else
    {
        tc->fov = mci->mcd->fov[0];
    }
}

void SetCamPos3(SgCAMERA *tc, MAP_CAM_INFO *mci)
{
    sceVu0FVECTOR tv2;
    sceVu0FVECTOR bv;
    float per;
    static sceVu0FVECTOR tv = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (mci->change != 0)
    {
        tv[0] = (short)mci->mcd->p2[0];
        tv[1] = (short)mci->mcd->p2[1];
        tv[2] = (short)mci->mcd->p2[2];
        tv[3] = 0.0f;

        RotFvector(plyr_wrk.move_box.rot, tv);
    }

    sceVu0AddVector(tv2, plyr_wrk.move_box.pos, tv);

    tv2[1] = tv2[1] + -400.0f;

    tc->i[0] = tv2[0];
    tc->i[1] = tv2[1];
    tc->i[2] = tv2[2];
    tc->i[3] = tv2[3];

    per = GetMCLocalPosPer(mci->no, mci->kind, mci->mcd->id);

    tv2[0] = ((u_short)mci->mcd->p1[0] - (u_short)mci->mcd->p0[0]) * per;
    tv2[1] = ((short)mci->mcd->p1[1] - (short)mci->mcd->p0[1]) * per;
    tv2[2] = ((u_short)mci->mcd->p1[2] - (u_short)mci->mcd->p0[2]) * per;
    tv2[3] = 0.0f;

    bv[0] = (u_short)mci->mcd->p0[0];
    bv[1] = (short)mci->mcd->p0[1];
    bv[2] = (u_short)mci->mcd->p0[2];
    bv[3] = 0.0f;

    sceVu0AddVector(tc->p, bv, tv2);

    if (mci->mcd->roll[1] != 0.0f)
    {
        bv[0] = mci->mcd->roll[1] - mci->mcd->roll[0];

        RotLimitChk(bv);

        bv[0] *= per;

        tc->roll = mci->mcd->roll[0] + bv[0];

        RotLimitChk(&tc->roll);
    }
    else
    {
        tc->roll = mci->mcd->roll[0];
    }

    if (mci->mcd->fov[1] != 0.0f)
    {
        tv[0] = (mci->mcd->fov[1] - mci->mcd->fov[0]) * per;

        tc->fov = mci->mcd->fov[0] + tv[0];
    }
    else
    {
        tc->fov = mci->mcd->fov[0];
    }
}

void SetCamPos4(SgCAMERA *tc, MAP_CAM_INFO *mci)
{
    sceVu0FVECTOR tv;
    sceVu0FVECTOR bv;
    float per;

    per = GetMCLocalPosPer(mci->no, mci->kind, mci->mcd->id);

    tv[0] = ((u_short)mci->mcd->p1[0] - (u_short)mci->mcd->p0[0]) * per;
    tv[1] = ((short)mci->mcd->p1[1] - (short)mci->mcd->p0[1]) * per;
    tv[2] = ((u_short)mci->mcd->p1[2] - (u_short)mci->mcd->p0[2]) * per;
    tv[3] = 0.0f;

    bv[0] = (u_short)mci->mcd->p0[0];
    bv[1] = (short)mci->mcd->p0[1];
    bv[2] = (u_short)mci->mcd->p0[2];
    bv[3] = 0.0f;

    sceVu0AddVector(tc->i, bv, tv);

    tv[0] = ((u_short)mci->mcd->p3[0] - (u_short)mci->mcd->p2[0]) * per;
    tv[1] = ((short)mci->mcd->p3[1] - (short)mci->mcd->p2[1]) * per;
    tv[2] = ((u_short)mci->mcd->p3[2] - (u_short)mci->mcd->p2[2]) * per;
    tv[3] = 0.0f;

    bv[0] = (u_short)mci->mcd->p2[0];
    bv[1] = (short)mci->mcd->p2[1];
    bv[2] = (u_short)mci->mcd->p2[2];
    bv[3] = 0.0f;

    sceVu0AddVector(tc->p, bv, tv);

    if (mci->mcd->roll[1] != 0.0f)
    {
        bv[0] = mci->mcd->roll[1] - mci->mcd->roll[0];

        RotLimitChk(bv);

        bv[0] = bv[0] * per;

        tc->roll = mci->mcd->roll[0] + bv[0];

        RotLimitChk(&tc->roll);

    }
    else
    {
        tc->roll = mci->mcd->roll[0];
    }

    if (mci->mcd->fov[1] != 0.0f)
    {
        tv[0] = (mci->mcd->fov[1] - mci->mcd->fov[0]) * per;

        tc->fov = mci->mcd->fov[0] + tv[0];
    }
    else
    {
        tc->fov = mci->mcd->fov[0];
    }
}

void SetCamPos5(SgCAMERA *tc, MAP_CAM_INFO *mci)
{
    sceVu0FVECTOR tv2;
    static sceVu0FVECTOR tv = { 0.0f, 0.0f, 0.0f, 0.0f };

    GetMCLocalPosPer(0, 0x0, 0xff);

    if (mci->change != 0x0)
    {
        tv[0] = (short)mci->mcd->p1[0];
        tv[1] = (short)mci->mcd->p1[1];
        tv[2] = (short)mci->mcd->p1[2];
        tv[3] = 0.0f;

        RotFvector(plyr_wrk.move_box.rot, tv);
    }

    sceVu0AddVector(tv2, plyr_wrk.move_box.pos, tv);

    tc->i[0] = tv2[0];
    tc->i[1] = tv2[1] + -400.0f;
    tc->i[2] = tv2[2];
    tc->i[3] = tv2[3];

    tv2[0] = (short)mci->mcd->p0[0];
    tv2[1] = (short)mci->mcd->p0[1];
    tv2[2] = (short)mci->mcd->p0[2];
    tv2[3] = 0.0f;

    sceVu0AddVector(tc->p, plyr_wrk.move_box.pos, tv2);

    tc->roll = mci->mcd->roll[0];
    tc->fov = mci->mcd->fov[0];
}

float GetMCLocalPosPer(u_short cn, u_char kind, u_char id)
{
    u_short xmin;
    u_short xmax;
    u_short zmin;
    u_short zmax;
    u_char kind_tbl[4] = { 1, 2, 3, 4 };
    static float min;
    static float max;
    static u_short req_cam_no_save = 0xFFFF;
    static u_char req_cam_kind_save = 0xFF;

    if (id == 0xff)
    {
        req_cam_no_save = -1;
        req_cam_kind_save = 0xff;

        return 0.0f;
    }

    if (cn != req_cam_no_save || kind != req_cam_kind_save)
    {
        req_cam_no_save = cn;
        req_cam_kind_save = kind;

        MapCameraCdivideAB(cn, &xmin, &xmax, &zmin, &zmax, kind_tbl[kind]);

        if (id == 0)
        {
            min = xmin;
            max = xmax;
        }
        else
        {
            min = zmin;
            max = zmax;
        }
    }

    if (id == 0)
    {
        return __builtin_fabsf((plyr_wrk.bwp[0] - min) / (max - min));
    }

    return __builtin_fabsf((plyr_wrk.bwp[2] - min) / (max - min));
}

void CompleCameraPos(SgCAMERA *tc, SgCAMERA *oc, MAP_CAM_INFO *mci)
{
    sceVu0FVECTOR tc0;
    sceVu0FVECTOR tc1;
    float comple_div;
    float tca0;
    float tca1;
    u_char i;
    u_char chk;
    u_int *tbl;
    static u_char rmax;
    static u_char pmax;
    static u_char comple_cnt;
    static u_char comple_spd_dwn;
    static u_char comple_dwn_tm;
    static sceVu0FVECTOR comple_adjr = { 0.0f, 0.0f, 0.0f, 0.0f };
    static sceVu0FVECTOR comple_adjp = { 0.0f, 0.0f, 0.0f, 0.0f };
    static float comple_adjfov = 0.0f;
    static float comple_adjroll = 0.0f;

    if (CompleReqChk(mci) != 0)
    {
        switch (mci->kind)
        {
        case 0:
            switch(ingame_wrk.msn_no)
            {
            case 0:
                tbl = ncam_comple_dat1[mci->no_old];
            break;
            case 1:
                tbl = ncam_comple_dat1[mci->no_old];
            break;
            case 2:
                tbl = ncam_comple_dat2[mci->no_old];
            break;
            case 3:
                tbl = ncam_comple_dat3[mci->no_old];
            break;
            case 4:
                tbl = ncam_comple_dat4[mci->no_old];
            break;
            }
        break;
        case 1:
            switch(ingame_wrk.msn_no)
            {
            case 0:
                tbl = bcam_comple_dat1[mci->no_old];
            break;
            case 1:
                tbl = bcam_comple_dat1[mci->no_old];
            break;
            case 2:
                tbl = bcam_comple_dat2[mci->no_old];
            break;
            case 3:
                tbl = bcam_comple_dat3[mci->no_old];
            break;
            case 4:
                tbl = bcam_comple_dat4[mci->no_old];
            break;
            }
        break;
        case 2:
            switch(ingame_wrk.msn_no)
            {
            case 0:
                tbl = dcam_comple_dat1[mci->no_old];
            break;
            case 1:
                tbl = dcam_comple_dat1[mci->no_old];
            break;
            case 2:
                tbl = dcam_comple_dat2[mci->no_old];
            break;
            case 3:
                tbl = dcam_comple_dat3[mci->no_old];
            break;
            case 4:
                tbl = dcam_comple_dat4[mci->no_old];
            break;
            }
        break;
        }

        for (i = 0; tbl[i] != 0xffffffff; i++)
        {
            if (((tbl[i] >> 8) & 0x1ff) == mci->no)
            {
                if (tbl[i] & 0x60000000)
                {
                    switch((tbl[i] >> 28) & 0x3)
                    {
                    case 1:
                        CallFocus(0, 1, 50);
                    break;
                    case 2:
                        CallFocus(0, 2, 70);
                    break;
                    case 3:
                        CallFocus(0, 3, 100);
                    break;
                    }
                }

                if (tbl[i] & 0xff)
                {
                    compling = mci->no;

                    comple_div = tbl[i] & 0xff;
                    comple_cnt = comple_div;
                    comple_spd_dwn = tbl[i] >> 30;

                    if (comple_spd_dwn != 0)
                    {
                        comple_dwn_tm = comple_cnt / 2;
                    }

                    sceVu0SubVector(comple_adjr, tc->i, camera.i);
                    sceVu0DivVector(comple_adjr, comple_adjr, comple_div);

                    rmax = (__builtin_fabsf(comple_adjr[0]) > __builtin_fabsf(comple_adjr[1])) ? 0 : 1;
                    rmax = (__builtin_fabsf(comple_adjr[rmax]) > __builtin_fabsf(comple_adjr[2])) ? rmax : 2;

                    sceVu0SubVector(comple_adjp, tc->p, camera.p);
                    sceVu0DivVector(comple_adjp, comple_adjp, comple_div);

                    pmax = (__builtin_fabsf(comple_adjr[0]) > __builtin_fabsf(comple_adjr[1])) ? 0 : 1;
                    pmax = (__builtin_fabsf(comple_adjr[pmax]) > __builtin_fabsf(comple_adjr[2])) ? pmax : 2;


                    comple_adjfov = (tc->fov - camera.fov);
                    comple_adjfov /= comple_div;

                    comple_adjroll = tc->roll - camera.roll;

                    RotLimitChk(&comple_adjroll);

                    comple_adjroll = comple_adjroll / comple_div;
                }
                break;
            }
        }
    }
    else
    {
        if (mci->type == 2 || mci->type == 3 || mci->type == 4)
        {
            if (comple_spd_dwn == 3)
            {
                sceVu0SubVector(comple_adjp, tc->p, oc->p);
            }
            else
            {
                tc0[0] = 0.0f;
                tc0[1] = 0.0f;
                tc0[2] = 0.0f;
                tc0[3] = 0.0f;

                tc1[0] = comple_adjp[0];
                tc1[1] = comple_adjp[1];
                tc1[2] = comple_adjp[2];
                tc1[3] = comple_adjp[3];

                tca0 = GetDistV(tc0, tc1);
                sceVu0SubVector(tc1, tc->p, camera.p);

                tc0[0] = 0.0f;
                tc0[1] = 0.0f;
                tc0[2] = 0.0f;
                tc0[3] = 0.0f;

                tca1 = GetDistV(tc0, tc1);
                sceVu0DivVector(comple_adjp, tc1, tca1 / tca0);
            }
            if (mci->type == 0x4)
            {

                if (comple_spd_dwn == 3)
                {
                    sceVu0SubVector(comple_adjr, tc->i, oc->i);
                }
                else
                {
                    tc0[0] = 0.0f;
                    tc0[1] = 0.0f;
                    tc0[2] = 0.0f;
                    tc0[3] = 0.0f;

                    tc1[0] = comple_adjr[0];
                    tc1[1] = comple_adjr[1];
                    tc1[2] = comple_adjr[2];
                    tc1[3] = comple_adjr[3];

                    tca0 = GetDistV(tc0, tc1);
                    sceVu0SubVector(tc1, tc->i, camera.i);

                    tc0[0] = 0.0f;
                    tc0[1] = 0.0f;
                    tc0[2] = 0.0f;
                    tc0[3] = 0.0f;

                    tca1 = GetDistV(tc0, tc1);
                    sceVu0DivVector(comple_adjr, tc1, tca1 / tca0);
                }
            }
        }
    }

    if (compling == 0xffff)
    {
        return;
    }

    if (((comple_spd_dwn & 0x1) && (plyr_wrk.mvsta & 0xf) == 0) || comple_spd_dwn == 2)
    {
        if (comple_cnt <= comple_dwn_tm)
        {
            if (__builtin_fabsf(comple_adjr[rmax]) > 0.1f)
            {
                sceVu0ScaleVector(comple_adjr, comple_adjr, 0.98f);
            }

            if (__builtin_fabsf(comple_adjp[pmax]) > 0.1f)
            {
                sceVu0ScaleVector(comple_adjp, comple_adjp, 0.98f);
            }
        }
    }

    chk = 0;

    tc0[0] = tc->i[0];
    tc0[1] = tc->i[1];
    tc0[2] = tc->i[2];
    tc0[3] = tc->i[3];

    for (i = 0; i < 3; i++)
    {
        if (comple_adjr[i] > 0.0f && tc->i[i] > camera.i[i])
        {
            tc->i[i] = camera.i[i] + comple_adjr[i];

            if (tc0[i] <= tc->i[i])
            {
                tc->i[0] = tc0[0];
                tc->i[1] = tc0[1];
                tc->i[2] = tc0[2];
                tc->i[3] = tc0[3];

                comple_adjr[0] = 0.0f;
                comple_adjr[1] = 0.0f;
                comple_adjr[2] = 0.0f;
                comple_adjr[3] = 0.0f;
                break;
            }

            chk = 1;
        }
        else
        {
            if (comple_adjr[i] < 0.0f && tc->i[i] < camera.i[i])
            {
                tc->i[i] = camera.i[i] + comple_adjr[i];

                if (tc->i[i] <= tc0[i])
                {
                    tc->i[0] = tc0[0];
                    tc->i[1] = tc0[1];
                    tc->i[2] = tc0[2];
                    tc->i[3] = tc0[3];

                    comple_adjr[0] = 0.0f;
                    comple_adjr[1] = 0.0f;
                    comple_adjr[2] = 0.0f;
                    comple_adjr[3] = 0.0f;
                    break;
                }

                chk = 1;
            }
        }
    }

    tc0[0] = tc->p[0];
    tc0[1] = tc->p[1];
    tc0[2] = tc->p[2];
    tc0[3] = tc->p[3];

    for (i = 0; i < 3; i++)
    {
        if (comple_adjp[i] > 0.0f && tc->p[i] > camera.p[i])
        {
            tc->p[i] = camera.p[i] + comple_adjp[i];

            if (tc0[i] <= tc->p[i])
            {
                tc->p[0] = tc0[0];
                tc->p[1] = tc0[1];
                tc->p[2] = tc0[2];
                tc->p[3] = tc0[3];

                comple_adjp[0] = 0.0f;
                comple_adjp[1] = 0.0f;
                comple_adjp[2] = 0.0f;
                comple_adjp[3] = 0.0f;
                break;
            }

            chk = 1;
        }
        else
        {
            if (comple_adjp[i] < 0.0f && tc->p[i] < camera.p[i])
            {
                tc->p[i] = camera.p[i] + comple_adjp[i];

                if (tc->p[i] <= tc0[i])
                {
                    tc->p[0] = tc0[0];
                    tc->p[1] = tc0[1];
                    tc->p[2] = tc0[2];
                    tc->p[3] = tc0[3];

                    comple_adjp[0] = 0.0f;
                    comple_adjp[1] = 0.0f;
                    comple_adjp[2] = 0.0f;
                    comple_adjp[3] = 0.0f;
                    break;
                }
                chk = 1;
            }
        }
    }

    if ((comple_adjfov > 0.0f && tc->fov > camera.fov) || (comple_adjfov < 0.0f && tc->fov < camera.fov))
    {
        tc->fov = camera.fov + comple_adjfov;
        chk = 1;
    }
    else
    {
        comple_adjfov = 0.0f;
    }

    if ((comple_adjroll > 0.0f && tc->roll > camera.roll) ||  (comple_adjroll < 0.0f && tc->roll < camera.roll))
    {
        tc->roll = camera.roll + comple_adjroll;
        chk = 1;
    }
    else
    {
        comple_adjroll = 0.0f;
    }

    if (chk)
    {
        if (comple_cnt != 0)
        {
            comple_cnt--;
        }
    }
    else
    {
        compling = -1;
    }
}

u_char CompleReqChk(MAP_CAM_INFO *mci)
{
    u_char req;

    req = 0;

    if (mci->kind == 3)
    {
        return 0;
    }

    if (compling != 0xFFFF)
    {
        if (compling != mci->no)
        {
            compling = 0xFFFF;
        }
    }

    if (compling == 0xFFFF)
    {
        if (mci->change != 0)
        {
            if (mci->no_old != 0xFFFF)
            {
                req = 1;
            }
        }
    }

    return req;
}

void FinderModeCameraCtrl()
{
    MOVE_BOX *mb;
    sceVu0FVECTOR tv;
    sceVu0FVECTOR rv;

    mb = &plyr_wrk.move_box;

    if (plyr_wrk.move_mot == 11)
    {
        tv[0] = 0.0f;
        tv[1] = -575.0f;
        tv[2] = 0.0f;
        tv[3] = 0.0f;
    }
    else
    {
        tv[0] = 0.0f;
        tv[1] = -687.0f;
        tv[2] = 0.0f;
        tv[3] = 0.0f;
    }

    RotFvector(mb->rot, tv);
    sceVu0AddVector(camera.p, mb->pos, tv);

    if (plyr_wrk.sta & 0x1000)
    {
        if ((ene_wrk[plyr_wrk.rock_trgt].sta & 0x1) != 0 && ene_wrk[plyr_wrk.rock_trgt].tr_rate != 0)
        {
            camera.i[0] = ene_wrk[plyr_wrk.rock_trgt].mpos.p0[0];
            camera.i[1] = ene_wrk[plyr_wrk.rock_trgt].mpos.p0[1];
            camera.i[2] = ene_wrk[plyr_wrk.rock_trgt].mpos.p0[2];
            camera.i[3] = ene_wrk[plyr_wrk.rock_trgt].mpos.p0[3];
        }

        camera.roll = PI;
        compling = -1;
        return;
    }
    else if (plyr_wrk.sta & 0x400)
    {
        plyr_wrk.frot_x += plyr_wrk.move_box.rspd[0];

        RotLimitChk(&plyr_wrk.frot_x);

        plyr_wrk.move_box.rot[1] += plyr_wrk.move_box.rspd[1];

        RotLimitChk(&plyr_wrk.move_box.rot[1]);

        plyr_wrk.move_box.rspd[0] *= 1.1f;
        plyr_wrk.move_box.rspd[1] *= 1.1f;

        plyr_wrk.move_box.loop -= 1;

        if (plyr_wrk.move_box.loop == 0)
        {
            plyr_wrk.sta &= ~0x400;

            plyr_wrk.move_box.rspd[0] = 0.0f;
            plyr_wrk.move_box.rspd[1] = 0.0f;
            plyr_wrk.move_box.rspd[2] = 0.0f;
            plyr_wrk.move_box.rspd[3] = 0.0f;
        }
    }
    else if (plyr_wrk.sta & 0x8000000)
    {
        if (plyr_wrk.move_box.loop == 0)
        {
            plyr_wrk.sta &= ~0x8000000;

            plyr_wrk.move_box.rspd[0] = 0.0f;
            plyr_wrk.move_box.rspd[1] = 0.0f;
            plyr_wrk.move_box.rspd[2] = 0.0f;
            plyr_wrk.move_box.rspd[3] = 0.0f;
        }
        else
        {
            plyr_wrk.move_box.loop -= 1;

            sceVu0AddVector(mb->rot, mb->rot, mb->rspd);
            RotLimitChk(&mb->rot[1]);
        }
    }
    else
    {
        PlyrCamCondChk();
        SetFinderRot();
    }

    tv[0] = 0.0f;
    tv[1] = 0.0f;
    tv[2] = 1000.0f;
    tv[3] = 0.0f;

    rv[0] = plyr_wrk.frot_x;
    rv[1] = mb->rot[1];
    rv[2] = 0.0f;
    rv[3] = 0.0f;

    RotFvector(rv, tv);
    sceVu0AddVector(camera.i, camera.p, tv);

    camera.roll = PI;

    compling = -1;
}

void PlyrCamCondChk()
{
    switch (plyr_wrk.cond)
    {
    case 1:
        PconMahiCameraCtrl();
    break;
    case 3:
    case 5:
        PconTebureCameraCtrl();
    break;
    }
}

void PconMahiCameraCtrl()
{
    MOVE_BOX *mb;
    static u_char time = 0;
    static u_char flag = 0;

    mb = &plyr_wrk.move_box;

    if (time == 0)
    {
        time = 3;
        flag ^= 1;
    }
    else
    {
        time --;
    }

    if (flag == 0)
    {
        mb->rot[1] += DEG2RAD(0.1f);
    }
    else
    {
        mb->rot[1] -= DEG2RAD(0.1f);
    }

    RotLimitChk(&mb->rot[1]);
}

void PconTebureCameraCtrl()
{
    MOVE_BOX *mb;
    float ax;
    float ay;
    char pad_x;
    char pad_y;
    u_char jpad_on;
    static float rot;
    static float rot_adj = 0.0f;
    static float spd;
    static u_char flag = 0;

    mb = &plyr_wrk.move_box;

    if (FinderModePadChk(&pad_x, &pad_y, &ax, &ay, &jpad_on) != 0)
    {
        spd = GetDist(ax, ay) * 0.0003f;
        rot = SgAtan2f(ax, -ay);

        flag = 1;
    }
    else
    {
        if (flag == 0)
        {
            if (rot >= 0.0f)
            {
                rot_adj = rot_adj + DEG2RAD(0.1f);
            }
            else
            {
                rot_adj = rot_adj - DEG2RAD(0.1f);
            }

            rot += rot_adj;
            spd = rot_adj * 0.05f;

            RotLimitChk(&rot);
        }
    }

    ax = spd * SgSinf(rot);
    ay = spd * SgCosf(rot);

    mb->rot[1] += ax;

    RotLimitChk(&mb->rot[1]);

    plyr_wrk.frot_x += ay;

    RotLimitChk(&plyr_wrk.frot_x);

    if (plyr_wrk.frot_x > DEG2RAD(60.0f))
    {
        plyr_wrk.frot_x = DEG2RAD(60.0f);
    }
    else if (plyr_wrk.frot_x < DEG2RAD(-60.0f))
    {
        plyr_wrk.frot_x = DEG2RAD(-60.0f);
    }

    if (flag != 0)
    {
        spd *= 0.95f;

        if (spd <= DEG2RAD(0.01f))
        {
            rot_adj = 0.0f;
            flag = 0;
        }
    }
}

u_short fior_tm = 0;

int FinderModePadChk(char *pad_x, char *pad_y, float *ax, float *ay, u_char *jpad_on)
{
    *pad_y = 0;
    *pad_x = 0;
    *jpad_on = 0;

    *ax = *ay = 0.0f;

    if (*key_now[0] != 0 || *key_now[1] != 0 || *key_now[3] != 0 || *key_now[2] != 0)
    {
        *jpad_on = 1;
    }

    if (pad[0].id == 0x79 || *jpad_on != 0)
    {
        if (*key_now[0] != 0)
        {
            *ay = -40.0f;
        }
        else if (*key_now[1] != 0)
        {
            *ay = 40.0f;
        }

        if (*key_now[3] != 0)
        {
            *ax = 40.0f;
        }
        else if (*key_now[2] != 0)
        {
            *ax = -40.0f;
        }

        if (*ax == 0.0f && *ay == 0.0f)
        {
            *ax = pad[0].analog[2] - 128.0f;
            *ay = pad[0].analog[3] - 128.0f;
        }

        if (*ax >= 40.0f)
        {
            *pad_x = 1;
        }
        else if (*ax <= -40.0f)
        {
            *pad_x = -1;
        }

        if (*ay >= 40.0f)
        {
            *pad_y = 1;
        }
        else if (*ay <= -40.0f)
        {
            *pad_y = -1;
        }

        if (*pad_y != 0 && (opt_wrk.key_type == 4 || opt_wrk.key_type == 5 || opt_wrk.key_type == 6 || opt_wrk.key_type  == 7))
        {
            *pad_y = -*pad_y;
        }
    }

    return *pad_x != 0 || *pad_y != 0;
}

void SetFinderRot()
{
    MOVE_BOX *mb;
    float delta;
    float dist;
    float spd;
    float ax;
    float ay;
    float rot;
    char pad_x;
    char pad_y;
    u_char jpad_on;

    mb = &plyr_wrk.move_box;

    rot = 10.0f;

    if (plyr_wrk.cond == 1)
    {
        return;
    }

    if (FinderModePadChk(&pad_x, &pad_y, &ax, &ay, &jpad_on))
    {
        rot = SgAtan2f(ax, ay);
        dist = GetDist(ax, ay);
        spd = 2.0f;
    }
    else
    {
        if (plyr_wrk.fp[0] != 0x140 || plyr_wrk.fp[1] != 0xd1)
        {
            rot = SgAtan2f(0x140 - plyr_wrk.fp[0], 0xd1 - plyr_wrk.fp[1]);
            spd = 1.9f;
        }
    }

    if (rot != 10.0f)
    {
        ax = spd * SgSinf(rot);
        ay = spd * SgCosf(rot);

        plyr_wrk.fp[0] += (short)ax;

        if (spd == 2.0f)
        {
            if (plyr_wrk.fp[0] < 300)
            {
                plyr_wrk.fp[0] = 300;
            }
            else if (0x154 < plyr_wrk.fp[0])
            {
                plyr_wrk.fp[0] = 0x154;
            }
        }
        else
        {
            if (ax > 0.0f && plyr_wrk.fp[0] >= 0x141)
            {
                plyr_wrk.fp[0] = 0x140;

            }
            else if (ax < 0.0f && plyr_wrk.fp[0] < 0x140)
            {
                plyr_wrk.fp[0] = 0x140;
            }
        }

        plyr_wrk.fp[1] += (short)ay;

        if (spd == 2.0f)
        {
            if (plyr_wrk.fp[1] < 0xc2)
            {
                plyr_wrk.fp[1] = 0xc2;
            }
            else if (0xe0 < plyr_wrk.fp[1])
            {
                plyr_wrk.fp[1] = 0xe0;
            }
        }
        else
        {
            if (ay > 0.0f && plyr_wrk.fp[1] >= 0xd2)
            {
                plyr_wrk.fp[1] = 0xd1;

            }
            else if (ay < 0.0f && plyr_wrk.fp[1] < 0xd1)
            {
                plyr_wrk.fp[1] = 0xd1;
            }
        }
    }

    if (cam_custom_wrk.set_spe == 2)
    {
        if (*key_now[9] != 0)
        {
            camera.fov -= 0.01f;

            if (camera.fov < 0.45f)
            {
                camera.fov = 0.45f;
            }
        }
        else if (*key_now[0xb] != 0)
        {
            camera.fov += 0.01f;

            if ((PI / 2) < camera.fov)
            {
                camera.fov = (PI / 2);
            }
        }
    }

    if (*key_now[6] != 0)
    {
        delta = DEG2RAD(1.8f);
    }
    else
    {
        if (dist > 125.0f || jpad_on != 0)
        {
            delta = DEG2RAD(0.9f);
        }
        else
        {
            delta = DEG2RAD(0.2f);
        }
    }

#ifdef BUILD_EU_VERSION
    delta *= sys_wrk.move_conv;
#endif

    if (pad_x < 0)
    {
        mb->rot[1] -= delta;
    }
    else if (pad_x > 0)
    {
        mb->rot[1] += delta;
    }

    RotLimitChk(&mb->rot[1]);

    if (pad_y < 0)
    {
        plyr_wrk.frot_x += delta;

        if (DEG2RAD(60.0f) < plyr_wrk.frot_x)
        {
            plyr_wrk.frot_x = DEG2RAD(60.0f);
        }
    }
    else if (pad_y > 0)
    {
        plyr_wrk.frot_x -= delta;

        if (plyr_wrk.frot_x < DEG2RAD(-60.0f))
        {
            plyr_wrk.frot_x = DEG2RAD(-60.0f);
        }
    }

    RotLimitChk(&plyr_wrk.frot_x);
}

void CameraIdMoveCtrl()
{
    MAP_CAM_DAT *mcd;
    sceVu0FVECTOR tv = { 0.0f, 0.0f, 0.0f, 0.0f };
    sceVu0FVECTOR rav = { 0.0f, 0.0f, 1000.0f, 0.0f };
    float delta = DEG2RAD(1.0f);
    u_short cn;
    char *str = "NORMAL       ";
    char *str5 = "BATTLE       ";
    char *str6 = "DRAMA        ";
    char *str7 = "DOOR         ";
    char *str0 = "CAMERA TYPE  ";
    char *str1 = "STEP         ";
    char *str2 = "MOVE ID      ";
    char *str3 = "NEW DATA SET!!";
    char *str4 = "MISSION NO   ";
    char *str_save = "FILE SAVE MODE";
    char *str_renewal = "RENEWAL DATA!!";
    char *str_norenewal = "NO RENEWAL DATA";

    if (*key_now[15] == 1)
    {
        cam_info_disp ^= 1;
    }

    switch (cam_kind)
    {
    case 0:
        cn = plyr_wrk.pr_info.camera_no;
        mcd = &map_cam_dat[cn];

        if (cam_info_disp != 0)
        {
            SetASCIIString2(0, 430.0f, 10.0f, 1, 0xdc, 0xdc, 0x32, str);
            SetInteger2(0, 540.0f, 10.0f, 1, 0xff, 0xff, 0x32, cn);
        }
    break;
    case 1:
        cn = plyr_wrk.pr_info.camera_btl;
        mcd = &map_cam_dat2[cn];

        if (cam_info_disp != 0)
        {
            SetASCIIString2(0, 430.0f, 10.0f, 1, 0xdc, 0x64, 0x64, str5);
            SetInteger2(0, 540.0f, 10.0f, 1, 0xff, 0x64, 0x64, cn);
        }
    break;
    case 2:
        cn = plyr_wrk.pr_info.camera_drm;
        mcd = &map_cam_dat3[cn];

        if (cam_info_disp != 0)
        {
            SetASCIIString2(0, 430.0f, 10.0f, 1, 0x32, 0xdc, 0x32, str6);
            SetInteger2(0, 540.0f, 10.0f, 1, 0x32, 0xff, 0x32, cn);
        }
    break;
    case 3:
        cn = plyr_wrk.pr_info.camera_door;
        mcd = &map_cam_dat4[cn];

        if (cam_info_disp != 0)
        {
            SetASCIIString2(0, 430.0f, 10.0f, 1, 0x32, 0xdc, 0x32, str7);
            SetInteger2(0, 540.0f, 10.0f, 1, 0x32, 0xff, 0x32, cn);
        }
    break;
    }

    if (cam_info_disp != 0)
    {
        if (cam_type < 6)
        {
            SetASCIIString2(0, 430.0f, 30.0f, 1, 0x64, 0x64, 0xfa, str0);
            SetInteger2(0, 540.0f, 30.0f, 1, 0xff, 0xff, 0xff, cam_type);

            SetASCIIString2(0, 430.0f, 50.0f, 1, 0x64, 0x64, 0xfa, str1);
            SetInteger2(0, 540.0f, 50.0f, 1, 0xff, 0xff, 0xff, cd_step);

            SetASCIIString2(0, 430.0f, 70.0f, 1, 0x64, 0x64, 0xfa, str2);
            SetInteger2(0, 540.0f, 70.0f, 1, 0xff, 0xff, 0xff, cam_id);
        }
        else
        {
            SetASCIIString2(0, 430.0f, 70.0f, 1, 0x64, 0x64, 0xfa, str4);
            SetInteger2(0, 540.0f, 70.0f, 1, 0xff, 0xff, 0xff, msn_no);

            SetASCIIString2(0, 430.0f, 50.0f, 1, 0xf5, 0x67, 0xcb, str_save);
        }
    }

    if (*key_now[8] != 0 && *key_now[10] != 0 && *key_now[11] == 1)
    {
        switch (cdcopy[0])
        {
        case 0:
            map_cam_dat[cdcopy[1]] = *mcd;
        break;
        case 1:
            map_cam_dat2[cdcopy[1]] = *mcd;
        break;
        case 2:
            map_cam_dat3[cdcopy[1]] = *mcd;
        break;
        case 3:
            map_cam_dat4[cdcopy[1]] = *mcd;
        break;
        }

        renewal_data_chk = 1;
    }

    if (*key_now[9] == 1 && *key_now[8] != 0)
    {
        cam_kind++;

        if (cam_kind > 3)
        {
            cam_kind = 0;
        }

        cd_step = 0;
        cam_id = 0;
    }
    else
    {
        if (cd_step == 0)
        {
            if (*key_now[9] == 1 && *key_now[8] == 0)
            {
                if (cam_type > 5)
                {
                    msn_no++;
                }

                cam_type++;

                if (cam_type > 8)
                {
                    cam_type = 0;
                    msn_no = 0;
                }

                cd_edit_end = 0;
            }
        }
        else
        {
            if (*key_now[9] == 1 && *key_now[8] == 0)
            {
                cam_id ^= 1;
            }
        }
    }

    if (*key_now[8] != 0 && *key_now[14] == 1)
    {
        cd_step = 0;
    }

    if (*key_now[13] == 1)
    {
        switch(cam_type)
        {
        case 0:
            cd_edit_end = SetMapCamDat0(mcd);
        break;
        case 1:
            cd_edit_end = SetMapCamDat1(mcd);
        break;
        case 2:
            cd_edit_end = SetMapCamDat2(mcd, cam_id);
        break;
        case 3:
            cd_edit_end = SetMapCamDat3(mcd, cam_id);
        break;
        case 4:
            cd_edit_end = SetMapCamDat4(mcd, cam_id);
        break;
        case 5:
            cd_edit_end = SetMapCamDat5(mcd);
        break;
        default:
            if (renewal_data_chk != 0)
            {
                SaveCamDat(cam_kind, msn_no);
            }

            cd_edit_end = 1;
        break;
        }
    }

    if (cd_edit_end != 0)
    {
        if (cam_type < 6)
        {
            SetASCIIString2(0, 430.0f, 90.0f, 1, 0xff, 0x32, 0x32, str3);
            renewal_data_chk = 1;
            renewal_data_chk_cnt = 0;
            GetMCLocalPosPer(0, 0x0, 0xff);
        }
        else
        {
            if (renewal_data_chk != 0)
            {
                SetASCIIString2(0, 430.0f, 90.0f, 1, 0xf0, 0x32, 0x32, str_renewal);

                if (renewal_data_chk_cnt > 40)
                {
                    renewal_data_chk = 0;
                    cd_edit_end = 0;
                }

                renewal_data_chk_cnt++;
            }
            else
            {
                SetASCIIString2(0, 430.0f, 90.0f, 1, 0xdc, 0xdc, 0x32, str_norenewal);
            }
        }

        cd_step = 0;
        cam_id = 0;
    }

    if (dbg_wrk.mode_on == 0)
    {
        if (*key_now[10] != 0)
        {
            tv[2] = 20.0f;
        }
        else
        {
            if (*key_now[11] != 0)
            {
                tv[2] = -20.0f;
            }
        }

        if (*key_now[8] != 0)
        {
            if (*key_now[3] != 0)
            {
                tv[0] = 20.0f;
            }
            else if (*key_now[2] != 0)
            {
                tv[0] = -20.0f;
            }

            if (*key_now[0] != 0)
            {
                tv[1] = -20.0f;
            }
            else if (*key_now[1] != 0)
            {
                tv[1] = 20.0f;
            }
        }
        else if (*key_now[5] != 0)
        {
            if (*key_now[3] != 0)
            {
                camera.roll -= delta;

                if (camera.roll < -PI)
                {
                    camera.roll += PI * 2;
                }
            }
            else if (*key_now[2] != 0)
            {
                camera.roll += delta;

                if (PI < camera.roll)
                {
                    camera.roll -= PI * 2;
                }
            }
            else if (*key_now[4] != 0)
            {
                camera.roll = PI;
            }
        }
        else if (*key_now[6] != 0)
        {
            if (*key_now[0] != 0)
            {
                camera.fov -= 0.01f;

                if (camera.fov < 0.1f)
                {
                    camera.fov = 0.1f;
                }
            }
            else if (*key_now[1] != 0)
            {
                camera.fov += 0.01f;

                if (camera.fov > PI)
                {
                    camera.fov = PI;
                }
            }
            else if (*key_now[4] != 0)
            {
                camera.fov = DEG2RAD(60.0f);
            }
        }
        else
        {
            if (*key_now[3] != 0)
            {
                cam_id_move.rot_y[1] += delta;

                if (PI < cam_id_move.rot_y[1])
                {
                    cam_id_move.rot_y[1] -= PI * 2;
                }

                cd_edit_end = 0x0;
            }
            else if (*key_now[2] != 0)
            {
                cam_id_move.rot_y[1] -= delta;

                if (cam_id_move.rot_y[1] < -PI)
                {
                    cam_id_move.rot_y[1] += PI * 2;
                }

                cd_edit_end = 0x0;
            }

            if (*key_now[0] != 0)
            {
                cam_id_move.rot_x[0] += delta;

                if (cam_id_move.rot_x[0] > DEG2RAD(72.0f))
                {
                    cam_id_move.rot_x[0] = DEG2RAD(72.0f);
                }

                cd_edit_end = 0;
            }
            else if (*key_now[1] != 0)
            {
                cam_id_move.rot_x[0] -= delta;

                if (cam_id_move.rot_x[0] < -DEG2RAD(72.0f))
                {
                    cam_id_move.rot_x[0] = -DEG2RAD(72.0f);
                }

                cd_edit_end = 0;
            }
        }

        if (tv[0] != 0.0f || tv[1] != 0.0f || tv[2] != 0.0f)
        {
            cd_edit_end = 0;
        }
    }

    RotFvector(cam_id_move.rot_x, tv);
    RotFvector(cam_id_move.rot_y, tv);
    RotFvector(cam_id_move.rot_x, rav);
    RotFvector(cam_id_move.rot_y, rav);

    sceVu0AddVector(cam_id_move.p, cam_id_move.p, tv);
    sceVu0AddVector(cam_id_move.i, cam_id_move.p, rav);

    camera.i[0] = cam_id_move.i[0];
    camera.i[1] = cam_id_move.i[1];
    camera.i[2] = cam_id_move.i[2];
    camera.i[3] = cam_id_move.i[3];

    camera.p[0] = cam_id_move.p[0];
    camera.p[1] = cam_id_move.p[1];
    camera.p[2] = cam_id_move.p[2];
    camera.p[3] = cam_id_move.p[3];
}

u_char SetMapCamDat0(MAP_CAM_DAT *mcd)
{
    u_char i;

    mcd->type = 0;
    mcd->id = 0;

    for (i = 0; i < 3; i++)
    {
        mcd->p0[i] = camera.i[i];
    }

    for (i = 0; i < 3; i++)
    {
        mcd->p1[i] = camera.p[i];
    }

    for (i = 0; i < 3; i++)
    {
        mcd->p2[i] = 0;
    }

    for (i = 0; i < 3; i++)
    {
        mcd->p3[i] = 0;
    }

    mcd->roll[0] = camera.roll;
    mcd->roll[1] = 0.0f;

    mcd->fov[0] = camera.fov;
    mcd->fov[1] = 0.0f;

    return 1;
}

u_char SetMapCamDat1(MAP_CAM_DAT *mcd)
{
    u_char i;

    mcd->type = 1;
    mcd->id = 0;

    for (i = 0; i < 3; i++)
    {
        mcd->p0[i] = camera.p[i];
    }

    for (i = 0; i < 3; i++)
    {
        mcd->p1[i] = plyr_adj[i];
    }

    for (i = 0; i < 3; i++)
    {
        mcd->p2[i] = 0;
    }

    for (i = 0; i < 3; i++)
    {
        mcd->p3[i] = 0;
    }

    mcd->roll[0] = camera.roll;
    mcd->roll[1] = 0.0f;

    mcd->fov[0] = camera.fov;
    mcd->fov[1] = 0.0f;

    return 0x1;
}

u_char SetMapCamDat2(MAP_CAM_DAT *mcd, u_char id)
{
    u_char i;
    u_char end;
    static MAP_CAM_DAT tmpd;

    end = 0;

    switch (cd_step)
    {
    case 0:
            tmpd = (MAP_CAM_DAT){0};

            mcd->type = 2;

            if (*key_now[8] == 0)
            {
                for (i = 0; i < 3; i++)
                {
                    tmpd.p0[i] = (u_short)(int)camera.i[i];
                }

                for (i = 0; i < 3; i++)
                {
                    tmpd.p1[i] = camera.p[i];
                }

                mcd->roll[0] = camera.roll;
                mcd->fov[0] = camera.fov;
            }

            cd_step++;
    break;
    case 1:
        if (*key_now[8] == 0)
        {
            for (i = 0; i < 3; i++)
            {
                tmpd.p2[i] = camera.p[i];
            }

            if (mcd->roll[0] != camera.roll)
            {
                mcd->roll[1] = camera.roll;
            }
            else
            {
                mcd->roll[1] = 0.0f;
            }

            if (mcd->fov[0] != camera.fov)
            {
                mcd->fov[1] = camera.fov;
            }
            else
            {
                mcd->fov[1] = 0.0f;
            }
        }

        cd_step++;
    break;
    case 2:
        mcd->id = id;

        if (tmpd.p0[0] != 0 || tmpd.p0[1] != 0 || tmpd.p0[2] != 0)
        {
            for (i = 0; i < 3; i++)
            {
                mcd->p0[i] = tmpd.p0[i];
            }
        }

        if (tmpd.p1[0] != 0 || tmpd.p1[1] != 0 || tmpd.p1[2] != 0)
        {
            for (i = 0; i < 3; i++)
            {
                mcd->p1[i] = tmpd.p1[i];
            }
        }

        if (tmpd.p2[0] != 0 || tmpd.p2[1] != 0 || tmpd.p2[2] != 0)
        {
            for (i = 0; i < 3; i++)
            {
                mcd->p2[i] = tmpd.p2[i];
            }
        }

        for (i = 0; i < 3; i++)
        {
            mcd->p3[i] = 0;
        }

        end = 1;
    break;
    default:
        end = 1;
    break;
    }

    return end;
}

u_char SetMapCamDat3(MAP_CAM_DAT *mcd, u_char id)
{
    u_char i;
    u_char end;
    static MAP_CAM_DAT tmpd;

    end = 0;

    switch (cd_step)
    {
    case 0:
            tmpd = (MAP_CAM_DAT){0};

            mcd->type = 3;

            if (*key_now[8] == 0)
            {
                for (i = 0; i < 3; i++)
                {
                    tmpd.p0[i] = camera.p[i];
                }

                mcd->roll[0] = camera.roll;
                mcd->fov[0] = camera.fov;
            }

            cd_step++;
    break;
    case 1:
        if (*key_now[8] == 0)
        {
            for (i = 0; i < 3; i++)
            {
                tmpd.p1[i] = camera.p[i];
            }

            if (mcd->roll[0] != camera.roll)
            {
                mcd->roll[1] = camera.roll;
            }
            else
            {
                mcd->roll[1] = 0.0f;
            }

            if ( mcd->fov[0] != camera.fov)
            {
                mcd->fov[1] = camera.fov;
            }
            else
            {
                mcd->fov[1] = 0.0f;
            }
        }

        cd_step++;
    break;
    case 2:
        mcd->id = id;

        if (tmpd.p0[0] != 0 || tmpd.p0[1] != 0 || tmpd.p0[2] != 0)
        {
            for (i = 0; i < 3; i++)
            {
                mcd->p0[i] = tmpd.p0[i];
            }
        }

        if (tmpd.p1[0] != 0 || tmpd.p1[1] != 0 || tmpd.p1[2] != 0)
        {
            for (i = 0; i < 3; i++)
            {
                mcd->p1[i] = tmpd.p1[i];
            }
        }

        for (i = 0; i < 3; i++)
        {
            mcd->p2[i] = plyr_adj[i];
        }

        for (i = 0; i < 3; i++)
        {
            mcd->p3[i] = 0;
        }

        end = 1;
    break;
    default:
        end = 1;
    break;
    }

    return end;
}

u_char SetMapCamDat4(MAP_CAM_DAT *mcd, u_char id)
{
    u_char i;
    u_char end;
    static MAP_CAM_DAT tmpd;

    end = 0;

    switch (cd_step)
    {
    case 0:
            tmpd = (MAP_CAM_DAT){0};

            mcd->type = 4;

            if (*key_now[8] == 0)
            {
                for (i = 0; i < 3; i++)
                {
                    tmpd.p0[i] = camera.i[i];
                }

                for (i = 0; i < 3; i++)
                {
                    tmpd.p2[i] = camera.p[i];
                }

                mcd->roll[0] = camera.roll;
                mcd->fov[0] = camera.fov;
            }

            cd_step++;
    break;
    case 1:
        if (*key_now[8] == 0)
        {
            for (i = 0; i < 3; i++)
            {
                tmpd.p1[i] = (u_short)(int)camera.i[i];
            }

            for (i = 0; i < 3; i++)
            {
                tmpd.p3[i] = (u_short)(int)camera.p[i];
            }

            if (mcd->roll[0] != camera.roll)
            {
                mcd->roll[1] = camera.roll;
            }
            else
            {
                mcd->roll[1] = 0.0f;
            }

            if (mcd->fov[0] != camera.fov)
            {
                mcd->fov[1] = camera.fov;
            }
            else
            {
                mcd->fov[1] = 0.0f;
            }
        }

        cd_step++;
    break;
    case 2:
        mcd->id = id;

        if (tmpd.p0[0] != 0 || tmpd.p0[1] != 0 || tmpd.p0[2] != 0)
        {
            for (i = 0; i < 3; i++)
            {
                mcd->p0[i] = tmpd.p0[i];
            }
        }

        if (tmpd.p1[0] != 0 || tmpd.p1[1] != 0 || tmpd.p1[2] != 0)
        {
            for (i = 0; i < 3; i++)
            {
                mcd->p1[i] = tmpd.p1[i];
            }
        }

        if (tmpd.p2[0] != 0 || tmpd.p2[1] != 0 || tmpd.p2[2] != 0)
        {
            for (i = 0; i < 3; i++)
            {
                mcd->p2[i] = tmpd.p2[i];
            }
        }

        if (tmpd.p3[0] != 0 || tmpd.p3[1] != 0 || tmpd.p3[2] != 0)
        {
            for (i = 0; i < 3; i++)
            {
                mcd->p3[i] = tmpd.p3[i];
            }
        }

        end = 1;
    break;
    default:
        end = 1;
    break;
    }

    return end;
}

u_char SetMapCamDat5(MAP_CAM_DAT *mcd)
{
    u_char i;

    mcd->type = 5;
    mcd->id = 0;

    for (i = 0; i < 3; i++)
    {
        mcd->p0[i] = camera.p[i] - plyr_wrk.move_box.pos[i];
    }

    for (i = 0; i < 3; i++)
    {
        mcd->p1[i] = plyr_adj[i];
    }

    for (i = 0; i < 3; i++)
    {
        mcd->p2[i] = 0;
    }

    for (i = 0; i < 3; i++)
    {
        mcd->p3[i] = 0;
    }

    mcd->roll[0] = camera.roll;
    mcd->roll[1] = 0.0f;

    mcd->fov[0] = camera.fov;
    mcd->fov[1] = 0.0f;

    return 1;
}

void SaveCamDat(u_char kind, u_char mn)
{
    int fd;
    char fname[38] = "host0:../bin/data/camera/map_cam1.obj";
    char fname2[39] = "host0:../bin/data/camera/bmap_cam1.obj";
    char fname3[39] = "host0:../bin/data/camera/dmap_cam1.obj";
    char fname4[39] = "host0:../bin/data/camera/tmap_cam1.obj";

    switch (kind)
    {
    case 0:
        fname[32] = '1' + mn; // replace the '1' map_cam1 with 'mn + 1'
        fd = sceOpen(fname, 0x202);
        sceLseek(fd, 0, 0);
        sceWrite(fd, map_cam_dat, 22000);
    break;
    case 1:
        fname2[33] = '1' + mn; // replace the '1' bmap_cam1 with 'mn + 1'
        fd = sceOpen(fname2, 0x202);
        sceLseek(fd, 0, 0);
        sceWrite(fd, map_cam_dat2, 22000);
    break;
    case 2:
        fname3[33] = '1' + mn; // replace the '1' dmap_cam1 with 'mn + 1'
        fd = sceOpen(fname3, 0x202);
        sceLseek(fd, 0, 0);
        sceWrite(fd, map_cam_dat3, 0x3390);
    break;
    case 3:
        fname4[33] = '1' + mn; // replace the '1' tmap_cam1 with 'mn + 1'
        fd = sceOpen(fname4, 0x202);
        sceLseek(fd, 0, 0);
        sceWrite(fd, map_cam_dat4, 0x3390);
    break;
    }

    sceClose(fd);
}

void ReqFinderInOverRap(u_short time)
{
    fior_tm = time;
}

void FinderInOverRapCtrl()
{
    if (fior_tm != 0)
    {
        fior_tm--;

        SetEffects(EF_OVERLAP, 1, 30);
    }
}

void PlyrDmgCameraCtrl()
{
    sceVu0FVECTOR tr;
    sceVu0FVECTOR tv;
    static int cnt = 0;
    static sceVu0FVECTOR cv = { 0.0f, 0.0f, 0.0f, 0.0f };
    float dist;

    switch (plyr_wrk.dmg_cam_flag)
    {
    case 0:
        NormalCameraCtrl();

        camera.i[0] = plyr_wrk.move_box.pos[0];
        camera.i[1] = plyr_wrk.move_box.pos[1];
        camera.i[2] = plyr_wrk.move_box.pos[2];
        camera.i[3] = plyr_wrk.move_box.pos[3];

        camera.i[1] -= 500.0f;

        GetTrgtRotFromPlyr(camera.p, tr, 3);

        tv[0] = 0.0f;
        tv[1] = 0.0f;
        tv[2] = 700.0f;
        tv[3] = 0.0f;

        RotFvector(tr, tv);
        sceVu0AddVector(tv, camera.i, tv);

        dist = GetDistV2(tv, camera.p);
        cnt = dist / 60.0f;

        sceVu0SubVector(cv, tv, camera.p);
        sceVu0DivVector(cv, cv, cnt);

        plyr_wrk.dmg_cam_flag = 1;
    break;
    case 1:
        if (cnt == 0)
        {
            plyr_wrk.dmg_cam_flag = 2;
        }
        else
        {
            cnt--;

            sceVu0AddVector(camera.p, camera.p, cv);
        }
    break;
    case 2:
        // do nothing ...
    break;
    }

}

CAM_ID_MOVE cam_id_move = {0};

int cam_kind = 0;
int cam_type = 0;
int cd_step = 0;
int cam_id = 0;
short int plyr_adj[4] = {0};

void ReqDramaCamera(u_char req, u_short no, u_short time)
{
    u_short *tbl;
    u_char i;

    if (req != 0)
    {
        drm_cam_req = 1;
        drm_cam_tm = time;

        switch(ingame_wrk.msn_no)
        {
        case 0:
            tbl = dc_no_tbl0[no];
        break;
        case 1:
            tbl = dc_no_tbl0[no];
        break;
        case 2:
            tbl = dc_no_tbl1[no];
        break;
        case 3:
            tbl = dc_no_tbl2[no];
        break;
        case 4:
            tbl = dc_no_tbl3[no];
        break;
        }

        for (i = 0; i < 12; i++)
        {
            if (tbl[i] == 0xffff)
            {
                break;
            }

            drm_cam_no[i] = tbl[i];
        }
    }
    else
    {
        ClearDramaCamReq();
    }
}

void DramaCameraReqCtrl()
{
    u_char i;
    u_char chk;

    if (drm_cam_req != 0)
    {
        if (drm_cam_tm != 0)
        {
            drm_cam_tm--;

            if (drm_cam_tm == 0)
            {
                ClearDramaCamReq();
            }
        }

        if (drm_cam_req != 0)
        {
            for (i = 0, chk = 0; i < 12; i++)
            {
                if (drm_cam_no[i] == 0xffff)
                {
                    break;
                }

                if (plyr_wrk.pr_info.camera_drm == drm_cam_no[i])
                {
                    chk = 1;
                    break;
                }
            }

            if (!chk)
            {
                ClearDramaCamReq();
            }
        }
    }
}

void ClearDramaCamReq()
{
    u_char i;

    drm_cam_req = 0;
    drm_cam_tm = 0;

    for (i = 0; i < 12; i++)
    {
        drm_cam_no[i] = -1;
    }
}
