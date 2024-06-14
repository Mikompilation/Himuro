#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ea_btlmode.h"

#include "os/eeiop/adpcm/ea_ctrl.h"
#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_dat.h"

void EAdpcmBtlmodeMain()
{
    if (adpcm_map.mode != 0xf)
    {
        adpcm_map.mode = 0xf;
        adpcm_map.btlmode.para.file_no = -1;
    }
    
    switch(adpcm_map.btlmode.mode)
    {
    case BTL_MODE_READY:
        EAdpcmCmdStop(0, 0, adpcm_map.btlmode.para.fout_flm);
        adpcm_map.btlmode.mode = BTL_MODE_GAME;
    break;
    case BTL_MODE_GAME:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.btlmode.mode = BTL_MODE_FAILED;
        }
    break;
    case BTL_MODE_FAILED:
        // do nothing ...
    break;
    case BTL_MODE_CLEAR:
        if (EAGetRetStat() > 5)
        {
            adpcm_map.btlmode.mode = BTL_MODE_NEXT;
        }
    break;
    case BTL_MODE_NEXT:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.btlmode.mode = BTL_MODE_FAILED;
            adpcm_map.btlmode.para.file_no = -1;
        }
    break;
    }
}

void AdpcmPreBtlmodeFadeOut(u_short fout_flm)
{
    adpcm_map.btlmode.mode = BTL_MODE_READY;
    adpcm_map.btlmode.para.fout_flm = fout_flm;
    adpcm_map.btlmode.use = 1;
}

u_char IsEndAdpcmBtlmode()
{
    return adpcm_map.btlmode.mode == BTL_MODE_FAILED;
}

void AdpcmPreBtlmodePreload(int file_no)
{
    adpcm_map.btlmode.para.file_no = file_no;
    EAdpcmCmdPreload(0, file_no, 0, 0);
    adpcm_map.btlmode.mode = BTL_MODE_FAILED;
}

u_char IsPreLoadEndAdpcmBtlmode()
{
    if (EAGetRetTune() == adpcm_map.btlmode.para.file_no)
    {
        if (EAGetRetStat() == -1)
        {
            return 0xff;
        }
        else if (EAGetRetStat() == 5)
        {
            return 1;
        }
    }
    
    return 0;
}

void AdpcmBtlmodePreLoadEndPlay()
{
    EAdpcmCmdPlay(0, 0, adpcm_map.btlmode.para.file_no, 0, GetAdpcmVol(adpcm_map.btlmode.para.file_no), 0x280, 0xfff, 0);
    adpcm_map.btlmode.mode = BTL_MODE_CLEAR;
}

void AdpcmBtlmodePlay(int file_no)
{
    adpcm_map.btlmode.para.file_no = file_no;
    EAdpcmCmdPlay(0, 0, adpcm_map.btlmode.para.file_no, 0, GetAdpcmVol(file_no), 0x280, 0xfff, 0);
    adpcm_map.btlmode.mode = BTL_MODE_CLEAR;
}

void AdpcmBtlmodeEnd()
{
    EAdpcmCmdStop(0, 0, 0x1e);
    adpcm_map.btlmode.use = 0;
    adpcm_map.hiso.use = 0;
    adpcm_map.autog.use = 0;
    adpcm_map.event.use = 0;
    adpcm_map.btlmode.mode = BTL_MODE_READY;
}