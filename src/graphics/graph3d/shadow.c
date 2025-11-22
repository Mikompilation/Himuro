#include "common.h"
#include "typedefs.h"
#include "shadow.h"

#include "ee/eestruct.h"
#include "sce/libvu0.h"
#include "sce/libgraph.h"

#include "graphics/graph3d/libsg.h"
#include "graphics/graph3d/sgdma.h"
#include "graphics/graph3d/sgsup.h"
#include "graphics/graph3d/sgsu.h"
// #include "graphics/graph3d/sgsu.h" // LoadSgProg, _SetLWMatrix0, _SetLWMatrix1 should be undeclared
#include "graphics/graph3d/sgcam.h"
#include "graphics/graph3d/sglib.h"

#define SCRATCHPAD ((u_char *)0x70000000)

#include "data/scamera.h" // SgCAMERA scamera;

static int write_flg = 0;
static sceVu0FMATRIX *LWSMtx = (sceVu0FMATRIX *)&SCRATCHPAD[0x90];
static sceVu0FMATRIX *LCMtx = (sceVu0FMATRIX *)&SCRATCHPAD[0xd0];
static sceVu0FMATRIX *LIPMatrix = (sceVu0FMATRIX *)&SCRATCHPAD[0x140];
static sceVu0IVECTOR *shadow_col = (sceVu0IVECTOR *)&SCRATCHPAD[0x180];
static u_int *common_vector = NULL;
static int shadow_dbg_flg = 0;
static int shadow_apgnum = -1;

static int write_counter;

static sceVu0FVECTOR ndirection;
static sceVu0FVECTOR spos;
static sceVu0FMATRIX InverseMatrix;
static sceVu0FMATRIX IPMatrix;
static sceVu0FMATRIX CullingMatrix;
static sceVu0FVECTOR wbbox[8];

extern void SHADOWDRAWTYPE0() __attribute__((section(".vutext")));
extern void SHADOWDRAWTYPE2() __attribute__((section(".vutext")));
extern void DIVPS_PROLOGUE() __attribute__((section(".vutext")));
extern void DPS_PROLOGUE() __attribute__((section(".vutext")));

#define SCREENX_TO_GSX_UL(x, swidth) (((2048-(swidth >> 1)) + (x)) << 4)
#define SCREENY_TO_GSY_UL(y, sheight) (((2048-(sheight >> 1)) + (y)) << 4)

#define SCREENX_TO_GSX_DR(x, swidth) (((2048+(swidth >> 1)) + (x)) << 4)
#define SCREENY_TO_GSY_DR(y, sheight) (((2048+(sheight >> 1)) + (y)) << 4)

void ShadowDbgOn()
{
    shadow_dbg_flg = 1;
}

void ShadowDbgOff()
{
    shadow_dbg_flg = 0;
}

void ShadowInit()
{
    shadowtex.addr = 0xa0000;
    shadowtex.max_width = 256;
    shadowtex.max_height = 256;
    shadowtex.scale = 1.0176f;
}

void SetShadowTexSize(u_int width, u_int height)
{
    shadowtex.max_width = width;
    shadowtex.max_height = height;
}

void SetShadowTexAddr(u_int addr)
{
    shadowtex.addr = addr;
}

void SetShadowFundamentScale(float size)
{
    shadowtex.fund_scale = size;
}

void SetShadowAssignGroup(int gnum)
{
    shadow_apgnum = gnum;
}

void TransShadowClut()
{
    #include "data/shadow_pal.h" // static qword shadow_pal[71];

    shadow_pal[0][0] = 0;
    shadow_pal[0][1] = 0;
    shadow_pal[0][2] = 0;
    shadow_pal[0][3] = DMAcall | (sizeof(shadow_pal) / sizeof(qword) - 1); // could also be a defined value - 1

    *(u_long *)&shadow_pal[1][0] = SCE_GIF_SET_TAG(1, SCE_GS_FALSE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 4);
    *(u_long *)&shadow_pal[1][2] = 0 \
        | SCE_GIF_PACKED_AD << (0 * 4)
        | SCE_GIF_PACKED_AD << (1 * 4)
        | SCE_GIF_PACKED_AD << (2 * 4)
        | SCE_GIF_PACKED_AD << (3 * 4);

    *(u_long *)&shadow_pal[2][0] = SCE_GS_SET_BITBLTBUF(0, 0, SCE_GS_PSMCT32, shadowtex.addr / 64, 1, SCE_GS_PSMCT32);
    *(u_long *)&shadow_pal[2][2] = SCE_GS_BITBLTBUF;

    *(u_long *)&shadow_pal[3][0] = SCE_GS_SET_TRXPOS(0, 0, 0, 0, 0);
    *(u_long *)&shadow_pal[3][2] = SCE_GS_TRXPOS;

    *(u_long *)&shadow_pal[4][0] = SCE_GS_SET_TRXREG(16, 16);
    *(u_long *)&shadow_pal[4][2] = SCE_GS_TRXREG;

    *(u_long *)&shadow_pal[5][0] = SCE_GS_SET_TRXDIR(0);
    *(u_long *)&shadow_pal[5][2] = SCE_GS_TRXDIR;

    *(u_long *)&shadow_pal[6][0] = SCE_GIF_SET_TAG(64, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_IMAGE, 0);

    AppendDmaTag((u_int)shadow_pal, sizeof(shadow_pal) / sizeof(qword)); // could also be a defined value
}

void GetShadowTexture()
{
    sceGsStoreImage spi;
}

void DispShadowSprite()
{
    dword *base;
    sceVu0IVECTOR urp;
    sceVu0IVECTOR dlp;

    GetShadowTexture();

    urp[0] = GS_X_COORD(320); // X (DISP_WIDTH/2 ?)
    urp[1] = GS_Y_COORD(8);   // Y
    urp[2] = 0x3A980;         // Z
    urp[3] = 0;               // F

    dlp[0] = GS_X_COORD(448); // X (DISP_HEIGHT ?)
    dlp[1] = GS_Y_COORD(72);  // Y
    dlp[2] = 0x3A980;         // Z
    dlp[3] = 0;               // F

    base = (dword *)getObjWrk() + 1;

    base[0][0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 6, SCE_GIF_PACKED, 3);
    base[0][1] = 0 \
        | SCE_GS_RGBAQ << (0 * 4)
        | SCE_GS_XYZF2 << (1 * 4)
        | SCE_GS_XYZF2 << (2 * 4);

    base++;

    ((u_int *)base[0])[0] = 0;    // R
    ((u_int *)base[0])[1] = 0x40; // G
    ((u_int *)base[0])[2] = 0;    // B
    ((u_int *)base[0])[3] = 0x80; // A

    ((u_int *)base[1])[0] = urp[0] - 16; // X
    ((u_int *)base[1])[1] = urp[1] - 16; // Y
    ((u_int *)base[1])[2] = urp[2];      // Z
    ((u_int *)base[1])[3] = 0;           // F

    ((u_int *)base[2])[0] = dlp[0] + 16; // X
    ((u_int *)base[2])[1] = dlp[1] + 16; // Y
    ((u_int *)base[2])[2] = dlp[2];      // Z
    ((u_int *)base[2])[3] = 0;           // F

    base += 3;

    base[0][0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 86, SCE_GIF_PACKED, 7);
    base[0][1] = 0 \
        | SCE_GS_TEX0_1 << (0 * 4)
        | SCE_GS_ST     << (1 * 4)
        | SCE_GS_RGBAQ  << (2 * 4)
        | SCE_GS_XYZF2  << (3 * 4)
        | SCE_GS_ST     << (4 * 4)
        | SCE_GS_RGBAQ  << (5 * 4)
        | SCE_GS_XYZF2  << (6 * 4);

    base[1][0] = SCE_GS_SET_TEX0_1(shadowtex.addr / 64, shadowtex.width / 64, SCE_GS_PSMCT16, shadowtex.width_p, shadowtex.height_p, 1, SCE_GS_MODULATE, 0, SCE_GS_PSMCT32, 0, 0, 1);

    base += 2;

    ((float *)base[0])[0] = 0.0f; // S
    ((float *)base[0])[1] = 0.0f; // T
    ((float *)base[0])[2] = 1.0f; // Q
    ((float *)base[0])[3] = 0.0f; // <unused>

    ((u_int *)base[1])[0] = 0x80; // R
    ((u_int *)base[1])[1] = 0x80; // G
    ((u_int *)base[1])[2] = 0x80; // B
    ((u_int *)base[1])[3] = 0x80; // A

    Vu0CopyVector(*(sceVu0FVECTOR *)base[2], *(sceVu0FVECTOR *)urp); // XYZF

    base += 3;

    ((float *)base[0])[0] = 1.0f; // S
    ((float *)base[0])[1] = 1.0f; // T
    ((float *)base[0])[2] = 1.0f; // Q
    ((float *)base[0])[3] = 0.0f; // <unused>

    ((u_int *)base[1])[0] = 0x80; // R
    ((u_int *)base[1])[1] = 0x80; // G
    ((u_int *)base[1])[2] = 0x80; // B
    ((u_int *)base[1])[3] = 0x80; // A

    Vu0CopyVector(*(sceVu0FVECTOR *)base[2], *(sceVu0FVECTOR *)dlp); // XYZF

    AppendDmaBufferFromEndAddress((qword*)(base + 3));
}

void GetInverseMatrix(sceVu0FMATRIX inv, sceVu0FMATRIX org)
{
    int i;
    sceVu0FVECTOR tmpv;
    sceVu0FMATRIX rot;
    sceVu0FMATRIX scale;
    sceVu0FMATRIX trans;

    sceVu0UnitMatrix(rot);
    sceVu0UnitMatrix(trans);
    sceVu0ScaleVector(trans[3], org[3], -1.0f);

    for (i = 0; i < 3; i++)
    {
        rot[i][0] = org[0][i];
        rot[i][1] = org[1][i];
        rot[i][2] = org[2][i];
    }

    sceVu0UnitMatrix(scale);

    tmpv[3] = 0.0f;

    for (i = 0; i < 3; i++)
    {
        scale[i][i] = SgRSqrtf(sceVu0InnerProduct(rot[i], rot[i]));
    }

    _MulMatrix(inv, rot, scale);
    _MulMatrix(inv, inv, trans);
}

void SetVU1HeaderShadow()
{
    sceVu0FVECTOR *svec;
    sceVu0FVECTOR *dvec;
    int i;

    svec = (sceVu0FVECTOR *)getObjWrk();
    dvec = (sceVu0FVECTOR *)&SCRATCHPAD[0];

    for (i = 0; i < 26; svec++, dvec++, i++)
    {
        Vu0CopyVector(*svec, *dvec);
    }

    AppendDmaBuffer(26);
    FlushModel(0);
}

static void _CalcWeightedVertexSM(sceVu0FVECTOR dp, sceVu0FVECTOR v)
{
    asm volatile("                               \n\
        lqc2            $vf12, 0(%0)             \n\
        vmulax.xyzw     ACC,   $vf4,     $vf12x  \n\
        vmadday.xyzw    ACC,   $vf5,     $vf12y  \n\
        vmaddaz.xyzw    ACC,   $vf6,     $vf12z  \n\
        vmaddw.xyz      $vf13, $vf7,     $vf0w   \n\
        vsubw.w         $vf14, $vf0,     $vf12   \n\
        vmove.w         $vf13, $vf12             \n\
        lqc2            $vf12, 0x10(%0)          \n\
        vmulax.xyzw     ACC,   $vf8,     $vf12x  \n\
        vmadday.xyzw    ACC,   $vf9,     $vf12y  \n\
        vmaddaz.xyzw    ACC,   $vf10,    $vf12z  \n\
        vmaddw.xyz      $vf14, $vf11,    $vf0w   \n\
        vmulaw.xyz      ACC,   $vf13,    $vf13w  \n\
        vmaddw.xyz      $vf15, $vf14,    $vf14w  \n\
        vmove.w         $vf15, $vf0              \n\
        sqc2            $vf15, 0(%1)             \n\
        ": :"r"(v), "r"(dp)
    );
}

static void _CalcWeightedVertexBufferSM(sceVu0FVECTOR dp, sceVu0FVECTOR v)
{
    asm volatile("                               \n\
        lqc2            $vf12, 0(%0)             \n\
        vmulax.xyzw     ACC,   $vf4,     $vf12x  \n\
        vmadday.xyzw    ACC,   $vf5,     $vf12y  \n\
        vmaddaz.xyzw    ACC,   $vf6,     $vf12z  \n\
        vmaddw.xyz      $vf13, $vf7,     $vf0w   \n\
        vsubw.w         $vf14, $vf0,     $vf12   \n\
        vmove.w         $vf13, $vf12             \n\
        lqc2            $vf12, 0x10(%0)          \n\
        vmulax.xyzw     ACC,   $vf8,     $vf12x  \n\
        vmadday.xyzw    ACC,   $vf9,     $vf12y  \n\
        vmaddaz.xyzw    ACC,   $vf10,    $vf12z  \n\
        vmaddw.xyz      $vf14, $vf11,    $vf0w   \n\
        vmulaw.xyz      ACC,   $vf13,    $vf13w  \n\
        vmaddw.xyz      $vf15, $vf14,    $vf14w  \n\
        vmove.w         $vf15, $vf0              \n\
        sqc2            $vf15, 0(%1)             \n\
        ": :"r"(v), "r"(dp)
    );
}

static void _CalcVertexSM(sceVu0FVECTOR dp, sceVu0FVECTOR v)
{
    asm volatile("\n\
        lqc2            $vf13, 0(%0)          \n\
        vmulax.xyzw     ACC,   $vf4,  $vf13x  \n\
        vmadday.xyzw    ACC,   $vf5,  $vf13y  \n\
        vmaddaz.xyzw    ACC,   $vf6,  $vf13z  \n\
        vmaddw.xyzw     $vf12, $vf7,  $vf13w  \n\
        sqc2            $vf12, 0(%1)          \n\
        ": :"r"(v), "r"(dp)
    );
}

void CalcVertexBufferShadow(u_int *prim)
{
    int i;
    int j;
    VERTEXLIST *vli;
    sceVu0FVECTOR *vpd;
    sceVu0FVECTOR *vps;

    if (prim[3] == 0)
    {
        return;
    }

    vli = (VERTEXLIST *)lphead->pWeightedList;

    if (vli == NULL)
    {
        return;
    }

    vps = lphead->pWeightedVertex;
    vpd = vertex_buffer;

    for (i = 0; i < vli->list_num; i++)
    {
        _SetLWMatrix0(lcp[vli->lists[i].cn0].workm);
        _SetLWMatrix1(lcp[vli->lists[i].cn1].workm);

        for (j = 0; j < vli->lists[i].vIdx; vpd++, vps += 2, j++)
        {
            _CalcWeightedVertexSM(*vpd, *vps);
        }
    }

}

u_int* SetVUVNDataShadowModel(u_int *prim)
{
    int i;
    VUVN_PRIM *vh;
    sceVu0FVECTOR *vp;
    sceVu0FVECTOR tmpvec;
    char *cn;

    vh = (VUVN_PRIM *)&prim[2];
    vp = (sceVu0FVECTOR *)getObjWrk();

    ((u_int *)&vp[0])[0] = 0x1000404;
    ((u_int *)&vp[0])[1] = 0x6C018000;
    ((u_int *)&vp[0])[2] = prim[8];
    ((u_int *)&vp[0])[3] = prim[9];

    ((u_int *)&vp[1])[0] = prim[10];
    ((u_int *)&vp[1])[1] = prim[11];
    ((u_int *)&vp[1])[2] = 0x1000102;
    ((u_int *)&vp[1])[3] = (*(short *)&prim[2] << 0x10) | 0x6c008001;

    prim = &prim[12];
    vp = vp + 2;

    switch (((char *)vh)[5])
    {
    case 0:
        _SetLWMatrix0((float (*) [4])0x70000430);

        for (i = 0; i < vh->vnum; vp++, prim += 2, i++)
        {
            Vu0CopyVector(vp[0], *(sceVu0FVECTOR *)*prim);
        }
    break;
    case 2:
        if (lphead->pWeightedList != NULL)
        {
            for (i = 0; i < vh->vnum; i++, vp++, prim += 2)
            {
                Vu0CopyVector(vp[0], *(sceVu0FVECTOR *)*prim);
            }
        }
        else
        {
            cn = (char *)*prim;

            _SetLWMatrix0(lcp[cn[0x1c]].workm);
            _SetLWMatrix1(lcp[cn[0x1d]].workm);

            for (i = 0; i < vh->vnum; i++, vp++, prim += 2)
            {
                _CalcWeightedVertexSM(*vp, *(sceVu0FVECTOR *)*prim);
            }
        }
    break;
    case 3:
        if (lphead->pWeightedList != NULL)
        {
            for (i = 0; i < vh->vnum; i++, vp++, prim += 2)
            {
                Vu0CopyVector(vp[0], *(sceVu0FVECTOR *)*prim);
            }
        }
        else
        {
            for (i = 0; i < vh->vnum; i++)
            {
                cn = (char *)*prim;
                _SetLWMatrix0(lcp[cn[0x1c]].workm);
                _SetLWMatrix1(lcp[cn[0x1d]].workm);
                _CalcWeightedVertexSM(*vp, *(sceVu0FVECTOR *)*prim);
                vp++;
                prim += 2;
            }
        }
    break;
    default:
        for (i = 0; i < vh->vnum; i++, vp++, prim += 2)
        {
            Vu0CopyVector(*vp, *(sceVu0FVECTOR *)*prim);
        }
    break;
    }

    return (u_int *)vp;
}

void ShadowModelMesh(u_int *prim)
{
    int mtype;
    u_int *read_p;
    short *tmp;

    tmp = (short *)vuvnprim;

    mtype = ((u_char *)prim)[13];

    switch (mtype & (0x1 | 0x2 | 0x10 | 0x40 | 0x80)) // 0xd3
    {
    case 0:
        read_p = SetVUVNDataShadowModel(vuvnprim);

        read_p[0] = 0x14000000 | ((u_int)SHADOWDRAWTYPE0 >> 3);
        read_p[1] = 0x17000000;
        read_p[2] = 0x11000000;
        read_p[3] = 0x17000000;

        AppendDmaTag((u_int)&prim[4], prim[2]);
        AppendDmaBuffer(tmp[4] + 3);
        FlushModel(0);
    break;
    case 2:
        read_p = SetVUVNDataShadowModel(vuvnprim);

        read_p[0] = 0x14000000 | ((u_int)SHADOWDRAWTYPE2 >> 3);
        read_p[1] = 0x17000000;
        read_p[2] = 0x11000000;
        read_p[3] = 0x17000000;

        AppendDmaTag((u_int)&prim[4], prim[2]);
        AppendDmaBuffer(tmp[4] + 3);
        FlushModel(0);
    break;
    case 128:
        AppendDmaTag((u_int)&prim[4], prim[2]);
        AppendDmaTag((u_int)&((u_char *)vuvnprim)[16], ((u_char *)vuvnprim)[12]);

        read_p = (u_int *)getObjWrk();
        read_p[0] = 0x14000000 | ((u_int)SHADOWDRAWTYPE0 >> 3);
        read_p[1] = 0x17000000;
        read_p[2] = 0x11000000;
        read_p[3] = 0x17000000;

        AppendDmaBuffer(1);
        FlushModel(0);
    break;
    case 130:
        AppendDmaTag((u_int)&((u_char *)vuvnprim)[16], ((u_char *)vuvnprim)[12]);
        AppendDmaTag((u_int)&prim[4], prim[2]);

        read_p = (u_int *)getObjWrk();

        read_p[0] = 0x14000000 | ((u_int)SHADOWDRAWTYPE2 >> 3);
        read_p[1] = 0x17000000;
        read_p[2] = 0x11000000;
        read_p[3] = 0x17000000;

        AppendDmaBuffer(1);
        FlushModel(0);
    break;
    }
}

void DrawShadowModelPrim(u_int *prim)
{
    u_int *read_p;

    if (prim == NULL)
    {
        return;
    }

    while (prim[0] != NULL)
    {
        switch(prim[1])
        {
        case 0:
            vuvnprim = prim;
        break;
        case 1:
            ShadowModelMesh(prim);
        break;
        case 3:
            if (prim[3] != 0)
            {
                CalcVertexBufferShadow(prim);

                read_p = (u_int *)getObjWrk();

                read_p[0] = 0;
                read_p[1] = 0;
                read_p[2] = 0x01000404;
                read_p[3] = 0x6c040008;

                Vu0CopyMatrix(*(sceVu0FMATRIX *)&read_p[4], SgWSMtx);
                AppendDmaBuffer(5);
            }
        break;
        case 4:
            read_p = (u_int *)getObjWrk();

            read_p[0] = 0;
            read_p[1] = 0;
            read_p[2] = 0x01000404;
            read_p[3] = 0x6c040008;

            _MulMatrix(*(sceVu0FMATRIX *)&read_p[4], SgWSMtx, lcp[prim[2]].lwmtx);
            AppendDmaBuffer(5);
        break;
        }

        prim = (u_int *)prim[0];
    }
}

void SetUpShadowModel()
{
    u_int *datap;

    LoadSgProg(3);

    datap = (u_int *)getObjWrk();

    datap[0] = 0x11000000;
    datap[1] = 0;
    datap[2] = 0x01000404;
    datap[3] = 0x6c110000;

    datap += 4;

    *((u_long *)&datap[12]) = SCE_GIF_SET_TAG(0, SCE_GS_TRUE, SCE_GS_TRUE, 68, SCE_GIF_PACKED, 2);
    datap[14] = 0 \
        | SCE_GS_RGBAQ    << (0 * 4)
        | SCE_GS_XYZF2 << (1 * 4);
    datap[15] = 0;

    *((u_long *)&datap[16]) = SCE_GIF_SET_TAG(0, SCE_GS_TRUE, SCE_GS_TRUE, 84, SCE_GIF_PACKED, 3);
    datap[18] = 0 \
        | SCE_GS_ST    << (0 * 4)
        | SCE_GS_RGBAQ << (1 * 4)
        | SCE_GS_XYZF2 << (2 * 4);
    datap[19] = 0;

    Vu0CopyMatrix(*(sceVu0FMATRIX *)&datap[32], SgWSMtx);
    Vu0CopyMatrix(*(sceVu0FMATRIX *)&datap[48], SgCMtx);

    Vu0CopyVector(*(sceVu0FVECTOR *)&datap[0], vf12reg[0]);
    Vu0CopyVector(*(sceVu0FVECTOR *)&datap[4], vf12reg[1]);

    datap[64] = 0;
    datap[65] = 0;
    datap[66] = 0;
    datap[67] = 0x80;

    AppendDmaBuffer(18);

    datap = (u_int *)getObjWrk();

    datap[0] = 0x30000000;
    ((float *)datap)[1] = 0.0f;
    ((float *)datap)[2] = 0.0f;
    ((float *)datap)[3] = 1.0f;
    ((float *)datap)[4] = 1.0f;
    ((float *)datap)[5] = 0.0f;
    ((float *)datap)[6] = 0.0f;
    ((float *)datap)[7] = 0.0f;

    AppendDmaBuffer(2);
}

void DrawShadowModel(void *sgd_top, int pnum)
{
    u_int *pk;
    u_int i;
    HeaderSection *hs;

    hs = (HeaderSection *)sgd_top;

    lcp = hs->coordp;
    blocksm = hs->blocks;

    lphead = (PHEAD *)hs->phead;

    pk = (u_int *)&hs->primitives;

    SetUpShadowModel();

    if (pnum < 0)
    {
        SgSortPreProcess((u_int *)pk[0]);

        for (i = 1; i < blocksm; i++)
        {
            DrawShadowModelPrim((u_int *)pk[i]);
        }
    }
    else if (pnum == 0)
    {
        SgSortPreProcess((u_int *)pk[0]);
    }
    else
    {
        DrawShadowModelPrim((u_int *)pk[pnum]);
    }
}

void ShadowMeshDataVU(u_int *prim)
{
    int mtype;
    u_int *datap;

    mtype = ((char *)prim)[13];

    switch (mtype  & (0x40 | 0x10 | 0x2 | 0x1))
    {
    case 0:
        // values other than 0 are also ok
    break;
    case 18:
    case 50:
        AppendDmaTag((u_int)&((u_char *)vuvnprim)[16], ((u_char *)vuvnprim)[12]);
        AppendDmaTag((u_int)&prim[8], prim[2]);

        datap = (u_int *)getObjWrk();

        if (edge_check != 0)
        {
            datap[0] = 0x15000000 | ((u_int)DIVPS_PROLOGUE >> 3);
            datap[2] = 0x17000000;
            datap[1] = 0x11000000;
            datap[3] = 0x17000000;
        }
        else
        {
            datap[0] = 0x14000000 | ((u_int)DPS_PROLOGUE >> 3);
            datap[1] = 0x17000000;
            datap[2] = 0x11000000;
            datap[3] = 0x17000000;
        }

        AppendDmaBuffer(1);
        FlushModel(0);
    }
}

int ClipCheckShadow(sceVu0FVECTOR *vec, float *cul)
{
    int ret;

    asm volatile("                      \n\
        lqc2          $vf16, 0(%1)       \n\
        lqc2          $vf12, 0(%2)       \n\
        lqc2          $vf13, 0x10(%2)    \n\
        lqc2          $vf14, 0x20(%2)    \n\
        lqc2          $vf15, 0x30(%2)    \n\
        vclipw.xyz    $vf12, $vf16w      \n\
        vclipw.xyz    $vf13, $vf16w      \n\
        vclipw.xyz    $vf14, $vf16w      \n\
        vclipw.xyz    $vf15, $vf16w      \n\
        vnop                             \n\
        vnop                             \n\
        vnop                             \n\
        vnop                             \n\
        vnop                             \n\
        cfc2          %0,    $vi18       \n\
        ":"=r"(ret):"r"(cul),"r"(vec)
    );

    return ret;
}

int ShadowBoundClip(float *v0, float *v1)
{
    int ret;

    asm volatile("                             \n\
        lqc2            $vf12, 0(%1)           \n\
        vmulax.xyzw     ACC,   $vf4,   $vf12x  \n\
        vmadday.xyzw    ACC,   $vf5,   $vf12y  \n\
        vmaddaz.xyzw    ACC,   $vf6,   $vf12z  \n\
        vmaddw.xyzw     $vf12, $vf7,   $vf0w   \n\
        vclipw.xyz      $vf12, $vf12w          \n\
        sqc2            $vf12, 0(%2)           \n\
        vnop                                   \n\
        vnop                                   \n\
        vnop                                   \n\
        cfc2            %0,    $vi18           \n\
        ":"=r"(ret):"r"(v1),"r"(v0)
    );

    return ret;
}

int AppendShadowClipCheck(sceVu0FVECTOR *sts, BoundLine *bl) {
    float bmin;
    float bmax;
    float smin;
    float smax;
    float tmp;
    float tmp2;
    int i;
    int s;
    int e;
    static sceVu0FVECTOR shadowtex[4] = {
        {-0.5f, -0.5f, 0.0f, 1.0f},
        {+0.5f, -0.5f, 0.0f, 1.0f},
        {-0.5f, +0.5f, 0.0f, 1.0f},
        {+0.5f, +0.5f, 0.0f, 1.0f},
    };
    sceVu0FVECTOR kei;

    s = bl->s;
    e = bl->e;

    if (sts[e][0] != sts[s][0])
    {
        kei[0] = (sts[e][1] - sts[s][1]) / (sts[e][0] - sts[s][0]);
    }
    else
    {
        kei[0] = 0.0f;
    }

    if (kei[0] == 0.0f)
    {
        return 1;
    }

    kei[1] = -1.0f;
    kei[2] = 0.0f;
    kei[3] = 0.0f;

    bmin = bmax = sceVu0InnerProduct(sts[bl[0].s], kei);
    smin = smax = sceVu0InnerProduct(shadowtex[0], kei);

    for (i = 1; i < 4; i++)
    {
        tmp = sceVu0InnerProduct(sts[bl[i].s], kei);
        tmp2 = sceVu0InnerProduct(shadowtex[i], kei);

        if (bmax < tmp)
        {
            bmax = tmp;
        }

        if (tmp < bmin)
        {
            bmin = tmp;
        }

        if (smax < tmp2)
        {
            smax = tmp2;
        }

        if (tmp2 < smin)
        {
            smin = tmp2;
        }

    }

    if (bmin > smax || smin > bmax)
    {
        return 0;
    }

    return 1;
}

int CheckBoundingBoxShadowTrace(sceVu0FMATRIX lwmtx, sceVu0FVECTOR *tmpv, float *dir)
{
    int i;
    int clip;
    sceVu0FMATRIX tmpmat;
    static sceVu0FMATRIX clipmtx = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {-0.5f, -0.5f, -0.5f, 0.5f},
    };
    static BoundLine boundline[12] = {
        {0, 1}, {4, 5}, {2, 3}, {6, 7}, {0, 4}, {1, 5},
        {2, 6}, {3, 7}, {1, 3}, {5, 7}, {4, 6}, {0, 2},
    };
    int clip0;
    int clip1;
    int cliparray[8];
    char text[256];

    _SetMulMatrix(CullingMatrix, lwmtx);

    clip = 0x2a;
    clip &= ShadowBoundClip(((sceVu0FVECTOR *)&SCRATCHPAD[0x7a0])[0], tmpv[0]);
    clip &= ShadowBoundClip(((sceVu0FVECTOR *)&SCRATCHPAD[0x7b0])[0], tmpv[1]);
    clip &= ShadowBoundClip(((sceVu0FVECTOR *)&SCRATCHPAD[0x7c0])[0], tmpv[2]);
    clip &= ShadowBoundClip(((sceVu0FVECTOR *)&SCRATCHPAD[0x7d0])[0], tmpv[3]);
    clip &= ShadowBoundClip(((sceVu0FVECTOR *)&SCRATCHPAD[0x7e0])[0], tmpv[4]);
    clip &= ShadowBoundClip(((sceVu0FVECTOR *)&SCRATCHPAD[0x7f0])[0], tmpv[5]);
    clip &= ShadowBoundClip(((sceVu0FVECTOR *)&SCRATCHPAD[0x800])[0], tmpv[6]);
    clip &= ShadowBoundClip(((sceVu0FVECTOR *)&SCRATCHPAD[0x810])[0], tmpv[7]);

    if (clip != 0)
    {
        return 0;
    }

    _MulMatrix(tmpmat, IPMatrix, lwmtx);
    _SetMulMatrix(clipmtx, tmpmat);

    clip0 = 0x2f;
    clip1 = 0;

    for (i = 0; i < 8; i++)
    {
        clip = ShadowBoundClip(((sceVu0FVECTOR *)&SCRATCHPAD[0x7a0] + i)[0], tmpv[i]) & 0xf;

        if (clip == 0)
        {
            return 1;
        }

        clip0 = clip0 & clip;
        clip1 = clip1 | clip;
    }

    if (clip0 != 0)
    {
        return 0;
    }

    if ((clip1 & 0xf) != 0xf) {

        if (
            AppendShadowClipCheck(((sceVu0FVECTOR *)&SCRATCHPAD[0x7a0]), &boundline[0]) == 0 ||
            AppendShadowClipCheck(((sceVu0FVECTOR *)&SCRATCHPAD[0x7a0]), &boundline[4]) == 0
        )
        {

            return 0;
        }
    }

    return 1;
}

int CheckBoundingBoxShadow(u_int *prim)
{
    if (CheckBoundingBoxShadowTrace(lcp[prim[2]].lwmtx, (sceVu0FVECTOR *)&prim[4], ndirection) != 0)
    {
        edge_check = lcp[prim[2]].edge_check;

        return 1;
    }

    return 0;
}

void AssignShadowPrim(u_int *prim)
{
    int i;
    int cn;
    sceVu0FVECTOR tmpvec;

    if (prim == NULL)
    {
        return;
    }

    while (prim[0] != NULL)
    {
        switch (prim[1])
        {
        case 0:
            vuvnprim = prim;
        break;
        case 1:
            ShadowMeshDataVU(prim);
        break;
        case 3:
            // do nothing
        break;
        case 4:
            if (CheckBoundingBoxShadow(prim) == 0)
            {
                return;
            }

            cn = prim[2];

            if (ccahe.cache_on == 1 && ccahe.edge_check == edge_check)
            {
                for (i = 0; i < 4; i++)
                {
                    if (
                        lcp[cn].lwmtx[i][0] != lcp[ccahe.cn0].lwmtx[i][0] ||
                        lcp[cn].lwmtx[i][1] != lcp[ccahe.cn0].lwmtx[i][1] ||
                        lcp[cn].lwmtx[i][2] != lcp[ccahe.cn0].lwmtx[i][2] ||
                        lcp[cn].lwmtx[i][3] != lcp[ccahe.cn0].lwmtx[i][3]
                    )
                    {
                        ccahe.cache_on = -1;

                        break;
                    }
                }
            }

            if (ccahe.cache_on != 1)
            {
                ccahe.cache_on = 1;
                ccahe.edge_check = edge_check;
                ccahe.cn0 = cn;

                Vu0CopyMatrix((sceVu0FVECTOR *)&SCRATCHPAD[0x430], lcp[cn].lwmtx);

                GetInverseMatrix(*LIPMatrix, (sceVu0FVECTOR *)&SCRATCHPAD[0x430]);

                _ApplyMatrixXYZ(tmpvec, *LIPMatrix, ndirection);

                _MulMatrix(*LIPMatrix, IPMatrix, (sceVu0FVECTOR *)&SCRATCHPAD[0x430]);
                _MulMatrix(*LWSMtx, SgWSMtx, (sceVu0FVECTOR *)&SCRATCHPAD[0x430]);
                _MulMatrix(*LCMtx, SgCMtx, (sceVu0FVECTOR *)&SCRATCHPAD[0x430]);

                (*LIPMatrix)[0][3] = -tmpvec[0];
                (*LIPMatrix)[1][3] = -tmpvec[1];
                (*LIPMatrix)[2][3] = -tmpvec[2];
                (*LIPMatrix)[3][3] = 0.0f;

                (*LWSMtx)[3][2] += 10000.0f;

                SetVU1HeaderShadow();
            }
        break;
        case 0xFF:
            // dummy case
        break;
        }
        prim = (u_int *)*prim;
    }
}

void AssignShadowPreProcess(u_int *prim)
{
    pGroupPacket = NULL;

    if (prim == NULL)
    {
        return;
    }

    pGroupPacket = NULL;

    while(prim[0] != NULL)
    {
        if (prim[1] == 14)
        {
            pGroupPacket = prim;
        }

        prim = (u_int *)prim[0];
    }
}

void AssignShadow(void *sgd_top, int except_num)
{
    u_int *pk;
    u_int i;
    u_int groups;
    ModelGroup *mgp;
    HeaderSection *hs;

    hs = (HeaderSection *)sgd_top;

    ccahe.cache_on = -1;
    lcp = hs->coordp;
    blocksm = hs->blocks;

    pk = (u_int *)&hs->primitives;

    if (except_num == -1)
    {
        if (shadow_apgnum >= 0)
        {
            AssignShadowPreProcess((u_int *)pk[0]);

            if (pGroupPacket != NULL)
            {
                groups = pGroupPacket[2];
                mgp = (ModelGroup *)&pGroupPacket[4];

                if (groups < shadow_apgnum)
                {
                    shadow_apgnum = groups;
                }

                for (i = 0; i < shadow_apgnum; i++)
                {
                    mgp = (ModelGroup *)((u_int)mgp + (mgp->Num + 2) * sizeof(short));
                }

                for (i = 0; i < mgp->Num != 0; i++)
                {
                    groups = mgp->Lists[i];

                    if (lcp[groups].camin != 0)
                    {
                        AssignShadowPrim((u_int *)pk[groups]);
                    }
                }
            }
        }
        else
        {
            for (i = 1; i < blocksm - 1; i++)
            {
                if (lcp[i].camin != 0)
                {
                    AssignShadowPrim((u_int *)pk[i]);
                }
            }
        }
    }
    else
    {
        for (i = 1; i < blocksm - 1; i++)
        {
            if (lcp[i].camin != 0 && i != except_num)
            {
                AssignShadowPrim((u_int *)pk[i]);
            }
        }
    }
}

void SetUpShadow(ShadowHandle *shandle)
{
    u_int *datap;
    dword *base;
    static sceDmaTag tag[2][3];
    static sceDmaTag *tp;

    *(u_int *)&SCRATCHPAD[0] = 0;
    *(u_int *)&SCRATCHPAD[4] = 0;
    *(u_int *)&SCRATCHPAD[8] = 0x01000404;
    *(u_int *)&SCRATCHPAD[12] = 0x6c190000;

    datap = (u_int *)&SCRATCHPAD[16];

    *(float *)&datap[12] = 0.0f;
    *(float *)&datap[13] = 0.0f;
    *(float *)&datap[14] = 625.0f;
    *(float *)&datap[15] = 0.0f;

    *(u_long *)&datap[16] = SCE_GIF_SET_TAG(0, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 0, 1, 1, 1, 0, 0, 0, 0), SCE_GIF_PACKED, 3);
    datap[18] = 0 \
        | SCE_GS_ST    << (0 * 4)
        | SCE_GS_RGBAQ << (1 * 4)
        | SCE_GS_XYZF2 << (2 * 4);
    datap[19] = 0;

    *(u_long *)&datap[20] = SCE_GIF_SET_TAG(0, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRIFAN, 0, 1, 1, 1, 0, 0, 0, 0), SCE_GIF_PACKED, 3);
    datap[22] = 0 \
        | SCE_GS_ST    << (0 * 4)
        | SCE_GS_RGBAQ << (1 * 4)
        | SCE_GS_XYZF2 << (2 * 4);
    datap[23] = 0;

    Vu0CopyVector(*(sceVu0FVECTOR *)&SCRATCHPAD[16], vf12reg[0]);
    Vu0CopyVector(*(sceVu0FVECTOR *)&SCRATCHPAD[32], vf12reg[1]);

    datap[8] = 0x60;
    datap[9] = 0x230;

    Vu0CopyVector(*(sceVu0FVECTOR *)&SCRATCHPAD[0x190], fog_value);

    LoadSgProg(3);

    datap = (u_int *)getObjWrk();

    datap[0] = 0;
    datap[1] = 0;
    datap[2] = 0;
    datap[3] = DMAcall | 4;

    base = (dword *)&datap[4];

    base[0][0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 3);
    base[0][1] = 0 \
        | SCE_GIF_PACKED_AD << (0 * 4)
        | SCE_GIF_PACKED_AD << (1 * 4)
        | SCE_GIF_PACKED_AD << (2 * 4);

    base[1][0] = shandle->shadow_tex0;
    base[1][1] = SCE_GS_TEX0_1;

    base[2][0] = SCE_GS_SET_CLAMP_1(SCE_GS_CLAMP, SCE_GS_CLAMP, 0, 0, 0, 0);
    base[2][1] = SCE_GS_CLAMP_1;

    base[3][0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GEQUAL, 1, SCE_GS_AFAIL_KEEP, 0, 1, 1, SCE_GS_DEPTH_GEQUAL);
    base[3][1] = SCE_GS_TEST_1;

    AppendDmaBuffer(5);

    datap = (u_int *)getObjWrk();

    ((u_int *)datap)[0] = DMAref;
    ((float *)datap)[1] = 0.0f;
    ((float *)datap)[2] = 0.0f;
    ((float *)datap)[3] = 1.0f;
    ((float *)datap)[4] = 1.0f;
    ((float *)datap)[5] = 0.0f;
    ((float *)datap)[6] = 0.0f;
    ((float *)datap)[7] = 0.0f;

    AppendDmaBuffer(2);
    FlushModel(0);

    Vu0CopyVector(*(sceVu0FVECTOR *)shadow_col, *(sceVu0FVECTOR *)&shandle->color);
}

void ClearShadowFrame()
{
    dword *base;

    base = (dword *)getObjWrk();

    base[1][0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0), SCE_GIF_PACKED, 3);
    base[1][1] = 0 \
        | SCE_GS_RGBAQ << (0 * 4)
        | SCE_GS_XYZF2 << (1 * 4)
        | SCE_GS_XYZF2 << (2 * 4);

    base += 2;

    ((u_int *)&base[0])[0] = 0; // R
    ((u_int *)&base[0])[1] = 0; // G
    ((u_int *)&base[0])[2] = 0; // B
    ((u_int *)&base[0])[3] = 0; // A

    ((u_int *)&base[1])[0] = SCREENX_TO_GSX_UL(0, shadowtex.width);  // X
    ((u_int *)&base[1])[1] = SCREENY_TO_GSY_UL(0, shadowtex.height); // Y
    ((u_int *)&base[1])[2] = 0x3a980;                                // Z
    ((u_int *)&base[1])[3] = 0;                                      // F

    ((u_int *)&base[2])[0] = SCREENX_TO_GSX_DR(0, shadowtex.width);  // X
    ((u_int *)&base[2])[1] = SCREENY_TO_GSY_DR(0, shadowtex.height); // Y
    ((u_int *)&base[2])[2] = 0x3a980;                                // Z
    ((u_int *)&base[2])[3] = 0;                                      // F

    AppendDmaBufferFromEndAddress((qword *)(base + 3));
}

void SetShadowEnvironment()
{
    qword *base;

    base = (qword *)getObjWrk() + 1;

    base[0][0] = (int)(SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 9) >>  0);
    base[0][1] = (int)(SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 9) >> 32);
    base[0][2] = 0 \
        | SCE_GIF_PACKED_AD << (0 * 4)
        | SCE_GIF_PACKED_AD << (1 * 4)
        | SCE_GIF_PACKED_AD << (2 * 4)
        | SCE_GIF_PACKED_AD << (3 * 4)
        | SCE_GIF_PACKED_AD << (4 * 4)
        | SCE_GIF_PACKED_AD << (5 * 4)
        | SCE_GIF_PACKED_AD << (6 * 4)
        | SCE_GIF_PACKED_AD << (7 * 4);
    base[0][3] = 0 \
        | SCE_GIF_PACKED_AD << (0 * 4);

    base[1][2] = SCE_GS_TEXFLUSH;

    base[2][2] = SCE_GS_ALPHA_1;
    base[2][0] = (int)(SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x40) >>  0);
    base[2][1] = (int)(SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x40) >> 32);

    base[3][2] = SCE_GS_TEX1_1;
    base[3][0] = (int)(SCE_GS_SET_TEX1_1(0, 0, SCE_GS_LINEAR, SCE_GS_LINEAR, 0, 0, 0) >>  0);

    base[4][2] = SCE_GS_CLAMP_1;
    base[4][0] = (int)(SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0) >>  0);
    base[4][1] = (int)(SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0) >> 32);

    base[5][2] = SCE_GS_TEST_1;
    base[5][0] = (int)(SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 128, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS) >> 0);

    base[6][2] = SCE_GS_SCISSOR_1;
    base[6][0] = (shadowtex.width - 1) << 16; // SCE_GS_SET_SCISSOR(0, shadowtex.width - 1, 0, shadowtex.height - 1)
    base[6][1] = (shadowtex.height - 1) << 16; // SCE_GS_SET_SCISSOR(0, shadowtex.width - 1, 0, shadowtex.height - 1)

    base[7][2] = SCE_GS_ZBUF_1;
    base[7][0] = (int)(SCE_GS_SET_ZBUF_1(0, SCE_GS_PSMCT32, 1) >>  0);
    base[7][1] = (int)(SCE_GS_SET_ZBUF_1(0, SCE_GS_PSMCT32, 1) >> 32);

    base[8][2] = SCE_GS_FRAME_1;
    base[8][0] = (shadowtex.addr >> 11 | (shadowtex.width >> 6) << 16 | SCE_GS_PSMCT16 << 24); // SCE_GS_SET_FRAME(shadowtex.addr >> 11, shadowtex.width >> 6, SCE_GS_PSMCT16, 0)
    base[8][1] = 0; // SCE_GS_SET_FRAME(shadowtex.addr >> 11, shadowtex.width >> 6, SCE_GS_PSMCT16, 0)

    base[9][2] = SCE_GS_XYOFFSET_1;
    base[9][0] = SCREENX_TO_GSX_UL(0, shadowtex.width);
    base[9][1] = SCREENY_TO_GSY_UL(0, shadowtex.height);

    AppendDmaBufferFromEndAddress((qword *)(base + 10));

    ClearShadowFrame();

    base = (qword *)getObjWrk() + 1;
    base[0][0] = (int)(SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 2) >>  0);
    base[0][1] = (int)(SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 2) >> 32);
    base[0][2] = 0 \
        | SCE_GIF_PACKED_AD << (0 * 4)
        | SCE_GIF_PACKED_AD << (1 * 4);

    base[1][2] = SCE_GS_TEST_1;
    base[1][0] = (int)(SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GEQUAL, 1, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS) >>  0);

    base[2][2] = SCE_GS_SCISSOR_1;
    base[2][0] = 1 | (shadowtex.width - 2) << 16;  // SCE_GS_SET_SCISSOR(1, shadowtex.width - 2, 1, shadowtex.height - 2)
    base[2][1] = 1 | (shadowtex.height - 2) << 16; // SCE_GS_SET_SCISSOR(1, shadowtex.width - 2, 1, shadowtex.height - 2)

    AppendDmaBufferFromEndAddress((qword *)(base + 3));
}

void GetRotMatrixYZPlain(sceVu0FMATRIX rmat, sceVu0FVECTOR vec)
{
    sceVu0FVECTOR cvec;
    float xzr;

    Vu0CopyVector(cvec, vec);

    xzr = 0.0f;

    sceVu0UnitMatrix(rmat);

    if (cvec[0] != 0.0f || cvec[2] != 0.0f)
    {
        xzr = SgAtan2f(cvec[0], cvec[2]);
        sceVu0RotMatrixY(rmat, rmat, -xzr);
    }
}

void GetRotMatrixZAxis(sceVu0FMATRIX rmat, sceVu0FVECTOR vec)
{
    sceVu0FVECTOR cvec;
    float yzr;

    Vu0CopyVector(cvec, vec);

    yzr = 0.0f;

    sceVu0UnitMatrix(rmat);

    if (cvec[0] != 0.0f || cvec[2] != 0.0f)
    {
        yzr = SgAtan2f(cvec[0], cvec[2]);
        sceVu0RotMatrixY(rmat, rmat, -yzr);
        sceVu0ApplyMatrix(cvec, rmat, cvec);
    }

    if (cvec[1] != 0.0f || cvec[2] != 0.0f)
    {
        yzr = SgAtan2f(cvec[1], cvec[2]);
        sceVu0RotMatrixX(rmat, rmat, yzr);
    }
}

void CalcShadowMatrix(ShadowHandle *shandle, sceVu0FVECTOR center, float ax, float ay)
{
    sceVu0FMATRIX touei;
    sceVu0FMATRIX tmpmat;
    sceVu0FMATRIX quat;
    sceVu0FVECTOR culplain;
    float culval[4];
    float mincal;
    int i;

    sceVu0UnitMatrix(touei);

    touei[0][0] = (shadowtex.scale / (float)shadowtex.width) * ax;
    touei[1][1] = (-shadowtex.scale / (float)shadowtex.height) * ay;
    touei[2][2] = -1.0f;

    sceVu0UnitMatrix(tmpmat);

    tmpmat[3][0] = 0.5f;
    tmpmat[3][1] = 0.5f;
    tmpmat[3][2] = 0.0f;

    sceVu0MulMatrix(touei, tmpmat, touei);

    GetRotMatrixZAxis(quat, ndirection);

    sceVu0MulMatrix(touei, touei, quat);

    sceVu0UnitMatrix(tmpmat);

    tmpmat[3][0] = -center[0];
    tmpmat[3][1] = -center[1];
    tmpmat[3][2] = -center[2];

    sceVu0MulMatrix(touei, touei, tmpmat);

    culplain[0] = touei[0][2];
    culplain[1] = touei[1][2];
    culplain[2] = touei[2][2];
    culplain[3] = touei[3][2];

    culval[0] = sceVu0InnerProduct(culplain, wbbox[0]);
    culval[1] = sceVu0InnerProduct(culplain, wbbox[1]);
    culval[2] = sceVu0InnerProduct(culplain, wbbox[4]);
    culval[3] = sceVu0InnerProduct(culplain, wbbox[5]);

    mincal = culval[0];

    if (culval[1] < mincal)
    {
        mincal = culval[1];
    }

    if (culval[2] < mincal)
    {
        mincal = culval[2];
    }

    if (culval[3] < mincal)
    {
        mincal = culval[3];
    }

    touei[3][2] = -mincal;

    sceVu0CopyMatrix(IPMatrix, touei);

    SetUpShadow(shandle);

    write_counter = 0;
    write_flg = 0;

    for (i = 0; i < shandle->search_num; i++)
    {
        if (shandle->search_model[i] != shandle->source_model)
        {
            AssignShadow(shandle->search_model[i], -1);
        }
    }
}

static void _ftoi0(int *out, float *in)
{
    asm volatile ("\n\
        lqc2    $vf12,0(%0) \n\
        vftoi0.xyzw $vf12xyzw,$vf12xyzw \n\
        sqc2    $vf12,0(%1) \n\
    ": :"r"(in),"r"(out));
}

void CalcShadowHeight(sceVu0FVECTOR *bbox)
{
    int tmp;
    sceVu0FVECTOR tmpvec;
    sceVu0IVECTOR itmp;
    float scale;

    sceVu0SubVector(tmpvec, bbox[0], bbox[7]);
    sceVu0MulVector(tmpvec, tmpvec, tmpvec);

    scale = SgSqrtf(tmpvec[0] + tmpvec[1] + tmpvec[2]);
    tmpvec[0] = scale / shadowtex.fund_scale + 0.5f;

    _ftoi0(itmp, tmpvec);

    tmp = itmp[0];

    if (tmp < 1)
    {
        tmp = 1;
    }
    else if (tmp == 3)
    {
        tmp = 4;
    }
    else if (tmp >= 5 && tmp <= 7)
    {
        tmp = 8;
    }

    shadowtex.width = tmp * 64;
    shadowtex.height = tmp * 64;

    if (shadowtex.width > shadowtex.max_width)
    {
        shadowtex.width = shadowtex.max_width;
    }

    if (shadowtex.height > shadowtex.max_height)
    {
        shadowtex.height = shadowtex.max_height;
    }

    tmp = 2;
    shadowtex.width_p = 1;

    while (tmp < shadowtex.width)
    {
        shadowtex.width_p++;
        tmp *= 2;
    }

    tmp = 2;
    shadowtex.height_p = 1;

    while (tmp < shadowtex.height)
    {
        shadowtex.height_p++;
        tmp *= 2;
    }
}

void SetShadowCamera(float *center, sceVu0FVECTOR *bbox, SgCOORDUNIT *cp)
{
    int i;
    float xmax;
    float ymax;
    sceVu0FVECTOR tmpvec;
    sceVu0FVECTOR *cbbox;
    sceVu0FMATRIX tmpmat;
    sceVu0FMATRIX quat;

    cbbox = (sceVu0FVECTOR *)&SCRATCHPAD[0x6a0];

    Vu0LoadMatrix(cp->lwmtx);

    Vu0ApplyVectorInline(wbbox[0], bbox[0]);
    Vu0ApplyVectorInline(wbbox[1], bbox[1]);
    Vu0ApplyVectorInline(wbbox[2], bbox[2]);
    Vu0ApplyVectorInline(wbbox[3], bbox[3]);
    Vu0ApplyVectorInline(wbbox[4], bbox[4]);
    Vu0ApplyVectorInline(wbbox[5], bbox[5]);
    Vu0ApplyVectorInline(wbbox[6], bbox[6]);
    Vu0ApplyVectorInline(wbbox[7], bbox[7]);

    sceVu0AddVector(center, wbbox[0], wbbox[7]);

    sceVu0ScaleVector(center, center, 0.5f);
    center[3] = 1.0f;

    Vu0CopyVector(scamera.i, center);

    sceVu0ScaleVector(tmpvec, ndirection, 25000.0f);

    sceVu0AddVector(scamera.p, scamera.i, tmpvec);

    scamera.ax = scamera.ay = 1.0f;

    SgSetRefCameraOrtho(&scamera);

    xmax = ymax = 0.0f;

    _SetMulMatrix(scamera.ws, cp->lwmtx);

    for (i = 0; i < 8; i++)
    {
        Vu0ApplyVectorInline(tmpvec, bbox[i]);

        tmpvec[0] = __builtin_fabsf(tmpvec[0] - scamera.cx);
        tmpvec[1] = __builtin_fabsf(tmpvec[1] - scamera.cy);

        if (xmax < tmpvec[0])
        {
            xmax = tmpvec[0];
        }

        if (ymax < tmpvec[1])
        {
            ymax = tmpvec[1];
        }
    }

    CalcShadowHeight(wbbox);

    scamera.ax = (shadowtex.width * 0.5f) / xmax;
    scamera.ay = (shadowtex.height * 0.5f) / ymax;

    SgSetRefCameraOrtho(&scamera);

    sceVu0UnitMatrix(tmpmat);

    if (0.0f < ndirection[0])
    {
        tmpmat[0][0] = -1.0f;
    }

    if (0.0f < ndirection[1])
    {
        tmpmat[1][1] = -1.0f;
    }

    if (0.0f < ndirection[2])
    {
        tmpmat[2][2] = -1.0f;
    }

    Vu0LoadMatrix(tmpmat);

    Vu0ApplyVectorInline(cbbox[0], wbbox[0]);
    Vu0ApplyVectorInline(cbbox[1], wbbox[1]);
    Vu0ApplyVectorInline(cbbox[2], wbbox[2]);
    Vu0ApplyVectorInline(cbbox[3], wbbox[3]);
    Vu0ApplyVectorInline(cbbox[4], wbbox[4]);
    Vu0ApplyVectorInline(cbbox[5], wbbox[5]);
    Vu0ApplyVectorInline(cbbox[6], wbbox[6]);
    Vu0ApplyVectorInline(cbbox[7], wbbox[7]);

    Vu0CopyVector(tmpvec, cbbox[0]);

    for (i = 1; i < 8; i++)
    {
        if (tmpvec[0] > cbbox[i][0])
        {
            tmpvec[0] = cbbox[i][0];
        }

        if (tmpvec[1] > cbbox[i][1])
        {
            tmpvec[1] = cbbox[i][1];
        }

        if (tmpvec[2] > cbbox[i][2])
        {
            tmpvec[2] = cbbox[i][2];
        }
    }

    tmpvec[3] = 0.0f;

    Vu0MulVectorX(tmpmat[3], tmpvec, -1.0f);

    Vu0CopyMatrix(CullingMatrix, tmpmat);
}

void DrawShadow(ShadowHandle *shandle, EnvFuncCallback env_func)
{
    SgCOORDUNIT oldcoord;
    sceVu0FMATRIX tmpmat;
    sceVu0FMATRIX quat;
    sceVu0FMATRIX touei;
    sceVu0FVECTOR tmpvec;
    sceVu0FVECTOR center;
    sceVu0FVECTOR vf2reg;
    HeaderSection *hs;
    static int count = 0;

    _NormalizeVector(ndirection, shandle->direction);

    hs = (HeaderSection *)shandle->shadow_model;

    SetShadowCamera(center, shandle->bbox, hs->coordp);

    shandle->shadow_tex0 = SCE_GS_SET_TEX0_1(shadowtex.addr >> 6, shadowtex.width >> 6, SCE_GS_PSMCT16, shadowtex.width_p, shadowtex.height_p, 1, SCE_GS_MODULATE, shadowtex.addr >> 6, SCE_GS_PSMCT32, 0, 0, 1);

    SetShadowEnvironment();

    DrawShadowModel(hs,shandle->smodel_num);

    SgSetWsMtx(shandle->camera->ws);
    SgSetClipMtx(shandle->camera->wc);
    SgSetClipVMtx(shandle->camera->wcv);

    env_func();

    CalcShadowMatrix(shandle,center, scamera.ax, scamera.ay);
}
