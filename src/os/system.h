#ifndef OS_SYSTEM_H
#define OS_SYSTEM_H

#include "typedefs.h"
#include "ee/eestruct.h"
#include "sce/libgraph.h"
#include "sce/libcdvd.h"

extern int odev;
extern int stop_put_draw_env;

#ifdef BUILD_EU_VERSION
extern int fr;
#endif

extern sceGsDBuff g_db;
extern sceGsDrawEnv1 *pdrawenv;

void InitSystem();
void EndSystem();
void LoadIRX();
void InitSysWrk();
void GetNowClock(sceCdCLOCK *nc);
void InitOptionWrk();
void InitGraphics();
int userGsSwapDBuff(sceGsDBuff *db, int id);
int vblankHandler(int val);
// void vfunc();
void SetSysBackColor(u_char r, u_char g, u_char b);

#endif // OS_SYSTEM_H
