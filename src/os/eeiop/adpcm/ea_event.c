#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ea_event.h"

#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_dat.h"
#include "os/eeiop/adpcm/ea_ctrl.h"

void EAdpcmEventMain()
{    
    if (adpcm_map.mode != ADPCM_MODE_EVENT) {
        if (
            adpcm_map.mode == ADPCM_MODE_MAP ||
            adpcm_map.mode == ADPCM_MODE_FURN ||
            adpcm_map.mode == ADPCM_MODE_AUTOG ||
            adpcm_map.mode == ADPCM_MODE_GHOST)
        {
            adpcm_map.event.mode = AMEV_MODE_PRE_FADE_OUT;
        }
        else
        {
            adpcm_map.event.para.start_flm = 0;
            EAdpcmCmdStop(0, 0, 0x14);
            adpcm_map.event.mode = AMEV_MODE_REQ_PLAYING;
        }
        
        adpcm_map.mode = ADPCM_MODE_EVENT;
    }
    
    switch(adpcm_map.event.mode)
    {
    case AMEV_MODE_PRE_FADE_OUT:
        if (adpcm_map.event.para.start_flm != 0)
        {
            adpcm_map.event.para.start_flm--;
        }
        EAdpcmCmdStop(0, 0, 0x50);
        adpcm_map.event.mode = AMEV_MODE_REQ_WAIT_STOP;
    break;
    case AMEV_MODE_REQ_WAIT_STOP:
        if (adpcm_map.event.para.start_flm != 0)
        {
            adpcm_map.event.para.start_flm--;
        }
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            if (adpcm_map.event.para.start_flm == 0) {
                adpcm_map.m_flg = 2;
                EAdpcmCmdPlay(0, 0, adpcm_map.event.para.file_no, 0, adpcm_map.event.para.vol, adpcm_map.event.para.pan, adpcm_map.event.para.pitch, adpcm_map.event.para.fin_flm);
                adpcm_map.event.mode = AMEV_MODE_REQ_PLAY;
            }
            adpcm_map.mvol = 0xfff;
        }
    break;
    case AMEV_MODE_REQ_PLAY:
        if (EAGetRetStat() > 5)
        {
            adpcm_map.event.mode = AMEV_MODE_REQ_PLAYING;
        }
        adpcm_map.mvol = 0xfff;
    break;
    case AMEV_MODE_REQ_PLAYING:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.event.use = 0;
            adpcm_map.autog.use = 0;
            adpcm_map.hiso.use = 0;
            adpcm_map.event.para.start_flm = 0;
            adpcm_map.event.mode = AMEV_MODE_PRE_FADE_OUT;
        }
        adpcm_map.mvol = 0xfff;
    break;
    case AMEV_MODE_END:
        adpcm_map.event.use = 0;
        adpcm_map.autog.use = 0;
        adpcm_map.hiso.use = 0;
        adpcm_map.event.para.start_flm = 0;
        adpcm_map.event.mode = AMEV_MODE_PRE_FADE_OUT;
        adpcm_map.mvol = 0xfff;
    }
}

void AdpcmPlayEvent(/* a0 4 */ int file_no, /* s1 17 */ u_short start_flm, /* s2 18 */ int fade_flm)
{
    switch(file_no) {
    case 0:
        adpcm_map.event.para.file_no = AVMC000_KIRIE0_STR;
    break;
    case 1:
        adpcm_map.event.para.file_no = AVMC001_KIRIE1_STR;
    break;
    case 2:
        adpcm_map.event.para.file_no = AVMC002_KIRIE2_STR;
    break;
    case 3:
        adpcm_map.event.para.file_no = AVMC003_MAHUYU0_STR;
#ifdef BUILD_EU_VERSION
        InitSubtitlesSys();
        SetSubtitlesNCntOne(2, 0);
#endif
    break;
    case 4:
        adpcm_map.event.para.file_no = AVMC004_MAHUYU1_STR;
    break;
    case 5:
        adpcm_map.event.para.file_no = AVMC005_MAHUYU2_STR;
    break;
    case 6:
        adpcm_map.event.para.file_no = AVMC006_MIKU0_STR;
    break;
    case 7:
        adpcm_map.event.para.file_no = AVMC007_MIKU1_STR;
    break;
    case 8:
        adpcm_map.event.para.file_no = AVMC008_MIKU2_STR;
    break;
    case 9:
        adpcm_map.event.para.file_no = AVMC009_MAFUYU3_STR;
    break;
    case 10:
        adpcm_map.event.para.file_no = AVMC010_MAFUYU4_STR;
    break;
    case 11:
        adpcm_map.event.para.file_no = AVMC011_MAFUYU5_STR;
    break;
    case 25:
        adpcm_map.event.para.file_no = AVMC012_MAFUYU6_STR;
#ifdef BUILD_EU_VERSION
        InitSubtitlesSys();
        SetSubtitlesNCntOne(2, 1);
#endif
    break;
    case 13:
        adpcm_map.event.para.file_no = AVMC021_DOKONIIKUNO_STR;
    break;
    case 14:
        adpcm_map.event.para.file_no = AVMC022_SAMUI_STR;
    break;
    case 15:
        adpcm_map.event.para.file_no = AVMC023_MITAWAYO_STR;
    break;
    case 16:
        adpcm_map.event.para.file_no = AVMC024_KAESHITE_STR;
    break;
    case 17:
        adpcm_map.event.para.file_no = AVMC025_KOKONIITE_STR;
    break;
    case 18:
        adpcm_map.event.para.file_no = AVMC026_NE_STR;
    break;
    case 19:
        adpcm_map.event.para.file_no = AVMC027_DOUSHITE_STR;
    break;
    case 20:
        adpcm_map.event.para.file_no = AVMC028_NAWAGA_STR;
    break;
    case 21:
        adpcm_map.event.para.file_no = AVMC029_SUSURI1_STR;
    break;
    case 22:
        adpcm_map.event.para.file_no = AVMC030_SUSURI2_STR;
    break;
    case 23:
        adpcm_map.event.para.file_no = AVMC031_SUSURI3_STR;
    break;
    case 24:
        adpcm_map.event.para.file_no = AVMC032_SUSURI4_STR;
    break;
    default:
    // case 12:
        adpcm_map.event.para.file_no = AVMC020_KOROSHITA_STR;
    break;
    }
    
    adpcm_map.event.para.vol = GetAdpcmVol(adpcm_map.event.para.file_no);
    adpcm_map.event.para.fin_flm = fade_flm;
    adpcm_map.event.para.start_flm = start_flm;
    adpcm_map.event.para.pan = 0x280;
    adpcm_map.event.para.pitch = 0xfff;
    adpcm_map.event.use = 1;
    adpcm_map.event.mode = AMEV_MODE_PRE_FADE_OUT;
    adpcm_map.mode = ADPCM_MODE_EVENT;
}

void AdpcmStopEvent(/* a0 4 */ int fout_flm)
{
    if (adpcm_map.event.use != 0)
    {
        EAdpcmCmdStop(0, 0, fout_flm);
        adpcm_map.event.para.start_flm = 0;
        adpcm_map.event.mode = AMEV_MODE_REQ_PLAYING;
    }
}
