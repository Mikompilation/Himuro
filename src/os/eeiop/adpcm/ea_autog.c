#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ea_autog.h"

#include "main/glob.h"
#include "common/ul_math.h"
#include "graphics/graph3d/sglib.h"
#include "os/eeiop/se_cmd.h"
#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_ctrl.h"

static void EadpcmUpdateAutog(ADPCM_TUNE_PARAM *atpp);
static void EAdpcmAutogParaSet(ADPCM_TUNE_PARAM *atpp);

void AdpcmPlayAutoGhost(int file_no, sceVu0FVECTOR *mpos, u_short vol, u_char ewrk_no, int fade_flm)
{
    if (adpcm_map.autog.use != 0)
    {
        if (adpcm_map.mode == ADPCM_MODE_AUTOG)
        {
            adpcm_map.autog.para.mpos = mpos;
            adpcm_map.autog.para.file_no = file_no;
            adpcm_map.autog.para.vol = GetAdpcmVol(file_no);
            adpcm_map.autog.para.ewrk_no = ewrk_no;
            adpcm_map.autog.para.fin_flm = fade_flm;
            adpcm_map.autog.mode = AMAG_MODE_PRE_FADE_OUT;
        }
    }
    else
    {
        adpcm_map.autog.para.mpos = mpos;
        adpcm_map.autog.para.file_no = file_no;
        adpcm_map.autog.para.vol = GetAdpcmVol(file_no);
        adpcm_map.autog.para.ewrk_no = ewrk_no;
        adpcm_map.autog.para.fin_flm = fade_flm;
        adpcm_map.autog.use = 1;
#ifdef BUILD_EU_VERSION
        InitSubtitlesSys();
        SetSubtitlesNCntOne(3, file_no);
#endif
    }
}

void AdpcmStopAutoGhost(int fout_flm)
{    
    if (adpcm_map.autog.use != 0 && IsHighModeUse(4) != 0)
    {
        adpcm_map.autog.use = 0;
        adpcm_map.hiso.use = 0;
    }
}

void EAdpcmAutogMain()
{
    if (adpcm_map.mode != ADPCM_MODE_AUTOG)
    {
        if (
            adpcm_map.mode == ADPCM_MODE_MAP ||
            adpcm_map.mode == ADPCM_MODE_FURN
        )
        {
            if (adpcm_map.autog.mode == AMAG_MODE_END)
            {
                adpcm_map.autog.mode = AMAG_MODE_REQ_STOP;
            }
            else
            {
                adpcm_map.autog.mode = AMAG_MODE_PRE_FADE_OUT;
            }
        }
        else
        {
            if (adpcm_map.autog.mode == AMAG_MODE_END)
            {
                adpcm_map.autog.mode = AMAG_MODE_REQ_STOP;
            }
            else
            {
                EAdpcmCmdStop(0, 0, 0);
                adpcm_map.autog.mode = AMAG_MODE_REQ_WAIT_STOP;
                adpcm_map.autog.count = 0;
            }
        }
        
        adpcm_map.mode = ADPCM_MODE_AUTOG;
    }
    
    switch(adpcm_map.autog.mode)
    {
    case AMAG_MODE_PRE_FADE_OUT:
        EAdpcmCmdStop(0, 0, 0x1e);
        
        adpcm_map.autog.mode = AMAG_MODE_REQ_WAIT_STOP;
        adpcm_map.autog.count = 0;
    break;
    case AMAG_MODE_REQ_WAIT_STOP:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            EadpcmUpdateAutog(&adpcm_map.autog.para);
            adpcm_map.mpara = adpcm_map.autog.para;
            EAdpcmAutogParaSet(&adpcm_map.mpara);
            adpcm_map.m_flg = 2;
            EAdpcmCmdPlay(0, 0, adpcm_map.mpara.file_no, 0, adpcm_map.mpara.vol, adpcm_map.mpara.pan, adpcm_map.mpara.pitch, 0);
            adpcm_map.autog.mode = AMAG_MODE_REQ_PLAY;
            adpcm_map.mvol = 0xfff;
        }
        else
        {
            adpcm_map.autog.count++;
            
            if (adpcm_map.autog.count == 200)
            {
                EAdpcmCmdStop(0, 0, 0);
            }
            else if (adpcm_map.autog.count > 500)
            {
                EAdpcmCmdStop(0, 0, 0);
                adpcm_map.autog.count = 0;
                adpcm_map.autog.use = 0;
                adpcm_map.hiso.use = 0;
            }
        }
    break;
    case AMAG_MODE_REQ_PLAY:
        if (EAGetRetStat() == 2)
        {
            EAdpcmCmdPos(0, adpcm_map.mpara.file_no, 0, 0x280, 0xfff);
            adpcm_map.autog.use = 0;
            adpcm_map.hiso.use = 0;
        }
        else
        {
            EadpcmUpdateAutog(&adpcm_map.autog.para);
            adpcm_map.mpara = adpcm_map.autog.para;
            EAdpcmAutogParaSet(&adpcm_map.mpara);
            EAdpcmCmdPos(0, adpcm_map.mpara.file_no, adpcm_map.mpara.vol, adpcm_map.mpara.pan, adpcm_map.mpara.pitch);
        }
        
        adpcm_map.mvol = 0xfff;
    break;
    case AMAG_MODE_REQ_STOP:
        EAdpcmCmdStop(0, 0, adpcm_map.autog.para.fout_flm);
        adpcm_map.autog.mode = AMAG_MODE_END;
        adpcm_map.mvol = 0xfff;
    break;
    case AMAG_MODE_END:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.autog.mode = AMAG_MODE_PRE_FADE_OUT;
            adpcm_map.autog.use = 0;
            adpcm_map.hiso.use = 0;
        }
        
        adpcm_map.mvol = 0xfff;
    break;
    }
}

static void EadpcmUpdateAutog(ADPCM_TUNE_PARAM *atpp)
{
    if (ene_wrk[atpp->ewrk_no].sta & 1)
    {
        atpp->pos[0] = atpp->mpos[0][0];
        atpp->pos[1] = atpp->mpos[0][1];
        atpp->pos[2] = atpp->mpos[0][2];
    }
}

static void EAdpcmAutogParaSet(ADPCM_TUNE_PARAM *atpp)
{
    static int cnt = 0;
    float rot_cam;
    float rot_oc;
    float dist_obj;
    
    if (atpp)
    {
        rot_oc = SgAtan2f(camera.i[0] - camera.p[0], camera.i[2] - camera.p[2]);
        rot_cam = SgAtan2f(atpp->pos[0] - camera.p[0], atpp->pos[2] - camera.p[2]);
        rot_cam = SeCmdGetAngle(rot_cam, rot_oc);
        dist_obj = GetDistV(atpp->pos, plyr_wrk.move_box.pos);
        dist_obj = GetDist(dist_obj, atpp->pos[1] - plyr_wrk.move_box.pos[1]);
        atpp->pan = EAdpcmGetPan(rot_cam);
        atpp->vol = EAdpcmGetVol(dist_obj, adpcm_map.autog.para.vol);
        cnt++;
        atpp->pitch = 0xfff;
        if (cnt % 0x1e == 0)
        {
            cnt = 0;
        }
    }
}
