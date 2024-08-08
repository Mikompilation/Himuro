#ifndef SCE_LIBGRAPH_H
#define SCE_LIBGRAPH_H

#include "ee/eestruct.h"
#include "ee/eeregs.h"

typedef struct {
    long unsigned int NLOOP : 15;
    long unsigned int EOP : 1;
    long unsigned int pad16 : 16;
    long unsigned int id : 14;
    long unsigned int PRE : 1;
    long unsigned int PRIM : 11;
    long unsigned int FLG : 2;
    long unsigned int NREG : 4;
    long unsigned int REGS0 : 4;
    long unsigned int REGS1 : 4;
    long unsigned int REGS2 : 4;
    long unsigned int REGS3 : 4;
    long unsigned int REGS4 : 4;
    long unsigned int REGS5 : 4;
    long unsigned int REGS6 : 4;
    long unsigned int REGS7 : 4;
    long unsigned int REGS8 : 4;
    long unsigned int REGS9 : 4;
    long unsigned int REGS10 : 4;
    long unsigned int REGS11 : 4;
    long unsigned int REGS12 : 4;
    long unsigned int REGS13 : 4;
    long unsigned int REGS14 : 4;
    long unsigned int REGS15 : 4;
} sceGifTag __attribute__((aligned(16)));

typedef struct {
    long unsigned int SBP : 14;
    long unsigned int pad14 : 2;
    long unsigned int SBW : 6;
    long unsigned int pad22 : 2;
    long unsigned int SPSM : 6;
    long unsigned int pad30 : 2;
    long unsigned int DBP : 14;
    long unsigned int pad46 : 2;
    long unsigned int DBW : 6;
    long unsigned int pad54 : 2;
    long unsigned int DPSM : 6;
    long unsigned int pad62 : 2;
} sceGsBitbltbuf;

typedef struct {
    long unsigned int SSAX : 11;
    long unsigned int pad11 : 5;
    long unsigned int SSAY : 11;
    long unsigned int pad27 : 5;
    long unsigned int DSAX : 11;
    long unsigned int pad43 : 5;
    long unsigned int DSAY : 11;
    long unsigned int DIR : 2;
    long unsigned int pad61 : 3;
} sceGsTrxpos;

typedef struct {
    long unsigned int RRW : 12;
    long unsigned int pad12 : 20;
    long unsigned int RRH : 12;
    long unsigned int pad44 : 20;
} sceGsTrxreg;

typedef struct {
    long unsigned int pad00;
} sceGsFinish;

typedef struct {
    long unsigned int XDR : 2;
    long unsigned int pad02 : 62;
} sceGsTrxdir;

typedef struct {
    sceGifTag giftag0;
    sceGsBitbltbuf bitbltbuf;
    long bitbltbufaddr;
    sceGsTrxpos trxpos;
    long trxposaddr;
    sceGsTrxreg trxreg;
    long trxregaddr;
    sceGsTrxdir trxdir;
    long trxdiraddr;
    sceGifTag giftag1;
} sceGsLoadImage __attribute__((aligned(16)));

typedef struct {
    u_int vifcode[4];
    sceGifTag giftag;
    sceGsBitbltbuf bitbltbuf;
    long bitbltbufaddr;
    sceGsTrxpos trxpos;
    long trxposaddr;
    sceGsTrxreg trxreg;
    long trxregaddr;
    sceGsFinish finish;
    long finishaddr;
    sceGsTrxdir trxdir;
    long trxdiraddr;
} sceGsStoreImage __attribute__((aligned(16)));

typedef struct {
    sceGsFrame frame1;
    u_long frame1addr;
    sceGsZbuf zbuf1;
    long zbuf1addr;
    sceGsXyoffset xyoffset1;
    long xyoffset1addr;
    sceGsScissor scissor1;
    long scissor1addr;
    sceGsPrmodecont prmodecont;
    long prmodecontaddr;
    sceGsColclamp colclamp;
    long colclampaddr;
    sceGsDthe dthe;
    long dtheaddr;
    sceGsTest test1;
    long test1addr;
} sceGsDrawEnv1 __attribute__((aligned(16)));

typedef struct {
    sceGsTest testa;
    long testaaddr;
    sceGsPrim prim;
    long primaddr;
    sceGsRgbaq rgbaq;
    long rgbaqaddr;
    sceGsXyz xyz2a;
    long xyz2aaddr;
    sceGsXyz xyz2b;
    long xyz2baddr;
    sceGsTest testb;
    long testbaddr;
} sceGsClear __attribute__((aligned(16)));

typedef struct {
    tGS_PMODE pmode;
    tGS_SMODE2 smode2;
    tGS_DISPFB2 dispfb;
    tGS_DISPLAY2 display;
    tGS_BGCOLOR bgcolor;
} sceGsDispEnv;

typedef struct {
    sceGsDispEnv disp[2];
    sceGifTag giftag0;
    sceGsDrawEnv1 draw0;
    sceGsClear clear0;
    sceGifTag giftag1;
    sceGsDrawEnv1 draw1;
    sceGsClear clear1;
} sceGsDBuff;

void sceGsSetDefDBuff(sceGsDBuff *dp, short psm, short w, short h, short ztest, short zpsm,  short clear);
void sceGsResetPath();
void sceGsResetGraph(short mode, short inter, short omode, short ffmode);
void sceGsPutDispEnv(sceGsDispEnv *disp);
int sceGsPutDrawEnv(sceGifTag *giftag);
void sceGsSetHalfOffset(sceGsDrawEnv1 *draw, short centerx, short centery, short halfoff);
int sceGsSyncV(int mode);
int sceGsSwapDBuff(sceGsDBuff *db, int id);
int sceGsSetDefStoreImage(sceGsStoreImage *sp, short sbp, short sbw, short spsm, short x, short y, short w, short h);
int sceGsExecStoreImage(sceGsStoreImage *sp, u_long128 *dstaddr);
int sceGsSyncPath(int mode, u_short timeout);
int sceGsSetDefLoadImage(sceGsLoadImage *lp, short dbp, short dbw, short dpsm, short x, short y, short w, short h);
int sceGsExecLoadImage(sceGsLoadImage *lp, u_long128 *srcaddr);

#endif // SCE_LIBGRAPH_H