#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "fod.h"

#include "sce/libvu0.h"

#include "common/utility.h"
#include "ingame/camera/camera.h"
#include "ingame/plyr/unit_ctl.h"
#include "graphics/scene/scene.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_scr.h"
#include "graphics/graph3d/libsg.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/sglight.h"

sceVu0FMATRIX fod_cmn_mtx = {0};
FOD_EFF_PARAM eff_param = {0};

#define PI 3.1415927f
#define DEG2RAD(x) ((float)(x)*PI/180.0f)

void FodInit(FOD_CTRL *fc, u_int *tcp, u_int *tlp, u_int *tep)
{
    sceVu0UnitMatrix(fod_cmn_mtx);

    fod_cmn_mtx[0][0] = fod_cmn_mtx[1][1] = fod_cmn_mtx[2][2] = 25.0f;
    fod_cmn_mtx[3][3] = 1.0f;

    sceVu0RotMatrixX(fod_cmn_mtx, fod_cmn_mtx, PI);

    memset(fc, 0, sizeof(FOD_CTRL));

    fc->cam_file_hdr = (FOD_FILE_HDR *)tcp;
    fc->lit_file_hdr = (FOD_FILE_HDR *)tlp;
    fc->eff_file_hdr = (FOD_FILE_HDR *)tep;

    fc->now_frame = 1;
    fc->now_reso = 0;
#ifdef BUILD_EU_VERSION
    fc->check_cnt = 0;
#endif
    fc->total_frame = 2;
    fc->frame_max = tcp[3];
    fc->resolution = tcp[2];
    fc->end_flg = 0;

    fc->cam_frame_top = (FOD_CAM_FRAME *)&tcp[16];
    fc->eff_frame_top = (FOD_EFF_FRAME *)FodGetFixEffect(tep);

    fc->fod_light.lit_top = tlp;

    FodGetLightNum(&fc->fod_light);
    FodGetLightSerial(&fc->fod_light);

    fc->lit_frame_top = (FOD_LIT_FRAME *)FodGetFirstLight(&fc->fod_light);

    fc->cam_frame = fc->cam_frame_top;
    fc->lit_frame = fc->lit_frame_top;
    fc->eff_frame = fc->eff_frame_top;
}

int FodNextFrame(FOD_CTRL *fc)
{
    u_int size;
    char *cam_addr;
    char *lit_addr;
    char *eff_addr;

    size = fc->cam_frame->size;

    lit_addr = (char *)fc->lit_frame;
    eff_addr = (char *)fc->eff_frame;
    cam_addr = (char *)fc->cam_frame;

    fc->now_reso++;
    fc->total_frame = fc->now_frame * 2 + fc->now_reso;

    if (fc->now_reso < fc->resolution)
    {
        return 0;
    }

    fc->now_reso = 0;
    fc->now_frame++;

    if (fc->frame_max < fc->now_frame)
    {
        fc->end_flg = 1;

        return 1;
    }

    if (fc->cam_file_hdr->frame != 0)
    {
        cam_addr = cam_addr + 16 + size;
        fc->cam_frame = (FOD_CAM_FRAME *)cam_addr;
    }

    if (fc->lit_file_hdr->frame != 0)
    {
        lit_addr = lit_addr + fc->lit_frame->size;
        fc->lit_frame = (FOD_LIT_FRAME *)lit_addr;
    }

    if (fc->eff_file_hdr->frame != 0)
    {
        eff_addr = eff_addr + fc->eff_frame->size;
        fc->eff_frame = (FOD_EFF_FRAME *)eff_addr;

        return NULL;
    }

    // no return ...
}

void FodSetFrame(FOD_CTRL *fc, u_int frame)
{
    u_int i;
    u_int *fod_cam_addr;
    u_int *fod_lit_addr;
    u_int *fod_eff_addr;

    if (frame > fc->frame_max || frame == 0)
    {
        return;
    }

    fod_cam_addr = (u_int *)fc->cam_frame_top;
    fod_lit_addr = (u_int *)fc->lit_frame_top;
    fod_eff_addr = (u_int *)fc->eff_frame_top;

    for(i = 1; i < frame; i++)
    {
        if (fc->cam_file_hdr->frame != 0)
        {
            fod_cam_addr = fod_cam_addr + ((FOD_CAM_FRAME *)fod_cam_addr)->size / 4; // sizeof(u_int) ?
            fod_cam_addr += 4;
        }

        if (fc->lit_file_hdr->frame != 0)
        {
            fod_lit_addr = fod_lit_addr + ((FOD_LIT_FRAME *)fod_lit_addr)->size / 4; // sizeof(u_int) ?
        }

        if (fc->eff_file_hdr->frame != 0)
        {
            fod_eff_addr = fod_eff_addr + ((FOD_EFF_FRAME *)fod_eff_addr)->size / 4; // sizeof(u_int) ?
        }
    }

    fc->now_frame = frame;
    fc->now_reso = 0;
    fc->total_frame = frame * 2;

    fc->cam_frame = (FOD_CAM_FRAME *)fod_cam_addr;
    fc->lit_frame = (FOD_LIT_FRAME *)fod_lit_addr;
    fc->eff_frame = (FOD_EFF_FRAME *)fod_eff_addr;
}

void FodGetLightNum(FOD_LIGHT *fl)
{
    u_int *lit_addr;

    lit_addr = fl->lit_top;
    lit_addr += 4;

    fl->ilit_num = lit_addr[0];
    fl->slit_num = lit_addr[1];
    fl->plit_num = lit_addr[2];

    fl->all_lit_num = lit_addr[3] - 1;
}

void FodGetLightSerial(FOD_LIGHT *fl) {
    FOD_LIT_SERIAL *fls;
    u_int *lit_addr;
    int i;

    lit_addr = (u_int *)fl->lit_top + 16;
    fls = (FOD_LIT_SERIAL *)lit_addr;
    fl->hand_spot_no = -1;

    for (i = 0; i < fl->all_lit_num; i++)
    {
        *(fl->lit_serial + i) = *fls;

        StrToLower((fl->lit_serial + i)->light_name);

        if (strcmp((fl->lit_serial + i)->light_name, "hand_spot") == 0)
        {
            if (fl->hand_spot_no == -1)
            {
                fl->hand_spot_no = i;
            }
        }
        else if (strcmp((fl->lit_serial + i)->light_name, "hand-spot") == 0)
        {
            if (fl->hand_spot_no == -1)
            {
                fl->hand_spot_no = i;
            }
        }

        fls++;
    }
}

u_int* FodGetFirstLight(FOD_LIGHT *fl)
{
    float cone_deg;
    int i;
    int lit_type;
    u_int *lit_addr;
    float cc;

    lit_addr = fl->lit_top;
    lit_addr += 8;
    lit_addr += (fl->all_lit_num + 1) * 8;

    for (i = 0; i < 6; i++)
    {
        fl->amb[i][0] = 0.0f;
        fl->amb[i][1] = 0.0f;
        fl->amb[i][2] = 0.0f;
        fl->amb[i][3] = 0.0f;
    }

    Vu0CopyVector(fl->amb[0], *(sceVu0FVECTOR *)&lit_addr[4]);

    lit_addr += 8;

    for (i = 0; i < fl->all_lit_num; i++)
    {
        lit_type = FodGetLightType(fl, lit_addr[0]);

        switch (lit_type)
        {
        case 0:
            lit_addr += 8;
        break;
        case 1:
            Vu0CopyVector(fl->all_lit[i].diffuse, *(sceVu0FVECTOR *)&lit_addr[4]);
            Vu0CopyVector(fl->all_lit[i].specular, *(sceVu0FVECTOR *)&lit_addr[4]);

            lit_addr += 12;
        break;
        case 2:
            fl->all_lit[i].ambient[0] = 0.0f;
            fl->all_lit[i].ambient[1] = 0.0f;
            fl->all_lit[i].ambient[2] = 0.0f;
            fl->all_lit[i].ambient[3] = 0.0f;

            Vu0CopyVector(fl->all_lit[i].diffuse, *(sceVu0FVECTOR *)&lit_addr[4]);
            Vu0CopyVector(fl->all_lit[i].specular, *(sceVu0FVECTOR *)&lit_addr[4]);
            Vu0CopyVector(fl->all_lit[i].pos, *(sceVu0FVECTOR *)&lit_addr[8]);
            Vu0CopyVector(fl->all_lit[i].direction, *(sceVu0FVECTOR *)&lit_addr[12]);

            sceVu0ApplyMatrix(fl->all_lit[i].pos, fod_cmn_mtx, fl->all_lit[i].pos);
            sceVu0ApplyMatrix(fl->all_lit[i].direction, fod_cmn_mtx, fl->all_lit[i].direction);

            cone_deg = *(float *)&lit_addr[16];

            cc = SgCosf(DEG2RAD(cone_deg));

            fl->all_lit[i].intens = cc * cc;

            if (fl->all_lit[i].intens < 0.0f)
            {
                fl->all_lit[i].intens = 0.0f;
            }

            fl->all_lit[i].power = _TransSPower(25.0f);
            fl->all_lit[i].ambient[0] = 25.0f;
            fl->all_lit[i].ambient[1] = cone_deg;
            fl->all_lit[i].Enable = 0;
            fl->all_lit[i].SEnable = 0;

            lit_addr += 20;
        break;
        case 3:
            fl->all_lit[i].ambient[0] = 0.0f;
            fl->all_lit[i].ambient[1] = 0.0f;
            fl->all_lit[i].ambient[2] = 0.0f;
            fl->all_lit[i].ambient[3] = 0.0f;

            Vu0CopyVector(fl->all_lit[i].diffuse, *(sceVu0FVECTOR *)&lit_addr[4]);
            Vu0CopyVector(fl->all_lit[i].specular, *(sceVu0FVECTOR *)&lit_addr[4]);
            Vu0CopyVector(fl->all_lit[i].pos, *(sceVu0FVECTOR *)&lit_addr[8]);

            sceVu0ApplyMatrix(fl->all_lit[i].pos, fod_cmn_mtx, fl->all_lit[i].pos);

            fl->all_lit[i].power = _TransPPower(25.0f);
            fl->all_lit[i].ambient[0] = 25.0f;
            fl->all_lit[i].Enable = 0;
            fl->all_lit[i].SEnable = 0;

            lit_addr += 12;
        break;
        }
    }

    return lit_addr;
}

void FodSetMyLight(FOD_LIGHT *fl, char *pfx, float *eye)
{
    static SgLIGHT ilight[3];
    static SgLIGHT slight[3];
    static SgLIGHT plight[3];
    static sceVu0FVECTOR ambient;
    FOD_LIT_SERIAL *fls;
    SgLIGHT *org;
    int i;
    int il_num;
    int sl_num;
    int pl_num;
    int get_light_flg;

    fls = fl->lit_serial;
    org = fl->all_lit;

    pl_num = 0;
    il_num = 0;
    sl_num = 0;

    for (i = 0; i< fl->all_lit_num; i++)
    {
        get_light_flg = 0;

        if (fl->hand_spot_no == i)
        {
            if (sl_num < 3)
            {
                slight[sl_num] = *org;
                sl_num++;
            }
        }
        else if (strncmp(fls->light_name, pfx, 4) == 0)
        {
            get_light_flg = 1;
        }
        else if (*pfx != 'm')
        {
            get_light_flg = strncmp(fls->light_name, "room", 4) == 0;
        }

        if (get_light_flg)
        {
            if (fls->light_type == 1)
            {
                if (il_num < 3)
                {
                    ilight[il_num] = *org;
                    il_num++;
                }
            }
            else if (fls->light_type == 2)
            {
                if (sl_num < 3)
                {
                    slight[sl_num] = *org;
                    sl_num++;
                }
            }
            else if (fls->light_type == 3)
            {
                if (pl_num < 3)
                {
                    plight[pl_num] = *org;
                    pl_num++;
                }
            }
        }

        fls++;
        org++;
    }

    Vu0CopyVector(ambient, fl->amb[0]);

    if (*pfx == 'm')
    {
        sceVu0AddVector(ambient, ambient, fl->amb[1]);
    }
    else if (*pfx == 'r')
    {
        sceVu0AddVector(ambient, ambient, fl->amb[2]);
    }
    else if (*pfx == 'f')
    {
        sceVu0AddVector(ambient, ambient, fl->amb[3]);
    }
    else if (*pfx == 'd')
    {
        sceVu0AddVector(ambient, ambient, fl->amb[4]);
    }
    else if (*pfx == 'i')
    {
        sceVu0AddVector(ambient, ambient, fl->amb[5]);
    }

    SgSetAmbient(ambient);

    if (il_num >= 1)
    {
        SgSetInfiniteLights(eye, ilight, il_num);
    }
    else
    {
        SgSetInfiniteLights(eye, NULL, 0);
    }

    if (pl_num >= 1)
    {
        SgSetPointLights(plight, pl_num);
    }
    else
    {
        SgSetPointLights(NULL, 0);
    }

    if (sl_num >= 1)
    {
        FodSetSpotLights(slight, sl_num);
    }
    else
    {
        FodSetSpotLights(slight, sl_num);
    }
}

void FodSetSpotLights(SgLIGHT *sl, u_int num)
{
    u_int i;

    if (sl != NULL)
    {
        for (i = 0; i < num; i++)
        {
            sl[i].pos[3] = 1.0f;

            sceVu0SubVector(sl[i].direction, sl[i].pos, sl[i].direction);
        }
    }

    SgSetSpotLights(sl, num);
}

void FodGetToSgLight(FOD_CTRL *fc)
{
    SgLIGHT *sl;
    int i;
    u_int *lit_addr;
    float *data;
    float cone_deg;
    float cc;

    lit_addr = (u_int *)fc->lit_frame;
    lit_addr += 4;

    for (i = 0; i < fc->fod_light.all_lit_num; i++)
    {
        if (fc->fod_light.lit_serial[i].anm_flg == 1)
        {
            data = (float *)&lit_addr[4];
            sl = &fc->fod_light.all_lit[i];

            switch (lit_addr[1])
            {
            case 1:
                if (((u_char *)lit_addr)[8] != 0)
                {
                    Vu0CopyVector(sl->diffuse, data);
                    Vu0CopyVector(sl->specular, sl->diffuse);

                    data += 4;

                    sl->ambient[0] = 0.0f;
                    sl->ambient[1] = 0.0f;
                    sl->ambient[2] = 0.0f;
                    sl->ambient[3] = 0.0f;
                }

                if (((u_char *)lit_addr)[10] != 0)
                {
                    Vu0CopyVector(sl->direction, data);

                    data += 4;
                }
            break;
            case 2:
                if (((u_char *)lit_addr)[8] != 0)
                {
                    Vu0CopyVector(sl->diffuse, data);
                    Vu0CopyVector(sl->specular, sl->diffuse);

                    data += 4;
                }

                if (((u_char *)lit_addr)[9] != 0)
                {
                    Vu0CopyVector(sl->pos, data);
                    sceVu0ApplyMatrix(sl->pos, fod_cmn_mtx, sl->pos);

                    data += 4;
                }

                if (((u_char *)lit_addr)[10] != 0)
                {
                    Vu0CopyVector(sl->direction, data);
                    sceVu0ApplyMatrix(sl->direction, fod_cmn_mtx, sl->direction);

                    data += 4;
                }

                if (((u_char *)lit_addr)[11] != 0)
                {
                    cone_deg = *(float *)data;
                    cc = SgCosf(DEG2RAD(cone_deg));

                    sl->intens = cc * cc;
                    sl->ambient[0] = 25.0f;
                    sl->ambient[1] = cone_deg;

                    data += 4;
                }
            break;
            case 3:
                if (((u_char *)lit_addr)[8] != 0)
                {
                    Vu0CopyVector(sl->diffuse, data);
                    Vu0CopyVector(sl->specular, sl->diffuse);

                    data += 4;
                }

                if (((u_char *)lit_addr)[9] != 0)
                {
                    Vu0CopyVector(sl->pos, data);
                    sceVu0ApplyMatrix(sl->pos, fod_cmn_mtx, sl->pos);

                    data += 4;
                }
            break;
            }

            lit_addr = (u_int *)data;
        }

    }
}

void FodGetHandSpotPos(FOD_LIGHT *fl, float *p, float *i)
{
    SgLIGHT *sl;

    sl = &fl->all_lit[fl->hand_spot_no];

    Vu0CopyVector(p, sl->pos);
    Vu0CopyVector(i, sl->direction);
}

void FodGetDropSpotPos(FOD_LIGHT *fl, char *pfx, float *lp, float *li)
{
    SgLIGHT *sl;
    int i;
    int no;

    no = -1;

    for (i = 0; i < fl->all_lit_num; i++)
    {
        if (
            strncmp(pfx, fl->lit_serial[i].light_name, 4) == 0 &&
            strncmp("drop_spot", &fl->lit_serial[i].light_name[5], 9) == 0
        )
        {
            no = i;
        }
    }

    if (no >= 0)
    {
        sl = &fl->all_lit[no];

        Vu0CopyVector(lp, sl->pos);
        Vu0CopyVector(li, sl->direction);
    }
}

int FodGetLightType(FOD_LIGHT *fl, int id)
{
    int i;
    int ret_type;

    ret_type = 0;

    for (i = 0; i < fl->all_lit_num; i++)
    {
        if (fl->lit_serial[i].light_no == id)
        {
            ret_type = fl->lit_serial[i].light_type;
        }
    }

    return ret_type;
}

void FodGetFirstCam(SgCAMERA *cam, FOD_CTRL *fc)
{
    FOD_FIRST_CAM *fcam;

    fcam = (FOD_FIRST_CAM *)((char *)fc->cam_file_hdr + sizeof(FOD_FILE_HDR));

    Vu0CopyVector(cam->p, fcam->p);
    Vu0CopyVector(cam->i, fcam->i);

    cam->roll = DEG2RAD(fcam->roll);

    sceVu0ApplyMatrix(cam->p,fod_cmn_mtx,cam->p);
    sceVu0ApplyMatrix(cam->i,fod_cmn_mtx,cam->i);

    cam->roll += PI;

    if (cam->roll > PI)
    {
        cam->roll -= PI * 2;
    }

    cam->fov = DEG2RAD(fcam->fov);

    if (cam->fov > PI)
    {
        cam->fov -= PI * 2;
    }

    cam->nearz = fcam->near;
    cam->farz = fcam->far;

    cam->ax = 1.0f;
    cam->ay = 0.4069f;

    cam->cx = 2048.0f;
    cam->cy = 2048.0f;

    cam->zmin = 0.0f;
    cam->zmax = 16777215.0f;
}

void FodGetCamData(SgCAMERA *cam, FOD_CTRL *fc)
{
    float *fdat;
    int i;

    fdat = (float *)((char *)fc->cam_frame +  sizeof(FOD_CAM_FRAME));

    for (i = 0; i < 6; i++)
    {
        if (fc->cam_frame->anm[i] == 1)
        {
            switch(i)
            {
            case 0:
                Vu0CopyVector(cam->p, fdat);
                sceVu0ApplyMatrix(cam->p,fod_cmn_mtx,cam->p);
                fdat += 4;
            break;
            case 1:
                Vu0CopyVector(cam->i, fdat);
                sceVu0ApplyMatrix(cam->i,fod_cmn_mtx,cam->i);
                fdat += 4;
            break;
            case 2:
                cam->roll = DEG2RAD(fdat[0]) + PI;
                if (cam->roll > PI)
                {
                    cam->roll -= PI * 2;
                }
                fdat += 1;
            break;
            case 3:
                cam->fov = DEG2RAD(fdat[0]);
                if (cam->fov > PI)
                {
                    cam->fov -= 2 * PI;
                }
                fdat += 1;
            break;
            case 4:
                cam->nearz = fdat[0];
                fdat += 1;
            break;
            case 5:
                cam->farz = fdat[0];
                fdat += 1;
            break;
            }
        }
    }
}

u_int* FodGetFixEffect(u_int *tep)
{
    FOD_EFF_DATA *fed;
    u_int i;

    if (tep[3] == 0)
    {
        eff_param.fix_eff_num = 0;
        return &tep[4];
    }
    else
    {
        memset(&eff_param, 0, sizeof(eff_param));
        tep += 4;

        fed = (FOD_EFF_DATA *)&tep[4];

        eff_param.fix_eff_num = tep[0];
        eff_param.pdf_p = NULL;

        for (i = 0; i < eff_param.fix_eff_num; i++)
        {
            eff_param.fix[i] = fed;
            fed++;
        }

        return &tep[eff_param.fix_eff_num * 4 + 4];
    }
}

void FodSetEffect(FOD_CTRL *fc)
{
    FOD_EFF_FRAME *fef;
    FOD_EFF_DATA *fed;
    int i;
    char *end_addr;

    fef = fc->eff_frame;

    fed = (FOD_EFF_DATA *)((u_char *)fef + sizeof(FOD_EFF_FRAME));

    end_addr = (char *)fc->eff_frame;
    end_addr += *((int *)&end_addr[4]);

    for (i = 0; i < 4; i++)
    {
        eff_param.fade_mdl[i] = NULL;
    }

    eff_param.fade_mdl_cnt = 0;

    for (i = 0; i < 4; i++)
    {
        eff_param.mdl_efct_sw[i] = -1;
    }

    eff_param.fire_num = 0;

    if (fef->zdepth == 1)
    {
        SetEffects(1, 1);
    }

    if (fef->mono == 1 && eff_param.mono_flg == 0)
    {
        eff_param.mono_flg = 1;
        ChangeMonochrome(1);
    }
    else if (fef->mono == 0 && eff_param.mono_flg != 0)
    {
        eff_param.mono_flg = 0;
        ChangeMonochrome(0);
    }

    while ((char *)fed < end_addr)
    {
        FodSetEffectParam(fed);
        fed++;
    }

    for (i = 0; i < eff_param.fix_eff_num; i++)
    {
        fed = eff_param.fix[i];

        if (fed->dither.efct_id == 18)
        {
            if (fc->now_frame == 1 && fc->now_reso == 0)
            {
                eff_param.pdf_spd = 1.0f;
                eff_param.pdf_rate = (fed->dither).speed;
                eff_param.pdf_trate = (fed->dither).alpha;

                eff_param.pdf_pos[0] = *(float *)(&((char *)fed)[20]);
                eff_param.pdf_pos[1] = *(float *)(&((char *)fed)[24]);
                eff_param.pdf_pos[2] = *(float *)(&((char *)fed)[28]);
                eff_param.pdf_pos[3] = 0.0f;

                sceVu0ApplyMatrix(eff_param.pdf_pos, fod_cmn_mtx, eff_param.pdf_pos);

                eff_param.pdf_p = SetEffects(27, 2,
                    (u_long)(fed->dither).alpmax,
                    (u_long)(fed->dither).colmax,
                    *(float *)(&((char *)fed)[36]),
                    *(float *)(&((char *)fed)[40]),
                    eff_param.pdf_pos,
                    0, 0, 0, 0,
                    &eff_param.pdf_spd,
                    &eff_param.pdf_rate,
                    &eff_param.pdf_trate
                );

                i += 2;
            }
        }
        else
        {
            FodSetEffectParam(fed);
        }
    }
}

void FodSetEffectParam(FOD_EFF_DATA *fed)
{
    int i;
    sceVu0FMATRIX mtx;
    sceVu0FVECTOR base;
    sceVu0FVECTOR trgt;
    float x;
    float y;
    float z;
    sceVu0FVECTOR *fire;
    float l;

    switch(fed->dither.efct_id)
    {
    case FOD_EFF_DITHER:
        SetEffects(EF_DITHER, 1, fed->dither.type, fed->dither.alpha, fed->dither.speed, fed->dither.alpmax, fed->dither.colmax);
    break;
    case FOD_EFF_BLUR_NORMAL:
    case FOD_EFF_BLUR_BLACK:
    case FOD_EFF_BLUR_WHITE:
        SetEffects(EF_BLUR_N, 1, &fed->blur.alpha, fed->blur.scale, fed->blur.rot, 320.0f, 112.0f);
    break;
    case FOD_EFF_DEFORM:
        SetEffects(EF_DEFORM, 1, fed->deform.type, fed->deform.volume);
    break;
    case FOD_EFF_FOCUS:
        SetEffects(EF_FOCUS, 1, fed->focus.volume);
    break;
    case FOD_EFF_CONTRAST1:
        SetEffects(EF_NCONTRAST, 1, fed->contrast.color, fed->contrast.alpha);
    break;
    case FOD_EFF_CONTRAST2:
        SetEffects(EF_NCONTRAST2, 1, fed->contrast.color, fed->contrast.alpha);
        break;
    case FOD_EFF_CONTRAST3:
        SetEffects(EF_NCONTRAST3, 1, fed->contrast.color, fed->contrast.alpha);
    break;
    case FOD_EFF_FADE_FRAME:
        SetEffects(EF_FADEFRAME, 1, fed->f_frame.volume, 0x1000);
    break;
    case FOD_EFF_LENZ_FLARE:
        if (fed->lenz_f.type == 0)
        {
            eff_param.lenz_rot[0] = fed->lenz_f.x;
            eff_param.lenz_rot[1] = fed->lenz_f.y;
            eff_param.lenz_rot[2] = fed->lenz_f.z;
            eff_param.lenz_rot[3] = 0.0f;

            eff_param.lenz_pos[0] = *(float *)((int)fed + 0x14);
            eff_param.lenz_pos[1] = *(float *)((int)fed + 0x18);
            eff_param.lenz_pos[2] = *(float *)((int)fed + 0x1c);
            eff_param.lenz_pos[3] = 0.0f;

            for (i = 0; i < 3; i++)
            {
                eff_param.lenz_rot[i] = DEG2RAD(eff_param.lenz_rot[i]);

                if (eff_param.lenz_rot[i] > PI)
                {
                    eff_param.lenz_rot[i] -= PI * 2;
                }
            }

            memset(base, 0, sizeof(base));

            sceVu0UnitMatrix(mtx);

            mtx[0][0] = mtx[1][1] = 0.0f;

            sceVu0RotMatrixX(mtx, mtx, eff_param.lenz_rot[0]);
            sceVu0RotMatrixY(mtx, mtx, eff_param.lenz_rot[1]);
            sceVu0RotMatrixZ(mtx, mtx, eff_param.lenz_rot[2]);

            x = mtx[2][0];
            y = mtx[2][1];
            z = mtx[2][2];

            l = SgSqrtf(x * x + z * z);

            eff_param.lenz_rot[0] = SgAtan2f(y, l);
            eff_param.lenz_rot[1] = SgAtan2f(x, z);

            eff_param.lenz_rot[0] = -eff_param.lenz_rot[0];
            eff_param.lenz_rot[1] = -eff_param.lenz_rot[1];
            eff_param.lenz_rot[2] = 0.0f;

            eff_param.lenz_rot[1] += PI;

            if (eff_param.lenz_rot[0] > PI)
            {
                eff_param.lenz_rot[0] -= PI * 2;
            }

            if (eff_param.lenz_rot[1] > PI)
            {
                eff_param.lenz_rot[1] -= PI * 2;
            }

            sceVu0ApplyMatrix(eff_param.lenz_pos, fod_cmn_mtx, eff_param.lenz_pos);

            SetEffects(EF_RENZFLARE, 1, 4, eff_param.lenz_pos, eff_param.lenz_rot);
        }
    break;
    case FOD_EFF_CROSS_FADE:
        SetEffects(EF_OVERLAP, 1, fed->cross_f.volume);
    break;
    case FOD_EFF_FADE_SCREEN:
        if (fed->fade_scr.a != 0)
        {
            SceneSetSquare(16, 0.0f, 0.0f, 640.0f, 448.0f, fed->fade_scr.r, fed->fade_scr.g, fed->fade_scr.b, fed->fade_scr.a);
        }
    break;
    case FOD_EFF_FADE_MODEL:
        eff_param.fade_mdl[eff_param.fade_mdl_cnt] = (FOD_EF_FADE_MDL *)fed;
        eff_param.fade_mdl_cnt++;
    break;
    case FOD_EFF_PARTS_DEFORM:
        if (fed->dither.type == 0)
        {
            eff_param.pdf_spd = GetRndSP(1, 2);
            eff_param.pdf_rate = fed->pdf1.rate + 0.2f;
            eff_param.pdf_trate = fed->pdf1.trate;

            eff_param.pdf_pos[0] = *(float *)(&((char *)fed)[20]);
            eff_param.pdf_pos[1] = *(float *)(&((char *)fed)[24]);
            eff_param.pdf_pos[2] = *(float *)(&((char *)fed)[28]);
            eff_param.pdf_pos[3] = 0.0f;

            sceVu0ApplyMatrix(eff_param.pdf_pos, fod_cmn_mtx, eff_param.pdf_pos);
        }
    break;
    case FOD_EFF_FIRE:
    case FOD_EFF_FIX_FIRE:
        fire = &eff_param.fire_pos[eff_param.fire_num];

        if (eff_param.fire_num >= 4)
        {
            break;
        }

        (*fire)[0] = fed->fire.posx;
        (*fire)[1] = fed->fire.posy;
        (*fire)[2] = fed->fire.posz;
        (*fire)[3] = 0.0f;

        sceVu0ApplyMatrix(*fire, fod_cmn_mtx, *fire);

        SetEffects(EF_FIRE, 1, 3, *fire, 0x50, 0x46, 0x1e, 0.4f, 0xf0, 0xd0, 0xa0, 3.0f);

        eff_param.fire_num++;
    break;
    case FOD_EFF_ENEMY:
        for (i = 0; i < 4; i++)
        {
            if (eff_param.mdl_efct_sw[i] == -1)
            {
                eff_param.mdl_efct_sw[i] = GetPrefixNo((char *)fed->enemy.prefix);
                break;
            }
        }
    break;
    case FOD_EFF_SHIBATA: // ??
    break;
    }
}
