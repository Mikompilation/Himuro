#ifndef SCE_LIBDMA_H
#define SCE_LIBDMA_H

#include "typedefs.h"

typedef struct {
    u_char  sts;
    u_char  std;
    u_char  mfd;
    u_char  rcycle;
    u_short express;
    u_short notify;
    u_short sqwc;
    u_short tqwc;
    void    *rbadr;
    u_int   rbmsk;
} sceDmaEnv;

int sceDmaReset(int mode);
sceDmaEnv *sceDmaGetEnv(sceDmaEnv *env);
int sceDmaPutEnv(sceDmaEnv *env);

#endif // SCE_LIBDMA_H