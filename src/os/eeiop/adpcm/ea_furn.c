#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ea_furn.h"

#include "common/ul_math.h"
#include "graphics/graph3d/sglib.h"
#include "main/glob.h"
#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_ctrl.h"
#include "os/eeiop/adpcm/ea_dat.h"
#include "os/eeiop/se_cmd.h"

static void EAdpcmFurnParaSet(ADPCM_TUNE_PARAM *atpp);

void EAdpcmFurnMain()
{
    if (adpcm_map.mode != ADPCM_MODE_FURN)
    {
        if (adpcm_map.mode == ADPCM_MODE_MAP)
        {
            adpcm_map.furn.mode = AMFR_MODE_PRE_FADE_OUT;
        }
        else if (adpcm_map.furn.mode == AMFR_MODE_END)
        {
            adpcm_map.furn.mode = AMFR_MODE_REQ_STOP;
        }
        else
        {
            adpcm_map.furn.mode = AMFR_MODE_PRE_FADE_OUT;
        }

        adpcm_map.mode = ADPCM_MODE_FURN;
    }

    switch(adpcm_map.furn.mode)
    {
    case AMFR_MODE_PRE_FADE_OUT:
        EAdpcmCmdStop(0, 0, 30);

        adpcm_map.furn.mode = AMFR_MODE_REQ_WAIT_STOP;
    break;
    case AMFR_MODE_REQ_WAIT_STOP:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.mpara = adpcm_map.furn.para;

            EAdpcmFurnParaSet(&adpcm_map.mpara);
            EAdpcmCmdPlay(0, 1, adpcm_map.mpara.file_no, 0, adpcm_map.mpara.vol, adpcm_map.mpara.pan, adpcm_map.mpara.pitch, 0);

            adpcm_map.furn.mode = AMFR_MODE_REQ_PLAY;
        }
    break;
    case AMFR_MODE_REQ_PLAY:
        adpcm_map.mpara = adpcm_map.furn.para;

        EAdpcmFurnParaSet(&adpcm_map.mpara);

        adpcm_map.m_flg = 0;

        EAdpcmCmdPos(0, adpcm_map.mpara.file_no, (adpcm_map.mpara.vol * adpcm_map.mvol) / 0xfff, adpcm_map.mpara.pan, adpcm_map.mpara.pitch);
    break;
    case AMFR_MODE_REQ_STOP:
        EAdpcmCmdStop(0, 0, adpcm_map.furn.para.fout_flm);

        adpcm_map.furn.mode = AMFR_MODE_END;
    break;
    case AMFR_MODE_END:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.furn.mode = AMFR_MODE_PRE_FADE_OUT;
            adpcm_map.furn.use = 0;
        }
    break;
    }
}

static void EAdpcmFurnParaSet(ADPCM_TUNE_PARAM *atpp)
{
    static int cnt = 0;
    float rot_cam;
    float rot_oc;
    float dist_obj;

    if (atpp)
    {
        rot_oc = VER_ATAN2F(camera.i[0] - camera.p[0], camera.i[2] - camera.p[2]);
        rot_cam = VER_ATAN2F(atpp->pos[0] - camera.p[0], atpp->pos[2] - camera.p[2]);

        rot_cam = SeCmdGetAngle(rot_cam, rot_oc);

        dist_obj = GetDist(GetDistV(atpp->pos, plyr_wrk.move_box.pos), atpp->pos[1] - plyr_wrk.move_box.pos[1]);

        atpp->pan = EAdpcmGetPan(rot_cam);
        atpp->vol = EAdpcmGetVol(dist_obj, atpp->vol);

        cnt++;

        atpp->pitch = 0xfff;

        if (cnt % 30 == 0)
        {
            cnt = 0;
        }
    }
}

void AdpcmPlayFurn(int file_no, float *pos, u_short vol, int fade_flm)
{
    if (adpcm_map.furn.use != 0)
    {
        return;
    }

    switch(file_no)
    {
    case FSPE_AE_TAPE:
        adpcm_map.furn.para.file_no = AE000_TAPE_STR;
    break;
    case FSPE_AE_KOTO:
        adpcm_map.furn.para.file_no = AE002_KOTO_STR;
    break;
#if defined(BUILD_JP_VERSION)
    case FSPE_AE_CHIKUON:
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    case FSPE_AE_CHIKUON:
#endif
        adpcm_map.furn.para.file_no = AE003_CHIKUONKI_STR;
    break;
    case FSPE_AE_OKYO:
        adpcm_map.furn.para.file_no = AE004_AYASHII_STR;
    break;
    case FSPE_AE_OKYO_L:
        adpcm_map.furn.para.file_no = AK006_AME_STR;
    break;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    case FSPE_AE_DOKO:
        adpcm_map.furn.para.file_no = AVMC021_DOKONIIKUNO_STR;
    break;
    case FSPE_AE_SAMUI:
        adpcm_map.furn.para.file_no = AVMC022_SAMUI_STR;
    break;
    case FSPE_AE_MITA:
        adpcm_map.furn.para.file_no = AVMC023_MITAWAYO_STR;
    break;
    case FSPE_AE_KAESHITE:
        adpcm_map.furn.para.file_no = AVMC024_KAESHITE_STR;
    break;
    case FSPE_AE_KOKONI:
        adpcm_map.furn.para.file_no = AVMC025_KOKONIITE_STR;
    break;
    case FSPE_AE_NE:
        adpcm_map.furn.para.file_no = AVMC026_NE_STR;
    break;
    case FSPE_AE_DOUSHITE:
        adpcm_map.furn.para.file_no = AVMC027_DOUSHITE_STR;
    break;
    case FSPE_AE_NAWAGA:
        adpcm_map.furn.para.file_no = AVMC028_NAWAGA_STR;
    break;
    case FSPE_AE_SUSURI1:
        adpcm_map.furn.para.file_no = AVMC029_SUSURI1_STR;
    break;
    case FSPE_AE_SUSURI2:
        adpcm_map.furn.para.file_no = AVMC030_SUSURI2_STR;
    break;
    case FSPE_AE_SUSURI3:
        adpcm_map.furn.para.file_no = AVMC031_SUSURI3_STR;
    break;
    case FSPE_AE_SUSURI4:
        adpcm_map.furn.para.file_no = AVMC032_SUSURI4_STR;
    break;
#endif
    default:
        adpcm_map.furn.para.file_no = AE001_USIRO_STR;
    break;
    }

    adpcm_map.furn.para.pos[0] = pos[0];
    adpcm_map.furn.para.pos[1] = pos[1];
    adpcm_map.furn.para.pos[2] = pos[2];
    adpcm_map.furn.para.pos[3] = pos[3];
    adpcm_map.furn.para.vol = GetAdpcmVol(adpcm_map.furn.para.file_no);
    adpcm_map.furn.para.fin_flm = fade_flm;
    adpcm_map.furn.use = 1;
}

void AdpcmStopFurn(int fout_flm)
{
    if (adpcm_map.furn.use != 0)
    {
        if (IsHighModeUse(2) != 0)
        {
            adpcm_map.furn.use = 0;
        }
        else
        {
            adpcm_map.furn.para.fout_flm = fout_flm;
            adpcm_map.furn.mode = AMFR_MODE_REQ_STOP;
        }
    }
}
