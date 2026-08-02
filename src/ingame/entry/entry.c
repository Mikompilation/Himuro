#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "entry.h"

#include "main/glob.h"
#include "ingame/plyr/time_ctl.h"
#include "ingame/entry/ap_fgost.h"
#include "ingame/entry/ap_ggost.h"
#include "ingame/entry/ap_pgost.h"
#include "ingame/entry/ap_rgost.h"
#include "ingame/entry/ap_zgost.h"
#include "ingame/entry/ap_sgost.h"

AP_WRK ap_wrk = {0};

void EntryInit()
{
    int i;

    ap_wrk = (AP_WRK){0};

    for (i = 0; i < 5; i++)
    {
        ap_wrk.pg_req[i] = 0xff;
    }

    FloatGhostAppearInit();
    SettleGhostInit();

    ap_wrk.ggst_no = 0xff;
}

void ClearAppearWrkInfo()
{
    int i;

    ap_wrk.gtime = 0;
    ap_wrk.rtime = 0;
    ap_wrk.ptime = 0;

    for (i = 0; i < 11; i++)
    {
        ap_wrk.atime[i] = 0;
    }

    ap_wrk.sound = 0;
    ap_wrk.pic_num = 0;
    ap_wrk.raze = 0;
}

void EntryMain()
{
    EntryTimeCtrl();

    if (dbg_wrk.param_fuyurei != 0)
    {
        if (ap_wrk.stts & 0x4 || ap_wrk.room_fg[plyr_wrk.pr_info.room_no] != 0 || ap_wrk.zh_mode != ZH_NO_REQ)
        {
            ZeroHourAppearMain();
        }

        if ((ap_wrk.stts & 0x40) == 0 && ap_wrk.room_fg[plyr_wrk.pr_info.room_no] != 0)
        {
            if (ap_wrk.zh_mode == ZH_NO_REQ)
            {
                FloatGhostAppearMain();
            }
        }
    }

    if (ap_wrk.zh_mode == ZH_NO_REQ)
    {
        GuardGhostAppearMain();
    }

    if ((ap_wrk.stts & 0x20) == 0)
    {
        RareGhostAppearCtrl();
    }

    if ((ap_wrk.stts & 0x10) == 0)
    {
        PuzzleGhostMain();
    }

    ClearAppearWrkInfo();
}

void AreaEntrySet()
{
    return;
}