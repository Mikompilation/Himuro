#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "sd_room.h"

#include "main/glob.h"

#define MAX_VOL 0x2fff
#define RM_DOOR_SIZE 42

typedef struct {
  u_int fno[3];
} ROOM_DOOR_SE;

typedef struct {
  u_int fno[5];
} ROOM_FOOT_SE;

#include "data/adpcm_param_tbl.h" // ADPCM_ROOM_PLAY adpcm_param_tbl[] = { ... };
#include "data/foot_se_index.h" // int foot_se_index[] = { ... };
#include "data/rm_snd_info.h" // static ROOM_SOUND_INFO rm_snd_info[] = { ... };
#include "data/rm_door.h" // static ROOM_DOOR_SE rm_door[] = { ... };
#include "data/rm_foot.h" // static ROOM_FOOT_SE rm_foot[] = { ... };
#include "data/rm_srund.h" // static u_int rm_srund[] = { ... };
#include "data/rm_event.h" // static u_int rm_event[] = { ... };

ROOM_SOUND_INFO* GetSdrDatP(u_char room_id)
{
  return &rm_snd_info[room_id];
}

int GetSdrAdpcmFno(u_char room_id)
{
    return adpcm_param_tbl[rm_snd_info[room_id].adpcm[ingame_wrk.msn_no]].file_no;
}

u_short GetSdrAdpcmVol(u_char room_id)
{
    return adpcm_param_tbl[rm_snd_info[room_id].adpcm[ingame_wrk.msn_no]].vol;
}

u_short GetSdrReverbVol(u_char room_id)
{
    return rm_snd_info[room_id].rvol;
}

void SetRoomReverbVol(u_char room_id)
{
    u_short vol;
  
    if (room_id >= RM_DOOR_SIZE) 
    {
        vol = MAX_VOL;
        
    }
    else 
    {
        vol = GetSdrReverbVol(room_id);
    }
    
    SetReverbVolume(vol);
}

u_int * GetSdrDoorSeTblP(u_char room_id)
{
    u_char door_no;
  
    door_no = rm_snd_info[room_id].door_no;

    if (ingame_wrk.msn_no != 1 && room_id == 13)
    {
        door_no = RM_DOOR_SIZE;
    }
    
    return rm_door[door_no].fno;
}

u_int * GetSdrFootSeTblP(u_char room_id)
{
    return rm_foot[rm_snd_info[room_id].foot_no].fno;
}

u_int GetSdrSrundSe(u_char room_id)
{
    return rm_srund[rm_snd_info[room_id].srund_no];
}

u_int GetSdrEventSe(u_char room_id)
{
    return rm_event[rm_snd_info[room_id].event_no];
}
