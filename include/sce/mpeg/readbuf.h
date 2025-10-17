#ifndef SCE_MPEG_READBUF_H
#define SCE_MPEG_READBUF_H

#include "typedefs.h"

#define READ_UNIT_SIZE (64*1024)
#define N_READ_UNIT     5
#define READBUF_BUFF_SIZE (N_READ_UNIT * READ_UNIT_SIZE)

typedef struct
{ // 0x5000c
    /* 0x00000 */ u_char data[N_READ_UNIT * READ_UNIT_SIZE];
    /* 0x50000 */ int put;
    /* 0x50004 */ int count;
    /* 0x50008 */ int size;
} ReadBuf;

#endif // SCE_MPEG_READBUF_H