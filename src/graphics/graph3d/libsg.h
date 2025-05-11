#ifndef GRAPHICS_GRAPH3D_LIBSG_H
#define GRAPHICS_GRAPH3D_LIBSG_H

#include "typedefs.h"

static inline void Vu0CopyVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
    __asm__ __volatile__("\n\
    lq    $6,0x0(%1)\n\
    sq    $6,0x0(%0)\n\
    ": : "r" (v0) , "r" (v1):"$6");
}

inline static void Vu0SubVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
{
    __asm__ __volatile__("\n\
        lqc2        vf12,0x0(%1)\n\
        lqc2        vf13,0x0(%2)\n\
        vsub.xyzw   vf14,vf12,vf13\n\
        sqc2        vf14,0x0(%0)\n\
        ": : "r" (v0) , "r" (v1), "r" (v2) : "memory");
}

static inline void Vu0LoadMatrix(sceVu0FMATRIX m0){
    __asm__ volatile(
        "lqc2      vf4,0x00(%0)\n"
        "lqc2      vf5,0x10(%0)\n"
        "lqc2      vf6,0x20(%0)\n"
        "lqc2      vf7,0x30(%0)\n"
        : : "r"(m0) : "memory");
}

static inline void Vu0LoadApplyMatrix(sceVu0FVECTOR v0, sceVu0FMATRIX m0, sceVu0FVECTOR v1)
{
    asm __volatile__(
        "lqc2          vf4,0x0(%1)\n"
        "lqc2          vf5,0x10(%1)\n"
        "lqc2          vf6,0x20(%1)\n"
        "lqc2          vf7,0x30(%1)\n"
        "lqc2          vf8,0x0(%2)\n"
        "vmulax.xyzw   ACC,vf4,vf8\n"
        "vmadday.xyzw  ACC,vf5,vf8\n"
        "vmaddaz.xyzw  ACC,vf6,vf8\n"
        "vmaddw.xyzw   vf12,vf7,vf8\n"
        "sqc2          vf12,0x0(%0)\n"
        : :"r"(v0),"r"(m0),"r"(v1):"memory"
    );
}

static inline void Vu0ApplyVectorInline(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
    asm __volatile__(
        "lqc2          vf13,0x0(%1)\n"
        "vmulax.xyzw   ACC,vf4,vf13x\n"
        "vmadday.xyzw  ACC,vf5,vf13y\n"
        "vmaddaz.xyzw  ACC,vf6,vf13z\n"
        "vmaddw.xyzw   vf12,vf7,vf0w\n"
        "sqc2          vf12,0x0(%0)\n"
        : :"r"(v0), "r"(v1):"memory"
    );
}

inline static void Vu0ZeroVector(sceVu0FVECTOR v0)
{
    __asm__ __volatile__(
        "sqc2    vf0,0x0(%0)\n"
        : : "r" (v0) : "memory");
}

static inline void Vu0AddVector(sceVu0FVECTOR v, sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
    __asm__ volatile("\n\
        lqc2        $vf12,0(%1)\n\
        lqc2        $vf13,0(%2)\n\
        vadd.xyzw   $vf14xyzw,$vf12xyzw,$vf13xyzw\n\
        sqc2        $vf14,0(%0)\n\
        ": : "r"(v), "r"(v0), "r"(v1));
}

static inline float vu0Rand()
{
    float r;
    
    __asm__ volatile(
        "vrnext.x $vf12x,R\n"
        "vsubw.x $vf12x,$vf12x,$vf0w\n"
        "qmfc2   %0,$vf12\n"
        :"=r"(r)
    );

    return r;
}

#endif //  GRAPHICS_GRAPH3D_LIBSG_H
