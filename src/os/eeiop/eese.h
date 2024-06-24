#ifndef OS_EEIOP_EESE_H
#define OS_EEIOP_EESE_H

#include "typedefs.h"

typedef struct {
	u_int btlhit_no;
	u_int voice_no;
	u_int door_no[3];
	u_int foot_no[8];
	u_int srund_no[2];
	u_int ghost_no[3];
	u_int event_no[2];
	u_int wide_no;
	u_int jidou_no[4];
	u_char ghost_type[3];
} SE_CTRL;

typedef struct {
	int mvol;
	int tgt_vol;
	int deg;
} SE_MENU_FADE;

struct tagSE_WRK {
	void *mpos;
	sceVu0FVECTOR spos;
	int status;
	u_int atr;
	u_short se_p;
	u_short v_p;
	u_short pitch;
	u_short now_pitch;
	u_short pan;
	u_short vol_rate;
	u_char vol_phase;
	u_char ene_wrk_no;
	u_short fade_vol;
	u_short fade_spd;
	u_short fade_tgt;
	u_char fade_mode;
	u_char room_id;
	u_char menu;
};

typedef struct tagSE_WRK SE_WRK;

extern SE_CTRL se_ctrl;
extern SE_MENU_FADE sm_fade;

void InitSe();
void InitSeGhost();
void InitSeMenuFade();
void UpdateSeMenuFade();
void SetTargetVolSeMenuFade(int tgt_vol);
void SeInitSeWrk();
void SetReverbVolume(u_short vol);
void SeStopAndBackup();
void SeBackupInit();
void SeRevival(u_short fin_spd);
int SeStartFix(int se_no, u_short fin_spd, u_short vol_max, u_short pitch, u_char menu);
int SeStartFixFlame(int se_no, u_short flame, u_short vol_max, u_short pitch, u_char menu);
int SeStartFixV(int se_no, u_short fin_spd, u_short vol_max, u_short pitch, u_char voice_no);
int SeStartPos(int se_no, float *pos, u_short fin_spd, u_short vol_max, u_short pitch, u_char room_id);
int SeStartPosFlame(int se_no, float *pos, u_short flame, u_short vol_max, u_short pitch, u_char room_id);
int SeStartPut(int se_no,float *pos,u_short fin_spd,u_short vol_max,u_short pitch,u_char room_id);
int SeStartPutFlame(int se_no,float *pos,u_short flame,u_short vol_max,u_short pitch,u_char room_id);
int SeStartGhost(int se_no, int ewrk_no, u_short fin_spd, u_short vol_max, u_short pitch, u_char gno);
int SeStartGhostFlame(int se_no, int ewrk_no, u_short flame, u_short vol_max, u_short pitch, u_char gno);
int SeStartPlyr(int se_no, u_short fin_spd, u_short vol_max, u_short pitch);
int SeStartPlyrFlame(int se_no, u_short flame, u_short vol_max, u_short pitch);
int SeStartPosSrundFlame(u_char room_id, float *pos, u_short flame, u_short vol_max, u_short pitch);
int SeStartPosEventFlame(u_char room_id, u_char req_pos, float *pos, u_short flame, u_short vol_max, u_short pitch);
int SeStopAll();
int SeStop(int voice_num);
void SeFadeOut(int voice_num, int fout_spd, int tgt_vol);
void SeFadeFlame(int voice_num, u_short flame, u_short tgt_vol);
void SeFadeFlameAll(u_short flame, u_short tgt_vol);
void SeWrkUpdate();
u_char IsEndSe(int v_no);
u_char IsEndSeGhost(int v_no, u_char se_no, u_char gno);
void SeSetEfctVol(u_short vol);
void SeSetMVol(u_short vol);
void SeSetSteMono(u_char mono);
int SeFileLoadAndSet(u_int file_no, int type);
int SeFileLoadAndSetFGhost(u_int file_no, int type);
int SeFileLoadAndSetWide(u_short lbl_no);
char SeGetGhostPos(u_int se_file_no, u_char type);
char SeGetAutoGhostPos(u_int se_file_no);
SE_WRK* SeGetSeWrk(int no);
int SeGetSvStat(int v_no);
int SeStartCmn(int se_no, float *pos, float *mb, int fin_spd, u_short vol_max);
void SeStartVpCmn(int se_no, int vpos, float *pos, float *mb, int fin_spd, u_short vol_max);
u_char SeGetFreeSv();
int SeStartV(int se_no, u_char voice_no);

#endif // OS_EEIOP_EESE_H
