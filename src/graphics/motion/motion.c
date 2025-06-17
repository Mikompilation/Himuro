#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "motion.h"

#include "sce/libvu0.h"

#include "main/glob.h"
#include "ingame/plyr/plyr_ctl.h"
#include "graphics/motion/mime.h"
#include "graphics/motion/mdldat.h"
#include "graphics/motion/motion.h"
#include "graphics/motion/mdlwork.h"
#include "graphics/motion/anicode.h"
#include "graphics/motion/accessory.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/sgsgd.h"
#include "graphics/graph3d/gra3d.h"

typedef struct {
    u_char file_id[4];
    u_int map_flg;
    u_int bone_num;
    u_int trans_num;
    u_int frame_num;
    u_int interp_frame;
    u_int flg;
    u_int si_frame;
} MOT_FILE_HEADER;

typedef struct {
    u_char parent_id;
    u_char trans_id;
} MOT_ID_TABLE;

typedef struct {
    u_int *frame_addr;
} MOT_ADDR_TABLE;

ENE_PKT_CTRL ene_pkt_ctrl[4] = {0};
ACS_ALPHA plyracs_ctrl[2] = {0};
MSN_PLYR_INIT plyr_init_ctrl = {0};
char plyr_mdl_no = 0;
u_char mim_mepati_id = 0;
u_char mim_nigiri_l_id = 0;
u_char mim_nigiri_r_id = 0;
float now_frot_x = 0.0f;

static sceVu0FMATRIX m_start[60];
static sceVu0FMATRIX m_end[60];


#define PI 3.1415927f
#define PI_HALF 1.5707964f

#define DEG2RAD(x) ((float)(x)*PI/180.0f)

#define PLAYER_ANM_ADDR 0x870000

void motInitPlayerAnm(char mdl_no)
{
    u_int *pkt_p;

    pkt_p = (u_int *)GetPakTaleAddr((void *)PLAYER_ANM_ADDR);

    motInitAniCtrl(ani_mdl, (u_int *)PLAYER_ANM_ADDR, pmanmpk[0], pkt_p, mdl_no, A000_MIKU);
    mimLNigiriReq(M001_MIM_LHAND_NIGIRI, 0);

    ani_mdl[0].mot.reso = 1;
}

void motInitEnemyAnm(u_int *anm_p, u_int mdl_no, u_int anm_no)
{
    if (mdl_no == M055_SYOUALL)
    {
        motInitMultiEnemyAnm(anm_p, M042_SYOUKI2, A042_SYOUKIA, 3);
    }
    else
    {
        motInitOneEnemyAnm(anm_p, mdl_no, anm_no);
    }
}

void motInitOneEnemyAnm(u_int *anm_p, u_int mdl_no, u_int anm_no)
{
    ANI_CTRL *ani_ctrl;
    u_int *mdl_p;
    u_int *pkt_p;
    u_char ret;

    ret = motSetAniMdlBuf(anm_no, anm_p);

    ani_ctrl = &ani_mdl[ret];
    mdl_p = pmanmodel[mdl_no];
    pkt_p = GetPakTaleAddr(anm_p);
    pkt_p = &pkt_p[ani_mdl_ctrl[ret].pkt_no * 0x4000];

    motInitAniCtrl(ani_ctrl, anm_p, mdl_p, pkt_p, mdl_no, anm_no);

    ret = 0; // FIXES instruction order
}

void motInitMultiEnemyAnm(u_int *anm_p, u_int mdl_no, u_int anm_no, u_int num)
{
    u_int i;

    if (num != 3)
    {
        num = 3;
    }

    for (i = 0; i < num; i++)
    {
        motInitOneEnemyAnm(anm_p, mdl_no + i, anm_no);
    }
}

void motInitEnemyMdl(u_int *mdl_p, u_int mdl_no)
{
    if (mdl_no == 55)
    {
        motInitMultiEnemyMdl(mdl_p, 42);
    }
    else if (mdl_no > 66)
    {
        motInitFlyMdl(mdl_p, mdl_no);
    }
    else
    {
        motInitOneEnemyMdl(mdl_p, mdl_no);
    }
}

void motInitOneEnemyMdl(u_int *mdl_p, u_int mdl_no)
{
    u_int i;
    u_int pack_num;
    u_int *top;

    pack_num = *mdl_p;
    pmanmodel[mdl_no] = mdl_p;

    for (i = 0; i < pack_num; i++)
    {
        top = (u_int *)GetFileInPak(mdl_p, i);

        switch (top[-3]) {
        case 0:
            // do nothing
        break;
        case 5:
            pmanmpk[mdl_no] = top;
        break;
        case 6:
            pmanpk2[mdl_no] = top;
        break;
        }
    }

    MpkMapUnit(pmanmpk[mdl_no]);
}

void motInitMultiEnemyMdl(u_int *mdl_addr, u_int mdl_no)
{
    u_int i;
    u_int mdl_num;
    u_int *mdl_p;

    mdl_num = *mdl_addr;

    for (i = 0; i < mdl_num; i++)
    {
        mdl_p = (u_int *)GetFileInPak(mdl_addr,i);
        motInitOneEnemyMdl(mdl_p, mdl_no + i);
    }
}

void motInitFlyMdl(u_int *mdl_p, u_int mdl_no)
{
    if (mdl_no == 67)
    {
        motInitMultiFlyMdl(mdl_p, 67);
    }
    else
    {
        motInitOneFlyMdl(mdl_p, mdl_no);
    }
}

void motInitMultiFlyMdl(u_int *mpk_p, u_int mdl_no)
{
    u_int i;
    u_int mdl_num; u_int *sgd_p;

    mdl_num = *mpk_p;

    for (i = 0; i < mdl_num; i++)
    {
        sgd_p = (u_int *)GetFileInPak(mpk_p, i);
        motInitOneFlyMdl(sgd_p, mdl_no + i);
    }
}

void motInitOneFlyMdl(u_int *sgd_p, u_int mdl_no)
{
    pmanmodel[mdl_no] = sgd_p;
    SgMapUnit(sgd_p);
}

static void motInitClearAniCtrl(ANI_CTRL *ani_ctrl)
{
    ani_ctrl->mdl_p = NULL;
    ani_ctrl->mpk_p = NULL;
    ani_ctrl->pk2_p = NULL;
    ani_ctrl->base_p = NULL;
    ani_ctrl->mtop = NULL;
    ani_ctrl->mdat = NULL;
    ani_ctrl->tanm_p = NULL;
    ani_ctrl->mot_num = 0;
    ani_ctrl->mim_num = 0;
    ani_ctrl->bg_num = 0;
    ani_ctrl->wmim_num = 0;
    ani_ctrl->mim = NULL;
    ani_ctrl->wmim = NULL;
    ani_ctrl->bgmim = NULL;
    ani_ctrl->ftype = 0;
    ani_ctrl->interp_flg = 1;
}

u_int* motInitAniCtrl(ANI_CTRL *ani_ctrl, u_int *anm_p, u_int *mdl_p, u_int *pkt_p, u_int mdl_no, u_int anm_no)
{
    u_int *top;
    u_int i;
    u_int pack_num;
    MIME_DAT *mim_dat;
    MIME_DAT *mim_bg;
    u_int num;
    u_int *tmp_p;
    HeaderSection *hs;

    ani_ctrl->anm_p = anm_p;
    pack_num = *anm_p;

    motSetAnime(ani_ctrl, anm_tbl[anm_no], 0);
    motInitClearAniCtrl(ani_ctrl);

    if (mdl_no == 0 || (mdl_no == 1 && plyr_init_ctrl.msn_no == 0))
    {
        ani_ctrl->mpk_p = mdl_p;
    }
    else
    {
        num = *mdl_p;
        ani_ctrl->mdl_p = mdl_p;

        for (i = 0; i < num; i++) {
            top = (u_int *)GetFileInPak(mdl_p, i);

            switch (top[-3]) {
            case 4:
                ani_ctrl->tanm_p = top;
            break;
            case 5:
                ani_ctrl->mpk_p = top;
            break;
            case 6:
                ani_ctrl->pk2_p = top;
            break;
            }
        }
    }

    ani_ctrl->base_p = &ani_ctrl->mpk_p[8];

    for (i = 0; i < pack_num; i++)
    {
        top = (u_int *)GetFileInPak(anm_p, i);

        switch (top[-3])
        {
        case 0:
            ani_ctrl->mot_num = *top;

            pkt_p = motInitMotCtrl(&ani_ctrl->mot, top, pkt_p);
        break;
        case 1:
            ani_ctrl->mim_num = *top;

            ani_ctrl->mim = (MIME_CTRL *)motAlign128(pkt_p);
            ani_ctrl->wmim = (WMIM_CTRL *)motAlign128((u_int *)&ani_ctrl->mim[20]);
            mim_dat = (MIME_DAT *)motAlign128((u_int *)&ani_ctrl->wmim[10]);
            tmp_p = motAlign128((u_int *)&mim_dat[20]);

            pkt_p = mimInitMimeCtrl(ani_ctrl->mim, mim_dat, top, ani_ctrl->mpk_p, tmp_p);

            mimInitAcsCtrl(ani_ctrl, mdl_no);
        break;
        case 2:
            ani_ctrl->bg_num = *top;

            ani_ctrl->bgmim = (MIME_CTRL *)motAlign128(pkt_p);
            tmp_p = motAlign128((u_int *)&ani_ctrl->bgmim[10]);
            mim_bg = (MIME_DAT *)motAlign128(tmp_p);
            tmp_p = motAlign128((u_int *)&mim_bg[20]);

            pkt_p = mimInitMimeCtrl(ani_ctrl->bgmim, mim_bg, top, ani_ctrl->mpk_p, tmp_p);
        break;
        case 3:
            ani_ctrl->mtop = top;
            ani_ctrl->mdat = (u_int *)GetFileInPak(top, 0);
        break;
        }
    }

    if (manmdl_dat[mdl_no].cdat != NULL)
    {
        ani_ctrl->cloth_ctrl = (CLOTH_CTRL *)motAlign128(pkt_p);
        top = motAlign128((u_int *)&ani_ctrl->cloth_ctrl[10]);

        pkt_p = acsInitCloth(ani_ctrl->cloth_ctrl, ani_ctrl->mpk_p, top, mdl_no);
    }

    hs = (HeaderSection *)ani_ctrl->base_p;

    motSetHierarchy(hs->coordp, ani_ctrl->mot.dat);

    return pkt_p;
}

u_int* motInitMotCtrl(MOT_CTRL *m_ctrl, u_int *mot_addr, u_int *rst_addr)
{
    u_short i;
    u_int *tale_addr;
    u_int *top_addr;

    m_ctrl->play_id = 0;
    m_ctrl->top = mot_addr;
    m_ctrl->dat = (u_int *)GetFileInPak(mot_addr, 0);

    for (i = 0; i < *mot_addr; i++)
    {
        top_addr = (u_int *)GetFileInPak(m_ctrl->top, i);
        motAddressMapping(top_addr);
    }

    m_ctrl->cnt = 0;
    m_ctrl->all_cnt = motGetFrameNum(m_ctrl->dat);
    m_ctrl->inp_cnt = 0;
    m_ctrl->inp_allcnt = (motGetInterpFrameNum(m_ctrl->dat) + 1) * 2;
    m_ctrl->reso = 0;
    m_ctrl->reso_cnt = 0;

    if (rst_addr != NULL)
    {
        m_ctrl->rst0 = (RST_DATA *)rst_addr;
        m_ctrl->rst1 = m_ctrl->rst0 + motGetBoneNum(m_ctrl->dat);

        motGetFrameData(m_ctrl->rst0, m_ctrl->dat, 0);
        motGetFrameData(m_ctrl->rst1, m_ctrl->dat, 0);

        tale_addr = (u_int *)(m_ctrl->rst1 + motGetBoneNum(m_ctrl->dat));
    }

    return tale_addr;
}

void motSetCoordCamera(ANI_CTRL *ani_ctrl)
{
    float sub;
    float rot;
    MOT_CTRL *m_ctrl;
    u_int frame;

    m_ctrl = &ani_ctrl->mot;

    if (ani_ctrl->interp_flg == 1)
    {
        motSetInterpMatrix(ani_ctrl, m_start, m_end);
        motInterpAnm(ani_ctrl, m_start, m_end);

        if (motAddFrame(m_ctrl) != 0)
        {
            ani_ctrl->interp_flg = 0;
        }
    }
    else
    {
        sub = plyr_wrk.frot_x - now_frot_x;

        if (__builtin_fabsf(sub) > 0.02f)
        {
            if (sub > 0.0f)
            {
                sub = 0.02f;
            }
            else
            {
                sub = -0.02f;
            }
        }

        now_frot_x += sub;
        rot = -now_frot_x;
        frame = (rot / DEG2RAD(60) + 1.0f) * 40.0f * 0.5f;

        motSetCoordFrame(ani_ctrl, frame);
    }
}

u_char motSetCoord(ANI_CTRL *ani_ctrl, u_char work_id)
{
    MOT_CTRL *m_ctrl;
    u_char ani_end;
#ifdef BUILD_EU_VERSION
    u_int i;
    u_int loop;
#endif
    ANI_CTRL tmp;
    u_int frame_num;
    u_int interp_cnt;
    u_int interp_frame_num;
    u_int si_frame_num;
    u_int bone_num;

    m_ctrl = &ani_ctrl->mot;

    motSetInterpMatrix(ani_ctrl, m_start, m_end);
    motInterpAnm(ani_ctrl, m_start, m_end);

    ani_end = 0;
#ifdef BUILD_EU_VERSION
    loop = 1;
#endif

    if (plyr_wrk.mvsta & 0x200000 && ingame_wrk.mode != 6)
    {
        return 0;
    }

#ifdef BUILD_EU_VERSION
    if (work_id == 0xff)
    {
        switch (m_ctrl->play_id)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 8:
        case 9:
        case 74:
        case 75:
        case 76:
        case 77:
        case 78:
        case 79:
        case 80:
        case 81:
        case 95:
        case 96:
            if (motGetNowFrame(&ani_ctrl->mot) % 6 == 4)
            {
                if (sys_wrk.pal_disp_mode == 0)
                {
                    loop = 2;
                }
            }
        break;
        }
    }
#endif

#ifdef BUILD_EU_VERSION
    for (i = 0; i < loop; i++)
    {
#endif
        if (motAddFrame(&ani_ctrl->mot) != 0)
        {
            m_ctrl->cnt++;

            if (ani_ctrl->interp_flg == 1)
            {
                motInitNewMotion(ani_ctrl);
            }
            else if (m_ctrl->cnt == m_ctrl->all_cnt - 1 || m_ctrl->all_cnt < 2)
            {
                tmp = *ani_ctrl;

                if (motGetNextMotion(&tmp) != 0)
                {
                    m_ctrl->inp_cnt = m_ctrl->inp_allcnt - 1;

                    if (m_ctrl->cnt >= 1)
                    {
                        m_ctrl->cnt--;
                    }
                    else
                    {
                        m_ctrl->cnt = 0;
                    }

                    ani_end = 1;
                }
                else
                {
                    interp_cnt = motGetInterpFrameNum(m_ctrl->dat) + 1;
                    interp_frame_num = (motGetInterpFrameNum(m_ctrl->dat) + 1) * 2;
                    frame_num = motGetFrameNum(m_ctrl->dat);
                    si_frame_num = motGetSIFrameNum(m_ctrl->dat);

                    m_ctrl->inp_cnt = 0;
                    m_ctrl->inp_allcnt = interp_frame_num - (interp_cnt * frame_num - si_frame_num) * 2;

                    ani_ctrl->mot.dat = tmp.mot.dat;
                }
            }
            else if (m_ctrl->cnt >= m_ctrl->all_cnt)
            {
                motInitNewMotion(ani_ctrl);

                if (work_id == 0xff)
                {
                    switch(m_ctrl->play_id)
                    {
                    case 98:
                    case 99:
                    case 101:
                    case 102:
                    case 111:
                    case 112:
                    case 114:
                    case 115:
                        // do nothing
                    break;
                    default:
                        plyr_wrk.mvsta &= ~0x200000;
                    break;
                    }
                }
            }
            else
            {
                m_ctrl->inp_cnt = 0;
                m_ctrl->inp_allcnt = (motGetInterpFrameNum(m_ctrl->dat) + 1) * 2;
            }

            bone_num = motGetBoneNum(m_ctrl->dat) * 48;

            memmove(m_ctrl->rst0, m_ctrl->rst1, bone_num);

            motGetFrameData(m_ctrl->rst1, m_ctrl->dat, (m_ctrl->cnt + 1) % m_ctrl->all_cnt);
        }

        if (ani_end != 1)
        {
            motAniTimerCodeExec(ani_ctrl);
        }
#ifdef BUILD_EU_VERSION
    }
#endif

    return ani_end;
}

static void motInitNewMotion(ANI_CTRL *ani_ctrl)
{
    MOT_CTRL *m_ctrl;

    m_ctrl = &ani_ctrl->mot;

    motAniCodeRead(ani_ctrl);

    m_ctrl->all_cnt = motGetFrameNum(m_ctrl->dat);
    m_ctrl->inp_cnt = 0;
    m_ctrl->inp_allcnt = (motGetInterpFrameNum(m_ctrl->dat) + 1) * 2;
    m_ctrl->cnt = 0;
}

static u_char motAddFrame(MOT_CTRL *m_ctrl)
{
    if (m_ctrl->reso == 0)
    {
        return 0;
    }

    m_ctrl->reso_cnt++;

    if (m_ctrl->reso <= m_ctrl->reso_cnt)
    {
        m_ctrl->inp_cnt += m_ctrl->reso_cnt / m_ctrl->reso;
        m_ctrl->reso_cnt = 0;

        if (m_ctrl->inp_allcnt <= m_ctrl->inp_cnt)
        {
            m_ctrl->inp_cnt = 0;
            return 1;
        }
    }

    return 0;
}

u_int motGetNowFrame(MOT_CTRL *m_ctrl)
{
    u_int interp_frame_num;

    interp_frame_num = (motGetInterpFrameNum(m_ctrl->dat) + 1) * 2;

    return m_ctrl->cnt * interp_frame_num + m_ctrl->inp_cnt;
}

void ReqPlayerAnime(u_char flame)
{
    if (plyr_wrk.anime_no == ani_mdl[0].anm.playnum)
    {
        return;
    }

    mimInitLoop(ani_mdl);

    if (
        (
            plyr_wrk.anime_no  < 10 ||
            plyr_wrk.anime_no == 85 || plyr_wrk.anime_no == 107  ||
            plyr_wrk.anime_no == 96 || plyr_wrk.anime_no ==  97  ||
            (plyr_wrk.anime_no > 47 && plyr_wrk.anime_no  <  56) ||
            (plyr_wrk.anime_no > 68 && plyr_wrk.anime_no  <  77)
        ) &&
        !(
            ani_mdl[0].anm.playnum  < 10 ||
            ani_mdl[0].anm.playnum == 85 || ani_mdl[0].anm.playnum == 107  ||
            ani_mdl[0].anm.playnum == 96 || ani_mdl[0].anm.playnum ==  97  ||
            (ani_mdl[0].anm.playnum > 47 && ani_mdl[0].anm.playnum  <  56) ||
            (ani_mdl[0].anm.playnum > 68 && ani_mdl[0].anm.playnum  <  77)
        )
    )
    {
        plyracs_ctrl[1].stat = 0;
        mimLNigiriReq(1, 0);
    }
    else if (
        !(
            plyr_wrk.anime_no  < 10 ||
            plyr_wrk.anime_no == 85 || plyr_wrk.anime_no == 107  ||
            plyr_wrk.anime_no == 96 || plyr_wrk.anime_no ==  97  ||
            (plyr_wrk.anime_no > 47 && plyr_wrk.anime_no  <  56) ||
            (plyr_wrk.anime_no > 68 && plyr_wrk.anime_no  <  77)
        ) &&
        (
            ani_mdl[0].anm.playnum  < 10 ||
            ani_mdl[0].anm.playnum == 85 || ani_mdl[0].anm.playnum == 107 ||
            ani_mdl[0].anm.playnum == 96 || ani_mdl[0].anm.playnum ==  97 ||
            (ani_mdl[0].anm.playnum > 47 && ani_mdl[0].anm.playnum  < 56) ||
            (ani_mdl[0].anm.playnum > 68 && ani_mdl[0].anm.playnum  < 77)
        )
    )
    {
        mimLNigiriReq(1, 1);
        plyracs_ctrl[1].stat = 1;
    }

    if (plyr_wrk.anime_no > 9 && plyr_wrk.anime_no < 26)
    {
        mimRNigiriReq(0, 0);
    }
    else if (ani_mdl[0].anm.playnum > 9 && ani_mdl[0].anm.playnum < 26)
    {
        mimRNigiriReq(1, 1);
    }

    if (plyr_wrk.anime_no == 107)
    {
        motPlyrActReq(4, NULL, 0);
    }

    if (
        plyr_wrk.anime_no == 65 || plyr_wrk.anime_no == 67 || plyr_wrk.anime_no == 103 ||
        (plyr_wrk.anime_no > 76 && plyr_wrk.anime_no < 85))
    {
        plyracs_ctrl[0].stat = 0;
    }
    else
    {
        plyracs_ctrl[0].stat = 1;
    }

    if (plyr_mdl_no == 0)
    {
        motSetAnime(ani_mdl, anm_tbl[0], plyr_wrk.anime_no);
    }
    else
    {
        motSetAnime(ani_mdl, anm_tbl[1], plyr_wrk.anime_no);
    }

    motInitInterpAnime(ani_mdl, flame);

}

void ReqEnemyAnime(u_char work_no, u_char flame)
{
    ENE_WRK *e_work;
    ANI_CTRL *ani_ctrl;
    u_char anm_no;

    e_work = &ene_wrk[work_no];
    anm_no = e_work->dat->anm_no;

    ani_ctrl = motGetAniMdl(work_no);

    mimInitLoop(ani_ctrl);
    motSetAnime(ani_ctrl, anm_tbl[anm_no], e_work->anime_no);
    motInitInterpAnime(ani_ctrl, flame);
}

void motSetAnime(ANI_CTRL *ani_ctrl, ANI_CODE **tbl, int req_no)
{
    ANI_CODE *ap;

    ap = tbl[req_no];

    ani_ctrl->anm.code_now = ani_ctrl->anm.code_head = ap;
    ani_ctrl->anm.playnum = req_no;
    ani_ctrl->anm.timer = 0;
    ani_ctrl->anm.stat = 0;
    ani_ctrl->anm.loop_rest = 0;

    motAniCodeClearBuf(ani_ctrl);

    ani_ctrl->interp_flg = 1;
}

void* GetFileInPak(void *pak_head, int num)
{
    int file_num;
    int file_size;
    int i;
    char *fp;

    file_num = *(int *)pak_head;

    fp = NULL;

    if (num < file_num)
    {
        fp = (char *)pak_head + 0x10;

        for (i = 0; i < num; i++)
        {
            file_size = *(int *)fp;

            fp += 0x10;
            fp += file_size;

            asm("nop");
            asm("nop");
            asm("nop");
        }

        fp += 0x10;
    }

    return fp;
}

void* GetPakTaleAddr(void *pak_head)
{
    int file_num;
    int file_size;
    int i;
    char *fp;

    file_num = *(int *)pak_head;

    fp = (char *)pak_head;
    fp += 16;

    for (i = 0; i < file_num; i++)
    {
        file_size = *(int *)fp;

        fp += 16;
        fp += file_size;

        asm("nop");
        asm("nop");
        asm("nop");
    }

    return motAlign128((u_int *)(fp + 16));
}

void GetMdlNeckPos(sceVu0FVECTOR pos, ANI_CTRL *ani_ctrl, u_short mdl_no)
{
    u_short id;
    static sceVu0FVECTOR p = {0.0f, 0.0f, 0.0f, 1.0f};
    HeaderSection *hs;

    id = manmdl_dat[mdl_no].neck_id;

    switch(mdl_no) {
    case 58:
        p[0] = 2.25f;
    break;
    case 10:
        p[0] = 7.5f;
    break;
    case 27:
        p[0] = 6.5f;
    break;
    case 20:
        p[0] = 9.5f;
    break;
    case 18:
        p[0] = 4.0f;
    break;
    case 11:
        p[0] = 8.0f;
    break;
    case 12:
        p[0] = 8.0f;
    break;
    case 24:
        p[0] = 8.0f;
    break;
    case 23:
        p[0] = 6.0f;
    break;
    case 14:
        p[0] = 5.5f;
    break;
    case 16:
        p[0] = 8.5f;
    break;
    case 33:
        p[0] = 8.0f;
    break;
    case 22:
        p[0] = 3.0f;
    break;
    default:
        p[0] = 5.0f;
    break;
    }

    p[0] /= manmdl_dat[mdl_no].scale;

    hs = (HeaderSection *)ani_ctrl->base_p;

    sceVu0ApplyMatrix(pos, hs->coordp[id].lwmtx, p);
}

u_int GetMdlBonePos(sceVu0FVECTOR *pos, u_char work_no)
{
    HeaderSection *hs;
    u_int i;
    ANI_CTRL *ani_ctrl;

    ani_ctrl = motGetAniMdl(work_no);

    hs = (HeaderSection *)ani_ctrl->base_p;

    for (i = 0; i < hs->blocks - 1; i++)
    {
        Vu0CopyVector(pos[i], hs->coordp[i].lwmtx[3]);
    }

    return hs->blocks - 1;
}

void GetMdlWaistPos(sceVu0FVECTOR pos, ANI_CTRL *ani_ctrl, u_short mdl_no)
{
    u_char id;
    HeaderSection *hs;

    id = manmdl_dat[mdl_no].waist_id;
    hs = (HeaderSection *)ani_ctrl->base_p;

    Vu0CopyVector(pos, hs->coordp[id].lwmtx[3]);
}

void GetMdlLegPos(sceVu0FVECTOR pos, ANI_CTRL *ani_ctrl, u_short mdl_no)
{
    float *v1;
    u_char id;
    HeaderSection *hs;

    id = manmdl_dat[mdl_no].leg_id;
    hs = (HeaderSection *)ani_ctrl->base_p;

    Vu0CopyVector(pos, hs->coordp[id].lwmtx[3]);
}

void GetMdlShldPos(sceVu0FVECTOR pos, ANI_CTRL *ani_ctrl, u_char lr)
{
    u_int id;
    HeaderSection *hs;

    id = lr + 1;
    hs = (HeaderSection *)ani_ctrl->base_p;

    Vu0CopyVector(pos, hs->coordp[id].lwmtx[3]);
}

void GetPlyrFootPos(sceVu0FVECTOR pos, ANI_CTRL *ani_ctrl, u_char lr)
{
    u_char id;
    HeaderSection *hs;

    id = lr + 10;
    hs = (HeaderSection *)ani_ctrl->base_p;

    Vu0CopyVector(pos, hs->coordp[id].lwmtx[3]);
}

void GetPlyrAcsLightPos(sceVu0FVECTOR pos, ANI_CTRL *ani_ctrl)
{
    static sceVu0FVECTOR p = {1.99221f, 0.9141f, 0.2095f, 1.0f};
    HeaderSection *hs;

    hs = (HeaderSection *)ani_ctrl->base_p;

    sceVu0ApplyMatrix(pos, hs->coordp[5].lwmtx, p);
}

void GetToushuKatanaPos(sceVu0FVECTOR p0, sceVu0FVECTOR p1, ANI_CTRL *ani_ctrl)
{
    static sceVu0FVECTOR ofs0 = {1.4f, 1.4f, 0.3f, 1.0f};
    static sceVu0FVECTOR ofs1 = {1.81f, 18.0f, -6.0f, 1.0f};

    sceVu0ApplyMatrix(p0, ((HeaderSection *)ani_ctrl->base_p)->coordp[5].lwmtx, ofs0);
    sceVu0ApplyMatrix(p1, ((HeaderSection *)ani_ctrl->base_p)->coordp[5].lwmtx, ofs1);
}

u_short GetPlyrFtype()
{
    return ani_mdl[0].ftype;
}

void motInitInterpAnime(ANI_CTRL *ani_ctrl, int flame)
{
    int args[3];
    ANI_CODE *ap;
    MOT_CTRL *m_ctrl;
    u_int frame;

    m_ctrl = &ani_ctrl->mot;

    m_ctrl->inp_cnt = 0;
    m_ctrl->inp_allcnt = flame;
    m_ctrl->reso_cnt = 0;
    m_ctrl->cnt = 0;

    for (ap = ani_ctrl->anm.code_head; motAniCodeIsEnd(*ap) == 0; ap++)
    {
        if (*ap >> 12 == 2)
        {
            GetAniCodeArgs(*ap, args);

            m_ctrl->dat = (u_int *)GetFileInPak(m_ctrl->top, args[0]);
            m_ctrl->play_id = args[0];

            if (ani_ctrl->mtop != NULL)
            {
                ani_ctrl->mdat = (u_int *)GetFileInPak(ani_ctrl->mtop, args[0]);
            }

            break;
        }
    }

    frame = m_ctrl->play_id != 64 ? 0 : 5;

    motGetFrameData(m_ctrl->rst1, m_ctrl->dat, frame);
}

static void motSetInterpMatrix(ANI_CTRL *ani_ctrl, sceVu0FMATRIX *start, sceVu0FMATRIX *end)
{
    HeaderSection *hs;
    sceVu0FMATRIX m0;
    sceVu0FMATRIX m1;
    MOT_CTRL *m_ctrl;
    u_int i;

    m_ctrl = &ani_ctrl->mot;

    hs = (HeaderSection *)ani_ctrl->base_p;

    for (i = 0; i < hs->blocks - 2; i++)
    {
        sceVu0UnitMatrix(m0);
        sceVu0UnitMatrix(m1);

        sceRotMatrixXYZ(m0, m0, m_ctrl->rst0[i].rot);

        sceVu0Normalize(m0[0], m0[0]);
        sceVu0Normalize(m0[1], m0[1]);
        sceVu0Normalize(m0[2], m0[2]);

        sceRotMatrixXYZ(m1, m1, m_ctrl->rst1[i].rot);

        sceVu0Normalize(m1[0], m1[0]);
        sceVu0Normalize(m1[1], m1[1]);
        sceVu0Normalize(m1[2], m1[2]);

        Vu0CopyMatrix(start[i], m0);
        Vu0CopyMatrix(end[i], m1);
    }
}

static void motInterpAnm(ANI_CTRL *ani_ctrl, sceVu0FMATRIX *start, sceVu0FMATRIX *end)
{
    short int i;
    sceVu0FVECTOR rot;
    sceVu0FVECTOR trans;
    sceVu0FVECTOR scale;
    sceVu0FMATRIX m0;
    sceVu0FMATRIX m1;
    sceVu0FMATRIX interp;
    HeaderSection *hs;
    SgCOORDUNIT *coord;
    float rate;
    MOT_CTRL *m_ctrl;

    m_ctrl = &ani_ctrl->mot;

    hs = (HeaderSection *)ani_ctrl->base_p;

    coord = hs->coordp;

    if (m_ctrl->inp_allcnt != 0)
    {
        rate = (float)m_ctrl->inp_cnt / (float)m_ctrl->inp_allcnt;
    }
    else
    {
        rate = 1.0f;
    }

    for (i = 0; i < hs->blocks - 1; i++)
    {
        coord[i].flg = 0;

        if (i == 0)
        {
            sceVu0UnitMatrix(coord[i].matrix);
        }
        else
        {
            sceVu0UnitMatrix(interp);
            motInterpMatrix(interp, start[i-1], end[i-1], rate);
            sceVu0InterVector(trans, m_ctrl->rst1[i-1].trans, m_ctrl->rst0[i-1].trans, rate);
            sceVu0InterVector(scale, m_ctrl->rst1[i-1].scale, m_ctrl->rst0[i-1].scale, rate);

            sceVu0UnitMatrix(coord[i].matrix);

            coord[i].matrix[0][0] = scale[0];
            coord[i].matrix[1][1] = scale[1];
            coord[i].matrix[2][2] = scale[2];
            coord[i].matrix[3][3] = 1.0f;

            sceVu0MulMatrix(coord[i].matrix, coord[i].matrix, interp);
            sceVu0TransMatrix(coord[i].matrix, coord[i].matrix, trans);
        }
    }
}

void motInterpMatrix(sceVu0FMATRIX interp, sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rate)
{
    short int i;
    sceVu0FMATRIX m;
    sceVu0FVECTOR v;
    float r;
    float cos;
    float sin;
    float val;

    for (i = 0; i < 2; i++)
    {
        sceVu0OuterProduct(v, m0[i], m1[i]);
        sceVu0Normalize(v, v);

        val = sceVu0InnerProduct(m0[i], m1[i]);
        r = SgACosf(val) * rate;

        cos = SgCosf(r);
        sin = SgSinf(r);

        val = 1.0f - cos;

        sceVu0UnitMatrix(m);

        m[0][0] = v[0] * v[0] * val + cos;
        m[1][0] = v[0] * v[1] * val - v[2] * sin;
        m[2][0] = v[0] * v[2] * val + v[1] * sin;
        m[0][1] = v[0] * v[1] * val + v[2] * sin;
        m[1][1] = v[1] * v[1] * val + cos;
        m[2][1] = v[1] * v[2] * val - v[0] * sin;
        m[0][2] = v[0] * v[2] * val - v[1] * sin;
        m[1][2] = v[1] * v[2] * val + v[0] * sin;
        m[2][2] = v[2] * v[2] * val + cos;

        sceVu0ApplyMatrix(interp[i], m, m0[i]);
        sceVu0Normalize(interp[i], interp[i]);
    }

    sceVu0OuterProduct(interp[2], interp[0], interp[1]);
    sceVu0Normalize(interp[2], interp[2]);
}

void motMatrix2Quaternion(sceVu0FVECTOR q, sceVu0FMATRIX m)
{
    float tr;
    float s;
    int i;
    int j;
    int k;
    int nxt[3] = {1, 2, 0};

    sceVu0TransposeMatrix(m, m);

    tr = m[0][0] + m[1][1] + m[2][2];

    if (tr > 0.0f)
    {
        s = SgSqrtf(tr + 1.0f);

        q[3] = s * 0.5f;

        s = 0.5f / s;

        q[0] = (m[1][2] - m[2][1]) * s;
        q[1] = (m[2][0] - m[0][2]) * s;
        q[2] = (m[0][1] - m[1][0]) * s;
    }
    else
    {
        i = 0;

        if (m[0][0] < m[1][1])
        {
            i = 1;
        }

        if (m[i][i] < m[2][2])
        {
            i = 2;
        }

        j = nxt[i];
        k = nxt[j];

        s = SgSqrtf(m[i][i] - (m[j][j] + m[k][k]) + 1.0); // double!

        q[i] = s * 0.5f;

        if (s != 0.0f)
        {
            s = 0.5f / s;
        }

        q[3] = (m[j][k] - m[k][j]) * s;
        q[j] = (m[i][j] + m[j][i]) * s;
        q[k] = (m[i][k] + m[k][i]) * s;
    }
}

void motQuaternion2Matrix(sceVu0FMATRIX m, sceVu0FVECTOR q) {
    float wx;
    float wy;
    float wz;
    float xx;
    float yy;
    float yz;
    float xy;
    float xz;
    float zz;
    float x2;
    float y2;
    float z2;

    x2 = q[0] *  2; y2 = q[1] *  2; z2 = q[2] *  2;
    xx = q[0] * x2; xy = q[0] * y2; xz = q[0] * z2;
    yy = q[1] * y2; yz = q[1] * z2; zz = q[2] * z2;
    wx = q[3] * x2; wy = q[3] * y2; wz = q[3] * z2;

    m[0][0] = 1.0f - (yy + zz);
    m[0][1] = xy - wz;
    m[0][2] = xz + wy;
    m[0][3] = 0.0f;

    m[1][0] = xy + wz;
    m[1][1] = 1.0f - (xx + zz);
    m[1][2] = yz - wx;
    m[1][3] = 0.0f;

    m[2][0] = xz - wy;
    m[2][1] = yz + wx;
    m[2][2] = 1.0f - (xx + yy);
    m[2][3] = 0.0f;

    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}

void motQuaternionSlerp(sceVu0FVECTOR q, sceVu0FVECTOR q1, sceVu0FVECTOR q2, float rate)
{
    sceVu0FVECTOR ret;
    float cosom;

    sceVu0MulVector(ret, q1, q2);

    cosom = ret[0] + ret[1] + ret[2] + ret[3];

    if (cosom < 0.0f)
    {
        sceVu0ScaleVector(q2, q2, -1.0f);
    }

    sceVu0InterVector(q, q1, q2, 1.0f - rate);
}

void LocalRotMatrixX(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rx)
{
    sceVu0FMATRIX rot;

    sceVu0UnitMatrix(rot);

    rot[1][1] = SgCosf(rx);
    rot[2][1] = -SgSinf(rx);

    rot[1][2] = SgSinf(rx);
    rot[2][2] = SgCosf(rx);

    sceVu0MulMatrix(m0, m1, rot);
}

void LocalRotMatrixY(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float ry)
{
    sceVu0FMATRIX rot;

    sceVu0UnitMatrix(rot);

    rot[0][0] = SgCosf(ry);
    rot[0][2] = SgSinf(ry);

    rot[2][0] = -SgSinf(ry);
    rot[2][2] = SgCosf(ry);

    sceVu0MulMatrix(m0,m1,rot);
}

void LocalRotMatrixZ(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rz)
{
    sceVu0FMATRIX rot;

    sceVu0UnitMatrix(rot);

    rot[0][0] = SgCosf(rz);
    rot[1][0] = -SgSinf(rz);

    rot[0][1] = SgSinf(rz);
    rot[1][1] = SgCosf(rz);

    sceVu0MulMatrix(m0, m1, rot);
}

void motInversKinematics(SgCOORDUNIT *cp, sceVu0FVECTOR target, u_int *top_addr, u_char bone_id)
{
    u_int parent_id;
    sceVu0FVECTOR end_eff;
    sceVu0FVECTOR root;
    sceVu0FVECTOR end_root;
    sceVu0FVECTOR target_root;
    float inner;
    sceVu0FMATRIX m;
    float r;
    float dist;
    sceVu0FVECTOR raxis;

    sceVu0CopyVector(end_eff, cp[bone_id].lwmtx[3]);

    parent_id = bone_id;

    do
    {
        parent_id = motGetParentId(top_addr, parent_id - 1);

        sceVu0CopyVector(root, cp[parent_id].lwmtx[3]);

        sceVu0SubVector(end_root, end_eff, root);
        sceVu0SubVector(target_root, target, root);
        sceVu0Normalize(end_root, end_root);
        sceVu0Normalize(target_root, target_root);

        inner = sceVu0InnerProduct(end_root, target_root);

        if (!(inner < 0.99999f))
        {
            return;
        }

        r = SgACosf(inner);

        if (PI_HALF < r)
        {
            r = PI_HALF;
        }

        if (plyr_wrk.spot_rot[0] < 0.0f)
        {
            r *= -0.25f;
        }

        sceVu0OuterProduct(raxis, end_root, target_root);
        LocalRotMatrixZ(cp[parent_id].matrix, cp[parent_id].matrix, r);
        sceVu0Normalize(raxis, raxis);
        GetMatrixRotateAxis(m, raxis, r);
        sceVu0SubVector(end_root, end_eff, root);

        dist = SgSqrtf(sceVu0InnerProduct(end_root, end_root));

        sceVu0Normalize(end_root, end_root);
        sceVu0ApplyMatrix(end_root, m, end_root);
        sceVu0Normalize(end_root, end_root);
        sceVu0ScaleVector(end_root, end_root, dist);
        sceVu0AddVector(end_eff, end_root, root);


    } while (parent_id != 1);
}

static u_int movGetFrameNum(u_int *mov_p)
{
    return *mov_p;
}

static float movGetMaxval(u_int *mov_p)
{
    return *(float*)(mov_p + 1);
}

void movGetMoveval(ANI_CTRL *ani_ctrl, u_int frame_num)
{
    float dist;
    float height;
    u_int *mov_p;
    u_int playnum;
    float *p1;

    mov_p = ani_ctrl->mdat;

    if (ani_ctrl->interp_flg == 1)
    {
        plyr_wrk.spd = GetPlyrSpd();
        plyr_wrk.spd_ud = 0.0f;
    }
    else
    {
        if (mov_p == NULL)
        {
            return;
        }

        movGetMaxval(mov_p);

        if (movGetFrameNum(mov_p) <= frame_num)
        {
            frame_num = movGetFrameNum(mov_p) - 1;
        }

        p1 = &((float *)mov_p)[2];
        p1 = &p1[frame_num * 2];

        dist = p1[0];
        height = p1[1];

        plyr_wrk.spd = dist * 25.0f * 0.5f;
        plyr_wrk.spd_ud = -height * 25.0f * 0.5f;
    }

    playnum = ani_mdl[0].mot.play_id;

    switch(playnum)
    {
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
    case 70:
    case 71:
    case 72:
    case 73:
    case 82:
    case 83:
    case 84:
    case 85:
    case 97:
    case 98:
    case 99:
    case 100:
    case 101:
    case 102:
    case 110:
    case 112:
    case 113:
    case 115:
        plyr_wrk.mvsta |= 0x200000;
    break;
    case 111:
    case 114:
        plyr_wrk.spd = 0.0f;
        plyr_wrk.mvsta |= 0x200000;
    break;
    }

    if (ani_mdl[0].mot.reso == 0)
    {
        plyr_wrk.spd = 0.0f;
        plyr_wrk.spd_ud = 0.0f;
    }
}

static u_int motGetBoneNum(u_int *mot_p)
{
    MOT_FILE_HEADER *mfh;

    mfh = (MOT_FILE_HEADER *)mot_p;

    return mfh->bone_num;
}

static u_int motGetTransBoneNum(u_int *mot_p)
{
    MOT_FILE_HEADER *mfh;

    mfh = (MOT_FILE_HEADER *)mot_p;

    return mfh->trans_num;
}

static u_int motGetFrameNum(u_int *mot_p)
{
    MOT_FILE_HEADER *mfh;

    mfh = (MOT_FILE_HEADER *)mot_p;

    return mfh->frame_num;
}

static u_int motGetSIFrameNum(u_int *mot_p)
{
    MOT_FILE_HEADER *mfh;

    mfh = (MOT_FILE_HEADER *)mot_p;

    return mfh->si_frame;
}

static u_int motGetInterpFrameNum(u_int *mot_p)
{
    MOT_FILE_HEADER *mfh;

    mfh = (MOT_FILE_HEADER *)mot_p;

    return mfh->interp_frame;
}

static u_int* motGetRstBuffer(u_int *mot_p)
{
    return (u_int*)(mot_p + 40);
}

static u_int motCheckRstFull(u_int *mot_p)
{
    MOT_FILE_HEADER *mfh;

    mfh = (MOT_FILE_HEADER *)mot_p;

    return mfh->flg & 0x1;
}

static u_int motCheckTransCut(u_int *mot_p)
{
    MOT_FILE_HEADER *mfh;

    mfh = (MOT_FILE_HEADER *)mot_p;

    return (mfh->flg >> 1) & 0x1;
}

static u_int motCheckIncludeRstPacket(u_int *mot_p)
{
    MOT_FILE_HEADER *mfh;

    mfh = (MOT_FILE_HEADER *)mot_p;

    return (mfh->flg >> 2) & 1;
}

static void motAddressMapping(u_int *top_addr)
{
    MOT_ADDR_TABLE *tbl_p;
    u_int i;
    u_int frame_num;

    frame_num = motGetFrameNum(top_addr);

    if (motCheckIncludeRstPacket(top_addr) != 0)
    {
        tbl_p = (MOT_ADDR_TABLE *)(top_addr + 40 + motGetBoneNum(top_addr) * 24);
    }
    else
    {
        tbl_p = (MOT_ADDR_TABLE *)(top_addr + 40);
    }

    if (*top_addr != 0x4e544f4d /*MOTN*/)
    {
        return;
    }

    if (top_addr[1] == 1)
    {
        return;
    }

    for (i = 0; i < frame_num; i++)
    {
        tbl_p->frame_addr = (u_int *)((int)top_addr + (int)tbl_p->frame_addr);
        tbl_p++;
    }

    top_addr[1] = 1;
}

static u_int* motGetFrameDataAddr(u_int *top_addr, u_int frame)
{
    u_int *addr;

    addr = top_addr + 0x28;

    if (motCheckIncludeRstPacket(top_addr) != 0)
    {
        addr = addr + motGetBoneNum(top_addr) * 0x18;
    }

    addr = &addr[frame];

    return (u_int *)*addr;
}

static void motGetFrameData(RST_DATA *rst, u_int *top_addr, u_int frame)
{
    if (frame == 0)
    {
        motGetFrameDataRST(rst, top_addr, 0);
    }
    else
    {
        motGetFrameDataRT(rst, top_addr, frame, 0);
    }
}

static void motGetFrameDataRST(RST_DATA *rst, u_int *top_addr, u_int frame)
{
    u_int i;
    u_int bone_num;
    float *pkt;

    if (motGetFrameNum(top_addr) <= frame)
    {
        frame = motGetFrameNum(top_addr) - 1;
    }

    pkt = (float *)motGetFrameDataAddr(top_addr, frame);

    bone_num = motGetBoneNum(top_addr);

    for (i = 0; i < bone_num; i++)
    {
            rst[i].rot[0] = *pkt++;
            rst[i].rot[1] = *pkt++;
            rst[i].rot[2] = *pkt++;

            rst[i].scale[0] = *pkt++;
            rst[i].scale[1] = *pkt++;
            rst[i].scale[2] = *pkt++;

            rst[i].trans[0] = *pkt++;
            rst[i].trans[1] = *pkt++;
            rst[i].trans[2] = *pkt++;
    }
}

static void motGetFrameDataRT(RST_DATA *rst, u_int *top_addr, u_int frame, u_int init_flg)
{
    u_int i;
    u_int bone_num;
    float *pkt;

    if (init_flg != 0)
    {
        motGetFrameDataRST(rst, top_addr, 0);

        if (frame == 0)
        {
            return;
        }
    }

    if (motGetFrameNum(top_addr) <= frame)
    {
        frame = motGetFrameNum(top_addr) - 1;

        if (frame == 0)
        {
            return;
        }
    }

    pkt = (float *)motGetFrameDataAddr(top_addr, frame);

    if (pkt == (float *)motGetFrameDataAddr(top_addr, 0))
    {
        return;
    }

    bone_num = motGetBoneNum(top_addr);

    for (i = 0; i < bone_num; i++)
    {
        rst[i].rot[0] = *pkt++;
        rst[i].rot[1] = *pkt++;
        rst[i].rot[2] = *pkt++;
    }

    for (i = 0; i < bone_num; i++)
    {
        if (motGetTransId(top_addr, i) != 0xff)
        {
            rst[i].trans[0] = *pkt++;
            rst[i].trans[1] = *pkt++;
            rst[i].trans[2] = *pkt++;
        }
    }
}

static u_int motGetTransId(u_int *top_addr, u_int id)
{
    top_addr = (u_int *)((int)top_addr + 0x21);

    return ((u_char *)top_addr)[id * 2];
}

static u_int motGetParentId(u_int *top_addr, u_int id)
{
    top_addr = (u_int *)((int)top_addr + 0x20);

    return ((u_char *)top_addr)[id * 2];
}

void motSetHierarchy(SgCOORDUNIT *coord, u_int *top_addr)
{
    u_int i;
    u_int bone_num;
    u_int parent_id;

    bone_num = motGetBoneNum(top_addr);

    coord[0].parent = NULL;

    for (i = 0; i < bone_num; i++)
    {
        parent_id = motGetParentId(top_addr, i);

        if (parent_id == 0xff)
        {
            coord[i+1].parent = NULL;
        }
        else
        {
            coord[i+1].parent = &coord[parent_id];
        }
    }
}
u_int* SceneInitAnime(ANI_CTRL *ani_ctrl, u_int *mdl_p, u_int *mot_p, u_int *mim_p, u_int *pkt_p, u_int mdl_no)
{
    MIME_DAT *mim_dat;
    u_int i;
    u_int *top_add;
    u_int *tmp_p;
    HeaderSection *hs; // not in STAB, added for readability

    motInitClearAniCtrl(ani_ctrl);

    if (mdl_no == 0 || (mdl_no == 1 && plyr_init_ctrl.msn_no == 0))
    {
        ani_ctrl->mpk_p = mdl_p;
    }
    else
    {
        ani_ctrl->mdl_p = mdl_p;
        ani_ctrl->mpk_p = (u_int *)GetFileInPak(mdl_p, 0);
        ani_ctrl->pk2_p = (u_int *)GetFileInPak(mdl_p, 1);
    }

    ani_ctrl->base_p = ani_ctrl->mpk_p + 8;

    if (mot_p != NULL)
    {
        motInitMotCtrl(&ani_ctrl->mot, mot_p, NULL);
        ani_ctrl->mot_num = *mot_p;
    }

    if (mim_p != NULL)
    {
        ani_ctrl->mim_num = *mim_p;
        ani_ctrl->mim = (MIME_CTRL *)motAlign128(pkt_p);
        ani_ctrl->wmim = (WMIM_CTRL *)motAlign128((u_int *)(ani_ctrl->mim + 20));
        mim_dat = (MIME_DAT *)motAlign128((u_int *)(ani_ctrl->wmim + 10));
        tmp_p = motAlign128((u_int *)(mim_dat + 20));

        pkt_p = mimInitMimeCtrl(ani_ctrl->mim, mim_dat, mim_p, ani_ctrl->mpk_p, tmp_p);

        for (i = 0; i < ani_ctrl->mim_num; i++)
        {
            ani_ctrl->mim[i].stat = 2;
        }

        mimInitAcsCtrl(ani_ctrl, (u_short)mdl_no);
    }

    if (manmdl_dat[mdl_no].cdat != NULL)
    {
        ani_ctrl->cloth_ctrl = (CLOTH_CTRL *)motAlign128(pkt_p);
        top_add = motAlign128((u_int *)(ani_ctrl->cloth_ctrl + 10));
        pkt_p = acsInitCloth(ani_ctrl->cloth_ctrl, ani_ctrl->mpk_p, top_add, mdl_no);
    }

    hs = (HeaderSection *)ani_ctrl->base_p;

    motSetHierarchy(hs->coordp, (ani_ctrl->mot).dat);

    return pkt_p;
}

u_int* SceneInitOtherAnime(ANI_CTRL *ani_ctrl, u_int *mdl_p, u_int *mot_p, u_int *mim_p, u_int *pkt_p)
{
    MIME_DAT *mim_dat;
    u_int i;
    u_int * tmp_p;
    HeaderSection *hs; // not in STAB, added for readability

    ani_ctrl->mot_num = 0;
    ani_ctrl->mim_num = 0;
    ani_ctrl->bg_num = 0;
    ani_ctrl->wmim_num = 0;
    ani_ctrl->mdl_p = NULL;
    ani_ctrl->mpk_p = NULL;
    ani_ctrl->pk2_p = NULL;
    ani_ctrl->base_p = mdl_p;
    ani_ctrl->ftype = 0;
    ani_ctrl->mim = NULL;
    ani_ctrl->wmim = NULL;
    ani_ctrl->bgmim = NULL;

    if (mot_p != NULL)
    {
        motInitMotCtrl(&ani_ctrl->mot, mot_p, NULL);
        ani_ctrl->mot_num = *mot_p;
    }

    if (mim_p != NULL)
    {
        tmp_p = (u_int *)GetPakTaleAddr(mim_p);
        ani_ctrl->mim_num = *mim_p;
        ani_ctrl->mim = (MIME_CTRL *)motAlign128(tmp_p);
        mim_dat = (MIME_DAT *)motAlign128((u_int *)(ani_ctrl->mim + 20));
        tmp_p = motAlign128((u_int *)(mim_dat + 20));

        pkt_p = mimInitMimeCtrl(ani_ctrl->mim, mim_dat, mim_p, ani_ctrl->mpk_p, tmp_p);

        for (i = 0; i < ani_ctrl->mim_num; i++)
        {
            ani_ctrl->mim[i].stat = 2;
        }
    }

    hs = (HeaderSection *)ani_ctrl->base_p;

    motSetHierarchy(hs->coordp, ani_ctrl->mot.dat);

    return pkt_p;
}

void motSetCoordFrame(ANI_CTRL *ani_ctrl, u_int frame)
{
    MOT_CTRL *m_ctrl;
    u_int data_no;

    m_ctrl = &ani_ctrl->mot;
    m_ctrl->inp_allcnt = (motGetInterpFrameNum(m_ctrl->dat) + 1) * 2;

    if (m_ctrl->inp_allcnt != 0)
    {
        data_no = frame / m_ctrl->inp_allcnt;
        m_ctrl->inp_cnt = frame % m_ctrl->inp_allcnt;
    }
    else
    {
        data_no = 0;
        m_ctrl->inp_cnt = 0;
    }

    motGetFrameDataRT(m_ctrl->rst0, m_ctrl->dat, data_no, 1);
    motGetFrameDataRT(m_ctrl->rst1, m_ctrl->dat, data_no + 1, 1);

    motSetInterpMatrix(ani_ctrl, m_start, m_end);
    motInterpAnm(ani_ctrl, m_start, m_end);
}

void SceneSetCoordFrame(ANI_CTRL *ani_ctrl, u_int frame, u_int type)
{
    u_int i;
    HeaderSection *hs;
    SgCOORDUNIT *cp;
    static RST_DATA rst[60];

    hs = (HeaderSection *)ani_ctrl->base_p;
    cp = hs->coordp;

    motGetFrameDataRT(rst, (ani_ctrl->mot).dat, frame, 1);

    if (type == 0)
    {
        sceVu0UnitMatrix(cp[0].matrix);

        cp[0].flg = 0;

        for (i = 1; i < hs->blocks - 1; i++)
        {
            sceVu0UnitMatrix(cp[i].matrix);

            cp[i].matrix[0][0] = rst[i-1].scale[0];
            cp[i].matrix[1][1] = rst[i-1].scale[1];
            cp[i].matrix[2][2] = rst[i-1].scale[2];

            sceRotMatrixXYZ(cp[i].matrix, cp[i].matrix, rst[i-1].rot);
            sceVu0TransMatrix(cp[i].matrix, cp[i].matrix, rst[i-1].trans);

            cp[i].flg = 0;
        }
    }
    else
    {
        sceVu0UnitMatrix(cp->matrix);

        cp->matrix[0][0] = rst[0].scale[0];
        cp->matrix[1][1] = rst[0].scale[1];
        cp->matrix[2][2] = rst[0].scale[2];

        sceRotMatrixXYZ(cp->matrix, cp->matrix, rst[0].rot);
        sceVu0TransMatrix(cp->matrix, cp->matrix, rst[0].trans);

        cp->flg = 0;
    }
}

void motSetInvMatrix(sceVu0FMATRIX m1, sceVu0FMATRIX m0)
{
    u_int i;
    sceVu0FMATRIX rot;
    sceVu0FMATRIX scale;
    sceVu0FMATRIX trans;
    sceVu0FVECTOR inv_inner;

    sceVu0UnitMatrix(rot);
    sceVu0UnitMatrix(scale);
    sceVu0UnitMatrix(trans);

    for (i = 0; i < 3; i++)
    {
        rot[i][0] = m0[0][i];
        rot[i][1] = m0[1][i];
        rot[i][2] = m0[2][i];
    }

    inv_inner[0] = 1.0f / SgSqrtf(sceVu0InnerProduct(rot[0], rot[0]));
    inv_inner[1] = 1.0f / SgSqrtf(sceVu0InnerProduct(rot[1], rot[1]));
    inv_inner[2] = 1.0f / SgSqrtf(sceVu0InnerProduct(rot[2], rot[2]));

    scale[0][0] = inv_inner[0];
    scale[1][1] = inv_inner[1];
    scale[2][2] = inv_inner[2];

    trans[3][0] = -m0[3][0];
    trans[3][1] = -m0[3][1];
    trans[3][2] = -m0[3][2];

    sceVu0ScaleVector(rot[0], rot[0], inv_inner[0]);
    sceVu0ScaleVector(rot[1], rot[1], inv_inner[1]);
    sceVu0ScaleVector(rot[2], rot[2], inv_inner[2]);

    _MulMatrix(m1, rot, scale);
    _MulMatrix(m1, m1, trans);
}

u_int* motAlign128(u_int *addr)
{
    u_int tmp;

    tmp = (u_int)addr;

    if (tmp & 0xf)
    {
        tmp += 16 - (tmp & 0xf);
    }

    return (u_int *)tmp;
}

void motPrintVector(char *str, sceVu0FVECTOR vec)
{
    return;
}

void sceRotMatrixXYZ(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FVECTOR rot)
{
    sceVu0FMATRIX mat;

    sceVu0CopyMatrix(mat, m1);
    sceVu0RotMatrixX(mat, mat, *rot);
    sceVu0RotMatrixY(mat, mat, rot[1]);
    sceVu0RotMatrixZ(mat, mat, rot[2]);
    sceVu0CopyMatrix(m0, mat);
}

MIME_CTRL mim_chodo[20] = {0};
u_char mim_chodo_se[20] = {0};
MIME_DAT mim_cdat[2][20] = {0};
ANI_CTRL ani_mdl[20] = {0};
ANI_MDL_CTRL ani_mdl_ctrl[20] = {0};
ROPE_CTRL rope_ctrl[20] = {0};
CMOVE_CTRL cmove_ctrl[10] = {0};
ENE_VRAM_CTRL ene_vram_ctrl[4] = {0};
ENE_VRAM_CTRL ene_vram_bak[4] = {0};
ENE_CHILD_CTRL ene_child_ctrl[4] = {0};
QUAKE_CTRL quake = {0};
PLYR_ACT_WRK plyr_act_wrk = {0};
