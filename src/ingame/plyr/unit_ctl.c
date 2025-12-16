#include "common.h"
#include "typedefs.h"
#include "unit_ctl.h"

// #include <cstdlib.h>
// RAND_MAX = (2**31-1)
#define RAND_MAX 2147483647

#include "sce/libvu0.h"

#include "common/ul_math.h"
#include "main/glob.h"
#include "ingame/ig_glob.h"
#include "ingame/map/map_ctrl.h"
#include "ingame/map/map_htck.h"
#include "ingame/plyr/unit_ctl.h"
#include "graphics/graph3d/sglib.h"

#define PI 3.1415927f

void RotLimitChk(sceVu0FVECTOR rot)
{
    if (PI < rot[0])
    {
        rot[0] -= PI * 2;
    }
    else if (rot[0] < -PI)
    {
        rot[0] += PI * 2;
    }
}

float GetTrgtRotY(sceVu0FVECTOR p0, sceVu0FVECTOR p1)
{
    return SgAtan2f(p1[0] - p0[0], p1[2] - p0[2]);
}

void GetTrgtRotFromPlyr(sceVu0FVECTOR p, sceVu0FVECTOR rot, int id)
{
    sceVu0FVECTOR dist;

    rot[0] = 0.0f;
    rot[1] = 0.0f;
    rot[2] = 0.0f;
    rot[3] = 0.0f;

    sceVu0SubVector(dist, p, plyr_wrk.move_box.pos);

    dist[1] = -(dist[1] - -600.0f);

    if (id & 0x1)
    {
        dist[3] = GetDistV(p,plyr_wrk.move_box.pos);

        rot[0] = SgAtan2f(dist[1], dist[3]);
    }

    if (id & 0x2)
    {
        rot[1] = SgAtan2f(dist[0],dist[2]);
    }
}

void GetTrgtRot(sceVu0FVECTOR p0, sceVu0FVECTOR p1, sceVu0FVECTOR rot, int id)
{
    sceVu0FVECTOR dist;

    rot[0] = 0.0f;
    rot[1] = 0.0f;
    rot[2] = 0.0f;
    rot[3] = 0.0f;

    sceVu0SubVector(dist, p1, p0);

    if (id & 0x1)
    {
        dist[3] = GetDistV(p0, p1);

        rot[0] = SgAtan2f(-dist[1], dist[3]);

        RotLimitChk(rot);
    }

    if (id & 0x2)
    {
        rot[1] = SgAtan2f(dist[0], dist[2]);

        RotLimitChk(&rot[1]);
    }
}

int GetRndSP(u_int min, u_int lng)
{
    long int result;

    if (lng != 0)
    {
        result = (int)(min + (long)((lng * (long)rand()) / (float)RAND_MAX));
    }
    else
    {
        result = min;
    }

    return result;
}

u_char ConvertRot2Dir(float rot, u_char id)
{
    u_char dir;

    dir = 0;

    switch (id)
    {
    case 0:
        dir = (int)((rot + PI + (PI / 4)) / (PI / 2)) % 4;
        dir += 2;

        if (dir >= 4)
        {
            dir -= 4;
        }
    break;
    case 1:
        dir = (int)((rot + PI + (PI / 8)) / (PI / 4)) % 8;
        dir += 4;

        if (dir > 7)
        {
            dir -= 8;
        }
    break;
    case 2:
        dir = (int)((rot + PI + (PI / 2)) / PI) % 2;
        dir += 1;

        if (dir >= 2)
        {
            dir -= 2;
        }
    break;
    default:
        if (rot < 0.0f)
        {
            dir = 1;
        }
    break;
    }

    return dir;
}

u_char HitChkSegment2AllChk(sceVu0FVECTOR f, sceVu0FVECTOR t, float deg)
{

    if (room_wrk.disp_no[0] != GetPointRoom(f[0], f[2]) || room_wrk.disp_no[0] != GetPointRoom(t[0], t[2]))
    {
        return 1;
    }

    return HitChkSegment2All(f, t, deg);
}

u_char OutSightChk(sceVu0FVECTOR tp, sceVu0FVECTOR vp, float rot, float sight, float dist)
{
    u_char chk;

    chk = 1;

    if (dist == 0.0f || dist >= GetDistV(tp, vp))
    {
        if (RotRngChk(vp,tp,rot,sight * 0.5f) != 0)
        {
            chk = 0;
        }
    }

    return chk;
}

int RotRngChk(sceVu0FVECTOR vp, sceVu0FVECTOR tp, float rot, float rng)
{
    rot = GetTrgtRotY(vp, tp) - rot;

    RotLimitChk(&rot);

    rot = __builtin_fabsf(rot);

    return rot <= rng;
}

int GetPointHeightChk(sceVu0FVECTOR tv)
{
    int result;

    result = 0;

    if (room_wrk.disp_no[0] == GetPointRoom(tv[0], tv[2]))
    {
        tv[1] = GetPointHeight(tv[0], tv[2]);

        result = 1;
    }

    return result;
}

void ReqEneStop(u_char req, u_char except)
{
    ENE_WRK *ew;
    FLY_WRK *fw;
    u_char i;

    if (req != 0)
    {
        ew = ene_wrk;

        for (i = 0; i < 4; i++)
        {
            if (ew->sta & 0x1 && (((except >> i) & 0x1) == 0))
            {
                ene_wrk[i].sta |= 0x8000000;

                if (req == 2)
                {
                    ene_wrk[i].sta |= 0x200000;
                }
            }

            ew++;
        }

        fw = fly_wrk;

        for (i = 0; i < 10; i++)
        {
            if (fw->sta & 0x1)
            {
                fw->sta |= 0x4;
            }

            fw++;
        }
    }
    else
    {
        ew = ene_wrk;

        for (i = 0; i < 4; i++)
        {
            if (ew->sta & 0x8000000)
            {
                ene_wrk[i].sta &= ~(0x8000000 | 0x200000);
            }

            ew++;
        }

        fw = fly_wrk;

        for (i = 0; i < 10; i++)
        {
            if (fw->sta & 0x4)
            {
                fw->sta &= ~0x4;
            }

            fw++;
        }
    }
}
