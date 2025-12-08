#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "mc_album.h"

#include "os/eeiop/eese.h"
#include "main/glob.h"
#include "ingame/menu/ig_menu.h"
#include "outgame/memory_album.h"
#include "mc/mc_acs.h"
#include "mc/mc_disp.h"
#include "mc/mc_exec.h"
#include "mc/mc_main.h"

char mcAlbumSaveMain()
{
    char end_flg;
    static char newfile_flg;

    end_flg = 0;

    mc_ctrl.stepbak = mc_ctrl.step;

    switch(mc_ctrl.step)
    {
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
        if (mcCtrlCheckAlbum() != 0)
        {
            mcSetStepAfterCheck();
        }
    break;
    case MC_SEL_FILE:
        newfile_flg = 0;

        switch(mcCtrlFileSel())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            mcSetStep(MC_SEL_SAVE, MC_MSG_SEL_OVERWRITE);
        break;
        case 2:
            mcSetStep(MC_SEL_SLOT, MC_MSG_SEL_SLOT);
        break;
        case 3:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOCARD);
        break;
        case 4:
            mcSetStep(MC_SEL_ALBUM_TYPE, MC_MSG_SEL_ALBUM_TYPE);

            newfile_flg = 1;
        break;
        case 5:
            mcSetStep(MC_SEL_REMAKE_DIR, MC_MSG_SEL_REMAKE_DIR);
        }
    break;
    case MC_SEL_ALBUM_TYPE:
        switch (mcCtrlAlbumSel())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            mcSetStep(MC_SEL_ALBUM_CHECK, MC_MSG_SEL_ALBUM_CHECK);
        break;
        case 2:
            mcSetStep(MC_SEL_FILE, MC_MSG_SEL_SAVEFILE);
        break;
        case 3:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOCARD);
        break;
        }
    break;
    case MC_SEL_ALBUM_CHECK:
        switch(mcCtrlYesno(1))
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            if (newfile_flg == 1)
            {
                mcSetStep(MC_MAKE_DIR, MC_MSG_MAKE_DIR);
            }
            else
            {
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
            }
        break;
        case 2:
        case 3:
            mcSetStep(MC_SEL_ALBUM_TYPE, MC_MSG_SEL_ALBUM_TYPE);
        break;
        case 4:
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
            if (is_newdata_albmside != 0)
            {
                mcSetStep(MC_SEL_ALBUM_TYPE, MC_MSG_SEL_ALBUM_TYPE);
            }
            else
            {
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
            }
        break;
        case 2:
        case 3:
            mcSetStep(MC_SEL_FILE, MC_MSG_SEL_SAVEFILE);
        break;
        case 4:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_NOCARD);
        break;
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
            mcSetStep(MC_SEL_FILE, MC_MSG_SEL_SAVEFILE);
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

            mc_ctrl.card[0].free = 0x578; // sizeof ??
            mc_ctrl.album.type = 0;
#endif
        break;
        case 2:
            mcSetStep(MC_DISP_ERR, MC_MSG_ERR_FORMAT);
        break;
        }
    break;
    case MC_INIT:
        mcSetStep(MC_SEL_SLOT, MC_MSG_SEL_SLOT);
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
            mcSetMessage(6);
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
            mcSetStep(MC_SEL_FILE, MC_MSG_SEL_SAVEFILE);
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
            mc_ctrl.album.type = 0;
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
            mc_ctrl.album.sta[mc_ctrl.sel_cur] = 0;
            mc_ctrl.album.file_no[mc_ctrl.sel_cur] = mc_ctrl.sel_file;
        }

        if (pad[0].one & 0x50)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);

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

            mc_ctrl.card[0].free = 0x578;
            mc_ctrl.album.type = 0;
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

char mcCtrlCheckAlbum(void)
{
    static int loop;

    switch (mc_ctrl.sub_step)
    {
    case 0:
        mc_ctrl.sub_step = 1;

        mcAcsReq(0);
    break;
    case 1:
        if (mcAcsMain() != 0)
        {
            if (mc_ctrl.acs.sta == 0)
            {
                mc_ctrl.sub_step = 2;
            }
            else
            {
                return 1;
            }
        }
    break;
    case 2:
        mc_ctrl.sub_step = 3;
#ifdef BUILD_EU_VERSION
        sprintf(mc_ctrl.rw.path, "%s%s", "/", "BESLES-50821albm");
#else
        sprintf(mc_ctrl.rw.path, "%s%s", "/", "BASLUS-20388albm");
#endif
        sprintf(mc_ctrl.rw.name, "%s*", mc_ctrl.rw.path);

        mcAcsReq(1);
    break;
    case 3:
        if (mcAcsMain() != 0)
        {
            if (mc_ctrl.acs.sta == 0 || mc_ctrl.acs.sta == 4)
            {
                mcCheckRootList();

                if (mc_ctrl.album.file_num == 0)
                {
                    if (mc_ctrl.mode == 1 || mc_ctrl.mode == 3 || mc_ctrl.mode == 4 || mc_ctrl.mode == 5 || mc_ctrl.mode == 6)
                    {
                        mc_ctrl.acs.sta = 4;

                    }
                    else
                    {
                        if (mcCheckEmpty(0) != 0)
                        {
                            mc_ctrl.acs.sta = 3;
                        }
                        else
                        {
                            mc_ctrl.acs.sta = 0;
                            mc_ctrl.album.file_no[0] = 0;
                        }
                    }
                    return 1;
                }

                mc_ctrl.sub_step = 4;

                loop = 0;
            }
            else
            {
                return 1;
            }
        }
    break;
    case 4:
        mc_ctrl.sub_step = 5;
        mc_ctrl.sel_file = mc_ctrl.album.file_no[loop];

        mcSetPathDir(mc_ctrl.sel_file);
        sprintf(mc_ctrl.rw.name, "%s*", mc_ctrl.rw.path);

        mcAcsReq(1);
    break;
    case 5:
        if (mcAcsMain() != 0)
        {
            if (mc_ctrl.acs.sta == 0)
            {
                if (mcCheckFileList() != 0)
                {
                    if (mcCheckEmpty(mc_ctrl.dir_size) != 0)
                    {
                        mc_ctrl.album.sta[loop] = 3;
                    }
                    else
                    {
                        mc_ctrl.album.sta[loop] = 9;
                    }

                    if (mc_ctrl.album.file_num <= ++loop)
                    {
                        return 1;
                    }

                    mc_ctrl.sub_step = 4;

                }
                else
                {
                    mc_ctrl.album.sta[loop] = mc_ctrl.acs.sta;
                    mc_ctrl.sub_step = 6;
                }
            }
            else
            {
                return 1;
            }

        }
    break;
    case 6:
        mcAcsFileReq(mc_ctrl.album.file_no[loop] + 8, 1, 1);

        mc_ctrl.sub_step = 7;
    break;
    case 7:
        if (mcReadFile() != 0)
        {
            if (mc_ctrl.acs.sta == 0)
            {
                mcSetLoadFile(mc_ctrl.work_addr, 8);

                mc_header_buf[mc_ctrl.album.file_no[loop]] = mc_header;

                if (mc_ctrl.album.file_num <= ++loop)
                {
                    return 1;
                }

                mc_ctrl.sub_step = 4;
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

char mcCtrlAlbumSel()
{
    if (mc_ctrl.sub_step == 0)
    {
        mc_ctrl.now_cur = mc_ctrl.album.type;
        mc_ctrl.sub_step = 1;
    }

    if (mc_ctrl.sub_step == 1)
    {
        mcAcsReq(7);

        mc_ctrl.sub_step = 2;
    }
    else  if (mcAcsMain() != 0)
    {
        if (mc_ctrl.acs.sta != 0)
        {
            return 3;
        }

        mc_ctrl.sub_step = 1;
    }

    mc_ctrl.album.type = mc_ctrl.now_cur;

    if (pad[0].one & 0x40)
    {
        SeStartFix(1, 0, 0x1000, 0x1000, 0);

        return 1;
    }
    else if (pad[0].one & 0x10)
    {
        SeStartFix(3, 0, 0x1000, 0x1000, 0);

        return 2;
    }
    else if (*key_now[3] == 1 || Ana2PadDirCnt(1) == 1)
    {
        SeStartFix(0, 0, 0x1000, 0x1000, 0);

        if (mc_ctrl.now_cur < 3)
        {
            if (++mc_ctrl.now_cur >= 3)
            {
                mc_ctrl.now_cur = 0;
            }
        }
        else if (++mc_ctrl.now_cur >= 6)
        {
            mc_ctrl.now_cur = 3;
        }
    }
    else if (*key_now[2] == 1 || Ana2PadDirCnt(3) == 1)
    {
        SeStartFix(0, 0, 0x1000, 0x1000, 0);

        if (mc_ctrl.now_cur < 3)
        {
            if (--mc_ctrl.now_cur < 0)
            {
                mc_ctrl.now_cur = 2;
            }
        }
        else if (--mc_ctrl.now_cur < 3)
        {
            mc_ctrl.now_cur = 5;
        }
    } else if (*key_now[0] == 1 || Ana2PadDirCnt(0) == 1)
    {
        SeStartFix(0, 0, 0x1000, 0x1000, 0);

        mc_ctrl.now_cur -= 3;

        if (mc_ctrl.now_cur < 0)
        {
            mc_ctrl.now_cur += 6;
        }
    } else if (*key_now[1] == 1 || Ana2PadDirCnt(2) == 1)
    {
        SeStartFix(0, 0, 0x1000, 0x1000, 0);

        mc_ctrl.now_cur += 3;

        if (mc_ctrl.now_cur > 5)
        {
            mc_ctrl.now_cur -= 6;
        }
    }

    return 0;
}
