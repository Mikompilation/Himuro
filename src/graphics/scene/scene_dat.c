#include "common.h"
#include "typedefs.h"
#include "scene_dat.h"

#include "data/scn_ene_efct.h" // SCN_ENE_EFCT scn_ene_efct[];
#include "data/scn_vib_type.h" // SCN_VIB_TYPE scn_vib_type[];
#include "data/scn_vib_0020.h" // SCN_VIB_DATA scn_vib_0020[];
#include "data/scn_vib_0022.h" // SCN_VIB_DATA scn_vib_0022[];
#include "data/scn_vib_0030.h" // SCN_VIB_DATA scn_vib_0030[];
#include "data/scn_vib_0031.h" // SCN_VIB_DATA scn_vib_0031[];
#include "data/scn_vib_1010.h" // SCN_VIB_DATA scn_vib_1010[];
#include "data/scn_vib_1030.h" // SCN_VIB_DATA scn_vib_1030[];
#include "data/scn_vib_1031.h" // SCN_VIB_DATA scn_vib_1031[];
#include "data/scn_vib_1034.h" // SCN_VIB_DATA scn_vib_1034[];
#include "data/scn_vib_1070.h" // SCN_VIB_DATA scn_vib_1070[];
#include "data/scn_vib_1080.h" // SCN_VIB_DATA scn_vib_1080[];
#include "data/scn_vib_1090.h" // SCN_VIB_DATA scn_vib_1090[];
#include "data/scn_vib_1100.h" // SCN_VIB_DATA scn_vib_1100[];
#include "data/scn_vib_1101.h" // SCN_VIB_DATA scn_vib_1101[];
#include "data/scn_vib_1102.h" // SCN_VIB_DATA scn_vib_1102[];
#include "data/scn_vib_1120.h" // SCN_VIB_DATA scn_vib_1120[];
#include "data/scn_vib_1150.h" // SCN_VIB_DATA scn_vib_1150[];
#include "data/scn_vib_1160.h" // SCN_VIB_DATA scn_vib_1160[];
#include "data/scn_vib_1170.h" // SCN_VIB_DATA scn_vib_1170[];
#include "data/scn_vib_1180.h" // SCN_VIB_DATA scn_vib_1180[];
#include "data/scn_vib_1220.h" // SCN_VIB_DATA scn_vib_1220[];
#include "data/scn_vib_1230.h" // SCN_VIB_DATA scn_vib_1230[];
#include "data/scn_vib_1240.h" // SCN_VIB_DATA scn_vib_1240[];
#include "data/scn_vib_1250.h" // SCN_VIB_DATA scn_vib_1250[];
#include "data/scn_vib_1260.h" // SCN_VIB_DATA scn_vib_1260[];
#include "data/scn_vib_1300.h" // SCN_VIB_DATA scn_vib_1300[];
#include "data/scn_vib_1301.h" // SCN_VIB_DATA scn_vib_1301[];
#include "data/scn_vib_1330.h" // SCN_VIB_DATA scn_vib_1330[];
#include "data/scn_vib_1332.h" // SCN_VIB_DATA scn_vib_1332[];
#include "data/scn_vib_1340.h" // SCN_VIB_DATA scn_vib_1340[];
#include "data/scn_vib_1410.h" // SCN_VIB_DATA scn_vib_1410[];
#include "data/scn_vib_2010.h" // SCN_VIB_DATA scn_vib_2010[];
#include "data/scn_vib_2013.h" // SCN_VIB_DATA scn_vib_2013[];
#include "data/scn_vib_2020.h" // SCN_VIB_DATA scn_vib_2020[];
#include "data/scn_vib_2030.h" // SCN_VIB_DATA scn_vib_2030[];
#include "data/scn_vib_2040.h" // SCN_VIB_DATA scn_vib_2040[];
#include "data/scn_vib_2060.h" // SCN_VIB_DATA scn_vib_2060[];
#include "data/scn_vib_2072.h" // SCN_VIB_DATA scn_vib_2072[];
#include "data/scn_vib_2090.h" // SCN_VIB_DATA scn_vib_2090[];
#include "data/scn_vib_2091.h" // SCN_VIB_DATA scn_vib_2091[];
#include "data/scn_vib_2092.h" // SCN_VIB_DATA scn_vib_2092[];
#include "data/scn_vib_2110.h" // SCN_VIB_DATA scn_vib_2110[];
#include "data/scn_vib_2130.h" // SCN_VIB_DATA scn_vib_2130[];
#include "data/scn_vib_2131.h" // SCN_VIB_DATA scn_vib_2131[];
#include "data/scn_vib_2132.h" // SCN_VIB_DATA scn_vib_2132[];
#include "data/scn_vib_2140.h" // SCN_VIB_DATA scn_vib_2140[];
#include "data/scn_vib_2141.h" // SCN_VIB_DATA scn_vib_2141[];
#include "data/scn_vib_2142.h" // SCN_VIB_DATA scn_vib_2142[];
#include "data/scn_vib_2150.h" // SCN_VIB_DATA scn_vib_2150[];
#include "data/scn_vib_2160.h" // SCN_VIB_DATA scn_vib_2160[];
#include "data/scn_vib_3010.h" // SCN_VIB_DATA scn_vib_3010[];
#include "data/scn_vib_3030.h" // SCN_VIB_DATA scn_vib_3030[];
#include "data/scn_vib_3040.h" // SCN_VIB_DATA scn_vib_3040[];
#include "data/scn_vib_3060.h" // SCN_VIB_DATA scn_vib_3060[];
#include "data/scn_vib_3070.h" // SCN_VIB_DATA scn_vib_3070[];
#include "data/scn_vib_3080.h" // SCN_VIB_DATA scn_vib_3080[];
#include "data/scn_vib_3090.h" // SCN_VIB_DATA scn_vib_3090[];
#include "data/scn_vib_3100.h" // SCN_VIB_DATA scn_vib_3100[];
#include "data/scn_vib_3110.h" // SCN_VIB_DATA scn_vib_3110[];
#include "data/scn_vib_4011.h" // SCN_VIB_DATA scn_vib_4011[];
#include "data/scn_vib_4020.h" // SCN_VIB_DATA scn_vib_4020[];
#include "data/scn_vib_4041.h" // SCN_VIB_DATA scn_vib_4041[];
#include "data/scn_vib_4042.h" // SCN_VIB_DATA scn_vib_4042[];
#include "data/scn_vib_4050.h" // SCN_VIB_DATA scn_vib_4050[];
#include "data/scn_vib_4070.h" // SCN_VIB_DATA scn_vib_4070[];
#include "data/scn_vib_4090.h" // SCN_VIB_DATA scn_vib_4090[];
#include "data/scn_vib_4100.h" // SCN_VIB_DATA scn_vib_4100[];
#include "data/scn_vib_4110.h" // SCN_VIB_DATA scn_vib_4110[];
#include "data/scn_vib_4120.h" // SCN_VIB_DATA scn_vib_4120[];
#include "data/scn_vib_5010.h" // SCN_VIB_DATA scn_vib_5010[];
#include "data/scn_vib_5020.h" // SCN_VIB_DATA scn_vib_5020[];
#include "data/scn_vib_dmy.h" // SCN_VIB_DATA scn_vib_dmy[];
SCN_VIB_DATA *scn_vib_tbl[] = {
    scn_vib_dmy,
    scn_vib_0020,
    scn_vib_0022,
    scn_vib_0030,
    scn_vib_0031,
    scn_vib_dmy,
    scn_vib_1010,
    scn_vib_1030,
    scn_vib_1031,
    scn_vib_1034,
    scn_vib_1070,
    scn_vib_1080,
    scn_vib_1090,
    scn_vib_1100,
    scn_vib_1101,
    scn_vib_1102,
    scn_vib_1120,
    scn_vib_1150,
    scn_vib_1160,
    scn_vib_1170,
    scn_vib_1180,
    scn_vib_1220,
    scn_vib_1230,
    scn_vib_1240,
    scn_vib_1250,
    scn_vib_1260,
    scn_vib_dmy,
    scn_vib_1300,
    scn_vib_1301,
    scn_vib_dmy,
    scn_vib_1330,
    scn_vib_dmy,
    scn_vib_1332,
    scn_vib_1340,
    scn_vib_1410,
    scn_vib_2010,
    scn_vib_dmy,
    scn_vib_2020,
    scn_vib_2030,
    scn_vib_2040,
    scn_vib_dmy,
    scn_vib_2060,
    scn_vib_dmy,
    scn_vib_dmy,
    scn_vib_dmy,
    scn_vib_dmy,
    scn_vib_2072,
    scn_vib_2090,
    scn_vib_2091,
    scn_vib_2092,
    scn_vib_dmy,
    scn_vib_2110,
    scn_vib_2130,
    scn_vib_2131,
    scn_vib_2132,
    scn_vib_2140,
    scn_vib_2141,
    scn_vib_2142,
    scn_vib_dmy,
    scn_vib_2150,
    scn_vib_2160,
    scn_vib_dmy,
    scn_vib_dmy,
    scn_vib_3010,
    scn_vib_dmy,
    scn_vib_3030,
    scn_vib_3040,
    scn_vib_dmy,
    scn_vib_3060,
    scn_vib_3070,
    scn_vib_3080,
    scn_vib_dmy,
    scn_vib_3090,
    scn_vib_3100,
    scn_vib_3110,
    scn_vib_dmy,
    scn_vib_4011,
    scn_vib_4020,
    scn_vib_dmy,
    scn_vib_dmy,
    scn_vib_dmy,
    scn_vib_4041,
    scn_vib_4042,
    scn_vib_4050,
    scn_vib_dmy,
    scn_vib_4070,
    scn_vib_dmy,
    scn_vib_4090,
    scn_vib_4100,
    scn_vib_4110,
    scn_vib_4120,
    scn_vib_5010,
    scn_vib_5020,
    scn_vib_dmy,
    scn_vib_dmy,
    scn_vib_dmy,
    scn_vib_dmy,
    scn_vib_dmy,
    scn_vib_dmy,
    scn_vib_dmy,
};
#include "data/scene_movie_no.h" // u_char scene_movie_no[];
