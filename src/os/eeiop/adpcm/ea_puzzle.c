#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ea_puzzle.h"

#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_dat.h"
#include "os/eeiop/adpcm/ea_ctrl.h"

void EAdpcmPuzzleMain()
{
    if (adpcm_map.mode != ADPCM_MODE_PUZZLE)
    {
        adpcm_map.mode = ADPCM_MODE_PUZZLE;
        adpcm_map.puzzle.para.file_no = -1;
    }
    
    switch(adpcm_map.puzzle.mode)
    {
    case AMPZ_MODE_WAIT1:
        EAdpcmCmdStop(0, 0, adpcm_map.puzzle.para.fout_flm);
        adpcm_map.puzzle.mode = AMPZ_MODE_WAIT2;
    break;
    case AMPZ_MODE_WAIT2:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.puzzle.mode = AMPZ_MODE_WAIT3;
        }
    break;
    case AMPZ_MODE_WAIT3:
        // do nothing ...
    break;
    case AMPZ_MODE_PLAY:
        if (EAGetRetStat() > 5)
        {
            adpcm_map.puzzle.mode = AMPZ_MODE_END_WAIT;
        }
    break;
    case AMPZ_MODE_END_WAIT:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.puzzle.mode = AMPZ_MODE_WAIT3;
            adpcm_map.puzzle.para.file_no = -1;
        }
    break;
    }
}

void AdpcmPrePuzzleFadeOut(u_short fout_flm)
{
    adpcm_map.puzzle.mode = AMPZ_MODE_WAIT1;
    adpcm_map.puzzle.para.fout_flm = fout_flm;
    adpcm_map.puzzle.use = 1;
}

u_char IsEndAdpcmPuzzle()
{
    return adpcm_map.puzzle.mode == AMPZ_MODE_WAIT3;
}

void AdpcmPreLoadPuzzle(int file_no)
{
    adpcm_map.puzzle.para.file_no = file_no;
    EAdpcmCmdPreload(0, file_no, 0, 0);
    adpcm_map.puzzle.mode = AMPZ_MODE_WAIT3;
}

u_char IsPreLoadEndAdpcmPuzzle()
{
    if (EAGetRetTune() == adpcm_map.puzzle.para.file_no)
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

void AdpcmPuzzlePreLoadEndPlay()
{
    EAdpcmCmdPlay(0, 0, adpcm_map.puzzle.para.file_no, 0, GetAdpcmVol(adpcm_map.puzzle.para.file_no), 0x280, 0xfff, 0);
    adpcm_map.puzzle.mode = AMPZ_MODE_PLAY;
}

void AdpcmPuzzlePlay(int file_no)
{
    adpcm_map.puzzle.para.file_no = file_no;
    EAdpcmCmdPlay(0, 0, adpcm_map.puzzle.para.file_no, 0, GetAdpcmVol(file_no), 0x280, 0xfff, 0);
    adpcm_map.puzzle.mode = AMPZ_MODE_PLAY;
}

void AdpcmPuzzleEnd()
{
    EAdpcmCmdStop(0, 0, 0x1e);
    adpcm_map.puzzle.use = 0;
    adpcm_map.autog.use = 0;
    adpcm_map.hiso.use = 0;
    adpcm_map.event.use = 0;
    adpcm_map.puzzle.mode = AMPZ_MODE_WAIT1;
}
