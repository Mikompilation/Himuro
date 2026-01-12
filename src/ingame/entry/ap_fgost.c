#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ap_fgost.h"

#include "main/glob.h"
#include "common/ul_math.h"
#include "os/eeiop/se_trans.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "ingame/plyr/unit_ctl.h"
#include "ingame/entry/entry.h"
#include "ingame/enemy/ene_ctl.h"
#include "ingame/entry/fgst_dat.h"
#include "ingame/entry/ap_dgost.h"
#include "ingame/event/ev_main.h"
#include "ingame/map/map_area.h"
#include "graphics/motion/motion.h"
#include "graphics/graph2d/effect_ene.h"

int load_mdl_addr[] = { 0xC80000, 0xD00000, 0xD80000, 0 };
int load_mot_addr[] = { 0xA30000, 0xAE0000, 0xB90000, 0 };
int load_se_addr[] = { 0x10, 0x11, 0x12, 0 };
FG_LOAD_WRK fg_load_wrk = {0};

#define ENEMY_ANM_ADDR 0x1330000
#define ENEMY_DMG_TEX_ADDR 0x13c8000
#define ENEMY_MDL_ADDR 0x13e0000
#define FLOAT_GHOST_SE_LOAD_ADDR 0x1460000

void FloatGhostAppearInit()
{
    int i;

    ap_wrk.fg_mode = 1;
    ap_wrk.fgst_no = -1;

    for (i = 0; i <= 2; i++)
    {
        ap_wrk.fg_pos[0][i] = -1;
        ap_wrk.fg_pos[1][i] = -1;
    }

    ap_wrk.stts |= 0x40;
}

int FloatGhostEntrySet(u_char area)
{
    return 0;
}

int FloatGhostAppearMain()
{
    u_char type;

    switch (ap_wrk.fg_mode)
    {
    case FG_COUNT:
        if (ap_wrk.fg_set_num != 0)
        {
            if (ingame_wrk.msn_no == 3)
            {
                ap_wrk.fg_ap += ap_wrk.ptime * 500;
            }
            else
            {
                ap_wrk.fg_ap += ap_wrk.ptime * 350;
            }
        }

        if (EnemyUseJudge(ET_FUYU))
        {
            if (ap_wrk.fg_ap < 950)
            {
                break;
            }
            ap_wrk.fg_ap = 950;
            break;
        }

        if (!FloatGhostAppearJudge())
        {
            break;
        }

        if (DeadGhostAppearJudge())
        {
            DeadGhostAppearReq();
            ap_wrk.fg_ap = 700;
            ap_wrk.fg_mode = FG_ENTRY;
            break;
        }
        ap_wrk.fg_mode = FG_WAIT;
        if (GetFloatGhostAppearPosType(&type) == 0 && plyr_wrk.mode != PMODE_FINDER)
        {
            break;
        }

        FloatGhostAppearPosSet(0xff, ene_wrk[EWRK_GHOST2].move_box.pos, ene_wrk[EWRK_GHOST2].move_box.rot);
        ene_wrk[EWRK_GHOST2].dat_no = fgst_ap_no[0][ap_wrk.fgst_cnt % 20 + ingame_wrk.msn_no * 20];
        ene_wrk[EWRK_GHOST2].sta = 2;
        ene_wrk[EWRK_GHOST2].type = ET_FUYU;
        ap_wrk.fg_mode = FG_ENTRY;
    break;
    case FG_WAIT:
        if (EnemyUseJudge(1) != 0)
        {
            ap_wrk.fg_mode = FG_COUNT;
            ap_wrk.fg_ap = 950;
            break;
        }

        if (GetFloatGhostAppearPosType(&type) == 0)
        {
            break;
        }

        FloatGhostAppearPosSet(type, ene_wrk[EWRK_GHOST2].move_box.pos, ene_wrk[EWRK_GHOST2].move_box.rot);
        ene_wrk[EWRK_GHOST2].dat_no = fgst_ap_no[0][ap_wrk.fgst_cnt % 20 + ingame_wrk.msn_no * 20];
        ap_wrk.fg_mode = FG_ENTRY;
        ene_wrk[EWRK_GHOST2].sta = 2;
        ene_wrk[EWRK_GHOST2].type = ET_FUYU;
    break;
    }

    return 0;
}

int FloatGhostAppearJudge()
{
    if (plyr_wrk.mode == PMODE_MSG_DISP)
    {
          return 0;
    }

    if(ingame_wrk.mode == INGAME_MODE_GET_ITEM)
    {
        return 0;
    }

    if (ap_wrk.fg_ap >= 1000)
    {
        ap_wrk.fg_ap = 950;
        return 1;
    }

    return 0;
}

int FloatGhostAppearTypeSet(u_char fg_no, u_char wrk_no, u_char room)
{
    int i;
    int ret;

    ret = 0;

    for (i = 0; i <= 2; i++)
    {
        ap_wrk.fg_pos[wrk_no][i] = -1;
    }

    i = 0;

    while (fg_ap_dat[ingame_wrk.msn_no][i].ene_type != 11)
    {
        if (
            fene_dat[ingame_wrk.msn_no][fg_no].dir == fg_ap_dat[ingame_wrk.msn_no][i].ene_type &&
            room == fg_ap_dat[ingame_wrk.msn_no][i].room_no
        )
        {
            ap_wrk.fg_pos[wrk_no][ret] = i;
            ret++;
        }

        i++;
    }

    return ret;
}

int GetFloatGhostAppearPosType(u_char *type)
{
    int i;
    int j;

    for (i = 0; i < 2; i++)
    {
        if (plyr_wrk.pr_info.room_no == area_wrk.room[i])
        {
            if (ap_wrk.fg_pos[i][0] == 0xff)
            {
                *type = 0xff;
                return 1;
            }

            if (plyr_wrk.mode == 1)
            {
                for (j = 0; j < 3; j++)
                {
                    if (fg_ap_dat[ingame_wrk.msn_no][ap_wrk.fg_pos[i][j]].cmr_no == plyr_wrk.pr_info.camera_no)
                    {
                        *type = 0xff;
                        return 1;
                    }
                }
            }
            else if (plyr_wrk.pr_info.cam_type == 0)
            {
                for (j = 0; j < 3; j++)
                {
                    if (fg_ap_dat[ingame_wrk.msn_no][ap_wrk.fg_pos[i][j]].cmr_no == plyr_wrk.pr_info.camera_no)
                    {
                        *type = ap_wrk.fg_pos[i][j];
                        return 1;
                    }
                }
            }

            break;
        }
    }

    return 0;
}

void FloatGhostAppearPosSet(u_char dat_no, float *set_pos, float *set_rot)
{
    int i;
    float dist;
    float dist_bak;
    sceVu0FVECTOR pos;
    sceVu0FVECTOR tv = {0.0f, 0.0f, 3000.0f, 0.0f};
    sceVu0FVECTOR rv = {0.0f, 0.0f, 0.0f, 0.0f};

    set_rot[0] = 0.0f;
    set_rot[1] = 0.0f;
    set_rot[2] = 0.0f;
    set_rot[3] = 0.0f;

    if (dat_no == 0xff)
    {
        rv[1] = SetRot360(GetRndSP(0, 359));
        RotFvector(rv, tv);
        sceVu0AddVector(set_pos, plyr_wrk.move_box.pos, tv);
    }
    else
    {
        dist_bak = set_rot[0];

        for (i = 0; i < fg_ap_dat[ingame_wrk.msn_no][dat_no].pos_num; i++)
        {
            pos[0] = fg_ap_dat[ingame_wrk.msn_no][dat_no].pos[i][0];
            pos[1] = fg_ap_dat[ingame_wrk.msn_no][dat_no].pos[i][1];
            pos[2] = fg_ap_dat[ingame_wrk.msn_no][dat_no].pos[i][2];

            sceVu0AddVector(pos, room_wrk.pos[0], pos);

            dist = GetDistV2(pos, plyr_wrk.move_box.pos);

            if (dist_bak < dist)
            {
                set_pos[0] = pos[0];
                set_pos[1] = pos[1];
                set_pos[2] = pos[2];
                set_pos[3] = pos[3];

                dist_bak = SetRot360(fg_ap_dat[ingame_wrk.msn_no][dat_no].rot[i]);
                set_rot[1] = dist_bak;
                dist_bak = dist;
            }
        }
    }
}

int FloatGhostBattleEnd()
{

    ap_wrk.fg_mode = 1;
    ap_wrk.fg_ap = 0;

    // missing return!!
}

int FloatGhostEscapeEnd()
{
    ap_wrk.fg_mode = 1;
    ap_wrk.fg_ap = 700;

    // missing return!!
}

void FloatGhostLoadReq()
{
    fg_load_wrk = (FG_LOAD_WRK){0};
    fg_load_wrk.mode = FG_NO_REQ;
}

int FloatGhostLoadMain()
{
    switch(fg_load_wrk.mode)
    {
    case FG_LOAD_MODE_READY:
        if (FloatGhostLoadSet())
        {
            fg_load_wrk.mode = FG_LOAD_MDL_LOAD;
        }
        else
        {
            fg_load_wrk.mode = FG_LOAD_MODE_END;
        }
    break;
    case FG_LOAD_MDL_LOAD:
        GetFloatGhostModelLoad();
        fg_load_wrk.mode = FG_LOAD_MDL_WAIT;
    break;
    case FG_LOAD_MDL_WAIT:
        if (IsLoadEndAll() == 0)
        {
            return 0;
        }
        GetFloatGhostModelLoadAfter();
        fg_load_wrk.mode = FG_LOAD_MOT_LOAD;
    case FG_LOAD_MOT_LOAD:
        GetFloatGhostMotionLoad();
        fg_load_wrk.mode = FG_LOAD_MOT_WAIT;
    break;
    case FG_LOAD_MOT_WAIT:
        if (IsLoadEndAll() == 0)
        {
            return 0;
        }
        GetFloatGhostMotionLoadAfter();
        fg_load_wrk.mode = FG_LOAD_SE_LOAD;
    case FG_LOAD_SE_LOAD:
        GetFloatGhostSELoad();
        fg_load_wrk.mode = FG_LOAD_SE_WAIT;
    break;
    case FG_LOAD_SE_WAIT:
        if (IsLoadEndAll() == 0)
        {
            return 0;
        }
        fg_load_wrk.mode = FG_LOAD_SE_TRANS;
        FGTransInit();
    case FG_LOAD_SE_TRANS:
        SeFGhostTransCtrl();
        if (IsEndFgTrans() == 0)
        {
            break;
        }
        fg_load_wrk.mode = FG_LOAD_MODE_END;
    case FG_LOAD_MODE_END:
        return 1;
    break;
    }

    return 0;
}

int FloatGhostLoadSet()
{
    if (ingame_wrk.msn_no == 0 || ingame_wrk.msn_no == 4)
    {
        return 0;
    }

    fg_load_wrk.load_no = fgst_ap_no[ingame_wrk.msn_no][ap_wrk.fgst_cnt % 20];

    ap_wrk.fg_set_num = 1;

    if (ap_wrk.fgst_no == fg_load_wrk.load_no)
    {
        return 0;
    }

    if (ap_wrk.fgst_no != 0xff)
    {
        motReleaseAniMdlBuf(fene_dat[ingame_wrk.msn_no][ap_wrk.fgst_no].anm_no, (u_int *)ENEMY_ANM_ADDR);
    }

    return 1;
}

int FloatGhostSetJudge()
{
    return 1;
}

void GetLoadFloatGhost(u_char set_num, u_char *set_fgst)
{
    set_fgst[0] = fgst_ap_no[ingame_wrk.msn_no][ap_wrk.fgst_cnt % 20];
}

void GetFloatGhostModelLoad()
{
    LoadReq(fene_dat[ingame_wrk.msn_no][fg_load_wrk.load_no].mdl_no + M000_MIKU_MDL, ENEMY_MDL_ADDR);

    ap_wrk.fgst_no = fg_load_wrk.load_no;
}

void GetFloatGhostModelLoadAfter()
{
    motInitEnemyMdl((u_int *)ENEMY_MDL_ADDR, fene_dat[ingame_wrk.msn_no][fg_load_wrk.load_no].mdl_no);
}

void GetFloatGhostMotionLoad()
{
    LoadEneDmgTex(fene_dat[ingame_wrk.msn_no][fg_load_wrk.load_no].mdl_no, (u_int *)ENEMY_DMG_TEX_ADDR);
    LoadReq(fene_dat[ingame_wrk.msn_no][fg_load_wrk.load_no].anm_no + M000_MIKU_ANM, ENEMY_ANM_ADDR);
}

void GetFloatGhostMotionLoadAfter()
{
  motInitEnemyAnm(
    (u_int *)ENEMY_ANM_ADDR,
    fene_dat[ingame_wrk.msn_no][fg_load_wrk.load_no].mdl_no,
    fene_dat[ingame_wrk.msn_no][fg_load_wrk.load_no].anm_no);
}

void GetFloatGhostSELoad()
{
    LoadReq(fene_dat[ingame_wrk.msn_no][fg_load_wrk.load_no].se_no, FLOAT_GHOST_SE_LOAD_ADDR);
}

void FloatGhostAppearStop()
{
    int i;

    for (i = 0; i < 42; i++)
    {
        ap_wrk.room_fg[i] = 0;
    }
}

void FloatGhostAppearStart()
{
    int i;

    for (i = 0; i < 42; i++)
    {
        ap_wrk.room_fg[i] = 1;
    }
}
