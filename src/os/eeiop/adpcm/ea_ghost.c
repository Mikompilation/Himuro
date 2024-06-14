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

#define DEFAULT_PITCH 0xFFF

static void EAdpcmGhostParaSet(ADPCM_TUNE_PARAM *atpp);

void AdpcmPlayGhost(int file_no, sceVu0FVECTOR *mpos, u_short vol, u_char ewrk_no, int fade_flm)
{
    if (!adpcm_map.ghost.use) 
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
    if (adpcm_map.ghost.use) 
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
    
    // Line 97
    if (adpcm_map.mode != ADPCM_MODE_GHOST)
    {
        // Line 98
        if ( adpcm_map.mode == ADPCM_MODE_MAP || adpcm_map.mode == ADPCM_MODE_FURN) 
        {
            adpcm_map.ghost.mode = AMG_MODE_PRE_FADE_OUT;
        }
        else if (adpcm_map.mode == ADPCM_MODE_AUTOG) 
        {
            adpcm_map.ghost.mode = AMG_MODE_PRE_FADE_OUT;
        }
        else 
        {
            // Line 104
            if (adpcm_map.ghost.mode == AMG_MODE_END) 
            {
                asm("");
                // Line 106
                adpcm_map.ghost.mode = AMG_MODE_REQ_STOP;
            }
            else 
            {
                adpcm_map.ghost.mode = AMG_MODE_PRE_FADE_OUT;
            }
        }
        
        // Line 112
        adpcm_map.mode = ADPCM_MODE_GHOST;
    }
    
    // Line 119
    switch(adpcm_map.ghost.mode) 
    {
        case AMG_MODE_PRE_FADE_OUT:
            // Line 121
            EAdpcmCmdStop(0, 0, 0x1e);
            
            // Line 123
            adpcm_map.ghost.mode = AMG_MODE_REQ_WAIT_STOP;
        break;
        case AMG_MODE_REQ_WAIT_STOP:
            // Line 126
            if (EAGetRetStat() == 1 || EAGetRetStat() == 2) 
            {                
                // Line 129
                adpcm_map.mpara = adpcm_map.ghost.para;   
        
                // Line 130
                EAdpcmGhostParaSet(&adpcm_map.mpara);
        
                // Line 132
                EAdpcmCmdPlay(0, 1, adpcm_map.mpara.file_no, 0, adpcm_map.mpara.vol, adpcm_map.mpara.pan, adpcm_map.mpara.pitch, 0);
                
                // Line 141
                adpcm_map.ghost.mode = AMG_MODE_REQ_PLAY;
            }
        break;
        case AMG_MODE_REQ_PLAY:
            adpcm_map.mpara = adpcm_map.ghost.para;
        
            EAdpcmGhostParaSet(&adpcm_map.mpara);
        
            // Line 146
            adpcm_map.m_flg = 0;
        
            // Line 147
            EAdpcmCmdPos(0, adpcm_map.mpara.file_no, adpcm_map.mpara.vol * adpcm_map.mvol / DEFAULT_PITCH, adpcm_map.mpara.pan,adpcm_map.mpara.pitch);
        return;
        case AMG_MODE_REQ_STOP:
            // Line 155
            EAdpcmCmdStop(0, 0, adpcm_map.ghost.para.fout_flm);
      
            // Line 157
            adpcm_map.ghost.mode = AMG_MODE_END;
        break;
        case AMG_MODE_END:
            // Line 160
            if (EAGetRetStat() == 1 || EAGetRetStat() == 2) 
            {
                // Line 162
                adpcm_map.ghost.mode = AMG_MODE_PRE_FADE_OUT;
                // Line 163
                adpcm_map.ghost.use = 0;
                // Line 164
                adpcm_map.autog.use = 0;
                // Line 165
                adpcm_map.hiso.use = 0;
            }
        break;
    }
}

static void EAdpcmGhostParaSet(ADPCM_TUNE_PARAM *atpp)
{
    /* sdata 356350 */ static int cnt = 0;
    float rot_cam;
    float rot_oc;
    float dist_obj;
  
    if (atpp) 
    {
        // Line 189
        rot_oc = SgAtan2f(camera.i[0] - camera.p[0], camera.i[2] - camera.p[2]);
        
        if (ene_wrk[atpp->ewrk_no].sta & 1) 
        {
            // Line 192
            atpp->pos[0] = atpp->mpos[0][0];
            // Line 193
            atpp->pos[1] = atpp->mpos[0][1];
            // Line 194
            atpp->pos[2] = atpp->mpos[0][2];
        }
        
        rot_cam = SgAtan2f(atpp->pos[0] - camera.p[0], atpp->pos[2] - camera.p[2]);
        
        
        rot_cam = SeCmdGetAngle(rot_cam, rot_oc);
        
        
        dist_obj = GetDistV(atpp->pos, plyr_wrk.move_box.pos);
        dist_obj = GetDist(dist_obj, atpp->pos[1] - plyr_wrk.move_box.pos[1]);
        
        
        atpp->pan = EAdpcmGetPan(rot_cam);
        
        
        atpp->vol = EAdpcmGetVol(dist_obj, atpp->vol);
        
        
        atpp->pitch = EAdpcmGetPitch(rot_cam, dist_obj, 0x1000);
        
        cnt++;

        if (cnt % 0x1e == 0) 
        {
            cnt = 0;
        }
    }
}