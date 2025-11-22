#ifndef OS_PAD_TYPES_H
#define OS_PAD_TYPES_H

#include "typedefs.h"

// #include "sce/libpad.h" // scePadDmaBufferMax ?

typedef struct {
	int port;
	int slot;
	u_long128 pad_dma_buf[16] __attribute__((aligned (64)));
	u_short now;
	u_short old;
	u_short cnt[16];
	u_short cnt_bak[16];
	u_short one;
	u_short rpt;
	u_short rpt_time;
	u_char pad_direct[6];
	char flags;
	char step;
	char id;
	u_char analog[4];
	u_char push[12];
	u_short an_cnt[2];
	u_short an_cnt_bak[2];
	float an_rot[2];
	float an_rot_bak[2];
	u_char an_dir[2];
	u_char an_dir_bak[2];
} PAD_STRUCT;

#endif // OS_PAD_TYPES_H

