#ifndef OS_EEIOP_ADPCM_EA_CTRL_H
#define OS_EEIOP_ADPCM_EA_CTRL_H

#include "typedefs.h"

typedef struct {
	sceVu0FVECTOR pos;
	sceVu0FVECTOR *mpos;
	int file_no;
	int count;
	u_short fin_flm;
	u_short fout_flm;
	u_short vol;
	u_short pitch;
	u_short pan;
	u_short start_flm;
	u_char mode;
	u_char ewrk_no;
} ADPCM_TUNE_PARAM;

typedef struct {
	ADPCM_TUNE_PARAM para;
	int count;
	u_char stop;
	u_char use;
	u_char mode;
} ADPCM_REQ_BOX;

typedef struct {
	ADPCM_TUNE_PARAM mpara;
	ADPCM_TUNE_PARAM bk_para;
	ADPCM_REQ_BOX btlmode;
	ADPCM_REQ_BOX gover;
	ADPCM_REQ_BOX scene;
	ADPCM_REQ_BOX maga;
	ADPCM_REQ_BOX shinkan;
	ADPCM_REQ_BOX puzzle;
	ADPCM_REQ_BOX soul;
	ADPCM_REQ_BOX tape;
	ADPCM_REQ_BOX gdead;
	ADPCM_REQ_BOX event;
	ADPCM_REQ_BOX ghost;
	ADPCM_REQ_BOX autog;
	ADPCM_REQ_BOX hiso;
	ADPCM_REQ_BOX furn;
	ADPCM_REQ_BOX map;
	int mvol;
	int tgt_vol;
	int deg;
	u_char m_flg;
	u_char movie;
	u_char outmode;
	u_char type;
	u_char mode;
} ADPCM_MAP;

extern ADPCM_MAP adpcm_map;

void AdpcmMapCtrlInit();
void AdpcmMapUse();
// void AdpcmMapNoUse();
void AdpcmMapCtrl();
void AdpcmShiftMovie();
void AdpcmReturnFromMovie();
void SetTargetVolAdpcmMenuFade(int tgt_vol);
void EAdpcmFadeOut(u_short fout_flm);
u_char IsEndAdpcmFadeOut();
void EAdpcmFadeOutGameEnd(u_short fout_flm);
u_char IsHighModeUse(u_char my_mode);

#endif // OS_EEIOP_ADPCM_EA_CTRL_H
