#ifdef BUILD_EU_VERSION

#include "common.h"
#include "typedefs.h"
#include "subtitles.h"

#include "sce/libdma.h"
#include "sce/libgraph.h"

#include "main/glob.h"
#include "graphics/graph2d/message.h"
#include "ingame/menu/ig_menu.h"
#include "graphics/graph2d/tim2.h"

typedef struct {
	int mesno;
	u_int in;
	u_int out;
	u_int attr;
} SUBTITLES;

typedef struct {
	int pos_x;
	int pos_y;
	int path;
	int fr_rate;
	int pri;
	u_int attr;
	int *tbl;
	SUBTITLES **list;
} SUBTITLES_DAT;

typedef struct {
	u_int num;
	u_int pflg;
	u_short inout[64][2];
} SUBTITLES_DBG;

#include "data/subtitles_dummy.h"
#include "data/subtitles0010.h"
#include "data/subtitles0031.h"
#include "data/subtitles1000.h"
#include "data/subtitles1031.h"
#include "data/subtitles1240.h"
#include "data/subtitles2010.h"
#include "data/subtitles2050.h"
#include "data/subtitles2061.h"
#include "data/subtitles2071.h"
#include "data/subtitles2091.h"
#include "data/subtitles2131.h"
#include "data/subtitles2171.h"
#include "data/subtitles3010.h"
#include "data/subtitles3080.h"
#include "data/subtitles4010.h"
#include "data/subtitles4031.h"
#include "data/subtitles4080.h"
#include "data/subtitles4090.h"
#include "data/subtitles4110.h"
#include "data/subtitles4120.h"
#include "data/subtitles5010.h"
#include "data/subtitles5020.h"
#include "data/subtitles0020.h"
#include "data/subtitles0022.h"
#include "data/subtitles1010.h"
#include "data/subtitles1034.h"
#include "data/subtitles1070.h"
#include "data/subtitles1102.h"
#include "data/subtitles1120.h"
#include "data/subtitles1160.h"
#include "data/subtitles1170.h"
#include "data/subtitles1180.h"
#include "data/subtitles1250.h"
#include "data/subtitles1260.h"
#include "data/subtitles1330.h"
#include "data/subtitles1410.h"
#include "data/subtitles2013.h"
#include "data/subtitles2072.h"
#include "data/subtitles2092.h"
#include "data/subtitles2100.h"
#include "data/subtitles2140.h"
#include "data/subtitles2141.h"
#include "data/subtitles2160.h"
#include "data/subtitles2170.h"
#include "data/subtitles3020.h"
#include "data/subtitles3060.h"
#include "data/subtitles4011.h"
#include "data/subtitles_tape_w1.h"
#include "data/subtitles_tape_w2.h"
#include "data/subtitles_tape_r1.h"
#include "data/subtitles_tape_r2.h"
#include "data/subtitles_tape_r3.h"
#include "data/subtitles_tape_r4.h"
#include "data/subtitles_tape_b1.h"
#include "data/subtitles_tape_b2.h"
#include "data/subtitles_mafuyu_1.h"
#include "data/subtitles_mafuyu_2.h"
#include "data/subtitles_ag28_hen05.h"
#include "data/subtitles_ag29_hen06.h"
#include "data/subtitles_ag26_hen03.h"
#include "data/subtitles_ag23_hen00.h"
#include "data/subtitles_ag43_haha01.h"
#include "data/subtitles_ag06_sakka06.h"
#include "data/subtitles_ag74_kokirie09.h"
#include "data/subtitles_ag05_sakka05.h"
#include "data/subtitles_ag00_sakka00.h"
#include "data/subtitles_ag11_josyu03.h"
#include "data/subtitles_ag70_kokirie05.h"
#include "data/subtitles_ag10_josyu02.h"
#include "data/subtitles_ag07_sakka07.h"
#include "data/subtitles_ag25_hen02.h"
#include "data/subtitles_ag08_josyu00.h"
#include "data/subtitles_ag09_josyu01.h"
#include "data/subtitles_ag13_josyu05.h"
#include "data/subtitles_ag17_josyu09.h"
#include "data/subtitles_ag11_mayoigo11.h"
#include "data/subtitles_ag13_mayoigo13.h"
#include "data/subtitles_ag01_minzoku01.h"
#include "data/subtitles_ag14_mayoigo14.h"
#include "data/subtitles_ag06_minzoku06.h"
#include "data/subtitles_ag00_mayoigo00.h"
#include "data/subtitles_ag05_kokirie05.h"
#include "data/subtitles_ag21_mayoigo21.h"
#include "data/subtitles_ag08_minzoku08.h"
#include "data/subtitles_ag11_minzoku11.h"
#include "data/subtitles_ag02_haha02.h"
#include "data/subtitles_ag09_minzoku09.h"
#include "data/subtitles_ag07_kokirie07.h"
#include "data/subtitles_gd_editor.h"
#include "data/subtitles_gd_assistant.h"
#include "data/subtitles_gd_novelist.h"
#include "data/subtitles_gd_lostchl1.h"
#include "data/subtitles_gd_lostchl2.h"
#include "data/subtitles_gd_lostchl3.h"
#include "data/subtitles_gd_folkwf.h"
#include "data/subtitles_gd_folk.h"
#include "data/subtitles_gd_mega.h"
#include "data/subtitles_gd_master.h"
#include "data/subtitles_gd_priest1.h"
#include "data/subtitles_gd_priest2.h"
#include "data/subtitles_gd_priest3.h"
#include "data/subtitles_gd_priest4.h"
#include "data/subtitles_master_00.h"
#include "data/subtitles_priest_01.h"
#include "data/subtitles_priest_02.h"
#include "data/subtitles_priest_03.h"
#include "data/subtitles_priest_04.h"

SUBTITLES *subtitles_mov_list[] = {
    subtitles0010,
    subtitles0020,
    subtitles0022,
    subtitles_dummy,
    subtitles0031,
    subtitles1000,
    subtitles1010,
    subtitles_dummy,
    subtitles1031,
    subtitles1034,
    subtitles1070,
    subtitles_dummy,
    subtitles_dummy,
    subtitles_dummy,
    subtitles_dummy,
    subtitles1102,
    subtitles1120,
    subtitles_dummy,
    subtitles1160,
    subtitles1170,
    subtitles1180,
    subtitles_dummy,
    subtitles_dummy,
    subtitles1240,
    subtitles1250,
    subtitles1260,
    subtitles_dummy,
    subtitles_dummy,
    subtitles_dummy,
    subtitles_dummy,
    subtitles1330,
    subtitles_dummy,
    subtitles_dummy,
    subtitles_dummy,
    subtitles1410,
    subtitles2010,
    subtitles2013,
    subtitles_dummy,
    subtitles_dummy,
    subtitles_dummy,
    subtitles2050,
    subtitles_dummy,
    subtitles2061,
    subtitles_dummy,
    subtitles_dummy,
    subtitles2071,
    subtitles2072,
    subtitles_dummy,
    subtitles2091,
    subtitles2092,
    subtitles2100,
    subtitles_dummy,
    subtitles_dummy,
    subtitles2131,
    subtitles_dummy,
    subtitles2140,
    subtitles2141,
    subtitles_dummy,
    subtitles_dummy,
    subtitles_dummy,
    subtitles2160,
    subtitles2170,
    subtitles2171,
    subtitles3010,
    subtitles3020,
    subtitles_dummy,
    subtitles_dummy,
    subtitles_dummy,
    subtitles3060,
    subtitles_dummy,
    subtitles3080,
    subtitles_dummy,
    subtitles_dummy,
    subtitles_dummy,
    subtitles_dummy,
    subtitles4010,
    subtitles4011,
    subtitles_dummy,
    subtitles_dummy,
    subtitles4031,
    subtitles_dummy,
    subtitles_dummy,
    subtitles_dummy,
    subtitles_dummy,
    subtitles_dummy,
    subtitles_dummy,
    subtitles4080,
    subtitles4090,
    subtitles_dummy,
    subtitles4110,
    subtitles4120,
    subtitles5010,
    subtitles5020,
    subtitles_dummy,
    subtitles_dummy,
    subtitles_dummy,
    subtitles_dummy,
    subtitles_dummy,
    subtitles0010,
    subtitles1000,
};
SUBTITLES *subtitles_tape_list[] = {
    subtitles_tape_w1,
    subtitles_tape_w2,
    subtitles_tape_r1,
    subtitles_tape_r2,
    subtitles_tape_r3,
    subtitles_tape_r4,
    subtitles_tape_b1,
    subtitles_tape_b2,
};
SUBTITLES *subtitles_mafuyu_list[] = {
    subtitles_mafuyu_1,
    subtitles_mafuyu_2,
};
SUBTITLES *subtitles_aghost_list[] = {
    subtitles_ag28_hen05,
    subtitles_ag29_hen06,
    subtitles_ag26_hen03,
    subtitles_ag23_hen00,
    subtitles_ag43_haha01,
    subtitles_ag06_sakka06,
    subtitles_ag74_kokirie09,
    subtitles_ag05_sakka05,
    subtitles_ag00_sakka00,
    subtitles_ag11_josyu03,
    subtitles_ag70_kokirie05,
    subtitles_ag10_josyu02,
    subtitles_ag07_sakka07,
    subtitles_ag25_hen02,
    subtitles_ag29_hen06,
    subtitles_ag08_josyu00,
    subtitles_ag09_josyu01,
    subtitles_ag13_josyu05,
    subtitles_ag17_josyu09,
    subtitles_ag11_mayoigo11,
    subtitles_ag13_mayoigo13,
    subtitles_ag01_minzoku01,
    subtitles_ag14_mayoigo14,
    subtitles_ag06_minzoku06,
    subtitles_ag00_mayoigo00,
    subtitles_ag05_kokirie05,
    subtitles_ag21_mayoigo21,
    subtitles_ag08_minzoku08,
    subtitles_ag11_minzoku11,
    subtitles_ag02_haha02,
    subtitles_ag09_minzoku09,
    subtitles_ag07_kokirie07,
};
SUBTITLES *subtitles_ghost_d_list[] = {
    subtitles_gd_editor,
    subtitles_gd_assistant,
    subtitles_gd_novelist,
    subtitles_gd_lostchl1,
    subtitles_gd_lostchl2,
    subtitles_gd_lostchl3,
    subtitles_gd_folkwf,
    subtitles_gd_folk,
    subtitles_gd_mega,
    subtitles_gd_master,
    subtitles_gd_priest1,
    subtitles_gd_priest2,
    subtitles_gd_priest3,
    subtitles_gd_priest4,
};
SUBTITLES *subtitles_priest_list[] = {
    subtitles_master_00,
    subtitles_priest_01,
    subtitles_priest_02,
    subtitles_priest_03,
    subtitles_priest_04,
};// subtitles_tape_no

#include "data/subtitles_tape_no.h"
#include "data/subtitles_priest_no.h"
#include "data/subtitles_ghost_d_no.h"
#include "data/subtitles_aghost_no.h"

#include "data/subtitles_dat.h" // static SUBTITLES_DAT subtitles_dat[/*6*/] = {...};
SUBTITLES_SYS subtitles_sys = {0};

static SUBTITLES_DBG st_dbg;

#define DMA_MEM 0x0FFFFFFF

void ContSubtitlesTime(int path, u_int frame)
{
	int i;
	int n;
	DISP_STR ds;
	STR_DAT sdt = {
        .str = NULL,
        .pos_x = 484,
        .pos_y = 70,
        .type = 1,
        .r = 0x50,
        .g = 0x50,
        .b = 0x50,
        .alpha = 0x80,
        .pri = 0x10,
    };
	u_char time[12] = {0, 0, 0, 0, 0, 0x8e, 0, 0, 0, 0, 0, 0xff};

    if (*key_now[8] != 0 && st_dbg.pflg == 0)
    {
        st_dbg.pflg = 1;
        st_dbg.inout[st_dbg.num][0] = frame;
    }
    
    if (st_dbg.pflg != 0)
    {
        st_dbg.inout[st_dbg.num][1] = frame;
    }
    
    if (*key_now[8] == 0 && st_dbg.pflg != 0)
    {
        st_dbg.inout[st_dbg.num++][1] = frame;
        st_dbg.pflg = 0;
    }
    
    if (*key_now[9] != 0)
    {
        InitSubtitlesSys();
    }
    
    if (*key_now[11] != 0)
    {
        subtitles_sys.flg = 0;
    }
    
    n = st_dbg.num + (st_dbg.pflg != 0);
    
    for (i = 0; i < n; i++)
    {
        time[4] = (st_dbg.inout[i][0] % 10) + 0x3f;
        time[3] = ((st_dbg.inout[i][0] / 10) % 10) + 0x3f;
        time[2] = ((st_dbg.inout[i][0] / 100) % 10) + 0x3f;
        time[1] = ((st_dbg.inout[i][0] / 1000) % 10) + 0x3f;
        time[0] = ((st_dbg.inout[i][0] / 10000) % 10) + 0x3f;
        
        time[10] = (st_dbg.inout[i][1] % 10) + 0x3f;
        time[9] = ((st_dbg.inout[i][1] / 10) % 10) + 0x3f;
        time[8] = ((st_dbg.inout[i][1] / 100) % 10) + 0x3f;
        time[7] = ((st_dbg.inout[i][1] / 1000) % 10) + 0x3f;
        time[6] = ((st_dbg.inout[i][1] / 10000) % 10) + 0x3f;
        
        CopyStrDToStr(&ds, &sdt);
        
        ds.str = time;
        
        if (st_dbg.pflg != 0 && i == st_dbg.num)
        {
            ds.r = 0xff;
            ds.g = 0xff;
            ds.b = 0xff;
        }
        
        ds.pos_x = 484;
        ds.pos_y = (n - i - 1) * 24 + 70;
        
        SetMessageMov(path,0,&ds);
    }
}

void DispNowFrame(int path, u_int frame)
{
	DISP_STR ds;
	STR_DAT sd = {
        .str = NULL,
        .pos_x = 520,
        .pos_y = 16,
        .type = 1,
        .r = 0xff,
        .g = 0xff,
        .b = 0xff,
        .alpha = 0x80,
        .pri = 0x10,
    };
	STR_DAT sd2 = {
        .str = NULL,
        .pos_x = 556,
        .pos_y = 40,
        .type = 1,
        .r = 0xff,
        .g = 0xff,
        .b = 0xff,
        .alpha = 0x80,
        .pri = 0x10,
    };
	u_char time[9] = {0x3f, 0x3f, 0x94, 0x3f, 0x3f, 0x94, 0x3f, 0x3f, 0xff}; // 00:00:00
	u_char time2[6] = {0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0xff}; // 00000
    u_int h;
    u_int m;
    u_int s;

    s = (frame % 30);
    m = (frame / 30) % 60;
    h = frame / 1800;
    
    time[0] += h / 10;
    time[1] += h % 10;

    time[3] += m / 10;
    time[4] += m % 10;

    time[6] += (s * 100 / 30) / 10;
    time[7] += (s * 100 / 30) % 10;
    
    CopyStrDToStr(&ds, &sd);
    
    ds.str = time;
    
    SetMessageMov(path, 0, &ds);
    
    time2[4] += (frame / 1) % 10;
    time2[3] += (frame / 10) % 10;
    time2[2] += (frame / 100) % 10;
    time2[1] += (frame / 1000) % 10;
    time2[0] += (frame / 10000) % 10;
    
    CopyStrDToStr(&ds, &sd2);
    
    ds.str = time2;
    
    SetMessageMov(path, 0, &ds);
}

int GetIndexNums(int *tbl, int no)
{
	int i;

    i = 0;

    while (tbl[i] != -1)
    {
        if (no == tbl[i])
        {
            return i;
        }

        i++;
    }

    return -1;
}

void MakeMovMes()
{
    if (sys_wrk.language != 0)
    {
        nmdpri = 0;
        nmdpkt = 0;
        
        DrawMovMes(mpbuf);
    }
}

void DrawMovMes(Q_WORDDATA *packet_buf)
{
	sceDmaChan *DmaGif;

    if (sys_wrk.language != 0)
    {
        DmaGif = sceDmaGetChan(2);
        sceDmaSend(DmaGif, (u_long128 *)((u_int)packet_buf & DMA_MEM));
        sceDmaSync(DmaGif, 0, 0);
        sceGsSyncPath(0, 0);
    }
}

void InitSubtitlesSys()
{
    subtitles_sys.type = 0;
    subtitles_sys.no = 0;
    subtitles_sys.cnt = 0;
    subtitles_sys.alp = 0;
    subtitles_sys.flg = 0;
    subtitles_sys.run = 0;
}

void FinishSubtitlesSys()
{
    return;
}

void SetSubtitlesNCnt(int type, int no)
{
    SetSubtitles(type, no, subtitles_sys.cnt++);
}

void SetSubtitlesNCntOne(int type, int no)
{
    subtitles_sys.type = type;
    subtitles_sys.flg = 1;
    subtitles_sys.no = no;
    
    SetSubtitles(type, no, subtitles_sys.cnt++);
}

void CallSubtitles()
{
    if (subtitles_sys.flg != 0)
    {
        SetSubtitles(subtitles_sys.type, subtitles_sys.no, subtitles_sys.cnt);
        
        subtitles_sys.cnt = subtitles_sys.cnt + 1;
    }
}

void SetSubtitles(int type, int no, u_int mframe)
{
	int i;
	int pass;
	int path;
	int posx;
	int posy;
	int frm;
	int add;
	u_int frame;
	DISP_STR ds;
	STR_DAT sd = {
        .str = NULL,
        .pos_x = 0,
        .pos_y = 0,
        .type = 1,
        .r = 0xff,
        .g = 0xff,
        .b = 0xff,
        .alpha = 0x80,
        .pri = 0x10,
    };
	SUBTITLES *stp;
	SUBTITLES *stlp;
	SUBTITLES_DAT *stdp;

    pass = 0;

    if (sys_wrk.language == 0)
    {
        subtitles_sys.flg = 0;
        return;
    }

    frame = sys_wrk.pal_disp_mode == 0 ? (mframe * 6) / 5 : mframe;

    if (type >= 0 && no >= 0)
    {
        stdp = &subtitles_dat[type];

        path = stdp->path;
        posx = stdp->pos_x;
        posy = stdp->pos_y;
        frame = frame / stdp->fr_rate;

        if (stdp->tbl != NULL)
        {
            i = GetIndexNums(stdp->tbl, no);
            if (i == -1)
            {
                return;
            }
        }
        else
        {
            i = no;
        }

        stlp = stdp->list[i];

        i = 0;
        while (stlp[i].mesno != -1)
        {
            stp = &stlp[i];

            if ((stp->in <= frame) && (frame < stp->out))
            {
                if (stp->attr & 0xf0)
                {
                    if (stp->attr & 0x20)
                    {
                        add = 8;
                        frm = 0x10;
                    }
                    else if (stp->attr & 0x10)
                    {
                        add = 4;
                        frm = 0x20;
                    }

                    if (stp->in + frm >= frame)
                    {
                        subtitles_sys.alp = subtitles_sys.alp + add > 0x80 ? 0x80 : subtitles_sys.alp + add;
                    }
                    else if (frame >= stp->out - frm)
                    {
                        subtitles_sys.alp = subtitles_sys.alp - add < 0 ? 0 : subtitles_sys.alp - add;
                    }
                    else
                    {
                        subtitles_sys.alp = 0x80;
                    }
                }
                else
                {
                    subtitles_sys.alp = 0x80;
                }

                CopyStrDToStr(&ds, &sd);

                ds.alpha = subtitles_sys.alp;
                ds.str = (u_char *)GetIngameMSGAddr(53, stp->mesno);
                ds.pri = stdp->pri;

                if (stp->attr & 0x400)
                {
                    ds.pos_x = posx;
                    ds.pos_y = 0x20;
                }
                else
                {
                    ds.pos_x = posx;
                    ds.pos_y = posy;
                }

                if (stp->attr & 0xf)
                {
                    if (stp->attr & 1)
                    {
                        ds.r = 0xff;
                        ds.g = 0x40;
                        ds.b = 0x40;
                    }
                    else if (stp->attr & 2)
                    {
                        ds.r = 0;
                        ds.g = 0;
                        ds.b = 0;
                    }
                    else if (stp->attr & 4)
                    {   
                        ds.r = 0x50;
                        ds.g = 0x50;
                        ds.b = 0x50;
                    }
                }

                if (
                    ((stdp->attr & 0x200) == 0 || (stp->attr & 0x400) != 0) ||
                    ((ingame_wrk.mode & 7) && ingame_wrk.stts == 0 && (plyr_wrk.mode == 0 || plyr_wrk.mode == 10))
                )
                {
                    SetMessageMov(path, (stp->attr & 0x100) == 0, &ds);
                }

                subtitles_sys.run = 1;
                pass = 1;
            }

            i++;
        }

        if (i > 0 && stlp[i - 1].out <= frame)
        {
            subtitles_sys.flg = 0;
        }

        if (!pass)
        {
            subtitles_sys.alp = 0;
        }

        if (path == 1)
        {
            MesPacketEnd();
        }
    }
}

void CheckSubtitlesFlag()
{
    if (subtitles_sys.run == 1)
    {
        subtitles_sys.run = 2;
    }
    else
    {
        subtitles_sys.run = 0;
    }
}

#endif // BUILD_EU_VERSION
