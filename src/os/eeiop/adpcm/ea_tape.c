#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ea_tape.h"

#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_dat.h"
#include "os/eeiop/adpcm/ea_ctrl.h"

void EAdpcmTapeMain()
{    
    switch(adpcm_map.tape.mode)
    {
    case AMTP_MODE_PRE_FADE_OUT:
        EAdpcmCmdStop(0, 0, 0x50);
        adpcm_map.tape.mode = AMTP_MODE_REQ_WAIT_STOP;
    break;
    case AMTP_MODE_REQ_WAIT_STOP:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.m_flg = 2;
            EAdpcmCmdPlay(0, 0, adpcm_map.tape.para.file_no, 0, adpcm_map.tape.para.vol, adpcm_map.tape.para.pan, adpcm_map.tape.para.pitch, adpcm_map.tape.para.fin_flm);
            adpcm_map.tape.mode = AMTP_MODE_REQ_PLAY;
            adpcm_map.mvol = 0xfff;
        }
    break;
    case AMTP_MODE_REQ_PLAY:
        if (EAGetRetStat() > 5)
        {
            adpcm_map.tape.mode = AMTP_MODE_REQ_PLAYING;
        }
    break;
    case AMTP_MODE_REQ_STOP:
        if (EAGetRetStat() == 1 || EAGetRetStat() == 2)
        {
            adpcm_map.tape.use = 0;
            adpcm_map.gdead.use = 0;
            adpcm_map.hiso.use = 0;
            adpcm_map.autog.use = 0;
            adpcm_map.event.use = 0;
            adpcm_map.mvol = adpcm_map.tgt_vol;
        }
    break;
    case AMTP_MODE_REQ_PLAYING:
        if (EAGetRetStat() == 2)
        {
            adpcm_map.tape.use = 0;
            adpcm_map.gdead.use = 0;
            adpcm_map.hiso.use = 0;
            adpcm_map.autog.use = 0;
            adpcm_map.event.use = 0;
            adpcm_map.mvol = adpcm_map.tgt_vol;
        }
    break;
    }
}

void AdpcmPlayTape(int file_no, u_short vol, int fade_flm)
{
    adpcm_map.tape.para.file_no = file_no;
    adpcm_map.tape.para.vol = GetAdpcmVol(file_no);
    adpcm_map.tape.para.fin_flm = fade_flm;
    adpcm_map.tape.para.pan = 0x280;
    adpcm_map.tape.para.pitch = 0xfff;
    adpcm_map.tape.use = 1;
    adpcm_map.tape.mode = AMTP_MODE_PRE_FADE_OUT;
    adpcm_map.mode = ADPCM_MODE_TAPE;
}

void AdpcmStopTape(int fout_flm)
{
    if (adpcm_map.tape.use != 0)
    {
        EAdpcmCmdStop(0, 0, fout_flm);
        adpcm_map.tape.mode = AMTP_MODE_REQ_STOP;
    }
}

u_char IsEndAdpcmTape()
{
    return adpcm_map.tape.use == 0;
}