#ifndef INGAME_PLYR_UNIT_CTL_H
#define INGAME_PLYR_UNIT_CTL_H

#include "typedefs.h"

void RotLimitChk(sceVu0FVECTOR rot);
float GetTrgtRotY(sceVu0FVECTOR p0, sceVu0FVECTOR p1);
void GetTrgtRotFromPlyr(sceVu0FVECTOR p, int id);
void GetTrgtRot(sceVu0FVECTOR p0, sceVu0FVECTOR p1, sceVu0FVECTOR rot, int id);
int GetRndSP(u_int min, u_int lng);
u_char ConvertRot2Dir(float rot, u_char id);
u_char HitChkSegment2AllChk(sceVu0FVECTOR f, sceVu0FVECTOR t, float deg);
u_char OutSightChk(sceVu0FVECTOR tp, sceVu0FVECTOR vp, float rot, float sight, float dist);
int RotRngChk(sceVu0FVECTOR vp, sceVu0FVECTOR tp, float rot, float rng);
int GetPointHeightChk(sceVu0FVECTOR tv);
void ReqEneStop(u_char req, u_char except);

#endif // INGAME_PLYR_UNIT_CTL_H
