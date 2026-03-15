#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ig_map.h"

#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph2d/tim2_new.h"
#include "graphics/graph3d/sglib.h"
#include "ingame/info/inf_disp.h"
#include "ingame/map/door_ctl.h"
#include "ingame/map/map_ctrl.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/ig_spd_menu.h"
#include "ingame/menu/item.h"
#include "main/glob.h"
#include "os/eeiop/eese.h"

static void MapCntInit();
static void MapCntRenew();
static void MapModeInOut();
static void MapModeInOut2();
static void DspPlyrInMap(u_char alp);
static void MapScoop();
static void MapMove(u_char alp);
static void MapInfo1(u_char alp);
static void MapInfo2(u_char alp);
static void MapPrint(short int mov_px, short int mov_py, u_char alp);
static void MapPlayer(short int mov_px, short int mov_py, u_char alp);
static void MapChrCtrl(short int pos_x, short int pos_y, u_char alp);
static void MapChrSet(u_char id, short int pos_x, short int pos_y, int rgb, u_char alp, float scl, int pri);
static void PutChrForMap(u_short chr_lbl, short int pos_x, short int pos_y, int rgb, u_char alp, float scl, int pri);
static void PutIcnForMap(u_short chr_lbl, short int pos_x, short int pos_y, short int rot, u_char alp, float scl);
static void PutIcnForMap2(u_char type, short int pos_x, short int pos_y, short int rot, u_char alp, float scl, u_char sz_ptn);
static void DspLayOut(short int pos_x, short int pos_y, u_char alpha);
static void MapTri(short int pos_x, short int pos_y, u_char alpha);
static u_char MapExstFlr(u_char id, u_char flr);
static void MapExistCHK();
static u_char MapMovableCHK();
static void SetLineStrip(int pri, u_char num, LINE_PRT *prot, u_short alp);
static void SetCircle(int pri, short int xo, short int yo, short int r, u_char dtl, u_char cr, u_char cg, u_char cb, u_char a);

u_char rm_in_flr1[29] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13,
    14, 15, 16, 17, 19, 21, 22, 23, 25, 26, 27, 29, 36, 40,
    0xff,
};
u_char rm_in_flr2[12] = {
    3, 8, 9, 14, 17, 18, 20, 24, 35, 38, 41, 0xff,
};
#include "data/map_door_pos.h" // MAP_DOOR_POS map_door_pos[];
#include "data/map_svp_pos.h" // MAP_SVP_POS map_svp_pos[];

u_char rm_in_flr0[8] = { 28, 29, 30, 31, 32, 33, 34, 0xff };
u_char rm_in_flr3[2] = { 20, 0xff };

static FLSH_CORE flsh[3];

static MAP_CTRL map;

#define PI 3.1415927f

void NewgameMenuMapInit()
{
    return;
}

void LoadgameMenuMapInit()
{
    return;
}

void StartMapModeInit()
{
    map.scl_mod = 0;
    map.scl_now = 15;
    map.mvx = map.mvy = 0;
    map.line_alp = 0;
    map.flr = map_wrk.floor;
    map.here_bak = 0xff;

    yw2d.map_flg = 0;

    MapExistCHK();

    yw2d.pad_lock = 1;

    ig_menu_wrk.csr[1] = 0;
}

void IngameMenuMap()
{
    IngameMenuMapDisp(spd_mnu.smap);

    if (yw2d.pad_lock == 0)
    {
        if (*key_now[4] == 1)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 1);

            if (spd_mnu.smap == 0)
            {
                yw2d.out_mode_cnt = 0x15;
            }
            else
            {
                yw2d.menu_io_flg = 2;
                yw2d.menu_io_cnt = 20;
            }
        }
        else if (*key_now[0xd] == 1)
        {
            if (spd_mnu.smap != 0)
            {
                SeStartFix(3, 0, 0x1000, 0x1000, 1);

                yw2d.menu_io_flg = 2;
                yw2d.menu_io_cnt = 20;
            }
        }
        else if (*key_now[5] == 1)
        {
            map.scl_mod = 1 - map.scl_mod;

            SeStartFix(1, 0, 0x1000, 0x1000, 1);
        }
        else if (*key_now[10] == 1)
        {
            if (map.visit_flr > 1)
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 1);

                if (map.flr < 3)
                {
                    map.flr++;
                }
                else
                {
                    map.flr = 0;
                }

                while (MapMovableCHK() != 0)
                {
                    if (map.flr < 3)
                    {
                        map.flr++;
                    }
                    else
                    {
                        map.flr = 0;
                    }
                }
            }
        }
        else if (*key_now[8] == 1)
        {
            if (map.visit_flr > 1)
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 1);

                if (map.flr != 0)
                {
                    map.flr--;
                }
                else
                {
                    map.flr = 3;
                }

                while (MapMovableCHK() != 0)
                {
                    if (map.flr != 0)
                    {
                        map.flr--;
                    }
                    else
                    {
                        map.flr = 3;
                    }
                }
            }
        }
    }
}

void IngameMenuMapDisp(u_char mod)
{
    MapCntInit();

    if (mod == 0)
    {
        MapModeInOut();
    }

    if (yw2d.map_flg != 0)
    {
        DspPlyrInMap(yw2d.io_a[5]);
    }

    DspMenuTitle(yw2d.io_x[1], yw2d.io_y[1], yw2d.io_a[1], 0, 1);
    DspLayOut(0, 0, yw2d.io_a[4]);

    if (mod == 0)
    {
        MapModeInOut2();
    }

    MapCntRenew();
}

static void MapCntInit()
{
    return;
}

static void MapCntRenew()
{
    if (yw2d.inn_mode_cnt < 65535)
    {
        yw2d.inn_mode_cnt++;
    }

    if (yw2d.out_mode_cnt != 0)
    {
        yw2d.out_mode_cnt--;
    }
}

static void MapModeInOut()
{
    int i;
    float fade_in;
    float fade_out;

    if (yw2d.inn_mode_cnt < 11)
    {
        fade_in = (yw2d.inn_mode_cnt / 10.0f) * 128.0f;
        fade_out = (1.0f - yw2d.inn_mode_cnt / 10.0f) * 128.0f;

        yw2d.io_a[0] = fade_out;
        yw2d.io_a[1] = fade_in;
        yw2d.io_a[2] = fade_out;
        yw2d.io_a[3] = fade_out;
        yw2d.io_a[4] = fade_in;
        yw2d.io_a[5] = 0.0f;
        yw2d.io_a[6] = 128.0f;

        PlayerStatusYW(yw2d.io_a[0]);

        DspTopMenu(yw2d.io_x[3], yw2d.io_y[3], yw2d.io_a[3], 0);
        DspPlayData(yw2d.io_x[2], yw2d.io_y[2], yw2d.io_a[2], 1);

        if (yw2d.inn_mode_cnt == 10)
        {
            yw2d.map_flg = 1;
        }
    }
    else if (yw2d.inn_mode_cnt < 21)
    {
        fade_in = ((yw2d.inn_mode_cnt - 10) * 128) / 10.0f;

        yw2d.io_a[0] = 0.0f;
        yw2d.io_a[1] = 128.0f;
        yw2d.io_a[2] = 0.0f;
        yw2d.io_a[3] = 0.0f;
        yw2d.io_a[4] = 128.0f;
        yw2d.io_a[5] = fade_in;
        yw2d.io_a[6] = fade_in;

        if (yw2d.inn_mode_cnt == 20)
        {
            yw2d.pad_lock = 0;
        }
    }
    else if (yw2d.out_mode_cnt >= 11)
    {
        fade_out = ((yw2d.out_mode_cnt - 11) * 128) / 10.0f;
        yw2d.pad_lock = 1;
        yw2d.io_a[0] = 0.0f;
        yw2d.io_a[1] = 128.0f;
        yw2d.io_a[2] = 0.0f;
        yw2d.io_a[3] = 0.0f;
        yw2d.io_a[4] = 128.0f;
        yw2d.io_a[5] = fade_out;
        yw2d.io_a[6] = fade_out;

        if (yw2d.out_mode_cnt == 11)
        {
            yw2d.map_flg = 0;
        }
    }
    else if (yw2d.out_mode_cnt != 0)
    {
        fade_in = (1.0f - (yw2d.out_mode_cnt - 1) / 10.0f) * 128.0f;
        fade_out = ((yw2d.out_mode_cnt - 1) * 0x80) / 10.0f;

        yw2d.io_a[0] = fade_in;
        yw2d.io_a[1] = fade_out;
        yw2d.io_a[2] = fade_in;
        yw2d.io_a[3] = fade_in;
        yw2d.io_a[4] = fade_out;
        yw2d.io_a[5] = 0.0f;
        yw2d.io_a[6] = 128.0f;

        PlayerStatusYW(yw2d.io_a[0]);

        DspTopMenu(yw2d.io_x[3], yw2d.io_y[3], yw2d.io_a[3], 0);
        DspPlayData(yw2d.io_x[2], yw2d.io_y[2], yw2d.io_a[2], 1);

        if (yw2d.out_mode_cnt == 1)
        {
            ig_menu_wrk.mode = 7;

            IngameMenuModeSlctDispInit();

            yw2d.menu_io_flg = 0;
            yw2d.pad_lock = 0;
        }
    }
    else
    {
        for (i = 0; i < 7; i++)
        {
            XYAdefaultYW(i);
        }
    }
}

static void MapModeInOut2()
{
    if (yw2d.inn_mode_cnt < 11)
    {
        DspPlayData(yw2d.io_x[2], yw2d.io_y[2], yw2d.io_a[2], 0x2);
    }
    else if (yw2d.out_mode_cnt > 0 && yw2d.out_mode_cnt < 11)
    {
        DspPlayData(yw2d.io_x[2], yw2d.io_y[2], yw2d.io_a[2], 0x2);
    }
}

static void DspPlyrInMap(u_char alp)
{
    MapScoop();
    MapInfo1(alp);
    MapMove(alp);
    MapPrint((map.mvx * map.scl_now) / 10.0f, (map.mvy * map.scl_now) / 10.0f, alp);
    MapInfo2(alp);
}

static void MapScoop()
{
    short int scl_cmp;
    short int scl_obj;

    scl_obj = (map.scl_mod + 1) * 15;
    scl_cmp = map.scl_now - scl_obj;

    if (map.scl_mod == 0)
    {
        if (scl_cmp - 1 > 0)
        {
            map.scl_now--;
        }
        else
        {
            map.scl_now = scl_obj;
        }
    }
    else
    {
        if (scl_cmp + 1 < 0)
        {
            map.scl_now++;
        }
        else
        {
            map.scl_now = scl_obj;
        }
    }
}

static void MapMove(u_char alp)
{
    short int pos_x;
    short int pos_y;
    short int mvx;
    short int mvy;
    u_char anacon;

    if (pad[0].id == 121)
    {
        anacon = pad[0].an_dir[0];
    }
    else
    {
        anacon = 0xff;
    }

    if (yw2d.pad_lock == 0)
    {
        if (*key_now[0] != 0 || *key_now[1] != 0 || *key_now[2] != 0 || *key_now[3] != 0)
        {
            if (*key_now[0] != 0)
            {
                map.mvy++;
            }

            if (*key_now[1] != 0)
            {
                map.mvy--;
            }

            if (*key_now[2] != 0)
            {
                map.mvx++;
            }

            if (*key_now[3] != 0)
            {
                map.mvx--;
            }

            if (map.line_alp + 4 < alp)
            {
                map.line_alp += 4;
            }
            else
            {
                map.line_alp = alp;
            }
        }
        else if (anacon != 0xff)
        {
            switch(anacon)
            {
            case 0:
                map.mvy++;
            break;
            case 2:
                map.mvx--;
            break;
            case 4:
                map.mvy--;
            break;
            case 6:
                map.mvx++;
            break;
            case 1:
                map.mvy++;
                map.mvx--;
            break;
            case 3:
                map.mvy--;
                map.mvx--;
            break;
            case 5:
                map.mvy--;
                map.mvx++;
            break;
            case 7:
                map.mvy++;
                map.mvx++;
            break;
            }

            if (map.line_alp + 4 < alp)
            {
                map.line_alp += 4;
            }
            else
            {
                map.line_alp = alp;
            }
        }
        else
        {
            if (map.line_alp - 4 > 0)
            {
                map.line_alp -= 4;
            }
            else
            {
                map.line_alp = 0;
            }
        }
    }

    if (alp < map.line_alp)
    {
        map.line_alp = alp;
    }

    pos_x = plyr_wrk.move_box.pos[0] / 100.0f;
    pos_y = plyr_wrk.move_box.pos[2] / 100.0f;

    mvx = pos_x - map.mvx;
    mvy = pos_y + map.mvy;

    if (mvy > 500)
    {
        map.mvy--;
    }

    if (mvy < 0)
    {
        map.mvy++;
    }

    if (mvx > 500)
    {
        map.mvx++;
    }

    if (mvx < 0)
    {
        map.mvx--;
    }
}

static void MapInfo1(u_char alp)
{
    float px;
    float py;
    u_char room_no;

    px = plyr_wrk.move_box.pos[0];
    py = plyr_wrk.move_box.pos[2];

    room_no = GetPointRoomNoFloor(plyr_wrk.move_box.pos[0] + 0.0f, plyr_wrk.move_box.pos[2] + 0.0f, map.flr);

    map.plyr_id = GetRoomIdFromRoomNoFloor(0, room_no, map.flr);
    map.here_no = GetPointRoomNoFloor(px - map.mvx * 100, py + map.mvy * 100, map.flr);
    map.here_id = GetRoomIdFromRoomNoFloor(0, map.here_no, map.flr);
}

static void MapInfo2(u_char alp)
{
    PutStringYW(26, map.flr, 325, 37, 0x808080, alp, 0x5000, 1);

    if (map.here_id < 42 && IsRoomPass(map.here_id) != 0 && MapExstFlr(map.here_id, map.flr) != 0)
    {
        PutStringYW(30, map.here_id, 320, 390, 0x808080, alp, 0x5000, 1);
    }
}

static void MapPrint(short int mov_px, short int mov_py, u_char alp)
{
    int i;
    int j;
    int id;
    short int map_ox;
    short int map_oy;
    short int door_mx;
    short int door_my;
    u_char door_stts;
    DOOR_STTS_MAP dsmp;
    float bg_scl;
    u_char num_i;
    u_char num_j;
    short int start_x;
    short int start_y;
    short int x0;
    short int y0;
    short int x1;
    short int y1;

    map_ox = (mov_px + 321) - (short)((map.scl_now / 10.0f) * plyr_wrk.move_box.pos[0] / 100.0f);
    map_oy = (mov_py + 214) - (short)((map.scl_now / 10.0f) * (512.0f - (plyr_wrk.move_box.pos[2] * 0.99f) / 100.0f));

    bg_scl = map.scl_now / 15.0f;

    start_x = map_ox % (short)(bg_scl * 160);
    start_y = map_oy % (short)(bg_scl * 112);

    num_i = (u_char)(640 / (bg_scl * 160)) + 1;
    num_j = (u_char)(448 / (bg_scl * 112)) + 1;

    for (j = -1; j < num_j + 1; j++)
    {
        for (i = -1; i < num_i + 1; i++)
        {
            PutChrForMap(MAP_SEAT_PAPER,
                         start_x + (short)((i * 160) * bg_scl),
                         start_y + (short)((j * 112) * bg_scl),
                         0x808080, alp, bg_scl , 0x64000);
        }
    }

    MapChrCtrl(map_ox, map_oy, alp);

    for (id = 0; id < 42; id++)
    {
        if (MapExstFlr(id, map.flr) == 0)
        {
            continue;
        }

        if (IsRoomPass(id) == 0)
        {
            continue;
        }

        for (i = 0; i < 7; i++)
        {
            if (map_svp_pos[i].room == id && map_svp_pos[i].flr == map.flr)
            {
                x0 = (map.scl_now / 10.0f) * (map_svp_pos[i].px / 100.0f);
                y0 = (map.scl_now / 10.0f) * (512 - map_svp_pos[i].py / 100.0f);

                door_mx = map_ox + x0;
                door_my = map_oy + y0;

                PutIcnForMap2(LIFEBAR_WAKU_3, door_mx, door_my, 0, alp, map.scl_now / 10.0f, 0);
            }
        }
    }

    for (i = 0; i < 77; i++)
    {
        if (GetDoorSttsForMap(&dsmp, map_door_pos[i].id, map_door_pos[i].flr) == 0)
        {
            continue;
        }

        switch(dsmp.stts)
        {
        case 0:
            door_stts = 0xff;
        break;
        case 1:
            door_stts = 0;
        break;
        case 2:
            door_stts = 1;
        break;
        case 3:
            door_stts = 2;
        break;
        case 4:
            door_stts = 3;
        break;
        default:
            door_stts = 0xff;
        break;
        }

        x1 = (map.scl_now / 10.0f) * map_door_pos[i].px;
        y1 = (map.scl_now / 10.0f) * (512 - map_door_pos[i].py);

        door_mx = map_ox + x1;
        door_my = map_oy + y1;

        if (door_stts != 0xff && map_door_pos[i].flr == map.flr)
        {
            if (i >= 5 && i <= 10)
            {
                PutIcnForMap2(door_stts, door_mx, door_my, -map_door_pos[i].rt, alp, (map.scl_now / 10.0f), 1);
            }
            else
            {
                PutIcnForMap2(door_stts, door_mx, door_my, -map_door_pos[i].rt, alp, (map.scl_now / 10.0f), 0);
            }
        }
    }

    if (map.flr == map_wrk.floor)
    {
        MapPlayer(mov_px, mov_py, alp);
    }

    PolySquareYW(0.0f, 0.0f, 0x280, 0x1c2, 0, 128.0f - yw2d.io_a[6], 1.0f, 1.0f, 0x4b000, 0, 0, 0);
    PutSpriteYW(MAP_BACK_MASK1, MAP_BACK_MASK4, 0.0f, 0.0f, 0.0f, 0x808080, 128.0f, 2.6890757f, 2.698795f, 1, 0xff, 1, 0, 0);
}

static void MapPlayer(short int mov_px, short int mov_py, u_char alp)
{
    int i;
    float now_rd;
    short int x[3];
    short int y[3];

    for (i = 0; i < 3; i++)
    {
        now_rd = plyr_wrk.move_box.rot[1] - i * (2 * PI / 3);

        if (PI < now_rd)
        {
            now_rd -= PI * 2;
        }
        if (now_rd < -PI)
        {
            now_rd += PI * 2;
        }
        if (i == 0)
        {
            x[0] = +(map.scl_now / 10.0f) * 7.0f * SgSinf(now_rd) + mov_px + 1.0f;
            y[0] = -(map.scl_now / 10.0f) * 7.0f * SgCosf(now_rd) + mov_py + -10.0f;
        }
        else
        {
            x[i] = +(map.scl_now / 10.0f) * 4.0f * SgSinf(now_rd) + mov_px + 1.0f;
            y[i] = -(map.scl_now / 10.0f) * 4.0f * SgCosf(now_rd) + mov_py + -10.0f;
        }
    }

    SetTriangle(0x55000, x[0], y[0], x[1], y[1], x[2], y[2], 0xff, 0x0b, 0x00, alp);
}

static void MapChrCtrl(short int pos_x, short int pos_y, u_char alp)
{
    int id;
    int rgb;
    int red;

    for (id = 0; id < 42; id++)
    {
        if (MapExstFlr(id, map.flr) == 0)
        {
            continue;
        }

        if (IsRoomPass(id) == 0)
        {
            continue;
        }

        if (map.here_id != id)
        {
            MapChrSet(id, pos_x, pos_y, 0x808080, alp, map.scl_now / 10.0f, 0x5f000);
        }
        else
        {
            MapChrSet(id, pos_x, pos_y, 0x808080, alp, map.scl_now / 10.0f, 0x5f000);

            if (map.here_id != map.here_bak)
            {
                FlashStarYW(flsh, 0, 128, 90, 0xff);
                CLRtoRGB(&rgb, flsh[0].alpha, flsh[0].alpha, flsh[0].alpha);

                map.here_bak = map.here_id;
            }
            else
            {
                FlashStarYW(flsh, 0, 128, 90, 0x0);
                CLRtoRGB(&rgb, flsh[0].alpha, flsh[0].alpha, flsh[0].alpha);
            }

            red = ((224 - (rgb & 0xff)) << 16) + (rgb & 0xffff);

            if (map.here_id != 0xff)
            {
                MapChrSet(map.here_id, pos_x, pos_y, red, alp, map.scl_now / 10.0f, 0x5a000);
            }
        }
    }
}

static void MapChrSet(u_char id, short int pos_x, short int pos_y, int rgb, u_char alp, float scl, int pri)
{
    switch(id)
    {
    case 3:
        if (map.flr == 1)
        {
            PutChrForMap(MAP_ID_03, pos_x, pos_y, rgb, alp, scl, pri);
        }
        else if (map.flr == 2)
        {
            PutChrForMap(MAP_AD_03, pos_x, pos_y, rgb, alp, scl, pri);
        }
    break;
    case 8:
        if (map.flr == 1)
        {
            PutChrForMap(MAP_ID_08, pos_x, pos_y, rgb, alp, scl, pri);
        }
        else if (map.flr == 2)
        {
            PutChrForMap(MAP_AD_08, pos_x, pos_y, rgb, alp, scl, pri);
        }
    break;
    case 9:
        if (map.flr == 1)
        {
            PutChrForMap(MAP_ID_09, pos_x, pos_y, rgb, alp, scl, pri);
        }
        else if (map.flr == 2)
        {
            PutChrForMap(MAP_AD_09, pos_x, pos_y, rgb, alp, scl, pri);
        }
    break;
    case 14:
        if (map.flr == 1)
        {
            PutChrForMap(MAP_ID_14, pos_x, pos_y, rgb, alp, scl, pri);
        }
        else if (map.flr == 2)
        {
            PutChrForMap(MAP_AD_14, pos_x, pos_y, rgb, alp, scl, pri);
        }
    break;
    case 17:
        if (map.flr == 1)
        {
            PutChrForMap(MAP_ID_17, pos_x, pos_y, rgb, alp, scl, pri);
        }
        else if (map.flr == 2)
        {
            PutChrForMap(MAP_AD_17, pos_x, pos_y, rgb, alp, scl, pri);
        }
    break;
    case 20:
        if (map.flr == 2)
        {
            PutChrForMap(MAP_AD_20, pos_x, pos_y, rgb, alp, scl, pri);
        }
        else if (map.flr == 3)
        {
            PutChrForMap(MAP_ID_20, pos_x, pos_y, rgb, alp, scl, pri);
        }
    break;
    case 29:
        if (map.flr == 0)
        {
            PutChrForMap(MAP_AD_29, pos_x, pos_y, rgb, alp, scl, pri);
        }
        else if (map.flr == 1)
        {
            PutChrForMap(MAP_ID_29, pos_x, pos_y, rgb, alp, scl, pri);
        }
    break;
    default:
        PutChrForMap(MAP_ID_00 + id, pos_x, pos_y, rgb, alp, scl, pri);
    break;
    }
}

static void PutChrForMap(u_short chr_lbl, short int pos_x, short int pos_y, int rgb, u_char alp, float scl, int pri)
{
    DISP_SPRT ds;

    CopySprDToSpr(&ds, &spr_dat[chr_lbl]);

    ds.x += pos_x;
    ds.y += pos_y;

    ds.scw = scl;
    ds.sch = scl;

    ds.csx = pos_x;
    ds.csy = pos_y;

    ds.r = (u_int)rgb >> 16;
    ds.g = (u_int)rgb >>  8;
    ds.b = (u_int)rgb >>  0;

    ds.alpha = alp;

    ds.pri = pri; ds.z = 0xfffffff - pri;

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);
}

static void PutIcnForMap(u_short chr_lbl, short int pos_x, short int pos_y, short int rot, u_char alp, float scl)
{
    DISP_SPRT ds;

    CopySprDToSpr(&ds, &spr_dat[chr_lbl]);

    ds.x = pos_x - (ds.w >> 1);
    ds.y = pos_y - (ds.h >> 1);

    ds.crx = pos_x;
    ds.cry = pos_y;

    ds.rot = rot;

    ds.scw = scl / 3.0f;
    ds.sch = scl / 3.0f;

    ds.csx = pos_x;
    ds.csy = pos_y;

    ds.pri = 0x55000;
    ds.z = 0xfffffff - ds.pri;

    ds.alpha = alp;

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);
}

static void PutIcnForMap2(u_char type, short int pos_x, short int pos_y, short int rot, u_char alp, float scl, u_char sz_ptn)
{
    SQAR_DAT base;
    DISP_SQAR ds;

    if (type == 0xff)
    {
        return;
    }

    if (type != 99)
    {
        if (sz_ptn == 0x0)
        {
            base.w = (scl / 3.0f) * 22.0f;
            base.h = (scl / 3.0f) * 8.0f;
        }
        else
        {
            base.w = (scl / 3.0f) * 33.0f;
            base.h = (scl / 3.0f) * 8.0f;
        }

        switch (type)
        {
        case 0:
            base.r = 0xff;
            base.g = 0x60;
            base.b = 0x60;
        break;
        case 1:
            base.r = 0xff;
            base.g = 0xff;
            base.b = 0x60;
        break;
        case 2:
            base.r = 0x91;
            base.g = 0xff;
            base.b = 0x84;
        break;
        case 3:
            base.r = 0x80;
            base.g = 0x80;
            base.b = 0x80;
        break;
        }

        base.x = pos_x - (base.w >> 1);
        base.y = pos_y - (base.h >> 1);

        base.pri = 0x55000;

        base.alpha = alp;

        CopySqrDToSqr(&ds, &base);

        ds.rot = rot;

        ds.crx = pos_x;
        ds.cry = pos_y;

        DispSqrD(&ds);
    }
    else
    {
        base.w = (scl / 3.0f) * 7.0f;
        base.h = (scl / 3.0f) * 7.0f;

        base.x = pos_x - (base.w >> 1);
        base.y = pos_y - (base.w >> 1);

        SetCircle(0x55000, base.x - 320, base.y - 224, (u_char)base.w, 18, 0x2d, 0xdf, 0xdf, alp);
    }
}

static void DspLayOut(short int pos_x, short int pos_y, u_char alpha)
{
    int i;
    LINE_PRT prot[20];
    SPRT_DAT ssd;
    DISP_SPRT ds;

    CmnWindow(0, pos_x, pos_y, alpha, 0x80);
    CmnWindow(2, pos_x, pos_y, alpha, 0x80);
    CmnWindow(1, pos_x, pos_y, alpha, 0x80);

    PutSpriteYW(MAP_FNT_PRESENT, MAP_FNT_PRESENT, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    DrawButtonTex(0x1e000, 3, pos_x + 475, pos_y + 55, alpha);

    PutSpriteYW(MAP_DIRECTION, MAP_DIRECTION, pos_x, pos_y + 30, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
#ifdef BUILD_EU_VERSION
#else
    PutSpriteYW(MAP_FNT_DOT1, MAP_FNT_DOT2, pos_x, pos_y + 30, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
#endif
    PutSpriteYW(MAP_FNT_ZOOM, MAP_FNT_MOOV, pos_x, pos_y + 30, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    DrawButtonTex(0x1e000, 1, pos_x + 475, pos_y + 25, alpha);

#ifdef BUILD_EU_VERSION
#else
    ssd = spr_dat[MAP_FNT_DOT1];

    CopySprDToSpr(&ds, &ssd);

    ds.x = 514.0f;
    ds.y = 35.0f;

    ds.pri = 0xa000;
    ds.z = 0xfffffff - ds.pri;

    ds.alpha = alpha;

    DispSprD(&ds);
#endif

    ssd = spr_dat[MNU_GRB_CAP_EXIT];

    CopySprDToSpr(&ds, &ssd);

    ds.pri = 0xa000;
    ds.z = 0xfffffff - ds.pri;

#ifdef BUILD_EU_VERSION
    ds.x = 524.0f;
#else
    ds.scw = 0.9f;
    ds.sch = 1.0f;

    ds.csx = 548.0f;
    ds.csy = 22.0f;

    ds.x = 548.0f;
#endif
    ds.y = 22.0f;

    ds.alpha = alpha;

    DispSprD(&ds);

    if (map.visit_flr > 1)
    {
        PutSpriteYW(MAP_FNT_L1R1, MAP_FNT_L1R1, pos_x, pos_y + 30, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
#ifdef BUILD_EU_VERSION
#else
        PutSpriteYW(MAP_FNT_DOT3, MAP_FNT_DOT3, pos_x, pos_y + 30, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
#endif
        PutSpriteYW(MAP_FNT_CHANGE, MAP_FNT_CHANGE, pos_x, pos_y + 30, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }

    for (i = 0; i < 20; i++)
    {
        prot[i].rgb = 0xc2a830;
        prot[i].x = 321.0f;
        prot[i].y = (i * 246) / 19.0f + 92.0f;
        prot[i].a = map.line_alp * SgSinf((i * PI) / 19.0f);
    }

    SetLineStrip(0x1000, 0x14, prot, alpha);

    for (i = 0; i < 20; i++)
    {
        prot[i].rgb = 0xc2a830;
        prot[i].x = (i * 306) / 19.0f + 167.0f;
        prot[i].y = 214.0f;
        prot[i].a = map.line_alp * 0.5f * SgSinf((i * PI) / 19.0f);
    }

    SetLineStrip(0x1000, 20, prot, alpha);

    MapTri(pos_x, pos_y, alpha);
}

static void MapTri(short int pos_x, short int pos_y, u_char alpha)
{
    int i;
    int rgb1[4];
    int rgb2[2];
    int flsh1;
    int flsh2;

    if (yw2d.pad_lock == 0)
    {
        FlashStarYW(flsh + 1, 64, 192, 60, 0);
        CLRtoRGB(&flsh1, flsh[1].alpha, flsh[1].alpha, flsh[1].alpha);
    }
    else
    {
        FlashStarYW(&flsh[1], 64, 192, 60, 0xff);
        CLRtoRGB(&flsh1, flsh[1].alpha, flsh[1].alpha, flsh[1].alpha);
    }

    flsh2 = 0x808080;

    for (i = 0; i < 4; i++)
    {
        rgb1[i] = flsh1;
    }

    for (i = 0; i < 2; i++)
    {
        rgb2[i] = flsh2;
    }

    PutSpriteYW(MAP_TRIANGLE1, MAP_TRIANGLE1, pos_x, pos_y, 0.0f, rgb1[0], alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(MAP_TRIANGLE2, MAP_TRIANGLE2, pos_x, pos_y, 0.0f, rgb1[1], alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(MAP_TRIANGLE3, MAP_TRIANGLE3, pos_x, pos_y, -90.0f, rgb1[2], alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(MAP_TRIANGLE4, MAP_TRIANGLE4, pos_x, pos_y, +90.0f, rgb1[3], alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    if (map.visit_flr > 1)
    {
        PutSpriteYW(MAP_CAPT_L1, MAP_CAPT_L1, pos_x, pos_y, 0.0f, rgb2[0], alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(MAP_CAPT_R1, MAP_CAPT_R1, pos_x, pos_y, 0.0f, rgb2[1], alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }
}

static u_char MapExstFlr(u_char id, u_char flr)
{
    u_char rtrn;
    int i;

    rtrn = 0;
    i = 0;

    switch (flr)
    {
    case 0:
        while (rm_in_flr0[i] != 0xff)
        {
            if (rm_in_flr0[i] == id)
            {
                rtrn = 1;

                break;
            }

            i++;
        }
    break;
    case 1:
        while (rm_in_flr1[i] != 0xff)
        {
            if (rm_in_flr1[i] == id)
            {
                rtrn = 1;

                break;
            }

            i++;
        }
    break;
    case 2:
        while (rm_in_flr2[i] != 0xff)
        {
            if (rm_in_flr2[i] == id)
            {
                rtrn = 1;

                break;
            }

            i++;
        }
    break;
    case 3:
        while (rm_in_flr3[i] != 0xff)
        {
            if (rm_in_flr3[i] == id)
            {
                rtrn = 1;

                break;
            }

            i++;
        }
    break;
    }

    return rtrn;
}

static void MapExistCHK()
{
    int i;
    int id;
    u_char room_exist;

    map.movable = 0;
    map.visit_flr = 0;

    for (i = 0; i < 5; i++)
    {
        if (floor_exist[ingame_wrk.msn_no][i] == 0)
        {
            continue;
        }

        room_exist = 0;

        for (id = 0; id < 42; id++)
        {
            if (MapExstFlr(id, i) != 0)
            {
                room_exist += IsRoomPass(id);
            }
        }

        if (room_exist != 0)
        {
            map.movable |= 1 << i;
            map.visit_flr++;
        }
    }
}

static u_char MapMovableCHK()
{
    if (floor_exist[ingame_wrk.msn_no][map.flr] != 0)
    {
        return ((map.movable >> (map.flr)) & 0x1) ^ 1;
    }

    return 1;
}

static void SetLineStrip(int pri, u_char num, LINE_PRT *prot, u_short alp)
{
    int i;
    float div;
    int z;
    LINE_PRT tmp;
    u_int r; // not in STAB
    u_int g; // not in STAB
    u_int b; // not in STAB

    div = g_bInterlace ? 2.0f : 1.0f;

    Reserve2DPacket(pri);

    z = 0xfffffff - pri;

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = (DMAend | 4) + num * 2;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(2, SCE_GS_FALSE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x80);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(num, SCE_GS_TRUE, SCE_GS_TRUE, 66, SCE_GIF_PACKED, 2);
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) \
        | SCE_GS_XYZF2 << (4 * 1);

    for (i = 0; i < num; i++)
    {
        tmp.x = ((prot[i].x - 320) + 2048) * 16;
        tmp.y = ((prot[i].y - 224) / div + 2048) * 16;
        tmp.a = prot[i].a;

        r = (u_int)(prot[i].rgb & 0xff0000) >> 16;
        g = (u_int)(prot[i].rgb & 0x00ff00) >>  8;
        b = (u_int)(prot[i].rgb & 0x0000ff) >>  0;

        pbuf[ndpkt].ui32[0] = r;
        pbuf[ndpkt].ui32[1] = g;
        pbuf[ndpkt].ui32[2] = b;
        pbuf[ndpkt++].ui32[3] = tmp.a;

        pbuf[ndpkt].ui32[0] = tmp.x;
        pbuf[ndpkt].ui32[1] = tmp.y;
        pbuf[ndpkt].ui32[2] = z;
        pbuf[ndpkt++].ui32[3] = 0;
    }
}

static void SetCircle(int pri, short int xo, short int yo, short int r, u_char dtl, u_char cr, u_char cg, u_char cb, u_char a)
{
    float rad;
    float div;
    int i;
    int x[20];
    int y[20];
    int z;
    int mpri;

    div = g_bInterlace ? 2.0f : 1.0f;
    mpri = pri < 1 ? 0x1000 : pri;

    Reserve2DPacket(mpri);

    x[0] = (xo + 2048.0f) * 16;
    y[0] = (yo / div + 2048.0f) * 16;

    for (i = 1; i < dtl + 2; i++)
    {
        rad = (((i - 1) * 2) * PI) / dtl;
        x[i] = ((r * SgSinf(rad) + xo + 2048.0f) * 16);
        y[i] = ((r * SgCosf(rad) + yo) / div + 2048.0f) * 16;
    }

    z = 0xfffffff - mpri;

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = (DMAend | 4) + (dtl + 2) * 2;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(2, SCE_GS_FALSE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, a);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(dtl + 2, SCE_GS_TRUE, SCE_GS_TRUE, 69, SCE_GIF_PACKED, 2);
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) \
        | SCE_GS_XYZF2 << (4 * 1);

    for (i = 0; i < (dtl + 2); i++)
    {
        pbuf[ndpkt].ui32[0] = cr;
        pbuf[ndpkt].ui32[1] = cg;
        pbuf[ndpkt].ui32[2] = cb;
        pbuf[ndpkt++].ui32[3] = 0x80;

        pbuf[ndpkt].ui32[0] = x[i];
        pbuf[ndpkt].ui32[1] = y[i];
        pbuf[ndpkt].ui32[2] = z;
        pbuf[ndpkt++].ui32[3] = 0;
    }
}

void AngleForWin(int pri, short int xo, short int yo, short int r, u_char ptn)
{
    float px;
    float py;
    float rad;
    float div;
    int i;
    int x[20];
    int y[20];
    int z;
    int mpri;
    u_int val;

    div = g_bInterlace ? 2.0f : 1.0f;
    mpri = pri < 1 ? 0x1000 : pri;

    Reserve2DPacket(mpri);

    x[0] = (xo + 2048.0f) * 16;
    y[0] = (yo / div + 2048.0f) * 16;

    val = 18;

    for (i = 0; i < 10; i++)
    {
        rad = (i * PI) / val;
        switch (ptn) {
        case 0:
            px = (xo + r + -1) - r * SgSinf(rad);
            py = (yo + r + -1) - r * SgCosf(rad);
        break;
        case 1:
            px = ((xo - r) + 1) + r * SgCosf(rad);
            py = (yo + r + -1) - r * SgSinf(rad);
        break;
        case 2:
            px = (xo + r + -1) - r * SgCosf(rad);
            py = ((yo - r) + 1) + r * SgSinf(rad);
        break;
        case 3:
            px = ((xo - r) + 1) + r * SgSinf(rad);
            py = ((yo - r) + 1) + r * SgCosf(rad);
        break;
        }

        x[i+1] = ((px + 2048.0f) * 16);
        y[i+1] = ((py / div + 2048.0f) * 16);
    }

    if (i < 10) // ???
    {
        return;
    }

    z = 0xfffffff - mpri;

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = (DMAend | 26);

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(2, SCE_GS_FALSE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, 0x1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(11, SCE_GS_TRUE, SCE_GS_TRUE, 69, SCE_GIF_PACKED, 2);
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) \
        | SCE_GS_XYZF2 << (4 * 1);

    for (i = 0; i < 11; i++)
    {
        pbuf[ndpkt].ui32[0] = 0;
        pbuf[ndpkt].ui32[1] = 0;
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt++].ui32[3] = 1;

        pbuf[ndpkt].ui32[0] = x[i];
        pbuf[ndpkt].ui32[1] = y[i];
        pbuf[ndpkt].ui32[2] = z;
        pbuf[ndpkt++].ui32[3] = 0;
    }
}
