#include "iopmain.h"

#include "se/iopse.h"
#include "adpcm/iopadpcm.h"
#include "cdvd/iopcdvd.h"

#include "intrman.h"
#include "introld.h"
#include "libsd.h"
#include "sifrpc.h"
#include "sysmem.h"
#include "thread.h"
#include "timerman.h"

static int iop_loop_thid = 0;
IOP_STAT iop_stat;
IOP_MASTER_VOL iop_mv;
IOP_SYS_CTRL iop_sys_ctrl;

static int IopMainLoop();
static void* IopDrvFunc(unsigned int command, void* data, int size);
static void IopInitDevice();
static int IopInitMain();
static int IopSetTimer();
static int IopMain();
static u_int IopIntrFunc(IOP_SYS_CTRL* iscp);

int start(int argc, char** argv)
{
    struct ThreadParam param;
    int tmp;

    param.attr = 0x2000000;
    param.entry = IopMainLoop;
    param.initPriority = 32;
    param.stackSize = 2048;
    param.option = 0;

    iop_loop_thid = CreateThread(&param);
    if (iop_loop_thid > 0) {
        StartThread(iop_loop_thid, 0);
        tmp = QueryMemSize();
        tmp = QueryTotalFreeMemSize();
        tmp = QueryMaxFreeMemSize();

        return 0;
    } else {
        return 1;
    }
}

static int IopMainLoop()
{
    sceSifQueueData qd;
    sceSifServeData sd;

    CpuEnableIntr();
    EnableIntr(36);
    EnableIntr(40);
    EnableIntr(9);
    sceSifInitRpc(0);
    sceSifSetRpcQueue(&qd, GetThreadId());
    sceSifRegisterRpc(&sd, 0x19740512, IopDrvFunc, &iop_sys_ctrl.get_cmd, 0, 0, &qd);
    sceSifRpcLoop(&qd);

    return 0;
}

static void* IopDrvFunc(unsigned int command, void* data, int size)
{
    IOP_COMMAND* icp;
    int i;

    if (command == 0) {
        iop_mv.vol = 0;
        iop_mv.mono = 0;

        IopInitDevice();

        iop_mv.vol = 0x3FFF;
        sceSdSetParam(SD_P_MVOLL | SD_CORE_0, iop_mv.vol);
        sceSdSetParam(SD_P_MVOLR | SD_CORE_0, iop_mv.vol);
        sceSdSetParam(SD_P_MVOLL | SD_CORE_1, iop_mv.vol);
        sceSdSetParam(SD_P_MVOLR | SD_CORE_1, iop_mv.vol);

        IopInitMain();
    } else if (command == 1) {
        icp = data;

        se_start_flg = 0;
        se_stop_flg = 0;

        for (i = 0; i < (size / sizeof(*icp)); i++) {
            if (icp->cmd_no >= IC_SE_INIT && icp->cmd_no <= IC_SE_QUIT) {
                ISeCmd(icp);
            } else if (icp->cmd_no >= IC_CDVD_INIT && icp->cmd_no <= IC_CDVD_BREAK) {
                ICdvdCmd(icp);
            } else if (icp->cmd_no >= IC_ADPCM_INIT && icp->cmd_no <= IC_ADPCM_QUIT) {
                IAdpcmCmd(icp);
            }

            icp->cmd_no = 0;
            icp++;
        }

        if (se_start_flg)
            sceSdSetSwitch(SD_S_KON | SD_CORE_1, se_start_flg);
        if (se_stop_flg)
            sceSdSetSwitch(SD_S_KOFF | SD_CORE_1, se_stop_flg);
    }

    ICdvdTransSeEnd();
    return &iop_stat;
}

static void IopInitDevice()
{
    sceSdInit(0);
    ICdvdInit(0);
    ISeInit(0);
    IAdpcmInit(0);
}

static int IopInitMain()
{
    struct ThreadParam param;
    int tmp;

    tmp = QueryTotalFreeMemSize();
    tmp = QueryMaxFreeMemSize();

    param.attr = 0x2000000;
    param.entry = IopMain;
    param.initPriority = 32;
    param.stackSize = 2048;
    param.option = 0;

    iop_sys_ctrl.thread_id = CreateThread(&param);
    StartThread(iop_sys_ctrl.thread_id, 0);

    param.entry = IAdpcmMain;
    param.initPriority = 30;
    iop_sys_ctrl.adpcm_thid = CreateThread(&param);
    StartThread(iop_sys_ctrl.adpcm_thid, 0);

    IopSetTimer();
    return 0;
}

static int IopSetTimer()
{
    struct SysClock clock;

    USec2SysClock(0x1047u, &clock);
    iop_sys_ctrl.count = clock.low;
    if ((iop_sys_ctrl.timer_id = AllocHardTimer(1, 32, 1)) <= 0) {
    }

    if (SetTimerHandler(iop_sys_ctrl.timer_id, iop_sys_ctrl.count, IopIntrFunc, &iop_sys_ctrl)) {
    }

    if (SetupHardTimer(iop_sys_ctrl.timer_id, 1, 0, 1)) {
    }

    if (StartHardTimer(iop_sys_ctrl.timer_id)) {
    }

    return 0;
}

static u_int IopIntrFunc(IOP_SYS_CTRL* iscp)
{
    iWakeupThread(iscp->thread_id);
    iWakeupThread(iscp->adpcm_thid);
    return iscp->count;
}

static void IopStereoChange(IOP_COMMAND* icp)
{
    if (icp->data1 == 0) {
        iop_mv.mono = 0;
    } else {
        iop_mv.mono = 1;
    }

    IaSetSteMono();
}

static int IopMain()
{
    while (1) {
        SleepThread();
        ISeMain();
        ICdvdMain();
        IAdpcmMain2();
    }

    return 0;
}

IOP_STAT* GetIopStatP()
{
    return &iop_stat;
}
