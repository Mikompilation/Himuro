#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ea_shinkan.h"

#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_dat.h"
#include "os/eeiop/adpcm/ea_ctrl.h"

void EAdpcmShinkanMain()
{
    if (adpcm_map.mode != ADPCM_MODE_SHINKAN)
    {
        if (
            adpcm_map.mode == ADPCM_MODE_MAP ||
            adpcm_map.mode == ADPCM_MODE_FURN ||
            adpcm_map.mode == ADPCM_MODE_AUTOG ||
            adpcm_map.mode == ADPCM_MODE_GHOST ||
            adpcm_map.mode == ADPCM_MODE_EVENT)
        {
            adpcm_map.shinkan.mode = AMSH_MODE_PRE_FADE_OUT;
        }
        else
        {
            adpcm_map.shinkan.para.start_flm = 0;
            EAdpcmCmdStop(0, 0, 20);
            adpcm_map.shinkan.mode = AMSH_MODE_REQ_PLAYING;
        }
        
        adpcm_map.mode = ADPCM_MODE_SHINKAN;
    }
    
    switch(adpcm_map.shinkan.mode)
    {
    case AMSH_MODE_PRE_FADE_OUT:
        adpcm_map.shinkan = adpcm_map.shinkan; // hack to force adpcm_map in s0 (instruction c0)
        EAdpcmCmdStop(0, 0, 16);
        adpcm_map.shinkan.mode = AMSH_MODE_REQ_WAIT_STOP;
    break;
    case AMSH_MODE_REQ_WAIT_STOP:
        if (EAGetRetStat() != 1 && EAGetRetStat() != 2)
        {
            return;
        }
        EAdpcmCmdPlay(0, 0, adpcm_map.shinkan.para.file_no, 0, adpcm_map.shinkan.para.vol, adpcm_map.shinkan.para.pan, adpcm_map.shinkan.para.pitch, adpcm_map.shinkan.para.fin_flm);
        adpcm_map.bk_para = adpcm_map.shinkan.para;
        adpcm_map.shinkan.mode = AMSH_MODE_REQ_PLAY;
    break;
    case AMSH_MODE_REQ_PLAY:
        if (EAGetRetStat() > 5)
        {
            adpcm_map.shinkan.mode = AMSH_MODE_REQ_PLAYING;
        }
        adpcm_map.bk_para = adpcm_map.shinkan.para;
    break;
    case AMSH_MODE_REQ_PLAYING:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.shinkan.mode = AMSH_MODE_END;
        }
        adpcm_map.bk_para = adpcm_map.shinkan.para;
    break;
    case AMSH_MODE_REQ_STOP_REQ:
        if (EAGetRetStat() > 5)
        {
            EAdpcmCmdStop(0, 0, 30);
            adpcm_map.shinkan.mode = AMSH_MODE_REQ_STOP_WAIT;
            adpcm_map.shinkan.para.start_flm = 0;
        }
        adpcm_map.bk_para = adpcm_map.shinkan.para;
    break;
    case AMSH_MODE_REQ_STOP_WAIT:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.shinkan.mode = AMSH_MODE_END;
            adpcm_map.event.use = 0;
            adpcm_map.autog.use = 0;
            adpcm_map.hiso.use = 0;
            adpcm_map.shinkan.use = 0;
        }
        adpcm_map.bk_para = adpcm_map.shinkan.para;
    break;
    case AMSH_MODE_END:
        adpcm_map.bk_para = adpcm_map.shinkan.para;
    break;
    }
}

void AdpcmPlayShinkan(int file_no, int fade_flm)
{
    adpcm_map.shinkan.para.file_no = file_no;
    adpcm_map.shinkan.para.vol = GetAdpcmVol(file_no);
    adpcm_map.shinkan.para.fin_flm = fade_flm;
    adpcm_map.shinkan.para.pan = 0x280;
    adpcm_map.shinkan.para.pitch = 0xfff;
    adpcm_map.shinkan.use = 1;
    adpcm_map.shinkan.mode = AMSH_MODE_PRE_FADE_OUT;
    adpcm_map.mode = ADPCM_MODE_SHINKAN;
#ifdef BUILD_EU_VERSION
    InitSubtitlesSys();
    SetSubtitlesNCntOne(5, file_no);
#endif
}

void AdpcmStopShinkan(int fout_flm)
{
    if (adpcm_map.shinkan.use != 0)
    {
        adpcm_map.shinkan.mode = AMSH_MODE_REQ_STOP_REQ;
    }
}

u_char IsEndAdpcmShinkan()
{
    if (adpcm_map.shinkan.mode == AMSH_MODE_END)
    {
        adpcm_map.event.use = 0;
        adpcm_map.autog.use = 0;
        adpcm_map.hiso.use = 0;
        adpcm_map.shinkan.use = 0;
        
        return 1;
    }
    
    return 0;
}
