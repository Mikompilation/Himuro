#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ul_math.h"

#include "sce/libvu0.h"

void GetSquare(sceVu0FVECTOR *v0, sceVu0FVECTOR *v1) 
{
    asm(
        "lqc2      $vf10,%1\n"
        "lqc2      $vf11,%2\n"
        "vmul.xyz  $vf10, $vf10, $vf11\n"
        "sqc2      $vf10,%0\n"
        :"=m"(v1[0][0])
        :"m"(v0[0][0]), "m"(v0[0][0])
    );
}

float GetDist(float x, float z)
{
    sceVu0FVECTOR v0;
    sceVu0FVECTOR v1;
    
    v0[0] = __builtin_fabsf(x);
    v0[2] = __builtin_fabsf(z);
    
    v0[1] = v0[3] = 0.0f;
    
    GetSquare(&v0, &v1);
    
    return __builtin_sqrtf(v1[0] + v1[2]);
}

float GetDistV(float *p0, float *p1)
{
    sceVu0FVECTOR v0;
    sceVu0FVECTOR v1;
    
    v0[0] = __builtin_fabsf(p0[0] - p1[0]);
    v0[2] = __builtin_fabsf(p0[2] - p1[2]);
    
    v0[1] = v0[3] = 0.0f;
    
    GetSquare(&v0,&v1);
    
    return __builtin_sqrtf(v1[0] + v1[2]);
}

float GetDistV2(float *p0, float *p1)
{
    sceVu0FVECTOR v0;
    sceVu0FVECTOR v1;
    
    v0[0] = __builtin_fabsf(p0[0] - p1[0]);
    v0[2] = __builtin_fabsf(p0[2] - p1[2]);
    
    GetSquare(&v0, &v1);
    
    v0[0] = __builtin_sqrtf(v1[0] + v1[2]);
    v0[2] = __builtin_fabsf(p0[1] - p1[1]);
    
    GetSquare(&v0, &v1);
    
    return __builtin_sqrtf(v1[0] + v1[2]);
}

void RotFvector(float *rot, float *tv)
{
    sceVu0FMATRIX work;
    sceVu0FMATRIX rot_matrix;
    
    sceVu0UnitMatrix(work);
    if (rot[2] != 0.0f)
    {
        sceVu0RotMatrixZ(rot_matrix, work, rot[2]);
        sceVu0ApplyMatrix(tv, rot_matrix, tv);
    }
    
    if (rot[0] != 0.0f)
    {
        sceVu0RotMatrixX(rot_matrix, work,rot[0]);
        sceVu0ApplyMatrix(tv, rot_matrix, tv);
    }
    
    if (rot[1] != 0.0f)
    {
        sceVu0RotMatrixY(rot_matrix, work, rot[1]);
        sceVu0ApplyMatrix(tv, rot_matrix, tv);
    }
}
