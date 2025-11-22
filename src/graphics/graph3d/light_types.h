#ifndef GRAPHICS_GRAPH3D_LIGHT_TYPES_H
#define GRAPHICS_GRAPH3D_LIGHT_TYPES_H

#include "common.h"
#include "typedefs.h"

typedef struct {
	u_char state;
	u_char room_no;
	float *power;
	sceVu0FVECTOR *pos;
	sceVu0FVECTOR *diffuse;
} POINT_LIGHT_WRK;

typedef struct {
    u_short time;
    float magnif;
} FSPE_LIGHT_ANM;

typedef enum {
    ST_FSLA_VACANT = 0,
    ST_FSLA_IN = 1,
    ST_FSLA_EXEC = 2,
    ST_FSLA_OUT = 3
} ST_FSLA;

typedef struct {
	ST_FSLA state;
	u_char room_id;
	FSPE_LIGHT_ANM *ap_in;
	u_int time;
	float now_magnif;
	sceVu0FVECTOR diffuse;
	u_char type;
	u_char fadeouttype;
	signed char plw_id;
} FSPE_LIGHT_WRK;

typedef struct {
	sceVu0FVECTOR pos;
	sceVu0FVECTOR direction;
	sceVu0FVECTOR diffuse;
	float intens;
	float power;
	float pad[2];
} SPOT_WRK;

typedef struct {
	sceVu0FVECTOR direction;
	sceVu0FVECTOR diffuse;
} PARARELL_WRK;

typedef struct {
	sceVu0FVECTOR pos;
	sceVu0FVECTOR diffuse;
	float power;
	float pad[3];
} POINT_WRK;

typedef struct {
	int parallel_num;
	int point_num;
	int spot_num;
	int pad;
	sceVu0FVECTOR ambient;
	PARARELL_WRK parallel[3];
	POINT_WRK point[3];
	SPOT_WRK spot[3];
} LIGHT_PACK;

#endif // GRAPHICS_GRAPH3D_LIGHT_TYPES_H
