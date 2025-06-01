#include "common.h"
#include "enums.h"
#include "typedefs.h"
#include "effect_oth.h"

#include "ee/eestruct.h"
#include "sce/libvu0.h"

#include "os/eeiop/eese.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "main/glob.h"
#include "ingame/ig_glob.h"
#include "ingame/menu/sp_menu.h"
#include "ingame/plyr/unit_ctl.h"
#include "ingame/enemy/ene_ctl.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph2d/tim2_new.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/effect_obj.h"

int stop_lf = 0;

static int init_pond;
static int init_hhaze;
static int init_newitem;
static int init_torch;
static int init_smoke;
static int init_haze_pond;
static void *amulet_fire_ret;
static int amulet_fire_flow;
static int amulet_fire_cnt;

static RIPPLE_SUB rs[48];
static EFF_LEAF eff_leaf[6];
static HEAT_HAZE ene_particle[3];
static HEAT_HAZE amu_particle[1];
static HEAT_HAZE torch_particle[5];
static HEAT_HAZE smoke_particle[4];
static NEW_ITEM ni[24];
static RIPPLE2 rip[8];

#define ADDRESS 0x1080000

#define PI 3.1415927f
#define PI2 6.2831855f

#define DEG2RAD(x) ((float)(x)*PI/180.0f)

void InitEffectOth()
{
    int i;

    for (i = 0; i < 3; i++)
    {
        cont_facespr_tbl[i] = -1;
        cont_facespr_otbl[i] = -1;
    }

    for (i = 0; i < 48; i++)
    {
        rs[i].alp = 0;
    }

    init_pond = 1;
    init_hhaze = 1;
    init_newitem = 1;
    init_torch = 1;

    for (i = 0; i < 5; i++)
    {
        torch_particle[i].flag = 0;
        torch_particle[i].cnt = 0;
    }

    init_smoke = 1;
    init_haze_pond = 0;

    amulet_fire_ret = NULL;
    amulet_fire_flow = -1;
    amulet_fire_cnt = -1;
}

void InitEffectOthEF()
{
    int i;

    for(i = 0; i < cont_facespr_tbl[3]; i++) // out of bounds access!
    {
        cont_facespr_otbl[i] = cont_facespr_tbl[i];
        cont_facespr_tbl[i] = -1;
    }
}

int SearchEmptyRippleBuf()
{
    int i;
    int fl;

    fl = -1;
    i = 0;

    while (i < 48 && fl == -1)
    {
        if (rs[i].alp == 0x0)
        {
            fl = i;
        }

        i++;
    }

    return fl;
}

void* CallRipple(void *pos, void *rot, float scale, int num, int time)
{
    return SetEffects(0x16, 8, 1, time, 0x80, 0x80, 0x80, 1.0f, scale, pos, rot, num);
}

void SetRipple(EFFECT_CONT *ec)
{
    int n;
    int init = 0;
    sceVu0FVECTOR vt = { 0.0f, 0.0f, 0.0f, 1.0f };

    if (stop_effects == 0)
    {
        if (ec->cnt == 0)
        {
            init = 1;

            if ((ec->dat.uc8[2] & 0xf) == 2)
            {
                ec->dat.uc8[3] = (char)(int)(vu0Rand() * 24.0f) + 4;
            }

            if ((ec->dat.uc8[1] & 8) != 0)
            {
                ec->dat.uc8[7]--;
            }
        }
        if ((ec->dat.uc8[2] & 0xf) && ec->dat.uc8[3])
        {
            ec->cnt = (ec->cnt + 1) % ec->dat.uc8[3];
        }
        else
        {
            ec->cnt = 1;
        }
    }

    if (init)
    {
        n = SearchEmptyRippleBuf();

        rs[n].scl = 0.1f;
        rs[n].alp = 0x20;

        Vu0CopyVector(rs[n].pos, ec->pnt[0]);

        if (ec->dat.uc8[0] == 0x15)
        {
            rs[n].r = 0x80;
            rs[n].g = 0x80;
            rs[n].b = 0x80;
            rs[n].spd = 1.0f;
            rs[n].mscl = 3.2f;

            Vu0CopyVector(rs[n].rot, vt);

            rs[n].rot[3] = ec->dat.uc8[2] & 0x80 ? 1.0f : (ec->dat.uc8[2] & 0x40 ? 2.0f : 0.0f);
        }
        else
        {
            if (monochrome_mode != 0)
            {
                rs[n].r = rs[n].g = rs[n].b = (ec->dat.uc8[4] + ec->dat.uc8[5] + ec->dat.uc8[6]) / 3;
            }
            else
            {
                rs[n].r = ec->dat.uc8[4];
                rs[n].g = ec->dat.uc8[5];
                rs[n].b = ec->dat.uc8[6];
            }

            rs[n].spd = ec->dat.fv[2];
            rs[n].mscl = ec->dat.fv[3];

            Vu0CopyVector(rs[n].rot, ec->pnt[1]);

            rs[n].rot[3] = ec->dat.uc8[2] & 0x80 ? 1.0f : (ec->dat.uc8[2] & 0x40 ? 2.0f : 0.0f);
        }
    }

    if (
        (ec->dat.uc8[1] & 1) ||
        (ec->dat.ul64[0] & 0xff00000000000000) == 0 && (ec->dat.ul64[0] & 0x800)
    )
    {
        ResetEffects(ec);
    }
}

void RunRipple2()
{
    int i;
    int j;
    int n;
    int bak;
    u_int clip[48];
    u_int clpx2;
    u_int clpy2;
    u_int clpz2;
    float add;
    float q;
    sceVu0IVECTOR vtiw[48][4];
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0FVECTOR vtw[4] = {
        { -4.0, -0.5, +4.0, +1.0 },
        { +4.0, -0.5, +4.0, +1.0 },
        { -4.0, -0.5, -4.0, +1.0 },
        { +4.0, -0.5, -4.0, +1.0 }
    };

    clpx2 = 0xfd00;
    clpy2 = 0xfd00;
    clpz2 = 0x0fffffff;

    n = 0;

    add = 0.12f;

    for (j = 0; j < 48; j++)
    {
        if (rs[j].alp != 0x0)
        {
            n++;
        }
    }

    if (n != 0)
    {
        for (j = 0, n = 0; j < 48; j++)
        {
            if (rs[j].alp != 0)
            {
                sceVu0UnitMatrix(wlm);
                if (rs[j].rot[3] != 1.0f)
                {
                    wlm[0][0] = wlm[2][2] = rs[j].scl * 25.0f;
                }
                else
                {
                    wlm[0][0] = wlm[2][2] = rs[j].mscl * 25.0f;
                }
                wlm[1][1] = 25.0f;

                sceVu0RotMatrixX(wlm, wlm, rs[j].rot[0]);
                sceVu0RotMatrixY(wlm, wlm, rs[j].rot[1]);
                sceVu0TransMatrix(wlm, wlm, rs[j].pos);
                sceVu0MulMatrix(slm, SgWSMtx, wlm);
                sceVu0RotTransPersN(vtiw[j], slm, vtw, 4, 1);

                clip[j] = 0;

                for (i = 0; i < 4; i++)
                {
                    if ((vtiw[j][i][0] >= 0 && vtiw[j][i][0] < 0x300) || vtiw[j][i][0] > clpx2)
                    {
                        clip[j]++;
                    }

                    if ((vtiw[j][i][1] >= 0 && vtiw[j][i][1] < 0x300) || vtiw[j][i][1] > clpy2)
                    {
                        clip[j]++;
                    }

                    if (vtiw[j][i][2] == 0 || vtiw[j][i][2] > clpz2)
                    {
                        clip[j]++;
                    }
                }

                if (clip[j] != 0)
                {
                    if (stop_effects == 0)
                    {
                        if (rs[j].mscl > rs[j].scl + add * rs[j].spd)
                        {
                            rs[j].alp = (32.0f - (rs[j].scl * 32.0f) / rs[j].mscl);
                            rs[j].scl += add * rs[j].spd;
                        }
                        else
                        {
                            rs[j].scl = rs[j].mscl;
                            rs[j].alp = 0;
                        }
                    }
                }
                else
                {
                    n ++;
                }
            }
        }

        if (n != 0)
        {
            Reserve2DPacket(0x1000);

            bak = ndpkt;

            pbuf[ndpkt++].ul128 = (u_long128)0;

            pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(5, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
            pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

            pbuf[ndpkt].ul64[0] = 0;
            pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

            pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
            pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

            pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
            pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

            pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
            pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

            pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
            pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

            for (j = 0; j < 48; j++)
            {
                if (clip[j] == 0)
                {
                    if (rs[j].alp != 0x0)
                    {
                        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
                        pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

                        if (rs[j].rot[3] > 1.0f)
                        {
                            pbuf[ndpkt].ul64[0] = effdat[monochrome_mode + 0x2e].tex0;
                        }
                        else if (rs[j].rot[3] > 0.0f)
                        {
                            pbuf[ndpkt].ul64[0] = effdat[monochrome_mode + 4].tex0;
                        }
                        else
                        {
                            pbuf[ndpkt].ul64[0] = effdat[monochrome_mode + 2].tex0;
                        }
                        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

                        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 92, SCE_GIF_PACKED, 3);
                        pbuf[ndpkt++].ul64[1] = 0 \
                            | SCE_GS_ST    << (4 * 0)
                            | SCE_GS_RGBAQ << (4 * 1)
                            | SCE_GS_XYZF2 << (4 * 2);

                        for (i = 0;  i < 4; i++)
                        {
                            q = (1.0f / vtiw[j][i][3]);

                            pbuf[ndpkt].fl32[0] = q * (i % 2 ? 0.98f : 0.02f);
                            pbuf[ndpkt].fl32[1] = q * (i / 2 ? 0.98f : 0.02f);
                            pbuf[ndpkt].fl32[2] = q;
                            pbuf[ndpkt++].fl32[3] = 0;

                            pbuf[ndpkt].ui32[0] = rs[j].r;
                            pbuf[ndpkt].ui32[1] = rs[j].g;
                            pbuf[ndpkt].ui32[2] = rs[j].b;
                            pbuf[ndpkt++].ui32[3] = rs[j].alp;

                            pbuf[ndpkt].ui32[0] = vtiw[j][i][0];
                            pbuf[ndpkt].ui32[1] = vtiw[j][i][1];
                            pbuf[ndpkt].ui32[2] = vtiw[j][i][2] * 16;
                            pbuf[ndpkt++].ui32[3] = i > 1 ? 0 : 0x8000;
                        }
                    }
                }
            }

            pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;

            if (stop_effects == 0)
            {
                for (j = 0; j < 48; j++)
                {
                    if (rs[j].mscl > rs[j].scl + add * rs[j].spd)
                    {
                        rs[j].alp = (32.0f - (rs[j].scl * 32.0f) / rs[j].mscl);
                        rs[j].scl += add * rs[j].spd;
                    }
                    else
                    {
                        rs[j].scl = rs[j].mscl;
                        rs[j].alp = 0x0;
                    }
                }
            }
        }
    }
}

void SetEffSQTex(int n, float *v, int tp, float w, float h, u_char r, u_char g, u_char b, u_char a)
{
    u_char rr;
    u_char gg;
    u_char bb;
    int i;
    int tw;
    int th;
    float xx[2];
    float yy[2];
    u_long tx0;

    xx[0] = *v - w;
    xx[1] = *v + w;

    yy[0] = v[1] - h * 0.5f;
    yy[1] = v[1] + h * 0.5f;

    if (monochrome_mode != 0)
    {
        gg = (r + g + b) / 3;
        bb = gg;
        rr = gg;
    }
    else
    {
        rr = r;
        gg = g;
        bb = b;
    }

    tx0 = effdat[n].tex0;

    tw = effdat[n].w * 16;
    th = effdat[n].h * 16;

    Reserve2DPacket(0x1000);

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = DMAend | 20;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    pbuf[ndpkt].ul64[0] = tx0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, tp);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 340, SCE_GIF_PACKED, 3);
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0)
        | SCE_GS_UV    << (4 * 1)
        | SCE_GS_XYZF2 << (4 * 2);

    for (i = 0; i < 4; i++)
    {
        pbuf[ndpkt].ui32[0] = rr;
        pbuf[ndpkt].ui32[1] = gg;
        pbuf[ndpkt].ui32[2] = bb;
        pbuf[ndpkt++].ui32[3] = a;

        pbuf[ndpkt].ui32[0] = i % 2 ? tw - 8 : 8;
        pbuf[ndpkt].ui32[1] = i / 2 ? th - 8 : 8;
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt++].ui32[3] = 0;

        pbuf[ndpkt].ui32[0] = (int)(xx[i % 2] * 16.0f);
        pbuf[ndpkt].ui32[1] = (int)(yy[i / 2] * 16.0f);
        pbuf[ndpkt].ui32[2] = (int)(v[2] * 16.0f);
        pbuf[ndpkt++].ui32[3] = i > 1 ? 0 : 0x8000;
    }
}

void SetEffSQITex(int n, int *v, int tp, float w, float h, u_char r, u_char g, u_char b, u_char a)
{
    u_char rr;
    u_char gg;
    u_char bb;
    int i;
    int tw;
    int th;
    int xx[2];
    int yy[2];
    u_long tx0;
    int tp1;
    int tp2;
    int tp3;

    xx[0] = *v - (int)(w * 16.0f);
    xx[1] = *v + (int)(w * 16.0f);

    yy[0] = v[1] - (int)(h * 0.5f * 16.0f);
    yy[1] = v[1] + (int)(h * 0.5f * 16.0f);

    if (monochrome_mode != 0)
    {
        gg = (r + g + b) / 3;
        bb = gg;
        rr = gg;
    }
    else
    {
        rr = r;
        gg = g;
        bb = b;
    }

    tp1 = tp & 1;
    tp2 = (tp & 2) ? SCE_GS_ALPHA_ZERO : SCE_GS_ALPHA_CD;
    tp3 = (tp & 4) ? SCE_GS_ALPHA_ZERO : SCE_GS_ALPHA_CD;

    tx0 = effdat[n].tex0;

    tw = effdat[n].w * 16;
    th = effdat[n].h * 16;


    Reserve2DPacket(0x1000);

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = DMAend | 20;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    pbuf[ndpkt].ul64[0] = tx0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, tp1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, tp2, SCE_GS_ALPHA_AS, tp3, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 340, SCE_GIF_PACKED, 3);
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0)
        | SCE_GS_UV    << (4 * 1)
        | SCE_GS_XYZF2 << (4 * 2);

    for (i = 0; i < 4; i++)
    {
        pbuf[ndpkt].ui32[0] = rr;
        pbuf[ndpkt].ui32[1] = gg;
        pbuf[ndpkt].ui32[2] = bb;
        pbuf[ndpkt++].ui32[3] = a;

        pbuf[ndpkt].ui32[0] = i % 2 ? tw - 8 : 8;
        pbuf[ndpkt].ui32[1] = i / 2 ? th - 8 : 8;
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt++].ui32[3] = 0;

        pbuf[ndpkt].ui32[0] = xx[i % 2];
        pbuf[ndpkt].ui32[1] = yy[i / 2];
        pbuf[ndpkt].ui32[2] = v[2];
        pbuf[ndpkt++].ui32[3] = i > 1 ? 0 : 0x8000;
    }
}

void* CallFire(void *pos, u_char r, u_char g, u_char b, float scale)
{
    static float rate = 1.0f;
    return SetEffects(0x19 , 2, 1, pos, &rate, &rate);
}

void* CallFire2(void *pos, u_char r, u_char g, u_char b, float scl, u_char r2, u_char g2, u_char b2, float scl2)
{
    return SetEffects(0x17, 2, 3, pos, r, g, b, scl, r2, g2, b2, scl2);
}

void* CallFire3(void *pos, int type, u_char r, u_char g, u_char b, float scl, u_char r2, u_char g2, u_char b2, float scl2)
{
    return SetEffects(0x17, 2, type ? 0 : 3,pos, r, g, b, scl, r2, g2, b2, scl2);
}

void SubFire1(EFFECT_CONT *ec)
{
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0IVECTOR ipos;
    sceVu0IVECTOR ivec[4];
    sceVu0FVECTOR vpos;
    sceVu0FVECTOR vtw[4];
    sceVu0FVECTOR wpos[4] = {
        { -2.0f, +8.0f, +0.0f, +1.0f },
        { +2.0f, +8.0f, +0.0f, +1.0f },
        { -2.0f, +0.0f, +0.0f, +1.0f },
        { +2.0f, +0.0f, +0.0f, +1.0f }
    };
    static int pat = 0;
    static int rnbk = 0;
    static float scw = 1.0f;
    static float sch = 1.0f;
    u_long tx0;
    float msc;
    float msch;
    float f;
    float rot_x;
    float rot_y;
    int i;
    int n;
    int rn;
    int tw;
    int th;
    int w;
    int wpat;
    u_char mr;
    u_char mg;
    u_char mb;
    u_char mrh;
    u_char mgh;
    u_char mbh;
    float arate;
    float wscw;
    float wsch;
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 0, 1, 0, 1, 0, 0, 0, 0), SCE_GIF_PACKED, 3),
    };
    int mm; // not in STAB

    if (monochrome_mode != 0)
    {
        mg = (ec->dat.uc8[2] + ec->dat.uc8[3] + ec->dat.uc8[4]) / 3;
        mb = mg;
        mr = mg;
    }
    else
    {
        mr = ec->dat.uc8[2];
        mg = ec->dat.uc8[3];
        mb = ec->dat.uc8[4];
    }

    msc = ec->dat.fl32[2];

    mrh = ec->dat.uc8[5];
    mgh = ec->dat.uc8[6];
    mbh = ec->dat.uc8[7];

    msch = ec->dat.fl32[3];

    if (ec->dat.uc8[1] & 1)
    {
        wpat = pat;
        wscw = scw;
        wsch = sch;
    }
    else
    {
        wpat = ec->cnt;
        wscw = ec->fw[1];
        wsch = ec->fw[2];
    }

    arate = ec->fw[0];

    switch (ec->flow)
    {
    case 0:
        wpat = 0;
        wscw = 1.0f;
        wsch = 1.0f;

        ec->flow = 1;
    break;
    case 1:
        wscw += 9.0f;
        wsch += 12.0f;

        if (wpat >= 6)
        {
            ec->flow = 2;
        }
    break;
    case 2:
        f = 4.0f;
        wsch = wsch - f <= 25.0f ? 25.0f : wsch - f;
        wscw = wscw - f <= 25.0f ? 25.0f : wscw - f;

        if (wsch <= 25.0f && wscw <= 25.0f)
        {
            ec->flow = 3;
        }
    break;
    case 3:
        wscw = 25.0f;
        wsch = 25.0f;
    break;
    }

    if (stop_effects == 0)
    {
        wpat++;
    }

    n = 7;
    mm = monochrome_mode + 32; // mm is not in STAB!
    i = ((wpat / 3) % n) * 2 + mm; // mm is not in STAB!

    tx0 = effdat[i].tex0;

    tw = effdat[i].w * 16;
    th = effdat[i].h * 16;

    Vu0CopyVector(vpos, ec->pnt[0]);
    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);
    sceVu0UnitMatrix(wlm);

    wlm[0][0] = wlm[2][2] = wscw * msc;
    wlm[1][1] = wsch * msc;

    sceVu0RotMatrixX(wlm, wlm, PI);
    sceVu0RotMatrixY(wlm, wlm, rot_y);
    sceVu0TransMatrix(wlm, wlm, vpos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);

    w = 0;

    for (i = 0; i < 4; i++)
    {
        sceVu0RotTransPers(ivec[i], slm, wpos[i], 0);

        if (0x8000 < ivec[i][0] - 0x4000U)
        {
            w = 1;
        }

        if (0x8000 < ivec[i][1] - 0x4000U)
        {
            w = 1;
        }

        if (0xfffff00 < ivec[i][2] - 0xffU)
        {
            w = 1;
        }
    }

    if (w == 0)
    {
        ipos[0] = (ivec[0][0] + ivec[3][0]) / 2;
        ipos[1] = (ivec[0][1] - ivec[3][1]) * 0.3f + ivec[3][1];
        ipos[2] = ec->z;
        ipos[3] = 0;

        f = (((ivec[3][1] - ivec[0][1]) / 2) < ((ivec[3][0] - ivec[0][0]) / 2) ? ((ivec[3][0] - ivec[0][0]) / 2) : ((ivec[3][1] - ivec[0][1]) / 2)) * 0.0625f;

        if (stop_effects == 0)
        {
            rnbk = rn = (int)(vu0Rand() * 8.0f);
        }
        else
        {
            rn = rnbk;
        }

        SetEffSQITex(monochrome_mode + 0x16, ipos, 3, f * msch, f * msch, mrh, mgh, mbh, ((int)(rn / 2) + 5) * arate);
        SetEffSQITex(monochrome_mode + 0x16, ipos, 3, f * msch * 0.5f, f * msch * 0.5f, mrh, mgh, mbh, (rn + 2) * arate);

        Reserve2DPacket(0x1000);

        pbuf[ndpkt].ul128 = (u_long128)0;
        pbuf[ndpkt++].ui32[0] = DMAend | 20;

        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
        pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

        pbuf[ndpkt].ul64[0] = 0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

        pbuf[ndpkt].ul64[0] = tx0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
        pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
        pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 348, SCE_GIF_PACKED, 3);
        pbuf[ndpkt++].ul64[1] = 0 \
            | SCE_GS_RGBAQ << (4 * 0)
            | SCE_GS_UV    << (4 * 1)
            | SCE_GS_XYZF2 << (4 * 2);

        for (i = 0; i < 4; i++)
        {
            pbuf[ndpkt].ui32[0] = mr;
            pbuf[ndpkt].ui32[1] = mg;
            pbuf[ndpkt].ui32[2] = mb;
            pbuf[ndpkt++].ui32[3] = arate * 96.0f;

            pbuf[ndpkt].ui32[0] = i % 2 ? tw - 8 : 8;
            pbuf[ndpkt].ui32[1] = i / 2 ? th - 8 : 8;
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = ivec[i][0];
            pbuf[ndpkt].ui32[1] = ivec[i][1];
            pbuf[ndpkt].ui32[2] = ec->z;
            pbuf[ndpkt++].ui32[3] = i > 1 ? 0 : 0x8000;
        }
    }

    if (ec->dat.uc8[1] & 1)
    {
        pat = wpat;
        scw = wscw;
        sch = wsch;

        ResetEffects(ec);
    }
    else
    {
        ec->cnt = wpat;
        ec->fw[1] = wscw;
        ec->fw[2] = wsch;
    }
}
void SetFire(EFFECT_CONT *ec)
{
    static float alp = 1.0f;

    ec->fw[0] = 1.0f;

    if (ec->flow == 3)
    {
        SubFire1(ec);
    }
    else
    {
        SubRDFire(ec);
    }
}

void SetFire2(EFFECT_CONT *ec)
{
    SubFire1(ec);
}

void SetHalo(EFFECT_CONT *ec)
{
    SubHalo(ec->pnt[0], ec->dat.uc8[2], ec->dat.uc8[6], ec->z, ec->dat.uc8[3], ec->dat.uc8[4], ec->dat.uc8[5], 0x64, ec->dat.fl32[2]);

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SubHalo(float *p, int type, int textp, u_int z, u_char r, u_char g, u_char b, u_char alp, float sc)
{
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0IVECTOR ipos;
    sceVu0IVECTOR ivec[4];
    sceVu0FVECTOR vpos;
    sceVu0FVECTOR vtw[4];
    sceVu0FVECTOR wpos[4] = {
        {-4.0f, +4.0f, 0.0f, 1.0f},
        {+4.0f, +4.0f, 0.0f, 1.0f},
        {-4.0f, -4.0f, 0.0f, 1.0f},
        {+4.0f, -4.0f, 0.0f, 1.0f},
    };
    static int rnbk = 0;
    static float scw = 25.0f;
    static float sch = 25.0f;
    float f;
    float rot_x;
    float rot_y;
    int i;
    int rn;
    int w;
    int n;

    Vu0CopyVector(vpos, p);
    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);
    sceVu0UnitMatrix(wlm);

    wlm[0][0] = wlm[2][2] = scw * sc;
    wlm[1][1] = sch * sc;

    sceVu0RotMatrixX(wlm, wlm, rot_x);
    sceVu0RotMatrixY(wlm, wlm, rot_y);
    sceVu0TransMatrix(wlm, wlm, vpos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);

    w = 0;

    for (i = 0; i < 4; i++)
    {
        sceVu0RotTransPers(ivec[i], slm, wpos[i], 0);

        if (0x5000 < ivec[i][0] - 0x5800U)
        {
            w = 1;
        }

        if (0x1c00 < ivec[i][1] - 0x7200U)
        {
            w = 1;
        }

        if (0xffff00 < ivec[i][2] - 0xffU)
        {
            w = 1;
        }
    }

    if (!w)
    {
        ipos[0] = (ivec[0][0] + ivec[3][0]) / 2;
        ipos[1] = (ivec[0][1] + ivec[3][1]) / 2;
        ipos[2] = (ivec[0][2] + ivec[3][2]) / 2;
        ipos[3] = 0;

        n = (((ivec[3][1] - ivec[0][1]) / 2) < ((ivec[3][0] - ivec[0][0]) / 2) ? ((ivec[3][0] - ivec[0][0]) / 2) : ((ivec[3][1] - ivec[0][1]) / 2));
        f = n * 0.0625f;

        if (stop_effects == 0)
        {
            if ((type & 1) == 0)
            {
                n = vu0Rand() * 9.0f;
                rnbk = rn = n;
            }
            else
            {
                n = (int)(vu0Rand() * 6.0f + 3.0f);
                rnbk = rn = n;
            }
        }
        else
        {
            rn = rnbk;
        }

        SetEffSQITex((textp * 2 + 0x16) + monochrome_mode, ipos, 1, f * 2, f * 2, r, g, b, ((int)((rn / 2 + 7) * alp) / 100));
        SetEffSQITex((textp * 2 + 0x16) + monochrome_mode, ipos, 1, f, f, r, g, b, ((int)((rn + 3) * alp) / 100));

    }
}

void InitHeatHaze()
{
    int i;

    for (i = 0; i < 3; i++)
    {
        if (ene_particle[i].flag & 0x80)
        {
            ene_particle[i].flag &= 0x7f;
        }
        else
        {
            ene_particle[i].flag = 0;
        }
    }

    for (i = 0; i < 1; i++)
    {
        if (amu_particle[i].flag & 0x80)
        {
            amu_particle[i].flag &= 0x7f;
        }
        else
        {
            amu_particle[i].flag = 0;
        }
    }

    for (i = 0; i < 5; i++)
    {
        if (torch_particle[i].flag & 0x80)
        {
            torch_particle[i].flag &= 0x7f;
        }
        else
        {
            torch_particle[i].flag = 0;
        }
    }

    for (i = 0; i < 4; i++)
    {
        if (smoke_particle[i].flag & 0x80)
        {
            smoke_particle[i].flag &= 0x7f;
        }
        else
        {
            smoke_particle[i].flag = 0;
        }
    }
}

int draw_distortion_particles(sceVu0FMATRIX *local_screen, sceVu0FMATRIX *local_clip, int fr, int t_particles, void *particles, int size_of_particle, float psize, float distortion_amount, int type)
{
    int i;
    int n;
    int num;
    u_long *d;
    u_long areg;
    u_long treg;
    float y_correction = 0.0f;
    float rr;
    float gg;
    float bb;
    sceVu0FVECTOR warp_add = { distortion_amount + y_correction, 0.0f, 0.0f, 0.0f };
    sceVu0FVECTOR screen_size = { 639.0f, 223.0f, 0.0f, 0.0f };
    sceVu0FVECTOR particle_size = { psize, psize * 0.5f, 0.0f, 0.0f };
    sceVu0FVECTOR *p;
    int dtex;
    sceVu0FVECTOR ones = { 1.0f, 1.0f, 1.0f, 0.0f };
    sceVu0FVECTOR st_add = { -1728.0f, -1936.0f, 0.0f, 0.0f };
    sceVu0FVECTOR st_scale = { 1.0 / 1024.0f, 1.0f / 256.0f, 0.0f, 0.0f };
    unsigned int clip_flags;

    p = (sceVu0FVECTOR *)particles;

    switch (type)
    {
    case 9:
    case 10:
    case 11:
        dtex = 1;
    break;
    case 8:
    case 12:
    case 13:
    default:
        dtex = 0;
    break;
    }

    if (dtex != 0)
    {
        switch (type)
        {
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
            LocalCopyLtoL((sys_wrk.count & 1) * 0x8c0, 0x1a40);
        break;
        }
    }

    switch (type)
    {
    case 9:
        treg = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS);
    break;
    case 8:
    case 10:
    case 11:
    case 12:
    case 13:
        treg = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    break;
    }

    switch (type)
    {
    case 13:
        areg = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    break;
    case 8:
    case 9:
    case 10:
        areg = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    break;
    case 11:
    case 12:
        areg = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    break;
    }

    Reserve2DPacket(0x1000);

    d = (u_long *)&pbuf[ndpkt];

    n = 0;

    d[n++] = DMAend;
    d[n++] = 0;

    d[n++] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    d[n++] = SCE_GIF_PACKED_AD;

    d[n++] = 0;
    d[n++] = SCE_GS_TEXFLUSH;

    d[n++] = SCE_GS_SET_TEX0_1(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 0);
    d[n++] = SCE_GS_TEX0_1;

    d[n++] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    d[n++] = SCE_GS_TEX1_1;

    d[n++] = treg;
    d[n++] = SCE_GS_TEST_1;

    d[n++] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    d[n++] = SCE_GS_ZBUF_1;

    d[n++] = areg;
    d[n++] = SCE_GS_ALPHA_1;

    asm __volatile__ (
        "lqc2 $vf4,0(%0)\n"
        "lqc2 $vf5,0x10(%0)\n"
        "lqc2 $vf6,0x20(%0)\n"
        "lqc2 $vf7,0x30(%0)\n"
        "lqc2 $vf28,0(%1)\n"
        "lqc2 $vf29,0x10(%1)\n"
        "lqc2 $vf30,0x20(%1)\n"
        "lqc2 $vf31,0x30(%1)\n"
        : :"r"(local_screen),"r"(local_clip)
    );

    asm __volatile__ (
        "lqc2 $vf3,0(%0)\n"
        "lqc2 $vf1,0(%1)\n"
        "lqc2 $vf25,0(%2)\n"
        "lqc2 $vf26,0(%3)\n"
        "lqc2 $vf19,0(%4)\n"
        "lqc2 $vf24,0(%5)\n"
        : :"r"(particle_size),"r"(warp_add),"r"(st_add),"r"(st_scale),"r"(ones),"r"(screen_size)
    );

    num = 0;

    for (i = 0; i < t_particles; i++, p = (sceVu0FVECTOR *)((u_char *)p + size_of_particle))
    {
        if (p[1][3] > 0.0f)
        {
            num++;

            rr = p[1][0];
            gg = p[1][1];
            bb = p[1][2];

            if (monochrome_mode != 0)
            {
                p[1][0] = p[1][1] = p[1][2] = (rr + gg + bb) / 3.0f;
            }

            asm __volatile__ (
                "lqc2 $vf8, 0x0(%1)\n"
                "lqc2 $vf22, 0x10(%1)\n"
                "vmulax.xyzw  ACC, $vf28, $vf8x\n"
                "vmadday.xyzw ACC, $vf29, $vf8y\n"
                "vmaddaz.xyzw ACC, $vf30, $vf8z\n"
                "vmaddw.xyzw  $vf12, $vf31, $vf8w\n"
                "vclipw.xyz   $vf12xyz,$vf12w\n"
                "vnop\n"
                "vnop\n"
                "vnop\n"
                "vnop\n"
                "vnop\n"
                "cfc2 %0, $vi18\n"
                :"=r"(clip_flags) :"r"(p)
            );

            if ((clip_flags & 0x3f) == 0)
            {
                asm __volatile__ (
                    "vmulax.xyzw  ACC, $vf4, $vf8x\n"
                    "vmadday.xyzw ACC, $vf5, $vf8y\n"
                    "vmaddaz.xyzw ACC, $vf6, $vf8z\n"
                    "vmaddw.xyzw $vf12, $vf7, $vf8w\n"
                    "vdiv Q, $vf0w, $vf12w\n"
                    "vwaitq\n"
                    "vmulq.xyzw $vf12, $vf12, Q\n"
                    "vmulq.xyz $vf9, $vf3, Q\n"
                    "vadd.xyzw $vf14, $vf12, $vf25\n"
                    "vadd.xyzw $vf27, $vf14, $vf1\n"
                    "vsub.xyzw $vf13, $vf27, $vf9\n"
                    "vsub.xyzw $vf13, $vf13, $vf19\n"
                    "vmax.xyzw $vf13, $vf13, $vf0\n"
                    "vadd.xyzw $vf14, $vf27, $vf9\n"
                    "vadd.xyzw $vf14, $vf14, $vf19\n"
                    "vmini.xyzw $vf14, $vf14, $vf24\n"
                );

                //                                                    prim 0x4d or 0x5d depending on dtex being 0 or 1
                d[n++] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, (u_long)dtex << 4 | 0x4d, SCE_GIF_PACKED, 14);

                d[n++] = 0 \
                    | (long)SCE_GS_RGBAQ << (4 * 0)
                    | (long)SCE_GS_ST    << (4 * 1)
                    | (long)SCE_GS_XYZF2 << (4 * 2)
                    | (long)SCE_GS_RGBAQ << (4 * 3)
                    | (long)SCE_GS_ST    << (4 * 4)
                    | (long)SCE_GS_XYZF2 << (4 * 5)
                    | (long)SCE_GS_ST    << (4 * 6)
                    | (long)SCE_GS_XYZF2 << (4 * 7)
                    | (long)SCE_GS_ST    << (4 * 8)
                    | (long)SCE_GS_XYZF2 << (4 * 9)
                    | (long)SCE_GS_ST    << (4 * 10)
                    | (long)SCE_GS_XYZF2 << (4 * 11)
                    | (long)SCE_GS_ST    << (4 * 12)
                    | (long)SCE_GS_XYZF2 << (4 * 13);

                asm __volatile__ (
                    "vftoi0.xyzw $vf22, $vf22\n"
                    "sqc2 $vf22, 0x0(%0)\n"
                    "vmove.w $vf22, $vf19\n"
                    "sqc2 $vf22, 0x30(%0)\n"
                    "vmove.w $vf9, $vf19\n"
                    "vmul.xyzw $vf8, $vf9, $vf26\n"
                    "vmove.xyzw $vf14, $vf8\n"
                    "vmove.y $vf8, $vf0\n"
                    "vmove.x $vf14, $vf0\n"
                    "vmove.xyzw $vf22, $vf9\n"
                    "vmove.y $vf9, $vf0\n"
                    "vmove.x $vf22, $vf0\n"
                    "vmove.z $vf27, $vf19\n"
                    "vmul.xy $vf27, $vf27, $vf26\n"
                    "sqc2 $vf27, 0x10(%0)\n"
                    "vftoi4.xyzw $vf13, $vf12\n"
                    "sqc2 $vf13, 0x20(%0)\n"
                    "vsub.xyzw $vf13, $vf27, $vf14\n"
                    "sqc2 $vf13, 0x40(%0)\n"
                    "vsub.xyzw $vf10, $vf12, $vf22\n"
                    "vftoi4.xyzw $vf13, $vf10\n"
                    "sqc2 $vf13, 0x50(%0)\n"
                    "vadd.xyzw $vf13, $vf27, $vf8\n"
                    "sqc2 $vf13, 0x60(%0)\n"
                    "vadd.xyzw $vf10, $vf12, $vf9\n"
                    "vftoi4.xyzw $vf13, $vf10\n"
                    "sqc2 $vf13, 0x70(%0)\n"
                    "vadd.xyzw $vf13, $vf27, $vf14\n"
                    "sqc2 $vf13, 0x80(%0)\n"
                    "vadd.xyzw $vf10, $vf12, $vf22\n"
                    "vftoi4.xyzw $vf13, $vf10\n"
                    "sqc2 $vf13, 0x90(%0)\n"
                    "vsub.xyzw $vf13, $vf27, $vf8\n"
                    "sqc2 $vf13, 0xa0(%0)\n"
                    "vsub.xyzw $vf13, $vf12, $vf9\n"
                    "vftoi4.xyzw $vf13, $vf13\n"
                    "sqc2 $vf13, 0xb0(%0)\n"
                    "vsub.xyzw $vf13, $vf27, $vf14\n"
                    "sqc2 $vf13, 0xc0(%0)\n"
                    "vsub.xyzw $vf13, $vf12, $vf22\n"
                    "vftoi4.xyzw $vf13, $vf13\n"
                    "sqc2 $vf13, 0xd0(%0)\n"
                    : :"r"(&d[n])
                );

                n += 14 * 2;
            }

            p[1][0] = rr;
            p[1][1] = gg;
            p[1][2] = bb;
        }
    }

    d[0] = DMAend + (n - 1) / 2;

    ndpkt = ndpkt + (n + 1) / 2;

    return num;
}

void add_particle(int type, HEAT_HAZE *hh, float *pos, float *vel, float r, float g, float b, float a)
{
    float oolife;
    PARTICLE *p;

    p = &hh->particles[hh->head];

    set_vect(p->position, pos[0], pos[1], pos[2], 1.0f);
    set_vect(p->velocity, vel[0], vel[1], vel[2], 0.0f);

    oolife = a * (1.0f / hh->blife);

    set_vect(p->color, r, g, b, a);

    p->alp_step = -oolife;
    p->lifetime = hh->blife;

    hh->head = (hh->head + 1) % 200;
}

void update_particles(PARTICLE *prt)
{
    int i;

    for (i = 0; i < 200; i++, prt++)
    {
        if (prt->lifetime)
        {
            prt->position[0] += prt->velocity[0];
            prt->position[1] += prt->velocity[1];
            prt->position[2] += prt->velocity[2];
            prt->color[3] += prt->alp_step;
            prt->velocity[2] += 0.0f;
            prt->lifetime--;
        }
        else
        {
            prt->color[3] = 0.0f;
        }
    }
}

void add_particle2(int type, HEAT_HAZE *hh, float *pos, float *vel, float r, float g, float b, float a)
{
    PARTICLE *p;

    p = &hh->particles[hh->head];

    set_vect(p->position, pos[0], pos[1], pos[2], 1.0f);
    set_vect(p->velocity, vel[0], vel[1], vel[2], 0.0f);

    set_vect(p->color, r, g, b, 0.0f);

    p->alp_step = a;
    p->lifetime = hh->blife;

    hh->head = (hh->head + 1) % 200;
}

void update_particles2(HEAT_HAZE *hh, float lng, float arate)
{
    int i;
    float a;
    float f;
    float f2;
    float center;

    center = 0.9f;

    if (lng > 800.0f)
    {
        a = arate;
    }
    else
    {
        a = lng < 500.0f ? 0.0f : ((lng - 500.0f) / 300.0f) * arate;
    }


    for (i = 0; i < 200; i++)
    {
        PARTICLE *p;

        p = &hh->particles[i];

        if (p->lifetime != 0)
        {
            p->position[0] += p->velocity[0];
            p->position[1] += p->velocity[1];
            p->position[2] += p->velocity[2];

            f = (hh->blife - p->lifetime) / (float)hh->blife;

            f2 = 1.0f - center;

            if (f <= center)
            {
                p->color[3] = ((p->alp_step * f) / center) * a;
            }
            else
            {
                p->color[3] =  ((p->alp_step * (1.0f - f)) / f2) * a;
            }
            p->lifetime--;
        }
        else
        {
            p->color[3] = 0.0f;
        }
    }
}

void sceVu0RotCameraMatrix(sceVu0FMATRIX m, float *p, float *zd, float *yd, float *rot)
{
    sceVu0FMATRIX work;
    sceVu0FVECTOR direction;
    sceVu0FVECTOR vertical;
    sceVu0FVECTOR position;

    sceVu0UnitMatrix(work);
    sceVu0RotMatrixX(work, work, rot[0]);
    sceVu0RotMatrixY(work, work, rot[1]);
    sceVu0RotMatrixZ(work, work, rot[2]);
    sceVu0ApplyMatrix(direction, work, zd);
    sceVu0ApplyMatrix(vertical, work, yd);
    sceVu0ApplyMatrix(position, work, p);
    sceVu0CameraMatrix(m, position, direction, vertical);
}

void sceVu0ViewClipMatrix(sceVu0FMATRIX vc, float scrw, float scrh, float scrz, float zmin, float zmax)
{
    sceVu0UnitMatrix(vc);
    vc[0][0] = (scrz + scrz) / scrw;
    vc[1][1] = (scrz + scrz) / scrh;
    vc[2][2] = (zmax + zmin) / (zmax - zmin);
    vc[3][2] = (zmax * -2.0f * zmin) / (zmax - zmin);
    vc[2][3] = 1.0f;
    vc[3][3] = 0.0f;
}

void* ContHeatHaze(void *addr, int type, float *pos, float *pos2, int st, float r, float g, float b, float a, float size, float arate)
{
    static float pcnt1;
    static float pcnt2;
    int n1;
#ifdef MATCHING_DECOMP
    register int n2 asm("s6");
#else
    int n2;
#endif
    int i;
    float f;
    float fw1;
    float lng;
    float escl;
    sceVu0FVECTOR wpos;
    sceVu0FVECTOR camera_p = {0.0f, 0.0f, -12.0f, 0.0f};
    sceVu0FVECTOR camera_zd = {0.0f, 0.0f, 1.0f, 1.0f};
    sceVu0FVECTOR camera_yd = {0.0f, 1.0f, 0.0f, 1.0f};
    sceVu0FVECTOR camera_rot = {PI, 0.0f, 0.0f, 0.0f};
    sceVu0FVECTOR obj_trans = {0.0f, 0.0f, 0.0f, 0.0f};
    sceVu0FVECTOR obj_rot = {PI/16.0f, 0.0f, 0.0f, 0.0f};
    sceVu0FMATRIX local_clip;
    sceVu0FMATRIX view_clip;
    sceVu0FMATRIX local_world;
    sceVu0FMATRIX world_view;
    sceVu0FMATRIX view_screen;
    sceVu0FMATRIX local_screen;
    sceVu0FMATRIX work;
    sceVu0FVECTOR ppos = {0.0f, 0.0f, 0.0f, 1.0f};
    sceVu0FVECTOR pvel = {0.0f, 0.0f, 0.0f, 0.0f};
    sceVu0FVECTOR rot;
    HEAT_HAZE *hh;

    n1 = 25;
    n2 = 12;

    escl = 3.8f;

    if (addr == NULL)
    {
        return NULL;
    }

    hh = (HEAT_HAZE *)addr;
    hh->flag |= 0xff;

    if (ingame_wrk.mode == 0xa)
    {
        return addr;
    }

    if (type == 0)
    {
        return addr;
    }

    if (init_hhaze != 0)
    {
        pcnt1 = 0.0f;
        pcnt2 = 0.0f;

        for (i = 0; i < 3; i++)
        {
            ene_particle[i].flag = 0;
            ene_particle[i].cnt = 0;
        }

        for (i = 0; i < 1; i++)
        {
            amu_particle[i].flag = 0;
            amu_particle[i].cnt = 0;
        }

        init_hhaze = 0;
    }

    switch (type)
    {
    case 9:
        GetTrgtRot(camera.p, camera.i, rot, 3);
        sceVu0UnitMatrix(work);

        work[0][0] = work[1][1] = work[2][2] = camera.fov * 25.0f;

        sceVu0RotMatrixX(work, work, rot[0]);
        sceVu0RotMatrixY(work, work, rot[1]);
        sceVu0TransMatrix(local_world, work, pos);
        sceVu0MulMatrix(local_screen, SgWSMtx, local_world);
        sceVu0MulMatrix(local_clip, SgCMtx, local_world);
    break;
    case 8:
    case 10:
    case 11: {
        float fx;
        float fy;
        float fz;
        float rx;
        float rz;

        if (pos2 == NULL)
        {
            rx = 0.0f;
            rz = 0.0f;
        }
        else
        {
            Get2PosRot2(pos2, pos, &rx, &rz);
        }

        fx = (pos[0] - camera.p[0]) * (pos[0] - camera.p[0]);
        fy = (pos[1] - camera.p[1]) * (pos[1] - camera.p[1]);
        fz = (pos[2] - camera.p[2]) * (pos[2] - camera.p[2]);

        lng = SgSqrtf(fx + fy + fz);

        sceVu0UnitMatrix(work);

        work[0][0] = work[1][1] = work[2][2] = 25.0f;

        sceVu0RotMatrixX(work, work, rx);
        sceVu0RotMatrixZ(work, work, -rz);
        sceVu0TransMatrix(local_world, work, pos);
        sceVu0MulMatrix(local_screen, SgWSMtx, local_world);
        sceVu0MulMatrix(local_clip, SgCMtx, local_world);
    } break;
    }

    switch (type)
    {
    case 8:
        Vu0CopyVector(wpos, pos);
        wpos[1] -= 10.0f;

#ifdef MATCHING_DECOMP
        // matches most of the asm generated by the code
        // that has been optimized out by the compiler.
        // needs patching to fix 2 asm instructions
        {
            int a = hh->cnt / n1;

            asm volatile ("mfhi $3");
            asm volatile ("andi $2,$2,1");

            if (hh->cnt / n1 <= n2 && hh->cnt / n1)
            {
                n2 = ~n2;
            }
        }
#endif

        SubHalo(wpos, 1, 0, 0, 0x28, 0x28, 0x6e, 0x6e, 0.6f);

        hh->disp = draw_distortion_particles(&local_screen, &local_clip, sys_wrk.count % 2, 200, hh->particles, 0x40, (size * escl) / camera.fov, -1.0f, type);
    break;
    case 9:
        Vu0CopyVector(wpos, pos);
        wpos[1] -= 20.0f;

        SubHalo(wpos, 0, 0, 0, 0x30, 0x30, 0xff, (hh->disp * 80.0f) / hh->max, (hh->disp * 0.5f) / hh->max);

        hh->disp = draw_distortion_particles(&local_screen, &local_clip, sys_wrk.count % 2, 200, hh->particles, 0x40, size / camera.fov, -1.0f, type);
    break;
    case 10: {
        float fx;
        float fy;
        float fz;
        float l;
        float rate;

        Vu0CopyVector(wpos, pos);
        wpos[1] -= 10.0f;

        fx = (pos[0] - camera.p[0]) * (pos[0] - camera.p[0]);
        fy = (pos[1] - camera.p[1]) * (pos[1] - camera.p[1]);
        fz = (pos[2] - camera.p[2]) * (pos[2] - camera.p[2]);

        l = SgSqrtf(fx + fy + fz);

        rate = 600.0f < l ? 2.0f : (l < 300.0f ? 8.0f : ((600.0f - l) * 6.0f) / 300.0f + 2.0f);
        fw1 = SgSinf(pcnt1) * rate;

        f = vu0Rand() * (PI / 360.0f);
        pcnt1 = pcnt1 + f > PI ? pcnt1 + f - PI2 : pcnt1 + f;

        hh->disp = draw_distortion_particles(&local_screen, &local_clip, sys_wrk.count % 2, 200, hh->particles, 0x40, (size * escl) / camera.fov, fw1, type);
    } break;
    case 11: {
        float fx;
        float fy;
        float fz;
        float l;
        float rate;

        Vu0CopyVector(wpos, pos);
        wpos[1] -= 10.0f;

        fx = (pos[0] - camera.p[0]) * (pos[0] - camera.p[0]);
        fy = (pos[1] - camera.p[1]) * (pos[1] - camera.p[1]);
        fz = (pos[2] - camera.p[2]) * (pos[2] - camera.p[2]);

        l = SgSqrtf(fx + fy + fz);

        rate = 600.0f < l ? 4.0f : (l < 300.0f ? 12.0f : ((600.0f - l) * 8.0f) / 300.0f + 4.0f);
        fw1 = SgSinf(pcnt1) * rate;

        f = vu0Rand() * (PI / 180.0f);
        pcnt1 = pcnt1 + f > PI ? pcnt1 + f - PI2 : pcnt1 + f;

        hh->disp = draw_distortion_particles(&local_screen, &local_clip, sys_wrk.count % 2, 200, hh->particles, 0x40, (size * escl) / camera.fov, fw1, type);
    } break;
    }

    if (stop_effects != 0)
    {
        return addr;
    }

    if (st == 0)
    {
        switch (type)
        {
        case 8:
            if ((hh->cnt / n1 % 2) == 0 && (hh->cnt % 2) == 0)
            {
                ppos[0] = (vu0Rand() - 0.5f) * 1.3f;
                ppos[1] = 0.0f;
                ppos[2] = (vu0Rand() - 0.5f) * 1.3f;

                pvel[0] = (vu0Rand() - 0.5f) * 0.01;
                pvel[1] = -(vu0Rand() * 0.04f) - 0.01f;
                pvel[2] = (vu0Rand() - 0.5f) * 0.01;

                add_particle(type, hh, ppos, pvel, r, g, b, a);
            }
        break;
        case 9:
            for (i = 0; i < 1; i++)
            {
                ppos[0] = (vu0Rand() - 0.5f) * 1.3f;
                ppos[1] = 0.0f;
                ppos[2] = (vu0Rand() - 0.5f) * 1.3f;

                pvel[0] = (vu0Rand() - 0.5f) * 0.01f;
                pvel[1] = -(vu0Rand() * 0.02f) - 0.02f;
                pvel[2] = (vu0Rand() - 0.5f) * 0.01f;

                add_particle(type, hh, ppos, pvel, r, g, b, a);
            }
        break;
        case 10:
            if ((hh->cnt % 2) == 0)
            {
                ppos[0] = ((vu0Rand() - 0.5f) * 1.5f) * escl;
                ppos[1] = 0.0f;
                ppos[2] = ((vu0Rand() - 0.5f) * 1.5f) * escl;

                pvel[0] = ((vu0Rand() - 0.5f) * 0.015f) * escl;
                pvel[1] = (-(vu0Rand() * 0.02f) - 0.04f) * escl;
                pvel[2] = ((vu0Rand() - 0.5f) * 0.015f) * escl;

                add_particle2(type, hh, ppos, pvel, r, g, b, a);
            }
        break;
        case 11:
            if ((hh->cnt % 4) == 0)
            {
                ppos[0] = ((vu0Rand() - 0.5f) * 1.5f) * escl;
                ppos[1] = 0.0f;
                ppos[2] = ((vu0Rand() - 0.5f) * 1.5f) * escl;

                pvel[0] = ((vu0Rand() - 0.5f) * 0.01f) * escl;
                pvel[1] = (-(vu0Rand() * 0.02f) - 0.01f) * escl;
                pvel[2] = ((vu0Rand() - 0.5f) * 0.01f) * escl;

                add_particle2(type, hh, ppos, pvel, r, g, b, a);
            }
        break;
        }

        hh->cnt++;
    }

    switch (type)
    {
    case 8:
    case 9:
        {
            if (st != 0)
            {
                update_particles(hh->particles);
                update_particles(hh->particles);
                update_particles(hh->particles);
                update_particles(hh->particles);
            }

            update_particles(hh->particles);
        }
    break;
    case 10:
    case 11:
        {
            if (st == 0)
            {
                update_particles2(hh, lng, arate);
            }
            else
            {
                update_particles2(hh, lng, arate);
                update_particles2(hh, lng, arate);
                update_particles2(hh, lng, arate);
                update_particles2(hh, lng, arate);
                update_particles2(hh, lng, arate);
            }
        }
    }
    return addr;
}

void SetPartInit(HEAT_HAZE *addr, int type, int lifetime)
{
    int j;

    for (j = 0; j < 200; j++)
    {
        PARTICLE *p;

        p = &((HEAT_HAZE *)addr)->particles[j];

        set_vect(p->position, 0.0f, 0.0f, 0.0f, 1.0f);

        p->color[3] = 0.0f;
        p->lifetime = 0;
    }
    addr->head = 0;
    addr->flag = 1;
    addr->cnt = 0;
    addr->disp = 0;

    switch(type)
    {
    case 8:
        addr->max = 100;
        addr->blife = 200;
    break;
    case 12:
        addr->max = lifetime * 2;
        addr->blife = lifetime;
    break;
    case 13:
        addr->max = lifetime / 8;
        addr->blife = lifetime;
    break;
    case 9:
        addr->max = 200;
        addr->blife = lifetime;
    break;
    case 10:
        addr->max = lifetime / 2;
        addr->blife = lifetime;
    break;
    case 11:
        addr->max = 75;
        addr->blife = 300;
    break;
    }
}

void* GetItemPartAddr(void *addr, int type, int lifetime)
{
    return;
}

void* GetEnePartAddr(void *addr, int type, int lifetime)
{
    int i;
    int n;
    void *ret;

    if (addr == NULL)
    {
        ret = NULL;

        i = 0;
        n = 0;

        while (i < 3 && n == 0)
        {
            if (ene_particle[i].flag == 0)
            {
                SetPartInit(&ene_particle[i], type, lifetime);
                ret = &ene_particle[i];
                n = 1;
            }

            i++;
        }
    }
    else
    {
        ret = addr;
    }

    return ret;
}

void* GetAmuPartAddr(void *addr, int type, int lifetime)
{
    int i;
    int n;
    void *ret;

    if (addr == NULL)
    {
        ret = NULL;

        i = 0;
        n = 0;

        while (i < 1 && n == 0)
        {
            if (amu_particle[i].flag == 0)
            {
                SetPartInit(&amu_particle[i], type, lifetime);
                ret = &amu_particle[i];
                n = 1;
            }

            i++;
        }
    }
    else
    {
        ret = addr;
    }

    return ret;
}

void* GetTorchPartAddr(void *addr, int type, int lifetime)
{
    int i;
    int n;
    void *ret;

    if (addr == NULL)
    {
        ret = NULL;

        i = 0;
        n = 0;

        while (i < 5 && n == 0)
        {
            if (torch_particle[i].flag == 0)
            {
                printf("Set Torch work no = [%d]\n", i);
                SetPartInit(&torch_particle[i], type, lifetime);
                ret = &torch_particle[i];
                n = 1;
            }

            i++;
        }
    }
    else
    {
        ret = addr;
    }

    return ret;
}

void* GetSmokePartAddr(void *addr, int type, int lifetime)
{
    int i;
    int n;
    void *ret;
    float r;

    if (addr == NULL)
    {
        ret = NULL;

        i = 0;
        n = 0;

        while (i < 4 && n == 0)
        {
            if (smoke_particle[i].flag == 0)
            {
                SetPartInit(&smoke_particle[i], type, lifetime);
                smoke_particle[i].cnt = (int)(vu0Rand() * 360.0f);
                ret = &smoke_particle[i];
                n = 1;
            }

            i++;
        }
    }
    else
    {
        ret = addr;
    }

    return ret;
}

void SetNewItemEffect(NEW_ITEM *nip)
{
    int flow;
    int flg;
    float rot_z;
    float cnt;
    float wait;
    float out;
    float rot_x;
    float rot_y;
    float size;
    sceVu0FVECTOR wpos;
    sceVu0FMATRIX wlm[5];
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 84, SCE_GIF_PACKED, 3),
    };

    Vu0CopyVector(wpos, nip->pos);

    flow = nip->flow;
    cnt = nip->cnt;
    rot_z = nip->rot;

    flg = GetCornHitCheck(wpos, 1200.0f);

    if (flg == 0)
    {
        if (stop_effects == 0)
        {
            rot_z = PI < rot_z + DEG2RAD(10.0f/3.0f) ? (rot_z + DEG2RAD(10.0f/3.0f)) - PI2 : rot_z + DEG2RAD(10.0f/3.0f);
        }

        wait = 60.0f;
        out = 30.0f;

        switch (flow)
        {
        case 0:
            cnt = cnt > wait ? wait : cnt;
        break;
        case 1:
            cnt = cnt > out ? out : cnt;
        break;
        case 2:
            cnt = cnt > out ? out : cnt;
        break;
        }
    }
    else
    {
        if (stop_effects == 0)
        {
            rot_z = PI < rot_z + DEG2RAD(5) ? (rot_z + DEG2RAD(5)) - PI2 : rot_z + DEG2RAD(5);
        }

        wait = 10.0f;
        out = 20.0f;

        switch (flow)
        {
        case 0:
            cnt = cnt > wait ? wait : cnt;
        break;
        case 1:
            cnt = cnt > out ? out : cnt;
        break;
        case 2:
            cnt = cnt > out ? out : cnt;
        break;
        }
    }

    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

    switch (flow)
    {
    case 0:
        size = 0;

        if (wait <= cnt)
        {
            cnt = 0.0f;
            flow = 1;
        }
        else if (stop_effects == 0)
        {
            cnt += 1.0f;
        }
    break;
    case 1:
        size = cnt / out;
        if (out <= cnt)
        {
            cnt = 0.0f;
            flow = 2;
        }
        else if (stop_effects == 0)
        {
            cnt += 1.0f;
        }
    break;
    case 2:
        size = (out - cnt) / out;
        if (out <= cnt)
        {
            cnt = 0.0f;
            flow = 0;
        }
        else if (stop_effects == 0)
        {
            cnt += 1.0f;
        }
    break;
    }

    if (flg == 0)
    {
        sceVu0UnitMatrix(wlm[0]);
        sceVu0RotMatrixX(wlm[0], wlm[0], rot_x);
        sceVu0RotMatrixY(wlm[0], wlm[0], rot_y);
        sceVu0TransMatrix(wlm[0], wlm[0], wpos);

        sceVu0UnitMatrix(wlm[1]);
        sceVu0RotMatrixZ(wlm[1], wlm[1], rot_z);
        sceVu0RotMatrixX(wlm[1], wlm[1], rot_x);
        sceVu0RotMatrixY(wlm[1], wlm[1], rot_y);
        sceVu0TransMatrix(wlm[1], wlm[1], wpos);

        Set3DPosTexure(wlm[1], &de, 0x10, size * 120.0f, size * 120.0f, 0xb4, 0xb4, 0xb4, 0x50);
        Set3DPosTexure(wlm[0], &de, 0x12, 54.0f, 54.0f, 0xb4, 0xb4, 0xb4, size * 48.0f);
    }
    else
    {
        sceVu0UnitMatrix(wlm[0]);
        sceVu0RotMatrixX(wlm[0], wlm[0], rot_x);
        sceVu0RotMatrixY(wlm[0], wlm[0], rot_y);
        sceVu0TransMatrix(wlm[0], wlm[0], wpos);

        sceVu0UnitMatrix(wlm[1]);
        sceVu0RotMatrixZ(wlm[1], wlm[1], rot_z);
        sceVu0RotMatrixX(wlm[1], wlm[1], rot_x);
        sceVu0RotMatrixY(wlm[1], wlm[1], rot_y);
        sceVu0TransMatrix(wlm[1], wlm[1], wpos);

        sceVu0UnitMatrix(wlm[2]);
        sceVu0RotMatrixX(wlm[2], wlm[2], rot_z);
        sceVu0RotMatrixY(wlm[2], wlm[2], DEG2RAD(45));
        sceVu0TransMatrix(wlm[2], wlm[2], wpos);

        sceVu0UnitMatrix(wlm[3]);
        sceVu0RotMatrixY(wlm[3], wlm[3], rot_z);
        sceVu0RotMatrixZ(wlm[3], wlm[3], DEG2RAD(45));
        sceVu0TransMatrix(wlm[3], wlm[3], wpos);

        sceVu0UnitMatrix(wlm[4]);
        sceVu0RotMatrixX(wlm[4], wlm[4], DEG2RAD(45));
        sceVu0RotMatrixZ(wlm[4], wlm[4], rot_z);
        sceVu0TransMatrix(wlm[4], wlm[4], wpos);

        Set3DPosTexure(wlm[1], &de, 0x10, size * 150.0f, size * 150.0f, 0xff, 0xff, 0xff, 0x50);
        Set3DPosTexure(wlm[2], &de, 0x12, 78.0f, 78.0f, 0xf0, 0xf0, 0xff, size * 64);
        Set3DPosTexure(wlm[3], &de, 0x12, 78.0f, 78.0f, 0xf0, 0xff, 0xf0, size * 64);
        Set3DPosTexure(wlm[4], &de, 0x12, 78.0f, 78.0f, 0xff, 0xf0, 0xf0, size * 64);
    }

    nip->flow = flow;
    nip->cnt = cnt;
    nip->rot = rot_z;
}

void CheckItemEffect()
{
    int i;
    int j;
    int n;
    int num;
    int item_heat_num[24];
    int iwn[24];
    int iwo[24];
    sceVu0FVECTOR item_heat_wpos[24];
    static int item_heat_onum[24];
    static int item_heat_use[24];
    static void *item_heat_addr[24];
    static sceVu0FVECTOR item_heat_pos[24];

    if (init_newitem != 0)
    {
        for (i = 0; i < 24; i++)
        {
            item_heat_addr[i] = NULL;
            item_heat_onum[i] = -1;
            item_heat_use[i] = -1;

            ni[i].flow = (u_int)(vu0Rand() * 2.9f);
            ni[i].rot = 0.0f;
            ni[i].cnt = vu0Rand() * 10.0f;
        }

        init_newitem = 0;
    }

    for (i = 0; i < 24; i++)
    {
        iwo[i] = 0;
        iwn[i] = 0;
    }

    i = 0;
    n = 0;
    while (i < 2)
    {
        j = 0;
        while (item_dsp_wrk[i][j].item_no != 0xffff)
        {
            if (item_dsp_wrk[i][j].disp != 0)
            {
                Vu0CopyVector(item_heat_wpos[n], item_dsp_wrk[i][j].pos);
                item_heat_num[n] = item_dsp_wrk[i][j].item_no;
                n++;
            }

            j++;
        }

        i++;
    }

    item_heat_num[n] = -1;

    i = 0;
    while (item_heat_onum[i] != -1)
    {
        j = 0;
        while (item_heat_num[j] != -1)
        {
            if (item_heat_onum[i] == item_heat_num[j])
            {
                iwo[i] = 1;
                iwn[j] = 1;
            }

            j++;
        }

        i++;
    }

    i = 0;
    num = 0;
    while (item_heat_onum[i] != -1)
    {
        if (iwo[i] == 0 || num >= 16)
        {
            j = 0;
            n = 1;
            while (j < 24 && n)
            {
                if (item_heat_onum[i] == item_heat_use[j])
                {
                    item_heat_use[j] = -1;
                    n = 0;
                }

                j++;
            }
        }
        else
        {
            num++;
        }

        i++;
    }

    i = 0;
    while (item_heat_num[i] != -1)
    {
        if (iwn[i] == 0 && num < 16)
        {
            j = 0;
            n = 1;
            while (j < 24 && n)
            {
                if (item_heat_use[j] == -1)
                {
                    item_heat_use[j] = item_heat_num[i];
                    Vu0CopyVector(item_heat_pos[j], item_heat_wpos[i]);
                    item_heat_addr[j] = NULL;
                    n = 0;

                    ni[j].flow = (u_int)(vu0Rand() * 2.9f);
                    ni[j].rot = 0.0f;
                    ni[j].cnt = vu0Rand() * 10.0f;

                    num++;
                }

                j++;
            }
        }

        i++;
    }

    for (i = 0; i < 24; i++)
    {
        item_heat_onum[i] = item_heat_num[i];
    }

    if (1) // preprocessor variable? or just a new context to define and initialize variables?
    {
        int w;
        sceVu0IVECTOR ivec;
        sceVu0FVECTOR fvec;
        sceVu0FMATRIX local_world;
        sceVu0FMATRIX local_screen;
        sceVu0FVECTOR itm_pos = { 0.0f, 0.0f, 0.0f, 1.0f };

        for (i = 0; i < 24; i++)
        {
            if (item_heat_use[i] != -1)
            {
                sceVu0UnitMatrix(local_world);

                local_world[0][0] = local_world[1][1] = local_world[2][2] = 25.0f;

                sceVu0TransMatrix(local_world, local_world, item_heat_pos[i]);
                sceVu0MulMatrix(local_screen, SgWSMtx, local_world);
                sceVu0RotTransPers(ivec, local_screen, itm_pos, 0);

                w = 0;

                if (ivec[0] < 0x6800 || ivec[0] > 0x9800)
                {
                    w = 1;
                }

                if (ivec[1] < 0x7500 || ivec[1] > 0x8b00)
                {
                    w = 1;
                }

                if (ivec[2] < 0xff || ivec[2] > 0xffffff)
                {
                    w = 1;
                }

                if (!w)
                {
                    Vu0CopyVector(ni[i].pos, item_heat_pos[i]);

                    ni[i].pos[1] -= 50.0f;
                    ni[i].pos[3] = 1.0f;

                    SetNewItemEffect(&ni[i]);
                }
            }
        }
    }
}

void SetEneFire(EFFECT_CONT *ec)
{
    float r;
    float g;
    float b;
    float a;
    float s;
    float t;
    float ar;
    u_int col;
    u_int ty;
    sceVu0FVECTOR pos1;
    sceVu0FVECTOR pos2;

    if (ingame_wrk.stts & 0x20)
    {
        return;
    }

    col = *(u_int *)ec->pnt[1];

    r = (col >> 24) & 0xff;
    g = (col >> 16) & 0xff;
    b = (col >>  8) & 0xff;
    a = (col >>  0) & 0xff;

    s = *(float *)ec->pnt[2];
    t = ec->dat.ui32[3];

    ar = *(float *)ec->pnt[5];
    ty = ec->dat.uc8[2] ? 11 : 10;

    Vu0CopyVector(pos1, ec->pnt[0]);

    if (ec->pnt[4] != NULL)
    {
        Vu0CopyVector(pos2, ec->pnt[4]);

        ec->pnt[3] = GetEnePartAddr(ec->pnt[3], ty, t);
        ec->pnt[3] = ContHeatHaze(ec->pnt[3], ty, pos1, pos2, 0, r, g, b, a, s, ar);
    }
    else
    {
        ec->pnt[3] = GetEnePartAddr(ec->pnt[3], ty, t);
        ec->pnt[3] = ContHeatHaze(ec->pnt[3], ty, pos1, NULL, 0, r, g, b, a, s, ar);
    }


    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void CallAmuletFire()
{
    amulet_fire_flow = 0;
    amulet_fire_cnt = 0;
}

int SetAmuletFire()
{
    static u_char alp[17];
    static int cnt;
    static int load_id;
    static float h = 0.0f;
    int i;
    int c;
    int n;
    int w;
    int ret;
    float fdiv;
    float size;
    float fx;
    float fy;
    float fz;
    float l;
    float lla;
    sceVu0IVECTOR ivec[17][2];
    sceVu0FVECTOR seal[17][2];
    sceVu0FMATRIX local_world;
    sceVu0FMATRIX world_view;
    sceVu0FMATRIX world_screen;
    sceVu0FMATRIX view_screen;
    sceVu0FMATRIX local_screen;
    sceVu0FMATRIX work;
    sceVu0FVECTOR rot;
    sceVu0FVECTOR bpos;
    sceVu0FVECTOR obj_pos;
    sceVu0FVECTOR fire_pos;
    U32DATA ts[17][2];
    U32DATA tt[17][2];
    U32DATA tq[17][2];

    n = 1;
    ret = 0;

    lla = 200.0f;

    if (dbg_wrk.eff_amufire == 0)
    {
        return 0;
    }

    switch (amulet_fire_flow)
    {
    case -1:
    case 0:
        for (i = 0; i < 17; i++)
        {
            alp[i] = 0x80;
        }
    break;
    }

    fx = (camera.i[0] - camera.p[0]) * (camera.i[0] - camera.p[0]);
    fy = (camera.i[1] - camera.p[1]) * (camera.i[1] - camera.p[1]);
    fz = (camera.i[2] - camera.p[2]) * (camera.i[2] - camera.p[2]);

    l = SgSqrtf(fx + fy + fz);

    bpos[0] = camera.p[0] + ((camera.i[0] - camera.p[0]) * lla) / l;
    bpos[1] = camera.p[1] + ((camera.i[1] - camera.p[1]) * lla) / l;
    bpos[2] = camera.p[2] + ((camera.i[2] - camera.p[2]) * lla) / l;
    bpos[3] = 1.0f;

    Vu0CopyVector(obj_pos, bpos);

    fdiv = 16.0f;
    size = 4.0f;

    for (i = 0; i < 17; i++)
    {
        seal[i][0][0] = -0.8f;
        seal[i][0][1] = ( size * i) / fdiv - (size * 0.5f);
        seal[i][0][2] = 0.0f;
        seal[i][0][3] = 1.0f;

        seal[i][1][0] = 0.8f;
        seal[i][1][1] = (size * i) / fdiv - (size * 0.5f);
        seal[i][1][2] = 0.0f;
        seal[i][1][3] = 1.0f;
    }

    GetTrgtRot(camera.p, camera.i, rot, 3);

    sceVu0UnitMatrix(local_world);

    local_world[0][0] = local_world[1][1] = local_world[2][2] = camera.fov * 25.0f;

    sceVu0RotMatrixX(local_world, local_world, rot[0]);
    sceVu0RotMatrixY(local_world, local_world, rot[1]);
    sceVu0TransMatrix(local_world, local_world, obj_pos);
    sceVu0MulMatrix(local_screen, SgWSMtx, local_world);

    for (i = 0, w = 0; i < 17; i++)
    {
        sceVu0RotTransPers(ivec[i][0], local_screen, seal[i][0], 1);
        sceVu0RotTransPers(ivec[i][1], local_screen, seal[i][1], 1);

        if (0x5000 < ivec[i][0][0] - 0x5800U)
        {
            w = 1;
        }

        if (0x1c00 < ivec[i][0][1] - 0x7200U)
        {
            w = 1;
        }

        if (0xffff00 < ivec[i][0][2] - 0xffU)
        {
            w = 1;
        }

        if (0x5000 < ivec[i][1][0] - 0x5800U)
        {
            w = 1;
        }

        if (0x1c00 < ivec[i][1][1] - 0x7200U)
        {
            w = 1;
        }

        if (0xffff00 < ivec[i][1][2] - 0xffU)
        {
            w = 1;
        }

        tq[i][0].fl32 = 1.0f / ivec[i][0][3];
        ts[i][0].fl32 = tq[i][0].fl32 * 0.02f * 94.0f / 128.0f;
        tt[i][0].fl32 = ((i * 0.96f) / fdiv + 0.02f) * tq[i][0].fl32;
        tq[i][1].fl32 = 1.0f / ivec[i][1][3];
        ts[i][1].fl32 = tq[i][1].fl32 * 0.98f * 94.0f / 128.0f;
        tt[i][1].fl32 = ((i * 0.96f) / fdiv + 0.02f) * tq[i][1].fl32;
    }

    if (w)
    {
        return ret;
    }

    Reserve2DPacket(0x1000);

    c = ndpkt;

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = DMAend;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    pbuf[ndpkt].ul64[0] = effdat[monochrome_mode + 28].tex0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(17, SCE_GS_TRUE, SCE_GS_TRUE, 92, SCE_GIF_PACKED, 6);
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_ST    << (4 * 0)
        | SCE_GS_RGBAQ << (4 * 1)
        | SCE_GS_XYZF2 << (4 * 2)
        | SCE_GS_ST    << (4 * 3)
        | SCE_GS_RGBAQ << (4 * 4)
        | SCE_GS_XYZF2 << (4 * 5);

    for (i = 0; i < 17; i++)
    {
        pbuf[ndpkt].ui32[0] = ts[i][0].ui32;
        pbuf[ndpkt].ui32[1] = tt[i][0].ui32;
        pbuf[ndpkt].ui32[2] = tq[i][0].ui32;
        pbuf[ndpkt++].ui32[3] = 0;

        pbuf[ndpkt].ui32[0] = 0x40;
        pbuf[ndpkt].ui32[1] = 0x40;
        pbuf[ndpkt].ui32[2] = 0x40;
        pbuf[ndpkt++].ui32[3] = alp[i];

        pbuf[ndpkt].ui32[0] = ivec[i][0][0];
        pbuf[ndpkt].ui32[1] = ivec[i][0][1];
        pbuf[ndpkt].ui32[2] = ivec[i][0][2] * 16;
        pbuf[ndpkt++].ui32[3] = (i > 0) ? 0 : 0x8000;

        pbuf[ndpkt].ui32[0] = ts[i][1].ui32;
        pbuf[ndpkt].ui32[1] = tt[i][1].ui32;
        pbuf[ndpkt].ui32[2] = tq[i][1].ui32;
        pbuf[ndpkt++].ui32[3] = 0;

        pbuf[ndpkt].ui32[0] = 0x40;
        pbuf[ndpkt].ui32[1] = 0x40;
        pbuf[ndpkt].ui32[2] = 0x40;
        pbuf[ndpkt++].ui32[3] = alp[i];

        pbuf[ndpkt].ui32[0] = ivec[i][1][0];
        pbuf[ndpkt].ui32[1] = ivec[i][1][1];
        pbuf[ndpkt].ui32[2] = ivec[i][1][2] * 16;
        pbuf[ndpkt++].ui32[3] = (i > 0) ? 0 : 0x8000;
    }

    pbuf[c].ui32[0] = ndpkt + DMAend - c - 1;


    switch (amulet_fire_flow)
    {
    case -1:
        return 0;
    break;
    case 0:
        cnt = 0;
        load_id = SeFileLoadAndSet(ST012_OFUDA_BD, 21);
        amulet_fire_flow++;
    break;
    case 1:
        if (IsLoadEnd(load_id) != 0)
        {
            SeStartFix(0x5d, 0, 0x1000, 0x1000, 0);
            amulet_fire_ret = NULL;
            amulet_fire_flow++;
        }
    break;
    case 2:
        if (stop_effects == 0)
        {
            cnt++;
        }

        if (cnt < 60)
        {
            h = -size * 0.5f;
            n = 0;
        }
        else
        {
            if (alp[0] != 0)
            {
                if (stop_effects == 0)
                {
                    alp[0] = alp[0] - 16 < 0 ? 0 : alp[0] - 16;
                }
                h = (((128.0f - alp[0]) / 128.0f + 0.0f) * size) / fdiv - (size * 0.5f);
                n = 0;
            }

            for (i = 1; i < 17; i++)
            {
                if (alp[i-1] == 0 && alp[i] != 0)
                {
                    if (stop_effects == 0)
                    {
                        alp[i] = alp[i] - 16 < 0 ? 0 : alp[i] - 16;
                    }

                    h = ((i + ((128.0f - alp[i]) / 128.0f)) * size) / fdiv - (size * 0.5f);
                    n = i < 15 ? 0 : 1;
                }
            }
        }

        if (1) // always true condition, or simply a context block?
        {
            sceVu0FVECTOR fpos = {0.0f, 0.0f, 0.0f, 1.0f};
            HEAT_HAZE *hh;

            fpos[1] = h + 0.6f;
            fpos[3] = 1.0f;
            sceVu0ApplyMatrix(fire_pos, local_world, fpos);
            amulet_fire_ret = GetAmuPartAddr(amulet_fire_ret, 9, 200);
            amulet_fire_ret = ContHeatHaze(amulet_fire_ret, 9, fire_pos, NULL, n, 128.0f, 120.0f, 255.0f, 128.0f, 3500.0f, 1.0f);
            hh = (HEAT_HAZE *)amulet_fire_ret;
            if (alp[16] == 0 && hh->disp <= 0)
            {
                amulet_fire_flow = 0xfe;
            }
        }
    break;
    case 0xfe:
        amulet_fire_flow = 0xff;
    case 0xff:
        ret = 1;
    break;
    }

    return ret;
}


u_char prgb = 0x40;
u_char pa = 0x0c;
float wind1 = 0.3f;
float wind2 = 0.002f;
float ppos1 = 0.3f;
float pvel1 = 0.004f;
float pvel2 = 0.005;
float pvel3 = 0.005;
float psize = 4000.0f;

void* ContTorch(void *addr, int type, float *pos, float *pos2, int st, float r, float g, float b, float a, float size, float arate, int tp2, float ar, float sr)
{
    float rrate;
    float escl;
    float ysp1;
    float ysp2;
    int i;
    int j;
    float wind;
    sceVu0FMATRIX work;
    sceVu0FMATRIX local_clip;
    sceVu0FMATRIX local_world;
    sceVu0FMATRIX local_screen;
    sceVu0FVECTOR ppos = {0.0f, 0.0f, 0.0f, 1.0f};
    sceVu0FVECTOR pvel = {0.0f, 0.0f, 0.0f, 0.0f};
    sceVu0FVECTOR wpos;
    float fx;
    float fy;
    float fz;
    float rx;
    float rz;
    HEAT_HAZE *hh;

    rrate = 3.4f;
    escl = 3.8f;
    ysp1 = 0.13f;
    ysp2 = 0.11f;
    wind = 0.0f;

    if (init_torch != 0)
    {
        init_torch = 0;
    }

    if (addr == NULL)
    {
        return NULL;
    }

    hh = addr;

    hh->flag |= 0xff;

    if (ingame_wrk.mode == INGAME_MODE_MENU)
    {
        return addr;
    }

    if (pos2 == NULL)
    {
        rx = wind;
        rz = wind;
    }
    else
    {
        Get2PosRot2(pos2, pos, &rx, &rz);
    }

    fx = (pos[0] - camera.p[0]) * (pos[0] - camera.p[0]);
    fy = (pos[1] - camera.p[1]) * (pos[1] - camera.p[1]);
    fz = (pos[2] - camera.p[2]) * (pos[2] - camera.p[2]);

    SgSqrtf(fx + fy + fz);

    sceVu0UnitMatrix(work);

    work[0][0] = work[1][1] = work[2][2] = 25.0f;

    sceVu0RotMatrixX(work, work, rx);
    sceVu0RotMatrixZ(work, work, -rz);
    sceVu0TransMatrix(local_world, work, pos);
    sceVu0MulMatrix(local_screen, SgWSMtx, local_world);
    sceVu0MulMatrix(local_clip, SgCMtx, local_world);
    Vu0CopyVector(wpos, pos);

    switch(tp2)
    {
    case 0:
        rrate = 1.14f;
        escl = 3.8f;
        wpos[1] -= 40.0f;
        SubHalo(wpos, 0, 0, 0, 0x80, 0x5c, 0x3c, (hh->disp * 80.0f * ar) / hh->max, (hh->disp * 0.3f * sr) / hh->max);
    break;
    case 1:
        rrate = 1.14f;
        escl = 3.8f;
        wpos[1] -= 40.0f;
        SubHalo(wpos, 0, 0, 0, 0x3c, 0x5c, 0x80, (hh->disp * 80.0f * ar) / hh->max, (hh->disp * 0.3f * sr) / hh->max);
    break;
    case 2:
        rrate = 1.2f;
        escl = 3.8f;
        wpos[1] -= 20.0f;
        SubHalo(wpos, 0, 0, 0, 0x3c, 0x5c, 0x80, (hh->disp * 80.0f * ar) / hh->max, (hh->disp * 0.2f * sr) / hh->max);
        wpos[1] += 10.0f;
        SubHalo(wpos, 0, 0, 0, 0xff, 0xff, 0xff, (hh->disp * 200.0f * ar) / hh->max, (hh->disp * 0.04f * sr) / hh->max);
    break;
    case 3:
        ysp1 = 0.12f;
        ysp2 = 0.1f;
        rrate = 3.4f;
        escl = 3.8f;
        wpos[1] -= 40.0f;
        SubHalo(wpos, 2, 0, 0, 0xff, 0xa0, 0x80, (hh->disp * 128.0f * ar) / hh->max, (hh->disp * 0.8f * sr) / hh->max);
    break;
    case 4:
        ysp1 = 0.16f;
        ysp2 = 0.3f;
        rrate = 4.8f;
        escl = 3.8f;
        wpos[1] -= 40.0f;
        SubHalo(wpos, 2, 0, 0, 0xff, 0xa0, 0x80, (hh->disp * 128.0f * ar) / hh->max,  (hh->disp * 0.8f * sr) / hh->max);
    break;
    }

    hh->disp = draw_distortion_particles(&local_screen, &local_clip, sys_wrk.count & 1, 200, addr, 0x40, (size * escl * sr) / camera.fov, -1.0f, type);

    wind = vu0Rand() * 0.02f - 0.01f;

    if (stop_effects == 0)
    {
        for (j = 0; j < st; j++)
        {
            switch (tp2)
            {
            case 3:
            case 4:
                for (i = 0; i < 4; i++)
                {
                    ppos[0] = (vu0Rand() - 0.5f) * 0.9f * rrate;
                    ppos[1] = 0.0f;
                    ppos[2] = (vu0Rand() - 0.5f) * 0.9f * rrate;

                    pvel[0] = (vu0Rand() - 0.5f) * 0.02f * rrate + wind;
                    pvel[1] = -(ysp1 * vu0Rand()) - ysp2;
                    pvel[2] = (vu0Rand() - 0.5f) * 0.02f * rrate;

                    add_particle(type, addr, ppos, pvel, r, g, b, a * ar);
                }
            break;
            case 0:
            case 1:
            case 2:
                for (i = 0; i < 4; i++)
                {
                    ppos[0] = (vu0Rand() - 0.5f) * 0.7f * rrate;
                    ppos[1] = 0.0f;
                    ppos[2] = (vu0Rand() - 0.5f) * 0.7f * rrate;

                    pvel[0] = (vu0Rand() - 0.5f) * 0.02f + wind;
                    pvel[1] = -(vu0Rand() * 0.03f) - 0.05f;
                    pvel[2] = (vu0Rand() - 0.5f) * 0.02f;

                    add_particle(type, addr, ppos, pvel, r, g, b, a * ar);
                }
            break;
            }

            hh->cnt++;

            update_particles(hh->particles);
        }
    }

    return addr;
}

void SetTorch(EFFECT_CONT *ec)
{
    sceVu0FVECTOR pos;
    int st;
    int life;
    float tr;
    float tg;
    float tb;
    float sr;
    float ar;
    int torch_type;

    st = 1;
    life = 50;
    tr = 6.0f;
    tg = 6.0f;
    tb = 32.0f;

    if (ec->dat.uc8[2] == 3)
    {
        if (ec->dat.uc8[3])
        {
            torch_type = 4;
            ec->dat.uc8[3]--;
        }
        else
        {
            torch_type = 3;
        }
    }
    else
    {
        torch_type = ec->dat.uc8[2];
    }

    sr = ec->pnt[2] != NULL ? *(float *)ec->pnt[2] : 1.0f;
    ar = ec->pnt[3] != NULL ? *(float *)ec->pnt[3] : 1.0f;

    switch(torch_type)
    {
    case 0:
        tr = 80.0f;
        tg = 48.0f;
        tb = 16.0f;
        st = 1;
    break;
    case 1:
        tr = 32.0f;
        tg = 32.0f;
        tb = 80.0f;
        st = 1;
    break;
    case 2:
        tr = 6.0f;
        tg = 6.0f;
        tb = 60.0f;
        st = 1;
    break;
    case 3:
        tr = 80.0f;
        tg = 48.0f;
        tb = 16.0f;
        ar = 0.27f;
        sr = 3.3f;
        st = 1;
    break;
    case 4:
        tr = 80.0f;
        tg = 48.0f;
        tb = 16.0f;
        st = 3;
        ar = 0.5f;
        sr = 4.5f;
    break;
    }

    Vu0CopyVector(pos, ec->pnt[0]);

    ec->pnt[1] = GetTorchPartAddr(ec->pnt[1], 12, life * sr);
    ec->pnt[1] = ContTorch(ec->pnt[1], 12, pos, NULL, st, tr, tg, tb, 128.0f, 1300.0f, 1.0f, torch_type, ar, sr);

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void* ContSmoke(void *addr, int type, float *pos, float *pos2, int st, float r, float g, float b, float a, float size, float arate)
{
    int i;
    float rate;
    float escl;
    float wind;
    sceVu0FMATRIX work;
    sceVu0FMATRIX local_clip;
    sceVu0FMATRIX local_world;
    sceVu0FMATRIX local_screen;
    sceVu0FVECTOR ppos = { 0.0f, 0.0f, 0.0f, 1.0f };
    sceVu0FVECTOR pvel = { 0.0f, 0.0f, 0.0f, 0.0f };
    sceVu0FVECTOR wpos;
    float fx;
    float fy;
    float fz;
    float rx;
    float rz;
    HEAT_HAZE *hh;

    rate = 1.14f;
    escl = 3.8f;

    if (init_smoke != 0)
    {
        for (i = 0; i < 4; i++)
        {
            smoke_particle[i].flag = 0;
            smoke_particle[i].cnt = 0;
        }

        init_smoke = 0;
    }

    if (addr == NULL)
    {
        return NULL;
    }

    hh = addr;

    hh->flag |= 0xff;

    if (ingame_wrk.mode != 0xa)
    {
        if (pos2 == NULL)
        {
            rx = 0.0f;
            rz = 0.0f;
        }
        else
        {
            Get2PosRot2(pos2, pos, &rx, &rz);
        }

        fx = (pos[0] - camera.p[0]) * (pos[0] - camera.p[0]);
        fy = (pos[1] - camera.p[1]) * (pos[1] - camera.p[1]);
        fz = (pos[2] - camera.p[2]) * (pos[2] - camera.p[2]);

        SgSqrtf(fx + fy + fz);

        sceVu0UnitMatrix(work);

        work[0][0] = work[1][1] = work[2][2] = 25.0f;

        sceVu0RotMatrixX(work, work, rx);
        sceVu0RotMatrixZ(work, work, -rz);
        sceVu0TransMatrix(local_world, work, pos);
        sceVu0MulMatrix(local_screen, SgWSMtx, local_world);
        sceVu0MulMatrix(local_clip, SgCMtx, local_world);

        hh->disp = draw_distortion_particles(
            &local_screen, &local_clip, sys_wrk.count % 2,
            200, hh->particles, sizeof(PARTICLE),
            (psize * escl) / camera.fov, -1.0f, type);

        if (stop_effects == 0)
        {
            if (st == 0)
            {
                wind = SgSinf((hh->cnt * wind1 * PI) / 180.0f) * wind2;

                if (hh->cnt % 8 == 0)
                {
                    ppos[0] = (vu0Rand() - 0.5f) * ppos1 * rate;
                    ppos[1] = 0.0f;
                    ppos[2] = (vu0Rand() - 0.5f) * ppos1 * rate;

                    pvel[0] = (vu0Rand() - 0.5f) * pvel3 + wind;
                    pvel[1] = -(pvel1 * vu0Rand()) - pvel2;
                    pvel[2] = (vu0Rand() - 0.5f) * pvel3 + wind;

                    add_particle(type, hh, ppos, pvel, prgb, prgb, prgb, pa);
                }

                hh->cnt++;

                update_particles(hh->particles);

                return addr;
            }

            if (st == 1)
            {
                for (i = 0; i < 5; i++)
                {
                    update_particles(hh->particles);
                }

                return addr;
            }

            if (st != 0xff)
            {
                return addr;
            }

            for (i = 0; i < 800; i++)
            {
                wind = SgSinf((hh->cnt * wind1 * PI) / 180.0f) * wind2;

                if (hh->cnt % 8 == 0)
                {
                    ppos[0] = (vu0Rand() - 0.5f) * ppos1 * rate;
                    ppos[1] = 0.0f;
                    ppos[2] = (vu0Rand() - 0.5f) * ppos1 * rate;

                    pvel[0] = (vu0Rand() - 0.5f) * pvel3 + wind;
                    pvel[1] = -(pvel1 * vu0Rand()) - pvel2;
                    pvel[2] = (vu0Rand() - 0.5f) * pvel3 + wind;

                    add_particle(type, hh, ppos, pvel, prgb, prgb, prgb, pa);
                }

                hh->cnt++;

                update_particles(hh->particles);
            }
        }
    }

    return addr;
}

void SetSmoke(EFFECT_CONT *ec)
{
    sceVu0FVECTOR pos;
    int st;
    int life;
    float r;
    float g;
    float b;
    float a;
    float s;

    life = 1600;

    r = 80.0f;
    g = 48.0f;
    b = 16.0f;

    a = 128.0f;
    s = 1600.0f;

    Vu0CopyVector(pos, ec->pnt[0]);

    st = ec->pnt[1] != NULL ? 0 : 0xff;

    ec->pnt[1] = GetSmokePartAddr(ec->pnt[1], 13, life);
    ec->pnt[1] = ContSmoke(ec->pnt[1], 13, pos, NULL, st, r, g, b, a, s, 1.0f);

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SetSunshine(EFFECT_CONT *ec)
{
    int nsun = 1;
    int i; int j; int k; int n;
    int num;
    float rot_x; float rot_y; float power;
    float ww;
    float hh;
    float fx; float fy; float fz;
    float l;
    float add;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0FVECTOR wpos;
    sceVu0FVECTOR base_pos = {22875.0f, -457.0f, 8676.0f, 1.0f};
    sceVu0FVECTOR mpos[6] = {
        {0.0f, -120.0f, -92.0f, 1.0f},
        {-5.0f, +5.0f, 0.0f, 1.0f},
        {+5.0f, +5.0f, 0.0f, 1.0f},
        {+5.0f, -5.0f, 0.0f, 1.0f},
        {-5.0f, -5.0f, 0.0f, 1.0f},
        {-5.0f, +5.0f, 0.0f, 1.0f}
    };
    EFF_SUNSHINE eff_ray[6];
    Q_WORDDATA pbh[1024];
#ifdef MATCHING_DECOMP
    u_int *cnt_ptr;
#endif
    u_char mr, mg, mb;
    u_int max;

    Vu0CopyVector(mpos[0], ec->pnt[0]);
    Vu0CopyVector(base_pos, ec->pnt[1]);
    Vu0CopyVector(wpos, ec->pnt[2]);

    max = ec->max;
    ww = ec->dat.fl32[2];
    hh = ec->dat.fl32[3];

    rot_x = wpos[0];
    rot_y = wpos[1];
    power = max;
#ifdef MATCHING_DECOMP
    cnt_ptr = &ec->cnt;

    if (rot_x) {}
    if (rot_y) {}
    {
        hh = -hh;
        hh = -hh;
        ww = -ww;
        ww = -ww;
    }
#endif

    mr = ec->dat.uc8[2];
    mg = ec->dat.uc8[3];
    mb = ec->dat.uc8[4];
    add = (int)ec->cnt / 2000.0f;

    mpos[1][0] = mpos[4][0] = mpos[5][0] = -ww;
    mpos[2][0] = mpos[3][0] = ww;

    mpos[3][1] = mpos[4][1] = -hh;
    mpos[1][1] = mpos[2][1] = mpos[5][1] = hh;

    num = 7;

    for (i = 0; i < 6; i++)
    {
        Vu0CopyVector(eff_ray[i].fpos[0], mpos[i]);
    }

    for (i = 1; i < 6; i++)
    {
        fx = eff_ray[i].fpos[0][0] - eff_ray[0].fpos[0][0];
        fy = eff_ray[i].fpos[0][1] - eff_ray[0].fpos[0][1];
        fz = eff_ray[i].fpos[0][2] - eff_ray[0].fpos[0][2];

        l = SgSqrtf(fx * fx + fy * fy + fz * fz);

        eff_ray[i].ts[0].fl32 = i - 1;
        eff_ray[i].tt[0].fl32 = 1.0f - add;
        eff_ray[i].alp[0] = (512 - ((u_char)((l * 512) / power))) / nsun;

        eff_ray[i].fpos[num][0] = eff_ray[0].fpos[0][0] + ((eff_ray[i].fpos[0][0] - eff_ray[0].fpos[0][0]) * power) / l;
        eff_ray[i].fpos[num][1] = eff_ray[0].fpos[0][1] + ((eff_ray[i].fpos[0][1] - eff_ray[0].fpos[0][1]) * power) / l;
        eff_ray[i].fpos[num][2] = eff_ray[0].fpos[0][2] + ((eff_ray[i].fpos[0][2] - eff_ray[0].fpos[0][2]) * power) / l;
        eff_ray[i].fpos[num][3] = 1.0f;

        eff_ray[i].ts[num].fl32 = i - 1;
        eff_ray[i].tt[num].fl32 = 2.0f - add;
        eff_ray[i].alp[num] = 0;

        for (j = 1; j < num; j++)
        {
            eff_ray[i].fpos[j][0] = ((eff_ray[i].fpos[num][0] - eff_ray[i].fpos[0][0]) * j) / num + eff_ray[i].fpos[0][0];
            eff_ray[i].fpos[j][1] = ((eff_ray[i].fpos[num][1] - eff_ray[i].fpos[0][1]) * j) / num + eff_ray[i].fpos[0][1];
            eff_ray[i].fpos[j][2] = ((eff_ray[i].fpos[num][2] - eff_ray[i].fpos[0][2]) * j) / num + eff_ray[i].fpos[0][2];
            eff_ray[i].fpos[j][3] = 1.0f;

            eff_ray[i].ts[j].fl32 = ((eff_ray[i].ts[num].fl32 - eff_ray[i].ts[0].fl32) * j) / num + eff_ray[i].ts[0].fl32;
            eff_ray[i].tt[j].fl32 = ((eff_ray[i].tt[num].fl32 - eff_ray[i].tt[0].fl32) * j) / num + eff_ray[i].tt[0].fl32;
            eff_ray[i].alp[j] = (((eff_ray[i].alp[num] - eff_ray[i].alp[0]) * j) / num) + eff_ray[i].alp[0];
        }
    }

#ifdef MATCHING_DECOMP
    *cnt_ptr = (int)*cnt_ptr + 2 >= 2000 ? 0 : (int)*cnt_ptr + 1;
#else
    ec->cnt = (int)ec->cnt + 2 >= 2000 ? 0 : (int)ec->cnt + 1;
#endif

    sceVu0UnitMatrix(wlm);

    wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;

    sceVu0RotMatrixX(wlm, wlm, rot_x);
    sceVu0RotMatrixY(wlm, wlm, rot_y);
    sceVu0TransMatrix(wlm, wlm, base_pos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);

    sceVu0RotTransPers(eff_ray[0].ipos[0], slm, eff_ray[0].fpos[0], 0);

    n = 0;

    if (eff_ray[0].ipos[0][0] < 0x4000 ||  eff_ray[0].ipos[0][0] > 0xc000)
    {
        n = 1;
    }

    if (eff_ray[0].ipos[0][1] < 0x4000 ||  eff_ray[0].ipos[0][1] > 0xc000)
    {
        n = 1;
    }

    if (eff_ray[0].ipos[0][2] < 0xff || eff_ray[0].ipos[0][2] > 0xfffffff)
    {
        n = 1;
    }

    eff_ray[0].clip[0] = !n;

    for (j = 0; j <= num; j++)
    {
        eff_ray[0].clip[j] = 0;
    }

    for (i = 1; i < 6; i++)
    {
        for (j = 0; j <= num; j++)
        {
            sceVu0RotTransPers(eff_ray[i].ipos[j], slm, eff_ray[i].fpos[j], 1);

            eff_ray[i].tq[j].fl32 = 1.0f / eff_ray[i].ipos[j][3];
            eff_ray[i].ts[j].fl32 = eff_ray[i].ts[j].fl32 * eff_ray[i].tq[j].fl32;
            eff_ray[i].tt[j].fl32 = eff_ray[i].tt[j].fl32 * eff_ray[i].tq[j].fl32;

            n = 0;

            if (eff_ray[i].ipos[j][0] < 0x300 || eff_ray[i].ipos[j][0] > 0xfd00)
            {
                n = 1;
            }

            if (eff_ray[i].ipos[j][1] < 0x300 || eff_ray[i].ipos[j][1] > 0xfd00)
            {
                n = 1;
            }

            if (eff_ray[i].ipos[j][2] < 0xff || eff_ray[i].ipos[j][2] > 0xfffffff)
            {
                n = 1;
            }

            eff_ray[i].clip[j] = !n;
        }
    }

    Reserve2DPacket(0x1000);

    n = ndpkt;

    pbuf[ndpkt++].ul128 = (u_long128)0;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX0_1(7, 2, SCE_GS_PSMCT32, 0, 0, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 16, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    pbuf[ndpkt].ul64[0] = effdat[monochrome_mode + 30].tex0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_CLAMP_1;

    if (eff_ray[0].clip[0] != 0)
    {
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 64, SCE_GIF_PACKED, 2);
        pbuf[ndpkt++].ul64[1] = 0 \
            | SCE_GS_RGBAQ << (4 * 0)
            | SCE_GS_XYZF2 << (4 * 1);

        pbuf[ndpkt].ui32[0] = 0xff;
        pbuf[ndpkt].ui32[1] = 0xff;
        pbuf[ndpkt].ui32[2] = 0xff;
        pbuf[ndpkt++].ui32[3] = 0x80;

        pbuf[ndpkt].ui32[0] = eff_ray[0].ipos[0][0];
        pbuf[ndpkt].ui32[1] = eff_ray[0].ipos[0][1];
        pbuf[ndpkt].ui32[2] = eff_ray[0].ipos[0][2];
        pbuf[ndpkt++].ui32[3] = 0;
    }

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(32, SCE_GS_TRUE, SCE_GS_TRUE, 92, SCE_GIF_PACKED, 6);
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_ST    << (4 * 0)
        | SCE_GS_RGBAQ << (4 * 1)
        | SCE_GS_XYZF2 << (4 * 2)
        | SCE_GS_ST    << (4 * 3)
        | SCE_GS_RGBAQ << (4 * 4)
        | SCE_GS_XYZF2 << (4 * 5);

    for (i = 1; i < 5; i++)
    {
        for (j = 0; j <= num; j++)
        {
            k = j != 0 ? eff_ray[i].clip[j-1] + eff_ray[i+1].clip[j-1] + eff_ray[i].clip[j] : 0;

            pbuf[ndpkt].ui32[0] = eff_ray[i].ts[j].ui32;
            pbuf[ndpkt].ui32[1] = eff_ray[i].tt[j].ui32;
            pbuf[ndpkt].ui32[2] = eff_ray[i].tq[j].ui32;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = mr;
            pbuf[ndpkt].ui32[1] = mg;
            pbuf[ndpkt].ui32[2] = mb;
            pbuf[ndpkt++].ui32[3] = eff_ray[i].alp[j];

            pbuf[ndpkt].ui32[0] = eff_ray[i].ipos[j][0];
            pbuf[ndpkt].ui32[1] = eff_ray[i].ipos[j][1];
            pbuf[ndpkt].ui32[2] = eff_ray[i].ipos[j][2] * 16;
            pbuf[ndpkt++].ui32[3] = k > 2 ? 0 : 0x8000;

            k = j != 0 ? eff_ray[i+1].clip[j-1] + eff_ray[i].clip[j] + eff_ray[i+1].clip[j] : 0;

            pbuf[ndpkt].ui32[0] = eff_ray[i+1].ts[j].ui32;
            pbuf[ndpkt].ui32[1] = eff_ray[i+1].tt[j].ui32;
            pbuf[ndpkt].ui32[2] = eff_ray[i+1].tq[j].ui32;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = mr;
            pbuf[ndpkt].ui32[1] = mg;
            pbuf[ndpkt].ui32[2] = mb;
            pbuf[ndpkt++].ui32[3] = eff_ray[i+1].alp[j];

            pbuf[ndpkt].ui32[0] = eff_ray[i+1].ipos[j][0];
            pbuf[ndpkt].ui32[1] = eff_ray[i+1].ipos[j][1];
            pbuf[ndpkt].ui32[2] = eff_ray[i+1].ipos[j][2] * 16;
            pbuf[ndpkt++].ui32[3] = k > 2 ? 0 : 0x8000;
        }
    }

    pbuf[n].ui32[0] = ndpkt + DMAend - n - 1;

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SetDust(EFFECT_CONT *ec)
{
    static float malp = 80.0f;
    static float mala = 2.3f;
    static float mphs = 0.3f;
    static float mang = 20.0f;
    static float mscl = 0.1f;
    static u_char mr = 0x40;
    static u_char mg = 0x36;
    static u_char mb = 0x30;
    static float mv1x = 100.0f;
    static float mv1y = 45.0f;
    static float grv1x = 8.0f;
    static float grv1y = 4.0f;
    static float mv2x = 180.0f;
    static float mv2y = 20.0f;
    static float grv2x = 10.0f;
    static float grv2y = 5.0f;
    static EFF_DUST eff_dust[64];
    float f;
    float ss;
    float cc;
    float fw1;
    float fw2;
    float rot_x;
    float rot_y;
    int c;
    int i;
    int j;
    int k;
    int n;
    int w;
    int th;
    int tw;
    int so[64];
    u_long tx0;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0FVECTOR bpos;
    sceVu0FVECTOR wpos;
    sceVu0FVECTOR ppos[4] = {
        {-5.0f, 10.0f, 0.0f, 1.0f},
        {+5.0f, 10.0f, 0.0f, 1.0f},
        {-5.0f,  0.0f, 0.0f, 1.0f},
        {+5.0f,  0.0f, 0.0f, 1.0f},
    };

    if (ec->dat.uc8[2] != 0)
    {
        for (f = 0.0f, n = 0; f < 360.0f; f += mang, n++)
        {
            eff_dust[n].alp = malp;
            eff_dust[n].cnt = 0.0f;
            eff_dust[n].pos[0] = 0.0f;
            eff_dust[n].pos[1] = 0.0f;
            eff_dust[n].vel[0] = vu0Rand() * 60.0f + mv1x;
            eff_dust[n].vel[1] = vu0Rand() * 10.0f + mv1y;
        }

        for (f = 0.0f; f < 360.0f; f += mang, n++)
        {
            eff_dust[n].alp = malp;
            eff_dust[n].cnt = 0.0f;
            eff_dust[n].pos[0] = 0.0f;
            eff_dust[n].pos[1] = 0.0f;
            eff_dust[n].vel[0] = vu0Rand() * 40.0f + mv2x;
            eff_dust[n].vel[1] = vu0Rand() * 10.0f + mv2y;
        }

        ec->dat.uc8[2] = 0;
    }

    if (eff_dust[0].alp <= 0.0f)
    {
        ResetEffects(ec);

        return;
    }

    Vu0CopyVector(bpos, ec->pnt[0]);

    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

    for (f = 0.0f, n = 0; f < 360.0f; f += mang)
    {
        eff_dust[n].pos[0] = +eff_dust[n].vel[0] * eff_dust[n].cnt - grv1x * eff_dust[n].cnt * eff_dust[n].cnt;
        eff_dust[n].pos[1] = -eff_dust[n].vel[1] * eff_dust[n].cnt + grv1y * eff_dust[n].cnt * eff_dust[n].cnt;

        if (eff_dust[n].opos[1] < eff_dust[n].pos[1])
        {
            fw1 = mala * 0.3f;
            fw2 = mphs * 0.3f;
        }
        else
        {
            fw1 = mala;
            fw2 = mphs;
        }

        eff_dust[n].alp = eff_dust[n].alp - fw1 <= 0.0f ? 0.0f : eff_dust[n].alp - fw1;
        eff_dust[n].cnt += fw2;

        Vu0CopyVector(eff_dust[n].opos, eff_dust[n].pos);
        Vu0CopyVector(wpos, bpos);

        ss = SgSinf((f * PI) / 180.0f);
        cc = SgCosf((f * PI) / 180.0f);

        wpos[0] += (eff_dust[n].pos[0] * cc - eff_dust[n].pos[2] * ss) * 0.2f;
        wpos[1] += (eff_dust[n].pos[1]) * 0.2f;
        wpos[2] += (eff_dust[n].pos[0] * ss + eff_dust[n].pos[2] * cc) * 0.2f;

        sceVu0UnitMatrix(wlm);

        wlm[0][0] = wlm[1][1] = wlm[2][2] = eff_dust[n].pos[0] * mscl * 0.2f;

        sceVu0RotMatrixX(wlm, wlm, rot_x);
        sceVu0RotMatrixY(wlm, wlm, rot_y);
        sceVu0TransMatrix(wlm, wlm, wpos);
        sceVu0MulMatrix(slm, SgWSMtx, wlm);

        w = 0;

        for (i = 0; i < 4; i++)
        {
            sceVu0RotTransPers(eff_dust[n].ivec[i], slm, ppos[i], 0);

            if (eff_dust[n].ivec[i][0] < 0x4000 || eff_dust[n].ivec[i][0] > 0xc000)
            {
                w = 1;
            }

            if (eff_dust[n].ivec[i][1] < 0x4000 || eff_dust[n].ivec[i][1] > 0xc000)
            {
                w = 1;
            }

            if (eff_dust[n].ivec[i][2] < 0xff || eff_dust[n].ivec[i][2] > 0x00ffffff)
            {
                w = 1;
            }
        }

        if (!w)
        {
            n++;
        }
    }

    for (f = mang * 0.5f; f < mang * 0.5f + 360.0f; f = f + mang)
    {
        eff_dust[n].pos[0] = +eff_dust[n].vel[0] * eff_dust[n].cnt - grv2x * eff_dust[n].cnt * eff_dust[n].cnt;
        eff_dust[n].pos[1] = -eff_dust[n].vel[1] * eff_dust[n].cnt + grv2y * eff_dust[n].cnt * eff_dust[n].cnt;

        if (eff_dust[n].opos[1] < eff_dust[n].pos[1])
        {
            fw1 = mala * 0.4f;
            fw2 = mphs * 0.4f;
        }
        else
        {
            fw1 = mala;
            fw2 = mphs;
        }

        eff_dust[n].alp = eff_dust[n].alp - fw1 <= 0.0f ? 0.0f : eff_dust[n].alp - fw1;
        eff_dust[n].cnt += fw2;

        Vu0CopyVector(eff_dust[n].opos, eff_dust[n].pos);
        Vu0CopyVector(wpos, bpos);

        ss = SgSinf((f * PI) / 180.0f);
        cc = SgCosf((f * PI) / 180.0f);

        wpos[0] += (eff_dust[n].pos[0] * cc - eff_dust[n].pos[2] * ss) * 0.2f;
        wpos[1] += (eff_dust[n].pos[1]) * 0.2f;
        wpos[2] += (eff_dust[n].pos[0] * ss + eff_dust[n].pos[2] * cc) * 0.2f;

        sceVu0UnitMatrix(wlm);

        wlm[0][0] = wlm[1][1] = wlm[2][2] = ((eff_dust[n].pos[0] * mscl) / 1.5f) * 0.2f;

        sceVu0RotMatrixX(wlm, wlm, rot_x);
        sceVu0RotMatrixY(wlm, wlm, rot_y);
        sceVu0TransMatrix(wlm, wlm, wpos);
        sceVu0MulMatrix(slm, SgWSMtx, wlm);

        w = 0;

        for (i = 0; i < 4; i++)
        {
            sceVu0RotTransPers(eff_dust[n].ivec[i], slm, ppos[i], 0);

            if (eff_dust[n].ivec[i][0] < 0x4000 || eff_dust[n].ivec[i][0] > 0xc000)
            {
                w = 1;
            }

            if (eff_dust[n].ivec[i][1] < 0x4000 || eff_dust[n].ivec[i][1] > 0xc000)
            {
                w = 1;
            }

            if (eff_dust[n].ivec[i][2] < 0xff || eff_dust[n].ivec[i][2] > 0x00ffffff)
            {
                w = 1;
            }
        }

        if (!w)
        {
            n++;
        }
    }

    if (n != 0)
    {
        for (i = 0; i < n; i++)
        {
            so[i] = i;
        }

        for (j = 0; j < n - 1; j++)
        {
            for (i = j + 1; i < n; i++)
            {
                if (eff_dust[so[j]].ivec[0][2] > eff_dust[so[i]].ivec[0][2])
                {
                    k = so[i];
                    so[i] = so[j];
                    so[j] = k;
                }
            }
        }

        th = effdat[monochrome_mode + 6].h * 16;
        tw = effdat[monochrome_mode + 6].w * 16;
        tx0 = effdat[monochrome_mode + 6].tex0;

        Reserve2DPacket(0x1000);

        c = ndpkt;

        pbuf[ndpkt++].ul128 = (u_long128)0;

        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
        pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

        pbuf[ndpkt].ul64[0] = 0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

        pbuf[ndpkt].ul64[0] = tx0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
        pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
        pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

        for (j = 0; j < n; j++)
        {
            k = so[j];

            pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 348, SCE_GIF_PACKED, 3);
            pbuf[ndpkt++].ul64[1] = 0 \
                    | SCE_GS_RGBAQ << (4 * 0)
                    | SCE_GS_UV    << (4 * 1)
                    | SCE_GS_XYZF2 << (4 * 2);

            for (i = 0; i < 4; i++)
            {
                pbuf[ndpkt].ui32[0] = mr;
                pbuf[ndpkt].ui32[1] = mg;
                pbuf[ndpkt].ui32[2] = mb;
                pbuf[ndpkt++].ui32[3] = eff_dust[k].alp;

                pbuf[ndpkt].ui32[0] = (i & 1) != 0 ? tw - 8 : 8;
                pbuf[ndpkt].ui32[1] = (i / 2) != 0 ? th - 8 : 8;
                pbuf[ndpkt].ui32[2] = 0;
                pbuf[ndpkt++].ui32[3] = 0;

                pbuf[ndpkt].ui32[0] = eff_dust[k].ivec[i][0];
                pbuf[ndpkt].ui32[1] = eff_dust[k].ivec[i][1];
                pbuf[ndpkt].ui32[2] = eff_dust[k].ivec[i][2];
                pbuf[ndpkt++].ui32[3] = i > 1 ? 0 : 0x8000;
            }
        }

        pbuf[c].ui32[0] = ndpkt + DMAend - c - 1;
    }

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SetWaterdrop(EFFECT_CONT *ec)
{
    u_char r;
    u_char g;
    u_char b;
    int c;
    int i;
    int w;
    int th;
    int tw;
    int so[64];
    int uv[6][2];
    u_int wdbwait;
    u_int wdnwait;
    float wdadd;
    float wdcnt;
    float wddely;
    float rot_x;
    float rot_y;
    float pos_y;
    float grv1;
    u_long tx0;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0FVECTOR wpos;
    static sceVu0FVECTOR dummy_rot = {0.0f, 0.0f, 0.0f, 1.0f};
    static sceVu0FVECTOR wwpos[24];
    static int wwcnt = 0;
    sceVu0IVECTOR ivec[4];
    sceVu0FVECTOR ppos[4] = {
        {-1.0f, +1.0f, 0.0f, 1.0f},
        {+1.0f, +1.0f, 0.0f, 1.0f},
        {-1.0f, -1.0f, 0.0f, 1.0f},
        {+1.0f, -1.0f, 0.0f, 1.0f},
    };
    u_int val50 = 50;

    wddely = ec->dat.fl32[2];
    wdbwait = ec->max;
    wdnwait = ec->cnt;

    wdadd = 0.4f;
    grv1 = 9.8f;
    wdcnt = ec->dat.fl32[3];

    if (ec->dat.uc8[7] == 0)
    {
        r = ec->dat.uc8[2];
        g = ec->dat.uc8[3];
        b = ec->dat.uc8[4];
    }
    else
    {
        g = 0x00;
        r = 0x80;
        b = 0x00;
    }

    Vu0CopyVector(wpos, ec->pnt[0]);
    pos_y = grv1 * wdcnt * wdcnt;
    wpos[1] += pos_y;

    if (stop_effects == 0)
    {
        if (wddely < wpos[1])
        {
            if (!(wdnwait-- > 0))
            {
                wdcnt = 0.0f;
                wdnwait = (u_int)(vu0Rand() * 100.0f) + (wdbwait - val50);

                if (ec->dat.uc8[5] == 4)
                {
                    ec->dat.uc8[7] = vu0Rand() * 100.0f < 10.0f ? 1 : 0;
                }
            }

            ec->cnt = wdnwait;
            ec->dat.fl32[3] = wdcnt;

            if (ec->flow == 0)
            {
                return;
            }

            if (ec->dat.uc8[5] == 1)
            {
                Vu0CopyVector(wwpos[wwcnt], wpos);
                wwpos[wwcnt][1] = wddely;

                SetEffects(0x16, 8, 1, (u_int)(vu0Rand() * 16.0f) + 8, 0x20, 0x20, 0x30, 0.3f, 4.5f, wwpos[wwcnt], dummy_rot, 2);

                if (plyr_wrk.pr_info.room_no == 23)
                {
                    SeStartPosEventFlame(plyr_wrk.pr_info.room_no, 0, wwpos[wwcnt], 0, 0x1000, 0x1000);
                }

                wwcnt = (wwcnt + 1) % 16;
                ec->flow = 0;
            }

            if (ec->flow == 0)
            {
                return;
            }

            if (ec->dat.uc8[5] == 0x2)
            {
                Vu0CopyVector(wwpos[wwcnt], wpos);
                wwpos[wwcnt][0] += (vu0Rand() * 128.0f - 64.0f);
                wwpos[wwcnt][1] = wddely;
                wwpos[wwcnt][2] += (vu0Rand() * 128.0f - 64.0f);

                SetEffects(0x16, 8, 0x81, 0x10, 0x40, 0, 0, 0.002f, 0.3f, wwpos[wwcnt], dummy_rot, 1);

                if (plyr_wrk.pr_info.room_no == 1 || plyr_wrk.pr_info.room_no == 26)
                {
                    SeStartPosEventFlame(plyr_wrk.pr_info.room_no, 0x0, wwpos[wwcnt], 0, 0x1000, 0x1000);
                }

                wwcnt = (wwcnt + 1) % 24;
                ec->flow = 0;
            }

            if (ec->flow == 0)
            {
                return;
            }

            if (ec->dat.uc8[5] != 4)
            {
                return;
            }

            Vu0CopyVector(wwpos[wwcnt], wpos);
            wwpos[wwcnt][1] = wddely;

            if (ec->dat.uc8[7] != 0)
            {
                SetEffects(0x16, 8, 0x41, 0x20, 0x40, 0, 0, 0.05f, 4.0f, wwpos[wwcnt], dummy_rot, 2);
            }
            else
            {
                SetEffects(0x16, 8, 1, (u_int)(vu0Rand() * 16.0f) + 8, 0x20, 0x20, 0x30, 0.3f, 4.5f, wwpos[wwcnt], dummy_rot, 2);
            }
            if (plyr_wrk.pr_info.room_no == 0x17)
            {
                SeStartPosEventFlame(plyr_wrk.pr_info.room_no, 0, wwpos[wwcnt], 0, 0x1000, 0x1000);
            }

            wwcnt = (wwcnt + 1) % 0x18;
            ec->flow = 0;

            return;
        }

        ec->flow = 1;
        wdcnt += wdadd;
    }

    wdnwait = wdnwait;
    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);
    sceVu0UnitMatrix(wlm);

    wlm[0][0] = wlm[1][1] = wlm[2][2] = 8.0f;

    sceVu0RotMatrixY(wlm, wlm, rot_y);
    sceVu0TransMatrix(wlm, wlm, wpos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);

    w = 0;

    for (i = 0; i < 4; i++)
    {
        sceVu0RotTransPers(ivec[i], slm, ppos[i], 0);

        if (ivec[i][0] < 0x4000 || ivec[i][0] > 0xc000)
        {
            w = 1;
        }

        if (ivec[i][1] < 0x4000 || ivec[i][1] > 0xc000)
        {
            w = 1;
        }

        if (ivec[i][2] < 0xff || ivec[i][2] > 0x00ffffff)
        {
            w = 1;
        }
    }

    if (w)
    {
        ec->cnt = wdnwait;
        ec->dat.fl32[3] = wdcnt;

        return;
    }

    th = effdat[monochrome_mode + 52].h * 16;
    tw = effdat[monochrome_mode + 52].w * 16;
    tx0 = effdat[monochrome_mode + 52].tex0;

    Reserve2DPacket(0x1000);

    c = ndpkt;

    pbuf[ndpkt++].ul128 = (u_long128)0;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    pbuf[ndpkt].ul64[0] = tx0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 348, SCE_GIF_PACKED, 3);
    pbuf[ndpkt++].ul64[1] = 0 \
                    | SCE_GS_RGBAQ << (4 * 0)
                    | SCE_GS_UV    << (4 * 1)
                    | SCE_GS_XYZF2 << (4 * 2);

    for (i = 0; i < 4; i++)
    {
        pbuf[ndpkt].ui32[0] = r;
        pbuf[ndpkt].ui32[1] = g;
        pbuf[ndpkt].ui32[2] = b;
        pbuf[ndpkt++].ui32[3] = 0x20;

        pbuf[ndpkt].ui32[0] = (i & 1) != 0 ? tw - 8 : 8;
        pbuf[ndpkt].ui32[1] = (i / 2) == 0 ? th - 8 : 8;
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt++].ui32[3] = 0;

        pbuf[ndpkt].ui32[0] = ivec[i][0];
        pbuf[ndpkt].ui32[1] = ivec[i][1];
        pbuf[ndpkt].ui32[2] = ivec[i][2];
        pbuf[ndpkt++].ui32[3] = i > 1 ? 0 : 0x8000;
    }

    pbuf[c].ui32[0] = ndpkt + DMAend - c - 1;

    ec->cnt = wdnwait;
    ec->dat.fl32[3] = wdcnt;

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SetDustLeaf(sceVu0FVECTOR pos, int type)
{
    u_char rgb1[3] = {0x12, 0x18, 0x1e};
    u_char rgb2[3] = {0x16, 0x16, 0x1a};
    u_char mr;
    u_char mg;
    u_char mb;
    u_char mrr;
    u_char mrg;
    u_char mrb;
    int i;
    int num;
    int status;
    int leaf_num;
    float ang;
    float angr;
    float wvx;
    float wvy;
    float mv1x;
    float mv1y;
    float mangr;
    EFF_LEAF *lep;
    EFF_LEAF_ONE *lepo;

    if (type == 0)
    {
        if (plyr_wrk.pr_info.room_no == 21)
        {
            mr = rgb1[0];
            mg = rgb1[1];
            mb = rgb1[2];

            mrr = 2;
            mrg = 2;
            mrb = 2;
        }
        else
        {
            mr = rgb2[0];
            mg = rgb2[1];
            mb = rgb2[2];

            mrr = 12;
            mrg = 12;
            mrb = 8;
        }

        mv1x = 20.0f;
        mv1y = 20.0f;

        mangr = 180.0f;
        leaf_num = 8;
    }
    else
    {
        mr = 28;
        mg = 24;
        mb = 20;

        mrr = 1;
        mrg = 1;
        mrb = 1;

        mv1x = 34.0f;
        mv1y = 20.0f;

        mangr = 360.0f;
        leaf_num = 16;
    }

    for (i = 0, num = -1; i < 6; i++)
    {
        if (eff_leaf[i].flag == 0)
        {
            num = i;
            break;
        }
    }

    if (num != -1)
    {
        if (plyr_wrk.sta & 1)
        {
            wvx = mv1x;
            wvy = mv1y;
        }
        else if (plyr_wrk.sta & 2)
        {
            wvx = mv1x * 0.4f;
            wvy = mv1y * 0.4f;
        }
        else if (plyr_wrk.sta & 4)
        {
            wvx = mv1x * 0.3f;
            wvy = mv1y * 0.3f;
        }
        else if (plyr_wrk.sta & 8)
        {
            wvx = mv1x * 0.2f;
            wvy = mv1y * 0.2f;
        }
        else
        {
            wvx = mv1x * 0.7f;
            wvy = mv1y * 0.7f;
        }

        ang = -plyr_wrk.move_box.rot[1] - (PI / 2);
        angr = (mangr * PI) / 180.0f;

        lep = &eff_leaf[num];
        lepo = eff_leaf[num].lo;

        lep->flag = 1;
        lep->type = type;

        Vu0CopyVector(lep->bpos, pos);

        for (i = 0; i < leaf_num; i++)
        {

            lepo[i].mang = (ang - angr * vu0Rand()) + angr * 0.5f;
            lepo[i].mang = lepo[i].mang < -PI ? lepo[i].mang + PI2 : (lepo[i].mang > PI ? lepo[i].mang - PI2 : lepo[i].mang);

            lepo[i].cnt = 0.0f;

            lepo[i].pos[0] = 0.0f;
            lepo[i].pos[1] = 0.0f;
            lepo[i].pos[2] = 0.0f;

            lepo[i].vel[0] = vu0Rand() * 10.0f + wvx;
            lepo[i].vel[1] = vu0Rand() * 15.0f + wvy;

            lepo[i].fl = 0;

            lepo[i].ang[0] = vu0Rand() * PI2 - PI;
            lepo[i].ang[1] = vu0Rand() * PI2 - PI;

            lepo[i].r = (mrr * vu0Rand() + mr);
            lepo[i].g = (mrg * vu0Rand() + mg);
            lepo[i].b = (mrb * vu0Rand() + mb);

            status = lep->type;

            lepo[i].a = status ? 0x1c : 0x80;
        }
    }
}

void SetLeaf(sceVu0FVECTOR pos)
{
    SetDustLeaf(pos, 0);
}

void SetDust2(sceVu0FVECTOR pos)
{
    SetDustLeaf(pos, 1);
}

#include "data/canal_bpos.h" // sceVu0FVECTOR canal_bpos[3];
#include "data/canal_size.h" // float canal_size[3][2];
float texsc[3] = {0};
float canaltm[3] = {0};

static float ini_yvel = 200.0f;
static float grav = 250.0f;
static float ydec = 0.85f;
static float ldec = 2000.0f;
static float lvel = 20.0f;
float ligscl = 2.0f;
float cana1 = 22.5f;
float cana2 = 15.0f;
float cana3 = 1.6f;
float cana4 = 12.0f;
int setlight = 1;
int setpoly = 0;

void RunLeafSub(EFF_LEAF *lep)
{
    float f;
    float ss;
    float cc;
    float fw2;
    int c;
    int i;
    int j;
    int k;
    int w;
    int th;
    int tw;
    int so[16];
    int leaf_num;
    int disp[32];
    float grv1y = 3.0f;
    float mphs = 0.3f;
    u_char rr;
    u_char gg;
    u_char bb;
    u_long tx0;
    float rot_x;
    float rot_y;
    sceVu0IVECTOR ivec[16][5];
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0FVECTOR bpos;
    sceVu0FVECTOR wpos;
    sceVu0FVECTOR ppos[2][5] = {
        {
            {-6.0f, 13.5f, 0.0f, 1.0f},
            {+6.0f, 13.5f, 0.0f, 1.0f},
            {-6.0f, 0.0f, 0.0f, 1.0f},
            {+6.0f, 0.0f, 0.0f, 1.0f},
            {0.0f, 6.7f, 0.0f, 1.0f},
        },
        {
            {-20.0f, +20.0f, 0.0f, 1.0f},
            {+20.0f, +20.0f, 0.0f, 1.0f},
            {-20.0f, -20.0f, 0.0f, 1.0f},
            {+20.0f, -20.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 0.0f, 1.0f},
        }
    };
    EFF_LEAF_ONE *elo = lep->lo;

    if (lep->type == 0)
    {
        leaf_num = 8;
    }
    else
    {
        leaf_num = 12;
    }

    Vu0CopyVector(bpos, lep->bpos);

    for (i = 0; i < leaf_num; i++)
    {
        f = elo[i].mang;

        elo[i].pos[0] = +elo[i].vel[0] * elo[i].cnt;
        elo[i].pos[1] = -elo[i].vel[1] * elo[i].cnt + grv1y * elo[i].cnt * elo[i].cnt;

        if (elo[i].pos[1] > 0.0f)
        {
            elo[i].fl = 2;
        }
        else if (elo[i].opos[1] < elo[i].pos[1])
        {
            elo[i].fl = 1;
        }
        else
        {
            elo[i].fl = 0;
        }

        if (lep->type != 0 && elo[i].fl >= 1)
        {
            fw2 = mphs * 0.2f;
        }
        else
        {
            fw2 = mphs;
        }

        if (stop_effects == 0 && stop_lf == 0)
        {
#ifdef MATCHING_DECOMP
            elo[i].fl += 0; // fixes float regswap
#endif
            elo[i].cnt += fw2;
        }

        Vu0CopyVector(elo[i].opos, elo[i].pos);
        Vu0CopyVector(wpos, bpos);

        ss = SgSinf(f);
        cc = SgCosf(f);

        wpos[0] += elo[i].pos[0] * cc - elo[i].pos[2] * ss;
        wpos[1] += elo[i].pos[1];
        wpos[2] += elo[i].pos[0] * ss + elo[i].pos[2] * cc;

        sceVu0UnitMatrix(wlm);

        wlm[0][0] = wlm[1][1] = wlm[2][2] = 4.0f;

        if (lep->type == 0)
        {
            sceVu0RotMatrixX(wlm, wlm, elo[i].ang[0]);
            sceVu0RotMatrixY(wlm, wlm, elo[i].ang[1]);
        }
        else
        {
            Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);
            sceVu0RotMatrixX(wlm, wlm, rot_x);
            sceVu0RotMatrixY(wlm, wlm, rot_y);
        }

        sceVu0TransMatrix(wlm, wlm, wpos);
        sceVu0MulMatrix(slm, SgWSMtx, wlm);

        for (j = 0, w = 0; j < 5; j++)
        {
            sceVu0RotTransPers(ivec[i][j], slm, ppos[lep->type][j], 0);

            if (ivec[i][j][0] < 0x4000 || ivec[i][j][0] > 0xc000)
            {
                w = 1;
            }

            if (ivec[i][j][1] < 0x4000 || ivec[i][j][1] > 0xc000)
            {
                w = 1;
            }

            if (ivec[i][j][2] < 0xff || ivec[i][j][2] > 0x00ffffff)
            {
                w = 1;
            }
        }

        disp[i] = !w;
    }

    j = 0;

    for (i = 0; i < leaf_num; i++)
    {
        j += elo[i].fl;
    }

    if (j / 2 == leaf_num)
    {
        lep->flag = 0;
    }

    for (i = 0; i < leaf_num; i++)
    {
        so[i] = i;
    }

    for (j = 0; j < leaf_num - 1; j++)
    {
        for (i = j + 1; i < leaf_num; i++)
        {
            if (ivec[so[j]][4][2] > ivec[so[i]][4][2])
            {
                k = so[i];
                so[i] = so[j];
                so[j] = k;
            }
        }
    }

    if (lep->type == 0)
    {
        th = effdat[monochrome_mode + 20].h * 16;
        tw = effdat[monochrome_mode + 20].w * 16;
        tx0 = effdat[monochrome_mode + 20].tex0;
    }
    else
    {
        th = effdat[monochrome_mode + 6].h * 16;
        tw = effdat[monochrome_mode + 6].w * 16;
        tx0 = effdat[monochrome_mode + 6].tex0;
    }

    Reserve2DPacket(0x1000);

    c = ndpkt;

    pbuf[ndpkt++].ul128 = (u_long128)0;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    pbuf[ndpkt].ul64[0] = tx0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    for (j = 0; j < leaf_num; j++)
    {
        if (elo[j].fl != 2)
        {
            k = so[j];

            if (disp[k] != 0)
            {
                pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 1, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 3);
                pbuf[ndpkt++].ul64[1] = 0 \
                    | SCE_GS_RGBAQ << (4 * 0)
                    | SCE_GS_UV    << (4 * 1)
                    | SCE_GS_XYZF2 << (4 * 2);

                if (monochrome_mode != 0)
                {
                    gg = (elo[k].r + elo[k].g + elo[k].b) / 3;
                    bb = gg;
                    rr = gg;
                }
                else
                {
                    rr = elo[k].r;
                    gg = elo[k].g;
                    bb = elo[k].b;
                }

                for (i = 0; i < 4; i++)
                {
                    pbuf[ndpkt].ui32[0] = rr;
                    pbuf[ndpkt].ui32[1] = gg;
                    pbuf[ndpkt].ui32[2] = bb;
                    pbuf[ndpkt++].ui32[3] = elo[k].a;

                    pbuf[ndpkt].ui32[0] = i & 1 ? tw - 8 : 8;
                    pbuf[ndpkt].ui32[1] = i / 2 ? th - 8 : 8;
                    pbuf[ndpkt].ui32[2] = 0;
                    pbuf[ndpkt++].ui32[3] = 0;

                    pbuf[ndpkt].ui32[0] = ivec[k][i][0];
                    pbuf[ndpkt].ui32[1] = ivec[k][i][1];
                    pbuf[ndpkt].ui32[2] = ivec[k][i][2];
                    pbuf[ndpkt++].ui32[3] = i > 1 ? 0 : 0x8000;
                }
            }
        }
    }

    pbuf[c].ui32[0] = ndpkt + DMAend - c - 1;
}

void RunLeaf()
{
    int i;

    for(i = 0; i < 6; i++)
    {
        if (eff_leaf[i].flag != 0)
        {
            RunLeafSub(&eff_leaf[i]);
        }
    }
}

void light_rgb(sceVu0IVECTOR c0, sceVu0FMATRIX LocalLightMatrix, sceVu0FMATRIX LightColorMatrix, float *v0, float *v1, float *v2, float *c1)
{
    sceVu0FVECTOR vector;

    Vu0SubOuterProduct(vector, v0, v2, v1);
    Vu0Normalize(vector, vector);
    Vu0ApplyMatrix(vector, LocalLightMatrix, vector);
    sceVu0ClampVector(vector, vector, 0.0f, 1.0f);
    vector[3] = 1.0f;
    Vu0ApplyMatrix(vector, LightColorMatrix, vector);
    sceVu0ClampVector(vector, vector, 0.0f, 1.0f);
    Vu0MulVector(vector, c1, vector);
    Vu0FTOI0Vector(c0, vector);
}

u_char SetCanalSub(int no, float *mpos)
{
    int tagnum;
    int cnt;
    u_long reg;
    RIP_WORK1 rw1;
    RIP_WORK2 rw2;
    RIPPLE2_M *rippm;
    sceVu0FVECTOR rip_light0 = {0.0f, 0.1f, -1.0f, 1.0f};
    sceVu0FVECTOR rip_light1 = {0.0f, 1.0f, 0.0f, 1.0f};
    sceVu0FVECTOR rip_light2 = {0.0f, 0.1f, +1.0f, 1.0f};
    sceVu0FVECTOR rip_color0 = {1.5f, 1.6f, 1.8f, 1.0f};
    sceVu0FVECTOR rip_color1 = {0.1f, 0.1f, 0.1f, 1.0f};
    sceVu0FVECTOR rip_color2 = {0.0f, 0.0f, 0.1f, 1.0f};
    sceVu0FVECTOR rip_ambient = {0.0f, 0.0f, 0.0f, 0.0f};
    u_int clpx2 = 0xfd00;
    u_int clpy2 = 0xfd00;
    u_int clpz2 = 0xfffffff;
    int i;
    int j;
    int k;
    int t;
    int m;
    int n;
    int wix;
    int wiy;
    float f;
    float l;
    float lw;
    float y;
    int v1;
    int v2;
    int v3;
    sceVu0FVECTOR wpos;
    sceVu0FMATRIX wlm;
    sceVu0IVECTOR c0;
    sceVu0FVECTOR c1;
    sceVu0FVECTOR sufcol;
    sceVu0FMATRIX NormalLightMatrix;
    sceVu0FMATRIX LightColorMatrix;
    sceVu0FMATRIX LocalLightMatrix;
    RIPPLE2 *ripp;
    float far = 5500.0f;
    float near = 800.0f;
    int tt = 0x2000;

    rw1.tsw = canal_size[no][0] / 24.0f;
    rw1.tsh = canal_size[no][1] / 10.0f;
    rw1.pnumw = 0x18;
    rw1.pnumh = 10;
    rw1.vnumw = 0x19;
    rw1.vnumh = 0xb;

    rw2.tsw = canal_size[no][0] * 0.25f;
    rw2.tsh = canal_size[no][1] * 0.5f;
    rw2.pnumw = 4;
    rw2.pnumh = 2;
    rw2.vnumw = 5;
    rw2.vnumh = 3;

    Vu0CopyVector(rw1.bpos, mpos);
    Vu0CopyVector(rw2.bpos, mpos);

    rw1.bpos[1] += 80.0f;

    sufcol[2] = sufcol[1] = sufcol[0] = 255.0f;
    sufcol[3] = 0.0f;

#ifdef MATCHING_DECOMP
    ripp = &rip[j]; // fixes reg+stack swap
#endif

    if (setlight != 0)
    {
        sceVu0UnitMatrix(wlm);

        wlm[0][0] = wlm[1][1] = wlm[2][2] = ligscl;

        sceVu0NormalLightMatrix(NormalLightMatrix, rip_light0, rip_light1, rip_light2);
        sceVu0LightColorMatrix(LightColorMatrix, rip_color0, rip_color1, rip_color2, rip_ambient);
        sceVu0MulMatrix(LocalLightMatrix, NormalLightMatrix, wlm);
    }

    t = (u_int)(texsc[no] * 16);

    sceVu0UnitMatrix(wlm);
    sceVu0TransMatrix(wlm, wlm, rw1.bpos);
    sceVu0MulMatrix(rw1.slm, SgWSMtx, wlm);

    for (i = 0; i < rw1.vnumw * rw1.vnumh; i++)
    {
        wix = i % rw1.vnumw;
        wiy = i / rw1.vnumw;

        rw1.vt[i][0] = wix * rw1.tsw - canal_size[no][0] * 0.5f;
        rw1.vt[i][1] = 0.0f;
        rw1.vt[i][2] = wiy * rw1.tsh - canal_size[no][1] * 0.5f;
        rw1.vt[i][3] = 1.0f;

        rw1.tx[i] = (wix * 256) / rw1.pnumw * 16 + t;
        rw1.ty[i] = (wiy * 128) / rw1.pnumh * 16;
    }


    for (i = 0; i < rw1.vnumw * rw1.vnumh; i++)
    {
        wix = i % rw1.vnumw;
        wiy = i / rw1.vnumw;
        f = wix * cana1 + wiy * cana2 + canaltm[no];
        rw1.vt[i][1] = SgSinf((f * PI) / 180.0f) * cana4;
    }

    if (stop_effects == 0)
    {
        canaltm[no] = 360.0f > canaltm[no] + cana3 ? canaltm[no] + cana3 : canaltm[no] + cana3 - 360.0f;
    }

    for (j = 0; j < 8; j++)
    {
        if (rip[j].sufno == no + 1)
        {
            ripp = &rip[j];

            for (i = 0; i < rw1.vnumw * rw1.vnumh; i++)
            {
                rippm = &ripp->rip2m[i];
                lw = (int)(rippm->dist / 4);

                if (lw < ripp->time)
                {
                    y = rippm->v * rippm->t - grav * rippm->t * rippm->t * 0.5f;

                    if (rippm->mode != 0)
                    {
                        y = -y;
                    }

                    if (stop_effects == 0)
                    {
                        if (y >= 0 && rippm->oy < 0.0f)
                        {
                            rippm->mode = 0;
                            rippm->v *= ydec;
                            rippm->t = 0.0f;
                        }
                        if (y <= 0.0f && rippm->oy > 0.0f)
                        {
                            rippm->mode = 1;
                            rippm->v *= ydec;
                            rippm->t = 0.0f;
                        }
                    }

                    if (rippm->v < 0.1f)
                    {
                        rippm->v = 0.0f;

                    }
                    else
                    {
                        if (stop_effects == 0)
                        {
                            rippm->oy = y;
                            rippm->t += 0.1f;
                        }

                        l = 1.0f - lw / ldec;
                        l = (l < 0.0f) ? 0.0f : l;
                        rw1.vt[i][1] += -y * l;
                    }
                }
            }

            if (stop_effects == 0)
            {
                ripp->time += lvel;
            }
        }
    }

    for (i = 0; i < rw1.vnumw * rw1.vnumh; i++)
    {
        sceVu0ApplyMatrix(wpos, wlm, rw1.vt[i]);
        l = Get2PLength(camera.p, wpos);
        rw1.fg[i] = far <= l ? 0 : (l <= near ? 0xff : (u_int)(((far - l) * 255.0f) / (far - near)));
    }

    sceVu0RotTransPersN(rw1.vtiw, rw1.slm, rw1.vt, rw1.vnumw * rw1.vnumh, 1);

    j = rw1.pnumw + 1;
    m = rw1.vnumw * rw1.vnumh;

    if (setlight != 0)
    {
        f = 255.0f / m;
    }
    else
    {
        f = 128.0f / m;
    }

    for (i = 0; i < rw1.vnumw * rw1.vnumh; i++)
    {
        int wix = i % (rw1.pnumw + 1); // local ??
        int wiy = i / (rw1.pnumw + 1); // local ??

        if (wix == rw1.pnumw && wiy == rw1.pnumh)
        {
            v1 = i - 1;
            v2 = i;
            v3 = i - j;
        }
        else if (wix == rw1.pnumw && wiy != rw1.pnumh)
        {
            v1 = i - 1;
            v2 = i + j - 1;
            v3 = i;
        }
        else if (wix != rw1.pnumw && wiy == rw1.pnumh)
        {
            v1 = i - j;
            v2 = i;
            v3 = i - j + 1;
        }
        else if (wix != rw1.pnumw && wiy != rw1.pnumh)
        {
            v1 = i;
            v2 = i + j;
            v3 = i + 1;
        }

        if (setpoly != 0)
        {
            c1[2] = c1[1] = c1[0] = i * f;
            c1[3] = 0.0f;
            light_rgb(rw1.col[i], LocalLightMatrix, LightColorMatrix, rw1.vt[v1], rw1.vt[v2], rw1.vt[v3], c1);
        }
        else
        {
            light_rgb(rw1.col[i], LocalLightMatrix, LightColorMatrix, rw1.vt[v1], rw1.vt[v2], rw1.vt[v3], sufcol);
        }
    }

    for (i = 0; i < rw1.vnumw * rw1.vnumh; i++)
    {
        rw1.clip[i] = 1;

        if ((rw1.vtiw[i][0] >= 0 && rw1.vtiw[i][0] < 0x300) || clpx2 < rw1.vtiw[i][0])
        {
            rw1.clip[i] = 0;
        }

        if ((rw1.vtiw[i][1] >= 0 && rw1.vtiw[i][1] < 0x300) || clpy2 < rw1.vtiw[i][1])
        {
            rw1.clip[i] = 0;
        }

        if ((rw1.vtiw[i][2] == 0) || clpz2 < rw1.vtiw[i][2])
        {
            rw1.clip[i] = 0;
        }
    }

    sceVu0UnitMatrix(wlm);
    sceVu0TransMatrix(wlm, wlm, rw2.bpos);
    sceVu0MulMatrix(rw2.slm, SgWSMtx, wlm);

    for (i = 0; i < rw2.vnumw * rw2.vnumh; i++)
    {
        wix = i % rw2.vnumw;
        wiy = i / rw2.vnumw;

        rw2.vt[i][0] = wix * rw2.tsw - canal_size[no][0] * 0.5f;
        rw2.vt[i][1] = 0.0f;
        rw2.vt[i][2] = wiy * rw2.tsh - canal_size[no][1] * 0.5f;
        rw2.vt[i][3] = 1.0f;

        rw2.tx[i] = (wix * 256 / rw2.pnumw) * 16 - (t * 2 - tt);
        rw2.ty[i] = (wiy * 128 / rw2.pnumh) * 16;

        sceVu0ApplyMatrix(wpos, wlm, rw2.vt[i]);
        l = Get2PLength(camera.p, wpos);
        rw2.fg[i] = far <= l ? 0 : (l <= near ? 0xff : (u_int)(((far - l) * 255.0f) / (far - near)));
    }

    sceVu0RotTransPersN(rw2.vtiw, rw2.slm, rw2.vt, rw2.vnumw * rw2.vnumh, 1);

    for (i = 0; i < rw2.vnumw * rw2.vnumh; i++)
    {
        rw2.clip[i] = 1;

        if ((rw2.vtiw[i][0] >= 0 && rw2.vtiw[i][0] < 0x300) || clpx2 < rw2.vtiw[i][0])
        {
            rw2.clip[i] = 0;
        }

        if ((rw2.vtiw[i][1] >= 0 && rw2.vtiw[i][1] < 0x300) || clpy2 < rw2.vtiw[i][1])
        {
            rw2.clip[i] = 0;
        }

        if ((rw2.vtiw[i][2] == 0) || clpz2 < rw2.vtiw[i][2])
        {
            rw2.clip[i] = 0;
        }
    }

    if (stop_effects == 0)
    {
        texsc[no] = texsc[no] + 0.02f > 256.0f ? texsc[no] + 0.02f - 256.0f : texsc[no] + 0.02f;
    }

    Reserve2DPacket(0x1000);

    n = ndpkt;

    pbuf[ndpkt++].ul128 = (u_long128)0;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(8, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    pbuf[ndpkt].ul64[0] = effdat[monochrome_mode + 54].tex0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_CLAMP_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_FOGCOL(0x13, 0x13, 0x14);
    pbuf[ndpkt++].ul64[1] = SCE_GS_FOGCOL;

    m = rw1.pnumh * (rw1.pnumw + 1);

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 0, SCE_GIF_REGLIST, 2);
    pbuf[ndpkt++].ul64[1] = 0xf0;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 1, 1, 1, 1, 0, 1, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_PRIM;

    tagnum = ndpkt++;

    reg = 0;
    cnt = 0;

    for (i = 0; i < m; i++)
    {
        j = i + rw1.pnumw+1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_UV(rw1.tx[i], rw1.ty[i]);
        pbuf[ndpkt++].ul64[1] = SCE_GS_SET_RGBAQ((u_char)rw1.col[i][0], (u_char)rw1.col[i][1], (u_char)rw1.col[i][2], 0x38, 0x3f800000);

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_XYZF(rw1.vtiw[i][0], rw1.vtiw[i][1], rw1.vtiw[i][2], rw1.fg[i]);

        k = i % (rw1.pnumw+1) ? rw1.clip[i-1] + rw1.clip[j-1] + rw1.clip[i] : 0;

        if (k >= 3)
        {
            reg |= (long)(0 \
                | SCE_GS_UV    << (4 * 0)
                | SCE_GS_RGBAQ << (4 * 1)
                | SCE_GS_XYZF2 << (4 * 2)
            ) << (cnt * 12);
        }
        else
        {
            reg |= (long)(0 \
                | SCE_GS_UV    << (4 * 0)
                | SCE_GS_RGBAQ << (4 * 1)
                | SCE_GS_XYZF3 << (4 * 2)
            ) << (cnt * 12);
        }
        pbuf[ndpkt++].ul64[1] = SCE_GS_SET_UV(rw1.tx[j], rw1.ty[j]);

        cnt++;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_RGBAQ((u_char)rw1.col[j][0], (u_char)rw1.col[j][1], (u_char)rw1.col[j][2], 0x38, 0x3f800000);
        pbuf[ndpkt++].ul64[1] = SCE_GS_SET_XYZF(rw1.vtiw[j][0], rw1.vtiw[j][1], rw1.vtiw[j][2], rw1.fg[j]);

        k = i % (rw1.pnumw+1) ? rw1.clip[j-1] + rw1.clip[i] + rw1.clip[j] : 0;

        if (k >= 3)
        {
            reg |= (long)(0 \
                | SCE_GS_UV    << (4 * 0)
                | SCE_GS_RGBAQ << (4 * 1)
                | SCE_GS_XYZF2 << (4 * 2)
            ) << (cnt * 12);
        }
        else
        {
            reg |= (long)(0 \
                | SCE_GS_UV    << (4 * 0)
                | SCE_GS_RGBAQ << (4 * 1)
                | SCE_GS_XYZF3 << (4 * 2)
            ) << (cnt * 12);
        }

        cnt++;

        if (cnt > 3)
        {
            pbuf[tagnum].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 0, SCE_GIF_REGLIST, cnt * 3);
            pbuf[tagnum].ul64[1] = reg;

            tagnum = ndpkt++;

            reg = 0;
            cnt = 0;
        }
    }
    if (cnt >= 1)
    {
        pbuf[tagnum].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 0, SCE_GIF_REGLIST, cnt * 3);
        pbuf[tagnum].ul64[1] = reg;
    }
    else
    {
        ndpkt--;
    }

    if (setpoly == 0)
    {
        m = rw2.pnumh * (rw2.pnumw + 1);

        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 0, SCE_GIF_REGLIST, 2);
        pbuf[ndpkt++].ul64[1] = 0xf0;

        pbuf[ndpkt].ul64[0] = 0x17c;
        pbuf[ndpkt++].ul64[1] = 0;

        tagnum = ndpkt++;

        reg = 0;
        cnt = 0;

        for (i = 0; i < m; i++)
        {
            j = i + rw2.pnumw+1;

            pbuf[ndpkt].ul64[0] = pbuf[ndpkt].ul64[0] = SCE_GS_SET_UV(rw2.tx[i], rw2.ty[i]);
            pbuf[ndpkt++].ul64[1] = SCE_GS_SET_RGBAQ(0x80, 0x80, 0x80, 0x30, 0x3f800000);

            pbuf[ndpkt].ul64[0] = SCE_GS_SET_XYZF(rw2.vtiw[i][0], rw2.vtiw[i][1], rw2.vtiw[i][2], rw2.fg[i]);

            k = i % (rw2.pnumw+1) ? rw2.clip[i-1] + rw2.clip[j-1] + rw2.clip[i] : 0;

            if (k >= 3)
            {
                reg |= (long)(0 | SCE_GS_UV    << (4 * 0) | SCE_GS_RGBAQ << (4 * 1) | SCE_GS_XYZF2 << (4 * 2)) << (cnt * 12);
            }
            else
            {
                reg |= (long)(0 | SCE_GS_UV    << (4 * 0) | SCE_GS_RGBAQ << (4 * 1) | SCE_GS_XYZF3 << (4 * 2)) << (cnt * 12);
            }
            pbuf[ndpkt++].ul64[1] = SCE_GS_SET_UV(rw2.tx[j], rw2.ty[j]);

            cnt++;

            pbuf[ndpkt].ul64[0] = SCE_GS_SET_RGBAQ(0x80, 0x80, 0x80, 0x30, 0x3f800000);
            pbuf[ndpkt++].ul64[1] = SCE_GS_SET_XYZF(rw2.vtiw[j][0], rw2.vtiw[j][1], rw2.vtiw[j][2], rw2.fg[j]);

            k = i % (rw2.pnumw+1) ? rw2.clip[j-1] + rw2.clip[i] + rw2.clip[j] : 0;

            if (k >= 3)
            {
                reg |= (long)(0 | SCE_GS_UV    << (4 * 0) | SCE_GS_RGBAQ << (4 * 1) | SCE_GS_XYZF2 << (4 * 2)) << (cnt * 12);
            }
            else
            {
                reg |= (long)(0 | SCE_GS_UV    << (4 * 0) | SCE_GS_RGBAQ << (4 * 1) | SCE_GS_XYZF3 << (4 * 2)) << (cnt * 12);
            }

            cnt++;

            if (cnt > 3)
            {
                pbuf[tagnum].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 0, SCE_GIF_REGLIST, cnt * 3);
                pbuf[tagnum].ul64[1] = reg;

                tagnum = ndpkt++;

                reg = 0;
                cnt = 0;
            }
        }

        if (cnt >= 1)
        {
            pbuf[tagnum].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 0, SCE_GIF_REGLIST, cnt * 3);
            pbuf[tagnum].ul64[1] = reg;
        }
        else
        {
            ndpkt--;
        }
    }

    pbuf[n].ui32[0] = ndpkt + DMAend - n - 1;

    return 0;
}

void Call3DRipple(sceVu0FVECTOR bpos)
{
    static int m = 0;
    int i;
    int n;
    int vnumw;
    int vnumh;
    float cntw;
    float cnth;
    float tsw;
    float tsh;
    float x1;
    float x2;
    float x3;
    float z1;
    float z2;
    float z3;

    vnumw = 25;
    vnumh = 11;

    cntw = bpos[0];
    cnth = bpos[2];

    n = -1;

    if (
        bpos[0] < canal_bpos[0][0] + canal_size[0][0] * 0.5f &&
        bpos[0] > canal_bpos[0][0] - canal_size[0][0] * 0.5f &&
        bpos[2] < canal_bpos[0][2] + canal_size[0][1] * 0.5f &&
        bpos[2] > canal_bpos[0][2] - canal_size[0][1] * 0.5f
    )
    {
        n = 0;
    }

    if (
        bpos[0] < canal_bpos[1][0] + canal_size[1][0] * 0.5f &&
        bpos[0] > canal_bpos[1][0] - canal_size[1][0] * 0.5f &&
        bpos[2] < canal_bpos[1][2] + canal_size[1][1] * 0.5f &&
        bpos[2] > canal_bpos[1][2] - canal_size[1][1] * 0.5f
    )
    {
        n = 1;
    }

    if (
        bpos[0] < canal_bpos[2][0] + canal_size[2][0] * 0.5f &&
        bpos[0] > canal_bpos[2][0] - canal_size[2][0] * 0.5f &&
        bpos[2] < canal_bpos[2][2] + canal_size[2][1] * 0.5f &&
        bpos[2] > canal_bpos[2][2] - canal_size[2][1] * 0.5f
    )
    {
        n = 2;
    }

    if (n != -1)
    {
        m = (m + 1) % 8;

        tsw = canal_size[n][0] / 24.0f;
        tsh = canal_size[n][1] / 10.0f;

        for (i = 0; i < vnumh * vnumw; i++)
        {
            rip[m].rip2m[i].v = ini_yvel;
            rip[m].rip2m[i].t = 0.0f;
            rip[m].rip2m[i].oy = 0.0f;
            rip[m].rip2m[i].mode = 0;

            x1 = (int)(i % vnumw) * tsw - canal_size[n][0] * 0.5f;
            z1 = (int)(i / vnumw) * tsh - canal_size[n][1] * 0.5f;

            x2 = cntw - canal_bpos[n][0];
            z2 = cnth - canal_bpos[n][2];

            x3 = x2 - x1;
            z3 = z2 - z1;

            rip[m].rip2m[i].dist = SgSqrtf(x3 * x3 + z3 * z3) * 4.0f;
        }

        rip[m].time = 0.0f;
        rip[m].sufno = n + 1;
    }
}

void ContCanal()
{
    static sceVu0FVECTOR bcp = {27850.0f, 200.0f, 29300.0f, 1.0f};
    int n;

    if (*key_now[15] == 1)
    {
        Call3DRipple(bcp);
    }

    if (*key_now[7] != 0)
    {
        bcp[0] += 20.0f;
    }

    if (*key_now[6] != 0)
    {
        bcp[0] -= 20.0f;
    }

    if (*key_now[4] != 0)
    {
        bcp[2] += 20.0f;
    }

    if (*key_now[5] != 0)
    {
        bcp[2] -= 20.0f;
    }

    n = 0;

    if (
        (bcp[0] < canal_bpos[0][0] + canal_size[0][0] * 0.5f) &&
        (canal_bpos[0][0] - canal_size[0][0] * 0.5f < bcp[0]) &&
        (bcp[2] < canal_bpos[0][2] + canal_size[0][1] * 0.5f) &&
        (canal_bpos[0][2] - canal_size[0][1] * 0.5f < bcp[2])
    )
    {
        n = 1;
    }

    if (
        (bcp[0] < canal_bpos[1][0] + canal_size[1][0] * 0.5f) &&
        (canal_bpos[1][0] - canal_size[1][0] * 0.5f < bcp[0]) &&
        (bcp[2] < canal_bpos[1][2] + canal_size[1][1] * 0.5f) &&
        (canal_bpos[1][2] - canal_size[1][1] * 0.5f < bcp[2])
    )
    {
        n = 1;
    }

    if (
        (bcp[0] < canal_bpos[2][0] + canal_size[2][0] * 0.5f) &&
        (canal_bpos[2][0] - canal_size[2][0] * 0.5f < bcp[0]) &&
        (bcp[2] < canal_bpos[2][2] + canal_size[2][1] * 0.5f) &&
        (canal_bpos[2][2] - canal_size[2][1] * 0.5f < bcp[2])
    )
    {
        n = 1;
    }

    DrawPoint(bcp, n);
}

#include "data/eneface.h" // static SPRT_DAT eneface[];
int cont_facespr_tbl[3] = {0};
int cont_facespr_otbl[3] = {0};
FACESPIRIT_ST facespirit[3] = {0};

void SetCanal()
{
    int i;

    if (plyr_wrk.pr_info.room_no == 23)
    {
        for (i = 0; i < 3; i++)
        {
            SetCanalSub(i, canal_bpos[i]);
        }
    }
}

void SetGlowOfAFirefly(float *p, float sc, u_char r1, u_char g1, u_char b1, u_char r2, u_char g2, u_char b2, u_char alp)
{
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0IVECTOR ipos;
    sceVu0IVECTOR ivec[4];
    sceVu0FVECTOR vpos;
    sceVu0FVECTOR vtw[4];
    sceVu0FVECTOR wpos[4] = {
        {-1.0f, +1.0f, 0.0f, 1.0f},
        {+1.0f, +1.0f, 0.0f, 1.0f},
        {-1.0f, -1.0f, 0.0f, 1.0f},
        {+1.0f, -1.0f, 0.0f, 1.0f},
    };
    float f;
    float rot_x;
    float rot_y;
    int i;
    int w;
    int n;

    Vu0CopyVector(vpos, p);
    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);
    sceVu0UnitMatrix(wlm);

    wlm[0][0] = wlm[1][1] = wlm[2][2] = sc;

    sceVu0RotMatrixX(wlm, wlm, rot_x);
    sceVu0RotMatrixY(wlm, wlm, rot_y);
    sceVu0TransMatrix(wlm, wlm, vpos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);

    w = 0;

    for (i = 0; i < 4; i++)
    {
        sceVu0RotTransPers(ivec[i], slm, wpos[i], 0);

        if (0x5000 < ivec[i][0] - 0x5800U)
        {
            w = 1;
        }

        if (0x1c00 < ivec[i][1] - 0x7200U)
        {
            w = 1;
        }

        if (0xffff00 < ivec[i][2] - 0xffU)
        {
            w = 1;
        }
    }

    if (!w)
    {

        ipos[0] = (ivec[0][0] + ivec[3][0]) / 2;
        ipos[1] = (ivec[0][1] + ivec[3][1]) / 2;
        ipos[2] = (ivec[0][2] + ivec[3][2]) / 2;
        ipos[3] = 0;

        n = (((ivec[3][1] - ivec[0][1]) / 2) < ((ivec[3][0] - ivec[0][0]) / 2) ? ((ivec[3][0] - ivec[0][0]) / 2) : ((ivec[3][1] - ivec[0][1]) / 2));
        f = n * 0.0625f;

        n = alp;

        SetEffSQITex(24, ipos, 1, f * 7.0f, f * 7.0f, r1, g1, b1, n / 4);
        SetEffSQITex(24, ipos, 1, f, f, r2, g2, b2, n);
    }
}

void SetFireflySub(FIREFLY *ffp)
{
    sceVu0FVECTOR rot;
    sceVu0FVECTOR pos;
    sceVu0FMATRIX mtx;
    int cnt = 20;
    float fw1;
    float fw2;
    float rate = 0.1f;
    float top = 0.0f;
    float under; // unused?
    float ph;

    if (ffp->time <= 0)
    {
        if (ffp->vel[2] != 0.0f)
        {
            fw1 = vu0Rand();
            fw1 = fw1 * 2.0f - 1.0f;
            fw1 = fw1 * 0.5f;

            if (ffp->npos[1] > 200.0f)
            {
                fw2 = (vu0Rand() - 1.0f) * 0.5f;
            }
            else if (ffp->npos[1] < top)
            {
                fw2 = vu0Rand() * 0.5f;
            }
            else
            {
                fw2 = (vu0Rand() * 2.0f - 1.0f) * 0.5f;
            }

            ffp->vel[0] = ffp->vel[0] + fw1 > 0.7f || ffp->vel[0] + fw1 < -0.7f ? ffp->vel[0] : ffp->vel[0] + fw1;
            ffp->vel[1] = ffp->vel[1] + fw2 > 0.7f || ffp->vel[1] + fw2 < -0.7f ? ffp->vel[1] : ffp->vel[1] + fw2;

            ffp->vel[0] *= rate;
            ffp->vel[1] *= rate;
        }

        ffp->time = cnt;
    }

    GetTrgtRotType2(ffp->bpos,ffp->npos,rot,3);

    while (rot[0] < -PI) rot[0] += PI2;
    while (PI <= rot[0]) rot[0] -= PI2;
    while (rot[1] < -PI) rot[1] += PI2;
    while (PI <= rot[1]) rot[1] -= PI2;

    sceVu0UnitMatrix(mtx);
    sceVu0RotMatrix(mtx,mtx,rot);
    sceVu0TransMatrix(mtx,mtx,ffp->npos);
    sceVu0ApplyMatrix(pos,mtx,ffp->vel);

    Vu0CopyVector(ffp->bpos, ffp->npos);
    Vu0CopyVector(ffp->npos, pos);

    if (ffp->vel[2] != 0.0f)
    {
        fw1 = ((vu0Rand() * 2.0f) - 1.0f) / 5.0f;
        under = ffp->vel[2] + fw1;
        under = under < 10.0f ? 8.0f : (12.0f < under ? 12.0f : under);
        ffp->vel[2] = under * rate;
    }

    if (ffp->life >= 836)
    {
        ffp->alp = 900 - ffp->life;
    }
    else if (ffp->life < 65)
    {
        ffp->alp = ffp->life;
    }
    else
    {
        ph = ((((836 - ffp->life) * 1080.0f) / 772.0f) * PI);
        ffp->alp = (SgCosf(ph / 180.0f) + 1.0f) * 64.0f * 0.5f + 12.0f;
    }

    ffp->life--;
    ffp->time--;
}

void SetFirefly()
{
    static int wait = 0;
    static int no = 0;
    static FIREFLY ff[20];
    int i;
    float rate = 0.1f;
    sceVu0FVECTOR fpos;
    sceVu0FVECTOR bpos = { 19800.0f, 100.0f, 36500.0f, 1.0f };
    sceVu0FMATRIX mtx;
    FIREFLY *ffp;

    if (plyr_wrk.pr_info.room_no == 22)
    {
        if (stop_effects == 0 && wait == 0)
        {
            ffp = &ff[no];

            Vu0CopyVector(ffp->npos, bpos);

            ffp->npos[0] = ffp->npos[0] + ((vu0Rand() * 2.0f) - 1.0f) * 7000.0f;
            ffp->npos[1] = ffp->npos[1] + ((vu0Rand() * 2.0f) - 1.0f) * 100.0f;
            ffp->npos[2] = ffp->npos[2] + ((vu0Rand() * 2.0f) - 1.0f) * 3000.0f;

            if (vu0Rand() * 2.0f < 1.0f)
            {
                ffp->vel[0] = 0.0f;
                ffp->vel[1] = 0.0f;
                ffp->vel[2] = 0.0f;
            }
            else
            {
                ffp->vel[0] = vu0Rand() * 2.0f - 1.0f;
                ffp->vel[1] = vu0Rand() * 2.0f - 1.0f;
                ffp->vel[2] = vu0Rand() * 5.0f + 10.0f;
            }

            ffp->vel[3] = 1.0f;
            ffp->time = 20;
            ffp->life = 900;
            ffp->vel[0] *= rate;
            ffp->vel[1] *= rate;
            ffp->vel[2] *= rate;

            sceVu0UnitMatrix(mtx);
            sceVu0TransMatrix(mtx, mtx,ffp->npos);
            sceVu0ApplyMatrix(fpos, mtx, ffp->vel);

            Vu0CopyVector(ffp->bpos, ffp->npos);
            Vu0CopyVector(ffp->npos, fpos);

            wait = 45;
            no = no + 1 >= 20 ? 0 : no + 1;
        }

        for (i = 0; i < 20; i++)
        {
            if (ff[i].life != 0)
            {
                if (stop_effects == 0)
                {
                    SetFireflySub(&ff[i]);
                }

                SetGlowOfAFirefly(ff[i].npos, 7.0f, 0xdc, 0xff, 0x80, 0x6a, 0xa0, 0x10, ff[i].alp);
            }
        }

        if (stop_effects == 0)
        {
            wait--;
        }
    }
}

void SetSky()
{
    float fx;
    float fz;
    float l;
    sceVu0IVECTOR ivec;
    sceVu0FVECTOR cpos;
    sceVu0FVECTOR rot;
    float length;
    float height;
    float sc_speed;
    int clip;
    int clpx2;
    int clpy2;
    u_int clpz2;
    float wline;
    u_int u1;
    u_int u2;
    u_int x1;
    u_int x2;
    u_int y1;
    u_int y2;
    int hori;
    int hline;
    int c;

    clpx2 = 0xc000;
    clpy2 = 0x8f00;
    clpz2 = 0xffffff;

    length = 5000.0f;
    wline = 400.0f;
    sc_speed = 6.0f;

    if (
        plyr_wrk.pr_info.room_no == 0x10 || plyr_wrk.pr_info.room_no == 0x16 ||
        plyr_wrk.pr_info.room_no == 0x15 || plyr_wrk.pr_info.room_no == 0x18 ||
        plyr_wrk.pr_info.room_no == 0x26 || plyr_wrk.pr_info.room_no == 0x19
    )
    {
        fx = (camera.i[0] - camera.p[0]) * (camera.i[0] - camera.p[0]);
        fz = (camera.i[2] - camera.p[2]) * (camera.i[2] - camera.p[2]);

        l = SgSqrtf(fx + fz);

        cpos[0] = camera.p[0] + ((camera.i[0] - camera.p[0]) * length) / l;
        cpos[1] = wline;
        cpos[2] = camera.p[2] + ((camera.i[2] - camera.p[2]) * length) / l;
        cpos[3] = 1.0f;

        sceVu0RotTransPers(ivec, SgWSMtx, cpos, 0);

        clip = 0;

        if (ivec[0] < 0x4000 || clpx2 < ivec[0])
        {
            clip = 1;
        }

        if (ivec[1] < 0x7800 || clpy2 < ivec[1])
        {
            clip = 1;
        }
        if ((ivec[2] >= 0 && ivec[2] < 0xf) || ivec[2] > clpz2)
        {
            clip = 1;
        }

        if (!clip)
        {
            GetTrgtRot(camera.p, camera.i, rot, 2);

            hori = ivec[1] / 16 - 1936;
            hline = hori < 128 ? 128 : (352 < hori ? 352 : hori);

            height = (rot[1] * 256.0f) * sc_speed / PI;
            while (height <    0.0f) height += 256.0f;
            while (height >= 256.0f) height -= 256.0f;

            x1 = 0x6c00;
            x2 = 0x9400;

            y1 = ivec[1];
            y2 = ivec[1] - (hline * 16);

            { // MACRO ?
            sceVu0IVECTOR ivec;
            ((float *)ivec)[0] = height;
            ((float *)ivec)[1] = height + 639.0f;
            _ftoi4(ivec, (float *)ivec);
            u1 = ivec[0];
            u2 = ivec[1];
            }

            Reserve2DPacket(0x1000);

            c = ndpkt;

            pbuf[ndpkt].ul128 = (u_long128)0;
            pbuf[ndpkt++].ui32[0] = DMAend;

            pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(7, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
            pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

            pbuf[ndpkt].ul64[0] = 0;
            pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

            pbuf[ndpkt].ul64[0] = effdat[monochrome_mode + 50].tex0;
            pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

            pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
            pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

            pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
            pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

            pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
            pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

            pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
            pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

            pbuf[ndpkt].ul64[0] = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_CLAMP, 0, 0, 0, 0);
            pbuf[ndpkt++].ul64[1] = SCE_GS_CLAMP_1;

            pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 342, SCE_GIF_PACKED, 5);
            pbuf[ndpkt++].ul64[1] = 0 \
                | SCE_GS_RGBAQ << (4 * 0)
                | SCE_GS_UV    << (4 * 1)
                | SCE_GS_XYZF2 << (4 * 2)
                | SCE_GS_UV    << (4 * 3)
                | SCE_GS_XYZF2 << (4 * 4);

            pbuf[ndpkt].ui32[0] = 0x40;
            pbuf[ndpkt].ui32[1] = 0x40;
            pbuf[ndpkt].ui32[2] = 0x40;
            pbuf[ndpkt++].ui32[3] = 0x80;

            pbuf[ndpkt].ui32[0] = u1;
            pbuf[ndpkt].ui32[1] = 0;
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = x1;
            pbuf[ndpkt].ui32[1] = y1;
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0x8000;

            pbuf[ndpkt].ui32[0] = u2;
            pbuf[ndpkt].ui32[1] = hline * 16;
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = x2;
            pbuf[ndpkt].ui32[1] = y2;
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[c].ui32[0] = ndpkt + DMAend - c - 1;
        }
    }
}

void SetPond()
{
    static float wave = 0.0f;
    static float texsc = 0.0f;
    int i;
    int j;
    int k;
    int t;
    int m;
    int n;
    int tagnum;
    int cnt;
    int wix;
    int wiy;
    int pnumw;
    int pnumh;
    int vnumw;
    int vnumh;
    u_int clpx2;
    u_int clpy2;
    u_int clpz2;
    float f;
    float l;
    float tsh;
    float fcana2;
    float fcana3;
    float fcana4;
    float far;
    float near;
    u_long reg;
    TEX_WORK txw[1089];
    sceVu0FVECTOR bpos = {19000.0f, 630.0f, 38000.0f, 1.0f};
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0FVECTOR wfv;
    sceVu0FVECTOR wpos;

    clpx2 = 0xfd00;
    clpy2 = 0xfd00;
    clpz2 = 0x0fffffff;

    fcana2 = 90.0f;
    // fcana3 = ???
    fcana4 = 16.0f;

    far = 5500.0f;
    near = 2000.0f;

    if (plyr_wrk.pr_info.room_no != 22 || EnemyUseJudge(0) != 0)
    {
        return;
    }

    pnumw = 0x20;
    pnumh = 0x20;
    vnumw = 0x21;
    vnumh = 0x21;

    tsh = 625.0f;

    sceVu0UnitMatrix(wlm);
    sceVu0TransMatrix(wlm, wlm, bpos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);

    t = (u_int)(texsc * fcana4);

    for (i = 0; i < vnumw*vnumh; i++)
    {
        wix = i % vnumw;
        wiy = i / vnumw;

        f = wix * fcana2 + wiy * fcana2 + wave;

        wfv[0] = wix * tsh - 10000.0f;
        wfv[1] = SgSinf((f * PI) / 180.0f) * fcana4;
        wfv[2] = wiy * tsh - 10000.0f;
        wfv[3] = 1.0f;

        txw[i].tx = ((wix * 512) / pnumw) * 16 + t;
        txw[i].ty = ((wiy * 256) / pnumh) * 16;

        sceVu0ApplyMatrix(wpos, wlm, wfv);

        l = Get2PLength(camera.p, wpos);

        txw[i].fg = far <= l ? 0 : (l <= near ? 0xff : ((far - l) * 255.0f) / (far - near));
        txw[i].alp = ((0x80 - (txw[i].fg / 2) & 0xffff) / 2) + 0x40;

        sceVu0RotTransPers(txw[i].iv, slm, wfv, 1);

        txw[i].cl = 1;

        if ((txw[i].iv[0] >= 0 && txw[i].iv[0] < 0x300) || clpx2 < txw[i].iv[0])
        {
            txw[i].cl = 0;
        }

        if ((txw[i].iv[1] >= 0 && txw[i].iv[1] < 0x300) || clpy2 < txw[i].iv[1])
        {
            txw[i].cl = 0;
        }

        if (txw[i].iv[2] == 0 || clpz2 < txw[i].iv[2])
        {
            txw[i].cl = 0;
        }
    }

    if (stop_effects == 0)
    {
        wave = wave + 1.0f < 360.0f ? wave + 1.0f : wave + 1.0f - 360.0f;
        texsc = texsc + 0.015f > 512.0f ? (texsc + 0.015f) - 512.0f : texsc + 0.01f;
    }

    Reserve2DPacket(0x1000);

    n = ndpkt;
    m = (pnumw + 1) * pnumh;

    pbuf[ndpkt++].ul128 = (u_long128)0;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(8, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    pbuf[ndpkt].ul64[0] = effdat[monochrome_mode + 54].tex0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_CLAMP_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_FOGCOL(0xa, 0xa, 0xa);
    pbuf[ndpkt++].ul64[1] = SCE_GS_FOGCOL;


    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 0, SCE_GIF_REGLIST, 2);
    pbuf[ndpkt++].ul64[1] = (u_long)0xf0;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 1, 1, 1, 1, 0, 1, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_PRIM;

    tagnum = ndpkt;
    ndpkt++;

    reg = 0;
    cnt = 0;

    for (i = 0; i < m; i++)
    {
        j = i + pnumw+1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_UV(txw[i].tx, txw[i].ty);
        pbuf[ndpkt++].ul64[1] = SCE_GS_SET_RGBAQ(0x80, 0x80, 0x80, txw[i].alp, 0x3f800000);

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_XYZF(txw[i].iv[0], txw[i].iv[1], txw[i].iv[2], txw[i].fg);

        k = i % (pnumw+1) ? txw[i-1].cl + txw[j-1].cl + txw[i].cl : 0;

        if (k >= 3)
        {
            reg |= (long)(0 \
                | SCE_GS_UV    << (4 * 0)
                | SCE_GS_RGBAQ << (4 * 1)
                | SCE_GS_XYZF2 << (4 * 2)
            ) << (cnt * 12);
        }
        else
        {
            reg |= (long)(0 \
                | SCE_GS_UV    << (4 * 0)
                | SCE_GS_RGBAQ << (4 * 1)
                | SCE_GS_XYZF3 << (4 * 2)
            ) << (cnt * 12);
        }
        pbuf[ndpkt++].ul64[1] = SCE_GS_SET_UV(txw[j].tx, txw[j].ty);

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_RGBAQ(0x80, 0x80, 0x80, txw[j].alp, 0x3f800000);
        pbuf[ndpkt++].ul64[1] = SCE_GS_SET_XYZF(txw[j].iv[0], txw[j].iv[1], txw[j].iv[2], txw[j].fg);

        cnt++;

        k = i % (pnumw+1) ? txw[j-1].cl + txw[i].cl + txw[j].cl : 0;

        if (k >= 3)
        {
            reg |= (long)(0 \
                | SCE_GS_UV    << (4 * 0)
                | SCE_GS_RGBAQ << (4 * 1)
                | SCE_GS_XYZF2 << (4 * 2)
            ) << (cnt * 12);
        }
        else
        {
            reg |= (long)(0 \
                | SCE_GS_UV    << (4 * 0)
                | SCE_GS_RGBAQ << (4 * 1)
                | SCE_GS_XYZF3 << (4 * 2)
            ) << (cnt * 12);
        }

        cnt++;

        if (cnt > 3)
        {
            pbuf[tagnum].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 0, SCE_GIF_REGLIST, cnt * 3);
            pbuf[tagnum].ul64[1] = reg;

            tagnum = ndpkt;
            ndpkt++;

            reg = 0;
            cnt = 0;
        }
    }

    if (cnt >= 1)
    {
        pbuf[tagnum].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 0, SCE_GIF_REGLIST, cnt * 3);
        pbuf[tagnum].ul64[1] = reg;
    }
    else
    {
        ndpkt--;
    }

    pbuf[n].ui32[0] = ndpkt + DMAend - n - 1;
}

void SetHaze_Pond_SW(int sw)
{
    init_haze_pond = sw;
}

void SetHaze_Pond()
{
    int i;
    int c;
    float far;
    float near;
    float dist;
    float wline;
    float ang;
    float maxalp;
    float lft;
    float rht;
    float fx;
    float fz;
    float l;
    float ll;
    sceVu0IVECTOR ivec;
    sceVu0FVECTOR cpos;
    sceVu0FVECTOR rot;
    sceVu0FVECTOR rw1;
    sceVu0FVECTOR rw2;
    HAZE_WORK hw[10];
    static HAZE_NUMS hn[10];
    long v2;

    if ((plyr_wrk.pr_info.room_no != 0x16 && plyr_wrk.pr_info.room_no != 0x10) || realtime_scene_flg != 0 || init_haze_pond == 0)
    {
        return;
    }

    maxalp = 128.0f;
    dist = 500.0f;
    near = 800.0f;
    far   = 5000.0f;

    if (init_pond != 0)
    {
        for (i = 0; i < 10; i++)
        {
            hn[i].scr = vu0Rand() * 128.0f;
            hn[i].mov = vu0Rand() * 0.15f - 0.05f;
            hn[i].lng = near + (float)(9 - i) * dist;
        }
    }

    if (hn[9].lng < near)
    {
        for (i = 9; i > 0; i--)
        {
            hn[i].lng = hn[i].lng + dist;
            hn[i].scr = hn[i-1].scr;
            hn[i].mov = hn[i-1].mov;
        }

        hn[0].lng = hn[0].lng + dist;
        hn[0].scr = vu0Rand() * 128.0f;
        hn[0].mov = vu0Rand() * 0.15f - 0.05f;
    }

    if (hn[9].lng >= (dist+near))
    {
        for (i = 0; i < 9; i++)
        {
            hn[i].lng = hn[i].lng - dist;
            hn[i].scr = hn[i+1].scr;
            hn[i].mov = hn[i+1].mov;
        }

        hn[9].lng = hn[9].lng - dist;
        hn[9].scr = vu0Rand() * 128.0f;
        hn[9].mov = vu0Rand() * 0.15f - 0.05f;
    }

    for (i = 0; i < 10; i++)
    {
        fx = camera.i[0] - camera.p[0];
        fz = camera.i[2] - camera.p[2];

        l = SgSqrtf(fx * fx + fz * fz);

        cpos[0] = camera.p[0] + ((camera.i[0] - camera.p[0]) * hn[i].lng) / l;
        cpos[1] = 330.0f;
        cpos[2] = camera.p[2] + ((camera.i[2] - camera.p[2]) * hn[i].lng) / l;
        cpos[3] = 1.0f;

        sceVu0RotTransPers(ivec, SgWSMtx, cpos, 0);

        if (init_pond != 0)
        {
            Vu0CopyVector(hn[i].oposp, cpos);
            Vu0CopyVector(hn[i].oposc, camera.p);
        }

        GetTrgtRot(camera.p, camera.i, rot, 2);

        if (CamChangeCheck() == 0)
        {
            GetTrgtRot(hn[i].oposc, hn[i].oposp, rw1, 2);
            GetTrgtRot(hn[i].oposc, camera.p, rw2, 2);

            ang = rw2[1] - rw1[1];

            while (ang < -PI) ang += PI2;
            while (ang >= PI) ang -= PI2;

            fx = camera.p[0] - hn[i].oposc[0];
            fz = camera.p[2] - hn[i].oposc[2];

            l = SgSqrtf(fx * fx + fz * fz);

            ll = l * SgCosf(ang);

            hn[i].lng = hn[i].lng - ll;
        }

        Vu0CopyVector(hn[i].oposp, cpos);
        Vu0CopyVector(hn[i].oposc, camera.p);

        fx = camera.i[0] - camera.p[0];
        fz = camera.i[2] - camera.p[2];

        l = SgSqrtf(fx * fx + fz * fz);

        cpos[0] = camera.p[0] + ((camera.i[0] - camera.p[0]) * hn[i].lng) / l;
        cpos[1] = 330.0f;
        cpos[2] = camera.p[2] + ((camera.i[2] - camera.p[2]) * hn[i].lng) / l;
        cpos[3] = 1.0f;

        sceVu0RotTransPers(ivec, SgWSMtx, cpos, 0);
        GetTrgtRot(camera.p, camera.i, rot, 2);

        wline = (rot[1] * 128.0f) / PI;
        wline += hn[i].scr;

        if (stop_effects == 0)
        {
            hn[i].scr = hn[i].scr + hn[i].mov;
        }

        v2 = 0x400; // long var ???

        while (wline < 0.0f) wline += 128.0f;
        while (wline >= 128.0f) wline -= 128.0f;

        hw[i].x1 = 0x6c00;
        hw[i].x2 = 0x9400;
        hw[i].y1 = ivec[1];
        hw[i].y2 = ivec[1] - 0x300;
        hw[i].zz = ivec[2];
        hw[i].u1 = wline * 16;
        hw[i].u2 = (wline + 128.0f) * 16;
        hw[i].v1 = 0;
        hw[i].v2 = v2;
    }

    for (i = 1; i < 10; i++)
    {
        hw[i].alp = ((hn[i].lng - near) * maxalp) / (dist * 9);
    }

    hw[0].alp = (((far + near) - hn[0].lng) * maxalp) / dist;

    GetTrgtRot(camera.p, camera.i, rot, 2);

    if (rot[1] < DEG2RAD(30) && -DEG2RAD(45) < rot[1])
    {
        lft = 1.0f;
        rht = 1.0f;
    }

    if (DEG2RAD(130) <= rot[1] || rot[1] <= DEG2RAD(-135))
    {
        lft = 0.0f;
        rht = 0.0f;
    }

    if (DEG2RAD(30) <= rot[1] && rot[1] < DEG2RAD(80))
    {
        lft = 1.0f;
        rht = (DEG2RAD(80) - rot[1]) / DEG2RAD(50);
    }

    if (DEG2RAD(80) <= rot[1] && rot[1] < DEG2RAD(130))
    {
        lft = (DEG2RAD(130) - rot[1]) / DEG2RAD(50);
        rht = 0.0f;
    }

    if (rot[1] <= DEG2RAD(-45) && DEG2RAD(-90) < rot[1])
    {
        lft = (DEG2RAD(-90) - rot[1]) / DEG2RAD(-45);
        rht = 1.0f;
    }

    if (rot[1] <= DEG2RAD(-90) && DEG2RAD(-135) < rot[1])
    {
        lft = 0.0f;
        rht = (DEG2RAD(-135) - rot[1]) / DEG2RAD(-45);
    }

    if (35000.0f < camera.p[2])
    {
        l = (40000.0f - camera.p[2]) / 5000.0f;
    }
    else
    {
        l = 1.0f;
    }

    lft = 1.0f - (1.0f - lft) * l;
    rht = 1.0f - (1.0f - rht) * l;

    Reserve2DPacket(0x1000);

    c = ndpkt;

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = DMAend;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(7, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    pbuf[ndpkt].ul64[0] = effdat[monochrome_mode + 10].tex0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_CLAMP_1;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(40, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 1, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 3);
    pbuf[ndpkt++].ul64[1] = 0 \
                | SCE_GS_RGBAQ << (4 * 0)
                | SCE_GS_UV    << (4 * 1)
                | SCE_GS_XYZF2 << (4 * 2);

    for (i = 0; i < 10; i++)
    {
        pbuf[ndpkt].ui32[0] = 0x44;
        pbuf[ndpkt].ui32[1] = 0x44;
        pbuf[ndpkt].ui32[2] = 0x44;
        pbuf[ndpkt++].ui32[3] = hw[i].alp * lft;

        pbuf[ndpkt].ui32[0] = hw[i].u1;
        pbuf[ndpkt].ui32[1] = hw[i].v1;
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt++].ui32[3] = 0;

        pbuf[ndpkt].ui32[0] = hw[i].x1;
        pbuf[ndpkt].ui32[1] = hw[i].y1;
        pbuf[ndpkt].ui32[2] = hw[i].zz;
        pbuf[ndpkt++].ui32[3] = 0x8000;

        pbuf[ndpkt].ui32[0] = 0x44;
        pbuf[ndpkt].ui32[1] = 0x44;
        pbuf[ndpkt].ui32[2] = 0x44;
        pbuf[ndpkt++].ui32[3] = hw[i].alp * lft;

        pbuf[ndpkt].ui32[0] = hw[i].u1;
        pbuf[ndpkt].ui32[1] = hw[i].v2;
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt++].ui32[3] = 0;

        pbuf[ndpkt].ui32[0] = hw[i].x1;
        pbuf[ndpkt].ui32[1] = hw[i].y2;
        pbuf[ndpkt].ui32[2] = hw[i].zz;
        pbuf[ndpkt++].ui32[3] = 0x8000;

        pbuf[ndpkt].ui32[0] = 0x44;
        pbuf[ndpkt].ui32[1] = 0x44;
        pbuf[ndpkt].ui32[2] = 0x44;
        pbuf[ndpkt++].ui32[3] = hw[i].alp * rht;

        pbuf[ndpkt].ui32[0] = hw[i].u2;
        pbuf[ndpkt].ui32[1] = hw[i].v1;
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt++].ui32[3] = 0;

        pbuf[ndpkt].ui32[0] = hw[i].x2;
        pbuf[ndpkt].ui32[1] = hw[i].y1;
        pbuf[ndpkt].ui32[2] = hw[i].zz;
        pbuf[ndpkt++].ui32[3] = 0;

        pbuf[ndpkt].ui32[0] = 0x44;
        pbuf[ndpkt].ui32[1] = 0x44;
        pbuf[ndpkt].ui32[2] = 0x44;
        pbuf[ndpkt++].ui32[3] = hw[i].alp * rht;

        pbuf[ndpkt].ui32[0] = hw[i].u2;
        pbuf[ndpkt].ui32[1] = hw[i].v2;
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt++].ui32[3] = 0;

        pbuf[ndpkt].ui32[0] = hw[i].x2;
        pbuf[ndpkt].ui32[1] = hw[i].y2;
        pbuf[ndpkt].ui32[2] = hw[i].zz;
        pbuf[ndpkt++].ui32[3] = 0;
    }

    pbuf[c].ui32[0] = ndpkt + DMAend - c - 1;

    if (init_pond != 0)
    {
        init_pond = 0;
    }
}

void DrawNewPerticleSub(int num, sceVu0FVECTOR *pos, u_char r1, u_char g1, u_char b1, u_char r2, u_char g2, u_char b2, u_char a)
{
    u_int clpx2;
    u_int clpy2;
    u_int clpz2;
    u_char rr1;
    u_char gg1;
    u_char bb1;
    u_char rr2;
    u_char gg2;
    u_char bb2;
    int i;
    int j;
    int k;
    int n;
    int alp;
    int cl;
    int tagnum;
    int cnt;
    int clip[192];
    sceVu0IVECTOR ivec[192];
    u_long xyzf[192];
    u_long rgbaq1;
    u_long rgbaq2;
    u_long reg;

    clpx2 = 0xff80;
    clpy2 = 0xff80;
    clpz2 = 0xffffff;

    if (monochrome_mode != 0)
    {
        gg1 = (r1 + g1 + b1) / 3;
        bb1 = gg1;
        rr1 = gg1;

        gg2 = (r2 + g2 + b2) / 3;
        bb2 = gg2;
        rr2 = gg2;
    }
    else
    {
        rr1 = r1;
        gg1 = g1;
        bb1 = b1;

        rr2 = r2;
        gg2 = g2;
        bb2 = b2;
    }

    rgbaq2 = SCE_GS_SET_RGBAQ(rr2, gg2, bb2, 0, 0);

    sceVu0RotTransPersN(ivec, SgWSMtx, pos, num * 3, 1);

    for (i = 0; i < num * 3; i++)
    {
        clip[i] = 1;

        if ((ivec[i][0] >= 0 && ivec[i][0] < 0x80) || ivec[i][0] > clpx2)
        {
            clip[i] = 0;
        }

        if ((ivec[i][1] >= 0 && ivec[i][1] < 0x80) || ivec[i][1] > clpy2)
        {
            clip[i] = 0;
        }

        if (ivec[i][2] == 0 || ivec[i][2] > clpz2)
        {
            clip[i] = 0;
        }

        xyzf[i] = SCE_GS_SET_XYZF(ivec[i][0], ivec[i][1], ivec[i][2], 0);
    }

    Reserve2DPacket(0x1000);

    n = ndpkt;

    pbuf[ndpkt++].ul128 = (u_long128)0;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(3, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 0, SCE_GIF_REGLIST, 2);
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_PRIM << (4 * 0)
        | 0x0f        << (4 * 1);

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 1, 0, 0, 1, 0, 1, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_PRIM;

    tagnum = ndpkt;

    reg = 0;
    cnt = 0;

    ndpkt++;

    for (j = 0; j < 2; j++)
    {
        for (i = 0; i < num; i++)
        {
            alp = (((num - i) - 1) * a) / num;

            if (plyr_wrk.mode != 1)
            {
                alp = (i * alp) / num;
            }

            rgbaq1 = SCE_GS_SET_RGBAQ(rr1, gg1, bb1, alp, 0);

            k = i * 3 + j;

            pbuf[ndpkt].ul64[0] = j != 0 ? rgbaq1 : rgbaq2;
            pbuf[ndpkt++].ul64[1] = xyzf[k];

            cl = i != 0 ? clip[k-3] + clip[k-2] + clip[k] : 0;

            if (cl >= 3)
            {
                reg |= (long)(0 \
                    | SCE_GS_RGBAQ << (4 * 0)
                    | SCE_GS_XYZF2 << (4 * 1)
                ) << (cnt * 8);
            }
            else
            {
                reg |= (long)(0 \
                    | SCE_GS_RGBAQ << (4 * 0)
                    | SCE_GS_XYZF3 << (4 * 1)
                ) << (cnt * 8);
            }

            pbuf[ndpkt].ul64[0] = j != 0 ? rgbaq2 : rgbaq1;
            pbuf[ndpkt++].ul64[1] = xyzf[k + 1];

            cnt++;

            cl = i != 0 ? clip[k-2] + clip[k] + clip[k+1] : 0;

            if (cl >= 3)
            {
                reg |= (long)(0 \
                    | SCE_GS_RGBAQ << (4 * 0)
                    | SCE_GS_XYZF2 << (4 * 1)
                ) << (cnt * 8);
            }
            else
            {
                reg |= (long)(0 \
                    | SCE_GS_RGBAQ << (4 * 0)
                    | SCE_GS_XYZF3 << (4 * 1)
                ) << (cnt * 8);
            }

            cnt++;

            if (cnt > 7)
            {
                pbuf[tagnum].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 0, SCE_GIF_REGLIST, cnt * 2);
                pbuf[tagnum].ul64[1] = reg;

                reg = 0;
                cnt = 0;

                tagnum = ndpkt;

                ndpkt++;
            }
        }
    }

    if (cnt >= 1)
    {
        pbuf[tagnum].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 0, SCE_GIF_REGLIST, cnt * 2);
        pbuf[tagnum].ul64[1] = reg;
    }
    else
    {
        ndpkt--;
    }

    pbuf[n].ui32[0] = ndpkt + DMAend - n - 1;
}

void SetEneFace(EFFECT_CONT *ec)
{
    int alp;
    int direc;
    int i;
    int j;
    int th;
    int tw;
    int bak;
    int num;
    u_int clip[16][4];
    u_int clpx2;
    u_int clpy2;
    u_int clpz2;
    float scl;
    float dirfl;
    float rot_x;
    float rot_y;
    float stq[2] = {0.01f, 0.99f};
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0IVECTOR ivec[16][4];
    sceVu0FVECTOR wpos;
    sceVu0FVECTOR ppos[4] = {
        {-256.0f, -217.0f, 0.0f, 1.0f},
        {+256.0f, -217.0f, 0.0f, 1.0f},
        {-256.0f, +217.0f, 0.0f, 1.0f},
        {+256.0f, +217.0f, 0.0f, 1.0f},
    };
    U32DATA ts[16][4];
    U32DATA tt[16][4];
    U32DATA tq[16][4];

#ifdef MATCHING_DECOMP
    if (rot_x)
    {
        // fixes stack reordering issue
    }
#endif

    clpx2 = 0xfd00;
    clpy2 = 0xfd00;
    clpz2 = 0x00ffffff;

    SetSprFile(ADDRESS);

    wpos[0] = ec->dat.fl32[1];
    wpos[1] = ec->dat.fl32[2];
    wpos[2] = ec->dat.fl32[3];

    num = ec->dat.uc8[2];

    direc = ec->dat.uc8[3];
    ec->cnt += 2;

    if (ec->cnt >= 180.0f)
    {
        alp = 0;
        ResetEffects(ec);
    }
    else if (ec->cnt < 90.0f)
    {
        alp = (ec->cnt * 32.0f) / 90.0f;
    }
    else
    {
        alp = ((180.0f - ec->cnt) * 32.0f) / 90.0f;
    }

    dirfl = scl = 1.0f;

    if (direc != 0)
    {
        dirfl = -1.0f;
    }

    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

    sceVu0UnitMatrix(wlm);
    sceVu0RotMatrixX(wlm, wlm, rot_x);
    sceVu0RotMatrixY(wlm, wlm, rot_y);
    sceVu0TransMatrix(wlm, wlm, wpos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);

    tw = eneface[num + monochrome_mode].w;
    th = eneface[num + monochrome_mode].h;

    for (j = 0; j < 16; j++)
    {
        ppos[0][0] = ppos[2][0] = -((j * 0.2f / 16.0f + scl) * 192.0f) * dirfl;
        ppos[1][0] = ppos[3][0] = +((j * 0.2f / 16.0f + scl) * 192.0f) * dirfl;
        ppos[0][1] = ppos[1][1] = +((j * 0.2f / 16.0f + scl) * 175.0f);
        ppos[2][1] = ppos[3][1] = -((j * 0.2f / 16.0f + scl) * 175.0f);

        sceVu0RotTransPersN(ivec[j], slm, ppos, 4, 1);


        for (i = 0; i < 4; i++)
        {
            clip[j][i] = 0;

            if ((ivec[j][i][0] >= 0 && ivec[j][i][0] < 0x300) || ivec[j][i][0] > clpx2)
            {
                clip[j][i] = 1;
            }

            if ((ivec[j][i][1] >= 0 && ivec[j][i][1] < 0x300) || ivec[j][i][1] > clpy2)
            {
                clip[j][i] = 1;
            }

            if (ivec[j][i][2] == 0 || ivec[j][i][2] > clpz2)
            {
                clip[j][i] = 1;
            }

            tq[j][i].fl32 = 1.0f / ivec[j][i][3];
            ts[j][i].fl32 = stq[i % 2] * tw * tq[j][i].fl32 / 256.0f;
            tt[j][i].fl32 = stq[i / 2] * th * tq[j][i].fl32 / 256.0f;
        }
    }

    Reserve2DPacket(0x1000);

    bak = ndpkt;

    pbuf[ndpkt++].ul128 = (u_long128)0;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    pbuf[ndpkt].ul64[0] = eneface[num + monochrome_mode].tex0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    for (j = 15; j >= 0; j--)
    {
        if (clip[j][0] + clip[j][1] + clip[j][2] + clip[j][3] == 0)
        {
            pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 84, SCE_GIF_PACKED, 3);
            pbuf[ndpkt++].ul64[1] = 0 \
                    | SCE_GS_ST    << (4 * 0)
                    | SCE_GS_RGBAQ << (4 * 1)
                    | SCE_GS_XYZF2 << (4 * 2);

            for (i = 0; i < 4; i++)
            {
                pbuf[ndpkt].ui32[0] = ts[j][i].ui32;
                pbuf[ndpkt].ui32[1] = tt[j][i].ui32;
                pbuf[ndpkt].ui32[2] = tq[j][i].ui32;
                pbuf[ndpkt++].ui32[3] = 0;

                pbuf[ndpkt].ui32[0] = 0x80;
                pbuf[ndpkt].ui32[1] = 0x80;
                pbuf[ndpkt].ui32[2] = 0x80;
                pbuf[ndpkt++].ui32[3] = alp - (j * alp / 16);

                pbuf[ndpkt].ui32[0] = ivec[j][i][0];
                pbuf[ndpkt].ui32[1] = ivec[j][i][1];
                pbuf[ndpkt].ui32[2] = ivec[j][i][2] * 16;
                pbuf[ndpkt++].ui32[3] = i > 1 ? 0 : 0x8000;
            }
        }
    }

    pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;
}

void SetSpirit(EFFECT_CONT *ec)
{
    static sceVu0FVECTOR bpos;
    static sceVu0FVECTOR opos[24];
    static int tbl[24];
    u_char mr;
    u_char mg;
    u_char mb;
    u_char mrh;
    u_char mgh;
    u_char mbh;
    u_char mf;
    int i;
    int n;
    int num;
    int top;
    float sc;
    float arate;
    float msc;
    float msch;

    mf = ec->flow;

    mr = ec->dat.uc8[2];
    mg = ec->dat.uc8[3];
    mb = ec->dat.uc8[4];

    msc = ec->dat.fl32[2];

    mrh = ec->dat.uc8[5];
    mgh = ec->dat.uc8[6];
    mbh = ec->dat.uc8[7];

    msch = ec->dat.fl32[3];

    Vu0CopyVector(bpos, ec->pnt[0]);

    arate = *(float *)ec->pnt[1];

    if (ec->keep == 0)
    {
        for (i = 0; i < 24; i++)
        {
            tbl[i] = i;
        }

        ec->cnt = 0;
        ec->keep = 1;
        ec->max = 0;
    }

    num = ec->cnt;
    top = ec->max;

    if (stop_effects == 0)
    {
        n = top;

        for (i = 0; i < 24; i++)
        {
            tbl[i] = n;

            n = n - 1 < 0 ? 23 : n - 1;
        }

        if (opos[tbl[1]][0] == bpos[0] && opos[tbl[1]][1] == bpos[1] && opos[tbl[1]][2] == bpos[2])
        {
            num = num - 1 < 0 ? 0 : num - 1;
        }
        else
        {
            num = num + 1 > 24 ? 24 : num + 1;
        }

        Vu0CopyVector(opos[top], bpos);

        top = (top + 1) % 24;
    }

    for (i = num - 1; i >= 0; i--)
    {
        sc = (float)(num - i) / (num + 1);

        SetEffects(24, 1, mf, &opos[tbl[i]], mr, mg, mb, msc * sc, mrh, mgh, mbh, msch * sc, sc * arate);
    }

    SetEffects(23, 1, mf,bpos, mr, mg, mb, msc, mrh, mgh, mbh, msch * arate);

    ec->cnt = num;
    ec->max = top;

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SetFaceSpirit(EFFECT_CONT *ec)
{
    float mr;
    float mg;
    float mb;
    int i;
    int j;
    int n;
    int tw;
    int th;
    int bak;
    int num;
    int top;
    int clip[24];
    int bclip;
    u_int clpx2 = 0xc000;
    u_int clpy2 = 0xc000;
    u_int clpz1 = 1; // ???
    u_int clpz2;
    float sc;
    float arate;
    float fx;
    float fy;
    float fz;
    float l;
    float rot_x;
    float rot_y;
    float tailsc[24];
    u_long tx0;
    u_long cnt;
    FACESPIRIT_ST *fs;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0IVECTOR ivclip;
    sceVu0IVECTOR ivec[24][4];
    sceVu0IVECTOR ivecb[4];
    sceVu0FVECTOR bpos;
    sceVu0FVECTOR wpos;
    sceVu0FVECTOR zero = {0.0f, 0.0f, 0.0f, 1.0f};
    sceVu0FVECTOR ppos[4] = {
        {-180.0f, -180.0f, 0.0f, 1.0f},
        {+180.0f, -180.0f, 0.0f, 1.0f},
        {-180.0f, +180.0f, 0.0f, 1.0f},
        {+180.0f, +180.0f, 0.0f, 1.0f},
    };

#ifdef MATCHING_DECOMP
    if (rot_x)
    {
        // fixes stack order
    }
#endif

    // clpz1 = 1; // ???
    clpz2 = 0xffffff;

    cnt = sys_wrk.count;

    mr = ec->dat.uc8[2];
    mg = ec->dat.uc8[3];
    mb = ec->dat.uc8[4];

    Vu0CopyVector(bpos, ec->pnt[0]);

    arate = *(float *)ec->pnt[clpz1]; // clpz1 ???
    if (ec->keep == 0)
    {
        ec->cnt = 0;
        ec->max = 0;
        ec->keep = 1;
        fs = &facespirit[ec->dat.uc8[5]];
        ec->dat.uc8[6] = fly_wrk[ec->dat.uc8[5]].ene;
        for (i = 0; i < 24; i++)
        {
            fs->tbl[i] = i;
        }
    }
    else
    {
        fs = &facespirit[ec->dat.uc8[5]];
    }

    num = ec->cnt;
    top = ec->max;

    arate *= ene_wrk[ec->dat.uc8[6]].tr_rate / 60.0f;

    if (stop_effects == 0 && (cnt % 3) == 0)
    {
        n = top;
        for (i = 0; i < 24; i++)
        {
            fs->tbl[i] = n;
            n = n - 1 < 0 ? 23 : n - 1;
        }

        if (num >= 3)
        {
            fx = bpos[0] - fs->opos[fs->tbl[1]][0];
            fy = 0.0f;
            fz = bpos[2] - fs->opos[fs->tbl[1]][2];

            l = SgSqrtf(fx * fx + fy * fy + fz * fz);
        }
        else
        {
            l = 3.0f;
        }

        if (l > 100.0f)
        {
            ec->cnt = 0;
            ec->max = 0;
            ec->keep = 1;

            for (i = 0; i < 24; i++)
            {
                fs->tbl[i] = i;
            }

            if (ec->dat.uc8[1] & 1)
            {
                ResetEffects(ec);
            }
            return;
        }

        if (l < 3.0f)
        {
            Vu0CopyVector(fs->opos[top], bpos);
        }
        else
        {
            num = num + 1 > 24 ? 24 : num + 1;
            Vu0CopyVector(fs->opos[top], bpos);
            top = (top + 1) % 24;
        }

        for (j = num-1; j > 0; j--)
        {
            fs->opos[fs->tbl[j]][1] = fs->opos[fs->tbl[j-1]][1];
        }
    }

    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

    for (i = num-1; i >= 0; i--)
    {
        n = fs->tbl[i];

        fx = fs->opos[n][0] - camera.p[0];
        fy = fs->opos[n][1] - camera.p[1];
        fz = fs->opos[n][2] - camera.p[2];

        l = SgSqrtf(fx * fx + fy * fy + fz * fz);

        wpos[0] = camera.p[0] + (fs->opos[n][0] - camera.p[0]) * (l - 150.0f) / l;
        wpos[1] = camera.p[1] + (fs->opos[n][1] - camera.p[1]) * (l - 150.0f) / l;
        wpos[2] = camera.p[2] + (fs->opos[n][2] - camera.p[2]) * (l - 150.0f) / l;
        wpos[3] = 1.0f;

        tailsc[n] = (num - i) / (float)(num + 1);

        sceVu0UnitMatrix(wlm);

        wlm[0][0] = wlm[1][1] = wlm[2][2] = tailsc[n];

        sceVu0RotMatrixX(wlm, wlm, rot_x);
        sceVu0RotMatrixY(wlm, wlm, rot_y);
        sceVu0TransMatrix(wlm, wlm, wpos);
        sceVu0MulMatrix(slm, SgWSMtx, wlm);
        sceVu0RotTransPersN(ivec[n], slm, ppos, 4, 0);

        clip[n] = 0;

        sceVu0RotTransPers(ivclip, slm, zero, 0);

        if ((ivclip[0] >= 0 && ivclip[0] < 0x4000) || ivclip[0] > clpx2)
        {
            clip[n] = 1;
        }

        if ((ivclip[1] >= 0 && ivclip[1] < 0x4000) || ivclip[1] > clpy2)
        {
            clip[n] = 1;
        }

        if (ivclip[2] == 0 || ivclip[2] > clpz2)
        {
            clip[n] = 1;
        }
    }

    fx = bpos[0] - camera.p[0];
    fy = bpos[1] - camera.p[1];
    fz = bpos[2] - camera.p[2];

    l = SgSqrtf(fx * fx + fy * fy + fz * fz);

    wpos[0] = camera.p[0] + ((bpos[0] - camera.p[0]) * (l - 150.0f)) / l;
    wpos[1] = camera.p[1] + ((bpos[1] - camera.p[1]) * (l - 150.0f)) / l;
    wpos[2] = camera.p[2] + ((bpos[2] - camera.p[2]) * (l - 150.0f)) / l;
    wpos[3] = 1.0f;

    sceVu0UnitMatrix(wlm);
    sceVu0RotMatrixX(wlm, wlm, rot_x);
    sceVu0RotMatrixY(wlm, wlm, rot_y);
    sceVu0TransMatrix(wlm, wlm, wpos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);
    sceVu0RotTransPersN(ivecb, slm, ppos, 4, 0);
    sceVu0RotTransPers(ivclip, slm, zero, 0);

    bclip = 0;

    if ((ivclip[0] >= 0 && ivclip[0] < 0x4000) || ivclip[0] > clpx2)
    {
        bclip = 1;
    }

    if ((ivclip[1] >= 0 && ivclip[1] < 0x4000) || ivclip[1] > clpy2)
    {
        bclip = 1;
    }

    if (ivclip[2] == 0 || ivclip[2] > clpz2)
    {
        bclip = 1;
    }

    th = effdat[24].h * 16;
    tw = effdat[24].w * 16;
    tx0 = effdat[24].tex0;

    Reserve2DPacket(0x1000);

    bak = ndpkt;

    pbuf[ndpkt++].ul128 = (u_long128)0;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    pbuf[ndpkt].ul64[0] = tx0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    for (j = num-1; j >= 0; j--)
    {
        l = (float)j / num;
        if (clip[fs->tbl[j]] == 0)
        {
            pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 340, SCE_GIF_PACKED, 3);
            pbuf[ndpkt++].ul64[1] = 0 \
                | SCE_GS_UV    << (4 * 0)
                | SCE_GS_RGBAQ << (4 * 1)
                | SCE_GS_XYZF2 << (4 * 2);

            sc = (float)(num - j) / (num + 1);

            for (i = 0; i < 4; i++)
            {
                pbuf[ndpkt].ui32[0] = (i & 1) != 0 ? tw - 8 : 8;
                pbuf[ndpkt].ui32[1] = (i / 2) != 0 ? th - 8 : 8;
                pbuf[ndpkt].ui32[2] = 0;
                pbuf[ndpkt++].ui32[3] = 0;

                pbuf[ndpkt].ui32[0] = (u_char)(mr * l);
                pbuf[ndpkt].ui32[1] = (u_char)(mg * l);
                pbuf[ndpkt].ui32[2] = (u_char)(mb * l);
                pbuf[ndpkt++].ui32[3] = sc * 32 * arate;

                pbuf[ndpkt].ui32[0] = ivec[fs->tbl[j]][i][0];
                pbuf[ndpkt].ui32[1] = ivec[fs->tbl[j]][i][1];
                pbuf[ndpkt].ui32[2] = ivec[fs->tbl[j]][i][2];
                pbuf[ndpkt++].ui32[3] = i > 1 ? 0 : 0x8000;
            }
        }
    }

    if (bclip == 0)
    {
        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 340, SCE_GIF_PACKED, 3);
        pbuf[ndpkt++].ul64[1] = 0 \
                | SCE_GS_UV    << (4 * 0)
                | SCE_GS_RGBAQ << (4 * 1)
                | SCE_GS_XYZF2 << (4 * 2);

        for (i = 0; i < 4; i++)
        {
            pbuf[ndpkt].ui32[0] = (i & 1) != 0 ? tw - 8 : 8;
            pbuf[ndpkt].ui32[1] = (i / 2) != 0 ? th - 8 : 8;
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = mr;
            pbuf[ndpkt].ui32[1] = mg;
            pbuf[ndpkt].ui32[2] = mb;
            pbuf[ndpkt++].ui32[3] = arate * 48;

            pbuf[ndpkt].ui32[0] = ivecb[i][0];
            pbuf[ndpkt].ui32[1] = ivecb[i][1];
            pbuf[ndpkt].ui32[2] = ivecb[i][2];
            pbuf[ndpkt++].ui32[3] = i > 1 ? 0 : 0x8000;
        }
    }

    pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;

    ec->cnt = num;
    ec->max = top;

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SetSaveCameraLamp()
{
    sceVu0FVECTOR bbpos[7] = {
        {23100.0f, 0.0f, 10325.0f, 1.0f},
        {17875.0f, 0.0f, 23125.0f, 1.0f},
        {35900.0f, 400.0f, 28050.0f, 1.0f},
        {27968.0f, -1400.0f, 16345.0f, 1.0f},
        {35850.0f, 400.0f, 17850.0f, 1.0f},
        {33050.0f, -1800.0, 20750.0f, 1.0f},
        {20775.0f, 3000.0f, 25975.0f, 1.0f},
    };
    int n;
    int col;
    u_char rgb1[2][4] = {
        {0xff, 0x00, 0x00, 0x08},
        {0x10, 0x10, 0xff, 0x08},
    };
    u_char rgb2[2][4] = {
        {0xff, 0x10, 0x10, 0x40},
        {0x40, 0x40, 0xff, 0x40},
    };
    float rot_x;
    float rot_y;
    float direc[7] = {
        PI, -PI/2, PI, -(3*PI)/4, 0.0f, -PI/2, PI,
    };
    sceVu0FVECTOR comp = {0.0f, -574.0f, 38.0f, 1.0f};
    sceVu0FVECTOR wpos;
    sceVu0FMATRIX wlm;
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 84, SCE_GIF_PACKED, 3),
    };
    float pos1 = 128.0f;
    float pos2 = 20.0f;

    if (plyr_wrk.pr_info.room_no == 0)
    {
        n = 0;
    }
    else if (plyr_wrk.pr_info.room_no == 15)
    {
        n = 1;
    }
    else if (plyr_wrk.pr_info.room_no == 21)
    {
        n = 2;
    }
    else if (plyr_wrk.pr_info.room_no == 41)
    {
        n = 3;
    }
    else if (plyr_wrk.pr_info.room_no == 6)
    {
        n = 4;
    }
    else if (plyr_wrk.pr_info.room_no == 14)
    {
        n = 5;
    }
    else if (plyr_wrk.pr_info.room_no == 31)
    {
        n = 6;
    }
    else
    {
        return;
    }

    col = SavePointLightJudge();

    sceVu0UnitMatrix(wlm);
    sceVu0RotMatrixY(wlm, wlm, direc[n]);
    sceVu0TransMatrix(wlm, wlm, bbpos[n]);
    sceVu0ApplyMatrix(wpos, wlm, comp);
    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);
    sceVu0UnitMatrix(wlm);
    sceVu0RotMatrixX(wlm, wlm, rot_x);
    sceVu0RotMatrixY(wlm, wlm, rot_y);
    sceVu0TransMatrix(wlm, wlm, wpos);

    Set3DPosTexure(wlm, &de, 22, pos1, pos1, rgb1[col][0], rgb1[col][1], rgb1[col][2], rgb1[col][3]);
    Set3DPosTexure(wlm, &de, 14, pos2, pos2, rgb2[col][0], rgb2[col][1], rgb2[col][2], rgb2[col][3]);
}
