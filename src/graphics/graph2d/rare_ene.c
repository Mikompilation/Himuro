#include "common.h"
#include "typedefs.h"
#include "rare_ene.h"

#include "sce/libvu0.h"

#include "main/glob.h"
#include "ingame/map/map_area.h"
#include "ingame/entry/ap_rgost.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph2d/tim2_new.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/libsg.h"

typedef struct {
    SPRT_DAT *spr;
    u_int num;
} RARE_ENE_1DAT;

typedef struct {
    RARE_ENE_1DAT *re1d;
    float sclw;
    float sclh;
    u_short attr;
    u_char attr_sub;
    u_char pat;
} RARE_ENE_DAT;

#include "data/rare_ene00101.h" // static SPRT_DAT rare_ene00101[];
#include "data/rare_ene00102.h" // static SPRT_DAT rare_ene00102[];
#include "data/rare_ene001.h"   // static RARE_ENE_1DAT rare_ene001[];
#include "data/rare_ene.h"      // static RARE_ENE_DAT rare_ene[];
#include "data/rare_textbl.h"   // static u_int rare_textbl[];
#include "data/pazz_ene00101.h" // static SPRT_DAT pazz_ene00101[];
#include "data/pazz_ene001.h"   // static RARE_ENE_1DAT pazz_ene001[];
#include "data/pazz_ene00201.h" // static SPRT_DAT pazz_ene00201[];
#include "data/pazz_ene002.h"   // static RARE_ENE_1DAT pazz_ene002[];
#include "data/pazz_ene00301.h" // static SPRT_DAT pazz_ene00301[];
#include "data/pazz_ene003.h"   // static RARE_ENE_1DAT pazz_ene003[];
#include "data/pazz_ene00401.h" // static SPRT_DAT pazz_ene00401[];
#include "data/pazz_ene004.h"   // static RARE_ENE_1DAT pazz_ene004[];
#include "data/pazz_ene00501.h" // static SPRT_DAT pazz_ene00501[];
#include "data/pazz_ene005.h"   // static RARE_ENE_1DAT pazz_ene005[];
#include "data/pazz_ene00601.h" // static SPRT_DAT pazz_ene00601[];
#include "data/pazz_ene006.h"   // static RARE_ENE_1DAT pazz_ene006[];
#include "data/pazz_ene00701.h" // static SPRT_DAT pazz_ene00701[];
#include "data/pazz_ene007.h"   // static RARE_ENE_1DAT pazz_ene007[];
#include "data/pazz_ene10301.h" // static SPRT_DAT pazz_ene10301[];
#include "data/pazz_ene103.h"   // static RARE_ENE_1DAT pazz_ene103[];
#include "data/pazz_ene.h"      // static RARE_ENE_DAT pazz_ene[];
#include "data/pazz_textbl.h"   // u_int pazz_textbl[];

#define PI 3.1415928f
#define PI_HALF 1.5707964f

void LoadRareEneTex()
{
    return;
}

void DrawRareEne_Sub(int mno, int dno, sceVu0FVECTOR pos, int tblno, int ani, int al)
{
    unsigned char alp;
    int a;
    int b;
    int i;
    int j;
    int k;
    int m;
    int n;
    int tx[4];
    int ty[4];
    int offset;
    float fm;
    float rot_x;
    float rot_y;
    RARE_ENE_DAT *redp;
    RARE_ENE_1DAT *re1dp;
    SPRT_DAT *sdp;
    Q_WORDDATA qw[1024];
    sceVu0FMATRIX slm;
    sceVu0FMATRIX wlm;
    sceVu0IVECTOR ivec[4];
    sceVu0FVECTOR fvec[4];
    sceVu0FVECTOR bpos  = { 25000.0f, -400.0f, 6500.0f, 1.0f };
    sceVu0FVECTOR fzero = { 0.0f, 0.0f, 0.0f, 1.0f };
    int rx;
    int ry;
    int sp;
    int sub;

    sceGsTex0 tx0;
    int one;

    one = 1;
    alp = al;

    Vu0CopyVector(bpos, pos);

    if (tblno < 0x80)
    {
        redp = &rare_ene[tblno];
    }
    else
    {
        redp = &((RARE_ENE_DAT *)&draw_mpri[902])[tblno];
    }

    re1dp = redp[ani].re1d;
    sdp = re1dp->spr;

    rx = (redp->attr & 0x4000) != 0 || (redp->attr & 0x8000) != 0;
    ry = (redp->attr & 0x2000) != 0 || (redp->attr & 0x8000) != 0;
    sp = (redp->attr & 0x100) != 0;

    sub = redp->attr_sub;

    if (redp->attr & 0x1000 && plyr_wrk.mode == 1)
    {
        return;
    }

    offset = dno * 256;

    SetSprFile2(mno * 0x10000 + 0x1c90000, offset);

    sceVu0UnitMatrix(wlm);

    wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;

    sceVu0TransMatrix(wlm, wlm, bpos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);
    sceVu0RotTransPers(ivec[0], slm, fzero, 0);

    k = 0;

    if (ivec[0][0] < 0x4000 || ivec[0][0] > 0xc000)
    {
        k = 1;
    }

    if (ivec[0][1] < 0x5800 || ivec[0][1] > 0xa800)
    {
        k = 1;
    }

    if (ivec[0][2] < 0xff   || ivec[0][2] > 0xffffff)
    {
        k = 1;
    }

    if (!k)
    {
        Reserve2DPacket(0x1000);

        n = ndpkt;

        pbuf[ndpkt++].ul128 = (u_long128)0;

        Get2PosRot(camera.p, bpos, &rot_x, &rot_y);

        for (i = 0; i < re1dp->num; i++)
        {
            sceVu0UnitMatrix(wlm);

            wlm[0][0] = redp->sclw * 25.0f;
            wlm[1][1] = redp->sclh * 25.0f;
            wlm[2][2] = 25.0f;

            if (sp != 0 && sub == 1)
            {
                sceVu0RotMatrixX(wlm, wlm, (0.0f < rot_x) ? rot_x - PI : rot_x + PI);
                sceVu0RotMatrixY(wlm, wlm, PI);
            }

            if (sp != 0 && sub == 3)
            {
                sceVu0RotMatrixY(wlm, wlm, PI_HALF);
            }

            if (rx != 0)
            {
                sceVu0RotMatrixX(wlm, wlm, rot_x);
            }

            if (ry != 0)
            {
                sceVu0RotMatrixY(wlm, wlm, rot_y);
            }

            sceVu0TransMatrix(wlm, wlm, bpos);
            sceVu0MulMatrix(slm, SgWSMtx, wlm);

            tx0 = *(sceGsTex0*)&(sdp + i*2 + monochrome_mode)->tex0;
            tx0.TBP0 += offset;
            tx0.CBP += offset;

            fvec[0][0] = sdp[i].x;
            fvec[0][1] = sdp[i].y;
            fvec[0][2] = 0.0f;
            fvec[0][3] = 1.0f;

            fvec[1][0] = sdp[i].x + sdp[i].w;
            fvec[1][1] = sdp[i].y;
            fvec[1][2] = 0.0f;
            fvec[1][3] = 1.0f;

            fvec[2][0] = sdp[i].x;
            fvec[2][1] = sdp[i].y + sdp[i].h;
            fvec[2][2] = 0.0f;
            fvec[2][3] = 1.0f;

            fvec[3][0] = sdp[i].x + sdp[i].w;
            fvec[3][1] = sdp[i].y + sdp[i].h;
            fvec[3][2] = 0.0f;
            fvec[3][3] = 1.0f;

            sceVu0RotTransPersN(ivec, slm, fvec, 4, 0);

            k = 0;

            for (j = 0; j < 4; j++)
            {
                if (ivec[j][0] < 0x4000 || ivec[j][0] > 0xc000)
                {
                    k = 1;
                }

                if (ivec[j][1] < 0x5800 || ivec[j][1] > 0xa800)
                {
                    k = 1;
                }

                if (ivec[j][2] < 0xff   || ivec[j][2] > 0xffffff)
                {
                    k = 1;
                }
            }

            if (!k)
            {
                tx[0] = sdp[i].u * 16;
                ty[0] = sdp[i].v * 16;

                tx[1] = (sdp[i].u + sdp[i].w) * 16;
                ty[1] = sdp[i].v * 16;

                tx[2] = sdp[i].u * 16;
                ty[2] = (sdp[i].v + sdp[i].h) * 16;

                tx[3] = (sdp[i].u + sdp[i].w) * 16;
                ty[3] = (sdp[i].v + sdp[i].h) * 16;

                pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(6, SCE_GS_FALSE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
                pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

                pbuf[ndpkt].ul64[0] = 0;
                pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

                pbuf[ndpkt].ul64[0] = *(u_long*)&tx0;
                pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

                pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_NEAREST, 0, 0, 0);
                pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

                pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
                pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

                pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
                pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

                pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
                pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

                // prim = 348 is:
                // SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 1, 1, 0, 1, 0, 1, 0, 0)
                pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(one * 4, SCE_GS_FALSE, SCE_GS_TRUE, 348, SCE_GIF_PACKED, 3);
                pbuf[ndpkt++].ul64[1] = 0 \
                    | SCE_GS_RGBAQ << (0 * 4) \
                    | SCE_GS_UV    << (1 * 4) \
                    | SCE_GS_XYZF2 << (2 * 4);

                for (k = 0; k < one; k++)
                {
                    a = (k+1) % 2;

                    b = (k+1); // need to split this into two lines
                    b /= 2;    // to fix a stack diff

                    fm = k != 0 ? (a ? (float)b : -(float)b) * 3.0f : 0.0f;

                    fvec[0][0] = sdp[i].x + fm;
                    fvec[0][1] = sdp[i].y;
                    fvec[0][2] = 0.0f;
                    fvec[0][3] = 1.0f;

                    fvec[1][0] = sdp[i].x + sdp[i].w + fm;
                    fvec[1][1] = sdp[i].y;
                    fvec[1][2] = 0.0f;
                    fvec[1][3] = 1.0f;

                    fvec[2][0] = sdp[i].x + fm;
                    fvec[2][1] = sdp[i].y + sdp[i].h;
                    fvec[2][2] = 0.0f;
                    fvec[2][3] = 1.0f;

                    fvec[3][0] = sdp[i].x + sdp[i].w + fm;
                    fvec[3][1] = sdp[i].y + sdp[i].h;
                    fvec[3][2] = 0.0f;
                    fvec[3][3] = 1.0f;

                    sceVu0RotTransPersN(ivec, slm, fvec, 4, 0);

                    for (j = 0; j < 4; j++)
                    {
                        m = j > 1 ? 0 : 0x8000;

                        pbuf[ndpkt].ui32[0] = 0x80;
                        pbuf[ndpkt].ui32[1] = 0x80;
                        pbuf[ndpkt].ui32[2] = 0x80;
                        pbuf[ndpkt++].ui32[3] = alp / (b * 2 + 1);

                        pbuf[ndpkt].ui32[0] = tx[j];
                        pbuf[ndpkt].ui32[1] = ty[j];
                        pbuf[ndpkt].ui32[2] = 0;
                        pbuf[ndpkt++].ui32[3] = 0;

                        pbuf[ndpkt].ui32[0] = ivec[j][0];
                        pbuf[ndpkt].ui32[1] = ivec[j][1];
                        pbuf[ndpkt].ui32[2] = ivec[j][2];
                        pbuf[ndpkt++].ui32[3] = m;
                    }
                }
            }
        }

        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
        pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

        pbuf[ndpkt].ul64[0] = 0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

        pbuf[n].ui32[0] = DMAend + ndpkt - n - 1;
    }
}

void DrawRareEne()
{
    int i;
    int j;
    int n;
    int passflg[3];
    static int fl = 1;
    static float spd = 1.0f;
    static float rate = 0.4f;
    static float trate = 1.0f;
    static sceVu0FVECTOR cpos;
    static float alp[3];
    static void *efbuf[3];
    static int old_num[3];
    float fx;
    float fy;
    float fz;
    float l;
    int nene;
    RARE_ENE_DAT *redp;

    if (fl != 0)
    {
        for (i = 0; i < 3; i++)
        {
            alp[i] = 128.0f;
            efbuf[i] = 0;
            old_num[i] = -1;
        }

        fl = 0;
    }

    for (i = 0; i < 3; i++)
    {
        passflg[i] = 0;

        if (rg_dsp_wrk[i].mode != 0 && plyr_wrk.mode == 1)
        {
            for (j = 0, n = 1; j < 5 && n; j++)
            {

                if (rg_dsp_wrk[i].rg_no == area_wrk.rgst[j])
                {
                    Vu0CopyVector(cpos, rg_dsp_wrk[i].pos);

                    fx = cpos[0] - camera.p[0];
                    fy = cpos[1] - camera.p[1];
                    fz = cpos[2] - camera.p[2];

                    l = SgSqrtf(fx * fx + fy * fy + fz * fz);

                    cpos[0] = camera.p[0] + ((cpos[0] - camera.p[0]) * (l - 100.0f)) / l;
                    cpos[1] = camera.p[1] + ((cpos[1] - camera.p[1]) * (l - 100.0f)) / l;
                    cpos[2] = camera.p[2] + ((cpos[2] - camera.p[2]) * (l - 100.0f)) / l;

                    nene = rg_dsp_wrk[i].rg_no;
                    alp[i] = rg_dsp_wrk[i].alpha;

                    if (nene < 0x80)
                    {
                        redp = &rare_ene[nene];
                    }
                    else
                    {
                        redp = &((RARE_ENE_DAT *)&draw_mpri[902])[nene];
                    }

                    if (efbuf[i] != 0)
                    {
                        if (nene != old_num[i])
                        {
                            ResetEffects(efbuf[i]);

                            efbuf[i] = 0; // ??
                            efbuf[i] = SetEffects(0x1b, 2, 21, 0, redp->sclw * 10.0f, redp->sclh * 10.0f, cpos, 0, 0, 0, &alp[i], &spd, &rate, &trate);
                        }
                    }
                    else
                    {
                        efbuf[i] = SetEffects(0x1b, 2, 21, 0, redp->sclw * 10.0f, redp->sclh * 10.0f, cpos, 0, 0, 0, &alp[i], &spd, &rate, &trate);
                    }

                    DrawRareEne_Sub(j, i, rg_dsp_wrk[i].pos, rg_dsp_wrk[i].rg_no, rg_dsp_wrk[i].dsp_no, rg_dsp_wrk[i].alpha);

                    old_num[i] = nene;
                    passflg[i] = 1;

                    n = 0;
                }
            }
        }
    }

    for (i = 0; i < 3; i++)
    {
        if (passflg[i] == 0 && efbuf[i] != 0)
        {
            ResetEffects(efbuf[i]);

            efbuf[i] = 0;
        }
    }
}
