#ifndef GRAPHICS_GRAPH3D_SGCAM_H
#define GRAPHICS_GRAPH3D_SGCAM_H

#include "typedefs.h"

#include "ingame/camera/camera.h"

extern sceVu0FVECTOR clip_volume;
extern SgCAMERA *nowcamera;

void SetViewScreenClipMatrix(SgCAMERA *camera, float scrz);
void SgSetRefCamera(SgCAMERA *camera);
void SetViewScreenClipMatrixOrtho(SgCAMERA *camera, float scrz);
void SgSetRefCameraOrtho(SgCAMERA *camera);
void SetClipValue(float minx, float maxx, float miny, float maxy);
void printClipValue();
int BoundClip(sceVu0FVECTOR, sceVu0FVECTOR);
int BoundClipQ(sceVu0FVECTOR, sceVu0FVECTOR, sceVu0FVECTOR);
void printBoundingBox(u_int *prim);
int ClipCheck(sceVu0FVECTOR *vec);
int CheckBoundingBox(u_int *prim);
int CheckBoundingBoxEdge(u_int *prim);
void SgSetFog(float min, float max, float near, float far, int r, int g, int b);
void SgSetWScissorBox(float ax, float ay, float az, float bx, float by, float bz);
void _SetMulMatrixBB(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FMATRIX lw);

#endif // GRAPHICS_GRAPH3D_SGCAM_H
