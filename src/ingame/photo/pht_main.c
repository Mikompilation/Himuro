#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "pht_main.h"

#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_oth.h"
#include "graphics/graph2d/effect_scr.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph2d/rare_ene.h"
#include "graphics/motion/mdlwork.h"
#include "ingame/entry/ap_pgost.h"
#include "ingame/event/ev_main.h"
#include "ingame/info/inf_disp.h"
#include "ingame/map/furn_ctl.h"
#include "ingame/map/furn_dat.h"
#include "ingame/map/map_area.h"
#include "ingame/photo/photo.h"
#include "ingame/photo/pht_main.h"
#include "ingame/photo/pht_make.h"
// #include "ingame/plyr/plyr_ctl.h" // FModeScreenEffect
#include "main/glob.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"

int after_timer = 0;

void PhotoCtrlInit()
{
    PhotoMakeSaveInit();
    InitPazEneTexLoad();
}

void PhotoCtrl()
{
    int i;
    static int load_id1;
    static int load_id2;
    static int btfl = 0;
    static int cnt = 0;
    static int fd_cnt = 0;
    static int no = -1;
    int fl;

    if (MesStatusCheck() == 0 && plyr_wrk.ap_timer != 0)
    {
        plyr_wrk.ap_timer--;
    }

    switch(photo_wrk.mode)
    {
    case PHOTO_MODE_PRE0:
        eff_dith_off = 1;
        load_id1 = -1;
        photo_wrk.mode = PHOTO_MODE_PRE1;
    break;
    case PHOTO_MODE_PRE1:
        pht_finder_off = 1;
        photo_wrk.mode = PHOTO_MODE_SCORE;
    /* case fall-through */
    case PHOTO_MODE_SCORE:
        load_id1 = SpecialPhotoMakeJudge();

        hint_2d = 0;
        hint_3d = 0;
        hint_rea = 0;

        btfl = (plyr_wrk.sta & 0x100) == 0 && (plyr_wrk.sta & 0x1);

        for (i = 0; i < 4; i++)
        {
            if (photo_wrk.spcl_pht_no[i][0] == 6)
            {
                if (btfl == 0 || photo_wrk.ene_dead != 0xff)
                {
                    hint_2d = 1;
                    hint_2d_num = photo_wrk.spcl_pht_no[i][1];

                    if (hint_2d_num >= 150 && hint_2d_num < 200)
                    {
                        hint_rea = 1;
                    }
                }
            }

            if (photo_wrk.spcl_pht_no[i][0] == 7 && btfl == 0)
            {
                hint_3d = 1;
                hint_3d_num = photo_wrk.spcl_pht_no[i][1];
            }
        }

        photo_wrk.score = PhotoScoreCount();

        if (ingame_wrk.pht_cnt < 9999999)
        {
            ingame_wrk.pht_cnt++;
        }

        if (hint_3d == 0)
        {
            if (btfl != 0)
            {
                SetWhiteIn2(1);
            }
            else
            {
                SetWhiteIn2(16);
            }
        }
        else
        {
            FurnPicExecStart();
        }

        if (load_id1 != -1)
        {
            photo_wrk.mode = PHOTO_MODE_HINT_LOAD;
        }
        else
        {
            FModeScreenEffect();

            photo_wrk.mode = PHOTO_MODE_CNG_PHOTO;
        }
    break;
    case PHOTO_MODE_HINT_LOAD:
        if (IsLoadEnd(load_id1) != 0)
        {
            FModeScreenEffect();

            photo_wrk.mode = PHOTO_MODE_CNG_PHOTO;
        }
    break;
    case PHOTO_MODE_CNG_PHOTO:
        ReqSavePicture();

        photo_wrk.mode = PHOTO_MODE_MAKE_DISP;

        if (plyr_wrk.sta & 0x100)
        {
            plyr_wrk.ap_timer = 60;
        }
        else if (btfl != 0)
        {
            plyr_wrk.ap_timer = 5;
        }
        else
        {
            plyr_wrk.ap_timer = 130;
        }

        pfile_wrk.pic_num++;

        AddPhotoData();

        if (hint_3d == 0)
        {
            furn_disp_flg = 0;
        }
        else
        {
            SetWhiteIn2(16);
        }
    break;
    case PHOTO_MODE_MAKE_DISP:
        if (plyr_wrk.ap_timer == 0)
        {
            if (btfl != 0)
            {
                pht_finder_off = 0;
                photo_wrk.mode = PHOTO_MODE_CAP;
            }
            else if (hint_2d != 0 || hint_3d != 0)
            {
                if (hint_rea != 0)
                {
                    SeStartFix(7, 0, 0x1000, 0x7a0, 0);
                }
                else
                {
                    SeStartFix(10, 0, 0x1000, 0x1000, 0);
                }

                photo_wrk.mode = PHOTO_MODE_DISP_WAIT1;

                plyr_wrk.ap_timer = 100;
            }
            else
            {
                photo_wrk.mode = PHOTO_MODE_CAP;

                ingame_wrk.stts |= 8;
                ingame_wrk.mode = INGAME_MODE_PHOTO_AFTER;

                pht_finder_off = 0;
            }

            if (no >= 0)
            {
                ene_wrk[no].tr_rate = 0;
            }
        }
    break;
    case PHOTO_MODE_DISP_WAIT1:
        if (plyr_wrk.ap_timer == 0)
        {
            if (hint_3d == 0 && hint_rea == 0)
            {
                photo_wrk.mode = PHOTO_MODE_DISP_WAIT2;
                plyr_wrk.ap_timer = 100;
            }
            else
            {
                photo_wrk.mode = PHOTO_MODE_CAP;
                pht_finder_off = 0;

                if (no >= 0)
                {
                    ene_wrk[no].tr_rate = 0;
                }
            }
        }
    break;
    case PHOTO_MODE_DISP_WAIT2:
        if (plyr_wrk.ap_timer == 0)
        {
            photo_wrk.mode = PHOTO_MODE_CAP;
            pht_finder_off = 0;

            if (no >= 0)
            {
                ene_wrk[no].tr_rate = 0;
            }
        }
    break;
    case PHOTO_MODE_CAP:
        FurnPicExecEnd();

        photo_wrk.mode = PHOTO_MODE_END;

        cnt = btfl != 0 ? 1 : 20;

        SendManMdlTex();

        eff_dith_off = 0;
    break;
    case PHOTO_MODE_END:
        if (cnt == 1)
        {
            load_id2 = -1;

            if (hint_2d != 0 && hint_2d_num < 100 && pazene_load_tbl[hint_2d_num] != -1)
            {
                for (i = 0, fl = -1; i < 2 && fl == -1; i++)
                {
                    if (((pazene_load_buf[i] & 0x80) != 0) && ((pazene_load_buf[i] & 0x7f) == pazene_load_tbl[hint_2d_num]))
                    {
                        fl = i;
                    }
                }

                if (fl == -1)
                {
                    for (i = 0, fl = -1; i < 2 && fl == -1; i++)
                    {
                        if (pazene_load_buf[i] == 0xff)
                        {
                            fl = i;

                            pazene_load_buf[i] = pazene_load_tbl[hint_2d_num] | 0x80;
                            area_wrk.rgst[i] = pazene_load_tbl[hint_2d_num] | 0x80;

                            break;
                        }
                    }

                    if (fl != -1)
                    {
                        i = pazene_load_buf[fl] & 0x7f;

                        load_id2 = LoadReq(pazz_textbl[i], 0x1c90000 + fl * 0x10000);
                    }
                }
            }

            ingame_wrk.stts &= ~0x8;
            ingame_wrk.mode = INGAME_MODE_NOMAL;

            if (photo_wrk.rare[0].no >= 0x80 && photo_wrk.rare[0].score != 0)
            {
                PuzzleGhostPhotoReq();
                InfoDispPause();

                photo_wrk.mode = PHOTO_MODE_AFTER;

                ingame_wrk.stts |= 0x8;
                ingame_wrk.mode = INGAME_MODE_PHOTO_AFTER;

                after_timer = 260;

                SetDebugMenuSwitch(1);

                break;
            }

            for (i = 0; i < 5; i++)
            {
                if (photo_wrk.furn[i].no != 0xffff && photo_wrk.furn[i].type == 32)
                {
                    if (PhotoEventOpenJudge(furn_dat[photo_wrk.furn[i].no].fp_mak, 0xff) != 0)
                    {
                        break;
                    }
                }
                else if (photo_wrk.furn[i].no != 0xffff && photo_wrk.furn[i].type == 64)
                {
                    if (PhotoEventOpenJudge(0xff, photo_wrk.furn[i].no) != 0)
                    {
                        break;
                    }
                }
            }
        }

        if (cnt == 0)
        {
            if (load_id2 == -1)
            {
                photo_wrk.mode = PHOTO_MODE_NO_REQ;
            }
            else if (IsLoadEnd(load_id2) != 0)
            {
                photo_wrk.mode = PHOTO_MODE_NO_REQ;
            }
        }
        else
        {
            cnt--;
        }
    break;
    case PHOTO_MODE_AFTER:
        if (after_timer != 0)
        {
            after_timer--;
        }
        else
        {
            SetDebugMenuSwitch(0);

            photo_wrk.mode = PHOTO_MODE_NO_REQ;

            ingame_wrk.stts &= ~0x8;
            ingame_wrk.mode = INGAME_MODE_NOMAL;

            FinderEndSet();
            InfoDispRestart();

            for (i = 0; i < 2; i++)
            {
                if (photo_wrk.rare[0].no == pazene_load_buf[i])
                {
                    pazene_load_buf[i] = 0xff;
                    area_wrk.rgst[i] = 0xff;
                }
            }
        }

        SetAmuletFire();

        if (after_timer == 220)
        {
            CallAmuletFire();
        }
    break;
    }
}


int pazene_load_buf[2] = { 0, 0 };

void InitPazEneTexLoad()
{
    int i;

    for (i = 0; i < 2; i++)
    {
        pazene_load_buf[i] = area_wrk.rgst[i];
    }
}
