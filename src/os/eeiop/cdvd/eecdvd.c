#include "common.h"
#include "typedefs.h"
#include "eecdvd.h"
#include "main/glob.h"
#include "os/eeiop/eeiop.h"

typedef struct {
    u_char start_pos;
    u_char req_pos;
    u_char com_num;
} CDVD_REQ_STAT;

typedef struct {
    char stat;
} LOAD_FINISH_DATA;

/* sbss */ static CDVD_REQ_STAT cdvd_rstat;
/* bss  */ static LOAD_FINISH_DATA load_finish[32];

static void CdvdInitEe();
static void CdvdInitIop();
static void CdvdInitResetIop();
static int IsLoadEndIop(int id);
static int GetFreeId();

void CdvdInit()
{
    CdvdInitEe();
    CdvdInitIop();
}

void CdvdInitSoftReset()
{
    CdvdInitEe();
    CdvdInitResetIop();
}

static void CdvdInitEe()
{
    int i;
    
    cdvd_rstat.start_pos = 0;
    cdvd_rstat.req_pos = 0;
    cdvd_rstat.com_num = 0;

    for (i = 0; i < 32; i++)
    {
        load_finish[i].stat = 0;
    }
}

static void CdvdInitIop()
{
    SetIopCmdSm(12, 0, 0, 0);
}

static void CdvdInitResetIop()
{
    SetIopCmdSm(12, 1, 0, 0);
}

int LoadReq(int file_no, u_int addr)
{
    IMG_ARRANGEMENT *img_arng;
    
    img_arng = GetImgArrangementP(file_no);

    return LoadReqNSector(file_no, img_arng->start, img_arng->size, addr);
}

u_int LoadReqGetAddr(int file_no, u_int addr, int *id)
{
    IMG_ARRANGEMENT *img_arng;
    u_int ret;
    
    img_arng = GetImgArrangementP(file_no);
    *id = LoadReqNSector(file_no, img_arng->start, img_arng->size, addr);
    ret = addr + img_arng->size;
    
    if (ret % 16)
    {
        ret += 16 - (ret % 16);
    }
    
    return ret;
}

int LoadReqSe(int file_no, u_char se_type)
{
    IMG_ARRANGEMENT *img_arng;
    int ret;
    
    img_arng = GetImgArrangementP(file_no);
    
    ret = GetFreeId();
    
    if (ret != -1)
    {
        SetIopCmdLg(14, file_no, img_arng->start, img_arng->size, se_type, 2, ret, 0);
    }
    
    return ret;
}

int LoadReqNSector(int file_no, int sector, int size, int addr)
{
    int ret;
    
    ret = GetFreeId();
    
    if (ret != -1)
    {
        SetIopCmdLg(14, 0, sector, size, addr, 0, ret, 0);
    }
    
    return ret;
}

int LoadReqNFno(int file_no, int addr)
{
    return -1;
}

int LoadReqBFno(int file_no, int addr)
{
    return -1;
}

u_int LoadReqBFnoGetAddr(int file_no, int addr)
{
    return 0;
}

int IsLoadEndAll()
{
    sys_wrk.sreset_count = 0;
    
    if (cdvd_rstat.com_num != 0)
    {
        return IsLoadEnd((cdvd_rstat.start_pos + cdvd_rstat.com_num - 1) % 32);
    }
        
    return 1;
}

int IsLoadEnd(int id)
{
    sys_wrk.sreset_count = 0;
    return IsLoadEndIop(id);
}

static int IsLoadEndIop(int id)
{
    int i;
    u_char pos;
    
    if (id >= 32U) // this should be just `32`, but with `32` the `4: sltiu` becomes a stli ...
    {
        return 0;
    }
    
    if (id >= cdvd_rstat.req_pos && id < cdvd_rstat.start_pos)
    {
        return 0;
    }
    
    if (cdvd_rstat.com_num == 0)
    {
        return 1;
    }
    
    if (cdvd_rstat.com_num != 0 && load_finish[id].stat == 0)
    {
        pos = cdvd_rstat.start_pos;
        for (i = 0; i < cdvd_rstat.com_num; i++)
        {
            if (pos >= 32) //
            {              // maybe a compiler optimization
                pos -= 32; // artifact for a % 32 increment?
            }              //
            load_finish[pos].stat = 0;
            cdvd_rstat.start_pos = (cdvd_rstat.start_pos + 1) % 32;
            if (pos == id) {
                cdvd_rstat.com_num -= (i + 1);
                break;
            }
            pos++;
        }
        
        return 1;
    }
    
    return 0;
}

void LoadEndFlgRenew()
{
    int i;

    for (i = 0; i < 32; i++)
    {
        switch (load_finish[i].stat)
        {
        case 0:
            // do nothing ...
        break;
        case 1:
            load_finish[i].stat = 4;
        break;
        case 4:
            if (rcv_stat.cdvd.fstat[i].stat == 0)
            {
                load_finish[i].stat = 0;
            }
        break;
        }
    }
}

u_char IsExistLoadReq()
{
    return cdvd_rstat.com_num != 0;
}

static int GetFreeId()
{
    int ret;
    
    if (cdvd_rstat.com_num >= 32)
    {
        return -1;
    }
    
    ret = cdvd_rstat.req_pos;
    load_finish[ret].stat = 1;
    cdvd_rstat.req_pos = (cdvd_rstat.req_pos + 1) % 32;
    cdvd_rstat.com_num ++;

    return ret;
}

IMG_ARRANGEMENT *GetImgArrangementP(int file_no)
{
    return (IMG_ARRANGEMENT *)(file_no * 8 + 0x12f0000);
}

#ifdef BUILD_EU_VERSION
int LoadReqLanguage(int file_no, u_int addr)
{
    return LoadReq(file_no + sys_wrk.language, addr);
}
#endif
