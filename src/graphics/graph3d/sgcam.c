#include "common.h"
#include "typedefs.h"
#include "sgcam.h"

// gcc/src/newlib/libm/math/sf_tan.c
float tanf(float x);

#include "ee/eeregs.h"
#include "ee/eestruct.h"
#include "sce/libvu0.h"

#include "graphics/graph3d/sgsu.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/libsg.h"
#include "graphics/graph3d/sgdma.h"

sceVu0FVECTOR clip_volume = {1920.0f, 896.0f, 256.0f, 16777000.0f};
static sceVu0FVECTOR clip_volumev = {320.0f, 112.0f, 0.1f, 16777000.0f};

#define SCRATCHPAD ((u_char *)0x70000000)

void SetViewScreenClipMatrix(SgCAMERA *camera, float scrz)
{
    float az;
    float cz;
    float gsx;
    float gsy;
    float gsxv;
    float gsyv;
    float rscrz;
    float zmin;
    float zmax;
    float farz;
    float nearz;
    sceVu0FMATRIX mt;

    zmin = camera->zmin;
    zmax = camera->zmax;
    farz = camera->farz;
    nearz = camera->nearz;

    rscrz = nearz / scrz;

    gsx = clip_volume[0] * rscrz;
    gsy = clip_volume[1] * rscrz;

    gsxv = clip_volumev[0] * rscrz;
    gsyv = clip_volumev[1] * rscrz;

    cz = (-zmax * nearz + zmin * farz) / (-nearz + farz);
    az = (farz * nearz) * (-zmin + zmax)/ (-nearz + farz);

    sceVu0UnitMatrix(camera->vs);

    camera->vs[0][0] = scrz;
    camera->vs[1][1] = scrz;
    camera->vs[2][2] = 0.0f;
    camera->vs[2][3] = camera->vs[3][2] = 1.0f;
    camera->vs[3][3] = 0.0f;

    sceVu0UnitMatrix(mt);

    mt[0][0] = camera->ax;
    mt[1][1] = camera->ay;
    mt[2][2] = az;

    mt[3][0] = camera->cx;
    mt[3][1] = camera->cy;
    mt[3][2] = cz;

    sceVu0MulMatrix(camera->vs, mt, camera->vs);

    sceVu0UnitMatrix(camera->vc);

    camera->vc[0][0] = ((nearz + nearz) * camera->ax) / (gsx + gsx);
    camera->vc[1][1] = ((nearz + nearz) * camera->ay) / (gsy + gsy);
    camera->vc[2][2] = (farz + nearz) / (farz - nearz);
    camera->vc[2][3] = 1.0f;
    camera->vc[3][2] = ((farz * nearz) * -2.0f) / (farz - nearz);
    camera->vc[3][3] = 0.0f;

    sceVu0UnitMatrix(camera->vcv);

    camera->vcv[0][0] = ((nearz + nearz) * camera->ax) / (gsxv + gsxv);
    camera->vcv[1][1] = ((nearz + nearz) * camera->ay) / (gsyv + gsyv);
    camera->vcv[2][2] = (farz + nearz) / (farz - nearz);
    camera->vcv[3][2] = ((farz * nearz) * -2.0f) / (farz - nearz);
    camera->vcv[2][3] = 1.0f;
    camera->vcv[3][3] = 0.0f;
}

void SgSetRefCamera(SgCAMERA *camera)
{
    sceVu0FMATRIX tmpmat;
    sceVu0FMATRIX clip_screen;
    float scrz;

    nowcamera = camera;

    scrz = clip_volumev[1] / tanf(camera->fov * 0.5f) * 2;

    sceVu0SubVector(camera->zd,camera->i,camera->p);
    sceVu0SubVector(ieye,camera->p,camera->i);

    camera->yd[0] = 0.0f;
    camera->yd[1] = -1.0f;
    camera->yd[2] = 0.0f;
    camera->yd[3] = 0.0f;

    sceVu0UnitMatrix(tmpmat);
    sceVu0RotMatrixZ(tmpmat,tmpmat,-camera->roll);
    sceVu0ApplyMatrix(camera->yd,tmpmat,camera->yd);
    SetViewScreenClipMatrix(camera,scrz);
    sceVu0CameraMatrix(camera->wv,camera->p,camera->zd,camera->yd);
    sceVu0MulMatrix(camera->ws,camera->vs,camera->wv);
    sceVu0MulMatrix(camera->wc,camera->vc,camera->wv);
    sceVu0MulMatrix(camera->wcv,camera->vcv,camera->wv);

    SgSetWsMtx(camera->ws);
    SgSetClipMtx(camera->wc);
    SgSetClipVMtx(camera->wcv);
}

void SetViewScreenClipMatrixOrtho(SgCAMERA *camera, float scrz)
{
    float az;
    float cz;
    float zmin;
    float zmax;
    float farz;
    float nearz;
    /* 0x0(sp) */sceVu0FMATRIX mt;
    float f24, f25, f26, f27; // not in STAB

    zmin = camera->zmin;
    zmax = camera->zmax;
    farz = camera->farz;
    nearz = camera->nearz;

    f24 = clip_volume[0];
    f25 = clip_volume[1];

    f26 = clip_volumev[0];
    f27 = clip_volumev[1];

    cz = ((-zmax * nearz) + (+zmin * farz)) / (-nearz + farz);
    az = ((+farz * nearz) * (-zmin + zmax)) / (-nearz + farz);

    sceVu0UnitMatrix(camera->vs);
    sceVu0UnitMatrix(mt);

    mt[0][0] = camera->ax;
    mt[1][1] = camera->ay;
    mt[2][2] = az;

    mt[3][0] = camera->cx;
    mt[3][1] = camera->cy;
    mt[3][2] = cz;

    sceVu0MulMatrix(camera->vs, mt, camera->vs);
    sceVu0UnitMatrix(camera->vc);

    camera->vc[0][0] = camera->ax / f24;
    camera->vc[1][1] = camera->ay / f25;
    camera->vc[2][2] = -2.0f / (farz - nearz);
    camera->vc[3][2] = (farz + nearz) / (farz - nearz);

    sceVu0UnitMatrix(camera->vcv);

    camera->vcv[0][0] = camera->ax / f26;
    camera->vcv[1][1] = camera->ay / f27;
    camera->vcv[2][2] = -2.0f / (farz - nearz);
    camera->vcv[3][2] = (farz + nearz) / (farz - nearz);
}

void SgSetRefCameraOrtho(SgCAMERA *camera)
{
    sceVu0FMATRIX tmpmat;
    sceVu0FMATRIX clip_screen;
    float scrz;

    nowcamera = camera;

    scrz = (clip_volumev[1] / tanf(camera->fov * 0.5f)) * 2;

    sceVu0SubVector(camera->zd, camera->i, camera->p);
    sceVu0SubVector(ieye, camera->p, camera->i);

    camera->yd[0] = 0.0f;
    camera->yd[1] = -1.0f;
    camera->yd[2] = 0.0f;
    camera->yd[3] = 0.0f;

    sceVu0UnitMatrix(tmpmat);
    sceVu0RotMatrixZ(tmpmat, tmpmat, -camera->roll);
    sceVu0ApplyMatrix(camera->yd, tmpmat, camera->yd);

    SetViewScreenClipMatrixOrtho(camera, scrz);

    sceVu0CameraMatrix(camera->wv, camera->p, camera->zd, camera->yd);
    sceVu0MulMatrix(camera->ws, camera->vs, camera->wv);
    sceVu0MulMatrix(camera->wc, camera->vc, camera->wv);
    sceVu0MulMatrix(camera->wcv, camera->vcv, camera->wv);

    SgSetWsMtx(camera->ws);
    SgSetClipMtx(camera->wc);
    SgSetClipVMtx(camera->wcv);
}

void SetClipValue(float minx, float maxx, float miny, float maxy)
{
    clip_value[0] = minx;
    clip_value[1] = maxx;
    clip_value[2] = miny;
    clip_value[3] = maxy;

    if (minx == -1.0f && maxx == 1.0f && miny == -1.0f && maxy == 1.0f)
    {
        clip_value_check = 0;
    }
    else
    {
        clip_value_check = 1;
    }
}

void printClipValue()
{
    printf("clip_value_check %d\n", clip_value_check);

    printVectorC(clip_value, "clip_value");
}

int BoundClip(sceVu0FVECTOR ed, sceVu0FVECTOR v)
{
    int ret;

    asm volatile("                             \n\
        lqc2            $vf12, 0(%1)           \n\
        vmulax.xyzw     ACC,   $vf4,   $vf12x  \n\
        vmadday.xyzw    ACC,   $vf5,   $vf12y  \n\
        vmaddaz.xyzw    ACC,   $vf6,   $vf12z  \n\
        vmaddw.xyzw     $vf14, $vf7,   $vf0w   \n\
        vmulax.xyzw     ACC,   $vf8,   $vf12x  \n\
        vmadday.xyzw    ACC,   $vf9,   $vf12y  \n\
        vmaddaz.xyzw    ACC,   $vf10,  $vf12z  \n\
        vmaddw.xyzw     $vf13, $vf11,  $vf0w   \n\
        vclipw.xyz      $vf14, $vf14w          \n\
        vnop                                   \n\
        vnop                                   \n\
        vnop                                   \n\
        vnop                                   \n\
        vnop                                   \n\
        sqc2            $vf13, 0(%2)           \n\
        cfc2            %0,    $vi18           \n\
        ":"=r"(ret):"r"(v),"r"(ed)
    );

    return ret;
}

int BoundClipQ(sceVu0FVECTOR ed, sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
    int ret;

    asm volatile("                             \n\
        lqc2            $vf12, 0(%1)           \n\
        vmulax.xyzw     ACC,   $vf4,   $vf12x  \n\
        vmadday.xyzw    ACC,   $vf5,   $vf12y  \n\
        vmaddaz.xyzw    ACC,   $vf6,   $vf12z  \n\
        vmaddw.xyzw     $vf14, $vf7,   $vf0w   \n\
        vmulax.xyzw     ACC,   $vf8,   $vf12x  \n\
        vmadday.xyzw    ACC,   $vf9,   $vf12y  \n\
        vmaddaz.xyzw    ACC,   $vf10,  $vf12z  \n\
        vdiv            Q,     $vf0w,  $vf14w  \n\
        vclipw.xyz      $vf14, $vf14w          \n\
        vmaddw.xyzw     $vf13, $vf11,  $vf0w   \n\
        vwaitq                                 \n\
        vmulq.xyz       $vf14, $vf14,  Q       \n\
        sqc2            $vf13, 0(%2)           \n\
        sqc2            $vf14, 0(%3)           \n\
        cfc2            %0,    $vi18           \n\
        ":"=r"(ret):"r"(v1),"r"(ed),"r"(v0)
    );

    return ret;
}

void printBoundingBox(u_int *prim)
{
    sceVu0FVECTOR tmpvec[8];

    printVectorC(*(sceVu0FVECTOR *)&prim[4], "min");
    printVectorC(*(sceVu0FVECTOR *)&prim[32], "max");

    if (0)
    {
        // this string is part of .rodata but it's not
        // referenced anywhere. this looks like a
        // reasonable place for it, as it may have
        // been used to debug min and/or max vectors.
        printf("%d:%x %x %x %x\n");
    }
}

SgCAMERA *nowcamera = NULL;

int ClipCheck(sceVu0FVECTOR *vec)
{
    int ret;

    asm volatile("\n\
        lqc2          $vf12, 0(%1)       \n\
        lqc2          $vf13, 0x10(%1)    \n\
        lqc2          $vf14, 0x20(%1)    \n\
        lqc2          $vf15, 0x30(%1)    \n\
        vclipw.xyz    $vf12, $vf12w      \n\
        vclipw.xyz    $vf13, $vf13w      \n\
        vclipw.xyz    $vf14, $vf14w      \n\
        vclipw.xyz    $vf15, $vf15w      \n\
        vnop                             \n\
        vnop                             \n\
        vnop                             \n\
        vnop                             \n\
        vnop                             \n\
        cfc2          %0,    $vi18       \n\
        ":"=r"(ret):"r"(*vec)
    );

    return ret;
}

// similar to: void _SetMulMatrixBB(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FMATRIX lw);
static inline void asm_1__CheckBoundingBox(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FMATRIX lw)
{
    asm volatile ("                              \n\
        lqc2            $vf12, 0(%0)             \n\
        lqc2            $vf13, 0x10(%0)          \n\
        lqc2            $vf14, 0x20(%0)          \n\
        lqc2            $vf15, 0x30(%0)          \n\
        lqc2            $vf16, 0(%1)             \n\
        lqc2            $vf17, 0x10(%1)          \n\
        lqc2            $vf18, 0x20(%1)          \n\
        lqc2            $vf19, 0x30(%1)          \n\
        vmulax.xyzw     ACC,   $vf12,    $vf16x  \n\
        vmadday.xyzw    ACC,   $vf13,    $vf16y  \n\
        vmaddaz.xyzw    ACC,   $vf14,    $vf16z  \n\
        vmaddw.xyzw     $vf4,  $vf15,    $vf16w  \n\
        vmulax.xyzw     ACC,   $vf12,    $vf17x  \n\
        vmadday.xyzw    ACC,   $vf13,    $vf17y  \n\
        vmaddaz.xyzw    ACC,   $vf14,    $vf17z  \n\
        vmaddw.xyzw     $vf5,  $vf15,    $vf17w  \n\
        vmulax.xyzw     ACC,   $vf12,    $vf18x  \n\
        vmadday.xyzw    ACC,   $vf13,    $vf18y  \n\
        vmaddaz.xyzw    ACC,   $vf14,    $vf18z  \n\
        vmaddw.xyzw     $vf6,  $vf15,    $vf18w  \n\
        vmulax.xyzw     ACC,   $vf12,    $vf19x  \n\
        vmadday.xyzw    ACC,   $vf13,    $vf19y  \n\
        vmaddaz.xyzw    ACC,   $vf14,    $vf19z  \n\
        vmaddw.xyzw     $vf7,  $vf15,    $vf19w  \n\
        lqc2            $vf12, 0(%2)             \n\
        lqc2            $vf13, 0x10(%2)          \n\
        lqc2            $vf14, 0x20(%2)          \n\
        lqc2            $vf15, 0x30(%2)          \n\
        vmulax.xyzw     ACC,   $vf12,    $vf16x  \n\
        vmadday.xyzw    ACC,   $vf13,    $vf16y  \n\
        vmaddaz.xyzw    ACC,   $vf14,    $vf16z  \n\
        vmaddw.xyzw     $vf8,  $vf15,    $vf16w  \n\
        vmulax.xyzw     ACC,   $vf12,    $vf17x  \n\
        vmadday.xyzw    ACC,   $vf13,    $vf17y  \n\
        vmaddaz.xyzw    ACC,   $vf14,    $vf17z  \n\
        vmaddw.xyzw     $vf9,  $vf15,    $vf17w  \n\
        vmulax.xyzw     ACC,   $vf12,    $vf18x  \n\
        vmadday.xyzw    ACC,   $vf13,    $vf18y  \n\
        vmaddaz.xyzw    ACC,   $vf14,    $vf18z  \n\
        vmaddw.xyzw     $vf10, $vf15,    $vf18w  \n\
        vmulax.xyzw     ACC,   $vf12,    $vf19x  \n\
        vmadday.xyzw    ACC,   $vf13,    $vf19y  \n\
        vmaddaz.xyzw    ACC,   $vf14,    $vf19z  \n\
        vmaddw.xyzw     $vf11, $vf15,    $vf19w  \n\
        ": :"r"(m0),"r"(lw),"r"(m1)
    );
}

int CheckBoundingBox(u_int *prim)
{
    sceVu0FVECTOR *pvec;
    sceVu0FVECTOR *tmpvec;
    int i;
    int clip1;
    int xmax_flg;
    int xmin_flg;
    int ymin_flg;
    int ymax_flg;
    // float *lw[4];
    // float *m0[4];
    // float *v1;
    sceVu0FVECTOR *ed;
    sceVu0FVECTOR *vec_690;
    sceVu0FVECTOR *vec_6e0;

    tmpvec = (sceVu0FVECTOR *)&SCRATCHPAD[0x620];
    vec_690 = (sceVu0FVECTOR *)&SCRATCHPAD[0x690];
    ed = (sceVu0FVECTOR *)&SCRATCHPAD[0x6a0];
    vec_6e0 = (sceVu0FVECTOR *)&SCRATCHPAD[0x6e0];

    lcp[prim[2]].camin = 0;

    asm_1__CheckBoundingBox(SgCMVtx, SgCMtx, lcp[prim[2]].lwmtx);

    pvec = (sceVu0FVECTOR *)&prim[4];

    if (clip_value_check != 0)
    {
        clip1 = 0x3f;

        for (i = 0; i < 8; i++, pvec++)
        {
            clip1 &= BoundClipQ(ed[i], tmpvec[i], *pvec);
        }

        if (clip1 != 0)
        {
            return 0;
        }

        ymax_flg = 1;
        ymin_flg = 1;
        xmax_flg = 1;
        xmin_flg = 1;

        for (i = 0; i < 8; i++)
        {
            if (clip_value[0] < tmpvec[i][0])
            {
                xmin_flg = 0;
            }

            if (tmpvec[i][0] < clip_value[1])
            {
                xmax_flg = 0;
            }

            if (clip_value[2] < tmpvec[i][1])
            {
                ymin_flg = 0;
            }

            if (tmpvec[i][1] < clip_value[3])
            {
                ymax_flg = 0;
            }

        }

        if (xmin_flg | xmax_flg | ymin_flg | ymax_flg)
        {
            return 0;
        }
    }
    else
    {
        pvec = (sceVu0FVECTOR *)&prim[4];

        clip1 = 0x3f;

        for (i = 0; i < 8; i++, pvec++)
        {
            clip1 &= BoundClip(ed[i], *pvec);
        }

        if (clip1 != 0)
        {
            return 0;
        }
    }

    if (wscissor_flg != 0)
    {
        Vu0LoadMatrix(lcp[prim[2]].lwmtx);

        pvec = (sceVu0FVECTOR *)&prim[4];

        for (i = 0; i < 8; i++, pvec++)
        {
            Vu0ApplyVectorInline(tmpvec[i], *pvec);
        }

        if (
            wscissor_upper[1] != 0.0f &&
            wscissor_upper[1] < tmpvec[0][1] &&
            wscissor_upper[1] < vec_690[0][1]
        )
        {
            return 0;
        }

        if (
            wscissor_lower[1] != 0.0f &&
            wscissor_lower[1] > tmpvec[0][1] &&
            wscissor_lower[1] > vec_690[0][1]
        )
        {
            return 0;
        }
    }

    lcp[prim[2]].camin = 1;

    clip1 = ClipCheck(ed);
    clip1 |= ClipCheck(vec_6e0);

    if (clip1 & 0xffffff)
    {
        edge_check = 1;
    }
    else
    {
        edge_check = 0;
    }

    return 1;
}

int CheckBoundingBoxEdge(u_int *prim)
{
    sceVu0FVECTOR *pvec;
    int clip;
    sceVu0FVECTOR *tmpvec;

    tmpvec = (sceVu0FVECTOR *)&SCRATCHPAD[0x620];

    _SetMulMatrix(SgCMtx, lcp[prim[2]].lwmtx);

    pvec = (sceVu0FVECTOR *)&prim[4];

    Vu0ApplyVectorInline(tmpvec[0], pvec[0]);
    Vu0ApplyVectorInline(tmpvec[1], pvec[1]);
    Vu0ApplyVectorInline(tmpvec[2], pvec[2]);
    Vu0ApplyVectorInline(tmpvec[3], pvec[3]);

    clip = ClipCheck(&tmpvec[0]);

    Vu0ApplyVectorInline(tmpvec[4], pvec[4]);
    Vu0ApplyVectorInline(tmpvec[5], pvec[5]);
    Vu0ApplyVectorInline(tmpvec[6], pvec[6]);
    Vu0ApplyVectorInline(tmpvec[7], pvec[7]);

    clip |= ClipCheck(&tmpvec[4]);

    return (clip & 0x00ffffff) != 0;
}

void SgSetFog(float min, float max, float near, float far, int r, int g, int b)
{
    u_int *datap;

    fog_value[0] = min;
    fog_value[1] = max;
    fog_value[2] = (((min - max) * (far + near)) / (far - near) + (min + max)) / 2;
    fog_value[3] = (far * near * (max - min)) / (far - near);

    datap = (u_int *)getObjWrk();

    datap[0] = 0;
    datap[1] = 0;
    datap[2] = VU0_MICRO_ADDR;
    datap[3] = 0x50000002;

    *(u_long *)&datap[4] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    *(u_long *)&datap[6] = SCE_GIF_PACKED_AD;

    *(u_long *)&datap[8] = SCE_GS_SET_FOGCOL(r, g, b);
    *(u_long *)&datap[10] = SCE_GS_FOGCOL;

    AppendDmaBuffer(3);
    FlushModel(0);
}

void SgSetWScissorBox(float ax, float ay, float az, float bx, float by, float bz)
{
    if (ax == 0.0f && ay == 0.0f && az == 0.0f && bx == 0.0f && by == 0.0f && bz == 0.0f)
    {
        wscissor_flg = 0;
    }
    else
    {
        wscissor_flg = 1;

        wscissor_upper[0] = ax;
        wscissor_upper[1] = ay;
        wscissor_upper[2] = az;

        wscissor_lower[0] = bx;
        wscissor_lower[1] = by;
        wscissor_lower[2] = bz;
    }
}

void _SetMulMatrixBB(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FMATRIX lw)
{
    asm volatile("\n\
        lqc2            $vf12, 0(%0)             \n\
        lqc2            $vf13, 0x10(%0)          \n\
        lqc2            $vf14, 0x20(%0)          \n\
        lqc2            $vf15, 0x30(%0)          \n\
        lqc2            $vf16, 0(%1)             \n\
        lqc2            $vf17, 0x10(%1)          \n\
        lqc2            $vf18, 0x20(%1)          \n\
        lqc2            $vf19, 0x30(%1)          \n\
        vmulax.xyzw     ACC,   $vf12,    $vf16x  \n\
        vmadday.xyzw    ACC,   $vf13,    $vf16y  \n\
        vmaddaz.xyzw    ACC,   $vf14,    $vf16z  \n\
        vmaddw.xyzw     $vf4,  $vf15,    $vf16w  \n\
        vmulax.xyzw     ACC,   $vf12,    $vf17x  \n\
        vmadday.xyzw    ACC,   $vf13,    $vf17y  \n\
        vmaddaz.xyzw    ACC,   $vf14,    $vf17z  \n\
        vmaddw.xyzw     $vf5,  $vf15,    $vf17w  \n\
        vmulax.xyzw     ACC,   $vf12,    $vf18x  \n\
        vmadday.xyzw    ACC,   $vf13,    $vf18y  \n\
        vmaddaz.xyzw    ACC,   $vf14,    $vf18z  \n\
        vmaddw.xyzw     $vf6,  $vf15,    $vf18w  \n\
        vmulax.xyzw     ACC,   $vf12,    $vf19x  \n\
        vmadday.xyzw    ACC,   $vf13,    $vf19y  \n\
        vmaddaz.xyzw    ACC,   $vf14,    $vf19z  \n\
        vmaddw.xyzw     $vf7,  $vf15,    $vf19w  \n\
        lqc2            $vf12, 0(%2)             \n\
        lqc2            $vf13, 0x10(%2)          \n\
        lqc2            $vf14, 0x20(%2)          \n\
        lqc2            $vf15, 0x30(%2)          \n\
        vmulax.xyzw     ACC,   $vf12,    $vf16x  \n\
        vmadday.xyzw    ACC,   $vf13,    $vf16y  \n\
        vmaddaz.xyzw    ACC,   $vf14,    $vf16z  \n\
        vmaddw.xyzw     $vf8,  $vf15,    $vf16w  \n\
        vmulax.xyzw     ACC,   $vf12,    $vf17x  \n\
        vmadday.xyzw    ACC,   $vf13,    $vf17y  \n\
        vmaddaz.xyzw    ACC,   $vf14,    $vf17z  \n\
        vmaddw.xyzw     $vf9,  $vf15,    $vf17w  \n\
        vmulax.xyzw     ACC,   $vf12,    $vf18x  \n\
        vmadday.xyzw    ACC,   $vf13,    $vf18y  \n\
        vmaddaz.xyzw    ACC,   $vf14,    $vf18z  \n\
        vmaddw.xyzw     $vf10, $vf15,    $vf18w  \n\
        vmulax.xyzw     ACC,   $vf12,    $vf19x  \n\
        vmadday.xyzw    ACC,   $vf13,    $vf19y  \n\
        vmaddaz.xyzw    ACC,   $vf14,    $vf19z  \n\
        vmaddw.xyzw     $vf11, $vf15,    $vf19w  \n\
        ": :"r"(m0),"r"(lw),"r"(m1)
    );
}
