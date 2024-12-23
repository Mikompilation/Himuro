#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "effect.h"

#include "gcc/stdarg.h"

#include "sce/libvu0.h"

#include "main/glob.h"
#include "ingame/ig_glob.h"
#include "ingame/event/ev_main.h"
#include "ingame/photo/pht_make.h"
#include "graphics/graph3d/gra3d.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/libsg.h"
#include "graphics/graph2d/sprt.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph2d/rare_ene.h"
#include "graphics/graph2d/g2d_debug.h"
#include "graphics/graph2d/effect_ene.h"
#include "graphics/graph2d/effect_obj.h"
#include "graphics/graph2d/effect_oth.h"
#include "graphics/graph2d/effect_rdr.h"
#include "graphics/graph2d/effect_scr.h"
#include "graphics/graph2d/effect_sub.h"
// #include "ingame/entry/ap_sgost.h" // DO NOT INCLUDE

#include "data/effdat.h" // SPRT_DAT effdat[];
#include "data/camdat.h" // SPRT_DAT camdat[];
static int tmp_effect_off = 0;
static void *r23_e1 = 0;
static void *r23_e2 = 0;
static u_char r28_torch_flag = 0;

void InitEffects()
{
    eff_blur_off = 1;
    eff_dith_off = 0;
    look_debugmenu = 1;
    stop_effects = 0;
    change_efbank = 1;
    monochrome_mode = 0;
    magatoki_mode = 0;
    shibata_set_off = 0;
    eff_filament_off = 0;
    effect_disp_flg = 1;
    
    InitShibataSet();
    InitEffectSub();
    InitEffectScr();
    InitEffectObj();
    InitEffectOth();
    InitEffectRdr();
    InitEffectEne();
    EneDmgTexInit();
    InitPhotoMake();
    
    memset(efcnt, 0, 64 * sizeof(EFFECT_CONT));
    memset(efcnt_cnt, 0, 64 * sizeof(EFFECT_CONT));
    memset(efcntm, 0, 48 * sizeof(EFFECT_CONT));
    memset(efcntm_cnt, 0, 48 * sizeof(EFFECT_CONT));
    
    SetScreenEffect();
    
    r23_e1 = NULL;
    r23_e2 = NULL;
    r28_torch_flag = 0;
    
    set_buffer[0] = 0;
    set_buffer[1] = 0;
    now_buffer[0] = 0;
    now_buffer[1] = 0;
}

void InitEffectsEF()
{
    int i;
    static void *e[3] = {0};
    static float aalp = 1.0f;
    static sceVu0FVECTOR canal1 = {27200.0f, -2000.0f, 29500.0f, 1.0f};
    static sceVu0FVECTOR canal2 = {28000.0f, -2000.0f, 29250.0f, 1.0f};
    static sceVu0FVECTOR torch_pos[3] = {
        { 7825.0f, 2600.0f, 35150.0f, 1.0f},
        { 7825.0f, 2575.0f, 38100.0f, 1.0f},
        {11225.0f, 2600.0f, 35150.0f, 1.0f},
    };
    
    set_buffer[0] = now_buffer[0];
    set_buffer[1] = now_buffer[1];
    
    effect_disp_flg = (ingame_wrk.stts & 0x20) == 0 ? effect_disp_flg : 0;
    
    now_buffer[0] = 0;
    now_buffer[1] = 0;
    
    InitEffectScrEF();
    InitEffectObjEF();
    InitEffectOthEF();
    InitEffectRdrEF();
    InitEffectEneEF();
    SettleGhostDispEffect();
    
    if (shibata_set_off == 0 && look_debugmenu != 0)
    {
        if (msbtset.dt.sw != 0)
        {
            SetEffects(2, 1, msbtset.dt.type, msbtset.dt.alp, msbtset.dt.spd, msbtset.dt.amax, msbtset.dt.cmax);
        }
        
        if (msbtset.bl.sw != 0)
        {
            SetEffects(3, 1, &msbtset.bl.alp, msbtset.bl.scl, msbtset.bl.rot, msbtset.bl.x, msbtset.bl.y);
        }
        
        if (msbtset.df.sw != 0)
        {
            SetEffects(6, 1, msbtset.df.type, msbtset.df.rate);
        }
        
        if (msbtset.cn.sw != 0)
        {
            switch (msbtset.cn.type)
            {
                case 1:
                    SetEffects(0xd, 1, msbtset.cn.col, msbtset.cn.alp);
                break;
                case 2:
                    SetEffects(0xe, 1, msbtset.cn.col, msbtset.cn.alp);
                break;
                case 3:
                    SetEffects(0xf, 1, msbtset.cn.col, msbtset.cn.alp);
                break;
            }
        }
        
        if (msbtset.ff.sw != 0)
        {
            SetEffects(9, 1, msbtset.ff.alp, 0x80000);
        }
        
        if (msbtset.ng.sw != 0)
        {
            SetEffects(0xc, 1, msbtset.ng.col, msbtset.ng.alp, msbtset.ng.alp2);
        }
        
        if (msbtset.mn.sw == 0)
        {
            if (efcnt[45].dat.uc8[0] == 0)
            {
                efcnt[45].dat.uc8[0] = 1;
                ChangeMonochrome(0);
            }
        }
        
        if (msbtset.mn.sw != 0 && efcnt[45].dat.uc8[0] == 0)
        {
            efcnt[45].dat.uc8[0] = 1;
            ChangeMonochrome(1);
        }
    }
    
    for (i = 0; i < 3; i++)
    {
        if (fly_display[i] != 0)
        {
            if (e[i] == NULL)
            {
                e[i] = SetEffects(0x22, 2, fly_wrk[i].move_box.pos, 0x50, 0x50, 0x5c, &aalp, i);
            }
        }
        else
        {
            if (e[i] != NULL)
            {
                ResetEffects(e[i]);
                e[i] = NULL;
            }
        }
    }
    
    if (plyr_wrk.pr_info.room_no != 0x17)
    {
        if (r23_e1 != NULL)
        {
            ResetEffects(r23_e1);
            r23_e1 = NULL;
        }
        if (r23_e2 != NULL)
        {
            ResetEffects(r23_e2);
            r23_e2 = NULL;
        }
    }
    else
    {        
        if (r23_e1 == NULL)
        {
            r23_e1 = SetEffects(0x1e, 2, canal1, 1, 250.0, 200, 0x80, 0x80, 0x80);
        }
        
        if (r23_e2 == NULL)
        {
            r23_e2 = SetEffects(0x1e, 2, canal2, 4, 250.0, 0x104, 0x80, 0x80, 0x80);
        }
    }
    
    if (plyr_wrk.pr_info.room_no != 29)
    {
        if (r28_torch_flag != 0)
        {
            for (i = 0; i < 3; i++)
            {
                ResetRDPFire(i);
            }
        }
        
        r28_torch_flag = 0;
    }
    else
    {
        if (r28_torch_flag == 0)
        {
            for (i = 0; i < 3; i++)
            {
                SetRDPFire(torch_pos[i], i);
            }
            
            r28_torch_flag = 1;
        }
    }
}

SBTSET msbtset = {0};
EFFECT_CONT efcnt[64] = {0};
EFFECT_CONT efcntm[48] = {0};
EFFECT_CONT efcnt_cnt[64] = {0};
EFFECT_CONT efcntm_cnt[48] = {0};

void EffectEndSet()
{
    eff_blur_off = 0;
}

void *SetEffects(int id, int fl, ...)
{
    int ret;
    va_list ap;
    EFFECT_CONT *ec;
    
    if (
        (ingame_wrk.stts & 0x20) &&
        ev_wrk.movie_on != 4 &&
        effect_disp_flg == 0
    )
    {
        return NULL;
    }
    
    switch(id)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 19:
    case 35:
    case 36:
    case 37:
        if (change_efbank != 0)
        {
            ec = &efcnt[id];
        }
        else
        {
            ec = &efcnt_cnt[id];
        }
    break;
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 38:
    case 40:
        ret = SearchEmptyEffectBuf();
        
        if (ret == -1)
        {
            return NULL;
        }
        
        if (change_efbank != 0)
        {
            ec = &efcntm[ret];
        }
        else
        {
            ec = &efcntm_cnt[ret];
        }
    break;
    default:
    return NULL;
    }

    va_start(ap, fl);
    
    switch(id)
    {
    case 1:
        ec->dat.uc8[0] = 1;
        ec->dat.uc8[1] = fl;
    break;
    case 36:
        ec->dat.uc8[0] = 36;
        ec->dat.uc8[1] = fl;
    break;
    case 2:
        ec->dat.uc8[0] = 2;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.fl32[3] = va_arg(ap, double);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        ec->dat.uc8[4] = va_arg(ap, u_char);
        
        if (fl & 4)
        {
            ec->in = va_arg(ap, u_int);
            ec->keep = va_arg(ap, u_int);
            ec->out = va_arg(ap, u_int);
            ec->cnt = 0;
            ec->flow = (ec->in == 0 ? (ec->keep == 0 ? ec->out != 0 ? 2 : 3 : 1) : 0);
        }
    break;
    case 35:
        ec->dat.uc8[0] = 35;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.fl32[1] = va_arg(ap, double);
        ec->dat.fl32[2] = va_arg(ap, double);
    break;
    case 3:
        ec->dat.uc8[0] = 3;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = 0;
        ec->pnt[0] = va_arg(ap, void *);
        ec->dat.ui32[2] = va_arg(ap, u_int);
        ec->dat.ui32[3] = va_arg(ap, u_int);
        ec->fw[0] = va_arg(ap, double);
        ec->fw[1] = va_arg(ap, double);
    break;
    case 4:
        ec->dat.uc8[0] = 4;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = 1;
        ec->pnt[0] = va_arg(ap, void *);
        ec->dat.ui32[2] = va_arg(ap, u_int);
        ec->dat.ui32[3] = va_arg(ap, u_int);
        ec->fw[0] = va_arg(ap, double);
        ec->fw[1] = va_arg(ap, double);
    break;
    case 5:
        ec->dat.uc8[0] = 5;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = 2;
        ec->pnt[0] = va_arg(ap, void *);
        ec->dat.ui32[2] = va_arg(ap, u_int);
        ec->dat.ui32[3] = va_arg(ap, u_int);
        ec->fw[0] = va_arg(ap, double);
        ec->fw[1] = va_arg(ap, double);
    break;
    case 6:
        ec->dat.uc8[0] = 6;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        ec->dat.uc8[4] = 0;
        
        if (fl & 4)
        {
            ec->flow = (ec->in == 0 ? (ec->keep == 0 ? ec->out != 0 ? 2 : 3 : 1) : 0);
            ec->cnt = 0;
            ec->in = va_arg(ap, u_int);
            ec->keep = va_arg(ap, u_int);
            ec->out = va_arg(ap, u_int);
        }
    break;
    case 7:
        ec->dat.uc8[0] = 7;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
    break;
    case 42:
        ec->dat.uc8[0] = 42;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = 0;
        ec->pnt[0] = va_arg(ap, void *);
    break;
    case 43:
        ec->dat.uc8[0] = 42;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = 1;
        ec->pnt[0] = va_arg(ap, void *);
    break;
    case 44:
        ec->dat.uc8[0] = 42;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = 2;
        ec->pnt[0] = va_arg(ap, void *);
    break;
    case 45:
        ec->dat.uc8[0] = 45;
        ec->dat.uc8[1] = fl;
    break;
    case 46:
        ec->dat.uc8[0] = 46;
        ec->dat.uc8[1] = fl;
    break;
    case 8:
        ec->dat.uc8[0] = 8;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
    break;
    case 9:
        ec->dat.uc8[0] = 9;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.ui32[1] = va_arg(ap, u_int);
    break;
    case 10:
        ec->dat.uc8[0] = 10;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->pnt[0] = va_arg(ap, void *);
        ec->pnt[1] = va_arg(ap, void *);
    break;
    case 37:
        ec->dat.uc8[0] = 37;
        ec->dat.uc8[1] = fl;
    break;
    case 11:
        ec->dat.uc8[0] = 11;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
    break;
    case 12:
        ec->dat.uc8[0] = 12;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        
        if (fl & 4)
        {
            ec->in = va_arg(ap, u_int);
            ec->keep = va_arg(ap, u_int);
            ec->out = va_arg(ap, u_int);
            ec->cnt = 0;
            ec->flow = (ec->in == 0 ? (ec->keep == 0 ? ec->out != 0 ? 2 : 3 : 1) : 0);
        }
        else
        {
            ec->pnt[0] = va_arg(ap, void *);
        }
    break;
    case 13:
        ec->dat.uc8[0] = 13;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
    break;
    case 14:
        ec->dat.uc8[0] = 14;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
    break;
    case 15:
        ec->dat.uc8[0] = 15;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
    break;
    case 16:
        ec->dat.uc8[0] = 16;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = fl;
        ec->flow = 0;
        ec->in = va_arg(ap, u_int);
        ec->out = va_arg(ap, u_int);
        ec->pnt[0] = va_arg(ap, void *);
    break;
    case 17:
        ec->dat.uc8[0] = 17;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg (ap, char);
    break;
    case 18:
        ec->dat.uc8[0] = 18;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg (ap, char);
    break;
    case 19:
        ec->dat.uc8[0] = 19;
        ec->dat.uc8[1] = fl;
        ec->flow = va_arg (ap, char);
        ec->pnt[0] = va_arg(ap, void *);
        ec->dat.uc8[2] = va_arg (ap, u_char);
        ec->dat.uc8[3] = va_arg (ap, u_char);
        ec->dat.uc8[4] = va_arg (ap, u_char);
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.uc8[5] = va_arg (ap, u_char);
        ec->dat.uc8[6] = va_arg (ap, u_char);
        ec->dat.uc8[7] = va_arg (ap, u_char);
        ec->dat.fl32[3] = va_arg (ap, double);
        ec->pnt[1] = va_arg (ap, void *);
        ec->cnt = 0;
        ec->max = 0;
        ec->keep = 0;
    break;
    case 38:
        ec->dat.uc8[0] = 38;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg (ap, u_char);
    break;
    case 41:
        ec->dat.uc8[0] = 41;
        ec->dat.uc8[1] = fl;
        ec->pnt[0] = va_arg(ap, void *);
    break;
    case 23:
        ec->dat.uc8[0] = 23;
        ec->dat.uc8[1] = fl | 0x80;
        ec->flow = va_arg(ap, char);
        ec->pnt[0] = va_arg(ap, void *);
        ec->dat.uc8[2] = va_arg (ap, u_char);
        ec->dat.uc8[3] = va_arg (ap, u_char);
        ec->dat.uc8[4] = va_arg (ap, u_char);
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.uc8[5] = va_arg (ap, u_char);
        ec->dat.uc8[6] = va_arg (ap, u_char);
        ec->dat.uc8[7] = va_arg (ap, u_char);
        ec->dat.fl32[3] = va_arg (ap, double);
        
        if (ec->flow == 3)
        {
            float r;
            
            // inlined from effect.h
            r = vu0Rand();
            // end of inlined section
            ec->cnt = r * 21.0f;
        }
        else
        {
            ec->cnt = 0;
        }
    break;
    case 24:
        ec->dat.uc8[0] = 24;
        ec->dat.uc8[1] = fl | 0x80;
        ec->flow = va_arg (ap, char);
        ec->pnt[0] = va_arg(ap, void *);
        ec->dat.uc8[2] = va_arg (ap, u_char);
        ec->dat.uc8[3] = va_arg (ap, u_char);
        ec->dat.uc8[4] = va_arg (ap, u_char);
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.uc8[5] = va_arg (ap, u_char);
        ec->dat.uc8[6] = va_arg (ap, u_char);
        ec->dat.uc8[7] = va_arg (ap, u_char);
        ec->dat.fl32[3] = va_arg (ap, double);
        ec->fw[0] = va_arg (ap, double);
        
        if (ec->flow == 3)
        {
            float r;
            // inlined from effect.h
            r = vu0Rand();
            // end of inlined section
            ec->cnt = r * 21.0f;
        }
        else
        {
            ec->cnt = 0;
        }
    break;
    case 20:
        ec->dat.uc8[0] = 20;
        ec->dat.uc8[1] = fl | 0x80;
        ec->dat.uc8[2] = va_arg (ap, u_char);
        ec->pnt[0] = va_arg(ap, void *);
        ec->dat.uc8[3] = va_arg (ap, u_char);
        ec->dat.uc8[4] = va_arg (ap, u_char);
        ec->dat.uc8[5] = va_arg (ap, u_char);
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.uc8[6] = va_arg (ap, u_char);
    break;
    case 21:
        ec->dat.uc8[0] = 21;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg (ap, u_char);
        ec->dat.uc8[3] = va_arg (ap, u_char);
        ec->dat.uc8[4] = 0;
        ec->dat.uc8[7] = 0;
        ec->pnt[0] = va_arg(ap, void *);
    break;
    case 22:
        ec->dat.uc8[0] = 22;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg (ap, u_char);
        ec->dat.uc8[3] = va_arg (ap, u_char);
        ec->cnt = 0;
        ec->dat.uc8[4] = va_arg (ap, u_char);
        ec->dat.uc8[5] = va_arg (ap, u_char);
        ec->dat.uc8[6] = va_arg (ap, u_char);
        ec->dat.uc8[7] = 0;
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.fl32[3] = va_arg (ap, double);
        ec->pnt[0] = va_arg (ap, void *);
        ec->pnt[1] = va_arg (ap, void *);
        
        if (fl & 8)
        {
            ec->dat.uc8[7] = va_arg (ap, u_char);
        }
    break;
    case 27:
        ec->dat.uc8[0] = 27;
        ec->dat.uc8[1] = fl | 0x80;
        ec->dat.uc8[2] = va_arg (ap, u_char);
        ec->max = va_arg(ap, u_int);
        ec->dat.uc8[4] = 0xff;
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.fl32[3] = va_arg(ap, double);
        ec->pnt[0] = va_arg(ap, void *);
        ec->fw[0] = 0.0f;
        ec->cnt = 0;
        ec->in = va_arg(ap, u_int);
        ec->keep = va_arg (ap, u_int);
        ec->out = va_arg (ap, u_int);
        ec->pnt[1] = va_arg (ap, void *);
        ec->pnt[2] = va_arg (ap, void *);
        ec->pnt[4] = va_arg (ap, void *);
        ec->pnt[5] = NULL;
        ec->pnt[5] = va_arg (ap, void *);
        
        if (fl & 4)
        {
            ec->flow = (ec->in == 0 ? (ec->keep == 0 ? ec->out != 0 ? 2 : 3 : 1) : 0);
        }
    break;
    case 39:
        ec->dat.uc8[0] = 39;
        ec->dat.uc8[1] = fl | 0x80;
        ec->dat.uc8[2] = va_arg (ap, u_char);
        ec->dat.uc8[3] = va_arg (ap, u_char);
        ec->dat.uc8[4] = 0xff;
        ec->dat.fl32[3] = va_arg(ap, double);
        ec->pnt[0] = va_arg(ap, void *);
        ec->cnt = 0;
        ec->in = va_arg(ap, u_int);
        ec->keep = va_arg(ap, u_int);
        ec->out = va_arg (ap, u_int);
        ec->max = 100;
        
        if (fl & 4)
        {
            ec->flow = (ec->in == 0 ? (ec->keep == 0 ? ec->out != 0 ? 2 : 3 : 1) : 0);
        }
    break;
    case 40:
        ec->dat.uc8[0] = 40;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = 0;
        ec->dat.uc8[3] = 0;
        ec->dat.uc8[4] = 0;
        ec->dat.uc8[6] = va_arg (ap, u_char);
        ec->dat.uc8[5] = va_arg (ap, u_char);
        ec->dat.uc8[7] = 0;
        ec->fw[0] = 2.0f;
        ec->fw[1] = 0.0f;
        ec->fw[2] = va_arg(ap, double);
        ec->pnt[0] = NULL;
    break;
    case 29:
        ec->dat.uc8[0] = 29;
        ec->dat.uc8[1] = fl | 0x80;
        ec->dat.uc8[2] = 1;
        ec->pnt[0] = va_arg(ap, void *);
    break;
    case 30:
        ec->dat.uc8[0] = 30;
        ec->dat.uc8[1] = fl | 0x80;
        ec->pnt[0] = va_arg(ap, void *);
        ec->dat.uc8[5] = va_arg (ap, u_char);
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.ui32[3] = 0;
        ec->cnt = ec->max = va_arg(ap, u_int);
        ec->dat.uc8[2] = va_arg (ap, u_char);
        ec->dat.uc8[3] = va_arg (ap, u_char);
        ec->dat.uc8[4] = va_arg (ap, u_char);
        ec->dat.uc8[6] = 0;
    break;
    case 31:
        ec->dat.uc8[0] = 31;
        ec->dat.uc8[1] = fl;
        ec->pnt[0] = va_arg(ap, void *);
        ec->pnt[1] = va_arg(ap, void *);
        ec->pnt[2] = va_arg(ap, void *);
        ec->max = va_arg(ap, u_int);
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.fl32[3] = va_arg(ap, double);
        ec->cnt = 0;
        ec->dat.uc8[2] = va_arg (ap, u_char);
        ec->dat.uc8[3] = va_arg (ap, u_char);
        ec->dat.uc8[4] = va_arg (ap, u_char);
    break;
    case 28:
        ec->dat.uc8[0] = 28;
        ec->dat.uc8[1] = fl | 0x80;
        ec->dat.uc8[2] = va_arg (ap, u_char);
        ec->pnt[0] = va_arg(ap, void *);
        ec->pnt[4] = va_arg(ap, void *);
        ec->pnt[1] = va_arg(ap, void *);
        ec->pnt[2] = va_arg(ap, void *);
        ec->pnt[3] = NULL;
        ec->dat.ui32[3] = va_arg(ap, u_int);
        ec->pnt[5] = va_arg(ap, void *);
    break;
    case 25:
        ec->dat.uc8[0] = 25;
        ec->dat.uc8[1] = fl | 0x80;
        ec->dat.uc8[2] = va_arg (ap, u_char);
        ec->dat.uc8[3] = 0;
        ec->pnt[0] = va_arg(ap, void *);
        ec->pnt[1] = NULL;
        ec->pnt[2] = va_arg(ap, void *);
        ec->pnt[3] = va_arg(ap, void *);
    break;
    case 26:
        ec->dat.uc8[0] = 26;
        ec->dat.uc8[1] = fl | 0x80;
        ec->pnt[0] = va_arg(ap, void *);
        ec->pnt[1] = NULL;
    break;
    case 32:
        ec->dat.uc8[0] = 32;
        ec->dat.uc8[1] = fl;
        ec->dat.fl32[1] = va_arg(ap, double);
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.fl32[3] = va_arg(ap, double);
        ec->dat.uc8[3] = va_arg (ap, u_char);
        ec->in = va_arg(ap, u_int);
        ec->keep = va_arg(ap, u_int);
        ec->out = va_arg(ap, u_int);
    break;
    case 33:
        ec->dat.uc8[0] = 33;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg (ap, u_char);
        ec->dat.uc8[3] = va_arg (ap, u_char);
        ec->dat.fl32[1] = va_arg (ap, double);
        ec->dat.fl32[2] = va_arg (ap, double);
        ec->dat.fl32[3] = va_arg (ap, double);
        ec->cnt = 0;
        ec->out = 0;
    break;
    case 34:
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[0] = 34;
        ec->pnt[0] = va_arg(ap, void *);
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        ec->dat.uc8[4] = va_arg(ap, u_char);
        ec->pnt[1] = va_arg(ap, void *);
        ec->dat.uc8[5] = va_arg(ap, u_char);
        ec->dat.uc8[6] = 0;
        ec->cnt = 0;
        ec->max = 0;
        ec->keep = 0;
    break;
    case 0:
        // do nothing ...
    break;
    }
    
    va_end(ap);
    
    return ec;
}

void ResetEffects(void *p)
{
    if (p != NULL)
    {
        *(u_char *)p = 0;
    }
}

int SearchEmptyEffectBuf()
{
    int i;
    int fl;
    EFFECT_CONT *ecm;
    
    fl = -1;
    
    if (change_efbank != 0)
    {
        ecm = efcntm;
    }
    else
    {
        ecm = efcntm_cnt;
    }
    
    for (i = 0; i < 48 && fl == -1; i++)
    {
        if (ecm[i].dat.uc8[0] == 0)
        {
            fl = i;
        }
    }
    
    return fl;
}

void EffectZSort()
{
    int i;
    int j;
    int num;
    sceVu0IVECTOR ivec;
    sceVu0FVECTOR vpos;
    sceVu0FVECTOR vtw;
    sceVu0FVECTOR wpos;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    EFFECT_CONT *ecm;
    EFFECT_CONT *ec;
    static int efzsort[48];
    float *v1;
    
    memset(wpos, 0, sizeof(sceVu0FVECTOR));
    
    wpos[3] = 1.0;
    
    if (change_efbank != 0)
    {
        ecm = efcntm;
    }
    else
    {
        ecm = efcntm_cnt;
    }
    
    for (i = 0, num = 0; i < 48; i++)
    {
        if (ecm[i].dat.uc8[0] != 0)
        {
            if ((ecm[i].dat.uc8[1] & 0x80) != 0)
            {
                v1 = ecm[i].pnt[0];
                
                // inlined from ../../graphics/graph3d/libsg.h
                Vu0CopyVector(vpos, v1);
                // end of inlined section
                
                sceVu0UnitMatrix(wlm);
                
                wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;
                
                sceVu0TransMatrix(wlm, wlm, vpos);
                sceVu0MulMatrix(slm, SgWSMtx, wlm);
                sceVu0RotTransPers(ivec, slm, wpos, 0);
                
                ecm[i].z = ivec[2];
            }
            else
            {
                ecm[i].z = 0;
            }
            
            efzsort[num] = i;
            num++;
        }
    }
    
    for (j = 0; j < num - 1; j++)
    {
        
        for (i = j + 1; i < num; i++)
        {
            if (ecm[efzsort[i]].z < ecm[efzsort[j]].z)
            {
                int tmp = efzsort[i];
                efzsort[i] = efzsort[j];
                efzsort[j] = tmp;
            }
        }
    }
    
    for (i = 0; i < num; i++)
    {
        ec = &ecm[efzsort[i]];

        switch (ec->dat.uc8[0])
        {
            case 23:
                SetFire(ec);
            break;
            case 24:
                SetFire2(ec);
            break;
            case 20:
                SetHalo(ec);
            break;
            case 21:
            case 22:
                SetRipple(ec);
            break;
            case 27:
            case 39:
                SetPartsDeform(ec);
            break;
            case 30:
                SetWaterdrop(ec);
            break;
            case 31:
                SetSunshine(ec);
            break;
            case 25:
                SetTorch(ec);
            break;
            case 26:
                SetSmoke(ec);
            break;
            case 33:
                SetEneFace(ec);
            break;
            case 34:
                SetFaceSpirit(ec);
            break;
        }
    }
    
    for (i = 0; i < num; i++)
    {
        ec = &ecm[efzsort[i]];

        switch (ec->dat.uc8[0])
        {
        case 29:
            SetDust(ec);
        break;
        }
    }

    SubRDFire3();
    RunRipple2();
}

void EffectZSort2()
{
    int i;
    int j;
    int num;
    sceVu0IVECTOR ivec;
    sceVu0FVECTOR vpos;
    sceVu0FVECTOR vtw;
    sceVu0FVECTOR wpos;
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    EFFECT_CONT *ecm;
    EFFECT_CONT *ec;
    static int efzsort[48];
    float *v1;
    
    memset(wpos, 0, sizeof(sceVu0FVECTOR));
    
    wpos[3] = 1.0;
    
    if (change_efbank != 0)
    {
        ecm = efcntm;
    }
    else
    {
        ecm = efcntm_cnt;
    }
    
    for (i = 0, num = 0; i < 48; i++)
    {
        if (ecm[i].dat.uc8[0] != 0)
        {
            if ((ecm[i].dat.uc8[1] & 0x80) != 0)
            {
                v1 = ecm[i].pnt[0];
                
                // inlined from ../../graphics/graph3d/libsg.h
                Vu0CopyVector(vpos, v1);
                // end of inlined section
                
                sceVu0UnitMatrix(wlm);
                
                wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;
                
                sceVu0TransMatrix(wlm, wlm, vpos);
                sceVu0MulMatrix(slm, SgWSMtx, wlm);
                sceVu0RotTransPers(ivec, slm, wpos, 0);
                
                ecm[i].z = ivec[2];
            }
            else
            {
                ecm[i].z = 0;
            }
            
            efzsort[num] = i;
            num++;
        }
    }
    
    for (j = 0; j < num - 1; j++)
    {
        
        for (i = j + 1; i < num; i++)
        {
            if (ecm[efzsort[i]].z < ecm[efzsort[j]].z)
            {
                int tmp = efzsort[i];
                efzsort[i] = efzsort[j];
                efzsort[j] = tmp;
            }
        }
    }
    
    for (i = 0; i < num; i++)
    {
        ec = &ecm[efzsort[i]];

        switch (ec->dat.uc8[0])
        {
        case 40:
            SetEneSeal(ec);
        break;
        case 28:
            SetEneFire(ec);
        break;
        }
    }
}

void EffectZSort3()
{
    int i;
    int j;
    int num;
    sceVu0IVECTOR ivec;
    sceVu0FVECTOR vpos;
    sceVu0FVECTOR vtw;
    sceVu0FVECTOR wpos;
    sceVu0FMATRIX wlm; // world local matrix ?
    sceVu0FMATRIX slm; // screen local matrix ?
    EFFECT_CONT *ecm;
    EFFECT_CONT *ec;
    static int efzsort[48];
    float *v1;
    
    memset(wpos, 0, sizeof(sceVu0FVECTOR));
    
    wpos[3] = 1.0;
    
    if (change_efbank != 0)
    {
        ecm = efcntm;
    }
    else
    {
        ecm = efcntm_cnt;
    }
    
    for (i = 0, num = 0; i < 48; i++)
    {
        if (ecm[i].dat.uc8[0] != 0)
        {
            if ((ecm[i].dat.uc8[1] & 0x80) != 0)
            {
                v1 = ecm[i].pnt[0];
                
                // inlined from ../../graphics/graph3d/libsg.h
                Vu0CopyVector(vpos, v1);
                // end of inlined section
                
                sceVu0UnitMatrix(wlm);
                
                wlm[0][0] = wlm[1][1] = wlm[2][2] = 25.0f;
                
                sceVu0TransMatrix(wlm, wlm, vpos);
                sceVu0MulMatrix(slm, SgWSMtx, wlm);
                sceVu0RotTransPers(ivec, slm, wpos, 0);
                
                ecm[i].z = ivec[2];
            }
            else
            {
                ecm[i].z = 0;
            }
            
            efzsort[num] = i;
            num++;
        }
    }
    
    for (j = 0; j < num - 1; j++)
    {
        
        for (i = j + 1; i < num; i++)
        {
            if (ecm[efzsort[i]].z < ecm[efzsort[j]].z)
            {
                int tmp = efzsort[i];
                efzsort[i] = efzsort[j];
                efzsort[j] = tmp;
            }
        }
    }
    
    for (i = 0; i < num; i++)
    {
        ec = &ecm[efzsort[i]];
        switch (ec->dat.uc8[0])
        {
            case 32:
                SetNegaCircle(ec);
            break;
        }
    }
}

int CheckEffectScrBuffer(int eno)
{
    int ret;
    
    if (set_buffer[0] == eno)
    {
        return 0;
    }
    
    if ((set_buffer[0] & 0xf) == 0)
    {
        set_buffer[0] = eno;
        return 0;
    }
    
    return -1;
}

void ResetEffectScrBuffer(int eno)
{
    if ((set_buffer[0] & eno) != 0)
    {
        set_buffer[0] = 0;
    }
    else if ((set_buffer[1] & eno) != 0)
    {
        set_buffer[1] = 0;
    }
}

void EffectControl(int no)
{
    static int fl;
    static void *ecw[3];
    EFFECT_CONT *ecm;
    int n;

    if (change_efbank != 0)
    {
        ecm = efcnt;
    }
    else
    {
        ecm = efcnt_cnt;
    }
    
    switch (no)
    {
    case 5:
        if (dbg_wrk.oth_pkt_num_sw != 0)
        {
            if (*key_now[8] != 0)
            {
                for (n = 0; n < 64; n++)
                {
                    SetString2(16, ((n / 16) * 80) + 10, ((n - ((n / 16) * 16)) * 16) + 20, 1, 0x80, 0x80, 0x80, "%2d:%2d", n, efcnt_cnt[n].dat.uc8[0]);
                }
                
                for (n = 0; n < 0x30; n++)
                {
                    SetString2(0x10, ((n / 16) * 80) + 0x168, ((n - ((n / 16) * 16)) * 16) + 20, 1, 0x80, 0x80, 0x80, "%2d:%2d", n, efcntm_cnt[n].dat.uc8[0]);
                }
            }
            else
            {
                for (n = 0; n < 64; n++)
                {
                    SetString2(0x10, ((n / 16) * 80) + 0xA, ((n - ((n / 16) * 16)) * 16) + 20, 1, 0x80, 0x80, 0x80, "%2d:%2d", n, efcnt[n].dat.uc8[0]);
                }
                for (n = 0; n < 0x30; n++)
                {
                    SetString2(0x10, ((n / 16) * 80) + 0x168, ((n - ((n / 16) * 16)) * 16) + 20, 1, 0x80, 0x80, 0x80, "%2d:%2d", n, efcntm[n].dat.uc8[0]);
                }
            }
        }
        
        pblur();
        SetSky();
        SetPond();
        SetHaze_Pond();
        SetFirefly();
        SetCanal();
        SetSaveCameraLamp();
        
        if (
            sys_wrk.game_mode != GAME_MODE_OUTGAME &&
            tmp_effect_off == 0 &&
            ingame_wrk.mode != 15 &&
            ingame_wrk.game != 1
        )
        {
            CheckItemEffect();
        }

        RunLeaf();
        SetHaze_Pond();
        SetFirefly();
        DrawRareEne();
        tes_p10();

        if (ecm[10].dat.uc8[0] == 10)
        {
            SetRenzFlare(&ecm[10]);
        }
        
        EffectZSort();
        
        if (ecm[19].dat.uc8[0] == 19)
        {
            SetSpirit(&ecm[19]);
        }
        
        SetMAGATOKI2();
        
        if (ecm[16].dat.uc8[0] == 16)
        {
            SetMAGATOKI(&ecm[16]);
        }
        
        if (ecm[1].dat.uc8[0] == 1)
        {
            SetForcusDepth(&ecm[1]);
        }
        
        if (ecm[36].dat.uc8[0] == 36)
        {
            SetForcusDepth2(&ecm[36]);
        }
        
        if (ecm[3].dat.uc8[0] == 3)
        {
            SetBlur(&ecm[3]);
        }
        
        if (ecm[4].dat.uc8[0] == 4)
        {
            SetBlur(&ecm[4]);
        }
        
        if (ecm[5].dat.uc8[0] == 5)
        {
            SetBlur(&ecm[5]);
        }
        
        RunBlur(&ecm[3]);
        
        if (ecm[6].dat.uc8[0] == 6)
        {
            SetDeform(&ecm[6]);
        }
        
        if (ecm[13].dat.uc8[0] == 13)
        {
            SetContrast2(&ecm[13]);
        }
        
        if (ecm[12].dat.uc8[0] == 12)
        {
            SetNega(&ecm[12]);
        }
        
        if (ecm[15].dat.uc8[0] == 15)
        {
            SetContrast3(&ecm[15]);
        }
        
        if (ecm[7].dat.uc8[0] == 7)
        {
            SetFocus(&ecm[7]);
        }
        
        RunFocus(&ecm[7]);
        
        if (ecm[2].dat.uc8[0] == 2)
        {
            SetDither3(&ecm[2]);
        }
        
        if (ecm[35].dat.uc8[0] == 35)
        {
            SetDither4(&ecm[35]);
        }
        
        if (ecm[11].dat.uc8[0] == 11)
        {
            SetBlackFilter(&ecm[11]);
        }
        
        if (ecm[14].dat.uc8[0] == 14)
        {
            SetContrast2(&ecm[14]);
        }
        
        tes_p11();
        SetRoomDirecPazzEne();
        break;
    case 7:
        SetEneDmgEffect1_Sub();
        SetEneDmgEffect2_Sub();
        RunCamStop();
        RunCamSlow();
        RunCamPush();
        tes_p20();
        EffectZSort2();
        RunCamSearch();
        RunCamView();
        tes_p21();
        EffectZSort3();
        return;
    case 8:
        tes_p3();
        
        if (ecm[9].dat.uc8[0] == 9)
        {
            SetFadeFrame(&ecm[9]);
        }
        
        if (ecm[8].dat.uc8[0] == 8)
        {
            SetOverRap(&ecm[8]);
        }
        
        ScreenCtrl();
        CamSave();
    return;
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:

        return;
    }
    return;
}

static int set_buffer[2];

void SetBlurOff()
{
    eff_blur_off = 1;
}

void SetDebugMenuSwitch(int sw)
{
    look_debugmenu = sw % 2;
}

void pblur()
{
    sceVu0FVECTOR vpos;
    SPRITE_DATA sd;
    DRAW_ENV de;
    float fy;
    float ss1;
    float ss2;
    float xx;
    float yy;
    float zz;
    float l;
    float ll;
    static float d1 = 0.0f;
    
    if (dbg_wrk.eff_prtblr_sw != 0)
    {
        LocalCopyBtoL(1, 0x1a40);

        sd = (SPRITE_DATA){
              .g_GsTex0 = {
                    .TBP0 = 0x1A40,
                    .TBW = 0xA,
                    .PSM = 0x0,
                    .TW = 0xA,
                    .TH = 0x8,
                    .TCC = 0x0,
                    .TFX = 0x0,
                    .CBP = 0x0,
                    .CPSM = 0x0,
                    .CSM = 0x0,
                    .CSA = 0x0,
                    .CLD = 0x1,
              },
              .g_nTexSizeW = 0x280,
              .g_nTexSizeH = 0xE0,
              .g_bMipmapLv = 0x0,
              .g_GsMiptbp1 = 0,  
              .g_GsMiptbp2 = 0, 
              .pos_x = -321.5f,
              .pos_y = -224.5f,
              .pos_z = 0,
              .size_w = 640.0f,
              .size_h = 448.0f,
              .scale_w = 1.0f,
              .scale_h = 1.0f,
              .clamp_u = 0,
              .clamp_v = 0,
              .rot_center = 0,
              .angle = 0.0f,
              .r = 0x80,
              .g = 0x80,
              .b = 0x80,
              .alpha = 0x80,
              .mask = 0
        };

        de = (DRAW_ENV){
            .tex1 = 0x161,
            .alpha = 0x44,
            .zbuf = 0x10100008c,
            .test = 0x30003,
            .clamp = 0,
            .prim = 0x50ab400000008001,
        };
        
        fy = 0.0f;
        ll = 2.0f;
        
        // inlined from ../../graphics/graph3d/libsg.h
        Vu0CopyVector(vpos, plyr_wrk.move_box.pos);
        // end of inlined section

        xx = (camera.p[0] - vpos[0]) * (camera.p[0] - vpos[0]);
        yy = (camera.p[1] - vpos[1]) * (camera.p[1] - vpos[1]);
        zz = (camera.p[2] - vpos[2]) * (camera.p[2] - vpos[2]);
        l = SgSqrtf(xx + yy + zz);
        
        if (l != fy)
        {
            ll = 1000.0f / l;
        }
        
        ll = ll < 1.0f ? 1.0f : ll;
        
        ss1 = SgSinf(((d1 + d1) / 180.0f) * 3.1415927f) * ll;
        ss2 = SgSinf(((d1 * 3.0f) / 180.0f) * 3.1415927f) * ll;
        
        if (
            (ss1 < 0.4f && ss1 > -0.4f) && 
            (ss2 < 0.4f && ss2 > -0.4f)
        )
        {
            d1 += 0.08f;
        }
        else
        {
            d1 += 0.2f;
        }
        
        sd.pos_x = -320.5f + ss1;
        sd.pos_y = -224.5f + ss2;
        sd.pos_z = 0;
        sd.alpha = dbg_wrk.eff_prtblr_alp;
        
        SetTexDirectS2(0, &sd, &de, 0);
        SetPanel(0xffff800, 0.0f, 0.0f, 640.0f, 448.0f, 0, 0, 0, 0x80);
        LocalCopyLtoB2(1, ((sys_wrk.count + 1 & 1) * 0x23 << 0x26) >> 0x20);
    }
}

void TmpEffectOff(int id)
{
    if (id != 0)
    {
        tmp_effect_off = 1;
    }
    else
    {
        tmp_effect_off = 0;
    }
}

void tes_p10()
{
    return;
}

void tes_p11()
{
    return;
}

void tes_p20()
{
    static void *ene_fire_work = NULL;
    sceVu0FVECTOR fire_pos;
    sceVu0FVECTOR fire_pos2;
    float fx;
    float fy;
    float fz;
    float l;
    float alp;
    static float off_x = 0.0f;
    static float off_z = 0.0f;
    
    if (dbg_wrk.eff_new_ene_ef_sw != 0)
    {
        if (ingame_wrk.mode != 10 && plyr_wrk.mode != 1)
        {
            alp = dbg_wrk.eff_new_ene_ef_alp;
            
            // inlined from ../../graphics/graph3d/libsg.h
            Vu0CopyVector(fire_pos, plyr_wrk.move_box.pos);
            // end of inlined section
            
            fx = (fire_pos[0] - camera.p[0]) * (fire_pos[0] - camera.p[0]);
            fy = (fire_pos[1] - camera.p[1]) * (fire_pos[1] - camera.p[1]);
            fz = (fire_pos[2] - camera.p[2]) * (fire_pos[2] - camera.p[2]);
            l = SgSqrtf(fx + fy + fz);
            
            fire_pos[0] = camera.p[0] + ((fire_pos[0] - camera.p[0]) * (l - 200.0f)) / l;
            fire_pos[1] = camera.p[1] + ((fire_pos[1] - camera.p[1]) * (l - 200.0f)) / l;
            fire_pos[2] = camera.p[2] + ((fire_pos[2] - camera.p[2]) * (l - 200.0f)) / l;
            
            fire_pos[3] = 1.0f;
            
            // inlined from ../../graphics/graph3d/libsg.h
            Vu0CopyVector(fire_pos2, fire_pos);
            // end of inlined section
            
            fire_pos2[0] += off_x;
            fire_pos2[1] -= 700.0f;
            fire_pos2[2] += off_z;
            
            ene_fire_work = GetEnePartAddr(ene_fire_work, 11, 160);
            ene_fire_work = ContHeatHaze(ene_fire_work, 11, fire_pos, fire_pos2, 0, 128.0f, 128.0f, 128.0f, alp, dbg_wrk.eff_new_ene_ef_sz * 100.0f, 1.0f);
        }
    }
    else
    {
        if (ene_fire_work != NULL)
        {
            ene_fire_work = NULL;
        }
    }
}

int change_efbank = 0;
int eff_blur_off = 0;
int effect_disp_flg = 0;
int eff_filament_off = 0;
int now_buffer[2] = {0, 0};
int eff_dith_off = 0;
int stop_effects = 0;
int monochrome_mode = 0;
int magatoki_mode = 0;
int shibata_set_off = 0;
int look_debugmenu = 0;

void tes_p21()
{
    return;
}

void tes_p3()
{
    return;
}
