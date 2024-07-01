#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "se_cmd.h"

#include "os/eeiop/eese.h"
#include "graphics/graph3d/sglib.h"
#include "common/ul_math.h"
#include "os/eeiop/eeiop.h"
#include "os/eeiop/se_cmd.h"
#include "main/glob.h"
#include "ingame/plyr/unit_ctl.h"


#define SE_WRK_SIZE 24

#define PI 3.141592741f
#define HALF_PI 1.5707964f
#define FLOAT_MAX_SOUND_DISTANCE 10000.0f
#define FLOAT_RES_X 640.0f
#define US_MAX_PAN 0x4ff

void SeCtrlInit()
{
    return;
}

void SeCtrlMain()
{
    SE_WRK tmp_sw; // unused
    SE_WRK *swp;
    int cmd_flg;
    int i;
    float rot_oc;
    float dist_obj;
    float rot_cam;
    u_char stop_flg;
  
    swp = SeGetSeWrk(0);
    UpdateSeMenuFade();
    
    rot_cam = SgAtan2f(camera.i[0] - camera.p[0], camera.i[2] - camera.p[2]);

    for (i = 0; i < SE_WRK_SIZE; i++, swp++)
    {
        stop_flg = 0;
        cmd_flg = 0;
        
        if (swp->status != SEW_STAT_FREE)
        {
            if (swp->fade_mode == SE_FADE_NONE)
            {
                swp->fade_vol = swp->fade_tgt;
            }
            else if (swp->fade_mode == SE_FADE_IN)
            {
                swp->fade_vol = swp->fade_vol + swp->fade_spd;
                if (swp->fade_tgt < swp->fade_vol) 
                {
                    swp->fade_vol = swp->fade_tgt;
                    swp->fade_mode = SE_FADE_NONE;
                }
                
                cmd_flg = 1;
            }
            else if (swp->fade_mode == SE_FADE_OUT)
            {
                if (swp->fade_spd > (swp->fade_vol - swp->fade_tgt)) 
                {
                    swp->fade_vol = swp->fade_tgt;
                    swp->fade_mode = SE_FADE_NONE;
                    SeStop(i);
                    swp->status = SEW_STAT_FREE;
                    stop_flg = 1;
                }
                else 
                {
                    swp->fade_vol = swp->fade_vol - swp->fade_spd;
                    cmd_flg = 1;
                }
            }
    
            if (swp->fade_vol != swp->fade_tgt) 
            {
                swp->vol_rate = (swp->fade_vol * 0x1000) / 0xfff;
            }
            else 
            {
                swp->vol_rate = 0x1000;
            }
    
            swp->now_pitch = swp->pitch;
            
            switch(swp->atr & 7) 
            {
                case 0:
                case 3:
                    if (sm_fade.mvol != sm_fade.tgt_vol) 
                    {
                        cmd_flg++;
                    }
                break;
                case 1:
                    rot_oc = SeCmdGetAngle(SgAtan2f(swp->spos[0] - camera.p[0], swp->spos[2] - camera.p[2]), rot_cam);
                    dist_obj = GetDist(GetDistV(swp->spos, plyr_wrk.move_box.pos), swp->spos[1] - plyr_wrk.move_box.pos[1]);
                    
                    swp->pan = SeCmdGetPan(rot_oc);
                    swp->vol_phase = SeCmdGetPhase(rot_oc);        
                    swp->now_pitch = SeCmdGetPitch(rot_oc, swp->pitch);
                    swp->vol_rate = SeCmdGetVol(dist_obj, swp->vol_rate);
                    
                    if (map_wrk.now_room != swp->room_id && swp->room_id != 0xff) 
                    {
                        swp->vol_rate = swp->vol_rate >> 1;
                    }
            
                    cmd_flg++;
                break;
                case 2:
                    if (ene_wrk[swp->ene_wrk_no].sta & 1) 
                    {
                        swp->spos[0] = ((MPOS*)swp->mpos)->p1[0];
                        swp->spos[1] = ((MPOS*)swp->mpos)->p1[1];
                        swp->spos[2] = ((MPOS*)swp->mpos)->p1[2];
                    }
                    
                    rot_oc = SeCmdGetAngle(SgAtan2f(swp->spos[0] - camera.p[0], swp->spos[2] - camera.p[2]), rot_cam);
                    dist_obj = GetDist(GetDistV(swp->spos, plyr_wrk.move_box.pos), swp->spos[1] - plyr_wrk.move_box.pos[1]);
                    
                    swp->pan = SeCmdGetPan(rot_oc);
                    swp->vol_phase = SeCmdGetPhase(rot_oc);
                    swp->now_pitch = SeCmdGetPitch(rot_oc, swp->pitch);;
                    swp->vol_rate = SeCmdGetVol(dist_obj, swp->vol_rate);
                    
                    if (map_wrk.now_room != swp->room_id && swp->room_id != 0xff) 
                    {
                        swp->vol_rate = swp->vol_rate >> 1;
                    }
            
                    cmd_flg++;
                break;
                case 4:
                    swp->spos[0] = ((MPOS*)swp->mpos)->p1[0];
                    swp->spos[1] = ((MPOS*)swp->mpos)->p1[1];
                    swp->spos[2] = ((MPOS*)swp->mpos)->p1[2];
    
                    rot_oc = SeCmdGetAngle(SgAtan2f(swp->spos[0] - camera.p[0], swp->spos[2] - camera.p[2]), rot_cam);
                    
                    swp->pan = SeCmdGetPan(rot_oc);
                    swp->vol_phase = SeCmdGetPhase(rot_oc);
                    swp->now_pitch = SeCmdGetPitch(rot_oc, swp->pitch);
                    swp->vol_rate = SeCmdGetVol(0.0f, swp->vol_rate);
                    
                    if (map_wrk.now_room != swp->room_id && swp->room_id != 0xff)
                    {
                        swp->vol_rate = swp->vol_rate >> 1;
                    }
            
                    cmd_flg++;
                break;
            }
        }
        
        if (cmd_flg && !stop_flg) 
        {
            if (swp->menu) 
            {
                SetIopCmdLg(IC_SE_POS, swp->v_p, swp->se_p, swp->now_pitch, swp->pan, swp->vol_rate, 0, 0);
            }
            else 
            {
                SetIopCmdLg(IC_SE_POS, swp->v_p, swp->se_p, swp->now_pitch, swp->pan, (swp->vol_rate * sm_fade.mvol) / 0xfff, 0, 0);
            }
        }
    }
}

u_short SeCmdGetPan(float rot)
{
    float panf;
  
    panf = SgSinf(rot);
    panf = panf * FLOAT_RES_X + FLOAT_RES_X;
    
    if (US_MAX_PAN < (u_short)panf) 
    {
        return US_MAX_PAN;
    }
     
    return panf;
}

u_short SeCmdGetPhase(float rot)
{
    if (rot > HALF_PI || HALF_PI > -rot) 
    {
        return 1;
    }
    
    return 0;
}

u_short SeCmdGetPitch(float rot, u_short pitch)
{
    float fpitch_rate;
    
    if (rot < 0.0f) 
    {
        rot = -rot;
    }

    fpitch_rate = (pitch + (rot / PI) * pitch * 0.05f);

    return fpitch_rate;
}

u_short SeCmdGetVol(float dist, u_short vol)
{
    float ratio_dist;
  
    if (dist < FLOAT_MAX_SOUND_DISTANCE) 
    {
        ratio_dist = (FLOAT_MAX_SOUND_DISTANCE - dist) / FLOAT_MAX_SOUND_DISTANCE;
        ratio_dist = ratio_dist * ratio_dist;
    }
    else 
    {
        ratio_dist = 0.0f;
    }
    
    return vol * ratio_dist;
}

void SeCmdSetVolForStts()
{
    return;
}

void SeCmdAjstVolChng()
{
    return;
}

int SeCmdFdIn(int sw_no, u_short vol_tgt, short int vol_inc)
{
    return;
}

int SeCmdFdOut(int sw_no, short int vol_inc)
{
    SE_WRK *swp;
  
    if (sw_no >= 0 && sw_no < SE_WRK_SIZE)
    {
        swp = SeGetSeWrk(sw_no);
        
        if (swp->status != 0)
        {
            if (vol_inc >= 0) 
            {
                return -1;
            }
            
            swp->fade_mode = SE_FADE_OUT;
            swp->fade_vol = 0;
            swp->fade_spd = 0;
            
            return sw_no;
        }
    }
    
    return -1;
}

void SeSetVolReg()
{
    return;
}

float SeCmdGetAngle(float rot0, float rot1)
{
    float rot_oc = rot0 - rot1;
    RotLimitChk(&rot_oc);
    return rot_oc;
}

void SeCmdGetRotRatio() 
{
    return;
}

int SeCmdSetPitch(int sw_no, u_short pitch)
{  
    SetIopCmdSm(IC_SE_PITCH, sw_no, pitch, 0);
}

u_short SeCmdGetDataPitch(int se_req_no)
{
    return;
}
