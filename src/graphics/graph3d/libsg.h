#ifndef GRAPHICS_GRAPH3D_LIBSG_H
#define GRAPHICS_GRAPH3D_LIBSG_H

#include "typedefs.h"

#include "graphics/graph3d/sglib.h"

// line 26
static inline void load_matrix_0(sceVu0FMATRIX m0)
{
    asm volatile ("\n\
        lqc2 $vf4,0(%0) \n\
        lqc2 $vf5,0x10(%0) \n\
        lqc2 $vf6,0x20(%0) \n\
        lqc2 $vf7,0x30(%0) \n\
    ": :"r"(m0));
}

// line 36
static inline void load_matrix_1(sceVu0FMATRIX m0)
{
    asm volatile ("\n\
        lqc2 $vf8,0(%0) \n\
        lqc2 $vf9,0x10(%0) \n\
        lqc2 $vf10,0x20(%0) \n\
        lqc2 $vf11,0x30(%0) \n\
    ": :"r"(m0));
}

// Line 73
static inline void copy_skinned_data(sceVu0FVECTOR *vb, float *s0, float *s1)
{
    __asm__ __volatile__("\n\
        lq $6,0x0(%1)\n\
        lq $7,0x0(%2)\n\
        sq $6,0x0(%0)\n\
        sq $7,0x10(%0)\n\
        ": :"r" (vb), "r" (s0), "r" (s1) : "$6", "$7");
}

// Line 151
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

// Line 233
static inline void calc_skinned_data(sceVu0FVECTOR *dp, float *m, float *n)
{
    __asm__ __volatile__("\n\
        lqc2         $vf12,0(%1)\n\
        vmulax.xyzw  ACCxyzw,$vf4xyzw,$vf12x\n\
        vmadday.xyzw ACCxyzw,$vf5xyzw,$vf12y\n\
        vmaddaz.xyzw ACCxyzw,$vf6xyzw,$vf12z\n\
        vmaddw.xyz   $vf13xyz,$vf7xyz,$vf0w\n\
        vsubw.w      $vf14w,$vf0w,$vf12w\n\
        vmove.w      $vf13w,$vf12w\n\
        lqc2         $vf12,0x10(%1)\n\
        vmulax.xyzw  ACCxyzw,$vf8xyzw,$vf12x\n\
        vmadday.xyzw ACCxyzw,$vf9xyzw,$vf12y\n\
        vmaddaz.xyzw ACCxyzw,$vf10xyzw,$vf12z\n\
        vmaddw.xyz   $vf14xyz,$vf11xyz,$vf0w\n\
        vmulaw.xyz   ACCxyz,$vf13xyz,$vf13w\n\
        vmaddw.xyz   $vf15xyz,$vf14xyz,$vf14w\n\
        vmove.w      $vf15w,$vf0w\n\
        lqc2         $vf12,0(%2)\n\
        vmulax.xyz   ACCxyz,$vf4xyz,$vf12x\n\
        vmadday.xyz  ACCxyz,$vf5xyz,$vf12y\n\
        vmaddz.xyz   $vf13xyz,$vf6xyz,$vf12z\n\
        vsubw.w      $vf14w,$vf0w,$vf12w\n\
        lqc2         $vf12,0x10(%2)\n\
        vmulax.xyz   ACCxyz,$vf8xyz,$vf12x\n\
        vmadday.xyz  ACCxyz,$vf9xyz,$vf12y\n\
        vmaddz.xyz   $vf14xyz,$vf10xyz,$vf12z\n\
        vmulaw.xyz   ACCxyz,$vf13xyz,$vf13w\n\
        vmaddw.xyz   $vf12xyz,$vf14xyz,$vf14w\n\
        vmulw.xyz    $vf12xyz,$vf12xyz,$vf7w\n\
        vmove.w      $vf12w,$vf0w\n\
        sqc2         $vf15,0(%0)\n\
        sqc2         $vf12,0x10(%0)\n\
    ": :"r"(dp), "r"(m), "r"(n));
}

// line 285
static inline void calc_skinned_position(sceVu0FVECTOR dp, sceVu0FVECTOR v)
{
    /*
    vec0 = {0, 0, 0, 1};
    vec12 = arg1[0];

    ACC.x = vec4.x * vec12.x;
    ACC.y = vec4.y * vec12.x;
    ACC.z = vec4.z * vec12.x;
    ACC.w = vec4.w * vec12.x;

    ACC.x += vec5.x * vec12.y;
    ACC.y += vec5.y * vec12.y;
    ACC.z += vec5.z * vec12.y;
    ACC.w += vec5.w * vec12.y;

    ACC.x += vec6.x * vec12.z;
    ACC.y += vec6.y * vec12.z;
    ACC.z += vec6.z * vec12.z;
    ACC.w += vec6.w * vec12.z;

    vec13.x = ACC.x + (vec7.x * vec0.w);
    vec13.y = ACC.y + (vec7.y * vec0.w);
    vec13.z = ACC.z + (vec7.z * vec0.w);

    vec14 = arg1[1];

    vec15.w = vec0.w;

    vec13.w = vec0.w - vec12.w;

    ACC.x = vec8.x * vec14.x;
    ACC.y = vec8.y * vec14.x;
    ACC.z = vec8.z * vec14.x;
    ACC.w = vec8.w * vec14.x;

    ACC.x += vec9.x * vec14.y;
    ACC.y += vec9.y * vec14.y;
    ACC.z += vec9.z * vec14.y;
    ACC.w += vec9.w * vec14.y;

    ACC.x += vec10.x * vec14.z;
    ACC.y += vec10.y * vec14.z;
    ACC.z += vec10.z * vec14.z;
    ACC.w += vec10.w * vec14.z;

    vec14.x = ACC.x + (vec11.x * vec0.w);
    vec14.y = ACC.y + (vec11.y * vec0.w);
    vec14.z = ACC.z + (vec11.z * vec0.w);

    ACC.x = vec13.x * vec12.w;
    ACC.y = vec13.y * vec12.w;
    ACC.z = vec13.z * vec12.w;
    ACC.w = vec13.w * vec12.w;

    vec15.x = ACC.x + (vec14.x * vec13.w);
    vec15.y = ACC.y + (vec14.y * vec13.w);
    vec15.z = ACC.z + (vec14.z * vec13.w);

    arg0[0] = vec15;

    // which simplifies to:
    sceVu0FMATRIX m0 = lcp[vli->lists[i].cn0].workm;
    sceVu0FMATRIX m1 = lcp[vli->lists[i].cn1].workm;

    sceVu0FVECTOR *vps = lphead->pWeightedVertex;
    sceVu0FVECTOR *vpd = vertex_buffer;

    vec13.x = (m0[0].x * vps[0].x) + (m0[1].x * vps[0].y) + (m0[2].x * vps[0].z) + m0[3].x;
    vec13.y = (m0[0].y * vps[0].x) + (m0[1].y * vps[0].y) + (m0[2].y * vps[0].z) + m0[3].y;
    vec13.z = (m0[0].z * vps[0].x) + (m0[1].z * vps[0].y) + (m0[2].z * vps[0].z) + m0[3].z;
    vec13.w = 1.0f - vps[0].w;

    vec14.x = (m1[0].x * vps[1].x) + (m1[1].x * vps[1].y) + (m1[2].x * vps[1].z) + m1[3].x;
    vec14.y = (m1[0].y * vps[1].x) + (m1[1].y * vps[1].y) + (m1[2].y * vps[1].z) + m1[3].y;
    vec14.z = (m1[0].z * vps[1].x) + (m1[1].z * vps[1].y) + (m1[2].z * vps[1].z) + m1[3].z;

    vpd->x = (vec13.x * vps[0].w) + (vec14.x * vec13.w);
    vpd->y = (vec13.y * vps[0].w) + (vec14.y * vec13.w);
    vpd->z = (vec13.z * vps[0].w) + (vec14.z * vec13.w);
    vpd->w = 1.0f;

    */
    asm volatile ("\n\
        lqc2         $vf12, 0(%1)\n\
        vmulax       ACC,   $vf4,  $vf12\n\
        vmadday      ACC,   $vf5,  $vf12\n\
        vmaddaz      ACC,   $vf6,  $vf12\n\
        vmaddw.xyz   $vf13, $vf7,  $vf0\n\
        lqc2         $vf14, 0x10(%1)\n\
        vmove.w      $vf15, $vf0\n\
        vsubw.w      $vf13, $vf0,  $vf12\n\
        vmulax       ACC,   $vf8,  $vf14\n\
        vmadday      ACC,   $vf9,  $vf14\n\
        vmaddaz      ACC,   $vf10, $vf14\n\
        vmaddw.xyz   $vf14, $vf11, $vf0\n\
        vmulaw.xyz   ACC,   $vf13, $vf12\n\
        vmaddw.xyz   $vf15, $vf14, $vf13\n\
        sqc2         $vf15, 0(%0)\n\
    ": :"r"(dp), "r"(v));
}

// Line 314
static inline void calc_skinned_normal(sceVu0FVECTOR dp, sceVu0FVECTOR v)
{
    /*
    vec0 = {0, 0, 0, 1};
    vec12 = arg1[0];

    ACC.x = vec4.x * vec12.x;
    ACC.y = vec4.y * vec12.x;
    ACC.z = vec4.z * vec12.x;

    ACC.x += vec5.x * vec12.y;
    ACC.y += vec5.y * vec12.y;
    ACC.z += vec5.z * vec12.y;

    vec13.x = ACC.x + (vec6.x * vec12.z);
    vec13.y = ACC.y + (vec6.y * vec12.z);
    vec13.z = ACC.z + (vec6.z * vec12.z);
    vec13.w = vec0.w - vec12.w;

    vec14 = arg1[1];

    vec15.w = vec0.w;

    vec12.w *= vec7.w;
    vec13.w *= vec7.w;

    ACC.x = vec8.x * vec14.x;
    ACC.y = vec8.y * vec14.x;
    ACC.z = vec8.z * vec14.x;

    ACC.x += vec9.x * vec14.y;
    ACC.y += vec9.y * vec14.y;
    ACC.z += vec9.z * vec14.y;

    vec14.x = ACC.x + (vec10.x * vec14.z);
    vec14.y = ACC.y + (vec10.y * vec14.z);
    vec14.z = ACC.z + (vec10.z * vec14.z);

    ACC.x = vec13.x * vec12.w;
    ACC.y = vec13.y * vec12.w;
    ACC.z = vec13.z * vec12.w;

    vec15.x = ACC.x + (vec14.x * vec13.w);
    vec15.y = ACC.y + (vec14.y * vec13.w);
    vec15.z = ACC.z + (vec14.z * vec13.w);

    arg0[0] = vec15;

    // which simplifies to:
    sceVu0FMATRIX m0 = lcp[vli->lists[i].cn0].workm;
    sceVu0FMATRIX m1 = lcp[vli->lists[i].cn1].workm;

    sceVu0FVECTOR *vps = lphead->pWeightedNormal;
    sceVu0FVECTOR *vpd = normal_buffer;

    float vec12_w;

    vec13.x = (m0[0].x * vps[0].x) + (m0[1].x * vps[0].y) + (m0[2].x * vps[0].z);
    vec13.y = (m0[0].y * vps[0].x) + (m0[1].y * vps[0].y) + (m0[2].y * vps[0].z);
    vec13.z = (m0[0].z * vps[0].x) + (m0[1].z * vps[0].y) + (m0[2].z * vps[0].z);
    vec13.w = 1.0f - vps[0].w;

    vec12_w = vps[0].w * m0[3].w;
    vec13.w *= m0[3].w;

    vec14.x = (m1[0].x * vps[1].x) + (m1[1].x * vps[1].y) + (m1[2].x * vps[1].z);
    vec14.y = (m1[0].y * vps[1].x) + (m1[1].y * vps[1].y) + (m1[2].y * vps[1].z);
    vec14.z = (m1[0].z * vps[1].x) + (m1[1].z * vps[1].y) + (m1[2].z * vps[1].z);

    vpd->x = (vec13.x * vec12_w) + (vec14.x * vec13.w);
    vpd->y = (vec13.y * vec12_w) + (vec14.y * vec13.w);
    vpd->z = (vec13.z * vec12_w) + (vec14.z * vec13.w);
    vpd->w = 1.0f;
    */

    asm volatile ("\n\
        lqc2         $vf12, 0(%1)\n\
        vmulax.xyz   ACC,   $vf4,  $vf12\n\
        vmadday.xyz  ACC,   $vf5,  $vf12\n\
        vmaddz.xyz   $vf13, $vf6,  $vf12\n\
        vsubw.w      $vf13, $vf0,  $vf12\n\
        lqc2         $vf14, 0x10(%1)\n\
        vmove.w      $vf15, $vf0\n\
        vmulw.w      $vf12, $vf12, $vf7\n\
        vmulw.w      $vf13, $vf13, $vf7\n\
        vmulax.xyz   ACC,   $vf8,  $vf14\n\
        vmadday.xyz  ACC,   $vf9,  $vf14\n\
        vmaddz.xyz   $vf14, $vf10, $vf14\n\
        vmulaw.xyz   ACC,   $vf13, $vf12\n\
        vmaddw.xyz   $vf15, $vf14, $vf13\n\
        sqc2         $vf15, 0(%0)\n\
    ": :"r"(dp), "r"(v));
}

// Line 351
static inline void Vu0CopyVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
    __asm__ __volatile__("\n\
    lq    $6,0x0(%1)\n\
    sq    $6,0x0(%0)\n\
    ": : "r" (v0) , "r" (v1):"$6");
}

// Line 367
inline static void Vu0ZeroVector(sceVu0FVECTOR v0)
{
    __asm__ __volatile__(
        "sqc2    vf0,0x0(%0)\n"
        : : "r" (v0) : "memory");
}

// Line 374
static inline void Vu0AddVector(sceVu0FVECTOR v, sceVu0FVECTOR v0, sceVu0FVECTOR v1)
{
    __asm__ volatile("\n\
        lqc2        $vf12,0(%1)\n\
        lqc2        $vf13,0(%2)\n\
        vadd.xyzw   $vf14xyzw,$vf12xyzw,$vf13xyzw\n\
        sqc2        $vf14,0(%0)\n\
        ": : "r"(v), "r"(v0), "r"(v1));
}

// // Line 383
inline static void Vu0SubVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2)
{
    __asm__ __volatile__("\n\
        lqc2        vf12,0x0(%1)\n\
        lqc2        vf13,0x0(%2)\n\
        vsub.xyzw   vf14,vf12,vf13\n\
        sqc2        vf14,0x0(%0)\n\
        ": : "r" (v0) , "r" (v1), "r" (v2) : "memory");
}

// Line 412
static inline void Vu0CopyMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1)
{
  __asm__ volatile (
        "lqc2 vf12, 0x00(%1)\n"
        "lqc2 vf13, 0x10(%1)\n"
        "lqc2 vf14, 0x20(%1)\n"
        "lqc2 vf15, 0x30(%1)\n"
        "sqc2 vf12, 0x00(%0)\n"
        "sqc2 vf13, 0x10(%0)\n"
        "sqc2 vf14, 0x20(%0)\n"
        "sqc2 vf15, 0x30(%0)\n"
        : : "r" (&m0[0][0]), "r" (&m1[0][0]));
}

// Line 438
static inline void Vu0LoadMatrix(sceVu0FMATRIX m0){
    __asm__ volatile(
        "lqc2      vf4,0x00(%0)\n"
        "lqc2      vf5,0x10(%0)\n"
        "lqc2      vf6,0x20(%0)\n"
        "lqc2      vf7,0x30(%0)\n"
        : : "r"(m0) : "memory");
}

// Line 448
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

// Line 494
static inline qword* getObjWrk()
{
    return (qword *)&objwork[sbuffer_p];
}

#endif //  GRAPHICS_GRAPH3D_LIBSG_H
