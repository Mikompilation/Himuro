#ifndef OS_EEIOP_ADPCM_EA_SCENE_H
#define OS_EEIOP_ADPCM_EA_SCENE_H

#include "typedefs.h"

void EAdpcmSceneMain();
void AdpcmPreSceneFadeOut(u_short fout_flm);
u_char IsFadeoutEndAdpcmScene();
void AdpcmScenePreLoadReq(int scene_no);
int IsAdpcmScenePreLoadEnd();
void AdpcmScenePreLoadEndPlay(int scene_no);
void AdpcmSceneStop();

#endif // OS_EEIOP_ADPCM_EA_SCENE_H
