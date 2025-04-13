#include "common.h"
#include "typedefs.h"
#include "message.h"

#include "gcc/stdarg.h"

#include "ee/eestruct.h"

#include "main/glob.h"
#include "mc/mc_main.h"
#include "ingame/plyr/plyr_ctl.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_sub.h"
#ifdef BUILD_EU_VERSION
#include "graphics/graph2d/subtitles.h"
#endif
#include "graphics/graph3d/sglib.h"

#include "data/ascii_font_tbl.h"  // u_char ascii_font_tbl[];
#include "data/ascii_font_tbl2.h" // u_char ascii_font_tbl2[];
#include "data/wbyte_font_tbl0.h" // int wbyte_font_tbl0[];
#include "data/wbyte_font_tbl1.h" // int wbyte_font_tbl1[];
#include "data/wbyte_font_tbl2.h" // int wbyte_font_tbl2[];
#include "data/wbyte_font_tbl3.h" // int wbyte_font_tbl3[];
#include "data/wbyte_font_tbl4.h" // int wbyte_font_tbl4[];
#include "data/wbyte_font_tbl5.h" // int wbyte_font_tbl5[];
#include "data/font_w_b0.h"       // static int font_w_b0[];
#ifdef BUILD_EU_VERSION
#include "data/font_w_b0_e.h"     // static int font_w_b0_e[];
#endif
#include "data/font_w_b1.h"       // static int font_w_b1[];
#include "data/font_w_b2.h"       // static int font_w_b2[];
#include "data/font_w_b3.h"       // static int font_w_b3[];
#include "data/font_w_b4.h"       // static int font_w_b4[];

#include "data/fntdat.h"          // SPRT_DAT fntdat[];
#include "data/mesbox.h"          // SPRT_DAT mesbox[];
#include "data/btntex.h"          // SPRT_DAT btntex[];
int *mes_ex_nums[] = { &mc_ctrl.port_mes };

static MES_DAT msdat;
static MESV22 mesv22;

static void SetMesFrame2(int pri, float bx, float by, float sw, float sh);
static void SetFontPacketHeader(int n, int type, u_char alp);
static void SetFont(int pri, int type, int no, int x, int y, u_char r, u_char g, u_char b, u_char a);
static void FontInit();
#ifdef BUILD_EU_VERSION
static void SetFontTex(int path, int flg, int bank);
static void SetFontEnv(int path);
static void SetFontPat(int path, int pri, int fn, int x, int y, int fw, u_char r, u_char g, u_char b, u_char a);
#else
static void SetFontTex(int flg, int bank);
static void SetFontEnv();
static void SetFontPat(int pri, int fn, int x, int y, int fw, u_char r, u_char g, u_char b, u_char a);
static void SetUnderLine(int sw, int pri, int x, int y, int fw, u_char r, u_char g, u_char b, u_char a);
#endif
static void MesPacketEnd();
static void PacketEnd();
static void MesKeyCheck();

#define PI 3.1415927f

#define SCR_WIDTH 640
#define SCR_HEIGHT 224

#define SCR_WIDTH_HALF (int)(SCR_WIDTH / 2)

void InitMessage()
{
    FontInit();
    save_mes_addr = 0;
    mesv22 = (MESV22){0};
}

void InitMessageEF()
{
    if (mesv22.pass != 0)
    {
        mesv22.pass = 0;
        mesv22.pass_flg = 1;
    }
    else
    {
        mesv22.pass_flg = 0;
    }

#ifdef BUILD_EU_VERSION
    if (msdat.bx_pass_st == 1 && msdat.bx_pass == 0)
    {
        msdat.bx_pass_st = 0;
    }

    msdat.bx_pass_old = msdat.bx_pass;
    msdat.bx_pass = 0;
#endif
}

void SetMessageDirect(int addr, int pri, int fn, int x, int y, u_char r, u_char g, u_char b)
{
    static DISP_STR ds;
    STR_DAT sd = {
        .str = NULL,
        .pos_x = 0,
        .pos_y = 0,
        .type = 1,
        .r = 0xFF,
        .g = 0xFF,
        .b = 0xFF,
        .alpha = 0x80,
        .pri = 0,
    };

    CopyStrDToStr(&ds, &sd);

    ds.pri = (u_char)pri * 4096;
    ds.str = (u_char *)addr;
    ds.pos_x = x;
    ds.pos_y = y;
    ds.r = r;
    ds.g = g;
    ds.b = b;

    SetMessageV2(&ds);
}

void SetMessageAddr(int addr)
{
    save_mes_addr = (u_char *)addr;
    SetMessageV3(save_mes_addr, 0x64000);
}

int SetMessage()
{
    SetMessageV3(save_mes_addr, 0x64000);
    return MesStatusCheck();
}
void MessageWaitOff()
{
    return;
}

static void SetMesFrame2(int pri, float bx, float by, float sw, float sh)
{
    float xx;
    float yy;

    xx = bx - 320.0f;
    yy = by - 224.0f;

    SetSquareS(pri, xx       , yy       , xx + sw + 14.0f, yy + sh + 35.0f, 0x00, 0x00, 0x00, 0x80);
    SetSquareS(pri, xx + 1.0f, yy + 1.0f, xx + sw + 13.0f, yy + sh + 33.0f, 0x40, 0x40, 0x40, 0x80);
    SetSquareS(pri, xx + 3.0f, yy + 3.0f, xx + sw + 11.0f, yy + sh + 31.0f, 0x80, 0x80, 0x80, 0x80);
    SetSquareS(pri, xx + 5.0f, yy + 5.0f, xx + sw +  9.0f, yy + sh + 29.0f, 0x40, 0x40, 0x40, 0x80);
    SetSquareS(pri, xx + 7.0f, yy + 7.0f, xx + sw +  7.0f, yy + sh + 27.0f, 0x00, 0x00, 0x00, 0x80);

    SetSquareS(pri, xx                    , yy                    , xx +       4.0f, yy +       4.0f, 0x00, 0x00, 0x00, 0x80);
    SetSquareS(pri, xx                    , yy + sh + 35.0f - 4.0f, xx +       4.0f, yy + sh + 35.0f, 0x00, 0x00, 0x00, 0x80);
    SetSquareS(pri, xx + sw + 14.0f - 4.0f, yy                    , xx + sw + 14.0f, yy +       4.0f, 0x00, 0x00, 0x00, 0x80);
    SetSquareS(pri, xx + sw + 14.0f - 4.0f, yy + sh + 35.0f - 4.0f, xx + sw + 14.0f, yy + sh + 35.0f, 0x00, 0x00, 0x00, 0x80);
}

void SetMesFrame(int pri, int type, float bx, float by, float sw, float sh)
{
    switch (type)
    {
        case 0:
            // do nothing
        break;
        case 1:
            SetMesFrame2(pri,bx,by,sw,sh);
        break;
    }
}

void SetInteger(float x, float y, int num)
{
    SetInteger2(0, x, y, 0, 0x80, 0x80, 0x80, num);
}

void SetInteger2(int pri, float x, float y, int type, u_char r, u_char g, u_char b, int num)
{
    char cwo[16];

    sprintf(cwo, "%5d", num);

    SetASCIIString2(pri, x, y, type, r, g, b, cwo);
}

void SetInteger3(int pri, float x, float y, int type, u_char r, u_char g, u_char b, u_char a, int num)
{
    char cwo[16];

    sprintf(cwo, "%d", num);

    SetASCIIString3(pri, x, y, type, r, g, b, a, cwo);
}

void SetASCIIString(float x, float y, char *str)
{
    SetASCIIString3(0, x, y, 0, 0x80, 0x80, 0x80, 0x80, str);
}

void SetASCIIString2(int pri, float x, float y, int type, u_char r, u_char g, u_char b, char *str)
{
    SetASCIIString3(pri, x, y, type, r, g, b, 0x80, str);
}

void SetASCIIString3(int pri, float x, float y, int type, u_char r, u_char g, u_char b, u_char a, char *str)
{
    int i;
    int n;
    int code;
    u_char c1;
    u_char c2;
    int w[2] = {24, 12};
    u_char *tbl;
    int ft;
    int offset;
    int xoff;

    i = 0;
    n = 0;
    xoff = 0;

    if (type & 1)
    {
        ft = 0;
    }
    else
    {
        ft = 1;
    }

    draw_mpri[nmdpri][0] = pri > 0 ? pri : 0x1000;
    draw_mpri[nmdpri][1] = nmdpkt;
    nmdpri++;

    while (str[i] != NULL)
    {
        c2 = str[i+1];

        if (c2 == 0xde || c2 == 0xdf)
        {
            i++;
        }

        i++;
        n++;
    }

    SetFontPacketHeader(n, ft, 0x7f);

    i = 0;

    while (str[i] != NULL)
    {
        c1 = str[i];
        c2 = str[i+1];

        if (c2 == 0xde)
        {
            if (c1 >= 0xb3 && c1 <= 0xce)
            {
                code = c1 + 11;
            }
            else
            {
                code = c1 - 32;
            }

            i++;
        }
        else if (c2 == 0xdf)
        {

            if (c1 >= 0xca && c1 <= 0xce)
            {
                code =  c1 + 16;
            }
            else
            {
                code = c1 - 32;
            }

            i++;
        }
        else
        {
            if (c1 >= 0x20 && c1 <= 0xdd)
            {
                code = c1 - 32;
            }
            else
            {
                code = 0;
            }
        }

        tbl = type & 1 ? ascii_font_tbl2 : ascii_font_tbl;

        offset = (type & 2) == 0 ? 0 : 10;

        SetFont(pri, type, tbl[code] + offset, x + xoff, y, r, g, b, a);

#if BUILD_EU_VERSION
        if (sys_wrk.language == 0)
        {
            if ((type & 1) == 0)
            {
                xoff += font_w_b0_e[tbl[code] + offset];
            }
            else
            {
                xoff += 12;
            }
        }
        else
        {
            if ((type & 1) == 0)
            {
                xoff += font_w_b0[tbl[code] + offset];
            }
            else
            {
                xoff += 12;
            }
        }
#else
        if ((type & 1) == 0)
        {
            xoff += font_w_b0[tbl[code] + offset];
        }
        else
        {
            xoff += 12;
        }
#endif

        i++;
    }
}

void SetWString2(int pri, float x, float y, u_char r, u_char g, u_char b, char *str)
{
    int i = 0;
    int n = 0;
    int code;
    int dc;
    int type;
    int font_code[2][16] = {
        {
            33089, 33096, 33359, 33375, 33376, 33401, 33409, 33434,
            33439, 33521, 33600, 33684, 33115, 33118, 33129, 33130,
        },
        {
            41378, 41385, 41904, 41913, 41921, 41946, 41953, 41978,
            42145, 42227, 42401, 42484, 41404, 41407, 41418, 41419
        },
    };

    draw_mpri[nmdpri][0] = pri;
    draw_mpri[nmdpri++][1] = nmdpkt;

    SetFontPacketHeader(strlen(str) * 0.5, 1, 0x7f);

    while(str[i] != 0)
    {
        dc = (u_char)str[i] << 8 | (u_char)str[i+1];

        if ((dc & 0xf000) == 0x8000)
        {
            type = 0;
        }
        else if ((dc & 0xf000) == (0x8000 | 0x2000))
        {
            type = 1;
        }
        else
        {
            type = 0;
        }

        if (font_code[type][0] <= dc && font_code[type][1] >= dc)
        {
            code = wbyte_font_tbl0[dc - font_code[type][0]];
        }

        if (font_code[type][2] <= dc && font_code[type][3] >= dc)
        {
            code = wbyte_font_tbl1[dc - font_code[type][2]];
        }

        if (font_code[type][4] <= dc && font_code[type][5] >= dc)
        {
            code = wbyte_font_tbl2[dc - font_code[type][4]];
        }

        if (font_code[type][6] <= dc && font_code[type][7] >= dc)
        {
            code = wbyte_font_tbl3[dc - font_code[type][6]];
        }

        if (font_code[type][8] <= dc && font_code[type][9] >= dc)
        {
            code = wbyte_font_tbl4[dc - font_code[type][8]];
        }

        if (font_code[type][10] <= dc && font_code[type][11] >= dc)
        {
            code = wbyte_font_tbl5[dc - font_code[type][10]];
        }

        if (dc == font_code[type][12])
        {
            code = 0xe2;
        }

        if (dc == font_code[type][13])
        {
            code = 0xe4;
        }

        if (dc == font_code[type][14])
        {
            code = 0xe0;
        }

        if (dc == font_code[type][15])
        {
            code = 0xe1;
        }

        SetFont(pri, 0, code, x + n * 24, y, r, g, b, 0x80);

        i += 2;
        n += 1;
    }
}

void SetString(float x, float y, char *fmt, ...)
{
    va_list ap;
    int i;
    int n;
    int flag;
    int len;
    char cw;
    char cwo[256];
    char str[256];
    char *buf;

    va_start(ap, fmt);

    buf = str;
    str[0] = '\0';

    while (*fmt != '\0')
    {
        if (*fmt == '%')
        {
            fmt++;
            cw = *fmt;
            n = -1;
            flag = 0;

            while (!flag)
            {
                switch (cw)
                {
                case 's':
                    sprintf(cwo, "%s", va_arg(ap, char *));
                    len = strlen(cwo);
                    if (len < n)
                    {
                        for (i = 0; i < n - len; i++)
                        {
                            *buf++= '+';
                        }
                    }
                    for (i = 0; i < strlen(cwo); i++)
                    {
                        *buf++ = cwo[i];
                    }
                    fmt++;
                    flag = 1;
                break;
                case 'd':
                    sprintf(cwo, "%d", va_arg(ap, int));
                    len = strlen(cwo);
                    if (len < n)
                    {
                        for (i = 0; i < n - len; i++)
                        {
                            *buf++ = '+';
                        }
                    }
                    for (i = 0; i < strlen(cwo); i++)
                    {
                        *buf++ = cwo[i];
                    }
                    fmt++;
                    flag = 1;
                break;
                case 'c':
                    sprintf(cwo, "%c", va_arg(ap, char));
                    len = strlen(cwo);
                    if (len < n)
                    {
                        for (i = 0; i < n - len; i++)
                        {
                            *buf++= '+';
                        }
                    }
                    for (i = 0; i < strlen(cwo); i++)
                    {
                        *buf++ = cwo[i];
                    }
                    fmt++;
                    flag = 1;
                break;
                case '%':
                    *buf++ = '%';
                    fmt++;
                    flag = 1;
                break;
                default:
                    // 0x30-0x39 are the ASCII values for 0-9
                    fmt++;
                    len = (cw - 0x30);
                    if ((len & 0xff) < 10U)
                    {
                        cw = *fmt;
                        n = len;
                        while (cw - 0x30U < 10)
                        {
                            fmt++;
                            n = n * 10 + cw - 0x30;
                            cw = *fmt;
                        }
                    }
                break;
                }
            }
        }
        else
        {
            *buf++ = *fmt++;
        }
    }

    va_end(ap);

    *buf = '\0';
    SetASCIIString(x, y, str);
}

void SetString2(int pri, float x, float y, int type, u_char r, u_char g, u_char b, char *fmt, ...)
{
    va_list ap;
    int i;
    int n;
    int flag;
    int len;
    char cw;
    char cwo[256];
    char str[256];
    char *buf;

    va_start(ap, fmt);

    buf = str;
    str[0] = '\0';

    while (*fmt != '\0')
    {
        if (*fmt == '%')
        {
            fmt++;
            cw = *fmt;
            n = -1;
            flag = 0;

            while (!flag)
            {
                switch (cw)
                {
                case 's':
                    sprintf(cwo, "%s", va_arg(ap, char *));
                    len = strlen(cwo);
                    if (len < n)
                    {
                        for (i = 0; i < n - len; i++)
                        {
                            *buf++= ' ';
                        }
                    }
                    for (i = 0; i < strlen(cwo); i++)
                    {
                        *buf++ = cwo[i];
                    }
                    fmt++;
                    flag = 1;
                break;
                case 'd':
                    sprintf(cwo, "%d", va_arg(ap, int));
                    len = strlen(cwo);
                    if (len < n)
                    {
                        for (i = 0; i < n - len; i++)
                        {
                            *buf++ = ' ';
                        }
                    }
                    for (i = 0; i < strlen(cwo); i++)
                    {
                        *buf++ = cwo[i];
                    }
                    fmt++;
                    flag = 1;
                break;
                case 'c':
                    sprintf(cwo, "%c", va_arg(ap, char));
                    len = strlen(cwo);
                    if (len < n)
                    {
                        for (i = 0; i < n - len; i++)
                        {
                            *buf++= ' ';
                        }
                    }
                    for (i = 0; i < strlen(cwo); i++)
                    {
                        *buf++ = cwo[i];
                    }
                    fmt++;
                    flag = 1;
                break;
                case '%':
                    *buf++ = '%';
                    fmt++;
                    flag = 1;
                break;
                default:
                    // 0x30-0x39 are the ASCII values for 0-9
                    fmt++;
                    len = (cw - 0x30);
                    if ((len & 0xff) < 10U)
                    {
                        cw = *fmt;
                        n = len;
                        while (cw - 0x30U < 10)
                        {
                            fmt++;
                            n = n * 10 + cw - 0x30;
                            cw = *fmt;
                        }
                    }
                break;
                }
            }
        }
        else
        {
            *buf++ = *fmt++;
        }
    }

    va_end(ap);

    *buf = '\0';
    SetASCIIString2(pri, x, y, type, r, g, b, str);
}

static void SetFontPacketHeader(int n, int type, u_char alp)
{
    sceGsTex0 Load;
    sceGsTex0 Change;
    int psm;

    psm = ((sceGsTex0 *)&fntdat[type].tex0)->PSM;
    if (psm == 20)
    {
        Load = *(sceGsTex0 *)&fntdat[type].tex0;
        Load.PSM = SCE_GS_PSMT8;
        Load.CSA = 0;
        Load.CLD = 1;

        Change = *(sceGsTex0 *)&fntdat[type].tex0;
        Change.CSA = 0;
        Change.CLD = 0;
    }

    mpbuf[nmdpkt].ul128 = (u_long128)0;
    mpbuf[nmdpkt++].ui32[0] = (DMAend | 10) + n * 5;

    mpbuf[nmdpkt].ul64[0] = SCE_GIF_SET_TAG(8, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    mpbuf[nmdpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    mpbuf[nmdpkt].ul64[0] = 0;
    mpbuf[nmdpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    if (psm == 20)
    {
        mpbuf[nmdpkt].ul64[0] = *(long *)&Load;
        mpbuf[nmdpkt++].ul64[1] = SCE_GS_TEX0_1;

        mpbuf[nmdpkt].ul64[0] = *(long *)&Change;
        mpbuf[nmdpkt++].ul64[1] = SCE_GS_TEX0_1;
    }
    else
    {
        mpbuf[nmdpkt].ul64[0] = fntdat[type].tex0;
        mpbuf[nmdpkt++].ul64[1] = SCE_GS_TEX0_1;

        mpbuf[nmdpkt].ul64[0] = 0;
        mpbuf[nmdpkt++].ul64[1] = SCE_GS_NOP;
    }

    mpbuf[nmdpkt].ul64[0] = SCE_GS_SET_TEX1_1(0, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_NEAREST, 0, 0, 0);
    mpbuf[nmdpkt++].ul64[1] = SCE_GS_TEX1_1;

    mpbuf[nmdpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, alp);
    mpbuf[nmdpkt++].ul64[1] = SCE_GS_ALPHA_1;

    mpbuf[nmdpkt].ul64[0] = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0);
    mpbuf[nmdpkt++].ul64[1] = SCE_GS_CLAMP_1;

    mpbuf[nmdpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    mpbuf[nmdpkt++].ul64[1] = SCE_GS_TEST_1;

    mpbuf[nmdpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
    mpbuf[nmdpkt++].ul64[1] = SCE_GS_ZBUF_1;

    mpbuf[nmdpkt].ul64[0] = SCE_GIF_SET_TAG(n, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 5);
    mpbuf[nmdpkt++].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0)
        | SCE_GS_UV    << (4 * 1)
        | SCE_GS_XYZF2 << (4 * 2)
        | SCE_GS_UV    << (4 * 3)
        | SCE_GS_XYZF2 << (4 * 4);
}

static void SetFont(int pri, int type, int no, int x, int y, u_char r, u_char g, u_char b, u_char a)
{
    unsigned int tw1;
    unsigned int th1;
    unsigned int tw2;
    unsigned int th2;
    int px2;
    int py2;
    int Font_W;
    int Font_H;
    int Num_W;
    int div;
    int off_w;
    int off_ht;
    int off_hd;
    int dx;
    int dy;
    int dw;
    int dh;
    int z;

    if (type & 1)
    {
        Font_W = 12;
        Font_H = 14;
        Num_W = 42;
        off_w = 8;
        off_ht = 1;
        off_hd = -1;
    }
    else
    {
        Font_W = 24;
        Font_H = 24;
        Num_W = 21;
        off_w = 0;
        off_ht = 0;
        off_hd = 0;
    }

    div = g_bInterlace ? 2 : 1;

    z = pri > 0 ? 0xfffffff - pri : 0xfffefff;

    dx = x + 1728; dx *= 16;
    dw = Font_W; dw *= 16;
    dy = y / div + 2048; dy -= 448 / (div * 2); dy *= 16;
    dh = Font_H / div + 1; dh *= 16;

    px2 = dx + dw;
    py2 = dy + dh;

    tw1 = ((no % Num_W) * Font_W + off_w); tw1 *= 16;
    th1 = ((no / Num_W) * Font_H + off_ht); th1 *= 16;
    tw2 = ((no % Num_W) * Font_W + Font_W + off_w); tw2 *= 16;
    th2 = ((no / Num_W) * Font_H + Font_H + off_hd); th2 *= 16;

    mpbuf[nmdpkt].ui32[0] = r;
    mpbuf[nmdpkt].ui32[1] = g;
    mpbuf[nmdpkt].ui32[2] = b;
    mpbuf[nmdpkt++].ui32[3] = a;

    mpbuf[nmdpkt].ui32[0] = tw1;
    mpbuf[nmdpkt].ui32[1] = th1;
    mpbuf[nmdpkt].ui32[2] = 0;
    mpbuf[nmdpkt++].ui32[3] = 0;

    mpbuf[nmdpkt].ui32[0] = dx;
    mpbuf[nmdpkt].ui32[1] = dy;
    mpbuf[nmdpkt].ui32[2] = z;
    mpbuf[nmdpkt++].ui32[3] = 0;

    mpbuf[nmdpkt].ui32[0] = tw2;
    mpbuf[nmdpkt].ui32[1] = th2;
    mpbuf[nmdpkt].ui32[2] = 0;
    mpbuf[nmdpkt++].ui32[3] = 0;

    mpbuf[nmdpkt].ui32[0] = px2;
    mpbuf[nmdpkt].ui32[1] = py2;
    mpbuf[nmdpkt].ui32[2] = z;
    mpbuf[nmdpkt++].ui32[3] = 0;
}

static void FontInit()
{
    msdat.pass = 0;

    msdat.bx = 0;
    msdat.by = 0;
    msdat.pri = 0;
    msdat.disptype = 0;

    msdat.stp = NULL;
    msdat.sta = 0x40;
    msdat.retst = 0;

    msdat.flg = 1;
    msdat.csr = 0;
    msdat.decide = 0;
    msdat.mes_is_end = 2;
    msdat.cnt = 0;

    msdat.r = 0xff;
    msdat.g = 0xff;
    msdat.b = 0xff;
    msdat.alp = 0x0;

    msdat.fntmcnt = 0;
    msdat.fntwait = 0;
    msdat.fntcnt = 0;
    msdat.selnum = 0;

#ifdef BUILD_EU_VERSION
    msdat.bx_pass = 0;
    msdat.bx_pass_old = 0;
    msdat.bx_pass_st = 0;
#endif
}

#ifdef BUILD_EU_VERSION
static void SetFontTex(int path, int flg, int bank)
#else
static void SetFontTex(int flg, int bank)
#endif
{
    static int obank = -1;
    sceGsTex0 tex0;

    if (flg != 0 && bank == obank)
    {
        return;
    }

    obank = bank;

    tex0 = *(sceGsTex0 *)&fntdat[bank].tex0;

#ifdef BUILD_EU_VERSION
    if (path == 1)
    {
        mpbuf[nmdpkt].ul128 = (u_long128)0;
        mpbuf[nmdpkt++].ui32[0] = DMAcnt | 4;
    }
#endif

    mpbuf[nmdpkt].ul64[0] = SCE_GIF_SET_TAG(3, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    mpbuf[nmdpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    mpbuf[nmdpkt].ul64[0] = 0;
    mpbuf[nmdpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    if (tex0.PSM == 20)
    {
        sceGsTex0 _tex0 = tex0;
        _tex0.PSM = SCE_GS_PSMT8;
        _tex0.CSA = 0;
        _tex0.CLD = 1;
        mpbuf[nmdpkt].ul64[0] = *(u_long *)&_tex0;
        mpbuf[nmdpkt++].ul64[1] = SCE_GS_TEX0_1;

        _tex0 = tex0;
        _tex0.CSA = 0;
        _tex0.CLD = 0;
        mpbuf[nmdpkt].ul64[0] = *(u_long *)&_tex0;
        mpbuf[nmdpkt++].ul64[1] = SCE_GS_TEX0_1;
    }
    else
    {
        mpbuf[nmdpkt].ul64[0] = *(u_long *)&tex0;
        mpbuf[nmdpkt++].ul64[1] = SCE_GS_TEX0_1;

        mpbuf[nmdpkt].ul64[0] = 0;
        mpbuf[nmdpkt++].ul64[1] = SCE_GS_NOP;
    }
}

#ifdef BUILD_EU_VERSION
static void SetFontEnv(int path)
#else
static void SetFontEnv()
#endif
{
#ifdef BUILD_EU_VERSION
    if (path == 1)
    {
        mpbuf[nmdpkt].ul128 = (u_long128)0;
        mpbuf[nmdpkt++].ui32[0] = DMAcnt | 6;
    }
#endif

    mpbuf[nmdpkt].ul64[0] = SCE_GIF_SET_TAG(5, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    mpbuf[nmdpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    mpbuf[nmdpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    mpbuf[nmdpkt++].ul64[1] = SCE_GS_TEX1_1;

    mpbuf[nmdpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x80);
    mpbuf[nmdpkt++].ul64[1] = SCE_GS_ALPHA_1;

    mpbuf[nmdpkt].ul64[0] = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0);
    mpbuf[nmdpkt++].ul64[1] = SCE_GS_CLAMP_1;

    mpbuf[nmdpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    mpbuf[nmdpkt++].ul64[1] = SCE_GS_TEST_1;

#ifdef BUILD_EU_VERSION
    if (path == 1)
    {
        mpbuf[nmdpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    }
    else
    {
        mpbuf[nmdpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
    }
#else
    mpbuf[nmdpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
#endif
    mpbuf[nmdpkt++].ul64[1] = SCE_GS_ZBUF_1;
}

#ifdef BUILD_EU_VERSION
static void SetFontPat(int path, int pri, int fn, int x, int y, int fw, u_char r, u_char g, u_char b, u_char a)
#else
static void SetFontPat(int pri, int fn, int x, int y, int fw, u_char r, u_char g, u_char b, u_char a)
#endif
{
    int px2;
    int py2;
    u_int tw1;
    u_int th1;
    u_int tw2;
    u_int th2;
    u_int Font_W;
    u_int Num_W;
    int div;
    int dx;
    int dy;
    int dw;
    int dh;
    u_int z;

    Font_W = 24;
    Num_W = 21;

    div = g_bInterlace ? 2 : 1;

    z = 0x0fffffff - pri;

    dx = (x + 0x6c0) * 16;
    dy = (((y - 0xe0) / div) + 0x800) * 16;
    dw = fw * 16;
    dh = (Font_W / div) * 16;

    px2 = dx + dw;
    py2 = dy + dh;

    tw1 = ((fn % Num_W) * Font_W) * 16;
    th1 = (((fn / Num_W) * Font_W)) * 16;
    tw2 = (((fn % Num_W) * Font_W) + fw) * 16;
    th2 = (((fn / Num_W) * Font_W)) * 16 + (Font_W * 16);

#ifdef BUILD_EU_VERSION
    if (path == 1)
    {
        mpbuf[nmdpkt].ul128 = (u_long128)0;
        mpbuf[nmdpkt++].ui32[0] = DMAcnt | 6;
    }
#endif

    mpbuf[nmdpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 342, SCE_GIF_PACKED, 5);
    mpbuf[nmdpkt++].ul64[1] = 0 \
            | SCE_GS_RGBAQ << (4 * 0)
            | SCE_GS_UV    << (4 * 1)
            | SCE_GS_XYZF2 << (4 * 2)
            | SCE_GS_UV    << (4 * 3)
            | SCE_GS_XYZF2 << (4 * 4);

    mpbuf[nmdpkt].ui32[0] = r;
    mpbuf[nmdpkt].ui32[1] = g;
    mpbuf[nmdpkt].ui32[2] = b;
    mpbuf[nmdpkt++].ui32[3] = a;

    mpbuf[nmdpkt].ui32[0] = tw1 + 8;
    mpbuf[nmdpkt].ui32[1] = th1 + 8;
    mpbuf[nmdpkt].ui32[2] = 0;
    mpbuf[nmdpkt++].ui32[3] = 0;

    mpbuf[nmdpkt].ui32[0] = dx + 8;
    mpbuf[nmdpkt].ui32[1] = dy + 8;
    mpbuf[nmdpkt].ui32[2] = z;
    mpbuf[nmdpkt++].ui32[3] = 0;

    mpbuf[nmdpkt].ui32[0] = tw2 - 8;
    mpbuf[nmdpkt].ui32[1] = th2 - 8;
    mpbuf[nmdpkt].ui32[2] = 0;
    mpbuf[nmdpkt++].ui32[3] = 0;

    mpbuf[nmdpkt].ui32[0] = px2 - 8;
    mpbuf[nmdpkt].ui32[1] = py2 - 8;
    mpbuf[nmdpkt].ui32[2] = z;
    mpbuf[nmdpkt++].ui32[3] = 0;
}

static void SetUnderLine(int sw, int pri, int x, int y, int fw, u_char r, u_char g, u_char b, u_char a)
{
    int px2;
    int py2;
    int div;
    int dx;
    int dy;
    int dw;
    int dh;
    u_int z;

    if (sw != 0)
    {
        div = g_bInterlace ? 2 : 1;

        dx = x + 0x6c0;
        dh = (y - 0xe0) / div + 0x800;
        dy = 24 / div + dh;
        dw = fw * 16;
        px2 = dx * 16 + dw;
        py2 = dy * 16 - 8;

        z = 0x0fffffff - pri;

        mpbuf[nmdpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 65, SCE_GIF_PACKED, 3);
        mpbuf[nmdpkt++].ul64[1] = 0 \
            | SCE_GS_RGBAQ << (4 * 0)
            | SCE_GS_XYZF2 << (4 * 1)
            | SCE_GS_XYZF2 << (4 * 2);

        mpbuf[nmdpkt].ui32[0] = r;
        mpbuf[nmdpkt].ui32[1] = g;
        mpbuf[nmdpkt].ui32[2] = b;
        mpbuf[nmdpkt++].ui32[3] = a;

        mpbuf[nmdpkt].ui32[0] = dx * 16;
        mpbuf[nmdpkt].ui32[1] = dy * 16 - 8;
        mpbuf[nmdpkt].ui32[2] = z;
        mpbuf[nmdpkt++].ui32[3] = 0;

        mpbuf[nmdpkt].ui32[0] = px2;
        mpbuf[nmdpkt].ui32[1] = py2;
        mpbuf[nmdpkt].ui32[2] = z;
        mpbuf[nmdpkt++].ui32[3] = 0;
    }
}

#ifdef BUILD_EU_VERSION
static void MesPacketEnd()
#else
static void PacketEnd()
#endif
{
    mpbuf[nmdpkt].ul128 = (u_long128)0;
    mpbuf[nmdpkt++].ui32[0] = DMAend | 2;

    mpbuf[nmdpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    mpbuf[nmdpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    mpbuf[nmdpkt].ul64[0] = 0;
    mpbuf[nmdpkt++].ul64[1] = SCE_GS_NOP;
}

void CopyStrDToStr(DISP_STR *s, STR_DAT *d)
{
    s->str = d->str;
    s->pos_x = d->pos_x;
    s->pos_y = d->pos_y;
    s->type = d->type;
    s->r = d->r;
    s->g = d->g;
    s->b = d->b;
    s->alpha = d->alpha;
    s->pri = d->pri;
    s->x_wide = 0;
    s->y_wide = 0;
    s->brnch_num = 0;
    s->csr = 0;
    s->st = 0;
}

int SetMessageV2_2(DISP_STR *s)
{
    u_char *c;
    u_char *cp;
    int i;
    int n;
    int m;
    int loop;
    int sflg;
    int npri;
    int nx;
    int ny;
    int nxw;
    int nyw;
    int px[32];
    int py[32];
    int pw[32];
    int selnum;
    u_char nr;
    u_char ng;
    u_char nb;
    u_char na;
    u_char nfn;
    u_char nfw;
    u_char nfh;
    char cwo[256];
    int line;
    int column;
    SQAR_DAT sq;
    DISP_SQAR dq;

    c = s->str;
    nxw = s->x_wide;
    nyw = s->y_wide;
    nr = s->r;
    ng = s->g;
    nb = s->b;
    na = s->alpha;
    npri = s->pri;
    nx = s->pos_x;
    ny = s->pos_y;

    selnum = 0;

    draw_mpri[nmdpri][0] = npri;
    draw_mpri[nmdpri][1] = nmdpkt++;

#ifdef BUILD_EU_VERSION
    SetFontTex(0, 0, 1);
    SetFontEnv(0);
#else
    SetFontTex(0, 1);
    SetFontEnv();
#endif

    loop = 1;
    sflg = 0;

    if ((mesv22.pass_flg == 0x0) || (mesv22.old_adr != c))
    {
        mesv22.cnt = 0;
        memset(mesv22.mes_alps, 0, sizeof(mesv22.mes_alps));
    }

    line = 0;
    column = 0;

    mesv22.old_adr = c;

    while (loop)
    {
        switch(*c)
        {
        case 0xf0:
            c++;
            nfn = *c++;
            nfw = font_w_b1[nfn];
#ifdef BUILD_EU_VERSION
            SetFontTex(0, 1, 2);
#else
            SetFontTex(1, 2);
#endif
            if (column <= mesv22.cnt - line)
            {
                mesv22.mes_alps[line][column] = mesv22.mes_alps[line][column] + 4 > 0x80 ? 0x80 : mesv22.mes_alps[line][column] + 4;
            }
#ifdef BUILD_EU_VERSION
            SetFontPat(0, npri, nfn, nx, ny, nfw, nr, ng, nb, mesv22.mes_alps[line][column] * na >> 7);
#else
            SetFontPat(npri, nfn, nx, ny, nfw, nr, ng, nb, mesv22.mes_alps[line][column] * na >> 7);
#endif
            nx += nfw + nxw;
            if (sflg != 0)
            {
                pw[selnum-1] += nfw + nxw;
            }
            column++;
        break;
        case 0xf1:
            c++;
            nfn = *c++;
            nfw = font_w_b2[nfn];
#ifdef BUILD_EU_VERSION
            SetFontTex(0, 1, 3);
#else
            SetFontTex(1, 3);
#endif
            if (column <= mesv22.cnt - line)
            {
                mesv22.mes_alps[line][column] = mesv22.mes_alps[line][column] + 4 > 0x80 ? 0x80 : mesv22.mes_alps[line][column] + 4;
            }
#ifdef BUILD_EU_VERSION
            SetFontPat(0, npri, nfn, nx, ny, nfw, nr, ng, nb, mesv22.mes_alps[line][column] * na >> 7);
#else
            SetFontPat(npri, nfn, nx, ny, nfw, nr, ng, nb,  mesv22.mes_alps[line][column] * na >> 7);
#endif
            nx += nfw + nxw;
            if (sflg != 0)
            {
                pw[selnum-1] += nfw + nxw;
            }
            column++;
        break;
        case 0xf2:
            c++;
            nfn = *c++;
            nfw = font_w_b3[nfn];
#ifdef BUILD_EU_VERSION
            SetFontTex(0, 1, 4);
#else
            SetFontTex(1, 4);
#endif
            if (column <= mesv22.cnt - line)
            {
                mesv22.mes_alps[line][column] = mesv22.mes_alps[line][column] + 4 > 0x80 ? 0x80 : mesv22.mes_alps[line][column] + 4;
            }
#ifdef BUILD_EU_VERSION
            SetFontPat(0, npri, nfn, nx, ny, nfw, nr, ng, nb, mesv22.mes_alps[line][column] * na >> 7);
#else
            SetFontPat(npri, nfn, nx, ny, nfw, nr, ng, nb, mesv22.mes_alps[line][column] * na >> 7);
#endif
            nx += nfw + nxw;
            if (sflg != 0)
            {
                pw[selnum-1] += nfw + nxw;
            }
            column++;
        break;
        case 0xf3:
            c++;
            nfn = *c++;
            nfw = font_w_b4[nfn];
#ifdef BUILD_EU_VERSION
            SetFontTex(0, 1, 5);
#else
            SetFontTex(1, 5);
#endif
            if (column <= mesv22.cnt - line)
            {
                mesv22.mes_alps[line][column] = mesv22.mes_alps[line][column] + 4 > 0x80 ? 0x80 : mesv22.mes_alps[line][column] + 4;
            }
#ifdef BUILD_EU_VERSION
            SetFontPat(0, npri, nfn, nx, ny, nfw, nr, ng, nb, mesv22.mes_alps[line][column] * na >> 7);
#else
            SetFontPat(npri, nfn, nx, ny, nfw, nr, ng, nb, mesv22.mes_alps[line][column] * na >> 7);
#endif
            nx += nfw + nxw;
            if (sflg != 0)
            {
                pw[selnum-1] += nfw + nxw;
            }
            column++;
        break;
        case 0xf7:
            c++;
            nxw = *c++;
        break;
        case 0xf8:
            c++;
            nyw = *c++;
        break;
        case 0xf9:
            s->st |= 0x20;
            cp = c;
            c++;
            {
                u_char c1, c2;
                c1 = *++c;
                c2 = *++c;
                nx = c2 << 8 | c1;
            }
            c++;
            if (s->type != 0)
            {
                if (s->str != cp)
                {
                    nfh = 24;
                    ny += nfh + nyw;
                }
            }
            else
            {
                if ((s->str != cp) && (sflg == 0))
                {
                    nfh = 24;
                    ny += nfh + nyw;
                }
            }
            sflg = 1;
            px[selnum] = nx;
            py[selnum] = ny;
            pw[selnum] = 0;
            selnum++;
        break;
        case 0xfa:
            nx = s->pos_x;
            nfh = 24;
            ny += nyw + nfh;
            c++;
            column = 0;
            line++;
        break;
        case 0xfb:
            c++;
            m = *c++;
            sprintf(cwo, "%d", *mes_ex_nums[*c++]);
            i = 0;
            while (cwo[i] != '\0' && loop != 2)
            {
                nfn = m != 0 ? cwo[i] + 5 :  cwo[i] + 15;
#ifdef BUILD_EU_VERSION
                nfw = sys_wrk.language == 0 ? font_w_b0_e[nfn] : font_w_b0[nfn];
                SetFontTex(0, 1, 1);
#else
                nfw = font_w_b0[nfn];
                SetFontTex(1, 1);
#endif
                if (column <= mesv22.cnt - line)
                {
                    mesv22.mes_alps[line][column] = mesv22.mes_alps[line][column] + 4 > 0x80 ? 0x80 : mesv22.mes_alps[line][column] + 4;
                }
#ifdef BUILD_EU_VERSION
                SetFontPat(0, npri, nfn, nx, ny, nfw, nr, ng, nb, mesv22.mes_alps[line][column] * na >> 7);
#else
                SetFontPat(npri, nfn, nx, ny, nfw, nr, ng, nb, mesv22.mes_alps[line][column] * na >> 7);
#endif
                nx += nfw + nxw;
                if (sflg != 0)
                {
                    pw[selnum-1] += nfw + nxw;
                }
                i++;
            }
        break;
        case 0xfc:
        break;
        case 0xfd:
            nr = *++c;
            ng = *++c;
            nb = *++c;
            c++;
        break;
        case 0xfe:
            nx = s->pos_x;
            nfh = 24;
            ny += nyw + nfh;
            c++;
            column = 0;
            line++;
        break;
        case 0xff:
            loop = 0;
            s->st |= 0x40;
            c++;
            column = 0;
            line = 0;
        break;
        default:
            nfn = *c++;
#ifdef BUILD_EU_VERSION
            nfw = sys_wrk.language == 0 ? font_w_b0_e[nfn] : font_w_b0[nfn];
            SetFontTex(0, 1, 1);
#else
            nfw = font_w_b0[nfn];
            SetFontTex(1, 1);
#endif
            if (column <= mesv22.cnt - line)
            {
                mesv22.mes_alps[line][column] = mesv22.mes_alps[line][column] + 4 > 0x80 ? 0x80 : mesv22.mes_alps[line][column] + 4;
            }
#ifdef BUILD_EU_VERSION
            SetFontPat(0, npri, nfn, nx, ny, nfw, nr, ng, nb, mesv22.mes_alps[line][column] * na >> 7);
#else
            SetFontPat(npri, nfn, nx, ny, nfw, nr, ng, nb, mesv22.mes_alps[line][column] * na >> 7);
#endif
            nx += nfw + nxw;
            if (sflg != 0)
            {
                pw[selnum-1] += nfw + nxw;
            }
            column++;
        break;
        }
    }

    if (s->st & 0x20)
    {
        sq = (SQAR_DAT){
            .w = 0x64,
            .h = 0x64,
            .x = 0,
            .y = 0,
            .pri = 0x4000,
            .r = 0x20,
            .g = 0x40,
            .b = 0x80,
            .alpha = 0x80,
        };

        if ((s->st & 0x80) == 0)
        {
            if (s->type != 0)
            {
                if (*key_now[0] == 1)
                {
                    s->csr = s->csr > 0 ? s->csr - 1 : selnum - 1;
                }

                if (*key_now[1] == 1)
                {
                    s->csr = s->csr >= selnum - 1 ? 0 : s->csr + 1;
                }
            }
            else
            {
                if (*key_now[2] == 1)
                {
                    s->csr = s->csr > 0 ? s->csr - 1 : selnum - 1;
                }

                if (*key_now[3] == 1)
                {
                    s->csr = s->csr >= selnum - 1 ? 0 : s->csr + 1;
                }
            }

            if (*key_now[5] == 1)
            {
                s->st = s->st ^ 0x20 | 0x80;
            }
        }

        CopySqrDToSqr(&dq, &sq);

        n = s->csr;

        dq.x[1] = px[n] + dq.x[1] - dq.x[0];
        dq.x[0] = px[n];
        dq.x[3] = px[n] + dq.x[3] - dq.x[2];
        dq.x[2] = px[n];
        dq.y[2] = py[n] + dq.y[2] - dq.y[0];
        dq.y[3] = py[n] + dq.y[3] - dq.y[1];
        dq.y[0] = py[n];
        dq.y[1] = py[n];
        dq.x[1] = dq.x[0] + pw[n];
        dq.x[3] = dq.x[2] + pw[n];
        dq.y[2] = dq.y[0] + 24;
        dq.y[3] = dq.y[1] + 24;

        dq.alpha = 0x40;
        {
            int i;
            for (i = 0; i < 4; i++)
            {
                dq.r[i] = 0x20;
                dq.g[i] = 0x40;
                dq.b[i] = 0x80;
            }
        }
        dq.pri = npri - 16;
        dq.z = 0x1000000f - npri;
        DispSqrD(&dq);
    }

    s->brnch_num = selnum;

    i = draw_mpri[nmdpri][1];
    mpbuf[i].ul128 = (u_long128)0;
    mpbuf[i].ui32[0] = nmdpkt + DMAend - i - 1;
    nmdpri++;

    mesv22.cnt++;
    mesv22.pass = 1;

    return s->csr + 1;
}

int SetMessageV2(DISP_STR *s)
{
    u_char *c;
    u_char *cp;
    int i;
    int n;
    int m;
    int loop;
    int sflg;
    int npri;
    int nx;
    int ny;
    int nxw;
    int nyw;
    int px[32];
    int py[32];
    int pw[32];
    int selnum;
    u_char nr;
    u_char ng;
    u_char nb;
    u_char na;
    u_char nfn;
    u_char nfw;
    u_char nfh;
    char cwo[256];
    SQAR_DAT sq;
    DISP_SQAR dq;

    c = s->str;
    nxw = s->x_wide;
    nyw = s->y_wide;
    nr = s->r;
    ng = s->g;
    nb = s->b;
    na = s->alpha;
    npri = s->pri;
    nx = s->pos_x;
    ny = s->pos_y;

    selnum = 0;

    draw_mpri[nmdpri][0] = npri;
    draw_mpri[nmdpri][1] = nmdpkt++;

#ifdef BUILD_EU_VERSION
    SetFontTex(0, 0, 1);
    SetFontEnv(0);
#else
    SetFontTex(0, 1);
    SetFontEnv();
#endif

    loop = 1;
    sflg = 0;

    while (loop)
    {
        switch(*c)
        {
        case 0xf0:
            c++;
            nfn = *c++;
            nfw = font_w_b1[nfn];
#ifdef BUILD_EU_VERSION
            SetFontTex(0, 1, 2);
            SetFontPat(0, npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#else
            SetFontTex(1, 2);
            SetFontPat(npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#endif
            nx += nfw + nxw;
            if (sflg != 0)
            {
                pw[selnum-1] += nfw + nxw;
            }
        break;
        case 0xf1:
            c++;
            nfn = *c++;
            nfw = font_w_b2[nfn];
#ifdef BUILD_EU_VERSION
            SetFontTex(0, 1, 3);
            SetFontPat(0, npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#else
            SetFontTex(1, 3);
            SetFontPat(npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#endif
            nx += nfw + nxw;
            if (sflg != 0)
            {
                pw[selnum-1] += nfw + nxw;
            }
        break;
        case 0xf2:
            c++;
            nfn = *c++;
            nfw = font_w_b3[nfn];
#ifdef BUILD_EU_VERSION
            SetFontTex(0, 1, 4);
            SetFontPat(0, npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#else
            SetFontTex(1, 4);
            SetFontPat(npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#endif
            nx += nfw + nxw;
            if (sflg != 0)
            {
                pw[selnum-1] += nfw + nxw;
            }
        break;
        case 0xf3:
            c++;
            nfn = *c++;
            nfw = font_w_b4[nfn];
#ifdef BUILD_EU_VERSION
            SetFontTex(0, 1, 5);
            SetFontPat(0, npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#else
            SetFontTex(1, 5);
            SetFontPat(npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#endif
            nx += nfw + nxw;
            if (sflg != 0)
            {
                pw[selnum-1] += nfw + nxw;
            }
        break;
        case 0xf7:
            c++;
            nxw = *c++;
        break;
        case 0xf8:
            c++;
            nyw = *c++;
        break;
        case 0xf9:
            s->st |= 0x20;
            cp = c;
            c++;
            {
                u_char c1, c2;
                c1 = *++c;
                c2 = *++c;
                nx = c2 << 8 | c1;
            }
            c++;
            if (s->type != 0)
            {
                if (s->str != cp)
                {
                    nfh = 24;
                    ny += nfh + nyw;
                }
            }
            else
            {
                if ((s->str != cp) && (sflg == 0))
                {
                    nfh = 24;
                    ny += nfh + nyw;
                }
            }
            sflg = 1;
            px[selnum] = nx;
            py[selnum] = ny;
            pw[selnum] = 0;
            selnum++;
        break;
        case 0xfa:
            nx = s->pos_x;
            nfh = 24;
            ny += nfh + nyw;
            c++;
        break;
        case 0xfb:
            c++;
            m = *c++;
            sprintf(cwo, "%d", *mes_ex_nums[*c++]);
            i = 0;
            while (cwo[i] != '\0' && loop != 2)
            {
                nfn = m != 0 ? cwo[i] + 5 :  cwo[i] + 15;
#ifdef BUILD_EU_VERSION
                nfw = sys_wrk.language == 0 ? font_w_b0_e[nfn] : font_w_b0[nfn];
                SetFontTex(0, 1, 1);
                SetFontPat(0, npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#else
                nfw = font_w_b0[nfn];
                SetFontTex(1, 1);
                SetFontPat(npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#endif
                nx += nfw + nxw;
                if (sflg != 0)
                {
                    pw[selnum-1] += nfw + nxw;
                }
                i++;
            }
        break;
        case 0xfc:
        break;
        case 0xfd:
            nr = *++c;
            ng = *++c;
            nb = *++c;
            c++;
        break;
        case 0xfe:
            c++;
            nx = s->pos_x;
            nfh = 24;
            ny += nyw + nfh;
        break;
        case 0xff:
            loop = 0;
            s->st |= 0x40;
            c++;
        break;
        default:
            nfn = *c++;
#ifdef BUILD_EU_VERSION
            nfw = sys_wrk.language == 0 ? font_w_b0_e[nfn] : font_w_b0[nfn];
            SetFontTex(0, 1, 1);
            SetFontPat(0, npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#else
            nfw = font_w_b0[nfn];
            SetFontTex(1, 1);
            SetFontPat(npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#endif
            nx += nfw + nxw;
            if (sflg != 0)
            {
                pw[selnum-1] += nfw + nxw;
            }
        break;
        }
    }

    if (s->st & 0x20)
    {
        sq = (SQAR_DAT){
            .w = 0x64,
            .h = 0x64,
            .x = 0,
            .y = 0,
            .pri = 0x4000,
            .r = 0x20,
            .g = 0x40,
            .b = 0x80,
            .alpha = 0x80,
        };

        if ((s->st & 0x80) == 0)
        {
            if (s->type != 0)
            {
                if (*key_now[0] == 1)
                {
                    s->csr = s->csr > 0 ? s->csr - 1 : selnum - 1;
                }

                if (*key_now[1] == 1)
                {
                    s->csr = s->csr >= selnum - 1 ? 0 : s->csr + 1;
                }
            }
            else
            {
                if (*key_now[2] == 1)
                {
                    s->csr = s->csr > 0 ? s->csr - 1 : selnum - 1;
                }

                if (*key_now[3] == 1)
                {
                    s->csr = s->csr >= selnum - 1 ? 0 : s->csr + 1;
                }
            }

            if (*key_now[5] == 1)
            {
                s->st = s->st ^ 0x20 | 0x80;
            }
        }

        CopySqrDToSqr(&dq, &sq);

        n = s->csr;

        dq.x[1] = px[n] + dq.x[1] - dq.x[0];
        dq.x[0] = px[n];
        dq.x[3] = px[n] + dq.x[3] - dq.x[2];
        dq.x[2] = px[n];
        dq.y[2] = py[n] + dq.y[2] - dq.y[0];
        dq.y[3] = py[n] + dq.y[3] - dq.y[1];
        dq.y[0] = py[n];
        dq.y[1] = py[n];
        dq.x[1] = dq.x[0] + pw[n];
        dq.x[3] = dq.x[2] + pw[n];
        dq.y[2] = dq.y[0] + 24;
        dq.y[3] = dq.y[1] + 24;

        dq.alpha = 0x40;
        {
            int i;
            for (i = 0; i < 4; i++)
            {
                dq.r[i] = 0x20;
                dq.g[i] = 0x40;
                dq.b[i] = 0x80;
            }
        }
        dq.pri = npri - 16;
        dq.z = 0x1000000f - npri;
        DispSqrD(&dq);
    }

    s->brnch_num = selnum;

    i = draw_mpri[nmdpri][1];
    mpbuf[i].ul128 = (u_long128)0;
    mpbuf[i].ui32[0] = nmdpkt + DMAend - i - 1;
    nmdpri++;

    return s->csr + 1;
}

int SubMessageV3(u_char *s, int pri, int delflg)
{
    u_char *c;
    u_char *cp;
    int i;
    int j;
    int k;
    int n;
    int m;
    int loop;
    int sflg;
    int npri;
    int nx;
    int ny;
    int nxw;
    int nyw;
    int px[32];
    int py[32];
    int pw[32];
    u_char nr;
    u_char ng;
    u_char nb;
    u_char na;
    u_char nfn;
    u_char nfw;
    u_char nfh;
    u_char nul;
    char cwo[256];
    SQAR_DAT sq;
    DISP_SQAR dq;

    if (msdat.pass != 0)
    {
        return;
    }

    if (delflg != 0 && plyr_wrk.mode == 1)
    {
        FinderEndSet();
    }

    if (msdat.flg == 0)
    {
        if (s == 0)
        {
            return;
        }

        msdat.bx = (s[1] << 8) + s[0];
        msdat.by = (s[3] << 8) + s[2];
        msdat.pri = s[4];
        msdat.disptype = s[5];

        msdat.stp = &s[6];
        msdat.sta = 0;
        msdat.retst = 0;

        msdat.flg = 1;
        msdat.csr = 0;
        msdat.decide = 0;
        msdat.mes_is_end = 0;
        msdat.cnt = 0;

        msdat.r = 0x80;
        msdat.g = 0x80;
        msdat.b = 0x80;

        msdat.usrgb[0] = 0x0;
        msdat.usrgb[1] = 0x80;
        msdat.usrgb[2] = 0x80;
        msdat.usrgb[3] = 0x80;

        msdat.vib = 0;


        msdat.alp = 0;

        if (msdat.disptype != 0)
        {
            msdat.fntmcnt = 0;
        }
        else
        {
            msdat.fntmcnt = 0xffff;
        }

        msdat.fntwait = 0;
    }

    msdat.fntcnt = 0;
    msdat.selnum = 0;

    c = msdat.stp;

    npri = msdat.pri;
    nx = msdat.bx;
    ny = msdat.by;
    nr = msdat.r;
    ng = msdat.g;
    nb = msdat.b;
    na = msdat.alp;
    nxw = 0;
    nyw = 0;
    nul = msdat.usrgb[0];

    loop = 0;
    sflg = 0;

    DrawMessageBox(pri, msdat.bx - 24, msdat.by - 16, 528.0f, 106.0f, msdat.alp);

    draw_mpri[nmdpri][0] = npri;
    draw_mpri[nmdpri][1] = nmdpkt++;

#ifdef BUILD_EU_VERSION
    SetFontTex(0, 0, 1);
    SetFontEnv(0);
#else
    SetFontTex(0, 1);
    SetFontEnv();
#endif

    while (loop == 0)
    {
        switch(*c)
        {
        case 0xf0:
            c++;
            nfn = *c++;
            nfw = font_w_b1[nfn];
#ifdef BUILD_EU_VERSION
            SetFontTex(0, 1, 2);
            SetFontPat(0, npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#else
            SetFontTex(1, 2);
            SetFontPat(npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#endif
            SetUnderLine(nul, npri, nx, ny, nfw, msdat.usrgb[1], msdat.usrgb[2], msdat.usrgb[3], (u_int)na);
            nx += nfw + nxw;
            msdat.fntcnt++;
            if (sflg != 0)
            {
                pw[msdat.selnum-1] += nfw + nxw;
            }
        break;
        case 0xf1:
            c++;
            nfn = *c++;
            nfw = font_w_b2[nfn];
#ifdef BUILD_EU_VERSION
            SetFontTex(0, 1, 3);
            SetFontPat(0, npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#else
            SetFontTex(1, 3);
            SetFontPat(npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#endif
            SetUnderLine(nul, npri, nx, ny, nfw, msdat.usrgb[1], msdat.usrgb[2], msdat.usrgb[3], na);
            nx += nfw + nxw;
            msdat.fntcnt++;
            if (sflg != 0)
            {
                pw[msdat.selnum-1] += nfw + nxw;
            }
        break;
        case 0xf2:
            c++;
            nfn = *c++;
            nfw = font_w_b3[nfn];
#ifdef BUILD_EU_VERSION
            SetFontTex(0, 1, 4);
            SetFontPat(0, npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#else
            SetFontTex(1, 4);
            SetFontPat(npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#endif
            SetUnderLine(nul, npri, nx, ny, nfw, msdat.usrgb[1], msdat.usrgb[2], msdat.usrgb[3], na);
            nx += nfw + nxw;
            msdat.fntcnt++;
            if (sflg != 0)
            {
                pw[msdat.selnum-1] += nfw + nxw;
            }
        break;
        case 0xf3:
            c++;
            nfn = *c++;
            nfw = font_w_b4[nfn];
#ifdef BUILD_EU_VERSION
            SetFontTex(0, 1, 5);
            SetFontPat(0, npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#else
            SetFontTex(1, 5);
            SetFontPat(npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#endif
            SetUnderLine(nul, npri, nx, ny, nfw, msdat.usrgb[1], msdat.usrgb[2], msdat.usrgb[3], na);
            nx += nfw + nxw;
            msdat.fntcnt++;
            if (sflg != 0)
            {
                pw[msdat.selnum-1] += nfw + nxw;
            }
        break;
        case 0xf5:
            i = *++c;
            j = *++c;
            k = *++c;
            c++;
            if (msdat.vib == 0x0)
            {
                SetVibrate(i, j, k);
                msdat.vib = 1;
            }
        break;
        case 0xf6:
            nul = *++c;
            msdat.usrgb[1] = *++c;
            msdat.usrgb[2] = *++c;
            msdat.usrgb[3] = *++c;
            c++;
        break;
        case 0xf7:
            c++;
            nxw = *c++;
        break;
        case 0xf8:
            c++;
            nyw = *c++;
        break;
        case 0xf9:
            cp = c++;
            msdat.seltype = *c;
            msdat.sta |= 0x20;
            {
                u_char c1, c2;
                c1 = *++c;
                c2 = *++c;
                nx = c2 << 8 | c1;
            }
            c++;
            if (msdat.seltype != 0)
            {
                if (msdat.stp != cp)
                {
                    nfh = 24;
                    ny += nfh + nyw;
                }
            }
            else
            {
                if (msdat.stp != cp && sflg == 0)
                {
                    nfh = 24;
                    ny += nfh + nyw;
                }
            }
            sflg = 1;
            px[msdat.selnum] = nx;
            py[msdat.selnum] = ny;
            pw[msdat.selnum] = 0;
            msdat.selnum++;
        break;
        case 0xfa:
            c++;
            if (msdat.sta & 0x1)
            {
                nx = msdat.bx;
                ny = msdat.by;
                msdat.sta ^= 0x1f;
                msdat.stp = c;
                if (msdat.disptype != 0)
                {
                    msdat.fntmcnt = 0;
                    msdat.fntwait = 0;
                }
                msdat.r = nr;
                msdat.g = ng;
                msdat.b = nb;
                msdat.usrgb[0] = nul;
                msdat.vib = 0;
            }
            else
            {
                msdat.sta |= 0x10;
            }
            loop = 1;
        break;
        case 0xfb:
            c++;
            m = *c++;
            sprintf(cwo, "%d", *mes_ex_nums[*c++]);
            i = 0;
            while (cwo[i] != '\0' && loop != 2)
            {
                nfn = m != 0 ? cwo[i] + 5 :  cwo[i] + 15;
#ifdef BUILD_EU_VERSION
                nfw = sys_wrk.language == 0 ? font_w_b0_e[nfn] : font_w_b0[nfn];
                SetFontTex(0, 1, 1);
                SetFontPat(0, npri, nfn & 0xff, nx, ny, nfw, nr, ng, nb, na);
#else
                nfw = font_w_b0[nfn];
                SetFontTex(1, 1);
                SetFontPat(npri, nfn & 0xff, nx, ny, nfw, nr, ng, nb, na);
#endif
                SetUnderLine(nul, npri, nx, ny, nfw, msdat.usrgb[1], msdat.usrgb[2], msdat.usrgb[3], na);
                msdat.fntcnt++;
                nx += nfw + nxw;
                if (sflg != 0)
                {
                    pw[msdat.selnum-1] += nfw + nxw;
                }
                if (msdat.fntmcnt <= msdat.fntcnt)
                {
                    loop = 2;
                }
                i++;
            }
        break;
        case 0xfc:
        break;
        case 0xfd:
            nr = *++c;
            ng = *++c;
            nb = *++c;
            c++;
        break;
        case 0xfe:
            nx = msdat.bx;
            nfh = 24;
            ny += nyw + nfh;
            c++;
        break;
        case 0xff:
            if (msdat.mes_is_end == 0)
            {
                if (msdat.sta & 0x1)
                {
                    msdat.mes_is_end = 1;
                    SetVibrate(0, 0, 0);
                    SetVibrate(1, 0, 0);
                }
                else
                {
                    msdat.sta = msdat.sta | 0x40;
                }
            }
            loop = 1;
        break;
        default:
            nfn = *c++;
#ifdef BUILD_EU_VERSION
            nfw = sys_wrk.language == 0 ? font_w_b0_e[nfn] : font_w_b0[nfn];
            SetFontTex(0, 1, 1);
            SetFontPat(0, npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#else
            nfw = font_w_b0[nfn];
            SetFontTex(1, 1);
            SetFontPat(npri, nfn, nx, ny, nfw, nr, ng, nb, na);
#endif
            SetUnderLine(nul, npri, nx, ny, nfw, msdat.usrgb[1], msdat.usrgb[2], msdat.usrgb[3], na);
            nx += nfw + nxw;
            msdat.fntcnt++;
            if (sflg != 0)
            {
                pw[msdat.selnum-1] += nfw + nxw;
            }
        break;
        }

        if (msdat.fntmcnt <= msdat.fntcnt)
        {
            loop = 2;
        }
    }

    if ((loop == 2) && (msdat.fntwait-- < 1))
    {
        msdat.fntmcnt++;
        msdat.fntwait = 4;
    }

    if ((msdat.sta & 0x10) != 0)
    {
        float ss;
        ss = SgCosf((msdat.cnt * 6.0f * PI) / 180.0f);
        DrawButtonTex(
            pri - 16,
            3,
            msdat.bx + (nxw + 24) * 0x13,
            msdat.by + (nyw + 24) * 2,
            (ss + 1.0f) * 64.0f);
        msdat.cnt = msdat.cnt + 1;
    }

    if ((msdat.sta & 0x20U) != 0)
    {
        sq = (SQAR_DAT){
            .w = 0x64,
            .h = 0x64,
            .x = 0,
            .y = 0,
            .pri = 0x4000,
            .r = 0x20,
            .g = 0x40,
            .b = 0x80,
            .alpha = 0x80,
        };

        if (msdat.decide == 0)
        {
            if (msdat.seltype != 0)
            {
                if (*key_now[0] == 1)
                {
                    msdat.csr = msdat.csr > 0 ? msdat.csr - 1 : msdat.selnum - 1;
                }

                if (*key_now[1] == 1)
                {
                    msdat.csr = msdat.csr >= msdat.selnum - 1 ? 0 : msdat.csr + 1;
                }
            }
            else
            {
                if (*key_now[2] == 1)
                {
                    msdat.csr = msdat.csr > 0 ? msdat.csr - 1 : msdat.selnum - 1;
                }

                if (*key_now[3] == 1)
                {
                    msdat.csr = msdat.csr >= msdat.selnum - 1 ? 0 : msdat.csr + 1;
                }
            }

            if (*key_now[5] == 1)
            {
                msdat.sta ^= 0x2f;
                msdat.decide = 1;
            }
        }

        msdat.retst = msdat.csr + 1;

        CopySqrDToSqr(&dq, &sq);

        n = msdat.csr;

        dq.x[1] = px[n] + dq.x[1] - dq.x[0];
        dq.x[0] = px[n];
        dq.x[3] = px[n] + dq.x[3] - dq.x[2];
        dq.x[2] = px[n];
        dq.y[2] = py[n] + dq.y[2] - dq.y[0];
        dq.y[3] = py[n] + dq.y[3] - dq.y[1];
        dq.y[0] = py[n];
        dq.y[1] = py[n];
        dq.x[1] = dq.x[0] + pw[n];
        dq.x[3] = dq.x[2] + pw[n];
        dq.y[2] = dq.y[0] + 24;
        dq.y[3] = dq.y[1] + 24;

        dq.alpha = msdat.alp / 2;
        {
            int i;
            for (i = 0; i < 4; i++)
            {
                dq.r[i] = 0x20;
                dq.g[i] = 0x40;
                dq.b[i] = 0x80;
            }
        }
        dq.pri = npri - 16;
        dq.z = 0x1000000f - npri;

        DispSqrD(&dq);
    }

    switch (msdat.mes_is_end)
    {
    case 0:
        if (msdat.alp < 0x80)
        {
            msdat.alp = msdat.alp + 8 > 0x80 ? 0x80 : msdat.alp + 8;
        }
    break;
    case 1:
        if (msdat.alp == 0)
        {
            msdat.mes_is_end = 2;
        }
        else
        {
            msdat.alp = msdat.alp - 8 < 0 ? 0 : msdat.alp - 8;
        }
    case 2:
        // do nothing
    break;
    }

    i = draw_mpri[nmdpri][1];
    mpbuf[i].ul128 = (u_long128)0;
    mpbuf[i].ui32[0] = nmdpkt + DMAend - i - 1;
    nmdpri++;

    MesKeyCheck();

    msdat.pass = 1;

    return msdat.retst;
}

int SetMessageV3(u_char *s, int pri)
{
    return SubMessageV3(s, pri, 1);
}

int SetMessageV3_2(u_char *s, int pri)
{
    return SubMessageV3(s, pri, 0);
}

void MesPassCheck()
{
    if (msdat.pass == 0)
    {
        msdat.flg = 0;
    }

    msdat.pass = 0;
}

static void MesKeyCheck()
{
    if ((msdat.sta & 0x10) != 0 && *key_now[5] == 1)
    {
        msdat.sta |= 0x1;
    }

    if ((msdat.sta & 0x40) != 0)
    {
        msdat.sta |= 0x1;
    }
}

int MesStatusCheck()
{
    int ret;

    ret = 0;

    if (msdat.sta & 0x10)
    {
        ret = 1;
    }
    else if ((msdat.sta & 0x40) == 0 || msdat.mes_is_end != 2)
    {
        if (msdat.sta & 0x20)
        {
            ret = 3;
        }
        else
        {
            ret = 2;
        }
    }

    return ret;
}

void DrawButtonTex(u_int pri, int type, float x, float y, u_char alp)
{
    DISP_SPRT ds;

    CopySprDToSpr(&ds, &btntex[type]);

    ds.tex0 = effdat[0].tex0;
    ds.pri = pri;
    ds.z = 0x0fffffff - pri;
    ds.alpha = alp;
    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    ds.x = x;
    ds.y = y;

    DispSprD(&ds);
}

void DrawButtonTexNZ(u_int pri, int type, float x, float y, u_char alp)
{
    DISP_SPRT ds;

    CopySprDToSpr(&ds, &btntex[type]);

    ds.tex0 = effdat[0].tex0;
    ds.pri = pri;
    ds.z = 0x0fffffff - pri;
    ds.alpha = alp;
    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    ds.x = x;
    ds.y = y;

    DispSprD(&ds);
}

void DrawMessageBox(u_int pri, float x, float y, float sizew, float sizeh, u_char alp)
{
    int i;
    float ssw;
    float ssh;
    float px[8];
    float py[8];
    float sw[4];
    float sh[4];
    float p1w;
    float p1h;
    DISP_SPRT ds;
    float msx;
    float msy;
    u_char alp1;
    u_char alp2;

    msx = 20.0f;
    msy = 10.0f;

    p1w = 30.0f;
    p1h = 14.0f;

    ReqPlayerStop(10);

    px[0] = x;
    py[0] = y;
    px[1] = (x + sizew) - p1w;
    py[1] = y;
    px[2] = x;
    py[2] = (y + sizeh) - p1h;
    px[3] = (x + sizew) - p1w;
    py[3] = (y + sizeh) - p1h;

    px[4] = x + p1w;
    py[4] = y;
    px[5] = x;
    py[5] = y + p1h;
    px[6] = (x + sizew) - p1w;
    py[6] = y + p1h;
    px[7] = x + p1w;
    py[7] = (y + sizeh) - p1h;

    ssw = sizew - 60.0f;
    ssh = sizeh - 28.0f;

    sw[0] = sw[3] = ssw / p1w;
    sw[1] = sw[2] = 1.0f;
    sh[1] = sh[2] = ssh / p1h;
    sh[0] = sh[3] = 1.0f;

#ifdef BUILD_EU_VERSION
    if (subtitles_sys.run != 0)
    {
        msdat.bx_pass_st = 1;
    }

    msdat.bx_pass = 1;

    if (msdat.bx_pass_st != 0)
    {
        alp1 = (alp * 128) / 128.0f;
    }
    else
    {
        alp1 = (alp * 80) / 128.0f;
    }

#else
    alp1 = (alp * 80) / 128.0f;
#endif
    alp2 = (alp * 4) / 128.0f;

    SetSquareSN(
        pri,
        px[4]       - SCR_WIDTH_HALF - msx,
        py[5]       - SCR_HEIGHT - msy,
        px[4] + ssw - SCR_WIDTH_HALF + msx,
        py[5] + ssh - SCR_HEIGHT + msy,
        0x0b, 0x08, 0x07, alp1);

    for (i = 0; i < 4; i++)
    {
        CopySprDToSpr(&ds, &mesbox[i]);

        ds.tex0 = effdat[26].tex0;
        ds.pri = pri;
        ds.z = 0x0fffffff - pri;
        ds.alpha = alp;
        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
        ds.x = px[i];
        ds.y = py[i];

        DispSprD(&ds);
    }

    for (i = 0; i < 4; i++)
    {
        CopySprDToSpr(&ds, &mesbox[i+4]);

        ds.tex0 = effdat[26].tex0;
        ds.pri = pri;
        ds.z = 0x0fffffff - pri;
        ds.alpha = alp;
        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
        ds.x = px[i+4];
        ds.y = py[i+4];
        ds.scw = sw[i];
        ds.sch = sh[i];
        ds.csx = px[i+4];
        ds.csy = py[i+4];

        DispSprD(&ds);
    }

    SetSquareSN(
        pri,
        px[4]       - SCR_WIDTH_HALF,
        py[5]       - SCR_HEIGHT,
        px[4] + ssw - SCR_WIDTH_HALF,
        py[5] + ssh - SCR_HEIGHT,
        0x0b, 0x08, 0x07, alp2);
}

int GetStrLength(u_char *str)
{
    u_char *c;
    int num;
    int loop;

    c = str;

    num = 0;
    loop = 1;

    while (loop)
    {
        switch(*c)
        {
        case 0xf0:
        case 0xf1:
        case 0xf2:
        case 0xf3:
            c += 2;
            num++;
        break;
        case 0xf6:
            c += 5;
        break;
        case 0xf7:
        case 0xf8:
            c += 2;
        break;
        case 0xf9:
        case 0xfb:
        case 0xfc:
            // do nothing ... no increment => endless loop ??
        break;
        case 0xfa:
        case 0xfe:
            c += 1;
        break;
        case 0xf5:
        case 0xfd:
            c += 4;
        break;
        case 0xff:
            c += 1;
            loop = 0;
        break;
        default:
            c += 1;
            num++;
        break;
        }
    }

    return num;
}

#ifdef BUILD_EU_VERSION
int GetStrWidthMain(u_char *str, int type)
{
    u_char *c;
    u_char n;
    int loop;
    int w;


    c = str;

    loop = 1;
    w = 0;

    while (loop)
    {
        switch(*c)
        {
        case 0xf0:
            n = c[1];
            c += 2;
            w += font_w_b1[n];
        break;
        case 0xf1:
            n = c[1];
            c += 2;
            w += font_w_b2[n];
        break;
        case 0xf2:
            n = c[1];
            c += 2;
            w += font_w_b3[n];
        break;
        case 0xf3:
            n = c[1];
            c += 2;
            w += font_w_b4[n];
        break;
        case 0xf6:
            c += 5;
        break;
        case 0xf7:
        case 0xf8:
            c += 2;
        break;
        case 0xf9:
        case 0xfb:
        case 0xfc:
            // do nothing ... no increment => endless loop ??
        break;
        case 0xf5:
        case 0xfd:
            c += 4;
        break;
        case 0xfa:
        case 0xfe:
        case 0xff:
            c += 1;
            loop = 0;
        break;
        default:
            n = c[0];
            if (type != 0)
            {
                w += font_w_b1[n];
            }
            else if (sys_wrk.language == 0)
            {
                w += font_w_b0_e[n];
            }
            else
            {
                w += font_w_b0[n];
            }
            c += 1;
        break;
        }
    }

    return w;
}

int GetStrWidth(u_char *str)
{
    return GetStrWidthMain(str, 0);
}
#else
int GetStrWidth(u_char *str)
{
    u_char *c;
    u_char n;
    int loop;
    int w;

    c = str;

    loop = 1;
    w = 0;

    while (loop)
    {
        switch(*c)
        {
        case 0xf0:
            n = c[1];
            c += 2;
            w += font_w_b1[n];
        break;
        case 0xf1:
            n = c[1];
            c += 2;
            w += font_w_b2[n];
        break;
        case 0xf2:
            n = c[1];
            c += 2;
            w += font_w_b3[n];
        break;
        case 0xf3:
            n = c[1];
            c += 2;
            w += font_w_b4[n];
        break;
        case 0xf6:
            c += 5;
        break;
        case 0xf7:
        case 0xf8:
            c += 2;
        break;
        case 0xf9:
        case 0xfb:
        case 0xfc:
            // do nothing ... no increment => endless loop ??
        break;
        case 0xfa:
        case 0xfe:
            c += 1;
        break;
        case 0xf5:
        case 0xfd:
            c += 4;
        break;
        case 0xff:
            c += 1;
            loop = 0;
        break;
        default:
            n = c[0];
            c += 1;
            w += font_w_b0[n];
        break;
        }
    }

    return w;
}
#endif

#ifdef BUILD_EU_VERSION
int GetStrWidthST(u_char *str)
{
    return GetStrWidthMain(str, 1);
}
#endif

void FontDispSample()
{
    static float w = 100.0f;
    static float h = 100.0f;
    static int fl = 0;
    u_char str3[14] = {
        0xF0, 0x00, 0xF0, 0x01,
        0xFB, 0x01, 0x00, 0xF1,
        0x00, 0xF1, 0x16, 0xF1,
        0x2A, 0xFF
    };
    u_char str2[11] = {
        0x3F, 0x40, 0x41, 0x42,
        0x43, 0x40, 0x42, 0x43,
        0x46, 0x77, 0xFF
    };
    u_char str1[11] = {
        0x3F, 0x40, 0x41, 0x42,
        0x43, 0x3F, 0x41, 0x42,
        0x45, 0x76, 0xFF
};
    DISP_STR ds;
    STR_DAT sd = {
        .str = NULL,
        .pos_x = 0,
        .pos_y = 0,
        .type = 1,
        .r = 0xFF,
        .g = 0xFF,
        .b = 0xFF,
        .alpha = 0x80,
        .pri = 0x5000,
    };

    CopyStrDToStr(&ds, &sd);

    ds.pri = 0x2000;
    ds.pos_x = 0x28;
    ds.pos_y = 0x5a;
    ds.str = str3;

    SetMessageV2(&ds);
}

#ifdef BUILD_EU_VERSION
int SetMessageMov(int path, int type, DISP_STR *s)
{
    u_char *c;
    int i;
    int loop;
    int npri;
    int nx;
    int ny;
    int nxw;
    int nyw;
    int px[32];
    int py[32];
    int pw[32];
    u_char nr;
    u_char ng;
    u_char nb;
    u_char na;
    u_char nfn;
    u_char nfw;
    int nfh;
    char cwo[256];

    c = s->str;
    nxw = s->x_wide;
    nyw = s->y_wide;
    nr = s->r;
    ng = s->g;
    nb = s->b;
    na = s->alpha;
    npri = s->pri;
    nx = type == 0 ? s->pos_x : s->pos_x - GetStrWidthST(c) / 2;
    ny = s->pos_y;

    draw_mpri[nmdpri][0] = npri;

    if (path == 1)
    {
        draw_mpri[nmdpri++][1] = nmdpkt;
    }
    else
    {
        draw_mpri[nmdpri][1] = nmdpkt++;
    }

    SetFontTex(path, 0, 1);
    SetFontEnv(path);

    loop = 1;

    while (loop)
    {
        switch(*c)
        {
        case 0xf0:
            c++;
            nfn = *c++;
            nfw = font_w_b1[nfn];
            SetFontTex(path, 1, 2);
            SetFontPat(path, npri, nfn, nx, ny, nfw, nr, ng, nb, na);
            nx += nfw + nxw;
        break;
        case 0xf1:
            c++;
            nfn = *c++;
            nfw = font_w_b2[nfn];
            SetFontTex(path, 1, 3);
            SetFontPat(path, npri, nfn, nx, ny, nfw, nr, ng, nb, na);
            nx += nfw + nxw;
        break;
        case 0xf2:
            c++;
            nfn = *c++;
            nfw = font_w_b3[nfn];
            SetFontTex(path, 1, 4);
            SetFontPat(path, npri, nfn, nx, ny, nfw, nr, ng, nb, na);
            nx += nfw + nxw;
        break;
        case 0xf3:
            c++;
            nfn = *c++;
            nfw = font_w_b4[nfn];
            SetFontTex(path, 1, 5);
            SetFontPat(path, npri, nfn, nx, ny, nfw, nr, ng, nb, na);
            nx += nfw + nxw;
        break;
        case 0xfd:
            nr = *++c;
            ng = *++c;
            nb = *++c;
        case 0xf7:
        case 0xf8:
        case 0xf9:
        case 0xfa:
        case 0xfb:
        case 0xfc:
            c++;
        break;
        case 0xfe:
            c++;
            nx = type == 0 ? s->pos_x : s->pos_x - GetStrWidthST(c) / 2;
            nfh = 24;
            ny += nfh + nyw;
        break;
        case 0xff:
            c++;
            loop = 0;
            s->st |= 0x40;
        break;
        default:
            nfn = *c++;
            nfw = font_w_b1[nfn];
            SetFontTex(path, 1, 2);
            SetFontPat(path, npri, nfn, nx, ny, nfw, nr, ng, nb, na);
            nx += nfw + nxw;
        break;
        }
    }

    if (path != 1)
    {
        i = draw_mpri[nmdpri][1];
        mpbuf[i].ul128 = (u_long128)0;
        mpbuf[i].ui32[0] = nmdpkt + DMAend - i - 1;
        nmdpri++;
    }

    return s->csr + 1;
}
#endif

u_char *save_mes_addr = NULL;
