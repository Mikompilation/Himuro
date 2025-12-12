#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "mc_at.h"

#include "ingame/menu/sp_menu.h"
#include "ingame/menu/item.h"
#include "graphics/graph2d/tim2.h"
// #include "mc/mc_album.h" // mcAlbumSaveMain
#include "mc/mc_disp.h"
#include "mc/mc_main.h"

typedef struct {
    u_char alp;
} DSP_MC;

static DSP_MC dsp_mc;

static char McAtAlbmMainLoop();
static char McAtSaveMainLoop();
static char McAtLoadMainLoop(u_char type);

char McAtLoadChk(u_char type)
{
    char end_flg;
    char ret;

    end_flg = mcLoadMain();

    ret = McAtLoadMainLoop(type);

    if (ret == 2 && end_flg != 0)
    {
        return end_flg;
    }

    return 0;
}

char McAtSaveChk()
{
    char end_flg;
    char ret;

    end_flg = mcSaveMain();

    ret = McAtSaveMainLoop();

    if (ret == 2 && end_flg != 0)
    {
        return end_flg;
    }

    return 0;
}

char McAtAlbmChk()
{
    char end_flg;
    char ret;

    end_flg = mcAlbumSaveMain();

    ret = McAtAlbmMainLoop();

    if (ret == 2 && end_flg != 0)
    {
        return end_flg;
    }

    return 0;
}

static char McAtLoadMainLoop(u_char type)
{
    char rtrn;
    u_char csr0;
    u_char yes_no;
    static u_char csr1;
    static u_char csr2;
    u_char msg;
    u_char alt;
    static u_char fdt;

    csr0 = 0xff;

    switch(mc_ctrl.stepbak)
    {
    case MC_INIT:
        fdt = 0xff;
        csr1 = csr2 = 0xff;
        dsp_mc.alp = 0;
    break;
    case MC_SEL_NEW_OR_LOAD:
        fdt = 0xff;
        csr0 = mc_ctrl.now_cur;
    break;
    case MC_SEL_GOTITLE:
        fdt = 0xff;
        csr0 = mc_ctrl.now_cur;
    break;
    case MC_SEL_SLOT:
        fdt = 0xff;
        csr1 = mc_ctrl.port;
        csr2 = 0xff;
    break;
    case MC_SEL_FILE:
        fdt = 1;
        csr2 = mc_ctrl.sel_cur;
    break;
    case MC_SEL_LOAD:
        csr0 = mc_ctrl.now_cur;
    break;
    case MC_LOAD:
    case MC_LOAD_END:
    case MC_END:
        // do nothing ...
    break;
    case MC_ERR_END:
        fdt = 0xff;
    break;
    default:
        fdt = 0xff;
    break;
    }

    rtrn = 0;

    if (mc_ctrl.step == MC_END || mc_ctrl.step == MC_ERR_END)
    {
        if (dsp_mc.alp - 8 > 0)
        {
            dsp_mc.alp -= 8;
        }
        else
        {
            dsp_mc.alp = 0;
            rtrn = 2;
        }
    }
    else
    {
        if (dsp_mc.alp + 8 < 0x80)
        {
            dsp_mc.alp += 8;
        }
        else
        {
            dsp_mc.alp = 0x80;
            rtrn = 1;
        }
    }


    yes_no = alt = msg = 0xff;

    if (mc_ctrl.msg_no != MC_MSG_NONE)
    {
        switch (mcCheckMsgType(mc_ctrl.msg_no))
        {
        case 0:
            alt = mc_ctrl.msg_no;
        break;
        case 1:
            msg = mc_ctrl.msg_no;
        break;
        case 2:
            alt = mc_ctrl.msg_no;
            yes_no = 1;
        break;
        }
    }

    BgFusumaYW(0x606060, 0.0f, dsp_mc.alp, 0x7d000);

    DspMemSavePoint(0xff, msg, fdt, 1, alt, csr0, csr1, csr2, yes_no, dsp_mc.alp, type);

    return rtrn;
}

static char McAtSaveMainLoop()
{
    char rtrn;
    u_char csr0;
    u_char yes_no;
    static u_char csr1;
    static u_char csr2;
    u_char msg;
    u_char alt;
    static u_char fdt;

    csr0 = 0xff;

    switch(mc_ctrl.stepbak)
    {
    case MC_INIT:
        fdt = 0xff;
        csr1 = csr2 = 0xff;
        dsp_mc.alp = 0;
    break;
    case MC_SEL_SLOT:
        fdt = 0xff;
        csr1 = mc_ctrl.port;
        csr2 = 0xff;
    break;
    case MC_SEL_FILE:
        fdt = 1;
        csr2 = mc_ctrl.sel_cur;
    break;
    case MC_SEL_SAVE:
        csr0 = mc_ctrl.now_cur;
    break;
    case MC_SEL_MAKE_DIR:
        fdt = 0xff;
        csr0 = mc_ctrl.now_cur;
    break;
    case MC_SEL_REMAKE_DIR:
        fdt = 0xff;
        csr0 = mc_ctrl.now_cur;
    break;
    case MC_SEL_FORMAT:
        fdt = 0xff;
        csr0 = mc_ctrl.now_cur;
    break;
    case MC_SAVE:
        // do nothing ...
    break;
    case MC_SAVE_END:
        fdt = 1;
    break;
    case MC_SEL_GOTITLE:
    case MC_END:
    case MC_ERR_END:
    default:
        fdt = 0xff;
    break;
    }

    rtrn = 0;

    if (mc_ctrl.step == MC_END || mc_ctrl.step == MC_ERR_END)
    {
        if (dsp_mc.alp - 8 > 0)
        {
            dsp_mc.alp -= 8;
        }
        else
        {
            dsp_mc.alp = 0;
            rtrn = 2;
        }
    }
    else
    {
        if (dsp_mc.alp + 8 < 0x80)
        {
            dsp_mc.alp += 8;
        }
        else
        {
            dsp_mc.alp = 0x80;
            rtrn = 1;
        }
    }

    yes_no = alt = msg = 0xff;

    if (mc_ctrl.msg_no != MC_MSG_NONE)
    {
        switch (mcCheckMsgType(mc_ctrl.msg_no))
        {
        case 0:
            alt = mc_ctrl.msg_no;
        break;
        case 1:
            msg = mc_ctrl.msg_no;
        break;
        case 2:
            alt = mc_ctrl.msg_no;
            yes_no = 1;
        break;
        }
    }


    BgFusumaYW(0x606060, 0.0f, dsp_mc.alp, 0x7d000);

    DspMemSavePoint(0xff, msg, fdt, 1, alt, csr0, csr1, csr2, yes_no, dsp_mc.alp, 0);

    return rtrn;
}

static char McAtAlbmMainLoop()
{
    char rtrn;
    u_char csr0;
    u_char yes_no;
    static u_char csr1;
    static u_char csr2;
    static u_char csr3;
    u_char msg;
    u_char alt;
    static u_char fdt;

    csr0 = 0xff;

    switch(mc_ctrl.stepbak)
    {
    case MC_INIT:
        fdt = 0xff;
        csr1 = mc_ctrl.port;
        csr2 = csr3 = 0xff;
        dsp_mc.alp = 0;
    break;
    case MC_SEL_SLOT:
        fdt = 0xff;
        csr1 = mc_ctrl.port;
        csr2 = 0xff;
    break;
    case MC_SEL_FILE:
        fdt = 1;
        csr2 = mc_ctrl.now_cur;
        csr3 = 0xff;
    break;
    case MC_SEL_ALBUM_TYPE:
        csr3 = mc_ctrl.now_cur;
    break;
    case MC_SEL_FORMAT:
        fdt = 0xff;
        csr0 = mc_ctrl.now_cur;
    break;
    case MC_SEL_MAKE_DIR:
        fdt = 0xff;
        csr0 = mc_ctrl.now_cur;
    break;
    case MC_SAVE:
        // do nothing ...
    break;
    case MC_MAKE_DIR:
        fdt = 1;
        csr0 = mc_ctrl.now_cur;
    break;
    case MC_SEL_SAVE:
        csr0 = mc_ctrl.now_cur;
    break;
    case MC_SEL_REMAKE_DIR:
        csr0 = mc_ctrl.now_cur;
    break;
    case MC_SEL_ALBUM_CHECK:
        csr0 = mc_ctrl.now_cur;
    break;
    case MC_SAVE_END:
        fdt = 1;
    break;
    case MC_SEL_GOTITLE:
    case MC_END:
    case MC_ERR_END:
    default:
        fdt = 0xff;
    break;
    }

    rtrn = 0;

    if (mc_ctrl.step == MC_END || mc_ctrl.step == MC_ERR_END)
    {
        if (dsp_mc.alp - 8 > 0)
        {
            dsp_mc.alp -= 8;
        }
        else
        {
            dsp_mc.alp = 0;

            if (csr3 != 0xff)
            {
                mc_album_type = csr3;
            }
            else
            {
                u_char no = mc_ctrl.album.file_no[csr2];
                mc_album_type = mc_header_buf[no].album.type;
            }

            rtrn = 2;
        }
    }
    else
    {
        if (dsp_mc.alp + 8 < 0x80)
        {
            dsp_mc.alp += 8;
        }
        else
        {
            dsp_mc.alp = 0x80;

            rtrn = 1;
        }
    }

    yes_no = alt = msg = 0xff;

    if (mc_ctrl.msg_no != MC_MSG_NONE)
    {
        switch (mcCheckMsgType(mc_ctrl.msg_no))
        {
        case 0:
            alt = mc_ctrl.msg_no;
        break;
        case 1:
            msg = mc_ctrl.msg_no;
        break;
        case 2:
            alt = mc_ctrl.msg_no;
            yes_no = 1;
        break;
        }
    }

    SetSprFile(0x1ce0000);

    BgFusumaYW(0x606060, 0.0f, 128.0f, 0x7d000);

    DspMemSavePoint(0xff, msg, fdt, 0x1, alt, csr0, csr1, csr2, yes_no, dsp_mc.alp, 2);

    AlbmSlct(csr3, mc_ctrl.stepbak, dsp_mc.alp, 0x0);

    return rtrn;
}
