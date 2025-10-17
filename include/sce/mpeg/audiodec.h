#ifndef SCE_MPEG_AUDIODEC_H
#define SCE_MPEG_AUDIODEC_H

#include "typedefs.h"

#define VOLUME_MASTER_MAX  0x3fff

#define AU_STATE_INIT		0
#define AU_STATE_PRESET		1
#define AU_STATE_PLAY		2
#define AU_STATE_PAUSE		3

#define AUTODMA_CH 0

#define UNIT_SIZE 1024

typedef struct { // 0x20
    /* 0x00 */ char id[4];
    /* 0x04 */ int size;
    /* 0x08 */ int type;
    /* 0x0c */ int rate;
    /* 0x10 */ int ch;
    /* 0x14 */ int interSize;
    /* 0x18 */ int loopStart;
    /* 0x1c */ int loopEnd;
} SpuStreamHeader;

typedef struct { // 0x8
    /* 0x0 */ char id[4];
    /* 0x4 */ int size;
} SpuStreamBody;

typedef struct { // 0x5c
    /* 0x00 */ int state;
    /* 0x04 */ SpuStreamHeader sshd;
    /* 0x24 */ SpuStreamBody ssbd;
    /* 0x2c */ int hdrCount;
    /* 0x30 */ u_char *data;
    /* 0x34 */ int put;
    /* 0x38 */ int count;
    /* 0x3c */ int size;
    /* 0x40 */ int totalBytes;
    /* 0x44 */ int iopBuff;
    /* 0x48 */ int iopBuffSize;
    /* 0x4c */ int iopLastPos;
    /* 0x50 */ int iopPausePos;
    /* 0x54 */ int totalBytesSent;
    /* 0x58 */ int iopZero;
} AudioDec;

#endif // SCE_MPEG_AUDIODEC_H