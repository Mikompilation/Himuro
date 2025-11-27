#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ap_rgost.h"

#include "os/eeiop/cdvd/eecdvd.h"
#include "main/glob.h"
#include "common/ul_math.h"
#include "ingame/entry/entry.h"
#include "ingame/entry/rgst_dat.h"
#include "ingame/map/door_ctl.h"
#include "ingame/map/map_area.h"
#include "ingame/map/map_ctrl.h"

RGOST_DSP_WRK rg_dsp_wrk[3] = {0};
RGOST_WRK rg_wrk[200] = {0};

#define ENE_START_ADDRESS 0x1c90000
#define ENE_BLOCK_SIZE 0x10000

void RareGhostInit()
{
    int i;

    memset(rg_wrk, 0, sizeof(rg_wrk));

    memset(rg_dsp_wrk, 0, sizeof(rg_dsp_wrk));

    for (i = 0; i < 3; i++)
    {
        rg_dsp_wrk[i].rg_no = 0xff;
        rg_dsp_wrk[i].se_wrk_no = 0xff;
    }
}

void RareGhostEntrySet()
{
    int i;
    int j;

    for (i = 0; i < 5; i++)
    {
        area_read_wrk.rgst[i] = 0xff;
    }

    for (i = 0; i < 5; i++)
    {
        // debug code?
    }

    for (i = 0; i < 5; i++)
    {
        if (ap_wrk.pg_req[i] != 0xff)
        {
            for (j = 0; j < 5; j++)
            {
                if (area_wrk.rgst[j] == (ap_wrk.pg_req[i] | 0x80))
                {
                    break;
                }
            }

            if (j == 5)
            {
                for (j = 0; j < 5; j++)
                {
                    if ((((area_wrk.rgst[j] & 0x80) == 0) || area_wrk.rgst[j] == 0xff) && area_read_wrk.rgst[j] == 0xff)
                    {
                        area_read_wrk.rgst[j] = ap_wrk.pg_req[i] | 0x80;
                        break;
                    }
                }
            }
        }

        ap_wrk.pg_req[i] = 0xff;
    }

    for (i = 0; i < 5; i++)
    {
        // debug code?
    }
}

int RareGhostLoadReq()
{
    int i;
    int ret;

    ret = 0xff;

    for (i = 0; i < 5; i++)
    {
        if (area_read_wrk.rgst[i] != 0xff)
        {
            if (area_read_wrk.rgst[i] & 0x80)
            {
                ret = LoadReq((area_read_wrk.rgst[i] & ~0x80) + PAZ_ENE001_PK2, ENE_START_ADDRESS + i * ENE_BLOCK_SIZE);
            }
            else
            {
                ret = LoadReq(area_read_wrk.rgst[i] + RARE_ENE001_PK2, ENE_START_ADDRESS + i * ENE_BLOCK_SIZE);
            }

            area_wrk.rgst[i] = area_read_wrk.rgst[i];
        }
    }

    return ret;
}

int RareGhostLoadGameLoadReq()
{
    int i;
    int ret;
    u_int addr;

    ret = 0xffff;

    for (addr = ENE_START_ADDRESS, i = 0; i < 5; addr += ENE_BLOCK_SIZE, i++)
    {
        if (area_wrk.rgst[i] != 0xff)
        {
            if (area_wrk.rgst[i] & 0x80)
            {
                ret = LoadReq((area_wrk.rgst[i] & ~0x80) + PAZ_ENE001_PK2, addr);

            } else
            {
                ret = LoadReq(area_wrk.rgst[i] + RARE_ENE001_PK2, addr);
            }

            area_wrk.rgst[i] = area_wrk.rgst[i];
        }
    }

    return ret;
}

void RareGhostMain()
{
    return;
}

void RareGhostAppearCtrl()
{
    int i;

    for (i = 0; i < 3; i++)
    {
        if (rg_ap_dat[i].ap_step0 & 0x80)
        {
            switch (rg_wrk[i].stts)
            {
                case 0:
                if (rg_ap_dat[i].ap_step1[0] >= ingame_wrk.clear_count &&
                    rg_ap_dat[i].ap_step1[1] * 0xA >= ingame_wrk.ghost_cnt &&
                    rg_ap_dat[i].ap_step1[2] >= ingame_wrk.rg_pht_cnt &&
                    rg_ap_dat[i].ap_step1[3] * 0x64 >= ingame_wrk.high_score)
                {
                    rg_wrk[i].stts = 1;
                }
                break;
                case 1:
                case 2:
                rg_wrk[i].ap_cnt += rg_ap_dat[i].ap_step2[0] * ap_wrk.gtime;
                rg_wrk[i].ap_cnt += rg_ap_dat[i].ap_step2[1] * ap_wrk.rtime;

                if (rg_ap_dat[i].ap_step2[2])
                {
                    rg_wrk[i].ap_cnt += rg_ap_dat[i].ap_step2[2] * ap_wrk.atime[GetRoomArea(rg_ap_dat[i].room)];
                }

                rg_wrk[i].ap_cnt += rg_ap_dat[i].ap_step2[3] * ap_wrk.sound;
                rg_wrk[i].ap_cnt += rg_ap_dat[i].ap_step2[4] * ap_wrk.pic_num;
                rg_wrk[i].ap_cnt += rg_ap_dat[i].ap_step2[5] * ap_wrk.raze;

                if ((rg_wrk[i].ap_cnt >= 1000) && (rg_wrk[i].stts == 2))
                {
                    rg_wrk[i].stts = 3;
                }
                break;
            }
        }
    }
}

void RareGhostDispCtrl()
{
    int i;
    int j;
    int dsp_cnt;

    dsp_cnt = 0;

    for (i = 0; i < 3; i++)
    {
        if (rg_dsp_wrk[i].mode != 0)
        {
            dsp_cnt++;
        }
    }

    if (dsp_cnt < 3)
    {
        for (i = 0; i < 5; i++)
        {
            if (rg_wrk[area_wrk.rgst[i]].stts == 0x3)
            {
                if (RareGhostDispStartJudge(area_wrk.rgst[i]) != 0)
                {
                    rg_wrk[area_wrk.rgst[i]].stts = 0x4;

                    for (j = 0; j < 3; j++)
                    {
                        if (rg_dsp_wrk[j].mode == 0)
                        {
                            break;
                        }
                    }

                    RareGhostDispWrkSet(j, area_wrk.rgst[i]);

                    dsp_cnt++;

                    if (dsp_cnt == 3)
                    {
                        break;
                    }
                }
            }
        }
    }

    if (dsp_cnt != 0)
    {
        for (i = 0; i < 3; i++)
        {
            if (rg_dsp_wrk[i].mode == 1 || rg_dsp_wrk[i].mode == 2)
            {
                if (RareGhostDispEndJudge(i) != 0)
                {
                    rg_dsp_wrk[i].mode = 3;
                    rg_dsp_wrk[i].time = rg_end_dat[rg_dat[rg_dsp_wrk[i].rg_no].end].flame;
                    rg_dsp_wrk[i].count = 0;
                    rg_dsp_wrk[i].alp_bak = rg_dsp_wrk[i].alpha;
                }
            }
        }

        if (dsp_cnt != 0)
        {
            for (i = 0; i < 3; i++)
            {
                if (rg_dsp_wrk[i].mode != 0)
                {
                    SetRareGhostDispAlpha(i, rg_dsp_wrk[i].rg_no);
                    SetRareGhostDispAnimation(i, rg_dsp_wrk[i].rg_no);

                    RareGhostDispTimeCtrl(i, rg_dsp_wrk[i].rg_no);
                }
            }
        }
    }
}

int RareGhostDispStartJudge(int rg_no)
{
    int dist;
    sceVu0FVECTOR room_pos;
    sceVu0FVECTOR rg_pos;

    if (GetRoomPos(rg_ap_dat[rg_no].room, room_pos) != 0)
    {
        return 0;
    }

    rg_pos[0] = rg_dat[rg_no].pos[0] + room_pos[0];
    rg_pos[1] = rg_dat[rg_no].pos[1] + room_pos[1];
    rg_pos[2] = rg_dat[rg_no].pos[2] + room_pos[2];

    dist = GetDistV(rg_pos, plyr_wrk.move_box.pos);

    if (rg_ap_dat[rg_no].ap_near > dist || rg_ap_dat[rg_no].ap_far < dist)
    {
        return 0;
    }

    if (rg_ap_dat[rg_no].ap_stts & 0x8000)
    {
        if (plyr_wrk.pr_info.room_no != rg_ap_dat[rg_no].room)
        {
            return 0;
        }
    }

    if (rg_ap_dat[rg_no].ap_stts & 0x4000)
    {
        if (plyr_wrk.mode != 1)
        {
            return 0;
        }
    }

    if (rg_ap_dat[rg_no].ap_stts & 0x2000)
    {
        if ((plyr_wrk.sta & 0x1) == 0)
        {
            return 0;
        }
    }

    return 1;
}

int RareGhostDispEndJudge(int wrk_no)
{
    int rg_no;
    int dist;

    rg_no = rg_dsp_wrk[wrk_no].rg_no;

    dist = GetDistV(rg_dsp_wrk[wrk_no].pos,plyr_wrk.move_box.pos);

    if (rg_ap_dat[rg_no].ap_near > dist || rg_ap_dat[rg_no].ap_far < dist)
    {
        return 1;
    }

    if (rg_ap_dat[rg_no].ap_stts & 0x8000)
    {
        if (plyr_wrk.pr_info.room_no != rg_ap_dat[rg_no].room)
        {
            return 1;
        }
    }

    if (rg_ap_dat[rg_no].ap_stts & 0x4000)
    {
        if (plyr_wrk.mode != 1)
        {
            return 1;
        }
    }

    if (rg_ap_dat[rg_no].ap_stts & 0x2000)
    {
        if ((plyr_wrk.sta & 0x1) == 0)
        {
            return 1;
        }
    }

    return 0;
}

void RareGhostDispTimeCtrl(int wrk_no, int rg_no)
{
    rg_dsp_wrk[wrk_no].count++;

    if (rg_dsp_wrk[wrk_no].time != rg_dsp_wrk[wrk_no].count)
    {
        return;
    }

    switch (rg_dsp_wrk[wrk_no].mode)
    {
    case 1:
        rg_dsp_wrk[wrk_no].mode = 2;
        rg_dsp_wrk[wrk_no].time = rg_inter_dat[rg_dat[rg_no].inter].flame;
        rg_dsp_wrk[wrk_no].count = 0;
        rg_dsp_wrk[wrk_no].alp_bak = rg_dsp_wrk[wrk_no].alpha;
    break;
    case 2:
        rg_dsp_wrk[wrk_no].mode = 0x3;
        rg_dsp_wrk[wrk_no].time = rg_end_dat[rg_dat[rg_no].end].flame;
        rg_dsp_wrk[wrk_no].count = 0;
        rg_dsp_wrk[wrk_no].alp_bak = rg_dsp_wrk[wrk_no].alpha;
    break;
    case 3:
        rg_dsp_wrk[wrk_no].mode = 0;
        rg_wrk[rg_no].stts = 0x2;
        rg_dsp_wrk[wrk_no].count = 0;
        rg_wrk[rg_no].ap_cnt = 0;
    break;
    }
}

void RareGhostDispEndSet(int wrk_no)
{
    if (rg_dsp_wrk[wrk_no].mode == 1 || rg_dsp_wrk[wrk_no].mode == 2)
    {
        rg_dsp_wrk[wrk_no].mode = 3;
        rg_dsp_wrk[wrk_no].time = rg_end_dat[rg_dat[rg_dsp_wrk[wrk_no].rg_no].end].flame;
        rg_dsp_wrk[wrk_no].count = 0;
        rg_dsp_wrk[wrk_no].alp_bak = rg_dsp_wrk[wrk_no].alpha;
    }
}


void RareGhostDispWrkSet(int wrk_no, int rg_no)
{
    sceVu0FVECTOR room_pos;

    rg_dsp_wrk[wrk_no].mode = 1;
    rg_dsp_wrk[wrk_no].rg_no = rg_no;

    GetRoomPos(rg_ap_dat[rg_no].room, room_pos);

    rg_dsp_wrk[wrk_no].pos[0] = rg_dat[rg_no].pos[0] + room_pos[0];
    rg_dsp_wrk[wrk_no].pos[1] = rg_dat[rg_no].pos[1] + room_pos[1];
    rg_dsp_wrk[wrk_no].pos[2] = rg_dat[rg_no].pos[2] + room_pos[2];

    rg_dsp_wrk[wrk_no].count = 0;
    rg_dsp_wrk[wrk_no].time = rg_start_dat[rg_dat[rg_no].start].flame;

    SetRareGhostDispAlpha(wrk_no, rg_no);
    SetRareGhostDispAnimation(wrk_no, rg_no);
}

void SetRareGhostDispAlpha(int wrk_no, int rg_no)
{
    int count;
    int cnt_min;
    u_char alp_no;
    u_char stts;
    u_char alpha;
    u_char mode;
    RG_ALP_DAT *rg_alp;
    int i;
    int flame;

    count = rg_dsp_wrk[wrk_no].count;
    mode = rg_dsp_wrk[wrk_no].mode;

    switch (mode)
    {
    case 1:
        alp_no = rg_start_dat[rg_dat[rg_no].start].alp_no;
        rg_alp = rg_alp_start[alp_no];
    break;
    case 2:
        alp_no = rg_inter_dat[rg_dat[rg_no].inter].alp_no;
        rg_alp = rg_alp_inter[alp_no];
    break;
    case 3:
        alp_no = rg_end_dat[rg_dat[rg_no].end].alp_no;
        rg_alp = rg_alp_end[alp_no];
    break;
    }

    if (count == 0)
    {
        if (mode == 1)
        {
            rg_dsp_wrk[wrk_no].alp_bak = rg_alp->alp;
        }
    }

    i = 0;

    while (rg_alp[i].flame < count)
    {
        i++;
    }

    if (rg_alp[i].flame == count)
    {
        rg_dsp_wrk[wrk_no].alp_bak = rg_alp[i-1].alp;
    }

    cnt_min = rg_alp[i-1].flame;

    stts = rg_alp[i].stts;
    alpha = rg_alp[i].alp;
    flame = rg_alp[i].flame;

    switch (stts)
    {
    case 0:
        rg_dsp_wrk[wrk_no].alpha = alpha;
    break;
    case 1:
    case 2:
        rg_dsp_wrk[wrk_no].alpha = rg_dsp_wrk[wrk_no].alp_bak + (alpha - rg_dsp_wrk[wrk_no].alp_bak) * (count - cnt_min) / (flame - cnt_min);
    break;
    }
}

void SetRareGhostDispAnimation(int wrk_no, int rg_no)
{
    rg_dsp_wrk[wrk_no].dsp_no = 0;
}

float SetRareGhostDispRate(int wrk_no)
{
    return 0.0f;
}
