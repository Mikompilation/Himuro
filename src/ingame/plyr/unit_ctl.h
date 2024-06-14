#ifndef INGAME_PLYR_UNIT_CTL_H
#define INGAME_PLYR_UNIT_CTL_H

#include "typedefs.h"

void RotLimitChk(float *rot);
float GetTrgtRotY(float *p0, float *p1);
void GetTrgtRotFromPlyr(float *p, int id);
void GetTrgtRot(float *p0, float *p1, int id);
int GetRndSP(u_int min, u_int lng);
u_char ConvertRot2Dir(float rot, u_char id);
u_char HitChkSegment2AllChk(float *f, float *t, float deg);
u_char OutSightChk(float *tp, float *vp, float rot, float sight, float dist);
int RotRngChk(float *vp, float *tp, float rot, float rng);
int GetPointHeightChk(float *tv);
void ReqEneStop(u_char req, u_char except);

#endif // INGAME_PLYR_UNIT_CTL_H
