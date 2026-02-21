#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "photo.h"

#include "ingame/map/furn_dat.h"
#include "ingame/menu/ig_glst.h"
#include "ingame/photo/photo.h"
#include "ingame/plyr/time_ctl.h"
#include "main/glob.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "outgame/btl_mode/btl_mode.h"

long int PhotoScoreCount()
{
    long int score;
    int i;
    float point;
    float point_2d;
    int j;

    point = 0.0f;
    point_2d = 0.0f;

    if (photo_wrk.plyr.no != 0xffff)
    {
        photo_wrk.plyr.score = SubjectScoreCount(&photo_wrk.plyr);

        point += photo_wrk.plyr.score;
    }

    for (i = 0; i < 4; i++)
    {
        if (photo_wrk.ene[i].no == 0xffff)
        {
            continue;
        }

        photo_wrk.ene[i].score = SubjectScoreCount(&photo_wrk.ene[i]);

        point += photo_wrk.ene[i].score;

#ifdef MATCHING_DECOMP
        {
            int ii = i;

            for (j = 0; j < 3; j++)
            {
                if (ene_wrk[j].dat_no == photo_wrk.ene[i].no && (score = ene_wrk[j].type) == photo_wrk.ene[ii].type)
                {
                    ene_wrk[j].type += 0;

                    break;
                }
            }
        }
#endif
    }

    for (i = 0; i < 5; i++)
    {
        if (photo_wrk.furn[i].no == 0xffff)
        {
            continue;
        }

        if (CheckGlistRareGhost(4, photo_wrk.furn[i].no) != 0)
        {
            if (photo_wrk.furn[i].type != 0x20)
            {
                photo_wrk.furn[i].score = SubjectScoreCount(&photo_wrk.furn[i]);

                point += photo_wrk.furn[i].score;
            }
            else
            {
                photo_wrk.furn[i].score = photo_wrk.furn[i].point;

                point_2d += photo_wrk.furn[i].point;
            }
        }
    }

    for (i = 0; i < 3; i++)
    {
        if (photo_wrk.rare[i].no != 0xffff)
        {
            photo_wrk.rare[i].score = SubjectScoreCount(&photo_wrk.rare[i]);

            point += photo_wrk.rare[i].score;
        }
    }

    point *= photo_wrk.ratio;

    score = point + point_2d;

    if (score >= 10000)
    {
        score = 9999;
    }

    if (photo_wrk.falth != 0)
    {
        score = 0;
    }

    cam_custom_wrk.point += score;

    if (cam_custom_wrk.point > 9999999)
    {
        cam_custom_wrk.point = 9999999;
    }

    if (ingame_wrk.game == 0x0)
    {
        ingame_wrk.total_point += score;

        if (ingame_wrk.total_point > 9999999)
        {
            ingame_wrk.total_point = 9999999;
        }

        if (score > ingame_wrk.high_score)
        {
            ingame_wrk.high_score = score;
        }
    }
    else
    {
        if (ingame_wrk.game == 1)
        {
            BattleModeScoreRecord(score);
        }
    }

    return score;
}

int SubjectScoreCount(SUBJECT_WRK *sj)
{
    float score;

    score = sj->point * sj->ratio;

    return score;
}

int SpecialPhotoMakeJudge()
{
    int count;
    int ret;
    u_char no;
    static int hint_tbl[48] = {
        HINT_00_PK2, HINT_01_PK2, HINT_02_PK2, HINT_03_PK2, HINT_04_PK2, HINT_05_PK2, HINT_06_PK2, HINT_07_PK2,
        HINT_08_PK2, HINT_09_PK2, HINT_10_PK2, HINT_11_PK2, HINT_12_PK2, HINT_13_PK2, HINT_14_PK2, HINT_15_PK2,
        HINT_16_PK2, HINT_17_PK2, HINT_18_PK2, HINT_19_PK2, HINT_20_PK2, HINT_21_PK2, HINT_22_PK2, HINT_23_PK2,
        HINT_24_PK2, HINT_25_PK2, HINT_26_PK2, HINT_27_PK2, HINT_28_PK2, HINT_29_PK2, HINT_30_PK2, HINT_31_PK2,
        HINT_32_PK2, HINT_33_PK2, HINT_34_PK2, HINT_35_PK2, HINT_36_PK2, HINT_37_PK2, HINT_38_PK2, HINT_39_PK2,
        HINT_40_PK2, HINT_41_PK2, HINT_42_PK2, HINT_43_PK2, HINT_44_PK2, HINT_45_PK2, HINT_46_PK2, HINT_47_PK2,
    };
    static int hint_rea_tbl[25] = {
        REA_00_PK2, REA_01_PK2, REA_02_PK2, REA_03_PK2, REA_04_PK2,
        REA_05_PK2, REA_06_PK2, REA_07_PK2, REA_08_PK2, REA_09_PK2,
        REA_10_PK2, REA_11_PK2, REA_12_PK2, REA_13_PK2, REA_14_PK2,
        REA_15_PK2, REA_16_PK2, REA_17_PK2, REA_18_PK2, REA_19_PK2,
        REA_20_PK2, REA_21_PK2, REA_22_PK2, REA_23_PK2, REA_24_PK2,
    };
    int i;

    ret = -1;

    count = 0;

    if (photo_wrk.falth != 0)
    {
        photo_wrk.spcl_pht_no[0][0] = 5;
        photo_wrk.spcl_pht_no[0][1] = 1;

        count = 1;
    }

    no = 0xff;

    if (photo_wrk.ene_dead != no)
    {
        no = photo_wrk.ene_dead;

        if (no >= 150 && no < 200)
        {
            ret = LoadReq(hint_rea_tbl[no - 150], 0x1e90000);
        }
        else
        {
            ret = LoadReq(hint_tbl[no], 0x1e90000);
        }

        photo_wrk.spcl_pht_no[count][0] = 6;
        photo_wrk.spcl_pht_no[count][1] = no;

        count++;
    }

    if (no == 0xff)
    {
        for (i = 0; i < 5; i++)
        {
            if (photo_wrk.furn[i].type == 32)
            {
                no = furn_dat[photo_wrk.furn[i].no].fp_mak;

                if (no >= 150 && no < 200)
                {
                    ret = LoadReq(hint_rea_tbl[no - 150], 0x1e90000);
                }
                else
                {
                    ret = LoadReq(hint_tbl[no], 0x1e90000);
                }

                photo_wrk.spcl_pht_no[count][0] = 6;
                photo_wrk.spcl_pht_no[count][1] = no;

                count++;

                break;
            }
        }
    }

    for (i = 0; i < 5; i++)
    {
        if (photo_wrk.furn[i].type == 64)
        {
            photo_wrk.spcl_pht_no[count][0] = 7;
            photo_wrk.spcl_pht_no[count][1] = furn_dat[photo_wrk.furn[i].no].fp_mak;

            break;
        }
    }

    return ret;
}

void AddPhotoData()
{
    int i;
    int dust;

    dust = 0;

    if (pfile_wrk.pic_num > 24)
    {
        for (i = 0; i < 23; i++)
        {
            if ((pfile_wrk.pic[i].status & 0x2) == 0)
            {
                dust = 1;
            }

            if (dust)
            {
                pfile_wrk.pic[i] = pfile_wrk.pic[i+1];
            }
        }

        pfile_wrk.pic_num = 24;
    }

    pfile_wrk.pic[pfile_wrk.pic_num-1].adr_no = photo_wrk.adr_no;
    pfile_wrk.pic[pfile_wrk.pic_num-1].score = photo_wrk.score;
    pfile_wrk.pic[pfile_wrk.pic_num-1].room = photo_wrk.room;
    pfile_wrk.pic[pfile_wrk.pic_num-1].msn_no = ingame_wrk.msn_no;
    pfile_wrk.pic[pfile_wrk.pic_num-1].status = 1;
    pfile_wrk.pic[pfile_wrk.pic_num-1].time = sys_wrk.rtc;

    SetNowClock(&pfile_wrk.pic[pfile_wrk.pic_num-1].time);
    GetRecordSubject(&pfile_wrk.pic[pfile_wrk.pic_num-1]);
}

int GetSavePhotoNo()
{
    int i;

    for (i = 0; i < 24; i++)
    {
        if ((pfile_wrk.pic[i].status & 0x1) == 0)
        {
            return pfile_wrk.pic[i].adr_no;
        }
    }

    for (i = 0; i < 23; i++)
    {
        if ((pfile_wrk.pic[i].status & 0x2) == 0)
        {
            return pfile_wrk.pic[i].adr_no;
        }
    }

    return pfile_wrk.pic[23].adr_no;
}

void DeletePhotoData(u_char no)
{
    int i;
    int dust;

    dust = pfile_wrk.pic[no].adr_no;

    for (i = no; i < 23; i++)
    {
        pfile_wrk.pic[i] = pfile_wrk.pic[i+1];
    }

    pfile_wrk.pic[23].status = 0;
    pfile_wrk.pic[23].adr_no = dust;

    pfile_wrk.pic_num--;
}

void GetRecordSubject(PICTURE_WRK *pic)
{
    int i;
    int sort[16][3];
    int tmp[3];
    int j;

    for (i = 0; i < 4; i++)
    {
        sort[i][0] = photo_wrk.ene[i].score;
        sort[i][1] = photo_wrk.ene[i].type + 1;
        sort[i][2] = photo_wrk.ene[i].no;
    }

    for (i = 4; i < 9; i++)
    {
        sort[i][0] = photo_wrk.furn[i-4].score;
        sort[i][1] = 4;
        sort[i][2] = photo_wrk.furn[i-4].no;
    }

    for (i = 9; i < 12; i++)
    {
        sort[i][0] = photo_wrk.rare[i-9].score;
        sort[i][1] = 5;
        sort[i][2] = photo_wrk.rare[i-9].no;
    }

#ifdef MATCHING_DECOMP
    for (i = 12; i < 16U; i++)
#else
    for (i = 12; i < 16; i++)
#endif
    {
        sort[i][0] = photo_wrk.spcl_pht_score[i-12];
        sort[i][1] = 6;
        sort[i][2] = photo_wrk.ene[i].no; // bug??
    }

    for (i = 0; i < 16; i++)
    {
        for (j = i+1; j < 16; j++)
        {
            if (sort[i][0] < sort[j][0])
            {
                tmp[0] = sort[i][0];
                tmp[1] = sort[i][1];
                tmp[2] = sort[i][2];

                sort[i][0] = sort[j][0];
                sort[i][1] = sort[j][1];
                sort[i][2] = sort[j][2];

                sort[j][0] = tmp[0];
                sort[j][1] = tmp[1];
                sort[j][2] = tmp[2];

                j = i+1;
            }
        }
    }

    for (i = 0; i < 3; i++)
    {
        if (sort[i][0] != 0)
        {
            pic->subject[i][0] = sort[i][1];
            pic->subject[i][1] = sort[i][2];
        }
        else
        {
            pic->subject[i][0] = 0;
            pic->subject[i][1] = 0;
        }
    }
}
