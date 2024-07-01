#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "se_ev.h"

#include "os/eeiop/eese.h"
#include "os/eeiop/se_data.h"

#define SE_EV_WRK_SIZE 24

SE_EV_WRK se_ev_wrk[SE_EV_WRK_SIZE] = {0};

static void SeEvInitOne(SE_EV_WRK *seewp);
static SE_EV_WRK * SeEvGetSeEvWrk(u_char no);

void SeEvInit()
{
    SE_EV_WRK *seewp;
    int i;

    seewp = SeEvGetSeEvWrk(0);

    for (i = 0; i < 24; i++)
    {
        SeEvInitOne(&seewp[i]);
    }
}

static void SeEvInitOne(SE_EV_WRK *seewp)
{
    if (seewp != NULL)
    {
        seewp->count = 0;
        seewp->sew_no0 = 0xff;
        seewp->mode = 0;
    }
}

void SeEvMain()
{
    SE_EV_WRK *seewp;
    SE_WRK *swp;
    int i;
    
    seewp = SeEvGetSeEvWrk(0);
    
    for (i = 0; i < 24; i++, seewp++)
    {
            switch (seewp->mode)
            {
            case 0:
                // ...
            break;
            case 1:
                seewp->count--;
                
                if (seewp->count < 1)
                {
                    SeEvStop(i);
                    SeEvInitOne(seewp);
                }
            }
        
        if (seewp->sew_no0 != 0xff)
        {
            swp = SeGetSeWrk(seewp->sew_no0);
            
            if (swp == NULL)
            {
                seewp->sew_no0 = 0xff;
            }
            else if (swp->status == 0)
            {
                seewp->sew_no0 = 0xff;
            }
        }
    }
}

void SeEvStopAll()
{
    int i;

    for (i = 0; i < 24; i++)
    {
        SeEvStop(i);
    }
}

void SeEvStop(int se_ev_pos)
{
    SE_EV_WRK *seewp;
    
    seewp = SeEvGetSeEvWrk(se_ev_pos);
    
    if (seewp != NULL)
    {
        if (seewp->sew_no0 != 0xff)
        {
            SeStop(seewp->sew_no0);
            
            seewp->sew_no0 = 0xff;
        }
        
        seewp->mode = 0;
        seewp->count = 0;
    }
}

void SeEvReq(int se_req_no, int se_ev_pos, int count, char mode)
{
    SE_EV_WRK *seewp;
    SE_STE *ssp;
    u_char free_sw;
    
    seewp = SeEvGetSeEvWrk(se_ev_pos);
    ssp = SeGetSeSte(se_req_no);
    
    if (seewp != NULL)
    {
        free_sw = SeGetFreeSv();
        
        if (free_sw == 0xff)
        {
            printf("SeEvReq free_se = 0xFF\n");
        }
        else
        {
            seewp->sew_no0 = free_sw;
            SeStartV(ssp->se_no0, seewp->sew_no0);
            if (mode == 1)
            {
                seewp->mode = 1;
                seewp->count = count;
            }
            else
            {
                seewp->mode = 0;
                seewp->count = 0;
            }
            
            printf("SeEvReq se_no = %d, v_no = %d\n", ssp->se_no0, seewp->sew_no0);
        }
    }
}

void SeEvSetPitch(int se_ev_pos,u_short pitch)
{
    SE_EV_WRK *seewp = SeEvGetSeEvWrk(se_ev_pos);

    if (seewp && seewp->sew_no0 != 0xff) 
    {
        SeCmdSetPitch(seewp->sew_no0, pitch);
    }
}

u_short SeEvGetPitch(int se_ev_pos)
{
    SE_EV_WRK *seewp = SeEvGetSeEvWrk(se_ev_pos);
  
    if (seewp && seewp->sew_no0 != 0xff) 
    {
        return 0;
    }
}

static SE_EV_WRK * SeEvGetSeEvWrk(u_char no)
{
    if (no < SE_EV_WRK_SIZE) {
        return &se_ev_wrk[no];
    }
    
    return NULL;
}

