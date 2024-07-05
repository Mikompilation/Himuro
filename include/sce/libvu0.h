#ifndef SCE_LIBVU0_H
#define SCE_LIBVU0_H

#include "typedefs.h"

void  sceVu0UnitMatrix(sceVu0FMATRIX m);
void  sceVu0RotMatrixZ(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rz);
void  sceVu0ApplyMatrix(sceVu0FVECTOR v0, sceVu0FMATRIX m, sceVu0FVECTOR v1);
void  sceVu0RotMatrixX(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rx);
void  sceVu0RotMatrixY(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float ry);

#endif // SCE_LIBVU0_H
