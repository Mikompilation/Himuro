#ifndef GRAPHICS_MOTION_MIM_DAT_H
#define GRAPHICS_MOTION_MIM_DAT_H

#include "typedefs.h"

typedef struct {
	u_int *addr;
	u_char *dat;
} MIME_LOAD;

extern u_char mimchodo_r003[];
extern u_char mimchodo_r007[];
extern u_char mimchodo_r009[];
extern u_char mimchodo_r011[];
extern u_char mimchodo_r021[];
extern u_char mimchodo_r041[];
extern MIME_LOAD mimchodo_tbl[];

#endif // GRAPHICS_MOTION_MIM_DAT_H
