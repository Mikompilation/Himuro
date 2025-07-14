#include "common.h"
#include "typedefs.h"
#include "sgdma.h"

#include "ee/kernel.h"
#include "sce/libgraph.h"
#include "sce/libdma.h"
#include "sce/devvif1.h"

#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/libsg.h"
#include "graphics/graph3d/sgsu.h"

#define REG_VIF1_STAT        ((volatile u_int *)(0x10003c00))
#define REG_DMAC_ENABLEW     ((volatile u_int *)(0x1000f590))
#define REG_DMAC_1_VIF1_CHCR ((volatile u_int *)(0x10009000))

static int DummyFlushData[] = {0, 0, 0, 0};

static u_int send_dma_flg = 1;

static u_int *previous_tri2_prim;
static u_int *push_tri2_prim;


void SetBlackWhiteCLUT()
{
    loadbw_flg = 1;
}

void ClearBlackWhiteCLUT()
{
    loadbw_flg = 0;
}

void SendDmaON()
{
    send_dma_flg = 1;
}

void SendDmaOFF()
{
    send_dma_flg = 0;
}

void ClearDMATrans()
{
    printf("Dma Trans Error %x\n", *REG_VIF1_STAT);

    *REG_DMAC_ENABLEW = 0x10000;
    *REG_DMAC_1_VIF1_CHCR &= 0xfffffeff;
    *REG_DMAC_ENABLEW = 0;

    sceDevVif1Reset();

    printf("wait\n");
}

void CheckDMATrans()
{
    if (*REG_VIF1_STAT & 0x3400)
    {
        ClearDMATrans();
    }
}

void CheckDmaSync()
{
    while (sceDmaSync(dvif1, 1, 0) != 0)
    {
        CheckDMATrans();
    }

    if (*REG_VIF1_STAT != 0)
    {
        ClearDMATrans();
    }
}

static void FlushBuffer()
{
    int i;

    dvif1 = sceDmaGetChan(1);

    ((int *)&cachetag[vu1tag_num])[0] = 1;
    ((int *)&cachetag[vu1tag_num])[1] = (int)DummyFlushData;

    cachetag[vu1tag_num].pad[0] = cachetag[vu1tag_num].pad[1] = 0;

    CheckDMATrans();
    sceDmaSync(dvif1, 0, 0);

    dvif1->chcr.TTE = 1;

    FlushCache(0);

    if (send_dma_flg != 0)
    {
        for (i = 0; i < 1; i++)
        {
            sceDmaSync(dvif1, 0, 0);
            sceDmaSend(dvif1, (u_int *)((u_int)cachetag & 0x0fffffff));
        }
    }

    tagswap = 1 - tagswap;
    cachetag = cachetagdbuf[tagswap];
    objwork = objworkdbuf[tagswap];
    sbuffer_p = vu1tag_num = 0;
}

void FlushModel(int signal)
{
    if (signal == 1)
    {
        PutDebugSign();
        FlushBuffer();
        return;
    }

    if (model_buffer_size - 0x400 <= sbuffer_p || model_tag_size - 10 <= vu1tag_num)
    {
        if (model_buffer_size <= sbuffer_p || model_buffer_size - 10 <= vu1tag_num)
        {
            printf("Serious Buffer Over %d %d\n", sbuffer_p, vu1tag_num);

            InitialDmaBuffer();
            SetDebugSign(1);
            return;
        }

        printf("Buffer Over %d %d\n", sbuffer_p, vu1tag_num);

        FlushBuffer();
    }
}

void AppendDmaTag(u_int addr, int size)
{
    SgSourceChainTag *ptag;

    ptag = &cachetag[vu1tag_num];

    ptag->pad[0] = ptag->pad[1] = 0;

    ((int *)ptag)[0] = size | 0x30000000;
    ((int *)ptag)[1] = addr;

    vu1tag_num++;
}

void AppendDmaBuffer(int size)
{
    SgSourceChainTag *ptag;

    ptag = &cachetag[vu1tag_num];

    ptag->pad[0] = ptag->pad[1] = 0;

    ((int *)ptag)[0] = size | 0x30000000;
    ((int *)ptag)[1] = (u_int)(&objwork[sbuffer_p]) & 0xfffffff;

    sbuffer_p += size;
    vu1tag_num++;
}

void AppendDmaTagCall(u_int next_tag_addr)
{
    SgSourceChainTag *ptag;

    ptag = &cachetag[vu1tag_num];

    ((int *)ptag)[0] = 0x20000000;
    ((int *)ptag)[1] = next_tag_addr & 0x0fffffff;

    ptag->pad[0] = ptag->pad[1] = 0;

    vu1tag_num++;
}

void AppendDmaTagNextRet(void *tag_addr)
{
    ((int *)tag_addr)[0] = 0x20000000;
    ((int *)tag_addr)[1] = ((u_int)&cachetag[vu1tag_num]) & 0xfffffff;
}

void AppendDmaBufferFromEndAddress(qword *end_adr)
{
    int size;
    int tmp;
    SgSourceChainTag *ptag;
    u_int *read_p;

    ptag = &cachetag[vu1tag_num];

    tmp = sbuffer_p;

    read_p = (u_int *)getObjWrk();

    size = ((u_int)end_adr - (u_int)read_p) / 16;

    read_p[0] = 0;
    read_p[1] = 0;
    read_p[2] = 0x11000000;
    read_p[3] = (size - 1 | 0x50000000);

    ((int *)ptag)[0] = size | 0x30000000;
    ((int *)ptag)[1] = (u_int)read_p & 0xfffffff;

    ptag->pad[0] = ptag->pad[1] = 0;

    sbuffer_p = tmp + size;
    vu1tag_num++;

    FlushModel(0);
}

void ClearTextureCache()
{
    previous_tri2_prim = NULL;
}

void PushTextureCache()
{
    push_tri2_prim = previous_tri2_prim;
}

void PopTextureCahche()
{
    previous_tri2_prim = push_tri2_prim;
}

void SgSetPacketBuffer(void *packet_buffer, int packet_size, void *tag_buffer, int tag_num)
{
    model_buffer_size = packet_size;
    model_tag_size = tag_num;

    objworkdbuf[0] = (sceVu0FVECTOR *)packet_buffer;
    objworkdbuf[1] = &((sceVu0FVECTOR *)packet_buffer)[packet_size];

    cachetagdbuf[0] = (SgSourceChainTag *)tag_buffer;
    cachetagdbuf[1] = &((SgSourceChainTag *)tag_buffer)[tag_num];

    InitialDmaBuffer();
}

void SetDmaScratch()
{
    return;
}

void SetDmaBSS()
{
    return;
}

void InitialDmaBuffer()
{
    sbuffer_p = tagswap = vu1tag_num = 0;

    cachetag = cachetagdbuf[0];
    objwork = objworkdbuf[0];

    ClearTextureCache();
}

void GsImageProcess(u_int *prim)
{
    AppendDmaTag((u_int)&prim[4], prim[2]);
}

void SgTEXTransEnable()
{
    loadtri2_flg = 1;
}

void SgTEXTransDisenable()
{
    loadtri2_flg = 0;
}

void LoadTRI2Files(u_int *prim)
{
    int i;
    int pads;
    int tnum;
    int tri2size;
    u_int *read_p;

    if (prim == previous_tri2_prim || loadtri2_flg == 0)
    {
        return;
    }

    previous_tri2_prim = prim;

    tnum = prim[2];
    pads = prim[3];

    prim = (u_int *)((u_int)prim + 16 + ((u_int)pads / 4) * 4);

    prim[2] = 0x11000000;

    for (i = 0; i < tnum; i++)
    {
        tri2size = *(u_short *)(prim + 3);

        AppendDmaTag((u_int)prim, tri2size + 1);

        prim = (u_int *)((u_int)prim + 0x10 + (u_int)tri2size * 0x10);

        FlushModel(0);
    }

    read_p = (u_int *)getObjWrk();

    read_p[0] = 0x11000000;
    read_p[1] = 0;
    read_p[2] = 0;
    read_p[3] = 0x50000002;
    read_p[4] = 0x8001;
    read_p[5] = 0x10000000;
    read_p[6] = 0xe;
    read_p[10] = 0x3f;

    AppendDmaBuffer(3);
    FlushModel(0);
}

void RebuildTRI2Files(u_int *prim)
{
    u_int next_pointer;
    int tnum;
    int pads;
    int tri2size;
    int i;
    u_int maxaddr;
    u_int minaddr;
    u_int maxtbp;
    u_int *search_addr;
    u_int *start_vif_code;
    u_int *fprim;
    u_int tbp;
    u_int tbw;
    u_int psm;
    u_int tsize;
    u_int vtsize;
    qword *base;
    sceGsStoreImage spi;

    fprim = prim;
    next_pointer = prim[0];
    tnum = prim[2];
    pads = prim[3];
    maxaddr = 0;
    minaddr = -1;
    maxtbp = 0;

    if (tnum <= 2)
    {
        return;
    }

    InitialDmaBuffer();

    start_vif_code = (u_int *)((int)&fprim[4] + ((u_int)pads / 4) * 4);

    prim = start_vif_code;

    for (i = 0; i < tnum; prim += (u_int)tri2size * 4 + 4, i++)
    {
        tri2size = *(u_short *)&prim[3];

        AppendDmaTag((u_int)prim, tri2size + 1);
        FlushModel(0);

        search_addr = prim + 4;

        while (((u_int)search_addr - (u_int)(prim)) / 16 < tri2size - 8)
        {
            tbp = search_addr[5] & 0xffff;
            tbw = search_addr[5] >> 16 & 0x3f;
            psm = search_addr[5] >> 24 & 0x3f;
            tsize = search_addr[20] & 0x7fff;

            if (tbp < minaddr)
            {
                minaddr = tbp;
            }

            if (maxtbp < tbp)
            {
                maxtbp = tbp;

                switch(psm)
                {
                    case SCE_GS_PSMT8:
                    case SCE_GS_PSMT4:
                        vtsize = tsize << (tbw % 2);
                    break;
                    case SCE_GS_PSMCT32:
                    case SCE_GS_PSMCT24:
                    case SCE_GS_PSMCT16:
                    case SCE_GS_PSMCT16S:
                    default:
                        vtsize = tsize;
                    break;
                }

                maxaddr = tbp + vtsize / 16;
            }

            search_addr = &search_addr[24 + tsize * 4];
        }
    }

    base = (qword *)getObjWrk();

    base[0][0] = 0x11000000;
    base[0][1] = 0;
    base[0][2] = 0;
    base[0][3] = 0x50000002;

    base[1][0] = 0x8001;
    base[1][1] = 0x10000000;
    base[1][2] = 0xe;

    base[2][2] = 0x3f;

    AppendDmaBuffer(3);
    FlushModel(1);
    sceGsSyncPath(0,0);

    tsize = maxaddr - minaddr;

    if ((tsize % 32) != 0)
    {
        tsize = (tsize / 32) + 1;
    }
    else
    {
        tsize = tsize / 32;
    }

    if (next_pointer - (u_int)start_vif_code < tsize * 0x2000 + 0x180)
    {
        printf("Not Enough Memory %d %d\n", next_pointer - (u_int)start_vif_code, tsize * 0x2000 + 0x180);
        return;
    }

    tnum = 0;

    while (tsize != 0)
    {
        vtsize = tsize < 0x40 ? tsize : 0x3f;

        sceGsSetDefStoreImage(&spi, minaddr, 1, 0, 0, 0, 0x40, vtsize * 32);
        FlushCache(0);
        sceGsExecStoreImage(&spi, (u_long128 *)&start_vif_code[28]);
        sceGsSyncPath(0, 0);

        start_vif_code[0] = 0;
        start_vif_code[1] = 0;
        start_vif_code[2] = 0;
        start_vif_code[3] = SCE_VIF1_SET_DIRECT(vtsize * 0x200 + 6, 0);

        *(u_long *)&start_vif_code[4] = SCE_GIF_SET_TAG(1, SCE_GS_FALSE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 4);
        *(u_long *)&start_vif_code[6] = 0xeeee;
        *(u_long *)&start_vif_code[8] = SCE_GS_SET_BITBLTBUF(0, 0, SCE_GS_PSMCT32, minaddr, 1, SCE_GS_PSMCT32);
        *(u_long *)&start_vif_code[10] = SCE_GS_BITBLTBUF;
        *(u_long *)&start_vif_code[12] = SCE_GS_SET_TRXPOS(0, 0, 0, 0, 0);
        *(u_long *)&start_vif_code[14] = SCE_GS_TRXPOS;
        *(u_long *)&start_vif_code[16] = SCE_GS_SET_TRXREG(2 * 32, vtsize * 32);
        *(u_long *)&start_vif_code[18] = SCE_GS_TRXREG;
        *(u_long *)&start_vif_code[20] = SCE_GS_SET_TRXDIR(0);
        *(u_long *)&start_vif_code[22] = SCE_GS_TRXDIR;
        *(u_long *)&start_vif_code[24] = SCE_GIF_SET_TAG(vtsize * 0x200, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_IMAGE, 0);

        start_vif_code = start_vif_code + 4 + (vtsize * 0x200 + 6) * 4;
        tsize -= vtsize;
        minaddr = minaddr + vtsize * 32;
        tnum++;
    }

    fprim[2] = tnum;
}

TextureAnimation* GetTextureAnimation(void *sgd_top)
{
    u_int *prim;
    HeaderSection *hs;

    hs = (HeaderSection *)sgd_top;

    prim = (u_int *)hs->primitives;

    if (prim == NULL)
    {
        return NULL;
    }

    while(prim[0] != NULL)
    {
        if (prim[1] == 12)
        {
            return (TextureAnimation *)&prim[2];
        }

        prim = (u_int *)prim[0];
    }

    return NULL;
}

void LoadTextureAnimation(u_int *prim)
{
    TextureAnimation *pta;
    int i;
    int tnum;
    int tri2size;
    u_int *data_p;

    pta = (TextureAnimation *)&prim[2];

    if (pta->AnmON != 0)
    {
        if (pta->AnmStep != 0)
        {
            tnum = (pta->AnmCnt * pta->AnmStep) / 16;
        }
        else
        {
            tnum = pta->AnmCnt;
        }

        pta->AnmCnt++;

        if (pta->TexNum <= tnum)
        {
            if (pta->AnmLoop == 0)
            {
                pta->AnmON = 0;
            }

            tnum = 0;
            pta->AnmCnt = 0;
        }
    }
    else
    {
        tnum = pta->AnmCnt;
    }

    prim = (u_int *)((int)&prim[4] + (pta->pads / 4) * 4);

    for (i = 0; i < tnum; i++)
    {
        tri2size = *(u_short *)&prim[3];
        prim = (u_int *)((u_int)&prim[4] + tri2size * 16);
    }

    tri2size = *(u_short *)&prim[3];

    prim[2] = 0x11000000;

    AppendDmaTag((u_int)prim, tri2size + 1);
    FlushModel(0);

    data_p = (u_int *)getObjWrk();

    data_p[0] = 0x11000000;
    data_p[1] = 0;
    data_p[2] = 0;
    data_p[3] = 0x50000002;
    data_p[4] = 0x8001;
    data_p[5] = 0x10000000;
    data_p[6] = 0xe;
    data_p[10] = 0x3f;

    AppendDmaBuffer(3);
    FlushModel(0);
}
