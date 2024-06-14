#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ea_gameover.h"

#include "os/eeiop/adpcm/ea_ctrl.h"
#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_dat.h"

#define DEFAULT_PITCH 0xFFF

void EAdpcmGoverMain(void)
{  
    if (adpcm_map.mode != ADPCM_MODE_GOVER) 
    {
        adpcm_map.mode = ADPCM_MODE_GOVER;
        adpcm_map.gover.mode = GAMEOVER_INIT;
    }

    switch (adpcm_map.gover.mode)
    {
        case GAMEOVER_INIT:
            EAdpcmCmdStop(0, 0, 0x50);
            adpcm_map.gover.mode = GAMEOVER_WHITE_OUT; 
        break;
        case GAMEOVER_WHITE_OUT:
            if (EAGetRetStat() == 1 || EAGetRetStat() == 2) 
            {
                EAdpcmCmdPlay(0, 0, adpcm_map.gover.para.file_no, 0, adpcm_map.gover.para.vol, adpcm_map.gover.para.pan, adpcm_map.gover.para.pitch, adpcm_map.gover.para.fin_flm);
                adpcm_map.gover.mode = GAMEOVER_LOAD_WAIT;
            }
        break;
        case GAMEOVER_LOAD_WAIT:
            if (EAGetRetStat() > 5) 
            {
                adpcm_map.gover.mode = GAMEOVER_WHITE_IN;
            }
        break;
        case GAMEOVER_WHITE_IN:
            if (EAGetRetStat() == 1 || EAGetRetStat() == 2) 
            {
                adpcm_map.gover.use = GAMEOVER_INIT;
                AdpcmMapCtrlInit();
            }
    }
}

void AdpcmPlayGameOver(int file_no, u_short vol, int fade_flm)
{
    adpcm_map.gover.para.file_no = file_no;
    adpcm_map.gover.para.vol = GetAdpcmVol(file_no);
    adpcm_map.gover.para.fin_flm = (u_short)fade_flm;
    adpcm_map.gover.para.pan = 0x280;
    adpcm_map.gover.para.pitch = DEFAULT_PITCH;
    adpcm_map.gover.use = 1;
    adpcm_map.gover.mode = GAMEOVER_INIT;
    adpcm_map.mode = ADPCM_MODE_GOVER;
}

void AdpcmStopGameOver(int fout_flm)
{
    if (adpcm_map.gover.use) 
    {
      EAdpcmCmdStop(0, 0, fout_flm);
      adpcm_map.gover.mode = GAMEOVER_WHITE_IN;
    }
}