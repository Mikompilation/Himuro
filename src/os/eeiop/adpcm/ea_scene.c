#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ea_scene.h"

#include "os/eeiop/adpcm/ea_ctrl.h"
#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_dat.h"

// generates jump table in .rodata
void EAdpcmSceneMain()
{    
    if (adpcm_map.mode != ADPCM_MODE_SCENE)
    {
        adpcm_map.mode = ADPCM_MODE_SCENE;
    }
    
    switch(adpcm_map.scene.mode)
    {
    case EAS_MODE_WAIT1:
        EAdpcmCmdStop(0, 0, adpcm_map.scene.para.fout_flm);
        adpcm_map.scene.mode = EAS_MODE_WAIT2;
    break;
    case EAS_MODE_WAIT2:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.scene.mode = EAS_MODE_WAIT3;
        }
    break;
    case EAS_MODE_WAIT3:
        // do nothing ...
    break;
    case EAS_MODE_WAIT4:
        if (EAGetRetStat() > 5)
        {
            adpcm_map.scene.mode = EAS_MODE_WAIT5;
        }
    break;
    case EAS_MODE_WAIT5:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.scene.use = 0;
            adpcm_map.hiso.use = 0;
            adpcm_map.autog.use = 0;
            adpcm_map.event.use = 0;
            adpcm_map.scene.mode = EAS_MODE_WAIT1;
        }
    break;
    }
}

void AdpcmPreSceneFadeOut(/* a0 4 */ u_short fout_flm)
{
    adpcm_map.scene.mode = EAS_MODE_WAIT1;
    adpcm_map.scene.para.fout_flm = fout_flm;
    adpcm_map.scene.use = 1;
}


u_char IsFadeoutEndAdpcmScene()
{
    return adpcm_map.scene.mode == EAS_MODE_WAIT3;
}

void AdpcmScenePreLoadReq(/* a0 4 */ int scene_no)
{
    adpcm_map.scene.para.file_no = SCENE0010_STR + scene_no;
    adpcm_map.scene.use = 1;
    EAdpcmCmdPreload(0, adpcm_map.scene.para.file_no, 0, 0);
    adpcm_map.scene.mode = EAS_MODE_WAIT3;
}


int IsAdpcmScenePreLoadEnd()
{
    if (EAGetRetTune() == adpcm_map.scene.para.file_no)
    {
        if (EAGetRetStat() == -1)
        {
            return -1;
        }
        else if (EAGetRetStat() == 5)
        {
            return 1;
        }
    }
    return 0;
}

void AdpcmScenePreLoadEndPlay(/* a0 4 */ int scene_no)
{
    EAdpcmCmdPlay(0, 0, SCENE0010_STR + scene_no, 0, GetAdpcmVol(SCENE0010_STR + scene_no), 0x280, 0xfff, 0);
    adpcm_map.scene.mode = EAS_MODE_WAIT4;
}


void AdpcmSceneStop()
{
    if (adpcm_map.scene.use != 0)
    {
        EAdpcmCmdStop(0, 0, 0);
        adpcm_map.scene.use = 0;
        adpcm_map.hiso.use = 0;
        adpcm_map.autog.use = 0;
        adpcm_map.event.use = 0;
        adpcm_map.scene.mode = 0;
    }
}

