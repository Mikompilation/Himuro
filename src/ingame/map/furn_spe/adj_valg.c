#include "common.h"
#include "typedefs.h"
#include "adj_valg.h"

#include "sce/libvu0.h"

static ADJ_VALG_WRK adj_valg_wrk[15];

void InitAdjValGWrk()
{
    memset(adj_valg_wrk, 0, sizeof(adj_valg_wrk));

    adj_valg_wrk[0].pre = adj_valg_wrk;
    adj_valg_wrk[0].next = NULL;
}

void InitAdjValGWrkOne(ADJ_VALG_WRK *avp)
{
    *avp = (ADJ_VALG_WRK){0};
}

ADJ_VALG_WRK* SearchVacantAdjValGWrk()
{
    int i;

    for (i = 1; i < 15; i++)
    {
        if (adj_valg_wrk[i].ori == NULL && adj_valg_wrk[i].time == 0)
        {
            return &adj_valg_wrk[i];
        }
    }

    return NULL;
}

void ReleaseAdjValGWrkVec(sceVu0FVECTOR ori)
{
    ADJ_VALG_WRK *avp;

    avp = adj_valg_wrk->next;

    while (avp != NULL)
    {
        if (ori == *avp->ori)
        {
            ReleaseAdjValG(avp);

            return;
        }

        avp = (ADJ_VALG_WRK *)avp->next;
    }
}

void ReleaseAdjValG(ADJ_VALG_WRK *avp)
{
    if (avp->next != NULL)
    {
        ((ADJ_VALG_WRK*)avp->next)->pre = avp->pre;
        ((ADJ_VALG_WRK*)avp->pre)->next = (ADJ_VALG_WRK*)avp->next;
    }
    else
    {
        ((ADJ_VALG_WRK*)avp->pre)->next = avp->next;
        adj_valg_wrk->pre = ((ADJ_VALG_WRK*)avp->pre);
    }

    InitAdjValGWrkOne(avp);
}

void SameObjectWrkDelete(sceVu0FVECTOR ori)
{
    ADJ_VALG_WRK *avp;

    avp = adj_valg_wrk->next;

    while (avp != NULL)
    {
        if (*avp->ori == ori)
        {
            ReleaseAdjValG(avp);

            return;
        }

        avp = (ADJ_VALG_WRK *)avp->next;
    }
}

void RegisterAdjValG(sceVu0FVECTOR ori, sceVu0FVECTOR des, u_int time)
{
    sceVu0FVECTOR temp;
    ADJ_VALG_WRK *avp;

    if (ori == NULL || des == NULL)
    {
        return;
    }

    avp = SearchVacantAdjValGWrk();

    if (avp == NULL || time == 0)
    {
        sceVu0CopyVector(ori,des);
    }
    else
    {
        ReleaseAdjValGWrkVec(ori);

        avp->pre = adj_valg_wrk[0].pre;
        avp->next = NULL;

        *(ADJ_VALG_WRK **)avp->pre = avp;

        adj_valg_wrk[0].pre = avp;

        sceVu0SubVector(temp, des, ori);
        sceVu0ScaleVector(temp, temp, 1.0f / time);

        avp->ori = (sceVu0FVECTOR *)ori;

        sceVu0CopyVector(avp->des,des);

        avp->time = time;
        avp->nowtime = 0;

        sceVu0CopyVector(avp->accel,temp);
    }
}

void AdjustValueG()
{
    ADJ_VALG_WRK *avp;
    avp = adj_valg_wrk->next;

    while (avp != NULL)
    {
        if (avp->nowtime == avp->time)
        {
            sceVu0CopyVector(*avp->ori, avp->des);
            ReleaseAdjValG(avp);
        }
        else
        {
            sceVu0AddVector(*avp->ori, *avp->ori, avp->accel);

            avp->nowtime ++;
        }

        avp = (ADJ_VALG_WRK *)avp->next;
    }
}
