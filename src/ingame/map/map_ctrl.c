#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "common.h"

#include "common/ul_math.h"
#include "graphics/graph2d/effect_oth.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/motion/accessory.h"
#include "graphics/motion/mime.h"
#include "ingame/camera/camera.h"
#include "ingame/event/ev_main.h"
#include "ingame/event/wan_soul.h"
#include "ingame/ig_glob.h"
#include "ingame/map/door_ctl.h"
#include "ingame/map/find_ctl.h"
#include "ingame/map/furn_ctl.h"
#include "ingame/map/furn_spe/furn_spe.h"
#include "ingame/map/item_ctl.h"
#include "ingame/map/map_area.h"
#include "ingame/map/map_ctrl.h"
#include "ingame/map/map_htck.h"
#include "main/glob.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"
#include "os/eeiop/sd_room.h"
#include "os/eeiop/se_ev.h"

static void CameraNoRenew() ;
static u_short CameraGetDoramaCameraNo(u_short now_cam_no);
static u_short CameraCheckDoorCameraNo(u_short door_id, u_char room_id);
static void PlayerHeightRenew();
static u_char GetRoomNum();
static u_char GetRoomNumFloor(u_char floor);
static u_char GetDataNum(u_char map, u_char room);
static u_char RoomChangeJudge();
static u_char CameraChangeJudgeEach(u_char cam_type, u_short cam_no);
static u_char CameraChangeJudge();
static u_char HeightChangeJudge();
static u_char GetNowRoom();
static u_char GetDoorRoomIdFromRoomNo(u_char room_no);
static u_char GetDoorRoomIdFromRoomNoFloor(u_char room_no, u_char floor);
static u_char GetDoorDataRoom(u_char room_id);
static u_short GetNowCameraEach(u_char cam_type, u_char cam_num, u_char *rm_cam_no);
static u_short GetNowCamera();
static float GetNowHeight();
static void GetPlayerRoom(u_char room);
static void GetRoomDispPos(u_char room);
static u_char PosInAreaJudge0Floor(u_char room, u_short pos_x, u_short pos_y, u_char floor);
static void GetNowOpenEvent();
static void GetNowOpenFindAct();
static void GetNowPlayerMoveMotion();
static void MapSetFloorSeNo();

u_char room_pass[42] = {0};

const u_char floor_exist[5][4] = {
    { 0, 1, 1, 0 }, { 0, 1, 1, 0 }, { 1, 1, 1, 0 },
    { 1, 1, 1, 1 }, { 1, 1, 1, 1 },
};

#define PI 3.1415927f
#define RAD2DEG(x) ((float)(x)*180.0f/PI)

#define BASE_ADDRESS 0x7f8000

int MissonMapDataLoad(u_char msn_no)
{
    return LoadReq(MSN01MAP_OBJ, BASE_ADDRESS);
}

void InitMapStatus(u_char msn_no)
{
    if (ingame_wrk.clear_count == 0 && (msn_no == 0 || msn_no == 1))
    {
        RoomPassInit();
    }

    ClearFurnAttrFlg();
    ClearDoorAttrFlg();
    InitFurnAttrFlg();
    InitDoorAttrFlg();
    InitFurnDatSave();
    NewMissionInitWanderSoul();
}

void MapDataInit()
{
    PreGameInitFActWrk();

    memset(furn_wrk, 0, sizeof(furn_wrk));
    memset(fefct_wrk, 0, sizeof(fefct_wrk));

    MapFloorChange(map_wrk.floor);
}

void MapMissionChange(u_char new_msn)
{
    SeBackupInit();
    InitMapStatus(new_msn);
}

void RoomPassInit()
{
    memset(room_pass, 0, sizeof(room_pass));
}

void RoomPass(u_char room_id)
{
    if (room_id < 42)
    {
        room_pass[room_id] |= 0x1;
    }
}

int IsRoomPass(u_char room_id)
{
    if (room_id < 42 && room_pass[room_id] & 1)
    {
        return 1;
    }

    return 0;
}

void MapFloorChange(u_char new_floor)
{
    map_wrk.floor = new_floor;
    map_wrk.room_update_flg = 0;
    map_wrk.dat_adr = GetFloorTopAddr(new_floor);

    InitMap();
    RoomWrkRenew();
}

int GetFloorTopAddr(u_char floor)
{
    int *addr = (int *)BASE_ADDRESS;

    return addr[floor] + BASE_ADDRESS;
}

void InitMap()
{
    int i;

    room_wrk = (ROOM_WRK){0};

    for(i = 0; i < 60; i++)
    {
        FurnSetWrkNoUse(&furn_wrk[i], i);
    }

    room_wrk.disp_no[0] = 0xff;
    room_wrk.disp_no[1] = 0xff;

    mimChodoInitWork();
    acsInitRopeWork();
    acsInitChodoWork();
    DoorAcInit();
    DoorCtrlInit();

    room_wrk.room_num = GetRoomNum();

    GetNewRoomData();
    FurnDataInit();
    DoorDataInit();
    FurnSortFurnWrk(0);

    map_wrk.now_room = room_wrk.disp_no[0];
    map_wrk.next_room = room_wrk.disp_no[1];
}

void MapCtrlMain()
{
    int get_room;
    float get_y;
    u_char room_id;

    area_wrk.area_no = GetRoomArea(plyr_wrk.pr_info.room_no);

    RoomWrkRenew();
    CameraNoRenewEach(1);
    CameraNoRenewEach(2);
    CameraNoRenewEach(3);
    CameraNoRenewEach(4);
    PlayerHeightRenew();
    MapSetFloorSeNo();

    if (map_wrk.room_update_flg != 0)
    {
        map_wrk.room_update_flg = 0;
    }

    room_id = NextRoomRenew();

    if (room_id != 0xff)
    {
        DoorFreeFurnWrk(room_id);
        FurnFreeFurnWrk(room_id);
        FurnDataRenewNext(room_id);
        DoorDataRenewNext(room_id);
        FurnSortFurnWrk(0);

        map_wrk.room_update_flg = 1;
    }

    DoorCtrlMain();
    FurnCtrlMain();
    FActWrkMain();
    GetNowOpenEvent();
    GetNowOpenFindAct();
    GetNowPlayerMoveMotion();
    SeEvMain();

    if (dbg_wrk.oth_info == 0)
    {
        return;
    }

    SetInteger2(0, 20.0f, 30.0f, 1, 0x80, 0x80, 0x80, room_wrk.room_no);
    SetInteger2(0, 70.0f, 30.0f, 1, 0x80, 0x80, 0x80, room_wrk.disp_no[0]);
    SetInteger2(0, 120.0f, 30.0f, 1, 0x80, 0x80, 0x80, room_wrk.disp_no[1]);
    SetInteger2(0, 180.0f, 30.0f, 1, 0x80, 0x80, 0x80, plyr_wrk.pr_info.room_no);
    SetInteger2(0, 20.0f, 50.0f, 1, 0x80, 0x80, 0x80, plyr_wrk.pr_info.camera_no);
    SetInteger2(0, 90.0f, 50.0f, 1, 0x80, 0x80, 0x80, plyr_wrk.pr_info.camera_btl);
    SetInteger2(0, 160.0f, 50.0f, 1, 0x80, 0x80, 0x80, plyr_wrk.pr_info.camera_drm);
    SetInteger2(0, 230.0f, 50.0f, 1, 0x80, 0x80, 0x80, plyr_wrk.pr_info.camera_door);
    SetInteger2(0, 30.0f, 80.0f, 1, 0x80, 0x80, 0x80, plyr_wrk.move_box.pos[0]);
    SetInteger2(0, 100.0f, 80.0f, 1, 0x80, 0x80, 0x80, plyr_wrk.move_box.pos[2]);
    SetInteger2(0, 170.0f, 80.0f, 1, 0x80, 0x80, 0x80, room_wrk.room_height);
    SetInteger2(0, 240.0f, 80.0f, 1, 0x80, 0x80, 0x80, plyr_wrk.move_box.pos[1]);
    SetInteger2(0, 30.0f, 100.0f, 1, 0x80, 0x80, 0x80, room_wrk.pos[0][0]);
    SetInteger2(0, 100.0f, 100.0f, 1, 0x80, 0x80, 0x80, room_wrk.pos[0][1]);
    SetInteger2(0, 170.0f, 100.0f, 1, 0x80, 0x80, 0x80, room_wrk.pos[0][2]);
    SetInteger2(0, 30.0f, 120.0f, 1, 0x80, 0x80, 0x80, camera.i[0]);
    SetInteger2(0, 100.0f, 120.0f, 1, 0x80, 0x80, 0x80, camera.i[1]);
    SetInteger2(0, 170.0f, 120.0f, 1, 0x80, 0x80, 0x80, camera.i[2]);
    SetInteger2(0, 30.0f, 140.0f, 1, 0x80, 0x80, 0x80, camera.p[0]);
    SetInteger2(0, 100.0f, 140.0f, 1, 0x80, 0x80, 0x80, camera.p[1]);
    SetInteger2(0, 170.0f, 140.0f, 1, 0x80, 0x80, 0x80, camera.p[2]);
    SetInteger2(0, 30.0f, 160.0f, 1, 0x80, 0x80, 0x80, item_dsp_wrk[0][0].pos[0]);
    SetInteger2(0, 100.0f, 160.0f, 1, 0x80, 0x80, 0x80, item_dsp_wrk[0][0].pos[1]);
    SetInteger2(0, 170.0f, 160.0f, 1, 0x80, 0x80, 0x80, item_dsp_wrk[0][0].pos[2]);
    SetInteger2(0, 100.0f, 300.0f, 1, 0x80, 0x80, 0x80, (int)(RAD2DEG(plyr_wrk.move_box.rot[1]) + 360.0f) % 360);
    SetInteger2(0, 30.0f, 350.0f, 1, 0x80, 0x80, 0x80, plyr_wrk.move_box.pos[0] - room_wrk.pos[0][0]);
    SetInteger2(0, 100.0f, 350.0f, 1, 0x80, 0x80, 0x80, plyr_wrk.move_box.pos[1] - room_wrk.pos[0][1]);
    SetInteger2(0, 170.0f, 350.0f, 1, 0x80, 0x80, 0x80, plyr_wrk.move_box.pos[2] - room_wrk.pos[0][2]);
    SetInteger2(0, 50.0f, 400.0f, 1, 0x80, 0x80, 0x80, room_wrk.mot_num);
    SetInteger2(0, 120.0f, 400.0f, 1, 0x80, 0x80, 0x80, plyr_wrk.move_mot);

    get_room = GetPointRoom(25000, 5500);
    get_y = GetPointHeight(plyr_wrk.move_box.pos[0], plyr_wrk.move_box.pos[2]);

    SetInteger2(0, 100.0f, 190.0f, 1, 0x80, 0x80, 0x80, get_room);
    SetInteger2(0, 170.0f, 190.0f, 1, 0x80, 0x80, 0x80, get_y);
    SetInteger2(0, 170.0f, 220.0f, 1, 0x80, 0x80, 0x80, GetNowHeight());
    SetInteger2(0, 100.0f, 220.0f, 1, 0x80, 0x80, 0x80, plyr_wrk.move_box.pos[1]);
}

void GetNewRoomData()
{
    int i;

    room_wrk.room_no = GetNowRoom();

    GetPlayerRoom(room_wrk.room_no);
    GetRoomDispPos(room_wrk.room_no);

    room_wrk.camera_num = GetDataNum(1, plyr_wrk.pr_info.room_no);
    room_wrk.camera_bnum = GetDataNum(2, plyr_wrk.pr_info.room_no);
    room_wrk.camera_tnum = GetDataNum(4, plyr_wrk.pr_info.room_no);
    room_wrk.height_num = GetDataNum(5 ,plyr_wrk.pr_info.room_no);

    room_wrk.hit_num_next = room_wrk.hit_num;

    room_wrk.hit_num = GetDataNum(6, plyr_wrk.pr_info.room_no);
    room_wrk.ev_num = GetDataNum(7, plyr_wrk.pr_info.room_no);
    room_wrk.find_num = GetDataNum(9, plyr_wrk.pr_info.room_no);
    room_wrk.mot_num = GetDataNum(12, plyr_wrk.pr_info.room_no);
    room_wrk.se_num = GetDataNum(8, plyr_wrk.pr_info.room_no);

    RoomPass(room_wrk.disp_no[0]);
    RoomPass(room_wrk.disp_no[1]);
    DoorPassRoom(room_wrk.disp_no[0]);
    DoorPassRoom(room_wrk.disp_no[1]);

    plyr_wrk.pr_info.camera_no = GetNowCameraEach(1, room_wrk.camera_num, &room_wrk.camera_no);
    plyr_wrk.pr_info.camera_btl = GetNowCameraEach(2, room_wrk.camera_bnum, &room_wrk.camera_bno);
    plyr_wrk.pr_info.camera_drm = CameraGetDoramaCameraNo(plyr_wrk.pr_info.camera_drm);
    plyr_wrk.pr_info.hight = GetNowHeight() + room_wrk.room_height;

    for (i = 0; i < 60; i++)
    {
        if (furn_wrk[i].use == 4)
        {
            furn_wrk[i].room_id = room_wrk.disp_no[0];
        }
    }

    FurnDataRenewNow();
    ItemDispDataRenew();
    SetRoomReverbVol(map_wrk.now_room);

    if (area_read_wrk.stat == 0)
    {
        DoorDataRenewNow();
    }
}

u_char RoomWrkRenew()
{
    room_wrk.disp_bak[0] = room_wrk.disp_no[0];
    room_wrk.disp_bak[1] = room_wrk.disp_no[1];

    if (RoomChangeJudge() == 0)
    {
        GetNewRoomData();
        FurnSortFurnWrk(1);

        return 1;
    }

    return 0;
}

void CameraNoRenewEach(u_char cam_type)
{
    if (dbg_wrk.cam_mode == 2)
    {
        return;
    }

    switch (cam_type)
    {
    case 1:
        if (CameraChangeJudgeEach(1, room_wrk.camera_no) == 0)
        {
            plyr_wrk.pr_info.camera_no = GetNowCameraEach(1, room_wrk.camera_num, &room_wrk.camera_no);
        }
    break;

    case 2:
        if (CameraChangeJudgeEach(2, room_wrk.camera_bno) == 0)
        {
            plyr_wrk.pr_info.camera_btl = GetNowCameraEach(2, room_wrk.camera_bnum, &room_wrk.camera_bno);
        }
    break;

    case 3:
        plyr_wrk.pr_info.camera_drm = CameraGetDoramaCameraNo(plyr_wrk.pr_info.camera_drm);
    break;

    case 4:
        if (plyr_wrk.pr_info.camera_door != 0xffff)
        {
            plyr_wrk.pr_info.camera_door = CameraCheckDoorCameraNo(plyr_wrk.pr_info.camera_door_did, plyr_wrk.pr_info.camera_door_rid);

            if (plyr_wrk.pr_info.camera_door == 0xffff && plyr_wrk.mode != PMODE_FINDER && plyr_wrk.pr_info.cam_type == 3)
            {
                AllCloseConnectDoor();
            }
        }
    break;
    }
}

static void CameraNoRenew()
{
    if (CameraChangeJudge() == 0)
    {
       plyr_wrk.pr_info.camera_no = GetNowCamera();
    }
}

void DbgGetDrmCamNo()
{
    int j;

    if (*key_now[5] == 1)
    {
        for (j = 0; j < 10; j++)
        {
            // debug code
        }
    }

}

static u_short CameraGetDoramaCameraNo(u_short now_cam_no)
{
    u_int *addr_ui0;
    u_int *addr_ui1;
    u_short *dcamp;
    int i;
    int j;
    u_short cam_no[2] = {0xffff, 0xffff};
    u_char room_no;
    u_char data_num;
    u_char get_flg;
    u_char now_camera_get_flg;
    u_char drm_cam_no_get_flg;
    static int count = 0;
    u_int *dat_addr;

    get_flg = 0;

    now_camera_get_flg = 0;
    drm_cam_no_get_flg = 0;

    room_no = GetDataRoom(3, plyr_wrk.pr_info.room_no);

    if (room_no == 0xff)
    {
        now_cam_no = 0xffff;
        return 0xffff;
    }

    dat_addr = (u_int *)map_wrk.dat_adr;
    addr_ui0 = (u_int *)(dat_addr[3] + BASE_ADDRESS);
    addr_ui0 = (u_int *)((u_int)&addr_ui0[1] + room_no * 4);
    addr_ui0 = (u_int *)(addr_ui0[0] + BASE_ADDRESS);
    data_num = *(u_char *)(addr_ui0[0] + BASE_ADDRESS);

    addr_ui0++;

    for (i = 0; i < data_num; addr_ui0++, i++)
    {
        if (PosInAreaJudge1(3, room_no, i, plyr_wrk.move_box.pos[2], plyr_wrk.move_box.pos[0]) == 0)
        {
            continue;
        }

        addr_ui1 = (u_int *)(*addr_ui0 + BASE_ADDRESS);

        if (get_flg)
        {
            cam_no[1] = *(u_short *)(*addr_ui1 + BASE_ADDRESS);
        }
        else
        {
            cam_no[1] = cam_no[0] = *(u_short *)(*addr_ui1 + BASE_ADDRESS);

            get_flg = 1;
        }

        for (j = 0, dcamp = drm_cam_no; j < 10; j++, dcamp++)
        {
            if (dcamp[0] == 0xffff || dcamp[0] != cam_no[1])
            {
                continue;
            }

            if (!drm_cam_no_get_flg)
            {
                cam_no[0] = cam_no[1];

                now_camera_get_flg = 0;
                drm_cam_no_get_flg = 1;
            }

            break;
        }

        if (drm_cam_no_get_flg && j >= 10)
        {
            continue;
        }

        if (now_cam_no == 0xffff)
        {
            continue;
        }

        if (!now_camera_get_flg)
        {
            if (now_cam_no == cam_no[1]) now_camera_get_flg = 1;
        }
    }

    if (now_camera_get_flg)
    {
        return now_cam_no;
    }

    return cam_no[0];
}

u_char gtmp_cam_type = 0;
u_char gtmp_room = 0;
u_short gtmp_cam_no = 0;

u_short CameraGetDoorCameraNo(u_short door_id0, u_short door_id1)
{
    CAMERA4_DATA_POP *cdpp;
    u_int *addr_ui0;
    u_int *addr_ui1;
    int i;
    int j;
    u_short dr_id[2] = {door_id0, door_id1};
    u_char room_no;
    u_char data_num;
    u_int *dat_addr;

    room_no = GetDataRoom(4, plyr_wrk.pr_info.room_no);

    if (room_no == 0xff)
    {
        return 0xffff;
    }

    dat_addr = (u_int *)map_wrk.dat_adr;
    addr_ui0 = (u_int *)(dat_addr[4] + BASE_ADDRESS);
    addr_ui0 = (u_int *)((u_int)&addr_ui0[1] + room_no * 4);
    addr_ui0 = (u_int *)(addr_ui0[0] + BASE_ADDRESS);

    data_num = *(u_char *)(addr_ui0[0] + BASE_ADDRESS);

    addr_ui0++;

    for (i = 0; i < data_num; addr_ui0++, i++)
    {
        addr_ui1 = (u_int *)(addr_ui0[0] + BASE_ADDRESS);
        cdpp = (CAMERA4_DATA_POP *)(addr_ui1[0] + BASE_ADDRESS);

        if (cdpp->room_id != plyr_wrk.pr_info.room_no)
        {
            continue;
        }

        for (j = 0; j < 2; j++)
        {
            if (dr_id[j] == 0xffff)
            {
                continue;
            }

            if (dr_id[j] == cdpp->door_id[0] || dr_id[j] == cdpp->door_id[1])
            {
                if (PosInAreaJudge1(4, room_no, i, plyr_wrk.move_box.pos[2], plyr_wrk.move_box.pos[0]))
                {
                    return cdpp->camera_no;
                }
            }
        }
    }

    return 0xffff;
}

static u_short CameraCheckDoorCameraNo(u_short door_id, u_char room_id)
{
    CAMERA4_DATA_POP *cdpp;
    u_int *addr_ui0;
    u_int *addr_ui1;
    u_int *addr_ui2;
    int i;
    u_char room_no;
    u_char data_num;
    int *dat_adr;

    room_no = GetDataRoom(4, plyr_wrk.pr_info.room_no);

    if (room_no == 0xff)
    {
        return -1;
    }

    if (door_id == 0xffff)
    {
        return -1;
    }

    dat_adr = &((int *)(map_wrk.dat_adr))[4];

    addr_ui0 = (u_int *)(*dat_adr + BASE_ADDRESS);
    addr_ui0 = &addr_ui0[room_no + 1];
    addr_ui0 = (u_int *)((int *)(*addr_ui0 + BASE_ADDRESS));

    data_num = *(u_char *)(*addr_ui0 + BASE_ADDRESS);

    addr_ui0++;

    addr_ui2 = addr_ui0;

    for (i = 0; i < data_num; i++)
    {
        addr_ui1 = (u_int *)((int *)(*addr_ui0 + BASE_ADDRESS));
        cdpp = (CAMERA4_DATA_POP *)(*addr_ui1 + BASE_ADDRESS);

        if (cdpp->room_id == room_id)
        {
            if (door_id == cdpp->door_id[0] || door_id == cdpp->door_id[1])
            {
                if (PosInAreaJudge1(4, room_no, i, plyr_wrk.move_box.pos[2], plyr_wrk.move_box.pos[0]) != 0)
                {
                    return cdpp->camera_no;
                }
            }
        }

        addr_ui0++;
    }

    addr_ui0 = addr_ui2;

    for (i = 0; i < data_num; i++)
    {
        addr_ui1 = (u_int *)(*addr_ui0 + BASE_ADDRESS);
        cdpp = (CAMERA4_DATA_POP *)(*addr_ui1 + BASE_ADDRESS);

        if (cdpp->room_id == room_id)
        {
            if (door_id == cdpp->door_id[0] || door_id == cdpp->door_id[1])
            {
                if (PosInAreaJudge1(4, room_no, i, plyr_wrk.move_box.pos[2], plyr_wrk.move_box.pos[0]) != 0)
                {
                    return cdpp->camera_no;
                }
            }
        }

        addr_ui0++;
    }

    return -1;
}

static void PlayerHeightRenew()
{
    if (HeightChangeJudge() == 0)
    {
        plyr_wrk.pr_info.hight = GetNowHeight() + room_wrk.room_height;
    }
}

static u_char GetRoomNum()
{
    int *addr;

    addr = (int *)(map_wrk.dat_adr);
    addr = (int *)(*addr + BASE_ADDRESS);
    addr = (int *)(*addr + BASE_ADDRESS);

    return *(u_char *)addr;
}

static u_char GetRoomNumFloor(u_char floor)
{
    int *addr;

    addr = (int *)GetFloorTopAddr(floor);
    addr = (int *)(*addr + BASE_ADDRESS);
    addr = (int *)(*addr + BASE_ADDRESS);

    return *(u_char *)addr;
}

static u_char GetDataNum(u_char map, u_char room)
{
    int *addr;
    u_char data_room;

    data_room = GetDataRoom(map, room);

    if (data_room == 0xff)
    {
        return 0;
    }

    addr = (int *)(map_wrk.dat_adr + map * 4);
    addr = (int *)(addr[0] + BASE_ADDRESS + (data_room * 4));
    addr = (int *)(addr[1] + BASE_ADDRESS);
    addr = (int *)(addr[0] + BASE_ADDRESS);

    return *addr;
}

/* /home/m_nagaura/reikoku/src/ingame/map/map_ctrl.c */

static u_char RoomChangeJudge()
{
    return PosInAreaJudge0(room_wrk.room_no, plyr_wrk.move_box.pos[2], plyr_wrk.move_box.pos[0]);
}

static u_char CameraChangeJudgeEach(u_char cam_type, u_short cam_no)
{
    u_char room_no;

    gtmp_cam_type = cam_type;
    gtmp_cam_no = cam_no;

    room_no = GetDataRoom(cam_type, plyr_wrk.pr_info.room_no);
    gtmp_room = room_no;

    return PosInAreaJudge1(cam_type, room_no, cam_no, plyr_wrk.move_box.pos[2], plyr_wrk.move_box.pos[0]);
}

static u_char CameraChangeJudge()
{
    return PosInAreaJudge1(1, GetDataRoom(1, plyr_wrk.pr_info.room_no), room_wrk.camera_no, plyr_wrk.move_box.pos[2], plyr_wrk.move_box.pos[0]);
}

static u_char HeightChangeJudge()
{
    return PosInAreaJudge1(
        5, GetDataRoom(5, plyr_wrk.pr_info.room_no),
        room_wrk.height_no, plyr_wrk.move_box.pos[2], plyr_wrk.move_box.pos[0]
    );
}

static u_char GetNowRoom()
{
    u_char i;

    for (i = 0; i < room_wrk.room_num; i++)
    {
        if (PosInAreaJudge0(i, plyr_wrk.move_box.pos[2], plyr_wrk.move_box.pos[0]))
        {
            return i;
        }
    }

    return 0xff;
}

u_char GetRoomIdFromRoomNo(u_char map, u_char room_no)
{
    int *addr0;
    u_char *addr1;
    int i;
    u_char room_id;
    u_char room_num;

    if (map == 10)
    {
        return GetDoorRoomIdFromRoomNo(room_no);
    }

    addr0 = (int *)(map_wrk.dat_adr + map * 4);
    addr0 = (int *)(*addr0 + BASE_ADDRESS);
    addr1 = (u_char *)(*addr0 + BASE_ADDRESS);
    room_num = *(u_char *)(*addr0 + BASE_ADDRESS);

    room_id = 0xff;

    addr1++;

    for (i = 0; i < room_num; i++)
    {
        if (i == room_no)
        {
            room_id = *addr1;

            break;
        }

        addr1++;
    }

    return room_id;
}

u_char GetRoomIdFromRoomNoFloor(u_char map, u_char room_no, u_char floor)
{
    int *addr0;
    u_char *addr1;
    int i;
    u_char room_id;
    u_char room_num;

    if (map == 10)
    {
        return GetDoorRoomIdFromRoomNoFloor(room_no, floor);
    }

    addr0 = (int *)(GetFloorTopAddr(floor) + map * 4);
    addr0 = (int *)(*addr0 + BASE_ADDRESS);

    addr1 = (u_char *)(*addr0 + BASE_ADDRESS);

    room_num = *(u_char *)(*addr0 + BASE_ADDRESS);

    room_id = 0xff;

    addr1++;

    for (i = 0; i < room_num; i++)
    {
        if (i == room_no)
        {
            room_id = *addr1;

            break;
        }

        addr1++;
    }

    return room_id;
}

static u_char GetDoorRoomIdFromRoomNo(u_char room_no)
{
    u_char *addr1;
    int i;
    u_char room_num;
    u_char room_id;

    addr1 = GetDoorRoomNumP();

    room_num = *addr1;

    room_id = 0xff;

    addr1++;

    for(i = 0; i < room_num; i++)
    {
        if (room_no == i)
        {
            room_id = *addr1;

            break;
        }

        addr1++;
    }

    return room_id;
}

static u_char GetDoorRoomIdFromRoomNoFloor(u_char room_no, u_char floor)
{
    u_char *addr1;
    int i;
    u_char room_num;
    u_char room_id;

    addr1 = GetDoorRoomNumPFloor(floor);

    room_num = *addr1;

    room_id = 0xff;

    addr1++;

    for (i = 0; i < room_num; i++)
    {
        if (room_no == i)
        {
            room_id = *addr1;

            break;
        }

        addr1++;
    }

    return room_id;
}

u_char GetDataRoom(u_char map, u_char room_id)
{
    int *addr0;
    u_char *addr1;
    u_char i;
    u_char room_num;

    if (map == 10)
    {
        return GetDoorDataRoom(room_id);
    }

    addr0 = (int *)(map_wrk.dat_adr + map * 4);
    addr0 = (int *)(*addr0 + BASE_ADDRESS);
    addr1 = (u_char *)(*addr0 + BASE_ADDRESS);

    room_num = *addr1;

    addr1++;

    for (i = 0; i < room_num; i++)
    {
        if (room_id == *addr1)
        {
            return i;
        }

        addr1++;
    }

    return 0xff;
}

static u_char GetDoorDataRoom(u_char room_id)
{
    u_char *addr1;
    u_char i;
    u_char room_num;

    addr1 = GetDoorRoomNumP();

    room_num = *addr1++;

    for (i = 0; i < room_num; i++, addr1++)
    {
        if (room_id == *addr1)
        {
                return i;
        }
    }

    return 0xff;
}

static u_short GetNowCameraEach(u_char cam_type, u_char cam_num, u_char *rm_cam_no)
{
    u_char i;
    u_char room;
    int *addr;
    int val;

    room = GetDataRoom(cam_type, plyr_wrk.pr_info.room_no);

    for (i = 0; i < cam_num; i++)
    {
        if (PosInAreaJudge1(cam_type, room, i, plyr_wrk.move_box.pos[2], plyr_wrk.move_box.pos[0]) != 0)
        {
            *rm_cam_no = i;

            addr = (int *)(map_wrk.dat_adr + (cam_type * 4));
            addr = (int *)(*addr + BASE_ADDRESS);
            addr = &addr[room] + 1;
            addr = (int *)(*addr + BASE_ADDRESS);
            addr = &addr[i] + 1;
            addr = (int *)(*addr + BASE_ADDRESS);

            val = *(int *)(*addr + BASE_ADDRESS);

            return val;
        }
    }

    *rm_cam_no = 0xff;

    return -1;
}

static u_short GetNowCamera()
{
    u_char i;
    u_char room;
    int *addr;
    int val;

    room = GetDataRoom(1, plyr_wrk.pr_info.room_no);

    for ( i = 0; i < room_wrk.camera_num; i++)
    {
        if (PosInAreaJudge1(1, room, i, plyr_wrk.move_box.pos[2], plyr_wrk.move_box.pos[0]))
        {
            room_wrk.camera_no = i;

            addr = (int *)(map_wrk.dat_adr + 4);
            addr = (int *)(*addr + BASE_ADDRESS);
            addr = &addr[room] + 1;
            addr = (int *)(*addr + BASE_ADDRESS);
            addr = &addr[i] + 1;
            addr = (int *)(*addr + BASE_ADDRESS);

            val = *(int *)(*addr + BASE_ADDRESS);

            return val;
        }
    }

    return -1;
}

static float GetNowHeight()
{
    u_char i;
    u_char room;
    int *addr;
    short val;

    room = GetDataRoom(5, plyr_wrk.pr_info.room_no);

    for (i = 0; i < room_wrk.height_num; i++)
    {
        if (PosInAreaJudge1(5, room, i, plyr_wrk.move_box.pos[2], plyr_wrk.move_box.pos[0]))
        {
            room_wrk.height_no = i;

            addr = (int *)(map_wrk.dat_adr + 5 * 4);
            addr = (int *)(*addr + BASE_ADDRESS);
            addr = &addr[room] + 1;
            addr = (int *)(*addr + BASE_ADDRESS);
            addr = &addr[i] + 1;
            addr = (int *)(*addr + BASE_ADDRESS);

            val = *(short *)(*addr + BASE_ADDRESS);

            return val;
        }
    }

    return 0.0f;
}

int GetPointRoomNo(u_short pos_x, u_short pos_z)
{
    int i;

    for (i = 0; i < room_wrk.room_num; i++)
    {
        if (PosInAreaJudge0(i, pos_z, pos_x))
        {
            return i;
        }
    }

    return 0xff;
}

int GetPointRoomNoFloor(u_short pos_x, u_short pos_z, u_char floor)
{
    int i;
    u_char room_num;

    room_num = GetRoomNumFloor(floor);

    for (i = 0; i < room_num; i++)
    {
        if (PosInAreaJudge0Floor(i, pos_z, pos_x, floor))
        {
            GetFloorTopAddr(floor);

            return i;
        }
    }

    return 0xff;
}

int GetPointRoom(u_short pos_x, u_short pos_z)
{
    int *addr;

    addr = (int *)(map_wrk.dat_adr);
    addr = (int *)(*addr + BASE_ADDRESS);

    return *(u_char *)(*addr + GetPointRoomNo(pos_x, pos_z) + 1 + BASE_ADDRESS);
}

float GetPointHeight(u_short pos_x, u_short pos_z)
{
    u_char i;
    u_char room;
    int *addr;
    u_char room_id;
    short val1;
    short val2;

    GetPointRoom(pos_x, pos_z);

    room = GetPointRoomNo(pos_x, pos_z);

    addr = (int *)(map_wrk.dat_adr);
    addr = (int *)(*addr + BASE_ADDRESS);

    room_id = *(u_char *)(*addr + room + 1 + BASE_ADDRESS);

    room = GetDataRoom(5, room_id);

    for (i = 0; i < room_wrk.height_num; i++)
    {
        if (PosInAreaJudge1(5, room, i, pos_z, pos_x) != 0)
        {
            room_wrk.height_no = i;

            addr = (int *)(map_wrk.dat_adr + 5 * 4);
            addr = (int *)(*addr + BASE_ADDRESS);
            addr = &addr[room] + 1;
            addr = (int *)(*addr + BASE_ADDRESS);
            addr = &addr[i] + 1;
            addr = (int *)(*addr + BASE_ADDRESS);

            val1 = *(short *)(*addr + BASE_ADDRESS);

            addr = (int *)(*(int *)map_wrk.dat_adr + BASE_ADDRESS);
            addr = &addr[room] + 1;
            addr = (int *)(*addr + BASE_ADDRESS);

            val2 = *(short *)(*addr + 6 + BASE_ADDRESS);

            return val1 + val2;
        }
    }

    return 0.0f;
}

static void GetPlayerRoom(u_char room)
{
    int *addr;

    if (room == 0xff)
    {
        return;
    }

    addr = (int*)map_wrk.dat_adr;
    addr = (int*)(*addr + BASE_ADDRESS);

    plyr_wrk.pr_info.room_old = plyr_wrk.pr_info.room_no;
    plyr_wrk.pr_info.room_no = *(int *)(*addr + room + 1 + BASE_ADDRESS);
}

static void GetRoomDispPos(u_char room)
{
    int *addr;

    if (room == 0xff)
    {
        return;
    }

    addr = (int *)map_wrk.dat_adr;
    addr = (int *)(*addr + BASE_ADDRESS);

    room_wrk.disp_no[1] = room_wrk.disp_no[0];

    room_wrk.pos[1][0] = room_wrk.pos[0][0];
    room_wrk.pos[1][1] = room_wrk.pos[0][1];
    room_wrk.pos[1][2] = room_wrk.pos[0][2];

    room_wrk.disp_no[0] = *(u_char *)(*addr + room + 1 + BASE_ADDRESS);

    map_wrk.now_room = room_wrk.disp_no[0];
    map_wrk.next_room = room_wrk.disp_no[1];

    addr = &addr[room] + 1;
    addr = (int *)(*addr + BASE_ADDRESS);

    room_wrk.pos[0][0] = ((u_short *)(*addr + BASE_ADDRESS))[0];
    room_wrk.pos[0][1] = ((short *)(*addr + BASE_ADDRESS))[1];
    room_wrk.pos[0][2] = ((u_short *)(*addr + BASE_ADDRESS))[2];

    room_wrk.room_height = ((short *)(*addr + BASE_ADDRESS))[3];
}

u_char PosInAreaJudge0(u_char room, u_short pos_x, u_short pos_y)
{
    int i;
    int *addr;
    int *addr_bak;
    u_char sq_num;
    char type;

    if (room == 0xff)
    {
        return 0;
    }

    addr = (int *)map_wrk.dat_adr;
    addr = (int *)(*addr + BASE_ADDRESS);
    addr = &addr[room] + 1;
    addr = (int *)(*addr + BASE_ADDRESS);

    sq_num = *(u_char *)(*addr + 8 + BASE_ADDRESS);

    addr_bak = addr;

    for (i = 0; i < sq_num; i++)
    {
        type = *(char *)(*addr + i + 9 + BASE_ADDRESS);

        addr = &addr[i] + 1;
        addr = (int *)(*addr + BASE_ADDRESS);

        if (PosInAreaJudgeSub(addr, pos_x, pos_y, type) != 0)
        {
            return 1;
        }

        addr = addr_bak;
    }

    return 0;
}

static u_char PosInAreaJudge0Floor(u_char room, u_short pos_x, u_short pos_y, u_char floor)
{
    int i;
    int *addr;
    int *addr_bak;
    u_char sq_num;
    char type;

    if (floor_exist[ingame_wrk.msn_no][floor] == 0)
    {
        return 0;
    }

    if (room == 0xff)
    {
        return 0;
    }

    addr = (int *)GetFloorTopAddr(floor);
    addr = (int *)(*addr + BASE_ADDRESS);
    addr = &addr[room] + 1;
    addr = (int *)(*addr + BASE_ADDRESS);

    sq_num = *(u_char *)(*addr + 8 + BASE_ADDRESS);

    addr_bak = addr;

    for (i = 0; i < sq_num; i++)
    {
        type = *(char *)(*addr + i + 9 + BASE_ADDRESS);

        addr = &addr[i] + 1;
        addr = (int *)(*addr + BASE_ADDRESS);

        if (PosInAreaJudgeSub(addr, pos_x, pos_y, type) != 0)
        {
            return 1;
        }

        addr = addr_bak;
    }

    return 0;
}

u_char PosInAreaJudge1(u_char map, u_char room, u_char data, u_short pos_x, u_short pos_y)
{
    int i;
    int *addr;
    int *addr_bak;
    u_char sq_num;
    u_char ofs[13] = { 8, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 24, 4 };
    char type;

    if (room == 0xff)
    {
        return 0;
    }

    addr = (int *)map_wrk.dat_adr + map;
    addr = (int *)(*addr + BASE_ADDRESS);
    addr = &addr[room] + 1;
    addr = (int *)(*addr + BASE_ADDRESS);
    addr = &addr[data] + 1;
    addr = (int *)(*addr + BASE_ADDRESS);

    sq_num = *(u_char *)(*addr + BASE_ADDRESS + ofs[map]);

    addr_bak = addr;

    for (i = 0; i < sq_num; i++)
    {
        type = *(char *)(int)(*addr + i + 1 + (long)BASE_ADDRESS + ofs[map]); // cast to long?

        addr = &addr[i] + 1;
        addr = (int *)(*addr + BASE_ADDRESS);

        if (PosInAreaJudgeSub(addr, pos_x, pos_y, type) != 0)
        {
            return 1;
        }

        addr = addr_bak;
    }

    return 0;
}

u_char MapHitCheck(u_short pos_x, u_short pos_y, u_char room_id)
{
    int i;
    u_char room_no;

    room_no = GetDataRoom(6, room_id);

    for (i = 0; i < room_wrk.hit_num; i++)
    {
        if (PosInAreaJudge1(6, room_no, i, pos_x, pos_y) != 0)
        {
            return 1;
        }
    }

    return 0;
}

u_char MapHitCheckAll(u_short pos_x, u_short pos_y, u_char room_id)
{
    u_char hit_flg;

    if (MapHitCheck(pos_x, pos_y, room_id) != 0)
    {
        hit_flg = 1;
    }
    else
    {
        hit_flg = 0;
    }

    if (!hit_flg)
    {
        if (FurnHitCheck2(pos_x, pos_y, room_id) != 0)
        {
            hit_flg = 1;
        }
        else
        {
            hit_flg = 0;
        }

        if (!hit_flg)
        {
            if (DoorHitCheck2(pos_x, pos_y, room_id) != 0)
            {
                hit_flg = 1;
            }
            else
            {
                hit_flg = 0;
            }
        }
    }

    return hit_flg;
}

void PlyrHitRot(sceVu0FVECTOR pos, sceVu0FVECTOR dst, float *go_rot, float *no_rot, u_char div, u_char room_id)
{
    sceVu0FVECTOR rpos;
    float chk_rot;
    u_short pos_x;
    u_short pos_y;

    chk_rot = (*go_rot + *no_rot) * 0.5f;

    if (__builtin_fabs(*go_rot - *no_rot) >= 0.03490658476948738)
    {
        rpos[0] = SgCosf(chk_rot) * (dst[0] * SgCosf(chk_rot) + dst[2] * SgSinf(chk_rot));
        rpos[2] = SgCosf(chk_rot) * (-dst[0] * SgSinf(chk_rot) + dst[2] * SgCosf(chk_rot));

        pos_y = rpos[0] + pos[0];
        pos_x = rpos[2] + pos[2];

        if (MapHitCheckAll(pos_x, pos_y, room_id) != 0)
        {
            *no_rot = chk_rot;
        }
        else
        {
            *go_rot = chk_rot;
        }

        PlyrHitRot(pos, dst, go_rot, no_rot, div, room_id);
    }
}

u_char PlyrMapHitCheck(sceVu0FVECTOR dst, sceVu0FVECTOR pos, u_char div, u_char room_id)
{
    sceVu0FVECTOR rpos;
    float chk_rot;
    float chk_rotbk[2];
    float ok_rot[2];
    int i;
    int j;
    u_char div_bk;
    u_char ng_flg[2];
    u_short pos_x;
    u_short pos_y;

    pos_y = pos[0] + dst[0];
    pos_x = pos[2] + dst[2];

    if (MapHitCheckAll(pos_x, pos_y, room_id) == 0)
    {
        return 0;
    }

    div_bk = 0;

    for(i = 0; i < div - 1; i++)
    {
        pos_y = (pos[0] + dst[0] * (i + 1)) / div;
        pos_x = (pos[2] + dst[2] * (i + 1)) / div;

        if (MapHitCheckAll(pos_x, pos_y, room_id))
        {
            div_bk = i;
            break;
        }
    }

    if (div_bk != 0)
    {
        dst[0] = dst[0] * div_bk / div;
        dst[2] = dst[2] * div_bk / div;

        return 1;
    }

    for (i = 0; i < 2; i++)
    {
        ng_flg[i] = 0;

        if (i != 0)
        {
            chk_rot = -1.5607964f; // almost -PI / 2;
        }
        else
        {
            chk_rot = +1.5607964f; // almost +PI / 2;
        }

        chk_rotbk[0] = 0.0f;
        chk_rotbk[1] = chk_rot;

        rpos[0] = +dst[0] * SgCosf(chk_rot) + dst[2] * SgSinf(chk_rot);
        rpos[2] = -dst[0] * SgSinf(chk_rot) + dst[2] * SgCosf(chk_rot);

        pos_y = rpos[0] + pos[0];
        pos_x = rpos[2] + pos[2];

        if (MapHitCheckAll(pos_x, pos_y, room_id) != 0)
        {
            ok_rot[i] = PI / 2;
            ng_flg[i] = 1;
        }
        else
        {
            for (j = 0; j < 7; j++)
            {
                chk_rot = (chk_rotbk[0] + chk_rotbk[1]) * 0.5f;

                rpos[0] =  dst[0] * SgCosf(chk_rot) + dst[2] * SgSinf(chk_rot);
                rpos[2] = -dst[0] * SgSinf(chk_rot) + dst[2] * SgCosf(chk_rot);

                pos_y = rpos[0] + pos[0];
                pos_x = rpos[2] + pos[2];

                if (MapHitCheckAll(pos_x, pos_y, room_id) == 0)
                {
                    chk_rotbk[1] = chk_rot;
                }
                else
                {
                    chk_rotbk[0] = chk_rot;
                }
            }

            ok_rot[i] = chk_rotbk[1];
        }
    }

    if (__builtin_fabsf(ok_rot[0]) > __builtin_fabsf(ok_rot[1]))
    {
        ok_rot[0] = ok_rot[1];
    }
    else
    {
        if (ng_flg[0] == ng_flg[1] && ng_flg[0] == 1)
        {
            dst[0] = 0.0f;
            dst[2] = 0.0f;

            return 1;
        }
    }

    rpos[0] = SgCosf(ok_rot[0]) * ( dst[0] * SgCosf(ok_rot[0]) + dst[2] * SgSinf(ok_rot[0]));
    rpos[2] = SgCosf(ok_rot[0]) * (-dst[0] * SgSinf(ok_rot[0]) + dst[2] * SgCosf(ok_rot[0]));

    pos_y = rpos[0] + pos[0];
    pos_x = rpos[2] + pos[2];

    if (MapHitCheckAll(pos_x, pos_y, room_id) == 0)
    {
        dst[0] = rpos[0];
        dst[2] = rpos[2];
    }
    else
    {
        dst[0] = 0.0f;
        dst[2] = 0.0f;
    }

    return 1;
}

int PlyrMapHitMoveOfs(sceVu0FVECTOR dst, sceVu0FVECTOR pos, u_char room, u_char flr_room)
{
    int i;
    int j;
    int k;
    int x_ofs;
    int z_ofs;
    float lng_cmp;
    sceVu0FVECTOR cpos;
    sceVu0FVECTOR dpos;
    float dist;
    int x_ofs_tmp;
    int z_ofs_tmp;

    x_ofs = 0;
    z_ofs = 0;

    dpos[0] = pos[0] + dst[0];
    dpos[1] = pos[1] + dst[1];
    dpos[2] = pos[2] + dst[2];

    lng_cmp = GetDistV(dpos, pos);

    for (i = 0; i < 11; i++)
    {
        for (j = 0; j < 11; j++)
        {
            cpos[0] = pos[0] + i - 5;
            cpos[1] = pos[1];
            cpos[2] = pos[2] + j - 5;

            for (k = 0; k < room_wrk.hit_num; k++)
            {
                if (PosInAreaJudge1(6, flr_room, k, cpos[2], cpos[0]) != 0)
                {
                    break;
                }
            }

            if (k == room_wrk.hit_num)
            {
                x_ofs_tmp = i - 5;
                z_ofs_tmp = j - 5;

                dist = GetDistV(dpos, cpos);

                if (lng_cmp > dist)
                {
                    x_ofs = x_ofs_tmp;
                    z_ofs = z_ofs_tmp;
                }
            }
        }
    }

    dst[0] = x_ofs;
    dst[2] = z_ofs;

    printf("x_ofs = %d   z_ofs = %d \n", x_ofs, z_ofs);

    return 0;
}

static void GetNowOpenEvent()
{
    u_char i;
    u_char room;
    u_char cnt;
    int *addr;

    if (ev_wrk.mode != 0)
    {
        return;
    }

    for (i = 0; i < 16; i++)
    {
        ev_wrk.pos_req[i] = 0xff;
    }

    cnt = 0;

    room = GetDataRoom(7, plyr_wrk.pr_info.room_no);

    for (i = 0; i < room_wrk.ev_num; i++)
    {
        if (PosInAreaJudge1(7, room, i, plyr_wrk.move_box.pos[2], plyr_wrk.move_box.pos[0]) != 0)
        {
            addr = (int *)(*(int *)(map_wrk.dat_adr + 7 * 4) + BASE_ADDRESS);
            addr = &addr[room] + 1;
            addr = (int *)(*addr + BASE_ADDRESS);
            addr = &addr[i] + 1;
            addr = (int *)(*addr + BASE_ADDRESS);

            ev_wrk.pos_req[cnt] = *(u_char *)(*addr + BASE_ADDRESS);

            cnt++;
        }
    }
}

static void GetNowOpenFindAct()
{
    u_char i;
    u_char room;
    u_char cnt;
    int *addr;

    if (find_wrk.mode != 0)
    {
        return;
    }

    for (i = 0; i < 10; i++)
    {
        find_wrk.pos_req[i] = 0xffff;
    }

    cnt = 0;

    room = GetDataRoom(9, plyr_wrk.pr_info.room_no);

    for (i = 0; i < room_wrk.find_num; i++)
    {
        if (PosInAreaJudge1(9, room, i, plyr_wrk.move_box.pos[2], plyr_wrk.move_box.pos[0]))
        {
            addr = (int *)(*(int *)(map_wrk.dat_adr + 9 * 4) + BASE_ADDRESS);
            addr = &addr[room] + 1;
            addr = (int *)(*addr + BASE_ADDRESS);
            addr = &addr[i] + 1;
            addr = (int *)(*addr + BASE_ADDRESS);

            find_wrk.pos_req[cnt] = *(u_short *)(*addr + BASE_ADDRESS);

            cnt++;
        }
    }
}

static void GetNowPlayerMoveMotion()
{
    plyr_wrk.move_mot = GetPointMoveMotion(plyr_wrk.move_box.pos, plyr_wrk.pr_info.room_no);
}

u_char GetPointMoveMotion(sceVu0FVECTOR p, u_char no)
{
    int *addr;
    u_char i;
    u_char room;
    u_char mot;

    mot = 0xff;

    room = GetDataRoom(12, no);

    for (i = 0; i < room_wrk.mot_num; i++)
    {
        if (PosInAreaJudge1(12, room, i, p[2], p[0]) != 0)
        {
            addr = (int *)(*(int *)(map_wrk.dat_adr + 12 * 4) + BASE_ADDRESS);
            addr = &addr[room] + 1;
            addr = (int *)(*addr + BASE_ADDRESS);
            addr = &addr[i] + 1;
            addr = (int *)(*addr + BASE_ADDRESS);

            mot = *(u_char *)(*addr + BASE_ADDRESS);
        }
    }

    return mot;
}

u_char MapCameraCdivideAB(u_short data_no, u_short *xmin, u_short *xmax, u_short *ymin, u_short *ymax, u_char cam_type)
{
    int i;
    int x_cnt, y_cnt;
    int *addr;
    int *dat_addr;
    int cmpx[2];
    int cmpy[2];
    u_char room;
    int *dat_addr2;

    room = GetDataRoom(cam_type, plyr_wrk.pr_info.room_no);

    if (room == 0xff)
    {
        return 0xff;
    }

    addr = (int *)map_wrk.dat_adr + cam_type;
    addr = (int *)(*addr + BASE_ADDRESS);
    addr = &addr[room] + 1;
    addr = (int *)(*addr + BASE_ADDRESS);

    i = 0;

    while (1)
    {
        dat_addr = &addr[i] + 1;
        dat_addr = (int *)(*dat_addr + BASE_ADDRESS);

        if (data_no == *(short *)(*dat_addr + BASE_ADDRESS))
        {
            break;
        }

        i++;
    }

    dat_addr = &addr[i] + 1;
    addr = (int *)(*dat_addr + BASE_ADDRESS);
    dat_addr2 = (int *)addr[1];

    x_cnt = 0;
    y_cnt = 0;

    for (i = 0; i < 4; i++)
    {
        if (((short *)((int)dat_addr2 + BASE_ADDRESS))[1] != 0)
        {
            if (x_cnt > 1)
            {
                return 0;
            }

            cmpx[x_cnt] = ((int*)((int)dat_addr2 + BASE_ADDRESS))[1] / ((short *)((int)dat_addr2 + BASE_ADDRESS))[1];

            if (cmpx[x_cnt] < 0)
            {
                cmpx[x_cnt] = -cmpx[x_cnt];
            }

            x_cnt++;
        }
        else
        {
            if (((short *)((int)dat_addr2 + BASE_ADDRESS))[0] != 0)
            {
                cmpy[y_cnt] = ((int *)((int)dat_addr2 + BASE_ADDRESS))[1] / ((short *)((int)dat_addr2 + BASE_ADDRESS))[0];

                if (cmpy[y_cnt] < 0)
                {
                    cmpy[y_cnt] = -cmpy[y_cnt];
                }
            }
            else
            {
                cmpy[y_cnt] = 0;
            }

            y_cnt++;
        }

         dat_addr2 += 2;
    }

    if (cmpx[0] > cmpx[1])
    {
        *xmax = cmpx[0];
        *xmin = cmpx[1];
    }
    else
    {
        *xmin = cmpx[0];
        *xmax = cmpx[1];
    }

    if (cmpy[0] > cmpy[1])
    {
        *ymax = cmpy[0];
        *ymin = cmpy[1];
    }
    else
    {
        *ymin = cmpy[0];
        *ymax = cmpy[1];
    }

    return 1;
}

void NakasuHazeSet()
{
    int i;
    u_char on_flg;

    on_flg = 0;

    for (i = 0; i < 2; i++)
    {
        if (room_wrk.disp_no[i] == 22)
        {
            on_flg = 1;

            break;
        }
    }

    if (on_flg)
    {
        SetHaze_Pond_SW(1);
    }
    else
    {
        SetHaze_Pond_SW(0);
    }
}

void DebugMemoryCheck(u_char *addr, int size)
{
    int i;

    for (i = 0; i < size; i++)
    {
        // debug code ...
    }
}

static void MapSetFloorSeNo()
{
    int *addr_si;
    int i;
    u_char room;
    u_char data_no;

    room = GetDataRoom(8, plyr_wrk.pr_info.room_no);

    data_no = 0xff;

    for (i = 0; i < room_wrk.se_num; i++)
    {
        if (PosInAreaJudge1(8, room, i, plyr_wrk.move_box.pos[2], plyr_wrk.move_box.pos[0]) != 0)
        {
            data_no = i;

            break;
        }
    }

    if (data_no == 0xff)
    {
        return;
    }

    addr_si = (int *)map_wrk.dat_adr + 2 * 4;
    addr_si = (int *)(*addr_si + BASE_ADDRESS);
    addr_si = &addr_si[room + 1];
    addr_si = (int *)(*addr_si + BASE_ADDRESS);
    addr_si = &addr_si[data_no + 1];
    addr_si = (int *)(*addr_si + BASE_ADDRESS);
    addr_si = (int *)(*addr_si + BASE_ADDRESS);

    switch (*(u_short *)addr_si)
    {
    case 0:
        plyr_wrk.pr_info.se_foot = 44;
    break;

    case 1:
        plyr_wrk.pr_info.se_foot = 45;
    break;

    case 2:
        plyr_wrk.pr_info.se_foot = 46;
    break;
    }
}

int GetRoomPos(u_char room_no, sceVu0FVECTOR room_pos)
{
    u_char dat_room;
    int *addr;

    dat_room = GetDataRoom(0, room_no);

    if (GetRoomIdFromRoomNo(0, dat_room) != room_no)
    {
        return 1;
    }

    addr = (int *)(map_wrk.dat_adr);
    addr = (int *)(*addr + BASE_ADDRESS);
    addr = &addr[dat_room] + 1;
    addr = (int *)(*addr + BASE_ADDRESS);

    room_pos[0] = ((u_short*)(*addr + BASE_ADDRESS))[0];
    room_pos[1] = ((short *)(*addr + BASE_ADDRESS))[1];
    room_pos[2] = ((u_short *)(*addr + BASE_ADDRESS))[2];

    return 0;
}
