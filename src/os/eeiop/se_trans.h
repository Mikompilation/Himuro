#ifndef OS_EEIOP_SE_TRANS_H
#define OS_EEIOP_SE_TRANS_H

#include "typedefs.h"

typedef struct { // 0x10
    /* 0x0 */ u_int dma_id;
    /* 0x4 */ u_int size;
    /* 0x8 */ int file_no;
    /* 0xc */ u_char trans_pos;
    /* 0xd */ u_char fg_no;
    /* 0xe */ u_char mode;
} SE_TRANS_CTRL;

typedef struct { // 0x8
    /* 0x0 */ int file_no;
    /* 0x4 */ u_char fg_no;
} FG_FILE_TBL;

extern FG_FILE_TBL fgf_tbl[];
extern SE_TRANS_CTRL st_ctrl;

void FGTransInit();
u_char IsEndFgTrans();
void FloatGhostSENotEmpty();
int SeFGhostTransCtrl();

#endif // OS_EEIOP_SE_TRANS_H
