#ifndef IOPCDVD_H_
#define IOPCDVD_H_

#include "iopmain.h"
#include "libcdvd.h"

typedef struct { // 0x20
    /* 0x00 */ int req_type;
    /* 0x04 */ int file_no;
    /* 0x08 */ int start_sector;
    /* 0x0c */ int size_sector;
    /* 0x10 */ int size_byte;
    /* 0x10:32 */ u_int id : 32;
    /* 0x18 */ u_int* taddr;
    /* 0x1c */ u_char se_buf_no;
    /* 0x1d */ u_char tmem;
} CDVD_REQ_BUF;

typedef struct { // 0x10
    /* 0x0 */ u_int* taddr;
    /* 0x4 */ int start;
    /* 0x8 */ int file_no;
    /* 0xc */ int size_now;
} CDVD_PCM;

typedef struct { // 0x18
    /* 0x00 */ u_int* taddr;
    /* 0x04 */ int start;
    /* 0x08 */ int file_no;
    /* 0x0c */ int size_now;
    /* 0x10 */ int read_now;
    /* 0x14 */ u_char req_type;
    /* 0x15 */ u_char call_play;
    /* 0x16 */ u_char endld_flg;
    /* 0x17 */ u_char now_load;
} CDVD_ADPCM;

typedef struct { // 0xc
    /* 0x0 */ int id;
    /* 0x4 */ int tid;
    /* 0x8 */ u_char ltrans;
    /* 0x9 */ u_char stat;
    /* 0xa */ u_char tmem;
} CDVD_TRANS_STAT;

typedef struct { // 0x94
    /* 0x00 */ sceCdRMode rmode;
    /* 0x04 */ sceCdlFILE cdlf;
    /* 0x28 */ CDVD_PCM pcm;
    /* 0x38 */ CDVD_ADPCM adpcm[2];
    /* 0x60:64 */ u_int ctime : 32;
    /* 0x6c */ int stat;
    /* 0x70 */ int start;
    /* 0x74 */ int id;
    /* 0x78 */ int file_no;
    /* 0x7c */ int size;
    /* 0x80 */ int now_size;
    /* 0x84 */ int end_size;
    /* 0x88 */ u_char start_pos;
    /* 0x89 */ u_char req_pos;
    /* 0x8a */ u_char buf_use_num;
    /* 0x8b */ u_char now_lbuf;
    /* 0x8c */ u_char cmd_on;
    /* 0x8d */ u_char vtrans_flg;
    /* 0x8e */ u_char adpcm_req;
    /* 0x8f */ u_char pcm_req;
    /* 0x90 */ u_char pcm_pre;
    /* 0x91 */ u_char pcm_pre_end;
    /* 0x92 */ u_char error_cnt;
} CDVD_STAT;

enum CDVD_STATUS {
    CDVD_STAT_FREE = 0,
    CDVD_STAT_LOADING = 1,
    CDVD_STAT_STREAMING = 2,
    CDVD_STAT_TRANS_WAIT = 3,
    CDVD_STAT_SEEKING = 4
};

enum ADPCM_LOADREQ_TYPE {
    ADPCM_LDREQ_NO = 0,
    ADPCM_LDREQ_PRELOAD_ONLY = 1,
    ADPCM_LDREQ_STREAM = 2
};

enum CDVD_LOAD_STATUS {
    CDVD_LS_FINISHED = 0,
    CDVD_LS_WAIT = 1,
    CDVD_LS_TRANS = 2,
    CDVD_LS_TRANS_END = 3,
    CDVD_LS_LOADING = 4
};

extern CDVD_STAT cdvd_stat;

void ICdvdInit(int reset);
void ICdvdCmd(IOP_COMMAND* icp);
void ICdvdMain();
void ICdvdBreak();

void ICdvdLoadReqAdpcm(int lsn, u_int size_now, void* buf, u_char channel, int req_type, int endld_flg);

#endif // IOPCDVD_H_
