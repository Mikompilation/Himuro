#include "common.h"
#include "typedefs.h"
#include "gra3d.h"

#include "sce/libvu0.h"
#include "sce/libdma.h"
#include "sce/sifdev.h"

#include "os/system.h"
#include "main/glob.h"
#include "ingame/ig_glob.h"
#include "ingame/map/door_ctl.h"
#include "ingame/map/furn_dat.h"
#include "graphics/scene/scene.h"
#include "graphics/motion/mime.h"
#include "graphics/motion/motion.h"
#include "graphics/motion/mdldat.h"
#include "graphics/motion/mdlact.h"
#include "graphics/motion/acs_dat.h"
#include "graphics/motion/mdlwork.h"
#include "graphics/motion/accessory.h"
#include "graphics/graph2d/tim2.h"
#include "graphics/graph2d/effect_ene.h"
#include "graphics/graph3d/sgsu.h"
#include "graphics/graph3d/sgcam.h"
#include "graphics/graph3d/libsg.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/shadow.h"
#include "graphics/graph2d/g2d_main.h"
// #include "graphics/graph3d/sgdma.h"
#include "graphics/graph3d/sglight.h"
#include "graphics/graph3d/mirror.h"
#include "graphics/graph3d/load3d.h"
#include "graphics/graph3d/sgsgd.h"
#include "graphics/graph3d/sgsup.h"
#include "graphics/graph3d/object.h"

#define SCR_WIDTH 640
#define SCR_HEIGHT 224

#define GS_X_COORD(x) ((2048 - (SCR_WIDTH / 2) + x) * 16)
#define GS_Y_COORD(y) ((2048 - (SCR_HEIGHT / 2) + y) * 16)

#define UNCACHED(ptr)          ((void*)((u_int)(ptr) | 0x20000000))
#define UNCACHED_ACCEL(ptr)    ((void*)((u_int)(ptr) | 0x30000000))

extern unsigned int dma __attribute__((section(".vutext")));

static u_int ene_display[4] = {0, 0, 0, 0};
u_int fly_display[3] = {0, 0, 0};
#include "data/fog_param.h"        // sceVu0FVECTOR fog_param[64];
#include "data/fog_rgb.h"          // sceVu0IVECTOR fog_rgb[64];
#include "data/fog_param_finder.h" // sceVu0FVECTOR fog_param_finder[64];
#include "data/fog_rgb_finder.h"   // sceVu0IVECTOR fog_rgb_finder[64];
u_char mimchodo_num[] = {6, 7, 8, 13, 14, 36};
u_int *pmanmodel[70] = {NULL};
u_int *pmanmpk[70] = {NULL};
u_int *pmanpk2[70] = {NULL};

static u_int *ptexture = 0;
static u_int *ctexture = 0;
static u_int *itexture = 0;
int disp_frame_counter = 0;
u_int *pgirlbase = 0;
u_int *pgirlshadow = 0;
u_int *pgirlacs[2] = {NULL, NULL};

static int mir_reflect_flg;
static int mir_room_workno;
static int disp3d_all;
static int disp3d_room;
static int disp3d_room_shadow;
static int disp3d_furn;
static int disp3d_girl;
static int disp3d_enemy;
static int disp3d_mirror;
static int disp3d_2ddraw;
static int room_no[2];

static SPOT_WRK mir_reflect_spot;
static SgSourceChainTag tag_buffer[2][6144];
static SgLIGHT lights[16];
static SgLIGHT slights[16];

#define PI_HALF 1.5707964f
#define PI 3.1415927f
#define PI_alt 3.1415925f
#define PI2 6.2831855f

u_int* LoadDataFromDVD(u_char *fname)
{
    int rfd;
    int len;
    u_int *buf;

    rfd = sceOpen((char *)fname, SCE_RDONLY);

    if (rfd < 0)
    {
        exit(1);
    }

    len = sceLseek(rfd, 0, SCE_SEEK_END);
    buf = (u_int *)malloc(len + 16);
    sceLseek(rfd, 0, SCE_SEEK_SET);
    sceRead(rfd, buf, len);
    sceClose(rfd);

    return buf;
}

u_int* LoadDataFromDVD2(u_char *fname, u_int *addr)
{
    int rfd;
    int len;

    rfd = sceOpen((char *)fname, SCE_RDONLY);

    if (rfd < 0)
    {
        exit(1);
    }

    len = sceLseek(rfd, 0, SCE_SEEK_END);
    sceLseek(rfd, 0, SCE_SEEK_SET);
    sceRead(rfd, addr, len);
    sceClose(rfd);

    return addr + (len + 16) / 4;
}

void LoadPackedTextureFromMemory(u_int *pointer)
{
    TIM2_PICTUREHEADER *ph;

    pointer += 4;

    while (*pointer - 1 < 0x7fffffff)
    {
        Tim2CheckFileHeaer(pointer + 4);

        ph = Tim2GetPictureHeader(pointer + 4, 0);

        if (ph != NULL)
        {
            // Tim2LoadImage: When tbp specification is -1, get tbp and tbw from GsTex0 member specified in picture header
            // Tim2LoadClut: If there is no cbp specification, get value from GsTex0 member of picture header
            Tim2LoadPicture(ph, -1, -1);
        }
        else
        {
            exit(1);
        }

        pointer += *pointer / 4 + 4;
    }
}

void LoadPackedTexture(u_char *fname)
{
    int rfd;
    int len;
    u_int *pointer;

    rfd = sceOpen((char *)fname, SCE_RDONLY);

    if (rfd < 0)
    {
        exit(1);
    }

    len = sceLseek(rfd, 0, SCE_SEEK_END);
    pointer = (u_int *)malloc(len + 16);
    sceLseek(rfd, 0, SCE_SEEK_SET);
    sceRead(rfd, pointer, len);
    sceClose(rfd);
    LoadPackedTextureFromMemory(pointer);
    free(pointer);
}

void CalcRoomCoord(void *sgd_top, float *pos)
{
    SgCOORDUNIT *cp;
    float *v0;
    float *v1;
    HeaderSection* hs;

    hs = (HeaderSection*)sgd_top;

    cp = hs->coordp;

    sceVu0RotMatrixX(cp->matrix, runit_mtx, PI);

    Vu0CopyVector(cp->matrix[3], pos);

    cp->matrix[3][3] = 1.0f;

    CalcCoordinate(cp, hs->blocks - 1);
}

void SetUpRoomCoordinate(int disp_room, float *pos)
{
    if (area_read_wrk.stat != 0 || disp_room == 0xff)
    {
        return;
    }

    if (room_addr_tbl[disp_room].near_sgd == NULL)
    {
        return;
    }

    CalcRoomCoord(room_addr_tbl[disp_room].near_sgd, pos);

    if (room_addr_tbl[disp_room].ss_sgd != NULL)
    {
        CalcRoomCoord(room_addr_tbl[disp_room].ss_sgd, pos);
    }

    if (room_addr_tbl[disp_room].sh_sgd != NULL)
    {
        CalcRoomCoord(room_addr_tbl[disp_room].sh_sgd, pos);
    }
}

void ChoudoPreRender(FURN_WRK* furn) {
    u_int* tmpModelp;
    u_int* lit_dat;
    int disp_chodo;
    int room_no;
    float grot;
    SgCOORDUNIT* cp;
    static SgLIGHT lights[3];
    static SgLIGHT plights[16];
    static SgLIGHT slights[16];
    sceVu0FVECTOR ambient;
    HeaderSection *hs;

    disp_chodo = furn->furn_no;

    if (disp_chodo >= 1000)
    {
        tmpModelp = door_addr_tbl[disp_chodo - 1000];
    }
    else
    {
        tmpModelp = furn_addr_tbl[disp_chodo];
    }

    if (tmpModelp == NULL)
    {
        return;
    }

    hs = (HeaderSection *)tmpModelp;

    cp = hs->coordp;

    if ((int)cp->matrix % 16) // check for sceVu0FMATRIX address alignment (should be aligned 16)
    {
        return;
    }

    if (abs((int)cp->matrix - (int)hs) <= 512) // matrix can't be too far from the header it's being used in ???
    {
        sceVu0RotMatrixX(cp->matrix, runit_mtx, PI);

        // I've seen this pattern often, I'm pretty sure it's a MACRO
        grot = furn->rotate[1] + PI > PI ? (furn->rotate[1] + PI) - PI2 : furn->rotate[1] + PI;

        sceVu0RotMatrixY(cp->matrix, cp->matrix, grot);
        sceVu0RotMatrixX(cp->matrix, cp->matrix, furn->rotate[0]);
        sceVu0RotMatrixZ(cp->matrix, cp->matrix, furn->rotate[2]);

        Vu0CopyVector(cp->matrix[3], furn->pos);
        cp->matrix[3][3] = 1.0f;

        CalcCoordinate(cp, hs->blocks - 1);

        room_no = furn->room_id;

        if (room_addr_tbl[room_no].near_sgd != NULL)
        {
            lit_dat = room_addr_tbl[room_no].lit_data;

            if (lit_dat != NULL)
            {
                SgReadLights(room_addr_tbl[room_no].near_sgd, lit_dat, ambient, lights, 3, plights, 16, slights, 16);
                SgSetAmbient(ambient);
                SgSetInfiniteLights(camera.zd, lights, lights[0].num);
                SgSetPointLights(plights, plights[0].num);
                SgSetSpotLights(slights, slights[0].num);
                SgClearPreRender((void *)hs, -1);
                SgPreRender((void *)hs, -1);
                SgPreRenderDbgOff();
            }
        }
    }
}

void ChoudoPreRenderDual(FURN_WRK *furn)
{
    u_int *tmpModelp;
    u_int *lit_dat;
    int disp_chodo;
    int room_no;
    static SgLIGHT lights[3];
    static SgLIGHT plights[16];
    static SgLIGHT slights[16];
    sceVu0FVECTOR ambient;

    disp_chodo = furn->furn_no;

    if (disp_chodo < 1000)
    {
        return;
    }

    tmpModelp = door_addr_tbl[disp_chodo - 1000];

    if (tmpModelp == NULL)
    {
        return;
    }

    SgClearPreRender(tmpModelp, -1);
    ChoudoPreRender(furn);

    if (room_wrk.disp_no[0] == furn->room_id)
    {
        room_no = room_wrk.disp_no[1];
    }
    else
    {
        room_no = room_wrk.disp_no[0];
    }

    if (room_no != 0xff)
    {
        if (room_addr_tbl[room_no].near_sgd != NULL)
        {
            lit_dat = room_addr_tbl[room_no].lit_data;

            if (lit_dat != NULL)
            {
                SgReadLights(room_addr_tbl[room_no].near_sgd, lit_dat, ambient, lights, 3, plights, 16, slights, 16);
                SgSetAmbient(ambient);
                SgSetInfiniteLights(camera.zd, lights, lights[0].num);
                SgSetPointLights(plights, plights[0].num);
                SgSetSpotLights(slights, slights[0].num);
                SgPreRender(tmpModelp, -1);
            }
        }
    }
}

void SetPreRender(void *buf, void *light_buf)
{
    static SgLIGHT lights[3];
    static SgLIGHT plights[16];
    static SgLIGHT slights[16];
    SgCOORDUNIT *cp;
    int i;
    HeaderSection *hs;

    hs = (HeaderSection *)buf;
    cp = hs->coordp;

    sceVu0UnitMatrix(cp[0].matrix);

    for (i = 0; i < hs->blocks - 1; i++)
    {
        cp[i].flg = 0;
    }

    for (i = 0; i < hs->blocks - 1; i++)
    {
        SetLWS(&cp[i], &camera);
    }

    lights[0].num = plights[0].num = slights[0].num = 0;

    if (light_buf == NULL)
    {
        SgReadLights(buf, NULL, room_ambient_light, lights, 3, plights, 16, slights, 16);
    }
    else
    {
        SgReadLights(buf, light_buf, room_ambient_light, lights, 3, plights, 16, slights, 16);
    }

    if (lights[0].num != 0 || plights[0].num != 0 || slights[0].num != 0)
    {
        SgSetAmbient(room_ambient_light);
        SgSetInfiniteLights(camera.zd, lights, lights[0].num);
        SgSetPointLights(plights, plights[0].num);
        SgSetSpotLights(slights, slights[0].num);
    }

    SgPreRender(buf, -1);

    cp[0].flg = 0;
}

void ScenePrerender()
{
    int i;
    int disp_room;
    int disp_model;
    u_int *tmpModelp;
    sceVu0FVECTOR ambient;
    static SgLIGHT lights;
    static SgLIGHT ilights[3];
    static SgLIGHT plights[16];
    static SgLIGHT slights[16];

    SgSetAmbient(room_ambient_light);
    SgSetInfiniteLights(camera.zd, room_pararell_light, room_pararell_light[0].num);
    SgSetPointLights(room_point_light, room_point_light[0].num);
    SgSetSpotLights(room_spot_light, room_spot_light[0].num);

    disp_room = room_wrk.disp_no[0];

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

    for (i = 0; i < 60; i++)
    {
        disp_model = furn_wrk[i].furn_no;

        if (disp_model == 0xffff)
        {
            break;
        }

        if (disp_model >= 1000)
        {
            tmpModelp = door_addr_tbl[disp_model - 1000];
        }
        else
        {
            tmpModelp = furn_addr_tbl[disp_model];
        }

        if (tmpModelp != NULL && furn_wrk[i].room_id == disp_room)
        {

            SgClearPreRender(tmpModelp, -1);
            SgPreRender(tmpModelp, -1);
        }
    }

    disp_room = room_wrk.disp_no[1];

    if (disp_room != 0xff || room_addr_tbl[disp_room].near_sgd == NULL)
    {
        SgReadLights(room_addr_tbl[disp_room].near_sgd, room_addr_tbl[disp_room].lit_data, ambient, ilights, 3, plights, 16, slights, 16);
        SgSetAmbient(ambient);
        SgSetInfiniteLights(camera.zd, ilights, ilights[0].num);
        SgSetPointLights(plights, plights[0].num);
        SgSetSpotLights(slights, slights[0].num);
        SgClearPreRender(room_addr_tbl[disp_room].near_sgd, -1);
        SgPreRender(room_addr_tbl[disp_room].near_sgd, -1);

        tmpModelp = room_addr_tbl[disp_room].ss_sgd;

        if (tmpModelp != NULL)
        {

            SgClearPreRender(tmpModelp, -1);
            SgPreRender(tmpModelp, -1);
        }


        for (i = 0; i < 60; i++)
        {
            disp_model = furn_wrk[i].furn_no;

            if (disp_model == 0xffff)
            {
                break;
            }

            if (disp_model < 1000)
            {
                tmpModelp = furn_addr_tbl[disp_model];

                if (tmpModelp != NULL && furn_wrk[i].room_id == disp_room)
                {

                    SgClearPreRender(tmpModelp, -1);
                    SgPreRender(tmpModelp, -1);
                }
            }
        }
    }
}

void RequestBlackWhiteMode()
{
    if (loadbw_flg != 0)
    {
        return;
    }

    dbg_wrk.black_white = 1;

    SetBlackWhiteCLUT();

    if (realtime_scene_flg != 0)
    {
        SceneScenePrerender();
    }
    else
    {
        ScenePrerender();
    }

    SetPlyrClut(loadbw_flg);
}

void CancelBlackWhiteMode()
{
    if (loadbw_flg == 0)
    {
        return;
    }

    dbg_wrk.black_white = 0;

    ClearBlackWhiteCLUT();

    if (realtime_scene_flg != 0)
    {
        SceneScenePrerender();
    }
    else
    {
        ScenePrerender();
    }

    SetPlyrClut(loadbw_flg);
}

void SetEnvironment()
{
    qword *base;

    base = getObjWrk() + 1;

    base[0][0] = (int)(SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 10) >>  0); // first half
    base[0][1] = (int)(SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 10) >> 32); // second half
    base[0][2] = 0 \
        | SCE_GIF_PACKED_AD << (0 * 4)
        | SCE_GIF_PACKED_AD << (1 * 4)
        | SCE_GIF_PACKED_AD << (2 * 4)
        | SCE_GIF_PACKED_AD << (3 * 4)
        | SCE_GIF_PACKED_AD << (4 * 4)
        | SCE_GIF_PACKED_AD << (5 * 4)
        | SCE_GIF_PACKED_AD << (6 * 4)
        | SCE_GIF_PACKED_AD << (7 * 4);
    base[0][3] = 0 \
        | SCE_GIF_PACKED_AD << (0 * 4)
        | SCE_GIF_PACKED_AD << (1 * 4);

    base[1][2] = SCE_GS_TEXFLUSH;

    base[2][2] = SCE_GS_ALPHA_1;
    base[2][0] = (int)(SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x40) >>  0); // first half
    base[2][1] = (int)(SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x40) >> 32); // second half

    base[3][2] = SCE_GS_TEX1_1;
    *(long *)base[3] = SCE_GS_SET_TEX1_1(0, 0, SCE_GS_LINEAR, SCE_GS_LINEAR, 0, 0, 0);

    base[4][2] = SCE_GS_CLAMP_1;
    base[4][0] = (int)(SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0) >>  0); // first half
    base[4][1] = (int)(SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0) >> 32); // second half

    base[5][2] = SCE_GS_TEST_1;
    base[5][0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GEQUAL, 1, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);

    base[6][2] = SCE_GS_SCISSOR_1;
    *(long *)base[6] = *(long *)&pdrawenv->scissor1;

    base[7][2] = SCE_GS_ZBUF_1;
    *(long *)base[7] = *(long *)&pdrawenv->zbuf1;

    base[8][2] = SCE_GS_FRAME_1;
    *(long *)base[8] = *(long *)&pdrawenv->frame1;

    base[9][2] = SCE_GS_XYOFFSET_1;
    *(long *)base[9] = *(long *)&pdrawenv->xyoffset1;

    base[10][2] = SCE_GS_DTHE;
    base[10][0] = SCE_GS_SET_DTHE(0);

    AppendDmaBufferFromEndAddress(base + 11);
}

void SetEnvironment2()
{
    qword *base;

    base = getObjWrk() + 1;

    base[0][0] = (int)(SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 10) >>  0); // first half
    base[0][1] = (int)(SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 10) >> 32); // second half
    base[0][2] = 0 \
        | SCE_GIF_PACKED_AD << (0 * 4)
        | SCE_GIF_PACKED_AD << (1 * 4)
        | SCE_GIF_PACKED_AD << (2 * 4)
        | SCE_GIF_PACKED_AD << (3 * 4)
        | SCE_GIF_PACKED_AD << (4 * 4)
        | SCE_GIF_PACKED_AD << (5 * 4)
        | SCE_GIF_PACKED_AD << (6 * 4)
        | SCE_GIF_PACKED_AD << (7 * 4);
    base[0][3] = 0 \
        | SCE_GIF_PACKED_AD << (0 * 4)
        | SCE_GIF_PACKED_AD << (1 * 4);

    base[1][2] = SCE_GS_TEXFLUSH;

    base[2][2] = SCE_GS_ALPHA_1;
    base[2][0] = (int)(SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x40) >>  0); // first half
    base[2][1] = (int)(SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x40) >> 32); // second half

    base[3][2] = SCE_GS_TEX1_1;
    *(long *)base[3] = SCE_GS_SET_TEX1_1(0, 0, SCE_GS_LINEAR, SCE_GS_LINEAR, 0, 0, 0);

    base[4][2] = SCE_GS_CLAMP_1;
    base[4][0] = (int)(SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0) >>  0); // first half
    base[4][1] = (int)(SCE_GS_SET_CLAMP_1(SCE_GS_REPEAT, SCE_GS_REPEAT, 0, 0, 0, 0) >> 32); // second half

    base[5][2] = SCE_GS_TEST_1;
    base[5][0] = SCE_GS_SET_TEST_1(1, SCE_GS_ALPHA_GEQUAL, 1, SCE_GS_AFAIL_KEEP, 0, 0, 1, SCE_GS_DEPTH_GEQUAL);

    base[6][2] = SCE_GS_SCISSOR_1;
    *(long *)base[6] = *(long *)&pdrawenv->scissor1;

    base[7][2] = SCE_GS_ZBUF_1;
    *(long *)base[7] = *(long *)&pdrawenv->zbuf1;

    base[8][2] = SCE_GS_FRAME_1;
    *(long *)base[8] = *(long *)&pdrawenv->frame1;

    base[9][2] = SCE_GS_XYOFFSET_1;
    *(long *)base[9] = *(long *)&pdrawenv->xyoffset1;

    base[10][2] = SCE_GS_DTHE;
    base[10][0] = SCE_GS_SET_DTHE(0);

    AppendDmaBufferFromEndAddress(base + 11);
}

void ClearFrame()
{
    dword *base;

    base = (dword *)SgGetWorkBase();

    base[0][0] = SCE_GIF_SET_TAG(1, SCE_GS_TRUE, SCE_GS_TRUE, 6, SCE_GIF_PACKED, 4);
    base[0][1] = 0 \
        | SCE_GS_RGBAQ << (4 * 0)
        | SCE_GS_XYZF2 << (4 * 1)
        | SCE_GS_RGBAQ << (4 * 2)
        | SCE_GS_XYZF2 << (4 * 3);

    base++;

    // SCE_GS_RGBAQ
    ((qword *)base)[0][0] = 0x80;
    ((qword *)base)[0][1] = 0;
    ((qword *)base)[0][2] = 0;
    ((qword *)base)[0][3] = 0x80;

    // SCE_GS_XYZF2
    ((qword *)base)[1][0] = GS_X_COORD(0);
    ((qword *)base)[1][1] = GS_Y_COORD(0);
    ((qword *)base)[1][2] = 0x3a980;
    ((qword *)base)[1][3] = 0;

    base += 2;

    // SCE_GS_RGBAQ
    ((qword *)base)[0][0] = 0x80;
    ((qword *)base)[0][1] = 0;
    ((qword *)base)[0][2] = 0;
    ((qword *)base)[0][3] = 0x80;

    // SCE_GS_XYZF2
    ((qword *)base)[1][0] = GS_X_COORD(SCR_WIDTH);
    ((qword *)base)[1][1] = GS_Y_COORD(SCR_HEIGHT);
    ((qword *)base)[1][2] = 0x3a980;
    ((qword *)base)[1][3] = 0;

    base += 2;

    SgSetWorkBase((qword *)base);
}

void SetLWS(SgCOORDUNIT *cp, SgCAMERA *camera)
{
    if (cp->flg != 0)
    {
        return;
    }

    if (cp->parent == NULL)
    {
        sceVu0CopyMatrix(cp->lwmtx, cp->matrix);
        sceVu0MulMatrix(cp->workm, camera->ws, cp->lwmtx);

        cp->flg = 1;
    }
    else
    {
        SetLWS(cp->parent, camera);
        sceVu0MulMatrix(cp->lwmtx, cp->parent->lwmtx, cp->matrix);
        sceVu0MulMatrix(cp->workm, camera->ws, cp->lwmtx);

        cp->flg = 1;
    }
}

void CalcCoordinate(SgCOORDUNIT *cp, int blocks)
{
    int i;

    for (i = 0; i < blocks; i++)
    {
        cp[i].flg = 0;
    }

    for (i = 0; i < blocks; i++)
    {
        SetLWS(&cp[i], &camera);
    }
}

void CopyCoordinate(SgCOORDUNIT *dcp, SgCOORDUNIT *scp, int blocks)
{
    int i;

    for (i = 0; i < blocks; i++)
    {
        dcp[i] = scp[i];
    }
}

char* appendchar(char *dest, char *source, char *append)
{
    strcpy(dest, source);
    strcat(dest, append);

    return dest;
}

void SetUnitMatrix(u_int *pmodel)
{
    HeaderSection *hs;
    SgCOORDUNIT *cp;
    int i;

    hs = (HeaderSection*)pmodel;

    cp = hs->coordp;

    for (i = 0; i < (hs->blocks - 1); i++)
    {
        sceVu0UnitMatrix(cp[i].matrix);
    }
}

void InitializeRoom(RMDL_ADDR *room_tbl)
{
    u_int pack_num;
    u_int *prim;

    prim = room_tbl->pk2;

    pack_num = prim[0];
    prim = &prim[4];

    room_tbl->near_sgd = &prim[4];
    prim = &prim[0] + (prim[0] / 4 + 4);

    room_tbl->far_sgd = &prim[4];
    prim = &prim[0] + (prim[0] / 4 + 4);

    if (pack_num == 4)
    {
        room_tbl->ss_sgd = &prim[4];
        prim = &prim[0] + (prim[0] / 4 + 4);

        room_tbl->sh_sgd = &prim[4];
    }
    else
    {
        room_tbl->ss_sgd = NULL;
        room_tbl->sh_sgd = NULL;
    }

    SgMapUnit(room_tbl->near_sgd);
    SgMapUnit(room_tbl->far_sgd);
    SgMapUnit(room_tbl->lit_data);

    room_tbl->far_sgd[2] = room_tbl->near_sgd[2];

    SetPreRender(room_tbl->near_sgd, room_tbl->lit_data);

    if (CheckMirrorModel(room_tbl->near_sgd) != 0)
    {
        ((char *)room_tbl->near_sgd)[5] |= 2;
    }

    if (room_tbl->ss_sgd != NULL)
    {
        SgMapUnit(room_tbl->ss_sgd);
        SetPreRender(room_tbl->ss_sgd, room_tbl->lit_data);
    }

    if (room_tbl->sh_sgd != NULL)
    {
        SgMapUnit(room_tbl->sh_sgd);
    }
}

void gra3dInitFirst()
{
    sceDmaChan *d1;

    sceVu0UnitMatrix(runit_mtx);

    runit_mtx[0][0] = 25.0f;
    runit_mtx[1][1] = 25.0f;
    runit_mtx[2][2] = 25.0f;

    SgSetPacketBuffer(UNCACHED(0x42fa00), 0x17830, UNCACHED_ACCEL(tag_buffer), 0x1800);
    SgSetVNBuffer((sceVu0FVECTOR *)0x420000, 4000);

    SgInit3D();
    objInit();

    d1 = sceDmaGetChan(1);
    d1->chcr.TTE = 1;

    sceDmaSync(d1, 0, 0);
    sceDmaSend(d1, &dma);
}

u_int PlayerModelInit()
{
    int i;
    u_int *tmpp;
    u_int *p;

    tmpp = (u_int *)0x9a0000;

    pmanmodel[0] = &tmpp[0];
    pmanmpk[0] = &tmpp[0];

    tmpp = &tmpp[4];

    i = 0;

    while (tmpp[0] - 1 < 0x7fffffff)
    {
        p = &tmpp[4];

        if (i == 0)
        {
            pgirlbase = p;
        }

        i++;

        SgMapUnit(p);

        tmpp += tmpp[0] / 4 + 4;
    }

    return (u_int)tmpp;
}

u_int PlayerAccessoryInit(int addr)
{
    int i;
    u_int *tmpp;

    tmpp = (u_int *)addr;
    tmpp = &tmpp[4];

    i = 0;

    while (tmpp[0] - 1 < 0x7fffffff)
    {
        if (i < 2)
        {
            pgirlacs[i] = &tmpp[4];

            SgMapUnit(&tmpp[4]);

            tmpp += tmpp[0] / 4 + 4;
        }
        else
        {
            pgirlshadow = &tmpp[4];

            SgMapUnit(&tmpp[4]);

            tmpp += tmpp[0] / 4 + 4;

            pgirlshadow[2] = pgirlbase[2];
        }

        i++;
    }

    item_addr_tbl[1] = pgirlacs[1];

    return (u_int)tmpp;
}

u_int SGDLoadInit(u_int *addr, int size)
{
    pgirlshadow = addr;

    SgMapUnit(addr);

    pgirlshadow[2] = pgirlbase[2];

    addr += (size + 16) / 4;

    return (u_int)addr;
}

void gra3dInit()
{
    int i;

    for (i = 0; i < 64; i++)
    {
        if (fog_param[i][1] == 0.0f)
        {
            fog_param[i][0] = 0.0f;
            fog_param[i][1] = 255.0f;
            fog_param[i][2] = 1000.0f;
            fog_param[i][3] = 10000.0f;

            fog_rgb[i][0] = 35.0f;
            fog_rgb[i][1] = 35.0f;
            fog_rgb[i][2] = 35.0f;
        }
    }
}

void Init3D()
{
    SgInit3D();

    objInit();

    InitialDmaBuffer();

    SetShadowFundamentScale(1000);
}

void SetDefaultLightPower(float pow)
{
    lights[4].diffuse[0] = pow;
    lights[4].diffuse[1] = pow;
    lights[4].diffuse[2] = pow;
    lights[4].diffuse[3] = 255.0f;

    lights[4].specular[0] = pow;
    lights[4].specular[1] = pow;
    lights[4].specular[2] = pow;
    lights[4].specular[3] = 255.0f;

    lights[4].ambient[0] = pow;
    lights[4].ambient[1] = pow;
    lights[4].ambient[2] = pow;
    lights[4].ambient[3] = 0.0f;
}

void RequestSpirit(int no, int swch)
{
    ene_display[no] = swch;
}

void InitRequestSpirit()
{
    u_int i;

    for (i = 0; i < 4; i++)
    {
        ene_display[i] = 0;
    }
}

void RequestFly(int no, int swch)
{
    fly_display[no] = swch;
}

void InitRequestFly()
{
    u_int i;

    for (i = 0; i < 3; i++)
    {
        fly_display[i] = 0;
    }
}

void SetSgSpotLight(SPOT_WRK *spot, SgLIGHT *plight)
{
    Vu0CopyVector(spot->pos, plight->pos);
    Vu0CopyVector(spot->direction, plight->direction);
    Vu0CopyVector(spot->diffuse, plight->diffuse);

    spot->intens = plight->intens;
    spot->power = plight->power;
}

void SetMyLight(LIGHT_PACK *light_pack, float *eyevec)
{
    int i;
    int num;
    static SgLIGHT parallel[6];
    static SgLIGHT point[6];
    static SgLIGHT spot[6];

    SgSetAmbient(light_pack->ambient);

    num = light_pack->parallel_num;

    if (num > 3)
    {
        num = 3;
    }

    for (i = 0; i < 3; i++)
    {
        Vu0ZeroVector(parallel[i].diffuse);
        Vu0ZeroVector(parallel[i].specular);
        Vu0ZeroVector(parallel[i].ambient);
    }

    for (i = 0; i < num; i++)
    {
        Vu0CopyVector(parallel[i].direction, light_pack->parallel[i].direction);
        Vu0CopyVector(parallel[i].diffuse, light_pack->parallel[i].diffuse);
        Vu0CopyVector(parallel[i].specular, parallel[i].diffuse);

        parallel[i].Enable = 0;
    }

    SgSetInfiniteLights(eyevec, parallel, num);

    num = light_pack->point_num;

    if (num > 3)
    {
        num = 3;
    }

    for (i = 0; i < num; i++)
    {
        Vu0CopyVector(point[i].pos, light_pack->point[i].pos);
        Vu0CopyVector(point[i].diffuse, light_pack->point[i].diffuse);
        Vu0CopyVector(point[i].specular, point[i].diffuse);

        point[i].power = light_pack->point[i].power;
        point[i].Enable = 0;
        point[i].SEnable = 0;
    }

    SgSetPointLights(point, num);

    num = light_pack->spot_num;

    if (num > 3)
    {
        num = 3;
    }

    for (i = 0; i < num; i++)
    {
        Vu0CopyVector(spot[i].pos, light_pack->spot[i].pos);
        Vu0CopyVector(spot[i].direction, light_pack->spot[i].direction);
        Vu0CopyVector(spot[i].diffuse, light_pack->spot[i].diffuse);
        Vu0CopyVector(spot[i].specular, spot[i].diffuse);

        spot[i].intens = light_pack->spot[i].intens;
        spot[i].power = light_pack->spot[i].power;
        spot[i].Enable = 0;
        spot[i].SEnable = 0;
    }

    SgSetSpotLights(spot, num);
}

void TransMyLight(LIGHT_PACK *dest_pack, LIGHT_PACK *light_pack, sceVu0FVECTOR pos)
{
    int i;
    int j;
    int k;
    int num;
    int nbuf[9];
    int stocks;
    sceVu0FVECTOR subvec;
    sceVu0FVECTOR mulvec;
    sceVu0FVECTOR dirbuf[9];
    sceVu0FVECTOR difbuf[9];
    float len;
    float lenbuf[9];

    stocks = 0;

    num = light_pack->parallel_num > 3 ? 3 : light_pack->parallel_num;  // need to call this twice to fix stack allocation order !!
    num = light_pack->parallel_num > 3 ? 3 : light_pack->parallel_num;

    if (num != 0)
    {
        for (i = 0; i < num; i++)
        {
            Vu0CopyVector(difbuf[stocks],  light_pack->parallel[i].diffuse);
            Vu0CopyVector(dirbuf[stocks],  light_pack->parallel[i].direction);

            nbuf[stocks] = stocks;
            lenbuf[stocks] = difbuf[stocks][0] + difbuf[stocks][1] + difbuf[stocks][2];

            stocks++;
        }
    }

    num = light_pack->point_num > 3 ? 3 : light_pack->point_num;

    if (num != 0)
    {
        for (i = 0; i < num; i++)
        {
            sceVu0SubVector(subvec, light_pack->point[i].pos, pos);
            sceVu0MulVector(mulvec, subvec, subvec);

            len = light_pack->point[i].power / SgSqrtf(mulvec[0] + mulvec[1] + mulvec[2]);

            sceVu0ScaleVector(difbuf[stocks], light_pack->point[i].diffuse, len * 6.0f);
            Vu0CopyVector(dirbuf[stocks],  subvec);

            nbuf[stocks] = stocks;
            lenbuf[stocks] = difbuf[stocks][0] + difbuf[stocks][1] + difbuf[stocks][2];

            stocks++;
        }
    }

    num = light_pack->spot_num > 3 ? 3 : light_pack->spot_num;

    for (i = 0; i < num; i++)
    {
        sceVu0SubVector(subvec, light_pack->spot[i].pos, pos);
        sceVu0MulVector(mulvec, subvec, subvec);

        len = light_pack->spot[i].power / SgSqrtf(mulvec[0] + mulvec[1] + mulvec[2]);

        sceVu0ScaleVector(difbuf[stocks], light_pack->spot[i].diffuse, len * 0.25f);
        Vu0CopyVector(dirbuf[stocks], light_pack->spot[i].direction);

        nbuf[stocks] = stocks;
        lenbuf[stocks] = difbuf[stocks][0] + difbuf[stocks][1] + difbuf[stocks][2];

        stocks++;
    }

    for (j = 0; j < stocks - 1; j++)
    {
        for (i = j + 1; i < stocks; i++)
        {
            if (lenbuf[j] < lenbuf[i])
            {
                float f;

                f = lenbuf[j];
                lenbuf[j] = lenbuf[i];
                lenbuf[i] = f;

                k = nbuf[j];
                nbuf[j] = nbuf[i];
                nbuf[i] = k;
            }
        }
    }

    dest_pack->spot_num = 0;
    dest_pack->point_num = 0;

    if (stocks > 3)
    {
           int dest_num;
        int source_num;
        float ad;
        float as;
        float total;
        sceVu0FVECTOR ld;
        sceVu0FVECTOR ls;

        dest_num = nbuf[2];
        source_num = nbuf[3];

        total = lenbuf[2] + lenbuf[3];
        ad = lenbuf[2] / total;
        as = lenbuf[3] / total;

        sceVu0ScaleVector(ld, difbuf[dest_num], ad);
        sceVu0ScaleVector(ls, difbuf[source_num], as);
        Vu0AddVector(difbuf[dest_num], ld, ls);

        sceVu0ScaleVector(ld, dirbuf[dest_num], ad);
        sceVu0ScaleVector(ls, dirbuf[source_num], as);
        Vu0AddVector(dirbuf[dest_num], ld, ls);

        stocks = 3;
    }

    for (i = 0; i < stocks; i++)
    {
        Vu0CopyVector(dest_pack->parallel[i].diffuse,  difbuf[nbuf[i]]);
        _NormalizeVector(dest_pack->parallel[i].direction, dirbuf[nbuf[i]]);
    }

    dest_pack->parallel_num = stocks;
}

void CalcReflectLight()
{
    SPOT_WRK *psp;
    sceVu0FVECTOR cdir;
    sceVu0FVECTOR sdirection;
    sceVu0FVECTOR ipos;
    float ip;
    float ip2;
    float intens;
    float len;

    if (mir_reflect_flg != 0)
    {
        return;
    }

    psp = &room_wrk.mylight[0].spot[0];

    Vu0SubVector(cdir, psp->pos, mir_center);

    len = SgSqrtf(cdir[0] * cdir[0] + cdir[1] * cdir[1] + cdir[2] * cdir[2]);

    _NormalizeVector(cdir, cdir);
    _NormalizeVector(sdirection, psp->direction);

    ip = sceVu0InnerProduct(cdir, sdirection);
    ip2 = sceVu0InnerProduct(sdirection, mir_norm);

    if (ip < 0.0f || ip2 < 0.1f)
    {
        return;
    }

    if ((psp->intens > ip && len > 600.0f) || len > 2200.0f)
    {
        return;
    }

    intens = psp->intens;

    if (1500.0f < len)
    {
        ip2 *= (2200.0f - len) / 700.0f;
    }

    mir_reflect_flg = 1;

    Vu0SubVector(ipos, psp->pos, psp->direction);
    Vu0LoadMatrix(mir_mtx);
    Vu0ApplyVectorInline(mir_reflect_spot.pos, psp->pos);
    Vu0ApplyVectorInline(mir_reflect_spot.direction, ipos);
    Vu0SubVector(mir_reflect_spot.direction, mir_reflect_spot.pos, mir_reflect_spot.direction);

    Vu0CopyVector(mir_reflect_spot.diffuse, psp->diffuse);

    mir_reflect_spot.intens = intens;
    mir_reflect_spot.power = psp->power * ip2;
}

void AppendReflectLight(LIGHT_PACK *light_pack)
{
    if (mir_reflect_flg != 0)
    {
        light_pack->spot[light_pack->spot_num] = mir_reflect_spot;

        light_pack->spot_num++;
    }
}

void DeleteReflectLight(LIGHT_PACK *light_pack)
{
    if (mir_reflect_flg != 0)
    {
        light_pack->spot_num--;
    }
}
void SetMyLightObj(LIGHT_PACK *trans_pack, LIGHT_PACK *light_pack, float *cam_zd, float *pos)
{
    int old_parallel_num;
    int old_spot_num;
    int old_point_num;

    old_parallel_num = light_pack->parallel_num;
    old_spot_num = light_pack->spot_num;
    old_point_num = light_pack->point_num;

    if (old_spot_num < 3)
    {
        AppendReflectLight(light_pack);
    }

    TransMyLight(trans_pack, light_pack, pos);
    SetMyLight(trans_pack, cam_zd);

    if (old_spot_num < 3)
    {
        DeleteReflectLight(light_pack);
    }

    light_pack->parallel_num = old_parallel_num;
    light_pack->spot_num = old_spot_num;
    light_pack->point_num = old_point_num;
}

void SetMyLightRoom(LIGHT_PACK *light_pack, float *eyevec)
{
    int old_spot_num;

    old_spot_num = light_pack->spot_num < 3;

    if (old_spot_num != 0)
    {
        AppendReflectLight(light_pack);
    }

    SetMyLight(light_pack, eyevec);

    if (old_spot_num != 0)
    {
        DeleteReflectLight(light_pack);
    }
}

void SceneSortUnit()
{
    search_num2 = 0;
    search_num = 0;

    CalcReflectLight();
    DrawRoom(mir_room_workno);
    DrawFurniture(room_wrk.disp_no[mir_room_workno]);
    DrawGirl(1);
    SetMyLight(room_wrk.mylight,camera.zd);
}

void Kagu027Control(void *sgd_top)
{
    static float trans_rate = 128.0f;
    static float trans_added = -0.035555556; // ~ -8.0f/225.0f
    TextureAnimation *pta;
    SgMaterial *matp;
    u_int *phead;
    HeaderSection *hs;

    pta = (TextureAnimation *)GetTextureAnimation(sgd_top);

    if (pta != NULL && pta->AnmON == 0)
    {
        pta->AnmON = 1;
        pta->AnmLoop = 1;
        pta->AnmStep = 8;
        pta->AnmCnt = 0;
    }

    hs = (HeaderSection *)sgd_top;

    matp = hs->matp;
    phead = hs->phead;

    while ((u_int)matp < (u_int)phead)
    {
        if (strncmp("sha_", matp->name, 4) == 0)
        {
            matp->Diffuse[3] = trans_rate;
        }

        matp++;
    }
}

static u_char bak_cam_door_or = 0;
static int locked_fog_room_no = -1;
static u_char fog_lock = 0;

void MakeDebugValue()
{
    disp3d_all = dbg_wrk.disp_3d_all;
    disp3d_room = dbg_wrk.disp_3d_room != 0 && disp3d_room_req != 0;
    disp3d_furn = dbg_wrk.disp_3d_furn != 0 && disp3d_furn_req != 0;
    disp3d_girl = dbg_wrk.disp_3d_girl;
    disp3d_enemy = dbg_wrk.disp_3d_enemy;
    disp3d_room_shadow = dbg_wrk.disp_3d_room_shadow;
    disp3d_mirror = dbg_wrk.disp_3d_mirror;
    disp3d_2ddraw = dbg_wrk.disp_3d_2ddraw;

    if (dbg_wrk.black_white != 0)
    {
        if (loadbw_flg == 0)
        {
            RequestBlackWhiteMode();
        }

        if (dbg_wrk.black_white != 0)
        {
            return;
        }
    }

    if (loadbw_flg == 0)
    {
        return;
    }

    CancelBlackWhiteMode();
}

void AppendSearchModel(void *sgd_top, int room_no)
{
    if (room_no == room_wrk.disp_no[0])
    {
        if (search_num < 15)
        {
            ssearch_models[search_num] = sgd_top;
            search_num++;
        }
    }
    else
    {
        if (search_num2 < 15)
        {
            ssearch_models2[search_num] = sgd_top;
            search_num2++;
        }
    }
}

void DrawOneRoom(int no)
{
    int disp_room;
    void *mdl_addr;
    SgCOORDUNIT *cp;
    HeaderSection *hs;

    disp_room = room_wrk.disp_no[no];
    mdl_addr = room_addr_tbl[disp_room].near_sgd;

    if (disp_room == 0xff)
    {
        return;
    }

    hs = (HeaderSection *)mdl_addr;

    if (hs == NULL || hs->VersionID != 0x1050)
    {
        return;
    }

    cp = hs->coordp;

    if (cp->flg == 0)
    {
        SetUpRoomCoordinate(disp_room, room_wrk.pos[no]);
    }

    Vu0CopyVector(room_wrk.mylight[no].ambient, room_ambient_light);

    SetMyLightRoom(&room_wrk.mylight[no], camera.zd);
    SgSortUnitP(mdl_addr, -1);
    AppendSearchModel(mdl_addr, disp_room);

    mdl_addr = room_addr_tbl[disp_room].ss_sgd;

    if (mdl_addr != NULL)
    {
        SgSortUnitP(mdl_addr, -1);
        AppendSearchModel(mdl_addr, disp_room);
    }
}

void DrawRoom(int disp_no)
{
    int j;

    if (disp3d_room == 0)
    {
        return;
    }

    if (disp_no == -1)
    {
        for (j = 1; j >= 0; j--)
        {
            DrawOneRoom(j);
        }
    }
    else
    {
        DrawOneRoom(disp_no);
    }
}

int CalcShadowDirecion(ShadowHandle *shandle)
{
    HeaderSection *hs;
    SgCOORDUNIT *cp;
    sceVu0FVECTOR center;
    sceVu0FVECTOR interest;
    sceVu0FVECTOR tmpvec;
    sceVu0FVECTOR sdirection;
    float degree;
    int num;
    int i;
    SPOT_WRK *psp;
    static int chk_bound[4] = {0, 1, 4, 5};

    shandle->color[0] = 0x80;
    shandle->color[1] = 0x80;
    shandle->color[2] = 0x80;
    shandle->color[3] = 0x50;

    if (plyr_wrk.shadow_direction[3] == 1.0f)
    {
        Vu0CopyVector(shandle->direction, plyr_wrk.shadow_direction);

        return 1;
    }

    psp = &room_wrk.mylight[0].spot[0];

    _NormalizeVector(sdirection, psp->direction);

    hs = (HeaderSection *)shandle->shadow_model;
    cp = hs->coordp;

    sceVu0AddVector(center, shandle->bbox[0], shandle->bbox[5]);
    sceVu0ScaleVector(center, center, 0.5f);

    num = shandle->smodel_num < 0 ? 0 : shandle->smodel_num;

    center[1] = shandle->bbox[0][1] * 0.7f + shandle->bbox[8][3] * 0.3f;
    center[3] = 1.0f;

    sceVu0ApplyMatrix(center,cp[num].lwmtx, center);
    sceVu0SubVector(tmpvec, psp->pos, center);
    _NormalizeVector(tmpvec, tmpvec);

    degree = sceVu0InnerProduct(tmpvec, sdirection);

    if (degree * degree - psp->intens < 0.0f)
    {
        for (i = 0; i < 4; i++)
        {
            sceVu0ApplyMatrix(tmpvec, cp[num].lwmtx, shandle->bbox[chk_bound[i]]);
            sceVu0SubVector(tmpvec, psp->pos, tmpvec);
            _NormalizeVector(tmpvec, tmpvec);
            degree = sceVu0InnerProduct(tmpvec, sdirection);
            if (degree * degree - psp->intens > 0.0f)
            {
                break;
            }
        }
    }

    sceVu0SubVector(interest, psp->pos, center);
    _NormalizeVector(interest, interest);
    sceVu0ScaleVector(interest, interest, 0.9f);
    sceVu0ScaleVector(tmpvec, sdirection, 0.1f);
    sceVu0AddVector(shandle->direction, interest, tmpvec);
    _NormalizeVector(shandle->direction, shandle->direction);

    if (shandle->direction[1] > -0.1f)
    {
        shandle->direction[1] = -0.1f;

        _NormalizeVector(shandle->direction, shandle->direction);
    }

    shandle->direction[3] = 1.0f;

    degree = sceVu0InnerProduct(shandle->direction, sdirection);
    degree = (SgACosf(degree) * 180.0f) / PI_alt;

    if (degree > 50.0f)
    {
        degree = (degree - 50.0f) * -1.6f + 80.0f;

        if (degree <= 1.0f)
        {
            return 0;
        }

        shandle->color[3] = degree;
    }

    return 1;
}

u_int* SearchBoundingBoxPacket(u_int *prim)
{
    if (prim == NULL)
    {
        return NULL;
    }

    while (*prim != NULL)
    {
        if (prim[1] == 4)
        {
            return prim;
        }

        prim = *(u_int **)prim;
    }

    return NULL;
}

void DrawRoomShadow()
{
    int i;
    int disp_room;
    SgCOORDUNIT *cp;
    SgCOORDUNIT oldcoord;
    ShadowHandle shandle;
    u_int *prim;
    HeaderSection *hs;
    u_int **pprim;

    if (disp3d_room_shadow == 0)
    {
        return;
    }

    disp_room = room_wrk.disp_no[0];

    if (disp_room == 0xff)
    {
        return;
    }

    if (room_addr_tbl[disp_room].near_sgd == NULL)
    {
        return;
    }

    if (room_addr_tbl[disp_room].sh_sgd == NULL)
    {
        return;
    }

    hs = (HeaderSection *)room_addr_tbl[disp_room].sh_sgd;

    if (room_addr_tbl[disp_room].ss_sgd == NULL)
    {
        return;
    }

    cp = hs->coordp;

    *cp = *((HeaderSection *)room_addr_tbl[disp_room].near_sgd)->coordp;

    CalcCoordinate(cp, hs->blocks - 1);

    shandle.search_model = (void **)ssearch_models;
    shandle.source_model = room_addr_tbl[disp_room].ss_sgd;
    shandle.shadow_model = hs;
    shandle.search_num = search_num;
    shandle.camera = nowcamera;

    SgSortUnitP(hs, 0);

    pprim = (u_int **)&hs->primitives;

    for (i = 1; i < hs->blocks - 1; i++)
    {
        prim = SearchBoundingBoxPacket(pprim[i]);

        if (prim != NULL)
        {
            shandle.smodel_num = i;
            shandle.bbox = (sceVu0FVECTOR *)&prim[4];

            if (CalcShadowDirecion(&shandle) != 0)
            {
                DrawShadow(&shandle, SetEnvironment);
            }
        }
    }

    nowcamera = shandle.camera;

    SetEnvironment();
}

void DrawFurniture(int disp_room)
{
    int i;
    int j;
    int disp_chodo;
    int now_room;
    SgCOORDUNIT *cp;
    u_int *tmpModelp;
    float grot;
    static int old_frame_counter;
    u_char acs_flg;

    if (disp3d_furn == 0)
    {
        return;
    }

    now_room = room_wrk.disp_no[0];

    for (j = 0; j < 60; j++)
    {
        disp_chodo = furn_wrk[j].furn_no;

        if (furn_wrk[j].furn_no == 0xffff)
        {
            break;
        }

        if (disp_chodo >= 1000)
        {
            tmpModelp = door_addr_tbl[disp_chodo - 1000];

            if (now_room == 21 && (disp_chodo == 1051 || disp_chodo == 1052))
            {
                continue;
            }
        }
        else
        {
            tmpModelp = furn_addr_tbl[disp_chodo];

            if (room_wrk.disp_no[1] == 0xff && furn_wrk[j].room_id != now_room)
            {
                continue;
            }
        }

        if (tmpModelp != NULL && (disp_room == -1 || disp_room == furn_wrk[j].room_id))
        {
            HeaderSection *hs;

            hs = (HeaderSection *)tmpModelp;
            cp = hs->coordp;

            acs_flg = furn_dat[furn_wrk[j].id].acs_flg;

            if (((u_int)cp->matrix % 16) == 0) // pointer alignment check
            {
                if (__builtin_abs((int)cp - (int)tmpModelp) <= 512)
                {
                    sceVu0RotMatrixX(cp->matrix, runit_mtx, PI);
                    grot = furn_wrk[j].rotate[1] + PI;
                    if (PI < grot)
                    {
                        grot = grot - PI2;
                    }
                    sceVu0RotMatrixZ(cp->matrix, cp->matrix, furn_wrk[j].rotate[2]);
                    sceVu0RotMatrixX(cp->matrix, cp->matrix, furn_wrk[j].rotate[0]);
                    sceVu0RotMatrixY(cp->matrix, cp->matrix, grot);
                    Vu0CopyVector(cp->matrix[3], furn_wrk[j].pos);
                    cp->matrix[3][3] = 1.0f;

                    if ((hs->kind & 1) == 0)
                    {
                        SetSpeFurnLight(furn_wrk + j);
                    }
                    else
                    {
                        SetMyLightRoom(&furn_wrk[j].mylight, camera.zd);
                    }

                    if (disp_chodo >= 1000 || (acs_flg & 0xf0) == 0)
                    {
                        CalcCoordinate(cp, hs->blocks - 1);
                        SgSortUnitKind(tmpModelp, -1);
                    }
                    else
                    {
                        if ((acs_flg & 0x40) != 0)
                        {
                            char flg;

                            flg = 0;

                            for (i = 0; i < 20; i++)
                            {
                                if (rope_ctrl[i].furn_id == furn_wrk[j].id)
                                {
                                    break;
                                }

                                if (i == 19)
                                {
                                    u_int k;

                                    for (k = 0; k < 60; k++) {}
                                    for (k = 0; k < 60; k++) {}

                                    flg = 1;
                                }
                            }

                            if (!flg)
                            {
                                if (disp_frame_counter != old_frame_counter)
                                {
                                    HeaderSection *ghs;

                                    ghs = (HeaderSection *)pgirlbase;

                                    acsMoveRope(&rope_ctrl[i], ghs->coordp, m000_collision, cp->matrix);
                                }

                                acsCalcCoordinate(cp, hs->blocks - 1, &furn_wrk[j], &rope_ctrl[i]);

                                {
                                    static sceVu0FVECTOR ofs = {3.45f, 0.0f, 0.0f, 1.0f};
                                    sceVu0ApplyMatrix(rope_ctrl[i].top, cp[rope_ctrl[i].p_num - 1].lwmtx, ofs);
                                }

                                if (rope_ctrl[i].move_mode != 0x4)
                                {
                                    SgSortUnit(tmpModelp, -1);
                                }
                            }
                        }
                        else if (acs_flg & 0xa0)
                        {
                            if (acs_flg & 0x20 && disp_frame_counter != old_frame_counter)
                            {
                                for (i = 0; i < 10; i++)
                                {
                                    if (cmove_ctrl[i].furn_id == furn_wrk[j].id)
                                    {
                                        acsChodoMove(&cmove_ctrl[i], tmpModelp);
                                        break;
                                    }
                                }
                            }

                            CalcCoordinate(cp, hs->blocks - 1);

                            if ((acs_flg & 0x80) != 0)
                            {
                                if (disp_frame_counter != old_frame_counter)
                                {
                                    for (i = 0; i < 20; i++)
                                    {
                                        if (mim_chodo[i].furn_id == furn_wrk[j].id)
                                        {
                                            mimDispChodo(&mim_chodo[i], tmpModelp);
                                            break;
                                        }
                                    }
                                }
                                else
                                {
                                    SgSortUnitKind(tmpModelp, -1);
                                }
                            }
                            else
                            {
                                SgSortUnitKind(tmpModelp, -1);
                            }
                        }
                    }

                    if (now_room == furn_wrk[j].room_id && write_coord > 0 && GetModelKind(tmpModelp) != 0)
                    {
                        AppendSearchModel(tmpModelp, furn_wrk[j].room_id);
                    }
                }
            }
        }
    }

    old_frame_counter = disp_frame_counter;
}

#include "data/girlbox.h" // static sceVu0FVECTOR girlbox[]; // 8
#include "data/enebox.h"  // static sceVu0FVECTOR enebox[]; // 8
sceVu0FMATRIX runit_mtx = {0};
LIGHT_PACK girls_light = {0};
LIGHT_PACK enemy_light = {0};
u_int *ssearch_models[15] = {NULL};
u_int *ssearch_models2[15] = {NULL};
sceVu0FVECTOR room_ambient_light = {0.0f, 0.0f, 0.0f, 0.0f};
SgLIGHT room_pararell_light[12] = {0};
SgLIGHT room_point_light[16] = {0};
SgLIGHT room_spot_light[16] = {0};

void SetWScissorBox(int disp_room)
{
    if (disp_room == 8)
    {
        SgSetWScissorBox(0, -1390.0f, 0, 0, 0, 0);
    }
    else
    {
        SgSetWScissorBox(0, 0, 0, 0, 0, 0);
    }
}

void ReSetWScissorBox()
{
    SgSetWScissorBox(0, 0, 0, 0, 0, 0);
}

void InitFogSelection()
{
    bak_cam_door_or = 0;
    locked_fog_room_no = -1;
    fog_lock = 0;
}

void FogSelection(int disp_room)
{
    u_char plyr_wrk_mode_or;
    u_char now_cam_door_or;

    plyr_wrk_mode_or = plyr_wrk.mode == 6 || plyr_wrk.mode == 1;
    now_cam_door_or = plyr_wrk.pr_info.cam_type == 3;

    if (!bak_cam_door_or && now_cam_door_or)
    {
        fog_lock = 1;
        locked_fog_room_no = GetRoomIdBeyondDoor2();
    }

    if (fog_lock != 0 && !now_cam_door_or && !plyr_wrk_mode_or)
    {
        fog_lock = 0;
    }

    if (fog_lock != 0)
    {
        disp_room = locked_fog_room_no;
    }

    if (plyr_wrk.cond == 4)
    {
        disp_room = 48;
    }

    if (plyr_wrk.mode == 1)
    {
        SgSetFog(
            fog_param_finder[disp_room][0], fog_param_finder[disp_room][1],
            fog_param_finder[disp_room][2], fog_param_finder[disp_room][3],
            fog_rgb_finder[disp_room][0], fog_rgb_finder[disp_room][1], fog_rgb_finder[disp_room][2]);
    }
    else
    {
        SgSetFog(
            fog_param[disp_room][0], fog_param[disp_room][1],
            fog_param[disp_room][2], fog_param[disp_room][3],
            fog_rgb[disp_room][0], fog_rgb[disp_room][1], fog_rgb[disp_room][2]);
    }

    bak_cam_door_or = now_cam_door_or;
}

void gra3dDraw()
{
    int i;
    int j;
    int disp_room;
    static float arad = 0.0f;
    static float adeg = 0.1f;

    disp_frame_counter++;

    MakeDebugValue();
    SgSetRefCamera(&camera);

    if (disp3d_all == 0)
    {
        return;
    }

    if (dbg_wrk.disp_3d_textrans != 0)
    {
        SgTEXTransEnable();
    }
    else
    {
        SgTEXTransDisenable();
    }

    SetEnvironment();
    CalcGirlCoord();

    disp_room = room_wrk.disp_no[0];

    if (disp_room != 0xff && room_addr_tbl[disp_room].near_sgd != NULL)
    {
        FogSelection(disp_room);
        SgReadLights(room_addr_tbl[disp_room].near_sgd, room_addr_tbl[disp_room].lit_data, room_ambient_light, room_pararell_light, 3, room_point_light, 16, room_spot_light, 16);

        map_wrk.mirror_flg = 0;
        mir_reflect_flg = 0;

        if (
            ((HeaderSection *)room_addr_tbl[disp_room].near_sgd)->kind & 2 &&
            disp3d_mirror != 0
        )
        {
            SetWScissorBox(disp_room);

            if (disp_room != 8 || !(plyr_wrk.move_box.pos[1] > -300.0f))
            {
                MirrorDraw(&camera, room_addr_tbl[disp_room].near_sgd, SceneSortUnit);

                map_wrk.mirror_flg = 1;

                CalcReflectLight();
            }

            ReSetWScissorBox();
        }

        disp_room = room_wrk.disp_no[1];

        if (disp_room != 0xff && room_addr_tbl[disp_room].near_sgd != NULL)
        {
            if (
                ((HeaderSection *)room_addr_tbl[disp_room].near_sgd)->kind & 2 &&
                disp_room != 26 && disp3d_mirror != 0
            )
            {
                MirrorDraw(&camera, room_addr_tbl[disp_room].near_sgd, SceneSortUnit);

                map_wrk.mirror_flg = 1;

                CalcReflectLight();
            }
        }
    }

    search_num2 = 0;
    search_num = 0;

    DrawRoom(-1);
    DrawFurniture(-1);
    CheckDMATrans();

    if (disp3d_2ddraw != 0)
    {
        gra2dDraw(1);
    }

    PlyrAcsAlphaCtrl();
    SetEnvironment();
    DrawGirl(0);
    DrawRoomShadow();
    CheckDMATrans();

    if (disp3d_2ddraw != 0)
    {
        gra2dDraw(2);
    }

    SetEnvironment();

    for (i = 0; i < 4; i++)
    {
        DrawEnemy(i);
    }

    for (j = 0; j < 3; j++)
    {
        DrawFlyMove(j);
    }

    CheckDMATrans();
}

int CheckModelBoundingBox(sceVu0FMATRIX lwmtx, sceVu0FVECTOR *bbox)
{
    int i;
    int clip1;
    int xmax_flg;
    int xmin_flg;
    int ymin_flg;
    int ymax_flg;
    sceVu0FVECTOR *tmpvec;
    sceVu0FVECTOR *ed;
    sceVu0FMATRIX tmpmat;
    float fog_max;

    tmpvec = (sceVu0FVECTOR *)0x70000620;
    ed = (sceVu0FVECTOR *)0x700006a0; // `ed[i]` can be replaced with `tmpvec[8+i]`

    _SetMulMatrix(SgCMVtx,lwmtx);

    if (clip_value_check != 0)
    {
        clip1 = 0x3f;

        for (i = 0; i < 8; i++)
        {
            clip1 &= BoundClipQ(ed[i], tmpvec[i], bbox[i]);
        }

        if (clip1 != 0)
        {
            return 0;
        }

        ymax_flg = 1;
        ymin_flg = 1;
        xmax_flg = 1;
        xmin_flg = 1;

        for (i = 0; i < 8; i++)
        {
            if (clip_value[0] < tmpvec[i][0])
            {
                xmin_flg = 0;
            }

            if (tmpvec[i][0] < clip_value[1])
            {
                xmax_flg = 0;
            }

            if (clip_value[2] < tmpvec[i][1])
            {
                ymin_flg = 0;
            }

            if (tmpvec[i][1] < clip_value[3])
            {
                ymax_flg = 0;
            }
        }

        if (xmin_flg | xmax_flg | ymin_flg | ymax_flg)
        {
            return 0;
        }
    }
    else
    {
        clip1 = 0x3f;

        for (i = 0; i < 8; i++)
        {
            clip1 &= BoundClip(ed[i], bbox[i]);
        }

        if (clip1 != 0)
        {
            return 0;
        }
    }

    _SetMulMatrix(SgWSMtx,lwmtx);

    Vu0ApplyVectorInline(tmpvec[0], bbox[0]);
    Vu0ApplyVectorInline(tmpvec[1], bbox[1]);
    Vu0ApplyVectorInline(tmpvec[2], bbox[2]);
    Vu0ApplyVectorInline(tmpvec[3], bbox[3]);
    Vu0ApplyVectorInline(tmpvec[4], bbox[4]);
    Vu0ApplyVectorInline(tmpvec[5], bbox[5]);
    Vu0ApplyVectorInline(tmpvec[6], bbox[6]);
    Vu0ApplyVectorInline(tmpvec[7], bbox[7]);

    fog_max = 40.0f;

    for (i = 0; i < 8; i++)
    {
        if (fog_max - fog_value[2] < fog_value[3] / tmpvec[i][3])
        {
            return 1;
        }
    }

    return 0;
}

void CalcGirlCoord()
{
    HeaderSection *hs;
    SgCOORDUNIT *cp;
    float grot;
    static float tmprot = 0.0f;
    ShadowHandle shandle;
    SgCOORDUNIT oldcoord;

    hs = (HeaderSection *)pgirlbase;
    cp = hs->coordp;

    if ((ingame_wrk.stts & 0x10 || ingame_wrk.stts & 0x80) == 0)
    {
        movGetMoveval(ani_mdl, motGetNowFrame(&ani_mdl[0].mot) / 2);

        if (ani_mdl[0].anm.playnum == 67)
        {
            motSetCoordCamera(ani_mdl);
        }
        else if (motSetCoord(ani_mdl, 0xff) != 0)
        {
            plyr_wrk.sta = plyr_wrk.sta | 0x20;
            plyr_wrk.mvsta &= ~0x200000;
        }

        mimPlyrMepatiCtrl();
        mimSetVertex(ani_mdl);
    }

    sceVu0UnitMatrix(cp->matrix);

    cp->matrix[0][0] = cp->matrix[1][1] = cp->matrix[2][2] = 25.0f / manmdl_dat[plyr_mdl_no].scale;

    grot = plyr_wrk.move_box.rot[1];
    if (PI < plyr_wrk.move_box.rot[1])
    {
        grot = plyr_wrk.move_box.rot[1] - PI2;
    }

    sceVu0RotMatrixX(cp->matrix, cp->matrix, PI);
    sceVu0RotMatrixY(cp->matrix, cp->matrix, grot);
    Vu0CopyVector(cp->matrix[3], plyr_wrk.move_box.pos);

    cp->matrix[3][3] = 1.0f;
    CalcCoordinate(cp, hs->blocks - 1);

    if (motPlayerActCtrl(cp) != 0)
    {
        plyr_wrk.sta |= 0x20;
    }

    CalcCoordinate(cp, hs->blocks - 1);
    GetMdlWaistPos(plyr_wrk.bwp, ani_mdl, plyr_mdl_no);
    GetPlyrAcsLightPos(plyr_wrk.spot_pos, ani_mdl);
}

u_int search_num = 0;
u_int search_num2 = 0;

void DrawGirl(int in_mirror)
{
    HeaderSection *hs;
    SgCOORDUNIT *cp;
    float grot;
    ShadowHandle shandle;
    sceVu0FVECTOR goffset;
    u_int i;
    sceVu0FMATRIX transmtx;
    sceVu0FVECTOR tgirlbox[8];
    u_int *dssearch_models[30];
    u_int dsearch_num;
    sceVu0FMATRIX tmpmat;
    sceVu0FVECTOR tmpvec;
    sceVu0FVECTOR tmpvec2;

    dsearch_num = 0;

    hs = (HeaderSection *)pgirlbase;
    cp = hs->coordp;

    Vu0SubVector(goffset, plyr_wrk.bwp, plyr_wrk.move_box.pos);
    sceVu0UnitMatrix(transmtx);

    transmtx[3][0] = goffset[0];
    transmtx[3][1] = 0.0f;
    transmtx[3][2] = goffset[2];

    _SetMulMatrix(transmtx, cp->matrix);

    Vu0ApplyVectorInline(tgirlbox[0], girlbox[0]);
    Vu0ApplyVectorInline(tgirlbox[1], girlbox[1]);
    Vu0ApplyVectorInline(tgirlbox[2], girlbox[2]);
    Vu0ApplyVectorInline(tgirlbox[3], girlbox[3]);
    Vu0ApplyVectorInline(tgirlbox[4], girlbox[4]);
    Vu0ApplyVectorInline(tgirlbox[5], girlbox[5]);
    Vu0ApplyVectorInline(tgirlbox[6], girlbox[6]);
    Vu0ApplyVectorInline(tgirlbox[7], girlbox[7]);

    sceVu0UnitMatrix(transmtx);

    if (((plyr_wrk.sta & 0x10) == 0 || in_mirror != 0) && CheckModelBoundingBox(transmtx, tgirlbox) != 0)
    {

        SetMyLightObj(&girls_light, &plyr_wrk.mylight, camera.zd, plyr_wrk.bwp);

        if (disp3d_girl == 0)
        {
            return;
        }

        ManTexflush();
        SgSortUnitKind(hs, -1);
        DrawGirlSubObj(ani_mdl[0].mpk_p, 0x7f);

        if (in_mirror != 0 || plyr_wrk.mode != 1)
        {

            DispPlyrAcs(pgirlbase, pgirlacs[0], &plyracs_ctrl[0], 6);
        }

        DispPlyrAcs(pgirlbase, pgirlacs[1], &plyracs_ctrl[1], 5);
    }

    if (disp3d_girl == 0 || plyr_wrk.sta & 0x10 || in_mirror != 0 || (plyr_wrk.sta & 0x1 && map_wrk.mirror_flg != 0x0))
    {
        return;
    }

    if (room_wrk.disp_no[0] == 0x18 || (room_wrk.disp_no[0] == 0x3 && plyr_wrk.move_box.pos[1] <= -1200.0f))
    {

        SetShadowAssignGroup(1);
    }
    else
    {
        SetShadowAssignGroup(-1);
    }

    for (i = 0; i < search_num; i++)
    {
        dssearch_models[i] = ssearch_models[i];
        dsearch_num++;
    }

    for (i = 0; i < search_num2; i++)
    {
        dssearch_models[i + search_num] = ssearch_models2[i];
        dsearch_num++;
    }

    shandle.shadow_model = pgirlshadow;
    shandle.smodel_num = -1;
    shandle.search_model = (void **)dssearch_models;
    shandle.bbox = girlbox;
    shandle.search_num = dsearch_num;
    shandle.source_model = NULL;

    if (plyr_wrk.shadow_direction[3] == 1.0f)
    {
        Vu0CopyVector(shandle.direction, plyr_wrk.shadow_direction);
    }
    else
    {
        grot = plyr_wrk.move_box.rot[1];
        if (PI < plyr_wrk.move_box.rot[1])
        {
            grot = plyr_wrk.move_box.rot[1] - PI2;
        }

        sceVu0UnitMatrix(tmpmat);
        sceVu0RotMatrixY(tmpmat, tmpmat, grot);

        tmpvec[0] = 0.0f;
        tmpvec[1] = -2.0f;
        tmpvec[2] = 1.0f;
        tmpvec[3] = 1.0f;

        sceVu0ApplyMatrix(tmpvec, tmpmat, tmpvec);
        sceVu0ScaleVector(tmpvec2, room_wrk.mylight[0].spot[0].direction, -1.0f);

        _NormalizeVector(tmpvec2, tmpvec2);

        sceVu0ScaleVector(tmpvec, tmpvec, 0.8f);
        sceVu0ScaleVector(tmpvec2, tmpvec2, 0.2f);
        sceVu0AddVector(tmpvec, tmpvec, tmpvec2);

        _NormalizeVector(tmpvec, tmpvec, tmpvec); // wrong number of args
        tmpvec[1] = -1.0f;

        Vu0CopyVector(shandle.direction, tmpvec);
    }

    shandle.color[0] = 0x80;
    shandle.color[1] = 0x80;
    shandle.color[2] = 0x80;
    shandle.color[3] = 0x40;
    shandle.camera = nowcamera;

    DrawShadow(&shandle, SetEnvironment);

    nowcamera = shandle.camera;

    SetEnvironment();
    SetShadowAssignGroup(-1);
}

int DrawEnemy(int no)
{
    int i;
    int j;
    u_int *tmpModelp;
    SgCOORDUNIT *cp;
    float grot;
    float scale;
    u_int mdl_no;
    sceVu0FVECTOR ebox[8];
    MANMDL_DAT *mdat;
    ANI_CTRL *ani_ctrl;
    sceVu0FVECTOR offset;
    static int old_frame_counter[4];
    HeaderSection *hs2;
    SgCOORDUNIT *cp2;
    sceVu0FVECTOR vec;
    sceVu0FMATRIX m;
    HeaderSection *hs;

    j = no;

    if (ene_display[j] == 0)
    {
        return 0;
    }

    mdl_no = ene_wrk[j].dat->mdl_no;

    ani_ctrl = motGetAniMdl(j);

    tmpModelp = ani_ctrl->base_p;

    if (
        disp_frame_counter != old_frame_counter[j] &&
        (ingame_wrk.stts & 0x10) == 0 &&
        (ingame_wrk.stts & 0x80) == 0
    )
    {
        ani_ctrl->mot.reso = ene_wrk[j].ani_reso;

        if (ani_ctrl->tanm_p != 0)
        {
            motEneTexAnm(ani_ctrl, mdl_no);
        }

        if (motSetCoord(ani_ctrl, j) != 0)
        {
            ene_wrk[j].sta |= 0x40000000;
        }

        mimSetVertex(ani_ctrl);
    }

    if (tmpModelp == 0)
    {
        return 0;
    }

    mdat = &manmdl_dat[mdl_no];

    hs = (HeaderSection *)tmpModelp;
    cp = hs->coordp;
    scale = mdat->scale;

    if (ene_child_ctrl[j].flg != 0)
    {
        int k = ene_child_ctrl[j].bone_id;

        sceVu0UnitMatrix(m);

        m[0][0] = 25.0f / scale;
        m[1][1] = 25.0f / scale;
        m[2][2] = 25.0f / scale;

        hs2 = ((HeaderSection *)pgirlbase);
        cp2 = hs2->coordp;

        sceVu0UnitMatrix(cp->matrix);
        sceVu0Normalize(cp->matrix[0], cp2[k].lwmtx[0]);
        sceVu0Normalize(cp->matrix[1], cp2[k].lwmtx[1]);
        sceVu0Normalize(cp->matrix[2], cp2[k].lwmtx[2]);
        sceVu0MulMatrix(cp->matrix, cp->matrix, m);
        sceVu0CopyVector(cp->matrix[3], cp2[k].lwmtx[3]);

        LocalRotMatrixZ(cp->matrix, cp->matrix, -PI_HALF);
        LocalRotMatrixY(cp->matrix, cp->matrix, ene_child_ctrl[j].ry);
        sceVu0Normalize(vec, cp->matrix[2]);
        sceVu0ScaleVector(vec, vec, ene_child_ctrl[j].r);
        sceVu0TransMatrix(cp->matrix, cp->matrix, vec);
    }
    else
    {
        sceVu0UnitMatrix(cp->matrix);

        cp->matrix[0][0] = 25.0f / scale;
        cp->matrix[1][1] = 25.0f / scale;
        cp->matrix[2][2] = 25.0f / scale;

        sceVu0RotMatrixX(cp->matrix, cp->matrix, PI);

        grot = ene_wrk[j].move_box.rot[1];
        if (PI < grot)
        {
            grot -= PI2;
        }

        sceVu0RotMatrixY(cp->matrix, cp->matrix, grot);

        Vu0CopyVector(cp->matrix[3], ene_wrk[j].move_box.pos);
        cp->matrix[3][3] = 1.0f;
    }

    for (i = 0; i < 8; i++)
    {
        sceVu0ScaleVector(ebox[i], enebox[i], mdat->scale * 40.0f);
        ebox[i][3] = 1.0f;
    }

    CalcCoordinate(cp, hs->blocks - 1);

    GetMdlNeckPos(ene_wrk[j].mpos.p0, ani_ctrl, mdl_no);
    GetMdlWaistPos(ene_wrk[j].mpos.p1, ani_ctrl, mdl_no);
    GetMdlShldPos(ene_wrk[j].mpos.p2,ani_ctrl, 0);
    GetMdlShldPos(ene_wrk[j].mpos.p3,ani_ctrl, 1);
    GetMdlLegPos(ene_wrk[j].mpos.p4,ani_ctrl,mdl_no);

    if (mdl_no == 0x25)
    {
        GetToushuKatanaPos(sword_line[0], sword_line[1], ani_ctrl);
        SetSwordLine();
    }

    offset[0] = 0;
    offset[1] = 0;
    offset[2] = 0;
    offset[3] = 0;

    if (CheckModelBoundingBox(cp[manmdl_dat[mdl_no].waist_id].lwmtx, ebox))
    {
        SetMyLightObj(&enemy_light, &ene_wrk[j].mylight, camera.zd, ene_wrk[j].mpos.p0);

        if(disp3d_enemy != 0)
        {
            ManmdlSetAlpha(tmpModelp, ene_wrk[j].tr_rate);
            ManTexflush();
            SgSortUnitKind(tmpModelp,-1);
            acsClothCtrl(ani_ctrl,ani_ctrl->mpk_p,mdl_no, 0);

            if (motCheckTrRateMdl(mdl_no) != 0)
            {
                DrawEneSubObj(ani_ctrl->mpk_p, ene_wrk[j].tr_rate, ene_wrk[j].tr_rate2);
            }
            else
            {
                DrawEneSubObj(ani_ctrl->mpk_p, ene_wrk[j].tr_rate, ene_wrk[j].tr_rate);
            }
        }
    }

    old_frame_counter[j] = disp_frame_counter;

    return 1;
}

int DrawFlyMove(int work_no)
{
    SgCOORDUNIT *cp;
    u_int *tmpModelp;
    u_int no;
    float grot;
    HeaderSection *hs;


    if (fly_display[work_no] == 0)
    {
        return 0;
    }

    no = fly_wrk[work_no].dat->mdl_no;
    tmpModelp = pmanmodel[no];

    if (tmpModelp == NULL)
    {
        return 0;
    }

    hs = (HeaderSection *)tmpModelp;
    cp = hs->coordp;

    sceVu0UnitMatrix(cp->matrix);
    sceVu0RotMatrixX(cp->matrix, runit_mtx, PI);

    grot = fly_wrk[work_no].move_box.rot[1];
    if (PI < grot)
    {
        grot = grot - PI2;
    }

    sceVu0RotMatrixY(cp->matrix, cp->matrix, grot);
    LocalRotMatrixY(cp->matrix, cp->matrix, PI);
    Vu0CopyVector(cp->matrix[3], fly_wrk[work_no].move_box.pos);

    cp->matrix[3][3] = 1.0f;
    CalcCoordinate(cp, hs->blocks - 1);
    ManmdlSetAlpha(tmpModelp, ene_wrk[fly_wrk[work_no].ene].tr_rate);
    SetMyLightObj(&enemy_light, &ene_wrk[fly_wrk[work_no].ene].mylight, camera.zd, ene_wrk[fly_wrk[work_no].ene].mpos.p0);
    SgSortUnitKind(tmpModelp, -1);

    return 1;
}
