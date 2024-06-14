#ifndef INGAME_IG_INIT_H
#define INGAME_IG_INIT_H

#include "typedefs.h"

// extern LOAD_START_WRK load_start_wrk;

void InitCamera();
void InitPlyr();
void InitPlyr2(int film_no);
void InitPlyrAfterLoad();
void InitEnemy();
void EnemyActDataLoad();
void InitFlyWrk();
void InitFilm();
void LoadStartDataInit();
int LoadStartDataSet();
void InitLoadStartLock();
int GetLoadStartLock();

#endif // INGAME_IG_INIT_H
