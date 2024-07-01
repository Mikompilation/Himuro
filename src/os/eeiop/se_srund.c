#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "se_srund.h"

#include "os/eeiop/eese.h"
#include "os/eeiop/sd_room.h"
#include "ingame/map/door_ctl.h"
// #include "ingame/map/map_ctrl.h"
#include "main/glob.h"

static void SSrundNoUse(u_char room_id0, u_char room_id1);
static void SSrundSetLoadTbl(u_int *ld_tbl, u_char room_id);

static void SSrundNoUse(u_char room_id0, u_char room_id1)
{
    u_int *sesp;
    int i;
    int j;
    u_char room_id[2];
    u_char exist_flg;
    
    sesp = se_ctrl.srund_no;
    
    room_id[0] = room_id0;
    room_id[1] = room_id1;
    
    for (i = 0; i < 2; i++, sesp++)
    {
        if (*sesp != 0xffffffff)
        {
            exist_flg = 0;
            
            for (j = 0; j < 2; j++)
            {
                if (room_id[j] != 0xff && *sesp == GetSdrSrundSe(room_id[j]))
                {
                    exist_flg = 1;
                    break;
                }
            }
            
            if (!exist_flg)
            {
                *sesp = 0xffffffff;
            }
        }
    }
}

static void SSrundSetLoadTbl(u_int *ld_tbl, u_char room_id)
{
    u_int load_file_no;
    int j;
    u_char vflg;
    
    *ld_tbl = 0xffffffff;
    
    load_file_no = GetSdrSrundSe(room_id);
        
    if (load_file_no != 0xffffffff)
    {
        vflg = 0;
        
        for (j = 0; j < 2; j++)
        {
            if (se_ctrl.srund_no[j] == load_file_no)
            {
                vflg = 1;
                break;
            }
        }
        
        if (!vflg)
        {
            *ld_tbl = load_file_no;
        }
    }
}

int SSrundLoadReqAndSetSub(int load_id, u_char room_from, u_char room_to)
{
    u_int load_file_no;
    u_char empty_tbl[2];
    int i;
    int j;
    
    SSrundNoUse(room_from, room_to);
    SSrundSetLoadTbl(&load_file_no, room_to);
    
    for (i = 0; i < 2; i++)
    {
        if (se_ctrl.srund_no[i] != 0xffffffff)
        {
            empty_tbl[i] = 1;
        }
        else
        {
            empty_tbl[i] = 0;
        }
    }
    
    if (load_file_no != 0xffffffff)
    {
        for (j = 0; j < 2; j++)
        {
            if (empty_tbl[j] == 0)
            {
                load_id = SeFileLoadAndSet(load_file_no, j + 14);
                empty_tbl[j] = 1;
                break;
            }
        }
    }
    
    return load_id;
}

int SSrundLoadReqAndSet(int load_id, u_short door_id)
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
            load_id = SSrundLoadReqAndSetSub(load_id, room_from, room_to);
        }
        else
        {
            return load_id;
        }
    }
    
    return load_id;
}

u_char SSrundGetSeNo(int file_no)
{
    int i;
    
    for (i = 0; i < 2; i++)
    {
        if (se_ctrl.srund_no[i] == file_no)
        {
            return i;
        }
    }
    
    return 0xff;
}
