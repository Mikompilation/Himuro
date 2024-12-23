#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "effect_sub.h"

#include "ee/kernel.h"
#include "ee/eestruct.h"
#include "sce/libgraph.h"
#include "sce/libvu0.h"

// #include "os/pad.h"
#include "ingame/ig_init.h"
#include "common/ul_math.h"
#include "graphics/graph2d/sprt.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph2d/effect_oth.h"
#include "graphics/graph2d/tim2_new.h"
#include "graphics/graph3d/sgdma.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/libsg.h"

typedef struct {
	int screen_flag;
	int time;
	int cnt;
	u_char col_r;
	u_char col_g;
	u_char col_b;
	u_char now_alpha;
} SCRCTRL;

static SCRCTRL sc_col = {0};
static u_char old_cam_no;
static u_long128 *buf;
static u_long128 *buf2;
static u_long128 *bufz;

#define PI_HALF 1.5707964f
#define PI 3.1415927f
#define TWO_PI 6.2831855f

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void InitEffectSub()
{
    buf = (u_long128 *)0x01E90000;
    buf2 = (u_long128 *)0x01F1C000;
    bufz = (u_long128 *)0x05000000;
    vib1_time = 0;
    vib2_time = 0;
    vib2_pow = 0;
}

void SetParam(int alp, int time, u_char r, u_char g, u_char b, int flag)
{
    sc_col.screen_flag = flag;
    sc_col.now_alpha = alp;
    sc_col.time = time;
    sc_col.col_r = r;
    sc_col.col_g = g;
    sc_col.col_b = b;
    sc_col.cnt = 0;
}

int ScreenCtrl(void)
{

    if (sc_col.now_alpha !=  0)
    {
        SetPanel2(0x10, 0.0, 0.0, 640.0, 448.0, 0, sc_col.col_r, sc_col.col_g, sc_col.col_g, sc_col.now_alpha);
    }
    
    
    switch (sc_col.screen_flag)
    {
        case 0:
        break;
        case 1:
            sc_col.now_alpha = ((sc_col.time - sc_col.cnt) * 0x80) / sc_col.time;
            sc_col.screen_flag = (sc_col.cnt++ >= sc_col.time) ? 0 : sc_col.screen_flag;
     
    
            if (sc_col.now_alpha == 0) 
            {
                InitLoadStartLock();
            }
        break;
        case 2:
            sc_col.now_alpha = sc_col.cnt * 0x80 / sc_col.time;

            sc_col.screen_flag = (sc_col.cnt++ >= sc_col.time) ? 0 : 2;

        break;
    }
    
    
    return sc_col.screen_flag;
}

void SetSquareS(int pri, float x1, float y1, float x4, float y4, u_char r, u_char g, u_char b, u_char a)
{
    SetSquare(pri, x1, y1, x1, y4, x4, y1, x4, y4, r, g, b, a);
}

void SetSquare(int pri, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, u_char r, u_char g, u_char b, u_char a)
{
    int x[4];
    int y[4];
    int z;
    int mpri;
    float div;

    div = g_bInterlace ? 2.0f : 1.0f;
    mpri = pri > 0 ? pri : 4096;

    Reserve2DPacket(mpri);

    x[0] = (x1 + 2048.0f) * 16.0f;
    x[1] = (x2 + 2048.0f) * 16.0f;
    x[2] = (x3 + 2048.0f) * 16.0f;
    x[3] = (x4 + 2048.0f) * 16.0f;

    y[0] = (y1 / div + 2048.0f) * 16.0f;
    y[1] = (y2 / div + 2048.0f) * 16.0f;
    y[2] = (y3 / div + 2048.0f) * 16.0f;
    y[3] = (y4 / div + 2048.0f) * 16.0f;

    z = 0x0fffffff - mpri;

    pbuf[ndpkt].ul128 = (u_long128)0; // clear tag
    // Is there nothing in the SDK to make dma tags???
    // this is DMA_ID_END + 10 quads worth of transfer data
    pbuf[ndpkt].ui32[0] = DMAend | 10;
    ndpkt++;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(3, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt].ul64[1] = SCE_GIF_PACKED_AD;
    ndpkt++;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, a);
    pbuf[ndpkt].ul64[1] = SCE_GS_ALPHA_1;
    ndpkt++;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GREATER, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt].ul64[1] = SCE_GS_TEST_1;
    ndpkt++;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
    pbuf[ndpkt].ul64[1] = SCE_GS_ZBUF_1;
    ndpkt++;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 68, SCE_GIF_PACKED, 5);
    // Apparently there's no macro for this in the SDK?
    pbuf[ndpkt].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) 
        | SCE_GS_XYZF2 << (4 * 1) 
        | SCE_GS_XYZF2 << (4 * 2)
        | SCE_GS_XYZF2 << (4 * 3) 
        | SCE_GS_XYZF2 << (4 * 4);
    ndpkt++;

    pbuf[ndpkt].ui32[0] = r;
    pbuf[ndpkt].ui32[1] = g;
    pbuf[ndpkt].ui32[2] = b;
    pbuf[ndpkt].ui32[3] = 128;
    ndpkt++;

    pbuf[ndpkt].ui32[0] = x[0];
    pbuf[ndpkt].ui32[1] = y[0];
    pbuf[ndpkt].ui32[2] = z;
    pbuf[ndpkt].ui32[3] = 0;
    ndpkt++;

    pbuf[ndpkt].ui32[0] = x[1];
    pbuf[ndpkt].ui32[1] = y[1];
    pbuf[ndpkt].ui32[2] = z;
    pbuf[ndpkt].ui32[3] = 0;
    ndpkt++;

    pbuf[ndpkt].ui32[0] = x[2];
    pbuf[ndpkt].ui32[1] = y[2];
    pbuf[ndpkt].ui32[2] = z;
    pbuf[ndpkt].ui32[3] = 0;
    ndpkt++;

    pbuf[ndpkt].ui32[0] = x[3];
    pbuf[ndpkt].ui32[1] = y[3];
    pbuf[ndpkt].ui32[2] = z;
    pbuf[ndpkt].ui32[3] = 0;
    ndpkt++;
}

void SetSquare2s(int pri, float x1, float y1, float x4, float y4, u_char r1, u_char g1, u_char b1, u_char r2, u_char g2, u_char b2, u_char a)
{
    int x[4];
    int y[4];
    int z;
    int mpri;
    float div;
    
    div = g_bInterlace ? 2.0f : 1.0f;
    mpri = pri > 0 ? pri : 4096;
    
    Reserve2DPacket(mpri);

    x[0] = (x1 + 2048.0f) * 16.0f;
    x[1] = (x4 + 2048.0f) * 16.0f;
    x[2] = (x1 + 2048.0f) * 16.0f;
    x[3] = (x4 + 2048.0f) * 16.0f;
    
    y[0] = (y1 / div + 2048.0f) * 16.0f;
    y[1] = (y1 / div + 2048.0f) * 16.0f;
    y[2] = (y4 / div + 2048.0f) * 16.0f;
    y[3] = (y4 / div + 2048.0f) * 16.0f;
    
    z = 0x0fffffff - mpri;
    
    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt].ui32[0] = DMAend | 12;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(2, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt].ul64[1] = SCE_GIF_PACKED_AD;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, a);
    pbuf[ndpkt].ul64[1] = SCE_GS_ALPHA_1;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt].ul64[1] = SCE_GS_ZBUF_1;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 76, SCE_GIF_PACKED, 8);
    pbuf[ndpkt].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) 
        | SCE_GS_XYZF2 << (4 * 1) 
        | SCE_GS_RGBAQ << (4 * 2)
        | SCE_GS_XYZF2 << (4 * 3) 
        | SCE_GS_RGBAQ << (4 * 4)
        | SCE_GS_XYZF2 << (4 * 5)
        | SCE_GS_RGBAQ << (4 * 6)
        | SCE_GS_XYZF2 << (4 * 7);
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = r1;
    pbuf[ndpkt].ui32[1] = g1;
    pbuf[ndpkt].ui32[2] = b1;
    pbuf[ndpkt].ui32[3] = 128;    
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = x[0];
    pbuf[ndpkt].ui32[1] = y[0];
    pbuf[ndpkt].ui32[2] = z;
    pbuf[ndpkt].ui32[3] = 0;    
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = r1;
    pbuf[ndpkt].ui32[1] = g1;
    pbuf[ndpkt].ui32[2] = b1;
    pbuf[ndpkt].ui32[3] = 128;    
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = x[1];
    pbuf[ndpkt].ui32[1] = y[1];
    pbuf[ndpkt].ui32[2] = z;
    pbuf[ndpkt].ui32[3] = 0;
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = r2;
    pbuf[ndpkt].ui32[1] = g2;
    pbuf[ndpkt].ui32[2] = b2;
    pbuf[ndpkt].ui32[3] = 128;    
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = x[2];
    pbuf[ndpkt].ui32[1] = y[2];
    pbuf[ndpkt].ui32[2] = z;
    pbuf[ndpkt].ui32[3] = 0;
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = r2;
    pbuf[ndpkt].ui32[1] = g2;
    pbuf[ndpkt].ui32[2] = b2;
    pbuf[ndpkt].ui32[3] = 128;    
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = x[3];
    pbuf[ndpkt].ui32[1] = y[3];
    pbuf[ndpkt].ui32[2] = z;
    pbuf[ndpkt].ui32[3] = 0;
    ndpkt++;
}

void SetSquareZ(int pri, float x1, float y1, float x4, float y4, int z)
{
    int x[4];
    int y[4];
    float div;
    
    div = g_bInterlace ? 2.0f : 1.0f;
    
    Reserve2DPacket(0);

    x[0] = (x1 + 2048.0f) * 16.0f;
    x[3] = (x4 + 2048.0f) * 16.0f;
    
    y[0] = (y1 / div + 2048.0f) * 16.0f;
    y[3] = (y4 / div + 2048.0f) * 16.0f;
    
    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt].ui32[0] = DMAend | 8;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(3, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt].ul64[1] = SCE_GIF_PACKED_AD;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, pri);
    pbuf[ndpkt].ul64[1] = SCE_GS_ALPHA_1;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt].ul64[1] = SCE_GS_ZBUF_1;
    ndpkt++;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_ALWAYS, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt].ul64[1] = SCE_GS_TEST_1;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 70, SCE_GIF_PACKED, 3);
    // Apparently there's no macro for this in the SDK?
    pbuf[ndpkt].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) 
        | SCE_GS_XYZF2 << (4 * 1) 
        | SCE_GS_XYZF2 << (4 * 2);
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = 128;
    pbuf[ndpkt].ui32[1] = 128;
    pbuf[ndpkt].ui32[2] = 128;
    pbuf[ndpkt].ui32[3] = 128;    
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = x[0];
    pbuf[ndpkt].ui32[1] = y[0];
    pbuf[ndpkt].ui32[2] = z;
    pbuf[ndpkt].ui32[3] = 0;    
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = x[3];
    pbuf[ndpkt].ui32[1] = y[3];
    pbuf[ndpkt].ui32[2] = z;
    pbuf[ndpkt].ui32[3] = 0;
    ndpkt++;
}

void SetPanel(u_int pri, float x1, float y1, float x2, float y2, u_char r, u_char g, u_char b, u_char a)
{
    SetPanel2(pri, x1, y1, x2, y2, 1, r, g, b, a);
}

void SetPanel2(u_int pri, float x1, float y1, float x2, float y2, int z, u_char r, u_char g, u_char b, u_char a)
{
    SQAR_DAT sq = {
        .w = 640,
        .h = 448,
        .x = 0,
        .y = 0,
        .pri = 0,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 128,
    };
    DISP_SQAR dq;
    int i;
    
    CopySqrDToSqr(&dq, &sq);

    dq.pri = pri;
    dq.z = 0xfffffff - pri;
    dq.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, z);
    
    dq.x[2] = dq.x[0] = x1;
    dq.y[1] = dq.y[0] = y1;
    dq.x[3] = dq.x[1] = dq.x[0] + (int)(x2 - x1);
    dq.y[3] = dq.y[2] = dq.y[0] + (int)(y2 - y1);
    
    for (i = 0; i < 4; i++)
    {
        dq.r[i] = r;
        dq.g[i] = g;
        dq.b[i] = b; 
    }
    
    dq.alpha = a;
    
    DispSqrD(&dq);
}

void SetSquareSN(int pri, float x1, float y1, float x4, float y4, u_char r, u_char g, u_char b, u_char a)
{
    SetSquare(pri, x1, y1, x1, y4, x4, y1, x4, y4, r, g, b,a);
}

void SetSquareN(int pri, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, u_char r, u_char g, u_char b, u_char a)
{
    return;
}

void SetTriangle(int pri, float x1, float y1, float x2, float y2, float x3, float y3, u_char r, u_char g, u_char b, u_char a)
{
    int x[3];
    int y[3];
    int z;
    int mpri;
    float div;

    div = g_bInterlace ? 2.0f : 1.0f;
    mpri = pri > 0 ? pri : 4096;
    
    Reserve2DPacket(mpri);

    x[0] = (x1 + 2048.0f) * 16.0f;
    x[1] = (x2 + 2048.0f) * 16.0f;
    x[2] = (x3 + 2048.0f) * 16.0f;

    y[0] = (y1 / div + 2048.0f) * 16.0f;
    y[1] = (y2 / div + 2048.0f) * 16.0f;
    y[2] = (y3 / div + 2048.0f) * 16.0f;
    
    z = 0xfffffff - mpri;

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt].ui32[0] = DMAend | 8;
    ndpkt++;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(2, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt].ul64[1] = SCE_GIF_PACKED_AD;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, a);
    pbuf[ndpkt].ul64[1] = SCE_GS_ALPHA_1;
    ndpkt++;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt].ul64[1] = SCE_GS_ZBUF_1;
    ndpkt++;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 68, SCE_GIF_PACKED, 4);
    pbuf[ndpkt].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) 
        | SCE_GS_XYZF2 << (4 * 1) 
        | SCE_GS_XYZF2 << (4 * 2)
        | SCE_GS_XYZF2 << (4 * 3);
    ndpkt++;

    pbuf[ndpkt].ui32[0] = r;
    pbuf[ndpkt].ui32[1] = g;
    pbuf[ndpkt].ui32[2] = b;
    pbuf[ndpkt].ui32[3] = 128;
    ndpkt++;

    pbuf[ndpkt].ui32[0] = x[0];
    pbuf[ndpkt].ui32[1] = y[0];
    pbuf[ndpkt].ui32[2] = z;
    pbuf[ndpkt].ui32[3] = 0;
    ndpkt++;

    pbuf[ndpkt].ui32[0] = x[1];
    pbuf[ndpkt].ui32[1] = y[1];
    pbuf[ndpkt].ui32[2] = z;
    pbuf[ndpkt].ui32[3] = 0;
    ndpkt++;

    pbuf[ndpkt].ui32[0] = x[2];
    pbuf[ndpkt].ui32[1] = y[2];
    pbuf[ndpkt].ui32[2] = z;
    pbuf[ndpkt].ui32[3] = 0;
    ndpkt++;
}

void SetTriangleZ(int pri, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, u_char r, u_char g, u_char b, u_char a)
{
	int x[3];
	int y[3];
	float div;

    div = g_bInterlace ? 2.0f : 1.0f;
    
    Reserve2DPacket(pri > 0 ? pri : 4096);

    x[0] = (x1 + 2048.0f) * 16.0f;
    x[1] = (x2 + 2048.0f) * 16.0f;
    x[2] = (x3 + 2048.0f) * 16.0f;

    y[0] = (y1 / div + 2048.0f) * 16.0f;
    y[1] = (y2 / div + 2048.0f) * 16.0f;
    y[2] = (y3 / div + 2048.0f) * 16.0f;
    
    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt].ui32[0] = DMAend | 9;
    ndpkt++;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(3, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt].ul64[1] = SCE_GIF_PACKED_AD;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, a);
    pbuf[ndpkt].ul64[1] = SCE_GS_ALPHA_1;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_FALSE, SCE_GS_ALPHA_NEVER, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt].ul64[1] = SCE_GS_TEST_1;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
    pbuf[ndpkt].ul64[1] = SCE_GS_ZBUF_1;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 68, SCE_GIF_PACKED, 4);
    pbuf[ndpkt].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) 
        | SCE_GS_XYZF2 << (4 * 1) 
        | SCE_GS_XYZF2 << (4 * 2)
        | SCE_GS_XYZF2 << (4 * 3);
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = r;
    pbuf[ndpkt].ui32[1] = g;
    pbuf[ndpkt].ui32[2] = b;
    pbuf[ndpkt].ui32[3] = 128;
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = x[0];
    pbuf[ndpkt].ui32[1] = y[0];
    pbuf[ndpkt].ui32[2] = z1;
    pbuf[ndpkt].ui32[3] = 0;
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = x[1];
    pbuf[ndpkt].ui32[1] = y[1];
    pbuf[ndpkt].ui32[2] = z2;
    pbuf[ndpkt].ui32[3] = 0;
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = x[2];
    pbuf[ndpkt].ui32[1] = y[2];
    pbuf[ndpkt].ui32[2] = z3;
    pbuf[ndpkt].ui32[3] = 0;
    ndpkt++;
}

void SetLine(int pri, float x1, float y1, float x2, float y2, u_char r, u_char g, u_char b, u_char a)
{
	int x[2];
	int y[2];
	int z;
	int mpri;
	float div;
    
    div = g_bInterlace ? 2.0f : 1.0f;
    mpri = pri > 0 ? pri : 4096;
    
    Reserve2DPacket(mpri);
 
    x[0] = (x1 + 2048.0f) * 16.0f;
    x[1] = (x2 + 2048.0f) * 16.0f;

    y[0] = (y1 / div + 2048.0f) * 16.0f;
    y[1] = (y2 / div + 2048.0f) * 16.0f;

    z = 0xfffffff - mpri;
    
    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt].ui32[0] = DMAend | 7;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(2, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt].ul64[1] = SCE_GIF_PACKED_AD;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, a);
    pbuf[ndpkt].ul64[1] = SCE_GS_ALPHA_1;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt].ul64[1] = SCE_GS_ZBUF_1;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 65, SCE_GIF_PACKED, 3);
    pbuf[ndpkt].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) 
        | SCE_GS_XYZF2 << (4 * 1) 
        | SCE_GS_XYZF2 << (4 * 2);
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = r;
    pbuf[ndpkt].ui32[1] = g;
    pbuf[ndpkt].ui32[2] = b;
    pbuf[ndpkt].ui32[3] = 128;
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = x[0];
    pbuf[ndpkt].ui32[1] = y[0];
    pbuf[ndpkt].ui32[2] = z;
    pbuf[ndpkt].ui32[3] = 0;
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = x[1];
    pbuf[ndpkt].ui32[1] = y[1];
    pbuf[ndpkt].ui32[2] = z;
    pbuf[ndpkt].ui32[3] = 0;
    ndpkt++;
}

void SetLine2(int pri, float x1, float y1, float x2, float y2, u_char r, u_char g, u_char b, u_char a)
    {
	u_char rr;
	u_char gg;
	u_char bb;
	float d;
	float dw;

    
    dw = SgAtanf((x1 - x2) / (y1 - y2));
    d = (dw * 180.0f) / PI;
    
    if (d > 45.0f || d < -45.0f)
    {
        rr = r * 0.5f;
        gg = g * 0.5f;
        bb = b * 0.5f;
    }
    else if (d > 30.0f || d < -30.0f)
    {
        if (d < 0.0f)
        {
            d = -d;
        }
        
        dw = ((45.0f - d) * 0.5f) / 15.0f + 0.5f;
        
        rr = r * dw;
        gg = g * dw;
        bb = b * dw;
    }
    else
    {
        rr = r;
        gg = g;
        bb = b;
    }
        
    SetLine(pri, x1, y1, x2, y2, rr, gg, bb, a);
}

void DrawPoint(float *mpos, int no)
{
	int n;
	int clip;
	sceVu0FMATRIX wlm;
	sceVu0FMATRIX slm;
	sceVu0FVECTOR fzero = { 0.0f, 0.0f, 0.0f, 1.0f };
	sceVu0IVECTOR ivec;

    sceVu0UnitMatrix(wlm);
    
    wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;
    
    sceVu0TransMatrix(wlm, wlm, mpos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);
    sceVu0RotTransPers(ivec, slm, fzero, 0);

    clip = 0;

    if (ivec[0] < GS_X_COORD(0) || ivec[0] > GS_X_COORD(SCR_WIDTH))
    {
        clip++;
    }
    
    if (ivec[1] < GS_Y_COORD(0) || ivec[1] > GS_Y_COORD(SCR_HEIGHT))
    {
        clip++;
    }
    
    if (ivec[2] < 0xff || ivec[2] > 0xffffff)
    {
        clip++;
    }

    if (clip == 0)
    {
        Reserve2DPacket(0x1000);

        n = ndpkt;
        
        pbuf[ndpkt].ul128 = (u_long128)0;
        pbuf[ndpkt].ui32[0] = DMAend | 0;
        ndpkt++;
        
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(3, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
        pbuf[ndpkt].ul64[1] = SCE_GIF_PACKED_AD;
        ndpkt++;
        
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
        pbuf[ndpkt].ul64[1] = SCE_GS_ZBUF_1;
        ndpkt++;
        
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_ALWAYS, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_ALWAYS);
        pbuf[ndpkt].ul64[1] = SCE_GS_TEST_1;
        ndpkt++;
        
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
        pbuf[ndpkt].ul64[1] = SCE_GS_ALPHA_1;
        ndpkt++;
        
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_SCISSOR_2(32769, 0, 16384, 8288);
        pbuf[ndpkt].ul64[1] = SCE_GS_SCISSOR_2;
        ndpkt++;
        
        pbuf[ndpkt].ui32[0] = 0xff;
        pbuf[ndpkt].ui32[1] = no * 0xff;
        pbuf[ndpkt].ui32[2] = no * 0xff;
        pbuf[ndpkt].ui32[3] = 128;
        ndpkt++;
        
        pbuf[ndpkt].iv[0] = ivec[0];
        pbuf[ndpkt].iv[1] = ivec[1];
        pbuf[ndpkt].iv[2] = ivec[2];
        pbuf[ndpkt].iv[3] = 0;
        ndpkt++;
        
        pbuf[n].ui32[0] = ndpkt + DMAend - n - 1;
    }
}

void DrawPoint2(float *mpos, u_char r, u_char g, u_char b, u_char a) {
	int n;
	int clip;
	sceVu0FMATRIX wlm;
	sceVu0FMATRIX slm;
	sceVu0FVECTOR fzero = { 0.0f, 0.0f, 0.0f, 1.0f };
	sceVu0IVECTOR ivec;    

    sceVu0UnitMatrix(wlm);
    
    wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;
    
    sceVu0TransMatrix(wlm, wlm, mpos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);
    sceVu0RotTransPers(ivec, slm, fzero, 0);

    clip = 0;

    if (ivec[0] < GS_X_COORD(0) || ivec[0] > GS_X_COORD(SCR_WIDTH))
    {
        clip++;
    }
    
    if (ivec[1] < GS_Y_COORD(0) || ivec[1] > GS_Y_COORD(SCR_HEIGHT))
    {
        clip++;
    }
    
    if (ivec[2] < 0xff || ivec[2] > 0xffffff)
    {
        clip++;
    }
    
    if (clip == 0)
    {
        Reserve2DPacket(0x1000);

        n = ndpkt;
        
        pbuf[ndpkt].ul128 = (u_long128)0;
        pbuf[ndpkt].ui32[0] = DMAend | 0;
        ndpkt++;
        
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(3, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
        pbuf[ndpkt].ul64[1] = SCE_GIF_PACKED_AD;
        ndpkt++;
        
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
        pbuf[ndpkt].ul64[1] = SCE_GS_ZBUF_1;
        ndpkt++;
        
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GREATER, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL);
        pbuf[ndpkt].ul64[1] = SCE_GS_TEST_1;
        ndpkt++;
        
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
        pbuf[ndpkt].ul64[1] = SCE_GS_ALPHA_1;
        ndpkt++;
        
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_SCISSOR_2(32769, 0, 16384, 8224);
        pbuf[ndpkt].ul64[1] = SCE_GS_SCISSOR_2;
        ndpkt++;
        
        pbuf[ndpkt].ui32[0] = r;
        pbuf[ndpkt].ui32[1] = g;
        pbuf[ndpkt].ui32[2] = b;
        pbuf[ndpkt].ui32[3] = a;
        ndpkt++;
        
        pbuf[ndpkt].iv[0] = ivec[0];
        pbuf[ndpkt].iv[1] = ivec[1];
        pbuf[ndpkt].iv[2] = ivec[2];
        pbuf[ndpkt].iv[3] = 0;
        ndpkt++;
        
        pbuf[n].ui32[0] = ndpkt + DMAend - n - 1;
    }
}

void DrawLine(float *mpos1, u_char r1, u_char g1, u_char b1, u_char a1, float *mpos2, u_char r2, u_char g2, u_char b2, u_char a2)
{
	// u_char b2;
	// u_char a2;
	int i;
	int n;
	int clip;
	sceVu0FMATRIX wlm;
	sceVu0FMATRIX slm;
	sceVu0FVECTOR fzero = { 0.0f, 0.0f, 0.0f, 1.0f };
	sceVu0IVECTOR ivec[2];
    
    sceVu0UnitMatrix(wlm);
    
    wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;
    
    sceVu0TransMatrix(wlm, wlm, mpos1);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);
    sceVu0RotTransPers(ivec[0], slm, fzero, 0);
    
    sceVu0UnitMatrix(wlm);
    
    wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;

    sceVu0TransMatrix(wlm, wlm, mpos2);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);
    sceVu0RotTransPers(ivec[1], slm, fzero, 0);

    clip = 0;

    for (i = 0; i < 2; i++)
    {
        // if (ivec[i][0] < GS_X_COORD(-1680) || ivec[i][0] > GS_X_COORD(2320))
        if (ivec[i][0] < 0x300 || ivec[i][0] > (0x300 + 0xfa00))
        {
            clip = clip | 1 << (i * 4); // 1,16
        }

        // if (ivec[i][1] < GS_Y_COORD(-1888) || ivec[i][1] > GS_Y_COORD(2112))
        if (ivec[i][1] < 0x300 || ivec[i][1] > (0x300 + 0xfa00))
        {
            clip = clip | 2 << (i * 4); // 2,32
        }

        if (ivec[i][2] < 0xf || ivec[i][2] > 0xfffffff)
        {
            clip = clip | 4 << (i * 4); // 4,64
        }
    }
    
    if (clip == 0)
    {
        Reserve2DPacket(0x1000);

        n = ndpkt;
        
        pbuf[ndpkt].ul128 = (u_long128)0;
        pbuf[ndpkt++].ui32[0] = DMAend | 0;
        
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(3, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
        pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;
        
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
        pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;
        
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GREATER, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL);
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;
        
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
        pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;
        
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 73, SCE_GIF_PACKED, 4);
        pbuf[ndpkt++].ul64[1] = 0 \
            | SCE_GS_RGBAQ << (4 * 0) 
            | SCE_GS_XYZF2 << (4 * 1) 
            | SCE_GS_RGBAQ << (4 * 2) 
            | SCE_GS_XYZF2 << (4 * 3);
        
        pbuf[ndpkt].ui32[0] = r1;
        pbuf[ndpkt].ui32[1] = g1;
        pbuf[ndpkt].ui32[2] = b1;
        pbuf[ndpkt++].ui32[3] = a1;
        
        pbuf[ndpkt].iv[0] = ivec[0][0];
        pbuf[ndpkt].iv[1] = ivec[0][1];
        pbuf[ndpkt].iv[2] = ivec[0][2];
        pbuf[ndpkt++].iv[3] = 0;
        
        pbuf[ndpkt].ui32[0] = r2;
        pbuf[ndpkt].ui32[1] = g2;
        pbuf[ndpkt].ui32[2] = b2;
        pbuf[ndpkt++].ui32[3] = a2;
        
        pbuf[ndpkt].iv[0] = ivec[1][0];
        pbuf[ndpkt].iv[1] = ivec[1][1];
        pbuf[ndpkt].iv[2] = ivec[1][2];
        pbuf[ndpkt++].iv[3] = 0;
        
        pbuf[n].ui32[0] = ndpkt + DMAend - n - 1;
    }
}

void Set3DPosTexure(sceVu0FMATRIX wlm, DRAW_ENV *de, int texno, float w, float h, u_char r, u_char g, u_char b, u_char a)
{
	int i;
	int bak;
	float th;
	float tw;
    u_int clpx2 = 0xc000;
	u_int clpy2 = 0xc000;
	u_int clpz2 = 0xffffff;
	u_long tx0;
	sceVu0FMATRIX slm;
	sceVu0IVECTOR ivec[4];
	sceVu0FVECTOR ppos[4] = {
        { -1.0f, 1.0f, 0.0f, 1.0f },
        { 1.0f, 1.0f, 0.0f, 1.0f },
        { -1.0f, -1.0f, 0.0f, 1.0f },
        { 1.0f, -1.0f, 0.0f, 1.0f },
    };
	float twoby[11] = {
        1.0f, 2.0f, 4.0f, 8.0f, 16.0f,
        32.0f, 64.0f, 128.0f, 256.0f, 512.0f, 1024.f,
    };
	float stq[2] = { 0.01f, 0.99f };
	U32DATA ts[4];
	U32DATA tt[4];
	U32DATA tq[4];
    

    for (i = 0; i < 4; i++)
    {
        ppos[i][0] = i & 1 ? w * 0.5f : -w * 0.5f;
        ppos[i][1] = i / 2 ? -h * 0.5f : h * 0.5f;
        ppos[i][2] = 0.0f;
        ppos[i][3] = 1.0f;
    }
    
    sceVu0MulMatrix(slm, SgWSMtx, wlm);
    sceVu0RotTransPersN(ivec , slm, ppos, 4, 1);
    
    tx0 = effdat[texno + monochrome_mode].tex0;
    tw = effdat[texno + monochrome_mode].w;
    th = effdat[texno + monochrome_mode].h;
    
    w = 0.0f;
    
    for (i = 0; i < 4; i++)
    {
        if (ivec[i][0] >= 0 && ivec[i][0] < 0x4000)
        {
            w = 1.0f;
        }
        else if (ivec[i][0] > clpx2)
        {
            w = 1.0f;
        }

        if (ivec[i][1] >= 0 && ivec[i][1] < 0x4000)
        {
            w = 1.0f;
        }
        else if (ivec[i][1] > clpy2)
        {
            w = 1.0f;
        }
        
        if (ivec[i][2] == 0)
        {
            w = 1.0f;
        }
        else if (ivec[i][2] > clpz2)
        {
            w = 1.0f;
        }
        
        tq[i].fl32 = 1.0f / ivec[i][3];
        ts[i].fl32 = (tw * stq[i % 2] * tq[i].fl32) / twoby[log_2(tw)];
        tt[i].fl32 = (th * stq[i / 2] * tq[i].fl32) / twoby[log_2(th)];
    }
    
    if (w == 0.0f)
    {
        Reserve2DPacket(0x1000);

        bak = ndpkt;
        
        pbuf[ndpkt++].ul128 = (u_long128)0;
        
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
        pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;
        
        pbuf[ndpkt].ul64[0] = 0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;
        
        pbuf[ndpkt].ul64[0] = tx0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;
        
        pbuf[ndpkt].ul64[0] = de->tex1;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;
        
        pbuf[ndpkt].ul64[0] = de->zbuf;
        pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;
        
        pbuf[ndpkt].ul64[0] = de->alpha;
        pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;
        
        pbuf[ndpkt].ul64[0] = de->test;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;
        
        pbuf[ndpkt].ul64[0] = de->prim;
        // pbuf[ndpkt++].ul64[1] = 0x412;
        pbuf[ndpkt++].ul64[1] = 0 \
            | SCE_GS_ST    << (4 * 0) 
            | SCE_GS_RGBAQ << (4 * 1) 
            | SCE_GS_XYZF2 << (4 * 2);
        
        for (i = 0; i < 4; i++)
        {            
            pbuf[ndpkt].ui32[0] = ts[i].ui32;
            pbuf[ndpkt].ui32[1] = tt[i].ui32;
            pbuf[ndpkt].ui32[2] = tq[i].ui32;
            pbuf[ndpkt++].ui32[3] = 0.0f;
            
            pbuf[ndpkt].ui32[0] = r;
            pbuf[ndpkt].ui32[1] = g;
            pbuf[ndpkt].ui32[2] = b;
            pbuf[ndpkt++].ui32[3] = a;
            
            pbuf[ndpkt].ui32[0] = ivec[i][0];
            pbuf[ndpkt].ui32[1] = ivec[i][1];
            pbuf[ndpkt].ui32[2] = ivec[i][2] * 16;
            pbuf[ndpkt++].ui32[3] = (i <= 1) ? 0x8000 : 0;
        }
        
        pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;
    }
}

static int old_ltol_addr1 = -1;
static int old_ltol_addr2 = -1;
int vib1_time = 0;
int vib2_time = 0;
int vib2_pow = 0;

void _SetTexDirectS(int pri, SPRITE_DATA *sd, int atype) {
	int xx[4];
	int yy[4];
	int mz;
	u_int tw;
	u_int th;
	float div;
	float mx;
	float my;
	float mscw;
	float msch;
	float mszw;
	float mszh;
	float px;
	float py;
	float pw;
	float ph;
	u_char malp;
	sceGsTex0 Load;
	sceGsTex0 Change;

    malp = sd->alpha;
    mz = sd->pos_z;

    mscw = sd->scale_w;
    msch = sd->scale_h;

    mszw = sd->size_w * mscw;
    mszh = sd->size_h * msch;
    
    mx = sd->pos_x + (sd->size_w - mszw) * 0.5f;
    my = (sd->pos_y + (sd->size_h - mszh) * 0.5f);
    
    div = g_bInterlace ? 2.0f : 1.0f;
    
    tw = MIN(sd->g_nTexSizeW, 1023);
    th = MIN(sd->g_nTexSizeH, 1023);

    tw *= 16;
    th *= 16;

    pw = mszw * 16.0f;
    ph = (mszh * 16.0f) / div;

    px = (mx + 2048.0f) * 16.0f;
    py = (my / div + 2048.0f) * 16.0f;
    
    xx[0] = px;
    xx[3] = pw + px;
    
    yy[0] = py;
    yy[3] = ph + py;

    if (sd->g_GsTex0.PSM == SCE_GS_PSMT4)
    {
        Load = sd->g_GsTex0;
        Load.PSM = SCE_GS_PSMT8;
        Load.CSA = 0;
        Load.CLD = 1;
        
        Change = sd->g_GsTex0;
        Change.CSA = 0;
        Change.CLD = 0;
    }
    
    Reserve2DPacket(pri);

    
    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = DMAend | 15;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(8, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;
    
    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;
    
    if (sd->g_GsTex0.PSM == SCE_GS_PSMT4)
    {
        pbuf[ndpkt].ul64[0] = *(u_long*)&Load;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;
        
        pbuf[ndpkt].ul64[0] = *(u_long*)&Change;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;
    }
    else
    {
        pbuf[ndpkt].ul64[0] = *(u_long*)&sd->g_GsTex0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;
        
        pbuf[ndpkt].ul64[0] = 0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_NOP;
    }
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    if (atype != 0)
    {
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, malp);
    }
    else
    {
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 128);
    }
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_CLAMP(0, 0, 0, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_CLAMP_1;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GREATER, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_ALWAYS);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 342, SCE_GIF_PACKED, 5);
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) 
        | SCE_GS_UV    << (4 * 1) 
        | SCE_GS_XYZF2 << (4 * 2) 
        | SCE_GS_UV    << (4 * 3) 
        | SCE_GS_XYZF2 << (4 * 4);
    
    pbuf[ndpkt].ui32[0] = 128;
    pbuf[ndpkt].ui32[1] = 128;
    pbuf[ndpkt].ui32[2] = 128;
    if (atype != 0)
    {
        pbuf[ndpkt++].ui32[3] = 128;
    }
    else
    {
        pbuf[ndpkt++].ui32[3] = malp;
    }

    pbuf[ndpkt].ui32[0] = 0;
    pbuf[ndpkt].ui32[1] = 0;
    pbuf[ndpkt].ui32[2] = 0;
    pbuf[ndpkt++].ui32[3] = 0;
    
    pbuf[ndpkt].ui32[0] = xx[0];
    pbuf[ndpkt].ui32[1] = yy[0];
    pbuf[ndpkt].ui32[2] = mz;
    pbuf[ndpkt++].ui32[3] = 0;
    
    pbuf[ndpkt].ui32[0] = tw;
    pbuf[ndpkt].ui32[1] = th;
    pbuf[ndpkt].ui32[2] = 0;
    pbuf[ndpkt++].ui32[3] = 0;
    
    pbuf[ndpkt].ui32[0] = xx[3];
    pbuf[ndpkt].ui32[1] = yy[3];
    pbuf[ndpkt].ui32[2] = mz;
    pbuf[ndpkt++].ui32[3] = 0;
}

void SetTexDirectS(int pri, SPRITE_DATA *sd, int atype) {
	int xx[4];
	int yy[4];
	int mz;
	u_int th;
	float div;
	int u[11];
	int v;
	int x[11];
	int i;
	int n;
	int mtw;
	float wx;
	float mx;
	float my;
	float mscw;
	float msch;
	float mszw;
	float mszh;
	float px;
	float py;
    // float pw;
	float ph;
	u_char malp;
	sceGsTex0 Load;
	sceGsTex0 Change;
    
    u_long uVar6;
    sceGsTex0 sVar9;
    u_int uVar10;
    u_int uVar11;
    int iVar12;
    float fVar15;
    
    malp = sd->alpha;
    mz = sd->pos_z;

    mscw = sd->scale_w;
    msch = sd->scale_h;
    
    mszw = sd->size_w * mscw;
    mszh = sd->size_h * msch;

    mx = sd->pos_x + (sd->size_w - mszw) * 0.5f;
    my = (sd->pos_y + (sd->size_h - mszh) * 0.5f);
    
    div = g_bInterlace ? 2.0f : 1.0f;

    mtw = MIN(sd->g_nTexSizeW, 1023);
    th = MIN(sd->g_nTexSizeH, 1023);

    v = th * 16;

    ph = (mszh * 16.0f) / div;

    px = mx + 2048.0f;
    py = (my / div + 2048.0f) * 16.0f;
    
    yy[0] = py;
    yy[3] = py + ph;

    u[0] = 0;
    x[0] = px * 16.0f;
    
    wx = 0.0f;
    n = 1;
    while (wx < mszw)
    {
        if (wx + 64.0f <= mszw)
        {
            wx = wx + 64.0f;
        }
        else
        {
            wx = mszw;
        }
        
        x[n] = (px + wx) * 16.0f;
        u[n] = ((wx * mtw) / mszw) * 16.0f;

        n++;
    }
    
    if (sd->g_GsTex0.PSM == SCE_GS_PSMT4)
    {
        Load = sd->g_GsTex0;
        Load.PSM = SCE_GS_PSMT8;
        Load.CSA = 0;
        Load.CLD = 1;
        
        Change = sd->g_GsTex0;
        Change.CSA = 0;
        Change.CLD = 0;
    }
    
    Reserve2DPacket(pri);
    
    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = DMAend + 10 + n * 5;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(8, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;
    
    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;
    
    if (sd->g_GsTex0.PSM == SCE_GS_PSMT4)
    {
        pbuf[ndpkt].ul64[0] = *(u_long*)&Load;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;
        
        pbuf[ndpkt].ul64[0] = *(u_long*)&Change;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;
    }
    else
    {
        pbuf[ndpkt].ul64[0] = *(u_long*)&sd->g_GsTex0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;
        
        pbuf[ndpkt].ul64[0] = 0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_NOP;
    }

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;
    
    if (atype != 0)
    {
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, malp);
    }
    else
    {
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 128);
    }
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_CLAMP(0, 0, 0, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_CLAMP_1;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_GREATER, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_ALWAYS);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(n, SCE_GS_TRUE, SCE_GS_TRUE, 340, SCE_GIF_PACKED, 5);
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) 
        | SCE_GS_UV    << (4 * 1) 
        | SCE_GS_XYZF2 << (4 * 2) 
        | SCE_GS_UV    << (4 * 3) 
        | SCE_GS_XYZF2 << (4 * 4);
    
    for (i = 0; i < n; i++)
    {
        pbuf[ndpkt].ui32[0] = 128;
        pbuf[ndpkt].ui32[1] = 128;
        pbuf[ndpkt].ui32[2] = 128;
        if (atype != 0)
        {
            pbuf[ndpkt++].ui32[3] = 128;
        }
        else
        {
            pbuf[ndpkt++].ui32[3] = malp;
        }
        
        pbuf[ndpkt].ui32[0] = u[i];
        pbuf[ndpkt].ui32[1] = 0;
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt++].ui32[3] = 0;
        
        pbuf[ndpkt].ui32[0] = x[i];
        pbuf[ndpkt].ui32[1] = yy[0];
        pbuf[ndpkt].ui32[2] = mz;
        pbuf[ndpkt++].ui32[3] = (i > 0) ? 0 : 0x8000;
        
        pbuf[ndpkt].ui32[0] = u[i];
        pbuf[ndpkt].ui32[1] = v;
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt++].ui32[3] = 0;
        
        pbuf[ndpkt].ui32[0] = x[i];
        pbuf[ndpkt].ui32[1] = yy[3];
        pbuf[ndpkt].ui32[2] = mz;
        pbuf[ndpkt++].ui32[3] = (i > 0) ? 0 : 0x8000;
    }
}

void SetTexDirectS2(int pri, SPRITE_DATA *sd, DRAW_ENV *de, int type)
{
	int n;
	int tw[2];
	int th[2];
	float mx;
	float my;
	u_int mz;
	float mscw;
	float msch;
	float mszw;
	float mszh;
	int mclu;
	int mclv;
	u_int r;
	u_int g;
	u_int b;
	u_int a;
	u_long tex1;
	u_long alpha;
	u_long zbuf;
	u_long test;
	u_long clmp;
	u_long prim;
	sceGsTex0 Load;
	sceGsTex0 Change;
	float div;
	float px;
	float py;
	float pw;
	float ph;
	float xx[2];
	float yy[2];
    
    div = g_bInterlace ? 2.0f : 1.0f;

    mz = sd->pos_z;
    
    mscw = sd->scale_w;
    msch = sd->scale_h;
    
    mszw = sd->size_w * mscw;
    mszh = sd->size_h * msch;
    
    mx = sd->pos_x + (sd->size_w - mszw) * 0.5f;
    my = (sd->pos_y + (sd->size_h - mszh) * 0.5f);

    mclu = sd->clamp_u;
    mclv = sd->clamp_v;
    r = sd->r;
    g = sd->g;
    b = sd->b;
    a = sd->alpha;
    
    tex1 = de->tex1;
    alpha = de->alpha;
    zbuf = de->zbuf;
    test = de->test;
    prim = de->prim;
    clmp = de->clamp;
    
    n = type != 0 ? 0 : 8;
    
    if (mclu == 0)
    {
        tw[0] = n;
        tw[1] = MIN(sd->g_nTexSizeW, 1023) * 16 - n;
    }
    else
    {
        tw[0] = mclu & 0xffff;
        tw[1] = (mclu >> 0x10) & 0xffff;
    }
    
    if (mclv == 0)
    {
        th[0] = n;
        th[1] = MIN(sd->g_nTexSizeH, 1023) * 16 - (th[0]);
    }
    else
    {
        th[0] = mclv & 0xffff;
        th[1] = (mclv >> 0x10) & 0xffff;
    }

    pw = mszw * 16.0f;
    ph = (mszh * 16.0f) / div;

    px = (mx + 2048.0f) * 16.0f;
    py = (my / div + 2048.0f) * 16.0f;

    xx[0] = px;
    xx[1] = pw + px;
    
    yy[0] = py;
    yy[1] = ph + py;
    
    if (sd->g_GsTex0.PSM == SCE_GS_PSMT4)
    {
        Load = sd->g_GsTex0;
        Load.PSM = SCE_GS_PSMT8;
        Load.CSA = 0;
        Load.CLD = 1;
        
        Change = sd->g_GsTex0;
        Change.CSA = 0;
        Change.CLD = 0;
    }
    Reserve2DPacket(pri);
    
    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = DMAend | 15;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(8, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;
    
    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;
    
    if (sd->g_GsTex0.PSM == SCE_GS_PSMT4)
    {
        pbuf[ndpkt].ul64[0] = *(u_long*)&Load;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;
        
        pbuf[ndpkt].ul64[0] = *(u_long*)&Change;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;
    }
    else
    {
        pbuf[ndpkt].ul64[0] = *(u_long*)&sd->g_GsTex0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;
        
        pbuf[ndpkt].ul64[0] = 0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_NOP;
    }
    
    pbuf[ndpkt].ul64[0] = tex1;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;
    
    pbuf[ndpkt].ul64[0] = alpha;
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;
    
    pbuf[ndpkt].ul64[0] = zbuf;
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;
    
    pbuf[ndpkt].ul64[0] = test;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;
    
    pbuf[ndpkt].ul64[0] = clmp;
    pbuf[ndpkt++].ul64[1] = SCE_GS_CLAMP_1;
    
    pbuf[ndpkt].ul64[0] = prim;
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) 
        | SCE_GS_UV    << (4 * 1) 
        | SCE_GS_XYZF2 << (4 * 2) 
        | SCE_GS_UV    << (4 * 3) 
        | SCE_GS_XYZF2 << (4 * 4);
    
    pbuf[ndpkt].ui32[0] = r;
    pbuf[ndpkt].ui32[1] = g;
    pbuf[ndpkt].ui32[2] = b;
    pbuf[ndpkt++].ui32[3] = a;
    
    pbuf[ndpkt].iv[0] = tw[0];
    pbuf[ndpkt].iv[1] = th[0];
    pbuf[ndpkt].iv[2] = 0;
    pbuf[ndpkt++].iv[3] = 0;
    
    pbuf[ndpkt].ui32[0] = (int)xx[0];
    pbuf[ndpkt].ui32[1] = (int)yy[0];
    pbuf[ndpkt].ui32[2] = mz;
    pbuf[ndpkt++].ui32[3] = 0;
    
    pbuf[ndpkt].iv[0] = tw[1];
    pbuf[ndpkt].iv[1] = th[1];
    pbuf[ndpkt].iv[2] = 0;
    pbuf[ndpkt++].iv[3] = 0;
    
    pbuf[ndpkt].ui32[0] = (int)xx[1];
    pbuf[ndpkt].ui32[1] = (int)yy[1];
    pbuf[ndpkt].ui32[2] = mz;
    pbuf[ndpkt++].ui32[3] = 0;
}

void SetTexDirect2(int pri, SPRITE_DATA *sd, DRAW_ENV *de, sceVu0FVECTOR *v)
{
	int tw[2];
	int th[2];
	float mx;
	float my;
	int mz;
	float mscw;
	float msch;
	float mszw;
	float mszh;
	int mclu;
	int mclv;
	u_int r;
	u_int g;
	u_int b;
	u_int a;
	u_long tex1;
	u_long alpha;
	u_long zbuf;
	u_long test;
	u_long clmp;
	u_long prim;
	sceGsTex0 Load;
	sceGsTex0 Change;
	float div;
	float px;
	float py;
	float pw;
	float ph;
	float xx[2];
	float yy[2];
    
    div = g_bInterlace ? 2.0f : 1.0f;

    mz = sd->pos_z;
    
    mscw = sd->scale_w;
    msch = sd->scale_h;
    
    mszw = sd->size_w * mscw;
    mszh = sd->size_h * msch;
    
    mx = sd->pos_x + (sd->size_w - mszw) * 0.5f;
    my = (sd->pos_y + (sd->size_h - mszh) * 0.5f);

    mclu = sd->clamp_u;
    mclv = sd->clamp_v;
    r = sd->r;
    g = sd->g;
    b = sd->b;
    a = sd->alpha;
    
    tex1 = de->tex1;
    alpha = de->alpha;
    zbuf = de->zbuf;
    test = de->test;
    prim = de->prim;
    clmp = de->clamp;
    
    if (mclu == 0)
    {
        tw[0] = 8;
        tw[1] = MIN(sd->g_nTexSizeW, 1023) * 16 - 8;
    }
    else
    {
        tw[0] = mclu & 0xffff;
        tw[1] = (mclu >> 0x10) & 0xffff;
    }
    
    if (mclv == 0)
    {
        th[0] = 8;
        th[1] = MIN(sd->g_nTexSizeH, 1023) * 16 - (th[0]);
    }
    else
    {
        th[0] = mclv & 0xffff;
        th[1] = (mclv >> 0x10) & 0xffff;
    }

    pw = mszw * 16.0f;
    ph = (mszh * 16.0f) / div;

    px = (mx + 2048.0f) * 16.0f;
    py = (my / div + 2048.0f) * 16.0f;

    xx[0] = px;
    xx[1] = pw + px;
    
    yy[0] = py;
    yy[1] = ph + py;
    
    if (sd->g_GsTex0.PSM == SCE_GS_PSMT4)
    {
        Load = sd->g_GsTex0;
        Load.PSM = SCE_GS_PSMT8;
        Load.CSA = 0;
        Load.CLD = 1;
        
        Change = sd->g_GsTex0;
        Change.CSA = 0;
        Change.CLD = 0;
    }

    Reserve2DPacket(pri);
    
    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = DMAend | 15;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(8, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;
    
    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;
    
    if (sd->g_GsTex0.PSM == SCE_GS_PSMT4)
    {
        pbuf[ndpkt].ul64[0] = *(u_long*)&Load;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;
        
        pbuf[ndpkt].ul64[0] = *(u_long*)&Change;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;
    }
    else
    {
        pbuf[ndpkt].ul64[0] = *(u_long*)&sd->g_GsTex0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;
        
        pbuf[ndpkt].ul64[0] = 0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_NOP;
    }
    
    pbuf[ndpkt].ul64[0] = tex1;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;
    
    pbuf[ndpkt].ul64[0] = alpha;
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;
    
    pbuf[ndpkt].ul64[0] = zbuf;
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;
    
    pbuf[ndpkt].ul64[0] = test;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;
    
    pbuf[ndpkt].ul64[0] = clmp;
    pbuf[ndpkt++].ul64[1] = SCE_GS_CLAMP_1;
    
    pbuf[ndpkt].ul64[0] = prim;
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) 
        | SCE_GS_UV    << (4 * 1) 
        | SCE_GS_XYZF2 << (4 * 2) 
        | SCE_GS_UV    << (4 * 3) 
        | SCE_GS_XYZF2 << (4 * 4);
    
    pbuf[ndpkt].ui32[0] = r;
    pbuf[ndpkt].ui32[1] = g;
    pbuf[ndpkt].ui32[2] = b;
    pbuf[ndpkt++].ui32[3] = a;
    
    pbuf[ndpkt].iv[0] = tw[0];
    pbuf[ndpkt].iv[1] = th[0];
    pbuf[ndpkt].iv[2] = 0;
    pbuf[ndpkt++].iv[3] = 0;
    
    pbuf[ndpkt].ui32[0] = (int)xx[0];
    pbuf[ndpkt].ui32[1] = (int)yy[0];
    pbuf[ndpkt].ui32[2] = mz;
    pbuf[ndpkt++].ui32[3] = 0;
    
    pbuf[ndpkt].iv[0] = tw[1];
    pbuf[ndpkt].iv[1] = th[1];
    pbuf[ndpkt].iv[2] = 0;
    pbuf[ndpkt++].iv[3] = 0;
    
    pbuf[ndpkt].ui32[0] = (int)xx[1];
    pbuf[ndpkt].ui32[1] = (int)yy[1];
    pbuf[ndpkt].ui32[2] = mz;
    pbuf[ndpkt++].ui32[3] = 0;
}

void SetTexDirect(SPRITE_DATA *sd, int atype)
{
	int i;
	float xx[4];
	float yy[4];
	unsigned int tw;
	unsigned int th;
	float ss;
	float cc;
	float div;
	int mz;
	float mx;
	float my;
	float mscw;
	float msch;
	float mszw;
	float mszh;
	float mang;
	unsigned int mrc;
	unsigned char malp;
	sceGsTex0 Load;
	sceGsTex0 Change;
	u_int r;
	u_int g;
	u_int b;
	float px;
	float py;
	float pw;
	float ph;

    malp = sd->alpha;
    mrc = sd->rot_center;

    mscw = sd->scale_w;
    msch = sd->scale_h;
    
    mszw = sd->size_w * mscw;
    mszh = sd->size_h * msch;
    
    mang = sd->angle;
    
    mz = sd->pos_z;
    r = sd->r;
    g = sd->g;
    b = sd->b;
    
    mx = sd->pos_x + (sd->size_w - mszw) * 0.5f;
    my = (sd->pos_y + (sd->size_h - mszh) * 0.5f);
    
    div = g_bInterlace ? 2.0f : 1.0f;

    tw = MIN(sd->g_nTexSizeW, 1023);
    th = MIN(sd->g_nTexSizeH, 1023);

    tw *= 16;
    th *= 16;

    ph = mszh * 16.0f;
    px = (mx + 2048.0f) * 16.0f;
    
    pw = mszw * 16.0f;
    py = (my / div + 2048.0f) * 16.0f;
    
    if (mang == 0.0f)
    {
        xx[0] = 0.0;
        yy[0] = 0.0;
        
        xx[1] = 0.0;
        yy[1] = ph / div;
        
        xx[2] = pw;
        yy[2] = 0.0;
        
        xx[3] = pw;
        yy[3] = ph / div;
    }
    else
    {
        ss = SgSinf((mang * PI) / 180.0f);
        cc = SgCosf((mang * PI) / 180.0f);
        
        switch(mrc)
        {
        case 0:
            xx[0] = 0.0f;
            yy[0] = 0.0f;
            
            xx[1] = 0.0f;
            yy[1] = ph / div;
            
            xx[2] = pw;
            yy[2] = 0.0f;
            
            xx[3] = pw;
            yy[3] = ph / div;
        break;
        case 1:
            xx[0] = (-pw * 0.5f * cc) + (ph* 0.5f * ss) + (pw * 0.5f);
            yy[0] = ((-pw * 0.5f * ss) - ((ph * 0.5f * cc)) + (ph * 0.5f)) / div;
            xx[1] = (-pw * 0.5f * cc) - (ph* 0.5f * ss) + (pw * 0.5f);
            yy[1] = ((-pw * 0.5f * ss) + (ph * 0.5f * cc) + (ph * 0.5f)) / div;
            xx[2] = (pw * 0.5f * cc) + (ph * 0.5f * ss ) + (pw * 0.5f);
            yy[2] = ((pw * 0.5f * ss) - (ph * 0.5f * cc) + (ph * 0.5f)) / div;
            yy[3] = ((pw * 0.5f * ss) + (ph * 0.5f * cc) + (ph * 0.5f)) / div;
            xx[3] = (pw * 0.5f * cc) - (ph * 0.5f * ss) + (pw * 0.5f);
        break;
        case 2:
            xx[0] = (-pw * 0.5f * cc) + (ph * 0.5f * ss);
            yy[0] = ((-pw * 0.5f * ss) - (ph * 0.5f * cc)) / div;
            xx[1] = (-pw * 0.5f * cc) - (ph * 0.5f * ss);
            yy[1] = ((-pw * 0.5f * ss) + (ph * 0.5f * cc)) / div;
            xx[2] = (pw * 0.5f * cc) + (ph * 0.5f * ss);
            yy[2] = ((pw * 0.5f * ss) - (ph * 0.5f * cc)) / div;
            xx[3] = (pw * 0.5f * cc) - (ph * 0.5f * ss);
            yy[3] = ((pw * 0.5f * ss) + (ph * 0.5f * cc)) / div;
        break;
        case 3:
            xx[0] = 0.0f;
            yy[0] = 0.0f;
            xx[1] = -ph * ss;
            yy[1] = (ph * cc) / div;
            xx[2] = pw * cc;
            yy[2] = (pw * ss) / div;
            xx[3] = pw * cc - ph * ss;
            yy[3] = (pw * ss + ph * cc) / div;
        break;
        case 4:
            xx[0] = ph * ss;
            yy[0] = (-ph * cc) / div;
            xx[1] = 0.0f;
            yy[1] = 0.0f;
            xx[2] = pw * cc + ph * ss;
            yy[2] = (pw * ss - ph * cc) / div;
            xx[3] = pw * cc;
            yy[3] = (pw * ss) / div;
        break;
        case 5:
            xx[0] = -pw * cc;
            yy[0] = (-pw * ss) / div;
            xx[1] = (-pw * cc) - ph * ss;
            yy[1] = ((-pw * ss) + ph * cc) / div;
            xx[2] = 0.0f;
            yy[2] = 0.0f;
            xx[3] = -ph * ss;
            yy[3] = (ph * cc) / div;
        break;
        case 6:
            xx[0] = -pw * cc + ph * ss;
            yy[0] = ((-pw * ss) - ph * cc) / div;
            xx[1] = -pw * cc;
            yy[1] = (-pw * ss) / div;
            xx[2] = ph * ss;
            yy[2] = (-ph * cc) / div;
            xx[3] = 0.0f;
            yy[3] = 0.0f;
        break;
        }
    }

    for (i = 0; i < 4; i++)
    {
        xx[i] += px;
        yy[i] += py;
    }
    
    if (sd->g_GsTex0.PSM == SCE_GS_PSMT4)
    {
        Load = sd->g_GsTex0;
        Load.PSM = SCE_GS_PSMT8;
        Load.CSA = 0;
        Load.CLD = 1;
        
        Change = sd->g_GsTex0;
        Change.CSA = 0;
        Change.CLD = 0;
    }
    
    Reserve2DPacket(0xffffffff);

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = DMAend | 19;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(8, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;
    
    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;
    
    if (sd->g_GsTex0.PSM == SCE_GS_PSMT4)
    {
        pbuf[ndpkt].ul64[0] = *(u_long*)&Load;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;
        
        pbuf[ndpkt].ul64[0] = *(u_long*)&Change;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;
    }
    else
    {
        pbuf[ndpkt].ul64[0] = *(u_long*)&sd->g_GsTex0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;
        
        pbuf[ndpkt].ul64[0] = 0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_NOP;
    }

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;
    if (atype != 0)
    {
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_FIX, SCE_GS_ALPHA_CD, malp);
    }
    else
    {
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_EQUAL, SCE_GS_ALPHA_CS, 0);
    }
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_CLAMP(0, 0, 0, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_CLAMP_1;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_FALSE, SCE_GS_ALPHA_NEVER, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 340, SCE_GIF_PACKED, 9);
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) 
        | SCE_GS_UV    << (4 * 1) 
        | SCE_GS_XYZF2 << (4 * 2) 
        | SCE_GS_UV    << (4 * 3) 
        | SCE_GS_XYZF2 << (4 * 4) 
        | SCE_GS_UV    << (4 * 5) 
        | SCE_GS_XYZF2 << (4 * 6) 
        | SCE_GS_UV    << (4 * 7) 
        | (u_long)SCE_GS_XYZF2 << (4 * 8);
    
    pbuf[ndpkt].ui32[0] = r;
    pbuf[ndpkt].ui32[1] = g;
    pbuf[ndpkt].ui32[2] = b;
    pbuf[ndpkt++].ui32[3] = 128;
    
    pbuf[ndpkt].ui32[0] = 8;
    pbuf[ndpkt].ui32[1] = 8;
    pbuf[ndpkt].ui32[2] = 0;
    pbuf[ndpkt++].ui32[3] = 0;
    
    pbuf[ndpkt].ui32[0] = (int)xx[0];
    pbuf[ndpkt].ui32[1] = (int)yy[0];
    pbuf[ndpkt].ui32[2] = mz;
    pbuf[ndpkt++].ui32[3] = 0;
    
    pbuf[ndpkt].ui32[0] = 8;
    pbuf[ndpkt].ui32[1] = th - 8;
    pbuf[ndpkt].ui32[2] = 0;
    pbuf[ndpkt++].ui32[3] = 0;
    
    pbuf[ndpkt].ui32[0] = (int)xx[1];
    pbuf[ndpkt].ui32[1] = (int)yy[1];
    pbuf[ndpkt].ui32[2] = mz;
    pbuf[ndpkt++].ui32[3] = 0;
    
    pbuf[ndpkt].ui32[0] = tw - 8;
    pbuf[ndpkt].ui32[1] = 8;
    pbuf[ndpkt].ui32[2] = 0;
    pbuf[ndpkt++].ui32[3] = 0;
    
    pbuf[ndpkt].ui32[0] = (int)xx[2];
    pbuf[ndpkt].ui32[1] = (int)yy[2];
    pbuf[ndpkt].ui32[2] = mz;
    pbuf[ndpkt++].ui32[3] = 0;
    
    pbuf[ndpkt].ui32[0] = tw - 8;
    pbuf[ndpkt].ui32[1] = th - 8;
    pbuf[ndpkt].ui32[2] = 0;
    pbuf[ndpkt++].ui32[3] = 0;
    
    pbuf[ndpkt].ui32[0] = (int)xx[3];
    pbuf[ndpkt].ui32[1] = (int)yy[3];
    pbuf[ndpkt].ui32[2] = mz;
    pbuf[ndpkt++].ui32[3] = 0;
}

void SetScreenDSlide(int addr, int type, int alpha, float ang, float scl, int z)
{
    SPRITE_DATA sd = {
        .g_GsTex0 = {
            .TBP0 = 0,
            .TBW = 10,
            .PSM = 0,
            .TW = 10,
            .TH = 8,
            .TCC = 1,
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
        .r = 128,
        .g = 128,
        .b = 128,
        .alpha = 128,
        .mask = 0,
    };
    // FIXME
    DRAW_ENV de = {
        .tex1 = 0x161,
        .alpha = 0x8000000064,
        .zbuf = 0x10100008c,
        .test = 0x5000d,
        .clamp = 0,
        .prim = 0x50ab400000008001,
    };

    switch(type) 
    {
        case 0:
            sd.pos_x = -320.5f;
            sd.pos_y = -224.5f;
            break;
        case 1:
            sd.pos_x = -320.5f;
            sd.pos_y = -224.0f;
            break;
        case 2:
            sd.pos_x = -320.0f;
            sd.pos_y = -224.0f;
            break;
        case 3:
            sd.pos_x = -320.0f;
            sd.pos_y = -223.5f;
            break;
        case 4:
            sd.pos_x = -319.5f;
            sd.pos_y = -223.5f;
            break;
        case 5:
            sd.pos_x = -319.5f;
            sd.pos_y = -223.0f;
            break;
    }
    
    sd.alpha = alpha;
    sd.angle = ang;
    sd.rot_center = 1;
    sd.g_GsTex0.TBP0 = addr;
    sd.scale_w = scl;
    sd.scale_h = scl;
    sd.pos_z = z;
    
    SetTexDirectS2(0, &sd, &de, 0);
}

void SetScreenZ(int addr)
{
	SPRITE_DATA sd = {
        .g_GsTex0 = {
              .TBP0 = 0,
              .TBW = 10,
              .PSM = 49,
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
        .pos_y = -224.0f,
        .pos_z = 0,
        .size_w = 640.0f,
        .size_h = 448.0f,
        .scale_w = 1.0f,
        .scale_h = 1.0f,
        .clamp_u = 0,
        .clamp_v = 0,
        .rot_center = 0,
        .angle = 0.0,
        .r = 255,
        .g = 255,
        .b = 255,
        .alpha = 128,
        .mask = 0,
    };
    
    sd.g_GsTex0.TBP0 = addr;
    
    sd.pos_z = 0xfffffef;
    
    SetTexDirectS(16, &sd, 1);
}

void CaptureScreen(u_int addr)
{
    LocalCopyLtoB2(1, (sys_wrk.count & 1) * 224 * 10);
}

void DrawScreen(u_int pri, u_int addr, u_char r, u_char g, u_char b, u_char a)
{
    DISP_SPRT ds;
    
    
    SPRT_DAT sd =
    {
        .tex0 = 0,
        .u = 0,
        .v = 0,
        .w = 640,
        .h = 224,
        .x = 0,
        .y = 0,
        .pri = 0,
        .alpha = 128,
    };

    LocalCopyBtoL(1, addr);
    CopySprDToSpr(&ds, &sd); 
    
    ds.tex0 = SCE_GS_SET_TEX0(addr, 640 / 64, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, 0, 0, 0, 1); // addr | 0x2000000228028000;
    ds.tex1 = SCE_GS_SET_TEX1(1, 0, SCE_GS_NEAREST, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0); // 0x141;
    ds.zbuf = SCE_GS_SET_ZBUF(0x8c, SCE_GS_PSMCT24, 0); // 0x100008c;
    ds.test = SCE_GS_SET_TEST(SCE_GS_TRUE, SCE_GS_ALPHA_GREATER, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_GEQUAL); // 0x5000d;
    ds.alphar = SCE_GS_SET_ALPHA(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0); // 0x44;
    ds.pri = pri;
    ds.texa = 0;
    ds.x = -0.5;
    ds.y = -0.5;
    ds.z =  0xfffffff - pri;
    ds.scw = 1.0;
    ds.sch = 2.0;
    ds.csx = -0.5;
    ds.csy = -0.5;
    ds.r = r;
    ds.g = g;
    ds.b = b;
    ds.alpha = a;
    
    DispSprD(&ds);
}

void ClearFBuffer()
{
	int i;
	int x[2];
	int y[2];
	int div;

    div = g_bInterlace ? 2 : 1;

    x[0] = GS_X_COORD(0);
    x[1] = GS_X_COORD(64);
    y[0] = (2048 - SCR_HEIGHT / div) * 16;
    y[1] = (SCR_HEIGHT / div + 2048) * 16;
    
    Reserve2DPacket(0x1000);

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt].ui32[0] = DMAend | 34;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(2, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt].ul64[1] = SCE_GIF_PACKED_AD;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_ALWAYS, SCE_GS_FALSE, SCE_GS_AFAIL_KEEP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_ALWAYS);
    pbuf[ndpkt].ul64[1] = SCE_GS_TEST_1;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt].ul64[1] = SCE_GS_ZBUF_1;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(10, SCE_GS_TRUE, SCE_GS_TRUE, 6, SCE_GIF_PACKED, 3);
    pbuf[ndpkt].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) 
        | SCE_GS_XYZF2 << (4 * 1) 
        | SCE_GS_XYZF2 << (4 * 2);
    ndpkt++;
    
    for (i = 0; i < 10; i++)
    {        
        pbuf[ndpkt].ui32[0] = 0;
        pbuf[ndpkt].ui32[1] = 0;
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt].ui32[3] = 0;
        ndpkt++;
        
        pbuf[ndpkt].ui32[0] = x[0] + (i * 1024);
        pbuf[ndpkt].ui32[1] = y[0];
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt].ui32[3] = 0;
        ndpkt++;
        
        pbuf[ndpkt].ui32[0] = x[1] + (i * 1024);
        pbuf[ndpkt].ui32[1] = y[1];
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt].ui32[3] = 0;
        ndpkt++;
    }
}

void ClearZBuffer()
{
	int i;
	int x[2];
	int y[2];
	int div;

    div = g_bInterlace ? 2 : 1;

    x[0] = GS_X_COORD(0);
    x[1] = GS_X_COORD(64);
    y[0] = (2048 - SCR_HEIGHT / div) * 16;
    y[1] = (SCR_HEIGHT / div + 2048) * 16;
    
    Reserve2DPacket(0x1000);

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt].ui32[0] = DMAend | 34;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(2, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt].ul64[1] = SCE_GIF_PACKED_AD;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(SCE_GS_TRUE, SCE_GS_ALPHA_NEVER, SCE_GS_FALSE, SCE_GS_AFAIL_ZB_ONLY, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE, SCE_GS_DEPTH_ALWAYS);
    pbuf[ndpkt].ul64[1] = SCE_GS_TEST_1;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
    pbuf[ndpkt].ul64[1] = SCE_GS_ZBUF_1;
    ndpkt++;
    
    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(10, SCE_GS_TRUE, SCE_GS_TRUE, 6, SCE_GIF_PACKED, 3);
    pbuf[ndpkt].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0) 
        | SCE_GS_XYZF2 << (4 * 1) 
        | SCE_GS_XYZF2 << (4 * 2);
    ndpkt++;
    
    for (i = 0; i < 10; i++)
    {        
        pbuf[ndpkt].ui32[0] = 0;
        pbuf[ndpkt].ui32[1] = 0;
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt].ui32[3] = 0;
        ndpkt++;
        
        pbuf[ndpkt].ui32[0] = x[0] + (i * 1024);
        pbuf[ndpkt].ui32[1] = y[0];
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt].ui32[3] = 0;
        ndpkt++;
        
        pbuf[ndpkt].ui32[0] = x[1] + (i * 1024);
        pbuf[ndpkt].ui32[1] = y[1];
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt].ui32[3] = 0;
        ndpkt++;
    }
}

void CamSave()
{
    old_cam_no = plyr_wrk.pr_info.camera_no;
}

int CamChangeCheck()
{
    return old_cam_no != plyr_wrk.pr_info.camera_no;
}

void* GetEmptyBuffer(int no)
{
    switch (no)
    {
        case 0:
        return buf;
        case 1:
        return buf2;
        case 2:
        return bufz;
    }
    
    return buf;
}

void CheckPointDepth(PP_JUDGE *ppj)
{
	int i;
	int xx[32];
	int yy[32];
	int clip[32];
	sceVu0FMATRIX wlm;
	sceVu0FMATRIX slm;
	sceVu0IVECTOR ivec[32];
	sceVu0FVECTOR bpos[32];
	sceVu0FVECTOR fzero = { 0.0f, 0.0f, 0.0f, 1.0f };
	sceVu0FVECTOR ofst_pos[5] = {
        { 0.0f, -32.0f, 0.0f, 1.0f },
        { 0.0f, -20.0f, 3.5f, 1.0f },
        { 4.2f, -20.0f, 0.0f, 1.0f },
        { 0.0f, -20.0f, -2.5f, 1.0f },
        { -4.3f, -20.0f, 0.0f, 1.0f },
    };
	float fr_f;
	static sceGsStoreImage gs_simage1;
	Q_WORDDATA q;
	int n1;
	int n2;
	u_int ui;
    
    int n = ppj->num; // you sneaky bastard !!!!
    
    for (i = 0; i < n; i++)
    {
        sceVu0UnitMatrix(wlm);
        
        wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;
        
        sceVu0TransMatrix(wlm, wlm, ppj->p[i]);
        sceVu0MulMatrix(slm, SgWSMtx, wlm);
        sceVu0RotTransPers(ivec[i], slm, fzero, 0);
        
        clip[i] = 0;

        if (ivec[i][0] < GS_X_COORD(0) || ivec[i][0] > GS_X_COORD(SCR_WIDTH))
        {
            clip[i]++;
        }

        if (ivec[i][1] < GS_Y_COORD(0) || ivec[i][1] > GS_Y_COORD(SCR_HEIGHT))
        {
            clip[i]++;
        }

        if (ivec[i][2] < 0xff || ivec[i][2] > 0xffffff)
        {
            clip[i]++;
        }

        xx[i] = (ivec[i][0] - GS_X_COORD(0) + 8) / 16;

        fr_f = (sys_wrk.count & 1) ? 0.5f : 0.0f;
        yy[i] = (ivec[i][1] + (int)((fr_f * 0.5f + -1935.5f) * 16.0f)) / 16;
        
        if (clip[i] == 0)
        {
            n1 = xx[i] / 4;
            n2 = xx[i] % 4;
            
            sceGsSetDefStoreImage(
                &gs_simage1,
                (DISP_WIDTH * DISP_HEIGHT) / 64,
                DISP_WIDTH / 64,
                SCE_GS_PSMZ32,
                n1 * 4,
                yy[i],
                4,
                1);
            CheckDMATrans();
            sceGsSyncPath(0, 0);
            FlushCache(0);
            sceGsExecStoreImage(&gs_simage1, &q.ul128);
            sceGsSyncPath(0, 0);

            ui = q.ui32[n2] & 0xffffff;
            
            if (ivec[i][2] >> 4 <= ui)
            {
                clip[i]++;
            }
        }
        
        ppj->result[i] = clip[i] == 0;
    }
}

void GetCamI2DPos(sceVu0FVECTOR pos, float *tx, float *ty)
{
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0FVECTOR vt = {0.0f, 0.0f, 0.0f, 1.0f};
    sceVu0FVECTOR vtw;
    float pl;
  
    pl = 0.0f;
    
    sceVu0UnitMatrix(wlm);
    
    wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;
    
    sceVu0TransMatrix(wlm, wlm, pos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);
    sceVu0ApplyMatrix(vtw, slm, vt);
    sceVu0ScaleVector(vtw, vtw, 1.0f / vtw[3]); // W can be zero ... OOPS!
    
    *tx = (vtw[0] - 2048.0f) + 320.0f;
    *ty = (vtw[1] - 2048.0f) + 112.0f + pl;
}

void Get2PosRot(sceVu0FVECTOR v1, sceVu0FVECTOR v2, float *x, float *y)
{
	float l;
	float fx;
	float fy;
	float fz;

    fx = v2[0] - v1[0];
    fy = v2[1] - v1[1];
    fz = v2[2] - v1[2];
    

    l = SgSqrtf(fx * fx + fz * fz);
    
    *y = SgAtan2f(fx, fz);
    *x = SgAtan2f(l, fy) + PI_HALF;
    
    // TODO: these are macros. create one!
    *x = (*x > PI) ? (*x - TWO_PI) : ((*x < -PI) ? *x + TWO_PI : *x);
    *y = (*y > PI) ? (*y - TWO_PI) : ((*y < -PI) ? *y + TWO_PI : *y);
}

void Get2PosRot2(sceVu0FVECTOR v1, sceVu0FVECTOR v2, float *x, float *z)
{
    float fx; // doesn't get into STABS but Get2PosRot has it
	float fy;
	float fz;

    fx = v2[0] - v1[0];
    fy = v2[1] - v1[1];
    fz = v2[2] - v1[2];
    
    *z = SgAtan2f(fx, fy);
    *x = SgAtan2f(fz, fy);

    // TODO: these are macros. create one!
    *x = (*x > PI) ? (*x - TWO_PI) : ((*x < -PI) ? *x + TWO_PI : *x);
    *z = (*z > PI) ? (*z - TWO_PI) : ((*z < -PI) ? *z + TWO_PI : *z);
}

void GetTrgtRotType2(float *p0, float *p1, float *rot, int id)
{
	sceVu0FVECTOR dist;
    
    rot[0] = 0.0f;
    rot[1] = 0.0f;
    rot[2] = 0.0f;
    rot[3] = 0.0f;
    
    sceVu0SubVector(dist, p1, p0);
    
    if (id & 1)
    {
        dist[3] = GetDistV(p0, p1);
        *rot = SgAtan2f(dist[1], dist[3]);
    }
    
    if (id & 2)
    {
        rot[1] = SgAtan2f(dist[0], dist[2]);
    }
}

void CalcSimEquations(double (*sq)[3], float *x, float *y)
{
	double d;
	int i;
	int j;
	int k;    

    for (k = 0; k < 1; k++)
    {
        for (i = k + 1; i < 2; i++)
        {
            d = sq[i][k] / sq[k][k];
            
            for (j = k + 1; j < 3; j++)
            {
                sq[i][j] -= sq[k][j] * d;
            }
        }
    }
    
    for (i = 1; i >= 0; i--)
    {
        d = sq[i][2];
        
        for (j = i + 1; j < 2; j++)
        {
            d -= sq[i][j] * sq[j][2];
        }
        
        sq[i][2] = d / sq[i][i];
    }
    
    *x = sq[0][2];
    *y = sq[1][2];
}

float Get2PLength(float *v1, float *v2)
{
	float xx;
	float yy;
	float zz;

    xx = (v2[0] - v1[0]) * (v2[0] - v1[0]);
    yy = (v2[1] - v1[1]) * (v2[1] - v1[1]);
    zz = (v2[2] - v1[2]) * (v2[2] - v1[2]);

    return SgSqrtf(xx + yy + zz);
}

int log_2(float num)
{
	int n;
    
    n = 0;
    
    if (num / 2 > 1)
    {
        n = log_2(num / 2);
    }
    
    return n + 1;
}

void LocalCopyLtoB_Sub(int no, int type, int addr) {
	u_long128 *pbuf;
	static sceGsStoreImage gs_simage1;
	static sceGsStoreImage gs_simage2;
    
    pbuf = (no != 0) ? buf2 : buf;
    
    sceGsSetDefStoreImage(&gs_simage1, addr, SCR_WIDTH / 64, SCE_GS_PSMCT32, 0, 0, SCR_WIDTH, 200);
    sceGsSetDefStoreImage(&gs_simage2, addr, SCR_WIDTH / 64, SCE_GS_PSMCT32, 0, 200, SCR_WIDTH, 24);
    
    CheckDMATrans();
    
    sceGsSyncPath(0, 0);
    
    FlushCache(0);
    
    sceGsExecStoreImage(&gs_simage1, pbuf);
    sceGsExecStoreImage(&gs_simage2, &pbuf[32000]);
    
    if (type == 0)
    {
        return;
    }
    
    sceGsSyncPath(0, 0);
}

void LocalCopyLtoB_Sub2(int no, int type, int addr)
{
    return;
}

void LocalCopyBtoL_Sub(int no, int type, int addr) {
	u_long128 *bbuf;
	int nloop;
	int bline;
	int rline;
	int oline;
    
    bbuf = (no != 0) ? buf2 : buf;

    bline = 100;
    rline = SCR_HEIGHT;
    
    Reserve2DPacket(0x1000);

    while (rline > 0)
    {
        oline = (rline > bline) ? bline : rline;
        nloop = oline * (SCR_WIDTH / 4);
        
        pbuf[ndpkt].ui32[0] = DMAcnt | 6;
        pbuf[ndpkt].ui32[1] = 0;
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt].ui32[3] = DMAcall | 6;
        ndpkt++;
        
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
        pbuf[ndpkt].ul64[1] = SCE_GIF_PACKED_AD;
        ndpkt++;
        
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_BITBLTBUF(0, 0, 0, addr, 10, 0);
        pbuf[ndpkt].ul64[1] = SCE_GS_BITBLTBUF;
        ndpkt++;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TRXPOS(0, 0, 0, (SCR_HEIGHT) - rline, 0);
        pbuf[ndpkt].ul64[1] = SCE_GS_TRXPOS;
        ndpkt++;
        
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TRXREG(SCR_WIDTH, oline);
        pbuf[ndpkt].ul64[1] = SCE_GS_TRXREG;
        ndpkt++;
        
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TRXDIR(0);
        pbuf[ndpkt].ul64[1] = SCE_GS_TRXDIR;
        ndpkt++;

        // pbuf[ndpkt].ul64[0] = nloop | 0x1800000000008000;
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(nloop, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_IMAGE, 1);
        pbuf[ndpkt].ul64[1] = SCE_GIF_PACKED_AD;
        ndpkt++;
        
        pbuf[ndpkt].ui32[0] = DMAref + nloop; // +? not |? probably a compiler opt. that will fix itself .
        pbuf[ndpkt].ui32[1] = (u_int)&bbuf[((SCR_HEIGHT) - rline) * (SCR_WIDTH / 4)];
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt].ui32[3] = DMAcall | nloop;
        ndpkt++;
        
        rline -= oline;
    }
    
    
    pbuf[ndpkt].ui32[0] = DMAcnt;
    pbuf[ndpkt].ui32[1] = 0;
    pbuf[ndpkt].ui32[2] = 0;
    pbuf[ndpkt].ui32[3] = SCE_VIF1_SET_FLUSH(0);
    ndpkt++;
    
    pbuf[ndpkt].ui32[0] = DMAend;
    pbuf[ndpkt].ui32[1] = 0;
    pbuf[ndpkt].ui32[2] = 0;
    pbuf[ndpkt].ui32[3] = 0;
}

int GetYOffset()
{
    return (int)GS_Y_COORD(0) + 8 - pdrawenv->xyoffset1.OFY;
}

float GetYOffsetf()
{
    return ((int)GS_Y_COORD(0) + 8 - pdrawenv->xyoffset1.OFY) * 0.0625f;
}

void LocalCopyLtoB2(int no, int addr)
{
    LocalCopyLtoB_Sub(no, 1, addr);
}

void LocalCopyLtoB(int no, int addr)
{
    LocalCopyLtoB_Sub2(no, 1, addr);
}

void LocalCopyBtoL(int no, int addr)
{
    LocalCopyBtoL_Sub(no, 1, addr);
}


void LocalCopyLtoB_NB(int no, int addr)
{
    LocalCopyLtoB_Sub(no, 0, addr);
}

void LocalCopyBtoL_NB(int no, int addr)
{
    LocalCopyBtoL_Sub(no, 0, addr);
}

void ClearLocalCopyLtoLCache()
{
    old_ltol_addr1 = old_ltol_addr2 = -1;
}

int LocalCopyLtoLDraw(int addr1, int addr2)
{
	int i;
	int old_ndpkt;
	int xyoff;
	Q_WORDDATA *ppbuf;
	// float *v0;
	// float *v1;
	// float *v0;
	// float *v1;
	// float *v0;
	// float *v1;

    xyoff = GS_Y_COORD(0) + 8 - pdrawenv->xyoffset1.OFY;
    
    Reserve2DPacket(0x1000);
    
    ppbuf = Get2DPacketBufferAddress();
    
    ppbuf[1].ul64[0] = SCE_GIF_SET_TAG(7, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    ppbuf[1].ul64[1] = SCE_GIF_PACKED_AD;
    
    ppbuf[2].ul64[0] = 0;
    ppbuf[2].ul64[1] = SCE_GS_TEXFLUSH;

    ppbuf[3].ul64[0] = SCE_GS_SET_FRAME(addr2 / 32, 10, 0, 0);
    ppbuf[3].ul64[1] = SCE_GS_FRAME_1;

    ppbuf[4].ul64[0] = SCE_GS_SET_XYOFFSET_1((2048-(SCR_WIDTH/2))*16, (2048-(SCR_HEIGHT/2))*16+8);
    ppbuf[4].ul64[1] = SCE_GS_XYOFFSET_1;
    
    ppbuf[5].ul64[0] = SCE_GS_SET_TEST(2, 0, 0, 1, 0, 0, 0, SCE_GS_ZALWAYS);
    ppbuf[5].ul64[1] = SCE_GS_TEST_1;

    ppbuf[6].ul64[0] = SCE_GS_SET_ZBUF(pdrawenv->zbuf1.ZBP, pdrawenv->zbuf1.PSM, 0);
    ppbuf[6].ul64[1] = SCE_GS_ZBUF_1;
    
    ppbuf[7].ul64[0] = SCE_GS_SET_TEX1(1, 0, SCE_GS_NEAREST, SCE_GS_NEAREST, 0, 0, 1);
    ppbuf[7].ul64[1] = SCE_GS_TEX1_1;
    
    ppbuf[8].ul64[0] = SCE_GS_SET_TEX0(addr1, 10, SCE_GS_PSMCT32, 10, 8, 3, 1, 0, SCE_GS_PSMCT32, 0, 0, 0);
    ppbuf[8].ul64[1] = SCE_GS_TEX0_1;

    ppbuf += 9;
    
    for (i = 0; i < SCR_WIDTH; i += 64, ppbuf += 5)
    {
        ppbuf[0].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 278, SCE_GIF_PACKED, 4);
        ppbuf[0].ul64[1] = 0 \
        | SCE_GS_UV    << (4 * 0) 
        | SCE_GS_XYZF2 << (4 * 1) 
        | SCE_GS_UV    << (4 * 2) 
        | SCE_GS_XYZF2 << (4 * 3);

        // UV, they are Q10.4 fixed point
        ppbuf[1].ui32[0] = i * 16; // U
        ppbuf[1].ui32[1] = 0 * 16; // V

        // XYZF2, X and Y are Q16.4, Z is 24bit and F is 8bit
        ppbuf[2].ui32[0] = GS_X_COORD(i); // X
        ppbuf[2].ui32[1] = GS_Y_COORD(0); // Y
        ppbuf[2].ul64[1] = 0; // Z = 0, F = 0, dbit = 0

        // UV, they are Q10.4 fixed point
        ppbuf[3].ui32[0] = SCR_WIDTH * 2 + (i - 17) * 16; // U
        ppbuf[3].ui32[1] = SCR_HEIGHT * 16; // V

        // XYZF2, X and Y are Q16.4, Z is 24bit and F is 8bit
        ppbuf[4].ui32[0] = GS_X_COORD(i + 64) - 8; // X
        ppbuf[4].ui32[1] = GS_Y_COORD(SCR_HEIGHT) + 8; // Y
        ppbuf[4].ul64[1] = 0; // Z = 0, F = 0, dbit = 0
    }
    
    ppbuf[0].ul64[0] = SCE_GIF_SET_TAG(3, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    ppbuf[0].ul64[1] = SCE_GIF_PACKED_AD;
    
    Vu0CopyVector(ppbuf[1].fl32, (float *)&pdrawenv->frame1);
    
    Vu0CopyVector(ppbuf[2].fl32, (float *)&pdrawenv->zbuf1);
    
    Vu0CopyVector(ppbuf[3].fl32, (float *)&pdrawenv->xyoffset1);
    
    old_ndpkt = ndpkt;
    
    Set2DPacketBufferAddress(&ppbuf[4]);

    ppbuf = &pbuf[old_ndpkt];
    
    i = (ndpkt - old_ndpkt) - 1;

    ppbuf->ui32[0] = DMAcnt | i;
    ppbuf->ui32[1] = 0;
    ppbuf->ui32[2] = SCE_VIF1_SET_FLUSH(0);
    ppbuf->ui32[3] = SCE_VIF1_SET_DIRECT(i, 0);

    return -xyoff;
}

void LocalCopyLtoL(int addr1, int addr2)
{
	int i;
    Q_WORDDATA *pbuf;
    
    Reserve2DPacket(0x1000);
    
    pbuf = Get2DPacketBufferAddress();
    
    pbuf->ui32[0] = DMAcnt | 2; // DMAcnt - 2 QW worth of data
    pbuf->ui32[1] = 0;
    // CHCR.TTE needs to be 1, so this 2 u32's are transferred
    pbuf->ui32[2] = SCE_VIF1_SET_DIRECT(2, 0);
    pbuf->ui32[3] = SCE_VIF1_SET_FLUSH(0);
    pbuf++;
    
    pbuf->ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf->ul64[1] = SCE_GIF_PACKED_AD;
    pbuf++;
    
    pbuf->ul64[0] = 0;
    pbuf->ul64[1] = SCE_GS_TEXFLUSH;
    pbuf++;
    
    pbuf->ui32[0] = DMAcnt | 32; // DMAcnt - 32 QW worth of data
    pbuf->ui32[1] = 0;
    // CHCR.TTE needs to be 1, so this 2 u32's are transferred
    pbuf->ui32[2] = 0; 
    pbuf->ui32[3] = SCE_VIF1_SET_DIRECT(32, 0);
    pbuf++;
    
    pbuf->ul64[0] = SCE_GIF_SET_TAG(31, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf->ul64[1] = SCE_GIF_PACKED_AD;
    pbuf++;

    pbuf->ul64[0] = SCE_GS_SET_BITBLTBUF(addr1, 10, 0, addr2, 10, 0);
    pbuf->ul64[1] = SCE_GS_BITBLTBUF;
    pbuf++;
    
    for (i = 0; i < 10; i++, pbuf += 3)
    {
        pbuf[0].ul64[0] = SCE_GS_SET_TRXPOS(i * 64, 0, i * 64, 0, 0);
        pbuf[0].ul64[1] = SCE_GS_TRXPOS;

        pbuf[1].ul64[0] = SCE_GS_SET_TRXREG(64, 224);
        pbuf[1].ul64[1] = SCE_GS_TRXREG;

        pbuf[2].ul64[0] = SCE_GS_SET_TRXDIR(2);
        pbuf[2].ul64[1] = SCE_GS_TRXDIR;
    }
    
    pbuf->ui32[0] = DMAcnt | 0; // DMAcnt - 0 QW worth of data (just the embedded data in the tag)
    pbuf->ui32[1] = 0;
    pbuf->ui32[2] = 0;
    pbuf->ui32[3] = SCE_VIF1_SET_FLUSH(0);
    pbuf++;
    
    Set2DPacketBufferAddress(pbuf);
}

void LocalCopyZtoBZ()
{
	static sceGsStoreImage gs_simage1;

    sceGsSetDefStoreImage(&gs_simage1, 4480, 10, 49, 0, 0, 640, 224);
    sceGsSyncPath(0, 0);
    CheckDMATrans();
    FlushCache(0);
    sceGsExecStoreImage(&gs_simage1,bufz);
    sceGsSyncPath(0, 0);
}

void LocalCopyBZtoZ()
{
	static sceGsLoadImage gs_limage1;

    sceGsSetDefLoadImage(&gs_limage1, 4480, 10, 49, 0, 0, 640, 224);
    FlushCache(0);
    sceGsExecLoadImage(&gs_limage1, bufz);
    sceGsSyncPath(0, 0);
}

void LocalCopyLtoBD(int addr, void *outbuf)
{
    static sceGsStoreImage gs_simage1;
    static sceGsStoreImage gs_simage2;
    
    sceGsSetDefStoreImage(&gs_simage1, addr, 10, 0, 0, 0, 640, 200);
    sceGsSetDefStoreImage(&gs_simage2, addr, 10, 0, 0, 200, 640, 24);
    CheckDMATrans();
    sceGsSyncPath(0, 0);
    FlushCache(0);
    sceGsExecStoreImage(&gs_simage1, outbuf);
    sceGsExecStoreImage(&gs_simage2, outbuf + 0x7d000);
    sceGsSyncPath(0, 0);
}

static SPRITE_DATA commonsp;

void SetVibrate(int type, int time, int pow)
{
    if (type != 0)
    {
        vib2_time = time;
        vib2_pow = pow;
    }
    else
    {
        vib1_time = time;
    }
}

void CallVibrate()
{
    if (vib1_time-- > 0)
    {
        VibrateRequest1(0, 1);
    }
    
    if (vib2_time-- > 0)
    {
        VibrateRequest2(0, vib2_pow);
    }
}

#ifdef BUILD_EU_VERSION
#else
int tecmo_logo_flow = 0;

void InitTecmotLogo()
{
    tecmo_logo_flow = 0;
}

int SetTecmoLogo()
{
    SPRT_DAT logotex[1] = {{
        // .tex0 = 0x2004480669329A40,
        .tex0 = SCE_GS_SET_TEX0(6720, 10, 19, 10, 9, 1, 0, 8768, 0, 0, 0, 1),
        .u = 0,
        .v = 0,
        .w = 640,
        .h = 448,
        .x = 0,
        .y = 0,
        .pri = 16,
        .alpha = 128,
    }};
    DISP_SPRT ds;
    static int cnt;
    u_char alp;
    int sec1;
    int sec2;
    int sec3;
    
    sec1 = 90;
    sec2 = 60;
    sec3 = 90;
    
    switch (tecmo_logo_flow)
    {
    case 0:
        cnt = 0;
        tecmo_logo_flow = 1;
    case 1:
        alp = cnt * 128 / sec1;
        cnt++;
        if (cnt >= sec1)
        {
            cnt = 0;
            tecmo_logo_flow++;
        }
    break;
    case 2:
        alp = 128;
        cnt++;
        if (cnt >= sec2)
        {
            cnt = 0;
            tecmo_logo_flow = 3;
        }
    break;
    case 3:
        alp = (sec3 - cnt) * 128 / sec3;
        cnt++;
        if (cnt >= sec3)
        {
            cnt = 0;
            tecmo_logo_flow = 0xff;
        }
    break;
    case 0xff:
        return 1;
    break;
    }
    
    SetSprFile3(0x1e90000, 0);
    CopySprDToSpr(&ds, logotex);
    
    ds.zbuf = 0x10100008c;
    ds.test = 0x5000d;
    ds.pri = logotex[0].pri;
    ds.z = 0x0fffffff - logotex[0].pri;
    ds.x = logotex[0].x;
    ds.y = logotex[0].y;
    ds.r = 96;
    ds.g = 96;
    ds.b = 96;
    ds.alpha = alp;
    
    DispSprD(&ds);
    
    return 0;
}
#endif

void set_vect(float *v, float x, float y, float z, float w)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
    v[3] = w;
}

void Vu0SubOuterProduct(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, sceVu0FVECTOR v3 )
{
    register u_int reg0 = 0;
    
	asm __volatile__(
    	"lqc2    vf4,0x0(%1)\n"
    	"lqc2    vf5,0x0(%2)\n"
    	"lqc2    vf7,0x0(%3)\n"
    	"vsub.xyz	vf5,vf5,vf4\n"
    	"vsub.xyz	vf4,vf7,vf4\n"
    	"vopmula.xyz	ACC,vf5,vf4\n"
    	"vopmsub.xyz	vf6,vf4,vf5\n"
    	"vsub.w vf6,vf6,vf6\n"
    	"sqc2    vf6,0x0(%0)\n"
    	: :"r"(v0),"r"(v1),"r"(v2),"r"(v3),"r"(reg0):"memory"
    );
}

void Vu0Normalize(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
    asm __volatile__(
        "lqc2    vf4,0x0(%1)\n"
        "vmul.xyz vf5,vf4,vf4\n"
        "vaddy.x vf5,vf5,vf5\n"
        "vaddz.x vf5,vf5,vf5\n"
        "vsqrt Q,vf5x\n"
        "vwaitq\n"
        "vaddq.x vf5x,vf0x,Q\n"
        "vdiv    Q,vf0w,vf5x\n"
        "vsub.xyzw vf7,vf0,vf0\n"
        "vwaitq\n"
        "vmulq.xyz  vf7,vf4,Q\n"
        "sqc2    vf7,0x0(%0)\n"
        : :"r"(v0),"r"(v1):"memory"
    );
}

void Vu0ApplyMatrix(sceVu0FVECTOR v0, sceVu0FMATRIX m0, sceVu0FVECTOR v1)
{
    asm __volatile__(
    	"lqc2    vf4,0x0(%1)\n"
    	"lqc2    vf5,0x10(%1)\n"
    	"lqc2    vf6,0x20(%1)\n"
    	"lqc2    vf7,0x30(%1)\n"
    	"lqc2    vf8,0x0(%2)\n"
    	"vmulax.xyzw	ACC,   vf4,vf8\n"
    	"vmadday.xyzw	ACC,   vf5,vf8\n"
    	"vmaddaz.xyzw	ACC,   vf6,vf8\n"
    	"vmaddw.xyzw	vf12,vf7,vf8\n"
    	"sqc2    vf12,0x0(%0)\n"
    	: :"r"(v0),"r"(m0),"r"(v1):"memory"
    );
}

void Vu0MulVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
{
    asm __volatile__(
    	"lqc2    vf4,0x0(%1)\n"
    	"lqc2    vf8,0x0(%2)\n"
    	"vmul.xyzw	vf12,vf4,vf8\n"
    	"sqc2    vf12,0x0(%0)\n"
    	: :"r"(v0),"r"(v1),"r"(v2):"memory"
    );
}

void Vu0FTOI0Vector(sceVu0IVECTOR v0, sceVu0FVECTOR v1)
{
    asm __volatile__(
    	"lqc2    vf4,0(%1)\n"
    	"vftoi0.xyzw	vf6,vf4\n"
    	"sqc2    vf6,0(%0)\n"
    	: :"r"(v0),"r"(v1):"memory"
    );
}
