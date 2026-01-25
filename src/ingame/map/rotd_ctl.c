#include "common.h"
#include "typedefs.h"
#include "rotd_ctl.h"

#include "graphics/graph3d/gra3d.h"
#include "graphics/graph3d/sglib.h"
#include "ingame/ig_glob.h"
#include "ingame/map/door_ctl.h"
#include "ingame/map/doortype.h"
#include "ingame/map/map_ctrl.h"
#include "ingame/map/rotd_ctl.h"
// #include "ingame/plyr/unit_ctl.h" // RotLimitChk
#include "main/glob.h"

#define PI 3.1415927f

int CheckRotDoorOpenStatus(int dwrk_no, u_char type)
{
    DOOR_TYPE_DAT *dtdp;
    DOOR_STTS_KEEP *dkeepp;

    dtdp = GetDoorTypeDatP(type);

    dkeepp = &door_keep[door_wrk[dwrk_no].door_id];

    if (dkeepp->room_id != 0xff)
    {
        if (dkeepp->room_id == plyr_wrk.pr_info.room_no)
        {
            if (dtdp->attribute & 0x2)
            {
                if (dkeepp->attr & 0x2000)
                {
                    return 0;
                }
                else
                {
                    return 1;
                }
            }
        }

        return 0;
    }

    return 0xffffffff;
}

void ChangeRotDoorFace(u_short door_id)
{
    DOOR_STTS_KEEP *dkeepp;
    char room_no; // not u_char!!
    u_char next_room_id;

    dkeepp = &door_keep[door_id];

    room_no = GetDataRoom(0, dkeepp->room_id);

    next_room_id = GetNextRIdFromRNoDId(room_no, door_id);

    if (next_room_id == 0xff)
    {
        return;
    }

    ChangeRotDoorFace2Room(door_id, next_room_id);
}

void ChangeRotDoorFaceRoomId(u_short door_id, u_char room_id)
{
    DOOR_STTS_KEEP *dkeepp;
    DOOR_WRK *dwp;
    int i;
    char room_no;
    u_char next_room_id;

    if (room_id == 0xff || door_id >= 300)
    {
        return;
    }

    dkeepp = &door_keep[door_id];

    if (dkeepp->room_id == room_id)
    {
        return;
    }

    room_no = GetDataRoom(0x0, dkeepp->room_id);

    next_room_id = GetNextRIdFromRNoDId(room_no, door_id);

    if (next_room_id != room_id)
    {
        return;
    }

    dkeepp->room_id = next_room_id;

    dwp = door_wrk;

    for (i = 0; i < 20; i++)
    {
        if (dwp->door_id == door_id && dwp->fwrk_no < 60)
        {
            dwp->rot += PI;

            RotLimitChk(&dwp->rot);

            furn_wrk[dwp->fwrk_no].rot += PI;

            RotLimitChk(&furn_wrk[dwp->fwrk_no].rot);

            furn_wrk[dwp->fwrk_no].rotate[1] = furn_wrk[dwp->fwrk_no].rot;

            if (furn_wrk[dwp->fwrk_no].furn_no != 0xffff && furn_wrk[dwp->fwrk_no].furn_no != 0)
            {
                ChoudoPreRenderDual(&furn_wrk[dwp->fwrk_no]);
            }
        }

        dwp++;
    }
}

void ChangeRotDoorFace2Room(u_short door_id, u_char room_id)
{
    DOOR_WRK *dwp;
    DOOR_STTS_KEEP *dkeepp;
    short dw_no;

    dw_no = SearchDoorWrk(door_id);

    if (dw_no == -1)
    {
        return;
    }

    dwp = &door_wrk[dw_no];

    dwp->rot += PI;

    RotLimitChk(&dwp->rot);

    dkeepp = &door_keep[door_id];

    dkeepp->room_id = room_id;
}

int JudgePlyrRotForRotDoor(int dwrk_no)
{
    DOOR_WRK *dwp;
    sceVu0FVECTOR dedge_left;
    sceVu0FVECTOR dedge_right;
    float rd_left;
    float rd_right;
    float p_rot;

    dwp = &door_wrk[dwrk_no];

    dedge_left[0] = dwp->pos[0] - SgCosf(dwp->rot) * 450.0f * 0.5f;
    dedge_left[1] = dwp->pos[1];
    dedge_left[2] = dwp->pos[2] + (SgSinf(dwp->rot) * 450.0f * 0.5f);

    dedge_right[0] = dwp->pos[0] + SgCosf(dwp->rot) * 450.0f * 0.5f;
    dedge_right[1] = dwp->pos[1];
    dedge_right[2] = dwp->pos[2] - (SgSinf(dwp->rot) * 450.0f * 0.5f);

    rd_left = SgAtan2f(dedge_left[0] - plyr_wrk.move_box.pos[0], dedge_left[2] - plyr_wrk.move_box.pos[2]);
    rd_right = SgAtan2f(dedge_right[0] - plyr_wrk.move_box.pos[0], dedge_right[2] - plyr_wrk.move_box.pos[2]);

    p_rot = plyr_wrk.move_box.rot[1];

    RotLimitChk(&p_rot);

    if (rd_left < rd_right)
    {
        if (p_rot <= rd_right && p_rot >= rd_left)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if (rd_left <= p_rot || p_rot <= rd_right)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

u_char ChkRotDoorRot(int dwrk_no)
{
    DOOR_WRK *dwp;
    sceVu0FVECTOR pvec;
    float drot;
    float prot;
    float dl_limit;
    float dr_limit;

    dwp = &door_wrk[dwrk_no];

    drot = dwp->rot;

    dl_limit = drot - (PI / 2);

    RotLimitChk(&dl_limit);

    dr_limit = drot + (PI / 2);

    RotLimitChk(&dr_limit);

    prot = SgAtan2f(plyr_wrk.move_box.pos[0] - dwp->pos[0], plyr_wrk.move_box.pos[2] - dwp->pos[2]);

    RotLimitChk(&prot);

    if (dr_limit > dl_limit)
    {
        if (prot > dl_limit && prot <= dl_limit)
        {
            return 0;
        }
    }
    else
    {
        if (dl_limit < prot || dr_limit > prot)
        {
            return 0;
        }
    }

    return 1;
}
