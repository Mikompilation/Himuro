
#include "common.h"
#include "typedefs.h"
#include "mc.h"

#include "main/glob.h"
#include "ingame/entry/ap_pgost.h"
#include "ingame/entry/entry.h"
#include "ingame/event/ev_load.h"
#include "ingame/event/ev_main.h"
#include "ingame/event/wan_soul.h"
#include "ingame/ig_glob.h"
#include "ingame/map/find_ctl.h"
#include "ingame/map/furn_spe/furn_spe.h"
#include "ingame/map/item_ctl.h"
#include "ingame/map/map_area.h"
#include "ingame/map/map_ctrl.h"
#include "ingame/menu/ig_glst.h"
#include "ingame/menu/ig_rank.h"
#include "outgame/btl_mode/btl_mode.h"
#include "outgame/memory_album.h"
#include "outgame/mode_slct.h"
#include "mc/mc_main.h"

MC_DATA_STR mc_gamedata_str[] = {
    { .addr = (char *)&mc_header,       .size = sizeof(mc_header)       },
    { .addr = (char *)&opt_wrk,         .size = sizeof(opt_wrk)         },
    { .addr = (char *)&ingame_wrk,      .size = sizeof(ingame_wrk)      },
    { .addr = (char *)load_dat_wrk,     .size = sizeof(load_dat_wrk)    },
    { .addr = (char *)&time_wrk,        .size = sizeof(time_wrk)        },
    { .addr = (char *)&plyr_wrk,        .size = sizeof(plyr_wrk)        },
    { .addr = (char *)ene_wrk,          .size = sizeof(ene_wrk)         },
    { .addr = (char *)&map_wrk,         .size = sizeof(map_wrk)         },
    { .addr = (char *)&ap_wrk,          .size = sizeof(ap_wrk)          },
    { .addr = (char *)&ev_wrk,          .size = sizeof(ev_wrk)          },
    { .addr = (char *)event_stts,       .size = sizeof(event_stts)      },
    { .addr = (char *)find_stts,        .size = sizeof(find_stts)       },
    { .addr = (char *)poss_item,        .size = sizeof(poss_item)       },
    { .addr = (char *)poss_file,        .size = sizeof(poss_file)       },
    { .addr = (char *)flm_exp_flg,      .size = sizeof(flm_exp_flg)     },
    { .addr = (char *)item_ap,          .size = sizeof(item_ap)         },
    { .addr = (char *)&area_wrk,        .size = sizeof(area_wrk)        },
    { .addr = (char *)furn_attr_flg,    .size = sizeof(furn_attr_flg)   },
    { .addr = (char *)door_keep,        .size = sizeof(door_keep)       },
    { .addr = (char *)room_pass,        .size = sizeof(room_pass)       },
    { .addr = (char *)&pg_wrk,          .size = sizeof(pg_wrk)          },
    { .addr = (char *)&cam_custom_wrk,  .size = sizeof(cam_custom_wrk)  },
    { .addr = (char *)f_dat_save,       .size = sizeof(f_dat_save)      },
    { .addr = (char *)wander_soul_wrk,  .size = sizeof(wander_soul_wrk) },
    { .addr = (char *)glist_index,      .size = sizeof(glist_index)     },
    { .addr = (char *)&cribo,           .size = sizeof(cribo)           },
    { .addr = (char *)stage_wrk,        .size = sizeof(stage_wrk)       },
    { .addr = (char *)&save_rank,       .size = sizeof(save_rank)       },
    { .addr = (char *)0x01a9f000,       .size = 0x6400                  },
    { .addr = (char *)0x01be2380,       .size = 0x84120                 },
};
MC_DATA_STR mc_albumdata_str[] = {
    { .addr = (char *)&mc_header,     .size = sizeof(mc_header)     },
    { .addr = (char *)&mc_album_save, .size = sizeof(mc_album_save) },
    { .addr = (char *)0x005a0000,     .size = 0xF000                },
    { .addr = (char *)0x005b5400,     .size = 0x13CF80              },
};
MC_DATA_STR mc_albumdata2_str[] = {
    { .addr = (char *)&mc_header,     .size = sizeof(mc_header)     },
    { .addr = (char *)&mc_album_save, .size = sizeof(mc_album_save) },
    { .addr = (char *)0x01a90000,     .size = 0xF000                },
    { .addr = (char *)0x01aa5400,     .size = 0x13CF80              },
};
u_long mc_gamedata_str_num = 30;
u_long mc_albumdata_str_num = 4;
