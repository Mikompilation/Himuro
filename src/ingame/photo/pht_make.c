#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "pht_make.h"

#include "ee/kernel.h"
#include "sce/libgraph.h"

#include "graphics/graph2d/compress.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_scr.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/g2d_debug.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph2d/tim2_new.h"
#include "graphics/graph3d/sglib.h"
#include "ingame/map/furn_spe/furn_spe.h"
#include "ingame/menu/ig_album.h"
#include "ingame/menu/ig_glst.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/ig_rank.h"
#include "ingame/photo/photo.h"
// #include "ingame/plyr/plyr_ctl.h" // FModeScreenEffect
#include "main/glob.h"

#define INCLUDING_FROM_PHT_MAKE_C
#include "ingame/map/furn_ctl.h" // SetFurnAttrEve
#undef INCLUDING_FROM_PHT_MAKE_C

#include "data/hintdat.h" // SPRT_DAT hintdat[];
#include "data/hintreadat.h" // SPRT_DAT hintreadat[];
#include "data/pazene_load_tbl.h" // int pazene_load_tbl[];

int frm_type = 2;
int ers_type = 1;

#define PI 3.1415927f

void InitPhotoMake()
{
    hint_2d = 0;
    hint_rea = 0;
    hint_3d = 0;
    hint_3d_num = hint_2d_num = 0;
    pic_save_flag = 0;
    pht_finder_off = 0;
}

void CopyScreenToBuffer(int addr, int szfl, int mx, int my, int mw, int mh)
{
    int *data_i;
    short int *data_o;
    int x;
    int y;
    int oneli;
    int onelo;
    u_int un;
    int myy;
    int mhh;

    myy = my / 2; // because of interlacing
    mhh = mh / 2; // because of interlacing

    oneli = 640;
    onelo = (((mw - 1) / 64) + 1) * 64; // ceiling to a multiple of 64 (if mw > 0) => DMA alignment?

    data_i = GetEmptyBuffer(0);

    if (szfl != 0)
    {
        *(int *)addr = 0x18000;

        data_o = (short *)(addr + 16);
    }
    else
    {
        data_o = (short *)addr;
    }

    LocalCopyLtoB2(0, ((sys_wrk.count + 1) & 1) * 0x8c0); // select field (framebuffer)

    // reads a 32-bit pixel and writes a 16-bit pixel for a rectangle (mx,my) of size (mw,mh)
    for (y = 0; y < mhh; y++)
    {
        for (x = 0; x < mw; x++)
        {
            un = *(u_int *)(data_i + mx + x + (myy + y) * oneli); // 0xAARRGGBB

            *(short *)(data_o + x + y * onelo) = // Converts 32-bit RGB to 16-bit PSMCT16
                0x8000               | // alpha (1 bit, 1 = opaque, 0 = transparent)
                ((un >> 9) & 0x7C00) | // Red   → bits 10–14 -> equivalent to: (((un >> 16) & 0xFF) >> 3) << 10
                ((un >> 6) & 0x03E0) | // Green → bits 5–9   -> equivalent to: (((un >>  8) & 0xFF) >> 3) <<  5
                ((un >> 3) & 0x001F);  // Blue  → bits 0–4   -> equivalent to: (((un >>  0) & 0xFF) >> 3) <<  0
            // extracts a rectangular region from a 640-wide framebuffer and converts it from 32-bit color to 16-bit PSMCT16 (A RRRRR GGGGG BBBBB) format, aligned to 64-pixel rows
        }
    }
}

void DrawPhotoBuffer(u_int pri, int addr, int szfl, int x, int y, int szw, int szh, int mszw, int mszh, int ftype, u_char alp, int ztype)
{
    u_long128 *data_i;
    int dbw;
    sceGsLoadImage gs_limage;
    float fh;
    float xx;
    float yy;
    SPRITE_DATA sd = {
        .g_GsTex0 = (sceGsTex0){
            .TBP0 = 0,
            .TBW = 0,
            .PSM = 2,
            .TW = 0,
            .TH = 0,
            .TCC = 0,
            .TFX = 0,
            .CBP = 0,
            .CPSM = 0,
            .CSM = 0,
            .CSA = 0,
            .CLD = 1,
        },
        .g_nTexSizeW = 0,
        .g_nTexSizeH = 0,
        .g_bMipmapLv = 0,
        .g_GsMiptbp1 = 0,
        .g_GsMiptbp2 = 0,
        .pos_x = 0.0f,
        .pos_y = 0.0f,
        .pos_z = 0,
        .size_w = 0.0f,
        .size_h = 0.0f,
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
        .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_CLAMP, SCE_GS_CLAMP, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 5),
    };
    float fw; // not in STAB

    if (ztype != 0) {
        de.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
        de.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    } else {
        de.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
        de.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS);
    }

    xx = (x - 320.0f) - 1.0f;
    yy = (y - 224.0f) - 1.0f;

    dbw = (((mszw - 1) / 64) + 1);

    if (szfl != 0)
    {
        data_i = (u_long128 *)(addr + 16);
    } else {
        data_i = (u_long128 *)addr;
    }

    sceGsSetDefLoadImage(&gs_limage, 0x2780, dbw, 2, 0, 0, dbw * 64, mszh);
    FlushCache(0);
    sceGsExecLoadImage(&gs_limage, data_i);
    sceGsSyncPath(0, 0);

    sd.g_GsTex0.TBP0 = 0x2780;
    sd.g_GsTex0.TBW = dbw;
    sd.g_GsTex0.TW = log_2(dbw * 64.0f);
    sd.g_GsTex0.TH = log_2(mszh);
    sd.g_nTexSizeW = mszw;
    sd.g_nTexSizeH = mszh;
    sd.alpha = alp;
    sd.size_w = szw;
    sd.size_h = szh;
    sd.pos_x = xx;
    sd.pos_y = yy;
    sd.pos_z = 0xfffffff - pri;

    SetTexDirectS2(pri, &sd, &de, 1);

    if (ftype == 1)
    {
        fh = (szw - 48.0f) / 84.0f + 2.0f;

        fw = (fh < 2.0f) ? 2.0f : fh;
        fh = (fh < 2.0f) ? 2.0f : fh;
        xx += 320.0f;
        yy += 224.0f;

        SetPanel(pri, xx -  fw + 2.0f, yy -  fh + 1.0f, xx + szw + fw - 2.0f, yy            + 1.0f, 0xcf, 0xbd, 0xa1, alp);
        SetPanel(pri, xx -  fw + 2.0f, yy + szh - 1.0f, xx + szw + fw - 2.0f, yy + szh + fh - 1.0f, 0xcf, 0xbd, 0xa1, alp);
        SetPanel(pri, xx -  fw + 2.0f, yy       + 1.0f, xx            + 2.0f, yy + szh      - 1.0f, 0xcf, 0xbd, 0xa1, alp);
        SetPanel(pri, xx + szw - 2.0f, yy       + 1.0f, xx + szw + fw - 2.0f, yy + szh      - 1.0f, 0xcf, 0xbd, 0xa1, alp);
    }

    DrawAll2D();
}

void CompressData(int addri, int addro, int n)
{
    int one_size;
    u_int quality;
    u_int *addr;
    float value;

    one_size = 0xd350;

    value = SlideEncode((u_char *)addri, (u_char *)addri + 0x18010, 0x18010);

    if (value > 0.549f)
    {
        quality = 1;

        while (1)
        {
            memset((void *)(addri + 0x18010), 0xff, 0x18010);

            value = CompressFile((u_short *)addri, (char *)(addri + 0x18010), 0x18010, quality);

            quality++;

            if (value < 0.549f)
            {
                break;
            }

            if (quality >= 5)
            {
                addr = (u_int *)(addri + 0x18010);
                addr[1] = 2;

                break;
            }
        }
    }

    memcpy((void *)(addro + one_size * n), (void *)(addri + 0x18010), one_size);
}

void UncompressData(int addri, int n, int addro)
{
    int one_size;
    u_int type;
    u_char *base;

    one_size = 0xd350;

    base = (u_char *)(addri + n * one_size);
    type = *(int *)(base + 4);

    switch (type)
    {
    case 0:
        SlideDecode(base, (u_char *)addro);

        photo_expand.cnt = 0;
        photo_expand.sta = 1;
    break;
    case 1:
        ExpandFile((char *)base, (u_short *)addro);
    break;
    case 2:
        memset((void *)addro, 0, 0x18010);

        photo_expand.cnt = 0;
        photo_expand.sta = 1;
    break;
    }
}

void TakePhotoFromScreen()
{
    CopyScreenToBuffer(0x1E90000, 1, 128, 80, 384, 256);
}

void MakeSPhotoFromWorkArea(int n)
{
    int addr;

    addr = 0x01a90000 + n * 0xa00;

    DrawPhotoBuffer(0x440, 0x01e90000, 1, 0, 0, 66, 42, 384, 128, 0, 0x80, 0);

    CopyScreenToBuffer(addr, 0, 1, 1, 64, 40);
}

void MakeSPhotoFromCompress(int ni, int no)
{
    int addr;

    addr = 0x01a90000 + no * 0xa00;

    UncompressData(0x1aa5400, ni, 0x1e90000);

    DrawPhotoBuffer(0x440, 0x1e90000, 1, 0, 0, 64, 40, 384, 128, 0, 0x80, 0);

    CopyScreenToBuffer(addr, 0, 0, 0, 64, 40);
}

void DrawSPhotoFromSmallPhotoArea(int n, int pri, int ftype, int x, int y, u_char alp)
{
    int addr;
    int addr2;

    addr = 0x01a90000;
    addr2 = addr + n * 2560;

    DrawPhotoBuffer(pri, addr2, 0, x, y, 64, 40, 64, 20, ftype, alp, 1);
}

void DrawSPhotoFromSmallPhotoAreaAD(int addr, int n, int pri, int ftype, int x, int y, u_char alp)
{
    int addr2;

    addr2 = addr + n * 0xa00;

    DrawPhotoBuffer(pri, addr2, 0, x, y, 64, 40, 64, 20, ftype, alp, 1);
}

void DrawSPhotoFromSmallPhotoArea2(int n, int pri, int ftype, int x, int y, int szw, int szh, u_char alp)
{
    int addr;

    addr = 0x01a90000 + n * 0xa00;

    DrawPhotoBuffer(pri, addr, 0, x, y, szw, szh, 64, 20, ftype, alp, 1);
}

void DrawSPhotoFromSmallPhotoArea2AD(int addr, int n, int pri, int ftype, int x, int y, int szw, int szh, u_char alp)
{
    int addr2;

    addr2 = addr + n * 0xa00;

    DrawPhotoBuffer(pri, addr2, 0, x, y, szw, szh, 64, 20, ftype, alp, 1);
}

void CompPhotoFromWorkArea(int n)
{
    CompressData(0x1E90000, 0x1AA5400, n);
}

void DrawPhotoFromWorkArea(int pri, int ftype, int x, int y, int szw, int szh, u_char alp)
{
    DrawPhotoBuffer(pri, 0x1e90000, 1, x, y, szw, szh, 384, 128, ftype, alp, 1);
}

void DrawPhotoFromCompress(int n, u_int pri, int ftype, int x, int y, int szw, int szh, u_char alp)
{
    return;
}

void UncompressPhoto(int n)
{
    UncompressData(0x1AA5400, n, 0x1E90000);
}

void UncompressPhotoAD(int addr, int n)
{
    UncompressData(addr, n, 0x1E90000);
}

void DrawPhotoFromPhotoWrk(int n, u_int pri, int ftype, int x, int y, int szw, int szh, u_char alp)
{
    DrawPhotoBuffer(pri, 0x1e90000, 1, x, y, szw, szh, 384, 128, ftype, alp, 1);
}

void CopyPhoto(int addri, int ni, int addro, int no)
{
    int i;
    short int *data_o;
    short int *data_i; // not in STAB
    int one_size;

    one_size = 54096;

    data_i = (short int*)((ni * one_size) + addri);
    data_o = (short int*)((no * one_size) + addro);

    for (i = 0; i < one_size / 2; i++, data_i++, data_o++)
    {
        *data_o = *data_i;
    }
}

void CopySPhoto(int addri, int ni, int addro, int no)
{
    int i;
    short int *data_o;
    short int *data_i; // not in STAB
    int one_size;

    one_size = 2560;

    data_i = (short int*)((ni * one_size) + addri);
    data_o = (short int*)((no * one_size) + addro);


    for (i = 0; i < one_size / 2; i++, data_i++, data_o++)
    {
        *data_o = *data_i;
    }
}

void DrawPhotoHinttex(u_int pri, int num)
{
    DISP_SPRT ds;

    CopySprDToSpr(&ds, &hintdat[num]);

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS);
    ds.pri = pri;
    ds.z = 0x0fffffff - ds.pri;
    ds.x = (int)((384 - hintdat[num].w) / 2) + 128;
    ds.y = (int)((256 - hintdat[num].h) / 2) + 80;

    DispSprD(&ds);
}

void DrawPhotoHinttex2(u_int sw, u_int pri, int num)
{
    DISP_SPRT ds;
    float f;
    float pos;
    u_char alp;
    u_char alp2;
    int time;
    int target;
    SPRT_DAT *sd;
    int max;
    SQAR_DAT sq;
    DISP_SQAR dq;
    int i;

    if (hint_rea != 0)
    {
        target = (num - 150) * 2 + monochrome_mode;

        sd = &hintreadat[target];

        max = 48;
    }
    else
    {
        target = num * 2 + monochrome_mode;

        sd = &hintdat[target];

        max = 128;
    }

    if (sw == 0)
    {
        if (plyr_wrk.ap_timer <= 100)
        {
            if (plyr_wrk.ap_timer >= 20)
            {
                time = 100 - plyr_wrk.ap_timer;

                alp = (sd->alpha * 0.25f * time) / 80.0f;
                alp2 = (time * max) / 80;
            }
            else
            {
                alp = sd->alpha / 4;
                alp2 = max;
            }
        }
    }
    else
    {
        if (plyr_wrk.ap_timer <= 100)
        {
            time = plyr_wrk.ap_timer;

            alp = (sd->alpha * 0.25f * time) / 100.0f;
            alp2 = (time * max) / 100;
        }
        else
        {
            alp = sd->alpha / 4;
            alp2 = max;
        }
    }

    sq = (SQAR_DAT){
        .w = 640,
        .h = 448,
        .x = 0,
        .y = 0,
        .pri = 0,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x80,
    };

    CopySqrDToSqr(&dq, &sq);

    dq.pri = 0x10; dq.z = 0xfffffff - dq.pri;

    dq.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    dq.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS);
    dq.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);

    dq.x[0] = 128;
    dq.x[2] = 128;

    dq.y[0] = 80;
    dq.y[1] = 80;

    dq.x[1] = 512;
    dq.x[3] = 512;

    dq.y[2] = 336;
    dq.y[3] = 336;

    {
        int i;

        for (i = 0; i < 4; i++)
        {
            dq.r[i] = 0;
            dq.g[i] = 0;
            dq.b[i] = 0;
        }
    }

    dq.alpha = alp2;

    DispSqrD(&dq);

    f = sd->alpha * 0.25f;
    pos = (f - alp) * 4.0f / f;

    MakeTim2ClutDirect4(0x1e90000, 0, -1, -1, 0);
    MakeTim2ClutDirect4(0x1e90000, 1, -1, -1, 0);

    CopySprDToSpr(&ds, sd);

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS);

    ds.pri = pri; ds.z = 0xfffffff - ds.pri;

    ds.alpha = alp;

    ds.x = (int)((384 - sd->w) / 2) + 128 - pos;
    ds.y = (int)((256 - sd->h) / 2) + 80 - pos;

    DispSprD(&ds);

    ds.x = (int)((384 - sd->w) / 2) + 128 - pos;
    ds.y = (int)((256 - sd->h) / 2) + 80 + pos;

    DispSprD(&ds);

    ds.x = (int)((384 - sd->w) / 2) + 128 + pos;
    ds.y = (int)((256 - sd->h) / 2) + 80 - pos;

    DispSprD(&ds);

    ds.x = (int)((384 - sd->w) / 2) + 128 + pos;
    ds.y = (int)((256 - sd->h) / 2) + 80 + pos;

    DispSprD(&ds);
}

void DrawPhotoEffect(u_int pri, int type)
{
    DISP_SPRT ds;
    SPRT_DAT sd = {
        .tex0 = 0,
        .u = 1,
        .v = 1,
        .w = 638,
        .h = 223,
        .x = 0,
        .y = 0,
        .pri = 0,
        .alpha = 0x80,
    };
    float sc;
    float velx;
    float vely;

    LocalCopyLtoL((sys_wrk.count & 1) * 0x8c0, 0x1a40);

    CopySprDToSpr(&ds, &sd);

    ds.pri = pri; ds.z = 0xfffffff - ds.pri;
    ds.tex0 = SCE_GS_SET_TEX0_1(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS);

    ds.texa = 0;

    ds.r = 0x80;
    ds.g = 0x80;
    ds.b = 0x80;

    if (type == 0)
    {
        ds.x = -16.0f;
        ds.y = 16.0f;

        ds.scw = 1.0f;
        ds.sch = 2.0f;

        ds.csx = -16.0f;
        ds.csy = 16.0f;

        ds.alpha = 0x40;

        DispSprD(&ds);
    }

    if (type == 1)
    {
        sc = 1.2f;

        ds.x = 0.0f;
        ds.y = 112.0f;

        ds.scw = sc;
        ds.sch = 2.4f;

        ds.csx = 320.0f;
        ds.csy = 224.0f;

        ds.alpha = 0x40;

        DispSprD(&ds);
    }

    if (type == 2)
    {
        velx = 2.0f;
        vely = 4.0f;

        ds.y = ds.x = 0.0f;

        ds.scw = 1.0f;
        ds.sch = velx;

        ds.csx = 0.0f;
        ds.csy = 0.0f;

        ds.alpha = 0x40;

        DispSprD(&ds);

        ds.x = velx;
        ds.y = vely;

        ds.scw = 1.0f;
        ds.sch = velx;

        ds.csx = velx;
        ds.csy = vely;

        ds.alpha = 0x30;

        DispSprD(&ds);

        ds.x = vely;
        ds.y = 8.0f;

        ds.csx = vely;
        ds.csy = 8.0f;

        ds.scw = 1.0f;
        ds.sch = velx;

        ds.alpha = 0x20;

        DispSprD(&ds);

        ds.x = 6.0f;
        ds.y = 12.0f;

        ds.scw = 1.0f;
        ds.sch = velx;

        ds.csx = 6.0f;
        ds.csy = 12.0f;

        ds.alpha = 0x10;

        DispSprD(&ds);
    }
}

void DrawPhotoFilter(u_int pri, int type, u_char alp)
{
    DISP_SPRT ds;
    #include "data/phtdat.h" // static SPRT_DAT phtdat[10];

    CopySprDToSpr(&ds, &phtdat[type * 2 + monochrome_mode]);

    ds.pri = pri;
    ds.z = 0xfffffff - ds.pri;

    ds.x = 128.0f;
    ds.y = 80.0f;

    if (type != 0)
    {
        ds.r = 0x80;
        ds.g = 0x80;
        ds.b = 0x80;
    }
    else
    {
        ds.r = 0x64;
        ds.g = 0x46;
        ds.b = 0x28;
    }

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
    ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS);

    ds.scw = 4.0f;
    ds.sch = 4.0f;

    ds.csx = 128.0f;
    ds.csy = 80.0f;

    ds.alpha = alp;

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0); // again ...

    DispSprD(&ds);
}

void DispPhotoFrame0()
{
    int i;
    int num1;
    int num2;

    SetSprFile2(0x1e85000, 0);

    for (i = 0; i < 4; i++)
    {
        num1 = photo_wrk.spcl_pht_no[i][0];
        num2 = photo_wrk.spcl_pht_no[i][1];

        switch(num1)
        {
        case 5:
            switch (num2)
            {
            case 0:
                DrawPhotoEffect(0x460, 2);
            break;
            case 1:
                DrawPhotoEffect(0x460, 0);
            break;
            case 2:
                DrawPhotoEffect(0x460, 1);
            break;
            }
        break;
        case 2:
            SubDeform(2, num2 * 10, 0x80);
        break;
        case 4:
            switch(num2)
            {
            case 0:
                DrawPhotoFilter(0x460, 1, 0x40);
            break;
            case 1:
                DrawPhotoFilter(0x460, 2, 0x40);
            break;
            case 2:
                DrawPhotoFilter(0x460, 3, 0x20);
            break;
            case 3:
                DrawPhotoFilter(0x460, 4, 0x20);
            break;
            case 4:
                // do nothing ...
            break;
            }
        break;
        case 3:
            SetPanel(0x460, 0.0f, 0.0f, 640.0f, 448.0f, 0, 0, 0, num2 * 12);
        break;
        case 1:
            switch (num2)
            {
            case 0:
                // do nothing ...
            break;
            case 1:
                // do nothing ...
            break;
            case 2:
                SubContrast2(180, 0x80);
            break;
            }
        break;
        case 6:
                // do nothing ...
        break;
        }
    }
}

void DispPhotoFrame00(int flag)
{
    static u_char cnt;
    static int flow = -1;
    u_char a;
    DISP_SPRT ds;
    SPRT_DAT sd = {
        .tex0 = 0,
        .u = 0,
        .v = 0,
        .w = 640,
        .h = 224,
        .x = 0,
        .y = 0,
        .pri = 0,
        .alpha = 0x80,
    };


    if (flag != 0) {
        flow = 0;
    }

    switch (flow)
    {
    case -1:
        // do nothing ...
    break;
    case 0:
        cnt = 0;

        LocalCopyLtoB2(0, (sys_wrk.count & 1) * 0x8c0);

        flow++;
    /* case fall-through */
    case 1:
        a = 0x80 - ((cnt * 128) / 0x14);

        LocalCopyBtoL(0, 0x1a40);

        CopySprDToSpr(&ds, &sd);

        ds.tex0 = SCE_GS_SET_TEX0_1(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
        ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
        ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS);
        ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);

        ds.pri = 0x80000; ds.z = 0xfffffff - ds.pri;

        ds.x = -0.5f;
        ds.y = ((sys_wrk.count + 1) & 1) * 0.5f + -1.0f;

        ds.scw = 1.0f;
        ds.sch = 2.0f;
        ds.csx = -0.5f;
        ds.csy = ((sys_wrk.count + 1) & 1) * 0.5f + -1.0f;

        ds.alpha = a;

        ds.r = 0x80;
        ds.g = 0x80;
        ds.b = 0x80;

        DispSprD(&ds);

        if (cnt >= 20)
        {
            cnt = 20;
        }
        else
        {
            cnt++;
        }
    break;
    }
}

int DispPhotoFrame1(int fl)
{
    static int time1 = 100;
    static int cnt = 0;
    static u_char alp1 = 0;
    static u_char alp2 = 0;
    static int flash_fr = 0;
    static int fl_cnt = 0;
    u_char a;
    int i;
    int j;
    int x;
    int y;
    int ret;
    int one; // not in STAB, fix a stack swap
    int pri; // not in STAB, fix a stack swap
    u_int xx; // not in STAB, fix a stack swap

    x = 128;
    y = 80;

    if (fl == 0)
    {
        time1 = plyr_wrk.sta & 1 ? 4 : 120;

        cnt = 0;
        alp1 = 0;
        alp2 = 0;

        flash_fr = plyr_wrk.sta & 1 ? 1 : 117;

        fl_cnt = flash_fr;
    }

    if (fl == 3)
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
            .alpha = 0x80,
        };

        a = 0x80 - (alp2 * 128) / 20;

        LocalCopyLtoL((sys_wrk.count & 1) * 0x8c0, 0x1a40);

        CopySprDToSpr(&ds, &sd);

        ds.tex0 = SCE_GS_SET_TEX0_1(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
        ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
        ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
        ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);

        ds.pri = 0x80000;
        ds.z = 0xfffffff - ds.pri;

        ds.x = -0.5f;
        ds.y = ((sys_wrk.count + 1) & 1) * 0.5f + -1.0f;

        ds.scw = 1.0f;
        ds.sch = 2.0f;

        ds.csx = -0.5f;
        ds.csy = (sys_wrk.count + 1 & 1) * 0.5f + -1.0f;

        ds.alpha = a;

        ds.r = 0x80;
        ds.g = 0x80;
        ds.b = 0x80;

        DispSprD(&ds);

        alp2 = (alp2 >= 20) ? 20 : alp2 + 1;

        return 3;
    }

    {{
        SQAR_DAT sq = {
            .w = 640,
            .h = 448,
            .x = 0,
            .y = 0,
            .pri = 0,
            .r = 0,
            .g = 0,
            .b = 0,
            .alpha = 0x80,
        };
        DISP_SQAR dq;

        CopySqrDToSqr(&dq, &sq);

        dq.pri = 0x3c0;
        dq.z = 0xfffffff - dq.pri;

        dq.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);

        dq.x[0] = x;
        dq.x[2] = x;

        dq.y[0] = y;
        dq.y[1] = y;

        dq.x[1] = dq.x[0] + 384;
        dq.x[3] = dq.x[0] + 384;

        dq.y[2] = dq.y[0] + 256;
        dq.y[3] = dq.y[0] + 256;

        {
            int i;

            for (i = 0; i < 4; i++)
            {
                dq.r[i] = 0;
                dq.g[i] = 0;
                dq.b[i] = 0;
            }
        }

        dq.alpha = 0x80;

        DispSqrD(&dq);

        if (hint_3d != 0)
        {
            LocalCopyBtoL(0, (sys_wrk.count & 1) * 0x8c0);
        }
        else
        {
            LocalCopyLtoL(0x2d00, (sys_wrk.count & 1) * 0x8c0);
        }

        if (fl == 1 && fl_cnt > 0)
        {
            i = SgSinf((int)(fl_cnt * 90 / flash_fr) * PI / 180.0f) * 160.0f;
            j = SgSinf((int)(fl_cnt * 90 / flash_fr) * PI / 180.0f) * 56.0f;

            SubBlur(1, j, 1.0f, 180.0f, 320.0f, 112.0f);
            SubContrast2(i, i);

            fl_cnt--;
        }
    }}

    one = 1; // HACK: fixes a stack swap

    if (fl == 2 && hint_3d != 0)
    {
        DISP_SPRT ds;
        SPRT_DAT sd = {
            .tex0 = 0,
            .u = 0,
            .v = 0,
            .w = 640,
            .h = 224,
            .x = 0x80,
            .y = 0x80,
            .pri = 0x80,
            .alpha = 0x80,
        };

        CopySprDToSpr(&ds, &sd);

        ds.tex0 = SCE_GS_SET_TEX0_1(0x2d00, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
        ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
        ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS);
        ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);

        ds.pri = 0x460;
        ds.z = 0xfffffff - ds.pri;

        ds.x = -0.5f;
        ds.y = (sys_wrk.count + 1 & one) * 0.5f + -1.0f;

        ds.scw = 1.0f;
        ds.sch = 2.0f;

        ds.csx = -0.5f;
        ds.csy = (sys_wrk.count + 1 & one) * 0.5f + -1.0f;

        ds.alpha = alp1;

        DispSprD(&ds);

        alp1 = alp1 + 4 < 0x80 ? alp1 + 4 : 0x80;
    }

    ret = alp1 >= 0x80 ? 2 : ret;

    {{
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
            .alpha = 0x80,
        };

        CopySprDToSpr(&ds, &sd);

        if (hint_3d != 0)
        {
            ds.tex0 = SCE_GS_SET_TEX0_1(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);

            LocalCopyBtoL(0, 0x1a40);
        }
        else
        {
            ds.tex0 = SCE_GS_SET_TEX0_1(0x2d00, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
        }

        ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
        ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
        ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);

        ds.pri = 0x440;
        ds.z = 0xfffffff - ds.pri;

        ds.x = -0.5f;
        ds.y = (sys_wrk.count + 1 & one) * 0.5f + -1.0f;

        ds.scw = 1.0f;
        ds.sch = 2.0f;

        ds.csx = -0.5f;
        ds.csy = (sys_wrk.count + 1 & one) * 0.5f + -1.0f;

        ds.r = 0x40;
        ds.g = 0x40;
        ds.b = 0x40;

        DispSprD(&ds);
    }}

    {{
        SQAR_DAT sq = {
            .w = 640,
            .h = 448,
            .x = 0,
            .y = 0,
            .pri = 0,
            .r = 0,
            .g = 0,
            .b = 0,
            .alpha = 0x80,
        };
        DISP_SQAR dq;

        CopySqrDToSqr(&dq, &sq);

        dq.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
        dq.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
        dq.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_ZERO, 0);

        dq.pri = 0x420;
        dq.z = 0xfffffff - dq.pri;

        {
            int i;

            for (i = 0; i < 4; i++)
            {
                dq.r[i] = 0x30;
                dq.g[i] = 0x30;
                dq.b[i] = 0x30;
            }
        }

        dq.alpha = 0xc8;

        DispSqrD(&dq);

        SubFadeFrame(0x60, 0x400);

        {
            float fh;
            float szw;
            float szh;
            float szhc;
            float fw; // not in STAB
            float zero; // not in STAB

            szw = 384.0f;
            szh = 256.0f;
            szhc = (sys_wrk.count & one) * 0.5f;

            pri = 0x3c0; // HACK: fixes a stack swap
            xx = x; // HACK: fixes a stack swap

            fh = (szw - 48.0f) / 84.0f + 2.0f;

            fw = (fh < 2.0f) ? 2.0f : fh;
            fh = (fh < 2.0f) ? 2.0f : fh;

            zero = 0;

            SetPanel(pri, x -  fw + zero, y -  fh + szhc, x + szw + fw - zero, y            + szhc, 0xcf, 0xbd, 0xa1, 0x80);
            SetPanel(pri, x -  fw + zero, y + szh - szhc, x + szw + fw - zero, y + szh + fh - szhc, 0xcf, 0xbd, 0xa1, 0x80);
            SetPanel(pri, x -  fw + zero, y       + szhc, xx           + zero, y + szh      - szhc, 0xcf, 0xbd, 0xa1, 0x80);
            SetPanel(pri, x + szw - zero, y       + szhc, x + szw + fw - zero, y + szh      - szhc, 0xcf, 0xbd, 0xa1, 0x80);
        }
    }}

    return ret; // ret is uninitialized if `fl != 3` and `alp1 < 0x80`
}

void DispPhotoFrame2()
{
    float x;
    float y;
    SQAR_DAT sq = {
        .w = 640,
        .h = 448,
        .x = 0,
        .y = 0,
        .pri = 0,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x80,
    };
    DISP_SQAR dq;
    float f25;
    float f26;

    x = 128.0F;
    y = 80.0f;

    CopySqrDToSqr(&dq, &sq);

    dq.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
    dq.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    dq.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_ZERO, 0);

    {
        int i = 0;

        for (i = 0; i < 4; i++)
        {
            dq.r[i] = 0x40;
            dq.g[i] = 0x40;
            dq.b[i] = 0x40;
        }
    }

    dq.pri = 0x420;
    dq.z = 0xfffffff - dq.pri;

    dq.alpha = 0x80;

    dq.x[0] = 0;
    dq.x[2] = 0;

    dq.y[0] = 0;
    dq.y[1] = 0;

    dq.x[1] = 640;
    dq.x[3] = 640;

    dq.y[2] = dq.y[0] + y;
    dq.y[3] = dq.y[0] + y;

    DispSqrD(&dq);

    dq.x[0] = 0;
    dq.x[2] = 0;

    dq.y[0] = y + 256;
    dq.y[1] = y + 256;

    dq.x[1] = 640;
    dq.x[3] = 640;

    dq.y[2] = dq.y[0] + (448 - (y + 256));
    dq.y[3] = dq.y[0] + (448 - (y + 256));

    DispSqrD(&dq);

    dq.x[0] = 0;
    dq.x[2] = 0;

    dq.y[0] = y;
    dq.y[1] = y;

    dq.x[1] = dq.x[0] + x;
    dq.x[3] = dq.x[0] + x;

    dq.y[2] = dq.y[0] + 256;
    dq.y[3] = dq.y[0] + 256;

    DispSqrD(&dq);

    dq.x[0] = x + 384;
    dq.x[2] = x + 384;

    dq.y[0] = y;
    dq.y[1] = y;

    dq.x[1] = dq.x[0] + (640 - (x + 384));
    dq.x[3] = dq.x[0] + (640 - (x + 384));

    dq.y[2] = dq.y[0] + 256;
    dq.y[3] = dq.y[0] + 256;

    DispSqrD(&dq);

    SubFadeFrame(0x60, 0x400);

    f26 = 2.0f;
    f25 = 6.0f;

    SetPanel(0x3c0, x       - f25 + f26, y - f25       + 1.0f, x + 384 + f25 - f26, y             + 1.0f, 0xff, 0xff, 0xff, 0x80);
    SetPanel(0x3c0, x       - f25 + f26, y       + 256 - 1.0f, x + 384 + f25 - f26, y + 256 + f25 - 1.0f, 0xff, 0xff, 0xff, 0x80);
    SetPanel(0x3c0, x       - f25 + f26, y             + 1.0f, x             + f26, y + 256       - 1.0f, 0xff, 0xff, 0xff, 0x80);
    SetPanel(0x3c0, x + 384       - f26, y             + 1.0f, x + 384 + f25 - f26, y + 256       - 1.0f, 0xff, 0xff, 0xff, 0x80);
}

void DispPhotoFrame2_2(int type)
{
    float x;
    float y;
    SQAR_DAT sq = {
        .w = 640,
        .h = 448,
        .x = 0,
        .y = 0,
        .pri = 0,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x80,
    };
    DISP_SQAR dq;

    x = 128.0f;
    y = 80.0f;

    CopySqrDToSqr(&dq, &sq);

    dq.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
    dq.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    dq.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);

    {
        int i;

        for (i = 0; i < 4; i++)
        {
            dq.r[i] = 0;
            dq.g[i] = 0;
            dq.b[i] = 0;
        }
    }

    dq.pri = 0x420;
    dq.z = 0xfffffff - dq.pri;

    dq.alpha = 0x80;

    dq.x[0] = 0;
    dq.x[2] = 0;

    dq.y[0] = 0;
    dq.y[1] = 0;

    dq.x[1] = 640;
    dq.x[3] = 640;

    dq.y[2] = dq.x[0] + y;
    dq.y[3] = dq.x[0] + y;

    DispSqrD(&dq);

    dq.x[0] = 0;
    dq.x[2] = 0;

    dq.y[0] = y + 256;
    dq.y[1] = y + 256;

    dq.x[1] = 640;
    dq.x[3] = 640;

    dq.y[2] = dq.y[0] + (448 - (y + 256));
    dq.y[3] = dq.y[0] + (448 - (y + 256));

    DispSqrD(&dq);

    dq.x[0] = 0;
    dq.x[2] = 0;

    dq.y[0] = y;
    dq.y[1] = y;

    dq.x[1] = x + dq.x[0];
    dq.x[3] = x + dq.x[0];

    dq.y[2] = dq.y[0] + 256;
    dq.y[3] = dq.y[0] + 256;

    DispSqrD(&dq);

    dq.x[0] = x + 384;
    dq.x[2] = x + 384;

    dq.y[0] = y;
    dq.y[1] = y;

    dq.x[1] = dq.x[0] + (640 - (x + 384));
    dq.x[3] = dq.x[0] + (640 - (x + 384));

    dq.y[2] = dq.y[0] + 256;
    dq.y[3] = dq.y[0] + 256;

    DispSqrD(&dq);

    SetNegaFilter(type == 0 ? 0 : (type == 1 ? 3 : ers_type));

    SubFadeFrame(0x60, 0x400);

    {
        float szw;
        float szh;
        float f27;
        float f21;

        szw = 384.0f;
        szh = 256.0f;

        f27 = 2.0f;
        f21 = 6.0f;

        SetPanel(0x3c0, x - f21 + f27, y - f21 + 1.0f, x + szw + f21 - f27, y             + 1.0f, 0xff, 0xff, 0xff, 0x80);
        SetPanel(0x3c0, x - f21 + f27, y + szh - 1.0f, x + szw + f21 - f27, y + szh + f21 - 1.0f, 0xff, 0xff, 0xff, 0x80);
        SetPanel(0x3c0, x - f21 + f27, y       + 1.0f, x             + f27, y + szh       - 1.0f, 0xff, 0xff, 0xff, 0x80);
        SetPanel(0x3c0, x + szw - f27, y       + 1.0f, x + szw + f21 - f27, y + szh       - 1.0f, 0xff, 0xff, 0xff, 0x80);
    }
}

void DispPhotoFrame2_3(int type)
{
    float x;
    float y;

    x = 128.0f;
    y = 80.0f;

    {{
        DISP_SPRT ds;
        SPRT_DAT sd = {
            .tex0 = 0,
            .u = 0,
            .v = 0,
            .w = 640,
            .h = 224,
            .x = 0,
            .y = 0,
            .pri = 0,
            .alpha = 0x80,
        };

        LocalCopyBtoL(0, 0x1a40);

        CopySprDToSpr(&ds, &sd);

        ds.tex0 = SCE_GS_SET_TEX0_1(0x1a40, 10, SCE_GS_PSMCT32, 10, 8, 0, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);
        ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);
        ds.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS);
        ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);

        ds.r = 0x80;
        ds.g = 0x80;
        ds.b = 0x80;

        ds.pri = 0x430;
        ds.z = 0xfffffff - ds.pri;

        ds.alpha = 0x80;


        ds.x = 0.0f;
        ds.y = 0.0f;

        ds.u = 0;
        ds.v = 0;

        ds.w = 640;
        ds.h = 40;

        ds.scw = 1.0f;
        ds.sch = 2.0f;

        ds.csx = 0.0f;
        ds.csy = 0.0f;

        DispSprD(&ds);

        ds.x = 0.0f;
        ds.y = 336.0f;

        ds.u = 0;
        ds.v = 168;

        ds.w = 640;
        ds.h = 56;

        ds.scw = 1.0f;
        ds.sch = 2.0f;

        ds.csx = 0.0f;
        ds.csy = 336.0f;

        DispSprD(&ds);

        ds.x = 0.0f;
        ds.y = y;

        ds.u = 0;
        ds.v = 40;

        ds.w = 128;
        ds.h = 128;

        ds.scw = 1.0f;
        ds.sch = 2.0f;

        ds.csx = 0.0f;
        ds.csy = 80.0f;

        DispSprD(&ds);

        ds.x = 512.0f;
        ds.y = y;

        ds.u = 512;
        ds.v = 40;

        ds.w = 128;
        ds.h = 128;

        ds.scw = 1.0f;
        ds.sch = 2.0f;

        ds.csx = 512.0f;
        ds.csy = 80.0f;

        DispSprD(&ds);
    }}

    {{
        SQAR_DAT sq = {
            .w = 640,
            .h = 448,
            .x = 0,
            .y = 0,
            .pri = 0,
            .r = 0,
            .g = 0,
            .b = 0,
            .alpha = 0x80,
        };
        DISP_SQAR dq;

        CopySqrDToSqr(&dq, &sq);

        dq.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
        dq.test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
        dq.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_ZERO, 0);

        {
            int i;

            for (i = 0;
        i < 4;
        i++)
            {
                dq.r[i] = 0x30;
                dq.g[i] = 0x30;
                dq.b[i] = 0x30;
            }
        }

        dq.pri = 0x420;
        dq.z = 0xfffffff - dq.pri;

        dq.alpha = 0x80;

        dq.x[0] = 0;
        dq.x[2] = 0;

        dq.y[0] = 0;
        dq.y[1] = 0;

        dq.x[1] = 640;
        dq.x[3] = 640;

        dq.y[2] = dq.x[0] + y;
        dq.y[3] = dq.x[0] + y;

        DispSqrD(&dq);

        dq.x[0] = 0;
        dq.x[2] = 0;

        dq.y[0] = y + 256;
        dq.y[1] = y + 256;

        dq.x[1] = 640;
        dq.x[3] = 640;

        dq.y[2] = dq.y[0] + (448 - (y + 256));
        dq.y[3] = dq.y[0] + (448 - (y + 256));

        DispSqrD(&dq);

        dq.x[0] = 0;
        dq.x[2] = 0;

        dq.y[0] = y;
        dq.y[1] = y;

        dq.x[1] = x + dq.x[0];
        dq.x[3] = x + dq.x[0];

        dq.y[2] = dq.y[0] + 256;
        dq.y[3] = dq.y[0] + 256;

        DispSqrD(&dq);

        dq.x[0] = x + 384;
        dq.x[2] = x + 384;

        dq.y[0] = y;
        dq.y[1] = y;

        dq.x[1] = dq.x[0] + (640 - (x + 384));
        dq.x[3] = dq.x[0] + (640 - (x + 384));

        dq.y[2] = dq.y[0] + 256;
        dq.y[3] = dq.y[0] + 256;

        DispSqrD(&dq);

        SetNegaFilter(type == 0 ? 0 : (type == 1 ? 3 : ers_type));

        SubFadeFrame(0x60, 0x400);

        {
            float szw;
            float szh;
            float f27;
            float f21;

            szw = 384.0f;
            szh = 256.0f;

            f27 = 2.0f;
            f21 = 6.0f;

            SetPanel(0x3c0, x - f21 + f27, y - f21 + 1.0f, x + szw + f21 - f27, y             + 1.0f, 0xff, 0xff, 0xff, 0x80);
            SetPanel(0x3c0, x - f21 + f27, y + szh - 1.0f, x + szw + f21 - f27, y + szh + f21 - 1.0f, 0xff, 0xff, 0xff, 0x80);
            SetPanel(0x3c0, x - f21 + f27, y       + 1.0f, x             + f27, y + szh       - 1.0f, 0xff, 0xff, 0xff, 0x80);
            SetPanel(0x3c0, x + szw - f27, y       + 1.0f, x + szw + f21 - f27, y + szh       - 1.0f, 0xff, 0xff, 0xff, 0x80);
        }
    }}
}

void SetNegaFilter(int type)
{
    float CHANGE_TIME;
    float x;
    float y;
    float y11;
    float y12;
    float y21;
    float y22;
    float f;
    int endf;

    CHANGE_TIME = 1.0f;

    endf = 58;

    x = 128.0f;
    y = 80.0f;

    switch(type)
    {
    case 0:
    case 3:
        // do nothing ...
    break;
    case 1:
    case 2:
        if (plyr_wrk.ap_timer >= 60)
        {
            y11 = y;
            y12 = 0.0f;
            y21 = y + 256.0f;
            y22 = 0.0f;
        }
        else if (plyr_wrk.ap_timer > endf)
        {
            if (type == 1)
            {
                f = 128.0f - (plyr_wrk.ap_timer - endf) * 128 / CHANGE_TIME;

                y11 = y + 128.0f;
                y12 = -f;
                y21 = y + 128.0f;
                y22 = f;
            }
            else
            {
                f = 128.0f - (plyr_wrk.ap_timer - endf) * 128 / CHANGE_TIME;

                y11 = y;
                y12 = f;
                y21 = y + 256.0f;
                y22 = -f;
            }
        }
        else
        {
            y11 = y;
            y12 = 128.0f;
            y21 = y + 256.0f;
            y22 = -y12;
        }
    break;
    case 4:
    case 5:
        if (plyr_wrk.ap_timer >= 60)
        {
            y11 = y;
            y12 = 128.0f;
            y21 = y + 256.0f;
            y22 = -y12;
        }
        else if (plyr_wrk.ap_timer > endf)
        {
            if (type == 4)
            {
                f = (plyr_wrk.ap_timer - endf) * 128 / CHANGE_TIME;

                y11 = y + 128.0f;
                y12 = -f;
                y21 = y + 128.0f;
                y22 = f;
            }
            else
            {
                f = (plyr_wrk.ap_timer - endf) * 128 / CHANGE_TIME;

                y11 = y;
                y12 = f;
                y21 = y + 256.0f;
                y22 = -f;
            }
        }
        else
        {
            y11 = y;
            y12 = 0.0f;
            y21 = y + 256.0f;
            y22 = 0.0f;
        }
    break;
    }

    if (CHANGE_TIME) {} // HACK: fixes codegen

    switch(type)
    {
    case 0:
        // do nothing ...
    break;
    case 3: {{
        SQAR_DAT sq = {
            .w = 640,
            .h = 448,
            .x = 0,
            .y = 0,
            .pri = 0,
            .r = 0,
            .g = 0,
            .b = 0,
            .alpha = 0x80,
        };
        DISP_SQAR dq;

        CopySqrDToSqr(&dq, &sq);

        dq.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_ZERO, 0);

        {
            /* a1 5 */
            int i;

            for (i = 0; i < 4; i++)
            {
                dq.r[i] = 0x60;
                dq.g[i] = 0x60;
                dq.b[i] = 0x60;
            }
        }

        dq.pri = 0x430;
        dq.z = 0xfffffff - dq.pri;

        dq.x[0] = x;
        dq.x[2] = x;

        dq.y[0] = y;
        dq.y[1] = y;

        dq.x[1] = dq.x[0] + 384;
        dq.x[3] = dq.x[0] + 384;

        dq.y[2] = dq.y[0] + 256;
        dq.y[3] = dq.y[0] + 256;

        DispSqrD(&dq);
    }}
    break;
    case 1:
    case 2:
    case 4:
    case 5: {{
        SQAR_DAT sq = {
            .w = 640,
            .h = 448,
            .x = 0,
            .y = 0,
            .pri = 0,
            .r = 0,
            .g = 0,
            .b = 0,
            .alpha = 0x80,
        };
        DISP_SQAR dq;

        CopySqrDToSqr(&dq, &sq);

        dq.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_ZERO, 0);

        {
            /* a1 5 */
            int i;

            for (i = 0; i < 4; i++)
            {
                dq.r[i] = 0x60;
                dq.g[i] = 0x60;
                dq.b[i] = 0x60;
            }
        }

        dq.pri = 0x420;
        dq.z = 0xfffffff - dq.pri;

        if (y12 != 0.0f)
        {
            dq.x[0] = x;
            dq.x[2] = x;

            dq.y[0] = y11;
            dq.y[1] = y11;

            dq.y[2] = dq.y[0] + y12;
            dq.y[3] = dq.y[0] + y12;

            dq.x[1] = dq.x[0] + 384;
            dq.x[3] = dq.x[0] + 384;

            DispSqrD(&dq);
        }

        if (y22 != 0.0f)
        {
            dq.x[0] = x;
            dq.x[2] = x;

            dq.y[0] = y21;
            dq.y[1] = y21;

            dq.y[2] = dq.y[0] + y22;
            dq.y[3] = dq.y[0] + y22;

            dq.x[1] = dq.x[0] + 384;
            dq.x[3] = dq.x[0] + 384;

            DispSqrD(&dq);
        }
    }}
    break;
    }
}

void SetFurnAttrEve(int id, u_short flg, int on_flg);
void PhotoMake()
{
    int num2;

    if (plyr_wrk.mode != 1)
    {
        return;
    }

    switch(photo_wrk.mode)
    {
    case 4:
        photo_wrk.adr_no = GetSavePhotoNo();
    break;
    case 5:
        if (hint_3d != 0)
        {
            if (pic_save_flag != 0)
            {
                DispPhotoFrame0();

                if (pic_save_flag == 1)
                {
                    stop_effects = pic_save_flag;

                    LocalCopyLtoB2(0, ((sys_wrk.count + 1) & 1) * 0x8c0);
                    LocalCopyLtoL(((sys_wrk.count + 1) & 1) * 0x8c0, (sys_wrk.count & 1) * 0x8c0);

                    FModeScreenEffect();

                    furn_disp_flg = 0;
                    pic_save_flag = 2;
                }
                else if (pic_save_flag == 2)
                {
                    LocalCopyLtoL((sys_wrk.count & 1) * 0x8c0, 0x2d00);
                    DispPhotoFrame1(0);

                    pic_save_flag = 0;
                    stop_effects = 0;

                    ingame_wrk.stts |= 0x20;
                }
            }
            else
            {
                DispPhotoFrame1(1);
            }
        }
        else
        {
            if (pic_save_flag == 1)
            {
                DispPhotoFrame0();
                LocalCopyLtoL((sys_wrk.count & 1) * 0x8c0, 0x2d00);
                DispPhotoFrame1(0);
                DispPhotoFrame1(1);

                pic_save_flag = 0;

                ingame_wrk.stts |= 0x20;
            }
            else
            {
                DispPhotoFrame1(1);

                if (hint_2d != 0)
                {
                    break;
                }

                if (plyr_wrk.sta & 0x100 || plyr_wrk.sta & 1)
                {
                    break;
                }

                if (pfile_wrk.pic[pfile_wrk.pic_num - 1].subject[0][0] != 0)
                {
                    STR_DAT sd;
                    DISP_STR ds;
                    int num;

                    sd.str = GetSubjectNameAddr(
                        pfile_wrk.pic[pfile_wrk.pic_num - 1].subject[0][0],
                        pfile_wrk.pic[pfile_wrk.pic_num - 1].subject[0][1],
                        pfile_wrk.pic[pfile_wrk.pic_num - 1].msn_no);

                    num = GetStrWidth(sd.str);

                    CopyStrDToStr(&ds, &sd);

                    ds.pos_x = 320 - num / 2;
                    ds.pos_y = 360;

                    ds.r = 0x80;
                    ds.g = 0x80;
                    ds.b = 0x80;

                    ds.alpha = 0x80;

                    ds.pri = 0xa0;

                    SetMessageV2(&ds);
                }
            }
        }
    break;
    case 6:
        DispPhotoFrame1(2);

        if (hint_2d != 0)
        {
            num2 = hint_2d_num;
        }
        else if (hint_3d != 0)
        {
            num2 = hint_3d_num;
        }
        else
        {
            num2 = 0;
        }

        if (hint_2d != 0)
        {
            DrawPhotoHinttex2(0, 0x470, hint_2d_num);
        }

        if (hint_rea != 0)
        {
            if ((plyr_wrk.sta & 0x100) == 0 && (plyr_wrk.sta & 1) == 0)
            {
                if (pfile_wrk.pic[pfile_wrk.pic_num - 1].subject[0][0] != 0)
                {
                    STR_DAT sd;
                    DISP_STR ds;
                    int num;

                    sd.str = GetSubjectNameAddr(
                        pfile_wrk.pic[pfile_wrk.pic_num - 1].subject[0][0],
                        pfile_wrk.pic[pfile_wrk.pic_num - 1].subject[0][1],
                        pfile_wrk.pic[pfile_wrk.pic_num - 1].msn_no);

                    num = GetStrWidth(sd.str);

                    CopyStrDToStr(&ds, &sd);

                    ds.pos_x = 320 - num / 2;
                    ds.pos_y = 360;

                    ds.r = 0x80;
                    ds.g = 0x80;
                    ds.b = 0x80;

                    ds.alpha = 0x80;

                    ds.pri = 0xa0;

                    SetMessageV2(&ds);
                }
            }
        }

        if (plyr_wrk.ap_timer != 1)
        {
            break;
        }

        if (hint_rea == 0)
        {
            u_char *strp;
            static int fl = 0;

            if (hint_2d != 0)
            {
                poss_file[1][num2] = 2;
            }

            if (num2 < 100)
            {
                strp = (u_char *)GetIngameMSGAddr(IGMSG_HINT_2D_MSG1, num2);
            }
            else if (num2 - 100 >= 0 && num2 - 100 < 0x32)
            {
                strp = (u_char *)GetIngameMSGAddr(IGMSG_HINT_3D_MSG1, num2 - 100);
            }
            else
            {
                strp = (u_char *)GetIngameMSGAddr(IGMSG_HINT_SP_MSG1, num2 - 200);
            }

            if (fl == 0)
            {
                fl = 1;

                SetMessageV3_2(strp, 0x50);
            }

            if (fl != 0)
            {
                SetMessageV3_2(strp, 0x50);

                if (MesStatusCheck() == 0)
                {
                    fl = 0;
                }
            }
        }
    break;
    case 7:
        DispPhotoFrame1(2);
        DrawPhotoHinttex2(1, 0x470, hint_2d_num);
    break;
    case 8:
        LocalCopyLtoL((sys_wrk.count & 1) * 0x8c0, 0x1a40);
        TakePhotoFromScreen();
        CompPhotoFromWorkArea(photo_wrk.adr_no);
        WaitFrameTop(1);
        MakeSPhotoFromWorkArea(photo_wrk.adr_no);
        WaitFrameTop(0);
        LocalCopyLtoL(0x1a40, ((sys_wrk.count + 1) & 1) * 0x8c0);
        DrawAll2D();
        DispPhotoFrame1(2);
        SetBlurOff();

        ingame_wrk.stts &= ~0x20;

        DispPhotoFrame00(1);

        if (hint_rea != 0)
        {
            SetFurnAttrEve(hint_2d_num + 227, 0x20, 0);
            SetFurnAttrEve(hint_2d_num + 227, 0x80, 0);

            FurnActOffEve(hint_2d_num + 227);
        }
    break;
    case 9:
        // do nothing ...
    break;
    }
}

int hint_2d = 0;
int hint_rea = 0;
int hint_3d = 0;
int hint_2d_num = 0;
int hint_3d_num = 0;
int pic_save_flag = 0;
int pht_finder_off = 0;

void PhotoMake_EneDead()
{
    int i;
    int num1;
    int num2;

    if (plyr_wrk.mode != 1)
    {
        return;
    }

    switch(photo_wrk.mode)
    {
    case 4:
        photo_wrk.adr_no = GetSavePhotoNo();

        stop_effects = 1;

        FModeScreenEffect();
    break;
    case 5:
        if (pic_save_flag != 0)
        {
            LocalCopyLtoB2(0, ((sys_wrk.count + 1) & 1)  * 0x8c0);
            SetWhiteIn2(1);

            pic_save_flag = 0;
        }

        FModeScreenEffect();
        DispPhotoFrame0();

        switch (frm_type)
        {
        case 0:
            DispPhotoFrame2();
        break;
        case 1:
            DispPhotoFrame2_2(-1);
        break;
        case 2:
            DispPhotoFrame2_3(-1);
        break;
        }
    break;
    case 6:
        FModeScreenEffect();
        DispPhotoFrame0();

        if (hint_2d != 0)
        {
            for (i = 0; i < 4; i++)
            {
                num1 = photo_wrk.spcl_pht_no[i][0];
                num2 = photo_wrk.spcl_pht_no[i][1];

                if ((num2 >= 0 && num2 < 100) && num1 == 6)
                {
                    DrawPhotoHinttex2(0, 0x470, num2);
                }
            }
        }

        switch (frm_type)
        {
        case 0:
            DispPhotoFrame2();
        break;
        case 1:
            DispPhotoFrame2_2(1);
        break;
        case 2:
            DispPhotoFrame2_3(1);
        break;
        }
    break;
    case 8:
        FModeScreenEffect();
        LocalCopyLtoL((sys_wrk.count & 1)  * 0x8c0, 0x1a40);
        TakePhotoFromScreen();
        CompPhotoFromWorkArea(photo_wrk.adr_no);
        MakeSPhotoFromWorkArea(photo_wrk.adr_no);
        LocalCopyLtoL(0x1a40, ((sys_wrk.count + 1) & 1)  * 0x8c0);
        DrawAll2D();
        FModeScreenEffect();
        DispPhotoFrame0();

        switch (frm_type)
        {
        case 0:
            DispPhotoFrame2();
        break;
        case 1:
            DispPhotoFrame2_2(1);
        break;
        case 2:
            DispPhotoFrame2_3(1);
        break;
        }

        SetBlurOff();
    break;
    case 9:
        stop_effects = 0;
    break;
    }
}

void PhotoMakeSaveInit()
{
    pic_save_flag = 0;
}

void DrawPicture(int pri, int addr, int n, float x, float y, float szw, float szh, u_char alp)
{
    DrawPhotoFromCompress(n, pri, 1, x, y, szw, szh, 128);
}

void CopyPicture(int addri, int ni, int addro, int no)
{
    CopyPhoto(addri, ni, addro, no);
}

void ReqSavePicture()
{
    pic_save_flag = 1;
}

void PhotoMakeSave()
{
    PhotoMake();
}

void PhotoMakeSave2()
{

    if (plyr_wrk.mode == PMODE_FINDER)
    {

        switch (photo_wrk.mode)
        {

        case 8:
            RankingChkMem(pfile_wrk.pic[pfile_wrk.pic_num - 1]);

            ReSetGhostList(pfile_wrk.pic[pfile_wrk.pic_num - 1]);
            asm(""); // what the ???
        break;
        case 9:
            FModeScreenEffect();
            DispPhotoFrame00(0);
            asm(""); // what the ???
        break;
        }
    }
}
