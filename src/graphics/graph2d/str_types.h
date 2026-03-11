#ifndef STR_TYPES_H
#define STR_TYPES_H

#include "typedefs.h"

typedef struct {
	u_char *str;
	int pos_x;
	int pos_y;
	int type;
	u_int r;
	u_int g;
	u_int b;
	int alpha;
	int pri;
	int x_wide;
	int y_wide;
	int brnch_num;
	int csr;
	int st;
} DISP_STR;

typedef struct {
	u_char *str;
	int pos_x;
	int pos_y;
	int type;
	u_int r;
	u_int g;
	u_int b;
	int alpha;
	int pri;
} STR_DAT;

#endif // STR_TYPES_H
