#ifndef SCE_LIBVU0_H
#define SCE_LIBVU0_H

#include "typedefs.h"

void sceVu0ScaleVectorXYZ(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float s);
void sceVu0AddVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2);
void sceVu0SubVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2);
void sceVu0MulVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2);
void sceVu0CopyVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1);
void sceVu0Normalize(sceVu0FVECTOR v0, sceVu0FVECTOR v1);
float sceVu0InnerProduct(sceVu0FVECTOR v0, sceVu0FVECTOR v1);
void sceVu0OuterProduct(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2);
void sceVu0UnitMatrix(sceVu0FMATRIX m);
void sceVu0TransposeMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1);
void sceVu0MulMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FMATRIX m2);
void sceVu0InversMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1);
void sceVu0CopyMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1);
void sceVu0RotMatrixZ(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rz);
void sceVu0ApplyMatrix(sceVu0FVECTOR v0, sceVu0FMATRIX m, sceVu0FVECTOR v1);
void sceVu0RotMatrixX(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rx);
void sceVu0RotMatrixY(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float ry);
void sceVu0TransMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FVECTOR tv);
void sceVu0MulMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FMATRIX m2);
void sceVu0RotTransPers(sceVu0IVECTOR v0, sceVu0FMATRIX m0, sceVu0FVECTOR v1, int mode);
void sceVu0SubVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2);
void sceVu0ScaleVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float s);
void sceVu0RotTransPers(sceVu0IVECTOR v0, sceVu0FMATRIX m0, sceVu0FVECTOR v1, int mode);
void sceVu0RotTransPersN(sceVu0IVECTOR *v0, sceVu0FMATRIX m0, sceVu0FVECTOR *v1, int n, int mode);
void sceVu0DivVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float q);
void sceVu0ClampVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float min, float max);

#endif // SCE_LIBVU0_H
