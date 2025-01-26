#include "common.h"
#include "typedefs.h"
#include "mdlact.h"

#include "sce/libvu0.h"

#include "main/glob.h"
#include "os/pad.h"
#include "os/eeiop/eese.h"
#include "graphics/motion/motion.h"
#include "graphics/motion/mdlact.h"
#include "graphics/graph3d/sglib.h"
#include "ingame/plyr/plyr_ctl.h"

plyr_act_func_t plyr_act_func[] = {
    motPlyrActLookAt,
    motPlyrActSurprise,
    motPlyrActSowasowa,
    motPlyrActKyoro,
    motPlyrActKyoro,
    motPlyrActKyoro,
    motPlyrActKyoro,
};
#include "data/plyr_act_furi0.h" // PLYR_FURI_DAT plyr_act_furi0[];
#include "data/plyr_act_furi1.h" // PLYR_FURI_DAT plyr_act_furi1[];
#include "data/plyr_act_furi2.h" // PLYR_FURI_DAT plyr_act_furi2[];
#include "data/plyr_act_furi3.h" // PLYR_FURI_DAT plyr_act_furi3[];

// #include <cstdlib.h>
// RAND_MAX = (2**31-1)
#define RAND_MAX 2147483647

#define PI 3.1415927f
#define PI2 6.2831855f
#define PI_HALF 1.5707964f

char motPlayerActCtrl(SgCOORDUNIT *cp)
{
    sceVu0FVECTOR p;
    sceVu0FVECTOR trot = { 0.0f, 0.0f, 0.0f, 0.0f };
    static float rot[2] = {0.0f, 0.0f};
    float spd;
    char type;
    sceVu0FVECTOR target;
    float yrot;
    float xrot;
    u_int i;
    float sub;
    float tmp;

    type = 1;

    spd = 1.0f;

    sceVu0CopyVector(target, cp[5].lwmtx[3]);

    target[1] += (plyr_wrk.spot_rot[0] * 60.0f) / 0.7853982f;

    xrot = (plyr_wrk.spot_rot[0] * 0.22f) / 0.69813174f;
    yrot = (plyr_wrk.spot_rot[1] * 0.22f) / 0.69813174f;

    trot[0] = -yrot;
    trot[1] = xrot;

    if (ani_mdl[0].anm.playnum <= 4 && __builtin_fabsf(plyr_wrk.soulp[3]) > 0.0001f)
    {
        motGetPlyrNeckRot(cp, trot, plyr_wrk.soulp);
    }
    else if (plyr_act_wrk.req == 1)
    {
        motPlyrActExec(cp);

        sceVu0CopyVector(trot, plyr_act_wrk.trot);

        spd = plyr_act_wrk.spd;

        type = plyr_act_wrk.move_type;
    }
    else if (ingame_wrk.mode == 7 || plyr_wrk.mode == 4)
    {
        trot[0] = trot[1] = 0.0f;
    }
    else if (
        ani_mdl[0].anm.playnum < 5 &&
        plyr_wrk.spot_rot[0] < 0.0001f &&
        plyr_wrk.spot_rot[1] < 0.0001f &&
        PlyrNeckDirectionChk(p) != 0
    )
    {
        motGetPlyrNeckRot(cp, trot, p);
    }

    if (!(ingame_wrk.stts & 0x80) && !(ingame_wrk.stts & 0x10))
    {
        if (type != 0)
        {
            rot[0] = rot[0] + ((trot[0] - rot[0]) / 25.0f) * spd;
            rot[1] = rot[1] + ((trot[1] - rot[1]) / 25.0f) * spd;
        }
        else
        {
            for (i = 0; i < 2; i++)
            {
                tmp = spd / 25.0f;

                sub = trot[i] - rot[i];

                if (__builtin_fabsf(sub) < tmp)
                {
                    rot[i] = trot[i];
                }
                else
                {
                    rot[i] = sub > 0.0f ? rot[i] + tmp : rot[i] - tmp;
                }
            }
        }

        LocalRotMatrixX(cp[13].matrix, cp[13].matrix, rot[0]);
        LocalRotMatrixZ(cp[13].matrix, cp[13].matrix, rot[1]);

        if (plyr_wrk.mode != 1)
        {
            motInversKinematics(cp, target, ani_mdl[0].mot.dat, 5);

            LocalRotMatrixX(cp[1].matrix, cp[1].matrix, yrot);
        }
    }

    return 0;
}

void motPlyrActExec(SgCOORDUNIT *cp)
{
    if (plyr_act_func[plyr_act_wrk.act_no](cp))
    {
        motPlyrActStop();
    }
}

void motInitPlyrAct()
{
    plyr_act_wrk.req = 0;
    plyr_act_wrk.act_no = 0xff;
    plyr_act_wrk.timer = 0;
    plyr_act_wrk.timer2 = 0;
    plyr_act_wrk.spd = 0.0;
    plyr_act_wrk.move_type = 0;
    plyr_act_wrk.pos[0] = 0.0f;
    plyr_act_wrk.pos[1] = 0.0f;
    plyr_act_wrk.pos[2] = 0.0f;
    plyr_act_wrk.pos[3] = 0.0f;
}

void motPlyrActReq(char act_no, sceVu0FVECTOR *pos, u_short cnt)
{
    plyr_act_wrk.req = 1;
    plyr_act_wrk.act_no = act_no;
    plyr_act_wrk.timer = 0;
    plyr_act_wrk.timer2 = cnt;
    plyr_act_wrk.spd = 0.0f;
    plyr_act_wrk.move_type = 0;

    if (pos != NULL)
    {
        sceVu0CopyVector(plyr_act_wrk.pos, *pos);
    }
}

void motPlyrActStop()
{
    plyr_act_wrk.req = 0;
    plyr_act_wrk.act_no = 0xff;
    plyr_act_wrk.timer = 0;
}

char motPlyrActLookAt(SgCOORDUNIT *cp)
{
    sceVu0FVECTOR sub;
    float len;

    plyr_act_wrk.timer++;

    if (plyr_act_wrk.timer > 300)
    {
        return 1;
    }

    sceVu0SubVector(sub, cp->lwmtx[3], plyr_act_wrk.pos);

    len = sceVu0InnerProduct(sub, sub);

    if (SgSqrtf(len) > 1500.0f)
    {
        return 0;
    }

    plyr_act_wrk.spd = 1.0f;
    plyr_act_wrk.move_type = 1;

    motGetPlyrNeckRot(cp, plyr_act_wrk.trot, plyr_act_wrk.pos);

    return 0;
}

char motPlyrActSurprise(SgCOORDUNIT *cp)
{
    static u_int se_handle;

    if (plyr_act_wrk.timer2 != 0)
    {
        plyr_act_wrk.timer2--;

        return 0;
    }

    plyr_act_wrk.move_type = 0x1;

    plyr_act_wrk.spd = 3.0f;

    motGetPlyrNeckRot(cp, plyr_act_wrk.trot, plyr_act_wrk.pos);

    if (((plyr_act_wrk.timer == 0) && (plyr_wrk.mode != 0x1)) && (ReqPlyrSpeAnime(68, 0) != 0))
    {
        plyr_wrk.mode = 0xA;
    }

    if ((plyr_wrk.mode == 0xA) && (plyr_act_wrk.timer < 20))
    {
        VibrateRequest2(0, 200);
    }

    if (plyr_act_wrk.timer == 15)
    {
        se_handle = SeStartFix(34, 0, 4096, 4096, 0);
    }

    if (plyr_act_wrk.timer == 20)
    {
        SeFadeFlame(se_handle, 10, 0);
    }

    plyr_act_wrk.timer++;

    return plyr_act_wrk.timer > 60;
}

char motPlyrActSowasowa(SgCOORDUNIT *cp)
{
    static char ofs = '\0';

    plyr_act_wrk.spd = 1.6f;
    plyr_act_wrk.move_type = 1;

    if (plyr_act_wrk.timer == ofs + 70)
    {
        plyr_act_wrk.trot[0] = motGetRandom(PI2 / 10.0f, PI / 10.0f);

        if (motGetRandom(1.0f, -1.0f) > 0)
        {
            plyr_act_wrk.trot[0] = -plyr_act_wrk.trot[0];
        }

        plyr_act_wrk.trot[1] = motGetRandom(PI2 / 50.0f, -PI2 / 50.0f);
    }
    else if (plyr_act_wrk.timer >= ofs + 70)
    {
        ofs = motGetRandom(30.0f, 0.0f);
        plyr_act_wrk.timer = 0;
    }

    plyr_act_wrk.trot[1] = 0.0f;
    plyr_act_wrk.timer++;

    return 0;
}

char motPlyrActKyoro(SgCOORDUNIT *cp)
{
    PLYR_FURI_DAT *dat;

    switch (plyr_act_wrk.act_no)
    {
        case 3:
            dat = plyr_act_furi0;
        break;
        case 4:
            dat = plyr_act_furi1;
        break;
        case 5:
            dat = plyr_act_furi2;
        break;
        case 6:
            if (plyr_act_wrk.timer == 0 && plyr_wrk.mode != 1 && ReqPlyrSpeAnime(68, 0) != 0)
            {
                plyr_wrk.mode = 10;
            }
            dat = plyr_act_furi3;
        break;
    }

    while (1)
    {
        if (plyr_act_wrk.timer > dat->timer)
        {
            dat++;

            if (dat->move_type == 0xffff)
            {
                return 1;
            }

            if (dat->move_type == 0xfffe)
            {
                plyr_act_wrk.timer = 0;
            }
        }
        else
        {
            plyr_act_wrk.timer++;
            plyr_act_wrk.trot[0] = dat->rot0;
            plyr_act_wrk.trot[1] = dat->rot1;
            plyr_act_wrk.move_type = dat->move_type;
            plyr_act_wrk.spd = dat->spd;

            break;
        }
    }

    return 0;
}

void motGetPlyrNeckRot(SgCOORDUNIT *cp, float *trot, float *tpos)
{
    sceVu0FVECTOR p;
    sceVu0FVECTOR d0;
    sceVu0FVECTOR d1;
    sceVu0FVECTOR d2;
    sceVu0FVECTOR n;
    sceVu0FVECTOR v0;
    sceVu0FVECTOR v1;
    float inner;

    sceVu0CopyVector(p, tpos);
    p[3] = 1.0f;

    // d1 = plyr_wrk.move_box.pos;

    sceVu0SubVector(d0, p, plyr_wrk.move_box.pos);

    sceVu0CopyVector(d1, cp[13].lwmtx[1]);

    sceVu0SubVector(d2, p, cp[13].lwmtx[3]);

    sceVu0CopyVector(v0, d0);
    sceVu0CopyVector(v1, d1);

    v0[1] = 0;
    v1[1] = 0;

    sceVu0Normalize(v0, v0);

    sceVu0ScaleVector(v1, v1, -1.0f);

    sceVu0Normalize(v1, v1);

    inner = sceVu0InnerProduct(v0, v1);

    if (inner > 0)
    {
        trot[0] = SgACosf(inner);

        sceVu0OuterProduct(n, v0, v1);

        if (n[1] < 0)
        {
            trot[0] = -trot[0];
        }

        if ((trot[0] > 1.0471976f) == 0)
        {
            if (trot[0] < -1.0471976f)
            {
                trot[0] = -1.0471976f;
            }
        }
        else
        {
            trot[0] = 1.0471976f;
        }

        sceVu0CopyVector(v0, d2);
        sceVu0CopyVector(v1, d2);

        v1[1] = 0;

        sceVu0Normalize(v0, v0);
        sceVu0Normalize(v1, v1);

        inner = sceVu0InnerProduct(v0, v1);

        trot[1] = SgACosf(inner);

        if (cp[13].lwmtx[3][1] < p[1])
        {
            trot[1] = -trot[1];
        }

        if ((trot[1] > 0.62831855f) == 0)
        {
            if (trot[1] < -0.20943952f)
            {
                trot[1] = -0.20943952f;
            }
        }
        else
        {
            trot[1] = 0.62831855f;
        }
    }
}

void InitQuake()
{
    quake.req = 0;
    quake.cnt = 0;
    quake.all_cnt = 0;
    quake.loop = 0;
    quake.flg = 0;
}

void StopQuake()
{
    quake.req = 0;

    ani_mdl[0].mot.reso = 1;

    motPlyrActStop();
}

void ReqQuake(u_int furn_id, float pow, u_int time, u_int loop, u_int stop_flg)
{
    quake.pow[0] = pow;
    quake.pow[1] = pow * 1.62f;
    quake.cnt = time;
    quake.all_cnt = time;
    quake.loop = loop;
    quake.req = 1;

    motPlyrActReq(3, NULL, 0);

    if (stop_flg == 1)
    {
        ani_mdl[0].mot.reso = 0;
    }

    SeStartPosSrundFlame(plyr_wrk.pr_info.room_no, camera.i, 0, 0x1000, 0x1000);
}

char QuakeCamera()
{
    float v0;
    float v1;
    float v2;
    sceVu0FVECTOR val[2];
    sceVu0FVECTOR xd;
    char i;
    QUAKE_CTRL *qk_p;
    float dat;

    qk_p = &quake;

    if (qk_p->req == 0)
    {
        return 0;
    }

    if (ingame_wrk.stts & 0x80 || ingame_wrk.stts & 0x10)
    {
        return 0;
    }

    if (ingame_wrk.stts & 0x20)
    {
        return 0;
    }

    if (qk_p->cnt >= qk_p->all_cnt)
    {
        qk_p->cnt--;
        return 0;
    }

    qk_p->cnt--;

    if (qk_p->cnt <= 0)
    {

        if (qk_p->loop == 0)
        {
            qk_p->req = 0;
            StopQuake();
            return 0;
        }
        else
        {
            qk_p->cnt = motGetRandom(600.0f, 1200.0f);
            qk_p->loop--;
            return 0;
        }
    }

    if (qk_p->cnt % 2)
    {
        VibrateRequest2(0, 160);
    }

    sceVu0OuterProduct(xd, camera.zd, camera.yd);

    for (i = 0; i < 2; i++)
    {
        dat = qk_p->pow[i];

        v0 = motGetRandom(dat, -dat);
        v1 = motGetRandom(dat, -dat);
        v2 = motGetRandom(dat, -dat);

        if (qk_p->cnt < 30)
        {
            v0 = v0 * (qk_p->cnt / 30.0f);
            v1 = v1 * (qk_p->cnt / 30.0f);
            v2 = v2 * (qk_p->cnt / 30.0f);
        }

        val[i][0] = (xd[0] * v0) + (camera.yd[0] * v1) + (camera.zd[0] * v2);
        val[i][1] = (xd[1] * v0) + (camera.yd[1] * v1) + (camera.zd[1] * v2);
        val[i][2] = (xd[2] * v0) + (camera.yd[2] * v1) + (camera.zd[2] * v2);
        val[i][3] = 0;
    }

    sceVu0AddVector(camera.p, camera.p, val[0]);
    sceVu0AddVector(camera.i, camera.i, val[1]);

    return 1;
}

float motGetRandom(float upper, float lower)
{
    return (rand() / (float)RAND_MAX) * (upper - lower) + lower;
}

float motLinearSupValue(float moto, float saki, u_char mode, u_int cnt, u_int all_cnt)
{
    u_int now_cnt;
    float val;
    float cnt_rate;
    float dv;

    now_cnt = all_cnt < cnt ? all_cnt : cnt;

    if (all_cnt == 0)
    {
        all_cnt++;
    }

    cnt_rate = (float)now_cnt / all_cnt;

    dv = saki - moto;

    switch (mode)
    {
    case 0:
        val = moto + dv * cnt_rate;
    break;
    case 1:
        val = moto + dv * (SgSinf(cnt_rate * PI_HALF + -PI_HALF) + 1);
    break;
    case 2:
        val = moto + dv * SgSinf(cnt_rate * PI_HALF);
    break;
    }

    return val;
}
