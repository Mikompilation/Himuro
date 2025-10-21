#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ap_dgost.h"

#include "main/glob.h"
#include "common/ul_math.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"
#include "graphics/graph2d/effect_ene.h"
#include "ingame/event/ev_main.h"
#include "ingame/event/ev_load.h"
#include "ingame/plyr/unit_ctl.h"
#include "ingame/enemy/ene_ctl.h"
#include "ingame/map/door_ctl.h"
#include "ingame/entry/entry.h"
#include "ingame/entry/ap_fgost.h"
// #include "graphics/motion/motion.h"

u_char dgst_room[] = {
    3, 7, 16, 21, 22, 23, 25, 27, 28, 0xFF,
};
#include "data/dgst_load.h" // MSN_LOAD_DAT dgst_load[3];

DGST_WRK dgst_wrk = {0};

void DeadGhostAppearReq()
{
    sceVu0FVECTOR pos;
    sceVu0FVECTOR tv0 = {0.0f, 0.0f, 1400.0f, 0.0f};
    sceVu0FVECTOR tv1 = {0.0f, 0.0f, 1800.0f, 0.0f};
    sceVu0FVECTOR tv2 = {0.0f, 0.0f, 2200.0f, 0.0f};
    sceVu0FVECTOR rv = {0.0f, 0.0f, 0.0f, 0.0f};

    if (ingame_wrk.msn_no == 3)
    {
        ene_wrk[0].dat_no = 11;
        ene_wrk[1].dat_no = 12;
        ene_wrk[2].dat_no = 13;
    }
    else if (ingame_wrk.msn_no == 4)
    {
        ene_wrk[0].dat_no = 8;
        ene_wrk[1].dat_no = 9;
        ene_wrk[2].dat_no = 10;
    }

    ene_wrk[0].sta = 2;
    ene_wrk[0].type = 1;

    ene_wrk[1].sta = 2;
    ene_wrk[1].type = 1;

    ene_wrk[2].sta = 2;
    ene_wrk[2].type = 1;

    rv[1] = SetRot360(GetRndSP(0, 359));
    RotFvector(rv, tv0);
    sceVu0AddVector(ene_wrk[0].move_box.pos, plyr_wrk.move_box.pos, tv0);

    rv[1] = SetRot360(GetRndSP(0, 359));
    RotFvector(rv, tv1);
    sceVu0AddVector(ene_wrk[1].move_box.pos, plyr_wrk.move_box.pos, tv1);

    rv[1] = SetRot360(GetRndSP(0, 359));
    RotFvector(rv, tv2);
    sceVu0AddVector(ene_wrk[2].move_box.pos, plyr_wrk.move_box.pos, tv2);
}

void DeadGhostAppearMain()
{
    return;
}

int DeadGhostAppearJudge()
{
    int i;

    if (ingame_wrk.msn_no != 3)
    {
        return 0;
    }

    for (i = 0; dgst_room[i] != plyr_wrk.pr_info.room_no; i++)
    {
        if (dgst_room[i] == 0xff)
        {
            return 0;
        }
    }

    if (EnemyUseJudge(0) != 0)
    {
        return 0;
    }

    for(i = 0; i < 5; i++)
    {
        if (ap_wrk.gg_room[i] == plyr_wrk.pr_info.room_no)
        {
            return 0;
        }
    }

    if (IsAllConnectDoorClose() == 0)
    {
        return 0;
    }

    if (ap_wrk.dgst_cnt > ap_wrk.fgst_cnt)
    {
        return 0;
    }

    ap_wrk.dgst_cnt += 1;

    return 1;
}

void DeadGhostLoadReq()
{
    dgst_wrk.load_mode = DGLOAD_MODE_REQ;
    return;
}

int DeadGhostLoad()
{
    if (ingame_wrk.msn_no != 3)
    {
        return 1;
    }

    switch(dgst_wrk.load_mode)
    {
    case DGLOAD_MODE_REQ:
        dgst_wrk.load_mode = DGLOAD_MODE_START;
    break;
    case DGLOAD_MODE_START:
        LoadReq(M055_SYOUALL_MDL, 0xc80000);

        dgst_wrk.load_mode = DGLOAD_MODE_MDL;
    break;
    case DGLOAD_MODE_MDL:
        if (IsLoadEndAll() != 0)
        {
            motInitEnemyMdl((u_int *)0xc80000, M055_SYOUALL);
            LoadEneDmgTex(42, (u_int *)0xac8000);
            LoadReq(M042_SYOUKI2_ANM, 0xa30000);

            dgst_wrk.load_mode = DGLOAD_MODE_MOT;
        }
    break;
    case DGLOAD_MODE_MOT:
        if (IsLoadEndAll() != 0)
        {
            motInitEnemyAnm((u_int *)0xa30000, M055_SYOUALL, A042_SYOUKIA);
            SeFileLoadAndSetFGhost(SG046_SYOUKI1_BD, 16);

            ap_wrk.fg_se_empty[0] = 1;
            dgst_wrk.load_mode = DGLOAD_MODE_SE;
        }
    break;
    case DGLOAD_MODE_SE:
        if (IsLoadEndAll() != 0)
        {
            DeadGhostSetLoad();

            dgst_wrk.load_mode = DGLOAD_MODE_END;
        }
    break;
    case DGLOAD_MODE_END:
        return 1;
    break;
    }

    return 0;
}

void DeadGhostBattleEnd()
{
    if (EnemyUseJudge(ET_FUYU) != 0)
    {
        FloatGhostBattleEnd();
    }
}

int EnemyNoDeadGhostJudge(int ene_no)
{
    if (ingame_wrk.msn_no == 3)
    {
        if (ene_no == 11)
        {
            return 1;
        }

        else if (ene_no == 12)
        {
            return 1;
        }

        else if (ene_no == 13)
        {
            return 1;
        }
    }

    else if (ingame_wrk.msn_no == 4)
    {
        if (ene_no == 8)
        {
            return 1;
        }

        else if (ene_no == 9)
        {
            return 1;
        }

        else if (ene_no == 10)
        {
            return 1;
        }
    }

    return 0;
}

void DeadGhostSetLoad()
{
    int i;

    for (i = 0; i <= 2; i++)
    {
        SetDataLoadWrk(&dgst_load[i]);
    }
}

void DeadGhostLoadDel()
{
    int i;

    for (i = 0; i <= 2; i++)
    {
        DelDataLoadWrk(dgst_load[i].file_no);
    }
}
