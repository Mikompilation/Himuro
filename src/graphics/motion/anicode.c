#include "common.h"
#include "typedefs.h"
#include "anicode.h"

#include "main/glob.h"
#include "os/eeiop/se_plyr.h"
#include "graphics/graph2d/effect_oth.h"
#include "graphics/graph2d/effect_ene.h"
#include "graphics/motion/mime.h"
#include "graphics/motion/mdlact.h"
#include "graphics/motion/motion.h"
#include "ingame/map/door_ctl.h"


void motAniCodeClearBuf(ANI_CTRL *ani_ctrl)
{
    u_int i;

    for (i = 0; i < 10; i++)
    {
        ani_ctrl->anm.buf[i].stat = 0;
        ani_ctrl->anm.buf[i].cnt = 0;
    }
}

void motAniCodeSetBuf(ANI_CTRL *ani_ctrl, ANI_CODE code)
{
    u_int i;
    ANI_BUF *a_buf;

    ani_ctrl->anm.code_now++;

    for (i = 0; i < 10; i++)
    {
        a_buf = &ani_ctrl->anm.buf[i];

        if (a_buf->stat == 0)
        {
            a_buf->code = code;
            a_buf->cnt = ani_ctrl->anm.timer;
            a_buf->stat = 1;

            return;
        }
    }
}

u_char motAniCodeRead(ANI_CTRL *ani_ctrl)
{
    ANI_CODE_CTRL *c_ctrl;
    int args[3];
    ANI_CODE code;

    c_ctrl = &ani_ctrl->anm;

    c_ctrl->stat = 0;

    do
    {
        code = *c_ctrl->code_now;

        GetAniCodeArgs(code, args);
        motAniCodeExec(ani_ctrl, code, args);
    } while (c_ctrl->stat == 0);

    return c_ctrl->stat == 1;
}

void motAniCodeExec(ANI_CTRL *ani_ctrl, ANI_CODE code, int *args)
{
    switch (code / 4096)
    {
    case 0:
        motAniCodeReadCTRL(ani_ctrl, args);
    break;
    case 2:
        motAniCodeReadMOT(ani_ctrl, args);
    break;
    case 1:
        motAniCodeReadTIMER(ani_ctrl, args);
    break;
    default:
        motAniCodeSetBuf(ani_ctrl, code);
    break;
    }
}

void motAniTimerCodeExec(ANI_CTRL *ani_ctrl)
{
    int args[3];
    u_int i;
    ANI_CODE code;

    for (i = 0; i < 10; i++)
    {
        if (ani_ctrl->anm.buf[i].stat == 0)
        {
            continue;
        }

        if (motGetNowFrame(&ani_ctrl->mot) == ani_ctrl->anm.buf[i].cnt)
        {
            code = ani_ctrl->anm.buf[i].code;

            GetAniCodeArgs(code, args);

            switch (code >> 12) {
            case 3:
                motAniCodeReadMIM(ani_ctrl, args);
            break;
            case 4:
                motAniCodeReadSE(ani_ctrl, args);
            break;
            case 5:
                motAniCodeReadDOOR(ani_ctrl, args);
            break;
            case 6:
                motAniCodeReadEFCT(ani_ctrl, args);
            break;
            }

            ani_ctrl->anm.buf[i].stat = 0;
            ani_ctrl->anm.code_now--;
        }
    }
}

int motAniCodeIsEnd(ANI_CODE code)
{
    int args[3];

    GetAniCodeArgs(code, args);

    if (code >> 12)
    {
        return 0;
    }

    return args[0] == 0;
}

void GetAniCodeArgs(ANI_CODE code, int *args)
{
    switch (code >> 10 & 0x3)
    {
    case 0:
        args[0] = code & 0x3ff;
        args[1] = 0;
        args[2] = 0;
    break;
    case 1:
        args[0] = code >> 3 & 0x7f;
        args[1] = code >> 0 & 0x7;
        args[2] = 0;
    break;
    case 2:
        args[0] = code >> 6 & 0xf;
        args[1] = code >> 3 & 0x7;
        args[2] = code >> 0 & 0x7;
    break;
    }
}

void motAniCodeReadCTRL(ANI_CTRL *ani_ctrl, int *args)
{
    switch (args[0])
    {
    case 0:
        ani_ctrl->anm.stat = 1;
    break;
    case 1:
        if (ani_ctrl->anm.loop_rest == 0)
        {
            ani_ctrl->anm.code_now++;
            return;
        }

        if (ani_ctrl->anm.loop_rest != 0xffffffff)
        {
            ani_ctrl->anm.loop_rest--;
        }

        ani_ctrl->anm.code_now = ani_ctrl->anm.loop_start;
    break;
    case 2:
        if (args[1] == 0)
        {
            ani_ctrl->anm.loop_rest = -1;
        }
        else
        {
            ani_ctrl->anm.loop_rest = args[1];
        }

        ani_ctrl->anm.timer = 0;
        ani_ctrl->anm.code_now++;
        ani_ctrl->anm.loop_start = ani_ctrl->anm.code_now;
    break;
    }
}

void motAniCodeReadTIMER(ANI_CTRL *ani_ctrl, int *args)
{
    ani_ctrl->anm.timer = args[0];
    ani_ctrl->anm.code_now++;
}

void motAniCodeReadMOT(ANI_CTRL *ani_ctrl, int *args)
{
    MOT_CTRL *m_ctrl;

    m_ctrl = &ani_ctrl->mot;

    m_ctrl->play_id = args[0];
    m_ctrl->dat = (u_int *)GetFileInPak(m_ctrl->top, m_ctrl->play_id);

    if (ani_ctrl->mtop != NULL)
    {
        ani_ctrl->mdat = (u_int *)GetFileInPak(ani_ctrl->mtop, m_ctrl->play_id);
    }

    ani_ctrl->ftype = args[1];
    ani_ctrl->interp_flg = 0;
    ani_ctrl->anm.stat = 2;
    ani_ctrl->anm.code_now++;
}

void motAniCodeReadMIM(ANI_CTRL *ani_ctrl, int *args)
{
    if (ani_ctrl->mim == NULL)
    {
        ani_ctrl->anm.code_now++;
        return;
    }

    switch (args[1])
    {
    case 0:
        ani_ctrl->mim[*args].loop = 0;

        if (args[2] == 0)
        {
            mimRequest(&ani_ctrl->mim[args[0]], args[1]);
        }
        else
        {
            mimRequestLastFrame(&ani_ctrl->mim[args[0]], args[1]);
        }
    break;
    case 1:
        ani_ctrl->mim[args[0]].loop = 1;

        if (ani_ctrl->mim[args[0]].stat == 1)
        {
            if (args[2] == 0)
            {
                mimRequest(&ani_ctrl->mim[args[0]], args[1]);
            }
            else
            {
                mimRequestLastFrame(&ani_ctrl->mim[args[0]], args[1]);
            }
        }
    break;
    }

    ani_ctrl->anm.code_now++;
}

void motAniCodeReadSE(ANI_CTRL *ani_ctrl, int *args)
{
    sceVu0FVECTOR pos;

    switch (args[0])
    {
    case 0:
        GetPlyrFootPos(pos,ani_ctrl, 0);
        SeReqFootStep(pos);

        ani_ctrl->anm.code_now++;
    break;
    case 1:
        GetPlyrFootPos(pos,ani_ctrl, 1);
        SeReqFootStep(pos);

        ani_ctrl->anm.code_now++;
    break;
    }
}

void motAniCodeReadDOOR(ANI_CTRL *ani_ctrl, int *args)
{
    switch(args[0])
    {
    case 0:
        door_open_ctrl.open[0] = 0;
        door_open_ctrl.open[1] = 0;
    break;
    case 1:
        door_open_ctrl.open[0] = 1;
    break;
    case 2:
        door_open_ctrl.open[1] = 1;
    break;
    case 3:
        door_open_ctrl.open[0] = 1;
        door_open_ctrl.open[1] = 1;
    break;
    case 4:
        door_open_ctrl.open[0] = 2;
        door_open_ctrl.open[1] = 2;
    break;
    }

    ani_ctrl->anm.code_now++;
}

void motAniCodeReadEFCT(ANI_CTRL *ani_ctrl, int *args)
{
    sceVu0FVECTOR pos;

    switch(args[0])
    {
    case 0:
        GetPlyrFootPos(pos, ani_ctrl, 0);

        if (IsHokoriZone() != 0)
        {
            SetDust2(pos);
        }
        else if (IsKarehaZone(plyr_wrk.move_box.pos) != 0 || IsTakenohaZone(plyr_wrk.move_box.pos) != 0)
        {
            SetLeaf(pos);
        }
    break;
    case 1:
        GetPlyrFootPos(pos, ani_ctrl, 1);

        if (IsHokoriZone() != 0)
        {
            SetDust2(pos);
        }
        else if (IsKarehaZone(plyr_wrk.move_box.pos) != 0 || IsTakenohaZone(plyr_wrk.move_box.pos) != 0)
        {
            SetLeaf(pos);
        }
    break;
    case 2:
        SetSwordSwitch(1);
    break;
    case 3:
        SetSwordSwitch(0);
    break;
    case 4:
        if (ingame_wrk.msn_no == 3 && quake.flg == 0)
        {
            ReqQuake(0, 0.015f, 180, 0, 1);
            quake.flg = 1;
        }
    break;
    }

    ani_ctrl->anm.code_now++;
}

u_char motGetNextMotion(ANI_CTRL *ani_ctrl)
{
    ANI_CODE_CTRL *c_ctrl;
    int args[3];
    ANI_CODE code;

    c_ctrl = &ani_ctrl->anm;

    c_ctrl->stat = 0;

    do
    {
        code = *c_ctrl->code_now;

        GetAniCodeArgs(code,args);

        switch (code >> 12)
        {
        case 0:
            motAniCodeReadCTRL(ani_ctrl, args);
        break;
        case 2:
            motAniCodeReadMOT(ani_ctrl, args);
        break;
        default:
            c_ctrl->code_now++;
        break;
        }
    } while (c_ctrl->stat == 0);

    return c_ctrl->stat == 1;
}
