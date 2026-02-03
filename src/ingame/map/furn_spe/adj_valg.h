#ifndef INGAME_MAP_FURN_SPE_ADJ_VALG_H
#define INGAME_MAP_FURN_SPE_ADJ_VALG_H

#include "typedefs.h"

typedef struct {
	void *next;
	void *pre;
	u_short time;
	u_short nowtime;
	sceVu0FVECTOR *ori;
	sceVu0FVECTOR accel;
	sceVu0FVECTOR des;
} ADJ_VALG_WRK;

void InitAdjValGWrk();
void InitAdjValGWrkOne(ADJ_VALG_WRK *avp);
ADJ_VALG_WRK* SearchVacantAdjValGWrk();
void ReleaseAdjValGWrkVec(float *ori);
void ReleaseAdjValG(ADJ_VALG_WRK *avp);
void SameObjectWrkDelete(float *ori);
void RegisterAdjValG(float *ori, float *des, u_int time);
void AdjustValueG();

#endif // INGAME_MAP_FURN_SPE_ADJ_VALG_H
