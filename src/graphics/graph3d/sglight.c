#include "common.h"
#include "typedefs.h"
#include "sglight.h"

#include "sce/libvu0.h"

#include "graphics/graph3d/libsg.h"
#include "graphics/graph3d/sgdma.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/sgsu.h"
#include "graphics/graph3d/sglight.h"

static int stack_num = 0;
static int dbg_flg = 0;

static int write_counter;

static SgLIGHT *stack_light[9];
static sceVu0FVECTOR stack_eye[3];
static int stack_light_num[9];

#define GET_MESH_TYPE(intpointer) (char)((char*)intpointer)[13]
#define GET_MESH_GLOOPS(intpointer) (int)((char*)intpointer)[14]

#define SCRATCHPAD ((u_char *)0x70000000)

void SgPreRenderDbgOn()
{
    dbg_flg = 1;
}

void SgPreRenderDbgOff()
{
    dbg_flg = 0;
}

void QueSetMatrix(sceVu0FMATRIX m, int que, sceVu0FVECTOR v)
{
    sceVu0FVECTOR tmpv;

    _NormalizeVector(tmpv, v);

    m[0][que] = tmpv[0];
    m[1][que] = tmpv[1];
    m[2][que] = tmpv[2];
}

void QueSetMatrixNonNormalized(sceVu0FMATRIX m, int que, sceVu0FVECTOR v)
{
    m[0][que] = v[0];
    m[1][que] = v[1];
    m[2][que] = v[2];
}

void SgSetAmbient(sceVu0FVECTOR ambient)
{
    Vu0CopyVector(TAmbient, ambient);
}

void SgSetDefaultLight(sceVu0FVECTOR eye, SgLIGHT *p0, SgLIGHT *p1, SgLIGHT *p2)
{
    sceVu0FVECTOR nl;
    sceVu0FVECTOR tmpv;
    SgLIGHT *po[3];
    int i;

    _NormalizeVector(ieye, eye);

    Vu0MulVectorX(ieye, ieye, -1.0f);

    for (i = 0; i < 3; i++)
    {
        Vu0ZeroVector(DLightMtx[i]);
        Vu0ZeroVector(SLightMtx[i]);
        Vu0ZeroVector(DColorMtx[i]);
        Vu0ZeroVector(SColorMtx[i]);
    }

    po[0] = p0;
    po[1] = p1;
    po[2] = p2;

    for (i = 0; i < 3; i++)
    {
        if (po[i] != NULL)
        {
            _NormalizeVector(nl, po[i]->direction);
            Vu0CopyVector(DColorMtx[i], po[i]->diffuse);
            Vu0CopyVector(SColorMtx[i], po[i]->specular);
            QueSetMatrix(DLightMtx, i, nl);
            Vu0AddVector(tmpv, ieye, nl);
            _NormalizeVector(tmpv, tmpv);
            QueSetMatrix(SLightMtx, i, tmpv);
        }
        else
        {
            Vu0ZeroVector(DColorMtx[i]);
            Vu0ZeroVector(SColorMtx[i]);
        }
    }

    DColorMtx[0][3] = 1.0f;
}

static void _SetColorMtx(sceVu0FMATRIX dc, sceVu0FMATRIX sc, sceVu0FVECTOR am, sceVu0FVECTOR v)
{
    __asm__ volatile ("\n\
        lqc2      $vf16,0(%3)\n\
        lqc2      $vf12,0(%2)\n\
        vmul.xyz  $vf19xyz,$vf16xyz,$vf12xyz\n\
        vmulw.xyz $vf19xyz,$vf19xyz,$vf12w\n\
        lqc2      $vf16,0x30(%3)\n\
        lqc2      $vf12,0(%0)\n\
        vmulw.xyz $vf16xyz,$vf16xyz,$vf12w\n\
        vadd.xyz  $vf19xyz,$vf19xyz,$vf16xyz\n\
        lqc2      $vf16,0x10(%3)\n\
        vmove.w   $vf19w,$vf12w\n\
        lqc2      $vf12,0(%0)\n\
        lqc2      $vf13,0x10(%0)\n\
        lqc2      $vf14,0x20(%0)\n\
        vmul.xyz  $vf26xyz,$vf12xyz,$vf16xyz\n\
        vmul.xyz  $vf27xyz,$vf13xyz,$vf16xyz\n\
        vmul.xyz  $vf28xyz,$vf14xyz,$vf16xyz\n\
        vmulw.xyz $vf26xyz,$vf26xyz,$vf12w\n\
        vmulw.xyz $vf27xyz,$vf27xyz,$vf12w\n\
        vmulw.xyz $vf28xyz,$vf28xyz,$vf12w\n\
        vmove.w   $vf27w,$vf16w\n\
        lqc2      $vf16,0x20(%3)\n\
        lqc2      $vf12,0(%1)\n\
        lqc2      $vf13,0x10(%1)\n\
        lqc2      $vf14,0x20(%1)\n\
        vmul.xyz  $vf12xyz,$vf12xyz,$vf16xyz\n\
        vmul.xyz  $vf13xyz,$vf13xyz,$vf16xyz\n\
        vmul.xyz  $vf14xyz,$vf14xyz,$vf16xyz\n\
        vmulw.xyz $vf12xyz,$vf12xyz,$vf13w\n\
        vmulw.xyz $vf13xyz,$vf13xyz,$vf13w\n\
        vmulw.xyz $vf14xyz,$vf14xyz,$vf13w\n\
        vmove.w   $vf12w,$vf16w\n\
        vmove.w   $vf13w,$vf27w\n\
        vmove.w   $vf19w,$vf14w\n\
        viaddi    $vi2,$vi0,0xf\n\
        viaddi    $vi2,$vi2,0xb\n\
        vsqi.xyzw $vf26xyzw,($vi2++)\n\
        vsqi.xyzw $vf27xyzw,($vi2++)\n\
        vsqi.xyzw $vf28xyzw,($vi2++)\n\
        vsqi.xyzw $vf12xyzw,($vi2++)\n\
        vsqi.xyzw $vf13xyzw,($vi2++)\n\
        vsqi.xyzw $vf14xyzw,($vi2++)\n\
        vsqi.xyzw $vf19xyzw,($vi2++) \n\
    ": :"r"(dc), "r"(sc), "r"(am), "r"(v));
}

void _ReadDLightMtx(sceVu0FMATRIX tmp)
{
    __asm__ volatile ("\n\
        sqc2    $vf20,0(%0)\n\
        sqc2    $vf21,0x10(%0)\n\
        sqc2    $vf22,0x20(%0)\n\
    ": :"r"(tmp));
}

static void _ReadSLightMtx(sceVu0FMATRIX tmp)
{
    __asm__ volatile ("\n\
        sqc2    $vf23,0(%0)\n\
        sqc2    $vf24,0x10(%0)\n\
        sqc2    $vf25,0x20(%0)\n\
    ": :"r"(tmp));
}

void _ReadDColor(sceVu0FMATRIX tmp)
{
    __asm__ volatile ("\n\
        sqc2    $vf26,0(%0)\n\
        sqc2    $vf27,0x10(%0)\n\
        sqc2    $vf28,0x20(%0)\n\
    ": :"r"(tmp));
}

static int Tim2CalcBufWidth(int psm, int w)
{
    switch (psm)
    {
    case SCE_GS_PSMT8H:
    case SCE_GS_PSMT4HL:
    case SCE_GS_PSMT4HH:
    case SCE_GS_PSMCT32:
    case SCE_GS_PSMCT24:
    case SCE_GS_PSMZ32:
    case SCE_GS_PSMZ24:
    case SCE_GS_PSMCT16:
    case SCE_GS_PSMCT16S:
    case SCE_GS_PSMZ16:
    case SCE_GS_PSMZ16S:
        return (w + 63) / 64;
    break;
    case SCE_GS_PSMT8:
    case SCE_GS_PSMT4:
        w = (w + 63) / 64;

        if (w & 1)
        {
            w++;
        }
        return w;
    break;
    }

    return 0;
}

int GetDecay()
{
    u_int tmp;

    __asm__ volatile("\n\
        cfc2    %0,$vi13\n\
        ": :"r"(tmp)
    );

    return tmp;
}

static inline void _SetDevay_asm(u_int decay)
{
    __asm__ volatile("\n\
        ctc2    %0,$vi13\n\
        ": :"r"(decay)
    );
}

static void SetDecay(u_int decay)
{
    int count;
    // u_int decay;

    count = 0;

    while (decay != 0) {
            decay >>= 1;
            count++;
    }

    _SetDevay_asm(count);
}

float SetMaxColor255(sceVu0FVECTOR dcol, sceVu0FVECTOR col)
{
    float div;

    if (col[0] > col[1])
    {
        if (col[0] > col[2])
        {
            div = col[0] / 255.0f;
        }
        else
        {
            div = col[2] / 255.0f;
        }
    }
    else if (col[1] > col[2])
    {
        div = col[1] / 255.0f;
    }
    else
    {
        div = col[2] / 255.0f;
    }

    if (div == 0.0f)
    {
        div = 1.0f;
    }

    sceVu0DivVectorXYZ(dcol, col, div);

    return div;
}

void SetBWLight(sceVu0FVECTOR col)
{
    float bwcolor;

    if (loadbw_flg != 0)
    {
        bwcolor = (col[0] + col[1] + col[2]) / 3.0f;
        col[0] = col[1] = col[2] = bwcolor;
    }
}

void SetMaterialPointer()
{
    SgLightParallelp = (SgVULightParallel *)&SCRATCHPAD[0x300];
    SgLightSpotp = (SgVULightSpot *)&SCRATCHPAD[0x370];
    SgLightPointp = (SgVULightPoint *)&SCRATCHPAD[0x3f0];
}

void ClearMaterialCache(HeaderSection *hs)
{
    int i;

    for (i = 0; i < hs->materials; i++)
    {
        ((int *)&hs->matp[i].vifcode)[0] = -1;
    }
}

void SetMaterialDataVU(u_int *prim)
{
    static int old_tag_buf = -1;
    qword *base;
    SgMaterialC *pmatC;
    static SgMaterialC *old_pmatC = NULL;
    int i;
    int qwc;

    if (old_tag_buf != tagswap)
    {
        ClearMaterialCache((HeaderSection *)sgd_top_addr);
        old_tag_buf = tagswap;
    }

    pmatC = (SgMaterialC *)prim[2];

    if (pmatC->cache_on >= 0)
    {
        if (pmatC->Point.num == SgPointGroupNum && pmatC->Spot.num == SgSpotGroupNum)
        {
            if (pmatC->Point.num != 0)
            {
                for (i = 0; i < 3; i++)
                {
                    if (pmatC->Point.lnum[i] != SgPointGroup[0].lnum[i])
                    {
                        goto label;
                    }
                }
            }

            if (SgSpotGroupNum != 0)
            {
                for (i = 0; i < 3; i++)
                {
                    if (pmatC->Spot.lnum[i] !=  SgSpotGroup[0].lnum[i])
                    {
                        goto label;
                    }
                }
            }

            if (pmatC == old_pmatC)
            {
                return;
            }

            old_pmatC = pmatC;

            AppendDmaTag(pmatC->tagd_addr, pmatC->qwc);
            FlushModel(0);

            return;
        }
    }
label:
    base = (qword *)getObjWrk();

    old_pmatC = pmatC;
    pmatC->tagd_addr = (u_int)base & 0x0fffffff;
    qwc = 0;

    if (SgSpotGroupNum != 0)
    {
        SgLightSpotp = (SgVULightSpot *)base;
        base += 8;
        qwc += 8;
    }

    if (SgPointGroupNum != 0)
    {
        SgLightPointp = (SgVULightPoint *)base;
        base += 8;
        qwc += 8;
    }

    SgLightParallelp = (SgVULightParallel *)base;
    qwc += 8;

    pmatC->qwc = qwc;

    SetMaterialData(prim);

    if (SgSpotGroupNum != 0)
    {
        SgLightSpotp->SpotVif[0] = SCE_VIF1_SET_NOP(0);
        SgLightSpotp->SpotVif[1] = SCE_VIF1_SET_NOP(0);
        SgLightSpotp->SpotVif[2] = SCE_VIF1_SET_STCYCL(4, 4, 0);
        SgLightSpotp->SpotVif[3] = SCE_VIF1_SET_UNPACK(0x35, 7, V4_32, 0); // VIF1_TOPS=0x35, count=7, V4_32, irq=0

        pmatC->Spot.lnum[0] = SgSpotGroup[0].lnum[0];
        pmatC->Spot.lnum[1] = SgSpotGroup[0].lnum[1];
        pmatC->Spot.lnum[2] = SgSpotGroup[0].lnum[2];
    }

    pmatC->Spot.num = SgSpotGroupNum;

    if (SgPointGroupNum != 0)
    {
        SgLightPointp->PointVif[0] = SCE_VIF1_SET_NOP(0);
        SgLightPointp->PointVif[1] = SCE_VIF1_SET_NOP(0);
        SgLightPointp->PointVif[2] = SCE_VIF1_SET_STCYCL(4, 4, 0);
        SgLightPointp->PointVif[3] = SCE_VIF1_SET_UNPACK(0x3c, 7, V4_32, 0); // VIF1_TOPS=0x3c, count=7, V4_32, irq=0

        pmatC->Point.lnum[0] = SgPointGroup[0].lnum[0];
        pmatC->Point.lnum[1] = SgPointGroup[0].lnum[1];
        pmatC->Point.lnum[2] = SgPointGroup[0].lnum[2];
    }

    pmatC->Point.num = SgPointGroupNum;

    SgLightParallelp->ParallelVif[0] = SCE_VIF1_SET_NOP(0);
    SgLightParallelp->ParallelVif[1] = SCE_VIF1_SET_NOP(0);
    SgLightParallelp->ParallelVif[2] = SCE_VIF1_SET_STCYCL(4, 4, 0);
    SgLightParallelp->ParallelVif[3] = SCE_VIF1_SET_UNPACK(0x2e, 7, V4_32, 0); // VIF1_TOPS=0x2e, count=7, V4_32, irq=0

    AppendDmaBuffer(qwc);
    FlushModel(0);
}

void SetMaterialPointerCoord()
{
    SgLightCoordp = (SgVULightCoord *)&SCRATCHPAD[0x1a0];
}

void SetMaterialPointerCoordVU()
{
    SgLightCoordp = (SgVULightCoord *)&((u_char *)getObjWrk())[0x1a0];
}

inline static void load_abmient(float *amb)
{
    __asm__ volatile ("\n\
        lqc2    $vf19,0(%0)\n\
        ": :"r"(amb)
    );
}

void SetMaterialDataPrerender()
{
    __asm__ volatile ("\n\
        lqc2    $vf4,0(%0)\n\
        lqc2    $vf5,0x10(%0)\n\
        lqc2    $vf6,0x20(%0)\n\
        lqc2    $vf7,0(%1)\n\
        lqc2    $vf8,0x10(%1)\n\
        lqc2    $vf9,0x20(%1)\n\
        lqc2    $vf10,0x30(%1)\n\
        ": :"r"(&SCRATCHPAD[0x110]), "r"(&SCRATCHPAD[0x150])
    );

    load_abmient(SgLightParallelp->Parallel_Ambient);
}

void SetMaterialData(u_int *prim)
{
    int i;
    int j;
    SgMaterialC *pmatC;
    sceVu0FVECTOR *dvec;
    float max_color;

    pmatC = (SgMaterialC *)prim[2];
    pmatC->cache_on = 1;

    if (pmatC->primtype != 0)
    {
        TAmbient[3] = 128.0f;
        DColorMtx[0][3] = 192.0f;
        SColorMtx[1][3] = (pmatC->Specular[0] + pmatC->Specular[1] + pmatC->Specular[2]) * 43.0f;
    }
    else
    {
        TAmbient[3] = 255.0f;
        DColorMtx[0][3] = 255.0f;
        SColorMtx[1][3] = (pmatC->Specular[0] + pmatC->Specular[1] + pmatC->Specular[2]) * 86.0f;
    }

    SColorMtx[2][3] = 255.0f;
    dvec = (sceVu0FVECTOR *)&SCRATCHPAD[0x620];

    Vu0MulVectorXYZ(*dvec, TAmbient, pmatC->Ambient);
    Vu0AddVector(*dvec, *dvec, pmatC->Emission);
    SetBWLight(*dvec);
    Vu0MulVectorX(SgLightParallelp->Parallel_Ambient, *dvec, TAmbient[3]);

    SgLightParallelp->Parallel_Ambient[3] = SColorMtx[2][3];

    for (i = 0; i < SgInfiniteNum; i++)
    {
        Vu0MulVectorXYZ(*dvec, DColorMtx[i], pmatC->Diffuse);
        SetBWLight(*dvec);
        Vu0MulVectorX(SgLightParallelp->Parallel_DColor[i], *dvec, DColorMtx[0][3]);
        Vu0MulVectorXYZ(*dvec, SColorMtx[i], pmatC->Specular);
        SetBWLight(*dvec);
        Vu0MulVectorX(SgLightParallelp->Parallel_SColor[i], *dvec, SColorMtx[1][3]);
    }

    for (; i < 3; i++)
    {
        Vu0ZeroVector(SgLightParallelp->Parallel_DColor[i]);
        Vu0ZeroVector(SgLightParallelp->Parallel_SColor[i]);
    }

    SgLightParallelp->Parallel_DColor[0][3] = pmatC->Diffuse[3];
    SgLightParallelp->Parallel_SColor[0][3] = pmatC->Diffuse[3];

    pmatC->Point.num = SgPointGroupNum;

    for (i = 0; i < SgPointGroupNum; i++)
    {
        SgPOINTGROUP *ppg = &SgPointGroup[i];

        for (j = 0; j < 3; j++)
        {
            Vu0MulVectorXYZW(*dvec, ppg->DColorMtx[j], pmatC->Diffuse);
            SetBWLight(*dvec);
            Vu0MulVectorX(*dvec, *dvec, DColorMtx[0][3]);

            max_color = SetMaxColor255(SgLightPointp->Point_DColor[j], *dvec);

            Vu0MulVectorXYZW(*dvec, ppg->SColorMtx[j], pmatC->Specular);
            SetBWLight(*dvec);
            Vu0MulVectorX(SgLightPointp->Point_SColor[j], *dvec, SColorMtx[1][3] / max_color);

            SgLightPointp->Point_btimes[j] = ppg->btimes[j] * max_color;
            pmatC->Point.lnum[j] = ppg->lnum[j];
        }
    }

    for (i = 0; i < SgSpotGroupNum; i++)
    {
        SgSPOTGROUP *spg = &SgSpotGroup[i];

        for (j = 0; j < 3; j++)
        {
            Vu0MulVectorXYZW(*dvec, spg->DColorMtx[j], pmatC->Diffuse);
            SetBWLight(*dvec);
            Vu0MulVectorX(*dvec, *dvec, DColorMtx[0][3]);

            max_color = SetMaxColor255(SgLightSpotp->Spot_DColor[j], *dvec);

            Vu0MulVectorXYZW(*dvec, spg->SColorMtx[j], pmatC->Specular);
            SetBWLight(*dvec);
            Vu0MulVectorX(SgLightSpotp->Spot_SColor[j], *dvec, SColorMtx[1][3] / max_color);

            SgLightSpotp->Spot_btimes[j] = spg->btimes[j] * max_color;
            pmatC->Spot.lnum[j] = spg->lnum[j];
        }
    }
}

static void _SetDLight(sceVu0FMATRIX m0)
{
    __asm__ volatile ("\n\
        lqc2      $vf20,0(%0)\n\
        lqc2      $vf21,0x10(%0)\n\
        lqc2      $vf22,0x20(%0)\n\
        viaddi    $vi2,$vi0,0xf\n\
        viaddi    $vi2,$vi2,5\n\
        vsqi.xyzw $vf20,($vi2++)\n\
        vsqi.xyzw $vf21,($vi2++)\n\
        vsqi.xyzw $vf22,($vi2++)\n\
        ": :"r"(m0)
    );
}

static void _SetSLight(sceVu0FMATRIX m0)
{
    __asm__ volatile ("\n\
        lqc2      $vf23,0(%0)\n\
        lqc2      $vf24,0x10(%0)\n\
        lqc2      $vf25,0x20(%0)\n\
        viaddi    $vi2,$vi0,0xf\n\
        viaddi    $vi2,$vi2,8\n\
        vsqi.xyzw $vf23,($vi2++)\n\
        vsqi.xyzw $vf24,($vi2++)\n\
        vsqi.xyzw $vf25,($vi2++)\n\
        ": :"r"(m0)
    );
}

void SetPointGroup()
{
    int i;
    int gnum;
    int gcount;
    SgLIGHT *TmpLight;
    SgPOINTGROUP *TmpGroup;

    if (SgPointNum == 0)
    {
        SgPointGroupNum = 0;
        return;
    }

    for (i = 0; i < SgPointNum; i++)
    {
        TmpLight = &SgPointLight[i];

        if (TmpLight->Enable == 0 && TmpLight->SEnable == 0)
        {
            Vu0CopyVector(SgLightCoordp->Point_pos[1], TmpLight->pos);
            Vu0CopyVector(SgLightCoordp->Point_pos[2], TmpLight->pos);

            SgLightCoordp->Point_pos[1][3] = SgLightCoordp->Point_pos[2][3] = 10000000.0f;
            break;
        }
    }

    Vu0ZeroVector(SgPointGroup[0].DColorMtx[1]);
    Vu0ZeroVector(SgPointGroup[0].DColorMtx[2]);

    Vu0ZeroVector(SgPointGroup[0].SColorMtx[1]);
    Vu0ZeroVector(SgPointGroup[0].SColorMtx[2]);

    SgPointGroup[0].btimes[1] = 0.0f;
    SgPointGroup[0].btimes[2] = 0.0f;

    gnum = gcount = 0;

    for (i = 0; i < SgPointNum; i++)
    {
        TmpLight = &SgPointLight[i];
        TmpGroup = &SgPointGroup[gnum];

        if (TmpLight->Enable != 0 || TmpLight->SEnable != 0)
        {
            continue;
        }

        Vu0CopyVector(SgLightCoordp->Point_pos[gcount], TmpLight->pos);
        Vu0CopyVector(SgLightCoordp->Point_eyevec, ieye);

        if (TmpLight->intens != 0.0f)
        {
            SgLightCoordp->Point_pos[gcount][3] = 1.0f / (TmpLight->power * TmpLight->intens * (TmpLight->diffuse[0] + TmpLight->diffuse[1] + TmpLight->diffuse[2]));
        }
        else
        {
            SgLightCoordp->Point_pos[gcount][3] = 0.0f;
        }

        TmpGroup->btimes[gcount] = TmpLight->power;

        Vu0CopyVector(TmpGroup->DColorMtx[gcount], TmpLight->diffuse);
        Vu0CopyVector(TmpGroup->SColorMtx[gcount], TmpLight->specular);

        TmpGroup->lnum[gcount] = i;

        gcount++;

        if (gcount > 2)
        {
            gcount = 0;
            gnum++;

            if (gnum > 0)
            {
                i = SgPointNum + 100;
            }
        }
    }
}

void SetSpotGroup(sceVu0FMATRIX wlmtx)
{
    int i;
    int gnum;
    int gcount;
    sceVu0FVECTOR dtmp;
    sceVu0FVECTOR stmp;
    SgLIGHT *TmpLight;
    SgSPOTGROUP *TmpGroup;

    if (SgSpotNum == 0)
    {
        SgSpotGroupNum = 0;
    }
    else
    {
        for (i = 0; i < 1; i++)
        {
            SgSPOTGROUP *TmpGroup = &SgSpotGroup[i];

            SgLightCoordp->Spot_intens[1] = 100.0f;
            SgLightCoordp->Spot_intens[2] = 100.0f;
            SgLightCoordp->Spot_intens_b[1] = 1.0f;
            SgLightCoordp->Spot_intens_b[2] = 1.0f;

            Vu0CopyVector(SgLightCoordp->Spot_pos[1], SgSpotLight->pos);
            Vu0CopyVector(SgLightCoordp->Spot_pos[2], SgSpotLight->pos);

            Vu0ZeroVector(SgLightCoordp->Spot_WDLightMtx[1]);
            Vu0ZeroVector(SgLightCoordp->Spot_WDLightMtx[2]);
            Vu0ZeroVector(SgLightCoordp->Spot_SLightMtx[0]);
            Vu0ZeroVector(SgLightCoordp->Spot_SLightMtx[1]);
            Vu0ZeroVector(SgLightCoordp->Spot_SLightMtx[2]);

            TmpGroup->btimes[0] = 1.0f;
            TmpGroup->btimes[1] = 1.0f;
            TmpGroup->btimes[2] = 1.0f;

            Vu0ZeroVector(TmpGroup->DColorMtx[1]);
            Vu0ZeroVector(TmpGroup->DColorMtx[2]);
            Vu0ZeroVector(TmpGroup->SColorMtx[1]);
            Vu0ZeroVector(TmpGroup->SColorMtx[2]);
        }

        gnum = gcount = 0;

        for (i = 0; i < SgSpotNum; i++)
        {
            TmpLight = &SgSpotLight[i];
            TmpGroup = &SgSpotGroup[gnum];

            if (TmpLight->Enable != 0 || TmpLight->SEnable != 0)
            {
                continue;
            }

            Vu0CopyVector(SgLightCoordp->Spot_pos[gcount], TmpLight->pos);

            SgLightCoordp->Spot_intens[gcount] = TmpLight->intens;
            SgLightCoordp->Spot_intens_b[gcount] = TmpLight->intens_b;

            Vu0CopyVector(SgLightCoordp->Spot_WDLightMtx[gcount], TmpLight->direction);

            if (wlmtx != NULL)
            {
                Vu0LoadMatrix(wlmtx);
                Vu0ApplyVectorInline(dtmp, TmpLight->direction);
                Vu0ApplyVectorInline(stmp, ieye);
            }
            else
            {
                Vu0CopyVector(dtmp, TmpLight->direction);
                Vu0CopyVector(stmp, ieye);
            }

            Vu0SubVector(stmp, dtmp, stmp);
            QueSetMatrix(SgLightCoordp->Spot_SLightMtx, gcount, dtmp);

            TmpGroup->btimes[gcount] = TmpLight->btimes;

            Vu0CopyVector(TmpGroup->DColorMtx[gcount], TmpLight->diffuse);
            Vu0CopyVector(TmpGroup->SColorMtx[gcount], TmpLight->specular);

            TmpGroup->lnum[gcount] = i;

            gcount++;

            if (gcount > 2)
            {
                gcount = 0;
                gnum++;

                if (gnum > 0)
                {
                    i = SgSpotNum + 100;
                }
            }
        }
    }
}

void SetLightData(SgCOORDUNIT *cp0, SgCOORDUNIT *cp1)
{
    int i;
    sceVu0FMATRIX tmp;
    sceVu0FMATRIX tmp2;
    sceVu0FVECTOR tmpv;
    sceVu0FVECTOR tmpv2;
    sceVu0FVECTOR scale;

    if (cp1 == NULL)
    {
        for (i = 0; i < 3; i++)
        {
            tmpv[0] = cp0->lwmtx[0][i];
            tmpv[1] = cp0->lwmtx[1][i];
            tmpv[2] = cp0->lwmtx[2][i];

            scale[i] = SgRSqrtf(tmpv[0] * tmpv[0] + tmpv[1] * tmpv[1] + tmpv[2] * tmpv[2]);

            _NormalizeVector(tmpv, tmpv);

            tmp[0][i] = tmpv[0];
            tmp[1][i] = tmpv[1];
            tmp[2][i] = tmpv[2];
        }

        Vu0CopyMatrix(*(sceVu0FMATRIX *)&SCRATCHPAD[0x150], cp0->lwmtx);
        Vu0CopyMatrix(*(sceVu0FMATRIX *)&SCRATCHPAD[0x110], tmp);

        _MulRotMatrix(SgLightCoordp->Parallel_DLight, DLightMtx, tmp);
        _MulRotMatrix(SgLightCoordp->Parallel_SLight, SLightMtx, tmp);

        SetPointGroup();
        sceVu0UnitMatrix(tmp);

        for (i = 0; i < 3; i++)
        {
            tmp[i][0] = cp0->lwmtx[0][i] * scale[i];
            tmp[i][1] = cp0->lwmtx[1][i] * scale[i];
            tmp[i][2] = cp0->lwmtx[2][i] * scale[i];
        }

        SetSpotGroup(tmp);
    }
    else
    {
        for (i = 0; i < SgSpotNum; i++)
        {
            SgSpotLight[i].SEnable = 0;
        }

        for (i = 0; i < SgPointNum; i++)
        {
            SgPointLight[i].SEnable = 0;
        }

        sceVu0UnitMatrix(tmp);
        Vu0CopyMatrix(*(sceVu0FMATRIX *)&SCRATCHPAD[0x150], tmp);
        Vu0CopyMatrix(*(sceVu0FMATRIX *)&SCRATCHPAD[0x110], tmp);
        Vu0CopyMatrix(SgLightCoordp->Parallel_DLight, DLightMtx);
        Vu0CopyMatrix(SgLightCoordp->Parallel_SLight, SLightMtx);
        SetPointGroup();
        SetSpotGroup(NULL);
    }

    *((int *)&SCRATCHPAD[0x64]) = SgSpotGroupNum;
    *((int *)&SCRATCHPAD[0x68]) = SgPointGroupNum;
}

void SgSetInfiniteLights(sceVu0FVECTOR eye, SgLIGHT *lights, int num)
{
    SgInfiniteLight = lights;
    SgInfiniteNum = num;

    switch (num)
    {
    case 0:
        SgSetDefaultLight(eye, NULL, NULL, NULL);
    break;
    case 1:
        SgSetDefaultLight(eye, &lights[0], NULL, NULL);
    break;
    case 2:
        SgSetDefaultLight(eye, &lights[0], &lights[1], NULL);
    break;
    default:
        SgSetDefaultLight(eye, &lights[0], &lights[1], &lights[2]);
    break;
    }
}

void SgSetPointLights(SgLIGHT *lights, int num)
{
    int i;

    SgPointLight = lights;
    SgPointNum = num;

    for (i = 0; i < num; i++)
    {
        if (lights[i].Enable == 0)
        {
            lights[i].SEnable = 0;
        }
    }
}

void SgSetSpotLights(SgLIGHT *lights, int num)
{
    int i;

    SgSpotLight = lights;
    SgSpotNum = num;

    for (i = 0; i < num; i++)
    {
        if (lights[i].Enable == 0)
        {
            _NormalizeVector(lights[i].direction, lights[i].direction);

            lights[i].intens_b = 1.0f / (1.0f - lights[i].intens);
            lights[i].btimes = lights[i].power;
            lights[i].SEnable = 0;
        }
    }
}

void PushLight()
{
    sceVu0FVECTOR eye;

    if (stack_num < 3)
    {
        stack_light[stack_num * 3 + 0] = SgInfiniteLight;
        stack_light[stack_num * 3 + 1] = SgPointLight;
        stack_light[stack_num * 3 + 2] = SgSpotLight;

        Vu0MulVectorX(eye, ieye, -1.0f);
        Vu0CopyVector(stack_eye[stack_num], eye);

        stack_light_num[stack_num * 3 + 0] = SgInfiniteNum;
        stack_light_num[stack_num * 3 + 1] = SgPointNum;
        stack_light_num[stack_num * 3 + 2] = SgSpotNum;

        stack_num++;
    }
}

void PopLight()
{
    sceVu0FVECTOR eye;

    if (stack_num != 0)
    {
        stack_num--;

        SgInfiniteLight = stack_light[stack_num * 3 + 0];
        SgPointLight = stack_light[stack_num * 3 + 1];
        SgSpotLight = stack_light[stack_num * 3 + 2];

        Vu0CopyVector(eye, stack_eye[stack_num]);

        SgInfiniteNum = stack_light_num[stack_num * 3 + 0];
        SgPointNum = stack_light_num[stack_num * 3 + 1];
        SgSpotNum = stack_light_num[stack_num * 3 + 2];

        SgSetInfiniteLights(eye, SgInfiniteLight, SgInfiniteNum);
        SgSetPointLights(SgPointLight, SgPointNum);
        SgSetSpotLights(SgSpotLight, SgSpotNum);
    }
}

void ClearLightStack()
{
    stack_num = 0;
}

static void _CalcPointA(sceVu0FMATRIX grc, float *grm, float *len)
{
    asm volatile("                              \n\
        lqc2           $vf14,   0x00(%0)        \n\
        lqc2           $vf15,   0x10(%0)        \n\
        lqc2           $vf16,   0x20(%0)        \n\
        vsub.xyz       $vf14,   $vf14,   $vf12  \n\
        vsub.xyz       $vf15,   $vf15,   $vf12  \n\
        vsub.xyz       $vf16,   $vf16,   $vf12  \n\
        vmulax.xyz     ACC,     $vf4,    $vf13x \n\
        vmadday.xyz    ACC,     $vf5,    $vf13y \n\
        vmaddz.xyz     $vf26,   $vf6,    $vf13z \n\
        vmul.xyz       $vf23,   $vf14,   $vf14  \n\
        vmul.xyz       $vf24,   $vf15,   $vf15  \n\
        vmul.xyz       $vf25,   $vf16,   $vf16  \n\
        vmul.xyz       $vf14,   $vf26,   $vf14  \n\
        vmul.xyz       $vf15,   $vf26,   $vf15  \n\
        vmul.xyz       $vf16,   $vf26,   $vf16  \n\
        vadday.x       ACCx,    $vf23x,  $vf23y \n\
        vmaddz.x       $vf23x,  $vf1x,   $vf23z \n\
        vaddax.y       ACCy,    $vf24y,  $vf24x \n\
        vmaddz.y       $vf23y,  $vf1y,   $vf24z \n\
        vaddax.z       ACCz,    $vf25z,  $vf25x \n\
        vmaddy.z       $vf23z,  $vf1z,   $vf25y \n\
        vdiv           Q,       $vf0w,   $vf23x \n\
        vadday.x       ACCx,    $vf14x,  $vf14y \n\
        vmaddz.x       $vf17x,  $vf1x,   $vf14z \n\
        vaddaz.y       ACCy,    $vf15y,  $vf15z \n\
        vmaddx.y       $vf17y,  $vf1y,   $vf15x \n\
        vaddax.z       ACCz,    $vf16z,  $vf16x \n\
        vmaddy.z       $vf17z,  $vf1z,   $vf16y \n\
        vwaitq                                  \n\
        vaddq.x        $vf27x,  $vf0x,   Q      \n\
        vdiv           Q,$vf0w, $vf23y          \n\
        lqc2           $vf24,   0x00(%1)        \n\
        vmul.xyz       $vf17,   $vf17,   $vf24  \n\
        vmaxx.xyz      $vf17,   $vf17,   $vf0x  \n\
        vwaitq                                  \n\
        vaddq.y        $vf27y,  $vf0y,   Q      \n\
        vdiv           Q,$vf0w, $vf23z          \n\
        lqc2           $vf20,   0x10(%1)        \n\
        lqc2           $vf21,   0x20(%1)        \n\
        lqc2           $vf22,   0x30(%1)        \n\
        lqc2           $vf14,   0x40(%1)        \n\
        lqc2           $vf15,   0x50(%1)        \n\
        lqc2           $vf16,   0x60(%1)        \n\
        vwaitq                                  \n\
        vaddq.z        $vf27z,  $vf0z,   Q      \n\
        sqc2           $vf27,   0x00(%2)        \n\
        ": :"r"(grc), "r"(grm), "r"(len)
    );
}

static void _CalcPointB(float *len)
{
    asm volatile("\n\
        lqc2           $vf27, 0x00(%0)        \n\
        vmul.xyz       $vf25, $vf27,   $vf17  \n\
        vminiw.xyz     $vf25, $vf25,   $vf0w  \n\
        vmul.xyz       $vf23, $vf25,   $vf25  \n\
        vmulax.xyz     ACC,   $vf20,   $vf25x \n\
        vmadday.xyz    ACC,   $vf21,   $vf25y \n\
        vmaddaz.xyz    ACC,   $vf22,   $vf25z \n\
        vmul.xyz       $vf25, $vf23,   $vf23  \n\
        vmul.xyz       $vf25, $vf25,   $vf25  \n\
        vmaddax.xyz    ACC,   $vf14,   $vf25x \n\
        vmadday.xyz    ACC,   $vf15,   $vf25y \n\
        vmaddaz.xyz    ACC,   $vf16,   $vf25z \n\
        vmadd.xyz      $vf18, $vf18,   $vf1   \n\
        ": :"r"(len)
    );
}

void CalcPointLight()
{
    sceVu0FVECTOR len;
    static float max_len = 0.0f;

    if (SgPointGroupNum > 0)
    {
        _CalcPointA(SgLightCoordp->Point_pos, SgLightPointp->Point_btimes,len);

        if (len[0] < SgLightCoordp->Point_pos[0][3])
        {
            len[0] = 0.0f;
        }

        if (len[1] < SgLightCoordp->Point_pos[1][3])
        {
            len[1] = 0.0f;
        }

        if (len[2] < SgLightCoordp->Point_pos[2][3])
        {
            len[2] = 0.0f;
        }

        _CalcPointB(len);
    }
}

inline static void asm_CalcSpotLight(LMATRIX cdata, sceVu0FVECTOR mdata)
{
    asm volatile("                              \n\
        lqc2           $vf14, 0x00(%0)          \n\
        lqc2           $vf15, 0x10(%0)          \n\
        lqc2           $vf16, 0x20(%0)          \n\
        vsub.xyz       $vf14, $vf14,    $vf12   \n\
        vsub.xyz       $vf15, $vf15,    $vf12   \n\
        vsub.xyz       $vf16, $vf16,    $vf12   \n\
        vmulax.xyz     ACC,   $vf4,     $vf13x  \n\
        vmadday.xyz    ACC,   $vf5,     $vf13y  \n\
        vmaddz.xyz     $vf26, $vf6,     $vf13z  \n\
        vmul.xyz       $vf23, $vf14,    $vf14   \n\
        vmul.xyz       $vf24, $vf15,    $vf15   \n\
        vmul.xyz       $vf25, $vf16,    $vf16   \n\
        lqc2           $vf20, 0x50(%0)          \n\
        lqc2           $vf21, 0x60(%0)          \n\
        lqc2           $vf22, 0x70(%0)          \n\
        vmul.xyz       $vf20, $vf20,    $vf14   \n\
        vmul.xyz       $vf21, $vf21,    $vf15   \n\
        vmul.xyz       $vf22, $vf22,    $vf16   \n\
        vadday.x       ACC,   $vf23,    $vf23y  \n\
        vmaddz.x       $vf23, $vf1,     $vf23z  \n\
        vaddax.y       ACC,   $vf24,    $vf24x  \n\
        vmaddz.y       $vf23, $vf1,     $vf24z  \n\
        vaddax.z       ACC,   $vf25,    $vf25x  \n\
        vmaddy.z       $vf23, $vf1,     $vf25y  \n\
        vdiv           Q,     $vf0w,    $vf23x  \n\
        vadday.x       ACC,   $vf20,    $vf20y  \n\
        vmaddz.x       $vf17, $vf1,     $vf20z  \n\
        vaddaz.y       ACC,   $vf21,    $vf21z  \n\
        vmaddx.y       $vf17, $vf1,     $vf21x  \n\
        vaddax.z       ACC,   $vf22,    $vf22x  \n\
        vmaddy.z       $vf17, $vf1,     $vf22y  \n\
        vwaitq                                  \n\
        vaddq.x        $vf23, $vf0,     Q       \n\
        vdiv           Q,     $vf0w,    $vf23y  \n\
        vmul.xyz       $vf14, $vf26,    $vf14   \n\
        vmul.xyz       $vf15, $vf26,    $vf15   \n\
        vmul.xyz       $vf16, $vf26,    $vf16   \n\
        vmaxx.xyz      $vf17, $vf17,    $vf0x   \n\
        vwaitq                                  \n\
        vaddq.y        $vf23, $vf0,     Q       \n\
        vdiv           Q,     $vf0w,    $vf23z  \n\
        vadday.x       ACC,   $vf14,    $vf14y  \n\
        vmaddz.x       $vf24, $vf1,     $vf14z  \n\
        vaddaz.y       ACC,   $vf15,    $vf15z  \n\
        vmaddx.y       $vf24, $vf1,     $vf15x  \n\
        vaddax.z       ACC,   $vf16,    $vf16x  \n\
        vmaddy.z       $vf24, $vf1,     $vf16y  \n\
        vmaxx.xyz      $vf24, $vf24,    $vf0x   \n\
        vwaitq                                  \n\
        vaddq.z        $vf23, $vf0,     Q       \n\
        vmul.xyz       $vf17, $vf17,    $vf17   \n\
        lqc2           $vf27, 0x30(%0)          \n\
        lqc2           $vf28, 0x40(%0)          \n\
        vmula.xyz      ACC,   $vf17,    $vf23   \n\
        vmsub.xyz      $vf17, $vf27,    $vf1    \n\
        vmaxx.xyz      $vf17, $vf17,    $vf0x   \n\
        vmul.xyz       $vf17, $vf17,    $vf28   \n\
        lqc2           $vf26, 0x00(%1)          \n\
        vmul.xyz       $vf24, $vf24,    $vf23   \n\
        vmul.xyz       $vf24, $vf24,    $vf26   \n\
        vminiw.xyz     $vf24, $vf24,    $vf0w   \n\
        vmul.xyz       $vf25, $vf24,    $vf24   \n\
        vmul.xyz       $vf25, $vf25,    $vf25   \n\
        vmul.xyz       $vf25, $vf25,    $vf25   \n\
        vmul.xyz       $vf24, $vf24,    $vf17   \n\
        vmul.xyz       $vf25, $vf25,    $vf17   \n\
        lqc2           $vf20, 0x10(%1)          \n\
        lqc2           $vf21, 0x20(%1)          \n\
        lqc2           $vf22, 0x30(%1)          \n\
        vmulax.xyz     ACC,   $vf18,    $vf1x   \n\
        vmaddax.xyz    ACC,   $vf20,    $vf24x  \n\
        vmadday.xyz    ACC,   $vf21,    $vf24y  \n\
        vmaddaz.xyz    ACC,   $vf22,    $vf24z  \n\
        lqc2           $vf14, 0x40(%1)          \n\
        lqc2           $vf15, 0x50(%1)          \n\
        lqc2           $vf16, 0x60(%1)          \n\
        vmaddax.xyz    ACC,   $vf14,    $vf25x  \n\
        vmadday.xyz    ACC,   $vf15,    $vf25y  \n\
        vmaddz.xyz     $vf18, $vf16,    $vf25z  \n\
        ": :"r"(cdata), "r"(mdata)
    );
}

void CalcSpotLight()
{
    if (SgSpotGroupNum > 0)
    {
        asm_CalcSpotLight(SgLightCoordp->Spot_pos, SgLightSpotp->Spot_btimes);
    }
}

void SgReadLights(void *sgd_top, void *light_top, float *Ambient, SgLIGHT *Ilight, int imax, SgLIGHT *Plight, int pmax, SgLIGHT *Slight, int smax)
{
    int num;
    int i;
    u_int *prim;
    u_int *pk;
    sceVu0FVECTOR *pvec;
    sceVu0FVECTOR interest;
    sceVu0FVECTOR tmpvec;
    SgCOORDUNIT *cp;
    float scale;
    float cc;

    if (sgd_top == NULL && light_top == NULL)
    {
        return;
    }

    cp = NULL;

    if (sgd_top != NULL)
    {
        cp = ((HeaderSection *)sgd_top)->coordp;
    }

    if (cp != NULL)
    {
        scale = SgSqrtf(inline_asm__libsg_g_line_463(cp->lwmtx[0], cp->lwmtx[0]));
        scale += SgSqrtf(inline_asm__libsg_g_line_463(cp->lwmtx[1], cp->lwmtx[1]));
        scale += SgSqrtf(inline_asm__libsg_g_line_463(cp->lwmtx[2], cp->lwmtx[2]));
        scale /= 3.0f;
    }
    else
    {
        scale = 1.0f;
    }

    pk = (u_int *)&((HeaderSection *)light_top)->primitives;

    if (light_top == NULL)
    {
        pk = (u_int *)&((HeaderSection *)sgd_top)->primitives;
    }

    if (Ilight != NULL)
    {
        Ilight->num = 0;
    }

    if (Plight != NULL)
    {
        Plight->num = 0;
    }

    if (Slight != NULL)
    {
        Slight->num = 0;
    }

    Vu0ZeroVector(Ambient);

    prim = (u_int *)*pk;

    while (*prim != NULL)
    {
        if (prim[1] == 11)
        {
            num = prim[3];

            switch (prim[2])
            {
            case 0:
                if (imax < num)
                {
                    num = imax;
                }

                pvec = (sceVu0FVECTOR *)&prim[4];

                if (Ilight != NULL)
                {
                    Ilight->num = num;

                    for (i = 0; i < num; i++)
                    {
                        Vu0CopyVector(Ilight[i].diffuse, pvec[0]);
                        Vu0CopyVector(Ilight[i].specular, pvec[0]);

                        Ilight[i].ambient[0] = 0.0f;
                        Ilight[i].ambient[1] = 0.0f;
                        Ilight[i].ambient[2] = 0.0f;
                        Ilight[i].ambient[3] = 0.0f;

                        Vu0CopyVector(Ilight[i].direction, pvec[1]);

                        Ilight[i].Enable = 0;

                        pvec += 2;
                    }
                }
            break;
            case 1:
                if (pmax < num)
                {
                    num = pmax;
                }

                pvec = (sceVu0FVECTOR *)&prim[4];

                if (Plight != NULL)
                {
                    Plight->num = num;

                    for (i = 0; i < num; i++)
                    {
                        Vu0CopyVector(Plight[i].diffuse, pvec[0]);
                        Vu0CopyVector(Plight[i].specular, pvec[0]);

                        Plight[i].intens = pvec[0][3] * scale;
                        Plight[i].ambient[0] = 0.0f;
                        Plight[i].ambient[1] = 0.0f;
                        Plight[i].ambient[2] = 0.0f;
                        Plight[i].ambient[3] = 0.0f;

                        if (cp != NULL)
                        {
                            sceVu0ApplyMatrix(Plight[i].pos, cp->lwmtx, pvec[1]);
                        }
                        else
                        {
                            Vu0CopyVector(Plight[i].pos, pvec[1]);
                        }

                        Plight[i].power = _TransPPower(scale);
                        Plight[i].ambient[0] = scale;
                        Plight[i].Enable = 0;

                        pvec += 2;
                    }
                }
            break;
            case 2:
                if (smax < num)
                {
                    num = smax;
                }

                pvec = (sceVu0FVECTOR *)&prim[4];

                if (Slight != NULL)
                {
                    Slight->num = num;

                    for(i = 0; i < num; i++)
                    {
                        Vu0CopyVector(Slight[i].diffuse, pvec[0]);
                        Vu0CopyVector(Slight[i].specular, pvec[0]);

                        Slight[i].ambient[0] = 0.0f;
                        Slight[i].ambient[1] = 0.0f;
                        Slight[i].ambient[2] = 0.0f;
                        Slight[i].ambient[3] = 0.0f;

                        if (cp != NULL)
                        {
                            Vu0CopyVector(tmpvec, pvec[1]);
                            tmpvec[3] = 1.0f;

                            sceVu0ApplyMatrix(Slight[i].pos, cp->lwmtx, tmpvec);

                            Vu0CopyVector(tmpvec, pvec[2]);
                            tmpvec[3] = 1.0f;

                            sceVu0ApplyMatrix(interest, cp->lwmtx, tmpvec);
                            sceVu0SubVector(Slight[i].direction, Slight[i].pos, interest);
                        }
                        else
                        {
                            Vu0CopyVector(Slight[i].pos, pvec[1]);
                            Slight[i].pos[3] = 1.0f;

                            sceVu0SubVector(Slight[i].direction, pvec[1], pvec[2]);
                        }

                        cc = SgCosfd(pvec[1][3]);

                        Slight[i].intens = cc * cc;
                        Slight[i].power = _TransSPower(scale);
                        Slight[i].ambient[0] = scale;
                        Slight[i].ambient[1] = pvec[1][3];
                        Slight[i].Enable = 0;

                        pvec += 3;
                    }
                }
            break;
            case 3:
                if (Ambient != NULL)
                {
                    pvec = (sceVu0FVECTOR *)&prim[4];
                    Vu0CopyVector(Ambient, pvec[0]);
                }
            break;
            }
        }

        prim = (u_int *)*prim;
    }
}

u_int* GetNextUnpackAddr(u_int *prim)
{
    while (1)
    {
        if ((*prim & 0x60000000) == 0x60000000)
        {
            return prim;
        }

        prim++;
    }
}

inline static void asm_1__SetPreRenderTYPE0(sceVu0FVECTOR normal, sceVu0FVECTOR vertex)
{
    asm volatile("                              \n\
        lqc2            $vf13, 0x00(%0)         \n\
        lqc2            $vf12, 0x00(%1)         \n\
        vmulax.xyzw     ACC,   $vf7,    $vf12x  \n\
        vmadday.xyzw    ACC,   $vf8,    $vf12y  \n\
        vmaddaz.xyzw    ACC,   $vf9,    $vf12z  \n\
        vmaddw.xyzw     $vf12, $vf10,   $vf12w  \n\
        ": :"r"(normal), "r"(vertex)
    );
}

inline static void asm_2__SetPreRenderTYPE0(sceVu0FVECTOR first)
{
    asm volatile("              \n\
        lqc2    $vf18, 0x00(%0) \n\
        ": :"r"(first)
    );
}

inline static void asm_3__SetPreRenderTYPE0(sceVu0FVECTOR pcol)
{
    asm volatile("                           \n\
        vminiw.xyzw    $vf18, $vf18, $vf19w  \n\
        sqc2           $vf18, 0x00(%0)       \n\
        ": :"r"(pcol)
    );
}

void SetPreRenderTYPE0(int gloops, u_int *prim)
{
    int i;
    int j;
    int loops;
    float *vp;
    sceVu0FVECTOR normal;
    sceVu0FVECTOR vertex;
    sceVu0FVECTOR first;
    sceVu0FVECTOR pcol;

    first[0] = 0.0f;
    first[1] = 0.0f;
    first[2] = 0.0f;
    first[3] = 0.0f;
    normal[3] = 1.0f;
    vertex[3] = 1.0f;

    vp = (float *)&vuvnprim[14];

    i = ((short *)prim)[11];

    if (i == 0)
    {
        return;
    }

    prim = &prim[i];

    for (j = 0; j < gloops; j++)
    {
        prim = GetNextUnpackAddr(prim);

        loops = ((u_char *)prim)[2];

        prim = &prim[1];

        for (i = 0; i < loops; i++)
        {
            if (prim[0] != 1)
            {
                vertex[0] = vp[0];
                vertex[1] = vp[1];
                vertex[2] = vp[2];

                normal[0] = vp[3];
                normal[1] = vp[4];
                normal[2] = vp[5];

                asm_1__SetPreRenderTYPE0(normal, vertex);

                first[0] = ((float *)prim)[0];
                first[1] = ((float *)prim)[1];
                first[2] = ((float *)prim)[2];

                asm_2__SetPreRenderTYPE0(first);

                CalcPointLight();
                CalcSpotLight();

                asm_3__SetPreRenderTYPE0(pcol);

                ((float *)prim)[0] = pcol[0];
                ((float *)prim)[1] = pcol[1];
                ((float *)prim)[2] = pcol[2];
            }

            vp += 6;
            prim = &prim[3];
        }
    }
}

void SetPreRenderTYPE2(int gloops, u_int *prim)
{
    int i;
    int j;
    int loops;
    float *vp;
    sceVu0FVECTOR normal;
    sceVu0FVECTOR vertex;
    sceVu0FVECTOR first;
    sceVu0FVECTOR pcol;

    first[0] = 0.0f;
    first[1] = 0.0f;
    first[2] = 0.0f;
    first[3] = 0.0f;
    normal[3] = 1.0f;
    vertex[3] = 1.0f;

    vp = (float *)&vuvnprim[14];

    i = ((short *)prim)[11];

    if (i == 0)
    {
        return;
    }

    prim = &prim[i];

    for (j = 0; j < gloops; j++)
    {
        prim = GetNextUnpackAddr(prim);

        loops = ((u_char *)prim)[2];

        prim = &prim[1];

        for (i = 0; i < loops; i++)
        {
            if (prim[0] != 1)
            {
                vertex[0] = vp[0];
                vertex[1] = vp[1];
                vertex[2] = vp[2];

                normal[0] = vp[3];
                normal[1] = vp[4];
                normal[2] = vp[5];

                asm_1__SetPreRenderTYPE0(normal, vertex);

                first[0] = ((float *)prim)[0];
                first[1] = ((float *)prim)[1];
                first[2] = ((float *)prim)[2];

                asm_2__SetPreRenderTYPE0(first);

                CalcPointLight();
                CalcSpotLight();

                asm_3__SetPreRenderTYPE0(pcol);

                if (dbg_flg != 0)
                {
                    printf("%f %f %f\n", pcol[0], pcol[1], pcol[2]);
                }

                ((float *)prim)[0] = pcol[0];
                ((float *)prim)[1] = pcol[1];
                ((float *)prim)[2] = pcol[2];

                if ((prim[0] & 0xffff) == 1)
                {
                    prim[0] = prim[0] + 1;
                }
            }

            vp += 6;
            prim = &prim[3];
        }
    }
}

void SetPreRenderTYPE2F(int gloops, u_int *prim)
{
    int i;
    int j;
    int loops;
    float *vp;
    float *np;
    sceVu0FVECTOR normal;
    sceVu0FVECTOR vertex;
    sceVu0FVECTOR first;
    sceVu0FVECTOR pcol;

    first[0] = 0.0f;
    first[1] = 0.0f;
    first[2] = 0.0f;
    first[3] = 0.0f;
    vertex[3] = 1.0f;
    normal[3] = 1.0f;

    np = (float *)&vuvnprim[14];

    vp = (float *)((u_int)np + ((short *)vuvnprim)[5] * 12);

    i = ((short *)prim)[11];

    if (i == 0)
    {
        return;
    }

    prim = &prim[i];

    for (j = 0; j < gloops; j++)
    {
        prim = GetNextUnpackAddr(prim);

        loops = ((u_char *)prim)[2];

        prim = &prim[1];

        normal[0] = np[0];
        normal[1] = np[1];
        normal[2] = np[2];

        np += 3;

        for (i = 0; i < loops; i++)
        {
            if (prim[0] != 1)
            {
                vertex[0] = vp[0];
                vertex[1] = vp[1];
                vertex[2] = vp[2];

                asm_1__SetPreRenderTYPE0(normal, vertex);

                first[0] = ((float *)prim)[0];
                first[1] = ((float *)prim)[1];
                first[2] = ((float *)prim)[2];

                asm_2__SetPreRenderTYPE0(first);

                CalcPointLight();
                CalcSpotLight();

                asm_3__SetPreRenderTYPE0(pcol);

                if (dbg_flg != 0)
                {
                    printf("%f %f %f\n", pcol[0], pcol[1], pcol[2]);
                }

                ((float *)prim)[0] = pcol[0];
                ((float *)prim)[1] = pcol[1];
                ((float *)prim)[2] = pcol[2];

                if ((prim[0] & 0xffff) == 1)
                {
                    prim[0] = prim[0] + 1;
                }
            }

            prim = &prim[3];
            vp += 3;
        }
    }
}

void SetPreRenderMeshData(u_int *prim)
{
    int gloops;
    int mtype;

    gloops = GET_MESH_GLOOPS(prim);
    mtype = GET_MESH_TYPE(prim);

    switch (mtype)
    {
    case 16:
        SetPreRenderTYPE0(gloops, prim);
    break;
    case 18:
        SetPreRenderTYPE2(gloops, prim);
    break;
    case 50:
        SetPreRenderTYPE2F(gloops, prim);
    break;
    }
}

static void _SetSpotPos(sceVu0FVECTOR pos, sceVu0FVECTOR dir)
{
    asm volatile("              \n\
        lqc2    $vf12, 0(%0)    \n\
        lqc2    $vf13, 0(%1)    \n\
        ": :"r"(pos), "r"(dir)
    );
}

static float _SpotInnerProduct(sceVu0FVECTOR bpos)
{
    float ret;

    asm volatile("\n\
        lqc2        $vf14, 0(%1)          \n\
        vsub.xyz    $vf14, $vf12, $vf14   \n\
        vmul.xyz    $vf14, $vf13, $vf14   \n\
        vaddy.x     $vf14, $vf14, $vf14y  \n\
        vaddz.x     $vf14, $vf14, $vf14z  \n\
        qmfc2       %0,    $vf14          \n\
        ":"=r"(ret) :"r"(bpos)
    );

    return ret;
}

void SelectLight(u_int *prim)
{
    SgLIGHT *TmpLight;
    sceVu0FVECTOR *pvec;
    sceVu0FVECTOR minvec;
    sceVu0FVECTOR maxvec;
    sceVu0FVECTOR ominvec;
    sceVu0FVECTOR omaxvec;
    sceVu0FVECTOR *tmpvec;
    sceVu0FVECTOR plain;
    sceVu0FVECTOR interest;
    float maxpower[4];
    float colscale;
    float spotdir;
    float spotvalue[8];
    int maxnum[4];
    int i;
    int j;
    int k;

    if (SgSpotNum == 0 && SgPointNum == 0) // Line 1614
    {
        return;
    }

    tmpvec = (sceVu0FVECTOR *)&SCRATCHPAD[0x620];
    pvec = (sceVu0FVECTOR *)&prim[4];

    Vu0AddVector(*(sceVu0FVECTOR *)&SCRATCHPAD[0x6a0], *pvec, *(sceVu0FVECTOR *)&prim[32]);
    Vu0MulVectorX(*(sceVu0FVECTOR *)&SCRATCHPAD[0x6a0], *(sceVu0FVECTOR *)&SCRATCHPAD[0x6a0], 0.5f);
    Vu0LoadMatrix(lcp[prim[2]].lwmtx);

    for (i = 0; i < 8; i++)
    {
        Vu0ApplyVectorInline(tmpvec[i], pvec[i]);
    }

    Vu0ApplyVectorInline(*(sceVu0FVECTOR *)&SCRATCHPAD[0x6a0], *(sceVu0FVECTOR *)&SCRATCHPAD[0x6a0]);

    maxpower[0] = maxpower[1] = maxpower[2] = 0.0f;
    maxnum[0]   = maxnum[1]   = maxnum[2] = -1;

    for (i = 0; i < SgPointNum; i++)
    {
        TmpLight = &SgPointLight[i];

        if (TmpLight->Enable == 0)
        {
            TmpLight->SEnable = 1;
            colscale = (TmpLight->diffuse[0] + TmpLight->diffuse[1] + TmpLight->diffuse[2]) * TmpLight->power;
            TmpLight->spower = colscale / _CalcLen(TmpLight->pos, *(sceVu0FVECTOR *)&SCRATCHPAD[0x6a0]);

            for (j = 0; j < 3; j++)
            {
                if (maxpower[j] < TmpLight->spower)
                {
                    for (k = 3; k > j; k--)
                    {
                        maxpower[k] = maxpower[k-1];
                        maxnum[k] = maxnum[k-1];
                    }

                    maxpower[j] = TmpLight->spower;
                    maxnum[j] = i;
                    break;
                }
            }
        }
    }

    SgPointGroupNum = 0;

    for (j = 0; j < 3; j++)
    {
        if (maxnum[j] >= 0)
        {
            SgPointLight[maxnum[j]].SEnable = 0;
            SgPointGroupNum = 1;
        }
    }

    maxpower[0] = maxpower[1] = maxpower[2] = 0.0f;
    maxnum[0]   = maxnum[1]   = maxnum[2]   = -1;

    for (i = 0; i < SgSpotNum; i++)
    {
        TmpLight = &SgSpotLight[i];
        if (TmpLight->Enable == 0)
        {
            TmpLight->SEnable = 1;

            Vu0CopyVector(plain, TmpLight->direction);
            plain[3] = -inline_asm__libsg_g_line_463(plain, TmpLight->pos);

            Vu0AddVector(interest, TmpLight->pos, TmpLight->direction);

            spotdir = inline_asm__libsg_g_line_463(plain, interest) + plain[3];

            for (j = 0; j < 8; j++)
            {
                spotvalue[j] = inline_asm__libsg_g_line_463(plain, tmpvec[j]) + plain[3];
                if (spotvalue[j] * spotdir < 0.0f)
                {
                    break;
                }
            }

            if (j != 8)
            {
                colscale = (TmpLight->diffuse[0] + TmpLight->diffuse[1] + TmpLight->diffuse[2]) * TmpLight->power;
                TmpLight->spower = colscale /  _CalcLen(TmpLight->pos, *(sceVu0FVECTOR *)&SCRATCHPAD[0x6a0]);

                for (j = 0; j < 3; j++)
                {
                    if (maxpower[j] < TmpLight->spower)
                    {
                        for (k = 3; k > j; k--)
                        {
                            maxpower[k] = maxpower[k-1];
                            maxnum[k] = maxnum[k-1];
                        }

                        maxpower[j] = TmpLight->spower;
                        maxnum[j] = i;
                        break;
                    }
                }
            }
        }
    }

    SgSpotGroupNum = 0;

    for (j = 0; j < 3; j++)
    {
        if (maxnum[j] >= 0)
        {
            SgSpotLight[maxnum[j]].SEnable = 0;
            SgSpotGroupNum = 1;
        }
    }
}

void SgPreRenderPrim(u_int *prim)
{
    if (prim == NULL)
    {
        return;
    }

    while (prim[0] != NULL)
    {
        switch(prim[1])
        {
        case NULL:
            vuvnprim = prim;
        break;
        case 1:
            nextprim = prim[0];

            SetPreRenderMeshData(prim);
        break;
        case 2:
            SetMaterialData(prim);
            SetMaterialDataPrerender();

            if (dbg_flg != 0)
            {
                printf("PNum %d(%d) SNum %d(%d)\n", SgPointGroupNum, SgPointNum, SgSpotGroupNum, SgSpotNum);

                printVectorC(SgLightCoordp->Spot_pos[0], "pos0");
                printVectorC(SgLightCoordp->Spot_pos[1], "pos1");
                printVectorC(SgLightCoordp->Spot_pos[2], "pos2");
                printVectorC(SgLightCoordp->Spot_intens, "intens");
                printVectorC(SgLightCoordp->Spot_intens_b, "intens_b");
                printLMatC(SgLightCoordp->Spot_WDLightMtx, "WDLightMtx");
                printLMatC(SgLightCoordp->Spot_SLightMtx, "SLightMtx");
                printVectorC(SgLightSpotp->Spot_btimes, "btimes");
                printLMatC(SgLightSpotp->Spot_DColor, "DColor");
                printLMatC(SgLightSpotp->Spot_SColor, "SColor");
            }
        break;
        case 3:
            Vu0CopyMatrix(*(sceVu0FMATRIX *)&SCRATCHPAD[0x430], lcp[prim[2]].lwmtx);
            _MulMatrix(*(sceVu0FMATRIX *)&SCRATCHPAD[0x90], SgWSMtx, *(sceVu0FMATRIX *)&SCRATCHPAD[0x430]);
            break;
        case 4:
            SelectLight(prim);
            SetLightData(&lcp[prim[2]], NULL);
        break;
        }

        prim = (u_int *)*prim;
    }
}

void SgPreRender(void *sgd_top, int pnum)
{
    int i;
    u_int *pk;
    HeaderSection *hs;

    Set12Register();

    write_counter = 0;

    hs = (HeaderSection *)sgd_top;

    pk = (u_int *)&hs->primitives;
    lcp = hs->coordp;
    blocksm = hs->blocks;

    SetMaterialPointer();
    SetMaterialPointerCoord();

    if (pnum < 0)
    {
        for (i = 1; i < blocksm; i++)
        {
            SgPreRenderPrim((u_int *)pk[i]);
        }
    }
    else if (pnum != 0)
    {
        SgPreRenderPrim((u_int *)pk[pnum]);
    }
}

void ClearPreRenderMeshData(u_int *prim)
{
    int i;
    int j;
    int loops;
    int gloops;
    int mtype;
    sceVu0FVECTOR first;

    mtype = ((char *)prim)[0xd];
    gloops = ((char *)prim)[0xe];

    i = ((short *)prim)[11];

    if (i == 0)
    {
        return;
    }

    prim = &prim[i];

    switch (mtype)
    {
    case 0x10:
        for (j = 0; j < gloops; j++)
        {
            prim = GetNextUnpackAddr(prim);

            loops = ((u_char *)prim)[2];

            prim = &prim[1];

            for (i = 0; i < loops; i++)
            {
                if (prim[0] != 1)
                {
                    prim[0] = 0;
                    prim[1] = 0;
                    prim[2] = 0;
                }

                prim = &prim[3];
            }
        }
    break;
    case 0x12:
    case 0x32:
        for (j = 0; j < gloops; j++)
        {
            prim = GetNextUnpackAddr(prim);

            loops = ((u_char *)prim)[2];

            prim = &prim[1];

            for (i = 0; i < loops; i++)
            {
                if (prim[0] != 1)
                {
                    prim[0] = 0;
                    prim[1] = 0;
                    prim[2] = 0;
                }

                prim = &prim[3];
            }
        }
    break;
    }
}

void SgClearPreRenderPrim(u_int *prim)
{
    if (prim == NULL)
    {
        return;
    }

    while (prim[0] != NULL)
    {
        if (prim[1] == 1)
        {
            ClearPreRenderMeshData(prim);
        }

        prim = (u_int *)prim[0];
    }
}

void SgClearPreRender(void *sgd_top, int pnum)
{
    int i;
    u_int *pk;
    HeaderSection *hs;

    hs = (HeaderSection *)sgd_top;

    blocksm = hs->blocks;
    pk = (u_int *)&hs->primitives;

    SetMaterialPointer();
    SetMaterialPointerCoord();

    if (pnum < 0)
    {
        for (i = 1; i < blocksm; i++)
        {
            SgClearPreRenderPrim((u_int *)pk[i]);
        }
    }
    else if (pnum != 0)
    {
        SgClearPreRenderPrim((u_int *)pk[pnum]);
    }
}
