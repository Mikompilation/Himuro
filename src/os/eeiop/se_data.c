#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "se_data.h"

#include "os/eeiop/eese.h"
#include "os/eeiop/se_data.h"

const char se_use_static_tbl[][30] = {
    { 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 0, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 2, 2, 1 }
};
const char se_use_btlhit_tbl[][3] = {
    { 1, 1, 1 },
    { 0, 1, 1 },
};
const char se_use_voice_tbl[][5] = {
    { 1, 1, 1, 2, 0 },
    { 1, 1, 1, 2, 0 },
    { 1, 1, 1, 0, 0 },
    { 1, 1, 1, 0, 0 },
    { 1, 1, 1, 0, 0 },
    { 1, 1, 1, 0, 0 },
    { 1, 1, 1, 0, 0 },
    { 1, 1, 0, 0, 0 },
    { 1, 1, 0, 0, 0 },
    { 1, 1, 0, 0, 0 },
};
const char se_use_door_tbl[][2] = {
    { 1, 1 },
    { 1, 1 },
    { 1, 1 },
    { 1, 1 },
    { 1, 1 },
    { 1, 1 },
    { 1, 1 },
};
const char se_use_foot_tbl[][ 1] = {
    { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
    { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
    { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
    { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
};
const char se_use_srund_tbl[][ 1] = {
    { 2 }, { 2 }, { 1 }, { 2 }, { 2 }, { 2 }, { 2 },
    { 2 }, { 1 }, { 2 }, { 1 }, { 1 }, { 1 }, { 1 },
    { 2 }, { 1 }, { 1 }, { 1 }, { 2 },
};
const char se_use_ghost_tbl[][11] = {
    { 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
    { 2, 2, 1, 1, 1, 0, 1, 1, 0, 0, 0 },
    { 2, 2, 1, 1, 1, 0, 1, 0, 0, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
    { 2, 2, 1, 1, 1, 0, 1, 1, 1, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
    { 2, 2, 1, 1, 1, 0, 1, 1, 0, 0, 0 },
    { 2, 2, 1, 1, 1, 0, 1, 1, 0, 0, 0 },
    { 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
};
const char se_use_event_tbl[][ 3] = {
    { 1, 1, 0 }, { 1, 0, 0 }, { 1, 0, 0 },
    { 1, 1, 2 }, { 1, 1, 0 }, { 2, 0, 0 },
    { 1, 1, 0 }, { 1, 0, 0 }, { 1, 1, 0 },
    { 1, 0, 0 }, { 1, 1, 0 }, { 1, 0, 0 },
    { 1, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 },
    { 1, 0, 0 }, { 1, 1, 0 }, { 1, 0, 0 },
    { 1, 0, 0 }, { 1, 0, 0 }, { 1, 0, 0 },
    { 1, 0, 0 }, { 1, 1, 0 }, { 2, 0, 0 },
    { 1, 0, 0 },
};
const char se_use_wide_tbl  [][ 1] = {
    { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
    { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
    { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
};
const char se_use_jidou_tbl [][ 1] = {
   { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
   { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
};

#include "data/se_ste_tbl.h" // static SE_STE se_ste_tbl[];

// tbl_no values have different offsets across game versions
#if defined(BUILD_JP_VERSION)
#define VER_TBL_NO_OFFSET 63
#elif defined(BUILD_US_VERSION)
#define VER_TBL_NO_OFFSET 16
#elif defined(BUILD_EU_VERSION)
#define VER_TBL_NO_OFFSET 0
#endif

char CheckSeUse(int se_no)
{
    u_char se_pos;
    u_char type;
    u_char tbl_no;

    if (se_no >= 0 && se_no < SE_ENE0_GRASP)
    {
        return se_use_static_tbl[0][se_no];
    }
    else if (se_no < SE_PLYR_DMG0)
    {
        se_pos = se_no + 226;
        type = SE_ADDRNO_BTLHIT;
        tbl_no = (char)se_ctrl.btlhit_no + 169 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_DOOR_OPN_00)
    {
        se_pos = se_no + 223;
        type = SE_ADDRNO_VOICE;
        tbl_no = (char)se_ctrl.voice_no + 167 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_DOOR_OPN_01)
    {
        se_pos = se_no + 218;
        type = SE_ADDRNO_DOOR0;
        tbl_no = (char)se_ctrl.door_no[0] + 48 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_DOOR_OPN_02)
    {

        se_pos = se_no + 216;
        type = SE_ADDRNO_DOOR1;
        tbl_no = (char)se_ctrl.door_no[1] + 48 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_FOOT0)
    {
        se_pos = se_no + 214;
        type = SE_ADDRNO_DOOR2;
        tbl_no = (char)se_ctrl.door_no[2] + 48 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_FOOT1)
    {
        se_pos = se_no + 212;
        type = SE_ADDRNO_FOOT0;
        tbl_no = (char)se_ctrl.foot_no[0] + 76 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_FOOT2)
    {
        se_pos = se_no + 211;
        type = SE_ADDRNO_FOOT1;
        tbl_no = (char)se_ctrl.foot_no[1] + 76 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_FOOT3)
    {
        se_pos = se_no + 210;
        type = SE_ADDRNO_FOOT2;
        tbl_no = (char)se_ctrl.foot_no[2] + 76 + VER_TBL_NO_OFFSET;

    }
    else if (se_no < SE_FOOT4)
    {
        se_pos = se_no + 209;
        type = SE_ADDRNO_FOOT3;
        tbl_no = (char)se_ctrl.foot_no[3] + 76 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_FOOT5)
    {
        se_pos = se_no + 208;
        type = SE_ADDRNO_FOOT4;
        tbl_no = (char)se_ctrl.foot_no[4] + 76 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_FOOT6)
    {
        se_pos = se_no + 207;
        type = SE_ADDRNO_FOOT5;
        tbl_no = (char)se_ctrl.foot_no[5] + 76 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_FOOT7)
    {
        se_pos = se_no + 206;
        type = SE_ADDRNO_FOOT6;
        tbl_no = (char)se_ctrl.foot_no[6] + 76 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_SURROUND0)
    {
        se_pos = se_no + 205;
        type = SE_ADDRNO_FOOT7;
        tbl_no = (char)se_ctrl.foot_no[7] + 76 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_SURROUND1)
    {
        se_pos = se_no + 204;
        type = SE_ADDRNO_SRUND0;
        tbl_no = (char)se_ctrl.srund_no[0] + 26 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_ENE0_OMEN)
    {
        se_pos = se_no + 203;
        type = SE_ADDRNO_SRUND1;
        tbl_no = (char)se_ctrl.srund_no[1] + 26 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_ENE1_OMEN)
    {
        se_pos = se_no + 202;
        type = SE_ADDRNO_GHOST0;
        tbl_no = (char)se_ctrl.ghost_no[0] + 157 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_ENE2_OMEN)
    {
        se_pos = se_no + 191;
        type = SE_ADDRNO_GHOST1;
        tbl_no = (char)se_ctrl.ghost_no[1] + 157 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_EVENT_00)
    {
        se_pos = se_no + 180;
        type = SE_ADDRNO_GHOST2;
        tbl_no = (char)se_ctrl.ghost_no[2] + 157 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_EVENT_10)
    {
        se_pos = se_no + 169;
        type = SE_ADDRNO_EVENT0;
        tbl_no = (char)se_ctrl.event_no[0] + 121 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_HANYOU)
    {
        se_pos = se_no + 166;
        type = SE_ADDRNO_EVENT1;
        tbl_no = (char)se_ctrl.event_no[1] + 121 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_ENE_JIDOU0)
    {
        se_pos = se_no + 163;
        type = SE_ADDRNO_WIDE;
        tbl_no = (char)se_ctrl.wide_no + 7 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_ENE_JIDOU1)
    {
        se_pos = se_no + 162;
        type = SE_ADDRNO_JIDOU0;
        tbl_no = (char)se_ctrl.jidou_no[0] + 90 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_ENE_JIDOU2)
    {
        se_pos = se_no + 161;
        type = SE_ADDRNO_JIDOU1;
        tbl_no = (char)se_ctrl.jidou_no[1] + 90 + VER_TBL_NO_OFFSET;
    }
    else if (se_no < SE_ENE_JIDOU3)
    {
        se_pos = se_no + 160;
        type = SE_ADDRNO_JIDOU2;
        tbl_no = (char)se_ctrl.jidou_no[2] + 90 + VER_TBL_NO_OFFSET;
    }
    else if (se_no <= SE_ENE_JIDOU3)
    {
        se_pos = se_no + 159;
        type = SE_ADDRNO_JIDOU3;
        tbl_no = (char)se_ctrl.jidou_no[3] + 90 + VER_TBL_NO_OFFSET;
    }
    else
    {
        return 0;
    }

    return GetSeUseTbl(type, tbl_no, se_pos);
}

char GetSeUseTbl(u_char type, u_char tbl_no, u_char se_no)
{
    switch (type)
    {
    case SE_ADDRNO_BTLHIT:
        if (se_no >= 3)
        {
            return 0;
        }
        if (tbl_no >= 6)
        {
            return 0;
        }
        return se_use_btlhit_tbl[tbl_no][se_no];
    break;
    case SE_ADDRNO_VOICE:
        if (se_no >= 5)
        {
            return 0;
        }
        if (tbl_no >= 50)
        {
            return 0;
        }
        return se_use_voice_tbl[tbl_no][se_no];
    break;
    case SE_ADDRNO_SRUND0:
    case SE_ADDRNO_SRUND1:
        if (se_no >= 1)
        {
            return 0;
        }
        if (tbl_no >= 19)
        {
            return 0;
        }
        return se_use_srund_tbl[tbl_no][se_no];
    break;
    case SE_ADDRNO_GHOST0:
    case SE_ADDRNO_GHOST1:
    case SE_ADDRNO_GHOST2:
        if (se_no >= 11)
        {
            return 0;
        }
        // if (tbl_no >= 27) probably checking against a number > `MAX_U_CHAR` (255)
        // {
        //     return 0;
        // }
        return se_use_ghost_tbl[tbl_no][se_no];
    break;
    case SE_ADDRNO_EVENT0:
    case SE_ADDRNO_EVENT1:
        if (se_no >= 3)
        {
            return 0;
        }
        if (tbl_no >= 75)
        {
            return 0;
        }
        return se_use_event_tbl[tbl_no][se_no];
    break;
    case SE_ADDRNO_WIDE:
        if (se_no >= 1)
        {
            return 0;
        }
        if (tbl_no >= 15)
        {
            return 0;
        }
        return se_use_wide_tbl[tbl_no][se_no];
    break;
    case SE_ADDRNO_JIDOU0:
    case SE_ADDRNO_JIDOU1:
    case SE_ADDRNO_JIDOU2:
    case SE_ADDRNO_JIDOU3:
        if (se_no >= 1)
        {
            return 0;
        }
        if (tbl_no >= 14)
        {
            return 0;
        }
        return se_use_jidou_tbl[tbl_no][se_no];
    break;
    case SE_ADDRNO_DOOR0:
    case SE_ADDRNO_DOOR1:
    case SE_ADDRNO_DOOR2:
    case SE_ADDRNO_FOOT0:
    case SE_ADDRNO_FOOT1:
    case SE_ADDRNO_FOOT2:
    case SE_ADDRNO_FOOT3:
    case SE_ADDRNO_FOOT4:
    case SE_ADDRNO_FOOT5:
    case SE_ADDRNO_FOOT6:
    case SE_ADDRNO_FOOT7:
        return; // missing return value
    break;
    }

    return 0;
}

SE_STE *SeGetSeSte(int num)
{
    return &se_ste_tbl[num];
}
