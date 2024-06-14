#include "common.h"
#include "typedefs.h"
#include "ea_dat.h"

#include "data/adpcm_vol_tbl.h"  // ADPCM_VOL adpcm_vol_tbl[NO_VOL_TABLE] = {...};

// file_no receives values from CD_FILE_DAT
u_short GetAdpcmVol(int file_no)
{
    ADPCM_VOL *avp;
    int i;
    u_short vol;

    vol = MAX_VOLUME;

    for (i = 0, avp = adpcm_vol_tbl; i < NO_VOL_TABLE; i++, avp++)
    {
        if (avp->file_no == file_no)
        {
            vol = avp->vol;
            break;
        }        
    }

    if (vol > MAX_VOLUME) 
    {
        vol = MAX_VOLUME;
    }

    return vol;
}

