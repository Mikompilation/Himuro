#ifndef IOPMAIN_H_
#define IOPMAIN_H_

typedef unsigned char u_char;
typedef short unsigned int u_short;
typedef unsigned int u_int;
typedef long unsigned int u_long;
typedef unsigned char unchar;
typedef short unsigned int ushort;
typedef unsigned int uint;
typedef long unsigned int ulong;

typedef signed char s_char;
typedef short int s_short;
typedef long int s_long;

enum IOP_COMMAND_ID {
    IC_COM_NOTHING = 0,
    IC_SE_INIT = 1,
    IC_SE_PLAY = 2,
    IC_SE_POS = 3,
    IC_SE_STOP = 4,
    IC_SE_VOL = 5,
    IC_SE_EVOL = 6,
    IC_SE_PITCH = 7,
    IC_SE_ALLSTOP = 8,
    IC_SE_MVOL = 9,
    IC_SE_STE_MONO = 10,
    IC_SE_QUIT = 11,
    IC_CDVD_INIT = 12,
    IC_CDVD_LOAD = 13,
    IC_CDVD_LOAD_SECT = 14,
    IC_CDVD_SEEK = 15,
    IC_CDVD_SE_TRANS = 16,
    IC_CDVD_SE_TRANS_RESET = 17,
    IC_CDVD_BREAK = 18,
    IC_BGM_INIT = 19,
    IC_BGM_PLAY = 20,
    IC_BGM_STOP = 21,
    IC_BGM_PAUSE = 22,
    IC_BGM_RESTART = 23,
    IC_BGM_VOL = 24,
    IC_BGM_QUIT = 25,
    IC_ADPCM_INIT = 26,
    IC_ADPCM_PLAY = 27,
    IC_ADPCM_PRELOAD = 28,
    IC_ADPCM_LOADEND_PLAY = 29,
    IC_ADPCM_STOP = 30,
    IC_ADPCM_PAUSE = 31,
    IC_ADPCM_RESTART = 32,
    IC_ADPCM_FADE_VOL = 33,
    IC_ADPCM_POS = 34,
    IC_ADPCM_MVOL = 35,
    IC_ADPCM_SET_SPU2IRQ = 36,
    IC_ADPCM_QUIT = 37
};

enum IOP_STEREO_SET {
    IS_STEREO = 0,
    IS_MONAURAL = 1
};

enum IOP_CMD_MODE {
    ICM_INIT = 0,
    ICM_REQ = 1,
    ICM_GET_STAT_ONLY = 2
};

enum TRANS_MEM_TYPE {
    TRANS_MEM_EE = 0,
    TRANS_MEM_IOP = 1,
    TRANS_MEM_SPU = 2,
    TRANS_PCM = 3,
    TRANS_MEM_NUM = 4
};

typedef struct { // 0x20
    /* 0x00 */ int cmd_no;
    /* 0x04 */ int data1;
    /* 0x08 */ int data2;
    /* 0x0c */ int data3;
    /* 0x10 */ int data4;
    /* 0x14 */ int data5;
    /* 0x18 */ int data6;
    /* 0x1c */ int data7;
} IOP_COMMAND;

typedef struct { // 0x414
    /* 0x000 */ int get_cmd[256];
    /* 0x400 */ int cmd_num;
    /* 0x404 */ int timer_id;
    /* 0x408 */ int thread_id;
    /* 0x40c */ int adpcm_thid;
    /* 0x410 */ unsigned int count;
} IOP_SYS_CTRL;

typedef struct { // 0x8
    /* 0x0 */ int file_no;
    /* 0x4 */ u_char stat;
    /* 0x5 */ u_char renew;
} CDVD_LOAD_STAT;

typedef struct { // 0x10c
    /* 0x000 */ CDVD_LOAD_STAT fstat[32];
    /* 0x100 */ int stat;
    /* 0x100 */ u_int ld_addr;
    /* 0x108 */ int se_trans;
} CDVD_RET_STAT;

typedef struct { // 0x4
    /* 0x0 */ int status;
} SE_VSTAT;

typedef struct { // 0x8
    /* 0x0 */ int thread_id;
    /* 0x4 */ int stat;
} BGM_STAT;

typedef struct { // 0x8
    /* 0x0 */ short int status;
    /* 0x2 */ short int tune_no;
    /* 0x4 */ int count;
} ADPCM_STAT;

typedef struct { // 0x180
    /* 0x000 */ SE_VSTAT sev_stat[24];
    /* 0x060 */ BGM_STAT bgm;
    /* 0x068 */ CDVD_RET_STAT cdvd;
    /* 0x174 */ ADPCM_STAT adpcm;
    /* 0x17c */ int dummy[1];
} IOP_STAT;

typedef struct { // 0x4
    /* 0x0 */ u_short vol;
    /* 0x2 */ u_char mono;
} IOP_MASTER_VOL;

extern IOP_STAT iop_stat;
extern IOP_MASTER_VOL iop_mv;
IOP_STAT* GetIopStatP();

#endif // IOPMAIN_H_
