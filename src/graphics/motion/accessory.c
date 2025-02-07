#include "common.h"
#include "typedefs.h"
#include "accessory.h"

#include "sce/libvu0.h"

#include "os/pad.h"
#include "os/eeiop/eese.h"
#include "main/glob.h"
#include "ingame/map/furn_dat.h"
#include "ingame/camera/camera.h"
#include "ingame/event/ev_spcl.h"
#include "outgame/memory_album.h"
#include "graphics/graph2d/effect_ene.h"
#include "graphics/graph2d/effect_sub2.h"
#include "graphics/graph3d/sglight.h"
#include "graphics/graph3d/load3d.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/gra3d.h"
#include "graphics/graph3d/sgsu.h"
#include "graphics/graph3d/libsg.h"
#include "graphics/motion/mdlact.h"
#include "graphics/motion/mdlwork.h"
#include "graphics/motion/motion.h"
#include "graphics/motion/mdldat.h"
#include "graphics/motion/acs_dat.h"

// #include <cstdlib.h>
// RAND_MAX = (2**31-1)
#define RAND_MAX 2147483647

#define PI 3.1415927f
#define PI2 6.2831855f

void InitPlyrAcsAlpha()
{
    char i;
    ACS_ALPHA *acs_ctrl;

    for (i = 0; i < 2; i++)
    {
        acs_ctrl = &plyracs_ctrl[i];

        if (i == 0)
        {
            acs_ctrl->stat = 1;
            acs_ctrl->alpha = 0;
        }
        else
        {
            acs_ctrl->stat = 0;
            acs_ctrl->alpha = 0x7f;
        }
    }
}

void DispPlyrAcs(u_int *base_p, u_int *mdl_p, ACS_ALPHA *acs_ctrl, u_int bone_id)
{
    SgCOORDUNIT *cp;
    SgCOORDUNIT *cp2;
    HeaderSection *hs;
    HeaderSection *hs2;

    hs = (HeaderSection *)base_p;
    hs2 = (HeaderSection *)mdl_p;

    cp2 = hs2->coordp;
    cp = hs->coordp;

    cp2[0] = cp[bone_id];

    CalcCoordinate(cp2, hs2->blocks - 1);
    ManmdlSetAlpha(mdl_p, acs_ctrl->alpha);
    SgSortUnitKind(mdl_p, -1);
}

void PlyrAcsAlphaCtrl()
{
    u_int i;
    u_char spd[2] = {3, 2};
    u_char dspd[2] = {5, 2};
    ACS_ALPHA *acs_ctrl;

    if (ingame_wrk.stts & 0x10 || ingame_wrk.stts & 0x80)
    {
        return;
    }

    for (i = 0; i < 2; i++)
    {
        acs_ctrl = &plyracs_ctrl[i];

        if (acs_ctrl->stat == 0)
        {
            acs_ctrl->alpha += spd[i];

            if (acs_ctrl->alpha > 0x7f)
            {
                acs_ctrl->alpha = 0x7f;
            }
        }
        else
        {
            acs_ctrl->alpha -= dspd[i];

            if (acs_ctrl->alpha < 0)
            {
                acs_ctrl->alpha = 0;
            }
        }
    }
}

void acsInitRopeWork()
{
    u_int i;

    for (i = 0; i < 20; i++)
    {
        rope_ctrl[i].furn_id = 0xffff;

        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
    }
}

void acsRopeSetWork(u_int furn_id)
{
    u_int i;
    u_char acs_no;

    if ((furn_dat[furn_id].acs_flg & 0x40) == 0)
    {
        return;
    }

    acs_no = furn_dat[furn_id].acs_flg & 0xbf;

    for (i = 0; i < 20; i++)
    {
        if (rope_ctrl[i].furn_id == 0xffff)
        {
            acsInitRopeSub(i,furn_id,acs_no);
            break;
        }
    }
}

void acsRopeReleaseWork(u_int furn_id)
{
    u_int i;

    if ((furn_dat[furn_id].acs_flg & 0x40) == 0)
    {
        return;
    }

    for (i = 0; i < 20; i++)
    {
        if (rope_ctrl[i].furn_id == furn_id)
        {
            rope_ctrl[i].furn_id = 0xffff;
            break;
        }
    }
}

sceVu0FVECTOR* acsGetRopePos(u_int furn_id)
{
    u_int i;

    for (i = 0; i < 20; i++)
    {
        if (furn_id == rope_ctrl[i].furn_id)
        {
            return &rope_ctrl[i].top;
        }
    }

    return NULL;
}

void acsInitRopeSub(u_int work_id, u_int furn_id, u_int type)
{
    u_int i;
    u_int j;
    ROPE_CTRL *rope;
    sceVu0FVECTOR p1;
    sceVu0FVECTOR p2;
    sceVu0FVECTOR p3;
    sceVu0FVECTOR gravity;
    C_PARTICLE *current;

    rope = &rope_ctrl[work_id];
    rope->particle = rope_particle[work_id];
    rope->spring = rope_spring[work_id];
    rope->rdat = rope_tbl[type];
    rope->furn_id = furn_id;
    rope->stat = 1;
    rope->pow = 1.0f;
    rope->move_mode = 0;

    rope->top[0] = 0.0f;
    rope->top[1] = 0.0f;
    rope->top[2] = 0.0f;
    rope->top[3] = 0.0f;

    acsSetMoveDir(rope->dir);

    for (i = 0; !(rope->rdat->vtx[i] < 0.0f); i++) {
        rope->particle[i].p[0] = 0.0f;
        rope->particle[i].p[1] = rope->rdat->vtx[i];
        rope->particle[i].p[2] = 0.0f;
        rope->particle[i].p[3] = 0.0f;

        sceVu0CopyVector(rope->particle[i].old, rope->particle[i].p);
        sceVu0ScaleVector(rope->particle[i].p, rope->particle[i].p, 25.0f);
        sceVu0ScaleVector(rope->particle[i].old, rope->particle[i].old, 25.0f);

        rope->particle[i].v[0] = 0.0f;
        rope->particle[i].v[1] = 0.0f;
        rope->particle[i].v[2] = 0.0f;
        rope->particle[i].v[3] = 0.0f;

        rope->particle[i].old[3] = 1.0f;

        rope->particle[i].p[3] = 1.0f;
    }

    rope->spring_num = i - 1;
    rope->p_num = i;

    for (i = 0; i < rope->spring_num; i++)
    {
        rope->spring[i].p1 = i;
        rope->spring[i].p2 = i + 1;
        sceVu0CopyVector(p1, rope->particle[rope->spring[i].p1].p);
        sceVu0CopyVector(p2, rope->particle[rope->spring[i].p2].p);
        sceVu0SubVector(p3, p1, p2);
        rope->spring[i].ldef = SgSqrtf(sceVu0InnerProduct(p3, p3));
    }

    for (j = 0; j < 60; j++)
    {
        sceVu0FVECTOR d = {0.0f, 0.0f, 0.0f, 0.0f};
        sceVu0FVECTOR force;

        d[1] = rope->rdat->gravity;
        current = rope->particle;

        memcpy(gravity, d, sizeof(sceVu0FVECTOR));

        for (i = 0; i < rope->p_num; i++)
        {
            sceVu0AddVector(current[i].v, current[i].v, gravity);
            sceVu0AddVector(current[i].p, current[i].p, current[i].v);
        }

        for (i = 0; i < rope->spring_num; i++)
        {
            float r0;
            float dr;
            u_int id0;
            u_int id1;

            id0 = rope->spring[i].p1;
            id1 = rope->spring[i].p2;

            sceVu0SubVector(d, current[id0].p, current[id1].p);
            r0 = SgSqrtf(sceVu0InnerProduct(d, d));
            dr = rope->spring[i].ldef;
            sceVu0ScaleVector(force, d, ((dr - r0) * 0.5f) / r0);
            sceVu0AddVector(current[id0].p, current[id0].p, force);
            sceVu0SubVector(current[id1].p, current[id1].p, force);
        }

        for (i = 0; i < rope->rdat->fixed_num; i++)
        {
            current[i].p[0] = 0.0f;
            current[i].p[1] = rope->rdat->vtx[i];
            current[i].p[2] = 0.0f;
            current[i].p[3] = 1.0f;
        }

        for (i = 0; i < rope->p_num; i++)
        {
            sceVu0SubVector(current[i].v, current[i].p, current[i].old);
            sceVu0ScaleVector(current[i].v, current[i].v, rope->rdat->Kd);
        }

        for (i = 0; i < rope->p_num; i++)
        {
            sceVu0CopyVector(current[i].old, current[i].p);
        }
    }
}

void acsRopeMoveRequest(u_int furn_id, u_char move_mode, float pow)
{
    u_char i;

    for (i = 0; i < 20 ;i++)
    {
        if (furn_id == rope_ctrl[i].furn_id)
        {
            rope_ctrl[i].move_mode = move_mode;
            rope_ctrl[i].pow = pow;
            return;
        }
    }
}

void acsRopeMoveStop(u_int furn_id)
{
    u_char i;

    for (i = 0; i < 20; i++)
    {
        if (furn_id == rope_ctrl[i].furn_id)
        {
            rope_ctrl[i].move_mode = 0;
            return;
        }
    }
}

u_char acsCheckRopeMoveExec(u_int furn_id)
{
    u_char i;

    for (i = 0; i < 20; i++)
    {
        if (furn_id == rope_ctrl[i].furn_id)
        {
            if (rope_ctrl[i].move_mode != 0)
            {
                return 1;
            }

            return 0;
        }
    }

    return 0;
}

void acsCalcCoordinate(SgCOORDUNIT* cp, u_int block_num, FURN_WRK* f_wrk, ROPE_CTRL* rope)
{
    float grot;
    u_int i;
    sceVu0FVECTOR trans[10];
    sceVu0FVECTOR x = {1.0f, 0.0f, 0.0f, 0.0f};
    sceVu0FVECTOR y = {0.0f, 1.0f, 0.0f, 0.0f};
    sceVu0FVECTOR z = {0.0f, 0.0f, 1.0f, 0.0f};
    sceVu0FMATRIX base;

    sceVu0UnitMatrix(base);

    grot = f_wrk->rotate[1] + PI;
    base[0][0] = base[1][1] = base[2][2] = 25.0f;

    if (PI < grot)
    {
        grot -= PI2;
    }

    sceVu0RotMatrixX(base, base, PI);
    sceVu0RotMatrixY(base, base, grot);
    sceVu0RotMatrixX(base, base, f_wrk->rotate[0]);
    sceVu0RotMatrixZ(base, base, f_wrk->rotate[2]);
    sceVu0CopyMatrix(cp->matrix, base);
    Vu0CopyVector(cp->matrix[3], f_wrk->pos);
    cp->matrix[3][3] = 1.0f;

    for (i = 0; i < rope->spring_num; i++)
    {
        sceVu0UnitMatrix(cp[i+1].matrix);
    }

    for (i = 0; i < rope->p_num; i++)
    {
        sceVu0CopyVector(trans[i], rope->particle[i].p);
    }

    for (i = 0; i < rope->spring_num; i++)
    {
        sceVu0FVECTOR vec;

        sceVu0SubVector(vec, trans[i+1], trans[i]);
        sceVu0Normalize(cp[i+1].matrix[0], vec);
    }

    for (i = 0; i < rope->spring_num; i++)
    {
        sceVu0FMATRIX m;
        sceVu0FVECTOR axis;

        sceVu0UnitMatrix(m);
        sceVu0OuterProduct(axis, x, cp[i+1].matrix[0]);
        sceVu0Normalize(axis, axis);
        GetMatrixRotateAxis(m, axis, SgACosf(sceVu0InnerProduct(x, cp[i+1].matrix[0])));
        sceVu0ApplyMatrix(cp[i+1].matrix[1], m, y);
        sceVu0ApplyMatrix(cp[i+1].matrix[2], m, z);
        sceVu0MulMatrix(cp[i+1].matrix, cp[i+1].matrix, base);
        sceVu0TransMatrix(cp[i+1].matrix, cp[i+1].matrix, f_wrk->pos);
        sceVu0TransMatrix(cp[i+1].matrix, cp[i+1].matrix, trans[i]);
    }

    for (i = 0; i < block_num; i++)
    {
        SetLWS2(cp + i, &camera);
    }
}

void acsSetMoveDir(float *dir)
{
    dir[0] = (float)(rand() - (int)(RAND_MAX / 2)) / (float)RAND_MAX;
    dir[1] = (float)(rand() - (int)(RAND_MAX / 2)) / (float)RAND_MAX;
    dir[2] = (float)(rand() - (int)(RAND_MAX / 2)) / (float)RAND_MAX;
    dir[3] = 0.0f;

    sceVu0Normalize(dir, dir);
}

void acsRopeMoveCtrl(ROPE_CTRL *rope)
{
    switch(rope->move_mode)
    {
    case 0:
        // do nothing
    break;
    case 1:
        acsRopeMoveWind(rope, 0);
    break;
    case 2:
        acsRopeMoveWind(rope, 1);
    break;
    case 3:
        acsRopeMoveVib(rope);
    break;
    case 4:
        // do nothing
    break;
    }
}

void acsRopeMoveWind(ROPE_CTRL* rope, char dir_cng)
{
    sceVu0FVECTOR wind;
    C_PARTICLE* current;
    u_int i;
    char cnt;
    char flg; // UNUSED!

    cnt = 0;
    current = rope->particle;

    if (dir_cng != 0)
    {
        if (sys_wrk.count % 180 == 0)
        {
            if (motGetRandom(1.0f, -1.0f) > 0.0f)
            {
                acsSetMoveDir(rope->dir);
            }
        }
    }

    if (!(rope->pow < 0.2f))
    {
        switch (rope->furn_id)
        {
        case 177:
            rope->pow = 0.07f;
        break;
        case 178:
            rope->pow = 0.07f;
            cnt = 5;
        break;
        case 209:
            rope->pow = 0.08f;
        break;
        case 264:
            rope->pow = 0.0f;
            cnt = 10;
        break;
        case 293:
            rope->pow = 0.04f;
            cnt = 10;
        break;
        }
    }

    sceVu0ScaleVector(wind, rope->dir, rope->pow);

    if (sys_wrk.count % 120 < cnt + 40)
    {
        for (i = 0; i < rope->p_num; i++)
        {
            sceVu0AddVector(current[i].v, current[i].v, wind);
        }
    }
}

void acsRopeMoveVib(ROPE_CTRL *rope)
{
    sceVu0FVECTOR wind;
    C_PARTICLE *current;
    u_int i;

    current = rope->particle;

    acsSetMoveDir(rope->dir);

    sceVu0ScaleVector(wind,rope->dir, rope->pow);

    for (i = rope->p_num - 2; i < rope->p_num; i++)
    {
        sceVu0AddVector(current[i].v, current[i].v, wind);
    }
}

void acsRopeMoveFall(ROPE_CTRL *rope)
{
    C_PARTICLE *current;
    u_int i;

    i = rope->p_num - 1;
    current = &rope->particle[i];

    sceVu0CopyVector(current->p, rope->particle->p);

    current->p[0] += 1.0f;
    current->p[1] -= 200.0f;
}

void acsMoveRope(ROPE_CTRL *rope, SgCOORDUNIT *cp, COLLISION_DAT *collision, sceVu0FMATRIX l2w_inscale)
{
    u_int i;
    u_int j;
    sceVu0FMATRIX m;
    sceVu0FMATRIX m1;
    sceVu0FMATRIX w2c[5];
    sceVu0FMATRIX l2c[5];
    sceVu0FMATRIX l2w;
    sceVu0FMATRIX w2l;
    sceVu0FVECTOR gravity;
    C_PARTICLE* current;
    ROPE_DAT* rdat;
    u_int fixed_num;
    sceVu0FVECTOR d;
    sceVu0FVECTOR force;
    float r0;
    float dr;
    u_int id0;
    u_int id1;
    sceVu0FVECTOR p;
    sceVu0FVECTOR v;
    sceVu0FVECTOR p1;
    sceVu0FVECTOR v1;
    u_char hit;

    rdat = rope->rdat;
    fixed_num = rdat->fixed_num;
    current = rope->particle;

    if (rope->stat == 0 || ingame_wrk.stts & 0x10 || ingame_wrk.stts & 0x80)
    {
        return;
    }

    sceVu0CopyMatrix(l2w, l2w_inscale);
    sceVu0Normalize(l2w[0], l2w[0]);
    sceVu0Normalize(l2w[1], l2w[1]);
    sceVu0Normalize(l2w[2], l2w[2]);
    sceVu0InversMatrix(w2l, l2w);
    sceVu0CopyMatrix(m, l2w);

    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;

    sceVu0TransposeMatrix(m1, m);

    gravity[0] = 0.0f;
    gravity[1] = rdat->gravity;
    gravity[2] = 0.0f;
    gravity[3] = 0.0f;

    sceVu0ApplyMatrix(gravity, m1, gravity);

    for (i = 0; i < rope->p_num; i++)
    {
        sceVu0ApplyMatrix(current[i].p, m1, current[i].p);
        sceVu0ApplyMatrix(current[i].v, m1, current[i].v);
    }

    acsRopeMoveCtrl(rope);

    for (i = 0; i < rope->p_num; i++)
    {
        sceVu0AddVector(current[i].v, current[i].v, gravity);
        sceVu0AddVector(current[i].p, current[i].p, current[i].v);
    }

    for (i = 0; i < rope->spring_num; i++)
    {
        id0 = rope->spring[i].p1;
        id1 = rope->spring[i].p2;
        sceVu0SubVector(d, current[id0].p, current[id1].p);
        r0 = SgSqrtf(sceVu0InnerProduct(d, d));
        dr = rope->spring[i].ldef;
        sceVu0ScaleVector(force, d, ((dr - r0) * 0.5f) / r0);
        sceVu0AddVector(current[id0].p, current[id0].p, force);
        sceVu0SubVector(current[id1].p, current[id1].p, force);
    }

    for (i = 0; collision[i].dat != NULL && i < 5; i++)
    {
        sceVu0CopyMatrix(l2c[i], cp[collision[i].bone_id].lwmtx);
        sceVu0Normalize(l2c[i][0], l2c[i][0]);
        sceVu0Normalize(l2c[i][1], l2c[i][1]);
        sceVu0Normalize(l2c[i][2], l2c[i][2]);
        sceVu0InversMatrix(w2c[i], l2c[i]);
    }

    for (i = 0; i < rope->p_num; i++)
    {
        current[i].v[3] = 0.0f;
        current[i].p[3] = 1.0f;

        sceVu0ApplyMatrix(p, l2w, current[i].p);
        sceVu0ApplyMatrix(v, l2w, current[i].v);

        for (j = 0; collision[j].dat != NULL && j < 5; j++)
        {
            sceVu0ApplyMatrix(p1, w2c[j], p);
            sceVu0ApplyMatrix(v1, w2c[j], v);

            if (collision[j].type == 0)
            {
                hit = acsCheckCollisionSphere((SPHERE*)collision[j].dat, p1, v1, rdat->Ke);
            } else
            {
                hit = acsCheckCollisionTube((TUBE*)collision[j].dat, p1, v1, rdat->Ke);
            }

            if (hit != 0x0)
            {
                sceVu0ApplyMatrix(p1, l2c[j], p1);
                sceVu0ApplyMatrix(current[i].p, w2l, p1);
            }
        }
    }

    for (i = 0; i < rope->p_num; i++)
    {
        sceVu0ApplyMatrix(current[i].p, m, current[i].p);
        sceVu0ApplyMatrix(current[i].v, m, current[i].v);
    }

    for (i = 0; i < fixed_num; i++)
    {
        current[i].p[0] = 0.0f;
        current[i].p[1] = rdat->vtx[i];
        current[i].p[2] = 0.0f;
        current[i].p[3] = 1.0f;
    }

    if (rope->move_mode == 4)
    {
        acsRopeMoveFall(rope);
    }

    for (i = 0; i < rope->p_num; i++)
    {
        sceVu0SubVector(current[i].v, current[i].p, current[i].old);
        sceVu0ScaleVector(current[i].v, current[i].v, rdat->Kd);
    }

    for (i = 0; i < rope->p_num; i++)
    {
        sceVu0CopyVector(current[i].old, current[i].p);
    }
}

u_int* acsInitCloth(CLOTH_CTRL *cloth_top, u_int *mpk_p, u_int *top_addr, u_int mdl_no)
{
    u_int i;
    CLOTH *dat;
    CLOTH_CTRL *cloth;
    PHEAD *ph;
    sceVu0FVECTOR *vtx;
    u_int *addr;
    u_int div;
    u_int div1;
    u_int div2;
    u_int cloth_no;

    addr = top_addr;

    cloth_no = 0;

    if (manmdl_dat[mdl_no].cdat == NULL)
    {
        return addr;
    }

    while (1)
    {
        cloth = cloth_top + cloth_no;
        cloth->cdat = &manmdl_dat[mdl_no].cdat[cloth_no];
        dat = cloth->cdat->dat;

        if (dat == NULL)
        {
            break;
        }

        cloth_no++;
        cloth->w = cloth->cdat->dat->w;
        cloth->h = cloth->cdat->dat->h;
        cloth->reset_flg = 0x3;

        ph = GetFileInPak(mpk_p, cloth->cdat->sgd_id);
        vtx = (sceVu0FVECTOR *)ph->pUniqList[2];

        if (dat->type == 0)
        {
            cloth->p_num = cloth->w * cloth->h;
            cloth->spring_num = cloth->w * (cloth->h - 1) + (cloth->w - 1) * cloth->h;
            div = cloth->w * (cloth->h - 1);
        }
        else if (dat->type == 1 || dat->type == 2)
        {
            cloth->p_num = cloth->w * cloth->h * 2;
            cloth->spring_num = cloth->w * (cloth->h - 1) + (cloth->w - 1) * cloth->h;
            div = cloth->w * (cloth->h - 1);
            div1 = cloth->spring_num;
            cloth->spring_num = div1 * 2;
            div2 = cloth->spring_num;
            cloth->spring_num += cloth->w * cloth->h;

        }
        else if (dat->type == 4)
        {
            cloth->p_num = cloth->w * cloth->h;
            cloth->spring_num = (cloth->h - 1 + cloth->h) * cloth->w;
            div = cloth->w * (cloth->h - 1);
            div1 = div + (cloth->w - 1) * cloth->h;
        }
        else
        {
            cloth->p_num = cloth->w * cloth->h;
            cloth->spring_num = ((cloth->h - 1) + cloth->h) * cloth->w;
            cloth->spring_num += (cloth->w * 2 * (cloth->h - 1));
            div = cloth->w * (cloth->h - 1);
            div1 = div + ((cloth->w - 1) * cloth->h);
            div2 = (cloth->h - 1 + cloth->h) * cloth->w;
        }

        cloth->particle = (C_PARTICLE *)motAlign128(addr);
        cloth->spring = (SPRING *)&cloth->particle[cloth->p_num];
        addr = (u_int *)&cloth->spring[cloth->spring_num];
        addr = motAlign128(addr);

        for (i = 0; i < cloth->p_num; i++)
        {
            sceVu0CopyVector(cloth->particle[i].p, *vtx);
            cloth->particle[i].p[3] = 1.0f;
            vtx++;

            sceVu0CopyVector(cloth->particle[i].old, cloth->particle[i].p);

            cloth->particle[i].v[0] = 0.0f;
            cloth->particle[i].v[1] = 0.0f;
            cloth->particle[i].v[2] = 0.0f;
            cloth->particle[i].v[3] = 0.0f;

            cloth->particle[i].f[0] = 0.0f;
            cloth->particle[i].f[1] = 0.0f;
            cloth->particle[i].f[2] = 0.0f;
            cloth->particle[i].f[3] = 0.0f;
        }

        if (dat->type == 0)
        {
            for (i = 0; i < cloth->spring_num; i++)
            {
                u_char id1;
                u_char id2;
                u_char j;

                if (i < div)
                {
                    id1 = (i % (cloth->h - 1)) + (i / (cloth->h - 1)) * cloth->h;
                    id2 = id1 + 1;
                }
                else
                {
                    j = i - div;
                    id1 = (j % (cloth->w - 1)) * cloth->h + (j / (cloth->w - 1));
                    id2 = id1 + cloth->h;
                }

                cloth->spring[i].p1 = id1;
                cloth->spring[i].p2 = id2;
            }

        }
        else if (dat->type == 1 || dat->type == 2)
        {
            for (i = 0; i < cloth->spring_num; i++)
            {
                u_char id1;
                u_char id2;
                u_char j;

                if (i < div)
                {
                    id1 = (i % (cloth->h - 1)) + (i / (cloth->h - 1)) * cloth->h;
                    id2 = id1 + 1;
                }
                else if (i < div1)
                {
                    j = i - div;
                    id1 = (j % (cloth->w - 1)) * cloth->h + (j / (cloth->w - 1));
                    id2 = id1 + cloth->h;
                }
                else if (i < div1 + div)
                {
                    j = ((i - div1) % (cloth->h - 1)) + ((i - div1) / (cloth->h - 1)) * cloth->h;
                    id1 = j + cloth->w * cloth->h;
                    id2 = id1 + 1;
                }
                else if (i < div2)
                {
                    j = (i - div1) - div;
                    j = (j % (cloth->w - 1)) * cloth->h + (j / (cloth->w - 1));
                    id1 = j + cloth->w * cloth->h;
                    id2 = id1 + cloth->h;
                }
                else
                {
                    id1 = i - div2;
                    id2 = id1 + cloth->w * cloth->h;
                }

                cloth->spring[i].p1 = id1;
                cloth->spring[i].p2 = id2;
            }

        }
        else if (dat->type == 4)
        {
            for (i = 0; i < cloth->spring_num; i++)
            {
                u_char id1;
                u_char id2;
                u_char j;

                if (i < div)
                {
                    id1 = i % (cloth->h - 1) + (i / (cloth->h - 1)) * cloth->h;
                    id2 = id1 + 1;
                }
                else if (i < div1)
                {
                    j = i - div;
                    id1 = (j % (cloth->w - 1)) * cloth->h + j / (cloth->w - 1);
                    id2 = ((id1 + cloth->h) % cloth->p_num);
                }
                else
                {
                    id1 = i - div1;
                    id2 = id1 + cloth->h * (cloth->w - 1);
                }

                cloth->spring[i].p1 = id1;
                cloth->spring[i].p2 = id2;
            }
        }
        else
        {
            for (i = 0; i < cloth->spring_num; i++)
            {
                u_char id1;
                u_char id2;
                u_char j;

                if (i < div)
                {
                    id1 = (i % (cloth->h - 1)) + (i / (cloth->h - 1)) * cloth->h;
                    id2 = id1 + 1;
                }
                else if (i < div1)
                {
                    j = i - div;
                    id1 = (j % (cloth->w - 1)) * cloth->h + j / (cloth->w - 1);
                    id2 = ((id1 + cloth->h) % cloth->p_num);
                }
                else if (i < div2)
                {
                    id1 = i - div1;
                    id2 = id1 + cloth->h * (cloth->w - 1);
                }
                else
                {
                    u_int ofs[2] = {cloth->h + 1, cloth->h - 1};

                    j = i - div2;
                    id1 = cloth->h * (j % (cloth->w * 2) / 2) + (j % 2) + j / (cloth->w * 2);
                    id2 = id1 + ofs[j % 2];
                    id2 = id2 % cloth->p_num;
                }

                cloth->spring[i].p1 = id1;
                cloth->spring[i].p2 = id2;
            }
        }
    }

    return addr;
}

void acsClothCtrl(ANI_CTRL* ani_ctrl, u_int* mpk_p, u_int mdl_no, u_char scene_flg) {
    u_int i;
    u_int limit;
    CLOTH_CTRL* cloth;
    SgCOORDUNIT* cp;
    sceVu0FVECTOR* vtx;
    PHEAD* ph;
    u_int no;
    static sceVu0FVECTOR pbak = {0.0f, 0.0f, 0.0f, 0.0f};
    sceVu0FVECTOR sub;
    sceVu0FVECTOR pos;
    float val;
    float* v1;
    CLOTH_DAT* cdat;
    COLLISION_DAT* collision;
    float scale;

    cloth = ani_ctrl->cloth_ctrl;
    limit = 0;

    if (manmdl_dat[mdl_no].cdat == NULL || ingame_wrk.stts & 0x10 || ingame_wrk.stts & 0x80)
    {
        return;
    }

    no = 0;
    cp = (SgCOORDUNIT*)mpk_p[10];

    Vu0CopyVector(pos, cp[manmdl_dat[mdl_no].waist_id].lwmtx[3]);
    sceVu0SubVector(sub, pos, ani_ctrl->pbak);
    scale = SgSqrtf(sceVu0InnerProduct(sub, sub));
    sceVu0CopyVector(ani_ctrl->pbak, pos);

    if (100.0f < scale)
    {
        limit = 1;
    }

    while (1)
    {
        collision = manmdl_dat[mdl_no].collision;
        scale = manmdl_dat[mdl_no].scale;
        cdat = cloth[no].cdat;

        if (cdat->dat == NULL)
        {
            return;
        }

        ph = GetFileInPak(mpk_p, cdat->sgd_id);
        vtx = (sceVu0FVECTOR*)ph->pUniqList[2];

        if (scene_flg != 0)
        {
            scale = -scale;
        }

        if (cloth[no].reset_flg & 1)
        {
            for (i = 0; i < 60; i++)
            {
                acsMoveCloth(vtx, &cloth[no], cp, collision, scale, 1.0f, mdl_no);
            }

            cloth[no].reset_flg &= ~1;
        }
        else
        {
            if (limit)
            {
                cloth[no].reset_flg |= 4;
            }

            acsMoveCloth(vtx, &cloth[no], cp, collision, scale, 1.0f, mdl_no);
        }

        no++;
    }
}

void acsMoveCloth(sceVu0FVECTOR *vtx, CLOTH_CTRL *cloth, SgCOORDUNIT *cp, COLLISION_DAT *collision, float scale, float collision_scale, u_int mdl_no)
{
#ifdef MATCHING_DECOMP
// fixes syntax errors for new compilers/type checkers
#define Restrict restrict
#endif
    u_int i;
    u_int j;
    u_int k;
    sceVu0FVECTOR gravity;
    C_PARTICLE *current = cloth->particle;
    SPRING *spring = cloth->spring;
    CLOTH *dat;
    sceVu0FMATRIX l2w;
    sceVu0FMATRIX w2l;
    sceVu0FMATRIX l2w_rist;
    char Restrict[121] = {0};
    sceVu0FVECTOR ldef;
    sceVu0FVECTOR d;
    sceVu0FVECTOR force;
    float r0;
    float dr;
    u_int id0;
    u_int id1;
    sceVu0FMATRIX w2c[5];
    sceVu0FMATRIX c2w[5];
    sceVu0FVECTOR p;
    sceVu0FVECTOR v;
    sceVu0FVECTOR p1;
    sceVu0FVECTOR v1;

    if (cloth->cdat == NULL)
    {
        return;
    }

    dat = cloth->cdat->dat;

    sceVu0CopyMatrix(l2w, cp[cloth->cdat->bone_id].lwmtx);

    if (dat->type == 1)
    {
        sceVu0CopyMatrix(l2w_rist, cp[cloth->cdat->bone_id2].lwmtx);
    }

    gravity[0] = 0.0f;
    gravity[1] = dat->gravity * scale;
    gravity[2] = 0.0f;
    gravity[3] = 0.0f;

    motSetInvMatrix(w2l, l2w);

    if (cloth->reset_flg & 0x4 || cloth->reset_flg & 0x2)
    {
        for (i = 0; i < cloth->p_num; i++)
        {
            sceVu0ApplyMatrix(current[i].p, l2w, vtx[i]);
            sceVu0CopyVector(current[i].old, current[i].p);

            current[i].v[0] = 0.0f;
            current[i].v[1] = 0.0f;
            current[i].v[2] = 0.0f;
            current[i].v[3] = 0.0f;

        }

        if (cloth->reset_flg & 2)
        {
            for (i = 0; i < cloth->spring_num; i++)
            {
                sceVu0SubVector(ldef, cloth->particle[cloth->spring[i].p1 & 0xff].p, cloth->particle[cloth->spring[i].p2 & 0xff].p);
                r0 = SgSqrtf(sceVu0InnerProduct(ldef, ldef));
                cloth->spring[i].ldef = r0;
            }
        }

        cloth->reset_flg &= 0xf9;
    }

    for (k = 0; k < 2; k++)
    {
        for (i = 0; i < cloth->p_num; i++)
        {
            sceVu0AddVector(current[i].v, current[i].v, gravity);
            sceVu0AddVector(current[i].p, current[i].p, current[i].v);
        }

        for (i = 0; i <  cloth->spring_num; i++)
        {
            id0 = spring[i].p1;
            id1 = spring[i].p2;
            sceVu0SubVector(d, current[id0].p, current[id1].p);
            r0 = SgSqrtf(sceVu0InnerProduct(d, d));
            dr = spring[i].ldef;
            sceVu0ScaleVector(force, d, ((dr - r0) * 0.5f) / r0);
            sceVu0AddVector(current[id0].p, current[id0].p, force);
            sceVu0SubVector(current[id1].p, current[id1].p, force);

        }

        if (k == 1 && cloth->cdat->flg != 0)
        {
            for (i = 0; collision[i].dat != NULL && i < 5; i++)
            {
                sceVu0CopyMatrix(c2w[i], cp[collision[i].bone_id].lwmtx);
                sceVu0Normalize(c2w[i][0], c2w[i][0]);
                sceVu0Normalize(c2w[i][1], c2w[i][1]);
                sceVu0Normalize(c2w[i][2], c2w[i][2]);
                sceVu0ScaleVector(c2w[i][0], c2w[i][0], collision_scale);
                sceVu0ScaleVector(c2w[i][1], c2w[i][1], collision_scale);
                sceVu0ScaleVector(c2w[i][2], c2w[i][2], collision_scale);
                motSetInvMatrix(w2c[i], c2w[i]);
            }

            for (i = 0; i < cloth->p_num; i++)
            {
                sceVu0CopyVector(p, current[i].p);
                sceVu0CopyVector(v, current[i].v);

                for (j = 0; collision[j].dat != NULL && j < 5; j++)
                {
                    u_char hit;

                    sceVu0ApplyMatrix(p1, w2c[j], p);
                    sceVu0ApplyMatrix(v1, w2c[j], v);


                    if (collision[j].type == 0)
                    {
                        hit = acsCheckCollisionSphere((SPHERE *)collision[j].dat, p1, v1, dat->Ke);
                    }
                    else
                    {
                        hit = acsCheckCollisionTube((TUBE *)collision[j].dat, p1, v1, dat->Ke);
                    }

                    if (hit != 0)
                    {
                        sceVu0ApplyMatrix(current[i].p, c2w[j], p1);
                    }
                }
            }
        }

        if (dat->type == 0)
        {
            u_short id0;
            u_short id1;

            if (mdl_no == 15)
            {
                id0 = cloth->h - 1;
                id1 = cloth->h - 2;
                for (i = 0; i < cloth->w; i++)
                {
                    sceVu0ApplyMatrix(current[(i * cloth->h) + id0].p, l2w, vtx[(i * cloth->h) + id0]);
                    sceVu0ApplyMatrix(current[(i * cloth->h) + id1].p, l2w, vtx[(i * cloth->h) + id1]);
                    Restrict[(i * cloth->h) + id0] = 1;
                    Restrict[(i * cloth->h) + id1] = 1;
                }
            }
            else
            {
                id0 =  cloth->h - 1;
                for (i = 0; i < cloth->w; i++)
                {
                    sceVu0ApplyMatrix(current[(i * cloth->h) + id0].p, l2w, vtx[(i * cloth->h) + id0]);
                    Restrict[(i * cloth->h) + id0] = 1;
                }
            }
        }
        else if (dat->type == 1)
        {
            u_short cnt;
            u_short id;

            cnt = 0;

            for (i = 0; i < cloth->w * 2; i++)
            {
                id =  (i * cloth->h) + (cloth->h - 1);

                if ((cloth->w - 2) <= i && (cloth->w > i) || ((cloth->w * 2) - 2) <= i)
                {
                    if (cloth->cdat->rist_vtx != NULL)
                    {
                        sceVu0ApplyMatrix(current[id].p, l2w_rist, cloth->cdat->rist_vtx[cnt]);
                        cnt++;
                    }
                }
                else
                {
                    sceVu0ApplyMatrix(current[id].p, l2w, vtx[id]);
                    Restrict[id] = 1;
                }
            }
        }
        else if (dat->type == 2)
        {
            u_short id;

            for (i = 0; i < cloth->w * 2; i++)
            {
                id = i * cloth->h + (cloth->h - 1);
                sceVu0ApplyMatrix(current[id].p, l2w, vtx[id]);
                Restrict[id] = 1;
            }
        }
        else
        {
            u_short id;

            id = cloth->h - 1;
            for (i = 0; i < cloth->w; i++)
            {
                sceVu0ApplyMatrix(current[(i * cloth->h) + id].p, l2w, vtx[(i * cloth->h) + id]);
                Restrict[(i * cloth->h) + id] = 1;
            }
        }

        for (i = 0; i < cloth->p_num; i++)
        {
            sceVu0SubVector(current[i].v, current[i].p, current[i].old);
            sceVu0ScaleVector(current[i].v, current[i].v, dat->Kd);
        }

        for (i = 0; i < cloth->p_num; i++)
        {
            sceVu0CopyVector(current[i].old, current[i].p);
        }
    }

    for (i = 0; i < cloth->p_num; i++)
    {
        if (Restrict[i] == 0)
        {
            sceVu0ApplyMatrix(vtx[i], w2l, current[i].p);
        }
    }
}

u_char acsCheckCollisionSphere(SPHERE *sphere, float *current, float *relative_v, float Ke)
{
    float r1;
    float r2;
    sceVu0FVECTOR tmp;
    sceVu0FVECTOR v;
    sceVu0FVECTOR v1;

    r1 = sphere->r;

    tmp[0] = current[0] - sphere->center[0];
    tmp[1] = current[1] - sphere->center[1];
    tmp[2] = current[2] - sphere->center[2];

    r2 = SgSqrtf(sceVu0InnerProduct(tmp, tmp));

    if (r2 < r1)
    {
        sceVu0Normalize(v, tmp);
        sceVu0ScaleVector(v, v, r1 - r2);

        if (Ke > 0.00001f)
        {
            sceVu0CopyVector(v1, relative_v);
            sceVu0ScaleVector(v1, v1, -Ke);
            sceVu0AddVector(v, v, v1);
        }

        sceVu0AddVector(current, current, v);

        return 1;
    }

    return 0;
}

u_char acsCheckCollisionTube(TUBE *tube, float *current, float *relative_v, float Ke)
{
    sceVu0FVECTOR dir0;
    sceVu0FVECTOR dir1;
    sceVu0FVECTOR v0;
    sceVu0FVECTOR v1;
    sceVu0FVECTOR v2;
    sceVu0FVECTOR n;
    sceVu0FVECTOR PL;
    sceVu0FVECTOR AP;
    sceVu0FVECTOR PA;
    sceVu0FVECTOR P1P0;
    sceVu0FVECTOR normal;
    sceVu0FVECTOR p0;
    sceVu0FVECTOR p1;
    float dist;
    float inner;

    p0[0] = tube->p0[0];
    p0[1] = tube->p0[1];
    p0[2] = tube->p0[2];
    p0[3] = 1.0f;

    p1[0] = tube->p1[0];
    p1[1] = tube->p1[1];
    p1[2] = tube->p1[2];
    p1[3] = 1.0f;

    sceVu0SubVector(v0, p0, p1);
    sceVu0SubVector(dir0, current, p0);

    if (sceVu0InnerProduct(dir0, v0) > 0.0f)
    {
        return 0;
    }

    sceVu0SubVector(dir1, current, p1);
    sceVu0ScaleVector(v0, v0, -1.0f);

    if (sceVu0InnerProduct(dir1, v0) > 0.0f)
    {
        return 0;
    }

    sceVu0SubVector(PA, current, p0);
    sceVu0SubVector(P1P0, p1, p0);
    sceVu0Normalize(n, P1P0);
    inner = sceVu0InnerProduct(P1P0, P1P0);
    dist = sceVu0InnerProduct(n, PA) / SgSqrtf(inner);
    sceVu0ScaleVector(v2, P1P0, dist);
    sceVu0ScaleVector(AP, PA, -1.0f);
    sceVu0AddVector(normal, v2, AP);
    inner = sceVu0InnerProduct(normal, normal);
    dist = SgSqrtf(inner);

    if (dist < tube->r)
    {
        sceVu0FVECTOR n;
        sceVu0FVECTOR v;
        float ydif;
        float rdif;
        u_int axis;

        axis = tube->axis;
        rdif = tube->r - dist;

        if (current[axis] > (p1[axis] + p0[axis]) * 0.5f)
        {
            ydif = p1[axis] - current[axis];
        }
        else
        {
            ydif = p0[axis] - current[axis];
        }

        if (rdif < __builtin_fabsf(ydif)) {
            sceVu0Normalize(n, normal);
            sceVu0ScaleVector(n, n, -rdif);
            sceVu0AddVector(current, current, n);
        }
        else
        {
            if (current[axis] > (p1[axis] + p0[axis]) * 0.5f)
            {
                current[axis] = p1[axis];
            }
            else
            {
                current[axis] = p0[axis];
            }
        }

        sceVu0ScaleVector(v, relative_v, -Ke);
        sceVu0AddVector(current, current, v);

        return 1;
    }

    return 0;
}

void SetLWS2(SgCOORDUNIT *cp, SgCAMERA *camera)
{
    sceVu0CopyMatrix(cp->lwmtx, cp->matrix);
    sceVu0MulMatrix(cp->workm, camera->ws, cp->lwmtx);

    cp->flg = 1;
}

void acsInitChodoWork()
{
    u_int i;

    for (i = 0; i < 10; i++)
    {
        cmove_ctrl[i].furn_id = 0xffff;
        cmove_ctrl[i].req = 0;

        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
    }
}

void acsChodoSetWork(u_int furn_id)
{
    u_int i;
    u_char acs_no;

    acs_no = furn_dat[furn_id].acs_flg;

    if ((acs_no & 0x20) == 0)
    {
        return;
    }

    acs_no &= 0xdf;

    for (i = 0; i < 10; i++)
    {
        if (cmove_ctrl[i].furn_id == 0xffff)
        {
            cmove_ctrl[i].furn_id = furn_id;
            cmove_ctrl[i].type = acs_no;
            cmove_ctrl[i].timer = 0;
            cmove_ctrl[i].req =  0;
            return;
        }
    }
}

void acsChodoReleaseWork(u_int furn_id)
{
    u_int i;

    if ((furn_dat[furn_id].acs_flg & 0x20) == 0)
    {
        return;
    }

    for (i = 0; i < 10; i++)
    {
        if (cmove_ctrl[i].furn_id == furn_id)
        {
            cmove_ctrl[i].furn_id = 0xffff;
            cmove_ctrl[i].req = 0;
            break;
        }
    }
}

void acsChodoMoveRequest(u_int furn_id)
{
    u_char i;

    for (i = 0; i < 10; i++)
    {
        if (furn_id == cmove_ctrl[i].furn_id)
        {
            cmove_ctrl[i].req = 1;
            cmove_ctrl[i].timer = 0;
            cmove_ctrl[i].alpha = 0x7f;
            break;
        }
    }
}

void acsChodoMoveStop(u_int furn_id)
{
    u_char i;

    for (i = 0; i < 10; i++)
    {
        if (furn_id == cmove_ctrl[i].furn_id)
        {
            cmove_ctrl[i].req = 0;
            return;
        }
    }
}

void acsChodoMove(CMOVE_CTRL *mv, u_int *sgd_top)
{
    sceVu0FVECTOR p;
    SgCOORDUNIT *cp;
    HeaderSection *hs;

    hs = (HeaderSection *)sgd_top;
    cp = hs->coordp;

    ManmdlSetAlpha(sgd_top, 0x7f);

    if (mv->req == 0)
    {
        return;
    }

    switch (mv->type)
    {
    case 1:
        if (acsChodoMoveBird(cp, p, mv) != 0)
        {
            sceVu0CopyVector(cp->matrix[3], p);
        }
    break;
    case 2:
        if (acsChodoMoveKaidan(cp, p, mv) != 0)
        {
            sceVu0CopyVector(cp->matrix[3], p);
        }
    break;
    case 3:
        acsChodoMoveNingyo(cp, p, mv);
        sceVu0CopyVector(cp->matrix[3], p);
    break;
    }

    ManmdlSetAlpha(sgd_top, mv->alpha);
}

void acsChodoMoveKage(SgCOORDUNIT *cp, float *p, CMOVE_CTRL *mv)
{
    return;
}

char acsChodoMoveBird(SgCOORDUNIT *cp, float *p, CMOVE_CTRL *mv)
{
    sceVu0FVECTOR pos;
    u_int i;
    u_int all_cnt;
    u_int reaction_cnt;
    char mode;
    sceVu0FVECTOR start;
    sceVu0FVECTOR end;
    sceVu0FVECTOR v1;
    sceVu0FVECTOR v2;

    if (mv->furn_id == 286)
    {
        start[0] = 23547.0f;
        start[1] = -2345.0f;
        start[2] = 19338.0f;
        start[3] = 0.0f;

        mode = 2;

        end[0] = 26127.0f;
        end[1] = -1645.0f;
        end[2] = 21023.0f;
        end[3] = 0.0f;

        all_cnt = 60;
        reaction_cnt = 25;
    }
    else
    {
        start[0] = 34050.0f;
        start[1] = -2425.0f;
        start[2] = 42305.0f;
        start[3] = 0.0f;

        end[0] = 35150.0f;
        end[1] = -156.0f;
        end[2] = 36672.0f;
        end[3] = 0.0f;

        all_cnt = 90;
        reaction_cnt = 60;

        mode = 0;
    }

    for (i = 0; i < 3; i++)
    {
        pos[i] = motLinearSupValue(start[i], end[i], mode, mv->timer, all_cnt);
    }

    sceVu0Normalize(v1, cp->matrix[2]);
    sceVu0Normalize(v2, pos);

    sceVu0RotMatrixY(cp->matrix, cp->matrix, SgSqrtf(sceVu0InnerProduct(v1, v2)));
    sceVu0RotMatrixY(cp->matrix, cp->matrix, PI);

    if (mv->timer == reaction_cnt)
    {
        motPlyrActReq(6, &pos, 0);
    }

    pos[3] = 1.0f;

    sceVu0CopyVector(p, pos);

    if (!(ingame_wrk.stts & 0x10) && !(ingame_wrk.stts & 0x80))
    {
        if (mv->furn_id == 286)
        {
            if (mv->timer == 0)
            {
                SeStartPosEventFlame(24, 0, p, 0, 4096, 4096);
            }
        }
        else if (mv->timer == 40)
        {
            SeStartPosEventFlame(25, 0, p, 0, 4096, 4096);
        }

        if (++mv->timer >= all_cnt)
        {
            mv->timer = all_cnt;

            mv->req = 0;

            return 0;
        }
    }

    return 1;
}

void acsChodoMoveNingyo(SgCOORDUNIT *cp, float *p, CMOVE_CTRL *mv)
{
    #include "data/ofs.h"   // static sceVu0FVECTOR ofs;
    #include "data/rot.h"   // static sceVu0FVECTOR rot[13];
    #include "data/trans.h" // static sceVu0FVECTOR trans[13];
    u_int id1;
    u_int id2;
    u_int i;
    sceVu0FVECTOR pos;
    sceVu0FMATRIX m;
    sceVu0FMATRIX m0;
    sceVu0FMATRIX m1;
    sceVu0FVECTOR r1;
    sceVu0FVECTOR r2;
    sceVu0FMATRIX s;

    id1 = mv->timer / 10;
    id2 = id1 + 1;

    if (id2 > 12)
    {
        id2 = 12;
    }

    for (i = 0; i < 3; i++)
    {
        pos[i] = motLinearSupValue(trans[id1][i], trans[id2][i], 0, mv->timer % 10, 12);
    }

    sceVu0UnitMatrix(m0);
    sceVu0UnitMatrix(m1);
    sceVu0UnitMatrix(s);

    s[0][0] = s[1][1] = s[2][2] = 25.0f;

    sceVu0ScaleVector(r1, rot[id1], 0.017453292f);
    sceVu0ScaleVector(r2, rot[id2], 0.017453292f);

    sceRotMatrixXYZ(m0, m0, r1);
    sceRotMatrixXYZ(m1, m1, r2);

    motInterpMatrix(m, m0, m1, (mv->timer % 10) / 10.0f);

    sceVu0MulMatrix(m, s, m);

    LocalRotMatrixX(m, m, PI);

    sceVu0CopyVector(cp->matrix[0], m[0]);
    sceVu0CopyVector(cp->matrix[1], m[1]);
    sceVu0CopyVector(cp->matrix[2], m[2]);

    sceVu0SubVector(pos, pos, ofs);

    sceVu0ScaleVector(pos, pos, 25.0f);

    pos[1] = -pos[1];

    pos[3] = 1.0f;

    if (mv->timer > 60)
    {
        mv->alpha = 127 - (((mv->timer - 60) * 127) / 30);
    }

    sceVu0CopyVector(p, pos);

    sceVu0AddVector(p, cp->matrix[3], p);

    p[3] = 1.0f;

    sceVu0CopyVector(plyr_act_wrk.pos, p);

    if (mv->timer == 20)
    {
        sceVu0CopyVector(pos, p);

        motPlyrActReq(0, &pos, 0);
    }

    if ((!(ingame_wrk.stts & 0x10)) && (!(ingame_wrk.stts & 0x80)))
    {
        if ((mv->timer == 15) || (mv->timer == 80))
        {
            SeStartPosEventFlame(40, 0, p, 0, 4096, 4096);
        }

        if (mv->timer < 15)
        {
            VibrateRequest2(0, 200);
        }

        if (mv->timer == 89)
        {
            motPlyrActStop();
        }

        if (++mv->timer > 90)
        {
            mv->timer = 90;
        }
    }
}

char acsChodoMoveKaidan(SgCOORDUNIT *cp, float *p1, CMOVE_CTRL *mv)
{
    static sceVu0FVECTOR p;
    static sceVu0FVECTOR v;
    static float a = 0.502f;
    static float Ke = 0.5f;
    static float Kd = 0.973f;
    static float dv = 5.667f;
    static sceVu0FVECTOR start = {27800.0f, -1350.0f, 13875.0f, 0.0f};
    static char cnt;
    float earth;
    static float r = 0.0f;
    char se_flg;
    float dr;
    sceVu0FVECTOR tmp;
    sceVu0FVECTOR sub;

    se_flg = 0;

    if (mv->timer == 0)
    {
        p[0] = 0.0f;
        p[1] = 0.0f;
        p[2] = 0.0f;
        p[3] = 0.0f;

        v[0] = 0.0f;
        v[1] = 0.0f;
        v[3] = 0.0f;
        v[2] = -dv;

        cnt = 0;

        r = 0.0f;
    }

    if ((!(ingame_wrk.stts & 0x10)) && (!(ingame_wrk.stts & 0x80)))
    {
        v[1] += a;

        sceVu0AddVector(p, p, v);

        switch (cnt)
        {
        case 0:
            earth = 100.0f;
            break;
        case 1:
            earth = 270.0f;
            break;
        case 2:
            earth = 560.0f;
            break;
        case 3:
            earth = 920.0f;
            break;
        case 4:
            earth = 1150.0f;
            break;
        default:
            earth = 1305.0f;
        }

        if (earth < p[1])
        {
            cnt++;

            v[1] *= -Ke;
            v[2] *= Kd;

            p[1] = earth;

            if (cnt >= 7)
            {
                cnt = 7;
            }
            else
            {
                se_flg = 1;
            }
        }

        if (++mv->timer > 420)
        {
            mv->req = 0;

            mv->timer = 420;

            motPlyrActStop();

            return 0;
        }
    }

    dr = 0.1f;

    if (__builtin_fabsf(v[2]) < 3.0f)
    {
        dr = __builtin_fabsf(v[2]) / 30.0f;
    }

    if (((ingame_wrk.stts & 0x10) == 0) && ((ingame_wrk.stts & 0x80) == 0))
    {
        r += dr;
    }

    LocalRotMatrixX(cp->matrix, cp->matrix, r);

    sceVu0AddVector(p1, p, start);

    p1[3] = 1.0f;

    sceVu0CopyVector(plyr_act_wrk.pos,p1);

    if (mv->timer == 1)
    {
        sceVu0CopyVector(tmp, p1);

        motPlyrActReq(0, &tmp, 0);
    }

    sceVu0SubVector(sub, p1, plyr_wrk.move_box.pos);

    if ((SgSqrtf(sceVu0InnerProduct(sub, sub)) <= 1000.0f) && (mv->timer < 360))
    {
        mv->timer = 360;
    }

    if (mv->timer >= 361)
    {
        mv->alpha = 127 - (((mv->timer - 360) * 127) / 60);
    }
    else
    {
        mv->alpha = 127;
    }

    if (se_flg != 0)
    {
        if ((!(ingame_wrk.stts & 0x10)) && (!(ingame_wrk.stts & 0x80)))
        {
            SeStartFix(45, 0, 4096, 4096, 0);
        }
    }

    return 1;
}

void SetSpeFurnLight(FURN_WRK *furn)
{
    u_int *lit_dat;
    int room_no;
    static SgLIGHT lights[3];
    static SgLIGHT plights[16];
    static SgLIGHT slights[16];
    sceVu0FVECTOR ambient;
    u_int num;
    LIGHT_PACK *light_pack;
    // float *v1;
    // float *v1;
    // float *v1;

    room_no = furn->room_id;

    if (room_addr_tbl[room_no].near_sgd == NULL)
    {
        return;
    }

    lit_dat = room_addr_tbl[room_no].lit_data;

    if (lit_dat == NULL)
    {
        return;
    }

    SgReadLights(room_addr_tbl[room_no].near_sgd, lit_dat, ambient, lights, 3, plights, 16, slights, 16);

    num = plights[0].num < 0 || plights[0].num > 3 ? 3 : plights[0].num;

    SgSetPointLights(plights, num);

    light_pack = &furn->mylight;

    slights[0].num = 1;

    Vu0CopyVector(slights[0].pos, light_pack->spot[0].pos);
    Vu0CopyVector(slights[0].direction, light_pack->spot[0].direction);
    Vu0CopyVector(slights[0].diffuse, light_pack->spot[0].diffuse);
    Vu0CopyVector(slights[0].specular, light_pack->spot[0].diffuse);

    slights[0].intens = light_pack->spot[0].intens;
    slights[0].power = light_pack->spot[0].power;
    slights[0].Enable = 0;
    slights[0].SEnable = 0;

    SgSetSpotLights(slights, 1);
}

SPRING rope_spring[20][9] = {0};
C_PARTICLE rope_particle[20][10] = {0};
