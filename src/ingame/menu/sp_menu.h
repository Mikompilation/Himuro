#ifndef INGAME_MENU_SP_MENU_H
#define INGAME_MENU_SP_MENU_H

#include "typedefs.h"

// extern SAVE_POINT_MENU_WRK spmenu_wrk;

void SavePointMenuInit();
void SavePointMenuOpen();
void SavePointMenuOpenInit(u_char msn);
void BtlModSaveInit();
char SavePointMenuMain(u_char msn);
void SavePointMenuModeSlct(u_char msn, u_char mode);
void SavePointMenuModeSlctDisp(u_char msn, u_char csr3, u_char csr4, u_char alp_max, u_char mode);
void SavePointMenuAlbum();
void SavePointMenuSave();
void DspMemSavePoint(u_char msk, u_char msg, u_char fdt, u_char svp, u_char alt, u_char csr0, u_char csr1, u_char csr2, int yes_no, int alp_max, int type);
char SaveCamChk();
char AlbmSlct(u_char csr3, u_char step, u_char alp_max, u_char ini);
void AlbumModeInGameOverInit();
char AlbumModeInGameOver();
int SavePointLightJudge();

#endif // INGAME_MENU_SP_MENU_H
