#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "sound_test.h"

// #include <cstdlib.h>
// RAND_MAX = (2**31-1)
#define RAND_MAX 2147483647

#include "graphics/graph2d/tim2.h"
#include "graphics/graph3d/sglib.h"
#include "ingame/info/inf_disp.h"
#include "ingame/menu/ig_file.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/item.h"
#include "main/glob.h"
// #include "os/eeiop/adpcm/ea_cmd.h"
#include "os/eeiop/adpcm/ea_ctrl.h"
#include "os/eeiop/adpcm/ea_dat.h"
#include "os/eeiop/eeiop.h"
#include "os/eeiop/eese.h"
#include "outgame/mode_slct.h"
#include "outgame/sound_test.h"

typedef struct {
    u_short bgm_no;
    u_short bgm_vol;
    u_short bgm_no_bak;
    u_short mst_vol;
    u_char csr[1];
    u_char run[6];
    u_char csr_alp[6];
    u_char run_alp[6];
    u_char run_flsh[6];
    u_char num_tmr[3];
    u_char ply_flm;
    u_char ply_min;
    u_char ply_sec;
    u_char msg_no;
} ST_F;

typedef struct {
    short int lvl_now;
    short int lvl_bak;
    u_char tmr;
    u_char beat;
} BEAT_DSP;

u_short music_list[] = {
    AO000_TITLE_STR,
    AO002_SHOU_TITLE_STR,
    AB003_STR,
    AB005_STR,
    AB004_STR,
    AB001_STR,
    AB002_STR,
    AB010_STR,
    AB012_STR,
    AB013_STR,
    AB014_STR,
    AB015_STR,
    AB016_STR,
    AB019_STR,
    AB027_STR,
    AY019_BOUREI_STR,
    AY010_HENREI_STR,
    AY011_JYOREI_STR,
    AY012_SAKKAREI_STR,
    AY013_TENAGA_STR,
    AY020_SAKASA_STR,
    AY021_KUBI_STR,
    AY014_KAMIONNA_STR,
    AY015_KOMUSO_STR,
    AY016_MINREI_STR,
    AY033_MINTSUMA_STR,
    AY018_MAYOIGO1_STR,
    AY022_MAYOIGO2_STR,
    AY023_MAYOIGO3_STR,
    AY024_MEKAKUSHI_STR,
    AY025_RTE_STR,
    AY027_KONNA_STR,
    AY028_KOTOKO_STR,
    AY031_HAIREI_STR,
    AY046_SYOUKI1_STR,
    AY032_SHINKAN1_STR,
    AY038_NAWAMIKO_STR,
    AY037_TOUSHU_STR,
    AY039_MAGAONLY_STR,
    AB026_STR,
    AB018_STR,
    AB000_STR,
    AB028_STR,
    AB029_STR,
    AB030_STR,
    AE002_KOTO_STR,
    AO003_GAMEOVER_STR,
    AO001_STAFF_STR,
};

typedef struct {
    u_char beat;
    u_char max;
    u_char min;
    u_char drop;
} BEAT_DATA;

static BEAT_DSP bb;
static char playback_part2;

static ST_F stf;

#define PI 3.1415927f

void SoundTestForModeSlectInit()
{
    stf = (ST_F){0};

    stf.bgm_no_bak = 0;
    stf.bgm_no = 0;
    stf.mst_vol = GetAdpcmVol(music_list[0]);
    stf.bgm_vol = opt_wrk.bgm_vol;
    stf.csr[0] = 2;
    stf.ply_flm = 0;
    stf.ply_sec = 0;
    stf.ply_min = 0;

    bb.lvl_now = 0;

    stf.run[2] = 0;
    stf.msg_no = 0xff;

    playback_part2 = 0;
}

char SoundTestForModeSlect()
{
    char set_flg;
    char rtrn;

    rtrn = 0;

    GetIopStatP();

    stf.run[0] = 0;
    stf.run[1] = 0;
    stf.run[3] = 0;
    stf.run[4] = 0;
    stf.run[5] = 0;

    if (*key_now[4] == 1)
    {
        rtrn = 1;
        EAdpcmFadeOut(15);
    }
    else if (*key_now[5] != 0)
    {
        stf.msg_no = 0xff;
        switch(stf.csr[0])
        {
        case 0:
            stf.run[0] = 1;
            if (stf.bgm_vol >= 0x21) stf.bgm_vol -= 0x20;
            else if (stf.bgm_vol != 0) stf.bgm_vol = 0;
            SeSetMVol((stf.bgm_vol * 0xfff) / 4095.0f);
        break;
        case 1:
            if (stf.run[2] == 2)
            {
                stf.run[1] = 1;
                if (*key_now[5] == 1) {
                    stf.run_flsh[1] = 0;
                    if (stf.bgm_no != 0) stf.bgm_no--;
                    EAdpcmCmdStop(0, 0, 0);
                    if (playback_part2 == 0)
                    {
                        playback_part2 = 1;
                    }
                }
            }
            else
            {
                stf.run[1] = 1;
                if (*key_now[5] == 1)
                {
                    stf.run_flsh[1] = 0;
                    if (stf.bgm_no != 0) stf.bgm_no--;
                }
            }
            break;
        case 2:
            if (*key_now[5] == 1)
            {
                bb.lvl_now = 0;
                stf.ply_min = stf.ply_sec = stf.ply_flm = 0;
            }
            stf.run[2] = 1;
            stf.msg_no = 7;
            stf.mst_vol = GetAdpcmVol(music_list[stf.bgm_no]);
            EAdpcmCmdPlay(0, 0, music_list[stf.bgm_no], 0, stf.mst_vol, 0x280, 4095, 0);
        break;
        case 3:
            if (*key_now[5] == 1)
            {
                bb.lvl_now = 0;
                stf.run_flsh[3] = 0;
                stf.ply_min = stf.ply_sec = stf.ply_flm = 0;
            }
            stf.run[3] = 1;
            stf.run[2] = 0;
            EAdpcmCmdStop(0, 0, 0);
        break;
        case 4:
            if (stf.run[2] == 2)
            {
                stf.run[4] = 1;
                if (*key_now[5] == 1)
                {
                    stf.run_flsh[4] = 0;
                    if (stf.bgm_no <= 46)
                    {
                        stf.bgm_no++;
                    }
                    EAdpcmCmdStop(0, 0, 0);
                    if (playback_part2 == 0)
                    {
                        playback_part2 = 1;
                    }
                }
            }
            else
            {
                stf.run[4] = 1;
                if (*key_now[5] == 1)
                {
                    stf.run_flsh[4] = 0;
                    if (stf.bgm_no <= 46) stf.bgm_no++;
                }
            }
        break;
        case 5:
            stf.run[5] = 1;
            if (stf.bgm_vol <= 4062) stf.bgm_vol += 32;
            else if (stf.bgm_vol != 4095) stf.bgm_vol = 4095;
            SeSetMVol((stf.bgm_vol * 4095) / 4095.0f);
        break;
        }
    }
    else
    {
        if (
            *key_now[2] == 1 ||
            (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
            Ana2PadDirCnt(3) == 1 ||
            (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
        )
        {
            stf.msg_no = 0xff;
            if (stf.csr[0] != 0)
            {
                stf.csr[0]--;
            }
            else
            {
                stf.csr[0] = 5;
            }
        }
        else if (
            *key_now[3] == 1 ||
            (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
            Ana2PadDirCnt(1) == 1 ||
            (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
        )
        {
            stf.msg_no = 0xff;
            if (stf.csr[0] <= 4)
            {
                stf.csr[0]++;
            }
            else
            {
                stf.csr[0] = 0;
            }
        }
    }

    if (playback_part2 != 0)
    {
        if (IsEndAdpcmFadeOut() != 0)
        {
            stf.mst_vol = GetAdpcmVol(music_list[stf.bgm_no]);
            EAdpcmCmdPlay(0, 0, music_list[stf.bgm_no], 0, stf.mst_vol, 640, 4095, 0);
            stf.run[2] = 1;
            stf.ply_min = stf.ply_sec = stf.ply_flm = 0;
            bb.lvl_now = 0;
            playback_part2 = 0;
        }
    }

    if (stf.run[2] == 1)
    {
        if (EAGetRetStat() > 5)
        {
            stf.run[2] = 2;
        }
    }
    else if (stf.run[2] == 2 && IsEndAdpcmFadeOut() != 0)
    {
        stf.run[2] = 0;
        stf.ply_min = stf.ply_sec = stf.ply_flm = 0;
        bb.lvl_now = 0;
    }

    if (stf.msg_no == 0xff)
    {
        stf.msg_no = stf.csr[0];
        if (stf.csr[0] == 2 && stf.run[2] == 2)
        stf.msg_no = 7;
    }

    return rtrn;
}

void SoundTestForModeSlectDisp(u_char alp, float flsh)
{
    int i;
    u_char dir;

    for (i = 0; i < 6; i++)
    {
        switch(i)
        {
        case 0:
            if (stf.csr[0] == 0)
            {
                if (stf.run[0] == 1)
                {
                    if (stf.csr_alp[0] + 8 < alp)
                    {
                        stf.csr_alp[0] += 8;
                    }
                    else
                    {
                        stf.csr_alp[0] = alp;
                    }

                    stf.run_alp[0] = alp * flsh;
                }
                else
                {
                    stf.csr_alp[0] = alp * flsh;

                    if (stf.run_alp[0] - 8 > 0)
                    {
                        stf.run_alp[0] -= 8;
                    }
                    else
                    {
                        stf.run_alp[0] = 0;
                    }
                }
            }
            else
            {
                if (stf.csr_alp[0] - 8 > 0)
                {
                    stf.csr_alp[0] -= 8;
                }

                if (stf.run_alp[0] - 8 > 0)
                {
                    stf.run_alp[0] -= 8;
                }
            }
        break;
        case 1:
            if (stf.csr[0] == 0x1)
            {
                if (stf.run[1] == 0x1)
                {
                    if (stf.csr_alp[1] + 8 < alp)
                    {
                        stf.csr_alp[1] += 8;
                    }
                    else
                    {
                        stf.csr_alp[1] = alp;
                    }

                    if (stf.run_flsh[1] < 30)
                    {
                        stf.run_flsh[1]++;
                    }

                    stf.run_alp[1] = alp * SgSinf((stf.run_flsh[1] * PI) / 30.0f);
                }
                else
                {
                    stf.csr_alp[1] = alp * flsh;

                    if (stf.run_alp[1] - 8 > 0)
                    {
                        stf.run_alp[1] -= 8;
                    }
                    else
                    {
                        stf.run_alp[1] = 0;
                    }
                }
            }
            else
            {
                if (stf.csr_alp[1] - 8 > 0)
                {
                    stf.csr_alp[1] -= 8;
                }

                if (stf.run_alp[1] - 8 > 0)
                {
                    stf.run_alp[1] -= 8;
                }
            }
        break;
        case 2:
            if (stf.csr[0] == 2)
            {
                if (stf.run[2] != 0)
                {
                    if (stf.csr_alp[2] + 8 < alp)
                    {
                        stf.csr_alp[2] += 8;
                    }
                    else
                    {
                        stf.csr_alp[2] = alp;
                    }
                }
                else
                {
                    stf.csr_alp[2] = alp * flsh;
                }
            }
            else
            {
                if (stf.csr_alp[2] - 8 > 0)
                {
                    stf.csr_alp[2] -= 8;
                }
            }

            if (stf.run[2] != 0)
            {
                stf.run_alp[2] = alp * flsh;
            }
            else
            {
                if (stf.run_alp[2] - 8 > 0)
                {
                    stf.run_alp[2] -= 8;
                }
                else
                {
                    stf.run_alp[2] = 0;
                }
            }
        break;
        case 3:
            if (stf.csr[0] == 3)
            {
                if (stf.run[3] == 1)
                {
                    if (stf.csr_alp[3] + 8 < alp)
                    {
                        stf.csr_alp[3] += 8;
                    }
                    else
                    {
                        stf.csr_alp[3] = alp;
                    }

                    if (stf.run_flsh[3] < 30)
                    {
                        stf.run_flsh[3]++;
                    }

                    stf.run_alp[3] = alp * SgSinf((stf.run_flsh[3] * PI) / 30.0f);
                }
                else
                {
                    stf.csr_alp[3] = alp * flsh;

                    if (stf.run_alp[3] - 8 > 0)
                    {
                        stf.run_alp[3] -= 8;
                    }
                    else
                    {
                        stf.run_alp[3] = 0;
                    }
                }
            }
            else
            {
                if (stf.csr_alp[3] - 8 > 0)
                {
                    stf.csr_alp[3] -= 8;
                }

                if (stf.run_alp[3] - 8 > 0)
                {
                    stf.run_alp[3] -= 8;
                }
            }
        break;
        case 4:
            if (stf.csr[0] == 4)
            {
                if (stf.run[4] == 1)
                {
                    if (stf.csr_alp[4] + 8 < alp)
                    {
                        stf.csr_alp[4] += 8;
                    }
                    else
                    {
                        stf.csr_alp[4] = alp;
                    }

                    if (stf.run_flsh[4] < 0x1e)
                    {
                        stf.run_flsh[4]++;
                    }

                    stf.run_alp[4] = alp * SgSinf((stf.run_flsh[4] * PI) / 30.0f);
                }
                else
                {
                    stf.csr_alp[4] = alp * flsh;

                    if (stf.run_alp[4] - 8 > 0)
                    {
                        stf.run_alp[4] -= 8;
                    }
                    else
                    {
                        stf.run_alp[4] = 0;
                    }
                }
            }
            else
            {
                if (stf.csr_alp[4] - 8 > 0)
                {
                    stf.csr_alp[4] -= 8;
                }

                if (stf.run_alp[4] - 8 > 0)
                {
                    stf.run_alp[4] -= 8;
                }
            }
        break;
        case 5:
            if (stf.csr[0] == 5)
            {
                if (stf.run[5] == 1)
                {
                    if (stf.csr_alp[5] + 8 < alp)
                    {
                        stf.csr_alp[5] += 8;
                    }
                    else
                    {
                        stf.csr_alp[5] = alp;
                    }

                    stf.run_alp[5] = alp * flsh;
                }
                else
                {
                    stf.csr_alp[5] = alp * flsh;

                    if (stf.run_alp[5] - 8 > 0)
                    {
                        stf.run_alp[5] -= 8;
                    }
                    else
                    {
                        stf.run_alp[5] = 0;
                    }
                }
            }
            else
            {
                if (stf.csr_alp[5] - 8 > 0)
                {
                    stf.csr_alp[5] -= 8;
                }

                if (stf.run_alp[5] - 8 > 0)
                {
                    stf.run_alp[5] -= 8;
                }
            }
        break;
        }
    }

    for (i = 0; i < 6; i++)
    {
        PutChrOne(i + 81, 0, 0, 0x80, stf.csr_alp[i], 0);
        PutChrOne(i + 75, 0, 0, 0x80, stf.run_alp[i], 0);
    }

    PutChrOne(89, (int)((stf.bgm_vol * 0xb0) / 4095.0f) & 0xff, 0, 0x80, alp, 0x0);
    ShockWave(0, alp);

    if (stf.bgm_no_bak < stf.bgm_no)
    {
        dir = 0;
    }
    else if (stf.bgm_no < stf.bgm_no_bak)
    {
        dir = 1;
    }

    if (stf.bgm_no_bak != stf.bgm_no)
    {
        if (stf.num_tmr[0] != 0)
        {
            if (stf.num_tmr[0] != 1)
            {
                stf.num_tmr[0]--;
            }
            else
            {
                stf.bgm_no_bak = stf.bgm_no;
                stf.num_tmr[0] = 10;
            }
        }
        else
        {
            stf.num_tmr[0] = 10;
        }
    }
    else
    {
        stf.num_tmr[0] = 10;
    }

    SlotNum(stf.bgm_no_bak, 2, 10 - stf.num_tmr[0], 10, 314, 122, 122, 147, alp, 0, dir);

    if (stf.run[2] == 2)
    {
        if (stf.ply_flm < 60)
        {
            stf.ply_flm++;
        }
        else
        {
            stf.ply_flm = 0;

            if (stf.ply_sec < 59)
            {
                stf.ply_sec++;
            }
            else if (stf.ply_sec < 60)
            {
                stf.ply_sec = 0;
                stf.ply_min++;
            }
            else if (stf.ply_min < 59)
            {
                stf.ply_min++;
            }
            else if (stf.ply_min < 60)
            {
                stf.ply_min = 0;
            }
        }
    }
    else
    {
        stf.ply_flm = 0;
        stf.ply_min = 0;
        stf.ply_sec = 0;
    }

    PutChrOne(100, 0, 0, 0x80, alp, 0x0);
    SlotNum(stf.ply_sec, 2, stf.ply_flm, 60, 324, 180, 180, 205, alp, 1, 0);

    if (stf.ply_sec == 0x3b)
    {
        SlotNum(stf.ply_min, 0x2, stf.ply_flm, 60, 273, 180, 180, 205, alp, 1, 0);
    }
    else
    {
        SlotNum(stf.ply_min, 2, 0, 0, 273, 180, 180, 205, alp, 1, 0);
    }

    if (stf.msg_no != 0xff)
    {
        PutStringYW(52, stf.msg_no, 80, 357, 0x808080, alp, 0x1000, 0);
    }

    SetSprFile(0x1ce0000);
    DispCaption(5, alp);
}

void SlotNum(int number, u_char digit, u_char timer, u_char tim_lmt, short int pos_x, short int pos_y, short int lmt_upp, short int lmt_dwn, u_char alp, u_char time_mode, u_char ud_sw)
{
    int i;
    int multi10;
    int multi10_bak;
    int dsp_num;
    u_char dsp_mode;
    int disp_digit;
    u_short limit;

    limit = tim_lmt;

    multi10 = 1;

    for (i = 0; i < digit; i++)
    {
        multi10 *= 10;
        multi10_bak = multi10;
    }

    number = number % multi10;

    disp_digit = 1;

    multi10 = multi10_bak;

    for (i = 0; i < digit; i++)
    {
        multi10 /= 10;

        dsp_mode = 0;

        dsp_num = ((number % (multi10 * 10)) / multi10);

        if (timer != 0)
        {
            if (limit >= timer)
            {
                if (i != digit - disp_digit)
                {
                    if (ud_sw == 0)
                    {
                        dsp_mode = ((number / (multi10 / 10)) % 10 != 9) ? dsp_mode: disp_digit;
                    }
                    else
                    {
                        dsp_mode = ((number / (multi10 / 10)) % 10 != 0) ? dsp_mode : 2;
                    }
                }
                else
                {
                    dsp_mode = ud_sw != 0 ? 2 : disp_digit;
                }
            }
            else
            {
                dsp_num = (((number + 1) % (multi10 * 10)) / multi10);
            }
        }

        OneNum(90, dsp_num, i, timer, tim_lmt, pos_x, pos_y, lmt_upp, lmt_dwn, dsp_mode, alp, time_mode);
    }
}

void ShockWave(u_char bgm_no, u_char alp)
{
    BEAT_DATA bd[1] = {
        { .beat = 30, .max = 14, .min = 3, .drop = 2 }
    };

    if (stf.run[2] == 2)
    {
        if (bb.tmr % bd[bgm_no].beat == 0)
        {
            bb.lvl_bak = ((bd[bgm_no].max - 7) * (rand() / (float)RAND_MAX) * 6.0f);

            if (bb.lvl_bak > bb.lvl_now)
            {
                bb.lvl_now = bb.lvl_bak;
            }
        }
        else
        {
            if ((bd[bgm_no].min - 7) * 6 < bb.lvl_now)
            {
                bb.lvl_now = bb.lvl_now - bd[bgm_no].drop;
            }
            else
            {
                bb.lvl_now = 6;
            }
        }
    }
    else
    {
        bb.lvl_now = 6;
    }

    PutChrOne(0x57, 0, 0xfffa, 0x80, alp, 0x0);
    PutChrOne(0x58, 0, 0xfffa, 0x80, alp, 0x0);

    if (bb.tmr < 100)
    {
        bb.tmr++;
    }
    else
    {
        bb.tmr = 0;
    }
}

void OneNum(u_short num_chr, u_char number, u_char no, u_char timer, u_char tim_lmt, short int pos_x, short int pos_y, short int lmt_upp, short int lmt_dwn, u_char mode, u_char alp, u_char time_mode)
{
    short int slot_mov;
    int i;
    short int dsp_y;
    u_char dsp_num;
    signed char dsp_v;
    signed char dsp_h;
    DISP_SPRT ds;
    SPRT_DAT *sd;
    short tmp_y;

    switch (mode)
    {
    case 0:
        slot_mov = 0;
    break;
    case 1:
        slot_mov = msel_sprt[num_chr].h * timer / (float)tim_lmt;
    break;
    case 2:
        slot_mov = -msel_sprt[num_chr].h * timer / (float)tim_lmt;
    break;
    }

    for (i = 0; i < 3; i++)
    {
        if (time_mode == 0)
        {
            dsp_num = (number + 11 - i) % 10;
        }
        else if (no == 0)
        {
            dsp_num = (number + 11 - i - 4) % 6;
        }
        else
        {
            dsp_num = (number + 11 - i) % 10;
        }

        sd = &msel_sprt[num_chr + dsp_num];

        CopySprDToSpr(&ds, sd);

        tmp_y = (ds.h + (pos_y - slot_mov)) - ds.h * i;

        YVH_CUT(tmp_y, ds.h, lmt_upp, lmt_dwn, &dsp_y, &dsp_v, (u_char *)&dsp_h);

        ds.x = no * 20 + pos_x;
        ds.y = tmp_y + dsp_y;
        ds.h = dsp_h;
        ds.v += dsp_v;
        ds.alpha = alp;
        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

        DispSprD(&ds);
    }
}
