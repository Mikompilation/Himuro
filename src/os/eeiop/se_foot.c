#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "se_foot.h"

#include "os/eeiop/eese.h"
#include "os/eeiop/sd_room.h"
#include "ingame/map/door_ctl.h"
// #include "ingame/map/map_ctrl.h"
#include "main/glob.h"

static void SFootNoUse(u_char room_id0, u_char room_id1);
static void SFootSetLoadTbl(u_int *ld_tbl, u_char room_id);

static void SFootNoUse(u_char room_id0, u_char room_id1)
{
    u_int *fp;
    u_int *sedp;
    int i;
    int j; // looks like gcc optimized this out by using the stack address (sp) 
    int k;
    u_char room_id[2];
    u_char exist_flg;
    
    sedp = se_ctrl.foot_no;
    
    room_id[0] = room_id0;
    room_id[1] = room_id1;
    
    for (i = 0; i < 8; i++, sedp++)
    {
        if (*sedp != 0xffffffff)
        {
            exist_flg = 0;
            
            for (j = 0; j < 2; j++)
            {
                if (room_id[j] != 0xff)
                {
                    for (k = 0, fp = GetSdrFootSeTblP(room_id[j]); k < 5; k++, fp++)
                    {
                        if (*sedp == *fp)
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

static void SFootSetLoadTbl(u_int *ld_tbl, u_char room_id)
{
    u_int *ifp;
    int i;
    int j;
    u_char vflg;

    for (i = 0, ifp = ld_tbl; i < 5; i++, ifp++)
    {
        *ifp = 0xffffffff;
    }

    for (i = 0, ifp = GetSdrFootSeTblP(room_id); i < 5; i++, ifp++)
    {
        vflg = 0;
        
        if (*ifp != 0xffffffff)
        {
            for (j = 0; j < 8; j++)
            {
                
                if (se_ctrl.foot_no[j] == *ifp)
                {
                    vflg = 1;
                    break;
                }
            }
            
            if (!vflg)
            {
                *ld_tbl = *ifp;
                ld_tbl++;
            }
        }
    }
}

int SFootLoadReqAndSetSub(int load_id, u_char room_from, u_char room_to)
{
    u_int load_file_tbl[5];
    u_char empty_tbl[8];
    int i;
    int j;
    
    SFootNoUse(room_from, room_to);
    SFootSetLoadTbl(load_file_tbl, room_to);
    
    for (i = 0; i < 8; i++)
    {
        if (se_ctrl.foot_no[i] != 0xffffffff)
        {
            empty_tbl[i] = 1;
        }
        else
        {
            empty_tbl[i] = 0;
        }
    }
    
    for (i = 0; i < 5; i++)
    {
        if (load_file_tbl[i] != -1)
        {
            for (j = 0; j < 8; j++)
            {
                if (empty_tbl[j] == 0)
                {
                    load_id = SeFileLoadAndSet(load_file_tbl[i], j + 6);
                    empty_tbl[j] = 1;
                    break;
                }
            }
        }
    }

    return load_id;
}

int SFootLoadReqAndSet(int load_id, u_short door_id)
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
            load_id = SFootLoadReqAndSetSub(load_id, room_from, room_to);
        }
        else
        {
            return load_id;
        }
    }

    return load_id;
}

u_char SFootGetSeNo(int call_no)
{
    int i;
    int file_no;

    
    if (call_no >= 0 && call_no < 0x1c)
    {
        file_no = foot_se_index[call_no];
        
        for (i = 0; i < 8; i++)
        {
            if (se_ctrl.foot_no[i] == file_no)
            {
                return i + 0x2c;
            }
        }
    }
    
    return 0xff;
}

u_char SFootGetSeNoFromFileNo(int file_no)
{
    int i;

    int iVar1;
    u_int *puVar2;
    
    for (i = 0; i < 8; i++) 
    {
        if (se_ctrl.foot_no[i] == file_no && file_no != -1)
        {
            return i + 0x2c;
        }
    }
    
    return 0xff;
}
