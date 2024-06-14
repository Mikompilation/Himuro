#include "common.h"
#include "typedefs.h"

#include "sce/libpc.h"
#include "sce/sifdev.h"

#include "mc/mc.h"
#include "os/backup.h"
#include "graphics/graph3d/sgdma.h"

#define DATASET_ADDR0 ((u_char *)0x00420000)
#define DATA_ADDR     ((u_char *)0x1a90000)
#define DATA_SIZE     (0x200000)

void SaveDataSet(u_char *addr0)
{
    u_char *addr1;
    int i;
    int j;
    
    for (i = 0; i < mc_gamedata_str_num; i++)
    {
        addr1 = mc_gamedata_str[i].addr;
        for (j = 0; j < mc_gamedata_str[i].size; j++)
        {
            *addr0++ = *addr1++;
        }
    }
}

void SavePCFile()
{
	int fd;
	u_char fname[27] = "host0:../bin/data/save.dat";
  
    scePcStop();
    InitialDmaBuffer();
    SaveDataSet(DATASET_ADDR0);
    fd = sceOpen((char *)fname, SCE_CREAT | SCE_WRONLY);
    sceLseek(fd, 0, 0);
    sceWrite(fd, DATASET_ADDR0, DATA_SIZE);
    sceWrite(fd, DATA_ADDR, DATA_SIZE);
    sceClose(fd);
}

void LoadPCFile()
{
	int fd;
	u_char fname[27] = "host0:../bin/data/save.dat";
    
    scePcStop();
    fd = sceOpen((char *)fname, SCE_CREAT | SCE_RDONLY);
    sceLseek(fd, 0, 0);
    sceRead(fd, DATASET_ADDR0, DATA_SIZE);
    sceRead(fd, DATA_ADDR, DATA_SIZE);
    LoadDataDevelop(DATASET_ADDR0);
    sceClose(fd);
}

void LoadDataDevelop(u_char *addr0)
{
	u_char *addr1;
	int i;
	int j;

    for (i = 0; i < mc_gamedata_str_num; i++)
    {
        addr1 = mc_gamedata_str[i].addr;
        for (j = 0; j < mc_gamedata_str[i].size; j++)
        {
            *addr1++ = *addr0++;
        }
    }
}
