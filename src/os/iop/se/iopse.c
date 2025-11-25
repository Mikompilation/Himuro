#include "iopse.h"

#include "adpcm/iopadpcm.h"
#include "iopmain.h"

#include "libsd.h"
#include "sdmacro.h"
#include "stdio.h"

u_int snd_buf_top[] = {
    0x5400,
    0x5F400,
    0x68800,
    0x78400,
    0x84900,
    0x90E00,
    0x9D300,
    0x9F600,
    0xA1900,
    0xA3C00,
    0xA5F00,
    0xA8200,
    0xAA500,
    0xAC800,
    0xAEB00,
    0xC2F00,
    0xD7300,
    0x115700,
    0x153B00,
    0x191F00,
    0x1A4F00,
    0x1B7F00,
    0x1BF700,
    0x1CC700,
    0x1D9700,
    0x1E6700,
    0x1F3700,
    0x1F3740,
    0x1F6740,
    0x1F6780
};

static SE_WRK_SET se_wrk_set[24];
SE_START_POINT se_start_point;
u_int se_start_flg;
u_int se_stop_flg;

static SE_VSTAT* GetSeVstat(int sv_no);
static SE_WRK_SET* GetSeWrkSetP(int v_no);
static void SeSetSeWrk(SE_WRK_SET* swsp);
static void ISeEndCheck();

static int ISePlay(IOP_COMMAND* icp);
static void ISeStop(IOP_COMMAND* icp);
static void ISeVol(IOP_COMMAND* icp);
static void ISeEfctVol(IOP_COMMAND* icp);
static void ISePitch(IOP_COMMAND* icp);
static void ISeAllStop(IOP_COMMAND* icp);
static void ISeQuit();
static void ISePos(IOP_COMMAND* icp);

static void IopSoundMasterVolChange(IOP_COMMAND* icp);
static void IopSoundSteMonoChange(IOP_COMMAND* icp);

static void SeChangeSetDataPlay(SE_WRK_SET* swsp, IOP_COMMAND* icp);
static void SeChangeSetDataPos(SE_WRK_SET* swsp, IOP_COMMAND* icp);

static void GetPrimAndBufNo(short int* prm_no, u_char* buf_no, int v_no);
static void SeGenerateVolPich(SE_WRK_SET* swsp, int vol_rate, int pan, int phase);

static int CidAndVnum(int voice_num, int voice_sift);
static void SeSetMix(int core_id, int v_no, char mix_mode);

void ISeInit(int mode)
{
    sceSdEffectAttr r_attr;
    SE_WRK_SET* swsp;
    int i;

    swsp = GetSeWrkSetP(0);
    for (i = 0; i < 24; i++, swsp++) {
        swsp->pitch = 0x1000;
        swsp->vol_l = swsp->vol_r = 0x1eff;
        swsp->adsr1 = swsp->adsr2 = 0x82ff;
        swsp->prm_no = -1;
        swsp->v_no = i;
        swsp->buf_no = 0;
        SeSetSeWrk(swsp);
    }

    sceSdSetParam(SD_P_MVOLL | SD_CORE_1, iop_mv.vol);
    sceSdSetParam(SD_P_MVOLR | SD_CORE_1, iop_mv.vol);
    sceSdSetAddr(SD_A_EEA | SD_CORE_1, 0x1FFFFFu);
    sceSdSetCoreAttr(3u, 1u);

    r_attr.core = SD_CORE_1;
    r_attr.mode = SD_REV_MODE_CLEAR_WA | SD_REV_MODE_STUDIO_B;
    r_attr.depth_L = 0x1FFF;
    r_attr.depth_R = 0x1FFF;
    r_attr.delay = 0;
    r_attr.feedback = 0;

    sceSdSetEffectAttr(1, &r_attr);
    sceSdSetSwitch(SD_S_VMIXL | SD_CORE_1, 0xFFFFFFu);
    sceSdSetSwitch(SD_S_VMIXR | SD_CORE_1, 0xFFFFFFu);
    sceSdSetSwitch(SD_S_VMIXEL | SD_CORE_1, 0xFFFFFFu);
    sceSdSetSwitch(SD_S_VMIXER | SD_CORE_1, 0xFFFFFFu);
    sceSdSetParam(SD_P_EVOLL | SD_CORE_1, 0x2FFFu);
    sceSdSetParam(SD_P_EVOLR | SD_CORE_1, 0x2FFFu);

    if (!mode) {
        se_start_point.btlhit = -1;
        se_start_point.voice = -1;
        se_start_point.door[0] = -1;
        se_start_point.door[1] = -1;
        se_start_point.door[2] = -1;
        se_start_point.foot[0] = -1;
        se_start_point.foot[1] = -1;
        se_start_point.foot[2] = -1;
        se_start_point.foot[3] = -1;
        se_start_point.foot[4] = -1;
        se_start_point.foot[5] = -1;
        se_start_point.foot[6] = -1;
        se_start_point.foot[7] = -1;
        se_start_point.srund[0] = -1;
        se_start_point.srund[1] = -1;
        se_start_point.ghost[0] = -1;
        se_start_point.ghost[1] = -1;
        se_start_point.ghost[2] = -1;
        se_start_point.event[0] = -1;
        se_start_point.event[1] = -1;
        se_start_point.wide = -1;
        se_start_point.jidou[0] = -1;
        se_start_point.jidou[1] = -1;
        se_start_point.jidou[2] = -1;
        se_start_point.jidou[3] = -1;
    }
}

void ISeMain()
{
    ISeEndCheck();
}

static void ISeEndCheck()
{
    SE_VSTAT* svsp;
    int i;
    int j;
    u_int core_flg[2];

    svsp = GetSeVstat(0);
    for (i = 0; i < 2; ++i) {
        core_flg[i] = sceSdGetSwitch(i | SD_S_ENDX);
        for (j = 0; j < 24; ++j) {
            if (24 * i + j >= 24 && 24 * i + j < 48) {
                if (((core_flg[i] >> j) & 1) != 0 && svsp->status == 1)
                    svsp->status = 0;
                svsp++;
            }
        }

        sceSdSetSwitch(i | SD_S_ENDX, 0);
    }
}

static void ISeWrkUpdate()
{
    SE_WRK_SET* swsp;
    SE_VSTAT* svsp;
    int i;

    swsp = GetSeWrkSetP(0);
    svsp = GetSeVstat(0);
    for (i = 0; i < 24; ++i) {
        if (!svsp->status)
            swsp->prm_no = -1;
        ++swsp;
        ++svsp;
    }
}

void ISeCmd(IOP_COMMAND* icp)
{
    ISeWrkUpdate();

    switch (icp->cmd_no) {
    case IC_SE_INIT:
        ISeInit(icp->data1);
        break;
    case IC_SE_PLAY:
        ISePlay(icp);
        break;
    case IC_SE_STOP:
        ISeStop(icp);
        break;
    case IC_SE_POS:
        ISePos(icp);
        break;
    case IC_SE_EVOL:
        ISeEfctVol(icp);
        break;
    case IC_SE_VOL:
        ISeVol(icp);
        break;
    case IC_SE_PITCH:
        ISePitch(icp);
        break;
    case IC_SE_ALLSTOP:
        ISeAllStop(icp);
        break;
    case IC_SE_MVOL:
        IopSoundMasterVolChange(icp);
        break;
    case IC_SE_STE_MONO:
        IopSoundSteMonoChange(icp);
        break;
    case IC_SE_QUIT:
        ISeQuit();
        break;
    }
}

static void IopSoundMasterVolChange(IOP_COMMAND* icp)
{
    SeSetMasterVol(icp->data1);
    IaSetMasterVol(icp->data1);
}

static void IopSoundSteMonoChange(IOP_COMMAND* icp)
{
    if (icp->data1) {
        iop_mv.mono = 1;
    } else {
        iop_mv.mono = 0;
    }

    IaSetSteMono();
}

static int ISePlay(IOP_COMMAND* icp)
{
    int vn;
    SE_WRK_SET* swsp;

    swsp = GetSeWrkSetP(icp->data1);
    SeChangeSetDataPlay(swsp, icp);
    SeSetSeWrk(swsp);
    vn = swsp->v_no + 24;

    if (vn >= 0 && vn < 24) {
        sceSdSetSwitch(SD_S_KON | SD_CORE_0, 1 << vn);
        if ((swsp->param->attribute & 0x100) != 0)
            GetSeVstat(swsp->v_no)->status = 2;
        else
            GetSeVstat(swsp->v_no)->status = 1;

        return swsp->v_no;
    } else if (vn >= 24 && vn < 48) {
        se_start_flg |= 1 << (vn - 24);
        if ((swsp->param->attribute & 0x100) != 0)
            GetSeVstat(swsp->v_no)->status = 2;
        else
            GetSeVstat(swsp->v_no)->status = 1;

        return swsp->v_no;
    }

    return -1;
}

static void ISeStop(IOP_COMMAND* icp)
{
    SE_WRK_SET* swsp;
    SE_VSTAT* svp;
    int vn;
    int candv;

    swsp = GetSeWrkSetP(icp->data1);
    svp = GetSeVstat(swsp->v_no);
    if (!swsp) {
        return;
    }

    vn = swsp->v_no + 24;
    candv = CidAndVnum(vn, 1);
    if (vn >= 0 && vn < 24) {
        svp->status = 0;
        swsp->prm_no = -1;
        swsp->param = 0;
        sceSdSetSwitch(SD_S_KOFF | SD_CORE_0, 1 << vn);
        GetSeVstat(swsp->v_no)->status = 0;

        return;
    } else if (vn >= 24 && vn < 48) {
        svp->status = 0;
        swsp->prm_no = -1;
        swsp->param = 0;
        se_stop_flg |= 1 << (vn - 24);
        GetSeVstat(swsp->v_no)->status = 0;

        return;
    }
}

static void ISeVol(IOP_COMMAND* icp)
{
    SE_WRK_SET* swsp;
    int vn;
    int candv;

    swsp = GetSeWrkSetP(icp->data1);
    if (!swsp) {
        return;
    }

    vn = swsp->v_no + 24;
    candv = CidAndVnum(vn, 1);
    swsp->pitch = swsp->param->pitch * icp->data2 / 4096;
    sceSdSetParam(candv | 0x200, swsp->pitch);
}

static void ISeEfctVol(IOP_COMMAND* icp)
{
    u_short evol;

    evol = icp->data1 & 0xffff;
    sceSdSetParam(SD_P_EVOLL | SD_CORE_1, evol);
    sceSdSetParam(SD_P_EVOLR | SD_CORE_1, evol);
}

static void ISePitch(IOP_COMMAND* icp)
{
    SE_WRK_SET* swsp;
    int vn;
    int candv;

    swsp = GetSeWrkSetP(icp->data1);
    if (!swsp) {
        return;
    }

    vn = swsp->v_no + 24;
    candv = CidAndVnum(vn, 1);
    swsp->pitch = swsp->param->pitch * icp->data2 / 4096;
    sceSdSetParam(candv | SD_VP_PITCH, swsp->pitch);
}

static void ISeAllStop(IOP_COMMAND* icp)
{
    int i;
    int candv;
    int vn;
    SE_WRK_SET* swsp;
    SE_VSTAT* svp;

    swsp = GetSeWrkSetP(icp->data1);
    svp = GetSeVstat(swsp->v_no);
    i = 0;
    swsp = GetSeWrkSetP(0);
    while (i < 24) {
        vn = i + 24;
        svp = GetSeVstat(swsp->v_no);
        candv = CidAndVnum(vn, 1);
        if (vn >= 24 && vn < 48) {
            svp->status = 0;
            swsp->prm_no = -1;
            swsp->param = 0;
            GetSeVstat(swsp->v_no)->status = 0;
            printf("(SE)Key-Off num = %d\n", vn);
        }
        i++;
        swsp++;
    }

    se_stop_flg |= 0xffffff;
}

static void ISeQuit() {}

static void ISePos(IOP_COMMAND* icp)
{
    SE_WRK_SET* swsp;
    int candv;

    swsp = GetSeWrkSetP(icp->data1);
    SeChangeSetDataPos(swsp, icp);
    candv = CidAndVnum(swsp->v_no + 24, 1);
    sceSdSetParam(candv | SD_VP_VOLL, swsp->vol_l);
    sceSdSetParam(candv | SD_VP_VOLR, swsp->vol_r);
    sceSdSetParam(candv | SD_VP_PITCH, swsp->pitch);
}

static void SeChangeSetDataPlay(SE_WRK_SET* swsp, IOP_COMMAND* icp)
{
    GetPrimAndBufNo(&swsp->prm_no, &swsp->buf_no, icp->data2);
    swsp->v_no = icp->data1;
    swsp->param = SeGetSeParamTbl(swsp->prm_no);
    swsp->pitch = (icp->data3 * swsp->param->pitch) / 4096;
    swsp->attribute = swsp->param->attribute;
    SeGenerateVolPich(swsp, icp->data5, icp->data4, 0);
    swsp->adsr1 = swsp->param->adsr1;
    swsp->adsr2 = swsp->param->adsr2;
    swsp->inc_vol_l = 0;
    swsp->inc_vol_r = 0;
    swsp->vol_l = swsp->tgt_vol_l;
    swsp->vol_r = swsp->tgt_vol_r;
}

static void SeChangeSetDataPos(SE_WRK_SET* swsp, IOP_COMMAND* icp)
{
    GetPrimAndBufNo(&swsp->prm_no, &swsp->buf_no, icp->data2);
    swsp->param = SeGetSeParamTbl(swsp->prm_no);
    swsp->pitch = icp->data3 * swsp->param->pitch / 4096;
    SeGenerateVolPich(swsp, icp->data5, icp->data4, 0);
    swsp->vol_l = swsp->tgt_vol_l;
    swsp->vol_r = swsp->tgt_vol_r;
}

static void GetPrimAndBufNo(short int* prm_no, u_char* buf_no, int v_no)
{
    if (v_no >= 0 && v_no < 30) {
        *prm_no = v_no;
        *buf_no = 0;
    } else if (v_no < 33) {
        *prm_no = v_no + se_start_point.btlhit * 3;
        *buf_no = 1;
    } else if (v_no < 38) {
        *prm_no = v_no + 3 + se_start_point.voice * 5;
        *buf_no = 2;
    } else if (v_no < 40) {
        *prm_no = v_no + 0x30 + se_start_point.door[0] * 2;
        *buf_no = 3;
    } else if (v_no < 42) {
        *prm_no = v_no + 0x2e + se_start_point.door[1] * 2;
        *buf_no = 4;
    } else if (v_no < 44) {
        *prm_no = v_no + 0x2c + se_start_point.door[2] * 2;
        *buf_no = 5;
    } else if (v_no < 45) {
        *prm_no = v_no + 0x38 + se_start_point.foot[0];
        *buf_no = 6;
    } else if (v_no < 46) {
        *prm_no = v_no + 0x37 + se_start_point.foot[1];
        *buf_no = 7;
    } else if (v_no < 47) {
        *prm_no = v_no + 0x36 + se_start_point.foot[2];
        *buf_no = 8;
    } else if (v_no < 48) {
        *prm_no = v_no + 0x35 + se_start_point.foot[3];
        *buf_no = 9;
    } else if (v_no < 49) {
        *prm_no = v_no + 0x34 + se_start_point.foot[4];
        *buf_no = 10;
    } else if (v_no < 50) {
        *prm_no = v_no + 0x33 + se_start_point.foot[5];
        *buf_no = 0xb;
    } else if (v_no < 51) {
        *prm_no = v_no + 0x32 + se_start_point.foot[6];
        *buf_no = 0xc;
    } else if (v_no < 52) {
        *prm_no = v_no + 0x31 + se_start_point.foot[7];
        *buf_no = 0xd;
    } else if (v_no < 53) {
        *prm_no = v_no + 0x4c + se_start_point.srund[0];
        *buf_no = 0xe;
    } else if (v_no < 54) {
        *prm_no = v_no + 0x4b + se_start_point.srund[1];
        *buf_no = 0xf;
    } else if (v_no < 65) {
        *prm_no = v_no + 0x5d + se_start_point.ghost[0] * 0xb;
        *buf_no = 0x10;
    } else if (v_no < 76) {
        *prm_no = v_no + 0x52 + se_start_point.ghost[1] * 0xb;
        *buf_no = 0x11;
    } else if (v_no < 87) {
        *prm_no = v_no + 0x47 + se_start_point.ghost[2] * 0xb;
        *buf_no = 0x12;
    } else if (v_no < 90) {
        *prm_no = v_no + 0x165 + se_start_point.event[0] * 3;
        *buf_no = 0x13;
    } else if (v_no < 93) {
        *prm_no = v_no + 0x162 + se_start_point.event[1] * 3;
        *buf_no = 0x14;
    } else if (v_no < 94) {
        *prm_no = v_no + 0x1aa + se_start_point.wide;
        *buf_no = 0x15;
    } else if (v_no < 95) {
        *prm_no = v_no + 0x1b8 + se_start_point.jidou[0];
        *buf_no = 0x16;
    } else if (v_no < 96) {
        *prm_no = v_no + 0x1b7 + se_start_point.jidou[1];
        *buf_no = 0x17;
    } else if (v_no < 97) {
        *prm_no = v_no + 0x1b6 + se_start_point.jidou[2];
        *buf_no = 0x18;
    } else if (v_no < 98) {
        *prm_no = v_no + 0x1b5 + se_start_point.jidou[3];
        *buf_no = 0x19;
    } else if (v_no == 0xff) {
        return;
    }
}

void SeSetStartPoint(u_char type, u_int no)
{
    switch (type) {
    case SE_ADDRNO_STATIC:
        break;
    case SE_ADDRNO_BTLHIT:
        se_start_point.btlhit = no - 0x547;
        break;
    case SE_ADDRNO_VOICE:
        se_start_point.voice = no - 0x549;
        break;
    case SE_ADDRNO_DOOR0:
        se_start_point.door[0] = no - 0x5C0;
        break;
    case SE_ADDRNO_DOOR1:
        se_start_point.door[1] = no - 0x5C0;
        break;
    case SE_ADDRNO_DOOR2:
        se_start_point.door[2] = no - 0x5C0;
        break;
    case SE_ADDRNO_FOOT0:
        se_start_point.foot[0] = no - 0x5A4;
        break;
    case SE_ADDRNO_FOOT1:
        se_start_point.foot[1] = no - 0x5A4;
        break;
    case SE_ADDRNO_FOOT2:
        se_start_point.foot[2] = no - 0x5A4;
        break;
    case SE_ADDRNO_FOOT3:
        se_start_point.foot[3] = no - 0x5A4;
        break;
    case SE_ADDRNO_FOOT4:
        se_start_point.foot[4] = no - 0x5A4;
        break;
    case SE_ADDRNO_FOOT5:
        se_start_point.foot[5] = no - 0x5A4;
        break;
    case SE_ADDRNO_FOOT6:
        se_start_point.foot[6] = no - 0x5A4;
        break;
    case SE_ADDRNO_FOOT7:
        se_start_point.foot[7] = no - 0x5A4;
        break;
    case SE_ADDRNO_SRUND0:
        se_start_point.srund[0] = no - 0x5D6;
        break;
    case SE_ADDRNO_SRUND1:
        se_start_point.srund[1] = no - 0x5D6;
        break;
    case SE_ADDRNO_GHOST0:
        se_start_point.ghost[0] = no - 0x553;
        break;
    case SE_ADDRNO_GHOST1:
        se_start_point.ghost[1] = no - 0x553;
        break;
    case SE_ADDRNO_GHOST2:
        se_start_point.ghost[2] = no - 0x553;
        break;
    case SE_ADDRNO_EVENT0:
        se_start_point.event[0] = no - 0x577;
        break;
    case SE_ADDRNO_EVENT1:
        se_start_point.event[1] = no - 0x577;
        break;
    case SE_ADDRNO_WIDE:
        se_start_point.wide = no - 0x5E9;
        break;
    case SE_ADDRNO_JIDOU0:
        se_start_point.jidou[0] = no - 0x596;
        break;
    case SE_ADDRNO_JIDOU1:
        se_start_point.jidou[1] = no - 0x596;
        break;
    case SE_ADDRNO_JIDOU2:
        se_start_point.jidou[2] = no - 0x596;
        break;
    case SE_ADDRNO_JIDOU3:
        se_start_point.jidou[3] = no - 0x596;
        break;
    }
}

static void SeGenerateVolPich(SE_WRK_SET* swsp, int vol_rate, int pan, int phase)
{
    swsp->tgt_vol_l = swsp->tgt_vol_r = ((0x1ff * vol_rate * swsp->param->vol) / 0x1000) >> 2;

    if (iop_mv.mono == 0) {
        if ((swsp->param->attribute & 0x30) == 0x10) {
            if (pan >= 641) {
                swsp->tgt_vol_l *= ((pan - 640) / 640);
                swsp->tgt_vol_r *= ((1270 - pan) / 640);
            } else {
                swsp->tgt_vol_r = 0;
            }
        } else if ((swsp->param->attribute & 0x30) == 0x20) {
            if (pan < 640) {
                swsp->tgt_vol_r *= (pan / 640);
                swsp->tgt_vol_l *= ((640 - pan) / 640);
            } else {
                swsp->tgt_vol_l = 0;
            }
        } else if (pan < 640) {
            swsp->tgt_vol_r = swsp->tgt_vol_l * pan / 640;
        } else {
            swsp->tgt_vol_l = swsp->tgt_vol_r * (1270 - pan) / 640;
        }
    } else {
        if ((swsp->param->attribute & 0x30) != 0) {
            swsp->tgt_vol_l >>= 1;
            swsp->tgt_vol_r >>= 1;
        } else {
        }
    }
}

static SE_VSTAT* GetSeVstat(int sv_no)
{
    if (sv_no >= 0 && sv_no < 24) {
        return &GetIopStatP()->sev_stat[sv_no];
    } else {
        return NULL;
    }
}

static SE_WRK_SET* GetSeWrkSetP(int v_no)
{
    if (v_no >= 0 && v_no < 24) {
        return &se_wrk_set[v_no];
    } else {
        return NULL;
    }
}

static void SeSetSeWrk(SE_WRK_SET* swsp)
{
    int num;
    int candv;

    num = swsp->v_no;
    candv = CidAndVnum(num + 24, 1);
    sceSdSetAddr(candv | SD_VA_SSA, GetSeAdrs(swsp->prm_no) + snd_buf_top[swsp->buf_no]);
    sceSdSetParam(candv | SD_VP_ADSR1, swsp->adsr1);
    sceSdSetParam(candv | SD_VP_ADSR2, swsp->adsr2);
    sceSdSetParam(candv | SD_VP_VOLL, swsp->vol_l);
    sceSdSetParam(candv | SD_VP_VOLR, swsp->vol_r);
    sceSdSetParam(candv | SD_VP_PITCH, swsp->pitch);
    SeSetMix(1, swsp->v_no, swsp->param->efct);
}

static void SeSetMix(int core_id, int v_no, char mix_mode)
{
    u_int reg;

    if ((mix_mode & 1) != 0) {
        reg = sceSdGetSwitch(core_id | SD_S_VMIXL);
        reg |= 1 << v_no;
        sceSdSetSwitch(core_id | SD_S_VMIXL, reg);

        reg = sceSdGetSwitch(core_id | SD_S_VMIXR);
        reg |= (1 << v_no);
        sceSdSetSwitch(core_id | SD_S_VMIXR, reg);
    } else {
        reg = sceSdGetSwitch(core_id | SD_S_VMIXL);
        reg &= ~(1 << v_no);
        sceSdSetSwitch(core_id | SD_S_VMIXL, reg);

        reg = sceSdGetSwitch(core_id | SD_S_VMIXR);
        reg &= ~(1 << v_no);
        sceSdSetSwitch(core_id | SD_S_VMIXR, reg);
    }

    if ((mix_mode & 2) != 0) {
        reg = sceSdGetSwitch(core_id | SD_S_VMIXEL);
        reg |= 1 << v_no;
        sceSdSetSwitch(core_id | SD_S_VMIXEL, reg);

        reg = sceSdGetSwitch(core_id | SD_S_VMIXER);
        reg |= (1 << v_no);
        sceSdSetSwitch(core_id | SD_S_VMIXER, reg);
    } else {
        reg = sceSdGetSwitch(core_id | SD_S_VMIXEL);
        reg &= ~(1 << v_no);
        sceSdSetSwitch(core_id | SD_S_VMIXEL, reg);

        reg = sceSdGetSwitch(core_id | SD_S_VMIXER);
        reg &= ~(1 << v_no);
        sceSdSetSwitch(core_id | SD_S_VMIXER, reg);
    }
}

static int CidAndVnum(int voice_num, int voice_sift)
{
    if (voice_num >= 0 && voice_num < 24) {
        return voice_num << voice_sift;
    } else if (voice_num >= 24 && voice_num < 48) {
        return ((voice_num - 24) << voice_sift) | 1;
    }

    return -1;
}

u_int SeGetSndBufTop(int pos)
{
    return snd_buf_top[pos];
}

void SeSetMasterVol(u_short mvol)
{
    sceSdSetParam(SD_P_MVOLL | SD_CORE_1, mvol & 0x3FFF);
    sceSdSetParam(SD_P_MVOLR | SD_CORE_1, mvol & 0x3FFF);
}
