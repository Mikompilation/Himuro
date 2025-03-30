#include "common.h"
#include "typedefs.h"
#include "tim2.h"

#include "ee/kernel.h"
#include "ee/eekernel.h"
#include "sce/libdma.h"
#include "sce/libgraph.h"
#include "sce/sifdev.h"

#include "graphics/graph2d/tim2_new.h"
#include "graphics/graph3d/sglib.h"

// borrowed and adapted from FF2 symbols
typedef struct {
    int pack_num;
    int pad[3];
    u_int offset[0];
} PK2_HEAD;

u_int draw_pri[4096][2] = {0};
Q_WORDDATA mpbufw[2][8192] = {0};
int draw_mpri[1024][2] = {0};

u_char g_bInterlace = 1;
int ndpkt = 0;
int ndpri = 0;
int nmdpkt = 0;
int nmdpri = 0;
int mes_swap = 0;
Q_WORDDATA *pbuf = NULL;
Q_WORDDATA *mpbuf = NULL;

static u_int next_tex_addr;
static u_int next_clut_addr;
static sceDmaChan *DmaGif;
static sceDmaChan *DmaVif;

#define PI 3.1415927f

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 224

#define PBUF_ADDRESS 0x720000

static int Tim2CalcBufWidth(int psm, int w);
static int Tim2CalcBufSize(int psm, int w, int h);
static int Tim2GetLog2(int n);
static void InitTIM2Addr();
static void FlushTextureCache();
static int GetLog2(u_int n);

void printClut(void *pClut, int ClutColors)
{
    int i;

    for (i = 0; i < ClutColors; i++)
    {
        // do nothing, probably some debug code to print stuff that has been omitted from the release build
    }
}

void ResetClutAlpha(void *pClut, int ClutColors)
{
    int i;
    u_char *pointer;

    pointer = &((u_char *)pClut)[3];

    for (i = 0; i < ClutColors; i++, pointer += 4)
    {
        if (*pointer > 0x80)
        {
            *pointer -= 0x7f;
        }
    }
}

void ResetPIXELAlpha(u_char *ip, int size)
{
    int i;

    for (i = 0; i < size / 2; i++)
    {
        // do nothing
    }
}

int Tim2CheckFileHeaer(void *pTim2)
{
    TIM2_FILEHEADER *pFileHdr;
    int i;

    pFileHdr = (TIM2_FILEHEADER *)pTim2;

    if (*(u_short *)&pFileHdr->FileId[0] == 0x4954 && *(u_short *)&pFileHdr->FileId[2] == 0x324d)
    { // 'TIM2'
        i = 1;
    }
    else if (*(u_short *)&pFileHdr->FileId[0] == 0x4c43 && *(u_short *)&pFileHdr->FileId[2] == 0x3254)
    { // 'CLT2'
        i = 2;
    }
    else
    {
        return 0;
    }

    if(
        pFileHdr->FormatVersion != 3 &&
        (pFileHdr->FormatVersion != 4 || (pFileHdr->FormatId != 0 && pFileHdr->FormatId != 1))
    )
    {
        return 0;
    }

    return i;
}

TIM2_PICTUREHEADER* Tim2GetPictureHeader(void *pTim2, int imgno)
{
    TIM2_PICTUREHEADER *pPictHdr;
    int i;
    TIM2_FILEHEADER *pFileHdr;

    pFileHdr = (TIM2_FILEHEADER *)pTim2;

    if (imgno>=pFileHdr->Pictures)
    {
        return NULL;
    }

    if (pFileHdr->FormatId == 0)
    {
        pPictHdr = (TIM2_PICTUREHEADER *)((char *)pTim2 + sizeof(TIM2_FILEHEADER));
    }
    else
    {
        pPictHdr = (TIM2_PICTUREHEADER *)((char *)pTim2 + 0x80);
    }

    // go to picture number
    for (i = 0; i < imgno; i++)
    {
        pPictHdr = (TIM2_PICTUREHEADER *)((char *)pPictHdr + pPictHdr->TotalSize);
    }

    return pPictHdr;
}

int Tim2IsClut2(TIM2_PICTUREHEADER *ph)
{
    // 1: TIM2, 0: CLUT2
    return ph->MipMapTextures == 0 ? 1 : 0;
}

int Tim2GetMipMapPictureSize(TIM2_PICTUREHEADER *ph, int mipmap, int *pWidth, int *pHeight)
{
    int w;
    int h;
    int n;

    w = ph->ImageWidth >> mipmap;
    h = ph->ImageHeight >> mipmap;

    if(pWidth)
    {
        *pWidth  = w;
    }

    if (pHeight)
    {
        *pHeight = h;
    }

    n = w * h;

    switch(ph->ImageType)
    {
    case TIM2_RGB16:
        n *= 2;
    break;
    case TIM2_RGB24:
        n *= 3;
    break;
    case TIM2_RGB32:
        n *= 4;
    break;
    case TIM2_IDTEX4:
        n /= 2;
    break;
    case TIM2_IDTEX8:
    break;
    }

    // MIPMAP texture size is always 16 bytes aligned
    n = (n + 15) & ~15;

    return n;
}

TIM2_MIPMAPHEADER* Tim2GetMipMapHeader(TIM2_PICTUREHEADER *ph, int *pSize)
{
    TIM2_MIPMAPHEADER *pMmHdr;
    const static char mmsize[] = {
        0,  // Texture number 0 (for CLUT2 data)
        0,  // LV0 texture only (no MIPMAP header)
        32, // Up to LV1 MipMap
        32, // Up to LV2 MipMap
        32, // Up to LV3 MipMap
        48, // Up to LV4 MipMap
        48, // Up to LV5 MipMap
        48, // Up to LV6 MipMap
    };

    if (ph->MipMapTextures > 1)
    {
        pMmHdr = (TIM2_MIPMAPHEADER *)((char *)ph + sizeof(TIM2_PICTUREHEADER));
    }
    else
    {
        pMmHdr = NULL;
    }

    if (pSize)
    {
        // no extended header
        *pSize = mmsize[ph->MipMapTextures];
    }

    return pMmHdr;
}

void* Tim2GetUserSpace(TIM2_PICTUREHEADER *ph, int *pSize)
{
    void *pUserSpace;
    const static char mmsize[] = {
        sizeof(TIM2_PICTUREHEADER),      // Texture number 0 (for CLUT2 data)
        sizeof(TIM2_PICTUREHEADER),      // LV0 texture only (no MIPMAP header)
        sizeof(TIM2_PICTUREHEADER) + 32, // Up to LV1 MipMap
        sizeof(TIM2_PICTUREHEADER) + 32, // Up to LV2 MipMap
        sizeof(TIM2_PICTUREHEADER) + 32, // Up to LV3 MipMap
        sizeof(TIM2_PICTUREHEADER) + 48, // Up to LV4 MipMap
        sizeof(TIM2_PICTUREHEADER) + 48, // Up to LV5 MipMap
        sizeof(TIM2_PICTUREHEADER) + 48, // Up to LV6 MipMap
    };

    if (ph->HeaderSize == mmsize[ph->MipMapTextures])
    {
        if (pSize)
        {
            *pSize = 0;
        }

        return NULL;
    }

    pUserSpace = (void *)((char *)ph + mmsize[ph->MipMapTextures]);

    if (pSize)
    {
        TIM2_EXHEADER *pExHdr;

        pExHdr = (TIM2_EXHEADER *)pUserSpace;

        if (
            pExHdr->ExHeaderId[0] != 'e' ||
            pExHdr->ExHeaderId[1] != 'X' ||
            pExHdr->ExHeaderId[2] != 't' ||
            pExHdr->ExHeaderId[3] != 0
        )
        {
            // without extended header
            *pSize = (ph->HeaderSize - mmsize[ph->MipMapTextures]);
        }
        else
        {
            // with extended header
            *pSize = (int)pExHdr->UserSpaceSize;
        }
    }

    return pUserSpace;
}

void* Tim2GetUserData(TIM2_PICTUREHEADER *ph, int *pSize)
{
    void *pUserSpace;
    TIM2_EXHEADER *pExHdr;

    pUserSpace = Tim2GetUserSpace(ph, pSize);

    if (pUserSpace == NULL)
    {
        return NULL;
    }

    pExHdr = (TIM2_EXHEADER *)pUserSpace;

    if (
        pExHdr->ExHeaderId[0] != 'e' ||
        pExHdr->ExHeaderId[1] != 'X' ||
        pExHdr->ExHeaderId[2] != 't' ||
        pExHdr->ExHeaderId[3] != 0
    )
    {
        return pUserSpace;
    }

    if (pSize)
    {
        *pSize = (int)pExHdr->UserDataSize;
    }

    return (char *)pUserSpace + sizeof(TIM2_EXHEADER);
}

char* Tim2GetComment(TIM2_PICTUREHEADER *ph)
{
    void *pUserSpace;
    TIM2_EXHEADER *pExHdr;

    pUserSpace = Tim2GetUserSpace(ph, NULL);

    if (pUserSpace == NULL)
    {
        return NULL;
    }

    pExHdr = (TIM2_EXHEADER *)pUserSpace;

    if (
        pExHdr->ExHeaderId[0] != 'e' ||
        pExHdr->ExHeaderId[1] != 'X' ||
        pExHdr->ExHeaderId[2] != 't' ||
        pExHdr->ExHeaderId[3] != 0
    )
    {
        return    NULL;
    }

    if (pExHdr->UserSpaceSize == sizeof(TIM2_EXHEADER) + pExHdr->UserDataSize)
    {
        return NULL;
    }

    return (char *)pUserSpace + sizeof(TIM2_EXHEADER) + pExHdr->UserDataSize;
}

void* Tim2GetImage(TIM2_PICTUREHEADER *ph, int mipmap)
{
    void *pImage;
    int i;
    TIM2_MIPMAPHEADER *pm;

    if (mipmap>=ph->MipMapTextures)
    {
        return NULL;
    }

    pImage = (void *)((char *)ph + ph->HeaderSize);

    if (ph->MipMapTextures == 1)
    {
        return pImage;
    }

    pm = (TIM2_MIPMAPHEADER *)((char *)ph + sizeof(TIM2_PICTUREHEADER));

    for (i = 0; i < mipmap; i++)
    {
        pImage = (void *)((char *)pImage + pm->MMImageSize[i]);
    }

    return pImage;
}

void* Tim2GetClut(TIM2_PICTUREHEADER *ph)
{
    void *pClut;

    if (ph->ClutColors == 0)
    {
        pClut = NULL;
    }
    else
    {
        pClut = (void *)((char *)ph + ph->HeaderSize + ph->ImageSize);
    }

    return pClut;
}

u_int Tim2GetClutColor(TIM2_PICTUREHEADER *ph, int clut, int no)
{
    unsigned char *pClut;
    int n;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    pClut = Tim2GetClut(ph);

    if (pClut == NULL)
    {
        return 0;
    }

    switch (ph->ImageType)
    {
    case TIM2_IDTEX4:
        n = clut * 16 + no;
    break;
    case TIM2_IDTEX8:
        n = clut * 256 + no;
    break;
    default:
        return 0;
    }

    if (n > ph->ClutColors)
    {
        return 0;
    }

    switch (ph->ClutType << 8 | ph->ImageType)
    {
    case (((TIM2_RGB16 | 0x40) << 8) | TIM2_IDTEX4):
    case (((TIM2_RGB24 | 0x40) << 8) | TIM2_IDTEX4):
    case (((TIM2_RGB32 | 0x40) << 8) | TIM2_IDTEX4):
    case (((TIM2_RGB16 | 0x00) << 8) | TIM2_IDTEX8):
    case (((TIM2_RGB24 | 0x00) << 8) | TIM2_IDTEX8):
    case (((TIM2_RGB32 | 0x00) << 8) | TIM2_IDTEX8):
        if ((n & 31) >= 8)
        {
            if ((n & 31) < 16)
            {
                n += 8;
            }
            else if ((n & 31) < 24)
            {
                n -= 8;
            }
        }
    break;
    }

    switch (ph->ClutType & 0x3F)
    {
    case TIM2_RGB16:
        r = (unsigned char)((((pClut[n*2 + 1] << 8) | pClut[n*2]) << 3) & 0xF8);
        g = (unsigned char)((((pClut[n*2 + 1] << 8) | pClut[n*2]) >> 2) & 0xF8);
        b = (unsigned char)((((pClut[n*2 + 1] << 8) | pClut[n*2]) >> 7) & 0xF8);
        a = (unsigned char)((((pClut[n*2 + 1] << 8) | pClut[n*2]) >> 8) & 0x80);
    break;
    case TIM2_RGB24:
        r = pClut[n*3];
        g = pClut[n*3+1];
        b = pClut[n*3+2];
        a = 0x80;
    break;
    case TIM2_RGB32:
        r = pClut[n*4+0];
        g = pClut[n*4+1];
        b = pClut[n*4+2];
        a = pClut[n*4+3];
    break;
    default:
        r = 0;
        g = 0;
        b = 0;
        a = 0;
    break;
    }

    return (u_int)(
        (a << 24) |
        (g << 16) |
        (b <<  8) |
        (r <<  0)
    );
}

u_int Tim2SetClutColor(TIM2_PICTUREHEADER *ph, int clut, int no, u_int newcolor)
{
    unsigned char *pClut;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    int n;

    pClut = Tim2GetClut(ph);

    if (pClut == NULL)
    {
        return 0;
    }

    switch (ph->ImageType)
    {
    case TIM2_IDTEX4:
        n = clut * 16 + no;
    break;
    case TIM2_IDTEX8:
        n = clut * 256 + no;
    break;
    default:
        return 0;
    break;
    }

    if (n > ph->ClutColors)
    {
        return 0;
    }

    switch ((ph->ClutType<<8) | ph->ImageType)
    {
    case (((TIM2_RGB16 | 0x40) << 8) | TIM2_IDTEX4):
    case (((TIM2_RGB24 | 0x40) << 8) | TIM2_IDTEX4):
    case (((TIM2_RGB32 | 0x40) << 8) | TIM2_IDTEX4):
    case (((TIM2_RGB16 | 0x00) << 8) | TIM2_IDTEX8):
    case (((TIM2_RGB24 | 0x00) << 8) | TIM2_IDTEX8):
    case (((TIM2_RGB32 | 0x00) << 8) | TIM2_IDTEX8):
        if ((n & 31) >= 8)
        {
            if ((n & 31) < 16)
            {
                n += 8;
            }
            else if ((n & 31) < 24)
            {
                n -= 8;
            }
        }
    break;
    }

    switch (ph->ClutType & 0x3F)
    {
    case TIM2_RGB16:
    {
        u_char rr;
        u_char gg;
        u_char bb;
        u_char aa;

        r = (u_char)((((pClut[n*2+1] << 8) | pClut[n*2]) << 3) & 0xF8);
        g = (u_char)((((pClut[n*2+1] << 8) | pClut[n*2]) >> 2) & 0xF8);
        b = (u_char)((((pClut[n*2+1] << 8) | pClut[n*2]) >> 7) & 0xF8);
        a = (u_char)((((pClut[n*2+1] << 8) | pClut[n*2]) >> 8) & 0x80);

        rr = (u_char)((newcolor >>  3) & 0x1F);
        gg = (u_char)((newcolor >> 11) & 0x1F);
        bb = (u_char)((newcolor >> 19) & 0x1F);
        aa = (u_char)((newcolor >> 31) & 0x01);

        pClut[n*2+0] = (u_char)((((aa << 15) | (bb << 10) | (gg << 5) | rr) >> 0) & 0xFF);
        pClut[n*2+1] = (u_char)((((aa << 15) | (bb << 10) | (gg << 5) | rr) >> 8) & 0xFF);
    }
    break;
    case TIM2_RGB24:
        r = pClut[n*3+0];
        g = pClut[n*3+1];
        b = pClut[n*3+2];
        a = 0x80;

        pClut[n*3+0] = (u_char)((newcolor >>  0) & 0xFF);
        pClut[n*3+1] = (u_char)((newcolor >>  8) & 0xFF);
        pClut[n*3+2] = (u_char)((newcolor >> 16) & 0xFF);
    break;
    case TIM2_RGB32:
        r = pClut[n*4+0];
        g = pClut[n*4+1];
        b = pClut[n*4+2];
        a = pClut[n*4+3];

        pClut[n*4+0] = (u_char)((newcolor >>  0) & 0xFF);
        pClut[n*4+1] = (u_char)((newcolor >>  8) & 0xFF);
        pClut[n*4+2] = (u_char)((newcolor >> 16) & 0xFF);
        pClut[n*4+3] = (u_char)((newcolor >> 24) & 0xFF);
    break;
    default:
        r = 0;
        g = 0;
        b = 0;
        a = 0;
    break;
    }

    return (u_int)(
        (a << 24) |
        (g << 16) |
        (b <<  8) |
        (r <<  0)
    );
}

u_int Tim2GetTexel(TIM2_PICTUREHEADER *ph, int mipmap, int x, int y)
{
    unsigned char *pImage;
    int t;
    int w;
    int h;

    pImage = Tim2GetImage(ph, mipmap);

    if (pImage == NULL)
    {
        return 0;
    }

    Tim2GetMipMapPictureSize(ph, mipmap, &w, &h);

    if(x > w || y > h)
    {
        return 0;
    }

    t = y * w + x;

    switch (ph->ImageType)
    {
    case TIM2_RGB16:
        return (u_int)((pImage[t*2+1] <<  8) | pImage[t*2]);
    break;
    case TIM2_RGB24:
        return (u_int)((pImage[t*3+2] << 16) | (pImage[t*3+1] << 8) | pImage[t*3]);
    break;
    case TIM2_RGB32:
        return (u_int)((pImage[t*4+3] << 24) | (pImage[t*4+2] << 16) | (pImage[t*4+1] << 8) | pImage[t*4]);
    break;
    case TIM2_IDTEX4:
        if (x & 1)
        {
            return (u_int)(pImage[t/2] >> 4);
        }
        else
        {
            return (u_int)(pImage[t/2] & 0x0F);
        }
    case TIM2_IDTEX8:
        return pImage[t];
    break;
    }

    return 0;
}

u_int Tim2SetTexel(TIM2_PICTUREHEADER *ph, int mipmap, int x, int y, u_int newtexel)
{
    unsigned char *pImage;
    int t;
    int w;
    int h;
    u_int oldtexel;

    pImage = Tim2GetImage(ph, mipmap);

    if (pImage == NULL)
    {
        return 0;
    }

    Tim2GetMipMapPictureSize(ph, mipmap, &w, &h);

    if (x > w || y > h)
    {
        return 0;
    }

    t = y * w + x;

    switch (ph->ImageType)
    {
    case TIM2_RGB16:
        oldtexel = (u_int)(pImage[t*2+1] << 8) | pImage[t*2];
        pImage[t*2+0] = (u_char)((newtexel >> 0) & 0xFF);
        pImage[t*2+1] = (u_char)((newtexel >> 8) & 0xFF);

        return oldtexel;
    break;
    case TIM2_RGB24:
        oldtexel = (u_int)(pImage[t*3+2] << 16) | (pImage[t*3+1] << 8) | pImage[t*3];
        pImage[t*3+0] = (u_char)((newtexel >>  0) & 0xFF);
        pImage[t*3+1] = (u_char)((newtexel >>  8) & 0xFF);
        pImage[t*3+2] = (u_char)((newtexel >> 16) & 0xFF);

        return oldtexel;
    break;
    case TIM2_RGB32:
        oldtexel = (u_int)(pImage[t*4+3] << 24) | (pImage[t*4+2] << 16) | (pImage[t*4+1] << 8) | pImage[t*4];
        pImage[t*4+0] = (u_char)((newtexel >>  0) & 0xFF);
        pImage[t*4+1] = (u_char)((newtexel >>  8) & 0xFF);
        pImage[t*4+2] = (u_char)((newtexel >> 16) & 0xFF);
        pImage[t*4+3] = (u_char)((newtexel >> 24) & 0xFF);

        return oldtexel;
    break;
    case TIM2_IDTEX4:
        if(x & 1)
        {
            oldtexel = (u_int)pImage[t/2] >> 4;
            pImage[t/2] = (u_char)((newtexel << 4) | oldtexel);
        }
        else
        {
            oldtexel = (u_int)pImage[t/2] & 0x0F;
            pImage[t/2] = (u_char)((oldtexel << 4) | newtexel);
        }

        return oldtexel;
    break;
    case TIM2_IDTEX8:
        oldtexel = pImage[t];
        pImage[t] = (u_char)newtexel;

        return oldtexel;
    break;
    }

    return 0;
}

u_int Tim2GetTextureColor(TIM2_PICTUREHEADER *ph, int mipmap, int clut, int x, int y)
{
    u_int t;

    if (Tim2GetImage(ph, mipmap) == NULL)
    {
        return 0;
    }

    t = Tim2GetTexel(ph, mipmap, x >> mipmap, y >> mipmap);

    switch (ph->ImageType)
    {
    case TIM2_RGB16: {
        u_char r;
        u_char g;
        u_char b;
        u_char a;

        r = (u_char)((t << 3) & 0xF8);
        g = (u_char)((t >> 2) & 0xF8);
        b = (u_char)((t >> 7) & 0xF8);
        a = (u_char)((t >> 8) & 0x80);

        return (u_int)((a<<24) | (g<<16) | (b<<8) | r);
    }
    break;
    case TIM2_RGB24:
        return (0x80 << 24) | (t & 0x00FFFFFF);
    break;
    case TIM2_RGB32:
        return t;
    break;
    case TIM2_IDTEX4:
    case TIM2_IDTEX8:
        return Tim2GetClutColor(ph, clut, t);
    break;
    }

    return 0;
}

u_int Tim2LoadPicture2(TIM2_PICTUREHEADER *ph, u_int tbp, u_int cbp, u_int offset)
{
    u_int addr;

    addr = Tim2LoadImage2(ph, tbp, offset);

    Tim2LoadClut2(ph, cbp, offset);

    return addr;
}

u_int Tim2LoadPicture(TIM2_PICTUREHEADER *ph, u_int tbp, u_int cbp)
{
    Tim2LoadPicture2(ph, tbp, cbp, 0);
}

u_int Tim2LoadImage2(TIM2_PICTUREHEADER *ph, u_int tbp, u_int offset)
{
    static const int psmtbl[] = {
        SCE_GS_PSMCT16,
        SCE_GS_PSMCT24,
        SCE_GS_PSMCT32,
        SCE_GS_PSMT4,
        SCE_GS_PSMT8
    };
    int i;
    int psm;
    u_long128 *pImage;
    int w;
    int h;
    int tbw;
    TIM2_MIPMAPHEADER *pm;
    int miptbp;

    if (ph->MipMapTextures > 0)
    {
        psm = psmtbl[ph->ImageType - 1];

        ((sceGsTex0 *)&ph->GsTex0)->PSM = psm;

        w = ph->ImageWidth;
        h = ph->ImageHeight;

        pImage = (u_long128 *)((char *)ph + ph->HeaderSize);

        if (tbp == -1)
        {

            tbp = ((sceGsTex0 *)&ph->GsTex0)->TBP0 + offset;
            tbw = ((sceGsTex0 *)&ph->GsTex0)->TBW;

            if (ph->ImageSize != 0)
            {
                Tim2LoadTexture(psm, tbp, tbw, w, h, pImage);
            }
        }
        else
        {

            tbw = Tim2CalcBufWidth(psm, w);

            ((sceGsTex0 *)&ph->GsTex0)->TBP0 = tbp + offset;
            ((sceGsTex0 *)&ph->GsTex0)->TBW  = tbw;

            Tim2LoadTexture(psm, tbp, tbw, w, h, pImage);

            tbp += (u_int)Tim2CalcBufSize(psm, w, h);
        }

        if (ph->MipMapTextures > 1)
        {
            pm = (TIM2_MIPMAPHEADER *)(ph + 1);

            if (tbp != -1)
            {
                pm->GsMiptbp1 = 0;
                pm->GsMiptbp2 = 0;
            }

            pImage = (u_long128 *)((char *)ph + ph->HeaderSize);

            for (i = 1; i < ph->MipMapTextures; i++)
            {
                w = w / 2;
                h = h / 2;

                pImage = (u_long128 *)((char *)pImage + pm->MMImageSize[i-1]);

                if (tbp == -1)
                {
                    if (i < 4)
                    {
                        miptbp = (pm->GsMiptbp1 >> ((i-1)*20)) & 0x3FFF;
                        tbw = (pm->GsMiptbp1 >> ((i-1)*20+14)) & 0x3F;
                    }
                    else
                    {
                        miptbp = (pm->GsMiptbp2>>((i-4)*20)) & 0x3FFF;
                        tbw = (pm->GsMiptbp2>>((i-4)*20+14)) & 0x3F;
                    }

                    Tim2LoadTexture(psm, (u_int)miptbp, tbw, w, h, pImage);
                }
                else
                {
                    tbw = Tim2CalcBufWidth(psm, w);

                    if (i < 4)
                    {
                        pm->GsMiptbp1 |= ((u_long)tbp) << ((i-1)*20);
                        pm->GsMiptbp1 |= ((u_long)tbw) << ((i-1)*20+14);
                    }
                    else
                    {
                        pm->GsMiptbp2 |= ((u_long)tbp) << ((i-4)*20);
                        pm->GsMiptbp2 |= ((u_long)tbw) << ((i-4)*20+14);
                    }

                    Tim2LoadTexture(psm, tbp, tbw, w, h, pImage);

                    tbp += Tim2CalcBufSize(psm, w, h);

                }
            }
        }
    }

    return tbp;
}

u_int Tim2LoadImage(TIM2_PICTUREHEADER *ph, u_int tbp)
{
    Tim2LoadImage2(ph, tbp, 0);
}

u_int Tim2LoadClut2(TIM2_PICTUREHEADER *ph, u_int cbp, u_int offset)
{
    int i;
    sceGsLoadImage li;
    u_long128 *pClut;
    int cpsm;

    if (ph->ClutType == TIM2_NONE)
    {
        return 1;
    }
    else if ((ph->ClutType & 0x3F) == TIM2_RGB16)
    {
        cpsm = SCE_GS_PSMCT16;
    }
    else if ((ph->ClutType & 0x3F) == TIM2_RGB24)
    {
        cpsm = SCE_GS_PSMCT24;
    }
    else
    {
        cpsm = SCE_GS_PSMCT32;
    }

    ((sceGsTex0 *)&ph->GsTex0)->CPSM = cpsm;
    ((sceGsTex0 *)&ph->GsTex0)->CSM  = 0;
    ((sceGsTex0 *)&ph->GsTex0)->CSA  = 0;
    ((sceGsTex0 *)&ph->GsTex0)->CLD  = 1;

    if (cbp == -1)
    {
        cbp = (u_int)((sceGsTex0 *)&ph->GsTex0)->CBP + offset;
    }
    else
    {
        ((sceGsTex0 *)&ph->GsTex0)->CBP = cbp + offset;
    }

    pClut = (u_long128 *)((char *)ph + ph->HeaderSize + ph->ImageSize);

    switch ((ph->ClutType << 8) | ph->ImageType)
    {
        case (((TIM2_RGB16 | 0x40) << 8) | TIM2_IDTEX4):
        case (((TIM2_RGB24 | 0x40) << 8) | TIM2_IDTEX4):
        case (((TIM2_RGB32 | 0x40) << 8) | TIM2_IDTEX4):
        case (((TIM2_RGB16 | 0x00) << 8) | TIM2_IDTEX8):
        case (((TIM2_RGB24 | 0x00) << 8) | TIM2_IDTEX8):
        case (((TIM2_RGB32 | 0x00) << 8) | TIM2_IDTEX8):
            Tim2LoadTexture(cpsm, cbp, 1, 16, (ph->ClutColors / 16), pClut);
        break;
        case (((TIM2_RGB16 | 0x00) << 8) | TIM2_IDTEX4):
        case (((TIM2_RGB24 | 0x00) << 8) | TIM2_IDTEX4):
        case (((TIM2_RGB32 | 0x00) << 8) | TIM2_IDTEX4):
        case (((TIM2_RGB16 | 0x80) << 8) | TIM2_IDTEX4):
        case (((TIM2_RGB24 | 0x80) << 8) | TIM2_IDTEX4):
        case (((TIM2_RGB32 | 0x80) << 8) | TIM2_IDTEX4):
        case (((TIM2_RGB16 | 0x80) << 8) | TIM2_IDTEX8):
        case (((TIM2_RGB24 | 0x80) << 8) | TIM2_IDTEX8):
        case (((TIM2_RGB32 | 0x80) << 8) | TIM2_IDTEX8):
            for(i=0; i<(ph->ClutColors/16); i++)
            {
                sceGsSetDefLoadImage(&li, cbp, 1, cpsm, (i & 1) * 8, (i >> 1) * 2, 8, 2);

                FlushCache(WRITEBACK_DCACHE);

                sceGsExecLoadImage(&li, pClut);
                sceGsSyncPath(0, 0);

                if ((ph->ClutType & 0x3F) == TIM2_RGB16)
                {
                    pClut = (u_long128 *)((char *)pClut + 2 * 16);
                }
                else if ((ph->ClutType & 0x3F) == TIM2_RGB24)
                {
                    pClut = (u_long128 *)((char *)pClut + 3 * 16);
                }
                else
                {
                    pClut = (u_long128 *)((char *)pClut + 4 * 16);

                }
            }
        break;
        default:
            return 0;
        break;
    }

    return 1;
}

u_int Tim2LoadClut(TIM2_PICTUREHEADER *ph, u_int cbp)
{
    Tim2LoadClut2(ph, cbp, 0);
}

int Tim2TakeSnapshot(sceGsDispEnv *d0, sceGsDispEnv *d1, char *pszFname)
{
    int i;
    int h;
    int nWidth;
    int nHeight;
    int nImageType;
    int psm;
    int nBytes;
    TIM2_FILEHEADER fhdr;
    TIM2_PICTUREHEADER phdr;
    int nImageSize;
    u_char buf[4096];
    sceGsStoreImage si;

    nWidth  = d0->display.DW / (d0->display.MAGH + 1);
    nHeight = d0->display.DH + 1;
    psm     = d0->dispfb.PSM;

    switch(psm) {
    case SCE_GS_PSMCT32:
        nBytes = nWidth * 4;
        nImageType = TIM2_RGB32;
    break;
    case SCE_GS_PSMCT24:
        nBytes = nWidth * 3;
        nImageType = TIM2_RGB24;
    break;
    case SCE_GS_PSMCT16:
        nBytes = nWidth * 2;
        nImageType = TIM2_RGB16;
    break;
    case SCE_GS_PSMCT16S:
        nBytes = nWidth * 2;
        nImageType = TIM2_RGB16;
    break;
    default:
    return 0;
    }

    h = sceOpen(pszFname, SCE_WRONLY | SCE_CREAT);

    if (h == -1)
    {
        return 0;
    }

    fhdr.FileId[0] = 'T';
    fhdr.FileId[1] = 'I';
    fhdr.FileId[2] = 'M';
    fhdr.FileId[3] = '2';
    fhdr.FormatVersion = 3;
    fhdr.FormatId  = 0;
    fhdr.Pictures  = 1;

    for (i = 0; i < 8; i++)
    {
        fhdr.pad[i] = 0x00;
    }

    sceWrite(h, &fhdr, sizeof(TIM2_FILEHEADER));

    nImageSize = nBytes * nHeight;

    phdr.TotalSize = sizeof(TIM2_PICTUREHEADER) + nImageSize;
    phdr.ClutSize = 0;
    phdr.ImageSize = nImageSize;
    phdr.HeaderSize = sizeof(TIM2_PICTUREHEADER);
    phdr.ClutColors = 0;
    phdr.PictFormat = 0;
    phdr.MipMapTextures = 1;
    phdr.ClutType = TIM2_NONE;
    phdr.ImageType = nImageType;
    phdr.ImageWidth = nWidth;
    phdr.ImageHeight = nHeight;
    phdr.GsTex0 = 0;
    ((sceGsTex0 *)&phdr.GsTex0)->TBW = Tim2CalcBufWidth(psm, nWidth);
    ((sceGsTex0 *)&phdr.GsTex0)->PSM = psm;
    ((sceGsTex0 *)&phdr.GsTex0)->TW = Tim2GetLog2(nWidth);
    ((sceGsTex0 *)&phdr.GsTex0)->TH = Tim2GetLog2(nHeight);
    phdr.GsTex1 = 0;
    phdr.GsTexaFbaPabe = 0;
    phdr.GsTexClut = 0;

    sceWrite(h, &phdr, sizeof(TIM2_PICTUREHEADER));

    for (i = 0; i < nHeight; i++)
    {
        if (d1)
        {
            if ((i & 1) == 0)
            {
                sceGsSetDefStoreImage(&si, (d0->dispfb.FBP*32), d0->dispfb.FBW, psm, d0->dispfb.DBX, (d0->dispfb.DBY + (i/2)), nWidth, 1);
            }
            else
            {
                sceGsSetDefStoreImage(&si, (d1->dispfb.FBP*32), d1->dispfb.FBW, psm, d1->dispfb.DBX, (d1->dispfb.DBY + (i/2)), nWidth, 1);
            }
        }
        else
        {
            sceGsSetDefStoreImage(&si, (d0->dispfb.FBP*32), d0->dispfb.FBW, psm, d0->dispfb.DBX, (d0->dispfb.DBY + i), nWidth, 1);
        }

        FlushCache(WRITEBACK_DCACHE);

        sceGsExecStoreImage(&si, (u_long128 *)buf);
        sceGsSyncPath(0, 0);

        sceWrite(h, buf, nBytes);

    }

    sceClose(h);

    return 1;
}

void Tim2LoadTexture(int psm, u_int tbp, int tbw, int w, int h, u_long128 *pImage)
{
    sceGsLoadImage li;
    int i;
    int l;
    int n;
    u_long128 *p;

    switch (psm)
    {
    case SCE_GS_PSMZ32:
    case SCE_GS_PSMCT32:
        ResetPIXELAlpha((u_char *)pImage, w * h);
        n = w * 4;
    break;
    case SCE_GS_PSMZ24:
    case SCE_GS_PSMCT24:
        n = w * 3;
    break;
    case SCE_GS_PSMZ16:
    case SCE_GS_PSMZ16S:
    case SCE_GS_PSMCT16:
    case SCE_GS_PSMCT16S:
        n = w * 2;
    break;
    case SCE_GS_PSMT8H:
    case SCE_GS_PSMT8:
        n = w;
    break;
    case SCE_GS_PSMT4HL:
    case SCE_GS_PSMT4HH:
    case SCE_GS_PSMT4:
        n = w / 2;
    break;
    default:
    return;
    }

    l = 32764 * 16 / n;

    for (i = 0; i < h; i += l)
    {
        p = (u_long128 *)((char *)pImage + n*i);

        if (i + l > h)
        {
            l = h - i;
        }

        sceGsSetDefLoadImage(&li, tbp, tbw, psm, 0, i, w, l);

        FlushCache(WRITEBACK_DCACHE);

        sceGsExecLoadImage(&li, p);
        sceGsSyncPath(0, 0);
    }
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

static int Tim2CalcBufSize(int psm, int w, int h)
{
    return (w * h) / 64;
}

static int Tim2GetLog2(int n)
{
    int i;

    for (i = 31; i > 0; i--)
    {
        if (n & (1 << i))
        {
            break;
        }
    }

    if (n > (1 << i))
    {
        i++;
    }

    return i;
}

u_long LoadTIM2Sub(SPRITE_DATA *sno, char *buf, int no, u_int offset)
{
    TIM2_PICTUREHEADER *ph;

    ph = Tim2GetPictureHeader(buf, no);

    if (ph == NULL)
    {
        return 0;
    }

    next_tex_addr = Tim2LoadPicture2(ph, -1, -1, offset);
    next_clut_addr += (ph->ClutSize & 0xff) ? 1 + (ph->ClutSize / 256) : (ph->ClutSize / 256);

    *(u_long *)&sno->g_GsTex0 = ph->GsTex0;

    sno->g_GsTex0.TBP0 += offset;
    sno->g_GsTex0.CBP += offset;

    sno->g_nTexSizeW = ph->ImageWidth;
    sno->g_nTexSizeH = ph->ImageHeight;

    sno->g_GsTex0.TW = GetLog2(ph->ImageWidth);
    sno->g_GsTex0.TH = GetLog2(sno->g_nTexSizeH);

    sno->g_bMipmapLv = ph->MipMapTextures - 1;

    if (sno->g_bMipmapLv != 0)
    {
        sno->g_GsMiptbp1 = *(u_long *)&ph[1].TotalSize;
        sno->g_GsMiptbp2 = *(u_long *)&ph[1].ImageSize;
    }
    else
    {
        sno->g_GsMiptbp1 = 0;
        sno->g_GsMiptbp2 = 0;
    }

    sno->pos_x = 0.0f;
    sno->pos_y = 0.0f;
    sno->size_w = sno->g_nTexSizeW;
    sno->size_h = sno->g_nTexSizeH;
    sno->scale_w = 1.0f;
    sno->scale_h = 1.0f;
    sno->clamp_u = 0;
    sno->clamp_v = 0;
    sno->rot_center = 0;
    sno->angle = 0.0f;
    sno->r = 0x80;
    sno->g = 0x80;
    sno->b = 0x80;
    sno->alpha = 0x80;
    sno->mask = 0;

    FlushTextureCache();

    return *(u_long *)&sno->g_GsTex0;
}

u_long GetTex0Reg(u_int addr, int no, u_int offset)
{
    u_long ret;
    TIM2_PICTUREHEADER *ph;
    PK2_HEAD *pk2h;

    ret = 0;

    pk2h = (PK2_HEAD *)addr;

    ph = Tim2GetPictureHeader((char *)addr + pk2h->offset[no], 0);

    if (ph != NULL)
    {
        u_long tex0 = ph->GsTex0;

        ((sceGsTex0 *)&tex0)->TBP0 += offset;
        ((sceGsTex0 *)&tex0)->CBP += offset;

        ret = tex0;
    }

    return ret;
}

u_long GetTex0RegTM(u_int addr, u_int offset)
{
    u_long ret;
    TIM2_PICTUREHEADER *ph;

    ret = 0;

    ph = Tim2GetPictureHeader((char *)addr, 0);

    if (ph != NULL)
    {
        u_long tex0 = ph->GsTex0;

        ((sceGsTex0 *)&tex0)->TBP0 += offset;
        ((sceGsTex0 *)&tex0)->CBP += offset;

        ret = tex0;
    }

    return ret;
}

void InitTIM2Files()
{
    sceDmaReset(1);
    sceGsResetPath();

    DmaGif = sceDmaGetChan(2);
    DmaVif = sceDmaGetChan(1);

    InitTIM2Addr();

    pbuf = (Q_WORDDATA *)PBUF_ADDRESS;
    mpbuf = mpbufw[0];
    mes_swap = 0;
    ndpkt = 0;
    ndpri = 0;
    nmdpkt = 0;
    nmdpri = 0;

    SetG2DTopPkt(PBUF_ADDRESS);
    LoadTIM2File();
}

static void InitTIM2Addr()
{
    long int l;

    if (g_bInterlace != 0)
    {
        l = SCR_WIDTH * SCR_HEIGHT;
    }
    else
    {
        l = SCR_WIDTH * SCR_HEIGHT * 2;
    }

    next_clut_addr = 248 * 64;
    next_tex_addr = (l / 64) * 3;
}

static void FlushTextureCache()
{
    Q_WORDDATA pflsh[8];

    pflsh[0].ul128 = (u_long128)0;
    pflsh[0].ui32[0] = DMAend | 2;

    pflsh[1].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pflsh[1].ul64[1] = SCE_GIF_PACKED_AD;

    pflsh[2].ul64[0] = 0;
    pflsh[2].ul64[1] = SCE_GS_TEXFLUSH;

    DrawOne2D(pflsh);
}

static int GetLog2(u_int n)
{
    int i;

    for (i = 31; i > 0; i--)
    {
        if (((int)n >> i) & 1)
        {
            break;
        }
    }

    return i + ((1 << i) < n);
}

void SortEffectPacket()
{
    int i;
    int j;
    int n;

    if (ndpri >= 0x1000 || ndpkt >= 0x2000)
    {
        ndpri = 0;
        ndpkt = 0;

        return;
    }

    for (j = 0; j < ndpri - 1; j++)
    {
        for (i = j + 1; i < ndpri; i++)
        {
            if (draw_pri[j][0] < draw_pri[i][0])
            {
                n = draw_pri[i][0];
                draw_pri[i][0] = draw_pri[j][0];
                draw_pri[j][0] = n;

                n = draw_pri[i][1];
                draw_pri[i][1] = draw_pri[j][1];
                draw_pri[j][1] = n;
            }
        }
    }
}

void SortMessagePacket()
{
    int i;
    int j;
    int n;

    if (nmdpri >= 0x400 || nmdpkt >= 0x2000)
    {
        nmdpri = 0;
        nmdpkt = 0;

        return;
    }

    for (j = 0; j < nmdpri - 1; j++)
    {
        for (i = j + 1; i < nmdpri; i++)
        {
            if (draw_mpri[j][0] < draw_mpri[i][0])
            {
                n = draw_mpri[i][0];
                draw_mpri[i][0] = draw_mpri[j][0];
                draw_mpri[j][0] = n;

                n = draw_mpri[i][1];
                draw_mpri[i][1] = draw_mpri[j][1];
                draw_mpri[j][1] = n;
            }
        }
    }
}

void DmaSync_Vif1()
{
    DmaVif = sceDmaGetChan(1);
    sceDmaSync(DmaVif, 0, 0);
}

void DrawOne2D_P2(Q_WORDDATA *packet_buf)
{
    int s;

    s = packet_buf->us16[0];

    if (s != 0)
    {
        packet_buf->ui32[2] = 0;
        packet_buf->ui32[3] = DMAcall | s;
    }


    DmaVif = sceDmaGetChan(1);
    DmaVif->chcr.TTE = 1;

    FlushCache(0);

    sceDmaSend(DmaVif, (void *)((u_int)packet_buf & 0x0fffffff));
    sceDmaSync(DmaVif, 0, 0);
    sceGsSyncPath(0, 0);
}

void DrawAll2D_P2()
{
    int i;
    int m;
    int n;
    int s;

    if (0 < ndpri)
    {
        for (i = 0; i < ndpri - 1; i++)
        {
            n = draw_pri[i][1];
            m = draw_pri[i+1][1];

            s = pbuf[n].us16[0];

            if (pbuf[n].uc8[3] == 0x70) // upper part of 0x70000000 (DMAend) ??
            {
                pbuf[n].uc8[3] = 0x20; // upper part of 0x20000000 (DMAnext) ??
                pbuf[n].ui32[1] = (u_int)&pbuf[m] & 0x0fffffff;
                pbuf[n].ui32[2] = 0;

                if (s != 0)
                {
                    pbuf[n].ui32[3] = s | DMAcall;
                }
            }
        }

        n = draw_pri[ndpri-1][1];
        m = ndpkt;

        s = pbuf[n].us16[0];

        if (pbuf[n].uc8[3] == 0x70) // upper part of 0x70000000 (DMAend) ??
        {
            pbuf[n].uc8[3] = 0x20; // upper part of 0x20000000 (DMAnext) ??
            pbuf[n].ui32[1] = (u_int)&pbuf[m] & 0x0fffffff;
            pbuf[n].ui32[2] = 0;

            if (s != 0)
            {
                pbuf[n].ui32[3] = s | DMAcall;
            }
        }

        pbuf[ndpkt].ui32[0] = DMAend;
        pbuf[ndpkt].ui32[1] = 0;
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt].ui32[3] = 0;

        DrawOne2D_P2(pbuf + draw_pri[0][1]);
    }

    ndpri = 0;
    ndpkt = 0;
}

void* DrawAllMes_P2(u_int ret_addr)
{
    int i;
    int m;
    int n;
    int s;
    void *ret;

    SortMessagePacket();

    if (nmdpri >= 1)
    {
        for (i = 0; i < nmdpri - 1; i++)
        {
                n = draw_mpri[i][1];
                m = draw_mpri[i+1][1];

                s = mpbuf[n].us16[0];

                mpbuf[n].uc8[3] = 0x20;
                mpbuf[n].ui32[1] = (u_int)&mpbuf[m] & 0x0fffffff;
                mpbuf[n].ui32[2] = 0;
                mpbuf[n].ui32[3] = s | DMAcall;
        }

        n = draw_mpri[nmdpri-1][1];

        s = mpbuf[n].us16[0];

        if (ret_addr != 0)
        {
            mpbuf[n].uc8[3] = 0x20;
            mpbuf[n].ui32[1] = ret_addr & 0xfffffff;
        }
        else
        {
            mpbuf[n].uc8[3] = 0x70;
            mpbuf[n].ui32[1] = 0;
        }

        mpbuf[n].ui32[3] = s | DMAcall;
        mpbuf[n].ui32[2] = 0;

        ret = (void *)((u_int)&mpbuf[draw_mpri[0][1]] & 0x0fffffff);
    }
    else
    {
        ret = NULL;
    }

    nmdpri = 0;
    nmdpkt = 0;

    return ret;
}

void DrawOne2D(Q_WORDDATA *packet_buf)
{
    DrawOne2D_P2(packet_buf);
}

void DrawOne2D2a(Q_WORDDATA *packet_buf)
{
    FlushCache(0);

    sceDmaSend(DmaGif, packet_buf);
}

void DrawOne2D2b()
{
    sceDmaSync(DmaGif, 0, 0);
}

void DrawAll2D()
{
    sceDmaSync(DmaVif, 0, 0);

    DrawAll2D_P2();
}

void DrawAllMes()
{
    DrawOne2D_P2((Q_WORDDATA *)DrawAllMes_P2(0));
}

void LoadSprFileToMainD(int no, int addr)
{
    return;
}

void SetSprFile(u_int addr)
{
    SetSprFile2(addr, 0);
}

void SetSprFile2(u_int addr, u_int offset)
{
    MakeTim2ClutDirect3(addr, -1, -1, offset);
}

void SetSprFile3(u_int addr, u_int offset)
{
    MakeTim2ClutDirect3(addr, -1, -1, offset);
}

void SetETIM2File(int addr)
{
    int i;
    int texnum;
    int *offtop;
    SPRITE_DATA dummy;

    texnum = ((int *)addr)[0];
    offtop = &((int *)addr)[4];

    for (i = 0; i < texnum; i++)
    {
        LoadTIM2Sub(&dummy, (char *)(addr + offtop[i]), 0, 0);
    }
}

void SetFTIM2File(int addr)
{
    CallFontTexSendPacket();
}

void LoadTIM2File()
{
    MakeFontTexSendPacket();
}

void SetScissor(int pri, int x, int y, int w, int h)
{
    int div;

    div = g_bInterlace ? 2 : 1;

    Reserve2DPacket(pri);

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = DMAend | 2;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_SCISSOR_1(x, x + w, y / div, (y + h) / div);
    pbuf[ndpkt++].ul64[1] = SCE_GS_SCISSOR_1;
}

void ResetScissor(int pri)
{
    Reserve2DPacket(pri);

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = DMAend | 2;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_SCISSOR_1(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);
    pbuf[ndpkt++].ul64[1] = SCE_GS_SCISSOR_1;
}

void CopySprDToSpr(DISP_SPRT *s, SPRT_DAT *d)
{
    s->att = 0;
    s->u = d->u;
    s->v = d->v;
    s->w = d->w;
    s->h = d->h;
    s->x = d->x;
    s->y = d->y;
    s->crx = 0.0f;
    s->cry = 0.0f;
    s->csx = 0.0f;
    s->csy = 0.0f;
    s->z = 0x0fffffff - d->pri;
    s->scw = 1.0f;
    s->sch = 1.0f;
    s->rot = 0.0f;
    s->tex0 = d->tex0;
    s->tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_NEAREST, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);
    s->texa = SCE_GS_SET_TEXA(0, 1, 128);
    s->alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x80);
    s->zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
    s->test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    s->gftg = 0 \
        | SCE_GS_XYZF2 << (4 * 0)
        | SCE_GS_XYZ2  << (4 * 1)
        | SCE_GS_RGBAQ << (4 * 2);
    s->pri = d->pri;
    s->r = 0x80;
    s->g = 0x80;
    s->b = 0x80;
    s->alpha = d->alpha;
    s->col = 0;
}

void _ftoi0(int *out, float *in)
{
    asm volatile ("\n\
        lqc2    $vf12,0(%0) \n\
        vftoi0.xyzw $vf12xyzw,$vf12xyzw \n\
        sqc2    $vf12,0(%1) \n\
    ": :"r"(in),"r"(out));
}

void _ftoi4(int *out, float *in)
{
    asm volatile ("\n\
        lqc2    $vf12,0(%0) \n\
        vftoi4.xyzw $vf12xyzw,$vf12xyzw \n\
        sqc2    $vf12,0(%1) \n\
    ": :"r"(in),"r"(out));
}

void DispSprD(DISP_SPRT *s)
{
    u_int ui;
    int i;
    int psm;
    float ss;
    float cc;
    u_int matt;
    u_int mu;
    u_int mv;
    float mw;
    float mh;
    float mx;
    float my;
    float mcrx;
    float mcry;
    float mcsx;
    float mcsy;
    u_int mz;
    float msw;
    float msh;
    float mrot;
    u_long mtex0;
    u_long mtex1;
    u_long mtexa;
    u_long malpr;
    u_long mzbuf;
    u_long mtest;
    u_long mgftg;
    u_int mpri;
    u_char mr;
    u_char mg;
    u_char mb;
    u_char ma;
    u_char mlud;
    float x[4];
    float y[4];
    float x2[4];
    float y2[4];
    u_int xx[4];
    u_int yy[4];
    u_int uu[4];
    u_int vv[4];
    sceVu0IVECTOR itmp;
    sceVu0FVECTOR ftmp;

    matt = s->att;
    mu = s->u;
    mv = s->v;
    mw = s->w;
    mh = s->h;
    mx = s->x - 320.0f;
    my = s->y - 224.0f;
    mcrx = s->crx - 320.0f;
    mcry = s->cry - 224.0f;
    mcsx = s->csx - 320.0f;
    mcsy = s->csy - 224.0f;
    mz = s->z;
    msw = s->scw;
    msh = s->sch;
    mrot = (s->rot * PI) / 180.0f;
    mtex0 = s->tex0;
    mtex1 = s->tex1;
    mtexa = s->texa;
    malpr = s->alphar;
    mzbuf = s->zbuf;
    mtest = s->test;
    mgftg = s->gftg;
    mpri = s->pri;

    mr = s->r;
    mg = s->g;
    mb = s->b;
    ma = s->alpha;
    mlud = s->col;

    psm = ((sceGsTex0 *)&s->tex0)->PSM;

    if (msw != 1.0f)
    {
        x[0] = x[2] = (mx - mcsx) * msw + mcsx;
        x[1] = x[3] = ((mx + mw) - mcsx) * msw + mcsx;
    }
    else
    {
        x[0] = x[2] = mx;
        x[1] = x[3] = mx + mw;
    }

    if (msh != 1.0f)
    {
        y[0] = y[1] = (my - mcsy) * msh + mcsy;
        y[2] = y[3] = ((my + mh) - mcsy) * msh + mcsy;
    }
    else
    {
        y[0] = y[1] = my;
        y[2] = y[3] = my + mh;
    }

    if (!msw)
    {
        // HACK to alter/fix stack allocation order
    }

    for (i = 0; i < 4; i++)
    {
        x[i] -= mcrx;
        y[i] -= mcry;
    }

    if (mrot != 0.0f)
    {
        ss = SgSinf(mrot);
        cc = SgCosf(mrot);
        x2[0] = x[0] * cc - y[0] * ss;
        y2[0] = x[0] * ss + y[0] * cc;
        x2[1] = x[1] * cc - y[1] * ss;
        y2[1] = x[1] * ss + y[1] * cc;
        x2[2] = x[2] * cc - y[2] * ss;
        y2[2] = x[2] * ss + y[2] * cc;
        x2[3] = x[3] * cc - y[3] * ss;
        y2[3] = x[3] * ss + y[3] * cc;
    }
    else
    {
        x2[0] = x[0];
        y2[0] = y[0];
        x2[1] = x[1];
        y2[1] = y[1];
        x2[2] = x[2];
        y2[2] = y[2];
        x2[3] = x[3];
        y2[3] = y[3];
    }

    for (i = 0; i < 4; i++)
    {
        x2[i] += mcrx;
        y2[i] += mcry;
    }

    for (i = 0; i < 4; i++)
    {
        ftmp[0] = x2[i] + 2048.0f;
        ftmp[1] = y2[i] * 0.5f + 2048.0f;

        _ftoi4(itmp,ftmp);

        xx[i] = itmp[0];
        yy[i] = itmp[1];
    }

    ftmp[0] = mw;
    ftmp[1] = mh;

    _ftoi0(itmp, ftmp);

    uu[0] = uu[2] = mu * 16;
    uu[1] = uu[3] = (itmp[0] + mu) * 16;

    vv[0] = vv[1] = mv * 16;
    vv[2] = vv[3] = (itmp[1] + mv) * 16;

    if (matt & 0x2)
    {
        ui = uu[0];
        uu[0] = uu[1];
        uu[2] = uu[1];
        uu[1] = ui;
        uu[3] = ui;
    }

    if (matt & 0x1)
    {
        ui = vv[0];
        vv[0] = vv[2];
        vv[2] = ui;
        ui = vv[1];
        vv[1] = vv[3];
        vv[3] = ui;
    }

    FlushCache(0);

    Reserve2DPacket(mpri);

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = DMAend | 24;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(10, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = 0;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXFLUSH;

    if (psm == 20)
    {
        sceGsTex0 tex0;

        tex0 = *(sceGsTex0 *)&mtex0;
        tex0.PSM = SCE_GS_PSMT8;
        tex0.CSA = 0;
        tex0.CLD = 1;
        pbuf[ndpkt].ul64[0] = *(u_long *)&tex0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

        tex0 = *(sceGsTex0 *)&mtex0;
        tex0.CSA = 0;
        tex0.CLD = 0;
        pbuf[ndpkt].ul64[0] = *(u_long *)&tex0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;
    }
    else
    {
        pbuf[ndpkt].ul64[0] = mtex0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_TEX0_1;

        pbuf[ndpkt].ul64[0] = 0;
        pbuf[ndpkt++].ul64[1] = SCE_GS_NOP;
    }

    pbuf[ndpkt].ul64[0] = mtex1;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEX1_1;

    pbuf[ndpkt].ul64[0] = mtexa;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXA;

    pbuf[ndpkt].ul64[0] = mtest;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    pbuf[ndpkt].ul64[0] = mzbuf;
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = malpr;
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_RGBAQ(0x5, 0, 0, 0, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_RGBAQ;

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_TEXCLUT(4, 0, mlud);
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXCLUT;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, mgftg, SCE_GIF_PACKED, 12);
    pbuf[ndpkt++].ul64[1] = 0 \
        | (long)SCE_GS_UV    << (4 *  0)
        | (long)SCE_GS_RGBAQ << (4 *  1)
        | (long)SCE_GS_XYZF2 << (4 *  2)
        | (long)SCE_GS_UV    << (4 *  3)
        | (long)SCE_GS_RGBAQ << (4 *  4)
        | (long)SCE_GS_XYZF2 << (4 *  5)
        | (long)SCE_GS_UV    << (4 *  6)
        | (long)SCE_GS_RGBAQ << (4 *  7)
        | (long)SCE_GS_XYZF2 << (4 *  8)
        | (long)SCE_GS_UV    << (4 *  9)
        | (long)SCE_GS_RGBAQ << (4 * 10)
        | (long)SCE_GS_XYZF2 << (4 * 11);

    for (i = 0; i < 4; i++)
    {
        pbuf[ndpkt].ui32[0] = uu[i];
        pbuf[ndpkt].ui32[1] = vv[i];
        pbuf[ndpkt].ui32[2] = 0;
        pbuf[ndpkt++].ui32[3] = 0;

        pbuf[ndpkt].ui32[0] = mr;
        pbuf[ndpkt].ui32[1] = mg;
        pbuf[ndpkt].ui32[2] = mb;
        pbuf[ndpkt++].ui32[3] = ma;

        pbuf[ndpkt].ui32[0] = xx[i];
        pbuf[ndpkt].ui32[1] = yy[i];
        pbuf[ndpkt].ui32[2] = mz;
        pbuf[ndpkt++].ui32[3] = 0;
    }
}

void CopySqrDToSqr(DISP_SQAR *s, SQAR_DAT *d)
{
    int i;

    s->att = 0;
    s->crx = 0.0f;
    s->cry = 0.0f;
    s->csx = 0.0f;
    s->csy = 0.0f;
    s->x[0] = s->x[2] = d->x;
    s->x[1] = s->x[3] = d->x + d->w;
    s->y[0] = s->y[1] = d->y;
    s->y[2] = s->y[3] = d->y + d->h;
    s->z = 0xfffffff - d->pri;
    s->scw = 1.0f;
    s->sch = 1.0f;
    s->rot = 0.0f;
    s->texa = SCE_GS_SET_TEXA(0, 1, 128);
    s->alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x80);
    s->zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
    s->test = SCE_GS_SET_ZBUF_1(0x5000d, SCE_GS_PSMCT32, 0);
    s->pri = d->pri;

    for (i = 0; i < 4; i++)
    {
        s->r[i] = d->r;
        s->g[i] = d->g;
        s->b[i] = d->b;
    }

    s->alpha = d->alpha;
}

void CopyGSqDToSqr(DISP_SQAR *s, GSQR_DAT *d)
{
    int i;

    s->att = 0;
    s->crx = 0.0f;
    s->cry = 0.0f;
    s->csx = 0.0f;
    s->csy = 0.0f;
    s->x[0] = s->x[2] = d->x;
    s->x[1] = s->x[3] = d->x + d->w;
    s->y[0] = s->y[1] = d->y;
    s->y[2] = s->y[3] = d->y + d->h;
    s->z = 0xfffffff - d->pri;
    s->scw = 1.0f;
    s->sch = 1.0f;
    s->rot = 0.0f;
    s->texa = SCE_GS_SET_TEXA(0, 1, 128);
    s->alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x80);
    s->zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
    s->test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    s->pri = d->pri;

    for (i = 0; i < 4; i++)
    {
        s->r[i] = d->r[i];
        s->g[i] = d->g[i];
        s->b[i] = d->b[i];
    }

    s->alpha = d->alpha;
}

void CopySq4DToSqr(DISP_SQAR *s, SQR4_DAT *d)
{
    int i;

    s->att = 0;
    s->crx = 0.0f;
    s->cry = 0.0f;
    s->csx = 0.0f;
    s->csy = 0.0f;

    for (i = 0; i < 4; i++)
    {
        s->x[i] = d->x[i];
        s->y[i] = d->y[i];
    }

    s->z = 0xfffffff - d->pri;
    s->scw = 1.0f;
    s->sch = 1.0f;
    s->rot = 0.0f;
    s->texa = SCE_GS_SET_TEXA(0, 1, 128);
    s->alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x80);
    s->zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
    s->test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    s->pri = d->pri;

    for (i = 0; i < 4; i++)
    {
        s->r[i] = d->r;
        s->g[i] = d->g;
        s->b[i] = d->b;
    }

    s->alpha = d->alpha;
}

void CopyGS4DToSqr(DISP_SQAR *s, GSQ4_DAT *d)
{
    int i;

    s->att = 0;
    s->crx = 0.0f;
    s->cry = 0.0f;
    s->csx = 0.0f;
    s->csy = 0.0f;

    for (i = 0; i < 4; i++)
    {
        s->x[i] = d->x[i];
        s->y[i] = d->y[i];
    }

    s->z = 0xfffffff - d->pri;
    s->scw = 1.0f;
    s->sch = 1.0f;
    s->rot = 0.0f;
    s->texa = SCE_GS_SET_TEXA(0, 1, 128);
    s->alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x80);
    s->zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 0);
    s->test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GREATER, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);
    s->pri = d->pri;

    for (i = 0; i < 4; i++)
    {
        s->r[i] = d->r[i];
        s->g[i] = d->g[i];
        s->b[i] = d->b[i];
    }

    s->alpha = d->alpha;
}

void DispSqrD(DISP_SQAR *s)
{
    int i;
    float ss;
    float cc;
    float mx[4];
    float my[4];
    float mcrx;
    float mcry;
    float mcsx;
    float mcsy;
    u_int mz;
    u_char ma;
    float msw;
    float msh;
    float mrot;
    u_long mtexa;
    u_long malpr;
    u_long mzbuf;
    u_long mtest;
    u_int mpri;
    u_char mr[4];
    u_char mg[4];
    u_char mb[4];
    float x[4];
    float y[4];
    float x2[4];
    float y2[4];
    u_int xx[4];
    u_int yy[4];
    sceVu0FVECTOR ftmp;
    sceVu0IVECTOR itmp;

    mcrx = s->crx - 320.0f;
    mcry = s->cry - 224.0f;
    mcsx = s->csx - 320.0f;
    mcsy = s->csy - 224.0f;

    for (i = 0; i < 4; i++)
    {
        mx[i] = s->x[i] - 320.0f;
        my[i] = s->y[i] - 224.0f;
    }

    mz = s->z; /* 0xd8(sp) */
    msw = s->scw; /* f5 */
    msh = s->sch; /* f6 */
    mrot = s->rot;  /* f24 */
    mtexa = s->texa; /* 0xe8(sp) */
    malpr = s->alphar; /* 0xf0(sp) */
    mzbuf = s->zbuf; /* 0xf8(sp) */
    mtest = s->test; /* 0x100(sp) */
    mpri = s->pri;  /* 0x10c(sp) */
    ma = s->alpha;  /* 0x78(sp) */

    for (i = 0; i < 4; i++)
    {
        mr[i] = s->r[i];
        mg[i] = s->g[i];
        mb[i] = s->b[i];
    }

    if (msw != 1.0f)
    {
        for (i = 0; i < 4; i++)
        {
            x[i] = (mx[i] - mcsx) * msw + mcsx;
        }
    }
    else
    {
        for (i = 0; i < 4; i++)
        {
            x[i] = mx[i];
        }
    }

    if (msh != 1.0f)
    {
        for (i = 0; i < 4; i++)
        {
            y[i] = (my[i] - mcsy) * msh + mcsy;
        }
    }
    else
    {
        for (i = 0; i < 4; i++)
        {
            y[i] = my[i];
        }
    }

    ss = SgSinf((mrot * PI) / 180.0f);
    cc = SgCosf((mrot * PI) / 180.0f);

    for (i = 0; i < 4; i++)
    {
        x[i] -= mcrx;
        y[i] -= mcry;
    }

    if (mrot != 0.0f)
    {
        x2[0] = x[0] * cc - y[0] * ss;
        y2[0] = x[0] * ss + y[0] * cc;
        x2[1] = x[1] * cc - y[1] * ss;
        y2[1] = x[1] * ss + y[1] * cc;
        x2[2] = x[2] * cc - y[2] * ss;
        y2[2] = x[2] * ss + y[2] * cc;
        x2[3] = x[3] * cc - y[3] * ss;
        y2[3] = x[3] * ss + y[3] * cc;
    }
    else
    {
        x2[0] = x[0];
        y2[0] = y[0];
        x2[1] = x[1];
        y2[1] = y[1];
        x2[2] = x[2];
        y2[2] = y[2];
        x2[3] = x[3];
        y2[3] = y[3];
    }

    for (i = 0; i < 4; i++)
    {
        x2[i] += mcrx;
        y2[i] += mcry;
    }

    for (i = 0; i < 4; i++)
    {
        ftmp[0] = x2[i] + 2048.0f;
        ftmp[1] = y2[i] * 0.5f + 2048.0f;
        _ftoi4(itmp, ftmp);
        xx[i] = itmp[0];
        yy[i] = itmp[1];
    }

    Reserve2DPacket(mpri);

    pbuf[ndpkt].ul128 = (u_long128)0;
    pbuf[ndpkt++].ui32[0] = DMAend | 14;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(4, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
    pbuf[ndpkt++].ul64[1] = SCE_GIF_PACKED_AD;

    pbuf[ndpkt].ul64[0] = mtexa;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEXA;

    pbuf[ndpkt].ul64[0] = mtest;
    pbuf[ndpkt++].ul64[1] = SCE_GS_TEST_1;

    pbuf[ndpkt].ul64[0] = mzbuf;
    pbuf[ndpkt++].ul64[1] = SCE_GS_ZBUF_1;

    pbuf[ndpkt].ul64[0] = malpr;
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 1, 0, 0, 1, 0, 0, 0, 0), SCE_GIF_PACKED, 8);
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0)
        | SCE_GS_XYZF2 << (4 * 1)
        | SCE_GS_RGBAQ << (4 * 2)
        | SCE_GS_XYZF2 << (4 * 3)
        | SCE_GS_RGBAQ << (4 * 4)
        | SCE_GS_XYZF2 << (4 * 5)
        | SCE_GS_RGBAQ << (4 * 6)
        | SCE_GS_XYZF2 << (4 * 7) ;

    for (i = 0; i < 4; i++)
    {
        pbuf[ndpkt].ui32[0] = mr[i];
        pbuf[ndpkt].ui32[1] = mg[i];
        pbuf[ndpkt].ui32[2] = mb[i];
        pbuf[ndpkt++].ui32[3] = ma;

        pbuf[ndpkt].ui32[0] = xx[i];
        pbuf[ndpkt].ui32[1] = yy[i];
        pbuf[ndpkt].ui32[2] = mz;
        pbuf[ndpkt++].ui32[3] = 0;
    }
}
