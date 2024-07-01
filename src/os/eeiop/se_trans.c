#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "se_trans.h"
#include "sce/sif.h"

#include "main/glob.h"
#include "os/eeiop/eese.h"
#include "os/eeiop/eeiop.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "ingame/entry/entry.h"
#include "ingame/enemy/ene_ctl.h"

#include "data/fgf_tbl.h" // FG_FILE_TBL fgf_tbl[/*4*/] = { ... };
SE_TRANS_CTRL st_ctrl = {0};

static u_char ChkLoadReqQueueFree();
static u_char ChkFGhostAlreadySet();
static u_char ChkNeedsFGhostSe();
static void GetSeTransSize();
static void DmaTransReq();
static u_char ChkDmaTransEndSe();
static void TransReqIopCmd();
static u_char TransWaitIopCmd();
static void UseOkFGhost();

#define MAX_TRANS_SIZE 0x64000

void FGTransInit()
{  
    if (ingame_wrk.msn_no != PMODE_NORMAL && ingame_wrk.msn_no != PMODE_MSG_DISP) 
    {
        if (ChkFGhostAlreadySet()) 
        {
            st_ctrl.mode = STMODE_USEOK;
            ap_wrk.stts = ap_wrk.stts & 0xbf;
            return;
        }
        if (ChkLoadReqQueueFree() && ChkNeedsFGhostSe()) 
        {
            st_ctrl.mode = STMODE_READY;
            return;
        }
    }
    
    st_ctrl.mode = STMODE_USEOK;
    return;
}

u_char IsEndFgTrans()
{
    return st_ctrl.mode == STMODE_USEOK;
}

static u_char ChkLoadReqQueueFree()
{
    return !IsExistLoadReq();
}

static u_char ChkFGhostAlreadySet()
{
    int i;

    st_ctrl.file_no = fene_dat[ingame_wrk.msn_no][ap_wrk.fgst_no].se_no;

    for (i = 0; i < 3; i++)
    {
        if (se_ctrl.ghost_no[i] == st_ctrl.file_no && se_ctrl.ghost_type[i] == 1)
        {
            return 1;
        }
    }

    return 0;
}

static u_char ChkNeedsFGhostSe()
{
    int i;
    u_char ret;
  
    ret = 0;

    for (i = 0; i < 3; i++) 
    {
        if (ap_wrk.fg_se_empty[i] == 0) 
        {
            st_ctrl.trans_pos = i;
            ret = 1;
            break;
        }
    }
    
    return ret;
}

void FloatGhostSENotEmpty()
{
    int i;

    for (i = 0; i < 3; i++)
    {
        if (ap_wrk.fg_se_empty[i] == 0)
        {
            return;
        }
    }
  
    ap_wrk.stts = ap_wrk.stts | 0x40;
}

static void GetSeTransSize()
{
    st_ctrl.size = GetImgArrangementP(st_ctrl.file_no)->size;
    
    if (MAX_TRANS_SIZE < st_ctrl.size) 
    {
        st_ctrl.size = MAX_TRANS_SIZE;
    }
}

static void DmaTransReq()
{
    sceSifDmaData dma;
  
    dma.data = 0x1460000;
    dma.addr = rcv_stat.cdvd.ld_addr;
    dma.size = st_ctrl.size;
    dma.mode = 0;

    sceSifSetDma(&dma, 1);
}

static u_char ChkDmaTransEndSe()
{
    if (sceSifDmaStat(st_ctrl.dma_id) < 0)
    {
        return 1;
    }

    return 0;
}

static void TransReqIopCmd()
{
    SetIopCmdSm(IC_CDVD_SE_TRANS, st_ctrl.size, st_ctrl.trans_pos + 0x10, st_ctrl.file_no);
}

static u_char TransWaitIopCmd()
{
    return rcv_stat.cdvd.se_trans == CDVD_SE_FINISHED;
}

static void UseOkFGhost()
{
    se_ctrl.ghost_no[st_ctrl.trans_pos] = st_ctrl.file_no;
    se_ctrl.ghost_type[st_ctrl.trans_pos] = 1;
    ap_wrk.stts = ap_wrk.stts & 0xbf;
}

int SeFGhostTransCtrl()
{
    int ret = 0;
    sys_wrk.sreset_count = 0;
    
    switch (st_ctrl.mode)
    {
        case STMODE_READY:
            GetSeTransSize();
            DmaTransReq();
            st_ctrl.mode = STMODE_EE2IOP;
        case STMODE_EE2IOP:
            if (ChkDmaTransEndSe()) 
            {
                TransReqIopCmd();
                st_ctrl.mode = STMODE_IOP2SPU;
            }
        break;
        case STMODE_IOP2SPU:
            // Line 386
            if (!TransWaitIopCmd()) 
            {
                break;
            }
                
            SetIopCmdSm(IC_CDVD_SE_TRANS_RESET, 0, 0, 0);
            UseOkFGhost();
            st_ctrl.mode = STMODE_USEOK;
            break;
        case STMODE_USEOK: ret = 1;
    }
    
    return ret;
}
