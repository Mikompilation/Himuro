#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "item_get.h"

#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph3d/sglib.h"
// #include "graphics/motion/mdlwork.h" // SendManMdlTex
#define INCLUDING_FROM_ITEM_GET
#include "ingame/map/item_ctl.h"
#undef INCLUDING_FROM_ITEM_GET
#include "ingame/menu/ig_file.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/item.h"
#include "ingame/menu/item_get.h"
#include "main/glob.h"
#include "os/eeiop/adpcm/ea_tape.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"

static int BookGetMain();
static void BookGetPad(u_char model_type, u_char model_no);
static void BookGetDsp(u_char model_type, u_char model_no, u_char msg0_no, u_char msg1_no);
static int PhotGetMain();
static void PhotGetPad(u_char model_type, u_char model_no);
static void PhotGetDsp(u_char model_type, u_char model_no, u_char msg0_no, u_char msg1_no);
static int ItemGetMain();
static void ItemGetPad(u_char model_type, u_char model_no);
static void ItemGetDsp(u_char model_type, u_char model_no, u_char msg0_no, u_char msg1_no);
static void GetStr(u_char model_type, u_char model_no, u_char str_dsp);
static void GetFad(u_char model_type, u_char model_no, u_char fade, u_char time);
static void FileName(u_char model_type, u_char model_no, u_char fade, u_char time);
static void GetMsg(u_char msg_type, u_char msg_no, u_char msg_dsp);

TAPE_DAT tape_dat[] = {
    { .item_no = 60, .adpcm_no = AVT000_OGATA0_STR },
    { .item_no = 61, .adpcm_no = AVT001_OGATA1_STR },
    { .item_no = 19, .adpcm_no = AVT002_HIRASAKA0_STR },
    { .item_no = 62, .adpcm_no = AVT003_HIRASAKA1_STR },
    { .item_no = 63, .adpcm_no = AVT004_HIRASAKA2_STR },
    { .item_no = 64, .adpcm_no = AVT005_HIRASAKA3_STR },
    { .item_no = 20, .adpcm_no = AVT006_TAKAMINE0_STR },
    { .item_no = 65, .adpcm_no = AVT007_TAKAMINE1_STR },
    { .item_no = -1, .adpcm_no = -1 },
};

int play_tape_timer = 0;
char tape_play = 0;

static FILE_GET_CTRL get_ctrl;
static FILE_GET_DSP file_dsp;
static FLSH_CORE flsh;
static int igt_load_id;

static FILE_GET_FADE fade_ctr;
static FILE2D file2d;

#define MAX_VOLUME 0x3fff
#define PI 3.1415927f

void ItemGet(u_char get_type, u_char get_no, u_char msg0_no, u_char msg1_no)
{
    get_ctrl = (FILE_GET_CTRL){0};
    file_dsp = (FILE_GET_DSP){0};
    fade_ctr = (FILE_GET_FADE){0};
    file2d = (FILE2D){0};

    get_ctrl.type = get_type;
    get_ctrl.no = get_no;
    get_ctrl.msg0 = msg0_no;
    get_ctrl.msg1 = msg1_no;

    RstMessageYW();

    igt_load_id = -1;
    eff_blur_off = 1;
    tape_play = 0;

    if (get_type == 0 || get_type == 1 || get_type == 2)
    {
        get_ctrl.flg = 1;
    }
    else if (get_type == 3)
    {
        get_ctrl.flg = 2;
    }
    else if (get_type == 4)
    {
        get_ctrl.flg = 3;
        get_ctrl.itm_num = poss_item[get_no];
    }
}

int ItemGetCtrl()
{
    SetSprFile(0x1ce0000);

    if (file_dsp.cnt < 60)
    {
        file_dsp.cnt++;
    }
    else
    {
        file_dsp.cnt = 0;
    }

    switch (get_ctrl.flg)
    {
    case 1:
        if (BookGetMain() != 0)
        {
            return 1;
        }
    break;
    case 2:
        if (PhotGetMain() != 0)
        {
            return 1;
        }
    break;
    case 3:
        if (ItemGetMain() != 0)
        {
            return 1;
        }
    break;
    }

    return 0;
}

static int BookGetMain()
{
    BookGetPad(get_ctrl.type,get_ctrl.no);
    BookGetDsp(get_ctrl.type,get_ctrl.no, get_ctrl.msg0, get_ctrl.msg1);

    if (file_dsp.stp == 3)
    {
        file_dsp.stp = 0;
        eff_blur_off = 0;

        SendManMdlTex();

        if (ItemGetContinueJudge() == 0)
        {
            get_ctrl.flg = 0;

            return 1;
        }
    }

    return 0;
}

static void BookGetPad(u_char model_type, u_char model_no)
{
    switch (file_dsp.stp)
    {
    case 0:
        // do nothing ...
    break;
    case 1:
        // do nothing ...
    break;
    case 2:
        // do nothing ...
    break;
    case 3:
        // do nothing ...
    break;
    case 4:
        if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            if (fade_ctr.msg_end != 0)
            {
                file_dsp.stp = 5;

                SeStartFix(0xb, 0, 0x1000, 0x1000, 0);
            }
        }
    break;
    case 5:
        // do nothing ...
    break;
    case 6:
        if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            file_dsp.stp = 7;
        }
    break;
    case 7:
        file_dsp.page_all = ChkPageYW(model_type + 0x12, model_no);
        file_dsp.open_page = 1;
        file_dsp.stp = 8;
    break;
    case 8:
        if (*key_now[4] == 1)
        {
            file_dsp.stp = 9;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            if (file_dsp.page_all >= 2)
            {
                if (file_dsp.open_page < file_dsp.page_all)
                {
                    file_dsp.open_page++;

                    SeStartFix(4, 0, 0x1000, 0x1000, 0);
                }
                else
                {
                    file_dsp.stp = 9;

                    SeStartFix(1, 0, 0x1000, 0x1000, 0);
                }
            }
            else
            {
                file_dsp.stp = 9;

                SeStartFix(1, 0, 0x1000, 0x1000, 0);
            }
        }
    break;
    case 9:
        // do nothing ...
    break;
    case 10:
        if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            if (fade_ctr.msg_end != 0)
            {
                file_dsp.stp = 11;
            }
        }
    break;
    case 11:
        // do nothing ...
    break;
    }
}

static void BookGetDsp(u_char model_type, u_char model_no, u_char msg0_no, u_char msg1_no)
{
    u_char fade_type;
    u_char chng_tm;
    u_char name_dsp;
    u_char msg_dsp;
    u_char str_dsp;
    u_char msg_type;
    u_char msg_no;
    u_char file_pct;

    msg_type = model_type;

    fade_type = 0;
    chng_tm = 16;
    name_dsp = 0;
    msg_dsp = 0;
    str_dsp = 0;

    switch(file_dsp.stp)
    {
    case 0:
        chng_tm = 0;

        file_pct = map_file_dat[msg_type][model_no].dsp_pct;

        if (file_pct != 0xff)
        {
            if (igt_load_id == -1)
            {
                igt_load_id = LoadItem2D(4, file_pct);
            }
            else if (IsLoadEnd(igt_load_id) != 0)
            {
                igt_load_id = -1;
                file_dsp.stp = 1;
            }
        }
    break;
    case 1:
        file_dsp.stp = 4;
    break;
    case 4:
        if (msg0_no != 0xff)
        {
            msg_no = msg0_no;
            msg_dsp = 2;
        }
        else
        {
            file_dsp.stp = 5;
        }
    break;
    case 5:
        fade_type = 3;
        name_dsp = 1;

        if (msg0_no != 0xff)
        {
            msg_type = 0xff;
            msg_dsp = 7;
        }

        if (file_dsp.dsp < 16)
        {
            file_dsp.dsp++;

            file2d.scl = file_dsp.dsp  / 16.0f;
            file2d.alp = (file_dsp.dsp * 128)  / 16.0f;
        }
        else
        {
            file_dsp.stp = 6;
        }
    break;
    case 6:
        fade_type = 3;
        name_dsp = 1;
        msg_dsp = 3;
    break;
    case 7:
        fade_type = 4;
        msg_type = 0xff;
        msg_dsp = 3;
    break;
    case 8:
        fade_type = 4;
        msg_type = 0xff;
        msg_dsp = 3;
        str_dsp = 1;
    break;
    case 9:
        fade_type = 3;
        msg_type = 0xff;
        msg_dsp = 3;

        if (file_dsp.dsp != 0)
        {
            file_dsp.dsp--;

            file2d.scl = file_dsp.dsp  / 16.0f;
            file2d.alp = (file_dsp.dsp * 128)  / 16.0f;
        }
        else if (fade_ctr.str == 0)
        {
            file_dsp.stp = 10;
        }
    break;
    case 10:
        fade_type = 3;

        if (msg1_no != 0xff)
        {
            msg_no = msg1_no;
            msg_dsp = 2;
        }
    break;
    case 11:
        msg_type = 0xff;
        msg_dsp = 3;

        if (file_dsp.dsp != 0)
        {
            file_dsp.dsp--;

            file2d.scl = file_dsp.dsp  / 16.0f;
            file2d.alp = (file_dsp.dsp * 128)  / 16.0f;
        }
        else if (fade_ctr.msg == 0)
        {
            file_dsp.stp = 3;
        }
    break;
    }

    GetFad(model_type, model_no, fade_type, chng_tm);

    GetMsg(msg_type, msg_no, msg_dsp);

    GetStr(model_type, model_no, str_dsp);

    FileName(model_type, model_no, name_dsp, chng_tm);
}

static int PhotGetMain()
{
    PhotGetPad(get_ctrl.type,get_ctrl.no);
    PhotGetDsp(get_ctrl.type,get_ctrl.no,get_ctrl.msg0,get_ctrl.msg1);

    if (file_dsp.stp == 3)
    {
        file_dsp.stp = 0;
        eff_blur_off = 0;

        SendManMdlTex();

        if (ItemGetContinueJudge() == 0)
        {
            get_ctrl.flg = 0;

            return 1;
        }
    }

    return 0;
}

static void PhotGetPad(u_char model_type, u_char model_no)
{
    switch(file_dsp.stp)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 13:
    case 15:
    case 18:
        // do nothing ...
    break;
    case 12:
        if (*key_now[6] != 1 && *key_now[5] != 1)
        {
            break;
        }

        if (fade_ctr.msg_end == 0)
        {
            break;
        }

        file_dsp.stp = 13;

        SeStartFix(11, 0, 0x1000, 0x1000, 0);
    break;
    case 14:
        if (*key_now[6] != 1 && *key_now[5] != 1)
        {
            break;
        }

        file_dsp.stp = 15;
    break;
    case 16:
        if (*key_now[6] != 1 && *key_now[5] != 1)
        {
            break;
        }

        file2d.scl = 1.0f;

        file_dsp.stp = 17;
    break;
    case 17:
        if (*key_now[6] != 1 && *key_now[5] != 1)
        {
            break;
        }

        if (fade_ctr.msg_end != 0)
        {
            file_dsp.stp = 18;
        }
    break;
    }
}

static void PhotGetDsp(u_char model_type, u_char model_no, u_char msg0_no, u_char msg1_no)
{
    u_char fade_type;
    u_char chng_tm;
    u_char name_dsp;
    u_char msg_dsp;
    u_char msg_type;
    u_char msg_no;
    u_char file_pct;

    msg_type = model_type;

    chng_tm = 16;
    fade_type = 0;
    name_dsp = 0;
    msg_dsp = 0;

    switch(file_dsp.stp)
    {
    case 0:
        chng_tm = 0;

        if (file_pct != 0xff)
        {
            if (igt_load_id == -1)
            {
                igt_load_id = LoadItem2D(4, file_pct);
            }
            else if (IsLoadEnd(igt_load_id) != 0)
            {
                igt_load_id = -1;
                file_dsp.stp = 1;
            }
        }
    break;
    case 1:
        file_dsp.stp = 12;
    break;
    case 12:
        if (msg0_no != 0xff)
        {
            msg_no = msg0_no;
            msg_dsp = 2;
        }
        else
        {
            file_dsp.stp = 13;
        }
    break;
    case 13:
        fade_type = 3;
        name_dsp = 1;

        if (msg0_no != 0xff)
        {
            msg_type = 0xff;
            msg_dsp = 7;
        }
        if (file_dsp.dsp < 16)
        {
            file_dsp.dsp++;

            file2d.scl = file_dsp.dsp  / 16.0f;
            file2d.alp = (file_dsp.dsp * 128)  / 16.0f;
        }
        else
        {
            file_dsp.stp = 14;
        }
    break;
    case 14:
        fade_type = 3;
        name_dsp = 1;
        msg_dsp = 3;
    break;
    case 15:
        fade_type = 3;
        file2d.scl = 1.5f;
        file_dsp.stp = 16;
        msg_type = 0xff;
        msg_dsp = 3;
    break;
    case 16:
        fade_type = 1;
        msg_type = 0xff;
        msg_dsp = 3;
    break;
    case 17:
        fade_type = 3;

        if (msg1_no != 0xff)
        {
            msg_no = msg1_no;
            msg_dsp = 2;
        }

        if (file_dsp.dsp != 0)
        {
            file_dsp.dsp--;

            file2d.scl = file_dsp.dsp  / 16.0f;
            file2d.alp = (file_dsp.dsp * 128)  / 16.0f;
        }
    break;
    case 18:
        msg_type = 0xff;
        msg_dsp = 3;

        if (file_dsp.dsp != 0)
        {
            file_dsp.dsp--;

            file2d.scl = file_dsp.dsp  / 16.0f;
            file2d.alp = (file_dsp.dsp * 128)  / 16.0f;
        }
        else if (fade_ctr.msg == 0)
        {
            file_dsp.stp = 3;
        }
    break;
    }

    GetFad(model_type, model_no, fade_type, chng_tm);

    GetMsg(msg_type, msg_no, msg_dsp);

    FileName(model_type, model_no, name_dsp, chng_tm);
}

static int ItemGetMain()
{
    ItemGetPad(get_ctrl.type, get_ctrl.no);
    ItemGetDsp(get_ctrl.type, get_ctrl.no, get_ctrl.msg0, get_ctrl.msg1);

    if (file_dsp.stp == 3)
    {
        file_dsp.stp = 0;

        eff_blur_off = 0;

        SendManMdlTex();

        if (file_dsp.stts == 0)
        {
            if (ItemGetContinueJudge() == 0)
            {
                get_ctrl.flg = 0;

                return 1;
            }
        }
        else
        {
            file_dsp.stts = 0;

            get_ctrl.flg = 0;

            return 1;
        }
    }

    return 0;
}

static void ItemGetPad(u_char model_type, u_char model_no)
{
    u_char stp; // not in STAB

    switch(file_dsp.stp)
    {
    case 0:
    case 1:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 20:
    case 23:
        // do nothing ...
    break;
    case 19:
        if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            if (fade_ctr.msg_end != 0)
            {
                file_dsp.stp = 20;
            }
        }
    break;
    case 21:
        stp = 2;
        if (model_no == 8)
        {
            if (get_ctrl.itm_num == 0)
            {
                file_dsp.stp = 22;
                SeStartFix(11, 0, 0x1000, 0x1000, 0);
            }
            else
            {
                file_dsp.stp = stp;
            }
        }
        else
        {
            if (get_ctrl.itm_num < 99)
            {
                file_dsp.stp = 22;
                SeStartFix(11, 0, 0x1000, 0x1000, 0);
            }
            else
            {
                file_dsp.stp = stp;
            }
        }
    break;
    case 2:
        if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            file_dsp.stts = 1;
            file_dsp.stp = 23;
        }
    break;
    case 22:
        if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            if (fade_ctr.msg_end != 0)
            {
                file_dsp.stp = 23;
            }
        }
    break;
    }
}

static void ItemGetDsp(u_char model_type, u_char model_no, u_char msg0_no, u_char msg1_no)
{
    u_char fade_type;
    u_char chng_tm;
    u_char msg_dsp;
    u_char msg_type;
    u_char msg_no;

    msg_type = model_type;

    chng_tm = 16;
    fade_type = 0;
    msg_dsp = 0;

    switch(file_dsp.stp)
    {
    case 0:
        chng_tm = 0;

        if (igt_load_id == -1)
        {
            igt_load_id = LoadItem2D(model_type, model_no);
        }
        else if (IsLoadEnd(igt_load_id) != 0)
        {
            igt_load_id = -1;
            file_dsp.stp = 1;
        }
    break;
    case 1:
        file_dsp.stp = 19;
    break;
    case 19:
        if (msg0_no != 0xff)
        {
            msg_no = msg0_no;
            msg_dsp = 2;
        }
        else
        {
            file_dsp.stp = 20;
        }
    break;
    case 20:
        fade_type = 3;

        if (msg0_no != 0xff)
        {
            msg_type = 0xff;
            msg_dsp = 7;
        }

        if (file_dsp.dsp < 16)
        {
            file_dsp.dsp++;

            file2d.scl = file_dsp.dsp  / 16.0f;
            file2d.alp = (file_dsp.dsp * 128)  / 16.0f;
        }
        else
        {
            file_dsp.stp = 21;
        }
    break;
    case 21:
        fade_type = 1;
        msg_dsp = 4;
    break;
    case 2:
        fade_type = 3;
        msg_no = 0;
        msg_dsp = 6;
    break;
    case 22:
        fade_type = 3;

        if (msg1_no != 0xff)
        {
            msg_no = msg1_no;
            msg_dsp = 6;
        }
    break;
    case 23:
        msg_type = 0xff;
        msg_dsp = 3;

        if (file_dsp.dsp != 0)
        {
            file_dsp.dsp--;

            file2d.scl = file_dsp.dsp  / 16.0f;
            file2d.alp = (file_dsp.dsp * 128)  / 16.0f;
        }
        else if (fade_ctr.msg == 0)
        {
            file_dsp.stp = 3;
        }
    break;
    }

    GetFad(model_type, model_no, fade_type, chng_tm);
    GetMsg(msg_type, msg_no, msg_dsp);
    FileName(model_type, model_no, 1, chng_tm);
}

static void GetStr(u_char model_type, u_char model_no, u_char str_dsp)
{
    float str_add;

    str_add = 8.0f;

    if (str_dsp != 0x0)
    {
        if (fade_ctr.str + str_add < 128.0f)
        {
            fade_ctr.str += str_add;
        }
        else
        {
            fade_ctr.str = 0x80;
        }
    }
    else
    {
        if (fade_ctr.str - str_add > 0.0f)
        {
            fade_ctr.str -= str_add;
        }
        else
        {
            fade_ctr.str = 0x0;
        }
    }

    if (fade_ctr.str != 0)
    {
        CmnWindow(9, -68, 0, fade_ctr.str, 0x80);
        NextPageNavi(file_dsp.open_page, file_dsp.page_all, 539, fade_ctr.str, 0, 0);
        PutStringYW(model_type + 10, model_no, 317, 35, 0x808080, fade_ctr.str, 0x19000, 1);
        PutPage2YW(model_type + 18, model_no, file_dsp.open_page, 90, 110, 0x808080, fade_ctr.str, 0x19000);
        PageInfo(file_dsp.open_page, file_dsp.page_all, 0, -13, fade_ctr.str);
        DispCaption(21, fade_ctr.str);
    }
}

static void GetFad(u_char model_type, u_char model_no, u_char fade, u_char time)
{
    u_char scn_obj;
    u_char chr_obj;
    u_char ply_obj;
    u_char chr_now;
    u_char ply_now;
    int chr;
    float per;

    switch(fade)
    {
    case 0:
        scn_obj = 128;
        chr_obj = 0;
        ply_obj = 0;
    break;
    case 1:
        scn_obj = 64;
        chr_obj = 128;
        ply_obj = 0;
    break;
    case 2:
        scn_obj = 128;
        chr_obj = 64;
        ply_obj = 0;
    break;
    case 3:
        scn_obj = 64;
        chr_obj = 64;
        ply_obj = 0;
    break;
    case 4:
        scn_obj = 64;
        chr_obj = 64;
        ply_obj = 64;
    break;
    default:
        scn_obj = 0;
        chr_obj = 0;
        ply_obj = 0;
    break;
    }

    if (time == 0)
    {
        fade_ctr.scn = scn_obj;
        fade_ctr.chr = chr_obj; chr_now = chr_obj;
        fade_ctr.ply = ply_obj; ply_now = ply_obj;
        fade_ctr.bak = fade;
        fade_ctr.tim = 0;
    }
    else if (fade_ctr.tim == 0)
    {
        chr_now = fade_ctr.chr;
        ply_now = fade_ctr.ply;

        if (fade_ctr.bak != fade)
        {
            fade_ctr.tim = time;
        }
    }
    else
    {
        fade_ctr.tim--;

        per = 1.0f - (float)fade_ctr.tim / time;

        if (fade_ctr.tim != 0)
        {
            chr_now = (chr_obj - fade_ctr.chr) * per + fade_ctr.chr;
            ply_now = (ply_obj - fade_ctr.ply) * per + fade_ctr.ply;
        }
        else
        {
            fade_ctr.scn = scn_obj;
            fade_ctr.chr = chr_obj; chr_now = chr_obj;
            fade_ctr.ply = ply_obj; ply_now = ply_obj;
            fade_ctr.bak = fade;
        }
    }

    if (file_dsp.stp >= 2)
    {
        chr = (chr_now << 16) + (chr_now << 8) + chr_now;

        SetPanel2(0x1000, 0.0f, 0.0f, 640.0f, 448.0f, 1, 0, 0, 0, chr_now);

        if (model_type == 4)
        {
            file2d.tex_addr = VramItem2D(0, 0);

            DspItem2D(file2d.tex_addr, -160.0f, -40.0f, chr, file2d.alp, 1.0f, 1.0f, 0x32);
        }
        else
        {
            file2d.tex_addr = VramItem2D(0, 0);

            DspItem2D(file2d.tex_addr, -160.0f, 0.0f, chr, file2d.alp, 1.5f, 1.5f, 0x32);
        }

        PolySquareYW(0.0f, 0.0f, 0x280, 0x1c0, 0, ply_now, 1.0f, 1.0f, 0x2d000, 0, 0, 0);
    }
}

static void FileName(u_char model_type, u_char model_no, u_char fade, u_char time)
{
    u_char msg_knd;
    char book_adj;
    u_char val;

    if (fade == 0)
    {

        val = 100.0f / time;
        if (fade_ctr.nam - val > 0)
        {
            fade_ctr.nam -= val;
        }
        else
        {
            fade_ctr.nam = 0;
        }
    }
    else
    {
        fade_ctr.nam = 100;
    }

    if (file_dsp.stp > 1)
    {
        if (model_type == 0 || model_type == 1 || model_type == 2)
        {
            book_adj = 100;
            msg_knd = model_type + 10;
        }
        else if (model_type == 3)
        {
            book_adj = 0;
            msg_knd = 13;
        }
        else if (model_type == 4)
        {
            book_adj = 0;
            msg_knd = 24;
        }

        PutStringYW(msg_knd, model_no, 320, 260 - book_adj, 0x808080, (file2d.alp * fade_ctr.nam) / 100.0f, 0x19000, 1);
    }
}

static void GetMsg(u_char msg_type, u_char msg_no, u_char msg_dsp)
{
    float msg_add;
    float btn_alp;

    switch(msg_dsp)
    {
    case 3:
        msg_add = 8.0f;

        if (fade_ctr.msg - msg_add > 0.0f)
        {
            fade_ctr.msg -= msg_add;
        }
        else
        {
            fade_ctr.msg = 0;
        }
    break;
    case 0:
    case 4:
        fade_ctr.msg = 0;
    break;
    case 1:
    case 5:
        fade_ctr.msg = 128;
    break;
    case 2:
    case 6:
        msg_add = 8.0f;

        if (fade_ctr.msg + msg_add < 128.0f)
        {
            fade_ctr.msg += msg_add;
        }
        else
        {
            fade_ctr.msg = 128;
        }
    break;
    case '\a':
        msg_add = 8.0f;

        if (fade_ctr.msg - msg_add > 0.0f)
        {
            fade_ctr.msg -= msg_add;
        }
        else
        {
            fade_ctr.msg = 0;
        }
    break;
    }

    if (msg_dsp < 4)
    {
        if (fade_ctr.msg != 0)
        {
            DrawMessageBox(0x19000, 56.0f, 304.0f, 528.0f, 106.0f, fade_ctr.msg);
        }
    }
    else
    {
        DrawMessageBox(0x19000, 56.0f, 304.0f, 528.0f, 106.0f, 128);
    }

    if (msg_type == 0xff)
    {
        msg_type = fade_ctr.msg_type_bak;
        msg_no = fade_ctr.msg_no_bak;
    }

    if (msg_no != 0xff)
    {
        if (msg_no == 1)
        {
            PutStringYW(msg_type, 1, 80, 320, 0x808080, fade_ctr.msg, 0xf000, 0);
            PutStringYW(6, 17, 48, 346, 0x808080, fade_ctr.msg, 0xf000, 0);

            YesNoCrslOKR(0x14000, (file_dsp.csr[0] * 132 + 184), 344.0f, 0x808080, fade_ctr.msg, 2.0f);
        }
        else
        {
            fade_ctr.msg_end = PutMessageYW(msg_type, msg_no, 80, 320, 0x808080, fade_ctr.msg, 0x19000);
            btn_alp = (SgSinf((file_dsp.cnt * (PI * 2)) / 60.0f) * fade_ctr.msg + fade_ctr.msg) * 0.5f;

            DrawButtonTex(0x14000, 3, 536.0f, 368.0f, btn_alp);
        }
    }

    fade_ctr.msg_type_bak = msg_type;
    fade_ctr.msg_no_bak = msg_no;
}

void NextPageNavi(u_char hav_now, u_char dsp_max, u_short chr_top, short int ofs_x, short int ofs_y, u_char alpha)
{
    int rgb_l;
    int rgb_r;

    if (dsp_max > 1)
    {
        FlashStarYW(&flsh, 64, 192, 60, 0);

        CLRtoRGB(&rgb_r, flsh.alpha, flsh.alpha, flsh.alpha);

        rgb_l = rgb_r;

        if (hav_now > 1)
        {
            PutSpriteYW(chr_top, chr_top, ofs_x, ofs_y, 0.0f, rgb_l, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }

        if (hav_now < dsp_max)
        {
            PutSpriteYW(chr_top + FND_FRAME_R, chr_top + FND_FRAME_R, ofs_x, ofs_y, 0.0f, rgb_r, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }
    }
    else
    {
        FlashStarYW(&flsh, 64, 192, 60, 0xff);

        CLRtoRGB(&rgb_r, flsh.alpha, flsh.alpha, flsh.alpha);
    }
}

int CheckTape(int item_no)
{
    int i;

    i = 0;

    while (tape_dat[i].item_no != -1)
    {
        if (item_no == tape_dat[i].item_no)
        {
            return tape_dat[i].adpcm_no;
        }

        i++;
    }

    return -1;
}

void PlayTape(int adpcm_no)
{
    AdpcmPlayTape(adpcm_no, MAX_VOLUME, 5);

    tape_play = 1;

    play_tape_timer = 180;
}

void StopTape(int item_no)
{
    int i;

    i = 0;

    while (tape_dat[i].item_no != -1)
    {
        if (item_no == tape_dat[i].item_no)
        {
            AdpcmStopTape(5);

            return;
        }

        i++;
    }
}
