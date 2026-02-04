#include "common.h"
#include "typedefs.h"
#include "fsla_main.h"

// #include <cstdlib.h>
// RAND_MAX = (2**31-1)
#define RAND_MAX 2147483647

#include "sce/libvu0.h"

#include "common/ul_math.h"
#include "graphics/graph3d/sglib.h"
#include "ingame/map/furn_spe/fsla_dat.h"
#include "ingame/map/furn_spe/fsla_main.h"
#include "main/glob.h"

u_char able_to_light_tbl[] = {
    2, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 2, 0, 0, 0, 0, 3, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
POINT_LIGHT_WRK point_light_wrk[8] = {0};
FSPE_LIGHT_WRK fspe_light_wrk[7] = {0};

FSPE_LIGHT_ANM* SetRandLightAnimEach(FSPE_LIGHT_ANM **ap)
{
    int i;
    int num;
    FSPE_LIGHT_ANM **temp;

    num = 0;
    temp = ap;

    while (*(temp++) != NULL)
    {
        num++;
    }

    i = (rand() / (float)RAND_MAX) * num;

    return ap[i];
}

void SetRandLightAnim(FSPE_LIGHT_WRK *lw)
{
    int num;
    FSPE_LIGHT_ANM **temp;
    FSPE_LIGHT_ANM **save;

    num = 0;

    save = fspe_la_array[lw->type];
    temp = save;

    while (*(temp++) != NULL)
    {
        num++;
    }

    num = (rand() / (float)RAND_MAX) * num;

    lw->ap_in = (FSPE_LIGHT_ANM *)save[num];
}

signed char GetOpenPointLightWrkID()
{
    int i;

    for (i = 0; i < 7; i++)
    {
        if (fspe_light_wrk[i].state == ST_FSLA_VACANT)
        {
            return i;
        }
    }

    return -1;
}

signed char GetVacantPLW()
{
    int i;

    for (i = 0; i < 8; i++)
    {
        if (point_light_wrk[i].state == 0)
        {
            return i;
        }
    }

    return -1;
}

void DeletePointLight(signed char num)
{
    point_light_wrk[num].state = 0;
}

signed char AddNewPointLight(sceVu0FVECTOR *pos, sceVu0FVECTOR *diffuse, float *power, u_char room_id)
{
    signed char i;
    POINT_LIGHT_WRK *plw;

    i = GetVacantPLW();

    if (i < 0)
    {
        return -1;
    }

    plw = &point_light_wrk[i];

    plw->pos = pos;
    plw->diffuse = diffuse;
    plw->power = power;
    plw->state = 1;
    plw->room_no = room_id;

    return i;
}

float SetPLW2PW(POINT_WRK *pw, POINT_LIGHT_WRK *plw, float obj_dist, float compare)
{
    float power;
    float multi;
    sceVu0FVECTOR temp;

    multi = (3000.0f - obj_dist) / 3000.0f;
    multi = multi * *plw->power;

    sceVu0ScaleVector(temp, *plw->diffuse, multi);

    power = SgSqrtf(sceVu0InnerProduct(temp, temp));

    if (compare < power)
    {
        sceVu0CopyVector(pw->diffuse, *plw->diffuse);
        sceVu0CopyVector(pw->pos, *plw->pos);

        pw->power = multi;

        return power;
    }

    return compare;
}

int SetPointLightPack(float *obj_pos, POINT_WRK *ppw, u_char start_num)
{
    int i;
    int j;
    POINT_LIGHT_WRK *plw;
    float power[3];
    float temppri;
    float obj_dist;
    u_char save_num;
    u_char registered_num;
    u_char lm_num;

    lm_num = able_to_light_tbl[room_wrk.disp_no[0]];

    plw = point_light_wrk;

    if (lm_num < 2)
    {
        return start_num;
    }

    registered_num = start_num;

    for (i = 0; i < 8; i++, plw++)
    {
        if (plw->state == 0)
        {
            continue;
        }

        if (plw->room_no == room_wrk.disp_no[0] || (plw->room_no == 0xff))
        {
            obj_dist = GetDistV(*plw->pos, obj_pos);

            if (obj_dist > 3000.0f)
            {
                continue;
            }

            if (registered_num < lm_num)
            {
                power[registered_num] = SetPLW2PW(ppw + registered_num, plw, obj_dist, 0.0f);
                registered_num++;
            }
            else
            {
                temppri = power[start_num];
                save_num = start_num;

                for (j = start_num + 1; j < lm_num; j++)
                {
                    if (temppri < power[j])
                    {
                        temppri = power[j];
                        save_num = j;
                    }
                }

                power[save_num] = SetPLW2PW(ppw + save_num, plw, obj_dist, power[save_num]);
            }
        }
    }

    return registered_num;
}

void FinishCandle(FSPE_LIGHT_WRK *lw)
{
    lw->state = ST_FSLA_VACANT;

    DeletePointLight(lw->plw_id);
}

void CandleAnmStop(signed char lw_id)
{
    FSPE_LIGHT_WRK *lw;

    if (lw_id < 0)
    {
        return;
    }

    if (fspe_light_wrk[lw_id].fadeouttype != 0)
    {
        fspe_light_wrk[lw_id].state = ST_FSLA_OUT;
        fspe_light_wrk[lw_id].ap_in = *fspe_la_array[fspe_light_wrk[lw_id].fadeouttype];
    }
    else
    {
        FinishCandle(&fspe_light_wrk[lw_id]);
    }
}

signed char CandleAnmOcc(sceVu0FVECTOR *pos, float r, float g, float b, u_char in_pat, u_char pat, u_char out_pat, u_char room_id)
{
    signed char id;
    FSPE_LIGHT_WRK *lw;

    id = GetOpenPointLightWrkID();

    if (id < 0)
    {
        return id;
    }

    lw = &fspe_light_wrk[id];
    lw->fadeouttype = out_pat;
    lw->type = pat;

    lw->diffuse[0] = r;
    lw->diffuse[1] = g;
    lw->diffuse[2] = b;

    if (in_pat != 0)
    {
        lw->state = ST_FSLA_IN;
        lw->ap_in = *fspe_la_array[in_pat];
    }
    else
    {
        lw->state = ST_FSLA_EXEC;

        SetRandLightAnim(lw);
    }

    lw->time = lw->ap_in->time;
    lw->now_magnif = lw->ap_in->magnif;
    lw->room_id = room_id;
    lw->plw_id = AddNewPointLight(pos, &lw->diffuse, &lw->now_magnif, room_id);

    return id;
}

void PointLightCtrl()
{
    plyr_wrk.mylight.point_num = SetPointLightPack(plyr_wrk.move_box.pos, plyr_wrk.mylight.point, 1);
}

void CandleAnmCtrl()
{
    int j;
    FSPE_LIGHT_WRK *lw;

    for (j = 0; j < 7; j++)
    {
        lw = &fspe_light_wrk[j];

        if (lw->state == ST_FSLA_VACANT)
        {
            continue;
        }

        if (lw->time != 0)
        {
            lw->time--;
        }
        else
        {
            lw->ap_in++;

            switch (lw->state)
            {
            case ST_FSLA_IN:
            case ST_FSLA_EXEC:
                if (lw->ap_in->time == 0)
                {
                    SetRandLightAnim(lw);

                    lw->state = ST_FSLA_EXEC;
                }
            break;
            case ST_FSLA_OUT:
                if (lw->ap_in->time == 0)
                {
                    FinishCandle(lw);
                }
            break;
            }

            lw->time = lw->ap_in->time;
            lw->now_magnif = lw->ap_in->magnif * 100.0f;
        }
    }
}
