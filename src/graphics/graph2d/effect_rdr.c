#include "common.h"
#include "typedefs.h"
#include "effect_rdr.h"

#include "sce/libvu0.h"

#include "os/pad.h"
#include "main/glob.h"
#include "os/eeiop/eese.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_oth.h"
#include "graphics/graph2d/rare_ene.h"
#include "graphics/graph2d/g2d_main.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/tim2_new.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/libsg.h"
#include "ingame/map/furn_spe/furn_spe.h"
#include "ingame/map/furn_spe/fsla_main.h"

typedef struct {
	sceVu0FVECTOR bpos;
	sceVu0FVECTOR brot;
	sceVu0FVECTOR lpos;
	float ww;
	float hh;
	int power;
	u_char r;
	u_char g;
	u_char b;
	u_char a;
	void *adr;
} SUNSHINE;

#include "data/sunshine.h"       // static SUNSHINE sunshine[];
#include "data/ef_pazzene_p.h"   // static sceVu0FVECTOR ef_pazzene_p;
#include "data/ef_smoke_pos11.h" // static sceVu0FVECTOR ef_smoke_pos11;
#include "data/ef_smoke_pos12.h" // static sceVu0FVECTOR ef_smoke_pos12;
#include "data/ef_smoke_pos13.h" // static sceVu0FVECTOR ef_smoke_pos13;
#include "data/ef_smoke_pos14.h" // static sceVu0FVECTOR ef_smoke_pos14;

#include "data/ef_smoke_pos21.h" // static sceVu0FVECTOR ef_smoke_pos21; // unused
#include "data/ef_smoke_pos22.h" // static sceVu0FVECTOR ef_smoke_pos22; // unused
#include "data/ef_smoke_pos23.h" // static sceVu0FVECTOR ef_smoke_pos23; // unused
#include "data/ef_smoke_pos24.h" // static sceVu0FVECTOR ef_smoke_pos24; // unused

BURN_FIRE burn_fire[10] = {0};
EFFRDR_RSV blood_drop_rsv[16] = {0};
EFFRDR_RSV pfire_rsv[4] = {0};

static int room_old;
static u_char ef_pazz_ene_flow;
static int ef_rdfire3_cnt;

static int ef_rdfire3_call[6];
static FURN_ACT_WRK *ef_rdfire3[6];
static void *ef_smoke_addr[4];

#define PI 3.1415927f

void InitEffectRdr()
{
	int i;

    room_old = -1;

    ef_pazz_ene_flow = 0;

    memset(burn_fire, 0, sizeof(burn_fire));

    ef_rdfire3_cnt = -1;

    for (i = 0; i < 7; i++)
    {
        sunshine[i].adr = NULL;
    }

    for (i = 0; i < 16; i++)
    {
        blood_drop_rsv[i].furn_id = -1;
    }

    for (i = 0; i < 4; i++)
    {
        pfire_rsv[i].furn_id = -1;
    }
}

void InitEffectRdrEF()
{
    return;
}


void* GetBurnFireWork()
{
    int i;

    for (i = 0; i < 10; i++)
    {
        if (burn_fire[i].usefl == 0)
        {
            burn_fire[i].usefl = 1;

            return &burn_fire[i];
        }
    }

    return NULL;
}

void* SearchBurnFireFurnID(u_short furn_id)
{
    int i;

    for (i = 0; i < 10; i++)
    {
        if (burn_fire[i].usefl == 1 && burn_fire[i].furn_id == furn_id)
        {
            return &burn_fire[i];
        }
    }

    return NULL;
}

void SetRDLongFire2(sceVu0FVECTOR pos, u_char sta, float szw, float szh, float sw, float sh, float r, float g, float b, float room, u_short furn_id)
{
	EFFECT_CONT *ecw;
	void *ret;

    ret = GetBurnFireWork();

    if (ret != NULL)
    {
        BURN_FIRE *bf = ret;

        Vu0CopyVector(bf->fpos, pos);

        bf->ebuf = SetEffects(0x17, 2, 0, &bf->fpos, 0x80, 0x75, 0x70, 0.3f, 0xf0, 0xd0, 0xa0, 3.0f);

        bf->furn_id = furn_id;
        bf->ligdiff[0] = r;
        bf->ligdiff[1] = g;
        bf->ligdiff[2] = b;
        bf->ligdiff[3] = 1.0f;
        bf->szw = szw;
        bf->szh = szh;
        bf->sw = sw;
        bf->sh = sh;
        bf->sta = sta;

        if (sta & 1)
        {
            bf->lignum = AddNewPointLight(&bf->ligpos, &bf->ligdiff, &bf->ligpow, room);
        }

        ecw = bf->ebuf;

        ecw->pnt[5] = bf;
    }
}

void SetRDLongFire(sceVu0FVECTOR pos, float r, float g, float b, float room, u_short furn_id)
{
    SetRDLongFire2(pos, 3, 1.0f, 1.0f, 1.0f, 1.0f, r, g, b, room, furn_id);
}

void ResetRDLongFire(u_short furn_id)
{
	void *ret;

    ret = SearchBurnFireFurnID(furn_id);

    if (ret != NULL)
    {
        BURN_FIRE *bf = ret;

        if (bf->sta & 1)
        {
            DeletePointLight(bf->lignum);
        }

        ResetEffects(bf->ebuf);

        bf->usefl = 0;
    }
}

void SubRDFire(EFFECT_CONT *ec)
{
	static int rnbk = 0;
	u_char mr;
	u_char mg;
	u_char mb;
	u_char mrh;
	u_char mgh;
	u_char mbh;
	int bak;
	int i;
	int n;
	int rn;
	int tw;
	int th;
	int w;
	int wpat;
	u_int vv[24];
	u_int clpx2;
	u_int clpy2;
	u_int clpz2;
	float bsw;
	float bsh;
	float sw;
	float sh;
	float fire1;
	float fire2;
	// float fire3;
	float fire4;
	float fire5;
	float fire6;
	float fire7;
	float arate;
	float wscw;
	float wsch;
	float ligpow;
	float msc;
	float msch;
	float f;
	float f1;
	float f2;
	float rot_x;
	float rot_y;
	u_long tx0;
	sceVu0FMATRIX wlm;
	sceVu0FMATRIX slm;
	sceVu0IVECTOR ipos;
	sceVu0IVECTOR ivec[48];
	sceVu0FVECTOR vtw[48];
	sceVu0FVECTOR vpos;
	sceVu0FVECTOR wcpos = { 0.0f, 4.0f, 0.0f, 1.0f };
	BURN_FIRE *bf;
	DRAW_ENV de = {
           .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
           .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
           .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
           .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL),
           .clamp = 0,
           .prim = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_TRUE, 84, SCE_GIF_PACKED, 3),
    };
    int mm; // not in STAB!
    int *ivec47; // not in STAB!
    float ss; // not in STAB!

    clpx2 = 0xe400;
    clpy2 = 0xe400;
    clpz2 = 0xffffff;

    if (ec->dat.uc8[1] & 1)
    {
        ResetEffects(ec);
        return;
    }

    bf = (BURN_FIRE *)ec->pnt[5];

    bsw = bf->szw * 25.0f;
    bsh = bf->szh * 25.0f;

    sw = bf->sw;
    sh = bf->sh;

    fire1 = sh * 60.0f;
    fire2 = sh * 0.5f;
    // fire3 ??
    fire4 = sh * 0.1f;
    fire5 = sh * 0.08f;
    fire6 = sh * 0.04f;
    fire7 = sh * 0.01f;

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

    wpat = ec->cnt;
    wscw = ec->fw[1];
    wsch = ec->fw[2];
    arate = ec->fw[0];

    switch (ec->flow)
    {
    case 0:
        wpat = 0;

        wscw = 1.0f;
        wsch = 1.0f;

        ligpow = 0.0f;

        bf->wavew = fire2;
        bf->rate = 0.0f;

        // wsch = wscw;

        if (plyr_wrk.pr_info.room_no == 14)
        {
            SeStartPosEventFlame(plyr_wrk.pr_info.room_no, 0, ec->pnt[0], 0, 0x1000, 0x1000);
        }
        else if (plyr_wrk.pr_info.room_no == 29)
        {
            SeStartPosSrundFlame(plyr_wrk.pr_info.room_no, ec->pnt[0], 0, 0x1000, 0x1000);
        }
        else
        {
            SeStartPut(17, ec->pnt[0], 0, 0x1000, 0x1000, 0xff);
        }

        ec->flow = 1;
    break;
    case 1:
        wscw += sw * 18.0f;
        wsch += sh * 74.0f;

        bf->wavew = fire4 < bf->wavew - fire5 ? bf->wavew - fire5 : fire4;

        if (bf->sta & 2)
        {
            VibrateRequest1(0, 1);
        }

        ligpow = (wsch * 150.0f) / 445.0f;

        if (wpat > 5)
        {
            ec->flow = 2;
        }
    break;
    case 2:
        f1 = sw * 8.0f;
        f2 = sh * 16.0f;

        bf->wavew = fire6 < bf->wavew - fire7 ? bf->wavew - fire7 : fire6;

        wscw = wscw - f1 <= bsw ? bsw : wscw - f1;
        wsch = wsch - f2 <= bsh ? bsh : wsch - f2;

        ligpow = (wsch * 144.0f) / 445.0f + 6.0f;

        if (wsch <= bsh && wscw <= bsw)
        {
            ec->flow = 0xff;

            FSpeFinishFlgOff(bf->furn_id);
        }
    break;
    case 0xff:
        ligpow = 6.0f;

        bf->wavew = 0.01f;

        wsch = bsh;
        wscw = bsw;
    break;
    }

    if (stop_effects == 0)
    {
        wpat++;
    }

    mm = monochrome_mode + 32; // mm is not in STAB!
    i = ((wpat / 3) % 7) * 2 + mm; // mm is not in STAB!

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
    sceVu0ApplyMatrix(bf->ligpos, wlm, wcpos);

    for (i = 0; i < 24; i++)
    {
        vtw[i*2+0][0] = -2.0f;
        vtw[i*2+1][0] = +2.0f;
        vtw[i*2+0][1] = vtw[i*2+1][1] = ((23 - i) * 8.0f) / 24;
        vtw[i*2+0][2] = vtw[i*2+1][2] = 0.0f;
        vtw[i*2+0][3] = vtw[i*2+1][3] = 1.0f;

        vv[i] = (i * th) / 24;
    }

    for (i = 0; i < 24; i++)
    {
        f1 = i;
        f = 23.0f / 2.0f;
        f = f - __builtin_fabsf(f - i);
        ss = SgSinf(((bf->rate + (f1 * fire1)) * PI) / 180.0f);
        ss = ss * bf->wavew;
        f2 = ss * f;

        vtw[i*2+0][0] += f2;
        vtw[i*2+1][0] += f2;
    }

    bf->rate = bf->rate + fire1;

    sceVu0RotTransPersN(ivec, slm, vtw, 48, 0);

    w = 0;

    for (i = 0; i < 48; i++)
    {
        if ((ivec[i][0] >= 0 && ivec[i][0] < 0x1c00) || ivec[i][0] > clpx2)
        {
            w = 1;
        }

        if ((ivec[i][1] >= 0 && ivec[i][1] < 0x1c00) || ivec[i][1] > clpy2)
        {
            w = 1;
        }

        if ((ivec[i][2] >= 0 && ivec[i][2] < 0xff) || ivec[i][2] > clpz2)
        {
            w = 1;
        }
    }

    if (!w)
    {
        ivec47 = ivec[47];

        ipos[0] = (ivec[0][0] + ivec47[0]) / 2;
        ipos[1] = ((ivec[0][1] - ivec47[1]) * 0.3f) + ivec47[1];
        ipos[2] = ec->z;
        ipos[3] = 0;

        n = ((((ivec47[1] - ivec[0][1]) / 2) < ((ivec47[0] - ivec[0][0]) / 2)) ? ((ivec47[0] - ivec[0][0]) / 2) : ((ivec47[1] - ivec[0][1]) / 2));
        f = n * 0.0625f;

        if (stop_effects == 0)
        {
            rnbk = rn = (int)(vu0Rand() * 8.0f);
        }
        else
        {
            rn = rnbk;
        }

        fire7 = (int)(rn / 2 + 5);
        bf->ligpow = (fire7 * 2 + ligpow) * arate;

        SetEffSQITex(monochrome_mode + 0x16, ipos, 3, f * msch, f * msch, mrh, mgh, mbh, (fire7 * arate));
        SetEffSQITex(monochrome_mode + 0x16, ipos, 3, f * msch * 0.5f, f * msch * 0.5f, mrh, mgh, mbh, (rn + 2) * arate);

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

        pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(24, SCE_GS_TRUE, SCE_GS_TRUE, 348, SCE_GIF_PACKED, 6);
        pbuf[ndpkt++].ul64[1] = 0 \
            | SCE_GS_RGBAQ << (4 * 0)
            | SCE_GS_UV    << (4 * 1)
            | SCE_GS_XYZF2 << (4 * 2)
            | SCE_GS_RGBAQ << (4 * 3)
            | SCE_GS_UV    << (4 * 4)
            | SCE_GS_XYZF2 << (4 * 5);

        for (i = 0; i < 24; i++) // i++
        {
            pbuf[ndpkt].ui32[0] = mr;
            pbuf[ndpkt].ui32[1] = mg;
            pbuf[ndpkt].ui32[2] = mb;
            pbuf[ndpkt++].ui32[3] = arate * 96.0f;

            pbuf[ndpkt].ui32[0] = 0;
            pbuf[ndpkt].ui32[1] = vv[i];
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = ivec[i*2+0][0];
            pbuf[ndpkt].ui32[1] = ivec[i*2+0][1];
            pbuf[ndpkt].ui32[2] = ivec[i*2+0][2];
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = mr;
            pbuf[ndpkt].ui32[1] = mg;
            pbuf[ndpkt].ui32[2] = mb;
            pbuf[ndpkt++].ui32[3] = arate * 96.0f;

            pbuf[ndpkt].ui32[0] = tw;
            pbuf[ndpkt].ui32[1] = vv[i];
            pbuf[ndpkt].ui32[2] = 0;
            pbuf[ndpkt++].ui32[3] = 0;

            pbuf[ndpkt].ui32[0] = ivec[i*2+1][0];
            pbuf[ndpkt].ui32[1] = ivec[i*2+1][1];
            pbuf[ndpkt].ui32[2] = ivec[i*2+1][2];
            pbuf[ndpkt++].ui32[3] = 0;
        }

        pbuf[bak].ui32[0] = ndpkt + DMAend - bak - 1;
    }

    ec->cnt = wpat;
    ec->fw[1] = wscw;
    ec->fw[2] = wsch;
}

void SetRDFire3(FURN_ACT_WRK *f1, FURN_ACT_WRK *f2, FURN_ACT_WRK *f3, FURN_ACT_WRK *f4, FURN_ACT_WRK *f5, FURN_ACT_WRK *f6)
{
	int i;

    ef_rdfire3_cnt = 0;

    ef_rdfire3[0] = f1;
    ef_rdfire3[1] = f2;
    ef_rdfire3[2] = f3;
    ef_rdfire3[3] = f4;
    ef_rdfire3[4] = f5;
    ef_rdfire3[5] = f6;

    for (i = 0; i < 6; i++)
    {
        ef_rdfire3_call[i] = -1;
    }
}

void ResetRDFire3()
{
	int i;

    for (i = 0; i < 6; i++)
    {
        ResetRDLongFire(ef_rdfire3[i]->furn_id);
    }

    ef_rdfire3_cnt = -1;
}

void SubRDFire3()
{
	int i;
	int time[6] = { 0, 30, 60, 90, 120, 150 };

    if (ef_rdfire3_cnt < 0)
    {
        return;
    }

    for (i = 0; i < 6; i++)
    {
        if (time[i] < ef_rdfire3_cnt && ef_rdfire3_call[i] == -1)
        {
            SetRDLongFire2(ef_rdfire3[i]->pos, 0, 1.0f, 1.0f, 0.3f, 0.1f, 100.0f, 80.0f, 40.0f, 0.0f, ef_rdfire3[i]->furn_id);

            ef_rdfire3_call[i] = 1;
        }
    }

    if (ef_rdfire3_call[5] == 1)
    {
        ef_rdfire3_cnt = -1;
    }
    else
    {
        ef_rdfire3_cnt++;
    }
}

void SetRDPazzEne(sceVu0FVECTOR pos)
{
    Vu0CopyVector(ef_pazzene_p, pos);

    ef_pazz_ene_flow = 3;
}

void ResetRDPazzEne()
{
    ef_pazz_ene_flow = 5;
}

void SetRoomDirecPazzEne()
{
	static u_char alp;
	sceVu0FVECTOR bpos;

    switch(ef_pazz_ene_flow)
    {
    case 0:
        // do nothing ...
    break;
    case 1:
        g2d_load_flg.pazz = LoadReq(pazz_textbl[7], 0x1cb0000);

        ef_pazz_ene_flow = 2;
    case 2:
        if (IsLoadEnd(g2d_load_flg.pazz))
        {
            ef_pazz_ene_flow = 0x3;
        }
    break;
    case 3:
        alp = 0;
        ef_pazz_ene_flow = 0x4;
    case 4:
        alp = alp + 4 >= 0x40 ? 0x40 : alp + 4;

        Vu0CopyVector(bpos, ef_pazzene_p);

        DrawRareEne_Sub(2, 2, bpos, 136, 0, alp);
    break;
    case 5:
        alp = alp - 4 < 0 ? 0 : alp - 4;

        Vu0CopyVector(bpos, ef_pazzene_p);

        DrawRareEne_Sub(2, 2, bpos, 136, 0, alp);

        if (alp == 0)
        {
            ef_pazz_ene_flow = 0x6;
        }
    break;
    case 6:
        ef_pazz_ene_flow = 0x0;
    }
}

void SetRDSmoke()
{
    ef_smoke_addr[0] = SetEffects(26, 2, ef_smoke_pos11);
    ef_smoke_addr[1] = SetEffects(26, 2, ef_smoke_pos12);
    ef_smoke_addr[2] = SetEffects(26, 2, ef_smoke_pos13);
    ef_smoke_addr[3] = SetEffects(26, 2, ef_smoke_pos14);
}

void ResetRDSmoke()
{
    ResetEffects(ef_smoke_addr[0]);
    ResetEffects(ef_smoke_addr[1]);
    ResetEffects(ef_smoke_addr[2]);
    ResetEffects(ef_smoke_addr[3]);
}

void SetRoomDirecSmoke()
{
	int room_id;

    room_id = 3;

    if (plyr_wrk.pr_info.room_no == room_id)
    {
        if (room_old != room_id)
        {
            SetRDSmoke();
        }
    }
    else
    {
        if (room_old == room_id)
        {
            ResetRDSmoke();
        }
    }

    room_old = plyr_wrk.pr_info.room_no;
}

void SetRDSunshine(int n)
{
    if (sunshine[n].adr == NULL)
    {
        sunshine[n].adr = SetEffects(
            31, 2,
            sunshine[n].lpos, &sunshine[n],
            sunshine[n].brot, sunshine[n].power,
            sunshine[n].ww, sunshine[n].hh,
            sunshine[n].r,sunshine[n].g, sunshine[n].b);
    }
}

void ResetRDSunshine(int n)
{
    ResetEffects(sunshine[n].adr);

    sunshine[n].adr = NULL;
}

short int GetRDBloodDropWork()
{
	int i;

    for (i = 0; i < 16; i++)
    {
        if (blood_drop_rsv[i].furn_id == -1)
        {
            return i;
        }
    }

    return -1;
}

short int SearchRDBloodDropWork(u_short furn_id)
{
	int i;

    for (i = 0; i < 16; i++)
    {
        if (blood_drop_rsv[i].furn_id == furn_id)
        {
            return i;
        }
    }

    return -1;
}

void SetRDBloodDrop(float *pos, int sta, u_short furn_id)
{
	int ret;

    ret = GetRDBloodDropWork();

    if (ret < 0)
    {
        return;
    }

    if (sta & 4)
    {
        blood_drop_rsv[ret].adr = SetEffects(0x1e, 2, pos, sta, 250.0f, 200, 0x80, 0x80, 0x80);
    }
    else
    {
        blood_drop_rsv[ret].adr = SetEffects(0x1e, 2, pos, sta, -10.0f, 0x78, 0x80, 0, 0);
    }

    blood_drop_rsv[ret].furn_id = furn_id;
}

void ResetRDBloodDrop(u_short furn_id)
{
    short int ret;

    ret = SearchRDBloodDropWork(furn_id);

    if (ret < 0)
    {
        return;
    }

    ResetEffects(blood_drop_rsv[ret].adr);

    blood_drop_rsv[ret].furn_id = -1;
}

short int GetRDPFireWork()
{
	int i;

    for (i = 0; i < 4; i++)
    {
        if (pfire_rsv[i].furn_id == -1)
        {
            return i;
        }
    }

    return -1;
}

short int SearchRDPFireWork(u_short furn_id)
{
	int i;

    for (i = 0; i < 4; i++)
    {
        if (pfire_rsv[i].furn_id == furn_id)
        {
            return i;
        }
    }

    return -1;
}

void SetRDPFire(sceVu0FVECTOR pos, u_short furn_id)
{
    short int ret;

    ret = GetRDPFireWork();

    if (ret < 0)
    {
        return;
    }

    pfire_rsv[ret].adr = SetEffects(25, 2, 3, pos, 0, 0);

    pfire_rsv[ret].furn_id = furn_id;
}

void SetRDPFire2(float *pos, u_short furn_id)
{
	EFFECT_CONT *ecw;
    short int ret;

    ret = SearchRDPFireWork(furn_id);

    if (ret < 0)
    {
        return;
    }

    ecw = pfire_rsv[ret].adr;

    ecw->dat.uc8[3] = 18;
}

void ResetRDPFire(u_short furn_id)
{
	HEAT_HAZE *hh;
	EFFECT_CONT *ec;
    short int ret;

    ret = SearchRDPFireWork(furn_id);

    if (ret < 0)
    {
        return;
    }

    ec = pfire_rsv[ret].adr;

    hh = ec->pnt[1];
    hh->flag = 0;

    ResetEffects(ec);

    pfire_rsv[ret].furn_id = 0xffff;
}
