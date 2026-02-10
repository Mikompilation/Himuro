#include "common.h"
#include "typedefs.h"
#include "door_conte.h"

#include "ingame/map/doortype.h"

#include "ingame/map/door_conte.h"
#include "ingame/map/door_ctl.h"
#include "ingame/map/doortype.h"
// #include "ingame/map/map_ctrl.h" // GetPointHeight
// #include "ingame/plyr/plyr_ctl.h" // SetPlyrAnime
#include "main/glob.h"

static void DoorConteInit();
static void PlayerPosAdjust(u_char *time, u_char max);
static void PlyrPosReSet();

static DOOR_CONTE dorcon;

void DoorOpenConteInit()
{
    dorcon = (DOOR_CONTE){0};

    dorcon.mtn_no = TargetDoorKindChk();

    if (dorcon.mtn_no == 0xff)
    {
        dorcon.state = DR_CNT_IDOL;
    }
    else
    {
        DoorConteInit();

        PlyrPosAdj(dorcon.mtn_no, 0);

        SetPlyrAnime(door_mtn_dat[dorcon.mtn_no].anm_no, 0);

        SetDoorOpenCameraNo();

        dorcon.state = DR_CNT_OPEN_STRT;
    }
}

u_char DoorOpenConte()
{
    u_char anm;
    static int i = 0;
    static u_char one_flg = 0;

    if (dorcon.state == DR_CNT_IDOL)
    {
        return 0xff;
    }

    switch(dorcon.state)
    {
    case DR_CNT_OPEN_STRT:
        if (door_open_ctrl.open[0] == 1 || door_open_ctrl.open[1] == 1)
        {
            dorcon.state = DR_CNT_DOR_OPEN;

            one_flg = 1;
        }
    break;
    case DR_CNT_DOR_OPEN:
        if (one_flg != 0)
        {
            DoorMoveStart(dorcon.mtn_no, 0);
        }

        one_flg = 0;

        if (plyr_wrk.sta & 0x20)
        {
            anm = door_mtn_dat[dorcon.mtn_no].c_anm_no;

            if (anm != 200)
            {
                PlyrPosAdj(dorcon.mtn_no, 0);

                SetPlyrAnime(anm, 0);

                dorcon.state = DR_CNT_CLOSE_STRT;
            }
            else
            {
                SetPlyrAnime(200, 0);

                dorcon.state = DR_CNT_END;

                plyr_wrk.sta ^= 0x8;
            }
        }
    break;
    case DR_CNT_CLOSE_STRT:
        if (door_open_ctrl.open[0] == 2 || door_open_ctrl.open[1] == 2)
        {
            one_flg = 1;

            dorcon.state = DR_CNT_DOR_CLOSE;
        }
    break;
    case DR_CNT_DOR_CLOSE:
        if (one_flg != '\0')
        {
            DoorMoveStart(dorcon.mtn_no, 1);
        }

        one_flg = 0;

        if (plyr_wrk.sta & 0x20)
        {
            PlyrPosReSet();
            SetPlyrAnime(0, 0);

            plyr_wrk.sta ^= 0x8;

            dorcon.state = DR_CNT_END;
        }
    break;
    case DR_CNT_END:
        dorcon.state = DR_CNT_IDOL;
    break;
    }

    if (dorcon.timer != 0)
    {
        dorcon.timer--;
    }

    return dorcon.state;
}

static void DoorConteInit()
{
    dorcon.timer = 0;
}

static void PlayerPosAdjust(u_char *time, u_char max)
{
    float per;

    if (max != 0)
    {
        per = 1.0f - (float)*time / (max - 1);
    }
    else
    {
        per = 1.0f;
    }

    plyr_wrk.move_box.pos[0] = (dorcon.adj_x1 - dorcon.adj_x0) * per + dorcon.adj_x0;
    plyr_wrk.move_box.pos[2] = (dorcon.adj_z1 - dorcon.adj_z0) * per + dorcon.adj_z0;
    plyr_wrk.move_box.rot[1] = dorcon.adj_org + (dorcon.adj_r1 - dorcon.adj_r0) * per + dorcon.adj_r0;
}

static void PlyrPosReSet()
{
    PLYR_WRK *pwp;
    MOVE_BOX *mbp;

    pwp = &plyr_wrk;
    mbp = &pwp->move_box;

    mbp->pos[0] = pwp->bwp[0];
    mbp->pos[2] = pwp->bwp[2];
    mbp->pos[1] = GetPointHeight(mbp->pos[0], mbp->pos[2]);
}

