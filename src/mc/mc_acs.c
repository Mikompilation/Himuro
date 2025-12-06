#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "mc_acs.h"

#include "sce/libmc.h"

#include "mc/mc_main.h"

char (*mc_acs[])() = {
    mcSlotCheck,
    mcFileCheck,
    mcOpen,
    mcLoad,
    mcSave,
    mcMakeDir,
    mcFormat,
    mcExistCheck,
    mcClose,
    mcDelete,
};

char mcAcsMain()
{
    char end_flg;

    end_flg = 0;

    if (mc_ctrl.acs.req == 0)
    {
        return 1;
    }

    if (mc_ctrl.acs.sync == 0)
    {
        switch (mc_acs[mc_ctrl.acs.func_no]())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            mc_ctrl.acs.sync = 1;
        break;
        default:
            end_flg = 1;
        break;
        }
    }
    else
    {
        switch (mcSyncNB())
        {
        case 0:
            // do nothing ...
        break;
        case 1:
        case 2:
            end_flg = 1;
        break;
        case 3:
            mc_ctrl.acs.sync = 0;
        break;
        }
    }

    if (end_flg)
    {
        mcAcsEnd();

        return 1;
    }

    return 0;
}

void mcAcsReq(int func_no)
{
    if (func_no < 10)
    {
        mc_ctrl.acs.sync = 0;
        mc_ctrl.acs.func_no = func_no;
        mc_ctrl.acs.retry_cnt = 5;
        mc_ctrl.acs.sta = 0;
        mc_ctrl.acs.req = 1;
    }
}

void mcAcsEnd()
{
    sceMcSync(0, 0, 0);

    mc_ctrl.acs.req = 0;
}

char mcSlotCheck()
{
    if (sceMcGetInfo(mc_ctrl.port, mc_ctrl.slot, &mc_ctrl.card[0].type, &mc_ctrl.card[0].free, &mc_ctrl.card[0].format) == 0)
    {
        return 1;
    }

    if (--mc_ctrl.acs.retry_cnt < 1)
    {
        mc_ctrl.acs.sta = 10;

        return 2;
    }

    return 0;
}

char mcFileCheck()
{
    if (sceMcGetDir(mc_ctrl.port, mc_ctrl.slot, mc_ctrl.rw.name, 0, 18, mc_ctrl.dir.table) == 0)
    {
        return 1;
    }

    if (--mc_ctrl.acs.retry_cnt < 1)
    {
        mc_ctrl.acs.sta = 10;

        return 2;
    }

    return 0;
}

char mcOpen()
{
    if (sceMcOpen(mc_ctrl.port,mc_ctrl.slot,mc_ctrl.rw.name,mc_ctrl.rw.mode) == 0)
    {
        return 1;
    }

    if (--mc_ctrl.acs.retry_cnt < 1)
    {
        if (mc_ctrl.mode == MC_MODE_GAMESAVE || mc_ctrl.mode == MC_MODE_ALBUMSAVE)
        {
            mc_ctrl.acs.sta = 7;
        }
        else
        {
            mc_ctrl.acs.sta = 6;
        }

        return 2;
    }

    return 0;
}

char mcClose()
{
    if (sceMcClose(mc_ctrl.rw.fd) == 0)
    {
        return 1;
    }

    if (--mc_ctrl.acs.retry_cnt < 1)
    {
        if (mc_ctrl.mode == MC_MODE_GAMESAVE || mc_ctrl.mode == MC_MODE_ALBUMSAVE)
        {
            mc_ctrl.acs.sta = 7;
        }
        else
        {
            mc_ctrl.acs.sta = 6;
        }

        return 2;
    }

    return 0;
}

char mcLoad()
{
    if (sceMcRead(mc_ctrl.rw.fd, mc_ctrl.rw.buf, mc_ctrl.rw.size) == 0)
    {
        return 1;
    }

    if (--mc_ctrl.acs.retry_cnt < 1)
    {
        mc_ctrl.acs.sta = 6;

        return 2;
    }

    return 0;
}

char mcSave()
{
    if (sceMcWrite(mc_ctrl.rw.fd, mc_ctrl.rw.buf, mc_ctrl.rw.size) == 0)
    {
        return 1;
    }

    if (--mc_ctrl.acs.retry_cnt < 1)
    {
        mc_ctrl.acs.sta = 7;

        return 2;
    }

    return 0;
}

char mcExistCheck()
{
    if (sceMcGetInfo(mc_ctrl.port, mc_ctrl.slot, &mc_ctrl.card[0].type, &mc_ctrl.card[0].free, &mc_ctrl.card[0].format) == 0)
    {
        return 1;
    }

    if (--mc_ctrl.acs.retry_cnt < 1)
    {
        mc_ctrl.acs.sta = 10;

        return 2;
    }

    return 0;
}

char mcMakeDir()
{
    if (sceMcMkdir(mc_ctrl.port, mc_ctrl.slot, mc_ctrl.rw.dir) == 0)
    {
        return 1;
    }

    if (--mc_ctrl.acs.retry_cnt < 1)
    {
        mc_ctrl.acs.sta = 7;

        return 2;
    }

    return 0;
}

char mcFormat()
{
    if (sceMcFormat(mc_ctrl.port, mc_ctrl.slot) == 0)
    {
        return 1;
    }

    if (--mc_ctrl.acs.retry_cnt < 1)
    {
        mc_ctrl.acs.sta = 8;

        return 2;
    }

    return 0;
}

char mcDelete()
{
    if (sceMcDelete(mc_ctrl.port, mc_ctrl.slot, mc_ctrl.rw.name) == 0)
    {
        return 1;
    }

    if (--mc_ctrl.acs.retry_cnt < 1)
    {
        mc_ctrl.acs.sta = 7;

        return 2;
    }

    return 0;
}

char mcSyncNB()
{
    int sync;
    int result;
    int cmd;

    sync = sceMcSync(1, &cmd, &result);

    if (sync == 0)
    {
        return 0;
    }

    if (sync == 1)
    {
        switch (cmd)
        {
        case 1:
            if (mc_ctrl.step == 1)
            {
                if (mc_ctrl.card[0].format == 1 && mc_ctrl.card[0].type == 2)
                {
                    mc_ctrl.acs.sta = 0;
                    break;
                }

                if (--mc_ctrl.acs.retry_cnt < 1)
                {
                    mcSyncErrWork(cmd, result);
                    return 2;
                }
            }
            else
            {
                mc_ctrl.acs.card_sta = result;

                if (result == 0)
                {
                    mc_ctrl.acs.card_sta = result; // HACK: fixes regswap

                    if (mc_ctrl.card[0].type == 2)
                    {
                        mc_ctrl.acs.sta = 0;

                        break;
                    }

                    mc_ctrl.acs.card_sta = -11;
                }

                if (--mc_ctrl.acs.retry_cnt < 1)
                {
                    mcSyncErrWork(cmd, result);

                    return 2;
                }
            }

            return 3;
        break;
        case 2:
            if (result >= 0)
            {
                mc_ctrl.rw.fd = result;
                mc_ctrl.acs.sta = 0;

                break;
            }

            if (--mc_ctrl.acs.retry_cnt < 1)
            {
                mcSyncErrWork(cmd, result);

                return 2;
            }

            return 3;
        break;
        case 13:
            if (result >= 0)
            {
                mc_ctrl.dir.file_num = result;
                mc_ctrl.acs.sta = 0;

                break;
            }

            if (result == -4)
            {
                mc_ctrl.dir.file_num = 0;

                mcSyncErrWork(cmd, -4);

                break;
            }
            else if (--mc_ctrl.acs.retry_cnt < 1)
            {
                mcSyncErrWork(cmd, result);

                return 2;
            }

            return 3;
        break;
        default:
            if (result >= 0)
            {
                mc_ctrl.acs.sta = 0;

                break;
            }

            if (--mc_ctrl.acs.retry_cnt < 1)
            {
                mcSyncErrWork(cmd, result);

                return 2;
            }

            return 3;
        break;
        }
    }
    else
    {
        if (sync == -1)
        {
            if (--mc_ctrl.acs.retry_cnt < 1)
            {
                mc_ctrl.acs.sta = 10;

                return 2;
            }

            return 3;
        }
    }

    return 1;
}

void mcSyncErrWork(int cmd, int result)
{
    switch(cmd)
    {
    case 1:
        if (mc_ctrl.step == 1)
        {
            if (mc_ctrl.card[0].type == 2 && mc_ctrl.card[0].format == 0)
            {
                mc_ctrl.acs.sta = 1;
            }
            else
            {
                mc_ctrl.acs.sta = 2;
            }
        }
        else
        {
            mc_ctrl.acs.sta = 2;
        }
    break;
    case 2:
        switch (result)
        {
        case -2:
            mc_ctrl.acs.sta = 1;
        break;
        case -3:
        case -4:
        case -5:
        case -7:
            mc_ctrl.acs.sta = 10;
        break;
        default:
            mc_ctrl.acs.sta = 2;
        break;
        }
    break;
    case 3:
        switch (result)
        {
        case -2:
            mc_ctrl.acs.sta = 1;
        break;
        case -4:
            mc_ctrl.acs.sta = 10;
        break;
        default:
            mc_ctrl.acs.sta = 2;
        break;
        }
    break;
    case 5:
        switch (result)
        {
        case -2:
            mc_ctrl.acs.sta = 1;
        break;
        case -3:
        case -4:
        case -5:
            mc_ctrl.acs.sta = 10;
        break;
        default:
            mc_ctrl.acs.sta = 2;
        break;
        }
    break;
    case 6:
        switch (result)
        {
        case -2:
            mc_ctrl.acs.sta = 1;
        break;
        case -3:
        case -4:
        case -5:
        case -8:
            mc_ctrl.acs.sta = 10;
        break;
        default:
            mc_ctrl.acs.sta = 2;
        break;
        }
    break;
    case 15:
        switch (result)
        {
        case -2:
            mc_ctrl.acs.sta = 1;
        break;
        case -4:
        case -5:
        case -6:
            mc_ctrl.acs.sta = 10;
        break;
        default:
            mc_ctrl.acs.sta = 2;
        break;
        }
    break;
    case 11:
        switch (result)
        {
        case -2:
            mc_ctrl.acs.sta = 1;
        break;
        case -3:
        case -4:
            mc_ctrl.acs.sta = 10;
        break;
        default:
            mc_ctrl.acs.sta = 2;
        break;
        }
    break;
    case 13:
        switch (result)
        {
        case -2:
            mc_ctrl.acs.sta = 1;
        break;
        case -4:
            mc_ctrl.acs.sta = 4;
        break;
        case -5:
            mc_ctrl.acs.sta = 10;
        break;
        default:
            mc_ctrl.acs.sta = 2;
        break;
        }
    break;
    case 14:
        switch (result)
        {
        case -2:
            mc_ctrl.acs.sta = 1;
        break;
        case -4:
            mc_ctrl.acs.sta = 10;
        break;
        default:
            mc_ctrl.acs.sta = 2;
        break;
        }
    break;
    case 16:
    default:
        mc_ctrl.acs.sta = 2;
    break;
    }
}
