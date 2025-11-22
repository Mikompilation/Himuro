#include "common.h"
#include "enums.h"
#include "typedefs.h"
#include "ev_main.h"

#include "main/glob.h"
#include "os/system.h"
#include "os/eeiop/eese.h"
#include "os/eeiop/se_ev.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/adpcm/ea_ctrl.h" // `AdpcmMapNoUse` needs to be implicitly declared
#include "os/eeiop/adpcm/ea_scene.h"
#include "os/eeiop/adpcm/ea_event.h"
#include "common/ul_math.h"
#include "ingame/ingame.h"
#include "ingame/ig_init.h"
#include "ingame/camera/camera.h"
#include "ingame/enemy/ene_ctl.h"
#include "ingame/entry/entry.h"
#include "ingame/entry/ap_fgost.h"
#include "ingame/entry/ap_pgost.h"
#include "ingame/entry/ap_sgost.h"
#include "ingame/event/ev_load.h" // `DelDataLoadWrk` needs to be implicitly declared
#include "ingame/event/ev_spcl.h"
#include "ingame/map/door_ctl.h" // `DoorKeyUnlockOnly` and `DoorSttsChange` need to be implicitly declared
#include "ingame/map/find_ctl.h"
#include "ingame/map/furn_ctl.h"
#include "ingame/map/item_ctl.h"
#include "ingame/map/rotd_ctl.h"
#include "ingame/map/furn_spe/furn_spe.h" // `FurnActOffEve` ans `FurnActOnEve` need to be implicitly declared
#include "ingame/menu/item.h"
#include "ingame/menu/item_get.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/sp_menu.h"
#include "ingame/plyr/plyr_ctl.h"
#include "outgame/mode_slct.h"
#include "graphics/mov/movie.h"
#include "graphics/scene/scene.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/g2d_main.h"
#include "graphics/graph2d/effect_scr.h"
#include "graphics/graph2d/effect_sub2.h"
#include "graphics/graph3d/gra3d.h"
#include "graphics/graph3d/load3d.h"

#define INCLUDING_FROM_EV_MAIN_C
#include "ingame/plyr/unit_ctl.h" // `ReqEneStop` needs to be implicitly declared
#undef INCLUDING_FROM_EV_MAIN_C

u_char event_stts[250] = {0};
EVENT_WRK ev_wrk = {0};

#define PI 3.1415927f

#define DEG2RAD(x) ((float)(x)*PI/180.0f)

#define ADDRESS ((u_int *)0x1090000)
#define DVD_DATA_ADDR 0x7F0000

void EventWrkInit()
{
    memset(&ev_wrk, 0, sizeof(ev_wrk));
    memset(event_stts, 0, sizeof(event_stts));
    black_time = 0;
    load_file_id = 0;
}

void EventInit()
{
    int i;

    if (event_stts[0] == 0)
    {
        ingame_wrk.mode = INGAME_MODE_EVENT;

        ev_wrk.evt_no = 0;
        ev_wrk.mode = 1;
        ev_wrk.btl_ene = 0xff;
        ev_wrk.use_item = 0xff;
        ev_wrk.msg_init = 0;
        ev_wrk.pht_2d = 0xff;

        for (i = 0; i <= 9; i++)
        {
            ev_wrk.pht_ev[i] = 0xff;
        }

        for (i = 0; i <= 1; i++)
        {
            ev_wrk.gst_door[i] = 0xff;
        }

        EventMain();
    }
}

void EventMain()
{
    int i;
    int loop;
    int *addr;
    u_char *dat_adr;
    short int scene_no;

    addr = (int *)DVD_DATA_ADDR;
    addr++;
    addr = (int *)(*addr + DVD_DATA_ADDR + ev_wrk.evt_no * 4);
    dat_adr = (u_char *)(*addr + DVD_DATA_ADDR);

    loop = 1;

    while(loop)
    {
        switch (dat_adr[0])
        {
        case MSG_DISP:
            if (dat_adr[1] == 0)
            {
                if (ev_wrk.msg_init == 0)
                {
                    SetMessageAddr(GetEventMessageAddr(*(short*)&dat_adr[2]));

                    ev_wrk.msg_init = 1;
                }

                if (ev_wrk.msg_init == 1)
                {
                    if (key_now[5][0] == 1)
                    {
                        MessageWaitOff();
                    }

                    if (SetMessage() == 0)
                    {
                        ev_wrk.msg_init = 2;
                    }

                    loop = 0;
                }
            }
            dat_adr += 4;
        break;
        case MOVIE_PLAY:
            if (dat_adr[1] == 0)
            {
                u_char temp_s2;
                u_char temp_s4;

                scene_no = dat_adr[2];
                temp_s2 = dat_adr[4];
                temp_s4 = dat_adr[6];

                eff_filament_off = 1;

                if (EventSceneCtrl(scene_no) == 2)
                {
                    dat_adr[1] = 1;
                    event_stts[ev_wrk.evt_no] = 1;
                    ingame_wrk.stts &= 0xD7;

                    if (temp_s2 != 0)
                    {
                        ingame_wrk.stts |= 0x20 | 0x8;

                        AdpcmSceneStop();
                        AdpcmMapNoUse();

                        if (temp_s4 == 0)
                        {
                            SetSysBackColor(0, 0, 0);
                            scene_bg_color = 0;
                        }
                        else
                        {
                            SetSysBackColor(0xFF, 0xFF, 0xFF);
                            scene_bg_color = 1;
                        }
                    }
                    else
                    {
                        SetSysBackColor(0, 0, 0);
                        scene_bg_color = 0;
                        SeRevival(0x1E);
                        eff_filament_off = 0;
                        AdpcmMapUse();

                        if (scene_no == 0x21)
                        {
                            r022_light_on = 1;

                            SetPreRenderR022(1);
                        }
                    }

                }
                dat_adr += 8; // it doesn't matter what increment is used (dat_adr is a local variable)
                return;
            }

            dat_adr += 8;
        break;
        case SAVE_POINT:
            SavePointMenuOpen();

            dat_adr += 2;
        break;
        case GAME_CLEAR:
            dat_adr += 2;
        break;
        case SPCL_REQ:
            ingame_wrk.mode = INGAME_MODE_SPECIAL_EVENT;
            loop = 0;
            SpecialEventInit(dat_adr[1]);
        break;
        case CG2D_DISP:
            dat_adr += 0xC;
        break;
        case DSP3D_OFF:
            if (dat_adr[1] == 0)
            {
                ingame_wrk.stts |= 0x20;
                dat_adr[1] = 1;
            }
            dat_adr += 2;
        break;
        case DSP3D_ON:
            if (dat_adr[1] == 0)
            {
                ingame_wrk.stts &= 0x80 | 0x40 | 0x10 | 0x8 | 0x4 | 0x2 | 0x1;
                dat_adr[1] = 1;
            }
            dat_adr += 2;
        break;
        case EV_BGM_REQ:
            if (dat_adr[1] == 0)
            {
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case EV_BGM_STOP:
            if (dat_adr[1] == 0)
            {
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case ADPCM_REQ:
            if (dat_adr[1] == 0)
            {
                AdpcmPlayEvent(*(short*)&dat_adr[2], *(u_short*)&dat_adr[4], *(short*)&dat_adr[6]);
                dat_adr[1] = 1;
            }
            dat_adr += 8;
        break;
        case ADPCM_STOP:
            if (dat_adr[1] == 0)
            {
                AdpcmStopEvent(*(short*)&dat_adr[2]);
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case EV_SE_REQ:
            if (dat_adr[1] == 0)
            {
                SeEvReq(dat_adr[2], dat_adr[3], 0, 0);
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case EV_SE_STOP:
            if (dat_adr[1] == 0)
            {
                SeEvStop(dat_adr[2]);
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case PLYR_DSP_OFF:
            if (dat_adr[1] == 0)
            {
                plyr_wrk.sta |= 0x10;
                dat_adr[1] = 1;
            }
            dat_adr += 2;
        break;
        case PLYR_DSP_ON:
            if (dat_adr[1] == 0)
            {
                plyr_wrk.sta &= ~0x10;
                dat_adr[1] = 1;
            }
            dat_adr += 2;
        break;
        case PPOS_SET:
            if (dat_adr[1] == 0)
            {
                plyr_wrk.move_box.pos[0] = *(u_short*)&dat_adr[2];
                plyr_wrk.move_box.pos[1] = *(short*)&dat_adr[4];
                plyr_wrk.move_box.pos[2] = *(u_short*)&dat_adr[6];
                plyr_wrk.mvsta = 0;
                SetPlyrAnime(0, 0);
                dat_adr[1] = 1;
            }
            dat_adr += 8;
        break;
        case PROT_SET:
            if (dat_adr[1] == 0)
            {
                plyr_wrk.move_box.rot[1] = SetRot360(*(short*)&dat_adr[2]);
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case ENE_SET:
            EventEnemySet(dat_adr);
            dat_adr += 0xA;
        break;
        case EPOS_SET:
            if (dat_adr[1] == 0)
            {
                ene_wrk[dat_adr[2]].move_box.pos[0] = *(u_short*)&dat_adr[4];
                ene_wrk[dat_adr[2]].move_box.pos[1] = *(short*)&dat_adr[6];
                ene_wrk[dat_adr[2]].move_box.pos[2] = *(u_short*)&dat_adr[8];
                ene_wrk[dat_adr[2]].move_box.pos[3] = 1.0f;
                dat_adr[1] = 1;
            }
            dat_adr += 0xA;
        break;
        case EROT_SET:
            if (dat_adr[1] == 0)
            {
                ene_wrk[dat_adr[2]].move_box.rot[1] = SetRot360(*(short*)&dat_adr[4]);
                ene_wrk[dat_adr[2]].move_box.rot[3] = 1.0f;
                dat_adr[1] = 1;
            }
            dat_adr += 6;
        break;
        case ENE_SE_LOAD:
            if (dat_adr[1] == 0)
            {
                dat_adr[1] = 1;
            }
            dat_adr += 6;
        break;
        case ITEM_GET:
            if (dat_adr[1] == 0)
            {
                if (poss_item[dat_adr[2]] + dat_adr[3] < 99)
                {
                    poss_item[dat_adr[2]] += dat_adr[3];
                }
                else
                {
                    poss_item[dat_adr[2]] = 99;
                }
                if (poss_item[9] != 0)
                {
                    poss_item[9] = 1;
                }

                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case ITEM_LOST:
            if (dat_adr[1] == 0)
            {
                if (dat_adr[3] < poss_item[dat_adr[2]])
                {
                    poss_item[dat_adr[2]]--;
                }
                else
                {
                    poss_item[dat_adr[2]] = 0;
                }
                dat_adr[1] = 1;
            }
            if (dat_adr[2] == 9)
            {
                if (ingame_wrk.clear_count != 0)
                {
                    poss_item[9] = 1;
                }
            }
            dat_adr += 4;
        break;
        case ITEM_SET:
            if (dat_adr[1] == 0)
            {
                item_ap[dat_adr[2]][0] = 0;
                item_ap[dat_adr[2]][1] = 0;
                ItemDispDataRenew();
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case ITEM_FIND:
            loop = 0;
            ItemGet(4, dat_adr[1], dat_adr[2], dat_adr[4]);
            ingame_wrk.mode = INGAME_MODE_GET_ITEM;
            ev_wrk.get_item = 1;
            event_stts[ev_wrk.evt_no] = 1;
            ev_wrk.evt_no = 0xFFU;
            ev_wrk.mode  = 0;
        break;
        case ITEM_DEL:
            if (dat_adr[1] == 0)
            {
                item_ap[dat_adr[2]][0] = 0xFFFF;
                item_ap[dat_adr[2]][1] = 0xFFFF;
                ItemDispDataRenew();
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case FILE_GET:
            if (dat_adr[1] == 0)
            {
                if (poss_file[dat_adr[2]][dat_adr[3]] != 3)
                {
                    poss_file[dat_adr[2]][dat_adr[3]] = 2;
                }
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case FILE_SET:
            if (dat_adr[1] == 0)
            {
                poss_file[dat_adr[2]][dat_adr[3]] = 1;
                dat_adr[1] = 1;
                ItemDispDataRenew();
            }
            dat_adr += 4;
        break;
        case FILE_FIND:
            loop = 0;
            ItemGet(dat_adr[2], dat_adr[3], dat_adr[4], dat_adr[6]);
            ingame_wrk.mode = INGAME_MODE_GET_ITEM;
            ev_wrk.get_item = 1;
            event_stts[ev_wrk.evt_no] = 1;
            ev_wrk.evt_no = 0xFFU;
            ev_wrk.mode  = 0;
        break;
        case FILE_DEL:
            if (dat_adr[1] == 0)
            {
                poss_file[dat_adr[2]][dat_adr[3]] = 0;
                dat_adr[1] = 1;
                ItemDispDataRenew();
            }
            dat_adr += 4;
        break;
        case DOOR_LOCK:
            if (dat_adr[1] == 0)
            {
                DoorKeyLockOnly(*(u_short*)&dat_adr[2]);
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case DOOR_UNLOCK:
            if (dat_adr[1] == 0)
            {
                DoorKeyUnlockOnly(*(short*)&dat_adr[2]);
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case DOOR_OPEN:
            if (dat_adr[1] == 0)
            {
                DoorSttsChange(*(short*)&dat_adr[2], 4);
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case DOOR_CLOSE:
            if (dat_adr[1] == 0)
            {
                DoorSttsChange(*(short*)&dat_adr[2], 1);
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case DOOR_OPNING:
            if (dat_adr[1] == 0)
            {
                DoorSttsChange(*(short*)&dat_adr[2], 3);
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case DOOR_OPNF:
            if (dat_adr[1] == 0)
            {
                DoorSttsChange(*(short*)&dat_adr[2], 5);
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case DOOR_CLSING:
            if (dat_adr[1] == 0)
            {
                DoorSttsChange(*(short*)&dat_adr[2], 6);
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case DOOR_APPEAR:
            if (dat_adr[1] == 0)
            {
                DoorPassSetHide(*(short*)&dat_adr[2]);
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case DOOR_CNG:
            if (dat_adr[1] == 0)
            {
                DoorChangeModel(*(short*)&dat_adr[2], *(short*)&dat_adr[4]);
                dat_adr[1] = 1;
            }
            dat_adr += 6;
        break;
        case DOOR_DSP:
            DoorDispOnOff(*(short*)&dat_adr[2], dat_adr[1]);
            dat_adr += 4;
        break;
        case ROT_DOOR_CNG:
            ChangeRotDoorFaceRoomId(*(short*)&dat_adr[2], dat_adr[1]);
            dat_adr += 4;
        break;
        case DOOR_PST:
            DoorPassSetTouch(*(short*)&dat_adr[2]);
            dat_adr += 4;
        break;
        case FURN_ACT:
            if (dat_adr[1] == 0)
            {
                if (dat_adr[4] != 0)
                {
                    FurnActOnEve(*(short*)&dat_adr[2]);
                }
                else
                {
                    FurnActOffEve(*(short*)&dat_adr[2]);
                }
            }
            dat_adr += 6;
        break;
        case FATTR_CNG:
            if (dat_adr[1] == 0)
            {
                SetFurnAttrEve(*(short*)&dat_adr[2], *(short*)&dat_adr[4], dat_adr[6]);
            }
            dat_adr += 8;
        break;
        case PHOTO_LOCK:
            if (dat_adr[1] != 0)
            {
                plyr_wrk.sta = plyr_wrk.sta & 0xFFFFFFFD;
            }
            else
            {
                plyr_wrk.sta |= 0x2;
            }

            if (ingame_wrk.clear_count != 0)
            {
                plyr_wrk.sta = plyr_wrk.sta & 0xFFFFFFFD;
            }
            dat_adr += 2;
        break;
        case PHT_EV_SET:
            ev_wrk.pht_ev[dat_adr[1]] = dat_adr[2];
            dat_adr += 4;
        break;
        case PHT_EV_CLR:
            for (i = 0; i < 10; i++)
            {
                ev_wrk.pht_ev[i] = 0xFF;
            }
            dat_adr += 2;
        break;
        case PGST_SET:
            PuzzleGhostReq(dat_adr[1]);
            dat_adr += 2;
        break;
        case PGST_REQ:
            PuzzleGhostLoadReq(dat_adr[1]);
            dat_adr += 2;
        break;
        case AGST_END:
            if (dat_adr[1] == 0)
            {
                EndAutoEne(1);
                dat_adr[1] = 1;
            }
            dat_adr += 2;
        break;
        case ZGST_PAUSE:
            ap_wrk.stts |= 0x80;
            dat_adr += 2;
        break;
        case ZGST_RSTRT:
            ap_wrk.stts &= 0x7F;
            dat_adr += 2;
        break;
        case FGST_PAUSE:
            FloatGhostAppearStop();
            dat_adr += 2;
        break;
        case FGST_RSTRT:
            FloatGhostAppearStart();
            dat_adr += 2;
        break;
        case ROOM_FG_RS:
            ap_wrk.room_fg[dat_adr[1]] = 1;
            dat_adr += 2;
        break;
        case ROOM_FG_PS:
            ap_wrk.room_fg[dat_adr[1]] = 0;
            dat_adr += 2;
        break;
        case FGST_DEL:
            FuyuEneRelease();
            dat_adr += 2;
        break;
        case RGST_PAUSE:
            ap_wrk.stts |= 0x20;
            dat_adr += 2;
        break;
        case RGST_RSTRT:
            ap_wrk.stts &= 0xDF;
            dat_adr += 2;
        break;
        case EVAP_FGST:
            ap_wrk.fg_ap = 0x3E8;
            dat_adr += 2;
        break;
        case EVAP_ZGST:
            ap_wrk.zh_ap = 0x3E8;
            ap_wrk.stts |= 4;
            dat_adr += 2;
        break;
        case SGST_SET:
            loop = 0;
            SettleGhostAppearReq(dat_adr[1]);
            event_stts[ev_wrk.evt_no] = 1;
            ev_wrk.evt_no  = 0xFFU;
            ev_wrk.mode = 0;
            dat_adr += 2;
        break;
        case DEL_LOAD:
            if (dat_adr[1] == 0)
            {
                DelDataLoadWrk(*(short*)&dat_adr[2]);
                dat_adr[1] = 1;
            }
            dat_adr += 4;
        break;
        case FLR_CNG:
            MapFloorChange(dat_adr[1]);
            dat_adr += 2;
        break;
        case PDOOR_OPEN:
            DoorOpenShiftForce(*(short*)&dat_adr[2]);
            if (ev_wrk.btl_lock != 0)
            {
                UnlockAllDoorInNowRoom();
                ev_wrk.btl_lock = 0;
            }
            dat_adr += 4;
        break;
        case DCAM_SET:
            ReqDramaCamera(dat_adr[1], *(short*)&dat_adr[2], *(short*)&dat_adr[4]);
            CameraNoRenewEach(3);
            dat_adr += 6;
        break;
        case BTL_LOCK:
            DoorLockBattleSet();
            dat_adr += 2;
        break;
        case MONO_DISP:
            ChangeMonochrome(dat_adr[1]);
            dat_adr += 2;
        break;
        case ZGST_DISP:
            ChangeMAGATOKI2(dat_adr[1]);
            dat_adr += 2;
        break;
        case EV_BLACK_OUT:
            if (dat_adr[1] == 0)
            {
                black_time = 64;
                SetBlackOut2(black_time);
                dat_adr[1] = 1;
            }
            if (black_time != 0)
            {
                loop = 0;
                black_time--;
            }
            dat_adr += 2;
        break;
        case EV_BLACK_IN:
            if (dat_adr[1] == 0)
            {
                SetBlackIn2(64);
                dat_adr[1] = 1;
            }
            dat_adr += 2;
        break;
        case EV_SE_LOAD:
            if (dat_adr[1] == 0)
            {
                load_file_id = SeFileLoadAndSetWide(*(short*)&dat_adr[2]);
                dat_adr[1] = 1;
            }
            if (dat_adr[1] == 1)
            {
                if ((load_file_id != -1) && (IsLoadEnd(load_file_id) == 0))
                {
                    loop = 0;
                }
                else
                {
                    dat_adr[1] = 2;
                }
            }
            dat_adr += 4;
        break;
        case HANYO_SE_REQ:
            if (dat_adr[1] == 0)
            {
               SeStartFix(0x5d,0,0x1000,0x1000,0);
                dat_adr[1] = 1U;
            }
            dat_adr += 2;
        break;
        case EV_SE_LOAD_BLK:
            if (dat_adr[1] == 0)
            {
                load_file_id = SeFileLoadAndSetWide(*(short*)&dat_adr[2]);
                SetBlackOut2(64);
                dat_adr[1] = 1;
            }
            if (black_time != 0)
            {
                loop = 0;
                black_time--;
            }
            if (dat_adr[1] == 1)
            {
                if (load_file_id != -1 && IsLoadEnd(load_file_id) == 0)
                {
                    loop = 0;
                }
                else
                {
                    dat_adr[1] = 2;
                }
            }
            dat_adr += 4;
        break;
        case OTHR_END:
            if (dat_adr[1] == 0)
            {
                 event_stts[*(short*)&dat_adr[2]] = 1;
            }
            dat_adr += 4;
        break;
        case NEXT_MSN:
            if (ingame_wrk.msn_no == 4)
            {
                EndingInit();
            }
            else if (ingame_wrk.msn_no != 0)
            {
                InterMissionInit();
            }
            else
            {
                ingame_wrk.msn_no += 1;
                NewgameItemInit2();
                InterMissionEnd();
            }
            ev_wrk.evt_no = 0xFF;
            loop = 0;
            ev_wrk.mode  = 0;
        break;
        case NV_END:
            if (ingame_wrk.mode != INGAME_MODE_SAVE_POINT)
            {
                ingame_wrk.mode = INGAME_MODE_NOMAL;
            }
            ev_wrk.evt_no = 0xFF;
            loop = 0;
            ev_wrk.mode  = 0;
        break;
        case EV_END:
            ingame_wrk.mode = INGAME_MODE_NOMAL;
            event_stts[ev_wrk.evt_no] = 1;
            loop = 0;
            ev_wrk.evt_no = 0xFF;
            ev_wrk.mode  = 0;
        break;
        }
    }
}

void EventOpenMain()
{
    if (ingame_wrk.mode != INGAME_MODE_EVENT && ev_wrk.get_item == 0 && IsDoorOpenCtrlMode() == 0 && EventOpenCtrl() != 0)
    {
        ingame_wrk.mode = INGAME_MODE_EVENT;
        ev_wrk.mode = 1;
    }
}

u_char EventOpenCtrl()
{
    int i;

    for (i = 0; i <= 15; i++)
    {
        if (ev_wrk.pos_req[i] != 0xFF && event_stts[ev_wrk.pos_req[i]] == 0 && EventOpenJudge(ev_wrk.pos_req[i]) != 0)
        {
            ev_wrk.evt_no = ev_wrk.pos_req[i];
            ev_wrk.use_item = 0xFF;
            ev_wrk.msg_init = 0;

            return 1;
        }
    }

    ev_wrk.use_item = 0xff;

    return 0;
}

u_char EventOpenJudge(short int event_no)
{
    int i;
    int *addr;
    sceVu0FVECTOR ev_pos;

    if (PuzzleGhostContinue(event_no) != 0)
    {
        return 0;
    }

    if (ev_wrk.get_item != 0)
    {
        return 0;
    }

    addr = (int *)(DVD_DATA_ADDR);
    addr = (int *)(*addr + (event_no * 4 + (int)addr));
    addr = (int *)(*addr + DVD_DATA_ADDR);

    if (ap_wrk.zh_mode != 0 && ((u_char *)addr)[3] != 202)
    {
        return 0;
    }

    if (plyr_wrk.sta & 1)
    {
        if (((u_char *)addr)[3] < 200 && ((u_char *)addr)[3] != 20 && ((u_char *)addr)[3] != 191 && ((u_char *)addr)[3] != 30)
        {
            return 0;
        }
    }

    for (i = 0; i < ((u_char *)addr)[0]; i++)
    {
        // skip the first 4 u_chars + 2 u_shorts and cycle over the next ((u_char *)addr)[0] shorts
        if (event_stts[(&((short *)addr)[4])[i]] == 0)
        {
            return 0;
        }
    }

    for (i = 0; i < ((u_char *)addr)[1]; i++)
    {
        // skip the first 4 u_chars + 2 u_shorts + ((u_char *)addr)[0] shorts and cycle over the next ((u_char *)addr)[1] shorts
        if (poss_item[(&((short *)addr)[4] + ((u_char *)addr)[0])[i]] == 0)
        {
            return 0;
        }
    }

    if (((u_char *)addr)[3] == 11 && ((u_char *)addr)[2] != ev_wrk.use_item)
    {
        return 0;
    }

    if (((u_char *)addr)[3] == 10 || ((u_char *)addr)[3] == 201 || ((u_char *)addr)[3] == 13)
    {
        if (plyr_wrk.mode != PMODE_NORMAL || *key_now[5] != 1 || GetLoadStartLock() != 0)
        {
            return 0;
        }
    }

    if (((u_char *)addr)[3] == 12)
    {
        if (ev_wrk.use_item == 0xff)
        {
            if (plyr_wrk.mode != PMODE_NORMAL || *key_now[5] != 1 || GetLoadStartLock() != 0)
            {
                return 0;
            }
        }
        else
        {
            if (((u_char *)addr)[2] != ev_wrk.use_item)
            {
                return 0;
            }
        }
    }

    if (((u_char *)addr)[3] == 30 && ((u_char *)addr)[2] != ev_wrk.pht_2d)
    {
        return 0;
    }

    if (((u_char *)addr)[3] == 40 && ((u_char *)addr)[2] != ev_wrk.pht_furn)
    {
        return 0;
    }

    ev_pos[0] = ((u_short *)addr)[2];
    ev_pos[2] = ((u_short *)addr)[3];

    if (ev_pos[0] != 0.0f && ev_pos[2] != 0.0f)
    {
        if (OutSightChk(ev_pos, plyr_wrk.move_box.pos, plyr_wrk.move_box.rot[1], DEG2RAD(60), 10000.0f) != 0)
        {
            if (((u_char *)addr)[3] != 13)
            {
                return 0;
            }

            if (GetDistV(ev_pos,plyr_wrk.move_box.pos) > 100.0f)
            {
                return 0;
            }
        }
    }

    if (((u_char *)addr)[3] < 190 && plyr_wrk.mode == PMODE_FINDER)
    {
        FinderEndSet();
    }
    else if ((((u_char *)addr)[3] == 0xff) && plyr_wrk.mode == PMODE_FINDER)
    {
        FinderEndSet();
    }

    return 1;
}

int GetEventMessageAddr(short int msg_no)
{
    u_char *addr;

    addr = (u_char *)Get4Byte((u_char *)DVD_DATA_ADDR + 2 * 4);
    addr += msg_no * 4 + DVD_DATA_ADDR;
    addr = (u_char *)Get4Byte(addr);

    return (int)addr + DVD_DATA_ADDR;
}

void EventEnemySet(u_char *addr)
{
    int i;

    ev_wrk.end_ev = addr[1];
    i = addr[2];

    if (ene_wrk[i].sta & 1)
    {
        ene_wrk[i].sta |= 0x2000;

        EneWrkReleaseChk(&ene_wrk[i]);
    }

    ev_wrk.btl_ene = addr[3];

    ene_wrk[i].dat_no = addr[3];
    ene_wrk[i].move_box.pos[0] = ((u_short *)addr)[2];
    ene_wrk[i].move_box.pos[1] = ((short *)addr)[3];
    ene_wrk[i].move_box.pos[2] = ((u_short *)addr)[4];
    ene_wrk[i].sta = 2;

    if (i == 3)
    {
        ene_wrk[i].type = 2;
    }
    else
    {
        ene_wrk[i].type = 0;
    }
}

int BattleEndEventOpenJudge(u_char ene_no)
{
    if (ingame_wrk.game != 1)
    {
        if (ev_wrk.btl_ene != ene_no)
        {
            return 0;
        }

        if (event_stts[ev_wrk.end_ev] != 0)
        {
            return 0;
        }

        if (EventOpenJudge(ev_wrk.end_ev) != 0)
        {
            ev_wrk.evt_no = ev_wrk.end_ev;
            ingame_wrk.mode = INGAME_MODE_EVENT;
            ingame_wrk.stts &= 0xdf;
            ev_wrk.mode = 1;
            ev_wrk.msg_init = 0;
            ev_wrk.btl_ene = 0xff;


            return 1;
        }
    }

    return 0;
}

int PhotoEventOpenJudge(u_char pht_2d, u_char pht_furn)
{
    int i;

    if (ingame_wrk.game == 1)
    {
        return 0;
    }

    ev_wrk.pht_2d = pht_2d;
    ev_wrk.pht_furn = pht_furn;

    for (i = 0; i < 10; i++)
    {
        if (
            event_stts[ev_wrk.pht_ev[i]] == 0 &&
            ev_wrk.pht_ev[i] != 0xff &&
            EventOpenJudge(ev_wrk.pht_ev[i]) != 0
        ) {
            ev_wrk.evt_no = ev_wrk.pht_ev[i];
            ingame_wrk.mode = INGAME_MODE_EVENT;
            ingame_wrk.stts &= 0xdf;
            ev_wrk.mode = 1;
            ev_wrk.pht_ev[i] = 0xff;
            ev_wrk.pht_2d = 0xff;
            ev_wrk.pht_furn = 0xff;
            ev_wrk.msg_init = 0;

            return 1;
        }
    }

    ev_wrk.pht_2d = 0xff;
    ev_wrk.pht_furn = 0xff;

    return 0;
}

float SetRot360(short int rot360)
{
    float rot;

    rot = rot360 / 180.0F * PI;

    RotLimitChk(&rot);

    return rot;
}

int EventSceneCtrl(short int scene_no)
{
    int ret;

    ret = 0;

    switch(ev_wrk.movie_on)
    {
        case 0:
            if (ingame_wrk.stts & 0x20)
            {
                ingame_wrk.stts |= 0x20 | 0x8;

                if(SceneDecisionMovie(scene_no) == 0)
                {
                    if (SceneAllLoad(scene_no, ADDRESS) != 0)
                    {
                        ev_wrk.movie_on = 4;
                        change_efbank = 0;

                        EventLoadDataInit();
                        SeStopAndBackup();
                        AdpcmMapNoUse();

                        scene_bg_load_flg = 1;
                    }

                    ret = 1;
                }
                else
                {
                    ret = 1;

                    SeStopAndBackup();
                    ReqMpegEvent(scene_no);

                    ev_wrk.movie_on = 3;
                    ingame_wrk.stts &= 0x80 | 0x40 | 0x10 | 0x4 | 0x2 | 0x1;
                }
            }
            else
            {
                if (SceneDecisionMovie(scene_no) == 0)
                {
                    SceneDataLoadReq(scene_no, ADDRESS);

                    change_efbank = 0;
                    ev_wrk.movie_on = 2;
                }
                else
                {
                    ev_wrk.movie_on = 1;
                }

                ret = 1;
            }
        break;
        case 2:
            if (SceneFadeOut() != 0)
            {
                if(SceneDataLoadWait())
                {
                    EventLoadDataInit();
                    AdpcmMapNoUse();

                    scene_bg_load_flg = 1;
                    ingame_wrk.stts |= 0x20 | 0x8;
                    ev_wrk.movie_on = 4;
                }
            }
        break;
        case 1:
            if (SceneFadeOut())
            {
                    ReqMpegEvent(scene_no);
                    ev_wrk.movie_on = 3;
                    ingame_wrk.stts |= 0x20 | 0x8;
            }
        break;
        case 3:
            if (movie_wrk.play_event_sta == 0)
            {
                ev_wrk.movie_on = 0;
                ingame_wrk.stts &= 0x80 | 0x40 | 0x10 | 0x8 | 0x4 | 0x2 | 0x1;

                AdpcmSceneStop();
                SetRenewDoorAddrForMovie();

                ret = 2;
            }
        break;
        case 4:
            if (EventLoadData(scene_no) != 0)
            {
                scene_bg_load_flg = 0;
            }

            gra2dDraw(3);
            SceneDraw(scene_no);
            gra2dDraw(4);
            gra2dDraw(5);

            if (SceneIsEnd() != 0)
            {
                ev_wrk.movie_on = 5;

                ret = 0;
            }
            else
            {
                SetSysBackColor(0,0,0);

                ret = 1;
            }
        break;
        case 5:
            if (EventLoadData(scene_no))
            {
                Init3D();

                change_efbank = 1;
                ev_wrk.movie_on = 0;

                ret = 2;
            }
    }

    return ret;
}

int SceneFadeOut()
{
    static int fade_cnt = 0;
    static int fade_cnt2 = 0;
    int ret;
    int frame;
    float alpha;

    ret = 0;
    frame = 30;

    if (fade_cnt2 > 0)
    {
        fade_cnt2 -= 1;

        if (fade_cnt2 <= 0)
        {
            fade_cnt2 = 0;
            ret = 1;
        }

        SceneSetSquare(1, 0.0f, 0.0f, 640.0f, 448.0f, 0, 0, 0, 0x80);

        return ret;
    }

    if (fade_cnt == 0)
    {
        AdpcmPreSceneFadeOut(30);
        SeStopAndBackup();

        fade_cnt = frame;
    }
    else
    {
        fade_cnt--;
    }

    alpha = (128.0f / frame) * (frame - fade_cnt);

    if (alpha < 0.0f)
    {
        alpha = 0.0f;
    }

    if (fade_cnt <= 0)
    {
        fade_cnt = 1;

        if (IsFadeoutEndAdpcmScene() != 0)
        {
            fade_cnt2 = 10;
            fade_cnt = 0;
            ingame_wrk.stts |= 0x28;
        }
    }

    SceneSetSquare(1, 0.0f, 0.0f, 640.0f, 448.0f, 0, 0, 0, alpha);

    return ret;
}

int black_time = 0;
int load_file_id = 0;
int ending_mode = 0;
int clear_disp = 0;
int clear_disp_mode = 0;
int first_clear_flg = 0;

void InterMissionInit()
{
    ingame_wrk.mode = INGAME_MODE_INTER_MSN;
    ingame_wrk.stts |= 0x20;
    ingame_wrk.msn_no += 1;
    ap_wrk.fgst_no = 0xff;
    ev_wrk.spev_tmp[0] = 0;
    effect_disp_flg = 1;

    EAdpcmFadeOut(60);
    AdpcmMapCtrlInit();
    SeStopAll();
    SavePointMenuOpenInit(1);
}

void InterMissionEnd()
{
    ingame_wrk.stts &= 0xdf;
    ev_wrk.spev_tmp[0] = 0;
    ev_wrk.next_msn = 1;

    InGameInit();

    ingame_wrk.mode = INGAME_MODE_MSN_TITLE;
    ap_wrk.fgst_no = 0xff;

    MissionTitleInit(ingame_wrk.msn_no);
}

void InterMissionMain()
{
    if (SavePointMenuMain(1) != 0 && IsEndAdpcmFadeOut() != 0)
    {
        InterMissionEnd();
    }
}

void DoorLockBattleSet()
{
    if (ev_wrk.btl_lock != 1)
    {
        LockAllDoorInNowRoom();

        ev_wrk.btl_lock = 1;
    }
}

void DoorLockBattleAfter()
{
    if (EnemyUseJudge(1) == 0)
    {
        if (ev_wrk.btl_lock == 1)
        {
            if (find_wrk.mode == 0 && ingame_wrk.mode != INGAME_MODE_GET_ITEM && plyr_wrk.mode == PMODE_MSG_DISP)
            {
                plyr_wrk.mode = PMODE_NORMAL;

                ReqEneStop(0,0);
            }

            UnlockAllDoorInNowRoom();

            ev_wrk.btl_lock = 0;
        }
        else if (ev_wrk.btl_lock == 2)
        {
            ev_wrk.btl_lock = 4;
        }
    }
}

void LockBattleDoorOpen(void)
{
    if (ev_wrk.btl_lock == 1 || ev_wrk.btl_lock == 2)
    {
        plyr_wrk.mode = PMODE_MSG_DISP;

        ReqEneStop(1, 0);
    }
    else if (ev_wrk.btl_lock == 4)
    {
        plyr_wrk.mode = 4;

        ReqEneStop(1, 0);
    }
}

int LockBattleAfterJudge()
{
  return ev_wrk.btl_lock == 4;
}

void LockBattleDoorOpenMSGDisp()
{
    switch(ev_wrk.btl_lock)
    {
        case BTL_LOCK_SET0:
        case BTL_LOCK_SET1:
            SetMessageV3((u_char *)GetIngameMSGAddr(6, 28),0x64000);

            if (MesStatusCheck() == 0)
            {
                plyr_wrk.mode = PMODE_NORMAL;
                ReqEneStop(0, 0);
            }
            else
            {
                ev_wrk.btl_lock = BTL_LOCK_SET_MSG;
            }
        break;
        case BTL_LOCK_SET_MSG:
            SetMessageV3((u_char *)GetIngameMSGAddr(6, 28), 0x64000);

            if (*key_now[5] == 1)
            {
                MessageWaitOff();
            }

            if (MesStatusCheck() == 0)
            {
                plyr_wrk.mode = PMODE_NORMAL;
                ReqEneStop(0,0);
                ev_wrk.btl_lock = BTL_LOCK_SET1;
            }
        break;
        case BTL_LOCK_AFTER:
            find_wrk.mode = 2;
            SetMessageV3((u_char *)GetIngameMSGAddr(6, 0x1d),0x64000);

            if (MesStatusCheck() == 0)
            {
                UnlockAllDoorInNowRoom();
                plyr_wrk.mode = PMODE_NORMAL;
                ReqEneStop(0, 0);
                ev_wrk.btl_lock = BTL_LOCK_NO_REQ;
            }
            else
            {
                ev_wrk.btl_lock = BTL_LOCK_AFTER_MSG;
            }
         break;
        case BTL_LOCK_AFTER_MSG:
            SetMessageV3((u_char *)GetIngameMSGAddr(6, 0x1d),0x64000);

            if (*key_now[5] == 1)
            {
                MessageWaitOff();
            }

            if (MesStatusCheck() == 0)
            {
                UnlockAllDoorInNowRoom();
                plyr_wrk.mode = PMODE_NORMAL;
                ReqEneStop(0, 0);
                ev_wrk.btl_lock = BTL_LOCK_NO_REQ;
                find_wrk.mode = 0;
            }
        break;
    }
}

int ScenePlay(short int scene_no)
{
    return EventSceneCtrl(scene_no) == 2;
}

void EndingInit()
{
    ingame_wrk.stts |= 0xa0;
    ev_wrk.spev_tmp[0] = 0;
    ingame_wrk.mode = INGAME_MODE_ENDING;
    ending_mode = 1;
    clear_disp = 0;
    clear_disp_mode = 0;
    first_clear_flg = 0;

    if (cam_custom_wrk.func_spe[2] == 0)
    {
        cam_custom_wrk.func_spe[2] = 1;
    }

    if (ingame_wrk.clear_count == 0)
    {
        first_clear_flg = 1;
        clear_disp_mode = 1;
        ingame_wrk.clear_count += 1;
    }

    else if ((cribo.clear_info & 2) != 0)
    {
        if (ingame_wrk.clear_count < 0x6c)
        {
            ingame_wrk.clear_count += 1;
        }

        if (ingame_wrk.difficult != 0)
        {
            if ((cribo.clear_info & 1) == 0)
            {
                clear_disp_mode = 2;
            }

            else if (ingame_wrk.clear_count == 3)
            {
                clear_disp_mode = 3;
            }

            cribo.clear_info = cribo.clear_info | 1;

            if (cam_custom_wrk.func_spe[3] == 0)
            {
                cam_custom_wrk.func_spe[3] = 1;
            }

        }
    }
    SavePointMenuOpenInit(2);

    return;
}

void EndingMain()
{
    switch(ending_mode)
    {
        case ENDING_MODE_MOVIE:
            if (ingame_wrk.difficult && cribo.clear_info & 2)
            {
                MoviePlay(92);
            }
            else
            {
                MoviePlay(91);
            }

            ingame_wrk.stts |= 0x20;
            ending_mode = ENDING_MODE_STAFF;
            break;

        case ENDING_MODE_STAFF:
            MoviePlay(96);
            ingame_wrk.stts |= 0x20;
            ending_mode = ENDING_MODE_CLEAR;
            SeBackupInit();
            break;

        case ENDING_MODE_CLEAR:
            if (CallStoryClear() != 0)
            {
                ending_mode = ENDING_MODE_SAVE;
            }
            break;

        case ENDING_MODE_SAVE:
            if (SavePointMenuMain(2) != 0)
            {
                EAdpcmFadeOut(60);
                sys_wrk.game_mode = GAME_MODE_OUTGAME;
                outgame_wrk.mode = OUTGAME_MODE_INIT;
            }
            break;
        }
}
