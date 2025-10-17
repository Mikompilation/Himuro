#ifndef SCE_MPEG_DEFS_H
#define SCE_MPEG_DEFS_H

#include "typedefs.h"

#ifdef BUILD_EU_VERSION
#define MAX_WIDTH 640
#define MAX_HEIGHT 448
#else
#define MAX_WIDTH 640
#define MAX_HEIGHT 480
#endif

// these are the same for both US and EU versions of the game
#define MPEG_DISP_WIDTH 640
#define MPEG_DISP_HEIGHT 480

#define bound(val, x) ((((val) + (x) - 1) / (x))*(x))

#define N_VOBUF 2

#define N_LDTAGS (MAX_WIDTH/16 * MAX_HEIGHT/16 * 6 + 10)

#define TS_NONE -1

static inline void *DmaAddr(void *val)
{
    return (void*)((u_int)val & 0x0fffffff);
}

static inline void *UncAddr(void *val)
{
    return (void*)(((u_int)val & 0x0fffffff) | 0x20000000);
}

void clearGsMem(int r, int g, int b, int disp_width, int disp_height);

#endif // SCE_MPEG_DEFS_H