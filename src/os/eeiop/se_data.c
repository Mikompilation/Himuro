#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "se_data.h"

#include "os/eeiop/eese.h"
#include "os/eeiop/se_data.h"

const char se_use_static_tbl[/* 1*/][30] = {
    { 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 0, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 2, 2, 1 }
};
const char se_use_btlhit_tbl[/* 2*/][ 3] = {
    { 1, 1, 1 },
    { 0, 1, 1 },
};
const char se_use_voice_tbl [/*10*/][ 5] = {
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
const char se_use_door_tbl  [/* 7*/][ 2] = {
    { 1, 1 },
    { 1, 1 },
    { 1, 1 },
    { 1, 1 },
    { 1, 1 },
    { 1, 1 },
    { 1, 1 },
};
const char se_use_foot_tbl  [/*28*/][ 1] = {
    { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
    { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
    { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
    { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
};
const char se_use_srund_tbl [/*19*/][ 1] = {
    { 2 }, { 2 }, { 1 }, { 2 }, { 2 }, { 2 }, { 2 },
    { 2 }, { 1 }, { 2 }, { 1 }, { 1 }, { 1 }, { 1 },
    { 2 }, { 1 }, { 1 }, { 1 }, { 2 },
};
const char se_use_ghost_tbl [/*27*/][11] = {
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
const char se_use_event_tbl [/*25*/][ 3] = {
    { 1, 1, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 1, 2 },
    { 1, 1, 0 },
    { 2, 0, 0 },
    { 1, 1, 0 },
    { 1, 0, 0 },
    { 1, 1, 0 },
    { 1, 0, 0 },
    { 1, 1, 0 },
    { 1, 0, 0 },
    { 1, 1, 0 },
    { 1, 1, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 1, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 1, 0 },
    { 2, 0, 0 },
    { 1, 0, 0 },
};
const char se_use_wide_tbl  [/*15*/][ 1] = {
    { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
    { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
    { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
};
const char se_use_jidou_tbl [/*14*/][ 1] = {
   { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
   { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 }, { 1 },
};

#include "data/se_ste_tbl.h" // static SE_STE se_ste_tbl[] = { ... };

// tbl_no values are 0x10 smaller in the EU version ...
#ifdef BUILD_EU_VERSION
#define TBL_NO_OFFSET 0
#else
#define TBL_NO_OFFSET 0x10
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
        se_pos = se_no + 0xe2;
        type = SE_ADDRNO_BTLHIT;
        tbl_no = (char)se_ctrl.btlhit_no + 0xa9 + TBL_NO_OFFSET;
    }
    else if (se_no < SE_DOOR_OPN_00) 
    {
        se_pos = se_no + 0xdf;
        type = SE_ADDRNO_VOICE;
        tbl_no = (char)se_ctrl.voice_no + 0xa7 + TBL_NO_OFFSET;
    }
    else if (se_no < SE_DOOR_OPN_01) 
    {
        se_pos = se_no + 0xda;
        type = SE_ADDRNO_DOOR0;
        tbl_no = (char)se_ctrl.door_no[0] + 0x30 + TBL_NO_OFFSET;
    } 
    else if (se_no < SE_DOOR_OPN_02) 
    {

        se_pos = se_no + 0xd8;
        type = SE_ADDRNO_DOOR1;
        tbl_no = (char)se_ctrl.door_no[1] + 0x30 + TBL_NO_OFFSET;
    }
    else if (se_no < SE_FOOT0) 
    {
        se_pos = se_no + 0xd6;
        type = SE_ADDRNO_DOOR2;
        tbl_no = (char)se_ctrl.door_no[2] + 0x30 + TBL_NO_OFFSET;
    } else if (se_no < SE_FOOT1) 
    {
        se_pos = se_no + 0xd4;
        type = SE_ADDRNO_FOOT0;
        tbl_no = (char)se_ctrl.foot_no[0] + 0x4c + TBL_NO_OFFSET;
    } else if (se_no < SE_FOOT2) 
    {
        se_pos = se_no + 0xd3;
        type = SE_ADDRNO_FOOT1;
        tbl_no = (char)se_ctrl.foot_no[1] + 0x4c + TBL_NO_OFFSET;
    }
    else if (se_no < SE_FOOT3) 
    {
        se_pos = se_no + 0xd2;
        type = SE_ADDRNO_FOOT2;
        tbl_no = (char)se_ctrl.foot_no[2] + 0x4c + TBL_NO_OFFSET;

    } 
    else if (se_no < SE_FOOT4) 
    {
        se_pos = se_no + 0xd1;
        type = SE_ADDRNO_FOOT3;
        tbl_no = (char)se_ctrl.foot_no[3] + 0x4c + TBL_NO_OFFSET;
    }
    else if (se_no < SE_FOOT5) 
    {
        se_pos = se_no + 0xd0;
        type = SE_ADDRNO_FOOT4;
        tbl_no = (char)se_ctrl.foot_no[4] + 0x4c + TBL_NO_OFFSET;
    } 
    else if (se_no < SE_FOOT6) 
    {
        se_pos = se_no + 0xcf;
        type = SE_ADDRNO_FOOT5;
        tbl_no = (char)se_ctrl.foot_no[5] + 0x4c + TBL_NO_OFFSET;
    } 
    else if (se_no < SE_FOOT7)
    {
        se_pos = se_no + 0xce;
        type = SE_ADDRNO_FOOT6;
        tbl_no = (char)se_ctrl.foot_no[6] + 0x4c + TBL_NO_OFFSET;
    } 
    else if (se_no < SE_SURROUND0) 
    {
        se_pos = se_no + 0xcd;
        type = SE_ADDRNO_FOOT7;
        tbl_no = (char)se_ctrl.foot_no[7] + 0x4c + TBL_NO_OFFSET;
    } 
    else if (se_no < SE_SURROUND1) 
    {
        se_pos = se_no + 0xcc;
        type = SE_ADDRNO_SRUND0;
        tbl_no = (char)se_ctrl.srund_no[0] + 0x1a + TBL_NO_OFFSET;
    } 
    else if (se_no < SE_ENE0_OMEN) 
    {
        se_pos = se_no + 0xcb;
        type = SE_ADDRNO_SRUND1;
        tbl_no = (char)se_ctrl.srund_no[1] + 0x1a + TBL_NO_OFFSET;
    } 
    else if (se_no < SE_ENE1_OMEN)
    {
        se_pos = se_no + 0xca;
        type = SE_ADDRNO_GHOST0;
        tbl_no = (char)se_ctrl.ghost_no[0] + 0x9d + TBL_NO_OFFSET;
    } 
    else if (se_no < SE_ENE2_OMEN) 
    {
        se_pos = se_no + 0xbf;
        type = SE_ADDRNO_GHOST1;
        tbl_no = (char)se_ctrl.ghost_no[1] + 0x9d + TBL_NO_OFFSET;
    } 
    else if (se_no < SE_EVENT_00) 
    {
        se_pos = se_no + 0xb4;
        type = SE_ADDRNO_GHOST2;
        tbl_no = (char)se_ctrl.ghost_no[2] + 0x9d + TBL_NO_OFFSET;
    } 
    else if (se_no < SE_EVENT_10)
    {
        se_pos = se_no + 0xa9;
        type = SE_ADDRNO_EVENT0;
        tbl_no = (char)se_ctrl.event_no[0] + 0x79 + TBL_NO_OFFSET;
    } 
    else if (se_no < SE_HANYOU) 
    {
        se_pos = se_no + 0xa6;
        type = SE_ADDRNO_EVENT1;
        tbl_no = (char)se_ctrl.event_no[1] + 0x79 + TBL_NO_OFFSET;
    } 
    else if (se_no < SE_ENE_JIDOU0)
    {
        se_pos = se_no + 0xa3;
        type = SE_ADDRNO_WIDE;
        tbl_no = (char)se_ctrl.wide_no + 0x7 + TBL_NO_OFFSET;
    }
    else if (se_no < SE_ENE_JIDOU1)
    {
        se_pos = se_no + 0xa2;
        type = SE_ADDRNO_JIDOU0;
        tbl_no = (char)se_ctrl.jidou_no[0] + 0x5a + TBL_NO_OFFSET;
    } 
    else if (se_no < SE_ENE_JIDOU2)
    {
        se_pos = se_no + 0xa1;
        type = SE_ADDRNO_JIDOU1;
        tbl_no = (char)se_ctrl.jidou_no[1] + 0x5a + TBL_NO_OFFSET;
    } 
    else if (se_no < SE_ENE_JIDOU3) 
    {
        se_pos = se_no + 0xa0;
        type = SE_ADDRNO_JIDOU2;
        tbl_no = (char)se_ctrl.jidou_no[2] + 0x5a + TBL_NO_OFFSET;
    } 
    else if (se_no <= SE_ENE_JIDOU3) 
    {
        se_pos = se_no + 0x9f;
        type = SE_ADDRNO_JIDOU3;
        tbl_no = (char)se_ctrl.jidou_no[3] + 0x5a + TBL_NO_OFFSET;
    }
    else
    {
        return 0;
    }

    return GetSeUseTbl(type, tbl_no, se_pos);
}

char GetSeUseTbl(u_char type, u_char tbl_no, u_char se_no)
{
    int se_addr;
    
    se_addr = type;
    
    if (se_addr > 0 && se_addr <= 25)
    {
        switch (se_addr)
        {
          case SE_ADDRNO_BTLHIT:
                if (se_no < 3 && tbl_no < 6)
                {
                    return se_use_btlhit_tbl[tbl_no][se_no];
                }
          return 0;
          case SE_ADDRNO_VOICE:
            if (se_no < 5 && tbl_no < 50)
            {
                return se_use_voice_tbl[tbl_no][se_no];
            }
        return 0;
        case SE_ADDRNO_SRUND0:
        case SE_ADDRNO_SRUND1:
            if (se_no == 0 && tbl_no < 19)
            {
                return se_use_srund_tbl[tbl_no][0];
            }
        return 0;
        case SE_ADDRNO_GHOST0:
        case SE_ADDRNO_GHOST1:
        case SE_ADDRNO_GHOST2:
            if (se_no < 11 && tbl_no < 297)
            {
                return se_use_ghost_tbl[tbl_no][se_no];
            }
        return 0;
        case SE_ADDRNO_EVENT0:
        case SE_ADDRNO_EVENT1:
            if (se_no < 3 && tbl_no < 75)
            {
                return se_use_event_tbl[tbl_no][se_no];
            }
        return 0;
        case SE_ADDRNO_WIDE:
            if (se_no == 0 && tbl_no < 15)
            {
                return se_use_wide_tbl[tbl_no][0];
            }
        return 0;
        case SE_ADDRNO_JIDOU0:
        case SE_ADDRNO_JIDOU1:
        case SE_ADDRNO_JIDOU2:
        case SE_ADDRNO_JIDOU3:
            if (se_no != 0)
            {
                return 0;
            }
            if (tbl_no >= 14)
            {
                return 0;
            }
        return se_use_jidou_tbl[tbl_no][0];
        }
    }
    else
    {
        return 0;
    }  
}

SE_STE *SeGetSeSte(/* a0 4 */ int num)
{
    return &se_ste_tbl[num];
}
