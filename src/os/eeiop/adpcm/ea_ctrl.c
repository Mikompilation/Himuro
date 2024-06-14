#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ea_ctrl.h"
#include "main/glob.h"
#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_map.h"
#include "os/eeiop/adpcm/ea_hiso.h"
#include "os/eeiop/adpcm/ea_furn.h"
#include "os/eeiop/adpcm/ea_ghost.h"
#include "os/eeiop/adpcm/ea_gdead.h"
#include "os/eeiop/adpcm/ea_autog.h"
#include "os/eeiop/adpcm/ea_event.h"
#include "os/eeiop/adpcm/ea_tape.h"
#include "os/eeiop/adpcm/ea_soul.h"
#include "os/eeiop/adpcm/ea_puzzle.h"
#include "os/eeiop/adpcm/ea_shinkan.h"
#include "os/eeiop/adpcm/ea_magatoki.h"
#include "os/eeiop/adpcm/ea_scene.h"
#include "os/eeiop/adpcm/ea_gameover.h"
#include "os/eeiop/adpcm/ea_btlmode.h"
#include "os/eeiop/eeiop.h"

ADPCM_MAP adpcm_map = {0};
static u_char fout_flg;

static u_char UpdateAdpcmMenuFade();

#define SPU2_MAX_VOL 0x3fff

void AdpcmMapCtrlInit()
{
    memset(&adpcm_map, 0, sizeof(ADPCM_MAP));
    adpcm_map.btlmode.para.file_no = -1;
    adpcm_map.gover.para.file_no = -1;
    adpcm_map.scene.para.file_no = -1;
    adpcm_map.maga.para.file_no = -1;
    adpcm_map.shinkan.para.file_no = -1;
    adpcm_map.puzzle.para.file_no = -1;
    adpcm_map.soul.para.file_no = -1;
    adpcm_map.tape.para.file_no = -1;
    adpcm_map.gdead.para.file_no = -1;
    adpcm_map.event.para.file_no = -1;
    adpcm_map.ghost.para.file_no = -1;
    adpcm_map.autog.para.file_no = -1;
    adpcm_map.hiso.para.file_no = -1;
    adpcm_map.furn.para.file_no = -1;
    adpcm_map.map.para.file_no = -1;
    adpcm_map.mvol = 0xfff;
    adpcm_map.tgt_vol = 0xfff;
    adpcm_map.deg = 128;
    // scale volume to SPU2 range of 0...0x3fff.
    // volume is stored in range of 0...0x1000.
    SetIopCmdSm(35, opt_wrk.bgm_vol * SPU2_MAX_VOL / 0x1000, 0, 0);
}

void AdpcmMapUse()
{
    adpcm_map.map.use = 1;
}

void AdpcmMapNoUse(void)
{
    adpcm_map.map.use = 0;
}

void AdpcmMapCtrl()
{
    adpcm_map.m_flg = 1;
    
    if (adpcm_map.outmode == 0)
    {
        if (adpcm_map.movie != 0)
        {
            if (adpcm_map.mode != 16)
            {
                adpcm_map.mode = 16;
            }
        }
        else if (adpcm_map.btlmode.use != 0)
        {
                adpcm_map.m_flg = 0;
                EAdpcmBtlmodeMain();
        }
        else if (adpcm_map.gover.use != 0)
        {
            adpcm_map.m_flg = 0;
            EAdpcmGoverMain();
        }
        else if (adpcm_map.scene.use != 0)
        {
            adpcm_map.m_flg = 0;
            EAdpcmSceneMain();
        }
        else if (adpcm_map.maga.use != 0)
        {
            EAdpcmMagatokiMain();
        }
        else if (adpcm_map.shinkan.use != 0)
        {
            EAdpcmShinkanMain();
        }
        else if (adpcm_map.puzzle.use != 0)
        {
            EAdpcmPuzzleMain();
        }
        else if (adpcm_map.soul.use != 0)
        {
            EAdpcmSoulMain();
        }
        else if (adpcm_map.tape.use != 0)
        {
            adpcm_map.m_flg = 0;
            EAdpcmTapeMain();
        }
        else if (adpcm_map.gdead.use != 0)
        {
            EAdpcmGdeadMain();
        }
        else if (adpcm_map.event.use != 0)
        {
            adpcm_map.m_flg = 0;
            EAdpcmEventMain();
        }
        else if (adpcm_map.ghost.use != 0)
        {
            EAdpcmGhostMain();
        }
        else if (adpcm_map.autog.use != 0)
        {
            adpcm_map.m_flg = 0;
            EAdpcmAutogMain();
        }
        else if (adpcm_map.hiso.use != 0)
        {
            adpcm_map.m_flg = 0;
            EAdpcmHisoMain();
        }
        else if (adpcm_map.furn.use != 0)
        {
            EAdpcmFurnMain();
        }
        else if (adpcm_map.map.use != 0)
        {
            EAdpcmMapMain();
        }
    }

    if (UpdateAdpcmMenuFade() && adpcm_map.m_flg == 1)
    {
        u_int vol;

        vol = ((opt_wrk.bgm_vol * 0x3fff / 0x1000) * adpcm_map.mvol) / 0xfff & 0xffff;

        if (0x3fff < vol)
        {
            vol = 0x3fff;
        }

        SetIopCmdSm(35, vol, 0, 0);
    }
    else if (adpcm_map.m_flg == 2)
    {
        SetIopCmdSm(35, opt_wrk.bgm_vol * 0x3fff / 0x1000, 0, 0);
    }
}

void AdpcmShiftMovie()
{
    EAdpcmCmdStop(0, 0, 0);
    adpcm_map.movie = 1;
}

void AdpcmReturnFromMovie()
{
    EAdpcmCmdInit(1);
    adpcm_map.movie = 0;
}

static u_char UpdateAdpcmMenuFade()
{
    u_char update_flg;
    
    update_flg = 0;

    if (adpcm_map.mvol < adpcm_map.tgt_vol)
    {
        
        if (adpcm_map.tgt_vol > adpcm_map.deg + adpcm_map.mvol)
        {
            adpcm_map.mvol = adpcm_map.deg + adpcm_map.mvol;
        }
        else
        {
            adpcm_map.mvol = adpcm_map.tgt_vol;
        }
        
        update_flg = 1;
    }
    else if (adpcm_map.mvol > adpcm_map.tgt_vol)
    {
        
        if (adpcm_map.mvol - adpcm_map.deg < adpcm_map.tgt_vol)
        {
            adpcm_map.mvol = adpcm_map.tgt_vol;
        }
        else
        {
            adpcm_map.mvol = adpcm_map.mvol - adpcm_map.deg;
        }
        
        update_flg = 1;
    }
    
    return update_flg;
}

void SetTargetVolAdpcmMenuFade(int tgt_vol)
{
    if (tgt_vol < 0)
    {
        tgt_vol = 0;
    }
    else if (tgt_vol > 0xfff)
    {
        tgt_vol = 0xfff;
    }
    
    adpcm_map.tgt_vol = tgt_vol;
}

void EAdpcmFadeOut(u_short fout_flm)
{
    EAdpcmCmdStop(0, 0, fout_flm);
    fout_flg = 0;
}

u_char IsEndAdpcmFadeOut()
{

    if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
    {
        return 1;
    }
        
    return 0;
}

void EAdpcmFadeOutGameEnd(u_short fout_flm)
{
    EAdpcmCmdStop(0, 0, fout_flm);
    AdpcmMapCtrlInit();
    adpcm_map.map.use = 0;
}

u_char IsHighModeUse(u_char my_mode)
{
    switch(my_mode) {
    case ADPCM_MODE_MAP:
        if (
            adpcm_map.furn.use    ||
            adpcm_map.hiso.use    ||
            adpcm_map.autog.use   ||
            adpcm_map.ghost.use   ||
            adpcm_map.event.use   ||
            adpcm_map.gdead.use   ||
            adpcm_map.tape.use    ||
            adpcm_map.soul.use    ||
            adpcm_map.puzzle.use  ||
            adpcm_map.shinkan.use ||
            adpcm_map.maga.use    ||
            adpcm_map.scene.use   ||
            adpcm_map.gover.use   ||
            adpcm_map.btlmode.use ||
            adpcm_map.movie
        ) return 1;
    break;
    case ADPCM_MODE_FURN:
        if (
            adpcm_map.hiso.use    ||
            adpcm_map.autog.use   ||
            adpcm_map.ghost.use   ||
            adpcm_map.event.use   ||
            adpcm_map.gdead.use   ||
            adpcm_map.tape.use    ||
            adpcm_map.soul.use    ||
            adpcm_map.puzzle.use  ||
            adpcm_map.shinkan.use ||
            adpcm_map.maga.use    ||
            adpcm_map.scene.use   ||
            adpcm_map.gover.use   ||
            adpcm_map.btlmode.use ||
            adpcm_map.movie
        ) return 1;
    break;
    case ADPCM_MODE_HISO:
        if (
            adpcm_map.autog.use   ||
            adpcm_map.ghost.use   ||
            adpcm_map.event.use   ||
            adpcm_map.gdead.use   ||
            adpcm_map.tape.use    ||
            adpcm_map.soul.use    ||
            adpcm_map.puzzle.use  ||
            adpcm_map.shinkan.use ||
            adpcm_map.maga.use    ||
            adpcm_map.scene.use   ||
            adpcm_map.gover.use   ||
            adpcm_map.btlmode.use ||
            adpcm_map.movie
        ) return 1;
    break;
    case ADPCM_MODE_AUTOG:
        if (
            adpcm_map.ghost.use   ||
            adpcm_map.event.use   ||
            adpcm_map.gdead.use   ||
            adpcm_map.tape.use    ||
            adpcm_map.soul.use    ||
            adpcm_map.puzzle.use  ||
            adpcm_map.shinkan.use ||
            adpcm_map.maga.use    ||
            adpcm_map.scene.use   ||
            adpcm_map.gover.use   ||
            adpcm_map.btlmode.use ||
            adpcm_map.movie
        ) return 1;
    break;
    case ADPCM_MODE_GHOST:
        if (
            adpcm_map.event.use   ||
            adpcm_map.gdead.use   ||
            adpcm_map.tape.use    ||
            adpcm_map.soul.use    ||
            adpcm_map.puzzle.use  ||
            adpcm_map.shinkan.use ||
            adpcm_map.maga.use    ||
            adpcm_map.scene.use   ||
            adpcm_map.gover.use   ||
            adpcm_map.btlmode.use ||
            adpcm_map.movie
        ) return 1;
    break;
    case ADPCM_MODE_EVENT:
        if (
            adpcm_map.gdead.use   ||
            adpcm_map.tape.use    ||
            adpcm_map.soul.use    ||
            adpcm_map.puzzle.use  ||
            adpcm_map.shinkan.use ||
            adpcm_map.maga.use    ||
            adpcm_map.scene.use   ||
            adpcm_map.gover.use   ||
            adpcm_map.btlmode.use ||
            adpcm_map.movie
        ) return 1;
    break;
    case ADPCM_MODE_GDEAD:
        if (
            adpcm_map.tape.use    ||
            adpcm_map.soul.use    ||
            adpcm_map.puzzle.use  ||
            adpcm_map.shinkan.use ||
            adpcm_map.maga.use    ||
            adpcm_map.scene.use   ||
            adpcm_map.gover.use   ||
            adpcm_map.btlmode.use ||
            adpcm_map.movie
        ) return 1;
    // break; // no break here!
    case ADPCM_MODE_TAPE:
        if (
            adpcm_map.soul.use    ||
            adpcm_map.puzzle.use  ||
            adpcm_map.shinkan.use ||
            adpcm_map.maga.use    ||
            adpcm_map.scene.use   ||
            adpcm_map.gover.use   ||
            adpcm_map.btlmode.use ||
            adpcm_map.movie
        ) return 1;
    break;
    case ADPCM_MODE_SOUL:
        if (
            adpcm_map.puzzle.use  ||
            adpcm_map.shinkan.use ||
            adpcm_map.maga.use    ||
            adpcm_map.scene.use   ||
            adpcm_map.gover.use   ||
            adpcm_map.btlmode.use ||
            adpcm_map.movie
        ) return 1;
    break;
    case ADPCM_MODE_PUZZLE:
        if (
            adpcm_map.shinkan.use ||
            adpcm_map.maga.use    ||
            adpcm_map.scene.use   ||
            adpcm_map.gover.use   ||
            adpcm_map.btlmode.use ||
            adpcm_map.movie
        ) return 1;
    break;
    case ADPCM_MODE_SHINKAN:
        if (
            adpcm_map.maga.use    ||
            adpcm_map.scene.use   ||
            adpcm_map.gover.use   ||
            adpcm_map.btlmode.use ||
            adpcm_map.movie
        ) return 1;
    break;
    case ADPCM_MODE_MAGATOKI:
        if (
            adpcm_map.scene.use   ||
            adpcm_map.gover.use   ||
            adpcm_map.btlmode.use ||
            adpcm_map.movie
        ) return 1;
    break;
    case ADPCM_MODE_SCENE:
        if (
            adpcm_map.gover.use   ||
            adpcm_map.btlmode.use ||
            adpcm_map.movie
        ) return 1;
    break;
    case ADPCM_MODE_GOVER:
        if (
            adpcm_map.btlmode.use ||
            adpcm_map.movie
        ) return 1;
    break;
    case ADPCM_MODE_BTLMODE:
        if (
            adpcm_map.movie
        ) return 1;
    break;
    case ADPCM_MODE_MOVIE:
        return 0;
    }
    
    return 0;
}
