#ifndef MAIN_GAMEMAIN_H
#define MAIN_GAMEMAIN_H

#include "typedefs.h"

// extern int init_load_id;
// extern int init_load_adr;
// extern int init_load_size;

void GameMain();
int GameInit();
int GameInitLoad();
void GameModeChange(u_char mode);

#ifdef BUILD_EU_VERSION
int LanguageSelectMain();
#endif

#endif // MAIN_GAMEMAIN_H
