#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ea_cmd.h"

#include "graphics/graph3d/sglib.h"
#include "os/eeiop/eeiop.h"
#include "os/eeiop/cdvd/eecdvd.h"

#define FLOAT_RES_X 640.0f
#define US_MAX_PAN 0x4ff
#define PI 3.141592741f
#define FLOAT_MAX_SOUND_DISTANCE 10000.0f
#define FLOAT_MIN_PITCH_SOUND_DISTANCE 2000.0f

#define CALC_RATIO(MAX_VALUE, VALUE) (MAX_VALUE - VALUE) / MAX_VALUE
#define CALC_DELTA(X,Y) X - Y
#define FLOAT_MAX_SOUND_DISTANCE 10000.0f
#define CONCAT_USHORT(X, Y) (((X) << 16) | ((Y)))

void EAdpcmCmdInit(u_int dev_init)
{
    SetIopCmdSm(IC_ADPCM_INIT, dev_init, 0, 0);
}

void EAdpcmCmdPlay(u_char channel, u_char loop, int file_no, int start_flm, u_short vol, u_short pan, u_short pitch, u_short fin_flm)
{
    IMG_ARRANGEMENT *iap;
    int flg;

    flg = 0xc;
    iap = GetImgArrangementP(file_no);

    if (loop)
    {
        flg = 0xe;
    }

    if (channel) 
    {
        flg |= 1;
    }
    
    SetIopCmdLg(IC_ADPCM_PLAY, file_no, iap->start, iap->size, flg, CONCAT_USHORT(pan, vol), CONCAT_USHORT(pitch, fin_flm), start_flm);
}

void EAdpcmCmdPreload(u_char channel,int file_no,int start_flm,u_short fin_flm)
{
    IMG_ARRANGEMENT *iap;
    
    iap = GetImgArrangementP(file_no);

    SetIopCmdLg(IC_ADPCM_PRELOAD, file_no, iap->start, iap->size, 0, 0, 0, start_flm);
}

void EAdpcmCmdPreEndPlay(u_char channel, u_char loop, int file_no, u_short vol, u_short pan, u_short pitch, u_short fin_flm)
{
    int flg;
    
    flg = 0xc;

    if (loop)
    {
        flg = 0xe;
    }

    if (channel) 
    {
        flg |= 1;
    }
    
    SetIopCmdLg(IC_ADPCM_LOADEND_PLAY, file_no, 0, 0, flg, CONCAT_USHORT(pan, vol), CONCAT_USHORT(pitch, fin_flm), 0);
}

void EAdpcmCmdStop(u_char channel,int file_no,u_short fout_flm)
{
  SetIopCmdLg(IC_ADPCM_STOP, file_no, 0, 0, channel != 0, 0, fout_flm, 0);
}

void EAdpcmCmdPause(u_char channel,int file_no,u_short fout_flm)
{
    SetIopCmdLg(IC_ADPCM_PAUSE, file_no, 0, 0, 0, 0, fout_flm, 0);
}

void EAdpcmCmdRestart(u_char channel, int file_no, u_short fout_flm)
{
    // Channel gets optimized away, all instances that call it give the parameter '0'
    SetIopCmdLg(IC_ADPCM_RESTART, file_no, 0, 0, 0, 0, fout_flm, 0);
}

void EAdpcmCmdFadeVol(u_char channel, int file_no, u_short vol_percent, u_short fin_flm)
{
    SetIopCmdLg(IC_ADPCM_FADE_VOL, file_no, 0, 0, 0, vol_percent, fin_flm, 0);
}

void EAdpcmCmdPos(u_char channel, int file_no, u_short vol, u_short pan, u_short pitch)
{
    SetIopCmdLg(IC_ADPCM_POS, file_no, 0, 0, 0, CONCAT_USHORT(pan,vol), pitch, 0);
}

short int EAGetRetStat(void)
{
    return rcv_stat.adpcm.status;
}

short int EAGetRetTune(void)
{
    return rcv_stat.adpcm.tune_no;
}

int EAGetRetCount(void)
{
    return rcv_stat.adpcm.count;
}

u_char IsAdpcmPlay(int file_no)
{    
    if (EAGetRetTune() == file_no && EAGetRetStat() > 5) 
    {
        return 1;
    }
    
    return 0;
}

u_short EAdpcmGetPan(float rot)
{
    float panf = (SgSinf(rot) * FLOAT_RES_X) + FLOAT_RES_X;
   
    if (US_MAX_PAN < (u_short)panf) 
    {
      return US_MAX_PAN;
    }
    
    return panf;
}

u_short EAdpcmGetVol(float dist, u_short vol)
{
    float ratio_dist;
  
    if (dist < FLOAT_MAX_SOUND_DISTANCE) 
    {
      ratio_dist = CALC_RATIO(FLOAT_MAX_SOUND_DISTANCE, dist);
      ratio_dist *= ratio_dist;
    }
    else 
    {
      ratio_dist = 0.0;
    }
    
    return vol * ratio_dist;
}

u_short EAdpcmGetPitch(float rot, float dist, u_short pitch)
{
    float fpitch_rate;
    
    if (rot < 0.0f) 
    {
        rot = -rot;
    }
    
    fpitch_rate = rot / PI * pitch * 0.5f;
    pitch += fpitch_rate;
    
    if (dist < FLOAT_MIN_PITCH_SOUND_DISTANCE) 
    {
        // 0.100000025f = 4000001/40000000
        return pitch + pitch * 0.100000025f * CALC_RATIO(FLOAT_MIN_PITCH_SOUND_DISTANCE, dist);
    }
    else if (FLOAT_MAX_SOUND_DISTANCE > dist)
    {
        return pitch - ((((dist - FLOAT_MIN_PITCH_SOUND_DISTANCE) * pitch) * 0.3f) / 8000);
    }
    else
    {
        return pitch * 0.7f;
    }
}
