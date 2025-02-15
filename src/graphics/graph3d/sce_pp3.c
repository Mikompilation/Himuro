#include "common.h"
#include "typedefs.h"
#include "sce_pp3.h"

#include "ee/kernel.h"
#include "ee/registers.h"
#include "sce/libdma.h"
#include "sce/libgraph.h"

static sceGsDBuff db;

void scePP13_Sync()
{
    static int fr = 0;
    static int count = 0;
    static int odev = 0;

    if (count < *REG_RCNT1_COUNT)
    {
        count = *REG_RCNT1_COUNT;
    }

    if (count > 220 || (fr & 0xff) == 0)
    {
        printf("count=%3d\n", count);
        count = 0;
    }

    sceGsSyncPath(0, 0);

    fr++;

    odev = !sceGsSyncV(0);

    *REG_RCNT1_COUNT = 0;

    sceGsSetHalfOffset(fr & 1 ? &db.draw1 : &db.draw0, 2048, 2048, odev);
    FlushCache(0);
    sceGsSwapDBuff(&db, fr);
}

void scePP1_Kick(void *addr)
{
    sceDmaChan *chVif1;

    chVif1 = sceDmaGetChan(1);

    chVif1->chcr.TTE = 1;

    sceDmaSend(chVif1, addr);
}

void scePP3_Kick(u_int *addr, int qwc)
{
    static sceDmaTag tag[2][3];
    static int tid = 0;
    sceDmaTag *tp;
    sceDmaChan *chGif;

    chGif = sceDmaGetChan(2);
    tid = 1 - tid;

    if (qwc >= 0x10000)
    {
        tp = tag[tid];
        sceDmaAddRef(&tp, 0xffff, addr);
        sceDmaAddRef(&tp, qwc - 0xffff, addr + 0x3fffc);
        sceDmaAddEnd(&tp, 0, 0);
    }
    else
    {
        tp = tag[tid];
        sceDmaAddRef(&tp, qwc, addr);
        sceDmaAddEnd(&tp, 0, 0);
    }

    FlushCache(0);
    sceDmaSync(chGif, 0, 0);
    sceDmaSend(chGif, tag[tid]);
}

int scePP1_Disp(u_int mode)
{
    return 0;
}

int scePP1_Init(u_int mode)
{
    return 0;
}

int scePP1_End(u_int mode)
{
    return 0;
}
