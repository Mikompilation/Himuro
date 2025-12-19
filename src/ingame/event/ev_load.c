#include "common.h"
#include "typedefs.h"
#include "enums.h"
#define INCLUDING_FROM_EV_LOAD_C
#include "ev_load.h"
#undef INCLUDING_FROM_EV_LOAD_C

#include "main/glob.h"
#include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_ctrl.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"
#include "ingame/ig_glob.h"
#include "ingame/entry/ap_dgost.h"
// #include "ingame/entry/ap_fgost.h" // FloatGhostAppearTypeSet
#include "ingame/entry/ap_ggost.h"
#include "ingame/entry/entry.h"
#include "ingame/event/ev_spcl.h" // SimpleDispSprt
#include "ingame/map/door_ctl.h"
#include "ingame/map/furn_ctl.h"
#include "ingame/map/furn_spe/fspe_acs.h"
#include "ingame/map/item_ctl.h"
#include "ingame/map/map_area.h"
#include "ingame/map/map_ctrl.h"
#include "outgame/btl_mode/btl_menu.h"
#include "graphics/motion/mdlwork.h"
#include "graphics/motion/motion.h"
#include "graphics/scene/scene.h"
#include "graphics/graph2d/effect_ene.h"
#include "graphics/graph3d/load3d.h"
#include "graphics/graph2d/tim2.h"

#include "data/msn0_title_load_dat.h" // MSN_LOAD_DAT msn0_title_load_dat[];
#include "data/scn0022_load.h" // MSN_LOAD_DAT scn0022_load[];
#include "data/scn0022_del_dat.h" // u_short scn0022_del_dat[];
#include "data/scn_load0.h" // SCN_LOAD_DAT scn_load0[];
#include "data/msn1_title_load_dat.h" // MSN_LOAD_DAT msn1_title_load_dat[];
#include "data/scn1010_load.h" // MSN_LOAD_DAT scn1010_load[];
#include "data/scn1010_del_dat.h" // u_short scn1010_del_dat[];
#include "data/scn1030_load.h" // MSN_LOAD_DAT scn1030_load[];
#include "data/scn1030_del_dat.h" // u_short scn1030_del_dat[];
#include "data/scn1034_load.h" // MSN_LOAD_DAT scn1034_load[];
#include "data/scn1034_del_dat.h" // u_short scn1034_del_dat[];
#include "data/scn1160_load.h" // MSN_LOAD_DAT scn1160_load[];
#include "data/scn1160_del_dat.h" // u_short scn1160_del_dat[];
#include "data/scn1070_load.h" // MSN_LOAD_DAT scn1070_load[];
#include "data/scn1070_del_dat.h" // u_short scn1070_del_dat[];
#include "data/scn1090_load.h" // MSN_LOAD_DAT scn1090_load[];
#include "data/scn1090_del_dat.h" // u_short scn1090_del_dat[];
#include "data/scn1100_load.h" // MSN_LOAD_DAT scn1100_load[];
#include "data/scn1100_del_dat.h" // u_short scn1100_del_dat[];
#include "data/scn1102_load.h" // MSN_LOAD_DAT scn1102_load[];
#include "data/scn1102_del_dat.h" // u_short scn1102_del_dat[];
#include "data/scn1120_load.h" // MSN_LOAD_DAT scn1120_load[];
#include "data/scn1120_del_dat.h" // u_short scn1120_del_dat[];
#include "data/scn1170_load.h" // MSN_LOAD_DAT scn1170_load[];
#include "data/scn1170_del_dat.h" // u_short scn1170_del_dat[];
#include "data/scn1150_load.h" // MSN_LOAD_DAT scn1150_load[];
#include "data/scn1150_del_dat.h" // u_short scn1150_del_dat[];
#include "data/scn1180_load.h" // MSN_LOAD_DAT scn1180_load[];
#include "data/scn1180_del_dat.h" // u_short scn1180_del_dat[];
#include "data/scn1230_load.h" // MSN_LOAD_DAT scn1230_load[];
#include "data/scn1230_del_dat.h" // u_short scn1230_del_dat[];
#include "data/scn1220_load.h" // MSN_LOAD_DAT scn1220_load[];
#include "data/scn1220_del_dat.h" // u_short scn1220_del_dat[];
#include "data/scn1260_load.h" // MSN_LOAD_DAT scn1260_load[];
#include "data/scn1260_del_dat.h" // u_short scn1260_del_dat[];
#include "data/scn1250_load.h" // MSN_LOAD_DAT scn1250_load[];
#include "data/scn1250_del_dat.h" // u_short scn1250_del_dat[];
#include "data/scn1280_load.h" // MSN_LOAD_DAT scn1280_load[];
#include "data/scn1280_del_dat.h" // u_short scn1280_del_dat[];
#include "data/scn1301_load.h" // MSN_LOAD_DAT scn1301_load[];
#include "data/scn1301_del_dat.h" // u_short scn1301_del_dat[];
#include "data/scn1320_load.h" // MSN_LOAD_DAT scn1320_load[];
#include "data/scn1320_del_dat.h" // u_short scn1320_del_dat[];
#include "data/scn1330_load.h" // MSN_LOAD_DAT scn1330_load[];
#include "data/scn1330_del_dat.h" // u_short scn1330_del_dat[];
#include "data/scn1410_load.h" // MSN_LOAD_DAT scn1410_load[];
#include "data/scn1410_del_dat.h" // u_short scn1410_del_dat[];
#include "data/scn_load1.h" // SCN_LOAD_DAT scn_load1[];
#include "data/msn2_title_load_dat.h" // MSN_LOAD_DAT msn2_title_load_dat[];
#include "data/scn2013_load.h" // MSN_LOAD_DAT scn2013_load[];
#include "data/scn2013_del_dat.h" // u_short scn2013_del_dat[];
#include "data/scn2160_load.h" // MSN_LOAD_DAT scn2160_load[];
#include "data/scn2160_del_dat.h" // u_short scn2160_del_dat[];
#include "data/scn2020_load.h" // MSN_LOAD_DAT scn2020_load[];
#include "data/scn2020_del_dat.h" // u_short scn2020_del_dat[];
#include "data/scn2030_load.h" // MSN_LOAD_DAT scn2030_load[];
#include "data/scn2030_del_dat.h" // u_short scn2030_del_dat[];
#include "data/scn2100_load.h" // MSN_LOAD_DAT scn2100_load[];
#include "data/scn2100_del_dat.h" // u_short scn2100_del_dat[];
#include "data/scn2040_load.h" // MSN_LOAD_DAT scn2040_load[];
#include "data/scn2040_del_dat.h" // u_short scn2040_del_dat[];
#include "data/scn2170_load.h" // MSN_LOAD_DAT scn2170_load[];
#include "data/scn2170_del_dat.h" // u_short scn2170_del_dat[];
#include "data/scn2060_load.h" // MSN_LOAD_DAT scn2060_load[];
#include "data/scn2060_del_dat.h" // u_short scn2060_del_dat[];
#include "data/scn2062_load.h" // MSN_LOAD_DAT scn2062_load[];
#include "data/scn2062_del_dat.h" // u_short scn2062_del_dat[];
#include "data/scn2070_load.h" // MSN_LOAD_DAT scn2070_load[];
#include "data/scn2070_del_dat.h" // u_short scn2070_del_dat[];
#include "data/scn2072_load.h" // MSN_LOAD_DAT scn2072_load[];
#include "data/scn2072_del_dat.h" // u_short scn2072_del_dat[];
#include "data/scn2090_load.h" // MSN_LOAD_DAT scn2090_load[];
#include "data/scn2090_del_dat.h" // u_short scn2090_del_dat[];
#include "data/scn2092_load.h" // MSN_LOAD_DAT scn2092_load[];
#include "data/scn2092_del_dat.h" // u_short scn2092_del_dat[];
#include "data/scn2130_load.h" // MSN_LOAD_DAT scn2130_load[];
#include "data/scn2130_del_dat.h" // u_short scn2130_del_dat[];
#include "data/scn2132_load.h" // MSN_LOAD_DAT scn2132_load[];
#include "data/scn2132_del_dat.h" // u_short scn2132_del_dat[];
#include "data/scn2140_load.h" // MSN_LOAD_DAT scn2140_load[];
#include "data/scn2140_del_dat.h" // u_short scn2140_del_dat[];
#include "data/scn2141_load.h" // MSN_LOAD_DAT scn2141_load[];
#include "data/scn2141_del_dat.h" // u_short scn2141_del_dat[];
#include "data/scn2150_load.h" // MSN_LOAD_DAT scn2150_load[];
#include "data/scn2150_del_dat.h" // u_short scn2150_del_dat[];
#include "data/scn_load2.h" // SCN_LOAD_DAT scn_load2[];
#include "data/msn3_title_load_dat.h" // MSN_LOAD_DAT msn3_title_load_dat[];
#include "data/scn3020_load.h" // MSN_LOAD_DAT scn3020_load[];
#include "data/scn3020_del_dat.h" // u_short scn3020_del_dat[];
#include "data/scn3030_load.h" // MSN_LOAD_DAT scn3030_load[];
#include "data/scn3030_del_dat.h" // u_short scn3030_del_dat[];
#include "data/scn3100_load.h" // MSN_LOAD_DAT scn3100_load[];
#include "data/scn3100_del_dat.h" // u_short scn3100_del_dat[];
#include "data/scn3050_load.h" // MSN_LOAD_DAT scn3050_load[];
#include "data/scn3050_del_dat.h" // u_short scn3050_del_dat[];
#include "data/scn3110_load.h" // MSN_LOAD_DAT scn3110_load[];
#include "data/scn3110_del_dat.h" // u_short scn3110_del_dat[];
#include "data/scn3060_load.h" // MSN_LOAD_DAT scn3060_load[];
#include "data/scn3060_del_dat.h" // u_short scn3060_del_dat[];
#include "data/scn3070_load.h" // MSN_LOAD_DAT scn3070_load[];
#include "data/scn3070_del_dat.h" // u_short scn3070_del_dat[];
#include "data/scn4900_load.h" // MSN_LOAD_DAT scn4900_load[];
#include "data/scn4900_del_dat.h" // u_short scn4900_del_dat[];
#include "data/scn4901_load.h" // MSN_LOAD_DAT scn4901_load[];
#include "data/scn4901_del_dat.h" // u_short scn4901_del_dat[];
#include "data/scn_load3.h" // SCN_LOAD_DAT scn_load3[];
#include "data/msn4_title_load_dat.h" // MSN_LOAD_DAT msn4_title_load_dat[];
#include "data/scn4011_load.h" // MSN_LOAD_DAT scn4011_load[];
#include "data/scn4011_del_dat.h" // u_short scn4011_del_dat[];
#include "data/scn4020_load.h" // MSN_LOAD_DAT scn4020_load[];
#include "data/scn4020_del_dat.h" // u_short scn4020_del_dat[];
#include "data/scn4040_load.h" // MSN_LOAD_DAT scn4040_load[];
#include "data/scn4040_del_dat.h" // u_short scn4040_del_dat[];
#include "data/scn4042_load.h" // MSN_LOAD_DAT scn4042_load[];
#include "data/scn4042_del_dat.h" // u_short scn4042_del_dat[];
#include "data/scn4030_load.h" // MSN_LOAD_DAT scn4030_load[];
#include "data/scn4030_del_dat.h" // u_short scn4030_del_dat[];
#include "data/scn4050_load.h" // MSN_LOAD_DAT scn4050_load[];
#include "data/scn4050_del_dat.h" // u_short scn4050_del_dat[];
#include "data/scn4070_load.h" // MSN_LOAD_DAT scn4070_load[];
#include "data/scn4070_del_dat.h" // u_short scn4070_del_dat[];
#include "data/scn_load4.h" // SCN_LOAD_DAT scn_load4[];
#include "data/msn_title_load_dat.h" // MSN_LOAD_DAT *msn_title_load_dat[];
#include "data/msn_start_floor.h" // u_char msn_start_floor[];
#include "data/msn_start_room.h" // u_char msn_start_room[];
#include "data/scn_load.h" // SCN_LOAD_DAT *scn_load[];
#include "data/msn_title_sp_bak.h" // SPRT_SDAT msn_title_sp_bak[];
#include "data/msn_title00_sp_flr.h" // SPRT_SDAT msn_title00_sp_flr[];
#include "data/msn_title00_sp_ttl.h" // SPRT_SDAT msn_title00_sp_ttl[];
#include "data/msn_title01_sp_flr.h" // SPRT_SDAT msn_title01_sp_flr[];
#include "data/msn_title01_sp_ttl.h" // SPRT_SDAT msn_title01_sp_ttl[];
#include "data/msn_title02_sp_flr.h" // SPRT_SDAT msn_title02_sp_flr[];
#include "data/msn_title02_sp_ttl.h" // SPRT_SDAT msn_title02_sp_ttl[];
#include "data/msn_title03_sp_flr.h" // SPRT_SDAT msn_title03_sp_flr[];
#include "data/msn_title03_sp_ttl.h" // SPRT_SDAT msn_title03_sp_ttl[];
#include "data/msn_title04_sp_flr.h" // SPRT_SDAT msn_title04_sp_flr[];
#include "data/msn_title04_sp_ttl.h" // SPRT_SDAT msn_title04_sp_ttl[];
#include "data/msn_title_sp_flr_no.h" // u_char msn_title_sp_flr_no[];
#include "data/msn_title_flr_sp_num.h" // u_char msn_title_flr_sp_num[];
#include "data/msn_title_sp_ttl_no.h" // u_char msn_title_sp_ttl_no[];
#include "data/msn_title_ttl_sp_num.h" // u_char msn_title_ttl_sp_num[];
#include "data/msn_title_sp_flr.h" // SPRT_SDAT *msn_title_sp_flr[];
#include "data/msn_title_sp_ttl.h" // SPRT_SDAT *msn_title_sp_ttl[];
#include "data/stg_title_sp_bak.h" // SPRT_SDAT stg_title_sp_bak[];
#include "data/stg_title_sp_flr.h" // SPRT_SDAT stg_title_sp_flr[];
#include "data/stg_title_sp_ttl.h" // SPRT_SDAT stg_title_sp_ttl[];

MSN_LOAD_DAT load_dat_wrk[40] = {0};

MSN_TITLE_WRK mttl_wrk = {0};
EVENT_LOAD_WRK ev_load_wrk = {0};

#define ENE_DMG_TEX_ADDRESS 0x98000
#define SPRT_ADDRESS 0xb30000
#define TITLE_PK2_ADDRESS 0x1e90000
#define SPR_FILE_ADDRESS 0x1e90000

void MissionTitleInit(int msn_no)
{
    DataLoadWrkInit();

    mttl_wrk = (MSN_TITLE_WRK){0};

    mttl_wrk.mode = 0;
    mttl_wrk.load_mode = 0;

    ingame_wrk.stts |= 0x20 | 0x8;

    if (ingame_wrk.game == 0)
    {
        map_wrk.floor = msn_start_floor[msn_no];
#ifdef BUILD_EU_VERSION
        mttl_wrk.load_id = LoadReqLanguage(MSN00TTL_E_PK2 + msn_no * 5, TITLE_PK2_ADDRESS);
#else
        mttl_wrk.load_id = LoadReq(msn_no + MSN00TTL_PK2, TITLE_PK2_ADDRESS);
#endif
    }
    else if (ingame_wrk.game == 1)
    {
        mttl_wrk.load_id = StageTitleInit();
    }

    MissionStartMapItemInit(msn_no);
    AdpcmMapNoUse();
}

int MissionTitleMain(int msn_no)
{
    switch(mttl_wrk.mode)
    {
    case MSN_TITLE_MODE_READY:
        if (IsLoadEnd(mttl_wrk.load_id) != 0)
        {
            mttl_wrk.mode = MSN_TITLE_MODE_IN;
            mttl_wrk.time = 30;
            EAdpcmCmdPlay(0, 0, AO002_SHOU_TITLE_STR, 0, 0x3fff, 0x280, 0xfff, 0);
        }
    break;
    case MSN_TITLE_MODE_IN:
        if (mttl_wrk.time == 0)
        {
            mttl_wrk.mode = MSN_TITLE_MODE_LOAD;
            mttl_wrk.time = 180;
        }
        else
        {
            mttl_wrk.time -= 1;
        }
    break;
    case MSN_TITLE_MODE_LOAD:
        if (mttl_wrk.time != 0)
        {
            mttl_wrk.time -= 1;
        }

        if (ingame_wrk.game == 0)
        {
            if (MissionTitleLoad(msn_no) != 0 && mttl_wrk.time == 0)
            {
                mttl_wrk.mode = MSN_TITLE_MODE_OUT;
                mttl_wrk.time = 30;
            }
        }
        else if (ingame_wrk.game == 1)
        {
            if (StageTitleLoad() != 0 && mttl_wrk.time == 0)
            {
                mttl_wrk.mode = MSN_TITLE_MODE_OUT;
                mttl_wrk.time = 30;
            }
        }
    break;
    case MSN_TITLE_MODE_OUT:
        if (mttl_wrk.time != 0)
        {
            mttl_wrk.time -= 1;
        }
        else
        {
            mttl_wrk.mode = MSN_TITLE_MODE_END_PRE;
            EAdpcmFadeOut(30);
        }
    break;
    case MSN_TITLE_MODE_END_PRE:
        if (IsEndAdpcmFadeOut() != 0)
        {
            mttl_wrk.mode = MSN_TITLE_MODE_END;
        }
    break;
    case MSN_TITLE_MODE_END:
        ingame_wrk.stts &= 0x80 | 0x40 | 0x10 | 0x4 | 0x2 | 0x1;
        return 1;
    break;
    }

    if (mttl_wrk.mode != MSN_TITLE_MODE_READY && mttl_wrk.mode != MSN_TITLE_MODE_END)
    {
        if (ingame_wrk.game == 0)
        {
            MissionTitleDisp(msn_no);
        }
        else if (ingame_wrk.game == 1)
        {
            StageTitleDisp(msn_no);
        }
    }

    return 0;
}

int MissionTitleLoad(int msn_no)
{
    if (mttl_wrk.load_mode == 9)
    {
        return 1;
    }

    if (mttl_wrk.load_mode == 0)
    {
        mttl_wrk.load_mode = 1;

        ReqMsnInitPlyr(msn_no);

        return 0;
    }
    else if (mttl_wrk.load_mode == 1)
    {
        if (MsnInitPlyr())
        {
            mttl_wrk.load_mode = 2;
        }

        return 0;
    }

    if (msn_title_load_dat[msn_no][mttl_wrk.load_count].file_no == 0xFFFF)
    {
        mttl_wrk.load_mode = 4;
        mttl_wrk.load_count = 0;
    }
    else if (mttl_wrk.load_mode == 3)
    {
        if (IsLoadEnd(mttl_wrk.load_id))
        {
            MissionDataLoadAfterInit(msn_title_load_dat[msn_no] + mttl_wrk.load_count);
            SetDataLoadWrk(msn_title_load_dat[msn_no] + mttl_wrk.load_count);

            mttl_wrk.load_mode = 2;
            mttl_wrk.load_count++;
        }
    }
    else if (mttl_wrk.load_mode == 2)
    {
        mttl_wrk.load_id = MissionDataLoadReq(&msn_title_load_dat[msn_no][mttl_wrk.load_count]);
        mttl_wrk.load_mode = 3;
    }
    else if (mttl_wrk.load_mode == 4)
    {
        if (mttl_wrk.load_count != 0)
        {
            FloatGhostLoadReq();

            mttl_wrk.load_mode = 6;

            return 0;
        }

        RoomMdlLoadReq(NULL, mttl_wrk.load_count, msn_no, msn_start_room[msn_no], 1);

        area_wrk.room[mttl_wrk.load_count] = msn_start_room[msn_no];

        mttl_wrk.load_mode = 5;
    }
    else if (mttl_wrk.load_mode == 5)
    {
        if (RoomMdlLoadWait() == 0)
        {
            return 0;
        }

        mttl_wrk.load_mode = 4;
        mttl_wrk.load_count++;
    }
    else if (mttl_wrk.load_mode == 6)
    {
        if (FloatGhostLoadMain() == 0)
        {
            return 0;
        }

        FloatGhostAppearTypeSet(ap_wrk.fgst_no, 0, msn_start_room[msn_no]);

        if (GuardGhostAppearSet() == 0)
        {
            mttl_wrk.load_mode = 9;
        }
        else
        {
            mttl_wrk.load_mode = 8;
        }
    }
    else if (mttl_wrk.load_mode == 7)
    {
        if (DeadGhostLoad() == 0)
        {
            return 0;
        }

        if (GuardGhostAppearSet() == 0)
        {
            mttl_wrk.load_mode = 9;
        }
        else
        {
            mttl_wrk.load_mode = 8;
        }
    }
    else
    {
        if (mttl_wrk.load_mode != 8)
        {
            return 0;
        }

        if (GuardGhostLoad() != 0)
        {
            mttl_wrk.load_mode = 9;
        }
    }

    return 0;
}

int MissionDataLoadReq(MSN_LOAD_DAT *dat)
{
    int ret;

    if (dat->file_type == 2)
    {
        if (dat->addr == 16)
        {
            ap_wrk.fg_se_empty[0] = 1;
        }
        else if (dat->addr == 17)
        {
            ap_wrk.fg_se_empty[1] = 1;
        }
        else if (dat->addr == 18)
        {
            ap_wrk.fg_se_empty[2] = 1;
        }

        if (dat->tmp_no == 0)
        {
            ret = SeFileLoadAndSet(dat->file_no, dat->addr);
        }
        else
        {
            ret = SeFileLoadAndSetFGhost(dat->file_no, dat->addr);
        }
    }
    else
    {
        if (dat->file_type == 9)
        {
            LoadEneDmgTex(dat->tmp_no, (u_int *)(dat->addr + ENE_DMG_TEX_ADDRESS));

            ret = LoadReq(dat->file_no, dat->addr);
        }
#ifdef BUILD_EU_VERSION
        else if (dat->file_type == 14)
        {
            ret = LoadReqLanguage(dat->file_no,dat->addr);
        }
#endif
        else
        {
            ret = LoadReq(dat->file_no, dat->addr);
        }
    }

    return ret;
}

void MissionDataLoadAfterInit(MSN_LOAD_DAT *dat)
{
    switch (dat->file_type)
    {
    case 3:
        memcpy(&map_cam_dat, (void *)dat->addr, sizeof(map_cam_dat));
    break;
    case 4:
        memcpy(&map_cam_dat2, (void *)dat->addr, sizeof(map_cam_dat2));
    break;
    case 5:
        memcpy(&map_cam_dat3, (void *)dat->addr, sizeof(map_cam_dat3));
    break;
    case 6:
        memcpy(&map_cam_dat4, (void *)dat->addr, sizeof(map_cam_dat4));
    break;
    case 7:
        FSpeMapDataMapping();
        map_wrk.dat_adr = GetFloorTopAddr(map_wrk.floor);
    break;
    case 8:
        motInitEnemyMdl((u_int *)dat->addr, dat->file_no - M000_MIKU_MDL);
    break;
    case 9:
    case 10:
        motInitEnemyAnm((u_int *)dat->addr, dat->tmp_no, dat->file_no - M000_MIKU_ANM);
    break;
    case 11:
        ItemLoadAfterInit(dat->file_no - I000_PLAY_CAMERA1_SGD, dat->addr);
    break;
    }
}

void DataLoadWrkInit() {
    int i;

    memset(&load_dat_wrk, 0, sizeof(load_dat_wrk));

    for (i = 0; i < 40; i++)
    {
        load_dat_wrk[i].file_no = -1;
    }
}

void SetDataLoadWrk(MSN_LOAD_DAT *dat)
{
    int i;

    for (i = 0; i < 40; i++)
    {
        if (load_dat_wrk[i].file_no == 0xFFFF)
        {
            load_dat_wrk[i].file_no = dat->file_no;
            load_dat_wrk[i].file_type = dat->file_type;
            load_dat_wrk[i].tmp_no = dat->tmp_no;
            load_dat_wrk[i].addr = dat->addr;
            return;
        }
    }
}

void DelDataLoadWrk(u_short file_no) {
    int i;

    for (i = 0; i < 40; i++)
    {
        if (load_dat_wrk[i].file_no == file_no)
        {
            if (file_no >= M000_MIKU_ANM && file_no <= I000_PLAY_CAMERA1_SGD)
            {
                motReleaseAniMdlBuf((u_short)file_no - M000_MIKU_ANM, (u_int *)load_dat_wrk[i].addr);
            }
            else if (load_dat_wrk[i].file_type == 2)
            {
                if (load_dat_wrk[i].addr == 16)
                {
                    ap_wrk.fg_se_empty[0] = 0;
                }
                else if (load_dat_wrk[i].addr == 17)
                {
                    ap_wrk.fg_se_empty[1] = 0;
                }
                else if (load_dat_wrk[i].addr == 18)
                {
                    ap_wrk.fg_se_empty[2] = 0;
                }
            }

            load_dat_wrk[i].file_no = -1;

            break;
        }
    }
}

u_int GetLoadDataAddr(u_short file_no)
{
    int i;

    for (i = 0; i < 40; i++)
    {
        if (load_dat_wrk[i].file_no == file_no)
        {
            return load_dat_wrk[i].addr;
        }
    }

    return 0;
}

void SortLoadDataAddr()
{
    int i;
    int j;
    MSN_LOAD_DAT tmp;

    for (i = 0; i < 40; i++)
    {
        if (load_dat_wrk[i].file_type == 9 || load_dat_wrk[i].file_type == 10)
        {
            for (j = i + 1; j < 40; j++)
            {
                if (load_dat_wrk[j].file_type != 9 && load_dat_wrk[j].file_type != 10)
                {
                    tmp.file_no = load_dat_wrk[i].file_no;
                    tmp.file_type = load_dat_wrk[i].file_type;
                    tmp.tmp_no = load_dat_wrk[i].tmp_no;
                    tmp.addr = load_dat_wrk[i].addr;

                    load_dat_wrk[i].file_no = load_dat_wrk[j].file_no;
                    load_dat_wrk[i].file_type = load_dat_wrk[j].file_type;
                    load_dat_wrk[i].tmp_no = load_dat_wrk[j].tmp_no;
                    load_dat_wrk[i].addr = load_dat_wrk[j].addr;

                    load_dat_wrk[j].file_no = tmp.file_no;
                    load_dat_wrk[j].file_type = tmp.file_type;
                    load_dat_wrk[j].tmp_no = tmp.tmp_no;
                    load_dat_wrk[j].addr = tmp.addr;
                }
            }
        }
    }
}

void MissionTitleDisp(int msn_no)
{
    int i;
    u_char alp_rate;
    SPRT_SDAT ssd;

    SetSprFile(SPR_FILE_ADDRESS);

    if (mttl_wrk.mode == 1)
    {
        alp_rate = (30 - mttl_wrk.time) * 100 / 30;
    }
    else if (mttl_wrk.mode == 3 || mttl_wrk.mode == 4)
    {
        alp_rate = mttl_wrk.time * 100 / 30;
    }
    else
    {
        alp_rate = 100;
    }

    for (i = 0; i < 11; i++)
    {
        SimpleDispSprt(&msn_title_sp_bak[i], SPR_FILE_ADDRESS, i, NULL, NULL, alp_rate);
    }

    for (i = 0; i < msn_title_flr_sp_num[msn_no]; i++)
    {
        SimpleDispAlphaSprt(&msn_title_sp_flr[msn_no][i], SPR_FILE_ADDRESS, msn_title_sp_flr_no[msn_no], alp_rate * 70 / 100, 0);
    }

    for (i = 0; i < msn_title_ttl_sp_num[msn_no]; i++)
    {
        SimpleDispSprt(&msn_title_sp_ttl[msn_no][i], SPR_FILE_ADDRESS, msn_title_sp_ttl_no[msn_no], NULL, NULL, alp_rate);
    }
}

void StageTitleDisp(int msn_no)
{
    int i;
    u_char alp_rate;
    SPRT_SDAT ssd;

    SetSprFile(SPR_FILE_ADDRESS);

    if (mttl_wrk.mode == 1)
    {
        alp_rate = (30 - mttl_wrk.time) * 100 / 30;
    }

    else if (mttl_wrk.mode == 3 || mttl_wrk.mode == 4)
    {
        alp_rate = mttl_wrk.time * 100 / 30;
    }
    else
    {
        alp_rate = 100;
    }

    for (i = 0; i < 11; i++)
    {
        SimpleDispSprt(&stg_title_sp_bak[i], SPR_FILE_ADDRESS, i, NULL, NULL, alp_rate);
    }

    return;
}

void EventLoadDataInit()
{
    ev_load_wrk = (EVENT_LOAD_WRK){0};
}

int EventLoadData(u_char load_no)
{
    SCN_LOAD_DAT *sld;
    MSN_LOAD_DAT *mld;
    int i;

    if (ev_load_wrk.mode == EV_LOAD_MODE_END)
    {
        return 1;
    }

    sld = scn_load[ingame_wrk.msn_no];

    i = 0;

    while (1)
    {
        if (sld[i].scn_no == 0xff)
        {
            ev_load_wrk.mode = EV_LOAD_MODE_END;
            return 1;
        }

        if (load_no == sld[i].scn_no)
        {
            load_no = i;
            break;
        }

        i++;
    }

    mld = &sld[load_no].load_dat[ev_load_wrk.count];

    if (mld->file_type == 0 && ev_load_wrk.mode != 0)
    {
        ev_load_wrk.mode = EV_LOAD_MODE_END;
        return 1;
    }

    if (ev_load_wrk.mode == EV_LOAD_MODE_REQ)
    {
        if (mld->file_type == 13)
        {
            SceneDataLoadReq(mld->file_no, (u_int *)mld->addr);

            ev_load_wrk.mode = EV_LOAD_MODE_SCENE;

            return 0;
        }
        else if (mld->file_type == 12)
        {
            RoomLoadReq(mld->file_no);

            ev_load_wrk.mode = EV_LOAD_MODE_ROOM;

            return 0;
        }
    }

    switch(ev_load_wrk.mode)
    {
    case EV_LOAD_MODE_READY:
        for (i = 0; sld[load_no].del_dat[i] != 0xffff; i++)
        {
            DelDataLoadWrk(sld[load_no].del_dat[i]);
        }

        ev_load_wrk.mode = EV_LOAD_MODE_REQ;
    break;
    case EV_LOAD_MODE_SCENE:
        if (SceneDataLoadWait() != 0)
        {
            SetDataLoadWrk(mld);

            ev_load_wrk.mode = EV_LOAD_MODE_REQ;
            ev_load_wrk.count++;
        }
    break;
    case EV_LOAD_MODE_ROOM:
        if (RoomMdlLoadWait() != 0)
        {
            ev_load_wrk.mode = EV_LOAD_MODE_REQ;
            ev_load_wrk.count++;
        }
    break;
    case EV_LOAD_MODE_REQ:
        ev_load_wrk.load_id = MissionDataLoadReq(mld);
        ev_load_wrk.mode = EV_LOAD_MODE_LOAD;
    break;
    case EV_LOAD_MODE_LOAD:
        if (IsLoadEnd(ev_load_wrk.load_id) != 0)
        {
            MissionDataLoadAfterInit(mld);
            SetDataLoadWrk(mld);

            ev_load_wrk.mode = EV_LOAD_MODE_REQ;
            ev_load_wrk.count++;
        }
    break;
    }

    return 0;
}

int GetLoadGhostInfo(u_char *load_inf)
{
    int i;
    int count;

    count = 0;

    for (i = 0; i < 40; i++)
    {
        if (load_dat_wrk[i].file_type == 8)
        {
            switch (load_dat_wrk[i].addr)
            {
            case 0xc80000:
                load_inf[0] = 1;
                count++;
            break;
            case 0xd00000:
                load_inf[1] = 1;
                count++;
            break;
            case 0xd80000:
                load_inf[2] = 1;
                count++;
            break;
            }
        }
    }

    return count;
}

void MikuCGDisp()
{
    int i;
    SPRT_SDAT ssd;

    for (i = 0; i < 11; i ++)
    {
        SimpleDispSprt(&msn_title_sp_bak[i], SPRT_ADDRESS,i, NULL, NULL, 0x64);
    }
}

void RoomLoadReq(int load_room)
{
    int i;

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
            if(furn_wrk[i].use == 0 || furn_wrk[i].use == 2)
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
            RoomMdlLoadReq(0, i, ingame_wrk.msn_no, load_room, 2);
            FloatGhostAppearTypeSet(ap_wrk.fgst_no, i, load_room);

            area_wrk.room[i] = load_room;

            break;
        }
    }
}
