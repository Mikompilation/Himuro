#ifndef OUTGAME_MEMORY_ALBUM_H
#define OUTGAME_MEMORY_ALBUM_H

#include "typedefs.h"

#include "ingame/menu/ig_rank.h"

typedef struct
{ // 0x44
    /* 0x00 */ u_int src_addr;
    /* 0x04 */ u_int cpy_addr;
    /* 0x08 */ u_short key_nashi_nori_take;
    /* 0x0a */ u_char is_title;
    /* 0x0b */ u_char side;
    /* 0x0c */ u_char side_chng;
    /* 0x0d */ u_char mode;
    /* 0x0e */ u_char mode_chng;
    /* 0x0f */ u_char pht_max[2];
    /* 0x11 */ u_char cl_type[2];
    /* 0x13 */ u_char slot[2];
    /* 0x15 */ u_char file[2];
    /* 0x17 */ u_char csr[5];
    /* 0x1c */ u_char csr_tim;
    /* 0x1d */ u_char buf[9];
    /* 0x26 */ u_char buf_sid[9];
    /* 0x2f */ u_char buf_set;
    /* 0x30 */ u_char mem;
    /* 0x31 */ u_char csr_mode;
    /* 0x32 */ u_char exit_realy;
    /* 0x33 */ u_char pst_mode;
    /* 0x34 */ u_char menu_alp;
    /* 0x35 */ u_char yes_no;
    /* 0x36 */ u_char pst_yes_no;
    /* 0x37 */ u_char rtrn_bak;
    /* 0x38 */ u_char io_mode;
    /* 0x39 */ u_char all_alp;
    /* 0x3a */ u_char inf_alp;
    /* 0x3b */ u_char dat_alp;
    /* 0x3c */ u_char pop_ud;
    /* 0x3d */ u_char buf_exp_flg;
    /* 0x3e */ u_char flsh_tmr;
    /* 0x3f */ u_char side_chng_wait;
    /* 0x40 */ u_char side_chng_mode;
    /* 0x41 */ u_char copy_wait;
    /* 0x42 */ u_char paste_wait;
} DSP_MEM_ALBM;


typedef struct {
	PICTURE_WRK pic[24];
} MC_ALBUM_SAVE;

extern MC_ALBUM_SAVE mc_album_save;
extern MC_ALBUM_SAVE album_save_dat[2];
// extern char is_newdata_albmside;

void MemAlbmInit(u_char h_type, u_char src_pht_max, u_char cpy_pht_max, u_char src_cl_type, u_char cpy_cl_type, u_char src_slot, u_char cpy_slot, u_char src_file, u_char cpy_file);
void MemAlbmInit2(u_char side, u_char pht_max, u_char cl_type, u_char slot, u_char file);
void MemAlbmInit3();
void NewAlbumInit(u_char side);
char SweetMemories(u_char mode, u_char alp);
void DspPhotInTopPage(u_char alp);
char NmlPadInTopPage();
char SamePhotoChk();
void KillRingBuffer();
char AbnPadInBigPage(u_char alp);
void MiniPht(u_char alp);
char MenuPadCmn();
char PasteCtrl();
void Paste();
void BuffPadCmn();
void BuffPadMnu();
void CopyToOsugi(u_char from, u_char no1, u_char to, u_char no2);
void CopyDataWithoutAdr(u_char from, u_char no1, u_char to, u_char no2);
void DelPyon(u_char file, u_char csr);
void DspOgAlbumTop1(u_char alp, u_char alp0, u_char alp1);
void DspOgAlbumTop2(u_char alp, u_char alp0, u_char alp1);
void DspOgAlbumBig(u_char alp);
void InfoWin(u_char mode, u_char side, u_char alp);
void Navion(u_char type, u_char sw, u_char alp);
void ComWin(u_char mode, u_char csr, u_char alp);
void BufWin(u_char type, u_char alp);
void DtlWin(u_char mode, u_char type, u_char alp);
void TruthPhoto(u_char mode, short int px, short int py, u_char alp);
void BtnExt(u_char mode, u_char alp);
char InKeyTamusi();
char GetAlbumCurPos(char side);

#endif // OUTGAME_MEMORY_ALBUM_H
