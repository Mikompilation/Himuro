#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "load3d.h"

#include "main/glob.h"
#include "ingame/ig_glob.h"
#include "ingame/map/map_ctrl.h"
#include "ingame/map/door_ctl.h"
#include "ingame/map/furn_ctl.h"
#include "ingame/event/ev_main.h"
#include "os/eeiop/se_door.h"
#include "os/eeiop/se_foot.h"
#include "os/eeiop/se_event.h"
#include "os/eeiop/se_srund.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "graphics/motion/mime.h"
#include "graphics/motion/mim_dat.h"
#include "graphics/graph3d/gra3d.h"
#include "graphics/graph3d/sgsgd.h"
#include "graphics/graph3d/mirror.h"
#include "graphics/graph3d/sglight.h"

ROOM_LOAD_BLOCK room_load_block[2] = {0};
#include "data/load_furn_num.h" // static short int load_furn_num[];
#include "data/load_door_num.h" // static short int load_door_num[];
RMDL_ADDR room_addr_tbl[64] = {0};
u_int *furn_addr_tbl[500] = {NULL};
u_int *door_addr_tbl[128] = {NULL};
u_int *item_addr_tbl[48] = {NULL};

int r022_light_on = 0;

static char now_load_block;
static int room_load_num;
static int room_req_id;

static ROOM_LOAD_BLOCK backup_block;
static int room_load_id[32];

u_int* RoomMdlLoadReq(u_int *addr, u_char blk_no, u_char msn_no, u_char room_no, u_char sereq_type)
{
    ROOM_LOAD_BLOCK *rlb;
    int next_addr;
    int i;
    int file_no;
    int tmp_load_id;
    char room_from;

    rlb = &room_load_block[blk_no];

    file_no = room_no * 2 + R000_GENKAN_PK2;

    SetRenewDoorAddr(blk_no);

    if (addr == NULL)
    {
        addr = (u_int *)(blk_no * 0x2f0000 + 0x14b0000);
    }

    next_addr = (u_int)addr;

    now_load_block = blk_no;

    rlb->room_no = room_no;
    rlb->block_no = now_load_block;
    rlb->chapter_no = msn_no;

    room_load_num = 0;

    for (i = 0; i < 30; i++)
    {
        rlb->furn_id[i] = 0;
    }

    for (i = 0; i < 16; i++)
    {
        rlb->door_id[i] = 0;
        rlb->door_addr[i] = NULL;
    }

    rlb->furn_num = GetRoomFurnID(room_no, rlb->furn_id, msn_no);
    rlb->door_num = GetRoomDoorID(room_no, rlb->door_id, msn_no);

    DelSameMdlID(rlb->furn_id, (int *)&rlb->furn_num);

    for (i = 0; i < rlb->furn_num; i++)
    {
        // empty
    }

    for (i = 0; i < rlb->door_num; i++)
    {
        // empty
    }

    // could be a macro?
    next_addr = LoadReqGetAddr(file_no, next_addr, &room_load_id[room_load_num]);
    room_addr_tbl[room_no].pk2 = addr;
    addr = (u_int *)next_addr;
    room_load_num++;

    next_addr = LoadReqGetAddr(file_no + 1, next_addr, &room_load_id[room_load_num]);
    room_addr_tbl[room_no].lit_data = addr;
    addr = (u_int *)next_addr;
    room_load_num++;

    for (i = 0; i < rlb->furn_num; i++)
    {
        if (LoadFDCheck(rlb->furn_id[i], 0) == 0)
        {
            next_addr = LoadReqGetAddr(F000_CLOCK_L_SGD + rlb->furn_id[i], next_addr, &room_load_id[room_load_num]);
            furn_addr_tbl[rlb->furn_id[i]] = addr;
            addr = (u_int *)next_addr;
            room_load_num++;
        }
    }

    for (i = 0; i < rlb->door_num; i++)
    {
        if (LoadFDCheck(rlb->door_id[i], 1) == 0)
        {
            next_addr = LoadReqGetAddr(D000_GEN1_SGD + rlb->door_id[i], next_addr, &room_load_id[room_load_num]);
            rlb->door_addr[i] = addr;
            addr = (u_int *)next_addr;
            room_load_num++;
        }
    }

    if (mimchodo_tbl[rlb->room_no].dat != NULL)
    {
        next_addr = LoadReqGetAddr(R000_GENKAN_MIM + rlb->room_no, next_addr, &room_load_id[room_load_num]);
        mimchodo_tbl[rlb->room_no].addr = addr;
        addr = (u_int *)next_addr;
        room_load_num++;
    }

    tmp_load_id = -1;

    if (sys_wrk.game_mode == GAME_MODE_OUTGAME)
    {
        sereq_type = 100;
    }

    switch (sereq_type)
    {
    case 0:
    break;
    case 1:
        tmp_load_id = SDoorLoadReqAndSetSub(tmp_load_id, 255, rlb->room_no);
        tmp_load_id = SFootLoadReqAndSetSub(tmp_load_id, 255, rlb->room_no);
        tmp_load_id = SSrundLoadReqAndSetSub(tmp_load_id, 255, rlb->room_no);
        tmp_load_id = SEventLoadReqAndSetSub(tmp_load_id, 255, rlb->room_no);
    break;
    case 2:
        room_from = GetRoomIdFromRoomNoFloor(0, room_wrk.room_no, map_wrk.floor);

        tmp_load_id = SDoorLoadReqAndSetSub(tmp_load_id, room_from, rlb->room_no);
        tmp_load_id = SFootLoadReqAndSetSub(tmp_load_id, room_from, rlb->room_no);
        tmp_load_id = SSrundLoadReqAndSetSub(tmp_load_id, room_from, rlb->room_no);
        tmp_load_id = SEventLoadReqAndSetSub(tmp_load_id, room_from, rlb->room_no);
    break;
    }

    if (tmp_load_id != -1)
    {
        room_load_id[room_load_num] = tmp_load_id;
        room_load_num++;
    }

    return addr;
}

int RoomMdlLoadWait()
{
    ROOM_LOAD_BLOCK *rlb;
    FURN_WRK *fwp;
    int i;

    if (room_load_num <= 0)
    {
        return 1;
    }

    if (!IsLoadEnd(room_load_id[room_load_num-1]))
    {
        return 0;
    }

    if (now_load_block < 0)
    {
        return 1;
    }

    rlb = &room_load_block[now_load_block];

    InitializeRoom(&room_addr_tbl[rlb->room_no]);

    if (rlb->room_no == 22 && ingame_wrk.msn_no < 2)
        {
        if (event_stts[112] == 0)
        {
            SetPreRenderR022(0);
        }
        else
        {
            SetPreRenderR022(1);
        }
    }

    for (i = 0; i < rlb->furn_num; i++)
    {
        if (furn_addr_tbl[rlb->furn_id[i]] != NULL)
        {
            SgMapUnit(furn_addr_tbl[rlb->furn_id[i]]);
        }
    }

    for (i = 0; i < rlb->door_num; i++)
    {
        door_addr_tbl[rlb->door_id[i]] = rlb->door_addr[i];

        if (door_addr_tbl[rlb->door_id[i]] != NULL)
        {
            SgMapUnit(door_addr_tbl[rlb->door_id[i]]);
        }
    }

    if (sys_wrk.game_mode != GAME_MODE_OUTGAME)
    {
        fwp = furn_wrk;

        for (i = 0; i < 60; i++)
        {
            if (fwp->use == 1 || fwp->use == 4 || fwp->use == 3)
            {
                if (fwp->furn_no != 0xffff)
                {
                    ChoudoPreRender(fwp);
                }
            }

            fwp++;
        }
    }

    mimInitChodo(mimchodo_tbl[rlb->room_no].addr, rlb->furn_id, now_load_block, rlb->room_no, rlb->furn_num);

    now_load_block = -1;

    return 1;
}

void SetRenewDoorAddr(u_char blk_no)
{
    ROOM_LOAD_BLOCK *rlb;
    ROOM_LOAD_BLOCK *seek_rlb;
    int i;
    int j;
    int k;

    rlb = &room_load_block[blk_no];

    for (i = 0; i < 2; i++)
    {
        if (i != blk_no)
        {
            seek_rlb = &room_load_block[i];

            for (j = 0; j < rlb->door_num; j++)
            {
                for (k = 0; k < seek_rlb->door_num; k++)
                {
                    if (rlb->door_id[j] == seek_rlb->door_id[k])
                    {
                        door_addr_tbl[rlb->door_id[j]] = seek_rlb->door_addr[k];
                    }
                }
            }
        }
    }
}

void SetRenewDoorAddrForMovie()
{
    ROOM_LOAD_BLOCK *rlb;
    int i;

    rlb = &room_load_block[0];

    for (i = 0; i < 2; i++)
    {
        if (room_wrk.disp_no[0] == room_load_block[i].room_no)
        {
            rlb = &room_load_block[i];
            break;
        }
    }

    for (i = 0; i < rlb->door_num; i++)
    {
        if (rlb->door_addr[i] != NULL)
        {
            door_addr_tbl[rlb->door_id[i]] = rlb->door_addr[i];
        }
    }
}

void InitModelLoad()
{
    u_int i;

    for (i = 0; i < 2; i++)
    {
        room_load_block[i] = (ROOM_LOAD_BLOCK){0};
        room_load_block[i].block_no = -1;
    }

    for (i = 0; i < 64; i++)
    {
        room_addr_tbl[i].near_sgd = NULL;
        room_addr_tbl[i].far_sgd = NULL;
        room_addr_tbl[i].lit_data = NULL;
        room_addr_tbl[i].ss_sgd = NULL;
        room_addr_tbl[i].sh_sgd = NULL;
    }

    for (i = 0; i < 500; i++)
    {
        furn_addr_tbl[i] = NULL;
    }

    for (i = 0; i < 128; i++)
    {
        door_addr_tbl[i] = NULL;
    }

    r022_light_on = 0;
}

void DelSameMdlID(u_short *mdl_id, int *mdl_num)
{
    int i;
    int j;
    u_short id_tmp[30];

    for (i = 0; i < *mdl_num; i++)
    {
        id_tmp[i] = mdl_id[i];

        if (mdl_id[i] != 0xffff)
        {
            for (j = i + 1; j < *mdl_num; j++)
            {
                if (mdl_id[i] == mdl_id[j])
                {
                    mdl_id[j] = 0xffff;
                }
            }
        }
    }

    j = 0;

    for (i = 0; i < *mdl_num; i++)
    {
        if (id_tmp[i] != 0xffff)
        {
            mdl_id[j] = id_tmp[i];
            j++;
        }
    }

    *mdl_num = j;
}

void SetPreRenderR022(int flg)
{
    sceVu0FVECTOR ambient;
    SgLIGHT lights;
    SgLIGHT ilights[3];
    SgLIGHT plights[16];
    SgLIGHT slights[16];

    if (room_addr_tbl[22].near_sgd == NULL || room_addr_tbl[22].lit_data == NULL)
    {
        return;
    }

    SgReadLights(room_addr_tbl[22].near_sgd, room_addr_tbl[22].lit_data, ambient, ilights, 3, plights, 0x10, slights, 0x10);

    if (flg == 0)
    {
        plights[0].diffuse[0] = 0.0f;
        plights[0].diffuse[1] = 0.0f;
        plights[0].diffuse[2] = 0.0f;
        plights[0].diffuse[3] = 0.0f;

        plights[1].diffuse[0] = 0.0f;
        plights[1].diffuse[1] = 0.0f;
        plights[1].diffuse[2] = 0.0f;
        plights[1].diffuse[3] = 0.0f;

        plights[2].diffuse[0] = 0.0f;
        plights[2].diffuse[1] = 0.0f;
        plights[2].diffuse[2] = 0.0f;
        plights[2].diffuse[3] = 0.0f;
    }

    SgSetAmbient(ambient);
    SgSetInfiniteLights(camera.zd, ilights, ilights[0].num);
    SgSetPointLights(plights, plights[0].num);
    SgSetSpotLights(slights, slights[0].num);
    SgClearPreRender(room_addr_tbl[22].near_sgd, -1);
    SgPreRender(room_addr_tbl[22].near_sgd, -1);
    SgClearPreRender(room_addr_tbl[22].ss_sgd, -1);
    SgPreRender(room_addr_tbl[22].ss_sgd, -1);
}

static int LoadFDCheck(u_short id, char flg)
{
    short int *mdl_num;

    if (flg == 0)
    {
        mdl_num = (short*)&load_furn_num;
    }
    else
    {
        mdl_num = (short*)&load_door_num;
    }

    while (1)
    {
        if (id == *mdl_num)
        {
            return 0;
        }

        if (*mdl_num == -1)
        {
            break;
        }

        mdl_num++;
    }

    return 1;
}

void LoadItemModel(u_int *tmpp)
{
    int i;

    for (i = 47; i > -1; i--)
    {
        item_addr_tbl[i] = NULL;
    }
}

void ItemLoadAfterInit(int item_no, int addr)
{
    void *sgd_top = (void *)addr;

    item_addr_tbl[item_no] = sgd_top;

    SgMapUnit(sgd_top);
}

int InitializeRoomUnder(RMDL_ADDR *room_tbl)
{
    static int init_st = 0;
    u_int pack_num;
    u_int *prim;
    int ret;

    ret = 0;

    switch (init_st)
    {
    case 0:
        if (IsLoadEnd(room_req_id) != 0)
        {
            prim = room_tbl->pk2;

            pack_num = *prim;

            prim += 4;

            room_tbl->near_sgd = prim + 4;

            prim += (*prim / 4) + 4;

            room_tbl->far_sgd = prim + 4;

            prim += (*prim / 4) + 4;

            if (pack_num == 4)
            {
                room_tbl->ss_sgd = prim + 4;

                prim += (*prim / 4) + 4;

                room_tbl->sh_sgd = prim + 4;
            }
            else
            {
                room_tbl->ss_sgd = NULL;
                room_tbl->sh_sgd = NULL;
            }

            SgMapUnit(room_tbl->near_sgd);

            init_st = 1;
        }

    break;
    case 1:
        SgMapUnit(room_tbl->far_sgd);

        init_st = 2;
    break;
    case 2:
        SgMapUnit(room_tbl->lit_data);

        init_st = 3;

        room_tbl->far_sgd[2] = room_tbl->near_sgd[2];
    break;
    case 3:
        SetPreRender(room_tbl->near_sgd,room_tbl->lit_data);

        if (CheckMirrorModel(room_tbl->near_sgd) != 0)
        {
            ((u_char*)room_tbl->near_sgd)[5] |= 0x2;
        }

        init_st = 4;
    break;
    case 4:
        if (room_tbl->ss_sgd != NULL)
        {
            SgMapUnit(room_tbl->ss_sgd);

            SetPreRender(room_tbl->ss_sgd,room_tbl->lit_data);
        }

        init_st = 5;
    break;
    case 5:
        if (room_tbl->sh_sgd != NULL)
        {
            SgMapUnit(room_tbl->sh_sgd);
        }

        init_st = 0;

        ret = 1;
    break;
    }

    return ret;
}

int LoadInitFurnModel(ROOM_LOAD_BLOCK *rlb)
{
    static int init_st = 0;
    static int load_cnt = 0;
    static int load_id;
    int ret;
    u_short furn_id;
    u_int addr;

    ret = 0;

    if (rlb->furn_num == 0)
    {
        return 1;
    }

    switch (init_st)
    {
    case 0:
        furn_id = rlb->furn_id[load_cnt];

        LoadFDCheck(furn_id, 0);

        addr = LoadReqGetAddr(F000_CLOCK_L_SGD + furn_id, (u_int)rlb->load_addr, &load_id);

        furn_addr_tbl[furn_id] = rlb->load_addr;

        rlb->load_addr = (u_int *)addr;

        init_st = 1;
    break;
    case 1:
        if (IsLoadEnd(load_id) != 0)
        {
            furn_id = rlb->furn_id[load_cnt];

            if (furn_addr_tbl[furn_id] != NULL)
            {
                SgMapUnit(furn_addr_tbl[furn_id]);
            }

            load_cnt++;

            init_st = 0;

            if (rlb->furn_num <= load_cnt)
            {
                load_cnt = 0;

                ret = 1;
            }
        }
    break;
    }

    return ret;
}

int LoadInitDoorModel(ROOM_LOAD_BLOCK *rlb)
{
    static int init_st = 0;
    static int load_cnt = 0;
    static int load_id;
    FURN_WRK *fwp;
    int i;
    int ret;
    u_short door_id;

    // the following var is not from the debugging symbols
    u_int addr;

    ret = 0;

    if (rlb->door_num == 0)
    {
        return 1;
    }

    switch (init_st)
    {
    case 0:
        door_id = rlb->door_id[load_cnt];

        LoadFDCheck(door_id, 1);

        addr = LoadReqGetAddr(D000_GEN1_SGD + door_id, (u_int)rlb->load_addr, &load_id);

        door_addr_tbl[door_id] = rlb->load_addr;

        rlb->door_addr[load_cnt] = rlb->load_addr;

        rlb->load_addr = (u_int*)addr;

        init_st = 1;
    break;
    case 1:
        if (IsLoadEnd(load_id) != 0)
        {
            door_id = rlb->door_id[load_cnt];

            if (door_addr_tbl[door_id] != NULL)
            {
                SgMapUnit(door_addr_tbl[door_id]);
            }

            load_cnt++;

            init_st = 0;

            if (rlb->door_num <= load_cnt)
            {
                load_cnt = 0;

                init_st = 2;
            }
        }

    break;
    case 2:
        fwp = furn_wrk;

        for (i = 0; i < 60; i++)
        {
            if (fwp->use == 1 || fwp->use == 4 || fwp->use == 3)
            {
                if (fwp->furn_no != 0xFFFF)
                {
                    ChoudoPreRender(fwp);
                }
            }

            fwp++;
        }

        load_cnt = 0;

        init_st = 0;

        ret = 1;
    break;
    }

    return ret;
}
