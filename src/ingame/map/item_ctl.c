#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "item_ctl.h"

#include "graphics/graph2d/effect.h"
#include "ingame/event/ev_main.h"
#include "ingame/ig_glob.h"
#include "ingame/map/find_ctl.h"
#include "ingame/map/item_ctl.h"
#include "ingame/menu/item.h"
#include "ingame/menu/item_get.h"
#include "ingame/plyr/unit_ctl.h"
#include "main/glob.h"

#include "data/map_item_dat.h" // MAP_ITEM_DAT map_item_dat[162];
u_char map_item_ap_type[][2] = {
    {1, 0}, {2, 0}, {2, 3}
};
#include "data/map_file_dat0.h" // MAP_FILE_DAT map_file_dat0[50];
#include "data/map_file_dat1.h" // MAP_FILE_DAT map_file_dat1[50];
#include "data/map_file_dat2.h" // MAP_FILE_DAT map_file_dat2[50];
#include "data/map_file_dat3.h" // MAP_FILE_DAT map_file_dat3[50];
MAP_FILE_DAT *map_file_dat[4] = {
    map_file_dat0, map_file_dat1,
    map_file_dat2, map_file_dat3,
};
u_short item_ap[300][2] = {0};

#define PI 3.1415927f

void MapItemInit()
{
    int i;

    for (i = 0; i < 162; i++)
    {
        if (ingame_wrk.game == 1)
        {
            item_ap[i][0] = 1;
            item_ap[i][1] = 1;
        }
        else if ((map_item_dat[i].stts & 0x1) == 0)
        {
            item_ap[i][0] = 0;
            item_ap[i][1] = 0;
        }
        else
        {
            item_ap[i][0] = 1;
            item_ap[i][1] = 1;
        }
    }
}

void MissionStartMapItemInit(int msn_no)
{
    int i;

    for (i = 0; i < 162; i++)
    {
        if ((map_item_dat[i].msn_no & 0x80) != 0)
        {
            if (msn_no == (map_item_dat[i].msn_no & 0x7f))
            {
                if ((map_item_dat[i].stts & 1) == 0)
                {
                    item_ap[i][0] = 0;
                    item_ap[i][1] = 0;
                }
                else
                {
                    item_ap[i][0] = 1;
                    item_ap[i][1] = 1;
                }
            }
            else
            {
                item_ap[i][0] = 1;
                item_ap[i][1] = 1;
            }
        }
        else if (msn_no == map_item_dat[i].msn_no)
        {
            item_ap[i][0] = 0;
            item_ap[i][1] = 0;
        }
    }

    for (i = 0; i < 50; i++)
    {
        if (map_file_dat0[i].msn_no == msn_no)
        {
            if (ingame_wrk.clear_count == 0)
            {
                poss_file[0][i] = map_file_dat0[i].stts;
            }
            else if (poss_file[0][i] < 2)
            {
                poss_file[0][i] = map_file_dat0[i].stts;
            }
        }
        else
        {
            if (poss_file[0][i] == 1)
            {
                poss_file[0][i] = 0;
            }
        }
    }

    for (i = 0; i < 50; i++)
    {
        if (map_file_dat1[i].msn_no == msn_no)
        {
            if (ingame_wrk.clear_count == 0)
            {
                poss_file[1][i] = map_file_dat1[i].stts;
            }
        }
        else
        {
            if (poss_file[1][i] == 1)
            {
                poss_file[1][i] = 0;
            }
        }
    }

    for (i = 0; i < 50; i++)
    {
        if (map_file_dat2[i].msn_no == msn_no)
        {
            if (ingame_wrk.clear_count == 0x0)
            {
                poss_file[2][i]  = map_file_dat2[i].stts;
            }
            else if (poss_file[2][i] < 2)
            {
                poss_file[2][i]  = map_file_dat2[i].stts;
            }
        }
        else
        {
            if (poss_file[2][i] == 1)
            {
                poss_file[2][i] = 0;
            }
        }
    }

    for (i = 0; i < 50; i++)
    {
        if (map_file_dat3[i].msn_no == msn_no)
        {
            if (ingame_wrk.clear_count == 0)
            {
                poss_file[3][i] = map_file_dat3[i].stts;
            }
        }
        else
        {
            if (poss_file[3][i] == 1)
            {
                poss_file[3][i] = 0;
            }
            else if (poss_file[3][i] < 2)
            {
                poss_file[3][i] = map_file_dat3[i].stts;
            }
        }
    }
}

void FileDateInit()
{
    int i;

    for (i = 0; i < 50; i++)
    {
        poss_file[0][i] = map_file_dat0[i].stts;
    }

    for (i = 0; i < 50; i++)
    {
        poss_file[1][i] = map_file_dat1[i].stts;
    }

    for (i = 0; i < 50; i++)
    {
        poss_file[2][i] = map_file_dat2[i].stts;
    }

    for (i = 0; i < 50; i++)
    {
        poss_file[3][i] = map_file_dat3[i].stts;
    }
}

int ItemGetMain()
{
    int get_item;

    get_item = 0xffff;

    if (
        ingame_wrk.mode == INGAME_MODE_NOMAL && plyr_wrk.mode == PMODE_NORMAL &&
        (ingame_wrk.stts & 0x80) == 0 && (plyr_wrk.sta & 0x8) == 0 &&
        find_wrk.mode == 0 && *key_now[5] == 1
    )
    {
        get_item = GetMapItemJudge();
    }

    if (get_item != 0xffff)
    {
        GetMapItem(get_item);

        return 1;
    }

    return 0;
}

void ItemCtrl()
{
    if (ev_wrk.get_item == 0)
    {
        ItemDispCtrl();
        FilmPossessionExp();
    }
    else
    {
        plyr_wrk.mode = PMODE_MSG_DISP;

        eff_blur_off = 1;
    }
}

void ItemDispCtrl()
{
    int i;

    for (i = 0; i < 2; i++)
    {
        if (room_wrk.disp_no[i] != room_wrk.disp_bak[i])
        {
            SetItemDispData(i);
        }
    }
}

void SetItemDispData(u_char id)
{
    int i;
    int j;
    int dsp_no;
    int item_no;

    if (id >= 2)
    {
        return;
    }

    for (i = 0; i < 16; i++)
    {
        item_dsp_wrk[id][i].item_no = -1;
    }

    if (room_wrk.disp_no[id] == 0xff)
    {
        return;
    }

    for (i = 0, dsp_no = 0; i < 162; i++)
    {
        if (item_ap[i][0] == 0 && item_ap[i][1] == 0)
        {
            if (map_item_dat[i].room == room_wrk.disp_no[id])
            {
                if ((map_item_dat[i].stts & 2) != 0)
                {
                    item_dsp_wrk[id][dsp_no].disp = 0;
                }
                else
                {
                    item_dsp_wrk[id][dsp_no].disp = 1;
                }

                item_dsp_wrk[id][dsp_no].item_no = i;

                item_dsp_wrk[id][dsp_no].pos[0] = map_item_dat[i].pos_x + room_wrk.pos[id][0];
                item_dsp_wrk[id][dsp_no].pos[1] = map_item_dat[i].pos_y + room_wrk.pos[id][1];
                item_dsp_wrk[id][dsp_no].pos[2] = map_item_dat[i].pos_z + room_wrk.pos[id][2];

                dsp_no++;

                if (dsp_no > 15)
                {
                    return;
                }
            }
        }
    }

    for (i = 0; i < 4; i++)
    {
        item_no = i * 0x2000 - 0x8000;

        for (j = 0; j < 50; item_no++, j++)
        {
            if (poss_file[i][j] == 1)
            {
                if (map_file_dat[i][j].room == room_wrk.disp_no[id])
                {
                    item_dsp_wrk[id][dsp_no].disp = map_file_dat[i][j].map_dsp;

                    item_dsp_wrk[id][dsp_no].item_no = item_no;

                    item_dsp_wrk[id][dsp_no].pos[0] = map_file_dat[i][j].pos_x + room_wrk.pos[id][0];
                    item_dsp_wrk[id][dsp_no].pos[1] = map_file_dat[i][j].pos_y + room_wrk.pos[id][1];
                    item_dsp_wrk[id][dsp_no].pos[2] = map_file_dat[i][j].pos_z + room_wrk.pos[id][2];

                    dsp_no++;

                    if (dsp_no > 15)
                    {
                        return;
                    }
                }
            }
        }
    }
}

void ItemDispDataRenew()
{
    int i;

    for (i = 0; i < 2; i++)
    {
        SetItemDispData(i);
    }
}

int GetMapItemJudge()
{
    int i;
    int j;

    for (i = 0; i < 2; i++)
    {
        if (room_wrk.disp_no[i] != 0xff)
        {
            for (j = 0; j < 16; j++) {
                if (item_dsp_wrk[i][j].item_no < 0x8000 && map_item_dat[item_dsp_wrk[i][j].item_no].stts & 0x80)
                {
                    continue;
                }

                if (item_dsp_wrk[i][j].item_no == 0xffff)
                {
                    continue;
                }

                if (
                    item_dsp_wrk[i][j].pos[1] < (int)(plyr_wrk.move_box.pos[1] + 500.0f) &&
                    item_dsp_wrk[i][j].pos[1] > (int)(plyr_wrk.move_box.pos[1] - 1000.0f)
                )
                {
                    if (OutSightChk(item_dsp_wrk[i][j].pos, plyr_wrk.move_box.pos, plyr_wrk.move_box.rot[1], PI / 2, 300.0f) == 0)
                    {
                        return item_dsp_wrk[i][j].item_no;
                    }

                    if (OutSightChk(item_dsp_wrk[i][j].pos, plyr_wrk.move_box.pos, plyr_wrk.move_box.rot[1], PI, 100.0f) == 0)
                    {
                        return item_dsp_wrk[i][j].item_no;
                    }
                }
            }
        }
    }

    return 0xffff;
}

void GetMapItem(u_short get_no)
{
    int tmp;
    u_char i;
    u_char j;

    plyr_wrk.mode = PMODE_MSG_DISP;

    eff_blur_off = 1;

    ingame_wrk.mode = INGAME_MODE_GET_ITEM;

    if (get_no & 0x8000)
    {
        tmp = get_no - ((get_no - 0x8000 < 0) ? 0x6001 : 0x8000);

        i = (tmp >> 13) & 0xff;
        j = get_no & 0xff;

        ItemGet(i, get_no, map_file_dat[i][j].get_msg[0], map_file_dat[i][j].get_msg[1]);

        poss_file[i][j] = 2;
    }
    else
    {
        ItemGet(4, map_item_dat[get_no].item_no, map_item_dat[get_no].get_msg[0], map_item_dat[get_no].get_msg[1]);

        if (map_item_dat[get_no].item_no == 8)
        {
            if (poss_item[map_item_dat[get_no].item_no] == 0)
            {
                poss_item[map_item_dat[get_no].item_no] = 1;

                item_ap[get_no][0] = 1;
                item_ap[get_no][1] = 1;
            }
        }
        else
        {
            if (poss_item[map_item_dat[get_no].item_no] != 0x63)
            {
                if (map_item_dat[get_no].item_no < 5)
                {
                    poss_item[map_item_dat[get_no].item_no] += 12;

                    if (poss_item[map_item_dat[get_no].item_no] > 99)
                    {
                        poss_item[map_item_dat[get_no].item_no] = 99;
                    }
                }
                else
                {
                    poss_item[map_item_dat[get_no].item_no]++;
                }

                item_ap[get_no][0] = 1;
                item_ap[get_no][1] = 1;
            }
        }
    }

    ItemDispDataRenew();
}

void ItemAppearCtrl()
{
    return;
}

int ItemGetContinueJudge()
{
    int get_item;

    get_item = GetMapItemJudge();

    if (get_item != 0xFFFF)
    {
        GetMapItem(get_item);

        return 1;
    }

    return 0;
}
