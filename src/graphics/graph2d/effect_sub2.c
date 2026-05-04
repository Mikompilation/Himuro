#include "common.h"
#include "typedefs.h"
#include "addresses.h"
#include "effect_sub2.h"
#include "enums.h"

#include "ee/eestruct.h"
#include "sce/libvu0.h"

#include "graphics/graph2d/effect_obj.h"
#include "graphics/graph2d/effect_scr.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph2d/sprt.h"
#include "graphics/graph2d/tim2_new.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph3d/libsg.h"
#include "graphics/graph3d/sglib.h"
#include "ingame/event/ev_main.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/item.h"
// #include "ingame/plyr/plyr_ctl.h"
#include "main/glob.h"
#include "os/eeiop/adpcm/ea_btlmode.h"
// #include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_ctrl.h"
#include "os/eeiop/adpcm/ea_dat.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"
#include "outgame/btl_mode/btl_dat.h"
#include "outgame/btl_mode/btl_mode.h"
#include "outgame/mode_slct.h"

typedef struct {
    void *start_point;
    short int start_num;
    short int end_num;
} ANM_WRK;

typedef struct {
    sceVu0FVECTOR fall_speed;
    sceVu0IVECTOR wind;
    sceVu0FVECTOR rotate;
    int stop_time;
    short int r;
    short int g;
    short int b;
    short int a;
} FALL_TABLE;

typedef struct {
    sceVu0FVECTOR mpos_keep;
    void *pos_p;
    short int at_ground[600];
    short int rgba[600][4];
    int fnum_keep;
    int fall_count;
    float fall_rate;
    float rate_remain;
    short int mode_keep;
    short int area_keep;
    short int dist;
    short int init_flg;
    short int offs[3];
} LEAVES_TABLE;

typedef struct {
    u_long tex_keep;
    short int tm2_id;
    short int mode;
    short int load_id;
    short int pad;
} BTL_ANM_LOAD;

enum T_LOAD_MODE {
    BANM_NORM = 0,
    BANM_LOAD = 1,
    BANM_AFTLOAD = 2,
    BANM_TEXDISP = 3
};

#define PI 3.1415927f

#define ANM2D_DAT_TABLE_P(table_p) ((ANM2D_DAT_TABLE *)table_p)
#define ANM2D_WRK_TABLE_P(table_p) ((ANM2D_WRK_TABLE *)table_p)

#include "data/fall_table.h" // FALL_TABLE fall_table[];
short int fallen_effect_switch = 0;
short int gus_effect_switch = 0;
short int hole_effect_switch = 0;
short int line_effect_switch[4] = { 0, 0, 0, 0 };

#include "data/btl_strt.h"  // SPRT_SDAT btl_strt[11];
#include "data/btl_rslt.h"  // SPRT_SDAT btl_rslt[23];
#include "data/msg_strt.h"  // SPRT_SDAT msg_strt;
#include "data/btl_msel.h"  // SPRT_SDAT btl_msel[];
#include "data/btl_msel2.h" // SPRT_SDAT btl_msel2[];
#include "data/anm2d_dat.h" // ANM2D_DAT_TABLE anm2d_dat[][12];
#include "data/wrk_table.h" // ANM2D_WRK_TABLE wrk_table[];
#include "data/clear_all.h" // SPRT_DAT clear_all[11];
#include "data/story_end.h" // SPRT_DAT story_end[11];
#include "data/fall_wrk.h"  // LEAVES_TABLE fall_wrk;
#include "data/gus_wrk.h"   // LEAVES_TABLE gus_wrk;
#include "data/hole_wrk.h"  // LEAVES_TABLE hole_wrk;
#include "data/line_wrk.h"  // LEAVES_TABLE line_wrk[4];
BTL_ANM_LOAD banm = {0};

static sceVu0FVECTOR leaves[600];
static sceVu0FVECTOR leavesaim[600];
static sceVu0FVECTOR accel[600];
static sceVu0FVECTOR rots[600];
static sceVu0FVECTOR hole_gus[200];
static sceVu0FVECTOR line_gus[4][80];

void InitEffectSub2()
{
    fallen_effect_switch = 0;
    gus_effect_switch = 0;

    fall_wrk = (LEAVES_TABLE){0};
    gus_wrk = (LEAVES_TABLE){0};
    hole_wrk = (LEAVES_TABLE){0};

    InitCallAnm();
}

void CallFallenEffect(sceVu0FVECTOR mpos, int area, int fall_num, int fall_mode)
{
    fallen_effect_switch = 1;

    if (fall_wrk.init_flg == 0)
    {
        FallObjInitAll(mpos, area, fall_num, fall_mode, 1500);

        fall_wrk.init_flg = 1;
    }

    FallObjDropSet();
}

void StopFallenEffect()
{
    fallen_effect_switch = 0;

    fall_wrk.init_flg = 0;
}

void FallObjInit(sceVu0FVECTOR mpos, int leaf_no, int area, int fall_num, int fall_mode)
{
    int tmp;

    tmp = (int)(10.0f * VER_RAND()) + 1;

    if (tmp < 6)
    {
        leaves[leaf_no][0] = mpos[0] + (int)((float)area * VER_RAND());
    }
    else
    {
        leaves[leaf_no][0] = mpos[0] - (int)((float)area * VER_RAND());
    }

    tmp = (int)(10.0f * VER_RAND()) + 1;

    if (tmp < 6)
    {
        leaves[leaf_no][2] = mpos[2] + (int)((float)area * VER_RAND());
    }
    else
    {
        leaves[leaf_no][2] = mpos[2] - (int)((float)area * VER_RAND());
    }

    leaves[leaf_no][1] = mpos[1] - fall_wrk.dist;
    leaves[leaf_no][3] = 1.0f;

    tmp = (int)(10.0f * VER_RAND()) + 1;

    if (tmp < 6)
    {
        leavesaim[leaf_no][0] = (int)(fall_table[fall_mode].fall_speed[3] * VER_RAND());
    }
    else
    {
        leavesaim[leaf_no][0] = -(int)(fall_table[fall_mode].fall_speed[3] * VER_RAND());
    }

    tmp = (int)(10.0f * VER_RAND()) + 1;

    if (tmp < 6)
    {
        leavesaim[leaf_no][2] = (int)(fall_table[fall_mode].fall_speed[3] * VER_RAND());
    }
    else
    {
        leavesaim[leaf_no][2] = -(int)(fall_table[fall_mode].fall_speed[3] * VER_RAND());
    }

    tmp = fall_table[fall_mode].fall_speed[1] - 1 + (int)(3.0f * VER_RAND());

    accel[leaf_no][0] = 0.0f;
    accel[leaf_no][1] = tmp;
    accel[leaf_no][2] = 0.0f;

    rots[leaf_no][0] = 0.0f;
    rots[leaf_no][1] = 0.0f;
    rots[leaf_no][2] = 0.0f;

    fall_wrk.rgba[leaf_no][0] = fall_table[fall_mode].r;
    fall_wrk.rgba[leaf_no][1] = fall_table[fall_mode].g;
    fall_wrk.rgba[leaf_no][2] = fall_table[fall_mode].b;
    fall_wrk.rgba[leaf_no][3] = fall_table[fall_mode].a;

    fall_wrk.at_ground[leaf_no] = 0;
}

void FallObjInitAll(sceVu0FVECTOR mpos, int area, int fall_num, int fall_mode, int height)
{
    int i;

    fall_wrk.pos_p = mpos;
    fall_wrk.fnum_keep = fall_num;
    fall_wrk.mode_keep = fall_mode;
    fall_wrk.area_keep = area / 2;
    fall_wrk.dist = height;
    fall_wrk.fall_count = 0;
    fall_wrk.fall_rate = (fall_wrk.dist / fall_table[fall_wrk.mode_keep].fall_speed[1] + fall_table[fall_wrk.mode_keep].stop_time) / fall_num;

    for (i = 0; i < fall_num; i++)
    {
        FallObjInit(fall_wrk.pos_p, i, area, fall_num, fall_mode);
    }
}

void FallObjDropSet()
{
    int i;

    for (i = 0; i < fall_wrk.fnum_keep; i++)
    {
        leaves[i][1] += (int)((float)fall_wrk.dist * VER_RAND()) -
            fall_table[fall_wrk.mode_keep].stop_time * fall_table[fall_wrk.mode_keep].fall_speed[1];
    }

    fall_wrk.fall_count = fall_wrk.fnum_keep;
    fall_wrk.rate_remain = fall_wrk.fall_rate;
}

void FallenObjects()
{
    static int now_status = 0;
    int i;

    if (fallen_effect_switch == 0)
    {
        return;
    }

    if (fall_wrk.fall_count < fall_wrk.fnum_keep)
    {
        fall_wrk.rate_remain++;

        while (fall_wrk.fall_rate <= fall_wrk.rate_remain)
        {
            fall_wrk.fall_count++;
            fall_wrk.rate_remain -= fall_wrk.fall_rate;
        }
    }

    for (i = 0; i < fall_wrk.fnum_keep; i++)
    {
        if (i > fall_wrk.fall_count)
        {
            return;
        }

        if (leaves[i][1] >= plyr_wrk.move_box.pos[1] - 20.0f)
        {
            fall_wrk.at_ground[i]++;

            if (fall_wrk.rgba[i][3] > 0)
            {
                fall_wrk.rgba[i][3] = fall_table[fall_wrk.mode_keep].a -
                          ((float)fall_wrk.at_ground[i] / (float)fall_table[fall_wrk.mode_keep].stop_time) *
                          fall_table[fall_wrk.mode_keep].a;
            }

            if (fall_table[fall_wrk.mode_keep].stop_time < fall_wrk.at_ground[i])
            {
                leaves[i][1] = fall_wrk.mpos_keep[1];

                FallObjInit(fall_wrk.pos_p, i, fall_wrk.area_keep, fall_wrk.fnum_keep, fall_wrk.mode_keep);
            }
        }
        else
        {
            leaves[i][1] += accel[i][1];

            FallObjTrans(leaves[i], accel[i], leavesaim[i], fall_wrk.mode_keep);
            FallObjRot(rots[i], accel[i], leavesaim[i], fall_wrk.mode_keep);
        }

        FallObjLight(leaves[i], fall_wrk.rgba[i], fall_wrk.mode_keep);
        FallObjDraw(leaves[i], rots[i], fall_wrk.rgba[i], fall_wrk.mode_keep);
    }
}

static u_char r_temp = 33;
static u_char g_temp = 48;
static u_char b_temp = 47;
static u_char rate_temp = 7;
ANM_WRK anm_wrk = {0};
static short int anm_no_keep = -1;
static short int anm_load_id = 0;
static u_char anm_init = 0;
static u_char clear_end = 0;
static u_char clear_end2 = 0;
static u_char clear_end3 = 0;
int clear_anime_timer = 0;

void FallObjTrans(sceVu0FVECTOR leaf, sceVu0FVECTOR axel, sceVu0FVECTOR aim, int fall_mode)
{
    int tmp;

    if (axel[0] < aim[0])
    {
        axel[0] = axel[0] + fall_table[fall_mode].fall_speed[0];

        if (axel[0] < aim[0] * 0.5f)
        {
            leaf[0] = leaf[0] + axel[0];
        }
        else
        {
            leaf[0] = leaf[0] + (aim[0] - axel[0]);
        }

        if (aim[0] <= axel[0])
        {
            tmp = (int)(10.0f * VER_RAND()) + 1;

            if (tmp <= 5)
            {
                aim[0] = (int)(fall_table[fall_mode].fall_speed[3] * VER_RAND());
            }
            else
            {
                aim[0] = -(int)(fall_table[fall_mode].fall_speed[3] * VER_RAND());
            }

            axel[0] = 0.0f;
        }
    }
    else
    {
        axel[0] = axel[0] - fall_table[fall_mode].fall_speed[0];

        if (aim[0] * 0.5f < axel[0])
        {
            leaf[0] = leaf[0] + axel[0];
        }
        else
        {
            leaf[0] = leaf[0] + (aim[0] - axel[0]);
        }

        if (axel[0] <= aim[0])
        {
            tmp = (int)(10.0f * VER_RAND()) + 1;

            if (tmp <= 5)
            {
                aim[0] = (int)(fall_table[fall_mode].fall_speed[3] * VER_RAND());
            }
            else
            {
                aim[0] = -(int)(fall_table[fall_mode].fall_speed[3] * VER_RAND());
            }

            axel[0] = 0.0f;
        }
    }

    tmp = (int)(10.0f * VER_RAND()) + 1;

    if (tmp <= 5)
    {
        leaf[1] += 1.0f;
    }
    else
    {
        leaf[1] -= 1.0f;
    }

    if (axel[2] < aim[2])
    {
        axel[2] = axel[2] + fall_table[fall_mode].fall_speed[2];

        if (axel[2] < aim[2] * 0.5f)
        {
            leaf[2] = leaf[2] + axel[2];
        }
        else
        {
            leaf[2] = leaf[2] + (aim[2] - axel[2]);
        }

        if (axel[2] >= aim[2])
        {
            tmp = (int)(10.0f * VER_RAND()) + 1;

            if (tmp <= 5)
            {
                aim[2] = (int)(fall_table[fall_mode].fall_speed[3] * VER_RAND());
            }
            else
            {
                aim[2] = -(int)(fall_table[fall_mode].fall_speed[3] * VER_RAND());
            }

            axel[2] = 0.0f;
        }
    }
    else
    {
        axel[2] = axel[2] - fall_table[fall_mode].fall_speed[2];

        if (aim[2] * 0.5f < axel[2])
        {
            leaf[2] = leaf[2] + axel[2];
        }
        else
        {
            leaf[2] = leaf[2] + (aim[2] - axel[2]);
        }

        if (axel[2] <= aim[2])
        {
            tmp = (int)(10.0f * VER_RAND()) + 1;

            if (tmp <= 5)
            {
                aim[2] = (int)(fall_table[fall_mode].fall_speed[3] * VER_RAND());
            }
            else
            {
                aim[2] = -(int)(fall_table[fall_mode].fall_speed[3] * VER_RAND());
            }

            axel[2] = 0.0f;
        }
    }
}

void FallObjRot(sceVu0FVECTOR rotation, sceVu0FVECTOR axel, sceVu0FVECTOR aim, int fall_mode)
{
    if (fall_mode == 0)
    {
        Get2PosRot(camera.p, camera.i, &rotation[0], &rotation[1]);

        return;
    }

    if (fall_mode != 1 && fall_mode != 2)
    {
        return;
    }

    if (axel[0] < aim[0] && (aim[0] < -1.0f || aim[0] > 1.0f))
    {

        if (axel[0] < aim[0] * 0.5f)
        {
            rotation[2] = (axel[0] * -(PI / 2)) / (aim[0] * 0.5f);
        }
        else
        {
            rotation[2] = ((aim[0] - axel[0]) * -(PI / 2)) / (aim[0] * 0.5f);
        }

    }
    else
    {
        if (axel[0] >= aim[0] && (aim[0] < -1.0f || aim[0] > 1.0f))
        {
            if (aim[0] * 0.5f < axel[0])
            {
                rotation[2] = (axel[0] * (PI / 2)) / (aim[0] * 0.5f);
            }
            else
            {
                rotation[2] = ((aim[0] - axel[0]) * (PI / 2)) / (aim[0] * 0.5f);
            }

        }
        else
        {
            rotation[1] = (fall_table[fall_mode].rotate[1] * PI) / 180.0f;
        }
    }

    if (axel[2] < aim[2] && (aim[2] < -1.0f || aim[2] > 1.0f))
    {
        if (axel[2] < aim[2] * 0.5f)
        {
            rotation[0] = (axel[2] * (PI / 2)) / (aim[2] * 0.5f);
        }
        else
        {
            rotation[0] = ((aim[2] - axel[2]) * (PI / 2)) / (aim[2] * 0.5f);
        }
    }
    else if (axel[2] >= aim[2] && (aim[2] < -1.0f || aim[2] > 1.0f))
    {
        if (aim[2] * 0.5f < axel[2])
        {
            rotation[0] = (axel[2] * -(PI / 2)) / (aim[2] * 0.5f);
        }
        else
        {
            rotation[0] = ((aim[2] - axel[2]) * -(PI / 2)) / (aim[2] * 0.5f);
        }
    }
    else
    {
        rotation[1] = (fall_table[fall_mode].rotate[1] * PI) / 180.0f;
    }
}

void FallObjWind(sceVu0FVECTOR leaf, int fall_mode)
{
    leaf[0] += fall_table[fall_mode].wind[0];
    leaf[1] += fall_table[fall_mode].wind[1];
    leaf[2] += fall_table[fall_mode].wind[2];
}

void FallObjLight(sceVu0FVECTOR leaf, short int *rgba, int fall_mode)
{
    float tes1;
    float tes2;

    tes1 = 0.0f;
    tes2 = 0.0f;

    if (GetCornHitCheck2(leaf, 1200.0f, &tes1, &tes2))
    {
        rgba[0] = fall_table[fall_mode].r;
        rgba[1] = fall_table[fall_mode].g;
        rgba[2] = fall_table[fall_mode].b;
    }
    else
    {
        rgba[0] = fall_table[fall_mode].r / 2;
        rgba[1] = fall_table[fall_mode].g / 2;
        rgba[2] = fall_table[fall_mode].b / 2;
    }
}

void FallObjDraw(sceVu0FVECTOR mpos, sceVu0FVECTOR rotation, short int *rgba, int fall_mode)
{
    int i;
    int w;
    int th;
    int tw;
    int bak;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0IVECTOR ivec[4];
    sceVu0FVECTOR wpos;
    sceVu0FVECTOR ppos[4] = {
        { -12.0f, +12.0f, 0.0f, 1.0f },
        { +12.0f, +12.0f, 0.0f, 1.0f },
        { -12.0f, -12.0f, 0.0f, 1.0f },
        { +12.0f, -12.0f, 0.0f, 1.0f },
    };
    u_char mr;
    u_char mg;
    u_char mb;
    u_long tex0;

    wpos[0] = mpos[0];
    wpos[1] = mpos[1];
    wpos[2] = mpos[2];

    if (monochrome_mode)
    {
        mg = (rgba[0] + rgba[1] + rgba[2]) / 3;
        mb = mg;
        mr = mg;
    }
    else
    {
        mr = rgba[0];
        mg = rgba[1];
        mb = rgba[2];
    }

    sceVu0UnitMatrix(wlm);
    sceVu0RotMatrixX(wlm, wlm, rotation[0]);
    sceVu0RotMatrixY(wlm, wlm, rotation[1]);
    sceVu0RotMatrixZ(wlm, wlm, rotation[2]);
    sceVu0TransMatrix(wlm, wlm, wpos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);

    sceVu0RotTransPersN(ivec, slm, ppos, 4, 0);

    w = 0;

    for (i = 0; i < 4; i++)
    {
        if (ivec[i][0] < 0x4000 || ivec[i][0] > 0xc000)
        {
            w = 1;
        }

        if (ivec[i][1] < 0x4000 || ivec[i][1] > 0xc000)
        {
            w = 1;
        }

        if (ivec[i][2] < 0xff || ivec[i][2] > 0xffffff)
        {
            w = 1;
        }
    }

    if (w == 0)
    {
        if (fall_mode == 0)
        {
            i = monochrome_mode + 58;
        }
        else if (fall_mode == 1)
        {
            i = monochrome_mode + 48;
        }
        else if (fall_mode == 2)
        {
            i = monochrome_mode + 20;
        }

        th = effdat[i].h * 16;
        tw = effdat[i].w * 16;

        tex0 = effdat[i].tex0;

        Reserve2DPacket(0x1000);

        bak = ndpkt;

        pbuf[ndpkt++].ul128 = (u_long128)0;

        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_FALSE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
        pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

        pbuf[ndpkt].ul64[0] = 0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

        pbuf[ndpkt].ul64[0] = tex0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
        pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
        pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GREATER, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL);
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 0, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 3);
        pbuf[ndpkt++].ul64[1] = 0 \
            | SCE_GS_RGBAQ << (4 * 0)
            | SCE_GS_UV    << (4 * 1)
            | SCE_GS_XYZF2 << (4 * 2);

        for (i = 0; i < 4; i++)
        {
            pbuf[ndpkt].ui32[0] = mr;
            pbuf[ndpkt].ui32[1] = mg;
            pbuf[ndpkt].ui32[2] = mb;
            pbuf[ndpkt++].ui32[3] = rgba[3];

            pbuf[ndpkt].ui32[0] = (i % 2) ? tw : 0;
            pbuf[ndpkt].ui32[1] = (i / 2) ? th : 0;
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = ivec[i][0];
            pbuf[ndpkt].ui32[1] = ivec[i][1];
            pbuf[ndpkt].ui32[2] = ivec[i][2];
            pbuf[ndpkt++].ui32[3] = (i <= 1) ? 0x8000 : 0;
        }

        pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;
    }
}

void GusObjDebug()
{
    if (PAD_BTN_HELD(PAD_L3))
    {
        if (PAD_BTN_NOT_HELD(PAD_R1))
        {
            if (r_temp < 0xff)
            {
                r_temp++;
            }
        }
        else
        {
            if (r_temp != 0)
            {
                r_temp--;
            }
        }
    }

    if (PAD_BTN_HELD(PAD_R3))
    {
        if (PAD_BTN_NOT_HELD(PAD_R1))
        {
            if (g_temp < 0xff)
            {
                g_temp++;
            }
        }
        else
        {
            if (g_temp != 0)
            {
                g_temp--;
            }
        }
    }

    if (PAD_BTN_HELD(PAD_CROSS))
    {
        if (PAD_BTN_NOT_HELD(PAD_R1))
        {
            if (b_temp < 0xff)
            {
                b_temp++;
            }
        }
        else
        {
            if (b_temp != 0)
            {
                b_temp--;
            }
        }
    }
}

void CallGusEffect(sceVu0FVECTOR mpos, int area, int height)
{
    gus_effect_switch = 1;

    if (gus_wrk.init_flg != 0)
    {
        GusObjMove(mpos);
    }
    else
    {
        GusObjInitAll(mpos,area,height);

        gus_wrk.init_flg = 1;
    }
}

void StopGusEffect()
{
    gus_effect_switch = 0;
}

void GusObjMove(sceVu0FVECTOR mpos)
{
    gus_wrk.pos_p = mpos;
    gus_wrk.mpos_keep[0] = mpos[0];
    gus_wrk.mpos_keep[2] = mpos[2];
}

void GusObjInit(sceVu0FVECTOR mpos, int leaf_no, int area)
{
    int tmp;

    gus_wrk.mpos_keep[0] = mpos[0];
    gus_wrk.mpos_keep[1] = mpos[1];
    gus_wrk.mpos_keep[2] = mpos[2];
    gus_wrk.mpos_keep[3] = mpos[3];

    tmp = (int)(10.0f * VER_RAND()) + 1;

    if (tmp < 6)
    {
        leaves[leaf_no][0] = (int)((float)gus_wrk.area_keep * VER_RAND());
    }
    else
    {
        leaves[leaf_no][0] = -(int)((float)gus_wrk.area_keep * VER_RAND());
    }

    tmp = (int)(10.0f * VER_RAND()) + 1;

    if (tmp < 6)
    {
        leaves[leaf_no][2] = (int)((float)gus_wrk.area_keep * VER_RAND());
    }
    else
    {
        leaves[leaf_no][2] = -(int)((float)gus_wrk.area_keep * VER_RAND());
    }

    leaves[leaf_no][1] = mpos[1];
    leaves[leaf_no][3] = 1.0f;

    accel[leaf_no][1] = 0.0f;
    accel[leaf_no][0] = 0.0f;
    accel[leaf_no][2] = 0.0f;

    rots[leaf_no][0] = 0.0f;
    rots[leaf_no][1] = 0.0f;
    rots[leaf_no][2] = 0.0f;

    gus_wrk.rgba[leaf_no][0] = 0x80;
    gus_wrk.rgba[leaf_no][1] = 0x80;
    gus_wrk.rgba[leaf_no][2] = 0x80;
    gus_wrk.rgba[leaf_no][3] = 0x40;

    gus_wrk.at_ground[leaf_no] = 2.0f * VER_RAND();
}

void GusObjInit3(sceVu0FVECTOR mpos, int leaf_no, int area, int fall_mode)
{
    int tmp;

    leaves[leaf_no][0] = (int)(-area / 2) + (int)((leaf_no * 250) % area);
    leaves[leaf_no][2] = (int)(-area / 2) + (int)((leaf_no * 250) / area) * 250;

    leaves[leaf_no][1] = mpos[1];
    leaves[leaf_no][3] = 1.0f;

    tmp = (int)((fall_table[fall_mode].fall_speed[1] - 1.0) + ((int) (3.0 * VER_RAND()))); // 1.0 and 3.0 are double!

    accel[leaf_no][0] = 0.0f;
    accel[leaf_no][1] = tmp;
    accel[leaf_no][2] = 0.0f;

    rots[leaf_no][0] = 0.0f;
    rots[leaf_no][1] = 0.0f;
    rots[leaf_no][2] = 0.0f;

    gus_wrk.rgba[leaf_no][0] = 0x80;
    gus_wrk.rgba[leaf_no][1] = 0x80;
    gus_wrk.rgba[leaf_no][2] = 0x80;
    gus_wrk.rgba[leaf_no][3] = fall_table[fall_mode].a;

    gus_wrk.at_ground[leaf_no] = 0;
}

void GusObjInitAll(sceVu0FVECTOR mpos, int area, int height)
{
    int i;

    gus_wrk.mpos_keep[0] = mpos[0];
    gus_wrk.mpos_keep[1] = mpos[1];
    gus_wrk.mpos_keep[2] = mpos[2];
    gus_wrk.mpos_keep[3] = mpos[3];

    gus_wrk.area_keep = area / 2;
    gus_wrk.fall_rate = 7.0f;
    gus_wrk.dist = height;
    gus_wrk.fnum_keep = gus_wrk.dist / 7.0f;
    gus_wrk.fall_count = 0;
    gus_wrk.pos_p = mpos;

    for (i = 0; i < gus_wrk.fnum_keep; i++)
    {
        GusObjInit(gus_wrk.pos_p,i,area);
    }

    gus_wrk.init_flg = 1;
}

void GusObjects()
{
    int i;

    if (gus_effect_switch != 0)
    {
        if (gus_wrk.fall_count < gus_wrk.fnum_keep)
        {
            for (
                gus_wrk.rate_remain = gus_wrk.rate_remain + 1.0f;
                gus_wrk.fall_rate <= gus_wrk.rate_remain;
                gus_wrk.rate_remain = gus_wrk.rate_remain - gus_wrk.fall_rate)
            {
                gus_wrk.fall_count++;
            }
        }

        for (i = 0; i < gus_wrk.fnum_keep; i++)
        {
            if (i <= gus_wrk.fall_count && GusObjTrans(i) != 0)
            {
                GusObjInit(gus_wrk.pos_p, i, gus_wrk.area_keep);
            }
        }

        for (i = 0; i < gus_wrk.fnum_keep; i++)
        {
            if (i <= gus_wrk.fall_count)
            {
                GusAlpha(i);
            }
        }

        GusObjDraw(gus_wrk.fnum_keep, 500, 0);
    }
}

int GusObjTrans(int leaf_no)
{
    int ret_num = 0;

    leaves[leaf_no][1]--;

    if (leaves[leaf_no][1] < gus_wrk.mpos_keep[1] - gus_wrk.dist)
    {
        ret_num = 1;

        leaves[leaf_no][1] = gus_wrk.mpos_keep[1];
    }

    return ret_num;
}

void GusAlpha(int leaf_no)
{
    if (gus_wrk.at_ground[leaf_no] >= 1)
    {
        if (gus_wrk.rgba[leaf_no][3] < 191)
        {
            gus_wrk.rgba[leaf_no][3]++;
        }
        else
        {
            gus_wrk.at_ground[leaf_no] = 0;
        }
    }
    else
    {
        if (gus_wrk.rgba[leaf_no][3] > 65)
        {
            gus_wrk.rgba[leaf_no][3]--;
        }
        else
        {
            gus_wrk.at_ground[leaf_no] = 1;
        }
    }
}

void GusObjDraw(int leaf_num, int area, int fall_mode)
{
    int i;
    int w;
    int k;
    int th;
    int tw;
    int bak;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0FMATRIX wlm2;
    sceVu0IVECTOR ivec[4];
    sceVu0FVECTOR wpos;
    sceVu0FVECTOR ppos[4] = {
        { -200.0f, +200.0f, 0.0f, 1.0f },
        { +200.0f, +200.0f, 0.0f, 1.0f },
        { -200.0f, -200.0f, 0.0f, 1.0f },
        { +200.0f, -200.0f, 0.0f, 1.0f },
    };
    u_char mr;
    u_char mg;
    u_char mb;
    u_long tex0;

    i = 8;

    tex0 = effdat[i].tex0;

    tw = effdat[i].w * 16;
    th = effdat[i].h * 16;

    if (monochrome_mode)
    {
        mg = (r_temp + g_temp + b_temp) / 3;
        mb = mg;
        mr = mg;
    }
    else
    {
        mr = r_temp;
        mg = g_temp;
        mb = b_temp;
    }

    Reserve2DPacket(0x1000);

    bak = ndpkt;

    pbuf[ndpkt++].ul128 = (u_long128)0;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    pbuf[ndpkt].ul64[0] = tex0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GREATER, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    Get2PosRot(camera.p, camera.i, &rots[0][0], &rots[0][1]);

    sceVu0UnitMatrix(wlm);
    sceVu0RotMatrixX(wlm, wlm, rots[0][0]);
    sceVu0RotMatrixY(wlm, wlm, rots[0][1]);
    sceVu0TransMatrix(wlm, wlm, gus_wrk.mpos_keep);

    for (k = 0; k < gus_wrk.fnum_keep; k++)
    {
        if (k <= gus_wrk.fall_count)
        {
            sceVu0TransMatrix(wlm2, wlm, leaves[k]);
            sceVu0MulMatrix(slm, SgWSMtx, wlm2);
            sceVu0RotTransPersN(ivec, slm, ppos, 4, 0);

            w = 0;

            for (i = 0; i < 4; i++)
            {
                if (ivec[i][0] < 0x4000 || ivec[i][0] > 0xc000)
                {
                    w = 1;
                }

                if (ivec[i][1] < 0x4000 || ivec[i][1] > 0xc000)
                {
                    w = 1;
                }

                if (ivec[i][2] < 0xff || ivec[i][2] > 0xffffff)
                {
                    w = 1;
                }
            }

            if (w == 0)
            {
                pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 340, SCE_GIF_PACKED, 3);
                pbuf[ndpkt++].ul64[1] = 0 \
                    | SCE_GS_RGBAQ << (4 * 0)
                    | SCE_GS_UV    << (4 * 1)
                    | SCE_GS_XYZF2 << (4 * 2);

                for (i = 0; i < 4; i++)
                {
                    pbuf[ndpkt].ui32[0] = mr;
                    pbuf[ndpkt].ui32[1] = mg;
                    pbuf[ndpkt].ui32[2] = mb;
                    pbuf[ndpkt++].ui32[3] = gus_wrk.rgba[k][3] >> 3;

                    pbuf[ndpkt].ui32[0] = (i & 1) * tw;
                    pbuf[ndpkt].ui32[1] = (i / 2) ? th : 0;
                    pbuf[ndpkt].ui32[2] = 0;
                    pbuf[ndpkt++].ui32[3] = 0;

                    pbuf[ndpkt].ui32[0] = ivec[i][0];
                    pbuf[ndpkt].ui32[1] = ivec[i][1];
                    pbuf[ndpkt].ui32[2] = ivec[i][2];
                    pbuf[ndpkt++].ui32[3] = (i <= 1) ? 0x8000 : 0;
                }
            }
        }
    }

    pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;
}

void CallHoleGusEffect(sceVu0FVECTOR mpos)
{
    int i;

    hole_effect_switch = 1;

    if (hole_wrk.init_flg == 0)
    {
        HoleGusInitAll(mpos);

        for (i = 0; i < hole_wrk.fnum_keep; i++)
        {
            HoleGusSetPos(i);
        }

        hole_wrk.init_flg = 1;
    }
}

void StopHoleGusEffect()
{
    hole_effect_switch = 0;

    hole_wrk.init_flg = 0;
}

void HoleGusInit(sceVu0FVECTOR mpos, int leaf_no)
{
    int tmp;

    hole_gus[leaf_no][0] = hole_wrk.mpos_keep[0];
    hole_gus[leaf_no][1] = hole_wrk.mpos_keep[1];
    hole_gus[leaf_no][2] = hole_wrk.mpos_keep[2];
    hole_gus[leaf_no][3] = 1.0f;

    tmp = (int)(10.0f * VER_RAND()) + 1;

    if (tmp < 6)
    {
        hole_wrk.rgba[leaf_no][0] = +(int)(20.0f * VER_RAND());
    }
    else
    {
        hole_wrk.rgba[leaf_no][0] = -(int)(20.0f * VER_RAND());
    }

    tmp = (int)(10.0f * VER_RAND()) + 1;

    if (tmp < 6)
    {
        hole_wrk.rgba[leaf_no][1] = (int)(20.0f * VER_RAND());
    }
    else
    {
        hole_wrk.rgba[leaf_no][1] = -(int)(20.0f * VER_RAND());
    }

    tmp = (int)(10.0f * VER_RAND()) + 1;

    if (tmp < 6)
    {
        hole_wrk.rgba[leaf_no][2] = hole_wrk.rgba[leaf_no][2] + (int)(2.0f * VER_RAND());
    }
    else
    {
        hole_wrk.rgba[leaf_no][2] = hole_wrk.rgba[leaf_no][2] - (int)(2.0f * VER_RAND());
    }

    hole_wrk.rgba[leaf_no][3] = 0x140;

    hole_wrk.at_ground[leaf_no] = 0;
}

void HoleGusInitAll(sceVu0FVECTOR mpos)
{
    int i;

    hole_wrk.mpos_keep[0] = mpos[0];
    hole_wrk.mpos_keep[1] = mpos[1];
    hole_wrk.mpos_keep[2] = mpos[2];
    hole_wrk.mpos_keep[3] = mpos[3];

    hole_wrk.area_keep = 300;
    hole_wrk.dist = 500;
    hole_wrk.fnum_keep = 100;
    hole_wrk.fall_count = 0;
    hole_wrk.fall_rate = 7.0;
    hole_wrk.rate_remain = 0.0;

    for (i = 0; i < hole_wrk.fnum_keep; i++)
    {
        HoleGusInit(mpos,i);
    }
}

void HoleGusSetPos(int leaf_no)
{
    hole_wrk.rgba[leaf_no][3] = (int)(240.0f * VER_RAND()) + 80;

    hole_gus[leaf_no][2] += (int)((320 - hole_wrk.rgba[leaf_no][3]) / 3 * 10);

    hole_wrk.at_ground[leaf_no] += (320 - hole_wrk.rgba[leaf_no][3]) / 3 * 2;
}

void HoleGusObjects()
{
    int i;

    i = 0;

    if (hole_effect_switch)
    {
        while (i < hole_wrk.fnum_keep)
        {
            HoleGusTransX(i);
            HoleGusTransY(i);
            HoleGusTransZ(i);

            if (HoleGusAlpha(i))
            {
                HoleGusInit(hole_wrk.mpos_keep,i);
            }

            i++;
        }

        for (i = 0; i < hole_wrk.fnum_keep; i++)
        {
            // debug code?
        }

        for (i = 0; i < hole_wrk.fnum_keep; i++)
        {
            HoleGusDraw(i);
        }
    }
}

int HoleGusTransX(int leaf_no)
{
    hole_gus[leaf_no][0] += hole_wrk.rgba[leaf_no][0] / 8;

    return 0;
}

int HoleGusTransZ(int leaf_no)
{
    int ret_num;

    ret_num = 0;

    if (2.0f < 10 - hole_wrk.at_ground[leaf_no] / 8)
    {
        hole_gus[leaf_no][2] += 10 - hole_wrk.at_ground[leaf_no] / 8;

        hole_wrk.at_ground[leaf_no] += 2;
    }
    else
    {
        hole_gus[leaf_no][2] += 2.0f;
    }

    if (hole_wrk.mpos_keep[2] + hole_wrk.dist < hole_gus[leaf_no][2])
    {
        ret_num = 1;
    }

    return ret_num;
}

int HoleGusTransY(int leaf_no)
{
    hole_gus[leaf_no][1] += hole_wrk.rgba[leaf_no][1] / 8;

    return 0;
}

int HoleGusAlpha(int leaf_no)
{
    int ret_num;

    ret_num = 0;

    if (hole_wrk.rgba[leaf_no][3] >= 82)
    {
        hole_wrk.rgba[leaf_no][3] -= 3;

        if (hole_wrk.rgba[leaf_no][3] < 82)
        {
            hole_wrk.rgba[leaf_no][3] = 80;

            ret_num = 1;
        }
    }

    return ret_num;
}

void HoleGusDraw(int leaf_no)
{
    int i;
    int w;
    int th;
    int tw;
    int bak;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0FMATRIX wlm2;
    sceVu0IVECTOR ivec[4];
    float tmp_x = 0.0f;
    float tmp_y = 0.0f;
    sceVu0FVECTOR wpos;
    sceVu0FVECTOR ppos[4] = {
        { -30.0f, +30.0f, 0.0f, 1.0f },
        { +30.0f, +30.0f, 0.0f, 1.0f },
        { -30.0f, -30.0f, 0.0f, 1.0f },
        { +30.0f, -30.0f, 0.0f, 1.0f },
    };
    u_long tex0;

    Get2PosRot(camera.p, camera.i, &tmp_x, &tmp_y);

    sceVu0UnitMatrix(wlm);
    sceVu0RotMatrixX(wlm, wlm, tmp_x);
    sceVu0RotMatrixY(wlm, wlm, tmp_y);
    sceVu0TransMatrix(wlm, wlm, hole_gus[leaf_no]);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);
    sceVu0RotTransPersN(ivec, slm, ppos, 4, 0);

    w = 0;

    for (i = 0; i < 4; i++)
    {
        if (ivec[i][0] < 0x4000 || ivec[i][0] > 0xc000)
        {
            w = 1;
        }

        if (ivec[i][1] < 0x4000 || ivec[i][1] > 0xc000)
        {
            w = 1;
        }

        if (ivec[i][2] < 0xff || ivec[i][2] > 0xffffff)
        {
            w = 1;
        }
    }

    if (w == 0)
    {
        i = 8;

        tw = effdat[i].w * 16;
        th = effdat[i].h * 16;

        tex0 = effdat[i].tex0;

        if (monochrome_mode)
        {
            if (tw / 3) // probably not tw but it doesn't seem to matter what var we use here ...
            {
                // debug code?
            }
        }

        Reserve2DPacket(0x1000);

        bak = ndpkt;

        pbuf[ndpkt++].ul128 = (u_long128)0;

        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
        pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

        pbuf[ndpkt].ul64[0] = 0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

        pbuf[ndpkt].ul64[0] = tex0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, 1, 5, 0, 0, 0);
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
        pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(0, 1, 0, 1, 0);
        pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GREATER, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL);
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 340, SCE_GIF_PACKED, 3);
        pbuf[ndpkt++].ul64[1] = 0 \
            | SCE_GS_RGBAQ << (4 * 0)
            | SCE_GS_UV    << (4 * 1)
            | SCE_GS_XYZF2 << (4 * 2);

        for (i = 0; i < 4; i++)
        {
            pbuf[ndpkt].ui32[0] = 101;
            pbuf[ndpkt].ui32[1] = 93;
            pbuf[ndpkt].ui32[2] = 94;
            pbuf[ndpkt++].ui32[3] = hole_wrk.rgba[leaf_no][3] / 8;

            pbuf[ndpkt].ui32[0] = (i & 1) * tw;
            pbuf[ndpkt].ui32[1] = (i / 2) ? th : 0;
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = ivec[i][0];
            pbuf[ndpkt].ui32[1] = ivec[i][1];
            pbuf[ndpkt].ui32[2] = ivec[i][2];
            pbuf[ndpkt++].ui32[3] = (i <= 1) ? 0x8000 : 0;
        }

        pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;
    }
}

void CallLineGusEffect(sceVu0FVECTOR mpos1, sceVu0FVECTOR mpos2, int dir, int line_num)
{
    int i;

    line_effect_switch[line_num] = 1;

    if (line_wrk[line_num].init_flg == 0)
    {
        LineGusInitAll(mpos1, mpos2, dir, line_num);

        for (i = 0; i < line_wrk[line_num].fnum_keep; i++)
        {
            LineGusSetPos(i, dir, line_num);
        }

        line_wrk[line_num].init_flg = 1;
    }
}

void StopLineGusEffect(int line_num)
{
    line_effect_switch[line_num] = 0;
    line_wrk[line_num].init_flg = 0;
}

void LineGusInit(sceVu0FVECTOR mpos, int leaf_no, int line_num)
{
    if (line_wrk[line_num].mode_keep == 1 || line_wrk[line_num].mode_keep == 2)
    {
        line_gus[line_num][leaf_no][0] = line_wrk[line_num].mpos_keep[0];
        line_gus[line_num][leaf_no][1] = line_wrk[line_num].mpos_keep[1];
        line_gus[line_num][leaf_no][2] = line_wrk[line_num].mpos_keep[2];

        line_gus[line_num][leaf_no][1] += (int)((float)line_wrk[line_num].offs[1] * VER_RAND());
        line_gus[line_num][leaf_no][2] += (int)((float)line_wrk[line_num].offs[2] * VER_RAND());

    }
    else if (line_wrk[line_num].mode_keep == 3 || line_wrk[line_num].mode_keep == 4)
    {
        line_gus[line_num][leaf_no][1] = line_wrk[line_num].mpos_keep[1];
        line_gus[line_num][leaf_no][0] = line_wrk[line_num].mpos_keep[0];
        line_gus[line_num][leaf_no][2] = line_wrk[line_num].mpos_keep[2];

        line_gus[line_num][leaf_no][0] += (int)((float)line_wrk[line_num].offs[0] * VER_RAND());
        line_gus[line_num][leaf_no][2] += (int)((float)line_wrk[line_num].offs[2] * VER_RAND());
    }
    else if (line_wrk[line_num].mode_keep == 5 || line_wrk[line_num].mode_keep == 6)
    {
        float keep2 = line_wrk[line_num].mpos_keep[2]; // not in STAB
        float keep1 = line_wrk[line_num].mpos_keep[1]; // not in STAB

        line_gus[line_num][leaf_no][1] = keep1;
        line_gus[line_num][leaf_no][2] = keep2;

        line_gus[line_num][leaf_no][1] += (int)((float)line_wrk[line_num].offs[1] * VER_RAND());
        line_gus[line_num][leaf_no][2] += (int)((float)line_wrk[line_num].offs[2] * VER_RAND());
    }

    line_gus[line_num][leaf_no][3] = 1.0f;

    line_wrk[line_num].rgba[leaf_no][3] = 0xf0;

    line_wrk[line_num].at_ground[leaf_no] = 0;
}

void LineGusInitAll(sceVu0FVECTOR mpos1, sceVu0FVECTOR mpos2, int dir, int line_num)
{
    int i;

    line_wrk[line_num].mpos_keep[3] = mpos1[3];

    if (mpos1[0] > mpos2[0])
    {
        line_wrk[line_num].offs[0] = mpos1[0] - mpos2[0];
        line_wrk[line_num].mpos_keep[0] = mpos2[0];
    }
    else
    {
        line_wrk[line_num].offs[0] = mpos2[0] - mpos1[0];
        line_wrk[line_num].mpos_keep[0] = mpos1[0];
    }

    if (mpos1[1] > mpos2[1])
    {
        line_wrk[line_num].offs[1] = mpos1[1] - mpos2[1];
        line_wrk[line_num].mpos_keep[1] = mpos2[1];
    }
    else
    {
        line_wrk[line_num].offs[1] = mpos2[1] - mpos1[1];
        line_wrk[line_num].mpos_keep[1] = mpos1[1];
    }

    if (mpos1[2] > mpos2[2])
    {
        line_wrk[line_num].offs[2] = mpos1[2] - mpos2[2];
        line_wrk[line_num].mpos_keep[2] = mpos2[2];
    }
    else
    {
        line_wrk[line_num].offs[2] = mpos2[2] - mpos1[2];
        line_wrk[line_num].mpos_keep[2] = mpos1[2];
    }

    line_wrk[line_num].area_keep = 300;
    line_wrk[line_num].dist = 0x32;
    line_wrk[line_num].fnum_keep = 100;
    line_wrk[line_num].fall_count = 0;
    line_wrk[line_num].fall_rate = 7.0f;
    line_wrk[line_num].rate_remain = 0.0f;
    line_wrk[line_num].mode_keep = dir;

    for (i = 0; i < line_wrk[line_num].fnum_keep; i++)
    {
        LineGusInit(mpos1,i,line_num);
    }
}

void LineGusSetPos(int leaf_no, int dir, int line_num)
{
    float tmp;

    line_gus[line_num][leaf_no][0] = line_wrk[line_num].mpos_keep[0];
    line_gus[line_num][leaf_no][1] = line_wrk[line_num].mpos_keep[1];
    line_gus[line_num][leaf_no][2] = line_wrk[line_num].mpos_keep[2];

    line_gus[line_num][leaf_no][0] += (int)((float)line_wrk[line_num].offs[0] * VER_RAND());
    line_gus[line_num][leaf_no][1] += (int)((float)line_wrk[line_num].offs[1] * VER_RAND());
    line_gus[line_num][leaf_no][2] += (int)((float)line_wrk[line_num].offs[2] * VER_RAND());

    if (line_wrk[line_num].mode_keep == 1)
    {
        tmp = line_gus[line_num][leaf_no][0];

        line_gus[line_num][leaf_no][0] += (int)((float)line_wrk[line_num].dist * VER_RAND());

        line_wrk[line_num].rgba[leaf_no][3] = ((line_gus[line_num][leaf_no][0] - tmp) / line_wrk[line_num].dist) * 240.0f;

        tmp = (int)(10.0f * VER_RAND()) + 1;

        if (tmp <= 5.0f)
        {
            line_wrk[line_num].rgba[leaf_no][1] += (int)(VER_RAND());
        }
        else
        {
            line_wrk[line_num].rgba[leaf_no][1] -= (int)(VER_RAND());
        }

        tmp = (int)(10.0f * VER_RAND()) + 1;

        if (tmp <= 5.0f)
        {
            line_wrk[line_num].rgba[leaf_no][2] += (int)(VER_RAND());
        }
        else
        {
            line_wrk[line_num].rgba[leaf_no][2] -= (int)(VER_RAND());
        }
    }

    if (line_wrk[line_num].mode_keep == 2)
    {
        tmp = line_gus[line_num][leaf_no][0];

        line_gus[line_num][leaf_no][0] -= (int)((float)line_wrk[line_num].dist * VER_RAND());

        line_wrk[line_num].rgba[leaf_no][3] = ((tmp - line_gus[line_num][leaf_no][0]) / line_wrk[line_num].dist) * 240.0f;

        tmp = (int)(10.0f * VER_RAND()) + 1;

        if (tmp <= 5.0f)
        {
            line_wrk[line_num].rgba[leaf_no][1] += (int)(VER_RAND());
        }
        else
        {
            line_wrk[line_num].rgba[leaf_no][1] -= (int)(VER_RAND());
        }

        tmp = (int)(10.0f * VER_RAND()) + 1;

        if (tmp <= 5.0f)
        {
            line_wrk[line_num].rgba[leaf_no][2] += (int)(VER_RAND());
        }
        else
        {
            line_wrk[line_num].rgba[leaf_no][2] -= (int)(VER_RAND());
        }
    }

    if (line_wrk[line_num].mode_keep == 3)
    {
        tmp = line_gus[line_num][leaf_no][1];

        line_gus[line_num][leaf_no][1] += (int)((float)line_wrk[line_num].dist * VER_RAND());

        line_wrk[line_num].rgba[leaf_no][3] = ((line_gus[line_num][leaf_no][1] - tmp) / line_wrk[line_num].dist) * 240.0f;

        tmp = (int)(10.0f * VER_RAND()) + 1;

        if (tmp <= 5.0f)
        {
            line_wrk[line_num].rgba[leaf_no][0] += (int)(VER_RAND());
        }
        else
        {
            line_wrk[line_num].rgba[leaf_no][0] -= (int)(VER_RAND());
        }

        tmp = (int)(10.0f * VER_RAND()) + 1;

        if (tmp <= 5.0f)
        {
            line_wrk[line_num].rgba[leaf_no][2] += (int)(VER_RAND());
        }
        else
        {
            line_wrk[line_num].rgba[leaf_no][2] -= (int)(VER_RAND());
        }
    }

    if (line_wrk[line_num].mode_keep == 4)
    {
        tmp = line_gus[line_num][leaf_no][1];

        line_gus[line_num][leaf_no][1] -= (int)((float)line_wrk[line_num].dist * VER_RAND());

        line_wrk[line_num].rgba[leaf_no][3] = ((tmp - line_gus[line_num][leaf_no][1]) / line_wrk[line_num].dist) * 240.0f;

        tmp = (int)(10.0f * VER_RAND()) + 1;

        if (tmp <= 5.0f)
        {
            line_wrk[line_num].rgba[leaf_no][0] += (int)(VER_RAND());
        }
        else
        {
            line_wrk[line_num].rgba[leaf_no][0] -= (int)(VER_RAND());
        }

        tmp = (int)(10.0f * VER_RAND()) + 1;

        if (tmp <= 5.0f)
        {
            line_wrk[line_num].rgba[leaf_no][2] += (int)(VER_RAND());
        }
        else
        {
            line_wrk[line_num].rgba[leaf_no][2] -= (int)(VER_RAND());
        }
    }

    if (line_wrk[line_num].mode_keep == 5)
    {
        tmp = line_gus[line_num][leaf_no][2];

        line_gus[line_num][leaf_no][2] -= (int)((float)line_wrk[line_num].dist * VER_RAND());

        line_wrk[line_num].rgba[leaf_no][3] = ((tmp - line_gus[line_num][leaf_no][2]) / line_wrk[line_num].dist) * 240.0f;

        tmp = (int)(10.0f * VER_RAND()) + 1;

        if (tmp <= 5.0f)
        {
            line_wrk[line_num].rgba[leaf_no][0] += (int)(VER_RAND());
        }
        else
        {
            line_wrk[line_num].rgba[leaf_no][0] -= (int)(VER_RAND());
        }

        tmp = (int)(10.0f * VER_RAND()) + 1;

        if (tmp <= 5.0f)
        {
            line_wrk[line_num].rgba[leaf_no][1] += (int)(VER_RAND());
        }
        else
        {
            line_wrk[line_num].rgba[leaf_no][1] -= (int)(VER_RAND());
        }
    }

    if (line_wrk[line_num].mode_keep == 6)
    {
        tmp = line_gus[line_num][leaf_no][2];

        line_gus[line_num][leaf_no][2] -= (int)((float)line_wrk[line_num].dist * VER_RAND());

        line_wrk[line_num].rgba[leaf_no][3] = ((tmp - line_gus[line_num][leaf_no][2]) / line_wrk[line_num].dist) * 240.0f;

        tmp = (int)(10.0f * VER_RAND()) + 1;

        if (tmp <= 5.0f)
        {
            line_wrk[line_num].rgba[leaf_no][0] += (int)(VER_RAND());
        }
        else
        {
            line_wrk[line_num].rgba[leaf_no][0] -= (int)(VER_RAND());
        }

        tmp = (int)(10.0f * VER_RAND()) + 1;

        if (tmp <= 5.0f)
        {
            line_wrk[line_num].rgba[leaf_no][1] += (int)(VER_RAND());
        }
        else
        {
            line_wrk[line_num].rgba[leaf_no][1] -= (int)(VER_RAND());
        }
    }
}

void LineGusObjects(int line_num)
{
    int i;

    i = 0;

    if (line_effect_switch[line_num] != 0)
    {
        while (i < line_wrk[line_num].fnum_keep)
        {
            if (line_wrk[line_num].mode_keep == 1 || line_wrk[line_num].mode_keep == 2)
            {
                LineGusTransX(i, line_wrk[line_num].mode_keep,line_num);
            }

            if (line_wrk[line_num].mode_keep == 3 || line_wrk[line_num].mode_keep == 4)
            {
                LineGusTransY(i, line_wrk[line_num].mode_keep,line_num);
            }

            if (line_wrk[line_num].mode_keep == 5 || line_wrk[line_num].mode_keep == 6)
            {
                LineGusTransZ(i, line_wrk[line_num].mode_keep,line_num);
            }

            if (LineGusAlpha(i,line_num))
            {
                LineGusInit(line_wrk[line_num].mpos_keep,i,line_num);
            }

            i++;
        }

        for (i = 0; i < line_wrk[line_num].fnum_keep; i++)
        {
            LineGusDraw(i,line_num);
        }
    }
}

int LineGusTransX(int leaf_no, int dir, int line_num)
{
    int ret_num;

    ret_num = 0;

    if (line_wrk[line_num].mode_keep == 1)
    {
        if (1.0f < 5 - line_wrk[line_num].at_ground[leaf_no] / 8)
        {
            line_gus[line_num][leaf_no][0] += 5 - line_wrk[line_num].at_ground[leaf_no] / 8;
            line_wrk[line_num].at_ground[leaf_no] += 5;
        }
        else
        {
            line_gus[line_num][leaf_no][0] += 1.0f;
        }
    }
    else
    {
        if (1.0f < 5 - line_wrk[line_num].at_ground[leaf_no] / 8)
        {
            line_gus[line_num][leaf_no][0] -= 5 - line_wrk[line_num].at_ground[leaf_no] / 8;
            line_wrk[line_num].at_ground[leaf_no] += 5;
        }
        else
        {
            line_gus[line_num][leaf_no][0] -= 1.0f;
        }
    }

    if (line_wrk[line_num].mpos_keep[0] + line_wrk[line_num].dist < line_gus[line_num][leaf_no][0])
    {
        ret_num = 1;
    }

    return ret_num;
}

int LineGusTransZ(int leaf_no, int dir, int line_num)
{
    int ret_num;

    ret_num = 0;

    if (line_wrk[line_num].mode_keep == 5)
    {
        if (1.0f < 5 - line_wrk[line_num].at_ground[leaf_no] / 8)
        {
            line_gus[line_num][leaf_no][2] += 5 - line_wrk[line_num].at_ground[leaf_no] / 8;
            line_wrk[line_num].at_ground[leaf_no] += 5;
        }
        else
        {
            line_gus[line_num][leaf_no][2] += 1.0f;
        }
    }
    else
    {
        if (1.0f < 5 - line_wrk[line_num].at_ground[leaf_no] / 8)
        {
            line_gus[line_num][leaf_no][2] -= 5 - line_wrk[line_num].at_ground[leaf_no] / 8;
            line_wrk[line_num].at_ground[leaf_no] += 5;
        }
        else
        {
            line_gus[line_num][leaf_no][2] -= 1.0f;
        }
    }

    if (line_wrk[line_num].mpos_keep[2] + line_wrk[line_num].dist < line_gus[line_num][leaf_no][2])
    {
        ret_num = 1;
    }

    return ret_num;
}

int LineGusTransY(int leaf_no, int dir, int line_num)
{
    int ret_num;

    ret_num = 0;

    if (line_wrk[line_num].mode_keep == 3)
    {
        if (1.0f < 5 - line_wrk[line_num].at_ground[leaf_no] / 8)
        {
            line_gus[line_num][leaf_no][1] += 5 - line_wrk[line_num].at_ground[leaf_no] / 8;
            line_wrk[line_num].at_ground[leaf_no] += 5;
        }
        else
        {
            line_gus[line_num][leaf_no][1] += 1.0f;
        }
    }
    else
    {
        if (1.0f < 5 - line_wrk[line_num].at_ground[leaf_no] / 8)
        {
            line_gus[line_num][leaf_no][1] -= 5 - line_wrk[line_num].at_ground[leaf_no] / 8;
            line_wrk[line_num].at_ground[leaf_no] += 5;
        }
        else
        {
            line_gus[line_num][leaf_no][1] -= 1.0f;
        }
    }

    if (line_wrk[line_num].mpos_keep[1] + line_wrk[line_num].dist < line_gus[line_num][leaf_no][1])
    {
        ret_num = 1;
    }

    line_gus[line_num][leaf_no][0] += line_wrk[line_num].rgba[leaf_no][0];
    line_gus[line_num][leaf_no][2] += line_wrk[line_num].rgba[leaf_no][2];

    return ret_num;
}

int LineGusAlpha(int leaf_no, int line_num)
{
    int ret_num;

    ret_num = 0;

    if (line_wrk[line_num].rgba[leaf_no][3] >= 12)
    {
        line_wrk[line_num].rgba[leaf_no][3] -= 3;

        if (line_wrk[line_num].rgba[leaf_no][3] < 12)
        {
            ret_num = 1;
            line_wrk[line_num].rgba[leaf_no][3] = 8;
        }
    }
    else
    {
        if (line_wrk[line_num].rgba[leaf_no][3] < 11)
        {
            ret_num = 1;
            line_wrk[line_num].rgba[leaf_no][3] = 8;
        }
    }

    return ret_num;
}

void LineGusDraw(int leaf_no, int line_num)
{
    int i;
    int w;
    int th;
    int tw;
    int bak;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0FMATRIX wlm2;
    sceVu0IVECTOR ivec[4];
    float tmp_x = 0.0f;
    float tmp_y = 0.0f;
    sceVu0FVECTOR wpos;
    sceVu0FVECTOR ppos[4] = {
        { -30.0f, +30.0f, 0.0f, 1.0f },
        { +30.0f, +30.0f, 0.0f, 1.0f },
        { -30.0f, -30.0f, 0.0f, 1.0f },
        { +30.0f, -30.0f, 0.0f, 1.0f },
    };
    u_long tex0;

    Get2PosRot(camera.p, camera.i, &tmp_x, &tmp_y);

    sceVu0UnitMatrix(wlm);
    sceVu0RotMatrixX(wlm, wlm, tmp_x);
    sceVu0RotMatrixY(wlm, wlm, tmp_y);
    sceVu0TransMatrix(wlm, wlm, line_gus[line_num][leaf_no]);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);
    sceVu0RotTransPersN(ivec, slm, ppos, 4, 0);

    w = 0;

    for (i = 0; i < 4; i++)
    {
        if (ivec[i][0] < 0x4000 || ivec[i][0] > 0xc000)
        {
            w = 1;
        }

        if (ivec[i][1] < 0x4000 || ivec[i][1] > 0xc000)
        {
            w = 1;
        }

        if (ivec[i][2] < 0xff || ivec[i][2] > 0xffffff)
        {
            w = 1;
        }
    }

    if (w == 0)
    {
        i = 8;

        tw = effdat[i].w * 16;
        th = effdat[i].h * 16;

        tex0 = effdat[i].tex0;

        if (monochrome_mode)
        {
            if (tw / 3) // probably not tw but it doesn't seem to matter what var we use here ...
            {
                // debug code?
            }
        }

        Reserve2DPacket(0x1000);

        bak = ndpkt;

        pbuf[ndpkt++].ul128 = (u_long128)0;

        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
        pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

        pbuf[ndpkt].ul64[0] = 0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

        pbuf[ndpkt].ul64[0] = tex0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
        pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
        pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GREATER, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL);
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 340, SCE_GIF_PACKED, 3);
        pbuf[ndpkt++].ul64[1] = 0 \
            | SCE_GS_RGBAQ << (4 * 0)
            | SCE_GS_UV    << (4 * 1)
            | SCE_GS_XYZF2 << (4 * 2);

        for (i = 0; i < 4; i++)
        {
            pbuf[ndpkt].ui32[0] = 0x51;
            pbuf[ndpkt].ui32[1] = 0x49;
            pbuf[ndpkt].ui32[2] = 0x4a;
            pbuf[ndpkt++].ui32[3] = line_wrk[line_num].rgba[leaf_no][3] / 8;

            pbuf[ndpkt].ui32[0] = (i & 1) * tw;
            pbuf[ndpkt].ui32[1] = (i / 2) ? th : 0;
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = ivec[i][0];
            pbuf[ndpkt].ui32[1] = ivec[i][1];
            pbuf[ndpkt].ui32[2] = ivec[i][2];
            pbuf[ndpkt++].ui32[3] = (i <= 1) ? 0x8000 : 0;
        }

        pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;
    }
}

void InitCallAnm()
{
    anm_no_keep = -1;
    anm_load_id = 0;
    anm_init = 0;

    clear_end = 0;
    clear_end2 = 0;
    clear_end3 = 0;

    banm.mode = BANM_NORM;
    banm.load_id = 0;
    banm.tex_keep = -1; // 0xffffffffffffffff;
    banm.tm2_id = -1;
}

int CallReadyGo()
{
    short int ret_num;

    ret_num = 0;

    if (ingame_wrk.stts & 0x20)
    {
        DrawScreen(0x7f000, 0x1a40, 0x80, 0x80, 0x80, 0x80);
    }

    switch(anm_init) {
    case 0:
        plyr_wrk.sta |= 0x2;

        AdpcmPreBtlmodeFadeOut(0);

        anm_init = 1;
    break;
    case 1:
        if (IsEndAdpcmBtlmode() != 0)
        {
            anm_init = 2;
        }
    break;
    case 2:
        if (BtlAnmInit(0) != 0)
        {
            AdpcmPreBtlmodePreload(AB030_STR);

            anm_init = 3;
        }
    break;
    case 3:
        if (IsPreLoadEndAdpcmBtlmode() != 0)
        {
            anm_init = 4;
        }
    break;
    case 4:
        AdpcmBtlmodePreLoadEndPlay();

        anm_init = 5;

        ingame_wrk.stts |= 0x20;

        CaptureScreen(0x1a40);
    // case fall-through
    case 5:
        if (BtlAnmMain() != 0)
        {
            anm_init = 0;

            plyr_wrk.sta &= ~0x2;

            ingame_wrk.stts &= ~0x20;

            ret_num = 1;
        }
    break;
    }

    return ret_num;
}

void ResultDisp()
{
    u_long clear_time;

    PutStringYW(IGMSG_BTL_MSON, 13, 118, 163, 0x808080, 0x80, 0x23000, 0);
    PutStringYW(IGMSG_BTL_MSON, 14, 218, 163, 0x808080, 0x80, 0x23000, 0);
    PutStringYW(IGMSG_BTL_MSON, 15, 118, 203, 0x808080, 0x80, 0x23000, 0);
    PutStringYW(IGMSG_BTL_MSON, 16, 203, 203, 0x808080, 0x80, 0x23000, 0);
    PutStringYW(IGMSG_BTL_MSON, 17, 118, 243, 0x808080, 0x80, 0x23000, 0);
    PutStringYW(IGMSG_BTL_MSON, 18, 218, 243, 0x808080, 0x80, 0x23000, 0);
    PutStringYW(IGMSG_BTL_MSON, 19, 118, 303, 0x808080, 0x80, 0x23000, 0);
#ifdef BUILD_EU_VERSION
    PutStringYW(IGMSG_BTL_MSON, 40, 118, 333, 0x808080, 0x80, 0x23000, 0);
#else
    PutStringYW(IGMSG_BTL_MSON, 17, 118, 333, 0x808080, 0x80, 0x23000, 0);
#endif

    clear_time = stage_dat[btl_wrk.stage_no].time - btl_wrk.time;

    PutNumberYW(3, clear_time / 3600, 429, 163, 1.0f, 1.0f, 0x808080, 0x80, 0x23000, 1, 1);
    PutStringYW(IGMSG_BTL_MSON, 11, 442, 163, 0x808080, 0x80, 0x23000, 0);
    PutNumberYW(3, (clear_time / 60) % 60, 456, 163, 1.0f, 1.0f, 0x808080, 0x80, 0x23000, 2, 1);
    PutStringYW(IGMSG_BTL_MSON, 11, 487, 163, 0x808080, 0x80, 0x23000, 0);
    PutNumberYW(3, (clear_time * 100 / 60) % 100, 499, 163, 1.0f, 1.0f, 0x808080, 0x80, 0x23000, 2, 1);
    PutNumberYW(3, msn_high_score, 428, 203, 1.0f, 1.0f, 0x808080, 0x80, 0x23000, 4, 0);
    PutStringYW(IGMSG_BTL_MSON, 12, 493, 203, 0x808080, 0x80, 0x23000, 0);
    PutNumberYW(3, msn_total_score, 364, 243, 1.0f, 1.0f, 0x808080, 0x80, 0x23000, 8, 0);
    PutStringYW(IGMSG_BTL_MSON, 12, 493, 243, 0x808080, 0x80, 0x23000, 0);
    PutNumberYW(3, msn_bonus, 364, 303, 1.0f, 1.0f, 0x808080, 0x80, 0x23000, 8, 0);
    PutStringYW(IGMSG_BTL_MSON, 12, 493, 303, 0x808080, 0x80, 0x23000, 0);
    PutNumberYW(3, msn_bonus + msn_total_score, 364, 333, 1.0f, 1.0f, 0x808080, 0x80, 0x23000, 8, 0);
    PutStringYW(IGMSG_BTL_MSON, 12, 493, 333, 0x808080, 0x80, 0x23000, 0);
}

int CallMissionClear()
{
    static u_char now_pos = 0;
    u_char alpha_res[3] = { 30, 30, 30 };
    int ret_num;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    STR_DAT sd;
    DISP_STR dsr;
#endif

    ret_num = 0;

    switch(anm_init)
    {
    case 0:
        plyr_wrk.sta |= 0x2;

        btl_wrk.mode = BTL_MODE_CLEAR;

        if (plyr_wrk.mode == 1)
        {
            FinderEndSet();

            plyr_wrk.mode = PMODE_FINDER_END;
        }

        AdpcmPreBtlmodeFadeOut(0);

        SeFadeFlameAll(30, 0);

        anm_init = 1;
    break;
    case 1:
        if (IsEndAdpcmBtlmode() != 0)
        {
            anm_init = 2;
        }
    break;
    case 2:
        if (BtlAnmInit(2) != 0)
        {
            anm_init = 3;

            clear_anime_timer = 180;

            AdpcmBtlmodePlay(AB028_STR);
        }
    break;
    case 3:
        if (clear_anime_timer != 0)
        {
            clear_anime_timer--;
        }

        if (BtlAnmMain() || clear_anime_timer == 0)
        {
            clear_anime_timer = 0;

            anm_init = 4;
        }
    break;
    case 4:
        if (BtlAnmInit(4) != 0)
        {
            anm_init = 5;

            ChangeMonochrome(1);
        }
    break;
    case 5:
        ResultDisp();

        VER_POLY_SQUARE_YW(0.0f, 0.0f, 640, 448, 0x00000000, 60.0f, 1.0f, 1.0f, 0x24000, 0, 0);

        if (BtlAnmMain() != 0)
        {
            anm_init = 6;

            AdpcmPreBtlmodeFadeOut(0);

            break;
        }

        if (PAD_BTN_REPEAT(PAD_DPAD_RIGHT) || PAD_LANA_REPEAT(PAD_LANA_RIGHT))
        {
            now_pos = now_pos != 2 ? now_pos + 1 : 0;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }

        if (PAD_BTN_REPEAT(PAD_DPAD_LEFT) || PAD_LANA_REPEAT(PAD_LANA_LEFT))
        {
            now_pos = now_pos != 0 ? now_pos - 1 : 2;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }

        alpha_res[now_pos] = 80;

        DispSprt2(&btl_rslt[10], LOAD_ADDRESS_41, 1, NULL, NULL, alpha_res[0]);
        DispSprt2(&btl_rslt[11], LOAD_ADDRESS_41, 1, NULL, NULL, alpha_res[1]);
        DispSprt2(&btl_rslt[12], LOAD_ADDRESS_41, 1, NULL, NULL, alpha_res[2]);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        if (btl_clear_disp == 2)
        {
            sd.r = 0x80;
            sd.g = 0x80;
            sd.b = 0x80;

            sd.alpha = 0x80;

            sd.pri = 0x1000;

            sd.str = (u_char *)GetIngameMSGAddr(IGMSG_MENU_MSG, 43);

            sd.pos_x = 88;
            sd.pos_y = 320;

            sd.type = 0;

            CopyStrDToStr(&dsr, &sd);
            SetMessageV2(&dsr);

            DrawMessageBox(0xa000, 64.0f, 302.0f, 512.0f, 60.0f, 0x80);
        }
#endif
    break;
    case 6:
        if (IsEndAdpcmBtlmode() != 0)
        {
            plyr_wrk.sta &= ~0x2;

            anm_init = 0;

            ret_num = now_pos + 1;

            AdpcmMapCtrlInit();
        }
    break;
    }

    return ret_num;
}

int CallMissionFailed()
{
    static u_char now_pos = 0;
    u_char alpha_res[3] = { 30, 30, 30 };
    int ret_num;

    ret_num = 0;

    switch(anm_init)
    {
    case 0:
        plyr_wrk.sta |= 0x2;

        btl_wrk.mode = BTL_MODE_FAILED;

#if defined(BUILD_JP_VERSION)
        if (plyr_wrk.mode == PMODE_FINDER)
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        if (plyr_wrk.mode == PMODE_FINDER || plyr_wrk.mode == PMODE_FINDER_IN)
#endif
        {
            FinderEndSet();

            plyr_wrk.mode = PMODE_FINDER_END;
        }

        AdpcmPreBtlmodeFadeOut(0);

        SeFadeFlameAll(30, 0);

        anm_init = 1;
    break;
    case 1:
        if (IsEndAdpcmBtlmode() != 0)
        {
            anm_init = 2;

#if defined(BUILD_EU_VERSION)
            wrk_table[18].sdat_p = &btl_msel2[sys_wrk.language * 2 + 25];
            wrk_table[19].sdat_p = &btl_msel2[sys_wrk.language * 2 + 26];
#endif
        }
    break;
    case 2:
        if (BtlAnmInit(1) != 0)
        {
            anm_init = 3;

            clear_anime_timer = 180;

            AdpcmBtlmodePlay(AB029_STR);
        }
    break;
    case 3:
        if (clear_anime_timer != 0)
        {
            clear_anime_timer--;
        }

        if (BtlAnmMain() != 0 || clear_anime_timer == 0)
        {
            clear_anime_timer = 0;

            anm_init = 4;
        }
    break;
    case 4:
        if (BtlAnmInit(4) != 0)
        {
            anm_init = 5;

            ChangeMonochrome(1);
        }
    break;
    case 5:
        ResultDisp();

        VER_POLY_SQUARE_YW(0.0f, 0.0f, 640, 448, 0x00000000, 60.0f, 1.0f, 1.0f, 0x24000, 0, 0);

        if (BtlAnmMain() != 0)
        {
            anm_init = 6;

            AdpcmPreBtlmodeFadeOut(0);

            break;
        }

        if (PAD_BTN_REPEAT(PAD_DPAD_RIGHT) || PAD_LANA_REPEAT(PAD_LANA_RIGHT))
        {
            now_pos = now_pos != 2 ? now_pos + 1 : 0;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }

        if (PAD_BTN_REPEAT(PAD_DPAD_LEFT) || PAD_LANA_REPEAT(PAD_LANA_LEFT))
        {
            now_pos = now_pos != 0 ? now_pos - 1 : 2;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }

        alpha_res[now_pos] = 100;

        DispSprt2(&btl_rslt[10], LOAD_ADDRESS_41, 1, NULL, NULL, alpha_res[0]);
        DispSprt2(&btl_rslt[11], LOAD_ADDRESS_41, 1, NULL, NULL, alpha_res[1]);
        DispSprt2(&btl_rslt[12], LOAD_ADDRESS_41, 1, NULL, NULL, alpha_res[2]);
    break;
    case 6:
        if (IsEndAdpcmBtlmode() != 0)
        {
            plyr_wrk.sta &= ~0x2;

            anm_init = 0;

            ret_num = now_pos + 1;

            AdpcmMapCtrlInit();
        }
    break;
    }

    return ret_num;
}

int CallMissionAllClear()
{
    static u_char now_pos = 0;
    u_char alpha_res[3] = { 30, 30, 30 };
    int ret_num;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    STR_DAT sd;
#endif
    SPRT_DAT ssd;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    DISP_STR dsr;
#endif
    DISP_SPRT ds;
    int i;

    ret_num = 0;

    switch(anm_init)
    {
    case 0:
        plyr_wrk.sta |= 0x2;

        btl_wrk.mode = BTL_MODE_CLEAR;

        if (plyr_wrk.mode == PMODE_FINDER)
        {
            FinderEndSet();

            plyr_wrk.mode = PMODE_FINDER_END;
        }

        AdpcmPreBtlmodeFadeOut(0);

        SeFadeFlameAll(30, 0);

        anm_init = 1;
    break;
    case 1:
        if (IsEndAdpcmBtlmode() != 0)
        {
            anm_init = 2;
        }
    break;
    case 2:
        if (BtlAnmInit(2) != 0)
        {
            anm_init = 3;

            clear_anime_timer = 180;

            AdpcmBtlmodePlay(AB028_STR);
        }
    break;
    case 3:
        if (clear_anime_timer != 0)
        {
            clear_anime_timer--;
        }

        if (BtlAnmMain() != 0 || clear_anime_timer == 0)
        {
            clear_anime_timer = 0;

            anm_init = 4;
        }
    break;
    case 4:
        if (BtlAnmInit(4))
        {
            anm_init = 5;

            ChangeMonochrome(1);
        }
    break;
    case 5:
        ResultDisp();

        VER_POLY_SQUARE_YW(0.0f, 0.0f, 640, 448, 0, 60.0f, 1.0f, 1.0f, 0x24000, 0, 0);

        if (BtlAnmMain() != 0)
        {
            anm_init = 7;
        }
    break;
    case 7:
        if (BtlAnmInit(3) != 0)
        {
            anm_init = 8;

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            clear_anime_timer = 120;
#endif
        }
    break;
    case 8:
        SetSprFile(LOAD_ADDRESS_43);

        for (i = 0; i < 11; i++)
        {
            ssd = clear_all[i];

            CopySprDToSpr(&ds, &ssd);
            DispSprD(&ds);
        }


        if (BtlAnmMain() != 0)
        {
            anm_init = 6;

            AdpcmPreBtlmodeFadeOut(0);

            break;
        }

        if (PAD_BTN_REPEAT(PAD_DPAD_RIGHT) || PAD_LANA_REPEAT(PAD_LANA_RIGHT))
        {
            now_pos = now_pos == 0;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }

        if (PAD_BTN_REPEAT(PAD_DPAD_LEFT) || PAD_LANA_REPEAT(PAD_LANA_LEFT))
        {
            now_pos = now_pos == 0;

            SeStartFix(0, 0, 0x1000, 0x1000, 0);
        }

        alpha_res[now_pos] = 100;

        DispSprt2(&btl_rslt[11], LOAD_ADDRESS_41, 1, NULL, NULL, alpha_res[0]);
        DispSprt2(&btl_rslt[12], LOAD_ADDRESS_41, 1, NULL, NULL, alpha_res[1]);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        if (clear_anime_timer != 0)
        {
            clear_anime_timer--;
        }
        else if (btl_clear_disp)
        {
            sd.r = 0x80;
            sd.g = 0x80;
            sd.b = 0x80;

            sd.alpha = 0x80;

            sd.pri = 0x1000;

            sd.str = (u_char *)GetIngameMSGAddr(IGMSG_MENU_MSG, 42);

#if defined(BUILD_US_VERSION)
            sd.pos_x = 88;
            sd.pos_y = 284;
#elif defined(BUILD_EU_VERSION)
            sd.pos_x = 46;
            sd.pos_y = 274;
#endif

            sd.type = 0;

            CopyStrDToStr(&dsr, &sd);
            SetMessageV2(&dsr);

#if defined(BUILD_US_VERSION)
            DrawMessageBox(0xa000, 64.0f, 262.0f, 512.0f, 88.0f, 0x80);
#elif defined(BUILD_EU_VERSION)
            DrawMessageBox(0xa000, 19.0f, 262.0f, 592.0f, 96.0f, 0x80);
#endif
        }
#endif
    break;
    case 6:
        if (IsEndAdpcmBtlmode() != 0)
        {
            AdpcmMapCtrlInit();

            plyr_wrk.sta &= ~0x2;

            clear_end = 0;
            clear_end2 = 0;
            clear_end3 = 0;

            anm_init = 0;

            ret_num = now_pos + 1;
        }
    break;
    }

    return ret_num;
}

int CallStoryClear()
{
    short int ret_num;
    static u_long clear_time = 0;
    static u_int t_point = 0;
    SPRT_DAT ssd;
    DISP_SPRT ds;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    DISP_STR dsr;
    STR_DAT sd;
#endif
    int i;

    ret_num = 0;

    for (i = 0; i < 11; i++)
    {
        ssd = story_end[i];

        CopySprDToSpr(&ds, &ssd);

        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

        DispSprD(&ds);
    }

    if (anm_init == 0)
    {
        if (BtlAnmInit(5) == 0)
        {
            return 0;
        }

        AdpcmMapCtrlInit();

        EAdpcmCmdPlay(0, 1, AB027_STR, 0, GetAdpcmVol(AB027_STR), 640, 0xfff, 0);

        anm_init = 1;

        clear_time = time_wrk.one_game;

        t_point = ingame_wrk.total_point;
    }

    if (first_clear_flg == 0 && (cribo.clear_info & 2) == 0)
    {
        SetSquareS(0x18000, -320.0f, -224.0f, 320.0f, 224.0f, 0, 0, 0, 1);
    }

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    if (clear_disp != 0)
    {
        sd.r = 0x80;
        sd.g = 0x80;
        sd.b = 0x80;

        sd.alpha = 0x80;

        sd.pri = 0x1000;

        if (clear_disp_mode == 1)
        {
            sd.str = (u_char *)GetIngameMSGAddr(IGMSG_MENU_MSG, 41);

#if defined(BUILD_US_VERSION)
            sd.pos_x = 88;
            sd.pos_y = 170;
#elif defined(BUILD_EU_VERSION)
            sd.pos_x = 72;
            sd.pos_y = 170;
#endif
            sd.type = 0;

            CopyStrDToStr(&dsr, &sd);
            SetMessageV2(&dsr);

#if defined(BUILD_US_VERSION)
            DrawMessageBox(0xa000, 64.0f, 152.0f, 512.0f, 128.0f, 0x80);
#elif defined(BUILD_EU_VERSION)
            DrawMessageBox(0xa000, 32.0f, 152.0f, 576.0f, 128.0f, 0x80);
#endif
        }

        if (clear_disp_mode == 2)
        {
            sd.str = (u_char *)GetIngameMSGAddr(IGMSG_MENU_MSG, 44);

#if defined(BUILD_US_VERSION)
            sd.pos_x = 88;
            sd.pos_y = 182;
#elif defined(BUILD_EU_VERSION)
            sd.pos_x = 72;
            sd.pos_y = 182;
#endif

            sd.type = 0;

            CopyStrDToStr(&dsr, &sd);
            SetMessageV2(&dsr);

#if defined(BUILD_US_VERSION)
            DrawMessageBox(0xa000, 64.0f, 152.0f, 512.0f, 128.0f, 0x80);
#elif defined(BUILD_EU_VERSION)
            DrawMessageBox(0xa000, 32.0f, 152.0f, 576.0f, 128.0f, 0x80);
#endif
        }

        if (clear_disp_mode == 3)
        {
            sd.str = (u_char *)GetIngameMSGAddr(IGMSG_MENU_MSG, 45);

#if defined(BUILD_US_VERSION)
            sd.pos_x = 110;
            sd.pos_y = 205;
#elif defined(BUILD_EU_VERSION)
            sd.pos_x = 94;
            sd.pos_y = 205;
#endif
            sd.type = 0;

            CopyStrDToStr(&dsr, &sd);
            SetMessageV2(&dsr);

#if defined(BUILD_US_VERSION)
            DrawMessageBox(0xa000, 64.0f, 172.0f, 512.0f, 88.0f, 0x80);
#elif defined(BUILD_EU_VERSION)
            DrawMessageBox(0xa000, 32.0f, 172.0f, 576.0f, 88.0f, 0x80);
#endif
        }

        if (PAD_BTN_PRESSED(PAD_CROSS))
        {
            ret_num = 1;

            clear_disp_mode = 0;
            anm_init = 0;

            banm.tex_keep = 0;
            banm.mode = BANM_NORM;
            banm.tm2_id = -1;
            banm.load_id = -1;
        }
    }
    else
    {
#endif
        if (BtlAnmMain() != 0)
        {
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            if (clear_disp_mode != 0)
            {
                clear_disp = 1;
            }
            else
            {
#endif
                ret_num = 1;

                anm_init = 0;

                banm.tex_keep = 0;
                banm.mode = BANM_NORM;
                banm.tm2_id = -1;
                banm.load_id = -1;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            }
#endif
        }
        else
        {
            PutStringYW(IGMSG_BTL_MSON, 13, 118, 123, 0x808080, 0x80, 0x18000, 0);
            PutStringYW(IGMSG_BTL_MSON, 14, 218, 123, 0x808080, 0x80, 0x18000, 0);

            PutNumberYW(3, clear_time / (3600 * 60), 413, 123, 1.0f, 1.0f, 0x808080, 0x80, 0x18000, 2, 1);
            PutStringYW(IGMSG_BTL_MSON, 11, 442, 123, 0x808080, 0x80, 0x18000, 0);

            PutNumberYW(3, (clear_time / 3600) % 60, 456, 123, 1.0f, 1.0f, 0x808080, 0x80, 0x18000, 2, 1);
            PutStringYW(IGMSG_BTL_MSON, 11, 487, 123, 0x808080, 0x80, 0x18000, 0);

            PutNumberYW(3, (clear_time / 60) % 60, 499, 123, 1.0f, 1.0f, 0x808080, 0x80, 0x18000, 2, 1);
            PutStringYW(IGMSG_BTL_MSON, 17, 118, 163, 0x808080, 0x80, 0x18000, 0);
            PutStringYW(IGMSG_BTL_MSON, 18, 218, 163, 0x808080, 0x80, 0x18000, 0);

            PutNumberYW(3, t_point, 364, 163, 1.0f, 1.0f, 0x808080, 0x80, 0x18000, 8, 0);
            PutStringYW(IGMSG_BTL_MSON, 12, 493, 163, 0x808080, 0x80, 0x18000, 0);
        }
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    }
#endif

    return ret_num;
}

int BtlAnmInit(int anm_no)
{
    int i;
    int j;
    int story_rank;

    j = i = 0;

    anm_no_keep = anm_no;

    anm_wrk.start_point = wrk_table;

    while (j <= anm_no)
    {
        if (i != 0)
        {
            i++;
        }

        anm_wrk.start_num = i;

        while (ANM2D_WRK_TABLE_P(anm_wrk.start_point)[i].sdat_p != NULL)
        {
            i++;
        }

        j++;
    }

    anm_wrk.end_num = i;

    if (anm_load_id == 0)
    {
        switch (anm_no)
        {
        case 0:
            VER_LOAD_REQ_LANG(TX_BTL_DMY_PK2, LOAD_ADDRESS_41);

            anm_load_id = 1;
        break;
        case 1:
        case 2:
            VER_LOAD_REQ_LANG(M_SLCT_STY_DMY_PK2, LOAD_ADDRESS_41);

            anm_load_id = 1;
        break;
        case 3:
            VER_LOAD_REQ_LANG(TX_BTL_DMY_PK2, LOAD_ADDRESS_41);
            LoadReq(TX_BTL_RES_PK2, LOAD_ADDRESS_43);

            anm_load_id = 1;
        break;
        case 4:
            VER_LOAD_REQ_LANG(M_SLCT_BTL_CHR_PK2, LOAD_ADDRESS_41);

            anm_load_id = 1;
        break;
        case 5:
            VER_LOAD_REQ_LANG(M_SLCT_STY_DMY_PK2, LOAD_ADDRESS_41);
            LoadReq(S_MODE_CLEARBG_PK2, LOAD_ADDRESS_23);

            anm_load_id = 1;
        break;
        default:
            VER_LOAD_REQ_LANG(TX_BTL_DMY_PK2, LOAD_ADDRESS_41);

            anm_load_id = 1;
        break;
        }
    }

    if (IsLoadEndAll() != 0)
    {
        switch (anm_no)
        {
        case 3:
            SetSprFile(LOAD_ADDRESS_41);
            SetSprFile(LOAD_ADDRESS_43);
        break;
        case 5:
            SetSprFile(LOAD_ADDRESS_41);
            SetSprFile(LOAD_ADDRESS_23);
        break;
        default:
            SetSprFile(LOAD_ADDRESS_41);
        break;
        }

        anm_load_id = 0;
    }
    else
    {
        return 0;
    }

    for (i = anm_wrk.start_num; i < anm_wrk.end_num; i++)
    {
        wrk_table[i].table_p = anm2d_dat[wrk_table[i].anm_index];

        if (ANM2D_DAT_TABLE_P(wrk_table[i].table_p)->attribute & 0x200)
        {
            wrk_table[i].sdat[0] = wrk_table[i].sdat_p[(btl_wrk.stage_no + 1) % 10];
            wrk_table[i].sdat[0].x += 20;
        }
        else if (ANM2D_DAT_TABLE_P(wrk_table[i].table_p)->attribute & 0x400)
        {
            wrk_table[i].sdat[0] = wrk_table[i].sdat_p[(btl_wrk.stage_no + 1) / 10];
        }
        else if (ANM2D_DAT_TABLE_P(wrk_table[i].table_p)->attribute & 0x800)
        {
            if (time_wrk.one_game < 3600 * 210)
            {
                story_rank = 0;
            }
            else if (time_wrk.one_game < 3600 * 240)
            {
                story_rank = 1;
            }
            else if (time_wrk.one_game < 3600 * 300)
            {
                story_rank = 2;
            }
            else if (time_wrk.one_game < 3600 * 420)
            {
                story_rank = 3;
            }
            else if (time_wrk.one_game < 3600 * 600)
            {
                story_rank = 4;
            }
            else
            {
                story_rank = 5;
            }

            wrk_table[i].sdat[0] = wrk_table[i].sdat_p[story_rank];
        }
        else
        {
            wrk_table[i].sdat[0] = wrk_table[i].sdat_p[0];
        }

        wrk_table[i].counter = anm2d_dat[wrk_table[i].anm_index][0].counter;

        wrk_table[i].sscl[0].cx = wrk_table[i].sdat[0].x + wrk_table[i].sdat[0].w / 2;
        wrk_table[i].sscl[0].cy = wrk_table[i].sdat[0].y + wrk_table[i].sdat[0].h / 2;
        wrk_table[i].sscl[0].sw = anm2d_dat[wrk_table[i].anm_index][0].x_scale;
        wrk_table[i].sscl[0].sh = anm2d_dat[wrk_table[i].anm_index][0].y_scale;

        wrk_table[i].srot[0].cx = wrk_table[i].sdat[0].x + wrk_table[i].sdat[0].w / 2;
        wrk_table[i].srot[0].cy = wrk_table[i].sdat[0].y + wrk_table[i].sdat[0].h / 2;
        wrk_table[i].srot[0].rot = anm2d_dat[wrk_table[i].anm_index][0].rotation;

        wrk_table[i].sdat[0].alp = anm2d_dat[wrk_table[i].anm_index][0].alpha;

        for (j = 1; j < 8; j++)
        {
            wrk_table[i].sdat[j].u = 0x7fff;
            wrk_table[i].srot[j].cx = 0x7fff;
            wrk_table[i].sscl[j].cy = 0x7fff;
        }
    }

    return 1;
}

int BtlAnmMain()
{
    int i;

    for (i = anm_wrk.start_num; i < anm_wrk.end_num; i++)
    {
        if (ANM2D_DAT_TABLE_P(wrk_table[i].table_p)->attribute & 0x80000)
        {
            return 1;
        }

#if defined(BUILD_JP_VERSION)
        if (ANM2D_DAT_TABLE_P(wrk_table[i].table_p)->attribute & 0x100000 && (PAD_BTN_PRESSED(PAD_CIRCLE) || PAD_BTN_PRESSED(PAD_SQUARE)))
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        if (ANM2D_DAT_TABLE_P(wrk_table[i].table_p)->attribute & 0x100000 && PAD_BTN_PRESSED(PAD_CROSS))
#endif
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);

            return 1;
        }

        if (ANM2D_DAT_TABLE_P(wrk_table[i].table_p)->attribute & 0x8000)
        {
            BtlDataDirectSet(&wrk_table[i]);
        }

        if ((ANM2D_DAT_TABLE_P(wrk_table[i].table_p)->attribute & 0x48000) == 0)
        {
            BtlTblTransX(&wrk_table[i], anm2d_dat[i], wrk_table[i].sdat);
            BtlTblTransY(&wrk_table[i], anm2d_dat[i], wrk_table[i].sdat);
            BtlTblRotate(&wrk_table[i], wrk_table[i].sdat, wrk_table[i].table_p);
            BtlTblScale(&wrk_table[i], wrk_table[i].sdat, wrk_table[i].table_p);

            TblAlphaChg(&wrk_table[i], wrk_table[i].sdat, wrk_table[i].table_p);

            ZanzouEffect(&wrk_table[i], wrk_table[i].sdat, wrk_table[i].table_p);

            wrk_table[i].table_p = BtlTblIncl(anm2d_dat[wrk_table[i].anm_index],wrk_table[i].table_p, &wrk_table[i].counter);

            if ((ANM2D_DAT_TABLE_P(wrk_table[i].table_p)->attribute & 0x10000) == 0)
            {
                if ((ANM2D_DAT_TABLE_P(wrk_table[i].table_p)->attribute & 0x10) == 0)
                {
                    BtlReadyDisp(&wrk_table[i]);
                }
                else
                {
                    DrawMessageBox(0x2000, wrk_table[i].sdat[0].x, wrk_table[i].sdat[0].y, 592.0f, 96.0f, wrk_table[i].sdat[0].alp);
                }
            }
        }
    }

    return 0;
}

void *BtlTblIncl(ANM2D_DAT_TABLE *b_table, void *p_table, short int *count_keep)
{
    int i;

    i = 0;

    if (ANM2D_DAT_TABLE_P(p_table)->attribute & 0x20000)
    {
        return p_table;
    }

    if (*count_keep < 1)
    {
        if (p_table == NULL)
        {
            p_table = b_table;
        }
        else
        {
            if (ANM2D_DAT_TABLE_P(p_table)->attribute & 0x2000)
            {
                while ((b_table[i].attribute & 0x1000) == 0)
                {
                    i++;
                }

                p_table = &b_table[i];
            }
            else
            {
                p_table = &ANM2D_DAT_TABLE_P(p_table)[1];
            }
        }

        *count_keep = ANM2D_DAT_TABLE_P(p_table)->counter;
    }
    else
    {
        (*count_keep)--;
    }

    return p_table;
}

void BtlDataDirectSet(ANM2D_WRK_TABLE *w_table)
{
    w_table->sdat[0].x = w_table->sdat[0].x + ANM2D_DAT_TABLE_P(w_table->table_p)->x_trans;
    w_table->sdat[0].y = w_table->sdat[0].y + ANM2D_DAT_TABLE_P(w_table->table_p)->y_trans;

    w_table->table_p = &ANM2D_DAT_TABLE_P(w_table->table_p)[1];

    w_table->counter = ANM2D_DAT_TABLE_P(w_table->table_p)->counter;
}

void BtlTblTransX(ANM2D_WRK_TABLE *w_table, ANM2D_DAT_TABLE *b_table, SPRT_SDAT *ssd_p)
{

    if (
        w_table->counter == ANM2D_DAT_TABLE_P(w_table->table_p)->counter &&
        (ANM2D_DAT_TABLE_P(w_table->table_p)->attribute & 1) == 0
    )
    {
        w_table->x_speed = ANM2D_DAT_TABLE_P(w_table->table_p)->x_trans;
    }

    if (w_table->table_p != NULL && (ANM2D_DAT_TABLE_P(w_table->table_p)->attribute & 0x4000) == 0)
    {
        if ((ANM2D_DAT_TABLE_P(w_table->table_p)->attribute & 1) != 0)
        {
            w_table->x_speed += ANM2D_DAT_TABLE_P(w_table->table_p)->x_trans;

            ssd_p->x += w_table->x_speed >> 4;
        }
        else
        {
            w_table->x_speed += ANM2D_DAT_TABLE_P(w_table->table_p)->x_trans;

            if (ANM2D_DAT_TABLE_P(w_table->table_p)->x_trans >= 1)
            {
                while (w_table->x_speed >= 16)
                {
                    ssd_p->x++;

                    w_table->x_speed -= 16;
                }
            }
            else
            {
                while (w_table->x_speed <= -16)
                {
                    ssd_p->x--;

                    w_table->x_speed += 16;
                }
            }
        }
    }
}

void BtlTblTransY(ANM2D_WRK_TABLE *w_table, ANM2D_DAT_TABLE *b_table, SPRT_SDAT *ssd_p)
{

    if (
        w_table->counter == ANM2D_DAT_TABLE_P(w_table->table_p)->counter &&
        (ANM2D_DAT_TABLE_P(w_table->table_p)->attribute & 2) == 0
    )
    {
        w_table->y_speed = ANM2D_DAT_TABLE_P(w_table->table_p)->y_trans;
    }

    if (w_table->table_p != NULL && (ANM2D_DAT_TABLE_P(w_table->table_p)->attribute & 0x4000) == 0)
    {
        if ((ANM2D_DAT_TABLE_P(w_table->table_p)->attribute & 2) != 0)
        {
            w_table->y_speed += ANM2D_DAT_TABLE_P(w_table->table_p)->y_trans;

            ssd_p->y += w_table->y_speed >> 4;
        }
        else
        {
            w_table->y_speed += ANM2D_DAT_TABLE_P(w_table->table_p)->y_trans;

            if (ANM2D_DAT_TABLE_P(w_table->table_p)->y_trans >= 1)
            {
                while (w_table->y_speed >= 16)
                {
                    ssd_p->y++;

                    w_table->y_speed -= 16;
                }
            }
            else
            {
                while (w_table->y_speed <= -16)
                {
                    ssd_p->y--;

                    w_table->y_speed += 16;
                }
            }
        }
    }
}

void BtlTblRotate(ANM2D_WRK_TABLE *w_table, SPRT_SDAT *ssd_p, void *p_table)
{
    w_table->srot[0].cx = ssd_p->x + w_table->sdat[0].w / 2;
    w_table->srot[0].cy = ssd_p->y + w_table->sdat[0].h / 2;

    if (ANM2D_DAT_TABLE_P(p_table)->counter == w_table->counter)
    {
        w_table->rot_off = ANM2D_DAT_TABLE_P(p_table)->rotation - w_table->srot[0].rot;

        if (ANM2D_DAT_TABLE_P(p_table)->counter == w_table->counter)
        {
            return;
        }
    }

    w_table->srot[0].rot = ANM2D_DAT_TABLE_P(p_table)->rotation - (w_table->rot_off * w_table->counter) / ANM2D_DAT_TABLE_P(p_table)->counter;
}

void BtlTblScale(ANM2D_WRK_TABLE *w_table, SPRT_SDAT *ssd_p, void *p_table)
{
    if (w_table->counter == ANM2D_DAT_TABLE_P(w_table->table_p)->counter)
    {
        if ((ANM2D_DAT_TABLE_P(w_table->table_p)->attribute & 4) != 0)
        {
            w_table->xscal_off = 0;
            w_table->yscal_off = 0;
        }
        else
        {
            w_table->xscal_off = ANM2D_DAT_TABLE_P(p_table)->x_scale - w_table->sscl[0].sw;
            w_table->yscal_off = ANM2D_DAT_TABLE_P(p_table)->y_scale - w_table->sscl[0].sh;
        }
    }

    w_table->sscl[0].cx = ssd_p->x + w_table->sdat[0].w / 2;
    w_table->sscl[0].cy = ssd_p->y + w_table->sdat[0].h / 2;

    if ((ANM2D_DAT_TABLE_P(w_table->table_p)->attribute & 0x4000) == 0)
    {
        if (ANM2D_DAT_TABLE_P(w_table->table_p)->attribute & 4)
        {
            w_table->xscal_off = w_table->xscal_off + ANM2D_DAT_TABLE_P(w_table->table_p)->x_scale;
            w_table->yscal_off = w_table->yscal_off + ANM2D_DAT_TABLE_P(w_table->table_p)->y_scale;


            if (w_table->sscl[0].sw + (w_table->xscal_off >> 4) >= 1)
            {
                w_table->sscl[0].sw += (w_table->xscal_off >> 4);
            }
            else
            {
                w_table->sscl[0].sw = 0;
            }

            if (w_table->sscl[0].sh + (w_table->yscal_off >> 4) > 0)
            {
                w_table->sscl[0].sh += (w_table->yscal_off >> 4);
            }
            else
            {
                w_table->sscl[0].sh = 0;
            }
        }
        else if (ANM2D_DAT_TABLE_P(p_table)->counter != w_table->counter)
        {
            w_table->sscl[0].sw = ANM2D_DAT_TABLE_P(p_table)->x_scale - (w_table->xscal_off * w_table->counter) / ANM2D_DAT_TABLE_P(p_table)->counter;
            w_table->sscl[0].sh = ANM2D_DAT_TABLE_P(p_table)->y_scale - (w_table->yscal_off * w_table->counter) / ANM2D_DAT_TABLE_P(p_table)->counter;
        }
    }
}

void TblAlphaChg(ANM2D_WRK_TABLE *w_table, SPRT_SDAT *ssd_p, void *p_table)
{
    if (ANM2D_DAT_TABLE_P(p_table)->counter == w_table->counter)
    {
        w_table->alpha_off = ANM2D_DAT_TABLE_P(p_table)->alpha - ssd_p->alp;

        if (ANM2D_DAT_TABLE_P(p_table)->counter == w_table->counter)
        {
            return;
        }
    }

    ssd_p->alp = ANM2D_DAT_TABLE_P(p_table)->alpha - (w_table->alpha_off * w_table->counter) / ANM2D_DAT_TABLE_P(p_table)->counter;
}

void ZanzouEffect(ANM2D_WRK_TABLE *w_table, SPRT_SDAT *ssd_p, void *p_table)
{
    int i;

    if ((ANM2D_DAT_TABLE_P(p_table)->attribute & 8) == 0)
    {
        return;
    }

    for (i = 7; i > 0; i--)
    {
        if (w_table->sdat[i-1].u != 0x7fff)
        {
            if (
                i != 1 &&
                w_table->sscl[i].cx == w_table->sscl[0].cx &&
                w_table->sscl[i].cy == w_table->sscl[0].cy &&
                w_table->sscl[i].sw == w_table->sscl[0].sw &&
                w_table->sscl[i].sh == w_table->sscl[0].sh &&
                w_table->srot[i].rot == w_table->srot[0].rot &&
                w_table->sdat[i].x == w_table->sdat[0].x &&
                w_table->sdat[i].y == w_table->sdat[0].y
            )
            {
                w_table->sdat[i].u = 0x7fff;
            }
            else
            {
                w_table->sdat[i] = w_table->sdat[i-1];
                w_table->srot[i] = w_table->srot[i-1];
                w_table->sscl[i] = w_table->sscl[i-1];

                w_table->sdat[i].alp >>= 1;
            }
        }
    }
}

void BtlReadyDisp(ANM2D_WRK_TABLE *w_table)
{
    int i;
    u_long addr = LOAD_ADDRESS_41;

    SetSprFile(addr);

    if (ANM2D_DAT_TABLE_P(w_table->table_p)->attribute & 0x10000)
    {
        return;
    }

    if (ANM2D_DAT_TABLE_P(w_table->table_p)->attribute & 0x10)
    {
        DrawMessageBox(0x1e000, w_table->sdat[0].x, w_table->sdat[0].y, 592.0f, 96.0f, w_table->sdat[0].alp);
    }
    else if (ANM2D_DAT_TABLE_P(w_table->table_p)->attribute & 0x20)
    {
        PutStringYW(IGMSG_BTL_MSON, 20, w_table->sdat[0].x, w_table->sdat[0].y, 0x808080, 0x80, 0x1000, 0);
    }
    else if (ANM2D_DAT_TABLE_P(w_table->table_p)->attribute & 0x100)
    {
        SetLine(
            0x19000,
            ANM2D_DAT_TABLE_P(w_table->table_p)->x_trans - 320,
            ANM2D_DAT_TABLE_P(w_table->table_p)->y_trans - 224,
            ANM2D_DAT_TABLE_P(w_table->table_p)->x_scale - 320,
            ANM2D_DAT_TABLE_P(w_table->table_p)->y_scale - 224,
            0x8a, 0x76, 0x60,
            ANM2D_DAT_TABLE_P(w_table->table_p)->alpha);
    }
    else if (ANM2D_DAT_TABLE_P(w_table->table_p)->attribute & 0x40)
    {
        SimpleMaskDraw(w_table->sdat[0].alp);
    }
    else if (ANM2D_DAT_TABLE_P(w_table->table_p)->attribute & 0x80)
    {
        DispSprt3(w_table->sdat, addr, w_table->sp_no, w_table->srot, w_table->sscl, 0x64);
    }
    else
    {
        DispSprt2(w_table->sdat, addr, w_table->sp_no, w_table->srot, w_table->sscl, 0x64);

        if (ANM2D_DAT_TABLE_P(w_table->table_p)->attribute & 0x8)
        {
            for (i = 2; i < 8; i++)
            {
                if (w_table->sdat[i].u == 0x7fff)
                {
                    break;
                }

                DispSprt2(&w_table->sdat[i], addr, w_table->sp_no, &w_table->srot[i], &w_table->sscl[i], 0x64);
            }
        }
    }
}

void SimpleMaskDraw(u_char alpha)
{
    SetSquareS(0xf000, -320.0f, -224.0f, 320.0f, 224.0f, 0, 0, 0, alpha);
}

void DispSprt2(SPRT_SDAT *ssd, u_int addr, int sp_no, SPRT_SROT *srot, SPRT_SSCL *sscl, u_char alp_rate)
{
    DISP_SPRT ds;
    SPRT_DAT sd;

    sd.tex0 = GetTex0Reg(addr, sp_no, 0);

    sd.u = ssd->u;
    sd.v = ssd->v;

    sd.w = ssd->w;
    sd.h = ssd->h;

    sd.x = ssd->x;
    sd.y = ssd->y;

    sd.pri = ssd->pri << 12;

    sd.alpha = (ssd->alp * alp_rate) / 100;

    CopySprDToSpr(&ds, &sd);

    ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);

    if (srot != NULL)
    {
        if (srot->cx != 0x7fff)
        {
            ds.rot = srot->rot;
            ds.crx = srot->cx;
            ds.cry = srot->cy;
        }
        else
        {
            ds.att |= srot->cy;
        }
    }

    if (sscl != NULL)
    {
        ds.scw = sscl->sw / 100.0f;
        ds.sch = sscl->sh / 100.0f;
        ds.csx = sscl->cx;
        ds.csy = sscl->cy;
    }

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);
}

void DispSprt3(SPRT_SDAT *ssd, u_int addr, int sp_no, SPRT_SROT *srot, SPRT_SSCL *sscl, u_char alp_rate)
{
    DISP_SPRT ds;
    SPRT_DAT sd;

    sd.tex0 = GetTex0Reg(addr, sp_no, 0);

    sd.u = ssd->u;
    sd.v = ssd->v;

    sd.w = ssd->w;
    sd.h = ssd->h;

    sd.x = ssd->x;
    sd.y = ssd->y;

    sd.alpha = ssd->alp;

    sd.pri = ssd->pri << 12;

    CopySprDToSpr(&ds, &sd);

    ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);

    ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 2, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);

    if (srot != NULL)
    {
        if (srot->cx != 0x7fff)
        {
            ds.rot = srot->rot;
            ds.crx = srot->cx;
            ds.cry = srot->cy;
        }
        else
        {
            ds.att |= srot->cy;
        }
    }

    if (sscl != NULL)
    {
        ds.scw = sscl->sw / 100.0f;
        ds.sch = sscl->sh / 100.0f;
        ds.csx = sscl->cx;
        ds.csy = sscl->cy;
    }

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);
}

void DispSprtTemp(SPRT_SDAT *ssd, u_int addr, int sp_no, SPRT_SROT *srot, SPRT_SSCL *sscl, u_char alp_rate)
{
    DISP_SPRT ds;
    SPRT_DAT sd;

    sd.tex0 = GetTex0Reg(addr, sp_no, 0);

    sd.u = ssd->u;
    sd.v = ssd->v;

    sd.w = ssd->w;
    sd.h = ssd->h;

    sd.x = ssd->x;
    sd.y = ssd->y;

    sd.pri = ssd->pri << 12;

    if (alp_rate == 0xff)
    {
        ds.alphar = 0x48;
    }
    else
    {
        sd.alpha = (ssd->alp * alp_rate) / 100;
    }

    CopySprDToSpr(&ds, &sd);

    if (srot != NULL)
    {
        if (srot->cx != 0x7fff)
        {
            ds.rot = srot->rot;
            ds.crx = srot->cx;
            ds.cry = srot->cy;
        }
        else
        {
            ds.att |= srot->cy;
        }
    }

    if (sscl != NULL)
    {
        ds.scw = sscl->sw / 100.0f;
        ds.sch = sscl->sh / 100.0f;
        ds.csx = sscl->cx;
        ds.csy = sscl->cy;
    }

    DispSprD(&ds);
}

void TestPk2Data_2dg(long int sendtexaddr)
{
    static int ttest_count = 0;
    SPRT_SDAT ssd;

    if (PAD_BTN_PRESSED(PAD_L1))
    {
        ttest_count++;
    }

    if (PAD_BTN_PRESSED(PAD_L2))
    {
        ttest_count--;
    }

    SimpleDispSprtDatCopy(btl_strt, &ssd);

    ssd.u = 1;
    ssd.v = 1;

    ssd.x = 0;
    ssd.y = 0;

    ssd.w = 680;
    ssd.h = 512;

    ssd.alp = 0x80;
    ssd.pri = 0;

    SimpleDispSprt(&ssd, sendtexaddr, ttest_count, NULL, NULL, 0x64);
}

enum T_LOAD_MODE T_LOAD_MODE = 0;
