#include "common.h"
#include "typedefs.h"
#include "mv_cmnd.h"

#include "graphics/graph2d/message.h"
#include "ingame/menu/ig_menu.h"

MOVIE_CMD_WRK mvcmd_wrk = {0};

#define BASE_ADDRESS 0x7f0000

void EventMovieCommandInit()
{
    mvcmd_wrk = (MOVIE_CMD_WRK){0};

    mvcmd_wrk.msg_no = -1;
}

void EventMovieCommandMain(int flame, int top_addr)
{
    int *dat_addr;
    u_char *cmd_addr;

    dat_addr = (int *)top_addr;

    while (dat_addr[0] <= flame && dat_addr[0] != -1)
    {
        if (flame == dat_addr[0])
        {
            cmd_addr = (u_char *)(top_addr + dat_addr[1]);

            switch(*cmd_addr)
            {
            case 0xf0:
            case 0xf1:
            case 0xf2:
            case 0xf3:
            case 0xf5:
            case 0xf6:
            case 0xf7:
            case 0xf8:
            case 0xf9:
                // do nothing ...
            break;
            case 0xf4:
                mvcmd_wrk.msg_no = *(short int *)(cmd_addr + 2);
                mvcmd_wrk.msg_wait = *(short int *)(cmd_addr + 4);
            break;
            }
        }

        dat_addr += 2;
    }

    if (mvcmd_wrk.msg_no != -1)
    {
        if (mvcmd_wrk.msg_wait != 0)
        {
            SetMessageAddr(GetMovieMessageAddr(mvcmd_wrk.msg_no));

            mvcmd_wrk.msg_wait--;

            SetMessage();
        }
        else
        {
            mvcmd_wrk.msg_no = 0xffff;
        }
    }
}

int GetMovieMessageAddr(short int msg_no)
{
    int addr;

    addr = Get4Byte((u_char *)BASE_ADDRESS + 3 * 4);
    addr = Get4Byte((u_char *)BASE_ADDRESS + addr + msg_no * 4);

    return BASE_ADDRESS + addr;
}
