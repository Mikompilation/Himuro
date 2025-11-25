#include "iopadpcm.h"

#include "cdvd/iopcdvd.h"
#include "iopmain.h"

#include "intrman.h"
#include "introld.h"
#include "libsd.h"
#include "loadcore.h"
#include "memory.h"
#include "thread.h"

IOP_ADPCM iop_adpcm[2];
u_char* AdpcmIopBuf[2];
u_char* AdpcmSpuBuf[2];
ADPCM_CMD now_cmd;
ADPCM_CMD cmd_buf[8];

void IAdpcmInit(int dev_init);
void IAdpcmInit(int dev_init);
void IAdpcmAddCmd(IOP_COMMAND*);
static void IAdpcmFadeVol(IOP_COMMAND*);
static void IAdpcmPos(IOP_COMMAND*);
static void IAdpcmMvol(IOP_COMMAND*);

static void AdpcmSpu2IntrHander(int core_bit, void* data);
static int AdpcmTransCB(int channel, void* common);
static void SetLoopFlgAll(u_short core);

void IAdpcmCmd(IOP_COMMAND* icp)
{
    switch (icp->cmd_no) {
    case IC_ADPCM_PLAY:
    case IC_ADPCM_PRELOAD:
    case IC_ADPCM_LOADEND_PLAY:
    case IC_ADPCM_STOP:
    case IC_ADPCM_PAUSE:
    case IC_ADPCM_RESTART:
        IAdpcmAddCmd(icp);
        break;
    case IC_ADPCM_INIT:
        IAdpcmInit(icp->data1);
        break;
    case IC_ADPCM_FADE_VOL:
        IAdpcmFadeVol(icp);
        break;
    case IC_ADPCM_POS:
        IAdpcmPos(icp);
        break;
    case IC_ADPCM_MVOL:
        IAdpcmMvol(icp);
    case IC_ADPCM_QUIT:
        break;
    }
}

void IAdpcmAddCmd(IOP_COMMAND* icp)
{
    ADPCM_CMD cmd;
    ADPCM_CMD* nc;
    ADPCM_CMD* ac;
    int i;
    int j;
    int k;

    memset(&cmd, 0, sizeof(cmd));

    cmd.cmd_type = 0;
    switch (icp->cmd_no) {
    case IC_ADPCM_PLAY:
        cmd.cmd_type = AC_PLAY;
        cmd.fade_flm = icp->data6 & 0xffff;
        cmd.first = icp->data2;
        cmd.size = icp->data3;
        cmd.start_cnt = icp->data7;
        cmd.tune_no = icp->data1;
        cmd.vol = icp->data5 & 0xffff;
        cmd.target_vol = cmd.vol;
        cmd.pan = (icp->data5 >> 16) & 0xffff;
        cmd.pitch = (icp->data6 >> 16) & 0xffff;
        cmd.loop = icp->data4 & 2;
        cmd.channel = icp->data4 & 1;
        if (cmd.fade_flm) {
            cmd.fade_mode = ADPCM_FADE_IN_PLAY;
            cmd.vol = 0;
        } else {
            cmd.fade_mode = 0;
        }
        break;
    case IC_ADPCM_PRELOAD:
        cmd.cmd_type = AC_PRELOAD;
        cmd.tune_no = icp->data1;
        cmd.first = icp->data2;
        cmd.size = icp->data3;
        cmd.start_cnt = icp->data7;
        cmd.channel = icp->data4 & 1;
        break;
    case IC_ADPCM_LOADEND_PLAY:
        cmd.cmd_type = AC_PLAY;
        cmd.fade_flm = icp->data6 & 0xffff;
        cmd.start_cnt = icp->data7;
        cmd.tune_no = icp->data1;
        cmd.vol = icp->data5 & 0xffff;
        cmd.target_vol = cmd.vol;
        cmd.pan = (icp->data5 >> 16) & 0xffff;
        cmd.pitch = (icp->data6 >> 16) & 0xffff;
        cmd.loop = icp->data4 & 2;
        cmd.channel = icp->data4 & 1;
        if (cmd.fade_flm) {
            cmd.fade_mode = ADPCM_FADE_IN_PLAY;
            cmd.vol = 0;
        } else {
            cmd.fade_mode = 0;
        }
        break;
    case IC_ADPCM_STOP:
        cmd.cmd_type = AC_STOP;
        cmd.fade_flm = icp->data6 & 0xffff;
        cmd.tune_no = icp->data1;
        cmd.channel = icp->data4 & 1;
        break;
    case IC_ADPCM_PAUSE:
        cmd.cmd_type = AC_PAUSE;
        cmd.fade_flm = icp->data6 & 0xffff;
        cmd.tune_no = icp->data1;
        cmd.channel = icp->data4 & 1;
        break;
    case IC_ADPCM_RESTART:
        cmd.cmd_type = AC_RESTART;
        cmd.fade_flm = icp->data6 & 0xffff;
        cmd.tune_no = icp->data1;
        cmd.channel = icp->data4 & 1;
        break;
    }

    // Insert into cmd buffer
    if (cmd.cmd_type) {
        nc = 0;
        for (i = 0; i < 8; i++) {
            if (cmd_buf[i].cmd_type == 0) {
                cmd_buf[i] = cmd;
                nc = &cmd_buf[i];
                break;
            }
        }

        if (i >= 8 || nc == 0) {
            return;
        }
    }
}

void IAdpcmSetSpu2Irq()
{
    static u_short pitch = 0x40;

    sceSdSetParam(SD_VP_PITCH | 0, pitch);
    sceSdSetParam(SD_VP_PITCH | 2, pitch);
    sceSdSetParam(SD_VP_PITCH | 0x2d, pitch);
    sceSdSetParam(SD_VP_PITCH | 0x2f, pitch);

    pitch /= 2;
    if (!pitch)
        pitch = 64;
}

void IAdpcmMain(void)
{
    IOP_ADPCM* iap = iop_adpcm;

    while (1) {
        SleepThread();

        switch (iap->fade_mode) {
        case ADPCM_FADE_NO:
            break;
        case ADPCM_FADE_IN_PLAY:
        case ADPCM_FADE_OUT_STOP:
        case ADPCM_FADE:
            if (iap->fade_flm > iap->fade_count) {
                iap->vol = iap->vol + ((iap->target_vol - iap->vol) / (iap->fade_flm - iap->fade_count));

                iap->fade_count++;
                IaSetWrkVolPanPitch(0, iap->pan, iap->vol, iap->pitch);
                IaSetRegVol(0);
            }
            break;
        }
    }
}

void IAdpcmMain2()
{
    IOP_ADPCM* iap;
    ADPCM_CMD ac;

    iap = &iop_adpcm[0];

    if (now_cmd.cmd_type == AC_NONE) {
        if (cmd_buf[0].cmd_type == AC_NONE) {
        } else {
            IAdpcmCmdSlide();
        }
    } else {
        switch (now_cmd.cmd_type) {
        case AC_PLAY:
            IAdpcmCmdPlay();
            break;
        case AC_PRELOAD:
            IAdpcmCmdPreLoad();
            break;
        case AC_STOP:
            IAdpcmCmdStop();
            break;
        case AC_PAUSE:
            IAdpcmCmdPause();
            break;
        case AC_RESTART:
            IAdpcmCmdRestart();
            break;
        }
    }

    switch (iap->fade_mode) {
    case ADPCM_FADE_NO:
        break;
    case ADPCM_FADE_IN_PLAY:
    case ADPCM_FADE_OUT_STOP:
    case ADPCM_FADE:
        if (iap->fade_flm <= iap->fade_count) {
            if (!iap->target_vol && iap->fade_mode == 2) {
                ac.cmd_type = 3;
                ac.channel = 0;
                ac.tune_no = iop_adpcm[0].tune_no;
                IAdpcmStop(&ac);
            }
            iap->vol = iap->target_vol;
            iap->fade_count = 0;
            iap->fade_flm = 0;
            iap->target_vol = 0;
            iap->fade_mode = 0;
        }
        break;
    }

    iop_stat.adpcm.tune_no = iap->tune_no;
    iop_stat.adpcm.count = iap->count;

    if (iap->stat == ADPCM_STAT_NOPLAY) {
        iap->stop_cnt++;
        if (iap->stop_cnt >= 41) {
            iap->stop_cnt = 16;
        }
    } else {
        iap->stop_cnt = 0;
    }

    if (iap->stop_cnt >= 16) {
        if (iap->loop_end) {
            iop_stat.adpcm.status = ADPCM_STAT_LOOPEND_STOP;
        } else {
            iop_stat.adpcm.status = ADPCM_STAT_FULL_STOP;
        }
    } else {
        iop_stat.adpcm.status = iap->stat;
    }
}

void IAdpcmInit(int dev_init)
{
    CpuEnableIntr();
    EnableIntr(INUM_DMA_4);
    EnableIntr(INUM_DMA_7);
    EnableIntr(INUM_SPU);
    sceSdSetParam(SD_P_MVOLL | SD_CORE_0, 0);
    sceSdSetParam(SD_P_MVOLR | SD_CORE_0, 0);
    sceSdInit(SD_INIT_HOT);
    if (!dev_init)
        IaInitDev(0);
    InitAdpcmCmdBuf();
    IaInitVolume();
    IaSetRegKoff(0);
    IaInitEffect();
    IaDbgMemoryCheck();
    sceSdVoiceTransStatus(0, SD_TRANS_STATUS_WAIT);
    sceSdSetSpu2IntrHandler(0, 0);
    sceSdSetSpu2IntrHandler(AdpcmSpu2IntrHander, iop_adpcm);
    sceSdSetAddr(iop_adpcm[0].core | SD_A_IRQA, (u_int)(AdpcmSpuBuf[0] + 2048) & 0xFFFFFFF0);
    sceSdSetAddr(iop_adpcm[1].core | SD_A_IRQA, (u_int)(AdpcmSpuBuf[1] + 2048) & 0xFFFFFFF0);
    sceSdSetTransIntrHandler(0, 0, 0);
    iop_adpcm[0].stat = 0;
    iop_stat.adpcm.status = 0;
}

void IAdpcmPreLoad(ADPCM_CMD* acp)
{
    int channel;
    u_char endld_flg;

    channel = acp->channel;
    iop_adpcm[channel].tune_no = acp->tune_no;
    iop_adpcm[channel].stat = 3;
    iop_adpcm[channel].use = 1;
    iop_adpcm[channel].dbids = 0;
    iop_adpcm[channel].dbidi = 0;
    iop_adpcm[channel].str_lpos = 0;
    iop_adpcm[channel].str_tpos = 0;
    iop_adpcm[channel].loop = acp->loop;
    iop_adpcm[channel].count = acp->start_cnt;
    iop_adpcm[channel].loop_end = 0;

    if (acp->size > (acp->start_cnt * 0x1000)) {
        iop_adpcm[channel].szFile_bk = acp->size;
        iop_adpcm[channel].szFile = acp->size - (acp->start_cnt * 0x1000);
        iop_adpcm[channel].first = acp->first;
        iop_adpcm[channel].start = iop_adpcm[channel].first + 2 * acp->start_cnt;
    } else {
        iop_adpcm[channel].szFile_bk = acp->size;
        iop_adpcm[channel].szFile = acp->size;
        iop_adpcm[channel].first = acp->first;
        iop_adpcm[channel].start = iop_adpcm[channel].first;
    }

    if (iop_adpcm[channel].szFile <= 266240) {
        iop_adpcm[channel].lreq_size = iop_adpcm[channel].szFile;
        endld_flg = 1;
    } else {
        iop_adpcm[channel].lreq_size = 266240;
        endld_flg = 0;
    }

    ICdvdLoadReqAdpcm(
        iop_adpcm[channel].start,
        iop_adpcm[channel].lreq_size,
        (u_int*)AdpcmIopBuf[channel],
        channel,
        1u,
        endld_flg);
}

void IAdpcmPreLoadEnd(int channel)
{
    int i;
    static int cnt = 0;
    channel = 0;

    if (iop_adpcm[channel].stat == ADPCM_STAT_PRELOAD) {
        iop_adpcm[channel].stat = ADPCM_STAT_PRELOAD_TRANS;
        iop_adpcm[channel].start += (iop_adpcm[channel].lreq_size + 2047) / 2048;
        SetLoopFlgAll(channel);
        FlushDcache();
        sceSdVoiceTransStatus(channel, SD_TRANS_STATUS_WAIT);
        sceSdSetCoreAttr(iop_adpcm[channel].core | 4, 0);
        for (i = 0; i < 4; ++i) {
            sceSdVoiceTransStatus(channel, SD_TRANS_STATUS_WAIT);
            while (sceSdVoiceTrans(
                       channel,
                       0,
                       &AdpcmIopBuf[channel][2048 * i],
                       (u_int)AdpcmSpuBuf[channel] + 2048 * (i / 2) + 4096 * (i & 1),
                       0x800u)
                < 0) {

                cnt++;
                if (cnt > 10000)
                    cnt = 0;
            }
            sceSdVoiceTransStatus(channel, SD_TRANS_STATUS_WAIT);
        }
        iop_adpcm[channel].str_lpos = iop_adpcm[channel].lreq_size;
        iop_adpcm[channel].str_tpos = 0x2000;
        iop_adpcm[channel].pos = 0x2000;
    }

    if (iop_adpcm[channel].use)
        iop_adpcm[channel].stat = ADPCM_STAT_PRELOAD_END;
}

void IAdpcmPlay(ADPCM_CMD* acp)
{
    u_char channel = acp->channel;

    if (iop_adpcm[channel].stat == ADPCM_STAT_PRELOAD_END) {
        iop_adpcm[channel].loop_end = 0;
        sceSdSetCoreAttr(iop_adpcm[channel].core | 4, 0);
        IaSetWrkVolPanPitch(channel, acp->pan, acp->vol, acp->pitch);
        IaSetRegVol(channel);
        IaSetRegPitch(channel);
        IaSetRegAdsr(channel);
        IaSetRegSsa(channel);
        sceSdSetTransIntrHandler(channel, AdpcmTransCB, 0);
        sceSdSetSpu2IntrHandler((sceSdSpu2IntrHandler)AdpcmSpu2IntrHander, iop_adpcm);
        sceSdSetAddr(iop_adpcm[channel].core | SD_A_IRQA, (u_int)(AdpcmSpuBuf[channel] + 2048) & 0xFFFFFFF0);
        IaSetRegKon(channel);
        sceSdSetCoreAttr(iop_adpcm[channel].core | 4, 1u);
        iop_adpcm[channel].stat = ADPCM_STAT_PLAY;

        return;
    }
}

void IAdpcmStop(ADPCM_CMD* acp)
{
    u_char channel;

    channel = acp->channel;
    sceSdSetCoreAttr(iop_adpcm[channel].core | 4, 0);
    IaSetWrkFadeInit(channel);

    if (iop_adpcm[channel].stat == ADPCM_STAT_NOPLAY) {
        if (iop_adpcm[channel].use) {
            iop_adpcm[channel].use = 0;
        }
    } else {
        if (iop_adpcm[channel].stat == ADPCM_STAT_PRELOAD) {
            ICdvdBreak();
            iop_adpcm[channel].use = 0;
        } else if (iop_adpcm[channel].stat == ADPCM_STAT_PRELOAD_END) {
            iop_adpcm[channel].use = 0;
        } else {
            if (cdvd_stat.stat == ADPCM_STAT_LOOPEND_STOP)
                ICdvdBreak();
            IaSetRegKoff(channel);
            sceSdSetTransIntrHandler(channel, 0, 0);
            sceSdVoiceTransStatus(channel, 1);
            iop_adpcm[channel].use = 0;
        }
    }

    if (IAdpcmChkCmdExist()) {
        iop_adpcm[channel].stat = ADPCM_STAT_NOPLAY;
    } else {
        iop_adpcm[channel].stat = ADPCM_STAT_NOPLAY;
    }

    iop_adpcm[channel].tune_no = 0;
}

static void IAdpcmFadeVol(IOP_COMMAND* iop) {}

static void IAdpcmPos(IOP_COMMAND* icp)
{
    int channel;

    channel = icp->data3;
    IaSetWrkVolPanPitch(channel, (icp->data5 >> 16) & 0xffff, icp->data5 & 0xffff, icp->data6 & 0xffff);
    IaSetRegVol(channel);
    IaSetRegPitch(channel);
}

static void IAdpcmMvol(IOP_COMMAND* icp)
{
    u_short mvol = icp->data1 & 0xffff;

    IaSetMasterVol(mvol);
}

static void AdpcmSpu2IntrHander(int core_bit, void* data)
{
    IOP_ADPCM* ia = data;

    if (iop_adpcm[0].stat == ADPCM_STAT_PLAY)
        sceSdSetCoreAttr(iop_adpcm[0].core | SD_C_IRQ_ENABLE, 0);

    if (iop_adpcm[1].stat == ADPCM_STAT_PLAY)
        sceSdSetCoreAttr(iop_adpcm[1].core | SD_C_IRQ_ENABLE, 0);

    if ((core_bit & 1) != 0)
        iWakeupThread(ia[0].thread_id);

    if ((core_bit & 2) != 0)
        iWakeupThread(ia[1].thread_id);
}

static int AdpcmTransCB(int channel, void* common)
{
    if (iop_adpcm[channel].stat == ADPCM_STAT_LTRANS) {
        iop_adpcm[channel].pos += 2048;
        iop_adpcm[channel].str_tpos += 2048;
        iop_adpcm[channel].stat = ADPCM_STAT_RTRANS;
        while (sceSdVoiceTrans(
                   channel,
                   0,
                   &AdpcmIopBuf[channel][iop_adpcm[channel].pos],
                   (u_int)((iop_adpcm[channel].dbids * 2048) + AdpcmSpuBuf[channel] + 4096),
                   0x800u)
            < 0)
            ;
    } else if (iop_adpcm[channel].stat == ADPCM_STAT_RTRANS) {
        iop_adpcm[channel].pos += 2048;
        iop_adpcm[channel].str_tpos += 2048;
        sceSdSetAddr(
            iop_adpcm[channel].core | SD_A_IRQA,
            ((u_int)AdpcmSpuBuf[channel] + (iop_adpcm[channel].dbids * 2048)) & 0xFFFFFFF0);
        iop_adpcm[channel].dbids ^= 1u;
        iop_adpcm[channel].stat = ADPCM_STAT_PLAY;
        sceSdSetCoreAttr(iop_adpcm[channel].core | 4, 1u);

        if (iop_adpcm[channel ^ 1].stat >= 4)
            sceSdSetCoreAttr(iop_adpcm[channel ^ 1].core | 4, 1u);
    } else {
    }

    return 1;
}

void IAdpcmReadCh0()
{
    int count = 0;
    int start;
    u_int remain_t;
    u_int remain_l;
    u_short tmp_tune_no;
    ADPCM_CMD cmd;
    u_char loop_ok;

    while (1) {
        SleepThread();
        if (iop_adpcm[0].stat >= ADPCM_STAT_PRELOAD_TRANS) {
            iop_adpcm[0].count++;

            remain_t = iop_adpcm[0].szFile - iop_adpcm[0].str_tpos;
            if (remain_t > 0x1000) {
                if (iop_adpcm[0].pos == (iop_adpcm[0].dbidi + 1) * 0x20000) {
                    remain_l = iop_adpcm[0].szFile - iop_adpcm[0].str_lpos;
                    start = iop_adpcm[0].start;

                    if (remain_l > 0x20000) {
                        iop_adpcm[0].lreq_size = 0x20000;
                        ICdvdLoadReqAdpcm(
                            start,
                            iop_adpcm[0].lreq_size,
                            (u_int*)&AdpcmIopBuf[0][0x20000 * iop_adpcm[0].dbidi],
                            0,
                            2u,
                            0);
                    } else if (remain_l) {
                        iop_adpcm[0].lreq_size = remain_l;
                        ICdvdLoadReqAdpcm(
                            start,
                            iop_adpcm[0].lreq_size,
                            (u_int*)&AdpcmIopBuf[0][0x20000 * iop_adpcm[0].dbidi],
                            0,
                            2u,
                            1u);
                    }

                    if (iop_adpcm[0].dbidi)
                        iop_adpcm[0].pos = 0;

                    iop_adpcm[0].dbidi ^= 1;
                }

                // ????
                if (iop_adpcm[0].stat != ADPCM_STAT_PLAY) {
                }

                if (!sceSdVoiceTransStatus(0, SD_TRANS_STATUS_CHECK))
                    sceSdVoiceTransStatus(0, SD_TRANS_STATUS_WAIT);

                iop_adpcm[0].stat = ADPCM_STAT_LTRANS;

                while (sceSdVoiceTrans(
                           0,
                           0,
                           &AdpcmIopBuf[0][iop_adpcm[0].pos],
                           (u_int)((iop_adpcm[0].dbids * 0x800) + AdpcmSpuBuf[0]),
                           0x800u)
                    < 0) {

                    count++;
                    if (count > 100000)
                        count = 0;
                }
            } else {
                loop_ok = 0;
                if (!IAdpcmChkCmdExist()) {
                    loop_ok = 1;
                }

                cmd.cmd_type = 3;
                cmd.fade_flm = 0;
                cmd.tune_no = 0;
                cmd.channel = 0;
                tmp_tune_no = iop_adpcm[0].tune_no;
                IAdpcmStop(&cmd);
                if (iop_adpcm[0].loop && loop_ok) {
                    if (iop_adpcm[0].fade_mode == ADPCM_FADE_NO) {
                        if (!IAdpcmChkCmdExist()) {
                            cmd.cmd_type = AC_PLAY;
                            cmd.fade_flm = 0;
                            cmd.start_cnt = 0;
                            cmd.tune_no = tmp_tune_no;
                            cmd.vol = iop_adpcm[0].vol;
                            cmd.target_vol = cmd.vol;
                            cmd.pan = iop_adpcm[0].pan;
                            cmd.pitch = iop_adpcm[0].pitch;
                            cmd.loop = 1;
                            cmd.channel = 0;
                            cmd.first = iop_adpcm[0].first;
                            cmd.size = iop_adpcm[0].szFile_bk;
                            now_cmd = cmd;
                        }
                    } else {
                        if (iop_adpcm[0].target_vol && !IAdpcmChkCmdExist()) {
                            cmd.cmd_type = AC_PLAY;
                            cmd.fade_flm = 0;
                            cmd.start_cnt = 0;
                            cmd.tune_no = tmp_tune_no;
                            cmd.vol = iop_adpcm[0].target_vol;
                            cmd.target_vol = cmd.vol;
                            cmd.pan = iop_adpcm[0].pan;
                            cmd.pitch = iop_adpcm[0].pitch;
                            cmd.loop = 1;
                            cmd.channel = 0;
                            cmd.first = iop_adpcm[0].first;
                            cmd.size = iop_adpcm[0].szFile_bk;

                            now_cmd = cmd;
                        }
                    }
                } else {
                    iop_adpcm[0].loop_end = 1;
                }
            }
        } else {
        }
    }
}

void IAdpcmTuneEndStop(u_char channel)
{
    ADPCM_CMD cmd;

    cmd.cmd_type = 3;
    cmd.fade_flm = 0;
    cmd.tune_no = 0;
    cmd.channel = channel;
    IAdpcmStop(&cmd);
}

void IAdpcmTuneEndLoop(u_char channel, u_char fade, u_short tune_no)
{
    ADPCM_CMD cmd;

    cmd.cmd_type = AC_PLAY;
    cmd.fade_flm = 0;
    cmd.start_cnt = 0;
    cmd.tune_no = tune_no;

    if (fade) {
        cmd.vol = iop_adpcm[channel].target_vol;
        cmd.target_vol = cmd.vol;
    } else {
        cmd.vol = iop_adpcm[channel].vol;
        cmd.target_vol = cmd.vol;
    }

    cmd.pan = iop_adpcm[channel].pan;
    cmd.pitch = iop_adpcm[channel].pitch;
    cmd.loop = 1;
    cmd.channel = channel;
    cmd.first = iop_adpcm[channel].first;
    cmd.size = iop_adpcm[channel].szFile_bk;

    now_cmd = cmd;
}

void IAdpcmReadCh1()
{
    int count = 0;
    int start;
    u_int remain_t;
    u_int remain_l;
    u_short tmp_tune_no;
    ADPCM_CMD cmd;
    u_char loop_ok;

    while (1) {
        SleepThread();
        if (iop_adpcm[1].stat >= ADPCM_STAT_PRELOAD_TRANS) {
            iop_adpcm[1].count++;

            remain_t = iop_adpcm[1].szFile - iop_adpcm[1].str_tpos;
            if (remain_t > 0x1000) {
                if (iop_adpcm[1].pos == (iop_adpcm[1].dbidi + 1) * 0x20000) {
                    remain_l = iop_adpcm[1].szFile - iop_adpcm[1].str_lpos;
                    start = iop_adpcm[1].start;

                    if (remain_l > 0x20000) {
                        iop_adpcm[1].lreq_size = 0x20000;
                        ICdvdLoadReqAdpcm(
                            start,
                            iop_adpcm[1].lreq_size,
                            (u_int*)&AdpcmIopBuf[1][0x20000 * iop_adpcm[1].dbidi],
                            1,
                            2u,
                            0);
                    } else if (remain_l) {
                        iop_adpcm[1].lreq_size = remain_l;
                        ICdvdLoadReqAdpcm(
                            start,
                            iop_adpcm[1].lreq_size,
                            (u_int*)&AdpcmIopBuf[1][0x20000 * iop_adpcm[1].dbidi],
                            1,
                            2u,
                            1u);
                    }

                    if (iop_adpcm[1].dbidi)
                        iop_adpcm[1].pos = 0;

                    iop_adpcm[1].dbidi ^= 1;
                }

                // ????
                if (iop_adpcm[1].stat != ADPCM_STAT_PLAY) {
                }

                if (!sceSdVoiceTransStatus(1, SD_TRANS_STATUS_CHECK))
                    sceSdVoiceTransStatus(1, SD_TRANS_STATUS_WAIT);

                iop_adpcm[1].stat = ADPCM_STAT_LTRANS;

                while (sceSdVoiceTrans(
                           1,
                           0,
                           &AdpcmIopBuf[1][iop_adpcm[1].pos],
                           (u_int)((iop_adpcm[1].dbids * 0x800) + AdpcmSpuBuf[1]),
                           0x800u)
                    < 0) {

                    count++;
                    if (count > 100000)
                        count = 0;
                }
            } else {
                loop_ok = 0;
                if (!IAdpcmChkCmdExist()) {
                    loop_ok = 1;
                }

                cmd.cmd_type = 3;
                cmd.fade_flm = 0;
                cmd.tune_no = 0;
                cmd.channel = 1;
                tmp_tune_no = iop_adpcm[1].tune_no;
                IAdpcmStop(&cmd);
                if (iop_adpcm[1].loop && loop_ok) {
                    if (iop_adpcm[1].fade_mode == ADPCM_FADE_NO) {
                        if (!IAdpcmChkCmdExist()) {
                            cmd.cmd_type = AC_PLAY;
                            cmd.fade_flm = 0;
                            cmd.start_cnt = 0;
                            cmd.tune_no = tmp_tune_no;
                            cmd.vol = iop_adpcm[1].vol;
                            cmd.target_vol = cmd.vol;
                            cmd.pan = iop_adpcm[1].pan;
                            cmd.pitch = iop_adpcm[1].pitch;
                            cmd.loop = 1;
                            cmd.channel = 1;

                            now_cmd = cmd;
                        }
                    } else {
                        if (iop_adpcm[1].target_vol && !IAdpcmChkCmdExist()) {
                            cmd.cmd_type = AC_PLAY;
                            cmd.fade_flm = 0;
                            cmd.start_cnt = 0;
                            cmd.tune_no = tmp_tune_no;
                            cmd.vol = iop_adpcm[1].target_vol;
                            cmd.target_vol = cmd.vol;
                            cmd.pan = iop_adpcm[1].pan;
                            cmd.pitch = iop_adpcm[1].pitch;
                            cmd.loop = 1;
                            cmd.channel = 1;

                            now_cmd = cmd;
                        }
                    }
                }
            }
        } else {
        }
    }
}

static void SetLoopFlag(u_int* st_addr, u_int szvag, u_char st_end)
{
    int i;
    u_char* lpflgp = (u_char*)st_addr;

    lpflgp++;
    for (i = 0; i < szvag >> 4; ++i) {
        *lpflgp = 2;
        lpflgp += 16;
    }

    if ((st_end & 2) != 0) {
        lpflgp -= 16;
        *lpflgp |= 1u;
    }

    if ((st_end & 1) != 0) {
        lpflgp = (u_char*)st_addr;
        lpflgp++;
        *lpflgp |= 4;
    }
}

static void SetLoopFlgAll(u_short core)
{
    u_char* pos;
    int i;
    int times;

    pos = 0;
    times = 32;
    for (i = 0; i < times; i++, pos += 0x2000) {
        SetLoopFlag(&AdpcmIopBuf[core][(u_int)pos] + 0x0000, 0x800u, 1u);
        SetLoopFlag(&AdpcmIopBuf[core][(u_int)pos] + 0x0800, 0x800u, 1u);
        SetLoopFlag(&AdpcmIopBuf[core][(u_int)pos] + 0x1000, 0x800u, 2u);
        SetLoopFlag(&AdpcmIopBuf[core][(u_int)pos] + 0x1800, 0x800u, 2u);
    }
}

static void SetLoopFlgAll2(u_short core)
{
    u_char* pos;
    int i;
    int times;

    pos = 0;
    times = 32;
    for (i = 0; i < times; i++, pos += 0x2000) {
        SetLoopFlag(&AdpcmIopBuf[core][(u_int)pos] + 0x0000, 0x800u, 1u);
        SetLoopFlag(&AdpcmIopBuf[core][(u_int)pos] + 0x0800, 0x800u, 1u);
        SetLoopFlag(&AdpcmIopBuf[core][(u_int)pos] + 0x1000, 0x800u, 2u);
        SetLoopFlag(&AdpcmIopBuf[core][(u_int)pos] + 0x1800, 0x800u, 2u);
    }
}

void SetLoopFlgSize(u_int size_byte, u_int* start, u_short core)
{
    int i;
    int j;
    int k;
    int lt0;
    int lt1;
    u_char* lpflgp;

    lpflgp = (u_char*)start;
    lt0 = size_byte / 0x2000;
    lt1 = 0x80;
    lpflgp++;

    for (i = 0; i < lt0; i++) {
        for (j = 0; j < 2; j++) {
            for (k = 0; k < lt1; k++) {
                if (!k)
                    *lpflgp = 6;
                else
                    *lpflgp = 2;

                lpflgp += 16;
            }
        }
        for (j = 0; j < 2; j++) {
            for (k = 0; k < lt1; k++) {
                if (k == lt1 - 1)
                    *lpflgp = 3;
                else
                    *lpflgp = 2;

                lpflgp += 16;
            }
        }
    }
}

void IAdpcmLoadEndStream(int channel)
{
    iop_adpcm[channel].str_lpos += iop_adpcm[channel].lreq_size;
    iop_adpcm[channel].start += (iop_adpcm[channel].lreq_size + 2047) / 2048;
}

void IAdpcmLoadEndPreOnly(int channel)
{
    IAdpcmPreLoadEnd(channel);
}
