#ifndef INGAME_MENU_IG_ALBUM_H
#define INGAME_MENU_IG_ALBUM_H

#include "typedefs.h"

void NewgameMenuAlbumInit();
void LoadgameMenuAlbumInit();
void StartAlbumModeInit();
void IngameMenuAlbum(char is_outgame);
void IngameAlbumAllPad(u_char *csr, u_char *csr_top, u_char *mode);
void IngameAlbumLstPad(u_char *csr, u_char *csr_top, u_char *mode);
void IngameAlbumBigPad(u_char *csr, u_char *csr_top, u_char *mode);
void IngameAlbumDispPhotoLarge(u_char csr);
void IngameAlbumDisp(u_char csr, u_char csr_top, u_char mode, char is_outgame);
u_char* GetSubjectNameAddr(u_short kind, u_short no, u_short show);
u_char TimeIsMoney(u_char hexhex);
u_char MoneyIsTime(u_char time);

#ifdef MATCHING_DECOMP
#ifdef INCLUDING_FROM_IG_ALBUM_C
// fix for undeclared PutDate in header
void PutDate(PICTURE_WRK *pic_inf, short int pos_x, short int pos_y, short int alpha, int pri);
#endif
#else
void PutDate(PICTURE_WRK *pic_inf, short int pos_x, short int pos_y, short int alpha, int pri);
#endif

void OutGameInitPhoto();
int isPhotoEnd();
float GetPhtDispAlpha();

#endif // INGAME_MENU_IG_ALBUM_H
