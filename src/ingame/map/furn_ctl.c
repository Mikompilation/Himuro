#include "common.h"
#include "typedefs.h"
#include "furn_ctl.h"

#include "graphics/graph3d/gra3d.h"
#include "graphics/motion/accessory.h"
#include "graphics/motion/mime.h"
#include "ingame/ig_glob.h"
#include "ingame/map/door_ctl.h"
#include "ingame/map/furn_ctl.h"
#include "ingame/map/furn_dat.h"
#include "ingame/map/furn_eff.h"
// #include "ingame/map/furn_spe/furn_spe.h" // (miss) SearchRegisterFW2FAW
#include "ingame/map/map_ctrl.h"
#include "ingame/map/map_htck.h"
// #include "ingame/plyr/unit_ctl.h" // (miss) RotLimitChk
#include "main/glob.h"

int furn_disp_flg = 0;

#define PI 3.1415927f

#define BASE_ADDRESS 0x7f8000

void FurnCtrlMain()
{
    FurnHintDeformCtrl();
    FurnAplyAmbient();
}

void InitFurnDispFlg()
{
    furn_disp_flg = 0;
}

FURN_WRK* FurnGetFurnWrk(u_char fw_no)
{
    if (fw_no < 60)
    {
        return &furn_wrk[fw_no];
    }

    return NULL;
}

u_char ChkFurnGetAlready(u_short furn_id)
{
    FURN_WRK *fwp;
    int i;

    fwp = furn_wrk;

    for (i = 0; i < 60; i++)
    {
        if (furn_id == fwp->id && (fwp->use == 0 || fwp->use == 2))
        {
            return 1;
        }

        fwp++;
    }

    return 0;
}

void FurnSetWrkNoUse(FURN_WRK *fwp, int no)
{
    if (fwp != NULL && fwp->use != 5)
    {
        FreeFActFwrkNo(no);
        FurnEfctFree(fwp);

        if (fwp->use == 0 || fwp->use == 2)
        {
            mimChodoReleaseWork(fwp->id,fwp->room_id);
            acsRopeReleaseWork(fwp->id);
            acsChodoReleaseWork(fwp->id);
        }

        fwp->use = 5;
        fwp->furn_no = 0xffff;
        fwp->fno_bk = 0xffff;
        fwp->attr_id = 0xffff;
        fwp->id = 0xffff;
    }
}

int FurnSetFurnWork(FURN_DATA_POP *fdpp, int fw_no, u_char msn_no, u_char use, u_char room_id)
{
    FURN_WRK *fwp;
    F_ACT_WRK fbuf;
    int fact_type;
    float rot;

    if (FurnIsUsable(fdpp->id,msn_no) == 0)
    {
        return 0;
    }

    if (ChkFurnGetAlready(fdpp->id) != 0)
    {
        return 0;
    }

    fwp = FurnGetFurnWrk(fw_no);

    if (fwp == NULL)
    {
        return 0;
    }
    else
    {
        fwp->id = fdpp->id;
        fwp->attr_id = fdpp->attr_id;

        if (FurnIsDisp(fdpp->id,msn_no) != 0)
        {
            fwp->furn_no = fdpp->model_no;
            fwp->fno_bk = 0xffff;
        }
        else
        {
            fwp->fno_bk = fdpp->model_no;
            fwp->furn_no = 0xffff;
        }

        *(int *)&rot = *(int *)&fdpp->rot_x;
        RotLimitChk(&rot);

        fwp->rotate[0] = rot;

        *(int *)&rot = *(int *)&fdpp->rot_y;
        RotLimitChk(&rot);

        rot += PI;

        RotLimitChk(&rot);

        fwp->rotate[1] = rot;
        fwp->rotate[2] = 0.0f;

        fwp->pos[0] = fdpp->pos_x;
        fwp->pos[1] = fdpp->pos_y;
        fwp->pos[2] = fdpp->pos_z;

        if (fwp->furn_no == 27)
        {
            fwp->pos[0] = fdpp->pos_x + 25.0f;
        }

        fwp->use = use;
        fwp->room_id = room_id;
        fwp->fs_flg = 1;
        fwp->fewrk_no = 0xff;

        FurnEfctSet(fwp);
        mimChodoSetWork(fwp->id,fwp->room_id);
        acsRopeSetWork(fwp->id);
        acsChodoSetWork(fwp->id);

        fact_type = furn_dat[fwp->id].fact_no;

        if (fact_type != 0xffff)
        {
            SearchRegisterFW2FAW(fw_no,fact_type);

            return 1;
        }
    }

    return 1;
}

int FurnIsPicDispOnly(u_short id, u_char msn_no)
{
    return FurnIsDisp(id, msn_no) == 0 && FurnIsPicDisp(id, msn_no) != 0;
}

void FurnAplyAmbient()
{
    FURN_WRK *fwp;
    int i;

    for (i = 0, fwp = furn_wrk; i < 60; i++, fwp++)
    {
        if (fwp->furn_no == 0xffff)
        {
            return;
        }

        fwp->mylight.ambient[0] = room_ambient_light[0];
        fwp->mylight.ambient[1] = room_ambient_light[1];
        fwp->mylight.ambient[2] = room_ambient_light[2];
        fwp->mylight.ambient[3] = room_ambient_light[3];
    }
}

int GetRoomFurnID(u_char room_id, u_short *furn_id, u_char msn_no)
{
    FURN_DATA_POP *fdpp;
    int *addr_si0;
    int *addr_si1;
    int *addr_si2;
    u_char *addr_uc0;
    u_char *addr_uc1;
    int ret;
    int i;
    int j;
    int k;
    u_char room_num;

    ret = 0;

    if (room_id >= 60)
    {
        return 0;
    }

    for (k = 0; k < 4; k++)
    {
        if (floor_exist[msn_no][k] == 0)
        {
            continue;
        }

        addr_si0 = (int *)GetFloorTopAddr(k);
        addr_si0 = (int *)(addr_si0[11] + BASE_ADDRESS);

        addr_uc0 = (u_char *)(*addr_si0 + BASE_ADDRESS);

        room_num = *addr_uc0;
        addr_uc0++;

        addr_si1 = &addr_si0[1];

        for (i = 0; i < room_num; i++)
        {
            if (*addr_uc0 == room_id)
            {
                addr_si1 = (int *)&addr_si1[i];
                addr_si1 = (int *)(*addr_si1 + BASE_ADDRESS);

                addr_uc1 = (u_char *)(*addr_si1 + BASE_ADDRESS);
                addr_si1++;

                for (j = 0 ; j < *addr_uc1; j++)
                {
                    addr_si2 = (int *)(addr_si1[j] + BASE_ADDRESS);

                    fdpp = (FURN_DATA_POP *)(*addr_si2 + BASE_ADDRESS);

                    if (room_id == 3)
                    {
                        *furn_id = fdpp->model_no;

                        furn_id++;

                        ret++;
                    }
                    else if (fdpp->model_no != 0)
                    {
                        *furn_id = fdpp->model_no;

                        furn_id++;

                        ret++;
                    }
                }

                return ret;
            }

            addr_uc0++;
        }
    }

    return 0;
}

int FurnIsWrkUse(FURN_WRK *fw)
{
    if (fw != NULL)
    {
        if (fw->use == 5)
        {
            return 0;
        }
    }

    return 1;
}

int FurnIsSeUse(u_short id, u_char msn_no)
{
    if (
        (furn_attr_dat[furn_dat[id].attr_no][msn_no] & 0x8000) == 0 &&
        furn_attr_dat[furn_dat[id].attr_no][msn_no] & 0x800
    )
    {
        return 1;
    }

    return 0;
}

int FurnIsUsable(u_short id, u_char msn_no)
{
    if (GetFurnAttr(id, msn_no) & 0x8000)
    {
        return 0;
    }

    return 1;
}

int FurnIsPicDisp(u_short id, u_char msn_no)
{
    if ((GetFurnAttr(id, msn_no) & (0x8000 | 0x40)) == 64)
    {
        return 1;
    }

    return 0;
}

int FurnIsDisp(u_short id, u_char msn_no)
{
    if (GetFurnAttr(id, msn_no) & 0x4000)
    {
        return 0;
    }

    return 1;
}

int FurnIsHit(u_short id, u_char msn_no)
{
    if (GetFurnAttr(id, msn_no) & (0x8000 | 0x2000))
    {
        return 0;
    }

    return 1;
}

void FurnPicExecStart()
{
    furn_disp_flg = 1;

    FurnPicNoDspOff();
    FurnPicDispOn();
    FurnSortFurnWrk(1);
}

void FurnPicExecEnd()
{
    furn_disp_flg = 0;

    FurnPicNoDspOn();
    FurnPicDispOff();
    FurnSortFurnWrk(1);
}

void FurnPicNoDspOff()
{
    FURN_WRK *fwp;
    int i;
    u_int attr;

    fwp = furn_wrk;

    for (i = 0; i < 60; i++)
    {
        if (fwp->furn_no == 0xffff)
        {
            break;
        }

        attr = GetFurnAttr(fwp->id, ingame_wrk.msn_no);

        if ((attr & 0x8) == 0)
        {
            if (attr & 0x10)
            {
                fwp->fno_bk = fwp->furn_no;
                fwp->furn_no = 0xffff;
            }
        }

        fwp++;
    }
}

void FurnPicNoDspOn()
{
    FURN_WRK *fwp;
    int i;
    u_int attr;

    fwp = furn_wrk;

    for (i = 0; i < 60; i++)
    {
        if (fwp->furn_no == 0xffff)
        {
            break;
        }

        fwp++;
    }

    while (i < 60)
    {
        if (fwp->furn_no == 0xffff)
        {
            attr = GetFurnAttr(fwp->id, ingame_wrk.msn_no);

            if ((attr & 8) == 0)
            {
                if (attr & 0x10)
                {
                    fwp->furn_no = fwp->fno_bk;
                    fwp->fs_flg = 1;
                    fwp->fno_bk = 0xffff;
                }
            }
        }

        fwp++;
        i++;
    }
}

void FurnSortFurnWrk(u_char mode)
{
    FURN_WRK *fwp0;
    FURN_WRK *fwp1;
    FURN_WRK fw_tmp;
    DOOR_WRK *dwp;
    int i;
    int j;
    int k;
    int flg;

    for (i = 0, fwp0 = furn_wrk; i < 60; i++, fwp0++)
    {
        if (fwp0->furn_no != 0xffff)
        {
            continue;
        }

        fwp1 = fwp0;

        flg = 0;

        for (j = i; j < 60; j++)
        {
            if (fwp1->furn_no != 0xffff)
            {
                fw_tmp = *fwp1;
                *fwp1 = *fwp0;
                *fwp0 = fw_tmp;

                for (k = 0, dwp = door_wrk; k < 20; k++, dwp++)
                {
                    if (dwp->fwrk_no == i)
                    {
                        dwp->fwrk_no = j;
                    }
                    else if (dwp->fwrk_no == j)
                    {
                        dwp->fwrk_no = i;
                    }
                }

                ExchangeFAWFWID(i, j);

                flg = 1;

                break;
            }

            fwp1++;
        }

        if (flg == 0)
        {
            fwp0 = furn_wrk;

            for (i = 0; i < 60; i++)
            {
                fwp0->fs_flg = 0;

                fwp0++;
            }

            return;
        }
    }

    fwp0 = furn_wrk;

    for (i = 0; i < 60; i++)
    {
        fwp0->fs_flg = 0;

        fwp0++;
    }
}

void FurnDataRenewNow()
{
    int i;

    for (i = 0; i < 60; i++)
    {
        if (furn_wrk[i].use == 2)
        {
            furn_wrk[i].use = 0;
        }
        else if (furn_wrk[i].use == 0)
        {
            furn_wrk[i].use = 2;
        }
    }
}

void FurnFreeFurnWrk(u_char rm_id)
{
    int i;

    if (rm_id == 0xff)
    {
        return;
    }

    for (i = 0; i < 60; i++)
    {
        if (furn_wrk[i].use == 2)
        {
            FurnSetWrkNoUse(&furn_wrk[i], i);
        }
    }
}

void FurnDataInit()
{
    FURN_DATA_POP *fdpp;
    int i;
    int *addr_si;
    int *addr_fdt;
    u_char *r_id_p;
    u_char rm_num;
    u_char dt_num;
    u_char no_use_fw;

    InitFActWrk();

    addr_si = (int *)(map_wrk.dat_adr + 11 * 4);
    addr_si = (int *)(*addr_si + BASE_ADDRESS);

    r_id_p = (u_char *)(*addr_si + BASE_ADDRESS);

    rm_num = *r_id_p;
    r_id_p++;

    for (i = 0; i < rm_num; i++, r_id_p++)
    {
        if (*r_id_p == map_wrk.now_room)
        {
            break;
        }
    }

    if (i >= rm_num)
    {
        return;
    }

    SetUpRoomCoordinate(*r_id_p, room_wrk.pos[0]);

    addr_si = (int *)(map_wrk.dat_adr + 11 * 4);
    addr_si = (int *)(*addr_si + BASE_ADDRESS);
    addr_si = (int *)(addr_si[i+1] + BASE_ADDRESS);

    dt_num = *(int *)(*addr_si + BASE_ADDRESS);

    no_use_fw = 0;

    for (i = 0; i < dt_num; i++)
    {
        addr_fdt = (int *)(addr_si[i+1] + BASE_ADDRESS);

        fdpp = (FURN_DATA_POP *)(*addr_fdt + BASE_ADDRESS);

        while(FurnIsWrkUse(&furn_wrk[no_use_fw]) != 0)
        {
            no_use_fw++;
        }

        FurnSetFurnWork(fdpp, no_use_fw, ingame_wrk.msn_no, 0, *r_id_p);

        if (furn_wrk[no_use_fw].furn_no != 0xffff && furn_wrk[no_use_fw].furn_no != 0)
        {
            ChoudoPreRender(&furn_wrk[no_use_fw]);

        }
        else if (
            furn_wrk[no_use_fw].furn_no == 0xffff && furn_wrk[no_use_fw].fno_bk != 0xffff &&
            furn_wrk[no_use_fw].fno_bk != 0
        )
        {
            furn_wrk[no_use_fw].furn_no = 0xffff;
        }
    }
}

void FurnDataRenewNext(u_char room_id)
{
    FURN_DATA_POP *fdpp;
    int *addr_si;
    int *addr_fdt;
    u_char *r_id_p;
    int i;
    u_char rm_num;
    u_char dt_num;
    u_char no_use_fw;

    if (room_id == 0xff)
    {
        return;
    }

    addr_si = (int *)(map_wrk.dat_adr + 11 * 4);
    addr_si = (int *)(*addr_si + BASE_ADDRESS);

    r_id_p = (u_char *)(*addr_si + BASE_ADDRESS);

    rm_num = *r_id_p;
    r_id_p++;

    for (i = 0; i < rm_num; i++, r_id_p++)
    {
        if (*r_id_p == room_id)
        {
            break;
        }
    }

    if (i >= rm_num)
    {
        return;
    }

    SetUpRoomCoordinate(*r_id_p, room_wrk.pos[1]);

    addr_si = (int *)(map_wrk.dat_adr + 11 * 4);
    addr_si = (int *)(*addr_si + BASE_ADDRESS);
    addr_si = (int *)(addr_si[i+1] + BASE_ADDRESS);

    dt_num = *(int *)(*addr_si + BASE_ADDRESS);

    no_use_fw = 0;

    for (i = 0; i < dt_num; i++)
    {
        addr_fdt = (int *)(addr_si[i+1] + BASE_ADDRESS);

        fdpp = (FURN_DATA_POP *)(*addr_fdt + BASE_ADDRESS);

        while(FurnIsWrkUse(&furn_wrk[no_use_fw]) != 0)
        {
            no_use_fw++;
        }

        FurnSetFurnWork(fdpp, no_use_fw, ingame_wrk.msn_no, 2, *r_id_p);

        if (furn_wrk[no_use_fw].furn_no != 0xffff && furn_wrk[no_use_fw].furn_no != 0)
        {
            ChoudoPreRender(&furn_wrk[no_use_fw]);

        }
        else if (
            furn_wrk[no_use_fw].furn_no == 0xffff && furn_wrk[no_use_fw].fno_bk != 0xffff &&
            furn_wrk[no_use_fw].fno_bk != 0
        )
        {
            furn_wrk[no_use_fw].furn_no = 0xffff;
        }
    }
}

void FurnPicDispOn()
{
    FURN_WRK *fwp0;
    int i;
    u_int attr;

    fwp0 = furn_wrk;

    for (i = 0; i < 60; i++)
    {
        if (fwp0->use == 2 || fwp0->use == 0)
        {
            attr = GetFurnAttr(fwp0->id, ingame_wrk.msn_no);

            if ((attr & 0x8) == 0)
            {
                if (attr & 0x40)
                {
                    if (fwp0->furn_no == 0xffff && fwp0->fno_bk != 0xffff)
                    {
                        fwp0->furn_no = fwp0->fno_bk;
                        fwp0->fs_flg = 1;

                        ChoudoPreRender(fwp0);
                    }
                }
            }
        }

        fwp0++;
    }
}

void FurnPicDispOff()
{
    FURN_WRK *fwp0;
    int i;
    u_int attr;

    fwp0 = furn_wrk;

    for (i = 0; i < 60; i++)
    {
        if (fwp0->use == 2 || fwp0->use == 0)
        {
            attr = GetFurnAttr(fwp0->id, ingame_wrk.msn_no);

            if ((attr & 0x8) == 0)
            {
                if (attr & 0x40) {
                    if (fwp0->furn_no != 0xffff && fwp0->fno_bk != 0xffff)
                    {
                        fwp0->furn_no = 0xffff;
                    }
                }
            }
        }

        fwp0++;
    }
}

void ClearFurnAttrFlg()
{
    memset(furn_attr_flg, 0, sizeof(furn_attr_flg));
}

void InitFurnAttrFlg()
{
    int addr_map;
    int *addr_si0;
    int *addr_si1;
    int *addr_si2;
    int i;
    int j;
    u_char room_num;
    u_char dat_num;
    int k;
    u_short *furn_no_addr;

    memset(furn_attr_flg, 0, sizeof(furn_attr_flg));

    for (k = 0; k < 4; k++)
    {
        if (floor_exist[ingame_wrk.msn_no][k] != 0)
        {
            addr_si0 = (int *)(k * 4 + BASE_ADDRESS);
            addr_map = (int)(*addr_si0 + BASE_ADDRESS);

            addr_si0 = (int *)(addr_map + 11 * 4);
            addr_si0 = (int *)(*addr_si0 + BASE_ADDRESS);

            addr_si1 = (int *)(*addr_si0 + BASE_ADDRESS);

            room_num = *addr_si1;

            for (i = 0; i < room_num; i++)
            {
                addr_si1 = (int *)(addr_si0[i+1] + BASE_ADDRESS);

                addr_map = (int)(*addr_si1 + BASE_ADDRESS);

                dat_num = *(u_char *)addr_map;

                for (j = 0; j < dat_num; j++)
                {
                    addr_si2 = (int *)(addr_si1[j+1] + BASE_ADDRESS);
                    addr_si2 = (int *)(*addr_si2 + BASE_ADDRESS);

                    furn_no_addr = &((u_short *)addr_si2)[9];

                    furn_attr_flg[*furn_no_addr].flg = furn_attr_dat[furn_dat[*furn_no_addr].attr_no][ingame_wrk.msn_no];
                }
            }
        }
    }
}

void SetFurnAttrEve(u_short id, u_short flg, int on_flg)
{
    FURN_WRK *fwp;
    int i;
    int j;
    int on;
    u_int mask;
    u_int checker;
    u_int furn_flg_bk;

    if (id >= 500)
    {
        return;
    }

    mask = flg;

    furn_flg_bk = furn_attr_flg[id].flg;

    if (on_flg != 0)
    {
        furn_attr_flg[id].flg = furn_flg_bk | flg;
    }
    else
    {
        flg = ~flg;

        furn_attr_flg[id].flg = furn_flg_bk & flg;
    }

    for (i = 0, fwp = furn_wrk; i < 60; i++, fwp++)
    {
        if (fwp->use != 2 && fwp->use != 0)
        {
            continue;
        }

        if (id != fwp->id)
        {
            continue;
        }

        for (j = 0; j < 32; j++)
        {
            checker = 0x80000000 >> j;

            if ((mask & checker) == 0 || (furn_flg_bk & checker) == (flg & checker))
            {
                continue;
            }

            on = (flg & checker) != 0;

            switch(j)
            {
            case 30:
            case 31:
                // do nothing ...
            break;
            case 16:
                if (on)
                {
                    FurnSetWrkNoUse(fwp, i);

                    FurnSortFurnWrk(0x1);
                }
                return;
            break;
            case 17:
                if (on)
                {
                    if (fwp->furn_no != 0xffff)
                    {
                        fwp->fno_bk = fwp->furn_no;
                        fwp->furn_no = 0xffff;

                        FurnSortFurnWrk(1);
                    }
                }
                else if (fwp->fno_bk != 0xffff)
                {
                    fwp->furn_no = fwp->fno_bk;
                    fwp->fno_bk = 0xffff;
                    fwp->fs_flg = 1;

                    if (fwp->furn_no != 0)
                    {
                        ChoudoPreRender(fwp);
                    }

                    FurnSortFurnWrk(1);
                }
            break;
            case 29:
                if (on)
                {
                    FurnEfctFree(fwp);
                }
                else
                {
                    FurnEfctSet(fwp);
                }
            break;
            }
        }

        break;
    }
}

u_int GetFurnAttrF(FURN_WRK *fwp, u_char msn_no)
{
    if (fwp != NULL)
    {
        return GetFurnAttr(fwp->id, msn_no);
    }

    return furn_attr_dat[0][0];
}

u_int GetFurnAttr(u_short id, u_char msn_no)
{
    if (id < 500)
    {
        return furn_attr_flg[id].flg;
    }

    return 0;
}

u_char FurnHitCheck(u_char *dx_max, u_char *dz_max, sceVu0FVECTOR pos, sceVu0FVECTOR dst, u_char div, u_char room)
{
    int *addr;
    int *addr_bak;
    int *addr_bak2;
    FURN_DATA_POP *fedp;
    u_char *sq_typep;
    int i;
    int j;
    int k;
    u_char dt_num;
    u_char sq_num;
    u_char rm_no;
    u_char div_x;
    u_char div_z;
    u_short pos_x;
    u_short pos_y;
    int *v1, *v0, *v0_2; // not in STAB

    div_x = *dx_max;
    div_z = *dz_max;

    addr = (int *)(map_wrk.dat_adr + 11 * 4);
    addr = (int *)(*addr + BASE_ADDRESS);

    rm_no = GetDataRoom(11, room);

    if (rm_no == 0xff)
    {
        return 0;
    }

    v0_2 = (int *)&addr[rm_no];
    addr_bak = addr = (int *)(v0_2[1] + BASE_ADDRESS);

    dt_num = (u_int)*(u_char *)(*addr_bak + BASE_ADDRESS);

    for (i = 0; i < dt_num; i++)
    {
        v0 = &addr_bak[i];
        addr = (int *)(v0[1] + BASE_ADDRESS);

        fedp = (FURN_DATA_POP *)(*addr + BASE_ADDRESS);

        if (FurnIsHit(fedp->id, ingame_wrk.msn_no) != 0)
        {
            sq_num = fedp->snum;

            sq_typep = &((u_char *)fedp)[26];
            addr++;
            addr_bak2 = addr;

            for (j = 1; j < sq_num; j++, sq_typep++)
            {
                addr = &addr_bak2[j];
                addr = (int *)(*addr + BASE_ADDRESS);

                for (k = 0; k < div_x; k++)
                {
                    pos_y = pos[0] + (dst[0] * (k + 1)) / div;
                    pos_x = pos[2];

                    if (PosInAreaJudgeSub(addr, pos_x, pos_y, *sq_typep) != 0)
                    {
                        div_x = k;

                        break;
                    }
                }

                for (k = 0; k < div_z; k++)
                {

                    pos_y = pos[0];
                    pos_x = pos[2] + (dst[2] * (k + 1)) / div;
                    if (PosInAreaJudgeSub(addr, pos_x, pos_y, *sq_typep) != 0)
                    {
                        div_z = k;

                        break;
                    }
                }
            }
        }
    }

    for (i = 0; i < dt_num; i++)
    {
        v1 = &addr_bak[i];
        addr = (int *)(v1[1] + BASE_ADDRESS);

        fedp = (FURN_DATA_POP *)(*addr + BASE_ADDRESS);

        if (FurnIsHit(fedp->id, ingame_wrk.msn_no) != 0)
        {
            sq_num = fedp->snum;

            sq_typep = &((u_char *)fedp)[26];
            addr++;
            addr_bak2 = addr;

            for (j = 1; j < sq_num; j++, sq_typep++)
            {
                addr = &addr_bak2[j];
                addr = (int *)(*addr + BASE_ADDRESS);

                pos_y = pos[0] + (dst[0] * div_x) / div;
                pos_x = pos[2] + (dst[2] * div_z) / div;

                if (PosInAreaJudgeSub(addr, pos_x, pos_y, *sq_typep) != 0)
                {
                    if (div_z < div_x)
                    {
                        div_x = div_z;
                    }

                    for (k = 0; k < div_x; k++)
                    {
                        pos_y = pos[0] + (dst[0] * (k+1)) / div;
                        pos_x = pos[2] + (dst[2] * (k+1)) / div;

                        if (PosInAreaJudgeSub(addr, pos_x, pos_y, *sq_typep) != 0)
                        {
                            div_x = k;
                            div_z = k;

                            break;
                        }
                    }
                }
            }
        }
    }

    if (*dx_max == div_x && *dz_max == div_z)
    {
        return 0;
    }

    *dx_max = div_x;
    *dz_max = div_z;

    return 1;
}

u_char FurnHitCheck2(u_short pos_x, u_short pos_y, u_char room_id)
{
    FURN_DATA_POP *fedp;
    int *addr_si;
    int *addr_bak_si;
    int *addr_bak2_si;
    u_char *sq_typep;
    int i;
    int j;
    u_char room_no;
    u_char dt_num;
    u_char sq_num;
    int *v0, *v1; // not in STAB

    addr_si = (int *)(map_wrk.dat_adr + 11 * 4);
    addr_si = (int *)(*addr_si + BASE_ADDRESS);
    room_no = GetDataRoom(11, room_id);

    if (room_no == 0xff)
    {
        return 0;
    }

    v0 = &addr_si[room_no];
    addr_bak_si = addr_si = (int *)(v0[1] + BASE_ADDRESS);

    dt_num = *(int *)(*addr_bak_si + BASE_ADDRESS);

    for (i = 0; i < dt_num; i++)
    {
        v1 = &addr_bak_si[i];
        addr_si = (int *)(v1[1] + BASE_ADDRESS);
        fedp = (FURN_DATA_POP *)(*addr_si + BASE_ADDRESS);

        if (FurnIsHit(fedp->id, ingame_wrk.msn_no) == 0)
        {
            continue;
        }

        sq_num = fedp->snum;

        if (sq_num == 0)
        {
            continue;
        }

        sq_typep = &((u_char *)fedp)[26];

        addr_bak2_si = &addr_si[1];

        for (j = 1; j < sq_num; j++, sq_typep++)
        {
            addr_si = &addr_bak2_si[j];
            addr_si = (int *)(*addr_si + BASE_ADDRESS);

            if (PosInAreaJudgeSub(addr_si, pos_x, pos_y, *sq_typep) != 0)
            {
                return 1;
            }
        }
    }

    return 0;
}
