#include "common.h"
#include "typedefs.h"
#include "ig_init.h"

#include "main/glob.h"
#include "ingame/camera/camera.h"
#include "ingame/event/ev_load.h"
#include "ingame/map/map_area.h"
#include "ingame/entry/entry.h"
#include "ingame/entry/ap_rgost.h"
#include "ingame/entry/ap_fgost.h"
#include "ingame/entry/ap_ggost.h"
#include "graphics/graph2d/effect_scr.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/fileload.h"

#define PI 3.1415928f

typedef struct {
	u_char mode;
	u_char count;
	u_char lock;
	int load_id;
} LOAD_START_WRK;

// `load_start_wrk` is located in `.sdata`, so it needs to be initialized.
// However, it *must* be initialized in `LoadStartDataInit`, not here.
// Initializing it here would interfere with the initialization logic
// in `LoadStartDataInit`, causing it to be modified. Therefore, we
// initialize it here with `{}` rather than `{0}`.
LOAD_START_WRK load_start_wrk = {};

void InitCamera()
{
    u_char i;

    memset(&camera, 0, sizeof(SgCAMERA));

    camera.roll = PI;
    camera.fov = 0.7683708f;
    camera.nearz = 0.1f;
    camera.farz = 32768.0f;
    camera.ax = 1.0f;
    camera.ay = 0.40689999f;
    camera.cx = 2048.0f;
    camera.cy = 2048.0f;
    camera.zmin = 0.0f;
    camera.zmax = 16777215.0f;

    for (i = 0; i < 50; i++)
    {
        ene_cam_req_checker[i] = 0;
    }
}

void InitPlyr()
{
    memset(&plyr_wrk, 0, sizeof(PLYR_WRK));

    plyr_wrk.hp = 500;
    plyr_wrk.spd = 5.5f;
    plyr_wrk.pr_info.se_foot = 0xff;
    plyr_wrk.dop.room_no = 0xff;
    plyr_wrk.film_no = 1;
    plyr_wrk.pr_info.camera_door = -1;
    plyr_wrk.pr_set = -1;
    plyr_wrk.po_set = -1;
    plyr_wrk.se_deadly = -1;
}

void InitPlyr2(int film_no)
{
    memset(&plyr_wrk, 0, sizeof(PLYR_WRK));

    plyr_wrk.hp = 500;
    plyr_wrk.spd = 5.5f;
    plyr_wrk.pr_info.se_foot = 0xff;
    plyr_wrk.dop.room_no = 0xff;
    plyr_wrk.pr_info.camera_door = -1;
    plyr_wrk.pr_set = -1;
    plyr_wrk.po_set = -1;
    plyr_wrk.film_no = film_no;
    plyr_wrk.se_deadly = -1;
}

void InitPlyrAfterLoad(void)
{
    plyr_wrk.se_deadly = -1;
}

void InitEnemy(void)
{
    memset(ene_wrk , 0, 4 * sizeof(ENE_WRK));

    InitRequestSpirit();
    InitRequestFly();
}

void EnemyActDataLoad()
{
#ifdef BUILD_EU_VERSION
    int eadat_tbl[3] = {46, 46, 46};
#else
    int eadat_tbl[3] = {22, 22, 22};
#endif

    FileLoadB(eadat_tbl[ingame_wrk.msn_no], 0x7e0000);
}

void InitFlyWrk()
{
    memset(&fly_wrk, 0, 10 * sizeof(FLY_WRK));
}

void InitFilm()
{
    return;
}

void LoadStartDataInit()
{
    // memset(&load_start_wrk, 0, sizeof(LOAD_START_WRK));
    load_start_wrk = (LOAD_START_WRK){0};

    ingame_wrk.stts |= 0x28;

    SortLoadDataAddr();

    load_start_wrk.mode = 0;

    InitNowLoading();
    SetNowLoadingON();

    load_start_wrk.lock = 1;
}

int LoadStartDataSet()
{
    int ret;

    ret = SetNowLoading();

    if (load_start_wrk.mode == 0)
    {
        load_start_wrk.mode = 1;

        ReqMsnInitPlyr(ingame_wrk.msn_no);
    }
    else if (load_start_wrk.mode == 1)
    {
        if (MsnInitPlyr())
        {
            load_start_wrk.mode = 2;
        }
    }

    if (load_start_wrk.mode == 3)
    {
        if (IsLoadEnd(load_start_wrk.load_id))
        {
            MissionDataLoadAfterInit(&load_dat_wrk[load_start_wrk.count]);

            load_start_wrk.mode = 2;
            load_start_wrk.count += 1;
        }
    }
    else if (load_start_wrk.mode == 2)
    {
        while (load_start_wrk.count < 40)
        {
            if (load_dat_wrk[load_start_wrk.count].file_no != 0xffff)
            {
                load_start_wrk.load_id = MissionDataLoadReq(&load_dat_wrk[load_start_wrk.count]);
                load_start_wrk.mode = 3;

                return 0;
            }
            load_start_wrk.count++;
        }

        AreaRoomAllLoadInit();
        RoomMdlLoadReq(0, 0, ingame_wrk.msn_no, plyr_wrk.pr_info.room_no, 1);

        area_wrk.room[0] = plyr_wrk.pr_info.room_no;
        area_wrk.room[1] = 0xff;
        load_start_wrk.mode = 4;
    }
    else if (load_start_wrk.mode == 4)
    {
        if (RoomMdlLoadWait())
        {
            load_start_wrk.mode = 5;
        }
    }
    else if (load_start_wrk.mode == 5)
    {
        RareGhostLoadGameLoadReq();

        load_start_wrk.mode = 6;
    }
    else if (load_start_wrk.mode == 6)
    {
        if (IsLoadEndAll())
        {
            FloatGhostLoadReq();

            ap_wrk.fgst_no = 0xff;
            load_start_wrk.mode = 7;
        }
    }
    else if (load_start_wrk.mode == 7)
    {
        if (FloatGhostLoadMain())
        {
            GuardGhostLoadReq();

            load_start_wrk.mode = 8;
        }
    }
    else if (load_start_wrk.mode == 8)
    {
        if (GuardGhostLoad())
        {
            printf("GuardGhostLoadend\n");

            load_start_wrk.mode = 0x9;
        }
    }
    else if (load_start_wrk.mode == 9)
    {
        SetNowLoadingOFF();

        load_start_wrk.mode = 10;
    }
    else if (load_start_wrk.mode == 10)
    {
        if (ret != 0xff)
        {
            return 0;
        }

        SetBlackIn2(0x3c);

        ingame_wrk.mode = 6;
        ingame_wrk.stts &= 0xd7;

        return 1;
    }

    return 0;
}

void InitLoadStartLock(void)
{
    load_start_wrk.lock = 0;
}

int GetLoadStartLock()
{
    return load_start_wrk.lock;
}
