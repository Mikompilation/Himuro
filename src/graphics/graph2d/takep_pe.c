#include "common.h"
#include "typedefs.h"
#include "takep_pe.h"

#include "sce/libvu0.h"
#include "ee/eestruct.h"

#include "graphics/graph3d/sglib.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph2d/tim2_new.h"

sceVu0IVECTOR particle_eff_col = {100, 100, 200, 0};
sceVu0FVECTOR l_eye_pos = {23087.0f, -687.0f, 12135.0f, 0.0f};
sceVu0FVECTOR r_eye_pos = {23104.0f, -687.0f, 12103.0f, 0.0f};
PARTICLE_EFF_OBJ dummy_pe_obj = {0};

static int pe_active_time;
static u_int pe_index;
static int rem_pe_ix;

static int pe_callflg = 0;
u_char eye_light_flg = 0;

// #include <cstdlib.h>
// RAND_MAX = (2**31-1)
#define RAND_MAX 2147483647

void GetBetweenABVecUnitLen(sceVu0FVECTOR out_a2bv, sceVu0FVECTOR out_unit_a2bv, float *out_len, sceVu0FVECTOR in_av, sceVu0FVECTOR in_bv)
{
    sceVu0FVECTOR temp;

    if (out_a2bv == NULL)
    {
        out_a2bv = *(sceVu0FVECTOR *)&temp;
    }

    sceVu0SubVector(out_a2bv, in_bv, in_av);

    if (out_len != NULL)
    {
        *out_len = SgSqrtf(sceVu0InnerProduct(out_a2bv, out_a2bv));
    }

    if (out_unit_a2bv != NULL)
    {
        sceVu0Normalize(out_unit_a2bv, out_a2bv);
    }
}

void SetPrePEImage(PARTICLE_EFF_OBJ *obj)
{
    PARTICLE_EFF_OBJ *temppep;
    PARTICLE_EFF_OBJ *prepep;
    PARTICLE_EFF_OBJ *temppointer;
    int tempalpha;

    temppointer = prepep = (PARTICLE_EFF_OBJ *)obj->underpe;

    tempalpha = obj->alphabl * 0.75f;
    temppep = obj;

    while (1)
    {
        prepep = temppep;

        temppep = (PARTICLE_EFF_OBJ *)temppep->underpe;
        temppep->alphabl = tempalpha = tempalpha * 0.75f;

        if (temppep->underpe == NULL)
        {
            break;
        }
    }

    obj->underpe = temppep;
    temppep->underpe = temppointer;
    prepep->underpe = NULL;

    temppep->state = obj->state;

    temppep->scrvec[0] = obj->scrvec[0];
    temppep->scrvec[1] = obj->scrvec[1];
    temppep->scrvec[2] = obj->scrvec[2];
    temppep->scrvec[3] = obj->scrvec[3];

    temppep->pe_width = obj->pe_width;
    temppep->alphabl = obj->alphabl * 0.75f;
}

int InitPrePEImage(PARTICLE_EFF_OBJ *obj, int pe_num)
{
    int i;
    PARTICLE_EFF_OBJ *temp;

    for (i = 0; i < 3; i++, pe_num--)
    {
        obj->underpe = temp;
        temp->pe_width = 0;
        obj = temp;
    }

    temp->underpe = NULL;

    return pe_num;
}

int SetPEObjPos(PARTICLE_EFF_OBJ *obj)
{
    sceVu0FVECTOR c2sv_move;
    float scale;

    obj->nowz += obj->steplen;

    sceVu0AddVector(obj->c2ov_move, obj->c2ov_move, obj->c2ov_step);

    scale = (obj->ka * obj->nowz + obj->kb) * obj->nowz + obj->c2slen;

    sceVu0ScaleVector(c2sv_move, obj->c2sv_unit, scale);

    SgSqrtf(c2sv_move[0] * c2sv_move[0] + c2sv_move[1] * c2sv_move[1] + c2sv_move[2] * c2sv_move[2]);

    sceVu0AddVector(obj->pos, c2sv_move, obj->c2ov_move);

    obj->pos[3] = 1.0f;

    return 1;
}

void MakeRandomVec(float *output, int m)
{
    if (rand() > RAND_MAX / 2)
    {
        output[0] = rand() % m;
    }
    else
    {
        output[0] = -rand() % m;
    }

    if (rand() > RAND_MAX / 2)
    {
        output[1] = rand() % m;
    }
    else
    {
        output[1] = -rand() % m;
    }

    if (rand() > RAND_MAX / 2)
    {
        output[2] = rand() % m;
    }
    else
    {
        output[2] = -rand() % m;
    }
}

void InitPEObj(PARTICLE_EFF_OBJ *obj, sceVu0FVECTOR ov, sceVu0FVECTOR sv, sceVu0FVECTOR tempgv)
{
    int temptime;
    float randrate;
    float tempkk;
    float hhh;
    float o2slen;
    sceVu0FVECTOR o2sv_unit;
    sceVu0FVECTOR o2gv_unit;
    sceVu0FVECTOR o2gv;
    sceVu0FVECTOR o2sv;
    sceVu0FVECTOR cv;
    sceVu0FVECTOR o2cv;
    sceVu0FVECTOR c2sv;
    sceVu0FVECTOR gv;
    float inner;

    MakeRandomVec(o2sv_unit, 100);

    sceVu0AddVector(gv, tempgv, o2sv_unit);

    hhh = 30.0f;

    GetBetweenABVecUnitLen(o2gv, o2gv_unit, NULL, ov, gv);
    GetBetweenABVecUnitLen(o2sv, o2sv_unit, &o2slen, ov, sv);

    inner = sceVu0InnerProduct(o2gv_unit, o2sv_unit);

    sceVu0ScaleVector(o2cv, o2gv_unit, inner * o2slen);

    obj->c2olen = SgSqrtf(sceVu0InnerProduct(o2cv, o2cv));

    sceVu0AddVector(cv, ov, o2cv);
    sceVu0CopyVector(obj->c2ov_move, cv);

    GetBetweenABVecUnitLen(c2sv, obj->c2sv_unit, &obj->c2slen, cv, sv);

    tempkk = -(obj->c2olen * 0.2f);

    randrate = rand() / (float)RAND_MAX;
    randrate = -(obj->c2olen * 0.5f * randrate);

    tempkk += randrate;

    obj->ka = (obj->c2slen * obj->c2olen + tempkk * (hhh - obj->c2slen)) / tempkk / obj->c2olen / (obj->c2olen - tempkk);
    obj->kb = -(obj->ka * (tempkk + obj->c2olen));

    temptime = rand() % 0x28 + 0x28;
    obj->m_time = temptime;

    if (obj->c2olen < 1000.0f)
    {
        obj->m_time = temptime * 0.5f;

        if (obj->c2olen > 100.0f)
        {
            temptime -= obj->m_time;
            temptime = (temptime * obj->c2olen) / 1000.0f;

            obj->m_time += temptime;
        }
    }

    obj->steplen = obj->c2olen / obj->m_time;

    sceVu0ScaleVector(obj->c2ov_step, o2cv, -1.0f / obj->m_time);

    obj->alphabl = 0;
    obj->pe_width = 0;
    obj->nowz = 0.0f;

    obj->state = ST_WAIT2;
    obj->time = 0;
}

void SetPEObjSprSize(PARTICLE_EFF_OBJ *obj)
{
    float farpoint;

    farpoint = obj->c2olen * 1.1f;
    obj->pe_width = (((obj->nowz + farpoint) - obj->c2olen) / farpoint) * 800.0f;
}

void SetPEObjAlphaBl(PARTICLE_EFF_OBJ *obj)
{
    if (obj->time != 0)
    {
        obj->alphabl = (obj->time * 32) / obj->m_time + 16;
    }
    else
    {
        obj->alphabl = 0;
    }
}

int CtrlPEObj(PARTICLE_EFF_OBJ *obj) {
    ST_PEOBJ state;

    state = obj->state;

    switch(state)
    {
    case ST_GO_CAMERA:
        if (obj->time != 0)
        {
            SetPrePEImage(obj);
            SetPEObjPos(obj);
            SetPEObjSprSize(obj);
            SetPEObjAlphaBl(obj);

            obj->time--;

            return 1;
        } else {
            obj->state = ST_FINISH_PE;

            SetPrePEImage(obj);
        }
    break;
    case ST_WAIT2:
        if (obj->time != 0)
        {
            obj->time--;
        }
        else
        {
            obj->state = ST_GO_CAMERA;
            obj->time = obj->m_time;
        }
    break;
    case ST_FINISH_PE:
        SetPrePEImage(obj);
    break;
    case ST_WAIT1:
        obj->state = ST_GO_AROUND;
    break;
    case ST_GO_AROUND:
        obj->time = 0;
        obj->state = ST_WAIT2;
    break;
    }

    return 0;
}

void SortRegisterPEObj(PARTICLE_EFF_OBJ *obj)
{
    PARTICLE_EFF_OBJ *nextpe;
    PARTICLE_EFF_OBJ *temppe;
    int flg;

    temppe = &dummy_pe_obj;
    flg = 1;

    while (nextpe = temppe->nextpe, temppe->nextpe != 0)
    {
        if (obj->scrvec[2] < nextpe->scrvec[2])
        {
            temppe->nextpe = obj;
            obj->nextpe = nextpe;
            flg = 0;

            break;
        }

        temppe = temppe->nextpe;
    }

    if (flg)
    {
        temppe->nextpe = obj;
        obj->nextpe = 0;
    }
}

Q_WORDDATA* ParticleEffectInTakingPicture(Q_WORDDATA *qd, u_int b_num)
{
    qd++;

    return qd;
}

Q_WORDDATA* SetPESpritePacket(Q_WORDDATA *qd, sceVu0IVECTOR col, sceVu0IVECTOR centervec, int pe_width, int albl)
{
    qd->ui32[0] = 0;
    qd->ui32[1] = 0;
    qd->ui32[2] = 0;
    qd->ui32[3] = 0;
    qd++;

    qd->ui32[0] = col[0];
    qd->ui32[1] = col[1];
    qd->ui32[2] = col[2];
    qd->ui32[3] = albl;
    qd++;

    qd->ui32[0] = centervec[0] - pe_width;
    qd->ui32[1] = centervec[1] - (pe_width >> 1);
    qd->ui32[2] = centervec[2];
    qd->ui32[3] = 0;
    qd++;

    qd->ui32[0] = effdat[46].w * 16;
    qd->ui32[1] = effdat[46].h * 16;
    qd->ui32[2] = 0;
    qd->ui32[3] = 0;
    qd++;

    qd->ui32[0] = col[0];
    qd->ui32[1] = col[1];
    qd->ui32[2] = col[2];
    qd->ui32[3] = albl;
    qd++;

    qd->ui32[0] = centervec[0] + pe_width;
    qd->ui32[1] = centervec[1] + (pe_width >> 1);
    qd->ui32[2] = centervec[2];
    qd->ui32[3] = 0;
    qd++;

    return qd;
}

void SetCenterGravOfFVECTOR(float *input, sceVu0FVECTOR *vec, u_int num)
{
    int i;

    input[0] = 0.0f;
    input[1] = 0.0f;
    input[2] = 0.0f;

    for (i = 0; i < num; i++)
    {
        sceVu0AddVector(input, input, vec[i]);
    }

    sceVu0ScaleVector(input, input, 1.0f / num);
}

void GetA2BLoadVector(sceVu0FVECTOR out_v, sceVu0FVECTOR fr_v, sceVu0FVECTOR to_v, float rate)
{
    sceVu0FVECTOR temp;

    sceVu0SubVector(temp, to_v, fr_v);
    sceVu0ScaleVector(temp, temp, rate);
    sceVu0AddVector(out_v, fr_v, temp);
}

void CallParticleEffect(u_char ene_id)
{
    return;
}

void StopParticleEffect()
{
    pe_callflg = 0;
}

void SetParticleEffect() {
    int n;
    u_int start_pktaddr;
    u_int end_pktaddr;
    sceVu0FMATRIX mat;

    if (pe_callflg == 0)
    {
        return;
    }

    Reserve2DPacket(0x1000);

    n = ndpkt;

    pbuf[ndpkt++].ul128 = (u_long128)0;

    start_pktaddr = (u_int)&pbuf[ndpkt];

    pbuf[ndpkt].ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_FALSE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 3);
    pbuf[ndpkt++].ul64[1] = 0 \
        | SCE_GS_TEX0_1     << (4 * 0) \
        | SCE_GS_TEX0_2     << (4 * 1) \
        | SCE_GIF_PACKED_AD << (4 * 2);

    pbuf[ndpkt++].ul64[0] = effdat[46].tex0;

    pbuf[ndpkt++].ul128 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0); // shouldn't this be a SCE_GS_TEX0_2 ??

    pbuf[ndpkt].ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    pbuf[ndpkt++].ul64[1] = SCE_GS_ALPHA_1;

    end_pktaddr = (u_int)ParticleEffectInTakingPicture(&pbuf[ndpkt], pe_index);
    end_pktaddr = (end_pktaddr - start_pktaddr) >> 4;

    pbuf[n].ui32[0] = DMAend + end_pktaddr;

    ndpkt = n + end_pktaddr + 1;

    if (--pe_active_time == 0)
    {
        pe_callflg = 0;
    }
}

void CallEyeLight(sceVu0FVECTOR *lpos, sceVu0FVECTOR *rpos)
{
    eye_light_flg = 1;

    l_eye_pos[3] = r_eye_pos[3] = 1.0f;
}

void StopEyeLight()
{
    eye_light_flg = 0;
}

void EyeLightCtrl() {
    int i;
    int n;
    int w;
    u_int start_pktaddr;
    u_int end_pktaddr;
    sceVu0FMATRIX mat;
    sceVu0IVECTOR scr[2];
    sceVu0IVECTOR col = {0.0f, 0.0f, 0.0f, 0.0f};
    sceVu0FVECTOR vec;
    Q_WORDDATA *qd;

    col[0] = 200;

    if (eye_light_flg == 0)
    {
        return;
    }

    sceVu0RotTransPers(scr[0], SgWSMtx, l_eye_pos, 1);
    sceVu0RotTransPers(scr[1], SgWSMtx, r_eye_pos, 1);

    for (w = 0, i = 0; i < 2; i++)
    {
        if (scr[i][0] < 0x4000 || scr[i][0] > 0xc000)
        {
            w = 1;
        }

        if (scr[i][1] < 0x4000 || scr[i][1] > 0xc000)
        {
            w = 1;
        }

        if (scr[i][2] < 0xff || scr[i][2] > 0x00ffffff)
        {
            w = 1;
        }
    }

    if (!w)
    {
        Reserve2DPacket(0x1000);

        n = ndpkt;

        pbuf[ndpkt++].ul128 = (u_long128)0;

        start_pktaddr = (u_int)&pbuf[ndpkt];
        qd = (Q_WORDDATA *)start_pktaddr;

        qd->ul64[0] = SCE_GIF_SET_TAG(1, SCE_GS_FALSE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 3);
        qd->ul64[1] = 0 \
            | SCE_GS_TEX0_1     << (4 * 0) \
            | SCE_GS_TEX0_2     << (4 * 1) \
            | SCE_GIF_PACKED_AD << (4 * 2);
        qd++;

        qd->ul64[0] = effdat[46].tex0;
        qd++;

        qd->ul128 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0); // shouldn't this be a SCE_GS_TEX0_2 ??
        qd++;

        qd->ul64[0] = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
        qd->ul64[1] = SCE_GS_ALPHA_1;
        qd++;

        qd->ul64[0] = SCE_GIF_SET_TAG(2, SCE_GS_TRUE, SCE_GS_TRUE, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 6);
        qd->ul64[1] = 0 \
            | SCE_GS_UV    << (4 * 0) \
            | SCE_GS_RGBAQ << (4 * 1) \
            | SCE_GS_XYZ2  << (4 * 2) \
            | SCE_GS_UV    << (4 * 3) \
            | SCE_GS_RGBAQ << (4 * 4) \
            | SCE_GS_XYZ2  << (4 * 5);
        qd++;

        for (i = 0; i < 2; i++)
        {
            qd = SetPESpritePacket(qd, col, scr[i], (scr[i][2] * 300.0f) / 4000.0f, 0x50);
        }

        end_pktaddr = ((u_int)qd - start_pktaddr) >> 4;

        pbuf[n].ui32[0] = DMAend + end_pktaddr;

        ndpkt = n + end_pktaddr + 1;
    }
}
