#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ap_zgost.h"

#include "os/eeiop/adpcm/ea_magatoki.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "common/ul_math.h"
#include "main/glob.h"
// #include "ingame/entry/ap_fgost.h" // FloatGhostAppearTypeSet
#include "ingame/entry/entry.h"
#include "ingame/event/ev_load.h"
#include "ingame/ig_glob.h"
#include "ingame/map/door_ctl.h"
#include "ingame/map/furn_ctl.h"
#include "ingame/map/map_area.h"
#include "ingame/map/map_ctrl.h"
#include "ingame/plyr/plyr_ctl.h"
#include "graphics/motion/mdlwork.h"
#include "graphics/motion/motion.h"
// #include "graphics/graph2d/effect_ene.h" // LoadEneDmgTex
#include "graphics/graph2d/effect.h"
#include "graphics/graph3d/load3d.h"

ZHOUR_WRK zh_wrk = {0};

#define ANIM_MODEL_TYPE_8_ADDRESS_0 0xc80000
#define ANIM_MODEL_TYPE_8_ADDRESS_1 0xd00000

#define ANIM_MODEL_TYPE_9_ADDRESS_0 0xa30000
#define ENE_DMG_TEX_TYPE_9_ADDRESS_0 0xac8000

#define ANIM_MODEL_TYPE_9_ADDRESS_1 0xae0000
#define ENE_DMG_TEX_TYPE_9_ADDRESS_1 0xb78000

#ifdef BUILD_EU_VERSION
#define TYPE_8_MDL_FILE_NO_OFFSET 47
#define TYPE_9_MDL_FILE_NO_OFFSET 115
#else
#define TYPE_8_MDL_FILE_NO_OFFSET 31
#define TYPE_9_MDL_FILE_NO_OFFSET 99
#endif

int ZeroHourAppearMain()
{
    sceVu0FVECTOR tv;
    u_char no_tbl[5] = { 12, 12, 20, 6, 3 };

    if (ingame_wrk.msn_no == 0 || ingame_wrk.msn_no == 1)
    {
        return; // missing return value
    }

    switch(ap_wrk.zh_mode)
    {
    case ZH_NO_REQ:
        if (ZeroHourAppearJudge() != 0)
        {
            ap_wrk.zh_efct = 0;

            SetEffects(16, 8, 180, 60, &ap_wrk.zh_efct);

            ZeroHourDataBackUp();

            LoadReq(M040_MAGATOKI_MDL, ANIM_MODEL_TYPE_8_ADDRESS_0);
            LoadReq(M040_MAGATOKI_ANM, ANIM_MODEL_TYPE_9_ADDRESS_0);

            LoadEneDmgTex(40, (u_int *)ENE_DMG_TEX_TYPE_9_ADDRESS_0);

            if (plyr_wrk.mode == 1)
            {
                FinderEndSet();
            }

            AdpcmPreMagatokiFadeOut(40);

            ingame_wrk.stts |= 0x80;

            ap_wrk.zh_mode = ZH_READY;
        }
    break;
    case ZH_READY:
        if (IsLoadEndAll() != 0)
        {
            motInitEnemyMdl((u_int *)ANIM_MODEL_TYPE_8_ADDRESS_0, 40);
            motInitEnemyAnm((u_int *)ANIM_MODEL_TYPE_9_ADDRESS_0, 40, 28);

            ap_wrk.zh_mode = ZH_READY2;
        }
    break;
    case ZH_READY2:
        if (IsFadeoutEndAdpcmMagatoki() != 0)
        {
            ap_wrk.zh_efct = 1;
            ap_wrk.zh_mode = ZH_START;
        }
    break;
    case ZH_START:
        if (ap_wrk.zh_efct == 2)
        {
            ene_wrk[0].dat_no = no_tbl[ingame_wrk.msn_no];
            ene_wrk[0].sta = 0x80000000 | 0x2;

            tv[0] = 0.0f;
            tv[1] = 0.0f;
            tv[2] = 3000.0f;
            tv[3] = 0.0f;

            RotFvector(plyr_wrk.move_box.rot, tv);

            sceVu0AddVector(ene_wrk[0].move_box.pos, plyr_wrk.move_box.pos, tv);
            AdpcmMagatokiPlay(AM000_MAGA_STR);

            ingame_wrk.stts &= 0x40 | 0x20 | 0x10 | 0x8 | 0x4 | 0x2 | 0x1;

            ap_wrk.zh_mode = ZH_NOW;

            return 1;
        }
    break;
    case ZH_NOW:
        // do nothing ...
    break;
    case ZH_OUT_REQ:
        ap_wrk.zh_efct = 3;

        zh_wrk.time = 180;

        ap_wrk.zh_mode = ZH_OUT_WAIT;
    break;
    case ZH_OUT:
        // do nothing ...
    break;
    case ZH_OUT_WAIT:
        if (--zh_wrk.time == 0xffff)
        {
            ap_wrk.zh_mode = ZH_OUT_LOAD0;
        }
    break;
    case ZH_OUT_LOAD0:
        ZeroHourEnemyReLoad();

        zh_wrk.time = 16;

        ap_wrk.zh_mode = ZH_OUT_LOAD1;
    break;
    case ZH_OUT_LOAD1:
        if (zh_wrk.time != 0)
        {
            zh_wrk.time--;

            if (zh_wrk.time == 0)
            {
                ingame_wrk.stts |= 0x20;
            }
        }
        else
        {
            if (IsLoadEndAll() != 0)
            {
                ZeroHourAfterRoomLoadReq();
                AdpcmMagatokiStop();

                ap_wrk.zh_mode = ZH_RETURN;
            }
        }
    break;
    case ZH_RETURN:
        if (RoomMdlLoadWait() != 0)
        {
            ap_wrk.zh_efct = 4;

            ZeroHourEnemyReLoadAfter();
            ZeroHourAfterPosReset();

            ingame_wrk.stts &= 0x80 | 0x40 | 0x10 | 0x8 | 0x4 | 0x2 | 0x1;

            ap_wrk.zh_mode = ZH_END;
        }
    break;
    case ZH_END:
        if (ap_wrk.zh_efct == 5)
        {
            ingame_wrk.stts &= 0x40 | 0x20 | 0x10 | 0x8 | 0x4 | 0x2 | 0x1;

            if (ap_wrk.stts & 0x4)
            {
                ap_wrk.stts &= 0x80 | 0x40 | 0x20 | 0x10 | 0x8 | 0x2 | 0x1;
            }

            ap_wrk.zh_mode = ZH_NO_REQ;
        }
    break;
    }

    return 0;
}

void ZeroHourDataBackUp()
{
    int i;
    MSN_LOAD_DAT *mld;

    for (i = 0; i < 2; i++)
    {
        zh_wrk.zh_mdl_bak[i] = 0xff;
        zh_wrk.zh_mot_bak[i] = 0xff;
    }

    for (i = 0; i < 40; i++)
    {
        mld = &load_dat_wrk[i];

        if (load_dat_wrk[i].file_no != 0xffff)
        {
            if (mld->file_type == 8)
            {
                if (mld->addr == ANIM_MODEL_TYPE_8_ADDRESS_0)
                {
                    zh_wrk.zh_mdl_bak[0] = mld->file_no - TYPE_8_MDL_FILE_NO_OFFSET;
                }
                else if (mld->addr == ANIM_MODEL_TYPE_8_ADDRESS_1)
                {
                    zh_wrk.zh_mdl_bak[1] = mld->file_no - TYPE_8_MDL_FILE_NO_OFFSET;
                }
            }
            else if (mld->file_type == 9)
            {
                if (mld->addr == ANIM_MODEL_TYPE_9_ADDRESS_0)
                {
                    zh_wrk.zh_mmt_bak[0] = mld->tmp_no;
                    zh_wrk.zh_mot_bak[0] = mld->file_no - TYPE_9_MDL_FILE_NO_OFFSET;

                    motReleaseAniMdlBuf(zh_wrk.zh_mot_bak[0], (u_int *)ANIM_MODEL_TYPE_9_ADDRESS_0);
                }
                else if (mld->addr == ANIM_MODEL_TYPE_9_ADDRESS_1)
                {
                    zh_wrk.zh_mmt_bak[1] = mld->tmp_no;
                    zh_wrk.zh_mot_bak[1] = mld->file_no - TYPE_9_MDL_FILE_NO_OFFSET;

                    motReleaseAniMdlBuf(zh_wrk.zh_mot_bak[1], (u_int *)ANIM_MODEL_TYPE_9_ADDRESS_1);
                }
            }
        }
    }

    zh_wrk.pos[0] = plyr_wrk.move_box.pos[0];
    zh_wrk.pos[1] = plyr_wrk.move_box.pos[1];
    zh_wrk.pos[2] = plyr_wrk.move_box.pos[2];
    zh_wrk.pos[3] = plyr_wrk.move_box.pos[3];

    zh_wrk.rot[0] = plyr_wrk.move_box.rot[0];
    zh_wrk.rot[1] = plyr_wrk.move_box.rot[1];
    zh_wrk.rot[2] = plyr_wrk.move_box.rot[2];
    zh_wrk.rot[3] = plyr_wrk.move_box.rot[3];

    zh_wrk.room = plyr_wrk.pr_info.room_no;
    zh_wrk.floor = map_wrk.floor;

    ZeroHourEneDmgTexDel((u_int)zh_wrk.zh_mdl_bak[0]);
    ZeroHourEneDmgTexDel((u_int)zh_wrk.zh_mdl_bak[1]);
}

void ZeroHourEnemyReLoad()
{
    if (zh_wrk.zh_mdl_bak[0] != 0xff)
    {
        LoadReq(zh_wrk.zh_mdl_bak[0] + M000_MIKU_MDL, ANIM_MODEL_TYPE_8_ADDRESS_0);
    }

    if (zh_wrk.zh_mdl_bak[1] != 0xff)
    {
        LoadReq(zh_wrk.zh_mdl_bak[1] + M000_MIKU_MDL, ANIM_MODEL_TYPE_8_ADDRESS_1);
    }

    if (zh_wrk.zh_mot_bak[0] != 0xff)
    {
        LoadReq(zh_wrk.zh_mot_bak[0] + M000_MIKU_ANM, ANIM_MODEL_TYPE_9_ADDRESS_0);
        LoadEneDmgTex((u_int)zh_wrk.zh_mdl_bak[0], (u_int *)ENE_DMG_TEX_TYPE_9_ADDRESS_0);
    }

    if (zh_wrk.zh_mot_bak[1] != 0xff)
    {
        LoadReq(zh_wrk.zh_mot_bak[1] + M000_MIKU_ANM, ANIM_MODEL_TYPE_9_ADDRESS_1);
        LoadEneDmgTex((u_int)zh_wrk.zh_mdl_bak[1], (u_int *)ENE_DMG_TEX_TYPE_9_ADDRESS_1);
    }
}

void ZeroHourEnemyReLoadAfter()
{
    if (zh_wrk.zh_mdl_bak[0] != 0xff)
    {
        motInitEnemyMdl((u_int *)ANIM_MODEL_TYPE_8_ADDRESS_0, zh_wrk.zh_mdl_bak[0]);
    }

    if (zh_wrk.zh_mdl_bak[1] != 0xff)
    {
        motInitEnemyMdl((u_int *)ANIM_MODEL_TYPE_8_ADDRESS_1, zh_wrk.zh_mdl_bak[1]);
    }

    if (zh_wrk.zh_mot_bak[0] != 0xff)
    {
        motInitEnemyAnm((u_int *)ANIM_MODEL_TYPE_9_ADDRESS_0, zh_wrk.zh_mmt_bak[0], zh_wrk.zh_mot_bak[0]);
    }

    if (zh_wrk.zh_mot_bak[1] != 0xff)
    {
        motInitEnemyAnm((u_int *)ANIM_MODEL_TYPE_9_ADDRESS_1, zh_wrk.zh_mmt_bak[1], zh_wrk.zh_mot_bak[1]);
    }
}

void ZeroHourAfterPosReset()
{
    plyr_wrk.move_box.pos[0] = zh_wrk.pos[0];
    plyr_wrk.move_box.pos[1] = zh_wrk.pos[1];
    plyr_wrk.move_box.pos[2] = zh_wrk.pos[2];
    plyr_wrk.move_box.pos[3] = zh_wrk.pos[3];

    plyr_wrk.move_box.rot[0] = zh_wrk.rot[0];
    plyr_wrk.move_box.rot[1] = zh_wrk.rot[1];
    plyr_wrk.move_box.rot[2] = zh_wrk.rot[2];
    plyr_wrk.move_box.rot[3] = zh_wrk.rot[3];

    if (zh_wrk.floor != map_wrk.floor)
    {
        MapFloorChange(zh_wrk.floor);
    }
}

void ZeroHourOutReq()
{
    motReleaseAniMdlBuf(28, (u_int *)ANIM_MODEL_TYPE_9_ADDRESS_0);

    ap_wrk.zh_efct = 0;

    FinderEndSet();

    ap_wrk.zh_mode = ZH_OUT_REQ;

    ingame_wrk.stts |= 0x80;
}

void ZeroHourAfterRoomLoadReq()
{
    int i;
    int load_room;

    load_room = zh_wrk.room;

    for (i = 0; i < 2; i++)
    {
        if (load_room == area_wrk.room[i])
        {
            return;
        }
    }

    for (i = 0; i < 60; i++)
    {
        if (furn_wrk[i].room_id != room_wrk.disp_no[0])
        {
            if (furn_wrk[i].use == 0 || furn_wrk[i].use == 2)
            {
                FurnSetWrkNoUse(&furn_wrk[i], i);
            }
        }
    }

    DoorFreeFurnWrk(0);
    FurnSortFurnWrk(0);

    for (i = 0; i < 2; i++)
    {
        if (plyr_wrk.pr_info.room_no != area_wrk.room[i])
        {
            RoomMdlLoadReq(NULL, i, ingame_wrk.msn_no, load_room, 1);
            FloatGhostAppearTypeSet(ap_wrk.fgst_no, i, load_room);

            area_wrk.room[i] = load_room;

            break;
        }
    }
}

int ZeroHourAppearJudge()
{
    return 0;
}
