#include "common.h"
#include "typedefs.h"
#include "sce/sifrpc.h"
#include "os/eeiop/eeiop.h"
#include "os/eeiop/eese.h"
#include "os/eeiop/cdvd/eecdvd.h"

// int printf ( const char * format, ... );

EI_SYS ei_sys = {0};
IOP_COMMAND send_cmd[32] __attribute__((aligned(64))) = {0};
IOP_STAT rcv_stat __attribute__((aligned(64))) = {0};

static int EiInitRpc();
static int EiInitIopDev();
static void InitIopCmd(IOP_COMMAND *cmdp);
static void SetIopCmd(IOP_COMMAND *cmdp);
static void TidyUpIopCmd(IOP_COMMAND *cmdp);
static int EiSendCmd();

void EiInit()
{
    ei_sys.cmd_num = 0;
    
    EiInitRpc();
    EiInitIopDev();
}

int EiMain()
{
    int i;

    if (EiSendCmd() == 1 && ei_sys.cmd_num != 0)
    {
        for (i = 0; i < 32; i++)
        {
            InitIopCmd(&send_cmd[i]);
        }
        
        ei_sys.cmd_num = 0;
    }
    
    return 1;
}

void SetIopCmdSm(int cmd_no, int data1, int data2, int data3)
{
    IOP_COMMAND cmd;
    
    cmd.cmd_no = cmd_no;
    cmd.data1 = data1;
    cmd.data2 = data2;
    cmd.data3 = data3;
    
    SetIopCmd(&cmd);
}

void SetIopCmdLg(int cmd_no, int data1, int data2, int data3, int data4, int data5, int data6, int data7)
{
    IOP_COMMAND cmd;
    
    cmd.cmd_no = cmd_no;
    cmd.data1 = data1;
    cmd.data2 = data2;
    cmd.data3 = data3;
    cmd.data4 = data4;
    cmd.data5 = data5;
    cmd.data6 = data6;
    cmd.data7 = data7;
    
    SetIopCmd(&cmd);
}

static int EiInitRpc()
{
    int i;
    
    while (1) {
        if (sceSifBindRpc(&ei_sys.gcd, 0x19740512, 0) < 0)
        {
            printf("***** ERROR : sceSifBindRpc *****\n");
            while(1) {};
        }
        i = 10000;
        while(i --) {};
        if (ei_sys.gcd.serve != NULL)
        {
            break;
        }
    }
    
    return 1;
}

static int EiInitIopDev()
{
    if (sceSifCallRpc(&ei_sys.gcd, 0, 0, NULL, 0, NULL, 0, NULL, NULL) < 0)
    {
        return -1;
    }
    
    return 1;
}

static void InitIopCmd(IOP_COMMAND *cmdp)
{
    cmdp->cmd_no = 0;
    cmdp->data1 = 0;
    cmdp->data2 = 0;
    cmdp->data3 = 0;
    cmdp->data4 = 0;
    cmdp->data5 = 0;
    cmdp->data6 = 0;
    cmdp->data7 = 0;
}

static void SetIopCmd(IOP_COMMAND *cmdp)
{
    if (cmdp != NULL && ei_sys.cmd_num < 32)
    {
        TidyUpIopCmd(cmdp);
        send_cmd[ei_sys.cmd_num] = *cmdp;
        ei_sys.cmd_num++;
    }
}

static void TidyUpIopCmd(IOP_COMMAND *cmdp)
{
    return;
}

int checkIOP()
{
    return sceSifCheckStatRpc((sceSifRpcData *)&ei_sys.gcd);
}

static int EiSendCmd()
{
    int ret;

    if (sceSifCheckStatRpc((sceSifRpcData *)&ei_sys.gcd) == 0)
    {
        SeWrkUpdate();
        LoadEndFlgRenew();
        
        if (ei_sys.cmd_num != 0)
        {
            ret = sceSifCallRpc(&ei_sys.gcd, 1, 1, send_cmd, ei_sys.cmd_num * 32, &rcv_stat, 0x180, NULL, NULL);
        }
        else
        {
            ret = sceSifCallRpc(&ei_sys.gcd, 2, 1, NULL, 0, &rcv_stat, 0x180, NULL, NULL);
        }
        
        return (ret < 0) ? -1 : 1;
    }
    
    return 0;
}

IOP_STAT *GetIopStatP()
{
    return &rcv_stat;
}
