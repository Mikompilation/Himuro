#include "common.h"
#include "typedefs.h"
#include "ap_pgost.h"

#include "os/eeiop/eese.h"
#include "ingame/entry/ap_rgost.h"
#include "ingame/entry/entry.h"
#include "ingame/event/ev_main.h"
#include "ingame/map/map_area.h"
#include "ingame/map/map_ctrl.h"

#include "data/pg_dat.h" // PGOST_DAT pg_dat[];

int test_rg_no = 0;
PGOST_WRK pg_wrk = {0};

void PuzzleGhostInit()
{
    pg_wrk = (PGOST_WRK){0};

    pg_wrk.pgst_no = 0xff;
    pg_wrk.req_no = 0xff;
}

void PuzzleGhostLoadReq(u_char pg_no)
{
    int i;

    for (i = 0; i < 5; i++)
    {
        // debug code?
    }

    for (i = 0; i < 5; i++)
    {
        if (ap_wrk.pg_req[i] == 0xff)
        {
            ap_wrk.pg_req[i] = pg_no;

            return;
        }
    }
}

void PuzzleGhostReq(u_char pg_no)
{
    int i;

    ap_wrk.stts |=  0x20;

    for (i = 0; i < 5; i++)
    {
        if (area_wrk.rgst[i] == (pg_no | 0x80))
        {
            pg_wrk.req_no = pg_no;
            break;
        }
    }

    pg_wrk.ev_no = ev_wrk.evt_no;
}

int PuzzleGhostContinue(u_char ev_no)
{
    if (pg_wrk.ev_no != 0xff && pg_wrk.ev_no == ev_no)
    {
        if (pg_wrk.pgst_no != 0xff)
        {
            pg_wrk.req_no = pg_wrk.pgst_no;

            return 1;
        }
    }

    return 0;
}

void PuzzleGhostPhotoReq()
{
    int i;

    pg_wrk.stts = 0x0;

    rg_dsp_wrk[0].mode = 0;

    if (rg_dsp_wrk[0].se_wrk_no != 0xff)
    {
        SeFadeFlame(rg_dsp_wrk[0].se_wrk_no, 20, 0);
    }

    rg_dsp_wrk[0].se_wrk_no = 0xff;

    event_stts[pg_wrk.ev_no] = 1;

    for (i = 0; i < 3; i++)
    {
        if (ev_wrk.spev_tmp[i] == pg_wrk.pgst_no)
        {
            ev_wrk.spev_tmp[i] = 0xff;
        }
    }

    for (i = 0; i < 5; i++)
    {
        if (area_wrk.rgst[i] == (pg_wrk.req_no | 0x80))
        {
            area_wrk.rgst[i] = 0xff;
            break;
        }
    }

    pg_wrk.ev_no = 0xff;
    pg_wrk.pgst_no = 0xff;
}

void PuzzleGhostMain()
{
    PuzzleGhostDispSet();

    if (pg_wrk.stts != 0x0)
    {
        PuzzleGhostDispMain();
    }
}

void PuzzleGhostDispSet()
{
    if (pg_wrk.req_no == 0xff)
    {
        if (pg_wrk.pgst_no != 0xff)
        {
            if (pg_wrk.stts == 0x3)
            {
                pg_wrk.stts = 0x4;
                pg_wrk.time = 20;

                if (rg_dsp_wrk[0].se_wrk_no != 0xff)
                {
                    SeFadeFlame(rg_dsp_wrk[0].se_wrk_no, pg_wrk.time, 0);
                }

                rg_dsp_wrk[0].se_wrk_no = 0xff;
            }
            else if (pg_wrk.stts == 0x2)
            {
                pg_wrk.stts = 0x4;
                pg_wrk.time = 20 - pg_wrk.time / 3;

                if (rg_dsp_wrk[0].se_wrk_no != 0xff)
                {
                    SeFadeFlame(rg_dsp_wrk[0].se_wrk_no, 20 - pg_wrk.time / 3, 0);
                }

                rg_dsp_wrk[0].se_wrk_no = 0xff;
            }
        }
    }
    else
    {
        pg_wrk.pgst_no = pg_wrk.req_no;

        if (pg_wrk.stts == 0x0)
        {
            pg_wrk.stts = 0x1;
        }
        else if (pg_wrk.stts == 0x4)
        {
            rg_dsp_wrk[0].se_wrk_no = SeStartPos(28, rg_dsp_wrk[0].pos, 0, 0x1000, 0x1000, 0xff);

            pg_wrk.stts = 0x2;
            pg_wrk.time = -(pg_wrk.time * 3 - 60);
        }

        pg_wrk.req_no = 0xff;
    }
}

void PuzzleGhostDispMain()
{
    sceVu0FVECTOR room_pos;

    if (pg_wrk.stts == 0x1)
    {
        rg_dsp_wrk[0].mode = 2;
        rg_dsp_wrk[0].rg_no = pg_wrk.pgst_no + 128;

        GetRoomPos(pg_dat[pg_wrk.pgst_no].room, room_pos);

        rg_dsp_wrk[0].pos[0] = pg_dat[pg_wrk.pgst_no].pos[0] + room_pos[0];
        rg_dsp_wrk[0].pos[1] = pg_dat[pg_wrk.pgst_no].pos[1] + room_pos[1];
        rg_dsp_wrk[0].pos[2] = pg_dat[pg_wrk.pgst_no].pos[2] + room_pos[2];

        rg_dsp_wrk[0].se_wrk_no = SeStartPos(28, rg_dsp_wrk[0].pos, 0, 0x1000, 0x1000, 0xff);

        pg_wrk.stts = 0x2;
        pg_wrk.time = 60;
    }

    if (pg_wrk.time != 0)
    {
        pg_wrk.time--;
    }

    switch (pg_wrk.stts)
    {
    case 0x2:
        rg_dsp_wrk[0].alpha = (int)((60 - pg_wrk.time) * 100) / 60;

        if (pg_wrk.time == 0)
        {
            pg_wrk.stts = 0x3;
        }
    break;
    case 0x3:
        rg_dsp_wrk[0].alpha = 0x64;
    break;
    case 0x4:
        rg_dsp_wrk[0].alpha = pg_wrk.time * 5;

        if (pg_wrk.time == 0)
        {
            pg_wrk.stts = 0x0;

            rg_dsp_wrk[0].mode = 0;
        }
    }
}
