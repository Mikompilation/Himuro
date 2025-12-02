#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "scn_test.h"

// newlib/libm/math/wf_fmod.c
float fmodf(float x, float y);

#include "sce/sifdev.h"
#include "sce/libvu0.h"

#include "os/eeiop/adpcm/ea_scene.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "outgame/outgame.h"
// #include "os/pad.h" // VibrateRequest, VibrateRequest1, VibrateRequest2
#include "main/glob.h"
#include "ingame/ig_init.h"
#include "graphics/motion/mdlwork.h"
#include "graphics/motion/motion.h"
#include "graphics/mov/movie.h"
#include "graphics/scene/fod.h"
#include "graphics/scene/scene.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_scr.h"
#include "graphics/graph2d/effect_obj.h"
#include "graphics/graph2d/effect_oth.h"
#include "graphics/graph2d/g2d_debug.h"
#include "graphics/graph2d/g2d_main.h" // gra2dDraw
#include "graphics/graph2d/message.h"
// #include "graphics/graph2d/tim2.h" // SetETIM2File
#include "graphics/graph3d/gra3d.h"
#include "graphics/graph3d/libsg.h"
#include "graphics/graph3d/load3d.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/sgsgd.h"

static int mono_flg = 0;
static SCENE_CTRL *scn_p = scene_ctrl;
static FOD_CTRL *scn_fc = &scene_ctrl[0].fod_ctrl;
static FOD_LIGHT *scn_fl = &scene_ctrl[0].fod_ctrl.fod_light;

static char *scene_name[100] = {
    "0010 movie", "0020", "0022", "0030", "0031 movie", "1000 movie",
    "1010", "1030", "1031 movie", "1034", "1070", "1080", "1090",
    "1100", "1101 movie", "1102", "1120", "1150", "1160", "1170", "1180",
    "1220", "1230", "1240 movie", "1250", "1260", "1280",
    "1300 movie", "1301", "1320", "1330", "1331 movie", "1332 movie", "1340",
    "1410",
    "2010 movie", "2013", "2020", "2030", "2040", "2050 movie", "2060", "2061 movie", "2062", "2070", "2071 movie", "2072", "2090", "2091 movie", "2092",
    "2100", "2110 movie", "2130", "2131 movie", "2132", "2140", "2141", "2142 movie", "2143 movie", "2150", "2160", "2170", "2171 movie",
    "3010 movie", "3020", "3030", "3040 movie", "3050", "3060", "3070", "3080 movie", "3081 movie", "3090 movie",
    "3100", "3110",
    "4010 movie", "4011", "4020", "4030", "4031 movie", "4040", "4041 movie", "4042", "4050", "4060 movie", "4070", "4080 movie", "4090 movie",
    "4100 movie", "4110 movie", "4120 movie",
    "5010 movie", "5020 movie",
    "Hashigo Up", "Hashigo Down",
    "9000 movie", "9001 movie",
    "SCENE_NO_MAX",
    NULL,
    NULL,
};
#ifdef BUILD_EU_VERSION
static char *scene_name_staff[5] = {
    "900e movie", "900f movie", "900g movie", "900s movie", "900i movie"
};
#endif
static int scn_test_load_id[20] = {0};
static void (*SceneTestDBMenu[])() = {
    SceneTestMainMenu,
    SceneTestCameraMode,
    SceneTestLightSelect,
    SceneTestAmbient,
    SceneTestFogData,
    SceneTestEffect,
    SceneTestEneEffect,
    SceneTestVibrate,
    SceneTestLightData,
    SceneTestEffectData,
    SceneTestEneEffFire,
    SceneTestEneEffPDeform,
    SceneTestEneEffPDeform,
    SceneTestEneEffOthers,
};

#define PI 3.1415927f
#define DEG2RAD(x) ((float)(x)*PI/180.0f)

#define EFFECT_ADDRESS 0x1e90000
#define SCENE_LOAD_ADDRESS 0x1090000
#define MSN_MAP_OBG_ADDRESS 0x7f8000
#define MODEL_DATA_ADDRESS 0x14b0000
#define SCENE_TEST_MD_LOAD_ADDRESS 0xe00000

void SceneTestCtrl()
{
    if (scn_test_wrk.mode == 0)
    {
        if (SceneTestInit() == 0)
        {
            return;
        }

        change_efbank = 0;
        scn_test_wrk.mode = 1;
    }

    SceneTestMain();
}

int SceneTestInit()
{
    int ret;
    static int load_id;
    static int load_st = 0;

    ret = 0;

    if (load_st == 0)
    {
        InitCamera();
        Init3D();

        gra2dInitST();

        load_id = LoadReq(EFF001_PK2, EFFECT_ADDRESS);

        load_st = 1;
    }
    else if (load_st == 1)
    {
        if (IsLoadEnd(load_id) != 0)
        {
            SetETIM2File(EFFECT_ADDRESS);

            load_st = 2;
        }
    }
    else
    {
        InitEffects();
        InitPerformanceCounter();

        scn_test_wrk = (SCN_TEST_WRK){0};

        item_addr_tbl[1] = pgirlacs[1];

        ret = 1;

        load_st = 0;
    }

    return ret;
}

static int scn_test_load_num = 0;

void SceneTestMain()
{
    if (pad[0].one & 0x100)
    {
        scn_test_wrk.db_flg = scn_test_wrk.db_flg == 0;
        dbg_wrk.oth_perf = scn_test_wrk.db_flg;
    }

    switch (scn_test_wrk.mode)
    {
    case 1:
        SceneTestSelect();
    break;
    case 2:
        SceneTestPlay();
    break;
    case 3:
        SceneTestMenu();
    break;
    }
}

void SceneTestSelect()
{
    char str_title[11] = "SCENE TEST";
    char str_press[21] = "Press START to play.";
    char str_scene_no[256] = "";
    int ret;
    int scene_no;

    ret = ScnLRCtrl(&scn_test_wrk.scene_no, 99, 0);

    if (ret == 1)
    {
        scene_no = scn_test_wrk.scene_no;

        if (SceneDecisionMovie(scene_no) == 0)
        {
            scn_test_wrk = (SCN_TEST_WRK){0};

            SceneTestSetDefEffect();

            scn_test_wrk.mode = 2;
            scn_test_wrk.scene_no = scene_no;
            scn_test_wrk.init = 1;
            scn_test_wrk.load_status = 0;
            scn_test_wrk.db_flg = 0;
            scn_test_wrk.loop_flg = 0;
        }
        else
        {
            MovieTest(scene_no);
        }
    }
    else if (ret == -1)
    {
            if (sys_wrk.game_mode == GAME_MODE_OUTGAME)
            {
                OutGameModeChange(4);
            }

            change_efbank = 1;

            dbg_wrk.oth_perf = 0;
        }

    SetASCIIString(70.0f, 70.0f, str_title);
    SetASCIIString(118.0f, 340.0f, str_press);

#ifdef BUILD_EU_VERSION
    if (scn_test_wrk.scene_no == 96)
    {
        sprintf(str_scene_no,"SCENE No. %s", scene_name_staff[sys_wrk.language]);
    }
    else
    {
        sprintf(str_scene_no,"SCENE No. %s", scene_name[scn_test_wrk.scene_no]);
    }
#else
    sprintf(str_scene_no, "SCENE No. %s", scene_name[scn_test_wrk.scene_no]);
#endif
    SetASCIIString(94.0f, 150.0f, str_scene_no);
}

void SceneTestPlay()
{
    int end_flg;

    if (scn_test_wrk.init == 1)
    {
        if (SceneTestLoad() == 0)
        {
            return;
        }

        AdpcmScenePreLoadEndPlay(scn_test_wrk.scene_no);

        scn_test_wrk.loop_flg = 1;
        scn_test_wrk.init = 0;
    }

    end_flg = SceneTestDraw(1);

    if (scn_test_wrk.mode != 3)
    {
        if (pad[0].one & 0x40)
        {
            scn_test_wrk.init = 1;
            scn_test_wrk.mode = 3;
        }

        if (pad[0].one & 0x800)
        {
            AdpcmSceneStop();
            scn_test_wrk.init = 1;
            scn_test_wrk.mode = 1;
            dbg_wrk.oth_perf = 0;
            return;
        }
    }

    if (end_flg == 1)
    {
        AdpcmSceneStop();

        scn_test_wrk.init = end_flg;

        if (scn_test_wrk.loop_flg == 0)
        {
            scn_test_wrk.mode = 1;
            dbg_wrk.oth_perf = 0;
        }
        else
        {
            SceneTestLoopManage(1);
        }
    }
}

int SceneTestLoad()
{
    static int load_id;
    int ret;

    ret = 0;

    switch(scn_test_wrk.load_status)
    {
    case 0:
        SceneDataLoadReq(scn_test_wrk.scene_no, (u_int *)SCENE_LOAD_ADDRESS);

        scn_test_wrk.load_status = 1;
    break;
    case 1:
        if (SceneDataLoadWait() != 0)
        {
            if (scn_p->chapter_no < 0)
            {
                load_id = LoadReq(MSN03MAP_OBJ, MSN_MAP_OBG_ADDRESS);
            }
            else
            {
                load_id = LoadReq(scn_p->chapter_no + MSN00MAP_OBJ, MSN_MAP_OBG_ADDRESS);
            }

            InitModelLoad();

            scn_test_wrk.load_status = 2;
        }
    break;
    case 2:
        if (IsLoadEnd(load_id) != 0)
        {
            if (scn_p->chapter_no < 0)
            {
                RoomMdlLoadReq((u_int *)MODEL_DATA_ADDRESS, 0, 3, scn_p->room_no, 0);
            }
            else
            {
                RoomMdlLoadReq((u_int *)MODEL_DATA_ADDRESS, 0, scn_p->chapter_no, scn_p->room_no, 0);
            }

            scn_test_wrk.load_status = 3;
        }
    break;
    case 3:
        if (RoomMdlLoadWait() != 0)
        {
            ReqMsnInitPlyr(scn_p->chapter_no);
            scn_test_wrk.load_status = 4;
        }
    break;
    case 4:
        if (MsnInitPlyr() != 0)
        {
            ScnTestMdlLoadReq((u_int *)SCENE_TEST_MD_LOAD_ADDRESS);


            scn_test_wrk.load_status = 5;
        }
    break;
    case 5:
        if (ScnTestMdlLoadWait() != 0)
        {
            SceneCheckModelEntry(scn_p);
            SceneAllMdlInit(scn_p);

            scn_test_wrk.load_status = 6;
        }
    break;
    case 6:
        AdpcmScenePreLoadReq(scn_test_wrk.scene_no);

        scn_test_wrk.load_status = 7;
    break;
    case 7:
        if (IsAdpcmScenePreLoadEnd() != 1 && IsAdpcmScenePreLoadEnd() != -1)
        {
            break;
        }

        scn_test_wrk.load_status = 8;
    case 8:
        if (scn_test_wrk.loop_flg != 0)
        {
            SceneTestLoopManage(0);
        }

        scn_test_wrk.load_status = 0;

        ret = 1;
    }
    return ret;
}

void ScnTestMdlLoadReq(u_int *addr)
{
    SCN_ANM_MDL *sam;
    int i;
    int hero_no;
    int next_addr;

    for (i = 0; i < scn_p->man_mdl_num; i++)
    {
        if (scn_p->chapter_no == 0)
        {
            hero_no = 1;
        }
        else
        {
            hero_no = 0;
        }

        sam = &scn_p->man_mdl[i];

        if (sam->mdl_no != hero_no)
        {
            next_addr = LoadReqGetAddr(sam->mdl_no + M000_MIKU_MDL, (u_int)addr, &scn_test_load_id[scn_test_load_num]);

            pmanmodel[sam->mdl_no] = addr;
            addr = (u_int *)next_addr;

            scn_test_load_num++;
        }
    }

    for (i = 0; i < scn_p->item_num; i++)
    {
        sam = &scn_p->item_mdl[i];

        if (sam->mdl_no != 1) {
            next_addr = LoadReqGetAddr(sam->mdl_no + I000_PLAY_CAMERA1_SGD, (u_int)addr, &scn_test_load_id[scn_test_load_num]);

            item_addr_tbl[sam->mdl_no] = addr;
            addr = (u_int *)next_addr;

            scn_test_load_num++;
        }
    }
}

int ScnTestMdlLoadWait()
{
    SCN_ANM_MDL *sam;
    int i;
    int hero_no;
    int *addr;

    if (scn_test_load_num <= 0)
    {
        return 1;
    }

    if (IsLoadEnd(scn_test_load_id[scn_test_load_num - 1]) == 0)
    {
        return 0;
    }

    for (i = 0; i < scn_p->man_mdl_num; i++)
    {
        if (scn_p->chapter_no == 0)
        {
            hero_no = 1;
        }
        else
        {
            hero_no = 0;
        }

        sam = &scn_p->man_mdl[i];

        if (sam->mdl_no != hero_no)
        {
            addr = (int *)pmanmodel[sam->mdl_no];

            pmanmpk[sam->mdl_no] = (u_int *)GetFileInPak(addr, 0);
            pmanpk2[sam->mdl_no] = (u_int *)GetFileInPak(addr, 1);

            MpkMapUnit(pmanmpk[sam->mdl_no]);
        }
    }

    for (i = 0; i< scn_p->item_num; i++)
    {
        sam = &scn_p->item_mdl[i];

        SgMapUnit(item_addr_tbl[sam->mdl_no]);
    }

    scn_test_load_num = 0;

    return 1;
}

void SceneTestMenu()
{
    int end_flg;

    if (scn_test_wrk.init == 1)
    {
        scn_test_wrk.init = 0;
        scn_test_wrk.main_menu_csr = 0;
        scn_test_wrk.now_menu = 0;
    }

    end_flg = SceneTestDraw(0);

    (*SceneTestDBMenu[scn_test_wrk.now_menu])();

    if (pad[0].one & 0x800)
    {
        dbg_wrk.oth_perf = 0;

        scn_test_wrk.init = 1;
        scn_test_wrk.mode = 1;

        return;
    }

    if (end_flg == 1)
    {
        scn_test_wrk.init = end_flg;

        if (scn_test_wrk.loop_flg == 0)
        {
            dbg_wrk.oth_perf = 0;

            scn_test_wrk.mode = 1;
        }
        else
        {
            scn_test_wrk.mode = 2;

            SceneTestLoopManage(1);
        }
    }
}

void SceneTestMainMenu()
{
    int i;
    char *menu_str[9] = {
        "CAMERA",
        "LIGHT",
        "AMBIENT",
        "FOG",
        "EFFECT",
        "ENE EFFECT",
        "VIBRATE",
        "FILE SAVE",
        "EXIT",
    };

    ScnUDCtrl(&scn_test_wrk.main_menu_csr, 9, 0);

    if (pad[0].one & 0x20)
    {
        switch (scn_test_wrk.main_menu_csr)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            scn_test_wrk.menu_csr = 0;
            scn_test_wrk.now_menu = scn_test_wrk.main_menu_csr + 1;
        break;
        case 7:
            SceneFileSaveBin();
        break;
        case 8:
            scn_test_wrk.mode = 2;
        break;
        }
    }
    else if (pad[0].one & 0x40)
    {
        scn_test_wrk.mode = 2;
    }

    if ((pad[0].now & 0x1) == 0)
    {
        for (i = 0; i < 9; i++)
        {
            SetASCIIString2(1, 48.0f, i * 14 + 48, 1, 0x80, 0x80, 0x80, menu_str[i]);
        }

        SceneSetSquare(2, 48.0f, scn_test_wrk.main_menu_csr * 14 + 48, 120.0f, 12.0f, 0x50, 0x50, 0x64, 0x50);
    }
}

void SceneTestLightSelect()
{
    FOD_LIT_SERIAL *fls;
    int i;
    int ret;
    char *ltype_name[4] = {
        "AMB  ",
        "INF  ",
        "SPOT ",
        "POINT",
    };
    char tmp_str[256] = "";

    ret = ScnUDCtrl(&scn_test_wrk.menu_csr, scn_fl->all_lit_num, 0);

    if (ret == 1)
    {
        scn_test_wrk.now_menu = 8;
        scn_test_wrk.light_no = scn_test_wrk.menu_csr;
        scn_test_wrk.l_menu_csr = 0;
    }

    if (ret == -1)
    {
        scn_test_wrk.now_menu = 0;
        scn_test_wrk.menu_csr = 0;
    }

    for (i = 0; i < scn_fl->all_lit_num; i++)
    {
        fls = &scn_fl->lit_serial[i];

        sprintf(tmp_str, "No.%02d : %s : %s", fls->light_no, ltype_name[fls->light_type], fls->light_name);

        SetASCIIString2(1, 48.0f, i * 14 + 48, 1, 0x80, 0x80, 0x80, tmp_str);
    }

    SceneSetSquare(2, 48.0f, scn_test_wrk.menu_csr * 14 + 48, 156.0f, 12.0f, 0x50, 0x50, 0x64, 0x50);
}

void SceneTestLightData()
{
    FOD_LIT_SERIAL *fls;
    SgLIGHT *light;
    sceVu0FVECTOR color;
    float cone_deg;
    int i;
    int yofs;
    int menu_num;
    int csr_pos;
    char *ltype_name[4] = {
        "AMB  ",
        "INF  ",
        "SPOT ",
        "POINT",
    };
    char *col_str[3] = { "R", "G", "B" };
    char tmp_str[256] = "";
    int ret;
    float cc;

    light = &scn_fl->all_lit[scn_test_wrk.light_no];
    fls = &scn_fl->lit_serial[scn_test_wrk.light_no];

    if (fls->light_type == 2)
    {
        menu_num = 5;
        cone_deg = light->ambient[1];
    }
    else if (fls->light_type == 3)
    {
        menu_num = 4;
    }
    else
    {
        menu_num = 3;
    }

    Vu0CopyVector(color, light->diffuse);

    ret = ScnUDCtrl(&scn_test_wrk.l_menu_csr, menu_num, 0);

    if (ret == -1)
    {
        scn_test_wrk.now_menu = 2;
        scn_test_wrk.l_menu_csr = 0;
    }

    if (scn_test_wrk.l_menu_csr < 3)
    {
        ScnValueCtrl(&color[scn_test_wrk.l_menu_csr], 100.0f, 0.0f, 0.01f, 20.0f);

    }
    else if (scn_test_wrk.l_menu_csr == 3)
    {
        ScnValueCtrl(&light->power, 10000.0f, 0.0f, 1.0f, 10.0f);
    }
    else if (ScnValueCtrl(&cone_deg, 90.0f, 1.0f, 0.1f, 10.0f) == 1)
    {
        cc = SgCosf(DEG2RAD(cone_deg));

        light->intens = cc * cc;
        light->ambient[1] = cone_deg;
    }

    sprintf(tmp_str, "No.%02d : %s : %s", fls->light_no, ltype_name[fls->light_type], fls->light_name);

    SetASCIIString2(1, 48.0f, 48.0f, 1, 0x80, 0x80, 0x80, tmp_str);

    yofs = 28;

    for (i = 0; i < 3; i++)
    {
        sprintf(tmp_str, "  %s : %.3f", col_str[i], color[i]);
        SetASCIIString2(1, 48.0f, yofs + 48, 1, 0x80, 0x80, 0x80, tmp_str);
        yofs += 14;
    }

    yofs += 14;

    if (fls->light_type == 2 || fls->light_type == 3)
    {
        sprintf(tmp_str, "POWER = %.2f", light->power);
        SetASCIIString2(1, 48.0f, yofs + 0x30, 1, 0x80, 0x80, 0x80, tmp_str);

        csr_pos = yofs + 48;
        yofs += 28;

        if (fls->light_type == 2)
        {
            sprintf(tmp_str, "CONE = %.1f", cone_deg);
            SetASCIIString2(1, 48.0f, yofs + 48, 1, 0x80, 0x80, 0x80, tmp_str);

            if (scn_test_wrk.l_menu_csr == 4)
            {
                csr_pos = yofs + 48;
            }

            yofs += 28;

            ScnDispPrintVector("POS ", light->pos, 48.0f, yofs + 48);
            ScnDispPrintVector("INT ", light->direction, 48.0f, yofs + 76);
        }
        else
        {
            ScnDispPrintVector("POS ", light->pos, 48.0f, yofs + 48);
        }

        if (scn_test_wrk.l_menu_csr <= 2)
        {
            csr_pos = scn_test_wrk.l_menu_csr * 14 + 76;
        }
    }
    else
    {
        csr_pos = scn_test_wrk.l_menu_csr * 14 + 76;
    }

    SceneSetSquare(2, 36.0f, csr_pos, 180.0f, 12.0f, 0x50, 0x50, 0x64, 0x50);

    Vu0CopyVector(light->diffuse, color);
}

void ScnDispPrintVector(char *ttl, sceVu0FVECTOR v, float x, float y)
{
    char tmp_str[256];

    sprintf(tmp_str, "%s = %f : %f : %f", ttl, v[0], v[1], v[2]);
    SetASCIIString2(1, x, y, 1, 0x80, 0x80, 0x80, tmp_str);
}

void SceneTestAmbient()
{
    SCN_TEST_WRK *stw = &scn_test_wrk;
    sceVu0FVECTOR *amb_p;
    char *amb_str[6] = {
        "COMMON",
        "CHARA ",
        "ROOM  ",
        "FURN  ",
        "DOOR  ",
        "ITEM  ",
    };
    char *col_str[3] = { "R", "G", "B" };
    char tmp_str[256] = "";
    int i;
    int j;
    int yofs;
    int csr_pos;
    int ret;

    ret = ScnUDCtrl(&stw->menu_csr, 18, 0);

    if (ret == -1)
    {
        stw->now_menu = 0;
        stw->menu_csr = 0;
    }
    else
    {
        amb_p = &scn_fl->amb[stw->menu_csr / 3];

        ScnValueCtrl(&amb_p[0][stw->menu_csr % 3], 10.0f, 0.0f, 0.001f, 20.0f);

        amb_p = scn_fl->amb;

        SetASCIIString2(1, 48.0f, 48.0f, 1, 0x80, 0x80, 0x80, "AMBIENT");

        yofs = 28;

        for (i = 0; i < 6; i++)
        {
            sprintf(tmp_str, " %s %s : %.3f", amb_str[i], col_str[0], amb_p[0][0]);
            SetASCIIString2(1, 48.0f, yofs + 48, 1, 0x80, 0x80, 0x80, tmp_str);

            yofs += 14;

            for (j = 0; j < 2; j++)
            {
                sprintf(tmp_str, "        %s : %.3f", col_str[j+1], amb_p[0][j+1]);
                SetASCIIString2(1, 48.0f, yofs + 48, 1, 0x80, 0x80, 0x80, tmp_str);

                yofs += 14;
            }

            yofs += 14;
            amb_p++;
        }

        csr_pos = (stw->menu_csr % 3) * 14 + 76 + (int)(stw->menu_csr / 3) * 56;
        SceneSetSquare(2, 48.0f, csr_pos, 240.0f, 12.0f, 0x50, 0x50, 0x64, 0x50);
    }
}

void RotVectorY(sceVu0FVECTOR normal, sceVu0FVECTOR vector, float delta)
{
    sceVu0FMATRIX m0;

    delta = fmodf(delta, PI * 2);

    if (delta >= PI)
    {
        delta = -PI + fmodf(delta, PI);
    }
    else if (delta <= -PI)
    {
        delta = PI + fmodf(delta, PI);
    }

    sceVu0UnitMatrix(m0);
    sceVu0RotMatrixY(m0, m0, delta);
    sceVu0ApplyMatrix(normal, m0, vector);
}

void SceneTestPadCamera()
{
    SgCAMERA *cam;
    sceVu0FMATRIX mat;
    sceVu0FVECTOR vec;
    sceVu0FVECTOR dir;
    sceVu0FVECTOR xz_dir;
    float dist;
    float accel;

    cam = scn_p->cam;

    accel = 1.0f;

    sceVu0SubVector(dir, cam->i, cam->p);

    dist = SgSqrtf(dir[0] * dir[0] + dir[2] * dir[2]);

    dir[1] = 0.0f;
    dir[3] = 1.0f; // accel ?

    sceVu0Normalize(dir, dir);
    sceVu0CopyVector(xz_dir, dir);

    xz_dir[1] = 0.0f;

    if ((pad[0].now & 2U) == 0)
    {
        sceVu0ScaleVectorXYZ(xz_dir, xz_dir, 10.0f);

        if (pad[0].now & 0x1000)
        {
            sceVu0AddVector(cam->i, cam->i, xz_dir);
            sceVu0AddVector(cam->p, cam->p, xz_dir);
        }

        if (pad[0].now & 0x4000)
        {
            sceVu0SubVector(cam->i, cam->i, xz_dir);
            sceVu0SubVector(cam->p, cam->p, xz_dir);
        }

        if (pad[0].now & 0x8000)
        {
            RotVectorY(xz_dir, xz_dir, PI / 2);

            sceVu0SubVector(cam->i, cam->i, xz_dir);
            sceVu0SubVector(cam->p, cam->p, xz_dir);
        }

        if (pad[0].now & 0x2000)
        {
            RotVectorY(xz_dir, xz_dir, PI / 2);

            sceVu0AddVector(cam->i, cam->i, xz_dir);
            sceVu0AddVector(cam->p, cam->p, xz_dir);
        }

        if (pad[0].now & 0x1)
        {
            cam->p[1] -= accel * 1.5f;
            cam->i[1] -= accel * 1.5f;
        }

        if (pad[0].now & 0x4)
        {
            cam->p[1] += accel * 1.5f;
            cam->i[1] += accel * 1.5f;
        }
    }
    else
    {
        if (pad[0].now & 0x2000)
        {
            RotVectorY(vec, xz_dir, 0.03f);
            cam->i[0] = cam->p[0] + vec[0] * dist;
            cam->i[2] = cam->p[2] + vec[2] * dist;
        }
        else if (pad[0].now & 0x8000)
        {
            RotVectorY(vec, xz_dir, -0.03f);
            cam->i[0] = cam->p[0] + vec[0] * dist;
            cam->i[2] = cam->p[2] + vec[2] * dist;
        }

        if (pad[0].now & 0x4000)
        {
            cam->i[1] -= (dist + dist) / 60.0f;
        }

        if (pad[0].now & 0x1000)
        {
            cam->i[1] += (dist + dist) / 60.0f;
        }
    }
}

void SceneTestCameraMode()
{
    sceVu0FVECTOR pos[2];
    int i;
    char *menu_str[2] = { "POSITION", "INTEREST" };
    char tmp_str[256];

    ScnUDCtrl(&scn_test_wrk.menu_csr, 2, 0);

    if (pad[0].one & 0x40)
    {
        scn_test_wrk.now_menu = 0;
        scn_test_wrk.menu_csr = 0;
    }
    else
    {
        SceneTestPadCamera();

        Vu0CopyVector(pos[0], scn_p->cam->p);
        Vu0CopyVector(pos[1], scn_p->cam->i);

        for (i = 0; i < 2; i++)
        {
            ScnDispPrintVector(menu_str[i], pos[i], 32.0f, i * 24 + 64);
        }
    }
}

void SceneTestFogData()
{
    int i;
    char *menu_str[7] = {
        "R   ",
        "G   ",
        "B   ",
        "NEAR",
        "FAR ",
        "MIN ",
        "MAX ",
    };
    char tmp_str[256];
    int fog_col[3] = { scn_p->fog.r, scn_p->fog.g, scn_p->fog.b };
    float fog_data[7] = {
        scn_p->fog.r,
        scn_p->fog.g,
        scn_p->fog.b,
        scn_p->fog.near,
        scn_p->fog.far,
        scn_p->fog.min,
        scn_p->fog.max,
    };
    int ret;

    ret = ScnUDCtrl(&scn_test_wrk.menu_csr, 7, 0);

    if (ret == -1)
    {
        scn_test_wrk.now_menu = 0;
        scn_test_wrk.menu_csr = 0;
    }

    if (scn_test_wrk.menu_csr < 3)
    {
        ScnValueCtrl(&fog_data[scn_test_wrk.menu_csr], 255.0f, 0.0f, 1.0f, 16.0f);
    }
    else if (scn_test_wrk.menu_csr < 5)
    {
        ScnValueCtrl(&fog_data[scn_test_wrk.menu_csr], 10000.0f, -10000.0f, 10.0f, 10.0f);
    }
    else
    {
        ScnValueCtrl(&fog_data[scn_test_wrk.menu_csr], 255.0f, 0.0f, 1.0f, 16.0f);
    }

    SetASCIIString2(1, 32.0f, 40.0f, 1, 0x80, 0x80, 0x80, "FOG DATA");

    for (i = 0; i < 7; i++)
    {
        if (i < 3)
        {
            sprintf(tmp_str, "%s : %d", menu_str[i], (int)fog_data[i]);
        }
        else if (i < 5)
        {
            sprintf(tmp_str, "%s : %.01f", menu_str[i], fog_data[i]);
        }
        else
        {
            sprintf(tmp_str, "%s : %.00f", menu_str[i], fog_data[i]);
        }

        SetASCIIString2(1, 40.0f, i * 14 + 64, 1, 0x80, 0x80, 0x80, tmp_str);
    }

    SceneSetSquare(2, 36.0f, scn_test_wrk.menu_csr * 14 + 64, 192.0f, 12.0f, 0x50, 0x50, 0x64, 0x50);

    scn_p->fog.r = fog_data[0];
    scn_p->fog.g = fog_data[1];
    scn_p->fog.b = fog_data[2];
    scn_p->fog.near = fog_data[3];
    scn_p->fog.far = fog_data[4];
    scn_p->fog.min = fog_data[5];
    scn_p->fog.max = fog_data[6];
}

static SCN_EFF_CTRL scn_eff_ctrl[] = {
    { .eff_data = {0}, .eff_flg = 0, .eff_no =  0, .param_num = 0, .eff_name = "Z DEPTH"     },
    { .eff_data = {0}, .eff_flg = 0, .eff_no =  1, .param_num = 0, .eff_name = "MONO"        },
    { .eff_data = {0}, .eff_flg = 0, .eff_no =  2, .param_num = 0, .eff_name = "SEPIA"       },
    { .eff_data = {0}, .eff_flg = 0, .eff_no =  3, .param_num = 5, .eff_name = "DITHER"      },
    { .eff_data = {0}, .eff_flg = 0, .eff_no =  4, .param_num = 3, .eff_name = "BLUR NORMAL" },
    { .eff_data = {0}, .eff_flg = 0, .eff_no =  5, .param_num = 3, .eff_name = "BLUR BLACK"  },
    { .eff_data = {0}, .eff_flg = 0, .eff_no =  6, .param_num = 3, .eff_name = "BLUR WHITE"  },
    { .eff_data = {0}, .eff_flg = 0, .eff_no =  7, .param_num = 2, .eff_name = "DEFORM"      },
    { .eff_data = {0}, .eff_flg = 0, .eff_no =  8, .param_num = 1, .eff_name = "FOCUS"       },
    { .eff_data = {0}, .eff_flg = 0, .eff_no =  9, .param_num = 2, .eff_name = "CONTRAST1"   },
    { .eff_data = {0}, .eff_flg = 0, .eff_no = 10, .param_num = 2, .eff_name = "CONTRAST2"   },
    { .eff_data = {0}, .eff_flg = 0, .eff_no = 11, .param_num = 2, .eff_name = "CONTRAST3"   },
    { .eff_data = {0}, .eff_flg = 0, .eff_no = 12, .param_num = 1, .eff_name = "FADE FRAME"  },
    { .eff_data = {0}, .eff_flg = 0, .eff_no = 13, .param_num = 1, .eff_name = "CROSS FADE"  },
    { .eff_data = {0}, .eff_flg = 0, .eff_no = 14, .param_num = 2, .eff_name = "FADE SCREEN" },
    { .eff_data = {0}, .eff_flg = 1, .eff_no = 15, .param_num = 0, .eff_name = "SHIBATA SET" },
};
SCN_TEST_WRK scn_test_wrk = {0};

void SceneTestEffect()
{
    SCN_TEST_WRK *stw = &scn_test_wrk;
    SCN_EFF_CTRL *sec;
    int i;
    int ret;
    int yofs;
    char tmp_str[256] = "";
    char *on_off[2] = { "OFF", "ON" };

    ret = ScnUDCtrl(&stw->menu_csr, 0x10, 0);

    sec = &scn_eff_ctrl[stw->menu_csr];

    if (ret == 1 && stw->menu_csr > 2)
    {
        stw->now_menu = 9;
        stw->effect_csr = 0;
        stw->effect_id = stw->menu_csr;
    }

    if (ret == -1)
    {
        stw->now_menu = 0; stw->menu_csr = 0;
    }
    else
    {
        ScnValueCtrlI((int *)&sec->eff_flg, 1, 0, 1, 1);

        SetASCIIString2(1, 48.0f, 48.0f, 1, 0x80, 0x80, 0x80, "EFFECT");

        yofs = 0x1c;

        for (i = 0; i < 16; i++)
        {
            sec = &scn_eff_ctrl[i];

            sprintf(tmp_str, "  %s", sec->eff_name);
            SetASCIIString2(1, 48.0f, yofs + 48, 1, 0x80, 0x80, 0x80, tmp_str);

            sprintf(tmp_str, ":  %s", on_off[sec->eff_flg]);
            SetASCIIString2(1, 216.0f, yofs + 48, 1, 0x80, 0x80, 0x80, tmp_str);

            yofs += 14;
        }

        SceneSetSquare(2, 48.0f, (float)(stw->menu_csr * 14 + 76), 252.0f, 12.0f, 0x50, 0x50, 0x64, 0x50);
    }
}

void SceneTestEffectData()
{
    SCN_TEST_WRK *stw = &scn_test_wrk;
    SCN_EFF_CTRL *sec = &scn_eff_ctrl[stw->effect_id];
    char tmp_str[256] = "";
    int ret;

    ret = ScnUDCtrl(&stw->effect_csr, sec->param_num, 0);

    if (ret == -1)
    {
        stw->now_menu = 5;
        stw->effect_id = 0;
        stw->effect_csr = 0;
        return;
    }

    SetASCIIString2(1, 48.0f, 48.0f, 1, 0x80, 0x80, 0x80, sec->eff_name);

    switch(stw->effect_id - 1)
    {
    case EF_DITHER:
        if (stw->effect_csr == 0)
        {
            ScnValueCtrlC(&sec->eff_data.dither.type, 0x7, 0x1, 1, 1);
        }
        else if (stw->effect_csr == 1)
        {
            ScnValueCtrl(&sec->eff_data.dither.speed, 90.0f, 0.0f, 0.1f, 10.0f);
        }
        else if (stw->effect_csr == 2)
        {
            ScnValueCtrl(&sec->eff_data.dither.alpha, 127.0f, 0.0f, 0.1f, 10.0f);
        }
        else if (stw->effect_csr == 3)
        {
            ScnValueCtrlC(&sec->eff_data.dither.alpmax, 0x80, 0x0, 1, 8);
        }
        else if (stw->effect_csr == 4)
        {
            ScnValueCtrlC(&sec->eff_data.dither.colmax, 0x80, 0x0, 1, 8);
        }

        sprintf(tmp_str, "TYPE        %d", sec->eff_data.dither.type);
        SetASCIIString2(1, 48.0f, 76.0f, 1, 0x80, 0x80, 0x80, tmp_str);

        sprintf(tmp_str, "SPEED       %.1f", sec->eff_data.dither.speed);
        SetASCIIString2(1, 48.0f, 90.0f, 1, 0x80, 0x80, 0x80, tmp_str);

        sprintf(tmp_str, "ALPHA       %.1f", sec->eff_data.dither.alpha);
        SetASCIIString2(1, 48.0f, 104.0f, 1, 0x80, 0x80, 0x80, tmp_str);

        sprintf(tmp_str, "ALPHA MAX   %d", sec->eff_data.dither.alpmax);
        SetASCIIString2(1, 48.0f, 118.0f, 1, 0x80, 0x80, 0x80, tmp_str);

        sprintf(tmp_str, "COLOR MAX   %d", sec->eff_data.dither.colmax);
        SetASCIIString2(1, 48.0f, 132.0f, 1, 0x80, 0x80, 0x80, tmp_str);
    break;
    case EF_BLUR_N:
    case EF_BLUR_B:
    case EF_BLUR_W:
        if (stw->effect_csr == 0)
        {
            ScnValueCtrlC(&sec->eff_data.blur.alpha, 0x7f, 0x0, 1, 8);
        }
        else if (stw->effect_csr == 1)
        {
            ScnValueCtrlI((int *)&sec->eff_data.blur.scale, 0x44c, 900, 1, 10);
        }
        else if (stw->effect_csr == 2)
        {
            ScnValueCtrlI((int *)&sec->eff_data.blur.rot, 0xe10, 0, 10, 10);
        }

        sprintf(tmp_str, "ALPHA       %d", sec->eff_data.blur.alpha);
        SetASCIIString2(1, 48.0f, 76.0f, 1, 0x80, 0x80, 0x80, tmp_str);

        sprintf(tmp_str, "SCALE       %d", sec->eff_data.blur.scale);
        SetASCIIString2(1, 48.0f, 90.0f, 1, 0x80, 0x80, 0x80, tmp_str);

        sprintf(tmp_str, "ROT         %d", sec->eff_data.blur.rot);
        SetASCIIString2(1, 48.0f, 104.0f, 1, 0x80, 0x80, 0x80, tmp_str);
    break;
    case EF_DEFORM:
        if (stw->effect_csr == 0)
        {
            ScnValueCtrlC(&sec->eff_data.deform.type, 0x7, 0x1, 1, 1);
        }
        else if (stw->effect_csr == 1)
        {
            ScnValueCtrlC(&sec->eff_data.deform.volume, 0x80, 0x0, 1, 8);
        }

        sprintf(tmp_str, "TYPE        %d", sec->eff_data.deform.type);
        SetASCIIString2(1, 48.0f, 76.0f, 1, 0x80, 0x80, 0x80, tmp_str);

        sprintf(tmp_str, "VOLUME      %d", sec->eff_data.deform.volume);
        SetASCIIString2(1, 48.0f, 90.0f, 1, 0x80, 0x80, 0x80, tmp_str);
    break;
    case EF_FOCUS:
        if (stw->effect_csr == 0)
        {
            ScnValueCtrlC(&sec->eff_data.focus.volume, 0x80, 0x0, 1, 8);
        }

        sprintf(tmp_str, "VOLUME      %d", sec->eff_data.focus.volume);
        SetASCIIString2(1, 48.0f, 76.0f, 1, 0x80, 0x80, 0x80, tmp_str);
    break;
    case EF_OVERLAP:
        if (stw->effect_csr == 0)
        {
            ScnValueCtrlC(&sec->eff_data.contrast.color, 0xff, 0x0, 1, 8);
        }
        else if (stw->effect_csr == 1)
        {
            ScnValueCtrlC(&sec->eff_data.contrast.alpha, 0xff, 0x0, 1, 8);
        }

        sprintf(tmp_str, "COLOR       %d", sec->eff_data.contrast.color);
        SetASCIIString2(1, 48.0f, 76.0f, 1, 0x80, 0x80, 0x80, tmp_str);

        sprintf(tmp_str, "ALPHA       %d", sec->eff_data.contrast.alpha);
        SetASCIIString2(1, 48.0f, 90.0f, 1, 0x80, 0x80, 0x80, tmp_str);
    break;
    case EF_FADEFRAME:
        if (stw->effect_csr == 0)
        {
            ScnValueCtrlC(&sec->eff_data.contrast.color, 0xff, 0x0, 1, 8);
        }
        else if (stw->effect_csr == 1)
        {
            ScnValueCtrlC(&sec->eff_data.contrast.alpha, 0xff, 0x0, 1, 8);
        }

        sprintf(tmp_str, "COLOR       %d", sec->eff_data.contrast.color);
        SetASCIIString2(1, 48.0f, 76.0f, 1, 0x80, 0x80, 0x80, tmp_str);

        sprintf(tmp_str, "ALPHA       %d", sec->eff_data.contrast.alpha);
        SetASCIIString2(1, 48.0f, 90.0f, 1, 0x80, 0x80, 0x80, tmp_str);
    break;
    case EF_RENZFLARE:
        if (stw->effect_csr == 0)
        {
            ScnValueCtrlC(&sec->eff_data.contrast.color, 0xff, 0x0, 1, 8);
        }
        else if (stw->effect_csr == 1)
        {
            ScnValueCtrlC(&sec->eff_data.contrast.alpha, 0xff, 0x0, 1, 8);
        }

        sprintf(tmp_str, "COLOR       %d", sec->eff_data.contrast.color);
        SetASCIIString2(1, 48.0f, 76.0f, 1, 0x80, 0x80, 0x80, tmp_str);

        sprintf(tmp_str, "ALPHA       %d", sec->eff_data.contrast.alpha);
        SetASCIIString2(1, 48.0f, 90.0f, 1, 0x80, 0x80, 0x80, tmp_str);
    break;
    case EF_BLACKFILTER:
    case EF_NEGA:
        if (stw->effect_csr == 0)
        {
            ScnValueCtrlC(&sec->eff_data.focus.volume, 0xff, 0x0, 1, 8);
        }

        sprintf(tmp_str, "VOLUME      %d", sec->eff_data.focus.volume);
        SetASCIIString2(1, 48.0f, 76.0f, 1, 0x80, 0x80, 0x80, tmp_str);
    break;
    case EF_NCONTRAST:
        if (stw->effect_csr == 0)
        {
            ScnValueCtrlC(&sec->eff_data.fade_scr.r, 0xff, 0x0, 1, 8);
            sec->eff_data.fade_scr.g = sec->eff_data.fade_scr.b = sec->eff_data.fade_scr.r;
        }
        else if (stw->effect_csr == 1)
        {
            ScnValueCtrlC(&sec->eff_data.fade_scr.a, 0x80, 0x0, 1, 8);
        }

        sprintf(tmp_str, "COLOR      %d", sec->eff_data.fade_scr.r);
        SetASCIIString2(1, 48.0f, 76.0f, 1, 0x80, 0x80, 0x80, tmp_str);

        sprintf(tmp_str, "ALPHA      %d", sec->eff_data.fade_scr.a);
        SetASCIIString2(1, 48.0f, 90.0f, 1, 0x80, 0x80, 0x80, tmp_str);
    break;
    }

    SceneSetSquare(2, 48.0f, stw->effect_csr * 14 + 76, 252.0f, 12.0f, 0x50, 0x50, 0x64, 0x50);
}

void SceneTestEneEffect()
{
    SCN_TEST_WRK *stw = &scn_test_wrk;
    int i;
    int ret;
    int yofs;
    int non_flg;
    char *eff_name[4] = {
        "ENE FIRE(AURA)",
        "PARTS DEFORM1",
        "PARTS DEFORM2",
        "OTHERS"
    };
    char tmp_str[256] = "";
    int no;

    non_flg = 0;

    for (i = 0; i < 4; i++)
    {
        if (scn_p->man_mdl[i].ene_efct != NULL)
        {
            break;
        }
    }

    no = i;

    if (no >= 4)
    {
        SetASCIIString2(1, 48.0f, 48.0f, 1, 0x80, 0x80, 0x80, "ENE EFFECT   NONE");
        non_flg = 1;
    }
    else
    {
        sprintf(tmp_str, "ENE EFFECT   %s", scn_p->man_mdl[no].prefix);
        SetASCIIString2(1, 48.0f, 48.0f, 1, 0x80, 0x80, 0x80, tmp_str);
    }

    yofs = 28;

    ret = ScnUDCtrl(&stw->menu_csr, 4, 0);

    if (ret == -1)
    {
        stw->now_menu = 0;
        stw->menu_csr = 0;

        return;
    }

    if (non_flg != 1)
    {
        if (ret == 1)
        {
            stw->now_menu = stw->menu_csr + 10;
            stw->effect_id = stw->menu_csr;
            stw->effect_csr = 0;
            stw->ene_mdl_no = no;
        }

        for (i = 0; i < 4; i++)
        {
            sprintf(tmp_str, "  %s", eff_name[i]);
            SetASCIIString2(1, 48.0f, yofs + 48, 1, 0x80, 0x80, 0x80, tmp_str);

            yofs += 14;
        }

        SceneSetSquare(2, 48.0f, stw->menu_csr * 14 + 76, 216.0f, 12.0f, 0x50, 0x50, 0x64, 0x50);
    }
}

void SceneTestEneEffFire()
{
    SCN_TEST_WRK *stw = &scn_test_wrk;
    SCN_ENE_EFCT *see;
    int i;
    int ret;
    int yofs;
    char tmp_str[256] = "";
    char *rgba_str[4] = {
        "R     ",
        "G     ",
        "B     ",
        "ALPHA ",
    };
    float size;
    float rate;
    u_char rgba[4];

    see = scn_p->man_mdl[stw->ene_mdl_no].ene_efct;

    size = see->aura_size;
    rate = see->aura_rate;

    rgba[0] = (see->aura_rgba >> 24) & 0xff;
    rgba[1] = (see->aura_rgba >> 16) & 0xff;
    rgba[2] = (see->aura_rgba >>  8) & 0xff;
    rgba[3] = (see->aura_rgba >>  0) & 0xff;

    ret = ScnUDCtrl(&stw->effect_csr, 7, 0);

    if (ret == -1)
    {
        stw->now_menu = 6;
        stw->effect_id = 0;

        return;
    }

    switch(stw->effect_csr)
    {
    case 0:
        ScnValueCtrl(&size, 20000.0f, 1.0f, 10.0f, 10.0f);
    break;
    case 1:
        ScnValueCtrl(&rate, 2.0f, 0.0f, 0.001f, 10.0f);
    break;
    case 2:
    case 3:
    case 4:
        ScnValueCtrlC(&rgba[stw->effect_csr - 2], 0xff, 0x0, 1, 16);
    break;
    case 5:
        ScnValueCtrlC(&rgba[3], 0x80, 0x0, 1, 16);
    break;
    case 6:
        if (ret == 1)
        {
            SceneReleaseEffect(scn_p);
        }
    break;
    }

    yofs = 56;

    sprintf(tmp_str, "ENE EFFECT  %s  FIRE", scn_p->man_mdl[stw->ene_mdl_no].prefix);
    SetASCIIString2(1, 48.0f, 48.0f, 1, 0x80, 0x80, 0x80, tmp_str);

    sprintf(tmp_str, "  SIZE    : %.1f", size);
    SetASCIIString2(1, 48.0f, 76.0f, 1, 0x80, 0x80, 0x80, tmp_str);

    sprintf(tmp_str, "  RATE    : %.6f", rate);
    SetASCIIString2(1, 48.0f, 90.0f, 1, 0x80, 0x80, 0x80, tmp_str);

    for (i = 0; i < 4; i++)
    {
        sprintf(tmp_str, "  %s   : %d", rgba_str[i], rgba[i]);
        SetASCIIString2(1, 48.0f, yofs + 48, 1, 0x80, 0x80, 0x80, tmp_str);

        yofs += 14;
    }

    SetASCIIString2(1, 48.0f, yofs + 48, 1, 0x80, 0x80, 0x80, "  APPLY");
    SceneSetSquare(2, 48.0f, stw->effect_csr * 14 + 76, 264.0f, 12.0f, 0x50, 0x50, 0x64, 0x50);

    see->aura_size = size;
    see->aura_rate = rate;

    see->aura_rgba = rgba[0] << 0x18 | rgba[1] << 0x10 | rgba[2] << 8 | rgba[3];
}

void SceneTestEneEffPDeform()
{
    SCN_TEST_WRK *stw = &scn_test_wrk;
    SCN_ENE_EF_PDF *pdf;
    SCN_ENE_EF_PDF pdf_buf;
    int ret;
    char tmp_str[256] = "";

    if (stw->effect_id == 1)
    {
        pdf = &(scn_p->man_mdl[stw->ene_mdl_no].ene_efct)->pdf1;
    }
    else if (stw->effect_id == 2)
    {
        pdf = &(scn_p->man_mdl[stw->ene_mdl_no].ene_efct)->pdf2;
    }

    pdf_buf = *pdf;

    ret = ScnUDCtrl(&stw->effect_csr, 7, 0);

    if (ret == -1)
    {
        stw->now_menu = 6;
        stw->effect_id = 0;
        return;
    }

    sprintf(tmp_str, "ENE EFFECT   %s PDEFORM%d", scn_p->man_mdl[stw->ene_mdl_no].prefix, stw->effect_id);
    SetASCIIString2(1, 48.0f, 48.0f, 1, 0x80, 0x80, 0x80, tmp_str);

    switch(stw->effect_csr)
    {
    case 0:
        ScnValueCtrlI((int *)&pdf_buf, 0x24, 0, 1, 10);
    break;
    case 1:
        ScnValueCtrl(&pdf_buf.sclx, 2.0f, -2.0f, 0.01f, 10.0f);
    break;
    case 2:
        ScnValueCtrl(&pdf_buf.scly, 2.0f, -2.0f, 0.01f, 10.0f);
    break;
    case 3:
        ScnValueCtrlI((int *)&pdf_buf.alpha, 0x80, 0, 1, 0x10);
    break;
    case 4:
        ScnValueCtrl(&pdf_buf.rate, 2.0f, 0.0f, 0.01f, 10.0f);
    break;
    case 5:
        ScnValueCtrl(&pdf_buf.trate, 2.0f, 0.0f, 0.01f, 10.0f);
    break;
    case 6:
        if (ret == 1)
        {
            SceneReleaseEffect(scn_p);
        }
    break;
    }

    sprintf(tmp_str, "  TYPE    : %d", pdf_buf.type);
    SetASCIIString2(1, 48.0f, 76.0f, 1, 0x80, 0x80, 0x80, tmp_str);

    sprintf(tmp_str, "  SCALE X : %f", pdf_buf.sclx);
    SetASCIIString2(1, 48.0f, 90.0f, 1, 0x80, 0x80, 0x80, tmp_str);

    sprintf(tmp_str, "  SCALE Y : %f", pdf_buf.scly);
    SetASCIIString2(1, 48.0f, 104.0f, 1, 0x80, 0x80, 0x80, tmp_str);

    sprintf(tmp_str, "  ALPHA   : %d", pdf_buf.alpha);
    SetASCIIString2(1, 48.0f, 118.0f, 1, 0x80, 0x80, 0x80, tmp_str);

    sprintf(tmp_str, "  RATE    : %f", pdf_buf.rate);
    SetASCIIString2(1, 48.0f, 132.0f, 1, 0x80, 0x80, 0x80, tmp_str);

    sprintf(tmp_str, "  TRATE   : %f", pdf_buf.trate);
    SetASCIIString2(1, 48.0f, 146.0f, 1, 0x80, 0x80, 0x80, tmp_str);

    SetASCIIString2(1, 48.0f, 160.0f, 1, 0x80, 0x80, 0x80, "  APPLY");

    SceneSetSquare(2, 48.0f, stw->effect_csr * 14 + 76, 264.0f, 12.0f, 0x50, 0x50, 0x64, 0x50);

    *pdf = pdf_buf;
}

void SceneTestEneEffOthers()
{
    SCN_TEST_WRK *stw = &scn_test_wrk;
    SCN_ENE_EFCT *see;
    int ret;
    char tmp_str[256] = "";
    float dist;
    float pos_ajst;
    int alpha;

    see = scn_p->man_mdl[stw->ene_mdl_no].ene_efct;

    dist = see->pdf_dist;
    pos_ajst = see->aura_pos_ajst;
    alpha = see->mdl_alpha;

    ret = ScnUDCtrl(&stw->effect_csr, 4, 0);

    if (ret == -1)
    {
        stw->now_menu = 6;
        stw->effect_id = 0;

        return;
    }

    sprintf(tmp_str, "ENE EFFECT   %s OTHERS", scn_p->man_mdl[stw->ene_mdl_no].prefix);
    SetASCIIString2(1, 48.0f, 48.0f, 1, 0x80, 0x80, 0x80, tmp_str);

    switch (stw->effect_csr)
    {
    case 0:
        ScnValueCtrl(&dist, 800.0f, -800.0f, 1.0f, 10.0f);
    break;
    case 1:
        ScnValueCtrl(&pos_ajst, 800.0f, -800.0f, 1.0f, 10.0f);
    break;
    case 2:
        ScnValueCtrlI(&alpha, 0x80, 0x0, 1, 16);
    break;
    case 3:
        if (ret == 1)
        {
            SceneReleaseEffect(scn_p);
        }
    break;
    }

    sprintf(tmp_str, "  PDEFORM DIST  : %f", (double)dist);
    SetASCIIString2(1, 48.0f, 76.0f, 1, 0x80, 0x80, 0x80, tmp_str);

    sprintf(tmp_str, "  AURA POS AJST : %f", (double)pos_ajst);
    SetASCIIString2(1, 48.0f, 90.0f, 1, 0x80, 0x80, 0x80, tmp_str);

    sprintf(tmp_str, "  MODEL ALPHA   : %d", alpha);
    SetASCIIString2(1, 48.0f, 104.0f, 1, 0x80, 0x80, 0x80, tmp_str);

    SetASCIIString2(1, 48.0f, 118.0f, 1, 0x80, 0x80, 0x80, "  APPLY");

    SceneSetSquare(2, 48.0f, stw->effect_csr * 14 + 76, 264.0f, 12.0f, 0x50, 0x50, 0x64, 0x50);

    see->pdf_dist = dist;
    see->aura_pos_ajst = pos_ajst;
    see->mdl_alpha = alpha;
}

void SceneTestEffectTest()
{
    FOD_EFF_DATA *fed;

    if (scn_eff_ctrl[0].eff_flg != 0)
    {
        SetEffects(1, 1);
    }

    if (scn_eff_ctrl[1].eff_flg != 0 && mono_flg == 0)
    {
        mono_flg = 1;

        ChangeMonochrome(1);
    }
    else if (scn_eff_ctrl[1].eff_flg == 0 && mono_flg != 0)
    {
        mono_flg = 0;

        ChangeMonochrome(0);
    }

    if (scn_eff_ctrl[3].eff_flg != 0)
    {
        fed = &scn_eff_ctrl[3].eff_data;

        SetEffects(EF_DITHER, 1, fed->dither.type, fed->dither.alpha, fed->dither.speed, fed->dither.alpmax, fed->dither.colmax);
    }

    if (scn_eff_ctrl[4].eff_flg != 0)
    {
        fed = &scn_eff_ctrl[4].eff_data;

        SetEffects(EF_BLUR_N, 1, &fed->blur.alpha, fed->blur.scale, fed->blur.rot, 320.0f, 112.0f);
    }

    if (scn_eff_ctrl[5].eff_flg != 0)
    {
        fed = &scn_eff_ctrl[5].eff_data;

        SetEffects(EF_BLUR_N, 1, &fed->blur.alpha, fed->blur.scale, fed->blur.rot, 320.0f, 112.0f);
    }

    if (scn_eff_ctrl[6].eff_flg != 0)
    {
        fed = &scn_eff_ctrl[6].eff_data;

        SetEffects(EF_BLUR_N, 1, &fed->blur.alpha, fed->blur.scale, fed->blur.rot, 320.0f, 112.0f);
    }

    if (scn_eff_ctrl[7].eff_flg != 0)
    {
        fed = &scn_eff_ctrl[7].eff_data;

        SetEffects(EF_DEFORM, 1, fed->deform.type, fed->deform.volume);
    }

    if (scn_eff_ctrl[8].eff_flg != 0)
    {
        fed = &scn_eff_ctrl[8].eff_data;

        SetEffects(EF_FOCUS, 1, fed->focus.volume);
    }

    if (scn_eff_ctrl[9].eff_flg != 0)
    {
        fed = &scn_eff_ctrl[9].eff_data;

        SetEffects(EF_NCONTRAST, 1, fed->contrast.color, fed->contrast.alpha);
    }

    if (scn_eff_ctrl[10].eff_flg != 0)
    {
        fed = &scn_eff_ctrl[10].eff_data;

        SetEffects(EF_NCONTRAST2, 1, fed->contrast.color, fed->contrast.alpha);

        if (scn_eff_ctrl[10].eff_flg != 0)
        {
            fed = &scn_eff_ctrl[11].eff_data;

            SetEffects(EF_NCONTRAST3, 1, fed->contrast.color, fed->contrast.alpha);
        }
    }

    if (scn_eff_ctrl[12].eff_flg != 0)
    {
        fed = &scn_eff_ctrl[12].eff_data;

        SetEffects(EF_FADEFRAME, 1, fed->f_frame.volume, 0x1000);
    }

    if (scn_eff_ctrl[13].eff_flg != 0)
    {
        fed = &scn_eff_ctrl[13].eff_data;

        SetEffects(EF_OVERLAP, 1, fed->f_frame.volume);
    }

    if (scn_eff_ctrl[14].eff_flg != 0)
    {
        fed = &scn_eff_ctrl[14].eff_data;

        SceneSetSquare(1, 0.0f, 0.0f, 640.0f, 448.0f, fed->fade_scr.r, fed->fade_scr.g, fed->fade_scr.b, fed->fade_scr.a);
    }
}

void SceneTestSetDefEffect()
{
    int i;

    for (i = 0; i < 16; i++)
    {
        scn_eff_ctrl[i].eff_flg = 0;
    }

    mono_flg = 0;

    scn_eff_ctrl[0].eff_data = (FOD_EFF_DATA){0};
    scn_eff_ctrl[1].eff_data = (FOD_EFF_DATA){0};
    scn_eff_ctrl[2].eff_data = (FOD_EFF_DATA){0};


    scn_eff_ctrl[3].eff_data.dither.type = 0x1;
    scn_eff_ctrl[3].eff_data.dither.speed = 6.0f;
    scn_eff_ctrl[3].eff_data.dither.alpha = 32.0f;
    scn_eff_ctrl[3].eff_data.dither.alpmax = 0x40;
    scn_eff_ctrl[3].eff_data.dither.colmax = 0x78;

    for (i = 4; i < 7; i++)
    {
        scn_eff_ctrl[i].eff_data.blur.alpha = 0x40;
        scn_eff_ctrl[i].eff_data.blur.scale = 1010;
        scn_eff_ctrl[i].eff_data.blur.rot = 1810;
    }

    scn_eff_ctrl[7].eff_data.deform.type = 1;
    scn_eff_ctrl[7].eff_data.deform.volume = 0x1e;

    scn_eff_ctrl[8].eff_data.focus.volume = 0x40;

    scn_eff_ctrl[9].eff_data.contrast.color = 0x40;
    scn_eff_ctrl[9].eff_data.contrast.alpha = 0x40;
    scn_eff_ctrl[10].eff_data.contrast.color = 0x40;
    scn_eff_ctrl[10].eff_data.contrast.alpha = 0x40;
    scn_eff_ctrl[11].eff_data.contrast.color = 0x40;
    scn_eff_ctrl[11].eff_data.contrast.alpha = 0x40;

    scn_eff_ctrl[12].eff_data.f_frame.volume = 0x80;

    scn_eff_ctrl[13].eff_data.f_frame.volume = 0x1e;

    scn_eff_ctrl[14].eff_data.fade_scr.r = 0x00;
    scn_eff_ctrl[14].eff_data.fade_scr.g = 0x00;
    scn_eff_ctrl[14].eff_data.fade_scr.b = 0x00;
    scn_eff_ctrl[14].eff_data.fade_scr.a = 0x80;

    scn_eff_ctrl[15].eff_flg = 1;
}

void SceneTestVibrate()
{
    static int vib2_val = 0x80;
    SCN_TEST_WRK *stw = &scn_test_wrk;
    int i;
    int yofs;
    char *vib_menu[3] = {
        "ACT1",
        "ACT2",
        "BOTH",
    };
    char tmp_str[256] = "";
    int ret;

    ret = ScnUDCtrl(&stw->menu_csr, 3, 0);

    if (ret == -1)
    {
        stw->now_menu = 0;
        stw->menu_csr = 0;

        return;
    }

    if (stw->menu_csr == 1)
    {
        ScnValueCtrlI(&vib2_val, 0xff, 0, 1, 0x10);
    }

    if (pad[0].now & 0x20)
    {
        switch (stw->menu_csr)
        {
        case 0:
            VibrateRequest1(0, 1);
        break;
        case 1:
            VibrateRequest2(0, vib2_val);
        break;
        case 2:
            VibrateRequest(0, 1, vib2_val);
        break;
        }
    }

    yofs = 28;

    SetASCIIString2(1, 48.0f, 48.0f, 1, 0x80, 0x80, 0x80, "VIBRATE TEST");

    for (i = 0; i < 3; i++)
    {
        if (i == 1)
        {
            sprintf(tmp_str, "  %s  : %d", vib_menu[i], vib2_val);
        }
        else
        {
            sprintf(tmp_str, "  %s", vib_menu[i]);
        }

        SetASCIIString2(1, 48.0f, yofs + 48, 1, 0x80, 0x80, 0x80, tmp_str);

        yofs += 14;
    }

    SceneSetSquare(2, 48.0f, stw->menu_csr * 14 + 76, 216.0f, 12.0f, 0x50, 0x50, 0x64, 0x50);
}

int SceneTestDraw(int cnt_flg)
{
    char frame[256] = "";

    if (cnt_flg == 0)
    {
        if (pad[0].rpt & 0x10)
        {
            cnt_flg = 1;
        }

        if (pad[0].rpt & 0x80)
        {
            if (scn_fc->now_reso != 0)
            {
                scn_fc->now_reso = scn_fc->now_reso - 1;
            }
            else
            {
                FodSetFrame(scn_fc, scn_fc->now_frame - 1);

                if (scn_fc->now_frame > 1)
                {
                    scn_fc->now_reso++;
                }
            }
        }
    }
    else
    {
        if (pad[0].rpt & 0x10)
        {
            scn_fc->now_reso = 0;
            scn_fc->now_frame += 6;

            if (scn_fc->frame_max < scn_fc->now_frame)
            {
                scn_fc->now_frame = scn_fc->frame_max;
            }

            FodSetFrame(scn_fc, scn_fc->now_frame - 1);
        }
    }

    scn_p->count_flg = cnt_flg;

    if (scn_eff_ctrl[15].eff_flg != 0)
    {
        InitEffectsEF();
    }
    else
    {
        InitEffectScrEF();
        InitEffectObjEF();
        InitEffectOthEF();
    }

    SceneDraw(scn_p->scene_no);
    SceneTestEffectTest();

    gra2dDraw(4);
    gra2dDraw(5);
    gra2dDraw(6);

    if (scn_test_wrk.db_flg != 0)
    {
        sprintf(frame, "NOW FRAME : %04d", scn_p->fod_ctrl.now_frame);
        SetASCIIString2(1, 424.0f, 28.0f, 1, 0x80, 0x80, 0x80, frame);

        SceneSetSquare(2, 418.0f, 26.0f, 204.0f, 16.0f, 0x28, 0x28, 0x28, 0x64);

        sprintf(frame, "NOW RESO  : %04d", scn_p->fod_ctrl.now_reso);
        SetASCIIString2(1, 424.0f, 44.0f, 1, 0x80, 0x80, 0x80, frame);

        SceneSetSquare(2, 418.0f, 42.0f, 204.0f, 16.0f, 0x28, 0x28, 0x28, 0x64);
    }

    return SceneIsEnd();
}

void SceneTestLoopManage(char flg)
{
    if (flg)
    {
        memcpy(scn_test_wrk.light_tmp, scn_fl->all_lit, sizeof(scn_test_wrk.light_tmp));
        memcpy(scn_test_wrk.amb_tmp, scn_fl->amb, sizeof(scn_test_wrk.amb_tmp));
        memcpy(&scn_test_wrk.fog_tmp, &scn_p->fog, sizeof(scn_test_wrk.fog_tmp));
    }
    else
    {
        memcpy(scn_fl->all_lit, scn_test_wrk.light_tmp, sizeof(scn_fl->all_lit));
        memcpy(scn_fl->amb, scn_test_wrk.amb_tmp, sizeof(scn_fl->amb));
        memcpy(&scn_p->fog, &scn_test_wrk.fog_tmp, sizeof(scn_test_wrk.fog_tmp));
    }
}

int ScnLRCtrl(int *csr, int max, int min)
{
    int ret;

    ret = 0;

    if (pad[0].rpt & 0x2000)
    {
        (*csr)++;

        if (max <= *csr)
        {
            *csr = 0;
        }
    }
    else if (pad[0].rpt & 0x8000)
    {
        (*csr)--;

        if (*csr < 0)
        {
            *csr = max - 1;
        }
    }
#ifdef BUILD_EU_VERSION
    else if (pad[0].one & 0x40)
#else
    else if (pad[0].one & 0x20)
#endif
    {
        ret = 1;
    }
#ifdef BUILD_EU_VERSION
    else if (pad[0].one & 0x10)
#else
    else if (pad[0].one & 0x40)
#endif
    {
        ret = -1;
    }

    return ret;
}

int ScnUDCtrl(int *csr, int max, int min)
{
    int ret;

    ret = 0;

    if (pad[0].rpt & 0x4000)
    {
        (*csr)++;

        if (max <= *csr)
        {
            *csr = 0;
        }
    }
    else if (pad[0].rpt & 0x1000)
    {
        (*csr)--;

        if (*csr < 0)
        {
            *csr = max - 1;
        }
    }
    else if (pad[0].one & 0x20)
    {
        ret = 1;
    }
    else if (pad[0].one & 0x40)
    {
        ret = -1;
    }

    return ret;
}

int ScnValueCtrl(float *val, float max, float min, float inc, float mul)
{
    int ret;

    ret = 0;

    if (pad[0].one & 0x2)
    {
        *val = max;

        ret = 1;
    }

    if (pad[0].one & 0x1)
    {
        *val = min;

        ret = 1;
    }

    if (pad[0].now & 0x8)
    {
        inc = inc * mul;
    }

    if (pad[0].now & 0x4)
    {
        inc *= mul * 10.0f;
    }

    if (pad[0].rpt & 0x2000)
    {
        *val += inc;

        ret = 1;

        if (*val > max)
        {
            *val = max;
        }
    }
    else if (pad[0].rpt & 0x8000)
    {
        *val -= inc;

        ret = 1;

        if (*val < min)
        {
            *val = min;
        }
    }

    return ret;
}

int ScnValueCtrlI(int *val, int max, int min, int inc, int mul)
{
    int ret;

    ret = 0;

    if (pad[0].one & 0x2)
    {
        *val = max;

        ret = 1;
    }

    if (pad[0].one & 0x1)
    {
        *val = min;

        ret = 1;
    }

    if (pad[0].now & 0x8)
    {
        inc = inc * mul;
    }

    if (pad[0].rpt & 0x2000)
    {
        *val += inc;

        ret = 1;

        if (*val > max)
        {
            *val = min;
        }
    }
    else if (pad[0].rpt & 0x8000)
    {
        *val -= inc;

        ret = 1;

        if (*val < min)
        {
            *val = max;
        }
    }

    return ret;
}

int ScnValueCtrlC(u_char *val, u_char max, u_char min, int inc, int mul)
{
    int ret;

    ret = 0;

    if (pad[0].one & 0x2)
    {
        *val = max;

        ret = 1;
    }

    if (pad[0].one & 0x1)
    {
        *val = min;

        ret = 1;
    }

    if (pad[0].now & 0x8)
    {
        inc = inc * mul;
    }

    if (pad[0].rpt & 0x2000)
    {
        *val += inc;

        ret = 1;

        if (*val > max)
        {
            *val = min;
        }
    }
    else if (pad[0].rpt & 0x8000)
    {
        *val -= inc;

        ret = 1;

        if (*val < min)
        {
            *val = max;
        }
    }

    if (max < *val)
    {
        *val = max;
    }

    if (*val < min)
    {
        *val = min;
    }

    return ret;
}

int SceneFileSaveBin()
{
    FOD_LIT_SERIAL *fls;
    SgLIGHT *lp;
    sceVu0FVECTOR amb;
    int i;
    int fd;
    int align128[4] = {0};
    char fname[256] = "";
    char *discrim;

    if (scn_p->chapter_no < 0)
    {
        sprintf(fname, "host0:../bin/data/scene/special/scene%s.slt", scene_name[scn_p->scene_no]);
    }
    else
    {
        sprintf(fname, "host0:../bin/data/scene/chapter%d/scene%s.slt", scn_p->chapter_no, scene_name[scn_p->scene_no]);
    }

    fd = sceOpen(fname, SCE_CREAT | SCE_WRONLY);

    if (fd != 0)
    {
        printf("Warning!! Light File Save Fail !!\n");
        printf("File Name <%s>\n", fname);

        return 1;
    }

    sceLseek(fd, 0, 0);

    align128[0] = scn_fl->all_lit_num;

    sceWrite(fd, align128, 0x10);

    for (i = 0; i < scn_fl->all_lit_num; i++)
    {
        fls = &scn_fl->lit_serial[i];
        lp =  &scn_fl->all_lit[i];

        sceWrite(fd, fls, sizeof(FOD_LIT_SERIAL));
        sceWrite(fd, lp, sizeof(SgLIGHT));
    }

    sceWrite(fd, &scn_p->fog, sizeof(SCENE_FOG));

    for (i = 0; i < 6; i++)
    {
        discrim = (char *)&amb[3];

        Vu0CopyVector(amb, scn_fl->amb[i]);

        discrim[0] = 'A';
        discrim[1] = 'M';
        discrim[2] = 'B';
        discrim[3] = '\0';

        sceWrite(fd, amb, sizeof(sceVu0FVECTOR));
    }

    sceClose(fd);

    printf("Light File [%s] Save Complete !!\n", &fname[6]);

    return 0;
}

int SceneFileSaveText()
{
    FOD_LIT_SERIAL *fls;
    SgLIGHT *lp;
    sceVu0FVECTOR ambient;
    int i;
    int fd;
    char *ltype_name[4] = {
        "AMB  ",
        "INF  ",
        "SPOT ",
        "POINT",
    };
    char *amb_str[6] = {
        "COMMON",
        "CHARA ",
        "ROOM  ",
        "FURN  ",
        "DOOR  ",
        "ITEM  ",
    };
    char fname[256] = "";
    char line[256] = "";
    int len;

    if (scn_p->chapter_no < 0)
    {
        sprintf(line, "host0:../bin/data/scene/special/scene%s.slt", scene_name[scn_p->scene_no]);
    }
    else
    {
        sprintf(line, "host0:../bin/data/scene/chapter%d/scene%s.slt", scn_p->chapter_no, scene_name[scn_p->scene_no]);
    }

    len = strlen(line);
    line[len - 4] = '\0';
    sprintf(fname, "%s-lit.txt", line);

    fd = sceOpen(fname, SCE_CREAT | SCE_WRONLY);

    if (fd != 0)
    {
        printf("Warning!! Light File Save Fail !!\n");
        printf("File Name <%s>\n", fname);

        return 1;
    }

    sceLseek(fd, 0, 0);

    sprintf(line, "=============================================\n");
    sceWrite(fd, line, strlen(line));

    sprintf(line, "AMBIENT\n");
    sceWrite(fd, line, strlen(line));

    for (i = 0; i < 6; i++)
    {
        Vu0CopyVector(ambient, scn_fl->amb[0]);

        sprintf(line, "  %s = [R : %.3f] [G : %.3f] [B : %.3f]\n", amb_str[i], ambient[0], ambient[1], ambient[2]);
        sceWrite(fd, line, strlen(line));
    }

    sprintf(line, "=============================================\n");
    sceWrite(fd, line, strlen(line));

    for (i = 0; i < scn_fl->all_lit_num; i++)
    {
        lp = &scn_fl->all_lit[i];
        fls = &scn_fl->lit_serial[i];

        sprintf(line, "=============================================\n");
        sceWrite(fd, line,strlen(line));

        sprintf(line, "No.%02d : %s : %s\n", fls->light_no, ltype_name[fls->light_type], fls->light_name);
        sceWrite(fd, line, strlen(line));

        sprintf(line, "  [R : %.3f] [G : %.3f] [B : %.3f]\n", lp->diffuse[0],  lp->diffuse[1], lp->diffuse[2]);
        sceWrite(fd, line, strlen(line));

        if (fls->light_type == 2 || fls->light_type == 3)
        {
            sprintf(line, "  [POWER : %.2f]\n", lp->power);
            sceWrite(fd, line, strlen(line));

            if (fls->light_type == 2)
            {
                sprintf(line, "  [CONE : %.1f]\n", lp->ambient[1]);
                sceWrite(fd, line, strlen(line));
            }
        }

        sceWrite(fd, "\n", 1);
    }

    sprintf(line, "\n=============================================\n");
    sceWrite(fd, line, strlen(line));

    sprintf(line, "                fog data\n");
    sceWrite(fd, line, strlen(line));

    sprintf(line, "=============================================\n");
    sceWrite(fd, line, strlen(line));

    sprintf(line, "min : max : near : far \n");
    sceWrite(fd, line, strlen(line));

    sprintf(line, "%.1f : %.1f : %.1f : %.1f \n\n", scn_p->fog.min, scn_p->fog.max, scn_p->fog.near, scn_p->fog.far);
    sceWrite(fd, line, strlen(line));

    sprintf(line, " r : g : b \n");
    sceWrite(fd, line, strlen(line));

    sprintf(line, "%d : %d : %d \n\n", scn_p->fog.r, scn_p->fog.g, scn_p->fog.b);
    sceWrite(fd, line, strlen(line));

    sceClose(fd);

    printf("Light File [%s] Save Complete !!\n", &fname[6]);

    return 0;
}
