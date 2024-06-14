#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "main.h"

#include "main/glob.h"
#include "main/gamemain.h"
#include "outgame/title.h"
#include "os/system.h"
#include "os/eeiop/eeiop.h"
#include "os/eeiop/eese.h"
#include "os/eeiop/se_cmd.h"
#include "graphics/mov/movie.h"
#include "graphics/graph2d/tim2_new.h"
#include "graphics/graph2d/g2d_main.h"
#include "graphics/graph3d/sgdma.h"
#include "graphics/graph3d/gra3d.h"
// #include "os/eeiop/adpcm/ea_cmd.h"

int sceGsSyncPath(int mode, u_short timeout);

int main()
{    
    InitSystem();
    do
    {
        InitGameFirst();
        do
        {
            while (PlayMpegEvent()) {};
            EiMain();
            GameMain();
            CheckDMATrans();
            sceGsSyncPath(0, 0);
            vfunc();
            DrawAll2DMes_P2();
            FlushModel(1);
            ClearTextureCache();
            SeCtrlMain();
        } while (!SoftResetChk());
    } while (1);
}

void InitGameFirst()
{
    sys_wrk.reset = 0;
    gra3dInitFirst();
    gra2dInitBG();
    MovieInitWrk();
    realtime_scene_flg = 0;
    sys_wrk.game_mode = GAME_MODE_INIT;
    outgame_wrk.mode = 0;
    outgame_wrk.mode_next = 4;
    memset(&ingame_wrk, 0, sizeof(INGAME_WRK));
    sys_wrk.sreset_ng = 0;
    opening_movie_type = 0;
}

void CallSoftReset()
{
    int lcount;

    lcount = 0;
    
    SeStopAll();
    InitSe();
    SetIopCmdSm(1, 0, 0, 0);
    EAdpcmCmdStop(0, 0, 0);
    
    sys_wrk.reset = 0;
    
    SetSysBackColor(0, 0, 0);
    
    scene_bg_color = 0;
    
    while (1)
    {
        EiMain();
        
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            EAdpcmCmdInit(1);
            break;
        }
        
        lcount++;
        
        if (lcount > 30)
        {
            EAdpcmCmdStop(0, 0, 0);
            lcount = 0;
        }
        
        vfunc();
    }
}

int SoftResetChk()
{
    if (
        *key_now[8] && *key_now[9] && *key_now[10] &&
        *key_now[11] && *key_now[12] && *key_now[13]
    )
    {
        sys_wrk.sreset_count = 0;
    }
    else
    {
        sys_wrk.sreset_count = 0;
    }
    
    if (sys_wrk.sreset_ng != 0)
    {
        sys_wrk.sreset_count = 0;
    }

    if (sys_wrk.sreset_count >= 60)
    {
        sys_wrk.reset = 1;
    }
    
    if (sys_wrk.reset == 1)
    {
        sys_wrk.sreset_count = 0;
        CallSoftReset();
        return 1;
    }
    
    return 0;
}
