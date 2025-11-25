#include "adpcm/iopadpcm.h"

#include "se/iopse.h"

#include "libsd.h"
#include "memory.h"
#include "sysmem.h"
#include "thread.h"

void GetPosCalc(ADPCM_POS_CALC* calcp);

static int IAdpcmMakeThread(u_char channel);

void IaInitDev(u_char channel)
{
    memset(&iop_adpcm[channel], 0, sizeof(IOP_ADPCM));

    while (IAdpcmMakeThread(channel))
        ;

    AdpcmIopBuf[channel] = (u_char*)AllocSysMemory(0, 266240, 0);
    if (!AdpcmIopBuf[channel]) {
        while (1)
            ;
    }

    if (channel == 0) {
        AdpcmSpuBuf[0] = (u_char*)0x1F3740;
        iop_adpcm[0].core = 0;
        iop_adpcm[0].vl = 0;
        iop_adpcm[0].vr = 2;
    } else {
        AdpcmSpuBuf[1] = (u_char*)0x1F6780;
        iop_adpcm[1].core = 1;
        iop_adpcm[1].vl = 44;
        iop_adpcm[1].vr = 46;
    }
}

static int IAdpcmMakeThread(u_char channel)
{
    struct ThreadParam param;

    param.attr = 0x2000000;

    if (channel == 0)
        param.entry = IAdpcmReadCh0;
    else
        param.entry = IAdpcmReadCh1;

    param.initPriority = 31;
    param.stackSize = 2048;
    param.option = 0;
    iop_adpcm[channel].thread_id = CreateThread(&param);
    if (iop_adpcm[channel].thread_id > 0) {
        StartThread(iop_adpcm[channel].thread_id, 0);
        return 0;
    } else {
        return 1;
    }
}

void IaInitEffect()
{
    sceSdSetCoreAttr(SD_C_EFFECT_ENABLE | SD_CORE_0, 0);
}

void IaInitVolume()
{
    sceSdSetParam(SD_P_MVOLL | SD_CORE_0, iop_mv.vol);
    sceSdSetParam(SD_P_MVOLR | SD_CORE_0, iop_mv.vol);
    sceSdSetSwitch(SD_S_VMIXL | SD_CORE_0, 0xFFFFFFu);
    sceSdSetSwitch(SD_S_VMIXR | SD_CORE_0, 0xFFFFFFu);
    sceSdSetSwitch(SD_S_VMIXEL | SD_CORE_0, 0);
    sceSdSetSwitch(SD_S_VMIXER | SD_CORE_0, 0);
    sceSdSetParam(SD_VP_VOLL | SD_VOICE(0) | SD_CORE_0, 0);
    sceSdSetParam(SD_VP_VOLR | SD_VOICE(0) | SD_CORE_0, 0);
    sceSdSetParam(SD_VP_VOLL | SD_VOICE(1) | SD_CORE_0, 0);
    sceSdSetParam(SD_VP_VOLR | SD_VOICE(1) | SD_CORE_0, 0);
}

void IaDbgMemoryCheck()
{
    int tmp;

    tmp = QueryMemSize();
    tmp = QueryTotalFreeMemSize();
    tmp = QueryMaxFreeMemSize();
}

void IaSetRegSsa(u_char channel)
{
    sceSdSetAddr(iop_adpcm[channel].core | iop_adpcm[channel].vl | SD_VA_SSA, (u_int)AdpcmSpuBuf[channel]);
    sceSdSetAddr(iop_adpcm[channel].core | iop_adpcm[channel].vr | SD_VA_SSA, (u_int)(AdpcmSpuBuf[channel] + 4096));
}

void IaSetRegAdsr(u_char channel)
{
    sceSdSetParam(iop_adpcm[channel].core | iop_adpcm[channel].vl | SD_VP_ADSR1, 0xFu);
    sceSdSetParam(iop_adpcm[channel].core | iop_adpcm[channel].vl | SD_VP_ADSR2, 0x1FC0u);
    sceSdSetParam(iop_adpcm[channel].core | iop_adpcm[channel].vr | SD_VP_ADSR1, 0xFu);
    sceSdSetParam(iop_adpcm[channel].core | iop_adpcm[channel].vr | SD_VP_ADSR2, 0x1FC0u);
}

void IaSetRegVol(u_char channel)
{
    sceSdSetParam(iop_adpcm[channel].core | iop_adpcm[channel].vl | SD_VP_VOLL, iop_adpcm[channel].vol_ll);
    sceSdSetParam(iop_adpcm[channel].core | iop_adpcm[channel].vl | SD_VP_VOLR, iop_adpcm[channel].vol_lr);
    sceSdSetParam(iop_adpcm[channel].core | iop_adpcm[channel].vr | SD_VP_VOLL, iop_adpcm[channel].vol_rl);
    sceSdSetParam(iop_adpcm[channel].core | iop_adpcm[channel].vr | SD_VP_VOLR, iop_adpcm[channel].vol_rr);
}

void IaSetRegPitch(u_char channel)
{
    sceSdSetParam(iop_adpcm[channel].core | iop_adpcm[channel].vl | SD_VP_PITCH, iop_adpcm[channel].pitch);
    sceSdSetParam(iop_adpcm[channel].core | iop_adpcm[channel].vr | SD_VP_PITCH, iop_adpcm[channel].pitch);
}

void IaSetRegKon(u_char channel)
{
    // What is even going on here? these seems completely wrong
    if (channel) {
        sceSdSetSwitch(iop_adpcm[channel].core | SD_S_KON, 0xC00000u);
    } else {
        sceSdSetSwitch(iop_adpcm[channel].core | SD_S_KON, 3);
    }
}

void IaSetRegKoff(u_char channel)
{
    // What is even going on here? these seems completely wrong
    if (channel) {
        sceSdSetSwitch(iop_adpcm[channel].core | SD_S_KOFF, 0xC00000u);
    } else {
        sceSdSetSwitch(iop_adpcm[channel].core | SD_S_KOFF, 3);
    }
}

void IaSetWrkVolPanPitch(u_char channel, u_short pan, u_short master_vol, u_short pitch)
{
    ADPCM_POS_CALC pcalc;

    pcalc.pan = pan;
    pcalc.master_vol = master_vol;
    GetPosCalc(&pcalc);
    iop_adpcm[channel].pan = pcalc.pan;
    iop_adpcm[channel].vol = pcalc.master_vol;
    iop_adpcm[channel].vol_ll = pcalc.ll;
    iop_adpcm[channel].vol_lr = pcalc.lr;
    iop_adpcm[channel].vol_rl = pcalc.rl;
    iop_adpcm[channel].vol_rr = pcalc.rr;
    iop_adpcm[channel].pitch = pitch;
}

void IaSetWrkFadeParam(u_char channel, int fade_flm, u_short target_vol)
{
    iop_adpcm[channel].fade_flm = fade_flm;
    iop_adpcm[channel].target_vol = target_vol;
}

void IaSetWrkFadeMode(u_char channel, u_char mode, u_short target_vol, int fade_flm)
{
    iop_adpcm[channel].count = 0;
    iop_adpcm[channel].fade_mode = mode;
    iop_adpcm[channel].fade_flm = fade_flm;
    iop_adpcm[channel].target_vol = target_vol;
}

void IaSetWrkFadeInit(u_char channel)
{
    iop_adpcm[channel].count = 0;
    iop_adpcm[channel].fade_mode = 0;
    iop_adpcm[channel].fade_flm = 0;
    iop_adpcm[channel].target_vol = iop_adpcm[channel].vol;
}

void GetPosCalc(ADPCM_POS_CALC* calcp)
{
    if (!iop_mv.mono) {
        if (calcp->pan < 0x280u) {
            calcp->ll = calcp->master_vol;
            calcp->lr = 0;
            calcp->rl = ((640 - calcp->pan) * calcp->master_vol) / 640;
            calcp->rr = calcp->pan * calcp->master_vol / 640;
        } else {
            calcp->ll = ((1279 - calcp->pan) * calcp->master_vol) / 640;
            calcp->lr = ((calcp->pan - 640) * calcp->master_vol) / 640;
            calcp->rl = 0;
            calcp->rr = calcp->master_vol;
        }
    } else {
        calcp->ll = calcp->master_vol >> 1;
        calcp->lr = calcp->master_vol >> 1;
        calcp->rl = calcp->master_vol >> 1;
        calcp->rr = calcp->master_vol >> 1;
    }
}

void IaSetSteMono()
{
    IaSetWrkVolPanPitch(0, iop_adpcm[0].pan, iop_adpcm[0].vol, iop_adpcm[0].pitch);
    IaSetRegVol(0);
}

void IaSetStopBlock(u_char channel)
{
    u_char sb_tbl[64];

    memset(sb_tbl, 0, sizeof(sb_tbl));
    sb_tbl[1] = 7;
    sb_tbl[17] = 7;
    sb_tbl[33] = 7;
    sb_tbl[49] = 7;
    while (sceSdVoiceTrans(channel, 0, sb_tbl, snd_buf_top[2 * channel + 26], 0x40u) < 0)
        ;
    sceSdVoiceTransStatus(channel, 1);
}

void IaSetMasterVol(u_short mvol)
{
    sceSdSetParam(SD_P_MVOLL, mvol & 0x3FFF);
    sceSdSetParam(SD_P_MVOLR, mvol & 0x3FFF);
}
