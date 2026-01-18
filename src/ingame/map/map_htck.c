#include "common.h"
#include "typedefs.h"
#include "map_htck.h"

#include "ingame/map/door_ctl.h" // DoorCoverCheck
#include "ingame/map/furn_ctl.h"
#include "ingame/map/map_ctrl.h"
#include "main/glob.h"

#define GET(t,o) (*((t *)(((u_char *)addr) + o)))

#define BASE_ADDRESS 0x7f8000

u_char PosInAreaJudgeSub(int *addr, u_short pos_x, u_short pos_y, u_char type)
{

    if (type == 2)
    {
        if (
            (GET(short,  0) * pos_x) + (GET(short,  2) * pos_y) + GET(int,  4) <= 0 &&
            (GET(short,  8) * pos_x) + (GET(short, 10) * pos_y) + GET(int, 12) <= 0 &&
            (GET(short, 16) * pos_x) + (GET(short, 18) * pos_y) + GET(int, 20) >= 0 &&
            (GET(short, 24) * pos_x) + (GET(short, 26) * pos_y) + GET(int, 28) >= 0
        )
        {
            return 1;
        }
    }
    else if (type == 1)
    {
        if (
            (GET(short,  0) * pos_x) + (GET(short,  2) * pos_y) + GET(int,  4) <= 0 &&
            (GET(short,  8) * pos_x) + (GET(short, 10) * pos_y) + GET(int, 12) >= 0 &&
            (GET(short, 16) * pos_x) + (GET(short, 18) * pos_y) + GET(int, 20) >= 0 &&
            (GET(short, 24) * pos_x) + (GET(short, 26) * pos_y) + GET(int, 28) >= 0
        )
        {
            return 1;
        }
    }
    else if (type == 3)
    {
        if (
            (GET(short,  0) * pos_x) + (GET(short,  2) * pos_y) + GET(int,  4) <= 0 &&
            (GET(short,  8) * pos_x) + (GET(short, 10) * pos_y) + GET(int, 12) <= 0 &&
            (GET(short, 16) * pos_x) + (GET(short, 18) * pos_y) + GET(int, 20) <= 0 &&
            (GET(short, 24) * pos_x) + (GET(short, 26) * pos_y) + GET(int, 28) >= 0
        )
        {
            return 1;
        }
    }

    return 0;
}

void Get2DLinearConstFromPos(float *a, float *b, float *c, float x0, float y0, float x1, float y1)
{
    *a = y0 - y1;
    *b = x1 - x0;
    *c = (x0 * y1) - (x1 * y0);
}

int Get2DLinearCrossPoint(float *cpx, float *cpy, float a0, float b0, float c0, float a1, float b1, float c1)
{
    float mother;

    mother = (a0 * b1) - (b0 * a1);

    if (mother != 0.0f)
    {
        *cpx = ((b0 * c1) - (c0 * b1)) / mother;
        *cpy = ((c0 * a1) - (a0 * c1)) / mother;

        return 1;
    }

    return 0;
}

int ChkLineCross(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3)
{
    float a0;
    float b0;
    float c0;
    float a1;
    float b1;
    float c1;
    float d;

    Get2DLinearConstFromPos(&a0, &b0, &c0, x0, y0, x1, y1);

    d = (a0 * x2 + b0 * y2 + c0) * (a0 * x3 + b0 * y3 + c0);

    if (d > 0.0f)
    {
        return 0;
    }

    Get2DLinearConstFromPos(&a1, &b1, &c1, x2, y2, x3, y3);

    d = (a1 * x0 + b1 * y0 + c1) * (a1 * x1 + b1 * y1 + c1);

    if (d > 0.0f)
    {
        return 0;
    }

    return 1;
}

int ChkInsideBox(float cpx, float cpy, float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3)
{
    float a0;
    float b0;
    float c0;
    float a1;
    float b1;
    float c1;
    float d;

    Get2DLinearConstFromPos(&a0, &b0, &c0, x0, y0, x1, y1);
    Get2DLinearConstFromPos(&a1, &b1, &c1, x2, y2, x3, y3);

    d = (a0 * cpx + b0 * cpy + c0) * (a1 * cpx + b1 * cpy + c1);

    if (d > 0.0f)
    {
        return 0;
    }

    Get2DLinearConstFromPos(&a0, &b0, &c0, x0, y0, x2, y2);
    Get2DLinearConstFromPos(&a1, &b1, &c1, x1, y1, x3, y3);

    d = (a0 * cpx + b0 * cpy + c0) * (a1 * cpx + b1 * cpy + c1);

    if (d > 0.0f)
    {
        return 0;
    }

    return 1;
}

u_char HitChkSegment2All(sceVu0FVECTOR f, sceVu0FVECTOR t, float deg)
{
    sceVu0FVECTOR seg;
    sceVu0FVECTOR chk;
    float ft;
    int i;
    int j;
    u_short cmp_x;
    u_short cmp_y;
    u_short cmp_z;
    u_char room_id;
    u_char room_no;
    u_char tmp_hit_num;

    if (deg == 0.0f)
    {
        return 0xff;
    }

    seg[0] = t[0] - f[0];
    seg[1] = t[1] - f[1];
    seg[2] = t[2] - f[2];

    for (ft = 0.0f; ft  < deg; ft += 1.0f)
    {
        chk[0] = (f[0] + (seg[0] * (ft + 1.0f)) / deg);
        chk[1] = (f[1] + (seg[1] * (ft + 1.0f)) / deg);
        chk[2] = (f[2] + (seg[2] * (ft + 1.0f)) / deg);

        cmp_x = chk[0];
        cmp_y = chk[1];
        cmp_z = chk[2];

        room_id = map_wrk.now_room;
        tmp_hit_num = room_wrk.hit_num;

        for (i = 0; i < 2; i++)
        {
            if (room_id == 0xff)
            {
                room_id = map_wrk.next_room;
                tmp_hit_num = room_wrk.hit_num_next;
            }
            else
            {
                room_no = GetDataRoom(6, room_id);

                for (j = 0; j < tmp_hit_num; j++)
                {
                    if (PosInAreaJudge1(6, room_no, j, cmp_z, cmp_x) != 0)
                    {
                        return 1;
                    }
                }

                if (DoorCoverCheck(cmp_x, cmp_y, cmp_z, room_id) != 0)
                {
                    return 2;
                }

                room_no = GetDataRoom(11, room_id);

                if (FurnCoverCheck(cmp_x, cmp_y, cmp_z, room_no) != 0)
                {
                    return 3;
                }

                room_id = map_wrk.next_room;
                tmp_hit_num = room_wrk.hit_num_next;
            }
        }
    }

    return 0;
}

u_char HitChkSegment2All2D(sceVu0FVECTOR f, sceVu0FVECTOR t, float deg)
{
    sceVu0FVECTOR seg;
    sceVu0FVECTOR chk;
    float ft;
    int i;
    int j;
    u_short cmp_x;
    u_short cmp_y;
    u_short cmp_z;
    u_char room_id;
    u_char room_no;

    if (deg == 0.0f)
    {
        return 0xff;
    }

    seg[0] = t[0] - f[0];
    seg[1] = 0.0f;
    seg[2] = t[2] - f[2];

    for (ft = 0.0f; ft  < deg; ft += 1.0)
    {
        chk[0] = (f[0] + (seg[0] * (ft + 1.0)) / deg);
        chk[1] = (f[1] + (seg[1] * (ft + 1.0)) / deg);
        chk[2] = (f[2] + (seg[2] * (ft + 1.0)) / deg);

        cmp_x = chk[0];
        cmp_y = chk[1];
        cmp_z = chk[2];

        room_id = map_wrk.now_room;
        for (i = 0; i < 2; i++)
        {
            if (room_id != 0xff)
            {
                room_no = GetDataRoom(6, room_id);

                for (j = 0; j < room_wrk.hit_num; j++)
                {
                    if (PosInAreaJudge1(6, room_no, j, cmp_z, cmp_x) != 0)
                    {
                        return 1;
                    }
                }

                room_no = GetDataRoom(11, room_id);

                if (FurnCoverCheck(cmp_x, cmp_y, cmp_z, room_no) != 0)
                {
                    return 3;
                }

                if (DoorCoverCheck(cmp_x, cmp_y, cmp_z, room_id) != 0)
                {
                    return 2;
                }

                room_id = map_wrk.next_room;
            }
        }
    }

    return 0;
}

u_char FurnCoverCheck(u_short pos_x, short int pos_y, u_short pos_z, u_char room_no)
{
    u_int *addr;
    u_int *addr_bak;
    FURN_DATA_POP *fedp;
    int i;
    u_char dt_num;
    u_char type;

    if (room_no == 0xff)
    {
        return 0;
    }

    addr = (u_int *)(map_wrk.dat_adr + 11 * 4);
    addr = (u_int *)(*addr + BASE_ADDRESS);
    addr = &addr[room_no] + 1;
    addr_bak = addr = (u_int *)(*addr + BASE_ADDRESS);

    dt_num = *(u_char *)(*addr + BASE_ADDRESS);

    for (i = 0; i < dt_num; i++)
    {
        addr = &addr_bak[i] + 1;
        addr = (u_int *)(*addr + BASE_ADDRESS);

        fedp = (FURN_DATA_POP *)(*addr + BASE_ADDRESS);

        GetFurnAttr(fedp->id, ingame_wrk.msn_no);

        addr = (u_int *)(addr[1] + BASE_ADDRESS);
        type = ((u_char *)fedp)[25];
        if (PosInAreaJudgeSub((int *)addr, pos_z, pos_x, type) != 0)
        {
            if (-fedp->top <= pos_y && pos_y <= -fedp->btm)
            {
                return 1;
            }
        }
    }

    return 0;
}

