#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "se_door.h"

#include "main/glob.h"
#include "os/eeiop/eese.h"
#include "os/eeiop/sd_room.h"
#include "ingame/map/doortype.h"
#include "ingame/map/door_ctl.h"
// #include "ingame/map/map_ctrl.h"

static void SDoorNoUse(u_char room_id0, u_char room_id1);
static void SDoorSetLoadTbl(u_int *ld_tbl, u_char room_id);
static u_char SDoorGetSeNo(int file_no, u_char shut);

static void SDoorNoUse(u_char room_id0, u_char room_id1)
{
    u_int *dp;
    u_int *sedp;
    int i;
    int j; // looks like gcc optimized this out by using the stack address (sp) 
    int k;
    u_char room_id[2];
    u_char exist_flg;
    
    sedp = se_ctrl.door_no;
    
    room_id[0] = room_id0;
    room_id[1] = room_id1;
    
    for (i = 0; i < 3; i++, sedp++)
    {
        if (*sedp != 0xffffffff)
        {
            exist_flg = 0;
            
            for (j = 0; j < 2; j++)
            {
                if (room_id[j] != 0xff)
                {
                    for (k = 0, dp = GetSdrDoorSeTblP(room_id[j]); k < 3; k++, dp++)
                    {
                        if (*sedp == *dp)
                        {
                            exist_flg = 1;
                            goto found;
                        }
                    }
                }
            }
            
            found:
            if (!exist_flg)
            {
                *sedp = 0xffffffff;
            }
        }
    }
}

static void SDoorSetLoadTbl(u_int *ld_tbl, u_char room_id)
{
    u_int *idp;
    int i;
    int j;
    u_char vflg;

    for (i = 0, idp = ld_tbl; i < 3; i++, idp++)
    {
        *idp = 0xffffffff;
    }

    for (i = 0, idp = GetSdrDoorSeTblP(room_id); i < 3; i++, idp++)
    {
        vflg = 0;
        
        if (*idp != 0xffffffff)
        {
            for (j = 0; j < 3; j++)
            {
                if (se_ctrl.door_no[j] == *idp)
                {
                    vflg = 1;
                    break;
                }
            }
            
            if (!vflg)
            {
                *ld_tbl = *idp;
                ld_tbl++;
            }
        }
    }
}

int SDoorLoadReqAndSetSub(int load_id, u_char room_from, u_char room_to)
{
    u_int load_file_tbl[3];
    u_char empty_tbl[3];
    int i;
    int j;
    
    SDoorNoUse(room_from, room_to);
    SDoorSetLoadTbl(load_file_tbl, room_to);
    
    for (i = 0; i < 3; i++)
    {
        if (se_ctrl.door_no[i] != 0xffffffff)
        {
            empty_tbl[i] = 1;
        }
        else
        {
            empty_tbl[i] = 0;
        }
    }
    
    for (i = 0; i < 3; i++)
    {
        if (load_file_tbl[i] != -1)
        {
            for (j = 0; j < 3; j++)
            {
                if (empty_tbl[j] == 0)
                {
                    load_id = SeFileLoadAndSet(load_file_tbl[i], j + 3);
                    empty_tbl[j] = 1;
                    break;
                }
            }
        }
    }

    return load_id;
}


int SDoorLoadReqAndSet(int load_id, u_short door_id)
{
    u_char *addr_uc;
    int i;
    u_char room_from;
    u_char room_to;
    u_char dr_num;
    
    room_from = GetRoomIdFromRoomNo(0, room_wrk.room_no);
    addr_uc = GetDoorRoomConectDataP(room_from);

    dr_num = addr_uc[0];
    room_to = 0xff;
    
    for (i = 0, addr_uc = &addr_uc[2]; i < dr_num; i++, addr_uc = &addr_uc[4])
    {
        if (*(u_short *)addr_uc == door_id)
        {
            room_to = addr_uc[2];
            break;
        }
    }

    if (room_to != 0xff)
    {
        if (room_from != room_to)
        {
            load_id = SDoorLoadReqAndSetSub(load_id, room_from, room_to);
        }
        else
        {
            return load_id;
        }
    }

    return load_id;
}

static u_char SDoorGetSeNo(int file_no, u_char shut)
{
    int i;

    shut = shut != 0;

    for (i = 0; i < 3; i++)
    {
        if (se_ctrl.door_no[i] == file_no)
        {
            return (shut + 0x26) + (i * 2);
        }
    }
    
    return 0xff;
}

void SeDoorOpen(int dwrk_no)
{
    u_char se_no;
    
    se_no = SDoorGetSeNo(GetDoorTypeDatP(door_wrk[dwrk_no].type)->se_file, 0);

    if (se_no != 0xff)
    {
        SeStartPos(se_no, door_wrk[dwrk_no].pos, 0, 0x1000, 0x1000, 0xff);
    }
}

void SeDoorClose(int dwrk_no)
{
    u_char se_no;
    
    se_no = SDoorGetSeNo(GetDoorTypeDatP(door_wrk[dwrk_no].type)->se_file, 1);

    if (se_no != 0xff)
    {
        SeStartPos(se_no, door_wrk[dwrk_no].pos, 0, 0x1000, 0x1000, 0xff);
    }
}
