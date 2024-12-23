#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "effect_scr.h"

// #include <cstdlib.h>
// RAND_MAX = (2**31-1)
#define RAND_MAX 2147483647

// /usr/local/sce/ee/gcc/src/newlib/libm/math/s_isnan.c
// int isnan(double x);

#include "ee/eestruct.h"
#include "ee/kernel.h"
#include "sce/libgraph.h"
#include "sce/libvu0.h"

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

typedef struct { // 0x28
	/* 0x00 */ u_int flow;
	/* 0x04 */ u_int cnt;
	/* 0x08 */ u_int in;
	/* 0x0c */ u_int keep;
	/* 0x10 */ u_int out;
	/* 0x14 */ u_int alp;
	/* 0x18 */ float scl;
	/* 0x1c */ float rot;
	/* 0x20 */ float cx;
	/* 0x24 */ float cy;
} EFF_BLUR;

typedef struct { // 0x18
	/* 0x00 */ u_int flow;
	/* 0x04 */ u_int cnt;
	/* 0x08 */ u_int in;
	/* 0x0c */ u_int keep;
	/* 0x10 */ u_int out;
	/* 0x14 */ u_int max;
} EFF_FOCUS;

typedef struct { // 0x4
	/* 0x0 */ u_char type;
	/* 0x1 */ u_char otype;
	/* 0x2 */ u_char init;
	/* 0x3 */ u_char pass;
} EFF_DEFORM;

typedef struct { // 0x18
	/* 0x00 */ float rrr;
	/* 0x04 */ float lll;
	/* 0x08 */ float mm1;
	/* 0x0c */ float mm2;
	/* 0x10 */ float sss;
	/* 0x14 */ float ccc;
} DEFWORK;

typedef struct { // 0xc
	/* 0x0 */ float scl;
	/* 0x4 */ short int rot;
	/* 0x6 */ short int bx;
	/* 0x8 */ short int by;
	/* 0xa */ u_char flip;
	/* 0xb */ u_char alp;
} ONE_HAND;

typedef struct { // 0xd0
	/* 0x00 */ ONE_HAND oh[16];
	/* 0xc0 */ u_long count;
	/* 0xc8 */ int wait;
	/* 0xcc */ u_int top;
} HAND_WRK;

/* bss 374f10 */ static HAND_WRK hand_wrk;
/* bss 374fe0 */ static DEFWORK dw[25][33];

/* sbss 3578fc */ static int now_loading_flow;

/* data 2809f0 */ EFF_BLUR eff_blur = {
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
/* data 280a18 */ EFF_FOCUS eff_focus = {
    .flow = 3,
    .cnt = 0,
    .in = 0,
    .keep = 0,
    .out = 0,
    .max = 0,
};

/* sdata 356438 */ EFF_DEFORM eff_deform = {
    .type = 0,
    .otype = 0,
    .init = 0,
    .pass = 0,
};
/* sdata 356440 */ short int overlap_passflg[2] = { 0, 0 };
/* sdata 356444 */ float blur_x_offset = 0.0f; // unused ??
/* sdata 356448 */ float blur_y_offset = 0.0f; // unused ??

#define PI 3.1415927f
#define PI2 6.2831855f
#define PI_HALF 1.5707964f

#define INIT_TBP 0x1a40
#define TBP_0x8c0 (INIT_TBP / 3)

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

void SetWhiteOut2(/* a0 4 */ int time)
{
    SetParam(0, time, 0xff, 0xff, 0xff, 2);
}

void SetWhiteIn2(/* a0 4 */ int time)
{
    SetParam(0x80, time, 0xff, 0xff, 0xff, 1);
}

void SetBlackOut2(/* a0 4 */ int time)
{
    SetParam(0, time, 0, 0, 0, 2);
}

void SetBlackIn2(/* a0 4 */ int time)
{
    SetParam(0x80,time,0x0,0x0,0x0,1);
}

void SetFlash()
{
    SetParam(0x80, 18, 0xff, 0xff, 0xff, 1);
}

void SetBlackFilter(/* s0 16 */ EFFECT_CONT *ec)
{
    SetPanel(0x500, 0.0f, 0.0f, 640.0f, 448.0f, 0, 0, 0, ec->dat.uc8[2]);

    if ((ec->dat.uc8[1] & 1) != 0)
    {
        ResetEffects(ec);
    }
}

void SubBlur(/* t2 10 */ int type, /* t3 11 */ u_char alpha, /* f12 50 */ float scale, /* f13 51 */ float rot, /* f14 52 */ float cx, /* f15 53 */ float cy)
{
	/* 0x0(sp) */ SPRITE_DATA sd = {
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
        .pos_y = -247.39999f,
        .pos_z = 0,
        .size_w = 704.0f,
        .size_h = 492.79999f,
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
	/* 0x60(sp) */ DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1_1(1, 0, 1, 5, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(0, 1, 0, 1, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GEQUAL, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GREATER),
        .clamp = 0,
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 342, SCE_GIF_PACKED, 5),
    };
	/* f2 40 */ float hw;
	/* f4 42 */ float hh;
	/* f1 39 */ float fx;
	/* f0 38 */ float fy;
    
    if ((plyr_wrk.sta & 1) && (efcnt[12].dat.uc8[0] != 0))
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
    sd.pos_z = 0xfffff5f;
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

void SetBlur(/* s0 16 */ EFFECT_CONT *ec)
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

void RunBlur(/* a0 4 */ EFFECT_CONT *ec)
{
	/* v1 3 */ int ef;
	/* f2 40 */ float phase;

    switch (eff_blur.flow)
    {
    case 0:
        eff_blur.cnt++;
        
        if (eff_blur.in != 0)
        {
            phase = (eff_blur.cnt * 90.0f) / eff_blur.in;
            ef = SgSinf(phase * PI / 180.0f) * eff_blur.alp;
            
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
        
        ef = SgSinf(PI_HALF) * eff_blur.alp;
        
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
            ef = SgSinf(phase * PI / 180.0f) * eff_blur.alp;

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

void CallBlur(/* a0 4 */ int type, /* a1 5 */ int wait, /* a2 6 */ u_char alpha, /* f12 50 */ float scale, /* f13 51 */ float rot)
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

void CallBlur2(/* a0 4 */ int in, /* a1 5 */ int keep, /* a2 6 */ int out, /* a3 7 */ u_char alpha, /* f12 50 */ float scale, /* f13 51 */ float rot)
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

void CallBlur3(/* a0 4 */ int in, /* a1 5 */ int keep, /* a2 6 */ int out, /* a3 7 */ u_char alpha, /* f12 50 */ float scale, /* f13 51 */ float rot, /* f14 52 */ float cx, /* f15 53 */ float cy)
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

void SubFocus(/* a0 4 */ int ef)
{
	/* 0x0(sp) */ SPRITE_DATA sd = {
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
	/* f24 62 */ float hw = 0.0f;
	/* f25 63 */ float ff = 0.0f;

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

void SetFocus(/* s0 16 */ EFFECT_CONT *ec)
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

void RunFocus(/* s2 18 */ EFFECT_CONT *ec)
{
	/* v0 2 */ int ef;
	/* f2 40 */ float phase;

    switch (eff_focus.flow)
    {
    case 0:
        eff_focus.cnt++;
        
        if (eff_focus.in != 0)
        {
            phase = (eff_focus.cnt * 90.0f) / eff_focus.in;
            ef = SgSinf(phase * PI / 180.0f) * eff_focus.max;

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
        
        ef = SgSinf(PI_HALF) * eff_focus.max;
        
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
            ef = SgSinf(phase * PI / 180.0f) * eff_focus.max;

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

void CallFocus(/* a0 4 */ int type, /* a1 5 */ int wait, /* a2 6 */ int gap)
{
    eff_focus.flow = type != 0;
    eff_focus.cnt = 0;
    eff_focus.max = gap;
    eff_focus.in = 30;
    eff_focus.keep = wait;
    eff_focus.out = 30;
}

void CallFocus2(/* a0 4 */ int in, /* a1 5 */ int keep, /* a2 6 */ int out, /* a3 7 */ int max)
{
    eff_focus.flow = in < 1 ? (keep < 1 ? (out > 0 ? 2 : 3) : 1) : 0;
    eff_focus.cnt = 0;
    eff_focus.max = max;
    eff_focus.in = in;
    eff_focus.keep = keep;
    eff_focus.out = out;
}

void SubDeform(/* a2 6 */ int type, /* f12 50 */ float rate, /* a1 5 */ u_char alp)
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

void SetDeform(/* s0 16 */ EFFECT_CONT *ec)
{
	/* f12 50 */ float ef;
	/* f2 40 */ float phase;

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
                ef = SgSinf((phase * PI) / 180.0f) * ec->dat.uc8[3];
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
        
            ef = SgSinf(PI_HALF) * ec->dat.uc8[3];
            
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
                ef = SgSinf(phase * PI / 180.0f) * ec->dat.uc8[3];
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

void RunDeform(/* s0 16 */ EFFECT_CONT *ec)
{
    float ef;
	/* f2 40 */ float phase;
    
    if (ec->dat.uc8[1] & 4)
    {
        switch(ec->flow)
        {
        case 0:
            ec->cnt++;
            
            if (ec->in != 0)
            {
                phase = (ec->cnt * 90.0f) / ec->in;
                ef = SgSinf(phase * PI / 180.0f) * ec->dat.uc8[3];
                
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
            
            ef = SgSinf(PI_HALF) * ec->dat.uc8[3];
            
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
                ef = SgSinf(phase * PI / 180.0f) * ec->dat.uc8[3];
    
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

void CallDeform2(/* a0 4 */ int in, /* a1 5 */ int keep, /* t2 10 */ int out, /* a3 7 */ int type, /* t0 8 */ int max)
{
    SetEffects(6, 4, type, max, in, keep, out);
}

static void _SetScrData(/* a0 4 */ Q_WORDDATA *dst, /* a1 5 */ SCRDEF *src)
{
    asm __volatile__(
        "lqc2    vf12,0(%1)\n"
        "lqc2    vf13,0x10(%1)\n"
        "vftoi4.xy vf12xy,$vf12xy\n"
        "vftoi4.xyz vf13xyz,$vf13xyz\n"
        "vmulx.zw vf12zw,$vf12zw,$vf0x\n"
        "sqc2    vf13,0x10(%0)\n"
        "sqc2    vf12,0(%0)\n"
    	: :"r"(dst),"r"(src):"memory"
    );
}

void MakeScrDeformPacket(/* s1 17 */ int pnumw, /* 0x0(sp) */ int pnumh, /* s2 18 */ u_long tex0, /* s7 23 */ SCRDEF (*scrdef)[33], /* s0 16 */ int alp)
{
	/* s4 20 */ int i;
	/* s6 22 */ int j;
	/* s5 21 */ int k;
	/* s3 19 */ Q_WORDDATA *ppbuf;
    
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

    ppbuf->ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 128);
    ppbuf->ul64[1] = SCE_GS_ALPHA_1;
    ppbuf++;

    ppbuf->ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_ALWAYS, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_ALWAYS);
    ppbuf->ul64[1] = SCE_GS_TEST_1;
    ppbuf++;

    ppbuf->ul64[0] = SCE_GS_SET_RGBAQ(0x80, 0x80, 0x80, alp, 1);
    ppbuf->ul64[1] = SCE_GS_RGBAQ;
    ppbuf++;

    ppbuf->ul64[0] = SCE_GIF_SET_TAG((pnumh * (pnumw + 1)) * 2, SCE_GS_TRUE, SCE_GS_TRUE, 348, SCE_GIF_PACKED, 2);
    ppbuf->ul64[1] = 0 \
        | SCE_GS_UV << (4 * 0) 
        | SCE_GS_XYZF2 << (4 * 1);
    ppbuf++;
    
    for (j = 0; j < pnumh; j++)
    {
        _SetScrData(&ppbuf[0], &scrdef[j][0]);
        _SetScrData(&ppbuf[2] , &scrdef[j + 1][0]);
        
        ppbuf[1].ui32[3] = 0x8000;
        ppbuf[3].ui32[3] = 0x8000;
        
        ppbuf += 4;
        
        for (i = 2, k = 1; i < (pnumw + 1) * 2;  k++, i += 2)
        {
            _SetScrData(&ppbuf[0], &scrdef[j][k]);
            _SetScrData(&ppbuf[2], &scrdef[j + 1][k]);
            
            ppbuf += 4;
        }
    }
    
    Set2DPacketBufferAddress(ppbuf);
}

void SetDeform0(/* 0x2850(sp) */ int type, /* f20 58 */ float rate, /* 0x2854(sp) */ u_char alp)
{
	/* sdata 35644c */ static float r = 0.0f;
	/* sdata 356450 */ static float add = 6.0f;
	/* sdata 356454 */ static int swch = 0;
	/* s1 17 */ int i;
	/* s2 18 */ int j;
	/* t9 25 */ int c;
    int pnumh;
	/* s0 16 */ int vnumw;
	/* f1 39 */ float ll;
	/* f23 61 */ float fw;
	/* f7 45 */ float yoff;
	/* 0x0(sp) */ float tx[17][25];
	/* 0x6b0(sp) */ float ty[17][25];
	/* 0xd60(sp) */ sceVu0FVECTOR vtw[17][25];
	/* 0x27f0(sp) */ SPRITE_DATA sd1 = {
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
    int h = 17;
    int w = 25;

    pnumh = 16;
    vnumw = 24;
    
    LocalCopyLtoLDraw((sys_wrk.count & 1) * 0x8c0, 0x1a40);
    
    yoff = GetYOffsetf();
    
    for (j = 0; j < h; j++)
    {
        for (i = 0; i < w; i++)
        {
            vtw[j][i][0] = (float)(i * 640) / vnumw + 1726.5f;
            vtw[j][i][1] = (float)(j * 224) / pnumh + 1936.0f;
            vtw[j][i][2] = 0.0f;
            vtw[j][i][3] = 1.0f;

            tx[j][i] = (float)(i * 640) / vnumw;
            ty[j][i] = (float)(j * 224) / pnumh + yoff;
            
            if (i == 0)
            {
                tx[j][0] += 1.0f;
            }
            
            if (i == w-1)
            {
                tx[j][w-1] -= 1.0f;
            }
            
            if (j == 0)
            {
                ty[0][i] += 1.0f;
            }
            
            if (j == h-1)
            {
                ty[h-1][i] -= 1.0f;
            }
        }
    }
    
    fw = rate / 10.0f;
    
    add = 2.0f;
    
    for (j = 0; j < h; j++)
    {
        for (i = 0; i < w; i++)
        {
            if (i != 0 && i != w-1 && j != 0 && j != h-1)
            {
                ll = SgSinf(((r + j * 50.0f) * PI) / 180.0f) * fw;

                vtw[j][i][0] += ll;
                
                if (type == 2)
                {
                    ll = SgSinf(((r + i * 50.0f) * PI) / 180.0f) * fw;
                }
                
                vtw[j][i][1] += ll;
            }
        }
    }
    
    if (stop_effects == 0)
    {
        r = r + add <= 360.0f ? r + add : (r + add) - 360.0f;
    }

    Reserve2DPacket(0x1000);
    
    pbuf[ndpkt].ul128 = (u_long128)0;
    // (25 * 16) * 6 + 0x70000008
    pbuf[ndpkt++].ui32[0] = DMAend + 8 + (pnumh * (vnumw+1)) * 6;

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
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG((pnumh * (vnumw+1))*2, SCE_GS_TRUE, SCE_GS_TRUE, 452, SCE_GIF_PACKED, 3);
    }
    else
    {
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG((pnumh * (vnumw+1))*2, SCE_GS_TRUE, SCE_GS_TRUE, 348, SCE_GIF_PACKED, 3);
    }
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) 
        | SCE_GS_UV    << (4 * 1) 
        | SCE_GS_XYZF2 << (4 * 2);

    if (0/((vnumw + 1) * 2)){} // debug code ??
    
    c = 2;
    
    for (j = 0; j < pnumh; j++)
    {
        for (i = 0; i < (vnumw + 1) * 2; i++)
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

void SetDeform2(/* 0x81f8(sp) */ int type, /* f30 68 */ float rate, /* 0x81fc(sp) */ u_char alp)
{
	/* bss 365610 */ static float rrr[825];
	/* bss 3662f8 */ static float lll[825];
	/* bss 366fe0 */ static float mm1[825];
	/* bss 367cc8 */ static float mm2[825];
	/* bss 3689b0 */ static float sss[825];
	/* bss 369698 */ static float ccc[825];
	/* sdata 35645c */ static float r = 0.0f;
	/* sdata 356460 */ static float add = 6.0f;
	/* sdata 356464 */ static float ll = 0.0f;
	/* sdata 356468 */ static int swch = 0;
    /* s4 20 */ int i;
    /* t6 14 */ int k;
    /* s3 19 */ int m;
    /* t7 15 */ int bak;
    /* s3 19 */ int vnumw;
    int vnumh;
    /* s0 16 */ int wix;
    /* v0 2 */ int pnumh;
    int pnumw;
    /* s2 18 */ int wiy;
    /* f20 58 */ float wfw;
    /* f21 59 */ float wfh;
    /* f20 58 */ float yoff;
    /* f0 38 */ float l;
    /* f24 62 */ float fw;
    /* f22 60 */ float lm;
    /* f26 64 */ float cnth;
    /* f27 65 */ float cntw;
    /* f23 61 */ float ts;
    /* f22 60 */ float fx;
    /* f21 59 */ float fy;
    /* f20 58 */ float fz;
    /* 0x81f0(sp) */ float rot_x;
    /* 0x81f4(sp) */ float rot_y;
    /* 0x0(sp) */ float tx[825];
    /* 0xcf0(sp) */ float ty[825];
	/* 0x19e0(sp) */ sceVu0FMATRIX slm;
	/* 0x1a20(sp) */ sceVu0FMATRIX wlm;
	/* 0x1a60(sp) */ sceVu0FVECTOR pos =  { 25000.0f, -800.0f, 6500.0f, 1.0f }; // lin 1090
	/* 0x1a70(sp) */ sceVu0FVECTOR vt[825];
	/* 0x4e00(sp) */ sceVu0FVECTOR vtw[825];
    /* 0x8190(sp) */ SPRITE_DATA sd1 =  {
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

    // not in STAB ...
    float new_var_w_32;
    float new_var_h_24;
    
    i = 0;
    
    ts = 2.0f;
    
    new_var_w_32 = 32.0f;
    new_var_h_24 = 24.0f;
    wfh = new_var_w_32;
    wfw = new_var_h_24;
    
    pnumh = 24;
    vnumh = 25;
    pnumw = 32; 
    vnumw = 33;

    cntw = 2 * (float)((pnumw/2) % vnumw); // 32.0f
    cnth = 2 * (float)((pnumh/2) % vnumw); // 24.0f
    
    fx = camera.i[0] - camera.p[0];
    fy = camera.i[1] - camera.p[1];
    fz = camera.i[2] - camera.p[2];
    
    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);
    
    l = SgSqrtf(fx * fx + fy * fy + fz * fz);
    
    pos[0] = camera.p[0] + (fx * 2000.0f) / l;
    pos[1] = camera.p[1] + (fy * 2000.0f) / l;
    pos[2] = camera.p[2] + (fz * 2000.0f) / l;
    
    LocalCopyLtoLDraw((sys_wrk.count & 1) * 0x8c0, 0x1a40);
    
    sceVu0UnitMatrix(wlm);
    
    wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;
    
    sceVu0TransMatrix(wlm, wlm, pos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);
    
    yoff = GetYOffsetf();
    
    for (i = 0; i < vnumw*vnumh; i++)
    {
        wix = (i % vnumw);
        wiy = (i / vnumw);
        
        vt[i][0] = wix * ts - wfh;
        vt[i][1] = wiy * ts - wfw;
        vt[i][2] = 0.0f;
        vt[i][3] = 1.0f;
        
        sceVu0ApplyMatrix(vtw[i], slm, vt[i]);
        sceVu0DivVector(vtw[i], vtw[i], vtw[i][3]);
        
        tx[i] = ((vtw[i][0] - 2048.0f) + 320.0f);
        ty[i] = ((vtw[i][1] - 2048.0f) + 112.0f) + yoff;
        
        tx[i] = (tx[i] < 0.0f) ? 0.0f : ((639.0f < tx[i]) ? 639.0f : tx[i]);
        ty[i] = (ty[i] < 0.0f) ? 0.0f : ((223.0f < ty[i]) ? 223.0f : ty[i]);   
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
            
            lll[i] = SgSqrtf(wfw * wfw + wfh * wfh);
            
            lm = lll[0];

            if ((wix != ((pnumw / 2) % vnumw)) || ((i / vnumw) != ((pnumh / 2) % vnumw)))
            {
                rrr[i] = SgAtan2f(wfw, wfh);
            }
            else
            {
                rrr[i] = 0.0f;
            }
            
            mm1[i] = (lll[i] * PI * 12.0f) / lm;
            mm2[i] = (lm - lll[i]) / lm;
            sss[i] = SgSinf(rrr[i]);
            ccc[i] = SgCosf(rrr[i]);
        }
        
        eff_deform.init = 0;
    }
    
    if (lll[0]) {}; // debug code ?? seems that addressing lll is enough ...

    
    for (i = 0; i < vnumw*vnumh; i++)
    {
        if (((i % vnumw) != (pnumw/2) % i)) {} // debug code ??

        fx = lll[i];
        fy = SgSinf(mm1[i] - r) * fw * mm2[i];
        
        vt[i][0] = sss[i] * fx - ccc[i] * fy;
        vt[i][1] = ccc[i] * fx + sss[i] * fy;
    }
    
    add = 0.08f;
    
    if (stop_effects == 0)
    {
        r = r + add <= PI2 ? r + add : (r + add) - PI2;
    }

    
    for (i = 0; i < vnumw*vnumh; i++)
    {
        if (((i % vnumw) != (pnumw/2) % i)) {} // debug code ?? 
        
        sceVu0ApplyMatrix(vtw[i], slm, vt[i]);
        sceVu0DivVector(vtw[i], vtw[i], vtw[i][3]);
        
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
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(m, SCE_GS_TRUE, SCE_GS_TRUE, 452, SCE_GIF_PACKED, 6);
    }
    else
    {
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(m, SCE_GS_TRUE, SCE_GS_TRUE, 348, SCE_GIF_PACKED, 6);
    }
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_UV    << (4 * 0) \
        | SCE_GS_RGBAQ << (4 * 1) \
        | SCE_GS_XYZF2 << (4 * 2) \
        | SCE_GS_UV    << (4 * 3) \
        | SCE_GS_RGBAQ << (4 * 4) \
        | SCE_GS_XYZF2 << (4 * 5);
    
    for (i = 0; i < m; i++)
    {
        wiy = i + vnumw; // ??
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
            pbuf[ndpkt].ui32[0] = (wiy) % 0xff;
            pbuf[ndpkt].ui32[1] = (wiy) % 0xff;
            pbuf[ndpkt].ui32[2] = (wiy) % 0xff;
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

void SetDeform3(/* 0x81f0(sp) */ int type, /* f20 58 */ float rate, /* 0x81f4(sp) */ u_char alp)
{
	/* sdata 35646c */ static float r = 0.0f;
	/* sdata 356470 */ static float add = 6.0f;
	/* sdata 356474 */ static float ll = 0.0f;
	/* sdata 356478 */ static int swch = 0;
	/* s1 17 */ int i;
	/* s2 18 */ int j;
	/* fp 30 */ int c;
	/* s7 23 */ int pnumw;
	/* s4 20 */ int vnumw;
    int pnumh;
    int vnumh;
	/* 0x81f8(sp) */ int wix;
	/* 0x81fc(sp) */ int wiy;
	/* f20 58 */ float wfw;
	/* f21 59 */ float wfh;
	/* f26 64 */ float fw;
	/* f22 60 */ float lm;
	/* f28 66 */ float cntw;
	/* f27 65 */ float cnth;
	/* f20 58 */ float fx;
	/* f0 38 */ float fy;
	/* 0x0(sp) */ float tx[25][33];
	/* 0xcf0(sp) */ float ty[25][33];
	/* 0x19e0(sp) */ sceVu0FMATRIX slm;
	/* 0x1a20(sp) */ sceVu0FMATRIX wlm;
	/* 0x1a60(sp) */ sceVu0FVECTOR pos = { 25000.0f, -800.0f, 6500.0f, 1.0f };
	/* 0x1a70(sp) */ sceVu0FVECTOR vt[25][33];
	/* 0x4e00(sp) */ sceVu0FVECTOR vtw[25][33];
	/* s0 16 */ DEFWORK *pdef;
	/* 0x8190(sp) */ SPRITE_DATA sd1 = {
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
	/* f30 68 */ float pszw;
	/* f29 67 */ float pszh;
	/* f3 41 */ float yoff;
    
    pnumh = 24;
    pnumw = 32;
    vnumh = 25;
    vnumw = 33;

    pszw = 640.0f; // $f30,0x4420
	pszh = 224.0f; // $f29,0x4360
    cntw = 320.0f; // $f28,0x43a0
	cnth = 112.0f; // $f27,0x42e0
    
    LocalCopyLtoLDraw((sys_wrk.count & 1) * 0x8c0, 0x1a40);
    
    yoff = GetYOffsetf();
    
    for (j = 0; j < vnumh; j++)
    {
        for (i = 0; i < vnumw; i++)
        {
            vt[j][i][0] = 2048.0f;
            vt[j][i][1] = 2048.0f;
            vt[j][i][2] = 0.0f;
            vt[j][i][3] = 1.0f;

            tx[j][i] = (float)(i * 640) / pnumw;
            ty[j][i] = (float)(j * 224) / pnumh + yoff;
            
            if (i == 0)
            {
                tx[j][0] += 1.0f;
            }
            
            if (i == vnumw-1)
            {
                tx[j][vnumw-1] -= 1.0f;
            }
            
            if (j == 0)
            {
                ty[0][i] += 1.0f;
            }
            
            if (j == vnumh-1)
            {
                ty[vnumh-1][i] -= 1.0f;
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
                
                pdef->lll = SgSqrtf(wfw * wfw + wfh * wfh);
            
                lm = dw[0][0].lll;
            
                if ((wix != (pnumw/2) % vnumw) || (wiy != (pnumh/2) % vnumw))
                {
                    pdef->rrr = SgAtan2f(wfw, wfh);
                }
                else
                {
                    pdef->rrr = 0.0f;
                }
            
                pdef->mm2 = (lm - pdef->lll) / lm;
                pdef->mm1 = (pdef->lll * PI * 2.0f) / lm;
            
                pdef->sss = SgSinf(pdef->rrr);
                pdef->ccc = SgCosf(pdef->rrr);
            }
        }
        
        eff_deform.init = 0;
    }
    
    for (j = 0; j < vnumh; j++)
    {
        for (i = 0; i < vnumw; i++)
        {
            pdef = &dw[j][i];
            
            fx = pdef->lll;
            fy = SgSinf(pdef->mm1 - r) * fw * pdef->mm2;
            
            vtw[j][i][0] = pdef->sss * fx - pdef->ccc * fy;
            vtw[j][i][1] = pdef->ccc * fx + pdef->sss * fy;
            vtw[j][i][2] = 0.0f;
        }
    }
    
    add = 0.2f;
    
    if (stop_effects == 0)
    {
        r = r + add <= PI2 ? r + add : (r + add) - PI2;
    }
    
    Reserve2DPacket(0x1000);
    
    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = DMAend + (pnumh)*(pnumw+1)*2*3 + 8; // 0x1298; // 8 + 24*33*2*3;
    
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
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG((pnumh)*(pnumw+1)*2, SCE_GS_TRUE, SCE_GS_TRUE, 452, SCE_GIF_PACKED, 3);
    }
    else
    {
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG((pnumh)*(pnumw+1)*2, SCE_GS_TRUE, SCE_GS_TRUE, 348, SCE_GIF_PACKED, 3);
    }
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) 
        | SCE_GS_UV    << (4 * 1) 
        | SCE_GS_XYZF2 << (4 * 2);

    if (0/((pnumw + 1) * 2)){} // debug code ??
    
    c = 1;
    
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
            
            pbuf[ndpkt].ui32[0] = (int)(tx[i % 2 + j][i / 2] * 16.0f);
            pbuf[ndpkt].ui32[1] = (int)(ty[i % 2 + j][i / 2] * 16.0f);
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0;
            
            pbuf[ndpkt].ui32[0] = (int)((vt[i % 2 + j][i / 2][0] + vtw[i % 2 + j][i / 2][0]) * 16.0f);
            pbuf[ndpkt].ui32[1] = (int)((vt[i % 2 + j][i / 2][1] + vtw[i % 2 + j][i / 2][1]) * 16.0f);
            pbuf[ndpkt].ui32[2] = (int)((vt[i % 2 + j][i / 2][2] + vtw[i % 2 + j][i / 2][2]) * 16.0f);
            pbuf[ndpkt++].ui32[3] = (i <= 1) ? 0x8000 : 0;
        }
    }
    
    eff_deform.pass = 1;
    eff_deform.type = type;
}

void SetDeform4(/* 0x6720(sp) */ int type, /* f20 58 */ float rate, /* 0x6724(sp) */ u_char alp)
{
	/* sdata 356480 */ static float r = 0.0f;
	/* sdata 356484 */ static float add = 6.0f;
	/* s1 17 */ int i;
	/* s2 18 */ int j;
	/* fp 30 */ int pnumw;
	/* s4 20 */ int vnumw;
	/* 0x6728(sp) */ int wix;
	/* s0 16 */ int pnumh;
    int vnumh;
	/* 0x672c(sp) */ int wiy;
	/* f21 59 */ float wfw;
	/* f20 58 */ float wfh;
	/* f21 59 */ float f;
	/* f23 61 */ float lw;
	/* f26 64 */ float fw;
	/* f24 62 */ float lm;
	/* f22 60 */ float fx;
	/* f20 58 */ float fy;
	/* f23 61 */ float fz;
	/* f2 40 */ float yoff;
	/* 0x0(sp) */ SCRDEF scrdef[25][33];
	/* v0 2 */ SCRDEF *pscr;
	/* s0 16 */ DEFWORK *pdef;
	// /* f21 59 */ float r;
    u_long tex0;

    // pnumw = 32;
    vnumw = 33;
    // pnumh = 24;
    vnumh = 25;
    
    LocalCopyLtoLDraw((sys_wrk.count & 1) * 0x8c0, 0x1a40);
    
    yoff = GetYOffsetf();
    
    for (j = 0; j < vnumh; j++)
    {
        for (i = 0; i < vnumw; i++)
        {
            pscr = &scrdef[j][i];
            
            pscr->stq[0] = (i * 640) / 32.0f;
            pscr->stq[1] = (j * 224) / 24.0f + yoff;
        }
        
        scrdef[j][0].stq[0] += 1.0f;
        scrdef[j][vnumw-1].stq[0] -= 1.0f;
    }
    
    for (i = 0; i < vnumw; i++)
    {
        scrdef[0][i].stq[1] += 1.0f;
        scrdef[vnumh-1][i].stq[1] -= 1.0f;
    }
    
    fw = rate / 4.0f;
    lm = SgSqrtf(224 * 224 + 320 * 320);
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
                
                pdef->lll = SgSqrtf(wfw * wfw + wfh * wfh);
                
                if ((wix != (pnumw / 2) % vnumw) || (wiy != (pnumh / 2) % vnumw))
                {
                    pdef->rrr = SgAtan2f(wfw, wfh);
                }
                else
                {
                    pdef->rrr = 0.0f;
                }
                
                pdef->mm2 = (lm - pdef->lll) * lw;
                pdef->mm1 = pdef->lll * PI * 12.0f * lw;
                
                pdef->sss = SgSinf(pdef->rrr);
                pdef->ccc = SgCosf(pdef->rrr);
            }
        }
        
        eff_deform.init = 0;
    }

    fz = r; // unlikely?
    // or lw?
    
    for (j = 0; j < vnumh; j++)
    {
        for (i = 0; i < vnumw; i++)
        {
            f = vu0Rand() + 1.0f;
            
            pdef = &dw[j][i];
            
            fx = pdef->lll;
            fy = SgSinf(pdef->mm1 - fz) * fw * pdef->mm2 * SgSinf((fz + pdef->rrr) * 18.0f) * f;
            
            pscr = &scrdef[j][i];
            
            pscr->vtw[0] = (pdef->sss * fx - pdef->ccc * fy) + 2048.0f;
            pscr->vtw[1] = (pdef->ccc * fx - pdef->sss * fy) * 0.5f + 2048.0f;
            pscr->vtw[2] = 0.0f;
            pscr->vtw[3] = 0.0f;
        }
    }
    
    add = 0.02f;
    
    if (stop_effects == 0)
    {
        r = r + add <= PI2 ? r + add : (r + add) - PI2;

        // HACK: force loading of a0 and a1 before a2 for MakeScrDeformPacket 
        tex0 = SCE_GS_SET_TEX0(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
    }
    
    MakeScrDeformPacket(
        vnumw - 1, vnumh - 1,
        tex0,
        scrdef, alp);
    
    eff_deform.pass = 1;
    eff_deform.type = type;
}

void SetDeform5(/* 0x6720(sp) */ int type, /* f21 59 */ float rate, /* 0x6724(sp) */ u_char alp)
{
	/* sdata 356488 */ static float r = 0.0f;
	/* sdata 35648c */ static float add = 6.0f;
	/* s1 17 */ int i;
	/* s2 18 */ int j;
	/* s7 23 */ int pnumw;
	/* s4 20 */ int vnumw;
	/* 0x6728(sp) */ int wix;
	/* s0 16 */ int pnumh;
    int vnumh;
	/* 0x672c(sp) */ int wiy;
	/* f21 59 */ float wfw;
	/* f20 58 */ float wfh;
	/* f21 59 */ float f;
	/* f23 61 */ float lw;
	/* f25 63 */ float fw;
	/* f24 62 */ float lm;
	/* f22 60 */ float fx;
	/* f20 58 */ float fy;
	/* f23 61 */ float fz;
	/* 0x0(sp) */ SCRDEF scrdef[25][33];
	/* v0 2 */ SCRDEF *pscr;
	/* f0 38 */ float xx;
	/* f2 40 */ float yy;
	/* s0 16 */ DEFWORK *pdef;
	// /* f21 59 */ float r;
    u_long tex0;

    // pnumw = 32;
    vnumw = 33;
    // pnumh = 24;
    vnumh = 25;
    
    LocalCopyLtoLDraw((sys_wrk.count & 1) * 0x8c0, 0x1a40);
    
    fx = 20.0f;
    fy = (56.0f / 6.0f);
    
    yy = GetYOffsetf();
    
    for (j = 0; j < vnumh; j++)
    {
        pdef = &dw[j][i];
        
        for (i = 0, xx = 0.0f; i < vnumw; i++)
        {
            pscr = &scrdef[j][i];
            
            pscr->stq[0] = xx;
            pscr->stq[1] = yy;

            xx += fx;
        }
        
        scrdef[j][0].stq[0] += 1.0f;
        scrdef[j][vnumw-1].stq[0] -= 1.0f;
        
        yy += fy;
    }
    
    for (i = 0; i < vnumw; i++)
    {
        scrdef[0][i].stq[1] += 1.0f;
        scrdef[vnumh-1][i].stq[1] -= 1.0f;
    }
    
    fw = rate / 4.0f;
    lm = SgSqrtf(224 * 224 + 320 * 320);
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
                
                pdef->lll = SgSqrtf(wfw * wfw + wfh * wfh);
                
                if ((wix != (pnumw / 2) % vnumw) || (wiy != (pnumh / 2) % vnumw))
                {
                    pdef->rrr = SgAtan2f(wfw, wfh);
                }
                else
                {
                    pdef->rrr = 0.0f;
                }
                
                pdef->mm2 = (lm - pdef->lll) * lw;
                pdef->mm1 = pdef->lll * PI * 2.0f * lw;
                
                pdef->sss = SgSinf(pdef->rrr);
                pdef->ccc = SgCosf(pdef->rrr);
            }
        }
        
        eff_deform.init = 0;
    }
    
    sys_wrk.count = sys_wrk.count; // HACK: fix v1<->v0 reg-swap

    fz = r; // unlikely?
    
    for (j = 0; j < vnumh; j++)
    {
        for (i = 0; i < vnumw; i++)
        {
            pdef = &dw[j][i];
            
            f = vu0Rand() + 1.0f;
            
            fx = pdef->lll;
            fy = SgSinf(pdef->mm1 - fz) * fw * pdef->mm2 * SgSinf((fz + pdef->rrr) * 18.0f) * f;
            
            pscr = &scrdef[j][i];
            
            pscr->vtw[0] = (pdef->sss * fx - pdef->ccc * fy) + 2048.0f;
            pscr->vtw[1] = (pdef->ccc * fx - pdef->sss * fy) * 0.5f + 2048.0f;
            pscr->vtw[2] = 0.0f;
            pscr->vtw[3] = 0.0f;
        }
    }
    
    add = 0.02f;
    
    if (stop_effects == 0)
    {
        r = r + add <= PI2 ? r + add : (r + add) - PI2;

        // HACK: force loading of a0 and a1 before a2 for MakeScrDeformPacket 
        tex0 = SCE_GS_SET_TEX0(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
    }
    
    MakeScrDeformPacket(
        vnumw - 1, vnumh - 1,
        tex0,
        scrdef, alp);
    
    eff_deform.pass = 1;
    eff_deform.type = type;
}

void SetDeform6(/* 0x6720(sp) */ int type, /* f20 58 */ float rate, /* 0x6724(sp) */ u_char alp)
{
    /* sdata 356490 */ static float r = 0.0f;
	/* sdata 356494 */ static float add = 6.0f;
	/* s2 18 */ int i;
	/* s3 19 */ int j;
	/* s5 21 */ int vnumw;
    int vnumh;
    int pnumw;
    int pnumh;
	/* s7 23 */ int wix;
	/* fp 30 */ int wiy;
	/* f21 59 */ float wfw;
	/* f20 58 */ float wfh;
	/* f21 59 */ float f;
	/* f22 60 */ float lw;
	/* f25 63 */ float fw;
	/* f24 62 */ float lm;
	/* f22 60 */ float fx;
	/* f20 58 */ float fy;
	/* f23 61 */ float fz;
	/* f2 40 */ float yoff;
	/* 0x0(sp) */ SCRDEF scrdef[25][33];
	/* s1 17 */ SCRDEF *pscr;
	/* s0 16 */ DEFWORK *pdef;
	// /* f21 59 */ float r;
    u_long tex0;

    // pnumw = 32;
    vnumw = 33;
    // pnumh = 24;
    vnumh = 25;
    
    LocalCopyLtoLDraw((sys_wrk.count & 1) * 0x8c0, 0x1a40);
    
    yoff = GetYOffsetf();
    
    for (j = 0; j < vnumh; j++)
    {
        pdef = &dw[j][i];
        
        for (i = 0; i < vnumw; i++)
        {
            pscr = &scrdef[j][i];
            
            pscr->stq[0] = (i * 640) / 32.0f;
            pscr->stq[1] = (j * 224) / 24.0f + yoff;
        }
        
        scrdef[j][0].stq[0] += 1.0f;
        scrdef[j][vnumw-1].stq[0] -= 1.0f;
    }
    
    for (i = 0; i < vnumw; i++)
    {
        scrdef[0][i].stq[1] += 1.0f;
        scrdef[vnumh-1][i].stq[1] -= 1.0f;
    }
    
    fw = rate / 4.0f;
    lm = SgSqrtf(224 * 224 + 320 * 320);
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
                
                pdef->lll = SgSqrtf(wfw * wfw + wfh * wfh);
                
                if ((wix != (33/2) % vnumw) || (wiy != (25/2) % vnumw))
                {
                    pdef->rrr = SgAtan2f(wfw, wfh);
                }
                else
                {
                    pdef->rrr = 0.0f;
                }
                
                pdef->mm2 = (lm - pdef->lll) * lw;
                pdef->mm1 = pdef->lll * PI * 2.0f * lw;
                
                pdef->sss = SgSinf(pdef->rrr);
                pdef->ccc = SgCosf(pdef->rrr);
            }
        }
        
        eff_deform.init = 0;
    }

    fz = r; // unlikely?
    // or lw?
    
    for (j = 0; j < vnumh; j++)
    {
        for (i = 0; i < vnumw; i++)
        {
            pdef = &dw[j][i];
            pscr = &scrdef[j][i];
            
            f = vu0Rand() + 1.0f;
            
            fx = pdef->lll;
            fy = SgSinf(pdef->mm1 - fz) * fw * pdef->mm2 * SgSinf((fz + pdef->rrr) * 18.0f) * f;
            
            pscr->vtw[0] = (pdef->sss * fx - pdef->ccc * fy) + 2048.0f;
            pscr->vtw[1] = (pdef->ccc * fx + pdef->sss * fy) * 0.5f + 2048.0f;
            pscr->vtw[2] = 0.0f;
            pscr->vtw[3] = 0.0f;
        }
    }
    
    add = 0.02f;
    
    if (stop_effects == 0)
    {
        r = r + add <= PI2 ? r + add : (r + add) - PI2;

        // HACK: force loading of a0 and a1 before a2 for MakeScrDeformPacket 
        tex0 = SCE_GS_SET_TEX0(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
    }
    
    MakeScrDeformPacket(
        vnumw - 1, vnumh - 1,
        tex0,
        scrdef, alp);
    
    eff_deform.pass = 1;
    eff_deform.type = type;
}

void SubContrast2(/* t5 13 */ u_char col, /* t6 14 */ u_char alp)
{
	/* 0x0(sp) */ SPRITE_DATA sd = {
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
	/* 0x60(sp) */ DRAW_ENV de = {
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
    sd.pos_z = 0xfffffdf;
    sd.size_w = 640.0f;
    sd.size_h = 448.0f;
    
    SetTexDirectS2(0, &sd, &de, 1);
}

void SetContrast2(/* s0 16 */ EFFECT_CONT *ec)
{
    SubContrast2(ec->dat.uc8[2], ec->dat.uc8[3]);

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SubContrast3(/* t4 12 */ u_char col, /* t5 13 */ u_char alp)
{
    /* 0x0(sp) */ SPRITE_DATA sd = {
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
	/* 0x60(sp) */ DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1(1, 0, SCE_GS_NEAREST, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_ZERO, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GREATER, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL),
        .clamp = 0,
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 326, SCE_GIF_PACKED, 5),
    };
    
    sd.r = col;
    sd.g = col;
    sd.b = col;
    sd.alpha = alp;
    
    LocalCopyLtoL((sys_wrk.count & 1) * 0x8c0, 0x1a40);
    
    sd.g_GsTex0.TBP0 = 0;
    sd.pos_x = -320.5f;
    sd.pos_y = -224.5f;
    sd.pos_z = 0xffffcdf;
    sd.size_w = 640.0f;
    sd.size_h = 448.0f;
    
    SetTexDirectS2(0x320, &sd, &de, 1);
    
    sd.pos_z = 0xffffcef;
    
    SetTexDirectS2(0x310, &sd, &de, 1);
}

void SetContrast3(/* s0 16 */ EFFECT_CONT *ec)
{
    SubContrast3(ec->dat.uc8[2], ec->dat.uc8[3]);
    
    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SubNega(/* s0 16 */ u_char r, /* s1 17 */ u_char g, /* s3 19 */ u_char b, /* s4 20 */ u_char alp, /* s2 18 */ u_char alp2)
{
	/* 0x0(sp) */ SPRITE_DATA sd = {
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
	/* 0x60(sp) */ DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1(1, 0, SCE_GS_NEAREST, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_ZERO, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GREATER, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL),
        .clamp = 0,
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 326, SCE_GIF_PACKED, 5),
    };

    LocalCopyLtoL((sys_wrk.count & 1) * 0x8c0, 0x1a40);
    
    sd.r = r;
    sd.g = g;
    sd.b = b;
    sd.alpha = alp;
    sd.g_GsTex0.TBP0 = 0;
    sd.pos_x = -320.5;
    sd.pos_y = -224.5;
    sd.pos_z = 0xffffcdf;
    sd.size_w = 640.0f;
    sd.size_h = 448.0f;
    
    SetTexDirectS2(0x320, &sd, &de, 1);
    
    sd.r = 0x80;
    sd.g = 0x80;
    sd.b = 0x80;
    sd.alpha = alp2 << 0x18 < 0 ? 0 : 0x80 - alp2;
    sd.g_GsTex0.TBP0 = 0x1a40;
    sd.pos_z = 0xffffcef;
    de.alpha = 0x44;
    de.prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 342, SCE_GIF_PACKED, 5);
    
    SetTexDirectS2(0x310, &sd, &de, 1);
}

void SetNega(/* s0 16 */ EFFECT_CONT *ec)
{
	/* s3 19 */ u_char col;
	/* s2 18 */ u_char alp;
	/* s1 17 */ u_char alp2;
    
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

void* CallNega2(/* a0 4 */ int in, /* a1 5 */ int keep, /* a2 6 */ int out)
{
	/* sdata 356498 */ static u_char alp = 0x80;

    return SetEffects(0xc, 4, 64, 196, in, keep, out);
}

void* CallNega(/* a0 4 */ int time)
{
    return CallNega2(0, time, 0);
}

void SetOverRap(/* s0 16 */ EFFECT_CONT *ec)
{
	/* sdata 35649c */ static float cx = 0.0f;
	/* sdata 3564a0 */ static float cy = 0.0f;
	/* sdata 3564a4 */ static float cz = 0.0f;
	/* sdata 3564a8 */ static float alp = 0.0f;
	/* f4 42 */ float x;
	/* f2 40 */ float y;
	/* f3 41 */ float z;
	/* f0 38 */ float fn;
	/* v1 3 */ int fl;
	/* v1 3 */ int ret;
	/* 0x0(sp) */ SPRITE_DATA sd;
	/* 0x60(sp) */ DRAW_ENV de;

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
    
    if ((ret && alp <= 0.0f) || overlap_passflg[0] != overlap_passflg[1])
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
            .clamp = 0,
            .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 342, SCE_GIF_PACKED, 5),
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

void SetForcusDepth(/* 0xe0(sp) */ EFFECT_CONT *ec)
{
	/* s5 21 */ int i;
	/* 0x0(sp) */ int zi[2];
	/* 0x10(sp) */ float bai[2] = { 1.5f, 2.2f };
	/* 0x20(sp) */ sceVu0FMATRIX wlm;
	/* 0x60(sp) */ sceVu0FMATRIX slm;
	/* 0xa0(sp) */ sceVu0FVECTOR vt = { 0.0f, 0.0f, 0.0f, 1.0f };
	/* 0xb0(sp) */ sceVu0FVECTOR vtw;
	/* 0xc0(sp) */ sceVu0FVECTOR vtww;
	/* 0xd0(sp) */ sceVu0IVECTOR ivec;
    
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

void SetForcusDepth2(/* s0 16 */ EFFECT_CONT *ec)
{
	/* 0x0(sp) */ int zi[4];
	/* 0x10(sp) */ sceVu0FMATRIX wlm;
	/* 0x50(sp) */ sceVu0FMATRIX slm;
	/* 0x90(sp) */ sceVu0FVECTOR vt = { 0.0f, 0.0f, 0.0f, 1.0f };
	/* 0xa0(sp) */ sceVu0FVECTOR vtw;
	/* 0xb0(sp) */ sceVu0FVECTOR vtww;
	/* 0xc0(sp) */ float bai[4] = { 0.5f, 0.69999999f, 1.5f, 2.2f };
	/* sdata 3564b8 */ static float ff = 0.0f;
	/* 0xd0(sp) */ sceVu0IVECTOR ivec;

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void MakeRDither3()
{
	/* 0x0(sp) */ u_char pat[16384];
	/* 0x4000(sp) */ u_int pal[256];
	/* a3 7 */ int i;
	// /* f0 38 */ float r;
	// /* f0 38 */ float r;
	/* bss 36a380 */ static sceGsLoadImage gs_limage1;
	/* bss 36a3e0 */ static sceGsLoadImage gs_limage2;
    
    for (i = 0; i < 16384; i++)
    {
        pat[i] = (int)(vu0Rand() * 64.0f);
    }
    
    for (i = 0; i < 256; i++)
    {
        
        pal[i] = (u_char)(vu0Rand() * 128.0f);
        pal[i] = \
            i      << 24 | \
            pal[i] << 16 | \
            pal[i] <<  8 | \
            pal[i] <<  0;
    }
    
    sceGsSetDefLoadImage(&gs_limage1, 0x37fc, 2, 0x13, 0, 0, 128, 128);
    sceGsSetDefLoadImage(&gs_limage2, 0x383c, 1, 0, 0, 0, 16, 32);
    
    FlushCache(0);
    
    sceGsExecLoadImage(&gs_limage1, (u_long128 *)pat);
    sceGsExecLoadImage(&gs_limage2, (u_long128 *)pal);
    
    sceGsSyncPath(0, 0);
}

void SubDither3(/* s0 16 */ int type, /* f27 65 */ float alp, /* f28 66 */ float spd, /* 0x4530(sp) */ u_char alpmx, /* 0x4534(sp) */ u_char colmx)
{
	/* data 280a30 */ static sceVu0FVECTOR old_cam_i = { 0.0f, 0.0f, 0.0f, 0.0f };
	/* sdata 3564bc */ static float cnf = 0.0f;
	/* sdata 3564c0 */ static float cx = 0.0f;
	/* sdata 3564c4 */ static float cy = 0.0f;
	/* sdata 3564c8 */ static int fl = 1;
	/* 0x4520(sp) */ float tx;
	/* 0x4524(sp) */ float ty;
	/* 0x4528(sp) */ float otx;
	/* 0x452c(sp) */ float oty;
	/* f1 39 */ float mvx;
	/* f4 42 */ float mvy;
	/* 0x0(sp) */ SPRITE_DATA sd = {
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
        .r = 128,
        .g = 128,
        .b = 128,
        .alpha = 128,
        .mask = 0,
    };
	/* 0x60(sp) */ SPRITE_DATA sd2 = {
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
        .pos_x = -320.5,
        .pos_y = -224.5,
        .pos_z = -4096,
        .size_w = 640.0,
        .size_h = 448.0,
        .scale_w = 1.0,
        .scale_h = 1.0,
        .clamp_u = 0,
        .clamp_v = 0,
        .rot_center = 0,
        .angle = 0.0,
        .r = 128,
        .g = 128,
        .b = 128,
        .alpha = 128,
        .mask = 0,
    };
	/* 0xc0(sp) */ DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1(1, 0, SCE_GS_NEAREST, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS),
        .clamp = 0,
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 342, SCE_GIF_PACKED, 5),
    };
	/* 0xf0(sp) */ DRAW_ENV de2 = {
        .tex1 = SCE_GS_SET_TEX1(1, 0, SCE_GS_NEAREST, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CS, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CS, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS),
        .clamp = 0,
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 342, SCE_GIF_PACKED, 5),
    };
	/* 0x120(sp) */ u_char pat[16384];
	/* 0x4120(sp) */ u_int pal[256];
	/* a3 7 */ int i;
	/* sdata 3564cc */ static u_char oalp = 0;
	/* sdata 3564cd */ static u_char ocol = 0;
	/* sdata 3564d0 */ static int numf = 0;
	// /* f0 38 */ float r;
	// /* f0 38 */ float r;
	/* bss 36a440 */ static sceGsLoadImage gs_limage1;
	/* bss 36a4a0 */ static sceGsLoadImage gs_limage2;


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
                pat[i] = (int)(alpmx * vu0Rand());
            }
            
            for (i = 0; i < 256; i++)
            {
                pal[i] = (u_char)(colmx * vu0Rand());
                pal[i] = \
                    i      << 24 | \
                    pal[i] << 16 | \
                    pal[i] <<  8 | \
                    pal[i] <<  0;
            }
            
            sceGsSetDefLoadImage(&gs_limage1, 0x37fc, 2, 0x13, 0, 0, 0x80, 0x80);
            sceGsSetDefLoadImage(&gs_limage2, 0x383c, 1, 0, 0, 0, 0x10, 0x20);
            FlushCache(0);
            sceGsExecLoadImage(&gs_limage1, (u_long128 *)pat);
            sceGsExecLoadImage(&gs_limage2, (u_long128 *)pal);
            sceGsSyncPath(0,0);
            WaitFrameTop(0);
        }
    }
    
    if (oalp != alpmx || ocol != colmx)
    {
        numf = 1;
    }

    oalp = alpmx;
    ocol = colmx;

    // NOTE: This is what causes the boss hang.
    //       A potential Division by zero case causes MaxFloat
    //       to be returned MaxFloat causes the loops to stall
    GetCamI2DPos(camera.i, &tx, &ty);
    GetCamI2DPos(old_cam_i, &otx, &oty);
    sceVu0CopyVector(old_cam_i, camera.i);
    
    
    if (isnan(tx) || isnan(ty) || isnan(otx) || isnan(oty))
    {
        mvx = 0.0f;
        mvy = 0.0f;
    }
    else
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
    sd2.alpha = (SgSinf((cnf * PI) / 180.0f) + 1.0f) * alp;
    
    SetTexDirectS2(-2, &sd2, &de2, 0);
    
    sd2.clamp_u = (int)((cx + 704.0f) * 16.0f) << 16 | (int)((cx + 64.0f) * 16.0f);
    sd2.clamp_v = (int)((cy + 512.0f) * 16.0f) << 16 | (int)(cy * 16.0f);
    sd2.alpha = (SgSinf(((cnf + 120.0f) * PI) / 180.0f) + 1.0f) * alp;
    
    SetTexDirectS2(-2, &sd2, &de2, 0);
    
    sd2.clamp_u = (int)((cx + 640.0f) * 16.0f) << 16 | (int)(cx * 16.0f);
    sd2.clamp_v = (int)((cy + 576.0f) * 16.0f) << 16 | (int)((cy + 64.0f) * 16.0f);
    sd2.alpha = (SgSinf(((cnf + 240.0f) * PI) / 180.0f) + 1.0f) * alp;
    
    SetTexDirectS2(-2, &sd2, &de2, 0);
    
    if (stop_effects == 0)
    {
        cnf += spd;
    }
}

void SetDither3(/* s0 16 */ EFFECT_CONT *ec)
{
	/* s2 18 */ u_char alpmx;
	/* s3 19 */ u_char colmx;
	/* s1 17 */ int type;
	/* f21 59 */ float spd;
	/* f20 58 */ float alp;    

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

void SubDither4(/* f27 65 */ float alp, /* f28 66 */ float spd)
{
	/* data 280a40 */ static sceVu0FVECTOR old_cam_i = { 0.0f, 0.0f, 0.0f, 0.0f };
	/* sdata 3564d4 */ static float cnf = 0.0f;
	/* sdata 3564d8 */ static float cx = 0.0f;
	/* sdata 3564dc */ static float cy = -9908638.0f;
	/* sdata 3564e0 */ static int fl = 1;
	/* 0x90(sp) */ float tx;
	/* 0x94(sp) */ float ty;
	/* 0x98(sp) */ float otx;
	/* 0x9c(sp) */ float oty;
	/* f1 39 */ float mvx;
	/* f5 43 */ float mvy;
	/* 0x0(sp) */ SPRITE_DATA sd = {
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
	/* 0x60(sp) */ DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1(1, 0, SCE_GS_NEAREST, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS),
        .clamp = 0,
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 342, SCE_GIF_PACKED, 5),
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
    
    if (isnan(tx) || isnan(ty) || isnan(otx) || isnan(oty))
    {
        mvx = 0.0f;
        mvy = 0.0f;
    }
    else
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
    sd.alpha = (SgSinf((cnf * PI) / 180.0f) + 1.0f) * alp;
    
    SetTexDirectS2(-2, &sd, &de, 0);
    
    sd.clamp_u = (int)((cx + 704.0f) * 16.0f) << 16 | (int)((cx + 64.0f) * 16.0f);
    sd.clamp_v = (int)((cy + 512.0f) * 16.0f) << 16 | (int)(cy * 16.0f);
    sd.alpha = (SgSinf(((cnf + 120.0f) * PI) / 180.0f) + 1.0f) * alp;
    
    SetTexDirectS2(-2, &sd, &de, 0);
    
    sd.clamp_u = (int)((cx + 640.0f) * 16.0f) << 16 | (int)(cx * 16.0f);
    sd.clamp_v = (int)((cy + 576.0f) * 16.0f) << 16 | (int)((cy + 64.0f) * 16.0f);
    sd.alpha = (SgSinf(((cnf + 240.0f) * PI) / 180.0f) + 1.0f) * alp;
    
    SetTexDirectS2(-2, &sd, &de, 0);
    
    if (stop_effects == 0)
    {
        cnf += spd;
    }
}

void SetDither4(/* s0 16 */ EFFECT_CONT *ec)
{
    SubDither4(ec->dat.fl32[2], ec->dat.fl32[3]);

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SubFadeFrame(/* t8 24 */ u_char alp, /* a1 5 */ u_int pri)
{
	/* 0x0(sp) */ SPRITE_DATA sd = {
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
	/* 0x60(sp) */ DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, 0, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL),
        .clamp = 0,
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 342, SCE_GIF_PACKED, 5),
    };

    sd.g_GsTex0 = *(sceGsTex0 *)&effdat[12].tex0;
    sd.g_nTexSizeW = effdat[12].w;
    sd.g_nTexSizeH = effdat[12].h;
    sd.pos_z = 0xfffffff - pri;
    sd.alpha = alp;
    
    SetTexDirectS2(pri, &sd, &de, 0);
}

void SetFadeFrame(/* s0 16 */ EFFECT_CONT *ec)
{
    SubFadeFrame(ec->dat.uc8[2], ec->dat.ui32[1]);

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void ChangeMAGATOKI2(/* a0 4 */ int sw)
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
	/* s0 16 */ EFFECT_CONT *ecm;
	/* sdata 3564e4 */ static u_char alpr = 0x39;
    
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
        
        // floats are promoted do doubles due to SetEffects being a vararg function
        if (plyr_wrk.mode != 0x1)
        {
            SetEffects(3, 1, &alpr, 1015, 1800, 320.0f, 112.0f);
            SetEffects(6, 1, 2, 12);
            SetEffects(13, 1, 70, 70);
            SetEffects(2, 1, 2, 24.0f, 8.0f, 106 ,101);
            SetEffects(9, 1, 30, 0x80000);
        }
        else
        {
            SetEffects(6, 1, 2, 12);
            SetEffects(13, 1, 70, 70);
            SetEffects(2, 1, 2, 24.0f, 8.0f, 106, 101);
            SetEffects(9, 1, 30, 0x80000);
        }
    }
}

void SetMAGATOKI(/* s0 16 */ EFFECT_CONT *ec)
{
	/* sbss 3578e4 */ static int cnt1;
	/* s2 18 */ u_char *flag;
	/* f18 56 */ float per1;
	/* f28 66 */ float mbalp;
	/* f27 65 */ float mbscl;
	/* f26 64 */ float mbrot;
	/* f25 63 */ float mccol;
	/* f23 61 */ float mcalp;
	/* f24 62 */ float mdalp;
	/* f22 60 */ float mdspd;
	/* f21 59 */ float mdfrt;
	/* f20 58 */ float mdfal;
	/* a2 6 */ int sec1_1;
	/* s1 17 */ int sec1_2;
	/* 0x0(sp) */ float balp[3] = { 57.0f, 90.0f, 57.0f };
	/* 0x10(sp) */ float bscl[3] = { 1.0f, 1.007f, 1.015f };
	/* 0x20(sp) */ float brot[3] = { 180.0f, 180.3f, 180.0f };
	/* 0x30(sp) */ float ccol[3] = { 70.0f, 70.0f, 70.0f };
	/* 0x40(sp) */ float calp[3] = { 70.0f, 140.0f, 70.0f };
	/* 0x50(sp) */ float dalp[3] = { 0.0f, 128.0f, 24.0f };
	/* 0x60(sp) */ float dspd[3] = { 8.0f, 8.0f, 8.0f };
	/* 0x70(sp) */ float dfrt[3] = { 0.0f, 0.0f, 12.0f };
	/* 0x80(sp) */ float dfal[3] = { 0.0f, 0.0f, 128.0f };
    
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

// STAB says u_char but doesn't match if we return something => assume devs forgot to declare it void and don't return anything
/* 0012ea48 00000ce8 */ u_char SubNowLoading(/* a0 4 */ int fl, /* 0x3440(sp) */ int num, /* f12 50 */ float sclx, /* f28 66 */ float scly, /* f27 65 */ float bx, /* f22 60 */ float by, /* f16 54 */ float spd, /* f29 67 */ float rate, /* f21 59 */ float trate)
{
	/* 0x0(sp) */ u_char alpha1[289] =   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x08, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x20, 0x40, 0x40, 0x40, 0x40, 0x20, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x20, 0x40, 0x60, 0x80, 0x80, 0x80, 0x60, 0x60, 0x40, 0x20, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x20, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x60, 0x40, 0x20, 0x10, 0x00, 0x00, 0x01, 0x20, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x60, 0x40, 0x20, 0x00, 0x00, 0x10, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x60, 0x20, 0x00, 0x01, 0x20, 0x60, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x60, 0x40, 0x01, 0x08, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x08, 0x08, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x08, 0x08, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x08, 0x01, 0x40, 0x60, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x60, 0x20, 0x01, 0x00, 0x20, 0x60, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x10, 0x00, 0x00, 0x20, 0x40, 0x60, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x20, 0x01, 0x00, 0x00, 0x10, 0x20, 0x40, 0x60, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x08, 0x20, 0x40, 0x60, 0x60, 0x80, 0x80, 0x80, 0x60, 0x40, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x20, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x08, 0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00 };
	/* 0x130(sp) */ u_char alpha2[289] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x08, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x20, 0x40, 0x40, 0x40, 0x40, 0x20, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x20, 0x40, 0x58, 0x68, 0x68, 0x68, 0x58, 0x58, 0x40, 0x20, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x20, 0x40, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x58, 0x40, 0x20, 0x10, 0x00, 0x00, 0x01, 0x20, 0x40, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x58, 0x40, 0x20, 0x00, 0x00, 0x10, 0x40, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x58, 0x20, 0x00, 0x01, 0x20, 0x58, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x58, 0x40, 0x01, 0x08, 0x40, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x40, 0x08, 0x08, 0x40, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x40, 0x08, 0x08, 0x40, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x40, 0x08, 0x01, 0x40, 0x58, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x58, 0x20, 0x01, 0x00, 0x20, 0x58, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x40, 0x10, 0x00, 0x00, 0x20, 0x40, 0x58, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x40, 0x20, 0x01, 0x00, 0x00, 0x10, 0x20, 0x40, 0x58, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, 0x40, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x08, 0x20, 0x40, 0x58, 0x58, 0x68, 0x68, 0x68, 0x58, 0x40, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, 0x20, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x08, 0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00 };
	/* s1 17 */ int i;
	/* t8 24 */ int j;
	/* s2 18 */ int k;
    int l;
	/* s5 21 */ int m;
	/* s4 20 */ int bak;
    int pnumw;
	/* s4 20 */ int vnumw;
	/* v0 2 */ int pnumh;
	/* fp 30 */ int vnumh;
    int sx2;
	/* s7 23 */ int sy2;
	/* 0x260(sp) */ int tx[289];
	/* 0x6f0(sp) */ int ty[289];
	/* 0xb80(sp) */ u_int clip[289];
    u_int clpx2;
    u_int clpy2;
	/* 0x3444(sp) */ u_int clpz2;
	/* f0 38 */ float f1;
	/* f1 39 */ float f2;
	/* f8 46 */ float f3;
	/* f9 47 */ float f4;
	/* f30 68 */ float bxf;
	/* f31 69 */ float byf;
	/* f26 64 */ float pr11;
	/* f25 63 */ float pr12;
	/* f24 62 */ float pr21;
	/* f23 61 */ float pr22;
	/* 0x1010(sp) */ sceVu0FVECTOR vt[289];
	/* 0x2220(sp) */ sceVu0IVECTOR vtiw[289];
	/* 0x3430(sp) */ sceVu0IVECTOR ivec;
	/* v0 2 */ u_long tex0;
	/* s0 16 */ EFFPOS *pefp;
	/* s2 18 */ EFFINFO2 *pefi;
	// /* f0 38 */ float r;
	// /* f1 39 */ float r;
	// /* f0 38 */ float r;
	// /* f1 39 */ float r;
	// /* f1 39 */ float r;
	// /* f0 38 */ float r;
	// /* f1 39 */ float r;
	// /* f0 38 */ float r;
    float c;

    sx2 = (SCR_WIDTH - 1) * 16; // 0x27f0
    sy2 = (SCR_HEIGHT - 1) * 16; // 0xdf0 // Line 4684

    pnumh = 16;
    pnumw = 16;
    vnumw = 17; // Line 4717
    vnumh = 17; // Line 4718
    
    if (0/vnumw) // Line 4719
    {
        c = 0.0f; // this fixes a big diff around here ...
    }
    
    c = 1.0f;
    
    clpx2 = 0xfd00;
    clpy2 = 0xfd00;
    clpz2 = 0x0fffffff; // Line 4694
    
    pr11 =  1.2f; // Line 4722
	pr12 = -0.6f; // Line 4723
	pr21 =  0.6f; // Line 4724
	pr22 =  0.3f; // Line 4725

    tex0 = SCE_GS_SET_TEX0(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);   
    
    if (fl != 0)
    {
        SetVURand(rand() / (float)RAND_MAX); // Line 4729
        
        for (j = 0; j < 2; j++)
        {
            pefi = &efi[j];
            
            for (i = 0; i < vnumw*vnumh; i++)
            {
                int q_i, r_i;
                
                q_i = i / (vnumw);
                r_i = i % (vnumw);
                
                if (r_i == 0)
                {
                    pefi->ep[i].h = pr11 * vu0Rand() + pr12;
                }
                else
                {
                    pefi->ep[i].h = pefi->ep[i - 1].h + pr21 * vu0Rand() - pr22;
                }
                
                if (q_i == 0)
                {
                    pefi->ep[i].r = pr11 * vu0Rand() + pr12;
                }
                else
                {
                    pefi->ep[i].r = pefi->ep[i - vnumw].r + pr21 * vu0Rand() - pr22;
                }
            }
        }
    }
    
    bxf = bx + 2048.0f;
    byf = by + 2048.0f;

    f3 = (sclx * 2) * 8.0f;
    f4 = (scly * 1) * 8.0f;
    
    for (i = 0; i < vnumw*vnumh; i++) // Line 4763
    {
        int q_i, r_i;
                
        q_i = i / (vnumw); // Line 4764
        r_i = i % (vnumw);

        f1 = (int)(r_i) * (sclx * 2);
        f2 = (int)(q_i) * (scly * 1);
        
        vt[i][0] = (f1 - f3) * trate; // Line 4769
        vt[i][1] = (f2 - f4) * trate; // Line 4770
        
        tx[i] = (int)((f1 - f3 + bx + 320.0f) * 16.0f); // Line 4772
        ty[i] = (int)((f2 - f4 + by + 112.0f) * 16.0f); // Line 4773

        tx[i] = (tx[i] < 0) ? 0 : ((sx2 < tx[i]) ? sx2 : tx[i]); // Line 4774
        ty[i] = (ty[i] < 0) ? 0 : ((sy2 < ty[i]) ? sy2 : ty[i]); // Line 4775

        vt[i][0] = f1 - f3; // Line 4777
        vt[i][1] = f2 - f4; // Line 4778
        vt[i][2] = 0.0f; // Line 4779
        vt[i][3] = 1.0f; // Line 4780
    }
    
    pefi = &efi[num];
    
    for (i = 0; i < vnumw*vnumh; i++)
    {
        int q_i, r_i;
                
        q_i = i / (vnumw);
        r_i = i % (vnumw);
        
        pefp = &pefi->ep[i];
        
        pefp->x = SgSinf(((pefi->r + q_i * 32.0f) * PI) / 180.0f) * pefp->r * rate; // Line 4797
        pefp->y = SgSinf(((pefi->r + r_i * 32.0f) * PI) / 180.0f) * pefp->h * rate; // Line 4800
        
        if (0.0f <= pefp->y && pefp->oy < 0.0f)
        {
            if (r_i != 0) // Line 4801
            {
                pefp->h = pefi->ep[i - 1].h + pr21 * vu0Rand() - pr22; // Line 4803
            }
            else
            {
                pefp->h = pr11 * vu0Rand() + pr12; // Line 4804
            }
        }
        
        if (0.0f <= pefp->x && pefp->ox < 0.0f)
        {
            if (q_i != 0) // Line 4809
            {
                pefp->r = pefi->ep[i - vnumw].r + pr21 * vu0Rand() - pr22; // Line 4811
            }
            else
            {
                pefp->r = pr11 * vu0Rand() + pr12; // Line 4812
            }
        }
        
        pefp->ox = pefp->x; // Line 4816
        pefp->oy = pefp->y; // Line 4817
        
        vt[i][0] += pefp->x; // Line 4819
        vt[i][1] += pefp->y; // Line 4791
    }

    pefi->r = \
        ((pefi->r + spd * 2) <= 360.0f) ? \
        pefi->r + spd * 2 : \
        pefi->r + spd * 2 - 360.0f;
    
    for (i = 0; i < vnumw*vnumh; i++)
    {
        vtiw[i][0] = (int)((vt[i][0] + bxf) * 16.0f); // Line 4826
        vtiw[i][1] = (int)((vt[i][1] + byf) * 16.0f); // Line 4827
        vtiw[i][2] = (int)((vt[i][2] * 16.0f)) + 0xfff5fff; // Line 4828
        vtiw[i][3] = 1; // Line 4829
    }
    
    for (i = 0; i < vnumw*vnumh; i++)
    {
        clip[i] = 1; // Line 4832
        
        if ((vtiw[i][0] >= 0 && vtiw[i][0] < 0x300) || vtiw[i][0] > clpx2) // Line 4833
        {
            clip[i] = 0;
        }
        
        if ((vtiw[i][1] >= 0 && vtiw[i][1] < 0x300) || vtiw[i][1] > clpy2) // Line 4834
        {
            clip[i] = 0;
        }
        
        if (vtiw[i][2] == 0 || vtiw[i][2] > clpz2) // Line 4835
        {
            clip[i] = 0;
        }
    }
    
    Reserve2DPacket(0x1000); // Line 4839
    
    bak = ndpkt;
    
    pbuf[ndpkt++].ul128 = (u_long128)0; // Line 4843
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(7, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1); // Line 4845
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD; // Line 4846
    
    pbuf[ndpkt].ul64[0] = 0; // Line 4848
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH; // Line 4849
    
    pbuf[ndpkt].ul64[0] = tex0; // Line 4851
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1; // Line 4852
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0); // Line 4854
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1; // Line 4855
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1); // Line 4857
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1; // Line 4858
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0); // Line 4860
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1; // Line 4861
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS); // Line 4863
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1; // Line 4864
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_CLAMP_1(SCE_GS_CLAMP, SCE_GS_CLAMP, 0, 0, 0, 0); // Line 4866
    pbuf[ndpkt++].ul64[1] = SCE_GS_CLAMP_1; // Line 4867

    m = (pnumw + 1) * pnumh;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(m, SCE_GS_TRUE, SCE_GS_TRUE, 348, SCE_GIF_PACKED, 6); // Line 4873
    pbuf[ndpkt++].ul64[1] = 0 // Line 4874
        | SCE_GS_RGBAQ << (4 * 0) 
        | SCE_GS_UV    << (4 * 1) 
        | SCE_GS_XYZF2 << (4 * 2)
        | SCE_GS_RGBAQ << (4 * 3) 
        | SCE_GS_UV    << (4 * 4) 
        | SCE_GS_XYZF2 << (4 * 5);

    if (0/m) {}
    
    for (i = 0; i < m; i++)
    {
        int l = i + pnumh; // HACK: not in STAB :(
        j = i + vnumw;
        k = i % (pnumw + 1) ? clip[i - 1] + clip[l] + clip[i] : 0;
        
        pbuf[ndpkt].ui32[0] = 0x80; // Line 4881
        pbuf[ndpkt].ui32[1] = 0x80; // Line 4882
        pbuf[ndpkt].ui32[2] = 0x80; // Line 4883
        pbuf[ndpkt++].ui32[3] = (u_char)(alpha1[i] * c); // Line 4888
        
        pbuf[ndpkt].ui32[0] = tx[i]; // Line 4896
        pbuf[ndpkt].ui32[1] = ty[i]; // Line 4897
        pbuf[ndpkt].ui32[2] = 0; // Line 4898
        pbuf[ndpkt++].ui32[3] = 0; // Line 4899
        
        pbuf[ndpkt].ui32[0] = vtiw[i][0]; // Line 4901
        pbuf[ndpkt].ui32[1] = vtiw[i][1]; // Line 4902
        pbuf[ndpkt].ui32[2] = vtiw[i][2]; // Line 4903
        pbuf[ndpkt++].ui32[3] = k <= 2 ? 0x8000 : 0; // Line 4904

        k = i % (pnumw + 1) ? clip[j - 1] + clip[i] + clip[j] : 0;
        
        pbuf[ndpkt].ui32[0] = 0x80; // Line 4908
        pbuf[ndpkt].ui32[1] = 0x80; // Line 4909
        pbuf[ndpkt].ui32[2] = 0x80; // Line 4910
        pbuf[ndpkt++].ui32[3] = (u_char)(alpha1[j] * c); // Line 4915
        
        pbuf[ndpkt].ui32[0] = tx[j]; // Line 4923
        pbuf[ndpkt].ui32[1] = ty[j]; // Line 4924
        pbuf[ndpkt].ui32[2] = 0; // Line 4925
        pbuf[ndpkt++].ui32[3] = 0; // Line 4926
        
        pbuf[ndpkt].ui32[0] = vtiw[j][0]; // Line 4928
        pbuf[ndpkt].ui32[1] = vtiw[j][1]; // Line 4929
        pbuf[ndpkt].ui32[2] = vtiw[j][2]; // Line 4930
        pbuf[ndpkt++].ui32[3] = k <= 2 ? 0x8000 : 0; // Line 4931
    }
    
    pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1; // Line 4933

    // dont't return anything otherwise the function doesn't match
    // return pbuf[bak].ui32[0];
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
	/* 0x0(sp) */ SPRT_DAT test[2] = {
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
	/* 0x40(sp) */ DISP_SPRT ds;
	/* f20 58 */ float out;
	/* f27 65 */ float alp;
    /* data 280a50 */ static float bx[3] = { -200.0f, 200.0f, 450.0f };
    /* data 280a60 */ static float by[3] = { 0.0f, 0.0f, 300.f };
    /* sdata 3564e8 */ static int cnt = 0;
    float sclx = 10.0f;
    float scly = 10.0f;
    
    out = 15.0f;
    alp = 0.0f;

    switch (now_loading_flow)
    {
        case 0:
            SubNowLoading(1,0, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
            
            now_loading_flow++;
            cnt = 0;
        case 1:
            alp = ((out - cnt) * 128.0f) / out;
            
            if (++cnt > out)
            {
                now_loading_flow++;
                cnt = 0;
            }
        break;
        case 2:
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
    SetSprFile3(0x1f1c000, 0);
    CopySprDToSpr(&ds, test);
    
    ds.tex1 = SCE_GS_SET_TEX1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    ds.z = 0xfffffff - test[0].pri;
    ds.x = test[0].x;
    ds.y = test[0].y;
    ds.pri = test[0].pri;
    
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

void SubGameOver(/* 0x11b60(sp) */ u_char alp, /* f20 58 */ float rate)
{
	/* bss 36a500 */ static float rrr[1813];
	/* bss 36c158 */ static float lll[1813];
	/* bss 36ddb0 */ static float mm1[1813];
	/* bss 36fa08 */ static float mm2[1813];
	/* bss 371660 */ static float sss[1813];
	/* bss 3732b8 */ static float ccc[1813];
	/* sdata 3564ec */ static int fl = 1;
	/* sdata 3564f0 */ static float r = 0.0f;
	/* s6 22 */ int i;
	/* s2 18 */ int k;
	/* 0x11b64(sp) */ int m;
	/* 0x11b68(sp) */ int bak;
	/* s2 18 */ int pnumw;
	/* s3 19 */ int vnumw;
	/* s0 16 */ int wix;
	/* v0 2 */ int pnumh;
	/* s7 23 */ int vnumh;
	/* s2 18 */ int wiy;
	/* s1 17 */ int sx2;
    int x8;
	/* s0 16 */ int sy2;
    int y8;
	/* f24 62 */ float sclx;
	/* f23 61 */ float scly;
	/* f1 39 */ float add;
	/* f1 39 */ float f1;
	/* f0 38 */ float f2;
	/* f14 52 */ float f3;
	/* f13 51 */ float f4;
	/* f20 58 */ float wfw;
	/* f21 59 */ float wfh;
	/* f25 63 */ float fw;
	/* f22 60 */ float lm;
	/* f27 65 */ float cntw;
	/* f26 64 */ float cnth;
	/* f20 58 */ float fx;
	/* f0 38 */ float fy;
	/* 0x0(sp) */ float tx[1813];
	/* 0x1c60(sp) */ float ty[1813];
	/* v0 2 */ u_long tex0;
	/* 0x38c0(sp) */ sceVu0FVECTOR vt[1813];
	/* 0xaa10(sp) */ sceVu0IVECTOR vtiw[1813];

    vnumw = 49;
    pnumw = 48;
    vnumh = 37;
    pnumh = 36;
    
    i = 0;
    
	sx2 = 16 * (640 - 1) - 8;
	sy2 = 16 * (224 - 1) - 8;

    if (lll[0]) {}

    sclx = 17.0f;
    scly = 13.0f;

    cntw = (float)((pnumw/2) % vnumw) * sclx; // 408.0f
    cnth = (float)((pnumh/2) % vnumw) * scly; // 234.0f;

    tex0 = SCE_GS_SET_TEX0_1(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
    
    LocalCopyLtoL((sys_wrk.count & 1) * 0x8c0, 0x1a40); // Line 5113

    /*
    0x4100 =   8.0f;
    0x43cc = 408.0f;
    0x436a = 234.0f;
    0x3f80 =   1.0f;
    0x43a0 = 320.0f;
    0x4180 =  16.0f;
    0x3f00 =   0.5f;
    0x42e0 = 112.0f;
    */

    x8 = 8;
    y8 = 8;
    f3 = 408.0f;
    f4 = 234.0f;
    
    for (i = 0; i < vnumw*vnumh; i++)
    {
        wiy = i / vnumw;
        wix = i % vnumw;

        f1 = wix * sclx;
        f2 = wiy * scly;
    
        vt[i][0] = f1 - f3; // Line 5128
        vt[i][1] = f2 - f4; // Line 5129
        vt[i][2] = 0.0f; // Line 5130
        vt[i][3] = 1.0f; // Line 5131
        
        if (wfh) {} // HACK: fixes a stack disalignment
        
        tx[i] = (int)(((f1 - f3)        + 320.0f) * 16.0f); // Line 5133
        ty[i] = (int)(((f2 - f4) * 0.5f + 112.0f) * 16.0f); // Line 5134
        
        tx[i] = (tx[i] < x8) ? x8 : ((sx2 < tx[i]) ? sx2 : tx[i]); // Line 5135
        ty[i] = (ty[i] < y8) ? y8 : ((sy2 < ty[i]) ? sy2 : ty[i]); // Line 5136
    }
    
    fw = rate / 25.0f; // Line 5140
    
    if (fl != 0) // Line 5142
    {
        for (i = 0; i < vnumw*vnumh; i++)
        {
            wix = i % vnumw;
            wiy = i / vnumw;
            
            wfw = wix * sclx - cntw; // Line 5145
            wfh = wiy * scly - cnth; // Line 5146
            
            lll[i] = SgSqrtf(wfw * wfw + wfh * wfh); // Line 5148
            
            lm = lll[0];
            
            if ((wix != ((pnumw / 2) % vnumw)) || ((i / vnumw) != ((pnumh / 2) % vnumw)))
            {
                rrr[i] = SgAtan2f(wfw, wfh);
            }
            else
            {
                rrr[i] = 0.0f;
            }
            
            mm1[i] = (lll[i] * PI * 12.0f) / lm; // Line 5157
            mm2[i] = (lm - lll[i]) / lm; // Line 5158
            sss[i] = SgSinf(rrr[i]); // Line 5159
            ccc[i] = SgCosf(rrr[i]); // Line 5142
        }
    }
    
    for (i = 0; i < vnumw*vnumh; i++)
    {
        if (((i % vnumw) != (pnumw/2) % i)) {} // debug code ??

        fx = lll[i]; // Line 5171
        fy = SgSinf(mm1[i] - r) * fw * mm2[i]; // Line 5172
        
        vt[i][0] = sss[i] * fx - ccc[i] * fy; // Line 5175
        vt[i][1] = ccc[i] * fx + sss[i] * fy; // Line 5167
    }

    add = 0.08f;
    r = r + add <= PI2 ? r + add : r + add - PI2;
    
    for (i = 0; i < vnumw*vnumh; i++)
    {
        vtiw[i][0] = (vt[i][0]        + 2048.0f) * 16.0f; // Line 5188
        vtiw[i][1] = (vt[i][1] * 0.5f + 2048.0f) * 16.0f; // Line 5189
        vtiw[i][2] = 0xfff5fff; // Line 5190
        vtiw[i][3] = 1; // Line 5191
    }
    
    Reserve2DPacket(0x1000); // Line 5195
    
    bak = ndpkt;
    
    pbuf[ndpkt++].ul128 = (u_long128)0; // Line 5199
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1); // Line 5201
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD; // Line 5202
    
    pbuf[ndpkt].ul64[0] = 0; // Line 5204
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH; // Line 5205
    
    pbuf[ndpkt].ul64[0] = tex0; // Line 5207
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1; // Line 5208
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0); // Line 5210
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1; // Line 5211
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1); // Line 5213
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1; // Line 5214
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0); // Line 5216
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1; // Line 5217
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL); // Line 5219
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1; // Line 5220

    m = (pnumw + 1) * pnumh;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(m, SCE_GS_TRUE, SCE_GS_TRUE, 348, SCE_GIF_PACKED, 6); // Line 5226
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_UV    << (4 * 0) \
        | SCE_GS_RGBAQ << (4 * 1) \
        | SCE_GS_XYZF2 << (4 * 2) \
        | SCE_GS_UV    << (4 * 3) \
        | SCE_GS_RGBAQ << (4 * 4) \
        | SCE_GS_XYZF2 << (4 * 5); // Line 5228
    
    for (i = 0; i < m; i++)
    {
        wiy = i + vnumw; // ??
        k = (i % (pnumw+1)) ? 0 : 0x8000;
        
        pbuf[ndpkt].ui32[0] = (u_int)tx[i];
        pbuf[ndpkt].ui32[1] = (u_int)ty[i]; // Line 5235
        pbuf[ndpkt].ui32[2] = 0; // Line 5236
        pbuf[ndpkt++].ui32[3] = 0; // Line 5237
        
        pbuf[ndpkt].ui32[0] = 0x80;
        pbuf[ndpkt].ui32[1] = 0x80; // Line 5243
        pbuf[ndpkt].ui32[2] = 0x80; // Line 5244
        pbuf[ndpkt++].ui32[3] = (u_int)alp; // Line 5245
        
        pbuf[ndpkt].ui32[0] = vtiw[i][0]; // Line 5247
        pbuf[ndpkt].ui32[1] = vtiw[i][1]; // Line 5248
        pbuf[ndpkt].ui32[2] = vtiw[i][2]; // Line 5249
        pbuf[ndpkt++].ui32[3] = k; // Line 5250
        
        pbuf[ndpkt].ui32[0] = (u_int)tx[wiy]; // Line 5252
        pbuf[ndpkt].ui32[1] = (u_int)ty[wiy]; // Line 5253
        pbuf[ndpkt].ui32[2] = 0; // Line 5254
        pbuf[ndpkt++].ui32[3] = 0; // Line 5255
        
        pbuf[ndpkt].ui32[0] = 0x80;
        pbuf[ndpkt].ui32[1] = 0x80; // Line 5261
        pbuf[ndpkt].ui32[2] = 0x80; // Line 5262
        pbuf[ndpkt++].ui32[3] = (u_int)alp; // Line 5263
        
        pbuf[ndpkt].ui32[0] = vtiw[wiy][0]; // Line 5265
        pbuf[ndpkt].ui32[1] = vtiw[wiy][1]; // Line 5266
        pbuf[ndpkt].ui32[2] = vtiw[wiy][2]; // Line 5267
        pbuf[ndpkt++].ui32[3] = k; // Line 5268
    }
    
    pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;
}

/* sdata 3564f4 */ int gameover_flow = 9;

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
	/* 0x0(sp) */ SPRT_DAT gameover_tbl[2] = {
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
	/* 0x40(sp) */ DISP_SPRT ds;
	/* s0 16 */ int i;
	/* sdata 3564f8 */ static int cnt = 0;
	/* s0 16 */ int sec5;
	/* t7 15 */ int sec0;
	/* v0 2 */ int sec1;
	/* t6 14 */ int sec2;
	/* t4 12 */ int sec3;
	/* t5 13 */ int sec4;
	/* 0xd0(sp) */ int blr;
	/* s7 23 */ int alp0;
	/* fp 30 */ int alp1;
	/* s6 22 */ int alp2;
	/* s1 17 */ int alp3;
	/* s5 21 */ int num;
	/* f21 59 */ float scl;
    int type;
    // int ret;

    sec5 = 30;
    sec0 = 30;
    sec1 = 240;
    sec2 = 90;
    sec3 = 30;
    sec4 = 30; // 0x1e;
    
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
            cnt = LoadReq(G_OVER_PK2, 0x1e90000);
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
        
        if ((sec1/2) > cnt)
        {
            alp1 = ((cnt * 128) / (sec1/2));
            blr = 300;
        }
        else
        {
            alp1 = 0x80;
            blr = ((sec1 - cnt) * 300) / (sec1/2);
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
        SetSprFile3(0x1e90000, 0);
        
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
    
    // ret = gameover_flow < 7;
    
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
    
    // return ret;
}

void ChangeMonochrome(/* a0 4 */ int sw)
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

void InitExFade1(/* a0 4 */ int fl, /* a1 5 */ int time1)
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
	/* sbss 3578e8 */ static int cnt1;
	/* sbss 3578ec */ static int cnt2;
	/* t1 9 */ int sec2_1;
	/* 0x0(sp) */ u_char dsp[2] = { 8, 16 };
	/* 0x10(sp) */ u_char dal[2] = { 0, 127 };
	/* 0x20(sp) */ u_char nal[2] = { 45, 102 };
	/* a3 7 */ u_char dalp;
	/* s0 16 */ u_char dspd;
	/* sbss 3578f0 */ static u_char nalp;
	/* f3 41 */ float per2;

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
    
    SetEffects(2, 1, 3, (float)dalp, (float)dspd, 0x80, 0x40);
    SetEffects(0xc, 1, 0x21, 0xff, &nalp);
    SetEffects(9, 1, msbtset.ff.alp, 0x80000);

#ifdef BUILD_EU_VERSION
    SetEffects(0xb, 1, 0x24);
#else
#endif
    
    return ef_exfade1_flow2 == 11;
}

void InitExFade2(/* a0 4 */ int fl, /* a1 5 */ int time1, /* a2 6 */ int time2, /* a3 7 */ int time3)
{
    ef_exfade2_flow1 = fl == 0 ? 0 : 3;
    ef_exfade2_time1 = time1;
    ef_exfade2_time2 = time2;
    ef_exfade2_time3 = time3;
}

int SetExFade2()
{
	/* sbss 3578f4 */ static int cnt1;
	/* t0 8 */ int sec1_1;
	/* a3 7 */ int sec1_2;
	/* a2 6 */ int sec1_3;
	/* 0x0(sp) */ u_char bal[3] = { 57, 67, 255 };
	/* 0x10(sp) */ u_char dfr[2] = { 0, 11 };
	/* 0x20(sp) */ u_char dal[2] = { 0, 13 };
	/* 0x30(sp) */ u_char ccl[2] = { 70, 100 };
	/* 0x40(sp) */ u_char cal[2] = { 70, 99 };
	/* sbss 3578f8 */ static u_char balp;
	/* s2 18 */ u_char dfrt;
	/* s0 16 */ u_char dalp;
	/* s3 19 */ u_char ccol;
	/* s1 17 */ u_char calp;
	/* f5 43 */ float per1;

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
    break;
    }
    
    if (ef_exfade2_flow1 < 6)
    {
        SetEffects(9, 1, 0x1e, 0x80000);
        SetEffects(3, 1, &balp, 1000, 1800, 320.0f, 112.0f);
        SetEffects(6, 1, 2, dfrt);
        SetEffects(2, 1, 2, (float)dalp, 8.0f, 0x40, 0x80);
        
        SetEffects(0xd, 1, ccol, calp);
    }
    
    return ef_exfade2_flow1 == 6;
}

/* sdata 35653c */ int ef_exfade1_flow1 = 0;
/* sdata 356540 */ int ef_exfade1_flow2 = 0;
/* sdata 356544 */ int ef_exfade1_time1 = 0;
/* sdata 356548 */ int ef_exfade2_flow1 = 0;
/* sdata 35654c */ int ef_exfade2_time1 = 0;
/* sdata 356550 */ int ef_exfade2_time2 = 0;
/* sdata 356554 */ int ef_exfade2_time3 = 0;

void InitScreenSaver()
{
    memset(&hand_wrk, 0, sizeof(HAND_WRK));
}

void SetScreenSaver()
{
	/* s1 17 */ int i;
	/* t3 11 */ int r3; // *key_now[15] ?? // l3 and r3 do not stop
	/* a0 4 */ int l3;  // *key_now[14] ?? // the screensaver in the game ...
	/* v0 2 */ u_int pad_all;
	/* s0 16 */ ONE_HAND *ohp;
	/* 0x0(sp) */ DISP_SPRT ds;
	/* 0x90(sp) */ SPRT_DAT hand_dat = {
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
	// /* f0 38 */ float r;
	// /* f0 38 */ float r;
	// /* f1 39 */ float r;
	// /* f1 39 */ float r;
	// /* f0 38 */ float r;
	// /* f0 38 */ float r;

    if (ingame_wrk.stts & 0x20)
    {
        hand_wrk.count = 0;
    }
    else
    {
        r3 = 
            (__builtin_abs(pad[0].analog[0] - 0x80) >= 32) + 
            (__builtin_abs(pad[0].analog[1] - 0x80) >= 32);
        
        l3 =
            (__builtin_abs(pad[0].analog[2] - 0x80) >= 32) + 
            (__builtin_abs(pad[0].analog[3] - 0x80) >= 32);
        
        pad_all = 
            *key_now[0] + 
            *key_now[1] + 
            *key_now[2] + 
            *key_now[3] + 
            *key_now[4] + 
            *key_now[5] + 
            *key_now[6] + 
            *key_now[7] + 
            *key_now[8] + 
            *key_now[9] + 
            *key_now[10] + 
            *key_now[11] +
            *key_now[13] + \
            *key_now[12] + \
            r3 +
            l3;

        hand_wrk.count = !pad_all ? hand_wrk.count + 1 : 0;
        
        if (hand_wrk.count >= 18000)
        {
            if (hand_wrk.wait-- < 1)
            {
                ohp = &hand_wrk.oh[hand_wrk.top];

                ohp->scl = (short)(vu0Rand() * 1.5f + 2.0); // yeah, 2.0 (double), not 2.0f (float) ... :D
                ohp->rot = (int)(vu0Rand() * 360.0f - 180.0f);
                ohp->bx = (int)(vu0Rand() * 370.0f + (hand_wrk.top & 1) * 220.0f);
                ohp->by = (int)(vu0Rand() * 274.0f + (hand_wrk.top / 2 & 1) * 124.0f);
                ohp->flip = (int)(vu0Rand() * 2);
                ohp->alp = 0xff;
                hand_wrk.wait = (int)(vu0Rand() * 70.0f) + 20;
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
                    ds.z = 0xfffffef;
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
	/* 0x0(sp) */ SBTSET screen_effect[7] = {
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
                .x = 320.0,
                .y = 112.0,
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
                .spd = 8.0,
                .alp = 21.0,
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
	/* v1 3 */ int n;

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
