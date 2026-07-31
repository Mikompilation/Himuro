#include "common.h"
#include "typedefs.h"
#include "addresses.h"
#include "enums.h"
#include "ig_init.h"

#include "main/glob.h"
#include "ingame/camera/camera.h"
#include "ingame/ig_glob.h"
#include "ingame/event/ev_load.h"
#include "ingame/map/map_area.h"
#include "ingame/entry/entry.h"
#include "ingame/entry/ap_rgost.h"
#include "ingame/entry/ap_fgost.h"
#include "ingame/entry/ap_ggost.h"
#include "graphics/graph2d/effect_scr.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/fileload.h"

#define PI 3.1415927f

typedef struct {
    u_char mode;
    u_char count;
    u_char lock;
    int load_id;
} LOAD_START_WRK;

LOAD_START_WRK load_start_wrk = {0};

void InitCamera()
{
    u_char i;

    camera = (SgCAMERA){0};

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
    plyr_wrk = (PLYR_WRK){0};

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
    plyr_wrk = (PLYR_WRK){0};

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
    memset(ene_wrk , 0, sizeof(ene_wrk));

    InitRequestSpirit();
    InitRequestFly();
}

void EnemyActDataLoad()
{
    int eadat_tbl[3] = {ENE_ACT1_OBJ, ENE_ACT1_OBJ, ENE_ACT1_OBJ};

    FileLoadB(eadat_tbl[ingame_wrk.msn_no], ENE_ACT_OBJ_ADDRESS);
}

void InitFlyWrk()
{
    memset(&fly_wrk, 0, sizeof(fly_wrk));
}

void InitFilm()
{
    return;
}

void LoadStartDataInit()
{
    load_start_wrk = (LOAD_START_WRK){0};

    ingame_wrk.stts |= (0x20 | 0x8);

    SortLoadDataAddr();

    load_start_wrk.mode = LOAD_START_MODE_PREQ;

    InitNowLoading();
    SetNowLoadingON();

    load_start_wrk.lock = 1;
}

int LoadStartDataSet()
{
    int ret;

    ret = SetNowLoading();

    if (load_start_wrk.mode == LOAD_START_MODE_PREQ)
    {
        load_start_wrk.mode = LOAD_START_MODE_PLYR;

        ReqMsnInitPlyr(ingame_wrk.msn_no);
    }
    else if (load_start_wrk.mode == LOAD_START_MODE_PLYR)
    {
        if (MsnInitPlyr())
        {
            load_start_wrk.mode = LOAD_START_MODE_DREQ;
        }
    }

    if (load_start_wrk.mode == LOAD_START_MODE_DATA)
    {
        if (IsLoadEnd(load_start_wrk.load_id))
        {
            MissionDataLoadAfterInit(&load_dat_wrk[load_start_wrk.count]);

            load_start_wrk.mode = LOAD_START_MODE_DREQ;

            load_start_wrk.count++;
        }
    }
    else if (load_start_wrk.mode == LOAD_START_MODE_DREQ)
    {
        while (load_start_wrk.count < 40)
        {
            if (load_dat_wrk[load_start_wrk.count].file_no != 0xffff)
            {
                load_start_wrk.load_id = MissionDataLoadReq(&load_dat_wrk[load_start_wrk.count]);

                load_start_wrk.mode = LOAD_START_MODE_DATA;

                return 0;
            }

            load_start_wrk.count++;
        }

        AreaRoomAllLoadInit();
        RoomMdlLoadReq(0, 0, ingame_wrk.msn_no, plyr_wrk.pr_info.room_no, 1);

        area_wrk.room[0] = plyr_wrk.pr_info.room_no;
        area_wrk.room[1] = 0xff;

        load_start_wrk.mode = LOAD_START_MODE_ROOM;
    }
    else if (load_start_wrk.mode == LOAD_START_MODE_ROOM)
    {
        if (RoomMdlLoadWait())
        {
            load_start_wrk.mode = LOAD_START_MODE_RGRQ;
        }
    }
    else if (load_start_wrk.mode == LOAD_START_MODE_RGRQ)
    {
        RareGhostLoadGameLoadReq();

        load_start_wrk.mode = LOAD_START_MODE_RGST;
    }
    else if (load_start_wrk.mode == LOAD_START_MODE_RGST)
    {
        if (IsLoadEndAll())
        {
            FloatGhostLoadReq();

            ap_wrk.fgst_no = 0xff;

            load_start_wrk.mode = LOAD_START_MODE_FGST;
        }
    }
    else if (load_start_wrk.mode == LOAD_START_MODE_FGST)
    {
        if (FloatGhostLoadMain())
        {
            GuardGhostLoadReq();

            load_start_wrk.mode = LOAD_START_MODE_GGST;
        }
    }
    else if (load_start_wrk.mode == LOAD_START_MODE_GGST)
    {
        if (GuardGhostLoad())
        {
            printf("GuardGhostLoadend\n");

            load_start_wrk.mode = LOAD_START_MODE_FADEIN;
        }
    }
    else if (load_start_wrk.mode == LOAD_START_MODE_FADEIN)
    {
        SetNowLoadingOFF();

        load_start_wrk.mode = LOAD_START_MODE_END;
    }
    else if (load_start_wrk.mode == LOAD_START_MODE_END)
    {
        if (ret != 0xff)
        {
            return 0;
        }

        SetBlackIn2(60);

        ingame_wrk.mode = INGAME_MODE_NOMAL;
        ingame_wrk.stts &= ~(0x20 | 0x8);

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
