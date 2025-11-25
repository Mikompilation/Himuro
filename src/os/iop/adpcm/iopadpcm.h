#ifndef IOPADPCM_H_
#define IOPADPCM_H_

#include "iopmain.h"

typedef struct { // 0x2c
    /* 0x00:00 */ unsigned int fsize : 32;
    /* 0x00:32 */ unsigned int asize : 32;
    /* 0x00:64 */ unsigned int offset : 32;
    /* 0x00:96 */ unsigned int pos : 32;
    /* 0x10 */ int pcm_no;
    /* 0x14 */ int intr;
    /* 0x18 */ int file;
    /* 0x1c */ short int vol_reg;
    /* 0x20 */ char* buf;
    /* 0x24 */ unsigned char stat;
    /* 0x25 */ char pre_load;
    /* 0x26 */ char vol;
    /* 0x27 */ char dbufid;
    /* 0x28 */ char loop;
} IOP_PCM;

enum IOP_PCM_STAT {
    IPS_IDOL = 0,
    IPS_PRELOAD = 1,
    IPS_PRELOADED = 2,
    IPS_PLAY = 3,
    IPS_PAUSE = 4,
    IPS_LOAD = 5,
    IPS_STOP = 6
};

typedef struct { // 0x60
    /* 0x00 */ int thread_id;
    /* 0x04 */ int first;
    /* 0x08 */ int start;
    /* 0x0c */ int szFile_bk;
    /* 0x10 */ int szFile;
    /* 0x14 */ int str_pos;
    /* 0x18 */ int str_lpos;
    /* 0x1c */ int str_tpos;
    /* 0x20 */ int lreq_size;
    /* 0x24 */ int pos;
    /* 0x28 */ int dbidi;
    /* 0x2c */ int dbids;
    /* 0x30 */ int fade_count;
    /* 0x34 */ int fade_flm;
    /* 0x38 */ int count;
    /* 0x3c */ int stop_cnt;
    /* 0x40 */ short int tune_no;
    /* 0x42 */ u_short target_vol;
    /* 0x44 */ u_short vol;
    /* 0x46 */ u_short pan;
    /* 0x48 */ u_short vol_ll;
    /* 0x4a */ u_short vol_lr;
    /* 0x4c */ u_short vol_rl;
    /* 0x4e */ u_short vol_rr;
    /* 0x50 */ u_short pitch;
    /* 0x52 */ u_short core;
    /* 0x54 */ u_short vl;
    /* 0x56 */ u_short vr;
    /* 0x58 */ u_char fade_mode;
    /* 0x59 */ u_char use;
    /* 0x5a */ u_char stat;
    /* 0x5b */ u_char load_type;
    /* 0x5c */ u_char loop;
    /* 0x5d */ u_char loop_end;
} IOP_ADPCM;

typedef struct { // 0x20
    /* 0x00 */ int first;
    /* 0x04 */ int size;
    /* 0x08 */ int start_cnt;
    /* 0x0c */ u_short fade_flm;
    /* 0x0e */ u_short tune_no;
    /* 0x10 */ u_short vol;
    /* 0x12 */ u_short start_vol;
    /* 0x14 */ u_short target_vol;
    /* 0x16 */ u_short pan;
    /* 0x18 */ u_short pitch;
    /* 0x1a */ u_char cmd_type;
    /* 0x1b */ u_char loop;
    /* 0x1c */ u_char channel;
    /* 0x1d */ u_char fade_mode;
} ADPCM_CMD;

typedef struct { // 0xc
    /* 0x0 */ u_short ll;
    /* 0x2 */ u_short lr;
    /* 0x4 */ u_short rl;
    /* 0x6 */ u_short rr;
    /* 0x8 */ u_short master_vol;
    /* 0xa */ u_short pan;
} ADPCM_POS_CALC;

enum ADPCM_FADE_MODE {
    ADPCM_FADE_NO = 0,
    ADPCM_FADE_IN_PLAY = 1,
    ADPCM_FADE_OUT_STOP = 2,
    ADPCM_FADE = 3
};

enum ADPCM_CMD_TYPE {
    AC_NONE = 0,
    AC_PLAY = 1,
    AC_PRELOAD = 2,
    AC_STOP = 3,
    AC_PAUSE = 4,
    AC_RESTART = 5
};

enum ADPCM_PLAY_STAT {
    ADPCM_STAT_NOPLAY = 0,
    ADPCM_STAT_FULL_STOP = 1,
    ADPCM_STAT_LOOPEND_STOP = 2,
    ADPCM_STAT_PRELOAD = 3,
    ADPCM_STAT_PRELOAD_TRANS = 4,
    ADPCM_STAT_PRELOAD_END = 5,
    ADPCM_STAT_PLAY = 6,
    ADPCM_STAT_LTRANS = 7,
    ADPCM_STAT_RTRANS = 8,
    ADPCM_STAT_PAUSE = 9,
    ADPCM_STAT_ERROR = -1
};

extern IOP_ADPCM iop_adpcm[2];
extern u_char* AdpcmIopBuf[2];
extern u_char* AdpcmSpuBuf[2];
extern ADPCM_CMD now_cmd;
extern ADPCM_CMD cmd_buf[8];

void IAdpcmInit(int dev_init);
void IAdpcmCmd(IOP_COMMAND* icp);
void IAdpcmMain();
void IAdpcmMain2();

void IaSetSteMono();

void SetLoopFlgSize(u_int size_byte, u_int* start, u_short core);
void IAdpcmLoadEndStream(int channel);
void IAdpcmLoadEndPreOnly(int channel);

void IAdpcmPlay(ADPCM_CMD* acp);
void IAdpcmStop(ADPCM_CMD* acp);
void IAdpcmPreLoad(ADPCM_CMD* acp);
void IaSetWrkVolPanPitch(u_char channel, u_short pan, u_short master_vol, u_short pitch);
void IaSetWrkFadeParam(u_char channel, int fade_flm, u_short target_vol);
void IaSetWrkFadeMode(u_char channel, u_char mode, u_short target_vol, int fade_flm);
void IaSetRegVol(u_char channel);
void DbgDispAdpcmCmdWrk(ADPCM_CMD* acp);
void IaDbgMemoryCheck();
void IaSetRegKoff(u_char channel);
void IaInitEffect();
void IaInitVolume();
void IaInitDev(u_char channel);
void IaSetRegPitch(u_char channel);
void IaSetRegAdsr(u_char channel);
void IaSetRegSsa(u_char channel);
void IaSetRegKon(u_char channel);

void IAdpcmReadCh0();
void IAdpcmReadCh1();

void IaSetWrkFadeInit(u_char channel);
void IaSetWrkFadeParam(u_char channel, int fade_flm, u_short target_vol);
void IaSetMasterVol(u_short mvol);

u_char IAdpcmChkCmdExist();
void InitAdpcmCmdBuf();
void IAdpcmCmdSlide();
void IAdpcmCmdPlay();
void IAdpcmCmdStop();
void IAdpcmCmdPreLoad();
void IAdpcmCmdPause();
void IAdpcmCmdRestart();

#endif // IOPADPCM_H_
