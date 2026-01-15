#include "common.h"
#include "typedefs.h"
#include "doortype.h"

#include "data/door_type_dat.h" // DOOR_TYPE_DAT door_type_dat[];
#include "data/dopen_tbl_dplrn.h" // static float dopen_tbl_dplrn[];
#include "data/dopen_tbl_dplrc.h" // static float dopen_tbl_dplrc[];
#include "data/dopen_tbl_dplrl.h" // static float dopen_tbl_dplrl[];
#include "data/dopen_tbl_dpsrc.h" // static float dopen_tbl_dpsrc[];
#include "data/dopen_tbl_dplln.h" // static float dopen_tbl_dplln[];
#include "data/dopen_tbl_dpllc.h" // static float dopen_tbl_dpllc[];
#include "data/dopen_tbl_dplll.h" // static float dopen_tbl_dplll[];
#include "data/dopen_tbl_dpslc.h" // static float dopen_tbl_dpslc[];
#include "data/dopen_tbl_dpln_dbl0.h" // static float dopen_tbl_dpln_dbl0[];
#include "data/dopen_tbl_dpln_dbl1.h" // static float dopen_tbl_dpln_dbl1[];
#include "data/dopen_tbl_dplc_dbl0.h" // static float dopen_tbl_dplc_dbl0[];
#include "data/dopen_tbl_dplc_dbl1.h" // static float dopen_tbl_dplc_dbl1[];
#include "data/dopen_tbl_dpsc_dbl0.h" // static float dopen_tbl_dpsc_dbl0[];
#include "data/dopen_tbl_dpsc_dbl1.h" // static float dopen_tbl_dpsc_dbl1[];
#include "data/dopen_tbl_frc.h" // static float dopen_tbl_frc[];
#include "data/dopen_tbl_frr.h" // static float dopen_tbl_frr[];
#include "data/dopen_tbl_flc.h" // static float dopen_tbl_flc[];
#include "data/dopen_tbl_flr.h" // static float dopen_tbl_flr[];
#include "data/dopen_tbl_f_dbl0.h" // static float dopen_tbl_f_dbl0[];
#include "data/dopen_tbl_f_dbl1.h" // static float dopen_tbl_f_dbl1[];
#include "data/dclose68_tbl.h" // static float dclose68_tbl[];
#include "data/dclose69_tbl.h" // static float dclose69_tbl[];
#include "data/dclose70_tbl.h" // static float dclose70_tbl[];
#include "data/dclose71_tbl.h" // static float dclose71_tbl[];
#include "data/dclose72r_tbl.h" // static float dclose72r_tbl[];
#include "data/dclose72l_tbl.h" // static float dclose72l_tbl[];
#include "data/dclose73r_tbl.h" // static float dclose73r_tbl[];
#include "data/dclose73l_tbl.h" // static float dclose73l_tbl[];
#include "data/dclose74r_tbl.h" // static float dclose74r_tbl[];
#include "data/dclose74l_tbl.h" // static float dclose74l_tbl[];
#include "data/dopen_tbl_drot.h" // static float dopen_tbl_drot[];
#include "data/dopen_tbl_dpsk.h" // static float dopen_tbl_dpsk[];
#include "data/dopen_tbl_dplk.h" // static float dopen_tbl_dplk[];
float *dtbl_tbl[] = {
    dopen_tbl_dplrn, dopen_tbl_dplrc, dopen_tbl_dplrl, dopen_tbl_dpsrc,
    dopen_tbl_dplln, dopen_tbl_dpllc, dopen_tbl_dplll, dopen_tbl_dpslc, dopen_tbl_dpln_dbl0,
    dopen_tbl_dpln_dbl1, dopen_tbl_dplc_dbl0, dopen_tbl_dplc_dbl1,
    dopen_tbl_dpsc_dbl0, dopen_tbl_dpsc_dbl1, dopen_tbl_frc,
    dopen_tbl_frr, dopen_tbl_flc, dopen_tbl_flr, dopen_tbl_f_dbl0, dopen_tbl_f_dbl1,
    dclose68_tbl, dclose69_tbl, dclose70_tbl, dclose71_tbl, dclose72r_tbl,
    dclose72l_tbl, dclose73r_tbl, dclose73l_tbl, dclose74r_tbl, dclose74l_tbl, dopen_tbl_drot,
    dopen_tbl_dpsk, dopen_tbl_dplk,
};
#include "data/door_mtn_dat.h" // DOOR_MTN_DAT door_mtn_dat[];
#include "data/door_move_dat.h" // DOOR_MOVE_DAT door_move_dat[];

DOOR_MTN_DAT* GetDoorMtnDat(u_char dtype)
{
    return &door_mtn_dat[dtype];
}

DOOR_TYPE_DAT* GetDoorTypeDatP(int no)
{
    return &door_type_dat[no];
}

DOOR_TYPE_DAT GetDoorTypeDat(int no)
{
    return door_type_dat[no];
}

u_short GetDoorMotionTblPos(u_char door_motion_dat_no)
{
    return door_move_dat[door_motion_dat_no].st_tbl_pos;
}

int IsFusumaType(u_char type)
{
    DOOR_TYPE_DAT *dtd;

    dtd = GetDoorTypeDatP(type);

    if (dtd->attribute & 0x1000)
    {
        return 1;
    }

    return 0;
}

int IsRotType(u_char type)
{
    return GetDoorTypeDatP(type)->room_id != 0xff;
}

int IsFusumaAttr(u_short attr)
{
    if (attr & 0x1000)
    {
        return 1;
    }

    return 0;
}

int MotionIsDouble(u_char motion_no)
{
    switch(motion_no)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 11:
    case 12:
    case 13:
    case 14:
    case 16:
        return 0;
    break;
    case 8:
    case 9:
    case 10:
    case 15:
        return 1;
    break;
    }

    return 0;
}
