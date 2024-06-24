#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ea_hiso.h"

#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_dat.h"
#include "os/eeiop/adpcm/ea_ctrl.h"

void AdpcmPlayHiso(int file_no, int fade_flm)
{
    int call_no;
    
    switch(file_no)
    {
    case 33:
        call_no = AE003_CHIKUONKI_STR;
    break;
    case 13:
        call_no = AVMC021_DOKONIIKUNO_STR;
    break;
    case 14:
        call_no = AVMC022_SAMUI_STR;
    break;
    case 15:
        call_no = AVMC023_MITAWAYO_STR;
    break;
    case 16:
        call_no = AVMC024_KAESHITE_STR;
    break;
    case 17:
        call_no = AVMC025_KOKONIITE_STR;
    break;
    case 18:
        call_no = AVMC026_NE_STR;
    break;
    case 19:
        call_no = AVMC027_DOUSHITE_STR;
    break;
    case 20:
        call_no = AVMC028_NAWAGA_STR;
    break;
    case 21:
        call_no = AVMC029_SUSURI1_STR;
    break;
    case 22:
        call_no = AVMC030_SUSURI2_STR;
    break;
    case 23:
        call_no = AVMC031_SUSURI3_STR;
    break;
    case 24:
        call_no = AVMC032_SUSURI4_STR;
    break;
    case 12:
    default:
        call_no = AVMC020_KOROSHITA_STR;
    break;
    }
    
    if (adpcm_map.hiso.use != 0)
    {
        if (adpcm_map.mode == ADPCM_MODE_HISO)
        {
            adpcm_map.hiso.para.file_no = call_no;
            adpcm_map.hiso.para.vol = GetAdpcmVol(call_no);
            adpcm_map.hiso.para.fin_flm = fade_flm;
            adpcm_map.hiso.para.pan = 0x280;
            adpcm_map.hiso.para.pitch = 0xfff;
            adpcm_map.hiso.mode = AMHS_MODE_PRE_FADE_OUT;
        }
    }
    else
    {
        adpcm_map.hiso.para.file_no = call_no;
        adpcm_map.hiso.para.vol = GetAdpcmVol(call_no);
        adpcm_map.hiso.para.fin_flm = fade_flm;
        adpcm_map.hiso.para.pan = 0x280;
        adpcm_map.hiso.para.pitch = 0xfff;
        adpcm_map.hiso.use = 1;
    }
}

void AdpcmStopHiso(int fout_flm)
{   
    if (adpcm_map.hiso.use != 0)
    {
        if (IsHighModeUse(3) != 0)
        {
            adpcm_map.hiso.use = 0;
        }
        else
        {
            adpcm_map.hiso.para.fout_flm = fout_flm;
            adpcm_map.hiso.mode = AMHS_MODE_REQ_STOP;
        }
    }
}

void EAdpcmHisoMain()
{    
    if (adpcm_map.mode != ADPCM_MODE_HISO)
    {
        if (
            adpcm_map.mode == ADPCM_MODE_MAP ||
            adpcm_map.mode == ADPCM_MODE_FURN
        )
        {
            if (adpcm_map.hiso.mode == AMHS_MODE_END)
            {
                adpcm_map.hiso.mode = AMHS_MODE_REQ_STOP;
            }
            else
            {
                adpcm_map.hiso.mode = AMHS_MODE_PRE_FADE_OUT;
            }
        }
        else
        {
            if (adpcm_map.hiso.mode == AMHS_MODE_END)
            {
                adpcm_map.hiso.mode = AMHS_MODE_REQ_STOP;
            }
            else
            {
                EAdpcmCmdStop(0, 0, 0);
                adpcm_map.hiso.count = 0;
                adpcm_map.hiso.mode = AMHS_MODE_REQ_WAIT_STOP;
            }
        }
        
        adpcm_map.mode = ADPCM_MODE_HISO;
    }
    
    switch(adpcm_map.hiso.mode)
    {
    case AMHS_MODE_PRE_FADE_OUT:
        EAdpcmCmdStop(0, 0, 0x1e);
        adpcm_map.hiso.mode = AMHS_MODE_REQ_WAIT_STOP;
        adpcm_map.hiso.count = 0;
    break;
    case AMHS_MODE_REQ_WAIT_STOP:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.m_flg = 2;
            EAdpcmCmdPlay(0, 0, adpcm_map.hiso.para.file_no, 0, adpcm_map.hiso.para.vol, adpcm_map.hiso.para.pan, adpcm_map.hiso.para.pitch, 0);
            adpcm_map.hiso.mode = AMHS_MODE_REQ_PLAY;
            adpcm_map.mvol = 0xfff;
        }
        else
        {
            adpcm_map.hiso.count++;
            
            if (adpcm_map.hiso.count == 200)
            {
                EAdpcmCmdStop(0, 0, 0);
            }
            else if (adpcm_map.hiso.count > 500)
            {
                EAdpcmCmdStop(0, 0, 0);
                adpcm_map.hiso.count = 0;
                adpcm_map.hiso.use = 0;
            }
        }
    break;
    case AMHS_MODE_REQ_PLAY:
        if (EAGetRetStat() == 2)
        {
            adpcm_map.hiso.use = 0;
        }
        
        adpcm_map.mvol = 0xfff;
    break;
    case AMHS_MODE_REQ_STOP:
        EAdpcmCmdStop(0, 0, adpcm_map.hiso.para.fout_flm);
        adpcm_map.hiso.mode = AMHS_MODE_END;
        adpcm_map.mvol = 0xfff;
    break;
    case AMHS_MODE_END:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.hiso.mode = AMHS_MODE_PRE_FADE_OUT;
            adpcm_map.hiso.use = 0;
        }
        
        adpcm_map.mvol = 0xfff;
    break;
    }
}
