#ifndef INGAME_MENU_IG_OPTION_H
#define INGAME_MENU_IG_OPTION_H

#include "typedefs.h"

void NewgameMenuOptionInit();
void LoadgameMenuOptionInit();
void StartOptionModeInit(u_char mode);
void IngameMenuOption();
char MenuOptionPad(u_char *csr, u_char *mode);
void DspOptCtrl(u_char csr, u_char mode, u_char alpha, u_char ttl);
void StepLinesSimpleDraw(short int *p_stp[2], u_char r, u_char g, u_char b, u_char alpha);
void SimpleHorLine(short int *p_stp[3], u_char r, u_char g, u_char b, u_char alpha, int typ);

#endif // INGAME_MENU_IG_OPTION_H
