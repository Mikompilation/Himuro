#include "common.h"
#include "typedefs.h"
#include "sgsup.h"

#include "ee/kernel.h"
#include "ee/eeregs.h"
#include "ee/eestruct.h"

#include "sce/libdma.h"
#include "sce/libvu0.h"
#include "sce/libgraph.h"

#include "graphics/graph3d/sgsu.h"
#include "graphics/graph3d/sgcam.h"
#include "graphics/graph3d/sgdma.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/libsg.h"
#include "graphics/graph3d/sglight.h"

static int write_flg = 0;
static int write_counter = 0;
static int dbg_flg = 0;

extern void DIVP0_PROLOGUE_and_DIVP2_PROLOGUE() __attribute__((section(".vutext")));
extern void MULTIP_PROLOGUE() __attribute__((section(".vutext")));
extern void DP0_PROLOGUE() __attribute__((section(".vutext")));
extern void DP2_PROLOGUE() __attribute__((section(".vutext")));

#define GET_MESH_TYPE(intpointer) (char)((char*)intpointer)[13]

#define SCRATCHPAD ((u_char *)0x70000000)

void SgSuPDbgOn()
{
    dbg_flg = 1;
}

void SgSuPDbgOff()
{
    dbg_flg = 0;
}

void DispMicroMemory()
{
    u_int *read_p;

    read_p = (u_int *)VU1_MICRO_ADDR;

    while((u_int)read_p <= (u_int)(VU1_MICRO_ADDR + 1968*4*4))
    {
        printf("%x:%x %x %x %x\n", (u_int)read_p, read_p[0], read_p[1], read_p[2], read_p[3]);

        read_p += 4;
    }

    exit(0);
}

void DispVUMemory()
{
    u_int *read_p;

    FlushModel(1);
    sceDmaSync(dvif1, 0, 0);
    WaitVU1();
    sceGsSyncPath(0, 0);
    FlushCache(0);

    read_p = (u_int *)(VU1_MEM_ADDR);

    while((u_int)read_p <= (u_int)(VU1_MEM_ADDR + 1023*4*4))
    {
        if (
            ((u_int *)read_p)[3] == 0x3f800000 ||
            read_p == (u_int *)(VU1_MEM_ADDR + 75*4*4) ||
            read_p == (u_int *)(VU1_MEM_ADDR + 80*4*4) ||
            read_p == (u_int *)(VU1_MEM_ADDR + 85*4*4) ||
            ((u_int)read_p >= (VU1_MEM_ADDR + 8*4*4) && (u_int)read_p <= (VU1_MEM_ADDR + 960 + 8*4*4)))
        {
            printf("%x(%3d):%f %f %f %f\n",
                (u_int)read_p, (u_int)((u_int)read_p - VU1_MEM_ADDR) / (4*4),
                ((float *)read_p)[0],
                ((float *)read_p)[1],
                ((float *)read_p)[2],
                ((float *)read_p)[3]);
        }
        else
        {
            printf("%x(%3d):%8x %8x %8x %8x\n",
                (u_int)read_p, (u_int)((u_int)read_p - VU1_MEM_ADDR) / (4*4),
                read_p[0],
                read_p[1],
                read_p[2],
                read_p[3]);
        }

        read_p += 4;
    }

    printf("\n");

    exit(0);
}

void DrawBoundingBox(sceVu0FVECTOR *box)
{
    int i;
    int j;
    int qwc;
    u_int *datap;
    u_int *startp;
    static char boxmesh[24] = {
        1, 0, 3, 2, 7, 5, 3, 1,
        6, 7, 2, 3, 4, 6, 0, 2,
        5, 4, 1, 0, 4, 5, 6, 7,
    };

    LoadSgProg(1);

    datap = (u_int *)getObjWrk();

    datap[0] = (float)0x11000000;
    datap[1] = 0;
    datap[2] = 0;
    datap[3] = 0x6c010005;

    datap[4] = 0x8000;
    datap[5] = 0;
    datap[6] = 0;
    datap[7] = 0;

    AppendDmaBuffer(2);

    datap = (u_int *)getObjWrk();

    datap[0] = 0;
    datap[1] = 0;
    datap[2] = 0;
    datap[3] = 0x6c07002e;

    ((float *)datap)[4] = 1.0f;
    ((float *)datap)[5] = 1.0f;
    ((float *)datap)[6] = 128.0f;
    ((float *)datap)[7] = 128.0f;

    ((float *)datap)[8] = 0.0f;
    ((float *)datap)[9] = 0.0f;
    ((float *)datap)[10] = 0.0f;
    ((float *)datap)[11] = 12.0f;

    datap[12] = 0;
    datap[13] = 0;
    datap[14] = 0;
    datap[15] = 0;

    datap[16] = 0;
    datap[17] = 0;
    datap[18] = 0;
    datap[19] = 0;

    datap[20] = 0;
    datap[21] = 0;
    datap[22] = 0;
    datap[23] = 0;

    datap[24] = 0;
    datap[25] = 0;
    datap[26] = 0;
    datap[27] = 0;

    datap[28] = 0;
    datap[29] = 0;
    datap[30] = 0;
    datap[31] = 0;

    AppendDmaBuffer(8);

    startp = (u_int *)getObjWrk();

    datap = &startp[4];

    datap[0] = 16;
    datap[1] = 49;
    datap[2] = 49;
    datap[3] = 6;

    datap = &startp[8];

    for (i = 0; i < 24; i++)
    {
        Vu0CopyVector((float *)datap, (float *)&box[boxmesh[i]]);

        ((float *)datap)[4] = 0.0f;
        ((float *)datap)[5] = 1.0f;
        ((float *)datap)[6] = 0.0f;
        ((float *)datap)[7] = 1.0f;

        datap += 8;
    }

    for (i = 0; i < 6; i++)
    {
        int eop = i == 5;

        if (eop)
        {
            eop = 1;
        }

        ((u_long *)datap)[0] = SCE_GIF_SET_TAG(4, eop, SCE_GS_TRUE, 100, SCE_GIF_PACKED, 2);
        ((long *)datap)[1] = 0 \
            | SCE_GS_RGBAQ << (4 * 0)
            | SCE_GS_XYZF2 << (4 * 1);

        datap += 4;

        for (j = 0; j < 4; j++)
        {
            ((float *)datap)[0] = i * 22.0f;
            ((float *)datap)[1] = 0.0f;
            ((float *)datap)[2] = 0.0f;
            ((float *)datap)[3] = 0.0f;

            ((float *)datap)[4] = 0.0f;
            ((float *)datap)[5] = 0.0f;
            ((float *)datap)[6] = 0.0f;
            ((float *)datap)[7] = 0.0f;

            datap += 8;
        }
    }
    datap[0] = 0x11000000;
    datap[1] = 0x14000000 | ((u_int)DIVP0_PROLOGUE_and_DIVP2_PROLOGUE >> 3);
    datap[2] = 0x11000000;
    datap[3] = 0x17000000;

    datap += 4;

    qwc = (u_int)((u_int)datap - (u_int)startp) >> 4;

    startp[0] = 0;
    startp[1] = 0;
    startp[2] = 0;
    startp[3] = ((qwc - 2) * 0x10000 | 0x6c008000);

    AppendDmaBuffer(qwc);
    FlushModel(0);
}

void SetVUMeshDataP(u_int *prim)
{
    int mtype;
    u_int *datap; u_int addr;

    mtype = GET_MESH_TYPE(prim);

    switch (mtype) {
    case 16:
        LoadSgProg(1);

        AppendDmaTag((u_int)&prim[8], prim[2]);

        addr = (u_int)&vuvnprim[4];
        AppendDmaTag(addr, *(u_char *)&vuvnprim[3]);

        datap = (u_int *)getObjWrk();

        if (edge_check != 0)
        {
            datap[0] = 0x11000000;
            datap[1] = 0x14000000 | ((u_int)DIVP0_PROLOGUE_and_DIVP2_PROLOGUE >> 3);
            datap[2] = 0x11000000;
            datap[3] = 0x17000000;
        }
        else
        {
            datap[0] = 0x14000000 | ((u_int)DP0_PROLOGUE >> 3);
            datap[1] = 0x17000000;
            datap[2] = 0x11000000;
            datap[3] = 0x17000000;
        }

        AppendDmaBuffer(1);
    break;
    case 18:
    case 50:
        LoadSgProg(2);

        addr = (u_int)&vuvnprim[4];
        AppendDmaTag(addr, *(u_char *)&vuvnprim[3]);
        AppendDmaTag((u_int)&prim[8], prim[2]);

        datap = (u_int *)getObjWrk();

        if (edge_check != 0)
        {
            datap[0] = 0x11000000;
            datap[1] = 0x14000000 | ((u_int)DIVP0_PROLOGUE_and_DIVP2_PROLOGUE >> 3);
            datap[2] = 0x11000000;
            datap[3] = 0x17000000;
        }
        else
        {
            datap[0] = 0x14000000 | ((u_int)DP2_PROLOGUE >> 3);
            datap[1] = 0x17000000;
            datap[2] = 0x11000000;
            datap[3] = 0x17000000;
        }

        AppendDmaBuffer(1);
        FlushModel(0);
    break;
    case 82:
    case 114:
        if (edge_check == 0)
        {
            AppendDmaTag((u_int)&prim[4], prim[2]);

            datap = (u_int *)getObjWrk();

            datap[0] = 0x14000000 | ((u_int)MULTIP_PROLOGUE >> 3);
            datap[1] = 0x17000000;
            datap[2] = 0x11000000;
            datap[3] = 0x17000000;

            AppendDmaBuffer(1);
        }
    break;
    default:
        return;
    break;
    }

    FlushModel(0);
}

int BoundingBoxCalcP(u_int *prim)
{
    if (CheckBoundingBox(prim) == 0)
    {
        return 0;
    }

    if (edge_check != 0)
    {
        Vu0CopyMatrix(*(sceVu0FMATRIX *)&SCRATCHPAD[0x430], lcp[prim[2]].lwmtx);

        _MulMatrix(*(sceVu0FMATRIX *)&SCRATCHPAD[0x90], SgWSMtx, *(sceVu0FMATRIX *)&SCRATCHPAD[0x430]);
        _MulMatrix(*(sceVu0FMATRIX *)&SCRATCHPAD[0xD0], SgCMVtx, *(sceVu0FMATRIX *)&SCRATCHPAD[0x430]);
    }
    else
    {
        Vu0CopyMatrix(*(sceVu0FMATRIX *)&SCRATCHPAD[0x430], lcp[prim[2]].lwmtx);

        _MulMatrix(*(sceVu0FMATRIX *)&SCRATCHPAD[0x90], SgWSMtx, *(sceVu0FMATRIX *)&SCRATCHPAD[0x430]);
        _MulMatrix(*(sceVu0FMATRIX *)&SCRATCHPAD[0xD0], SgCMtx, *(sceVu0FMATRIX *)&SCRATCHPAD[0x430]);
    }

    lcp[prim[2]].edge_check = edge_check;

    SelectLight(prim);

    if (CheckCoordCache(prim[2]) == 0)
    {
        SetMaterialPointerCoordVU();
        SetLightData(&lcp[prim[2]], NULL);
        SetVU1Header();
    }

    return 1;
}

void SgSortUnitPrimP(u_int *prim)
{
    if (prim != NULL)
    {
        while (*prim != NULL)
        {
            switch(prim[1])
            {
            case 0:
                vuvnprim = prim;
            break;
            case 1:
                SetVUMeshDataP(prim);
            break;
            case 2:
                SetMaterialDataVU(prim);
            break;
            case 3:
                // do nothing ...
            break;
            case 4:
                if (BoundingBoxCalcP(prim) == 0)
                {
                    return;
                }

                if (save_tri2_pointer != NULL)
                {
                    LoadTRI2Files(save_tri2_pointer);
                    save_tri2_pointer = NULL;
                }

                if (save_bw_pointer != NULL)
                {
                    LoadTRI2Files(save_bw_pointer);
                    save_bw_pointer = NULL;
                }

                write_coord++;
            break;
            case 5:
                GsImageProcess(prim);
            break;
            case 65:
                return;
            break;
            }

            prim = (u_int*)*prim;
        }
    }
}

void SgSortPreProcessP(u_int *prim)
{
    static u_int *old_pointer = NULL;
    static sceDmaTag tag[2][3];
    static sceDmaTag *tp;
    static qword base[3];
    static qword base3[6];
    static qword *base2;

    pGroupPacket = NULL;

    if (prim == NULL)
    {
        return;
    }

    while (*prim != NULL)
    {
        switch(prim[1])
        {
        case 5:
            GsImageProcess(prim);
        break;
        case 10:
            if (save_tri2_pointer == (u_int *)0xFFFFFFFF)
            {
                LoadTRI2Files(prim);
                save_tri2_pointer = NULL;
            }
            else
            {
                save_tri2_pointer = prim;
            }
        break;
        case 13:
            if (loadbw_flg != 0)
            {
                if (save_bw_pointer == (u_int *)0xFFFFFFFF)
                {
                    LoadTRI2Files(prim);
                    save_bw_pointer = NULL;
                }
                else
                {
                    save_bw_pointer = prim;
                }
            }
        break;
        case 12:
            if (save_tri2_pointer != NULL)
            {
                LoadTRI2Files(save_tri2_pointer);
            }
            LoadTextureAnimation(prim);
        break;
        case 14:
            pGroupPacket = prim;
        }

        prim = (u_int*)*prim;
    }
}

u_int *save_tri2_pointer = NULL;
u_int *save_bw_pointer = NULL;

void SgSortUnitP(void *sgd_top, int pnum)
{
    int i;
    u_int *pk;
    HeaderSection *hs;

    hs = (HeaderSection *)sgd_top;

    lcp = hs->coordp;

    if (((u_int)lcp & 0xf) != 0)
    {
        printf("SgSortUnitP Data broken. %x\n", (u_int)hs);
        return;
    }

    pk = (u_int *)&hs->primitives;
    blocksm = hs->blocks;

    sgd_top_addr = sgd_top;

    write_flg = 0;
    write_counter = 0;
    write_coord = 0;

    save_tri2_pointer = NULL;
    save_bw_pointer = NULL;

    ClearMaterialCache(hs);

    SetUpSortUnit();

    if (pnum < 0)
    {
        SgSortPreProcessP((u_int*)pk[0]);

        for (i = 1; i < blocksm; i++)
        {
            SgSortUnitPrimP((u_int*)pk[i]);
        }
    }
    else if (pnum == 0)
    {
        save_tri2_pointer = (u_int *)0xFFFFFFFF;
        save_bw_pointer = (u_int *)0xFFFFFFFF;
        SgSortPreProcessP((u_int *)pk[pnum]);
    }
    else
    {
        SgSortUnitPrimP((u_int *)pk[pnum]);
    }
}

u_int* GetModelGroupPacket(void *sgd_top)
{
    u_int *prim;
    u_int *pk;
    HeaderSection *hs;

    hs = (HeaderSection *)sgd_top;

    pk = (u_int *)hs->primitives;

    if (pk == NULL)
    {
        return NULL;
    }

    while(*pk)
    {
        if (pk[1] == 14)
        {
            return pk;
        }

        pk = (u_int *)*pk;
    }

    return NULL;
}

void SgSortGroupP(void *sgd_top, int gnum)
{
    int i;
    int groups;
    u_int *pk;
    ModelGroup *mgp;
    HeaderSection *hs;

    hs = (HeaderSection *)sgd_top;

    lcp = hs->coordp;

    if (((u_int)lcp & 0xf) != 0)
    {
        printf("SgSortGroupP Data broken. %x\n", (u_int)hs);
        return;
    }

    blocksm = hs->blocks;
    pk = (u_int *)&hs->primitives;

    sgd_top_addr = sgd_top;

    write_flg = 0;
    write_counter = 0;
    write_coord = 0;

    save_tri2_pointer = NULL;
    save_bw_pointer = NULL;

    ClearMaterialCache(hs);

    SetUpSortUnit();

    SgSortPreProcessP((u_int *)pk[0]);

    if (pGroupPacket == NULL)
    {
        for (i = 1; i < blocksm; i++)
        {
            SgSortUnitPrimP((u_int*)pk[i]);
        }
    }
    else
    {
        groups = pGroupPacket[2];
        mgp = (ModelGroup *)&pGroupPacket[4];

        gnum = groups < gnum ? groups : gnum;

        for (i = 0; i < gnum; i++)
        {
            mgp = (ModelGroup *)((u_int)mgp + (mgp->Num + 2) * sizeof(short)); // mgp = (ModelGroup *)&mgp->Lists[mgp->Num]; :(
        }

        for (i = 0; i < mgp->Num; i++)
        {
            SgSortUnitPrimP((u_int *)pk[mgp->Lists[i]]);
        }
    }
}
