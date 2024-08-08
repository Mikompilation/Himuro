#ifndef GRAPHICS_GRAPH3D_LIBSG_H
#define GRAPHICS_GRAPH3D_LIBSG_H

#include "typedefs.h"

static inline void sceVu0CopyVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
    __asm__ __volatile__("\n\
    lq    $6,0x0(%1)\n\
    sq    $6,0x0(%0)\n\
    ": : "r" (v0) , "r" (v1):"$6");
}

#endif //  GRAPHICS_GRAPH3D_LIBSG_H
