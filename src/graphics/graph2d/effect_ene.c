#include "common.h"
#include "typedefs.h"
#include "effect_ene.h"

#include "ee/eestruct.h"
#include "sce/libvu0.h"

#include "main/glob.h"
#include "os/eeiop/eese.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/adpcm/ea_gdead.h"
#include "ingame/plyr/unit_ctl.h"
#include "ingame/plyr/plyr_ctl.h"
#include "ingame/enemy/ene_ctl.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph2d/tim2_new.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_ene.h"
#include "graphics/graph2d/effect_scr.h"
#include "graphics/graph2d/effect_sub.h"
// #include "graphics/graph2d/effect_oth.h" // SetGlowOfAFirefly needs to be undeclared
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/libsg.h"
// #include "graphics/graph3d/gra3d.h" // RequestSpirit needs to be undeclared

#include "data/enedmg_fileno_tbl.h" // static int enedmg_fileno_tbl[][2];
#include "data/enedmg_texno_tbl.h"  // static SPRT_DAT enedmg_texno_tbl[];
#include "data/eto_light.h"         // static sceVu0FVECTOR eto_light[5];
#include "data/eto_rgb.h"           // static u_char eto_rgb[5][3];

static sceVu0FVECTOR reserve_lig;
static sceVu0FVECTOR spos[96];
static u_int enedmg_tex_addr[4];
static NEW_PERTICLE new_perticle[44];

#define PI 3.1415927f
#define PI2 6.2831855f

#define ADDRESS 0x01fa8000
#define ADDRESS_2 0x00ac8000

void InitEffectEne()
{
    int i;

    IniCamPush();
    IniCamStop();
    IniCamSlow();
    IniCamView();
    IniCamSearch();

    for (i = 0; i < 3; i++)
    {
        enedmg1[i].enedmg1_flg = 0;
    }

    enedmg2.enedmg2_flg = 0;

    enedmg_status = 0;

    eneseal_status = 0;

    sw_line.sw = 0;
    sw_line.num = 1;
    sw_line.top = 0;
}

void InitEffectEneEF()
{
    return;
}

void SetETOCircleTexure(sceVu0FMATRIX wlm, DRAW_ENV *de, float w, float h, u_char r, u_char g, u_char b, u_char a)
{
    int i;
    int bak;
    u_int clpx2;
    u_int clpy2;
    u_int clpz2;
    u_long tx0;
    sceVu0FMATRIX slm;
    sceVu0IVECTOR ivec[4];
    sceVu0FVECTOR ppos[4] = {
        {-1.0f, +1.0f, 0.0f, 1.0f},
        {+1.0f, +1.0f, 0.0f, 1.0f},
        {-1.0f, -1.0f, 0.0f, 1.0f},
        {+1.0f, -1.0f, 0.0f, 1.0f}
    };
    float stq[2] = {0.01f, 0.99f};
    U32DATA ts[4];
    U32DATA tt[4];
    U32DATA tq[4];

    clpx2 = 0xc000;
    clpy2 = 0xc000;
    clpz2 = 0x00ffffff;

    for (i = 0; i < 4; i++)
    {
        ppos[i][0] = i & 1 ? +w * 0.5f : -w * 0.5f;
        ppos[i][1] = i / 2 ? -h * 0.5f : +h * 0.5f;
        ppos[i][2] = 0.0f;
        ppos[i][3] = 1.0f;
    }

    sceVu0MulMatrix(slm, SgWSMtx, wlm);
    sceVu0RotTransPersN(ivec, slm, ppos, 4, 1);

    w = 0.0f;

    for (i = 0; i < 4; i++)
    {
        if (ivec[i][0] >= 0 && ivec[i][0] < 0x4000 || ivec[i][0] > clpx2)
        {
            w = 1.0f;
        }

        if (ivec[i][1] >= 0 && ivec[i][1] < 0x4000 || ivec[i][1] > clpy2)
        {
            w = 1.0f;
        }

        if (ivec[i][2] == 0 || ivec[i][2] > clpz2)
        {
            w = 1.0f;
        }

        tq[i].fl32 = 1.0f / ivec[i][3];
        ts[i].fl32 = stq[i % 2] * 192.0f * tq[i].fl32 / 256.0f;
        tt[i].fl32 = stq[i / 2] * 192.0f * tq[i].fl32 / 256.0f;
    }

    if (w == 0.0f)
    {
        tx0 = camdat[monochrome_mode + 12].tex0;

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

int SetCamFont(int no, int fl)
{
    static float scw[5][2];
    static float sch[5][2];
    static float alp[5][2];
    static int cnt[5][2];
    static int flow[5][2];
    int i;
    int j;
    int bak;
    u_int clpx2;
    u_int clpy2;
    u_int clpz2;
    int clip[2];
    u_int tw[2];
    u_int th[2];
    u_long tx[2];
    sceVu0FVECTOR ppos[2][4];
    sceVu0IVECTOR ivec[2][4];
    U32DATA ts[2][4];
    U32DATA tt[2][4];
    U32DATA tq[2][4];

    clpx2 = 0xfd00;
    clpy2 = 0xfd00;
    clpz2 = 0x00ffffff;

    if (fl != 0)
    {
        for (i = 0; i < 2; i++)
        {
            scw[no][i] = 0.0f;
            sch[no][i] = 0.0f;
            alp[no][i] = 0.0f;
            cnt[no][i] = 0;
            flow[no][i] = 0;
        }
    }

    if (plyr_wrk.mode != 0x1)
    {
        flow[no][0] = 0xff;
        flow[no][1] = 0xff;
    }

    if (flow[no][0] == 0xff && flow[no][1] == 0xff)
    {
        return 0xff;
    }

    SetSprFile2(ADDRESS,0);

    switch (flow[no][0])
    {
    case 0:
        scw[no][0] = sch[no][0] = 1.0f;
        alp[no][0] = 0.0f;

        if (cnt[no][0] >= 0)
        {
            alp[no][0] = 48.0f;
            flow[no][0]++;
            cnt[no][0] = 0;
        }
        else if (stop_effects == 0)
        {
            do { } while (0); // HACK: fixes stack reorder issue
            cnt[no][0]++;
        }
    break;
    case 1:
        scw[no][0] = sch[no][0] = (cnt[no][0] * 1.8f) / 10.0f + 1.0f;
        alp[no][0] = 64.0f - (cnt[no][0] * 48.0f) / 10.0f;

        if (cnt[no][0] >= 10)
        {
            cnt[no][0] = 0;
            flow[no][0] = 0xff;
            alp[no][0] = 0.0f;
        }
        else if (stop_effects == 0)
        {
            cnt[no][0]++;
        }
    break;
    case 0xff:
        // do nothing
    break;
    }

    switch (flow[no][1])
    {
    case 0:
        scw[no][1] = sch[no][1] = 2.5f;
        alp[no][1] = 0.0f;

        if (cnt[no][1] >= 0)
        {
            cnt[no][1] = 0;
            flow[no][1]++;
            alp[no][1] = 0.0f;
        }
        else if (stop_effects == 0)
        {
            cnt[no][1]++;
        }
    break;
    case 1:
        scw[no][1] = sch[no][1] = ((10 - cnt[no][1]) * 1.5f) / 10.0f + 1.0f;
        alp[no][1] = (cnt[no][1] * 48.0f) / 10.0f;

        if (cnt[no][1] >= 10)
        {
            cnt[no][1] = 0;
            flow[no][1]++;
            alp[no][1] = 48.0f;
        }
        else if (stop_effects == 0)
        {
            cnt[no][1]++;
        }
    break;
    case 2:
        scw[no][1] = sch[no][1] = cnt[no][1] * 0.1f * 0.25f + 1.0f;
        alp[no][1] = 48.0f;

        if (cnt[no][1] > 3)
        {
            cnt[no][1] = 0;
            flow[no][1]++;
            alp[no][1] = 48.0f;
        }
        else if (stop_effects == 0)
        {
            cnt[no][1]++;
        }
    break;
    case 3:
        scw[no][1] = sch[no][1] = 1.1f;
        alp[no][1] = 48.0f;

        if (cnt[no][1] > 9)
        {
            cnt[no][1] = 0;
            flow[no][1]++;
            alp[no][1] = 48.0f;
        }
        else if (stop_effects == 0)
        {
            cnt[no][1]++;
        }
    break;
    case 4:
        scw[no][1] = 1.1f - cnt[no][1] / 6.0f;
        sch[no][1] = (cnt[no][1] * 3.0f) / 6.0f + 1.1f;
        alp[no][1] = 48.0f;

        if (cnt[no][1] > 5)
        {
            cnt[no][1] = 0;
            flow[no][1]++;
            alp[no][1] = 48.0f;
        }
        else if (stop_effects == 0)
        {
            cnt[no][1]++;
        }
    break;
    case 5:
        sch[no][1] = 4.1f - (cnt[no][1] * 4.0f) / 6.0f;
        alp[no][1] = 48.0f - (cnt[no][1] * 48.0f) / 6.0f;

        if (cnt[no][1] > 5)
        {
            cnt[no][1] = 0;
            flow[no][1] = 0xff;
            alp[no][1] = 0.0f;
        }
        else if (stop_effects == 0)
        {
            cnt[no][1]++;
        }
    break;
    case 0xff:
        // do nothing
    break;
    }

    for (j = 0; j < 2; j++)
    {
        ivec[j][0][0] = ivec[j][2][0] = (2048 - (int)(scw[no][j] * 40)) * 16;
        ivec[j][1][0] = ivec[j][3][0] = ((int)(scw[no][j] * 40) + 2048) * 16;
        ivec[j][0][1] = ivec[j][1][1] = (2048 - (int)(sch[no][j] * 20)) * 16;
        ivec[j][2][1] = ivec[j][3][1] = ((int)(sch[no][j] * 20) + 2048) * 16;
        ivec[j][0][2] = ivec[j][1][2] = ivec[j][2][2] = ivec[j][3][2] = 0x00fffffe;
        ivec[j][0][3] = ivec[j][1][3] = ivec[j][2][3] = ivec[j][3][3] = 0x3f800000;

        tx[j] = camdat[no * 2].tex0;
        tw[j] = camdat[no * 2].w;
        th[j] = camdat[no * 2].h;

        clip[j] = 1;

        for (i = 0; i < 4; i++)
        {
            if ((ivec[j][i][0] >= 0 && ivec[j][i][0] < 0x300) || clpx2 < ivec[j][i][0])
            {
                clip[j] = 0;
            }

            if ((ivec[j][i][1] >= 0 && ivec[j][i][1] < 0x300) || clpy2 < ivec[j][i][1])
            {
                clip[j] = 0;
            }

            if (ivec[j][i][2] == 0 || clpz2 < ivec[j][i][2])
            {
                clip[j] = 0;
            }

            tq[j][i].fl32 = 1.0f / ivec[j][i][3];
            ts[j][i].fl32 = tw[j] * tq[j][i].fl32 / 64.0f;
            tt[j][i].fl32 = th[j] * tq[j][i].fl32 / 64.0f;
        }
    }

    Reserve2DPacket(0x1000);

    bak = ndpkt;

    pbuf[ndpkt++].ul128 = (u_long128)0;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX0_1(0x5, 2, SCE_GS_PSMCT32, 0, 0, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 16, 0);
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

    for (j = 0; j < 2; j++)
    {
        if (clip[j] != 0)
        {
            pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
            pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

            pbuf[ndpkt].ul64[0] = tx[j];
            pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

            pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 84, SCE_GIF_PACKED, 3);
            pbuf[ndpkt++].ul64[1] = 0 \
                | SCE_GS_ST    << (4 * 0)
                | SCE_GS_RGBAQ << (4 * 1)
                | SCE_GS_XYZF2 << (4 * 2);

            for (i = 0; i < 4; i++)
            {
                pbuf[ndpkt].ui32[0] = i & 1 ? ts[j][i].ui32 : 0;
                pbuf[ndpkt].ui32[1] = i / 2 ? tt[j][i].ui32 : 0;
                pbuf[ndpkt].ui32[2] = tq[j][i].ui32;
                pbuf[ndpkt++].ui32[3] = 0;

                pbuf[ndpkt].ui32[0] = 0x80;
                pbuf[ndpkt].ui32[1] = 0x80;
                pbuf[ndpkt].ui32[2] = 0x80;
                pbuf[ndpkt++].ui32[3] = alp[no][j];

                pbuf[ndpkt].ui32[0] = ivec[j][i][0];
                pbuf[ndpkt].ui32[1] = ivec[j][i][1];
                pbuf[ndpkt].ui32[2] = ivec[j][i][2] * 16;
                pbuf[ndpkt++].ui32[3] = i > 1 ? 0 : 0x8000;
            }
        }
    }

    pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;

    return (flow[no][0] == 0xff && flow[no][1] == 0xff) ? 0xff : 0;
}

int SetEFLight(int flg, int eneno, int ligno, int in, int keep, int out)
{
    static sceVu0FVECTOR lig1[3];
    static sceVu0FVECTOR lig2[3];
    static sceVu0FVECTOR lig3[3];
    static int flow[3] = {0xff, 0xff, 0xff};
    static int cnt[3];

    flow[eneno] = flg == 0 ? flow[eneno] : 0;

    switch (flow[eneno])
    {
    case 0:
        Vu0CopyVector(lig1[eneno], ene_wrk[eneno].mylight.parallel[1].diffuse);
        Vu0CopyVector(lig2[eneno], eto_light[ligno]);

        lig3[eneno][0] = lig1[eneno][0];
        lig3[eneno][1] = lig1[eneno][1];
        lig3[eneno][2] = lig1[eneno][2];

        flow[eneno] = 1;
        cnt[eneno] = 0;
    break;
    case 1:
        lig3[eneno][0] = ((lig2[eneno][0] - lig1[eneno][0]) * cnt[eneno]) / in + lig1[eneno][0];
        lig3[eneno][1] = ((lig2[eneno][1] - lig1[eneno][1]) * cnt[eneno]) / in + lig1[eneno][1];
        lig3[eneno][2] = ((lig2[eneno][2] - lig1[eneno][2]) * cnt[eneno]) / in + lig1[eneno][2];
        lig3[eneno][3] = ((lig2[eneno][3] - lig1[eneno][3]) * cnt[eneno]) / in + lig1[eneno][3];

        if (in <= ++cnt[eneno])
        {
            flow[eneno] = 2;
            cnt[eneno] = 0;
        }
    break;
    case 2:
        lig3[eneno][0] = lig2[eneno][0];
        lig3[eneno][1] = lig2[eneno][1];
        lig3[eneno][2] = lig2[eneno][2];
        lig3[eneno][3] = lig2[eneno][3];

        if (keep <= ++cnt[eneno])
        {
            cnt[eneno] = 0;
            flow[eneno] = 3;
        }
    break;
    case 3:
        lig3[eneno][0] = ((lig2[eneno][0] - lig1[eneno][0]) * (out - cnt[eneno])) / out + lig1[eneno][0];
        lig3[eneno][1] = ((lig2[eneno][1] - lig1[eneno][1]) * (out - cnt[eneno])) / out + lig1[eneno][1];
        lig3[eneno][2] = ((lig2[eneno][2] - lig1[eneno][2]) * (out - cnt[eneno])) / out + lig1[eneno][2];
        lig3[eneno][3] = ((lig2[eneno][3] - lig1[eneno][3]) * (out - cnt[eneno])) / out + lig1[eneno][3];

        if (out <= ++cnt[eneno])
        {
            cnt[eneno] = 0;
            flow[eneno]++;
        }
    break;
    case 4:
        lig3[eneno][0] = lig1[eneno][0];
        lig3[eneno][1] = lig1[eneno][1];
        lig3[eneno][2] = lig1[eneno][2];
        lig3[eneno][3] = lig1[eneno][3];
        flow[eneno] = 0xff;
    break;
    case 0xff:
        return 0xff;
    break;
    }

    Vu0CopyVector(ene_wrk[eneno].mylight.parallel[0].diffuse, lig3[eneno]);

    return flow[eneno];
}

ENDMG1 enedmg1[3] = {0};
ENDMG2 enedmg2 = {0};
SWORD_LINE sw_line = {0};
CAMSEARCH_STR camsearch[3] = {0};
CAMVIEW_STR camview[3] = {0};
CAMPUSH_STR campush[3] = {0};
CAMSLOW_STR camslow[3] = {0};
CAMSTOP_STR camstop[3] = {0};
sceVu0FVECTOR sword_line[2] = {0};
EDMG_TEX_WRK edmg_tex_wrk[10] = {0};
TAIL_DMG2_DAT enedmg2_tail[44] = {0};

void IniCamSearch()
{
    int i;

    for (i = 0; i < 3; i++)
    {
        camsearch[i].cam_search_flow = -1;
    }

    camsearch_miss = -1;
}

void SetCamSearch(int eneno)
{
    if (eneno == -1)
    {
        camsearch_miss = 0;
    }
    else
    {
        camsearch[eneno].cam_search_flow = 0;
    }
}

void RunCamSearch()
{
    int sam4rgb[3] = {0x49, 0x8A, 0xEA};
    int i;
    int j;
    int k;
    int n;
    int o;
    int tbl[30];
    float rot_x;
    float rot_y;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 84, SCE_GIF_PACKED, 3),
    };
    CAMSEARCH_STR *ct;
    // float *v0;
    int alp;
    // 0x50

    alp = 80;

    if (ingame_wrk.stts & 0x20)
    {
        return;
    }

    switch (camsearch_miss)
    {
    case 0:
        SetCamFont(4, 1);
        camsearch_miss = 0xff;
    break;
    case 0xff:
        if (SetCamFont(4, 0) == 0xff)
        {
            camsearch_miss = -1;
        }
    break;
    }

    j = 0;
    k = 0;
    // alp = 80;

    for (i = 0; i < 3; i++)
    {
        if (camsearch[i].cam_search_flow != -1 && plyr_wrk.mode == 1)
        {
            j++;
        }
        else
        {
            camsearch[i].cam_search_flow = -1;
        }

        if (camsearch[i].cam_search_flow == 0)
        {
            k++;
        }
    }

    if (j == 0)
    {
        return;
    }

    if (k != 0)
    {
        SetCamFont(4, 1);
    }

    SetSprFile2(ADDRESS, 0);

    for (o = 0; o < 3; o++)
    {
        ct = &camsearch[o];

        if (ct->cam_search_flow == -1)
        {
            continue;
        }

        if (ct->cam_search_flow == 0)
        {
            ct->top = 0;
            ct->num = 0;
            ct->walp = 0x0;
            ct->alp = 0.0f;
            ct->cnt = 0.0f;
            ct->cam_search_flow = 1;
        }

        n = ct->top;

        for (i = 0; i < 30; i++)
        {
            tbl[i] = n;
            n = n - 1 >= 0 ? n - 1 : 29;
        }

        Vu0CopyVector(ct->cpos[tbl[0]], camera.i);
        Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

        if (ct->num >= 2)
        {
            ct->alp = ct->alp + 0.02 < 1 ? ct->alp + 0.02f : 1.0f;
        }
        else
        {
            ct->alp = ct->alp - 0.04 > 0 ? ct->alp - 0.04f : 0.0f;
        }

        for (i = ct->num - 1; i > 0; i -= 2)
        {
            sceVu0UnitMatrix(wlm);
            sceVu0RotMatrixX(wlm, wlm, rot_x);
            sceVu0RotMatrixY(wlm, wlm, rot_y);
            sceVu0TransMatrix(wlm, wlm, ct->cpos[tbl[i]]);

            SetETOCircleTexure(
                wlm, &de,
                500.0f - i * 5.0f,
                500.0f - i * 5.0f,
                eto_rgb[4][0], eto_rgb[4][1], eto_rgb[4][2],
                (int)(((ct->num - i) * alp) / ct->num) * ct->alp);
        }

        sceVu0UnitMatrix(wlm);
        sceVu0RotMatrixX(wlm, wlm, rot_x);
        sceVu0RotMatrixY(wlm, wlm, rot_y);
        sceVu0TransMatrix(wlm, wlm, ct->cpos[tbl[0]]);

        SetETOCircleTexure(wlm, &de, 500.0f, 500.0f, eto_rgb[4][0], eto_rgb[4][1], eto_rgb[4][2], alp * ct->alp);

        sceVu0UnitMatrix(wlm);
        sceVu0RotMatrixZ(wlm, wlm, ct->cnt);
        sceVu0RotMatrixX(wlm, wlm, rot_x);
        sceVu0RotMatrixY(wlm, wlm, rot_y);
        sceVu0TransMatrix(wlm, wlm, ct->cpos[tbl[0]]);

        switch (ct->cam_search_flow)
        {
        case 0:
            // do nothing
        break;
        case 1:
            // do nothing
        break;
        case 10:
            ct->walp = ct->walp + 0x20 < 0x80 ? ct->walp + 0x20 : 0x80;

            for (i = 9; i >= 0; i--)
            {
                SetETOCircleTexure(
                    wlm, &de,
                    i * ct->cnt * 20.0f + 500.0f,
                    i * ct->cnt * 20.0f + 500.0f,
                    sam4rgb[0], sam4rgb[1], sam4rgb[2],
                    ((float)ct->walp - (int)(ct->walp * i / 10)));
            }

            ct->cnt = ct->cnt + (PI * 3 / 2) / 180.0f <= PI ? ct->cnt + (PI * 3 / 2) / 180.0f : ct->cnt + (PI * 3 / 2) / 180.0f - PI2;

            if (ct->walp >= 0x80)
            {
                ct->cam_search_flow++;
            }
        break;
        case 11:
            ct->walp = ct->walp - 4 >= 0 ? ct->walp - 4 : 0;

            for (i = 9; i >= 0; i--)
            {
                SetETOCircleTexure(
                    wlm, &de,
                    i * ct->cnt * 20.0f + 500.0f,
                    i * ct->cnt * 20.0f + 500.0f,
                    sam4rgb[0], sam4rgb[1], sam4rgb[2],
                    ((float)ct->walp - (int)(ct->walp * i / 10)));
            }

            ct->cnt = ct->cnt + (PI * 3 / 2) / 180.0f <= PI ? ct->cnt + (PI * 3 / 2) / 180.0f : ct->cnt + (PI * 3 / 2) / 180.0f - PI2;

            if (ct->walp == 0)
            {
                ct->cam_search_flow = 0xff;
            }
        break;
        }

        if (plyr_wrk.sta & 0x400)
        {
            ct->num = ct->num + 1 < 30 ? ct->num + 1 : 30;
            ct->top = (ct->top + 1) % 30;
        }
        else
        {
            ct->num = ct->num - 1 > 0 ? ct->num - 1 : 1;

            if (ct->cam_search_flow == 1)
            {
                ct->cam_search_flow = 10;
                SetEFLight(1, o, 4, 8, 4, 24);
            }
        }

        if (SetCamFont(4, 0) == 0xff && SetEFLight(0, o, 4, 8, 4, 24) == 0xff && ct->cam_search_flow == 0xff)
        {
            ct->cam_search_flow = -1;
        }
    }
}

u_char sr1 = 0x80;
u_char sg1 = 0x20;
u_char sb1 = 0x00;
u_char sa1 = 0x40;
u_char sr2 = 0x20;
u_char sg2 = 0x20;
u_char sb2 = 0x20;
u_char sa2 = 0x10;
int SWORDTYPE = 2;
float rac1 = 38.0f;
float rac2 = 50.0f;
float rbre = 0.85f;
u_char rrr = 0x50;
u_char ggg = 0x60;
u_char bbb = 0xff;

void IniCamView()
{
    int i;

    for (i = 0; i < 3; i++)
    {
        camview[i].cam_view_flow = -1;
    }

    camview_miss = -1;
}

void SetCamView(int eneno)
{
    if (eneno == -1)
    {
        camview_miss = 0;
    }
    else
    {
        camview[eneno].cam_view_flow = 10;
    }
}

void RunCamViewP(int o, int fl, float *cpos, float ap)
{
    int i;
    float rot_x;
    float rot_y;
    float fx;
    float fy;
    float fz;
    float l;
    sceVu0FVECTOR bpos;
    sceVu0FVECTOR wpos;
    sceVu0FVECTOR zero = {0.0f, 0.0f, 0.0f, 1.0f};
    sceVu0FMATRIX wlm;
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 0, 1, 0, 1, 0, 0, 0, 0), SCE_GIF_PACKED, 3),
    };
    CAMVIEW_STR *ct;

    ct = &camview[o];

    Vu0CopyVector(bpos, cpos);

    fx = (bpos[0] - camera.p[0]) * (bpos[0] - camera.p[0]);
    fy = (bpos[1] - camera.p[1]) * (bpos[1] - camera.p[1]);
    fz = (bpos[2] - camera.p[2]) * (bpos[2] - camera.p[2]);

    l = SgSqrtf(fx + fy + fz);

    bpos[0] = camera.p[0] + ((bpos[0] - camera.p[0]) * (l - 200.0f)) / l;
    bpos[1] = camera.p[1] + ((bpos[1] - camera.p[1]) * (l - 200.0f)) / l;
    bpos[2] = camera.p[2] + ((bpos[2] - camera.p[2]) * (l - 200.0f)) / l;

    if (fl != 0)
    {
        for (i = 0; i < 48; i++)
        {
            ct->rrad[i] = vu0Rand() * 300.0f;
            ct->rrot[i] = vu0Rand() * PI2 - PI;
            ct->racc[i] = vu0Rand() * 0.15f + 0.2f;
            ct->ralp[i] = 0;
        }
    }

    for (i = 0; i < 48; i++)
    {
        if (ct->rrad[i] - ct->racc[i] > 0.0f)
        {
            ct->rrad[i] -= ct->racc[i];
            ct->ralp[i] = ct->ralp[i] + 1 < 16 ? ct->ralp[i] + 1 : 16;
            ct->racc[i] *= 1.03f;
        }
        else
        {
            ct->rrad[i] = vu0Rand() * 150.0f + 150.0f;
            ct->rrot[i] = vu0Rand() * PI2 - PI;
            ct->racc[i] = vu0Rand() * 0.15f + 0.2f;
            ct->ralp[i] = 0;
        }
    }

    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

    for (i = 0; i < 48; i++)
    {
        zero[0] = ct->rrad[i];
        sceVu0UnitMatrix(wlm);
        sceVu0TransMatrix(wlm, wlm, zero);
        sceVu0RotMatrixZ(wlm, wlm, ct->rrot[i]);
        sceVu0RotMatrixX(wlm, wlm, rot_x);
        sceVu0RotMatrixY(wlm, wlm, rot_y);
        sceVu0TransMatrix(wlm, wlm, bpos);
        Set3DPosTexure(wlm, &de, 0x2e, ct->racc[i] * 5.0f + 10.0f, 10.0f, eto_rgb[2][0], eto_rgb[2][1], eto_rgb[2][2], ct->ralp[i] * ap);
    }
}

void RunCamView()
{
    int i;
    int j;
    int k;
    int o;
    float cv_fr1;
    float cv_fr2;
    float rot_r;
    float rot_x;
    float rot_y;
    sceVu0FVECTOR epos;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 84, SCE_GIF_PACKED, 3),
    };
    CAMVIEW_STR *ct;
    // float *v1;

    cv_fr1 = 60.0f;
    cv_fr2 = 30.0f;

    rot_r = PI / 180.0f;

    if (ingame_wrk.stts & 0x20)
    {
        return;
    }

    switch (camview_miss)
    {
    case 0:
        SetCamFont(2, 1);
        camview_miss = 0xff;
    break;
    case 0xff:
        if (SetCamFont(2, 0) == 0xff)
        {
            camview_miss = -1;
        }
    break;
    }

    j = 0;
    k = 0;

    for (i = 0; i < 3; i++)
    {
        if (camview[i].cam_view_flow != -1)
        {
            j++;
        }

        if (camview[i].cam_view_flow == 0 || camview[i].cam_view_flow == 10)
        {
            k++;
        }
    }

    if (j == 0)
    {
        return;
    }

    if (k != 0)
    {
        SetCamFont(2, 1);
    }

    SetSprFile2(ADDRESS, 0);

    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

    for (o = 0, j = 0; o < 3; o++)
    {
        ct = &camview[o];

        if (ct->cam_view_flow == -1)
        {
            continue;
        }

        Vu0CopyVector(epos, ene_wrk[o].mpos.p1);

        switch (ct->cam_view_flow)
        {
        case 0:
            ct->cnt = 0;
            ct->alpn = 0x80;
            ct->alpc = 0.0f;
            ct->cam_view_flow = 1;
            ct->rot_z = 0.0f;
        break;
        case 1:
            ct->alpn = ((cv_fr1 - ct->cnt) * 128.0f) / cv_fr1;

            if (++ct->cnt >= cv_fr1)
            {
                ct->cam_view_flow = 0xff;
            }
        break;
        case 10:
            ct->cnt = 0;
            ct->alpn = 0x80;
            ct->alpc = 0.0f;
            ct->rot_z = 0.0f;

            RunCamViewP(o, 1, epos, ct->alpc);

            if (ene_wrk[o].stm_view >= 33)
            {
                ct->ligtime[0] = 8;
                ct->ligtime[1] = ene_wrk[o].stm_view - 0x20;
                ct->ligtime[2] = 0x18;
            }
            else
            {
                ct->ligtime[0] = ene_wrk[o].stm_view / 2;
                ct->ligtime[1] = 0;
                ct->ligtime[2] = ene_wrk[o].stm_view / 2;
            }

            SetEFLight(1, o, 2, ct->ligtime[0], ct->ligtime[1], ct->ligtime[2]);

            ct->cam_view_flow = ct->cam_view_flow + 1;
        break;
        case 11:
            ct->alpc = ct->cnt / cv_fr1;
            ct->alpn = ((cv_fr1 - ct->cnt) * 112.0f) / cv_fr1 + 16.0f;

            sceVu0UnitMatrix(wlm);
            sceVu0RotMatrixZ(wlm, wlm, ct->rot_z);
            sceVu0RotMatrixX(wlm, wlm, rot_x);
            sceVu0RotMatrixY(wlm, wlm, rot_y);
            sceVu0TransMatrix(wlm, wlm, epos);

            SetETOCircleTexure(wlm, &de, 500.0f, 500.0f, eto_rgb[2][0], eto_rgb[2][1], eto_rgb[2][2], ct->alpc * 0x0a);
            RunCamViewP(o, 0, epos, ct->alpc);
            SetEFLight(0, o, 2, ct->ligtime[0], ct->ligtime[1], ct->ligtime[2]);

            ct->rot_z = ct->rot_z + rot_r < PI ? ct->rot_z + rot_r : ct->rot_z + rot_r - PI2;

            if (++ct->cnt >= cv_fr1)
            {
                ct->alpn = 0x10;
                ct->alpc = 1.0f;
                ct->cam_view_flow++;
            }
        break;
        case 12:
            sceVu0UnitMatrix(wlm);
            sceVu0RotMatrixZ(wlm, wlm, ct->rot_z);
            sceVu0RotMatrixX(wlm, wlm, rot_x);
            sceVu0RotMatrixY(wlm, wlm, rot_y);
            sceVu0TransMatrix(wlm, wlm, epos);

            SetETOCircleTexure(wlm, &de, 500.0f, 500.0f, eto_rgb[2][0], eto_rgb[2][1], eto_rgb[2][2], ct->alpc * 0x0a);
            RunCamViewP(o, 0, epos, ct->alpc);
            SetEFLight(0, o, 2, ct->ligtime[0], ct->ligtime[1], ct->ligtime[2]);

            ct->rot_z = ct->rot_z + rot_r < PI ? ct->rot_z + rot_r : ct->rot_z + rot_r - PI2;

            if (ene_wrk[o].stm_view <= cv_fr2)
            {
                ct->cnt = 0;
                ct->cam_view_flow++;
            }
        break;
        case 13:
            ct->alpc = (cv_fr2 - ct->cnt) / cv_fr2;
            ct->alpn = ((cv_fr2 - ct->cnt) * 16.0f) / cv_fr2;
            sceVu0UnitMatrix(wlm);
            sceVu0RotMatrixZ(wlm, wlm, ct->rot_z);
            sceVu0RotMatrixX(wlm, wlm, rot_x);
            sceVu0RotMatrixY(wlm, wlm, rot_y);
            sceVu0TransMatrix(wlm, wlm, epos);

            SetETOCircleTexure(wlm, &de, 500.0f, 500.0f, eto_rgb[2][0], eto_rgb[2][1], eto_rgb[2][2], ct->alpc * 0xa);
            RunCamViewP(o, 0, epos, ct->alpc);
            SetEFLight(0, o, 2, ct->ligtime[0], ct->ligtime[1], ct->ligtime[2]);

            ct->rot_z = ct->rot_z + rot_r < PI ? ct->rot_z + rot_r : ct->rot_z + rot_r - PI2;

            if (++ct->cnt >= cv_fr2)
            {
                ct->alpn = 0x0;
                ct->alpc = 0.0f;
                ct->cam_view_flow = 0xff;
            }
        break;
        case 0xfe:
            if (SetEFLight(0, o, 2, ct->ligtime[0], ct->ligtime[1], ct->ligtime[2]) == 0xff)
            {
                ct->cam_view_flow = 0xff;
            }
        break;
        }

        if (SetCamFont(2, 0) == 0xff && ct->cam_view_flow == 0xff)
        {
            ct->cam_view_flow = -1;
        }

        if (j < ct->alpn)
        {
            j = ct->alpn;
        }
    }

    SubNega(eto_rgb[2][0], eto_rgb[2][1], eto_rgb[2][2], 0x80, j);
}

void IniCamPush()
{
    int i;

    for (i = 0; i < 3; i++)
    {
        campush[i].cam_push_flow = -1;
    }

    campush_miss = -1;
}

void SetCamPush(int eneno)
{
    if (eneno == -1)
    {
        campush_miss = 0;
    }
    else
    {
        campush[eneno].cam_push_flow = 10;
    }
}

void RunCamPushP(int o, int fl, float *cpos, float ap)
{
    int i;
    int n;
    float rot_x;
    float rot_y;
    sceVu0FVECTOR bpos;
    sceVu0FVECTOR wpos;
    sceVu0FVECTOR zero = {0.0f, 0.0f, 0.0f, 1.0f};
    sceVu0FMATRIX wlm;
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 0, 1, 0, 1, 0, 0, 0, 0), SCE_GIF_PACKED, 3),
    };
    CAMPUSH_STR *ct;

    ct = &campush[o];

    Vu0CopyVector(bpos, cpos);

    if (fl != 0)
    {
        for (i = 0; i < 1; i++)
        {
            ct->rrad[i] = 0.0f;
            ct->rrot[i] = vu0Rand() * PI2 - PI;
            ct->racc[i] = vu0Rand() * 20.0f + 10.0f;
            ct->ralp[i] = ct->racc[i] < 1.0f ? ct->racc[i] : 1.0f;

            Vu0CopyVector(ct->rpos[i], bpos);
        }

        for (i = 1; i < 64; i++)
        {
            ct->rrad[i] = 0.0f;
            ct->rrot[i] = 0.0f;
            ct->racc[i] = 0.0f;
            ct->ralp[i] = 0.0f;
        }
    }

    for (i = 0, n = 0; i < 64; i++)
    {
        if (ct->racc[i] > 0.1f)
        {
            ct->rrad[i] += ct->racc[i];
            ct->racc[i] *= 0.9f;
            ct->ralp[i] = ct->racc[i] < 1.0f ? ct->racc[i] : 1.0f;
        }
        else
        {
            if (n < 1)
            {
                ct->rrad[i] = 0.0f;
                ct->rrot[i] = vu0Rand() * PI2 - PI;
                ct->racc[i] = vu0Rand() * 20.0f + 10.0f;
                ct->ralp[i] = ct->racc[i] < 1.0f ? ct->racc[i] : 1.0f;

                Vu0CopyVector(ct->rpos[i], bpos);

                n++;
            }
            else
            {
                ct->rrad[i] = 0.0f;
                ct->rrot[i] = 0.0f;
                ct->racc[i] = 0.0f;
                ct->ralp[i] = 0.0f;
            }
        }
    }

    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

    for (i = 0; i < 64; i++)
    {
        if (ct->ralp[i] > 0.0f)
        {
            zero[0] = ct->rrad[i];

            sceVu0UnitMatrix(wlm);
            sceVu0TransMatrix(wlm, wlm, zero);
            sceVu0RotMatrixZ(wlm, wlm, ct->rrot[i]);
            sceVu0RotMatrixX(wlm, wlm, rot_x);
            sceVu0RotMatrixY(wlm, wlm, rot_y);
            sceVu0TransMatrix(wlm, wlm, ct->rpos[i]);
            Set3DPosTexure(wlm, &de, 0x2e, 20.0f, 12.0f, eto_rgb[0][0], eto_rgb[0][1], eto_rgb[0][2], ct->ralp[i] * 64.0f * ap);
        }
    }
}

void RunCamPush()
{
    float cp_fr1;
    float cp_fr2;
    float cp_fr3;
    float cp_fr4;
    float rot_r;
    float rot_x;
    float rot_y;
    sceVu0FVECTOR epos;
    sceVu0FVECTOR wpos;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 84, SCE_GIF_PACKED, 3),
    };
    sceVu0FVECTOR ppos;
    sceVu0FVECTOR tpos;
    float fx;
    float fy;
    float fz;
    float l;
    float dist;
    int i;
    int j;
    int k;
    int o;
    CAMPUSH_STR *ct;

    cp_fr1 = 10.0f;
    cp_fr2 = 10.0f; // this is probably equal to cp_fr1 and that's why it has been optimized out
    cp_fr3 = 80.0f;
    cp_fr4 = 15.0f;

    rot_r = (PI * 6) / 180.0f;

    if (ingame_wrk.stts & 0x20)
    {
        return;
    }

    switch (campush_miss)
    {
    case 0:
        SetCamFont(0, 1);
        campush_miss = 0xff;
    break;
    case 0xff:
        if (SetCamFont(0, 0) == 0xff)
        {
            campush_miss = -1;
        }
    break;
    }

    j = 0;
    k = 0;

    for (i = 0; i < 3; i++)
    {
        if (campush[i].cam_push_flow != -1)
        {
            j++;
        }

        if (campush[i].cam_push_flow == 0 || campush[i].cam_push_flow == 10)
        {
            k++;
        }
    }

    if (j == 0)
    {
        return;
    }

    if (k != 0)
    {
        SetCamFont(0, 1);
    }

    SetSprFile2(ADDRESS, 0);

    for (o = 0; o < 3; o++)
    {
        ct = &campush[o];

        if (ct->cam_push_flow != -1)
        {
            Vu0CopyVector(ppos, camera.p);
            Vu0CopyVector(tpos, ene_wrk[o].mpos.p1);

            fx = (tpos[0] - ppos[0]) * (tpos[0] - ppos[0]);
            fy = (tpos[1] - ppos[1]) * (tpos[1] - ppos[1]);
            fz = (tpos[2] - ppos[2]) * (tpos[2] - ppos[2]);

            l = SgSqrtf(fx + fy + fz);

            Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

            switch (ct->cam_push_flow)
            {
            case 0:
                ct->cnt = 0;
                ct->size = 500.0f;
                ct->rot_z = 0.0f;
                ct->cam_push_flow = 1;
            case 1:
                dist = ct->cnt / cp_fr1;

                wpos[0] = ppos[0] + ((tpos[0] - ppos[0]) * (l * dist)) / l;
                wpos[1] = ppos[1] + ((tpos[1] - ppos[1]) * (l * dist)) / l;
                wpos[2] = ppos[2] + ((tpos[2] - ppos[2]) * (l * dist)) / l;

                sceVu0UnitMatrix(wlm);
                sceVu0RotMatrixZ(wlm, wlm, ct->rot_z);
                sceVu0RotMatrixX(wlm, wlm, rot_x);
                sceVu0RotMatrixY(wlm, wlm, rot_y);
                sceVu0TransMatrix(wlm, wlm, wpos);

                SetETOCircleTexure(
                    wlm, &de,
                    ct->size, ct->size,
                    eto_rgb[0][0], eto_rgb[0][1], eto_rgb[0][2], 0x40);

                ct->rot_z = ct->rot_z + rot_r < PI ? ct->rot_z + rot_r : ct->rot_z + rot_r - PI2;

                if (cp_fr1 <= ++ct->cnt)
                {
                    dist = 1.0f; // HACK: fixes a diff around a float value 1.0f not being correctly assigned to $f0
                    ct->alpc = 1.0f;
                    ct->cnt = 0;
                    ct->cam_push_flow++;
                }
            break;
            case 2:
                ct->alpc = (cp_fr2 - ct->cnt) / cp_fr2;

                dist = ct->cnt / cp_fr2 + 1.0f;

                wpos[0] = ppos[0] + ((tpos[0] - ppos[0]) * (l * dist)) / l;
                wpos[1] = ppos[1] + ((tpos[1] - ppos[1]) * (l * dist)) / l;
                wpos[2] = ppos[2] + ((tpos[2] - ppos[2]) * (l * dist)) / l;

                sceVu0UnitMatrix(wlm);
                sceVu0RotMatrixZ(wlm, wlm, ct->rot_z);
                sceVu0RotMatrixX(wlm, wlm, rot_x);
                sceVu0RotMatrixY(wlm, wlm, rot_y);
                sceVu0TransMatrix(wlm, wlm, wpos);

                SetETOCircleTexure(
                    wlm, &de,
                    ct->size, ct->size,
                    eto_rgb[0][0], eto_rgb[0][1], eto_rgb[0][2],
                    ct->alpc * 0x40);

                ct->rot_z = ct->rot_z + rot_r < PI ? ct->rot_z + rot_r : ct->rot_z + rot_r - PI2;

                if (cp_fr2 <= ++ct->cnt)
                {
                    ct->cam_push_flow = 0xff;
                }
            break;
            case 10:
                ct->cnt = 0;
                ct->size = 500.0f;
                ct->rot_z = 0.0f;

                SetEFLight(1, o, 0, 4, 0x58, 0x10);

                ct->cam_push_flow++;
            case 11:
                dist = ct->cnt / cp_fr1;

                wpos[0] = ppos[0] + ((tpos[0] - ppos[0]) * (l * dist)) / l;
                wpos[1] = ppos[1] + ((tpos[1] - ppos[1]) * (l * dist)) / l;
                wpos[2] = ppos[2] + ((tpos[2] - ppos[2]) * (l * dist)) / l;

                sceVu0UnitMatrix(wlm);
                sceVu0RotMatrixZ(wlm, wlm, ct->rot_z);
                sceVu0RotMatrixX(wlm, wlm, rot_x);
                sceVu0RotMatrixY(wlm, wlm, rot_y);
                sceVu0TransMatrix(wlm, wlm, wpos);

                SetETOCircleTexure(
                    wlm, &de,
                    ct->size, ct->size,
                    eto_rgb[0][0], eto_rgb[0][1], eto_rgb[0][2],
                    0x40);

                SetEFLight(0, o, 0, 4, 0x58, 0x10);

                ct->rot_z = ct->rot_z + rot_r < PI ? ct->rot_z + rot_r : ct->rot_z + rot_r - PI2;

                if (cp_fr1 <= ++ct->cnt)
                {
                    dist = 1.0f; // HACK: fixes a diff around a float value 1.0f not being correctly assigned to $f0
                    ct->alpc = 1.0f;
                    ct->cnt = 0;

                    RunCamPushP(o, 1, tpos, 1.0f);

                    ct->cam_push_flow++;
                }
            break;
            case 12:
                sceVu0UnitMatrix(wlm);
                sceVu0RotMatrixZ(wlm, wlm, ct->rot_z);
                sceVu0RotMatrixX(wlm, wlm, rot_x);
                sceVu0RotMatrixY(wlm, wlm, rot_y);
                sceVu0TransMatrix(wlm, wlm, tpos);

                for (i = 0; i < 4; i++)
                {
                    SetETOCircleTexure(
                        wlm, &de,
                        ct->size + i * 6.0f,
                        ct->size + i * 6.0f,
                        eto_rgb[0][0], eto_rgb[0][1], eto_rgb[0][2],
                        0x40);
                }

                RunCamPushP(o, 0, tpos, ct->alpc);
                SetEFLight(0, o, 0, 4, 0x58, 0x10);

                ct->rot_z = ct->rot_z + rot_r < PI ? ct->rot_z + rot_r : ct->rot_z + rot_r - PI2;
                ct->size -= 4.0f;

                if (cp_fr3 <= ++ct->cnt)
                {
                    ct->alpc = 1.0f;
                    ct->cnt = 0;

                    RunCamPushP(o, 0, tpos, 1.0f);

                    ct->cam_push_flow++;
                }
            break;
            case 13:
                ct->alpc = (cp_fr4 - ct->cnt) / cp_fr4;

                sceVu0UnitMatrix(wlm);
                sceVu0RotMatrixZ(wlm, wlm, ct->rot_z);
                sceVu0RotMatrixX(wlm, wlm, rot_x);
                sceVu0RotMatrixY(wlm, wlm, rot_y);
                sceVu0TransMatrix(wlm, wlm, tpos);

                for (i = 0; i < 4; i++)
                {
                    SetETOCircleTexure(
                        wlm, &de,
                        ct->size + i * 6.0f,
                        ct->size + i * 6.0f,
                        eto_rgb[0][0], eto_rgb[0][1], eto_rgb[0][2],
                        ct->alpc * 0x40);
                }

                RunCamPushP(o, 0, tpos, ct->alpc);
                SetEFLight(0, o, 0, 4, 0x58, 0x10);

                ct->rot_z = ct->rot_z + rot_r < PI ? ct->rot_z + rot_r : ct->rot_z + rot_r - PI2;
                ct->size -= 4.0f;

                if (cp_fr4 <= ++ct->cnt)
                {
                    ct->cam_push_flow = 0xfe;
                }
            break;
            case 0xfe:
                if (SetEFLight(0, o, 0, 4, 0x58, 0x10) == 0xff)
                {
                    ct->cam_push_flow = 0xff;
                }
            break;
            case 0xff:
                // do nothing
            break;
            }

            if (SetCamFont(0, 0) == 0xff && ct->cam_push_flow == 0xff)
            {
                ct->cam_push_flow = -1;
            }
        }
    }
}

void RunCamSlow2(int o, float hrt, float rrt, u_int alp)
{
    int i;
    int k;
    int n;
    int bak;
    float f;
    float rad;
    int clip[2][33];
    sceVu0IVECTOR nci[2][33];
    sceVu0FVECTOR tpos;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    u_int clpx2;
    u_int clpy2;
    u_int clpz2;
    CAMSLOW_STR *ct;
    float add;

    clpx2 = 0xfd00;
    clpy2 = 0xfd00;
    clpz2 = 0x00ffffff;

    ct = &camslow[o];

    if (rrt <= 0.1) // double !!
    {
        return;
    }

    rad = rrt * 225.0f;
    add = 360.0f / (33 - 1); // 11.25f;

    for (f = 0.0f, n = 0; f < 360.0f; f += add, n++)
    {
        ct->ncf[0][n][0] = rad * SgCosf((f * PI) / 180.0f);
        ct->ncf[0][n][1] = -60.0f;
        ct->ncf[0][n][2] = rad * SgSinf((f * PI) / 180.0f);
        ct->ncf[0][n][3] = 1.0f;

        ct->ncf[1][n][0] = rad * SgCosf((f * PI) / 180.0f);
        ct->ncf[1][n][1] = (vu0Rand() * -128.0f - 640.0f) * hrt - 60.0f;
        ct->ncf[1][n][2] = rad * SgSinf((f * PI) / 180.0f);
        ct->ncf[1][n][3] = 1.0f;
    }

    Vu0CopyVector(ct->ncf[0][n], ct->ncf[0][0]);
    Vu0CopyVector(ct->ncf[1][n], ct->ncf[1][0]);
    Vu0CopyVector(tpos, ene_wrk[o].mpos.p1);
    tpos[1] = ene_wrk[o].mpos.p4[1];

    sceVu0UnitMatrix(wlm);
    sceVu0TransMatrix(wlm, wlm, tpos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);
    sceVu0RotTransPersN(nci[0], slm, ct->ncf[0], 33, 0);
    sceVu0RotTransPersN(nci[1], slm, ct->ncf[1], 33, 0);

    for (i = 0; i < 33; i++)
    {
        clip[0][i] = 1;

        if ((nci[0][i][0] >= 0 && nci[0][i][0] < 0x300) || nci[0][i][0] > clpx2)
        {
            clip[0][i] = 0;
        }

        if ((nci[0][i][1] >= 0 && nci[0][i][1] < 0x300) || nci[0][i][1] > clpy2)
        {
            clip[0][i] = 0;
        }

        if ((nci[0][i][2] >= 0 && nci[0][i][2] < 0xf) || nci[0][i][2] > clpz2)
        {
            clip[0][i] = 0;
        }

        clip[1][i] = 1;

        if ((nci[1][i][0] >= 0 && nci[1][i][0] < 0x300) || nci[1][i][0] > clpx2)
        {
            clip[1][i] = 0;
        }

        if ((nci[1][i][1] >= 0 && nci[1][i][1] < 0x300) || nci[1][i][1] > clpy2)
        {
            clip[1][i] = 0;
        }

        if ((nci[1][i][2] >= 0 && nci[1][i][2] < 0xf) || nci[1][i][2] > clpz2)
        {
            clip[1][i] = 0;
        }
    }

    ct = &camslow[o]; // HACK to fix stack reordering (??)

    Reserve2DPacket(0x1000);

    bak = ndpkt;

    pbuf[ndpkt++].ul128 = (u_long128)0;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(3, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(33, SCE_GS_TRUE, SCE_GS_TRUE, 76, SCE_GIF_PACKED, 4);
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0)
        | SCE_GS_XYZF2 << (4 * 1)
        | SCE_GS_RGBAQ << (4 * 2)
        | SCE_GS_XYZF2 << (4 * 3);

    for (i = 0; i < 33; i++)
    {
        k = i != 0 ? clip[0][i-1] + clip[1][i-1] + clip[0][i] : 0;

        pbuf[ndpkt].ui32[0] = eto_rgb[0][0];
        pbuf[ndpkt].ui32[1] = eto_rgb[0][1];
        pbuf[ndpkt].ui32[2] = eto_rgb[0][2];
        pbuf[ndpkt++].ui32[3] = alp;

        pbuf[ndpkt].ui32[0] = nci[0][i][0];
        pbuf[ndpkt].ui32[1] = nci[0][i][1];
        pbuf[ndpkt].ui32[2] = nci[0][i][2];
        pbuf[ndpkt++].ui32[3] = k <= 2 ? 0x8000 : 0;

        k = i != 0 ? clip[1][i-1] + clip[0][i] + clip[1][i] : 0;

        pbuf[ndpkt].ui32[0] = eto_rgb[1][0];
        pbuf[ndpkt].ui32[1] = eto_rgb[1][1];
        pbuf[ndpkt].ui32[2] = eto_rgb[1][2];
        pbuf[ndpkt++].ui32[3] = 0;

        pbuf[ndpkt].ui32[0] = nci[1][i][0];
        pbuf[ndpkt].ui32[1] = nci[1][i][1];
        pbuf[ndpkt].ui32[2] = nci[1][i][2];
        pbuf[ndpkt++].ui32[3] = k <= 2 ? 0x8000 : 0;
    }

    pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;
}

void IniCamSlow()
{
    int i;

    for (i = 0; i < 3; i++)
    {
        camslow[i].cam_slow_flow = -1;
    }

    camslow_miss = -1;
}

void SetCamSlow(int eneno)
{
    if (eneno == -1)
    {
        camslow_miss = 0;
    }
    else
    {
        camslow[eneno].cam_slow_flow = 0;
    }
}

void RunCamSlow()
{
    int i;
    int j;
    int k;
    int o;
    u_int alp;
    u_int ralp[3];
    float size;
    float top;
    float bottom;
    float mvy;
    float ang1;
    float ang2;
    float angr;
    float rot_ry;
    float rot_rz;
    float rot_x;
    float rot_y;
    float f;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0FVECTOR tpos;
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 84, SCE_GIF_PACKED, 3),
    };
    CAMSLOW_STR *ct;

    size = 600.0f;
    top = -900.0f;
    bottom = -60.0f;
    mvy = 40.0f;
    ang1 = 66.0f;
    ang2 = 90.0f;
    angr = 0.4f;

    if (ang2)
    {
        // HACK: fixes stack reordering issue
    }

    rot_ry = (PI * 9) / 180.0f;

    if (ingame_wrk.stts & 0x20)
    {
        return;
    }

    switch (camslow_miss)
    {
    case 0:
        SetCamFont(1, 1);
        camslow_miss = 0xff;
    break;
    case 0xff:
        if (SetCamFont(1, 0) == 0xff)
        {
            camslow_miss = -1;
        }
    break;
    }

    j = 0;
    k = 0;

    for (i = 0; i < 3; i++)
    {
        if (camslow[i].cam_slow_flow != -1)
        {
            j++;
        }

        if (camslow[i].cam_slow_flow == 0)
        {
            k++;
        }
    }

    if (j == 0)
    {
        return;
    }

    if (k != 0)
    {
        SetCamFont(1, 1);
    }

    SetSprFile2(ADDRESS, 0);

    for (o = 0; o < 3; o++)
    {
        ct = &camslow[o];

        if (ct->cam_slow_flow == -1)
        {
            continue;
        }

        switch (ct->cam_slow_flow)
        {
        case 0:
            ct->fl = 1;
            ct->hrt = 0.0f;
            ct->rrt = 1.0f;

            ct->pos_y[0] = top;
            ct->pos_y[1] = top;
            ct->pos_y[2] = top;

            ct->rot_xx[0] = ang1;
            ct->rot_xx[1] = ang1;
            ct->rot_xx[2] = ang1;

            ralp[0] = 0x40;
            ralp[1] = 0;
            ralp[2] = 0;

            ct->cam_slow_flow++;
        case 1:
            ct->pos_y[0] = ct->pos_y[0] + mvy <= bottom ? ct->pos_y[0] + mvy : bottom;
            ct->rot_xx[0] = ct->rot_xx[0] + angr < ang2 ? ct->rot_xx[0] + angr : ang2;

            f = (ct->rot_xx[0] - ang1) / (ang2 - ang1);
            rot_rz = PI / (f * 10.0f + 10.0f);

            ct->rot_yy[0] = ct->rot_yy[0] + rot_ry < PI ? ct->rot_yy[0] + rot_ry : (ct->rot_yy[0] + rot_ry) - PI2;
            ct->rot_zz[0] = ct->rot_zz[0] + rot_rz < PI ? ct->rot_zz[0] + rot_rz : (ct->rot_zz[0] + rot_rz) - PI2;

            ralp[0] = 0x40;

            if (-640.0f <= ct->pos_y[0])
            {
                ct->pos_y[1] = ct->pos_y[1] + mvy <= bottom ? ct->pos_y[1] + mvy : bottom;
                ct->rot_xx[1] = ct->rot_xx[1] + angr < ang2 ? ct->rot_xx[1] + angr : ang2;

                f = (ct->rot_xx[1] - ang1) / (ang2 - ang1);
                rot_rz = PI / (f * 10.0f + 10.0f);

                ct->rot_yy[1] = ct->rot_yy[1] + rot_ry < PI ? ct->rot_yy[1] + rot_ry : (ct->rot_yy[1] + rot_ry) - PI2;
                ct->rot_zz[1] = ct->rot_zz[1] + rot_rz < PI ? ct->rot_zz[1] + rot_rz : (ct->rot_zz[1] + rot_rz) - PI2;

                ralp[1] = 0x40;
            }
            else
            {
                ralp[1] = 0;
            }

            if (-640.0f <= ct->pos_y[1])
            {
                ct->pos_y[2] = bottom >= ct->pos_y[2] + mvy ? ct->pos_y[2] + mvy : bottom;
                ct->rot_xx[2] = ang2 > ct->rot_xx[2] + angr ? ct->rot_xx[2] + angr : ang2;

                f = (ct->rot_xx[2] - ang1) / (ang2 - ang1);
                rot_rz = PI / (f * 10.0f + 10.0f);

                ct->rot_yy[2] = ct->rot_yy[2] + rot_ry < PI ? ct->rot_yy[2] + rot_ry : (ct->rot_yy[2] + rot_ry) - PI2;
                ct->rot_zz[2] = ct->rot_zz[2] + rot_rz < PI ? ct->rot_zz[2] + rot_rz : (ct->rot_zz[2] + rot_rz) - PI2;

                ralp[2] = 0x40;
            }
            else
            {
                ralp[2] = 0;
            }

            if (ang2 <= ct->rot_xx[2])
            {
                ct->cam_slow_flow = 0x14;
            }
        break;
        case 20:
            if (ene_wrk[o].stm_slow > 32)
            {
                ct->ligtime[0] = 8;
                ct->ligtime[1] = ene_wrk[o].stm_slow - 32;
                ct->ligtime[2] = 24;
            }
            else
            {
                ct->ligtime[0] = ene_wrk[o].stm_slow / 2;
                ct->ligtime[1] = 0;
                ct->ligtime[2] = ene_wrk[o].stm_slow / 2;
            }

            SetEFLight(1, o, 1, ct->ligtime[0], ct->ligtime[1], ct->ligtime[2]);

            ct->cam_slow_flow++;
        case 21:
            ct->hrt = ct->hrt + 0.1f < 1.0f ? ct->hrt + 0.1f : 1.0f;
            alp = (u_char)(ct->hrt * 32.0f);
            ralp[0] = 0x80;
            ralp[1] = 0x80;
            ralp[2] = 0x80;
            SetEFLight(0, o, 1, ct->ligtime[0], ct->ligtime[1], ct->ligtime[2]);
            if (1.0f <= ct->hrt)
            {
                ct->hrt = 1.0f;
                ct->cnt = 0;
                ct->cam_slow_flow++;
            }
        break;
        case 22:
            alp = 0x20;
            ralp[0] = 0x80;
            ralp[1] = 0x80;
            ralp[2] = 0x80;
            SetEFLight(0, o, 1, ct->ligtime[0], ct->ligtime[1], ct->ligtime[2]);
            if (++ct->cnt > 7)
            {
                ct->cam_slow_flow++;
            }
        break;
        case 23:
            ct->hrt = ct->hrt + 0.04f;
            ct->rrt = ct->rrt - 0.04f > 0.0f ? ct->rrt - 0.04f: 0.0f;

            alp = (u_char)(ct->hrt * 32.0f);

            ralp[0] = 0x80;
            ralp[1] = 0x80;
            ralp[2] = 0x80;

            SetEFLight(0, o, 1, ct->ligtime[0], ct->ligtime[1], ct->ligtime[2]);

            if (ct->hrt >= 2.0f && ct->rrt <= 0.0f)
            {
                ct->cam_slow_flow = 0xfe;
            }
        break;
        case 0xfe:
            if (SetEFLight(0, o, 1, ct->ligtime[0], ct->ligtime[1], ct->ligtime[2]) == 0xff)
            {
                ct->cam_slow_flow = 0xff;
            }
        break;
        case 0xff:
            // do nothing
        break;
        }

        for (i = 0; i < 3; i++)
        {
            Vu0CopyVector(tpos, ene_wrk[o].mpos.p1);
            tpos[1] = ene_wrk[o].mpos.p4[1] + ct->pos_y[i];

            sceVu0UnitMatrix(wlm);
            sceVu0RotMatrixZ(wlm, wlm, ct->rot_zz[i]);
            sceVu0RotMatrixX(wlm, wlm, (ct->rot_xx[i] * PI) / 180.0f);
            sceVu0RotMatrixY(wlm, wlm, ct->rot_yy[i]);
            sceVu0TransMatrix(wlm, wlm, tpos);

            SetETOCircleTexure(
                wlm, &de,
                (size + i * 6) * ct->rrt,
                (size + i * 6) * ct->rrt,
                eto_rgb[1][0], eto_rgb[1][1], eto_rgb[1][2],
                ralp[i]);
        }

        if (ct->cam_slow_flow >= 20 && ct->cam_slow_flow < 30)
        {
            sceVu0FVECTOR zero = {225.0f, 0.0f, 0.0f, 1.0f};
            sceVu0FVECTOR wpos;
            int n;

            zero[0] = ct->rrt * 225.0f;

            if (ct->fl != 0)
            {
                for (i = 1; i < 32; i++)
                {
                    ct->rrad[i] = 0.0f;
                    ct->rrot[i] = 0.0f;
                    ct->racc[i] = 0.0f;
                    ct->ralp[i] = 0.0f;
                }
                ct->fl = 0;
            }

            for (i = 0, n = 0; i < 32; i++)
            {
                if (ct->rlif[i] > 0.0f && ct->racc[i] > 0.1f)
                {
                    ct->rrad[i] += ct->racc[i];
                    ct->racc[i] *= 0.98f;
                    ct->rlif[i] -= 1.0f;
                    ct->ralp[i] = ct->rlif[i] < 10.0f ? ct->rlif[i] / 10.0f : 1.0f;
                }
                else
                {

                    if (n < 1)
                    {
                        ct->rrad[i] = 0.0f;
                        ct->rrot[i] = PI2 * vu0Rand() - PI;
                        ct->racc[i] = 20.0f * vu0Rand() + 15.0f;
                        ct->rlif[i] = 40.0f * vu0Rand() + 30.0f;
                        ct->ralp[i] = ct->rlif[i] < 10.0f ? ct->rlif[i] / 10.0f : 1.0f;
                        Vu0CopyVector(ct->rpos[i], tpos);
                        n++;
                    }
                    else
                    {
                        ct->rrad[i] = 0.0f;
                        ct->rrot[i] = 0.0f;
                        ct->racc[i] = 0.0f;
                        ct->ralp[i] = 0.0f;
                        ct->rlif[i] = 0.0f;
                    }
                }
            }

            Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

            for (i = 0; i < 32; i++)
            {
                if (ct->ralp[i] > 0.0f)
                {
                    zero[2] = ct->rrad[i];

                    sceVu0UnitMatrix(wlm);
                    sceVu0RotMatrixZ(wlm, wlm, ct->rrot[i]);
                    sceVu0RotMatrixX(wlm, wlm, PI / 2);
                    sceVu0TransMatrix(wlm, wlm, tpos);
                    sceVu0ApplyMatrix(wpos, wlm, zero);
                    sceVu0UnitMatrix(wlm);
                    sceVu0RotMatrixX(wlm, wlm, rot_x);
                    sceVu0RotMatrixY(wlm, wlm, rot_y);
                    sceVu0TransMatrix(wlm, wlm, wpos);

                    Set3DPosTexure(
                        wlm, &de, 0x2e,
                        10.0f, 40.0f,
                        eto_rgb[1][0], eto_rgb[1][1], eto_rgb[1][2],
                        ct->ralp[i] * 0x40);
                }
            }
        }

        RunCamSlow2(o, ct->hrt, ct->rrt, alp);

        if (SetCamFont(1, 0) == 0xff && ct->cam_slow_flow == 0xff)
        {
            ct->cam_slow_flow = -1;
        }
    }
}

void IniCamStop()
{
    int i;

    for (i = 0; i < 3; i++)
    {
        camstop[i].cam_stop_flow = -1;
    }

    camstop_miss = -1;
}

void SetCamStop(int eneno)
{
    if (eneno == -1)
    {
        camstop_miss = 0;
    }
    else
    {
        camstop[eneno].cam_stop_flow = 0;
    }
}

void RunCamStop()
{
    int i;
    int j;
    int k;
    int n;
    int o;
    sceVu0FVECTOR tpos;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 84, SCE_GIF_PACKED, 3),
    };
    CAMSTOP_STR *ct;
    // float *v1;
    float cs_fr1;
    float rot_rz;
    float ralp[32];
    float rot_x;
    float rot_y;

    if (ingame_wrk.stts & 0x20)
    {
        return;
    }

    switch (camstop_miss)
    {
    case 0:
        SetCamFont(3, 1);
        camstop_miss = 0xff;
    break;
    case 0xff:
        if (SetCamFont(3, 0) == 0xff)
        {
            camstop_miss = -1;
        }
    break;
    }

    j = 0;
    k = 0;

    for (i = 0; i < 3; i++)
    {
        if (camstop[i].cam_stop_flow != -1)
        {
            j++;
        }

        if (camstop[i].cam_stop_flow == 0)
        {
            k++;
        }
    }

    if (j == 0)
    {
        return;
    }

    if (k != 0)
    {
        SetCamFont(3, 1);
    }

    SetSprFile2(ADDRESS, 0);

    for (o = 0; o < 3; o++)
    {
        ct = &camstop[o];

        if (ct->cam_stop_flow == -1)
        {
            continue;
        }

        Vu0CopyVector(tpos, ene_wrk[o].mpos.p1);

        cs_fr1 = 24.0f;

        rot_rz = (PI * 2) / 180.0f;

        switch (ct->cam_stop_flow)
        {
        case 0:
            ct->rot_z = 0.0f;

            ct->size[0] = 0.0f;
            ct->size[1] = 6.0f;
            ct->size[2] = 12.0f;

            ct->alp[0] = 0x50;
            ct->alp[1] = 0x50;
            ct->alp[2] = 0x50;

            if (ene_wrk[o].stm_stop > 32)
            {
                ct->ligtime[0] = 8;
                ct->ligtime[1] = ene_wrk[o].stm_stop - 32;
                ct->ligtime[2] = 24;
            }
            else
            {
                ct->ligtime[0] = ene_wrk[o].stm_stop / 2;
                ct->ligtime[1] = 0;
                ct->ligtime[2] = ene_wrk[o].stm_stop / 2;
            }

            SetEFLight(1, o, 3, ct->ligtime[0], ct->ligtime[1], ct->ligtime[2]);

            ct->flow2 = 0;
            ct->cam_stop_flow++;
        case 1:
            ct->size[0] = 500.0f > ct->size[0] + 50.0f ? ct->size[0] + 50.0f : 500.0f;
            ct->size[1] = ct->size[0] + 6.0f;
            ct->size[2] = ct->size[1] + 6.0f;

            ct->alp[0] = 0x50;
            ct->alp[1] = 0x50;
            ct->alp[2] = 0x50;

            SetEFLight(0, o, 3, ct->ligtime[0], ct->ligtime[1], ct->ligtime[2]);

            if (ct->size[0] >= 500.0f)
            {
                ct->cam_stop_flow++;
            }
        break;
        case 2:
            ct->size[0] = 500.0f;
            ct->size[1] = 700.0f > ct->size[1] + 30.0f ? ct->size[1] + 30.0f : 700.0f;
            ct->size[2] = ct->size[1] + 6.0f;

            ct->alp[0] = 0x40;
            ct->alp[1] = (((700.0f - ct->size[1]) * 0x40) / 200.0f);
            ct->alp[2] = (((900.0f - (ct->size[1] + 6.0f)) * 0x40) / 400.0f);

            SetEFLight(0, o, 3, ct->ligtime[0], ct->ligtime[1], ct->ligtime[2]);

            if (ct->size[1] >= 700.0f)
            {
                ct->cam_stop_flow ++;
            }
        break;
        case 3:
            ct->size[0] = 500.0f;
            ct->size[1] = 700.0f;
            ct->size[2] = 900.0f > ct->size[2] + 30.0f ? ct->size[2] + 30.0f : 900.0f;

            ct->alp[0] = 0x40;
            ct->alp[1] = 0x0;
            ct->alp[2] = ((900.0f - ct->size[2]) * 0x40) / 400.0f;

            SetEFLight(0, o, 3, ct->ligtime[0], ct->ligtime[1], ct->ligtime[2]);

            if (900.0f <= ct->size[2])
            {
                ct->cnt = 0;
                ct->cam_stop_flow = 4;
            }
        break;
        case 4:
            ct->alp[0] = 0x40;
            ct->alp[1] = 0x0;
            ct->alp[2] = 0x0;

            SetEFLight(0, o, 3, ct->ligtime[0], ct->ligtime[1], ct->ligtime[2]);
            if (ene_wrk[o].stm_stop <= cs_fr1)
            {
                ct->cam_stop_flow = 5;
                ct->cnt = 0;
            }
        break;
        case 5:
            ct->size[0] = (ct->cnt * 400.0f) / cs_fr1 + 500.0f;

            ct->alp[0] = ((cs_fr1 - ct->cnt) * 0x40) / cs_fr1;
            ct->alp[1] = 0x0;
            ct->alp[2] = 0x0;

            SetEFLight(0, o, 3, ct->ligtime[0], ct->ligtime[1], ct->ligtime[2]);

            if (++ct->cnt >= 24.0f)
            {
                ct->cam_stop_flow = 0xfe;
            }
        break;
        case 0xfe:
            if (SetEFLight(0, o, 3, ct->ligtime[0], ct->ligtime[1], ct->ligtime[2]) == 0xff)
            {
                ct->cam_stop_flow = 0xff;
            }
        break;
        case 0xff:
            // do nothing
        break;
        }

        for (i = 0; i < 3; i++)
        {
            sceVu0UnitMatrix(wlm);
            sceVu0RotMatrixZ(wlm, wlm, ct->rot_z);
            sceVu0RotMatrixX(wlm, wlm, PI / 2);
            sceVu0TransMatrix(wlm, wlm, tpos);

            SetETOCircleTexure(
                wlm, &de,
                ct->size[i],
                ct->size[i],
                eto_rgb[3][0], eto_rgb[3][1], eto_rgb[3][2],
                ct->alp[i]);
        }

        ct->rot_z = ct->rot_z + rot_rz < PI ? ct->rot_z + rot_rz : ct->rot_z + rot_rz - PI2;

        if (1) // probably just a scope block
        {
            float rac1 = 12.0f;
            float rac2 = 30.0f;
            float rbre = 0.85f;
            sceVu0FVECTOR zero = {0.0f, 0.0f, 0.0f, 1.0f};
            sceVu0FVECTOR wpos;

            switch (ct->flow2)
            {
            case 0:
                for (i = 0; i < 32; i++)
                {
                    ct->rrad[i] = 0.0f;
                    ct->rrotx[i] = vu0Rand() * PI2 - PI;
                    ct->rroty[i] = vu0Rand() * PI2 - PI;
                    ct->racc[i] = rac1 * vu0Rand() + rac2;
                    ct->rscl[i] = 120.0f * vu0Rand() + 80.0f;
                    ct->rbrk[i] = 0.06f * vu0Rand() + rbre;
                    ralp[i] = 0.0f;
                }

                ct->flow2++;
            case 1:
                for (i = 0, n = 0; i < 32; i++)
                {
                    if (ct->racc[i] > 0.1f)
                    {
                        ct->rrad[i] += ct->racc[i];
                        ct->racc[i] *= ct->rbrk[i];
                        ralp[i] = ct->racc[i] < 1.0f ? ct->racc[i] : 1.0f;
                        n++;
                    }
                    else
                    {
                        ralp[i] = 0.0f;
                    }
                }

                if (n == 0)
                {
                    ct->flow2 = 0xff;
                }
            break;
            case 0xff:
                // do nothing
            break;
            }

            if (ct->flow2 == 1)
            {
                Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

                for (i = 0; i < 32; i++)
                {
                    if (ralp[i] > 0.0f)
                    {
                        zero[2] = ct->rrad[i];

                        sceVu0UnitMatrix(wlm);
                        sceVu0RotMatrixX(wlm, wlm, ct->rrotx[i]);
                        sceVu0RotMatrixY(wlm, wlm, ct->rroty[i]);
                        sceVu0TransMatrix(wlm, wlm, tpos);
                        sceVu0ApplyMatrix(wpos, wlm, zero);
                        sceVu0UnitMatrix(wlm);
                        sceVu0RotMatrixX(wlm, wlm, rot_x);
                        sceVu0RotMatrixY(wlm, wlm, rot_y);
                        sceVu0TransMatrix(wlm, wlm, wpos);

                        Set3DPosTexure(
                            wlm, &de, 0x2e,
                            ct->rscl[i] / 10.0f,
                            ct->rscl[i] / 10.0f,
                            eto_rgb[3][0], eto_rgb[3][1], eto_rgb[3][2],
                            ralp[i] * 0x50);
                    }
                }
            }
        }

        if (SetCamFont(3, 0) == 0xff && ct->cam_stop_flow == 0xff && ct->flow2 == 0xff)
        {
            ct->cam_stop_flow = -1;
        }
    }
}

int CheckCamSubEffect()
{
    int i;
    int n;

    n = 0;

    for (i = 0; i < 3; i++)
    {
        n += campush[i].cam_push_flow;
        n += camslow[i].cam_slow_flow;
        n += camview[i].cam_view_flow;
        n += camstop[i].cam_stop_flow;
        n += camsearch[i].cam_search_flow;
    }

    return n != -15;
}

void SetSwordLineSub(void *pos, int num, u_char r1, u_char g1, u_char b1, u_char r2, u_char g2, u_char b2)
{
    u_int clpx2;
    u_int clpy2;
    u_int clpz1;
    u_int clpz2;
    u_long reg;
    int i;
    int j;
    int k;
    int n;
    int alp1;
    int alp2;
    int cl;
    int tagnum;
    int cnt;
    sceVu0FVECTOR bpos;
    sceVu0IVECTOR ivec[96];
    int clip[96];
    u_long xyzf[96];
    u_long rgbaq1;
    u_long rgbaq2;
    int uu[96];
    int vv[96];
    u_long uv[96];
    int x = 0x27f8;
    int y = 0x0df8;

    clpx2 = 0xfe80;
    clpy2 = 0xfe80;
    clpz1 = 1;
    clpz2 = 0x00ffffff;

    if (SWORDTYPE == clpz1) // ???
    {
        LocalCopyLtoL((sys_wrk.count & 1) * 0x8c0, 0x1a40);
    }

    sceVu0RotTransPersN(ivec, SgWSMtx, pos, num * 2, 1);

    for (i = 0; i < num * 2; i++)
    {
        uu[i] = ivec[i][0] - 0x6bc0;
        vv[i] = ivec[i][1] - 0x78c0;

        uu[i] = uu[i] < 8 ? 8 : (uu[i] >= x ? 0x27f8 : uu[i]);
        vv[i] = vv[i] < 8 ? 8 : (vv[i] >= y ? 0x0df8 : vv[i]);

        uv[i] = SCE_GS_SET_UV(uu[i], vv[i]);
    }

    for (i = 0; i < num * 2; i++)
    {
        clip[i] = 1;

        if ((ivec[i][0] >= 0 && ivec[i][0] < 0x180) || ivec[i][0] > clpx2)
        {
            clip[i] = 0;
        }
        if ((ivec[i][1] >= 0 && ivec[i][1] < 0x180) || ivec[i][1] > clpy2)
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

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(7, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    if (SWORDTYPE == 1)
    {
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX0_1(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 0);
    }
    else
    {
        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX0_1((sys_wrk.count + 1 & 1) * 0x8c0, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 0);
    }
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_CLAMP_1(SCE_GS_CLAMP, SCE_GS_CLAMP, 0, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_CLAMP_1;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 0, SCE_GIF_REGLIST, 2);
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_PRIM << (4 * 0)
        | 0x0f        << (4 * 1);

    if (SWORDTYPE == 0)
    {
        pbuf[ndpkt].ul64[0] = (long)(0 \
            | SCE_GS_XYZF3 << (4 * 0)
            | SCE_GS_XYZF2 << (4 * 1)
            | SCE_GS_RGBAQ << (4 * 2)
        );
    }
    else
    {
        pbuf[ndpkt].ul64[0] = (long)(0 \
            | SCE_GS_XYZF3 << (4 * 0)
            | SCE_GS_XYZ2  << (4 * 1)
            | SCE_GS_RGBAQ << (4 * 2)
        );
    }
    pbuf[ndpkt++].ul64[1] = 0;

    tagnum = ndpkt;

    reg = 0;
    cnt = 0;

    ndpkt++;

    for (i = 0; i < num; i++)
    {
        if (SWORDTYPE == 0)
        {
            alp1 = (((num-i)-1) * sa1) / num;
            alp2 = (((num-i)-1) * sa2) / num;
        }
        else
        {
            alp1 = (((num-i)-1) * 0x80) / num;
            alp2 = (((num-i)-1) * 0x20) / num;
        }

        j = i * 2;
        k = j - 2;

        rgbaq1 = SCE_GS_SET_RGBAQ(r1, g1, b1, alp1, 0);
        rgbaq2 = SCE_GS_SET_RGBAQ(r2, g2, b2, alp2, 0);

        pbuf[ndpkt].ul64[0] = uv[j];
        pbuf[ndpkt++].ul64[1] = rgbaq1;

        pbuf[ndpkt].ul64[0] = xyzf[j];

        cl = i > 0 ? clip[k] + clip[j-1] + clip[j] : 0;

        if (cl >= 3)
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

        pbuf[ndpkt++].ul64[1] = uv[j+1];

        pbuf[ndpkt].ul64[0] = rgbaq2;
        pbuf[ndpkt++].ul64[1] = xyzf[j+1];

        cnt++;

        cl = i > 0 ? clip[k+1] + clip[j] + clip[j+1] : 0;

        if (cl >= 3)
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

            reg = 0;
            cnt = 0;

            tagnum = ndpkt;

            ndpkt++;
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

void SetSwordSwitch(int sw)
{
    sw_line.sw = sw;

    if (sw_line.sw != NULL)
    {
        sw_line.num = 1;
        sw_line.top = 0;
    }
}

void SetSwordLine()
{
    int i;
    int n;
    int tbl[48];
    sceVu0FMATRIX wlm;
    sceVu0FVECTOR bpos[2];
    sceVu0FVECTOR wpos = {0.0f ,0.0f, 300.0f, 1.0f};
    static sceVu0FVECTOR spos2[96];

    n = sw_line.top;

    for (i = 0; i < 48; i++)
    {
        tbl[i] = n;
        n = n - 1 >= 0 ? n - 1 : 47;
    }

    Vu0CopyVector(bpos[0], sword_line[0]);
    Vu0CopyVector(bpos[1], sword_line[1]);

    Vu0CopyVector(spos[tbl[0] * 2 + 0], bpos[1]);
    Vu0CopyVector(spos[tbl[0] * 2 + 1], bpos[0]);

    for (i = 0; i < sw_line.num; i++)
    {
        Vu0CopyVector(spos2[i * 2 + 0], spos[tbl[i] * 2 + 0]);
        Vu0CopyVector(spos2[i * 2 + 1], spos[tbl[i] * 2 + 1]);
    }

    if (sw_line.num > 1)
    {
        if (SWORDTYPE == 0)
        {
            SetSwordLineSub(spos2, sw_line.num, sr1, sg1, sb1, sr2, sg2, sb2);
        }
        else
        {
            SetSwordLineSub(spos2, sw_line.num, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
        }
    }

    if (sw_line.sw != 0)
    {
        sw_line.top = (sw_line.top + 1) % 48;
        sw_line.num = sw_line.num + 1 <= 48 ? sw_line.num + 1 : 48;
    }
    else
    {
        sw_line.top = (sw_line.top + 1) % 48;
        sw_line.num = sw_line.num - 1 >= 0 ? sw_line.num - 1 : 0;
    }
}

int SetSpiritAway(int fl, float *tpos)
{
    int i;
    int n;
    static SPAWAY spaway;
    SPAWAY *ct;
    DRAW_ENV de = {
        .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 0, 1, 0, 1, 0, 0, 0, 0), SCE_GIF_PACKED, 3),
    };
    float ralp[64];
    float rot_x;
    float rot_y;

    ct = &spaway;

    if (fl != 0)
    {
        ct->flow = 0;
    }

    if (ct->flow != 0xff)
    {
        sceVu0FVECTOR zero = {0.0f, 0.0f, 0.0f, 1.0f};
        sceVu0FVECTOR wpos;
        sceVu0FMATRIX wlm;

        switch (ct->flow)
        {
        case 0:
            for (i = 0; i < 64; i++)
            {
                ct->rrad[i] = 0.0f;
                ct->rrotx[i] = vu0Rand() * PI2 - PI;
                ct->rroty[i] = vu0Rand() * PI2 - PI;
                ct->racc[i] = rac1 * vu0Rand() + rac2;
                ct->rscl[i] = vu0Rand() * 250.0f + 60.0f;
                ct->rbrk[i] = vu0Rand() * 0.06f + rbre;

                ralp[i] = 0.0f;
            }

            ct->flow ++;
        case 1:
            for (i = 0, n = 0; i < 64; i++)
            {
                if (ct->racc[i] > 0.1f)
                {
                    if (stop_effects == 0)
                    {
                        ct->rrad[i] += ct->racc[i];
                        ct->racc[i] *= ct->rbrk[i];
                    }

                    ralp[i] = ct->racc[i] < 5.0f ? ct->racc[i] / 5.0f : 1.0f;

                    n++;
                }
                else
                {
                    ralp[i] = 0.0f;
                }
            }

            if (n == 0)
            {
                ct->flow = 0xff;
            }
        break;
        case 0xff:
            // do nothing
        break;
        }

        if (ct->flow == 1)
        {
            Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

            for (i = 0; i < 64; i++)
            {
                if (ralp[i] > 0.0f)
                {
                    zero[2] = ct->rrad[i];

                    sceVu0UnitMatrix(wlm);
                    sceVu0RotMatrixX(wlm, wlm, ct->rrotx[i]);
                    sceVu0RotMatrixY(wlm, wlm, ct->rroty[i]);
                    sceVu0TransMatrix(wlm, wlm, tpos);
                    sceVu0ApplyMatrix(wpos, wlm, zero);
                    sceVu0UnitMatrix(wlm);
                    sceVu0RotMatrixX(wlm, wlm, rot_x);
                    sceVu0RotMatrixY(wlm, wlm, rot_y);
                    sceVu0TransMatrix(wlm, wlm, wpos);

                    Set3DPosTexure(wlm, &de, 0x16, ct->rscl[i], ct->rscl[i], rrr, ggg, bbb, ralp[i] * 14.0f);
                    Set3DPosTexure(wlm, &de, 0x2e, ct->rscl[i] / 5.0f, ct->rscl[i] / 5.0f, rrr, ggg, bbb, ralp[i] * 80.0f);
                }
            }
        }

        return ct->flow;
    }

    return 0xff;
}

int SetNewEneOut(int flag, u_char eneno, u_char type, float *bpos, float sc)
{
    static float sscl[3];
    static float r[3];
    static int flow[3];
    float alpha1[289] = {
          0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   8.0f,   8.0f,   8.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,
          0.0f,   0.0f,   0.0f,   0.0f,   1.0f,  16.0f,  32.0f,  64.0f,  64.0f,  64.0f,  64.0f,  32.0f,  32.0f,  16.0f,   0.0f,   0.0f,   0.0f,
          0.0f,   0.0f,   0.0f,   1.0f,  32.0f,  64.0f,  96.0f, 128.0f, 128.0f, 128.0f,  96.0f,  96.0f,  64.0f,  32.0f,   8.0f,   0.0f,   0.0f,
          0.0f,   0.0f,   1.0f,  32.0f,  64.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f,  96.0f,  64.0f,  32.0f,  16.0f,   0.0f,
          0.0f,   1.0f,  32.0f,  64.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f,  96.0f,  64.0f,  32.0f,   0.0f,
          0.0f,  16.0f,  64.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f,  96.0f,  32.0f,   0.0f,
          1.0f,  32.0f,  96.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f,  96.0f,  64.0f,   1.0f,
          8.0f,  64.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f,  64.0f,   8.0f,
          8.0f,  64.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f,  64.0f,   8.0f,
          8.0f,  64.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f,  64.0f,   8.0f,
          1.0f,  64.0f,  96.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f,  96.0f,  32.0f,   1.0f,
          0.0f,  32.0f,  96.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f,  64.0f,  16.0f,   0.0f,
          0.0f,  32.0f,  64.0f,  96.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f,  64.0f,  32.0f,   1.0f,   0.0f,
          0.0f,  16.0f,  32.0f,  64.0f,  96.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f, 128.0f,  64.0f,  32.0f,   1.0f,   0.0f,   0.0f,
          0.0f,   0.0f,   8.0f,  32.0f,  64.0f,  96.0f,  96.0f, 128.0f, 128.0f, 128.0f,  96.0f,  64.0f,  32.0f,   1.0f,   0.0f,   0.0f,   0.0f,
          0.0f,   0.0f,   0.0f,  16.0f,  32.0f,  32.0f,  64.0f,  64.0f,  64.0f,  64.0f,  32.0f,  16.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,
          0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   8.0f,   8.0f,   8.0f,   1.0f,   0.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,
    };
    int i;
    int j;
    int k;
    int m;
    int bak;
    int wix;
    int vnumw;
    int vnumh;
    int pnumw;
    int pnumh;
    int wiy;
    register int sx2 asm("s6"); // HACK: fixes regswap
    int sy2;
    int clip;
    float l;
    float lw;
    float fw = 1.8f;
    float cntw;
    float cnth;
    float rad;
    float ss;
    float cc;
    float ts;
    float rot_x;
    float rot_y;
    float fa;
    float alp = 1.0f;
    float scl = sc * fw;
    float wfw;
    float wfh;
    float wff;
    float ml = 0.0f;
    int tx2[289];
    int ty2[289];
    sceVu0FVECTOR vpos;
    sceVu0FVECTOR pos = {0.0f, 0.0f, 0.0f, 1.0f};
    sceVu0FVECTOR fzero = {0.0f, 0.0f, 0.0f, 1.0f};
    sceVu0FVECTOR vt[289];
    sceVu0IVECTOR vtiw[289];
    sceVu0IVECTOR ivec;
    sceVu0FMATRIX wm; // unused
    sceVu0FMATRIX lrm; // unused
    sceVu0FMATRIX rm; // unused
    sceVu0FMATRIX swm; // unused
    sceVu0FMATRIX svm; // unused
    sceVu0FMATRIX slm;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX wvm; // unused

    if (flag == 1)
    {
        flow[eneno] = 0;
        LocalCopyLtoB2(0, (sys_wrk.count + 1 & 1) * 0x8c0);
        LocalCopyLtoL((sys_wrk.count + 1 & 1) * 0x8c0, (sys_wrk.count & 1) * 0x8c0);

        return 2;
    }

    switch (flow[eneno])
    {
    case 0:
        RequestSpirit(eneno, 0);

        sscl[eneno] = scl;
        r[eneno] = 0;
        flow[eneno]++;
    break;
    case 1:
        if (stop_effects == 0)
        {
            if (r[eneno] + 0.1f < 10.0f)
            {
                r[eneno] += 0.1f;
            }
            else if ((r[eneno] + r[eneno] / 100.0f) < 45.0f)
            {
                do { } while (0); // HACK: fixes stack order
                r[eneno] += r[eneno] / 100.0f;
            }
            else if (r[eneno] + r[eneno] / 100.0f * 1.8f < 83.0f)
            {
                r[eneno] += r[eneno] / 100.0f * 1.8f;
            }
            else if (r[eneno] + r[eneno] / 100.0f < 87.0f)
            {
                r[eneno] += r[eneno] / 100.0f;
            }
            else
            {
                r[eneno] += r[eneno] / 2000.0f;
            }
        }

        sscl[eneno] = ((90.0f - r[eneno]) * scl) / 90.0f;
        alp = (90.0f - r[eneno]) / 90.0f;

        if (90.0f <= r[eneno])
        {
            sscl[eneno] = 0.0f;
            r[eneno] = 90.0f;
            alp = sscl[eneno];
            flow[eneno]++;
        }
    break;
    case 2:
        flow[eneno] = 3;
    case 3:
        return 0xff;
    break;
    case 0xff:
        sscl[eneno] = scl;
        r[eneno] = 0;
    break;
    }

    if (flow[eneno] == 0xff)
    {
        return 1;
    }

    fa = r[eneno] < 80.0f ? 1.0f : (90.0f - r[eneno]) / 10.0f;

    if (sscl[eneno] == 0.0f)
    {
        return r[eneno] > 88.0f ? 3 : 2;
    }

    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

    Vu0CopyVector(vpos, bpos);
    vpos[1] -= 10.0f;

    sceVu0UnitMatrix(wlm);

    wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;

    sceVu0RotMatrixX(wlm, wlm, rot_x);
    sceVu0RotMatrixY(wlm, wlm, rot_y);
    sceVu0TransMatrix(wlm, wlm, vpos);
    sceVu0MulMatrix(slm, SgWSMtx, wlm);

    vnumw = 17;
    vnumh = 17;
    pnumw = 16;
    pnumh = 16;

    ts = sscl[eneno] * 2.0f;

    cntw = ((vnumw / 2) % vnumw) * ts;
    cnth = ((vnumh / 2) % vnumw) * ts;
    wff = ts * 8.0f;

    sx2 = 0x27f0;
    sy2 = 0xde0;

    for (i = 0; i < vnumw*vnumh; i++)
    {
        wix = i % vnumh;
        wiy = i / vnumh;

        vt[i][0] = wix * ts - wff;
        vt[i][1] = wiy * ts - wff;
        vt[i][2] = 0.0f;
        vt[i][3] = 1.0f;

        sceVu0RotTransPers(vtiw[i], slm, vt[i], 0);

        tx2[i] = vtiw[i][0] - 0x6bf8;
        ty2[i] = vtiw[i][1] - 0x78f8;

        tx2[i] = tx2[i] < 16 ? 16 : (tx2[i] > sx2 ? sx2 : tx2[i]);
        ty2[i] = ty2[i] < 16 ? 16 : (ty2[i] > sy2 ? sy2 : ty2[i]);
    }

    if (1)
    {
        float rr = r[eneno] * 1.2f;

        for (i = 0; i < vnumw*vnumh; i++)
        {
            wix = i % vnumh;
            wiy = i / vnumh;

            wfw = wix * ts - cntw;
            wfh = wiy * ts - cnth;

            lw = SgSqrtf(wfw * wfw + wfh * wfh);

            if (i == 0)
            {
                ml = lw;
            }

            l = (ml - lw) / sscl[eneno];
            l = l * l * 0.005f;

            if (wix != 0 && wix != pnumw && wiy != 0 && wiy != pnumw)
            {
                rad = (rr * l * PI) / 180.0f;

                ss = SgSinf(rad);
                cc = SgCosf(rad);

                vt[i][0] = wfw * cc - wfh * ss + cntw - wff;
                vt[i][1] = wfw * ss + wfh * cc + cnth - wff;
            }
            else
            {
                vt[i][0] = wix * ts - wff;
                vt[i][1] = wiy * ts - wff;
            }
        }
    }

    sceVu0RotTransPers(ivec, slm, fzero, 0);

    clip = 0;

    if (ivec[0] < 0x5800 || ivec[0] > 0xa800)
    {
        clip = 1;
    }

    if (ivec[1] < 0x6400 || ivec[1] > 0x9c00)
    {
        clip = 1;
    }

    if (ivec[2] < 0xff || ivec[2] > 0xffffff)
    {
        clip = 1;
    }

    if (clip == 0)
    {
        sceVu0RotTransPersN(vtiw, slm, vt, vnumw*vnumh, 0);

        if (stop_effects == 0)
        {
            LocalCopyBtoL(0, 0x1a40);
        }

        Reserve2DPacket(0x1000);

        bak = ndpkt;

        pbuf[ndpkt++].ul128 = (u_long128)0;

        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(7, SCE_GS_FALSE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
        pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

        pbuf[ndpkt].ul64[0] = 0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX0_1(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
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
        pbuf[ndpkt++].ul64[1] = 0 \
            | SCE_GS_RGBAQ << (4 * 0)
            | SCE_GS_UV    << (4 * 1)
            | SCE_GS_XYZF2 << (4 * 2)
            | SCE_GS_RGBAQ << (4 * 3)
            | SCE_GS_UV    << (4 * 4)
            | SCE_GS_XYZF2 << (4 * 5);

        for (i = 0; i < m; i++)
        {
            wix = i % vnumh;
            wiy = i / vnumh;

            j = i + vnumw;
            k = wix ? 0 : 0x8000;

            pbuf[ndpkt].ui32[0] = 0x80;
            pbuf[ndpkt].ui32[1] = 0x80;
            pbuf[ndpkt].ui32[2] = 0x80;
            pbuf[ndpkt++].ui32[3] = (u_int)(alpha1[i] * fa);

            pbuf[ndpkt].ui32[0] = tx2[i];
            pbuf[ndpkt].ui32[1] = ty2[i];
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = vtiw[i][0];
            pbuf[ndpkt].ui32[1] = vtiw[i][1];
            pbuf[ndpkt].ui32[2] = vtiw[i][2];
            pbuf[ndpkt++].ui32[3] = k;

            pbuf[ndpkt].ui32[0] = 0x80;
            pbuf[ndpkt].ui32[1] = 0x80;
            pbuf[ndpkt].ui32[2] = 0x80;
            pbuf[ndpkt++].ui32[3] = (u_int)(alpha1[j] * fa);

            pbuf[ndpkt].ui32[0] = tx2[j];
            pbuf[ndpkt].ui32[1] = ty2[j];
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = vtiw[j][0];
            pbuf[ndpkt].ui32[1] = vtiw[j][1];
            pbuf[ndpkt].ui32[2] = vtiw[j][2];
            pbuf[ndpkt++].ui32[3] = k;
        }

        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(2, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
        pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEX0_1((sys_wrk.count + 1 & 1) * 0x8c0, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

        pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
        pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(m, SCE_GS_TRUE, SCE_GS_TRUE, 348, SCE_GIF_PACKED, 6);
        pbuf[ndpkt++].ul64[1] = 0 \
            | SCE_GS_RGBAQ << (4 * 0)
            | SCE_GS_UV    << (4 * 1)
            | SCE_GS_XYZF2 << (4 * 2)
            | SCE_GS_RGBAQ << (4 * 3)
            | SCE_GS_UV    << (4 * 4)
            | SCE_GS_XYZF2 << (4 * 5);

        l = (1.0f - 0.9f) < alp ? (1.0f - alp) / 0.9f : 1.0f;

        for (i = 0; i < m; i++)
        {
            j = i + vnumh;
            k = i % (pnumw+1) > 0 ? 0 : 0x8000;

            pbuf[ndpkt].ui32[0] = 0xa0;
            pbuf[ndpkt].ui32[1] = 0xc0;
            pbuf[ndpkt].ui32[2] = 0xff;
            pbuf[ndpkt++].ui32[3] = (u_char)(alpha1[i] * l * fa * 0.5f);

            pbuf[ndpkt].ui32[0] = tx2[i];
            pbuf[ndpkt].ui32[1] = ty2[i];
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = vtiw[i][0];
            pbuf[ndpkt].ui32[1] = vtiw[i][1];
            pbuf[ndpkt].ui32[2] = vtiw[i][2];
            pbuf[ndpkt++].ui32[3] = k;

            pbuf[ndpkt].ui32[0] = 0xa0;
            pbuf[ndpkt].ui32[1] = 0xc0;
            pbuf[ndpkt].ui32[2] = 0xff;
            pbuf[ndpkt++].ui32[3] = (u_char)(alpha1[j] * l * fa * 0.5f);

            pbuf[ndpkt].ui32[0] = tx2[j];
            pbuf[ndpkt].ui32[1] = ty2[j];
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = vtiw[j][0];
            pbuf[ndpkt].ui32[1] = vtiw[j][1];
            pbuf[ndpkt].ui32[2] = vtiw[j][2];
            pbuf[ndpkt++].ui32[3] = k;
        }

        pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;
    }

    if (1)
    {
        float rot_x;
        float rot_y;
        sceVu0FMATRIX wlm;
        DRAW_ENV de = {
            .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
            .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
            .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
            .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL),
            .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
            .prim = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 84, SCE_GIF_PACKED, 3),
        };

        if (clip == 0)
        {
            Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

            sceVu0UnitMatrix(wlm);
            sceVu0RotMatrixX(wlm, wlm, rot_x);
            sceVu0RotMatrixY(wlm, wlm, rot_y);
            sceVu0TransMatrix(wlm, wlm, bpos);

            Set3DPosTexure(wlm, &de, 0x16, sscl[eneno] * 1000.0f, sscl[eneno] * 1000.0f, 0x50, 0x60, 0xff, l * 16.0f);
            Set3DPosTexure(wlm, &de, 0x16, sscl[eneno] *  600.0f, sscl[eneno] *  600.0f, 0x50, 0x60, 0xff, l * 24.0f);
            Set3DPosTexure(wlm, &de, 0x16, sscl[eneno] *  300.0f, sscl[eneno] *  300.0f, 0x50, 0x60, 0xff, l * 36.0f);
        }

        if (type != 0)
        {
            if (clip == 0)
            {
                fw = r[eneno] / 90.0f + 1.0f;
                Set3DPosTexure(wlm, &de, 0x16, sscl[eneno] * 250.0f * fw, sscl[eneno] * 250.0f * fw, 0x50, 0x60, 0xff, l *  16.0f);
                Set3DPosTexure(wlm, &de, 0x16, sscl[eneno] *  90.0f * fw, sscl[eneno] *  90.0f * fw, 0x50, 0x60, 0xff, l *  24.0f);
                Set3DPosTexure(wlm, &de, 0x16, sscl[eneno] *  50.0f * fw, sscl[eneno] *  50.0f * fw, 0x50, 0x60, 0xff, l * 110.0f);
            }

            return r[eneno] > 70.0f ? 3 : 2;
        }
        else
        {
            if (clip == 0)
            {
                fw = r[eneno] < 45.0f ? 0.0f : ((r[eneno] - 45.0f) * 1.2f) / 45.0f;
                l = 0.9f < alp ? (1.0f - alp) / 0.1f : 1.0f;

                Set3DPosTexure(wlm, &de, 0x16, fw * 250.0f, fw * 250.0f, 0x50, 0x60, 0xff, l *  16.0f);
                Set3DPosTexure(wlm, &de, 0x16, fw *  90.0f, fw *  90.0f, 0x50, 0x60, 0xff, l *  24.0f);
                Set3DPosTexure(wlm, &de, 0x16, fw *  50.0f, fw *  50.0f, 0x50, 0x60, 0xff, l * 110.0f);
            }

            return r[eneno] > 88.0f ? 3 : 2;
        }
    }
}

void SetEneDmgEffect3_Sub2(NEW_PERTICLE *np, float *bpos1, float *bpos2, u_char r1, u_char g1, u_char b1, u_char r2, u_char g2, int b2)
{
    int enedmg2_sp;
    float fx;
    float fy;
    float fz;
    float l;
    float f1;
    float f2;
    float rot_z;
    float r1l;
    float r2l;
    float r1r;
    float r2r;
    sceVu0FVECTOR bpos3;
    sceVu0FVECTOR opos1 = {0.0f, 100.0f, 0.0f, 1.0f};
    sceVu0FVECTOR rot;
    sceVu0FVECTOR pos;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;

    enedmg2_sp = 2;

    r1r = 1.0f;
    r2r = 2.0f;

    fx = (bpos2[0] - bpos1[0]) * (bpos2[0] - bpos1[0]);
    fy = (bpos2[1] - bpos1[1]) * (bpos2[1] - bpos1[1]);
    fz = (bpos2[2] - bpos1[2]) * (bpos2[2] - bpos1[2]);

    l = SgSqrtf(fx + fy + fz);

    r1l = (r1r * l) / (r1r + r2r);
    r2l = (r2r * l) / (r1r + r2r);

    if (stop_effects != 0)
    {
        return;
    }

    if (np->cnt < 180.0f)
    {
        // do nothing
    }
    else
    {
        np->num = np->num - 1 < 0 ? 0 : np->num - 1;
    }

    if (np->cnt < 90.0f)
    {
        f1 = 1.0f - SgSinf((np->cnt * PI) / 180.0f);
        f2 = SgSqrtf(1.0f - f1 * f1);

        np->x = f2 * r1l;
        np->y = SgSinf(f2 * PI * 0.5f) * np->n;
    }
    else
    {
        f1 = 1.0f - SgSinf((np->cnt * PI) / 180.0f);
        f2 = 1.0f - SgSqrtf(1.0f - f1 * f1);

        np->x = f2 * r2l + r1l;
        np->y = SgSinf((f2 + 1.0f) * PI * 0.5f) * np->n;
    }

    bpos3[0] = bpos1[0] + ((bpos2[0] - bpos1[0]) * np->x) / l;
    bpos3[1] = bpos1[1] + ((bpos2[1] - bpos1[1]) * np->x) / l;
    bpos3[2] = bpos1[2] + ((bpos2[2] - bpos1[2]) * np->x) / l;
    bpos3[3] = 1.0f;

    opos1[0] = 0.0f;
    opos1[1] = np->y;
    opos1[2] = 0.0f;
    opos1[3] = 1.0f;

    rot_z = np->rot + (np->rotp * np->x * 135.0f) / (r1l + r2l);

    while (rot_z < -PI) rot_z += PI2;
    while (PI < rot_z) rot_z -= PI2;

    sceVu0UnitMatrix(wlm);
    sceVu0RotMatrixZ(wlm, wlm, rot_z);
    sceVu0ApplyMatrix(opos1, wlm, opos1);
    GetTrgtRotType2(bpos1, bpos2, rot, 3);
    sceVu0UnitMatrix(wlm);
    sceVu0RotMatrixX(wlm, wlm, rot[0]);
    sceVu0RotMatrixY(wlm, wlm, rot[1]);
    sceVu0TransMatrix(wlm, wlm, bpos3);
    sceVu0ApplyMatrix(np->npos, wlm, opos1);

    if (np->cnt < 85.0f)
    {
        np->cnt += np->xp * 0.8f * enedmg2_sp;
    }
    else if (np->cnt < 90.0f)
    {
        np->cnt += np->xp * 15.0f * enedmg2_sp;
    }
    else if (np->cnt < 105.0f)
    {
        np->cnt += np->xp * 4.0f * enedmg2_sp;
    }
    else if (np->cnt < 120.0f)
    {
        np->cnt += np->xp * 2.0f * enedmg2_sp;
    }
    else if (np->cnt < 180.0f)
    {
        np->cnt += np->xp * 1.0f * enedmg2_sp;
        if (np->cnt >= 180.0f)
        {
            np->cnt = 180.0f; // Line ????
        }
    }
    else
    {
        np->cnt = 180.0f;
    }
}

int SetEneDmgEffect3(int flag, u_char eneno, sceVu0FVECTOR *pos)
{
    static sceVu0FVECTOR bpos1[3];
    static NEW_PERTICLE nyoro2_npt[3];
    u_char rgb1[8] = {0x80, 0xa0, 0xff, 0x48, 0x40, 0x60, 0x90, 0x80}; // rgb1
    u_char rgb2[8] = {0x80, 0xa0, 0xff, 0x00, 0x40, 0x60, 0x90, 0x00}; // rgb2
    int enedmg2_sp;
    int ret;
    int j;
    int k;
    float fx;
    float fy;
    float fz;
    float l;
    float f1;
    float f2;
    float rot_z;
    float r1l;
    float r2l;
    float r1r;
    float r2r;
    float nyo1;
    float nyo2;
    float nyo3;
    float nyo4;
    sceVu0FVECTOR bpos2;
    sceVu0FVECTOR opos1 = {0.0f, 100.0f, 0.0f, 1.0f};
    sceVu0FVECTOR rot;
    sceVu0FVECTOR wpos;
    sceVu0FVECTOR ppp = {-22.0f, -690.0f, 80.0f, 1.0f};
    sceVu0FVECTOR ppp2 = {-45.0f, 40.0f, 80.0f, 1.0f};
    sceVu0FVECTOR spos = {24000.0f, -200.0f, 6400.0f, 1.0f};
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    NEW_PERTICLE *np;

    enedmg2_sp = 2;
    ret = 0;

    r1r = 1.0f;
    r2r = 2.0f;

    nyo1 = 0.7853982f;
    nyo2 = 0.0f;
    nyo3 = 120.0f;
    nyo4 = 0.8f;

    if (flag == 1)
    {
        Vu0CopyVector(bpos1[eneno], pos[0]);
        ret = 2;
    }

    sceVu0UnitMatrix(wlm);

    if (plyr_wrk.mode != 1)
    {
        sceVu0RotMatrixX(wlm, wlm, plyr_wrk.move_box.rot[0]);
        sceVu0RotMatrixY(wlm, wlm, plyr_wrk.move_box.rot[1]);
        sceVu0TransMatrix(wlm, wlm, plyr_wrk.move_box.pos);
        sceVu0ApplyMatrix(bpos2, wlm, ppp);
    }
    else
    {
        GetTrgtRot(camera.p, camera.i, rot, 3);
        sceVu0RotMatrixX(wlm, wlm, rot[0]);
        sceVu0RotMatrixY(wlm, wlm, rot[1]);
        sceVu0TransMatrix(wlm, wlm, camera.p);
        sceVu0ApplyMatrix(bpos2, wlm, ppp2);
    }

    fx = (bpos2[0] - bpos1[eneno][0]) * (bpos2[0] - bpos1[eneno][0]);
    fy = (bpos2[1] - bpos1[eneno][1]) * (bpos2[1] - bpos1[eneno][1]);
    fz = (bpos2[2] - bpos1[eneno][2]) * (bpos2[2] - bpos1[eneno][2]);

    l = SgSqrtf(fx + fy + fz);

    r1l = (r1r * l) / (r1r + r2r);
    r2l = (r2r * l) / (r1r + r2r);

    if (flag == 1)
    {
        np = &nyoro2_npt[eneno];

        Vu0CopyVector(np->npos, bpos1[eneno]);

        np->rot = nyo1;
        np->rotp = nyo2;
        np->n = nyo3;
        np->xp = nyo4;
        np->cnt = 0.3f;

        np->top = 1;
        np->num = 1;

        f1 = 1.0f - SgSinf((np->cnt * PI) / 180.0f);
        f2 = SgSqrtf(1.0f - f1 * f1);

        np->x = f2 * r1l;
        np->y = SgSinf(f2 * PI * 0.5f) * np->n;

        wpos[0] = bpos1[eneno][0] + ((bpos2[0] - bpos1[eneno][0]) * np->x) / l;
        wpos[1] = bpos1[eneno][1] + ((bpos2[1] - bpos1[eneno][1]) * np->x) / l;
        wpos[2] = bpos1[eneno][2] + ((bpos2[2] - bpos1[eneno][2]) * np->x) / l;
        wpos[3] = 1.0f;

        opos1[0] = 0.0f;
        opos1[1] = np->y;
        opos1[2] = 0.0f;
        opos1[3] = 1.0f;

        rot_z = np->rot + (np->rotp * np->x * 135.0f) / (r1l + r2l);

        while (rot_z < -PI) rot_z += PI2;
        while (PI < rot_z) rot_z -= PI2;

        sceVu0UnitMatrix(wlm);
        sceVu0RotMatrixZ(wlm, wlm, rot_z);
        sceVu0ApplyMatrix(opos1, wlm, opos1);
        GetTrgtRotType2(bpos1[eneno], bpos2, rot, 3);
        sceVu0UnitMatrix(wlm);
        sceVu0RotMatrixX(wlm, wlm, rot[0]);
        sceVu0RotMatrixY(wlm, wlm, rot[1]);
        sceVu0TransMatrix(wlm, wlm, wpos);
        sceVu0ApplyMatrix(wpos, wlm, opos1);

        np->cnt += np->xp * 0.3f * enedmg2_sp;

        Vu0CopyVector(np->npos, wpos);
        Vu0CopyVector(pos[0], np->npos);
    }

    if (flag > 0)
    {
        np = &nyoro2_npt[eneno];

        k = -1;

        if (np->num > 0)
        {
            SetEneDmgEffect3_Sub2(np, bpos1[eneno], bpos2,  rgb2[0], rgb2[1], rgb2[2], rgb2[4], rgb2[5], rgb2[6]);

            Vu0CopyVector(pos[0], np->npos);

            k = 1;
        }

        j = 0;

        if (!(np->cnt < 2))
        {
            j = 1;
            if (!(np->cnt < 55.0f))
            {
                j = 3;
                if (np->cnt < 180.0f)
                {
                    j = 2;
                }
            }
        }

        if (j == 0)
        {
            ret = 2;
        }
        else if (flag == 2 && j == 1)
        {
            ret = 3;
        }
        else if (j == 1)
        {
            ret = 4;
        }
        else if (flag == 4 && j == 2)
        {
            ret = 5;
        }
        else if (j == 2)
        {
            ret = 6;
        }
        else if (j == 3 && k == 1)
        {
            ret = 7;
        }
        else if (k == 3 || j == 3) // HACK: fixes the last part of this if/else chain
        {
            ret = 0xff;
        }
        else
        {
            ret = 0xff;
        }
    }

    return ret;
}

float enedmg2_sp = 1.3f;
int eneseal_status = 0;
int enedmg_status = 0;
int camsearch_miss = 0;
int camview_miss = 0;
int campush_miss = 0;
int camslow_miss = 0;
int camstop_miss = 0;

void SetEneSeal(EFFECT_CONT *ec)
{
    static sceVu0FVECTOR bpos[3];
    u_char flag1;
    u_char flag2;
    u_char flag3;
    u_char eneno;
    u_char inifl;
    u_char eneseal_type;
    float scl;
    void *spr_fire;
    sceVu0IVECTOR ivec;
    sceVu0FVECTOR pos;
    sceVu0FVECTOR rot;
    sceVu0FVECTOR fzero = {0.0f, 0.0f, 0.0f, 1.0f};
    sceVu0FVECTOR spos = {25853.0f, -180.0f, 7560.0, 1.0f};
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;

    flag1 = ec->dat.uc8[2];
    flag2 = ec->dat.uc8[3];
    flag3 = ec->dat.uc8[4];

    eneseal_type = ec->dat.uc8[5];
    eneno = ec->dat.uc8[6];
    inifl = ec->dat.uc8[7];

    scl = ec->fw[2];

    spr_fire = ec->pnt[0];

    if (inifl == 0)
    {
        Vu0CopyVector(bpos[eneno], ene_wrk[eneno].mpos.p1);

        ec->fw[0] = 1.0f;
        ec->fw[1] = 0.0f;

        spr_fire = SetEffects(0x19, 2, 1, &bpos[eneno], &ec->fw[0], &ec->fw[1]);

        eff_filament_off = inifl = 1;

        ec->dat.uc8[2] = 1;
        ec->dat.uc8[3] = 0;
        ec->dat.uc8[4] = 0;
        ec->dat.uc8[5] = eneseal_type;
        ec->dat.uc8[6] = eneno;
        ec->dat.uc8[7] = inifl;
        ec->pnt[0] = spr_fire;
        ec->fw[2] = scl;
        return;
    }

    if (eneseal_type != 0)
    {
        if (flag1 != 0)
        {
            flag1 = SetNewEneOut(flag1, eneno, eneseal_type, bpos[eneno], scl);

            if (flag1 == 3)
            {
                flag2 = 1;
            }

            if (flag1 == 0xff)
            {
                if (ingame_wrk.game == 0 && ene_wrk[eneno].dat->dead_adpcm != -1)
                {
                    AdpcmPlayGhostDead(ene_wrk[eneno].dat->dead_adpcm, 0x3fff, 0x14);
                }

                flag1 = 0;
                flag3 = 1;
            }
        }

        if (flag2 != 0)
        {
            if (flag2 == 1)
            {
                if (flag3 < 0xff)
                {
                    if (stop_effects == 0)
                    {
                        ec->fw[1] = ec->fw[1] + 0.05f < 1 ? ec->fw[1] + 0.05f : 1.0f;
                    }
                }
                else
                {
                    if (ec->fw[1] >= 1.0f)
                    {
                        SeStartFix(7, 0, 0x1000, 0x1280, 0);
                        eneseal_status = 1;
                    }
                    else
                    {
                        eneseal_status = 0;
                    }

                    if (stop_effects == 0)
                    {
                        ec->fw[1] = ec->fw[1] - 0.02f > 0.0f ? ec->fw[1] - 0.02f : 0.0f;
                    }

                    if (ec->fw[1] == 0.0f)
                    {
                        flag2 = 2;
                        flag3 = 0;

                        ResetEffects(spr_fire);

                        spr_fire = NULL;
                    }
                }
            }
            else
            {
                if (flag2 != 10)
                {
                    if (flag2 > 10)
                    {
                        eneseal_status = 0;
                        flag2 = 0;

                        ResetEffects(ec);

                        ene_wrk[eneno].sta |=  0x40000;
                        eff_filament_off = 0;
                    }
                }
                flag2++;
            }
        }

        if (flag3 != 0)
        {
            flag3 = SetEneDmgEffect3(flag3, eneno, &bpos[eneno]);

            if (flag3 == 5)
            {
                Vu0CopyVector(pos, plyr_wrk.move_box.pos);

                if (plyr_wrk.move_mot == 11)
                {
                    pos[1] += -575.0f;
                }
                else
                {
                    pos[1] += -687.0f;
                }

                GetTrgtRot(pos, bpos[eneno], rot, 3);

                if (plyr_wrk.mode != 1)
                {
                    FinderInSet();
                }

                plyr_wrk.frot_x = rot[0];
                plyr_wrk.move_box.rot[1] = rot[1];
            }
        }
    }
    else
    {
        if (flag1 != 0)
        {
            flag1 = SetNewEneOut(flag1, eneno, 0x0, bpos[eneno], scl);

            if (flag1 == 3)
            {
                flag1 = 0;
                flag3 = 1;

                SeStartFix(0x1d, 0, 0x1000, 0x1000, 0);
                SetSpiritAway(1, bpos[eneno]);
                ResetEffects(spr_fire);

                spr_fire = NULL;
            }
        }

        if (flag2 != 0 && stop_effects == 0)
        {
            if (flag3 < 0xff)
            {
                ec->fw[1] = ec->fw[1] + 0.05f < 1.0f ? ec->fw[1] + 0.05f : 1.0f;
            }
            else
            {
                ec->fw[1] = ec->fw[1] - 0.02f > 0.0f ? ec->fw[1] - 0.02f : 0.0f;
            }
        }

        if (flag3 != 0)
        {
            flag3 = SetSpiritAway(0, bpos[eneno]);

            if (flag3 == 0xff)
            {
                flag2 = 0;
                flag3 = 0;
                ene_wrk[eneno].sta |= 0x40000;

                ResetEffects(ec);

                eff_filament_off = 0;
            }
        }
    }

    ec->dat.uc8[2] = flag1;
    ec->dat.uc8[3] = flag2;
    ec->dat.uc8[4] = flag3;

    ec->dat.uc8[5] = eneseal_type;
    ec->dat.uc8[6] = eneno;
    ec->dat.uc8[7] = inifl;

    ec->pnt[0] = spr_fire;
    ec->fw[2] = scl;
}

void EneDmgTexInit()
{
    int i;

    for (i = 0; i < 10; i++)
    {
        edmg_tex_wrk[i].mdl_no = 0xff;
    }
}

void LoadEneDmgTex(int no, u_int *addr)
{
    int i;
    int count;
    int texno;

    if (no == 55)
    {
        no = 42;
    }

    texno = enedmg_fileno_tbl[no][0];

    count = 0;

    if (texno < 0)
    {
        return;
    }

    LoadReq(texno, (u_int)addr);

    for (i = 0; i < 10; i++)
    {
        if (edmg_tex_wrk[i].mdl_no == 0xff)
        {
            edmg_tex_wrk[i].tex_no = texno;
            edmg_tex_wrk[i].mdl_no = no + count;
            edmg_tex_wrk[i].addr = (int)addr;

            if (no != 42)
            {
                return;
            }

            count++;

            if (count == 3)
            {
                return;
            }
        }
    }
}

void SetEneDmgTex(int wrk_no)
{
    int i;

    for (i = 0; i < 10; i++)
    {
        switch (ene_wrk[wrk_no].type)
        {
        case 0:
            if (edmg_tex_wrk[i].mdl_no == jene_dat[ingame_wrk.msn_no][ene_wrk[wrk_no].dat_no].mdl_no)
            {
                enedmg_tex_addr[wrk_no] = edmg_tex_wrk[i].addr;
                return;
            }
        break;
        case 1:
            if (edmg_tex_wrk[i].mdl_no == fene_dat[ingame_wrk.msn_no][ene_wrk[wrk_no].dat_no].mdl_no)
            {
                enedmg_tex_addr[wrk_no] = edmg_tex_wrk[i].addr;
                return;
            }
        break;
        }
    }
}

void EneDmgTexDel(int wrk_no)
{
    int i;

    for (i = 0; i < 10; i++)
    {
        switch (ene_wrk[wrk_no].type)
        {
        case 0:
            if (edmg_tex_wrk[i].mdl_no == jene_dat[ingame_wrk.msn_no][ene_wrk[wrk_no].dat_no].mdl_no)
            {
                edmg_tex_wrk[i].mdl_no = 0xff;
                return;
            }
        break;
        case 1:
            if (edmg_tex_wrk[i].mdl_no == fene_dat[ingame_wrk.msn_no][ene_wrk[wrk_no].dat_no].mdl_no)
            {
                edmg_tex_wrk[i].mdl_no = 0xff;
                return;
            }
        break;
        }
    }
}

void ZeroHourEneDmgTexDel(int mdl_no)
{
    int i;

    for (i = 0; i < 10; i++)
    {
        if (edmg_tex_wrk[i].mdl_no == mdl_no)
        {
            edmg_tex_wrk[i].mdl_no = 0xff;

            return;
        }
    }
}

void SetEneDmgEffect1_Sub2(int num)
{
    u_char rgb[2][2][2][3] = {
        {
            {{0x80, 0x80, 0x80}, {0xff, 0x20, 0x20}},
            {{0x49, 0x8a, 0xea}, {0xff, 0x50, 0x30}},
        },
        {
            {{0x80, 0x80, 0x80}, {0x6a, 0x6a, 0x6a}},
            {{0x94, 0x94, 0x94}, {0x80, 0x80, 0x80}},
        },
    };
    sceVu0FVECTOR bpos1 = {24000.0f, -200.0f, 6400.0f, 1.0f};
    sceVu0FVECTOR wpos[4];
    sceVu0FVECTOR bpos;
    sceVu0FVECTOR cpos;
    int i;
    int j;
    int n;
    int st;
    int bak;
    float rot_x;
    float rot_y;
    float fx;
    float fy;
    float fz;
    float l;
    float scmax;
    float scl[4];
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    int mono;
    int clip[4];
    u_int tw[4];
    u_int th[4];
    u_long tex0[4];
    sceVu0FVECTOR ppos[4][4];
    sceVu0IVECTOR ivec[4][4];
    U32DATA ts[4][4];
    U32DATA tt[4][4];
    U32DATA tq[4][4];
    float dist[4] = {100.0f, -200.0f, -450.0f, -460.0f};
    float bww[4] = {450.0f, 16.0f, 80.0f, 80.0f};
    float bhh[4] = {450.0f, 16.0f, 80.0f, 80.0f};
    float szw[4] = {256.0f, 256.0f, 128.0f, 128.0f};
    float szh[4] = {256.0f, 256.0f, 128.0f, 128.0f};
    u_int textbl[4] = {0, 12, 0, 0};
    u_int clpx1; // HACK: fixes stack order
    u_int clpx2;
    u_int clpy1; // HACK: fixes stack order
    u_int clpy2;
    u_int clpz1;
    u_int clpz2;
    int two; // HACK: fixes stack order
    ENDMG1 *dmg1;
    // float *v0;
    // float *v1;
    // float *v0;
    // float *v1;

    clpz1 = 1; // ???

    clpx2 = 0xfd00;
    clpy2 = 0xfd00;
    clpz2 = 0xffffff;

    if (ingame_wrk.stts & 0x20)
    {
        return;
    }

    dmg1 = &enedmg1[num];

    if (dmg1->enedmg1_flg == 0)
    {
        return;
    }

    mono = monochrome_mode;
    monochrome_mode = 0;

    if (dmg1->enedmg1_flg == clpz1) // ???
    {
        Vu0CopyVector(dmg1->wbpos[0], ene_wrk[dmg1->enedmg_no].mpos.p1);
        Vu0CopyVector(dmg1->wbpos[1], ene_wrk[dmg1->enedmg_no].mpos.p0);

        for (i = 0; i < 2; i++)
        {
            dmg1->scw[i] = 0.0f;
            dmg1->sch[i] = 0.0f;
            dmg1->alp[i] = 0.0f;
            dmg1->flow[i] = 0;
            dmg1->cnt[i] = 0;
            dmg1->rot_z[0] = 0.0f;
        }

        enedmg_status = 0;
        dmg1->enedmg1_flg = 2;
    }

    n = ene_wrk[dmg1->enedmg_no].dmg_old;
    n = n < photo_dmg_tbl[5] ? photo_dmg_tbl[5] : (photo_dmg_tbl[14] < n ? photo_dmg_tbl[14] : n);

    scmax = ((float)(n - photo_dmg_tbl[5]) / (float)(photo_dmg_tbl[14] - photo_dmg_tbl[5])) + 0.6f;

    switch (dmg1->flow[0])
    {
    case 0:
        dmg1->scw[0] = dmg1->sch[0] = 1.0f;
        dmg1->alp[0] = 0.0f;
        if (dmg1->cnt[0] >= 0xf)
        {
            dmg1->flow[0]++;
            dmg1->alp[0] = 80.0f;
            dmg1->cnt[0] = 0;
            SetEneDmgEffect2();
        }
        else if (stop_effects == 0)
        {
            dmg1->cnt[0]++;
        }
    break;
    case 1:
        dmg1->scw[0] = ((dmg1->cnt[0] * scmax) * 0.5f) / 40.0f + 1.0f;
        dmg1->sch[0] = (dmg1->cnt[0] * scmax) / 40.0f + 1.0f;
        dmg1->alp[0] = 80.0f - (dmg1->cnt[0] * 80.0f) / 40.0f;
        if (dmg1->cnt[0] >= 0x28)
        {
            dmg1->flow[0]++;
            dmg1->cnt[0] = 0;
            dmg1->alp[0] = 0.0f;
        }
        else if (stop_effects == 0)
        {
            dmg1->cnt[0]++;
        }
    break;
    case 0xff:
        // do nothing
    break;
    }

    switch (dmg1->flow[1])
    {
    case 0:
        dmg1->scw[1] = dmg1->sch[1] = 1.0f;
        dmg1->alp[1] = 0.0f;
        if (0xe < dmg1->cnt[1])
        {
            dmg1->flow[1]++;
            dmg1->alp[1] = 64.0f;
            dmg1->cnt[1] = 0;
        }
        else if (stop_effects == 0)
        {
            dmg1->cnt[1]++;
        }
    break;
    case 1:
        scmax = dmg1->rot_z[1] + (PI * 2) / 180;
        dmg1->rot_z[1] = PI <= scmax ? scmax - PI2 : scmax;
        dmg1->alp[1] = 64.0f - (dmg1->cnt[1] * 64.0f) / 40.0f;
        if (0x27 < dmg1->cnt[1])
        {
            dmg1->flow[1]++;
            dmg1->cnt[1] = 0;
            dmg1->alp[1] = 0.0f;
        }
        else if (stop_effects == 0)
        {
            dmg1->cnt[1]++;
        }
    break;
    case 2:
        dmg1->enedmg1_flg = 0;
    break;
    case 0xff:
        // do nothing
    break;
    }

    SetSprFile2(ADDRESS, 0);

    Vu0CopyVector(cpos, camera.p);
    cpos[3] = 1.0f;

    for (i = 0; i < 2; i++)
    {
        Vu0CopyVector(bpos, dmg1->wbpos[i]);

        fx = (bpos[0] - cpos[0]) * (bpos[0] - cpos[0]);
        fy = (bpos[1] - cpos[1]) * (bpos[1] - cpos[1]);
        fz = (bpos[2] - cpos[2]) * (bpos[2] - cpos[2]);

        l = SgSqrtf(fx + fy + fz);
        two = 2; // HACK: fixes stack order

        if (i == 1)
        {
            scl[i] = l < 200.0f ? 1.0f : (2000.0f < l ? 25.0f : ((l - 200.0f) * 24.0f) / 1800.0f + 1.0f);
        }
        else
        {
            scl[i] = 1.0f;
        }

        ppos[i][0][0] = ppos[i][two][0] = -bww[i] * scl[i]; // HACK: fixes stack order
        ppos[i][1][0] = ppos[i][3][0] = +bww[i] * scl[i];
        ppos[i][0][1] = ppos[i][1][1] = -bhh[i] * scl[i];
        ppos[i][2][1] = ppos[i][3][1] = +bhh[i] * scl[i];
        ppos[i][0][2] = ppos[i][1][2] = ppos[i][2][2] = ppos[i][3][2] = 0.0f;
        ppos[i][0][3] = ppos[i][1][3] = ppos[i][2][3] = ppos[i][3][3] = 1.0f;

        wpos[i][0] = cpos[0] + ((bpos[0] - cpos[0]) * (l + dist[i])) / l;
        wpos[i][1] = cpos[1] + ((bpos[1] - cpos[1]) * (l + dist[i])) / l;
        wpos[i][2] = cpos[2] + ((bpos[2] - cpos[2]) * (l + dist[i])) / l;
        wpos[i][3] = 1.0f;
    }

    Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

    for (j = 0; j < 2; j++)
    {
        sceVu0UnitMatrix(wlm);

        wlm[0][0] = dmg1->scw[j];
        wlm[1][1] = dmg1->sch[j];
        wlm[2][2] = 25.0f;

        sceVu0RotMatrixZ(wlm, wlm, dmg1->rot_z[j]);
        sceVu0RotMatrixY(wlm, wlm, rot_y);
        sceVu0TransMatrix(wlm, wlm, wpos[j]);
        sceVu0MulMatrix(slm, SgWSMtx, wlm);
        sceVu0RotTransPersN(ivec[j], slm, ppos[j], 4, 1);

        if (j == 0)
        {
            if (enedmg_fileno_tbl[ene_wrk[dmg1->enedmg_no].dat->mdl_no][0] != -1)
            {
                if (
                    ene_wrk[dmg1->enedmg_no].dat->mdl_no == 42 ||
                    ene_wrk[dmg1->enedmg_no].dat->mdl_no == 43 ||
                    ene_wrk[dmg1->enedmg_no].dat->mdl_no == 44
                )
                {
                    SetSprFile2(ADDRESS_2, 0);
                }
                else
                {
                    SetSprFile2(enedmg_tex_addr[dmg1->enedmg_no], 0);
                }

                n = enedmg_fileno_tbl[ene_wrk[dmg1->enedmg_no].dat->mdl_no][1];

                tex0[j] = enedmg_texno_tbl[n + monochrome_mode].tex0;
                tw[j] = enedmg_texno_tbl[n + monochrome_mode].w;
                th[j] = enedmg_texno_tbl[n + monochrome_mode].h;
                st = 0;
            }
            else
            {
                st = 1;
            }
        }
        else
        {
            tex0[j] = camdat[textbl[j] + monochrome_mode].tex0;
            tw[j] = camdat[textbl[j] + monochrome_mode].w;
            th[j] = camdat[textbl[j] + monochrome_mode].h;
        }

        clip[j] = 1;

        for (i = 0; i < 4; i++)
        {
             if ((ivec[j][i][0] >= 0 && ivec[j][i][0] < 0x300) || clpx2 < ivec[j][i][0])
            {
                clip[j] = 0;
            }

            if ((ivec[j][i][1] >= 0 && ivec[j][i][1] < 0x300) || clpy2 < ivec[j][i][1])
            {
                clip[j] = 0;
            }

            if (ivec[j][i][2] == 0 || clpz2 < ivec[j][i][2])
            {
                clip[j] = 0;
            }

            tq[j][i].fl32 = 1.0f / ivec[j][i][3];
            ts[j][i].fl32 = (tw[j] * tq[j][i].fl32) / szw[j];
            tt[j][i].fl32 = (th[j] * tq[j][i].fl32) / szh[j];
        }
    }

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

    for (j = st; j < 2; j++)
    {
        if (clip[j] != 0)
        {
            pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
            pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

            pbuf[ndpkt].ul64[0] = tex0[j];
            pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

            pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 0, 1, 0, 1, 0, 0, 0, 0), SCE_GIF_PACKED, 3);
            pbuf[ndpkt++].ul64[1] = 0 \
                | SCE_GS_ST    << (4 * 0)
                | SCE_GS_RGBAQ << (4 * 1)
                | SCE_GS_XYZF2 << (4 * 2);

            for (i = 0; i < 4; i++)
            {
                pbuf[ndpkt].ui32[0] = i & 1 ? ts[j][i].ui32 : 0;
                pbuf[ndpkt].ui32[1] = i / 2 ? tt[j][i].ui32 : 0;
                pbuf[ndpkt].ui32[2] = tq[j][i].ui32;
                pbuf[ndpkt++].ui32[3] = 0;

                pbuf[ndpkt].ui32[0] = rgb[monochrome_mode][j][dmg1->enedmg_chance][0];
                pbuf[ndpkt].ui32[1] = rgb[monochrome_mode][j][dmg1->enedmg_chance][1];
                pbuf[ndpkt].ui32[2] = rgb[monochrome_mode][j][dmg1->enedmg_chance][2];
                pbuf[ndpkt++].ui32[3] = dmg1->alp[j];

                pbuf[ndpkt].ui32[0] = ivec[j][i][0];
                pbuf[ndpkt].ui32[1] = ivec[j][i][1];
                pbuf[ndpkt].ui32[2] = ivec[j][i][2] * 16;
                pbuf[ndpkt++].ui32[3] = i > 1 ? 0 : 0x8000;
            }
        }
    }

    pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;

    monochrome_mode = mono;
}

void SetEneDmgEffect1_Sub()
{
    int i;

    for (i = 0; i < 3; i++)
    {
        SetEneDmgEffect1_Sub2(i);
    }
}

void SetEneDmgEffect1(int num)
{
    enedmg2.enedmg_no = num;
    enedmg2.enedmg_chance = (ene_wrk[num].sta & 0x20000) != 0;

    enedmg1[num].enedmg_chance = enedmg2.enedmg_chance;
    enedmg1[num].enedmg_no = num;
    enedmg1[num].enedmg1_flg = 0x1;
}

void SetEneDmgEffect2_Sub2(NEW_PERTICLE *np, float *bpos1, float *bpos2, u_char r1, u_char g1, u_char b1, u_char r2, u_char g2, u_char b2)
{
    int i;
    int n;
    int new_var;
    int tbl[12];
    float fx;
    float fy;
    float fz;
    float l;
    float f1;
    float f2;
    float rot_z;
    float r1l;
    float r2l;
    float r1r;
    float r2r;
    sceVu0FVECTOR wwpos[36];
    sceVu0FVECTOR bpos3;
    sceVu0FVECTOR opos1 = {0.0f, 100.0f, 0.0f, 1.0f};
    sceVu0FVECTOR wpos[3] = {
        {+5.0f, 0.0f, 0.0f, 1.0f},
        {+0.0f, 0.0f, 0.0f, 1.0f},
        {-5.0f, 0.0f, 0.0f, 1.0f},
    };
    sceVu0FVECTOR wkpos[3];
    sceVu0FVECTOR rot;
    sceVu0FVECTOR pos;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    sceVu0FVECTOR rottt;

    r1r = 1.0f;
    r2r = 5.0f;

    fx = (bpos2[0] - bpos1[0]) * (bpos2[0] - bpos1[0]);
    fy = (bpos2[1] - bpos1[1]) * (bpos2[1] - bpos1[1]);
    fz = (bpos2[2] - bpos1[2]) * (bpos2[2] - bpos1[2]);

    l = SgSqrtf(fx + fy + fz);

    r1l = r1r * l / (r1r + r2r);
    r2l = r2r * l / (r1r + r2r);

    n = np->top;

    for (i = 0; i < 12; i++)
    {
        tbl[i] = n;
        n = n - 1 >= 0 ? n - 1 : 11;
    }

    GetTrgtRot(np->oposp[tbl[1]], np->npos, rot, 3);

    while (rot[0] < -PI) rot[0] += PI2;
    while (PI <= rot[0]) rot[0] -= PI2;

    while (rot[1] < -PI) rot[1] += PI2;
    while (PI <= rot[1]) rot[1] -= PI2;

    GetTrgtRot(camera.p, camera.i, rottt, 3);
    sceVu0UnitMatrix(wlm);
    sceVu0RotMatrixY(wlm, wlm, rottt[1]);
    sceVu0RotMatrixX(wlm, wlm, rottt[0]);
    sceVu0TransMatrix(wlm, wlm, np->npos);
    Vu0CopyMatrix(np->wmtxp[tbl[0]], wlm);

    n = 0;

    for (i = 0; i < np->num; i++)
    {
        // not in debug symbols. can be easily removed but
        // the code is more readable with an extra variable
        float f = np->num - i - 1;

        wpos[0][1] = +f * 8.0f / np->num;
        wpos[2][1] = -f * 8.0f / np->num;

        sceVu0ApplyMatrix(wwpos[n*3+0], np->wmtxp[tbl[i]], wpos[0]);
        sceVu0ApplyMatrix(wwpos[n*3+1], np->wmtxp[tbl[i]], wpos[1]);
        sceVu0ApplyMatrix(wwpos[n*3+2], np->wmtxp[tbl[i]], wpos[2]);

        n++;
    }

    DrawNewPerticleSub(n, wwpos, r1, g1, b1, r2, g2, b2, 0x30);

    new_var = np->num; // should not be needed!

    if (stop_effects != 0)
    {
        return;
    }

    if (np->cnt < 180.0f)
    {
        np->top = (np->top + 1) % 12;
        np->num = new_var + 1 >= 12 ? 12 : new_var + 1;
    }
    else
    {
        np->num = new_var - 1 < 0 ? 0 : new_var - 1;
    }

    if (np->cnt < 90.0f)
    {
        f1 = 1.0f - SgSinf((np->cnt * PI) / 180.0f);
        f2 = SgSqrtf(1.0f - f1 * f1);

        np->x = f2 * r1l;
        np->y = SgSinf(f2 * PI * 0.5f) * np->n;
    }
    else
    {
        f1 = 1.0f - SgSinf((np->cnt * PI) / 180.0f);
        f2 = 1.0f - SgSqrtf(1.0f - f1 * f1);

        np->x = f2 * r2l + r1l;
        np->y = SgSinf((f2 + 1.0f) * PI * 0.5f) * np->n;
    }

    bpos3[0] = bpos1[0] + ((bpos2[0] - bpos1[0]) * np->x) / l;
    bpos3[1] = bpos1[1] + ((bpos2[1] - bpos1[1]) * np->x) / l;
    bpos3[2] = bpos1[2] + ((bpos2[2] - bpos1[2]) * np->x) / l;
    bpos3[3] = 1.0f;

    opos1[0] = 0.0f;
    opos1[1] = np->y;
    opos1[2] = 0.0f;
    opos1[3] = 1.0f;

    rot_z = np->rot + (np->rotp * np->x * 135.0f) / (r1l + r2l);

    while (rot_z < -PI) rot_z += PI2;
    while (PI < rot_z) rot_z -= PI2;

    sceVu0UnitMatrix(wlm);
    sceVu0RotMatrixZ(wlm, wlm, rot_z);
    sceVu0ApplyMatrix(opos1, wlm, opos1);
    GetTrgtRot(bpos1, bpos2, rot, 3);
    sceVu0UnitMatrix(wlm);
    sceVu0RotMatrixX(wlm, wlm, rot[0]);
    sceVu0RotMatrixY(wlm, wlm, rot[1]);
    sceVu0TransMatrix(wlm, wlm, bpos3);
    sceVu0ApplyMatrix(wkpos[0], wlm, opos1);

    if (np->cnt < 40.0f)
    {
        np->cnt += np->xp * 3.0f * enedmg2_sp;
    }
    else if (np->cnt < 70.0f)
    {
        np->cnt += np->xp * 10.0f * enedmg2_sp;
    }
    else if (np->cnt < 90.0f)
    {
        np->cnt += np->xp * 15.0f * enedmg2_sp;
    }
    else if (np->cnt < 105.0f)
    {
        np->cnt += np->xp * 2.0f * enedmg2_sp;
    }
    else if (np->cnt < 120.0f)
    {
        np->cnt += np->xp * 1.0f * enedmg2_sp;
    }
    else if (np->cnt < 180.0f)
    {
        np->cnt += np->xp * 0.5f * enedmg2_sp;
        if (np->cnt >= 180.0f)
        {
            np->cnt = 180.0f;
        }
    }
    else
    {
        np->cnt = 180.0f;
    }

    Vu0CopyVector(np->oposp[np->top], np->npos);
    Vu0CopyVector(np->npos, wkpos[0]);
}

void SetEneDmgEffect2_Sub()
{
    int fl;
    u_char rgb1[2][8] = {
        {0x80, 0xa0, 0xff, 0x48, 0x40, 0x60, 0x90, 0x80},
        {0xff, 0x32, 0x32, 0x48, 0x90, 0x20, 0x20, 0x80},
    };
    u_char rgb2[2][8] = {
        {0x90, 0xb0, 0xff, 0x00, 0x40, 0x60, 0x90, 0x00},
        {0xff, 0x48, 0x48, 0x00, 0x90, 0x20, 0x20, 0x00},
    };
    int mono;
    int c;
    int i;
    int j;
    int k;
    int n;
    int nyoro_num;
    float fx;
    float fy;
    float fz;
    float l;
    float f1;
    float f2;
    float rot_z;
    float span1;
    float span2;
    float r1l;
    float r2l;
    float r1r;
    float r2r;
    static sceVu0FVECTOR bpos1;
    sceVu0FVECTOR bpos2;
    sceVu0FVECTOR opos1 = {0.0f, 100.0f, 0.0f, 1.0f};
    sceVu0FVECTOR rot;
    sceVu0FVECTOR pos;
    sceVu0FVECTOR wpos;
    sceVu0FVECTOR ppp2 = {-45.0f, 24.0f, 80.0f, 1.0f};
    sceVu0FVECTOR ppp = {-22.0f, -690.0f, 80.0f, 1.0f};
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    NEW_PERTICLE *np;
    sceVu0FVECTOR spos = {24000.0f, -200.0f, 6400.0f, 1.0f};

    fl = 0;

    r1r = 1.0f;
    r2r = 5.0f;

    if (ingame_wrk.stts & 0x20 || enedmg2.enedmg2_flg == 0)
    {
        return;
    }

    mono = monochrome_mode;
    monochrome_mode = 0;

    ppp2[0] *= camera.fov / 0.89f;
    ppp2[1] *= camera.fov / 0.89f;

    if (enedmg2.enedmg2_flg == 1)
    {
        Vu0CopyVector(bpos1, ene_wrk[enedmg2.enedmg_no].mpos.p0);
        enedmg2.enedmg2_flg = 2;
        fl = 1;
    }
    else
    {
        fl = 0;
    }

    c = enedmg2.enedmg_chance;

    n = ene_wrk[enedmg2.enedmg_no].dmg_old;
    n = n < photo_dmg_tbl[5] ? photo_dmg_tbl[5] : (photo_dmg_tbl[14] < n ? photo_dmg_tbl[14] : n);

    nyoro_num = ((n - photo_dmg_tbl[5]) * 0x28) / (photo_dmg_tbl[14] - photo_dmg_tbl[5]) + 4;

    sceVu0UnitMatrix(wlm);

    if (plyr_wrk.mode != 1)
    {
        sceVu0RotMatrixX(wlm, wlm, plyr_wrk.move_box.rot[0]);
        sceVu0RotMatrixY(wlm, wlm, plyr_wrk.move_box.rot[1]);
        sceVu0TransMatrix(wlm, wlm, plyr_wrk.move_box.pos);
        sceVu0ApplyMatrix(bpos2, wlm, ppp);
    }
    else
    {
        GetTrgtRot(camera.p, camera.i, rot, 3);
        sceVu0RotMatrixX(wlm, wlm, rot[0]);
        sceVu0RotMatrixY(wlm, wlm, rot[1]);
        sceVu0TransMatrix(wlm, wlm, camera.p);
        sceVu0ApplyMatrix(bpos2, wlm, ppp2);
    }

    fx = (bpos2[0] - bpos1[0]) * (bpos2[0] - bpos1[0]);
    fy = (bpos2[1] - bpos1[1]) * (bpos2[1] - bpos1[1]);
    fz = (bpos2[2] - bpos1[2]) * (bpos2[2] - bpos1[2]);

    l = SgSqrtf(fx + fy + fz);

    r1l = (r1r * l) / (r1r + r2r);
    r2l = (r2r * l) / (r1r + r2r);

    span1 = l / 10.0f;
    span2 = span1 + 50.0f;

    if (fl)
    {
        for (i = 0; i < nyoro_num; i++)
        {
            np = &new_perticle[i];

            Vu0CopyVector(np->npos, bpos1);

            np->wmtxp = enedmg2_tail[i].wmtx;
            np->oposp = enedmg2_tail[i].opos;
            np->rot = vu0Rand() * PI2 - PI;
            np->rotp = vu0Rand() * ((PI * 2.0f) / 180.0f) - ((PI / 2.0f) / 180.0f);
            np->n = span1 * vu0Rand() + span2;
            np->xp = vu0Rand() * 0.3f + 0.9f;
            np->cnt = 3.0f;
            np->time = 1;
            np->top = 1;
            np->num = 1;

            f1 = 1.0f - SgSinf((PI * 3.0f) / 180.0f);
            f2 = SgSqrtf(1.0f - f1 * f1);
            np->x = f2 * r1l;
            np->y = SgSinf(f2 * PI * 0.5f) * np->n;

            wpos[0] = bpos1[0] + ((bpos2[0] - bpos1[0]) * np->x) / l;
            wpos[1] = bpos1[1] + ((bpos2[1] - bpos1[1]) * np->x) / l;
            wpos[2] = bpos1[2] + ((bpos2[2] - bpos1[2]) * np->x) / l;
            wpos[3] = 1.0f;

            opos1[0] = 0.0f;
            opos1[1] = np->y;
            opos1[2] = 0.0f;
            opos1[3] = 1.0f;

            rot_z = np->rot + (np->rotp * np->x * 135.0f) / (r1l + r2l);

            while (rot_z < -PI) rot_z += PI2;
            while (PI < rot_z) rot_z -= PI2;

            sceVu0UnitMatrix(wlm);
            sceVu0RotMatrixZ(wlm, wlm, rot_z);
            sceVu0ApplyMatrix(opos1, wlm, opos1);

            GetTrgtRot(bpos1, bpos2, rot, 3);

            sceVu0UnitMatrix(wlm);
            sceVu0RotMatrixX(wlm, wlm, rot[0]);
            sceVu0RotMatrixY(wlm, wlm, rot[1]);
            sceVu0TransMatrix(wlm, wlm, wpos);
            sceVu0ApplyMatrix(wpos, wlm, opos1);

            np->cnt = np->cnt + np->xp * enedmg2_sp;

            Vu0CopyVector(np->oposp[0], np->npos);
            Vu0CopyVector(np->npos, wpos);
        }
    }

    j = -1;
    k = -1;

    for (i = 0; i < nyoro_num; i++)
    {
        if (new_perticle[i].time == 1.0f)
        {
            if (new_perticle[i].num > 0)
            {
                SetEneDmgEffect2_Sub2(
                    &new_perticle[i],
                    bpos1, bpos2,
                    rgb2[c][0], rgb2[c][1], rgb2[c][2],
                    rgb2[c][4], rgb2[c][5], rgb2[c][6]);
                k++;
            }

            if (new_perticle[i].cnt < 180.0f)
            {
                DRAW_ENV de = {
                    .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
                    .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
                    .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
                    .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL),
                    .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
                    .prim = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 0, 1, 0, 1, 0, 0, 0, 0), SCE_GIF_PACKED, 3),
                };
                float rot_x;
                float rot_y;
                float f;

                Get2PosRot(camera.p, camera.i, &rot_x, &rot_y);

                sceVu0UnitMatrix(wlm);
                sceVu0RotMatrixX(wlm, wlm, rot_x);
                sceVu0RotMatrixY(wlm, wlm, rot_y);
                sceVu0TransMatrix(wlm, wlm, new_perticle[i].npos);

                if (plyr_wrk.mode != 1)
                {
                    f = 1.0f - new_perticle[i].cnt / 180.0f;
                }
                else
                {
                    f = 1.0f;
                }

                Set3DPosTexure(wlm, &de, 0x16, 72.0f, 72.0f, rgb1[c][0], rgb1[c][1], rgb1[c][2], f * 0x10);
                Set3DPosTexure(wlm, &de, 0x2e, 20.0f, 20.0f, rgb1[c][4], rgb1[c][5], rgb1[c][6], f * 0x6e);
            }
            else
            {
                j = i;
            }
        }
    }

    if (j != -1 && k != -1)
    {
        if (plyr_wrk.mode == 1)
        {
            // SetGlowOfAFirefly is undeclared !!
            SetGlowOfAFirefly(bpos2, 12.0f, rgb1[c][0], rgb1[c][1], rgb1[c][2], rgb1[c][4], rgb1[c][5], rgb1[c][6], rgb1[c][7]);
        }
        enedmg_status = 1;
    }

    if ((j != -1) && (k == -1))
    {
        enedmg_status = 2;
        enedmg2.enedmg2_flg = 0;
    }

    monochrome_mode = mono;
}

void SetEneDmgEffect2()
{
    enedmg2.enedmg2_flg = 1;
}
