#include "common.h"
#include "typedefs.h"
#include "addresses.h"
#include "enums.h"
#include "effect_scr.h"

#include "ee/eestruct.h"
#include "ee/kernel.h"
#include "sce/libgraph.h"
#include "sce/libvu0.h"

// gcc/src/newlib/libm/math/sf_sin.c
float sinf(float x);

// gcc/src/newlib/libm/math/wf_sqrt.c
float sqrtf(float x);

#include "main/glob.h"
#include "graphics/graph2d/sprt.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph2d/tim2_new.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/g2d_debug.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/effect_obj.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/libsg.h"
#include "outgame/btl_mode/btl_mode.h"

typedef struct {
    u_int flow;
    u_int cnt;
    u_int in;
    u_int keep;
    u_int out;
    u_int alp;
    float scl;
    float rot;
    float cx;
    float cy;
} EFF_BLUR;

typedef struct {
    u_int flow;
    u_int cnt;
    u_int in;
    u_int keep;
    u_int out;
    u_int max;
} EFF_FOCUS;

typedef struct {
    u_char type;
    u_char otype;
    u_char init;
    u_char pass;
} EFF_DEFORM;

typedef struct {
    float rrr;
    float lll;
    float mm1;
    float mm2;
    float sss;
    float ccc;
} DEFWORK;

typedef struct {
    float scl;
    short int rot;
    short int bx;
    short int by;
    u_char flip;
    u_char alp;
} ONE_HAND;

typedef struct {
    ONE_HAND oh[16];
    u_long count;
    int wait;
    u_int top;
} HAND_WRK;

static HAND_WRK hand_wrk;
static DEFWORK dw[25][33];

static int now_loading_flow;

EFF_BLUR eff_blur = {
    .flow = 3,
    .cnt = 0,
    .in = 0,
    .keep = 0,
    .out = 0,
    .alp = 0,
    .scl = 1.0,
    .rot = 180.0,
    .cx = 320.0,
    .cy = 224.0,
};

EFF_FOCUS eff_focus = {
    .flow = 3,
    .cnt = 0,
    .in = 0,
    .keep = 0,
    .out = 0,
    .max = 0,
};

EFF_DEFORM eff_deform = {
    .type = 0,
    .otype = 0,
    .init = 0,
    .pass = 0,
};
short int overlap_passflg[2] = { 0, 0 };
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
float blur_x_offset = 0.0f;
float blur_y_offset = 0.0f;
#endif

#define PI 3.1415927f

#define INIT_TBP 0x1a40
#define TBP_0x8c0 (INIT_TBP / 3)

#if defined(BUILD_JP_VERSION)
#define VER_LOCAL_COPY_L_TO_L LocalCopyLtoL
#elif defined(BUILD_US_VERSION)
#define VER_LOCAL_COPY_L_TO_L LocalCopyLtoLDraw
#elif defined(BUILD_EU_VERSION)
#define VER_LOCAL_COPY_L_TO_L LocalCopyLtoLDraw
#endif

void InitEffectScr()
{
    MakeRDither3();

    SetParam(0, 0, 0, 0, 0, 0);

    eff_blur.flow = 3;
    eff_blur.cnt = 0;
    eff_blur.in = 0;
    eff_blur.keep = 0;
    eff_blur.out = 0;
    eff_blur.alp = 0;
    eff_blur.scl = 1.0f;
    eff_blur.rot = 180.0f;
    eff_blur.cx = 320.0f;
    eff_blur.cy = 224.0f;

    eff_focus.flow = 3;
    eff_focus.cnt = 0;
    eff_focus.in = 0;
    eff_focus.keep = 0;
    eff_focus.out = 0;
    eff_focus.max = 0;

    eff_deform.type = 0;
    eff_deform.otype = 0;
    eff_deform.init = 1;
    eff_deform.pass = 0;

    ef_exfade1_flow1 = 7;
    ef_exfade1_flow2 = 11;
    ef_exfade1_time1 = 1;

    ef_exfade2_flow1 = 6;
    ef_exfade2_time1 = 1;
    ef_exfade2_time2 = 1;
    ef_exfade2_time3 = 1;

    InitScreenSaver();
}

void InitEffectScrEF()
{
    overlap_passflg[1] = overlap_passflg[0];

    overlap_passflg[0] = 0;

    if (eff_deform.pass == 0)
    {
        eff_deform.init = 1;
    }

    eff_deform.otype = eff_deform.type;
    eff_deform.pass = 0;
}

void SetWhiteOut()
{
    SetParam(0, 32, 0xff, 0xff, 0xff, 2);
}

void SetWhiteIn()
{
    SetParam(0x80, 32, 0xff, 0xff, 0xff, 1);
}

void SetBlackOut()
{
    SetParam(0, 32, 0, 0, 0, 2);
}

void SetBlackIn()
{
    SetParam(0x80, 32, 0, 0, 0, 1);
}

void SetWhiteOut2(int time)
{
    SetParam(0, time, 0xff, 0xff, 0xff, 2);
}

void SetWhiteIn2(int time)
{
    SetParam(0x80, time, 0xff, 0xff, 0xff, 1);
}

void SetBlackOut2(int time)
{
    SetParam(0, time, 0, 0, 0, 2);
}

void SetBlackIn2(int time)
{
    SetParam(0x80, time, 0x0, 0x0, 0x0, 1);
}

void SetFlash()
{
    SetParam(0x80, 18, 0xff, 0xff, 0xff, 1);
}

void SetBlackFilter(EFFECT_CONT *ec)
{
    SetPanel(0x500, 0.0f, 0.0f, 640.0f, 448.0f, 0, 0, 0, ec->dat.uc8[2]);

    if (ec->dat.uc8[1] & 0x1)
    {
        ResetEffects(ec);
    }
}

void SubBlur(int type, u_char alpha, float scale, float rot, float cx, float cy)
{
    SPRITE_DATA sd = {
        .g_GsTex0 = {
            .TBP0 = INIT_TBP,
            .TBW = 10,
            .PSM = 0,
            .TW = 10,
            .TH = 8,
            .TCC = 0,
            .TFX = 0,
            .CBP = 0,
            .CPSM = 0,
            .CSM = 0,
            .CSA = 0,
            .CLD = 1,
        },
        .g_nTexSizeW = 640,
        .g_nTexSizeH = 224,
        .g_bMipmapLv = 0,
        .g_GsMiptbp1 = 0,
        .g_GsMiptbp2 = 0,
        .pos_x = -353.0f,
        .pos_y = -247.4,
        .pos_z = 0,
        .size_w = 704.0f,
        .size_h = 492.8,
        .scale_w = 1.0f,
        .scale_h = 1.0f,
        .clamp_u = 0,
        .clamp_v = 0,
        .rot_center = 1,
        .angle = 0.0f,
        .r = 0x80,
        .g = 0x80,
        .b = 0x80,
        .alpha = 0x80,
        .mask = 0,
    };
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GEQUAL, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GREATER),
        .clamp = 0,
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 5),
    };
    float hw;
    float hh;
    float fx;
    float fy;

    if (plyr_wrk.sta & 0x1 && efcnt[12].dat.uc8[0] != 0)
    {
        return;
    }

    switch (type)
    {
    case 0:
        sd.r = 0x80;
        sd.g = 0x80;
        sd.b = 0x80;
    break;
    case 1:
        sd.r = 0x78;
        sd.g = 0x78;
        sd.b = 0x78;
    break;
    case 2:
        sd.r = 0x88;
        sd.g = 0x88;
        sd.b = 0x88;
    break;
    }

    hh = scale * 448.0f - 448.0f;
    hw = scale * 640.0f - 640.0f;

    fx = (hw * cx) / 640.0f;
    fy = (hh * cy * 2) / 448.0f;

    sd.pos_x = -320.5f - fx;
    sd.pos_y = -224.5f - fy;

    sd.pos_z = 0xfffffff - 0xa0;

    sd.size_w = hw + 640.0f;
    sd.size_h = hh + 448.0f;

    sd.angle = rot - 180.0f;

    sd.alpha = alpha;

    if (eff_blur_off == 0)
    {
        sd.g_GsTex0.TBP0 = ((sys_wrk.count + 1) % 2) * TBP_0x8c0;
    }
    else
    {
        sd.g_GsTex0.TBP0 = (sys_wrk.count % 2) * TBP_0x8c0;
    }

    SetTexDirect(&sd, 1);
}

void SetBlur(EFFECT_CONT *ec)
{
    if (ec->pnt[0] != NULL)
    {
        SubBlur(ec->dat.uc8[2], *(u_char *)ec->pnt[0], ec->dat.ui32[2] / 1000.0f, ec->dat.ui32[3] / 10.0f, ec->fw[0], ec->fw[1]);
    }

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void RunBlur(EFFECT_CONT *ec)
{
    int ef;
    float phase;

    switch (eff_blur.flow)
    {
    case 0:
        eff_blur.cnt++;

        if (eff_blur.in != 0)
        {
            phase = (eff_blur.cnt * 90.0f) / eff_blur.in;

            ef = VER_SINF(phase * PI / 180.0f) * eff_blur.alp;

            SubBlur(0, ef, eff_blur.scl, eff_blur.rot, eff_blur.cx, eff_blur.cy);
        }

        if (eff_blur.cnt >= eff_blur.in)
        {
            eff_blur.flow = eff_blur.keep ? 1 : (eff_blur.out ? 2 : 3);
            eff_blur.cnt = 0;
        }
    break;
    case 1:
        eff_blur.cnt++;

        ef = VER_SINF(PI / 2) * eff_blur.alp;

        SubBlur(0, ef, eff_blur.scl, eff_blur.rot, eff_blur.cx, eff_blur.cy);

        if (eff_blur.cnt >= eff_blur.keep)
        {
            eff_blur.flow = eff_blur.out ? 2 : 3;
            eff_blur.cnt = 0;
        }
    break;
    case 2:
        eff_blur.cnt++;

        if (eff_blur.out != 0)
        {
            phase = (eff_blur.cnt * 90.0f) / eff_blur.out + 90.0f;

            ef = VER_SINF(phase * PI / 180.0f) * eff_blur.alp;

            SubBlur(0, ef, eff_blur.scl, eff_blur.rot, eff_blur.cx, eff_blur.cy);
        }

        if (eff_blur.cnt >= eff_blur.out)
        {
            eff_blur.flow = 3;
            eff_blur.cnt = 0;
        }
    break;
    }
}

void CallBlur(int type, int wait, u_char alpha, float scale, float rot)
{
    eff_blur.flow = wait ? type != 0 : 3;
    eff_blur.cnt = 0;
    eff_blur.alp = alpha;
    eff_blur.scl = scale;
    eff_blur.rot = rot;
    eff_blur.in = 30;
    eff_blur.keep = wait;
    eff_blur.out = 30;
    eff_blur.cx = 320.0f;
    eff_blur.cy = 224.0f;
}

void CallBlur2(int in, int keep, int out, u_char alpha, float scale, float rot)
{
    eff_blur.flow = in < 1 ? (keep < 1 ? (out > 0 ? 2 : 3) : 1) : 0;
    eff_blur.cnt = 0;
    eff_blur.in = in;
    eff_blur.keep = keep;
    eff_blur.out = out;
    eff_blur.alp = alpha;
    eff_blur.scl = scale;
    eff_blur.rot = rot;
    eff_blur.cx = 320.0f;
    eff_blur.cy = 224.0f;
}

void CallBlur3(int in, int keep, int out, u_char alpha, float scale, float rot, float cx, float cy)
{
    eff_blur.flow = in < 1 ? (keep < 1 ? (out > 0 ? 2 : 3) : 1) : 0;
    eff_blur.cnt = 0;
    eff_blur.in = in;
    eff_blur.keep = keep;
    eff_blur.out = out;
    eff_blur.alp = alpha;
    eff_blur.scl = scale;
    eff_blur.rot = rot;
    eff_blur.cx = cx;
    eff_blur.cy = cy;
}

void SubFocus(int ef)
{
    SPRITE_DATA sd = {
        .g_GsTex0 = {
            .TBP0 = 0,
            .TBW = 10,
            .PSM = 0,
            .TW = 10,
            .TH = 8,
            .TCC = 0,
            .TFX = 0,
            .CBP = 0,
            .CPSM = 0,
            .CSM = 0,
            .CSA = 0,
            .CLD = 1,
        },
        .g_nTexSizeW = 640,
        .g_nTexSizeH = 224,
        .g_bMipmapLv = 0,
        .g_GsMiptbp1 = 0,
        .g_GsMiptbp2 = 0,
        .pos_x = -320.5f,
        .pos_y = -224.5f,
        .pos_z = 0,
        .size_w = 640.0f,
        .size_h = 448.0f,
        .scale_w = 1.0f,
        .scale_h = 1.0f,
        .clamp_u = 0,
        .clamp_v = 0,
        .rot_center = 0,
        .angle = 0.0,
        .r = 0x80,
        .g = 0x80,
        .b = 0x80,
        .alpha = 0x50,
        .mask = 0,
    };
    float hw = 0.0f;
    float ff = 0.0f;

    if (ef <= 0)
    {
        return;
    }

    hw = ef / 40.0f;

    sd.alpha = ef > 10 ? 0x28 : ef * 4;

    sd.pos_x = -320.5f - hw;
    sd.pos_y = (-224.5f - hw) + ff;
    sd.pos_z = 0xffffdff;

    sd.g_GsTex0.TBP0 = (sys_wrk.count % 2) * TBP_0x8c0;

    SetTexDirectS(0x200, &sd, 0);

    sd.pos_x = hw + -320.5f;
    sd.pos_y = (-224.5f - hw) + ff;

    SetTexDirectS(0x200, &sd, 0);

    sd.pos_x = -320.5f - hw;
    sd.pos_y = hw + -224.5f + ff;

    SetTexDirectS(0x200, &sd, 0);

    sd.pos_x = hw + -320.5f;
    sd.pos_y = hw + -224.5f + ff;

    SetTexDirectS(0x200, &sd, 0);
}

void SetFocus(EFFECT_CONT *ec)
{
    if (ec->dat.uc8[2] == 0 || eff_focus.flow != 3)
    {
        return;
    }

    SubFocus(ec->dat.uc8[2]);

    if(ec->dat.uc8[1] % 2)
    {
        ResetEffects(ec);
    }
}

void RunFocus(EFFECT_CONT *ec)
{
    int ef;
    float phase;

    switch (eff_focus.flow)
    {
    case 0:
        eff_focus.cnt++;

        if (eff_focus.in != 0)
        {
            phase = (eff_focus.cnt * 90.0f) / eff_focus.in;

            ef = VER_SINF(phase * PI / 180.0f) * eff_focus.max;

            SubFocus(ec->dat.uc8[2] + ef);
        }

        if (eff_focus.cnt >= eff_focus.in)
        {
            eff_focus.flow = eff_focus.keep ? 1 : (eff_focus.out ? 2 : 3);
            eff_focus.cnt = 0;
        }
    break;
    case 1:
        eff_focus.cnt++;

        ef = VER_SINF(PI / 2) * eff_focus.max;

        SubFocus(ec->dat.uc8[2] + ef);

        if (eff_focus.cnt >= eff_focus.keep)
        {
            eff_focus.flow = eff_focus.out ? 2 : 3;
            eff_focus.cnt = 0;
        }
    break;
    case 2:
        eff_focus.cnt++;

        if (eff_focus.out != 0)
        {
            phase = (eff_focus.cnt * 90.0f) / eff_focus.out + 90.0f;

            ef = VER_SINF(phase * PI / 180.0f) * eff_focus.max;

            SubFocus(ec->dat.uc8[2] + ef);
        }

        if (eff_focus.cnt >= eff_focus.out)
        {
            eff_focus.flow = 3;
            eff_focus.cnt = 0;
        }
    break;
    }
}

void CallFocus(int type, int wait, int gap)
{
    eff_focus.flow = type != 0;
    eff_focus.cnt = 0;
    eff_focus.max = gap;
    eff_focus.in = 30;
    eff_focus.keep = wait;
    eff_focus.out = 30;
}

void CallFocus2(int in, int keep, int out, int max)
{
    eff_focus.flow = in < 1 ? (keep < 1 ? (out > 0 ? 2 : 3) : 1) : 0;
    eff_focus.cnt = 0;
    eff_focus.max = max;
    eff_focus.in = in;
    eff_focus.keep = keep;
    eff_focus.out = out;
}

void SubDeform(int type, float rate, u_char alp)
{
    switch(type)
    {
    case 1:
        SetDeform0(type, rate, alp);
    break;
    case 2:
        SetDeform0(type, rate, alp);
    break;
    case 3:
        SetDeform2(type, rate, alp);
    break;
    case 4:
        SetDeform3(type, rate, alp);
    break;
    case 5:
        SetDeform4(type, rate, alp);
    break;
    case 6:
        SetDeform5(type, rate, alp);
    break;
    case 7:
        SetDeform6(type, rate, alp);
    break;
    }
}

void SetDeform(EFFECT_CONT *ec)
{
    float ef;
    float phase;

    if (ec->dat.uc8[2] != eff_deform.otype)
    {
        eff_deform.init = 1;
    }

    if (ec->dat.uc8[1] & 4)
    {
        switch (ec->flow)
        {
        case 0:
            ec->cnt++;

            if (ec->in != 0)
            {
                phase = ((ec->cnt * 90.0f) / ec->in);

                ef = VER_SINF((phase * PI) / 180.0f) * ec->dat.uc8[3];
            }
            else
            {
                ef = 0.0f;
            }

            if (ec->cnt >= ec->in)
            {
                ec->flow = ec->keep ? 1 : (ec->out ? 2 : 3);
                ec->cnt = 0;
            }
        break;
        case 1:
            ec->cnt++;

            ef = VER_SINF(PI / 2) * ec->dat.uc8[3];

            if (ec->cnt >= ec->keep)
            {
                ec->flow = ec->out ? 2 : 3;
                ec->cnt = 0;
            }
        break;
        case 2:
            ec->cnt++;

            if (ec->out != 0)
            {
                phase = (ec->cnt * 90.0f) / ec->out + 90.0f;

                ef = VER_SINF(phase * PI / 180.0f) * ec->dat.uc8[3];
            }
            else
            {
                ef = 0.0f;
            }

            if (ec->cnt >= ec->out)
            {
                ec->flow = 3;
                ec->cnt = 0;

                ResetEffects(ec);

                return;
            }
        break;
        }
    }
    else
    {
        ef = ec->dat.uc8[3];
    }

    SubDeform(ec->dat.uc8[2], ef, 0x80);

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void RunDeform(EFFECT_CONT *ec)
{
    float ef;
    float phase;

    if (ec->dat.uc8[1] & 0x4)
    {
        switch(ec->flow)
        {
        case 0:
            ec->cnt++;

            if (ec->in != 0)
            {
                phase = (ec->cnt * 90.0f) / ec->in;

                ef = VER_SINF(phase * PI / 180.0f) * ec->dat.uc8[3];

                SubDeform(ec->dat.uc8[2], ef, 0x80);
            }

            if (ec->cnt >= ec->in)
            {
                ec->flow = ec->keep ? 1 : (ec->out ? 2 : 3);
                ec->cnt = 0;
            }
        break;
        case 1:
            ec->cnt++;

            ef = VER_SINF(PI / 2) * ec->dat.uc8[3];

            SubDeform(ec->dat.uc8[2], ef, 0x80);

            if (ec->cnt >= ec->keep)
            {
                ec->flow = ec->out ? 2 : 3;
                ec->cnt = 0;
            }
        break;
        case 2:
            ec->cnt++;

            if (ec->out != 0)
            {
                phase = (ec->cnt * 90.0f) / ec->out + 90.0f;

                ef = VER_SINF(phase * PI / 180.0f) * ec->dat.uc8[3];

                SubDeform(ec->dat.uc8[2], ef, 0x80);
            }

            if (ec->cnt >= ec->out)
            {
                ec->flow = 3;
                ec->cnt = 0;

                ResetEffects(ec);
            }
        break;
        }
    }
}

void CallDeform2(int in, int keep, int out, int type, int max)
{
    SetEffects(6, 4, type, max, in, keep, out);
}

#if defined(BUILD_JP_VERSION)
static inline void _SetScrData(Q_WORDDATA *dst, SCRDEF *src)
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
static void _SetScrData(Q_WORDDATA *dst, SCRDEF *src)
#endif
{
    asm volatile("                      \n\
        lqc2       vf12,0(%1)           \n\
        lqc2       vf13,0x10(%1)        \n\
        vftoi4.xy  vf12xy,$vf12xy       \n\
        vftoi4.xyz vf13xyz,$vf13xyz     \n\
        vmulx.zw   vf12zw,$vf12zw,$vf0x \n\
        sqc2       vf13,0x10(%0)        \n\
        sqc2       vf12,0(%0)           \n\
    ": :"r"(dst),"r"(src));
}

void MakeScrDeformPacket(int pnumw, int pnumh, u_long tex0, SCRDEF (*scrdef)[33], int alp)
{
    int i;
    int j;
    int k;
    Q_WORDDATA *ppbuf;

    Reserve2DPacket(0x1000);

    ppbuf = Get2DPacketBufferAddress();

    ppbuf->ul128 = (u_long128)0;
    ppbuf->ui32[0] = (pnumh * (pnumw + 1)) * 4 + DMAend + 9;
    ppbuf++;

    ppbuf->ul64[0] = SCE_GIF_SET_TAG(7, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    ppbuf->ul64[1] = SCE_GIF_PACKED_AD;
    ppbuf++;

    ppbuf->ul64[0] = 0;
    ppbuf->ul64[1] = SCE_GS_TEXFLUSH;
    ppbuf++;

    ppbuf->ul64[0] = tex0;
    ppbuf->ul64[1] = SCE_GS_TEX0_1;
    ppbuf++;

    ppbuf->ul64[0] = SCE_GS_SET_TEX1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    ppbuf->ul64[1] = SCE_GS_TEX1_1;
    ppbuf++;

    ppbuf->ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    ppbuf->ul64[1] = SCE_GS_ZBUF_1;
    ppbuf++;

    ppbuf->ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x80);
    ppbuf->ul64[1] = SCE_GS_ALPHA_1;
    ppbuf++;

    ppbuf->ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_ALWAYS, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_ALWAYS);
    ppbuf->ul64[1] = SCE_GS_TEST_1;
    ppbuf++;

    ppbuf->ul64[0] = SCE_GS_SET_RGBAQ(0x80, 0x80, 0x80, alp, 1);
    ppbuf->ul64[1] = SCE_GS_RGBAQ;
    ppbuf++;

    ppbuf->ul64[0] = SCE_GIF_SET_TAG((pnumh * (pnumw + 1)) * 2, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 1, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 2);
    ppbuf->ul64[1] = 0 \
        | SCE_GS_UV << (4 * 0)
        | SCE_GS_XYZF2 << (4 * 1);
    ppbuf++;

#if defined(BUILD_JP_VERSION)
    for (j = 0; j < pnumh; j++)
    {
        for (i = 0; i < 2; i++)
        {
            _SetScrData(&ppbuf[0], &scrdef[i % 2 + j][i / 2]);

            ppbuf[1].ui32[3] = 0x8000;

            ppbuf += 2;
        }

        for (i = 2; i < (pnumw + 1) * 2;  i++)
        {
            _SetScrData(&ppbuf[0], &scrdef[i % 2 + j][i / 2]);

            ppbuf += 2;
        }
    }
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    for (j = 0; j < pnumh; j++)
    {
        _SetScrData(&ppbuf[0], &scrdef[j][0]);
        _SetScrData(&ppbuf[2] , &scrdef[j+1][0]);

        ppbuf[1].ui32[3] = 0x8000;
        ppbuf[3].ui32[3] = 0x8000;

        ppbuf += 4;

        for (i = 2, k = 1; i < (pnumw + 1) * 2;  k++, i += 2)
        {
            _SetScrData(&ppbuf[0], &scrdef[j][k]);
            _SetScrData(&ppbuf[2], &scrdef[j+1][k]);

            ppbuf += 4;
        }
    }
#endif

    Set2DPacketBufferAddress(ppbuf);
}

void SetDeform0(int type, float rate, u_char alp)
{
    static float r = 0.0f;
    static float add = 6.0f;
    static int swch = 0;
    int i;
    int j;
    int c;
    int pnumw;
    int vnumw;
    int pnumh;
    int vnumh;
    float ll;
    float fw;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    float yoff;
#endif
    float tx[17][25];
    float ty[17][25];
    sceVu0FVECTOR vtw[17][25];
    SPRITE_DATA sd1 = {
        .g_GsTex0 = {
            .TBP0 = 0x1a40,
            .TBW = 10,
            .PSM = 0,
            .TW = 10,
            .TH = 8,
            .TCC = 0,
            .TFX = 0,
            .CBP = 0,
            .CPSM = 0,
            .CSM = 0,
            .CSA = 0,
            .CLD = 1,
        },
        .g_nTexSizeW = 640,
        .g_nTexSizeH = 224,
        .g_bMipmapLv = 0,
        .g_GsMiptbp1 = 0,
        .g_GsMiptbp2 = 0,
        .pos_x = -320.5f,
        .pos_y = -224.5f,
        .pos_z = 0,
        .size_w = 640.0f,
        .size_h = 448.0f,
        .scale_w = 1.0f,
        .scale_h = 1.0f,
        .clamp_u = 0,
        .clamp_v = 0,
        .rot_center = 0,
        .angle = 0.0f,
        .r = 0xff,
        .g = 0xff,
        .b = 0xff,
        .alpha = 0x80,
        .mask = 0,
    };

    vnumh = 17;
    vnumw = 25;

    pnumh = vnumh-1;
    pnumw = vnumw-1;

    VER_LOCAL_COPY_L_TO_L((sys_wrk.count & 1) * 0x8c0, 0x1a40);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    yoff = GetYOffsetf();
#endif

    for (j = 0; j < vnumh; j++)
    {
        for (i = 0; i < vnumw; i++)
        {
#if defined(BUILD_JP_VERSION)
            vtw[j][i][0] = (float)(i * 640) / pnumw + 1727.5f;
            vtw[j][i][1] = (float)(j * 224) / pnumh + 1935.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            vtw[j][i][0] = (float)(i * 640) / pnumw + 1726.5f;
            vtw[j][i][1] = (float)(j * 224) / pnumh + 1936.0f;
#endif
            vtw[j][i][2] = 0.0f;
            vtw[j][i][3] = 1.0f;

            tx[j][i] = (float)(i * 640) / pnumw;
#if defined(BUILD_JP_VERSION)
            ty[j][i] = (float)(j * 224) / pnumh;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            ty[j][i] = (float)(j * 224) / pnumh + yoff;
#endif

            if (i == 0)
            {
#if defined(BUILD_JP_VERSION)
                tx[j][0] += 0.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                tx[j][0] += 1.0f;
#endif
            }

            if (i == vnumw-1)
            {
#if defined(BUILD_JP_VERSION)
                tx[j][vnumw-1] -= 0.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                tx[j][vnumw-1] -= 1.0f;
#endif
            }

            if (j == 0)
            {
#if defined(BUILD_JP_VERSION)
                ty[0][i] += 0.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                ty[0][i] += 1.0f;
#endif
            }

            if (j == vnumh-1)
            {
#if defined(BUILD_JP_VERSION)
                ty[vnumh-1][i] -= 0.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                ty[vnumh-1][i] -= 1.0f;
#endif
            }
        }
    }

    add = 2.0f;

    fw = rate / 10.0f;

    for (j = 0; j < vnumh; j++)
    {
        for (i = 0; i < vnumw; i++)
        {
            if (i == 0 || i == vnumw-1 || j == 0 || j == vnumh-1) {
                continue;
            }
            ll = VER_SINF(((r + j * 50.0f) * PI) / 180.0f) * fw;

            vtw[j][i][0] += ll;

            if (type == 2)
            {
                ll = VER_SINF(((r + i * 50.0f) * PI) / 180.0f) * fw;
            }

            vtw[j][i][1] += ll;
        }
    }

    if (stop_effects == 0)
    {
        r = r + add <= 360.0f ? r + add : (r + add) - 360.0f;
    }

    Reserve2DPacket(0x1000);

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = DMAend + 8 + (pnumh * (pnumw+1)) * 6;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    pbuf[ndpkt].ul64[0] = *(u_long *)&sd1.g_GsTex0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x80);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_ALWAYS, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_ALWAYS);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    if (swch != 0)
    {
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG((pnumh * (pnumw+1))*2, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 0, 0, 0, 1, 1, 1, 0, 0), SCE_GIF_PACKED, 3);
    }
    else
    {
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG((pnumh * (pnumw+1))*2, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 1, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 3);
    }
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0)
        | SCE_GS_UV    << (4 * 1)
        | SCE_GS_XYZF2 << (4 * 2);

    c = 100 / ((pnumw+1)*2);

    for (j = 0; j < pnumh; j++)
    {
        for (i = 0; i < (pnumw + 1) * 2; i++)
        {
            if (swch == 0)
            {
                pbuf[ndpkt].ui32[0] = 0x80;
                pbuf[ndpkt].ui32[1] = 0x80;
                pbuf[ndpkt].ui32[2] = 0x80;
                pbuf[ndpkt++].ui32[3] = alp;
            }
            else
            {
                pbuf[ndpkt].ui32[0] = i * c;
                pbuf[ndpkt].ui32[1] = i * c;
                pbuf[ndpkt].ui32[2] = i * c;
                pbuf[ndpkt++].ui32[3] = alp;
            }

            pbuf[ndpkt].ui32[0] = (int)(tx[i % 2 + j][i / 2] * 16.0f);
            pbuf[ndpkt].ui32[1] = (int)(ty[i % 2 + j][i / 2] * 16.0f);
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = (int)(vtw[i % 2 + j][i / 2][0] * 16.0f);
            pbuf[ndpkt].ui32[1] = (int)(vtw[i % 2 + j][i / 2][1] * 16.0f);
            pbuf[ndpkt].ui32[2] = (int)(vtw[i % 2 + j][i / 2][2] * 16.0f);
            pbuf[ndpkt++].ui32[3] = (i <= 1) ? 0x8000 : 0;
        }
    }

    eff_deform.init = 1;
    eff_deform.pass = 1;
    eff_deform.type = type;
}

void SetDeform2(int type, float rate, u_char alp)
{
    static float rrr[825];
    static float lll[825];
    static float mm1[825];
    static float mm2[825];
    static float sss[825];
    static float ccc[825];
    static float r = 0.0f;
    static float add = 6.0f;
    static float ll = 0.0f;
    static int swch = 0;
    int i;
    int j;
    int k;
    int m;
    int bak;
    int pnumw;
    int vnumw;
    int wix;
    int pnumh;
    int vnumh;
    int wiy;
    float wfw;
    float wfh;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    float yoff;
#endif
    float l;
    float fw;
    float lm;
    float cntw;
    float cnth;
    float ts;
    float fx;
    float fy;
    float fz;
    float rot_x;
    float rot_y;
    float tx[825];
    float ty[825];
    sceVu0FMATRIX slm;
    sceVu0FMATRIX wlm;
    sceVu0FVECTOR pos =  { 25000.0f, -800.0f, 6500.0f, 1.0f };
    sceVu0FVECTOR vt[825];
    sceVu0FVECTOR vtw[825];
    SPRITE_DATA sd1 = {
        .g_GsTex0 = {
            .TBP0 = 0x1a40,
            .TBW = 10,
            .PSM = 0,
            .TW = 10,
            .TH = 8,
            .TCC = 0,
            .TFX = 0,
            .CBP = 0,
            .CPSM = 0,
            .CSM = 0,
            .CSA = 0,
            .CLD = 1,
        },
        .g_nTexSizeW = 640,
        .g_nTexSizeH = 224,
        .g_bMipmapLv = 0,
        .g_GsMiptbp1 = 0,
        .g_GsMiptbp2 = 0,
        .pos_x = -320.5f,
        .pos_y = -224.5f,
        .pos_z = 0,
        .size_w = 640.0f,
        .size_h = 448.0f,
        .scale_w = 1.0f,
        .scale_h = 1.0f,
        .clamp_u = 0,
        .clamp_v = 0,
        .rot_center = 0,
        .angle = 0.0f,
        .r = 0xff,
        .g = 0xff,
        .b = 0xff,
        .alpha = 0x80,
        .mask = 0,
    };
#if defined(BUILD_JP_VERSION)
    float scale;
#endif
    float pnumw_f; // not in STAB ...
    float pnumh_f; // not in STAB ...

    ts = 2.0f;

    pnumw = 32;
    pnumh = 24;
    vnumw = pnumw+1;
    vnumh = pnumh+1;

    cntw = 2 * (float)((pnumw/2) % vnumw);
    cnth = 2 * (float)((pnumh/2) % vnumw);

    fx = camera.i[0] - camera.p[0];
    fy = camera.i[1] - camera.p[1];
    fz = camera.i[2] - camera.p[2];

    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

    l = VER_SQRTF(fx * fx + fy * fy + fz * fz);

    pos[0] = camera.p[0] + (fx * 2000.0f) / l;
    pos[1] = camera.p[1] + (fy * 2000.0f) / l;
    pos[2] = camera.p[2] + (fz * 2000.0f) / l;

    VER_LOCAL_COPY_L_TO_L((sys_wrk.count & 1) * 0x8c0, 0x1a40);

    sceVu0UnitMatrix(wlm);

    wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;

    sceVu0TransMatrix(wlm, wlm, pos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    yoff = GetYOffsetf();
#endif

    pnumw_f = 32.0f;
    pnumh_f = 24.0f;

    for (i = 0; i < vnumw*vnumh; i++)
    {
        wix = (i % vnumw);
        wiy = (i / vnumw);

        vt[i][0] = wix * ts - pnumw_f;
        vt[i][1] = wiy * ts - pnumh_f;
        vt[i][2] = 0.0f;
        vt[i][3] = 1.0f;

        sceVu0ApplyMatrix(vtw[i], slm, vt[i]);
#if defined(BUILD_JP_VERSION)
        scale = 1.0f / vtw[i][3];
        sceVu0ScaleVector(vtw[i], vtw[i], scale);

        tx[i] = ((vtw[i][0] - 2048.0f) + 320.0f) + 0.5f;
        ty[i] = ((vtw[i][1] - 2048.0f) + 112.0f) + 0.5f;

        tx[i] = (tx[i] < 0.0f) ? 0.0f : ((640.0f < tx[i]) ? 640.0f : tx[i]);
        ty[i] = (ty[i] < 0.0f) ? 0.0f : ((224.0f < ty[i]) ? 224.0f : ty[i]);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        sceVu0DivVector(vtw[i], vtw[i], vtw[i][3]);

        tx[i] = ((vtw[i][0] - 2048.0f) + 320.0f);
        ty[i] = ((vtw[i][1] - 2048.0f) + 112.0f) + yoff;

        tx[i] = (tx[i] < 0.0f) ? 0.0f : ((639.0f < tx[i]) ? 639.0f : tx[i]);
        ty[i] = (ty[i] < 0.0f) ? 0.0f : ((223.0f < ty[i]) ? 223.0f : ty[i]);
#endif
    }

    fw = rate / 25.0f;

    if (eff_deform.init != 0)
    {
        for (i = 0; i < vnumw*vnumh; i++)
        {
            wix = i % vnumw;
            wiy = i / vnumw;

            wfw = wix * ts - cntw;
            wfh = wiy * ts - cnth;

            lll[i] = VER_SQRTF(wfw * wfw + wfh * wfh);

            lm = lll[0];

            if ((wix != ((pnumw / 2) % vnumw)) || (wiy != ((pnumh / 2) % vnumw)))
            {
                rrr[i] = VER_ATAN2F(wfw, wfh);
            }
            else
            {
                rrr[i] = 0.0f;
            }

            mm1[i] = (lll[i] * PI * 12.0f) / lm;
            mm2[i] = (lm - lll[i]) / lm;
            sss[i] = VER_SINF(rrr[i]);
            ccc[i] = VER_COSF(rrr[i]);
        }

        eff_deform.init = 0;
    }

#if defined(MATCHING_DECOMP)
    if (lll[0]) {}; // debug code ?? seems that addressing lll is enough ...
#endif

    for (i = 0; i < vnumw*vnumh; i++)
    {
        wix = i % vnumw;
        wiy = i / vnumw;

        fx = lll[i];
        fy = VER_SINF(mm1[i] - r) * fw * mm2[i];

        vt[i][0] = sss[i] * fx - ccc[i] * fy;
        vt[i][1] = ccc[i] * fx + sss[i] * fy;
    }

    add = 0.08f;

    if (stop_effects == 0)
    {
        r = r + add <= (PI * 2) ? r + add : (r + add) - (PI * 2);
    }

    for (i = 0; i < vnumw*vnumh; i++)
    {
        wix = i % vnumw;
        wiy = i / vnumw;

        sceVu0ApplyMatrix(vtw[i], slm, vt[i]);
#if defined(BUILD_JP_VERSION)
        scale = 1.0f / vtw[i][3];
        sceVu0ScaleVector(vtw[i], vtw[i], scale);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        sceVu0DivVector(vtw[i], vtw[i], vtw[i][3]);
#endif
    }

    Reserve2DPacket(0x1000);

    bak = ndpkt;

    pbuf[ndpkt++].ul128 = (u_long128)0;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    pbuf[ndpkt].ul64[0] = *(u_long *)&sd1.g_GsTex0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x80);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, 0, SCE_GS_TRUE, SCE_GS_DEPTH_ALWAYS);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    m = (pnumw + 1) * pnumh;

    if (swch != 0)
    {
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(m, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 0, 0, 0, 1, 1, 1, 0, 0), SCE_GIF_PACKED, 6);
    }
    else
    {
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(m, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 1, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 6);
    }
    pbuf[ndpkt++].ul64[1] = 0
        | SCE_GS_UV    << (4 * 0)
        | SCE_GS_RGBAQ << (4 * 1)
        | SCE_GS_XYZF2 << (4 * 2)
        | SCE_GS_UV    << (4 * 3)
        | SCE_GS_RGBAQ << (4 * 4)
        | SCE_GS_XYZF2 << (4 * 5);

    for (i = 0; i < m; i++)
    {
        j = i + vnumw;
        k = (i % (pnumw+1)) ? 0 : 0x8000;

        pbuf[ndpkt].ui32[0] = (int)(tx[i] * 16.0f);
        pbuf[ndpkt].ui32[1] = (int)(ty[i] * 16.0f);
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt++].ui32[3] = 0;

        if (swch == 0)
        {
            pbuf[ndpkt].ui32[0] = 0x80;
            pbuf[ndpkt].ui32[1] = 0x80;
            pbuf[ndpkt].ui32[2] = 0x80;
            pbuf[ndpkt++].ui32[3] = 0x80;
        }
        else
        {
            pbuf[ndpkt].ui32[0] = (i) % 0xff;
            pbuf[ndpkt].ui32[1] = (i) % 0xff;
            pbuf[ndpkt].ui32[2] = (i) % 0xff;
            pbuf[ndpkt++].ui32[3] = 0x80;
        }

        pbuf[ndpkt].ui32[0] = (int)(vtw[i][0] * 16.0f);
        pbuf[ndpkt].ui32[1] = (int)(vtw[i][1] * 16.0f);
        pbuf[ndpkt].ui32[2] = (int)(vtw[i][2] * 16.0f);
        pbuf[ndpkt++].ui32[3] = k;

        pbuf[ndpkt].ui32[0] = (int)(tx[i + vnumw] * 16.0f);
        pbuf[ndpkt].ui32[1] = (int)(ty[i + vnumw] * 16.0f);
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt++].ui32[3] = 0;

        if (swch == 0)
        {
            pbuf[ndpkt].ui32[0] = 0x80;
            pbuf[ndpkt].ui32[1] = 0x80;
            pbuf[ndpkt].ui32[2] = 0x80;
            pbuf[ndpkt++].ui32[3] = alp;
        }
        else
        {
            pbuf[ndpkt].ui32[0] = (j) % 0xff;
            pbuf[ndpkt].ui32[1] = (j) % 0xff;
            pbuf[ndpkt].ui32[2] = (j) % 0xff;
            pbuf[ndpkt++].ui32[3] = alp;
        }

        pbuf[ndpkt].ui32[0] = (int)(vtw[i + vnumw][0] * 16.0f);
        pbuf[ndpkt].ui32[1] = (int)(vtw[i + vnumw][1] * 16.0f);
        pbuf[ndpkt].ui32[2] = (int)(vtw[i + vnumw][2] * 16.0f);
        pbuf[ndpkt++].ui32[3] = k;
    }

    pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;

    eff_deform.pass = 1;
    eff_deform.type = type;
}

void SetDeform3(int type, float rate, u_char alp)
{
    static float r = 0.0f;
    static float add = 6.0f;
    static float ll = 0.0f;
    static int swch = 0;
    int i;
    int j;
    int c;
    int pnumw;
    int vnumw;
    int pnumh;
    int vnumh;
    int wix;
    int wiy;
    float wfw;
    float wfh;
    float fw;
    float lm;
    float cntw;
    float cnth;
    float fx;
    float fy;
    float tx[25][33];
    float ty[25][33];
    sceVu0FMATRIX slm;
    sceVu0FMATRIX wlm;
    sceVu0FVECTOR pos = { 25000.0f, -800.0f, 6500.0f, 1.0f };
    sceVu0FVECTOR vt[25][33];
    sceVu0FVECTOR vtw[25][33];
    DEFWORK *pdef;
    SPRITE_DATA sd1 = {
        .g_GsTex0 = {
            .TBP0 = 0x1a40,
            .TBW = 10,
            .PSM = 0,
            .TW = 10,
            .TH = 8,
            .TCC = 0,
            .TFX = 0,
            .CBP = 0,
            .CPSM = 0,
            .CSM = 0,
            .CSA = 0,
            .CLD = 1,
        },
        .g_nTexSizeW = 640,
        .g_nTexSizeH = 224,
        .g_bMipmapLv = 0,
        .g_GsMiptbp1 = 0,
        .g_GsMiptbp2 = 0,
        .pos_x = -320.5f,
        .pos_y = -224.5f,
        .pos_z = 0,
        .size_w = 640.0f,
        .size_h = 448.0f,
        .scale_w = 1.0f,
        .scale_h = 1.0f,
        .clamp_u = 0,
        .clamp_v = 0,
        .rot_center = 0,
        .angle = 0.0f,
        .r = 0xff,
        .g = 0xff,
        .b = 0xff,
        .alpha = 0x80,
        .mask = 0,
    };
    float pszw;
    float pszh;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    float yoff;
#endif

    pnumh = 24;
    pnumw = 32;
    vnumh = pnumh+1;
    vnumw = pnumw+1;

    pszw = 640.0f;
    pszh = 224.0f;
    cntw = 320.0f;
    cnth = 112.0f;

    VER_LOCAL_COPY_L_TO_L((sys_wrk.count & 1) * 0x8c0, 0x1a40);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    yoff = GetYOffsetf();
#endif

    for (j = 0; j < vnumh; j++)
    {
        for (i = 0; i < vnumw; i++)
        {
#if defined(BUILD_JP_VERSION)
            vt[j][i][0] = 2047.5f;
            vt[j][i][1] = 2047.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            vt[j][i][0] = 2048.0f;
            vt[j][i][1] = 2048.0f;
#endif
            vt[j][i][2] = 0.0f;
            vt[j][i][3] = 1.0f;

            tx[j][i] = (float)(i * 640) / pnumw;
#if defined(BUILD_JP_VERSION)
            ty[j][i] = (float)(j * 224) / pnumh;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            ty[j][i] = (float)(j * 224) / pnumh + yoff;
#endif

            if (i == 0)
            {
#if defined(BUILD_JP_VERSION)
                tx[j][0] += 0.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                tx[j][0] += 1.0f;
#endif
            }

            if (i == vnumw-1)
            {
#if defined(BUILD_JP_VERSION)
                tx[j][vnumw-1] -= 0.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                tx[j][vnumw-1] -= 1.0f;
#endif
            }

            if (j == 0)
            {
#if defined(BUILD_JP_VERSION)
                ty[0][i] += 0.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                ty[0][i] += 1.0f;
#endif
            }

            if (j == vnumh-1)
            {
#if defined(BUILD_JP_VERSION)
                ty[vnumh-1][i] -= 0.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
                ty[vnumh-1][i] -= 1.0f;
#endif
            }
        }
    }

    fw = rate / 4.0f;

    if (eff_deform.init != 0)
    {
        for (j = 0; j < vnumh; j++)
        {
            for (i = 0; i < vnumw; i++)
            {
                pdef = &dw[j][i];

                wfw = i * (pszw / pnumw) - cntw;
                wfh = j * (pszh / pnumh) - cnth;

                pdef->lll = VER_SQRTF(wfw * wfw + wfh * wfh);

                lm = dw[0][0].lll;

                if ((wix != (pnumw/2) % vnumw) || (wiy != (pnumh/2) % vnumw))
                {
                    pdef->rrr = VER_ATAN2F(wfw, wfh);
                }
                else
                {
                    pdef->rrr = 0.0f;
                }

                pdef->mm2 = (lm - pdef->lll) / lm;
                pdef->mm1 = (pdef->lll * PI * 2) / lm;

                pdef->sss = VER_SINF(pdef->rrr);
                pdef->ccc = VER_COSF(pdef->rrr);
            }
        }

        eff_deform.init = 0;
    }

    if (dw[0][0].lll) {} // debug code ?? seems that addressing dw is enough ...

    for (j = 0; j < vnumh; j++)
   {
        for (i = 0; i < vnumw; i++)
        {
            pdef = &dw[j][i];

            fx = pdef->lll;
            fy = VER_SINF(pdef->mm1 - r) * fw * pdef->mm2;

            vtw[j][i][0] = pdef->sss * fx - pdef->ccc * fy;
            vtw[j][i][1] = pdef->ccc * fx + pdef->sss * fy;
            vtw[j][i][2] = 0.0f;
        }
    }

    add = 0.2f;

    if (stop_effects == 0)
    {
        r = r + add <= (PI * 2) ? r + add : (r + add) - (PI * 2);
    }

    Reserve2DPacket(0x1000);

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = DMAend + (pnumh)*(pnumw+1)*2*3 + 8;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    pbuf[ndpkt].ul64[0] = *(u_long *)&sd1.g_GsTex0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x80);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, 0, SCE_GS_TRUE, SCE_GS_DEPTH_ALWAYS);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    if (swch != 0)
    {
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG((pnumh)*(pnumw+1)*2, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 0, 0, 0, 1, 1, 1, 0, 0), SCE_GIF_PACKED, 3);
    }
    else
    {
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG((pnumh)*(pnumw+1)*2, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 1, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 3);
    }
    pbuf[ndpkt++].ul64[1] = 0
        | SCE_GS_RGBAQ << (4 * 0)
        | SCE_GS_UV    << (4 * 1)
        | SCE_GS_XYZF2 << (4 * 2);

    c = 100 / ((pnumw+1)*2);

    for (j = 0; j < pnumh; j++)
    {
        for (i = 0; i < (pnumw+1)*2; i++)
        {
            if (swch == 0)
            {
                pbuf[ndpkt].ui32[0] = 0x80;
                pbuf[ndpkt].ui32[1] = 0x80;
                pbuf[ndpkt].ui32[2] = 0x80;
                pbuf[ndpkt++].ui32[3] = alp;
            }
            else
            {
                pbuf[ndpkt].ui32[0] = i * c;
                pbuf[ndpkt].ui32[1] = i * c;
                pbuf[ndpkt].ui32[2] = i * c;
                pbuf[ndpkt++].ui32[3] = alp;
            }

            pbuf[ndpkt].ui32[0] = (int)(tx[i%2+j][i/2] * 16.0f);
            pbuf[ndpkt].ui32[1] = (int)(ty[i%2+j][i/2] * 16.0f);
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = (int)((vt[i%2+j][i/2][0] + vtw[i%2+j][i/2][0]) * 16.0f);
            pbuf[ndpkt].ui32[1] = (int)((vt[i%2+j][i/2][1] + vtw[i%2+j][i/2][1]) * 16.0f);
            pbuf[ndpkt].ui32[2] = (int)((vt[i%2+j][i/2][2] + vtw[i%2+j][i/2][2]) * 16.0f);
            pbuf[ndpkt++].ui32[3] = (i <= 1) ? 0x8000 : 0;
        }
    }

    eff_deform.pass = 1;
    eff_deform.type = type;
}

void SetDeform4(int type, float rate, u_char alp)
{
    static float r = 0.0f;
    static float add = 6.0f;
    int i;
    int j;
    int pnumw;
    int vnumw;
    int wix;
    int pnumh;
    int vnumh;
    int wiy;
    float wfw;
    float wfh;
    float f;
    float lw;
    float fw;
    float lm;
    float fx;
    float fy;
    float fz;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    float yoff;
#endif
    SCRDEF scrdef[25][33];
    SCRDEF *pscr;
    DEFWORK *pdef;
    u_long tex0;

    // pnumw; // not initialized
    vnumw = 33;
    // pnumh; // not initialized
    vnumh = 25;

    VER_LOCAL_COPY_L_TO_L((sys_wrk.count & 1) * 0x8c0, 0x1a40);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    yoff = GetYOffsetf();
#endif

    for (j = 0; j < vnumh; j++)
    {
        for (i = 0; i < vnumw; i++)
        {
            pscr = &scrdef[j][i];

            pscr->stq[0] = (i * 640) / 32.0f;
#if defined(BUILD_JP_VERSION)
            pscr->stq[1] = (j * 224) / 24.0f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            pscr->stq[1] = (j * 224) / 24.0f + yoff;
#endif
        }

#if defined(BUILD_JP_VERSION)
        scrdef[j][0].stq[0] += 0.5f;
        scrdef[j][vnumw-1].stq[0] -= 0.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        scrdef[j][0].stq[0] += 1.0f;
        scrdef[j][vnumw-1].stq[0] -= 1.0f;
#endif
    }

    for (i = 0; i < vnumw; i++)
   {
#if defined(BUILD_JP_VERSION)
        scrdef[0][i].stq[1] += 0.5f;
        scrdef[vnumh-1][i].stq[1] -= 0.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        scrdef[0][i].stq[1] += 1.0f;
        scrdef[vnumh-1][i].stq[1] -= 1.0f;
#endif
    }

    fw = rate / 4.0f;
    lm = VER_SQRTF(224 * 224 + 320 * 320);
    lw = 1.0f / lm;

    if (eff_deform.init != 0)
    {
        for (j = 0; j < vnumh; j++)
        {
            for (i = 0; i < vnumw; i++)
            {
                pdef = &dw[j][i];

                wfw = i * 20.0f - 320.0f;
                wfh = j * (448.0f / 24.0f) - 224.0f;

                pdef->lll = VER_SQRTF(wfw * wfw + wfh * wfh);

                if ((wix != (pnumw / 2) % vnumw) || (wiy != (pnumh / 2) % vnumw))
                {
                    pdef->rrr = VER_ATAN2F(wfw, wfh);
                }
                else
                {
                    pdef->rrr = 0.0f;
                }

                pdef->mm1 = pdef->lll * PI * 12.0f * lw;
                pdef->mm2 = (lm - pdef->lll) * lw;

                pdef->sss = VER_SINF(pdef->rrr);
                pdef->ccc = VER_COSF(pdef->rrr);
            }
        }

        eff_deform.init = 0;
    }

    fz = r;

    for (j = 0; j < vnumh; j++)
    {
        for (i = 0; i < vnumw; i++)
        {
            f = VER_RAND() + 1.0f;

            pdef = &dw[j][i];

            fx = pdef->lll;
            fy = VER_SINF(pdef->mm1 - fz) * fw * pdef->mm2 * VER_SINF((fz + pdef->rrr) * 18.0f) * f;

            pscr = &scrdef[j][i];

#if defined(BUILD_JP_VERSION)
            pscr->vtw[0] = (pdef->sss * fx - pdef->ccc * fy) + 2048.0f - 0.5f;
            pscr->vtw[1] = (pdef->ccc * fx - pdef->sss * fy) * 0.5f + 2048.0f - 0.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            pscr->vtw[0] = (pdef->sss * fx - pdef->ccc * fy) + 2048.0f;
            pscr->vtw[1] = (pdef->ccc * fx - pdef->sss * fy) * 0.5f + 2048.0f;
#endif
            pscr->vtw[2] = 0.0f;
            pscr->vtw[3] = 0.0f;
        }
    }

    add = 0.02f;

    if (stop_effects == 0)
    {
        r = r + add <= (PI * 2) ? r + add : (r + add) - (PI * 2);
#if defined(MATCHING_DECOMP)
        // HACK: force loading of a0 and a1 before a2 for MakeScrDeformPacket
        tex0 = SCE_GS_SET_TEX0(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
#endif
    }

#if !defined(MATCHING_DECOMP)
    tex0 = SCE_GS_SET_TEX0(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
#endif
    MakeScrDeformPacket(vnumw - 1, vnumh - 1, tex0, scrdef, alp);

    eff_deform.pass = 1;
    eff_deform.type = type;
}


void SetDeform5(int type, float rate, u_char alp)
{
    static float r = 0.0f;
    static float add = 6.0f;
    int i;
    int j;
    int pnumw;
    int vnumw;
    int wix;
    int pnumh;
    int vnumh;
    int wiy;
    float wfw;
    float wfh;
    float f;
    float lw;
    float fw;
    float lm;
    float fx;
    float fy;
    float fz;
    SCRDEF scrdef[25][33];
    SCRDEF *pscr;
    float xx;
    float yy;
    DEFWORK *pdef;
    u_long tex0;

    // pnumw; // not initialized
    vnumw = 33;
    // pnumh; // not initialized
    vnumh = 25;

    VER_LOCAL_COPY_L_TO_L((sys_wrk.count & 1) * 0x8c0, 0x1a40);

    fx = 20.0f;
    fy = (56.0f / 6.0f);

#if defined(BUILD_JP_VERSION)
    yy = 0.0f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    yy = GetYOffsetf();
#endif

    for (j = 0; j < vnumh; j++)
    {
#if defined(MATCHING_DECOMP)
        pdef = (DEFWORK *)&dw; // HACK: fixes codegen
#endif
        xx = 0.0f;

        for (i = 0; i < vnumw; i++)
        {
            pscr = &scrdef[j][i];

            pscr->stq[0] = xx;
            pscr->stq[1] = yy;

            xx += fx;
        }

#if defined(BUILD_JP_VERSION)
        scrdef[j][0].stq[0] += 0.5f;
        scrdef[j][vnumw-1].stq[0] -= 0.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        scrdef[j][0].stq[0] += 1.0f;
        scrdef[j][vnumw-1].stq[0] -= 1.0f;
#endif

        yy += fy;
    }

    for (i = 0; i < vnumw; i++)
    {
#if defined(BUILD_JP_VERSION)
        scrdef[0][i].stq[1] += 0.5f;
        scrdef[vnumh-1][i].stq[1] -= 0.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        scrdef[0][i].stq[1] += 1.0f;
        scrdef[vnumh-1][i].stq[1] -= 1.0f;
#endif
    }

    fw = rate / 4.0f;
    lm = VER_SQRTF(224 * 224 + 320 * 320);
    lw = 1.0f / lm;

    if (eff_deform.init != 0)
    {
        for (j = 0; j < vnumh; j++)
        {
            for (i = 0; i < vnumw; i++)
            {
                pdef = &dw[j][i];

                wfw = i * 20.0f - 320.0f;
                wfh = j * (448.0f / 24.0f) - 224.0f;

                pdef->lll = VER_SQRTF(wfw * wfw + wfh * wfh);

                if ((wix != (pnumw / 2) % vnumw) || (wiy != (pnumh / 2) % vnumw))
                {
                    pdef->rrr = VER_ATAN2F(wfw, wfh);
                }
                else
                {
                    pdef->rrr = 0.0f;
                }

                pdef->mm1 = pdef->lll * PI * 2.0f * lw;
                pdef->mm2 = (lm - pdef->lll) * lw;

                pdef->sss = VER_SINF(pdef->rrr);
                pdef->ccc = VER_COSF(pdef->rrr);
            }
        }

        eff_deform.init = 0;
    }

#if defined(MATCHING_DECOMP)
    sys_wrk.count = sys_wrk.count; // HACK: fix v1<->v0 reg-swap
#endif

    fz = r;

    for (j = 0; j < vnumh; j++)
    {
        for (i = 0; i < vnumw; i++)
        {
            pdef = &dw[j][i];

            f = VER_RAND() + 1.0f;

            fx = pdef->lll;
            fy = VER_SINF(pdef->mm1 - fz) * fw * pdef->mm2 * VER_SINF((fz + pdef->rrr) * 18.0f) * f;

            pscr = &scrdef[j][i];

#if defined(BUILD_JP_VERSION)
            pscr->vtw[0] = (pdef->sss * fx - pdef->ccc * fy) + 2048.0f - 0.5f;
            pscr->vtw[1] = (pdef->ccc * fx - pdef->sss * fy) * 0.5f + 2048.0f - 0.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            pscr->vtw[0] = (pdef->sss * fx - pdef->ccc * fy) + 2048.0f;
            pscr->vtw[1] = (pdef->ccc * fx - pdef->sss * fy) * 0.5f + 2048.0f;
#endif
            pscr->vtw[2] = 0.0f;
            pscr->vtw[3] = 0.0f;
        }
    }

    add = 0.02f;

    if (stop_effects == 0)
    {
        r = r + add <= (PI * 2) ? r + add : (r + add) - (PI * 2);
#if defined(MATCHING_DECOMP)
        // HACK: force loading of a0 and a1 before a2 for MakeScrDeformPacket
        tex0 = SCE_GS_SET_TEX0(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
#endif
    }

#if !defined(MATCHING_DECOMP)
    tex0 = SCE_GS_SET_TEX0(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
#endif
    MakeScrDeformPacket(vnumw - 1, vnumh - 1, tex0, scrdef, alp);

    eff_deform.pass = 1;
    eff_deform.type = type;
}

void SetDeform6(int type, float rate, u_char alp)
{
    static float r = 0.0f;
    static float add = 6.0f;
    int i;
    int j;
    int pnumw;
    int vnumw;
    int pnumh;
    int vnumh;
    int wix;
    int wiy;
    float wfw;
    float wfh;
    float f;
    float lw;
    float fw;
    float lm;
    float fx;
    float fy;
    float fz;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    float yoff;
#endif
    SCRDEF scrdef[25][33];
    SCRDEF *pscr;
    DEFWORK *pdef;
    u_long tex0;

    // pnumw; // not initialized
    vnumw = 33;
    // pnumh; // not initialized
    vnumh = 25;

    VER_LOCAL_COPY_L_TO_L((sys_wrk.count & 1) * 0x8c0, 0x1a40);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    yoff = GetYOffsetf();
#endif

    for (j = 0; j < vnumh; j++)
    {
#if defined(MATCHING_DECOMP)
        pdef = (DEFWORK *)dw;
#endif
        for (i = 0; i < vnumw; i++)
        {
            pscr = &scrdef[j][i];

            pscr->stq[0] = (i * 640) / 32.0f;
#if defined(BUILD_JP_VERSION)
            pscr->stq[1] = (j * 224) / 24.0f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            pscr->stq[1] = (j * 224) / 24.0f + yoff;
#endif
        }

#if defined(BUILD_JP_VERSION)
        scrdef[j][0].stq[0] += 0.5f;
        scrdef[j][vnumw-1].stq[0] -= 0.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        scrdef[j][0].stq[0] += 1.0f;
        scrdef[j][vnumw-1].stq[0] -= 1.0f;
#endif
    }

    for (i = 0; i < vnumw; i++)
    {
#if defined(BUILD_JP_VERSION)
        scrdef[0][i].stq[1] += 0.5f;
        scrdef[vnumh-1][i].stq[1] -= 0.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        scrdef[0][i].stq[1] += 1.0f;
        scrdef[vnumh-1][i].stq[1] -= 1.0f;
#endif
    }

    fw = rate / 4.0f;
    lm = VER_SQRTF(224 * 224 + 320 * 320);
    lw = 1.0f / lm;

    if (eff_deform.init != 0)
    {
        for (j = 0; j < vnumh; j++)
        {
            for (i = 0; i < vnumw; i++)
            {
                pdef = &dw[j][i];

                wfw = i * 20.0f - 320.0f;
                wfh = j * (448.0f / 24.0f) - 224.0f;

                pdef->lll = VER_SQRTF(wfw * wfw + wfh * wfh);

                if ((wix != (33/2) % vnumw) || (wiy != (25/2) % vnumw))
                {
                    pdef->rrr = VER_ATAN2F(wfw, wfh);
                }
                else
                {
                    pdef->rrr = 0.0f;
                }

                pdef->mm1 = pdef->lll * PI * 2.0f * lw;
                pdef->mm2 = (lm - pdef->lll) * lw;

                pdef->sss = VER_SINF(pdef->rrr);
                pdef->ccc = VER_COSF(pdef->rrr);
            }
        }

        eff_deform.init = 0;
    }

    fz = r;

    for (j = 0; j < vnumh; j++)
    {
        for (i = 0; i < vnumw; i++)
        {
            pdef = &dw[j][i];
            pscr = &scrdef[j][i];

            f = VER_RAND() + 1.0f;

            fx = pdef->lll;
            fy = VER_SINF(pdef->mm1 - fz) * fw * pdef->mm2 * VER_SINF((fz + pdef->rrr) * 18.0f) * f;

#if defined(BUILD_JP_VERSION)
            pscr->vtw[0] = (pdef->sss * fx - pdef->ccc * fy) + 2048.0f - 0.5f;
            pscr->vtw[1] = (pdef->ccc * fx + pdef->sss * fy) * 0.5f + 2048.0f - 0.5f;
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            pscr->vtw[0] = (pdef->sss * fx - pdef->ccc * fy) + 2048.0f;
            pscr->vtw[1] = (pdef->ccc * fx + pdef->sss * fy) * 0.5f + 2048.0f;
#endif
            pscr->vtw[2] = 0.0f;
            pscr->vtw[3] = 0.0f;
        }
    }

    add = 0.02f;

    if (stop_effects == 0)
    {
        r = r + add <= (PI * 2) ? r + add : (r + add) - (PI * 2);
#if defined(MATCHING_DECOMP)
        // HACK: force loading of a0 and a1 before a2 for MakeScrDeformPacket
        tex0 = SCE_GS_SET_TEX0(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
#endif
    }

#if !defined(MATCHING_DECOMP)
    tex0 = SCE_GS_SET_TEX0(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
#endif
    MakeScrDeformPacket(vnumw - 1, vnumh - 1, tex0, scrdef, alp);

    eff_deform.pass = 1;
    eff_deform.type = type;
}

void SubContrast2(u_char col, u_char alp)
{
    SPRITE_DATA sd = {
        .g_GsTex0 = {
            .TBP0 = 0x1a40,
            .TBW = 10,
            .PSM = 0,
            .TW = 10,
            .TH = 8,
            .TCC = 0,
            .TFX = 0,
            .CBP = 0,
            .CPSM = 0,
            .CSM = 0,
            .CSA = 0,
            .CLD = 1,
        },
        .g_nTexSizeW = 640,
        .g_nTexSizeH = 224,
        .g_bMipmapLv = 0,
        .g_GsMiptbp1 = 0,
        .g_GsMiptbp2 = 0,
        .pos_x = -320.5f,
        .pos_y = -224.5f,
        .pos_z = 0,
        .size_w = 640.0f,
        .size_h = 448.0f,
        .scale_w = 1.0f,
        .scale_h = 1.0f,
        .clamp_u = 0,
        .clamp_v = 0,
        .rot_center = 1,
        .angle = 0.0f,
        .r = 0x80,
        .g = 0x80,
        .b = 0x80,
        .alpha = 0x80,
        .mask = 0,
    };
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1(1, 0, SCE_GS_NEAREST, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GREATER, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL),
        .clamp = 0,
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 342, SCE_GIF_PACKED, 5),
    };

    sd.r = col;
    sd.g = col;
    sd.b = col;

    sd.alpha = alp;

    sd.g_GsTex0.TBP0 = (sys_wrk.count & 1) * 0x8c0;

    sd.pos_x = -320.5f;
    sd.pos_y = -224.5f;

    sd.pos_z = 0x0fffffff - 0x20;

    sd.size_w = 640.0f;
    sd.size_h = 448.0f;

    SetTexDirectS2(0, &sd, &de, 1);
}

void SetContrast2(EFFECT_CONT *ec)
{
    SubContrast2(ec->dat.uc8[2], ec->dat.uc8[3]);

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SubContrast3(u_char col, u_char alp)
{
    SPRITE_DATA sd = {
        .g_GsTex0 = {
            .TBP0 = 0x1a40,
            .TBW = 10,
            .PSM = 0,
            .TW = 10,
            .TH = 8,
            .TCC = 0,
            .TFX = 0,
            .CBP = 0,
            .CPSM = 0,
            .CSM = 0,
            .CSA = 0,
            .CLD = 1,
        },
        .g_nTexSizeW = 640,
        .g_nTexSizeH = 224,
        .g_bMipmapLv = 0,
        .g_GsMiptbp1 = 0,
        .g_GsMiptbp2 = 0,
        .pos_x = -320.5f,
        .pos_y = -224.5f,
        .pos_z = 0,
        .size_w = 640.0f,
        .size_h = 448.0f,
        .scale_w = 1.0f,
        .scale_h = 1.0f,
        .clamp_u = 0,
        .clamp_v = 0,
        .rot_center = 1,
        .angle = 0.0f,
        .r = 0x80,
        .g = 0x80,
        .b = 0x80,
        .alpha = 0x80,
        .mask = 0,
    };
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1(1, 0, SCE_GS_NEAREST, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_ZERO, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GREATER, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL),
        .clamp = 0,
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 0, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 5),
    };

    sd.r = col;
    sd.g = col;
    sd.b = col;

    sd.alpha = alp;

    LocalCopyLtoL((sys_wrk.count & 1) * 0x8c0, 0x1a40);

    sd.g_GsTex0.TBP0 = 0;

    sd.pos_x = -320.5f;
    sd.pos_y = -224.5f;

    sd.pos_z = 0x0fffffff - 0x320;

    sd.size_w = 640.0f;
    sd.size_h = 448.0f;

    SetTexDirectS2(0x320, &sd, &de, 1);

    sd.pos_z = 0x0fffffff - 0x310;

    SetTexDirectS2(0x310, &sd, &de, 1);
}

void SetContrast3(EFFECT_CONT *ec)
{
    SubContrast3(ec->dat.uc8[2], ec->dat.uc8[3]);

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SubNega(u_char r, u_char g, u_char b, u_char alp, u_char alp2)
{
    SPRITE_DATA sd = {
        .g_GsTex0 = {
            .TBP0 = 0x1a40,
            .TBW = 10,
            .PSM = 0,
            .TW = 10,
            .TH = 8,
            .TCC = 0,
            .TFX = 0,
            .CBP = 0,
            .CPSM = 0,
            .CSM = 0,
            .CSA = 0,
            .CLD = 1,
        },
        .g_nTexSizeW = 640,
        .g_nTexSizeH = 224,
        .g_bMipmapLv = 0,
        .g_GsMiptbp1 = 0,
        .g_GsMiptbp2 = 0,
        .pos_x = -320.5f,
        .pos_y = -224.5f,
        .pos_z = 0,
        .size_w = 640.0f,
        .size_h = 448.0f,
        .scale_w = 1.0f,
        .scale_h = 1.0f,
        .clamp_u = 0,
        .clamp_v = 0,
        .rot_center = 1,
        .angle = 0.0f,
        .r = 0x80,
        .g = 0x80,
        .b = 0x80,
        .alpha = 0x80,
        .mask = 0,
    };
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1(1, 0, SCE_GS_NEAREST, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_ZERO, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GREATER, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL),
        .clamp = 0,
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 0, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 5),
    };

    LocalCopyLtoL((sys_wrk.count & 1) * 0x8c0, 0x1a40);

    sd.r = r;
    sd.g = g;
    sd.b = b;

    sd.alpha = alp;

    sd.g_GsTex0.TBP0 = 0;

    sd.pos_x = -320.5;
    sd.pos_y = -224.5;

    sd.pos_z = 0x0fffffff - 0x320;

    sd.size_w = 640.0f;
    sd.size_h = 448.0f;

    SetTexDirectS2(0x320, &sd, &de, 1);

    sd.r = 0x80;
    sd.g = 0x80;
    sd.b = 0x80;

    sd.alpha = alp2 << 0x18 < 0 ? 0 : 0x80 - alp2;

    sd.g_GsTex0.TBP0 = 0x1a40;

    sd.pos_z = 0x0fffffff - 0x310;

    de.alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    de.prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 5);

    SetTexDirectS2(0x310, &sd, &de, 1);
}

void SetNega(EFFECT_CONT *ec)
{
    u_char col;
    u_char alp;
    u_char alp2;

    if (ec->dat.uc8[1] & 4)
    {
        col = ec->dat.uc8[2];
        alp = ec->dat.uc8[3];
        alp2 = 0;

        switch (ec->flow)
        {
        case 0:
            alp2 = (ec->cnt * 128) / ec->in;

            if (ec->in <= ec->cnt++)
            {
                ec->cnt = 0;
                ec->flow = ec->keep == 0 ? (ec->out != 0 ? 2 : 3) : 1;
            }
        break;
        case 1:
            alp2 = 0x80;

            if (ec->keep <= ec->cnt++)
            {
                ec->cnt = 0;
                ec->flow = ec->out != 0 ? 2 : 3;
            }
        break;
        case 2:
            alp2 = ((ec->out - ec->cnt) * 128) / ec->out;

            if (ec->out <= ec->cnt++)
            {
                ec->cnt = 0;
                ec->flow = 3;
            }
        break;
        case 3:
            ResetEffects(ec);
            alp2 = 0;
        break;
        }
    }
    else
    {
        col = ec->dat.uc8[2];
        alp2 = *(u_char *)ec->pnt[0];
        alp = ec->dat.uc8[3];
    }

    SubNega(col, col, col, alp, alp2);

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void* CallNega2(int in, int keep, int out)
{
    static u_char alp = 0x80;

    return SetEffects(0xc, 4, 64, 196, in, keep, out);
}

void* CallNega(int time)
{
    return CallNega2(0, time, 0);
}

void SetOverRap(EFFECT_CONT *ec)
{
    static float cx = 0.0f;
    static float cy = 0.0f;
    static float cz = 0.0f;
    static float alp = 0.0f;
    float x;
    float y;
    float z;
    float fn;
    int fl;
    int ret;
    SPRITE_DATA sd;
    DRAW_ENV de;

    overlap_passflg[0] = 1;

    x = camera.p[0];
    y = camera.p[1];
    z = camera.p[2];

    ret = 0;

    fl = cx - camera.p[0];

    if (__builtin_abs(fl) > 128.0f)
    {
        ret = 1;
    }
    else
    {
        fl = cy - camera.p[1];

        if (__builtin_abs(fl) > 128.0f)
        {
            ret = 1;
        }
        else
        {
            fl = cz - camera.p[2];

            if (__builtin_abs(fl) > 128.0f)
            {
                ret = 1;
            }
        }
    }

    cx = x;
    cy = y;
    cz = z;

    if ((ret != 0 && alp <= 0.0f) || overlap_passflg[0] != overlap_passflg[1])
    {
        LocalCopyLtoB2(1, (sys_wrk.count & 1) * 0x8c0);

        alp = 128.0f;
    }

    if (0.0f < alp)
    {
        LocalCopyBtoL(1,0x1a40);

        sd = (SPRITE_DATA){
            .g_GsTex0 = {
                .TBP0 = 0x1A40,
                .TBW = 10,
                .PSM = 0,
                .TW = 10,
                .TH = 8,
                .TCC = 0,
                .TFX = 0,
                .CBP = 0,
                .CPSM = 0,
                .CSM = 0,
                .CSA = 0,
                .CLD = 1,
            },
            .g_nTexSizeW = 640,
            .g_nTexSizeH = 224,
            .g_bMipmapLv = 0,
            .g_GsMiptbp1 = 0,
            .g_GsMiptbp2 = 0,
            .pos_x = -320.5f,
            .pos_y = -224.5f,
            .pos_z = 0,
            .size_w = 640.0f,
            .size_h = 448.0f,
            .scale_w = 1.0f,
            .scale_h = 1.0f,
            .clamp_u = 0,
            .clamp_v = 0,
            .rot_center = 0,
            .angle = 0.0f,
            .r = 0x80,
            .g = 0x80,
            .b = 0x80,
            .alpha = 0x80,
            .mask = 0,
        };

        de = (DRAW_ENV){
            .tex1 = SCE_GS_SET_TEX1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
            .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, (u_char)alp),
            .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
            .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS),
            .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
            .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 5),
        };

        sd.alpha = alp;

        SetTexDirectS2(0x200, &sd, &de, 0);
    }

    if (ec->dat.uc8[2] != 0)
    {
        fn = 128.0f / ec->dat.uc8[2];
    }
    else
    {
        fn = 8.0f;
    }

    alp = (alp - fn > 0.0f) ? alp - fn : 0.0f;

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SetForcusDepth(EFFECT_CONT *ec)
{
    int i;
    int zi[2];
    float bai[2] = { 1.5f, 2.2f };
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0FVECTOR vt = { 0.0f, 0.0f, 0.0f, 1.0f };
    sceVu0FVECTOR vtw;
    sceVu0FVECTOR vtww;
    sceVu0IVECTOR ivec;

    for (i = 0; i < 2; i++)
    {
        vtww[0] = (camera.i[0] - camera.p[0]) * bai[i] + camera.p[0];
        vtww[1] = (camera.i[1] - camera.p[1]) * bai[i] + camera.p[1];
        vtww[2] = (camera.i[2] - camera.p[2]) * bai[i] + camera.p[2];
        vtww[3] = 1.0f;

        sceVu0UnitMatrix(wlm);

        wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;

        sceVu0TransMatrix(wlm, wlm, vtww);
        sceVu0MulMatrix(slm, SgWSMtx, wlm);
        sceVu0RotTransPers(ivec, slm, vt, 0);

        zi[i] = ivec[2];
    }

    LocalCopyLtoL((sys_wrk.count & 1) * 0x8c0, 0x1a40);

    SetScreenDSlide(0x1a40, 1, 0x40, 0.0f, 1.0f, zi[1]);
    SetScreenDSlide(0x1a40, 1, 0x28, 0.0f, 1.0f, zi[0]);

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SetForcusDepth2(EFFECT_CONT *ec)
{
    int zi[4];
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0FVECTOR vt = { 0.0f, 0.0f, 0.0f, 1.0f };
    sceVu0FVECTOR vtw;
    sceVu0FVECTOR vtww;
    float bai[4] = { 0.5f, 0.7f, 1.5f, 2.2f };
    static float ff = 0.0f;
    sceVu0IVECTOR ivec;

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void MakeRDither3()
{
    u_char pat[16384];
    u_int pal[256];
    int i;
    static sceGsLoadImage gs_limage1;
    static sceGsLoadImage gs_limage2;

        for (i = 0; i < 16384; i++)
    {
        pat[i] = (int)(64.0f * VER_RAND());
    }

    for (i = 0; i < 256; i++)
    {
        pal[i] = (u_char)(128.0f * VER_RAND());
        pal[i] = i << 24 | pal[i] << 16 | pal[i] <<  8 | pal[i];
    }

    sceGsSetDefLoadImage(&gs_limage1, 0x37fc, 2, 19, 0, 0, 128, 128);
    sceGsSetDefLoadImage(&gs_limage2, 0x383c, 1, 0, 0, 0, 16, 32);

    FlushCache(0);

    sceGsExecLoadImage(&gs_limage1, (u_long128 *)pat);
    sceGsExecLoadImage(&gs_limage2, (u_long128 *)pal);

    sceGsSyncPath(0, 0);
}

void SubDither3(int type, float alp, float spd, u_char alpmx, u_char colmx)
{
    static sceVu0FVECTOR old_cam_i = { 0.0f, 0.0f, 0.0f, 0.0f };
    static float cnf = 0.0f;
    static float cx = 0.0f;
    static float cy = 0.0f;
    static int fl = 1;
    float tx;
    float ty;
    float otx;
    float oty;
    float mvx;
    float mvy;
    SPRITE_DATA sd = {
        .g_GsTex0 = {
            .TBP0 = 0x37FC,
            .TBW = 2,
            .PSM = 19,
            .TW = 7,
            .TH = 7,
            .TCC = 1,
            .TFX = 0,
            .CBP = 0x383C,
            .CPSM = 0,
            .CSM = 0,
            .CSA = 0,
            .CLD = 1,
        },
        .g_nTexSizeW = 128,
        .g_nTexSizeH = 128,
        .g_bMipmapLv = 0,
        .g_GsMiptbp1 = 0,
        .g_GsMiptbp2 = 0,
        .pos_x = -320.5f,
        .pos_y = -224.5f,
        .pos_z = 0,
        .size_w = 320.0f,
        .size_h = 448.0f,
        .scale_w = 1.0f,
        .scale_h = 1.0f,
        .clamp_u = 0,
        .clamp_v = 0,
        .rot_center = 0,
        .angle = 0.0f,
        .r = 0x80,
        .g = 0x80,
        .b = 0x80,
        .alpha = 0x80,
        .mask = 0,
    };
    SPRITE_DATA sd2 = {
        .g_GsTex0 = {
            .TBP0 = 0x37FC,
            .TBW = 2,
            .PSM = 19,
            .TW = 7,
            .TH = 7,
            .TCC = 1,
            .TFX = 0,
            .CBP = 0x383C,
            .CPSM = 0,
            .CSM = 0,
            .CSA = 0,
            .CLD = 1,
        },
        .g_nTexSizeW = 128,
        .g_nTexSizeH = 128,
        .g_bMipmapLv = 0,
        .g_GsMiptbp1 = 0,
        .g_GsMiptbp2 = 0,
        .pos_x = -320.5f,
        .pos_y = -224.5f,
        .pos_z = -4096,
        .size_w = 640.0f,
        .size_h = 448.0f,
        .scale_w = 1.0f,
        .scale_h = 1.0f,
        .clamp_u = 0,
        .clamp_v = 0,
        .rot_center = 0,
        .angle = 0.0f,
        .r = 0x80,
        .g = 0x80,
        .b = 0x80,
        .alpha = 0x80,
        .mask = 0,
    };
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1(1, 0, SCE_GS_NEAREST, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 5),
    };
    DRAW_ENV de2 = {
        .tex1 = SCE_GS_SET_TEX1(1, 0, SCE_GS_NEAREST, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CS, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CS, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 5),
    };
    u_char pat[16384];
    u_int pal[256];
    int i;
    static u_char oalp = 0;
    static u_char ocol = 0;
    static int numf = 0;
    static sceGsLoadImage gs_limage1;
    static sceGsLoadImage gs_limage2;

    if (eff_dith_off != 0)
    {
        return;
    }

    if (plyr_wrk.mode == 1)
    {
        if (fl == 0)
        {
            Vu0CopyVector(old_cam_i, camera.i);
        }

        fl = 1;
    }
    else
    {
        if (fl == 1 || CamChangeCheck())
        {
            Vu0CopyVector(old_cam_i, camera.i);
        }

        fl = 0;
    }

    switch(type)
    {
    case 1:
        de2.alpha = 0x44;
    break;
    case 2:
        de2.alpha = 0x48;
    break;
    case 3:
        de2.alpha = 0x41;
    break;
    case 4:
        sd2.r = 0x80;
        sd2.g = 0x0;
        sd2.b = 0x80;

        de2.alpha = 0x41;
    break;
    case 5:
        sd2.r = 0x80;
        sd2.g = 0x80;
        sd2.b = 0x0;

        de2.alpha = 0x41;
    break;
    case 6:
        de2.alpha = 0x49;
    break;
    case 7:
        de2.alpha = 0x42;
    break;
    }

    if (oalp == alpmx)
    {
        if (ocol == colmx && numf != 0)
        {
            numf = 0;

            for (i = 0; i < 16384; i++)
            {
                pat[i] = (int)((float)alpmx * VER_RAND());
            }

            for (i = 0; i < 256; i++)
            {
                pal[i] = (u_char)((float)colmx * VER_RAND());
                pal[i] = i << 24 | pal[i] << 16 | pal[i] <<  8 | pal[i];
            }

            sceGsSetDefLoadImage(&gs_limage1, 0x37fc, 2, 0x13, 0, 0, 0x80, 0x80);
            sceGsSetDefLoadImage(&gs_limage2, 0x383c, 1, 0, 0, 0, 0x10, 0x20);

            FlushCache(0);

            sceGsExecLoadImage(&gs_limage1, (u_long128 *)pat);
            sceGsExecLoadImage(&gs_limage2, (u_long128 *)pal);

            sceGsSyncPath(0, 0);

            WaitFrameTop(0);
        }
    }

    if (oalp != alpmx || ocol != colmx)
    {
        numf = 1;
    }

    oalp = alpmx;
    ocol = colmx;

    GetCamI2DPos(camera.i, &tx, &ty);
    GetCamI2DPos(old_cam_i, &otx, &oty);
    sceVu0CopyVector(old_cam_i, camera.i);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    if (isnan(tx) || isnan(ty) || isnan(otx) || isnan(oty))
    {
        mvx = 0.0f;
        mvy = 0.0f;
    }
    else
#endif
    {
        mvx = (tx - otx) * 0.2f;
        mvy = (ty - oty) * 0.1f;
    }

    if (stop_effects == 0)
    {
        cx += mvx;
        cx -= (int)(cx / 128.0f) * 128;

        cy += mvy;
        cy -= (int)(cy / 128.0f) * 128;

        while (cx > 128.0f) cx -= 128.0f;
        while (cx <   0.0f) cx += 128.0f;

        while (cy > 128.0f) cy -= 128.0f;
        while (cy <   0.0f) cy += 128.0f;
    }

    sd2.clamp_u = (int)((cx + 640.0f) * 16.0f) << 16 | (int)(cx * 16.0f);
    sd2.clamp_v = (int)((cy + 512.0f) * 16.0f) << 16 | (int)(cy * 16.0f);
    sd2.alpha = (VER_SINF((cnf * PI) / 180.0f) + 1.0f) * alp;

    SetTexDirectS2(-2, &sd2, &de2, 0);

    sd2.clamp_u = (int)((cx + 704.0f) * 16.0f) << 16 | (int)((cx + 64.0f) * 16.0f);
    sd2.clamp_v = (int)((cy + 512.0f) * 16.0f) << 16 | (int)(cy * 16.0f);
    sd2.alpha = (VER_SINF(((cnf + 120.0f) * PI) / 180.0f) + 1.0f) * alp;

    SetTexDirectS2(-2, &sd2, &de2, 0);

    sd2.clamp_u = (int)((cx + 640.0f) * 16.0f) << 16 | (int)(cx * 16.0f);
    sd2.clamp_v = (int)((cy + 576.0f) * 16.0f) << 16 | (int)((cy + 64.0f) * 16.0f);
    sd2.alpha = (VER_SINF(((cnf + 240.0f) * PI) / 180.0f) + 1.0f) * alp;

    SetTexDirectS2(-2, &sd2, &de2, 0);

    if (stop_effects == 0)
    {
        cnf += spd;
    }
}

void SetDither3(EFFECT_CONT *ec)
{
    u_char alpmx;
    u_char colmx;
    int type;
    float spd;
    float alp;

    type = ec->dat.uc8[2];
    alp = ec->dat.fl32[2];
    spd = ec->dat.fl32[3];
    alpmx = ec->dat.uc8[3];
    colmx = ec->dat.uc8[4];

    if (ec->dat.uc8[1] & 4)
    {
        alp = 0.0f;

        switch (ec->flow)
        {
        case 0:
            alp = (ec->dat.fl32[2] * ec->cnt) / ec->in;

            if (ec->in <= ec->cnt++)
            {
                ec->cnt = 0;
                ec->flow = ec->keep == 0 ? ec->out != 0 ? 2 : 3 : 1;
            }
        break;
        case 1:
            alp = ec->dat.fl32[2];

            if (ec->keep <= ec->cnt++)
            {
                ec->cnt = 0;
                ec->flow = ec->out != 0 ? 2 : 3;
            }
        break;
        case 2:
            alp = (ec->dat.fl32[2] * (ec->out - ec->cnt)) / ec->out;

            if (ec->out <= ec->cnt++)
            {
                ec->cnt = 0;
                ec->flow = 3;

                ResetEffects(ec);
            }
        break;
        }
    }

    if (type < 8)
    {
        SubDither3(type, alp, spd, alpmx, colmx);
    }
    else
    {
        SubDither4(alp,spd);
    }

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SubDither4(float alp, float spd)
{
    static sceVu0FVECTOR old_cam_i = { 0.0f, 0.0f, 0.0f, 0.0f };
    static float cnf = 0.0f;
    static float cx = 0.0f;
    static float cy = -9908638.0f;
    static int fl = 1;
    float tx;
    float ty;
    float otx;
    float oty;
    float mvx;
    float mvy;
    SPRITE_DATA sd = {
        .g_GsTex0 = {
            .TBP0 = 0x37fc,
            .TBW = 2,
            .PSM = 19,
            .TW = 7,
            .TH = 7,
            .TCC = 1,
            .TFX = 0,
            .CBP = 0x383c,
            .CPSM = 0,
            .CSM = 0,
            .CSA = 0,
            .CLD = 1,
        },
        .g_nTexSizeW = 128,
        .g_nTexSizeH = 128,
        .g_bMipmapLv = 0,
        .g_GsMiptbp1 = 0,
        .g_GsMiptbp2 = 0,
        .pos_x = -320.5f,
        .pos_y = -224.5f,
        .pos_z = 0,
        .size_w = 640.0f,
        .size_h = 448.0f,
        .scale_w = 1.0f,
        .scale_h = 1.0f,
        .clamp_u = 0,
        .clamp_v = 0,
        .rot_center = 0,
        .angle = 0.0f,
        .r = 0x80,
        .g = 0x80,
        .b = 0x80,
        .alpha = 0x80,
        .mask = 0,
    };
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1(1, 0, SCE_GS_NEAREST, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 5),
    };

    cy = (cy - 1024.0f > -9908638.0f) ? cy - 1024.0f : cy;

    if (plyr_wrk.mode == 1)
    {
        if (fl == 0)
        {
            Vu0CopyVector(old_cam_i, camera.i);
        }

        fl = 1;
    }
    else
    {
        if (fl == 1 || CamChangeCheck())
        {
            Vu0CopyVector(old_cam_i, camera.i);
        }

        fl = 0;
    }

    GetCamI2DPos(camera.i, &tx, &ty);
    GetCamI2DPos(old_cam_i, &otx, &oty);
    sceVu0CopyVector(old_cam_i,camera.i);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    if (isnan(tx) || isnan(ty) || isnan(otx) || isnan(oty))
    {
        mvx = 0.0f;
        mvy = 0.0f;
    }
    else
#endif
    {
        mvx = (tx - otx) * 0.2f;
        mvy = (ty - oty) * 0.1f;
    }

    cx = (cx + mvx);
    cy = (cy + mvy);

    cx = ((cx + mvx) > 128.0f) ? (cx + mvx) - 128.0f : (((cx + mvx) < 0.0f) ? (cx + mvx) + 128.0f : (cx + mvx));
    cy = ((cy + mvy) > 128.0f) ? (cy + mvy) - 128.0f : (((cy + mvy) < 0.0f) ? (cy + mvy) + 128.0f : (cy + mvy));

                    // Line 4001
    sd.clamp_u = (int)((cx + 640.0f) * 16.0f) << 16 | (int)(cx * 16.0f);
    sd.clamp_v = (int)((cy + 512.0f) * 16.0f) << 16 | (int)(cy * 16.0f);
    sd.alpha = (VER_SINF((cnf * PI) / 180.0f) + 1.0f) * alp;

    SetTexDirectS2(-2, &sd, &de, 0);

    sd.clamp_u = (int)((cx + 704.0f) * 16.0f) << 16 | (int)((cx + 64.0f) * 16.0f);
    sd.clamp_v = (int)((cy + 512.0f) * 16.0f) << 16 | (int)(cy * 16.0f);
    sd.alpha = (VER_SINF(((cnf + 120.0f) * PI) / 180.0f) + 1.0f) * alp;

    SetTexDirectS2(-2, &sd, &de, 0);

    sd.clamp_u = (int)((cx + 640.0f) * 16.0f) << 16 | (int)(cx * 16.0f);
    sd.clamp_v = (int)((cy + 576.0f) * 16.0f) << 16 | (int)((cy + 64.0f) * 16.0f);
    sd.alpha = (VER_SINF(((cnf + 240.0f) * PI) / 180.0f) + 1.0f) * alp;

    SetTexDirectS2(-2, &sd, &de, 0);

    if (stop_effects == 0)
    {
        cnf += spd;
    }
}

void SetDither4(EFFECT_CONT *ec)
{
    SubDither4(ec->dat.fl32[2], ec->dat.fl32[3]);

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SubFadeFrame(u_char alp, u_int pri)
{
    SPRITE_DATA sd = {
        .g_GsTex0 = {
            .TBP0 = 0,
            .TBW = 0,
            .PSM = 0,
            .TW = 0,
            .TH = 0,
            .TCC = 0,
            .TFX = 0,
            .CBP = 0,
            .CPSM = 0,
            .CSM = 0,
            .CSA = 0,
            .CLD = 0,
        },
        .g_nTexSizeW = 0,
        .g_nTexSizeH = 0,
        .g_bMipmapLv = 0,
        .g_GsMiptbp1 = 0,
        .g_GsMiptbp2 = 0,
        .pos_x = -320.5f,
        .pos_y = -224.5f,
        .pos_z = 0,
        .size_w = 640.0f,
        .size_h = 448.0f,
        .scale_w = 1.0f,
        .scale_h = 1.0f,
        .clamp_u = 0,
        .clamp_v = 0,
        .rot_center = 0,
        .angle = 0.0f,
        .r = 0x80,
        .g = 0x80,
        .b = 0x80,
        .alpha = 0x80,
        .mask = 0,
    };
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, 0, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 342, SCE_GIF_PACKED, 5),
    };

    sd.g_GsTex0 = *(sceGsTex0 *)&effdat[12].tex0;

    sd.g_nTexSizeW = effdat[12].w;
    sd.g_nTexSizeH = effdat[12].h;

    sd.pos_z = 0xfffffff - pri;

    sd.alpha = alp;

    SetTexDirectS2(pri, &sd, &de, 0);
}

void SetFadeFrame(EFFECT_CONT *ec)
{
    SubFadeFrame(ec->dat.uc8[2], ec->dat.ui32[1]);

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
void ChangeMAGATOKI2(int sw)
{
    if (sw != 0)
    {
        magatoki_mode = 1;
        ChangeMonochrome(1);
    }
    else
    {
        magatoki_mode = 0;
        ChangeMonochrome(0);
    }
}

void SetMAGATOKI2()
{
    EFFECT_CONT *ecm;
    static u_char alpr = 0x39;

    if (change_efbank != 0)
    {
        ecm = efcnt;
    }
    else
    {
        ecm = efcnt_cnt;
    }

    if (magatoki_mode != 0)
    {
        ResetEffects(&ecm[3]);
        ResetEffects(&ecm[4]);
        ResetEffects(&ecm[5]);
        ResetEffects(&ecm[6]);
        ResetEffects(&ecm[13]);
        ResetEffects(&ecm[15]);
        ResetEffects(&ecm[2]);
        ResetEffects(&ecm[9]);

        if (plyr_wrk.mode != 0x1)
        {
            SetEffects(EF_BLUR_N, 1, &alpr, 1015, 1800, 320.0f, 112.0f);
            SetEffects(EF_DEFORM, 1, 2, 12);
            SetEffects(EF_NCONTRAST, 1, 70, 70);
            SetEffects(EF_DITHER, 1, 2, 24.0f, 8.0f, 106 ,101);
            SetEffects(EF_FADEFRAME, 1, 30, 0x80000);
        }
        else
        {
            SetEffects(EF_DEFORM, 1, 2, 12);
            SetEffects(EF_NCONTRAST, 1, 70, 70);
            SetEffects(EF_DITHER, 1, 2, 24.0f, 8.0f, 106, 101);
            SetEffects(EF_FADEFRAME, 1, 30, 0x80000);
        }
    }
}
#endif

void SetMAGATOKI(EFFECT_CONT *ec)
{
    static int cnt1;
    u_char *flag;
    float per1;
    float mbalp;
    float mbscl;
    float mbrot;
    float mccol;
    float mcalp;
    float mdalp;
    float mdspd;
    float mdfrt;
    float mdfal;
    int sec1_1;
    int sec1_2;
    float balp[3] = {  57.0f,  90.0f,  57.0f };
    float bscl[3] = {   1.0f, 1.007f, 1.015f };
    float brot[3] = { 180.0f, 180.3f, 180.0f };
    float ccol[3] = {  70.0f,  70.0f,  70.0f };
    float calp[3] = {  70.0f, 140.0f,  70.0f };
    float dalp[3] = {   0.0f, 128.0f,  24.0f };
    float dspd[3] = {   8.0f,   8.0f,   8.0f };
    float dfrt[3] = {   0.0f,   0.0f,  12.0f };
    float dfal[3] = {   0.0f,   0.0f, 128.0f };

    sec1_1 = ec->in;
    sec1_2 = ec->out;

    flag = ec->pnt[0];

    switch(ec->flow)
    {
    case 0:
        shibata_set_off = 1;

        cnt1 = 0;

        ec->flow = 1;
    case 1:
        per1 = cnt1 / (float)sec1_1;

        mbalp = balp[0] + (balp[1] - balp[0]) * per1;
        mbscl = bscl[0] + (bscl[1] - bscl[0]) * per1;
        mbrot = brot[0] + (brot[1] - brot[0]) * per1;
        mccol = ccol[0] + (ccol[1] - ccol[0]) * per1;
        mcalp = calp[0] + (calp[1] - calp[0]) * per1;
        mdalp = dalp[0] + (dalp[1] - dalp[0]) * per1;
        mdspd = dspd[0] + (dspd[1] - dspd[0]) * per1;
        mdfrt = dfrt[0] + (dfrt[1] - dfrt[0]) * per1;
        mdfal = dfal[0] + (dfal[1] - dfal[0]) * per1;

        cnt1++;

        if (cnt1 >= sec1_1)
        {
            ec->flow = 2;
        }
    break;
    case 2:
        ec->flow = *flag != 0 ? 3 : 2;

        mbalp = balp[1];
        mbscl = bscl[1];
        mbrot = brot[1];
        mccol = ccol[1];
        mcalp = calp[1];
        mdalp = dalp[1];
        mdspd = dspd[1];
        mdfrt = dfrt[1];
        mdfal = dfal[1];
    break;
    case 3:
        cnt1 = 0;

        ChangeMonochrome(1);

        ec->flow = 4;
    case 4:
        per1 = cnt1 / (float)sec1_2;

        mbalp = balp[1] + (balp[2] - balp[1]) * per1;
        mbscl = bscl[1] + (bscl[2] - bscl[1]) * per1;
        mbrot = brot[1] + (brot[2] - brot[1]) * per1;
        mccol = ccol[1] + (ccol[2] - ccol[1]) * per1;
        mcalp = calp[1] + (calp[2] - calp[1]) * per1;
        mdalp = dalp[1] + (dalp[2] - dalp[1]) * per1;
        mdspd = dspd[1] + (dspd[2] - dspd[1]) * per1;
        mdfrt = dfrt[1] + (dfrt[2] - dfrt[1]) * per1;
        mdfal = dfal[1] + (dfal[2] - dfal[1]) * per1;

        cnt1++;

        if (sec1_2 <= cnt1)
        {
            *flag = 2;

            ec->flow = 5;
        }
    break;
    case 5:
        mbalp = balp[2];
        mbscl = bscl[2];
        mbrot = brot[2];
        mccol = ccol[2];
        mcalp = calp[2];
        mdalp = dalp[2];
        mdspd = dspd[2];
        mdfrt = dfrt[2];
        mdfal = dfal[2];

        ec->flow = *flag > 2 ? 6 : 5;
    break;
    case 6:
        cnt1 = 0;

        ec->flow = 7;
    case 7:
        per1 = cnt1 / (float)sec1_1;

        mbalp = balp[2] + (balp[1] - balp[2]) * per1;
        mbscl = bscl[2] + (bscl[1] - bscl[2]) * per1;
        mbrot = brot[2] + (brot[1] - brot[2]) * per1;
        mccol = ccol[2] + (ccol[1] - ccol[2]) * per1;
        mcalp = calp[2] + (calp[1] - calp[2]) * per1;
        mdalp = dalp[2] + (dalp[1] - dalp[2]) * per1;
        mdspd = dspd[2] + (dspd[1] - dspd[2]) * per1;
        mdfrt = dfrt[2] + (dfrt[1] - dfrt[2]) * per1;
        mdfal = dfal[2] + (dfal[1] - dfal[2]) * per1;

        cnt1++;

        if (sec1_1 <= cnt1)
        {
            ec->flow = 8;
        }
    break;
    case 8:
        ec->flow = *flag > 3 ? 9 : 8;

        mbalp = balp[1];
        mbscl = bscl[1];
        mbrot = brot[1];
        mccol = ccol[1];
        mcalp = calp[1];
        mdalp = dalp[1];
        mdspd = dspd[1];
        mdfrt = dfrt[1];
        mdfal = dfal[1];
    break;
    case 9:
        cnt1 = 0;

        ChangeMonochrome(0);

        ec->flow = 10;
    case 10:
        per1 = cnt1 / (float)sec1_2;

        mbalp = balp[1] + (balp[0] - balp[1]) * per1;
        mbscl = bscl[1] + (bscl[0] - bscl[1]) * per1;
        mbrot = brot[1] + (brot[0] - brot[1]) * per1;
        mccol = ccol[1] + (ccol[0] - ccol[1]) * per1;
        mcalp = calp[1] + (calp[0] - calp[1]) * per1;
        mdalp = dalp[1] + (dalp[0] - dalp[1]) * per1;
        mdspd = dspd[1] + (dspd[0] - dspd[1]) * per1;
        mdfrt = dfrt[1] + (dfrt[0] - dfrt[1]) * per1;
        mdfal = dfal[1] + (dfal[0] - dfal[1]) * per1;

        cnt1++;

        if (sec1_2 <= cnt1)
        {
            shibata_set_off = 0;

            ec->flow = 11;
        }
    break;
    case 11:
        mbalp = balp[1];
        mbscl = bscl[1];
        mbrot = brot[1];
        mccol = ccol[1];
        mcalp = calp[1];
        mdalp = dalp[1];
        mdspd = dspd[1];
        mdfrt = dfrt[1];
        mdfal = dfal[1];

        ec->flow = 12;
    case 12:
        *flag = 5;

        if (ec->dat.uc8[1] & 8)
        {
            ResetEffects(ec);
        }
    break;
    }

    if (ec->flow < 12)
    {
        if (plyr_wrk.mode != 1)
        {
            SubBlur(1, mbalp, mbscl, mbrot, 320.0f, 112.0f);
            SubDeform(2, mdfrt, mdfal);
            SubContrast2(mccol, mcalp);
            SubDither3(2, mdalp, mdspd, 0x6a, 0x65);
            SubFadeFrame(0x1e, 0x80000);
        }
        else
        {
            SubDeform(2, mdfrt, mdfal);
            SubContrast2(mccol, mcalp);
            SubDither3(2, mdalp, mdspd, 0x6a, 0x65);
            SubFadeFrame(0x1e, 0x80000);
        }
    }
}

u_char SubNowLoading(int fl, int num, float sclx, float scly, float bx, float by, float spd, float rate, float trate)
{
    u_char alpha1[289] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x08, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x20, 0x40, 0x40, 0x40, 0x40, 0x20, 0x20, 0x10, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x20, 0x40, 0x60, 0x80, 0x80, 0x80, 0x60, 0x60, 0x40, 0x20,
        0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x20, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x60,
        0x40, 0x20, 0x10, 0x00, 0x00, 0x01, 0x20, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x60, 0x40, 0x20, 0x00, 0x00, 0x10, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x60, 0x20, 0x00, 0x01, 0x20, 0x60, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x60, 0x40, 0x01, 0x08, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x08, 0x08, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x08, 0x08, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x08, 0x01, 0x40, 0x60, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x60, 0x20, 0x01, 0x00, 0x20, 0x60, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x10, 0x00, 0x00, 0x20, 0x40, 0x60,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x20, 0x01, 0x00, 0x00, 0x10, 0x20,
        0x40, 0x60, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x08, 0x20, 0x40, 0x60, 0x60, 0x80, 0x80, 0x80, 0x60, 0x40, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x10, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x20, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x08, 0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x00,
    };
    u_char alpha2[289] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x08, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x20, 0x40, 0x40, 0x40, 0x40, 0x20, 0x20, 0x10, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x20, 0x40, 0x58, 0x68, 0x68, 0x68, 0x58, 0x58, 0x40, 0x20,
        0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x20, 0x40, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x58,
        0x40, 0x20, 0x10, 0x00, 0x00, 0x01, 0x20, 0x40, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68,
        0x68, 0x58, 0x40, 0x20, 0x00, 0x00, 0x10, 0x40, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68,
        0x68, 0x68, 0x68, 0x58, 0x20, 0x00, 0x01, 0x20, 0x58, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68,
        0x68, 0x68, 0x68, 0x68, 0x58, 0x40, 0x01, 0x08, 0x40, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68,
        0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x40, 0x08, 0x08, 0x40, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68,
        0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x40, 0x08, 0x08, 0x40, 0x68, 0x68, 0x68, 0x68, 0x68,
        0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x40, 0x08, 0x01, 0x40, 0x58, 0x68, 0x68, 0x68,
        0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x58, 0x20, 0x01, 0x00, 0x20, 0x58, 0x68, 0x68,
        0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x40, 0x10, 0x00, 0x00, 0x20, 0x40, 0x58,
        0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x40, 0x20, 0x01, 0x00, 0x00, 0x10, 0x20,
        0x40, 0x58, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x40, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x08, 0x20, 0x40, 0x58, 0x58, 0x68, 0x68, 0x68, 0x58, 0x40, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x10, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x20, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x08, 0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x00,
    };
    int i;
    int j;
    int k;
    int l;
    int m;
    int bak;
    int pnumw;
    int vnumw;
    int wix;
    int pnumh;
    int vnumh;
    int wiy;
    int sx2;
    int sy2;
    int tx[289];
    int ty[289];
    u_int clip[289];
    u_int clpx2;
    u_int clpy2;
    u_int clpz2;
    float f1;
    float f2;
    float f3;
    float f4;
    float bxf;
    float byf;
    float pr11;
    float pr12;
    float pr21;
    float pr22;
    sceVu0FVECTOR vt[289];
    sceVu0IVECTOR vtiw[289];
    sceVu0IVECTOR ivec;
    u_long tex0;
    EFFPOS *pefp;
    EFFINFO2 *pefi;
    float c;

    sx2 = (SCR_WIDTH - 1) * 16;
    sy2 = (SCR_HEIGHT - 1) * 16;

    pnumh = 16;
    pnumw = 16;
    vnumw = 17;
    vnumh = 17;

#if defined(MATCHING_DECOMP)
    if (0/vnumw) { c = 0.0f; } // HACK: fixes codegen
#endif

    c = 1.0f;

    pr11 =  1.2f;
    pr12 = -0.6f;
    pr21 =  0.6f;
    pr22 =  0.3f;

    clpx2 = 0xfd00;
    clpy2 = 0xfd00;
    clpz2 = 0x0fffffff;

    tex0 = SCE_GS_SET_TEX0(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);

    if (fl != 0)
    {
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        SetVURand(rand() / (float)RAND_MAX);
#endif

        for (j = 0; j < 2; j++)
        {
            pefi = &efi[j];

            for (i = 0; i < vnumw*vnumh; i++)
            {
                wix = i % vnumw;
                wiy = i / vnumw;

                if (wix == 0)
                {
                    pefi->ep[i].h = pr11 * VER_RAND() + pr12;
                }
                else
                {
                    pefi->ep[i].h = pefi->ep[i - 1].h + pr21 * VER_RAND() - pr22;
                }

                if (wiy == 0)
                {
                    pefi->ep[i].r = pr11 * VER_RAND() + pr12;
                }
                else
                {
                    pefi->ep[i].r = pefi->ep[i - vnumw].r + pr21 * VER_RAND() - pr22;
                }
            }
        }
    }

    bxf = bx + 2048.0f;
    byf = by + 2048.0f;

    f3 = (sclx * 2) * 8.0f;
    f4 = (scly * 1) * 8.0f;

    for (i = 0; i < vnumw*vnumh; i++)
    {
        wix = i % vnumw;
        wiy = i / vnumw;

        f1 = (int)(wix) * (sclx * 2);
        f2 = (int)(wiy) * (scly * 1);

        vt[i][0] = (f1 - f3) * trate;
        vt[i][1] = (f2 - f4) * trate;

        tx[i] = (int)((f1 - f3 + bx + 320.0f) * 16.0f);
        ty[i] = (int)((f2 - f4 + by + 112.0f) * 16.0f);

        tx[i] = (tx[i] < 0) ? 0 : ((sx2 < tx[i]) ? sx2 : tx[i]);
        ty[i] = (ty[i] < 0) ? 0 : ((sy2 < ty[i]) ? sy2 : ty[i]);

        vt[i][0] = f1 - f3;
        vt[i][1] = f2 - f4;
        vt[i][2] = 0.0f;
        vt[i][3] = 1.0f;
    }

    pefi = &efi[num];

    for (i = 0; i < vnumw*vnumh; i++)
    {
        wix = i / vnumw;
        wiy = i % vnumw;

        pefp = &pefi->ep[i];

        pefp->x = VER_SINF(((pefi->r + wix * 32.0f) * PI) / 180.0f) * pefp->r * rate;
        pefp->y = VER_SINF(((pefi->r + wiy * 32.0f) * PI) / 180.0f) * pefp->h * rate;

        if (pefp->y >= 0.0f && pefp->oy < 0.0f)
        {
            if (wiy != 0)
            {
                pefp->h = pefi->ep[i - 1].h + pr21 * (1.0f * VER_RAND()) - pr22;
            }
            else
            {
                pefp->h = pr11 * (1.0f * VER_RAND()) + pr12;
            }
        }

        if (pefp->x >= 0.0f && pefp->ox < 0.0f)
        {
            if (wix != 0)
            {
                pefp->r = pefi->ep[i - vnumw].r + pr21 * (1.0f * VER_RAND()) - pr22;
            }
            else
            {
                pefp->r = pr11 * (1.0f * VER_RAND()) + pr12;
            }
#if defined(MATCHING_DECOMP) && defined(BUILD_JP_VERSION)
            asm(""); // HACK: fixes codegen
#endif
        }
        pefp->ox = pefp->x;
        pefp->oy = pefp->y;

        vt[i][0] += pefp->x;
        vt[i][1] += pefp->y;
    }

    pefi->r = ((pefi->r + spd * 2) <= 360.0f) ? pefi->r + spd * 2 : pefi->r + spd * 2 - 360.0f;

    for (i = 0; i < vnumw*vnumh; i++)
    {
        vtiw[i][0] = (int)((vt[i][0] + bxf) * 16.0f);
        vtiw[i][1] = (int)((vt[i][1] + byf) * 16.0f);
        vtiw[i][2] = (int)((vt[i][2] * 16.0f)) + (0x0fffffff - 0xa000);
        vtiw[i][3] = 1;
    }

    for (i = 0; i < vnumw*vnumh; i++)
    {
        clip[i] = 1;

        if ((vtiw[i][0] >= 0 && vtiw[i][0] < 0x300) || vtiw[i][0] > clpx2)
        {
            clip[i] = 0;
        }

        if ((vtiw[i][1] >= 0 && vtiw[i][1] < 0x300) || vtiw[i][1] > clpy2)
        {
            clip[i] = 0;
        }

        if (vtiw[i][2] == 0 || vtiw[i][2] > clpz2)
        {
            clip[i] = 0;
        }
    }

    Reserve2DPacket(0x1000);

    bak = ndpkt;

    pbuf[ndpkt++].ul128 = (u_long128)0;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(7, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    pbuf[ndpkt].ul64[0] = tex0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_CLAMP_1(SCE_GS_CLAMP, SCE_GS_CLAMP, 0, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_CLAMP_1;

    m = (pnumw + 1) * pnumh;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(m, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 1, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 6);
    pbuf[ndpkt++].ul64[1] = 0
        | SCE_GS_RGBAQ << (4 * 0)
        | SCE_GS_UV    << (4 * 1)
        | SCE_GS_XYZF2 << (4 * 2)
        | SCE_GS_RGBAQ << (4 * 3)
        | SCE_GS_UV    << (4 * 4)
        | SCE_GS_XYZF2 << (4 * 5);

#if defined(MATCHING_DECOMP)
    if (0/m) {} // HACK: fixes codegen
#endif

    for (i = 0; i < m; i++)
    {
        l = i + pnumh;
        j = i + vnumw;

        k = i % (pnumw + 1) ? clip[i - 1] + clip[l] + clip[i] : 0;

        pbuf[ndpkt].ui32[0] = 0x80;
        pbuf[ndpkt].ui32[1] = 0x80;
        pbuf[ndpkt].ui32[2] = 0x80;
        pbuf[ndpkt++].ui32[3] = (u_char)(alpha1[i] * c);

        pbuf[ndpkt].ui32[0] = tx[i];
        pbuf[ndpkt].ui32[1] = ty[i];
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt++].ui32[3] = 0;

        pbuf[ndpkt].ui32[0] = vtiw[i][0];
        pbuf[ndpkt].ui32[1] = vtiw[i][1];
        pbuf[ndpkt].ui32[2] = vtiw[i][2];
        pbuf[ndpkt++].ui32[3] = k <= 2 ? 0x8000 : 0;

        k = i % (pnumw + 1) ? clip[j - 1] + clip[i] + clip[j] : 0;

        pbuf[ndpkt].ui32[0] = 0x80;
        pbuf[ndpkt].ui32[1] = 0x80;
        pbuf[ndpkt].ui32[2] = 0x80;
        pbuf[ndpkt++].ui32[3] = (u_char)(alpha1[j] * c);

        pbuf[ndpkt].ui32[0] = tx[j];
        pbuf[ndpkt].ui32[1] = ty[j];
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt++].ui32[3] = 0;

        pbuf[ndpkt].ui32[0] = vtiw[j][0];
        pbuf[ndpkt].ui32[1] = vtiw[j][1];
        pbuf[ndpkt].ui32[2] = vtiw[j][2];
        pbuf[ndpkt++].ui32[3] = k <= 2 ? 0x8000 : 0;
    }

    pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;

    // missing return
}

void InitNowLoading()
{
    now_loading_flow = 0xff;
}

void SetNowLoadingON()
{
    now_loading_flow = 0;
}

void SetNowLoadingOFF()
{
    now_loading_flow = 3;
}

int SetNowLoading()
{
    SPRT_DAT test[2] = {
        {
            .tex0 = SCE_GS_SET_TEX0(0x2300, 8, SCE_GS_PSMT8, 9, 8, 1, SCE_GS_MODULATE, 0x2540, SCE_GS_PSMCT32, 0, 0, 1),
            .u = 0,
            .v = 0,
            .w = 512,
            .h = 256,
            .x = 64,
            .y = 96,
            .pri = 0x64000,
            .alpha = 0x80,
        },
        {
            .tex0 = SCE_GS_SET_TEX0(0x2500, 2, SCE_GS_PSMT8, 7, 7, 1, SCE_GS_MODULATE, 0x2544, SCE_GS_PSMCT32, 0, 0, 1),
            .u = 0,
            .v = 0,
            .w = 128,
            .h = 128,
            .x = 450,
            .y = 300,
            .pri = 0x5a000,
            .alpha = 0x80,
    }
    };
    DISP_SPRT ds;
    float out;
    float alp;
    static float bx[3] = { -200.0f, 200.0f, 450.0f };
    static float by[3] = {    0.0f,   0.0f,  300.f };
    static int cnt = 0;
    float sclx = 10.0f;
    float scly = 10.0f;

    out = 15.0f;
    alp = 0.0f;

    switch (now_loading_flow)
    {
    case 0:
        SubNowLoading(1, 0, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

        now_loading_flow++;

        cnt = 0;
    // case fall-through
    case 1:
        alp = ((out - cnt) * 128.0f) / out;

        if (++cnt > out)
        {
            now_loading_flow++;

            cnt = 0;
        }
    break;
    case 2:
        // do nothing ...
    break;
    case 3:
        alp = (cnt * 128.0f) / out;

        if (++cnt > out)
        {
            now_loading_flow = 0xff;

            cnt = 0;
        }
    break;
    case 0xff:
        return 0xff;
    break;
    }

    SetPanel(0x80000, 0.0f, 0.0f, 640.0f, 448.0f, 0, 0, 0, 0x80);
    SetSprFile3(LOAD_ADDRESS_44, 0);

    CopySprDToSpr(&ds, test);

    ds.tex1 = SCE_GS_SET_TEX1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);

    ds.pri = test[0].pri;
    ds.z = 0xfffffff - test[0].pri;

    ds.x = test[0].x;
    ds.y = test[0].y;


    DispSprD(&ds);

    LocalCopyLtoL((sys_wrk.count & 1) * 0x8c0, 0x1a40);

    bx[0] = bx[0] + 0.4f <  320.0f ? bx[0] + 0.4f : bx[0] + 0.4f - 640.0f;
    bx[1] = bx[1] - 0.4f > -320.0f ? bx[1] - 0.4f : bx[1] - 0.4f + 640.0f;
    by[0] = by[1] = 0.0f;

    SubNowLoading(0, 0, sclx, scly, bx[0], by[0], 1.5f, 2.5f, 1.0f);
    SubNowLoading(0, 1, sclx, scly, bx[1], by[1], 1.5f, 2.5f, 1.0f);

    SubBlur(1, 0x32, 1.003f, 180.0f, 220.0f, 112.0f);
    SubBlur(1, 0x32, 1.003f, 180.0f, 420.0f, 112.0f);

    SetPanel(0x10, 0.0f, 0.0f, 640.0f, 448.0f, 0, 0, 0, alp);

    return now_loading_flow;
}

void SubGameOver(u_char alp, float rate)
{
    static float rrr[1813];
    static float lll[1813];
    static float mm1[1813];
    static float mm2[1813];
    static float sss[1813];
    static float ccc[1813];
    static int fl = 1;
    static float r = 0.0f;
    int i;
    int j;
    int k;
    int m;
    int bak;
    int pnumw;
    int vnumw;
    int wix;
    int pnumh;
    int vnumh;
    int wiy;
    int sx2;
    int sy2;
    float sclx;
    float scly;
    float add;
    float f1;
    float f2;
    float f3;
    float f4;
    float wfw;
    float wfh;
    float fw;
    float lm;
    float cntw;
    float cnth;
    float fx;
    float fy;
    float tx[1813];
    float ty[1813];
    u_long tex0;
    sceVu0FVECTOR vt[1813];
    sceVu0IVECTOR vtiw[1813];
    int x8; // not in STAB
    int y8; // not in STAB

    pnumw = 48;
    vnumw = 49;
    pnumh = 36;
    vnumh = 37;

    sx2 = 16 * (640 - 1) - 8;
    sy2 = 16 * (224 - 1) - 8;

    sclx = 17.0f;
    scly = 13.0f;

    cntw = (float)((pnumw/2) % vnumw) * sclx;
    cnth = (float)((pnumh/2) % vnumw) * scly;

    tex0 = SCE_GS_SET_TEX0_1(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);

    LocalCopyLtoL((sys_wrk.count & 1) * 0x8c0, 0x1a40);

    x8 = 8;
    y8 = 8;

    f3 = 408.0f;
    f4 = 234.0f;

    for (i = 0; i < vnumw*vnumh; i++)
    {
        wix = i % vnumw;
        wiy = i / vnumw;

        f1 = wix * sclx;
        f2 = wiy * scly;

        vt[i][0] = f1 - f3;
        vt[i][1] = f2 - f4;
        vt[i][2] = 0.0f;
        vt[i][3] = 1.0f;

        tx[i] = (int)(((f1 - f3)        + 320.0f) * 16.0f);
        ty[i] = (int)(((f2 - f4) * 0.5f + 112.0f) * 16.0f);
        tx[i] = (tx[i] < x8) ? x8 : ((sx2 < tx[i]) ? sx2 : tx[i]);
        ty[i] = (ty[i] < y8) ? y8 : ((sy2 < ty[i]) ? sy2 : ty[i]);
    }

#if defined(MATCHING_DECOMP)
    if (fw) {} // HACK: fixes a stack misalignment
#endif

    fw = rate / 25.0f;

    if (fl != 0)
    {
        for (i = 0; i < vnumw*vnumh; i++)
        {
            wix = i % vnumw;
            wiy = i / vnumw;

            wfw = wix * sclx - cntw;
            wfh = wiy * scly - cnth;

            lll[i] = VER_SQRTF(wfw * wfw + wfh * wfh);

            lm = lll[0];

            if (wix != ((pnumw / 2) % vnumw) || wiy != ((pnumh / 2) % vnumw))
            {
                rrr[i] = VER_ATAN2F(wfw, wfh);
            }
            else
            {
                rrr[i] = 0.0f;
            }

            mm1[i] = (lll[i] * PI * 12.0f) / lm;
            mm2[i] = (lm - lll[i]) / lm;

            sss[i] = VER_SINF(rrr[i]);
            ccc[i] = VER_COSF(rrr[i]);
        }
    }

#if defined(MATCHING_DECOMP)
    if (lll[0]) {}  // debug code ??
#endif

    for (i = 0; i < vnumw*vnumh; i++)
    {
        wix = i % vnumw;
        wiy = i / vnumw;

        fx = lll[i];
        fy = VER_SINF(mm1[i] - r) * fw * mm2[i];

        vt[i][0] = sss[i] * fx - ccc[i] * fy;
        vt[i][1] = ccc[i] * fx + sss[i] * fy;
    }

    add = 0.08f;

    r = r + add <= (PI * 2) ? r + add : r + add - (PI * 2);

    for (i = 0; i < vnumw*vnumh; i++)
    {
        vtiw[i][0] = (vt[i][0]        + 2048.0f) * 16.0f;
        vtiw[i][1] = (vt[i][1] * 0.5f + 2048.0f) * 16.0f;
        vtiw[i][2] = 0xfff5fff;
        vtiw[i][3] = 1;
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

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    m = (pnumw + 1) * pnumh;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(m, SCE_GS_TRUE, SCE_GS_TRUE, 348, SCE_GIF_PACKED, 6);

    pbuf[ndpkt++].ul64[1] = 0
        | SCE_GS_UV    << (4 * 0)
        | SCE_GS_RGBAQ << (4 * 1)
        | SCE_GS_XYZF2 << (4 * 2)
        | SCE_GS_UV    << (4 * 3)
        | SCE_GS_RGBAQ << (4 * 4)
        | SCE_GS_XYZF2 << (4 * 5);

    for (i = 0; i < m; i++)
    {
        j = i + vnumw;
        k = (i % (pnumw+1)) ? 0 : 0x8000;

        pbuf[ndpkt].ui32[0] = (u_int)tx[i];
        pbuf[ndpkt].ui32[1] = (u_int)ty[i];
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt++].ui32[3] = 0;

        pbuf[ndpkt].ui32[0] = 0x80;
        pbuf[ndpkt].ui32[1] = 0x80;
        pbuf[ndpkt].ui32[2] = 0x80;
        pbuf[ndpkt++].ui32[3] = (u_int)alp;

        pbuf[ndpkt].ui32[0] = vtiw[i][0];
        pbuf[ndpkt].ui32[1] = vtiw[i][1];
        pbuf[ndpkt].ui32[2] = vtiw[i][2];
        pbuf[ndpkt++].ui32[3] = k;

        pbuf[ndpkt].ui32[0] = (u_int)tx[j];
        pbuf[ndpkt].ui32[1] = (u_int)ty[j];
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt++].ui32[3] = 0;

        pbuf[ndpkt].ui32[0] = 0x80;
        pbuf[ndpkt].ui32[1] = 0x80;
        pbuf[ndpkt].ui32[2] = 0x80;
        pbuf[ndpkt++].ui32[3] = (u_int)alp;

        pbuf[ndpkt].ui32[0] = vtiw[j][0];
        pbuf[ndpkt].ui32[1] = vtiw[j][1];
        pbuf[ndpkt].ui32[2] = vtiw[j][2];
        pbuf[ndpkt++].ui32[3] = k;
    }

    pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;
}

int gameover_flow = 9;

void InitGameOver()
{
    LocalCopyLtoL((sys_wrk.count & 1) * 0x8c0, 0x1a40);

    gameover_flow = 9;
}

void SetGameOverON()
{
    gameover_flow = 0;
}

int SetGameOver()
{
    SPRT_DAT gameover_tbl[2] = {
        {
            .tex0 = SCE_GS_SET_TEX0_1(0x2300, 10, SCE_GS_PSMT8, 10, 9, 1, SCE_GS_MODULATE, 0x2b80, SCE_GS_PSMCT32, 0, 0, 1),
            .u = 0,
            .v = 0,
            .w = 640,
            .h = 448,
            .x = 0,
            .y = 0,
            .pri = 0x7f000,
            .alpha = 0x80,
        },
        {
            .tex0 = SCE_GS_SET_TEX0_1(0x2b00, 8, SCE_GS_PSMT4, 9, 7, 1, SCE_GS_MODULATE, 0x2b84, SCE_GS_PSMCT32, 0, 0, 1),
            .u = 1,
            .v = 1,
            .w = 510,
            .h = 126,
            .x = 64,
            .y = 160,
            .pri = 0x64000,
            .alpha = 0x80,
        },
    };
    DISP_SPRT ds;
    int i;
    static int cnt = 0;
    int sec5;
    int sec0;
    int sec1;
    int sec2;
    int sec3;
    int sec4;
    int blr;
    int alp0;
    int alp1;
    int alp2;
    int alp3;
    int num;
    float scl;
    int type;

    sec5 = 30;
    sec0 = 30;
    sec1 = 240;
    sec2 = 90;
    sec3 = 30;
    sec4 = 30;

    num = 1;

    switch(gameover_flow)
    {
    case 0:
        alp0 = 0;
        alp1 = 0;
        alp2 = 0;
        alp3 = 0;

        blr = 300;

        scl = 0.0f;

        cnt = 0;

        efcnt[12].dat.uc8[0] = 0;

        LocalCopyLtoL(0x1a40, (sys_wrk.count & 1) * 0x8c0);

        gameover_flow = 1;
    break;
    case 1:
        alp0 = 0;
        alp1 = 0;
        alp2 = 0;
        alp3 = ((cnt * 128) / sec5);

        blr = 300;

        scl = 0.0f;

        LocalCopyLtoL(0x1a40, (sys_wrk.count & 1) * 0x8c0);

        cnt++;

        if (cnt >= sec5)
        {
            cnt = LoadReq(G_OVER_PK2, LOAD_ADDRESS_41);

            gameover_flow = 2;
        }
    break;
    case 2:
        alp0 = 0;
        alp1 = 0;
        alp2 = 0;
        alp3 = 0x80;

        scl = 0.0f;

        blr = 300;

        if (IsLoadEnd(cnt))
        {
            cnt = 0;

            gameover_flow = 3;
        }
    break;
    case 3:
        alp0 = 0x80;
        alp1 = 0;
        alp2 = 0x80;
        alp3 = (((sec0 - cnt) * 0x80) / sec0);

        blr = 300;

        scl = 0.0f;

        cnt++;

        if (cnt >= sec0)
        {
            cnt = 0;

            gameover_flow = 5;
        }
    break;
    case 5:
        alp0 = 128;

        if (sec1 / 2 > cnt)
        {
            alp1 = (cnt * 128) / (sec1 / 2);
            blr = 300;
        }
        else
        {
            alp1 = 0x80;
            blr = ((sec1 - cnt) * 300) / (sec1 / 2);
        }

        alp2 = 0x80;
        alp3 = 0;

        scl = 0.0f;

        cnt++;

        if (cnt >= sec1)
        {
            cnt = 0;

            gameover_flow = 6;
        }
    break;
    case 6:
        alp0 = 0x80;
        alp1 = 0x80;
        alp2 = ((sec2 - cnt) * 0x80) / sec2;
        alp3 = 0;

        blr = 0;

        cnt++;

        if (cnt >= sec2)
        {
            cnt = 0;

            gameover_flow = 7;
        }
    break;
    case 7:
        alp0 = 0x80;
        alp1 = ((sec3 - cnt) * 0x80) / sec3;
        alp2 = 0;
        alp3 = (cnt * 128) / sec3;

        blr = 0;

        scl = (cnt * 8.0f) / sec3;

        num = (cnt * 60) / sec3 + 1;

        cnt++;

        if (cnt >= sec3)
        {
            cnt = 0;

            gameover_flow = 8;
        }
    break;
    case 8:
        alp0 = 0;
        alp1 = 0;
        alp2 = 0;
        alp3 = ((sec4 - cnt) * 0x80) / sec4;

        blr = 0;

        scl = 0.0f;

        SetPanel2(0x80000, 0.0f, 0.0f, DISP_WIDTH, DISP_HEIGHT, 0, 0, 0, 0, 0x80);

        cnt++;

        if (cnt >= sec4)
        {
            gameover_flow = 9;
        }
    break;
    case 9:
        SetPanel2(0x80000, 0.0f, 0.0f, DISP_WIDTH, DISP_HEIGHT, 0, 0, 0, 0, 0x80);
    return 0xff;
    }

    if (gameover_flow > 2)
    {
        SetSprFile3(LOAD_ADDRESS_41, 0);

        if (gameover_flow > 2)
        {
            CopySprDToSpr(&ds, &gameover_tbl[0]);

            ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
            ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
            ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
            ds.alpha = alp0;

            DispSprD(&ds);

            if (gameover_flow > 2)
            {
                CopySprDToSpr(&ds, &gameover_tbl[1]);

                ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
                ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
                ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);

                if (gameover_flow == 7)
                {
                    for (i = num - 1; i >= 0; i--)
                    {
                        ds.alpha = alp1;

                        ds.scw = (scl * 1.5f * i) / num + 1;
                        ds.sch = (scl * i) / num + 1;

                        ds.csx = (float)DISP_WIDTH / 2;
                        ds.csy = (float)DISP_HEIGHT / 2;

                        DispSprD(&ds);
                    }
                }

                ds.alpha = alp1;

                ds.x = gameover_tbl[1].x;
                ds.y = gameover_tbl[1].y;

                ds.scw = 1.0f;
                ds.sch = 1.0f;

                ds.csx = (float)DISP_WIDTH / 2;
                ds.csy = (float)DISP_HEIGHT / 2;

                DispSprD(&ds);

                type = 1;

                if (gameover_flow >= 3 && alp2 != 0)
                {
                    SubGameOver(alp2, blr);
                }
            }
        }
    }

    if (gameover_flow < 8)
    {
        if (gameover_flow >= 7)
        {
            SubBlur(type, 0x64, 1.02f, 180.0f, (float)SCR_WIDTH / 2, (float)SCR_HEIGHT / 2);
        }
        else
        {
            SubBlur(type, 0x58, 1.005f, 180.0f, (float)SCR_WIDTH / 2, (float)SCR_HEIGHT / 2);
        }
    }

    SetPanel(0x10, 0.0f, 0.0f, DISP_WIDTH, DISP_HEIGHT, 0xff, 0xff, 0xff, alp3);

    // missing return
}

void ChangeMonochrome(int sw)
{
    if (sw)
    {
        monochrome_mode = 1;

        RequestBlackWhiteMode();
    }
    else
    {
        monochrome_mode = 0;

        CancelBlackWhiteMode();
    }
}

void InitExFade1(int fl, int time1)
{
    switch (fl)
    {
    case 0:
        ef_exfade1_flow2 = 8;
    break;
    case 1:
        ef_exfade1_flow2 = 11;
    break;
    }

    ef_exfade1_time1 = time1;
}

int SetExFade1()
{
    static int cnt1;
    static int cnt2;
    int sec2_1;
    u_char dsp[2] = {  8,  16 };
    u_char dal[2] = {  0, 127 };
    u_char nal[2] = { 45, 102 };
    u_char dalp;
    u_char dspd;
    static u_char nalp;
    float per2;

    sec2_1 = ef_exfade1_time1;

    if (ef_exfade1_flow2 == 11)
    {
        shibata_set_off = 0;

        return 1;
    }

    switch (ef_exfade1_flow2)
    {
    case 8:
        shibata_set_off = 1;

        cnt2 = 0;

        ef_exfade1_flow2 = 9;
    case 9:
        per2 = (float)cnt2 / sec2_1;

        dalp = dal[0] + (u_char)((dal[1] - dal[0]) * per2);
        dspd = dsp[0] + (u_char)((dsp[1] - dsp[0]) * per2);
        nalp = nal[0] + (u_char)((nal[1] - nal[0]) * per2);

        if (sec2_1 <= ++cnt2)
        {
            ef_exfade1_flow2 = 10;
        }
    break;
    case 10:
        dalp = dal[1];
        dspd = dsp[1];
        nalp = nal[1];
    break;
    case 11:
        dalp = dal[1];
        dspd = dsp[1];
        nalp = nal[1];

        shibata_set_off = 0;
    break;
    }

    SetEffects(EF_DITHER, 1, 3, (float)dalp, (float)dspd, 0x80, 0x40);
    SetEffects(EF_NEGA, 1, 0x21, 0xff, &nalp);
    SetEffects(EF_FADEFRAME, 1, msbtset.ff.alp, 0x80000);

#if defined(BUILD_EU_VERSION)
    SetEffects(EF_BLACKFILTER, 1, 0x24);
#endif

    return ef_exfade1_flow2 == 11;
}

void InitExFade2(int fl, int time1, int time2, int time3)
{
    ef_exfade2_flow1 = fl == 0 ? 0 : 3;
    ef_exfade2_time1 = time1;
    ef_exfade2_time2 = time2;
    ef_exfade2_time3 = time3;
}

int SetExFade2()
{
    static int cnt1;
    int sec1_1;
    int sec1_2;
    int sec1_3;
    u_char bal[3] = { 57, 67, 255 };
    u_char dfr[2] = {  0,  11 };
    u_char dal[2] = {  0,  13 };
    u_char ccl[2] = { 70, 100 };
    u_char cal[2] = { 70,  99 };
    static u_char balp;
    u_char dfrt;
    u_char dalp;
    u_char ccol;
    u_char calp;
    float per1;

    sec1_1 = ef_exfade2_time1;
    sec1_2 = ef_exfade2_time2;
    sec1_3 = ef_exfade2_time3;

    if (ef_exfade2_flow1 == 6)
    {
        shibata_set_off = 0;

        return 1;
    }

    switch(ef_exfade2_flow1)
    {
    case 0:
        shibata_set_off = 1;

        cnt1 = 0;

        ef_exfade2_flow1 = 1;
    // case fall-through
    case 1:
        per1 = (float)cnt1 / sec1_1;

        balp = bal[0] + (u_char)((bal[1] - bal[0]) * per1);
        dfrt = dfr[0] + (u_char)((dfr[1] - dfr[0]) * per1);
        dalp = dal[0] + (u_char)((dal[1] - dal[0]) * per1);
        ccol = ccl[0] + (u_char)((ccl[1] - ccl[0]) * per1);
        calp = cal[0] + (u_char)((cal[1] - cal[0]) * per1);

        if (sec1_1 <= ++cnt1)
        {
            ef_exfade2_flow1 = 2;
        }
    break;
    case 2:
        balp = bal[1];
        dfrt = dfr[1];
        dalp = dal[1];
        ccol = ccl[1];
        calp = cal[1];
    break;
    case 3:
        cnt1 = 0;

        ef_exfade2_flow1 = 4;
    // case fall-through
    case 4:
        per1 = (float)cnt1 / sec1_2;

        balp = bal[1] + (u_char)((bal[2] - bal[1]) * per1);
        dfrt = dfr[1];
        dalp = dal[1];
        ccol = ccl[1];
        calp = cal[1];

        if (sec1_2 <= ++cnt1)
        {
            ef_exfade2_flow1 = 5;

            cnt1 = 0;
        }
    break;
    case 5:
        per1 = (float)cnt1 / sec1_3;

        balp = bal[2] + (u_char)((bal[0] - bal[2]) * per1);
        dfrt = dfr[1] + (u_char)((dfr[0] - dfr[1]) * per1);
        dalp = dal[1] + (u_char)((dal[0] - dal[1]) * per1);
        ccol = ccl[1] + (u_char)((ccl[0] - ccl[1]) * per1);
        calp = cal[1] + (u_char)((cal[0] - cal[1]) * per1);

        if (sec1_3 <= ++cnt1)
        {
            shibata_set_off = 0;

            ef_exfade2_flow1 = 6;
        }
    break;
    case 6:
        // do nothing ...
    break;
    }

    if (ef_exfade2_flow1 < 6)
    {
        SetEffects(EF_FADEFRAME, 1, 0x1e, 0x80000);
        SetEffects(EF_BLUR_N, 1, &balp, 1000, 1800, 320.0f, 112.0f);
        SetEffects(EF_DEFORM, 1, 2, dfrt);
        SetEffects(EF_DITHER, 1, 2, (float)dalp, 8.0f, 0x40, 0x80);

        SetEffects(EF_NCONTRAST, 1, ccol, calp);
    }

    return ef_exfade2_flow1 == 6;
}

int ef_exfade1_flow1 = 0;
int ef_exfade1_flow2 = 0;
int ef_exfade1_time1 = 0;
int ef_exfade2_flow1 = 0;
int ef_exfade2_time1 = 0;
int ef_exfade2_time2 = 0;
int ef_exfade2_time3 = 0;

void InitScreenSaver()
{
    memset(&hand_wrk, 0, sizeof(HAND_WRK));
}

void SetScreenSaver()
{
    int i;
    int r3;
    int l3;
    u_int pad_all;
    ONE_HAND *ohp;
    DISP_SPRT ds;
    SPRT_DAT hand_dat = {
        .tex0 = SCE_GS_SET_TEX0_1(0x2300, 2, SCE_GS_PSMT8, 7, 7, 1, SCE_GS_MODULATE, 0x2340, SCE_GS_PSMCT32, 0, 0, 1),
        .u = 1,
        .v = 1,
        .w = 126,
        .h = 126,
        .x = 0,
        .y = 0,
        .pri = 0,
        .alpha = 0x80,
    };

    if (ingame_wrk.stts & 0x20)
    {
        hand_wrk.count = 0;
    }
    else
    {
        r3 = (__builtin_abs(pad[0].analog[0] - 0x80) >= 32) + (__builtin_abs(pad[0].analog[1] - 0x80) >= 32);
        l3 = (__builtin_abs(pad[0].analog[2] - 0x80) >= 32) + (__builtin_abs(pad[0].analog[3] - 0x80) >= 32);

        pad_all =
            *key_now[PAD_DPAD_UP] + *key_now[PAD_DPAD_DOWN] + *key_now[PAD_DPAD_LEFT] + *key_now[PAD_DPAD_RIGHT] +
            *key_now[PAD_TRIANGLE] + *key_now[PAD_CROSS] + *key_now[PAD_SQUARE] + *key_now[PAD_CIRCLE] +
            *key_now[PAD_L1] + *key_now[PAD_L2] + *key_now[PAD_R1] + *key_now[PAD_R2] +
            *key_now[PAD_SELECT] + *key_now[PAD_START] +
            r3 + l3;

        hand_wrk.count = pad_all == 0 ? hand_wrk.count + 1 : 0;

        if (hand_wrk.count >= 18000)
        {
            if (hand_wrk.wait-- <= 0)
            {
                ohp = &hand_wrk.oh[hand_wrk.top];

                ohp->scl = (short)(1.5f * VER_RAND() + 2.0); // 2.0 is double!!
                ohp->rot = (int)(360.0f * VER_RAND() - 180.0f);
                ohp->bx = (int)(370.0f * VER_RAND() + (hand_wrk.top & 1) * 220.0f);
                ohp->by = (int)(274.0f * VER_RAND() + (hand_wrk.top / 2 & 1) * 124.0f);
                ohp->flip = (int)(2.0f * VER_RAND());
                ohp->alp = 0xff;

                hand_wrk.wait = (int)(70.0f * VER_RAND()) + 20;
                hand_wrk.top = (hand_wrk.top + 1) % 16;
            }

            SetSprFile2(0x1fc8000, 0);

            for (i = 0; i < 16; i++)
            {
                ohp = &hand_wrk.oh[i];

                if (ohp->alp != 0)
                {
                    u_int att;

                    CopySprDToSpr(&ds, &hand_dat);

                    ds.pri = 0x10;
                    ds.z = 0xfffffff - ds.pri;

                    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
                    ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);

                    att = ds.att;
                    ds.att = ohp->flip ? att | 2 : att;

                    ds.alpha = ohp->alp / 2;

                    ds.x = ohp->bx - (u_short)(hand_dat.w / 2);
                    ds.y = ohp->by - (u_short)(hand_dat.h / 2);

                    ds.scw = ohp->scl;
                    ds.sch = ohp->scl;

                    ds.csx = ohp->bx;
                    ds.csy = ohp->by;

                    ds.rot = ohp->rot;

                    ds.crx = ohp->bx;
                    ds.cry = ohp->by;

                    DispSprD(&ds);

                    ohp->alp = ohp->alp - 1 < 0 ? 0 : ohp->alp - 1;
                }
            }
        }
    }
}

void SetScreenEffect()
{
    SBTSET screen_effect[7] = {
        {
            .bl = {
                .sw = 1,
                .alp = 57,
                .scl = 1000,
                .rot = 1800,
                .x = 320.0f,
                .y = 112.0f,
            },
            .cn = {
                .sw = 1,
                .type = 1,
                .col = 80,
                .alp = 80,
            },
            .ff = {
                .sw = 1,
                .alp = 30,
            },
            .dt = {
                .sw = 1,
                .type = 2,
                .spd = 8.0f,
                .alp = 10.0f,
                .amax = 64,
                .cmax = 128,
            },
            .df = {
                .sw = 0,
                .type = 0,
                .rate = 0,
            },
            .ng = {
                .sw = 0,
                .col = 0,
                .alp = 0,
                .alp2 = 0,
            },
            .mn = {
                .sw = 1,
            },
        },
        {
            .bl = {
                .sw = 1,
                .alp = 57,
                .scl = 1000,
                .rot = 1800,
                .x = 320.0f,
                .y = 112.0f,
            },
            .cn = {
                .sw = 1,
                .type = 1,
                .col = 70,
                .alp = 70,
            },
            .ff =  {
                .sw = 1,
                .alp = 30,
            },
            .dt = {
                .sw = 0,
                .type = 0,
                .spd = 0.0f,
                .alp = 0.0f,
                .amax = 0,
                .cmax = 0,
            },
            .df = {
                .sw = 0,
                .type = 0,
                .rate = 0,
            },
            .ng = {
                .sw = 0,
                .col = 0,
                .alp = 0,
                .alp2 = 0,
            },
            .mn = {
                .sw = 0,
            },
        },
        {
            .bl = {
                .sw = 1,
                .alp = 57,
                .scl = 1000,
                .rot = 1800,
                .x = 320.0f,
                .y = 112.0f,
            },
            .cn = {
                .sw = 1,
                .type = 1,
                .col = 70,
                .alp = 70,
            },
            .ff = {
                .sw = 1,
                .alp = 30,
            },
            .dt = {
                .sw = 1,
                .type = 2,
                .spd = 8.0f,
                .alp = 28.0f,
                .amax = 36,
                .cmax = 104,
            },
            .df = {
                .sw = 0,
                .type = 0,
                .rate = 0,
            },
            .ng = {
                .sw = 0,
                .col = 0,
                .alp = 0,
                .alp2 = 0,
            },
            .mn = {
                .sw = 0,
            },
        },
        {
            .bl = {
                .sw = 1,
                .alp = 57,
                .scl = 1000,
                .rot = 1800,
                .x = 320.0f,
                .y = 112.0f,
            },
            .cn = {
                .sw = 1,
                .type = 1,
                .col = 70,
                .alp = 70,
            },
            .ff = {
                .sw = 1,
                .alp = 30,
            },
            .dt = {
                .sw = 1,
                .type = 4,
                .spd = 8.0f,
                .alp = 26.0f,
                .amax = 64,
                .cmax = 35,
            },
            .df = {
                .sw = 0,
                .type = 0,
                .rate = 0,
            },
            .ng = {
                .sw = 0,
                .col = 0,
                .alp = 0,
                .alp2 = 0,
            },
            .mn = {
                .sw = 0,
            },
        },
        {
            .bl = {
                .sw = 1,
                .alp = 25,
                .scl = 1000,
                .rot = 1800,
                .x = 320.0f,
                .y = 112.0f,
            },
            .cn = {
                .sw = 1,
                .type = 3,
                .col = 59,
                .alp = 166,
            },
            .ff = {
                .sw = 1,
                .alp = 30,
            },
            .dt = {
                .sw = 1,
                .type = 6,
                .spd = 8.0f,
                .alp = 52.0f,
                .amax = 64,
                .cmax = 128,
            },
            .df = {
                .sw = 0,
                .type = 0,
                .rate = 0,
            },
            .ng = {
                .sw = 0,
                .col = 0,
                .alp = 0,
                .alp2 = 0,
            },
            .mn = {
                .sw = 0,
            },
        },
        {
            .bl = {
                .sw = 1,
                .alp = 57,
                .scl = 1000,
                .rot = 1800,
                .x = 320.0f,
                .y = 112.0f,
            },
            .cn = {
                .sw = 1,
                .type = 1,
                .col = 70,
                .alp = 70,
            },
            .ff = {
                .sw = 1,
                .alp = 30,
            },
            .dt = {
                .sw = 1,
                .type = 2,
                .spd = 8.0f,
                .alp = 21.0f,
                .amax = 76,
                .cmax = 67,
            },
            .df = {
                .sw = 1,
                .type = 2,
                .rate = 10,
            },
            .ng = {
                .sw = 0,
                .col = 0,
                .alp = 0,
                .alp2 = 0,
            },
            .mn = {
                .sw = 0,
            },
        },
        {
            .bl = {
                .sw = 1,
                .alp = 25,
                .scl = 1000,
                .rot = 1800,
                .x = 320.0f,
                .y = 112.0f,
            },
            .cn = {
                .sw = 1,
                .type = 3,
                .col = 59,
                .alp = 166,
            },
            .ff = {
                .sw = 1,
                .alp = 30,
            },
            .dt = {
                .sw = 1,
                .type = 6,
                .spd = 8.0f,
                .alp = 52.0f,
                .amax = 64,
                .cmax = 128,
            },
            .df = {
                .sw = 0,
                .type = 0,
                .rate = 0,
            },
            .ng = {
                .sw = 0,
                .col = 0,
                .alp = 0,
                .alp2 = 0,
            },
            .mn = {
                .sw = 1,
            },
        },
    };
    int n;

    if (ingame_wrk.game == 1)
    {
        n = battle_scr_effct[btl_wrk.stage_no];

        if (n == -1)
        {
            return;
        }
    }
    else
    {
        n = ingame_wrk.msn_no != 0;
    }

    msbtset.bl.sw = screen_effect[n].bl.sw;
    msbtset.bl.alp = screen_effect[n].bl.alp;
    msbtset.bl.scl = screen_effect[n].bl.scl;
    msbtset.bl.rot = screen_effect[n].bl.rot;
    msbtset.bl.x = screen_effect[n].bl.x;
    msbtset.bl.y = screen_effect[n].bl.y;

    msbtset.cn.sw = screen_effect[n].cn.sw;
    msbtset.cn.type = screen_effect[n].cn.type;
    msbtset.cn.col = screen_effect[n].cn.col;
    msbtset.cn.alp = screen_effect[n].cn.alp;

    msbtset.ff.sw = screen_effect[n].ff.sw;
    msbtset.ff.alp = screen_effect[n].ff.alp;

    msbtset.dt.sw = screen_effect[n].dt.sw;
    msbtset.dt.type = screen_effect[n].dt.type;
    msbtset.dt.alp = screen_effect[n].dt.alp;
    msbtset.dt.spd = screen_effect[n].dt.spd;
    msbtset.dt.amax = screen_effect[n].dt.amax;
    msbtset.dt.cmax = screen_effect[n].dt.cmax;

    msbtset.df.sw = screen_effect[n].df.sw;
    msbtset.df.type = screen_effect[n].df.type;
    msbtset.df.rate = screen_effect[n].df.rate;

    msbtset.ng.sw = screen_effect[n].ng.sw;
    msbtset.ng.col = screen_effect[n].ng.col;
    msbtset.ng.alp = screen_effect[n].ng.alp;
    msbtset.ng.alp2 = screen_effect[n].ng.alp2;

    msbtset.mn.sw = screen_effect[n].mn.sw;
}
