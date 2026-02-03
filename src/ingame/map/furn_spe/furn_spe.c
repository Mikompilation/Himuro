#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "furn_spe.h"

// #include <cstdlib.h>
// RAND_MAX = (2**31-1)
#define RAND_MAX 2147483647

#include "sce/libvu0.h"

#include "common/ul_math.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_rdr.h"
#include "graphics/graph2d/effect_sub2.h"
#include "graphics/graph2d/takep_pe.h"
#include "graphics/graph3d/load3d.h"
#include "graphics/graph3d/sgdma.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/motion/accessory.h"
#include "graphics/motion/mdlact.h"
#include "graphics/motion/mime.h"
#include "ingame/ig_glob.h"
#include "ingame/map/furn_dat.h"
#include "ingame/map/furn_eff.h"
#include "ingame/map/furn_spe/adj_valg.h"
#include "ingame/map/furn_spe/fsla_main.h"
#include "ingame/map/furn_spe/fspe_acs.h"
#include "ingame/map/furn_spe/furn_spe.h"
#include "ingame/map/item_ctl.h"
#include "ingame/plyr/unit_ctl.h"
#include "main/glob.h"
#include "os/eeiop/adpcm/ea_furn.h"
#include "os/eeiop/adpcm/ea_hiso.h"
#include "os/eeiop/eese.h"
#include "os/pad.h"

sceVu0FVECTOR special_furn_pos[] = {
    { 34885.0f, -4800.0f, 46059.0f, 1.0f },
    { 15101.0f, -1300.0f, 11048.0f, 1.0f },
    { 25235.0f, -1400.0f, 16750.0f, 1.0f },
    { 27870.0f,     0.0f, 11922.0f, 1.0f },
    { 35850.0f,     0.0f, 31232.0f, 1.0f },
    { 22886.0f,  -225.0f, 16200.0f, 1.0f },
    { 27573.0f,     0.0f, 14999.0f, 1.0f },
    { 33347.0f, -1005.0f, 24130.0f, 1.0f },
};
FURN_DAT_SAVE f_dat_save[430] = {0};
FURN_ACT_WRK f_act_wrk[20] = {0};
TREMBLE_H_WRK tremble_h_wrk[5] = {0};
sceVu0FVECTOR thunder_direction = { 0.0f, 0.0f, 0.0f, 0.0f };

#define DEG2RAD(x) ((float)(x)*PI/180.0f)
#define PI 3.1415927f

void NewgameFActInit()
{
    return;
}

void InitFurnDatSave()
{
    int i;

    for (i = 0; i < 430; i++)
    {
        f_dat_save[i].eventflg = 0;

        f_dat_save[i].data = 0xFF;
    }
}

void PreGameInitFActWrk()
{
    memset(f_act_wrk, 0, sizeof(f_act_wrk));
    memset(fspe_light_wrk, 0, sizeof(point_light_wrk)); // wrong size? sizeof(fspe_light_wrk) is 0x1c0, not 0x80
    memset(point_light_wrk, 0, sizeof(point_light_wrk));

    InitAdjValGWrk();
}

void InitFActWrk()
{
    int i;

    for (i = 0; i < 20; i++)
    {
        FreeFActFwrk(&f_act_wrk[i]);
    }
}

void FActWaitExecJob(FURN_ACT_WRK *fawp)
{
    fawp->count++;

    if (fawp->fw_id >= 0)
    {
        sceVu0CopyVector(fawp->pos, furn_wrk[fawp->fw_id].pos);

        TrembleHCtrl();

        AddRotVector(furn_wrk[fawp->fw_id].rotate,fawp->rot_speed);

        fawp->pta = FSpeTexAnmCtrl(fawp->pta, &fawp->ta);
    }
}

void FActWrkMain()
{
    int i;

    temp_miku_act = 0;

    for (i = 0; i < 20; i++)
    {
        if (f_act_wrk[i].state != ST_FACT_VACANT)
        {
            if (f_act_wrk[i].state == ST_FACT_WAITWAIT)
            {
                FActExecOccur(&f_act_wrk[i], f_act_wrk[i].wp);

                f_act_wrk[i].state = ST_FACT_WAIT;
            }

            FActWaitExecJob(&f_act_wrk[i]);

            if (f_act_wrk[i].state == ST_FACT_WAIT)
            {
                FActWait(&f_act_wrk[i]);
            }
            else if (f_act_wrk[i].state == ST_FACT_EXEC)
            {
                FActCheckExec(&f_act_wrk[i]);
            }
        }
    }

    ThunderLight();
    GusObjects();
    FallenObjects();
    CandleAnmCtrl();
    AdjustValueG();
    EyeLightCtrl();
}

int SearchRegisterFW2FAW(u_short fw_id, u_short fact_no)
{
    int vacant_id;
    int temp_id;

    vacant_id = GetOpenedFAWID();

    if (vacant_id < 0)
    {
        return 0;
    }

    if (SearchFActWrkIDFromFWID(fw_id) >= 0)
    {
        return 0;
    }

    temp_id = SearchFActWrkIDFromFurnID(furn_wrk[fw_id].id);

    if (temp_id < 0)
    {
        RegisterFW2FAW(fw_id, &f_act_wrk[vacant_id], fact_no);
    }
    else
    {
        f_act_wrk[temp_id].fw_id = fw_id;
    }

    return 1;
}

void ExchangeFAWFWID(int fw_id1, int fw_id2)
{
    int faw_id1;
    int faw_id2;

    faw_id1 = SearchFActWrkIDFromFWID(fw_id1);
    faw_id2 = SearchFActWrkIDFromFWID(fw_id2);

    if (faw_id1 >= 0)
    {
        f_act_wrk[faw_id1].fw_id = fw_id2;
    }

    if (faw_id2 >= 0)
    {
        f_act_wrk[faw_id2].fw_id = fw_id1;
    }
}

void FSpeFinishFlgOff(u_short furn_id)
{
    int id;

    id = SearchFActWrkIDFromFurnID(furn_id);

    if (id >= 0)
    {
        f_act_wrk[id].flg &= ~0x2;
    }
}

void FurnActOffEve(u_short furn_id)
{
    if (furn_id <= 430)
    {
        f_dat_save[furn_id].eventflg = 0;
    }
}

void FurnActOnEve(u_short furn_id)
{
    if (furn_id <= 430)
    {
        f_dat_save[furn_id].eventflg = 1;
    }
}

int SearchFActWrkIDFromFurnID(u_short furn_id)
{
    int i;

    for (i = 0; i < 20; i++)
    {
        if (f_act_wrk[i].state != ST_FACT_VACANT && f_act_wrk[i].furn_id == furn_id)
        {

            return i;
        }
    }

    return -1;
}

void AddRotVector(sceVu0FVECTOR rot, sceVu0FVECTOR rot1)
{
    sceVu0AddVector(rot, rot, rot1);

    RotLimitChk(&rot[0]);
    RotLimitChk(&rot[1]);
    RotLimitChk(&rot[2]);
}

void FActCheckExec(FURN_ACT_WRK *fawp)
{
    if (FACheckCondition(fawp, &fawp->chk_end) != 0)
    {
        return;
    }

    fawp->count = 0;

    if (fawp->attribute & 0x1)
    {
        fawp->state = ST_FACT_WAIT;

        FActStopAction(fawp, fawp->ap);

        if (
            fawp->fact_no != f_dat_save[fawp->furn_id].data &&
            f_dat_save[fawp->furn_id].data != 0xff &&
            SetFurnActData(fawp, f_dat_save[fawp->furn_id].data) == 0
        )
        {
            fawp->state = ST_FACT_VACANT;
            FreeFActFwrk(fawp);

            return;
        }

        FActExecOccur(fawp,fawp->wp);
    }
    else
    {
        fawp->state = ST_FACT_END;
        FreeFActFwrk(fawp);
    }
}

void FreeFActFwrk(FURN_ACT_WRK *fawp)
{
    if (fawp->state == ST_FACT_END)
    {
        FActStopAction(fawp, fawp->ap);
    }

    if (fawp->state == ST_FACT_EXEC)
    {
        if (fawp->room_no == map_wrk.now_room)
        {
            return;
        }

        FActStopAction(fawp, fawp->ap);
    }

    if (fawp->state == ST_FACT_WAIT)
    {
        FActStopAction(fawp, fawp->wp);
    }

    memset(fawp, 0, sizeof(FURN_ACT_WRK));
}

int SearchFurnWrkIDFromFurnID(u_short furn_id)
{
    int i;

    for (i = 0; i < 60; i++)
    {
        if (furn_wrk[i].id == furn_id)
        {
            return i;
        }
    }

    return -1;
}

void FreeFActFwrkNo(int fw_id)
{
    int id;

    id = SearchFActWrkIDFromFWID(fw_id);

    if (id >= 0)
    {
        FreeFActFwrk(&f_act_wrk[id]);
    }
}

u_char* ForwardActionPointer(u_char *pointer)
{
    while (1)
    {
        switch(*pointer)
        {
        case 1:
            pointer += 4;
        break;
        case 2:
            pointer += 8;
        break;
        case '\x1e':
            pointer += 6;
        break;
        case 3:
            pointer += 2;
        break;
        case 4:
            pointer += 4;
        break;
        case 5:
            pointer += 6;
        break;
        case 16:
            pointer += 8;
        break;
        case 31:
            pointer += 2;
        break;
        case 8:
            pointer += 6;
        break;
        case 7:
            pointer += 4;
        break;
        case 9:
            pointer += 2;
        break;
        case 10:
            pointer += 6;
        break;
        case 11:
            pointer += 4;
        break;
        case 25:
            pointer += 4;
        break;
        case 15:
            pointer += 2;
        break;
        case 13:
            pointer += 2;
        break;
        case 14:
            pointer += 6;
        break;
        case 32:
            pointer += 2;
        break;
        case 12:
            pointer += 2;
        break;
        case 17:
            pointer += 2;
        break;
        case 19:
            pointer += 2;
        break;
        case 18:
            pointer += 8;
        break;
        case 20:
            pointer += 8;
        break;
        case 21:
            pointer += 2;
        break;
        case 22:
            pointer += 4;
        break;
        case 33:
            pointer += 2;
        break;
        case 23:
            pointer += 6;
        break;
        case 6:
            pointer += 4;
        break;
        case 24:
            pointer += 6;
        break;
        case 26:
            pointer += 2;
        break;
        case 27:
            pointer += 2;
        break;
        case 28:
            pointer += 2;
        break;
        case 29:
            pointer += 2;
        break;
        case 0:
            return pointer + 2;
        break;
        }
    }
}

u_char* ForwardConditionPointer(u_char *cp, u_char count)
{
    while(--count != 0xff)
    {
        switch(*cp)
        {
        case 3:
        case 4:
        case 5:
        case 7:
        case 9:
        case 12:
        case 13:
            cp += 6;
        break;
        case 14:
        case 15:
            cp += 4;
        break;
        case 17:
            cp += 12;
        break;
        case 16:
        case 22:
        case 23:
            cp += 16;
        break;
        case 6:
        case 8:
        case 10:
        case 11:
        case 18:
        case 19:
        case 20:
        case 21:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
            cp += 2;
        break;
        }
    }

    return cp;
}

int SetFurnActData(FURN_ACT_WRK *fawp, u_char fact_no)
{
    int i;
    u_short header;
    u_short temp;
    u_char *pointer;

    pointer = FSpeGetTopAddr(fact_no);

    fawp->attribute = *pointer;

    if (*pointer & 0x4)
    {
        return 0;
    }

    pointer += 2;

    header = *(u_short *)(pointer);

    temp = header & 0xf;

    pointer += 2;

    header >>= 4;

    switch (temp)
    {
    case 0:
        for (i = 0; i < 4; i++)
        {
            fawp->chk_occ.chknum[i] = header & 7;

            header >>= 3;
        }

        fawp->chk_occ.p = pointer;

        for (i = 0; i < 4; i++)
        {
            pointer = ForwardConditionPointer((u_char *)pointer, fawp->chk_occ.chknum[i]);
        }
    break;
    case 1:
        // do nothing ...
    break;
    case 2:
        fawp->chk_occ.nochkflg = 1;
    break;
    case 3:
        fawp->chk_occ.nochkflg = -1;
    break;
    }

    header = *(u_short *)pointer;

    temp = header & 0xf;

    pointer += 2;

    header = header >> 4;

    switch (temp)
    {
    case 0:
        fawp->chk_end.p = pointer;

        for (i = 0; i < 4; i++)
        {
            fawp->chk_end.chknum[i] = header & 7;

            header >>= 3;
        }

        for (i = 0; i < 4; i++)
        {
            pointer = ForwardConditionPointer((u_char *)pointer, fawp->chk_end.chknum[i]);
        }
    break;
    case 1:
        fawp->chk_end = fawp->chk_occ;
    break;
    case 2:
        fawp->chk_end.nochkflg = 1;
    break;
    case 3:
        fawp->chk_end.nochkflg = -1;
    break;
    }

    fawp->ap = pointer;

    fawp->wp = ForwardActionPointer((u_char *)pointer);

    fawp->fact_no = fact_no;

    return 1;
}

void RegisterFW2FAW(u_short fw_id, FURN_ACT_WRK *fawp, u_short fact_no)
{
    fawp->count = 0;
    fawp->fw_id = fw_id;
    fawp->furn_id = furn_wrk[fw_id].id;

    if (room_wrk.disp_no[1] == 0xff)
    {
        fawp->room_no = room_wrk.disp_no[0];
    }
    else
    {
        fawp->room_no = room_wrk.disp_no[1];
    }

    if (f_dat_save[fawp->furn_id].data != 0xff)
    {
        fact_no = f_dat_save[fawp->furn_id].data;
    }

    if (SetFurnActData(fawp,fact_no) != 0)
    {
        fawp->state = ST_FACT_WAITWAIT;
    }
    else
    {
        fawp->state = ST_FACT_VACANT;
    }
}

int GetOpenedFAWID()
{
    int i;

    for (i = 0; i < 20; i++)
    {
        if (f_act_wrk[i].state == ST_FACT_VACANT)
        {
            return i;
        }
    }

    return -1;
}

int SearchFActWrkIDFromFWID(u_short fw_id)
{
    int i;

    for (i = 0; i < 20; i++)
    {
        if (f_act_wrk[i].state != ST_FACT_VACANT && f_act_wrk[i].fw_id == fw_id)
        {
            return i;
        }
    }

    return -1;
}

void FActStopAction(FURN_ACT_WRK *fawp, void *point)
{
    u_short *sp;
    u_char *pointer;
    u_char *printer;
    register u_short *a2 asm("a2"); // HACK: fixes regswap

    printer = (u_char *)point;

    fawp->flg &= ~0x2;

    pointer = (u_char *)point;

    while (*printer)
    {
        a2 = (u_short *)&printer[2]; // HACK: should be sp = &printer[2]; fixes regswap

        switch (*printer)
        {
        case 2:
        case 16:
        case 18:
        case 20:
            printer += 8;
        break;
        case 5:
        case 8:
        case 14:
        case 23:
        case 24:
        case 30:
            printer += 6;
        break;
        case 3:
        case 9:
        case 12:
        case 13:
        case 19:
        case 21:
        case 26:
        case 27:
        case 28:
        case 29:
        case 31:
        case 32:
        case 33:
            printer = (u_char *)a2; // HACK: should be: printer += 2; fixes regswap
        break;
        case 1:
        case 4:
        case 6:
        case 7:
        case 11:
        case 22:
        case 25:
            printer += 4;
        break;
        }
    }

    while (1)
    {
        sp = (u_short *)&pointer[2];

        switch(*pointer)
        {
        case 3:
            pointer += 2;
        break;
        case 4:
            acsRopeMoveRequest(fawp->furn_id, 0, 0.0f);

            pointer += 4;
        break;
        case 29:
            acsChodoMoveStop(fawp->furn_id);
            pointer += 2;
        break;
        case 24:
            pointer += 6;
        break;
        case 5:
            fawp->pta = NULL;

            pointer += 6;
        break;
        case 16:
            if (fawp->lw_id >= 0)
            {
                CandleAnmStop(fawp->lw_id);
            }

            if (fawp->furn_id == 334 || fawp->furn_id == 335 || fawp->furn_id == 336 || fawp->furn_id == 337)
            {
                ResetRDPFire(fawp->furn_id);
            }
            else
            {
                if (fawp->buff != NULL)
                {
                    ResetEffects(fawp->buff);
                }
            }

            pointer += 8;
        break;
        case 31:
            if (fawp->furn_id == 334 || fawp->furn_id == 335 || fawp->furn_id == 336 || fawp->furn_id == 337)
            {
                ResetRDPFire(fawp->furn_id);
            }
            else
            {
                if (fawp->buff != NULL)
                {
                    ResetEffects(fawp->buff);
                }
            }

            pointer += 2;
        break;
        case 8:
            ReleaseAdjValGWrkVec(fawp->rot_speed);

            pointer += 6;
        break;
        case 7:
            fawp->rot_speed[0] = 0.0f;
            fawp->rot_speed[1] = 0.0f;
            fawp->rot_speed[2] = 0.0f;
            fawp->rot_speed[3] = 0.0f;

            pointer += 4;
        break;
        case 9:
            StopTrembleH(fawp, fawp->trembleflg);

            pointer += 2;
        break;
        case 10:
            pointer += 6;
        break;
        case 11:
            motPlyrActStop();

            pointer += 4;
        break;
        case 25:
            switch (fawp->furn_id)
            {
            case 334:
            case 335:
            case 336:
            case 337:
                ResetRDPFire(fawp->furn_id);
            break;
            default:
                ResetRDLongFire(fawp->furn_id);
            break;
            }

            pointer += 4;
        break;
        case 13:
            VibrateRequest1(0, 1);

            pointer += 2;
        break;
        case 14:
            StopGusEffect();

            pointer += 6;
        break;
        case 32:
            StopGusEffect();

            pointer += 2;
        break;
        case 12:
            f_dat_save[fawp->furn_id].data = pointer[1];

            pointer += 2;
        break;
        case 19:
            ResetRDPazzEne();

            pointer += 2;
        break;
        case 20:
            StopLampTremble(&pointer[1]);

            pointer += 8;
        break;
        case 21:
            StopEyeLight();

            pointer += 2;
        break;
        case 22:
            AdpcmStopFurn(20);

            pointer += 4;
        break;
        case 33:
            AdpcmStopHiso(0);

            pointer += 2;
        break;
        case 23:
            StopFallenEffect();

            pointer += 6;
        break;
        case 6:
            FSpeDelPointLight(fawp);

            pointer += 4;
        break;
        case 26: {
            int _12 = 12; // HACK: fixes regswap

            if (fawp->room_no == _12)
            {
                ResetRDSunshine(1);
            }
            else if (fawp->room_no == 11)
            {
                ResetRDSunshine(0);
            }
            else if (fawp->room_no == 23)
            {
                ResetRDSunshine(2); ResetRDSunshine(3);
            }
            else if (fawp->room_no == 8)
            {
                ResetRDSunshine(4); ResetRDSunshine(5);
            }
            else if (fawp->room_no == 17)
            {
                ResetRDSunshine(6);
            }

            pointer += 2;
        }
        break;
        case 27:
            ResetRDBloodDrop(fawp->furn_id);

            pointer += 2;
        break;
        case 28: {
            int i;
            u_short furn_id[4] = { 330, 331, 332, 333 };

            for (i = 0; i < 4; i++)
            {
                ResetRDLongFire(furn_id[i]);
            }

            pointer += 2;
        }
        break;
        case 1:
            if (sp[0] < 1000)
            {
                SeStop(fawp->se_no);
            }

            pointer += 4;
        break;
        case 2:
            if (sp[0] < 1000 || sp[0] == 0xffff)
            {
                SeFadeFlame(fawp->se_no, sp[2], 0);
            }
        // fall-through
        case 18:
            pointer += 8;
        break;
        case 30:
            SeFadeFlame(fawp->se_no, sp[1], 0);

            pointer += 6;
        break;
        case 0:
        case 15:
        case 17:
        default:
            return;
        break;
        }
    }
}

void FActExecOccur(FURN_ACT_WRK *fawp, void *point)
{
    u_short *sp;
    u_char *pointer;
    u_char *printer;
    int i;
    register u_short *a2 asm("a2"); // HACK: fixes regswap

    printer = (u_char *)point;

    fawp->flg |= 2;

    pointer = (u_char *)point;

    while(*printer)
    {
        a2 = (u_short *)&printer[2]; // HACK: should be sp = &printer[2]; fixes regswap

        switch(*printer)
        {
        case 2:
        case 16:
        case 18:
        case 20:
            printer += 8;
        break;
        case 5:
        case 8:
        case 14:
        case 23:
        case 24:
        case 30:
            printer += 6;
        break;
        case 1:
        case 4:
        case 6:
        case 7:
        case 11:
        case 22:
        case 25:
            printer += 4;
        break;
        case 3:
        case 9:
        case 12:
        case 13:
        case 19:
        case 21:
        case 26:
        case 27:
        case 28:
        case 29:
        case 31:
        case 32:
        case 33:
            printer = (u_char *)a2; // HACK: should be: printer += 2; fixes regswap
        break;
        }
    }

    while (1)
    {
        sp = (u_short *)&pointer[2];

        switch(*pointer)
        {
        case 28: {
            u_short furn_id[4] = { 330, 331, 332, 333 };
            int wrkid;

            for (i = 0; i < 4; i++)
            {
                wrkid = SearchFurnWrkIDFromFurnID(furn_id[i]);

                if (wrkid >= 0)
                {
                    SetRDLongFire2(furn_wrk[wrkid].pos, 1, 1.3f, 1.3f, 0.6f, 0.2f, 100.0f, 80.0f, 40.0f, fawp->room_no, furn_id[i]);
                }
            }

            pointer += 2;
        }
        break;
        case 25:
            switch(fawp->furn_id)
            {
            case 330:
            case 331:
            case 332:
            case 333:
                SetRDLongFire2(fawp->pos, 1, 1.3f, 1.3f, 0.6f, 0.2f, 100.0f, 80.0f, 40.0f, fawp->room_no, fawp->furn_id);
            break;
            case 338:
            case 339:
                SetRDLongFire2(fawp->pos, 1, 0.3f, 0.6f, 0.1f, 0.2f, 100.0f, 80.0f, 40.0f, fawp->room_no, fawp->furn_id);
            break;
            case 334:
            case 335:
            case 336:
            case 337:
                SetRDPFire2(fawp->pos, fawp->furn_id);
            break;
            default:
                SetRDLongFire(fawp->pos, pointer[1] / 100.0f, pointer[2] / 100.0f, pointer[3] / 100.0f, fawp->room_no, fawp->furn_id);
            break;
            }

            pointer += 4;
        break;
        case 19:
            SetRDPazzEne(fawp->pos);

            pointer += 2;
        break;
        case 26:
            if (fawp->room_no == 12)
            {
                SetRDSunshine(1);
            }
            else if (fawp->room_no == 11)
            {
                SetRDSunshine(0);
            }
            else if (fawp->room_no == 23)
            {
                SetRDSunshine(2);
                SetRDSunshine(3);
            }
            else if (fawp->room_no == 8)
            {
                SetRDSunshine(4);
                SetRDSunshine(5);
            }
            else if (fawp->room_no == 17)
            {
                SetRDSunshine(6);
            }

            pointer += 2;
        break;
        case 27:
            SetRDBloodDrop((float *)((int)map_item_dat + fawp->furn_id * 16 + 0x4d0), pointer[1], fawp->furn_id); // ???

            pointer += 2;
        break;
        case 11:
            motPlyrActReq(pointer[1], &fawp->pos, *(u_short *)(pointer + 2));

            pointer += 4;
        break;
        case 4:
            acsRopeMoveRequest(fawp->furn_id, pointer[1], sp[0] / 1000.0f);

            pointer += 4;
        break;
        case 3:
            if (mimChodoChkExec(fawp->furn_id) == 0)
            {
                mimChodoRequest(fawp, 0, 0);
            }

            pointer += 2;
        break;
        case 24:
            ReqQuake(fawp->furn_id, *((u_short *)&pointer[2]) / 1000.0f, *((u_short *)&pointer[4]), pointer[1], 0);

            pointer += 6;
        break;
        case 29:
            acsChodoMoveRequest(fawp->furn_id);

            pointer += 2;
        break;
        case 23:
            if (pointer[1] != 0)
            {
                CallFallenEffect(plyr_wrk.move_box.pos, sp[0], pointer[4], pointer[5]);
            }
            else
            {
                CallFallenEffect(fawp->pos, sp[0], pointer[4], pointer[5]);
            }

            pointer += 6;
        break;
        case 14:
            CallGusEffect(fawp->pos, (float)sp[0], (float)sp[1]);

            pointer += 6;
        break;
        case 32:
            switch (fawp->furn_id)
            {
            case 23:
                CallGusEffect(special_furn_pos[3], 800, 2000);
            break;
            case 101:
                CallGusEffect(special_furn_pos[4], 800, 2000);
            break;
            case 123:
                CallGusEffect(special_furn_pos[5], 800, 2000);
            break;
            }

            pointer += 2;
        break;
        case 18:
            CallThunderLight((char)sp[0], (char)sp[1], (char)sp[2], pointer[1], &furn_wrk[fawp->fw_id].pos, fawp->room_no);

            pointer += 8;
        break;
        case 20:
            CallLampTremble(&pointer[1]);

            pointer += 8;
        break;
        case 21:
            fawp->pos[3] = 1.0f;

            CallEyeLight(&fawp->pos, &fawp->pos);

            pointer += 2;
        break;
        case 22:
            if (fawp->furn_id == 39)
            {
                AdpcmPlayFurn(pointer[1], special_furn_pos[2], 0x3fff, 40);
            }
            else if (fawp->furn_id == 323)
            {
                AdpcmPlayFurn(pointer[1], special_furn_pos[7], 0x3fff, 40);
            }
            else
            {
                AdpcmPlayFurn(pointer[1], fawp->pos, 0x3fff, 40);
            }

            pointer += 4;
        break;
        case 33:
            AdpcmPlayHiso(pointer[1] + 8, 0);

            pointer += 2;
        break;
        case 1:
            if (sp[0] == 0xffff)
            {
                fawp->se_no = SeStartPosSrundFlame(fawp->room_no, fawp->pos, 1, 0x1000, 0x1000);
            }
            else if (sp[0] < 1000)
            {
                fawp->se_no = SeStartPos(sp[0], fawp->pos, 0, 0x1000, 0x1000, 0xff);
            }

            pointer += 4;
        break;
        case 2:
            if (sp[0] == 0xffff)
            {
                if (fawp->furn_id == 314)
                {
                    fawp->se_no = SeStartPosSrundFlame(12, special_furn_pos[6], 0, 0x1000, 0x1000);
                }
                else
                {
                    fawp->se_no = SeStartPosSrundFlame(fawp->room_no, fawp->pos, sp[1], 0x1000, 0x1000);
                }
            }
            else if (sp[0] < 1000)
            {
                fawp->se_no = SeStartPosFlame(sp[0], fawp->pos, sp[0], 0x1000, 0x1000, fawp->room_no);
            }

            pointer += 8;
        break;
        case 30:
            fawp->se_no = SeStartPosEventFlame(fawp->room_no, pointer[1], fawp->pos, sp[0], 0x1000, 0x1000);

            pointer += 6;
        break;
        case 5:
            FSpeTexAnmExe(pointer[1], fawp, pointer[2], pointer[3], pointer[4]);

            pointer += 6;
        break;
        case 16: {
            u_char no = furn_dat[fawp->furn_id].ef_no;

            fawp->lw_id = -1;

            if (fawp->furn_id == 334 || fawp->furn_id == 335 || fawp->furn_id == 336 || fawp->furn_id == 337)
            {
                SetRDPFire(fawp->pos, fawp->furn_id);
            }
            else
            {
                fawp->buff = SetEffects(23, 2, 3, fawp->pos,
                    fefct_fire[no].fr, fefct_fire[no].fg, fefct_fire[no].fb, fefct_fire[no].fscl,
                    fefct_fire[no].kr, fefct_fire[no].kg, fefct_fire[no].kb, fefct_fire[no].kscl
                );

                if (fawp->buff == NULL)
                {
                    pointer += 8;

                    break;
                }
            }

            fawp->lw_id = CandleAnmOcc(&fawp->pos, pointer[5] / 100.0f, pointer[6] / 100.0f, pointer[7] / 100.0f, pointer[2], pointer[3], pointer[4], fawp->room_no);

            pointer += 8;
        }
        break;
        case 31: {
            u_char no = furn_dat[fawp->furn_id].ef_no;

            fawp->lw_id = -1;

            if (fawp->furn_id == 334 || fawp->furn_id == 335 || fawp->furn_id == 336 || fawp->furn_id == 337)
            {
                SetRDPFire(fawp->pos, fawp->furn_id);
            }

            else if (!(fawp->furn_id == 330 || fawp->furn_id == 331 || fawp->furn_id == 332 || fawp->furn_id == 333))
            {
                fawp->buff = SetEffects(23, 2, 3, fawp->pos,
                    fefct_fire[no].fr, fefct_fire[no].fg, fefct_fire[no].fb, fefct_fire[no].fscl,
                    fefct_fire[no].kr, fefct_fire[no].kg, fefct_fire[no].kb, fefct_fire[no].kscl
                );
            }
        }
        // fall-through
        case 12:
        case 13:
            pointer += 2;
        break;
        case 8: {
            float f1 = ((sp[0] * 3.1415927f) / 180.0f) / 60.0f;
            sceVu0FVECTOR temp = { f1, 0.0f, 0.0f, 0.0f };

            RegisterAdjValG(fawp->rot_speed, temp, sp[1]);

            pointer += 6;
        }
        break;
        case 7: {
            float f1 = ((sp[0] * 3.1415927f) / 180.0f) / 60.0f;

            fawp->rot_speed[0] = f1;
            fawp->rot_speed[1] = 0.0f;
            fawp->rot_speed[2] = 0.0f;
            fawp->rot_speed[3] = 0.0f;

            pointer += 4;
        }
        break;
        case 9:
            fawp->trembleflg = CallTrembleH(fawp);

            pointer += 2;
        break;
        case 6:
            FSpeSetPointLight(fawp, pointer[1] / 100.0f, pointer[2] / 100.0f, pointer[3] / 100.0f);

            pointer += 4;
        break;
        case 0:
        case 10:
        case 15:
        case 17:
        default:
            return;
        break;
        }
    }
}

void FActWait(FURN_ACT_WRK *fawp)
{
    if (FACheckCondition(fawp, &fawp->chk_occ) != 0)
    {
        FActStopAction(fawp,fawp->wp);
        FActExecOccur(fawp,fawp->ap);

        fawp->count = 0;
        fawp->state = ST_FACT_EXEC;
    }
}

int CompareFloatVal(float a, float b)
{
    if (a > b)
    {
        return 1;
    }
    else if (a != b && a < b)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int ChkANDCondition(FURN_ACT_WRK *fawp, u_char **cpp, u_char count)
{
    int flg;
    u_short *sp;
    int i;
    int j;

    flg = 1;

    while (--count != 0xff)
    {
        sp = (u_short *)(&(*cpp)[2]);

        switch(**cpp)
        {
        case 3:
            if (flg != 0)
            {
                if (FACheckDistNear(sp[1], sp[0], fawp->pos) == 0)
                {
                    flg = 0;
                }
            }

            *cpp += 6;
        break;
        case 4:
            if (flg != 0)
            {
                if (FACheckDistFar(sp[1], sp[0], fawp->pos) == 0)
                {
                    flg = 0;
                }
            }

            *cpp += 6;
        break;
        case 5:
            if (flg != 0)
            {
                if (FACheckStatus(sp[0], sp[1]) == 0)
                {
                    flg = 0;
                }
            }

            *cpp += 6;
        break;
        case 6:
            if (f_dat_save[fawp->furn_id].eventflg == 0)
            {
                flg = 0;
            }

            *cpp += 2;
        break;
        case 11:
            if (f_dat_save[fawp->furn_id].eventflg != 0)
            {
                flg = 0;
            }

            *cpp += 2;
        break;
        case 7:
            if (flg != 0)
            {
                if (FACheckDirect((float)sp[0], sp[1], fawp->pos) == 0)
                {
                    flg = 0;
                }
            }

            *cpp += 6;
        break;
        case 9:
            if (flg != 0)
            {
                if (FACheckDirect((float)sp[0], sp[1], fawp->pos) != 0)
                {
                    flg = 0;
                }
            }

            *cpp += 6;
        break;
        case 8:
            if (plyr_wrk.mode != PMODE_FINDER)
            {
                flg = 0;
            }

            *cpp += 2;
        break;
        case 10:
            if (plyr_wrk.mode == PMODE_FINDER)
            {
                flg = 0;
            }
            *cpp += 2;
        break;
        case 12:
            if (flg != 0)
            {
                if (FACheckSpeedLess(sp[0], sp[1]) == 0)
                {
                    flg = 0;
                }
            }
            *cpp += 6;
        break;
        case 13:
            if (flg != 0)
            {
                if (FACheckSpeedLess(sp[0], sp[1]) != 0)
                {
                    flg = 0;
                }
            }

            *cpp += 6;
        break;
        case 14:
            if (sp[0] < fawp->count)
            {
                flg = 0;
            }

            *cpp += 4;
        break;
        case 15:
            if (fawp->count <= sp[0])
            {
                flg = 0;
            }

            *cpp += 4;
        break;
        case 16:
            if (flg != 0)
            {
                if (FACheckSquareArea(sp[0], sp[1], sp[2], sp[3], sp[4], sp[5], sp[6]) == 0)
                {
                    flg = 0;
                }
            }

            *cpp += 16;
        break;
        case 17:
            if (flg != 0)
            {
                if (FACheckCircleArea(sp[0], sp[1], sp[2], sp[3], sp[4]) == 0)
                {
                    flg = 0;
                }
            }

            *cpp += 12;
        break;
        case 20:
            if (room_wrk.disp_no[0] != fawp->room_no)
            {
                flg = 0;
            }

            *cpp += 2;
        break;
        case 21:
            if (FSpeCheckOutOfRoom(fawp->room_no) != 0)
            {
                flg = 0;
            }

            *cpp += 2;
        break;
        case 18:
            j = fawp->prejudge;

            if (j == 0)
            {
                i = (rand() / (float)RAND_MAX) * 100.0f;

                if ((*cpp)[1] < i)
                {
                    fawp->prejudge = 2;

                    j = 2;
                }
                else
                {
                    fawp->prejudge = 1;

                    j = 1;
                }
            }

            if (j == 2)
            {
                flg = 0;
            }

            *cpp += 2;
        break;
        case 19:
            if ((fawp->flg & 0x2) == 0)
            {
                flg = 0;
            }

            *cpp += 2;
        break;
        case 22:
            if (flg != 0)
            {
                if ( FACheckSquareArea(sp[0], sp[1], sp[2], sp[3], sp[4], sp[5], sp[6]) != 0)
                {
                    flg = 0;
                }
            }

            *cpp += 16;
        break;
        case 23:
            if (flg != 0)
            {
                if (FACheckCircleArea(sp[0], sp[1], sp[2], sp[3], sp[4]) != 0)
                {
                    flg = 0;
                }
            }

            *cpp += 16;
        break;
        case 24:
            if ((plyr_wrk.sta & 0x8) == 0)
            {
                flg = 0;
            }

            *cpp += 2;
        break;
        case 25:
            if (plyr_wrk.sta & 0x8)
            {
                flg = 0;
            }

            *cpp += 2;
        break;
        case 27:
            if (plyr_wrk.sta & 0x1)
            {
                flg = 0;
            }

            *cpp += 2;
        break;
        case 26:
            if ((plyr_wrk.sta & 0x1) == 0)
            {
                flg = 0;
            }

            *cpp += 2;
        break;
        case 28:
            if (map_wrk.floor != (*cpp)[1])
            {
                flg = 0;
            }

            *cpp += 2;
        break;
        case 29:
            if (map_wrk.floor == (*cpp)[1])
            {
                flg = 0;
            }

            *cpp += 2;
        break;
        case 30:
            if (ingame_wrk.clear_count < (*cpp)[1])
            {
                flg = 0;
            }

            *cpp += 2;
        break;
        }
    }

    return flg;
}

int FSpeCheckOutOfRoom(u_char room_no)
{
    if (plyr_wrk.pr_info.room_no != room_no && plyr_wrk.pr_info.room_old == room_no)
    {
        return 1;
    }

    return 0;
}

int FACheckCondition(FURN_ACT_WRK *fawp, SPE_CHK_COND *p_con)
{
    u_char *cp;
    u_char **cpp;
    int i;

    cp = (u_char *)p_con->p;
    cpp = &cp;

    if (p_con->nochkflg >= 1)
    {
        return 1;
    }

    if (p_con->nochkflg < 0)
    {
        return 0;
    }

    for (i = 0; i < 4; i++)
    {
        if (p_con->chknum[i] != 0)
        {
            if (ChkANDCondition(fawp, cpp, p_con->chknum[i]) != 0)
            {
                return 1;
            }
        }
    }

    return 0;
}

int FACheckDirect(u_short range, u_short obj, float *fpos)
{
    int i;

    if (obj & 0x8000)
    {
        if (RotCheck(fpos, &plyr_wrk.move_box, range) != 0)
        {
            return 1;
        }
    }

    for (i = 0; i < 4; i++)
    {
        if (ene_wrk[i].sta & 0x1)
        {
            if (
                (obj & 0x4000 && ene_wrk[i].type == 0) ||
                (obj & 0x2000 && ene_wrk[i].type == 1) ||
                (obj & 0x1000 && ene_wrk[i].type == 2)
            )
            {
                if (RotCheck(fpos, &ene_wrk[i].move_box, range) != 0)
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}

int RotCheck(float *fpos, MOVE_BOX *mbp, u_short range)
{
    sceVu0FVECTOR o2f;
    float rot1;
    float rot2;

    sceVu0SubVector(o2f, fpos, mbp->pos);

    rot1 = SgAtan2f(o2f[0], o2f[2]);

    rot2 = mbp->rot[1];

    RotLimitChk(&rot1);
    RotLimitChk(&rot2);

    rot1 -= rot2;

    RotLimitChk(&rot1);

    rot1 = __builtin_fabsf(rot1);

    return rot1 <= (range * PI) / 180.0;
}

int FACheckDistNear(u_short obj, float dist, float *fpos)
{
    int i;
    float dist2;

    if (obj & 0x8000)
    {
        dist2 = GetDistV(fpos, plyr_wrk.move_box.pos);

        if (CompareFloatVal(dist2, dist) < 0)
        {
            return 1;
        }
    }

    for (i = 0; i < 4; i++)
    {
        if (ene_wrk[i].sta & 0x1)
        {
            if (
                (obj & 0x4000 && ene_wrk[i].type == 0) ||
                (obj & 0x2000 && ene_wrk[i].type == 1) ||
                (obj & 0x1000 && ene_wrk[i].type == 2)
            )
            {
                dist2 = GetDistV(fpos, ene_wrk[i].move_box.pos);

                if (CompareFloatVal(dist2, dist) < 0)
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}

int CheckSquareAreaOne(sceVu0FVECTOR pos, sceVu0FVECTOR square_center, sceVu0FMATRIX mat, float tate, float yoko)
{
    sceVu0FVECTOR obj_pos;

    sceVu0CopyVector(obj_pos,pos);
    sceVu0SubVector(obj_pos, obj_pos, square_center);
    sceVu0ApplyMatrix(obj_pos, mat, obj_pos);

    if (-yoko < obj_pos[0] && obj_pos[0] < yoko && -tate < obj_pos[2] && obj_pos[2] < tate)
    {
        return 1;
    }

    return 0;
}

int FACheckCircleArea(float x, float y, float z, float dist, u_short obj)
{
    sceVu0FVECTOR obj_pos;
    sceVu0FVECTOR temp;
    sceVu0FMATRIX mat;
    int i;

    temp[0] = x;
    temp[1] = y;
    temp[2] = z;
    temp[3] = 1.0f;

    if (obj & 0x8000)
    {
        if(GetDistV(plyr_wrk.move_box.pos,temp) < dist)
        {
            return 1;
        }
    }

    for (i = 0; i < 4; i++)
    {
        if (ene_wrk[i].sta & 0x1)
        {
            if (
                (obj & 0x4000 && ene_wrk[i].type == 0) ||
                (obj & 0x2000 && ene_wrk[i].type == 1) ||
                (obj & 0x1000 && ene_wrk[i].type == 2)
            )
            {
                if (GetDistV(ene_wrk[i].move_box.pos, temp) < dist)
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}

int FACheckSquareArea(float x, float y, float z, float tate, float yoko, float y_rot, u_short obj)
{
    sceVu0FVECTOR obj_pos;
    sceVu0FVECTOR temp;
    sceVu0FMATRIX mat;
    int i;

    yoko *= 0.5f;
    tate *= 0.5f;

    temp[0] = 0.0f;
    temp[1] = -DEG2RAD(y_rot);
    temp[2] = 0.0f;
    temp[3] = 1.0f;

    sceVu0UnitMatrix(mat);
    sceVu0RotMatrix(mat, mat, temp);

    temp[0] = x;
    temp[1] = y;
    temp[2] = z;
    temp[3] = 1.0f;

    if (obj & 0x8000)
    {
       if (CheckSquareAreaOne(plyr_wrk.move_box.pos, temp, mat, tate, yoko) != 0)
       {
           return 1;
       }
    }

    for (i = 0; i < 4; i++)
    {
        if (ene_wrk[i].sta & 0x1)
        {
            if (
                (obj & 0x4000 && ene_wrk[i].type == 0) ||
                (obj & 0x2000 && ene_wrk[i].type == 1) ||
                (obj & 0x1000 && ene_wrk[i].type == 2)
            )
            {
                if (CheckSquareAreaOne(ene_wrk[i].move_box.pos, temp, mat, tate, yoko) != 0)
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}

int FACheckDistFar(u_short obj, float dist, float *fpos)
{
    int i;
    float dist2;

    if (obj & 0x8000)
    {
        dist2 = GetDistV(fpos, plyr_wrk.move_box.pos);

        if (CompareFloatVal(dist2, dist) >= 0)
        {
            return 1;
        }
    }

    for (i = 0; i < 4; i++)
    {
        if (ene_wrk[i].sta & 0x1)
        {
            if (
                (obj & 0x4000 && ene_wrk[i].type == 0) ||
                (obj & 0x2000 && ene_wrk[i].type == 1) ||
                (obj & 0x1000 && ene_wrk[i].type == 2)
            )
            {
                dist2 = GetDistV(fpos, ene_wrk[i].move_box.pos);

                if (CompareFloatVal(dist2, dist) >= 0)
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}

int FACheckStatus(u_short a, u_short b)
{
    return 1;
}

int SpeedLessOrVecF(float s_speed, sceVu0FVECTOR vec)
{
    float inner;

    inner = sceVu0InnerProduct(vec, vec);

    s_speed = s_speed * s_speed;

    if (inner < s_speed)
    {
        return 1;
    }

    return 0;
}

int SpeedLessOrVecVec(sceVu0FVECTOR vec1, sceVu0FVECTOR vec2)
{
    float f1;
    float f2;

    f1 = sceVu0InnerProduct(vec1, vec1);
    f2 = sceVu0InnerProduct(vec2, vec2);

    if (f2 < f1)
    {
        return 1;
    }

    return 0;
}

int FACheckSpeedLess(float speed, u_short obj)
{
    int i;

    if (obj & 0x8000)
    {
        if (CompareFloatVal(speed,plyr_wrk.spd) >= 1)
        {
            return 1;
        }
    }

    for (i = 0; i < 4; i++)
    {
        if (ene_wrk[i].sta & 0x1)
        {
            if (
                (obj & 0x4000 && ene_wrk[i].type == 0) ||
                (obj & 0x2000 && ene_wrk[i].type == 1) ||
                (obj & 0x1000 && ene_wrk[i].type == 2)
            )
            {
                if (SpeedLessOrVecF(speed,ene_wrk[i].move_box.spd) != 0)
                {
                    return 1;
                }
            }
        }
    }

    // missing return 0 ??

    // luckily the function still "returns" 0 as `v0` will be zero when the loop ends,
    // but this is definitely a coincidence so the devs "got lucky" with this one.
    // a proper reimplementation should return 0 here!!
}

TextureAnimation* FSpeTexAnmCtrl(TextureAnimation *pta, FSPE_TEXTURE_ANM *ta)
{
    if (pta == NULL)
    {
        return NULL;
    }

    pta->AnmCnt = (ta->nowstep * pta->AnmStep) / 16;

    if (ta->regmode == 1)
    {
        ta->nowstep++;
    }
    else if (ta->regmode == 2)
    {
        ta->nowstep--;
    }

    if (ta->nowstep >= (pta->TexNum * 16) / pta->AnmStep)
    {
        switch (ta->looptype)
        {
        case 0:
            ta->regmode = 0;

            return NULL;
        break;
        case 1:
            ta->nowstep = 0;
        break;
        case 2:
            ta->regmode = 2;
        break;
        }
    }
    else if (ta->nowstep < 0)
    {
        switch (ta->looptype)
        {
        case 0:
            ta->regmode = 0;

            return NULL;
        break;
        case 1:
            ta->nowstep = ((pta->TexNum * 16) / pta->AnmStep) - 1;
        break;
        case 2:
            ta->regmode = 1;
        break;
        }
    }

    return pta;
}

void FSpeTexAnmStop(FURN_ACT_WRK *fawp)
{
    fawp->pta = NULL;
}

void FSpeTexAnmExe(u_char modelnum, FURN_ACT_WRK *fawp, u_char regmode, u_char looptype, u_char speed)
{
    static float trans_rate = 128.0f;
    static float trans_added = -8.0f / 225.0f;
    SgMaterial *matp;
    u_int *phead;
    u_int *tmpModelp;
    u_int *sgd_top;
    HeaderSection *hs;
    TextureAnimation *pta;

    tmpModelp = furn_addr_tbl[modelnum];

    fawp->pta = pta = GetTextureAnimation(tmpModelp);

    if (pta == NULL)
    {
        return;
    }

    fawp->pta->AnmStep = speed;

    fawp->ta.regmode = regmode;
    fawp->ta.looptype = looptype;

    if (regmode == 1)
    {
        fawp->ta.nowstep = 0;
    }
    else if (regmode == 2)
    {
        fawp->ta.nowstep = (pta->TexNum * 16) / pta->AnmStep - 1;
    }

    hs = (HeaderSection *)tmpModelp;

    matp = hs->matp;
    phead = hs->phead;

    while ((u_int)matp < (u_int)phead)
    {
        if (strncmp("sha_", matp->name, 4) == 0)
        {
            matp->Diffuse[3] = trans_rate;
        }

        matp++;
    }
}

void FSpeDelPointLight(FURN_ACT_WRK *fawp)
{
    if (fawp->lw_id >= 0)
    {
        DeletePointLight(fawp->lw_id);
    }

    if (fawp->buff != NULL)
    {
        ResetEffects(fawp->buff);
    }
}

void FSpeSetPointLight(FURN_ACT_WRK *fawp, float r, float g, float b)
{
    sceVu0FVECTOR *pl_pos;
    static float power = 40.0f;

    fawp->lw_id = -1;

    pl_pos = acsGetRopePos(fawp->furn_id);

    fawp->buff = SetEffects(0x14, 2, 1, pl_pos, 0xff, 0xdf, 0xa0, 0.82f, 1);

    if (fawp->buff != NULL)
    {
        fawp->util_v[0] = r;
        fawp->util_v[1] = g;
        fawp->util_v[2] = b;
        fawp->util_v[3] = 1.0f;

        fawp->lw_id = AddNewPointLight(pl_pos, &fawp->util_v, &power, fawp->room_no);
    }
}

static float tremble_width = 20.0f;
static float tremble_time = 40.0f;
static int tremble_count = 0;
u_char fspe_miku_act = 0;
u_char temp_miku_act = 0;
u_char thunder_time = 0;
u_char lightning_time = 0;
u_char thunder_state = 0;
u_char lightning_state = 0;
sceVu0FVECTOR *thunder_pos_p = NULL;
u_short thunder_sound_no = 0;
u_char thunder_room = 0;

float GetRandVal(float min, float max)
{
    float width;

    width = max - min;

    return min + (rand() / (float)RAND_MAX) * width;
}

void StopTrembleH(FURN_ACT_WRK *fawp, signed char num)
{
    sceVu0CopyVector(furn_wrk[fawp->fw_id].pos, tremble_h_wrk[num].ori_pos);

    tremble_h_wrk[num].state = ST_TREMBLE_VACANT;
}

signed char GetVacantTrembleHWrk()
{
    int i;

    for (i = 0; i < 5; i++)
    {
        if (tremble_h_wrk[i].state == ST_TREMBLE_VACANT)
        {
            return i;
        }
    }

    return -1;
}

signed char CallTrembleH(FURN_ACT_WRK *fawp)
{
    signed char i;
    TREMBLE_H_WRK *wrk;
    float rand_val;

    i = GetVacantTrembleHWrk();

    if (i < 0)
    {
        return -1;
    }

    wrk = &tremble_h_wrk[i];

    wrk->pos = &furn_wrk[fawp->fw_id].pos;
    wrk->rot = &furn_wrk[fawp->fw_id].rotate;

    sceVu0CopyVector(wrk->ori_pos, furn_wrk[fawp->fw_id].pos);
    sceVu0CopyVector(wrk->ori_rot, furn_wrk[fawp->fw_id].rotate);

    wrk->time = (u_char)GetRandVal(9.0f, 15.0f) * 0.5f;

    rand_val = GetRandVal(10.0f, 16.0f);
    wrk->speed = rand_val / (float)wrk->time;

    sceVu0UnitMatrix(wrk->mat);
    sceVu0RotMatrix(wrk->mat, wrk->mat, furn_wrk[fawp->fw_id].rotate);

    wrk->state = ST_TREMBLE_NOR;

    tremble_count = 0;

    return i;
}

void TrembleHCtrl()
{
    int i;
    float width;
    float temp;
    sceVu0FVECTOR tempv;
    TREMBLE_H_WRK *wrk;

    wrk = tremble_h_wrk;

    for (i = 0; i < 5; i++)
    {
        if (wrk->state != ST_TREMBLE_VACANT)
        {
            if (--wrk->time == 0xff)
            {
                wrk->time = GetRandVal(9.0f, 15.0f);

                if (wrk->state == ST_TREMBLE_REV)
                {
                    width = -GetRandVal(10.0f, 16.0f);

                    wrk->state = ST_TREMBLE_NOR;
                } else
                {
                    width = GetRandVal(10.0f, 16.0f);

                    wrk->state = ST_TREMBLE_REV;
                }

                wrk->speed = (width - wrk->nowdist) / wrk->time;
            }

            wrk->nowdist = wrk->nowdist + wrk->speed;

            tempv[0] = 0.0f;
            tempv[1] = wrk->nowdist;
            tempv[2] = 0.0f;
            tempv[3] = 1.0f;

            sceVu0ApplyMatrix(tempv, wrk->mat, tempv);
            sceVu0AddVector(*wrk->pos, tempv, wrk->ori_pos);

            if (tremble_count > 700)
            {
                tremble_width = 10.0f;
                tremble_time = 80.0f;
            } else
            {
                tremble_width = 50.0f;
                tremble_time = 20.0f;
            }

            temp = SgSinf((tremble_count * PI) / tremble_time);

            if (temp >= 0.0f)
            {
                temp = 1.0f - temp;
                (*wrk->rot)[2] = (1.0f - temp * temp) / tremble_width;
            }
            else
            {
                temp = temp + 1.0f;
                (*wrk->rot)[2] = (temp * temp - 1.0f) / tremble_width;
            }
        }

        wrk++;
    }

    tremble_count++;
}

void FSpeTrembleCtrl(FURN_ACT_WRK *fawp)
{
    float temp;

    if (fawp->trembleflg == 0)
    {
        return;
    }

    tremble_width += 0.1f;
    tremble_time -= 0.1f;

    if (tremble_time < 20.0f)
    {
        tremble_time = 1.0f; // yep

        furn_wrk[fawp->fw_id].rotate[0] = 0.0f;

        fawp->trembleflg = 0;

        tremble_width = 1.0f;
        tremble_time = 100.0f;
    }
    else
    {
        temp = SgSinf(fawp->count * PI / tremble_time);

        if (0.0f <= temp)
        {
            temp = 1.0f - temp;
            furn_wrk[fawp->fw_id].rotate[0] = (1.0f - temp * temp) / tremble_width;
        }
        else
        {
            temp = temp + 1.0f;
            furn_wrk[fawp->fw_id].rotate[0] = (temp * temp- 1.0f) / tremble_width;
        }
    }
}

void CallThunderLight(float x, float y, float z, u_char delay, sceVu0FVECTOR *pos, u_char room_no)
{
    if (thunder_state != 0 || lightning_state != 0 || plyr_wrk.sta & 0x1)
    {
        return;
    }

    lightning_time = 0;

    thunder_time = delay;
    thunder_state = lightning_state = 4;
    thunder_pos_p = pos;
    thunder_room = room_no;

    if (rand() < RAND_MAX / 2)
    {
        thunder_sound_no = 0;
    }
    else
    {
        thunder_sound_no = 0;
    }

    thunder_direction[0] = x;
    thunder_direction[1] = y;
    thunder_direction[2] = z;
    thunder_direction[3] = 1.0f;

    sceVu0CopyVector(plyr_wrk.shadow_direction, thunder_direction);
}

void SetThunderLightEach(PARARELL_WRK *par, int *num)
{
    u_char pn;

    pn = 2;

    if (*num < 3)
    {
        pn = *num;

        (*num)++;
    }

    par = &par[pn];

    sceVu0CopyVector(par->direction, thunder_direction);

    par->diffuse[0] = 0.8f;
    par->diffuse[1] = 0.8f;
    par->diffuse[2] = 1.0f;
    par->diffuse[3] = 1.0f;
}

void SetThunderLight()
{
    int i;

    SetThunderLightEach(room_wrk.mylight[0].parallel, &room_wrk.mylight[0].parallel_num);
    SetThunderLightEach(plyr_wrk.mylight.parallel, &plyr_wrk.mylight.parallel_num);

    for (i = 0; i < 60; i++)
    {
        SetThunderLightEach(furn_wrk[i].mylight.parallel, &furn_wrk[i].mylight.parallel_num);
    }
}

void InitThunderLight()
{
    int i;

    plyr_wrk.mylight.parallel_num = 1;

    room_wrk.mylight[0].parallel_num = 0;

    for (i = 0; i < 60; i++)
    {
        furn_wrk[i].mylight.parallel_num = 0;
    }

    plyr_wrk.shadow_direction[3] = 0.0f;
}

void ThunderLight()
{
    switch (lightning_state)
    {
    case 4:
        if (--lightning_time != 0xff)
        {
            break;
        }

        lightning_state = 1;
        lightning_time = 4;
    // fall-through
    case 1:
        lightning_time--;

        SetThunderLight();

        if (lightning_time == 0)
        {
            lightning_state = 2;
            lightning_time = 4;

            InitThunderLight();
        }
    break;
    case 2:
        lightning_time--;

        if (lightning_time == 0)
        {
            lightning_state = 3;
            lightning_time = 25;

            plyr_wrk.shadow_direction[3] = 1.0f;
        }
    break;
    case 3:
        lightning_time--;

        SetThunderLight();

        if (lightning_time == 0)
        {
            lightning_state = 0;

            InitThunderLight();
        }
    break;
    }

    switch (thunder_state)
    {
    case 4:
        if (--thunder_time != 0xff)
        {
            break;
        }

        if (thunder_room == 21)
        {
            SeStartPosSrundFlame(21, *thunder_pos_p, 0, 0x1000, 0x1000);
        }
        else
        {
            SeStartPosEventFlame(thunder_room, 0, *thunder_pos_p, 0, 0x1000, 0x1000);
        }

        thunder_state = 1;
        thunder_time = 4;
    // fall-through
    case 1:
        thunder_time--;

        VibrateRequest1(0, 1);

        if (thunder_time == 0)
        {
            thunder_state = 2;
            thunder_time = 4;
        }
    break;
    case 2:
        if (--thunder_time == 0)
        {
            thunder_state = 3;
            thunder_time = 90;
        }
    break;
    case 3:
        thunder_time--;

        VibrateRequest1(0, 1);

        if (thunder_time == 0)
        {
            thunder_state = 0;
        }
    break;
    }
}

u_char* CallLampTremble(u_char *pointer)
{
    while (*pointer != 0xff)
    {
        acsRopeMoveRequest(*pointer, 3, 1.3f);

        pointer++;
    }

    return pointer;
}

u_char* StopLampTremble(u_char *pointer)
{
    while (*pointer != 0xff)
    {
        acsRopeMoveStop(*pointer);

        pointer++;
    }

    return pointer;
}
