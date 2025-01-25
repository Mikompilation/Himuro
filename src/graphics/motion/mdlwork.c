#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "mdlwork.h"

#include "sce/misc/diei.h"

#include "main/glob.h"
#include "main/gamemain.h"
#include "os/eeiop/eese.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "graphics/motion/motion.h"
#include "graphics/motion/mdlact.h"
#include "graphics/graph3d/gra3d.h"
#include "graphics/graph3d/sglight.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/sgsu.h"
#include "graphics/graph3d/sgsup.h"
#include "graphics/motion/accessory.h"
#include "graphics/graph2d/tim2_new.h"
#include "outgame/mode_slct.h"

#include "data/plyr_file_id.h" // static PLYR_FILE_ID plyr_file_id[];

void ManmdlSetAlpha(void *sgd_top, u_char alpha)
{
    SgMaterial *matp;
    u_int *phead;
    HeaderSection *hs = (HeaderSection *)sgd_top;

    matp = hs->matp;
    phead = hs->phead;

    while (matp < (SgMaterial *)phead)
    {
        matp->Diffuse[3] = alpha;
        matp++;
    }
}

char motCheckTrRateMdl(u_int mdl_no)
{
    if (mdl_no == 32 || mdl_no == 34 || mdl_no == 35 || mdl_no == 36)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

u_int* MpkMapUnit(u_int *mpk_p)
{
    mpk_p += 4;

    while (mpk_p[0] - 1 < 0x7fffffff)
    {
        HeaderSection *hs = (HeaderSection *)&mpk_p[4];

        SgMapUnit(hs);

        mpk_p += mpk_p[0] / 4 + 4;
    }

    return mpk_p;
}

void DrawGirlSubObj(u_int *mpk_p, u_char alpha)
{
    u_int i;
    u_int obj_num;
    sceVu0FVECTOR tmpvec;
    sceVu0FVECTOR vf2reg;
    HeaderSection *hs;
    SgCOORDUNIT *cp;
    char obj_id[2][2] = { {0x03, 0x06}, {0x01, 0x08} };

    obj_num = *mpk_p;

    cp = (SgCOORDUNIT*)mpk_p[10];

    GetVF2Register(vf2reg);

    tmpvec[0] = 0.0f;
    tmpvec[1] = 0.0f;
    tmpvec[2] = 0.0f;
    tmpvec[3] = 0.0f;

    for (i = 1; i < obj_num; i++)
    {
        hs = GetFileInPak(mpk_p,i);

        switch (i)
        {
        case 3:
        case 4:
        case 5:
        case 6:
            SetVF2Register(tmpvec);
        break;
        default:
            SetVF2Register(vf2reg);
        break;
        }

        ManmdlSetAlpha(hs, alpha);
        ManTexflush();

        SortUnitRefCoordKind(hs, cp, -1);
    }
}

void DrawEneSubObj(u_int *mpk_p, u_char alpha1, u_char alpha2)
{
    u_int i;
    HeaderSection *hs;
    SgCOORDUNIT *cp;
    u_int obj_num;
    u_char alpha;

    obj_num = mpk_p[0];
    cp = (SgCOORDUNIT* )mpk_p[10];

    for (i = 1; i < obj_num; i++)
    {
        alpha = i == 1 ? alpha2 : alpha1;

        hs = GetFileInPak(mpk_p, i);
        ManmdlSetAlpha(hs, alpha);
        ManTexflush();
        SortUnitRefCoordKind(hs, cp, -1);
    }
}

void SortUnitRefCoordKind(void *sgd_top, SgCOORDUNIT *coordp, int num)
{
    int old_di;
    HeaderSection *hs = (HeaderSection *)sgd_top;

    old_di = DIntr();

    if (hs->kind & 0x1)
    {
        SortUnitRefCoordP(sgd_top, coordp, num);
    }
    else
    {
        SortUnitRefCoord(sgd_top, coordp, num);
    }

    if (old_di != 0)
    {
        EIntr();
    }
}

void SortUnitRefCoordP(void *sgd_top, SgCOORDUNIT *coordp, int pnum)
{
    int i;
    u_int *pk;
    HeaderSection *hs;

    hs = (HeaderSection *)sgd_top;

    lcp = coordp;

    if (((u_int)coordp % 16)) // checking alignment? coordp->matrix is a sceVu0FMATRIX and should be 16 aligned!
    {
        printf("SgSortUnitP Data broken. %x\n", (u_int)sgd_top);
        return;
    }

    blocksm = hs->blocks;
    pk = (u_int *)&hs->primitives;

    sgd_top_addr = sgd_top;

    save_tri2_pointer = NULL;
    save_bw_pointer = NULL;

    ClearMaterialCache(hs);

    SetUpSortUnit();

    if (pnum < 0)
    {
        SgSortPreProcessP((u_int *)pk[0]);

        for (i = 1; i < blocksm; i++)
        {
            SgSortUnitPrimP((u_int *)pk[i]);
        }
    }
    else if (pnum == 0)
    {
        save_tri2_pointer = (u_int *)0xffffffff; // an "invalid" pointer value?
        save_bw_pointer = (u_int *)0xffffffff;   // probably a #define
        SgSortPreProcessP((u_int *)pk[0]);
    }
    else
    {
        SgSortUnitPrimP((u_int *)pk[pnum]);
    }
}

void SortUnitRefCoord(void *sgd_top, SgCOORDUNIT *coordp, int pnum)
{
    int i;
    u_int *pk;
    HeaderSection *hs;

    hs = (HeaderSection *)sgd_top;

    lphead = (PHEAD *)hs->phead;
    pk = (u_int *)&hs->primitives;
    blocksm = hs->blocks;

    sgd_top_addr = sgd_top;

    lcp = coordp;

    ClearMaterialCache(hs);

    SetUpSortUnit();

    LoadSgProg(0);

    if (pnum < 0)
    {
        SgSortPreProcess((u_int*)pk[0]);

        for (i = 1; i < blocksm - 1; i++)
        {
            SgSortUnitPrim((u_int*)pk[i]);
        }

        if ((u_int*)pk[i] != NULL)
        {
            SgSortUnitPrimPost((u_int*)pk[i]);
        }

        return;
    }

    if (pnum == 0)
    {
        SgSortPreProcess((u_int *)pk[0]);
        return;
    }

    if (pnum == blocksm - 1)
    {
        SgSortUnitPrimPost((u_int *)pk[pnum]);
        return;
    }

    SgSortUnitPrim((u_int *)pk[pnum]);
}

void InitEneVramCtrl()
{
    u_int i;

    for (i = 0; i < 4; i++)
    {
        InitEneVramCtrlSub(&ene_vram_ctrl[i]);
    }
}

void InitEneVramCtrlSub(ENE_VRAM_CTRL *ev_ctrl)
{
    ev_ctrl->mdl_p = NULL;
    ev_ctrl->flg = 0;
    ev_ctrl->mdl_no = 0xffff;
    ev_ctrl->offset = 0;
}

void SetEneTexture(u_int work_id)
{
    ANI_CTRL* ani;
    u_int *mdl_p;
    u_int offset;
    u_int i;
    u_int num;
    u_int mdl_no;

    mdl_no = ene_wrk[work_id].dat->mdl_no;
    ani = motGetAniMdl(work_id);
    mdl_p = ani->mdl_p;

    for (i = 0; i < 4; i++)
    {
        if (ene_vram_ctrl[i].mdl_no == mdl_no)
        {
            return;
        }

        asm volatile ("nop");
        asm volatile ("nop");
        asm volatile ("nop");
    }

    for (i = 0; i < 4; i++)
    {
        if (ene_vram_ctrl[i].flg == 0)
        {
            ene_vram_ctrl[i].mdl_no = mdl_no;
            ene_vram_ctrl[i].flg = 1;
            ene_vram_ctrl[i].mdl_p = mdl_p;

            switch (mdl_no)
            {
            case 42:
            case 43:
            case 44:

                if (i == 0)
                {
                    ene_vram_ctrl[i].offset = 0x2d00;
                }
                else if (i == 1)
                {
                    if (
                        ene_vram_ctrl[0].mdl_no == 42 ||
                        ene_vram_ctrl[0].mdl_no == 43 ||
                        ene_vram_ctrl[0].mdl_no == 44
                    )
                    {
                        ene_vram_ctrl[i].offset = 0x2fbc;
                    }
                    else
                    {
                        ene_vram_ctrl[i].offset = 0x3160;
                    }
                }
                else if (i == 2)
                {
                    ene_vram_ctrl[i].offset = 0x3278;
                }
            break;
            default:
                if (i == 0)
                {
                    ene_vram_ctrl[i].offset = 0x2d00;
                }
                else
                {
                    ene_vram_ctrl[i].offset = 0x3160;
                }
            break;
            }

            offset = ene_vram_ctrl[i].offset;
            break;
        }

        switch (mdl_no)
        {
        case 42:
        case 43:
        case 44:
            num = 3;
        break;
        default:
            num = 2;
        break;
        }

        if (i == num - 1)
        {
            motPrintVramCtrl();
            return;
        }
    }

    motPrintVramCtrl();
    SetEneVram(mdl_p, offset);
}

void SetEneVram(u_int *mdl_p, int offset)
{
    u_int *mpk_p;
    u_int *pk2_p;

    mpk_p = (u_int*)GetFileInPak(mdl_p, 0);
    pk2_p = (u_int*)GetFileInPak(mdl_p, 1);

    if (offset >= 0)
    {
        if (mpk_p[1] == 1)
        {
            return;
        }

        mpk_p[1] = 1;

        SetManmdlTm2(pk2_p, offset, 0);
    }
    else
    {
        if (mpk_p[1] == 0)
        {
            return;
        }

        mpk_p[1] = 0;
    }

    MpkAddTexOffset(mpk_p, offset);
}

void ReleaseEneTexture(u_int work_id)
{
    u_int *mdl_p;
    u_int mdl_no;
    u_int offset;
    u_int i;

    mdl_no = ene_wrk[work_id].dat->mdl_no;

    mdl_p = motGetAniMdl(work_id)->mdl_p;

    for (i = 0; i < 4; i++)
    {
        if (i != work_id && ene_wrk[i].sta & 0x1 && ene_wrk[i].dat->mdl_no == mdl_no)
        {
            return;
        }
    }

    for (i = 0; i < 4; i++)
    {
        if (ene_vram_ctrl[i].mdl_p == mdl_p)
        {
            offset = ene_vram_ctrl[i].offset;
            InitEneVramCtrlSub(&ene_vram_ctrl[i]);
            break;
        }

        if (i == 3)
        {
            motPrintVramCtrl();
            return;
        }

    }

    motPrintVramCtrl();
    SetEneVram(mdl_p, -offset);
}

void SendManMdlTex()
{
    u_int i;

    for (i = 0; i < 4; i++)
    {
        if (ene_vram_ctrl[i].flg != 0)
        {
            SetManmdlTm2((u_int*)GetFileInPak(ene_vram_ctrl[i].mdl_p, 1), ene_vram_ctrl[i].offset, 0);
        }
    }
}

void SetTextureToScene()
{
    u_int i;
    int offset;

    BackupEneVramCtrl();

    for (i = 0; i < 4; i++)
    {
        if (ene_vram_ctrl[i].flg == 0x1)
        {
            offset = ene_vram_ctrl[i].offset;

            SetEneVram(ene_vram_ctrl[i].mdl_p, -offset);
        }
    }

    InitEneVramCtrl();
}

void SetTextureAfterScene()
{
    u_int i;

    SetupEneVramCtrl();

    for (i = 0; i < 4; i++)
    {
        if (ene_vram_ctrl[i].flg != 0)
        {
            SetEneVram(ene_vram_ctrl[i].mdl_p, ene_vram_ctrl[i].offset);
        }
    }
}

void MpkAddTexOffset(u_int *mpk_p, int offset)
{
    HeaderSection *hs; // not from debugging symbols

    mpk_p += 4;

    while ((*mpk_p - 1) < ~0x80000000)
    {
        hs = (HeaderSection *)(mpk_p + 4);

        SgdAddTexOffset(hs, offset);

        mpk_p = (u_int*)(mpk_p + ((*mpk_p / 4) + 4));
    }
}

void SetManmdlTm2(u_int *pak_addr, int offset, char mode)
{
    u_int tm2_num;
    u_int i;
    u_int *tm2_addr;

    tm2_num = *pak_addr;

    for (i = 0; i < tm2_num; i++)
    {
        tm2_addr = GetFileInPak(pak_addr, i);

        if (mode == 0)
        {
            MakeTim2SendPacket_3Dpkt((u_int)tm2_addr, offset);
        }
        else
        {
            MakeTim2SendPacket((u_int)tm2_addr, offset);
        }
    }
}

void BackupEneVramCtrl()
{
    u_int i;

    for (i = 0; i < 4; i++)
    {
        ene_vram_bak[i] = ene_vram_ctrl[i];
    }
}

void SetupEneVramCtrl()
{
    u_int i;

    for (i = 0; i < 4; i++)
    {
        ene_vram_ctrl[i] = ene_vram_bak[i];
    }
}

void motPrintVramCtrl()
{
    u_int i;

    for (i = 0; i < 4; i++)
    {
        // debug code? optimized out? something that printed ene_vram_ctrl?
    }
}

void ManTexflush()
{
    return;
}

void SgdAddTexOffset(void *sgd_top, int offset)
{
    u_int i;
    u_int *p;
    u_int *nextprim;
    u_int *pk;
    HeaderSection* hs; // not from debugging symbols

    hs = sgd_top;

    pk = &hs->primitives;

    for (i = 0; i < hs->blocks; i++)
    {
        if (pk[i] != NULL)
        {
            p = (u_int *)pk[i];

            while (p[0] != NULL)
            {
                nextprim = p;
                p++;

                if (p[0] == 1 && p[11] == 6)
                {
                    p[9] += offset;
                    p[10] += offset * 32;
                }

                p = (u_int *)*nextprim;
            }
        }
    }
}

u_int* GetOkuraBuffer(u_char work_id)
{
    ANI_CTRL *ani_ctrl;

    ani_ctrl = motGetAniMdl(work_id);

    return &ani_ctrl->anm_p[19 * 8192];
}

void ReqMsnInitPlyr(u_char msn_no)
{
    plyr_init_ctrl.msn_no = msn_no;
    plyr_init_ctrl.step = 0;
}

char MsnInitPlyr()
{
    static char sync_flg;
    char pk2_id;

    if (plyr_init_ctrl.msn_no == 0)
    {
        plyr_mdl_no = 1;
        pk2_id = 4;
        mim_nigiri_l_id = 1;
        mim_mepati_id = 0;
        mim_nigiri_r_id = 255;
    }
    else
    {
        mim_mepati_id = 5;
        mim_nigiri_l_id = 6;
        mim_nigiri_r_id = 7;
        plyr_mdl_no = 0;
        pk2_id = cribo.costume;
    }

    switch (plyr_init_ctrl.step)
    {
    case 0:
        init_load_id = SeFileLoadAndSet(plyr_mdl_no + SV000_MIKU_BD, 2);
        plyr_init_ctrl.step = 1;
        break;
    case 1:
        if (IsLoadEnd(init_load_id) == 0)
        {
            break;
        }

        plyr_init_ctrl.step = 2;
    case 2:
        init_load_id = LoadReq(plyr_file_id[pk2_id].pk2, 0x9a0000);
        plyr_init_ctrl.step = 3;
        break;
    case 3:
        if (IsLoadEnd(init_load_id) == 0)
        {
            break;
        }

        SetManmdlTm2((u_int*)0x9a0000, 0, 1);
        plyr_init_ctrl.step = 4;
        sync_flg = 2;
        break;
    case 4:
        if (0 < sync_flg)
        {
            sync_flg--;
            break;
        }

        init_load_id = LoadReq(plyr_file_id[pk2_id].mpk, 0x9a0000);
        plyr_init_ctrl.step = 5;
        break;
    case 5:
        if (IsLoadEnd(init_load_id) != 0)
        {
            init_load_adr = PlayerModelInit();
            plyr_init_ctrl.step = 6;
        }

        break;
    case 6:
        init_load_id = LoadReq(plyr_file_id[pk2_id].acs, init_load_adr);
        plyr_init_ctrl.step = 7;
        break;
    case 7:
        if (IsLoadEnd(init_load_id) != 0)
        {
            init_load_adr = PlayerAccessoryInit(init_load_adr);
            plyr_init_ctrl.step = 8;
        }

        break;
    case 8:
        init_load_id = LoadReq(plyr_file_id[pk2_id].bwc, init_load_adr);
        plyr_init_ctrl.step = 9;
        break;
    case 9:
        if (IsLoadEnd(init_load_id) == 0)
        {
            break;
        }

        plyr_bwc_addr = (u_int*)init_load_adr;
        init_load_adr = (int)GetPakTaleAddr((void*)init_load_adr);
        plyr_init_ctrl.step = 10;
        break;
    case 10:
        init_load_id = LoadReq(plyr_file_id[pk2_id].clt, init_load_adr);
        plyr_init_ctrl.step = 11;
        break;
    case 11:
        if (IsLoadEnd(init_load_id) == 0)
        {
            break;
        }

        plyr_clut_addr = (u_int*)init_load_adr;
        init_load_adr = (int)GetPakTaleAddr(plyr_bwc_addr);
        plyr_init_ctrl.step = 12;
        break;
    case 12:
        plyr_init_ctrl.step = 13;
        init_load_id = LoadReq(plyr_file_id[pk2_id].anm, 0x870000);
        break;
    case 13:
        if (IsLoadEnd(init_load_id) == 0)
        {
            break;
        }

        motInitPlayerAnm(plyr_mdl_no);
        motInitMsn();
        return 1;
    }

    return 0;
}

void motInitMsn()
{
    motInitAniMdlBuf();
    motInitPacketCtrl();
    InitEneVramCtrl();
    motInitEneKuttuki();
    InitPlyrAcsAlpha();
    InitQuake();
    motInitPlyrAct();
}

void motInitAniMdlBuf()
{
    u_int i;

    for (i = 0; i < 20; i++)
    {
        motInitAniMdlBufSub(&ani_mdl_ctrl[i]);
    }
}

void motInitAniMdlBufSub(ANI_MDL_CTRL *am_ctrl)
{
    am_ctrl->anm_no = 0xffff;
    am_ctrl->anm_p = NULL;
    am_ctrl->map_flg = 0;
    am_ctrl->pkt_no = 0;
}

u_char motSetAniMdlBuf(u_int anm_no, u_int *anm_p)
{
    u_int i;
    u_int j;
    u_int pkt_no;

    pkt_no = 0;

    for (i = 1; i < 20; i++)
    {
        if (ani_mdl_ctrl[i].anm_no == 0xffff)
        {
            for (j = 1; j < 20; j++)
            {
                if (ani_mdl_ctrl[j].anm_p == anm_p && ani_mdl_ctrl[j].anm_no == anm_no)
                {
                    pkt_no++;

                    if (ani_mdl_ctrl[j].anm_p)
                    {
                        // anything that forces an access to `ani_mdl_ctrl` makes the function match
                    }
                }
            }

            ani_mdl_ctrl[i].map_flg = 0;
            ani_mdl_ctrl[i].anm_no = anm_no;
            ani_mdl_ctrl[i].anm_p = anm_p;
            ani_mdl_ctrl[i].pkt_no = pkt_no;

            return i;
        }
    }

    return 0xff;
}

void motReleaseOneAniMdlBuf(u_int anm_no, u_int *anm_p)
{
    u_int i;
    u_int j;

    for (i = 1; i < 20; i++)
    {
        if (ani_mdl_ctrl[i].anm_p == anm_p && ani_mdl_ctrl[i].anm_no == anm_no)
        {
            motInitAniMdlBufSub(&ani_mdl_ctrl[i]);

            for (j = 0; j < 4; j++)
            {
                if (ene_pkt_ctrl[j].buf_no == i)
                {
                    ene_pkt_ctrl[j].buf_no = 255;
                    ene_pkt_ctrl[j].pkt_no = 255;
                }
            }
        }
    }
}

void motReleaseAniMdlBuf(u_int anm_no, u_int *anm_p)
{
    motReleaseOneAniMdlBuf(anm_no, anm_p);
}

ANI_CTRL* motGetAniMdl(u_int work_no)
{
    u_int i;
    u_int anm_no;
    u_int pkt_no;
    ANI_CTRL *ani_ctrl;

    ani_ctrl = NULL;

    anm_no = ene_wrk[work_no].dat->anm_no;
    pkt_no = ene_pkt_ctrl[work_no].pkt_no;

    for (i = 1; i < 20; i++)
    {
        if (anm_no == ani_mdl_ctrl[i].anm_no && pkt_no == ani_mdl_ctrl[i].pkt_no)
        {
            ani_ctrl = &ani_mdl[i];
            break;
        }
    }

    if (ani_ctrl == NULL)
    {
        for (i = 0; i < 20; i++)
        {
            // empty
        }
    }

    return ani_ctrl;
}

void motInitPacketCtrl()
{
    u_int i;

    for (i = 0; i < 4; i++)
    {
        ene_pkt_ctrl[i].pkt_no = 0xff;
        ene_pkt_ctrl[i].buf_no = 0xff;
    }
}

void motSetAnmPacket(u_char no)
{
    u_int i;

    for (i = 0; i < 20; i++)
    {
        if (ani_mdl_ctrl[i].anm_no == ene_wrk[no].dat->anm_no && ani_mdl_ctrl[i].map_flg == 0)
        {
            ani_mdl_ctrl[i].map_flg = 0x1;

            ene_pkt_ctrl[no].pkt_no = ani_mdl_ctrl[i].pkt_no;
            ene_pkt_ctrl[no].buf_no = i;
            break;
        }
    }
}

void motReleaseAnmPacket(u_char no)
{
    u_int i; // not from the debugging symbols

    for (i = 0; i < 20; i++)
    {
        if ((ani_mdl_ctrl[i].anm_no == ene_wrk[no].dat->anm_no) && (ani_mdl_ctrl[i].pkt_no == ene_pkt_ctrl[no].pkt_no))
        {
            ani_mdl_ctrl[i].map_flg = 0;

            ene_pkt_ctrl[no].buf_no = 0xff;
            ene_pkt_ctrl[no].pkt_no = 0xff;
            break;
        }
    }
}

void motEneTexAnm(ANI_CTRL *ani_ctrl, u_int work_id)
{
    u_int *pk2_p;
    u_int i;
    u_int offset;
    static u_int cnt = 0;
    int num; // this does not get added to STAB

    if (sys_wrk.count % 2)
    {
        cnt++;
    }

    num = cnt % 7;

    if ((cnt / 7) % 2)
    {
        num = 7 - num;
    }

    pk2_p = (u_int*)GetFileInPak(ani_ctrl->tanm_p, num);

    for (i = 0; i < 4; i++)
    {
        if (ene_vram_ctrl[i].mdl_p == ani_ctrl->mdl_p)
        {
            offset = ene_vram_ctrl[i].offset;
        }

        if (i == 3)
        {
            return;
        }
    }

    SetManmdlTm2(pk2_p, offset, 0);
}

void motInitEneKuttuki()
{
    u_int i;

    for (i = 0; i < 4; i++)
    {
        ene_child_ctrl[i].flg = 0;
        ene_child_ctrl[i].bone_id = 0;
        ene_child_ctrl[i].r = 0;
        ene_child_ctrl[i].ry = 0;
    }
}

void motSetEnemyKuttuki(u_char work_id, u_char bone_id, float radius, float ry)
{
    short int bone_list[1] = {12};

    if (bone_id >= 1)
    {
        bone_id = 0;
    }

    if (work_id < 4)
    {
        ene_child_ctrl[work_id].r = radius;
        ene_child_ctrl[work_id].ry = ry;
        ene_child_ctrl[work_id].bone_id = bone_list[bone_id];
        ene_child_ctrl[work_id].flg = 1;
    }
}

void motReleaseEnemyKuttuki(u_char work_id)
{
    if (work_id < 4)
    {
        ene_child_ctrl[work_id].flg = 0;
    }
}

u_int *plyr_clut_addr = NULL;
u_int *plyr_bwc_addr = NULL;

void SetPlyrClut(int bw_flg)
{
    u_int *clut_addr;
    u_int i;
    u_int num;

    if (bw_flg == 0)
    {
        clut_addr = plyr_clut_addr;
    }
    else
    {
        clut_addr = plyr_bwc_addr;
    }

    num = *clut_addr;

    for (i = 0; i < num; i++)
    {
        MakeTim2SendPacket((u_int)GetFileInPak(clut_addr, i), 0);
    }
}
