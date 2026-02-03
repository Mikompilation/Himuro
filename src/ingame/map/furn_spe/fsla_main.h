#ifndef INGAME_MAP_FURN_SPE_FSLA_MAIN_H
#define INGAME_MAP_FURN_SPE_FSLA_MAIN_H

#include "typedefs.h"
#include "graphics/graph3d/light_types.h"

// extern u_char able_to_light_tbl[];
extern POINT_LIGHT_WRK point_light_wrk[8];
extern FSPE_LIGHT_WRK fspe_light_wrk[7];

FSPE_LIGHT_ANM* SetRandLightAnimEach(FSPE_LIGHT_ANM **ap);
void SetRandLightAnim(FSPE_LIGHT_WRK *lw);
signed char GetOpenPointLightWrkID();
signed char GetVacantPLW();
void DeletePointLight(signed char num);
signed char AddNewPointLight(sceVu0FVECTOR *pos, sceVu0FVECTOR *diffuse, float *power, u_char room_id);
float SetPLW2PW(POINT_WRK *pw, float obj_dist, float compare);
int SetPointLightPack(float *obj_pos, POINT_WRK *ppw, u_char start_num);
void FinishCandle();
void CandleAnmStop(signed char lw_id);
signed char CandleAnmOcc(sceVu0FVECTOR *pos, float r, float g, float b, u_char in_pat, u_char pat, u_char out_pat, u_char room_id);
void PointLightCtrl();
void CandleAnmCtrl();

#endif // INGAME_MAP_FURN_SPE_FSLA_MAIN_H
