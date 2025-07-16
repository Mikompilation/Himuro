#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "outgame.h"

#include "os/eeiop/cdvd/eecdvd.h"
#include "main/glob.h"
#include "main/gamemain.h"
#include "edit/test_2d.h"
#include "ingame/menu/ig_menu.h"
#include "outgame/title.h"
#include "outgame/outgame.h"
#include "outgame/scn_test.h"
#include "outgame/mode_slct.h"
#include "outgame/btl_mode/btl_menu.h"
#include "graphics/graph2d/message.h"
// #include "graphics/graph2d/g2d_main.h" // gra2dInitST() should be undeclared
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph3d/load3d.h"

static int rsc_menu_csr = 0;
static int rsc_no[2] = {0, 0};

static u_int load_size;

#define LOAD_ADDRESS (u_int *)0x04610000
#define REQ_ADDRESS_1 (u_int *)0x01e90000
#define REQ_ADDRESS_2 (u_int *)0x007f8000

void OutGameCtrl(void)
{
    AnaPonChk();
    switch(outgame_wrk.mode)
    {
        case OUTGAME_MODE_INIT:
            init_load_id = LoadReq(LOGO_PK2, (u_int)REQ_ADDRESS_1);
            OutGameModeChange(OUTGAME_MODE_WAIT);
        break;
        case OUTGAME_MODE_WAIT:
            if (IsLoadEnd(init_load_id) != 0)
            {
                InitTecmotLogo();
                OutGameModeChange(OUTGAME_MODE_LOGO);
            }
        break;
        case OUTGAME_MODE_LOGO:
            if (SetTecmoLogo() != 0)
            {
                OutGameModeChange(OUTGAME_MODE_TITLE_TOP);
            }
        break;
        case OUTGAME_MODE_TITLE_TOP:
            title_wrk.mode = TITLE_INIT;
            OutGameModeChange(OUTGAME_MODE_TITLE);
        case OUTGAME_MODE_TITLE:
            TitleCtrl();
        break;
        case OUTGAME_MODE_BATTLE:
            ModeSlctLoop();
        break;
        case OUTGAME_MODE_OPTION:
            ModeSlctLoop();
        break;
        case OUTGAME_MODE_SCENE_TEST:
          SceneTestCtrl();
        break;
        case OUTGAME_MODE_MOTION_TEST:
            gra2dDraw(0);
        break;
        case OUTGAME_MODE_ROOM_SIZE_CHECK:
            RoomSizeCheckCtrl();
            gra2dDraw(0);
        break;
        case OUTGAME_MODE_LAYOUT_TEST:
            LayoutTestMain();
            gra2dDraw(0);
        break;
        case OUTGAME_MODE_MODESEL:
            ModeSlctLoop();
        break;
  }
  return;
}

void OutGameModeChange(u_char mode)
{
    switch(mode)
    {
        case OUTGAME_MODE_INIT:
            // do nothing ...
        break;
        case OUTGAME_MODE_BATTLE:
            BattleModeInit();
        break;
        case OUTGAME_MODE_OPTION:
            ModeSlctInit(3, 9);
        break;
        case OUTGAME_MODE_SCENE_TEST:
            scn_test_wrk.mode = 0;
        break;
        case OUTGAME_MODE_ROOM_SIZE_CHECK:
            RoomSizeCheckInit();
        break;
        case OUTGAME_MODE_LAYOUT_TEST:
            LayoutTestInit();
        break;
    }

    outgame_wrk.mode = mode;
}

void OrgSetSquare(int pri, float x, float y, float w, float h, u_char r, u_char g, u_char b, u_char a)
{
    SetSquare(pri, x - 320.0f, y - 224.0f, (x - 320.0f) + w, y - 224.0f, x - 320.0f, (y - 224.0f) + h, (x - 320.0f) + w, (y - 224.0f) + h, r, g, b, a);
}

void RoomSizeDisp()
{
    char tmp_str[256] = "";
    const char *room_data_size = "ROOM DATA SIZE";

    OrgSetSquare(2, 300.0f, 78.0f, 288.0f, 300.0f, 0x50, 0x78, 0x78, 0x28);
    sprintf(tmp_str, "ROOM DATA SIZE <%d>", rsc_no[1]);
    SetASCIIString2(1, 312.0f, 86.0f, 1, 0x80, 0x80, 0x80, tmp_str);
    sprintf(tmp_str, "%d byte", load_size);
    SetASCIIString2(1, 380.0f, 110.0f, 1, 0x80, 0x80, 0x80, tmp_str);
    sprintf(tmp_str, "0x%08X byte", load_size);
    SetASCIIString2(1, 380.0f, 130.0f, 1, 0x80, 0x80, 0x80, tmp_str);
}

void RoomSizeCheckCtrl()
{
    char tmp_str[256] = "";
    char *str[6] = {
        "ROOM SIZE CHECK",
        "Mission No  ",
        "Room No     ",
        "Check It !! ",
        "Exit",
        "PRESS X EXIT",
    };
    int max[2] = {5, 39};
    u_int *end_addr;

    if (pad[0].one & 0x40)
    {
        OutGameModeChange(OUTGAME_MODE_TITLE);
        return;
    }

    if (pad[0].rpt & 0x4000)
    {
        rsc_menu_csr++;

        if (rsc_menu_csr > 3)
        {
            rsc_menu_csr = 0;
        }
    }
    else if (pad[0].rpt & 0x1000 && --rsc_menu_csr < 0)
    {
        rsc_menu_csr = 3;
    }

    if (pad[0].one & 0x20)
    {
        if (rsc_menu_csr == 3)
        {
            OutGameModeChange(OUTGAME_MODE_TITLE);
            return;
        }

        if (rsc_menu_csr == 2)
        {
            LoadReq(rsc_no[0] + 10, (u_int)REQ_ADDRESS_2);

            end_addr = RoomMdlLoadReq(LOAD_ADDRESS, 0, rsc_no[0], rsc_no[1], 0);

            while (RoomMdlLoadWait() == 0) {}

            load_size = (u_int)end_addr - (u_int)LOAD_ADDRESS;
        }
    }

    if (rsc_menu_csr < 2)
    {
        if (pad[0].rpt & 0x2000)
        {
            rsc_no[rsc_menu_csr]++;

            if (max[rsc_menu_csr] <= rsc_no[rsc_menu_csr])
            {
                rsc_no[rsc_menu_csr] = 0;
            }
        }

        if (pad[0].rpt & 0x8000)
        {
            rsc_no[rsc_menu_csr]--;

            if (rsc_no[rsc_menu_csr] < 0)
            {
                rsc_no[rsc_menu_csr] = max[rsc_menu_csr] - 1;
            }
        }
    }

    SetASCIIString2(1, 60.0f, 40.0f, 0, 0x20, 0x80, 0x80, str[0]);
    sprintf(tmp_str, "%s%d", str[1], rsc_no[0]);
    SetASCIIString2(1, 100.0f, 80.0f, 1, 0x80, 0x80, 0x80, tmp_str);
    sprintf(tmp_str, "%s%d", str[2], rsc_no[1]);
    SetASCIIString2(1, 100.0f, 100.0f, 1, 0x80, 0x80, 0x80, tmp_str);
    SetASCIIString2(1, 100.0f, 120.0f, 1, 0x80, 0x80, 0x80, str[3]);
    SetASCIIString2(1, 100.0f, 140.0f, 1, 0x80, 0x80, 0x80, str[4]);
    SetASCIIString2(1, 400.0f, 400.0f, 1, 0x80, 0x80, 0x80, str[5]);

    if (load_size != -1)
    {
        RoomSizeDisp();
    }

    OrgSetSquare(2, 80.0f, rsc_menu_csr * 20 + 78, 216.0f, 16.0f, 0x78, 0x50, 0x50, 0x50);
}

void RoomSizeCheckInit(void)
{
    gra2dInitST();

    rsc_menu_csr = 0;
    rsc_no[0] = 0;
    rsc_no[1] = 0;
    load_size = -1;
}
