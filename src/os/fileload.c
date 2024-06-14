#include "common.h"
#include "typedefs.h"

#include "sce/sifrpc.h"

#include "os/eeiop/cdvd/eecdvd.h"

static u_int sbuff[16];
static sceSifClientData flCd;

int FileLoadInit()
{
    int i;

    while (1) {
		if (sceSifBindRpc(&flCd, 0x12358, 0) < 0) {
            // error ... loop forever!
			while(1);
		}
		i = 10000;
		while( i-- ) {}
		if(flCd.serve != 0) break;
	}
    
    while (sceSifCheckStatRpc(&flCd.rpcd) != 0) {};
    sceSifCallRpc(&flCd,0,0,sbuff,4,sbuff,0x40,NULL,NULL);
    return 1;
}

void FileLoadNReq(int file_no, long int addr)
{
    sbuff[1] = addr;
    sbuff[0] = file_no;
    while (sceSifCheckStatRpc(&flCd.rpcd) != 0) {};
    sceSifCallRpc(&flCd, 1, 0, sbuff, 0x80, sbuff, 0x40, NULL, NULL);
}

int FileLoadNEnd(int file_no)
{
    return 1;
}

void FileLoadB(int file_no, long int addr)
{
    LoadReq(file_no, addr);
}
