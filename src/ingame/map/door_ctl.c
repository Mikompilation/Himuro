#include "common.h"
#include "typedefs.h"
#include "enums.h"
#define INCLUDING_FROM_DOOR_CTL_C
#include "door_ctl.h"
#undef INCLUDING_FROM_DOOR_CTL_C

#include "common/ul_math.h"
#include "graphics/graph3d/gra3d.h"
#include "graphics/graph3d/load3d.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/motion/motion.h"
#include "ingame/entry/ap_fgost.h"
#include "ingame/entry/entry.h"
#include "ingame/event/ev_main.h"
#include "ingame/ig_glob.h"
#include "ingame/map/door_ctl.h"
#include "ingame/map/doortype.h"
#include "ingame/map/furn_ctl.h"
#include "ingame/map/map_area.h"
#include "ingame/map/map_ctrl.h"
#include "ingame/map/map_htck.h"
#include "ingame/map/rotd_ctl.h"
#include "ingame/plyr/plyr_ctl.h"
#include "ingame/plyr/unit_ctl.h"
#include "main/glob.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"
#include "os/eeiop/se_door.h"
#include "os/eeiop/se_event.h"
#include "os/eeiop/se_foot.h"
#include "os/eeiop/se_srund.h"
#include "os/eeiop/se_trans.h"

static short int SearchAcDoor(int door_id);
static short int SearchNcDoor(int door_id);
static void InitDoorOpenCtrl();
static void SetDoorSttsData(int dw_no, int door_id, u_char type);
static void SetDoorSttsEve(int door_id, u_char stts);
static void SetDoorLockEve(int door_id, u_char lock);
static void SetDoorWrk(int dw_no, u_int *addr, int dr_id, int fwrk_no);
static void DoorSetFurnWrk(int dwrk_no, int fwrk_no, u_char use, u_char room_id);
static void DoorCtrlOpen();
static void SetNcDwrk(int nc_dwrk_pos, int dwrk_pos, u_char stts);
static void SetAcDwrk(int ac_dwrk_pos, int dwrk_pos, u_char stts);
static void DoorCtrlAutoClose();
static int DoorReOpenCheck(sceVu0FVECTOR pos, int dwrk_no, u_short count, u_char type);
static void DoorCtrlNoClose();
static float DoorOpenCtrlRot(float count, float move_max, float open_wait);
static short int DoorOpenCtrlSlide(u_short count, short int move_max, u_short open_wait);
static void SetDoorOpenPlyrPos();
static u_char DoorOpenCheck(u_char chk_stat);
static int GetDoorBaseRot(DOJ_SQUARE_MTN *dsmp, float *dbrot1, float *dbrot2, u_char is_dbl, u_char is_door);
static void DoorOpenCheckSquareArea(DOJ_SQUARE_MTN *dsmp, u_char stat_chk, u_char hs_chk);
static u_char DoorOpenSttsPermissionCheck(int dwrk_pos, u_char stat_chk);
static void DoorInitDSM(DOJ_SQUARE_MTN *dsmp);
static void DoorSetDSM(DOJ_SQUARE_MTN *dsmp, int dwrk_no, u_char type, u_char sq_no);
static u_char DoorGetOpenSquareNo(DOJ_SQUARE_MTN *dsmp);
static int DoorLockChange(u_short door_id, u_char lock);
static void GetFusumaHitVertex(sceVu0FVECTOR axis, sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, sceVu0FVECTOR v3, float pre_rot, u_short count, u_short type);
static void GetFusumaHitVertex2(sceVu0FVECTOR axis, sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, sceVu0FVECTOR v3, float pre_rot, u_short count, u_short type, u_char mv);
static void GetDoorHitVertex(sceVu0FVECTOR axis, sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, sceVu0FVECTOR v3, float pre_rot, u_short count, u_short type);
static void GetDoorHitVertex2(sceVu0FVECTOR axis, sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, sceVu0FVECTOR v3, float pre_rot, u_short count, u_short type, u_char mv);
static int ChkRotBetween(float sa, float sb, float rot);
static u_short* GetDoorDoorNumP();
static int* GetDoorDataTopP(u_short door_id);
static int* GetDoorDataHeader(u_short door_id);
static int ChkDoorIsOpen(u_short door_id);
static u_short GetDoorIdFromDoorNo(u_char door_no);
static int GetDoorStts(DOOR_DATA_POP **dppp, u_short door_id, u_char floor);

AREA_READ_WRK area_read_wrk = {0};
DOOR_OPEN_CTRL door_open_ctrl = {0};
DOOR_WRK door_wrk[20] = {0};
AUTO_CLOSE_DOOR_WRK ac_door_wrk[10] = {0};
NON_CLOSE_DOOR_WRK nc_door_wrk[50] = {0};
DOJ_SQUARE_MTN doj_square_mtn = {0};
sceVu0FVECTOR tmp_area_end_door_pos = { 0.0f, 0.0f, 0.0f, 0.0f };

static char load_conte_flg;

static u_short lock_did[20];

#define PI 3.1415927f
#define DEG2RAD(x) ((float)(x)*PI/180.0f)

#define BASE_ADDRESS 0x7f8000

u_char IsUseDoor(u_char use)
{
    if (use == 1 || use == 3 || use == 4)
    {
        return 1;
    }

    return 0;
}

short int SearchDoorWrk(int door_id)
{
    DOOR_WRK *dwp;
    int i;

    for (i = 0, dwp = door_wrk; i < 0x14; i++, dwp++)
    {
        if (IsUseDoor(dwp->use) != 0)
        {
            if (dwp->door_id == door_id)
            {
                return i;
            }
        }
    }

    return -1;
}

static short int SearchAcDoor(int door_id)
{
    AUTO_CLOSE_DOOR_WRK *acp;
    int i;

    acp = ac_door_wrk;

    for (i = 0; i < 10; i++, acp++)
    {
        if (acp->door_id == door_id)
        {
            return i;
        }
    }

    return -1;
}

static short int SearchNcDoor(int door_id)
{
    NON_CLOSE_DOOR_WRK *ncp;
    int i;

    ncp = nc_door_wrk;

    for (i = 0; i < 50; i++, ncp++)
    {
        if (ncp->door_id == door_id)
        {
            return i;
        }
    }

    return -1;
}

void ClearDoorAttrFlg()
{
    memset(door_keep, 0, sizeof(door_keep));
}

void InitDoorAttrFlg()
{
    int addr_map;
    int *addr_si0;
    int *addr_si1;
    u_short *addr_us;
    int i;
    u_short dat_num;
    int k;

    memset(door_keep, 0, sizeof(door_keep));

    for (k = 0; k < 4; k++)
    {
        if (floor_exist[ingame_wrk.msn_no][k] != 0)
        {
            addr_si0 = (int *)(k * 4 + BASE_ADDRESS);
            addr_map = (int)(*addr_si0 + BASE_ADDRESS);
            addr_si0 = (int *)(((int *)addr_map)[10] + BASE_ADDRESS);

            addr_si1 = (int *)(*addr_si0 + BASE_ADDRESS);

            addr_us = (u_short *)(addr_si1[1] + BASE_ADDRESS);
            dat_num = *addr_us;
            addr_us++;

            addr_si0 = (int *)(addr_si0[2] + BASE_ADDRESS);

            for (i = 0; i < dat_num; i++)
            {
                addr_si1 = (int *)(addr_si0[i] + BASE_ADDRESS);
                addr_si1 = (int *)(*addr_si1 + BASE_ADDRESS);

                door_keep[*addr_us].attr = GetDoorTypeDatP(((u_short *)addr_si1)[6])->attribute;
                door_keep[*addr_us].room_id = GetDoorTypeDatP(((u_short *)addr_si1)[6])->room_id;
                door_keep[*addr_us].mdl_no = ((u_short *)addr_si1)[7];

                if (door_keep[*addr_us].attr & 0x4000)
                {
                    door_keep[*addr_us].stts = (0x4 | 0x1);
                }
                else
                {
                    door_keep[*addr_us].stts = 0x1;
                }

                addr_us++;
            }
        }
    }
}

void DoorPassRoom(u_char room_id)
{
    int *addr_si0;
    u_char *addr_uc0;
    int i;
    u_char dr_num;
    u_char room_no;

    room_no = GetDataRoom(10, room_id);

    if (room_no == 0xff)
    {
        return;
    }

    addr_si0 = (int *)(map_wrk.dat_adr + 0x28);
    addr_si0 = (int *)(*addr_si0 + BASE_ADDRESS);
    addr_si0 = (int *)(addr_si0[1] + BASE_ADDRESS);

    addr_uc0 = (u_char *)(addr_si0[room_no] + BASE_ADDRESS);

    dr_num = *addr_uc0;

    addr_uc0 += 2;

    for (i = 0; i < dr_num; i++)
    {
        if (*(u_short *)addr_uc0 < 300 && (door_keep[*(u_short *)addr_uc0].attr & 0x1) == 0 && (door_keep[*(u_short *)addr_uc0].attr & 0x8000) == 0)
        {
            door_keep[*(u_short *)addr_uc0].attr |= 0x1;

            DoorPassSetSttsMap(*(u_short *)addr_uc0);
        }

        addr_uc0 += 4;
    }
}

void DoorRDoorSetRotOK(u_short door_id)
{
    DOOR_WRK *dwp;
    int i;

    if (door_id >= 300)
    {
        return;
    }

    dwp = door_wrk;

    for (i = 0; i < 20; i++)
    {
        if (IsUseDoor(dwp->use) != 0x0 && dwp->door_id == door_id && GetDoorTypeDatP(dwp->type)->room_id != 0xff)
        {
            door_keep[door_id].attr |= 0x2;

            return;
        }

        dwp++;
    }
}

void DoorPassSetSttsMap(u_short door_id)
{
    if (door_id >= 300)
    {
        return;
    }

    if ((door_keep[door_id].attr & 0x8000) == 0 && door_keep[door_id].attr & 0x1)
    {
        if (door_keep[door_id].attr & 0x2000)
        {
            door_keep[door_id].stts_map = 1;
        }
        else
        {
            door_keep[door_id].stts_map = 3;
        }
    }
}

void DoorPassSetHide(u_short door_id)
{
    if (door_id < 300)
    {
        if (door_keep[door_id].attr & 0x8000)
        {
            door_keep[door_id].attr |= 0x1;
            door_keep[door_id].stts_map = 2;
        }
    }
}

void DoorPassSetTouch(u_short door_id)
{
    if (door_id >= 300)
    {
        return;
    }

    if ((door_keep[door_id].attr & 0x8000) == 0 && door_keep[door_id].attr & 0x1)
    {
        if (door_keep[door_id].attr & 0x2000)
        {
            door_keep[door_id].stts_map = 0x1;
        }
        else
        {
            door_keep[door_id].stts_map = (0x2 | 0x1);
        }
    }
}

void DoorAcInit()
{
    int i;

    memset(door_wrk, 0, sizeof(door_wrk));
    memset(ac_door_wrk, 0, sizeof(ac_door_wrk));

    for (i = 0; i < 10; i++)
    {
        ac_door_wrk[i].stts = 0;
    }
}

void DoorCtrlInit()
{
    int i;
    int j;
    int *addr;
    u_short *addr_us;
    u_short ncd_num;

    memset(nc_door_wrk, 0, sizeof(nc_door_wrk));

    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 20; j++)
        {
            if (ac_door_wrk[i].door_id != door_wrk[j].door_id || ac_door_wrk[i].stts == 0x0)
            {
                door_wrk[j].door_id = 0xffff;
                door_wrk[j].stts = 0x0;
                door_wrk[j].use = 5;
            }
        }
    }

    for (i = 0; i < 50; i++)
    {
        nc_door_wrk[i].stts = 0x0;
    }

    addr = (int *)(map_wrk.dat_adr + 10 * 4);
    addr = (int *)(*addr + BASE_ADDRESS);
    addr = (int *)(*addr + BASE_ADDRESS);

    addr_us = (u_short *)(addr[2] + BASE_ADDRESS);

    ncd_num = *addr_us;

    addr_us += 1;

    for (i = 0; i < ncd_num; i++, addr_us++)
    {
        nc_door_wrk[i].stts = door_keep[*addr_us].stts;
        nc_door_wrk[i].type = 2;
        nc_door_wrk[i].door_id = *addr_us;
        nc_door_wrk[i].count = 0;
    }

    InitDoorOpenCtrl();
}

static void InitDoorOpenCtrl()
{
    door_open_ctrl.stts = 0;
    door_open_ctrl.mode = 0;
    door_open_ctrl.count = 0;
    door_open_ctrl.move_flame = 0;
    door_open_ctrl.open[0] = 0;
    door_open_ctrl.open[1] = 0;
    door_open_ctrl.chk_flg = 0;

    room_wrk.disp_no[1] = 0xff;
}

void InitAreaReadWrk()
{
    area_read_wrk = (AREA_READ_WRK){0};
}

void DoorDataInit()
{
    int i;
    int j;
    int *addr_si;
    u_char *addr_uc;
    u_short *dr_tbl;
    u_char dr_all;
    u_char dr_num;
    u_short dr_id;
    u_short dr_no;
    u_char no_use_fw;
    u_char no_use_dw;
    u_char room_id;
    u_char room_no;

    GetDoorTypeDatP(0);

    if (room_wrk.room_no == 0xff)
    {
        return;
    }

    addr_si = (int *)(map_wrk.dat_adr + 10 * 4);
    addr_si = (int *)(*addr_si + BASE_ADDRESS);
    addr_si = (int *)(addr_si[1] + BASE_ADDRESS);

    room_id = GetRoomIdFromRoomNo(0, room_wrk.room_no);
    room_no = GetDataRoom(10, room_id);

    if (room_no == 0xff)
    {
        return;
    }

    addr_si = (int *)(addr_si[room_no] + BASE_ADDRESS);

    SetUpRoomCoordinate(room_id, room_wrk.pos[0]);

    dr_num = *(u_char *)addr_si;
    addr_uc = (u_char *)((int)addr_si + 2);

    no_use_fw = 0;
    no_use_dw = 0;

    for (i = 0; i < dr_num; i++)
    {
        dr_id = *(u_short *)addr_uc;

        addr_uc += 4;

        dr_tbl = GetDoorDoorNumP();

        dr_all = *dr_tbl++;

        for (j = 0; j < dr_all; j++, dr_tbl++)
        {
            if (*dr_tbl == dr_id)
            {
                dr_no = j;

                break;
            }
        }

        if (dr_all <= dr_no)
        {
            return;
        }

        while (FurnIsWrkUse(&furn_wrk[no_use_fw]) != 0)
        {
            no_use_fw++;

            if (no_use_fw > 250)
            {
                break;
            }
        }

        while (door_wrk[no_use_dw].use != 5)
        {
            no_use_dw++;

            if (no_use_dw > 250)
            {
                break;
            }
        }

        addr_si = (int *)(map_wrk.dat_adr + 10 * 4);
        addr_si = (int *)(*addr_si + BASE_ADDRESS);
        addr_si = (int *)(addr_si[2] + BASE_ADDRESS);
        addr_si = (int *)(addr_si[dr_no] + BASE_ADDRESS);
        addr_si = (int *)(*addr_si + BASE_ADDRESS);

        SetDoorWrk(no_use_dw, (u_int*)addr_si, dr_id, no_use_fw);

        door_wrk[no_use_dw].use = 1;

        DoorSetFurnWrk(no_use_dw, no_use_fw, 1, room_id);
    }
}

static void SetDoorSttsData(int dw_no, int door_id, u_char type)
{
    int i;

    door_wrk[dw_no].stts = door_keep[door_id].stts;

    if (door_keep[door_id].attr & 0x2000)
    {
        door_wrk[dw_no].lock = 0x1;
    }
    else
    {
        door_wrk[dw_no].lock = 0x0;
    }

    door_wrk[dw_no].mdl_no = door_keep[door_id].mdl_no;

    for (i = 0; i < 50; i++)
    {
        if (nc_door_wrk[i].door_id == door_id)
        {
            SetNcDwrk(i, dw_no, door_wrk[dw_no].stts);

            return;
        }
    }

    for (i = 0; i < 10; i++)
    {
        if (ac_door_wrk[i].stts != 0x0 && ac_door_wrk[i].door_id == door_id)
        {
            SetAcDwrk(i, dw_no, door_wrk[dw_no].stts);

            return;
        }
    }
}

static void SetDoorSttsEve(int door_id, u_char stts)
{
    DOOR_STTS_KEEP *dskp;

    if (door_id >= 300)
    {
        return;
    }

    door_keep[door_id].stts = stts;

    if (stts == (0x4 | 0x1))
    {
        door_keep[door_id].attr |= 0x4000;
    }

    else if (stts == 0x1)
    {
        door_keep[door_id].attr &= ~0x4000;
    }
}

static void SetDoorLockEve(int door_id, u_char lock)
{
    DOOR_STTS_KEEP *dskp;

    if (door_id >= 300)
    {
        return;
    }

    dskp = &door_keep[door_id];

    switch (lock)
    {
    case 1:
        dskp->attr |= 0x2000;
    break;
    case 0:
        dskp->attr &= ~0x2000;
    break;
    }
}

void DoorSetAllLocked()
{
    DOOR_STTS_KEEP *dskp;
    DOOR_WRK *dwp;
    int i;

    dskp = door_keep;

    for (i = 0; i < 300; i++)
    {
        dskp->attr |= 0x2000;

        dskp++;
    }

    dwp = door_wrk;

    for (i = 0; i < 20; i++)
    {
        dwp->lock = 1;

        dwp++;
    }
}

int IsInDoorSpuare()
{
    DOJ_SQUARE_MTN dsm;

    DoorInitDSM(&dsm);
    DoorOpenCheckSquareArea(&dsm, 1, 1);

    if (dsm.dojs[0].type == 0xff)
    {
        return 0;
    }

    return CheckRotDoorOpenStatus(dsm.dojs[0].dwrk_no, dsm.dojs[0].type) != 0;
}

u_char GetRoomIdBeyondDoor2()
{
    return GetRoomIdBeyondDoor(door_wrk[doj_square_mtn.dwno_base].door_id, room_wrk.disp_no[0]);
}

u_char GetRoomIdBeyondDoor(u_short door_id, u_char now_room_id)
{
    u_char *addr_uc;
    int i;
    u_char next_room_id;
    u_char dr_num;

    next_room_id = 0xff;

    addr_uc = GetDoorRoomConectDataP(now_room_id);

    if (addr_uc == NULL)
    {
        return 0xff;
    }

    dr_num = *addr_uc;

    addr_uc += 2;

    for (i = 0; i < dr_num; i++)
    {
        if (*(u_short *)addr_uc == door_id)
        {
            next_room_id = addr_uc[2];

            break;
        }

        addr_uc += 4;
    }

    return next_room_id;
}

u_char IsAllConnectDoorClose()
{
    DOOR_WRK *dwp;
    NON_CLOSE_DOOR_WRK *ncp;
    int i;
    int j;
    u_char dscv_flg;

    dwp = door_wrk;

    for (i = 0; i < 20; i++)
    {
        if (IsUseDoor(dwp->use) != 0 && dwp->use != 5)
        {
            dscv_flg = 0;

            if (dwp->stts != 0x1)
            {
                ncp = nc_door_wrk;

                for (j = 0; j < 50; j++)
                {
                    if (ncp->door_id == dwp->door_id)
                    {
                        dscv_flg = 1;
                    }

                    ncp++;
                }

                if (dscv_flg == 0)
                {
                    return 0;
                }
            }
        }

        dwp++;
    }

    return 1;
}

void LockAllDoorInNowRoom()
{
    NON_CLOSE_DOOR_WRK *ncp;
    AUTO_CLOSE_DOOR_WRK *acp;
    u_char *addr_uc;
    int i;
    int j;
    int k;
    u_short tmp_did;
    u_char now_room_id;
    u_char next_room_id;
    u_char dr_num;
    u_char nuke;
    u_char lock_did_num;
    u_char room_no;

    memset(lock_did, 0xff, sizeof(lock_did));

    lock_did_num = 0;

    next_room_id = 0xff;

    room_no = GetRoomIdFromRoomNo(0, room_wrk.room_no);

    if (room_no == 0xff)
    {
        return;
    }

    addr_uc = GetDoorRoomConectDataP(room_no);

    if (addr_uc == NULL)
    {
        return;
    }

    dr_num = *addr_uc;

    addr_uc += 2;

    for (i = 0; i < dr_num; i++)
    {
        tmp_did = *(u_short *)addr_uc;

        next_room_id = addr_uc[2];

        addr_uc += 4;

        if (next_room_id == 0xff || next_room_id == room_no)
        {
            continue;
        }

        if ((door_keep[tmp_did].attr & 0x2000) == 0)
        {
            door_keep[tmp_did].attr |= 0x2000;

            lock_did[lock_did_num++] = tmp_did;

            for (j = 0; j < 20; j++)
            {
                if (door_wrk[j].use == 5)
                {
                    continue;
                }

                if (door_wrk[j].door_id == tmp_did)
                {
                    nuke = 0;

                    ncp = nc_door_wrk;

                    for (k = 0; k < 50; k++)
                    {
                        if (ncp->door_id == door_wrk[j].door_id)
                        {
                            nuke = 1;

                            break;
                        }

                        ncp++;
                    }

                    if (!nuke)
                    {
                        acp = ac_door_wrk;

                        for (k = 0; k < 10; k++)
                        {
                            if (acp->door_id == door_wrk[j].door_id)
                            {
                                acp->stts = 0x1;
                            }

                            acp++;
                        }

                        door_wrk[j].lock = 1;
                        door_wrk[j].stts = 0x1;
                    }
                }
            }
        }
    }

    for (i = 0; i < 20; i++)
    {
        // debug code?
    }

}

void UnlockAllDoorInNowRoom()
{
    DOOR_WRK *dwp;
    int i;
    int j;

    for (i = 0; i < 20; i++)
    {
        if (lock_did[i] != 0xffff)
        {
            door_keep[lock_did[i]].attr &= ~0x2000;

            dwp = door_wrk;

            for (j = 0; j < 20; j++)
            {
                if (dwp->use != 5 && dwp->door_id == lock_did[i])
                {
                    dwp->lock = 0;
                }

                dwp++;
            }

            lock_did[i] = 0xffff;
        }
    }
}

u_char IsDoorLoad(u_char type)
{
    DOOR_TYPE_DAT *dtd;

    dtd = GetDoorTypeDatP(type);

    if (dtd->attribute & 0x10)
    {
        return 1;
    }

    return 0;
}

int GetRoomDoorID(u_char room_id, u_short *door_id, u_char msn_no)
{
    u_int *addr_cmn;
    u_int *adr_door_data_top;
    u_int *adr_drrm_top;
    u_int *adr_drdt_top;
    u_char *adr_rmnum;
    u_short *adr_drnum;
    u_char *adr_dr_tmp;
    u_int *adr_dd_tmp;
    u_short *adr_dtmp;
    u_char *adr_rtmp;
    u_char *adr_tmp_uc0;
    int ret;
    int i;
    int j;
    int k;
    u_short all_door_num;
    u_char dr_room_num;
    u_short door_num;

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

        adr_drrm_top = (u_int *)GetFloorTopAddr(k);

        adr_door_data_top = (u_int *)(adr_drrm_top[10] + BASE_ADDRESS);

        addr_cmn = (u_int *)(adr_door_data_top[0] + BASE_ADDRESS);
        adr_rtmp = (u_char *)(addr_cmn[0] + BASE_ADDRESS);

        dr_room_num = adr_rtmp[0];

        adr_drnum = (u_short *)(addr_cmn[1] + BASE_ADDRESS);

        all_door_num = adr_drnum[0];

        adr_drrm_top = (u_int *)(adr_door_data_top[1] + BASE_ADDRESS);
        adr_drdt_top = (u_int *)(adr_door_data_top[2] + BASE_ADDRESS);

        adr_rmnum = adr_rtmp + 1;

        for (i = 0; i < dr_room_num; i++, adr_rmnum++)
        {
            if (*adr_rmnum != room_id)
            {
                continue;
            }

            adr_dr_tmp = (u_char *)(adr_drrm_top[i] + BASE_ADDRESS);

            door_num = adr_dr_tmp[0];
            adr_dr_tmp += 2;

            for (j = 0; j < door_num; j++, adr_dr_tmp += 4)
            {
                adr_dtmp = &adr_drnum[1];

                for (k = 0; k < all_door_num; k++, adr_dtmp++)
                {
                    if ((*((u_short *)adr_dr_tmp)) == adr_dtmp[0])
                    {
                        adr_dd_tmp = (u_int *)(adr_drdt_top[k] + BASE_ADDRESS);
                        adr_tmp_uc0 = (u_char *)(adr_dd_tmp[0] + BASE_ADDRESS);

                        door_id[0] = adr_tmp_uc0[14];

                        door_id++;
                        ret++;
                    }
                }
            }

            return ret;
        }
    }

    return 0;
}

static void SetDoorWrk(int dw_no, u_int *addr, int dr_id, int fwrk_no)
{
    DOOR_WRK *dwp;

    dwp = &door_wrk[dw_no];

    memcpy(&dwp->rot, addr++, sizeof(float));

    RotLimitChk(&dwp->rot);

    dwp->pos[0] = ((u_short *)addr)[0];
    dwp->pos[1] = ((short *)addr)[1];
    dwp->pos[2] = ((u_short *)addr)[2];

    dwp->npos[0] = dwp->pos[0];
    dwp->npos[1] = dwp->pos[1];
    dwp->npos[2] = dwp->pos[2];

    dwp->type = ((short *)addr)[4];
    dwp->fwrk_no = fwrk_no;
    dwp->door_id = dr_id;

    SetDoorSttsData(dw_no, dr_id, dwp->type);

    if (GetDoorTypeDatP(dwp->type)->room_id != door_keep[dwp->door_id].room_id)
    {
        dwp->rot += PI;
    }

    RotLimitChk(&dwp->rot);

    dwp->nrot = dwp->rot;
}

static void DoorSetFurnWrk(int dwrk_no, int fwrk_no, u_char use, u_char room_id)
{
    if (dwrk_no >= 20 || fwrk_no >= 60)
    {
        return;
    }

    furn_wrk[fwrk_no].rotate[0] = 0.0f;
    furn_wrk[fwrk_no].rot = door_wrk[dwrk_no].rot + PI;

    RotLimitChk(&furn_wrk[fwrk_no].rot);

    furn_wrk[fwrk_no].rotate[1] = furn_wrk[fwrk_no].rot;
    furn_wrk[fwrk_no].rotate[2] = 0.0f;

    furn_wrk[fwrk_no].pos[0] = door_wrk[dwrk_no].pos[0];
    furn_wrk[fwrk_no].pos[1] = door_wrk[dwrk_no].pos[1];
    furn_wrk[fwrk_no].pos[2] = door_wrk[dwrk_no].pos[2];

    furn_wrk[fwrk_no].use = use;
    furn_wrk[fwrk_no].room_id = room_id;
    furn_wrk[fwrk_no].furn_no = door_wrk[dwrk_no].mdl_no + 1000;
    furn_wrk[fwrk_no].fno_bk = -1;

    ChoudoPreRender(&furn_wrk[fwrk_no]);

    if (door_keep[door_wrk[dwrk_no].door_id].attr & 0x200)
    {
        furn_wrk[fwrk_no].fno_bk = door_wrk[dwrk_no].mdl_no + 1000;
        furn_wrk[fwrk_no].furn_no = -1;
    }
}

void DoorCtrlMain()
{
    DoorCtrlOpen();
    DoorCtrlAutoClose();
    DoorCtrlNoClose();
}

u_char IsDoorOpenCtrlMode()
{
    return door_open_ctrl.stts & 0x1;
}

static void DoorCtrlOpen()
{
    DOJ_SQUARE_MTN *dsmtnp;
    DOOR_MTN_DAT *dmotionp;
    AUTO_CLOSE_DOOR_WRK *acp;
    NON_CLOSE_DOOR_WRK *ncp;
    float tmp_rot;
    int i;
    int j;
    static int load_id;
    static u_char req_flg;
    u_short tmp_rot_door_id;

    dsmtnp = &doj_square_mtn;

    if ((door_open_ctrl.stts & 0x1) == 0)
    {
        return;
    }

    if ((plyr_wrk.sta & 0x8) == 0)
    {
        door_open_ctrl.stts ^= 1;
    }

    switch(door_open_ctrl.mode)
    {
    case DOCM_FREE:
        door_open_ctrl.spos[0] = plyr_wrk.move_box.pos[0];
        door_open_ctrl.spos[1] = plyr_wrk.move_box.pos[1];
        door_open_ctrl.spos[2] = plyr_wrk.move_box.pos[2];

        door_open_ctrl.mtn_dat_no = dsmtnp->mtn_no;

        GetDoorTypeDatP(door_open_ctrl.mtn_dat_no);

        dmotionp = GetDoorMtnDat(door_open_ctrl.mtn_dat_no);

        tmp_rot = door_wrk[dsmtnp->dwno_base].rot;

        RotLimitChk(&tmp_rot);

        if (dsmtnp->dbl_flg == 0)
        {
            if (dsmtnp->exc_flg != 0)
            {
                tmp_rot += PI;

                RotLimitChk(&tmp_rot);
            }
        }

        door_open_ctrl.epos[0] =
             dmotionp->sx * SgCosf(tmp_rot) + dmotionp->sy * SgSinf(tmp_rot) + door_wrk[dsmtnp->dwno_base].pos[0];

        door_open_ctrl.epos[1] = plyr_wrk.move_box.pos[1];

        door_open_ctrl.epos[2] =
             (dmotionp->sy * SgCosf(tmp_rot) - dmotionp->sx * SgSinf(tmp_rot)) + door_wrk[dsmtnp->dwno_base].pos[2];

        door_open_ctrl.door_pre = tmp_rot;
        door_open_ctrl.srot = plyr_wrk.move_box.rot[1] - tmp_rot;

        RotLimitChk(&door_open_ctrl.srot);

        door_open_ctrl.erot = 0.0f;

        RotLimitChk(&door_open_ctrl.erot);

        door_open_ctrl.spos[0] = door_open_ctrl.epos[0];
        door_open_ctrl.spos[1] = door_open_ctrl.epos[1];
        door_open_ctrl.spos[2] = door_open_ctrl.epos[2];

        door_open_ctrl.srot = door_open_ctrl.erot;

        SetDoorOpenCameraNo();

        door_open_ctrl.mode = DOCM_LOAD_WAIT;

        OneRoomLoadReq();
    case DOCM_LOAD_WAIT:
        door_open_ctrl.move_flame = 40;
        door_open_ctrl.wait_flame = 5;

        SetPlyrAnime(dmotionp->anm_no, door_open_ctrl.wait_flame);

        door_open_ctrl.mode = DOCM_WAIT_MOTION;

        for (i = 0; i < 2; i++)
        {
            door_open_ctrl.open[i] = 0;
        }

        SetPlyrAnime(dmotionp->anm_no, door_open_ctrl.move_flame);
    case DOCM_WAIT_MOTION:
        if (door_open_ctrl.move_flame > door_open_ctrl.count)
        {
            plyr_wrk.move_box.pos[0] = door_open_ctrl.spos[0] +
                 (door_open_ctrl.epos[0] - door_open_ctrl.spos[0]) * door_open_ctrl.count / door_open_ctrl.move_flame;

            plyr_wrk.move_box.pos[1] = door_open_ctrl.spos[1] +
                 (door_open_ctrl.epos[1] - door_open_ctrl.spos[1]) * door_open_ctrl.count / door_open_ctrl.move_flame;

            plyr_wrk.move_box.pos[2] = door_open_ctrl.spos[2] +
                 (door_open_ctrl.epos[2] - door_open_ctrl.spos[2]) * door_open_ctrl.count / door_open_ctrl.move_flame;

            plyr_wrk.move_box.rot[1] = door_open_ctrl.srot + door_open_ctrl.door_pre +
                 (door_open_ctrl.erot - door_open_ctrl.srot) * door_open_ctrl.count / door_open_ctrl.move_flame;

            RotLimitChk(&plyr_wrk.move_box.rot[1]);
        }

        if (RoomMdlLoadWait() != 0)
        {
            door_open_ctrl.mode = DCOM_WAIT_OPEN;

            load_id = -1;
            load_id = SDoorLoadReqAndSet(-1, door_wrk[dsmtnp->dojs[0].dwrk_no].door_id);
            load_id = SFootLoadReqAndSet(load_id, door_wrk[dsmtnp->dojs[0].dwrk_no].door_id);
            load_id = SSrundLoadReqAndSet(load_id, door_wrk[dsmtnp->dojs[0].dwrk_no].door_id);
            load_id = SEventLoadReqAndSet(load_id, door_wrk[dsmtnp->dojs[0].dwrk_no].door_id);

            if (load_id != -1)
            {
                req_flg = 1;
            }
            else
            {
                req_flg = 0;

                FGTransInit();
            }

        }
        else
        {
            for (i = 0; i < 2; i++)
            {
                if (dsmtnp->dojs[i].dwrk_no == 0xffffffff)
                {
                    continue;
                }

                if (door_open_ctrl.open[i] == 1)
                {
                    ani_mdl[0].mot.reso = 0;
                }
            }

            return;
        }
    case DCOM_WAIT_OPEN:
        if (req_flg == 1)
        {
            if (IsLoadEnd(load_id) != 0)
            {
                req_flg = 0;

                FGTransInit();
            }
        }
        else if (req_flg == 0)
        {
            SeFGhostTransCtrl();
        }

        if (door_open_ctrl.wait_flame <= door_open_ctrl.count)
        {
            dmotionp = GetDoorMtnDat(door_open_ctrl.mtn_dat_no);

            for (i = 0; i < 2; i++)
            {
                if (dsmtnp->dojs[i].dwrk_no == 0xffffffff)
                {
                    continue;
                }

                if (door_open_ctrl.open[i] == 1)
                {
                    if (ani_mdl[0].mot.reso == 0)
                    {
                        ani_mdl[0].mot.reso = 1;
                    }

                    plyr_wrk.sta |= 0x2000;

                    if (dsmtnp->exc_flg != 0 && dsmtnp->dbl_flg == 0)
                    {
                        if (i == 0)
                        {
                            door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no = dmotionp->dmd_no1;

                        }
                        else
                        {
                            door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no = dmotionp->dmd_no2;

                        }
                    }
                    else
                    {
                        if (dsmtnp->exc_flg != i && MotionIsDouble(door_open_ctrl.mtn_dat_no) != 0)
                        {
                            door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no = dmotionp->dmd_no2;
                        }
                        else
                        {
                            door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no = dmotionp->dmd_no1;
                        }
                    }

                    for (ncp = nc_door_wrk, j = 0; j < 50; j++, ncp++)
                    {
                        if (ncp->door_id == door_wrk[dsmtnp->dojs[i].dwrk_no].door_id && ncp->stts != 0)
                        {
                            ncp->type = door_wrk[dsmtnp->dojs[i].dwrk_no].type;
                            ncp->npos[0] = 0.0f;
                            ncp->npos[1] = 0.0f;
                            ncp->npos[2] = 0.0f;
                            ncp->nrot = 0.0f;
                            ncp->dw_no = dsmtnp->dojs[i].dwrk_no;
                            ncp->stts = 0x2;
                            ncp->opnmtn_flg = 0x0;
                            ncp->dmd_no = door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no;

                            door_wrk[dsmtnp->dojs[i].dwrk_no].stts = DOOR_STTS_OPENMTN;

                            ncp->open_tbl = dtbl_tbl[door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no];

                            SeDoorOpen(dsmtnp->dojs[i].dwrk_no);

                            break;
                        }
                    }

                    if (j >= 50)
                    {
                        for (acp = ac_door_wrk, j = 0; j < 10; j++, acp++)
                        {
                            if (acp->stts == 0x0)
                            {
                                acp->door_id = door_wrk[dsmtnp->dojs[i].dwrk_no].door_id;
                                acp->type = door_wrk[dsmtnp->dojs[i].dwrk_no].type;
                                acp->npos[0] = 0.0f;
                                acp->npos[1] = 0.0f;
                                acp->npos[2] = 0.0f;
                                acp->nrot = 0.0f;
                                acp->dw_no = dsmtnp->dojs[i].dwrk_no;
                                acp->stts = 0x2;
                                acp->opnmtn_flg = 0;
                                acp->dmd_no = door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no;

                                door_wrk[dsmtnp->dojs[i].dwrk_no].stts = DOOR_STTS_OPENMTN;

                                acp->open_tbl = dtbl_tbl[door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no];

                                SeDoorOpen(dsmtnp->dojs[i].dwrk_no);

                                break;
                            }
                        }
                    }

                    door_open_ctrl.open[i] = 2;
                }
            }
        }

        if (door_open_ctrl.count < 30000)
        {
            door_open_ctrl.count++;
        }

        if (plyr_wrk.sta & 0x20 && req_flg == 0)
        {
            SetPlyrPosAfterDOpen();

            dmotionp = GetDoorMtnDat(door_open_ctrl.mtn_dat_no);

            SetPlyrAnime(dmotionp->anm_last_no, 0);

            door_open_ctrl.mode = DOCM_MOTION_ADD;

            if (dsmtnp->mtn_no == 17 || dsmtnp->mtn_no == 18)
            {
                plyr_wrk.pr_info.camera_door = -1;

                DoorSttsChange(door_wrk[dsmtnp->dojs[0].dwrk_no].door_id, DOOR_STTS_CLOSE);
            }
        }

    break;
    case DOCM_MOTION_ADD:
        if (plyr_wrk.sta & 0x20 && IsEndFgTrans() != 0)
        {
            if (dsmtnp->mtn_no == 16)
            {
                plyr_wrk.pr_info.camera_door = 0xffff;

                DoorSttsChange(door_wrk[dsmtnp->dojs[0].dwrk_no].door_id, DOOR_STTS_CLOSE);

                if (ingame_wrk.msn_no == 0x2)
                {
                    tmp_rot_door_id = door_wrk[dsmtnp->dojs[0].dwrk_no].door_id;

                    if (tmp_rot_door_id == 26 || tmp_rot_door_id == 47 || tmp_rot_door_id == 34 || tmp_rot_door_id == 49)
                    {
                        DoorKeyLockOnly(tmp_rot_door_id);
                    }

                    else if (tmp_rot_door_id == 29)
                    {
                        if (plyr_wrk.pr_info.room_no == 4)
                        {
                            DoorKeyLockOnly(tmp_rot_door_id);
                        }
                    }
                }

                if (IsRotType(door_wrk[dsmtnp->dojs[0].dwrk_no].type) != 0)
                {
                    ChoudoPreRenderDual(furn_wrk + door_wrk[dsmtnp->dojs[0].dwrk_no].fwrk_no);
                }
            }

            door_open_ctrl.mode = DOCM_MOTION_ADD2;
        }
        else
        {
            SeFGhostTransCtrl();
        }
    break;
    case DOCM_MOTION_ADD2:
        door_open_ctrl.mode = DOCM_MOTION_ADD3;
    break;
    case DOCM_MOTION_ADD3:
        plyr_wrk.sta &= ~0x8;
        plyr_wrk.sta &= ~0x2000;
        door_open_ctrl.stts ^= 1;
    break;
    }
}

void SetPlyrPosAfterDOpen()
{
    PLYR_WRK *pwp = &plyr_wrk;
    MOVE_BOX *mbp = &plyr_wrk.move_box;

    mbp->pos[0] = pwp->bwp[0];
    mbp->pos[2] = pwp->bwp[2];
    mbp->pos[1] = GetPointHeight(pwp->bwp[0], pwp->bwp[2]);
}

static void SetNcDwrk(int nc_dwrk_pos, int dwrk_pos, u_char stts)
{
    NON_CLOSE_DOOR_WRK *ncp;
    DOOR_WRK *dwp;
    DOOR_TYPE_DAT *dtdp;

    ncp = &nc_door_wrk[nc_dwrk_pos];
    dwp = &door_wrk[dwrk_pos];

    ncp->type = dwp->type;
    ncp->dw_no = dwrk_pos;
    ncp->dmd_no = dwp->dmd_no;

    dwp->stts = stts;
    ncp->stts = stts;

    dtdp = GetDoorTypeDatP(ncp->type);

    switch(stts)
    {
    case DOOR_STTS_CLOSE:
    case DOOR_STTS_OPENMTN:
        // do nothing
    break;
    case DOOR_STTS_OPENING:
    case DOOR_STTS_OPEN:
    case DOOR_STTS_OPEN_FORCE:
        ncp->count = 0;
    break;
    case DOOR_STTS_CLOSING:
        ncp->count = dtdp->open_wait;
    break;
    }
}

static void SetAcDwrk(int ac_dwrk_pos, int dwrk_pos, u_char stts)
{
    AUTO_CLOSE_DOOR_WRK *acp;
    DOOR_WRK *dwp;
    DOOR_TYPE_DAT *dtdp;

    acp = &ac_door_wrk[ac_dwrk_pos];
    dwp = &door_wrk[dwrk_pos];

    acp->door_id = dwp->door_id;
    acp->type = dwp->type;
    acp->dw_no = dwrk_pos;
    acp->dmd_no = dwp->dmd_no;

    dwp->stts = stts;
    acp->stts = stts;

    dtdp = GetDoorTypeDatP(acp->type);

    switch(stts)
    {
    case DOOR_STTS_CLOSE:
    case DOOR_STTS_OPENMTN:
        // do nothing
    break;
    case DOOR_STTS_OPENING:
    case DOOR_STTS_OPEN:
    case DOOR_STTS_OPEN_FORCE:
        acp->count = 0;
    break;
    case DOOR_STTS_CLOSING:
        acp->count = dtdp->open_wait;
    break;
    }
}

static void DoorCtrlAutoClose()
{
    DOOR_TYPE_DAT *dtdp;
    DOOR_WRK *dwp;
    AUTO_CLOSE_DOOR_WRK *acp;
    int i;
    int j;
    int k;
    u_short del_id;

    for (i = 0, acp = ac_door_wrk; i < 10; i++, acp++)
    {
        if (acp->stts == DOOR_STTS_NO_USE)
        {
            continue;
        }

        del_id = 0xffff;

        dtdp = GetDoorTypeDatP(acp->type);

        switch(acp->stts)
        {
        case DOOR_STTS_INERT_CLS:
            if (IsFusumaType(acp->type) != 0)
            {
                acp->npos[0] += acp->inert;

                if (acp->npos[0] < 0.0f)
                {
                    acp->npos[0] = 0.0f;
                    acp->count = 0;
                    acp->stts = DOOR_STTS_CLOSE;
                    acp->inert = 0.0f;
                }
            }
            else
            {
                acp->nrot += acp->inert;

                if (acp->nrot < 0.0f)
                {
                    acp->nrot = 0.0f;
                    acp->count = 0;
                    acp->stts = DOOR_STTS_CLOSE;
                    acp->inert = 0.0f;
                }
            }
        break;
        case DOOR_STTS_INERT_OPN:
            if (IsFusumaType(acp->type) != 0)
            {
                acp->npos[0] += acp->inert;

                if (
                    (dtdp->move_max >= 0 && acp->npos[0] > dtdp->move_max) ||
                    (dtdp->move_max < 0 && acp->npos[0] < dtdp->move_max)
                )
                {
                    acp->count = 0;
                    acp->stts = DOOR_STTS_OPEN;
                    acp->inert = 0.0f;
                }

            }
            else if (IsRotType(acp->type) != 0)
            {
                acp->nrot += acp->inert;

                if (
                    (dtdp->move_max >= 0 && DEG2RAD(dtdp->move_max) < acp->nrot) ||
                    (dtdp->move_max < 0 && DEG2RAD(dtdp->move_max) > acp->nrot)
                )
                {
                    acp->count = 0;
                    acp->stts = DOOR_STTS_CLOSE;
                    acp->inert = 0.0f;
                }
            }
            else
            {
                acp->nrot += acp->inert;

                if (
                    (dtdp->move_max >= 0 && acp->nrot > DEG2RAD(dtdp->move_max)) ||
                    (dtdp->move_max < 0 && acp->nrot < DEG2RAD(dtdp->move_max))
                )
                {
                    acp->count = 0;
                    acp->stts = DOOR_STTS_OPEN;
                    acp->inert = 0.0f;
                }
            }
        break;
        case DOOR_STTS_OPENMTN:
            if (acp->opnmtn_flg == 0)
            {
                if (acp->open_tbl[1] == 10000.0f)
                {
                    if (IsFusumaType(acp->type) != 0)
                    {
                        acp->npos[0] = acp->open_tbl[0] + (acp->open_tbl[0] - acp->open_tbl[-1]) * 0.5f;
                    }
                    else
                    {
                        acp->nrot = acp->open_tbl[0] + (acp->open_tbl[0] - acp->open_tbl[-1]) * 0.5f;
                    }
                }
                else
                {
                    if (IsFusumaType(acp->type) != 0)
                    {
                        acp->npos[0] = (acp->open_tbl[0] + acp->open_tbl[1]) * 0.5f;
                    }
                    else {
                        acp->nrot = (acp->open_tbl[0] + acp->open_tbl[1]) * 0.5f;
                    }
                }

                acp->opnmtn_flg = 1;
                acp->open_tbl++;
            }
            else
            {
                acp->opnmtn_flg = 0;

                if (IsFusumaType(acp->type) != 0)
                {
                    acp->npos[0] = acp->open_tbl[0];
                }
                else
                {
                    acp->nrot = acp->open_tbl[0];
                }
            }

            if (acp->open_tbl[0] == 10000.0f)
            {
                acp->count = 0;
                acp->stts = DOOR_STTS_INERT_OPN;
                acp->inert = (acp->open_tbl[-1] - acp->open_tbl[-2]) * 0.5f;
            }
        break;
        case DOOR_STTS_OPENING:
            if (IsFusumaType(acp->type) != 0)
            {
                acp->npos[0] = DoorOpenCtrlSlide(acp->count, dtdp->move_max, dtdp->open_wait);
            }
            else
            {
                acp->nrot = DoorOpenCtrlRot(acp->count, dtdp->move_max, dtdp->open_wait);
            }

            acp->count++;

            if (acp->count > dtdp->open_wait)
            {
                acp->count = 0;
                acp->stts = DOOR_STTS_OPEN;
            }
        break;
        case DOOR_STTS_OPEN:
            if (IsFusumaType(acp->type) != 0)
            {
                acp->npos[0] = dtdp->move_max;
            }
            else
            {
                acp->nrot = DEG2RAD(dtdp->move_max);
            }

            acp->count++;

            if (acp->count > dtdp->close_wait)
            {
                acp->count = dtdp->open_wait;
                acp->stts = DOOR_STTS_CLOSING;

            }
        break;
        case DOOR_STTS_OPEN_FORCE:
            if (IsFusumaType(acp->type) != 0)
            {
                acp->npos[0] = dtdp->move_max;
            }
            else
            {
                acp->nrot = DEG2RAD(dtdp->move_max);
            }
            DoorSttsChange(acp->door_id, acp->stts);
        break;
        case DOOR_STTS_CLOSING:
            if (IsFusumaType(acp->type) != 0)
            {
                acp->npos[0] = DoorOpenCtrlSlide(acp->count, dtdp->move_max, dtdp->open_wait);
            }
            else
            {
                acp->nrot = DoorOpenCtrlRot(acp->count, dtdp->move_max, dtdp->open_wait);
            }

            acp->count--;

            if (acp->count == 0)
            {
                acp->stts = DOOR_STTS_CLOSE;
            }
        break;
        case DOOR_STTS_CLOSE:
            if (IsFusumaType(acp->type) != 0)
            {
                acp->npos[0] = DoorOpenCtrlSlide(0, dtdp->move_max, dtdp->open_wait);
            }
            else if (IsRotType(acp->type) != 0)
            {
                acp->count = 0;
                acp->inert = 0.0f;
                acp->nrot = DoorOpenCtrlRot(acp->inert, dtdp->move_max, dtdp->open_wait);

                ChangeRotDoorFace(acp->door_id);
            }
            else
            {
                acp->nrot = DoorOpenCtrlRot(0.0f, dtdp->move_max, dtdp->open_wait);
            }

            DoorSttsChange(acp->door_id, acp->stts);

            if (acp->door_id < 0xffff)
            {
                del_id = acp->door_id;
            }
        break;
        case DOOR_STTS_MANUAL_OPEN:
            if (acp->opnmtn_flg == 0)
            {
                acp->opnmtn_flg = 1;
            }
            else
            {
                acp->opnmtn_flg = 0;

                if (IsFusumaType(acp->type) != 0)
                {
                    acp->npos[0] = acp->open_tbl[0];
                }
                else
                {
                    acp->nrot = acp->open_tbl[0];
                }

                acp->open_tbl++;
            }

            if (acp->open_tbl[0] == 10000.0f)
            {
                acp->stts = DOOR_STTS_CLOSE;

                DoorSttsChange(acp->door_id, acp->stts);

                if (acp->door_id < 0xffff)
                {
                    del_id = acp->door_id;
                }
            }
        break;
        }

        for (j = 0, dwp = door_wrk; j < 20; j++, dwp++)
        {
            if (dwp->door_id == acp->door_id && dwp->stts != DOOR_STTS_NO_USE)
            {
                if (IsFusumaType(acp->type) != 0)
                {
                    dwp->npos[0] = dwp->pos[0] + acp->npos[0] * SgCosf(dwp->rot);
                    dwp->npos[2] = dwp->pos[2] - acp->npos[0] * SgSinf(dwp->rot);

                    furn_wrk[dwp->fwrk_no].pos[0] = dwp->npos[0];
                    furn_wrk[dwp->fwrk_no].pos[2] = dwp->npos[2];
                }
                else
                {
                    dwp->nrot = dwp->rot + acp->nrot;

                    furn_wrk[dwp->fwrk_no].rotate[0] = 0.0f;
                    furn_wrk[dwp->fwrk_no].rot = dwp->nrot + PI;

                    RotLimitChk(&furn_wrk[dwp->fwrk_no].rot);

                    furn_wrk[dwp->fwrk_no].rotate[1] = furn_wrk[dwp->fwrk_no].rot;
                    furn_wrk[dwp->fwrk_no].rotate[2] = 0.0f;
                }

                if (acp->stts == DOOR_STTS_OPEN)
                {
                    if (DoorReOpenCheck(plyr_wrk.move_box.pos, j, acp->count, acp->type) != 0)
                    {
                        acp->count = 0;

                        if (dtdp->dbl_did != 0xffff)
                        {
                            for (k = 0; k < 10; k++)
                            {
                                if (ac_door_wrk[k].door_id == dtdp->dbl_did)
                                {
                                    ac_door_wrk[k].count = 0;
                                    ac_door_wrk[k].stts = DOOR_STTS_OPEN;

                                    door_wrk[ac_door_wrk[k].dw_no].stts = DOOR_STTS_OPEN;

                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        acp->count = dtdp->close_wait + 1;
                    }
                }

                dwp->stts = acp->stts;
            }

            if (del_id != 0xffff && del_id == dwp->door_id && dwp->stts != DOOR_STTS_NO_USE)
            {
                SeDoorClose(j);

                dwp->stts = DOOR_STTS_CLOSE;

                acp->stts = DOOR_STTS_NO_USE;
                acp->door_id = 0xffff;
                acp->type = 0;
                acp->dw_no = 0xffff;
                acp->nrot = 0.0f;
                acp->npos[0] = 0.0f;

                del_id = 0xffff;
            }
        }
    }
}

void AllCloseAcDoor()
{
    AUTO_CLOSE_DOOR_WRK *acp;
    int i;

    for (i = 0; i < 10; i++)
    {
        if (ac_door_wrk[i].stts != 0)
        {
            ac_door_wrk[i].stts = 1;
        }
    }
}

void AllCloseConnectDoor()
{
    DOOR_WRK *dwp;
    int i;
    u_char now_rid;
    u_char room_no;

    now_rid = GetRoomIdFromRoomNo(0, room_wrk.room_no);

    dwp = door_wrk;

    for (i = 0; i < 20; i++)
    {
        if (dwp->use == 1 || dwp->use == 3 || dwp->use == 4)
        {
            if (dwp->stts != 1 && dwp->stts != 5)
            {
                room_no = GetRoomIdBeyondDoor(dwp->door_id, now_rid);

                if (room_no != 0xff && room_no != now_rid && IsRotType(dwp->type) == 0)
                {
                    DoorSttsChange(dwp->door_id, 1);
                }
            }
        }

        dwp++;
    }
}

static int DoorReOpenCheck(sceVu0FVECTOR pos, int dwrk_no, u_short count, u_char type)
{
    DOOR_WRK *dwp;
    sceVu0FVECTOR tmp_dpos;
    float x;
    float z;

    dwp = &door_wrk[dwrk_no];

    GetDoorTypeDatP(type);

    tmp_dpos[0] = dwp->pos[0] + SgCosf(dwp->rot) * 225.0f;
    tmp_dpos[2] = dwp->pos[2] - SgSinf(dwp->rot) * 225.0f;

    x = pos[0] - tmp_dpos[0];
    z = pos[2] - tmp_dpos[2];

    if (GetDist(z, x) < 900.0f)
    {
        return 1;
    }

    return 0;
}

static void DoorCtrlNoClose()
{
    DOOR_TYPE_DAT *dtdp;
    DOOR_WRK *dwp;
    NON_CLOSE_DOOR_WRK *ncp;
    int i;
    u_short del_id;

    GetDoorTypeDatP(0);

    ncp = nc_door_wrk;

    for (i = 0; i < 50; i++, ncp++)
    {
        if (ncp->stts == DOOR_STTS_NO_USE)
        {
            continue;
        }

        del_id = 0xffff;

        switch(ncp->stts)
        {
        case DOOR_STTS_INERT_CLS:
            if (IsFusumaType(ncp->type) != 0)
            {
                ncp->npos[0] += ncp->inert;

                if (ncp->npos[0] < 0.0f)
                {
                    ncp->npos[0] = 0.0f;
                    ncp->stts = DOOR_STTS_CLOSE;
                    ncp->inert = 0.0f;
                    ncp->count = 0;
                }
            }
            else
            {
                ncp->nrot += ncp->inert;

                if (ncp->nrot < 0.0f)
                {
                    ncp->nrot = 0.0f;
                    ncp->stts = DOOR_STTS_CLOSE;
                    ncp->inert = 0.0f;
                    ncp->count = 0;
                }
            }
        break;
        case DOOR_STTS_INERT_OPN:
            dtdp = GetDoorTypeDatP(ncp->type);

            if (IsFusumaType(ncp->type) != 0)
            {
                ncp->npos[0] += ncp->inert;

                if (
                    (dtdp->move_max >= 0 && ncp->npos[0] > dtdp->move_max) ||
                    (dtdp->move_max < 0 && ncp->npos[0] < dtdp->move_max)
                )
                {
                    ncp->count = 0;
                    ncp->stts = DOOR_STTS_OPEN_FORCE;
                    ncp->inert = 0.0f;
                }
            }
            else
            {
                ncp->nrot += ncp->inert;

                if (
                    (dtdp->move_max >= 0 && ncp->nrot > DEG2RAD(dtdp->move_max)) ||
                    (dtdp->move_max < 0 && DEG2RAD(dtdp->move_max) > ncp->nrot)
                )
                {
                    ncp->count = 0;
                    ncp->stts = DOOR_STTS_OPEN_FORCE;
                    ncp->inert = 0.0f;
                }
            }
        break;
        case DOOR_STTS_OPENMTN:
            if (ncp->opnmtn_flg == 0)
            {
                ncp->opnmtn_flg = 1;
            }
            else
            {
                if (IsFusumaType(ncp->type) != 0)
                {
                    ncp->npos[0] = ncp->open_tbl[0];
                }
                else
                {
                    ncp->nrot = ncp->open_tbl[0];
                }

                ncp->opnmtn_flg = 0;
                ncp->open_tbl++;
            }

            if (ncp->open_tbl[0] == 10000.0f)
            {
                ncp->stts = DOOR_STTS_INERT_OPN;
                ncp->count = 0;
                ncp->inert = (ncp->open_tbl[-1] - ncp->open_tbl[-2]) * 0.5f;
            }
        break;
        case DOOR_STTS_OPEN_FORCE:
            dtdp = GetDoorTypeDatP(ncp->type);

            if (IsFusumaType(ncp->type) != 0)
            {
                ncp->npos[0] = dtdp->move_max;
            }
            else
            {
                ncp->nrot = DEG2RAD(dtdp->move_max);
            }

            DoorSttsChange(ncp->door_id,ncp->stts);
        break;
        case DOOR_STTS_OPEN:
            dtdp = GetDoorTypeDatP(ncp->type);

            if (IsFusumaType(ncp->type) != 0)
            {
                ncp->npos[0] = dtdp->move_max;
            }
            else
            {
                ncp->nrot = DEG2RAD(dtdp->move_max);
            }

            ncp->count++;

            if (ncp->count > dtdp->close_wait)
            {
                ncp->count = dtdp->open_wait;
                ncp->stts = DOOR_STTS_CLOSING;
            }
        break;
        case DOOR_STTS_CLOSING:
            dtdp = GetDoorTypeDatP(ncp->type);

            if (IsFusumaType(ncp->type) != 0)
            {
                ncp->npos[0] = DoorOpenCtrlSlide(ncp->count,dtdp->move_max,dtdp->open_wait);
            }
            else
            {
                ncp->nrot = DoorOpenCtrlRot(ncp->count,dtdp->move_max, dtdp->open_wait);
            }

            ncp->count--;

            if (ncp->count == 0)
            {
                ncp->stts = DOOR_STTS_CLOSE;
            }
        break;
        case DOOR_STTS_OPENING:
            dtdp = GetDoorTypeDatP(ncp->type);

            if (IsFusumaType(ncp->type) != 0)
            {
                ncp->npos[0] = DoorOpenCtrlSlide(ncp->count,dtdp->move_max,dtdp->open_wait);
            }
            else
            {
                ncp->nrot = DoorOpenCtrlRot(ncp->count,dtdp->move_max, dtdp->open_wait);
            }

            ncp->count++;

            if (ncp->count > dtdp->open_wait)
            {
                ncp->count = 0;
                ncp->stts = DOOR_STTS_OPEN;
            }
        break;
        case DOOR_STTS_CLOSE:
            if (IsFusumaType(ncp->type) != 0)
            {
                ncp->npos[0] = 0.0f;
            }
            else
            {
                ncp->nrot = 0.0f;
            }
        break;
        }

        for (i = 0, dwp = door_wrk; i < 20; i++, dwp++)
        {
            if (dwp->door_id == ncp->door_id && dwp->stts != DOOR_STTS_NO_USE)
            {
                if (IsFusumaType(ncp->type) != 0)
                {
                    dwp->npos[0] = dwp->pos[0] + ncp->npos[0] * SgCosf(dwp->rot);
                    dwp->npos[2] = dwp->pos[2] - ncp->npos[0] * SgSinf(dwp->rot);

                    furn_wrk[dwp->fwrk_no].pos[0] = dwp->npos[0];
                    furn_wrk[dwp->fwrk_no].pos[2] = dwp->npos[2];
                }
                else
                {
                    dwp->nrot = dwp->rot + ncp->nrot;

                    furn_wrk[dwp->fwrk_no].rotate[0] = 0.0f;
                    furn_wrk[dwp->fwrk_no].rot = dwp->nrot + PI;

                    RotLimitChk(&furn_wrk[dwp->fwrk_no].rot);

                    furn_wrk[dwp->fwrk_no].rotate[1] = furn_wrk[dwp->fwrk_no].rot;
                    furn_wrk[dwp->fwrk_no].rotate[2] = 0.0f;
                }

                dwp->stts = ncp->stts;
            }

            if (del_id != 0xffff && del_id == dwp->door_id && dwp->stts != DOOR_STTS_NO_USE)
            {
                dwp->stts = DOOR_STTS_CLOSE;

                del_id = 0xffff;
            }
        }
    }
}

static float DoorOpenCtrlRot(float count, float move_max, float open_wait)
{
    return (count * move_max * PI) / (open_wait * 180.0f);
}

static short int DoorOpenCtrlSlide(u_short count, short int move_max, u_short open_wait)
{
    return (count * move_max) / open_wait;
}

void DoorOpenCheckMain()
{
    int temp; // not from STABS

    if (door_open_ctrl.chk_flg != 0)
    {
        DoorOpenJudge();
        DoorCheckOff();

        temp = 0; // HACK: fixes jal vs j
    }
}

void DoorCheckOn(u_char mode)
{
    if (ev_wrk.btl_lock != 0)
    {
        if (LockBattleAfterJudge() != 0)
        {
            if (DoorOpenCheck(2) != 0)
            {
                LockBattleDoorOpen();

                return;
            }
        }
        else
        {
            if (DoorOpenCheck(0) != 0)
            {
                LockBattleDoorOpen();

                return;
            }
        }
    }

    if (mode != 0)
    {
        plyr_wrk.sta |= 0x8;
    }

    door_open_ctrl.chk_flg = 1;

    asm(""); // HACK: fixes instruction ordering (maybe some optimized out debug code?)
}

void DoorCheckOff()
{
    door_open_ctrl.chk_flg = 0;
}

static void SetDoorOpenPlyrPos()
{
    plyr_wrk.dop.door_id = door_wrk[doj_square_mtn.dojs->dwrk_no].door_id;
}

u_char DoorOpenJudge()
{
    u_short door_id[2];
    u_char sq_no;

    plyr_wrk.sta &= ~0x8;

    if (ev_wrk.mode != 0 || DoorOpenCheck(1) == 0)
    {
        return 0;
    }

    AreaReadCheck();
    InitDoorOpenCtrl();

    plyr_wrk.sta |= 0x8;
    door_open_ctrl.stts |= 0x1;

    SetDoorOpenPlyrPos();
    AllCloseConnectDoor();
    DoorCheckOff();

    if (ev_wrk.btl_lock != 0)
    {
        ev_wrk.btl_lock = 0;
    }

    return sq_no;
}

void SetDoorOpenCameraNo()
{
    int i;
    u_short door_id[2];
    u_short camera_door;

    for (i = 0; i < 2; i++)
    {
        if (doj_square_mtn.dojs[i].dwrk_no == 0xffffffff)
        {
            door_id[i] = -1;
        }
        else
        {
            door_id[i] = door_wrk[doj_square_mtn.dojs[i].dwrk_no].door_id;
        }
    }

    camera_door = CameraGetDoorCameraNo(door_id[0],door_id[1]);

    if (camera_door != 0xffff)
    {
        plyr_wrk.pr_info.camera_door = camera_door;
        plyr_wrk.pr_info.camera_door_rid = plyr_wrk.pr_info.room_no;

        if (door_id[0] != 0xffff)
        {
            plyr_wrk.pr_info.camera_door_did = door_id[0];
        }
        else
        {
            plyr_wrk.pr_info.camera_door_did = door_id[1];
        }
    }
}

int AreaReadCheck()
{
    u_char *addr_uc;
    int i;
    int dwrk_no;
    u_char type;
    u_char dr_num;

    area_read_wrk.rm_from = GetRoomIdFromRoomNo(0, room_wrk.room_no);

    if (doj_square_mtn.dojs[0].type != 0xff)
    {
        type = doj_square_mtn.dojs[0].type;
        dwrk_no = doj_square_mtn.dojs[0].dwrk_no;
    }
    else if (doj_square_mtn.dojs[1].type != 0xff)
    {
        type = doj_square_mtn.dojs[1].type;
        dwrk_no = doj_square_mtn.dojs[1].dwrk_no;
    }

    addr_uc = GetDoorRoomConectDataP(area_read_wrk.rm_from);

    dr_num = *addr_uc;

    addr_uc += 2;

    for (i = 0; i < dr_num; i++)
    {
        if (*(u_short *)addr_uc == door_wrk[dwrk_no].door_id)
        {
            area_read_wrk.rm_to = addr_uc[2];

            break;
        }

        addr_uc += 4;
    }

    if (IsDoorLoad(type) != 0)
    {
        area_read_wrk.stat = 1;

        ingame_wrk.mode = INGAME_MODE_AREA_MOVE;

        return 1;
    }

    return 0;
}

static u_char DoorOpenCheck(u_char chk_stat)
{
    sceVu0FVECTOR dbase1;
    sceVu0FVECTOR dbase2;
    DOJ_SQUARE tmp;
    DOJ_SQUARE_MTN *dsmp;
    DOOR_TYPE_DAT *dtdp;
    int dwn[2];
    int exc_flg;
    float dbrot1;
    float dbrot2;
    float p_rot;
    u_char is_door;
    u_char is_dbl;
    u_char is_kuguri;
    int i;
    int j;
    u_char ret_flg;

    dsmp = &doj_square_mtn;

    is_dbl = 0;

    DoorInitDSM(dsmp);

    if (chk_stat == 1)
    {
        DoorOpenCheckSquareArea(dsmp, 2, 0);
    }
    else if (chk_stat == 0)
    {
        DoorOpenCheckSquareArea(dsmp, 3, 0);
    }
    else if (chk_stat == 2)
    {
        ret_flg = 0;

        DoorOpenCheckSquareArea(dsmp, 0, 0);

        for (i = 0; i < 2; i++)
        {
            if (dsmp->dojs[i].type != 0xff)
            {
                for (j = 0; j < 20; j++)
                {
                    if (door_wrk[dsmp->dojs[i].dwrk_no].use == 5)
                    {
                        continue;
                    }
                    if (door_wrk[dsmp->dojs[i].dwrk_no].door_id == lock_did[j])
                    {
                        ret_flg = 1;

                        break;
                    }
                }
            }
        }

        if (!ret_flg)
        {
            return 0;
        }

        { u_char c = 0; do { } while (c); } // HACK: forces correct block merge of chk_stat == 1 and chk_stat == 0
    }

    if (dsmp->dojs[0].type == 0xff)
    {
        return 0;
    }

    dtdp = GetDoorTypeDatP(dsmp->dojs[0].type);

    switch (CheckRotDoorOpenStatus(dsmp->dojs[0].dwrk_no, dsmp->dojs[0].type))
    {
    case 0:
        return 0;
    break;
    case 1:
        if (JudgePlyrRotForRotDoor(dsmp->dojs[0].dwrk_no) != 0)
        {
            dsmp->exc_flg = 0;
            dsmp->dbl_flg = 0;

            dsmp->dwno_base = dsmp->dojs[0].dwrk_no;

            dsmp->dor_flg = 0;
            dsmp->mtn_no = 16;

            return 1;
        }

        return 0;
    break;
    case 0xffffffff:
        // negative case
    break;
    }

    if (dtdp->attribute & 0x400)
    {
        is_kuguri = 1;
    }
    else
    {
        is_kuguri = 0;
    }

    if ((dtdp->attribute & 0x1000) == 0)
    {
        is_door = 1;
    }
    else
    {
        is_door = 0;
    }

    if (
        (dsmp->dojs[0].sq_no[0] == 4 || dsmp->dojs[0].sq_no[0] == 8 || dsmp->dojs[0].sq_no[1] == 4 || dsmp->dojs[0].sq_no[1] == 8) &&
        (dsmp->dojs[1].sq_no[0] == 4 || dsmp->dojs[1].sq_no[0] == 8 || dsmp->dojs[1].sq_no[1] == 4 || dsmp->dojs[1].sq_no[1] == 8)
    )
    {
        if (GetDoorTypeDatP(dsmp->dojs[1].type)->attribute & 0x100)
        {
            is_dbl = ((dtdp->attribute & 0x100) >> 8) & 1;
        }
    }

    exc_flg = GetDoorBaseRot(dsmp, &dbrot1, &dbrot2, is_dbl, is_door);

    p_rot = plyr_wrk.move_box.rot[1];

    while (p_rot < 0.0f)
    {
        p_rot += PI * 2.0; // 2.0 is double!
    }

    if (dbrot2 >= dbrot1)
    {
        if (!(dbrot2 - p_rot >= 0.0f && p_rot - dbrot1 >= 0.0f))
        {
            return 0;
        }
    }
    else if (!(dbrot2 > p_rot) && !(p_rot > dbrot1))
    {
        return 0;
    }

    dsmp->exc_flg = exc_flg;
    dsmp->dbl_flg = is_dbl;
    dsmp->dor_flg = is_door;

    if (is_dbl != 0)
    {
        dtdp = GetDoorTypeDatP(dsmp->dojs[exc_flg].type);

        dsmp->dwno_base = dsmp->dojs[exc_flg].dwrk_no;

        if (is_door != 0)
        {
            if (dtdp->move_max >= 1)
            {
                if (dsmp->dojs[0].sq_no[0] == 1 || dsmp->dojs[1].sq_no[0] == 1)
                {
                    dsmp->mtn_no = 8;
                }
                else if (dsmp->dojs[0].sq_no[0] == 3 || dsmp->dojs[1].sq_no[0] == 3)
                {
                    dsmp->mtn_no = 9;
                }
            }
            else
            {
                dsmp->mtn_no = 10;
            }
        }
        else
        {
            if (exc_flg == 0)
            {
                dsmp->dwno_base = dsmp->dojs[1].dwrk_no;
            }
            else
            {
                dsmp->dwno_base = dsmp->dojs[0].dwrk_no;
            }

            dsmp->mtn_no = 15;
        }

        return 1;
    }

    dsmp->dwno_base = dsmp->dojs[0].dwrk_no;

    dtdp = GetDoorTypeDatP(door_wrk[dsmp->dojs[0].dwrk_no].type);

    if (is_door != 0)
    {
        if (dtdp->move_max >= 1)
        {
            if (is_kuguri)
            {
                if (exc_flg == 0)
                {
                    dsmp->mtn_no = 18;
                }
                else
                {
                    dsmp->mtn_no = 17;
                }
            }
            else if (exc_flg == 0)
            {
                switch(dsmp->dojs[0].sq_no[0])
                {
                case 1:
                case 5:
                    dsmp->mtn_no = 0;
                break;
                case 3:
                case 7:
                    dsmp->mtn_no = 1;
                break;
                case 2:
                case 6:
                    dsmp->mtn_no = 2;
                break;
                }
            }
            else
            {
                dsmp->mtn_no = 7;
            }
        }
        else if (is_kuguri)
        {
            if (exc_flg == 0)
            {
                dsmp->mtn_no = 17;
            }
            else
            {
                dsmp->mtn_no = 18;
            }
        }
        else if (exc_flg == 0)
        {
            dsmp->mtn_no = 3;
        }
        else
        {
            switch(dsmp->dojs[0].sq_no[0])
            {
            case 1:
            case 5:
                dsmp->mtn_no = 4;
            break;
            case 3:
            case 7:
                dsmp->mtn_no = 5;
            break;
            case 2:
            case 6:
                dsmp->mtn_no = 6;
            break;
            }
        }

        return 1;
    }

    else if (exc_flg != 0)
    {
        switch (dsmp->dojs[0].sq_no[0])
        {
            case 1:
            case 5:
                dsmp->mtn_no = 13;
            break;
            case 2:
            case 6:
                dsmp->mtn_no = 14;
            break;
        }
    }
    else
    {
        switch (dsmp->dojs[0].sq_no[0])
        {
            case 1:
            case 5:
                dsmp->mtn_no = 11;
            break;
            case 2:
            case 6:
                dsmp->mtn_no = 12;
            break;
        }
    }

    return 1;
}

static int GetDoorBaseRot(DOJ_SQUARE_MTN *dsmp, float *dbrot1, float *dbrot2, u_char is_dbl, u_char is_door)
{
    sceVu0FVECTOR dbase1;
    sceVu0FVECTOR dbase2;
    int dwn[2];
    float temp;

    dwn[0] = dsmp->dojs[0].dwrk_no;
    dwn[1] = dsmp->dojs[1].dwrk_no;

    if (is_door != 0)
    {
        if (is_dbl != 0)
        {
            dbase1[0] = door_wrk[dwn[0]].pos[0];
            dbase1[1] = door_wrk[dwn[0]].pos[1];
            dbase1[2] = door_wrk[dwn[0]].pos[2];

            dbase2[0] = door_wrk[dwn[1]].pos[0];
            dbase2[1] = door_wrk[dwn[1]].pos[1];
            dbase2[2] = door_wrk[dwn[1]].pos[2];
        }
        else
        {
            dbase1[0] = door_wrk[dwn[0]].pos[0];
            dbase1[1] = door_wrk[dwn[0]].pos[1];
            dbase1[2] = door_wrk[dwn[0]].pos[2];

            dbase2[0] = door_wrk[dwn[0]].pos[0] + SgCosf(door_wrk[dwn[0]].rot) * 450.0f;
            dbase2[1] = door_wrk[dwn[0]].pos[1];
            dbase2[2] = door_wrk[dwn[0]].pos[2] - SgSinf(door_wrk[dwn[0]].rot) * 450.0f;
        }
    }
    else
    {
        if (is_dbl != 0)
        {
            dbase1[0] = door_wrk[dwn[0]].pos[0] + SgCosf(door_wrk[dwn[0]].rot) * 450.0f;
            dbase1[1] = door_wrk[dwn[0]].pos[1];
            dbase1[2] = door_wrk[dwn[0]].pos[2] - SgSinf(door_wrk[dwn[0]].rot) * 450.0f;

            dbase2[0] = door_wrk[dwn[1]].pos[0] + SgCosf(door_wrk[dwn[1]].rot) * 450.0f;
            dbase2[1] = door_wrk[dwn[1]].pos[1];
            dbase2[2] = door_wrk[dwn[1]].pos[2] - SgSinf(door_wrk[dwn[1]].rot) * 450.0f;
        }
        else
        {
            dbase1[0] = door_wrk[dwn[0]].pos[0];
            dbase1[1] = door_wrk[dwn[0]].pos[1];
            dbase1[2] = door_wrk[dwn[0]].pos[2];

            dbase2[0] = door_wrk[dwn[0]].pos[0] + SgCosf(door_wrk[dwn[0]].rot) * 450.0f;
            dbase2[1] = door_wrk[dwn[0]].pos[1];
            dbase2[2] = door_wrk[dwn[0]].pos[2] - SgSinf(door_wrk[dwn[0]].rot) * 450.0f;
        }
    }

    area_wrk.tmp_after_pos[0] = (dbase1[0] + dbase2[0]) - plyr_wrk.move_box.pos[0];
    area_wrk.tmp_after_pos[1] = plyr_wrk.move_box.pos[1];
    area_wrk.tmp_after_pos[2] = (dbase1[2] + dbase2[2]) - plyr_wrk.move_box.pos[2];
    area_wrk.tmp_after_pos[3] = 1.0f;

    *dbrot1 = SgAtan2f(dbase1[0] - plyr_wrk.move_box.pos[0],dbase1[2] - plyr_wrk.move_box.pos[2]);
    *dbrot2 = SgAtan2f(dbase2[0] - plyr_wrk.move_box.pos[0],dbase2[2] - plyr_wrk.move_box.pos[2]);

    while (*dbrot1 < 0.0f)
    {
        *dbrot1 += PI * 2.0; // 2.0 is double!
    }

    while (*dbrot2 < 0.0f)
    {
        *dbrot2 += PI * 2.0; // 2.0 is double!
    }

    if (*dbrot2 - *dbrot1 > PI || (*dbrot1 - *dbrot2 > 0.0f && *dbrot1 - *dbrot2 <  PI))
    {
        temp = *dbrot1;
        *dbrot1 = *dbrot2;
        *dbrot2 = temp;

        return 1;
    }

    return 0;
}

static void DoorOpenCheckSquareArea(DOJ_SQUARE_MTN *dsmp, u_char stat_chk, u_char hs_chk)
{
    int *ddp;
    int *dhp;
    int *dsp;
    u_char *rdp;
    u_char *stp;
    int i;
    int j;
    int dwrk_pos;
    u_short pos_x;
    u_short pos_y;
    u_char dr_num;
    u_char ds_num;

    pos_x = plyr_wrk.move_box.pos[2];
    pos_y = plyr_wrk.move_box.pos[0];

    rdp = GetDoorRoomConectDataP(GetRoomIdFromRoomNo(0, room_wrk.room_no));

    if (rdp == NULL) {
        return;
    }

    dr_num = *rdp;

    rdp += 2;

    for (i = 0; i < dr_num; i++, rdp += 4)
    {
        ddp = GetDoorDataTopP(*(u_short *)rdp);

        if (ddp == NULL)
        {
            return;
        }

        for (j = 0; j < 20; j++)
        {
            if (door_wrk[j].door_id == *(u_short *)rdp)
            {
                dwrk_pos = j;

                break;
            }
        }

        if (j >= 20)
        {
            continue;
        }

        if (DoorOpenSttsPermissionCheck(dwrk_pos, stat_chk) == 0)
        {
            continue;
        }

        dhp = (int *)(*ddp + BASE_ADDRESS);
        dhp = (int *)(*dhp + BASE_ADDRESS);

        ds_num = ((u_char *)dhp)[16];

        stp = ((u_char *)dhp + 17);

        j = 0;

        if (hs_chk == 0)
        {
            for (; j < 1; j++)
            {
                stp++;
            }
        }

        for (; j < ds_num; j++, stp++)
        {
            dsp = (int *)(*ddp + BASE_ADDRESS);
            dsp = (int *)(dsp[j + 1] + BASE_ADDRESS);

            if (PosInAreaJudgeSub(dsp, pos_x, pos_y, *stp) != 0)
            {
                DoorSetDSM(dsmp, dwrk_pos, door_wrk[dwrk_pos].type, j);
            }
        }
    }
}

static u_char DoorOpenSttsPermissionCheck(int dwrk_pos, u_char stat_chk)
{
    switch (stat_chk)
    {
    case 0:
        return 1;
    break;
    case 1:
        if (door_wrk[dwrk_pos].lock == DOOR_LOCK_UNLOCKED)
        {
            if (door_keep[door_wrk[dwrk_pos].door_id].attr & 0x8000 && (door_keep[door_wrk[dwrk_pos].door_id].attr & 1) == 0)
            {
                return 0;
            }

            return 1;
        }
    break;
    case 2:
        if ((door_wrk[dwrk_pos].lock == DOOR_LOCK_UNLOCKED) && (door_wrk[dwrk_pos].stts == DOOR_STTS_CLOSE))
        {
            if (door_keep[door_wrk[dwrk_pos].door_id].attr & 0x8000 && (door_keep[door_wrk[dwrk_pos].door_id].attr & 1) == 0)
            {
                return 0;
            }

            return 1;
        }
    break;
    case 3:
        if (door_wrk[dwrk_pos].stts == DOOR_STTS_CLOSE)
        {
            if (door_keep[door_wrk[dwrk_pos].door_id].attr & 0x8000 && (door_keep[door_wrk[dwrk_pos].door_id].attr & 1) == 0)
            {
                return 0;
            }

            return 1;
        }
    break;
    }

    return 0;
}

static void DoorInitDSM(DOJ_SQUARE_MTN *dsmp)
{
    dsmp->dojs[0].type = 0xff;
    dsmp->dojs[0].dwrk_no = -1;
    dsmp->dojs[0].sq_no[0] = 0xff;
    dsmp->dojs[0].sq_no[1] = 0xff;

    dsmp->dojs[1].type = 0xff;
    dsmp->dojs[1].dwrk_no = -1;
    dsmp->dojs[1].sq_no[0] = 0xff;
    dsmp->dojs[1].sq_no[1] = 0xff;

    dsmp->mtn_no = 0xff;
}

static void DoorSetDSM(DOJ_SQUARE_MTN *dsmp, int dwrk_no, u_char type, u_char sq_no)
{
    if (dsmp == NULL)
    {
        return;
    }

    if (dsmp->dojs[0].type == 0xff)
    {
        dsmp->dojs[0].type = type;
        dsmp->dojs[0].dwrk_no = dwrk_no;
        dsmp->dojs[0].sq_no[0] = sq_no;

        return;
    }

    if (dsmp->dojs[0].sq_no[1] == 0xff)
    {
        if (dsmp->dojs[0].dwrk_no == dwrk_no)
        {
            dsmp->dojs[0].sq_no[1] = sq_no;

            return;
        }
    }

    if (dsmp->dojs[1].type == 0xff)
    {
        if (dsmp->dojs[0].dwrk_no != dwrk_no)
        {
            dsmp->dojs[1].type = type;
            dsmp->dojs[1].dwrk_no = dwrk_no;
            dsmp->dojs[1].sq_no[0] = sq_no;

            return;
        }
    }

    if (dsmp->dojs[1].sq_no[1] == 0xff && dsmp->dojs[1].dwrk_no == dwrk_no)
    {
        dsmp->dojs[1].sq_no[1] = sq_no;
    }
}

static u_char DoorGetOpenSquareNo(DOJ_SQUARE_MTN *dsmp)
{
    if (dsmp == NULL)
    {
        return 0;
    }

    if (dsmp->dojs[0].type == 0xff)
    {
        return 0;
    }

    if (dsmp->dojs[1].type == 0xff)
    {
        if (dsmp->dojs[0].sq_no[1] == 0xff)
        {
            return dsmp->dojs[0].sq_no[0];
        }
        else
        {
            return dsmp->dojs[0].sq_no[1];
        }
    }

    return dsmp->dojs[0].sq_no[0];
}

int DoorKeyLock(u_short door_id)
{
    if (DoorLockChange(door_id, DOOR_LOCK_LOCKED) != 0)
    {
        return DoorSttsChange(door_id, DOOR_STTS_CLOSE);
    }

    return 0;
}

int DoorKeyLockOnly(u_short door_id)
{
    return DoorLockChange(door_id, 1);
}

int DoorKeyUnlock(u_short door_id)
{
    if (DoorLockChange(door_id, DOOR_LOCK_UNLOCKED) != 0)
    {
        return DoorSttsChange(door_id, DOOR_STTS_CLOSE);
    }

    return 0;
}

int DoorKeyUnlockOnly(u_short door_id)
{
    return DoorLockChange(door_id, 0);
}

static int DoorLockChange(u_short door_id, u_char lock)
{
    int i;

    if (lock != DOOR_LOCK_UNLOCKED && lock != DOOR_LOCK_LOCKED)
    {
        return 0;
    }

    for (i = 0; i < 20; i++)
    {
        if (door_wrk[i].door_id == door_id)
        {
            door_wrk[i].lock = lock;

            break;
        }
    }

    SetDoorLockEve(door_id, lock);
    DoorPassSetSttsMap(door_id);

    return 1;
}

int DoorSttsChange(u_short door_id, u_char stts)
{
    int i;
    int dwrk_no;

    if (
        stts != DOOR_STTS_NO_USE && stts != DOOR_STTS_CLOSE &&
        stts != DOOR_STTS_OPENMTN && stts != DOOR_STTS_OPENING &&
        stts != DOOR_STTS_OPEN && stts != DOOR_STTS_OPEN_FORCE &&
        stts != DOOR_STTS_CLOSING && stts != DOOR_STTS_INERT_CLS &&
        stts != DOOR_STTS_INERT_OPN && stts != DOOR_STTS_MANUAL_OPEN)
    {
        return 0;
    }

    SetDoorSttsEve(door_id, stts);

    for (i = 0; i < 20; i++)
    {
        if (door_wrk[i].door_id == door_id)
        {
            door_wrk[i].stts = stts;

            dwrk_no = i;

            break;
        }
    }

    if (i < 20)
    {
        for (i = 0; i < 50; i++)
        {
            if (nc_door_wrk[i].door_id == door_id)
            {
                SetNcDwrk(i, dwrk_no, stts);

                return 1;
            }
        }

        for (i = 0; i < 10; i++)
        {
            if (ac_door_wrk[i].stts != 0 && ac_door_wrk[i].door_id == door_id)
            {
                SetAcDwrk(i, dwrk_no, stts);

                return 1;
            }
        }


        for (i = 0; i < 10; i++)
        {
            if (ac_door_wrk[i].stts == 0)
            {
                SetAcDwrk(i, dwrk_no, stts);

                return 1;
            }
        }

        return 0;
    }

    return 1;
}

u_char DoorHitCheck(u_char *dx_max, u_char *dz_max, float *dst, float *ppos, u_char div)
{
    sceVu0FVECTOR v0;
    sceVu0FVECTOR v1;
    sceVu0FVECTOR v2;
    sceVu0FVECTOR v3;
    sceVu0FVECTOR pos;
    DOOR_TYPE_DAT *dtdp;
    int *ddp;
    int *dhp;
    int *dsp;
    u_char *rdp;
    u_char *stp;
    int i;
    int j;
    int k;
    float mv_x;
    float mv_z;
    float sa;
    float sb;
    float p_rot;
    u_char div_x;
    u_char div_z;
    u_char dr_num;
    u_short dr_type;
    u_char door_no;
    u_short pos_x;
    u_short pos_y;

    dtdp = GetDoorTypeDatP(0);

    div_x = *dx_max;
    div_z = *dz_max;

    pos[0] = ppos[0];
    pos[1] = ppos[1];
    pos[2] = ppos[2];
    pos[3] = ppos[3];

    door_no = GetRoomIdFromRoomNo(0, room_wrk.room_no);
    rdp = GetDoorRoomConectDataP(door_no);

    if (rdp == NULL)
    {
        return 0;
    }

    dr_num = *rdp;

    rdp += 2;

    for (i = 0; i < dr_num; i++, rdp += 4)
    {
        for (j = 0; j < 20; j++)
        {
            if (door_wrk[j].door_id == *(u_short *)rdp)
            {
                ddp = GetDoorDataTopP(*(u_short *)rdp);

                if (ddp == NULL)
                {
                    return 0xff;
                }

                dhp = (int *)(*ddp + BASE_ADDRESS);
                dhp = (int *)(*dhp + BASE_ADDRESS);

                switch (door_wrk[j].stts)
                {
                case 1:

                    stp = &((u_char *)dhp)[17];

                    dsp = (int *)(*ddp + BASE_ADDRESS);
                    dsp = (int *)(dsp[1] + BASE_ADDRESS);

                    for (k = 0; k < div_x; k++)
                    {
                        pos_y = plyr_wrk.move_box.pos[0] + (dst[0] * (k+1)) / div;
                        pos_x = plyr_wrk.move_box.pos[2];

                        if (PosInAreaJudgeSub(dsp, pos_x, pos_y, *stp) != 0)
                        {
                            div_x = k;

                            break;
                        }
                    }

                    for (k = 0; k < div_z; k++)
                    {
                        pos_y = plyr_wrk.move_box.pos[0];
                        pos_x = plyr_wrk.move_box.pos[2] + (dst[2] * (k+1)) / div;

                        if (PosInAreaJudgeSub(dsp, pos_x, pos_y, *stp) != 0)
                        {
                            div_z = k;

                            break;
                        }
                    }

                    pos_y = (plyr_wrk.move_box.pos[0] + (dst[0] * div_x) / div);
                    pos_x = (plyr_wrk.move_box.pos[2] + (dst[2] * div_z) / div);

                    if (PosInAreaJudgeSub(dsp, pos_x, pos_y, *stp) != 0)
                    {
                        div_x = 0;
                        div_z = 0;
                    }
                break;
                case 4:
                case 5:
                    dr_type = door_wrk[j].type;

                    if ((dtdp[dr_type].attribute & 0x1000) == 0)
                    {
                        GetDoorHitVertex(door_wrk[j].pos, v0, v1, v2, v3, door_wrk[j].rot, dtdp[dr_type].open_wait, dr_type);

                        for (k = 0; k < div_x; k++)
                        {
                            mv_x = plyr_wrk.move_box.pos[0] + (dst[0] * (k+1)) / div;
                            mv_z = plyr_wrk.move_box.pos[2];

                            if (ChkInsideBox(mv_x, mv_z, v0[0], v0[2], v1[0], v1[2], v2[0], v2[2], v3[0], v3[2]) != 0)
                            {
                                div_x = k;

                                break;
                            }
                        }

                        for (k = 0; k < div_z; k++)
                        {
                            mv_x = plyr_wrk.move_box.pos[0];
                            mv_z = plyr_wrk.move_box.pos[2] + (dst[2] * (k+1)) / div;

                            if (ChkInsideBox(mv_x, mv_z, v0[0], v0[2], v1[0], v1[2], v2[0], v2[2], v3[0], v3[2]) != 0)
                            {
                                div_z = k;

                                break;
                            }
                        }

                        mv_x = plyr_wrk.move_box.pos[0] - door_wrk[j].pos[0];
                        mv_z = plyr_wrk.move_box.pos[2] - door_wrk[j].pos[2];

                        if (GetDist(mv_z, mv_x) > 550.0f)
                        {
                            continue;
                        }
                        if (dtdp[dr_type].move_max < 0)
                        {
                            sa = door_wrk[j].rot;
                            sb = sa + DEG2RAD(dtdp[dr_type].move_max);
                        }
                        else
                        {
                            sb = door_wrk[j].rot;
                            sa = sb + DEG2RAD(dtdp[dr_type].move_max);
                        }

                        p_rot = SgAtanf(mv_x / mv_z);

                        if (mv_z < 0.0f)
                        {
                            p_rot += PI;
                        }

                        p_rot -= PI / 2;

                        if (ChkRotBetween(sa, sb, p_rot) != 0)
                        {
                            for (k = 0; k < 10; k++)
                            {
                                if (ac_door_wrk[k].door_id == door_wrk[j].door_id)
                                {
                                    break;
                                }
                            }

                            if (k < 10)
                            {

                                ac_door_wrk[k].count = 0;
                            }
                        }
                    }
                break;
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

u_char DoorHitCheck2(u_short pos_x, u_short pos_y, u_char room_id)
{
    DOOR_TYPE_DAT *dtdp;
    sceVu0FVECTOR v0;
    sceVu0FVECTOR v1;
    sceVu0FVECTOR v2;
    sceVu0FVECTOR v3;
    DOOR_WRK *dwp;
    int *ddp;
    int *dhp;
    int *dsp;
    u_char *rdp;
    int i;
    int j;
    u_char dr_num;

    rdp = GetDoorRoomConectDataP(room_id);

    if (rdp == NULL)
    {
        return 0;
    }

    dr_num = *rdp;

    rdp += 2;

    for (i = 0; i < dr_num; i++)
    {
        dwp = door_wrk;

        for (j = 0; j < 20; j++)
        {
            if (dwp->door_id == *(u_short *)rdp)
            {
                ddp = GetDoorDataTopP(*(u_short *)rdp);

                if (ddp == NULL)
                {
                    return 0;
                }

                dhp = (int *)(*ddp + BASE_ADDRESS);
                dhp = (int *)(*dhp + BASE_ADDRESS);

                switch(dwp->stts)
                {
                case 1:
                    dsp = (int *)(*ddp + BASE_ADDRESS);
                    dsp = (int *)(dsp[1] + BASE_ADDRESS);

                    dhp = (int *)&((char *)dhp)[17];

                    if (PosInAreaJudgeSub(dsp, pos_x, pos_y, *(u_char *)dhp) != 0)
                    {
                        return 1;
                    }
                break;
                case 3:
                case 6:
                case 7:
                case 8:
                    dtdp = GetDoorTypeDatP(dwp->type);

                    if (IsFusumaType(dwp->type) != 0)
                    {
                        GetFusumaHitVertex2(dwp->npos, v0, v1, v2, v3, dwp->nrot, dtdp->open_wait, dwp->type, 1);
                    }
                    else
                    {
                        GetDoorHitVertex2(dwp->pos, v0, v1, v2, v3, dwp->nrot, dtdp->open_wait, dwp->type, 1);
                    }

                    if (ChkInsideBox(pos_y, pos_x, v0[0], v0[2], v1[0], v1[2], v2[0], v2[2], v3[0], v3[2]) != 0)
                    {
                        return 1;
                    }
                case 4:
                case 5:
                    dtdp = GetDoorTypeDatP(dwp->type);

                    if (IsFusumaType(dwp->type) != 0)
                    {
                        GetFusumaHitVertex2(dwp->npos, v0, v1, v2, v3, dwp->nrot, dtdp->open_wait, dwp->type, 0);
                    }
                    else
                    {
                        GetDoorHitVertex2(dwp->pos, v0, v1, v2, v3, dwp->nrot, dtdp->open_wait, dwp->type, 0);
                    }

                    if (ChkInsideBox(pos_y, pos_x, v0[0], v0[2], v1[0], v1[2], v2[0], v2[2], v3[0], v3[2]) != 0)
                    {
                        return 1;
                    }
                break;
                }
            }

            dwp++;
        }

        rdp += 4;
    }

    return 0;
}

u_char DoorCoverCheck(u_short pos_x, short int pos_y, u_short pos_z, u_char room_id)
{
    DOOR_TYPE_DAT *dtdp;
    int *dsp;
    int *ddp;
    int *dhp;
    u_char *rdp;
    sceVu0FVECTOR v0;
    sceVu0FVECTOR v1;
    sceVu0FVECTOR v2;
    sceVu0FVECTOR v3;
    int i;
    int j;
    int k;
    u_short count;
    u_char dr_num;
    u_char dr_type;
    u_char hit_flg;

    dtdp = GetDoorTypeDatP(0);

    rdp = GetDoorRoomConectDataP(room_id);

    if (rdp == NULL)
    {
        return 0;
    }

    dr_num = *rdp;

    rdp += 2;

    for (i = 0; i < dr_num; i++, rdp += 4)
    {
        for (j = 0; j < 20; j++)
        {
            hit_flg = 0;

            if (door_wrk[j].door_id != *(u_short *)rdp)
            {
                continue;
            }

            ddp = GetDoorDataTopP(*(u_short *)rdp);

            if (ddp == NULL)
            {
                return 0xff;
            }

            dhp = (int *)(*ddp + BASE_ADDRESS);
            dhp = (int *)(*dhp + BASE_ADDRESS);

            switch(door_wrk[j].stts)
            {
            case 1:
                dsp = (int *)(*ddp + BASE_ADDRESS);
                dsp = (int *)(dsp[1] + BASE_ADDRESS);

                dhp = (int *)&((char *)dhp)[17];

                hit_flg = PosInAreaJudgeSub(dsp, pos_z, pos_x, *(u_char *)dhp) != 0;
            break;
            case 4:
                dr_type = door_wrk[j].type;

                if (dtdp[dr_type].attribute & 0x1000)
                {
                    GetFusumaHitVertex(door_wrk[j].pos, v0, v1, v2, v3, door_wrk[j].rot, dtdp[dr_type].open_wait, dr_type);
                }
                else
                {
                    GetDoorHitVertex(door_wrk[j].pos, v0, v1, v2, v3, door_wrk[j].rot, dtdp[dr_type].open_wait, dr_type);
                }

                hit_flg = ChkInsideBox(pos_x, pos_z, v0[0], v0[2], v1[0], v1[2], v2[0], v2[2], v3[0], v3[2]) != 0;
            break;
            case 3:
            case 6:
                dr_type = door_wrk[j].type;

                for (k = 0; k < 10; k++)
                {
                    if (ac_door_wrk[k].door_id == door_wrk[j].door_id)
                    {
                        count = ac_door_wrk[k].count;

                        break;
                    }
                }

                if (k >= 10)
                {
                    for (k = 0; k < 50; k++)
                    {
                        if (nc_door_wrk[k].door_id == door_wrk[j].door_id)
                        {
                            count = nc_door_wrk[k].count;

                            break;
                        }
                    }

                    if (k >= 50)
                    {
                        break;
                    }
                }

                if (dtdp[dr_type].attribute & 0x1000)
                {
                    GetFusumaHitVertex(door_wrk[j].pos, v0, v1, v2, v3, door_wrk[j].rot, count, dr_type);
                }
                else
                {
                    GetDoorHitVertex(door_wrk[j].pos, v0, v1, v2, v3, door_wrk[j].rot, count, dr_type);
                }

                hit_flg = ChkInsideBox(pos_z, pos_x, v0[2], v0[0], v1[2], v1[0], v2[2], v2[0], v3[2], v3[0]) != 0;
            break;
            case 2:
            case 5:
                // do nothing ...
            break;
            }

            if (hit_flg)
            {
                if (door_wrk[j].pos[1] - 900.0f <= pos_y && pos_y <= door_wrk[j].pos[1])
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}

static void GetFusumaHitVertex(sceVu0FVECTOR axis, sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, sceVu0FVECTOR v3, float pre_rot, u_short count, u_short type)
{
    sceVu0FVECTOR av0;
    sceVu0FVECTOR av1;
    sceVu0FVECTOR av2;
    sceVu0FVECTOR av3;
    sceVu0FVECTOR rot;
    sceVu0FVECTOR add_sld;
    DOOR_TYPE_DAT *dtdp;
    int i;

    dtdp = GetDoorTypeDatP(type);

    av0[0] = 0.0f;
    av0[1] = 0.0f;
    av0[2] = 175.0f;

    av1[0] = 550.0f;
    av1[1] = 0.0f;
    av1[2] = 175.0f;

    av2[0] = 0.0f;
    av2[1] = 0.0f;
    av2[2] = -175.0f;

    av3[0] = 550.0f;
    av3[1] = 0.0f;
    av3[2] = -175.0f;

    rot[0] = 0.0f;
    rot[1] = pre_rot;
    rot[2] = 0.0f;

    add_sld[2] = 0.0f;
    add_sld[1] = 0.0f;
    add_sld[0] = (int)((dtdp->move_max * count) / dtdp->open_wait);

    for (i = 0; i < 3; i++)
    {
        av0[i] += add_sld[i];
        av1[i] += add_sld[i];
        av2[i] += add_sld[i];
        av3[i] += add_sld[i];
    }

    RotFvector(rot, av0);
    RotFvector(rot, av1);
    RotFvector(rot, av2);
    RotFvector(rot, av3);

    for (i = 0; i < 3; i++)
    {
        v0[i] = axis[i] + av0[i];
        v1[i] = axis[i] + av1[i];
        v2[i] = axis[i] + av2[i];
        v3[i] = axis[i] + av3[i];
    }
}

static void GetFusumaHitVertex2(sceVu0FVECTOR axis, sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, sceVu0FVECTOR v3, float pre_rot, u_short count, u_short type, u_char mv)
{
    sceVu0FVECTOR av0;
    sceVu0FVECTOR av1;
    sceVu0FVECTOR av2;
    sceVu0FVECTOR av3;
    sceVu0FVECTOR rot;
    sceVu0FVECTOR add_sld;
    int i;

    GetDoorTypeDatP(type);

    if (mv != 0)
    {
        av0[0] = -50.0f;
        av0[1] = 0.0f;
        av0[2] = 200.0f;

        av1[0] = 600.0f;
        av1[1] = 0.0f;
        av1[2] = 200.0f;

        av2[0] = -50.0f;
        av2[1] = 0.0f;
        av2[2] = -200.0f;

        av3[0] = 600.0f;
        av3[1] = 0.0f;
        av3[2] = -200.0f;
    }
    else
    {
        av0[0] = -50.0f;
        av0[1] = 0.0f;
        av0[2] = 175.0f;

        av1[0] = 550.0f;
        av1[1] = 0.0f;
        av1[2] = 175.0f;

        av2[0] = -50.0f;
        av2[1] = 0.0f;
        av2[2] = -175.0f;

        av3[0] = 550.0f;
        av3[1] = 0.0f;
        av3[2] = -175.0f;
    }

    rot[0] = 0.0f;
    rot[1] = pre_rot;
    rot[2] = 0.0f;

    RotLimitChk(&rot[1]);

    RotFvector(rot, av0);
    RotFvector(rot, av1);
    RotFvector(rot, av2);
    RotFvector(rot, av3);

    for (i = 0; i < 3; i++)
    {
        v0[i] = axis[i] + av0[i];
        v1[i] = axis[i] + av1[i];
        v2[i] = axis[i] + av2[i];
        v3[i] = axis[i] + av3[i];
    }
}

static void GetDoorHitVertex(sceVu0FVECTOR axis, sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, sceVu0FVECTOR v3, float pre_rot, u_short count, u_short type)
{
    sceVu0FVECTOR av0;
    sceVu0FVECTOR av1;
    sceVu0FVECTOR av2;
    sceVu0FVECTOR av3;
    sceVu0FVECTOR rot;
    sceVu0FVECTOR add_sld;
    int i;
    DOOR_TYPE_DAT *dtdp;

    dtdp = GetDoorTypeDatP(0);

    av0[0] = 0.0f;
    av0[1] = 0.0f;
    av0[2] = 175.0f;

    av1[0] = 550.0f;
    av1[1] = 0.0f;
    av1[2] = 175.0f;

    av2[0] = 0.0f;
    av2[1] = 0.0f;
    av2[2] = -175.0f;

    av3[0] = 550.0f;
    av3[1] = 0.0f;
    av3[2] = -175.0f;

    rot[0] = 0.0f;
    rot[1] = pre_rot;
    rot[2] = 0.0f;

    if (dtdp[type].attribute & 0x1000)
    {
        add_sld[2] = dtdp[type].move_max * SgSinf(pre_rot);
        add_sld[1] = 0.0f;
        add_sld[0] = dtdp[type].move_max * SgCosf(pre_rot);
    }
    else
    {
        add_sld[0] = 0.0f;
        add_sld[1] = 0.0f;
        add_sld[2] = 0.0f;

        RotLimitChk(&rot[1]);
    }

    RotFvector(rot, av0);
    RotFvector(rot, av1);
    RotFvector(rot, av2);
    RotFvector(rot, av3);

    for (i = 0; i < 3; i++)
    {
        v0[i] = axis[i] + av0[i] + add_sld[i];
        v1[i] = axis[i] + av1[i] + add_sld[i];
        v2[i] = axis[i] + av2[i] + add_sld[i];
        v3[i] = axis[i] + av3[i] + add_sld[i];
    }
}

static void GetDoorHitVertex2(sceVu0FVECTOR axis, sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, sceVu0FVECTOR v3, float pre_rot, u_short count, u_short type, u_char mv)
{
    sceVu0FVECTOR av0;
    sceVu0FVECTOR av1;
    sceVu0FVECTOR av2;
    sceVu0FVECTOR av3;
    sceVu0FVECTOR rot;
    sceVu0FVECTOR add_sld;
    int i;

    GetDoorTypeDatP(0);

    if (mv != 0)
    {
        av0[0] = 0.0f;
        av0[1] = 0.0f;
        av0[2] = 200.0f;

        av1[0] = 600.0f;
        av1[1] = 0.0f;
        av1[2] = 200.0f;

        av2[0] = 0.0f;
        av2[1] = 0.0f;
        av2[2] = -200.0f;

        av3[0] = 600.0f;
        av3[1] = 0.0f;
        av3[2] = -200.0f;
    }
    else
    {
        av0[0] = 0.0f;
        av0[1] = 0.0f;
        av0[2] = 175.0f;

        av1[0] = 550.0f;
        av1[1] = 0.0f;
        av1[2] = 175.0f;

        av2[0] = 0.0f;
        av2[1] = 0.0f;
        av2[2] = -175.0f;

        av3[0] = 550.0f;
        av3[1] = 0.0f;
        av3[2] = -175.0f;
    }

    rot[0] = 0.0f;
    rot[1] = pre_rot;
    rot[2] = 0.0f;

    RotLimitChk(&rot[1]);

    RotFvector(rot, av0);
    RotFvector(rot, av1);
    RotFvector(rot, av2);
    RotFvector(rot, av3);

    for (i = 0; i < 3; i++)
    {
        v0[i] = axis[i] + av0[i];
        v1[i] = axis[i] + av1[i];
        v2[i] = axis[i] + av2[i];
        v3[i] = axis[i] + av3[i];
    }
}

static int ChkRotBetween(float sa, float sb, float rot)
{
    while (sa < 0.0f)
    {
        sa += PI * 2;
    }

    while (sb < 0.0f)
    {
        sb += PI * 2;
    }

    while (rot < 0.0f)
    {
        rot += PI * 2;
    }

    if (sa >= sb)
    {
        if (rot <= sa && sb <= rot)
        {
            return 1;
        }
    }
    else
    {
        if (rot <= sa || sb <= rot)
        {
            return 1;
        }
    }

    return 0;
}

void DoorDataRenewNow()
{
    int i;

    for (i = 0; i < 20; i++)
    {
        if (door_wrk[i].use == 3)
        {
            door_wrk[i].use = 1;
            furn_wrk[door_wrk[i].fwrk_no].use = 1;
        }
        else if (door_wrk[i].use == 1)
        {
            door_wrk[i].use = 3;
            furn_wrk[door_wrk[i].fwrk_no].use = 3;
        }
    }
}

void DoorFreeFurnWrk(u_char room_id)
{
    int i;

    if (room_id == 0xff)
    {
        return;
    }

    for (i = 0; i < 20; i++)
    {
        if (door_wrk[i].use == 3)
        {
            door_wrk[i].use = 5;
            door_wrk[i].stts = DOOR_STTS_NO_USE;

            FurnSetWrkNoUse(&furn_wrk[door_wrk[i].fwrk_no], door_wrk[i].fwrk_no);

            door_wrk[i].door_id = 0xffff;
        }
        else
        {
            if (door_wrk[i].use == 4)
            {
                door_wrk[i].use = 1;

                furn_wrk[door_wrk[i].fwrk_no].use = 1;
            }
        }
    }
}

void DoorJoint2Next()
{
    int i;

    for (i = 0; i < 20; i++)
    {
        if (door_wrk[i].use == 4)
        {
            door_wrk[i].use = 3;

            furn_wrk[door_wrk[i].fwrk_no].use = 3;
        }
    }
}

void DoorDataRenewNext(u_char room_id)
{
    int *addr_si;
    u_char *addr_uc;
    u_short *dr_tbl;
    int i;
    int j;
    u_char no_use_dw;
    u_char no_use_fw;
    u_char dr_num;
    u_short dr_id;
    u_short dr_no;
    u_short dr_all;
    u_char room_no;

    GetDoorTypeDatP(0);

    if (room_id == 0xff)
    {
        return;
    }

    SetUpRoomCoordinate(room_id, room_wrk.pos[1]);

    room_no = GetDataRoom(10, room_id);

    addr_si = (int *)(map_wrk.dat_adr + 10 * 4);
    addr_si = (int *)(*addr_si + BASE_ADDRESS);
    addr_si = (int *)(addr_si[1] + BASE_ADDRESS);
    addr_si = (int *)(addr_si[room_no] + BASE_ADDRESS);

    addr_uc = (u_char *)addr_si;

    dr_num = *addr_uc;
    addr_uc += 2;

    no_use_dw = 0;
    no_use_fw = 0;

    for (i = 0; i < dr_num; i++)
    {
        dr_id = *(u_short *)addr_uc;

        addr_uc += 4;

        if (room_id == GetNextRIdFromRNoDId(room_wrk.room_no, dr_id))
        {
            for (j = 0; j < 20; j++)
            {
                if (door_wrk[j].door_id == dr_id)
                {
                    door_wrk[j].use = 4;

                    furn_wrk[door_wrk[j].fwrk_no].use = 4;

                    if (furn_wrk[door_wrk[j].fwrk_no].furn_no != 0xffff && furn_wrk[door_wrk[j].fwrk_no].furn_no != 0)
                    {
                        ChoudoPreRenderDual(&furn_wrk[door_wrk[j].fwrk_no]);
                    }
                }
            }
        }
        else
        {
            dr_tbl = GetDoorDoorNumP();

            dr_all = *dr_tbl;
            dr_tbl++;

            for (j = 0; j < dr_all; j++)
            {
                if (*dr_tbl == dr_id)
                {
                    dr_no = j;

                    break;
                }

                dr_tbl++;
            }

            if (dr_no >= dr_all)
            {
                return;
            }

            addr_si = (int *)(map_wrk.dat_adr + 10 * 4);
            addr_si = (int *)(*addr_si + BASE_ADDRESS);
            addr_si = (int *)(addr_si[2] + BASE_ADDRESS);
            addr_si = (int *)(addr_si[dr_no] + BASE_ADDRESS);
            addr_si = (int *)(*addr_si + BASE_ADDRESS);

            while (door_wrk[no_use_dw].use != 5)
            {
                no_use_dw++;

                if (no_use_dw > 250)
                {
                    break;
                }
            }

            while (FurnIsWrkUse(furn_wrk + no_use_fw) != 0)
            {
                no_use_fw++;

                if (no_use_fw > 250)
                {
                    break;
                }
            }

            SetDoorWrk(no_use_dw, (u_int *)addr_si, dr_id, no_use_fw);

            door_wrk[no_use_dw].use = 3;

            DoorSetFurnWrk(no_use_dw, no_use_fw, 3, room_id);
        }
    }
}

u_char GetNextRIdFromRNoDId(u_char room_no, u_short door_id)
{
    int *addr_si;
    u_char *addr_uc;
    int i;
    u_char dr_num;
    u_char room_id;

    room_id = GetRoomIdFromRoomNo(0, room_no);
    room_no = GetDataRoom(10, room_id);

    addr_si = (int *)(map_wrk.dat_adr + 10 * 4);
    addr_si = (int *)(*addr_si + BASE_ADDRESS);
    addr_si = (int *)(addr_si[1] + BASE_ADDRESS);

    addr_uc = (u_char *)(addr_si[room_no] + BASE_ADDRESS);

    dr_num = *addr_uc;

    addr_uc += 2;

    for (i = 0; i < dr_num; i++)
    {
        if (door_id == *(u_short *)addr_uc)
        {
            return (u_char)addr_uc[2];
        }

        addr_uc += 4;
    }

    return 0xff;
}

u_char NextRoomRenew()
{
    int *addr;
    u_char disp_no;
    u_char room_no;

    disp_no = ChkNextRoomIsInsite();

    if (disp_no != 0xff)
    {
        if (disp_no == room_wrk.disp_no[0] || disp_no == room_wrk.disp_no[1])
        {
            return 0xff;
        }

        room_no = GetDataRoom(0, disp_no);

        if (room_no != 0xff)
        {
            addr = (int *)map_wrk.dat_adr;
            addr = (int *)(*addr + BASE_ADDRESS);
            addr = &addr[room_no] + 1;
            addr = (int *)(*addr + BASE_ADDRESS);

            room_wrk.disp_no[1] = disp_no;

            room_wrk.pos[1][0] = ((u_short *)(*addr + BASE_ADDRESS))[0];
            room_wrk.pos[1][1] = ((short *)(*addr + BASE_ADDRESS))[1];
            room_wrk.pos[1][2] = ((u_short *)(*addr + BASE_ADDRESS))[2];

            return disp_no;
        }
    }

    return 0xff;
}

u_char ChkNextRoomIsInsite()
{
    sceVu0FVECTOR dp;
    sceVu0FVECTOR dv;
    short int *addr_ss;
    float drot;
    float dst_min;
    float tmp_dst;
    u_short dst_min_did;
    u_char dst_min_nrid;
    u_char *addr_uc;
    u_char next_rm_id;
    u_short dr_id;
    u_char dr_num;
    u_char i;
    u_char now_room_id;

    now_room_id = GetRoomIdFromRoomNo(0, room_wrk.room_no);

    addr_uc = GetDoorRoomConectDataP(now_room_id);

    if (addr_uc == NULL)
    {
        return 0xff;
    }

    dr_num = *addr_uc;
    addr_uc += 2;

    dst_min_did = 0xffff;
    dst_min_nrid = 0xff;

    for (i = 0; i < dr_num; i++)
    {
        dr_id = *(u_short *)addr_uc;
        addr_uc += 2;

        next_rm_id =  *(u_char *)addr_uc;
        addr_uc += 2;

        if (next_rm_id == now_room_id)
        {
            continue;
        }

        if (ChkDoorIsOpen(dr_id) != 0)
        {
            addr_ss = (short *)GetDoorDataHeader(dr_id);

            if (addr_ss == NULL)
            {
                continue;
            }

            drot = *(float *)addr_ss;
            addr_ss += 2;

            dp[0] = *(u_short *)addr_ss;
            addr_ss++;

            dp[1] = *(short *)addr_ss;
            addr_ss++;

            dp[2] = *(u_short *)addr_ss;

            if (ChkDoorIsInsite(dp, drot, dr_id) == 0)
            {
                if (ChkDoorIsInsite2(dp, drot, dr_id) == 0)
                {
                    continue;
                }
            }

            tmp_dst = GetDist(dp[0] - camera.p[0], dp[2] - camera.p[2]);

            if (dst_min_did == 0xffff || dst_min > tmp_dst)
            {
                dst_min = tmp_dst;
                dst_min_did = dr_id;
                dst_min_nrid = next_rm_id;
            }
        }
    }

    if (dst_min_did == 0xffff)
    {
        return 0xff;
    }

    return dst_min_nrid;
}

u_char* GetDoorRoomNumP()
{
    int *addr;

    addr = (int*)(((int*)map_wrk.dat_adr)[10] + BASE_ADDRESS);
    addr = (int*)(addr[0] + BASE_ADDRESS);
    addr = (int*)(addr[0] + BASE_ADDRESS);

    return (u_char*)addr;
}

u_char* GetDoorRoomNumPFloor(u_char floor)
{
    int *addr;

    addr = (int *)(GetFloorTopAddr(floor) + 10 * 4);
    addr = (int *)(*addr + BASE_ADDRESS);
    addr = (int *)(*addr + BASE_ADDRESS);
    addr = (int *)(*addr + BASE_ADDRESS);

    return (u_char *)addr;
}

static u_short* GetDoorDoorNumP()
{
    int *addr;

    addr = (int*)(((int*)map_wrk.dat_adr)[10] + BASE_ADDRESS);
    addr = (int*)(addr[0] + BASE_ADDRESS);
    addr = (int*)(addr[1] + BASE_ADDRESS);

    return (u_short*)addr;
}

u_char* GetDoorRoomConectDataP(u_char room_id)
{
    u_char *addr_uc;
    int *addr_si;
    u_char room_num;
    u_char i;

    addr_uc = GetDoorRoomNumP();

    room_num = *addr_uc;
    addr_uc++;

    for (i = 0; i < room_num; i++)
    {
        if (*addr_uc == room_id)
        {
            addr_si = (int *)(map_wrk.dat_adr + 10 * 4);
            addr_si = (int *)(addr_si[0] + BASE_ADDRESS);
            addr_si = (int *)(addr_si[1] + BASE_ADDRESS);
            addr_si = (int *)(addr_si[i] + BASE_ADDRESS);

            return (u_char *)addr_si;
        }

        addr_uc++;
    }

    return NULL;
}

static int* GetDoorDataTopP(u_short door_id)
{
    int *addr_si;
    u_short *addr_us;
    u_char i;
    u_short door_num;

    addr_us = (u_short *)GetDoorDoorNumP();

    door_num = *addr_us;
    addr_us++;

    for (i = 0; i < door_num; i++)
    {
        if (*addr_us == door_id)
        {
            addr_si = (int *)(map_wrk.dat_adr + 10 * 4);
            addr_si = (int *)(addr_si[0] + BASE_ADDRESS);
            addr_si = (int *)(addr_si[2] + BASE_ADDRESS);
            addr_si = &addr_si[i];

            return (int *)addr_si;
        }

        addr_us++;
    }

    return NULL;
}

static int* GetDoorDataHeader(u_short door_id)
{
    int *addr_si;

    addr_si = GetDoorDataTopP(door_id);

    if (addr_si == NULL)
    {
        return NULL;
    }

    addr_si = (int *)(*addr_si + BASE_ADDRESS);
    addr_si = (int *)(*addr_si + BASE_ADDRESS);

    return addr_si;
}

int ChkDoorIsInsite(sceVu0FVECTOR dpos, float dpre_rot, u_short door_id)
{
    sceVu0FVECTOR vr;
    sceVu0FVECTOR vp;
    sceVu0FVECTOR r2;
    sceVu0FVECTOR tvp;
    sceVu0FVECTOR dp;
    float rot_center;
    float rot_door;
    float view_rad;
    float dist_obj;
    float cam_rot;
    int ret_val;
    int i;
    float rot_center_min;
    float rot_center_max;

    vp[0] = camera.p[0];
    vp[1] = camera.p[1];
    vp[2] = camera.p[2];

    tvp[0] = 0.0f;
    tvp[1] = 0.0f;
    tvp[2] = -200.0f;

    cam_rot = SgAtan2f(camera.i[0] - camera.p[0], camera.i[2] - camera.p[2]);

    vp[0] += tvp[0] * SgCosf(cam_rot);
    vp[2] -= tvp[2] * SgSinf(cam_rot);

    vr[0] = camera.i[0];
    vr[1] = camera.i[1];
    vr[2] = camera.i[2];

    for (i = 0; i < 2; i++)
    {
        ret_val = 0;

        if (i == 0)
        {
            dp[0] = dpos[0];
            dp[1] = dpos[1];
            dp[2] = dpos[2];
        }
        else
        {
            dp[0] = +(SgCosf(dpre_rot) * 450.0f) + dpos[0];
            dp[1] = dpos[1];
            dp[2] = -(SgSinf(dpre_rot) * 450.0f) + dpos[2];
        }

        dist_obj = GetDistV(dp, vp);

        if (dist_obj > 10000.0f)
        {
            continue;
        }

        view_rad = (10000.0f - dist_obj) * (PI / 2) / 10000.0f;

        rot_center = SgAtan2f(vr[0] - vp[0], vr[2] - vp[2]);
        rot_door = SgAtan2f(dp[0] - vp[0], dp[2] - vp[2]);

        while (rot_center < 0.0f)
        {
            rot_center += (PI * 2);
        }

        rot_center_min = rot_center - view_rad;

        while (rot_door < 0.0f)
        {
            rot_door += (PI * 2);
        }

        rot_center_max = rot_center + view_rad;

        if (rot_center_min < 0.0f)
        {
            if (rot_center_min + (PI * 2) <= rot_door || rot_door <= rot_center_max)
            {
                ret_val = 1;
            }
        }
        else if (rot_center_max > (PI * 2))
        {
            if (rot_door <= rot_center_max - (PI * 2) || rot_door >= rot_center_min)
            {
                ret_val = 1;
            }
        }
        else if (rot_door >= rot_center_min && rot_door <= rot_center_max)
        {
            ret_val = 1;
        }

        if (ret_val == 1)
        {
            return 1;
        }
    }

    return ret_val;
}

int ChkDoorIsInsite2(sceVu0FVECTOR dpos, float dpre_rot, u_short door_id)
{
    sceVu0FVECTOR vr1;
    sceVu0FVECTOR vr2;
    sceVu0FVECTOR vr3;
    sceVu0FVECTOR tvr1;
    sceVu0FVECTOR tvr2;
    sceVu0FVECTOR tvr3;
    sceVu0FVECTOR vp;
    sceVu0FVECTOR tvpb;
    sceVu0FVECTOR vpb;
    sceVu0FVECTOR dp1;
    sceVu0FVECTOR dp2;
    sceVu0FVECTOR prot;
    sceVu0FVECTOR drot;
    float cam_rot;
    float tmp_cos;
    float tmp_sin;
    static int count = 0;

    vp[0] = camera.p[0];
    vp[1] = camera.p[1];
    vp[2] = camera.p[2];

    tvr1[0] = 500.0f;
    tvr1[1] = camera.i[1];
    tvr1[2] = 50.0f;

    tvr2[0] = -500.0f;
    tvr2[1] = camera.i[1];
    tvr2[2] = 50.0f;

    tvr3[0] = 0.0f;
    tvr3[1] = 0;
    tvr3[2] = 500.0f;

    tvpb[0] = 0.0f;
    tvpb[1] = camera.i[1];
    tvpb[2] = -200.0f;

    cam_rot = SgAtan2f(camera.i[0] - camera.p[0], camera.i[2] - camera.p[2]);

    tmp_cos = SgCosf(cam_rot);
    tmp_sin = SgSinf(cam_rot);

    vr1[0] = tvr1[0] * tmp_cos + tvr1[2] * tmp_sin + vp[0];
    vr1[1] = vp[1];
    vr1[2] = tvr1[2] * tmp_cos - tvr1[0] * tmp_sin + vp[2];

    vr2[0] = tvr2[0] * tmp_cos + tvr2[2] * tmp_sin + vp[0];
    vr2[1] = vp[1];
    vr2[2] = tvr2[2] * tmp_cos - tvr2[0] * tmp_sin + vp[2];

    vr3[0] = tvr3[0] * tmp_cos + tvr3[2] * tmp_sin + vp[0];
    vr3[1] = vp[1];
    vr3[2] = tvr3[2] * tmp_cos - tvr3[0] * tmp_sin + vp[2];

    vpb[0] = tvpb[0] * tmp_cos + tvpb[2] * tmp_sin + vp[0];
    vpb[1] = vp[1];
    vpb[2] = tvpb[2] * tmp_cos - tvpb[0] * tmp_sin + vp[2];

    dp2[0] = (SgCosf(dpre_rot) * 450.0f) + dpos[0];
    dp2[1] = dpos[1];
    dp2[2] = -(SgSinf(dpre_rot) * 450.0f) + dpos[2];

    if (ChkLineCross(vpb[0], vpb[2], vr1[0], vr1[2], dpos[0], dpos[2], dp2[0], dp2[2]) != 0)
    {
        return 1;
    }

    if (ChkLineCross(vpb[0], vpb[2], vr2[0], vr2[2], dpos[0], dpos[2], dp2[0], dp2[2]) != 0)
    {
        return 1;
    }

    if (ChkLineCross(vpb[0], vpb[2], vr3[0], vr3[2], dpos[0], dpos[2], dp2[0], dp2[2]) != 0)
    {
        return 1;
    }

    asm(""); // debug code?

    return 0;
}

int TmpMapHit(u_short door_id)
{
    sceVu0FVECTOR fpos;
    sceVu0FVECTOR tpos;
    u_int *addr_ui;
    u_char chk;

    fpos[0] = camera.p[0];
    fpos[1] = camera.p[1];
    fpos[2] = camera.p[2];

    addr_ui = (u_int *)GetDoorDataHeader(door_id);

    if (addr_ui == NULL)
    {
        return 0;
    }

    addr_ui++;

    tpos[0] = ((short *)addr_ui)[0];
    tpos[1] = ((short *)addr_ui)[1];
    tpos[2] = ((short *)addr_ui)[2];

    chk = HitChkSegment2All2D(fpos, tpos, 100.0f);

    return chk == 0;
}

static int ChkDoorIsOpen(u_short door_id)
{
    int i;

    for (i = 0; i < 50; i++)
    {
        if (
            nc_door_wrk[i].door_id == door_id &&
            nc_door_wrk[i].stts != DOOR_STTS_NO_USE &&
            nc_door_wrk[i].stts != DOOR_STTS_CLOSE
        )
        {
            return 1;
        }
    }

    for (i = 0; i < 10; i++)
    {
        if (ac_door_wrk[i].door_id == door_id)
        {
            break;
        }
    }

    if (i >= 10)
    {
        return 0;
    }

    if (ac_door_wrk[i].stts == DOOR_STTS_NO_USE || ac_door_wrk[i].stts == DOOR_STTS_CLOSE)
    {
        return 0;
    }

    return 1;
}

static u_short GetDoorIdFromDoorNo(u_char door_no)
{
    u_short *addr_us;
    int i;
    u_short dr_num;

    addr_us = (u_short *)GetDoorDoorNumP();

    dr_num = *addr_us;
    addr_us++;

    for (i = 0; i < dr_num; i++)
    {
        if (door_no == i)
        {
            break;
        }

        addr_us++;
    }

    if (i >= dr_num)
    {
        return 0xffff;
    }

    return *(u_short *)addr_us;
}

static int GetDoorStts(DOOR_DATA_POP **dppp, u_short door_id, u_char floor)
{
    int *addr_si0;
    int *addr_si1;
    u_short *addr_us;
    int i;
    u_short dat_num;

    if (floor_exist[ingame_wrk.msn_no][floor] == 0)
    {
        return 0;
    }

    addr_si0 = (int *)GetFloorTopAddr(floor);
    addr_si0 = (int *)(addr_si0[10] + BASE_ADDRESS);

    addr_si1 = (int *)(*addr_si0 + BASE_ADDRESS);
    addr_si1 = (int *)(addr_si1[1] + BASE_ADDRESS);

    dat_num = *addr_si1;

    addr_us = (u_short *)addr_si1 + 1;

    for (i = 0; i < dat_num; i++, addr_us++)
    {
        if (*addr_us == door_id)
        {
            break;
        }
    }

    if (i >= dat_num)
    {
        return 0;
    }

    addr_si1 = (int *)(addr_si0[2] + BASE_ADDRESS);
    addr_si1 = (int *)(addr_si1[i] + BASE_ADDRESS);

    *dppp = (DOOR_DATA_POP *)(*addr_si1 + BASE_ADDRESS);

    return 1;
}

int GetDoorSttsForMap(DOOR_STTS_MAP *dsmp, u_short door_id, u_char floor)
{
    DOOR_DATA_POP *dpp;

    if (GetDoorStts(&dpp, door_id, floor) != 0)
    {
        dsmp->rot = dpp->rot;
        dsmp->pos_x = SgCosf(dsmp->rot) * +225.0f + dpp->pos_x;
        dsmp->pos_z = SgSinf(dsmp->rot) * -225.0f + dpp->pos_z;
        dsmp->stts = door_keep[door_id].stts_map;

        return 1;
    }

    dsmp->stts = 0x0;

    return 0;
}

u_short DoorChangeModel(u_short door_id, u_short model_no)
{
    DOOR_WRK *dwp;
    int i;
    u_short last_model_no;

    last_model_no = 0xffff;

    if (door_id < 300)
    {
        dwp = door_wrk;

        last_model_no = door_keep[door_id].mdl_no;

        door_keep[door_id].mdl_no = model_no;

        for (i = 0; i < 20; i++)
        {
            if (dwp->door_id == door_id)
            {
                dwp->mdl_no = door_keep[door_id].mdl_no;

                furn_wrk[dwp->fwrk_no].furn_no = dwp->mdl_no + 1000;

                ChoudoPreRender(&furn_wrk[dwp->fwrk_no]);

                return last_model_no;
            }

            dwp++;
        }
    }

    return last_model_no;
}

void DoorDispOnOff(u_short door_id, u_char on_off)
{
    DOOR_WRK *dwp;
    int i;

    if (door_id >= 300)
    {
        return;
    }

    if (on_off == 0)
    {
        door_keep[door_id].attr |= 0x200;

        dwp = door_wrk;

        for (i = 0; i < 20; i++)
        {
            if (dwp->door_id == door_id)
            {
                if (furn_wrk[dwp->fwrk_no].furn_no != 0xffff)
                {
                    furn_wrk[dwp->fwrk_no].fno_bk = furn_wrk[dwp->fwrk_no].furn_no;
                    furn_wrk[dwp->fwrk_no].furn_no = 0xffff;

                    FurnSortFurnWrk(1);

                    return;
                }
            }
            dwp++;
        }
    }
    else
    {
        if (door_keep[(u_short)door_id].attr & 0x200)
        {
            door_keep[(u_short)door_id].attr ^= 0x200;
        }

        dwp = door_wrk;

        for (i = 0; i < 20; i++)
        {
            if (dwp->door_id == door_id)
            {
                if (furn_wrk[dwp->fwrk_no].fno_bk != 0xffff && furn_wrk[dwp->fwrk_no].fno_bk >= 1000)
                {
                    furn_wrk[dwp->fwrk_no].furn_no = furn_wrk[dwp->fwrk_no].fno_bk;
                    furn_wrk[dwp->fwrk_no].fs_flg = 1;

                    FurnSortFurnWrk(1);

                    return;
                }
            }
            dwp++;
        }
    }
}

void DoorOpenShiftForce(u_short door_id)
{
    DOOR_WRK *dwp;
    sceVu0FVECTOR ref;
    int i;

    dwp = door_wrk;

    for (i = 0; i < 20; i++)
    {
        if (dwp->door_id == door_id)
        {
            ref[0] = dwp->pos[0] + SgCosf(dwp->rot) * 225.0f;
            ref[1] = dwp->pos[1];
            ref[2] = dwp->pos[2] - SgSinf(dwp->rot) * 225.0f;

            ref[0] -= plyr_wrk.move_box.pos[0];
            ref[1] -= plyr_wrk.move_box.pos[1];
            ref[2] -= plyr_wrk.move_box.pos[2];

            plyr_wrk.move_box.rot[1] = SgAtan2f(ref[0], ref[2]);

            break;
        }

        if (i >= 20)
        {
            return;
        }

        dwp++;
    }

    DoorCheckOn(1);
}

char TargetDoorKindChk()
{
    DOJ_SQUARE_MTN dsmp;
    int dwn[2];

    return doj_square_mtn.mtn_no;
}

void PlyrPosAdj(u_char mtn_no, u_char mtn_type)
{
    DOJ_SQUARE_MTN *dsmtnp;
    DOOR_MTN_DAT *dmotionp;
    float tmp_rot;
    float adj_r;
    u_short pos_x;
    u_short pos_y;

    if (mtn_no == 0xff)
    {
        return;
    }

    dsmtnp = &doj_square_mtn;

    dmotionp = GetDoorMtnDat(mtn_no);

    tmp_rot = door_wrk[dsmtnp->dwno_base].rot;

    RotLimitChk(&tmp_rot);

    if (dsmtnp->dbl_flg == 0 && dsmtnp->exc_flg != 0)
    {
        tmp_rot += PI;

        RotLimitChk(&tmp_rot);
    }

    pos_x = dmotionp->sx * SgCosf(tmp_rot) + dmotionp->sy * SgSinf(tmp_rot) + door_wrk[dsmtnp->dwno_base].pos[0];
    pos_y = dmotionp->sy * SgCosf(tmp_rot) - dmotionp->sx * SgSinf(tmp_rot) + door_wrk[dsmtnp->dwno_base].pos[2];

    plyr_wrk.move_box.pos[0] = pos_x;
    plyr_wrk.move_box.pos[2] = pos_y;

    if (mtn_type == 0)
    {
        adj_r = 0.0f;

        RotLimitChk(&adj_r);

        plyr_wrk.move_box.rot[1] = adj_r + tmp_rot;
    }
}

void DoorMoveStart(u_char mtn_no, u_char door_close_flg)
{
    int i;
    int j;
    AUTO_CLOSE_DOOR_WRK *acp;
    NON_CLOSE_DOOR_WRK *ncp;
    DOOR_MTN_DAT *dmotionp;
    DOJ_SQUARE_MTN *dsmtnp;
    DOOR_WRK *dwp;

    dsmtnp = &doj_square_mtn;

    dmotionp = GetDoorMtnDat(mtn_no);

    for (i = 0; i < 2; i++)
    {
        if (dsmtnp->dojs[i].dwrk_no == 0xffffffff)
        {
            continue;
        }

        if (door_close_flg != 0)
        {
            if (dsmtnp->exc_flg != 0 && dsmtnp->dbl_flg == 0)
            {
                if (i == 0)
                {
                    door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no = dmotionp->c_dmd_no1;
                }
                else
                {
                    door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no = dmotionp->c_dmd_no2;
                }
            }
            else if (dsmtnp->exc_flg == i)
            {
                door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no = dmotionp->c_dmd_no1;
            }
            else
            {
                if (MotionIsDouble(mtn_no) != 0)
                {
                    door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no = dmotionp->c_dmd_no2;
                }
                else
                {
                    door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no = dmotionp->c_dmd_no1;
                }
            }

        }
        else
        {
            if (dsmtnp->exc_flg != 0 && dsmtnp->dbl_flg == 0)
            {
                if (i == 0)
                {
                    door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no = dmotionp->dmd_no1;
                }
                else
                {
                    door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no = dmotionp->dmd_no2;
                }
            }
            else if (dsmtnp->exc_flg == i)
            {
                door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no = dmotionp->dmd_no1;
            }
            else
            {
                if (MotionIsDouble(mtn_no) != 0)
                {
                    door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no = dmotionp->dmd_no2;
                }
                else
                {
                    door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no = dmotionp->dmd_no1;
                }
            }
        }

        for (j = 0, ncp = nc_door_wrk; j < 50; j++)
        {
            if (ncp->door_id == door_wrk[dsmtnp->dojs[i].dwrk_no].door_id && ncp->stts != DOOR_STTS_NO_USE)
            {
                ncp->type = door_wrk[dsmtnp->dojs[i].dwrk_no].type;
                ncp->npos[0] = 0.0f;
                ncp->npos[1] = 0.0f;
                ncp->npos[2] = 0.0f;
                ncp->nrot = 0.0f;
                ncp->dw_no = dsmtnp->dojs[i].dwrk_no;
                ncp->stts = DOOR_STTS_OPENMTN;
                ncp->opnmtn_flg = 1;
                ncp->dmd_no = door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no;

                door_wrk[dsmtnp->dojs[i].dwrk_no].stts = DOOR_STTS_OPENMTN;

                ncp->open_tbl = dtbl_tbl[door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no];

                if (IsFusumaType(ncp->type) != 0)
                {
                    SeStartPos(30, door_wrk[dsmtnp->dojs[i].dwrk_no].pos, 0, 0x1000, 0x1000, 0xff);
                }
                else
                {
                    SeStartPos(28, door_wrk[dsmtnp->dojs[i].dwrk_no].pos, 0, 0x1000, 0x1000, 0xff);
                }

                break;
            }

            ncp++;
        }

        if (j >= 50)
        {
            do { // HACK: fix instruction ordering
            u_int n = 1; // HACK: fix instruction ordering

            acp = ac_door_wrk;

            acp->door_id = door_wrk[dsmtnp->dojs[i].dwrk_no].door_id;
            acp->type = door_wrk[dsmtnp->dojs[i].dwrk_no].type;
            acp->npos[0] = acp->npos[n] = acp->npos[2] = 0.0f;
            acp->nrot = 0.0f;
            acp->dw_no = dsmtnp->dojs[i].dwrk_no;
            acp->stts = DOOR_STTS_MANUAL_OPEN;
            acp->opnmtn_flg = 1;
            acp->dmd_no = door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no;

            door_wrk[dsmtnp->dojs[i].dwrk_no].stts = DOOR_STTS_MANUAL_OPEN;

            acp->open_tbl = dtbl_tbl[door_wrk[dsmtnp->dojs[i].dwrk_no].dmd_no];

            dwp = door_wrk; // HACK: fix block merging for below if/else

            if (IsFusumaType(ac_door_wrk->type) != 0)
            {
                SeStartPos(30, dwp[dsmtnp->dojs[i].dwrk_no].pos, 0, 0x1000, 0x1000, 0xff);
            }
            else
            {
                SeStartPos(28, dwp[dsmtnp->dojs[i].dwrk_no].pos, 0, 0x1000, 0x1000, 0xff);
            }
            } while(0); // HACK: fix instruction ordering
        }
    }
}

void OneRoomLoadReq()
{
    int i;
    int dr_num;
    int dwrk_no;
    int load_room;
    u_char *addr_uc;

    for (i = 0; i < 60; i++)
    {
        if (furn_wrk[i].room_id != room_wrk.disp_no[0])
        {
            if (furn_wrk[i].use == 0 || furn_wrk[i].use == 2)
            {
                FurnSetWrkNoUse(&furn_wrk[i], i);
            }
        }
    }

    DoorFreeFurnWrk(0);
    FurnSortFurnWrk(0);

    if (doj_square_mtn.dojs[0].type != 0xff)
    {
        dwrk_no = doj_square_mtn.dojs[0].dwrk_no;
    }
    else if (doj_square_mtn.dojs[1].type != 0xff)
    {
        dwrk_no = doj_square_mtn.dojs[1].dwrk_no;
    }

    addr_uc = GetDoorRoomConectDataP(area_read_wrk.rm_from);

    dr_num = *addr_uc;
    addr_uc += 2;

    for (i = 0; i < dr_num; i++)
    {
        if (*(u_short *)addr_uc == door_wrk[dwrk_no].door_id)
        {
            load_room = addr_uc[2];

            break;
        }

        addr_uc += 4;
    }

    for (i = 0; i < 2; i++)
    {
        if (load_room == area_wrk.room[i])
        {
            return;
        }
    }

    for (i = 0; i < 2; i++)
    {
        if (plyr_wrk.pr_info.room_no != area_wrk.room[i])
        {
            RoomMdlLoadReq(NULL, i, ingame_wrk.msn_no, load_room, 0);
            FloatGhostAppearTypeSet(ap_wrk.fgst_no, i, load_room);

            area_wrk.room[i] = load_room;

            return;
        }
    }
}

void PrintDoorID(int no)
{
    return;
}
