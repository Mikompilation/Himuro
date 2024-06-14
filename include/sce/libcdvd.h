#ifndef SCE_LIBCDVD_H
#define SCE_LIBCDVD_H

#include "typedefs.h"

typedef struct {
        u_char stat;
        u_char second;
        u_char minute;
        u_char hour;
        u_char pad;
        u_char day;
        u_char month;
        u_char year;
} sceCdCLOCK;

int sceCdInit(int init_mode);
int sceCdMmode(int media);
int sceCdReadClock(sceCdCLOCK *rtc);

#endif // SCE_LIBCDVD_H