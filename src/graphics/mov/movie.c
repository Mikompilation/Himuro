#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "movie.h"

#include "gcc/ctype.h"
#include "gcc/index.h"

#include "ee/eekernel.h"
#include "ee/eeregs.h"
#include "ee/eestruct.h"
#include "ee/kernel.h"

#include "sce/libdma.h"
#include "sce/libgraph.h"
#include "sce/sif.h"
#include "sce/sifdev.h"
#include "sce/libgifpk.h"
#include "sce/libcdvd.h"
#include "sce/libsdr.h"
#include "sce/sdmacro.h"
#include "sce/sdrcmd.h"
#include "sce/libpad.h"
#include "sce/libpc.h"
#include "sce/misc/diei.h"
#include "sce/mpeg/libmpeg.h"
#include "sce/mpeg/readbuf.h"
#include "sce/mpeg/videodec.h"
#include "sce/mpeg/audiodec.h"

#include "os/pad.h"
#include "main/glob.h"
#include "os/system.h"
#include "os/eeiop/eeiop.h"
#include "os/eeiop/adpcm/ea_ctrl.h"
#include "os/eeiop/eese.h"
#include "graphics/graph3d/sglib.h"
// #include "ingame/map/map_area.h"
#include "graphics/scene/scene_dat.h"
#include "ingame/map/map_area.h"
#include "graphics/scene/scene.h"

#ifdef BUILD_EU_VERSION
char *mpegName[][40] = {
      {
            "cdrom0:\\MOVIE3\\SCN0010P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN0031P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN1000P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN1031P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN1101P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN1240P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN1300P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN1331P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN1332P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN2010P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN2050P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN2061P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN2071P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN2091P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN2110P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN2131P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN2142P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN2143P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN2171P.PSS;1",
            "cdrom0:\\MOVIE4\\SCN3010P.PSS;1",
            "cdrom0:\\MOVIE4\\SCN3040P.PSS;1",
            "cdrom0:\\MOVIE4\\SCN3080P.PSS;1",
            "cdrom0:\\MOVIE4\\SCN3081P.PSS;1",
            "cdrom0:\\MOVIE4\\SCN3090P.PSS;1",
            "cdrom0:\\MOVIE4\\SCN4010P.PSS;1",
            "cdrom0:\\MOVIE4\\SCN4031P.PSS;1",
            "cdrom0:\\MOVIE4\\SCN4041P.PSS;1",
            "cdrom0:\\MOVIE4\\SCN4060P.PSS;1",
            "cdrom0:\\MOVIE4\\SCN4080P.PSS;1",
            "cdrom0:\\MOVIE4\\SCN4090P.PSS;1",
            "cdrom0:\\MOVIE4\\SCN4100P.PSS;1",
            "cdrom0:\\MOVIE4\\SCN4110P.PSS;1",
            "cdrom0:\\MOVIE4\\SCN4120P.PSS;1",
            "cdrom0:\\MOVIE4\\SCN5010P.PSS;1",
            "cdrom0:\\MOVIE4\\SCN5020P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN9000P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN9001P.PSS;1",
            "cdrom0:\\MOVIE3\\TECMOP.PSS;1",
            "cdrom0:\\MOVIE3\\SCN9100P.PSS;1",
            "cdrom0:\\MOVIE3\\SCN9200P.PSS;1",
      },
      {
            "cdrom0:\\MOVIE\\SCN0010.PSS;1",
            "cdrom0:\\MOVIE\\SCN0031.PSS;1",
            "cdrom0:\\MOVIE\\SCN1000.PSS;1",
            "cdrom0:\\MOVIE\\SCN1031.PSS;1",
            "cdrom0:\\MOVIE\\SCN1101.PSS;1",
            "cdrom0:\\MOVIE\\SCN1240.PSS;1",
            "cdrom0:\\MOVIE\\SCN1300.PSS;1",
            "cdrom0:\\MOVIE\\SCN1331.PSS;1",
            "cdrom0:\\MOVIE\\SCN1332.PSS;1",
            "cdrom0:\\MOVIE\\SCN2010.PSS;1",
            "cdrom0:\\MOVIE\\SCN2050.PSS;1",
            "cdrom0:\\MOVIE\\SCN2061.PSS;1",
            "cdrom0:\\MOVIE\\SCN2071.PSS;1",
            "cdrom0:\\MOVIE\\SCN2091.PSS;1",
            "cdrom0:\\MOVIE\\SCN2110.PSS;1",
            "cdrom0:\\MOVIE\\SCN2131.PSS;1",
            "cdrom0:\\MOVIE\\SCN2142.PSS;1",
            "cdrom0:\\MOVIE\\SCN2143.PSS;1",
            "cdrom0:\\MOVIE\\SCN2171.PSS;1",
            "cdrom0:\\MOVIE2\\SCN3010.PSS;1",
            "cdrom0:\\MOVIE2\\SCN3040.PSS;1",
            "cdrom0:\\MOVIE2\\SCN3080.PSS;1",
            "cdrom0:\\MOVIE2\\SCN3081.PSS;1",
            "cdrom0:\\MOVIE2\\SCN3090.PSS;1",
            "cdrom0:\\MOVIE2\\SCN4010.PSS;1",
            "cdrom0:\\MOVIE2\\SCN4031.PSS;1",
            "cdrom0:\\MOVIE2\\SCN4041.PSS;1",
            "cdrom0:\\MOVIE2\\SCN4060.PSS;1",
            "cdrom0:\\MOVIE2\\SCN4080.PSS;1",
            "cdrom0:\\MOVIE2\\SCN4090.PSS;1",
            "cdrom0:\\MOVIE2\\SCN4100.PSS;1",
            "cdrom0:\\MOVIE2\\SCN4110.PSS;1",
            "cdrom0:\\MOVIE2\\SCN4120.PSS;1",
            "cdrom0:\\MOVIE2\\SCN5010.PSS;1",
            "cdrom0:\\MOVIE2\\SCN5020.PSS;1",
            "cdrom0:\\MOVIE\\SCN9000.PSS;1",
            "cdrom0:\\MOVIE\\SCN9001.PSS;1",
            "cdrom0:\\MOVIE\\TECMO.PSS;1",
            "cdrom0:\\MOVIE\\SCN9100.PSS;1",
            "cdrom0:\\MOVIE\\SCN9200.PSS;1",
      }
};
char *mpegStaff[][5] = {
      {
            "cdrom0:\\MOVIE5\\SCN900EP.PSS;1",
            "cdrom0:\\MOVIE5\\SCN900FP.PSS;1",
            "cdrom0:\\MOVIE5\\SCN900GP.PSS;1",
            "cdrom0:\\MOVIE5\\SCN900SP.PSS;1",
            "cdrom0:\\MOVIE5\\SCN900IP.PSS;1",
      },
      {
            "cdrom0:\\MOVIE5\\SCN900E.PSS;1",
            "cdrom0:\\MOVIE5\\SCN900F.PSS;1",
            "cdrom0:\\MOVIE5\\SCN900G.PSS;1",
            "cdrom0:\\MOVIE5\\SCN900S.PSS;1",
            "cdrom0:\\MOVIE5\\SCN900I.PSS;1",
      }
};
#else
char *mpegName[] = {
    "cdrom0:\\MOVIE\\SCN0010.PSS;1",
    "cdrom0:\\MOVIE\\SCN0031.PSS;1",
    "cdrom0:\\MOVIE\\SCN1000.PSS;1",
    "cdrom0:\\MOVIE\\SCN1031.PSS;1",
    "cdrom0:\\MOVIE\\SCN1101.PSS;1",
    "cdrom0:\\MOVIE\\SCN1240.PSS;1",
    "cdrom0:\\MOVIE\\SCN1300.PSS;1",
    "cdrom0:\\MOVIE\\SCN1331.PSS;1",
    "cdrom0:\\MOVIE\\SCN1332.PSS;1",
    "cdrom0:\\MOVIE\\SCN2010.PSS;1",
    "cdrom0:\\MOVIE\\SCN2050.PSS;1",
    "cdrom0:\\MOVIE\\SCN2061.PSS;1",
    "cdrom0:\\MOVIE\\SCN2071.PSS;1",
    "cdrom0:\\MOVIE\\SCN2091.PSS;1",
    "cdrom0:\\MOVIE\\SCN2110.PSS;1",
    "cdrom0:\\MOVIE\\SCN2131.PSS;1",
    "cdrom0:\\MOVIE\\SCN2142.PSS;1",
    "cdrom0:\\MOVIE\\SCN2143.PSS;1",
    "cdrom0:\\MOVIE\\SCN2171.PSS;1",
    "cdrom0:\\MOVIE2\\SCN3010.PSS;1",
    "cdrom0:\\MOVIE2\\SCN3040.PSS;1",
    "cdrom0:\\MOVIE2\\SCN3080.PSS;1",
    "cdrom0:\\MOVIE2\\SCN3081.PSS;1",
    "cdrom0:\\MOVIE2\\SCN3090.PSS;1",
    "cdrom0:\\MOVIE2\\SCN4010.PSS;1",
    "cdrom0:\\MOVIE2\\SCN4031.PSS;1",
    "cdrom0:\\MOVIE2\\SCN4041.PSS;1",
    "cdrom0:\\MOVIE2\\SCN4060.PSS;1",
    "cdrom0:\\MOVIE2\\SCN4080.PSS;1",
    "cdrom0:\\MOVIE2\\SCN4090.PSS;1",
    "cdrom0:\\MOVIE2\\SCN4100.PSS;1",
    "cdrom0:\\MOVIE2\\SCN4110.PSS;1",
    "cdrom0:\\MOVIE2\\SCN4120.PSS;1",
    "cdrom0:\\MOVIE2\\SCN5010.PSS;1",
    "cdrom0:\\MOVIE2\\SCN5020.PSS;1",
    "cdrom0:\\MOVIE\\SCN9000.PSS;1",
    "cdrom0:\\MOVIE\\SCN9001.PSS;1",
    "cdrom0:\\MOVIE\\TECMO.PSS;1",
    "cdrom0:\\MOVIE\\SCN9100.PSS;1",
    "cdrom0:\\MOVIE\\SCN9200.PSS;1",
};
#endif
u_char mpeg_vol_rate[] = {
    95,  90,  75,  90,  100, 100, 100, 90,  100, 100, 90, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 90,  90, 90,  100, 80,
    100, 100, 100, 100, 100, 100, 100, 100, 85,  90,  85, 70
};
VoBuf voBuf = {0};
StrFile infile = {0};
VideoDec videoDec = {0};
AudioDec audioDec = {0};
sceGsDBuff db = {0};

static VoData *voBufData = (VoData *)0x00470100;
static VoTag *voBufTag = (VoTag *)0x006d8100;
ReadBuf *readBuf = (ReadBuf *)0x00420000;
static u_char *audioBuff = (u_char *)0x006c8100;
static char *videoDecStack = (u_char *)0x006d4100;
u_int scene_bg_color = 0;
int isWithAudio = 1;
char *commandname = NULL;
char *VERSION = "1.2";

static u_char *mpegWork;
static u_long128 *viBufData;

static u_long128 viBufTag[257];
static TimeStamp timeStamp[512];
static char _0_buf[2048] __attribute__ ((aligned(64))) __attribute__ ((section (".bss")));

#define min(x, y) (((x) > (y))? (y): (x))
#define max(x, y) (((x) < (y))? (y): (x))

typedef struct {
    int x;
    int y;
    int w;
    int h;
} Rect;

static int readMpeg(VideoDec *vd, ReadBuf *rb, StrFile *file);
static int isAudioOK();
static int readMpeg(VideoDec *vd, ReadBuf *rb, StrFile *file) ;
static int isAudioOK();
static void termMov();
static void usage();
static void iopGetArea(int *pd0, int *d0, int *pd1, int *d1, AudioDec *ad, int pos);
static int sendToIOP2area(int pd0, int d0, int pd1, int d1, u_char *ps0, int s0, u_char *ps1, int s1);
static int sendToIOP(int dst, u_char *src, int size);
static void changeMasterVolume(u_int val);
static void changeInputVolume(u_int val);
static int copy2area(u_char *pd0, int d0, u_char *pd1, int d1, u_char *ps0, int s0, u_char *ps1, int s1);
static int cpy2area(u_char *pd0, int d0, u_char *pd1, int d1, u_char *ps0, int s0, u_char *ps1, int s1);

#define IOP_BUFF_SIZE (12288*2)
#define STACK_SIZE (16*1024)

void MovieInitWrk(void)

{
    movie_wrk.play_event_no = 0;
    movie_wrk.play_event_sta = 0;
}

void ReqMpegEvent(int no)
{
    movie_wrk.play_event_no = no;
    movie_wrk.play_event_sta = 1;
}

int PlayMpegEvent()
{
    int i;
    int ret;

    play_mov_no = 0;

    switch(movie_wrk.play_event_sta)
    {
        case 0:
            ret = 0;
        break;
        case 1:
            if (checkIOP() == 0)
            {
                AdpcmShiftMovie();
                SeStopAll();

                movie_wrk.play_event_sta = 2;
            }

            ret = 0;
        break;
        case 2:
            movie_wrk.play_event_sta = 3;

            ret = 0;
        break;
        case 3:
            for (i = 0; i < 99; i++ )
            {
                u_char *smn = scene_movie_no; // HACK: regswap fix
                if (scene_movie_no[i] == movie_wrk.play_event_no)
                {
                    play_mov_no = i;
                    break;
                }
                smn = scene_movie_no; // HACK: regswap fix
            }

            ClearDispRoom(1);

#ifdef BUILD_EU_VERSION
            movie(mpegName[sys_wrk.pal_disp_mode][play_mov_no]);
#else
            movie(mpegName[play_mov_no]);
#endif
            SetIopCmdSm(1, 1, 0, 0);
            SeSetMVol(opt_wrk.bgm_vol);
            SeSetSteMono(opt_wrk.sound_mode);

            movie_wrk.play_event_sta = 4;
        case 4:
#ifdef BUILD_EU_VERSION
            sceGsResetGraph(1, SCE_GS_INTERLACE, sys_wrk.pal_disp_mode == 0 ? SCE_GS_PAL: SCE_GS_NTSC, SCE_GS_FRAME);
#else
            sceGsResetGraph(1, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
#endif
            sceGsSetDefDBuff(&g_db, 0, 0x280, 0xe0, 2, 0x31, 1);

            pdrawenv = &g_db.draw0;

#ifdef BUILD_EU_VERSION
            if (sys_wrk.pal_disp_mode == 0)
            {
                g_db.disp[1].display.DX = 656;
                g_db.disp[0].display.DX = 656;
                g_db.disp[1].display.DY = 104;
                g_db.disp[0].display.DY = 104;
            }
            else
            {
                g_db.disp[1].display.DX = 636;
                g_db.disp[0].display.DX = 636;
                g_db.disp[1].display.DY = 50;
                g_db.disp[0].display.DY = 50;
            }
#endif

            sceGsSyncPath(0, 0);
            SgInit3D();
            sceGsSyncPath(0, 0);

            vfunc();

            if (scene_bg_color == 0)
            {
                SetSysBackColor(0, 0, 0);
                clearGsMem(0, 0, 0, 0x280, 0x1c0);
            }
            else
            {
                SetSysBackColor(0xff, 0xff, 0xff);
                clearGsMem(0xff, 0xff, 0xff, 0x280, 0x1c0);
            }

            AdpcmReturnFromMovie();
            EiMain();


            *(int *)REG_DMAC_CTRL &= ~D_CTRL_RELE_M; // yeah ...

            MovieInitWrk();

            ret = 0;
        break;
    }

    return ret;
}

u_int movie(char *name)
{
    static int count = 0;

    sceGsSyncPath(0, 0);
    sceGsResetPath();
    sceDmaReset(1);
#ifdef BUILD_EU_VERSION
            sceGsResetGraph(1, SCE_GS_INTERLACE, sys_wrk.pal_disp_mode == 0 ? SCE_GS_PAL: SCE_GS_NTSC, SCE_GS_FRAME);
#else
            sceGsResetGraph(1, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
#endif
    if (scene_bg_color == 0)
    {
        clearGsMem(0, 0, 0, 0x280, 0x1c0);
    }
    else
    {
        clearGsMem(0xff, 0xff, 0xff, 0x280, 0x1c0);
    }

#ifdef BUILD_EU_VERSION
    SendFontTex();
#endif

    sceGsSetDefDBuff(&db, 0, 0x280, 0xe0, 0, 0, 1);

#ifdef BUILD_EU_VERSION
    if (sys_wrk.pal_disp_mode == 0)
    {
        db.disp[1].display.DX = 656;
        db.disp[0].display.DX = 656;
        db.disp[1].display.DY = 104;
        db.disp[0].display.DY = 104;
    }
    else
    {
        db.disp[1].display.DX = 636;
        db.disp[0].display.DX = 636;
        db.disp[1].display.DY = 50;
        db.disp[0].display.DY = 50;
    }
#endif

    FlushCache(0);

    thread_id = GetThreadId();

    ChangeThreadPriority(thread_id, 1);

    initMov(name);
    readMpeg(&videoDec,readBuf,(StrFile *)&infile);
    termMov();

    ChangeThreadPriority(GetThreadId(), thread_id);

    return controller_val;
}

volatile int isCountVblank = 0;
volatile int vblankCount = 0;
volatile int isFrameEnd = 0;
volatile int oddeven = 0;
volatile int handler_error = 0;
int isStrFileInit = 0;

void switchThread()
{
    RotateThreadReadyQueue(1);
}

static int readMpeg(VideoDec *vd, ReadBuf *rb, StrFile *file)
{
    u_char cdata[32];
    int isStarted;
    u_char *put_ptr;
    u_char *get_ptr;
    int putsize;
    int getsize;
    int readrest;
    int writerest;
    int count;
    int proceed;
    int isPaused;

    isStarted = 0;
    isPaused = 0;
    readrest = file->size;
    writerest = file->size;

    while (isPaused || (writerest > 4 && videoDecGetState(vd) != VD_STATE_END))
    {
#ifdef BUILD_EU_VERSION
        if (sys_wrk.pal_disp_mode == 0)
        {
            SceneSetVibrate(movie_wrk.play_event_no, (vd->mpeg.frameCount * 6) / 5);
        }
        else
        {
            SceneSetVibrate(movie_wrk.play_event_no, vd->mpeg.frameCount);
        }
#else
        SceneSetVibrate(movie_wrk.play_event_no, vd->mpeg.frameCount);
#endif
        movVblankPad();

        if (*key_now[12] != 0 && vd->mpeg.frameCount >= 31 && movie_wrk.play_event_sta != 7)
        {
            videoDec.state = 1;
        }

        putsize = readBufBeginPut(rb, &put_ptr);
        if (readrest > 0 && putsize >= READ_UNIT_SIZE)
        {
            count = strFileRead(file, put_ptr, READ_UNIT_SIZE);
            readBufEndPut(rb, count);
            readrest -= count;
        }

        switchThread();

        getsize = readBufBeginGet(rb, &get_ptr);
        if (getsize > 0)
        {
            proceed = sceMpegDemuxPssRing(&vd->mpeg, get_ptr, getsize, rb->data, rb->size);
            readBufEndGet(rb, proceed);
            writerest -= proceed;
        }

        proceedAudio();

        if (!isStarted && voBufIsFull(&voBuf) && isAudioOK())
        {
            startDisplay(1);

            if (isWithAudio)
            {
                audioDecStart(&audioDec);
            }

            isStarted = 1;
        }
    }

    while (videoDecFlush(vd) == 0)
    {
        switchThread();
    }

    while (videoDecIsFlushed(vd) == 0 && videoDecGetState(vd) != VD_STATE_END)
    {
        switchThread();
    }

    endDisplay();

    if (isWithAudio != 0)
    {
        audioDecReset(&audioDec);
    }

    return 1;
}

static int isAudioOK()
{
    return (isWithAudio)? audioDecIsPreset(&audioDec): 1;
}

void initMov(char *bsfilename)
{
    ThreadParam th_param;
    void *val;

    *REG_DMAC_CTRL |= 3;
    *REG_DMAC_STAT = 4;

    scePcStop();

    if (movie_wrk.play_event_sta == 6 || movie_wrk.play_event_sta == 7)
    {
        mpegWork = (u_char *)0x14b0000;
    }
    else
    {
        mpegWork = (u_char *)GetEmptyRoomAddr();
    }

    val = mpegWork;
    viBufData = (val + (SCE_MPEG_BUFFER_SIZE(MPEG_DISP_WIDTH, MPEG_DISP_HEIGHT) - 8192));

    readBufCreate(readBuf);
    sceMpegInit();

    videoDecCreate(&videoDec, mpegWork, (SCE_MPEG_BUFFER_SIZE(MAX_WIDTH, MAX_HEIGHT) - 8192), viBufData, viBufTag, VIBUF_SIZE, timeStamp, VIBUF_TS_SIZE);

    sceSdRemoteInit();
    sceSdRemote(1, rSdInit, SD_INIT_COLD);

    audioDecCreate(&audioDec, audioBuff, 0xc000, IOP_BUFF_SIZE);
    videoDecSetStream(&videoDec, 0, 0, (sceMpegCallback)videoCallback, readBuf);

    if (isWithAudio != 0)
    {
        videoDecSetStream(&videoDec, 2, 0, (sceMpegCallback)pcmCallback, readBuf);
    }

    voBufCreate(&voBuf, UncAddr(voBufData), voBufTag, N_GSBUF);

    th_param.entry = videoDecMain;
    th_param.stack = videoDecStack;
    th_param.stackSize = STACK_SIZE;
    th_param.initPriority = 1;
    th_param.gpReg = &_gp;
    th_param.option = 0;

    videoDecTh = CreateThread(&th_param);
    StartThread(videoDecTh, &videoDec);

    while (!strFileOpen(&infile, bsfilename)) {}

    sceGsSyncVCallback(vblankHandlerM);

    videoDec.hid_endimage = AddDmacHandler(DMAC_GIF, handler_endimage, 0);

    EnableDmac(2);

    vblankCount = 0;
    scn_vib_time1 = 0;
    scn_vib_time0 = 0;
}

static void termMov()
{
    sceGsGParam *gparam;
    u_long UserIMR;

    readBufDelete(readBuf);
    voBufDelete(&voBuf);

    TerminateThread(videoDecTh);
    DeleteThread(videoDecTh);

    DisableDmac(2);
    RemoveDmacHandler(2,videoDec.hid_endimage);

    videoDecDelete(&videoDec);
    audioDecDelete(&audioDec);

    strFileClose(&infile);

    UserIMR = sceGsPutIMR(0xff00);
    gparam = sceGsGetGParam();

    DisableIntc(2);
    RemoveIntcHandler(2, gparam->sceGsVSCid);

    gparam->sceGsVSCfunc = NULL;
    gparam->sceGsVSCid = 0;

    sceGsPutIMR(UserIMR);
}

void defMain(void)
{
    while(1)
    {
       switchThread();
    }
}

void ErrMessage(char *message)
{
    return;
}

static void usage()
{
    return;
}

void proceedAudio()
{
    audioDecSendToIOP(&audioDec);
}

int MoviePlay(int scene_no)
{
    int i;

    SetSysBackColor(0, 0, 0);
    clearGsMem(0, 0, 0, DISP_WIDTH, DISP_HEIGHT);

    while (checkIOP() != 0)
    {
        vfunc();
    }

    AdpcmShiftMovie();
    SeStopAll();
    vfunc();

    do
    {
        movie_wrk.play_event_no = scene_no;
        movie_wrk.play_event_sta = 0x2 | 0x4;

        for (i = 0; i < 99; i++)
        {
            u_char *smn = scene_movie_no; // HACK: regswap fix
            if (scene_movie_no[i] == movie_wrk.play_event_no)
            {
                play_mov_no = i;
                break;
            }
            smn = scene_movie_no; // HACK: regswap fix
        }
    } while (0);

    if (scene_no != 0 && scene_no != 0x62 && scene_no != 99)
    {
        ClearDispRoom(1);
    }

#ifdef BUILD_EU_VERSION
    if (scene_no == 0x60)
    {
        movie(mpegStaff[sys_wrk.pal_disp_mode][sys_wrk.language]);
    }
    else
    {
        movie(mpegName[sys_wrk.pal_disp_mode][play_mov_no]);
    }
#else
    movie(mpegName[play_mov_no]);
#endif

    SetIopCmdSm(1, 1, 0, 0);
    SeSetMVol(opt_wrk.bgm_vol);
    SeSetSteMono(opt_wrk.sound_mode);
#ifdef BUILD_EU_VERSION
    sceGsResetGraph(1, SCE_GS_INTERLACE, sys_wrk.pal_disp_mode == 0 ? SCE_GS_PAL: SCE_GS_NTSC, SCE_GS_FRAME);
#else
    sceGsResetGraph(1, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
#endif
    sceGsSetDefDBuff(&g_db, SCE_GS_PSMCT32, DISP_WIDTH, (DISP_HEIGHT/2), 2, 0x31, SCE_GS_CLEAR);

    pdrawenv = &g_db.draw0;

#ifdef BUILD_EU_VERSION
    if (sys_wrk.pal_disp_mode == 0)
    {
        g_db.disp[1].display.DX = 656;
        g_db.disp[0].display.DX = 656;
        g_db.disp[1].display.DY = 104;
        g_db.disp[0].display.DY = 104;
    }
    else
    {
        g_db.disp[1].display.DX = 636;
        g_db.disp[0].display.DX = 636;
        g_db.disp[1].display.DY = 50;
        g_db.disp[0].display.DY = 50;
    }
#endif

    sceGsSyncPath(0, 0);
    SgInit3D();
    sceGsSyncPath(0, 0);

    if (scene_no != 0x5f)
    {
        SetSysBackColor(0x00, 0x00, 0x00);
        clearGsMem(0x00, 0x00, 0x00, DISP_WIDTH, DISP_HEIGHT);
    }
    else
    {
        SetSysBackColor(0xff, 0xff, 0xff);
        clearGsMem(0xff, 0xff, 0xff, DISP_WIDTH, DISP_HEIGHT);
    }

    vfunc();
    AdpcmReturnFromMovie();
    EiMain();

    *(int *)REG_DMAC_CTRL &= ~D_CTRL_RELE_M; // yeah ...

    MovieInitWrk();

    return 0;
}

void MovieTest(int scene_no)
{
    int i;

    movie_wrk.play_event_no = scene_no;
    movie_wrk.play_event_sta = 0x2 | 0x4;

    for (i = 0; i < 99; i++)
    {
        u_char *smn = scene_movie_no; // HACK: regswap fix
        if (scene_movie_no[i] == movie_wrk.play_event_no)
        {
            play_mov_no = i;
            break;
        }
        smn = scene_movie_no; // HACK: regswap fix
    }

#ifdef BUILD_EU_VERSION
    if (scene_no == 0x60)
    {
        movie(mpegStaff[sys_wrk.pal_disp_mode][sys_wrk.language]);
    }
    else
    {
        movie(mpegName[sys_wrk.pal_disp_mode][play_mov_no]);
    }
#else
    movie(mpegName[play_mov_no]);
#endif

    SetIopCmdSm(1, 1, 0, 0);
    SeSetMVol(opt_wrk.bgm_vol);
    SeSetSteMono(opt_wrk.sound_mode);
#ifdef BUILD_EU_VERSION
    sceGsResetGraph(1, SCE_GS_INTERLACE, sys_wrk.pal_disp_mode == 0 ? SCE_GS_PAL: SCE_GS_NTSC, SCE_GS_FRAME);
#else
    sceGsResetGraph(1, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
#endif
    sceGsSetDefDBuff(&g_db, SCE_GS_PSMCT32, DISP_WIDTH, (DISP_HEIGHT/2), 2, 0x31, SCE_GS_CLEAR);

    pdrawenv = &g_db.draw0;

#ifdef BUILD_EU_VERSION
    if (sys_wrk.pal_disp_mode == 0)
    {
        g_db.disp[1].display.DX = 656;
        g_db.disp[0].display.DX = 656;
        g_db.disp[1].display.DY = 104;
        g_db.disp[0].display.DY = 104;
    }
    else
    {
        g_db.disp[1].display.DX = 636;
        g_db.disp[0].display.DX = 636;
        g_db.disp[1].display.DY = 50;
        g_db.disp[0].display.DY = 50;
    }
#endif

    sceGsSyncPath(0, 0);
    SgInit3D();
    sceGsSyncPath(0, 0);

    vfunc();
    AdpcmReturnFromMovie();
    EiMain();

    *(int *)REG_DMAC_CTRL &= ~D_CTRL_RELE_M;

    MovieInitWrk();
}

int PadSyncCallback2()
{
    int id;
    int i;
    int state;
    int wrk;
    int exid;
    int p_id;
    PAD_STRUCT *psp;
    char act_align[6];

    p_id = 0;
    psp = pad;

    while (p_id < 2)
    {
        state = scePadGetState(psp->port, psp->slot);

        if (state == scePadStateDiscon)
        {
            psp->flags = psp->flags & ~1;
            psp->step = 0;
            PadClearCount(p_id);
        }
        else
        {
            psp->flags = psp->flags | 1;
        }

        if (state == scePadStateFindPad)
        {
            psp->step = 0;
        }

        switch (psp->step)
        {
            case 0:
                if (state != scePadStateStable && state != scePadStateFindCTP1)
                {
                    break;
                }

                id = scePadInfoMode(psp->port, psp->slot, InfoModeCurID, 0);

                if (id == 0)
                {
                    break;
                }

                exid = scePadInfoMode(psp->port, psp->slot, InfoModeCurExID, 0);

                id = (exid > 0) ? exid : id;

                psp->id = 0;

                if (id != 4 && id != 7)
                {
                    psp->step = 99;
                    break;
                }

                if (scePadInfoAct(psp->port, psp->slot, -1, 0) == 0)
                {
                    psp->step = 99;
                    break;
                }

                act_align[0] = 0;
                act_align[1] = 1;

                for (i = 2; i < 6; i++)
                {
                    act_align[i] = 0xff;
                }

                for (i = 0; i < 6; i++)
                {
                    psp->pad_direct[i] = 0;
                }

                if (scePadSetActAlign(psp->port, psp->slot, (const u_char *)&act_align) == 1)
                {
                    psp->step = 1;
                }
            break;
            case 1:
                if (scePadGetState(psp->port, psp->slot) != scePadStateExecCmd)
                {
                    if (scePadSetMainMode(psp->port, psp->slot, 1, 3) == 1)
                    {
                        psp->step = 2;
                    }
                    else
                    {
                        psp->step = 0;
                    }
                }
            break;
            case 2:
                switch (scePadGetReqState(psp->port, psp->slot))
                {
                    case scePadReqStateBusy:
                        // do nothing ...
                    break;
                    case scePadReqStateFailed:
                        psp->step = 0;
                    break;
                    case scePadReqStateComplete:
                        psp->step = 3;
                    break;
                }
            break;
            case 3:
                id = scePadInfoMode(psp->port, psp->slot, InfoModeCurID, 0);

                if (id == 0)
                {
                    break;
                }

                exid = scePadInfoMode(psp->port, psp->slot, InfoModeCurExID, 0);

                id = (exid > 0) ? exid : id;

                if (id != 7)
                {
                    psp->step = 99;
                }
                else
                {
                    psp->step = 10;
                }
            break;
            case 10:
                if (scePadInfoPressMode(psp->port, psp->slot) != 1)
                {
                    psp->step = 99;
                    break;
                }

                if (scePadEnterPressMode(psp->port, psp->slot) == 1)
                {
                    psp->step = 11;
                }
            break;
            case 11:
                wrk = scePadGetReqState(psp->port, psp->slot);

                if (wrk == 1)
                {
                    psp->step = 10;
                }

                if (wrk == 0)
                {
                    psp->step = 99;
                }
            break;
            case 99:
                if (state != scePadStateStable && state != scePadStateFindCTP1)
                {
                    break;
                }

                PadReadFunc(psp, p_id);

                if ((psp->pad_direct[0] & 0x80) != 0)
                {
                    psp->pad_direct[0] &= 1;

                    scePadSetActDirect(psp->port, psp->slot, psp->pad_direct);

                    if (psp->pad_direct[0] == 0 && psp->pad_direct[1] == 0)
                    {
                        psp->pad_direct[0] = 0;
                        psp->pad_direct[1] = 0;
                    }
                    else
                    {
                        VibrateRequest(p_id, 0, 0);
                    }
                }
            break;
        }

        psp++;
        p_id++;
    }

    return 0;
}

void movVblankPad()
{
    PadSyncCallback2();
}

void ReqLogoMovie(void) {
    movie_wrk.play_event_sta = 0x4 | 0x2 | 0x1;
    play_mov_no = 37;

    vfunc();

    DrawAll2DMes_P2();

    FlushModel(0);
    FlushModel(1);
    ClearTextureCache();

#ifdef BUILD_EU_VERSION
    movie(mpegName[sys_wrk.pal_disp_mode][play_mov_no]);
#else
    movie(mpegName[play_mov_no]);
#endif

    SetIopCmdSm(1, 1, 0, 0);

    SeSetMVol(opt_wrk.bgm_vol);
    SeSetSteMono(opt_wrk.sound_mode);

#ifdef BUILD_EU_VERSION
    sceGsResetGraph(1, SCE_GS_INTERLACE, sys_wrk.pal_disp_mode == 0 ? SCE_GS_PAL: SCE_GS_NTSC, SCE_GS_FRAME);
#else
    sceGsResetGraph(1, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
#endif
    sceGsSetDefDBuff(&g_db, SCE_GS_PSMCT32, DISP_WIDTH, (DISP_HEIGHT/2), 2, 0x31, SCE_GS_CLEAR);

    pdrawenv = &g_db.draw0;

#ifdef BUILD_EU_VERSION
    if (sys_wrk.pal_disp_mode == 0)
    {
        g_db.disp[1].display.DX = 0x290;
        g_db.disp[1].display.DY = 0x68;
        g_db.disp[0].display.DX = 0x290;
        g_db.disp[0].display.DY = 0x68;
    }
    else
    {
        g_db.disp[1].display.DX = 0x27c;
        g_db.disp[1].display.DY = 0x32;
        g_db.disp[0].display.DX = 0x27c;
        g_db.disp[0].display.DY = 0x32;
    }
#endif

    sceGsSyncPath(0, 0);
    SgInit3D();
    sceGsSyncPath(0, 0);

    AdpcmReturnFromMovie();

    EiMain();

    *(int *)REG_DMAC_CTRL &= ~D_CTRL_RELE_M; // yeah ...

    MovieInitWrk();
}

int audioDecCreate(AudioDec *ad, u_char *buff, int buffSize, int iopBuffSize)
{
    ad->state = 0;
    ad->hdrCount = 0;
    ad->put = 0;
    ad->count = 0;
    ad->totalBytes = 0;
    ad->totalBytesSent = 0;
    ad->iopLastPos = 0;
    ad->iopPausePos = 0;
    ad->data = buff;
    ad->size = buffSize;
    ad->iopBuffSize = iopBuffSize;
    ad->iopBuff = (int)sceSifAllocIopHeap(iopBuffSize);

    if (ad->iopBuff < 0)
    {
        return 0;
    }

    ad->iopZero = (int)sceSifAllocIopHeap(0x800);

    if (ad->iopZero < 0)
    {
        return 0;
    }

    memset(_0_buf, 0, sizeof(_0_buf));

    sendToIOP(ad->iopZero, (u_char *)_0_buf, 0x800);
    changeMasterVolume(((opt_wrk.bgm_vol * VOLUME_MASTER_MAX) / 4096) * mpeg_vol_rate[play_mov_no] / 100);

    return 1;
}

int audioDecDelete(AudioDec *ad)
{
    sceSifFreeIopHeap((void *)ad->iopBuff);
    sceSifFreeIopHeap((void *)ad->iopZero);

    changeMasterVolume(0);

    return 1;
}

void audioDecPause(AudioDec* ad)
{
    ad->state = AU_STATE_PAUSE;

    changeInputVolume(0);

    ad->iopPausePos = (sceSdRemote(1, rSdBlockTrans, AUTODMA_CH, SD_TRANS_MODE_STOP, NULL, 0) & 0x00FFFFFF) - ad->iopBuff;

    sceSdRemote(1, rSdVoiceTrans, AUTODMA_CH, SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA, ad->iopZero, 0x4000, 0x800);
}

void audioDecResume(AudioDec* ad)
{
    changeInputVolume(0x7FFF);

    sceSdRemote(1, rSdBlockTrans, AUTODMA_CH, (SD_TRANS_MODE_WRITE_FROM | SD_BLOCK_LOOP), ad->iopBuff, (ad->iopBuffSize/UNIT_SIZE)*UNIT_SIZE, ad->iopBuff + ad->iopPausePos);

    ad->state = AU_STATE_PLAY;
}

void audioDecStart(AudioDec *ad)
{
    audioDecResume(ad);
}

void audioDecReset(AudioDec *ad)
{
    audioDecPause(ad);

    ad->state = AU_STATE_INIT;
    ad->hdrCount = 0;
    ad->put = 0;
    ad->count = 0;
    ad->totalBytes = 0;
    ad->totalBytesSent = 0;
    ad->iopLastPos = 0;
    ad->iopPausePos = 0;
}

void audioDecBeginPut(AudioDec *ad, u_char **ptr0, int *len0, u_char **ptr1, int *len1)
{
    int len;

    if (ad->state == AU_STATE_INIT)
    {
        *ptr0 = (u_char*)ad + (ad->hdrCount + 4);
        *len0 = 0x28 - ad->hdrCount;
        *ptr1 = ad->data;
        *len1 = ad->size;

        return;
    }

    len = ad->size - ad->count;

    if (len <= (ad->size - ad->put)) {
        *ptr0 = ad->data + ad->put;
        *len0 = len;
        *ptr1 = NULL;
        *len1 = 0;

        return;
    }

    *ptr0 = ad->data + ad->put;
    *len0 = (ad->size - ad->put);
    *ptr1 = ad->data;
    *len1 = len - (ad->size - ad->put);
}


void audioDecEndPut(AudioDec *ad, int size)
{
    int hdr_add;

    if (ad->state == AU_STATE_INIT)
    {
        hdr_add = size <= 40U - ad->hdrCount ? size : 40U - ad->hdrCount;

        ad->hdrCount += hdr_add;

        if (ad->hdrCount < 0 || ad->hdrCount >= 40)
        {
            ad->state = AU_STATE_PRESET;
        }

        size -= hdr_add;
    }

    ad->put = (ad->put + size) % ad->size;
    ad->count = ad->count + size;
    ad->totalBytes = ad->totalBytes + size;
}

int audioDecIsPreset(AudioDec *ad)

{
    return ad->totalBytesSent >= ad->iopBuffSize;
}

int audioDecSendToIOP(AudioDec *ad)
{
    int pd0;
    int pd1;
    int d0;
    int d1;
    int pos;
    u_char *ps0;
    u_char *ps1;
    int s0;
    int s1;
    int count_sent;
    int offset;
    int len;

    count_sent = 0;

    switch (ad->state)
    {
    case 0:
        return 0;
    break;
    case 1:
        pd0 = ad->iopBuff + ad->totalBytesSent % ad->iopBuffSize;
        d0 = ad->iopBuffSize - ad->totalBytesSent;
        pd1 = 0;
        d1 = 0;
    break;
    case 2:
        pos = (sceSdRemote(1, 0x8100, 0) & 0x00ffffff)  - ad->iopBuff;
        iopGetArea(&pd0, &d0, &pd1, &d1, ad, pos);
    break;
    case 3:
        return 0;
    break;
    }


    offset = ((ad->put - ad->count) + ad->size) % ad->size;
    len = (ad->count / 1024) * 1024;

    s0 = len < ad->size - offset ? len : ad->size - offset;
    s1 = len - s0;

    ps0 = &ad->data[offset];
    ps1 = &ad->data[0];

    if (d0 + d1 >= 1024 && s0 + s1 >= 1024)
    {
        count_sent = sendToIOP2area(pd0, d0, pd1, d1, ps0, s0, ps1, s1);
    }

    ad->count = ad->count - count_sent;

    ad->totalBytesSent = ad->totalBytesSent + count_sent;
    ad->iopLastPos = (ad->iopLastPos + count_sent) % ad->iopBuffSize;

    return count_sent;
}

static void iopGetArea(int *pd0, int *d0, int *pd1, int *d1, AudioDec *ad, int pos)
{
    int len;

    len = (((pos + ad->iopBuffSize) - ad->iopLastPos) - 1024) % ad->iopBuffSize;
    len = (len / 1024) * 1024;

    if (len <= ad->iopBuffSize - ad->iopLastPos)
    {
        *pd0 = ad->iopBuff + ad->iopLastPos;
        *d0 = len;
        *pd1 = 0;
        *d1 = 0;
    }
    else
    {
        *pd0 = ad->iopBuff + ad->iopLastPos;
        *d0 = ad->iopBuffSize - ad->iopLastPos;
        *pd1 = ad->iopBuff;
        *d1 = len - (ad->iopBuffSize - ad->iopLastPos);
    }
}

static int sendToIOP2area(int pd0, int d0, int pd1, int d1, u_char *ps0, int s0, u_char *ps1, int s1)
{
    int diff;

    if (d0 + d1 < s0 + s1)
    {
        diff = (s0 + s1) - (d0 + d1);

        if (diff >= s1)
        {
            s0 -= diff - s1;
            s1 = 0;
        }
        else
        {
            s1 -= diff;
        }
    }

    if (s0 >= d0)
    {
        sendToIOP(pd0, ps0, d0);
        sendToIOP(pd1, ps0 + d0, s0 - d0);
        sendToIOP(pd1 + s0 - d0, ps1, s1);
    }
    else if (s1 >= d0 - s0)
    {
        sendToIOP(pd0, ps0, s0);
        sendToIOP(pd0 + s0, ps1, d0 - s0);
        sendToIOP(pd1, ps1 + d0 - s0, s1 - (d0 - s0));
    }
    else
    {
        sendToIOP(pd0, ps0, s0);
        sendToIOP(pd0 + s0, ps1, s1);
    }

    return s0 + s1;
}

static int sendToIOP(int dst, u_char *src, int size)
{
    sceSifDmaData transData;
    int did;

    if (size < 1)
    {
        return 0;
    }

    transData.data = (u_int)src;
    transData.addr = dst;
    transData.size = size;
    transData.mode = 0;

    FlushCache(0);

    did = sceSifSetDma(&transData,1);
    while (sceSifDmaStat(did) >= 0) {}

    return size;
}

static void changeMasterVolume(u_int val)
{
    int i;

    for (i = 0; i < 2; i++)
    {
        sceSdRemote(1, 0x8010, i | 0x980, val);
        sceSdRemote(1, 0x8010, i | 0xa80, val);
    }
}

static void changeInputVolume(u_int val)
{
    sceSdRemote(1, 0x8010, 0xf80, val);
    sceSdRemote(1, 0x8010, 0x1080, val);
}

void clearGsMem(int r, int g, int b, int disp_width, int disp_height)
{
    u_long giftag_clear[2] = {
        SCE_GIF_SET_TAG(0, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1),
        SCE_GIF_PACKED_AD,
    };
    sceGifPacket packet;
    u_long128 packetBase[6];
    sceDmaChan *dmaGif;

    dmaGif = sceDmaGetChan(SCE_DMA_GIF);

    SCE_GIF_CLEAR_TAG(&db.giftag0);

    db.giftag0.NLOOP = 8;
    db.giftag0.EOP = SCE_GS_TRUE;
    db.giftag0.NREG = 1;
    db.giftag0.REGS0 = SCE_GIF_PACKED_AD;

    sceGsSetDefDrawEnv(
        &db.draw0,
        SCE_GS_PSMCT32,
        disp_width,
        bound(disp_height / 2, 32) * 2 + bound(disp_height, 32) * 2,
        SCE_GS_ZNOUSE,
        SCE_GS_PSMCT32);

    *(u_long *)&db.draw0.xyoffset1 = SCE_GS_SET_XYOFFSET_1(0, 0);

    FlushCache(0);

    sceGsSyncPath(0, 0);
    sceGsPutDrawEnv(&db.giftag0);

    sceGifPkInit(&packet, packetBase);
    sceGifPkReset(&packet);

    sceGifPkEnd(&packet, 0, 0, 0);

    sceGifPkOpenGifTag(&packet, *(u_long128*)&giftag_clear);
    sceGifPkAddGsAD(&packet, SCE_GS_PRIM, SCE_GS_SET_PRIM(6, 0, 0, 0, 0, 0, 0, 0, 0));
    sceGifPkAddGsAD(&packet, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(r, g, b, 0, 0));
    sceGifPkAddGsAD(&packet, SCE_GS_XYZ2, SCE_GS_SET_XYZ2(0 << 4, 0 << 4, 0));
    sceGifPkAddGsAD(&packet, SCE_GS_XYZ2, SCE_GS_SET_XYZ2(MAX_WIDTH << 4, MAX_HEIGHT * 5 << 4, 0));
    sceGifPkCloseGifTag(&packet);

    sceGifPkTerminate(&packet);

    FlushCache(0);

    sceGsSyncPath(0, 0);
    sceDmaSend(dmaGif, packet.pBase);
    sceGsSyncPath(0, 0);
}

void setImageTag(u_int *tags, void *image, int index, int image_w, int image_h)
{
    int mbx = image_w >> 4;
    int mby = image_h >> 4;
    int i;
    int j;
    Rect tex;
    Rect poly;
    sceGifPacket packet;
    u_long giftag[2] = {
        SCE_GIF_SET_TAG(0, SCE_GS_FALSE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1),
        SCE_GIF_PACKED_AD,
    };
    u_long giftag_eop[2] = {
        SCE_GIF_SET_TAG(0, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1),
        SCE_GIF_PACKED_AD,
    };
    u_long* tag;
    u_int xdr;

    xdr = 0;

    sceGifPkInit(&packet, (u_long128 *)UncAddr(tags));
    sceGifPkReset(&packet);

    if (index == 0)
    {
        sceGifPkCnt(&packet, 0, 0, 0);

        sceGifPkOpenGifTag(&packet, *(u_long128 *)&giftag);
        sceGifPkAddGsAD(
            &packet,
            SCE_GS_BITBLTBUF,
            SCE_GS_SET_BITBLTBUF(0, 0, SCE_GS_PSMCT32,
                (bound(MAX_WIDTH, 64) * bound((MAX_HEIGHT / 2), 32) * 2) / 64,
                bound(MAX_WIDTH, 64) / 64,
                SCE_GS_PSMCT32));
        sceGifPkAddGsAD(&packet, SCE_GS_TRXREG, SCE_GS_SET_TRXREG(16, 16));
        sceGifPkCloseGifTag(&packet);

        for (i = 0; i < mbx; i++)
        {
            for (j = 0; j < mby; j++)
            {
                sceGifPkCnt(&packet, 0, 0, 0);

                sceGifPkOpenGifTag(&packet, *(u_long128 *)giftag);
                sceGifPkAddGsAD(&packet, SCE_GS_TRXPOS, SCE_GS_SET_TRXPOS(0, 0, 16 * i, 16 * j, 0));
                sceGifPkAddGsAD(&packet, SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(xdr));
                sceGifPkCloseGifTag(&packet);

                tag = (u_long*)sceGifPkReserve(&packet, 4);
                tag[0] = SCE_GIF_SET_TAG(16 * 16 * 4 / 16, 0, 0, 0, 2, 0);
                tag[1] = 0;

                sceGifPkRef(&packet, (u_long128 *)DmaAddr(image), 16 * 16 * 4 / 16, 0, 0, 0);
                image = (u_char*)image + 16 * 16 * 4;
            }
        }
    }

    tex.x = 8; // 0.5 << 4
    tex.y = 8; // 0.5 << 4
    tex.w = image_w << 4;
    tex.h = image_h << 4;

    poly.x = (2048 - 320) << 4;
    poly.y = (2048 - (DISP_HEIGHT / 2) / 2) << 4;
    poly.w = DISP_WIDTH << 4;
    poly.h = (DISP_HEIGHT / 2) << 4;

    sceGifPkEnd(&packet, 0, 0, 0);

    sceGifPkOpenGifTag(&packet, *(u_long128 *)giftag_eop);
    sceGifPkAddGsAD(&packet, SCE_GS_TEXFLUSH, 0);
    sceGifPkAddGsAD(&packet, SCE_GS_TEX1_1, SCE_GS_SET_TEX1_1(0, 0, 1, 1, 0, 0, 0));
    sceGifPkAddGsAD(&packet, SCE_GS_TEX0_1,
        SCE_GS_SET_TEX0_1(
            (bound(MAX_WIDTH, 64) * bound((MAX_HEIGHT / 2), 32) * 2) / 64,
            bound(MAX_WIDTH, 64) / 64,
            SCE_GS_PSMCT32, 10, 10, 0, 1, 0, 0, 0, 0, 0
        )
    );
    sceGifPkAddGsAD(&packet, SCE_GS_PRIM, SCE_GS_SET_PRIM(6, 0, 1, 0, 0, 0, 1, 0, 0));
    sceGifPkAddGsAD(&packet, SCE_GS_UV, SCE_GS_SET_UV(tex.x, tex.y));
    sceGifPkAddGsAD(&packet, SCE_GS_XYZ2, SCE_GS_SET_XYZ2(poly.x, poly.y, 0));
    sceGifPkAddGsAD(&packet, SCE_GS_UV, SCE_GS_SET_UV(tex.x + tex.w, tex.y + tex.h));
    sceGifPkAddGsAD(&packet, SCE_GS_XYZ2, SCE_GS_SET_XYZ2(poly.x + poly.w, poly.y + poly.h, 0));
    sceGifPkCloseGifTag(&packet);

    sceGifPkTerminate(&packet);
}

int vblankHandlerM(int val)
{
    sceDmaChan *dmaGif_loadimage;
    VoTag *tag;

#ifdef BUILD_EU_VERSION
    DIntr();
#endif

    if (isCountVblank != 0)
    {
        dmaGif_loadimage = sceDmaGetChan(SCE_DMA_GIF);

#if BUILD_EU_VERSION
#else
        vblankCount++;
#endif
        oddeven = ((*GS_CSR) >> 13) & 1;

        handler_error = sceGsSyncPath(1, 0);

        if (handler_error == 0)
        {
            tag = voBufGetTag(&voBuf);

            if (tag == NULL)
            {
                frd++;

#if BUILD_EU_VERSION
                SetSubtitles(0, movie_wrk.play_event_no, vblankCount);
                MakeMovMes();
#endif

                ExitHandler();

#if BUILD_EU_VERSION
                EIntr();
#endif

                return 0;
            }

            sceGsSetHalfOffset(
                (sceGsDrawEnv1*)(oddeven & 1 ? UncAddr(&db.draw1) : UncAddr(&db.draw0)),
                2048,
                2048,
                oddeven ^ 1
            );

            if (oddeven == 0 && tag->status == VOBUF_STATUS_FULL)
            {
                sceGsSwapDBuff(&db, 0);
                sceGsSyncPath(0, 0);
                sceDmaSend(dmaGif_loadimage, &tag->v[0]);

                tag->status = VOBUF_STATUS_TOPDONE;
            }
            else if (oddeven == 1 && tag->status == VOBUF_STATUS_TOPDONE)
            {
                sceGsSwapDBuff(&db, 1);
                sceGsSyncPath(0, 0);
                sceDmaSend(dmaGif_loadimage, &tag->v[1]);

                tag->status = VOBUF_STATUS_;
                isFrameEnd = 1;
#if BUILD_EU_VERSION
                vblankCount++;
#endif
            }

#if BUILD_EU_VERSION
            SetSubtitles(0, movie_wrk.play_event_no, vblankCount);
            MakeMovMes();
#endif
        }
    }

    ExitHandler();

#if BUILD_EU_VERSION
    EIntr();
#endif

    return 0;
}

int handler_endimage(int val)
{
    if (isFrameEnd)
    {
        voBufDecCount(&voBuf);

        isFrameEnd = 0;
    }

    ExitHandler();

    return 0;
}

void startDisplay(int waitEven)
{
    while (sceGsSyncV(0) == waitEven) {}

    frd = 0;
    isCountVblank = 1;
    vblankCount = 0;
}

void endDisplay()
{
    isCountVblank = 0;

    frd = 0;
}


int videoCallback(sceMpeg *mp, sceMpegCbDataStr *str, void *data)
{
    u_char *ps0;
    u_char *ps1;
    int s0;
    int s1;
    u_char *pd0;
    u_char *pd1;
    int d0;
    int d1;
    int len;
    ReadBuf *rb;
    u_char *pd0_unc;
    u_char *pd1_unc;

    rb = (ReadBuf*)data;

    ps0 = str->data;
    ps1 = rb->data;

    s0 = min(rb->data + rb->size - str->data, str->len);
    s1 = str->len - s0;

    videoDecBeginPut(&videoDec, &pd0, &d0, &pd1, &d1);

    pd0_unc = (u_char *)UncAddr(pd0);
    pd1_unc = (u_char *)UncAddr(pd1);

    len = copy2area(pd0_unc, d0, pd1_unc, d1, ps0, s0, ps1, s1);

    if (len > 0 && videoDecPutTs(&videoDec, str->pts, str->dts, pd0, len) == 0)
    {
        ErrMessage("pts buffer overflow\n");
    }

    videoDecEndPut(&videoDec, len);

    return len > 0;
}

int pcmCallback(sceMpeg *mp, sceMpegCbDataStr *str, void *data)
{
    ReadBuf *rb;
    u_char *ps0;
    u_char *ps1;
    int s0;
    int s1;
    u_char *pd0;
    u_char *pd1;
    int d0;
    int d1;
    int len;
    int ret;

    rb = (ReadBuf*)data;
    ps0 = str->data;
    ps1 = rb->data;

    ps0 = str->data + 4;

    if (ps0 >= rb->data + rb->size)
    {
        ps0 -= rb->size;
    }

    len = str->len - 4;

    ps1 = rb->data;

    s0 = min(rb->data + rb->size - ps0, len);
    s1 = len - s0;

    audioDecBeginPut(&audioDec, &pd0, &d0, &pd1, &d1);

    ret = copy2area(pd0, d0, pd1, d1, ps0, s0, ps1, s1);

    audioDecEndPut(&audioDec, ret);

    return ret > 0;
}

static int copy2area(u_char *pd0, int d0, u_char *pd1, int d1, u_char *ps0, int s0, u_char *ps1, int s1)
{
    if (d0 + d1 < s0 + s1)
    {
        return 0;
    }

    if (s0 >= d0)
    {
        memcpy(pd0, ps0, d0);
        memcpy(pd1, ps0 + d0, s0 - d0);
        memcpy(pd1 + s0 - d0, ps1, s1);
    }
    else if (s1 >= d0 - s0)
    {
        memcpy(pd0, ps0, s0);
        memcpy(pd0 + s0, ps1, d0 - s0);
        memcpy(pd1, ps1 + d0 - s0, s1 - (d0 - s0));
    }
    else
    {
        memcpy(pd0, ps0, s0);
        memcpy(pd0 + s0, ps1, s1);
    }

    return s0 + s1;
}

void readBufCreate(ReadBuf *b)
{
    b->put = b->count = 0;
    b->size = READBUF_BUFF_SIZE;
}

void readBufDelete(ReadBuf *b)
{
    return;
}

int readBufBeginPut(ReadBuf *b, u_char **ptr)
{
    int size;

    size = b->size - b->count;

    if (size)
    {
        *ptr = b->data + b->put;
    }

    return size;
}

int readBufEndPut(ReadBuf *b, int size)
{
    int size_ok;

    size_ok = min(b->size - b->count, size);

    b->put = (b->put + size_ok) % b->size;
    b->count += size_ok;

    return size_ok;
}

int readBufBeginGet(ReadBuf *b, u_char **ptr)
{
    if (b->count)
    {
        *ptr = b->data + (b->put - b->count + b->size) % b->size;
    }

    return b->count;
}

int readBufEndGet(ReadBuf *b, int size)
{
    int size_ok;

    size_ok = min(b->count, size);

    b->count -= size_ok;

    return size_ok;
}

int strFileOpen(StrFile *file, char *filename)
{
    char *body;
    char fn[256];
    char devname[64];
    int dlen;
    int i;
    int len;
    char *tail;
    sceCdRMode mode;
    int ret;

    body = index(filename, ':');

    if (body)
    {
        dlen = body - filename;
        strncpy(devname, filename, dlen);
        devname[dlen] = 0;

        body += 1;

        if (!strcmp(devname, "cdrom0"))
        {
            len = strlen(body);

            file->isOnCD = 1;

            for (i = 0; i < len; i++)
            {
                if (body[i] == '/')
                {
                    body[i] = '\\';
                }

                body[i] = toupper(body[i]);
            }

            tail = (index(filename, ';'))? "": ";1";

            sprintf(fn, "%s%s", body, tail);
        }
        else
        {
            file->isOnCD = 0;
            sprintf(fn, "%s:%s", devname, body);
        }
    }
    else
    {
        body = filename;
        strcpy(devname, "host0");

        file->isOnCD = 0;
        sprintf(fn, "%s:%s", devname, body);
    }

    if (file->isOnCD)
    {
        if (isStrFileInit == 0)
        {
            sceCdDiskReady(0);
            isStrFileInit = 1;
        }

        file->iopBuf = sceSifAllocIopHeap(2048 * 80 + 16);

        sceCdStInit(80, 5, bound((u_int)file->iopBuf, 16));

        if(sceCdSearchFile(&file->fp, fn) == 0)
        {
            return 0;
        }

        file->size = file->fp.size;

        mode.trycount = 0;
        mode.spindlctrl = SCECdSpinStm;
        mode.datapattern = SCECdSecS2048;

        sceCdStStart(file->fp.lsn, &mode);
    }
    else
    {
        file->fd = sceOpen(fn, SCE_RDONLY);

        if (file->fd < 0)
        {
            return 0;
        }

        file->size = sceLseek(file->fd, 0, SCE_SEEK_END);

        if (file->size < 0)
        {
            sceClose(file->fd);
            return 0;
        }

        ret = sceLseek(file->fd, 0, SCE_SEEK_SET);

        if (ret < 0)
        {
            sceClose(file->fd);
            return 0;
        }
    }

    return 1;
}

int strFileClose(StrFile *file)
{
    if (file->isOnCD)
    {
        sceCdStStop();
        sceSifFreeIopHeap(file->iopBuf);
    }
    else
    {
        sceClose(file->fd);
    }

    return 1;
}

int strFileRead(StrFile *file, void *buff, int size)
{
    int count;
    u_int err;

    if (file->isOnCD)
    {
        count= sceCdStRead(size >> 11, (u_int *)buff, STMBLK, &err);
        count <<= 11;
    }
    else
    {
        count = sceRead(file->fd, buff, size);
    }

    return count;
}

int getFIFOindex(ViBuf *f, void *addr)
{
    void *val;

    if (addr == DmaAddr(f->tag + (f->n + 1)))
    {
        return 0;
    }
    else
    {
        return ((u_int)addr - (u_int)f->data) / VIBUF_ELM_SIZE;
    }
}

void setD3_CHCR(u_int val)
{
    DIntr();

    *D_ENABLEW = *D_ENABLER | 0x00010000;

    *D3_CHCR = val;

    *D_ENABLEW = *D_ENABLER & ~0x00010000;

    EIntr();
}

void setD4_CHCR(u_int val)
{
    DIntr();

    *D_ENABLEW = ((*D_ENABLER)|0x00010000);

    *D4_CHCR = val;

    *D_ENABLEW = ((*D_ENABLER)&~0x00010000);

    EIntr();
}

void scTag2(QWORD *q, void *addr, u_int id, u_int qwc)
{
    q->l[0] = (u_long)(u_int)addr << 32 | (u_long)id << 28 | (u_long)qwc << 0;
}

int viBufCreate(ViBuf *f, u_long128 *data, u_long128 *tag, int size, TimeStamp *ts, int n_ts)
{
    SemaParam param;

    f->data = data;
    f->tag = UncAddr(tag);
    f->n = size;
    f->buffSize = size * VIBUF_ELM_SIZE;
    f->ts = ts;
    f->n_ts = n_ts;

    param.initCount = 1;
    param.maxCount = 1;

    f->sema = CreateSema(&param);

    viBufReset(f);

    f->totalBytes = 0;

    return 1;
}

int viBufReset(ViBuf *f)
{
    int i;

    f->dmaStart = 0;
    f->dmaN = 0;
    f->readBytes = 0;
    f->isActive = 1;
    f->count_ts = 0;
    f->wt_ts = 0;

    for (i = 0; i < f->n_ts; i++)
    {
        f->ts[i].pts = TS_NONE;
        f->ts[i].dts = TS_NONE;
        f->ts[i].pos = 0;
        f->ts[i].len = 0;
    }


    for (i = 0; i < f->n; i++)
    {
        scTag2(
            (QWORD*)(f->tag + i),
            DmaAddr((char*)f->data + VIBUF_ELM_SIZE * i),
            DMA_ID_REF,
            VIBUF_ELM_SIZE / 16
        );
    }

    scTag2(
        (QWORD*)(f->tag + i),
        DmaAddr(f->tag),
        DMA_ID_NEXT,
        0
    );

    *D4_QWC = 0;
    *D4_MADR = (u_int)DmaAddr(f->data);
    *D4_TADR = (u_int)DmaAddr(f->tag);

    setD4_CHCR((0 << 8) | (1 << 2) | 1);

    return 1;
}

void viBufBeginPut(ViBuf *f, u_char **ptr0, int *len0, u_char **ptr1, int *len1)
{
    int en;
    int es;
    int fs;
    int fn;

    WaitSema(f->sema);

    fs = ((f->dmaStart + f->dmaN) * VIBUF_ELM_SIZE);
    fn = ((f->n - 2 -  f->dmaN) * VIBUF_ELM_SIZE);

    es = (fs + f->readBytes) % f->buffSize;
    en = fn - f->readBytes;

    if (f->buffSize - es >= en)
    {
        *ptr0 = (u_char*)f->data + es;
        *len0 = en;
        *ptr1 = NULL;
        *len1 = 0;
    }
    else
    {
        *ptr0 = (u_char*)f->data + es;
        *len0 = f->buffSize - es;
        *ptr1 = (u_char*)f->data;
        *len1 = en - (f->buffSize - es);
    }

    SignalSema(f->sema);
}

void viBufEndPut(ViBuf *f, int size)
{
    WaitSema(f->sema);

    f->readBytes += size;
    f->totalBytes += size;

    SignalSema(f->sema);
}

int viBufAddDMA(ViBuf *f)
{
    int i;
    int index;
    u_int d4chcr;
    int isNewData;
    int read_n;
    int done;
    int start;

    isNewData = 0;

    WaitSema(f->sema);

    if (!f->isActive)
    {
        ErrMessage("DMA ADD not active\n");
        return 0;
    }

    setD4_CHCR((DMA_ID_REFE<<28) | (0<<8) | (1<<2) | 1);

    d4chcr = *D4_CHCR;

    index = getFIFOindex(f, (void*)*D4_MADR);

    done = (index + f->n - f->dmaStart) % f->n;

    f->dmaStart = (f->dmaStart + done) % f->n;
    f->dmaN -= done;

    start = (f->dmaStart + f->dmaN) % f->n;
    read_n = f->readBytes/VIBUF_ELM_SIZE;
    f->readBytes %= VIBUF_ELM_SIZE;

    if (read_n > 0)
    {
        u_int offset = (f->dmaStart + f->dmaN - 1 + f->n) % f->n;

        scTag2(
            (QWORD*)(f->tag + offset),
            (char*)f->data + VIBUF_ELM_SIZE * offset,
            DMA_ID_REF,
            VIBUF_ELM_SIZE / 16
        );

        isNewData = 1;
    }

    index = start;

    for (i = 0; i < read_n; i++)
    {
        u_int id = (i == read_n - 1)? DMA_ID_REFE: DMA_ID_REF;

        scTag2(
            (QWORD*)(f->tag + index),
            (char*)f->data + VIBUF_ELM_SIZE * index,
            id,
            VIBUF_ELM_SIZE / 16
        );

        index = (index + 1) % f->n;
    }

    f->dmaN += read_n;

    if (f->dmaN)
    {
        if (isNewData)
        {
            d4chcr = (d4chcr & 0x0fffffff) | (DMA_ID_REF << 28);
        }

        setD4_CHCR(d4chcr | 0x100);
    }

    SignalSema(f->sema);

    return 1;
}

int viBufStopDMA(ViBuf *f)
{
    WaitSema(f->sema);

    f->isActive = 0;

    setD4_CHCR(0 << 8 | 1 << 2 | 1);

    f->env.d4madr = *D4_MADR;
    f->env.d4tadr = *D4_TADR;
    f->env.d4qwc =  *D4_QWC;
    f->env.d4chcr = *D4_CHCR;

    while (DGET_IPU_CTRL() & 0xf0) {}

    setD3_CHCR(0 << 8 | 0);

    f->env.d3madr = *D3_MADR;
    f->env.d3qwc =  *D3_QWC;
    f->env.d3chcr = *D3_CHCR;
    f->env.ipubp = DGET_IPU_BP();
    f->env.ipuctrl = DGET_IPU_CTRL();

    SignalSema(f->sema);

    return 1;
}

int viBufRestartDMA(ViBuf *f)
{
    int bp;
    int fp;
    int ifc;
    u_int d4madr_next;
    u_int d4qwc_next;
    u_int d4tadr_next;
    u_int d4chcr_next;
    int index;
    int index_next;
    int id;

    bp = f->env.ipubp & 0x7f;
    fp = (f->env.ipubp >> 16) & 0x3;
    ifc = (f->env.ipubp >> 8) & 0xf;
    d4madr_next = f->env.d4madr - ((fp + ifc) << 4);
    d4qwc_next = f->env.d4qwc + (fp + ifc);
    d4tadr_next = f->env.d4tadr;
    d4chcr_next = f->env.d4chcr | 0x100;

    WaitSema(f->sema);

    if (d4madr_next < (u_int)f->data)
    {
        d4qwc_next = (DATA_ADDR(0) - d4madr_next) >> 4;
        d4madr_next += (u_int)(f->n * VIBUF_ELM_SIZE);
        d4tadr_next = TAG_ADDR(0);

        id = (f->env.d4madr == DATA_ADDR(0) || f->env.d4madr == DATA_ADDR(f->n)) ? DMA_ID_REFE : DMA_ID_REF;

        d4chcr_next = (f->env.d4chcr & 0x0fffffff) | id << 28 | 0x100;

        if (!IsInRegion(0, f->dmaStart, f->dmaN, f->n))
        {
            f->dmaStart = f->n - 1;
            f->dmaN++;
        }
    }
    else if ((index = getFIFOindex(f, (void*)f->env.d4madr)) != (index_next = getFIFOindex(f, (void*)d4madr_next)))
    {
        d4tadr_next = TAG_ADDR(index);
        d4qwc_next = (DATA_ADDR(index) - d4madr_next) >> 4;

        id = (WRAP_ADDR(f->env.d4madr) == DATA_ADDR((f->dmaStart + f->dmaN) % f->n)) ? DMA_ID_REFE : DMA_ID_REF;

        d4chcr_next = (f->env.d4chcr & 0x0fffffff) | id << 28 | 0x100;

        if (!IsInRegion(index_next, f->dmaStart, f->dmaN, f->n))
        {
            f->dmaStart = index_next;
            f->dmaN++;
        }
    }

    if (f->env.d3madr && f->env.d3qwc)
    {
        *D3_MADR = f->env.d3madr;
        *D3_QWC  = f->env.d3qwc;

        setD3_CHCR(f->env.d3chcr | 0x100);
    }

    if (f->dmaN)
    {
        while (sceIpuIsBusy()) {}

        sceIpuBCLR(bp);

        while (sceIpuIsBusy()) {}
    }

    *D4_MADR = d4madr_next;
    *D4_TADR = d4tadr_next;
    *D4_QWC  = d4qwc_next;

    if (f->dmaN)
    {
        setD4_CHCR(d4chcr_next);
    }

    *IPU_CTRL = f->env.ipuctrl;

    f->isActive = 1;

    SignalSema(f->sema);

    return 1;
}

int viBufDelete(ViBuf *f)
{
    setD4_CHCR(0 << 8 | 1 << 2 | 1);

    *D4_QWC = 0;
    *D4_MADR = 0;
    *D4_TADR = 0;

    DeleteSema(f->sema);

    return 1;
}

int viBufIsActive(ViBuf *f)
{
    int ret;

    WaitSema(f->sema);

    ret = f->isActive;

    SignalSema(f->sema);

    return ret;
}

int viBufCount(ViBuf *f)
{
    int ret;

    WaitSema(f->sema);

    ret = f->dmaN * VIBUF_ELM_SIZE + f->readBytes;

    SignalSema(f->sema);

    return ret;
}

void viBufFlush(ViBuf *f)
{
    WaitSema(f->sema);

    f->readBytes = bound(f->readBytes, VIBUF_ELM_SIZE);

    SignalSema(f->sema);
}

int viBufModifyPts(ViBuf *f, TimeStamp *new_ts)
{
    TimeStamp *ts;
    int rd;
    int datasize;
    int loop;
    int tgt;
    int pos;
    int size;
    int len;

    rd = (f->wt_ts - f->count_ts + f->n_ts) % f->n_ts;
    datasize =  VIBUF_ELM_SIZE * f->n;
    loop = 1;

    if (f->count_ts > 0)
    {
        while (loop)
        {
            ts = f->ts + rd;

            if (ts->len == 0 || new_ts->len == 0)
            {
                break;
            }

            if (IsPtsInRegion(ts->pos, new_ts->pos, new_ts->len, datasize))
            {
                len = min(new_ts->pos + new_ts->len - ts->pos, ts->len);

                ts->pos = (ts->pos + len) % datasize;
                ts->len -= len;

                if (ts->len == 0)
                {
                    if (ts->pts >= 0)
                    {
                        ts->pts = TS_NONE;
                        ts->dts = TS_NONE;
                        ts->pos = 0;
                        ts->len = 0;
                    }

                    f->count_ts = max(f->count_ts - 1, 0);
                }
            }
            else
            {
                loop = 0;
            }

            rd = (rd + 1) % f->n_ts;
        }
    }

    return 0;
}

int viBufPutTs(ViBuf *f, TimeStamp *ts)
{
    int ret;

    ret = 0;

    WaitSema(f->sema);

    if (f->count_ts < f->n_ts)
    {
        viBufModifyPts(f, ts);

        if (ts->pts >= 0 || ts->dts >= 0)
        {
            f->ts[f->wt_ts].pts = ts->pts;
            f->ts[f->wt_ts].dts = ts->dts;
            f->ts[f->wt_ts].pos = ts->pos;
            f->ts[f->wt_ts].len = ts->len;

            f->count_ts++;
            f->wt_ts = (f->wt_ts + 1) % f->n_ts;
        }

        ret = 1;
    }

    SignalSema(f->sema);

    return ret;
}

int viBufGetTs(ViBuf *f, TimeStamp *ts)
{
    u_int d4madr;
    u_int ipubp;
    int bp;
    int fp;
    int ifc;
    u_int d4madr_next;
    int datasize;
    int isEnd;
    int tscount;
    int wt;
    int i;
    int pos;
    int len;
    int size;
    u_int stop;

    d4madr = *D4_MADR;
    ipubp = DGET_IPU_BP();
    bp = f->env.ipubp & 0x7f;
    fp = (ipubp >> 16) & 0x3;
    ifc = (ipubp >> 8) & 0xf;
    d4madr_next = d4madr - ((fp + ifc) << 4);
    datasize =  VIBUF_ELM_SIZE * f->n;
    isEnd = 0;

    WaitSema(f->sema);

    ts->pts = TS_NONE;
    ts->dts = TS_NONE;

    stop = (d4madr_next + (bp >> 3) + datasize - (u_int)f->data) % datasize;

    tscount = f->count_ts;

    wt = f->wt_ts;

    for (i = 0; i < tscount && !isEnd; i++)
    {
        int j = (wt - tscount + f->n_ts + i) % f->n_ts;

        if (IsPtsInRegion(stop, f->ts[j].pos, f->ts[j].len, datasize))
        {
            isEnd = 1;

            ts->pts = f->ts[j].pts;
            ts->dts = f->ts[j].dts;

            f->ts[j].pts = TS_NONE;
            f->ts[j].dts = TS_NONE;

            f->count_ts -= min(1, f->count_ts);
        }
    }

    SignalSema(f->sema);

    return 1;
}

int videoDecCreate(VideoDec *vd, u_char *mpegWork, int mpegWorkSize, u_long128 *data, u_long128 *tag, int tagSize, TimeStamp *pts, int n_pts)
{
    sceMpegCreate(&vd->mpeg, mpegWork, mpegWorkSize);

    sceMpegAddCallback(&vd->mpeg, sceMpegCbError, (sceMpegCallback)mpegError, NULL);
    sceMpegAddCallback(&vd->mpeg, sceMpegCbNodata, mpegNodata, NULL);
    sceMpegAddCallback(&vd->mpeg, sceMpegCbStopDMA, mpegStopDMA, NULL);
    sceMpegAddCallback(&vd->mpeg, sceMpegCbRestartDMA, mpegRestartDMA, NULL);
    sceMpegAddCallback(&vd->mpeg, sceMpegCbTimeStamp, (sceMpegCallback)mpegTS, NULL);

    videoDecReset(vd);

    viBufCreate(&vd->vibuf, data, tag, tagSize, pts, n_pts);

    return 1;
}

void videoDecSetDecodeMode(VideoDec *vd, int ni, int np, int nb)
{
    sceMpegSetDecodeMode(&vd->mpeg, ni, np, nb);
}

int videoDecSetStream(VideoDec *vd, int strType, int ch, sceMpegCallback cb, void *data)
{
    sceMpegAddStrCallback(&vd->mpeg, strType, ch, cb, data);

    return 1;
}

void videoDecBeginPut(VideoDec *vd, u_char **ptr0, int *len0, u_char **ptr1, int *len1)
{
    viBufBeginPut(&vd->vibuf, ptr0, len0, ptr1, len1);
}

void videoDecEndPut(VideoDec *vd, int size)
{
    viBufEndPut(&vd->vibuf, size);
}

void videoDecReset(VideoDec *vd)
{
    vd->state = 0;
}

int videoDecDelete(VideoDec *vd)
{
    viBufDelete(&vd->vibuf);

    sceMpegDelete(&vd->mpeg);

    return 1;
}

void videoDecAbort(VideoDec *vd)
{
    vd->state = 1;
}

u_int videoDecGetState(VideoDec *vd)
{
    return vd->state;
}

u_int videoDecSetState(VideoDec *vd, u_int state)
{
    u_int old;

    old = vd->state;

    vd->state = state;

    return old;
}

int videoDecPutTs(VideoDec *vd, long int pts_val, long int dts_val, u_char *start, int len)
{
    TimeStamp ts;

    ts.pts = pts_val;
    ts.dts = dts_val;
    ts.pos = start - (u_char *)vd->vibuf.data;
    ts.len = len;

    return viBufPutTs(&videoDec.vibuf, &ts);
}

int videoDecInputCount(VideoDec *vd)
{
    return viBufCount(&vd->vibuf);
}

int videoDecInputSpaceCount(VideoDec *vd)
{
    u_char *ptr0;
    u_char *ptr1;
    int len0;
    int len1;

    videoDecBeginPut(vd, &ptr0, &len0, &ptr1, &len1);

    return len0 + len1;
}

int videoDecFlush(VideoDec *vd)
{
    u_char *pd0;
    u_char *pd1;
    u_char seq_end_code[4] = {0x00, 0x00, 0x01, 0xb7};
    int d0;
    int d1;
    u_char *pd0_unc;
    u_char *pd1_unc;
    int size;

    videoDecBeginPut(vd, &pd0, &d0, &pd1, &d1);

    if (d0 + d1 < 4)
    {
        return 0;
    }

    pd0_unc = (u_char*)UncAddr(pd0);
    pd1_unc = (u_char*)UncAddr(pd1);

    size = cpy2area(pd0_unc, d0, pd1_unc, d1, seq_end_code, 4, NULL, 0);

    videoDecEndPut(&videoDec, size);

    viBufFlush(&vd->vibuf);

    if (vd->state == VD_STATE_NORMAL)
    {
        vd->state = VD_STATE_FLUSH;
    }

    return 1;
}

MOVIE_WRK movie_wrk = {0};
int thread_id = 0;
u_int controller_val = 0;
int videoDecTh = 0;
int defaultTh = 0;
int frd = 0;
int play_mov_no = 0;

int videoDecIsFlushed(VideoDec *vd)
{
    return videoDecInputCount(vd) == 0 && sceMpegIsRefBuffEmpty(&vd->mpeg);
}

void videoDecMain(VideoDec *vd)
{
    viBufReset(&vd->vibuf);

    voBufReset(&voBuf);

    decBs0(vd);

    while (voBuf.count) {}

     videoDecSetState(vd, VD_STATE_END);
}

int decBs0(VideoDec *vd)
{
    VoData *voData;
    int status;
    int i;
    int image_w;
    int image_h;

    status = 1;

    while (sceMpegIsEnd(&vd->mpeg) == 0)
    {
        if (videoDecGetState(vd) == VD_STATE_ABORT)
        {
            status = -1;
            break;
        }

        while ((voData = voBufGetData(&voBuf)) == 0)
        {
            switchThread();
        }

        if (sceMpegGetPicture(&vd->mpeg, (sceIpuRGB32*)voData->v, MAX_WIDTH / 16 * MAX_HEIGHT / 16) < 0)
        {
            ErrMessage("sceMpegGetPicture() decode error");
        }

        if (vd->mpeg.frameCount == 0)
        {
            image_w = vd->mpeg.width;
            image_h = vd->mpeg.height;

            for (i = 0; i < voBuf.size; i++)
            {
              setImageTag(voBuf.tag[i].v[0], voBuf.data[i].v, 0, image_w, image_h);
              setImageTag(voBuf.tag[i].v[1], voBuf.data[i].v, 1, image_w, image_h);
            }
        }

        voBufIncCount(&voBuf);

        switchThread();
    }

    sceMpegReset(&vd->mpeg);

    return status;
}

int mpegError(sceMpeg *mp, sceMpegCbDataError *cberror, void *anyData)
{
    return 1;
}

int mpegNodata(sceMpeg *mp, sceMpegCbData *cbdata, void *anyData)
{
    switchThread();

    viBufAddDMA(&videoDec.vibuf);

    return 1;
}

int mpegStopDMA(sceMpeg *mp, sceMpegCbData *cbdata, void *anyData)
{
    viBufStopDMA(&videoDec.vibuf);

    return 1;
}

int mpegRestartDMA(sceMpeg *mp, sceMpegCbData *cbdata, void *anyData)
{
    viBufRestartDMA(&videoDec.vibuf);

    return 1;
}

int mpegTS(sceMpeg *mp, sceMpegCbDataTimeStamp *cbts, void *anyData)
{
    TimeStamp ts;

    viBufGetTs(&videoDec.vibuf, &ts);

    cbts->pts = ts.pts;
    cbts->dts = ts.dts;

    return 1;
}

static int cpy2area(u_char *pd0, int d0, u_char *pd1, int d1, u_char *ps0, int s0, u_char *ps1, int s1)
{
    if (d0 + d1 < s0 + s1)
    {
        return 0;
    }

    if (s0 >= d0)
    {
        memcpy(pd0, ps0, d0);
        memcpy(pd1, ps0 + d0, s0 - d0);
        memcpy(pd1 + s0 - d0, ps1, s1);
    }
    else if (s1 >= d0 - s0)
    {
        memcpy(pd0, ps0, s0);
        memcpy(pd0 + s0, ps1, d0 - s0);
        memcpy(pd1, ps1 + d0 - s0, s1 - (d0 - s0));
    }
    else
    {
        memcpy(pd0, ps0, s0);
        memcpy(pd0 + s0, ps1, s1);
    }

    return s0 + s1;
}

void voBufCreate(VoBuf *f, VoData *data, VoTag *tag, int size)
{
    int i;

    f->data = data;
    f->tag = tag;
    f->size = size;
    f->count = 0;
    f->write = 0;

    for (i = 0; i < size; i++)
    {
        f->tag[i].status = VOBUF_STATUS_;
    }
}

void voBufDelete(VoBuf *f)
{
    return;
}

void voBufReset(VoBuf *f)
{
    f->count = 0;
    f->write = 0;
}

int voBufIsFull(VoBuf *f)
{
    return f->count == f->size;
}

void voBufIncCount(VoBuf *f)
{
    DIntr();

    f->tag[f->write].status = VOBUF_STATUS_FULL;
    f->count++;
    f->write = (f->write + 1) % f->size;

    EIntr();
}

VoData* voBufGetData(VoBuf *f)
{
    return voBufIsFull(f) ? NULL: f->data + f->write;
}

int voBufIsEmpty(VoBuf *f)
{
    return f->count == 0;
}

VoTag* voBufGetTag(VoBuf *f)
{
    return voBufIsEmpty(f) ? NULL : f->tag + ((f->write - f->count + f->size) % f->size);
}

void voBufDecCount(VoBuf *f)
{
    if (f->count > 0)
    {
        f->count--;
    }
}
