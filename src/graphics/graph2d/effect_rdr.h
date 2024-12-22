#ifndef GRAPHICS_GRAPH2D_EFFECT_RDR_H
#define GRAPHICS_GRAPH2D_EFFECT_RDR_H

#include "typedefs.h"

#include "ingame/map/furn_dat.h"

typedef struct {
	void *adr;
	short int furn_id;
	u_short dummy;
} EFFRDR_RSV;

typedef struct {
	sceVu0FVECTOR ligdiff;
	sceVu0FVECTOR ligpos;
	sceVu0FVECTOR fpos;
	void *ebuf;
	float ligpow;
	float wavew;
	float rate;
	float szw;
	float szh;
	float sw;
	float sh;
	u_short furn_id;
	u_char lignum;
	u_char usefl;
	u_char sta;
} BURN_FIRE;

#include "graphics/graph2d/effect.h"

// extern BURN_FIRE burn_fire[10];
// extern EFFRDR_RSV blood_drop_rsv[16];
// extern EFFRDR_RSV pfire_rsv[4];

void InitEffectRdr();
void InitEffectRdrEF();
void* GetBurnFireWork();
void* SearchBurnFireFurnID(u_short furn_id);
void SetRDLongFire2(float *pos, u_char sta, float szw, float szh, float sw, float sh, float r, float g, float b, float room, u_short furn_id);
void SetRDLongFire(float *pos, float r, float g, float b, float room, u_short furn_id);
void ResetRDLongFire(u_short furn_id);
void SubRDFire(EFFECT_CONT *ec);
void SetRDFire3(FURN_ACT_WRK *f1, FURN_ACT_WRK *f2, FURN_ACT_WRK *f3, FURN_ACT_WRK *f4, FURN_ACT_WRK *f5, FURN_ACT_WRK *f6);
void ResetRDFire3();
void SubRDFire3();
void SetRDPazzEne(float *pos);
void ResetRDPazzEne();
void SetRoomDirecPazzEne();
void SetRDSmoke();
void ResetRDSmoke();
void SetRoomDirecSmoke();
void SetRDSunshine(int n);
void ResetRDSunshine(int n);
short int GetRDBloodDropWork();
short int SearchRDBloodDropWork(u_short furn_id);
void SetRDBloodDrop(float *pos, int sta, u_short furn_id);
void ResetRDBloodDrop(u_short furn_id);
short int GetRDPFireWork();
short int SearchRDPFireWork(u_short furn_id);
void SetRDPFire(float *pos, u_short furn_id);
void SetRDPFire2(float *pos, u_short furn_id);
void ResetRDPFire(u_short furn_id);

#endif // GRAPHICS_GRAPH2D_EFFECT_RDR_H
