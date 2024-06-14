#ifndef MC_MC_START_H
#define MC_MC_START_H

#include "typedefs.h"

// extern MC_START_CHECK mc_start_check[0];

char mcStartCheckMain();
void mcStartCheckSet();
char mcStartCheckResult();
char mcStartCheckYesno(int def_pos);

#ifdef BUILD_EU_VERSION
char mcLanguageLoadMain();
char mcCtrlCheckLanguage();
#endif

#endif // MC_MC_START_H
