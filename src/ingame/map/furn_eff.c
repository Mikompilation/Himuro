#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "furn_eff.h"

#include "graphics/graph2d/effect.h"
#include "ingame/enemy/ene_ctl.h"
#include "ingame/ig_glob.h"
#include "ingame/map/furn_dat.h"
#include "ingame/map/furn_eff.h"
#include "ingame/plyr/plyr_ctl.h"

static void FurnEfctSetUse(FURN_WRK *fwp, float *pos, int fewrk_no);
static void FurnEfctSetPos0(FURN_WRK *fwp, float *pos);
static void FurnEfctSetPos1(FURN_WRK *fwp, float *pos);
static void FurnEfctSetRot0(FURN_WRK *fwp, float *rot);
static int FurnEfctGetFefct();
static void FurnEfctFreeFefct(int num);

#include "data/fefct_fire.h" // FURN_EFCT_FIRE fefct_fire[];
#include "data/fefct_sunshine.h" // FURN_EFCT_SUNSHINE fefct_sunshine[];
#include "data/fefct_deform.h" // FURN_EFCT_DEFORM fefct_deform[];

void FurnEfctSet(FURN_WRK *fwp)
{
    float *pos_top;
    int fewrk_no;
    u_char ef_no;
    u_char ef_type;

    if (
        furn_dat[fwp->id].ef_type != FEFCT_FIRE0 && furn_dat[fwp->id].ef_type != FEFCT_KASA0 &&
        furn_dat[fwp->id].ef_type != FEFCT_SUNSHINE0 && furn_dat[fwp->id].ef_type != FEFCT_DEFORM0
    )
    {
        return;
    }

    if (furn_attr_flg[fwp->id].flg & 0x4)
    {
        return;
    }

    fewrk_no = FurnEfctGetFefct();

    if (fewrk_no == -1)
    {
        return;
    }

    ef_no = furn_dat[fwp->id].ef_no; ef_type = furn_dat[fwp->id].ef_type;

    if (ef_type == 2 && ef_no < 3)
    {
        pos_top = (float *)&fefct_sunshine[ef_no];

        FurnEfctSetUse(fwp, pos_top, fewrk_no);

        pos_top += 3;

        FurnEfctSetPos1(fwp, pos_top);

        pos_top += 3;

        FurnEfctSetRot0(fwp, pos_top);

        fefct_wrk[fewrk_no].addr = SetEffects(
            31, 2,
            fefct_wrk[fewrk_no].pos1, fefct_wrk[fewrk_no].pos0,
            fefct_wrk[fewrk_no].rot0, fefct_sunshine[ef_no].power,
            fefct_sunshine[ef_no].w, fefct_sunshine[ef_no].h,
            fefct_sunshine[ef_no].r, fefct_sunshine[ef_no].g, fefct_sunshine[ef_no].b
        );

        if (fefct_wrk[fewrk_no].addr == NULL)
        {
            FurnEfctFreeFefct(fewrk_no);
        }
    }
}

void FurnEfctFree(FURN_WRK *fwp)
{
    if (fwp->fewrk_no != 0xff)
    {
        if (fefct_wrk[fwp->fewrk_no].use == 1 && fefct_wrk[fwp->fewrk_no].addr != NULL)
        {
            fefct_wrk[fwp->fewrk_no].use = 0;

            ResetEffects(fefct_wrk[fwp->fewrk_no].addr);
        }

        fwp->fewrk_no = 0xff;
    }
}

static void FurnEfctSetUse(FURN_WRK *fwp, float *pos, int fewrk_no)
{
    fwp->fewrk_no = fewrk_no;

    fefct_wrk[fewrk_no].use = 1;

    FurnEfctSetPos0(fwp, pos);
}

static void FurnEfctSetPos0(FURN_WRK *fwp, float *pos)
{
    FURN_EFCT *fep;

    fep = &fefct_wrk[fwp->fewrk_no];

    fep->pos0[0] = fwp->pos[0] + *pos++;
    fep->pos0[1] = fwp->pos[1] + *pos++;
    fep->pos0[2] = fwp->pos[2] + *pos++;
    fep->pos0[3] = 1.0f;
}

static void FurnEfctSetPos1(FURN_WRK *fwp, float *pos)
{
    FURN_EFCT *fep;

    fep = &fefct_wrk[fwp->fewrk_no];

    fep->pos1[0] = *pos++;
    fep->pos1[1] = *pos++;
    fep->pos1[2] = *pos++;
    fep->pos1[3] = 1.0f;
}

static void FurnEfctSetRot0(FURN_WRK *fwp, float *rot)
{
    FURN_EFCT *fep;

    fep = &fefct_wrk[fwp->fewrk_no];

    fep->rot0[0] = fwp->rotate[0] + *rot++;
    fep->rot0[1] = fwp->rotate[1] + *rot++;
    fep->rot0[2] = *rot++;
    fep->rot0[3] = 1.0f;
}

static int FurnEfctGetFefct()
{
    FURN_EFCT *fep;
    int i;

    fep = fefct_wrk;

    for (i = 0; i < 60; i++)
    {
        if (fep->use == 0)
        {
            return i;
        }

        fep++;
    }

    return -1;
}

static void FurnEfctFreeFefct(int num)
{
    if (num < 60)
    {
        fefct_wrk[num].use = 0;
    }
}

void FurnHintDeformCtrl()
{
    FURN_WRK *fwp;
    float sclx;
    float scly;
    u_int fsta;
    int i;
    int j;
    u_char ef_no;
    u_char ef_type;
    static sceVu0FVECTOR ep[10];
    static float spd = 0.5f;
    static float rate = 1.0f;
    static float trate = 1.0f;

    if (EnemyUseJudge(0) != 0)
    {
        return;
    }

    for (fwp = furn_wrk, i = 0, j = 0; i < 60; i++, fwp++)
    {
        if (j >= 10)
        {
            break;
        }

        if (GetFurnHintPos(fwp, ep[j], &fsta) != 0)
        {
            ef_no = furn_dat[fwp->id].ef_no;
            ef_type = furn_dat[fwp->id].ef_type;

            if (ef_type != 0xff)
            {
                sclx = fefct_deform[ef_no].sclw / 10.0f;
                scly = fefct_deform[ef_no].sclh / 10.0f;
            }
            else
            {
                sclx = 0.7f;
                scly = 0.7f;
            }

            SetEffects(27, 1, 23, 80, sclx, scly, &ep[j], 0, 0, 0, 0, &spd, &rate, &trate);

            j++;
        }
    }
}
