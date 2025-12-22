#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "wan_soul.h"

// #include <cstdlib.h>
// RAND_MAX = (2**31-1)
#define RAND_MAX 2147483647

#include "sce/libvu0.h"

#include "common/ul_math.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph3d/sglib.h"
#include "ingame/event/ev_main.h"
#include "ingame/event/wan_dat.h"
#include "ingame/event/wan_soul.h"
#include "ingame/map/furn_ctl.h"
#include "ingame/map/furn_spe/furn_spe.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/plyr/plyr_ctl.h"
#include "ingame/plyr/unit_ctl.h"
#include "main/glob.h"
#include "os/eeiop/adpcm/ea_soul.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"

WANDER_SOUL_WRK wander_soul_wrk[1] = {0};

static int ene_face_load_id;
static void *ws_noise_buf;
static void *face_buff;
static u_short adpcm_wait_limit;
static void *p_deform_buf;
static void *overlap_buf;
static u_char soul_tell_mode;
static short int telling_count;

static sceVu0FVECTOR save_soul_pos;
static sceVu0FVECTOR end_cam_pos;
static sceVu0FVECTOR cam_move_speed;
static sceVu0FVECTOR cam_ref_adjust_val;

#define PI 3.1415927f
#define WANDER_SOUL_FACE_ADDRESS 0x1080000

static inline float get_i(sceVu0FVECTOR v, int i) // HACK: fixes while loop in SetFirstDestination
{
    return v[i];
}

sceVu0FVECTOR* SetFirstDestination(sceVu0FVECTOR *destination, sceVu0FVECTOR pos)
{
    float dist;
    float dist2;

    dist = GetDistV(*destination++, pos);

    while (get_i(*destination, 0) != 0.0f || get_i(*destination, 1) != 0.0f || get_i(*destination, 2) != 0.0f)
    {
        dist2 = GetDistV(*destination++, pos);

        if (dist <= dist2)
        {
            break;
        }
    }

    return destination - 1;
}

float GetDist3(sceVu0FVECTOR v1, sceVu0FVECTOR v2)
{
    sceVu0FVECTOR temp;

    sceVu0SubVector(temp, v2, v1);

    return SgSqrtf(sceVu0InnerProduct(temp, temp));
}

float GetDistPlyr(sceVu0FVECTOR plyr, sceVu0FVECTOR soul)
{
    sceVu0FVECTOR temp;

    sceVu0CopyVector(temp, soul);

    temp[1] -= -500.0f;

    sceVu0SubVector(temp, temp, plyr);

    return SgSqrtf(sceVu0InnerProduct(temp, temp));
}

void* SetEffectsWithScale(sceVu0FVECTOR pos, SOUL_EFF_PARAM *param, float scale, float rgb)
{
    static SOUL_EFF_PARAM temp;

    temp.r1 = param->r1 * rgb;
    temp.g1 = param->g1 * rgb;
    temp.b1 = param->b1 * rgb;

    temp.scale1 = param->scale1 * scale;

    temp.r2 = param->r2 * rgb;
    temp.g2 = param->g2 * rgb;
    temp.b2 = param->b2 * rgb;

    temp.scale2 = param->scale2 * scale;

    return SetEffects(0x17, 1, 3, pos, temp.r1, temp.g1, temp.b1, temp.scale1, temp.r2, temp.g2, temp.b2, temp.scale2);
}

int GetUsableWanSoWrkID()
{
    return 0;
}

void SetNewSpeed(WANDER_SOUL_WRK *wswrk)
{
    sceVu0SubVector(wswrk->accelv, *wswrk->destination, wswrk->ori_pos);
    sceVu0Normalize(wswrk->accelv, wswrk->accelv);
    sceVu0ScaleVector(wswrk->accelv, wswrk->accelv, 0.5f);
}

void NewMissionInitWanderSoul()
{
    *wander_soul_wrk = (WANDER_SOUL_WRK){0};
}

void ReviveWanderSoul()
{
    int i;

    telling_count = 0;
    ws_noise_buf = NULL;
    face_buff = NULL;
    adpcm_wait_limit = 0;
    p_deform_buf = NULL;
    overlap_buf = NULL;

    for (i = 0; i < 1; i++)
    {
        if (wander_soul_wrk[i].eff_buff != NULL)
        {
            wander_soul_wrk[i].disp_flg = 0;

            RegisterWansoEffect(&wander_soul_wrk[i]);
        }
    }
}

void BSplinePos(sceVu0FVECTOR pos, sceVu0FMATRIX mat_point_array, float t1)
{
    float rev;
    float t2;
    float t3;
    sceVu0FVECTOR coeff;

    rev = 1.0f - t1;
    t2 = t1 * t1;
    t3 = t2 * t1;

    coeff[0] = rev * rev * rev;
    coeff[1] = (t3 * 3.0f - t2 * 6.0f) + 4.0f;
    coeff[2] = (t2 + t1 - t3) * 3.0f + 1.0f;
    coeff[3] = t3;

    sceVu0ApplyMatrix(pos, mat_point_array, coeff);

    pos[3] = 6.0f;

    sceVu0ScaleVector(pos, pos, 0.16666667f);
}

void ClearWanderSoulOne(WANDER_SOUL_WRK *wswrk)
{
    *wswrk = (WANDER_SOUL_WRK){0};
}

void CallWanderSoul(u_char data_id, sceVu0FVECTOR pos)
{
    WANDER_SOUL_WRK *wswrk;

    wswrk = &wander_soul_wrk[GetUsableWanSoWrkID()];

    if (wswrk->eff_buff != NULL)
    {
        ResetEffects(wswrk->eff_buff);
    }

    ClearWanderSoulOne(wswrk);

    wswrk->state = ST_WANSO_INIT;

    sceVu0CopyVector(wswrk->ori_pos, pos);

    wswrk->id = data_id;
}

void RegisterWansoEffect(WANDER_SOUL_WRK *wswrk)
{
    if (wswrk->disp_flg == 0 && change_efbank != 0)
    {
        wswrk->eff_buff = SetEffects(25, 2, 2, wswrk->disp_pos, &wswrk->srate, &wswrk->arate);

        if (wswrk->eff_buff != NULL)
        {
            wswrk->disp_flg = 1;
        }
    }
}

void WansoLateInit(WANDER_SOUL_WRK *wswrk)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        wswrk->sp_mat[i][0] = wswrk->ori_pos[0];
        wswrk->sp_mat[i][1] = wswrk->ori_pos[1];
        wswrk->sp_mat[i][2] = wswrk->ori_pos[2];
        wswrk->sp_mat[i][3] = wswrk->ori_pos[3];
    }

    SetWanSoDat2Wrk(wswrk->id,wswrk);

    if (wswrk->destination != NULL)
    {
        wswrk->state = ST_WANSO_SPLINE;

        SetFirstDestination(wswrk->destination,wswrk->ori_pos);

        wswrk->spl_speed = 0.5f;
    }
    else
    {
        wswrk->state = ST_WANSO_FLOAT;
    }

    wswrk->disp_pos[0] = 0.0f;
    wswrk->disp_pos[1] = 0.0f;
    wswrk->disp_pos[2] = 0.0f;
    wswrk->room_no = room_wrk.disp_no[0];
    wswrk->count = 0;
    wswrk->srate = 1.0f;
    wswrk->arate = 1.0f;

    RegisterWansoEffect(wswrk);
}

void ReleaseWanderSoul(u_char data_id)
{
    int i;
    WANDER_SOUL_WRK *wswrk;

    wswrk = wander_soul_wrk;

    for (i = 0; i < 1; i++)
    {
        if (wswrk->id == data_id)
        {
            break;
        }

        wswrk++;
    }

    if (i < 1)
    {
        wswrk->state = ST_WANSO_VACANT;
    }
}

int WanSoNearJudge(float dist, float distv)
{
    if (dist < 2000.0f || distv < 1000.0f)
    {
        return 1;
    }

    return 0;
}

int WanSoTouchJudge(sceVu0FVECTOR soul_pos, MOVE_BOX *mbp)
{
    if (RotCheck(soul_pos, mbp, 50) == 0)
    {
        return 0;
    }

    if (GetDist3(soul_pos, mbp->pos) > 400.0f)
    {
        return 0;
    }

    return 1;
}

void SoulFloating(sceVu0FVECTOR pos, float *rgb)
{
    static int time = 0;

    pos[1] = SgSinf((time / 80.0f) * PI) * 15.0f;

    pos[0] = 0.0f;
    pos[2] = 0.0f;

    time++;
}

void SetSoulNewMove(WANDER_SOUL_WRK *wswrk)
{
    float dist;
    sceVu0FVECTOR temp;

    dist = GetDist3(wswrk->ori_pos,*wswrk->destination);

    if (dist <= 500.0f)
    {
        wswrk->dist = dist;
        wswrk->count = wswrk->dist / 10.5f;
    }
    else if (dist <= 750.0f)
    {
        wswrk->dist = (rand() / (float)RAND_MAX * 0.5f + 1.0f) * 250.0f;
        wswrk->count = wswrk->dist / 10.5f;
    }
    else
    {
        wswrk->dist = rand() * 500.0f / (float)RAND_MAX;
        wswrk->count = wswrk->dist / 10.5f;
    }
}

void Change2WanSoExtinct(WANDER_SOUL_WRK *wswrk)
{
    if (wswrk->adpcm_id != 0xffff)
    {
        AdpcmStopSoul(20);
    }

    wswrk->wanso_wait_time = 20;
    wswrk->state = ST_WANSO_EXTINCT;
    wswrk->vanish = SOUL_LIGHT_WAIT;
    wswrk->destination = NULL;
}

void FinishWanSoWrk(WANDER_SOUL_WRK *wswrk)
{
    if (ws_noise_buf != NULL)
    {
        ResetEffects(ws_noise_buf);
    }

    if (p_deform_buf == NULL)
    {
        wswrk->state = ST_WANSO_VACANT;
    }
    else
    {
        ResetEffects(p_deform_buf);

        wswrk->state = ST_WANSO_VACANT;
    }

    ingame_wrk.mode = INGAME_MODE_NOMAL;

    plyr_wrk.soulp[3] = 0.0f;

    furn_disp_flg = 0;
}

void ComeOnSignal(float *scale, float *rgb, u_short time)
{
    return;
}

float StopAbleDist(float speed, float accel, u_short *count)
{
    float time;

    time = speed / accel;

    *count = time;

    return speed * time - time * time * accel * 0.5f;
}

float GetRot(sceVu0FVECTOR a, sceVu0FVECTOR b)
{
    sceVu0FVECTOR temp;

    sceVu0SubVector(temp, b, a);

    return SgAtan2f(temp[0], temp[2]);
}

int RotCheck2(float rot1, float rot2, u_short range)
{
    float rot;

    rot = rot1 - rot2;

    RotLimitChk(&rot);

    rot = __builtin_fabsf(rot);

    return rot <= (range * PI) / 180.0;
}

void GetBSplinePos(sceVu0FVECTOR pos, sceVu0FMATRIX mat_point_array, float t1)
{
    float rev;
    float t2;
    float t3;
    sceVu0FVECTOR coeff;

    rev = 1.0f - t1;
    t2 = t1 * t1;
    t3 = t2 * t1;

    coeff[0] = rev * rev * rev;
    coeff[1] = (t3 * 3.0f - t2 * 6.0f) + 4.0f;
    coeff[2] = (t2 + t1 - t3) * 3.0f + 1.0f;
    coeff[3] = t3;

    sceVu0ApplyMatrix(pos, mat_point_array, coeff);

    pos[3] = 6.0f;

    sceVu0ScaleVector(pos, pos, 0.16666667f);
}

int SetMoveDirect(WANDER_SOUL_WRK *wswrk, float s2p_len)
{
    float rot1;
    float rot2;
    sceVu0FVECTOR temp;
    sceVu0FVECTOR *tempdes;

    tempdes = wswrk->destination;

    rot2 = GetRot(wswrk->ori_pos,plyr_wrk.move_box.pos);

    while ((*tempdes)[0] != -1.0f || (*tempdes)[1] != -1.0f || (*tempdes)[2] != -1.0f)
    {
        sceVu0SubVector(temp, *tempdes, wswrk->ori_pos);

        rot1 = SgAtan2f(temp[0], temp[2]);

        if (RotCheck2(rot1, rot2, 30) == 0)
        {
            wswrk->destination = tempdes;

            sceVu0Normalize(temp, temp);
            sceVu0ScaleVector(wswrk->accelv, temp, 0.5f);

            return 1;
        }

        tempdes++;
    }

    return 0;
}

void AddVec2MatD(sceVu0FMATRIX mat, sceVu0FVECTOR vec)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        mat[i][0] = mat[i+1][0];
        mat[i][1] = mat[i+1][1];
        mat[i][2] = mat[i+1][2];
        mat[i][3] = mat[i+1][3];
    }

    mat[3][0] = vec[0];
    mat[3][1] = vec[1];
    mat[3][2] = vec[2];
    mat[3][3] = vec[3];
}

void Change2WanSoFloat(WANDER_SOUL_WRK *wswrk)
{
    wswrk->state = ST_WANSO_FLOAT;

    wswrk->destination = NULL;
}

float GetKnotDist(sceVu0FMATRIX mat)
{
    sceVu0FVECTOR coeff;
    sceVu0FVECTOR first_p;
    sceVu0FVECTOR last_p;

    coeff[0] = 0.16666667f;
    coeff[1] = 0.6666667f;
    coeff[2] = 0.16666667f;
    coeff[3] = 0.0f;

    sceVu0ApplyMatrix(first_p, mat, coeff);

    coeff[0] = 0.0f;
    coeff[1] = 0.16666667f;
    coeff[2] = 0.6666667f;
    coeff[3] = 0.16666667f;

    sceVu0ApplyMatrix(last_p, mat, coeff);
    sceVu0SubVector(coeff, last_p, first_p);

    return SgSqrtf(sceVu0InnerProduct(coeff, coeff));
}

void SetWansoSpeed(WANDER_SOUL_WRK *wswrk, float dist)
{
    static int count = 0;

    if (count != 10)
    {
        count++;
    }
    else
    {
        count = 11;

        if (dist > 1500.0f)
        {
            wswrk->spl_speed = 0.0f;
        }
        else
        {
            wswrk->spl_speed = SgSqrtf(sceVu0InnerProduct(plyr_wrk.move_box.spd, plyr_wrk.move_box.spd));
        }
    }
}

void WansoPreTell(WANDER_SOUL_WRK *wswrk)
{
    if (wswrk->adpcm_id != 0xffff)
    {
        AdpcmPreSoulFadeOut(10);
    }
    else
    {
        wswrk->wanso_wait_time = 10;
    }

    FinderEndSet();
    CallSoulTellingCameraIn(wswrk->ori_pos, plyr_wrk.move_box.pos, 30);
    SetPlyrAnime(PANI_STAND, 0);

    wswrk->count = 30;
    wswrk->state = ST_WANSO_TELLIN;

    ingame_wrk.mode = INGAME_MODE_WANDER_SOUL;

    SeStartPos(7, wswrk->ori_pos, 0, 0x1000, 0x1000, 0xff);
}

u_char WansoAlphaCtrl(WANDER_SOUL_WRK *wswrk, float dist)
{
    int flg;

    flg = 1;

    if (ev_wrk.movie_on != 0 || plyr_wrk.sta & 0x1)
    {
        flg = 0;
    }
    else if (dist < 600.0f && wswrk->message_id == 0xff && wswrk->destination != NULL)
    {
        flg = 0;
    }

    if (flg)
    {
        wswrk->arate += 0.04f;
    }
    else
    {
        wswrk->arate -= 0.04f;
    }

    if (wswrk->arate > 1.0f)
    {
        wswrk->arate = 1.0f;
    }
    else if (wswrk->arate < 0.0f)
    {
        wswrk->arate = 0.0f;
    }

    if (wswrk->arate >= 1.0f)
    {
        return 1;
    }

    return 0;
}

void WansoExtinctCtrl(WANDER_SOUL_WRK *wswrk)
{
    switch (wswrk->state)
    {
    case SOUL_LIGHT_BIGGER:
        if (--wswrk->count == 0)
        {
            wswrk->count = 20;
            wswrk->state = SOUL_LIGHT_LESSER;
        }
        else
        {
            wswrk->srate = (30 - wswrk->count) / 20.0f;
        }
    break;
    case SOUL_LIGHT_LESSER:
        if (--wswrk->count == 0)
        {
            wswrk->count = 1;
            wswrk->state = SOUL_LIGHT_END;
        }
        else
        {
            wswrk->srate = wswrk->count / 20.0f;
        }
    break;
    case SOUL_LIGHT_END:
        if (--wswrk->count == 0)
        {
            wswrk->state = ST_WANSO_VACANT;

            FinishWanSoWrk(wswrk);
        }
        else
        {
            wswrk->srate = 0.0f;
        }
    break;
    default:
        // do nothing ...
    break;
    }
}

void OneSoulCtrl(WANDER_SOUL_WRK *wswrk, float *srate, float *arate)
{
    float dist;
    float knot_dist;
    static float rate = 0.0f;
    u_char visible;

    SoulFloating(wswrk->disp_pos, arate);
    sceVu0AddVector(wswrk->disp_pos, wswrk->ori_pos, wswrk->disp_pos);

    dist = GetDistPlyr(plyr_wrk.move_box.pos, wswrk->ori_pos);

    GetDistV(plyr_wrk.move_box.pos, wswrk->ori_pos);
    SgSqrtf(sceVu0InnerProduct(wswrk->speedv, wswrk->speedv));

    visible = WansoAlphaCtrl(wswrk, dist);

    if (dist < 1500.0f && visible != 0)
    {
        wswrk->disp_pos[3] = 1.0f;

        sceVu0CopyVector(plyr_wrk.soulp, wswrk->disp_pos);
    }
    else
    {
        plyr_wrk.soulp[3] = 0.0f;
    }

    if (visible != 0)
    {
        if (ingame_wrk.mode == INGAME_MODE_NOMAL && wswrk->message_id != 0xff)
        {
            if (wswrk->state < ST_WANSO_TELLIN)
            {
                if (dist < 400.0f && (plyr_wrk.sta & 0x8) == 0)
                {
                    if (map_wrk.now_room != 21 || wswrk->room_no != 25)
                    {
                        if (RotCheck(wswrk->ori_pos, &plyr_wrk.move_box, 50) != 0)
                        {
                            WansoPreTell(wswrk);
                        }
                    }
                }
            }
        }
    }

    switch(wswrk->state)
    {
    case ST_WANSO_SPLINE:
        rate += wswrk->spl_speed;

        while (rate >= 1.0f)
        {
            rate -= 1.0f;

            if (wswrk->spl_flg != 0)
            {
                if (wswrk->spl_flg > 4)
                {
                    if (wswrk->message_id == 0xff)
                    {
                        Change2WanSoExtinct(wswrk);
                    }
                    else
                    {
                        Change2WanSoFloat(wswrk);
                    }
                    break;
                }

                wswrk->spl_flg++;
            }
            else
            {
                wswrk->destination++;

                if (wswrk->destination[0][0] == -1.0f && wswrk->destination[0][1] == -1.0f && wswrk->destination[0][2] == -1.0f)
                {
                    wswrk->spl_flg = 1;
                    wswrk->destination--;
                }
            }

            AddVec2MatD(wswrk->sp_mat, *wswrk->destination);

            knot_dist = GetKnotDist(wswrk->sp_mat);

            if (knot_dist > 0.0f)
            {
                wswrk->spl_speed = 2.0f / knot_dist;
            }
            else
            {
                wswrk->spl_speed = 1.0f;
            }
        }

        GetBSplinePos(wswrk->ori_pos, wswrk->sp_mat, rate);
    break;
    case ST_WANSO_FLOAT:
        // do nothing ...
    break;
    case ST_WANSO_TELLIN: {
        sceVu0FVECTOR soul = { 0.0f, -500.0f, 400.0f, 1.0f };
        sceVu0FVECTOR camera = { 150.0f, -660.0f, -25.0f, 1.0f };
        sceVu0FVECTOR interest = { 0.0f, -600.0f, 225.0f, 1.0f };

        if (--wswrk->count != 0)
        {
            if (wswrk->count == 3)
            {
                overlap_buf = SetEffects(8, 2, 0x28);
            }
        }
        else
        {
            wswrk->turn = CallSoulTellingCamera(wswrk->ori_pos, camera, interest, soul);
            wswrk->state = ST_WANSO_ADPCM_OUT;
        }
    } break;
    case ST_WANSO_ADPCM_OUT:
        if (wswrk->adpcm_id != 0xffff)
        {
            if (IsFadeoutEndAdpcmSoul() == 0)
            {
                if (adpcm_wait_limit++ >= 900)
                {
                    AdpcmStopSoul(10);

                    wswrk->adpcm_id = -1;
                    wswrk->state = ST_WANSO_TELL;
                }

                break;
            }
        }
        else if (wswrk->wanso_wait_time != 0)
        {
            wswrk->wanso_wait_time--;

            break;
        }
        wswrk->state = ST_WANSO_TELL;
    case ST_WANSO_TELL: {
        static u_char *strp;
        static signed char noise_in_time;
        static u_short se_no;
        static u_char face_load_end;

        if (telling_count == 0)
        {
            se_no = SeStartPos(8, wswrk->ori_pos, 0, 0x1000, 0x1000, 0xff);

            ws_noise_buf = SetEffects(2, 4, 7, 32.0f, 9.0f, 0x46, 0x78, 0x78, 0, 0x78);

            noise_in_time = 40;

            strp = (u_char *)GetIngameMSGAddr(44, wswrk->message_id);

            if (wswrk->adpcm_id != 0xffff)
            {
                AdpcmPlaySoul(wswrk->adpcm_id, 0x3fff, 5);
            }

            if (wswrk->face_id != 0xffff)
            {
                ene_face_load_id = LoadReq(wswrk->face_id, WANDER_SOUL_FACE_ADDRESS);

                face_load_end = 0;
            }
        }

        if (telling_count < 1)
        {
            if (face_load_end == 0 && IsLoadEnd(ene_face_load_id) != 0)
            {
                face_load_end = 1;
            }

            if (telling_count < -60 && face_load_end != 0)
            {
                face_buff = SetEffects(33, 8, 0, wswrk->turn, wswrk->ori_pos[0] + 0.0f, wswrk->ori_pos[1] + -80.0f, wswrk->ori_pos[2] + 0.0f);

                if (face_buff != NULL)
                {
                    static float spd = 0.5f;
                    static float rate = 1.0f;
                    static float trate = 1.0f;

                    p_deform_buf = SetEffects(27, 2, 0x17, 0x50, 0.7f, 0.7f, wswrk->ori_pos, 0, 0, 0, 0, &spd, &rate, &trate);

                    telling_count = 50;
                }
            }
        }

        if (telling_count != 0)
        {
            if (strp != NULL)
            {
                SetMessageV3(strp, 0x64000);
            }

            if (MesStatusCheck() == 0)
            {
                strp = NULL;
            }
        }

        if (noise_in_time >= 0 && --noise_in_time == 0)
        {
            if (ws_noise_buf != NULL)
            {
                ResetEffects(ws_noise_buf);
            }

            ws_noise_buf = SetEffects(2, 2, 7, 32.0f, 9.0f, 0x46, 0x78, 0, 0, 0);
        }

        if (telling_count == 1 || --telling_count == 1)
        {
            if (strp == NULL)
            {
                SeStop(se_no);
                Change2WanSoExtinct(wswrk);

                telling_count = 0;
            }
        }
    } break;
    case ST_WANSO_EXTINCT:
        if (wswrk->wanso_wait_time != 0)
        {
            wswrk->wanso_wait_time--;
        }
        else if (dist < 400.0f || wswrk->message_id != 0xff)
        {
            wswrk->state = SOUL_LIGHT_BIGGER;
            wswrk->count = 10;

            ingame_wrk.mode = INGAME_MODE_WANDER_SOUL;

            SetPlyrAnime(PANI_STAND, 10);
        }
    break;
    default:
        WansoExtinctCtrl(wswrk);
    break;
    }
}

void WanderSoulCtrl()
{
    int i;
    WANDER_SOUL_WRK *wswrk;
    static signed char time = 0;

    wswrk = wander_soul_wrk;

    for (i = 0; i < 1; i++, wswrk++)
    {
        if (wswrk->state == ST_WANSO_VACANT)
        {
            if (overlap_buf != NULL && time++ > '(')
            {
                ResetEffects(overlap_buf);

                overlap_buf = NULL;

                time = 0;
            }

            if (wswrk->disp_flg != 0)
            {
                wswrk->disp_flg = 0;

                if (wswrk->eff_buff != NULL)
                {
                    ResetEffects(wswrk->eff_buff);

                    wswrk->eff_buff = NULL;
                }
            }
        }
        else
        {
            if (wswrk->state == ST_WANSO_INIT)
            {
                WansoLateInit(wswrk);
            }
            else
            {
                RegisterWansoEffect(wswrk);
                OneSoulCtrl(wswrk, &wswrk->srate, &wswrk->arate);
            }
        }
    }
}

u_char CallSoulTellingCamera(sceVu0FVECTOR soul_pos, sceVu0FVECTOR relative_camera_pos, sceVu0FVECTOR relative_interest, sceVu0FVECTOR relative_soul_pos)
{
    sceVu0FVECTOR tempv;
    sceVu0FVECTOR axis;
    sceVu0FMATRIX mtx;
    float tempf;
    u_char turn;

    turn = 1;

    sceVu0SubVector(tempv, camera.p, soul_pos);
    sceVu0SubVector(axis, plyr_wrk.move_box.pos, soul_pos);

    axis[1] = 0.0f;
    tempv[1] = 0.0f;

    tempf = axis[0];
    axis[0] = -axis[2];
    axis[2] = tempf;

    if (sceVu0InnerProduct(axis, tempv) < 0.0f)
    {
        turn = 0;

        relative_camera_pos[0] = -relative_camera_pos[0];
    }

    sceVu0SubVector(tempv, soul_pos, plyr_wrk.move_box.pos);

    plyr_wrk.move_box.rot[1] = SgAtan2f(tempv[0], tempv[2]);

    sceVu0UnitMatrix(mtx);
    sceVu0RotMatrix(mtx, mtx, plyr_wrk.move_box.rot);
    sceVu0TransMatrix(mtx, mtx, plyr_wrk.move_box.pos);
    sceVu0ApplyMatrix(soul_pos, mtx, relative_soul_pos);
    sceVu0ApplyMatrix(camera.i, mtx, relative_interest);
    sceVu0ApplyMatrix(camera.p, mtx, relative_camera_pos);

    camera.roll = PI;
    camera.fov = PI / 3;

    soul_tell_mode = 0;

    return turn;
}

void CallSoulTellingCameraIn(sceVu0FVECTOR soul_pos, sceVu0FVECTOR mic_eye_pos, int time)
{
    sceVu0FVECTOR temp;

    sceVu0SubVector(temp, soul_pos, camera.p);
    sceVu0ScaleVector(cam_move_speed, temp, 1.0f / (time * 1.4f));
    sceVu0SubVector(temp, soul_pos, camera.i);
    sceVu0ScaleVector(cam_ref_adjust_val, temp, 1.0f / time);

    soul_tell_mode = 1;

    sceVu0CopyVector(save_soul_pos, soul_pos);
}

int SoulTellingCameraInCtrl()
{
    if (ingame_wrk.mode != INGAME_MODE_WANDER_SOUL)
    {
        return 0;
    }

    if (soul_tell_mode != 0)
    {
        sceVu0AddVector(camera.p, camera.p, cam_move_speed);
        sceVu0AddVector(camera.i, camera.i, cam_ref_adjust_val);
    }

    return 1;
}
