#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "mc_start.h"

#include "os/eeiop/eese.h"
#include "main/glob.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/item.h"
#include "mc/mc_main.h"
#include "mc/mc_acs.h"
#include "mc/mc_disp.h"
#include "mc/mc_exec.h"

MC_START_CHECK mc_start_check[] = {
    {0}, {0},
};

#define MC_WORK_ADDRESS 0x420000

char mcStartCheckMain()
{
    static char err_flg = 0;

    AnaPonChk();

    mc_ctrl.stepbak = mc_ctrl.step;

    switch (mc_ctrl.step)
    {
    case 0:
        mcSetStep(4, MC_MSG_NONE);
    break;
    case 4:
        if (err_flg == 1)
        {
            mcSetStep(1, MC_MSG_NONE);
        }
        else
        {
            mcSetStep(1, MC_MSG_NONE);
        }
    case 1:
        if (mcCtrlCheck() != 0)
        {
            mcStartCheckSet();

            if (mc_ctrl.port == 0)
            {
                if (err_flg == 1)
                {
                    mcSetStep(4, MC_MSG_NONE);
                }
                else
                {
                    mcSetStep(4, MC_MSG_NONE);
                }

                mc_ctrl.port = 1;
                mc_ctrl.port_mes = 2;
            }
            else
            {
                switch(mcStartCheckResult())
                {
                    case 0:
                        mcSetStep(5, MC_MSG_SEL_START);
                    break;
                    case 1:
                        mcSetStep(5, MC_MSG_SEL_START2);
                    break;
                    case 3:
                        mcSetStep(5, MC_MSG_SEL_START3);
                    break;
                    default:
                        return 1;
                    break;
                }
            }
        }
    break;
    case 5:
        err_flg = 1;
        switch (mcStartCheckYesno(1))
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            return 1;
        break;
        case 2:
            mcInit(MC_MODE_STARTCHECK, NULL, 0);
            mcSetStep(1, MC_MSG_NONE);
        break;
        }
    break;
    }

    mcDispMessage();

    return 0;
}

void mcStartCheckSet()
{
    mc_start_check[mc_ctrl.port].type = mc_ctrl.card[0].type;
    mc_start_check[mc_ctrl.port].free = mc_ctrl.card[0].free;
    mc_start_check[mc_ctrl.port].format = mc_ctrl.card[0].format;

    if (mc_ctrl.acs.sta == 0)
    {
        mc_start_check[mc_ctrl.port].dir = 1;
    }
    else
    {
        mc_start_check[mc_ctrl.port].dir = 0;
    }
}

char mcStartCheckResult()
{
    u_int i;
    u_int card_exist;

    card_exist = 0;

    for (i = 0; i < 2; i++)
    {
        if (mc_start_check[i].type == 2)
        {
            card_exist = 1;
        }
    }

    if (card_exist == 0)
    {
        return 1;
    }

    for (i = 0; i < 2; i++)
    {
        if (mc_start_check[i].dir == 1)
        {
            return 2;
        }
    }

    for (i = 0; i < 2; i++)
    {
        if (mc_start_check[i].type == 2 && mc_start_check[i].format == 0)
        {
            return 2;
        }

        if (mc_start_check[i].free > 0x6fc) // sizeof / sum of sizeofs ??
        {
            return 2;
        }
    }

    if (mc_start_check[0].type != 2)
    {
        mc_ctrl.port_mes = 2;
    }
    else
    {
        mc_ctrl.port_mes = 1;
    }

    if (mc_start_check[0].type == 2 && mc_start_check[1].type == 2)
    {
        if (mc_start_check[0].free < 0x6fd && mc_start_check[1].free < 0x6fd) // sizeof / sum of sizeofs ??
        {
            return 3;
        }
    }

    return 0;
}

char mcStartCheckYesno(int def_pos)
{
    static int backup[2];

    if (mc_ctrl.sub_step == 0)
    {
        mc_ctrl.sub_step = 1;
        mc_ctrl.now_cur = def_pos;

        backup[0] = backup[1] = 0xff;
    }

    YesNoCrslOKR(0x14000, mc_ctrl.now_cur * 0x84 + 0xb4, 325.0f, 0x808080, 128.0f, 2.0f);

    if (mc_ctrl.sub_step == 1)
    {
        mcAcsReq(7);

        mc_ctrl.sub_step = 2;
    }
    else if (mcAcsMain() != 0)
    {
        if (backup[mc_ctrl.port] != 0xff)
        {
            if (mc_ctrl.acs.card_sta < -10 && backup[mc_ctrl.port ^ 1] < -10)
            {
                mcSetMessage(MC_MSG_SEL_START2);
            }
            else if (backup[mc_ctrl.port] != mc_ctrl.acs.card_sta)
            {
                return 2;
            }
            else if (backup[mc_ctrl.port] == 0)
            {
                if (mc_ctrl.msg_no == 28)
                {
                    return 2;
                }
            }
        }

        backup[mc_ctrl.port] = mc_ctrl.acs.card_sta;

        mc_ctrl.sub_step = 1;
        mc_ctrl.port = mc_ctrl.port ^ 1;
    }

    if (pad[0].one & 0x40)
    {
        SeStartFix(1, 0, 0x1000, 0x1000, 0);

        return mc_ctrl.now_cur != 0 ? 2 : 1;
    }

    if (pad[0].one & 0x10)
    {
        SeStartFix(3, 0, 0x1000, 0x1000, 0);

        return 2;
    }

    if (*key_now[2] == 1 || Ana2PadDirCnt(3) == 1 || *key_now[3] == 1 || Ana2PadDirCnt(1) == 1)
    {
        SeStartFix(0, 0, 0x1000, 0x1000, 0);

        mc_ctrl.now_cur ^= 1;
    }

    return 0;
}

#ifdef BUILD_EU_VERSION
char mcCtrlCheckLanguage();

char mcLanguageLoadMain()
{
    mc_ctrl.stepbak = mc_ctrl.step;

    switch (mc_ctrl.step)
    {
    case 0:
        mcSetStep(MC_CHECK, MC_MSG_NONE);
    break;
    case 1:
        switch (mcCtrlCheckLanguage())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            if (mc_ctrl.port != 0)
            {
                return 1;
            }

            mcInit(7, (u_int *)MC_WORK_ADDRESS, 0);

            mcSetStep(MC_CHECK, MC_MSG_NONE);

            mc_ctrl.port = 1;
        break;
        case 2:
            mc_language = mc_header_buf[0].game.language;

            return 2;
        break;
        }
    break;
    }

    return 0;
}

char mcCtrlCheckLanguage()
{
    switch(mc_ctrl.sub_step)
    {
    case 0:
        mc_ctrl.sub_step = 1;

        mcAcsReq(0);
    break;
    case 1:
        if (mcAcsMain() != 0)
        {
            if (mc_ctrl.acs.sta != 0)
            {
                return 1;
            }

            mc_ctrl.sub_step = 4;
        }
    break;
    case 4:
        mc_ctrl.sub_step = 5;

        mcSetPathDir(0);

        sprintf(mc_ctrl.rw.name, "%s*", mc_ctrl.rw.path);

        mcAcsReq(1);
    break;
    case 5:
        if (mcAcsMain() != 0)
        {
            if (mc_ctrl.acs.sta != 0)
            {
                return 1;
            }

            if (mcCheckFileList() != 0)
            {
                return 1;
            }

            mc_ctrl.sub_step = 6;
        }
    break;
    case 6:
        mcAcsFileReq(5, 1, 1);

        mc_ctrl.sub_step = 7;
    break;
    case 7:
        if (mcReadFile() != 0) {
            if (mc_ctrl.acs.sta != 0)
            {
                return 1;
            }

            mcSetLoadFile(mc_ctrl.work_addr, 5);

            mc_header_buf[0] = mc_header;

            if (mc_header_buf[0].game.map_flg == 0)
            {
                return 1;
            }

            return 2;
        }
    break;
    }

    return 0;
}
#endif
