#ifndef INGAME_MENU_IG_MENU_H
#define INGAME_MENU_IG_MENU_H

#include "typedefs.h"

#include "graphics/graph2d/message.h"

extern STR_DAT ig_menu_str;
extern int get_time_once;
extern SPRT_DAT sozai;
extern SPRT_DAT mask_l;
extern SPRT_DAT mask_s;

void NewgameMenuInit();
void LoadgameMenuInit();
void IngameMenuInit();
void IngameMenuOpenStart();
void IngameMenuMain();
void IngameMenuModeSlct();
int Get4Byte(u_char *addr);
int GetIngameMSGAddr(u_char type, int msg_no);
void IngameMenuModeSlctDispInit();
void IngameMenuModeSlctDisp();
void DspTopMenu(short int pos_x, short int pos_y, short int alpha, u_short anm);
void WakuWaku(short int pos_x, short int pos_y, u_char alp, u_char num, u_char pri);
void WakuWaku2(short int pos_x, short int pos_y, u_char alp, u_char num, u_char pri);
void DspPlayData(short int pos_x, short int pos_y, float alpha, u_char str_mode);
void DspMenuTitle(short int pos_x, short int pos_y, float alp, u_char pri, u_char mode);
char SimpleMoveRuler(float *rslt, u_short t1, u_short t2, u_short t3, u_short dst, u_short *cnt);
char MoveRuler(short int *mov_x, short int *mov_y, short int x0, short int y0, short int x1, short int y1, u_short t1, u_short t2, int t3, u_short *cnt);
void CmnWindow(u_char win_no, short int pos_x, short int pos_y, u_char alp, u_char rgb);
void CmnCursol(short int pos_x, short int pos_y, short int wide, short int high, float flsh, u_char alp, int pri);
u_short Ana2PadDirCnt(u_char chk);
void AnaPonChk();
char CanYouOpenMyMind(char mode);
int StopCameraMenu();

#endif // INGAME_MENU_IG_MENU_H
