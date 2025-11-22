#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "move_ctl.h"

#include "sce/libvu0.h"

#include "main/glob.h"
#include "os/eeiop/eese.h"
#include "os/eeiop/se_foot.h"
#include "common/ul_math.h"
#include "ingame/ig_glob.h"
#include "ingame/camera/camera.h"
#include "ingame/enemy/ene_ctl.h"
#include "ingame/info/inf_disp.h"
#include "ingame/map/door_ctl.h"
#include "ingame/map/map_ctrl.h"
#include "ingame/plyr/unit_ctl.h"
#include "ingame/plyr/plyr_ctl.h"
#include "graphics/motion/mdlact.h"
#include "graphics/motion/mdlwork.h"
#include "graphics/motion/motion.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_obj.h"
#include "graphics/graph2d/effect_oth.h"
#include "graphics/graph2d/effect_scr.h"
#include "graphics/graph3d/gra3d.h"

void (*CommJmpTbl[])(MOVE_BOX * mb) = {
    EJob000, EJob001, EJob002, EJob003, EJob004, EJob005, EJob006, EJob007,
    EJob008, EJob009, EJob00A, EJob00B, EJob00C, EJob00D, EJob00E, EJob00F,
    EJob010, EJob011, EJob012, EJob013, EJob014, EJob015, EJob016, EJob017,
    EJob018, EJob019, EJob01A, EJob01B, EJob01C, EJob01D, EJob01E, EJob01F,
    EJob020, EJob021, EJob022, EJob023, EJob024, EJob025, EJob026, EJob027,
    EJob028, EJob029, EJob02A, EJob02B, EJob02C, EJob02D, EJob02E, EJob02F,
    EJob030, EJob031, EJob032, EJob033, EJob034, EJob035, EJob036, EJob037,
    EJob038, EJob039, EJob03A, EJob03B, EJob03C, EJob03D, EJob03E, EJob03F,
    EJob040, EJob041, EJob042, EJob043, EJob044, EJob045, EJob046, EJob047,
    EJob048, EJob049, EJob04A, EJob04B, EJob04C, EJob04D, EJob04E, EJob04F,
    EJob050, EJob051, EJob052, EJob053, EJob054, EJob055, EJob056, EJob057,
    EJob058, EJob059, EJob05A, EJob05B, EJob05C, EJob05D, EJob05E, EJob05F,
    EJob060, EJob061, EJob062, EJob063, EJob064, EJob065, EJob066, EJob067,
    EJob068, EJob069, EJob06A, EJob06B, EJob06C, EJob06D, EJob06E, EJob06F,
    EJob070, EJob071, EJob072, EJob073,
};
void (*BCommJmpTbl[])(ENE_WRK *ew) = {
    BJob000,   BJob001,   BJobDammy, BJobDammy, BJobDammy, BJobDammy, BJobDammy,
    BJobDammy, BJobDammy, BJob009,   BJob00A,   BJob00B,   BJob00C,   BJobDammy,
    BJobDammy, BJobDammy, BJob010,   BJob011,   BJob012,   BJob013,   BJob014,
};

/* sdata 356fa8 */u_char er_max_tbl[3] = {0, 0, 0};

#define PI 3.1415927f
#define DEG2RAD(x) ((float)(x)*PI/180.0f)

// read a 16-bit little-endian value from an arbitrary (and potentially unaligned) address
#define READ_LE16(addr) \
    ( ((u_short)(*((u_char *)(int)(addr) + 1)) << 8) | \
      ((u_short)(*((u_char *)(int)(addr) + 0))) )

// produce an address by combining base segment (e.g., 0x7E0000) + 16-bit offset
#define SEGMENT_ADDR(base, offset16) \
    ((u_long)((base) | ((offset16) & 0xFFFF)))

// add an index where each entry is 2 bytes (16-bit)
#define INDEX16(base, no) \
    ((u_long)(base) + ((no) * 2))

#define COMM_ADD_TOP_ADDRESS 0x7e0000

void InitMoveBox(MOVE_BOX *mb)
{
    memset(mb, 0, sizeof(MOVE_BOX));

    mb->idx = 0xff;
}

void MoveCtrl(MOVE_BOX *mb)
{
    if (mb->wait_time == 0)
    {
        return;
    }

    mb->wait_time--;

    while (mb->wait_time == 0)
    {
        if (mb->pos_no == 0)
        {
            mb->job_no = *(mb->comm_add).pu8++;
        }

        (*CommJmpTbl[mb->job_no])(mb);

        if (mb->wait_time == 0xff)
        {
            mb->wait_time = 0;
            mb->pos_no = 0;

            return;
        }
    }
}

void EneBlinkCtrl(ENE_WRK *ew)
{
    if (ew->sta & 0x30000000)
    {
        return;
    }

    if (dbg_wrk.param_enestop != 0)
    {
        ew->tr_rate = 80;

        return;
    }

    if (ew->bwait_time == 0)
    {
        return;
    }

    ew->bwait_time--;

    while (ew->bwait_time == 0)
    {
        if (ew->bpos_no == 0)
        {
            ew->bjob_no = *ew->bcomm_add.pu8++;
        }

        (*BCommJmpTbl[ew->bjob_no])(ew);

        if (ew->bwait_time == 0xff)
        {
            ew->bwait_time = 0;
            ew->bpos_no = 0;

            return;
        }
    }
}

void EJob000(MOVE_BOX *mb)
{
    u_short pos0;
    short pos1;
    u_short pos2;

    pos0 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    pos1 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    pos2 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    mb->pos[0] = pos0;
    mb->pos[1] = pos1;
    mb->pos[2] = pos2;
    mb->pos[3] = 0.0f;

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob001(MOVE_BOX *mb)
{
    sceVu0FVECTOR tv;
    short pos0;
    short pos1;
    short pos2;

    pos0 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    pos1 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    pos2 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    tv[0] = pos0;
    tv[1] = pos1;
    tv[2] = pos2;
    tv[3] = 0.0f;

    sceVu0AddVector(mb->pos, tv, tv);

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob002(MOVE_BOX *mb)
{
    u_char time;

    switch (mb->pos_no)
    {
    case 0:
        time = *mb->comm_add.pu8;

        mb->comm_add.pu8++;

        if (time == 0)
        {
            mb->wait_time = 1;
            mb->pos_no = 2;
        }
        else
        {
            mb->wait_time = time;
            mb->pos_no++;
        }
    break;
    case 1:
        mb->wait_time = 0;
        mb->pos_no = 0;
    break;
    case 2:
        mb->wait_time = 1;
    break;
    }
}

void EJob003(MOVE_BOX *mb)
{
    int chk;
    u_short pos0;
    u_short pos2;

    switch (mb->pos_no)
    {
    case 0:
        pos0 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
        pos2 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

        mb->tpos[0] = pos0;
        mb->tpos[1] = 0.0f;
        mb->tpos[2] = pos2;
        mb->tpos[3] = 0.0f;

        GetTrgtRot(mb->pos, mb->tpos, mb->rot, 2);

        mb->spd[0] = 0.0f;
        mb->spd[1] = 0.0f;
        mb->spd[2] = ene_wrk[mb->idx].dat->wspd;
        mb->spd[3] = 0.0f;

        mb->wait_time = 1;
        mb->pos_no++;
    break;
    case 1:
        chk = 0;

        switch (ConvertRot2Dir(mb->rot[1], 0))
        {
        case 0:
            if (mb->pos[2] > mb->tpos[2]) chk = 1;
        break;
        case 1:
            if (mb->pos[0] > mb->tpos[0]) chk = 1;
        break;
        case 2:
            if (mb->pos[2] < mb->tpos[2]) chk = 1;
        break;
        case 3:
            if (mb->pos[0] < mb->tpos[0]) chk = 1;
        break;
        }

        if (chk)
        {
            mb->spd[0] = 0.0f;
            mb->spd[1] = 0.0f;
            mb->spd[2] = 0.0f;
            mb->spd[3] = 0.0f;

            mb->wait_time = 0;
            mb->pos_no = 0;
        }
        else
        {
            mb->wait_time = 1;
        }
    break;
    }
}

void EJob004(MOVE_BOX *mb)
{
    sceVu0FVECTOR tv;
    float dist[3];
    int *erd;
    char i;
    char j;
    char n;
    char max;
    char candidate[3];
    u_char mt;

    switch (mb->pos_no)
    {
    case 0:
        mt = *mb->comm_add.pu8++;

        for (i = 0; i < 3; i++) candidate[i] = -1;

        erd = GetEneRootData(mb->idx);
        max = GetEneRootMax(mb->idx);

        dist[1] = dist[2] = 0.0f;

        for (i = 0; i < max; i++)
        {
            tv[0] = erd[i * 3 + 0];
            tv[1] = erd[i * 3 + 1];
            tv[2] = erd[i * 3 + 2];
            tv[3] = 0.0f;

            if (tv[1] == mb->pos[1])
            {
                dist[0] = GetDistV2(tv, mb->pos);

                if (HitChkSegment2AllChk(tv, mb->pos, dist[0] / 50.0f) == 0)
                {
                    if (dist[1] == 0.0f || dist[0] < dist[1])
                    {
                        if (candidate[0] != -1)
                        {
                            dist[2] = dist[1];
                            candidate[1] = candidate[0];
                        }

                        dist[1] = dist[0];
                        candidate[0] = i;
                    }
                    else if (dist[2] == 0.0f || dist[0] < dist[2])
                    {
                        dist[2] = dist[0];
                        candidate[1] = i;
                    }
                }
            }
        }

        if (dist[1] == 0.0f && dist[2] == 0.0f)
        {
            dist[1] = dist[2] = 0.0f;

            for (i = 0; i < max; i++)
            {
                tv[0] = erd[i * 3 + 0];
                tv[1] = erd[i * 3 + 1];
                tv[2] = erd[i * 3 + 2];
                tv[3] = 0.0f;

                dist[0] = GetDistV2(tv, mb->pos);

                if (dist[1] == 0.0f || dist[0] < dist[1])
                {
                    if (candidate[0] != -1)
                    {
                        dist[2] = dist[1];
                        candidate[1] = candidate[0];
                    }

                    dist[1] = dist[0];
                    candidate[0] = i;
                }
                else if (dist[2] == 0.0f || dist[0] < dist[2])
                {
                    dist[2] = dist[0];
                    candidate[1] = i;
                }
            }

            if (dist[1] == 0.0f && dist[2] == 0.0f)
            {
                mb->wait_time = 1;
                mb->pos_no = 100;

                return;
            }
        }

        i = 0;
        n = -1;
        dist[1] = 0.0f;

        while (1)
        {
            j = candidate[i];

            if (j == -1)
            {
                break;
            }

            tv[0] = erd[j * 3 + 0];
            tv[1] = erd[j * 3 + 1];
            tv[2] = erd[j * 3 + 2];
            tv[3] = 0.0f;

            dist[0] = GetDistV2(plyr_wrk.move_box.pos, tv);

            if (dist[1] == 0.0f || dist[0] < dist[1])
            {
                dist[1] = dist[0];
                n = j;
            }

            i++;
        }

        mb->tpos[0] = erd[n * 3 + 0];
        mb->tpos[1] = erd[n * 3 + 1];
        mb->tpos[2] = erd[n * 3 + 2];
        mb->tpos[3] = 0.0f;

        GetTrgtRot(mb->pos, mb->tpos, mb->trot, 3);

        mb->trot[3] = DEG2RAD(2.0f);

        if (mt == 0)
        {
            mb->spd[0] = 0.0f;
            mb->spd[1] = 0.0f;
            mb->spd[2] = ene_wrk[mb->idx].dat->wspd;
            mb->spd[3] = 0.0f;
        }
        else
        {
            mb->spd[0] = 0.0f;
            mb->spd[1] = 0.0f;
            mb->spd[2] = ene_wrk[mb->idx].dat->rspd;
            mb->spd[3] = 0.0f;
        }

        mb->pos_no++;
        mb->wait_time = 1;
    break;
    case 1:
        if (mb->trot[3] != 0.0f)
        {
            tv[0] = mb->trot[0];
            tv[1] = mb->trot[1];
            tv[2] = mb->trot[2];
            tv[3] = mb->trot[3];
        }
        else
        {
            tv[0] = mb->rot[0];
            tv[1] = mb->rot[1];
            tv[2] = mb->rot[2];
            tv[3] = mb->rot[3];
        }

        n = 0;

        switch (ConvertRot2Dir(tv[1], 0))
        {
        case 0:
            if (mb->pos[2] > mb->tpos[2]) n = 1;
        break;
        case 1:
            if (mb->pos[0] > mb->tpos[0]) n = 1;
        break;
        case 2:
            if (mb->pos[2] < mb->tpos[2]) n = 1;
        break;
        case 3:
            if (mb->pos[0] < mb->tpos[0]) n = 1;
        break;
        }

        if (n)
        {
            mb->wait_time = 0;
            mb->pos_no = 0;
        }
        else
        {
            mb->wait_time = 1;
        }
    break;
    case 100:
        mb->pos[0] = plyr_wrk.move_box.pos[0];
        mb->pos[1] = plyr_wrk.move_box.pos[1];
        mb->pos[2] = plyr_wrk.move_box.pos[2];
        mb->pos[3] = plyr_wrk.move_box.pos[3];

        mb->wait_time = 0;
        mb->pos_no = 0;
    break;
    }
}

int* GetEneRootData(u_char no)
{
    int *rd;

    switch (no)
    {
    case 0:
        rd = (int *)erootd0;
    break;
    case 1:
        rd = (int *)erootd1;
    break;
    case 2:
        rd = (int *)erootd2;
    break;
    }
    return rd;
}

u_char GetEneRootMax(u_char no)
{
    return er_max_tbl[no];
}

void EJob005(MOVE_BOX *mb)
{
    float spd;

    switch (*mb->comm_add.pu8++)
    {
    case 1:
        spd = ene_wrk[mb->idx].dat->rspd;
    break;
    case 2:
        spd = ene_wrk[mb->idx].dat->wspd * 0.5f;
    break;
    default:
        spd = ene_wrk[mb->idx].dat->wspd;
    break;
    }

    mb->spd[0] = 0.0f;
    mb->spd[1] = 0.0f;
    mb->spd[2] = spd;
    mb->spd[3] = 0.0f;

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob006(MOVE_BOX *mb)
{
    u_short pos0;

    pos0 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    mb->comm_add.wrk = mb->comm_add_top + pos0;

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob007(MOVE_BOX *mb)
{
    ene_wrk[mb->idx].sta &= ~0x1000000;
    ene_wrk[mb->idx].sta &= ~0x400000;
    ene_wrk[mb->idx].sta &= ~0x800000;
    ene_wrk[mb->idx].sta &= ~0x2000000;
    ene_wrk[mb->idx].sta &= ~0x80000;
    ene_wrk[mb->idx].sta &= ~0x10000;
    ene_wrk[mb->idx].sta &= ~0x20000;

    ene_wrk[mb->idx].sta |= 0x1000 | 0x4;

    ene_wrk[mb->idx].act_no = 0xff;

    mb->wait_time = 0xff;
    mb->pos_no = 0;
}

void EJob008(MOVE_BOX *mb)
{
    short int rot[3];
    u_char i;

    for(i = 0; i < 3; i++)
    {
        rot[i] = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

        rot[i] = (rot[i] + 0x800) & 0x0fff;

        if (rot[i] != 0x800)
        {
            mb->rspd[i] = (rot[i] * (PI * 2)) / 4095.0f - PI;
        }
        else
        {
            mb->rspd[i] = 0.0f;
        }
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob009(MOVE_BOX *mb)
{
    u_char chk;
    short pos0;
    short pos1;
    short pos2;

    chk = *mb->comm_add.pu8++;

    pos0 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    pos1 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    pos2 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    mb->spd[0] = pos0;
    mb->spd[1] = pos1;
    mb->spd[2] = pos2;
    mb->spd[3] = 0.0f;

    if (chk)
    {
        RotFvector(mb->rot, mb->spd);
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob00A(MOVE_BOX *mb)
{
    sceVu0FVECTOR tv;
    sceVu0FVECTOR tr;
    u_char x;
    u_char z;

    x = *mb->comm_add.pu8++;
    z = *mb->comm_add.pu8++;

    GetTrgtRot(mb->pos, plyr_wrk.move_box.pos, tr, 2);

    tv[2] = (z > 1 ? GetRndSP(1, z) : z) * 20.0f;
    tv[0] = x != 0 ? GetRndSP(0, x * 2) * 100.0f - x * 100.0f : 0.0f;
    tv[1] = 0.0f;

    RotFvector(tr, tv);
    sceVu0AddVector(mb->pos, mb->pos, tv);
    EnePosInfoSet(ene_wrk + mb->idx);
    EneBlinkPosSet(ene_wrk + mb->idx);

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob00B(MOVE_BOX *mb)
{
    u_short time;
    u_short adj;

    time = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (time != 0 && time <= time_wrk.stop)
    {
        time = 0;
    }

    if (time == 0 && (plyr_wrk.mvsta & (0x8 | 0x4 | 0x2 | 0x1)) == 0)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob00C(MOVE_BOX *mb)
{
    sceVu0FVECTOR tv;
    u_char id;

    id = *mb->comm_add.pu8++;

    sceVu0AddVector(tv, plyr_wrk.move_box.pos, ene_wrk[mb->idx].adjp);

    if (id == 0) {
        GetTrgtRot(mb->pos, tv, mb->rot, 3);

        mb->trot[3] = 0.0f;
    }
    else if (id == 0xff)
    {
        mb->trot[1] = mb->rot[1];
        mb->trot[3] = 0.0f;
    }
    else if (id == 0xfe)
    {
        tv[1] += -687.0f;

        GetTrgtRot(mb->pos, tv, mb->trot, 3);

        mb->rot[0] = mb->trot[0];
        mb->trot[3] = DEG2RAD(2.0f);
    }
    else
    {
        GetTrgtRot(mb->pos, tv, mb->trot, 3);

        mb->rot[0] = mb->trot[0];
        mb->trot[3] = DEG2RAD(2.0f) / id;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob00D(MOVE_BOX *mb)
{
    sceVu0FVECTOR tv;
    sceVu0FVECTOR tr;
    u_char type;
    u_char dmin;
    u_char _min;
    u_char wmin;
    static u_char tr_rate_save[4];

    switch (mb->pos_no)
    {
    case 0:
        type = *mb->comm_add.pu8++;
        dmin = *mb->comm_add.pu8++;
        _min = *mb->comm_add.pu8++;
        wmin = *mb->comm_add.pu8++;
        mb->comm_add.pu8++;

        if (_min != dmin)
        {
            tv[0] = 0.0f;
            tv[1] = 0.0f;
            tv[2] = GetRndSP(dmin, _min - dmin) * 50.0f;
            tv[3] = 0.0f;
        }
        else
        {
            tv[0] = 0.0f;
            tv[1] = 0.0f;
            tv[2] = dmin * 50.0f;
            tv[3] = 0.0f;
        }

        switch (type)
        {
        case 0:
            tr[0] = 0.0f;
            tr[1] = plyr_wrk.move_box.rot[1] + PI;
            tr[2] = 0.0f;
            tr[3] = 0.0f;
        break;
        case 1:
            tr[0] = 0.0f;
            tr[1] = GetRndSP(0, 36) * DEG2RAD(10.0f) - PI;
            tr[2] = 0.0f;
            tr[3] = 0.0f;
        break;
        case 2:
            tr[0] = 0.0f;
            tr[1] = plyr_wrk.move_box.rot[1];
            tr[2] = 0.0f;
            tr[3] = 0.0f;
        break;
        }

        RotLimitChk(&tr[1]);
        RotFvector(tr, tv);

        sceVu0AddVector(tv, plyr_wrk.move_box.pos, tv);

        tv[1] = mb->pos[1] + ene_wrk[mb->idx].adjp[1];

        mb->pos[0] = tv[0];
        mb->pos[1] = tv[1];
        mb->pos[2] = tv[2];
        mb->pos[3] = tv[3];

        EnePosInfoSet(&ene_wrk[mb->idx]);
        EneBlinkPosSet(&ene_wrk[mb->idx]);

        SeStartPos(31, mb->pos, 0, 0x1000, 0x1000, 0xff);

        if (wmin != 0)
        {
            tr_rate_save[mb->idx] = ene_wrk[mb->idx].tr_rate;

            ene_wrk[mb->idx].tr_rate = 0;

            mb->wait_time = wmin;
            mb->pos_no++;
        }
        else
        {
            mb->wait_time = 0x0;
            mb->pos_no = 0x0;
        }
    break;
    case 1:
        ene_wrk[mb->idx].tr_rate = tr_rate_save[mb->idx];

        mb->wait_time = 0;
        mb->pos_no = 0;
    break;
    }
}

void EJob00E(MOVE_BOX *mb)
{
    float hit_rng;
    float rot;
    u_short adj;
    u_char id;

    id = *mb->comm_add.pu8++;
    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (id == 1)
    {
        hit_rng = ene_wrk[mb->idx].dat->hit_rng;
    }
    else if (id == 2)
    {
        hit_rng = ene_wrk[mb->idx].dat->hit_rng * 0.4f;
    }
    else if (ene_wrk[mb->idx].sta2 & 0x1 && plyr_wrk.mode == 1)
    {
        hit_rng = ene_wrk[mb->idx].dat->hit_rng * 0.7f;

        ene_wrk[mb->idx].sta2 |= 0x2;
    }
    else
    {
        hit_rng = ene_wrk[mb->idx].dat->hit_rng + 200.0f;
    }

    rot = GetTrgtRotY(mb->pos, plyr_wrk.move_box.pos) - mb->rot[1];

    RotLimitChk(&rot);

    if (
        GetDistV(plyr_wrk.move_box.pos, mb->pos) <= hit_rng &&
        __builtin_fabsf((plyr_wrk.move_box.pos[1] - mb->pos[1]) + ene_wrk[mb->idx].adjp[1]) <= 300.0f &&
        __builtin_fabsf(rot) <= DEG2RAD(45.0f)
    )
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob00F(MOVE_BOX *mb)
{
    ENE_WRK *ew;
    sceVu0FVECTOR tv;
    sceVu0FVECTOR rv;
    u_short adj;
    u_char id;
    u_char per;
    u_char flag;
    u_char ani;
    u_char pani_tbl[3] = { 56, 58, 60 };

    ew = &ene_wrk[mb->idx];

    id = *mb->comm_add.pu8++;
    per = *mb->comm_add.pu8++;
    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    flag = 0;

    if (plyr_wrk.mvsta & 0x200000 ||
        plyr_wrk.sta & 0x8 ||
        __builtin_fabsf((plyr_wrk.move_box.pos[1] - mb->pos[1]) + ew->adjp[1]) > 200.0f ||
        plyr_wrk.mode == 2
    )
    {
        flag = 1;
    }

    if (flag || GetRndSP(1, 99) <= per)
    {
        id = 2;
    }

    ani = 0xff;

    if (id & 0x80)
    {
        ani = id & 0x7f;
        id = 1;
    }

    switch (id)
    {
    case 0:
        plyr_wrk.mvsta &= ~0xf;

        ReqFinderInOverRap(0);

        if (plyr_wrk.mode == 1)
        {
            FinderEndSet();
        }

        plyr_wrk.sta &= ~0x10;
        plyr_wrk.mode = 7;
        plyr_wrk.dmg_cam_flag = 0;
        plyr_wrk.atk_no = mb->idx;

        GetTrgtRot(plyr_wrk.move_box.pos, mb->pos, rv, 2);

        rv[1] -= plyr_wrk.move_box.rot[1];

        RotLimitChk(&rv[1]);

        if (rv[1] >= (PI / 3) && rv[1] <= PI)
        {
            SetPlyrAnime(87, 0);
        }
        else if (rv[1] >= -3.1241393f && rv[1] <= -(PI / 3)) // 3.1241393 != PI !!
        {
            SetPlyrAnime(86, 0);
        }
    break;
    case 1:
        if (GetRndSP(0, 99) < 71)
        {
            SeStartPos(34, plyr_wrk.move_box.pos, 0, 0x1000, 0x1000, 0xff);
        }
        else
        {
            SeStartPos(35, plyr_wrk.move_box.pos, 0, 0x1000, 0x1000, 0xff);
        }

        SeStartPos(30, mb->pos, 0, 0x1000, 0x1000, 0xff);

        if (plyr_wrk.mode == 1)
        {
            FinderEndSet();

            plyr_wrk.dmg_cam_flag = 0;
        }

        if (plyr_wrk.mode != 2)
        {
            plyr_wrk.mode = 7;
        }

        plyr_wrk.sta &= ~0x10;
        plyr_wrk.cond_tm = 0;
        plyr_wrk.dmg = ew->dat->atk_h;

        if (ingame_wrk.difficult != 0 && ingame_wrk.game == 0)
        {
            plyr_wrk.dmg += ew->dat->atk_h + (ew->dat->atk_h / 2);
        }

        plyr_wrk.dmg_type = 2;
        plyr_wrk.atk_no = mb->idx;
        plyr_wrk.move_box.rot[1] = GetTrgtRotY(plyr_wrk.move_box.pos, mb->pos);

        if (ani == 0xff)
        {
            ani = pani_tbl[ew->dat->atk_ptn];
        }
        else
        {
            ani = pani_tbl[ani];
        }

        SetPlyrAnime(ani, 0);

        plyr_wrk.dwalk_tm = 60;

        ew->atk_tm = ew->dat->atk_tm * 30;
        ene_wrk[mb->idx].sta |= 0x8000;

        tv[0] = 0.0f;
        tv[1] = ene_wrk[mb->idx].adjp[1];
        tv[2] = ew->dat->hit_rng;
        tv[3] = 0.0f;

        RotFvector(plyr_wrk.move_box.rot, tv);
        sceVu0AddVector(mb->pos, plyr_wrk.move_box.pos, tv);

        GetTrgtRot(mb->pos, plyr_wrk.move_box.pos, mb->rot, 2);

        tv[0] = ew->dat->hit_adjx;
        tv[1] = 0.0f;
        tv[2] = 0.0f;
        tv[3] = 0.0f;

        RotFvector(mb->rot, tv);

        sceVu0AddVector(mb->pos, mb->pos, tv);

        mb->pos[1] = plyr_wrk.move_box.pos[1] + ew->adjp[1];
    break;
    case 2:
        if (GetRndSP(0, 99) < 71)
        {
            SeStartPos(33, plyr_wrk.move_box.pos, 0, 0x1000, 0x1000, 0xff);
        }
        else
        {
            SeStartPos(34, plyr_wrk.move_box.pos, 0, 0x1000, 0x1000, 0xff);
        }

        SeStartPos(32, mb->pos, 0, 0x1000, 0x1000, 0xff);

        ReqFinderInOverRap(0);

        if (plyr_wrk.mode == 1)
        {
            FinderEndSet();
        }

        plyr_wrk.sta &= ~0x10;
        plyr_wrk.cond_tm = 0;
        plyr_wrk.dmg = ew->dat->atk_p;

        if (ingame_wrk.difficult != 0 && ingame_wrk.game == 0)
        {
            plyr_wrk.dmg += ew->dat->atk_p + (ew->dat->atk_p / 2);
        }

        plyr_wrk.dmg_type = 0;
        plyr_wrk.dmg_cam_flag = 0;
        plyr_wrk.atk_no = mb->idx;
        ene_wrk[mb->idx].sta |= 0x8000;

        GetTrgtRot(mb->pos, plyr_wrk.move_box.pos, mb->rot, 2);

        if (!flag)
        {
            plyr_wrk.dmg_type = 1;
            plyr_wrk.mvsta &= ~0xf;

            GetTrgtRot(plyr_wrk.move_box.pos, mb->pos, rv, 2);

            rv[1] -= plyr_wrk.move_box.rot[1];

            RotLimitChk(&rv[1]);
            ConvertRot2Dir(rv[1], 2);

            SetPlyrAnime(62, 0);

            if (plyr_wrk.mode != 2)
            {
                plyr_wrk.mode = 7;
            }
        }

        mb->comm_add.wrk = adj + mb->comm_add_top;
    break;
    case 3:
        SeStartPos(35, plyr_wrk.move_box.pos, 0, 0x1000, 0x1000, 0xff);

        if (plyr_wrk.mode == 1)
        {
            FinderEndSet();

            plyr_wrk.dmg_cam_flag = 0;
        }

        plyr_wrk.mode = 7;
        plyr_wrk.sta &= ~0x10;
        plyr_wrk.cond_tm = 0;
        plyr_wrk.dmg = ew->dat->atk_h;

        if (ingame_wrk.difficult != 0 && ingame_wrk.game == 0)
        {
            plyr_wrk.dmg += ew->dat->atk_h + (ew->dat->atk_h / 2);
        }

        plyr_wrk.dmg_type = 2;
        plyr_wrk.atk_no = mb->idx;

        if (ani == 0xff)
        {
            ani = pani_tbl[ew->dat->atk_ptn];
        }
        else
        {
            ani = pani_tbl[ani];
        }

        SetPlyrAnime(ani, 0);

        plyr_wrk.dwalk_tm = 60;

        ew->atk_tm = ew->dat->atk_tm * 30;
        ene_wrk[mb->idx].sta |= 0x8000;
    break;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob010(MOVE_BOX *mb)
{
    u_char no;
    u_char type;

    no = mb->idx;

    type = *mb->comm_add.pu8++;

    ene_wrk[no].sta &= ~(0x10 | 0x8);

    switch (type)
    {
    case 1:
        ene_wrk[no].sta |= 0x8;
    break;
    case 2:
        ene_wrk[no].sta |= 0x10 | 0x8;
    break;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob011(MOVE_BOX *mb)
{
    ene_wrk[mb->idx].sta |= 0x2000;

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob012(MOVE_BOX *mb)
{
    u_char type;
    u_char no;

    type = *mb->comm_add.pu8++;
    no = *mb->comm_add.pu8++;

    switch (type)
    {
    case 0:
        mb->se = SeStartFix(no, 0, 0x1000, 0x1000, 0x0);
    break;
    case 1:
        mb->se = SeStartPos(no, mb->pos, 0, 0x1000, 0x1000, 0xff);
    break;
    case 2:
        if (mb->se != -1)
        {
            SeStop(mb->se);
            mb->se = -1;
        }
    break;
    case 3:
        if (ene_wrk[mb->idx].aie->se_foot != -1)
        {
            no = SFootGetSeNoFromFileNo(ene_wrk[mb->idx].aie->se_foot);
            mb->se = SeStartPos(no, ene_wrk[mb->idx].mpos.p1, 0, 0x1000, 0x1000, 0xff);
        }
    break;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob013(MOVE_BOX *mb)
{
    u_char cond;
    u_short adj;

    cond = *mb->comm_add.pu8++;
    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (cond == plyr_wrk.cond)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob014(MOVE_BOX *mb)
{
    u_short time;
    u_short adj;

    time = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (time != 0)
    {
        if (time_wrk.nwalk >= time || time_wrk.swalk >= time || time <= time_wrk.fwalk)
        {
            time = 0;
        }

    }

    if (time == 0 && plyr_wrk.mvsta & (0x4 | 0x2))
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob015(MOVE_BOX *mb)
{
    sceVu0FVECTOR rot = { 0.0f, 0.0f, 0.0f, 0.0f };
    static u_char turn[3];
    static u_char time[3];

    switch(mb->pos_no)
    {
    case 0:
        turn[mb->idx] = *mb->comm_add.pu8++;
        time[mb->idx] = *mb->comm_add.pu8++;

        ene_wrk[mb->idx].sta |= 0x800000;

        mb->loop = GetRndSP(time[mb->idx], 0x1e);

        GetTrgtRot(mb->pos, plyr_wrk.move_box.pos, rot, 1);

        if (turn[mb->idx] == 0x2)
        {
            rot[1] = GetTrgtRotY(mb->pos, plyr_wrk.move_box.pos) + PI;

            RotLimitChk(&rot[1]);
        }
        else
        {
            rot[1] = (GetRndSP(0, 0xe) - 7.0f) * 0.1f;

            RotLimitChk(&rot[1]);

            rot[1] = rot[1] > 0.0f ?  rot[1] + DEG2RAD(45.0f) : rot[1] - DEG2RAD(45.0f);
        }

        mb->spd[0] = 0.0f;
        mb->spd[1] = 0.0f;
        mb->spd[2] = 20.0f;
        mb->spd[3] = 0.0f;

        RotFvector(rot, mb->spd);

        mb->wait_time = 1;
        mb->pos_no++;
    break;
    case 1:
        if (mb->loop != 0)
        {
            if (--mb->loop < 31)
            {
                sceVu0ScaleVector(mb->spd, mb->spd, 0.95f);
            }

            mb->wait_time = 1;
        }
        else if (turn[mb->idx] == 2)
        {
            mb->wait_time = 1;
            mb->pos_no = 4;
        }
        else
        {
            mb->wait_time = 1;
            mb->pos_no++;
        }
    break;
    case 2:
        if (turn[mb->idx] != 0 && GetRndSP(0, 100) < 41)
        {
            mb->wait_time = 1;
            mb->pos_no = 4;
        }
        else
        {
            GetTrgtRot(mb->pos, plyr_wrk.move_box.pos, rot, 1);

            mb->loop = GetRndSP(time[mb->idx], 0x1e);

            rot[1] = GetRndSP(0, 14) * 0.1f;

            RotLimitChk(&rot[1]);

            rot[1] = rot[1] > 0.0f ? rot[1] + DEG2RAD(45.0f) : rot[1] - DEG2RAD(45.0f);

            mb->spd[2] = 20.0f;

            RotFvector(rot, mb->spd);

            mb->wait_time = 1;
            mb->pos_no++;
        }
    break;
    case 3:
        if (mb->loop == 0)
        {
            mb->wait_time = 1;
            mb->pos_no++;
        }
        else
        {
            if (--mb->loop < 31)
            {
                sceVu0ScaleVector(mb->spd, mb->spd, 0.95f);
            }

            mb->wait_time = 0x1;
        }
    break;
    case 4:
        ene_wrk[mb->idx].sta &= ~0x800000;

        mb->spd[0] = 0.0f;
        mb->spd[1] = 0.0f;
        mb->spd[2] = ene_wrk[mb->idx].dat->wspd;
        mb->spd[3] = 0.0f;

        mb->wait_time = 0;
        mb->pos_no = 0;
    break;
    }
}

void EJob016(MOVE_BOX *mb)
{
    u_char per;
    u_short adj;

    per = *mb->comm_add.pu8++;
    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (per >= GetRndSP(1, 99))
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob017(MOVE_BOX *mb)
{
    mb->loop = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob018(MOVE_BOX *mb)
{
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (mb->loop == 0)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }
    else
    {
        mb->loop--;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob019(MOVE_BOX *mb)
{
    u_char no;
    u_char flame;

    no = *mb->comm_add.pu8++;
    flame = *mb->comm_add.pu8++;

    ene_wrk[mb->idx].ani_reso = 1;
    ene_wrk[mb->idx].sta &= ~(0x40000000 | 0x20000000 | 0x10000000);
    ene_wrk[mb->idx].anime_no = no;

    ReqEnemyAnime(mb->idx, flame);

    if (ene_wrk[mb->idx].type < 2)
    {
        EneARatioDataSet(&ene_wrk[mb->idx], no);
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob01A(MOVE_BOX *mb)
{
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (ene_wrk[mb->idx].sta & 0x40000000)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob01B(MOVE_BOX *mb)
{
    ENE_WRK *ew;
    sceVu0FVECTOR tv;
    sceVu0FVECTOR rv;
    static float spd[3];
    static float rate[3];
    static float trate[3];
    static sceVu0FVECTOR p[3];
    u_short var1;
    u_short var2;
    u_short var3;
    u_char no;

    ew = &ene_wrk[mb->idx];
    no = mb->idx;

    var1 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    var2 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    var3 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    spd[no] = 1.0f;
    rate[no] = 2.0f;
    trate[no] = 1.0f;

    tv[0] = 0.0f;
    tv[1] = ew->bep[1];
    tv[2] = 300.0f;
    tv[3] = 0.0f;

    GetTrgtRot(mb->pos, camera.p, rv, 2);
    RotFvector(rv, tv);

    sceVu0AddVector(p[no], mb->pos, tv);

    SetEffects(EF_PDEFORM, 4, 23, 100, 0.5f, 1.0f, p[no], var1, var2, var3, 0, &spd[no], &rate[no], &trate[no]);

    ene_wrk[mb->idx].nee_col = 0x40404860;
    ene_wrk[mb->idx].nee_size = 10000.0f;

    if (ew->slight_svo != 0xff)
    {
        ene_wrk[no].mylight.spot[ew->slight_svo].diffuse[0] = 0.9f;
        ene_wrk[no].mylight.spot[ew->slight_svo].diffuse[1] = 0.9f;
        ene_wrk[no].mylight.spot[ew->slight_svo].diffuse[2] = 0.9f;
        ene_wrk[no].mylight.spot[ew->slight_svo].diffuse[3] = 255.0f;

        ene_wrk[no].mylight.parallel[0].diffuse[0] = 0.9f;
        ene_wrk[no].mylight.parallel[0].diffuse[1] = 0.9f;
        ene_wrk[no].mylight.parallel[0].diffuse[2] = 0.9f;
        ene_wrk[no].mylight.parallel[0].diffuse[3] = 255.0f;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob01C(MOVE_BOX *mb)
{
    u_char time;

    time = *mb->comm_add.pu8++;

    ReqQuake(0, 0.01f, time, 0, 0);

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob01D(MOVE_BOX *mb)
{
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (ingame_wrk.game == 1)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob01E(MOVE_BOX *mb)
{
    u_char req;
    u_short no;
    u_short time;

    req = *mb->comm_add.pu8++;
    no = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    time = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    ReqDramaCamera(req,no,time);

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob01F(MOVE_BOX *mb)
{
    u_short time;
    u_short adj;

    time = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (time != 0)
    {
        if (time_wrk.run >= time)
        {
            time = 0;
        }
    }

    if (time == 0 && plyr_wrk.mvsta & 0x1)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob020(MOVE_BOX *mb)
{
    u_char door_id_tbl[5] = { 3, 4, 5, 6, 1 };

    u_char id;
    u_short door_id;

    door_id = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    id = *mb->comm_add.pu8++;

    DoorSttsChange(door_id,door_id_tbl[id]);

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob021(MOVE_BOX *mb)
{
    u_short se;

    se = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (ene_wrk[mb->idx].se_area_no != -1)
    {
        if (ene_wrk[mb->idx].type == 2)
        {
            mb->se = SeStartPos(ene_wrk[mb->idx].se_area_no + 94, mb->pos, 0, 0x1000, 0x1000, 0xff);
        }
        else
        {
            mb->se = SeStartGhost(se, mb->idx, 0, 0x1000, 0x1000, ene_wrk[mb->idx].se_area_no);
        }
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob022(MOVE_BOX *mb)
{
    u_char no;
    static sceVu0FVECTOR ep[3];
    static float fade[3];
    static float rate[3];
    u_short var0;
    u_short var1;
    u_char unused1;
    u_char unused2;
    u_char unused3;

    no = mb->idx;

    switch (mb->pos_no)
    {
    case 0:
        unused1 = *mb->comm_add.pu8++;
        unused2 = *mb->comm_add.pu16++;

        var0 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
        var1 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

        unused3 = *mb->comm_add.pu16++;

        ep[no][0] = ene_wrk[no].bep[0];
        ep[no][1] = ene_wrk[no].bep[1];
        ep[no][2] = ene_wrk[no].bep[2];
        ep[no][3] = ene_wrk[no].bep[3];

        ene_wrk[no].sta |= 0x2000000;

        mb->loop = var0 + var1;

        fade[no] = ene_wrk[no].tr_rate / (float)mb->loop;
        rate[no] = ene_wrk[no].tr_rate;

        mb->wait_time = 1; mb->pos_no++;
    break;
    case 1:
        if (mb->loop == 0)
        {
            ene_wrk[no].sta &= ~0x2000000;

            mb->wait_time = 0;
            mb->pos_no = 0;
        }
        else
        {
            mb->loop--;

            rate[no] = rate[no] - fade[no];
            ene_wrk[no].tr_rate = rate[no];

            mb->wait_time = 1;
        }
    break;
    }
}

void EJob023(MOVE_BOX *mb)
{
    static sceVu0FVECTOR ep[3];
    u_short var0;
    u_short var1;
    u_char unused1;
    u_char unused2;
    u_char unused3;

    switch (mb->pos_no)
    {
    case 0:
        unused1 = *mb->comm_add.pu8++;
        unused2 = *mb->comm_add.pu16++;

        var0 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
        var1 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

        unused3 = *mb->comm_add.pu16++;

        ep[mb->idx][0] = ene_wrk[mb->idx].bep[0];
        ep[mb->idx][1] = ene_wrk[mb->idx].bep[1];
        ep[mb->idx][2] = ene_wrk[mb->idx].bep[2];
        ep[mb->idx][3] = ene_wrk[mb->idx].bep[3];

        mb->wait_time = var0 + var1;
        mb->pos_no++;
    break;
    case 1:
        mb->wait_time = 0;
        mb->pos_no = 0;
    break;
    }
}

void EJob024(MOVE_BOX *mb)
{
    mb->comm_add.pu8++;

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob025(MOVE_BOX *mb)
{
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (ene_wrk[mb->idx].atk_tm == 0)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }
    else if ((ene_wrk[mb->idx].atk_tm % 4) == 0)
    {
        ReqPlyrHPdown(ene_wrk[mb->idx].dat->atk, 0);
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob026(MOVE_BOX *mb)
{
    int rot;

    rot = mb->comm_add.pu8[0] | (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (rot == 1000)
    {
        mb->rot[1] = GetTrgtRotY(mb->pos, plyr_wrk.move_box.pos);
    }
    else
    {
        mb->rot[1] = (rot / 180.0f) * PI;

        RotLimitChk(&mb->rot[1]);
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob027(MOVE_BOX *mb)
{
    u_char chk;

    chk = *mb->comm_add.pu8++;

    if (chk)
    {
        ene_wrk[mb->idx].sta |= 0x800000;
    }
    else
    {
        ene_wrk[mb->idx].sta &= ~0x800000;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob028(MOVE_BOX *mb)
{
    MOVE_BOX *fmb;
    u_char no;
    u_char i;
    u_char id;
    u_short adj;
    u_long addr; // not in STAB

    no = *mb->comm_add.pu8++;
    id = *mb->comm_add.pu8++;

    for (i = 0; i < 10; i++)
    {
        if ((fly_wrk[i].sta & 0x1) == 0)
        {
            break;
        }
    }

    if (i < 10)
    {
        fly_wrk[i].sta = 1;
        fly_wrk[i].ene = mb->idx;
        fly_wrk[i].dat = &fly_dat[no];

        fmb = &fly_wrk[i].move_box;

        InitMoveBox(fmb);

        fmb->idx = i;

        fmb->pos[0] = ene_wrk[mb->idx].bep[0];
        fmb->pos[1] = ene_wrk[mb->idx].bep[1];
        fmb->pos[2] = ene_wrk[mb->idx].bep[2];
        fmb->pos[3] = ene_wrk[mb->idx].bep[3];

        fmb->comm_add_top = COMM_ADD_TOP_ADDRESS;
        
        addr = INDEX16(COMM_ADD_TOP_ADDRESS, 3);
        adj = READ_LE16(addr);
        addr = SEGMENT_ADDR(COMM_ADD_TOP_ADDRESS, adj);

        addr = INDEX16(addr, no);
        adj = READ_LE16(addr);
        fmb->comm_add.wrk = SEGMENT_ADDR(COMM_ADD_TOP_ADDRESS, adj);

        fmb->pos_no = 0;
        fmb->wait_time = 1;
        fmb->rot[1] = mb->rot[1];

        fly_wrk[i].adjp[0] = 0.0f;
        fly_wrk[i].adjp[1] = 0.0f;
        fly_wrk[i].adjp[2] = 0.0f;
        fly_wrk[i].adjp[3] = 0.0f;

        fly_wrk[i].adjmv[0] = 0.0f;
        fly_wrk[i].adjmv[1] = 0.0f;
        fly_wrk[i].adjmv[2] = 0.0f;
        fly_wrk[i].adjmv[3] = 0.0f;

        if (fly_dat[no].mdl_no != 0xffff)
        {
            RequestFly(i, 1);
        }

        if (id != 0)
        {
            fly_wrk[i].sta |= 0x8;
        }
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob029(MOVE_BOX *mb)
{
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (fly_wrk[mb->idx].sta & 0x2)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob02A(MOVE_BOX *mb)
{
    FlyDataEnd(mb->idx);

    mb->wait_time = 0xff;
    mb->pos_no = 0;
}

void FlyDataEnd(u_char no)
{
    fly_wrk[no].sta &= ~0x1;

    if (fly_wrk[no].dat->mdl_no != 0xffff)
    {
        RequestFly(no, 0);
    }
}

void EJob02B(MOVE_BOX *mb)
{
    u_char chk;
    u_char r;
    u_char g;
    u_char b;

    chk = *mb->comm_add.pu8++;

    r = *mb->comm_add.pu8++;
    g = *mb->comm_add.pu8++;
    b = *mb->comm_add.pu8++;

    if (chk != 0)
    {
        fly_wrk[mb->idx].ep = CallFire(mb->pos, r, g, b, 1.5f);
    }
    else
    {
        ResetEffects(fly_wrk[mb->idx].ep);
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob02C(MOVE_BOX *mb)
{
    u_short min;
    u_char rng;

    min = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    rng = *mb->comm_add.pu8++;

    if (rng != 0)
    {
        rng = GetRndSP(0, rng);
    }

    mb->loop = min + rng;

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob02D(MOVE_BOX *mb)
{
    sceVu0FVECTOR tv;
    sceVu0FVECTOR rv;
    short tv2;
    short rv1;

    rv1 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    tv2 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    tv[0] = 0.0f;
    tv[1] = 0.0f;
    tv[2] = tv2;
    tv[3] = 0.0f;

    rv[0] = 0.0f;
    rv[1] = DEG2RAD(rv1) + plyr_wrk.move_box.rot[1];
    rv[2] = 0.0f;
    rv[3] = 0.0f;

    RotLimitChk(&rv[1]);
    RotFvector(rv, tv);
    sceVu0AddVector(tv, tv, plyr_wrk.move_box.pos);
    GetTrgtRot(mb->pos, tv, mb->trot, 3);

    mb->rot[0] = mb->trot[0];
    mb->trot[3] = DEG2RAD(2.0f);

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob02E(MOVE_BOX *mb)
{
    ENE_WRK *ew;
    u_char val;
    u_short adj;

    ew = &ene_wrk[mb->idx];

    val = *mb->comm_add.pu8++;
    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (ew->hp <= (ew->dat->hp * val) / 100)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob02F(MOVE_BOX *mb)
{
    u_char chk;

    chk = *mb->comm_add.pu8++;

    if (chk)
    {
        ene_wrk[mb->idx].sta |= 0x400000;
    }
    else
    {
        ene_wrk[mb->idx].sta &= ~0x400000;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob030(MOVE_BOX *mb)
{
    sceVu0FVECTOR tv;
    sceVu0FVECTOR rv;
    short rot;
    u_char min;
    u_char max;

    rot = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    min = *mb->comm_add.pu8++;
    max = *mb->comm_add.pu8++;

    if (rot != 0)
    {
        rot = GetRndSP(0, (u_char)(rot * 2)) - rot;
    }

    rv[0] = 0.0f;
    rv[1] = DEG2RAD(rot) + plyr_wrk.move_box.rot[1];
    rv[2] = 0.0f;
    rv[3] = 0.0f;

    RotLimitChk(rv + 1);

    if (min == 0 && max == 0)
    {
        tv[0] = 0.0f;
        tv[1] = 0.0f;
        tv[2] = GetDistV(mb->pos, plyr_wrk.move_box.pos);
        tv[3] = 0.0f;
    }
    else if (max != min)
    {
        tv[0] = 0.0f;
        tv[1] = 0.0f;
        tv[2] = GetRndSP(min, max - min) * 500;
        tv[3] = 0.0f;
    }
    else
    {
        tv[0] = 0.0f;
        tv[1] = 0.0f;
        tv[2] = min * 500;
        tv[3] = 0.0f;
    }

    RotFvector(rv, tv);

    sceVu0AddVector(tv, plyr_wrk.move_box.pos, tv);

    tv[1] = mb->pos[1] + ene_wrk[mb->idx].adjp[1];

    mb->pos[0] = tv[0];
    mb->pos[1] = tv[1];
    mb->pos[2] = tv[2];
    mb->pos[3] = tv[3];

    EnePosInfoSet(&ene_wrk[mb->idx]);

    EneBlinkPosSet(&ene_wrk[mb->idx]);

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob031(MOVE_BOX *mb)
{
    u_char val;
    u_short adj;

    val = *mb->comm_add.pu8++;
    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (((ene_wrk[mb->idx].dat)->hp * val) / 100 <= ene_wrk[mb->idx].dmg_old)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob032(MOVE_BOX *mb)
{
    u_short val;
    u_short adj;

    val = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (val <= GetDistV(plyr_wrk.move_box.pos,mb->pos))
    {
        (mb->comm_add).wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob033(MOVE_BOX *mb)
{
    u_char chk;

    chk = *(mb->comm_add).pu8++;

    if (chk)
    {
        ene_wrk[mb->idx].sta |= 0x1000000;
    }
    else
    {
        ene_wrk[mb->idx].sta &= ~0x1000000;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob034(MOVE_BOX *mb)
{
    u_char chk;

    chk = *(mb->comm_add).pu8++;

    if (chk)
    {
        ene_wrk[mb->idx].sta |= 0x2000000;
    }
    else
    {
        ene_wrk[mb->idx].sta &= ~0x2000000;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob035(MOVE_BOX *mb)
{
    u_char chk;

    chk = *(mb->comm_add).pu8++;

    if (chk)
    {
        ene_wrk[mb->idx].sta |= 0x4000;
    }
    else
    {
        ene_wrk[mb->idx].sta &= ~0x4000;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob036(MOVE_BOX *mb)
{
    int rot;
    u_short adj;

    rot = mb->comm_add.pu8[0] | (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    adj = mb->comm_add.pu8[0] | (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (RotRngChk(mb->pos, plyr_wrk.move_box.pos, mb->rot[1], (rot / 180.0f) * PI) != 0)
    {
        (mb->comm_add).wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob037(MOVE_BOX *mb)
{
    sceVu0FVECTOR tv;
    u_short time;
    short spd;
    u_char chk;

    switch (mb->pos_no)
    {
    case 0:
        chk = *mb->comm_add.pu8++;

        time = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
        spd = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

        if (chk)
        {
            spd = -spd;
        }

        mb->spd[0] = spd;
        mb->spd[1] = 0.0f;
        mb->spd[2] = 0.0f;
        mb->spd[3] = 0.0f;

        sceVu0AddVector(tv, mb->pos, mb->spd);

        GetPointHeightChk(tv);

        tv[1] -= mb->pos[1];

        if (__builtin_fabsf(tv[1]) > 5.0f)
        {
            tv[1] = tv[1] < 0.0f ? -5.0f : 5.0f;
        }

        mb->spd[1] = tv[1];

        mb->wait_time = time;
        mb->pos_no++;
    break;
    case 1:
        mb->spd[0] = 0.0f;
        mb->spd[1] = 0.0f;
        mb->spd[2] = 0.0f;
        mb->spd[3] = 0.0f;

        mb->wait_time = 0;
        mb->pos_no = 0;
    break;
    }
}

void EJob038(MOVE_BOX *mb)
{
    sceVu0FVECTOR tv;
    sceVu0FVECTOR rv;
    float rot;
    u_char side;
    u_char type;

    type = *mb->comm_add.pu8++;
    side = *mb->comm_add.pu8++;
    rot = mb->comm_add.pu8[0] | (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    tv[0] = 0.0f;
    tv[1] = 0.0f;
    tv[2] = 0.0f;
    tv[3] = 0.0f;

    tv[2] = GetDistV(plyr_wrk.move_box.pos, mb->pos);

    switch (type)
    {
    case 1:
        // do nothing ...
    break;
    case 2:
        tv[2] -= ene_wrk[mb->idx].dat->wspd;
    break;
    default:
        tv[2] -= ene_wrk[mb->idx].dat->rspd;
    break;
    }

    GetTrgtRot(plyr_wrk.move_box.pos, mb->pos, rv, 2);

    if (side == 0)
    {
        rv[1] -= DEG2RAD(rot / 10.0f);
    }
    else
    {
        rv[1] += DEG2RAD(rot / 10.0f);
    }

    RotLimitChk(&rv[1]);
    RotFvector(rv, tv);

    sceVu0AddVector(tv, tv, plyr_wrk.move_box.pos);
    sceVu0SubVector(tv, tv, mb->pos);

    tv[1] = 0.0f;

    mb->spd[0] = tv[0];
    mb->spd[1] = tv[1];
    mb->spd[2] = tv[2];
    mb->spd[3] = tv[3];

    sceVu0AddVector(tv, tv, mb->pos);

    GetPointHeightChk(tv);

    tv[1] = (tv[1] + ene_wrk[mb->idx].adjp[1]) - mb->pos[1];

    if (__builtin_fabsf(tv[1]) > 1.0f)
    {
        tv[1] = tv[1] < 0.0f ? -1.0f : 1.0f;
    }

    mb->spd[1] = tv[1];

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob039(MOVE_BOX *mb)
{
    u_short time;
    u_short max;
    u_short mrng;
    u_char chk;
    u_char trng;

    chk = *mb->comm_add.pu8++;
    time = *mb->comm_add.pu8++;
    trng = *mb->comm_add.pu8++;
    max = *mb->comm_add.pu8++;
    mrng = *mb->comm_add.pu8++;

    if (chk)
    {
        ene_wrk[mb->idx].sta |= 0x4000;

        if (trng != 0)
        {
            time = GetRndSP(time, trng);
        }

        ene_wrk[mb->idx].tr_time = time;

        if (mrng != 0)
        {
            max = GetRndSP(max, mrng);
        }

        ene_wrk[mb->idx].tr_max = max;
    }
    else
    {
        ene_wrk[mb->idx].sta &= ~0x4000;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob03A(MOVE_BOX *mb)
{
    u_short adj;

    adj = mb->comm_add.pu8[0] | (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (ene_wrk[mb->idx].sta & 0x400)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob03B(MOVE_BOX *mb)
{
    u_char cond;
    u_short cond_tm;

    cond = *mb->comm_add.pu8++;
    cond_tm = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (plyr_wrk.cond != cond)
    {
        plyr_wrk.cond = cond;
        plyr_wrk.cond_tm = cond_tm;

        if (cond == 2)
        {
            if (plyr_wrk.mode == 1)
            {
                FinderEndSet();

                plyr_wrk.mode = 3;
            }

            plyr_wrk.cond_tm = GetDistV(mb->pos,plyr_wrk.move_box.pos) / 2.6f;
        }
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob03C(MOVE_BOX *mb)
{
    u_short adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (ene_wrk[mb->idx].sta & 0x20)
    {
        (mb->comm_add).wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob03D(MOVE_BOX *mb)
{
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (plyr_wrk.aphoto_tm != 0)
    {
        plyr_wrk.aphoto_tm = 0;

        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob03E(MOVE_BOX *mb)
{
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if ((ene_wrk[mb->idx].sta & 0x200) == 0)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob03F(MOVE_BOX *mb)
{
    ene_wrk[mb->idx].tr_rate = 0;

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob040(MOVE_BOX *mb)
{
    u_char no;
    static u_short time[4];
    static u_char max[4];
    static float up[4][2];

    no = mb->idx;

    switch (mb->pos_no)
    {
    case 0:
        time[no] = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
        max[no] = *mb->comm_add.pu8++;

        up[no][0] = max[no] / (float)time[no];
        up[no][1] = 0.0f;

        ene_wrk[no].tr_rate = 0;

        mb->pos_no++;
    case 1:
        if (time[no] == 0)
        {
            ene_wrk[no].tr_rate = max[no];
            mb->wait_time = 0;
            mb->pos_no = 0;
        }
        else
        {
            time[no]--;
            up[no][1] += up[no][0];
            ene_wrk[no].tr_rate = up[no][1];
            mb->wait_time = 1;
        }
    break;
    }
}

void EJob041(MOVE_BOX *mb)
{
    u_char no;
    static u_short time[4];
    static float down[4][2];

    no = mb->idx;

    switch (mb->pos_no)
    {
    case 0:
        time[no] = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

        if (ene_wrk[no].tr_rate == 0)
        {
            mb->wait_time = 0; mb->pos_no = 0; return;
        }

        down[no][0] = ene_wrk[no].tr_rate / (float)time[no];
        down[no][1] = ene_wrk[no].tr_rate;
        mb->pos_no++;
    case 1:
        if (time[no] == 0)
        {
            ene_wrk[no].tr_rate = 0;

            mb->wait_time = 0;
            mb->pos_no = 0;
        }
        else
        {
            time[no]--;

            if (down[no][1] > 0.0f)
            {
                down[no][1] -= down[no][0];
                ene_wrk[no].tr_rate = down[no][1];
            }

            mb->wait_time = 1;
        }
    break;
    }
}

void EJob042(MOVE_BOX *mb)
{
    u_short no;
    u_short adj;
    u_char brnch;
    u_char chk;

    brnch = 0;

    chk = *mb->comm_add.pu8++;

    no = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (chk != 0)
    {
        if ((plyr_wrk.sta & 0x8) || (plyr_wrk.mvsta & 0x200000))
        {
            brnch = 1;
        }
        else
        {
            brnch = 0;
        }
    }
    else
    {
        if (no == 0xffff)
        {
            no = plyr_wrk.dop.door_id;
        }

        if (plyr_wrk.sta & 0x2000 && plyr_wrk.sta & 0x8)
        {
            brnch = plyr_wrk.dop.door_id == no;
        }
    }

    if (brnch)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob043(MOVE_BOX *mb)
{
    u_char chk;

    chk = *mb->comm_add.pu8++;

    if (chk)
    {
        ene_wrk[mb->idx].sta |= 0x80000;
    }
    else
    {
        ene_wrk[mb->idx].sta &= ~0x80000;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob044(MOVE_BOX *mb)
{
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (plyr_wrk.mode == 0x1)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob045(MOVE_BOX *mb)
{
    u_char *puVar1;

    ene_wrk[mb->idx].tr_rate = *mb->comm_add.pu8++;

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob046(MOVE_BOX *mb)
{
    short tv0;
    short tv1;
    short tv2;
    sceVu0FVECTOR tv;

    tv0 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    tv1 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    tv2 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    tv[0] = tv0;
    tv[1] = tv1;
    tv[2] = tv2;
    tv[3] = 0.0f;

    RotFvector(mb->rot, tv);

    sceVu0AddVector(mb->pos, mb->pos, tv);

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob047(MOVE_BOX *mb)
{
    u_short adj;
    u_short rng;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (ene_wrk[mb->idx].type == 2)
    {
        rng = (ene_wrk[mb->idx].aie)->rng;

        if (rng != 0)
        {
            if (rng < GetDistV(plyr_wrk.move_box.pos, ene_wrk[mb->idx].mpos.p0))
            {
                rng = 0;
            }
        }

        if (rng != 0 || (ene_wrk[mb->idx].sta & 0x800) != 0)
        {
            mb->comm_add.wrk = adj + mb->comm_add_top;
        }
    }
    else
    {
        if (ene_wrk[mb->idx].sta & 0x800)
        {
            ene_wrk[mb->idx].sta &= ~0x800;

            mb->comm_add.wrk = adj + mb->comm_add_top;
        }
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob048(MOVE_BOX *mb)
{
    u_char no;
    static float fade[3];
    static float rate[3];

    no = mb->idx;

    switch (mb->pos_no)
    {
    case 0:
        mb->loop = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

        fade[no] = (float)ene_wrk[no].tr_rate / (float)mb->loop;
        rate[no] = (float)ene_wrk[no].tr_rate;

        mb->pos_no++;
    case 1:
        if (mb->loop == 0)
        {
            mb->wait_time = 0;
            mb->pos_no = 0;
        }
        else
        {
            mb->loop--;

            rate[no] = fade[no] < rate[no] ? rate[no] - fade[no] : 0.0f;

            ene_wrk[no].tr_rate = rate[no];

            mb->wait_time = 1;
        }
    }
}

void EJob049(MOVE_BOX *mb)
{
    u_short a, c, d, e, f, g;
    float b;
    float spd;

    a = mb->comm_add.pu8[0] | (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    b = mb->comm_add.pu8[0] | (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    spd = (mb->comm_add.pu8[0] | (mb->comm_add.pu8[1] << 8)) * 0.1f; mb->comm_add.pu16++;
    c = mb->comm_add.pu8[0] | (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    d = mb->comm_add.pu8[0] | (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    e = mb->comm_add.pu8[0] | (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    f = mb->comm_add.pu8[0] | (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    g = mb->comm_add.pu8[0] | (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    SetEffects(2, 4, a, b, spd, c, d, e, f, g);

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob04A(MOVE_BOX *mb)
{
    u_char time;

    switch (mb->pos_no)
    {
    case 0:
        time = 10;

        if (ene_wrk[mb->idx].sta2 & 0x2)
        {
            mb->spd[0] = 0.0f;
            mb->spd[1] = 0.0f;
            mb->spd[2] = 0.0f;
            mb->spd[3] = 0.0f;
        }
        else
        {
            time = 40;
        }

        ene_wrk[mb->idx].sta2 &= ~0x2;

        mb->wait_time = time;
        mb->pos_no++;
    break;
    case 1:
        mb->wait_time = 0;
        mb->pos_no = 0;
    break;
    }
}

void EJob04B(MOVE_BOX *mb)
{
    u_short adj[3];

    adj[0] = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    adj[1] = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    adj[2] = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    switch (ene_wrk[mb->idx].dmg_type)
    {
    case 1:
        mb->comm_add.wrk = mb->comm_add_top + adj[0];
    break;
    case 2:
        mb->comm_add.wrk = mb->comm_add_top + adj[1];
    break;
    default:
        mb->comm_add.wrk = mb->comm_add_top + adj[2];
    break;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob04C(MOVE_BOX *mb)
{
    u_short time;
    u_short adj;
    u_char id;

    time = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    id = *mb->comm_add.pu8++;

    if (id != 0)
    {
        if (ene_wrk[mb->idx].sta & 0x200)
        {
            id = 0;
        }
    }

    if (id == 0 && ene_wrk[mb->idx].in_finder_tm >= time)
    {
        (mb->comm_add).wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob04D(MOVE_BOX *mb)
{
    u_short adj;
    u_short unused;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    unused = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); // no increment

    if (time_wrk.zerohour == 0)
    {
        if ((plyr_wrk.sta & 0x8) == 0)
        {
            mb->comm_add.wrk = adj + mb->comm_add_top;
        }
    }
    else
    {
        time_wrk.zerohour = time_wrk.zerohour - 1;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob04E(MOVE_BOX *mb)
{
    u_char chk;
    u_char bone_id;

    chk = *mb->comm_add.pu8++;
    bone_id = *mb->comm_add.pu8++;

    if (chk)
    {
        motSetEnemyKuttuki(mb->idx, bone_id, ene_wrk[mb->idx].dat->hit_rng, -(PI / 2));
    }
    else
    {
        motReleaseEnemyKuttuki(mb->idx);
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob04F(MOVE_BOX *mb)
{
    u_char chk;

    chk = *mb->comm_add.pu8++;

    if (chk)
    {
        ene_wrk[mb->idx].sta |= 0x10000;
    }
    else
    {
        ene_wrk[mb->idx].sta &= ~(0x20000 | 0x10000);
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob050(MOVE_BOX *mb)
{
    short adjp0;
    short adjp1;
    short adjp2;

    adjp0 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    adjp1 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    adjp2 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    ene_wrk[mb->idx].adjp[0] = adjp0;
    ene_wrk[mb->idx].adjp[1] = adjp1;
    ene_wrk[mb->idx].adjp[2] = adjp2;
    ene_wrk[mb->idx].adjp[3] = 0.0f;

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob051(MOVE_BOX *mb)
{
    u_char ani_reso;

    ani_reso = *mb->comm_add.pu8++;

    ene_wrk[mb->idx].ani_reso = ani_reso;

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob052(MOVE_BOX *mb)
{
    u_char type;
    u_char volume;

    type = *mb->comm_add.pu8++;
    volume = *mb->comm_add.pu8++;

    SetEffects(EF_DEFORM, 1, type, volume);

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob053(MOVE_BOX *mb)
{
    sceVu0FVECTOR vb;
    sceVu0FVECTOR vt;
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    vb[0] = mb->pos[0];
    vb[1] = mb->pos[1];
    vb[2] = mb->pos[2];
    vb[3] = mb->pos[3];

    vb[1] -= 300.0f;

    vt[0] = plyr_wrk.move_box.pos[0];
    vt[1] = plyr_wrk.move_box.pos[1];
    vt[2] = plyr_wrk.move_box.pos[2];
    vt[3] = plyr_wrk.move_box.pos[3];

    vt[1] -= 300.0f;

    if (HitChkSegment2AllChk(vb,vt,GetDistV(vb,vt) / 50.0f) == 0)
    {
        (mb->comm_add).wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob054(MOVE_BOX *mb)
{
    u_char time;

    time = *mb->comm_add.pu8++;

    SetWhiteIn2(time);

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob055(MOVE_BOX *mb)
{
    u_char dir;
    u_short adj;

    dir = *mb->comm_add.pu8++;
    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (dir == ene_wrk[mb->idx].dat->dir)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob056(MOVE_BOX *mb)
{
    int *erd;
    u_short x;
    short y;
    u_short z;
    u_char i;
    u_char n;

    n = mb->idx;

    erd = GetEneRootData(n);

    for(i = 0;;i++)
    {
        x = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
        y = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
        z = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

        if (x == 0xffff && z == 0xffff)
        {
            break;
        }

        erd[i * 3 + 0] = x;
        erd[i * 3 + 1] = y;
        erd[i * 3 + 2] = z;
    }

    er_max_tbl[n] = i;

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob057(MOVE_BOX *mb)
{
    sceVu0FVECTOR tv;
    u_char chk;

    chk = *mb->comm_add.pu8++;

    tv[0] = mb->pos[0];
    tv[1] = mb->pos[1];
    tv[2] = mb->pos[2];
    tv[3] = mb->pos[3];

    GetPointHeightChk(tv);

    if (chk)
    {
        if (__builtin_fabsf(tv[1] - mb->pos[1]) > 5.0f)
        {
            if (tv[1] < mb->pos[1])
            {
                tv[1] = mb->pos[1] - 5.0f;
            }
            else
            {
                tv[1] = mb->pos[1] + 5.0f;
            }
        }
    }

    mb->pos[1] = tv[1];

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob058(MOVE_BOX *mb)
{
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (mb->pos[1] != plyr_wrk.move_box.pos[1])
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob059(MOVE_BOX *mb)
{
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (plyr_wrk.mode == 2)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob05A(MOVE_BOX *mb)
{
    u_char chk;

    chk = *mb->comm_add.pu8++;

    if (chk)
    {
        ene_wrk[mb->idx].sta |= 0x20000;
    }
    else
    {
        ene_wrk[mb->idx].sta &= ~0x20000;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob05B(MOVE_BOX *mb)
{
    sceVu0FVECTOR rv;
    u_char i;
    short rv0;
    short rv1;
    short rv2;

    rv0 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    rv1 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    rv2 = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    rv[0] = DEG2RAD(rv0);
    rv[1] = DEG2RAD(rv1);
    rv[2] = DEG2RAD(rv2);
    rv[3] = 0.0f;

    for(i = 0; i < 3; i++)
    {
        RotLimitChk(&rv[i]);
    }

    sceVu0AddVector(mb->rot, rv, rv);

    for(i = 0; i < 3; i++)
    {
        RotLimitChk(&rv[i]);
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob05C(MOVE_BOX *mb)
{
    u_char no;
    u_short adj;

    no = *mb->comm_add.pu8++;
    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (no == ene_wrk[mb->idx].anime_no)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob05D(MOVE_BOX *mb)
{
    ReqEneSpeakAdpcmCnt(ene_wrk[mb->idx].aie->adpcm_tm);

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob05E(MOVE_BOX *mb)
{
    ene_wrk[mb->idx].tr_rate2 = *mb->comm_add.pu8++;

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob05F(MOVE_BOX *mb)
{
    sceVu0FVECTOR tv;
    u_char a;
    short b;

    a = *mb->comm_add.pu8++;
    b = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    tv[0] = 0.0f;
    tv[1] = 0.0f;
    tv[2] = GetDistV2(mb->pos, plyr_wrk.move_box.pos) + b;
    tv[3] = 0.0f;

    tv[2] /= a;

    RotFvector(mb->rot, tv);

    mb->spd[0] = tv[0];
    mb->spd[1] = tv[1];
    mb->spd[2] = tv[2];
    mb->spd[3] = tv[3];

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob060(MOVE_BOX *mb)
{
    GetTrgtRot(mb->pos, camera.p, mb->rot, 3);

    mb->trot[3] = 0.0f;

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob061(MOVE_BOX *mb)
{
    u_char val;

    switch (mb->pos_no)
    {
    case 0:
        val = *(mb->comm_add).pu8++;

        ene_wrk[mb->idx].sta = ene_wrk[mb->idx].sta & ~0x40000;

        SetEffects(EF_ENEOUT, 8, mb->idx, val, 1.0f);

        mb->wait_time = 1;
        mb->pos_no++;
    break;
    case 1:
        GhostDeadInit(mb->idx);
        RequestSpirit(mb->idx, 0);
        ReleaseEneTexture(mb->idx);
        motReleaseAnmPacket(mb->idx);

        mb->wait_time = 0;
        mb->pos_no = 0;
    break;
    }
}

void EJob062(MOVE_BOX *mb)
{
    sceVu0FVECTOR rv;
    sceVu0FVECTOR tv;
    u_char req;
    u_char i;

    req = *mb->comm_add.pu8++;

    switch (req)
    {
    case 0:
        plyr_wrk.sta &= ~0x1000;

        FinderEndSet();

        plyr_wrk.mode = 0;

        ReqEneStop(0, 0);

        disp3d_room_req = 1;
        disp3d_furn_req = 1;

        for (i = 0; i < 3; i++)
        {
            if (ene_wrk[i].sta & 0x1 && ene_wrk[i].hp != 0 && ene_wrk[i].room_no == plyr_wrk.pr_info.room_no)
            {
                plyr_wrk.mode = 0x1;
                plyr_wrk.sta |= 0x10;

                SetDebugMenuSwitch(0);

                plyr_wrk.mvsta &= ~(0x8 | 0x4 | 0x2 | 0x1);

                break;
            }
        }
    break;
    case 1:
        plyr_wrk.mode = 1;
        plyr_wrk.sta &= ~0x1000;

        ReqEneStop(0, 0);
    break;
    case 2:
        plyr_wrk.mode = 1;
        plyr_wrk.sta |= 0x10;

        SetDebugMenuSwitch(0);

        plyr_wrk.sta |= 0x1000;
        plyr_wrk.mvsta &= ~(0x8 | 0x4 | 0x2 | 0x1);

        SetPlyrAnime(67, 0);
        ReqEneStop(2, 1 << mb->idx);

        plyr_wrk.rock_trgt = mb->idx;

        for (i = 0; i < 10; i++)
        {
            if (fly_wrk[i].sta & 0x1 && fly_wrk[i].ene == mb->idx && fly_wrk[i].dat->mdl_no != 0xffff)
            {
                FlyDataEnd((u_char)i);
            }
        }
    break;
    case 3:
        plyr_wrk.mode = 9;

        tv[0] = 0.0f;
        tv[1] = 0.0f;
        tv[2] = 750.0f;
        tv[3] = 0.0f;

        GetTrgtRot(ene_wrk[mb->idx].mpos.p0, camera.p, rv, 3);
        RotFvector(rv, tv);

        sceVu0AddVector(camera.p, ene_wrk[mb->idx].mpos.p0, tv);

        if (mb->se != -1)
        {
            SeStop(mb->se);
        }

        for(i = 0; i < 2; i++)
        {
            if (ene_wrk[mb->idx].se_omen[i] != -1)
            {
                SeStop(ene_wrk[mb->idx].se_omen[i]);

                ene_wrk[mb->idx].se_omen[i] = -1;
            }
        }

        if (
            PosInAreaJudge0(room_wrk.room_no, camera.p[2], camera.p[0]) == 0 ||
            PosInAreaJudge0(room_wrk.room_no, ene_wrk[mb->idx].mpos.p0[2], ene_wrk[mb->idx].mpos.p0[0]) == 0
        )
        {
            disp3d_room_req = 0;
            disp3d_furn_req = 0;
        }
    break;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob063(MOVE_BOX *mb)
{
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (ene_wrk[mb->idx].sta & 0x40000)
    {
        if (GhostDeadLoadEndJudge() != 0)
        {
            mb->comm_add.wrk = adj + mb->comm_add_top;
        }
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob064(MOVE_BOX *mb)
{
    sceVu0FVECTOR tv;
    short int x;
    short int y;
    short int z;
    u_char type;

    type = *mb->comm_add.pu8++;

    x = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    y = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    z = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    switch (type)
    {
    case 0:
        fly_wrk[mb->idx].adjp[3] = 0.0f;
        fly_wrk[mb->idx].adjmv[3] = 0.0f;
    break;
    case 1:
        fly_wrk[mb->idx].adjp[0] = x;
        fly_wrk[mb->idx].adjp[1] = y;
        fly_wrk[mb->idx].adjp[2] = z;
        fly_wrk[mb->idx].adjp[3] = 1.0f;
    break;
    default:
        tv[0] = x;
        tv[1] = y;
        tv[2] = z;
        tv[3] = 0.0f;

        sceVu0AddVector(fly_wrk[mb->idx].adjmv, fly_wrk[mb->idx].adjmv, tv);

        fly_wrk[mb->idx].adjmv[3] = 1.0f;
    break;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob065(MOVE_BOX *mb)
{
    u_char i;

    for (i = 0; i < 10; i++)
    {
        if (fly_wrk[i].sta & 0x1 && fly_wrk[i].ene == mb->idx)
        {
            fly_wrk[i].sta |= 0x8;

            ene_wrk[mb->idx].sta |= 0x4000000;
        }
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob066(MOVE_BOX *mb)
{
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (fly_wrk[mb->idx].sta & 0x8)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob067(MOVE_BOX *mb)
{
    u_char id;
    u_short rot;
    float adj;

    id = *mb->comm_add.pu8++;
    rot = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (id & 0x1)
    {
        adj = DEG2RAD((float)GetRndSP(0, rot * 2) - rot);

        RotLimitChk(&adj);

        mb->rot[0] += adj;

        RotLimitChk(&mb->rot[0]);
    }

    if (id & 0x2)
    {
        adj = DEG2RAD((float)GetRndSP(0, rot * 2) - rot);

        RotLimitChk(&adj);

        mb->rot[1] += adj;

        RotLimitChk(&mb->rot[1]);
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob068(MOVE_BOX *mb)
{
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (ene_wrk[mb->idx].sta & 0x4000000)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob069(MOVE_BOX *mb)
{
    u_short adj;
    u_char i;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    for (i = 0; i < 10; i++)
    {
        if (fly_wrk[i].sta & 0x1 && fly_wrk[i].ene == mb->idx)
        {
            break;
        }
    }

    if (i >= 10)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob06A(MOVE_BOX *mb)
{
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (plyr_wrk.mode == 3)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob06B(MOVE_BOX *mb)
{
    sceVu0FVECTOR pv;
    sceVu0FVECTOR tv;
    sceVu0FVECTOR rv;
    float rot;
    float dist;
    short spd;
    u_char var;
    u_char side;

    var = *mb->comm_add.pu8++;
    side = *mb->comm_add.pu8++;

    rot = mb->comm_add.pu8[0] | (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;
    spd = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    pv[0] = plyr_wrk.move_box.pos[0];
    pv[1] = plyr_wrk.move_box.pos[1];
    pv[2] = plyr_wrk.move_box.pos[2];
    pv[3] = plyr_wrk.move_box.pos[3];

    pv[1] += -687.0f;

    fly_wrk[mb->idx].sta |= 0x10;

    dist = GetDistV(pv, mb->pos);

    if (var == 1)
    {
        tv[0] = 0.0f;
        tv[2] = dist;
        tv[1] = 0.0f;
        tv[3] = 0.0f;
    }
    else
    {
        tv[0] = 0.0f;
        tv[1] = 0.0f;
        tv[2] = dist - spd;
        tv[3] = 0.0f;
    }


    GetTrgtRot(pv, mb->pos, rv, 3);

    if (side == 0)
    {
        rv[1] -= DEG2RAD(rot / 10.0f);
    }
    else
    {
        rv[1] += DEG2RAD(rot / 10.0f);
    }

    RotLimitChk(rv + 1);
    RotFvector(rv, tv);

    sceVu0AddVector(tv, tv, pv);
    sceVu0SubVector(tv, tv, mb->pos);

    mb->spd[0] = tv[0];
    mb->spd[1] = tv[1];
    mb->spd[2] = tv[2];
    mb->spd[3] = tv[3];

    GetTrgtRot(mb->pos, pv, mb->rot, 3);

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob06C(MOVE_BOX *mb)
{
    plyr_wrk.hp = 1;

    ReqPlyrHPdown(100, 1);

    if (plyr_wrk.mode == 1)
    {
        FinderEndSet();

        plyr_wrk.mode = 0;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob06D(MOVE_BOX *mb)
{
    u_short adj;
    u_char charge;

    charge = *mb->comm_add.pu8++;
    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (charge == 0xff)
    {
        charge = charge_max_tbl[cam_custom_wrk.charge_max];
    }

    if (plyr_wrk.charge_num >= charge)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob06E(MOVE_BOX *mb)
{
    u_short var;

    var = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    ene_wrk[mb->idx].hp += var;

    if (ene_wrk[mb->idx].dat->hp < ene_wrk[mb->idx].hp)
    {
        ene_wrk[mb->idx].hp = ene_wrk[mb->idx].dat->hp;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob06F(MOVE_BOX *mb)
{
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (ene_wrk[mb->idx].sta & 0x100)
    {
        (mb->comm_add).wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob070(MOVE_BOX *mb)
{
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    if (point_get_end & 0xff)
    {
        mb->comm_add.wrk = adj + mb->comm_add_top;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob071(MOVE_BOX *mb)
{
    GhostDeadInit(mb->idx);
    RequestSpirit(mb->idx, 0);
    ReleaseEneTexture(mb->idx);
    motReleaseAnmPacket(mb->idx);

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob072(MOVE_BOX *mb)
{
    float dist;
    u_short adj;

    adj = mb->comm_add.pu8[0] + (mb->comm_add.pu8[1] << 8); mb->comm_add.pu16++;

    dist = GetDistV(mb->pos, plyr_wrk.move_box.pos);

    if (mb->pos[1] == plyr_wrk.move_box.pos[1])
    {
        if (dist <= 2000.0f)
        {
            if (HitChkSegment2AllChk(mb->pos, plyr_wrk.move_box.pos, dist / 50.0f) == 0x0)
            {
                mb->comm_add.wrk = adj + mb->comm_add_top;
            }
        }
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void EJob073(MOVE_BOX *mb)
{
    u_char chk;

    chk = *mb->comm_add.pu8++;

    if (chk)
    {
        fly_wrk[mb->idx].sta |= 0x8;
    }
    else
    {
        fly_wrk[mb->idx].sta &= ~0x8;
    }

    mb->wait_time = 0;
    mb->pos_no = 0;
}

void BJobDammy(ENE_WRK *ew)
{
    return;
}

void BJob000(ENE_WRK *ew)
{
    u_char time;

    switch (ew->bpos_no)
    {
    case 0:
        time = *ew->bcomm_add.pu8++;

        if (time == 0)
        {
            ew->bwait_time = 1;
            ew->bpos_no = 2;
        }
        else
        {
            ew->bwait_time = time;
            ew->bpos_no++;
        }
    break;
    case 1:
        ew->bwait_time = 0;
        ew->bpos_no = 0;
    break;
    case 2:
        ew->bwait_time = 1;
    break;
    }
}


/* ingame/enemy/move_ctl.h - BJob001 */

void BJob001(ENE_WRK *ew)
{
    u_short adj;

    adj = ew->bcomm_add.pu8[0] + (ew->bcomm_add.pu8[1] << 8); ew->bcomm_add.pu16++;

    ew->bpos_no = 0;
    ew->bwait_time = 0;

    ew->bcomm_add.wrk = ew->bcomm_add_top + adj;
}

void BJob009(ENE_WRK *ew)
{
    LIGHT_PACK *l;
    float diff;
    float r;
    float g;
    float b;

    l = &ew->mylight;

    diff = *ew->bcomm_add.pu8++;
    r = *ew->bcomm_add.pu8++;
    g = *ew->bcomm_add.pu8++;
    b = *ew->bcomm_add.pu8++;

    diff /= 10.0f;
    r /= 10.0f;
    g /= 10.0f;
    b /= 10.0f;

    if (l->parallel_num < 3)
    {
        l->parallel[l->parallel_num].direction[0] = 0.0f;
        l->parallel[l->parallel_num].direction[1] = 0.0f;
        l->parallel[l->parallel_num].direction[2] = 0.0f;
        l->parallel[l->parallel_num].direction[3] = 0.0f;

        l->parallel[l->parallel_num].diffuse[0] = diff;
        l->parallel[l->parallel_num].diffuse[1] = diff;
        l->parallel[l->parallel_num].diffuse[2] = diff;
        l->parallel[l->parallel_num].diffuse[3] = 255.0f;

        l->ambient[0] = r;
        l->ambient[1] = g;
        l->ambient[2] = b;
        l->ambient[3] = 0.0f;

        l->parallel_num++;

        l->parallel[l->parallel_num].direction[0] = 0.0f;
        l->parallel[l->parallel_num].direction[1] = 0.0f;
        l->parallel[l->parallel_num].direction[2] = 0.0f;
        l->parallel[l->parallel_num].direction[3] = 0.0f;

        l->parallel[l->parallel_num].diffuse[0] = diff;
        l->parallel[l->parallel_num].diffuse[1] = diff;
        l->parallel[l->parallel_num].diffuse[2] = diff;
        l->parallel[l->parallel_num].diffuse[3] = 255.0f;
    }

    ew->bwait_time = 0;
    ew->bpos_no = 0;
}

void BJob00A(ENE_WRK *ew)
{
    sceVu0FVECTOR tv = { 0.0f, 0.0f, 0.0f, 0.0f };
    POINT_WRK *p;
    float power;
    float diff;
    short int tv2[3];
    u_char id;
    u_char i;

    power = ew->bcomm_add.pu8[0] | (ew->bcomm_add.pu8[1] << 8); ew->bcomm_add.pu16++;
    diff = ew->bcomm_add.pu8[0] | (ew->bcomm_add.pu8[1] << 8); ew->bcomm_add.pu16++;

    tv2[0] = ew->bcomm_add.pu8[0] + (ew->bcomm_add.pu8[1] << 8); ew->bcomm_add.pu16++;
    tv2[1] = ew->bcomm_add.pu8[0] + (ew->bcomm_add.pu8[1] << 8); ew->bcomm_add.pu16++;
    tv2[2] = ew->bcomm_add.pu8[0] + (ew->bcomm_add.pu8[1] << 8); ew->bcomm_add.pu16++;

    tv[0] = tv2[0];
    tv[1] = tv2[1];
    tv[2] = tv2[2];
    tv[3] = 0.0f;

    diff /= 100.0f;

    ew->pp[0] = tv[0];
    ew->pp[1] = tv[1];
    ew->pp[2] = tv[2];
    ew->pp[3] = tv[3];

    RotFvector(ew->move_box.rot, tv);

    sceVu0AddVector(tv, tv, ew->mpos.p0);

    for (i = 0; i < 2; i++)
    {
        id = ew->plight_svm[i];

        if (id == 0xff)
        {
            if (room_wrk.mylight[i].point_num < 3)
            {
                id = room_wrk.mylight[i].point_num;

                ew->plight_svm[i] = room_wrk.mylight[i].point_num;

                room_wrk.mylight[i].point_num++;
            }
        }

        if (id != 0xff)
        {
            p = &room_wrk.mylight[i].point[id];

            p->diffuse[0] = diff;
            p->diffuse[1] = diff;
            p->diffuse[2] = diff;
            p->diffuse[3] = 255.0f;

            p->pos[0] = tv[0];
            p->pos[1] = tv[1];
            p->pos[2] = tv[2];
            p->pos[3] = tv[3];

            p->power = power;
        }
    }

    id = ew->plight_svo;

    if (id == 0xff)
    {
        if (ew->mylight.point_num < 3)
        {
            id = ew->mylight.point_num;

            ew->plight_svo = id;

            ew->mylight.point_num++;
        }
    }

    if (id != 0xff)
    {
        p = &ew->mylight.point[id];

        p->diffuse[0] = diff;
        p->diffuse[1] = diff;
        p->diffuse[2] = diff;
        p->diffuse[3] = 255.0f;

        p->pos[0] = tv[0];
        p->pos[1] = tv[1];
        p->pos[2] = tv[2];
        p->pos[3] = tv[3];

        p->power = power;
    }

    ew->bwait_time = 0;
    ew->bpos_no = 0;
}

void BJob00B(ENE_WRK *ew)
{
    sceVu0FVECTOR p = { 0.0f, 0.0f, 0.0f, 0.0f };
    sceVu0FVECTOR i = { 0.0f, 0.0f, 0.0f, 0.0f };
    SPOT_WRK *s;
    float power;
    float diff;
    float intens;
    short int p2[3];
    short int i2[3];
    u_char id;

    power = ew->bcomm_add.pu8[0] | (ew->bcomm_add.pu8[1] << 8); ew->bcomm_add.pu16++;

    intens = ew->bcomm_add.pu8[0] | (ew->bcomm_add.pu8[1] << 8); ew->bcomm_add.pu16++;
    diff = ew->bcomm_add.pu8[0] | (ew->bcomm_add.pu8[1] << 8); ew->bcomm_add.pu16++;

    p2[0] = ew->bcomm_add.pu8[0] + (ew->bcomm_add.pu8[1] << 8); ew->bcomm_add.pu16++;
    p2[1] = ew->bcomm_add.pu8[0] + (ew->bcomm_add.pu8[1] << 8); ew->bcomm_add.pu16++;
    p2[2] = ew->bcomm_add.pu8[0] + (ew->bcomm_add.pu8[1] << 8); ew->bcomm_add.pu16++;

    i2[0] = ew->bcomm_add.pu8[0] + (ew->bcomm_add.pu8[1] << 8); ew->bcomm_add.pu16++;
    i2[1] = ew->bcomm_add.pu8[0] + (ew->bcomm_add.pu8[1] << 8); ew->bcomm_add.pu16++;
    i2[2] = ew->bcomm_add.pu8[0] + (ew->bcomm_add.pu8[1] << 8); ew->bcomm_add.pu16++;

    p[0] = p2[0];
    p[1] = p2[1];
    p[2] = p2[2];
    p[3] = 0.0f;

    i[0] = i2[0];
    i[1] = i2[1];
    i[2] = i2[2];
    i[3] = 0.0f;

    diff /= 100.0f;
    intens /= 100.0f;

    ew->sp[0] = p[0];
    ew->sp[1] = p[1];
    ew->sp[2] = p[2];
    ew->sp[3] = p[3];

    ew->si[0] = i[0];
    ew->si[1] = i[1];
    ew->si[2] = i[2];
    ew->si[3] = i[3];

    RotFvector(ew->move_box.rot, p);

    sceVu0AddVector(p, p, ew->mpos.p0);

    RotFvector(ew->move_box.rot, i);

    sceVu0AddVector(i, i, ew->mpos.p0);

    sceVu0SubVector(i, p, i);

    id = ew->slight_svo;

    if (id == 0xff)
    {
        if (ew->mylight.spot_num < 3)
        {
            id = (u_char)(ew->mylight).spot_num;
            ew->slight_svo = id;
            ew->mylight.spot_num++;
        }
    }

    if (id != 0xff)
    {
        s = &ew->mylight.spot[id];

        s->diffuse[0] = diff;
        s->diffuse[1] = diff;
        s->diffuse[2] = diff;
        s->diffuse[3] = 255.0f;

        s->pos[0] = p[0];
        s->pos[1] = p[1];
        s->pos[2] = p[2];
        s->pos[3] = p[3];

        s->direction[0] = i[0];
        s->direction[1] = i[1];
        s->direction[2] = i[2];
        s->direction[3] = i[3];

        s->power = power;
        s->intens = intens;
    }

    ew->bwait_time = 0;
    ew->bpos_no = 0;
}

void BJob00C(ENE_WRK *ew)
{
    u_char type;
    u_char sclx;
    u_char scly;

    type = *ew->bcomm_add.pu8++;
    sclx = *ew->bcomm_add.pu8++;
    scly = *ew->bcomm_add.pu8++;

    ew->d_pd = *ew->bcomm_add.pu8++;

    if (type != 0xff)
    {
        ew->pdf = CallPartsDeform5(type, sclx / 100.0f, scly / 100.0f, ew->bep, &ew->d_pd);
    }
    else
    {
        ResetEffects(ew->pdf);
    }

    ew->bwait_time = 0;
    ew->bpos_no = 0;
}

void BJob010(ENE_WRK *ew)
{
    u_short adj;

    adj = ew->bcomm_add.pu8[0] | (ew->bcomm_add.pu8[1] << 8); ew->bcomm_add.pu16++;

    if (plyr_wrk.mode == 2 || ew->sta & 0x2000000)
    {
        (ew->bcomm_add).wrk = adj + ew->bcomm_add_top;
    }

    ew->bwait_time = 0x0;
    ew->bpos_no = 0x0;
}

void BJob011(ENE_WRK *ew)
{
    u_short adj;

    adj = ew->bcomm_add.pu8[0] | (ew->bcomm_add.pu8[1] << 8); ew->bcomm_add.pu16++;

    if (ew->sta & 0x4000)
    {
        ene_wrk[ew->move_box.idx].sta &= ~0x4000;

        ew->bcomm_add.wrk = adj + ew->bcomm_add_top;
    }

    ew->bwait_time = 0;
    ew->bpos_no = 0;
}

void BJob012(ENE_WRK *ew)
{
    u_char n;
    static float rate[4];
    static float r_adj[4];

    n = ew->move_box.idx;

    switch(ew->bpos_no)
    {
    case 0:
        if (ew->tr_time == 0)
        {
            ew->tr_rate = ew->tr_max;
            ew->bwait_time = 0;
            ew->bpos_no = 0;

            return;
        }

        ew->tr_rate = 0;
        rate[n] = 0.0f;

        ew->tr_time /= 2;
        ew->bloop = ew->tr_time;

        r_adj[n] = (float)ew->tr_max / ew->bloop;

        ew->bpos_no++;
    case 1:
        if (plyr_wrk.mode == 2 || ew->sta & 0x2000000 || ew->sta & 0x4000)
        {
            ew->bwait_time = 0;
            ew->bpos_no = 0;

            return;
        }

        if (ew->bloop == 0)
        {
            ew->bwait_time = 1;
            ew->bpos_no++;

            return;
        }

        if ((ew->sta & 0x80000) == 0)
        {
            ew->bloop--;

            rate[n] = rate[n] + r_adj[n];

            if (rate[n] > 127.0f)
            {
                rate[n] = 127.0f;
            }

            if ((ew->dat->attr1 & 0x10 || (ingame_wrk.difficult != 0x0 && ingame_wrk.game == 0)) && plyr_wrk.mode != 1)
            {
                ew->tr_rate = 0;
            }
            else
            {
                ew->tr_rate = rate[n];
            }
        }

        ew->bwait_time = 1;
    break;
    case 2:
            ew->bloop = ew->tr_time;

            r_adj[n] = -((float)ew->tr_max / ew->tr_time);

            ew->bpos_no++;
    case 3:
        if (plyr_wrk.mode == 2 || ew->sta & 0x2000000 || ew->sta & 0x4000)
        {
            ew->bwait_time = 0;
            ew->bpos_no = 0;

            return;
        }

        if (ew->bloop == 0 || ew->tr_rate == 0)
        {
            ew->tr_rate = 0;
            ew->bwait_time = 0;
            ew->bpos_no = 0;

            return;
        }

        if ((ew->sta & 0x80000) == 0)
        {
            ew->bloop--;

            rate[n] = __builtin_fabsf(r_adj[n]) < rate[n] ? rate[n] + r_adj[n] : 0.0f;

            if (ew->dat->attr1 & 0x10 && plyr_wrk.mode != 1)
            {
                ew->tr_rate = 0;
            }
            else
            {
                ew->tr_rate = rate[n];
            }
        }

        ew->bwait_time = 1;
    break;
    }
}

void BJob013(ENE_WRK *ew)
{
    ew->tr_rate = *ew->bcomm_add.pu8++;

    ew->bwait_time = 0;
    ew->bpos_no = 0;
}

void BJob014(ENE_WRK *ew)
{
    u_char type;
    u_char sclx;
    u_char scly;

    type = *ew->bcomm_add.pu8++;
    sclx = *ew->bcomm_add.pu8++;
    scly = *ew->bcomm_add.pu8++;

    ew->d_pd2 = *ew->bcomm_add.pu8++;

    if (type != 0xff)
    {
        ew->pdf2 = CallPartsDeform5(type, sclx / 100.0f, scly / 100.0f, ew->bep, &ew->d_pd2);
    }
    else
    {
        ResetEffects(ew->pdf2);
    }

    ew->bwait_time = 0;
    ew->bpos_no = 0;
}
