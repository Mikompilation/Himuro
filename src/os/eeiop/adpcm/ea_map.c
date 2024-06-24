#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ea_map.h"

#include "os/eeiop/adpcm/ea_dat.h"
#include "os/eeiop/adpcm/ea_ctrl.h"
#include "os/eeiop/adpcm/ea_cmd.h"

#include "os/eeiop/sd_room.h"
#include "main/glob.h"

static int EAGetNowMapFileNo();
static u_short EAGetNowMapBgmVol();

void EAdpcmMapMain()
{
    int file_no;
    static int wait_cnt;
    
    if (adpcm_map.mode != ADPCM_MODE_MAP)
    {
        adpcm_map.mode = ADPCM_MODE_MAP;
        wait_cnt = 10;
    }
    else if (wait_cnt != 0)
    {
        if (wait_cnt > 10)
        {
            wait_cnt = 10;
        }
        
        wait_cnt--;
        
        if (wait_cnt != 0)
        {
            return;
        }
        
        file_no = EAGetNowMapFileNo();
        
        if (file_no == 0xffffffff)
        {
            wait_cnt = 1;
            return;
        }
        
        if (adpcm_map.map.para.file_no == file_no)
        {
            adpcm_map.map.para.vol = GetAdpcmVol(file_no);
            adpcm_map.map.para.pan = 0x280;
            adpcm_map.map.para.pitch = 0xfff;
            EAdpcmCmdPlay(0, 1, adpcm_map.map.para.file_no, adpcm_map.map.para.count, adpcm_map.map.para.vol, 0x280, 0xfff, 0x96);
        }
        else
        {
            adpcm_map.map.para.file_no = file_no;
            adpcm_map.map.para.vol = GetAdpcmVol(file_no);
            adpcm_map.map.para.pan = 0x280;
            adpcm_map.map.para.pitch = 0xfff;
            EAdpcmCmdPlay(0, 1, adpcm_map.map.para.file_no, 0, adpcm_map.map.para.vol, 0x280, 0xfff, 0);
        }
    }
    else if (adpcm_map.map.stop != 0)
    {
        if (EAGetRetStat() != 1 && EAGetRetStat() != 2)
        {
            return;
        }
        
        file_no = EAGetNowMapFileNo();
        
        if (file_no == 0xffffffff)
        {
            return;
        }

        adpcm_map.map.stop = 0;
        
        if (adpcm_map.map.para.file_no == file_no)
        {
            adpcm_map.map.para.vol = GetAdpcmVol(file_no);
            adpcm_map.map.para.pan = 0x280;
            adpcm_map.map.para.pitch = 0xfff;
            EAdpcmCmdPlay(0, 1, adpcm_map.map.para.file_no, adpcm_map.map.para.count, adpcm_map.map.para.vol, 0x280, 0xfff, 0x96);
        }
        else
        {
            adpcm_map.map.para.file_no = file_no;
            adpcm_map.map.para.vol = GetAdpcmVol(file_no);
            adpcm_map.map.para.pan = 0x280;
            adpcm_map.map.para.pitch = 0xfff;
            EAdpcmCmdPlay(0, 1, adpcm_map.map.para.file_no, 0, adpcm_map.map.para.vol, 0x280, 0xfff, 0);
        }
    }
    else
    {
        file_no = EAGetNowMapFileNo();
        
        if (adpcm_map.map.para.file_no == file_no)
        {
            adpcm_map.map.para.count = EAGetRetCount();
        }
        else
        {
            EAdpcmCmdStop(0, 0, 0x1e);
            adpcm_map.map.stop = ADPCM_MODE_MAP;
        }
    }
}

static int EAGetNowMapFileNo()
{
    if (plyr_wrk.pr_info.room_no == 0xff)
    {
        return -1;
    }
    
    return GetSdrAdpcmFno(plyr_wrk.pr_info.room_no);
}

static u_short EAGetNowMapBgmVol()
{
    if (plyr_wrk.pr_info.room_no == 0xff)
    {
        return 0x3fff;
    }

    return GetSdrAdpcmVol(plyr_wrk.pr_info.room_no);
}
