#ifndef GRAPHICS_GRAPH2D_TIM2_H
#define GRAPHICS_GRAPH2D_TIM2_H

#include "typedefs.h"

#include "sce/libgraph.h"

typedef struct {
	u_int att;
	u_int u;
	u_int v;
	u_int w;
	u_int h;
	float crx;
	float cry;
	float csx;
	float csy;
	float x;
	float y;
	u_int z;
	float scw;
	float sch;
	float rot;
	u_long gftg;
	u_long tex0;
	u_long tex1;
	u_long texa;
	u_long alphar;
	u_long zbuf;
	u_long test;
	u_int pri;
	u_char r;
	u_char g;
	u_char b;
	u_char alpha;
	u_char col;
} DISP_SPRT;

typedef struct {
	u_int att;
	float crx;
	float cry;
	float csx;
	float csy;
	int x[4];
	int y[4];
	u_int z;
	float scw;
	float sch;
	float rot;
	u_long texa;
	u_long alphar;
	u_long zbuf;
	u_long test;
	u_int pri;
	u_char r[4];
	u_char g[4];
	u_char b[4];
	u_char alpha;
} DISP_SQAR;

typedef struct {
	u_int w;
	u_int h;
	int x;
	int y;
	u_int pri;
	u_char r;
	u_char g;
	u_char b;
	u_char alpha;
} SQAR_DAT;

typedef struct {
	u_int w;
	u_int h;
	int x;
	int y;
	u_int pri;
	u_char r[4];
	u_char g[4];
	u_char b[4];
	u_char alpha;
} GSQR_DAT;

typedef struct {
	int x[4];
	int y[4];
	u_int pri;
	u_char r;
	u_char g;
	u_char b;
	u_char alpha;
} SQR4_DAT;

typedef struct {
	int x[4];
	int y[4];
	u_int pri;
	u_char r[4];
	u_char g[4];
	u_char b[4];
	u_char alpha;
} GSQ4_DAT;

typedef union {
	u_long128 ul128;
	u_long ul64[2];
	u_int ui32[4];
	float fl32[4];
	u_short us16[8];
	u_char uc8[16];
	sceVu0FVECTOR fv;
	sceVu0IVECTOR iv;
} Q_WORDDATA;

typedef unsigned char TIM2_UCHAR8;
typedef u_int TIM2_UINT32;
typedef short unsigned int TIM2_UINT16;
typedef long unsigned int TIM2_UINT64;

typedef struct {
	TIM2_UINT32 TotalSize;
	TIM2_UINT32 ClutSize;
	TIM2_UINT32 ImageSize;
	TIM2_UINT16 HeaderSize;
	TIM2_UINT16 ClutColors;
	TIM2_UCHAR8 PictFormat;
	TIM2_UCHAR8 MipMapTextures;
	TIM2_UCHAR8 ClutType;
	TIM2_UCHAR8 ImageType;
	TIM2_UINT16 ImageWidth;
	TIM2_UINT16 ImageHeight;
	TIM2_UINT64 GsTex0;
	TIM2_UINT64 GsTex1;
	TIM2_UINT32 GsTexaFbaPabe;
	TIM2_UINT32 GsTexClut;
} TIM2_PICTUREHEADER;

typedef struct {
	TIM2_UINT64 GsMiptbp1;
	TIM2_UINT64 GsMiptbp2;
	TIM2_UINT32 MMImageSize[0];
} TIM2_MIPMAPHEADER;

#include "graphics/graph2d/effect_sub.h"
#include "outgame/mode_slct.h"

extern u_char g_bInterlace;
extern int ndpkt;
// extern int ndpri;
// extern int nmdpkt;
// extern int nmdpri;
// extern int mes_swap;
extern Q_WORDDATA *pbuf;
// extern u_int draw_pri[4096][2];
// extern Q_WORDDATA *mpbuf;
// extern Q_WORDDATA mpbufw[2][8192];
// extern int draw_mpri[1024][2];

void printClut(void *pClut, int ClutColors);
void ResetClutAlpha(void *pClut, int ClutColors);
void ResetPIXELAlpha(u_char *ip, int size);
int Tim2CheckFileHeaer(void *pTim2);
TIM2_PICTUREHEADER* Tim2GetPictureHeader(void *pTim2, int imgno);
int Tim2IsClut2(TIM2_PICTUREHEADER *ph);
int Tim2GetMipMapPictureSize(TIM2_PICTUREHEADER *ph, int mipmap, int *pWidth, int *pHeight);
TIM2_MIPMAPHEADER* Tim2GetMipMapHeader(TIM2_PICTUREHEADER *ph, int *pSize);
void* Tim2GetUserSpace(TIM2_PICTUREHEADER *ph, int *pSize);
void* Tim2GetUserData(TIM2_PICTUREHEADER *ph, int *pSize);
char* Tim2GetComment(TIM2_PICTUREHEADER *ph);
void* Tim2GetImage(TIM2_PICTUREHEADER *ph, int mipmap);
void* Tim2GetClut(TIM2_PICTUREHEADER *ph);
u_int Tim2GetClutColor(TIM2_PICTUREHEADER *ph, int clut, int no);
u_int Tim2SetClutColor(TIM2_PICTUREHEADER *ph, int clut, int no, u_int newcolor);
u_int Tim2GetTexel(TIM2_PICTUREHEADER *ph, int mipmap, int x, int y);
u_int Tim2SetTexel(TIM2_PICTUREHEADER *ph, int mipmap, int x, int y, u_int newtexel);
u_int Tim2GetTextureColor(TIM2_PICTUREHEADER *ph, int mipmap, int clut, int x, int y);
u_int Tim2LoadPicture2(TIM2_PICTUREHEADER *ph, u_int tbp, u_int cbp, u_int offset);
u_int Tim2LoadPicture(TIM2_PICTUREHEADER *ph, u_int tbp, u_int cbp);
u_int Tim2LoadImage2(TIM2_PICTUREHEADER *ph, u_int tbp, u_int offset);
u_int Tim2LoadImage(TIM2_PICTUREHEADER *ph, u_int tbp);
u_int Tim2LoadClut2(TIM2_PICTUREHEADER *ph, u_int cbp, u_int offset);
u_int Tim2LoadClut(TIM2_PICTUREHEADER *ph, u_int cbp);
int Tim2TakeSnapshot(sceGsDispEnv *d0, sceGsDispEnv *d1, char *pszFname);
void Tim2LoadTexture(int psm, u_int tbp, int tbw, int w, int h, u_long128 *pImage);
u_long LoadTIM2Sub(SPRITE_DATA *sno, char *buf, int no, u_int offset);
u_long GetTex0Reg(u_int addr, int no, u_int offset);
u_long GetTex0RegTM(u_int addr, u_int offset);
void InitTIM2Files();
void SortEffectPacket();
void SortMessagePacket();
void DmaSync_Vif1();
void DrawOne2D_P2(Q_WORDDATA *packet_buf);
void DrawAll2D_P2();
void* DrawAllMes_P2(u_int ret_addr);
void DrawOne2D();
void DrawOne2D2a();
void DrawOne2D2b();
void DrawAll2D();
void DrawAllMes();
void LoadSprFileToMainD(int no, int addr);
void SetSprFile(u_int addr);
void SetSprFile2(u_int addr, u_int offset);
void SetSprFile3(u_int addr, u_int offset);
void SetETIM2File(int addr);
void SetFTIM2File(int addr);
void LoadTIM2File();
void SetScissor(int pri, int x, int y, int w, int h);
void ResetScissor(int pri);
void CopySprDToSpr(DISP_SPRT *s, SPRT_DAT *d);
void _ftoi0(int *out, float *in);
void _ftoi4(int *out, float *in);
void DispSprD(DISP_SPRT *s);
void CopySqrDToSqr(DISP_SQAR *s, SQAR_DAT *d);
void CopyGSqDToSqr(GSQR_DAT *d);
void CopySq4DToSqr(SQR4_DAT *d);
void CopyGS4DToSqr(GSQ4_DAT *d);
void DispSqrD(DISP_SQAR *s);

#endif // GRAPHICS_GRAPH2D_TIM2_H
