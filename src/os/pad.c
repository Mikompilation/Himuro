#include "common.h"
#include "typedefs.h"

#define INCLUDING_FROM_PAD_C
#include "pad.h"
#undef INCLUDING_FROM_PAD_C

#include "sce/libpad.h"
#include "sce/misc/diei.h"
#include "main/glob.h"
#include "graphics/graph3d/sglib.h"

u_short sce_pad[16] = {
    0x1000,  0x4000,  0x8000,  0x2000,
    0x10,    0x40,    0x80,    0x20,
    0x4,     0x1,     0x8,     0x2,
    0x800,   0x100,   0x200,   0x400,
};

#define abs(x) ((x)<0 ? (-(x)) : (x))

int InitPad()
{
	PAD_STRUCT *psp;
	int i;
	int j;
	int loop;
	int port_slot[2][2] = {
        {0, 0},
        {1, 0},
    };
    
    scePadInit(0);

    for (i = 0, psp = pad; i < 2; i++, psp++)
    {
        psp->port = port_slot[i][0];
        psp->slot = port_slot[i][1];
        psp->now = 0;
        psp->old = 0;
        psp->one = 0;
        psp->rpt = 0;
        psp->rpt_time = 0;
        psp->flags = 2;
        psp->step = 0;
        
        for (loop = 0; loop < 6; loop++)
        {
            psp->pad_direct[loop] = 0;
        }
        
        scePadPortOpen(psp->port, psp->slot, psp->pad_dma_buf);
    }

    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 16; j++)
        {
            pad[i].cnt[j] = 0;
            pad[i].cnt_bak[j] = 0;
        }
        
        for (j = 0; j < 2; j++)
        {
            pad[i].an_cnt_bak[j] = 0;
            pad[i].an_cnt[j] = 0;
            pad[i].an_dir_bak[j] = 0xff;
            pad[i].an_dir[j] = 0xff;
            pad[i].an_rot_bak[j] = 0.0f;
            pad[i].an_rot[j] = 0.0f;
        }
        
        for (j = 0; j < 4; j++)
        {
            pad[i].analog[j] = 0x80;
        }
    }
    
    return 0;
}

int PadSyncCallback()
{
    int id;
    int i;
    int state;
    int wrk;
    int exid;
    int p_id;
    int intr;
    PAD_STRUCT *psp;
    char act_align[6];


    intr = EIntr();
    
    for(psp = pad, p_id = 0; p_id < 2; psp++, p_id++)
    {
        state = scePadGetState(psp->port, psp->slot);
        if (state == scePadStateDiscon)
        {
            psp->step = 0;
            psp->flags &= 0xfe;
            PadClearCount(p_id);
        }
        else
        {
            psp->flags |= 1;
        }
        if (state == scePadStateFindPad)
        {
            psp->step = 0;
        }

        switch (psp->step)
        {
        case 0:
            if (state != scePadStateStable && state != scePadStateFindCTP1)
            {
                break;
            }
            
            id = scePadInfoMode(psp->port, psp->slot, InfoModeCurID, 0);
            if (id == 0)
            {
                break;
            }
            
            exid = scePadInfoMode(psp->port, psp->slot, InfoModeCurExID, 0);
            if (exid > 0)
            {
                id = exid;
            }
            
            psp->id = 0;
            // 4: STANDARD CONTROLLER (Dualshock)
            // 7: ANALOG CONTROLLER (Dualshock 2)
            if (id != 4 && id != 7)
            {
                psp->step = 99;
                break;
            }
            
            if (scePadInfoAct(psp->port, psp->slot, -1, 0) == 0)
            {
                psp->step = 99;
                break;
            }
            
            act_align[0] = 0; // offset 0 => motor0
            act_align[1] = 1; // offset 1 => motor1
            
            for (i = 2; i < 6; i++)
            {
                act_align[i] = -1;
            }
            
            for (i = 0; i < 6; i++)
            {
                psp->pad_direct[i] = 0;
            }

            if (scePadSetActAlign(psp->port, psp->slot, (u_char *)act_align) == 1)
            {
                psp->step = 1;
            }
        break;
        case 1:
            state = scePadGetState(psp->port, psp->slot);
            // scePadStateExecCmd indicates a "verifying" state
            if (state != scePadStateExecCmd)
            {
                // switch to analog mode
                if (scePadSetMainMode(psp->port, psp->slot, 1, 3) != 1)
                {
                    psp->step = 0;
                }
                else
                {
                    psp->step = 2;
                }
            }
        break;
        case 2:
            switch (scePadGetReqState(psp->port, psp->slot))
            {
            case scePadReqStateComplete:
                psp->step = 3;
            break;
            case scePadReqStateFailed:
                psp->step = 0;
            break;
            case scePadReqStateBusy:
                // do nothing
            break;
            }
        break;
        case 3:
            id = scePadInfoMode(psp->port, psp->slot, InfoModeCurID, 0);
            if (id == 0)
            {
                break;
            }
            
            exid = scePadInfoMode(psp->port, psp->slot, InfoModeCurExID, 0);
            if (exid > 0)
            {
                id = exid;
            }

            // ANALOG CONTROLLER (Dualshock 2)
            if (id == 7)
            {
                psp->step = 10;
            }
            else
            {
                psp->step = 99;
            }
        break;
        case 10:
            // check if DualShock 2 supports pressure sensitive mode
            if (scePadInfoPressMode(psp->port, psp->slot) == 1)
            {
                // enable DualShock2 pressure sensitive mode (function is asynchronous)
                if (scePadEnterPressMode(psp->port, psp->slot) == 1)
                {
                    __asm__ volatile (""); // dirty hack
                    psp->step = 11;
                }
            }
            else
            {
                psp->step = 99;
            }
        break;
        case 11:
            // check request status for Dualshock2 pressure sensitive mode
            wrk = scePadGetReqState(psp->port, psp->slot);
            if (wrk == scePadReqStateFailed)
            {
                psp->step = 10;
            }
            if (wrk == scePadReqStateComplete)
            {
                psp->step = 99;
            }
        break;
        case 99:
            if (state == scePadStateStable || state == scePadStateFindCTP1)
            {
                PadReadFunc(psp, p_id);
                if (psp->pad_direct[0] & 0x80)
                {
                    psp->pad_direct[0] &= 1;
                    
                    // returns true or false to indicate the completion of the vibration operation
                    scePadSetActDirect(psp->port, psp->slot, psp->pad_direct);
                    if (
                        psp->pad_direct[0] == 0 &&
                        psp->pad_direct[1] == 0
                    )
                    {
                        psp->pad_direct[0] = 0;
                        psp->pad_direct[1] = 0;
                    }
                    else
                    {
                        VibrateRequest(p_id, 0, 0);
                    }
                }
            }
        break;
        }
    }
    
    if (!intr)
    {
        DIntr();
    }
    
    return 0;
}

int PadReadFunc(PAD_STRUCT *psp, int p_id)
{
	char r_data[32];
	int i;
	int j;
    
    psp->old = psp->now;
    
    if (scePadRead(psp->port,psp->slot, (u_char *)r_data) == 0)
    {
        psp->now = 0;
        return 0;
    }

    // if the 1st byte of r_data is 0, then button info is valid,
    // otherwise info is invalid
    if (r_data[0] != 0)
    {
        psp->now = 0;
        return 0;
    }

    if (psp->id != 0 && psp->id != r_data[1])
    {
        psp->step = 0;
        return 0;
    }
    
    // logical reverse button info => 1: press down, 0: release
    psp->now = 0xffff ^ ((r_data[2] << 8) | (u_char)r_data[3]);    
    psp->one = psp->now & (psp->now ^ psp->old);
    psp->rpt = psp->one;
    
    psp->id = r_data[1];
    
    if (psp->now == psp->old)
    {
        psp->rpt_time++;
        if (psp->rpt_time > 9)
        {
            psp->rpt = psp->now;
            psp->rpt_time = 5;
        }
    }
    else
    {
        psp->rpt_time = 0;
    }
    
    for (i = 0; i < 16; i++)
    {
        pad[p_id].cnt_bak[i] = pad[p_id].cnt[i];
        
        if ((sce_pad[i] & psp->now) != 0)
        {
            pad[p_id].cnt[i]++;
        }
        else
        {
            pad[p_id].cnt[i] = 0;
        }
    }
    
    if ((psp->id & 0xf0) == 0x70)
    {
        for (j = 0; j < 4; j++)
        {
            psp->analog[j] = r_data[j + 4];
        }
    }
    
    if (psp->id == 0x79)
    {
        for (j = 0; j< 12; j++)
        {
            psp->push[j] = r_data[j + 8];
        }
        
        SetAnlgInfo(psp, p_id);
    }
    
    return 0;
}

void SetAnlgInfo(PAD_STRUCT *psp, int p_id)
{
    float rot;
    short int pad_x;
    short int pad_y;
    u_char i;
    u_char dir_old;
        
    for (i = 0; i < 2; i++)
    {
        dir_old = psp->an_dir[i];
        psp->an_cnt_bak[i] = psp->an_cnt[i];
        psp->an_dir_bak[i] = psp->an_dir[i];
        psp->an_rot_bak[i] = psp->an_rot[i];
    
        if (i == 0)
        {
            pad_y = pad[p_id].analog[2] - 128;
            pad_x = 128 - pad[p_id].analog[3];
        }
        else
        {
            pad_y = pad[p_id].analog[0] - 128;
            pad_x = 128 - pad[p_id].analog[1];
        }

        if (abs(pad_y) >= 60 || abs(pad_x) >= 60)
        {
            rot = SgAtan2f(pad_y, pad_x);

            psp->an_dir[i] = (int)((rot + 3.1415927f + 0.39269909f) / 0.78539819f) % 8 + 4;
            
            if (psp->an_dir[i] > 7)
            {
                psp->an_dir[i] -= 8;
            }
            
            psp->an_rot[i] = rot;
            
            if (psp->an_dir[i] == dir_old)
            {
                psp->an_cnt[i]++;
            }
            else
            {
                psp->an_cnt[i] = 0;
            }
        }
        else
        {
            psp->an_dir[i] = 0xff;
            psp->an_cnt[i] = 0;
        }
    }
}

u_short VibrateRequest(u_short p_id, u_short act1, u_short act2)
{
    PAD_STRUCT *psp;

    psp = &pad[p_id];
    
    if (opt_wrk.pad_move == 0)
    {
        psp->pad_direct[0] = act1 | 0x80;
        psp->pad_direct[1] = act2;
    }
    
    return 0;
}

u_short VibrateRequest1(u_short p_id, u_short act_1)
{
    PAD_STRUCT *psp;

    psp = &pad[p_id];

    if (opt_wrk.pad_move == 0)
    {
        psp->pad_direct[0] = act_1 | 0x80;
    }
    
    return 0;
}

u_short VibrateRequest2(u_short p_id, u_short act_2)
{
    PAD_STRUCT *psp;

    psp = &pad[p_id];

    if (opt_wrk.pad_move == 0)
    {
        psp->pad_direct[1] = act_2;
        psp->pad_direct[0] = 0x80;
    }
    
    return 0;
}

void PadClearCount(int p_id)
{
    u_int i;
    
    for (i = 0; i < 16; i++)
    {
        pad[p_id].cnt_bak[i] = 0;
        pad[p_id].cnt[i] = 0;
    }
    
    for (i = 0; i < 2; i++)
    {
        pad[p_id].an_cnt_bak[i] = 0;
        pad[p_id].an_cnt[i] = 0;
        pad[p_id].an_dir_bak[i] = 0xff;
        pad[p_id].an_dir[i] = 0xff;
        pad[p_id].an_rot_bak[i] = 0.0f;
        pad[p_id].an_rot[i] = 0.0f;
    }
    
    for (i = 0; i < 4; i++)
    {
        pad[p_id].analog[i] = 0x80;
    }
}
