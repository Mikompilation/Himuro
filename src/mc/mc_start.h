#ifndef MC_MC_START_H
#define MC_MC_START_H

#include "typedefs.h"

typedef struct {
    int type;
    int free;
    int format;
    int dir;
} MC_START_CHECK;

extern MC_START_CHECK mc_start_check[];

char mcStartCheckMain();
void mcStartCheckSet();
char mcStartCheckResult();
char mcStartCheckYesno(int def_pos);

#ifdef BUILD_EU_VERSION
char mcLanguageLoadMain();
char mcCtrlCheckLanguage();
#endif

#endif // MC_MC_START_H
