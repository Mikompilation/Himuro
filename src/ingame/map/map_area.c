#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "map_area.h"

#include "graphics/graph3d/load3d.h"
#include "ingame/entry/ap_fgost.h"
#include "ingame/ig_glob.h"
#include "ingame/map/door_ctl.h"
#include "ingame/map/furn_ctl.h"
#include "ingame/map/map_area.h"
#include "main/glob.h"

u_char area_dat[11][6] = {
    {   0,   1,   2,   3,   8,  38 },
    {   4,   5,   6,  14, 255, 255 },
    {   9, 255, 255, 255, 255, 255 },
    {   7,  40,  15,  16,  22,  27 },
    {  11,  12,  13, 255, 255, 255 },
    {  17,  21,  23,  25,  26, 255 },
    {   8,  20,  35,  38, 255, 255 },
    {  14,  18, 255, 255, 255, 255 },
    {  24, 255, 255, 255, 255, 255 },
    {  19,  28,  29,  30,  31, 255 },
    {  32,  33,  34, 255, 255, 255 },
};
AREA_WRK area_wrk = {0};

u_char one_room_load_ok = 0;

#define ROOM_DATA_ADDRESS 0x14b0000
#define ROOM_DATA_SIZE 0x2f0000

void MapAreaInit()
{
    int i;

    area_wrk = (AREA_WRK){0};

    for (i = 0; i <= 5; i++)
    {
        area_wrk.room[i] = 0xff;
    }

    for (i = 0; i <= 4; i++)
    {
        area_wrk.rgst[i] = 0xff;
    }

    area_wrk.fg_max = 1;
}

void MapAreaMain()
{
    area_wrk.area_bak = area_wrk.area_no;

    switch(area_read_wrk.stat)
    {
    case 0:
        // do nothing ...
    break;
    case 1:
        area_read_wrk.next_area = GetRoomArea(area_read_wrk.rm_to);
        area_read_wrk.stat = 0x4;
    break;
    case 5:
        if (FloatGhostLoadMain() == 0)
        {
            break;
        }
    case 4:
        area_read_wrk.stat = 0x8;
    break;
    case 8:
        ingame_wrk.mode = INGAME_MODE_NOMAL;

        area_read_wrk.stat = 0x0;
    break;
    }

    area_wrk.area_no = GetRoomArea(plyr_wrk.pr_info.room_no);
}

int MapAreaRoomLoadReq()
{
    while (area_read_wrk.load_count < 6)
    {
        if (area_read_wrk.room[area_read_wrk.load_count] != 0xff)
        {
            RoomMdlLoadReq(NULL, area_read_wrk.load_count, ingame_wrk.msn_no, area_read_wrk.room[area_read_wrk.load_count], 0x0);

            area_wrk.room[area_read_wrk.load_count] = area_read_wrk.room[area_read_wrk.load_count];

            area_read_wrk.load_mode = 1;
            area_read_wrk.load_count++;

            return 0;
        }

        area_read_wrk.load_count++;
    }

    return 1;
}

void MapAreaLoadSet()
{
    area_read_wrk.next_area = GetRoomArea(area_read_wrk.rm_to);

    SetAreaLoadRoom(0,area_read_wrk.next_area);
    FloatGhostEntrySet(area_read_wrk.next_area);
}

u_char GetRoomArea(u_char room_id)
{
    int i;
    int j;

    for (i = 0; i < 11; i++)
    {
        for (j = 0; j < 6; j++)
        {
            if (area_dat[i][j] == room_id)
            {
                return i;
            }
        }
    }

    return 0xff;
}

int SetAreaLoadRoom(u_char mode, u_char area_no)
{
    int i;
    int j;
    int count;
    int tmp0[6];
    int tmp1[6];
    int tmp2[6];
    int tmp3;
    int n;

    for (i = 0; i < 6; i++)
    {
        tmp0[i] = 0xff;
        tmp1[i] = 0xff;
        tmp2[i] = 0xff;

        area_read_wrk.room[i] = 0xff;
    }

    count = 0;

    for (i = 0; i < 6; i++)
    {
        for (j = 0; j < 6; j++)
        {
            if (area_wrk.room[i] == area_dat[area_no][j])
            {
                tmp0[i] = area_wrk.room[i];
                tmp1[count] = j;

                count++;

                break;
            }
        }
    }

    n = 0;

    for (i = 0; i < 6; i++)
    {
        for (j = 0; j < 6; j++)
        {
            if (tmp1[j] == i)
            {
                break;
            }
        }

        if (j == 6)
        {
            tmp2[n] = area_dat[area_no][i];

            n++;
        }
    }

    for (i = 0; i < 6; i++)
    {
        if (tmp2[i] == area_read_wrk.rm_to)
        {
            if (i != 0)
            {
                tmp3 = tmp2[0];
                tmp2[0] = tmp2[i];
                tmp2[i] = tmp3;
            }

            break;
        }
    }

    if (mode == 0)
    {
        for (i = 0; i < 6; i++)
        {
            if (area_wrk.room[i] == area_read_wrk.rm_from)
            {
                tmp0[i] = area_read_wrk.rm_from;

                break;
            }
        }
    }

    n = 0;

    for (i = 0; i < 6; i++)
    {
        if (tmp0[i] == 0xff)
        {
            area_read_wrk.room[i] = tmp2[n];

            if (area_read_wrk.room[i] != 0xff)
            {
                area_wrk.room[i] = area_read_wrk.room[i];
            }

            n++;
        }
    }

    for (i = 0; i < 6; i++)
    {
        // debug code?
    }

    for (i = 0; i < 6; i++)
    {
        // debug code?
    }

    return count;
}

void AreaRoomAllLoadInit()
{
    int i;

    for (i = 0; i < 6; i++)
    {
        area_wrk.room[i] = 0xFF;
    }
}

int AreaRoomAllLoad(u_char area_no)
{
    int i;

    if (area_read_wrk.load_mode != 0)
    {
        if (RoomMdlLoadWait() == 0)
        {
            return 0;
        }

        area_read_wrk.load_mode = 0;
    }
    else if (area_read_wrk.load_count >= 6 || area_dat[area_no][area_read_wrk.load_count] == 0xff)
    {
        area_read_wrk.load_mode = 0;
        area_read_wrk.load_count = 0;

        return 1;
    }
    else
    {
        RoomMdlLoadReq(NULL, area_read_wrk.load_count, ingame_wrk.msn_no, area_dat[area_no][area_read_wrk.load_count], 0);

        area_wrk.room[i] = area_dat[area_no][area_read_wrk.load_count];

        area_read_wrk.load_mode = 1;
        area_read_wrk.load_count++;
    }

    return 0;
}

int GetEmptyRoomAddr()
{
    int i;

    if (sys_wrk.game_mode != GAME_MODE_INGAME)
    {
        return ROOM_DATA_ADDRESS;
    }

    for (i = 0; i < 2; i++)
    {
        if (plyr_wrk.pr_info.room_no != area_wrk.room[i])
        {
            return i * ROOM_DATA_SIZE + ROOM_DATA_ADDRESS;
        }
    }

    return 0;
}

void ClearDispRoom(int wrk_no)
{
    DOOR_WRK *dwp;
    int i;
    u_char room_no;

    room_wrk.disp_no[wrk_no] = 0xff;

    for (i = 0; i < 60; i++)
    {
        if (furn_wrk[i].room_id != room_wrk.disp_no[wrk_no ^ 1] && (furn_wrk[i].use == 0 || furn_wrk[i].use == 2))
        {
            FurnSetWrkNoUse(&furn_wrk[i], i);
        }
    }

    dwp = door_wrk;

    for (i = 0; i < 20; i++, dwp++)
    {
        if (dwp->use == 1 || dwp->use == 4 || dwp->use == 3)
        {
            room_no = GetRoomIdBeyondDoor(dwp->door_id, room_wrk.disp_no[wrk_no ^ 1]);

            if (room_no != 0xff && room_no != room_wrk.disp_no[wrk_no ^ 1] && (dwp->stts != 0 && dwp->stts != 1))
            {
                DoorSttsChange(dwp->door_id, 1);
            }
        }
    }

    DoorFreeFurnWrk(0);
    FurnSortFurnWrk(0);

    for (i = 0; i < 2; i++)
    {
        if (plyr_wrk.pr_info.room_no != area_wrk.room[i])
        {
            area_wrk.room[i] = 0xff;

            break;
        }
    }
}
