#ifndef OUTGAME_MEMORY_ALBUM_H
#define OUTGAME_MEMORY_ALBUM_H

#include "typedefs.h"

#include "ingame/menu/ig_rank.h"

typedef struct {
    u_int src_addr;
    u_int cpy_addr;
    u_short key_nashi_nori_take;
    u_char is_title;
    u_char side;
    u_char side_chng;
    u_char mode;
    u_char mode_chng;
    u_char pht_max[2];
    u_char cl_type[2];
    u_char slot[2];
    u_char file[2];
    u_char csr[5];
    u_char csr_tim;
    u_char buf[9];
    u_char buf_sid[9];
    u_char buf_set;
    u_char mem;
    u_char csr_mode;
    u_char exit_realy;
    u_char pst_mode;
    u_char menu_alp;
    u_char yes_no;
    u_char pst_yes_no;
    u_char rtrn_bak;
    u_char io_mode;
    u_char all_alp;
    u_char inf_alp;
    u_char dat_alp;
    u_char pop_ud;
    u_char buf_exp_flg;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    u_char flsh_tmr;
    u_char side_chng_wait;
    u_char side_chng_mode;
    u_char copy_wait;
    u_char paste_wait;
#endif
} DSP_MEM_ALBM;


typedef struct {
	PICTURE_WRK pic[24];
} MC_ALBUM_SAVE;

extern MC_ALBUM_SAVE mc_album_save;
extern MC_ALBUM_SAVE album_save_dat[2];
extern char is_newdata_albmside;

void MemAlbmInit(u_char h_type, u_char src_pht_max, u_char cpy_pht_max, u_char src_cl_type, u_char cpy_cl_type, u_char src_slot, u_char cpy_slot, u_char src_file, u_char cpy_file);
#ifndef INCLUDING_FROM_SP_MENU_C
// fix for undeclared MemAlbmInit2 in ingame/menu/sp_menu.c
void MemAlbmInit2(u_char side, u_char pht_max, u_char cl_type, u_char slot, u_char file);
#endif
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
