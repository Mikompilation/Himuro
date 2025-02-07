#include "common.h"
#include "typedefs.h"
#include "mime.h"

#include "sce/libvu0.h"

#include "os/pad.h"
#include "os/eeiop/eese.h"
#include "main/glob.h"
#include "ingame/map/furn_dat.h"
#include "graphics/graph3d/sg_dat.h"
#include "graphics/graph3d/sgsu.h"
#include "graphics/graph3d/load3d.h"
#include "graphics/graph3d/libsg.h"
#include "graphics/motion/motion.h"
#include "graphics/motion/mim_dat.h"
#include "graphics/motion/mdldat.h"
#include "graphics/motion/mdlwork.h"

typedef struct {
    u_char file_id[4];
    u_int map_flg;
    u_int key_num;
    u_int frame_num;
    u_int parts_no;
    u_int flg;
    u_char w_name[8];
} MIM_FILE_HEADER;

typedef struct {
    u_int *wav_addr;
    u_int *key_addr;
} MIM_ADDR_TABLE;

static u_int plyr_mepati_cnt = 0;

u_int* mimInitMimeCtrl(MIME_CTRL *m_ctrl, MIME_DAT *mdat, u_int *mim_p, u_int *mdl_p, u_int *tmp_p)
{
    u_int i;
    u_int j;
    u_int mim_num;
    u_int parts_buf[20];
    u_int *parts_p[20];
    u_int *parts;
    u_int fsize;
    u_int parts_no;
    u_int flg;

    mim_num = mim_p[0];
    mim_p += 4;

    for (i = 0; i < mim_num; i++)
    {
        fsize = mim_p[0];
        mim_p += 4;
        flg = 0;
        m_ctrl[i].stat = 0;

        parts_no = mimGetPartsNo(mim_p);
        parts = GetFileInPak(mdl_p, parts_no);

        parts_buf[i] = parts_no;
        parts_p[i] = tmp_p;

        for (j = 0 ; j < i; j++)
        {
            if (parts_no == parts_buf[j])
            {
                flg = 1;
                break;
            }
        }

        if (flg == 0)
        {
            tmp_p = mimSetMimeDat(&mdat[i], mim_p, tmp_p, parts);
        }
        else
        {
            mimSetMimeDat(&mdat[i], mim_p, parts_p[j], parts);
        }

        mimSetMimeCtrl(&m_ctrl[i], &mdat[i], 0, parts_no);
        mimAddressMapping(m_ctrl[i].mdat->dat);
        mimSetOriVertex(m_ctrl[i].mdat);

        mim_p += fsize / 4;
    }

    return tmp_p;
}

void mimInitAcsCtrl(ANI_CTRL *ani_ctrl, u_short mdl_no)
{
    u_int i;
    u_int j;
    u_int k;
    u_int wmim_num;
    WMIM_DAT *wmim_tbl;
    MIME_CTRL *m_ctrl;
    u_char flg;
    u_char name[100];
    WMIM_DAT *wdat;

    if (ani_ctrl->wmim == NULL)
    {
        return;
    }

    wmim_tbl = manmdl_dat[mdl_no].wdat;

    wmim_num = 0;

    while (wmim_tbl != NULL && wmim_tbl[wmim_num].dat != NULL)
    {
        ani_ctrl->wmim[wmim_num].wdat = &wmim_tbl[wmim_num];

        ani_ctrl->wmim[wmim_num].pbak[0] = 0.0f;
        ani_ctrl->wmim[wmim_num].pbak[1] = 0.0f;
        ani_ctrl->wmim[wmim_num].pbak[2] = 0.0f;
        ani_ctrl->wmim[wmim_num].pbak[3] = 0.0f;

        ani_ctrl->wmim[wmim_num].v[0] = 0.0f;
        ani_ctrl->wmim[wmim_num].v[1] = 0.0f;
        ani_ctrl->wmim[wmim_num].v[2] = 0.0f;
        ani_ctrl->wmim[wmim_num].v[3] = 0.0f;

        ani_ctrl->wmim[wmim_num].w[0] = 0.0f;
        ani_ctrl->wmim[wmim_num].w[1] = 0.0f;
        ani_ctrl->wmim[wmim_num].w[2] = 0.0f;
        ani_ctrl->wmim[wmim_num].w[3] = 0.0f;

        wmim_num++;

        wmim_tbl = manmdl_dat[mdl_no].wdat;
    }

    for (i = wmim_num; i < 10; i++)
    {
        ani_ctrl->wmim[i].wdat = NULL;

        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
    }

    ani_ctrl->wmim_num = wmim_num;

    if (ani_ctrl->mim_num < wmim_num)
    {
        ani_ctrl->mim_num = 0;
        ani_ctrl->wmim_num = 0;
    }
    else
    {
        wmim_num = 0;

        for (i = 0; i < ani_ctrl->mim_num; i++)
        {
            flg = 0;

            if (ani_ctrl->mim == NULL)
            {
                return;
            }

            m_ctrl = &ani_ctrl->mim[i];

            if (m_ctrl->flg != 0)
            {
                strncpy((char *)name, (const char *)mimGetWeightName(m_ctrl->mdat->dat), 8);

                for (j = 0; j < ani_ctrl->wmim_num; j++)
                {
                    wdat = ani_ctrl->wmim[j].wdat;

                    k = 0;

                    while ( 1 )
                    {
                        if (wdat->dat->name[k] == '\0')
                        {
                            m_ctrl->weight_id = j;
                            mimRequest(m_ctrl, 0);
                            flg = 1;
                            break;
                        }
                        else if (name[k] != wdat->dat->name[k])
                        {
                            break;
                        }

                        k++;
                    }

                    if (flg != 0)
                    {
                        break;
                    }
                }

            }
            else
            {
                m_ctrl->weight_id = 0;
            }
        }
    }
}

void mimInitLoop(ANI_CTRL *ani_ctrl)
{
    u_int i;

    if (ani_ctrl->mim != NULL)
    {
        for (i = 0; i < ani_ctrl->mim_num; i++)
        {
            ani_ctrl->mim[i].loop = 0;

            asm volatile ("nop");
            asm volatile ("nop");
            asm volatile ("nop");
        }
    }
}

void mimRequest(MIME_CTRL *m_ctrl, u_char rev)
{
    if (rev != 0)
    {
        m_ctrl->frame = mimGetFrameNum(m_ctrl->mdat->dat) - 1;
    }
    else
    {
        m_ctrl->frame = 0;
    }

    m_ctrl->stat = 2;
    m_ctrl->rev = rev;
}

void mimRequestLastFrame(MIME_CTRL *m_ctrl, u_char rev)
{
    if (rev == 0)
    {
        m_ctrl->frame = mimGetFrameNum(m_ctrl->mdat->dat) - 1;
    }
    else
    {
        m_ctrl->frame = 0;
    }

    m_ctrl->stat = 2;
    m_ctrl->rev = rev;
}

void mimMepatiReq(u_char mode, u_char rev)
{
    if (mim_mepati_id == 0xff)
    {
        return;
    }

    if (mode == 0)
    {
        mimRequest(&ani_mdl[0].mim[mim_mepati_id], rev);
    }
    else
    {
        mimRequestLastFrame(&ani_mdl[0].mim[mim_mepati_id], rev);
    }
}

void mimLNigiriReq(u_char mode, u_char rev)
{
    if (mim_nigiri_l_id == 0xff)
    {
        return;
    }

    if (mode == 0)
    {
        mimRequest(&ani_mdl[0].mim[mim_nigiri_l_id], rev);
    }
    else
    {
        mimRequestLastFrame(&ani_mdl[0].mim[mim_nigiri_l_id], rev);
    }
}

void mimRNigiriReq(u_char mode, u_char rev)
{
    if (mim_nigiri_r_id == 0xff)
    {
        return;
    }

    if (mode == 0)
    {
        mimRequest(&ani_mdl[0].mim[mim_nigiri_r_id], rev);
    }
    else
    {
        mimRequestLastFrame(&ani_mdl[0].mim[mim_nigiri_r_id], rev);
    }
}

void mimStop(MIME_CTRL *m_ctrl)
{
    m_ctrl->rev = 0;
    m_ctrl->frame = 0;
    m_ctrl->stat = 1;
}

void mimSetReso(MIME_CTRL *m_ctrl, u_char reso)
{
    m_ctrl->reso = reso;
}

void mimClearVertex(MIME_CTRL *m_ctrl)
{
    u_int i;
    u_int vtx_num;
    sceVu0FVECTOR *pkt;
    sceVu0FVECTOR *vtx;
    MIME_DAT *mdat;

    mdat = m_ctrl->mdat;

    vtx_num = mdat->vtx_num;

    vtx = mdat->vtx;
    pkt = mdat->pkt;

    for (i = 0; i < vtx_num; i++)
    {
        Vu0CopyVector(*pkt, *vtx);

        pkt++;
        vtx++;
    }
}

void mimCalcVertex(MIME_CTRL *m_ctrl, WMIM_CTRL *wmim, u_char clear_vtx_flg)
{
    u_int i;
    u_int j;
    u_int k;
    u_int vtx_num;
    u_int key_num;
    u_int vtx_ofs;
    sceVu0FVECTOR vec;
    sceVu0FVECTOR *key;
    sceVu0FVECTOR *pkt;
    sceVu0FVECTOR *key_top;
    u_int *ko_top;
    float weight;
    MIME_DAT *mdat;
    u_char acs_flg[6];
    u_int koblock_num;

    mdat = m_ctrl->mdat;

    key_num = mimGetKeymdlNum(mdat->dat);

    if (clear_vtx_flg != 0)
    {
        mimClearVertex(m_ctrl);
    }

    if (m_ctrl->flg & 1)
    {
        j = 0;
        for (i = 0; i < 6; i++)
        {
            acs_flg[j] = 0;

            if ((m_ctrl->flg >> (i+1)) & 1)
            {
                acs_flg[j] = i;
                j++;
            }
        }
    }

    for (i = 0; i < key_num; i++)
    {
        if (m_ctrl->flg & 1)
        {
            if (wmim == NULL)
            {
                return;
            }

            if (wmim->wdat == NULL)
            {
                continue;
            }

            vtx_ofs = acs_flg[i];

            if (vtx_ofs & 1)
            {
                weight = wmim->w[vtx_ofs / 2];
            }
            else
            {
                weight = -wmim->w[vtx_ofs / 2];
            }

            if (weight < 0.0f)
            {
                continue;
            }
        }
        else
        {
            weight = mimGetWavdata(mdat->dat, i, m_ctrl->frame);
        }

        if (weight < 0.00001f)
        {
            continue;
        }

        key_top = mimGetKeymdlTop(mdat->dat, i);

        koblock_num = ((u_int *)key_top)[0];

        pkt = &mdat->pkt[ko_top[1]];

        ko_top = (u_int *)&key_top[1];

        for (j = 0; j < koblock_num; j++)
        {
            vtx_num = ko_top[0];
            pkt = &mdat->pkt[ko_top[1]];
            key = (sceVu0FVECTOR *)(&ko_top[4]);

            for (k = 0; k < vtx_num; k++)
            {
                sceVu0ScaleVectorXYZ(vec, *key, weight);

                pkt[0][0] += vec[0];
                pkt[0][1] += vec[1];
                pkt[0][2] += vec[2];

                key++;
                pkt++;
            }

            ko_top = (u_int *)key;
        }
    }
}

void mimSetMimeCtrl(MIME_CTRL *m_ctrl, MIME_DAT *mdat, u_int furn_id, u_int parts_id)
{
    m_ctrl->furn_id = furn_id;
    m_ctrl->parts_id = parts_id;
    m_ctrl->frame = 0;
    m_ctrl->reso = 2;
    m_ctrl->cnt = 0;
    m_ctrl->mdat = mdat;
    m_ctrl->flg = mimGetFlg(mdat->dat);

    if (m_ctrl->flg == 1)
    {
        m_ctrl->stat = 0x2;
    }
    else
    {
        m_ctrl->stat = 1;
    }

    m_ctrl->loop = 0;
    m_ctrl->rev = 0;
}

u_int* mimSetMimeDat(MIME_DAT *mdat, u_int *mim_p, u_int *tmp_buf, u_int *mdl_p)
{
    PHEAD *ph;

    ph = (PHEAD *)mdl_p[4];

    mdat->dat = mim_p;
    mdat->pkt = ph->pUniqVertex;
    mdat->vtx = (sceVu0FVECTOR *)tmp_buf;

    // count how many `sceVu0FVECTOR` is `pUniqVertex` made of by subtracting it's start address
    // `mdat->pkt` (actually `ph->pUniqVertex`) from it's end address `ph->pUniqNormal`
    mdat->vtx_num = (*(u_int *)&ph->pUniqNormal - *(u_int *)&mdat->pkt) / sizeof(sceVu0FVECTOR);

    return (u_int *)&mdat->vtx[mdat->vtx_num];
}

void mimSetOriVertex(MIME_DAT *mdat)
{
    u_int i;
    u_int vtx_num;
    sceVu0FVECTOR *vtx;
    sceVu0FVECTOR *pkt;

    vtx_num = mdat->vtx_num;

    vtx = mdat->vtx;
    pkt = mdat->pkt;

    for (i = 0; i < vtx_num; i++)
    {
        Vu0CopyVector(*vtx, *pkt);

        pkt++;
        vtx++;
    }
}

void mimSetVertex(ANI_CTRL *ani_ctrl)
{
    MIME_DAT *mdat;
    u_int stat;
    u_int i;
    u_char flg[30];

    u_int mim_num; // not from the debugging symbols
    u_int bg_num; // not from the debugging symbols

    mim_num = ani_ctrl->mim_num;
    bg_num = ani_ctrl->bg_num;

    for (i = 0; i < 30; i++)
    {
        flg[i] = 1;

        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
    }

    for (i = 0; i < ani_ctrl->wmim_num; i++)
    {
        mimWeightCtrl(ani_ctrl, i, 1.0f);
    }

    for (i = 0; i < mim_num; i++)
    {
        MIME_CTRL *m_ctrl;

        if (ani_ctrl->mim == NULL)
        {
            return;
        }

        m_ctrl = &ani_ctrl->mim[i];

        mdat = m_ctrl->mdat;

        stat = m_ctrl->stat;

        if (stat == 2)
        {
            if (ani_ctrl->wmim == NULL)
            {
                mimCalcVertex(m_ctrl, NULL, flg[m_ctrl->parts_id]);
            }
            else
            {
                mimCalcVertex(m_ctrl, &ani_ctrl->wmim[m_ctrl->weight_id], flg[m_ctrl->parts_id]);
            }

            if ((m_ctrl->flg & 0x1) == 0)
            {
                mimAddFrame(m_ctrl, mdat->dat);
            }

            flg[m_ctrl->parts_id] = 0;
        }
    }

    for (i = 0; i < bg_num; i++)
    {
        MIME_CTRL *m_ctrl;

        if (ani_ctrl->bgmim == NULL)
        {
            return;
        }

        m_ctrl = &ani_ctrl->bgmim[i];

        mdat = m_ctrl->mdat;

        if (flg[m_ctrl->parts_id] == 1)
        {
            mimCalcVertex(m_ctrl, NULL, flg[m_ctrl->parts_id]);

            mimAddFrame(m_ctrl, mdat->dat);

            flg[m_ctrl->parts_id] = 0;
        }
    }
}

void mimClearToScene()
{
    u_int i;

    mimClearAllVertex(ani_mdl);

    for (i = 0; i < 4; i++)
    {
        if (ene_wrk[i].sta & 0x1)
        {
            mimClearAllVertex(motGetAniMdl(i));
        }
    }
}

void mimClearAllVertex(ANI_CTRL *ani_ctrl)
{
    u_int mim_num;
    u_int i;

    mim_num = ani_ctrl->mim_num;

    for (i = 0; i < mim_num; i++)
    {
        if (ani_ctrl->mim == NULL)
        {
            return;
        }

        mimClearVertex(&ani_ctrl->mim[i]);
    }
}

u_char mimAddFrame(MIME_CTRL *m_ctrl, u_int *dat)
{
    if (m_ctrl->reso == 0)
    {
        return 0;
    }

    m_ctrl->cnt++;

    if (m_ctrl->cnt < m_ctrl->reso)
    {
        return 0;
    }

    if (m_ctrl->rev == 0)
    {
        m_ctrl->frame += m_ctrl->cnt / m_ctrl->reso;

        m_ctrl->cnt = 0;

        if (m_ctrl->frame >= mimGetFrameNum(dat))
        {
            m_ctrl->frame = 0;

            if (m_ctrl->loop == 0)
            {
                mimStop(m_ctrl);
            }

            return 1;
        }
    }
    else
    {
        m_ctrl->frame -= m_ctrl->cnt / m_ctrl->reso;

        m_ctrl->cnt = 0;

        if (m_ctrl->frame <= -1)
        {
            m_ctrl->frame = mimGetFrameNum(dat) - 1;

            if (m_ctrl->loop == 0)
            {
                mimStop(m_ctrl);
            }

            return 1;
        }
    }

    return 0;
}

void SceneMimSetVertex(ANI_CTRL *ani_ctrl, u_int frame)
{
    MIME_DAT *mdat;
    u_int i;
    u_char flg[30];
    MIME_CTRL *m_ctrl;

    for (i = 0; i < 30; i++)
    {
        flg[i] = 1;

        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
    }

    for (i = 0; i < ani_ctrl->wmim_num; i++)
    {
        mimWeightCtrl(ani_ctrl, i, 1.0f);
    }

    for (i = 0; i < ani_ctrl->mim_num; i++)
    {
        if (ani_ctrl->mim == NULL)
        {
            return;
        }

        m_ctrl = &ani_ctrl->mim[i];

        mdat = m_ctrl->mdat;

        if (!(m_ctrl->flg & 0x1))
        {
            if (frame < mimGetFrameNum(mdat->dat))
            {
                m_ctrl->frame = frame;
            }
            else
            {
                frame = mimGetFrameNum(mdat->dat) - 1;

                m_ctrl->frame = frame;
            }
        }

        if (ani_ctrl->wmim == NULL)
        {
            mimCalcVertex(m_ctrl, NULL, flg[m_ctrl->parts_id]);
        }
        else
        {
            mimCalcVertex(m_ctrl, &ani_ctrl->wmim[m_ctrl->weight_id], flg[m_ctrl->parts_id]);
        }

        flg[m_ctrl->parts_id] = 0;
    }
}

void mimChodoInitWork()
{
    u_int i;

    for (i = 0; i < 20; i++)
    {
        mim_chodo[i].furn_id = -1;

        mim_chodo_se[i] = 0xFF;

        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
    }
}

void mimChodoSetWork(u_int furn_id, u_char room_no)
{
    u_int i;
    char room_id;
    u_char mim_no;

    mim_no = furn_dat[furn_id].acs_flg;

    if (!(mim_no & 0x80)) // sign bit is used as flag?
    {
        return;
    }

    mim_no &= 0x7f; // remove sign bit to get correct value

    for (i = 0; i < 2; i++)
    {
        if (room_no == room_load_block[i].room_no)
        {
            room_id = room_load_block[i].block_no;
            break;
        }

        if (i == 1 || room_id == -1)
        {
            return;
        }
    }

    for (i = 0; i < 20; i++)
    {
         if (mim_chodo[i].furn_id == 0xffff)
         {
             mimSetMimeCtrl(&mim_chodo[i], &mim_cdat[room_id][mim_no], furn_id, 0);
             break;
         }
    }
}

void mimChodoReleaseWork(u_int furn_id, u_char room_no)
{
    u_int i;

    if ((furn_dat[furn_id].acs_flg & 0x80))
    {
        for (i = 0; i < 20; i++)
        {
            if (mim_chodo[i].furn_id == furn_id)
            {
                mim_chodo[i].furn_id = -1;

                mim_chodo_se[i] = 0xFF;
                break;
            }
        }
    }
}

void mimInitChodo(u_int *mim_p, u_short *furn_id, u_char room_id, u_short room_no, u_int num)
{
    u_short i;
    u_short j;
    u_int *pkt_p;
    u_short mim_num;
    u_char *chodo_num;
    u_int fsize;
    u_int flg;
    chodo_num = mimchodo_tbl[room_no].dat;

    if (chodo_num == NULL)
    {
        return;
    }

    pkt_p = GetPakTaleAddr(mim_p);

    mim_num = mim_p[0];
    mim_p = &mim_p[4];

    i = 0;
    while (chodo_num[i] != 0xff)
    {
        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");

        i++;
    }

    if (i != mim_num)
    {
        return;
    }

    for (i = 0; i < mim_num; i++)
    {
        fsize = mim_p[0];
        mim_p = &mim_p[4];

        flg = 0;

        if (furn_addr_tbl[chodo_num[i]] != NULL)
        {

            for (j = 0; j < num; j++)
            {
                if (chodo_num[i] == furn_id[j])
                {
                    flg = 1;
                    break;
                }

                asm volatile ("nop");
                asm volatile ("nop");
                asm volatile ("nop");
            }

            if (flg)
            {
                if (chodo_num[i] == 0xff)
                {
                    return;
                }

                pkt_p = mimSetMimeDat(&mim_cdat[room_id][i], mim_p, pkt_p, furn_addr_tbl[chodo_num[i]]);
                mimAddressMapping(mim_cdat[room_id][i].dat);
                mimSetOriVertex(&mim_cdat[room_id][i]);

                mim_p += (fsize / 4);
            }
            else
            {
                mim_p += (fsize / 4);
            }
        }
        else
        {
            mim_p += (fsize / 4);
        }
    }
}

void mimDispChodo(MIME_CTRL *m_ctrl, u_int *mdl_p)
{
    MIME_DAT *mdat;

    mdat = m_ctrl->mdat;

    if (m_ctrl->stat == 2)
    {
        mimCalcVertex(m_ctrl, NULL, 0x1);

        if (m_ctrl->frame < 10)
        {
            switch (m_ctrl->furn_id)
            {
            case 270:
                if (ingame_wrk.clear_count != 0)
                {
                    VibrateRequest1(0, 1);
                }

                break;
            case 100:
            case 269:
            case 271:
                VibrateRequest1(0, 1);
                break;
            case 316:
                VibrateRequest1(0, 1);
                break;
            }
        }

        if (mimAddFrame(m_ctrl, mdat->dat) != 0)
        {
            mimChodoStop(m_ctrl->furn_id);
        }
    }

    SgSortUnit(mdl_p, -1);

    if (m_ctrl->stat != 2)
    {
        return;
    }

    mimClearVertex(m_ctrl);
}

u_char mimChodoChkExec(u_int furn_id)
{
    u_char i;

    for (i = 0; i < 20; i++)
    {
        if (furn_id == mim_chodo[i].furn_id && mim_chodo[i].stat == 2)
        {
            return 1;
        }
    }

    return 0;
}

void mimChodoRequest(FURN_ACT_WRK *fawp, u_char mode, u_char rev)
{
    u_char i;

    for (i = 0; i < 20; i++)
    {
        if (fawp->furn_id == mim_chodo[i].furn_id && !(fawp->furn_id == 270 && ingame_wrk.clear_count == 0))
        {
            switch (fawp->furn_id)
            {
            case 59:
                // do nothing
            break;
            case 100:
                SeStartPosEventFlame(21, 0, fawp->pos, 0, 4096, 4096);
                SeStartPosEventFlame(21, 1, fawp->pos, 0, 4096, 4096);
            break;
            case 269:
                SeStartPosEventFlame(9, 0, fawp->pos, 0, 4096, 4096);
            break;
            case 270:
                if (ingame_wrk.clear_count != 0)
                {
                    SeStartPosEventFlame(3, 0, fawp->pos, 0, 4096, 4096);
                }
            break;
            case 271:
                SeStartPosEventFlame(11, 1, fawp->pos, 0, 4096, 4096);
            break;
            case 316:
                SeStartPosEventFlame(7, 0, fawp->pos, 0, 4096, 4096);
            break;
            }

            if (mode == 0)
            {
                mimRequest(&mim_chodo[i], rev);
            }
            else
            {
                mimRequestLastFrame(&mim_chodo[i], rev);
            }

            return;
        }
    }
}

void mimChodoStop(u_int furn_id)
{
    u_char i;

    for (i = 0; i < 20; i++)
    {
        if (furn_id == mim_chodo[i].furn_id)
        {
            mimStop(&mim_chodo[i]);
            break;
        }
    }
}

void mimChodoSetReso(u_int furn_id, u_char reso)
{
    u_char i;

    for (i = 0; i < 20; i++)
    {
        if (furn_id == mim_chodo[i].furn_id)
        {
            mimSetReso((MIME_CTRL*)&mim_chodo[i].mdat, reso);
        }
    }
}

u_int mimGetFrameNum(u_int *mim_top)
{
    MIM_FILE_HEADER *mfh = (MIM_FILE_HEADER *)mim_top;

    return mfh->frame_num;
}

u_int mimGetKeymdlNum(u_int *mim_top)
{
    MIM_FILE_HEADER *mfh = (MIM_FILE_HEADER *)mim_top;

    return mfh->key_num;
}

u_int mimGetPartsNo(u_int *mim_top)
{
    MIM_FILE_HEADER *mfh = (MIM_FILE_HEADER *)mim_top;

    return mfh->parts_no;
}

u_int mimGetFlg(u_int *mim_top)
{
    MIM_FILE_HEADER *mfh = (MIM_FILE_HEADER *)mim_top;

    return mfh->flg;
}

u_char* mimGetWeightName(u_int *mim_top)
{
    MIM_FILE_HEADER *mfh = (MIM_FILE_HEADER *)mim_top;

    return mfh->w_name;
}

float mimGetWavdata(u_int *mim_top, u_int key_no, u_int frame)
{
    u_int *wav_addr;

    wav_addr = &mim_top[8];
    wav_addr = &wav_addr[key_no * 2];

    if (mim_top[1] == 0)
    {
        wav_addr = (u_int *)((int)wav_addr + (int)mim_top);
    }

    wav_addr = (u_int *)wav_addr[0];

    return ((float *)wav_addr)[frame];
}

sceVu0FVECTOR* mimGetKeymdlTop(u_int *mim_top, u_int key_no)
{
    u_int *vtx_addr;

    vtx_addr = &mim_top[key_no * 2];
    vtx_addr = &vtx_addr[9];

    if (mim_top[1] == 0)
    {
        vtx_addr = (u_int *)((int)vtx_addr + (int)mim_top);
    }

    return ((sceVu0FVECTOR **)vtx_addr)[0];
}

void mimAddressMapping(u_int *top_addr)
{
    MIM_FILE_HEADER *head_p;
    MIM_ADDR_TABLE *tbl_p;
    u_int i;

    head_p = (MIM_FILE_HEADER*)top_addr;

    tbl_p = (MIM_ADDR_TABLE *)(head_p + 1);

    if (*top_addr == 0x454D494D && head_p->map_flg != 1) // 0x454d494d is "EMIM" if interpreted as chars
    {
        for (i = 0; i < head_p->key_num; i++)
        {
            tbl_p->wav_addr = (u_int *)((u_int)head_p + (u_int)tbl_p->wav_addr);
            tbl_p->key_addr = (u_int *)((u_int)head_p + (u_int)tbl_p->key_addr);

            tbl_p++;
        }

        head_p->map_flg = 1;
    }
}

void mimPlyrMepatiCtrl()
{
    if (++plyr_mepati_cnt >= 300)
    {
        mimMepatiReq(0, 0);
        plyr_mepati_cnt = 0;
    }
}

void mimInitWeight(ANI_CTRL *ani_ctrl)
{
    u_int i;

    for (i = 0; i < ani_ctrl->wmim_num; i++)
    {
        ani_ctrl->wmim[i].v[0] = 0.0f;
        ani_ctrl->wmim[i].v[1] = 0.0f;
        ani_ctrl->wmim[i].v[2] = 0.0f;
        ani_ctrl->wmim[i].v[3] = 0.0f;

        ani_ctrl->wmim[i].w[0] = 0.0f;
        ani_ctrl->wmim[i].w[1] = 0.0f;
        ani_ctrl->wmim[i].w[2] = 0.0f;
        ani_ctrl->wmim[i].w[3] = 0.0f;
    }
}

void mimWeightCtrl(ANI_CTRL *ani_ctrl, u_int weight_id, float scale)
{
    u_int i;
    sceVu0FMATRIX m;
    sceVu0FVECTOR pos;
    sceVu0FVECTOR f;
    sceVu0FVECTOR reverse;
    sceVu0FVECTOR force;
    HeaderSection *hs;
    WMIM_CTRL *w_ctrl;
    WMIM_DAT *wdat;

    if (ani_ctrl->wmim == NULL)
    {
        return;
    }

    w_ctrl = &ani_ctrl->wmim[weight_id];
    wdat = w_ctrl->wdat;
    hs = (HeaderSection *)ani_ctrl->base_p;

    sceVu0CopyMatrix(m, hs->coordp[wdat->bone_id].lwmtx);
    sceVu0ApplyMatrix(pos, m, wdat->dat->pos);
    sceVu0SubVector(f, pos, w_ctrl->pbak);
    sceVu0CopyVector(w_ctrl->pbak, pos);
    sceVu0ScaleVector(f, f, wdat->dat->mass);
    sceVu0ScaleVector(force, wdat->dat->gravity, wdat->dat->mass);
    sceVu0AddVector(f, f, force);
    sceVu0ScaleVector(f, f, scale);

    if (sceVu0InnerProduct(f, f) > 0.8f)
    {
        mimInitWeight(ani_ctrl);
    }
    else
    {
        m[0][3] = m[1][3] = m[2][3] = 0.0f;
        m[3][0] = m[3][1] =m[3][2] = 0.0f;
        m[3][3] = 1.0f;

        sceVu0Normalize(m[0], m[0]);
        sceVu0Normalize(m[1], m[1]);
        sceVu0Normalize(m[2], m[2]);
        sceVu0InversMatrix(m, m);

        f[3] = 0.0f;

        sceVu0ApplyMatrix(f, m, f);
        sceVu0ScaleVector(reverse, w_ctrl->w, -wdat->dat->Ks);
        sceVu0AddVector(w_ctrl->v, w_ctrl->v, reverse);
        sceVu0ScaleVector(w_ctrl->v, w_ctrl->v, wdat->dat->dec);
        sceVu0AddVector(w_ctrl->w, w_ctrl->w, w_ctrl->v);
        sceVu0AddVector(w_ctrl->w, w_ctrl->w, f);

        for (i = 0; i < 3; i++)
        {
            if (1.0f < w_ctrl->w[i])
            {
                w_ctrl->w[i] = 1.0f;
            }

            if (w_ctrl->w[i] < -1.0f)
            {
                w_ctrl->w[i] = -1.0f;
            }
        }
    }
}
