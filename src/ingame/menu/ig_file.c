#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "ig_file.h"

#include "graphics/graph2d/effect_sub.h"
#include "graphics/graph2d/message.h"
#include "graphics/graph2d/tim2.h"
#include "ingame/info/inf_disp.h"
#include "ingame/map/item_ctl.h"
#include "ingame/menu/ig_file.h"
#include "ingame/menu/ig_glst.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/item.h"
#include "main/glob.h"
#include "os/eeiop/cdvd/eecdvd.h"
#include "os/eeiop/eese.h"
#include "os/pad.h"

static void FileCntInit();
static void FileCntRenew();
static void FileModeInOut();
static void FileModeInOut2();
static void DspFileOut(short int pos_x, short int pos_y, short int alpha);
static void DspKindTab(short int pos_x, short int pos_y, short int alpha);
static void DspFile2D(short int pos_x, short int pos_y, short int alpha);
static void DspFile2D2(short int pos_x, short int pos_y, short int alpha);
static void DspFileLst(short int pos_x, short int pos_y, short int alpha);
static void PutBarScale(short int pos_x, short int pos_y, u_short now_lng, u_char alp, u_short chr_top);
static void SideTri(u_char hav_now, u_char dsp_max, u_short chr_top, short int alpha);
static void DspMsgWndw(short int pos_x, short int pos_y, short int alpha);
static void DspFileDtl(short int pos_x, short int pos_y, short int alpha);
static void DspFileBook(short int pos_x, short int pos_y, short int alpha);
static void DspFilePhot(short int pos_x, short int pos_y, short int alpha);
static void PhotDispCtrl(u_char alp);
static char AllVramTensoOK(char pht_num, DSP_BOX *dsp_box);
static short int TargetPhot(short int csr, short int no);
static u_char DspPhot2D(u_long tex, u_char rt, short int px, short int py, short int sx, short int sy, u_char alp);
static void DspBack2D(short int pht_no, u_char rt, short int px, short int py, short int sx, short int sy, u_char alp);
static void DspFileCmmn(short int pos_x, short int pos_y, short int alpha);
static char TateizFreeSpace();


#ifdef BUILD_EU_VERSION
#define LOAD_ADDRESS 0x1d51db0
#define SPRT_ADDRESS 0x1d83000
#define SPRT_ADDRESS_2 0x1e04000
#else
#define LOAD_ADDRESS 0x1d573b0
#define SPRT_ADDRESS 0x1d88100
#define SPRT_ADDRESS_2 0x1e05b00
#endif

u_char file_kind[] = { 0,  2,  1, 0xff };
#include "data/name_plate.h" // static RELATION_NAME_PLATE name_plate[];
#include "data/spr_dat_tmp.h" // SPRT_DAT spr_dat_tmp[];
#include "data/exp_table.h" // static char exp_table[][8][2];
#include "data/hum_table.h" // static char hum_table[][8][2];
#include "data/rel_com.h" // RELATION_COM rel_com[];
#ifdef BUILD_EU_VERSION
#include "data/rel_com_pos_adj.h" // int rel_com_pos_adj[][5][2];
#endif
#include "data/name_rbl.h" // static char name_rbl[][2];
#include "data/rel_prt.h" // RELATION_PRT rel_prt[];
#include "data/rel_dat.h" // RELATION_DAT rel_dat[];
#include "data/rel_yaj.h" // RELATION_YAJI rel_yaj[];
#include "data/caption_list.h" // CAPTION_LIST caption_list[];
#include "data/caption_data.h" // CAPTION_DATA caption_data[];
RELATION_CSR rel_csr = {0};

static FILE_REL file_rel;
static FLSH_CORE flsh[4];

static MEM_BOX mem_box[3];
static MENU_FILE menu_file;
static PHOT_FILE phot_file[4];
static short int menu_dsp_file[50];

void NewgameMenuFileInit()
{
    menu_file = (MENU_FILE){0};

    memset(&phot_file, 0, sizeof(phot_file));
}

void LoadgameMenuFileInit()
{
    menu_file = (MENU_FILE){0};

    memset(&phot_file, 0, sizeof(phot_file));
}

void StartFileModeInit()
{
    int i;

    ig_menu_wrk.csr[1] = ig_menu_wrk.csr[2] = 0;

    menu_file.load_id = -1;
    menu_file.tex_now = 0;
    menu_file.tex_bak = 0;
    menu_file.alp_now = 0;
    menu_file.alp_bak = 0;
    menu_file.f_mode = 0;
    menu_file.can_dsp = 0xff;
    menu_file.lst_top = 0;
    menu_file.mod = 0;
    menu_file.open_page = 1;
    menu_file.poss_chk = 0;

    for (i = 0; i < 4; i++)
    {
        GetDispMenuFile(file_kind[i]);

        menu_file.poss_chk = menu_file.poss_chk + ig_menu_wrk.file_num;
    }

    menu_file.pht_alp = 0;

    memset(mem_box, -1, sizeof(mem_box));

    yw2d.pad_lock = 1;

    menu_file.pad_lock = 0;
}

void IngameMenuFile()
{
    IngameMenuFileDisp();

    if (menu_file.poss_chk == 0)
    {
        if (yw2d.pad_lock == 0 && *key_now[4] == 1)
        {
            yw2d.out_mode_cnt = 11;

            SeStartFix(3, 0, 0x1000, 0x1000, 1);
        }

        return;
    }

    switch (menu_file.mod)
    {
    case 0:
        if (yw2d.pad_lock != 0 || menu_file.pad_lock != 0)
        {
            break;
        }

        if (*key_now[4] == 1)
        {
            yw2d.out_mode_cnt = 11;

            SeStartFix(3, 0, 0x1000, 0x1000, 1);
        }
        else if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            if (ig_menu_wrk.csr[1] < 3)
            {
                if (ig_menu_wrk.file_num != 0)
                {
                    menu_file.mod = 1;

                    SeStartFix(1, 0, 0x1000, 0x1000, 1);
                }
                else
                {
                    menu_file.mod = 2;

                    SeStartFix(2, 0, 0x1000, 0x1000, 1);
                }
            }
            else
            {
                ig_menu_wrk.mode = 11;

                file_rel.load_id = -1;
                file_rel.step = 0;

                SeStartFix(1, 0, 0x1000, 0x1000, 1);
            }
        }
        else if (
            *key_now[0] == 1 ||
            (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
            Ana2PadDirCnt(0) == 1 ||
            (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
        )
        {
            if (menu_file.poss_chk != 0)
            {
                if (ig_menu_wrk.csr[1] != 0)
                {
                    ig_menu_wrk.csr[1]--;
                }
                else
                {
                    ig_menu_wrk.csr[1] = 3;
                }

                ig_menu_wrk.csr[2] = 0;

                menu_file.lst_top = 0;

                SeStartFix(0, 0, 0x1000, 0x1000, 1);
            }
        }
        else if (
            *key_now[1] == 1 ||
            (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
            Ana2PadDirCnt(2) == 1 ||
            (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
        )
        {
            if (menu_file.poss_chk != 0)
            {
                if (ig_menu_wrk.csr[1] <= 2)
                {
                    ig_menu_wrk.csr[1]++;
                }
                else
                {
                    ig_menu_wrk.csr[1] = 0;
                }

                ig_menu_wrk.csr[2] = 0;

                menu_file.lst_top = 0;

                SeStartFix(0, 0, 0x1000, 0x1000, 1);
            }
        }
    break;
    case 1:
        if (yw2d.pad_lock != 0 || menu_file.pad_lock != 0)
        {
            break;
        }

        if (*key_now[4] == 1)
        {
            menu_file.mod = 0;

            SeStartFix(3, 0, 0x1000, 0x1000, 1);
        }
        else if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            ig_menu_wrk.mode = IGMENU_MODE_FILE_DATA;

            menu_file.mod = 3;

            switch (ig_menu_wrk.csr[1])
            {
            case 0:
                menu_file.dtl = 0;
            break;
            case 1:
                menu_file.dtl = 2;
            break;
            case 2:
                menu_file.dtl = 3;
                menu_file.pht_alp = 0;
            break;
            case 3:
                menu_file.dtl = 0xff;
            break;
            }

            menu_file.open_page = 1;
            menu_file.csr_bak = 0xff;

            SeStartFix(1, 0, 0x1000, 0x1000, 1);
        }
        else if (
            *key_now[0] == 1 ||
            (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
            Ana2PadDirCnt(0) == 1 ||
            (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
        )
        {
            if (ig_menu_wrk.csr[2] != 0)
            {
                if (ig_menu_wrk.csr[2] == menu_file.lst_top)
                {
                    menu_file.lst_top--;
                }

                ig_menu_wrk.csr[2]--;
            }
            else
            {
                ig_menu_wrk.csr[2] = ig_menu_wrk.file_num - 1;

                if (ig_menu_wrk.file_num >= 8)
                {
                    menu_file.lst_top = ig_menu_wrk.file_num - 8;
                }
                else
                {
                    menu_file.lst_top = 0;
                }
            }

            SeStartFix(0, 0, 0x1000, 0x1000, 1);
        }
        else if (
            *key_now[1] == 1 ||
            (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
            Ana2PadDirCnt(2) == 1 ||
            (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
        )
        {
            if (ig_menu_wrk.csr[2] < ig_menu_wrk.file_num - 1)
            {
                if (ig_menu_wrk.csr[2] == menu_file.lst_top + 7)
                {
                    menu_file.lst_top++;
                }

                ig_menu_wrk.csr[2]++;
            }
            else
            {
                ig_menu_wrk.csr[2] = 0;

                menu_file.lst_top = 0;
            }

            SeStartFix(0, 0, 0x1000, 0x1000, 1);
        }
    break;
    case 2:
        if (yw2d.pad_lock != 0 || menu_file.pad_lock != 0)
        {
            break;
        }

        if (*key_now[4] == 1)
        {
            menu_file.mod = 0;

            SeStartFix(3, 0, 0x1000, 0x1000, 1);
        }
        else if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            menu_file.mod = 0;

            SeStartFix(1, 0, 0x1000, 0x1000, 1);
        }
        else if (
            *key_now[0] == 1 ||
            (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
            Ana2PadDirCnt(0) == 1 ||
            (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
        )
        {
            if (menu_file.poss_chk != 0)
            {
                if (ig_menu_wrk.csr[1] != 0)
                {
                    ig_menu_wrk.csr[1]--;
                }
                else
                {
                    ig_menu_wrk.csr[1] = 3;
                }

                ig_menu_wrk.csr[2] = 0;

                menu_file.lst_top = 0;
                menu_file.mod = 0;

                SeStartFix(0, 0, 0x1000, 0x1000, 1);
            }
        }
        else if (
            *key_now[1] == 1 ||
            (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
            Ana2PadDirCnt(2) == 1 ||
            (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
        )
        {
            if (menu_file.poss_chk != 0)
            {
                if (ig_menu_wrk.csr[1] <= 2)
                {
                    ig_menu_wrk.csr[1]++;
                }
                else
                {
                    ig_menu_wrk.csr[1] = 0;
                }

                ig_menu_wrk.csr[2] = 0;

                menu_file.lst_top = 0;
                menu_file.mod = 0;

                SeStartFix(0, 0, 0x1000, 0x1000, 1);
            }
        }
    break;
    }
}

void IngameMenuFileDisp()
{
    GetDispMenuFile(file_kind[ig_menu_wrk.csr[1]]);
    FileCntInit();
    FileModeInOut();
    DspMenuTitle(yw2d.io_x[1], yw2d.io_y[1], yw2d.io_a[1], 0x32, 4);

    if (menu_file.poss_chk != 0)
    {
        DspFileOut(yw2d.io_x[4], yw2d.io_y[4], yw2d.io_a[4]);
    }
    else
    {
        DrawMessageBox(0x64000, 170.0f, 210.0f, 300.0f, 60.0f, yw2d.io_a[4]);
        PutStringYW(21, 3, 195, 228, 0x808080, yw2d.io_a[4], 0x1000, 0);
    }

    FileModeInOut2();
    FileCntRenew();
}

void IngameMenuFileData()
{
    short int no;

    IngameMenuFileDataDisp();

    if (menu_file.dtl != 3)
    {
        if (yw2d.pad_lock == 0 && menu_file.pad_lock == 0)
        {
            if (*key_now[4] == 1)
            {
                menu_file.mod = 1;

                ig_menu_wrk.mode = IGMENU_MODE_FILE;

                SeStartFix(3, 0, 0x1000, 0x1000, 1);
            }
            else if (*key_now[6] == 1 || *key_now[5] == 1)
            {
                if (menu_file.page_all >= 2)
                {
                    if (menu_file.open_page < menu_file.page_all)
                    {
                        menu_file.open_page++;

                        SeStartFix(4, 0, 0x1000, 0x1000, 1);
                    }
                    else
                    {
                        menu_file.mod = 1;

                        ig_menu_wrk.mode = IGMENU_MODE_FILE;

                        SeStartFix(1, 0, 0x1000, 0x1000, 1);
                    }
                }
                else
                {
                    menu_file.mod = 1;

                    ig_menu_wrk.mode = IGMENU_MODE_FILE;

                    SeStartFix(3, 0, 0x1000, 0x1000, 1);
                }
            }
            else if (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            )
            {
                if (menu_file.page_all >= 2 && menu_file.page_all > menu_file.open_page)
                {
                    menu_file.open_page++;

                    SeStartFix(4, 0, 0x1000, 0x1000, 1);
                }
            }
            else if (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            )
            {
                if (menu_file.page_all >= 2 && menu_file.open_page >= 2)
                {
                    menu_file.open_page--;

                    SeStartFix(4, 0, 0x1000, 0x1000, 1);
                }
            }
        }
    }
    else
    {
        if (yw2d.pad_lock == 0 && menu_file.pad_lock == 0)
        {
            if (*key_now[4] == 1)
            {
                if (IsLoadEndAll() != 0)
                {
                    menu_file.tex_now = 0;
                    menu_file.load_id = -1;
                    menu_file.can_dsp = 0xff;
                    menu_file.alp_now = 0;
                    menu_file.tex_bak = 0;

                    memset(mem_box, -1, sizeof(mem_box));

                    menu_file.mod = 1;

                    ig_menu_wrk.mode = IGMENU_MODE_FILE;

                    SeStartFix(3, 0, 0x1000, 0x1000, 1);
                }
            }
            else if (
                *key_now[2] == 1 ||
                (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
                Ana2PadDirCnt(3) == 1 ||
                (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
            )
            {
                if (ig_menu_wrk.file_num >= 2)
                {
                    if (ig_menu_wrk.csr[2] != 0)
                    {
                        ig_menu_wrk.csr[2]--;
                    }
                    else
                    {
                        ig_menu_wrk.csr[2] = ig_menu_wrk.file_num - 1;

                        if (ig_menu_wrk.csr[2] > 7)
                        {
                            menu_file.lst_top = ig_menu_wrk.csr[2] + -7;
                        }
                        else
                        {
                            menu_file.lst_top = 0;
                        }
                    }

                    if (ig_menu_wrk.csr[2] - menu_file.lst_top < 0)
                    {
                        menu_file.lst_top--;
                    }

                    menu_file.pht_alp = 0;

                    SeStartFix(0, 0, 0x1000, 0x1000, 1);
                }
            }
            else if (
                *key_now[3] == 1 ||
                (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
                Ana2PadDirCnt(1) == 1 ||
                (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
            )
            {
                if (ig_menu_wrk.file_num > 1)
                {
                    if (ig_menu_wrk.file_num - 1 > ig_menu_wrk.csr[2])
                    {
                        ig_menu_wrk.csr[2]++;
                    }
                    else
                    {
                        ig_menu_wrk.csr[2] = 0;

                        menu_file.lst_top = 0;
                    }

                    if (ig_menu_wrk.csr[2] - menu_file.lst_top > 7)
                    {
                        menu_file.lst_top++;
                    }

                    SeStartFix(0, 0, 0x1000, 0x1000, 1);

                    menu_file.pht_alp = 0;
                }
            }
        }
    }

    if (file_kind[ig_menu_wrk.csr[1]] != 1)
    {
        if (poss_file[file_kind[ig_menu_wrk.csr[1]]][menu_dsp_file[ig_menu_wrk.csr[2]]] == 2)
        {
            poss_file[file_kind[ig_menu_wrk.csr[1]]][menu_dsp_file[ig_menu_wrk.csr[2]]] = 3;
        }
    }
    else if (menu_dsp_file[ig_menu_wrk.csr[2]] < 50)
    {
        if (poss_file[1][menu_dsp_file[ig_menu_wrk.csr[2]]] == 2)
        {
            poss_file[1][menu_dsp_file[ig_menu_wrk.csr[2]]] = 3;
        }
    }
    else
    {
        no = menu_dsp_file[ig_menu_wrk.csr[2]] - 50;

        if (poss_file[3][no] == 2)
        {
            poss_file[3][no] = 3;
        }
    }
}

void IngameMenuFileDataDisp()
{
    GetDispMenuFile(file_kind[ig_menu_wrk.csr[1]]);

    DspMenuTitle(yw2d.io_x[1], yw2d.io_y[1], yw2d.io_a[1], 0x32, 4);

    DspFileDtl(0, 0, 0x80);
}

void GetDispMenuFile(u_char csr)
{
    int i;

    if (csr == 0xff)
    {
        ig_menu_wrk.file_num = 0;

        return;
    }

    memset(menu_dsp_file, -1, sizeof(menu_dsp_file[0]) * 25); // memset only half of menu_dsp_file?

    ig_menu_wrk.file_num = 0;

    for (i = 0; i < 50; i++)
    {
        if (poss_file[csr][i]> 1)
        {
            menu_dsp_file[ig_menu_wrk.file_num] = i;

            ig_menu_wrk.file_num++;
        }
    }

    if (csr == 1)
    {
        for (i = 0; i < 50; i++)
        {
            if (poss_file[3][i] > 1)
            {
                menu_dsp_file[ig_menu_wrk.file_num] = i + 50;

                ig_menu_wrk.file_num++;
            }
        }
    }
}

static void FileCntInit()
{
    return;
}

static void FileCntRenew()
{
    if (yw2d.inn_mode_cnt < 65535)
    {
        yw2d.inn_mode_cnt++;
    }

    if (yw2d.out_mode_cnt != 0)
    {
        yw2d.out_mode_cnt--;
    }
}

static void FileModeInOut()
{
    int i;
    float fade_in;
    float fade_out;

    if (yw2d.inn_mode_cnt < 11)
    {
        fade_in = (yw2d.inn_mode_cnt * 128) / 10.0f;
        fade_out = (1.0f - yw2d.inn_mode_cnt / 10.0f) * 128.0f;

        yw2d.io_a[0] = 128.0f;
        yw2d.io_a[1] = fade_in;
        yw2d.io_a[2] = fade_out;
        yw2d.io_a[3] = fade_out;
        yw2d.io_a[4] = fade_in;

        PlayerStatusYW(yw2d.io_a[2]);
        DspTopMenu(yw2d.io_x[3], yw2d.io_y[3], yw2d.io_a[3], 0);
        DspPlayData(yw2d.io_x[2], yw2d.io_y[2], yw2d.io_a[2], 1);

        if (yw2d.inn_mode_cnt == 10)
        {
            yw2d.pad_lock = 0;
        }
    }
    else if (yw2d.out_mode_cnt != 0)
    {
        fade_in = (1.0f - (yw2d.out_mode_cnt - 1) / 10.0f) * 128.0f;
        fade_out = ((yw2d.out_mode_cnt - 1) * 128) / 10.0f;

        yw2d.io_a[0] = 128.0f;
        yw2d.io_a[1] = fade_out;
        yw2d.io_a[2] = fade_in;
        yw2d.io_a[3] = fade_in;
        yw2d.io_a[4] = fade_out;

        PlayerStatusYW(yw2d.io_a[2]);
        DspTopMenu(yw2d.io_x[3], yw2d.io_y[3], yw2d.io_a[3], 0);
        DspPlayData(yw2d.io_x[2], yw2d.io_y[2], yw2d.io_a[2], 1);

        yw2d.pad_lock = 1;

        if (yw2d.out_mode_cnt == 1)
        {
            ig_menu_wrk.mode = IGMENU_MODE_SLCT;

            IngameMenuModeSlctDispInit();

            yw2d.menu_io_flg = 0;
            yw2d.pad_lock = 0;
        }
    }
    else
    {
        for (i = 0; i < 7; i++)
        {
            XYAdefaultYW(i);
        }
    }
}

static void FileModeInOut2()
{
    if (yw2d.inn_mode_cnt < 11)
    {
        DspPlayData(yw2d.io_x[2], yw2d.io_y[2], yw2d.io_a[2], 2);
    }
    else if (yw2d.out_mode_cnt != 0)
    {
        DspPlayData(yw2d.io_x[2], yw2d.io_y[2], yw2d.io_a[2], 2);
    }
}

static void DspFileOut(short int pos_x, short int pos_y, short int alpha)
{
    DspKindTab(pos_x - 15, pos_y,alpha);
    DspFile2D2(pos_x - 15, pos_y,alpha);
    DspFileLst(pos_x,pos_y, alpha);
    DspMsgWndw(pos_x,pos_y, alpha);
}

static void DspKindTab(short int pos_x, short int pos_y, short int alpha)
{
    int rgb;

    WakuWaku(pos_x - 2, pos_y + 0x94, alpha, 4, 0x2d);

    PutSpriteYW(FIL_SEL_MNU_REPORT, FIL_SEL_MEU_PHOT, pos_x + 5, pos_y - 3, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    if (menu_file.mod == 0)
    {
        FlashStarYW(flsh, 64, 96, 90, 0);

        CLRtoRGB(&rgb, flsh[0].alpha, flsh[0].alpha, flsh[0].alpha);
    }
    else
    {
        FlashStarYW(flsh, 64, 96, 90, 0xff);

        CLRtoRGB(&rgb, flsh[0].alpha, flsh[0].alpha, flsh[0].alpha);
    }

    CmnCursol(pos_x + 47, (pos_y + 180) + ig_menu_wrk.csr[1] * 38,  122, 30, flsh[0].alpha, alpha, 0x23000);
}

static void DspFile2D(short int pos_x, short int pos_y, short int alpha)
{
    int i;
    char add;

    add = alpha / 30.0f;

    for (i = 0; i < 4; i++)
    {
        if (i == ig_menu_wrk.csr[1])
        {
            if (menu_file.pic_alp[i] + add < alpha)
            {
                menu_file.pic_alp[i] += add;
            }
            else
            {
                menu_file.pic_alp[i] = alpha;
            }
        }
        else
        {
            if (menu_file.pic_alp[i] - add > 0)
            {
                menu_file.pic_alp[i] -= add;
            }
            else
            {
                menu_file.pic_alp[i] = 0;
            }
        }
    }

    PutSpriteYW(FIL_MEDIA_05_A, FIL_MEDIA_05_B, pos_x, pos_y, 0.0f, 0x808080, menu_file.pic_alp[0], 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(FIL_MEDIA_01, FIL_MEDIA_01, pos_x, pos_y, 0.0f, 0x808080, menu_file.pic_alp[1], 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(FIL_MEDIA_06_A, FIL_END, pos_x, pos_y, 0.0f, 0x808080, menu_file.pic_alp[2], 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(FIL_MEDIA_02, FIL_MEDIA_02, pos_x, pos_y, 0.0f, 0x808080, menu_file.pic_alp[3], 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(FIL_SEL_LENS, FIL_SEL_LENS, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
}

static void DspFile2D2(short int pos_x, short int pos_y, short int alpha)
{
    short int no;
    u_short alp_add;

    if (file_kind[ig_menu_wrk.csr[1]] != 0xff)
    {
        alp_add = alpha / 30.0f;

        if (menu_file.mod == 0)
        {
            if (menu_file.alp_bak - alp_add > 0)
            {
                menu_file.alp_bak -= alp_add;
            }
            else
            {
                menu_file.alp_bak = 0;
            }

            if (menu_file.alp_now - alp_add > 0)
            {
                menu_file.alp_now -= alp_add;
            }
            else
            {
                menu_file.can_dsp = 0xff;
                menu_file.tex_now = 0;
                menu_file.tex_bak = 0;
            }
        }
        else if (menu_file.mod == 1)
        {
            if (file_kind[ig_menu_wrk.csr[1]] != 1)
            {
                no = menu_dsp_file[ig_menu_wrk.csr[2]];

                menu_file.csr_now = map_file_dat[file_kind[ig_menu_wrk.csr[1]]][no].dsp_pct;

                if (menu_file.csr_now != menu_file.can_dsp)
                {
                    if (menu_file.load_id == -1)
                    {
                        menu_file.loading = menu_file.csr_now;
                        menu_file.load_id = LoadItem2D(4, menu_file.loading);
                        menu_file.dsp_flg = 1;
                    }
                }
                else
                {
                    if (menu_file.alp_now == alpha && menu_file.dsp_flg != 0)
                    {
                        menu_file.tex_bak = menu_file.tex_now;
                        menu_file.tex_now = VramItem2D(menu_file.f_mode, 2);
                        menu_file.f_mode = 1 - menu_file.f_mode;
                        menu_file.alp_now = 0;
                        menu_file.dsp_flg = 0;
                    }
                }

                if (IsLoadEnd(menu_file.load_id) != 0)
                {
                    menu_file.load_id = -1;
                    menu_file.can_dsp = menu_file.loading;
                }

                if (menu_file.alp_now + alp_add < alpha)
                {
                    menu_file.alp_now += alp_add;
                }
                else
                {
                    menu_file.alp_now = alpha;
                }

                menu_file.alp_bak = alpha - menu_file.alp_now;
            }

            DspItem2D(menu_file.tex_now, pos_x - 365, pos_y - 92, 0x808080, menu_file.alp_now, 0.4f, 0.4f, 0x32);
            DspItem2D(menu_file.tex_bak, pos_x - 365, pos_y - 92, 0x808080, menu_file.alp_bak, 0.4f, 0.4f, 0x32);
        }
    }
    else
    {
        if (menu_dsp_file[ig_menu_wrk.csr[2]] < 50)
        {
            no = menu_dsp_file[ig_menu_wrk.csr[2]];

            menu_file.csr_now = map_file_dat[1][no].dsp_pct;
        }
        else
        {
            no = menu_dsp_file[ig_menu_wrk.csr[2]] - 50;

            menu_file.csr_now = map_file_dat[3][no].dsp_pct;
        }
    }

    PutSpriteYW(FIL_SEL_LENS, FIL_SEL_LENS, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
}

static void DspFileLst(short int pos_x, short int pos_y, short int alpha)
{
    int i;
    short int no;

    CmnWindow(8, pos_x, pos_y, alpha, 0x80);

    PutSpriteYW(FIL_SCRBAR_BASE_U, FIL_SCRBAR_BASE_U, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(FIL_SCRBAR_BASE_D, FIL_SCRBAR_BASE_D, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(FIL_SCRBAR_BASE_M, FIL_SCRBAR_BASE_M, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 26.2f, 0, 0xff, 1, 0, 0);

    SideBar(ig_menu_wrk.file_num, 8, 281, menu_file.lst_top, 0x199, alpha, 0, 0);

    SideTri(ig_menu_wrk.file_num, 8, 407, alpha);

    if (file_kind[ig_menu_wrk.csr[1]] == 0xff)
    {
        DispCaption(12, alpha);

        return;
    }

    for (i = 0; i < 8; i++)
    {
        if (menu_dsp_file[i + menu_file.lst_top] == -1) break; // must be on the same line without braces!!

        if (file_kind[ig_menu_wrk.csr[1]] != 1)
        {
            PutStringYW(file_kind[ig_menu_wrk.csr[1]] + 10, menu_dsp_file[i + menu_file.lst_top], pos_x + 266, i * 34 + 48 + pos_y, 0x808080, alpha, 0x23000, 0);

            if (poss_file[file_kind[ig_menu_wrk.csr[1]]][menu_dsp_file[i + menu_file.lst_top]] == 2)
            {
                PutSpriteYW(FIL_SEL_FNT_SHINKI, FIL_SEL_FNT_SHINKI, pos_x, i * 34 + pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            }
        }
        else
        {
            if (menu_dsp_file[i + menu_file.lst_top] < 50)
            {
                no = menu_dsp_file[i + menu_file.lst_top];

                PutStringYW(11, no, pos_x + 266, i * 34 + 48 + pos_y , 0x808080, alpha, 0x23000, 0);

                if (poss_file[1][no] == 2)
                {
                    PutSpriteYW(FIL_SEL_FNT_SHINKI, FIL_SEL_FNT_SHINKI, pos_x, i * 34 + pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                }
            }
            else
            {
                no = menu_dsp_file[i + menu_file.lst_top] - 50;

                PutStringYW(24, file2item[no], pos_x + 266, i * 34 + 48 + pos_y, 0x808080, alpha, 0x23000, 0);

                if (poss_file[3][no] == 2)
                {
                    PutSpriteYW(FIL_SEL_FNT_SHINKI, FIL_SEL_FNT_SHINKI, pos_x, i * 34 + pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                }
            }
        }
    }

    if (ig_menu_wrk.file_num != 0)
    {
        if (menu_file.mod == 1)
        {
            FlashStarYW(&flsh[1], 64, 96, 90, 0);

            CmnCursol(pos_x + 257, pos_y + 42 + (ig_menu_wrk.csr[2] - menu_file.lst_top) * 34, 334, 35, flsh[1].alpha, alpha, 0x28000);
        }
        else
        {
            FlashStarYW(flsh + 1, 64, 96, 90, 0xff);
        }
    }

    DispCaption(12, alpha);
}

void SideBar(u_char hav_now, u_char dsp_max, u_short mov_lng, u_char lst_top, u_short chr_top, u_char alp, short int dx, short int dy)
{
    u_char csr_top;
    u_short now_lng;
    short pos_y;

    if (hav_now != 0)
    {
        if (dsp_max < hav_now)
        {
            csr_top = hav_now - dsp_max;
            now_lng = (float)(mov_lng * dsp_max) / hav_now;
        }
        else
        {
            csr_top = 0;
            now_lng = mov_lng;
        }

        pos_y = dy + (int)(((float)(mov_lng - now_lng) / csr_top) * lst_top);

        PutBarScale(dx, pos_y, now_lng, alp, chr_top);
    }
}

static void PutBarScale(short int pos_x, short int pos_y, u_short now_lng, u_char alp, u_short chr_top)
{
    int i;
    float scl_px;
    float scl_py;
    float mdl_lng;
    float mdl_scl;
    DISP_SPRT ds;

    scl_px = spr_dat[chr_top + 2].x;
    scl_py = spr_dat[chr_top + 2].y;

    mdl_scl = (now_lng - (spr_dat[chr_top].h + spr_dat[chr_top + 1].h)) / (float)spr_dat[chr_top + 2].h;
    mdl_lng = mdl_scl * spr_dat[chr_top + 2].h;

    for (i = 0; i < 3; i++)
    {
        CopySprDToSpr(&ds, &spr_dat[chr_top + i]);

        if (i == 0)
        {
            ds.x += pos_x;
            ds.y += pos_y;
        }
        else if (i == 1)
        {
            ds.x += pos_x;
            ds.y = ds.y + pos_y + mdl_lng;
        }
        else if (i == 2)
        {
            ds.x += pos_x;
            ds.y = ds.y + pos_y / mdl_scl;

            ds.scw = 1.0f;
            ds.sch = mdl_scl;

            ds.csx = scl_px;
            ds.csy = scl_py;
        }

        ds.alpha = alp;

        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_NEAREST, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

        DispSprD(&ds);
    }
}

static void SideTri(u_char hav_now, u_char dsp_max, u_short chr_top, short int alpha)
{
    int rgb_l;
    int rgb_r;

    if (menu_file.mod != 3)
    {
        if (dsp_max < hav_now)
        {
            FlashStarYW(&flsh[2], 64, 192, 60, 0);
            CLRtoRGB(&rgb_r, flsh[2].alpha, flsh[2].alpha, flsh[2].alpha);

            rgb_l = rgb_r;
        }
        else
        {
            FlashStarYW(&flsh[2], 64, 192, 60, 0xff);
            CLRtoRGB(&rgb_r, flsh[2].alpha, flsh[2].alpha, flsh[2].alpha);

            rgb_l = rgb_r;
        }

        PutSpriteYW(chr_top, chr_top, 0.0f, 0.0f, 0.0f, rgb_l, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        PutSpriteYW(chr_top + FND_FRAME_R, chr_top + FND_FRAME_R, 0.0f, 0.0f, 0.0f, rgb_r, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    }
    else
    {
        if (menu_file.dtl != 3)
        {
            if (dsp_max >= 2)
            {
                FlashStarYW(&flsh[3], 64, 192, 60, 0);
                CLRtoRGB(&rgb_r, flsh[3].alpha, flsh[3].alpha, flsh[3].alpha);

                rgb_l = rgb_r;

                if (hav_now > 1)
                {
                    PutSpriteYW(chr_top, chr_top, 0.0f, 0.0f, 0.0f, rgb_l, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                }

                if (dsp_max > hav_now)
                {
                    PutSpriteYW(chr_top + FND_FRAME_R, chr_top + FND_FRAME_R, 0.0f, 0.0f, 0.0f, rgb_r, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
                }
            }
            else
            {
                FlashStarYW(flsh + 3, 64, 192, 60, 0xff);
                CLRtoRGB(&rgb_r, flsh[3].alpha, flsh[3].alpha, flsh[3].alpha);
            }
        }
        else if (hav_now >= 2)
        {
            FlashStarYW(&flsh[3], 64, 192, 60, 0);
            CLRtoRGB(&rgb_r, flsh[3].alpha, flsh[3].alpha, flsh[3].alpha);

            rgb_l = rgb_r;

            PutSpriteYW(chr_top, chr_top, 0.0f, 0.0f, 0.0f, rgb_l, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
            PutSpriteYW(chr_top + FND_FRAME_R, chr_top + FND_FRAME_R, 0.0f, 0.0f, 0.0f, rgb_r, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
        }
        else
        {
            FlashStarYW(&flsh[3], 64, 192, 60, 0xff);
        }
    }
}

static void DspMsgWndw(short int pos_x, short int pos_y, short int alpha)
{
    short int no;

    DrawMessageBox(0x1e000, pos_x + 26, pos_y + 340, 588.0f, 80.0f, alpha);

    if (file_kind[ig_menu_wrk.csr[1]] != 0xff)
    {
        switch (menu_file.mod)
        {
        case 0:
            PutStringYW(21, file_kind[ig_menu_wrk.csr[1]] + 4, pos_x + 48, pos_y + 356, 0x808080, alpha, 0x19000, 0);
        break;
        case 1:
            if (file_kind[ig_menu_wrk.csr[1]] != 1)
            {
                PutStringYW(file_kind[ig_menu_wrk.csr[1]] + 14, menu_dsp_file[ig_menu_wrk.csr[2]], pos_x + 48, pos_y + 356, 0x808080, alpha, 0x19000, 0);
            }
            else if (menu_dsp_file[ig_menu_wrk.csr[2]] < 50)
            {
                no = menu_dsp_file[ig_menu_wrk.csr[2]];

                PutStringYW(15, no, pos_x + 48, pos_y + 356, 0x808080, alpha, 0x19000, 0);
            }
            else
            {
                no = menu_dsp_file[ig_menu_wrk.csr[2]] - 50;

                PutStringYW(22, file2item[no], pos_x + 48, pos_y + 356, 0x808080, alpha, 0x19000, 0);
            }
        break;
        case 2:
            PutStringYW(21, 0, pos_x + 48, pos_y + 356, 0x808080, alpha, 0x19000, 0);
        break;
        }
    }
    else
    {
        PutStringYW(21, 8, pos_x + 48, pos_y + 356, 0x808080, alpha, 0x19000, 0);
    }
}

static void DspFileDtl(short int pos_x, short int pos_y, short int alpha)
{
    if (menu_file.dtl != 3)
    {
        DspFileBook(pos_x, pos_y, alpha);
    }
    else
    {
        DspFilePhot(pos_x, pos_y, alpha);
    }
}

static void DspFileBook(short int pos_x, short int pos_y, short int alpha)
{
    PolySquareYW(55.0f, 91.0f, 530, 328, 0, alpha * 0.5f, 1.0f, 1.0f, 0x1e000, 0, 0, 0);

    DspFileCmmn(pos_x, pos_y, alpha);

    CmnWindow(10, pos_x, pos_y, alpha, 0x80);

    menu_file.page_all = ChkPageYW(file_kind[ig_menu_wrk.csr[1]] + 18, menu_dsp_file[ig_menu_wrk.csr[2]]);

    PageInfo(menu_file.open_page, menu_file.page_all, pos_x, pos_y - 13, alpha);

    SideTri(menu_file.open_page, menu_file.page_all, 539, alpha);

    if (ig_menu_wrk.file_num != 0)
    {
        PutStringYW(file_kind[ig_menu_wrk.csr[1]] + 10, menu_dsp_file[ig_menu_wrk.csr[2]], 235, 35, 0x808080, 0x80, 0x19000, 0);

        PutPage2YW(file_kind[ig_menu_wrk.csr[1]] + 18, menu_dsp_file[ig_menu_wrk.csr[2]], menu_file.open_page, 90, 110, 0x808080, 0x80, 0x19000);
    }

    DispCaption(13, alpha);
}

static void DspFilePhot(short int pos_x, short int pos_y, short int alpha)
{
    short int no;

    DspFileCmmn(pos_x, pos_y, alpha);

    PutSpriteYW(FIL_PHT_FRAME_LFT1, FIL_PHT_FRAME_RGT3, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    PutSpriteYW(FIL_PHT_FRAME_EMBS_LUV1, FIL_PHT_FRAME_EMBS_RDH4, pos_x, pos_y, 0.0f, 0x808080, alpha, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);

    SideTri(ig_menu_wrk.file_num, 8, 539, alpha);

    if (file_kind[ig_menu_wrk.csr[1]] != 1)
    {
        PutStringYW(file_kind[ig_menu_wrk.csr[1]] + 10, menu_dsp_file[ig_menu_wrk.csr[2]], 235, 35, 0x808080, 0x80, 0x19000, 0);
    }
    else if (menu_dsp_file[ig_menu_wrk.csr[2]] < 50)
    {
        no = menu_dsp_file[ig_menu_wrk.csr[2]];

        PutStringYW(11, no, 235, 35, 0x808080, 0x80, 0x19000, 0);
    }
    else
    {
        no = menu_dsp_file[ig_menu_wrk.csr[2]] - 50;

        PutStringYW(0x18, file2item[no], 235, 35, 0x808080, 0x80, 0x19000, 0);
    }

    PhotDispCtrl(alpha);

    DispCaption(14, alpha);
}

static void PhotDispCtrl(u_char alp)
{
    int i;
    int j;
    short int no;
    short int wk_no;
    char tate_yoko;
    signed char box_no;
    signed char lod_chk;
    u_int mem_ofs;
    SPRITE_DATA dmy;
    DSP_BOX dsp_box[3];
    DSP_PHT dsp_pht[6] = {
        { .px = 105, .py = 262, .sx = 78,  .sy =  52, },
        { .px = 320, .py = 261, .sx = 288, .sy = 192, },
        { .px = 535, .py = 262, .sx = 78,  .sy =  52, },
        { .px = 105, .py = 262, .sx = 52,  .sy =  78, },
        { .px = 320, .py = 261, .sx = 192, .sy = 288, },
        { .px = 535, .py = 262, .sx = 52,  .sy =  78, },
    };
    u_int tim_offset;

    for (i = 0; i < 3; i++)
    {
        mem_box[i].vrm_no = -1;
        dsp_box[i].lod_rq = -1;
        dsp_box[i].mem_no = -1;
    }

    for (i = 0; i < 3; i++)
    {
        dsp_box[i].pht_no = TargetPhot(ig_menu_wrk.csr[2], i);

        if (dsp_box[i].pht_no == -1)
        {
            continue;
        }

        box_no = -1;

        for (j = 0; j < 3; j++)
        {
            if (dsp_box[i].pht_no == mem_box[j].dsp_ok)
            {
                box_no = j;

                break;
            }
        }

        if (box_no != -1)
        {
            mem_box[box_no].vrm_no = i;

            dsp_box[i].mem_no = box_no;
        }
        else
        {
            lod_chk = 0;

            for (j = 0; j < 3; j++)
            {
                if (dsp_box[i].pht_no == mem_box[j].lod_no)
                {
                    lod_chk++;
                }
            }

            if (lod_chk == 0)
            {
                dsp_box[i].lod_rq = dsp_box[i].pht_no;
            }
        }
    }

    for (i = 0; i < 3; i++)
    {
        if (dsp_box[i].pht_no == -1)
        {
            continue;
        }

        if (dsp_box[i].pht_no < 50)
        {
            tate_yoko = 0;
        }
        else
        {
            no = dsp_box[i].pht_no - 50;

            tate_yoko = pht_waku_put[no];
        }

        DspBack2D(dsp_box[i].pht_no, tate_yoko, dsp_pht[i].px, dsp_pht[i].py, dsp_pht[i].sx, dsp_pht[i].sy, alp);
    }

    for (i = 0; i < 3; i++)
    {
        if (dsp_box[i].mem_no == -1)
        {
            continue;
        }

        mem_ofs = dsp_box[i].mem_no * 0x10440;
        tim_offset = dsp_box[i].mem_no * 0x204 + 0x3080;

        mem_box[dsp_box[i].mem_no].tx = LoadTIM2Sub(&dmy, (char *)(LOAD_ADDRESS + mem_ofs), 0, tim_offset);
    }

    if (AllVramTensoOK(3, dsp_box) != 0)
    {
        if (menu_file.pht_alp + 16 < alp)
        {
            menu_file.pht_alp += 16;
        }
        else
        {
            menu_file.pht_alp = alp;
        }

        for (i = 0; i < 3; i++)
        {
            if (dsp_box[i].pht_no == -1)
            {
                continue;
            }

            if (dsp_box[i].pht_no < 50)
            {
                tate_yoko = 0;
            }
            else
            {
                no = dsp_box[i].pht_no - 50;

                tate_yoko = pht_waku_put[no];
            }

            DspPhot2D(mem_box[dsp_box[i].mem_no].tx, tate_yoko, dsp_pht[i].px, dsp_pht[i].py, dsp_pht[i].sx, dsp_pht[i].sy, menu_file.pht_alp);
        }
    }

    for (i = 0; i < 3; i++)
    {
        if (dsp_box[i].pht_no == -1)
        {
            continue;
        }

        if (dsp_box[i].pht_no < 50)
        {
            tate_yoko = 0;
        }
        else
        {
            no = dsp_box[i].pht_no - 50;

            tate_yoko = pht_waku_put[no];
        }

        if (tate_yoko == 0)
        {
            switch (i)
            {
            case 0:
                PolySquareYW(66.0f, 237.0f, 0x4e, 0x32, 0, 1.0f, 1.0f, 1.0f, 0x1000, 0, 0, 0);
                PolySquareYW(64.0f, 235.0f, 0x52, 0x36, 0x808080, alp, 1.0f, 1.0f, 0x3000, 0, 0, 0);
            break;
            case 1:
                PolySquareYW(176.0f, 165.0f, 0x120, 0xbc, 0, 1.0f, 1.0f, 1.0f, 0x1000, 0, 0, 0);
                PolySquareYW(170.0f, 159.0f, 300, 200, 0x808080, alp, 1.0f, 1.0f, 0x3000, 0, 0, 0);
            break;
            case 2:
                PolySquareYW(496.0f, 237.0f, 0x4e, 0x32, 0, 1.0f, 1.0f, 1.0f, 0x1000, 0, 0, 0);
                PolySquareYW(494.0f, 235.0f, 0x52, 0x36, 0x808080, alp, 1.0f, 1.0f, 0x3000, 0, 0, 0);
            break;
            }
        }
        else
        {
            switch (i)
            {
            case 0:
                PolySquareYW(80.0f, 224.0f, 0x32, 0x4e, 0, 1.0f, 1.0f, 1.0f, 0x1000, 0, 0, 0);
                PolySquareYW(78.0f, 222.0f, 0x36, 0x52, 0x808080, alp, 1.0f, 1.0f, 0x3000, 0, 0, 0);
            break;
            case 1:
                PolySquareYW(226.0f, 120.0f, 0xbc, 0x11c, 0, 1.0f, 1.0f, 1.0f, 0x1000, 0, 0, 0);
                PolySquareYW(220.0f, 114.0f, 200, 0x128, 0x808080, alp, 1.0f, 1.0f, 0x3000, 0, 0, 0);
            break;
            case 2:
                PolySquareYW(510.0f, 224.0f, 0x32, 0x4e, 0, 1.0f, 1.0f, 1.0f, 0x1000, 0, 0, 0);
                PolySquareYW(508.0f, 222.0f, 0x36, 0x52, 0x808080, alp, 1.0f, 1.0f, 0x3000, 0, 0, 0);
            break;
            }
        }
    }

    for (i = 0; i < 3; i++)
    {
        if (dsp_box[i].lod_rq == -1)
        {
            continue;
        }

        for (j = 0; j < 3; j++)
        {
            if (mem_box[j].lod_id == -1 && mem_box[j].vrm_no == -1)
            {
                mem_box[j].lod_no = dsp_box[i].lod_rq;
                mem_box[j].dsp_ok = 0xffff;

                mem_ofs = j * 0x10440;

                if (dsp_box[i].lod_rq < 50)
                {
                    wk_no = mem_box[j].lod_no;

                    mem_box[j].lod_id = LoadReq(wk_no + HINT_00_TM2, LOAD_ADDRESS + mem_ofs);
                }
                else
                {
                    wk_no = mem_box[j].lod_no - 50;

                    mem_box[j].lod_id = LoadReq(file2phot[wk_no] + IPHT_00_TM2, LOAD_ADDRESS + mem_ofs);
                }

                break;
            }
        }
    }

    for (i = 0; i < 3; i++)
    {
        if (mem_box[i].lod_id == -1)
        {
            continue;
        }

        if (IsLoadEnd(mem_box[i].lod_id) != 0)
        {
            mem_box[i].dsp_ok = mem_box[i].lod_no;
            mem_box[i].lod_no = 0xffff;
            mem_box[i].lod_id = -1;
        }
    }
}

static char AllVramTensoOK(char pht_num, DSP_BOX *dsp_box)
{
    int i;
    short int dsp_ok;

    dsp_ok = 0;

    for (i = 0; i < 3; i++)
    {
        if (mem_box[i].lod_id == -1)
        {
            dsp_ok++;
        }
    }

    return dsp_ok == pht_num;
}

static short int TargetPhot(short int csr, short int no)
{
    short int pht_no;
    short int load_pht;

    if (ig_menu_wrk.file_num > 1)
    {
        load_pht = csr + no - 1;

        if (load_pht < 0)
        {
            load_pht = ig_menu_wrk.file_num - 1;
        }

        if (ig_menu_wrk.file_num - 1 < load_pht)
        {
            load_pht = 0;
        }

        pht_no = menu_dsp_file[load_pht];
    }
    else if (no == 1)
    {
        pht_no = menu_dsp_file[csr];
    }
    else
    {
        pht_no = -1;
    }

    return pht_no;
}

static u_char DspPhot2D(u_long tex, u_char rt, short int px, short int py, short int sx, short int sy, u_char alp)
{
    DISP_SPRT ds;

    if (tex == 0xffffffffffffffff)
    {
        return 1;
    }

    if (alp == 0)
    {
        return 2;
    }

    CopySprDToSpr(&ds, &spr_dat[287]);

    ds.tex0 = tex;

    ds.w = 288;
    ds.h = 192;

    ds.csx = px;
    ds.csy = py;

    ds.scw = sx / 288.0f;
    ds.sch = sy / 192.0f;

    ds.x = px - 144.0f;
    ds.y = py - 96.0f;

    if (rt != 0)
    {
        ds.rot = -90.0f;
        ds.crx = px;
        ds.cry = py;
    }

    ds.alpha = alp;

    ds.pri = 0x5000;
    ds.z = 0xfffffff - ds.pri;

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);

    return 0;
}

static void DspBack2D(short int pht_no, u_char rt, short int px, short int py, short int sx, short int sy, u_char alp)
{
    if (pht_no == -1)
    {
        return;
    }

    if (rt == 0)
    {
        PolySquareYW(px - sx * 0.5f, py - sy * 0.5f, sx, sy, 0, alp, 1.0f, 1.0f, 0xa000, 0, 0, 0);
    }
    else
    {
        PolySquareYW(px - sy * 0.5f, py - sx * 0.5f, sy, sx, 0, alp, 1.0f, 1.0f, 0xa000, 0, 0, 0);
    }
}

static void DspFileCmmn(short int pos_x, short int pos_y, short int alpha)
{
    CmnWindow(9, pos_x, pos_y, alpha, 0x80);
}

void PageInfo(u_char now, u_char all, short int pos_x, short int pos_y, u_char alpha)
{
    PolySquareYW(pos_x + 272, pos_y + 402, 17, 2, 0x808080, alpha, 1.0f, 1.0f, 0x19000, 0, 0, 0);
    PutNumberYW(0, now, pos_x + 291, pos_y + 390, 1.0f, 1.0f, 0x808080, alpha, 0x19000, 1, 0);
    PutStringYW(21, 1, pos_x + 314, pos_y + 390, 0x808080, alpha, 0x19000, 0);
    PutNumberYW(0, all, pos_x + 327, pos_y + 390, 1.0f, 1.0f, 0x808080, alpha, 0x19000, 1, 0);
    PolySquareYW(pos_x + 353, pos_y + 402, 17, 2, 0x808080, alpha, 1.0f, 1.0f, 0x19000, 0, 0, 0);
}

void DokiDokiLetter(u_char level)
{
    u_char musinon_tbl[6] = { 0, 90, 60, 45, 30, 28 };
    u_short ketuatu_tbl[6] = { 0, 120, 150, 180, 200, 220 };
    static u_char musinon = 0;
    static u_char sinpaku = 0;
    static u_short ketuatu = 0;

    if (5 < level)
    {
        level = 5;
    }

    if (level == 0)
    {
        return;
    }

    if (sinpaku != 0)
    {
        if (sinpaku == 18)
        {
            SeStartFix(0xf, 0, 0x1000, 0x1000, 0x1);
        }

        sinpaku--;

        VibrateRequest2(0, ketuatu);
    }
    else if (musinon == 0)
    {
        musinon = musinon_tbl[level];
        sinpaku = 18;
        ketuatu = ketuatu_tbl[level];
    }
    else
    {
        musinon--;
    }
}

void RelationShip()
{
    switch(file_rel.step)
    {
    case 0:
#ifdef BUILD_EU_VERSION
        file_rel.load_id = LoadReqLanguage(PL_FILE_REL_E_PK2, SPRT_ADDRESS);
        file_rel.load_id = LoadReqLanguage(PL_GLST_E_PK2, 0x1e90000);
#else
        file_rel.load_id = LoadReq(PL_FILE_REL_PK2, SPRT_ADDRESS);
        file_rel.load_id = LoadReq(PL_GLST_PK2, 0x1e90000);
#endif
        file_rel.step = 1;
    break;
    case 1:
        if (IsLoadEnd(file_rel.load_id) != 0)
        {
            file_rel.step = 2;
        }
    break;
    case 2:
        SetSprFile(SPRT_ADDRESS);
        if (TateizFreeSpace() != 0)
        {
            file_rel.step = 3;
#ifdef BUILD_EU_VERSION
            file_rel.load_id = LoadReqLanguage(PL_FILE_E_PK2, 0x1e90000);
            file_rel.load_id = LoadReqLanguage(PL_FNDR_E_PK2, SPRT_ADDRESS);
#else
            file_rel.load_id = LoadReq(PL_FILE_PK2, 0x1e90000);
            file_rel.load_id = LoadReq(PL_FNDR_PK2, SPRT_ADDRESS);
#endif
        }
    break;
    case 3:
        if (IsLoadEnd(file_rel.load_id) != 0)
        {
            file_rel.step = 4;
        }
    break;
    case 4:
        ig_menu_wrk.mode = IGMENU_MODE_FILE;
    break;
    }
}

static char TateizFreeSpace()
{
    u_int i;
    int ret_num;
    DISP_SPRT ds;

    ret_num = 0;

    if (rel_csr.init_switch == 0)
    {
        RelationMapInit();

        rel_csr.init_switch = 1;
    }

    RelAtributeChg();
    RelComAtributeChg();

    if ((rel_csr.attribute & 0x2) == 0)
    {
        ret_num = ButtonManager();

        if (ret_num == 1)
        {
            return 1;
        }
    }

    if ((rel_csr.attribute & 0x1) == 0)
    {
        MoveBeyondData();
        MoveBeyondGroup();
        RelMapScroolY(rel_csr.now_prt);
    }

    RelDspBackGrd();

    for (i = 0; i < 5; i++)
    {
        if (rel_csr.now_prt != &rel_prt[i])
        {
            RelDspPrt(&rel_prt[i]);
        }
        else
        {
            RelDspPrtFla(&rel_prt[i]);
        }
    }

    for (i = 0; i < 32; i++)
    {
        RelationDispDat(&rel_dat[i]);
    }

    if (rel_csr.attribute & 0x1)
    {
        RelationDispMsg(rel_csr.now_dat);
    }

    for (i = 0; i < 81; i++)
    {
        RelDspYajirusi(&rel_yaj[i]);
    }

    RelDspComment();

    if (ret_num == 1)
    {
        return 1;
    }

    return ret_num;
}

void RelationMapInit()
{
    u_int i; // STAB say it's int but to make it work as an int the 29 and 32 must be made unsigned, so changing the type here is cleaner

    for (i = 0; i < 29; i++)
    {
        rel_com[i].attribute = 0x2;
    }

    for (i = 0; i < 32; i++)
    {
        rel_dat[i].attribute = 0x2;
    }

    rel_csr.now_dat = rel_dat;
    rel_csr.now_prt = rel_dat[0].parent_p;
    rel_csr.attribute = 0;
    rel_csr.offx = 0;
    rel_csr.offy = 0;
    rel_csr.mode = 0;
    rel_csr.load_id = -1;

    RelAtributeChg();
}

void RelAtributeChg()
{
    u_int i; // STAB say int but for loop has unsigned logic => index literal is probably unsigned, but making the variable unsigned is cleaner
    u_int j; // STAB say int but for loop has unsigned logic => index literal is probably unsigned, but making the variable unsigned is cleaner
    int k;

    for (i = 0; i < 32; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (exp_table[i][j][0] == -1)
            {
                break;
            }

            if (exp_table[i][j][0] == -2)
            {
                rel_dat[i].attribute = 0;
                rel_dat[rel_dat[i].dat_idx].msg_idx = rel_dat[i].msg_idx;

                while (exp_table[i][j][0] != -1)
                {
                    j++;
                }

                for (k = exp_table[i][j][1]; k > 0; k--)
                {
                    rel_dat[i - k].attribute = 1;
                }

                break;
            }

            if (exp_table[i][j][0] == -4)
            {
                if (exp_table[i][j][1] <= ingame_wrk.msn_no || ingame_wrk.clear_count != 0)
                {
                    rel_dat[i].attribute = 0;
                    rel_dat[rel_dat[i].dat_idx].msg_idx = rel_dat[i].msg_idx;

                    while (exp_table[i][j][0] != -1)
                    {
                        j++;
                    }

                    for (k = exp_table[i][j][1]; k > 0; k--)
                    {
                        rel_dat[i - k].attribute = 1;
                    }

                    break;
                }
            }
            else if (poss_file[exp_table[i][j][0]][exp_table[i][j][1]] == 3)
            {
                rel_dat[i].attribute = 0;
                rel_dat[rel_dat[i].dat_idx].msg_idx = rel_dat[i].msg_idx;

                while (exp_table[i][j][0] != -1)
                {
                    j++;
                }

                for (k = exp_table[i][j][1]; k > 0; k--)
                {
                    rel_dat[i - k].attribute = 1;
                }

                break;
            }
        }
    }
}

void RelComAtributeChg()
{
    u_int i; // STAB say int but for loop has unsigned logic => index literal is probably unsigned, but making the variable unsigned is cleaner
    u_int j; // STAB say int but for loop has unsigned logic => index literal is probably unsigned, but making the variable unsigned is cleaner
    int k;

    for (i = 0; i < 29; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (hum_table[i][j][0] == -1)
            {
                break;
            }

            if (hum_table[i][j][0] == -2)
            {
                rel_com[i].attribute = 0;

                while (hum_table[i][j][0] != -1)
                {
                    j++;
                }

                for (k = hum_table[i][j][1]; k > 0; k--)
                {
                    rel_com[i - k].attribute = 1;
                }

                break;
            }

            if (hum_table[i][j][0] == -4)
            {
                if (hum_table[i][j][1] <= ingame_wrk.msn_no || ingame_wrk.clear_count != 0)
                {
                    rel_com[i].attribute = 0;

                    while (hum_table[i][j][0] != -1)
                    {
                        j++;
                    }

                    for (k = hum_table[i][j][1]; k > 0; k--)
                    {
                        rel_com[i - k].attribute = 1;
                    }

                    break;
                }
            }
            else if (poss_file[hum_table[i][j][0]][hum_table[i][j][1]] == 3)
            {
                rel_com[i].attribute = 0;

                while (hum_table[i][j][0] != -1)
                {
                    j++;
                }

                for (k = hum_table[i][j][1]; k > 0; k--)
                {
                    rel_com[i - k].attribute = 1;
                }

                break;
            }
        }
    }
}

void MoveBeyondData()
{
    if (
        *key_now[2] == 1 ||
        (*key_now[2] > 25 && (*key_now[2] % 5) == 1) ||
        Ana2PadDirCnt(3) == 1 ||
        (Ana2PadDirCnt(3) > 25 && (Ana2PadDirCnt(3) % 5) == 1)
    )
    {
        if (rel_csr.now_dat->left_p != NULL && rel_csr.now_dat->left_p->attribute != 2)
        {
            rel_csr.now_dat = rel_csr.now_dat->left_p;

            SeStartFix(0, 0, 0x1000, 0x1000, 0x1);
        }
    }
    else if (
        *key_now[3] == 1 ||
        (*key_now[3] > 25 && (*key_now[3] % 5) == 1) ||
        Ana2PadDirCnt(1) == 1 ||
        (Ana2PadDirCnt(1) > 25 && (Ana2PadDirCnt(1) % 5) == 1)
    )
    {
        if (rel_csr.now_dat->right_p != NULL && rel_csr.now_dat->right_p->attribute != 2)
        {
            rel_csr.now_dat = rel_csr.now_dat->right_p;

            SeStartFix(0, 0, 0x1000, 0x1000, 0x1);
        }
    }
    else if (
        *key_now[0] == 1 ||
        (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
        Ana2PadDirCnt(0) == 1 ||
        (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
    )
    {
        if (rel_csr.now_dat->up_p != NULL && rel_csr.now_dat->up_p->attribute != 2)
        {
            rel_csr.now_dat = rel_csr.now_dat->up_p;

            SeStartFix(0, 0, 0x1000, 0x1000, 0x1);
        }
    }
    else if (
        *key_now[1] == 1 ||
        (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
        Ana2PadDirCnt(2) == 1 ||
        (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
    )
    {
        if (rel_csr.now_dat->down_p != NULL && rel_csr.now_dat->down_p->attribute != 2)
        {
            rel_csr.now_dat = rel_csr.now_dat->down_p;

            SeStartFix(0, 0, 0x1000, 0x1000, 0x1);
        }
    }
}

void MoveBeyondGroup()
{
    u_int i; // STAB say int but loop upper bound must be an unsigned literal. u_int is cleaner
    RELATION_PRT *temp_prt;

    temp_prt = rel_csr.now_prt;

    if (*key_now[8] == 1)
    {
        while (temp_prt->left_prt->mission_no > ingame_wrk.msn_no && ingame_wrk.clear_count == 0)
        {
            temp_prt = temp_prt->left_prt;
        }

        rel_csr.now_prt = temp_prt->left_prt;

        for (i = 0; i < 4; i++)
        {
            if (rel_csr.now_prt->st_index[i] == -1)
            {
                rel_csr.now_dat = &rel_dat[rel_csr.now_prt->st_index[0]];

                break;
            }

            if (rel_dat[rel_csr.now_prt->st_index[i]].attribute != 2)
            {
                rel_csr.now_dat = &rel_dat[rel_csr.now_prt->st_index[i]];

                break;
            }
        }

        if (i == 4)
        {
            rel_csr.now_dat = &rel_dat[rel_csr.now_prt->st_index[0]];
        }

        rel_csr.attribute |= 2;

        SeStartFix(0, 0, 0x1000, 0x1000, 0x1);
    }
    else if (*key_now[10] == 1)
    {
        while (temp_prt->right_prt->mission_no > ingame_wrk.msn_no && ingame_wrk.clear_count == 0)
        {
            temp_prt = temp_prt->right_prt;
        }

        rel_csr.now_prt = temp_prt->right_prt;

        for (i = 0; i < 4; i++)
        {
            if (rel_csr.now_prt->st_index[i] == -1)
            {
                rel_csr.now_dat = &rel_dat[rel_csr.now_prt->st_index[0]];

                break;
            }

            if (rel_dat[rel_csr.now_prt->st_index[i]].attribute != 2)
            {
                rel_csr.now_dat = &rel_dat[rel_csr.now_prt->st_index[i]];

                break;
            }
        }

        if (i == 4)
        {
            rel_csr.now_dat = &rel_dat[rel_csr.now_prt->st_index[0]];
        }

        rel_csr.attribute |= 2;

        SeStartFix(0, 0, 0x1000, 0x1000, 0x1);
    }
}

void RelMapScroolX(RELATION_DAT *r_dat)
{
    if (rel_csr.offx < r_dat->parent_p->ssd_p->x - 320 + r_dat->parent_p->ssd_p->w / 2)
    {
        rel_csr.offx += 8;

        if (rel_csr.offx >= r_dat->parent_p->ssd_p->x - 320 + r_dat->parent_p->ssd_p->w / 2)
        {
            rel_csr.offx = r_dat->parent_p->ssd_p->x - 320 + r_dat->parent_p->ssd_p->w / 2;
        }

    }
    else if (rel_csr.offx > r_dat->parent_p->ssd_p->x - 320 + r_dat->parent_p->ssd_p->w / 2)
    {
        rel_csr.offx -= 8;

        if (rel_csr.offx <= r_dat->parent_p->ssd_p->x - 320 + r_dat->parent_p->ssd_p->w / 2)
        {
            rel_csr.offx = r_dat->parent_p->ssd_p->x - 320 + r_dat->parent_p->ssd_p->w / 2;
        }
    }
}

void RelMapScroolY(RELATION_PRT *r_prt)
{
    if (rel_csr.offy > 224 - r_prt->csr_y)
    {
        rel_csr.offy -= 8;

        if (rel_csr.offy <= 224 - r_prt->csr_y)
        {
            rel_csr.offy = 224 - r_prt->csr_y;

            rel_csr.attribute &= ~0x2;
        }
    }
    else if (rel_csr.offy < 224 - r_prt->csr_y)
    {
        rel_csr.offy += 8;

        if (rel_csr.offy >= 224 - r_prt->csr_y)
        {
            rel_csr.offy = 224 - r_prt->csr_y;

            rel_csr.attribute &= ~0x2;
        }
    }
    else
    {
        rel_csr.attribute &= ~0x2;
    }
}

int ButtonManager()
{
    int ret_num;

    ret_num = 0;

    if ((*key_now[6] == 1 || *key_now[5] == 1) && rel_csr.now_dat->attribute != 2)
    {
        rel_csr.mode = 0;
        rel_csr.attribute ^= 0x1;

        SeStartFix(1, 0, 0x1000, 0x1000, 1);
    }

    if (*key_now[4] == 1)
    {
        if (rel_csr.attribute & 0x1)
        {
            rel_csr.mode = 0;
            rel_csr.attribute ^= 0x1;
        }
        else
        {
            rel_csr.init_switch = 0;

            ret_num = 1;
        }

        SeStartFix(3, 0, 0x1000, 0x1000, 1);
    }

    return ret_num;
}

void RelationDispDat(RELATION_DAT *r_dat)
{
    SPRT_DAT ssd;
    DISP_SPRT ds;

    if (r_dat->attribute == 0x2)
    {
        return;
    }

    if (rel_csr.now_dat == r_dat)
    {
        ssd = r_dat->fla_p[0];

        ssd.x += r_dat->plate_p->x;
        ssd.y += r_dat->plate_p->y;

        ssd.y += + rel_csr.offy;

        CopySprDToSpr(&ds, &ssd);

        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

        DispSprD(&ds);

        ssd = r_dat->fla_p[1];

        ssd.x += r_dat->plate_p->x;
        ssd.y += r_dat->plate_p->y;

        ssd.y += rel_csr.offy;

        CopySprDToSpr(&ds, &ssd);

        ds.att |= 0x2;

        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

        DispSprD(&ds);
    }

    if (r_dat->attribute != 0x1)
    {
        ssd = *r_dat->ssd_p;

        ssd.x += r_dat->plate_p->x;
        ssd.y += r_dat->plate_p->y;

        ssd.x += rel_csr.offx;
        ssd.y += rel_csr.offy;

        CopySprDToSpr(&ds, &ssd);

        if (r_dat != rel_dat + 27)
        {
            ds.r = 0;
            ds.g = 0;
            ds.b = 0;
        }

        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

        DispSprD(&ds);

        ssd = r_dat->ssd_p[1];

        ssd.x += r_dat->plate_p->x - 1;
        ssd.y += r_dat->plate_p->y;

        ssd.x += rel_csr.offx;
        ssd.y += rel_csr.offy;

        CopySprDToSpr(&ds, &ssd);

        if (r_dat != rel_dat + 27)
        {
            ds.r = 0x0;
            ds.g = 0x0;
            ds.b = 0x0;
        }

        ds.att |= 0x2;

        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

        DispSprD(&ds);

        ssd = *r_dat->fnt_p;

        ssd.x += r_dat->plate_p->x;
        ssd.y += r_dat->plate_p->y;

        ssd.u = r_dat->plate_p->u;
        ssd.v = r_dat->plate_p->v;

        ssd.x += rel_csr.offx;
        ssd.y += rel_csr.offy;

        CopySprDToSpr(&ds, &ssd);

        DispSprD(&ds);

        ssd = spr_dat[r_dat->plate_p->tbl_no];

        ssd.x += r_dat->plate_p->x;
        ssd.y += r_dat->plate_p->y;

        ssd.u = r_dat->plate_p->exp_u;
        ssd.v = r_dat->plate_p->exp_v;

        ssd.x += rel_csr.offx;
        ssd.y += rel_csr.offy;

        CopySprDToSpr(&ds, &ssd);

        DispSprD(&ds);
    }
}

void RelDspYajirusi(RELATION_YAJI *r_yaj)
{
    float temp_x1;
    float temp_y1;
    float temp_x2;
    float temp_y2;
    float temp_x3;
    float temp_y3;
    u_char yaj_r;
    int yaj_pri;
    float f14;
    float f15;

    if (rel_com[r_yaj->com_idx].attribute == 0x2)
    {
        return;
    }

    if (
        rel_csr.now_dat == rel_dat + rel_com[r_yaj->com_idx].dat_idx[0] ||
        rel_csr.now_dat == rel_dat + rel_com[r_yaj->com_idx].dat_idx[1]
    )
    {
        yaj_r = 0x80;
        yaj_pri = 0x14000;

#ifdef MATCHING_DECOMP
        if (*key_now[3] != 1 && *key_now[2] != 1 && *key_now[2] != 1)
        {
            char c = ~c; // dead code
        }
#endif
    }
    else
    {
        yaj_r = 0;
        yaj_pri = 0x19000;

#ifdef MATCHING_DECOMP
        if (*key_now[3] != 1 && *key_now[2] != 1 && *key_now[2] != 1)
        {
            char c = ~c; // dead code
        }
#endif
    }

    if (r_yaj->attribute & 0x1)
    {
        temp_x1 = r_yaj->x1 - 320;
        temp_y1 = r_yaj->y1 + rel_csr.offy - 224;
        f14 = temp_x1 + r_yaj->len;
        f15 = temp_y1 + 2.0f;

        SetSquareS(yaj_pri, temp_x1, temp_y1, f14, f15, yaj_r, 0, 0, 0x80);
    }

    if (r_yaj->attribute & 0x2)
    {
        temp_x1 = r_yaj->x1 - 320;
        temp_y1 = r_yaj->y1 + rel_csr.offy - 224;
        f14 = temp_x1 + 2.0f;
        f15 = temp_y1 + r_yaj->len;

        SetSquareS(yaj_pri, temp_x1, temp_y1, f14, f15, yaj_r, 0, 0, 0x80);
    }

    if (r_yaj->attribute & (0x20 | 0x10 | 0x8 | 0x4))
    {
        if (r_yaj->attribute & 0x4)
        {
            temp_x1 = r_yaj->x1 - 325;
            temp_y1 = r_yaj->y1 + rel_csr.offy - 224;
            temp_x2 = temp_x1 + 9.0f + 1.0f;
            temp_y2 = r_yaj->y1 + rel_csr.offy - 224;
            temp_x3 = temp_x1 + 4.0f;
            temp_y3 = temp_y1 - 14.0f - 1.0f;
        }

        if (r_yaj->attribute & 0x8)
        {
            temp_x1 = r_yaj->x1 - 325;
            temp_y1 = r_yaj->y1 + rel_csr.offy - 224;
            temp_x2 = temp_x1 + 9.0f + 1.0f;
            temp_y2 = r_yaj->y1 + rel_csr.offy - 224;
            temp_x3 = temp_x1 + 4.0f;
            temp_y3 = temp_y1 + 14.0f + 1.0f;
        }

        if (r_yaj->attribute & 0x10)
        {
            temp_x1 = r_yaj->x1 - 320;
            temp_y1 = r_yaj->y1 + rel_csr.offy - 228;
            temp_x2 = r_yaj->x1 - 320;
            temp_y2 = temp_y1 + 9.0f;
            temp_x3 = temp_x1 - 14.0f;
            temp_y3 = r_yaj->y1 + rel_csr.offy - 224;
        }

        if (r_yaj->attribute & 0x20)
        {
            temp_x1 = r_yaj->x1 - 320;
            temp_y1 = r_yaj->y1 + rel_csr.offy - 228;
            temp_x2 = r_yaj->x1 - 320;
            temp_y2 = temp_y1 + 9.0f;
            temp_x3 = temp_x1 + 14.0f;
            temp_y3 = r_yaj->y1 + rel_csr.offy - 224;
        }

        SetTriangle(yaj_pri, temp_x1, temp_y1, temp_x2, temp_y2, temp_x3, temp_y3, yaj_r, 0, 0, 0x80);
    }
}

void RelDspComment()
{
    int i;
    int j;
    int l;
    int disp_flg;
    SPRT_DAT ssd;
    DISP_SPRT ds;

    l = 0;

    disp_flg = 0;

    for (i = 0; i < 29U; i++)
    {
        if (rel_com[i].attribute != 1 && rel_com[i].attribute != 2)
        {
            for (j = 0; j < 2; j++)
            {
                if (rel_com[i].dat_idx[j] != -1 && rel_csr.now_dat == rel_dat + rel_com[i].dat_idx[j])
                {
                    disp_flg = 1;
                }
            }

            if (disp_flg == 1)
            {
                while (rel_com[i].ssd_p[l] != NULL)
                {
                    ssd = *rel_com[i].ssd_p[l];

#ifdef BUILD_EU_VERSION
                    ssd.x += rel_csr.offx + rel_com_pos_adj[i][sys_wrk.language][0];
                    ssd.y += rel_csr.offy + rel_com_pos_adj[i][sys_wrk.language][1];
#else
                    ssd.x += rel_csr.offx;
                    ssd.y += rel_csr.offy;
#endif

                    ssd.alpha = 0x80;

                    ssd.pri = 0x18000;

                    CopySprDToSpr(&ds, &ssd);

                    if (flip_table[rel_com[i].flp_num[l]] != 0)
                    {
                        ds.crx = ssd.x;
                        ds.cry = ssd.y;
                        ds.rot = 90.0f;
                    }

                    DispSprD(&ds);

                    l++;

                    if (l >= 4U)
                    {
                        break;
                    }
                }

                l = 0;
            }
            else
            {

                while (rel_com[i].ssd_p[l] != NULL)
                {
                    ssd = *rel_com[i].ssd_p[l];

#ifdef BUILD_EU_VERSION
                    ssd.x += rel_csr.offx + rel_com_pos_adj[i][sys_wrk.language][0];
                    ssd.y += rel_csr.offy + rel_com_pos_adj[i][sys_wrk.language][1];
#else
                    ssd.x += rel_csr.offx;
                    ssd.y += rel_csr.offy;
#endif

                    ssd.alpha = 0x30;

                    ssd.pri = 0x19000;

                    CopySprDToSpr(&ds, &ssd);

                    ds.r = 0xff;
                    ds.g = 0xff;
                    ds.b = 0xff;

                    if (flip_table[rel_com[i].flp_num[l]] != 0)
                    {
                        ds.crx = ssd.x;
                        ds.cry = ssd.y;
                        ds.rot = 90.0f;
                    }

                    DispSprD(&ds);

                    l++;

                    if (l >= 4U)
                    {
                        break;
                    }
                }

                l = 0;
            }

            disp_flg = 0;
        }
    }
}

void RelDspPrtFla(RELATION_PRT *r_prt)
{
    int i;
    int j;
    int scl_flg;
    float scl_tmpx;
    float scl_tmpy;
    SPRT_DAT ssd;
    DISP_SPRT ds;

    j = 0;

    if (r_prt->attribute == 1)
    {
        return;
    }

    if (ingame_wrk.msn_no < r_prt->mission_no && ingame_wrk.clear_count == 0)
    {
        return;
    }

    for (i = r_prt->start_no; i <= r_prt->end_no; i++)
    {
        scl_flg = 0;

        scl_tmpy = 1.0f;
        scl_tmpx = 1.0f;

        ssd = spr_dat[i];

        ssd.x += rel_csr.offx;
        ssd.y += rel_csr.offy;

        CopySprDToSpr(&ds, &ssd);

        ds.att |= flip_table[i];

        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

        if (r_prt->wk_scl[j][0] != 0)
        {
            scl_tmpx = (r_prt->wk_scl[j][0] + (float)r_prt->ssd_p->w) / r_prt->ssd_p->w;

            scl_flg = 1;
        }

        if (r_prt->wk_scl[j][1] != 0)
        {
            scl_tmpy = (r_prt->wk_scl[j][1] + (float)r_prt->ssd_p->h) / r_prt->ssd_p->h;

            scl_flg = 1;
        }

        if (scl_flg)
        {
            ds.csx = (int)(ssd.x + ssd.w / 2);
            ds.csy = (int)(ssd.y + ssd.h / 2);

            ds.scw = scl_tmpx;
            ds.sch = scl_tmpy;
        }

        DispSprD(&ds);

        j++;
    }
}

void RelDspPrt(RELATION_PRT *r_prt)
{
    int i;
    float temp_x1;
    float temp_y1;
    float temp_x4;
    float temp_y4;

    if (r_prt->attribute == 1)
    {
        return;
    }

    if (r_prt->mission_no <= ingame_wrk.msn_no || ingame_wrk.clear_count != 0)
    {
        for (i = 0; i < 2U; i++)
        {
            if (r_prt->wk_pos[i][0] != r_prt->wk_pos[i][2])
            {
                temp_x1 = (r_prt->wk_pos[i][0] + -0x140);
                temp_y1 = (r_prt->wk_pos[i][1] + rel_csr.offy + -0xe0);
                temp_x4 = (r_prt->wk_pos[i][2] + -0x140);
                temp_y4 = (r_prt->wk_pos[i][3] + rel_csr.offy + -0xe0);
                SetSquareS(0x24000, temp_x1 + 4.0f, temp_y1 + 4.0f, temp_x4 - 4.0f, temp_y4 - 4.0f, 0, 0, 0, 1);
                SetSquareS(0x25000, temp_x1, temp_y1, temp_x4, temp_y4, 0, 0, 0, 0x7f);
            }
        }
    }
}

void RelDspBackGrd()
{
    int i;
    int j;
    u_char alp_rate;
    SPRT_DAT ssd;
    SPRT_DAT ssd_fnt = {
        .tex0 = SCE_GS_SET_TEX0_1(0x2944, 4, SCE_GS_PSMT4, 8, 7, 1, SCE_GS_MODULATE, 0x2984, SCE_GS_PSMCT32, 0, 0, 1),
        .u = 1,
        .v = 1,
        .w = 126,
        .h = 95,
        .x = 490,
        .y = 122,
        .pri = 0x1e000,
        .alpha = 0x80,
    };
    SPRT_DAT ssd_sakka = {
        .tex0 = SCE_GS_SET_TEX0_1(0x2781, 4, SCE_GS_PSMT4, 8, 7, 1, SCE_GS_MODULATE, 0x27c1, SCE_GS_PSMCT32, 0, 0, 1),
       .u = 1,
       .v = 1,
       .w = 164,
       .h = 24,
       .y = 0x62,
       .x = 30,
       .pri = 0x1e000,
       .alpha = 0x80,
    };
    SPRT_DAT ssd_himuro = {
       .tex0 = SCE_GS_SET_TEX0_1(0x2781, 4, SCE_GS_PSMT4, 8, 7, 1, SCE_GS_MODULATE, 0x27c1, SCE_GS_PSMCT32, 0, 0, 1),
       .u = 1,
       .v = 0x1b,
       .w = 164,
       .h = 24,
       .y = 338,
       .x = 26,
       .pri = 0x1e000,
       .alpha = 0x80,
    };
    SPRT_DAT ssd_hina = {
       .tex0 = SCE_GS_SET_TEX0_1(0x2781, 4, SCE_GS_PSMT4, 8, 7, 1, SCE_GS_MODULATE, 0x27c1, SCE_GS_PSMCT32, 0, 0, 1),
       .u = 1,
       .v = 53,
       .w = 164,
       .h = 24,
       .y = 379,
       .x = 453,
       .pri = 0x1e000,
       .alpha = 0x80,
    };
    SPRT_DAT ssd_muna = {
       .tex0 = SCE_GS_SET_TEX0_1(0x2781, 4, SCE_GS_PSMT4, 8, 7, 1, SCE_GS_MODULATE, 0x27c1, SCE_GS_PSMCT32, 0, 0, 1),
       .u = 1,
       .v = 79,
       .w = 164,
       .h = 24,
       .y = 668,
       .x = 30,
       .pri = 0x1e000,
       .alpha = 0x80,
    };
    DISP_SPRT ds;
    int alpha; // not in STAB

    alpha = 0x64;

#ifdef BUILD_EU_VERSION
    ssd = spr_dat[1020];
#else
    ssd = spr_dat[1019];
#endif

    ssd.y += rel_csr.offy;

    CopySprDToSpr(&ds, &ssd);

    DispSprD(&ds);

#ifdef BUILD_EU_VERSION
    ssd = spr_dat[1021];
#else
    ssd = spr_dat[1020];
#endif

    ssd.y += rel_csr.offy;

    CopySprDToSpr(&ds, &ssd);

    DispSprD(&ds);

    SetSprFile(0x1e90000);

    SetSprFile(SPRT_ADDRESS_2);

    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 6; j++)
        {
            ssd = spr_dat[463];

            ssd.x = ssd.w * j;
            ssd.y = ssd.h * i;
            ssd.y += rel_csr.offy - 50;

            CopySprDToSpr(&ds, &ssd);

            DispSprD(&ds);
        }
    }

    PutSpriteYW(MAP_BACK_MASK1, MAP_BACK_MASK4, 0.0f, 0.0f, 0.0f, 0x808080, 80.0f, 2.6890757f, 2.698795f, 1, 0xff, 1, 0, 0);

    ssd = ssd_sakka;

    ssd.y += rel_csr.offy;

    CopySprDToSpr(&ds, &ssd);

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);

    if (ingame_wrk.msn_no > 1)
    {
        ssd = ssd_muna;

        ssd.y += rel_csr.offy;

        CopySprDToSpr(&ds, &ssd);

        ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

        DispSprD(&ds);
    }

    ssd = ssd_himuro;

    ssd.y += rel_csr.offy;

    CopySprDToSpr(&ds, &ssd);

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);

    ssd = ssd_hina;

    ssd.y += rel_csr.offy;

    CopySprDToSpr(&ds, &ssd);

    ds.tex1 = SCE_GS_SET_TEX1_1(1, 0, SCE_GS_LINEAR, SCE_GS_LINEAR_MIPMAP_LINEAR, 0, 0, 0);

    DispSprD(&ds);

#ifdef BUILD_EU_VERSION
    SetSquareS(0x1e000, 100.0f, rel_csr.offy - 0x80, 310.0f, rel_csr.offy - 13, 0x40, 0x40, 0x40, 0x40);
#else
    SetSquareS(0x1e000, 100.0f, rel_csr.offy - 0x80, 300.0f, rel_csr.offy - 13, 0x40, 0x40, 0x40, 0x40);
#endif

    ssd_fnt.w += 30;

    ssd_fnt.x += -18;
    ssd_fnt.y += rel_csr.offy;

    CopySprDToSpr(&ds, &ssd_fnt);

    DispSprD(&ds);

    SetSprFile(SPRT_ADDRESS_2);

    DrawButtonTex(0x14000, 3, spr_dat[476].x - 45, spr_dat[476].y + rel_csr.offy + 69, 0x64);

    ssd = spr_dat[476];

    ssd.x += -45;
    ssd.y += rel_csr.offy + 97;

    ssd.pri = 0x14000;

    CopySprDToSpr(&ds, &ssd);

    DispSprD(&ds);

    ssd = spr_dat[471];

    ssd.x += -45;
    ssd.y += rel_csr.offy + 97;

    ssd.pri = 0x14000;

    CopySprDToSpr(&ds, &ssd);

    DispSprD(&ds);

    DrawButtonTex(0x14000, 1, spr_dat[476].x - 45, spr_dat[476].y + rel_csr.offy + 43, 0x64);

    ssd = spr_dat[203];

    CopySprDToSpr(&ds, &ssd);

    ds.pri = 0x14000;
    ds.z = 0xfffffff - ds.pri;

    ds.alpha = alpha;

    ds.x = 474.0f;
    ds.y = rel_csr.offy + 96;

    DispSprD(&ds);

    DspMenuTitle(yw2d.io_x[1], yw2d.io_y[1] + rel_csr.offy - 22.0f, 128.0f, 30, 4);

    SetSprFile(SPRT_ADDRESS);
}

void RelationDispMsg(RELATION_DAT *r_dat)
{
    SPRT_SDAT ssd;
    SPRT_SDAT ssd2;
    SPRT_SDAT ssd3;
    SPRT_SDAT ssd4;
    DISP_SQAR dsq;
    DISP_STR ds;
    STR_DAT spev_str;
    short int dsp_offy;

    if (r_dat->dsp_pos != 1)
    {
        dsp_offy = 0;
    }
    else
    {
        dsp_offy = 230;
    }

    spev_str.str = (u_char *)GetIngameMSGAddr(HUMAN_NAME_TBL, name_rbl[r_dat->msg_idx][0]);
    spev_str.pos_x = 80;
    spev_str.pos_y = dsp_offy + 30;
    spev_str.r = 0;
    spev_str.g = 0;
    spev_str.b = 0;
    spev_str.alpha = 0x80;
    spev_str.pri = 0xc;

    CopyStrDToStr(&ds, &spev_str);

    SetMessageV2(&ds);

    spev_str.str = (u_char *)GetIngameMSGAddr(HUMAN_MSG_TBL, name_rbl[r_dat->msg_idx][1]);
    spev_str.pos_x = 240;
    spev_str.pos_y = dsp_offy + 50;
    spev_str.r = 0;
    spev_str.g = 0;
    spev_str.b = 0;
    spev_str.alpha = 0x80;
    spev_str.pri = 0xc;

    CopyStrDToStr(&ds, &spev_str);

    SetMessageV2(&ds);

    switch (rel_csr.mode)
    {
    case 0:
        if (r_dat->tm2_id != -1)
        {
            rel_csr.load_id = LoadReq(r_dat->tm2_id, LOAD_ADDRESS);
            rel_csr.mode = 1;
        }
    break;
    case 1:
        if (IsLoadEnd(rel_csr.load_id) != 0)
        {
            rel_csr.load_id = -1;
            rel_csr.mode = 2;
        }
    break;
    case 2:
        rel_csr.mode = 3;
    break;
    case 3:
        rel_csr.tex_keep = VramItem2D(0, 3);

        PkTm2(rel_csr.tex_keep, 39, dsp_offy + 57, 192, 128, 0x80, 0xd000);
    break;
    }

    SetSprFile(0x1e90000);

#ifdef BUILD_EU_VERSION
    Sheet(814, 20, dsp_offy + 20, 600, 180, 0xf, 0x80);
    Fuchidori(816, 20, dsp_offy + 20, 600, 180, 0xe, 0x80);
#else
    Sheet(813, 20, dsp_offy + 20, 600, 180, 0xf, 0x80);
    Fuchidori(815, 20, dsp_offy + 20, 600, 180, 0xe, 0x80);
#endif

    SetSprFile(SPRT_ADDRESS);
}

void SimpleDispSprtDS(SPRT_DAT *ssd, u_int addr, int sp_no, SPRT_SROT *srot, SPRT_SSCL *sscl, u_char alp_rate)
{
    DISP_SPRT ds;
    SPRT_DAT sd;

    sd.tex0 = ssd->tex0;
    sd.u = ssd->u;
    sd.v = ssd->v;
    sd.w = ssd->w;
    sd.h = ssd->h;
    sd.x = ssd->x;
    sd.y = ssd->y;
    sd.pri = (ssd->pri & 0xff) << 12;

    if (alp_rate == 0xff)
    {
        ds.alphar = SCE_GS_SET_ALPHA_1(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_ZERO, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0);
    }
    else
    {
        sd.alpha = (ssd->alpha * alp_rate) / 100;
    }

    CopySprDToSpr(&ds,&sd);

    if (srot != NULL)
    {
        if (srot->cx != 0x7fff)
        {
            ds.rot = srot->rot;
            ds.crx = srot->cx;
            ds.cry = srot->cy;
        }
        else
        {
            ds.att |= srot->cy;
        }
    }

    if (sscl != NULL)
    {
        ds.scw = sscl->sw / 100.0f;
        ds.sch = sscl->sh / 100.0f;
        ds.csx = sscl->cx;
        ds.csy = sscl->cy;
    }

    DispSprD(&ds);
}

void TestPk2DataSou(long int sendtexaddr)
{
    static int ttest_count = 0;
    SPRT_DAT ssd;

    if (*key_now[8] == 1)
    {
        ttest_count++;
    }

    if (*key_now[9] == 1)
    {
        ttest_count--;
    }

#ifdef BUILD_EU_VERSION
    ssd.tex0 = spr_dat[929].tex0;
#else
    ssd.tex0 = spr_dat[928].tex0;
#endif

    ssd.u = 1;
    ssd.v = 1;

    ssd.w = 680;
    ssd.h = 512;

    ssd.x = 0;
    ssd.y = 0;

    ssd.pri = 0;

    SimpleDispSprtDS(&ssd, (u_int)sendtexaddr, ttest_count, NULL, NULL, 0x64);
}

void DispCaption(int no, u_char alp)
{
    int i;
    u_int pri = 0xa000;
    SPRT_DAT ssd;
    DISP_SPRT ds;
    CAPTION_DATA_SUB *cdsp;
    CAPTION_DATA *cdp;
    CAPTION_LIST *clp;
    DISP_SQAR dq;
    SQAR_DAT sq = {
        .w = 100,
        .h = 100,
        .x = 0,
        .y = 0,
        .pri = 0,
        .r = 0,
        .g = 0,
        .b = 0,
        .alpha = 0x80,
    };

    clp = &caption_list[no];
    cdp = &caption_data[clp->no];

    if (clp->pnl != 0)
    {
        CopySqrDToSqr(&dq, &sq);

        dq.x[0] = clp->x;
        dq.x[2] = clp->x;

        dq.y[0] = clp->y;
        dq.y[1] = clp->y;

        dq.pri = pri;
        dq.z = 0xfffffff - pri;

        dq.x[1] = clp->x + cdp->w;
        dq.x[3] = clp->x + cdp->w;

        dq.y[2] = clp->y + cdp->h;
        dq.y[3] = clp->y + cdp->h;

        dq.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);

        dq.alpha = alp * 2 / 3;

        DispSqrD(&dq);
    }

    for (i = 0; cdp->cds[i].str != 0 && i < 4; i++)
    {
        cdsp = &cdp->cds[i];

        DrawButtonTexNZ(0xa000, cdsp->mark, clp->x + cdsp->x, clp->y + cdsp->y + 1, alp);

        ssd = spr_dat[cdsp->str];

        CopySprDToSpr(&ds, &ssd);

        ds.pri = pri;
        ds.z = 0xfffffff - pri;

        ds.zbuf = SCE_GS_SET_ZBUF_1(0x8c, SCE_GS_PSMCT24, 1);

        ds.alpha = alp;

        ds.x = clp->x + cdsp->x + 24;
        ds.y = clp->y + cdsp->y;

        DispSprD(&ds);

    }
}

enum NAME_PLATE_RABEL NAME_PLATE_RABEL = SAKKA;
enum PARENT_RABEL PARENT_RABEL = SAKKA_PRT;
enum REL_DIRECTION REL_DIRECTION = R_UP;
enum REL_ALPB REL_ALPB = R_A;
