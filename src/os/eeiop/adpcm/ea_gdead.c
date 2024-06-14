#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ea_gdead.h"

#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_dat.h"
#include "os/eeiop/adpcm/ea_ctrl.h"

void EAdpcmGdeadMain()
{
    if (adpcm_map.mode != ADPCM_MODE_GDEAD)
    {
        if (
            adpcm_map.mode == ADPCM_MODE_MAP ||
            adpcm_map.mode == ADPCM_MODE_FURN ||
            adpcm_map.mode == ADPCM_MODE_AUTOG ||
            adpcm_map.mode == ADPCM_MODE_GHOST)
        {
            adpcm_map.gdead.mode = AMGD_MODE_PRE_FADE_OUT;
        }
        else
        {
            adpcm_map.gdead.mode = AMGD_MODE_REQ_WAIT_STOP;
        }
        
        adpcm_map.mode = ADPCM_MODE_GDEAD;
    }
    
    switch(adpcm_map.gdead.mode)
    {
    case AMGD_MODE_PRE_FADE_OUT:
        EAdpcmCmdStop(0, 0, 0x50);
        adpcm_map.gdead.mode = AMGD_MODE_REQ_WAIT_STOP;
    break;
    case AMGD_MODE_REQ_WAIT_STOP:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            EAdpcmCmdPlay(0, 0, adpcm_map.gdead.para.file_no, 0, adpcm_map.gdead.para.vol, adpcm_map.gdead.para.pan, adpcm_map.gdead.para.pitch, adpcm_map.gdead.para.fin_flm);
            adpcm_map.gdead.mode = AMGD_MODE_REQ_PLAY;
        }
    break;
    case AMGD_MODE_REQ_PLAY:
        if (5 < EAGetRetStat())
        {
            adpcm_map.gdead.mode = AMGD_MODE_REQ_PLAYING;
        }
    break;
    case AMGD_MODE_REQ_PLAYING:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.gdead.use = 0;
            adpcm_map.event.use = 0;
            adpcm_map.autog.use = 0;
            adpcm_map.hiso.use = 0;
        }
    break;
    case AMGD_MODE_END:
        // do nothing ...
    break;
    case AMGD_MODE_REQ_STOP:
        // do nothing ...
    break;
    }
}

void AdpcmPlayGhostDead(int file_no, u_short vol, int fade_flm)
{
    adpcm_map.gdead.para.file_no = file_no;
    adpcm_map.gdead.para.vol = GetAdpcmVol(file_no);
    adpcm_map.gdead.para.fin_flm = fade_flm;
    adpcm_map.gdead.para.pan = 0x280;
    adpcm_map.gdead.para.pitch = 0xfff;
    adpcm_map.gdead.use = 1;
    adpcm_map.gdead.mode = AMGD_MODE_PRE_FADE_OUT;
    adpcm_map.mode = ADPCM_MODE_GDEAD;
#ifdef BUILD_EU_VERSION
    InitSubtitlesSys();
    SetSubtitlesNCntOne(4, file_no);
#endif
}

void AdpcmStopGhostDead(int fout_flm)
{
    if (adpcm_map.gdead.use != 0)
    {
        EAdpcmCmdStop(0, 0, fout_flm);
        adpcm_map.gdead.mode = AMGD_MODE_REQ_PLAYING;
    }
}
