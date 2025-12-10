#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "mc_main.h"

#include "sce/sifdev.h"

#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"
#include "main/glob.h"
#include "ingame/menu/ig_menu.h"
#include "outgame/memory_album.h"
#include "mc/mc_acs.h" // mcAcsReq
#include "mc/mc_album.h"
#include "mc/mc_disp.h"
#include "mc/mc_exec.h"
#include "mc/mc_icon.h"

MC_CTRL mc_ctrl = {0};
MC_HEADER mc_header = {0};
MC_HEADER mc_header_buf[16] = {0};

char mcSaveMain()
{
    char end_flg;

    end_flg = 0;

    mc_ctrl.stepbak = mc_ctrl.step;

    switch(mc_ctrl.step)
    {
    case MC_INIT:
        mcSetStep(MC_SEL_SLOT, MC_MSG_SEL_SLOT);
    break;
    case MC_SEL_SLOT:
        switch (mcCtrlSelSlot())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            mcSetStep(MC_CHECK, MC_MSG_CHECK);
        break;
        case 2:
            mcSetStep(MC_ERR_END, MC_MSG_NONE);
        break;
        }
    break;
    case MC_CHECK:
        if (mcCtrlCheck() != 0)
        {
            mcSetStepAfterCheck();
        }
    break;
    case MC_SEL_FILE:
        switch (mcCtrlFileSel())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            if (mc_header_buf[mc_ctrl.sel_file].game.map_flg == 0)
            {
                mcSetStep(MC_SEL_SAVE, MC_MSG_SEL_SAVE);
            }
            else
            {
                mcSetStep(MC_SEL_SAVE, MC_MSG_SEL_OVERWRITE);
            }
        break;
        case 2:
            mcSetStep(MC_SEL_SLOT, MC_MSG_SEL_SLOT);
        break;
        case 3:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOCARD);
        break;
        }
    break;
    case MC_SEL_FORMAT:
        switch(mcCtrlYesno(1))
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            mcSetStep(MC_FORMAT, MC_MSG_FORMAT);
        break;
        case 2:
        case 3:
            mcSetStep(MC_SEL_SLOT, MC_MSG_SEL_SLOT);
        break;
        case 4:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOCARD);
        break;
        }
    break;
    case MC_SEL_SAVE:
        switch(mcCtrlYesno(1))
        {
        case 0:
            // do nothing ...
        break;
        case 1:
#ifdef BUILD_EU_VERSION
            if (mc_header_buf[mc_ctrl.sel_file].game.map_flg == 0)
            {
                mcSetStep(MC_SAVE, MC_MSG_SAVE);
            }
            else
            {
                mcSetStep(MC_SAVE, MC_MSG_OVERWRITE);
            }
#else
            mcSetStep(MC_SAVE, MC_MSG_SAVE);
#endif
        break;
        case 2:
        case 3:
            mcSetStep(MC_SEL_FILE, MC_MSG_SEL_SAVEFILE);
        break;
        case 4:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOCARD);
        }
    break;
    case MC_SEL_MAKE_DIR:
        switch(mcCtrlYesno(1))
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            mcSetStep(MC_MAKE_DIR, MC_MSG_MAKE_DIR);
        break;
        case 2:
        case 3:
            mcSetStep(MC_SEL_SLOT, MC_MSG_SEL_SLOT);
        break;
        case 4:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOCARD);
        break;
        }
    break;
    case MC_MAKE_DIR:
        switch (mcCtrlMakeDir())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
#ifdef BUILD_EU_VERSION
            if (mc_header_buf[mc_ctrl.sel_file].game.map_flg == 0)
            {
                mcSetStep(MC_SAVE_END, MC_MSG_END_SAVE);
            }
            else
            {
                mcSetStep(MC_SAVE_END, MC_MSG_END_OVERWRITE);
            }
#else
            mcSetStep(MC_SAVE_END, MC_MSG_END_SAVE);
#endif
        break;
        case 2:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_SAVE);
        break;
        }
    break;
    case MC_SAVE:
        switch (mcCtrlSave())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
#ifdef BUILD_EU_VERSION
            if (mc_header_buf[mc_ctrl.sel_file].game.map_flg == 0)
            {
                mcSetStep(MC_SAVE_END, MC_MSG_END_SAVE);
            }
            else
            {
                mcSetStep(MC_SAVE_END, MC_MSG_END_OVERWRITE);
            }
#else
            mcSetStep(MC_SAVE_END, MC_MSG_END_SAVE);
#endif
        break;
        case 2:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_SAVE);
        break;
        }
    break;
    case MC_FORMAT:
        switch (mcCtrlFormat())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
#ifdef BUILD_EU_VERSION
            mcSetStep(MC_FORMAT_END, MC_MSG_END_FORMAT);
            mcFormatEndInit();
#else
            mcSetStep(MC_MAKE_DIR, MC_MSG_MAKE_DIR);
#endif
        break;
        case 2:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_FORMAT);
        break;
        }
    break;
    case MC_DISP_ERR:
        switch (mcCtrlDispErr())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            mcSetStep(MC_SEL_SLOT, MC_MSG_SEL_SLOT);
        break;
        case 2:
            mcSetMessage(MC_MSG_ERR_NOCARD);
        break;
        case 3:
            mcSetStep(MC_CHECK, MC_MSG_CHECK);
        break;
        }
    break;
    case MC_SEL_REMAKE_DIR:
        switch(mcCtrlYesno(1))
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            mcSetStep(MC_DELETE_DIR, MC_MSG_REMAKE_DIR);
        break;
        case 2:
        case 3:
            mcSetStep(MC_SEL_SLOT, MC_MSG_SEL_SLOT);
        break;
        case 4:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOCARD);
        break;
        }
    break;
    case MC_DELETE_DIR:
        switch (mcCtrlDelete())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            mcSetStep(MC_MAKE_DIR, MC_MSG_REMAKE_DIR);

            mc_ctrl.sub_step = 2;
        break;
        case 2:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_SAVE);
        break;
        }
    break;
    case MC_SAVE_END:
        if (mc_ctrl.sub_step == 0)
        {
            mc_ctrl.sub_step = 1;

            mc_header_buf[mc_ctrl.sel_file] = mc_header;
        }

        if (pad[0].one & 0x50)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, MC_MSG_SEL_SLOT);

            mcSetStep(MC_END, MC_MSG_NONE);
        }
    break;
    case MC_END:
        end_flg = 1;
    break;
    case MC_ERR_END:
        end_flg = 2;
    break;
#ifdef BUILD_EU_VERSION
    case MC_FORMAT_END:
        if (mcFormatEnd() != 0)
        {
            mcSetStep(MC_MAKE_DIR, MC_MSG_MAKE_DIR);
        }
    break;
#endif
    }

    if (end_flg != 0)
    {
        mcEnd();
    }

    return end_flg;
}

char mcLoadMain()
{
    char end_flg;

    end_flg = 0;

    mc_ctrl.stepbak = mc_ctrl.step;

    switch(mc_ctrl.step)
    {
    case MC_INIT:
        mc_newalbum_flg = 0;

        if (mc_ctrl.mode == MC_MODE_GAMESAVE || mc_ctrl.mode == MC_MODE_GAMELOAD || mc_ctrl.mode == MC_MODE_STARTCHECK)
        {
            mcSetStep(MC_SEL_SLOT, MC_MSG_SEL_SLOT);
            break;
        }

        switch (mc_ctrl.mode)
        {
        case 3:
            mcSetStep(MC_SEL_NEW_OR_LOAD, MC_MSG_SEL_NEW_OR_LOAD);
        break;
        case 4:
        case 5:
            mcSetStep(MC_CAP_ALBUM_LOAD, MC_MSG_CAP_ALBUM_LOAD);
        break;
        case 6:
            mcSetStep(MC_SEL_NEW_OR_LOAD, MC_MSG_SEL_NEW_OR_LOAD2);
        break;
        }
    break;
    case MC_SEL_NEW_OR_LOAD:
        switch (mcCtrl3taku(1))
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            mcSetStep(MC_END, MC_MSG_NONE);

            mc_newalbum_flg = 1;
        break;
        case 2:
            mcSetStep(MC_SEL_SLOT, MC_MSG_SEL_SLOT);
        break;
        case 3:
            if (mc_ctrl.mode == MC_MODE_ALBUMLOAD_TITLE2)
            {
                mcSetStep(MC_SEL_GOTITLE, MC_MSG_SEL_GOTITLE);
            }
            else
            {
                mcSetStep(MC_ERR_END, MC_MSG_NONE);
            }
        break;
        }
    break;
    case MC_CAP_ALBUM_LOAD:
        if (pad[0].one & 0x50)
        {
            mcSetStep(MC_SEL_SLOT, MC_MSG_SEL_SLOT);
        }
    break;
    case MC_SEL_GOTITLE:
        switch (mcCtrlYesno(1))
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            mcSetStep(MC_ERR_END, MC_MSG_NONE);
        break;
        case 2:
            mcSetStep(MC_SEL_NEW_OR_LOAD, MC_MSG_SEL_NEW_OR_LOAD2);
        break;
        case 3:
            mcSetStep(MC_SEL_NEW_OR_LOAD, MC_MSG_SEL_NEW_OR_LOAD2);
        break;
#ifdef BUILD_EU_VERSION
        case 4:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOCARD);
        break;
#endif
        }
    break;
    case MC_SEL_SLOT:
        switch (mcCtrlSelSlot())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            mcSetStep(MC_CHECK, MC_MSG_CHECK);
        break;
        case 2:
            if (mc_ctrl.mode == MC_MODE_ALBUMLOAD_GAME1)
            {
                mcSetStep(MC_SEL_NEW_OR_LOAD, MC_MSG_SEL_NEW_OR_LOAD);
            }
            else
            {
                mcSetStep(MC_ERR_END, MC_MSG_NONE);
            }
        break;
        }
    break;
    case MC_CHECK:
        if (mc_ctrl.mode == MC_MODE_GAMESAVE || mc_ctrl.mode == MC_MODE_GAMELOAD || mc_ctrl.mode == MC_MODE_STARTCHECK)
        {
            if (mcCtrlCheck() != 0)
            {
                mcSetStepAfterCheck();
            }
        }
        else
        {
            if (mcCtrlCheckAlbum() != 0)
            {
                mcSetStepAfterCheck();
            }
        }
    break;
    case MC_SEL_FILE:
        switch (mcCtrlFileSel())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            mcSetStep(MC_SEL_LOAD, MC_MSG_SEL_LOAD);
        break;
        case 2:
            mcSetStep(MC_SEL_SLOT, MC_MSG_SEL_SLOT);
        break;
        case 3:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOCARD);
        break;
        }
    break;
    case MC_SEL_LOAD:
        switch(mcCtrlYesno(0))
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            mcSetStep(MC_LOAD, MC_MSG_LOAD);
        break;
        case 2:
        case 3:
            mcSetStep(MC_SEL_FILE, MC_MSG_SEL_LOADFILE);
        break;
        case 4:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOCARD);
        }
    break;
    case MC_LOAD:
        switch (mcCtrlLoad())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            mcSetStep(MC_LOAD_END, MC_MSG_END_LOAD);
        break;
        case 2:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_LOAD);
        break;
        case 3:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_SUM);
        break;
        }
    break;
    case MC_DISP_ERR:
        switch (mcCtrlDispErr())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            mcSetStep(MC_SEL_SLOT, MC_MSG_SEL_SLOT);
        break;
        case 2:
            mcSetMessage(MC_MSG_ERR_NOCARD);
        break;
        case 3:
            mcSetStep(MC_CHECK, MC_MSG_CHECK);
        break;
        }
    break;
    case MC_LOAD_END:
        if (pad[0].one & 0x50)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);
            mcSetStep(MC_END, MC_MSG_NONE);
        }
    break;
    case MC_END:
        end_flg = 1;

        if (
            (
                mc_ctrl.mode == MC_MODE_ALBUMSAVE ||
                mc_ctrl.mode == MC_MODE_ALBUMLOAD_GAME1 || mc_ctrl.mode == MC_MODE_ALBUMLOAD_GAME2 ||
                mc_ctrl.mode == MC_MODE_ALBUMLOAD_TITLE1 || mc_ctrl.mode == MC_MODE_ALBUMLOAD_TITLE2
            ) &&
            mc_newalbum_flg != 0
        )
        {
            end_flg = 3;

            mc_photo_num = 0;
        }
    break;
    case MC_ERR_END:
        end_flg = 2;
    break;
    }

    if (end_flg != 0)
    {
        mcEnd();
    }

    return end_flg;
}

char mcCtrlCheck()
{
    static int loop;

    switch(mc_ctrl.sub_step)
    {
    case 0:
        mc_ctrl.sub_step = 1;

        mcAcsReq(0);
    break;
    case 1:
        if (mcAcsMain() != 0)
        {
            if (mc_ctrl.acs.sta == 0x0)
            {
                mc_ctrl.sub_step = 4;
            }
            else
            {
                return 1;
            }
        }
    break;
    case 4:
        mc_ctrl.sub_step = 5;

        mcSetPathDir(0);

        sprintf(mc_ctrl.rw.name, "%s*", mc_ctrl.rw.path);

        mcAcsReq(1);
    break;
    case 5:
        if (mcAcsMain() == 0)
        {
            return 0;
        }

        if (mc_ctrl.acs.sta == 0x0)
        {
            if (mcCheckFileList() != 0)
            {
                if (mcCheckEmpty(mc_ctrl.dir_size) != 0)
                {
                    mc_ctrl.acs.sta = 0x2 | 0x1;
                }
                else
                {
                    mc_ctrl.acs.sta = 0x8 | 0x1;
                }

                return 1;
            }

            if (mc_ctrl.mode == MC_SEL_LOAD)
            {
                return 1;
            }

            loop = 0;

            mc_ctrl.sub_step = 6;
        }
        else
        {
            if (mc_ctrl.acs.sta == 0x4)
            {
                if (mcCheckEmpty(0) != 0)
                {
                    mc_ctrl.acs.sta = 0x2 | 0x1;
                }
            }

            return 1;
        }
    break;
    case 6:
        mcAcsFileReq((loop & 0xff) + 5, 1, 1);

        mc_ctrl.sub_step = 7;
    break;
    case 7:
        if (mcReadFile() != 0)
        {
            if (mc_ctrl.acs.sta == 0x0)
            {
                mcSetLoadFile(mc_ctrl.work_addr, 5);

                mc_header_buf[loop] = mc_header;

                if (++loop > 2)
                {
                    if (mcCheckHeaderFile() != 0)
                    {
                        mc_ctrl.acs.sta = 0x4 | 0x1;
                    }

                    return 1;
                }

                mc_ctrl.sub_step = 6;
            }
            else
            {
                return 1;
            }
        }
    break;
    }

    return 0;
}

char mcCtrlSelSlot()
{
    if (pad[0].one & 0x40)
    {
        SeStartFix(1, 0, 0x1000, 0x1000, 0);

        if (mc_ctrl.port != 2)
        {
            mcSubInit();

            mc_ctrl.sel_cur = 0;
            mc_ctrl.now_cur = 0;

            return 1;
        }

        return 2;
    }

    if (pad[0].one & 0x10)
    {
        SeStartFix(3, 0, 0x1000, 0x1000, 0);

        return 2;
    }

    if (*key_now[1] == 1 || Ana2PadDirCnt(2) == 1)
    {
        SeStartFix(0, 0, 0x1000, 0x1000, 0);

        mc_ctrl.port++;

        if (mc_ctrl.port > 2)
        {
            mc_ctrl.port = 0;
        }
    }
    else if (*key_now[0] == 1 || Ana2PadDirCnt(0) == 1)
    {
        SeStartFix(0, 0, 0x1000, 0x1000, 0);

        mc_ctrl.port--;

        if (mc_ctrl.port < 0)
        {
            mc_ctrl.port = 2;
        }
    }

    mc_ctrl.port_mes = mc_ctrl.port + 1;

    if (mc_ctrl.port_mes > 2)
    {
        mc_ctrl.port_mes = 1;
    }

    return 0;
}

char mcCtrlFileSel()
{
    int num;

    switch(mc_ctrl.mode)
    {
    case 0:
        num = mc_ctrl.dir.gamefile_num;
        if (++num > 3)
        {
            num = 3;
        }
    break;
    case 1:
        num = mc_ctrl.dir.gamefile_num;
    break;
    case 2:
        num = mc_ctrl.album.file_num;
        if (mcCheckEmpty(0) == 0)
        {
            if (++num > 5)
            {
                num = 5;
            }
        }
    break;
    case 3:
    case 4:
    case 5:
    case 6:
        num = mc_ctrl.album.file_num;
    break;
    }

    if (mc_ctrl.sub_step == 0)
    {
        mc_ctrl.sub_step = 1;
        mc_ctrl.now_cur = mc_ctrl.sel_cur;
    }

    if (mc_ctrl.sub_step == 1)
    {
        mcAcsReq(7);
        mc_ctrl.sub_step = 2;
    }
    else
    {
        if (mcAcsMain() != 0)
        {
            if (mc_ctrl.acs.sta != 0)
            {
                return 3;
            }
            mc_ctrl.sub_step = 1;
        }
    }

    mc_ctrl.sel_cur = mc_ctrl.now_cur;

    if (pad[0].one & 0x40)
    {
        if (mc_ctrl.mode == 2 || mc_ctrl.mode == 3 || mc_ctrl.mode == 4 || mc_ctrl.mode == 5 || mc_ctrl.mode == 6)
        {
            if (mc_ctrl.mode == 1 || mc_ctrl.mode == 3 || mc_ctrl.mode == 4 || mc_ctrl.mode == 5 || mc_ctrl.mode == 6)
            {
                if (mc_ctrl.album.sta[mc_ctrl.sel_cur] != 0)
                {
                    SeStartFix(2, 0, 0x1000, 0x1000, 0);

                    return 0;
                }

                SeStartFix(1, 0, 0x1000, 0x1000, 0);

                mc_ctrl.sel_file = mc_ctrl.album.file_no[mc_ctrl.sel_cur];
            }
            else if (mc_ctrl.now_cur >= mc_ctrl.album.file_num)
            {
                mc_ctrl.sel_file = mcGetAlbumNo();

                if (mc_ctrl.sel_file == 0xff)
                {
                    SeStartFix(2, 0, 0x1000, 0x1000, 0);

                    return 0;
                }
                else {
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);

                    return 4;
                }

            } else {

                switch (mc_ctrl.album.sta[mc_ctrl.now_cur])
                {
                case 3:
                    SeStartFix(2, 0, 0x1000, 0x1000, 0);

                    return 0;
                break;
                case 9:
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);

                    mc_ctrl.sel_file = mc_ctrl.album.file_no[mc_ctrl.sel_cur];

                    return 5;
                break;
                default:
                    SeStartFix(1, 0, 0x1000, 0x1000, 0);

                    mc_ctrl.sel_file = mc_ctrl.album.file_no[mc_ctrl.sel_cur];
                    mc_ctrl.album.type = mc_header_buf[mc_ctrl.sel_file].album.type;
                break;
                }
            }
        }
        else
        {
            if ((mc_ctrl.mode == 0 || mc_ctrl.mode == 2) && mc_ctrl.dir.gamefile_num < mc_ctrl.now_cur)
            {
                mc_ctrl.sel_file = mc_ctrl.dir.gamefile_num;
            }
            else
            {
                mc_ctrl.sel_file = mc_ctrl.sel_cur;
            }

            SeStartFix(1, 0, 0x1000, 0x1000, 0);
        }

        return 1;
    }
    else
    {
        if (pad[0].one & 0x10)
        {
            SeStartFix(3, 0, 0x1000, 0x1000, 0);

            return 2;
        }

        if (*key_now[1] == 1 || Ana2PadDirCnt(2) == 1)
        {
            if (num > 1)
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);
            }

            mc_ctrl.now_cur++;

            if (num <= mc_ctrl.now_cur)
            {
                mc_ctrl.now_cur = 0;
            }
        }
        else if (*key_now[0] == 1 || Ana2PadDirCnt(0) == 1)
        {
            if (num > 1)
            {
                SeStartFix(0, 0, 0x1000, 0x1000, 0);
            }

            mc_ctrl.now_cur--;

            if (mc_ctrl.now_cur < 0)
            {
                mc_ctrl.now_cur = num - 1;
            }
        }
    }

    return 0;
}

char mcCtrlLoad()
{
    u_char file_id;

    if (mc_ctrl.mode == MC_MODE_GAMESAVE || mc_ctrl.mode == MC_MODE_GAMELOAD || mc_ctrl.mode == MC_MODE_STARTCHECK)
    {
        file_id = 5;
    }
    else
    {
        file_id = 8;
    }

    file_id += mc_ctrl.sel_file;

    switch (mc_ctrl.sub_step)
    {
    case 0:
        mcSetPathDir(mc_ctrl.sel_file);
        mcAcsFileReq(file_id, 1, 0);

        mc_ctrl.sub_step = 1;
    break;
    case 1:
        if (mcReadFile() != 0)
        {
            if (mc_ctrl.acs.sta != 0)
            {
                return 2;
            }

            if (mcSetLoadFile(mc_ctrl.work_addr, file_id) != 0)
            {
                return 3;
            }

            mcSetHeaderFile();

            return 1;
        }
    break;
    }

    return 0;
}

char mcCtrlSave()
{
    u_char file_id;

    if (mc_ctrl.mode == MC_MODE_GAMESAVE || mc_ctrl.mode == MC_MODE_GAMELOAD || mc_ctrl.mode == MC_MODE_STARTCHECK)
    {
        file_id = mc_ctrl.sel_file + 5;
    }
    else
    {
        file_id =  mc_ctrl.sel_file + 8;
    }

    switch (mc_ctrl.sub_step)
    {
    case 0:
        mcMakeHeaderFile();
        mcMakeSaveFile(mc_ctrl.work_addr, file_id);
        mcSetPathDir(mc_ctrl.sel_file);
        mcAcsFileReq(file_id, 3, 0);

        mc_ctrl.sub_step = 1;
    break;
    case 1:
        if (mcWriteFile() != 0)
        {
            if (mc_ctrl.acs.sta == 0)
            {
                return 1;
            }

            return 2;
        }
    break;
    }

    return 0;
}

char mcCtrlMakeDir()
{
    static char loop;
    static int load_id;

    switch(mc_ctrl.sub_step)
    {
    case 0:
        mcSetPathDir(mc_ctrl.sel_file);
        mcAcsReq(5);

        mc_ctrl.sub_step = 1;
    break;
    case 1:
        if (mcAcsMain() != 0)
        {
            if (mc_ctrl.acs.sta != 0)
            {
                return 2;
            }

            mc_ctrl.sub_step = 2;
        }
    break;
    case 2:
        mcSetIconSys(&mc_ctrl.icon);
        mcAcsFileReq(0, SCE_CREAT | SCE_RDWR, 0);

        mc_ctrl.sub_step = 3;
    break;
    case 3:
        if (mcWriteFile() != 0)
        {
            if (mc_ctrl.acs.sta == 0)
            {
                loop = 0;

                mc_ctrl.sub_step = 4;
            }
            else
            {
                return 2;
            }
        }
    break;
    case 4:
        if (mc_ctrl.mode == MC_MODE_GAMESAVE || mc_ctrl.mode == MC_MODE_GAMELOAD || mc_ctrl.mode == MC_MODE_STARTCHECK)
        {
            load_id = LoadReq(mc_iconFile[loop], (u_int)mc_ctrl.work_addr);
        }
        else
        {
            load_id = LoadReq(mc_iconFileAlbum[loop], (u_int)mc_ctrl.work_addr);
        }

        mc_ctrl.sub_step = 5;
    break;
    case 5:
        if (IsLoadEnd(load_id) != 0)
        {
            mc_ctrl.sub_step = 6;
        }
    break;
    case 6:
        mcAcsFileReq(loop + 1, 0x203, 0);
        mc_ctrl.sub_step = 7;
    break;
    case 7:
        if (mcWriteFile() != 0)
        {
            if (mc_ctrl.acs.sta != 0)
            {
                return 2;
            }

            if (++loop >= 1)
            {
                if (mc_ctrl.mode == MC_MODE_GAMESAVE || mc_ctrl.mode == MC_MODE_GAMELOAD || mc_ctrl.mode == MC_MODE_STARTCHECK)
                {
                    mc_ctrl.sub_step = 8;
                }
                else
                {
                    mc_ctrl.sub_step = 10;
                }

                loop = 0;
            }
            else
            {
                mc_ctrl.sub_step = 4;
            }
        }
    break;
    case 8:
        if (loop == 0)
        {
            mcMakeHeaderFile();
            mcMakeSaveFile(mc_ctrl.work_addr, 5);
        }
        else
        {
            memset(mc_ctrl.work_addr, 0xff, mc_game_size);
            memset(mc_ctrl.work_addr, 0, 0x30); // 0x30 = ??
        }

        mcAcsFileReq(loop + 5, SCE_CREAT | SCE_RDWR, 0);

        mc_ctrl.sub_step = 9;
    break;
    case 9:
        if (mcWriteFile() != 0)
        {
            if (mc_ctrl.acs.sta == 0)
            {
                if (++loop > 2)
                {
                    return 1;
                }

                mc_ctrl.sub_step = 8;
            }
            else
            {
                return 2;
            }
        }
        break;
    case 10:
        mcMakeHeaderFile();
        mcMakeSaveFile(mc_ctrl.work_addr, mc_ctrl.sel_file + 8);
        mcAcsFileReq(mc_ctrl.sel_file + 8, SCE_CREAT | SCE_RDWR, 0);

        mc_ctrl.sub_step = 11;
    break;
    case 11:
        if (mcWriteFile() != 0)
        {
            if (mc_ctrl.acs.sta == 0)
            {
                return 1;
            }

            return 2;
        }
    break;
    }

    return 0;
}

char mcCtrlFormat()
{
    switch (mc_ctrl.sub_step)
    {
    case 0:
        mcAcsReq(6);

        mc_ctrl.sub_step = 1;
    break;
    default:
        if (mcAcsMain() != 0)
        {
            if (mc_ctrl.acs.sta == 0)
            {
                return 1;
            }

            return 2;
        }
    break;
    }

    return 0;
}

char mcCtrlDelete()
{
    static u_int cnt;

    switch(mc_ctrl.sub_step)
    {
    case 0:
        cnt = 0;

        if (mc_ctrl.mode == MC_MODE_GAMESAVE || mc_ctrl.mode == MC_MODE_GAMELOAD || mc_ctrl.mode == MC_MODE_STARTCHECK)
        {
            mc_ctrl.sub_step = 3;
        }
        else
        {
            mc_ctrl.sub_step = 1;
        }

        mcSetPathDir((char)mc_ctrl.sel_file);
    break;
    case 1:
        mc_ctrl.sub_step = 2;

        sprintf(mc_ctrl.rw.name, "%s*", mc_ctrl.rw.path);

        mcAcsReq(1);
    break;
    case 2:
        if (mcAcsMain() != 0)
        {
            if (mc_ctrl.acs.sta != 0)
            {
                return 2;
            }

            mc_ctrl.sub_step = 3;
        }
    break;
    case 3:
        if (mc_ctrl.dir.file_num <= cnt)
        {
            return 1;
        }

        sprintf(mc_ctrl.rw.name, "%s%s", mc_ctrl.rw.path, mc_ctrl.dir.table[cnt].EntryName);

        if (mc_ctrl.dir.table[cnt].EntryName[0] != '.')
        {
            mcAcsReq(9);

            mc_ctrl.sub_step = 4;
        }
        else
        {
            cnt++;
        }
    break;
    case 4:
        if (mcAcsMain() != 0)
        {
            if (mc_ctrl.acs.sta != 0)
            {
                return 2;
            }

            mc_ctrl.sub_step = 3;

        cnt++;
        }
    break;
    }

    return 0;
}


u_int mc_game_size = 0;
u_int mc_album_size = 0;
u_char mc_start_flg = 0;
u_char mc_msn_flg = 0;
u_char mc_newalbum_flg = 0;
u_int mc_album_type = 0;
u_int mc_photo_num = 0;
#ifdef BUILD_EU_VERSION
u_char mc_language = 0;
short int mc_check_timer = 0;
#endif

char mcCtrlDispErr()
{
    if (pad[0].one & 0x50)
    {
        SeStartFix(1, 0, 0x1000, 0x1000, 0x0);

#ifdef BUILD_EU_VERSION
        mc_check_timer = 0;
#endif

        return 1;
    }

#ifdef BUILD_EU_VERSION
    if (mc_check_timer >= 1)
    {
        mc_check_timer--;

        return 0;
    }
#endif

    switch (mc_ctrl.sub_step)
    {
    case 0:
        mcAcsReq(7);

        mc_ctrl.sub_step = 1;
    break;
    default:
        if (mcAcsMain() != 0)
        {
            mc_ctrl.sub_step = 0;

            if (mc_ctrl.acs.sta == 0)
            {
                if (mc_ctrl.msg_no == 6)
                {
                    mcSubInit();

                    return 3;
                }
            }
            else
            {
                return 2;
            }
        }
    break;
    }

    return 0;
}

char mcCtrlYesno(int def_pos)
{
    if (mc_ctrl.sub_step == MC_INIT)
    {
        mc_ctrl.sub_step = 1;
        mc_ctrl.now_cur = def_pos;
    }

#ifdef BUILD_EU_VERSION
    if (mc_ctrl.step != MC_SEL_NEW_OR_LOAD && mc_ctrl.step != MC_SEL_GOTITLE)
#else
    if (mc_ctrl.step != MC_SEL_NEW_OR_LOAD)
#endif
    {
        if (mc_ctrl.sub_step == MC_CHECK)
        {
            mcAcsReq(7);

            mc_ctrl.sub_step = 2;
        }
        else  if (mcAcsMain() != 0)
        {
            if (mc_ctrl.acs.sta == 0)
            {
                mc_ctrl.sub_step = 1;
            }
            else
            {
                return 4;
            }
        }
    }

    if (pad[0].one & 0x40)
    {
        SeStartFix(1, 0, 0x1000, 0x1000, 0);

        if (mc_ctrl.now_cur == 0)
        {
            return 1;
        }

        return 2;
    }
    else if (pad[0].one & 0x10)
    {
        SeStartFix(3, 0, 0x1000, 0x1000, 0);

        return 3;
    }
    else
    {
        if (*key_now[2] == 1 || Ana2PadDirCnt(3) == 1 || *key_now[3] == 1 || Ana2PadDirCnt(1) == 1)
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            mc_ctrl.now_cur ^= 1;
        }
    }

    return 0;
}

char mcCtrl3taku(int def_pos)
{
    if (mc_ctrl.sub_step == 0)
    {
        mc_ctrl.sub_step = 1;
        mc_ctrl.now_cur = def_pos;
    }

    if (pad[0].one & 0x40)
    {
        SeStartFix(1, 0, 0x1000, 0x1000, 0);

        return mc_ctrl.now_cur + 1;
    }

    if (pad[0].one & 0x10)
    {
        SeStartFix(3, 0, 0x1000, 0x1000, 0);

        return 3;
    }

    if (*key_now[2] == 1 || Ana2PadDirCnt(3) == 1)
    {
        SeStartFix(0, 0, 0x1000, 0x1000, 0);

        if (--mc_ctrl.now_cur < 0)
        {
            mc_ctrl.now_cur = 2;
        }
    }
    else if (*key_now[3] == 1 || Ana2PadDirCnt(1)== 1)
    {
        SeStartFix(0, 0, 0x1000, 0x1000, 0);

        if (++mc_ctrl.now_cur > 2)
        {
            mc_ctrl.now_cur = 0;
        }
    }

    return 0;
}

void mcInit(u_char mode, u_int *work_addr, u_char msn_flg)
{
    mcSetMessage(MC_MSG_NONE);

    mc_ctrl.step = 0;
    mc_ctrl.mode = mode;
    mc_ctrl.stepbak = 0;
    mc_ctrl.port = 0;
    mc_ctrl.port_mes = 1;
    mc_ctrl.slot = 0;
    mc_ctrl.work_addr = work_addr;

    mcSubInit();

    mc_game_size = mcGetFileSizeCeil(5, 4);
    mc_album_size = mcGetFileSizeCeil(8, 4);

    mc_msn_flg = msn_flg;

    sys_wrk.sreset_ng = 1;
}

void mcSubInit()
{
    u_int i;

    mc_ctrl.sub_step = 0;
    mc_ctrl.now_cur = 0;
    mc_ctrl.sel_cur = 0;
    mc_ctrl.dir_size = 0;
    mc_ctrl.sel_file = 0;

    memset(mc_ctrl.rw.path, 0, sizeof(mc_ctrl.rw.path));
    memset(mc_ctrl.rw.name, 0, sizeof(mc_ctrl.rw.name));
    memset(mc_ctrl.rw.dir, 0, sizeof(mc_ctrl.rw.dir));

    mcClearHeader();

    mc_ctrl.album.file_num = 0;
    mc_ctrl.album.type = 0;

    for (i = 0; i < 5; i++) {
        mc_ctrl.album.file_no[i] = 0xff;
        mc_ctrl.album.sta[i] = 0xff;
    }

    mc_ctrl.dir.file_num = 0;
    mc_ctrl.dir.gamefile_num = 0;
    mc_ctrl.dir.size = 0;

#ifdef BUILD_EU_VERSION
    mc_check_timer = 0;
#endif
}

void mcClearHeader()
{
    u_int i;

    for (i = 0; i < 16; i++)
    {
        if (mc_ctrl.mode == MC_MODE_GAMESAVE || mc_ctrl.mode == MC_MODE_GAMELOAD || mc_ctrl.mode == MC_MODE_STARTCHECK) {
            mc_header_buf[i].game.map_flg = 0;
            mc_header_buf[i].game.msn_no = 0xff;
            mc_header_buf[i].game.room_no = 0xff;
            mc_header_buf[i].game.hour = 0xff;
            mc_header_buf[i].game.minute = 0xff;
            mc_header_buf[i].game.sec = 0xff;
            mc_header_buf[i].game.msn_flg = 0;
#ifdef BUILD_EU_VERSION
            mc_header_buf[i].game.language = 0;
#endif
            mc_header_buf[i].game.clear_flg = 0;
            mc_header_buf[i].game.difficult = 0;
        }
        else
        {
            mc_header_buf[i].album.map_flg = 0;
            mc_header_buf[i].album.photo_num = 0;
            mc_header_buf[i].album.type = 0;
            mc_header_buf[i].album.pad[0] = 0xff;
            mc_header_buf[i].album.pad[1] = 0xff;
            mc_header_buf[i].album.pad[2] = 0xff;
            mc_header_buf[i].album.pad[3] = 0xff;
            mc_header_buf[i].album.pad[4] = 0xff;
        }
    }
}

void mcEnd()
{
    sys_wrk.sreset_ng = 0;
#ifdef BUILD_EU_VERSION
    sys_wrk.language = mc_language & 0x7f;
    sys_wrk.pal_disp_mode = (mc_language & 0x80) != 0;
#endif
}

void mcSetStep(int step, int msg_no)
{
#ifdef BUILD_EU_VERSION
    float rate;
#endif

    mcAcsEnd();
    mcSetMessage(msg_no);

    mc_ctrl.step = step;
    mc_ctrl.sub_step = 0;

#ifdef BUILD_EU_VERSION
    if (step != MC_DISP_ERR)
    {
        return;
    }

    if (msg_no == MC_MSG_ERR_NOCARD)
    {
        return;
    }

    if (sys_wrk.pal_disp_mode == 0)
    {
        rate = 1.2f;
    }
    else
    {
        rate = 1.0f;
    }

    mc_check_timer = 180.0f / rate;
#endif
}

void mcSetStepAfterCheck()
{
    if (mc_ctrl.mode == MC_MODE_GAMESAVE || mc_ctrl.mode == MC_MODE_ALBUMSAVE)
    {
        switch(mc_ctrl.acs.sta)
        {
        case 0:
            mcSetStep(MC_SEL_FILE, MC_MSG_SEL_SAVEFILE);

            if (mc_ctrl.mode == MC_MODE_ALBUMSAVE && is_newdata_albmside != 0 && mcCheckEmpty(0) == 0)
            {
                mc_ctrl.sel_cur = mc_ctrl.album.file_num;
            }
        break;
        case 4:
            mcSetStep(MC_SEL_MAKE_DIR, MC_MSG_SEL_MAKE_DIR);
        break;
        case 1:
            mcSetStep(MC_SEL_FORMAT, MC_MSG_SEL_FORMAT);
        break;
        case 2:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOCARD);
        break;
        case 3:
            if (mc_ctrl.mode == MC_MODE_GAMESAVE || mc_ctrl.mode == MC_MODE_GAMELOAD || mc_ctrl.mode == MC_MODE_STARTCHECK)
            {
                mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOEMPTY_GAME);
            }
            else
            {
                mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOEMPTY_ALBUM);
            }
        break;
        case 10:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_ACCESS);
        break;
        case 9:
            mcSetStep(MC_SEL_REMAKE_DIR, MC_MSG_SEL_REMAKE_DIR);
        break;
        default:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_ACCESS);
        break;
        }
    }
    else
    {
        switch(mc_ctrl.acs.sta)
        {
        case 0:
            mcSetStep(MC_SEL_FILE, MC_MSG_SEL_LOADFILE);
        break;
        case 1:
            if (mc_ctrl.mode == MC_MODE_GAMESAVE || mc_ctrl.mode == MC_MODE_GAMELOAD || mc_ctrl.mode == MC_MODE_STARTCHECK)
            {
                mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOGAMEFILE);
            }
            else
            {
                mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOALBUMFILE);
            }
        break;
        case 2:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOCARD);
        break;
        case 4:
            if (mc_ctrl.mode == MC_MODE_GAMESAVE || mc_ctrl.mode == MC_MODE_GAMELOAD || mc_ctrl.mode == MC_MODE_STARTCHECK)
            {
                mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NODIR);
            } else {
                mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOALBUMFILE);
            }
        break;
        case 5:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOGAMEFILE);
        break;
        case 10:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_ACCESS);
        break;
        case 9:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_ILLEGUL_LOAD);
        break;
        default:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_ACCESS);
        break;
        }
    }
}

#ifdef BUILD_EU_VERSION
void mcFormatEndInit()
{
    mc_ctrl.cnt = 0;
}

int mcFormatEnd()
{
    return (mc_ctrl.cnt++ <= 90) ^ 1;
}
#endif
