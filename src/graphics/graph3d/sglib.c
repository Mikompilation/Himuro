#include "common.h"
#include "typedefs.h"
#include "sglib.h"

#include "ee/eestruct.h"
#include "sce/libdma.h"
#include "sce/libvu0.h"

#include "graphics/graph3d/sg_dat.h"
#include "graphics/graph3d/sgdma.h"
#include "graphics/graph3d/sgcam.h"
#include "graphics/graph3d/shadow.h"

#define PI 3.1415925f

sceVu0FVECTOR vf12reg[2] = {
    {1.0f, 1.0f, 1.0f, -1.0f},
    {1.0f, 0.0f, 0.0f, 0.0f},
};
static sceVu0FVECTOR trad = {0.0f, 1/(PI * 2), -2 * PI, PI/ 180.0f};
static sceVu0FVECTOR tgsinf[2] = {
    {0.0f, -1.0f/6.0f, 1.0f/120.0f, 0.0f},
    {-1.0f/5040.0f, 1.0f/362880.0f, 1.0f, 0.0f},
};
sceVu0FMATRIX SgWSMtx = {0};
sceVu0FMATRIX SgCMtx = {0};
sceVu0FMATRIX SgCMVtx = {0};
sceVu0FMATRIX DLightMtx = {0};
sceVu0FMATRIX SLightMtx = {0};
sceVu0FMATRIX DColorMtx = {0};
sceVu0FMATRIX SColorMtx = {0};
sceVu0FVECTOR TAmbient = {0};
SgPOINTGROUP SgPointGroup[1] = {0};
sceVu0FVECTOR ieye = {0};
SgSPOTGROUP SgSpotGroup[1] = {0};
CoordCache ccahe = {0};
sceVu0FVECTOR fog_value = {0};
sceVu0IVECTOR fog_color = {0};
ShadowTexture shadowtex = {0};
sceVu0FVECTOR clip_value = {0};
sceVu0FVECTOR wscissor_upper = {0};
sceVu0FVECTOR wscissor_lower = {0};

SgLIGHT *SgInfiniteLight = NULL;
SgLIGHT *SgPointLight = NULL;
SgLIGHT *SgSpotLight = NULL;
int model_buffer_size = 0;
int model_tag_size = 0;
sceVu0FVECTOR *vertex_buffer = NULL;
sceVu0FVECTOR *normal_buffer = NULL;
int vnbuf_size = 0;
int loadtri2_flg = 1;
int loadbw_flg = 0;
int set_bw_color = 0;

static inline float asm_1(float rad)
{
    float ret;

    __asm__ volatile("\n\
        lqc2     $vf12,0(%0)\n\
        qmtc2    %1,$vf13\n\
        ": : "r" (trad), "r" (rad) : "v1"
    );

    __asm__ volatile("\n\
        vmulx.y  $vf13y,$vf12y,$vf13x\n\
        vftoi0.y $vf13y,$vf13y\n\
        vitof0.y $vf13y,$vf13y\n\
        vmulz.y  $vf13y,$vf13y,$vf12z\n\
        vaddy.x  $vf13x,$vf13x,$vf13y\n\
    ");

    __asm__ volatile("\n\
        qmfc2    %0,$vf13\n\
        ": "=r" (ret) : : "v0"
    );

    return ret;
}

static inline float asm_2(float rad)
{
    float ret;

    __asm__ volatile("\n\
        qmtc2     %1,$vf14\n\
        lqc2      $vf12,0(%0)\n\
        lqc2      $vf13,0x10(%0)\n\
        ": : "r" (tgsinf), "r" (rad) : "v1"
    );

    __asm__ volatile("\n\
        vmove.x   $vf12x,$vf14x\n\
        vmulx.x   $vf14x,$vf12x,$vf12x\n\
        vmulaw.x  ACCx,$vf12x,$vf0w\n\
        vsubw.z   $vf14z,$vf0z,$vf0w\n\
        vmulx.x   $vf15x,$vf14x,$vf12x\n\
        vmadday.x ACCx,$vf15x,$vf12y\n\
        vmulx.x   $vf15x,$vf15x,$vf14x\n\
        vmaddaz.x ACCx,$vf15x,$vf12z\n\
        vmulx.x   $vf15x,$vf15x,$vf14x\n\
        vmaddax.x ACCx,$vf15x,$vf13x\n\
        vmulx.x   $vf15x,$vf15x,$vf14x\n\
        vmaddy.x  $vf12x,$vf15x,$vf13y\n\
        vminiw.x  $vf12x,$vf12x,$vf0w\n\
        vmaxz.x   $vf12x,$vf12x,$vf14z\n\
    ");

     __asm__ volatile("\n\
        qmfc2     %0,$vf12\n\
        ": "=r" (ret) : : "v0"
    );

    return ret;
}

static inline float asm_3(float degree)
{
    float ret;

    __asm__ volatile("\n\
        lqc2     $vf12,0(%0)\n\
        qmtc2    %1,$vf13\n\
        ": : "r" (trad), "r" (degree) : "v1"
    );

    __asm__ volatile("\n\
        vmulw.x  $vf13x,$vf13x,$vf12w\n\
        vmulx.y  $vf13y,$vf12y,$vf13x\n\
        vftoi0.y $vf13y,$vf13y\n\
        vitof0.y $vf13y,$vf13y\n\
        vmulz.y  $vf13y,$vf13y,$vf12z\n\
        vaddy.x  $vf13x,$vf13x,$vf13y\n\
    ");

    __asm__ volatile("\n\
        qmfc2    %0,$vf13\n\
        ": "=r" (ret) : : "v0"
    );

    return ret;
}

void _GetNormalVectorFromVector(sceVu0FVECTOR norm, sceVu0FVECTOR p0, sceVu0FVECTOR p1)
{
    __asm__ volatile("\n\
    lqc2        vf12,0x0(%1)\n\
    lqc2        vf13,0x0(%2)\n\
    vopmula.xyz ACC,vf12,vf13\n\
    vopmsub.xyz vf12,vf13,vf12\n\
    sqc2        vf12,0x0(%0)\n\
    ": : "r" (norm) , "r" (p0) ,"r" (p1));
}

void WaitVU1()
{
    asm volatile("\n\
        WVU1:\n\
        bc2t WVU1\n\
        nop\n\
    ");
}

float _TransPPower(float scale)
{
    return scale * 60.0f;
}

float _TransSPower(float scale)
{
    return scale * 200.0f;
}

void Set12Register()
{
    sceVu0FVECTOR vf[2];

    vf[0][0] = 1.0f;
    vf[0][1] = 1.0f;
    vf[0][2] = 1.0f;
    vf[0][3] = -1.0f;

    vf[1][0] = 1.0f;
    vf[1][1] = 0.0f;
    vf[1][2] = 0.0f;
    vf[1][3] = 0.0f;

    __asm__ volatile("\n\
        lqc2 $vf1,0x0(%0)\n\
        lqc2 $vf2,0x10(%0)\n\
    ": :"r"(vf));
}

void SetVF2Register(sceVu0FVECTOR vf2reg)
{
    Vu0CopyVector(vf12reg[1], vf2reg);
}

void GetVF2Register(float *vf2reg)
{
    Vu0CopyVector(vf2reg, vf12reg[1]);
}

void printVector(sceVu0FVECTOR v)
{
    printf("(%f %f %f %f)\n", v[0], v[1], v[2], v[3]);
}

void printVectorC(sceVu0FVECTOR v, char *s)
{
    printf("%s (%ff,%ff,%ff,%ff)\n", s, v[0], v[1], v[2], v[3]);
}

void printMat(sceVu0FMATRIX m0)
{
    int i;

    for(i = 0; i < 4; i++)
    {
        printf("%f %f %f %f\n", m0[0][i], m0[1][i], m0[2][i], m0[3][i]);
    }
}

void printMatC(sceVu0FMATRIX m0, char *s)
{
    printf("%s\n", s);
    printMat(m0);
}

void printLMatC(sceVu0FMATRIX m0, char *str)
{
    int i;

    printf("%s\n",str);

    for(i = 0; i < 3; i++)
    {
        printf("%f %f %f\n",m0[0][i], m0[1][i], m0[2][i]);
    }
}

sceVu0FVECTOR *objwork = NULL;
SgSourceChainTag *cachetag = NULL;
int vu1tag_num = 0;
int sbuffer_p = 0;
int edge_check = 0;
int ptag = 0;
qword *SgWorkBase = NULL;
SgVULightCoord *SgLightCoordp = NULL;
SgVULightParallel *SgLightParallelp = NULL;
SgVULightSpot *SgLightSpotp = NULL;
SgVULightPoint *SgLightPointp = NULL;
int SgInfiniteNum = 0;
int SgPointNum = 0;
int SgPointGroupNum = 0;
int SgSpotNum = 0;
int SgSpotGroupNum = 0;
void *sgd_top_addr = NULL;
sceDmaChan *dvif0 = NULL;
sceDmaChan *dvif1 = NULL;
sceDmaChan *dgif = NULL;
sceDmaChan *dfspr = NULL;
SgSourceChainTag *cachetagdbuf[2] = {0};
sceVu0FVECTOR *objworkdbuf[2] = {0};
int tagswap = 0;
int vu_prog_num = 0;
int clip_value_check = 0;
int wscissor_flg = 0;
u_int *pGroupPacket = NULL;

void SgSetWsMtx(sceVu0FMATRIX m0)
{
    Vu0CopyMatrix(SgWSMtx, m0);
}

void SgSetClipMtx(sceVu0FMATRIX m0)
{
    Vu0CopyMatrix(SgCMtx, m0);
}

void SgSetClipVMtx(sceVu0FMATRIX m0)
{
    Vu0CopyMatrix(SgCMVtx, m0);
}

void SgSetWorkBase(qword *base)
{
    SgWorkBase = base;
}

qword* SgGetWorkBase()
{
    return SgWorkBase;
}

void SgSetProjection(float scrz)
{
    sceVu0FVECTOR tmp;

    tmp[3] = scrz;

    __asm__ volatile ("\n\
        lqc2    $vf12,0(%0)\n\
        vdiv    Q,$vf0w,$vf12w\n\
        vwaitq  \n\
        vmulq.w $vf1w,$vf0w,Q\n\
        ": : "r" (tmp));
}

float SgGetProjection()
{
    sceVu0FVECTOR tmp;

    __asm__ volatile ("\n\
        sqc2 $vf1,0(%0)\n\
        ": : "r" (tmp));

    return tmp[3];
}

void SgInit3D()
{
    u_int prim[4];
    SgSourceChainTag ftag;

    SgSpotLight = NULL;
    SgPointLight = NULL;
    SgSpotNum = 0;
    SgPointNum = 0;

    loadtri2_flg = 1;
    loadbw_flg = 0;

    dvif0 = sceDmaGetChan(0);
    dvif1 = sceDmaGetChan(1);
    dgif = sceDmaGetChan(2);
    dfspr = sceDmaGetChan(8);

    dfspr->chcr.TTE = 0;
    dgif->chcr.TTE = 1;
    dvif1->chcr.TTE = 0;

    vu_prog_num = -1;
    wscissor_flg = 0;

    SetClipValue(-1.0f, 1.0f, -1.0f, 1.0f);
    InitialDmaBuffer();
    Set12Register();
    ShadowInit();
}

void GetMatrixFromQuaternion(sceVu0FMATRIX quat, sceVu0FVECTOR qvert)
{
    sceVu0FMATRIX rmat;
    sceVu0FMATRIX lmat;

    rmat[0][0] = +qvert[3];
    rmat[0][1] = +qvert[2];
    rmat[0][2] = -qvert[1];
    rmat[0][3] = +qvert[0];
    rmat[1][0] = -qvert[2];
    rmat[1][1] = +qvert[3];
    rmat[1][2] = +qvert[0];
    rmat[1][3] = +qvert[1];
    rmat[2][0] = +qvert[1];
    rmat[2][1] = -qvert[0];
    rmat[2][2] = +qvert[3];
    rmat[2][3] = +qvert[2];
    rmat[3][0] = -qvert[0];
    rmat[3][1] = -qvert[1];
    rmat[3][2] = -qvert[2];
    rmat[3][3] = +qvert[3];

    lmat[0][0] = +qvert[3];
    lmat[0][1] = +qvert[2];
    lmat[0][2] = -qvert[1];
    lmat[0][3] = -qvert[0];
    lmat[1][0] = -qvert[2];
    lmat[1][1] = +qvert[3];
    lmat[1][2] = +qvert[0];
    lmat[1][3] = -qvert[1];
    lmat[2][0] = +qvert[1];
    lmat[2][1] = -qvert[0];
    lmat[2][2] = +qvert[3];
    lmat[2][3] = -qvert[2];
    lmat[3][0] = +qvert[0];
    lmat[3][1] = +qvert[1];
    lmat[3][2] = +qvert[2];
    lmat[3][3] = +qvert[3];

    __asm__ volatile ("\n\
        lqc2         $vf12,0(%1)\n\
        lqc2         $vf13,0x10(%1)\n\
        lqc2         $vf14,0x20(%1)\n\
        lqc2         $vf15,0x30(%1)\n\
        lqc2         $vf16,0(%2)\n\
        lqc2         $vf17,0x10(%2)\n\
        lqc2         $vf18,0x20(%2)\n\
        lqc2         $vf19,0x30(%2)\n\
        vmulax.xyzw  ACCxyzw,$vf12xyzw,$vf16x\n\
        vmadday.xyzw ACCxyzw,$vf13xyzw,$vf16y\n\
        vmaddaz.xyzw ACCxyzw,$vf14xyzw,$vf16z\n\
        vmaddw.xyzw  $vf16xyzw,$vf15xyzw,$vf16w\n\
        vmulax.xyzw  ACCxyzw,$vf12xyzw,$vf17x\n\
        vmadday.xyzw ACCxyzw,$vf13xyzw,$vf17y\n\
        vmaddaz.xyzw ACCxyzw,$vf14xyzw,$vf17z\n\
        vmaddw.xyzw  $vf17xyzw,$vf15xyzw,$vf17w\n\
        vmulax.xyzw  ACCxyzw,$vf12xyzw,$vf18x\n\
        vmadday.xyzw ACCxyzw,$vf13xyzw,$vf18y\n\
        vmaddaz.xyzw ACCxyzw,$vf14xyzw,$vf18z\n\
        vmaddw.xyzw  $vf18xyzw,$vf15xyzw,$vf18w\n\
        vmulax.xyzw  ACCxyzw,$vf12xyzw,$vf19x\n\
        vmadday.xyzw ACCxyzw,$vf13xyzw,$vf19y\n\
        vmaddaz.xyzw ACCxyzw,$vf14xyzw,$vf19z\n\
        vmaddw.xyzw  $vf19xyzw,$vf15xyzw,$vf19w\n\
        sqc2         $vf16,0(%0)\n\
        sqc2         $vf17,0x10(%0)\n\
        sqc2         $vf18,0x20(%0)\n\
        sqc2         $vf19,0x30(%0)\n\
        ": : "r" (quat), "r" (rmat), "r" (lmat)
    );
}

void GetMatrixRotateAxis(sceVu0FMATRIX quat, float *axis, float theta)
{
    sceVu0FVECTOR qvert;
    float theta2;

    theta2 = theta * 0.5f;
    sceVu0ScaleVector(qvert, axis, SgSinf(theta2));

    qvert[3] = SgCosf(theta2);
    GetMatrixFromQuaternion(quat, qvert);
}

float SgSinf(float rad)
{
    rad = asm_1(rad);

    if (rad < 0.0f)
    {
        rad += (PI * 2);
    }

    if (((PI * 3) / 2) < rad)
    {
        rad -= (PI * 2);
    }
    else if ((PI / 2) < rad)
    {
        rad = PI - rad;
    }

    rad = asm_2(rad);

    return rad;
}

float SgCosf(float rad)
{
    return SgSinf(rad + (PI / 2));
}

float SgSinfd(float degree)
{
    float rad;

    rad = asm_3(degree);

    if (rad < 0.0f)
    {
        rad += (PI * 2);
    }

    if (((PI * 3) / 2) < rad)
    {
        rad -= (PI * 2);
    }
    else if ((PI / 2) < rad)
    {
        rad = PI - rad;
    }

    rad = asm_2(rad);

    return rad;
}

float SgCosfd(float degree)
{
    return SgSinfd(degree + 90.0f);
}

float SgACosf(float ccos)
{
    float sqrt;

    sqrt = SgSqrtf(1.0f - ccos * ccos);
    return SgAtan2f(sqrt, ccos);
}

static void GetATanf(sceVu0FVECTOR *tmpv)
{
     __asm__ volatile("\n\
        lqc2      $vf12,0(%0)\n\
        lqc2      $vf13,0x10(%0)\n\
        lqc2      $vf16,0x20(%0)\n\
        vaddw.x   $vf15x,$vf12x,$vf0w\n\
        vsubw.x   $vf14x,$vf12x,$vf0w\n\
        vdiv      Q,$vf14x,$vf15x\n\
        vwaitq    \n\
        vaddq.x   $vf12x,$vf0x,Q\n\
        vmulx.x   $vf14x,$vf12x,$vf12x\n\
        vmulay.x  ACCx,$vf12x,$vf12y\n\
        vmulx.x   $vf15x,$vf14x,$vf12x\n\
        vmaddaz.x ACCx,$vf15x,$vf12z\n\
        vmulx.x   $vf15x,$vf15x,$vf14x\n\
        vmaddaw.x ACCx,$vf15x,$vf12w\n\
        vmulx.x   $vf15x,$vf15x,$vf14x\n\
        vmaddax.x ACCx,$vf15x,$vf13x\n\
        vmulx.x   $vf15x,$vf15x,$vf14x\n\
        vmadday.x ACCx,$vf15x,$vf13y\n\
        vmulx.x   $vf15x,$vf15x,$vf14x\n\
        vmaddaz.x ACCx,$vf15x,$vf13z\n\
        vmulx.x   $vf15x,$vf15x,$vf14x\n\
        vmaddaw.x ACCx,$vf15x,$vf13w\n\
        vmulx.x   $vf15x,$vf15x,$vf14x\n\
        vmaddx.x  $vf12x,$vf15x,$vf16x\n\
        vaddy.x   $vf12x,$vf12x,$vf16y\n\
        sqc2      $vf12,0(%0)\n\
        ": :"r" (tmpv)
    );
}

float SgAtanf(float x)
{
    sceVu0FVECTOR tmpv[3];

    // the followings are most likely polynomial coefficients
    // used to approximate arctan(x)

    if (x >= 0.0f)
    {
        if (x > 1.0f)
        {
            tmpv[0][0] = 1.0f / x;
            tmpv[0][1] = +0.99999934f;
            tmpv[0][2] = -0.33329856f;
            tmpv[0][3] = +0.19946536f;
            tmpv[1][0] = -0.13908534f;
            tmpv[1][1] = +0.09642004f;
            tmpv[1][2] = -0.055909887f;
            tmpv[1][3] = +0.021861229f;
            tmpv[2][0] = -0.0040540579f;
            tmpv[2][1] = +(PI / 4);

            GetATanf(tmpv);

            return (PI / 2) - tmpv[0][0];
        }
        else
        {
            tmpv[0][0] = x;
            tmpv[0][1] = +0.99999934f;
            tmpv[0][2] = -0.33329856f;
            tmpv[0][3] = +0.19946536f;
            tmpv[1][0] = -0.13908534f;
            tmpv[1][1] = +0.09642004f;
            tmpv[1][2] = -0.055909887f;
            tmpv[1][3] = +0.021861229f;
            tmpv[2][0] = -0.0040540579f;
            tmpv[2][1] = +(PI / 4);

            GetATanf(tmpv);

            return tmpv[0][0];
        }
    }
    else
    {
        return -SgAtanf(-x);
    }

}

float SgAtan2f(float y, float x)
{
    float rad;

    if (x == 0.0f)
    {
        if (y < 0.0f)
        {
            return -PI / 2;
        }
        else
        {
            return +PI / 2;
        }
    }

    rad = SgAtanf(y / x);

    if (x < 0.0f)
    {
        rad += PI;

        if (PI < rad)
        {
            rad -= PI * 2;
        }
    }

    return rad;
}

float SgSqrtf(float len)
{
    float ret;

    __asm__ volatile("\n\
        qmtc2   %0,$vf12\n\
        ": :"r" (len) : "v0"
    );

    __asm__ volatile("\n\
        vsqrt   Q,$vf12x\n\
        vwaitq  \n\
        vaddq.x $vf12x,$vf0x,Q\n\
    ");

    __asm__ volatile("\n\
        qmfc2   %0,$vf12\n\
        ": "=r"(ret)
    );

    return ret;
}

float SgRSqrtf(float len)
{
    float ret;

    __asm__ volatile("\n\
        qmtc2   %0,$vf12\n\
        ": :"r" (len) : "v0"
    );

    __asm__ volatile("\n\
        vrsqrt   Q,$vf0w,$vf12x\n\
        vwaitq  \n\
        vaddq.x $vf12x,$vf0x,Q\n\
    ");

    __asm__ volatile("\n\
        qmfc2   %0,$vf12\n\
        ": "=r"(ret)
    );

    return ret;
}

static void GetSgCalclen(sceVu0FVECTOR tmpv)
{
    __asm__ volatile("\n\
        lqc2     $vf12,0(%0)\n\
        vmul.xyz $vf12xyz,$vf12xyz,$vf12xyz\n\
        vaddy.x  $vf12x,$vf12x,$vf12y\n\
        vaddz.x  $vf12x,$vf12x,$vf12z\n\
        vsqrt    Q,$vf12x\n\
        vwaitq   \n\
        vaddq.x  $vf12x,$vf0x,Q\n\
        sqc2     $vf12,0(%0)\n\
        ": : "r"(tmpv)
    );
}

float SgCalcLen(float x, float y, float z)
{
    sceVu0FVECTOR tmpv;

    tmpv[0] = x;
    tmpv[1] = y;
    tmpv[2] = z;

    GetSgCalclen(tmpv);

    return tmpv[0];
}

float _CalcLen(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
    sceVu0FVECTOR tmpv;

    __asm__ volatile("\n\
        lqc2     $vf12,0(%0)\n\
        lqc2     $vf13,0(%1)\n\
        vsub.xyz $vf14xyz,$vf12xyz,$vf13xyz\n\
        vmul.xyz $vf14xyz,$vf14xyz,$vf14xyz\n\
        vaddy.x  $vf14x,$vf14x,$vf14y\n\
        vaddz.x  $vf14x,$vf14x,$vf14z\n\
        vsqrt    Q, $vf14x\n\
        vwaitq   \n\
        vaddq.x  $vf14x,$vf0x,Q\n\
        sqc2     $vf14,0(%2)\n\
        ": : "r"(v0), "r"(v1), "r"(tmpv)
    );

    return tmpv[0];
}

void _MulRotMatrix(sceVu0FMATRIX ans, sceVu0FMATRIX m0, sceVu0FMATRIX m1)
{
    __asm__ volatile("\n\
        lqc2        $vf12,0(%0)\n\
        lqc2        $vf13,0x10(%0)\n\
        lqc2        $vf14,0x20(%0)\n\
        lqc2        $vf15,0(%1)\n\
        lqc2        $vf16,0x10(%1)\n\
        lqc2        $vf17,0x20(%1)\n\
        vmulax.xyz  ACCxyz,$vf12xyz,$vf15x\n\
        vmadday.xyz ACCxyz,$vf13xyz,$vf15y\n\
        vmaddz.xyz  $vf15xyz,$vf14xyz,$vf15z\n\
        vmulax.xyz  ACCxyz,$vf12xyz,$vf16x\n\
        vmadday.xyz ACCxyz,$vf13xyz,$vf16y\n\
        vmaddz.xyz  $vf16xyz,$vf14xyz,$vf16z\n\
        vmulax.xyz  ACCxyz,$vf12xyz,$vf17x\n\
        vmadday.xyz ACCxyz,$vf13xyz,$vf17y\n\
        vmaddz.xyz  $vf17xyz,$vf14xyz,$vf17z\n\
        sqc2        $vf15,0(%2)\n\
        sqc2        $vf16,0x10(%2)\n\
        sqc2        $vf17,0x20(%2)\n\
        ": : "r"(m0), "r"(m1), "r"(ans)
    );
}

void _MulMatrix(sceVu0FMATRIX ans, sceVu0FMATRIX m0, sceVu0FMATRIX m1)
{
    __asm__ volatile("\n\
        lqc2         $vf12,0(%0)\n\
        lqc2         $vf13,0x10(%0)\n\
        lqc2         $vf14,0x20(%0)\n\
        lqc2         $vf15,0x30(%0)\n\
        lqc2         $vf16,0(%1)\n\
        lqc2         $vf17,0x10(%1)\n\
        lqc2         $vf18,0x20(%1)\n\
        lqc2         $vf19,0x30(%1)\n\
        vmulax.xyzw  ACCxyzw,$vf12xyzw,$vf16x\n\
        vmadday.xyzw ACCxyzw,$vf13xyzw,$vf16y\n\
        vmaddaz.xyzw ACCxyzw,$vf14xyzw,$vf16z\n\
        vmaddw.xyzw  $vf16xyzw,$vf15xyzw,$vf16w\n\
        vmulax.xyzw  ACCxyzw,$vf12xyzw,$vf17x\n\
        vmadday.xyzw ACCxyzw,$vf13xyzw,$vf17y\n\
        vmaddaz.xyzw ACCxyzw,$vf14xyzw,$vf17z\n\
        vmaddw.xyzw  $vf17xyzw,$vf15xyzw,$vf17w\n\
        vmulax.xyzw  ACCxyzw,$vf12xyzw,$vf18x\n\
        vmadday.xyzw ACCxyzw,$vf13xyzw,$vf18y\n\
        vmaddaz.xyzw ACCxyzw,$vf14xyzw,$vf18z\n\
        vmaddw.xyzw  $vf18xyzw,$vf15xyzw,$vf18w\n\
        vmulax.xyzw  ACCxyzw,$vf12xyzw,$vf19x\n\
        vmadday.xyzw ACCxyzw,$vf13xyzw,$vf19y\n\
        vmaddaz.xyzw ACCxyzw,$vf14xyzw,$vf19z\n\
        vmaddw.xyzw  $vf19xyzw,$vf15xyzw,$vf19w\n\
        sqc2         $vf16,0(%2)\n\
        sqc2         $vf17,0x10(%2)\n\
        sqc2         $vf18,0x20(%2)\n\
        sqc2         $vf19,0x30(%2)\n\
        ": : "r"(m0), "r"(m1), "r"(ans)
    );
}

void _SetMulMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1)
{
    __asm__ volatile("\n\
        lqc2         $vf12,0(%0)\n\
        lqc2         $vf13,0x10(%0)\n\
        lqc2         $vf14,0x20(%0)\n\
        lqc2         $vf15,0x30(%0)\n\
        lqc2         $vf16,0(%1)\n\
        lqc2         $vf17,0x10(%1)\n\
        lqc2         $vf18,0x20(%1)\n\
        lqc2         $vf19,0x30(%1)\n\
        vmulax.xyzw  ACCxyzw,$vf12xyzw,$vf16x\n\
        vmadday.xyzw ACCxyzw,$vf13xyzw,$vf16y\n\
        vmaddaz.xyzw ACCxyzw,$vf14xyzw,$vf16z\n\
        vmaddw.xyzw  $vf4xyzw,$vf15xyzw,$vf16w\n\
        vmulax.xyzw  ACCxyzw,$vf12xyzw,$vf17x\n\
        vmadday.xyzw ACCxyzw,$vf13xyzw,$vf17y\n\
        vmaddaz.xyzw ACCxyzw,$vf14xyzw,$vf17z\n\
        vmaddw.xyzw  $vf5xyzw,$vf15xyzw,$vf17w\n\
        vmulax.xyzw  ACCxyzw,$vf12xyzw,$vf18x\n\
        vmadday.xyzw ACCxyzw,$vf13xyzw,$vf18y\n\
        vmaddaz.xyzw ACCxyzw,$vf14xyzw,$vf18z\n\
        vmaddw.xyzw  $vf6xyzw,$vf15xyzw,$vf18w\n\
        vmulax.xyzw  ACCxyzw,$vf12xyzw,$vf19x\n\
        vmadday.xyzw ACCxyzw,$vf13xyzw,$vf19y\n\
        vmaddaz.xyzw ACCxyzw,$vf14xyzw,$vf19z\n\
        vmaddw.xyzw  $vf7xyzw,$vf15xyzw,$vf19w\n\
        ": : "r"(m0), "r"(m1)
    );
}

void _CalcLenASM(sceVu0FVECTOR out, sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
    __asm__ volatile("\n\
        lqc2     $vf12,0(%0)\n\
        lqc2     $vf13,0(%1)\n\
        vsub.xyz $vf14xyz,$vf12xyz,$vf13xyz\n\
        vmul.xyz $vf14xyz,$vf14xyz,$vf14xyz\n\
        vaddy.x  $vf14x,$vf14x,$vf14y\n\
        vaddz.x  $vf14x,$vf14x,$vf14z\n\
        vsqrt    Q, $vf14x\n\
        vwaitq   \n\
        vaddq.x  $vf14x,$vf0x,Q\n\
        sqc2     $vf14,0(%2)\n\
        ": : "r"(v0), "r"(v1), "r"(out)
    );
}

void _NormalizeVector(sceVu0FVECTOR v, sceVu0FVECTOR v0)
{
    __asm__ volatile("\n\
        lqc2      $vf12,0(%0)\n\
        vmul.xyz  $vf13xyz,$vf12xyz,$vf12xyz\n\
        vaddy.x   $vf13x,$vf13x,$vf13y\n\
        vaddz.x   $vf13x,$vf13x,$vf13z\n\
        vrsqrt    Q,$vf0w,$vf13x\n\
        vmul.w    $vf13w,$vf12w,$vf0w\n\
        vwaitq    \n\
        vmulq.xyz $vf13xyz,$vf12xyz,Q\n\
        sqc2      $vf13,0(%1)\n\
        ": : "r"(v0), "r"(v)
    );
}

void _NormalizeVector2(sceVu0FVECTOR v, sceVu0FVECTOR v0)
{
    __asm__ volatile("\n\
        lqc2      $vf12,0(%0)\n\
        vmul.xyz  $vf13xyz,$vf12xyz,$vf12xyz\n\
        vaddy.x   $vf13x,$vf13x,$vf13y\n\
        vaddz.x   $vf13x,$vf13x,$vf13z\n\
        vrsqrt    Q,$vf0w,$vf13x\n\
        vwaitq    \n\
        vmulq.xyz $vf13xyz,$vf12xyz,Q\n\
        vmulq.w   $vf13w,$vf0w,Q\n\
        sqc2      $vf13,0(%1)\n\
        ": : "r"(v0), "r"(v)
    );
}

void _ApplyRotMatrix(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
    __asm__ volatile("\n\
        lqc2         $vf13,0(%0)\n\
        vmulax.xyzw  ACCxyzw,$vf4xyzw,$vf13x\n\
        vmadday.xyzw ACCxyzw,$vf5xyzw,$vf13y\n\
        vmaddz.xyzw  $vf12xyzw,$vf6xyzw,$vf13z\n\
        sqc2         $vf12,0(%1)\n\
        ": : "r"(v1), "r"(v0)
    );
}

void _ApplyMatrixXYZ(sceVu0FVECTOR v0, sceVu0FMATRIX m, sceVu0FVECTOR v1)
{
    __asm__ volatile("\n\
        lqc2        $vf12,0(%0)\n\
        lqc2        $vf13,0(%1)\n\
        lqc2        $vf14,0x10(%1)\n\
        lqc2        $vf15,0x20(%1)\n\
        vmulax.xyz  ACCxyz,$vf13xyz,$vf12x\n\
        vmadday.xyz ACCxyz,$vf14xyz,$vf12y\n\
        vmaddz.xyz  $vf16xyz,$vf15xyz,$vf12z\n\
        sqc2        $vf16,0(%2)\n\
        ": : "r"(v1), "r"(m), "r"(v0)
    );
}
