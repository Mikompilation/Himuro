#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ea_soul.h"

#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_dat.h"
#include "os/eeiop/adpcm/ea_ctrl.h"

void EAdpcmSoulMain()
{
    if (adpcm_map.mode != ADPCM_MODE_SOUL)
    {
        if (
            adpcm_map.mode == ADPCM_MODE_MAP ||
            adpcm_map.mode == ADPCM_MODE_FURN ||
            adpcm_map.mode == ADPCM_MODE_AUTOG ||
            adpcm_map.mode == ADPCM_MODE_GHOST ||
            adpcm_map.mode == ADPCM_MODE_GDEAD ||
            adpcm_map.mode == ADPCM_MODE_TAPE)
        {
            adpcm_map.soul.mode = AMSL_MODE_PRE_FADE_OUT;
        }
        else if (adpcm_map.soul.mode >= AMSL_MODE_REQ_WAIT2)
        {
            EAdpcmCmdStop(0, 0, 0x1e);
            adpcm_map.soul.mode = AMSL_MODE_REQ_STOP;
        }
        else
        {
            adpcm_map.soul.mode = AMSL_MODE_PRE_FADE_OUT;
        }
        
        adpcm_map.mode = ADPCM_MODE_SOUL;
    }
    
    switch(adpcm_map.soul.mode)
    {
    case AMSL_MODE_PRE_FADE_OUT:
        EAdpcmCmdStop(0,0,adpcm_map.soul.para.fout_flm);
        adpcm_map.soul.count = 0;
        adpcm_map.soul.mode = AMSL_MODE_REQ_WAIT1;
    break;
    case AMSL_MODE_REQ_WAIT1:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.soul.mode = AMSL_MODE_REQ_WAIT2;
        }
        else
        {
            adpcm_map.soul.count++;
            
            if (adpcm_map.soul.count == 200)
            {
                EAdpcmCmdStop(0, 0, 0);
            }
            else if (adpcm_map.soul.count > 1000)
            {
                adpcm_map.soul.count = 0;
            }
        }
    break;
    case AMSL_MODE_REQ_WAIT2:
        // do nothing ...
    break;
    case AMSL_MODE_REQ_PLAY:
        // do nothing ...
    break;
    case AMSL_MODE_REQ_STOP:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.soul.use = 0;
            adpcm_map.hiso.use = 0;
            adpcm_map.autog.use = 0;
            adpcm_map.event.use = 0;
            adpcm_map.soul.mode = AMSL_MODE_PRE_FADE_OUT;
        }
    break;
    }
}

void AdpcmPreSoulFadeOut(/* a0 4 */ u_short fout_flm)
{
    adpcm_map.soul.mode = AMSL_MODE_PRE_FADE_OUT;
    adpcm_map.soul.para.fout_flm = fout_flm;
    adpcm_map.soul.use = 1;
}

u_char IsFadeoutEndAdpcmSoul()
{
    return adpcm_map.soul.mode == AMSL_MODE_REQ_WAIT2;
}

void AdpcmPlaySoul(/* v0 2 */ int file_no, /* a1 5 */ u_short vol, /* s1 17 */ int fade_flm)
{
    adpcm_map.soul.para.file_no = file_no;
    adpcm_map.soul.para.vol = GetAdpcmVol(file_no);
    adpcm_map.soul.para.fin_flm = fade_flm;
    adpcm_map.soul.para.pan = 0x280;
    adpcm_map.soul.para.pitch = 0xfff;
    EAdpcmCmdPlay(0, 0, adpcm_map.soul.para.file_no, 0, adpcm_map.soul.para.vol, 0x280, 0xfff, adpcm_map.soul.para.fin_flm);
    adpcm_map.soul.mode = AMSL_MODE_REQ_PLAY;
}

void AdpcmStopSoul(/* a0 4 */ int fout_flm)
{
    if (adpcm_map.soul.use != 0x0)
    {
        EAdpcmCmdStop(0, 0, fout_flm);
        adpcm_map.soul.mode = AMSL_MODE_REQ_STOP;
    }
}
