#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "se_event.h"

#include "os/eeiop/eese.h"
#include "os/eeiop/sd_room.h"
#include "ingame/map/door_ctl.h"
// #include "ingame/map/map_ctrl.h"
#include "main/glob.h"

static void SEventNoUse(u_char room_id0, u_char room_id1);
static void SEventSetLoadTbl(u_int *ld_tbl, u_char room_id);

static void SEventNoUse(u_char room_id0, u_char room_id1)
{
    u_int *seep;
    int i;
    int j;
    u_char room_id[2];
    u_char exist_flg;
    
    seep = se_ctrl.event_no;
    
    room_id[0] = room_id0;
    room_id[1] = room_id1;
    
    for (i = 0; i < 2; i++, seep++)
    {
        if (*seep != 0xffffffff)
        {
            exist_flg = 0;
            
            for (j = 0; j < 2; j++)
            {
                if (room_id[j] != 0xff && *seep == GetSdrEventSe(room_id[j]))
                {
                    exist_flg = 1;
                    break;
                }
            }
            
            if (!exist_flg)
            {
                *seep = 0xffffffff;
            }
        }
    }
}

static void SEventSetLoadTbl(u_int *ld_tbl, u_char room_id)
{
    u_int load_file_no;
    int j;
    u_char vflg;
    
    *ld_tbl = 0xffffffff;
    
    load_file_no = GetSdrEventSe(room_id);
        
    if (load_file_no != 0xffffffff)
    {
        vflg = 0;
        
        for (j = 0; j < 2; j++)
        {
            if (se_ctrl.event_no[j] == load_file_no)
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

int SEventLoadReqAndSetSub(int load_id, u_char room_from, u_char room_to)
{
    u_int load_file_no;
    u_char empty_tbl[2];
    int i;
    int j;
    
    SEventNoUse(room_from, room_to);
    SEventSetLoadTbl(&load_file_no, room_to);
    
    for (i = 0; i < 2; i++)
    {
        if (se_ctrl.event_no[i] != 0xffffffff)
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
                load_id = SeFileLoadAndSet(load_file_no, j + 19);
                empty_tbl[j] = 1;
                break;
            }
        }
    }
    
    return load_id;
}

int SEventLoadReqAndSet(int load_id, u_short door_id)
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
            load_id = SEventLoadReqAndSetSub(load_id, room_from, room_to);
        }
        else
        {
            return load_id;
        }
    }

    return load_id;
}

u_char SEventGetSeNo(int file_no)
{
    int i;
    
    for (i = 0; i < 2; i++)
    {
        if (se_ctrl.event_no[i] == file_no)
        {
            return i;
        }
    }
    
    return 0xff;
}
