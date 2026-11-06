#include "common.h"
#include "typedefs.h"
#include "addresses.h"
#include "enums.h"
#include "item_get.h"

// gcc/src/newlib/libm/math/sf_sin.c
float sinf(float x);

#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph3d/sglib.h"
// #include "graphics/motion/mdlwork.h"
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

enum {
    START = 0,
    INIT = 1,
    ERROR = 2,
    END = 3,
    STEP_B00 = 4,
    STEP_B01 = 5,
    STEP_B02 = 6,
    STEP_B03 = 7,
    STEP_B04 = 8,
    STEP_B05 = 9,
    STEP_B06 = 10,
    STEP_B07 = 11,
    STEP_P00 = 12,
    STEP_P01 = 13,
    STEP_P02 = 14,
    STEP_P03 = 15,
    STEP_P04 = 16,
    STEP_P05 = 17,
    STEP_P06 = 18,
    STEP_I00 = 19,
    STEP_I01 = 20,
    STEP_I02 = 21,
    STEP_I03 = 22,
    STEP_I04 = 23
};

#if defined(BUILD_JP_VERSION)
#define PAD_ACTION_CONFIRM PAD_CIRCLE
#define PAD_ACTION_CANCEL PAD_CROSS
#elif defined(BUILD_US_VERSION)
#define PAD_ACTION_CONFIRM PAD_CROSS
#define PAD_ACTION_CANCEL PAD_TRIANGLE
#elif defined(BUILD_EU_VERSION)
#define PAD_ACTION_CONFIRM PAD_CROSS
#define PAD_ACTION_CANCEL PAD_TRIANGLE
#endif

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

    if (get_type == GET_ITM_TYPE_FILE0 || get_type == GET_ITM_TYPE_FILE1 || get_type == GET_ITM_TYPE_FILE2)
    {
        get_ctrl.flg = GET_FILE_BOOK;
    }
    else if (get_type == GET_ITM_TYPE_PHOT)
    {
        get_ctrl.flg = GET_FILE_PHOT;
    }
    else if (get_type == GET_ITM_TYPE_ITEM)
    {
        get_ctrl.flg = GET_FILE_ITEM;

        get_ctrl.itm_num = poss_item[get_no];
    }
}

int ItemGetCtrl()
{
    SetSprFile(LOAD_ADDRESS_31);

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
    case GET_FILE_BOOK:
        if (BookGetMain() != 0)
        {
            return 1;
        }
    break;
    case GET_FILE_PHOT:
        if (PhotGetMain() != 0)
        {
            return 1;
        }
    break;
    case GET_FILE_ITEM:
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

    if (file_dsp.stp == END)
    {
        file_dsp.stp = START;
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
    case START:
        // do nothing ...
    break;
    case INIT:
        // do nothing ...
    break;
    case ERROR:
        // do nothing ...
    break;
    case END:
        // do nothing ...
    break;
    case STEP_B00:
        if (PAD_BTN_PRESSED(PAD_SQUARE) || PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
        {
            if (fade_ctr.msg_end != 0)
            {
                file_dsp.stp = STEP_B01;

                SeStartFix(0xb, 0, 0x1000, 0x1000, 0);
            }
        }
    break;
    case STEP_B01:
        // do nothing ...
    break;
    case STEP_B02:
        if (PAD_BTN_PRESSED(PAD_SQUARE) || PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
        {
            file_dsp.stp = STEP_B03;
        }
    break;
    case STEP_B03:
        file_dsp.page_all = ChkPageYW(model_type + IGMSG_FILE0_DAT, model_no);
        file_dsp.open_page = 1;
        file_dsp.stp = STEP_B04;
    break;
    case STEP_B04:
        if (PAD_BTN_PRESSED(PAD_ACTION_CANCEL))
        {
            file_dsp.stp = STEP_B05;

            SeStartFix(3, 0, 0x1000, 0x1000, 0);
        }
        else if (PAD_BTN_PRESSED(PAD_SQUARE) || PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
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
                    file_dsp.stp = STEP_B05;

                    SeStartFix(1, 0, 0x1000, 0x1000, 0);
                }
            }
            else
            {
                file_dsp.stp = STEP_B05;

                SeStartFix(1, 0, 0x1000, 0x1000, 0);
            }
        }
    break;
    case STEP_B05:
        // do nothing ...
    break;
    case STEP_B06:
        if (PAD_BTN_PRESSED(PAD_SQUARE) || PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
        {
            if (fade_ctr.msg_end != 0)
            {
                file_dsp.stp = STEP_B07;
            }
        }
    break;
    case STEP_B07:
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
    case START:
        chng_tm = 0;

        file_pct = map_file_dat[msg_type][model_no].dsp_pct;

        if (file_pct != 0xff)
        {
            if (igt_load_id == -1)
            {
                igt_load_id = LoadItem2D(GET_ITM_TYPE_ITEM, file_pct);
            }
            else if (IsLoadEnd(igt_load_id) != 0)
            {
                igt_load_id = -1;
                file_dsp.stp = INIT;
            }
        }
    break;
    case INIT:
        file_dsp.stp = STEP_B00;
    break;
    case STEP_B00:
        if (msg0_no != 0xff)
        {
            msg_no = msg0_no;
            msg_dsp = 2;
        }
        else
        {
            file_dsp.stp = STEP_B01;
        }
    break;
    case STEP_B01:
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
            file_dsp.stp = STEP_B02;
        }
    break;
    case STEP_B02:
        fade_type = 3;
        name_dsp = 1;
        msg_dsp = 3;
    break;
    case STEP_B03:
        fade_type = 4;
        msg_type = 0xff;
        msg_dsp = 3;
    break;
    case STEP_B04:
        fade_type = 4;
        msg_type = 0xff;
        msg_dsp = 3;
        str_dsp = 1;
    break;
    case STEP_B05:
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
            file_dsp.stp = STEP_B06;
        }
    break;
    case STEP_B06:
        fade_type = 3;

        if (msg1_no != 0xff)
        {
            msg_no = msg1_no;
            msg_dsp = 2;
        }
    break;
    case STEP_B07:
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
            file_dsp.stp = END;
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

    if (file_dsp.stp == END)
    {
        file_dsp.stp = START;
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
    case START:
    case INIT:
    case ERROR:
    case END:
    case STEP_B00:
    case STEP_B01:
    case STEP_B02:
    case STEP_B03:
    case STEP_B04:
    case STEP_B05:
    case STEP_B06:
    case STEP_B07:
    case STEP_P01:
    case STEP_P03:
    case STEP_P06:
        // do nothing ...
    break;
    case STEP_P00:
        if (PAD_BTN_PRESSED(PAD_SQUARE) || PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
        {
            if (fade_ctr.msg_end != 0)
            {
                file_dsp.stp = STEP_P01;

                SeStartFix(11, 0, 0x1000, 0x1000, 0);
            }
        }
    break;
    case STEP_P02:
        if (PAD_BTN_PRESSED(PAD_SQUARE) || PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
        {
            file_dsp.stp = STEP_P03;
        }
    break;
    case STEP_P04:
        if (PAD_BTN_PRESSED(PAD_SQUARE) || PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
        {
            file2d.scl = 1.0f;

            file_dsp.stp = STEP_P05;
        }
    break;
    case STEP_P05:
        if (PAD_BTN_PRESSED(PAD_SQUARE) || PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
        {
            if (fade_ctr.msg_end != 0)
            {
                file_dsp.stp = STEP_P06;
            }
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
    case START:
        chng_tm = 0;

        if (file_pct != 0xff)
        {
            if (igt_load_id == -1)
            {
                igt_load_id = LoadItem2D(GET_ITM_TYPE_ITEM, file_pct);
            }
            else if (IsLoadEnd(igt_load_id) != 0)
            {
                igt_load_id = -1;
                file_dsp.stp = INIT;
            }
        }
    break;
    case INIT:
        file_dsp.stp = STEP_P00;
    break;
    case STEP_P00:
        if (msg0_no != 0xff)
        {
            msg_no = msg0_no;
            msg_dsp = 2;
        }
        else
        {
            file_dsp.stp = STEP_P01;
        }
    break;
    case STEP_P01:
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
            file_dsp.stp = STEP_P02;
        }
    break;
    case STEP_P02:
        fade_type = 3;
        name_dsp = 1;
        msg_dsp = 3;
    break;
    case STEP_P03:
        fade_type = 3;
        file2d.scl = 1.5f;
        file_dsp.stp = STEP_P04;
        msg_type = 0xff;
        msg_dsp = 3;
    break;
    case STEP_P04:
        fade_type = 1;
        msg_type = 0xff;
        msg_dsp = 3;
    break;
    case STEP_P05:
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
    case STEP_P06:
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
            file_dsp.stp = END;
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

    if (file_dsp.stp == END)
    {
        file_dsp.stp = START;

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
    case START:
    case INIT:
    case END:
    case STEP_B00:
    case STEP_B01:
    case STEP_B02:
    case STEP_B03:
    case STEP_B04:
    case STEP_B05:
    case STEP_B06:
    case STEP_B07:
    case STEP_P00:
    case STEP_P01:
    case STEP_P02:
    case STEP_P03:
    case STEP_P04:
    case STEP_P05:
    case STEP_P06:
    case STEP_I01:
    case STEP_I04:
        // do nothing ...
    break;
    case STEP_I00:
        if (PAD_BTN_PRESSED(PAD_SQUARE) || PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
        {
            if (fade_ctr.msg_end != 0)
            {
                file_dsp.stp = STEP_I01;
            }
        }
    break;
    case STEP_I02:
        stp = ERROR;
        if (model_no == 8)
        {
            if (get_ctrl.itm_num == 0)
            {
                file_dsp.stp = STEP_I03;
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
                file_dsp.stp = STEP_I03;
                SeStartFix(11, 0, 0x1000, 0x1000, 0);
            }
            else
            {
                file_dsp.stp = stp;
            }
        }
    break;
    case ERROR:
        if (PAD_BTN_PRESSED(PAD_SQUARE) || PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
        {
            file_dsp.stts = 1;
            file_dsp.stp = STEP_I04;
        }
    break;
    case STEP_I03:
        if (PAD_BTN_PRESSED(PAD_SQUARE) || PAD_BTN_PRESSED(PAD_ACTION_CONFIRM))
        {
            if (fade_ctr.msg_end != 0)
            {
                file_dsp.stp = STEP_I04;
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
    case START:
        chng_tm = 0;

        if (igt_load_id == -1)
        {
            igt_load_id = LoadItem2D(model_type, model_no);
        }
        else if (IsLoadEnd(igt_load_id) != 0)
        {
            igt_load_id = -1;
            file_dsp.stp = INIT;
        }
    break;
    case INIT:
        file_dsp.stp = STEP_I00;
    break;
    case STEP_I00:
        if (msg0_no != 0xff)
        {
            msg_no = msg0_no;
            msg_dsp = 2;
        }
        else
        {
            file_dsp.stp = STEP_I01;
        }
    break;
    case STEP_I01:
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
            file_dsp.stp = STEP_I02;
        }
    break;
    case STEP_I02:
        fade_type = 1;
        msg_dsp = 4;
    break;
    case ERROR:
        fade_type = 3;
        msg_no = GET_MSG_ERR;
        msg_dsp = 6;
    break;
    case STEP_I03:
        fade_type = 3;

        if (msg1_no != 0xff)
        {
            msg_no = msg1_no;
            msg_dsp = 6;
        }
    break;
    case STEP_I04:
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
            file_dsp.stp = END;
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
#if defined(BUILD_JP_VERSION)
    char num;
    char pos_y;
#endif

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

#if defined(BUILD_JP_VERSION)
        NextPageNavi(file_dsp.open_page, file_dsp.page_all, 535, fade_ctr.str, 0, 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        NextPageNavi(file_dsp.open_page, file_dsp.page_all, 539, fade_ctr.str, 0, 0);
#endif

#if defined(BUILD_JP_VERSION)
        num = ChkChrNumYW(model_type + IGMSG_FILE0_NAME, model_no);
        pos_y = ((13 - num) * 24) * 0.5;

        PutStringYW(model_type + IGMSG_FILE0_NAME, model_no, pos_y + 162, 35, 0x808080, fade_ctr.str, 0x19000, 0);
        PutPageYW(model_type + IGMSG_FILE0_DAT,model_no,file_dsp.open_page,0x5a,0x6e,0x808080,fade_ctr.str,0x19000);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutStringYW(model_type + IGMSG_FILE0_NAME, model_no, 317, 35, 0x808080, fade_ctr.str, 0x19000, 1);
        PutPage2YW(model_type + IGMSG_FILE0_DAT, model_no, file_dsp.open_page, 90, 110, 0x808080, fade_ctr.str, 0x19000);
#endif
        PageInfo(file_dsp.open_page, file_dsp.page_all, 0, -13, fade_ctr.str);

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        DispCaption(21, fade_ctr.str);
#endif
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

    if (file_dsp.stp >= ERROR)
    {
        chr = (chr_now << 16) + (chr_now << 8) + chr_now;

        SetPanel2(0x1000, 0.0f, 0.0f, 640.0f, 448.0f, 1, 0, 0, 0, chr_now);

        if (model_type == GET_ITM_TYPE_ITEM)
        {
            file2d.tex_addr = VramItem2D(0, 0);

            DspItem2D(file2d.tex_addr, -160.0f, -40.0f, chr, file2d.alp, 1.0f, 1.0f, 0x32);
        }
        else
        {
            file2d.tex_addr = VramItem2D(0, 0);

            DspItem2D(file2d.tex_addr, -160.0f, 0.0f, chr, file2d.alp, 1.5f, 1.5f, 0x32);
        }

        VER_POLY_SQUARE_YW(0.0f, 0.0f, 0x280, 0x1c0, 0, ply_now, 1.0f, 1.0f, 0x2d000, 0, 0);
    }
}

static void FileName(u_char model_type, u_char model_no, u_char fade, u_char time)
{
    u_char msg_knd;
    char book_adj;
    u_char val;
#if defined(BUILD_JP_VERSION)
    char num;
    char pos_y;
#endif

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

    if (file_dsp.stp > INIT)
    {
        if (model_type == GET_ITM_TYPE_FILE0 || model_type == GET_ITM_TYPE_FILE1 || model_type == GET_ITM_TYPE_FILE2)
        {
            book_adj = 100;
            msg_knd = model_type + IGMSG_FILE0_NAME;
        }
        else if (model_type == GET_ITM_TYPE_PHOT)
        {
            book_adj = 0;
            msg_knd = IGMSG_PHOTO_NAME;
        }
        else if (model_type == GET_ITM_TYPE_ITEM)
        {
            book_adj = 0;
            msg_knd = IGMSG_ITEM_NAME;
        }

#if defined(BUILD_JP_VERSION)
        num = ChkChrNumYW(msg_knd, model_no);
        pos_y = ((8 - num) * 24) * 0.5;

        PutStringYW(msg_knd, model_no, pos_y + 215, 260 - book_adj, 0x808080, (file2d.alp * fade_ctr.nam) / 100.0f, 0x19000, 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        PutStringYW(msg_knd, model_no, 320, 260 - book_adj, 0x808080, (file2d.alp * fade_ctr.nam) / 100.0f, 0x19000, 1);
#endif
    }
}

static void GetMsg(u_char msg_type, u_char msg_no, u_char msg_dsp)
{
    float msg_add;
#if defined(BUILD_JP_VERSION)
    float msg_alpha;
#endif
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
        if (msg_no == GET_MSG_SLCT)
        {
            PutStringYW(msg_type, GET_MSG_SLCT, 80, 320, 0x808080, fade_ctr.msg, 0xf000, 0);
#if defined(BUILD_JP_VERSION)
            PutStringYW(IGMSG_MENU_MSG, MENU_MSG_YES_NO, 96, 346, 0x808080, fade_ctr.msg, 0xf000, 0);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            PutStringYW(IGMSG_MENU_MSG, MENU_MSG_YES_NO, 48, 346, 0x808080, fade_ctr.msg, 0xf000, 0);
#endif

#if defined(BUILD_JP_VERSION)
            YesNoCrslOKR(0x14000, (file_dsp.csr[0] * 105 + 243), 344.0f, 0x808080, fade_ctr.msg, 1.5f);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            YesNoCrslOKR(0x14000, (file_dsp.csr[0] * 132 + 184), 344.0f, 0x808080, fade_ctr.msg, 2.0f);
#endif
        }
        else
        {
            fade_ctr.msg_end = PutMessageYW(msg_type, msg_no, 80, 320, 0x808080, fade_ctr.msg, 0x19000);
#if defined(BUILD_JP_VERSION)
            msg_alpha = fade_ctr.msg; btn_alp = (VER_SINF((file_dsp.cnt * (PI * 2)) / 60.0f) * msg_alpha + msg_alpha) * 0.5f;
            DrawButtonTex(0x14000, 2, 536.0f, 368.0f, btn_alp);
#elif defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
            btn_alp = (VER_SINF((file_dsp.cnt * (PI * 2)) / 60.0f) * fade_ctr.msg + fade_ctr.msg) * 0.5f;
            DrawButtonTex(0x14000, 3, 536.0f, 368.0f, btn_alp);
#endif
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
