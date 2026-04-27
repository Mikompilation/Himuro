#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ea_ghost.h"

#include "main/glob.h"
#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_dat.h"
#include "os/eeiop/adpcm/ea_ctrl.h"
#include "graphics/graph3d/sglib.h"
#include "os/eeiop/se_cmd.h"
#include "common/ul_math.h"

#define DEFAULT_PITCH 0xfff

static void EAdpcmGhostParaSet(ADPCM_TUNE_PARAM *atpp);

void AdpcmPlayGhost(int file_no, sceVu0FVECTOR *mpos, u_short vol, u_char ewrk_no, int fade_flm)
{
    if (adpcm_map.ghost.use == 0)
    {
        adpcm_map.ghost.para.mpos = mpos;
        adpcm_map.ghost.para.file_no = file_no;
        adpcm_map.ghost.para.vol = GetAdpcmVol(file_no);
        adpcm_map.ghost.para.ewrk_no = ewrk_no;
        adpcm_map.ghost.para.fin_flm = fade_flm;
        adpcm_map.ghost.use = 1;
    }
}

void AdpcmStopGhost(int fout_flm)
{
    if (adpcm_map.ghost.use != 0)
    {
        if (IsHighModeUse(ADPCM_MODE_GHOST))
        {
            adpcm_map.ghost.use = 0;
            adpcm_map.autog.use = 0;
            adpcm_map.hiso.use = 0;
        }
        else
        {
            adpcm_map.ghost.para.fout_flm = fout_flm;
            adpcm_map.ghost.mode = AMG_MODE_REQ_STOP;
        }
    }
}

void EAdpcmGhostMain()
{
    static int wait_cnt;

    if (adpcm_map.mode != ADPCM_MODE_GHOST)
    {
        if (
            adpcm_map.mode == ADPCM_MODE_MAP ||
            adpcm_map.mode == ADPCM_MODE_FURN ||
            adpcm_map.mode == ADPCM_MODE_AUTOG
        )
        {
            adpcm_map.ghost.mode = AMG_MODE_PRE_FADE_OUT;
        }
        else if (adpcm_map.ghost.mode == AMG_MODE_END)
        {
            adpcm_map.ghost.mode = AMG_MODE_REQ_STOP;
        }
        else
        {
            adpcm_map.ghost.mode = AMG_MODE_PRE_FADE_OUT;
        }

        adpcm_map.mode = ADPCM_MODE_GHOST;
    }

    switch(adpcm_map.ghost.mode)
    {
    case AMG_MODE_PRE_FADE_OUT:
        EAdpcmCmdStop(0, 0, 30);

        adpcm_map.ghost.mode = AMG_MODE_REQ_WAIT_STOP;
    break;
    case AMG_MODE_REQ_WAIT_STOP:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.mpara = adpcm_map.ghost.para;

            EAdpcmGhostParaSet(&adpcm_map.mpara);
            EAdpcmCmdPlay(0, 1, adpcm_map.mpara.file_no, 0, adpcm_map.mpara.vol, adpcm_map.mpara.pan, adpcm_map.mpara.pitch, 0);

            adpcm_map.ghost.mode = AMG_MODE_REQ_PLAY;
        }
    break;
    case AMG_MODE_REQ_PLAY:
        adpcm_map.mpara = adpcm_map.ghost.para;

        EAdpcmGhostParaSet(&adpcm_map.mpara);

        adpcm_map.m_flg = 0;

        EAdpcmCmdPos(0, adpcm_map.mpara.file_no, adpcm_map.mpara.vol * adpcm_map.mvol / DEFAULT_PITCH, adpcm_map.mpara.pan,adpcm_map.mpara.pitch);
    return;
    case AMG_MODE_REQ_STOP:
        EAdpcmCmdStop(0, 0, adpcm_map.ghost.para.fout_flm);

        adpcm_map.ghost.mode = AMG_MODE_END;
    break;
    case AMG_MODE_END:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.ghost.mode = AMG_MODE_PRE_FADE_OUT;
            adpcm_map.ghost.use = 0;
            adpcm_map.autog.use = 0;
            adpcm_map.hiso.use = 0;
        }
    break;
    }
}

static void EAdpcmGhostParaSet(ADPCM_TUNE_PARAM *atpp)
{
    static int cnt = 0;
    float rot_cam;
    float rot_oc;
    float dist_obj;

    if (atpp)
    {
#if defined(BUILD_JP_VERSION)
        rot_oc = atan2f(camera.i[0] - camera.p[0], camera.i[2] - camera.p[2]);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        rot_oc = SgAtan2f(camera.i[0] - camera.p[0], camera.i[2] - camera.p[2]);
#endif

        if (ene_wrk[atpp->ewrk_no].sta & 0x1)
        {
            atpp->pos[0] = atpp->mpos[0][0];
            atpp->pos[1] = atpp->mpos[0][1];
            atpp->pos[2] = atpp->mpos[0][2];
        }

#if defined(BUILD_JP_VERSION)
        rot_cam = atan2f(atpp->pos[0] - camera.p[0], atpp->pos[2] - camera.p[2]);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        rot_cam = SgAtan2f(atpp->pos[0] - camera.p[0], atpp->pos[2] - camera.p[2]);
#endif

        rot_cam = SeCmdGetAngle(rot_cam, rot_oc);
        dist_obj = GetDist(GetDistV(atpp->pos, plyr_wrk.move_box.pos), atpp->pos[1] - plyr_wrk.move_box.pos[1]);

        atpp->pan = EAdpcmGetPan(rot_cam);
        atpp->vol = EAdpcmGetVol(dist_obj, atpp->vol);
        atpp->pitch = EAdpcmGetPitch(rot_cam, dist_obj, 0x1000);

        cnt++;

        if (cnt % 30 == 0)
        {
            cnt = 0;
        }
    }
}
