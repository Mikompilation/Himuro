#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "eese.h"

#include "main/glob.h"
#include "common/ul_math.h"
#include "os/eeiop/eeiop.h"
#include "os/eeiop/se_cmd.h"
#include "os/eeiop/se_data.h"
#include "os/eeiop/se_trans.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "graphics/graph3d/sglib.h"

typedef struct {
    sceVu0FVECTOR pos;
    int se_no;
    int vpos;
    int ewrk_no;
    int fin_spd;
    u_short vol_max;
    u_short pitch;
    u_char mode;
    u_char room_id;
    u_char menu;
} SE_REQ_TMP_STR;

SE_CTRL se_ctrl = {0};
SE_MENU_FADE sm_fade = {0};

#define SE_WRK_SIZE 24
#define DEFAULT_VOL_RATE 0x1000
#define DEFAULT_PITCH 0x1000
#define DEFAULT_PAN 0x280

static SE_WRK se_wrk[SE_WRK_SIZE];
static SE_WRK se_wrk_bk[SE_WRK_SIZE];

static void SeInitSeWrkSub(int pos);
static void SeInitSeWrkP(SE_WRK *swp);
static void SeStartVp(SE_REQ_TMP_STR *tmp_str);
static int SeGetFreeVoice(int start_no, int get);
static int SeChkReqMode(float *pos, float *mb);

void InitSe(void)
{
    se_ctrl.btlhit_no = -1;
    se_ctrl.voice_no = -1;
    se_ctrl.door_no[0] = -1;
    se_ctrl.door_no[1] = -1;
    se_ctrl.door_no[2] = -1;
    se_ctrl.foot_no[0] = -1;
    se_ctrl.foot_no[1] = -1;
    se_ctrl.foot_no[2] = -1;
    se_ctrl.foot_no[3] = -1;
    se_ctrl.foot_no[4] = -1;
    se_ctrl.foot_no[5] = -1;
    se_ctrl.foot_no[6] = -1;
    se_ctrl.foot_no[7] = -1;
    se_ctrl.srund_no[0] = -1;
    se_ctrl.srund_no[1] = -1;
    se_ctrl.ghost_no[0] = -1;
    se_ctrl.ghost_no[1] = -1;
    se_ctrl.ghost_no[2] = -1;
    se_ctrl.event_no[0] = -1;
    se_ctrl.event_no[1] = -1;
    se_ctrl.wide_no = -1;
    se_ctrl.jidou_no[0] = -1;
    se_ctrl.jidou_no[1] = -1;
    se_ctrl.jidou_no[2] = -1;
    se_ctrl.jidou_no[3] = -1;
    se_ctrl.ghost_type[0] = 0;
    se_ctrl.ghost_type[1] = 0;
    se_ctrl.ghost_type[2] = 0;

    SeInitSeWrk();
    InitSeMenuFade();
}

void InitSeGhost(void)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        se_ctrl.ghost_no[i] = -1;
        se_ctrl.ghost_type[i] = 0;
    }
}

void InitSeMenuFade(void)
{
    sm_fade.mvol = 0xfff;
    sm_fade.tgt_vol = 0xfff;
    sm_fade.deg = 0x80;    
}

void UpdateSeMenuFade(void)
{
    if (sm_fade.mvol < sm_fade.tgt_vol) 
    {
        if (sm_fade.deg + sm_fade.mvol < sm_fade.tgt_vol) 
        {
            sm_fade.mvol += sm_fade.deg;
            return;
        }
        sm_fade.mvol = sm_fade.tgt_vol;
    }
    else if (sm_fade.mvol > sm_fade.tgt_vol)
    {
        if (sm_fade.tgt_vol > sm_fade.mvol - sm_fade.deg)
        {
            sm_fade.mvol = sm_fade.tgt_vol;
            return;
        }
        sm_fade.mvol -= sm_fade.deg;
    }
}

void SetTargetVolSeMenuFade(int tgt_vol)
{
    if (tgt_vol < 0) 
    {
        tgt_vol = 0;
    }
    else if (tgt_vol > 0xfff) 
    {
        tgt_vol = 0xfff;
    }

    sm_fade.tgt_vol = tgt_vol;
}

void SeInitSeWrk(void)
{
    int i;

    for (i = 0; i < SE_WRK_SIZE; i++)
    {
        SeInitSeWrkSub(i);
    }
}

static void SeInitSeWrkSub(int pos)
{
    SeInitSeWrkP(SeGetSeWrk(pos));
}

static void SeInitSeWrkP(SE_WRK *swp)
{
    if (swp) 
    {
        swp->mpos = 0x0;
        swp->spos[0] = swp->spos[1] = swp->spos[2] = 0.0f;
        
        swp->status = 0;
        swp->atr = 0;
        swp->se_p = 0;
        swp->v_p = 0;
        swp->pitch = DEFAULT_PITCH;
        swp->vol_rate = DEFAULT_VOL_RATE;
        swp->pan = DEFAULT_PAN;
        swp->vol_phase = 0;
        swp->menu = 0;
    }
}

void SetReverbVolume(u_short vol)
{
    SetIopCmdSm(IC_SE_EVOL, vol, 0, 0);
}

void SeStopAndBackup(void)
{
    int i;
    
    for (i = 0; i < SE_WRK_SIZE; i++)
    {
        SeInitSeWrkP(&se_wrk_bk[i]);

        if (se_wrk[i].status != 0) 
        {
            if (
                se_wrk[i].fade_mode != 2 &&
                (se_wrk[i].atr & 7) != 2 &&
                CheckSeUse(se_wrk[i].se_p) == 2
            )
            {
                se_wrk_bk[i] = se_wrk[i];
            }
            
            SeFadeFlame(se_wrk[i].v_p, 10, 0);
        }
    }
}

void SeBackupInit(void)
{
    int i;

    for (i = 0; i < SE_WRK_SIZE; i++)
    {
        SeInitSeWrkP(&se_wrk_bk[i]);
    }
}

void SeRevival(u_short fin_spd)
{
    SE_REQ_TMP_STR tmp;
    int i;

    for (i = 0; i < SE_WRK_SIZE; i++)
    {
        if (se_wrk_bk[i].status == SEW_STAT_FREE)
        {
            continue;
        }

        switch (se_wrk_bk[i].atr & 7)
        {
        case 0:
            tmp.pos[0] = 0.0;
            tmp.pos[1] = 0.0;
            tmp.pos[2] = 0.0;
            tmp.se_no = se_wrk_bk[i].se_p;
            tmp.vpos = se_wrk_bk[i].v_p;
            tmp.fin_spd = fin_spd;
            tmp.vol_max = se_wrk_bk[i].fade_tgt;
            tmp.mode = 1;
            tmp.pitch = se_wrk_bk[i].pitch;
            tmp.room_id = 0xff;

            SeStartVp(&tmp);
            
        break;
        case 3:
            tmp.pos[0] = se_wrk_bk[i].spos[0];
            tmp.pos[1] = se_wrk_bk[i].spos[1];
            tmp.pos[2] = se_wrk_bk[i].spos[2];
            tmp.se_no = se_wrk_bk[i].se_p;
            tmp.vpos = se_wrk_bk[i].v_p;
            tmp.fin_spd = fin_spd;
            tmp.vol_max = se_wrk_bk[i].fade_tgt;
            tmp.mode = 4;
            tmp.pitch = se_wrk_bk[i].pitch;
            tmp.room_id = se_wrk_bk[i].room_id;
        break;
        case 1:
            tmp.pos[0] = se_wrk_bk[i].spos[0];
            tmp.pos[1] = se_wrk_bk[i].spos[1];
            tmp.pos[2] = se_wrk_bk[i].spos[2];
            tmp.se_no = se_wrk_bk[i].se_p;
            tmp.vpos = se_wrk_bk[i].v_p;
            tmp.fin_spd = fin_spd;
            tmp.vol_max = se_wrk_bk[i].fade_tgt;
            tmp.mode = 2;
            tmp.pitch = se_wrk_bk[i].pitch;
            tmp.room_id = se_wrk_bk[i].room_id;
            SeStartVp(&tmp);
        break;
        case 4:
            tmp.pos[0] = 0.0;
            tmp.pos[1] = 0.0;
            tmp.pos[2] = 0.0;
            tmp.se_no = se_wrk_bk[i].se_p;
            tmp.vpos = se_wrk_bk[i].v_p;
            tmp.fin_spd = fin_spd;
            tmp.vol_max = se_wrk_bk[i].fade_tgt;
            tmp.mode = 5;
            tmp.pitch = se_wrk_bk[i].pitch;
            tmp.room_id = plyr_wrk.pr_info.room_no;
            SeStartVp(&tmp);
        break;
        }
    }
}

int SeStartFix(int se_no, u_short fin_spd, u_short vol_max, u_short pitch, u_char menu)
{
    SE_REQ_TMP_STR tmp;
    int vpos;

    if (se_no == 0xff)
    {
        return -1;
    }
    
    if (CheckSeUse(se_no) == 0) 
    {
        return -1;
    }
    
    vpos = SeGetFreeVoice(0, 1);
    
    if (vpos == -1)
    {
        return -1;
    }
    
    tmp.pos[0] = 0.0;
    tmp.pos[1] = 0.0;
    tmp.pos[2] = 0.0;
    tmp.se_no = se_no;
    tmp.fin_spd = fin_spd;
    tmp.vol_max = vol_max;
    tmp.mode = 1;
    tmp.pitch = pitch;
    tmp.room_id = 0xff;
    tmp.menu = menu;
    tmp.vpos = vpos;
    
    SeStartVp(&tmp);
    
    return vpos;
}

int SeStartFixFlame(int se_no, u_short flame, u_short vol_max, u_short pitch, u_char menu)
{
    u_short fin_spd;
    
    if (flame != 0) 
    {
        fin_spd = vol_max / flame;
        if (flame != 0) 
        {
            // probably was some assert here
        }
    }
    else 
    {
        fin_spd = 0;
    }
    
    return SeStartFix(se_no, fin_spd, vol_max, pitch,menu);
}

int SeStartFixV(int se_no, u_short fin_spd, u_short vol_max, u_short pitch, u_char voice_no)
{
    SE_REQ_TMP_STR tmp;
    int vpos;

    if (se_no == 0xff)
    {
        return -1;
    }
    
    if (CheckSeUse(se_no) == 0) 
    {
        return -1;
    }
    
    tmp.pos[0] = 0.0;
    tmp.pos[1] = 0.0;
    tmp.pos[2] = 0.0;
    tmp.se_no = se_no;
    tmp.vpos = voice_no;
    tmp.fin_spd = fin_spd;
    tmp.vol_max = vol_max;
    tmp.mode = 1;
    tmp.pitch = pitch;
    tmp.room_id = 0xff;
    tmp.menu = 0;
    
    SeStartVp(&tmp);
    
    return vpos;
}

int SeStartPos(int se_no, float *pos, u_short fin_spd, u_short vol_max, u_short pitch, u_char room_id)
{
    SE_REQ_TMP_STR tmp;
    int vpos;

    if (se_no == 0xff)
    {
        return -1;
    }
    
    if (CheckSeUse(se_no) == 0) 
    {
        return -1;
    }

    vpos = SeGetFreeVoice(0, 1);
    
    if (vpos == -1)
    {
        return -1;
    }
    
    tmp.pos[0] = pos[0];
    tmp.pos[1] = pos[1];
    tmp.pos[2] = pos[2];
    tmp.se_no = se_no;
    tmp.vpos = vpos;
    tmp.fin_spd = fin_spd;
    
    tmp.vol_max = vol_max;
    tmp.mode = 2;
    tmp.pitch = pitch;
    tmp.room_id = room_id;
    tmp.menu = 0;
    
    SeStartVp(&tmp);
    
    return vpos;
}

int SeStartPosFlame(int se_no, float *pos, u_short flame, u_short vol_max, u_short pitch, u_char room_id)
{
    u_short fin_spd;
    
    if (flame != 0) 
    {
        fin_spd = vol_max / flame;
    }
    else 
    {
        fin_spd = 0;
    }
    
    return SeStartPos(se_no, pos, fin_spd, vol_max, pitch, room_id);
}

int SeStartPut(int se_no, float *pos, u_short fin_spd, u_short vol_max, u_short pitch, u_char room_id)
{
    SE_REQ_TMP_STR tmp;
    int vpos;

    if (se_no == 0xff)
    {
        return -1;
    }
    
    if (CheckSeUse(se_no) == 0) 
    {
        return -1;
    }

    vpos = SeGetFreeVoice(0, 1);
    
    if (vpos == -1)
    {
        return -1;
    }
    
    tmp.pos[0] = pos[0];
    tmp.pos[1] = pos[1];
    tmp.pos[2] = pos[2];
    tmp.se_no = se_no;
    tmp.vpos = vpos;
    tmp.fin_spd = fin_spd;
    
    tmp.vol_max = vol_max;
    tmp.mode = 4;
    tmp.pitch = pitch;
    tmp.room_id = room_id;
    tmp.menu = 0;
    
    SeStartVp(&tmp);
    
    return vpos;
}

int SeStartPutFlame(int se_no,float *pos,u_short flame,u_short vol_max,u_short pitch,u_char room_id)
{
    u_short fin_spd;
    
    if (flame != 0) 
    {
        fin_spd = vol_max / flame;
    }
    else 
    {
        fin_spd = 0;
    }
    
    return SeStartPut(se_no, pos, fin_spd, vol_max, pitch, room_id);
}

int SeStartGhost(int se_no, int ewrk_no, u_short fin_spd, u_short vol_max, u_short pitch, u_char gno)
{
    SE_REQ_TMP_STR tmp;
    int vpos;
    ENE_WRK* ework;

    if (se_no == 0xff)
    {
        return -1;
    }

    if (CheckSeUse(gno * 0xb + se_no) == 0) 
    {
        return -1;
    }

    ework = &ene_wrk[ewrk_no];
    vpos = SeGetFreeVoice(0, 1);
    if (vpos == -1)
    {
        return -1;
    }
    
    tmp.pos[0] = 0.0;
    tmp.pos[1] = 0.0;
    tmp.pos[2] = 0.0;
    tmp.se_no = gno * 0xb + se_no;
    tmp.vpos = vpos;
    tmp.ewrk_no = ewrk_no;
    tmp.fin_spd = fin_spd;
    tmp.vol_max = vol_max;
    tmp.mode = 3;
    tmp.pitch = pitch;
    tmp.room_id = ework->room_no;
    tmp.menu = 0;
    
    SeStartVp(&tmp);
    
    return vpos;
}

int SeStartGhostFlame(int se_no, int ewrk_no, u_short flame, u_short vol_max, u_short pitch, u_char gno)
{
    u_short fin_spd;
    
    if (flame != 0) 
    {
        fin_spd = vol_max / flame;
    }
    else 
    {
        fin_spd = 0;
    }
    
    return SeStartGhost(se_no, ewrk_no, fin_spd, vol_max, pitch, gno);
}

int SeStartPlyr(int se_no, u_short fin_spd, u_short vol_max, u_short pitch)
{
    SE_REQ_TMP_STR tmp;
    int vpos;

    if (se_no == 0xff)
    {
        return -1;
    }
    
    if (CheckSeUse(se_no) == 0) 
    {
        return -1;
    }
    
    vpos = SeGetFreeVoice(0, 1);
    
    if (vpos == -1)
    {
        return -1;
    }
    
    tmp.pos[0] = 0.0;
    tmp.pos[1] = 0.0;
    tmp.pos[2] = 0.0;
    tmp.se_no = se_no;
    tmp.fin_spd = fin_spd;
    tmp.vol_max = vol_max;
    tmp.mode = 5;
    tmp.pitch = pitch;
    tmp.room_id = plyr_wrk.pr_info.room_no;
    tmp.vpos = vpos;
    tmp.menu = 0;
    SeStartVp(&tmp);
    
    return vpos;
}

int SeStartPlyrFlame(int se_no, u_short flame, u_short vol_max, u_short pitch)
{
    u_short fin_spd;
    
    if (flame != 0) 
    {
        fin_spd = vol_max / flame;
    }
    else 
    {
        fin_spd = 0;
    }
    
    return SeStartPlyr(se_no,fin_spd,vol_max,pitch);
}

static void SeStartVp(SE_REQ_TMP_STR *tmp_str)
{
    SE_WRK *swp;
    float rot_oc;
    
    swp = SeGetSeWrk(tmp_str->vpos);
    
    SeInitSeWrkSub(tmp_str->vpos);
    
    swp->se_p = *&tmp_str->se_no;
    swp->v_p = *&tmp_str->vpos;
    swp->fade_spd = tmp_str->fin_spd;
    swp->pitch = tmp_str->pitch;
    swp->room_id = tmp_str->room_id;
    swp->menu = tmp_str->menu;
    
    if (swp->fade_spd == 0)
    {
        swp->fade_mode = 0;
        swp->fade_vol = tmp_str->vol_max;
        swp->fade_tgt = tmp_str->vol_max;
    }
    else
    {
        swp->fade_mode = 1;
        swp->fade_vol = 0;
        swp->fade_tgt = tmp_str->vol_max;
    }
    
    swp->vol_rate = (swp->fade_vol * 0x1000) / 0xfff;
    
    switch(tmp_str->mode)
    {
    case 0:
        // ...
    break;
    case 1:
        swp->status = 4;
    break;
    case 4:
        swp->status = 4;
        swp->atr = 3;
        swp->spos[0] = tmp_str->pos[0];
        swp->spos[1] = tmp_str->pos[1];
        swp->spos[2] = tmp_str->pos[2];
        rot_oc = SeCmdGetAngle(
            SgAtan2f(swp->spos[0] - camera.p[0], swp->spos[2] - camera.p[2]),
            SgAtan2f(camera.i[0] - camera.p[0], camera.i[2] - camera.p[2]));
        swp->pan = SeCmdGetPan(rot_oc);
        swp->vol_rate = SeCmdGetVol(GetDist(GetDistV(swp->spos,camera.p), swp->spos[1] - camera.p[1]), swp->vol_rate);
        if (map_wrk.now_room != swp->room_id && swp->room_id != 0xff)
        {
            swp->vol_rate = swp->vol_rate >> 1;
        }
        swp->pitch = SeCmdGetPitch(rot_oc, swp->pitch);
    break;
    case 2:
        swp->atr = 1;
        swp->status = 4;
        swp->spos[0] = tmp_str->pos[0];
        swp->spos[1] = tmp_str->pos[1];
        swp->spos[2] = tmp_str->pos[2];
        rot_oc = SeCmdGetAngle(
            SgAtan2f(swp->spos[0] - camera.p[0], swp->spos[2] - camera.p[2]),
            SgAtan2f(camera.i[0] - camera.p[0], camera.i[2] - camera.p[2]));
        swp->pan = SeCmdGetPan(rot_oc);
        swp->vol_rate = SeCmdGetVol(GetDist(GetDistV(swp->spos,camera.p), swp->spos[1] - camera.p[1]), swp->vol_rate);
        if (map_wrk.now_room != swp->room_id && swp->room_id != 0xff)
        {
            swp->vol_rate = swp->vol_rate >> 1;
        }
        swp->pitch = SeCmdGetPitch(rot_oc, swp->pitch);
    break;
    case 3:
        swp->ene_wrk_no = tmp_str->ewrk_no;
        swp->mpos = &ene_wrk[swp->ene_wrk_no].move_box;
        swp->atr = 2;
        swp->status = 4;
        swp->spos[0] = ((MOVE_BOX *)swp->mpos)->pos[0];
        swp->spos[1] = ((MOVE_BOX *)swp->mpos)->pos[1];
        swp->spos[2] = ((MOVE_BOX *)swp->mpos)->pos[2];
        rot_oc = SeCmdGetAngle(
            SgAtan2f(swp->spos[0] - camera.p[0], swp->spos[2] - camera.p[2]),
            SgAtan2f(camera.i[0] - camera.p[0], camera.i[2] - camera.p[2]));
        swp->pan = SeCmdGetPan(rot_oc);
        swp->vol_rate = SeCmdGetVol(GetDist(GetDistV(swp->spos, camera.p), swp->spos[1] - camera.p[1]), swp->vol_rate);
        if (map_wrk.now_room != swp->room_id && swp->room_id != 0xff)
        {
            swp->vol_rate = swp->vol_rate >> 1;
        }
        swp->pitch = SeCmdGetPitch(rot_oc, swp->pitch);
    break;
    case 5:
        swp->mpos = &plyr_wrk.move_box;
        swp->atr = 4;
        swp->status = 4;
        swp->spos[0] = ((MOVE_BOX *)swp->mpos)->pos[0];
        swp->spos[1] = ((MOVE_BOX *)swp->mpos)->pos[1];
        swp->spos[2] = ((MOVE_BOX *)swp->mpos)->pos[2];
        rot_oc = SeCmdGetAngle(
            SgAtan2f(swp->spos[0] - camera.p[0], swp->spos[2] - camera.p[2]),
            SgAtan2f(camera.i[0] - camera.p[0], camera.i[2] - camera.p[2]));
        swp->pan = SeCmdGetPan(rot_oc);
        swp->vol_rate = SeCmdGetVol(GetDist(GetDistV(swp->spos, camera.p), swp->spos[1] - camera.p[1]), swp->vol_rate);
        if (map_wrk.now_room != swp->room_id && swp->room_id != 0xff)
        {
            swp->vol_rate = swp->vol_rate >> 1;
        }
        swp->pitch = SeCmdGetPitch(rot_oc, swp->pitch);
    break;
    }

    if (swp->menu != 0)
    {
        SetIopCmdLg(2, tmp_str->vpos, swp->se_p, swp->pitch, swp->pan, swp->vol_rate, 0, 0);
    }
    else
    {
        SetIopCmdLg(2, tmp_str->vpos, swp->se_p, swp->pitch, swp->pan, (swp->vol_rate * sm_fade.mvol) / 0xfff, 0, 0);
    }
}

int SeStartPosSrundFlame(u_char room_id, float *pos, u_short flame, u_short vol_max, u_short pitch)
{
    int i;
    int req_file_no;
    int req_se_no;
    
    req_se_no = -1;
    
    switch(room_id)
    {
    case 2:
        req_file_no = SS002_BD;
    break;
    case 3:
        req_file_no = SS003_BD;
    break;
    case 4:
        req_file_no = SS004_BD;
    break;
    case 7:
        req_file_no = SS007_BD;
    break;
    case 9:
        req_file_no = SS009_BD;
    break;
    case 11:
        req_file_no = SS011_BD;
    break;
    case 12:
        req_file_no = SS012_BD;
    break;
    case 14:
        req_file_no = SS014_BD;
    break;
    case 15:
        req_file_no = SS015_BD;
    break;
    case 16:
        req_file_no = SS016_BD;
    break;
    case 20:
        req_file_no = SS020_BD;
    break;
    case 21:
        req_file_no = SS021_BD;
    break;
    case 22:
        req_file_no = SS022_BD;
    break;
    case 30:
        req_file_no = SS030_BD;
    break;
    case 28:
    case 29:
    case 32:
    case 34:
        req_file_no = SS028_BD;
    break;
    case 10:
    case 40:
        req_file_no = SS010_BD;
    break;
    case 41:
        req_file_no = SS041_BD;
    break;
    case 0:
    default:
        req_file_no = SS000_BD;
    break;
    }
    
    for (i = 0; i < 2; i++)
    {
        if (se_ctrl.srund_no[i] == req_file_no)
        {
            req_se_no = i + 0x34;
            break;
        }
    }
    
    if (req_se_no != -1)
    {
        return SeStartPosFlame(req_se_no, pos, flame, vol_max, pitch, room_id);
    }
    
    return -1;
}

int SeStartPosEventFlame(u_char room_id, u_char req_pos, float *pos, u_short flame, u_short vol_max, u_short pitch)
{
    int i;
    int req_file_no;
    int req_se_no;
    
    req_se_no = -1;
    
    switch(room_id)
    {
    case 1:
        req_file_no = SE001_BD;
    break;
    case 2:
        req_file_no = SE002_BD;
    break;
    case 3:
        req_file_no = SE003_BD;
    break;
    case 5:
        req_file_no = SE005_BD;
    break;
    case 6:
        req_file_no = SE006_BD;
    break;
    case 7:
        req_file_no = SE007_BD;
    break;
    case 8:
        req_file_no = SE008_BD;
    break;
    case 9:
        req_file_no = SE009_BD;
    break;
    case 11:
        req_file_no = SE011_BD;
    break;
    case 12:
        req_file_no = SE012_BD;
    break;
    case 14:
        req_file_no = SE014_BD;
    break;
    case 15:
        req_file_no = SE015_BD;
    break;
    case 17:
        req_file_no = SE017_BD;
    break;
    case 18:
    case 21:
        req_file_no = SE018_BD;
    break;
    case 22:
        req_file_no = SE022_BD;
    break;
    case 23:
        req_file_no = SE023_BD;
    break;
    case 24:
        req_file_no = SE024_BD;
    break;
    case 25:
        req_file_no = SE025_BD;
    break;
    case 26:
        req_file_no = SE026_BD;
    break;
    case 16:
    case 27:
        req_file_no = SE027_BD;
    break;
    case 35:
        req_file_no = SE035_BD;
    break;
    case 38:
        req_file_no = SE038_BD;
    break;
    case 10:
    case 40:
        req_file_no = SE010_BD;
    break;
    case 0:
    default:
        req_file_no = SE000_BD;
    break;
    }
    
    for (i = 0; i < 2; i++)
    {
        if (se_ctrl.event_no[i] == req_file_no)
        {
            req_se_no = i * 3 + 0x57 + req_pos;
            break;
        }
    }

    if (req_se_no != -1)
    {
        return SeStartPosFlame(req_se_no, pos, flame, vol_max, pitch, room_id);
    }
    
    return -1;
}

static int SeGetFreeVoice(int start_no, int get)
{
    SE_VSTAT *svp;
    int i;
    
    svp = &GetIopStatP()->sev_stat[start_no];
    
    if (svp == NULL || start_no >= SE_WRK_SIZE)
    {
        return -1;
    }
    
    for (i = start_no; i < SE_WRK_SIZE; i++, svp++)
    {
        if (svp->status == 0 && SeGetSeWrk(i)->status != 4)
        {
            if (get != 0)
            {
                svp->status = 3;
            }
            
            return i;
        }
    }
    
    return -1;
}

int SeStopAll()
{
    SeInitSeWrk();
    SetIopCmdSm(8, 0, 0, 0);
    return 0;
}

int SeStop(int voice_num)
{
    if (voice_num >= 0)
    {
        SetIopCmdSm(4, voice_num, 0, 0);
    }
    
    return 0;
}

void SeFadeOut(int voice_num, int fout_spd, int tgt_vol)
{
    SE_WRK *swp;
    
    swp = SeGetSeWrk(voice_num);
    
    if (swp != NULL)
    {
        swp->fade_mode = 2;
        swp->fade_spd = fout_spd;
        swp->fade_tgt = tgt_vol;
    }
}

void SeFadeFlame(int voice_num, u_short flame, u_short tgt_vol)
{
    SE_WRK *swp;
    
    swp = SeGetSeWrk(voice_num);
    
    if (swp == NULL)
    {
        return;
    }
    
    if (flame == 0)
    {
        SeStop(voice_num);
        return;
    }
    
    if (tgt_vol < swp->fade_vol)
    {
        swp->fade_mode = 0x2;
        swp->fade_spd = (swp->fade_vol - tgt_vol) / flame;
        
        if (swp->fade_spd == 0)
        {
            swp->fade_spd = 1;
        }
        
    }
    else if (tgt_vol == swp->fade_vol)
    {
        swp->fade_mode = 0;
    }
    else
    {
        swp->fade_mode = 1;
        swp->fade_spd = (tgt_vol - swp->fade_vol) / flame;
        
        if (swp->fade_spd == 0)
        {
            swp->fade_spd = 1;
        }
    }
    
    swp->fade_tgt = tgt_vol;
}

void SeFadeFlameAll(u_short flame, u_short tgt_vol)
{
    SE_WRK *swp;
    int i;
    
    swp = &se_wrk[0];
    
    for (i = 0; i < SE_WRK_SIZE; i++, swp++)
    {
        if (swp->status != 0)
        {
            SeFadeFlame(swp->v_p, flame, tgt_vol);
        }
    }
}

void SeWrkUpdate()
{
    IOP_STAT *isp;
    SE_WRK *swp;
    int i;
    
    isp = GetIopStatP();
    swp = SeGetSeWrk(0);
    
    for (i = 0; i < SE_WRK_SIZE; i++, swp++)
    {
        if (isp->sev_stat[i].status == 0)
        {
            if (swp->status != 4)
            {
                swp->status = 0;
            }
        }
        else
        {
            if (swp->status == 4)
            {
                swp->status = 1;
            }
        }
    }
}

u_char IsEndSe(int v_no)
{
    if (v_no >= 0 && v_no < 0x18)
    {
        return se_wrk[v_no].status == 0;
    }
    
    return 1;
}

u_char IsEndSeGhost(int v_no, u_char se_no, u_char gno)
{
    if (gno >= 2)
    {
        return 1;
    }
    
    se_no += gno * 11;
    
    if (v_no >= 0 && v_no <= 0x17)
    {
        if (se_wrk[v_no].se_p != se_no)
        {
            return 1;
        }
        else
        {
            return se_wrk[v_no].status == 0;
        }
    }
    
    return 1;
}

void SeSetEfctVol(u_short vol)
{
    SetIopCmdSm(6, vol, 0, 0);
}

void SeSetMVol(u_short vol)
{
    int iop_vol;

    iop_vol = vol;
    
    if (vol > 0xfff)
    {
        iop_vol = 0xfff;
    }
    
    SetIopCmdSm(9, (iop_vol * 0x3fff) / 0xfff, 0, 0);
}

void SeSetSteMono(u_char mono)
{
    if (mono != 0)
    {
        SetIopCmdSm(10, 1, 0, 0);
    }
    else
    {
        SetIopCmdSm(10, 0, 0, 0);
    }
}

int SeFileLoadAndSet(u_int file_no, int type)
{
    int ret;

    ret = -1;
    
    if (type >= 0 && type < 26)
    {
        switch(type)
        {
        case 0:
            ret = LoadReqSe(SSYSTEM_BD, 0);
        break;
        case 1:
            se_ctrl.btlhit_no = file_no;
            ret = LoadReqSe(file_no, type);
        break;
        case 2:
            se_ctrl.voice_no = file_no;
            ret = LoadReqSe(file_no, type);
        break;
        case 3:
        case 4:
        case 5:
            se_ctrl.door_no[type - 3] = file_no;
            ret = LoadReqSe(file_no, type);
        break;
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
            se_ctrl.foot_no[type - 6] = file_no;
            ret = LoadReqSe(file_no, type);
        break;
        case 14:
        case 15:
            se_ctrl.srund_no[type - 14] = file_no;
            ret = LoadReqSe(file_no, type);
        break;
        case 16:
        case 17:
        case 18:
            se_ctrl.ghost_no[type - 16] = file_no;
            se_ctrl.ghost_type[type - 16] = 0;
            ret = LoadReqSe(file_no, type);
            FloatGhostSENotEmpty();
            return ret;
        case 19:
        case 20:
            se_ctrl.event_no[type - 19] = file_no;
            ret = LoadReqSe(file_no, type);
        break;
        case 21:
            se_ctrl.wide_no = file_no;
            ret = LoadReqSe(file_no, type);
        break;
        case 22:
        case 23:
        case 24:
        case 25:
            se_ctrl.jidou_no[type - 22] = file_no;
            ret = LoadReqSe(file_no, type);
        break;
        }
    } 
    
    return ret;
}

int SeFileLoadAndSetFGhost(u_int file_no, int type)
{
    int ret;
    
    if (type < 19)
    {
        if (type > 15)
        {
            se_ctrl.ghost_no[type - 16] = file_no;
            se_ctrl.ghost_type[type - 16] = 1;
            ret = LoadReqSe(file_no, type);
            FloatGhostSENotEmpty();
            return ret;
        }
    } 

    return SeFileLoadAndSet(file_no, type);
}

int SeFileLoadAndSetWide(u_short lbl_no)
{
    u_int file_no;

    switch(lbl_no)
    {
    case 2:
        file_no = ST000_ZOU_OKU_BD;
    break;
    case 1:
    case 4:
        file_no = ST006_DMDL_KAGI_BD;
    break;
    case 5:
        file_no = ST014_NINGYOU_BD;
    break;
    case 0:
    case 7:
        file_no = ST008_KAGAMI_BD;
    break;
    case 8:
        file_no = ST011_NAWA_KAKE_BD;
    break;
    case 3:
    case 10:
        file_no = ST007_DSML_KAGI_BD;
    break;
    case 11:
        file_no = ST004_DMDL_AKANAI_BD;
    break;
    case 12:
        file_no = ST005_FUSUMA_AKANAI_BD;
    break;
    default:
        file_no = ST001_MEN_KAKE_BD;
    break;
    }
    
    return SeFileLoadAndSet(file_no, 21);
}

char SeGetGhostPos(u_int se_file_no, u_char type)
{
    char i;
    
    for (i = 0; i < 3; i++)
    {
        if (
            se_ctrl.ghost_no[i] == se_file_no &&
            se_ctrl.ghost_no[i] != 0xffffffff &&
            se_ctrl.ghost_type[i] == type
        )
        {
            return i;
        }
    }
    
    return -1;
}

char SeGetAutoGhostPos(u_int se_file_no)
{
    char i;
    
    for (i = 0; i < 4; i++)
    {
        if (
            se_ctrl.jidou_no[i] == se_file_no &&
            se_ctrl.jidou_no[i] != 0xffffffff
        )
        {
            return i;
        }
    }
    
    return -1;
}

SE_WRK *SeGetSeWrk(int no)
{
    if (no >= 0 && no < SE_WRK_SIZE)
    {
        return &se_wrk[no];
    }
    
    return NULL;
}

int SeGetSvStat(int v_no)
{
    return GetIopStatP()->sev_stat[v_no].status;
}

int SeStartCmn(int se_no, float *pos, float *mb, int fin_spd, u_short vol_max)
{
    int vpos;
    
    if (se_no == 0xff)
    {
        return -1;
    }
    else
    {
        vpos = SeGetFreeVoice(0, 1);

        if (vpos == -1)
        {
            return -1;
        }
        else
        {
            SeStartVpCmn(se_no, vpos, pos, mb, fin_spd, vol_max);
            return vpos;
        }
    }
}

void SeStartVpCmn(int se_no, int vpos, float *pos, float *mb, int fin_spd, u_short vol_max)
{
    SE_WRK *swp;
    int mode;
    float rot_oc;

    mode = SeChkReqMode(pos,mb);
    swp = SeGetSeWrk(vpos);
    SeInitSeWrkSub(vpos);
    swp->se_p = se_no;
    swp->v_p = vpos;
    swp->fade_spd = fin_spd;
    
    if ((fin_spd & 0xffff) == 0)
    {
        swp->fade_mode = 0;
        swp->fade_vol = 0xfff;
    }
    else
    {
        swp->fade_mode = 1;
        swp->fade_vol = 0;
    }

    swp->vol_rate = (swp->fade_vol << 0xc) / 0xfff;
    
    switch(mode)
    {
    case 4:
        swp->atr = 3;
        swp->spos[0] = pos[0];
        swp->spos[1] = pos[1];
        swp->spos[2] = pos[2];
        rot_oc = SeCmdGetAngle(
            SgAtan2f(swp->spos[0] - camera.p[0], swp->spos[2] - camera.p[2]),
            SgAtan2f(camera.i[0] - camera.p[0], camera.i[2] - camera.p[2]));
        swp->pan = SeCmdGetPan(rot_oc);
        swp->pitch = SeCmdGetPitch(rot_oc, swp->pitch);
    break;
    case 2:
        swp->atr = 1;
        swp->status = 1;
        swp->spos[0] = pos[0];
        swp->spos[1] = pos[1];
        swp->spos[2] = pos[2];
        rot_oc = SeCmdGetAngle(
            SgAtan2f(swp->spos[0] - camera.p[0], swp->spos[2] - camera.p[2]),
            SgAtan2f(camera.i[0] - camera.p[0], camera.i[2] - camera.p[2]));
        swp->pan = SeCmdGetPan(rot_oc);
        swp->pitch = SeCmdGetPitch(rot_oc, swp->pitch);
    break;
    case 1:
    case 3:
    case 0:
        break;
    }
    
    SetIopCmdLg(2, vpos, swp->se_p, swp->pitch, swp->pan, swp->vol_rate, 0, 0);
}

static int SeChkReqMode(float *pos, float *mb)
{
    int mode;
    
    mode = 2;
    
    if (pos == NULL && mb == NULL)
    {
        mode = 1;
    }
    else if (pos != NULL && mb != NULL)
    {
        mode = 0;
    }
    
    return mode;
}

u_char SeGetFreeSv()
{
    return 0;
}

int SeStartV(int se_no, u_char voice_no)
{
    // no return with int return value in function signature ...
}
