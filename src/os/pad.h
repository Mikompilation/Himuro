#ifndef OS_PAD_H
#define OS_PAD_H

#include "typedefs.h"

// #include "main/glob.h"

#include "sce/libpad.h"

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

extern u_short sce_pad[16];

int InitPad();
int PadSyncCallback();
int PadReadFunc(PAD_STRUCT *psp, int p_id);
void SetAnlgInfo(PAD_STRUCT *psp, int p_id);
// u_short VibrateRequest(u_short p_id, u_short act1, u_short act2);
// u_short VibrateRequest1(u_short p_id, u_short act_1);
// u_short VibrateRequest2(u_short p_id, u_short act_2);
void PadClearCount(int p_id);

#ifdef MATCHING_DECOMP
#ifdef INCLUDING_FROM_PAD_C
// fix for undeclared VibrateRequest in header
u_short VibrateRequest(u_short p_id, u_short act1, u_short act2);
#endif
#else
u_short VibrateRequest(u_short p_id, u_short act1, u_short act2);
#endif

#endif // OS_PAD_H
