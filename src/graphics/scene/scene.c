#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "scene.h"

#include "sce/libvu0.h"

#include "main/glob.h"
#include "common/ul_math.h"
#include "ingame/event/ev_main.h"
#include "ingame/plyr/unit_ctl.h"
#include "ingame/enemy/ene_ctl.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/adpcm/ea_scene.h"
#include "graphics/graph2d/effect.h"
#include "graphics/graph2d/effect_scr.h"
#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/effect_sub2.h"
#ifdef BUILD_EU_VERSION
#include "graphics/graph2d/subtitles.h"
#include "graphics/graph2d/tim2_new.h"
#endif
#include "graphics/graph3d/load3d.h"
#include "graphics/graph3d/sgsu.h"
#include "graphics/graph3d/gra3d.h"
#include "graphics/graph3d/sgcam.h"
#include "graphics/graph3d/mirror.h"
#include "graphics/graph3d/object.h"
#include "graphics/graph3d/sglight.h"
#include "graphics/graph3d/shadow.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/libsg.h"
#include "graphics/motion/mdldat.h"
#include "graphics/motion/motion.h"
#include "graphics/motion/mime.h"
#include "graphics/motion/accessory.h"
#include "graphics/motion/mdlwork.h"
#include "graphics/scene/scene_dat.h"
#include "graphics/scene/fod.h"

SCENE_CTRL scene_ctrl[2] = {0};
SCENE_FILE scene_file = {0};
#include "data/m000_bbox.h" // static sceVu0FVECTOR m000_bbox[];
#include "data/m030_bbox.h" // static sceVu0FVECTOR m030_bbox[];
#include "data/m060_bbox.h" // static sceVu0FVECTOR m060_bbox[];

static int scn_load_status = 0;
static int scn_load_num = 0;
int scn_vib_time0 = 0;
int scn_vib_time1 = 0;

static int scn_load_id[2];

#define MIM_BUF_BASE_ADDR 0x1310000

#define PI 3.1415927f

int SceneAllLoad(int scene_no, u_int *load_addr)
{
    int ret;

    ret = 0;

    switch (scn_load_status)
    {
    case 0:
        SceneDataLoadReq(scene_no, load_addr);
        scn_load_status = 1;
    break;
    case 1:
        if (SceneDataLoadWait() != 0)
        {
            scn_load_status = 2;
        }
    break;
    case 2:
        AdpcmScenePreLoadReq(scene_no);
        scn_load_status = 3;
    break;
    case 3:
        if (IsAdpcmScenePreLoadEnd() == 1 || IsAdpcmScenePreLoadEnd() == -1)
        {
            scn_load_status = 0;
            ret = 1;
        }
    break;
    }

    return ret;
}

u_int* SceneDataLoadReq(int scene_no, u_int *load_addr)
{
    SCENE_CTRL *sc;
    u_int *scn_addr;
    int scn_file_no;
    int i;
    int next_addr;

    scn_addr = load_addr;

    scn_file_no = scene_no * 2 + SCENE0010_MOVIE_SCN;

    for (i = 0; i < 2; i++)
    {
        if (scene_ctrl[i].init_flg == 1 && scene_ctrl[i].scene_no == scene_no)
        {
            return NULL;
        }
    }

    for (i = 0; i < 2; i++)
    {
        if (scene_ctrl[i].init_flg == 0)
        {
            scn_load_buf_id = i;

            break;
        }
    }

    if (i >= 2)
    {
        scn_load_buf_id = 0;
    }

    if (sys_wrk.game_mode == GAME_MODE_OUTGAME)
    {
        scn_load_buf_id = 0;
    }

    sc = &scene_ctrl[scn_load_buf_id];

    sc->scene_no = scene_no;
    sc->chapter_no = SceneGetChapterNo(scene_no);

    next_addr = LoadReqGetAddr(scn_file_no, (u_int)scn_addr, &scn_load_id[scn_load_num]);

    sc->scn_data_addr = scn_addr;
    sc->light_rev_addr = scn_addr = (u_int *)next_addr;

    scn_load_num++;

    next_addr = LoadReqGetAddr(scn_file_no + 1, (u_int)scn_addr, &scn_load_id[scn_load_num]);

    if (next_addr == NULL)
    {
        sc->light_rev_addr = NULL;
        next_addr = (int)scn_addr;
    }
    else
    {
        scn_load_num++;
    }

    scn_addr = (u_int *)next_addr;

    return scn_addr;
}

int SceneDataLoadWait()
{
    SCENE_CTRL *sc;
    int i;

    if (scn_load_num == 0) return; // missing return value??

    if (IsLoadEnd(scn_load_id[scn_load_num - 1]) == 0)
    {
        return 0;
    }

    sc = &scene_ctrl[scn_load_buf_id];

    for (i = 0; i < 4; i++)
    {
        memset(&sc->man_mdl[i], 0, sizeof(SCN_ANM_MDL));
        sc->man_mdl[i].disp_flg = 1;
    }

    for (i = 0; i < 14; i++)
    {
        memset(&sc->furn_mdl[i], 0, sizeof(SCN_ANM_MDL));
        sc->furn_mdl[i].disp_flg = 1;
    }

    for (i = 0; i < 8; i++)
    {
        memset(&sc->door_mdl[i], 0, sizeof(SCN_ANM_MDL));
        sc->door_mdl[i].disp_flg = 1;
    }

    for (i = 0; i < 8; i++)
    {
        memset(&sc->item_mdl[i], 0, sizeof(SCN_ANM_MDL));
        sc->item_mdl[i].disp_flg = 1;
    }

    SceneDataAnalyze(&scene_file, sc->scn_data_addr);
    SceneInitEnvironment(&scene_file, sc);

    mimClearToScene();

    if (sys_wrk.game_mode == GAME_MODE_INGAME)
    {
        EndAutoEne(1);
    }

    SetTextureToScene();

    if (sys_wrk.game_mode == GAME_MODE_INGAME)
    {
        SceneCheckModelEntry(sc);
        SceneAllMdlInit(sc);
    }

    sc->count_flg = 1;
    sc->init_flg = 1;
    scn_load_num = 0;

#ifdef BUILD_EU_VERSION
    InitSubtitlesSys();
#endif

    return 1;
}

void SceneDataAnalyze(SCENE_FILE *sf, u_int *scn_addr)
{
    sf->scn_file_addr = scn_addr;
    sf->file_num = scn_addr[0];
    sf->ofs_top_addr = &scn_addr[4];

    sf->hdr_addr = GetADRTBL(scn_addr, 0);
    sf->cam_fod_addr = GetADRTBL(scn_addr, 1);
    sf->lit_fod_addr = GetADRTBL(scn_addr, 2);
    sf->eff_fod_addr = GetADRTBL(scn_addr, 3);
    sf->man_mot_addr = GetADRTBL(scn_addr, 4);
    sf->man_mim_addr = GetADRTBL(scn_addr, 5);
    sf->furn_mot_addr = GetADRTBL(scn_addr, 6);
    sf->door_mot_addr = GetADRTBL(scn_addr, 7);
    sf->item_mot_addr = GetADRTBL(scn_addr, 8);
}

void SceneGetHeaderData(SCENE_CTRL *sc, u_int *hdr_top)
{
    u_short *hdr_num_data;
    char *hdr_pfx;

    hdr_num_data = (u_short *)&hdr_top[1];
    hdr_pfx = (char *)&hdr_top[4];

    sc->room_no = hdr_num_data[0];
    sc->mirror_flg = hdr_num_data[1];
    sc->man_mdl_num = hdr_num_data[2];
    sc->door_num = hdr_num_data[3];
    sc->furn_num = hdr_num_data[4];
    sc->item_num = hdr_num_data[5];

    hdr_pfx = GetHeaderMdlNo(sc->man_mdl, sc->man_mdl_num, hdr_pfx);
    hdr_pfx = GetHeaderMdlNo(sc->door_mdl, sc->door_num, hdr_pfx);
    hdr_pfx = GetHeaderMdlNo(sc->furn_mdl, sc->furn_num, hdr_pfx);

    GetHeaderMdlNo(sc->item_mdl, sc->item_num, hdr_pfx);
}

void SceneInitEnvironment(SCENE_FILE *sf, SCENE_CTRL *sc)
{
    SceneGetHeaderData(sc,sf->hdr_addr);

    sc->tmp_cam = camera;
    sc->cam = &camera;

    sc->fog.min = 0.0f;
    sc->fog.max = 255.0f;
    sc->fog.near = 1000.0f;
    sc->fog.far = 10000.0f;
    sc->fog.r = sc->fog.g = sc->fog.b = 0x10;

    FodInit(&sc->fod_ctrl,sf->cam_fod_addr,sf->lit_fod_addr,sf->eff_fod_addr);

    SceneLightRevision(sc);

    FodGetFirstCam(sc->cam,&sc->fod_ctrl);

    scn_vib_time0 = scn_vib_time1 = 0;
}

void SceneLightRevision(SCENE_CTRL *sc)
{
    sceVu0FVECTOR amb;
    FOD_LIGHT *fl;
    u_int *lit_addr;
    int i;
    int err_flg;

    lit_addr = (u_int *)((char *)lit_addr + sizeof(SgLIGHT)); // this forces proper instruction ordering ... why??
    lit_addr = sc->light_rev_addr;

    fl = &sc->fod_ctrl.fod_light;

    if (lit_addr == NULL)
    {
        return;
    }

    err_flg = fl->all_lit_num != lit_addr[0];

    lit_addr += 4;

    for (i = 0; i < fl->all_lit_num; i++)
    {
        if (strcmp((char *)&lit_addr[1], fl->lit_serial[i].light_name) != 0)
        {
            err_flg = 1;
            break;
        }

        lit_addr = (u_int *)((char *)lit_addr + sizeof(FOD_LIT_SERIAL) + sizeof(SgLIGHT));
    }

    if (err_flg == 0)
    {
        lit_addr = &sc->light_rev_addr[4];

        for (i = 0; i < fl->all_lit_num; i++)
        {
            memcpy(&fl->lit_serial[i], lit_addr, sizeof(FOD_LIT_SERIAL));
            lit_addr = (u_int *)((char *)lit_addr + sizeof(FOD_LIT_SERIAL));

            memcpy(&fl->all_lit[i], lit_addr, sizeof(SgLIGHT));
            lit_addr = (u_int *)((char *)lit_addr + sizeof(SgLIGHT));
        }

        sc->fog = *(SCENE_FOG *)lit_addr;
        lit_addr = (u_int *)((char *)lit_addr + sizeof(SCENE_FOG));

        for (i = 0; i < 6; i++)
        {
            Vu0CopyVector(amb, *(sceVu0FVECTOR *)lit_addr);

            if (strcmp((char *)&amb[3], "AMB") == 0)
            {
                amb[3] = 0.0f;
                Vu0CopyVector(fl->amb[i], amb);
            }
            else
            {
                fl->amb[i][0] = 0.0f;
                fl->amb[i][1] = 0.0f;
                fl->amb[i][2] = 0.0f;
                fl->amb[i][3] = 1.0f;
            }

            lit_addr = (u_int *)((char *)lit_addr + sizeof(sceVu0FVECTOR));
        }
    }
}

void SceneAllMdlInit(SCENE_CTRL *sc)
{
    int i;

    SceneSetManMdlTexOffset(sc);

    for (i = 0; i < sc->man_mdl_num; i++)
    {
        SceneInitManMdl(&sc->man_mdl[i], scene_file.man_mot_addr, scene_file.man_mim_addr, i);
    }

    for (i = 0; i < sc->furn_num; i++)
    {
        SceneInitOtherMdl(&sc->furn_mdl[i], scene_file.furn_mot_addr, i);
    }

    for (i = 0; i < sc->door_num; i++)
    {
        SceneInitOtherMdl(&sc->door_mdl[i], scene_file.door_mot_addr, i);
    }

    for (i = 0; i < sc->item_num; i++)
    {
        SceneInitOtherMdl(&sc->item_mdl[i], scene_file.item_mot_addr, i);
    }

    if (sys_wrk.game_mode == GAME_MODE_INGAME)
    {
        SceneSpecialManage();
    }
}

void SceneInitManMdl(SCN_ANM_MDL *sam, u_int *mot_addr, u_int *mim_addr, u_int mdl_id)
{
    SCENE_CTRL *sc;
    int i;
    char pfx[5];

    sc = &scene_ctrl[scn_now_play_id];

    sam->mot_addr = SceneGetMotAddr(mot_addr, mdl_id, pfx);
    sam->mim_addr = SceneGetMimAddr(mim_addr, pfx);

    sam->mim_buf_addr = (u_int *)(MIM_BUF_BASE_ADDR + mdl_id * 0x10000);

    if (sam->mdl_no == 1 && sc->chapter_no == 0)
    {
        sam->mdl_addr = pmanmodel[0];
    }
    else
    {
        sam->mdl_addr = pmanmodel[sam->mdl_no];
    }

    sam->ene_efct = SceneGetEneEfctAddr(sc, sam->mdl_no);

    for (i = 0; i < 4; i++)
    {
        sam->efct_addr[i] = NULL;
    }

    sam->scn_mdl_no = mdl_id;
    sam->mdl_alpha = 0x7f;

    SceneInitAnime(&sam->mdl_anm, sam->mdl_addr, sam->mot_addr, sam->mim_addr, sam->mim_buf_addr, sam->mdl_no);
    mimInitWeight(&sam->mdl_anm);

    if (sc->man_mdl_tex[mdl_id] != 0)
    {
        SetEneVram(sam->mdl_anm.mdl_p, sc->man_mdl_tex[mdl_id]);
    }
}

void SceneInitOtherMdl(SCN_ANM_MDL *sam, u_int *pk2_mot_addr, u_int mdl_id)
{
    SCENE_CTRL *sc;
    char pfx[5];

    sc = &scene_ctrl[scn_now_play_id];

    if (sam->disp_flg == 0)
    {
        return;
    }

    sam->mot_addr = SceneGetMotAddr(pk2_mot_addr, mdl_id, pfx);

    if (pfx[0] == 'f')
    {
        sam->mdl_addr = furn_addr_tbl[sam->mdl_no];

        if (sam->mdl_addr == NULL)
        {
            sc->furn_num--;

            return;
        }
    }
    else if (pfx[0] ==  'd')
    {
        sam->mdl_addr = door_addr_tbl[sam->mdl_no];

        if (sam->mdl_addr == NULL)
        {
            sc->door_num--;

            return;
        }
    }
    else if (pfx[0] ==  'i')
    {
        sam->mdl_addr = item_addr_tbl[sam->mdl_no];
    }

    sam->mim_addr = NULL;
    sam->scn_mdl_no = mdl_id;
    sam->mdl_alpha = 0x7f;

    SceneInitOtherAnime(&sam->mdl_anm, sam->mdl_addr, sam->mot_addr, NULL, NULL);
}

void SceneDraw(int scene_no)
{
    SCENE_CTRL *sc;
    FOD_CTRL *fc;
    int i;
    sceVu0FVECTOR *mpos;

    for (i = 0; i < 2; i++)
    {
        if (scene_ctrl[i].scene_no == scene_no)
        {
            sc = &scene_ctrl[i];
            scn_now_play_id = i;
            break;
        }
    }

    if (i >= 2)
    {
        sc = scene_ctrl;
        scn_now_play_id = 0;
    }

    fc = &sc->fod_ctrl;

    if (fc->now_frame == 1 && fc->now_reso == 0)
    {
        if (sys_wrk.game_mode == GAME_MODE_INGAME)
        {
            AdpcmScenePreLoadEndPlay(scene_no);
        }
    }

    realtime_scene_flg = 1;

    SceneRenewEnvironment(sc);
    objInit();
    SetEnvironment();

    SgSetFog(sc->fog.min, sc->fog.max, sc->fog.near, sc->fog.far, sc->fog.r, sc->fog.g, sc->fog.b);

    SceneLightClear(sc);

    if (sc->mirror_flg == 1)
    {
        if (((HeaderSection *)room_addr_tbl[sc->room_no].near_sgd)->kind & 2)
        {
            MirrorDraw(sc->cam, room_addr_tbl[sc->room_no].near_sgd, SceneMirrorMdlDraw);
        }
    }

    SceneDrawRoom(sc);

    for (i = 0; i < sc->door_num; i++)
    {
        SceneDrawOtherMdl(sc, &sc->door_mdl[i]);
    }

    for (i = 0; i < sc->furn_num; i++)
    {
        if (sc->furn_mdl[i].mdl_no != 140)
        {
            SceneDrawOtherMdl(sc, &sc->furn_mdl[i]);
        }
    }

    for (i = 0; i < sc->item_num; i++)
    {
        SceneDrawOtherMdl(sc, &sc->item_mdl[i]);
    }

    for (i = 0; i < sc->man_mdl_num; i++)
    {
        SceneDrawManMdl(sc, i);
    }

    for (i = 0; i < sc->furn_num; i++)
    {
        if (sc->furn_mdl[i].mdl_no == 140)
        {
            SceneDrawOtherMdl(sc, &sc->furn_mdl[i]);
        }
    }

    if (sc->scene_no == 24)
    {
        if (fc->now_frame == 1 && fc->now_reso == 0)
        {
            StopFallenEffect();

            mpos = SceneGetMdlWaistPos(&sc->man_mdl[0].mdl_anm, 0);
            CallFallenEffect(mpos[0], 2000, 160, 2);
        }

        FallenObjects();
    }

#ifdef BUILD_EU_VERSION
    SendFontTex();

    SetSubtitles(0,scene_no,sys_wrk.pal_disp_mode == 0 ? (fc->now_frame * 5) / 6 : fc->now_frame);

    MakeMovMes();

    if (sc->count_flg == 1)
    {
        if (sys_wrk.pal_disp_mode == 0)
        {
            fc->check_cnt++;

            if (fc->check_cnt >= 5)
            {
                fc->check_cnt = 0;
                FodSetFrame(fc, fc->now_frame + 1);
            }
            else
            {
                FodNextFrame(fc);
            }
        }
        else
        {
            FodNextFrame(fc);
        }
    }

    if (SceneIsEnd() != 0)
    {
        FinishSubtitlesSys();
    }
#else
    if (sc->count_flg == 1)
    {
        FodNextFrame(fc);
    }
#endif
}

void SceneRenewEnvironment(SCENE_CTRL *sc)
{
    int i;

    if (sc->fod_ctrl.now_reso == 0)
    {
        FodGetToSgLight(&sc->fod_ctrl);
        FodGetCamData(sc->cam, &sc->fod_ctrl);
    }

    for (i = 0; i < sc->man_mdl_num; i++)
    {
        if (sc->man_mdl_tex[i] != 0)
        {
            SetManmdlTm2(sc->man_mdl[i].mdl_anm.pk2_p, sc->man_mdl_tex[i], 1);
        }
    }

    FodSetEffect(&sc->fod_ctrl);
    SceneSetFadeMdlEffect(sc);

    if (sc->count_flg != 0)
    {
        SceneSetVibrate(sc->scene_no, sc->fod_ctrl.now_frame - 1);
    }

    SgSetRefCamera(sc->cam);
}

void SceneMirrorMdlDraw()
{
    SCENE_CTRL *sc;
    FOD_CTRL *fc;
    SCN_ANM_MDL *sam;
    HeaderSection *hs;
    SgCOORDUNIT *cp;
    int i;

    sc = &scene_ctrl[scn_now_play_id];
    fc = &sc->fod_ctrl;

    hs = (HeaderSection *)room_addr_tbl[sc->room_no].near_sgd;
    cp = hs->coordp;

    sceVu0UnitMatrix(cp->matrix);

    cp->matrix[0][0] = 25.0f;
    cp->matrix[1][1] = 25.0f;
    cp->matrix[2][2] = 25.0f;

    cp->matrix[3][0] = 0.0f;
    cp->matrix[3][1] = 0.0f;
    cp->matrix[3][2] = 0.0f;
    cp->matrix[3][3] = 1.0f;

    sceVu0RotMatrixX(cp->matrix, cp->matrix, PI);

    CalcCoordinate(cp, hs->blocks - 1);

    FodSetMyLight(&fc->fod_light, "room", sc->cam->zd);

    SgSortUnitKind(hs, -1);

    for (i = 0; i < sc->door_num; i++)
    {
        sam = &sc->door_mdl[i];
        hs = (HeaderSection *)sam->mdl_addr;

        if (hs != 0)
        {
            CalcCoordinate(hs->coordp, hs->blocks -1);
            FodSetMyLight(&fc->fod_light, sam->prefix, sc->cam->zd);
            SgSortUnitKind(sam->mdl_addr, -1);
        }
    }

    for (i = 0; i < sc->furn_num; i++)
    {
        sam = &sc->furn_mdl[i];
        hs = (HeaderSection *)sam->mdl_addr;

        if (hs != 0)
        {
            CalcCoordinate(hs->coordp, hs->blocks -1);
            FodSetMyLight(&fc->fod_light, sam->prefix, sc->cam->zd);
            SgSortUnitKind(sam->mdl_addr, -1);
        }
    }

    for (i = 0; i < sc->item_num; i++)
    {
        sam = &sc->item_mdl[i];
        hs = (HeaderSection *)sam->mdl_addr;

        if (hs != 0)
        {
            CalcCoordinate(hs->coordp, hs->blocks -1);
            FodSetMyLight(&fc->fod_light, sam->prefix, sc->cam->zd);
            SgSortUnitKind(sam->mdl_addr, -1);
        }
    }

    for (i = 0; i < sc->man_mdl_num; i++)
    {
        sam = &sc->man_mdl[i];

        if (sam->mdl_no == sc->hero_mdl_no)
        {
            FodSetMyLight(&fc->fod_light, sam->prefix, sc->cam->zd);
            SgSortUnitKind(sam->mdl_anm.base_p, -1);

            if (sam->mdl_no == 0)
            {
                DrawGirlSubObj(sam->mdl_anm.mpk_p, 0x7f);
            }
            else
            {
                DrawEneSubObj(sam->mdl_anm.mpk_p, 0x7f, 0x7f);
            }
        }
    }
}

int SceneIsEnd()
{
    SCENE_CTRL *sc;
    int i;
    int ret;
    int skip_ok;

    sc = &scene_ctrl[scn_now_play_id];

    ret = 0;
    skip_ok = 0;

    if (scene_bg_load_flg == 0)
    {
        skip_ok = sc->fod_ctrl.now_frame * 2 > 30;
    }

    if (skip_ok == 1)
    {
        if (pad[0].one & 0x800)
        {
            sc->fod_ctrl.end_flg = 1;
            AdpcmSceneStop();
        }
    }

    if (sc->fod_ctrl.end_flg != 0)
    {
        for (i = 0; i < sc->man_mdl_num; i++)
        {
            mimClearAllVertex(&sc->man_mdl[i].mdl_anm);
            mimInitWeight(&sc->man_mdl[i].mdl_anm);
        }

        for (i = 0; i < sc->man_mdl_num; i++)
        {
            if (sc->man_mdl_tex[i] != 0)
            {
                SetEneVram(sc->man_mdl[i].mdl_anm.mdl_p, -sc->man_mdl_tex[i]);
            }
        }

        SceneReleaseEffect(sc);

        if (sc->scene_no == 24)
        {
            StopFallenEffect();
        }

        if (eff_param.mono_flg != 0)
        {
            eff_param.mono_flg = 0;
            ChangeMonochrome(0);
        }

        SetTextureAfterScene();
        mimLNigiriReq(1, 0);

        *sc->cam = sc->tmp_cam;

        sc->init_flg = 0;
        realtime_scene_flg = 0;

        ret = 1;
    }

    return ret;
}

void SceneLightClear(SCENE_CTRL *sc)
{
    sceVu0FVECTOR zd = {0.0f, 0.0f, 0.0f, 0.0f};

    SgSetInfiniteLights(zd,sc->fod_ctrl.fod_light.all_lit, 0);
    SgSetPointLights(sc->fod_ctrl.fod_light.all_lit, 0);
    FodSetSpotLights(sc->fod_ctrl.fod_light.all_lit, 0);
}

void SceneScenePrerender()
{
    static SgLIGHT lights;
    static SgLIGHT ilights[3];
    static SgLIGHT plights[16];
    static SgLIGHT slights[16];
    sceVu0FVECTOR ambient;
    SCENE_CTRL *sc;
    SCN_ANM_MDL *sam;
    int i;
    int disp_room;
    u_int *tmpModelp;

    sc = &scene_ctrl[scn_now_play_id];

    disp_room = sc->room_no;

    if (disp_room == 0xff && room_addr_tbl[disp_room].near_sgd != 0)
    {
        return;
    }

    SgReadLights(room_addr_tbl[disp_room].near_sgd, room_addr_tbl[disp_room].lit_data, ambient, ilights, 3, plights, 0x10, slights, 0x10);
    SgSetAmbient(ambient);
    SgSetInfiniteLights(camera.zd, ilights, ilights[0].num);
    SgSetPointLights(plights, plights[0].num);
    SgSetSpotLights(slights, slights[0].num);

    tmpModelp = room_addr_tbl[disp_room].near_sgd;

    if (disp_room != 0xff && tmpModelp != NULL)
    {
        SgClearPreRender(tmpModelp, -1);
        SgPreRender(tmpModelp, -1);

        tmpModelp = room_addr_tbl[disp_room].ss_sgd;

        if (tmpModelp != NULL)
        {
            SgClearPreRender(tmpModelp, -1);
            SgPreRender(tmpModelp, -1);
        }
    }

    for (i = 0; i < sc->furn_num; i++)
    {
        sam = &sc->furn_mdl[i];
        tmpModelp = furn_addr_tbl[sam->mdl_no];

        if (tmpModelp != NULL)
        {
            SgClearPreRender(tmpModelp, -1);
            SgPreRender(tmpModelp, -1);
        }
    }

    for (i = 0; i < sc->door_num; i++)
    {
        sam = &sc->door_mdl[i];
        tmpModelp = door_addr_tbl[sam->mdl_no];

        if (tmpModelp != NULL)
        {
            SgClearPreRender(tmpModelp, -1);
            SgPreRender(tmpModelp, -1);
        }
    }
}

sceVu0FVECTOR* SceneGetMdlWaistPos(ANI_CTRL *ani_ctrl, u_short mdl_no)
{
    int waist_id;
    HeaderSection *hs;

    waist_id = manmdl_dat[mdl_no].waist_id;

    hs = (HeaderSection *)ani_ctrl->base_p;

    return &hs->coordp[waist_id].lwmtx[3];
}

void SceneSetEneEffect(SCN_ANM_MDL *sam)
{
    static sceVu0FVECTOR pos;
    static sceVu0FVECTOR pos2;
    static float spd;
    sceVu0FVECTOR tv = {0.0f, 0.0f, 0.0f, 0.0f};
    sceVu0FVECTOR tr;
    int i;

    for (i = 0; i < 4; i++)
    {
        if (sam->mdl_no == eff_param.mdl_efct_sw[i])
        {
            break;
        }
    }

    if (i >= 4)
    {
        for (i = 0; i < 4; i++)
        {
            if (sam->efct_addr[i] != NULL)
            {
                ResetEffects(sam->efct_addr[i]);
                sam->efct_addr[i] = NULL;
            }
        }

        return;
    }

    GetMdlWaistPos(pos, &sam->mdl_anm, sam->mdl_no);

    Vu0CopyVector(pos2, pos);

    pos[1] = pos[1] + sam->ene_efct->aura_pos_ajst;

    if ((sam->mdl_alpha & 0x80000000) == 0)
    {
        sam->mdl_alpha = sam->ene_efct->mdl_alpha;
    }

    if (sam->efct_addr[0] == NULL)
    {
        sam->efct_addr[0] = SetEffects(
            EF_ENEFIRE, 2, 1, &pos,
            0, &sam->ene_efct->aura_rgba, &sam->ene_efct->aura_size, 0xa0,
            &sam->ene_efct->aura_rate);
    }

    tv[2] = sam->ene_efct->pdf_dist;

    GetTrgtRot(pos2, camera.p, tr, 3);

    RotFvector(tr, tv);

    sceVu0AddVector(pos2, pos2, tv);

    if (sam->efct_addr[1] == NULL)
    {
        if (sam->ene_efct->pdf1.type != 0)
        {
            spd = GetRndSP(1, 2);
            sam->efct_addr[1] = SetEffects(
                EF_PDEFORM, 2, (u_char)sam->ene_efct->pdf1.type, (u_char)sam->ene_efct->pdf1.alpha,
                sam->ene_efct->pdf1.sclx, sam->ene_efct->pdf1.scly, &pos2, 0,
                0, 0, 0, &spd, &sam->ene_efct->pdf1.rate, &sam->ene_efct->pdf1.trate);
        }
    }

    if (sam->efct_addr[2] == NULL)
    {
        if (sam->ene_efct->pdf2.type != 0)
        {
            spd =  GetRndSP(1, 2);
            sam->efct_addr[2] = SetEffects(
                EF_PDEFORM, 2, (u_char)sam->ene_efct->pdf2.type, (u_char)sam->ene_efct->pdf2.alpha,
                sam->ene_efct->pdf2.sclx, sam->ene_efct->pdf2.scly, &pos2, 0,
                0, 0, 0, &spd, &sam->ene_efct->pdf2.rate, &sam->ene_efct->pdf2.trate);
        }
    }
}

void SceneReleaseEffect(SCENE_CTRL *sc)
{
    SCN_ANM_MDL *sam;
    int i;
    int j;

    for (i = 0; i < 4; i++)
    {
        sam = &sc->man_mdl[i];

        for (j = 0; j < 4; j++)
        {
            if (sam->efct_addr[j] != NULL)
            {
                ResetEffects(sam->efct_addr[j]);

                sam->efct_addr[j] = 0;
            }
        }
    }

    if (eff_param.pdf_p != NULL)
    {
        ResetEffects(eff_param.pdf_p);

        eff_param.pdf_p = NULL;
    }
}

SCN_ENE_EFCT* SceneGetEneEfctAddr(SCENE_CTRL *sc, u_int mdl_no)
{
    SCN_ENE_EFCT *ret_addr;
    u_int cnt;

    ret_addr = NULL;
    cnt = 0;

    while (1)
    {
        if (scn_ene_efct[cnt].scene_no == sc->scene_no)
        {
            if (scn_ene_efct[cnt].mdl_no == mdl_no)
            {
                ret_addr = &scn_ene_efct[cnt];
                break;
            }

            if (scn_ene_efct[cnt].mdl_no == 0xFFFF)
            {
                break;
            }
        }
        else if (scn_ene_efct[cnt].scene_no == 0xFFFF)
        {
            break;
        }

        cnt++;
    }

    return ret_addr;
}

void SceneSetFadeMdlEffect(SCENE_CTRL *sc)
{
    SCN_ANM_MDL *sam;
    FOD_EF_FADE_MDL *fefm;
    int i;
    int j;
    int mdl_num;

    for (i = 0; i < eff_param.fade_mdl_cnt; i++)
    {
        fefm = eff_param.fade_mdl[i];

        switch (fefm->prefix[0])
        {
        case 'm':
            mdl_num = sc->man_mdl_num;
            sam = sc->man_mdl;
        break;
        case 'f':
            mdl_num = sc->furn_num;
            sam = sc->furn_mdl;
        break;
        case 'd':
            mdl_num = sc->door_num;
            sam = sc->door_mdl;
        break;
        case 'i':
            mdl_num = sc->item_num;
            sam = sc->item_mdl;
        break;
        }

        for (j = 0; j < mdl_num; j++)
        {
            if (fefm->mdl_id == sam->mdl_no)
            {
                sam->mdl_alpha = fefm->alpha | 0x80000000;
            }

            sam++;
        }
    }
}

void SceneSetVibrate(int scene_no, int frame)
{
    SCN_VIB_DATA *vib_datp;
    int i;

    if (opt_wrk.pad_move != 0) return;

    vib_datp = scn_vib_tbl[scene_no];

    for(i = 0; vib_datp[i].start != 0xFFFF; i++)
    {
        if (vib_datp[i].start <= frame && frame < vib_datp[i].end)
        {
            if (vib_datp[i].type != 1)
            {
                if (vib_datp[i].start == frame) {
                    scn_vib_time0 = scn_vib_type[vib_datp[i].type].time0;
                    scn_vib_time1 = scn_vib_type[vib_datp[i].type].time1;
                }
                if (scn_vib_time1 != 0) {
                    scn_vib_time1--;
                    VibrateRequest2(0, scn_vib_type[vib_datp[i].type].val);
                } else {
                    if (scn_vib_time0 == 0) {
                        scn_vib_time0 = scn_vib_type[vib_datp[i].type].time0;
                        scn_vib_time1 = scn_vib_type[vib_datp[i].type].time1;
                    } else {
                        scn_vib_time0--;
                    }
                }
            }
            else
            {
                VibrateRequest1(0, 1);
            }
        }
    }
}

void SceneDrawManMdl(SCENE_CTRL *sc, u_int mdl_id)
{
    SgCOORDUNIT *cp;
    MANMDL_DAT *mdat;
    FOD_CTRL *fc;
    SCN_ANM_MDL *sam;
    u_int *base_p;
    float scale;
    HeaderSection *hs;

    fc = &sc->fod_ctrl;
    sam = &sc->man_mdl[mdl_id];
    base_p = sam->mdl_anm.base_p;
    hs = (HeaderSection *)base_p;

    SceneSetCoordFrame(&sam->mdl_anm, fc->now_frame - 1, 0);
    SceneMimSetVertex(&sam->mdl_anm, fc->now_frame - 1);

    cp = hs->coordp;

    sceVu0UnitMatrix(cp->matrix);

    mdat = &manmdl_dat[sam->mdl_no];

    scale = 25.0f / mdat->scale;
    cp->matrix[0][0] = cp->matrix[1][1] = cp->matrix[2][2] = scale;
    cp->matrix[3][3] = 1.0f;

    sceVu0RotMatrixX(cp->matrix, cp->matrix, PI);
    CalcCoordinate(cp, hs->blocks - 1);
    FodSetMyLight(&fc->fod_light, sam->prefix, sc->cam->zd);
    acsClothCtrl(&sam->mdl_anm, sam->mdl_anm.mpk_p, sam->mdl_no, 0);

    if (sam->ene_efct != NULL)
    {
        SceneSetEneEffect(sam);
    }

    sam->mdl_alpha &= 0x7fffffff;

    ManmdlSetAlpha(base_p, sam->mdl_alpha);

    if (sam->mdl_no != 30 && sam->mdl_no != 60)
    {
        SgSortUnitKind(base_p, -1);

        if (sc->man_mdl_tex[mdl_id] == 0)
        {
            DrawGirlSubObj(sam->mdl_anm.mpk_p, sam->mdl_alpha);
        }
        else
        {
            DrawEneSubObj(sam->mdl_anm.mpk_p, sam->mdl_alpha, sam->mdl_alpha);
        }
    }
    else
    {
        ManmdlSetAlpha(base_p, 0);
        SgSortUnitKind(base_p, -1);
        SceneDrawManShadow(sc, sam);
    }
}

void SceneDrawManShadow(SCENE_CTRL *sc, SCN_ANM_MDL *sam)
{
    ShadowHandle shandle;
    SCN_ANM_MDL *dsam;
    sceVu0FMATRIX mat;
    sceVu0FVECTOR shadow_bbox[8];
    sceVu0FVECTOR dir;
    sceVu0FVECTOR lp;
    sceVu0FVECTOR li;
    sceVu0FVECTOR *bbox_addr;
    u_int *ysearch_models[30] = {0};
    u_int i;
    u_int born_no;
    u_int search_num;
    SgCOORDUNIT *cp;
    u_int *shdw_addr;
    HeaderSection *hs;

    search_num = 0;

    FodGetDropSpotPos(&sc->fod_ctrl.fod_light, sam->prefix, lp, li);

    Get2PosRot(lp, li, dir, dir + 1);

    if (dir[0] > PI)
    {
        dir[0] -= PI * 2;
    }

    if (dir[1] > PI)
    {
        dir[1] -= PI * 2;
    }

    sceVu0UnitMatrix(mat);
    sceVu0RotMatrixX(mat, mat, dir[0]);
    sceVu0RotMatrixY(mat, mat, dir[1]);

    dir[0] = 0.0f;
    dir[1] = -1.0f;
    dir[2] = 1.0f;
    dir[3] = 1.0f;

    sceVu0ApplyMatrix(dir, mat, dir);
    sceVu0ScaleVector(dir, dir, 1.0f);
    _NormalizeVector(dir, dir);

    dir[1] = (0.0f < dir[1]) ? -dir[1] : dir[1];

    if (sam->mdl_no == 60)
    {
        dir[1] += -0.5f;
    }

    if (sam->mdl_no == 30)
    {
        shdw_addr = sam->mdl_anm.base_p;
        bbox_addr = m030_bbox;
        born_no = 19;
    }
    else if (sam->mdl_no == 60)
    {
        shdw_addr = sam->mdl_anm.base_p;
        bbox_addr = m060_bbox;
        born_no = 19;
    }
    else
    {
        shdw_addr = pgirlshadow;
        bbox_addr = m000_bbox;
        born_no = 21;
    }

    hs = (HeaderSection *)sam->mdl_anm.base_p;
    cp = hs->coordp;

    for (i = 0; i < 8; i++)
    {
        sceVu0UnitMatrix(mat);

        Vu0CopyVector(mat[3], cp[born_no].matrix[3]);
        mat[3][1] -= 40.0f;

        sceVu0ApplyMatrix(shadow_bbox[i], mat, bbox_addr[i]);
    }

    ysearch_models[search_num] = room_addr_tbl[sc->room_no].near_sgd;

    search_num++;

    for (i = 0; i < sc->door_num != 0; i++)
    {
        dsam = &sc->door_mdl[i];
        ysearch_models[search_num] = dsam->mdl_addr;
        search_num++;
    }

    for (i = 0; i < sc->furn_num; i++)
    {
        dsam = &sc->furn_mdl[i];
        ysearch_models[search_num] = dsam->mdl_addr;
        search_num++;
    }

    shandle.shadow_model = shdw_addr;
    shandle.smodel_num = -1;
    shandle.source_model = NULL;
    shandle.search_model = (void **)ysearch_models;
    shandle.search_num = search_num;
    shandle.bbox = shadow_bbox;

    Vu0CopyVector(shandle.direction, dir);

    shandle.color[0] = 0x80; shandle.color[1] = 0x80; shandle.color[2] = 0x80; shandle.color[3] = 0x40;
    shandle.camera = sc->cam;

    DrawShadow(&shandle, SetEnvironment);

    sc->cam = shandle.camera;

    SetEnvironment();
}

void SceneDrawOtherMdl(SCENE_CTRL *sc, SCN_ANM_MDL *sam)
{
    SgCOORDUNIT *cp;
    FOD_CTRL *fc;
    u_int *base_p;
    u_int frame;

    base_p = sam->mdl_addr;

    if (sam->disp_flg == 0)
    {
        return;
    }

    fc = &sc->fod_ctrl;

    frame = fc->now_frame - 1;

    SceneSetCoordFrame(&sam->mdl_anm, frame, 1);
    SceneMimSetVertex(&sam->mdl_anm, frame);

    if (base_p != NULL)
    {
        cp = ((HeaderSection *)base_p)->coordp;

        sceVu0MulMatrix(cp->matrix, fod_cmn_mtx, cp->matrix);
        CalcCoordinate(cp, base_p[5] - 1);

        if (strncmp(sam->prefix, "i001", 4) == 0)
        {
            FodSetMyLight(&fc->fod_light, "m000", sc->cam->zd);
        }
        else
        {
            FodSetMyLight(&fc->fod_light, sam->prefix, sc->cam->zd);
        }

        sam->mdl_alpha &= 0x7fffffff;

        ManmdlSetAlpha(sam->mdl_anm.base_p, sam->mdl_alpha);
        SgSortUnitKind(base_p, -1);
    }
}

void SceneDrawRoom(SCENE_CTRL *sc)
{
    HeaderSection *hs;
    SgCOORDUNIT *cp;
    u_int *room_mdl[2] = {
        room_addr_tbl[sc->room_no].near_sgd,
        room_addr_tbl[sc->room_no].ss_sgd
    };
    int i;

    for (i = 0; i < 2; i++)
    {
        hs = (HeaderSection *)room_mdl[i];

        if (hs != NULL)
        {
            cp = hs->coordp;

            sceVu0UnitMatrix(cp->matrix);

            cp->matrix[0][0] = 25.0f;
            cp->matrix[1][1] = 25.0f;
            cp->matrix[2][2] = 25.0f;

            cp->matrix[3][0] = 0.0f;
            cp->matrix[3][1] = 0.0f;
            cp->matrix[3][2] = 0.0f;
            cp->matrix[3][3] = 1.0f;

            sceVu0RotMatrixX(cp->matrix, cp->matrix, PI);

            CalcCoordinate(cp, hs->blocks -1);
            FodSetMyLight(&sc->fod_ctrl.fod_light, "room", sc->cam->zd);
            SgSortUnitKind(room_mdl[i], -1);

            cp->flg = 0;
        }
    }
}

void SceneSpecialManage()
{
    SCENE_CTRL *sc;
    SCN_ANM_MDL *sam;
    int i;
    int j;

    sc = &scene_ctrl[scn_now_play_id];

    if (sc->scene_no != 0x31 && sc->scene_no != 0x2f)
    {
        return;
    }

    for (i = 0; i < sc->furn_num; i++)
    {
        sam = &sc->furn_mdl[i];

        for (j = 0; j < 4; j++)
        {
            if (sam->mdl_no == j+0x29)
            {
                if (((ev_wrk.face_stts[0] >> (j+1)) & 0x1) == 0)
                {
                    sam->disp_flg = 0;
                }
            }

            if (sam->mdl_no == 0x2c)
            {
                sam->disp_flg = 1;
            }
        }
    }
}

int SceneDecisionMovie(int scene_no)
{
    int i;

    i = 0;

    while (1)
    {
        if (scene_movie_no[i] == scene_no)
        {
            return 1;
        }

        if (scene_movie_no[i] >= 99)
        {
            break;
        }

        i++;
    }

    return 0;
}

u_int* SceneGetMotAddr(u_int *pak_top, u_int no, char *pfx)
{
    u_char *pfx_top;

    pfx_top = (u_char *)GetADRTBL(pak_top, 0);
    pfx_top += no * 4;

    strncpy(pfx, (char *)pfx_top, 4);
    pfx[4] = '\0';

    pfx_top = (u_char *)GetADRTBL(pak_top, no + 1);

    return (u_int* )pfx_top;
}

u_int* SceneGetMimAddr(u_int *pak_top, char *pfx)
{
    int i;
    int no;
    char *pfx_top;

    pfx_top = (char *)GetADRTBL(pak_top,0);

    no = -1;

    for (i = 0, no = -1; i < 4; i++)
    {
        if (strncmp((char *)pfx_top, pfx, 4) == 0)
        {
            no = i;
            break;
        }

        pfx_top += 4;
    }

    if (no < 0)
    {
        return NULL;
    }

    pfx_top = (char *)GetADRTBL(pak_top, no + 1);

    return (u_int *)pfx_top;
}

char* GetHeaderMdlNo(SCN_ANM_MDL *sam, int num, char *hdr_pfx)
{
    int i;

    if (num <= 0)
    {
        return hdr_pfx;
    }

    for (i = 0; i < num; i++)
    {
        strncpy(sam->prefix, hdr_pfx, 4);
        sam->mdl_no = GetPrefixNo(sam->prefix);

        hdr_pfx += 4;
        sam++;
    }

    if (num % 4 > 0)
    {
        hdr_pfx += (4 - num % 4) * 4;
    }

    return hdr_pfx;
}

u_int GetPrefixNo(char *pfx)
{
    u_int mdl_no;
    char pfx_tmp[5];

    strncpy(pfx_tmp, pfx, 4);
    pfx_tmp[4] = '\n';

    mdl_no = atoi(&pfx_tmp[1]);

    if (mdl_no > 500)
    {
        mdl_no = mdl_no % 100;
    }

    return mdl_no;
}

u_int* GetADRTBL(u_int *top, u_int no)
{
    return (u_int *)((u_char *)top + *(&top[no + 2] + 2)); // needs the double sum to prevent incorrect addu operand order
}

void SceneSetManMdlTexOffset(SCENE_CTRL *sc)
{
    int vram_offset[2] = { 0x2D00, 0x3160 };
    int i;
    int ofs_cnt;
    u_int hero_mdl_no;

    ofs_cnt = 0;

    for (i = 0; i < 4; i++)
    {
        sc->man_mdl_tex[i] = 0;
    }

    hero_mdl_no = sc->chapter_no == 0;

    sc->hero_mdl_no = hero_mdl_no;

    for (i = 0; i < sc->man_mdl_num; i++)
    {
        if (sc->man_mdl[i].mdl_no == hero_mdl_no)
        {
            sc->man_mdl_tex[i] = 0;
        }
        else if (sc->man_mdl[i].mdl_no == 30)
        {
            sc->man_mdl_tex[i] = 0;
        }
        else if (sc->man_mdl[i].mdl_no == 60)
        {
            sc->man_mdl_tex[i] = 0;
        }
        else
        {
            if (ofs_cnt > 1)
            {
                break;
            }

            sc->man_mdl_tex[i] = vram_offset[ofs_cnt];
            ofs_cnt++;
        }
    }
}

int scn_load_buf_id = 0;
int scn_now_play_id = 0;

int SceneGetChapterNo(int scene_no)
{
    int chapter_no;

    if (scene_no <= 4)
    {
        chapter_no = 0;
    }
    else if (scene_no <= 34)
    {
        chapter_no = 1;
    }
    else if (scene_no <= 62)
    {
        chapter_no = 2;
    }
    else if (scene_no <= 74)
    {
        chapter_no = 3;
    }
    else if (scene_no <= 90)
    {
        chapter_no = 4;
    }
    else if (scene_no <= 92)
    {
        chapter_no = 5;
    }
    else if (scene_no <= 96)
    {
        chapter_no = -1;
    }
    else
    {
        chapter_no = 0;
    }

    return chapter_no;
}

void SceneSetSquare(int pri, float x, float y, float w, float h, u_char r, u_char g, u_char b, u_char a)
{
    x -= 320.0f;
    y -= 224.0f;

    SetSquare(pri, x, y, x + w, y, x, y + h, x + w, y + h, r, g, b, a);
}

void SceneCheckModelEntry(SCENE_CTRL *sc)
{
    SCN_ANM_MDL *sam;
    char prefix[5];
    int i;

    for (i = 0; i < sc->furn_num; i++)
    {
        sam = &sc->furn_mdl[i];

        if (furn_addr_tbl[sam->mdl_no] == NULL)
        {
            sam->disp_flg = 0;

            strncpy(prefix, sam->prefix, 4);
            prefix[4] = '\0';
        }
        else
        {
            sam->disp_flg = 1;
        }
    }

    for (i = 0; i < sc->door_num; i++)
    {
        sam = &sc->door_mdl[i];

        if (door_addr_tbl[sam->mdl_no] == NULL)
        {
            sam->disp_flg = 0;

            strncpy(prefix, sam->prefix, 4);
            prefix[4] = '\0';
        }
        else
        {
            sam->disp_flg = 1;
        }
    }

    for (i = 0; i < sc->item_num; i++)
    {
        sam = &sc->item_mdl[i];

        if (item_addr_tbl[sam->mdl_no] == NULL)
        {
            sam->disp_flg = 0;

            strncpy(prefix, sam->prefix, 4);
            prefix[4] = '\0';
        }
        else
        {
            sam->disp_flg = 1;
        }
    }
}

void InitSceneWork()
{
    int i;

    for (i = 0; i < 2; i++)
    {
        scene_ctrl[i].init_flg = 0;
    }

    scn_load_status = 0;
    scn_load_num = 0;
    scn_vib_time0 = 0;
    scn_vib_time1 = 0;
}
