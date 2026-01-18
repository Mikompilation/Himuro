#ifndef INGAME_MAP_MAP_HTCK_H
#define INGAME_MAP_MAP_HTCK_H

#include "typedefs.h"

u_char PosInAreaJudgeSub(int *addr, u_short pos_x, u_short pos_y, u_char type);
void Get2DLinearConstFromPos(float *a, float *b, float *c, float x0, float y0, float x1, float y1);
int Get2DLinearCrossPoint(float *cpx, float *cpy, float a0, float b0, float c0, float a1, float b1, float c1);
int ChkLineCross(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);
int ChkInsideBox(float cpx, float cpy, float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);
u_char HitChkSegment2All(sceVu0FVECTOR f, sceVu0FVECTOR t, float deg);
u_char HitChkSegment2All2D(sceVu0FVECTOR f, sceVu0FVECTOR t, float deg);
u_char FurnCoverCheck(u_short pos_x, short int pos_y, u_short pos_z, u_char room_no);

#endif // INGAME_MAP_MAP_HTCK_H
