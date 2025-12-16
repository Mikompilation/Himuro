#ifndef INGAME_ENTRY_AP_PGOST_H
#define INGAME_ENTRY_AP_PGOST_H

#include "typedefs.h"

typedef struct {
	u_short score;
	u_char efct_no;
	u_char room;
	short pos[3];
} PGOST_DAT;

typedef struct {
	u_char req_no;
	u_char pgst_no;
	u_char ev_no;
	u_char stts;
	u_char time;
} PGOST_WRK;

extern PGOST_DAT pg_dat[];
// extern int test_rg_no;
extern PGOST_WRK pg_wrk;

void PuzzleGhostInit();
void PuzzleGhostLoadReq(u_char pg_no);
void PuzzleGhostReq(u_char pg_no);
int PuzzleGhostContinue(u_char ev_no);
void PuzzleGhostPhotoReq();
void PuzzleGhostMain();
void PuzzleGhostDispSet();
void PuzzleGhostDispMain();

#endif // INGAME_ENTRY_AP_PGOST_H
