#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "find_ctl.h"

#include "graphics/graph2d/message.h"
#include "ingame/map/find_ctl.h"
#include "ingame/map/find_dat.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/plyr/unit_ctl.h"
#include "main/glob.h"

u_char find_stts[250] = {0};
FIND_WRK find_wrk = {0};

void InitFindStatus()
{
    memset(find_stts, 0, sizeof(find_stts));
}

void FindMapInit()
{
    int i;

    find_wrk = (FIND_WRK){0};

    find_wrk.mode = 0;

    for (i = 0; i < 10; i++)
    {
        find_wrk.pos_req[i] = 0xffff;
    }
}

u_char FindMapReqJudge()
{
    int i;
    int open;
    int after_no;
    sceVu0FVECTOR fd_pos;

    for (i = 0; i < 10; i++)
    {
        open = 1;

        if (find_wrk.pos_req[i] != 0xffff)
        {
            if (find_dat[find_wrk.pos_req[i]].type & 0x40)
            {
                if (find_stts[find_wrk.pos_req[i]] == 0)
                {
                    open = 0;
                }

                if (find_dat[find_wrk.pos_req[i]].type & 0x80)
                {
                    if (find_stts[find_wrk.pos_req[i]] != 1)
                    {
                        open = 0;
                    }
                }
            }
            else if (find_dat[find_wrk.pos_req[i]].type & 0x80)
            {
                if (find_stts[find_wrk.pos_req[i]] != 0)
                {
                    open = 0;
                }
            }

            if (find_dat[find_wrk.pos_req[i]].type & 0x20)
            {
                if (*key_now[5] != 1)
                {
                    open = 0;
                }
            }

            if (find_dat[find_wrk.pos_req[i]].type & 0x10)
            {
                fd_pos[0] = find_dat[find_wrk.pos_req[i]].pos_x;
                fd_pos[2] = find_dat[find_wrk.pos_req[i]].pos_y;

                if (OutSightChk(fd_pos, plyr_wrk.move_box.pos, plyr_wrk.move_box.rot[1], 1.0471976f, 10000.0f) != 0)
                {
                    open = 0;
                }
            }

            if (open)
            {
                find_wrk.mode = 1;
                find_wrk.req_no = find_wrk.pos_req[i];
                find_wrk.msg_no = find_dat[find_wrk.pos_req[i]].msg_no;

                if (find_stts[find_wrk.pos_req[i]] != 0xff)
                {
                    find_stts[find_wrk.pos_req[i]]++;
                }

                if (find_dat[find_wrk.pos_req[i]].after != 0xffff)
                {
                    if (find_dat[find_wrk.pos_req[i]].type & 0x8)
                    {
                        after_no = find_dat[find_wrk.pos_req[i]].after;

                        while (after_no != 0xffff)
                        {
                            find_stts[after_no] = 0x1;

                            after_no = find_dat[after_no].after;
                        }
                    }
                    else
                    {
                        find_stts[find_dat[find_wrk.pos_req[i]].after] = 0x1;
                    }
                }
                GetIngameMSGAddr(IGMSG_FIND_MSG, find_wrk.msg_no);

                return 1;
            }
        }
    }

    return 0;
}

u_char FindMapCtrl()
{
    switch (find_wrk.mode)
    {
    case 1:
        find_wrk.mode = 2;

        SetMessageAddr(GetIngameMSGAddr(5, find_wrk.msg_no));

        if (SetMessage() == 0)
        {
            find_wrk.mode = 3;
        }
    break;
    case 2:
        if (*key_now[5] == 1)
        {
            MessageWaitOff();
        }

        if (SetMessage() == 0)
        {
            find_wrk.mode = 3;
        }
    break;
    case 3:
        find_wrk.mode = 0;

        return 1;
    break;
    }

    return 0;
}
