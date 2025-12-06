#ifndef MC_MC_H
#define MC_MC_H

#include "typedefs.h"

typedef struct {
	u_char *addr;
	int size;
} MC_DATA_STR;

extern MC_DATA_STR mc_gamedata_str[];
// extern MC_DATA_STR mc_albumdata_str[];
// extern MC_DATA_STR mc_albumdata2_str[];
extern u_long mc_gamedata_str_num;
// extern u_long mc_albumdata_str_num;

#endif // MC_MC_H
