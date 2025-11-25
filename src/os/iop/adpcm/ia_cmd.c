#include "adpcm/iopadpcm.h"

#include "memory.h"

void InitAdpcmCmdBuf()
{
    memset(&now_cmd, 0, sizeof(now_cmd));
    memset(cmd_buf, 0, sizeof(cmd_buf));
}

void IAdpcmCmdSlide()
{
    ADPCM_CMD* ac0;
    ADPCM_CMD* ac1;
    int i;

    now_cmd = cmd_buf[0];
    ac0 = &cmd_buf[0];
    ac1 = &cmd_buf[1];

    for (i = 0; i < 7; i++) {
        *ac0++ = *ac1++;
    }

    ac0->cmd_type = 0;
}

u_char IAdpcmChkCmdExist()
{
    if (now_cmd.cmd_type || cmd_buf[0].cmd_type) {
        return 1;
    }

    return 0;
}

void IAdpcmCmdPlay()
{
    ADPCM_CMD ac;
    static int c = 0;

    if (now_cmd.tune_no == iop_adpcm[0].tune_no) {
        switch (iop_adpcm[0].stat) {
        case ADPCM_STAT_PRELOAD_END:
            DbgDispAdpcmCmdWrk(&now_cmd);
            if (now_cmd.fade_flm)
                IaSetWrkFadeMode(now_cmd.channel, now_cmd.fade_mode, now_cmd.target_vol, 4 * now_cmd.fade_flm);
            IAdpcmPlay(&now_cmd);
            now_cmd.cmd_type = 0;
            break;
        case ADPCM_STAT_PRELOAD:
            break;
        case ADPCM_STAT_PRELOAD_TRANS:
            break;
        default:
            iop_adpcm[0].tune_no = 0;
            c++;
            if (!(c % 30)) {
                c = 0;
            }
        }

        return;
    }

    switch (iop_adpcm[0].stat) {
    case ADPCM_STAT_NOPLAY:
    case ADPCM_STAT_PRELOAD_END:
        DbgDispAdpcmCmdWrk(&now_cmd);
        IAdpcmPreLoad(&now_cmd);
        return;
    case ADPCM_STAT_PRELOAD_TRANS:
        return;
    }

    ac.cmd_type = AC_STOP;
    ac.channel = 0;
    ac.tune_no = iop_adpcm[0].tune_no;
    IAdpcmStop(&ac);
}

void IAdpcmCmdStop()
{
    switch (iop_adpcm[0].stat) {
    case ADPCM_STAT_NOPLAY:
        now_cmd.cmd_type = 0;
        break;
    case ADPCM_STAT_PRELOAD:
    case ADPCM_STAT_PRELOAD_TRANS:
        break;
        break;
    case ADPCM_STAT_PRELOAD_END:
        IAdpcmStop(&now_cmd);
        break;
    case ADPCM_STAT_PLAY:
    case ADPCM_STAT_LTRANS:
    case ADPCM_STAT_RTRANS:
    case ADPCM_STAT_PAUSE:
        if (iop_adpcm[0].fade_mode != 2) {
            if (now_cmd.fade_flm) {
                IaSetWrkFadeParam(0, 4 * now_cmd.fade_flm, 0);
                iop_adpcm[0].fade_mode = 2;
                iop_adpcm[0].fade_count = 0;
            } else {
                IAdpcmStop(&now_cmd);
            }
        }
        break;
    }
}

void IAdpcmCmdPreLoad()
{
    ADPCM_CMD ac;

    if (now_cmd.tune_no == iop_adpcm[0].tune_no) {
        switch (iop_adpcm[0].stat) {
        case ADPCM_STAT_PRELOAD_END:
            now_cmd.cmd_type = AC_NONE;
            break;
        }
    } else {
        switch (iop_adpcm[0].stat) {
        case ADPCM_STAT_NOPLAY:
        case ADPCM_STAT_PRELOAD_END:
            IAdpcmPreLoad(&now_cmd);
            return;
        case ADPCM_STAT_PRELOAD_TRANS:
            return;
        }

        if (iop_adpcm[0].fade_mode != ADPCM_FADE_OUT_STOP) {
            ac.cmd_type = AC_STOP;
            ac.channel = 0;
            ac.tune_no = iop_adpcm[0].tune_no;
            IAdpcmStop(&ac);
        }
    }
}

void IAdpcmCmdPause()
{
    if (now_cmd.tune_no == iop_adpcm[0].tune_no) {
        switch (iop_adpcm[0].stat) {
        case ADPCM_STAT_PRELOAD:
            break;
        case ADPCM_STAT_PRELOAD_END:
        case ADPCM_STAT_PLAY:
        case ADPCM_STAT_LTRANS:
        case ADPCM_STAT_RTRANS:
        case ADPCM_STAT_PAUSE:
            break;
        }

        return;
    }
}

void IAdpcmCmdRestart()
{
    if (now_cmd.tune_no == iop_adpcm[0].tune_no) {
        switch (iop_adpcm[0].stat) {
        case ADPCM_STAT_PAUSE:
            break;
        }

        return;
    }
}

void DbgDispAdpcmCmdWrk(ADPCM_CMD* acp) {}
