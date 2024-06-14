#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ea_magatoki.h"

#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_dat.h"
#include "os/eeiop/adpcm/ea_ctrl.h"

void EAdpcmMagatokiMain()
{    
    if (adpcm_map.mode != ADPCM_MODE_MAGATOKI)
    {
        adpcm_map.mode = ADPCM_MODE_MAGATOKI;
        adpcm_map.maga.mode = AMMG_MODE_FIRST_FOUT;
    }
    
    switch(adpcm_map.maga.mode)
    {
    case AMMG_MODE_FIRST_FOUT:
        EAdpcmCmdStop(0, 0, 30);
        adpcm_map.maga.para.file_no = AM001_MAGAIN_STR;
        adpcm_map.maga.para.vol = GetAdpcmVol(AM001_MAGAIN_STR);
        adpcm_map.maga.para.pan = 0x280;
        adpcm_map.maga.para.pitch = 0xfff;
        adpcm_map.maga.mode = AMMG_MODE_FOUT_WAIT;
    break;
    case AMMG_MODE_FOUT_WAIT:
        if (EAGetRetStat() != 1 && EAGetRetStat() != 2)
        {
            return;
        }
        EAdpcmCmdPlay(0, 0, adpcm_map.maga.para.file_no, 0, adpcm_map.maga.para.vol, adpcm_map.maga.para.pan, adpcm_map.maga.para.pitch, 0);
        adpcm_map.bk_para = adpcm_map.maga.para;
        adpcm_map.maga.mode = AMMG_MODE_MIN_BGM;
    case AMMG_MODE_MIN_BGM:
        if (EAGetRetStat() > 5)
        {
            adpcm_map.maga.mode = AMMG_MODE_MIN_BGM_FADE;
        }
        adpcm_map.bk_para = adpcm_map.maga.para;
    break;
    case AMMG_MODE_MIN_BGM_FADE:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.maga.mode = AMMG_MODE_MAGA_PLAY_WAIT;
        }
        adpcm_map.bk_para = adpcm_map.maga.para;
    break;
    case AMMG_MODE_PLAY:
        if (EAGetRetStat() > 5) {
            adpcm_map.maga.mode = AMMG_MODE_PLAYING;
        }
        adpcm_map.bk_para = adpcm_map.maga.para;
    break;
    case AMMG_MODE_PLAYING:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.maga.mode = AMMG_MODE_FIRST_FOUT;
            adpcm_map.maga.para.file_no = 0;
            adpcm_map.maga.use = 0;
            adpcm_map.ghost.use = 0;
            adpcm_map.autog.use = 0;
            adpcm_map.hiso.use = 0;
            adpcm_map.event.use = 0;
        }
        adpcm_map.bk_para = adpcm_map.maga.para;
    break;
    case AMMG_MODE_MAGA_PLAY_WAIT:
        adpcm_map.bk_para = adpcm_map.maga.para;
    break;
    }
}

void AdpcmPreMagatokiFadeOut(/* a0 4 */ u_short fout_flm)
{
    adpcm_map.maga.mode = AMMG_MODE_FIRST_FOUT;
    adpcm_map.maga.para.fout_flm = fout_flm;
    adpcm_map.maga.use = 1;
}

u_char IsFadeoutEndAdpcmMagatoki()
{
    return adpcm_map.maga.mode == AMMG_MODE_MAGA_PLAY_WAIT;
}

void AdpcmMagatokiPlay(/* v1 3 */ int file_no)
{
    adpcm_map.maga.mode = AMMG_MODE_PLAY;
    adpcm_map.maga.para.file_no = file_no;
    adpcm_map.maga.para.vol = GetAdpcmVol(file_no);
    adpcm_map.maga.para.pan = 0x280;
    adpcm_map.maga.para.pitch = 0xfff;
    EAdpcmCmdPlay(0, 1, adpcm_map.maga.para.file_no, 0, adpcm_map.maga.para.vol, 0x280, 0xfff, 0);
}

void AdpcmMagatokiStop()
{
    EAdpcmCmdStop(0, 0, 100);
    
    if (adpcm_map.maga.mode == AMMG_MODE_PLAY)
    {
        adpcm_map.maga.mode = AMMG_MODE_FIRST_FOUT;
        adpcm_map.maga.para.file_no = 0;
        adpcm_map.maga.use = 0;
        adpcm_map.ghost.use = 0;
        adpcm_map.autog.use = 0;
        adpcm_map.hiso.use = 0;
        adpcm_map.event.use = 0;
    }
}
