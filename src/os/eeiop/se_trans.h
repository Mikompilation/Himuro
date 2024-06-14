#ifndef OS_EEIOP_SE_TRANS_H
#define OS_EEIOP_SE_TRANS_H

#include "typedefs.h"

// extern FG_FILE_TBL fgf_tbl[0];
// extern SE_TRANS_CTRL st_ctrl;

void FGTransInit();
u_char IsEndFgTrans();
void FloatGhostSENotEmpty();
int SeFGhostTransCtrl();

#endif // OS_EEIOP_SE_TRANS_H
