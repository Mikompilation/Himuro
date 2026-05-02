#include "common.h"
#include "typedefs.h"
#include "enums.h"

// gcc/src/newlib/libm/math/sf_sin.c
float sinf(float x);

// gcc/src/newlib/libm/math/wf_sqrt.c
float sqrtf(float x);

#define INCLUDING_FROM_EFFECT_C
#include "effect.h"
#undef INCLUDING_FROM_EFFECT_C

#include "gcc/stdarg.h"

#include "sce/libvu0.h"

#include "graphics/graph2d/effect_ene.h"
#include "graphics/graph2d/effect_obj.h"
#include "graphics/graph2d/effect_oth.h"
#include "graphics/graph2d/effect_rdr.h"
#include "graphics/graph2d/effect_scr.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/g2d_debug.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph2d/rare_ene.h"
#include "graphics/graph2d/sprt.h"
#include "graphics/graph3d/gra3d.h"
#include "graphics/graph3d/libsg.h"
#include "graphics/graph3d/sglib.h"
// #include "ingame/entry/ap_sgost.h" //
#include "ingame/event/ev_main.h"
#include "ingame/ig_glob.h"
#include "ingame/photo/pht_make.h"
#include "main/glob.h"

#include "data/effdat.h" // SPRT_DAT effdat[];
#include "data/camdat.h" // SPRT_DAT camdat[];
static int tmp_effect_off = 0;
static void *r23_e1 = 0;
static void *r23_e2 = 0;
static u_char r28_torch_flag = 0;

#define PI 3.1415927f

void InitEffects()
{
    eff_blur_off = 1;
    eff_dith_off = 0;
    look_debugmenu = 1;
    stop_effects = 0;
    change_efbank = 1;
    monochrome_mode = 0;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
    magatoki_mode = 0;
#endif
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

    memset(efcnt, 0, sizeof(efcnt));
    memset(efcnt_cnt, 0, sizeof(efcnt_cnt));
    memset(efcntm, 0, sizeof(efcntm));
    memset(efcntm_cnt, 0, sizeof(efcntm_cnt));

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
    static sceVu0FVECTOR canal1 = { 27200.0f, -2000.0f, 29500.0f, 1.0f };
    static sceVu0FVECTOR canal2 = { 28000.0f, -2000.0f, 29250.0f, 1.0f };
    static sceVu0FVECTOR torch_pos[3] = {
        {  7825.0f, 2600.0f, 35150.0f, 1.0f },
        {  7825.0f, 2575.0f, 38100.0f, 1.0f },
        { 11225.0f, 2600.0f, 35150.0f, 1.0f },
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
            SetEffects(EF_DITHER, 1, msbtset.dt.type, msbtset.dt.alp, msbtset.dt.spd, msbtset.dt.amax, msbtset.dt.cmax);
        }

        if (msbtset.bl.sw != 0)
        {
            SetEffects(EF_BLUR_N, 1, &msbtset.bl.alp, msbtset.bl.scl, msbtset.bl.rot, msbtset.bl.x, msbtset.bl.y);
        }

        if (msbtset.df.sw != 0)
        {
            SetEffects(EF_DEFORM, 1, msbtset.df.type, msbtset.df.rate);
        }

        if (msbtset.cn.sw != 0)
        {
            switch (msbtset.cn.type)
            {
                case 1:
                    SetEffects(EF_NCONTRAST, 1, msbtset.cn.col, msbtset.cn.alp);
                break;
                case 2:
                    SetEffects(EF_NCONTRAST2, 1, msbtset.cn.col, msbtset.cn.alp);
                break;
                case 3:
                    SetEffects(EF_NCONTRAST3, 1, msbtset.cn.col, msbtset.cn.alp);
                break;
            }
        }

        if (msbtset.ff.sw != 0)
        {
            SetEffects(EF_FADEFRAME, 1, msbtset.ff.alp, 0x80000);
        }

        if (msbtset.ng.sw != 0)
        {
            SetEffects(EF_NEGA, 1, msbtset.ng.col, msbtset.ng.alp, msbtset.ng.alp2);
        }

        if (msbtset.mn.sw == 0)
        {
            if (efcnt[EF_MONO].dat.uc8[0] == 0)
            {
                efcnt[EF_MONO].dat.uc8[0] = 1;

                ChangeMonochrome(0);
            }
        }

        if (msbtset.mn.sw != 0 && efcnt[EF_MONO].dat.uc8[0] == 0)
        {
            efcnt[EF_MONO].dat.uc8[0] = 1;

            ChangeMonochrome(1);
        }
    }

    for (i = 0; i < 3; i++)
    {
        if (fly_display[i] != 0)
        {
            if (e[i] == NULL)
            {
                e[i] = SetEffects(EF_FACESPIRIT, 2, fly_wrk[i].move_box.pos, 0x50, 0x50, 0x5c, &aalp, i);
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

    if (plyr_wrk.pr_info.room_no != 23)
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
            r23_e1 = SetEffects(EF_WATERDROP, 2, canal1, 1, 250.0f, 200, 0x80, 0x80, 0x80);
        }

        if (r23_e2 == NULL)
        {
            r23_e2 = SetEffects(EF_WATERDROP, 2, canal2, 4, 250.0f, 260, 0x80, 0x80, 0x80);
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

    if (ingame_wrk.stts & 0x20 && ev_wrk.movie_on != 4 && effect_disp_flg == 0)
    {
        return NULL;
    }

    switch(id)
    {
    case EF_NULL:
    case EF_Z_DEP:
    case EF_DITHER:
    case EF_BLUR_N:
    case EF_BLUR_B:
    case EF_BLUR_W:
    case EF_DEFORM:
    case EF_FOCUS:
    case EF_OVERLAP:
    case EF_FADEFRAME:
    case EF_RENZFLARE:
    case EF_BLACKFILTER:
    case EF_NEGA:
    case EF_NCONTRAST:
    case EF_NCONTRAST2:
    case EF_NCONTRAST3:
    case EF_MAGATOKI:
    case EF_SPIRIT:
    case EF_DITHER2:
    case EF_Z_DEP2:
    case EF_HAZE:
        ec = change_efbank != 0 ? &efcnt[id] : &efcnt_cnt[id];
    break;
    case EF_HALO:
    case EF_RIPPLE:
    case EF_RIPPLE2:
    case EF_FIRE:
    case EF_FIRE2:
    case EF_TORCH:
    case EF_SMOKE:
    case EF_PDEFORM:
    case EF_ENEFIRE:
    case EF_DUST:
    case EF_WATERDROP:
    case EF_SUNSHINE:
    case EF_NEGACIRCLE:
    case EF_ENEFACE:
    case EF_FACESPIRIT:
    case EF_PBLUR:
    case EF_ENEOUT:
        ret = SearchEmptyEffectBuf();

        if (ret == -1)
        {
            return NULL;
        }

        ec = change_efbank != 0 ? &efcntm[ret] : &efcntm_cnt[ret];
    break;
    default:
        return NULL;
    break;
    }

    va_start(ap, fl);

    switch(id) {
    case EF_NULL:
        // do nothing ...
    break;
    case EF_Z_DEP:
        ec->dat.uc8[0] = EF_Z_DEP;
        ec->dat.uc8[1] = fl;
    break;
    case EF_Z_DEP2:
        ec->dat.uc8[0] = EF_Z_DEP2;
        ec->dat.uc8[1] = fl;
    break;
    case EF_DITHER:
        ec->dat.uc8[0] = EF_DITHER;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.fl32[3] = va_arg(ap, double);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        ec->dat.uc8[4] = va_arg(ap, u_char);    

        if (fl & 0x4)
        {
            ec->in = va_arg(ap, u_int);
            ec->keep = va_arg(ap, u_int);
            ec->out = va_arg(ap, u_int);
            ec->cnt = 0;
            ec->flow = (ec->in == 0 ? (ec->keep == 0 ? ec->out != 0 ? 2 : 3 : 1) : 0);
        }
    break;
    case EF_DITHER2:
        ec->dat.uc8[0] = EF_DITHER2;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.fl32[1] = va_arg(ap, double);
        ec->dat.fl32[2] = va_arg(ap, double);
    break;
    case EF_BLUR_N:
        ec->dat.uc8[0] = EF_BLUR_N;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = 0;
        ec->pnt[0] = va_arg(ap, void *);
        ec->dat.ui32[2] = va_arg(ap, u_int);
        ec->dat.ui32[3] = va_arg(ap, u_int);
        ec->fw[0] = va_arg(ap, double);
        ec->fw[1] = va_arg(ap, double);
    break;
    case EF_BLUR_B:
        ec->dat.uc8[0] = EF_BLUR_B;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = 1;
        ec->pnt[0] = va_arg(ap, void *);
        ec->dat.ui32[2] = va_arg(ap, u_int);
        ec->dat.ui32[3] = va_arg(ap, u_int);
        ec->fw[0] = va_arg(ap, double);
        ec->fw[1] = va_arg(ap, double);
    break;
    case EF_BLUR_W:
        ec->dat.uc8[0] = EF_BLUR_W;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = 2;
        ec->pnt[0] = va_arg(ap, void *);
        ec->dat.ui32[2] = va_arg(ap, u_int);
        ec->dat.ui32[3] = va_arg(ap, u_int);
        ec->fw[0] = va_arg(ap, double);
        ec->fw[1] = va_arg(ap, double);
    break;
    case EF_DEFORM:
        ec->dat.uc8[0] = EF_DEFORM;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        ec->dat.uc8[4] = 0;

        if (fl & 0x4)
        {
            ec->flow = (ec->in == 0 ? (ec->keep == 0 ? ec->out != 0 ? 2 : 3 : 1) : 0);
            ec->cnt = 0;
            ec->in = va_arg(ap, u_int);
            ec->keep = va_arg(ap, u_int);
            ec->out = va_arg(ap, u_int);
        }
    break;
    case EF_FOCUS:
        ec->dat.uc8[0] = EF_FOCUS;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
    break;
    case EF_STEALTH:
        ec->dat.uc8[0] = EF_STEALTH;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = 0;
        ec->pnt[0] = va_arg(ap, void *);
    break;
    case EF_STEALTH2:
        ec->dat.uc8[0] = EF_STEALTH;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = 1;
        ec->pnt[0] = va_arg(ap, void *);
    break;
    case EF_007:
        ec->dat.uc8[0] = EF_STEALTH;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = 2;
        ec->pnt[0] = va_arg(ap, void *);
    break;
    case EF_MONO:
        ec->dat.uc8[0] = EF_MONO;
        ec->dat.uc8[1] = fl;
    break;
    case EF_SEPIA:
        ec->dat.uc8[0] = EF_SEPIA;
        ec->dat.uc8[1] = fl;
    break;
    case EF_OVERLAP:
        ec->dat.uc8[0] = EF_OVERLAP;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
    break;
    case EF_FADEFRAME:
        ec->dat.uc8[0] = EF_FADEFRAME;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.ui32[1] = va_arg(ap, u_int);
    break;
    case EF_RENZFLARE:
        ec->dat.uc8[0] = EF_RENZFLARE;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->pnt[0] = va_arg(ap, void *);
        ec->pnt[1] = va_arg(ap, void *);
    break;
    case EF_HAZE:
        ec->dat.uc8[0] = EF_HAZE;
        ec->dat.uc8[1] = fl;
    break;
    case EF_BLACKFILTER:
        ec->dat.uc8[0] = EF_BLACKFILTER;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
    break;
    case EF_NEGA:
        ec->dat.uc8[0] = EF_NEGA;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        
        if (fl & 0x4)
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
    case EF_NCONTRAST:
        ec->dat.uc8[0] = EF_NCONTRAST;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
    break;
    case EF_NCONTRAST2:
        ec->dat.uc8[0] = EF_NCONTRAST2;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
    break;
    case EF_NCONTRAST3:
        ec->dat.uc8[0] = EF_NCONTRAST3;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
    break;
    case EF_MAGATOKI:
        ec->dat.uc8[0] = EF_MAGATOKI;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = fl;
        ec->flow = 0;
        ec->in = va_arg(ap, u_int);
        ec->out = va_arg(ap, u_int);
        ec->pnt[0] = va_arg(ap, void *);
    break;
    case EF_ENEDMG1:
        ec->dat.uc8[0] = EF_ENEDMG1;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, char);
    break;
    case EF_ENEDMG2:
        ec->dat.uc8[0] = EF_ENEDMG2;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, char);
    break;
    case EF_SPIRIT:
        ec->dat.uc8[0] = EF_SPIRIT;
        ec->dat.uc8[1] = fl;
        ec->flow = va_arg(ap, char);
        ec->pnt[0] = va_arg(ap, void *);
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        ec->dat.uc8[4] = va_arg(ap, u_char);
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.uc8[5] = va_arg(ap, u_char);
        ec->dat.uc8[6] = va_arg(ap, u_char);
        ec->dat.uc8[7] = va_arg(ap, u_char);
        ec->dat.fl32[3] = va_arg(ap, double);
        ec->pnt[1] = va_arg(ap, void *);
        ec->cnt = 0;
        ec->max = 0;
        ec->keep = 0;
    break;
    case EF_PBLUR:
        ec->dat.uc8[0] = EF_PBLUR;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
    break;
    case EF_LUMINE:
        ec->dat.uc8[0] = EF_LUMINE;
        ec->dat.uc8[1] = fl;
        ec->pnt[0] = va_arg(ap, void *);
    break;
    case EF_FIRE:
        ec->dat.uc8[0] = EF_FIRE;
        ec->dat.uc8[1] = fl | 0x80;
        ec->flow = va_arg(ap, char);
        ec->pnt[0] = va_arg(ap, void *);
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        ec->dat.uc8[4] = va_arg(ap, u_char);
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.uc8[5] = va_arg(ap, u_char);
        ec->dat.uc8[6] = va_arg(ap, u_char);
        ec->dat.uc8[7] = va_arg(ap, u_char);
        ec->dat.fl32[3] = va_arg(ap, double);   

        if (ec->flow == 3)
        {
            ec->cnt = 21.0f * VER_RAND();
        }
        else
        {
            ec->cnt = 0;
        }
    break;
    case EF_FIRE2:
        ec->dat.uc8[0] = EF_FIRE2;
        ec->dat.uc8[1] = fl | 0x80;
        ec->flow = va_arg(ap, char);
        ec->pnt[0] = va_arg(ap, void *);
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        ec->dat.uc8[4] = va_arg(ap, u_char);
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.uc8[5] = va_arg(ap, u_char);
        ec->dat.uc8[6] = va_arg(ap, u_char);
        ec->dat.uc8[7] = va_arg(ap, u_char);
        ec->dat.fl32[3] = va_arg(ap, double);
        ec->fw[0] = va_arg(ap, double);
        
        if (ec->flow == 3)
        {
            ec->cnt = 21.0f * VER_RAND();
        } 
        else
        {
            ec->cnt = 0;
        }
    break;
    case EF_HALO:
        ec->dat.uc8[0] = EF_HALO;
        ec->dat.uc8[1] = fl | 0x80;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->pnt[0] = va_arg(ap, void *);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        ec->dat.uc8[4] = va_arg(ap, u_char);
        ec->dat.uc8[5] = va_arg(ap, u_char);
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.uc8[6] = va_arg(ap, u_char);
    break;
    case EF_RIPPLE:
        ec->dat.uc8[0] = EF_RIPPLE;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        ec->dat.uc8[4] = 0;
        ec->dat.uc8[7] = 0;
        ec->pnt[0] = va_arg(ap, void *);
    break;
    case EF_RIPPLE2:
        ec->dat.uc8[0] = EF_RIPPLE2;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        ec->cnt = 0;
        ec->dat.uc8[4] = va_arg(ap, u_char);
        ec->dat.uc8[5] = va_arg(ap, u_char);
        ec->dat.uc8[6] = va_arg(ap, u_char);
        ec->dat.uc8[7] = 0;
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.fl32[3] = va_arg(ap, double);
        ec->pnt[0] = va_arg(ap, void *);
        ec->pnt[1] = va_arg(ap, void *);

        if (fl & 0x8)
        {
            ec->dat.uc8[7] = va_arg(ap, u_char);
        }
    break;
    case EF_PDEFORM:
        ec->dat.uc8[0] = EF_PDEFORM;
        ec->dat.uc8[1] = fl | 0x80;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->max = va_arg(ap, u_int);
        ec->dat.uc8[4] = 0xff;
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.fl32[3] = va_arg(ap, double);
        ec->pnt[0] = va_arg(ap, void *);
        ec->fw[0] = 0.0f;
        ec->cnt = 0;
        ec->in = va_arg(ap, u_int);
        ec->keep = va_arg(ap, u_int);
        ec->out = va_arg(ap, u_int);
        ec->pnt[1] = va_arg(ap, void *);
        ec->pnt[2] = va_arg(ap, void *);
        ec->pnt[4] = va_arg(ap, void *);
        ec->pnt[5] = NULL;
        ec->pnt[5] = va_arg(ap, void *);
        
        if (fl & 0x4)
        {
            ec->flow = (ec->in == 0 ? (ec->keep == 0 ? ec->out != 0 ? 2 : 3 : 1) : 0);
        }
    break;
    case EF_ENEIN:
        ec->dat.uc8[0] = EF_ENEIN;
        ec->dat.uc8[1] = fl | 0x80;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        ec->dat.uc8[4] = 0xff;
        ec->dat.fl32[3] = va_arg(ap, double);
        ec->pnt[0] = va_arg(ap, void *);
        ec->cnt = 0;
        ec->in = va_arg(ap, u_int);
        ec->keep = va_arg(ap, u_int);
        ec->out = va_arg(ap, u_int);
        ec->max = 100;

        if (fl & 0x4)
        {
            ec->flow = (ec->in == 0 ? (ec->keep == 0 ? ec->out != 0 ? 2 : 3 : 1) : 0);
        }
    break;
    case EF_ENEOUT:
        ec->dat.uc8[0] = EF_ENEOUT;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = 0;
        ec->dat.uc8[3] = 0;
        ec->dat.uc8[4] = 0;
        ec->dat.uc8[6] = va_arg(ap, u_char);
        ec->dat.uc8[5] = va_arg(ap, u_char);
        ec->dat.uc8[7] = 0;
        ec->fw[0] = 2.0f;
        ec->fw[1] = 0.0f;
        ec->fw[2] = va_arg(ap, double);
        ec->pnt[0] = NULL;
    break;
    case EF_DUST:
        ec->dat.uc8[0] = EF_DUST;
        ec->dat.uc8[1] = fl | 0x80;
        ec->dat.uc8[2] = 1;
        ec->pnt[0] = va_arg(ap, void *);
    break;
    case EF_WATERDROP:
        ec->dat.uc8[0] = EF_WATERDROP;
        ec->dat.uc8[1] = fl | 0x80;
        ec->pnt[0] = va_arg(ap, void *);
        ec->dat.uc8[5] = va_arg(ap, u_char);
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.ui32[3] = 0;
        ec->max = va_arg(ap, u_int);
        ec->cnt = ec->max;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        ec->dat.uc8[4] = va_arg(ap, u_char);
        ec->dat.uc8[6] = 0;
    break;
    case EF_SUNSHINE:
        ec->dat.uc8[0] = EF_SUNSHINE;
        ec->dat.uc8[1] = fl;
        ec->pnt[0] = va_arg(ap, void *);
        ec->pnt[1] = va_arg(ap, void *);
        ec->pnt[2] = va_arg(ap, void *);
        ec->max = va_arg(ap, u_int);
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.fl32[3] = va_arg(ap, double);
        ec->cnt = 0;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        ec->dat.uc8[4] = va_arg(ap, u_char);
    break;
    case EF_ENEFIRE:
        ec->dat.uc8[0] = EF_ENEFIRE;
        ec->dat.uc8[1] = fl | 0x80;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->pnt[0] = va_arg(ap, void *);
        ec->pnt[4] = va_arg(ap, void *);
        ec->pnt[1] = va_arg(ap, void *);
        ec->pnt[2] = va_arg(ap, void *);
        ec->pnt[3] = NULL;
        ec->dat.ui32[3] = va_arg(ap, u_int);
        ec->pnt[5] = va_arg(ap, void *);
    break;
    case EF_TORCH:
        ec->dat.uc8[0] = EF_TORCH;
        ec->dat.uc8[1] = fl | 0x80;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = 0;
        ec->pnt[0] = va_arg(ap, void *);
        ec->pnt[1] = NULL;
        ec->pnt[2] = va_arg(ap, void *);
        ec->pnt[3] = va_arg(ap, void *);
    break;
    case EF_SMOKE:
        ec->dat.uc8[0] = EF_SMOKE;
        ec->dat.uc8[1] = fl | 0x80;
        ec->pnt[0] = va_arg(ap, void *);
        ec->pnt[1] = NULL;
    break;
    case EF_NEGACIRCLE:
        ec->dat.uc8[0] = EF_NEGACIRCLE;
        ec->dat.uc8[1] = fl;
        ec->dat.fl32[1] = va_arg(ap, double);
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.fl32[3] = va_arg(ap, double);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        ec->in = va_arg(ap, u_int);
        ec->keep = va_arg(ap, u_int);
        ec->out = va_arg(ap, u_int);
    break;
    case EF_ENEFACE:
        ec->dat.uc8[0] = EF_ENEFACE;
        ec->dat.uc8[1] = fl;
        ec->dat.uc8[2] = va_arg(ap, u_char);
        ec->dat.uc8[3] = va_arg(ap, u_char);
        ec->dat.fl32[1] = va_arg(ap, double);
        ec->dat.fl32[2] = va_arg(ap, double);
        ec->dat.fl32[3] = va_arg(ap, double);
        ec->cnt = 0;
        ec->out = 0;
    break;
    case EF_FACESPIRIT:
        ec->dat.uc8[0] = EF_FACESPIRIT;
        ec->dat.uc8[1] = fl;
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
    sceVu0FVECTOR wpos = { 0.0f, 0.0f, 0.0f, 1.0f };
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    EFFECT_CONT *ecm;
    EFFECT_CONT *ec;
    static int efzsort[48];

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
            if (ecm[i].dat.uc8[1] & 0x80)
            {
                Vu0CopyVector(vpos, ecm[i].pnt[0]);

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
        case EF_FIRE:
            SetFire(ec);
        break;
        case EF_FIRE2:
            SetFire2(ec);
        break;
        case EF_HALO:
            SetHalo(ec);
        break;
        case EF_RIPPLE:
        case EF_RIPPLE2:
            SetRipple(ec);
        break;
        case EF_PDEFORM:
        case EF_ENEIN:
            SetPartsDeform(ec);
        break;
        case EF_WATERDROP:
            SetWaterdrop(ec);
        break;
        case EF_SUNSHINE:
            SetSunshine(ec);
        break;
        case EF_TORCH:
            SetTorch(ec);
        break;
        case EF_SMOKE:
            SetSmoke(ec);
        break;
        case EF_ENEFACE:
            SetEneFace(ec);
        break;
        case EF_FACESPIRIT:
            SetFaceSpirit(ec);
        break;
        }
    }

    for (i = 0; i < num; i++)
    {
        ec = &ecm[efzsort[i]];

        switch (ec->dat.uc8[0])
        {
        case EF_DUST:
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
    sceVu0FVECTOR wpos = { 0.0f, 0.0f, 0.0f, 1.0f };
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    EFFECT_CONT *ecm;
    EFFECT_CONT *ec;
    static int efzsort[48];

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
            if (ecm[i].dat.uc8[1] & 0x80)
            {
                Vu0CopyVector(vpos, ecm[i].pnt[0]);

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
        case EF_ENEOUT:
            SetEneSeal(ec);
        break;
        case EF_ENEFIRE:
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
    sceVu0FVECTOR wpos = { 0.0f, 0.0f, 0.0f, 1.0f };
    sceVu0FMATRIX wlm;
    sceVu0FMATRIX slm;
    EFFECT_CONT *ecm;
    EFFECT_CONT *ec;
    static int efzsort[48];

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
            if (ecm[i].dat.uc8[1] & 0x80)
            {
                Vu0CopyVector(vpos, ecm[i].pnt[0]);

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
            case EF_NEGACIRCLE:
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
    if (set_buffer[0] & eno)
    {
        set_buffer[0] = 0;
    }
    else if (set_buffer[1] & eno)
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

    ecm = change_efbank != 0 ? efcnt: efcnt_cnt;
    
    switch (no)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
        // do nothing ...
    break;
    case 5:
        if (dbg_wrk.oth_pkt_num_sw != 0)
        {
            if (PAD_BTN_HELD(PAD_L1))
            {
                for (n = 0; n < 64; n++)
                {
                    SetString2(0x10, (int)(n / 16) * 80 + 10, (n % 16) * 16 + 20, 1, 0x80, 0x80, 0x80, "%2d:%2d", n, efcnt_cnt[n].dat.uc8[0]);
                }

                for (n = 0; n < 48; n++)
                {
                    SetString2(0x10, (int)(n / 16) * 80 + 360, (n % 16) * 16 + 20, 1, 0x80, 0x80, 0x80, "%2d:%2d", n, efcntm_cnt[n].dat.uc8[0]);
                }
            }
            else
            {
                for (n = 0; n < 64; n++)
                {
                    SetString2(0x10, (int)(n / 16) * 80 + 10, (n % 16) * 16 + 20, 1, 0x80, 0x80, 0x80, "%2d:%2d", n, efcnt[n].dat.uc8[0]);
                }

                for (n = 0; n < 48; n++)
                {
                    SetString2(0x10, (int)(n / 16) * 80 + 360, (n % 16) * 16 + 20, 1, 0x80, 0x80, 0x80, "%2d:%2d", n, efcntm[n].dat.uc8[0]);
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
            ingame_wrk.mode != INGAME_MODE_WANDER_SOUL &&
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

        if (ecm[EF_RENZFLARE].dat.uc8[0] == EF_RENZFLARE)
        {
            SetRenzFlare(&ecm[EF_RENZFLARE]);
        }
        
        EffectZSort();
        
        if (ecm[EF_SPIRIT].dat.uc8[0] == EF_SPIRIT)
        {
            SetSpirit(&ecm[EF_SPIRIT]);
        }

#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
        SetMAGATOKI2();
#endif

        if (ecm[EF_MAGATOKI].dat.uc8[0] == EF_MAGATOKI)
        {
            SetMAGATOKI(&ecm[EF_MAGATOKI]);
        }
        
        if (ecm[EF_Z_DEP].dat.uc8[0] == EF_Z_DEP)
        {
            SetForcusDepth(&ecm[EF_Z_DEP]);
        }
        
        if (ecm[EF_Z_DEP2].dat.uc8[0] == EF_Z_DEP2)
        {
            SetForcusDepth2(&ecm[EF_Z_DEP2]);
        }
        
        if (ecm[EF_BLUR_N].dat.uc8[0] == EF_BLUR_N)
        {
            SetBlur(&ecm[EF_BLUR_N]);
        }
        
        if (ecm[EF_BLUR_B].dat.uc8[0] == EF_BLUR_B)
        {
            SetBlur(&ecm[EF_BLUR_B]);
        }
        
        if (ecm[EF_BLUR_W].dat.uc8[0] == EF_BLUR_W)
        {
            SetBlur(&ecm[EF_BLUR_W]);
        }
        
        RunBlur(&ecm[EF_BLUR_N]);
        
        if (ecm[EF_DEFORM].dat.uc8[0] == EF_DEFORM)
        {
            SetDeform(&ecm[EF_DEFORM]);
        }
        
        if (ecm[EF_NCONTRAST].dat.uc8[0] == EF_NCONTRAST)
        {
            SetContrast2(&ecm[EF_NCONTRAST]);
        }
        
        if (ecm[EF_NEGA].dat.uc8[0] == EF_NEGA)
        {
            SetNega(&ecm[EF_NEGA]);
        }
        
        if (ecm[EF_NCONTRAST3].dat.uc8[0] == EF_NCONTRAST3)
        {
            SetContrast3(&ecm[EF_NCONTRAST3]);
        }
        
        if (ecm[EF_FOCUS].dat.uc8[0] == EF_FOCUS)
        {
            SetFocus(&ecm[EF_FOCUS]);
        }
        
        RunFocus(&ecm[EF_FOCUS]);
        
        if (ecm[EF_DITHER].dat.uc8[0] == EF_DITHER)
        {
            SetDither3(&ecm[EF_DITHER]);
        }
        
        if (ecm[EF_DITHER2].dat.uc8[0] == EF_DITHER2)
        {
            SetDither4(&ecm[EF_DITHER2]);
        }
        
        if (ecm[EF_BLACKFILTER].dat.uc8[0] == EF_BLACKFILTER)
        {
            SetBlackFilter(&ecm[EF_BLACKFILTER]);
        }
        
        if (ecm[EF_NCONTRAST2].dat.uc8[0] == EF_NCONTRAST2)
        {
            SetContrast2(&ecm[EF_NCONTRAST2]);
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
    break;
    case 8:
        tes_p3();
        
        if (ecm[EF_FADEFRAME].dat.uc8[0] == EF_FADEFRAME)
        {
            SetFadeFrame(&ecm[EF_FADEFRAME]);
        }
        
        if (ecm[EF_OVERLAP].dat.uc8[0] == EF_OVERLAP)
        {
            SetOverRap(&ecm[EF_OVERLAP]);
        }
        
        ScreenCtrl();

        CamSave();
    break;
    }
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
    
    if (dbg_wrk.eff_prtblr_sw == 0)
    {
        return;
    }

    LocalCopyBtoL(1, 0x1a40);

    sd = (SPRITE_DATA){
          .g_GsTex0 = {
                .TBP0 = 0x1a40,
                .TBW = 10,
                .PSM = 0,
                .TW = 10,
                .TH = 8,
                .TCC = 0,
                .TFX = 0,
                .CBP = 0,
                .CPSM = 0,
                .CSM = 0,
                .CSA = 0,
                .CLD = 1,
          },
          .g_nTexSizeW = 640,
          .g_nTexSizeH = 224,
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
        .tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0),
        .alpha = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0),
        .zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1),
        .test = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_ALWAYS, 0, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_ALWAYS),
        .clamp = SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0),
        .prim = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 5),
    };

    fy = 0.0f;
    ll = 2.0f;

    Vu0CopyVector(vpos, plyr_wrk.move_box.pos);

    xx = (camera.p[0] - vpos[0]) * (camera.p[0] - vpos[0]);
    yy = (camera.p[1] - vpos[1]) * (camera.p[1] - vpos[1]);
    zz = (camera.p[2] - vpos[2]) * (camera.p[2] - vpos[2]);

    l = VER_SQRTF(xx + yy + zz);
    
    if (l != fy)
    {
        ll = 1000.0f / l;
    }
    
    ll = ll < 1.0f ? 1.0f : ll;
    
    ss1 = VER_SINF((d1 + d1) / 180.0f * PI) * ll;
    ss2 = VER_SINF(((d1 * 3.0f) / 180.0f) * PI) * ll;
    
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

    LocalCopyLtoB2(1, ((sys_wrk.count + 1) & 1) * 0x8c0);
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

            Vu0CopyVector(fire_pos, plyr_wrk.move_box.pos);
            
            fx = (fire_pos[0] - camera.p[0]);
            fy = (fire_pos[1] - camera.p[1]);
            fz = (fire_pos[2] - camera.p[2]);

            l = VER_SQRTF(fx * fx + fy * fy + fz * fz);
            
            fire_pos[0] = camera.p[0] + ((fire_pos[0] - camera.p[0]) * (l - 200.0f)) / l;
            fire_pos[1] = camera.p[1] + ((fire_pos[1] - camera.p[1]) * (l - 200.0f)) / l;
            fire_pos[2] = camera.p[2] + ((fire_pos[2] - camera.p[2]) * (l - 200.0f)) / l;

            fire_pos[3] = 1.0f;

            Vu0CopyVector(fire_pos2, fire_pos);
            
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

void tes_p21()
{
    return;
}

void tes_p3()
{
    return;
}

int change_efbank = 0;
int eff_blur_off = 0;
int effect_disp_flg = 0;
int eff_filament_off = 0;
int now_buffer[2] = { 0, 0 };
int eff_dith_off = 0;
int stop_effects = 0;
int monochrome_mode = 0;
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
int magatoki_mode = 0;
#endif
int shibata_set_off = 0;
int look_debugmenu = 0;
