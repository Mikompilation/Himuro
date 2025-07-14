#include "common.h"
#include "typedefs.h"
#include "sgsgd.h"

#include "sce/libgraph.h"

#include "graphics/graph3d/sg_dat.h"
#include "graphics/graph3d/sgdma.h"
#include "graphics/graph3d/sglib.h"

static int post_process = 0;
static int PresetChk;

#define SVA_UNIQUE 0
#define SVA_COMMON 1
#define SVA_WEIGHTED 2

// Mesh Types
#define iMT_0 0x10
#define iMT_2 0x12
#define iMT_2F 0x32

// ProcUnitType
#define VUVN 0
#define MESH 1
#define MATERIAL 2
#define COORDINATE 3
#define BOUNDING_BOX 4
#define GS_IMAGE 5
#define TRI2 10
#define END 11
#define INVALID 12
#define MonotoneTRI2 13
#define StackTRI2 14

#define GET_MESH_TYPE(intpointer) (char)((char*)intpointer)[13]
#define GET_MESH_GLOOPS(intpointer) (int)((char*)intpointer)[14]

void MappingVUVNData(u_int *intpointer, HeaderSection *hs)
{
    int i;
    VUVN_PRIM *vh;
    PHEAD *ph;
    sceVu0FVECTOR *vp;
    sceVu0FVECTOR *np;

    ph = (PHEAD *)hs->phead;
    vh = (VUVN_PRIM *)&intpointer[2];

    intpointer += 12;

    switch (vh->vtype)
    {
    case SVA_UNIQUE:
        vp = ph->pUniqVertex;
        np = ph->pUniqNormal;

        for (i = 0; i < vh->vnum; i++)
        {
            intpointer[0] = intpointer[0] * 16 + (int)vp; intpointer++;
            intpointer[0] = intpointer[0] * 16 + (int)np; intpointer++;
        }
    break;
    case SVA_COMMON:
        // ...
    break;
    case SVA_WEIGHTED:
    default:
        if (ph->pWeightedList)
        {
            vp = vertex_buffer;
            np = normal_buffer;

            for (i = 0; i < vh->vnum; i++)
            {
                intpointer[0] = intpointer[0] * 16 + (int)vp; intpointer++;
                intpointer[0] = intpointer[0] * 16 + (int)np; intpointer++;
            }
        }
        else
        {
            vp = ph->pWeightedVertex;
            np = ph->pWeightedNormal;

            for (i = 0; i < vh->vnum; i++)
            {
                intpointer[0] = intpointer[0] * 32 + (int)vp; intpointer++;
                intpointer[0] = intpointer[0] * 32 + (int)np; intpointer++;
            }
        }
    break;
    }
}

void MappingVUVNDataPreset(u_int *intpointer, int mtype, int gloops, int hsize)
{
    return;
}

void MappingTYPE0(int gloops, u_int *prim)
{
    int j;
    int loops;

    loops = gloops;

    for (j = 0; j < loops; j++)
    {
        prim[1] |= 0x100000;
        prim = prim + ((prim[0] & 0x7fff) * 8 + 4);
    }
}
int MappingTYPE2(int gloops, u_int *prim)
{
    int j;
    int loops;
    int hsize;

    hsize = 0;

    if ((prim[2] & 0xf) == SCE_GIF_PACKED_AD)
    {
        hsize = ((sceGifTag *)prim)->NREG + 1;
    }

    prim = &prim[hsize*4];

    for(j = 0; j < gloops; j++)
    {
        loops = prim[0];
        prim[1] |= 0x100000;
        prim += (loops & 0x7fff) * 12 + 4;
    }

    return hsize;
}


void MappingMeshData(u_int *intpointer, u_int *vuvnprim, HeaderSection *hs)
{
    int mtype;
    int gloops;
    int hsize;
    static u_int *old_mesh_p;

    mtype = GET_MESH_TYPE(intpointer);
    gloops = GET_MESH_GLOOPS(intpointer);

    if (mtype & iMT_0)
    {
        PresetChk = 1;

        if ((mtype & 0x40) == 0)
        {
            MappingVUVNDataPreset(vuvnprim, mtype, gloops, hsize);
            return;
        }
    }
    else
    {
        if ((mtype & 0xc0) == 0)
        {
            MappingVUVNData(vuvnprim, hs);
            return;
        }
    }

    return;
}

void MappingCoordinateData(u_int *intpointer, HeaderSection *hs)
{
    return;
}

void MappingVertexList(VERTEXLIST *vli, PHEAD *ph)
{
    int i;
    int size;
    int vnnum;

    for (i = 0, size = 0; i < vli->list_num; i++)
    {
        vnnum = *(int *)&vli->lists[i].vIdx;
        vli->lists[i].vOff = size;
        size += vnnum;
    }

    if (vnbuf_size < size)
    {
        printf("VNBuffer Over size %d needs %d\n", vnbuf_size, size);
        ph->pUniqList = ph->pWeightedList = NULL;
    }
}

void SgMapRebuld(void *sgd_top)
{
    u_int *prim;
    u_int *nextprim;
    HeaderSection *hs;

    hs = (HeaderSection *)sgd_top;

    if (hs->primitives == 0)
    {
        return;
    }

    prim = (u_int *)((int)hs->primitives + (int)sgd_top);

    while (prim[0] != 0)
    {
        nextprim = (u_int *)(prim[0] + (int)prim);

        // check that always evaluates to false ???
        // most likely `prim == 0` that got optimized out
        if (0)
        {
            break;
        }

        switch (prim[1])
        {
        case TRI2:
            RebuildTRI2Files(prim);
        break;
        }

        prim = nextprim;
    }
}

void SgMapUnit(void *sgd_top)
{
    int i;
    int j;
    int size;
    u_int *intpointer;
    u_int *nextprim;
    u_int *pk;
    u_int *vuvnprim;
    HeaderSection *hs;
    SgMaterial *matp;
    u_int *phead;
    SgCOORDUNIT *cp;
    PHEAD *ph;
    VERTEXLIST *vli;

    hs = (HeaderSection *)sgd_top;

    if (hs->MAPFLAG != 0)
    {
        return;
    }

    hs->MAPFLAG = 1;

    PresetChk = 0;

    if ((u_int)hs->coordp - 1 < 0x2fffffff)
    {
        hs->coordp = (SgCOORDUNIT *)((u_int)hs->coordp + (int)sgd_top);
    }

    if ((u_int)hs->matp - 1 < 0x2fffffff)
    {
        hs->matp = (SgMaterial *)((u_int)hs->matp + (int)sgd_top);
    }

    hs->phead = (u_int *)((u_int)hs->phead + (int)sgd_top);

    pk = (int *)&hs->primitives;

    for (i = 0; i < hs->blocks; i++)
    {
        if (pk[i] != 0)
        {
            pk[i] = pk[i] + (int)sgd_top;
        }
    }

    cp = hs->coordp;

    if (cp != 0)
    {
        for (i = 0; i < hs->blocks - 1; i++)
        {
            if ((int)cp[i].parent < 0)
            {
                cp[i].parent = NULL;
            }
            else if ((int)cp[i].parent < (u_int)cp)
            {
                cp[i].parent = (int)cp[i].parent + cp;
            }
        }
    }

    intpointer = (u_int *)((int)hs->phead);
    intpointer++;

    for (i = 0; i < hs->phead[0]; i++)
    {
        size = *intpointer++;

        for (j = 0; j < size - 1; j++, intpointer++)
        {
            if (*intpointer != 0)
            {
                *intpointer = *intpointer + (int)sgd_top;
            }
        }
    }

    ph = (PHEAD *)hs->phead;

    if (ph->pUniqList != NULL && ph->UniqHeaderSize == 4)
    {
        if (vnbuf_size == 0)
        {
            ph->pUniqList = NULL;
            ph->pCommonList = NULL;
            ph->pWeightedList = NULL;
        }
        else
        {
            if (ph->pWeightedVertex == NULL && ph->pWeightedNormal == NULL)
            {
                ph->pWeightedList = NULL;
            }
        }

        ph->pUniqList = NULL;
        if (ph->pWeightedList != NULL)
        {
            vli = ((VERTEXLIST *)ph->pWeightedList);
            MappingVertexList((VERTEXLIST *)ph->pWeightedList, ph);

            vli = (VERTEXLIST *)(&vli->lists[vli->list_num]);
            MappingVertexList(vli, ph);
        }
    }

    for (i = 0; i < hs->blocks; i++)
    {
        if (i == hs->blocks - 1)
        {
            post_process = 1;
        }
        else
        {
            post_process = 0;
        }

        if (pk[i] == NULL)
        {
            continue;
        }

        intpointer = (u_int *)pk[i];

        while (*intpointer != NULL)
        {
            *intpointer = (int)*intpointer + (int)intpointer;

            if (0) // intpointer == NULL ?
            {
                break;
            }

            nextprim = intpointer++;

            switch(*intpointer)
            {
            case VUVN:
                vuvnprim = intpointer-1;
            break;
            case MESH:
                MappingMeshData(intpointer-1, vuvnprim, hs);
            break;
            case MATERIAL:
                intpointer++;
                *intpointer = (u_int)&hs->matp[*intpointer];
            break;
            case COORDINATE:
                MappingCoordinateData(intpointer-1, hs);
            break;
            case TRI2:
                RebuildTRI2Files(intpointer-1);
            break;
            }

            intpointer = (u_int *)*nextprim;
        }
    }

    if (hs->materials == 0)
    {
        size = 0;
        matp = hs->matp;
        phead = hs->phead;

        while ((u_int)matp < (u_int)phead)
        {
            size++;
            matp++;
        }

        hs->materials = size;
        hs->kind = (int)PresetChk;
    }
}

u_int *GetPrimitiveAddr(void *sgd_top, int num)
{
    int blocks;
    u_int *pk;
    HeaderSection *hs;

    hs = (HeaderSection *)sgd_top;

    blocks = hs->blocks;

    if (blocks <= num)
    {
        return (u_int*)0xFFFFFFFF;
    }

    pk = (u_int *)&hs->primitives;

    return (pk[num] != 0) ? (u_int *)pk[num] : &pk[num];
}

u_int *GetStartPacketAddr(void *sgd_top, int num)
{
    int blocks;
    u_int *pk;
    HeaderSection *hs;

    hs = (HeaderSection *)sgd_top;

    blocks = hs->blocks;

    if (blocks <= num)
    {
        return (u_int *)0xFFFFFFFF;
    }

    pk = (u_int *)&hs->primitives;

    return &pk[num];
}

u_int* GetEndPacketAddr(void *sgd_top, int num)
{
    u_int *pk;
    u_int *pkback;

    pk = GetPrimitiveAddr(sgd_top, num);

    if ((u_int)pk == 0xFFFFFFFF || pk == NULL)
    {
        return pk;
    }

    pkback = pk;

    while (*pk != 0)
    {
        pkback = pk;
        pk = (u_int*)*pkback;
    }

    return pkback;
}

void PostChainPacket(u_int *dest, u_int *source)
{
    *source = *dest;
    *dest = (u_int)source;
}

int GetModelKind(void *sgd_top)
{
    return ((HeaderSection*)sgd_top)->kind & 0x1;
}

void SgSetTEX1Prim(u_int *prim, sceGsTex1 *ptex1)
{
    u_int *mesh_prim;
    int i;
    int hsize;

    if (prim == NULL)
    {
        return;
    }

    while (prim[0] != NULL)
    {
        if (prim[1] == 1 && (prim[10] & 0xf) == 14)
        {
            hsize = *(u_long *)&prim[8] >> 60;
            mesh_prim = &prim[12];

            for (i = 0; i < hsize; i++)
            {
                if (mesh_prim[2] == SCE_GS_TEX1_1)
                {
                    sceGsTex1 *tex1 = (sceGsTex1 *)mesh_prim;

                    if (tex1->MXL != 0)
                    {
                        tex1->LCM = ptex1->LCM;

                        if (ptex1->MXL > 0)
                        {
                            tex1->MXL = ptex1->MXL;
                        }

                        tex1->MMAG = ptex1->MMAG;
                        tex1->MMIN = ptex1->MMIN;
                        tex1->MTBA = ptex1->MTBA;
                        tex1->L = ptex1->L;
                        tex1->K = ptex1->K;
                    }
                }

                mesh_prim += 4;
            }
        }

        prim = (u_int *)*prim;
    }
}

void SgSetTEX1(void *sgd_top, sceGsTex1 *ptex1)
{
    int i;
    HeaderSection *hs;
    u_int **pk;

    hs = (HeaderSection *)sgd_top;
    pk = (u_int **)&hs->primitives;


    for (i = 1; i < hs->blocks; i++)
    {
        SgSetTEX1Prim(pk[i], ptex1);
    }
}
