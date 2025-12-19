#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "common.h"

#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"
#include "main/gamemain.h"
#include "main/glob.h"
#include "ingame/enemy/ene_ctl.h"
#include "ingame/entry/ap_fgost.h"
#include "ingame/entry/ap_pgost.h"
#include "ingame/entry/entry.h"
#include "ingame/event/ev_load.h" // SetDataLoadWrk
#include "ingame/event/ev_main.h"
#include "ingame/event/ev_load_types.h"
#include "ingame/event/wan_soul.h"
#include "ingame/ig_glob.h"
#include "ingame/map/find_ctl.h"
#include "ingame/map/furn_spe/furn_spe.h"
#include "ingame/map/item_ctl.h"
#include "ingame/map/map_area.h"
#include "ingame/map/map_ctrl.h"
#include "outgame/btl_mode/btl_dat.h"
#include "outgame/btl_mode/btl_menu.h"
#include "outgame/btl_mode/btl_mode.h"
#include "outgame/outgame.h"
#include "graphics/motion/mdlwork.h"
// #include "graphics/motion/motion.h" // motInitEnemyAnm, motInitEnemyMdl
#include "graphics/graph2d/effect_ene.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph3d/load3d.h"

#include "data/stage_load_dat0.h" // MSN_LOAD_DAT stage_load_dat0[];
#include "data/stage_load_dat1.h" // MSN_LOAD_DAT stage_load_dat1[];
#include "data/stage_load_dat2.h" // MSN_LOAD_DAT stage_load_dat2[];
#include "data/stage_load_dat3.h" // MSN_LOAD_DAT stage_load_dat3[];
#include "data/stage_load_dat4.h" // MSN_LOAD_DAT stage_load_dat4[];
MSN_LOAD_DAT *stage_load_dat[] = {
    stage_load_dat0, stage_load_dat1, stage_load_dat2,
    stage_load_dat3, stage_load_dat4,
};
#include "data/free_dat.h" // FREE_DAT free_dat[];
BTL_SAVE_STR btl_save_str[] = {
    { .addr = (u_char *)load_dat_wrk,    .size = sizeof(load_dat_wrk)    },
    { .addr = (u_char *)&time_wrk,       .size = sizeof(time_wrk)        },
    { .addr = (u_char *)&plyr_wrk,       .size = sizeof(plyr_wrk)        },
    { .addr = (u_char *)ene_wrk,         .size = sizeof(ene_wrk)         },
    { .addr = (u_char *)&map_wrk,        .size = sizeof(map_wrk)         },
    { .addr = (u_char *)&ap_wrk,         .size = sizeof(ap_wrk)          },
    { .addr = (u_char *)&ev_wrk,         .size = sizeof(ev_wrk)          },
    { .addr = (u_char *)event_stts,      .size = sizeof(event_stts)      },
    { .addr = (u_char *)find_stts,       .size = sizeof(find_stts)       },
    { .addr = (u_char *)poss_item,       .size = sizeof(poss_item)       },
    { .addr = (u_char *)poss_file,       .size = sizeof(poss_file)       },
    { .addr = (u_char *)flm_exp_flg,     .size = sizeof(flm_exp_flg)     },
    { .addr = (u_char *)item_ap,         .size = sizeof(item_ap)         },
    { .addr = (u_char *)&area_wrk,       .size = sizeof(area_wrk)        },
    { .addr = (u_char *)furn_attr_flg,   .size = sizeof(furn_attr_flg)   },
    { .addr = (u_char *)door_keep,       .size = sizeof(door_keep)       },
    { .addr = (u_char *)room_pass,       .size = sizeof(room_pass)       },
    { .addr = (u_char *)&pg_wrk,         .size = sizeof(pg_wrk)          },
    { .addr = (u_char *)f_dat_save,      .size = sizeof(f_dat_save)      },
    { .addr = (u_char *)wander_soul_wrk, .size = sizeof(wander_soul_wrk) },
};

#define ENE_DMG_TEX_BASE_ADDRESS 0x98000
#define FLY_MDL_ADDRESS 0xd80000
#define MSN00TTL_PK2_ADDRESS 0x1e90000
#define STORY_WORK_SAVE_ADDRESS 0x1090000

void FreeModeMain()
{
    char *str0 = "FREE MODE";
    char *str1 = "MSN NO";
    char *str2 = "ROOM NO";
    char *str3 = "START";
    char *csr0 = "o";

    SetASCIIString(60.0f, 50.0f, str0);
    SetASCIIString(120.0f, 110.0f, str1);
    SetASCIIString(120.0f, 150.0f, str2);
    SetASCIIString(120.0f, 190.0f, str3);
    SetASCIIString(90.0f, btl_wrk.csr[0] * 40 + 110, csr0);

    SetInteger2(0, 230.0f, 110.0f, 0, 0x80, 0x80, 0x80, btl_wrk.csr[1]);
    SetInteger2(0, 230.0f, 150.0f, 0, 0x80, 0x80, 0x80, btl_wrk.csr[2]);

    if (*key_now[5] == 1)
    {
        OutGameModeChange(4);

        return;
    }

    if (*key_now[7] == 1)
    {
        if (btl_wrk.csr[0] == 2)
        {
            ingame_wrk.game = 1;

            GameModeChange(0);

            return;
        }
    }
    else if (*key_now[0] == 1)
    {
        if (btl_wrk.csr[0] != 0)
        {
            btl_wrk.csr[0]--;
        }
    }
    else if (*key_now[1] == 1)
    {
        if (btl_wrk.csr[0] < 2)
        {
            btl_wrk.csr[0]++;
        }
    }
    else if (*key_now[2] == 1)
    {
        if (btl_wrk.csr[0] == 0)
        {
            if (btl_wrk.csr[1] != 0)
            {
                btl_wrk.csr[1]--;
            }
            else
            {
                btl_wrk.csr[1] = 4;
            }
        }

        if (btl_wrk.csr[0] == 1)
        {
            if (btl_wrk.csr[2] != 0)
            {
                btl_wrk.csr[2]--;
            }
            else
            {
                btl_wrk.csr[2] = 0x26;
            }
        }
    }
    else if (*key_now[3] == 1)
    {
        if (btl_wrk.csr[0] == 0)
        {
            if (btl_wrk.csr[1] < 4)
            {
                btl_wrk.csr[1]++;
            }
            else
            {
                btl_wrk.csr[1] = 0;
            }
        }

        if (btl_wrk.csr[0] == 1)
        {
            if (btl_wrk.csr[2] < 38)
            {
                btl_wrk.csr[2]++;
            }
            else
            {
                btl_wrk.csr[2] = 0;
            }
        }
    }
}

void FreeModePosSet()
{
    map_wrk.floor = free_dat[btl_wrk.csr[2]].floor;

    plyr_wrk.move_box.pos[0] = free_dat[btl_wrk.csr[2]].pos[0];
    plyr_wrk.move_box.pos[1] = free_dat[btl_wrk.csr[2]].pos[1];
    plyr_wrk.move_box.pos[2] = free_dat[btl_wrk.csr[2]].pos[2];
    plyr_wrk.move_box.pos[3] = free_dat[btl_wrk.csr[2]].pos[3];
}

void BattleModeInit()
{
    btl_wrk = (BTL_MODE_WRK){0};
}

void ClearStageWrk()
{
    int i;

    memset(&stage_wrk, 0, sizeof(stage_wrk));

    for (i = 0; i <= 19; i++)
    {
        stage_wrk[i].best_time = 0x8c64;
    }
}

void BattleModeMain()
{
    char *str0 = "BATTLE MODE";
    char *str1 = "STAGE 1";
    char *str2 = "STAGE 2";
    char *str3 = "STAGE 3";
    char *str4 = "STAGE 4";
    char *str7 = "STAGE 5";
    char *str5 = "TIME";
    char *str6 = "SCORE";
    char *csr0 = "o";

    SetASCIIString(30.0f, 50.0f, str0);
    SetASCIIString(70.0f, 150.0f, str1);
    SetASCIIString(70.0f, 190.0f, str2);
    SetASCIIString(70.0f, 230.0f, str3);
    SetASCIIString(70.0f, 270.0f, str4);
    SetASCIIString(70.0f, 310.0f, str7);
    SetASCIIString(270.0f, 110.0f, str5);
    SetASCIIString(410.0f, 110.0f, str6);

    SetASCIIString(40.0f, btl_wrk.stage_no * 0x28 + 0x96, csr0);

    SetInteger2(0, 212.0f, 150.0f, 0, 0x80, 0x80, 0x80, stage_wrk[0].best_time / 60);
    SetInteger2(0, 240.0f, 154.0f, 0, 0x80, 0x80, 0x80, (stage_wrk[0].best_time % 60) / 6);
    SetInteger2(0, 212.0f, 190.0f, 0, 0x80, 0x80, 0x80, stage_wrk[1].best_time / 60);
    SetInteger2(0, 240.0f, 194.0f, 0, 0x80, 0x80, 0x80, (stage_wrk[1].best_time % 60) / 6);
    SetInteger2(0, 212.0f, 230.0f, 0, 0x80, 0x80, 0x80, stage_wrk[2].best_time / 60);
    SetInteger2(0, 240.0f, 234.0f, 0, 0x80, 0x80, 0x80, (stage_wrk[2].best_time % 60) / 6);
    SetInteger2(0, 212.0f, 270.0f, 0, 0x80, 0x80, 0x80, stage_wrk[3].best_time / 60);
    SetInteger2(0, 240.0f, 274.0f, 0, 0x80, 0x80, 0x80, (stage_wrk[3].best_time % 60) / 6);
    SetInteger2(0, 212.0f, 310.0f, 0, 0x80, 0x80, 0x80, stage_wrk[4].best_time / 60);
    SetInteger2(0, 240.0f, 314.0f, 0, 0x80, 0x80, 0x80, (stage_wrk[4].best_time % 60) / 6);

    SetInteger2(0, 400.0f, 150.0f, 0, 0x80, 0x80, 0x80, stage_wrk[0].best_shot);
    SetInteger2(0, 400.0f, 190.0f, 0, 0x80, 0x80, 0x80, stage_wrk[1].best_shot);
    SetInteger2(0, 400.0f, 230.0f, 0, 0x80, 0x80, 0x80, stage_wrk[2].best_shot);
    SetInteger2(0, 400.0f, 270.0f, 0, 0x80, 0x80, 0x80, stage_wrk[3].best_shot);
    SetInteger2(0, 400.0f, 310.0f, 0, 0x80, 0x80, 0x80, stage_wrk[4].best_shot);

    SetInteger2(0, 470.0f, 150.0f, 0, 0x80, 0x80, 0x80, stage_wrk[0].rank);
    SetInteger2(0, 470.0f, 190.0f, 0, 0x80, 0x80, 0x80, stage_wrk[1].rank);
    SetInteger2(0, 470.0f, 230.0f, 0, 0x80, 0x80, 0x80, stage_wrk[2].rank);
    SetInteger2(0, 470.0f, 270.0f, 0, 0x80, 0x80, 0x80, stage_wrk[3].rank);
    SetInteger2(0, 470.0f, 310.0f, 0, 0x80, 0x80, 0x80, stage_wrk[4].rank);

    if (*key_now[5] == 1)
    {
        OutGameModeChange(4);
    }
    else if (*key_now[7] == 1)
    {
        ingame_wrk.game = 1;

        GameModeChange(0);
    }
    else if (*key_now[0] == 1)
    {
        if (btl_wrk.stage_no != 0)
        {
            btl_wrk.stage_no--;
        }
    }
    else if (*key_now[1] == 1)
    {
        if (btl_wrk.stage_no < 4)
        {
            btl_wrk.stage_no++;
        }
    }
}

int StageTitleInit()
{
    int ret;

    btl_wrk.load_mode = 0;

#ifdef BUILD_EU_VERSION
    ret = LoadReqLanguage((btl_wrk.stage_no % 5) * 5 + MSN00TTL_E_PK2, MSN00TTL_PK2_ADDRESS);
#else
    ret = LoadReq(btl_wrk.stage_no % 5 + MSN00TTL_PK2, MSN00TTL_PK2_ADDRESS);
#endif

    map_wrk.floor = stage_dat[btl_wrk.stage_no].floor;

    plyr_wrk.move_box.rot[1] = SetRot360(stage_dat[btl_wrk.stage_no].prot);

    plyr_wrk.move_box.pos[0] = stage_dat[btl_wrk.stage_no].ppos[0];
    plyr_wrk.move_box.pos[1] = stage_dat[btl_wrk.stage_no].ppos[1];
    plyr_wrk.move_box.pos[2] = stage_dat[btl_wrk.stage_no].ppos[2];

    ingame_wrk.msn_no = stage_dat[btl_wrk.stage_no].map_no;

    return ret;
}

int StageTitleLoad()
{
    int map_no;

    map_no = stage_dat[btl_wrk.stage_no].map_no;

    if (btl_wrk.load_mode == 7)
    {
        return 1;
    }

    if (btl_wrk.load_mode == 0)
    {
        btl_wrk.load_mode = 1;

        ReqMsnInitPlyr(1);
    }
    else if (btl_wrk.load_mode == 1)
    {
        if (MsnInitPlyr() != 0)
        {
            btl_wrk.load_mode = 2;
        }
    }
    else if (stage_load_dat[map_no][btl_wrk.load_count].file_no == 0xffff)
    {
        StageGhostLoadReq();

        btl_wrk.load_count = 0;
        btl_wrk.load_mode = 6;
    }
    else if (btl_wrk.load_mode == 3)
    {
        if (IsLoadEnd(btl_wrk.load_id) != 0)
        {
            MissionDataLoadAfterInit(&stage_load_dat[map_no][btl_wrk.load_count]);
            SetDataLoadWrk(&stage_load_dat[map_no][btl_wrk.load_count]);

            btl_wrk.load_count++;
            btl_wrk.load_mode = 2;
        }
    }
    else if (btl_wrk.load_mode == 2)
    {
        btl_wrk.load_id = MissionDataLoadReq(&stage_load_dat[map_no][btl_wrk.load_count]);
        btl_wrk.load_mode = 3;
    }
    else if (btl_wrk.load_mode == 6)
    {
        if (IsLoadEndAll() != 0)
        {
            StageGhostLoadAfter();

            btl_wrk.load_mode = 4;
        }
    }
    else if (btl_wrk.load_mode == 4)
    {
        if (btl_wrk.load_count != 0)
        {
            btl_wrk.load_mode = 7;

            return 1;
        }

        RoomMdlLoadReq(NULL, btl_wrk.load_count, map_no, stage_dat[btl_wrk.stage_no].room, 1);

        area_wrk.room[btl_wrk.load_count] = stage_dat[btl_wrk.stage_no].room;

        btl_wrk.load_mode = 5;
    }
    else if (btl_wrk.load_mode == 5)
    {
        if (RoomMdlLoadWait() != 0)
        {
            btl_wrk.load_count++;
            btl_wrk.load_mode = 4;
        }
    }

    return 0;
}

void StageGhostLoadReq()
{
    int i;
    int ene_no;
    ENE_DAT *edp;

    edp = jene_dat[stage_dat[btl_wrk.stage_no].map_no];

    for (i = 0; i < 3; i++)
    {
        ene_no = stage_dat[btl_wrk.stage_no].ene_no[i];

        if (ene_no == 0xff)
        {
            break;
        }

        LoadReq(edp[ene_no].mdl_no + M000_MIKU_MDL, load_mdl_addr[i]);
        LoadEneDmgTex(edp[ene_no].mdl_no, (u_int *)(load_mot_addr[i] + ENE_DMG_TEX_BASE_ADDRESS));

        LoadReq(edp[ene_no].anm_no + M000_MIKU_ANM, load_mot_addr[i]);

        if (stage_dat[btl_wrk.stage_no].fly_mdl_no != 0xffff)
        {
            SeFileLoadAndSet(SGY001_BD, 1);
        }

        SeFileLoadAndSet(edp[ene_no].se_no, load_se_addr[i]);
    }

    if (stage_dat[btl_wrk.stage_no].fly_mdl_no != 0xffff)
    {
        LoadReq(stage_dat[btl_wrk.stage_no].fly_mdl_no + M000_MIKU_MDL, FLY_MDL_ADDRESS);
    }
}

void StageGhostLoadAfter()
{
    int i;
    int ene_no;
    ENE_DAT *edp;

    edp = jene_dat[stage_dat[btl_wrk.stage_no].map_no];

    for (i = 0; i < 3; i++)
    {
        ene_no = stage_dat[btl_wrk.stage_no].ene_no[i];

        if (ene_no == 0xff)
        {
            break;
        }

        motInitEnemyMdl(load_mdl_addr[i], edp[ene_no].mdl_no);
        motInitEnemyAnm(load_mot_addr[i], edp[ene_no].mdl_no, edp[ene_no].anm_no);
    }

    if (stage_dat[btl_wrk.stage_no].fly_mdl_no != 0xffff)
    {
        motInitEnemyMdl((u_int *)FLY_MDL_ADDRESS, stage_dat[btl_wrk.stage_no].fly_mdl_no);
    }
}

u_long btl_save_str_num = 20;

void SaveStoryWrk()
{
    u_char *srcp;
    u_char *dstp;
    int i;
    int j;

    dstp = (u_char *)STORY_WORK_SAVE_ADDRESS;

    for (i = 0; i < btl_save_str_num; i++)
    {
        srcp = btl_save_str[i].addr;

        for (j = 0; j < btl_save_str[i].size; j++)
        {
            *dstp++ = *srcp++;
        }
    }

    *dstp++ = ingame_wrk.init_flg;
    *dstp++ = ingame_wrk.stts;
    *dstp++ = ingame_wrk.msn_no;
    *dstp++ = ingame_wrk.difficult;
}

void LoadStoryWrk()
{
    u_char *srcp;
    u_char *dstp;
    int i;
    int j;

    srcp = (u_char *)STORY_WORK_SAVE_ADDRESS;

    for (i = 0; i < btl_save_str_num; i++)
    {
        dstp = btl_save_str[i].addr;

        for (j = 0; j < btl_save_str[i].size; j++)
        {
            *dstp++ = *srcp++;
        }
    }
    ingame_wrk.init_flg = *srcp++;
    ingame_wrk.stts = *srcp++;
    ingame_wrk.msn_no = *srcp++;
    ingame_wrk.difficult = *srcp++;
}
