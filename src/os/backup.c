#include "common.h"
#include "typedefs.h"
#include "addresses.h"
#include "backup.h"

#include "sce/libpc.h"
#include "sce/sifdev.h"

#include "mc/mc.h"
#include "graphics/graph3d/sgdma.h"

#define DATA_SIZE 0x200000

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

    SaveDataSet((u_char *)MC_WORK_ADDRESS);

    fd = sceOpen((char *)fname, SCE_CREAT | SCE_WRONLY);

    sceLseek(fd, 0, 0);
    sceWrite(fd, (u_char *)MC_WORK_ADDRESS, DATA_SIZE);
    sceWrite(fd, (u_char *)MEM_ALBUM_SRC_ADDRESS_0, DATA_SIZE);
    sceClose(fd);
}

void LoadPCFile()
{
	int fd;
	u_char fname[27] = "host0:../bin/data/save.dat";
    
    scePcStop();

    fd = sceOpen((char *)fname, SCE_CREAT | SCE_RDONLY);

    sceLseek(fd, 0, 0);
    sceRead(fd, (u_char *)MC_WORK_ADDRESS, DATA_SIZE);
    sceRead(fd, (u_char *)MEM_ALBUM_SRC_ADDRESS_0, DATA_SIZE);

    LoadDataDevelop((u_char *)MC_WORK_ADDRESS);

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
