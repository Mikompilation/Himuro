#include "common.h"
#include "typedefs.h"
#include "fsla_dat.h"

#include "graphics/graph3d/light_types.h"

#include "data/fspe_lightanm_fadein1.h" // FSPE_LIGHT_ANM fspe_lightanm_fadein1[];
#include "data/fspe_lightanm_fadein2.h" // FSPE_LIGHT_ANM fspe_lightanm_fadein2[];
#include "data/fspe_lightanm_normal1.h" // FSPE_LIGHT_ANM fspe_lightanm_normal1[];
#include "data/fspe_lightanm_normal2.h" // FSPE_LIGHT_ANM fspe_lightanm_normal2[];
#include "data/fspe_lightanm_normal3.h" // FSPE_LIGHT_ANM fspe_lightanm_normal3[];
#include "data/fspe_lightanm_normal4.h" // FSPE_LIGHT_ANM fspe_lightanm_normal4[];
#include "data/fspe_lightanm_strong1.h" // FSPE_LIGHT_ANM fspe_lightanm_strong1[];
#include "data/fspe_lightanm_strong2.h" // FSPE_LIGHT_ANM fspe_lightanm_strong2[];
#include "data/fspe_lightanm_fadeout1.h" // FSPE_LIGHT_ANM fspe_lightanm_fadeout1[];
#include "data/fspe_lightanm_fadeout2.h" // FSPE_LIGHT_ANM fspe_lightanm_fadeout2[];
FSPE_LIGHT_ANM *fspe_la_normal1[] = {
    fspe_lightanm_normal1, fspe_lightanm_normal2,
    NULL,
};
FSPE_LIGHT_ANM *fspe_la_strong[] = {
    fspe_lightanm_strong1, fspe_lightanm_strong2, 
    NULL, 
};
FSPE_LIGHT_ANM *fspe_la_normal2[] = {
    fspe_lightanm_normal3, fspe_lightanm_normal4, 
    NULL,
};
FSPE_LIGHT_ANM *fspe_la_inp1[] = { fspe_lightanm_fadein1 };
FSPE_LIGHT_ANM *fspe_la_inp2[] = { fspe_lightanm_fadein2 };
FSPE_LIGHT_ANM *fspe_la_outp1[] = { fspe_lightanm_fadeout1 };
FSPE_LIGHT_ANM *fspe_la_outp2[] = { fspe_lightanm_fadeout2 };
FSPE_LIGHT_ANM **fspe_la_array[] = {
    fspe_la_normal1, fspe_la_strong, fspe_la_normal2,
    fspe_la_inp1, fspe_la_inp2,
    fspe_la_outp1, fspe_la_outp2,
    NULL,
};
