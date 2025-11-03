#include "common.h"
#include "typedefs.h"
#include "time_ctl.h"

#include "main/glob.h"
#include "os/system.h"
#include "ingame/map/map_area.h"
#include "ingame/entry/entry.h"

#define SECONDS_PER_MINUTE (60)
#define SECONDS_PER_HOUR   (60 * SECONDS_PER_MINUTE)
#define SECONDS_PER_DAY    (24 * SECONDS_PER_HOUR)

#define SECONDS_249D_23H_59M_00S \
    ((249 * SECONDS_PER_DAY) + (23 * SECONDS_PER_HOUR) + (59 * SECONDS_PER_MINUTE))

#define SECONDS_10H_00M_00S \
    (10 * SECONDS_PER_HOUR)

#define SECONDS_01H_00M_00S \
    (1 * SECONDS_PER_HOUR)

#define SECONDS_00H_10M_00S \
    (10 * SECONDS_PER_MINUTE)

void TimeCtrlInit()
{
    time_wrk = (TIME_WRK){0};

    GetNowClock(&time_wrk.start);
    GetNowClock(&time_wrk.real);
}

void GameTimeCtrl()
{
    if (time_wrk.real_ofs < 0xffffffffffffffff)
    {
        time_wrk.real_ofs++;
    }

    if (ingame_wrk.stts & 0x80)
    {
        return;
    }

    if (time_wrk.game < SECONDS_249D_23H_59M_00S)
    {
        time_wrk.game++;
    }

    if (time_wrk.one_game < SECONDS_249D_23H_59M_00S)
    {
        time_wrk.one_game++;
    }
}

void SetNowClock(sceCdCLOCK *nc)
{
    GetNowClock(nc);

    if (nc->pad == 0)
    {
        time_wrk.real = *nc;
        time_wrk.real_ofs = 0;
    }
}

void PlyrTimerCtrl()
{
    if (plyr_wrk.pr_info.room_no != plyr_wrk.pr_info.room_old)
    {
        time_wrk.room = 0;
    }
    else
    {
        if (time_wrk.room < 0xffffffffffffffff)
        {
            time_wrk.room++;
        }
    }

    if (plyr_wrk.mode == 0x1)
    {
        if (time_wrk.finder < 0xffffffffffffffff)
        {
            time_wrk.finder++;
        }

        if (plyr_wrk.mvsta & 0x4)
        {
            if (time_wrk.fwalk < 0xffffffffffffffff)
            {
                time_wrk.fwalk = time_wrk.fwalk + 1;
            }
        }
        else
        {
            time_wrk.fwalk = 0;
        }
    }
    else
    {
        time_wrk.finder = 0;
        time_wrk.fwalk = 0;
    }

    if (plyr_wrk.mvsta & 0x4)
    {
        if (time_wrk.swalk < 0xffffffffffffffff)
        {
            time_wrk.swalk++;
        }

        time_wrk.nwalk = 0;
        time_wrk.run = 0;
        time_wrk.stop = 0;
    }
    else if (plyr_wrk.mvsta & 0x2)
    {
        if (time_wrk.nwalk < 0xffffffffffffffff)
        {
            time_wrk.nwalk++;
        }

        time_wrk.swalk = 0;
        time_wrk.run = 0;
        time_wrk.stop = 0;
    }
    else if (plyr_wrk.mvsta & 0x1)
    {
        if (time_wrk.run < 0xffffffffffffffff)
        {
            time_wrk.run++;
        }

        time_wrk.swalk = 0;
        time_wrk.nwalk = 0;
        time_wrk.stop = 0;
    }
    else
    {
        if (time_wrk.stop < 0xffffffffffffffff)
        {
            time_wrk.stop++;
        }

        time_wrk.swalk = 0;
        time_wrk.nwalk = 0;
        time_wrk.run = 0;
    }

    if (pad[0].now == 0 && time_wrk.stop != 0)
    {
        if (time_wrk.no_pad < 0xffffffffffffffff)
        {
            time_wrk.no_pad++;
        }
    }
    else
    {
        time_wrk.no_pad = 0;
    }

    if (area_wrk.area_no != area_wrk.area_bak)
    {
        time_wrk.area = 0;
    }
    else
    {
        if (time_wrk.area < 0xffffffffffffffff)
        {
            time_wrk.area++;
        }
    }

    if (time_wrk.play < 0xffffffffffffffff)
    {
        time_wrk.play++;
    }
}

void EntryTimeCtrl()
{
    if (time_wrk.game != 0 && (time_wrk.game % SECONDS_10H_00M_00S) == 0)
    {
        ap_wrk.gtime++;
    }

    if (time_wrk.play != 0 && (time_wrk.play % SECONDS_01H_00M_00S) == 0)
    {
        ap_wrk.ptime++;
    }

    if (time_wrk.area != 0 && (time_wrk.area % SECONDS_00H_10M_00S) == 0)
    {
        ap_wrk.atime[area_wrk.area_no]++;
    }
}

void SetRealTime()
{
    SetNowClock(&time_wrk.real);
}
