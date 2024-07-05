#ifndef OS_EEIOP_SE_PLYR_H
#define OS_EEIOP_SE_PLYR_H

#include "typedefs.h"

typedef struct {
    u_char num;
    u_char rate[4];
} SE_FOOT_RAND_ITA;

extern SE_FOOT_RAND_ITA rand_ita_tbl[];

int SeReqFootStep(float *pos);
u_char IsKarehaZone();
u_char IsTakenohaZone();
u_char IsHokoriZone();

#endif // OS_EEIOP_SE_PLYR_H
