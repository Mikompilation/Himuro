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
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    case ADPCM_FURN_CHIKUONKI:
        call_no = AE003_CHIKUONKI_STR;
    break;
#endif
    case ADPCM_MC_DOKONIIKUNO:
        call_no = AVMC021_DOKONIIKUNO_STR;
    break;
    case ADPCM_MC_SAMUI:
        call_no = AVMC022_SAMUI_STR;
    break;
    case ADPCM_MC_MITAWAYO:
        call_no = AVMC023_MITAWAYO_STR;
    break;
    case ADPCM_MC_KAESHITE:
        call_no = AVMC024_KAESHITE_STR;
    break;
    case ADPCM_MC_KOKONIITE:
        call_no = AVMC025_KOKONIITE_STR;
    break;
    case ADPCM_MC_NE:
        call_no = AVMC026_NE_STR;
    break;
    case ADPCM_MC_DOUSHITE:
        call_no = AVMC027_DOUSHITE_STR;
    break;
    case ADPCM_MC_NAWAGA:
        call_no = AVMC028_NAWAGA_STR;
    break;
    case ADPCM_MC_SUSURI1:
        call_no = AVMC029_SUSURI1_STR;
    break;
    case ADPCM_MC_SUSURI2:
        call_no = AVMC030_SUSURI2_STR;
    break;
    case ADPCM_MC_SUSURI3:
        call_no = AVMC031_SUSURI3_STR;
    break;
    case ADPCM_MC_SUSURI4:
        call_no = AVMC032_SUSURI4_STR;
    break;
    case ADPCM_MC_KOROSHITA:
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
        if (adpcm_map.mode == ADPCM_MODE_MAP || adpcm_map.mode == ADPCM_MODE_FURN)
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
        EAdpcmCmdStop(0, 0, 30);

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
