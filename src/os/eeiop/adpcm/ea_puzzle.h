#ifndef OS_EEIOP_ADPCM_EA_PUZZLE_H
#define OS_EEIOP_ADPCM_EA_PUZZLE_H

#include "typedefs.h"

void EAdpcmPuzzleMain();
void AdpcmPrePuzzleFadeOut(u_short fout_flm);
u_char IsEndAdpcmPuzzle();
void AdpcmPreLoadPuzzle(int file_no);
u_char IsPreLoadEndAdpcmPuzzle();
void AdpcmPuzzlePreLoadEndPlay();
void AdpcmPuzzlePlay(int file_no);
void AdpcmPuzzleEnd();

#endif // OS_EEIOP_ADPCM_EA_PUZZLE_H
