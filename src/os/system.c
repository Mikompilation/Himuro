#include "common.h"
#include "typedefs.h"
#include "system.h"
#include "enums.h"

#include "ee/registers.h"
#include "ee/kernel.h"
#include "sce/libdma.h"
#include "sce/libsdr.h"
#include "sce/libcdvd.h"
#include "sce/libgraph.h"
#include "sce/devvif0.h"
#include "sce/devvu0.h"
#include "sce/sifdev.h"
#include "sce/sifrpc.h"
#include "sce/mpeg/disp.h"
#include "sce/misc/diei.h"

#include "main/glob.h"
#include "mc/mc_main.h"
#include "os/key_cnf.h"
#include "os/pad.h"
#include "os/eeiop/eese.h"
#include "graphics/graph2d/g2d_debug.h"

int odev = 0;
int stop_put_draw_env = 0;
static int fr = 0;
sceGsDBuff g_db = {0};
static int down_fr;

static void InitIop();
static void LoadDefModule();

void InitSystem()
{
    InitIop();
    LoadDefModule();
    InitIop();
    sceSifInitIopHeap();
    sceFsReset();
    LoadIRX();
    InitSysWrk();
    SetDefaultKeyType();
    InitGraphics();
    InitPad();
    EiInit();
    InitSe();
    InitOptionWrk();
    sceMcInit();

#ifdef BUILD_EU_VERSION
    mc_language = 0;
#endif
    mc_start_flg = 1;
    sceSdRemoteInit();
}

void EndSystem()
{
    //
}

static void InitIop()
{
    sceSifInitRpc(0);
    sceCdInit(0);
    sceCdMmode(2);
}

static void LoadDefModule()
{
#ifdef BUILD_EU_VERSION
    while (!sceSifRebootIop("cdrom0:\\IOPRP23.IMG;1")) {};
#else
    while (!sceSifRebootIop("cdrom0:\\IOPRP224.IMG;1")) {};
#endif
    while (!sceSifSyncIop()) {};
}

void LoadIRX()
{
    while (sceSifLoadModule("cdrom0:\\LIBSD.IRX;1", 0, NULL) < 0) {};
    while (sceSifLoadModule("cdrom0:\\SDRDRV.IRX;1", 0, NULL) < 0) {};
    while (sceSifLoadModule("cdrom0:\\SIO2MAN.IRX;1", 0, NULL) < 0) {};
    while (sceSifLoadModule("cdrom0:\\PADMAN.IRX;1", 0, NULL) < 0) {};
    while (sceSifLoadModule("cdrom0:\\MCMAN.IRX;1", 0, NULL) < 0) {};
    while (sceSifLoadModule("cdrom0:\\MCSERV.IRX;1", 0, NULL) < 0) {};
    while (sceSifLoadModule("cdrom0:\\IOPSYS.IRX;1", 0, NULL) < 0) {};
}

void InitSysWrk()
{
    GetNowClock(&sys_wrk.rtc);
    sys_wrk.reset = 0;
    sys_wrk.pause = 0;
    sys_wrk.game_mode = GAME_MODE_INIT;
    sys_wrk.mode = 0;
    sys_wrk.init = 1;
}

void GetNowClock(sceCdCLOCK *nc)
{
    memset(nc, 0, sizeof(sceCdCLOCK));
    
    if (!sceCdReadClock(nc)) {
        memset(nc, 0, sizeof(sceCdCLOCK));
        nc->pad = 1;
    }
}

void InitOptionWrk()
{
    opt_wrk.pad_mode = 0;
    opt_wrk.se_vol = 0x3fff;
    opt_wrk.bgm_vol = 0xccc;
    opt_wrk.pad_move = 0;
    opt_wrk.key_type = 0;
    opt_wrk.sound_mode = 0;
    SeSetMVol(0xccc);
    SeSetSteMono(opt_wrk.sound_mode);
}

void InitGraphics()
{
    sceDmaEnv env;
    
    sceDmaReset(1);
    sceGsResetPath();
    sceDevVif0Reset();
    sceDevVu0Reset();

#ifdef BUILD_EU_VERSION
    sceGsResetGraph(0, 1, sys_wrk.pal_disp_mode == 0 ? 3 : 2, 1);
#else
    sceGsResetGraph(0, 1, 2, 1);
#endif
    
    *REG_RCNT0_MODE = 0x83;
    *REG_RCNT1_MODE = 0x83;
    
    sceDmaGetEnv(&env);
    
    env.notify = 4;
    
    sceDmaPutEnv(&env);
    sceGsSetDefDBuff(&g_db, 0, 0x280, 0xe0, 2, 0x31, 1);
    
    pdrawenv = &g_db.draw0;
    
    clearGsMem(0, 0, 0, 0x280, 0x1c0);
    SetSysBackColor(0, 0, 0);

#ifdef BUILD_EU_VERSION
    if (sys_wrk.pal_disp_mode == 0)
    {
        g_db.disp[1].display.DX = 0x290;
        g_db.disp[1].display.DY = 0x68;
        g_db.disp[0].display.DX = 0x290;
        g_db.disp[0].display.DY = 0x68;
    }
    else
    {
        g_db.disp[1].display.DX = 0x27c;
        g_db.disp[1].display.DY = 0x32;
        g_db.disp[0].display.DX = 0x27c;
        g_db.disp[0].display.DY = 0x32;
    }
#endif
}


int userGsSwapDBuff(sceGsDBuff *db, int id)
{
    int ret;
    
    ret = 0;
    
    if (id % 2)
    {
        sceGsPutDispEnv(&db->disp[1]);
    }
    else
    {
        sceGsPutDispEnv(&db->disp[0]);
    }
    
    
    if (stop_put_draw_env == 0)
    {
        if (id % 2)
        {
            ret = sceGsPutDrawEnv(&db->giftag1);
        }
        else
        {
            ret = sceGsPutDrawEnv(&db->giftag0);
        }
    }
    
    return ret;
}

int vblankHandler(int val)
{
    down_fr++;
    EIntr();
    return 0;
}

void vfunc()
{
    static int count = 0;
    static int dfro = 0;
    
    StopPerformanceCounter();
    
    if (count < *REG_RCNT1_COUNT)
    {
        count = *REG_RCNT1_COUNT;
    }
    
    if (count > 0xdc || (fr & 0xff) == 0)
    {
        count = 0;
    }
    
    if (down_fr != 0 && down_fr == dfro && odev != (sys_wrk.count & 1))
    {
        sys_wrk.count++;
        fr++;
        down_fr = 0;
    }
    
    dfro = down_fr;
    *REG_RCNT0_COUNT = 0;
    *REG_RCNT1_COUNT = 0;
    
    if (sys_wrk.count & 1)
    {
        pdrawenv = &g_db.draw1;
        sceGsSetHalfOffset(&g_db.draw0, 0x800, 0x800, (odev & 0xffffU) + 1 & 1);
    }
    else
    {
        pdrawenv = &g_db.draw0;
        sceGsSetHalfOffset(&g_db.draw1, 0x800, 0x800, (odev & 0xffffU) + 1 & 1);
    }
    
    sys_wrk.count++;
    fr++;
    
    odev = sceGsSyncV(0) ^ 1;
    FlushCache(0);
    sceGsSwapDBuff(&g_db, fr);
    PadSyncCallback();
    
    stop_put_draw_env = 0;
    *REG_RCNT1_COUNT = 0;
    
    StartPerformanceCounter();
}

sceGsDrawEnv1 *pdrawenv = NULL;

void SetSysBackColor(u_char r, u_char g, u_char b)
{
    g_db.clear0.rgbaq.R = r;
    g_db.clear0.rgbaq.G = g;
    g_db.clear0.rgbaq.B = b;
    g_db.clear1.rgbaq.R = r;
    g_db.clear1.rgbaq.G = g;
    g_db.clear1.rgbaq.B = b;
}
