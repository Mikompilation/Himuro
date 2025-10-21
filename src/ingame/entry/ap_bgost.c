#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ap_bgost.h"

#include "main/glob.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"
#include "ingame/event/ev_load.h"
#include "ingame/enemy/ene_ctl.h"
#include "graphics/motion/mdlwork.h"
// #include "graphics/graph2d/effect_ene.h" // LoadEneDmgTex should be undeclared
// #include "graphics/motion/motion.h" // motInitEnemyAnm, motInitEnemyMdl should be undeclared

BGST_WRK bgst_wrk = {0};

void Mission03BindGhost01Delete()
{
    DelDataLoadWrk(M037_TOUSHU_MDL);
    DelDataLoadWrk(M900_ONNRYOU_MPK);
    DelDataLoadWrk(M037_TOUSHU_ANM);
    DelDataLoadWrk(SG037_TOUSHU_BD);
    DelDataLoadWrk(SGY001_BD);
}

void Mission04BindGhostLoad01Req()
{
    bgst_wrk.load_mode = 0;
    bgst_wrk.rel_mot = 17;
    bgst_wrk.bg_no = 6;
    bgst_wrk.mdl_adr = 0xd80000;
    bgst_wrk.mot_adr = 0xb90000;
    bgst_wrk.se_adr = 0x12;
}

void Mission04BindGhostLoad02Req()
{
    return;
}

int BindGhostBattleEnd(int wrk_no)
{
    if (ingame_wrk.msn_no == 4)
    {
        if (ene_wrk[wrk_no].dat_no == 4)
        {
            Mission04BindGhostLoad01Req();

            return 1;
        }
        else if (ene_wrk[wrk_no].dat_no == 6)
        {
            Mission04BindGhostLoad02Req();

            return 1;
        }
    }

    return 0;
}

int BindGhostLoad(void)
{
    switch(bgst_wrk.load_mode)
    {
    case BGLOAD_MODE_REQ:
        bgst_wrk.load_mode = BGLOAD_MODE_START;
    break;
    case BGLOAD_MODE_START:
        if (bgst_wrk.rel_mot != 0xff)
        {
            motReleaseAniMdlBuf(bgst_wrk.rel_mot, (u_int *)bgst_wrk.mot_adr);
        }

        LoadReq((jene_dat[ingame_wrk.msn_no][bgst_wrk.bg_no].mdl_no + M000_MIKU_MDL), bgst_wrk.mdl_adr);

        bgst_wrk.load_mode = BGLOAD_MODE_MDL;
    break;
    case BGLOAD_MODE_MDL:
        if (IsLoadEndAll() != 0)
        {
            motInitEnemyMdl((u_int *)bgst_wrk.mdl_adr, jene_dat[ingame_wrk.msn_no][bgst_wrk.bg_no].mdl_no);
            LoadEneDmgTex(jene_dat[ingame_wrk.msn_no][bgst_wrk.bg_no].mdl_no, (u_int *)(bgst_wrk.mot_adr + 0x98000));

            LoadReq((jene_dat[ingame_wrk.msn_no][bgst_wrk.bg_no].anm_no + M000_MIKU_ANM), bgst_wrk.mot_adr);

            bgst_wrk.load_mode = BGLOAD_MODE_MOT;
        }
    break;
    case BGLOAD_MODE_MOT:
        if (IsLoadEndAll() != 0)
        {
            motInitEnemyAnm((u_int *)bgst_wrk.mot_adr, jene_dat[ingame_wrk.msn_no][bgst_wrk.bg_no].mdl_no, jene_dat[ingame_wrk.msn_no][bgst_wrk.bg_no].anm_no);
            SeFileLoadAndSet((jene_dat[ingame_wrk.msn_no][bgst_wrk.bg_no].se_no), bgst_wrk.se_adr);

            bgst_wrk.load_mode = BGLOAD_MODE_SE;
        }
    break;
    case BGLOAD_MODE_SE:
        if (IsLoadEndAll() != 0)
        {
            bgst_wrk.load_mode = BGLOAD_MODE_END;
        }
    break;
    case BGLOAD_MODE_END:
        return 1;
    break;
    }

    return 0;
}
