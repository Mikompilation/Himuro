#include "common.h"
#include "typedefs.h"
#include "effect_obj.h"

// gcc/src/newlib/libm/math/sf_tan.c
float tanf(float x);

#include "ee/kernel.h"
#include "ee/eestruct.h"
#include "sce/libvu0.h"
#include "sce/libgraph.h"

#include "main/glob.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/tim2_new.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/effect_oth.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/libsg.h"
#include "graphics/graph3d/sgdma.h"
#include "graphics/graph3d/sgcam.h"

static inline void Vu0Func0000(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
    __asm__ __volatile__("                     \n\
        lqc2         $vf12,0(%1)               \n\
        vmulz.xy     $vf12xy,$vf12xy,$vf8z     \n\
        vmulaw.xyzw  ACCxyzw,$vf7xyzw,$vf0w    \n\
        vmaddaz.xyzw ACCxyzw,$vf6xyzw,$vf12z   \n\
        vmadday.xyzw ACCxyzw,$vf5xyzw,$vf12y   \n\
        vmaddx.xyzw  $vf13xyzw,$vf4xyzw,$vf12x \n\
        vdiv         Q,$vf0w,$vf13w            \n\
        vwaitq                                 \n\
        vmulq.xyz    $vf13xyz,$vf13xyz,Q       \n\
        vsub.xy      $vf13xy,$vf13xy,$vf8xy    \n\
        vmaxx.x      $vf13x,$vf13x,$vf9x       \n\
        vminiy.x     $vf13x,$vf13x,$vf9y       \n\
        vmaxz.y      $vf13y,$vf13y,$vf9z       \n\
        vminiw.y     $vf13y,$vf13y,$vf9w       \n\
        vmulq.xy     $vf14xy,$vf10xy,Q         \n\
        vmul.xy      $vf13xy,$vf13xy,$vf14xy   \n\
        vmulq.z      $vf13z,$vf10z,Q           \n\
        vmulw.xyz    $vf13xyz,$vf13xyz,$vf10w  \n\
        sqc2         $vf13,0(%0)               \n\
        ": : "r" (v0), "r" (v1)
    );
}

static inline void dummy1(sceVu0FMATRIX m0)
{
    asm volatile ("\n\
        lqc2 $vf4,0(%0) \n\
        lqc2 $vf5,0x10(%0) \n\
        lqc2 $vf6,0x20(%0) \n\
        lqc2 $vf7,0x30(%0) \n\
    ": :"r"(m0));
}

static inline void dummy2(sceVu0FVECTOR *v0)
{
    asm volatile ("\n\
        lqc2 $vf8,0(%0) \n\
        lqc2 $vf9,0x10(%0) \n\
        lqc2 $vf10,0x20(%0) \n\
    ": :"r"(v0));
}

typedef struct {
    u_int type;
    u_char rgba[4];
    float lscl;
    float tscl;
} EFRENZ;

typedef struct {
    int on[4];
    int flg[4];
    u_int flow;
    u_int cnt;
    u_int in;
    u_int keep;
    u_int out;
    u_int max;
    float *vol;
} EFF_PARTSBLUR;

#define PI 3.1415927f
#define PI2 6.2831855f

#define DISP_WIDTH 640
#define DISP_HEIGHT 448

static int p_dbg_flg = 0;
static float pdyoff0 = 1.0f;
static float pdyoff1 = 0.0f;
static float pdxoff = 0.0f;

static int init_pdef2;

static EFF_PARTSBLUR eff_partsblur;

void InitEffectObj()
{
    int i;

    for (i = 0; i < 8; i++)
    {
        efi[i].use = 0;
        efi[i].pass = 0;
    }

    for (i = 0; i < 4; i++)
    {
        eff_partsblur.on[i] = 0;
        eff_partsblur.flg[i] = 0;
    }

    eff_partsblur.flow = 3;
    eff_partsblur.cnt = 0;
    eff_partsblur.in = 0;
    eff_partsblur.keep = 0;
    eff_partsblur.out = 0;
    eff_partsblur.max = 0;
    eff_partsblur.vol = NULL;

    init_pdef2 = 1;
}

void InitEffectObjEF()
{
    int i;

    for (i = 0; i < 8; i++)
    {
        if (efi[i].use == 1 && efi[i].pass == 0)
        {
            efi[i].use = 0;
        }

        efi[i].pass = 0;
    }
}

void SetPartsBlurSW(int sw)
{
    return;
}

void SetPartsBlur1Sub(u_char alp)
{
    return;
}

void SetPartsBlur1(u_char alp)
{
    return;
}

void SetPartsBlur2()
{
    return;
}

void RunPartsBlur()
{
    return;
}

void CallPartsBlur2(int no, int in, int keep, int out, u_char alp)
{
    return;
}

void CallPartsBlur4(int no, float *vol)
{
    return;
}

void ResetPartsBlur()
{
    return;
}

void* CallPartsDeform2(int type, float scale, void *pos, u_int in, u_int keep, u_int out)
{
    return SetEffects(0x1b, 4, type, 100, scale, scale, pos, in, keep, out, 0, 0, 0, 0);
}

void* CallPartsDeform3(int type, float scale, void *pos, u_int in, u_int keep, u_int out, int alp)
{
    return SetEffects(0x1b, 4, type, alp, scale, scale, pos, in, keep, out, 0, 0, 0, 0);
}

void* CallPartsDeform3_2(int type, float sclx, float scly, void *pos, u_int in, u_int keep, u_int out, int alp)
{
    return SetEffects(0x1b, 4, type, alp, sclx, scly, pos, in, keep, out, 0, 0, 0, 0);
}

void* CallPartsDeform4(int type, float scale, void *pos, float *vol)
{
    return SetEffects(0x1b, 2, type, 0x80, scale, scale, pos, 0, 0, 0, vol, 0, 0, 0);
}

void* CallPartsDeform5(int type, float sclx, float scly, void *pos, float *vol)
{
    return SetEffects(0x1b, 2, type, 0x80, sclx, scly, pos, 0, 0, 0, vol, 0, 0, 0);
}

void SetPartsDeform(EFFECT_CONT *ec)
{
    int ef;
    int n0;
    int sbj;
    int page;
    float sp;
    float rt;
    float vol;
    float tr;

    n0 = (u_int)ec->dat.uc8[2];

    sbj = n0 % 10;
    page = n0 / 10;

    if (n0 == 24)
    {
        // ??
    }
    else if (n0 == 25)
    {
        // ??
    }
    else
    {
        if (ec->dat.uc8[1] & 4)
        {
            switch (ec->flow)
            {
            case 0:
                if (stop_effects == 0)
                {
                    ec->cnt = ec->cnt + 1;
                }

                if (ec->in)
                {
                    ef = (int)((ec->cnt * ec->max) / (float)ec->in);
                }

                if (ec->in <= ec->cnt)
                {
                    ec->flow = ec->keep == 0 ? (ec->out != 0 ? 2 : 3) : 1;
                    ec->cnt = 0;
                }
            break;
            case 1:
                if (stop_effects == 0)
                {
                    ec->cnt++;
                }
                ef = ec->max;

                if (ec->cnt >= ec->keep)
                {
                    ec->cnt = 0;
                    ec->flow = ec->out != 0 ? 2 : 3;
                }
            break;
            case 2:
                if (stop_effects == 0)
                {
                    ec->cnt++;
                }

                if (ec->out != 0)
                {
                    ef = ec->max - (int)((ec->cnt * ec->max) / (float)ec->out);
                }
                if (ec->out <= ec->cnt)
                {
                    ec->cnt = 0;
                    ec->flow = 3;
                }
            break;
            case 3:
                ec->flow = 4;
                ResetEffects(ec);
            return;
            }
        }
        else
        {
            if (ec->pnt[1] != NULL)
            {
                ef = *(float *)ec->pnt[1];
            }
            else
            {
                ef = ec->max;
            }
        }
    }

    sp = 1.0f;

    if (ec->pnt[2] != NULL)
    {
        sp = *(float *)ec->pnt[2];
    }

    rt = 1.0f;

    if (ec->pnt[4] != NULL)
    {
        rt = *(float *)ec->pnt[4];
    }

    if (ec->pnt[5] != NULL)
    {
        tr = 2.0f - *(float *)ec->pnt[5];
    }
    else
    {
        tr = 1.0f;
    }

    vol = (float)ef / 100.0f;

    if (page == 2)
    {
        if (sbj == 5)
        {
            ResetEffects(ec);
        }
        else if (sbj == 4)
        {
            ResetEffects(ec);
        }
        else
        {
            ec->dat.uc8[4] = SubPartsDeform2(ec, ec->dat.uc8[4], page, sbj, ec->dat.fl32[2], ec->dat.fl32[3], vol, 1, sp, rt, tr);
        }
    }
    else
    {
        ec->dat.uc8[4] = SubPartsDeform1(ec, ec->dat.uc8[4], page, sbj, ec->dat.fl32[2], ec->dat.fl32[3], vol, 1, sp, rt, tr);
    }

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SetVURand(float x)
{
    asm __volatile__(
        "qmtc2    $2,$vf12\n"
        "vrinit    R,$vf12x\n"
        : :"r"(x):"memory"
    );
}

int CalcPartsDeformXYZ(sceVu0IVECTOR vi, sceVu0FVECTOR vf)
{
    int ret;

    asm __volatile__(
        "lqc2       $vf12, 0x0(%0)\n"
        "vmulax.xyzw ACC, $vf4, $vf12x\n"
        "vmadday.xyzw ACC, $vf5, $vf12y\n"
        "vmaddaz.xyzw ACC, $vf6, $vf12z\n"
        "vmaddw.xyzw $vf13, $vf7, $vf0w\n"
        "vmulax.xyzw ACC, $vf8, $vf12x\n"
        "vmadday.xyzw ACC, $vf9, $vf12y\n"
        "vmaddaz.xyzw ACC, $vf10, $vf12z\n"
        "vmaddw.xyzw $vf14, $vf11, $vf0w\n"
        "vdiv       Q, $vf0w, $vf13w\n"
        "vnop\n"
        "vnop\n"
        "vclipw.xyz $vf14, $vf14w\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vwaitq\n"
        "vmulq.xyz  $vf13, $vf13, Q\n"
        "vnop\n"
        "vftoi4.xy  $vf13, $vf13\n"
        "vnop\n"
        "vftoi0.z   $vf13, $vf13\n"
        "vnop\n"
        "sqc2       $vf13, 0x0(%1)\n"
        "cfc2       $2, $vi18\n"
        : :"r"(vf),"r"(vi):"memory"
    );

    return ret;
}

void MakePartsDeformPacket(int pnumw, int pnumh, sceVu0FVECTOR *vt, sceVu0FMATRIX wlm, sceVu0FVECTOR *stq, u_char *use_alpha, float aprate, u_long tex0)
{
    int i;
    int j;
    int k;
    int l;
    int m;
    int bak;
    u_long *plong;
    u_int *pint;
    Q_WORDDATA *ppbuf;
    int clip[289];
    sceVu0IVECTOR vtiw[289];
    sceVu0IVECTOR colvec;
    sceVu0FMATRIX slm;

    _SetMulMatrixBB(SgWSMtx, SgCMtx, wlm);

    for (i = 0; i <  (pnumw + 1) * (pnumh + 1); i++)
    {
        clip[i] = CalcPartsDeformXYZ(vtiw[i], vt[i]) & 0x3f;
    }

    bak = ndpkt;

    m = pnumh * (pnumw + 1);

    ppbuf = Get2DPacketBufferAddress();

    ppbuf[0].ul128 = (u_long128)0;

    ppbuf[1].ul64[0] = SCE_GIF_SET_TAG(7, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    ppbuf[1].ul64[1] = SCE_GIF_PACKED_AD;

    ppbuf[2].ul64[0] = 0;
    ppbuf[2].ul64[1] = SCE_GS_TEXFLUSH;

    ppbuf[3].ul64[0] = tex0;
    ppbuf[3].ul64[1] = SCE_GS_TEX0_1;

    ppbuf[4].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    ppbuf[4].ul64[1] = SCE_GS_TEX1_1;

    ppbuf[5].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    ppbuf[5].ul64[1] = SCE_GS_ZBUF_1;

    ppbuf[6].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    ppbuf[6].ul64[1] = SCE_GS_ALPHA_1;

    ppbuf[7].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    ppbuf[7].ul64[1] = SCE_GS_TEST_1;

    ppbuf[8].ul64[0] = SCE_GS_SET_CLAMP_1(SCE_GS_CLAMP, SCE_GS_CLAMP, 0, 0, 0, 0);
    ppbuf[8].ul64[1] = SCE_GS_CLAMP_1;

    ppbuf[9].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 0, SCE_GIF_REGLIST, 2);
    ppbuf[9].ul64[1] = 0 \
        | SCE_GS_PRIM << (4 * 0)
        | 0x0F        << (4 * 1);

    ppbuf[10].ul64[0] = SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 1, 1, 0, 1, 0, 0, 0, 0);
    ppbuf[10].ul64[1] = 0;

    ppbuf += 11;

    for (i = 0; i < m; i++)
    {
        j = i + pnumw + 1;

        if (i % (pnumw + 1) == 0)
        {
            ppbuf[0].ul64[1] = 0 \
                | SCE_GS_ST    << (4 * 0)
                | SCE_GS_RGBAQ << (4 * 1)
                | SCE_GS_XYZF3 << (4 * 2)
                | SCE_GS_ST    << (4 * 3)
                | SCE_GS_RGBAQ << (4 * 4)
                | SCE_GS_XYZF3 << (4 * 5);
        }
        else
        {
            k = clip[i - 1] + clip[i + pnumw] + clip[i];
            l = clip[i + pnumw] + clip[i] + clip[j];

            if (k + l == 0) // k == 0 && l == 0
            {
                ppbuf[0].ul64[1] = 0 \
                    | SCE_GS_ST    << (4 * 0)
                    | SCE_GS_RGBAQ << (4 * 1)
                    | SCE_GS_XYZF2 << (4 * 2)
                    | SCE_GS_ST    << (4 * 3)
                    | SCE_GS_RGBAQ << (4 * 4)
                    | SCE_GS_XYZF2 << (4 * 5);
            }
            else if (k != 0 && l != 0)
            {
                ppbuf[0].ul64[1] = 0 \
                    | SCE_GS_ST    << (4 * 0)
                    | SCE_GS_RGBAQ << (4 * 1)
                    | SCE_GS_XYZF3 << (4 * 2)
                    | SCE_GS_ST    << (4 * 3)
                    | SCE_GS_RGBAQ << (4 * 4)
                    | SCE_GS_XYZF3 << (4 * 5);
            }
            else if (k != 0 && l == 0)
            {
                ppbuf[0].ul64[1] = 0 \
                    | SCE_GS_ST    << (4 * 0)
                    | SCE_GS_RGBAQ << (4 * 1)
                    | SCE_GS_XYZF3 << (4 * 2)
                    | SCE_GS_ST    << (4 * 3)
                    | SCE_GS_RGBAQ << (4 * 4)
                    | SCE_GS_XYZF2 << (4 * 5);
            }
            else if (k == 0 && l != 0)
            {
                ppbuf[0].ul64[1] = 0 \
                    | SCE_GS_ST    << (4 * 0)
                    | SCE_GS_RGBAQ << (4 * 1)
                    | SCE_GS_XYZF2 << (4 * 2)
                    | SCE_GS_ST    << (4 * 3)
                    | SCE_GS_RGBAQ << (4 * 4)
                    | SCE_GS_XYZF3 << (4 * 5);
            }
            else
            {
                continue;
            }
        }

        ppbuf[0].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 0, SCE_GIF_REGLIST, 6);

        plong = ppbuf[1].ul64;
        pint = ppbuf[1].ui32;

        pint[0] = ((u_int *)stq[i])[0];
        pint[1] = ((u_int *)stq[i])[1];

        plong[1] = SCE_GS_SET_RGBAQ(0x80, 0x80, 0x80, (u_char)(use_alpha[i] * aprate), ((u_int *)stq[i])[2]);
        plong[2] = SCE_GS_SET_XYZF(vtiw[i][0], vtiw[i][1], vtiw[i][2], 0);

        pint[6] = ((u_int *)stq[j])[0];
        pint[7] = ((u_int *)stq[j])[1];

        plong[4] = SCE_GS_SET_RGBAQ(0x80, 0x80, 0x80, (u_char)(use_alpha[j] * aprate), ((u_int *)stq[j])[2]);
        plong[5] = SCE_GS_SET_XYZF(vtiw[j][0], vtiw[j][1], vtiw[j][2], 0);

        ppbuf += 4;
    }

    Set2DPacketBufferAddress(ppbuf);

    pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;
}

u_char SubPartsDeform1(EFFECT_CONT *ec, u_char num, int page, int sbj, float sclx, float scly, float vol, int fl, float spd, float rate, float trate)
{
    #include "data/alpha1.h" // u_char alpha1[289];
    #include "data/alpha2.h" // u_char alpha2[289];
    EFFINFO2 *pefi;
    u_char ret_num;
    int i;
    int j;
    int k;
    int n;
    int vnumw;
    int vnumh;
    int pnumw;
    int pnumh;
    int wix;
    int wiy;
    float l;
    float lw;
    float fw;
    float cntw;
    float cnth;
    float rad;
    float ss;
    float cc;
    float fx;
    float fy;
    float fz;
    float rot_x;
    float rot_y;
    float f1;
    float f2;
    float f3;
    float f4;
    float xx;
    float yy;
    u_int clpx2;
    u_int clpy2;
    u_int clpz2;
    sceVu0FVECTOR stqparam[3];
    u_long tex0;
    sceVu0FVECTOR stq[289];
    sceVu0FVECTOR vt[289];
    sceVu0IVECTOR ivec;
    sceVu0FVECTOR vpos;
    sceVu0FVECTOR pos = { 25000.0f, -800.0f, 6500.0f, 1.0f };
    sceVu0FVECTOR fzero = { 0.0f, 0.0f, 0.0f, 1.0f };
    sceVu0FMATRIX slm;
    sceVu0FMATRIX wlm;
    float add;
    float wfw;
    float wfh;
    float ml;
    float aprate;
    float r;
    static float r2 = 0.0f;
    static float renz = 1.0f;
    static int passflg[10] = {0};
    static float passcnt[10] = {0};
    float sclx2;
    float scly2;

    sclx2 = sclx * 2;
    scly2 = scly * 2;

    vnumw = 17;
    vnumh = 17;
    pnumw = 16;
    clpz2 = 0x0fffffff;
    pnumh = 16;

    clpx2 = 0xfd00;
    clpy2 = 0xfd00;

    ml = 0.0f;
    add = 6.0f;

    cntw = ((pnumw / 2) % vnumw) * sclx2;
    cnth = ((pnumh / 2) % vnumw) * scly2;

    if (num == 0xff)
    {
        num = 0;

        i = 0;
        j = 0;
        ret_num = 0xff;

        while (i < 8 && j == 0)
        {

            if (efi[i].use == 0)
            {
                efi[i].use = 1;
                pefi = &efi[i];
                ret_num = i;
                j = 1;
            }

            i++;
        }

        if (ret_num == 0xff)
        {
            return 0xff;
        }

        r2 = 0;

        switch (sbj)
        {
        case 0:
        break;
        case 4:
        case 5:
            for (i = 0; i < vnumw*vnumh; i++)
            {
                wix = i % vnumw;
                wiy = i / vnumw;

                wfw = (float)wix * sclx2 - cntw;
                wfh = (float)wiy * scly2 - cnth;

                lw = SgSqrtf(wfw * wfw + wfh * wfh);

                if (i == 0)
                {
                    ml = lw;
                }

                l = (ml - lw);

                pefi->ep[i].lng = (l * l) * 0.006f * rate;
            }
        break;
        }
    }
    else
    {
        pefi = &efi[num];
        ret_num = num;
    }

    pefi->pass = 1;

    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

    Vu0CopyVector(vpos, ec->pnt[0]);

    fx = (vpos[0] - camera.p[0]) * (vpos[0] - camera.p[0]);
    fy = (vpos[1] - camera.p[1]) * (vpos[1] - camera.p[1]);
    fz = (vpos[2] - camera.p[2]) * (vpos[2] - camera.p[2]);

    l = SgSqrtf(fx + fy + fz);

    if (l > 200.0f)
    {
        aprate = vol;
    }
    else
    {
        if (l < 100.0f) aprate = 0.0f;
        else aprate = ((l - 100.0f) * vol) / 100.0f;
    }

    sceVu0UnitMatrix(wlm);

    wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;

    sceVu0RotMatrixY(wlm, wlm, rot_y);
    sceVu0TransMatrix(wlm, wlm, vpos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);
    sceVu0RotTransPers(ivec, slm, fzero, 0);

    n = 0;

    if ((ivec[0] >= 0 && ivec[0] < 0x300) || ivec[0] > clpx2)
    {
        n = 1;
    }

    if ((ivec[1] >= 0 && ivec[1] < 0x300) || ivec[1] > clpy2)
    {
        n = 1;
    }

    if (ivec[2] == 0 || ivec[2] > clpz2)
    {
        n = 1;
    }

    if (n != 0)
    {
        return (u_char)ret_num;
    }

    if (page == 0)
    {
        LocalCopyLtoLDraw((sys_wrk.count & 1) * 0x8c0, 0x1a40);
        tex0 = SCE_GS_SET_TEX0_1(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
    }
    else
    {
        if (eff_blur_off == 0)
        {
            tex0 = SCE_GS_SET_TEX0_1((sys_wrk.count + 1 & 1) * 0x8c0, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
        }
        else
        {
            LocalCopyLtoLDraw((sys_wrk.count & 1) * 0x8c0, 0x1a40);
            tex0 = SCE_GS_SET_TEX0_1(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
        }
    }

    f3 = sclx2 * 8.0f;
    f4 = scly2 * 8.0f;

    stqparam[0][0] = 1726.5f;
    stqparam[0][1] = 1936.0f - GetYOffsetf();
    stqparam[0][2] = trate;

    stqparam[1][0] = 0;
    stqparam[1][1] = 639.0f;
    stqparam[1][2] = 0;
    stqparam[1][3] = 223.0f;

    stqparam[2][0] = 1.0f / 1024.0f;
    stqparam[2][1] = 1.0f / 256.0f;
    stqparam[2][2] = 1.0;
    stqparam[2][3] = 1.0f / 16.0f;

    dummy1(slm);
    dummy2(stqparam);

    k = 0;

    yy = -f4;

    for (j = 0; j < vnumh; j++)
    {
        xx = -f3;

        for (i = 0; i < vnumw; i++)
        {
            vt[k][0] = xx;
            vt[k][1] = yy;
            vt[k][2] = 0.0f;
            vt[k][3] = 1.0f;

            xx += sclx2;
            k++;
        }
        yy += scly2;
    }

    for (i = 0; i < vnumh*vnumw; i++)
    {
        Vu0Func0000(stq[i], vt[i]);
    }

    if (ec->dat.uc8[1] & 1)
    {
        n = passflg[sbj] != sys_wrk.count;
        r = passcnt[sbj];
    }
    else
    {
        n = 1;
        r = ec->fw[0];
    }

    switch(sbj)
    {
    case 0:
    return ret_num;
    case 1: {
        float vtrate = rate * 0.8f;

        add = spd * 2;

        for (i = 0; i < vnumw*vnumh; i++)
        {
            vt[i][1] += SgSinfd(r + (float)((int)(i / vnumw) * 30)) * vtrate;
        }

        if (stop_effects == 0 && n != 0)
        {
            r += add;
            r = 360.0f < r ? r - 360.0f : r;
        }
    } break;
    case 2: {
        float vtrate;

        add = spd * 4.0f;
        vtrate = rate * 3.0f;

        for (i = 0; i < vnumw*vnumh; i++)
        {
            wix = i % vnumw;
            wiy = i / vnumw;

            wfw = (float)wix * sclx2 - cntw;
            wfh = (float)wiy * scly2 - cnth;

            lw = SgSqrtf(wfw * wfw + wfh * wfh);

            if (i == 0)
            {
                ml = lw;
            }

            l = (lw * 30.0f);

            vt[i][2] = SgSinfd(r + l) * (1.0f - lw / ml) * vtrate;
        }

        if (stop_effects == 0 && n != 0)
        {
            r = r - add;
            r = r < -360.0f ? r + 360.0f : r;
        }
    } break;
    case 3: {
        float rr = SgSinfd(r) * 60.0f;

        add = spd * 0.2f;

        for (i = 0; i < vnumw*vnumh; i++)
        {
            wix = i % vnumw;
            wiy = i / vnumw;

            wfw = (float)wix * sclx2 - cntw;
            wfh = (float)wiy * scly2 - cnth;

            lw = SgSqrtf(wfw * wfw + wfh * wfh);

            if (i == 0)
            {
                ml = lw;
            }

            l = (lw * 10.0f);

            ss = SgSinfd(rr * l) * (1.0f - lw / ml) * rate;

            vt[i][2] = ss;
        }

        if (stop_effects == 0 && n != 0)
        {
            r += add;
            r = 360.0f < r ? r - 360.0f : r;
        }
    } break;
    case 4: {
        float rr = SgSinfd(r) * 60.0f;

        add = spd * 2.0f;

        f1 = sclx2 * 8.0f;
        f2 = scly2 * 8.0f;

        for (i = 0; i < vnumw*vnumh; i++)
        {
            wix = i % vnumw;
            wiy = i / vnumw;

            f3 = (float)wix * sclx2;
            f4 = (float)wiy * scly2;

            wfw = f3 - cntw;
            wfh = f4 - cnth;

            l = pefi->ep[i].lng;

            if (wix != 0 && wix != pnumw && wiy != 0 && wiy != pnumh)
            {
                rad = ((rr * l) * PI) / 180.0f;

                ss = SgSinf(rad);
                cc = SgCosf(rad);

                vt[i][0] = (wfw * cc - wfh * ss + cntw) - f1;
                vt[i][1] = (wfw * ss + wfh * cc + cnth) - f2;
            }
            else
            {
                vt[i][0] = f3 - f1;
                vt[i][1] = f4 - f2;
            }
        }

        if (stop_effects == 0 && n != 0)
        {
            r += add;
            r = 180.0f < r ? r - 360.0f: r;
        }
    } break;
    case 5: {
        float rr = SgSinfd(r) * 60.0f;

        add = spd * 2.0f;

        f1 = sclx2 * 8.0f;
        f2 = scly2 * 8.0f;

        for (i = 0; i < vnumw*vnumh; i++)
        {
            wix = i % vnumw;
            wiy = i / vnumw;

            f3 = (float)wix * sclx2;
            f4 = (float)wiy * scly2;

            wfw = f3 - cntw;
            wfh = f4 - cnth;

            l = pefi->ep[i].lng;

            if (wix != 0 && wix != pnumw && wiy != 0 && wiy != pnumh)
            {
                rad = ((rr * l) * PI) / 180.0f;

                ss = SgSinf(rad);
                cc = SgCosf(rad);

                vt[i][0] = (wfw * cc - wfh * ss + cntw) - f1;
                vt[i][1] = (wfw * ss + wfh * cc + cnth) - f2;
            }
            else
            {
                vt[i][0] = f3 - f1;
                vt[i][1] = f4 - f2;
            }
        }

        if (stop_effects == 0 && n != 0)
        {
            r += add;
            r = 180.0f < r ? r - 360.0f: r;
        }
    } break;
    case 6:
        for (i = 0; i < vnumw*vnumh; i++)
        {
            wix = i % vnumw;
            wiy = i / vnumw;

            f1 = (float)wix * sclx2;
            f4 = (float)wiy * scly2;

            wfw = f1 - cntw;
            wfh = f4 - cnth;

            lw = SgSqrtf((wfw * wfw) + (wfh * wfh));

            if (i == 0)
            {
                ml = lw;
            }

            fw = SgCosfd(r) * renz;

            vt[i][0] = ((((1.0f - (lw / ml)) * fw) * wfw) + f1) - (sclx2 * 8.0f);
            vt[i][1] = ((((1.0f - (lw / ml)) * fw) * wfh) + f4) - (scly2 * 8.0f);
        }

        fw = SgCosf((r * PI) / 180.0f) * renz;

        if (stop_effects == 0 && n != 0)
        {
            r += add;
            r = 360.0f < r ? r - 360.0f : r;
        }



        if (SgCosfd(r) * fw < 0.0f)
        {
            if (renz <= 0.02f)
            {
                renz = 0.0f;
            }
            else
            {
                renz *= 0.8f;
            }
        }
    break;
    case 7:
        for (i = 0; i < vnumw*vnumh; i++)
        {
            wix = i % vnumw;
            wiy = i / vnumw;

            wfw = (wix * sclx2) - cntw;
            wfh = (wiy * scly2) - cnth;

            lw = SgSqrtf(wfw * wfw + wfh * wfh);

            if (i == 0)
            {
                ml = lw;
            }

            vt[i][0] = ((((1.0f - (lw / ml)) * wfw) * renz) + cntw) - (sclx2 * 8.0f);
            vt[i][1] = ((((1.0f - (lw / ml)) * wfh) * renz) + cnth) - (scly2 * 8.0f);
        }
    break;
    case 8:
        for (i = 0; i < vnumw*vnumh; i++)
        {
            wix = i % vnumw;
            wiy = i / vnumw;

            wfw = (wix * sclx2) - cntw;
            wfh = (wiy * scly2) - cnth;

            lw = SgSqrtf(wfw * wfw + wfh * wfh);

            if (i == 0)
            {
                ml = lw;
            }

            vt[i][0] = (((1.0f - ((lw / ml) * renz)) * wfw) + cntw) - (sclx2 * 8.0f);
            vt[i][1] = (((1.0f - ((lw / ml) * renz)) * wfh) + cnth) - (scly2 * 8.0f);
        }
    break;
    }

    passflg[sbj] = (u_int)sys_wrk.count;

    if (ec->dat.uc8[1] & 1)
    {
        passcnt[sbj] = r;
    }
    else
    {
        ec->fw[0] = r;
    }

    Reserve2DPacket(0x1000);

    if (page != 0)
    {
        MakePartsDeformPacket(pnumw, pnumh, vt, wlm, stq, alpha2, aprate, tex0);
    }
    else
    {
        MakePartsDeformPacket(pnumw, pnumh, vt, wlm, stq, alpha1, aprate, tex0);
    }

    return ret_num;
}

u_char SubPartsDeform2(EFFECT_CONT *ec, u_char num, int page, int sbj, float sclx, float scly, float vol, int fl, float spd, float rate, float trate)
{
    #include "data/alpha.h" // u_char alpha[289];
    EFFPOS *pefp;
    EFFINFO2 *pefi;
    static EFFINFO2 pefi_once[3];
    u_char retnum;
    int i;
    int j;
    int k;
    int n;
    int vnumw;
    int vnumh;
    int pnumw;
    int pnumh;
    int wix;
    int wiy;
    float l;
    float lw;
    float cntw;
    float cnth;
    float fx;
    float fy;
    float fz;
    float rot_x;
    float rot_y;
    float pr11;
    float pr12;
    float pr21;
    float pr22;
    float comp;
    float wfw;
    float wfh;
    float add;
    float ml;
    float aprate;
    float xx;
    float yy;
    u_int clpx2;
    u_int clpy2;
    u_int clpz2;
    sceVu0FVECTOR stqparam[3];
    sceVu0FVECTOR stq[289];
    sceVu0FVECTOR vt[289];
    sceVu0IVECTOR ivec;
    sceVu0FVECTOR vpos;
    sceVu0FVECTOR pos = { 25000.0f, -800.0f, 6500.0f, 1.0f };
    sceVu0FVECTOR fzero = { 0.0f, 0.0f, 0.0f, 1.0f };
    sceVu0FMATRIX slm;
    sceVu0FMATRIX wlm;
    float sclx2;
    float scly2;
    u_long tex0;

    tex0 = SCE_GS_SET_TEX0_1(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);

    sclx2 = sclx * 2;
    scly2 = scly * 2;

    vnumw = 17;
    vnumh = 17;
    pnumw = 16;
    pnumh = 16;

    ml = 0;

    pr11 = 1.0f;
    pr12 = -0.3f;
    pr21 = 0.4f;
    pr22 = 0.2f;

    cntw = ((pnumw / 2) % vnumw) * sclx2;
    cnth = ((pnumh / 2) % vnumw) * scly2;

    clpx2 = 0xfd00;
    clpy2 = 0xfd00;
    clpz2 = 0x0fffffff;

    if (init_pdef2 != 0)
    {
        for (j = 0; j < 3; j++)
        {
            pefi = &pefi_once[j];

            pefi->r = 0.0f;

            switch (j)
            {
            case 0:
                for (i = 0; i < vnumw*vnumh; i++)
                {
                    pefi->ep[i].r = vu0Rand() * 360.0f;
                    pefi->ep[i].h = vu0Rand() * 0.4f;
                    pefi->ep[i].add = vu0Rand() * 1.5f;
                }
            break;
            case 1:
                for (i = 0; i < vnumw*vnumh; i++)
                {
                    wix = i % vnumw;
                    wiy = i / vnumw;

                    wfw = wix * sclx2 - cntw;
                    wfh = wiy * scly2 - cnth;

                    lw = SgSqrtf(wfw * wfw + wfh * wfh);

                    if (i == 0)
                    {
                        ml = lw;
                    }

                    pefi->ep[i].lng = lw * 150.0f;
                    pefi->ep[i].r = vu0Rand() * 360.0f;
                    pefi->ep[i].h = vu0Rand() * 4.0f;
                    pefi->ep[i].add = vu0Rand() * 3.0f;
                }
            break;
            case 2:
                for (i = 0; i < vnumw*vnumh; i++)
                {
                    wix = i % vnumw;
                    wiy = i / vnumw;

                    if (wix == 0)
                    {
                        pefi->ep[i].h = pr11 * vu0Rand() + pr12;
                    }
                    else
                    {
                        pefi->ep[i].h = (pefi->ep[i-1].h + pr21 * vu0Rand()) - pr22;
                    }

                    if (wiy == 0)
                    {
                        pefi->ep[i].r = pr11 * vu0Rand() + pr12;
                    }
                    else
                    {
                        pefi->ep[i].r = (pefi->ep[i-vnumh].r + pr21 * vu0Rand()) - pr22;
                    }
                }
            break;
            }
        }

        init_pdef2 = 0;
    }

    if (ec->dat.uc8[1] & 1)
    {
        pefi = &pefi_once[sbj-1];
    }
    else
    {
        if (num == 0xff)
        {
            i = 0;
            j = 0;

            retnum = 0xff;

            while (i < 8 && j== 0)
            {
                if (efi[i].use == 0)
                {
                    efi[i].use = 1;
                    pefi = &efi[i];
                    retnum = i;
                    j = 1;
                }

                i++;
            }

            if (retnum == 0xff)
            {
                return 0xff;
            }

            pefi->r = 0.0f;

            switch (sbj)
            {
            case 0:
            break;
            case 1:
                for (i = 0; i < vnumw*vnumh; i++)
                {
                    pefi->ep[i].r = vu0Rand() * 360.0f;
                    pefi->ep[i].h = vu0Rand() * 0.4f + 0.1f;
                    pefi->ep[i].add = vu0Rand() * 1.5f * spd + spd;
                }
            break;
            case 2:
                for (i = 0; i < vnumw*vnumh; i++)
                {
                    wix = i % vnumw;
                    wiy = i / vnumw;

                    wfw = wix * sclx2 - cntw;
                    wfh = wiy * scly2 - cnth;

                    lw = SgSqrtf(wfw * wfw + wfh * wfh);

                    if (i == 0)
                    {
                        ml = lw;
                    }

                    pefi->ep[i].lng = lw * 150.0f;
                    pefi->ep[i].r = vu0Rand() * 360.0f;
                    pefi->ep[i].h = vu0Rand() * 4.0f + 1.0f;
                    pefi->ep[i].add = (vu0Rand() * 3.0f + 2.0f) * spd;
                }
            break;
            case 3:
                for (i = 0; i < vnumw*vnumh; i++)
                {
                    wix = i % vnumw;
                    wiy = i / vnumw;

                    if (wix == 0)
                    {
                        pefi->ep[i].h = pr11 * vu0Rand() + pr12;
                    }
                    else
                    {
                        pefi->ep[i].h = (pefi->ep[i-1].h + pr21 * vu0Rand()) - pr22;
                    }

                    if (wiy == 0)
                    {
                        pefi->ep[i].r = pr11 * vu0Rand() + pr12;
                    }
                    else
                    {
                        pefi->ep[i].r = (pefi->ep[i-vnumw].r + pr21 * vu0Rand()) - pr22;
                    }
                }
            break;
            }
        }
        else
        {
            pefi = &efi[num];
            retnum = num;
        }

        pefi->pass = 1;
    }

    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);
    Vu0CopyVector(vpos, ec->pnt[0]);

    fx = (vpos[0] - camera.p[0]) * (vpos[0] - camera.p[0]);
    fy = (vpos[1] - camera.p[1]) * (vpos[1] - camera.p[1]);
    fz = (vpos[2] - camera.p[2]) * (vpos[2] - camera.p[2]);

    l = SgSqrtf(fx + fy + fz);

    if (750.0f < l)
    {
        comp = l / 1300.0f;
    }
    else
    {
        comp = 0.57692307f;
    }

    aprate = l > 200.0f ? vol : (l < 100.0f ? 0 : ((l - 100.0f) * vol) / 100.0f);

    sceVu0UnitMatrix(wlm);

    wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;

    sceVu0RotMatrixY(wlm, wlm, rot_y);
    sceVu0TransMatrix(wlm, wlm, vpos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);
    sceVu0RotTransPers(ivec, slm, fzero, 0);

    n = 0;

    if ((ivec[0] >= 0 && ivec[0] < 0x300) || ivec[0] > clpx2)
    {
        n = 1;
    }

    if ((ivec[1] >= 0 && ivec[1] < 0x300) || ivec[1] > clpy2)
    {
        n = 1;
    }

    if (ivec[2] == 0 || ivec[2] > clpz2)
    {
        n = 1;
    }

    if (n)
    {
        return retnum;
    }

    LocalCopyLtoLDraw((sys_wrk.count & 1) * 0x8c0, 0x1a40);

    stqparam[0][0] = 1726.5f;
    stqparam[0][1] = 1936.0f - GetYOffsetf();
    stqparam[0][2] = 1.0f;

    stqparam[1][0] = 0;
    stqparam[1][1] = 639.0f;
    stqparam[1][2] = 0;
    stqparam[1][3] = 223.0f;

    stqparam[2][0] = 1.0f / 1024.0f;
    stqparam[2][1] = 1.0f / 256.0f;
    stqparam[2][2] = 1.0f;
    stqparam[2][3] = 1.0f / 16.0f;

    dummy1(slm);
    dummy2(stqparam);

    k = 0;

    yy = scly2 * -8.0f;
    for (j = 0; j < vnumh; j++)
    {
        xx = sclx2 * -8.0f;
        for (i = 0; i < vnumw; i++)
        {
            vt[k][0] = xx;
            vt[k][1] = yy;
            vt[k][2] = 0.0f;
            vt[k][3] = 1.0f;

            Vu0Func0000(stq[k], vt[k]);

            xx += sclx2;
            k++;
        }
        yy += scly2;
    }

    switch (sbj)
    {
    case 0:
        return retnum;
    break;
    case 1:
        for (i = 0; i < vnumw*vnumh; i++)
        {
            pefp = &pefi->ep[i];

            wix = i % vnumw;
            wiy = i / vnumw;

            // if (wix != 0 && wix != pnumw && wiy != 0 && wiy != pnumh)
            // if (wix != 0 && wix != 0x10 && wiy != 0 && wiy != 0x10)
            if (wix != 0 && wix != vnumw-1 && wiy != 0 && wiy != vnumw-1)
            {
                l = SgSinf(((pefp->r + wiy * 50.0f) * PI) / 180.0f);
                l = l * pefp->h * comp * rate;
                vt[i][0] += l;

                l = SgSinf(((pefp->r + wix * 50.0f) * PI) / 180.0f);
                l = l * pefp->h * comp * rate;
                vt[i][1] += l;

                if (stop_effects == 0)
                {
                    if (vt[i][0] >= 0.0f && pefp->ox < 0.0f)
                    {
                        pefp->h = vu0Rand() * 0.4f + 0.1f;
                        pefp->add = vu0Rand() * 1.5f * spd + spd;
                    }
                }
            }

            if (stop_effects == 0)
            {
                pefp->r = pefp->r - pefp->add < -360.0f ? (pefp->r + 360.0f) - pefp->add : pefp->r - pefp->add;
                pefp->ox = vt[i][0];
            }
        }
    break;
    case 2:
        for (i = 0; i < vnumw*vnumh; i++)
        {
            wix = i % vnumw;
            wiy = i / vnumw;

            pefp = &pefi->ep[i];

            l = pefi->ep[i].lng;

            if (i == 0)
            {
                ml = l;
            }

            // if (wix != 0 && wix != pnumw && wiy != 0 && wiy != pnumh)
            // if (wix != 0 && wix != 0x10 && wiy != 0 && wiy != 0x10)
            if (wix != 0 && wix != vnumw-1 && wiy != 0 && wiy != vnumw-1)
            {
                vt[i][2] = SgSinf(((pefp->r + l) * PI) / 180.0f) * (1.0f - l / ml) * pefp->h * comp * rate;

                if (stop_effects == 0)
                {
                    if (vt[i][2] >= 0.0f && pefp->ox < 0.0f)
                    {
                        pefp->h = vu0Rand() * 4.0f + 1.0f;
                        pefp->add = (vu0Rand() * 3.0f + 2.0f) * spd;
                    }
                }
            }

            if (stop_effects == 0)
            {
                pefp->r = pefp->r - pefp->add < -360.0f ? (pefp->r + 360.0f) - pefp->add : pefp->r - pefp->add;
                pefp->ox = vt[i][2];
            }
        }
    break;
    case 3:
        add = spd * 2.0f;
        if (stop_effects == 0)
        {
            float vtrate = comp * rate;

            for (i = 0; i < vnumw*vnumh; i++)
            {
                wix = i % vnumw;
                wiy = i / vnumw;

                pefp = &pefi->ep[i];

                pefp->x = SgSinfd(pefi->r + wiy * 32.0f) * pefp->r * vtrate;
                pefp->y = SgSinfd(pefi->r + wix * 32.0f) * pefp->h * vtrate;

                if (pefp->y >= 0 && pefp->oy < 0.0f)
                {
                    if (wix != 0)
                    {
                        pefp->h = (pefi->ep[i-1].h + pr21 * vu0Rand()) - pr22;
                    }
                    else
                    {
                        pefp->h = pr11 * vu0Rand() + pr12;
                    }
                }

                if (pefp->x >= 0 && pefp->ox < 0.0f)
                {
                    if (wiy != 0)
                    {
                        pefp->r = (pefi->ep[i-vnumh].r + pr21 * vu0Rand()) - pr22;
                    }
                    else
                    {
                        pefp->r = pr11 * vu0Rand() + pr12;
                    }
                }

                pefp->ox = pefp->x;
                pefp->oy = pefp->y;

                vt[i][0] += pefp->x;
                vt[i][1] += pefp->y;
            }

            pefi->r += add;
            if (360.0f < pefi->r)
            {
                pefi->r = pefi->r - 360.0f;
            }
        }
        else
        {
            for (i = 0; i < vnumw*vnumh; i++)
            {
                pefp = &pefi->ep[i];
                vt[i][0] += pefp->x;
                vt[i][1] += pefp->y;
            }
        }
    break;
    }

    Reserve2DPacket(0x1000);
    MakePartsDeformPacket(pnumw, pnumh, vt, wlm, stq, alpha, aprate, tex0);

    return retnum;
}

int GetCornHitCheck(float *bpos, float power)
{
    float rrate;
    float lrate;

    return GetCornHitCheck2(bpos, power, &rrate, &lrate);
}

int GetCornHitCheck2(float *bpos, float power, float *rrate, float *lrate)
{
    sceVu0FMATRIX wlm1;
    sceVu0FMATRIX wlm2;
    sceVu0FVECTOR wpos;
    float rot_x;
    float rot_y;
    float lp;
    float lc;
    float ang;

    ang = 40.0f;

    rot_x = plyr_wrk.spot_rot[0] + plyr_wrk.move_box.rot[0];
    rot_y = plyr_wrk.spot_rot[1] + plyr_wrk.move_box.rot[1];

    while (rot_x < -PI) rot_x += PI2;
    while (PI <= rot_x) rot_x -= PI2;

    while (rot_y < -PI) rot_y += PI2;
    while (PI <= rot_y) rot_y -= PI2;

    sceVu0UnitMatrix(wlm1);
    sceVu0RotMatrixX(wlm1, wlm1, rot_x);
    sceVu0RotMatrixY(wlm1, wlm1, rot_y);
    sceVu0TransMatrix(wlm1, wlm1, plyr_wrk.spot_pos);
    sceVu0InversMatrix(wlm2, wlm1);
    sceVu0ApplyMatrix(wpos, wlm2, bpos);

    lc = wpos[2] * tanf((PI * ang) / 180.0f);
    lp = SgSqrtf(wpos[0] * wpos[0] + wpos[1] * wpos[1]);

    *rrate = lp / lc;
    *lrate = wpos[2] / power;

    if (wpos[2] > 0.0f && wpos[2] < power && lp < lc)
    {
        return 1;
    }

    return 0;
}

void SetRenzFlare(EFFECT_CONT *ec)
{
    static float f1bk = 0.0f;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0FVECTOR tpos;
    sceVu0FVECTOR pos1;
    sceVu0FVECTOR vpos;
    sceVu0FVECTOR trot;
    sceVu0FVECTOR wppos = { 0.0f, 0.0f, 0.0f, 1.0f };
    float t1rot_x;
    float t2rot_x;
    float t1rot_y;
    float t2rot_y;
    float f1;
    float f2;
    float rx;
    float ry;
    float mx;
    float my;
    float ang;
    int i;
    int ok;
    int tp;
    u_int ui;
    #include "data/efrenz.h" // EFRENZ efrenz[9];
    static sceGsStoreImage gs_simage1;
    Q_WORDDATA q;
    int n1;
    int n2;

    Vu0CopyVector(tpos, ec->pnt[0]);
    Vu0CopyVector(trot, ec->pnt[1]);

    Get2PosRot(camera.i,camera.p, &f1, &f2);

    t1rot_x = (trot[0] * 180.0f) / PI;
    t1rot_y = (trot[1] * 180.0f) / PI;

    if ((f1 < 10*PI) && (-10*PI < f1))
    {
        t2rot_x = (f1 * 180.0f) / PI - 180.0f;
    }

    if ((f2 < 10*PI) && (-10*PI < f2))
    {
        t2rot_y = (f2 * 180.0f) / PI;
    }

    while (t1rot_x < 0.0f) t1rot_x += 360.0f;
    while (t2rot_x < 0.0f) t2rot_x += 360.0f;

    while (t1rot_x + 180.0f < t2rot_x) t1rot_x += 360.0f;
    while (t2rot_x + 180.0f < t1rot_x) t2rot_x += 360.0f;

    while (t1rot_y < 0.0f) t1rot_y += 360.0f;
    while (t2rot_y < 0.0f) t2rot_y += 360.0f;

    while (t1rot_y + 180.0f < t2rot_y) t1rot_y += 360.0f;
    while (t2rot_y + 180.0f < t1rot_y) t2rot_y += 360.0f;

    n1 = (int)(t1rot_x - t2rot_x);
    n1 = n1 < 0 ? -n1 : n1;
    rx = n1 < 60.0f ? (60.0f - n1) / 60.0f : 0.0f;

    n1 = (int)(t1rot_y - t2rot_y);
    n1 = n1 < 0 ? -n1 : n1;
    ry = n1 < 60.0f ? (60.0f - n1) / 60.0f : 0.0f;

    GetCamI2DPos(tpos, &f1, &f2);

    mx = f1 - 320.0f;
    my = f2 - 112.0f;

    ang = (SgAtan2f(mx, my) * 180.0f) / PI + 45.0f;

    sceVu0UnitMatrix(wlm);

    wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;

    sceVu0TransMatrix(wlm, wlm, tpos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);
    sceVu0ApplyMatrix(pos1, slm, wppos);
    sceVu0ScaleVector(pos1, pos1, 1.0f / pos1[3]);

    if (plyr_wrk.sta & 1)
    {
        ok = 1;
        tp = 0;
    }
    else
    {
        n1 = (int)f1 / 4;
        n2 = (int)f1 % 4;

        sceGsSetDefStoreImage (
            &gs_simage1,
            (DISP_WIDTH * DISP_HEIGHT) / 64,
            DISP_WIDTH / 64,
            SCE_GS_PSMZ32,
            n1 * 4,
            f2,
            4,
            1);

        FlushCache(0);
        CheckDMATrans();
        sceGsExecStoreImage(&gs_simage1, &q.ul128);
        CheckDMATrans();
        sceGsSyncPath(0,0);

        ui = q.ui32[n2] & 0xffffff;

        if ((ui >> 4) * 15.5f < pos1[2])
        {
            ok = 1;
        }
        else
        {
            ok = 0;
        }

        tp = 1;
    }

    if (ok)
    {
        if (stop_effects == 0)
        {
            f1 = vu0Rand() / 100.0f + 0.25f;
            f1bk = f1;
        }
        else
        {
            f1 = f1bk;
        }

        f1 = (rx * ry) * f1;

        if (f1 != 0.0f)
        {
            for (i = 1; i < 10; i++)
            {
                vpos[0] = pos1[0] + mx * efrenz[i - 1].lscl;
                vpos[1] = pos1[1] + my * efrenz[i - 1].lscl;

                if (tp == 0)
                {
                    vpos[2] = pos1[2];
                }
                else
                {
                    vpos[2] = 2.683945e+08;
                }

                vpos[3] = 1.0f;

                if (i == 4)
                {
                    SetEffSQTex(0x16, vpos, tp, 80.0f, 80.0f, 0xff, 0x80, 0x80, (u_char)(f1 * 16.0f));

                    if (ec->dat.uc8[2] != 0)
                    {
                        SetStarRay(vpos,tp,f1,ec->dat.uc8[2],-ang);
                    }

                    SetEffSQTex(0x16, vpos, tp, 24.0f, 24.0f, 0xff, 0xc0, 0xd0, (u_char)(f1 * 32.0f));
                    SetEffSQTex(0x16, vpos, tp, 12.0f, 12.0f, 0xff, 0xc0, 0xd0, (u_char)(f1 * 48.0f));
                }

                SetEffSQTex(
                    efrenz[i - 1].type,
                    vpos,
                    tp,
                    efrenz[i - 1].tscl * 10.0f,
                    efrenz[i - 1].tscl * 10.0f,
                    efrenz[i - 1].rgba[0],
                    efrenz[i - 1].rgba[1],
                    efrenz[i - 1].rgba[2],
                    (u_char)(efrenz[i - 1].rgba[3] * f1));
            }
        }
    }

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void SetStarRay(float *bpos, int tp, float sc, int num, float aang)
{
    int i;
    int x[4];
    int y[4];
    u_int z;
    float f;
    float div;
    float ss;
    float cc;
    float ang;
    float pos2[4][2];
    float pos[4][3] = {
        { 0.0f, 4.0f, 0.0f },
        { 0.0f, 0.0f, 28.0f },
        { -100.0f, 0.0f, 0.0f },
        { 0.0f, -4.0f, 0.0f },
    };
    u_char rr;
    u_char gg;
    u_char bb;
    float bx;
    float by;

    if (monochrome_mode != 0)
    {
        bb = 0xda;
        gg = 0xda;
        rr = 0xda;
    }
    else
    {
        rr = 0xff;
        gg = 0xc0;
        bb = 0xd0;
    }

    bx = bpos[0];
    by = bpos[1];
    z = bpos[2] * 16.0f;

    div = g_bInterlace != 0 ? 2.0f : 1.0f;

    ang = 360.0f / (float)num;

    Reserve2DPacket(0x1000);

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = num * 8 + DMAend + 5 ;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(3, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, tp);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_SCISSOR_2(0x8000 | num * 4, 0, 0x4000, 0x2026);
    pbuf[ndpkt++].ul64[1] = SCE_GS_SCISSOR_2;

    for (f = 0.0f; f < 360.0f; f += ang)
    {
        ss = SgSinfd(f + aang);
        cc = SgCosfd(f + aang);

        for (i = 0; i < 4; i++)
        {
            pos2[i][0] = pos[i][0] * ss - pos[i][1] * cc;
            pos2[i][1] = pos[i][0] * cc + pos[i][1] * ss;

            x[i] = (pos2[i][0] + bx) * 16.0f;
            y[i] = (pos2[i][1] / div + by) * 16.0f;

            pbuf[ndpkt].ui32[0] = rr;
            pbuf[ndpkt].ui32[1] = gg;
            pbuf[ndpkt].ui32[2] = bb;
            pbuf[ndpkt++].ui32[3] = (int)(pos[i][2] * sc);

            pbuf[ndpkt].ui32[0] = x[i];
            pbuf[ndpkt].ui32[1] = y[i];
            pbuf[ndpkt].ui32[2] = z;
            pbuf[ndpkt++].ui32[3] = (i > 1) ? 0 : 0x8000;
        }
    }
}

void SetNegaCircle(EFFECT_CONT *ec)
{
    int i;
    int n;
    int bak;
    int r;
    int g;
    int b;
    u_int alp;
    u_int z;
    float f;
    float add;
    float bx;
    float by;
    float rad;
    sceVu0FVECTOR ncf[37];
    sceVu0IVECTOR nci[37];
    sceVu0IVECTOR bipos;

    bx = ec->dat.fl32[1];
    by = ec->dat.fl32[2];
    rad = ec->dat.fl32[3];
    alp = ec->dat.uc8[3];

    r = ec->in;
    g = ec->keep;
    b = ec->out;
    z = 0xfffefff;

    add = 10.0f;

    n = 0;

    for (f = 0.0; f < 360.0f; f += add)
    {
        ncf[n][0] = rad * SgCosf((f * PI) / 180.0f);
        ncf[n][1] = rad * SgSinf((f * PI) / 180.0f);
        ncf[n][2] = 0.0f;
        ncf[n][3] = 1.0f;

        n++;
    }

    Vu0CopyVector(ncf[n], ncf[0]);

    n++;

    bipos[0] = (u_int)((bx + 2048.0f) * 16.0f);
    bipos[1] = (u_int)((by * 0.5f + 2048.0f) * 16.0f);

    for (i = 0; i < n; i++)
    {
        nci[i][0] = (u_int)((ncf[i][0] + bx + 2048.0f) * 16.0f);
        nci[i][1] = (u_int)(((ncf[i][1] + by) * 0.5f + 2048.0f) * 16.0f);
    }

    Reserve2DPacket(0x1000);

    bak = ndpkt;

    pbuf[ndpkt++].ul128 = (u_long128)0;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(3, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_ZERO, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_SCISSOR_2(0x8001, 0, 0xc000, 0x2022);
    pbuf[ndpkt++].ul64[1] = SCE_GS_SCISSOR_2;

    pbuf[ndpkt].ui32[0] = r;
    pbuf[ndpkt].ui32[1] = g;
    pbuf[ndpkt].ui32[2] = b;
    pbuf[ndpkt++].ui32[3] = alp;

    pbuf[ndpkt].ui32[0] = bipos[0];
    pbuf[ndpkt].ui32[1] = bipos[1];
    pbuf[ndpkt].ui32[2] = z;
    pbuf[ndpkt++].ui32[3] = 0;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(n, SCE_GS_TRUE, SCE_GS_TRUE, 69, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_XYZF2 << (4 * 0);

    for (i = 0; i < n; i++)
    {
        pbuf[ndpkt].ui32[0] = nci[i][0];
        pbuf[ndpkt].ui32[1] = nci[i][1];
        pbuf[ndpkt].ui32[2] = z;
        pbuf[ndpkt++].ui32[3] = (i > 1) ? 0 : 0x8000;
    }

    pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
    }
}

void _SetPartsDeformSTQRegs(sceVu0FVECTOR *params)
{
    asm __volatile__(
        "lqc2    $vf8,0(%0)\n"
        "lqc2    $vf9,0x10(%0)\n"
        "lqc2    $vf10,0x20(%0)\n"
        : :"r"(params):"memory"
    );
}

void _CalcParstDeformSTQ(float *stq, float *vt)
{
    asm __volatile__(
        "lqc2       $vf12, 0x0(%0)\n"
        "vmulz.xy   $vf12, $vf12, $vf8z\n"
        "vmulaw.xyzw ACC, $vf7, $vf0w\n"
        "vmaddaz.xyzw ACC, $vf6, $vf12z\n"
        "vmadday.xyzw ACC, $vf5, $vf12y\n"
        "vmaddx.xyzw $vf13, $vf4, $vf12x\n"
        "vdiv       Q, $vf0w, $vf13w\n"
        "vwaitq\n"
        "vmulq.xyz  $vf13, $vf13, Q\n"
        "vsub.xy    $vf13, $vf13, $vf8\n"
        "vmaxx.x    $vf13, $vf13, $vf9x\n"
        "vminiy.x   $vf13, $vf13, $vf9y\n"
        "vmaxz.y    $vf13, $vf13, $vf9z\n"
        "vminiw.y   $vf13, $vf13, $vf9w\n"
        "vmulq.xy   $vf14, $vf10, Q\n"
        "vmul.xy    $vf13, $vf13, $vf14\n"
        "vmulq.z    $vf13, $vf10, Q\n"
        "vmulw.xyz  $vf13, $vf13, $vf10w\n"
        "sqc2       $vf13, 0x0(%1)\n"
        : :"r"(vt),"r"(stq):"memory"
    );
}

EFFINFO2 efi[8] = {0};
